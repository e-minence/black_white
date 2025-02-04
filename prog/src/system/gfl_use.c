//=============================================================================================
/**
 * @file  gfl_use.c
 * @date  2006.12.06
 * @author  GAME FREAK inc.
 * @brief ユーザーのGFライブラリ使用のためのスケルトンソース
 *
 */
//=============================================================================================

#include <nitro.h>

#include <gflib.h>
#include <assert_default.h>

#include <procsys.h>
#include <gf_overlay.h>
#include <tcb.h>
#include <backup_system.h>

//#include "gflib_inter.h"

#include "system\main.h"
#include "system\gfl_use.h"
#include "arc_def.h"
#include "arc_file.h"
//#include "net_old/net_old.h"
#include "savedata/save_control.h"
#include "net/net_dev.h"
#include "system/save_error_warning.h"

#include "textprint.h"

#ifdef PM_DEBUG
#include "test/debug_pause.h"
#include "debug/debugwin_sys.h"
#include "debug/gf_mcs.h"
#include "sound/snd_viewer_mcs.h"
#include "debug/mcs_resident.h"
#include "debug/mcs_mode.h"


#endif //PM_DEBUG

//=============================================================================================
//=============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
  TCB_HINTR_MAX = 4,
  TCB_VINTR_MAX = 16,
};

//------------------------------------------------------------------
//  ヒープサイズ定義
//------------------------------------------------------------------
#define HEAPSIZE_SYSTEM (0x006000)
#define HEAPSIZE_APP    (0x180000)   // PL,GSでは約0x13A000
#define HEAPSIZE_DSI    (0x486000)   // DSIは16M
//NITROでmakeした時にHEAPID_EXTRA用に4000増やす
#define HEAPSIZE_APP_NTR    (0x184000)   // PL,GSでは約0x13A000

//------------------------------------------------------------------
//------------------------------------------------------------------
typedef struct {
    GFL_STD_RandContext publicRandContext;
  GFL_TCBSYS *  TCBSysHintr;
  void *    TCBMemHintr;
  GFL_TCBSYS *  TCBSysVintr;
  void *    TCBMemVintr;
}GFL_USE_WORK;

//------------------------------------------------------------------
//------------------------------------------------------------------
static const HEAP_INIT_HEADER hih[]={
  { HEAPSIZE_SYSTEM,  OS_ARENA_MAIN },
#ifdef DEBUG_ONLY_FOR_taya
  { 0x182000,   OS_ARENA_MAIN },
//  { HEAPSIZE_APP,   OS_ARENA_MAIN },
#else
#if (defined(SDK_TWL))
  { HEAPSIZE_APP,   OS_ARENA_MAIN },
#else
  { HEAPSIZE_APP_NTR,   OS_ARENA_MAIN },
#endif
#endif
};

static const HEAP_INIT_HEADER hihDSi[]={  //DSI用
  { HEAPSIZE_SYSTEM,  OS_ARENA_MAIN },
  { HEAPSIZE_DSI,   OS_ARENA_MAIN },
};


static GFL_USE_WORK * gfl_work = NULL;
static int        GFL_USE_VintrCounter;

static void GFUser_PublicRandInit(void);

#ifdef PM_DEBUG
FS_EXTERN_OVERLAY(mcs_lib);
BOOL  mcs_recv_auto;

static void mcsRecv( void );
#endif

static GFL_USE_VINTR_FUNC GflUseVintrFunc;
static void *GflUseVintrWork;
static void *extraHeapBuffer = NULL;
static OSHeapHandle extraHeapHandle;
//=============================================================================================
//
//      関数
//
//=============================================================================================
//------------------------------------------------------------------
/**
 * @brief GFライブラリ利用部分：初期化
 *
 * InitSystemの内部で呼んでいるInitFileSystemはgflibの外では
 * 呼べないため、InitSystemを呼んでいる。
 */
