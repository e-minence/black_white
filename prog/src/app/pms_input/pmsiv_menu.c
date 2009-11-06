////=============================================================================
/**
 *
 *	@file		pmsiv_menu.c
 *	@brief  ���j���[�Ǘ����W���[��
 *	@author	hosaka genya
 *	@data		2009.11.04
 *
 */
//=============================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system\main.h"
#include "system\pms_word.h"
#include "system\pms_data.h"

//�^�b�`�o�[
#include "ui/touchbar.h"

//�^�X�N���j���[
#include "app/app_taskmenu.h"

#include "print\printsys.h"

#include "pms_input_prv.h"
#include "pms_input_view.h"

#include "pms2_obj_main_NANR_LBLDEFS.h"
#include "msg\msg_pms_input_button.h"
#include "msg\msg_pms_input.h"
#include "message.naix"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
enum
{ 
  // �G�f�B�b�g���[�h�̔z�u
  TASKMENU_WIN_EDIT_H = APP_TASKMENU_PLATE_HEIGHT,
  TASKMENU_WIN_EDIT_W = 9,
  TASKMENU_WIN_EDIT_X = 24,
  TASKMENU_WIN_EDIT_Y = 18,

  // �J�e�S���E�C�j�V�������[�h�̔z�u
  TASKMENU_WIN_INITIAL_H = APP_TASKMENU_PLATE_HEIGHT,
  TASKMENU_WIN_INITIAL_W = 9,
  TASKMENU_WIN_INITIAL_X = 6,
  TASKMENU_WIN_INITIAL_Y = 21,

  MENU_CLWKICON_BASE_X = 4,
  MENU_CLWKICON_BASE_Y = 8 * 21,
  MENU_CLWKICON_OFS_X = 8 * 3,

};

//--------------------------------------------------------------
///	�^�X�N���j���[�E�B���h�E
//==============================================================
typedef enum {
  // �����Ă��E��߂�
  TASKMENU_WIN_EDIT_DECIDE = 0,
  TASKMENU_WIN_EDIT_CANCEL,
  TASKMENU_WIN_EDIT_MAX,

  // ����ԁE�����E��߂�
  TASKMENU_WIN_INITIAL_SELECT = 0,
  TASKMENU_WIN_INITIAL_DELETE,
  TASKMENU_WIN_INITIAL_CANCEL,
  TASKMENU_WIN_INITIAL_MAX,

  // ���[�N�̍ő吔
  TASKMENU_WIN_MAX = TASKMENU_WIN_INITIAL_MAX,

} TASKMENU_WIN;

