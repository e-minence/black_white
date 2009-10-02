//=============================================================================
/**
 *
 *	@file		ui_template.c
 *	@brief  新規アプリ用テンプレート
 *	@author	Toru=Nagihashi / genya hosaka
 *	@data		2009.09.29
 *
 */
//=============================================================================
//必ず必要なインクルード
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

//FONT
#include "print/gf_font.h"
#include "font/font.naix"

//PRINT_QUE
#include "print/printsys.h"

#include "print/wordset.h"

//INFOWIN
#include "infowin/infowin.h"

//描画設定
#include "ui_template_graphic.h"

//テクスチャをOAMに転送
#include "ui/nsbtx_to_clwk.h"

//マップモデルのnsbtx
#include "fieldmap/fldmmdl_mdlres.naix"

//タッチバー
#include "ui/touchbar.h"

//ポケパラ、ポケモンNO
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

//ポケアイコン
#include "pokeicon/pokeicon.h"

//MCSS
#include "system/mcss.h"
#include "system/mcss_tool.h"

//タスクメニュー
#include "app/app_taskmenu.h"

//どうぐアイコン
#include "item/item.h"
#include "item_icon.naix"

//アプリ共通素材
#include "app/app_menu_common.h"

//アーカイブ
#include "arc_def.h"

//外部公開
#include "ui_template.h"


//@TODO BG読み込み とりあえずマイクテストのリソース
#include "message.naix"
#include "mictest.naix"	// アーカイブ
#include "msg/msg_mictest.h"  // GMM

//=============================================================================
// 下記defineをコメントアウトすると、機能を取り除けます
//=============================================================================
#define UI_TEMPLATE_INFOWIN
#define UI_TEMPLATE_TOUCHBAR
#define UI_TEMPLATE_MCSS
#define UI_TEMPLATE_TASKMENU
#define UI_TEMPLATE_TYPEICON      // 分類、技アイコン
#define UI_TEMPLATE_ITEM_ICON     // どうぐアイコン
#define UI_TEMPLATE_POKE_ICON     // ポケアイコン
#define UI_TEMPLATE_OAM_MAPMODEL  // マップモデルをOAMで表示

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================
enum
{ 
  UI_TEMPLATE_HEAP_SIZE = 0x20000,  ///< ヒープサイズ
};

//-------------------------------------
///	フレーム
//=====================================
enum
{	
	BG_FRAME_BAR_M	= GFL_BG_FRAME1_M,
	BG_FRAME_BACK_M	= GFL_BG_FRAME2_M,
	BG_FRAME_BACK_S	= GFL_BG_FRAME2_S,
};
//-------------------------------------
///	パレット
//=====================================
enum
{	
	//メインBG
	PLTID_BG_BACK_M				= 0,
	PLTID_BG_TASKMENU_M		= 11,
	PLTID_BG_TOUCHBAR_M		= 13,
	PLTID_BG_INFOWIN_M		= 15,
	//サブBG
	PLTID_BG_BACK_S				=	0,

	//メインOBJ
	PLTID_OBJ_TOUCHBAR_M	= 0, // 3本使用
	PLTID_OBJ_TYPEICON_M	= 3, // 3本使用
  PLTID_OBJ_OAM_MAPMODEL_M = 6, // 1本使用
  PLTID_OBJ_POKEICON_M = 7,     // 3本使用
  PLTID_OBJ_ITEMICON_M = 10,
	//サブOBJ
};

//=============================================================================
/**
 *								構造体定義
 */
//=============================================================================
//--------------------------------------------------------------
///	BG管理ワーク
//==============================================================
typedef struct 
{
	int dummy;
} UI_TEMPLATE_BG_WORK;

//--------------------------------------------------------------
///	CLWK管理ワーク
//==============================================================
typedef struct {
  u32       res_ncg;
  u32       res_ncl;
  u32       res_nce;
  GFL_CLWK* clwk;
} UI_TEMPLATE_CLWK_SET;

//--------------------------------------------------------------
///	圧縮フラグビット
//==============================================================
typedef enum{
  CLWK_RES_COMP_NONE = 0,     ///< 圧縮なし
  CLWK_RES_COMP_NCLR = 0x1,   ///< パレット圧縮
  CLWK_RES_COMP_NCGR = 0x2,   ///< キャラクタ圧縮
} CLWK_RES_COMP;

//--------------------------------------------------------------
///	CLWK初期化パラメータ
//==============================================================
typedef struct {
  CLSYS_DRAW_TYPE draw_type; ///< 描画先タイプ
  CLWK_RES_COMP   comp_flg;  ///< 圧縮フラグ
  u32 arc_id;   ///< アーカイブNO
  u32 pltt_id;  ///< パレットリソースNO
  u32 ncg_id;   ///< キャラクタリソースNO
  u32 cell_id;  ///< セルリソースNO
  u32 anm_id;   ///< セルアニメリソースNO
  u8 pltt_line; ///< パレット転送先NO
  u8 px;        ///< X座標
  u8 py;        ///< Y座標
  u8 padding[1];
} CLWK_SET_PARAM;