//------------------------------------------------------------------
void GFLUser_Init(void)
{
  //InitSystem();
#if 0
#ifdef USE_MAINEXMEMORY
  OS_EnableMainExArena();
#endif

  // システム初期化
  OS_Init();
  OS_InitTick();
  OS_InitAlarm();
  // RTC初期化
  RTC_Init();
  // GX初期化
  GX_Init();
  // 固定小数点API初期化
  FX_Init();
  // ファイルシステム初期化
  InitFileSystem();
#endif

#ifndef MULTI_BOOT_MAKE
#if (defined(SDK_TWL))
  if ( OS_IsRunOnTwl() == FALSE )
  {
    void *nstart;
    nstart = OS_InitAlloc( OS_ARENA_MAIN, OS_GetMainArenaLo(), OS_GetMainArenaHi(), 1 );
    OS_SetMainArenaLo( nstart );
    extraHeapHandle = OS_CreateExtraHeap( OS_ARENA_MAIN );
    extraHeapBuffer = OS_AllocFromHeap( OS_ARENA_MAIN, extraHeapHandle, 0x3F00 );
  }
  else
#endif
  {
  }
#endif

#if (defined(SDK_TWL))
  //ヒープシステム初期化
  if( OS_IsRunOnTwl() ){//DSIなら
    GFL_HEAP_Init(&hihDSi[0],GFL_HEAPID_MAX,HEAPID_CHILD_MAX,0); //メインアリーナ
  }
  else{
    GFL_HEAP_Init(&hih[0],GFL_HEAPID_MAX,HEAPID_CHILD_MAX,0); //メインアリーナ
  }
#else
  GFL_HEAP_Init(&hih[0],GFL_HEAPID_MAX,HEAPID_CHILD_MAX,0); //メインアリーナ
#endif


  GFL_HEAP_DTCM_Init( 0x2000 );       //ＤＴＣＭアリーナ

#ifndef MULTI_BOOT_MAKE
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_SAVE, HEAPSIZE_SAVE );

  //特殊ヒープ領域確保
  //ハイブリッドモード時にNITRO起動だと先頭16kbがヒープとして確保可能
  //TWLの場合は多いから普通にとる
  
#if (defined(SDK_TWL))
  if ( OS_IsRunOnTwl() == FALSE )
  {
    GFL_HEAP_CreateHeapInBuffer( extraHeapBuffer , 0x3F00 , HEAPID_EXTRA );
  }
  else
#endif
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_EXTRA, 0x3F00 );
  }
  
#endif //MULTI_BOOT_MAKE

  //STD 標準ライブラリ初期化（乱数やCRC）
  GFL_STD_Init(GFL_HEAPID_SYSTEM);
  //アーカイブシステム初期化
#ifndef MULTI_BOOT_MAKE
  GFL_ARC_Init(&ArchiveFileTable[0],ARCID_TABLE_MAX);
#else
  GFL_ARC_InitMultiBoot(&ArchiveFileTable[0],ARCID_TABLE_MAX);
#endif //MULTI_BOOT_MAKE

  //UIシステム初期化
#ifndef MULTI_BOOT_MAKE
  GFL_UI_Boot(GFL_HEAPID_SYSTEM,FALSE);
#else
  GFL_UI_Boot(GFL_HEAPID_SYSTEM,TRUE);
#endif //MULTI_BOOT_MAKE

  // アサート停止関数の設定
  if( OS_GetConsoleType() & (OS_CONSOLE_NITRO|OS_CONSOLE_TWL) ){
    // ROM実行用（可能なら情報をLCD表示）
    GFL_ASSERT_SetLCDMode();
  }else{
    // NITROデバッガ用（コールスタック表示＆可能なら処理続行のため）
    GFL_ASSERT_SetDisplayFunc( NULL, NULL, GFUser_AssertFinish );
  }

  //OVERLAYシステム初期化
  GFL_OVERLAY_boot(GFL_HEAPID_SYSTEM, 16, 4, 4);

  //PROCシステム初期化
  GFL_PROC_boot(GFL_HEAPID_SYSTEM);
  gfl_work = GFL_HEAP_AllocMemory(GFL_HEAPID_SYSTEM, sizeof(GFL_USE_WORK));
  gfl_work->TCBMemHintr = GFL_HEAP_AllocMemory(
      GFL_HEAPID_SYSTEM, GFL_TCB_CalcSystemWorkSize(TCB_HINTR_MAX));
  gfl_work->TCBSysHintr = GFL_TCB_Init(TCB_HINTR_MAX, gfl_work->TCBMemHintr);
  gfl_work->TCBMemVintr = GFL_HEAP_AllocMemory(
      GFL_HEAPID_SYSTEM, GFL_TCB_CalcSystemWorkSize(TCB_VINTR_MAX));
  gfl_work->TCBSysVintr = GFL_TCB_Init(TCB_VINTR_MAX, gfl_work->TCBMemVintr);

  // 汎用乱数初期化
  GFUser_PublicRandInit();

  //FADEシステム初期化
  GFL_FADE_Init(GFL_HEAPID_SYSTEM);

  //サウンドシステム初期化
  //GFL_SOUND_Init(&sndHeap[0],SOUND_HEAP_SIZE);

  //バックアップシステム初期化
