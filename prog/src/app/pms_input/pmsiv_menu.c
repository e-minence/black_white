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


#define DEF_TOUCHBAR_ICON_OWN  // これが定義されているとき、タッチバーTOUCHBAR_WORKを使用せず、自前のtouchbar_icon_を使用する。


/*
パレットアニメーション
素材メモ
pms_obj_main.ncl
2行B列〜F列 プログラムで使用する。3行と4行の間で変化する色。
3行B列〜F列 暗い色。
4行B列〜F列 明るい色。
*/

#include <gflib.h>

#include "arc_def.h"
#include "system\main.h"
#include "system\pms_word.h"
#include "system\pms_data.h"

//タッチバー
#include "ui/touchbar.h"

//タスクメニュー
#include "app/app_taskmenu.h"

#include "print/printsys.h"

#include "pms_input_prv.h"
#include "pms_input_view.h"

#include "pms2_obj_main_NANR_LBLDEFS.h"
#include "msg\msg_pms_input_button.h"
#include "msg\msg_pms_input.h"
#include "message.naix"


#ifdef DEF_TOUCHBAR_ICON_OWN
  #include "app/app_menu_common.h"
#endif


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
  TASKMENU_WIN_EDIT_X = 23,
  TASKMENU_WIN_EDIT_Y = 18,

  // カテゴリ・イニシャルモードの配置
  TASKMENU_WIN_INITIAL_H = APP_TASKMENU_PLATE_HEIGHT,
  TASKMENU_WIN_INITIAL_W = 9,
  TASKMENU_WIN_INITIAL_X = 5,
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

// タスクメニューのプレートのアニメ
// アクティブ  // prog/src/app/app_taskmenu.c参照
#define ACTIVE_TASKMENU_ANIME_S_R (5)
#define ACTIVE_TASKMENU_ANIME_S_G (10)
#define ACTIVE_TASKMENU_ANIME_S_B (13)
#define ACTIVE_TASKMENU_ANIME_E_R (12)
#define ACTIVE_TASKMENU_ANIME_E_G (25)
#define ACTIVE_TASKMENU_ANIME_E_B (30)
/*
// パッシブ 
#define PASSIVE_TASKMENU_ANIME_S_R (0)
#define PASSIVE_TASKMENU_ANIME_S_G (0)
#define PASSIVE_TASKMENU_ANIME_S_B (0)
#define PASSIVE_TASKMENU_ANIME_E_R (0)
#define PASSIVE_TASKMENU_ANIME_E_G (10)
#define PASSIVE_TASKMENU_ANIME_E_B (15)
*/
// パッシブ 
#define PASSIVE_TASKMENU_ANIME_S_R (5)
#define PASSIVE_TASKMENU_ANIME_S_G (10)
#define PASSIVE_TASKMENU_ANIME_S_B (13)
#define PASSIVE_TASKMENU_ANIME_E_R (12)
#define PASSIVE_TASKMENU_ANIME_E_G (25)
#define PASSIVE_TASKMENU_ANIME_E_B (30)

static const GXRgb active_taskmenu_anime_s = GX_RGB( ACTIVE_TASKMENU_ANIME_S_R,
                                                     ACTIVE_TASKMENU_ANIME_S_G,
                                                     ACTIVE_TASKMENU_ANIME_S_B );
static const GXRgb active_taskmenu_anime_e = GX_RGB( ACTIVE_TASKMENU_ANIME_E_R,
                                                     ACTIVE_TASKMENU_ANIME_E_G,
                                                     ACTIVE_TASKMENU_ANIME_E_B );

static const GXRgb passive_taskmenu_anime_s = GX_RGB( PASSIVE_TASKMENU_ANIME_S_R,
                                                      PASSIVE_TASKMENU_ANIME_S_G,
                                                      PASSIVE_TASKMENU_ANIME_S_B );
static const GXRgb passive_taskmenu_anime_e = GX_RGB( PASSIVE_TASKMENU_ANIME_E_R,
                                                      PASSIVE_TASKMENU_ANIME_E_G,
                                                      PASSIVE_TASKMENU_ANIME_E_B );


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

//--------------------------------------------------------------
///	見せるだけCLWK定義(ボタンのようにタッチするものではなく、見せるだけけのもの)
//==============================================================
typedef enum
{
  MARK_L,
  MARK_R,
  MARK_MAX,
}
MARK;

//--------------------------------------------------------------
///	パレットアニメーション
//==============================================================
#define PAL_ANIME_ROW_ORIGINAL  (  6)
#define PAL_ANIME_ROW_START     (  4)
#define PAL_ANIME_ROW_END       (  3)
#define PAL_ANIME_ROW_NOW       (  2)
#define PAL_ANIME_CLM_START     (0xB)
#define PAL_ANIME_CLM_NUM       (  5)
#define PAL_ANIME_ADD           (0x400)

