//=============================================================================
/**
 *
 *	@file		comm_btl_demo.c
 *	@brief  通信バトルデモ
 *	@author	hosaka genya
 *	@data		2009.09.29
 *
 */
//=============================================================================
//必ず必要なインクルード
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

// 簡易会話表示システム
#include "system/pms_draw.h"

//FONT
#include "print/gf_font.h"
#include "font/font.naix"

//PRINT_QUE
#include "print/printsys.h"

#include "print/wordset.h"

//簡易CLWK読み込み＆開放ユーティリティー
#include "ui/ui_easy_clwk.h"

//シーンコントローラー
#include "ui/ui_scene.h"

//描画設定
#include "comm_btl_demo_graphic.h"

//アプリ共通素材
#include "app/app_menu_common.h"

//アーカイブ
#include "arc_def.h"

//外部公開
#include "demo/comm_btl_demo.h"

#include "message.naix"
#include "comm_btl_demo.naix"	// アーカイブ

#include "msg/msg_mictest.h"  // GMM

#include "comm_btl_demo_res.cdat"

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================
enum
{ 
  //@TODO デカめにとってある
  COMM_BTL_DEMO_HEAP_SIZE = 0x90000,  ///< ヒープサイズ
};

//-------------------------------------
///	フレーム
//=====================================
enum
{	
	BG_FRAME_BAR_M	= GFL_BG_FRAME1_M,
	BG_FRAME_POKE_M	= GFL_BG_FRAME2_M,
	BG_FRAME_BACK_M	= GFL_BG_FRAME3_M,
	BG_FRAME_BACK_S	= GFL_BG_FRAME2_S,
  BG_FRAME_TEXT_S = GFL_BG_FRAME0_S, 
};

//-------------------------------------
///	パレット
//=====================================
enum
{	
	//メインBG
	PLTID_BG_BACK_M				= 0,
	PLTID_BG_POKE_M				= 1,
	PLTID_BG_TASKMENU_M		= 11,
	PLTID_BG_TOUCHBAR_M		= 13,
	PLTID_BG_INFOWIN_M		= 15,
	//サブBG
	PLTID_BG_BACK_S				=	0,
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
} COMM_BTL_DEMO_BG_WORK;

//--------------------------------------------------------------
///	G3D管理ワーク
//==============================================================
typedef struct {
  GFL_G3D_UTIL*   g3d_util;
  u16             unit_idx;
  u8              padding[2];
} COMM_BTL_DEMO_G3D_WORK;

//--------------------------------------------------------------
///	メインワーク
//==============================================================
typedef struct 
{
  HEAPID heapID;

	COMM_BTL_DEMO_BG_WORK				wk_bg;
  COMM_BTL_DEMO_G3D_WORK      wk_g3d;

	//描画設定
	COMM_BTL_DEMO_GRAPHIC_WORK	*graphic;

	//フォント
	GFL_FONT						*font;

	//プリントキュー
	PRINT_QUE						*print_que;
	GFL_MSGDATA					*msg;
  
  // シーンコントローラ
  UI_SCENE_CNT_PTR  cntScene;

  int timer;  ///< デモ起動時間カウンタ

} COMM_BTL_DEMO_MAIN_WORK;


//=============================================================================
/**
 *							シーン定義
 */
//=============================================================================