#ifndef MULTI_BOOT_MAKE
  GFL_BACKUP_Init(HEAPID_SAVE, SaveErrorCall_Load, SaveErrorCall_Save);
#endif //MULTI_BOOT_MAKE

  //システムフォント初期化
  GFL_TEXT_CreateSystem( NULL );
  //RTC初期化
  GFL_RTC_Init();

  GFL_USE_VintrCounter = 0;

  //MCSライブラリを拡張メモリにロード
#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  if( OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER) )
  {
    GFL_OVERLAY_Load( FS_OVERLAY_ID( mcs_lib ) );
    GFL_MCS_Init();
  }
  MCS_USEMODE_Set( MCS_USEMODE_NONE );
#endif //MULTI_BOOT_MAKE
#endif
  GFUser_ResetVIntrFunc();

  GFUser_SetSleepSoundFunc();

  GFL_FADE_SetFadeSpeed( 2 );
}


//------------------------------------------------------------------
/**
 * @brief GFライブラリ利用部分：メイン呼び出し
 *
 * 毎フレーム呼び出される必要があるメイン処理を記述する
 */
//------------------------------------------------------------------
void GFLUser_Main(void)
{
  GFL_UI_Main();

  NET_DEV_Main();


#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  //デバッグシステム
  DEBUGWIN_UpdateSystem();
  //Lを押している間停止。L押しながらRで1コマ送る
  if( DEBUG_PAUSE_Update() == TRUE &&
      DEBUGWIN_IsActive() == FALSE )
#endif MULTI_BOOT_MAKE
#endif
  {
    GFL_PROC_Main();
  }
  GFL_RTC_Main();
  //GFL_SOUND_Main();
  //
#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  //MCS受信
  if( OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER) )
  {
    GFL_MCS_Main();
    GFL_MCS_SNDVIEWER_Main();
    if ( MCS_USEMODE_Get() == MCS_USEMODE_RESIDENT )
    {
      GFL_MCS_Resident();
    }
  }
#endif MULTI_BOOT_MAKE
#endif
}

//------------------------------------------------------------------
/**
 * @brief GFライブラリ利用部分：描画処理
 *
 * 毎フレーム呼び出される必要がある描画処理を記述する
 */
//------------------------------------------------------------------
void GFLUser_Display(void)
{
  //GFL_OAM_sysMain(...);
  //GFL_BG_sysMain(...);
}

//------------------------------------------------------------------
/**
 * @brief GFライブラリ利用部分：終了処理
 */
//------------------------------------------------------------------
void GFLUser_Exit(void)
{
#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  if( OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER) )
  {
    GFL_MCS_Exit();
  }
#endif //MULTI_BOOT_MAKE
#endif
  GFL_TEXT_DeleteSystem();
#ifndef MULTI_BOOT_MAKE
  GFL_BACKUP_Exit();
#endif //MULTI_BOOT_MAKE
  //GFL_SOUND_Exit();
  GFL_FADE_Exit();
  GFL_PROC_Exit();
  GFL_TCB_Exit(gfl_work->TCBSysVintr);
  GFL_HEAP_FreeMemory(gfl_work->TCBSysVintr);
  GFL_HEAP_FreeMemory(gfl_work->TCBMemVintr);
  GFL_OVERLAY_Exit();
  GFL_UI_Exit();
  GFL_HEAP_DeleteHeap( HEAPID_SAVE );
  GFL_HEAP_DTCM_Exit();
  GFL_HEAP_Exit();
}

