//=============================================================================
/**
 *
 *  @file   demo3d.c
 *  @brief  3Dデモ再生アプリ
 *  @author genya hosak -> miyuki iwasawa
 *  @data   2009.11.27 -> 2010.02.18
 */
//=============================================================================
//必ず必要なインクルード
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/wipe.h"

//タスクメニュー
#include "app/app_taskmenu.h"

//アプリ共通素材
#include "app/app_menu_common.h"

// 文字列関連
#include "font/font.naix"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/printsys.h" //PRINT_QUE

//簡易CLWK読み込み＆開放ユーティリティー
#include "ui/ui_easy_clwk.h"

#include "field/field_light_status.h"
#include "field/weather_no.h"

//アーカイブ
#include "arc_def.h"
#include "arc/fieldmap/zone_id.h"

//外部公開
#include "message.naix"

#include "demo3d_local.h"
#include "demo3d_graphic.h"
#include "demo3d_engine.h"
#include "demo3d_exp.h"
#include "demo3d_data.h"

FS_EXTERN_OVERLAY(ui_common);

//=============================================================================
/**
 *                定数定義
 */
//=============================================================================
enum
{ 
  DEMO3D_HEAP_SIZE = 0x130000,  ///< ヒープサイズ
};

//=============================================================================
/**
 *                構造体定義
 */
//=============================================================================

//--------------------------------------------------------------
/// BG管理ワーク
//==============================================================
typedef struct 
{
  int dummy;
} DEMO3D_BG_WORK;

//--------------------------------------------------------------
/// メインワーク
//==============================================================
typedef struct 
{
  HEAPID heapID;

  DEMO3D_PARAM*         param;

  DEMO3D_BG_WORK        wk_bg;

  //描画設定
  DEMO3D_GRAPHIC_WORK *graphic;

  //フォント
  GFL_FONT                  *font;

  //プリントキュー
  PRINT_QUE                 *print_que;
  GFL_MSGDATA               *msg;

  // デモエンジン
  DEMO3D_ENGINE_WORK*   engine;

  // アプリ例外処理エンジン
  APP_EXCEPTION_WORK*   expection;
  

} DEMO3D_MAIN_WORK;


//=============================================================================
/**
 *              データ定義
 */
//=============================================================================


//=============================================================================
/**
 *              プロトタイプ宣言
 */
//=============================================================================
//-------------------------------------
/// PROC
//=====================================
static GFL_PROC_RESULT Demo3DProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT Demo3DProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk );
static GFL_PROC_RESULT Demo3DProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk );

//-----------------------------------------------------------
// アプリ例外処理
//-----------------------------------------------------------
static APP_EXCEPTION_WORK* APP_EXCEPTION_Create( DEMO3D_ID demo_id, DEMO3D_GRAPHIC_WORK* graphic, DEMO3D_ENGINE_WORK* engine, HEAPID heapID );
static void APP_EXCEPTION_Delete( APP_EXCEPTION_WORK* wk );
static void APP_EXCEPTION_Main( APP_EXCEPTION_WORK* wk );

static BOOL sub_FadeInOutReq( u8 demo_id, u8 wipe, HEAPID heapID );
static int _key_check( DEMO3D_MAIN_WORK *wk );

//=============================================================================
/**
 *                外部公開
 */