// ノーマル開始
static BOOL SceneNormalStart_Init( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneNormalStart_Main( UI_SCENE_CNT_PTR cnt, void* work );
static BOOL SceneNormalStart_End( UI_SCENE_CNT_PTR cnt, void* work );

// ノーマル終了
static BOOL SceneNormalEnd_Main( UI_SCENE_CNT_PTR cnt, void* work );

// マルチ開始
static BOOL SceneMultiStart_Main( UI_SCENE_CNT_PTR cnt, void* work );

// マルチ終了
static BOOL SceneMultiEnd_Main( UI_SCENE_CNT_PTR cnt, void* work );


//--------------------------------------------------------------
///	SceneID
//==============================================================
typedef enum
{ 
  CBD_SCENE_ID_NORMAL_START = 0,  ///< ノーマル開始
  CBD_SCENE_ID_NORMAL_END,         ///< ノーマル終了
  CBD_SCENE_ID_MULTI_START,       ///< マルチ開始
  CBD_SCENE_ID_MULTI_END,         ///< マルチ終了

  CBD_SCENE_ID_MAX,
} CBD_SCENE_ID;

//--------------------------------------------------------------
///	SceneFunc
//==============================================================
static const UI_SCENE_FUNC_SET c_scene_func_tbl[ CBD_SCENE_ID_MAX ] = 
{
  // CBD_SCENE_ID_NORMAL_START
  {
    SceneNormalStart_Init,
    NULL,
    SceneNormalStart_Main,
    NULL,
    SceneNormalStart_End,
  },
  // CBD_SCENE_ID_NORMAL_END
  {
    NULL,
    NULL,
    SceneNormalEnd_Main,
    NULL,
    NULL,
  },
  // CBD_SCENE_ID_MULTI_START
  {
    NULL,
    NULL,
    SceneMultiStart_Main,
    NULL,
    NULL,
  },
  // CBD_SCENE_ID_MULTI_END
  {
    NULL,
    NULL,
    SceneMultiEnd_Main,
    NULL,
    NULL,
  },
};

//-----------------------------------------------------------------------------
/**
 *	@brief  最初のシーンを判定
 *
 *	@param	COMM_BTL_DEMO_MAIN_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static CBD_SCENE_ID calc_first_scene( COMM_BTL_DEMO_MAIN_WORK* wk )
{
  return CBD_SCENE_ID_NORMAL_START;
}

//=============================================================================
/**
 *							プロトタイプ宣言
 */
//=============================================================================
static void G3D_Init( COMM_BTL_DEMO_G3D_WORK* g3d, HEAPID heapID );
static void G3D_Draw( COMM_BTL_DEMO_G3D_WORK* g3d );
static void G3D_End( COMM_BTL_DEMO_G3D_WORK* g3d );

  //-------------------------------------
///	PROC
//=====================================
static GFL_PROC_RESULT CommBtlDemoProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT CommBtlDemoProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT CommBtlDemoProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );

//-------------------------------------
///	汎用処理ユーティリティ
//=====================================
static void CommBtlDemo_BG_LoadResource( COMM_BTL_DEMO_BG_WORK* wk, HEAPID heapID );

//=============================================================================
/**
 *								外部公開
 */