//--------------------------------------------------------------
///	アイコンのアニメ番号
//==============================================================
  static const u8 menuClwkIconAnmIdx[ MENU_CLWKICON_MAX ][2] = 
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


#ifdef DEF_TOUCHBAR_ICON_OWN
//--------------------------------------------------------------
///	自前のtouchbar_icon_の状態
//==============================================================
typedef enum  // TOUCHBAR_SEQ_を参考にした
{
  TI_STATE_MAIN,     // タッチ待ち
  TI_STATE_ANM_TRG,  // ボタンアニメ  //アニメ前の１Fを得るため
  TI_STATE_ANM,      // ボタンアニメ
  TI_STATE_TRG,      // トリガ        //トリガを1フレームだけ伝える
}
TI_STATE;
#endif


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
 
#ifndef DEF_TOUCHBAR_ICON_OWN
  TOUCHBAR_WORK*          touchbar;
#else
  u32                     touchbar_icon_ncl;
  u32                     touchbar_icon_ncg;
  u32                     touchbar_icon_nce;
  GFL_CLWK*               touchbar_icon_clwk;
  TI_STATE                touchbar_icon_state;
#endif
	GFL_MSGDATA*            msgman;
  //APP_TASKMENU_RES*       menu_res;
  APP_TASKMENU_RES*       menu_res_win[ TASKMENU_WIN_MAX ];  // APP_TASKMENU_WIN_WORKごとに異なるAPP_TASKMENU_RESを用意する。
  APP_TASKMENU_ITEMWORK   menu_item[ TASKMENU_WIN_MAX ];
  APP_TASKMENU_WIN_WORK*  menu_win[ TASKMENU_WIN_MAX ];
  GFL_CLWK*               clwk_icon[ MENU_CLWKICON_MAX ];
  GFL_CLWK*               mark_clwk[MARK_MAX];

  // パレットアニメーション
  u16  pal_anime_start[PAL_ANIME_CLM_NUM];
  u16  pal_anime_end[PAL_ANIME_CLM_NUM];
  u16  pal_anime_now[PAL_ANIME_CLM_NUM];
  int  pal_anime_count;
};

//=============================================================================
/**
 *							プロトタイプ宣言
 */
//=============================================================================
static void _clwk_create( PMSIV_MENU* wk );
static void _clwk_delete( PMSIV_MENU* wk );
static void _clwk_setanm( PMSIV_MENU* wk, MENU_CLWKICON iconIdx, BOOL is_on );
static void _mark_create( PMSIV_MENU* wk );  // 必ず_clwk_createの後に呼ぶこと
static void _mark_delete( PMSIV_MENU* wk );  // 必ず_clwk_deleteの前に呼ぶこと
#ifndef DEF_TOUCHBAR_ICON_OWN
static TOUCHBAR_WORK* _touchbar_init( GFL_CLUNIT* clunit, HEAPID heap_id );
#else
static void _touchbar_icon_init( PMSIV_MENU* wk );
#endif
static void _setup_category_group( PMSIV_MENU* wk );
static void _setup_category_initial( PMSIV_MENU* wk );

// パレットアニメーション
static void _pal_anime_create( PMSIV_MENU* wk );
static void _pal_anime_delete( PMSIV_MENU* wk );
static void _pal_anime_main( PMSIV_MENU* wk );
static void _pal_anime_start( PMSIV_MENU* wk, int i );
static void _pal_anime_end( PMSIV_MENU* wk, int i );


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
  //wk->menu_res = APP_TASKMENU_RES_Create( 
  //      FRM_MAIN_TASKMENU, PALNUM_MAIN_TASKMENU, 
  //      PMSIView_GetFontHandle(wk->vwk),
  //      PMSIView_GetPrintQue(wk->vwk),
  //      HEAPID_PMS_INPUT_VIEW );

  // タスクメニューで決定できないボタンの色を変更するために、タスクメニューのリソースをウィンドウの数だけ用意する
  {
    int i;
    for( i=0; i<TASKMENU_WIN_MAX; i++ )
    {
      wk->menu_res_win[i] = APP_TASKMENU_RES_Create( 
        FRM_MAIN_TASKMENU, PALNUM_MAIN_TASKMENU, 
        PMSIView_GetFontHandle(wk->vwk),
        PMSIView_GetPrintQue(wk->vwk),
        HEAPID_PMS_INPUT_VIEW );
    }
  }
  // タスクメニューで決定できないボタンの色を転送する
  {
    // ARCID_APP_MENU_COMMON, NARC_app_menu_common_task_menu_NCLRと同じ構成のパレットを転送する
    // prog/src/app/app_taskmenu.c  APP_TASKMENU_RES_Create  参考
    GFL_ARC_UTIL_TransVramPalette(
        ARCID_PMSI_GRAPHIC,
			  NARC_pmsi_pms_task_menu_passive_NCLR,
        PALTYPE_MAIN_BG,
        PALNUM_MAIN_TASKMENU_PASSIVE*32,
        32*2,
        HEAPID_PMS_INPUT_VIEW );	
  }