//=============================================================================
const GFL_PROC_DATA Demo3DProcData = 
{
  Demo3DProc_Init,
  Demo3DProc_Main,
  Demo3DProc_Exit,
};
//=============================================================================
/**
 *                PROC
 */
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief  PROC 初期化処理
 *
 *  @param  GFL_PROC *procプロセスシステム
 *  @param  *seq          シーケンス
 *  @param  *pwk          DEMO3D_PARAM
 *  @param  *mywk         PROCワーク
 *
 *  @retval 終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT Demo3DProc_Init( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{
  DEMO3D_MAIN_WORK *wk;
  DEMO3D_PARAM *param;

  //オーバーレイ読み込み
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );
  
  //引数取得
  param = pwk;

  GF_ASSERT( param->demo_id != DEMO3D_ID_NULL && param->demo_id < DEMO3D_ID_MAX );

  //ヒープ作成
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_DEMO3D, DEMO3D_HEAP_SIZE );
  wk = GFL_PROC_AllocWork( proc, sizeof(DEMO3D_MAIN_WORK), HEAPID_DEMO3D );
  GFL_STD_MemClear( wk, sizeof(DEMO3D_MAIN_WORK) );

  // 初期化
  wk->heapID      = HEAPID_DEMO3D;
  wk->param       = param;

  //描画設定初期化
  wk->graphic = DEMO3D_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, param->demo_id, wk->heapID );

  //フォント作成
  wk->font      = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
                        GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );

  //メッセージ
  wk->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
      NARC_message_mictest_dat, wk->heapID );

  //PRINT_QUE作成
  wk->print_que   = PRINTSYS_QUE_Create( wk->heapID );

  // BG/OBJを非表示にしておく
  GFL_BG_SetVisible( BG_FRAME_BACK_S, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_TEXT_S, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_BACK_M, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_POKE_M, VISIBLE_OFF );
  GFL_BG_SetVisible( BG_FRAME_BAR_M,  VISIBLE_OFF );
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );

  //3D 初期化
  wk->engine = Demo3D_ENGINE_Init( wk->graphic, param, wk->heapID );
 
  // デモ毎の例外処理エンジン初期化
  wk->expection = APP_EXCEPTION_Create( param->demo_id, wk->graphic, wk->engine, wk->heapID );

  // フェードイン リクエスト
  sub_FadeInOutReq( param->demo_id, WIPE_TYPE_FADEIN, wk->heapID );

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  PROC 終了処理
 *
 *  @param  GFL_PROC *procプロセスシステム
 *  @param  *seq          シーケンス
 *  @param  *pwk          DEMO3D_PARAM
 *  @param  *mywk         PROCワーク
 *
 *  @retval 終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT Demo3DProc_Exit( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
  DEMO3D_MAIN_WORK* wk = mywk;
 
#if 0
  switch( *seq ){
  case 0:
    // フェードアウト リクエスト
    (*seq) = 1+sub_FadeInOutReq( wk->param->demo_id, WIPE_TYPE_FADEOUT, wk->heapID );
    return GFL_PROC_RES_CONTINUE;
  case 1:
    if( WIPE_SYS_EndCheck() == FALSE ){
      return GFL_PROC_RES_CONTINUE;
    }
    break;
  default:
    break;
  }
#endif
  // 例外処理エンジン 終了処理
  APP_EXCEPTION_Delete( wk->expection );

  //メッセージ破棄
  GFL_MSG_Delete( wk->msg );

  //PRINT_QUE
  PRINTSYS_QUE_Delete( wk->print_que );

  //FONT
  GFL_FONT_Delete( wk->font );
  
  //3D 破棄
  Demo3D_ENGINE_Exit( wk->engine );
  GF_ASSERT_MSG( GFL_HEAP_CheckHeapSafe( HEAPID_DEMO3D ) == TRUE,"Demo3D HeapError!\n" );

  //描画設定破棄
  DEMO3D_GRAPHIC_Exit( wk->graphic );

  //PROC用メモリ解放
  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( wk->heapID );

  //オーバーレイ破棄
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

  return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  PROC 主処理
 *
 *  @param  GFL_PROC *procプロセスシステム
 *  @param  *seq          シーケンス
 *  @param  *pwk          DEMO3D_PARAM
 *  @param  *mywk         PROCワーク
 *
 *  @retval 終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT Demo3DProc_Main( GFL_PROC *proc, int *seq, void *pwk, void *mywk )
{ 
  DEMO3D_MAIN_WORK* wk = mywk;
  BOOL is_end;

#if 0 
  // フェード中は処理しない
  if( WIPE_SYS_EndCheck() == FALSE ){
  {
    return GFL_PROC_RES_CONTINUE;
  }
#endif

  // 例外処理エンジン 主処理
  APP_EXCEPTION_Main( wk->expection );

  //PRINT_QUE
  PRINTSYS_QUE_Main( wk->print_que );

  //2D描画
  DEMO3D_GRAPHIC_2D_Draw( wk->graphic );

  //3D描画
  is_end = Demo3D_ENGINE_Main( wk->engine );

  switch( *seq ){
  case 0:
    // ループ検出かキー終了有効の時にはキーでも終了
    if(_key_check(wk) || is_end ){
      // [OUT] フレーム値を設定
      wk->param->end_frame  = DEMO3D_ENGINE_GetNowFrame( wk->engine ) >> FX32_SHIFT; 
      wk->param->result     = DEMO3D_RESULT_USER_END + is_end;
      DEMO3D_ENGINE_SetEndResult( wk->engine, wk->param->result );
    
      // フェードアウト リクエスト
      if( sub_FadeInOutReq( wk->param->demo_id, WIPE_TYPE_FADEOUT, wk->heapID )){
        return GFL_PROC_RES_FINISH;
      }
      (*seq)++;
    }
    break;
  case 1:
    if( WIPE_SYS_EndCheck() == TRUE ){
      return GFL_PROC_RES_FINISH;
    }
  }
  return GFL_PROC_RES_CONTINUE;
}


//=============================================================================
/**
 *                static関数
 */
//=============================================================================

//----------------------------------------------------------------------------------
/*
 *  @brief  フェードイン/アウトリクエスト
 */
//----------------------------------------------------------------------------------
static BOOL sub_FadeInOutReq( u8 demo_id, u8 wipe, HEAPID heapID )
{
  u8  type, sync;
  u16 color;

  Demo3D_DATA_GetFadeParam( demo_id, (wipe == WIPE_TYPE_FADEOUT), &type, &sync );
  
  if( type == DEMO3D_FADE_BLACK ){
    color = WIPE_FADE_BLACK;
  }else{
    color = WIPE_FADE_WHITE;
  }
  if( sync > 0 ){
    WIPE_SYS_Start( WIPE_PATTERN_WMS, wipe, wipe, color, sync, 1, heapID );
    return FALSE;
  }
  WIPE_SetBrightnessFadeOut( color );
  return TRUE;
}

//----------------------------------------------------------------------------------
/**
 * @brief キーチェック（key_skipがTRUE時だけデモ終了できる)
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int _key_check( DEMO3D_MAIN_WORK *wk )
{
  if( wk->param->result != DEMO3D_RESULT_NULL || WIPE_SYS_EndCheck() == FALSE ){
    return FALSE;
  }
  if( wk->expection->key_skip )
  {
    if( GFL_UI_KEY_GetTrg() & 
        (  PAD_BUTTON_DECIDE 
        | PAD_BUTTON_CANCEL 
        | PAD_BUTTON_X 
        | PAD_BUTTON_Y
        | PAD_KEY_UP 
        | PAD_KEY_DOWN 
        | PAD_KEY_LEFT 
        | PAD_KEY_RIGHT )
    )
    {
      return TRUE;
    }
        
  }

#ifdef PM_DEBUG
  if( (GFL_UI_KEY_GetCont() &PAD_BUTTON_X) && (GFL_UI_KEY_GetCont() &PAD_BUTTON_Y)){
    return TRUE;
  }
#endif
  return FALSE;
}

//-----------------------------------------------------------------------------
// アプリごとの例外表示
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *  @brief  アプリ例外処理エンジン 生成
 *
 *  @param  DEMO3D_ID demo_id
 *  @param  heapID 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static APP_EXCEPTION_WORK* APP_EXCEPTION_Create( DEMO3D_ID demo_id, DEMO3D_GRAPHIC_WORK* graphic, DEMO3D_ENGINE_WORK* engine, HEAPID heapID )
{
  APP_EXCEPTION_WORK* wk;

  GF_ASSERT( graphic );
  GF_ASSERT( engine );
  
  // メインワーク アロケート
  wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(APP_EXCEPTION_WORK) );

  // メンバ初期化
  wk->graphic = graphic;
  wk->engine = engine;
  wk->heapID = heapID;

  wk->p_funcset = NULL;
  wk->key_skip  = FALSE;

  // 遊覧船
  switch( demo_id )
  {
  case DEMO3D_ID_C_CRUISER :
    wk->p_funcset = &DATA_c_exp_funcset_c_cruiser;
    wk->key_skip  = TRUE;                     // キースキップ有効
    break;
  default : 
    ;
  }

  // 初期化処理
  if( wk->p_funcset )
  {
    wk->p_funcset->Init( wk );
  }

  return wk;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  アプリ例外処理エンジン 削除
 *
 *  @param  APP_EXCEPTION_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void APP_EXCEPTION_Delete( APP_EXCEPTION_WORK* wk )
{
  GF_ASSERT(wk);

  // 終了処理
  if( wk->p_funcset )
  {
    wk->p_funcset->End( wk );
  }

  GFL_HEAP_FreeMemory( wk );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  アプリ例外処理エンジン 主処理
 *
 *  @param  APP_EXCEPTION_WORK* wk 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void APP_EXCEPTION_Main( APP_EXCEPTION_WORK* wk )
{
  GF_ASSERT(wk);

  // 主処理
  if( wk->p_funcset )
  {
    wk->p_funcset->Main( wk );
  }
}