//--------------------------------------------------------------
///	メインワーク
//==============================================================
typedef struct 
{
  HEAPID heap_id;

	UI_TEMPLATE_BG_WORK				wk_bg;

	//描画設定
	UI_TEMPLATE_GRAPHIC_WORK	*graphic;

#ifdef UI_TEMPLATE_TOUCHBAR
	//タッチバー
	TOUCHBAR_WORK							*touchbar;
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_MCSS
	//MCSS
	MCSS_SYS_WORK							*mcss_sys;
	MCSS_WORK									*mcss_wk;
#endif //UI_TEMPLATE_MSCC

	//フォント
	GFL_FONT									*font;

	//プリントキュー
	PRINT_QUE									*print_que;
	GFL_MSGDATA								*msg;

#ifdef UI_TEMPLATE_TASKMENU
	//タスクメニュー
	APP_TASKMENU_RES					*menu_res;
	APP_TASKMENU_WORK					*menu;
#endif //UI_TEMPLATE_TASKMENU

#ifdef UI_TEMPLATE_TYPEICON
	//分類、技アイコン
	u32												type_ncg;
	u32												type_ncl;
	u32												type_nce;
	GFL_CLWK									*type_clwk;
#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_ITEM_ICON
  UI_TEMPLATE_CLWK_SET      clset_item_icon;
#endif //UI_TEMPLATE_ITEM_ICON

#ifdef UI_TEMPLATE_POKE_ICON
  UI_TEMPLATE_CLWK_SET      clset_poke_icon;
#endif //UI_TEMPLATE_POKE_ICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
  UI_TEMPLATE_CLWK_SET      clset_oam_mmdl;
#endif //UI_TEMPLATE_OAM_MAPMODEL

#ifdef	UI_TEMPLATE_PRINT_TOOL
	//プリントユーティリティ
	PRINT_UTIL								print_util;
	u32												seq;
#endif	//UI_TEMPLATE_PRINT_TOOL

} UI_TEMPLATE_MAIN_WORK;


//=============================================================================
/**
 *							データ定義
 */
//=============================================================================


//=============================================================================
/**
 *							プロトタイプ宣言
 */
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
static GFL_PROC_RESULT UITemplateProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT UITemplateProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT UITemplateProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
//-------------------------------------
///	汎用処理ユーティリティ
//=====================================
static void UITemplate_BG_LoadResource( UI_TEMPLATE_BG_WORK* wk, HEAPID heap_id );
static void UITemplate_OBJ_CreateCLWK( UI_TEMPLATE_CLWK_SET* wk, CLWK_SET_PARAM* prm, GFL_CLUNIT* unit, HEAPID heap_id );
static void UItemplate_OBJ_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk );
//-------------------------------------
///	INFOWIN
//=====================================
static void UITemplate_INFOWIN_Init( GAMESYS_WORK *gamesys, HEAPID heap_id );
static void UITemplate_INFOWIN_Exit( void );
static void UITemplate_INFOWIN_Main( void );
//-------------------------------------
///	タッチバー
//=====================================
static TOUCHBAR_WORK * UITemplate_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heap_id );
static void UITemplate_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar );
static void UITemplate_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar );
//-------------------------------------
///	MCSS
//=====================================
static MCSS_SYS_WORK * UITemplate_MCSS_Init( u16 wk_max, HEAPID heap_id );
static void UITemplate_MCSS_Exit( MCSS_SYS_WORK * mcss );
static void UITemplate_MCSS_Draw( MCSS_SYS_WORK * mcss );
static MCSS_WORK * UITemplate_MCSS_CreateWkPP( MCSS_SYS_WORK * mcss, POKEMON_PARAM *pp, const VecFx32 *pos );
static void UITemplate_MCSS_DeleteWk( MCSS_SYS_WORK * mcss, MCSS_WORK *wk );
//-------------------------------------
///	リストシステムはい、いいえ
//=====================================
static APP_TASKMENU_WORK * UITemplate_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *p_msg, HEAPID heap_id );
static void UITemplate_TASKMENU_Exit( APP_TASKMENU_WORK *menu );
static void UITemplate_TASKMENU_Main( APP_TASKMENU_WORK *menu );
//-------------------------------------
///	分類、技アイコン
//=====================================
static void UITemplate_TYPEICON_CreateCLWK( UI_TEMPLATE_MAIN_WORK *wk, PokeType type, GFL_CLUNIT *unit, HEAPID heap_id );
static void UITemplate_TYPEICON_DeleteCLWK( UI_TEMPLATE_MAIN_WORK *wk );

#ifdef UI_TEMPLATE_ITEM_ICON
//-------------------------------------
///	どうぐアイコン
//=====================================
static void UITemplate_ITEM_ICON_CreateCLWK( UI_TEMPLATE_CLWK_SET* wk, u16 item_id, GFL_CLUNIT* unit, HEAPID heap_id );
static void UITemplate_ITEM_ICON_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk );
#endif //UI_TEMPLATE_ITEM_ICON

#ifdef UI_TEMPLATE_POKE_ICON
//-------------------------------------
///	ポケアイコン
//=====================================
static void UITemplate_POKE_ICON_CreateCLWK( UI_TEMPLATE_CLWK_SET* wk, u32 mons, u32 form_no, BOOL egg, GFL_CLUNIT* unit, HEAPID heap_id );
static void UITemplate_POKE_ICON_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk );
#endif //UI_TEMPLATE_POKE_ICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
//-------------------------------------
///	OAMでマップモデル表示
//=====================================
static void UITemplate_OAM_MAPMODEL_CreateCLWK( UI_TEMPLATE_CLWK_SET *wk, u16 tex_idx, u8 ptn_ofs, GFL_CLUNIT *unit, HEAPID heap_id );
static void UITemplate_OAM_MAPMODEL_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk );
#endif //UI_TEMPLATE_OAM_MAPMODEL


//=============================================================================
/**
 *								外部公開
 */