#ifndef DEF_TOUCHBAR_ICON_OWN
  // タッチバー
  wk->touchbar = _touchbar_init( PMSIView_GetCellUnit(wk->vwk), HEAPID_PMS_INPUT_VIEW );
#else
  _touchbar_icon_init( wk );
#endif

  // CLWK
  _clwk_create( wk );
  // MARK
  _mark_create( wk );
  // パレットアニメーション
  _pal_anime_create( wk );

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

#ifndef DEF_TOUCHBAR_ICON_OWN
  // タッチバー開放
  TOUCHBAR_Exit( wk->touchbar );
#else
  GFL_CLACT_WK_Remove( wk->touchbar_icon_clwk );
  GFL_CLGRP_PLTT_Release( wk->touchbar_icon_ncl );
  GFL_CLGRP_CGR_Release( wk->touchbar_icon_ncg );
  GFL_CLGRP_CELLANIM_Release( wk->touchbar_icon_nce );
#endif

  // タスクメニュー リソース開放
  //APP_TASKMENU_RES_Delete( wk->menu_res );

  // タスクメニューで決定できないボタンの色を変更するために、ウィンドウの数だけ用意していたリソースを開放する
  {
    int i;
    for( i=0; i<TASKMENU_WIN_MAX; i++ )
    {
      APP_TASKMENU_RES_Delete( wk->menu_res_win[i] );
    }
  }

  _pal_anime_delete( wk );  // パレットアニメーション
  _mark_delete( wk );
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

#ifndef DEF_TOUCHBAR_ICON_OWN
  // タッチバー
  TOUCHBAR_Main( wk->touchbar );
#else
  switch( wk->touchbar_icon_state )
  {
  case TI_STATE_MAIN:
    {
      // 外部から
      // wk->touchbar_icon_state = TI_STATE_ANM_TRG;
      // に変更してもらう
    }
    break;
  case TI_STATE_ANM_TRG:
    {
      wk->touchbar_icon_state = TI_STATE_ANM;
    }
    //break;  // breakなし
  case TI_STATE_ANM:
    {
      if( !GFL_CLACT_WK_CheckAnmActive( wk->touchbar_icon_clwk ) )
      {
        wk->touchbar_icon_state = TI_STATE_TRG;
      }
    }
    break;
  case TI_STATE_TRG:
    {
      wk->touchbar_icon_state = TI_STATE_MAIN;
    }
    break;
  }
#endif

  // パレットアニメーション
  _pal_anime_main( wk );
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

  // MARK
  for( i=0; i<MARK_MAX; i++ )
  {
    GFL_CLACT_WK_SetDrawEnable( wk->mark_clwk[i], FALSE );
  }
 
  GFL_BG_LoadScreenReq( FRM_MAIN_TASKMENU );

#ifndef DEF_TOUCHBAR_ICON_OWN
  // 全部消す
  TOUCHBAR_SetVisibleAll( wk->touchbar, FALSE );
#else
  GFL_CLACT_WK_SetDrawEnable( wk->touchbar_icon_clwk, FALSE );
#endif
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

    wk->menu_win[i] = APP_TASKMENU_WIN_Create( wk->menu_res_win[i], &wk->menu_item[i], 
        TASKMENU_WIN_EDIT_X,
        TASKMENU_WIN_EDIT_Y + TASKMENU_WIN_EDIT_H * i, 
        TASKMENU_WIN_EDIT_W,
        HEAPID_PMS_INPUT_VIEW );
     
    APP_TASKMENU_WIN_SetPaletteAndAnimeColor( wk->menu_win[i], wk->menu_res_win[i],
        PALNUM_MAIN_TASKMENU, active_taskmenu_anime_s, active_taskmenu_anime_e );  // パレットを通常のものにしておく

    HOSAKA_Printf("create win[%d] \n",i);
  }

  PMSIV_MENU_UpdateEditTaskMenu( wk );

  GFL_BG_LoadScreenReq( FRM_MAIN_TASKMENU );

  // ロックモードでは表示しない && 単語１個モード単語２個モードでは表示しない
  if( PMSI_GetLockFlag( wk->mwk ) == FALSE && PMSI_GetInputMode( wk->mwk ) == PMSI_MODE_SENTENCE )
  {
    // CLWK
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_L ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_MAIL ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_BTL_READY ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_BTL_WIN ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_BTL_LOST ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_EDIT_UNION ], TRUE );
    GFL_CLACT_WK_SetDrawEnable( wk->clwk_icon[ MENU_CLWKICON_R ], TRUE );

    // MARK
    for( i=0; i<MARK_MAX; i++ )
    {
      GFL_CLACT_WK_SetDrawEnable( wk->mark_clwk[i], TRUE );
    }
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

