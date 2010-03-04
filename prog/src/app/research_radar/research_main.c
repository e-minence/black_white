/////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  調査レーダー
 * @file   research_main.c
 * @author obata
 * @date   2010.02.02
 */
///////////////////////////////////////////////////////////////////////////////// 
#include "app/research_radar.h"
#include "research_common.h"
#include "research_common_def.h"
#include "research_common_data.cdat"
#include "research_test.h"
#include "research_menu.h"
#include "research_select.h"
#include "research_check.h"

#include "system/main.h"                    // for HEAPID_xxxx
#include "arc/arc_def.h"                    // for ARCID_xxxx
#include "arc/research_radar_graphic.naix"  // for NARC_xxxx



//===============================================================================
// ■定数
//===============================================================================

// SUB-BG 背景面
static const GFL_BG_BGCNT_HEADER SubBGControl_BACK = 
{
  0, 0,                    // 初期表示位置
  0x800,                   // スクリーンバッファサイズ
  0,                       // スクリーンバッファオフセット
  GFL_BG_SCRSIZ_256x256,   // スクリーンサイズ
  GX_BG_COLORMODE_16,      // カラーモード
  GX_BG_SCRBASE_0x0000,    // スクリーンベースブロック
  GX_BG_CHARBASE_0x04000,  // キャラクタベースブロック
  GFL_BG_CHRSIZ_256x256,   // キャラクタエリアサイズ
  GX_BG_EXTPLTT_01,        // BG拡張パレットスロット選択
  SUB_BG_BACK_PRIORITY,    // 表示プライオリティー
  0,                       // エリアオーバーフラグ
  0,                       // DUMMY
  FALSE,                   // モザイク設定
}; 

// SUB-BG レーダー面
static const GFL_BG_BGCNT_HEADER SubBGControl_RADAR = 
{
  0, 0,                    // 初期表示位置
  0x800,                   // スクリーンバッファサイズ
  0,                       // スクリーンバッファオフセット
  GFL_BG_SCRSIZ_256x256,   // スクリーンサイズ
  GX_BG_COLORMODE_16,      // カラーモード
  GX_BG_SCRBASE_0x0800,    // スクリーンベースブロック
  GX_BG_CHARBASE_0x04000,  // キャラクタベースブロック
  GFL_BG_CHRSIZ_256x256,   // キャラクタエリアサイズ
  GX_BG_EXTPLTT_01,        // BG拡張パレットスロット選択
  SUB_BG_RADAR_PRIORITY,   // 表示プライオリティー
  0,                       // エリアオーバーフラグ
  0,                       // DUMMY
  FALSE,                   // モザイク設定
}; 

// MAIN-BG 背景面
static const GFL_BG_BGCNT_HEADER MainBGControl_BACK = 
{
  0, 0,                    // 初期表示位置
  0x800,                   // スクリーンバッファサイズ
  0,                       // スクリーンバッファオフセット
  GFL_BG_SCRSIZ_256x256,   // スクリーンサイズ
  GX_BG_COLORMODE_16,      // カラーモード
  GX_BG_SCRBASE_0x0000,    // スクリーンベースブロック
  GX_BG_CHARBASE_0x04000,  // キャラクタベースブロック
  GFL_BG_CHRSIZ_256x256,   // キャラクタエリアサイズ
  GX_BG_EXTPLTT_01,        // BG拡張パレットスロット選択
  MAIN_BG_BACK_PRIORITY,   // 表示プライオリティー
  0,                       // エリアオーバーフラグ
  0,                       // DUMMY
  FALSE,                   // モザイク設定
};


//===============================================================================
// ■各種プロセス シーケンス番号
//===============================================================================
// 初期化プロセス
typedef enum{
  PROC_INIT_SEQ_ALLOC_WORK,        // プロセスワーク確保
  PROC_INIT_SEQ_INIT_WORK,         // プロセスワーク初期化
  PROC_INIT_SEQ_SETUP_DISPLAY,     // 表示準備
  PROC_INIT_SEQ_SETUP_BG,          // BG 準備
  PROC_INIT_SEQ_SETUP_COMMON_WORK, // 全画面共通ワークの準備
  PROC_INIT_SEQ_FINISH,            // 終了
} PROC_INIT_SEQ;