//=============================================================================
const GFL_PROC_DATA UITemplateProcData = 
{
	UITemplateProc_Init,
	UITemplateProc_Main,
	UITemplateProc_Exit,
};
//=============================================================================
/**
 *								PROC
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC 初期化処理
 *
 *	@param	GFL_PROC *procプロセスシステム
 *	@param	*seq					シーケンス
 *	@param	*pwk					UI_TEMPLATE_PARAM
 *	@param	*mywk					PROCワーク
 *
 *	@retval	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UITemplateProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
	UI_TEMPLATE_MAIN_WORK *wk;
	UI_TEMPLATE_PARAM *param;

	//オーバーレイ読み込み
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	
	//引数取得
	param	= pwk;

	//ヒープ作成
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_UI_DEBUG, UI_TEMPLATE_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(UI_TEMPLATE_MAIN_WORK), HEAPID_UI_DEBUG );
  GFL_STD_MemClear( wk, sizeof(UI_TEMPLATE_MAIN_WORK) );

  // 初期化
  wk->heap_id = HEAPID_UI_DEBUG;
	
	//描画設定初期化
	wk->graphic	= UI_TEMPLATE_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heap_id );

	//フォント作成
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heap_id );

	//メッセージ
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
			NARC_message_mictest_dat, wk->heap_id );

	//PRINT_QUE作成
	wk->print_que		= PRINTSYS_QUE_Create( wk->heap_id );

	//BGリソース読み込み
	UITemplate_BG_LoadResource( &wk->wk_bg, wk->heap_id );

#ifdef UI_TEMPLATE_INFOWIN
	//INFOWINの初期化
	UITemplate_INFOWIN_Init( param->gamesys, wk->heap_id );
#endif //UI_TEMPLATE_INFOWIN

#ifdef UI_TEMPLATE_TOUCHBAR
	//タッチバーの初期化
	{	
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
		wk->touchbar	= UITemplate_TOUCHBAR_Init( clunit, wk->heap_id );
	}
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_MCSS
	//MCSS初期化&ワーク作成
	wk->mcss_sys	= UITemplate_MCSS_Init( 1, wk->heap_id );
	{
		POKEMON_PARAM	*pp;  
		VecFx32 pos	= { 0, 0, 0	};

		pp	= PP_Create( MONSNO_HUSIGIDANE, 0, 0, wk->heap_id );
		wk->mcss_wk		= UITemplate_MCSS_CreateWkPP( wk->mcss_sys, pp, &pos );
		GFL_HEAP_FreeMemory( pp );
	}
#endif //UI_TEMPLATE_MSCC

#ifdef UI_TEMPLATE_TASKMENU
	//TASKMENUリソース読み込み＆初期化
	wk->menu_res	= APP_TASKMENU_RES_Create( BG_FRAME_BAR_M, PLTID_BG_TASKMENU_M, wk->font, wk->print_que, wk->heap_id );
	wk->menu			= UITemplate_TASKMENU_Init( wk->menu_res, wk->msg, wk->heap_id );
#endif //UI_TEMPLATE_TASKMENU

#ifdef UI_TEMPLATE_TYPEICON
	//属性アイコンの読み込み
	{	
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
		UITemplate_TYPEICON_CreateCLWK( wk, POKETYPE_KUSA, clunit, wk->heap_id );
	}
#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
  //OAMマップモデルの読み込み
  {
    // 主人公は現状 0=後ろ, 1=後ろアニメ1, 2=後ろアニメ2, 3=正面....
    // トレーナーなどは違う可能性があるので注意。
    u16 ptn_ofs = 3;
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );

		UITemplate_OAM_MAPMODEL_CreateCLWK( &wk->clset_oam_mmdl, NARC_fldmmdl_mdlres_hero_nsbtx, ptn_ofs, clunit, wk->heap_id );
  }
#endif //UI_TEMPLATE_OAM_MAPMODEL

#ifdef UI_TEMPLATE_POKE_ICON
  {
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
    UITemplate_POKE_ICON_CreateCLWK( &wk->clset_item_icon, MONSNO_HUSIGIDANE, 0, FALSE, clunit, wk->heap_id );
  }
#endif //UI_TEMPLATE_POKE_ICON

#ifdef UI_TEMPLATE_ITEM_ICON
  // どうぐアイコンの読み込み
  { 
		GFL_CLUNIT	*clunit	= UI_TEMPLATE_GRAPHIC_GetClunit( wk->graphic );
    UITemplate_ITEM_ICON_CreateCLWK( &wk->clset_item_icon, ITEM_KIZUGUSURI, clunit, wk->heap_id );
  }
#endif //UI_TEMPLATE_ITEM_ICON

	//@todo	フェードシーケンスがないので
	GX_SetMasterBrightness(0);
	GXS_SetMasterBrightness(0);

  return GFL_PROC_RES_FINISH;
}
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC 終了処理
 *
 *	@param	GFL_PROC *procプロセスシステム
 *	@param	*seq					シーケンス
 *	@param	*pwk					UI_TEMPLATE_PARAM
 *	@param	*mywk					PROCワーク
 *
 *	@retval	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UITemplateProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	UI_TEMPLATE_MAIN_WORK* wk = mywk;

#ifdef UI_TEMPLATE_TYPEICON
	//属性アイコンの破棄
	UITemplate_TYPEICON_DeleteCLWK( wk );
#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_ITEM_ICON
#endif //UI_TEMPLATE_ITEM_ICON

#ifdef UI_TEMPLATE_ITEM_ICON
  //どうぐアイコンの破棄
  UITemplate_ITEM_ICON_DeleteCLWK( &wk->clset_item_icon );
#endif //UI_TEMPLATE_ITEM_ICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
  //OAMマップモデルの破棄
  UITemplate_OAM_MAPMODEL_DeleteCLWK( &wk->clset_oam_mmdl );
#endif //UI_TEMPLATE_OAM_MAPMODEL

#ifdef UI_TEMPLATE_POKE_ICON
   UITemplate_POKE_ICON_DeleteCLWK( &wk->clset_poke_icon );
#endif //UI_TEMPLATE_POKE_ICON

#ifdef UI_TEMPLATE_TASKMENU
	//TASKMENUシステム＆リソース破棄
	UITemplate_TASKMENU_Exit( wk->menu );
	APP_TASKMENU_RES_Delete( wk->menu_res );	
#endif //UI_TEMPLATE_TASKMENU

#ifdef UI_TEMPLATE_MCSS
	//MSCC破棄
	UITemplate_MCSS_DeleteWk( wk->mcss_sys, wk->mcss_wk );
	UITemplate_MCSS_Exit( wk->mcss_sys );
#endif //UI_TEMPLATE_MCSS

#ifdef UI_TEMPLATE_TOUCHBAR
	//タッチバー
	UITemplate_TOUCHBAR_Exit( wk->touchbar );
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_INFOWIN
	//INFWIN
	UITemplate_INFOWIN_Exit();
#endif //UI_TEMPLATE_INFOWIN

	//メッセージ破棄
	GFL_MSG_Delete( wk->msg );

	//PRINT_QUE
	PRINTSYS_QUE_Delete( wk->print_que );

	//FONT
	GFL_FONT_Delete( wk->font );

	//描画設定破棄
	UI_TEMPLATE_GRAPHIC_Exit( wk->graphic );

	//PROC用メモリ解放
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( wk->heap_id );

	//オーバーレイ破棄
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

  return GFL_PROC_RES_FINISH;
}
//-----------------------------------------------------------------------------
/**
 *	@brief  PROC 主処理
 *
 *	@param	GFL_PROC *procプロセスシステム
 *	@param	*seq					シーケンス
 *	@param	*pwk					UI_TEMPLATE_PARAM
 *	@param	*mywk					PROCワーク
 *
 *	@retval	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT UITemplateProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	UI_TEMPLATE_MAIN_WORK* wk = mywk;

  // デバッグボタンでアプリ終了
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG )
  {
    return GFL_PROC_RES_FINISH;
  }

#ifdef UI_TEMPLATE_TOUCHBAR
	//タッチバーメイン処理
	UITemplate_TOUCHBAR_Main( wk->touchbar );
#endif //UI_TEMPLATE_TOUCHBAR

#ifdef UI_TEMPLATE_INFOWIN
	//INFWINメイン処理
	UITemplate_INFOWIN_Main();
#endif //UI_TEMPLATE_INFOWIN

#ifdef UI_TEMPLATE_TASKMENU
	//タスクメニューメイン処理
	UITemplate_TASKMENU_Main( wk->menu );
#endif //UI_TEMPLATE_TASKMENU

	//PRINT_QUE
	PRINTSYS_QUE_Main( wk->print_que );

	//2D描画
	UI_TEMPLATE_GRAPHIC_2D_Draw( wk->graphic );

	//3D描画
	UI_TEMPLATE_GRAPHIC_3D_StartDraw( wk->graphic );
#ifdef UI_TEMPLATE_MCSS
	UITemplate_MCSS_Draw( wk->mcss_sys );
#endif //UI_TEMPLATE_MCSS
	UI_TEMPLATE_GRAPHIC_3D_EndDraw( wk->graphic );


  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *								static関数
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *	@brief  BG管理モジュール リソース読み込み
 *
 *	@param	UI_TEMPLATE_BG_WORK* wk BG管理ワーク
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_BG_LoadResource( UI_TEMPLATE_BG_WORK* wk, HEAPID heap_id )
{
  //@TODO とりあえずマイクテストのリソース
	ARCHANDLE	*handle;

	handle	= GFL_ARC_OpenDataHandle( ARCID_MICTEST_GRA, heap_id );

	// 上下画面ＢＧパレット転送
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_mictest_back_bg_down_NCLR, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, 0x20, heap_id );
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_mictest_back_bg_up_NCLR, PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0x20, heap_id );
	
  //	----- 下画面 -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_mictest_back_bg_down_NCGR,
						BG_FRAME_BACK_S, 0, 0, 0, heap_id );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_mictest_back_bg_down_NSCR,
						BG_FRAME_BACK_S, 0, 0, 0, heap_id );	

	//	----- 上画面 -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_mictest_back_bg_up_NCGR,
						BG_FRAME_BACK_M, 0, 0, 0, heap_id );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_mictest_back_bg_up_NSCR,
						BG_FRAME_BACK_M, 0, 0, 0, heap_id );		

	GFL_ARC_CloseDataHandle( handle );
}


//-----------------------------------------------------------------------------
/**
 *	@brief  汎用リソース読み込み＆アクター生成処理
 *
 *	@param	UI_TEMPLATE_CLWK_SET* wk
 *	@param	prm
 *	@param	unit
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_OBJ_CreateCLWK( UI_TEMPLATE_CLWK_SET* wk, CLWK_SET_PARAM* prm, GFL_CLUNIT* unit, HEAPID heap_id )
{
  //リソース読み込み
	{	
		ARCHANDLE *p_handle;
    BOOL comp_ncg = ( prm->comp_flg & CLWK_RES_COMP_NCGR );
  	
		p_handle	= GFL_ARC_OpenDataHandle( prm->arc_id, heap_id );

    if( prm->comp_flg & CLWK_RES_COMP_NCLR )
    {
      wk->res_ncl	= GFL_CLGRP_PLTT_RegisterComp( p_handle,
          prm->pltt_id, prm->draw_type, prm->pltt_line*0x20, heap_id );
    }
    else
    {
      wk->res_ncl	= GFL_CLGRP_PLTT_Register( p_handle,
          prm->pltt_id, prm->draw_type, prm->pltt_line*0x20, heap_id );
    }
    
    wk->res_ncg	= GFL_CLGRP_CGR_Register( p_handle,
        prm->ncg_id, comp_ncg, prm->draw_type, heap_id );

		wk->res_nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
        prm->cell_id,	prm->anm_id, heap_id );

		GFL_ARC_CloseDataHandle( p_handle );	
	}

	//CLWK作成
	{	
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.pos_x	= prm->px;
		cldata.pos_y	= prm->py;
		wk->clwk = GFL_CLACT_WK_Create( unit,
				wk->res_ncg,
				wk->res_ncl,
				wk->res_nce,
				&cldata,
				prm->draw_type,
				heap_id );
	}
}

//-----------------------------------------------------------------------------
/**
 *	@brief  汎用リソース読み込み＆アクター削除処理
 *
 *	@param	UI_TEMPLATE_CLWK_SET* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UItemplate_OBJ_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk )
{
	//CLWK破棄
	GFL_CLACT_WK_Remove( wk->clwk );

	//リソース破棄
	GFL_CLGRP_PLTT_Release( wk->res_ncl );
	GFL_CLGRP_CGR_Release( wk->res_ncg );
	GFL_CLGRP_CELLANIM_Release( wk->res_nce );
}




//=============================================================================
/**
 *		INFOWIN
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	INFOWIN初期化
 *
 *	@param	GAMESYS_WORK *gamesys	ゲームシステム
 *	@param	heap_id		ヒープID
 */