//=============================================================================
const GFL_PROC_DATA CommBtlDemoProcData = 
{
	CommBtlDemoProc_Init,
	CommBtlDemoProc_Main,
	CommBtlDemoProc_Exit,
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
 *	@param	*pwk					COMM_BTL_DEMO_PARAM
 *	@param	*mywk					PROCワーク
 *
 *	@retval	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CommBtlDemoProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
	COMM_BTL_DEMO_MAIN_WORK *wk;
	COMM_BTL_DEMO_PARAM *param;

	//オーバーレイ読み込み
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	
	//引数取得
	param	= pwk;

	//ヒープ作成
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_UI_DEBUG, COMM_BTL_DEMO_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(COMM_BTL_DEMO_MAIN_WORK), HEAPID_UI_DEBUG );
  GFL_STD_MemClear( wk, sizeof(COMM_BTL_DEMO_MAIN_WORK) );

  // 初期化
  wk->heapID = HEAPID_UI_DEBUG;
	
	//描画設定初期化
	wk->graphic	= COMM_BTL_DEMO_GRAPHIC_Init( GX_DISP_SELECT_SUB_MAIN, wk->heapID );

	//フォント作成
	wk->font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

	//メッセージ
	wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
			NARC_message_mictest_dat, wk->heapID );

	//PRINT_QUE作成
	wk->print_que		= PRINTSYS_QUE_Create( wk->heapID );
  

  // シーンコントローラ作成
  wk->cntScene = UI_SCENE_CNT_Create( 
      wk->heapID, c_scene_func_tbl, CBD_SCENE_ID_MAX, 
      calc_first_scene(wk), wk );

	//BGリソース読み込み
	CommBtlDemo_BG_LoadResource( &wk->wk_bg, wk->heapID );

  G3D_Init( &wk->wk_g3d, wk->heapID );
  
  // フェードイン リクエスト
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 2 );

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  PROC 終了処理
 *
 *	@param	GFL_PROC *procプロセスシステム
 *	@param	*seq					シーケンス
 *	@param	*pwk					COMM_BTL_DEMO_PARAM
 *	@param	*mywk					PROCワーク
 *
 *	@retval	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CommBtlDemoProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	COMM_BTL_DEMO_MAIN_WORK* wk = mywk;
  
  // フェード中は処理しない
  if( GFL_FADE_CheckFade() == TRUE )
  {
    return GFL_PROC_RES_CONTINUE;
  }
  
  // シーンコントーラ削除
  UI_SCENE_CNT_Delete( wk->cntScene );

	//メッセージ破棄
	GFL_MSG_Delete( wk->msg );

	//PRINT_QUE
	PRINTSYS_QUE_Delete( wk->print_que );

	//FONT
	GFL_FONT_Delete( wk->font );

  // 3Dシステム開放
  G3D_End( &wk->wk_g3d );

	//描画設定破棄
	COMM_BTL_DEMO_GRAPHIC_Exit( wk->graphic );

	//PROC用メモリ解放
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( wk->heapID );

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
 *	@param	*pwk					COMM_BTL_DEMO_PARAM
 *	@param	*mywk					PROCワーク
 *
 *	@retval	終了コード
 */
