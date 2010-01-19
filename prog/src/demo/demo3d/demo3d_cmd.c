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

//�A�[�J�C�u
#include "arc_def.h"

#include "demo3d_graphic.h"
#include "demo3d_mb.h" 

//�f�[�^
#include "demo3d_data.h"

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

//--------------------------------------------------------------
///	�R�}���h�ɓn���p�����[�^
//==============================================================
typedef struct {
  // [IN]
  DEMO3D_CMD_WORK* mwk;
  // [PRIVATE]
  // ���܂̂Ƃ���Ȃ�
} CMD_UNIT;

//--------------------------------------------------------------
///	�R�}���h���[�N
//==============================================================
struct _DEMO3D_CMD_WORK {
  // [IN]
  HEAPID heap_id;
  DEMO3D_ID demo_id;
  // [PRIVATE]
  BOOL  is_cmd_end;
  int pre_frame; ///< 1sync=1
  int cmd_idx;
  DEMO3D_MBL_WORK* mb;
  GFL_TCBSYS*   tcbsys;
  void*         tcbsys_work;
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static BOOL cmd_setup( DEMO3D_CMD_WORK* mwk, DEMO3D_ID id, u32 now_frame, int* out_idx );
static void cmd_exec( DEMO3D_CMD_WORK* mwk, const DEMO3D_CMD_DATA* data );

static void CMD_SE(CMD_UNIT* unit, int* param);
static void CMD_SE_STOP(CMD_UNIT* unit, int* param);
static void CMD_BRIGHTNESS_REQ(CMD_UNIT* unit, int* param);
static void CMD_MOTIONBL_START(CMD_UNIT* unit, int* param);
static void CMD_MOTIONBL_END(CMD_UNIT* unit, int* param);

// DEMO3D_CMD_TYPE �ƑΉ�
//--------------------------------------------------------------
///	�R�}���h�e�[�u�� (�֐��|�C���^�e�[�u��)
//==============================================================
static void (*c_cmdtbl[ DEMO3D_CMD_TYPE_MAX ])() = 
{ 
  NULL, // null
  CMD_SE,
  CMD_SE_STOP,
  CMD_BRIGHTNESS_REQ,
  CMD_MOTIONBL_START,
  CMD_MOTIONBL_END,
  NULL, // end
};

//-----------------------------------------------------------------------------
/**
 *	@brief  SE�Đ��R�}���h
 *
 *	@param	param[0] SE_Label
 *	@param	param[1] volume : 0�Ȃ疳��
 *	@param	param[2] pan : 0�Ȃ疳��
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void CMD_SE(CMD_UNIT* unit, int* param)
{
  int player_no;


  GFL_SOUND_PlaySE( param[0] );
  
  player_no = GFL_SOUND_GetPlayerNo( param[0] );

  // volume
  if( param[1] != 0 )
  {
    GFL_SOUND_SetVolume( player_no, param[1] );
  }

  // pan
  if( param[2] != 0 )
  {
    GFL_SOUND_SetPan( player_no, 0xFFFF, param[2] );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  SE:�Đ���~
 *
 *	@param	param[0] SE_Label
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void CMD_SE_STOP(CMD_UNIT* unit, int* param)
{
  int player_no;

  player_no = GFL_SOUND_GetPlayerNo( param[0] );

  GFL_SOUND_StopPlayerNo( player_no );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �t���b�V��
 *
 *  @param  param[0]  sync
 *  @param  param[1]  �t�F�[�h�I�����̋P�x
 *  @param  param[2]  �t�F�[�h�J�n���̋P�x
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void CMD_BRIGHTNESS_REQ(CMD_UNIT* unit, int* param)
{
  ChangeBrightnessRequest( param[0], param[1], param[2], (PLANEMASK_BG0|PLANEMASK_BG1|PLANEMASK_BG2|PLANEMASK_BG3|PLANEMASK_OBJ), MASK_MAIN_DISPLAY );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���[�V�����u���[ �J�n
 *
 *	@param	unit
 *	@param	param[0] ���[�V�����u���[�W�� �V�����u�����h����G
 *	@param	param[1] ���[�V�����u���[�W�� ���Ƀo�b�t�@����Ă���G
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void CMD_MOTIONBL_START(CMD_UNIT* unit, int* param)
{
  GF_ASSERT( unit->mwk->mb == NULL );

	unit->mwk->mb = DEMO3D_MotionBlInit( unit->mwk->tcbsys, param[0], param[1] );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���[�V�����u���[ ��~
 *
 *	@param	unit
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void CMD_MOTIONBL_END(CMD_UNIT* unit, int* param)
{
  DEMO3D_MotionBlExit( unit->mwk->mb );
  unit->mwk->mb = NULL;
}

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
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
DEMO3D_CMD_WORK* Demo3D_CMD_Init( DEMO3D_ID demo_id, u32 start_frame, HEAPID heap_id )
{
  DEMO3D_CMD_WORK* wk;

  // ���C�����[�N �A���P�[�V����
  wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof( DEMO3D_CMD_WORK ) );

  // TCB������
  wk->tcbsys_work = GFL_HEAP_AllocClearMemory( heap_id, GFL_TCB_CalcSystemWorkSize(5) );
  wk->tcbsys = GFL_TCB_Init( 5, wk->tcbsys_work );

  // �����o������
  wk->heap_id = heap_id;
  wk->demo_id = demo_id;
  wk->is_cmd_end = cmd_setup( wk, demo_id, start_frame, &wk->cmd_idx );
  wk->pre_frame = -1;

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
  const DEMO3D_CMD_DATA* data;
  int i;
  
  // TCB�폜
  GFL_TCB_Exit( wk->tcbsys );
  GFL_HEAP_FreeMemory( wk->tcbsys_work );
  
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
      cmd_exec( wk, data );
      data++;
    }
  }

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
  const DEMO3D_CMD_DATA* data;

  // �R�}���h�𖖒[�܂łȂ߂���A�����Ȃɂ����Ȃ��B
  if( wk->is_cmd_end )
  {
    return;
  }

  // TCB�又��
  GFL_TCB_Main( wk->tcbsys );
  
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
      cmd_exec( wk, data );
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
 *	@brief  �R�}���h������
 *
 *	@param	DEMO3D_ID id
 *	@param	now_frame 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL cmd_setup( DEMO3D_CMD_WORK* mwk, DEMO3D_ID id, u32 now_frame, int* out_idx )
{
  int i;
  const DEMO3D_CMD_DATA* data;
  
  data = Demo3D_DATA_GetCmdData( id );

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
      cmd_exec( mwk, &data[i] );
    }
    else if( data[i].frame >= now_frame )
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
static void cmd_exec( DEMO3D_CMD_WORK* mwk, const DEMO3D_CMD_DATA* data )
{
  CMD_UNIT unit = {0};

  GF_ASSERT( data->type != DEMO3D_CMD_TYPE_NULL );
  GF_ASSERT( data->type < DEMO3D_CMD_TYPE_END );
      
  OS_TPrintf("call cmd type=%d [%d,%d,%d,%d]\n", data->type,
      data->param[0],
      data->param[1],
      data->param[2],
      data->param[3]
      );

  unit.mwk = mwk;

  c_cmdtbl[ data->type ]( &unit, data->param );
}