// 終了プロセス
typedef enum {
  PROC_END_SEQ_CLEAN_UP_COMMON_WORK, // 全画面共通ワークの後片付け
  PROC_END_SEQ_CLEAN_UP_BG,          // BG後片付け
  PROC_END_SEQ_FREE_WORK,            // プロセスワーク解放
  PROC_END_SEQ_FINISH,               // 終了
} PROC_END_SEQ;

// メインプロセス
typedef enum{
  PROC_MAIN_SEQ_SETUP,   // 初期画面準備
  PROC_MAIN_SEQ_TEST,    // テスト画面
  PROC_MAIN_SEQ_MENU,    // 調査初期画面
  PROC_MAIN_SEQ_SELECT,  // 調査内容変更画面
  PROC_MAIN_SEQ_CHECK,   // 調査報告確認画面
  PROC_MAIN_SEQ_FINISH,  // 終了
} PROC_MAIN_SEQ;


//===============================================================================
// ■調査レーダー プロセス ワーク
//===============================================================================
typedef struct
{
  HEAPID        heapID;
  GAMESYS_WORK* gameSystem;

  // フレームカウンタ
  u32 frameCount;

  // 全画面共通ワーク
  RESEARCH_COMMON_WORK* commonWork;

  // 各画面専用ワーク
  RESEARCH_TEST_WORK*   testWork;   // テスト画面
  RESEARCH_MENU_WORK*   menuWork;   // 調査初期画面 ( メニュー画面 )
  RESEARCH_SELECT_WORK* selectWork; // 調査内容変更画面 
  RESEARCH_CHECK_WORK*  checkWork;  // 調査報告確認画面

} RESEARCH_WORK;


