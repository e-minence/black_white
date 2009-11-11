////=============================================================================
/**
 *
 *	@file		pmsiv_menu.c
 *	@brief  メニュー管理モジュール
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

//タッチバー
#include "ui/touchbar.h"

//タスクメニュー
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
 *								定数定義
 */
//=============================================================================
enum
{ 
  // エディットモードの配置
  TASKMENU_WIN_EDIT_H = APP_TASKMENU_PLATE_HEIGHT,
  TASKMENU_WIN_EDIT_W = 9,
  TASKMENU_WIN_EDIT_X = 24,
  TASKMENU_WIN_EDIT_Y = 18,

  // カテゴリ・イニシャルモードの配置
  TASKMENU_WIN_INITIAL_H = APP_TASKMENU_PLATE_HEIGHT,
  TASKMENU_WIN_INITIAL_W = 9,
  TASKMENU_WIN_INITIAL_X = 6,
  TASKMENU_WIN_INITIAL_Y = 21,

  MENU_CLWKICON_BASE_X = 0,
  MENU_CLWKICON_BASE_Y = 8 * 21,
  MENU_CLWKICON_OFS_X = 8 * 3,

};

//--------------------------------------------------------------
///	タスクメニューウィンドウ
//==============================================================
typedef enum {
  // けってい・やめる
  TASKMENU_WIN_EDIT_DECIDE = 0,
  TASKMENU_WIN_EDIT_CANCEL,
  TASKMENU_WIN_EDIT_MAX,

  // えらぶ・けす・やめる
  TASKMENU_WIN_INITIAL_SELECT = 0,
  TASKMENU_WIN_INITIAL_DELETE,
  TASKMENU_WIN_INITIAL_CANCEL,
  TASKMENU_WIN_INITIAL_MAX,

  // ワークの最大数
  TASKMENU_WIN_MAX = TASKMENU_WIN_INITIAL_MAX,

} TASKMENU_WIN;

//--------------------------------------------------------------
///	CLWKボタン定義
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
 *								構造体定義
 */
//=============================================================================
//--------------------------------------------------------------
///	メニュー管理モジュール メインワーク
//==============================================================
struct _PMSIV_MENU {
  // [IN]
  PMS_INPUT_VIEW* vwk;
  const PMS_INPUT_WORK* mwk;
  const PMS_INPUT_DATA* dwk;
  int*                  p_key_mode;
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
 *							プロトタイプ宣言
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
 *								外部公開関数
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  メニュー管理モジュール生成
 *
 *	@param	PMS_INPUT_VIEW* vwk
 *	@param	PMS_INPUT_WORK* mwk
 *	@param	PMS_INPUT_DATA* dwk 
 *
 *	@retval PMSIV_MENU* ワーク
 */
//-----------------------------------------------------------------------------
PMSIV_MENU* PMSIV_MENU_Create( PMS_INPUT_VIEW* vwk, const PMS_INPUT_WORK* mwk, const PMS_INPUT_DATA* dwk )
{
  PMSIV_MENU* wk = GFL_HEAP_AllocClearMemory( HEAPID_PMS_INPUT_VIEW, sizeof(PMSIV_MENU) );

	wk->vwk = vwk;
	wk->mwk = mwk;
	wk->dwk = dwk;

	wk->p_key_mode = PMSI_GetKTModePointer(wk->mwk);
  
  wk->msgman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE,
      NARC_message_pms_input_dat, HEAPID_PMS_INPUT_VIEW );
    
  // リソース展開
  wk->menu_res = APP_TASKMENU_RES_Create( 
        FRM_MAIN_TASKMENU, PALNUM_MAIN_TASKMENU, 
        PMSIView_GetFontHandle(wk->vwk),
        PMSIView_GetPrintQue(wk->vwk),
        HEAPID_PMS_INPUT_VIEW );

  // タッチバー
  wk->touchbar = _touchbar_init( PMSIView_GetCellUnit(wk->vwk), HEAPID_PMS_INPUT_VIEW );

