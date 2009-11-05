//=============================================================================
/**
 *
 *	@file		pmsiv_menu.c
 *	@brief  ���j���[�Ǘ����W���[��
 *	@author		hosaka genya
 *	@data		2009.11.04
 *
 */
//=============================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system\main.h"
#include "system\pms_word.h"
#include "print\printsys.h"
#include "msg\msg_pms_input_button.h"
#include "msg\msg_pms_input.h"
#include "message.naix"

#include "pms_input_prv.h"
#include "pms_input_view.h"

//�^�X�N���j���[
#include "app/app_taskmenu.h"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  TASKMENU_WIN_EDIT_X = 20,
  TASKMENU_WIN_EDIT_Y = 18,
  TASKMENU_WIN_EDIT_H = 9,
  TASKMENU_WIN_EDIT_W = APP_TASKMENU_PLATE_WIDTH,
};

//--------------------------------------------------------------
///	�^�X�N���j���[�E�B���h�E
//==============================================================
typedef enum {
  // �����Ă��E��߂�
  TASKMENU_WIN_EDIT_DECIDE,
  TASKMENU_WIN_EDIT_CANCEL,
  TASKMENU_WIN_EDIT_MAX,

  // ����ԁE�����E��߂�
  TASKMENU_WIN_INITIAL_SELECT,
  TASKMENU_WIN_INITIAL_DELETE,
  TASKMENU_WIN_INITIAL_CANCEL,
  TASKMENU_WIN_INITIAL_MAX,

  // ���[�N�̍ő吔
  TASKMENU_WIN_MAX = TASKMENU_WIN_INITIAL_MAX,

} TASKMENU_WIN;

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
//--------------------------------------------------------------
///	���j���[�Ǘ����W���[�� ���C�����[�N
//==============================================================
struct _PMSIV_MENU {
  // [IN]
  PMS_INPUT_VIEW* vwk;
  const PMS_INPUT_WORK* mwk;
  const PMS_INPUT_DATA* dwk;
  // [PRIVATE]
	GFL_MSGDATA*            msgman;
  APP_TASKMENU_RES*       menu_res;
  APP_TASKMENU_ITEMWORK   menu_item[ TASKMENU_WIN_MAX ];
  APP_TASKMENU_WIN_WORK*  menu_win[ TASKMENU_WIN_MAX ];
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	PMS_INPUT_VIEW* vwk
 *	@param	PMS_INPUT_WORK* mwk
 *	@param	PMS_INPUT_DATA* dwk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
PMSIV_MENU* PMSIV_MENU_Create( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk )
{
  PMSIV_MENU* wk = GFL_HEAP_AllocClearMemory( HEAPID_PMS_INPUT_VIEW, sizeof(PMSIV_MENU) );

	wk->vwk = vwk;
	wk->mwk = mwk;
	wk->dwk = dwk;
  
  wk->msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
      NARC_message_pms_input_dat, HEAPID_PMS_INPUT_VIEW );
    
  // ���\�[�X�W�J
  wk->menu_res = APP_TASKMENU_RES_Create( 
        FRM_MAIN_TASKMENU, PALNUM_MAIN_TASKMENU, 
        PMSIView_GetFontHandle(wk->vwk),
        PMSIView_GetPrintQue(wk->vwk),
        HEAPID_PMS_INPUT_VIEW );

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_Delete( PMSIV_MENU* wk )
{
  int i;

  // �^�X�N���j���[�J��
  for( i=0; i<TASKMENU_WIN_EDIT_DECIDE; i++ )
  {
    if( wk->menu_item[i].str != NULL )
    {
      GFL_STR_DeleteBuffer( wk->menu_item[i].str );
    }
    if( wk->menu_win[i] )
    {
      APP_TASKMENU_WIN_Delete( wk->menu_win[i] );
    }
  }

  // �^�X�N���j���[ ���\�[�X�J��
  APP_TASKMENU_RES_Delete( wk->menu_res );

  GFL_MSG_Delete( wk->msgman );

  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �S�Ẵ^�X�N���j���[���J��
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_Clear( PMSIV_MENU* wk )
{
  int i;

  for( i=0; i<TASKMENU_WIN_MAX; i++ )
  {
    if( wk->menu_item[i].str != NULL )
    {
      GFL_STR_DeleteBuffer( wk->menu_item[i].str );
    }
    if( wk->menu_win[i] )
    {
      APP_TASKMENU_WIN_Delete( wk->menu_win[i] );
    }
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	PMS_INPUT_VIEW* vwk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_SetupEdit( PMSIV_MENU* wk )
{
  int i;
    
  for( i=0; i<TASKMENU_WIN_EDIT_MAX; i++ )
  {
    if( wk->menu_item[i].str != NULL )
    {
      GFL_STR_DeleteBuffer( wk->menu_item[i].str );
    }

		wk->menu_item[i].str			= GFL_MSG_CreateString( wk->msgman, str_decide + i );
		wk->menu_item[i].msgColor	= APP_TASKMENU_ITEM_MSGCOLOR;
		wk->menu_item[i].type			= APP_TASKMENU_WIN_TYPE_NORMAL+i;

    if( wk->menu_win[i] )
    {
      APP_TASKMENU_WIN_Delete( wk->menu_win[i] );
    }

    wk->menu_win[i] = APP_TASKMENU_WIN_Create( wk->menu_res, &wk->menu_item[i], 
        TASKMENU_WIN_EDIT_X,
        TASKMENU_WIN_EDIT_Y + TASKMENU_WIN_EDIT_H * i, 
        TASKMENU_WIN_EDIT_W,
        HEAPID_PMS_INPUT_VIEW );
  }
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================