//-----------------------------------------------------------------------------
#include "ui/debug_code/g3d/camera_test.c"
static GFL_PROC_RESULT CommBtlDemoProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
	COMM_BTL_DEMO_MAIN_WORK* wk = mywk;

  // デバッグカメラ
  {
    GFL_G3D_CAMERA* p_camera = COMM_BTL_DEMO_GRAPHIC_GetCamera( wk->graphic );
    debug_camera_test( p_camera );
  }
  
  // フェード中は処理しない
  if( GFL_FADE_CheckFade() == TRUE )
  {
    return GFL_PROC_RES_CONTINUE;
  }
	
  // SCENE
  if( UI_SCENE_CNT_Main( wk->cntScene ) )
  {
    // フェードアウト リクエスト
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 2 );
    return GFL_PROC_RES_FINISH;
  }

	//PRINT_QUE
	PRINTSYS_QUE_Main( wk->print_que );

	//2D描画
	COMM_BTL_DEMO_GRAPHIC_2D_Draw( wk->graphic );
  
	//3D描画
	COMM_BTL_DEMO_GRAPHIC_3D_StartDraw( wk->graphic );

  G3D_Draw( &wk->wk_g3d );

	COMM_BTL_DEMO_GRAPHIC_3D_EndDraw( wk->graphic );

  wk->timer++;

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
 *	@param	COMM_BTL_DEMO_BG_WORK* wk BG管理ワーク
 *	@param	heapID  ヒープID 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void CommBtlDemo_BG_LoadResource( COMM_BTL_DEMO_BG_WORK* wk, HEAPID heapID )
{
	ARCHANDLE	*handle;

	handle	= GFL_ARC_OpenDataHandle( ARCID_COMM_BTL_DEMO_GRA, heapID );

	// 上下画面ＢＧパレット転送
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_comm_btl_demo_bg_base_u_NCLR, PALTYPE_MAIN_BG, PLTID_BG_BACK_M, 0x0, heapID );
	GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_comm_btl_demo_bg_base_d_NCLR, PALTYPE_SUB_BG, PLTID_BG_BACK_S, 0x0, heapID );
	
  //	----- 下画面 -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_comm_btl_demo_bg_base_d_NCGR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_comm_btl_demo_bg_base_d_NSCR,
						BG_FRAME_BACK_S, 0, 0, 0, heapID );	

	//	----- 上画面 -----
	GFL_ARCHDL_UTIL_TransVramBgCharacter(	handle, NARC_comm_btl_demo_bg_base_u_NCGR,
						BG_FRAME_BACK_M, 0, 0, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen(	handle, NARC_comm_btl_demo_bg_base_u_NSCR,
						BG_FRAME_BACK_M, 0, 0, 0, heapID );		

	GFL_ARC_CloseDataHandle( handle );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  通常バトル前デモ 初期化処理
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneNormalStart_Init( UI_SCENE_CNT_PTR cnt, void* work )
{
  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  通常バトル前デモ 主処理
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneNormalStart_Main( UI_SCENE_CNT_PTR cnt, void* work )
{ 
  COMM_BTL_DEMO_MAIN_WORK* wk = work;

// @TODO 保坂のみデバッグボタンで終了
#ifdef DEBUG_ONLY_FOR_genya_hosaka
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG )
#else
  // @TODO 5秒で終了
  if( wk->timer > 60 * 5 )
#endif
  {
    return TRUE;
  }

  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  通常バトル前デモ 終了処理
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneNormalStart_End( UI_SCENE_CNT_PTR cnt, void* work )
{ 
  // 終了
  UI_SCENE_CNT_SetNextScene( cnt, UI_SCENE_ID_END );

  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  通常バトル後デモ 主処理
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneNormalEnd_Main( UI_SCENE_CNT_PTR cnt, void* work )
{
  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  マルチバトル前デモ 主処理
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneMultiStart_Main( UI_SCENE_CNT_PTR cnt, void* work )
{
  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  マルチバトル後デモ 主処理
 *
 *	@param	UI_SCENE_CNT_PTR cnt
 *	@param	work 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static BOOL SceneMultiEnd_Main( UI_SCENE_CNT_PTR cnt, void* work )
{
  return TRUE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  初期化
 *
 *	@param	COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void G3D_Init( COMM_BTL_DEMO_G3D_WORK* g3d, HEAPID heapID )
{

  // 3D管理ユーティリティーの生成
  g3d->g3d_util = GFL_G3D_UTIL_Create( 10, 16, heapID );
  
  // ユニット生成
  //@TODO 汎用化
  g3d->unit_idx = GFL_G3D_UTIL_AddUnit( g3d->g3d_util, &sc_setup[ 0 ] );


  HOSAKA_Printf("add unit idx=%d\n", g3d->unit_idx);
}

//-----------------------------------------------------------------------------
/**
 *	@brief  描画処理
 *
 *	@param	COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void G3D_Draw( COMM_BTL_DEMO_G3D_WORK* g3d )
{ 
  GFL_G3D_OBJSTATUS status;

  GF_ASSERT( g3d );
  GF_ASSERT( g3d->g3d_util );

  // ステータス初期化
  VEC_Set( &status.trans, 0, 0, 0 );
  VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &status.rotate );
  
  {
    int j;

    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( g3d->g3d_util, g3d->unit_idx );
    int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
    static int frame = 0;

    // アニメーション更新
    for( j=0; j<anime_count; j++ )
    {
//    GFL_G3D_OBJECT_SetAnimeFrame( obj, j, &frame );
      GFL_G3D_OBJECT_LoopAnimeFrame( obj, j, FX32_ONE );
    }
    
    // 描画
    GFL_G3D_DRAW_DrawObject( obj, &status );
  }
}

//-----------------------------------------------------------------------------
/**
 *	@brief  終了処理
 *
 *	@param	COMM_BTL_DEMO_G3D_WORK* g3d 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void G3D_End( COMM_BTL_DEMO_G3D_WORK* g3d )
{ 
  GF_ASSERT( g3d );
  GF_ASSERT( g3d->g3d_util );

  // ユニット削除
  GFL_G3D_UTIL_DelUnit( g3d->g3d_util, g3d->unit_idx );

  // 3D管理ユーティリティーの破棄
  GFL_G3D_UTIL_Delete( g3d->g3d_util );
}

