//=============================================================================
/**
 *
 *	@file		intro_cmd.c
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

#include "intro_sys.h"

#include "intro_graphic.h"

//�f�[�^
#include "intro_cmd_data.h"

#include "intro_cmd.h"

#include "sound/pm_sndsys.h" // for SEQ_SE_XXX

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  STORE_NUM = 8,  ///< �������s�R�}���h�̌��E��
};


//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
//--------------------------------------------------------------
///	�R�}���h���[�N
//==============================================================
struct _INTRO_CMD_WORK {
  // [IN]
  HEAPID heap_id;
  // [PRIVATE]
  INTRO_SCENE_ID scene_id;
  const INTRO_CMD_DATA* store[ STORE_NUM ];
  int cmd_idx;
};

// �R�}���h
static BOOL CMD_BG_LOAD( INTRO_CMD_WORK* wk, int* param );
static BOOL CMD_SE( INTRO_CMD_WORK* wk, int* param );
static BOOL CMD_SE_STOP( INTRO_CMD_WORK* wk, int* param);
static BOOL CMD_KEY_WAIT( INTRO_CMD_WORK* wk, int* param );

// INTRO_CMD_TYPE �ƑΉ�
//--------------------------------------------------------------
///	�R�}���h�e�[�u�� (�֐��|�C���^�e�[�u��)
//==============================================================
static BOOL (*c_cmdtbl[ INTRO_CMD_TYPE_MAX ])() = 
{ 
  NULL, // null
  CMD_BG_LOAD,
  CMD_SE,
  CMD_SE_STOP,
  CMD_KEY_WAIT,
  NULL, // end
};

//-----------------------------------------------------------------------------
/**
 *	@brief  �O���t�B�b�N���[�h
 *
 *	@param	INTRO_CMD_WORK* wk
 *	@param	param 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
#include "mictest.naix"
static BOOL CMD_BG_LOAD( INTRO_CMD_WORK* wk, int* param )
{
  //@TODO �Ƃ肠�����}�C�N�e�X�g�̃��\�[�X
  HEAPID heap_id;
	ARCHANDLE	*handle;

  heap_id = wk->heap_id;
	handle	= GFL_ARC_OpenDataHandle( ARCID_MICTEST_GRA, heap_id );

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_mictest_back_bg_down_NCLR, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, 0x20, heap_id );
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_mictest_back_bg_up_NCLR, PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0x20, heap_id );
	
  //	----- ����� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_mictest_back_bg_down_NCGR,
						BG_FRAME_BACK_S, 0, 0, 0, heap_id );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_mictest_back_bg_down_NSCR,
						BG_FRAME_BACK_S, 0, 0, 0, heap_id );	

	//	----- ���� -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_mictest_back_bg_up_NCGR,
						BG_FRAME_BACK_M, 0, 0, 0, heap_id );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_mictest_back_bg_up_NSCR,
						BG_FRAME_BACK_M, 0, 0, 0, heap_id );		

	GFL_ARC_CloseDataHandle( handle );

  return TRUE;
}

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
static BOOL CMD_SE( INTRO_CMD_WORK* wk, int* param )
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

  return TRUE;
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
static BOOL CMD_SE_STOP( INTRO_CMD_WORK* wk, int* param)
{
  int player_no;

  player_no = GFL_SOUND_GetPlayerNo( param[0] );

  GFL_SOUND_StopPlayerNo( player_no );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �L�[�E�F�C�g
 *
 *	@param	INTRO_CMD_WORK* wk
 *	@param	param 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL CMD_KEY_WAIT( INTRO_CMD_WORK* wk, int* param )
{
  if( GFL_UI_TP_GetTrg() || GFL_UI_KEY_GetTrg() )
  {
    return TRUE;
  }

  return FALSE;
}

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static BOOL cmd_store( INTRO_CMD_WORK* wk, const INTRO_CMD_DATA* data );
static BOOL cmd_store_exec( INTRO_CMD_WORK* wk );

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�f���R�}���h�R���g���[���[ ������
 *
 *	@param  start_frame
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
INTRO_CMD_WORK* Intro_CMD_Init( HEAPID heap_id )
{
  INTRO_CMD_WORK* wk;

  // ���C�����[�N �A���P�[�V����
  wk = GFL_HEAP_AllocClearMemory(  heap_id, sizeof( INTRO_CMD_WORK ) );

  // �����o������
  wk->heap_id = heap_id;

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�f���R�}���h�R���g���[���[ �j��
 *
 *	@param	INTRO_CMD_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void Intro_CMD_Exit( INTRO_CMD_WORK* wk )
{

  // �q�[�v�J��
  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  3D�f���R�}���h�R���g���[���[ �又��
 *
 *	@param	INTRO_CMD_WORK* wk 
 *
 *	@retval TRUE:�p�� FALSE:�I��
 */