//===============================================================================
// ■非公開関数
//===============================================================================
// プロセス動作
static GFL_PROC_RESULT ResearchRadarProcInit( GFL_PROC* proc, int* seq, void* prm, void* wk );
static GFL_PROC_RESULT ResearchRadarProcEnd ( GFL_PROC* proc, int* seq, void* prm, void* wk );
static GFL_PROC_RESULT ResearchRadarProcMain( GFL_PROC* proc, int* seq, void* prm, void* wk );
// メインプロセス シーケンス制御
static void SwitchMainProcSeq    ( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq );
static void ChangeMainProcSeq    ( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq );
static void EndCurrentMainProcSeq( RESEARCH_WORK* work, int* seq );
static void SetMainProcSeq       ( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq );
// メインプロセス シーケンス動作
static PROC_MAIN_SEQ ProcMain_SETUP ( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_TEST  ( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_MENU  ( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_SELECT( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_CHECK ( RESEARCH_WORK* work );
// BG
static void SetupBG  ( HEAPID heapID );
static void CleanUpBG( void );
static void LoadSubBGResources ( HEAPID heapID );
static void LoadMainBGResources( HEAPID heapID );
static void SUB_BG_RADAR_UpdatePaletteAnime( const RESEARCH_WORK* work );


//===============================================================================
// ■プロセス 定義データ
//===============================================================================
GFL_PROC_DATA ResearchRadarProcData = 
{
  ResearchRadarProcInit,
  ResearchRadarProcMain,
  ResearchRadarProcEnd,
};


//===============================================================================
// ■プロセス動作
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief プロセス初期化関数
 *
 * @param proc
 * @param seq  シーケンスワーク
 * @param prm  プロセス呼び出しパラメータ
 * @param wk   プロセスワーク
 */
//-------------------------------------------------------------------------------
static GFL_PROC_RESULT ResearchRadarProcInit( GFL_PROC* proc, int* seq, void* prm, void* wk )
{
  RESEARCH_PARAM* param = prm;
  RESEARCH_WORK*  work  = wk;

  switch( *seq )
  {
  // プロセスワーク確保
  case PROC_INIT_SEQ_ALLOC_WORK:
    GFL_PROC_AllocWork( proc, sizeof(RESEARCH_WORK), GFL_HEAPID_APP );
    (*seq)++;
    break;

  // プロセスワーク初期化
  case PROC_INIT_SEQ_INIT_WORK:
    work->heapID     = GFL_HEAPID_APP;
    work->gameSystem = param->gameSystem;
    work->frameCount = 0;
    work->commonWork = NULL;
    work->testWork   = NULL;
    work->menuWork   = NULL;
    work->selectWork = NULL;
    work->checkWork  = NULL;
    (*seq)++;
    break;

  // 表示準備
  case PROC_INIT_SEQ_SETUP_DISPLAY:
    GFL_DISP_SetBank( &VRAMBankSettings );         // VRAM-Bank 割り当て
    GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );  // 下画面がメイン
    (*seq)++;
    break;

  // BG準備
  case PROC_INIT_SEQ_SETUP_BG:
    SetupBG( work->heapID );
    LoadMainBGResources( work->heapID ); 
    LoadSubBGResources ( work->heapID ); 
    (*seq)++;
    break;

  // 全画面共通ワークの準備
  case PROC_INIT_SEQ_SETUP_COMMON_WORK:
    work->commonWork = RESEARCH_COMMON_CreateWork( work->heapID, work->gameSystem );
    (*seq)++;
    break;

  // 終了
  case PROC_INIT_SEQ_FINISH:
    return GFL_PROC_RES_FINISH;

  // エラー
  default:
    GF_ASSERT(0);
  }
  return GFL_PROC_RES_CONTINUE;
}


//-------------------------------------------------------------------------------
/**
 * @brief プロセス終了関数
 *
 * @param proc
 * @param seq  シーケンスワーク
 * @param prm  プロセス呼び出しパラメータ
 * @param wk   プロセスワーク
 */
//-------------------------------------------------------------------------------
static GFL_PROC_RESULT ResearchRadarProcEnd( GFL_PROC* proc, int* seq, void* prm, void* wk )
{
  RESEARCH_WORK* work = wk;

  switch( *seq )
  {
  // 全画面共通ワークの後片付け
  case PROC_END_SEQ_CLEAN_UP_COMMON_WORK:
    RESEARCH_COMMON_DeleteWork( work->commonWork );
    (*seq)++;
  break;

  // BG後片付け
  case PROC_END_SEQ_CLEAN_UP_BG:
    CleanUpBG();
    (*seq)++;
    break;

  // プロセスワーク解放
  case PROC_END_SEQ_FREE_WORK:
    GFL_PROC_FreeWork( proc );
    (*seq)++;
    break;

  // 終了
  case PROC_END_SEQ_FINISH:
    return GFL_PROC_RES_FINISH;

  // エラー
  default:
    GF_ASSERT(0);
  }
  return GFL_PROC_RES_CONTINUE;
}


//-------------------------------------------------------------------------------
/**
 * @brief プロセス メイン関数
 *
 * @param proc
 * @param seq  シーケンスワーク
 * @param prm  プロセス呼び出しパラメータ
 * @param wk   プロセスワーク
 */
//-------------------------------------------------------------------------------
static GFL_PROC_RESULT ResearchRadarProcMain( GFL_PROC* proc, int* seq, void* prm, void* wk )
{ 
  RESEARCH_WORK* work = wk;
  PROC_MAIN_SEQ  nextSeq;

  switch( *seq )
  {
  case PROC_MAIN_SEQ_SETUP:   nextSeq = ProcMain_SETUP ( work );  break;
  case PROC_MAIN_SEQ_TEST:    nextSeq = ProcMain_TEST  ( work );  break;
  case PROC_MAIN_SEQ_MENU:    nextSeq = ProcMain_MENU  ( work );  break;
  case PROC_MAIN_SEQ_SELECT:  nextSeq = ProcMain_SELECT( work );  break;
  case PROC_MAIN_SEQ_CHECK:   nextSeq = ProcMain_CHECK ( work );  break;
  case PROC_MAIN_SEQ_FINISH:  return GFL_PROC_RES_FINISH;
  default:  GF_ASSERT(0);
  }

  // SUB-BG レーダー面のパレットアニメーション更新
  SUB_BG_RADAR_UpdatePaletteAnime( work );

  // フレームカウンタ更新
  work->frameCount++; 

  // シーケンスの更新
  SwitchMainProcSeq( work, seq, nextSeq );

  // メインプロセス継続
  return GFL_PROC_RES_CONTINUE;
}


//===============================================================================
// ■メインプロセス シーケンス制御
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief シーケンスのスイッチ制御を行う
 *
 * @param work    プロセスワーク
 * @param seq     シーケンスワーク
 * @param nextSeq 次のシーケンス
 */
//-------------------------------------------------------------------------------
static void SwitchMainProcSeq( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq )
{
  // 変更なし
  if( *seq == nextSeq ){ return; }

  // 変更
  ChangeMainProcSeq( work, seq, nextSeq );
}

//-------------------------------------------------------------------------------
/**
 * @brief シーケンスを変更する
 *
 * @param work    プロセスワーク
 * @param seq     シーケンスワーク
 * @param nextSeq 次のシーケンス
 */
//-------------------------------------------------------------------------------
static void ChangeMainProcSeq( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq )
{ 
  // 現在のシーケンスを終了
  EndCurrentMainProcSeq( work, seq );

  // 次のシーケンスを開始
  SetMainProcSeq( work, seq, nextSeq );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-PROC-MAIN: chenge seq ==> " );
  switch( *seq )
  {
  case PROC_MAIN_SEQ_SETUP:   OS_TFPrintf( PRINT_TARGET, "SETUP(%d)\n",  *seq );  break;
  case PROC_MAIN_SEQ_TEST:    OS_TFPrintf( PRINT_TARGET, "TEST(%d)\n",   *seq );  break;
  case PROC_MAIN_SEQ_MENU:    OS_TFPrintf( PRINT_TARGET, "MENU(%d)\n",   *seq );  break;
  case PROC_MAIN_SEQ_SELECT:  OS_TFPrintf( PRINT_TARGET, "SELECT(%d)\n", *seq );  break;
  case PROC_MAIN_SEQ_CHECK:   OS_TFPrintf( PRINT_TARGET, "CHECK(%d)\n",  *seq );  break;
  case PROC_MAIN_SEQ_FINISH:  OS_TFPrintf( PRINT_TARGET, "FINISH(%d)\n", *seq );  break;
  default:  GF_ASSERT(0);
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief 現在のメインプロセス シーケンスを終了する
 *
 * @param work プロセスワーク
 * @param seq  シーケンスワーク
 */
//-------------------------------------------------------------------------------
static void EndCurrentMainProcSeq( RESEARCH_WORK* work, int* seq )
{
  // 現在のシーケンスの専用ワークを破棄
  switch( *seq )
  {
  case PROC_MAIN_SEQ_SETUP:  break;
  case PROC_MAIN_SEQ_TEST:   DeleteResearchTestWork  ( work->testWork );    break;
  case PROC_MAIN_SEQ_MENU:   DeleteResearchMenuWork  ( work->menuWork );    break;
  case PROC_MAIN_SEQ_SELECT: DeleteResearchSelectWork( work->selectWork );  break;
  case PROC_MAIN_SEQ_CHECK:  DeleteResearchCheckWork ( work->checkWork );   break;
  case PROC_MAIN_SEQ_FINISH: break;
  default:  GF_ASSERT(0);
  }

  // 専用ワークポインタをクリア
  switch( *seq )
  {
  case PROC_MAIN_SEQ_SETUP:   break;
  case PROC_MAIN_SEQ_TEST:    work->testWork   = NULL;  break;
  case PROC_MAIN_SEQ_MENU:    work->menuWork   = NULL;  break;
  case PROC_MAIN_SEQ_SELECT:  work->selectWork = NULL;  break;
  case PROC_MAIN_SEQ_CHECK:   work->checkWork  = NULL;  break;
  case PROC_MAIN_SEQ_FINISH:  break;
  default:  GF_ASSERT(0);
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-PROC-MAIN: end current seq(%d)\n", *seq );
}

//-------------------------------------------------------------------------------
/**
 * @brief シーケンスを設定する
 *
 * @param work    プロセスワーク
 * @param seq     シーケンスワーク
 * @param nextSeq 設定するシーケンス
 */
//-------------------------------------------------------------------------------
static void SetMainProcSeq( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq )
{
  // 多重生成チェック
  switch( *seq )
  {
  case PROC_MAIN_SEQ_SETUP:   break;
  case PROC_MAIN_SEQ_TEST:    GF_ASSERT( work->testWork   == NULL );  break;
  case PROC_MAIN_SEQ_MENU:    GF_ASSERT( work->menuWork   == NULL );  break;
  case PROC_MAIN_SEQ_SELECT:  GF_ASSERT( work->selectWork == NULL );  break;
  case PROC_MAIN_SEQ_CHECK:   GF_ASSERT( work->checkWork  == NULL );  break;
  case PROC_MAIN_SEQ_FINISH:  break;
  default:  GF_ASSERT(0);
  } 

  // 次のシーケンスの専用ワークを生成
  switch( nextSeq )
  {
  case PROC_MAIN_SEQ_SETUP:   break;
  case PROC_MAIN_SEQ_TEST:    work->testWork   = CreateResearchTestWork  ( work->heapID );  break;
  case PROC_MAIN_SEQ_MENU:    work->menuWork   = CreateResearchMenuWork  ( work->commonWork );  break;
  case PROC_MAIN_SEQ_SELECT:  work->selectWork = CreateResearchSelectWork( work->commonWork );  break;
  case PROC_MAIN_SEQ_CHECK:   work->checkWork  = CreateResearchCheckWork ( work->commonWork );  break;
  case PROC_MAIN_SEQ_FINISH:  break;
  default:  GF_ASSERT(0);
  } 

  // シーケンスを変更
  *seq = nextSeq; 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-PROC-MAIN: set seq(%d)\n", *seq );
}


//===============================================================================
// ■メインプロセス シーケンス動作
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief メインプロセス 初期画面準備
 *
 * @param work プロセスワーク
 *
 * @return 次のシーケンス
 */
//-------------------------------------------------------------------------------
static PROC_MAIN_SEQ ProcMain_SETUP( RESEARCH_WORK* work )
{
  return PROC_MAIN_SEQ_MENU;
  //return PROC_MAIN_SEQ_TEST;
}

//-------------------------------------------------------------------------------
/**
 * @brief メインプロセス テスト画面
 *
 * @param work プロセスワーク
 *
 * @return テスト画面が終了した場合 次のシーケンス
 *         そうでなければ 現在のシーケンス
 */
//-------------------------------------------------------------------------------
static PROC_MAIN_SEQ ProcMain_TEST( RESEARCH_WORK* work )
{ 
  RESEARCH_TEST_RESULT result;
  PROC_MAIN_SEQ  nextSeq;

  // テスト画面メイン処理
  result = ResearchTestMain( work->testWork );
  
  // 次のシーケンスを決定
  switch( result )
  {
  case RESEARCH_TEST_RESULT_CONTINUE:  nextSeq = PROC_MAIN_SEQ_TEST;  break;
  case RESEARCH_TEST_RESULT_END:       nextSeq = PROC_MAIN_SEQ_MENU;  break;
  default:  GF_ASSERT(0);
  } 
  return nextSeq;
}

//-------------------------------------------------------------------------------
/**
 * @brief メインプロセス 調査レーダー初期画面
 *
 * @param work プロセスワーク
 *
 * @return 調査レーダー初期画面が終了した場合 次のシーケンス
 *         そうでなければ 現在のシーケンス
 */
//-------------------------------------------------------------------------------
static PROC_MAIN_SEQ ProcMain_MENU( RESEARCH_WORK* work )
{
  RESEARCH_MENU_RESULT result;
  PROC_MAIN_SEQ  nextSeq;

  // 調査レーダー初期画面メイン処理
  result = ResearchMenuMain( work->menuWork );
  
  // 次のシーケンスを決定
  switch( result )
  {
  case RESEARCH_MENU_RESULT_CONTINUE:  nextSeq = PROC_MAIN_SEQ_MENU;    break;
  case RESEARCH_MENU_RESULT_TO_SELECT: nextSeq = PROC_MAIN_SEQ_SELECT;  break;
  case RESEARCH_MENU_RESULT_TO_CHECK:  nextSeq = PROC_MAIN_SEQ_CHECK;   break;
  case RESEARCH_MENU_RESULT_EXIT:      nextSeq = PROC_MAIN_SEQ_FINISH;  break;
  default:  GF_ASSERT(0);
  } 
  return nextSeq;
}

//-------------------------------------------------------------------------------
/**
 * @brief メインプロセス 調査項目変更画面
 *
 * @param work プロセスワーク
 *
 * @return 調査項目変更画面が終了した場合 次のシーケンス
 *         そうでなければ 現在のシーケンス
 */
//-------------------------------------------------------------------------------
static PROC_MAIN_SEQ ProcMain_SELECT( RESEARCH_WORK* work )
{
  RESEARCH_SELECT_RESULT result;
  PROC_MAIN_SEQ  nextSeq;

  // 調査項目変更画面メイン処理
  result = ResearchSelectMain( work->selectWork );
  
  // 次のシーケンスを決定
  switch( result )
  {
  case RESEARCH_SELECT_RESULT_CONTINUE:  nextSeq = PROC_MAIN_SEQ_SELECT;  break;
  case RESEARCH_SELECT_RESULT_TO_MENU:   nextSeq = PROC_MAIN_SEQ_MENU;    break;
  default:  GF_ASSERT(0);
  } 
  return nextSeq;
}

//-------------------------------------------------------------------------------
/**
 * @brief メインプロセス 調査報告確認画面
 *
 * @param work プロセスワーク
 *
 * @return 調査報告確認画面が終了した場合 次のシーケンス
 *         そうでなければ 現在のシーケンス
 */
//-------------------------------------------------------------------------------
static PROC_MAIN_SEQ ProcMain_CHECK( RESEARCH_WORK* work )
{
  RESEARCH_CHECK_RESULT result;
  PROC_MAIN_SEQ  nextSeq;

  // 調査報告確認画面メイン処理
  result = ResearchCheckMain( work->checkWork );
  
  // 次のシーケンスを決定
  switch( result )
  {
  case RESEARCH_CHECK_RESULT_CONTINUE:  nextSeq = PROC_MAIN_SEQ_CHECK;  break;
  case RESEARCH_CHECK_RESULT_TO_MENU:   nextSeq = PROC_MAIN_SEQ_MENU;   break;
  default:  GF_ASSERT(0);
  } 
  return nextSeq;
} 


//===============================================================================
// ■BG
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief BG 準備
 * 
 * @param heapID
 */
//-------------------------------------------------------------------------------
static void SetupBG( HEAPID heapID )
{ 
  GFL_BG_Init( heapID );

  // BGモード
  GFL_BG_SetBGMode( &BGSysHeader2D );

  // SUB-BG
  GFL_BG_SetBGControl( SUB_BG_BACK,  &SubBGControl_BACK,  GFL_BG_MODE_TEXT );
  GFL_BG_SetBGControl( SUB_BG_RADAR, &SubBGControl_RADAR, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( SUB_BG_BACK,  VISIBLE_ON );
  GFL_BG_SetVisible( SUB_BG_RADAR, VISIBLE_ON );

  // MAIN-BG
  GFL_BG_SetBGControl( MAIN_BG_BACK, &MainBGControl_BACK, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( MAIN_BG_BACK, VISIBLE_ON );

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH: setup BG\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief BG 後片付け
 */
//-------------------------------------------------------------------------------
static void CleanUpBG()
{
  GFL_BG_FreeBGControl( MAIN_BG_BACK );
  GFL_BG_FreeBGControl( SUB_BG_BACK );
  GFL_BG_Exit();

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH: clean up BG\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief SUB-BG のリソースを読み込む
 *
 * @param heapID 使用するヒープID
 */
//-------------------------------------------------------------------------------
static void LoadSubBGResources( HEAPID heapID )
{ 
  // データ読み込み
  {
    ARCHANDLE* handle;

    // ハンドルオープン
    handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, heapID ); 

    // パレットデータ
    {
      void* src;
      ARCDATID datID;
      NNSG2dPaletteData* data;
      datID = NARC_research_radar_graphic_bgu_NCLR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedPaletteData( src, &data );
      GFL_BG_LoadPalette( SUB_BG_BACK, data->pRawData, FULL_PALETTE_SIZE, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    // キャラクタデータ
    {
      void* src;
      ARCDATID datID;
      NNSG2dCharacterData* data;
      datID = NARC_research_radar_graphic_bgu_NCGR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedBGCharacterData( src, &data );
      GFL_BG_LoadCharacter( SUB_BG_BACK, data->pRawData, data->szByte, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    // スクリーンデータ ( 背景面 )
    {
      void* src;
      ARCDATID datID;
      NNSG2dScreenData* data;
      datID = NARC_research_radar_graphic_bgu_base_NSCR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedScreenData( src, &data );
      GFL_BG_WriteScreen( SUB_BG_BACK, data->rawData, 0, 0, 32, 24 );
      GFL_BG_LoadScreenReq( SUB_BG_BACK );
      GFL_HEAP_FreeMemory( src );
    }
    // スクリーンデータ ( レーダー面 )
    {
      void* src;
      ARCDATID datID;
      NNSG2dScreenData* data;
      datID = NARC_research_radar_graphic_bgu_ring_NSCR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedScreenData( src, &data );
      GFL_BG_WriteScreen( SUB_BG_RADAR, data->rawData, 0, 0, 32, 24 );
      GFL_BG_LoadScreenReq( SUB_BG_RADAR );
      GFL_HEAP_FreeMemory( src );
    }

    // ハンドルクローズ
    GFL_ARC_CloseDataHandle( handle );
  } 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH: load SUB-BG resources\n" );
}

//-------------------------------------------------------------------------------
/**
 * @brief SUB-BG レーダー面のパレットアニメーションを更新する
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void SUB_BG_RADAR_UpdatePaletteAnime( const RESEARCH_WORK* work )
{ 
  u8 paletteOffset;
  u8 paletteNo;

  // 設定するパレット番号を決定
  paletteOffset = ( (work->frameCount / SUB_BG_RADAR_PLT_ANIME_FRAME) % SUB_BG_RADAR_PLT_NUM );
  paletteNo     = SUB_BG_RADAR_FIRST_PLT_IDX + paletteOffset;

  // スクリーンデータを更新
  GFL_BG_ChangeScreenPalette( SUB_BG_RADAR, 0, 0, 32, 24, paletteNo );
  GFL_BG_LoadScreenReq( SUB_BG_RADAR );
}

//-------------------------------------------------------------------------------
/**
 * @brief MAIN-BG のリソースを読み込む
 *
 * @param heapID 使用するヒープID
 */
//-------------------------------------------------------------------------------
static void LoadMainBGResources( HEAPID heapID )
{ 
  // データ読み込み
  {
    ARCHANDLE* handle;

    // ハンドルオープン
    handle = GFL_ARC_OpenDataHandle( ARCID_RESEARCH_RADAR_GRAPHIC, heapID ); 

    // パレットデータ
    {
      void* src;
      ARCDATID datID;
      NNSG2dPaletteData* data;
      datID = NARC_research_radar_graphic_bgd_NCLR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedPaletteData( src, &data );
      GFL_BG_LoadPalette( MAIN_BG_BACK, data->pRawData, FULL_PALETTE_SIZE, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    // キャラクタデータ
    {
      void* src;
      ARCDATID datID;
      NNSG2dCharacterData* data;
      datID = NARC_research_radar_graphic_bgd_NCGR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedBGCharacterData( src, &data );
      GFL_BG_LoadCharacter( MAIN_BG_BACK, data->pRawData, data->szByte, 0 );
      GFL_HEAP_FreeMemory( src );
    }
    // スクリーンデータ ( 背景面 )
    {
      void* src;
      ARCDATID datID;
      NNSG2dScreenData* data;
      datID = NARC_research_radar_graphic_bgd_base_NSCR;
      src   = GFL_ARC_LoadDataAllocByHandle( handle, datID, heapID );
      NNS_G2dGetUnpackedScreenData( src, &data );
      GFL_BG_WriteScreen( MAIN_BG_BACK, data->rawData, 0, 0, 32, 24 );
      GFL_BG_LoadScreenReq( MAIN_BG_BACK );
      GFL_HEAP_FreeMemory( src );
    }

    // ハンドルクローズ
    GFL_ARC_CloseDataHandle( handle );
  } 

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH: load MAIN-BG resources\n" );
}