  // CLWK
  _clwk_create( wk );

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  メニュー管理モジュール削除
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_Delete( PMSIV_MENU* wk )
{
  // メニュークリア
  PMSIV_MENU_Clear( wk );

  // タッチバー開放
  TOUCHBAR_Exit( wk->touchbar );

  // タスクメニュー リソース開放
  APP_TASKMENU_RES_Delete( wk->menu_res );

  _clwk_delete( wk );

  GFL_MSG_Delete( wk->msgman );

  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  主処理
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_Main( PMSIV_MENU* wk )
{
  int i;

  // キーモードを反映
//  GFL_UI_SetTouchOrKey( *wk->p_key_mode );

  // タスクメニュー
  for( i=0; i<TASKMENU_WIN_MAX; i++ )
  {
    if( wk->menu_win[i] != NULL )
    {
      APP_TASKMENU_WIN_Update( wk->menu_win[i] );
    }
  }

  // タッチバー
  TOUCHBAR_Main( wk->touchbar );

}

//-----------------------------------------------------------------------------
/**
 *	@brief  全てのタスクメニューを開放
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval none
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

  // 全部消す
  TOUCHBAR_SetVisibleAll( wk->touchbar, FALSE );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  EDITシーン用の配置でセットアップ
 *
 *	@param	PMS_INPUT_VIEW* vwk 
 *
 *	@retval none
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

  // ロックモードでは表示しない
  if( PMSI_GetLockFlag( wk->mwk ) == FALSE )
  {
    // CLWK
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_L ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_MAIL ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_BTL_READY ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_BTL_WIN ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_BTL_LOST ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_UNION ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_R ], TRUE );
  }
  
  PMSIV_MENU_UpdateEditIcon( wk );

  HOSAKA_Printf("PMSIV_MENU_SetupEdit\n");
}

//-----------------------------------------------------------------------------
/**
 *	@brief  カテゴリーモードのセットアップ
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval none
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
 *	@brief  単語リストのセットアップ
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_SetupWordWin( PMSIV_MENU* wk )
{
  PMSIV_MENU_Clear( wk );
  // タッチバーのリターンボタン表示
  TOUCHBAR_SetVisibleAll( wk->touchbar, TRUE );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  エディットモードのアイコンを更新
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval none
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

//-----------------------------------------------------------------------------
/**
 *	@brief  タスクメニューをアクティブに
 *
 *	@param	PMSIV_MENU* wk
 *	@param	pos
 *	@param	is_on 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_TaskMenuSetActive( PMSIV_MENU* wk, u8 pos, BOOL is_on )
{
  int i;

  GF_ASSERT( pos < TASKMENU_WIN_MAX );
  GF_ASSERT( wk->menu_win[pos] != NULL );

  // 一端全てを非アクティブに
  for( i=0; i<TASKMENU_WIN_MAX; i++ )
  {
    if( wk->menu_win[i] != NULL )
    {
      APP_TASKMENU_WIN_SetActive( wk->menu_win[ i ], FALSE );
    }
  }
   
  APP_TASKMENU_WIN_SetActive( wk->menu_win[ pos ], is_on );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  タスクメニュー 決定
 *
 *	@param	PMSIV_MENU* wk
 *	@param	pos
 *	@param	is_on 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_TaskMenuSetDecide( PMSIV_MENU* wk, u8 pos, BOOL is_on )
{ 
  int i;

  GF_ASSERT( pos < TASKMENU_WIN_MAX );
  GF_ASSERT( wk->menu_win[pos] != NULL );
  
  // 一端全てを非アクティブに
  for( i=0; i<TASKMENU_WIN_MAX; i++ )
  {
    if( wk->menu_win[i] != NULL )
    {
      APP_TASKMENU_WIN_SetActive( wk->menu_win[ i ], FALSE );
    }
  }

  APP_TASKMENU_WIN_SetDecide( wk->menu_win[ pos ], is_on );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  タスクメニュー 終了待ち
 *
 *	@param	PMSIV_MENU* wk 
 *	@param  pos
 *
 *	@retval TRUE : 終了
 */
//-----------------------------------------------------------------------------
BOOL PMSIV_MENU_TaskMenuIsFinish( PMSIV_MENU* wk, u8 pos )
{ 
  GF_ASSERT( pos < TASKMENU_WIN_MAX );
  GF_ASSERT( wk->menu_win[pos] != NULL );

  return APP_TASKMENU_WIN_IsFinish( wk->menu_win[ pos ] );
}


//=============================================================================
/**
 *								static関数
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  CLWK 生成
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

  wk->resCell.pltIdx = GFL_CLGRP_PLTT_RegisterEx( p_handle, NARC_pmsi_pms2_obj_main_NCLR, CLSYS_DRAW_MAIN,
      0x20*PALNUM_OBJ_M_MENU, 0, PALLINE_NUM_OBJ_M_MENU, HEAPID_PMS_INPUT_VIEW );

  wk->resCell.ncgIdx = GFL_CLGRP_CGR_Register( p_handle, NARC_pmsi_pms2_obj_main_NCGR, FALSE, CLSYS_DRAW_MAIN, HEAPID_PMS_INPUT_VIEW );
  wk->resCell.anmIdx = GFL_CLGRP_CELLANIM_Register( p_handle, NARC_pmsi_pms2_obj_main_NCER, NARC_pmsi_pms2_obj_main_NANR, HEAPID_PMS_INPUT_VIEW );
	
  GFL_ARC_CloseDataHandle( p_handle );

  for( i=0; i<MENU_CLWKICON_MAX; i++ )
  {
    s16 px, py;

    px = MENU_CLWKICON_BASE_X + MENU_CLWKICON_OFS_X * i;
    py = MENU_CLWKICON_BASE_Y;

    // カテゴリチェンジボタンは例外
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
 *	@brief  CLWK 開放
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _clwk_delete( PMSIV_MENU* wk )
{
  // リソース
  GFL_CLGRP_PLTT_Release( wk->resCell.pltIdx );
	GFL_CLGRP_CGR_Release( wk->resCell.ncgIdx );
	GFL_CLGRP_CELLANIM_Release( wk->resCell.anmIdx );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  アイコンのアニメシーケンスを設定
 *
 *	@param	iconIdx   アニメーションIDX
 *	@param	is_on     TRUE:ONアニメ, FALSE:OFFアニメ
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
 *	@brief  タッチバーの設定
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

  //アイコンの設定
	//数分作る
	TOUCHBAR_SETUP	touchbar_setup = {0};

	TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]	=
	{	
		{	
			TOUCHBAR_ICON_RETURN,
			{	TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
		},
	};

	//設定構造体
	//さきほどの窓情報＋リソース情報をいれる
	touchbar_setup.p_item		= touchbar_icon_tbl;				//上の窓情報
	touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);//いくつ窓があるか
	touchbar_setup.p_unit		= clunit;										//OBJ読み込みのためのCLUNIT
	touchbar_setup.bar_frm	= FRM_MAIN_BAR;					//BG読み込みのためのBG面
	touchbar_setup.bg_plt		= PALNUM_MAIN_TOUCHBAR;			//BGﾊﾟﾚｯﾄ
	touchbar_setup.obj_plt	= PALNUM_OBJ_M_TOUCHBAR;		//OBJﾊﾟﾚｯﾄ
	touchbar_setup.mapping	= APP_COMMON_MAPPING_64K;	//マッピングモード
  touchbar_setup.is_notload_bg = TRUE;

  wk = TOUCHBAR_Init( &touchbar_setup, heap_id );

  // 一端全部消しておく
  TOUCHBAR_SetVisibleAll( wk, FALSE );

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  グループモード
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _setup_category_group( PMSIV_MENU* wk )
{
  // タッチバーのリターンボタン表示
  TOUCHBAR_SetVisibleAll( wk->touchbar, TRUE );

  HOSAKA_Printf(" _setup_category_group \n");
}

//-----------------------------------------------------------------------------
/**
 *	@brief  イニシャルモード
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