//-----------------------------------------------------------------------------
static void UITemplate_INFOWIN_Init( GAMESYS_WORK *gamesys, HEAPID heap_id )
{	
	GAME_COMM_SYS_PTR comm;
	comm	= GAMESYSTEM_GetGameCommSysPtr(gamesys);
	INFOWIN_Init( BG_FRAME_BAR_M, PLTID_BG_INFOWIN_M, comm, heap_id );
}
//----------------------------------------------------------------------------
/**
 *	@brief	INFOWIN破棄
 *
 *	@param	void 
 */
//-----------------------------------------------------------------------------
static void UITemplate_INFOWIN_Exit( void )
{	
	INFOWIN_Exit();
}
//----------------------------------------------------------------------------
/**
 *	@brief	INFWINメイン処理
 *
 *	@param	void 
 */
//-----------------------------------------------------------------------------
static void UITemplate_INFOWIN_Main( void )
{	
	INFOWIN_Update();
}
//=============================================================================
/**
 *	TOUCHBAR
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR初期化
 *
 *	@param	GFL_CLUNIT *clunit	CLUNIT
 *	@param	heap_id							ヒープID
 *
 *	@return	TOUCHBAR_WORK
 */
//-----------------------------------------------------------------------------
static TOUCHBAR_WORK * UITemplate_TOUCHBAR_Init( GFL_CLUNIT *clunit, HEAPID heap_id )
{	
	//アイコンの設定
	//数分作る
	TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]	=
	{	
		{	
			TOUCHBAR_ICON_RETURN,
			{	TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
		},
		{	
			TOUCHBAR_ICON_CLOSE,
			{	TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_Y },
		},
		{	
			TOUCHBAR_ICON_CHECK,
			{	TOUCHBAR_ICON_X_05, TOUCHBAR_ICON_Y_CHECK },
		},
	};

	//設定構造体
	//さきほどの窓情報＋リソース情報をいれる
	TOUCHBAR_SETUP	touchbar_setup;
	touchbar_setup.p_item		= touchbar_icon_tbl;				//上の窓情報
	touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);//いくつ窓があるか
	touchbar_setup.p_unit		= clunit;										//OBJ読み込みのためのCLUNIT
	touchbar_setup.bar_frm	= BG_FRAME_BAR_M;						//BG読み込みのためのBG面
	touchbar_setup.bg_plt		= PLTID_BG_TOUCHBAR_M;			//BGﾊﾟﾚｯﾄ
	touchbar_setup.obj_plt	= PLTID_OBJ_TOUCHBAR_M;			//OBJﾊﾟﾚｯﾄ
	touchbar_setup.mapping	= APP_COMMON_MAPPING_128K;	//マッピングモード

	return TOUCHBAR_Init( &touchbar_setup, heap_id );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR破棄
 *
 *	@param	TOUCHBAR_WORK	*touchbar タッチバー
 */