//--------------------------------------------------------------
///	CLWK�{�^����`
//==============================================================
typedef enum {
  MENU_CLWKICON_L,
  MENU_CLWKICON_EDIT_MAIL,
  MENU_CLWKICON_EDIT_BTL_READY,
  MENU_CLWKICON_EDIT_BTL_WIN,
  MENU_CLWKICON_EDIT_BTL_LOST,
  MENU_CLWKICON_EDIT_UNION,
  MENU_CLWKICON_R,
  MENU_CLWKICON_CATEGORY_CHANGE,
  MENU_CLWKICON_MAX,
} MENU_CLWKICON;

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
  PMSIV_CELL_RES          resCell;
  TOUCHBAR_WORK*          touchbar;
	GFL_MSGDATA*            msgman;
  APP_TASKMENU_RES*       menu_res;
  APP_TASKMENU_ITEMWORK   menu_item[ TASKMENU_WIN_MAX ];
  APP_TASKMENU_WIN_WORK*  menu_win[ TASKMENU_WIN_MAX ];
  GFL_CLWK*               clwk_icon[ MENU_CLWKICON_MAX ];
};

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static void _clwk_create( PMSIV_MENU* wk );
static void _clwk_delete( PMSIV_MENU* wk );
static void _clwk_setanm( PMSIV_MENU* wk, MENU_CLWKICON iconIdx, BOOL is_on );
static TOUCHBAR_WORK* _touchbar_init( GFL_CLUNIT* clunit, HEAPID heap_id );
static void _setup_category_group( PMSIV_MENU* wk );
static void _setup_category_initial( PMSIV_MENU* wk );

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  ���j���[�Ǘ����W���[������
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

  // �^�b�`�o�[
  wk->touchbar = _touchbar_init( PMSIView_GetCellUnit(wk->vwk), HEAPID_PMS_INPUT_VIEW );

  // CLWK
  _clwk_create( wk );

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���j���[�Ǘ����W���[���폜
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_Delete( PMSIV_MENU* wk )
{
  // �^�b�`�o�[�J��
  TOUCHBAR_Exit( wk->touchbar );

  // �^�X�N���j���[�J��
  PMSIV_MENU_Clear( wk );

  // �^�X�N���j���[ ���\�[�X�J��
  APP_TASKMENU_RES_Delete( wk->menu_res );

  _clwk_delete( wk );

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
      wk->menu_item[i].str = NULL;
    }
    if( wk->menu_win[i] != NULL )
    {

      APP_TASKMENU_WIN_Delete( wk->menu_win[i] );
      wk->menu_win[i] = NULL;
      HOSAKA_Printf("dell win[%d] \n",i);
    }
  }

  // CLWK
  for( i=0; i<MENU_CLWKICON_MAX; i++ )
  {
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[i], FALSE );
  }
 
  GFL_BG_LoadScreenReq( FRM_MAIN_TASKMENU );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  EDIT�V�[���p�̔z�u�ŃZ�b�g�A�b�v
 *
 *	@param	PMS_INPUT_VIEW* vwk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_SetupEdit( PMSIV_MENU* wk )
{
  int i;
    
  PMSIV_MENU_Clear( wk );
    
  for( i=0; i<TASKMENU_WIN_EDIT_MAX; i++ )
  {
    GF_ASSERT( wk->menu_item[i].str == NULL ); 
    GF_ASSERT( wk->menu_win[i] == NULL );

		wk->menu_item[i].str			= GFL_MSG_CreateString( wk->msgman, str_decide + i );
		wk->menu_item[i].msgColor	= APP_TASKMENU_ITEM_MSGCOLOR;
		wk->menu_item[i].type			= APP_TASKMENU_WIN_TYPE_NORMAL + (i==TASKMENU_WIN_EDIT_CANCEL);

    wk->menu_win[i] = APP_TASKMENU_WIN_Create( wk->menu_res, &wk->menu_item[i], 
        TASKMENU_WIN_EDIT_X,
        TASKMENU_WIN_EDIT_Y + TASKMENU_WIN_EDIT_H * i, 
        TASKMENU_WIN_EDIT_W,
        HEAPID_PMS_INPUT_VIEW );
      
    HOSAKA_Printf("create win[%d] \n",i);
  }

  GFL_BG_LoadScreenReq( FRM_MAIN_TASKMENU );

  // CLWK
  GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_L ], TRUE );
  GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_MAIL ], TRUE );
  GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_BTL_READY ], TRUE );
  GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_BTL_WIN ], TRUE );
  GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_BTL_LOST ], TRUE );
  GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_UNION ], TRUE );
  GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_R ], TRUE );
  
  PMSIV_MENU_UpdateEditIcon( wk );

  HOSAKA_Printf("PMSIV_MENU_SetupEdit\n");
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �J�e�S���[���[�h�̃Z�b�g�A�b�v
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_SetupCategory( PMSIV_MENU* wk )
{
  PMSIV_MENU_Clear( wk );
  
  GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_CATEGORY_CHANGE ], TRUE );
	  
  if( PMSI_GetCategoryMode(wk->mwk) == CATEGORY_MODE_GROUP )
	{
    _setup_category_group( wk );
  }
  else
  {
    _setup_category_initial( wk );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �G�f�B�b�g���[�h�̃A�C�R�����X�V
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_UpdateEditIcon( PMSIV_MENU* wk )
{
  static const u8 iconIdx[PMS_TYPE_MAX] = 
  {
    MENU_CLWKICON_EDIT_MAIL,
    MENU_CLWKICON_EDIT_BTL_READY,
    MENU_CLWKICON_EDIT_BTL_WIN,
    MENU_CLWKICON_EDIT_BTL_LOST,
    MENU_CLWKICON_EDIT_UNION,
  };
  
  int i;
  enum PMS_TYPE type;

  type = PMSI_GetSentenceType( wk->mwk );

  for( i=0; i<PMS_TYPE_MAX; i++ )
  {
    BOOL is_on = ( i == type );

    _clwk_setanm( wk, iconIdx[i], is_on );
  }

}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  CLWK ����
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _clwk_create( PMSIV_MENU* wk )
{

  int i;
  ARCHANDLE* p_handle;

	p_handle = GFL_ARC_OpenDataHandle( ARCID_PMSI_GRAPHIC, HEAPID_PMS_INPUT_VIEW );

  wk->resCell.pltIdx = GFL_CLGRP_PLTT_Register( p_handle, NARC_pmsi_pms2_obj_main_NCLR, CLSYS_DRAW_MAIN,
      0x20*PALNUM_OBJ_M_MENU, HEAPID_PMS_INPUT_VIEW );

  wk->resCell.ncgIdx = GFL_CLGRP_CGR_Register( p_handle, NARC_pmsi_pms2_obj_main_NCGR, FALSE, CLSYS_DRAW_MAIN, HEAPID_PMS_INPUT_VIEW );
  wk->resCell.anmIdx = GFL_CLGRP_CELLANIM_Register( p_handle, NARC_pmsi_pms2_obj_main_NCER, NARC_pmsi_pms2_obj_main_NANR, HEAPID_PMS_INPUT_VIEW );
	
  GFL_ARC_CloseDataHandle( p_handle );

  for( i=0; i<MENU_CLWKICON_MAX; i++ )
  {
    s16 px, py;

    px = MENU_CLWKICON_BASE_X + MENU_CLWKICON_OFS_X * i;
    py = MENU_CLWKICON_BASE_Y;

    // �J�e�S���`�F���W�{�^���͗�O
    if( i == MENU_CLWKICON_CATEGORY_CHANGE )
    {
      px = MENU_CLWKICON_BASE_X;
    }

    wk->clwk_icon[i] = PMSIView_AddActor( wk->vwk, &wk->resCell, 
        px, py, 0, NNS_G2D_VRAM_TYPE_2DMAIN );
  
    _clwk_setanm( wk, i, FALSE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[i], FALSE );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  CLWK �J��
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _clwk_delete( PMSIV_MENU* wk )
{
  // ���\�[�X
  GFL_CLGRP_PLTT_Release( wk->resCell.pltIdx );
	GFL_CLGRP_CGR_Release( wk->resCell.ncgIdx );
	GFL_CLGRP_CELLANIM_Release( wk->resCell.anmIdx );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �A�C�R���̃A�j���V�[�P���X���擾
 *
 *	@param	iconIdx   �A�j���[�V����IDX
 *	@param	is_on     TRUE:ON�A�j��, FALSE:OFF�A�j��
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _clwk_setanm( PMSIV_MENU* wk, MENU_CLWKICON iconIdx, BOOL is_on )
{
  static const u8 anmIdx[ MENU_CLWKICON_MAX ][2] = 
  {
    {
      NANR_pms2_obj_main_hidari, NANR_pms2_obj_main_hidari_tp,
    },
    {
      NANR_pms2_obj_main_mail_off, NANR_pms2_obj_main_mail_on,
    },
    {
      NANR_pms2_obj_main_sentoumae_off, NANR_pms2_obj_main_sentoumae_on,
    },
    {
      NANR_pms2_obj_main_win_off, NANR_pms2_obj_main_win_on,
    },
    {
      NANR_pms2_obj_main_lose_off, NANR_pms2_obj_main_lose_on,
    },
    {
      NANR_pms2_obj_main_yunion_off, NANR_pms2_obj_main_yunion_on,
    },
    {
      NANR_pms2_obj_main_migi, NANR_pms2_obj_main_migi_tp,
    },
    {
      NANR_pms2_obj_main_change_off, NANR_pms2_obj_main_change_on,
    },
  };

  GF_ASSERT( iconIdx < MENU_CLWKICON_MAX );
    
  GFL_CLACT_WK_SetAnmSeq( wk->clwk_icon[ iconIdx ], anmIdx[ iconIdx ][ is_on ] );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �^�b�`�o�[�̐ݒ�
 *
 *	@param	GFL_CLUNIT* unit
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static TOUCHBAR_WORK* _touchbar_init( GFL_CLUNIT* clunit, HEAPID heap_id )
{
  TOUCHBAR_WORK* wk;

  //�A�C�R���̐ݒ�
	//�������
	TOUCHBAR_SETUP	touchbar_setup = {0};

	TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]	=
	{	
		{	
			TOUCHBAR_ICON_RETURN,
			{	TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
		},
	};

	//�ݒ�\����
	//�����قǂ̑����{���\�[�X���������
	touchbar_setup.p_item		= touchbar_icon_tbl;				//��̑����
	touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);//�����������邩
	touchbar_setup.p_unit		= clunit;										//OBJ�ǂݍ��݂̂��߂�CLUNIT
	touchbar_setup.bar_frm	= FRM_MAIN_BAR;					//BG�ǂݍ��݂̂��߂�BG��
	touchbar_setup.bg_plt		= PALNUM_MAIN_TOUCHBAR;			//BG��گ�
	touchbar_setup.obj_plt	= PALNUM_OBJ_M_TOUCHBAR;		//OBJ��گ�
	touchbar_setup.mapping	= APP_COMMON_MAPPING_64K;	//�}�b�s���O���[�h
  touchbar_setup.is_notload_bg = TRUE;

  wk = TOUCHBAR_Init( &touchbar_setup, heap_id );

  // ��[�S�������Ă���
  TOUCHBAR_SetVisibleAll( wk, FALSE );

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �O���[�v���[�h
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _setup_category_group( PMSIV_MENU* wk )
{

  HOSAKA_Printf(" _setup_category_group \n");
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �C�j�V�������[�h
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _setup_category_initial( PMSIV_MENU* wk )
{
  int i;
    
  for( i=0; i<TASKMENU_WIN_INITIAL_MAX; i++ )
  {
    GF_ASSERT( wk->menu_item[i].str == NULL ); 
    GF_ASSERT( wk->menu_win[i] == NULL );

		wk->menu_item[i].str			= GFL_MSG_CreateString( wk->msgman, select01_01 + i );
		wk->menu_item[i].msgColor	= APP_TASKMENU_ITEM_MSGCOLOR;
		wk->menu_item[i].type			= APP_TASKMENU_WIN_TYPE_NORMAL + (i==TASKMENU_WIN_INITIAL_CANCEL);

    wk->menu_win[i] = APP_TASKMENU_WIN_Create( wk->menu_res, &wk->menu_item[i], 
        TASKMENU_WIN_INITIAL_X + TASKMENU_WIN_INITIAL_W * i,
        TASKMENU_WIN_INITIAL_Y, 
        TASKMENU_WIN_INITIAL_W,
        HEAPID_PMS_INPUT_VIEW );
  }

  GFL_BG_LoadScreenReq( FRM_MAIN_TASKMENU );

  HOSAKA_Printf(" _setup_category_initial \n");
}