//------------------------------------------------------------------
/**
 * @brief GFライブラリ利用部分：HBlank割り込み処理
 */
//------------------------------------------------------------------
void GFLUser_HIntr(void)
{
  GFL_TCB_Main(gfl_work->TCBSysHintr);
}

//------------------------------------------------------------------
/**
 * @brief GFライブラリ利用部分：HBlank中TCBの登録処理
 */
//------------------------------------------------------------------
GFL_TCB * GFUser_HIntr_CreateTCB(GFL_TCB_FUNC * func, void * work, u32 pri)
{
  return GFL_TCB_AddTask(gfl_work->TCBSysHintr, func, work, pri);
}

//------------------------------------------------------------------
/**
 * @brief GFライブラリ利用部分：VBlank割り込み処理
 */
//------------------------------------------------------------------
void GFLUser_VIntr(void)
{
  if(GflUseVintrFunc){ GflUseVintrFunc(GflUseVintrWork); }
}

//VBlank割り込み関数の登録
BOOL GFUser_SetVIntrFunc(GFL_USE_VINTR_FUNC func,void *pWork)
{
  if(GflUseVintrFunc){
    GF_ASSERT(0);
    return FALSE;
  }
  GflUseVintrFunc = func;
  GflUseVintrWork = pWork;
  return TRUE;
}

//VBlank割り込み関数の解除
void GFUser_ResetVIntrFunc(void)
{
  GflUseVintrFunc = NULL;
  GflUseVintrWork = NULL;
}

//------------------------------------------------------------------
/**
 * @brief GFライブラリ利用部分：VBlankタイミング処理
 */
//------------------------------------------------------------------
void GFLUser_VTiming(void)
{
  GFL_TCB_Main(gfl_work->TCBSysVintr);
  GFL_BG_VBlankFunc();
  // Vブランク期間で実行します。
  // ただ、ユニットの描画が行われていないのに
  // この関数を実行すると、描画しているOBJが消えてしまうため
  // 割り込みないで呼ばないほうが良いかもしれません。
    //開放しました Ariizumi1031
  //GFL_CLACT_VBlankFunc();
    // 通信アイコンの描画のためにあります。通信自体は行っていません
    //GFL_NET_VBlankFunc();
#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  //MCS受信
  if( OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER) )
  {
    GFL_MCS_VIntrFunc();
  }
#endif //MULTI_BOOT_MAKE
#endif
  GFL_DMA_Main();


#ifdef PM_DEBUG
#ifndef MULTI_BOOT_MAKE
  //Lを押している間停止。L押しながらRで1コマ送る
  if( DEBUG_PAUSE_Update() == TRUE &&
      DEBUGWIN_IsActive() == FALSE )
#endif MULTI_BOOT_MAKE
#endif
  {
    GFL_FADE_Main();  // フェード反映
  }

  GFL_USE_VintrCounter++;
}

//------------------------------------------------------------------
/**
 * @brief GFライブラリ利用部分：VBlank中TCBの登録処理
 */
//------------------------------------------------------------------
GFL_TCB * GFUser_VIntr_CreateTCB(GFL_TCB_FUNC * func, void * work, u32 pri)
{
  return GFL_TCB_AddTask(gfl_work->TCBSysVintr, func, work, pri);
}

//------------------------------------------------------------------
/**
 * @brief GFライブラリ利用部分：VBlank中TCBSYSの取得
 */
//------------------------------------------------------------------
GFL_TCBSYS * GFUser_VIntr_GetTCBSYS( void )
{
  return gfl_work->TCBSysVintr;
}

//------------------------------------------------------------------
/**
 * @brief コンテキストを生成する
 * @param context GFL_STD_RandContextポインタ
 */
//------------------------------------------------------------------
void GFUser_SetStartRandContext(GFL_STD_RandContext *context)
{
  GFL_STD_RandGeneralInit(context);
}

//------------------------------------------------------------------
/**
 * @brief GFUser 汎用乱数作成
 */