//-----------------------------------------------------------------------------
static void UITemplate_TOUCHBAR_Exit( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Exit( touchbar );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBARメイン処理
 *
 *	@param	TOUCHBAR_WORK	*touchbar タッチバー
 */
//-----------------------------------------------------------------------------
static void UITemplate_TOUCHBAR_Main( TOUCHBAR_WORK	*touchbar )
{	
	TOUCHBAR_Main( touchbar );
}
//=============================================================================
/**
 *		MCSS
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	MCSS初期化
 *
 *	@param	u16 wk_max		MSCCのワーク作成最大数
 *	@param	heap_id				ヒープID
 *
 *	@return	MCSS_SYS
 */
//-----------------------------------------------------------------------------
static MCSS_SYS_WORK * UITemplate_MCSS_Init( u16 wk_max, HEAPID heap_id )
{	
	MCSS_SYS_WORK *mcss;
	mcss = MCSS_Init( wk_max , heap_id );
	MCSS_SetTextureTransAdrs( mcss, 0 );
	MCSS_SetOrthoMode( mcss );
	return mcss;
}
//----------------------------------------------------------------------------
/**
 *	@brief	MCSS破棄
 *
 *	@param	MCSS_SYS_WORK * mcss	MCSS＿SYS
 */
//-----------------------------------------------------------------------------
static void UITemplate_MCSS_Exit( MCSS_SYS_WORK * mcss )
{	
	MCSS_Exit(mcss);
}
//----------------------------------------------------------------------------
/**
 *	@brief	MCSS描画
 *
 *	@param	MCSS_SYS_WORK * mcss MCSS＿SYS
 */
//-----------------------------------------------------------------------------
static void UITemplate_MCSS_Draw( MCSS_SYS_WORK * mcss )
{	
	MCSS_Main( mcss );
	MCSS_Draw( mcss );
}
//----------------------------------------------------------------------------
/**
 *	@brief	MCSSワーク作成
 *
 *	@param	MCSS_SYS_WORK * mcss	MCSS_SYS
 *	@param	*ppp					ポケモンパーソナルパラメータ
 *	@param	VecFx32 *pos	位置
 *
 *	@return	MCSS_WK
 */
//-----------------------------------------------------------------------------
static MCSS_WORK * UITemplate_MCSS_CreateWkPP( MCSS_SYS_WORK * mcss, POKEMON_PARAM *pp, const VecFx32 *pos )
{	
	MCSS_WORK *poke;
	MCSS_ADD_WORK addWork;

	VecFx32 scale = {FX32_ONE*16,FX32_ONE*16,FX32_ONE};

	MCSS_TOOL_MakeMAWPP( pp , &addWork , MCSS_DIR_FRONT );
	poke = MCSS_Add( mcss, pos->x, pos->y , pos->z, &addWork );
	MCSS_SetScale( poke , &scale );
	return poke;
}
//----------------------------------------------------------------------------
/**
 *	@brief	MCSSワーク破棄
 *
 *	@param	MCSS_SYS_WORK * mcss	MCSS_SYS
 *	@param	*wk										ワーク
 *
 */
//-----------------------------------------------------------------------------
static void UITemplate_MCSS_DeleteWk( MCSS_SYS_WORK * mcss, MCSS_WORK *wk )
{	
	MCSS_SetVanishFlag( wk );
	MCSS_Del(mcss,wk);
}
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENUの初期化
 *
 *	@param	menu_res	リソース
 */
//-----------------------------------------------------------------------------
static APP_TASKMENU_WORK * UITemplate_TASKMENU_Init( APP_TASKMENU_RES *menu_res, GFL_MSGDATA *p_msg, HEAPID heap_id )
{	
	int i;
	APP_TASKMENU_INITWORK	init;
	APP_TASKMENU_ITEMWORK	item[3];
	APP_TASKMENU_WORK			*menu;	

	//窓の設定
	for( i = 0; i < NELEMS(item); i++ )
	{	
		item[i].str	= GFL_MSG_CreateString( p_msg, 0 );	//文字列
		item[i].msgColor	= APP_TASKMENU_ITEM_MSGCOLOR;	//文字色
		item[i].type			= APP_TASKMENU_WIN_TYPE_NORMAL;	//窓の種類
	}

	//初期化
	init.heapId		= heap_id;
	init.itemNum	= NELEMS(item);
	init.itemWork = item;
	init.posType	= ATPT_RIGHT_DOWN;
	init.charPosX	= 32;
	init.charPosY = 21;

	menu	= APP_TASKMENU_OpenMenu( &init, menu_res );

	//文字列解放
	for( i = 0; i < NELEMS(item); i++ )
	{	
		GFL_STR_DeleteBuffer( item[i].str );
	}

	return menu;

}
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENUの破棄
 *
 *	@param	APP_TASKMENU_WORK *menu	ワーク
 */
//-----------------------------------------------------------------------------
static void UITemplate_TASKMENU_Exit( APP_TASKMENU_WORK *menu )
{	
	APP_TASKMENU_CloseMenu( menu );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TASKMENUのメイン処理
 *
 *	@param	APP_TASKMENU_WORK *menu	ワーク
 */
//-----------------------------------------------------------------------------
static void UITemplate_TASKMENU_Main( APP_TASKMENU_WORK *menu )
{	
	APP_TASKMENU_UpdateMenu( menu );
}

#ifdef UI_TEMPLATE_TYPEICON
//=============================================================================
/**
 *	TYPEICON
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	属性、タイプアイコン作成
 *
 *	@param	PokeType					タイプ
 *	@param	GFL_CLUNIT *unit	CLUNIT
 *	@param	heap_id						ヒープID
 *
 *	@return	CLWK
 */
//-----------------------------------------------------------------------------
static void UITemplate_TYPEICON_CreateCLWK( UI_TEMPLATE_MAIN_WORK *wk, PokeType type, GFL_CLUNIT *unit, HEAPID heap_id )
{	
	//リソース読み込み
	{	
		ARCHANDLE *p_handle;

		p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heap_id );

		wk->type_ncl	= GFL_CLGRP_PLTT_Register( p_handle, 
				APP_COMMON_GetPokeTypePltArcIdx(), CLSYS_DRAW_MAIN, PLTID_OBJ_TYPEICON_M*0x20, heap_id );

		wk->type_ncg	= GFL_CLGRP_CGR_Register( p_handle,
				APP_COMMON_GetPokeTypeCharArcIdx(type), FALSE, CLSYS_DRAW_MAIN, heap_id );

		wk->type_nce	= GFL_CLGRP_CELLANIM_Register( p_handle,
				APP_COMMON_GetPokeTypeCellArcIdx(APP_COMMON_MAPPING_128K ),
				APP_COMMON_GetPokeTypeAnimeArcIdx( APP_COMMON_MAPPING_128K ), heap_id );


		GFL_ARC_CloseDataHandle( p_handle );	
	}


	//CLWK作成
	{	
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.pos_x	= 128;
		cldata.pos_y	= 50;
		wk->type_clwk	=		GFL_CLACT_WK_Create( unit,
				wk->type_ncg,
				wk->type_ncl,
				wk->type_nce,
				&cldata,
				CLSYS_DEFREND_MAIN,
				heap_id );
		GFL_CLACT_WK_SetPlttOffs( wk->type_clwk, APP_COMMON_GetPokeTypePltOffset(type),
				CLWK_PLTTOFFS_MODE_PLTT_TOP );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	属性、タイプアイコン破棄
 *
 *	@param	GFL_CLUNIT *unit	CLUNIT
 *	@param	GFL_CLWK	 *clwk	CLWK
 */
//-----------------------------------------------------------------------------
static void UITemplate_TYPEICON_DeleteCLWK( UI_TEMPLATE_MAIN_WORK *wk )
{	
	//CLWK破棄
	GFL_CLACT_WK_Remove( wk->type_clwk );

	//リソース破棄
	GFL_CLGRP_PLTT_Release( wk->type_ncl );
	GFL_CLGRP_CGR_Release( wk->type_ncg );
	GFL_CLGRP_CELLANIM_Release( wk->type_nce );
}

#endif //UI_TEMPLATE_TYPEICON

#ifdef UI_TEMPLATE_ITEM_ICON
//=============================================================================
/**
 *	ITEM_ICON
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  どうぐアイコン生成
 *
 *	@param	UI_TEMPLATE_CLWK_SET* wk
 *	@param	item_id
 *	@param	unit
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_ITEM_ICON_CreateCLWK( UI_TEMPLATE_CLWK_SET* wk, u16 item_id, GFL_CLUNIT* unit, HEAPID heap_id )
{	
  CLWK_SET_PARAM prm;

  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.comp_flg  = CLWK_RES_COMP_NONE;
  prm.arc_id    = ARCID_ITEMICON;
  prm.pltt_id   = ITEM_GetIndex( item_id, ITEM_GET_ICON_PAL );
  prm.ncg_id    = ITEM_GetIndex( item_id, ITEM_GET_ICON_CGX );
  prm.cell_id   = NARC_item_icon_itemicon_NCER;
  prm.anm_id    = NARC_item_icon_itemicon_NANR;
  prm.pltt_line = PLTID_OBJ_ITEMICON_M;
  prm.px        = 16;
  prm.py        = 32;

  UITemplate_OBJ_CreateCLWK( wk, &prm, unit, heap_id );
}

//-----------------------------------------------------------------------------
/**
 *	@brief	どうぐアイコン破棄
 *
 *	@param	UI_TEMPLATE_CLWK_SET* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_ITEM_ICON_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk )
{
  UItemplate_OBJ_DeleteCLWK( wk );
}

#endif //UI_TEMPLATE_ITEM_ICON


#ifdef UI_TEMPLATE_POKE_ICON
//=============================================================================
/**
 *	POKE_ICON
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  ポケアイコン 生成
 *
 *	@param	UI_TEMPLATE_CLWK_SET* wk
 *	@param	unit
 *	@param	heap_id 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE_ICON_CreateCLWK( UI_TEMPLATE_CLWK_SET* wk, u32 mons, u32 form_no, BOOL egg, GFL_CLUNIT* unit, HEAPID heap_id )
{
  CLWK_SET_PARAM prm;

  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.comp_flg  = CLWK_RES_COMP_NCLR;
  prm.arc_id    = ARCID_POKEICON;
  prm.pltt_id   = POKEICON_GetPalArcIndex();
//  prm.ncg_id    = POKEICON_GetCgxArcIndex(ppp);
  prm.ncg_id    = POKEICON_GetCgxArcIndexByMonsNumber( mons, form_no, egg );
  prm.cell_id   = POKEICON_GetCellArcIndex(); 
  prm.anm_id    = POKEICON_GetAnmArcIndex();
  prm.pltt_line = PLTID_OBJ_POKEICON_M;
  prm.px        = 16 + 64;
  prm.py        = 32;
  
  UITemplate_OBJ_CreateCLWK( wk, &prm, unit, heap_id );

  {
    u8 pal_num = POKEICON_GetPalNum( mons, form_no, egg );
    GFL_CLACT_WK_SetPlttOffs( wk->clwk, pal_num, CLWK_PLTTOFFS_MODE_OAM_COLOR );
  }

}

//-----------------------------------------------------------------------------
/**
 *	@brief  ポケアイコン 破棄
 *
 *	@param	UI_TEMPLATE_CLWK_SET* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_POKE_ICON_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk )
{ 
  UItemplate_OBJ_DeleteCLWK( wk );
}

#endif //UI_TEMPLATE_POKE_ICON

#ifdef UI_TEMPLATE_OAM_MAPMODEL
//=============================================================================
/**
 *	OAM_MAPMODEL
 */
//=============================================================================


//-----------------------------------------------------------------------------
/**
 *	@brief  OAMマップモデル 作成 (人物OBJ特化仕様)
 *
 *	@param	UI_TEMPLATE_CLWK_SET *wk
 *	@param	tex_idx
 *	@param  ptn_ofs
 *	@param	sx
 *	@param	sy
 *	@param	GFL_CLUNIT *unit	CLUNIT
 *	@param	heap_id						ヒープID
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_OAM_MAPMODEL_CreateCLWK( UI_TEMPLATE_CLWK_SET *wk, u16 tex_idx, u8 ptn_ofs, GFL_CLUNIT *unit, HEAPID heap_id )
{	
  CLWK_SET_PARAM prm;

  prm.draw_type = CLSYS_DRAW_MAIN;
  prm.comp_flg  = CLWK_RES_COMP_NONE;
  prm.arc_id    = APP_COMMON_GetArcId();
  prm.pltt_id   = APP_COMMON_GetNull4x4PltArcIdx();
  prm.ncg_id    = APP_COMMON_GetNull4x4CharArcIdx();
  prm.cell_id   = APP_COMMON_GetNull4x4CellArcIdx();
  prm.anm_id    = APP_COMMON_GetNull4x4AnimeArcIdx();
  prm.pltt_line = PLTID_OBJ_OAM_MAPMODEL_M;
  prm.px        = 16 + 32;
  prm.py        = 32;

  UITemplate_OBJ_CreateCLWK( wk, &prm, unit, heap_id );

  // テクスチャを転送
  {
    // 人物は基本 4 x 4
    int sx = 4;
    int sy = 4;

    CLWK_TransNSBTX( wk->clwk, ARCID_MMDL_RES, tex_idx, ptn_ofs, sx, sy, 0, prm.draw_type, heap_id );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  OAMマップモデル 破棄
 *
 *	@param	UI_TEMPLATE_CLWK_SET* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void UITemplate_OAM_MAPMODEL_DeleteCLWK( UI_TEMPLATE_CLWK_SET* wk )
{
  UItemplate_OBJ_DeleteCLWK( wk );
}

#endif //UI_TEMPLATE_OAM_MAPMODEL


#ifdef	UI_TEMPLATE_PRINT_TOOL
//=============================================================================
/**
 *	PRINT_TOOL
 */
//=============================================================================
/*
		・「HP ??/??」を表示するサンプルです
		・BMPWINのサイズは 20 x 2 です
		・現在のHP = 618, 最大HP = 999 とします
		・サンプルメインが FALSE を返すと終了です
*/

// サンプルメイン
static BOOL PrintTool_MainFunc( UI_TEMPLATE_MAIN_WORK * wk )
{
	switch( wk->seq ){
	case 0:
		PrintTool_AddBmpWin( wk );			// BMPWIN作成
		PrintTool_PrintHP( wk );				// ＨＰ表示
		PrintTool_ScreenTrans( wk );		// スクリーン転送
		wk->seq = 1;
		break;

	case 1:
		// プリント終了待ち
		if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
			wk->seq = 2;
		}
		break;

	case 2:
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
			PrintTool_DelBmpWin( wk );		// BMPWIN削除
			return FALSE;
		}
	}

	PRINT_UTIL_Trans( &wk->print_util, wk->que );
	return TRUE;
}

// BMPWIN作成
static void PrintTool_AddBmpWin( UI_TEMPLATE_MAIN_WORK * wk )
{
	wk->print_util.win = GFL_BMPWIN_Create(
													GFL_BG_FRAME0_M,					// ＢＧフレーム
													1, 1,											// 表示座標
													20, 2,										// 表示サイズ
													15,												// パレット
													GFL_BMP_CHRAREA_GET_B );	// キャラ取得方向
}

// BMPWIN削除
static void PrintTool_DelBmpWin( UI_TEMPLATE_MAIN_WORK * wk )
{
		GFL_BMPWIN_Delete( wk->print_util.win );
}

// BMPWINスクリーン転送
static void PrintTool_ScreenTrans( UI_TEMPLATE_MAIN_WORK * wk )
{
	GFL_BMPWIN_MakeScreen( wk->print_util.win );
	GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(wk->print_util.win) );
}

// ＨＰ表示
static void PrintTool_PrintHP( UI_TEMPLATE_MAIN_WORK * wk )
{
	STRBUF * strbuf;

	// BMPWIN内の座標(32,0)を基準に右詰めで「ＨＰ」と表示
	PRINTTOOL_Print(
				&wk->print_util,				// プリントユーティル ( BMPWIN )
				wk->que,								// プリントキュー
				32, 0,									// 表示基準座標
				strbuf,									// 文字列（すでに「ＨＰ」が展開されているものとする）
				wk->font,								// フォント
				PRINTTOOL_MODE_RIGHT );	// 右詰

	// BMPWIN内の座標(100,0)を「／」の中心にしてＨＰを表示 ( hp / mhp )
	PRINTTOOL_PrintFraction(
				&wk->print_util,				// プリントユーティル ( BMPWIN )
				wk->que,								// プリントキュー
				wk->font,								// フォント
				100, 0,									// 表示基準座標
				618,										// hp
				999,										// mhp
				wk->heapID );						// ヒープＩＤ
}
#endif	//UI_TEMPLATE_PRINT_TOOL