//-----------------------------------------------------------------------------
BOOL Intro_CMD_Main( INTRO_CMD_WORK* wk )
{
  // �X�g�A���ꂽ�R�}���h�����s
  if( cmd_store_exec( wk ) == FALSE )
  {
    int i;

    // �����R�}���h�����ׂďI�����Ă����玟�̃R�}���h���Ăяo��
    for( i=0; i<STORE_NUM+1; i++ )
    {
      const INTRO_CMD_DATA* data;

      GF_ASSERT( i < STORE_NUM ); ///< �X�g�A���E�`�F�b�N
      
      data = Intro_DATA_GetCmdData( wk->scene_id );
      data += wk->cmd_idx;

      // �R�}���h�I������
      if( data->type == INTRO_CMD_TYPE_END )
      {
        // ���̃V�[��
        wk->scene_id++;
          
        HOSAKA_Printf("next scene_id=%d\n",wk->scene_id);
        
        // �I���`�F�b�N
        if( wk->scene_id >= Intro_DATA_GetSceneMax() )
        {
          // �I��
          return FALSE;
        }
        else
        {
          // �R�}���h�Z�b�g
          wk->cmd_idx = 0;
          data = Intro_DATA_GetCmdData( wk->scene_id );
        }
      }
      
      HOSAKA_Printf("cmd_idx=%d\n",wk->cmd_idx);

      // ���̃R�}���h�������Ă���
      wk->cmd_idx++;

      // �X�g�A
      if( cmd_store( wk, data ) == FALSE )
      {
        // ���̃R�}���h��ǂݍ��܂Ȃ��ꍇ�̓��[�v���甲����
        break;
      }
    }
  }

  return TRUE;
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �R�}���h���X�g�A
 *
 *	@param	INTRO_CMD_DATA* data 
 *
 *	@retval TRUE:�p�����ēǂݍ���
 */
//-----------------------------------------------------------------------------
static BOOL cmd_store( INTRO_CMD_WORK* wk, const INTRO_CMD_DATA* data )
{
  GF_ASSERT( data->type != INTRO_CMD_TYPE_NULL );
  GF_ASSERT( data->type < INTRO_CMD_TYPE_END );

  // �R�}���h���X�g�A
  {
    int i;
    BOOL is_store = FALSE;

    // �󂫂�T��
    for( i=0; i<STORE_NUM; i++ )
    {
      if( wk->store[i] == NULL )
      {
        wk->store[i] = data;

        is_store = TRUE;
          
        OS_TPrintf("store [%d] cmd type=%d [%d,%d,%d,%d]\n", i, data->type,
            data->param[0],
            data->param[1],
            data->param[2],
            data->param[3]
            );
        
        break;
      }
    }

    GF_ASSERT( is_store == TRUE );
  }
  
  return data->read_next;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �X�g�A���ꂽ�R�}���h�����s
 *
 *	@param	INTRO_CMD_WORK* wk 
 *
 *	@retval TRUE:�p�� FALSE:�S�ẴR�}���h���I��
 */
//-----------------------------------------------------------------------------
static BOOL cmd_store_exec( INTRO_CMD_WORK* wk )
{
  int i;
  BOOL is_continue = FALSE;

  for( i=0; i<STORE_NUM; i++ )
  {
    if( wk->store[i] )
    {
      const INTRO_CMD_DATA* data;

      data = wk->store[i];

      if( c_cmdtbl[ data->type ]( wk, data->param ) == FALSE )
      {
        // ��ł����s���̂��̂�����Ύ����[�v�����s
        is_continue = TRUE;
      }
      else
      {
        // �I�������R�}���h������
        wk->store[i] = NULL;
        HOSAKA_Printf("store [%d] is kill \n", i );
      }
    }
  }

  return is_continue;
}

