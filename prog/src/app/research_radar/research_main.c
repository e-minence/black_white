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
// メインプロセス
typedef enum{
  PROC_MAIN_SEQ_SETUP,   // 初期画面準備
  PROC_MAIN_SEQ_MENU,    // 調査初期画面
  PROC_MAIN_SEQ_SELECT,  // 調査内容変更画面
  PROC_MAIN_SEQ_CHECK,   // 調査報告確認画面
  PROC_MAIN_SEQ_FINISH,  // 終了
} PROC_MAIN_SEQ;


//===============================================================================
// ■調査レーダー プロセス ワーク
//===============================================================================
typedef struct {

  HEAPID        heapID;
  GAMESYS_WORK* gameSystem;

  // フレームカウンタ
  u32 frameCount;

  // 全画面共通ワーク
  RESEARCH_COMMON_WORK* commonWork;

  // 各画面専用ワーク
  RESEARCH_MENU_WORK*   menuWork;   // 調査初期画面 ( メニュー画面 )
  RESEARCH_SELECT_WORK* selectWork; // 調査内容変更画面 
  RESEARCH_CHECK_WORK*  checkWork;  // 調査報告確認画面

} RESEARCH_WORK;


//===============================================================================
// ■prototype
//===============================================================================
// プロセス動作
static GFL_PROC_RESULT ResearchRadarProcInit( GFL_PROC* proc, int* seq, void* prm, void* wk );
static GFL_PROC_RESULT ResearchRadarProcEnd ( GFL_PROC* proc, int* seq, void* prm, void* wk );
static GFL_PROC_RESULT ResearchRadarProcMain( GFL_PROC* proc, int* seq, void* prm, void* wk );
// メインプロセス シーケンス制御
static void ChangeMainProcSeq( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq );
static void EndCurrentMainProcSeq( RESEARCH_WORK* work, int* seq );
static void SetMainProcSeq( RESEARCH_WORK* work, int* seq, PROC_MAIN_SEQ nextSeq );
// メインプロセス シーケンス動作
static PROC_MAIN_SEQ ProcMain_SETUP( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_MENU( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_SELECT( RESEARCH_WORK* work );
static PROC_MAIN_SEQ ProcMain_CHECK( RESEARCH_WORK* work );
// フレームカウンタ
static u32 GetFrameCount( const RESEARCH_WORK* work );
static void CountUpFrame( RESEARCH_WORK* work );
// プロセスワークの生成・破棄
static void InitProcWork( RESEARCH_WORK* work, GAMESYS_WORK* gameSystem );
static void CreateCommonWork( RESEARCH_WORK* work );
static void DeleteCommonWork( RESEARCH_WORK* work );
// BG のセットアップ・クリーンアップ
static void SetupBG( HEAPID heapID );
static void CleanUpBG( void );
static void LoadSubBGResources( HEAPID heapID );
static void LoadMainBGResources( HEAPID heapID );
static void UpdateRingAnime( const RESEARCH_WORK* work );


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

  switch( *seq ) {
    case 0:
      // プロセスワーク確保
      GFL_PROC_AllocWork( proc, sizeof(RESEARCH_WORK), GFL_HEAPID_APP ); 
      (*seq)++;
      break;

    case 1: 
      // プロセスワークを初期化
      InitProcWork( work, param->gameSystem );

      // 表示準備
      GFL_DISP_SetBank( &VRAMBankSettings );         // VRAM-Bank 割り当て
      GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );  // 下画面がメイン

      // BG準備
      SetupBG( work->heapID );
      LoadMainBGResources( work->heapID ); 
      LoadSubBGResources ( work->heapID ); 

      // 全画面共通ワークを生成
      CreateCommonWork( work );

      return GFL_PROC_RES_FINISH;
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

  DeleteCommonWork( work ); // 全画面共通ワークを破棄
  CleanUpBG(); // BG をクリーンアップ
  GFL_PROC_FreeWork( proc ); // プロセスワークを破棄

  return GFL_PROC_RES_FINISH;
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
  PROC_MAIN_SEQ  next_seq;

  switch( *seq ) {
  case PROC_MAIN_SEQ_SETUP:   next_seq = ProcMain_SETUP ( work );  break;
  case PROC_MAIN_SEQ_MENU:    next_seq = ProcMain_MENU  ( work );  break;
  case PROC_MAIN_SEQ_SELECT:  next_seq = ProcMain_SELECT( work );  break;
  case PROC_MAIN_SEQ_CHECK:   next_seq = ProcMain_CHECK ( work );  break;
  case PROC_MAIN_SEQ_FINISH:  return GFL_PROC_RES_FINISH;
  default:  GF_ASSERT(0);
  }

  UpdateRingAnime( work ); // 上画面のリングアニメーションを更新
  CountUpFrame( work ); // フレームカウンタ更新

  // シーケンスを更新
  if( *seq != next_seq ) {
    ChangeMainProcSeq( work, seq, next_seq );
  }

  return GFL_PROC_RES_CONTINUE;
}


//===============================================================================
// ■メインプロセス シーケンス制御
//===============================================================================

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

  // 画面の遷移を登録する
  switch( *seq ) {
  case PROC_MAIN_SEQ_MENU:    RESEARCH_COMMON_SetNowSeq( work->commonWork, RADAR_SEQ_MENU ); break;
  case PROC_MAIN_SEQ_SELECT:  RESEARCH_COMMON_SetNowSeq( work->commonWork, RADAR_SEQ_SELECT ); break;
  case PROC_MAIN_SEQ_CHECK:   RESEARCH_COMMON_SetNowSeq( work->commonWork, RADAR_SEQ_CHECK ); break;
  }

  // DEBUG:
  OS_TFPrintf( PRINT_TARGET, "RESEARCH-PROC-MAIN: chenge seq ==> " );
  switch( *seq ) {
  case PROC_MAIN_SEQ_SETUP:   OS_TFPrintf( PRINT_TARGET, "SETUP(%d)\n",  *seq );  break;
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
  switch( *seq ) {
  case PROC_MAIN_SEQ_SETUP:  break;
  case PROC_MAIN_SEQ_MENU:   DeleteResearchMenuWork  ( work->menuWork );    break;
  case PROC_MAIN_SEQ_SELECT: DeleteResearchSelectWork( work->selectWork );  break;
  case PROC_MAIN_SEQ_CHECK:  DeleteResearchCheckWork ( work->checkWork );   break;
  case PROC_MAIN_SEQ_FINISH: break;
  default:  GF_ASSERT(0);
  }

  // 専用ワークポインタをクリア
  switch( *seq ) {
  case PROC_MAIN_SEQ_SETUP:   break;
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

//-------------------------------------------------------------------------------
/**
 * @brief フレームカウンタの値を取得する
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static u32 GetFrameCount( const RESEARCH_WORK* work )
{
  return work->frameCount;
}

//-------------------------------------------------------------------------------
/**
 * @brief フレームカウンタをインクリメントする
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CountUpFrame( RESEARCH_WORK* work )
{
  work->frameCount++;
}

//===============================================================================
// ■プロセスワーク
//===============================================================================

//-------------------------------------------------------------------------------
/**
 * @brief プロセスワークを初期化する
 *
 * @param work
 * @param gameSystem
 */
//-------------------------------------------------------------------------------
static void InitProcWork( RESEARCH_WORK* work, GAMESYS_WORK* gameSystem )
{
  work->heapID     = GFL_HEAPID_APP;
  work->gameSystem = gameSystem;
  work->frameCount = 0;
  work->commonWork = NULL;
  work->menuWork   = NULL;
  work->selectWork = NULL;
  work->checkWork  = NULL;
}

//-------------------------------------------------------------------------------
/**
 * @brief 全画面共通ワークを生成する
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void CreateCommonWork( RESEARCH_WORK* work )
{
  GF_ASSERT( work->commonWork == NULL );

  work->commonWork = RESEARCH_COMMON_CreateWork( work->heapID, work->gameSystem );
}

//-------------------------------------------------------------------------------
/**
 * @brief 全画面共通ワークを破棄する
 *
 * @param work
 */
//-------------------------------------------------------------------------------
static void DeleteCommonWork( RESEARCH_WORK* work )
{
  GF_ASSERT( work->commonWork );

  RESEARCH_COMMON_DeleteWork( work->commonWork );
  work->commonWork = NULL;
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
static void UpdateRingAnime( const RESEARCH_WORK* work )
{ 
  u32 frame;
  u8 paletteOffset;
  u8 paletteNo;

  // 設定するパレット番号を決定
  frame         = GetFrameCount( work );
  paletteOffset = ( (frame / SUB_BG_RADAR_PLT_ANIME_FRAME) % SUB_BG_RADAR_PLT_NUM );
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