#ifndef DEF_TOUCHBAR_ICON_OWN
  // タッチバーのリターンボタン表示
  TOUCHBAR_SetVisibleAll( wk->touchbar, TRUE );
  TOUCHBAR_SetActiveAll( wk->touchbar, TRUE );
#else
  GFL_CLACT_WK_SetDrawEnable( wk->touchbar_icon_clwk, TRUE );
  GFL_CLACT_WK_SetAnmSeq( wk->touchbar_icon_clwk, APP_COMMON_BARICON_RETURN );
  wk->touchbar_icon_state = TI_STATE_MAIN;
#endif
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
  static const u8 iconIdx[PMS_TYPE_USER_MAX] = 
  {
    MENU_CLWKICON_EDIT_MAIL,
    MENU_CLWKICON_EDIT_BTL_READY,
    MENU_CLWKICON_EDIT_BTL_WIN,
    MENU_CLWKICON_EDIT_BTL_LOST,
    MENU_CLWKICON_EDIT_UNION,
  };
  
  int i;
  enum PMS_TYPE type;

  // 選択が変更されたか
  int before = -1;  // -1のとき選択されているものがない
  int after  = -1;

  type = PMSI_GetSentenceType( wk->mwk );

  // 選択が変更されたか
  {
    // 変更前
    for( i=MENU_CLWKICON_EDIT_MAIL; i<=MENU_CLWKICON_EDIT_UNION; i++ )
    {
      u16 anmseq = GFL_CLACT_WK_GetAnmSeq( wk->clwk_icon[i] );
      if( anmseq == menuClwkIconAnmIdx[i][1] )
      {
        before = i;
        break;
      }
    }
    // 変更後
    for( i=0; i<PMS_TYPE_USER_MAX; i++ )
    {
      if( i == type )
      {
        after = iconIdx[i];
        break;
      }
    }
  }

  // 変更前に元に戻す
  if( before >= 0 && before != after )
  {
    _pal_anime_end( wk, before );
  }

  for( i=0; i<PMS_TYPE_USER_MAX; i++ )
  {
    BOOL is_on = ( i == type );

    _clwk_setanm( wk, iconIdx[i], is_on );
  }

  // 変更後にアニメーションにする
  if( after >= 0 && before != after )
  {
    _pal_anime_start( wk, after );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  エディットモードのタスクメニューを更新
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_UpdateEditTaskMenu( PMSIV_MENU* wk )
{
  // TASKMENU_WIN_EDIT_DECIDE
  if( PMSI_CheckInputComplete( wk->mwk ) )
  {
    APP_TASKMENU_WIN_SetPaletteAndAnimeColor(
        wk->menu_win[TASKMENU_WIN_EDIT_DECIDE],
        wk->menu_res_win[TASKMENU_WIN_EDIT_DECIDE],
        PALNUM_MAIN_TASKMENU, active_taskmenu_anime_s, active_taskmenu_anime_e );  // パレットを通常のものにしておく
  }
  else
  {
    APP_TASKMENU_WIN_SetPaletteAndAnimeColor(
        wk->menu_win[TASKMENU_WIN_EDIT_DECIDE],
        wk->menu_res_win[TASKMENU_WIN_EDIT_DECIDE],
        PALNUM_MAIN_TASKMENU_PASSIVE, passive_taskmenu_anime_s, passive_taskmenu_anime_e );  // パレットをパッシブのものにしておく
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  カテゴリモードのタスクメニューを更新
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_UpdateCategoryTaskMenu( PMSIV_MENU* wk )
{
  // TASKMENU_WIN_INITIAL_SELECT
  if( PMSI_GetSearchResultCount( wk->mwk ) > 0 )
  {
    APP_TASKMENU_WIN_SetPaletteAndAnimeColor(
        wk->menu_win[TASKMENU_WIN_INITIAL_SELECT],
        wk->menu_res_win[TASKMENU_WIN_INITIAL_SELECT],
        PALNUM_MAIN_TASKMENU, active_taskmenu_anime_s, active_taskmenu_anime_e );  // パレットを通常のものにしておく
  }
  else
  {
    APP_TASKMENU_WIN_SetPaletteAndAnimeColor(
        wk->menu_win[TASKMENU_WIN_INITIAL_SELECT],
        wk->menu_res_win[TASKMENU_WIN_INITIAL_SELECT],
        PALNUM_MAIN_TASKMENU_PASSIVE, passive_taskmenu_anime_s, passive_taskmenu_anime_e );  // パレットをパッシブのものにしておく
  }

  // TASKMENU_WIN_INITIAL_DELETE
  if( PMSI_GetSearchCharNum( wk->mwk ) > 0 )
  {
    APP_TASKMENU_WIN_SetPaletteAndAnimeColor(
        wk->menu_win[TASKMENU_WIN_INITIAL_DELETE],
        wk->menu_res_win[TASKMENU_WIN_INITIAL_DELETE],
        PALNUM_MAIN_TASKMENU, active_taskmenu_anime_s, active_taskmenu_anime_e );  // パレットを通常のものにしておく
  }
  else
  {
    APP_TASKMENU_WIN_SetPaletteAndAnimeColor(
        wk->menu_win[TASKMENU_WIN_INITIAL_DELETE],
        wk->menu_res_win[TASKMENU_WIN_INITIAL_DELETE],
        PALNUM_MAIN_TASKMENU_PASSIVE, passive_taskmenu_anime_s, passive_taskmenu_anime_e );  // パレットをパッシブのものにしておく
  }
}


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void _decide_category_cancel( PMSIV_MENU* wk )
{
  if( PMSI_GetCategoryMode(wk->mwk) == CATEGORY_MODE_INITIAL )
  {
      PMSIV_MENU_TaskMenuSetDecide( wk, 2, TRUE );
  }
}

static BOOL _wait_category_cancel( PMSIV_MENU* wk )
{ 
  if( PMSI_GetCategoryMode(wk->mwk) == CATEGORY_MODE_INITIAL )
  {
    if( PMSIV_MENU_TaskMenuIsFinish( wk, 2 ) )
    {
      return TRUE;
    }
  }
  else
  {
    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void _decide_category_change( PMSIV_MENU* wk )
{
  _clwk_setanm( wk, MENU_CLWKICON_CATEGORY_CHANGE, TRUE );
}

static BOOL _wait_category_change( PMSIV_MENU* wk )
{
  return GFL_CLACT_WK_CheckAnmActive( wk->clwk_icon[ MENU_CLWKICON_CATEGORY_CHANGE ] ) == FALSE;
}

static void _reset_decide_category_change( PMSIV_MENU* wk )
{
  _clwk_setanm( wk, MENU_CLWKICON_CATEGORY_CHANGE, FALSE );
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void _decide_category_erase( PMSIV_MENU* wk )
{
  if( PMSI_GetCategoryMode(wk->mwk) == CATEGORY_MODE_INITIAL )
  {
      PMSIV_MENU_TaskMenuSetDecide( wk, 1, TRUE );
  }
}

static BOOL _wait_category_erase( PMSIV_MENU* wk )
{
  if( PMSI_GetCategoryMode(wk->mwk) == CATEGORY_MODE_INITIAL )
  {
    if( PMSIV_MENU_TaskMenuIsFinish( wk, 1 ) )
    {
      APP_TASKMENU_WIN_ResetDecide( wk->menu_win[ 1 ] );  // 決定演出を終わらせる
      return TRUE;
    }
  }
  else
  {
     return TRUE;
  }

  return FALSE;

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static void _decide_category_search( PMSIV_MENU* wk )
{
  if( PMSI_GetCategoryMode(wk->mwk) == CATEGORY_MODE_INITIAL )
  {
    PMSIV_MENU_TaskMenuSetDecide( wk, 0, TRUE );
  }
}

static BOOL _wait_category_search( PMSIV_MENU* wk )
{
  if( PMSI_GetCategoryMode(wk->mwk) == CATEGORY_MODE_INITIAL )
  {
    if( PMSIV_MENU_TaskMenuIsFinish( wk, 0 ) )
    {
      return TRUE;
    }
  }
  else
  {
    return TRUE;
  }

  return FALSE;
}


//-----------------------------------------------------------------------------
/**
 *	@brief  カテゴリモードのメニュー決定
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_SetDecideCategory( PMSIV_MENU* wk, CATEGORY_DECIDE_ID id )
{
  GF_ASSERT( wk );

  switch( id )
  {
  case CATEGORY_DECIDE_ID_CANCEL :
    _decide_category_cancel( wk );
    break;

  case CATEGORY_DECIDE_ID_CHANGE :
    _decide_category_change( wk );
    break;

  case CATEGORY_DECIDE_ID_ERASE :
    _decide_category_erase( wk );
    break;

  case CATEGORY_DECIDE_ID_SEARCH :
    _decide_category_search( wk );
    break;

  default : GF_ASSERT(0);
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  エディット画面タッチボタン処理
 *
 *	@param	PMSIV_MENU* wk ワーク
 *	@param	id ボタンID
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_TouchEditButton( PMSIV_MENU* wk, EDIT_BUTTON_ID id )
{
  GF_ASSERT( wk );

  switch( id )
  {
  case EDIT_BUTTON_ID_LEFT :
    _clwk_setanm( wk, MENU_CLWKICON_L, TRUE );
    break;

  case EDIT_BUTTON_ID_RIGHT :
    _clwk_setanm( wk, MENU_CLWKICON_R, TRUE );
    break;

  default : GF_ASSERT(0);
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief  カテゴリモードのメニュー決定演出終了待ち
 *
 *	@param	PMSIV_MENU* wk
 *	@param	id 
 *
 *	@retval TRUE:終了
 */
//-----------------------------------------------------------------------------
BOOL PMSIV_MENU_IsFinishCategory( PMSIV_MENU* wk, CATEGORY_DECIDE_ID id )
{
  GF_ASSERT( wk );

  switch( id )
  {
  case CATEGORY_DECIDE_ID_CANCEL :
    return _wait_category_cancel( wk );

  case CATEGORY_DECIDE_ID_CHANGE :
    return _wait_category_change( wk );
  
  case CATEGORY_DECIDE_ID_ERASE :
    return _wait_category_erase( wk );
  
  case CATEGORY_DECIDE_ID_SEARCH :
    return _wait_category_search( wk );

  default : GF_ASSERT(0);
  }	

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  決定状態にあったカテゴリモードのメニューの見た目を元に戻しておく
 *
 *	@param	PMSIV_MENU* wk 
 *	@param	id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void PMSIV_MENU_ResetDecideCategory( PMSIV_MENU* wk, CATEGORY_DECIDE_ID id )
{
  GF_ASSERT( wk );

  switch( id )
  {
  case CATEGORY_DECIDE_ID_CHANGE :
    _reset_decide_category_change( wk );
    break;

  default : GF_ASSERT(0);
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

  if( wk->menu_win[pos] == NULL )
  {
    GF_ASSERT(0);
    return;
  }

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

  // 決定演出開始
  if( wk->menu_win[pos] != NULL )
  {
    APP_TASKMENU_WIN_SetDecide( wk->menu_win[ pos ], is_on );
  }

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


//-----------------------------------------------------------------------------
/**
 *	@brief  
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval 
 */
//-----------------------------------------------------------------------------
BOOL PMSIV_MENU_TouchbarIconSetTouch( PMSIV_MENU* wk )
{
  // ボタンが反応したらTRUEを返す
#ifndef DEF_TOUCHBAR_ICON_OWN
  return TRUE;
#else
  if( GFL_CLACT_WK_GetDrawEnable( wk->touchbar_icon_clwk ) )
  {
    if( wk->touchbar_icon_state == TI_STATE_MAIN )
    {
      GFL_CLACT_WK_SetAnmSeq( wk->touchbar_icon_clwk, APP_COMMON_BARICON_RETURN_ON );
      wk->touchbar_icon_state = TI_STATE_ANM_TRG;
      return TRUE;
    }
  }
  return FALSE;
#endif
}
BOOL PMSIV_MENU_TouchbarIconGetTrg( PMSIV_MENU* wk )
{
  // ボタンのアニメが終わってから１Fのみ返します
#ifndef DEF_TOUCHBAR_ICON_OWN
  return TRUE;
#else
  if( GFL_CLACT_WK_GetDrawEnable( wk->touchbar_icon_clwk ) )
  {
    if( wk->touchbar_icon_state != TI_STATE_TRG )
      return FALSE;
  }
  return TRUE;
#endif
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

  // パレットIDのみ共通素材
  PMSIView_SetupDefaultActHeader( wk->vwk, &wk->resCell, 0, 0 );

//  wk->resCell.pltIdx = GFL_CLGRP_PLTT_RegisterEx( p_handle, NARC_pmsi_pms2_obj_main_NCLR, CLSYS_DRAW_MAIN,
//      0x20*PALNUM_OBJ_M_MENU, 0, PALLINE_NUM_OBJ_M_MENU, HEAPID_PMS_INPUT_VIEW );
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
  // GFL_CLUNITをGFL_CLACT_UNIT_Deleteで破棄するときに、一緒に破棄されるので敢えてGFL_CLACT_WK_Removeは書かなくてもいいのだが、一応書いておく
  int i;
  for( i=0; i<MENU_CLWKICON_MAX; i++ )
  {
    GFL_CLACT_WK_Remove( wk->clwk_icon[i] );
  }

  // リソース
//  GFL_CLGRP_PLTT_Release( wk->resCell.pltIdx );
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
  GF_ASSERT( iconIdx < MENU_CLWKICON_MAX );
    
  GFL_CLACT_WK_SetAnmSeq( wk->clwk_icon[ iconIdx ], menuClwkIconAnmIdx[ iconIdx ][ is_on ] );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  
 *
 *	@param	
 *	@param	 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _mark_create( PMSIV_MENU* wk )  // 必ず_clwk_createの後に呼ぶこと
{
  static const u8 setting_value[MARK_MAX][3] =
  {
    //  x,   y, anmseq
    {   8,  20, NANR_pms2_obj_main_s_yajirushi01 },
    { 240,  20, NANR_pms2_obj_main_s_yajirushi02 },
  };

  u8 i;
  for( i=0; i<MARK_MAX; i++ )
  {
    wk->mark_clwk[i] = PMSIView_AddActor(
                           wk->vwk,
                           &wk->resCell, 
                           setting_value[i][0], setting_value[i][1],
                           0,
                           NNS_G2D_VRAM_TYPE_2DMAIN );
    GFL_CLACT_WK_SetAnmSeq( wk->mark_clwk[i], setting_value[i][2] );
    GFL_CLACT_WK_SetDrawEnable( wk->mark_clwk[i], FALSE );
  }
}
//-----------------------------------------------------------------------------
/**
 *	@brief  
 *
 *	@param	
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _mark_delete( PMSIV_MENU* wk )  // 必ず_clwk_deleteの前に呼ぶこと
{
  u8 i;
  for( i=0; i<MARK_MAX; i++ )
  {
    GFL_CLACT_WK_Remove( wk->mark_clwk[i] );
  }
}

#ifndef DEF_TOUCHBAR_ICON_OWN
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
#else
static void _touchbar_icon_init( PMSIV_MENU* wk )
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_PMS_INPUT_VIEW );

  // 共通アイコンリソース	
  wk->touchbar_icon_ncl = GFL_CLGRP_PLTT_RegisterEx( p_handle,
      APP_COMMON_GetBarIconPltArcIdx(), CLSYS_DRAW_MAIN, PALNUM_OBJ_M_TOUCHBAR*0x20, 0, TOUCHBAR_OBJ_PLT_NUM, HEAPID_PMS_INPUT_VIEW );	
  wk->touchbar_icon_ncg = GFL_CLGRP_CGR_Register( p_handle,
      APP_COMMON_GetBarIconCharArcIdx(), FALSE, CLSYS_DRAW_MAIN, HEAPID_PMS_INPUT_VIEW );
  wk->touchbar_icon_nce = GFL_CLGRP_CELLANIM_Register( p_handle,
      APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_64K),
      APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_64K), HEAPID_PMS_INPUT_VIEW );

  GFL_ARC_CloseDataHandle( p_handle );

  // CLWK作成
  {
    GFL_CLWK_DATA  cldata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.pos_x   = TOUCHBAR_ICON_X_07;
    cldata.pos_y   = TOUCHBAR_ICON_Y;
    cldata.anmseq  = APP_COMMON_BARICON_RETURN;
    cldata.softpri = 0;
    cldata.bgpri   = FRM_MAIN_BAR;

    wk->touchbar_icon_clwk = GFL_CLACT_WK_Create( PMSIView_GetCellUnit(wk->vwk),
        wk->touchbar_icon_ncg,
        wk->touchbar_icon_ncl, 
        wk->touchbar_icon_nce, 
        &cldata, CLSYS_DEFREND_MAIN, HEAPID_PMS_INPUT_VIEW );
    GFL_CLACT_WK_SetAutoAnmFlag( wk->touchbar_icon_clwk, TRUE );
  }

  // 一端消しておく
  GFL_CLACT_WK_SetDrawEnable( wk->touchbar_icon_clwk, FALSE );

  wk->touchbar_icon_state = TI_STATE_MAIN;
}
#endif

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
#ifndef DEF_TOUCHBAR_ICON_OWN
  // タッチバー表示(リターンボタンのみ)
  TOUCHBAR_SetVisibleAll( wk->touchbar, TRUE );
  TOUCHBAR_SetActiveAll( wk->touchbar, TRUE );
#else
  GFL_CLACT_WK_SetDrawEnable( wk->touchbar_icon_clwk, TRUE );
  GFL_CLACT_WK_SetAnmSeq( wk->touchbar_icon_clwk, APP_COMMON_BARICON_RETURN );
  wk->touchbar_icon_state = TI_STATE_MAIN;
#endif

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

    wk->menu_win[i] = APP_TASKMENU_WIN_Create( wk->menu_res_win[i], &wk->menu_item[i], 
        TASKMENU_WIN_INITIAL_X + TASKMENU_WIN_INITIAL_W * i,
        TASKMENU_WIN_INITIAL_Y, 
        TASKMENU_WIN_INITIAL_W,
        HEAPID_PMS_INPUT_VIEW );
    
    APP_TASKMENU_WIN_SetPaletteAndAnimeColor( wk->menu_win[i], wk->menu_res_win[i],
        PALNUM_MAIN_TASKMENU, active_taskmenu_anime_s, active_taskmenu_anime_e );  // パレットを通常のものにしておく
  }

  PMSIV_MENU_UpdateCategoryTaskMenu( wk );

  GFL_BG_LoadScreenReq( FRM_MAIN_TASKMENU );

  HOSAKA_Printf(" _setup_category_initial \n");
}


//-----------------------------------------------------------------------------
/**
 *	@brief  パレットアニメーション 生成
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _pal_anime_create( PMSIV_MENU* wk )
{
  NNSG2dPaletteData* pal_data;
  void* buf = GFL_ARC_UTIL_LoadPalette( ARCID_PMSI_GRAPHIC, NARC_pmsi_pms_obj_main_NCLR, &pal_data, HEAPID_PMS_INPUT_VIEW );
  u16* raw_data = pal_data->pRawData;
  GFL_STD_MemCopy( &raw_data[ PAL_ANIME_ROW_START*0x10 + PAL_ANIME_CLM_START ], wk->pal_anime_start, PAL_ANIME_CLM_NUM*0x2 );
  GFL_STD_MemCopy( &raw_data[ PAL_ANIME_ROW_END  *0x10 + PAL_ANIME_CLM_START ], wk->pal_anime_end,   PAL_ANIME_CLM_NUM*0x2 );
  GFL_HEAP_FreeMemory( buf );

  GFL_STD_MemCopy( wk->pal_anime_start, wk->pal_anime_now, PAL_ANIME_CLM_NUM*0x2 );
  wk->pal_anime_count = 0;
}
//-----------------------------------------------------------------------------
/**
 *	@brief  パレットアニメーション 破棄
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _pal_anime_delete( PMSIV_MENU* wk )
{
  // 何もしない
}
//-----------------------------------------------------------------------------
/**
 *	@brief  パレットアニメーション ループ
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _pal_anime_main( PMSIV_MENU* wk )
{
  u8 i;
  fx16 cos;

  if( wk->pal_anime_count + PAL_ANIME_ADD >= 0x10000 )
  {
    wk->pal_anime_count = wk->pal_anime_count + PAL_ANIME_ADD - 0x10000;
  }
  else
  {
    wk->pal_anime_count = wk->pal_anime_count + PAL_ANIME_ADD;
  }
  cos = ( FX_CosIdx( wk->pal_anime_count ) + FX16_ONE ) / 2;  // 0<= <=1にしておく

  for( i=0; i<PAL_ANIME_CLM_NUM; i++ )
  {
    u8 s_r = ( wk->pal_anime_start[i] & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    u8 s_g = ( wk->pal_anime_start[i] & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    u8 s_b = ( wk->pal_anime_start[i] & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
    u8 e_r = ( wk->pal_anime_end[i]   & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    u8 e_g = ( wk->pal_anime_end[i]   & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    u8 e_b = ( wk->pal_anime_end[i]   & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

    u8 r = s_r + (((e_r-s_r)*cos)>>FX16_SHIFT);
    u8 g = s_g + (((e_g-s_g)*cos)>>FX16_SHIFT);
    u8 b = s_b + (((e_b-s_b)*cos)>>FX16_SHIFT);

    wk->pal_anime_now[i] = GX_RGB( r, g, b );
  }

  NNS_GfdRegisterNewVramTransferTask(
        NNS_GFD_DST_2D_OBJ_PLTT_MAIN,
        PAL_ANIME_ROW_NOW*0x20 + PAL_ANIME_CLM_START*0x2,
        wk->pal_anime_now,
        PAL_ANIME_CLM_NUM*0x2 );
}
//-----------------------------------------------------------------------------
/**
 *	@brief  パレットアニメーション 選択が変更された直後に呼ぶ開始処理
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _pal_anime_start( PMSIV_MENU* wk, int i )
{
  // アニメーションにする
  //GFL_CLACT_WK_SetPlttOffs( wk->clwk_icon[i], PAL_ANIME_ROW_NOW, CLWK_PLTTOFFS_MODE_OAM_COLOR );
  GFL_CLACT_WK_SetPlttOffs( wk->clwk_icon[i], PAL_ANIME_ROW_NOW, CLWK_PLTTOFFS_MODE_PLTT_TOP );

  GFL_STD_MemCopy( wk->pal_anime_start, wk->pal_anime_now, PAL_ANIME_CLM_NUM*0x2 );
  wk->pal_anime_count = 0;
}
//-----------------------------------------------------------------------------
/**
 *	@brief  パレットアニメーション 選択が変更される直前に呼ぶ終了処理
 *
 *	@param	PMSIV_MENU* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void _pal_anime_end( PMSIV_MENU* wk, int i )
{
  // 元に戻す
  //GFL_CLACT_WK_SetPlttOffs( wk->clwk_icon[i], PAL_ANIME_ROW_ORIGINAL, CLWK_PLTTOFFS_MODE_OAM_COLOR );
  GFL_CLACT_WK_SetPlttOffs( wk->clwk_icon[i], PAL_ANIME_ROW_ORIGINAL, CLWK_PLTTOFFS_MODE_PLTT_TOP );
}

