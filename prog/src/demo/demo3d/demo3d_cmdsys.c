//=============================================================================
/**
 *
 *	@file		demo3d_cmd.c
 *	@brief  3D�f���R�}���h
 *	@author	hosaka genya
 *	@data		2009.12.09
 *
 */
//=============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/brightness.h"

// ������֘A
#include "font/font.naix"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h" //PRINT_QUE
#include "gamesystem/msgspeed.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "demo3d_cmd.h"

#include "sound/pm_sndsys.h" // for SEQ_SE_XXX

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  CMD_ELEM_MAX = 9999,  ///< �R�}���h���[�v�����p
};

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static void cmd_SystemWorkInit( DEMO3D_CMD_WORK* wk );
static void cmd_SystemWorkRelease( DEMO3D_CMD_WORK* wk );
static BOOL cmd_setup( DEMO3D_CMD_WORK* wk, DEMO3D_ENGINE_WORK* core, int* out_idx );
static void cmd_exec( DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, const DEMO3D_CMD_DATA* data );

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�f���R�}���h�R���g���[���[ ������
 *
 *	@param	DEMO3D_ID demo_id
 *	@param  start_frame
 *	@param	heapID 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
DEMO3D_CMD_WORK* Demo3D_CMD_Init( DEMO3D_ENGINE_WORK* core, HEAPID heapID )
{
  DEMO3D_CMD_WORK* wk;

  // ���C�����[�N �A���P�[�V����
  wk = GFL_HEAP_AllocClearMemory( heapID, sizeof( DEMO3D_CMD_WORK ) );

  // �����o������
  wk->core = core;
  wk->heapID = heapID;
  wk->tmpHeapID = GFL_HEAP_LOWID(heapID);
  wk->demo_id = core->env.demo_id;
  wk->pre_frame = -1;
  
  cmd_SystemWorkInit( wk );

  //�C�j�V�����R�}���h����s
  wk->is_cmd_end = cmd_setup( wk, core, &wk->cmd_idx );

  return wk;
}
//-----------------------------------------------------------------------------
/**
 *	@brief  3D�f���R�}���h�R���g���[���[ �j��
 *
 *	@param	DEMO3D_CMD_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void Demo3D_CMD_Exit( DEMO3D_CMD_WORK* wk )
{
  DEMO3D_ENGINE_WORK* core = wk->core;
  const DEMO3D_CMD_DATA* data;
  int i;
  
  
  data  = Demo3D_DATA_GetEndCmdData( wk->demo_id );

  // �I���R�}���h�����s
  for( i=0; i<CMD_ELEM_MAX; i++ )
  {
    if( data->type == DEMO3D_CMD_TYPE_END )
    {
      break;
    }
    else
    {
      cmd_exec( wk, core, data );
      data++;
    }
  }

  cmd_SystemWorkRelease( wk );

  // �q�[�v�J��
  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�f���R�}���h�R���g���[���[ �又��
 *
 *	@param	DEMO3D_CMD_WORK* wk 
 *	@param  now_frame
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void Demo3D_CMD_Main( DEMO3D_CMD_WORK* wk, fx32 now_frame )
{
  DEMO3D_ENGINE_WORK* core = wk->core;
  const DEMO3D_CMD_DATA* data;

  // TCB�又��
  GFL_TCBL_Main( wk->tcbsys );

  // �R�}���h�𖖒[�܂łȂ߂���A�R�}���h�T�[�`�͂������Ȃ�
  if( wk->is_cmd_end ){
    return;
  }
  data  = Demo3D_DATA_GetCmdData( wk->demo_id );

  // ���o��
  data = &data[ wk->cmd_idx ];

  now_frame = (now_frame >> FX32_SHIFT); ///< ������
    
  GF_ASSERT_MSG( data->frame >= now_frame, "cmd_idx=%d data->frame=%d now_frame=%d ", wk->cmd_idx, data->frame, now_frame );

  // �w��t���[���܂őҋ@ && �t���[�����i�܂Ȃ������ꍇ�̓R�}���h���������Ȃ�
  if( data->frame == now_frame && wk->pre_frame != now_frame )
  {
    int i;
    for( i=0; i<CMD_ELEM_MAX; i++ )
    {
      // ���s
      cmd_exec( wk, core, data );
      // ���̃R�}���h��END�Ȃ�t���O�𗧂Ă�
      data++;
      wk->is_cmd_end = ( data->type == DEMO3D_CMD_TYPE_END );

      //@TODO TCB���c���Ă�����A�T�[�g
     
      wk->cmd_idx++;

      // �I������ END�t���O�������Ă��邩�A���݂̃t���[���Ŏ��s����R�}���h�������Ȃ��Ă����珈���𔲂���
      if( wk->is_cmd_end == TRUE || data->frame != now_frame )
      {
        break;
      }
    }
  }
  
  wk->pre_frame = now_frame;
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�f���R�}���h�R���g���[���[ �V�X�e�����[�N������ 
 *
 *	@param	DEMO3D_CMD_WORK* wk 
 *	@retval
 */
//-----------------------------------------------------------------------------
static void cmd_SystemWorkInit( DEMO3D_CMD_WORK* wk )
{
  // TCB������
  wk->tcbsys = GFL_TCBL_Init( wk->heapID, wk->heapID, 16, 128 );

  wk->printQue = PRINTSYS_QUE_Create( wk->heapID );
	wk->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
		GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�f���R�}���h�R���g���[���[ �V�X�e�����[�N���
 *
 *	@param	DEMO3D_CMD_WORK* wk 
 *	@retval
 */
//-----------------------------------------------------------------------------
static void cmd_SystemWorkRelease( DEMO3D_CMD_WORK* wk )
{
  ///////////////////////////////////////////////////
  //�f�����f���̃R�}���h�����j��

  //���[�V�����u���[�������Ă�����폜
  if( wk->mb != NULL ){
    DEMO3D_MotionBlExit( wk->mb );
  }
  //�X�g���[���������Ă�����폜
  if( wk->printStream != NULL){
    PRINTSYS_PrintStreamDelete( wk->printStream );
    wk->printStream = NULL;
  }
  //�������Ă���S�Ẵ^�X�N���폜
  GFL_TCBL_DeleteAll( wk->tcbsys );

  ///////////////////////////////////////////////////
  //�V�X�e�����[�N�j��
  GFL_FONT_Delete(wk->fontHandle);
  PRINTSYS_QUE_Delete(wk->printQue);

  // TCB�폜
  GFL_TCBL_Exit( wk->tcbsys );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �R�}���h������
 *
 *	@param	DEMO3D_ID id
 *	@param	now_frame 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL cmd_setup( DEMO3D_CMD_WORK* wk, DEMO3D_ENGINE_WORK* core, int* out_idx )
{
  int i;
  const DEMO3D_CMD_DATA* data;
  
  data = Demo3D_DATA_GetCmdData( core->env.demo_id );

  for( i=0; i<CMD_ELEM_MAX; i++ )
  {
    GF_ASSERT( data[i].type != DEMO3D_CMD_TYPE_NULL );

    // �R�}���h�I�����o
    if( data[i].type == DEMO3D_CMD_TYPE_END )
    {
      return TRUE;
    }
    //  �������p�����[�^�����s
    else if( data[i].frame == DEMO3D_CMD_SYNC_INIT )
    {
      cmd_exec( wk, core, &data[i] );
    }
    else if( data[i].frame >= core->env.start_frame )
    {
      // ���o���I��
      *out_idx = i;
      HOSAKA_Printf("setup cmd_idx=%d \n", i);
      return FALSE;
    }
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �R�}���h���s
 *
 *	@param	DEMO3D_CMD_DATA* data 
 *
 *	@retval 
 */
//-----------------------------------------------------------------------------
static void cmd_exec( DEMO3D_CMD_WORK* wk,DEMO3D_ENGINE_WORK* core, const DEMO3D_CMD_DATA* data )
{
  GF_ASSERT( data->type != DEMO3D_CMD_TYPE_NULL );
  GF_ASSERT( data->type < DEMO3D_CMD_TYPE_END );
      
  OS_TPrintf("call cmd type=%d [%d,%d,%d,%d]\n", data->type,
      data->param[0],
      data->param[1],
      data->param[2],
      data->param[3]
      );

  DATA_Demo3D_CmdTable[ data->type ]( wk, core, data->param );
}