//------------------------------------------------------------------
static void GFUser_PublicRandInit(void)
{
  GFUser_SetStartRandContext( &gfl_work->publicRandContext );
  GFL_STD_MtRandInit(GFUser_GetPublicRand(0));
}

//------------------------------------------------------------------
/**
 * @brief  汎用乱数を得る
 * @param  range   取得数値の範囲を指定 0 〜 range-1 の範囲の値が取得できます。
 *                 GFL_STD_RAND_MAX = 0 を指定した場合にはすべての範囲の32bit値となります。
 *                 % や / を使用せずにここのrange値を変更してください
 * @return 32bitのランダム値
 */
//------------------------------------------------------------------
u32 GFUser_GetPublicRand(u32 range)
{
  return GFL_STD_Rand( &gfl_work->publicRandContext, range );
}

//------------------------------------------------------------------
/**
 * @brief  汎用乱数を得る
 * @param  range   取得数値の範囲を指定 0 〜 range-1 の範囲の値が取得できます。
 *                  % や / を使用せずにここのrange値を変更してください
 * @return 32bitのランダム値
 */
//------------------------------------------------------------------
u32 GFUser_GetPublicRand0(u32 range)
{
  return GFL_STD_Rand0( &gfl_work->publicRandContext, range );
}

//------------------------------------------------------------------
/**
 * @brief VBlankカウンター取得とリセット
 */
//------------------------------------------------------------------
void GFUser_VIntr_ResetVblankCounter( void )
{
  GFL_USE_VintrCounter = 0;
}

int GFUser_VIntr_GetVblankCounter( void )
{
  return GFL_USE_VintrCounter;
}

//--------------------------------------------------------------------------
/**
 * @brief ArchiveIDでファイル名を取得する
 */
//--------------------------------------------------------------------------
const char * GFUser_GetFileNameByArcID(ARCID arcID)
{
  GF_ASSERT(arcID < ARCID_TABLE_MAX);
  return ArchiveFileTable[arcID];
}

//--------------------------------------------------------------------------
/**
 * ASSERT停止後に呼び出されるユーザ関数
 *
 * ※ デフォルトだとOS_Haltにより停止してしまい、
 *    コールスタック表示がされないなどの問題があるため用意してあります
 *
 */
//--------------------------------------------------------------------------
void GFUser_AssertFinish( void )
{
  int key_cont;

  while(1){

    GFL_UI_Main();
    key_cont = GFL_UI_KEY_GetCont();

    // キーが押されるまでループ
    if( (key_cont & PAD_BUTTON_L) && (key_cont & PAD_BUTTON_R) &&
      (key_cont & PAD_BUTTON_X) && (key_cont & PAD_BUTTON_Y) ){
      return ;
    }

    // VBlank待ち
    // (これを有効にすると、デバッカで停止させたときにコールスタックが表示されない)
//    OS_WaitIrq(TRUE,OS_IE_V_BLANK);
  }
}

//--------------------------------------------------------------------------
/**
 * MCS常駐アプリへのリクエスト
 */
//--------------------------------------------------------------------------
BOOL GFUser_SendHeapStatus(void);

BOOL GFUser_SendHeapStatus(void)
{
  if( OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER) )
  {
#if PM_DEBUG
    GFL_MCS_Resident_SendHeapStatus();
#endif
    return TRUE;
  }
  return FALSE;
}


//=============================================================================================
//
//  DSのカバーを開閉した時の処理
//
//=============================================================================================
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
static void sleepSoundSetupDefault( void * work )
{
  NNS_SndSetMasterVolume( 0 );
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
static void sleepSoundReleaseDefault( void * work )
{
  NNS_SndSetMasterVolume( 127 );
}
//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
void GFUser_SetSleepSoundFunc( void )
{
  GFL_UI_SleepSoundSetFunc( sleepSoundSetupDefault, sleepSoundReleaseDefault, NULL );
}

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
BOOL GFUser_IsSleepSoundFunc( void )
{
  if( GFL_UI_SleepSoundGoGetFunc() == sleepSoundSetupDefault ){
    GF_ASSERT( GFL_UI_SleepSoundReleaseGetFunc() == sleepSoundReleaseDefault );
    return TRUE;
  }
  return FALSE;
}



