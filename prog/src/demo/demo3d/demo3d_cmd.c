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

// ������֘A
#include "font/font.naix"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h" //PRINT_QUE

//�A�[�J�C�u
#include "arc_def.h"

#include "demo3d_graphic.h"

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
///	�R�}���h���[�N
//==============================================================
struct _DEMO3D_CMD_WORK {
  // [IN]
  HEAPID heap_id;
  DEMO3D_ID demo_id;
  // [PRIVATE]
  BOOL  is_cmd_end;
  int cmd_idx;
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static BOOL cmd_setup( DEMO3D_ID id, u32 now_frame, int* out_idx );
static void cmd_exec( const DEMO3D_CMD_DATA* data );


static void CMD_SE( int* param );

// DEMO3D_CMD_TYPE �ƑΉ�
//--------------------------------------------------------------
///	�R�}���h�e�[�u�� (�֐��|�C���^�e�[�u��)
//==============================================================
static void (*c_cmdtbl[])() = 
{ 
  NULL, // null
  CMD_SE,
  NULL, // end
};

//-----------------------------------------------------------------------------
/**
 *	@brief  SE�Đ��R�}���h
 *
 *	@param	int* param �R�}���h�p�����[�^�̐擪�|�C���^
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void CMD_SE( int* param )
{
  int player_no;

  player_no = param[0];

  GFL_SOUND_PlaySE( param[0] );

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
  wk = GFL_HEAP_AllocClearMemory(  heap_id, sizeof( DEMO3D_CMD_WORK ) );

  // �����o������
  wk->heap_id = heap_id;
  wk->demo_id = demo_id;
  wk->is_cmd_end = cmd_setup( demo_id, start_frame, &wk->cmd_idx );

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

  data  = Demo3D_DATA_GetCmdData( wk->demo_id );

  // ���o��
  data = &data[ wk->cmd_idx ];
    
  GF_ASSERT( data->frame * FX32_ONE >= now_frame );

  // �w��t���[���܂őҋ@
  if( data->frame * FX32_ONE == now_frame )
  {
    int i;
    for( i=0; i<CMD_ELEM_MAX; i++ )
    {
      // ���s
      cmd_exec( data );
      // ���̃R�}���h��END�Ȃ�t���O�𗧂Ă�
      data++;
      wk->is_cmd_end = ( data->type == DEMO3D_CMD_TYPE_END );
     
      wk->cmd_idx++;

      // �I������ END�t���O�������Ă��邩�A���݂̃t���[���Ŏ��s����R�}���h�������Ȃ��Ă����珈���𔲂���
      if( wk->is_cmd_end == TRUE || data->frame * FX32_ONE != now_frame )
      {
        break;
      }
    }
  }
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
static BOOL cmd_setup( DEMO3D_ID id, u32 now_frame, int* out_idx )
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
    else if( data[i].frame == -1 )
    {
      cmd_exec( &data[i] );
    }
    else if( data[i].frame * FX32_ONE >= now_frame )
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
static void cmd_exec( const DEMO3D_CMD_DATA* data )
{
  GF_ASSERT( data->type != DEMO3D_CMD_TYPE_NULL );
  GF_ASSERT( data->type < DEMO3D_CMD_TYPE_END );
      
  OS_TPrintf("call cmd type=%d [%d,%d,%d,%d]\n", data->type,
      data->param[0],
      data->param[1],
      data->param[2],
      data->param[3]
      );
  
  c_cmdtbl[ data->type ]( data->param );
}
