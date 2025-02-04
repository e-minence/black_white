//============================================================================================
/**
 * @file  pm_sndsys.c
 * @brief 
 * @author  
 * @date  
 */
//============================================================================================
#include "gflib.h"
#include "sound/sound_manager.h"

//マルチブート用きり分け
#ifdef MULTI_BOOT_MAKE  //DL子機時処理
  #include "multiboot/wb_sound_palpark.sadl"
  #define GROUP_GLOBAL (0)
  #define SEQ_SE_END (1400)
  // エラー回避用に定義
  #define BANK_MUS_WB_SHINKA  (1010)
  #define SEQ_BGM_SHINKA  (1010)
  #define PLAYER_BGM2 (0)
#endif //MULTI_BOOT_MAKE


#include "sound/pm_sndsys.h"
//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief 
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief ＳＥプレーヤー設定定義
 */
//------------------------------------------------------------------
typedef struct {
  u32           soundIdx;
  NNSSndHandle  sndHandle;
}PMSND_SEPLAYER_DATA;
//------------------------------------------------------------------
/**
 * @brief システムフェード構造定義
 */
//------------------------------------------------------------------
typedef struct {
  BOOL  active;
  int   seq;
  u16   volumeCounter;
  u16   nextTrackBit;
  u32   nextSoundIdx;
  int   fadeInFrame;
  int   fadeOutFrame;
}PMSND_FADESTATUS;

//------------------------------------------------------------------
/**
 * @brief リバーブ構造定義
 */
//------------------------------------------------------------------
#ifdef REVERB_USE
typedef struct {
  BOOL  active;
  u32   samplingRate;
  u16   volume;
  u16   depth;
  int   stopFrames;
}PMSND_REVERB;
#endif
//------------------------------------------------------------------
/**
 * @brief スレッド制御定義
 */
//------------------------------------------------------------------
#ifdef PM_DEBUG
int BGM_BLOCKLOAD_SIZE = 0x1300;
#else
#define BGM_BLOCKLOAD_SIZE  (0x1300)
#endif

typedef enum {
  THREADLOAD_SEQBANK = 0,
  THREADLOAD_WAVE,
}THREADLOAD_MODE;

// スレッド使用引数構造体
typedef struct {
  u32 soundIdx;
}THREAD_ARG;

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief サウンドシステム（main.c等から呼び出される）
 *
 *
 *
 *
 *
 */
//============================================================================================
//マルチブート用きり分け
#ifndef MULTI_BOOT_MAKE //通常時処理
#define SOUND_HEAP_SIZE (0x0a0000 - 30000 - 12288)   //640K - 30000 - 0x3000
#else                   //DL子機時処理
#define SOUND_HEAP_SIZE (0x030000)   //非常に適当
#endif //MULTI_BOOT_MAKE
#define CAPTURE_BUFSIZE (0x2000)
#define TRACK_NUM (16)

static u8                       PmSoundHeap[SOUND_HEAP_SIZE];
#ifdef REVERB_USE
static u8                       captureBuffer[ CAPTURE_BUFSIZE ] ATTRIBUTE_ALIGN(32);
#endif
static NNSSndArc                PmSoundArc;
static NNSSndHeapHandle         PmSndHeapHandle;
static u32                      bgmFadeCounter;
static PMSND_FADESTATUS         fadeStatus;
#ifdef REVERB_USE
static PMSND_REVERB             reverbStatus;
#endif
//static SOUNDMAN_PRESET_HANDLE*  systemPresetHandle;
static SOUNDMAN_PRESET_HANDLE*  usrPresetHandle1;
static PMSND_SEPLAYER_DATA      sePlayerData[SEPLAYER_MAX];
static SNDTrackInfo             trackInfo[TRACK_NUM];
static u16                      trackActiveBit; 

static PMSND_PLAYABLE_CALLBACK  playableCallBackFunc;

//static OSMutex                  sndTreadMutex;    
//
#ifdef PM_DEBUG
static u32 heapRemainsAfterSys;
static u32 heapRemainsAfterPlayer;
static u32 heapRemainsAfterPresetSE;
#endif

#define THREAD_STACKSIZ   (0x0400)
OSThread    soundLoadThread;
static u64  threadStack[ THREAD_STACKSIZ / sizeof(u64) ];
THREAD_ARG  threadArg;
OSThread*   pSoundLoadThread;

static void PMSND_InitCore( BOOL systemSEload );

static void PMSND_InitSEplayer( void );
static void PMSND_InitSystemFadeBGM( void );
static void PMSND_SetSystemFadeBGM( u32 nextSoundIdx, u16 trackBit, u8 outFrame, u8 inFrame );
static void PMSND_ResetSystemFadeBGM( void );
static void PMSND_CancelSystemFadeBGM( void );
static void PMSND_SystemFadeBGM( void );

static void PMSND_InitCaptureReverb( void );

static void createSoundPlayThread( u32 soundIdx, THREADLOAD_MODE mode );
static void deleteSoundPlayThread( void );
static BOOL checkEndSoundPlayThread( void );
//============================================================================================
/**
 *
 * @brief セットアップ＆初期化
 *
 */
//============================================================================================
void  PMSND_Init( void )
{
  // サウンドシステム初期化
  NNS_SndInit();
  PmSndHeapHandle = NNS_SndHeapCreate(PmSoundHeap, SOUND_HEAP_SIZE);

  // サウンドの設定
  NNS_SndArcInitWithResult( &PmSoundArc, "wb_sound_data.sdat", PmSndHeapHandle, FALSE );

  PMSND_InitCore( TRUE );

  // サウンド拡張機能初期化（主にマイクサンプリングのノイズフィルターで使用）
#ifdef SDK_TWL
  SNDEX_Init();
#endif
}

void  PMSND_InitMultiBoot( void* sndData )
{
  // サウンドシステム初期化(マルチブート子機
  // 初期化データは32バイトアライメント
  
  //下外でやる
  //NNS_SndInit();
  PmSndHeapHandle = NNS_SndHeapCreate(PmSoundHeap, SOUND_HEAP_SIZE);

  // サウンドの設定
  NNS_SndArcInitOnMemory( &PmSoundArc, sndData );

  PMSND_InitCore( FALSE );
}

//---------------------------------------------------
//  初期化コア部分
//---------------------------------------------------
static void PMSND_InitCore( BOOL systemSEload )
{
#ifdef PM_DEBUG
  heapRemainsAfterSys = NNS_SndHeapGetFreeSize(PmSndHeapHandle);
#endif
  // サウンドプレーヤーの設定（サウンドデータに定義されているものを参照する）
  NNS_SndArcPlayerSetup(PmSndHeapHandle);
#ifdef PM_DEBUG
  heapRemainsAfterPlayer = NNS_SndHeapGetFreeSize(PmSndHeapHandle);
#endif
  // サウンド管理初期化
  SOUNDMAN_Init(&PmSndHeapHandle);

  // 常駐サウンドデータ読み込み
  if( systemSEload ){
#if 0
    systemPresetHandle = SOUNDMAN_PresetGroup(GROUP_GLOBAL);
#else
    const NNSSndArcGroupInfo* groupInfo;
    u32 groupIdx = GROUP_GLOBAL;

    groupInfo = NNS_SndArcGetGroupInfo(groupIdx);
    NNS_SndArcLoadGroup(groupIdx, PmSndHeapHandle);
#endif
    usrPresetHandle1 = NULL;
#ifdef PM_DEBUG
    heapRemainsAfterPresetSE = NNS_SndHeapGetFreeSize(PmSndHeapHandle);
#endif
  }
  // 階層プレーヤーリセットのヒープＬＶを更新
  //SOUNDMAN_UpdateHierarchyPlayerSoundHeapLv();

  // ＳＥプレーヤーデータ初期化
  PMSND_InitSEplayer();
  // サウンド階層構造初期化
  SOUNDMAN_InitHierarchyPlayer(PLAYER_BGM);

  PMSND_InitSystemFadeBGM();
#ifdef REVERB_USE
  PMSND_InitCaptureReverb();
#endif
  // 排他制御用Mutex初期化(NNS_SndMainがスレッドセーフ設計ではないため)
  //OS_InitMutex(&sndTreadMutex);   

  trackActiveBit = 0; 
  bgmFadeCounter = 0;

  pSoundLoadThread = NULL;
  playableCallBackFunc = NULL;
}


//============================================================================================
/**
 *
 * @brief フレームワーク
 *
 */
//============================================================================================
void  PMSND_Main( void )
{
  PMSND_SystemFadeBGM();

  NNS_SndMain();
  // サウンドドライバ情報更新
  NNS_SndUpdateDriverInfo();
  // 現在のトラック状態を取得
  {
    NNSSndHandle* sndHandle = PMSND_GetBGMhandlePointer();
    int i;

    trackActiveBit = 0;
    for( i=0; i<TRACK_NUM; i++ ){
      if(NNS_SndPlayerReadDriverTrackInfo(sndHandle, i, &trackInfo[i]) == TRUE){
        trackActiveBit |= (0x0001 << i);  // 有効/無効をBitFieldに変換
      }
    }
  }
  if( bgmFadeCounter ){ bgmFadeCounter--; }
}

//============================================================================================
/**
 *
 * @brief 終了処理＆破棄
 *
 */
//============================================================================================
void  PMSND_Exit( void )
{
//マルチブート用きり分け
#ifndef MULTI_BOOT_MAKE  //通常時処理
#if 0
  SOUNDMAN_ReleasePresetData(systemPresetHandle);
#endif
#endif //MULTI_BOOT_MAKE

  NNS_SndHeapDestroy(PmSndHeapHandle);
}

//============================================================================================
/**
 *
 * @brief ステレオ/モノラル設定
 *
 */
//============================================================================================
void  PMSND_SetStereo( BOOL flag )
{
  BOOL setFlag;

  setFlag = (flag == TRUE)? FALSE : TRUE;

  NNS_SndSetMonoFlag( setFlag );
}

//============================================================================================
/**
 *
 * @brief 情報取得
 *
 */
//============================================================================================
#ifdef PM_DEBUG
u32 PMSND_GetSndHeapSize( void )                  { return SOUND_HEAP_SIZE; }
u32 PMSND_GetSndHeapRemainsAfterSys( void )       { return heapRemainsAfterSys; }
u32 PMSND_GetSndHeapRemainsAfterPlayer( void )    { return heapRemainsAfterPlayer; }
u32 PMSND_GetSndHeapRemainsAfterPresetSE( void )  { return heapRemainsAfterPresetSE; }
u32 PMSND_GetSEPlayerNum( void )                  { return SEPLAYER_MAX; }
#endif

u32 PMSND_GetSndHeapFreeSize( void )
{
  return NNS_SndHeapGetFreeSize(PmSndHeapHandle);
}

NNSSndHandle* PMSND_GetNowSndHandlePointer( void )
{
  int i;
  for( i=0; i<SEPLAYER_MAX; i++ ){
    if( PMSND_CheckPlaySE_byPlayerID((SEPLAYER_ID)i) == TRUE ){
      return &sePlayerData[i].sndHandle;
    }
  }
  return SOUNDMAN_GetHierarchyPlayerSndHandle();
}

NNSSndHandle* PMSND_GetBGMhandlePointer( void )
{
  return SOUNDMAN_GetHierarchyPlayerSndHandle();
}

u32 PMSND_GetBGMsoundNo( void )
{
  return SOUNDMAN_GetHierarchyPlayerSoundIdx();
}

u32 PMSND_GetNextBGMsoundNo( void )
{
  if( fadeStatus.active == FALSE ){
    return SOUNDMAN_GetHierarchyPlayerSoundIdx();
  }
  return fadeStatus.nextSoundIdx;
}

u32 PMSND_GetBGMplayerNoIdx( void )
{
  if( PMSND_CheckPlaySE() == TRUE ){
    return 0;
  }
  return  SOUNDMAN_GetHierarchyPlayerPlayerNoIdx() + 1;
}
 
BOOL PMSND_CheckOnReverb( void )
{
  //return reverbStatus.active;
  return FALSE;		// 機能削除
}

u8 PMSND_GetBGMTrackVolume( int trackNo )
{
  if(trackNo >= TRACK_NUM){ return 0; }
  if(trackActiveBit & (0x0001 << trackNo)){
    return trackInfo[TRACK_NUM].volume;
  }
  return 0;
}

BOOL PMSND_IsLoading( void )
{
  if(pSoundLoadThread == NULL){ return FALSE; }
  return OS_IsThreadTerminated(&soundLoadThread) == FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  サウンド分割読み込みスレッド　CARDカードアクセス中チェック
 *
 *	サウンド分割読み込みスレッド内でのCARDアクセス中にメインスレッドに戻ってきた状態をチェックする関数
 *	注意点は２点
 *	１．処理タイミングによっては、VBlank期間中にカードアクセス完了の割り込みが発生し、
 *	　　本来VBlank期間中に行わなければならない処理がVBlank期間中に行われない可能性がある。
 *	
 *	２・サウンドデータがカードにアクセスしているため、その状態でメインスレッドに戻ってきた際には
 *	　　メインスレッドからカードにアクセスすることが出来ない。
 *
 *	１，２に当てはまる処理では、この関数を使用し、１フレーム処理をスキップする必要があります。
 *
 *	＊さらに、スレッドに関しての知識不足なため、上記内容に誤まったものがある可能性もあります。
 *	　2010.05.21の状態では本関数を使用することで、不具合の対処は出来ています。　tomoya takahashi
 *  
 *	@retval TRUE    CARDアクセス中
 *	@retval FALSE   通常動作
 */
//-----------------------------------------------------------------------------
BOOL PMSND_IsAccessCARD( void )
{
#if (defined(SDK_TWL))
  if( ((OS_GetIrqMask() & OS_IE_CARD_A_DATA) == 0)  ){
    return FALSE;
  }
  return TRUE;
#else
  return FALSE;
#endif
}

//============================================================================================
/**
 *
 * @brief プレーヤーの音量コントロール(ON/OFF)
 *          ※現状32bit = 32プレーヤーまで対応
 *          in: bitmask : 最下位bitから0〜のプレーヤー指定
 */
//============================================================================================
void PMSND_AllPlayerVolumeEnable( BOOL playerON, u32 bitmask )
{
  if(playerON == TRUE){
		PMSND_AllPlayerVolumeSet( 127, bitmask );
  } else {
		PMSND_AllPlayerVolumeSet( 0, bitmask );
  }
}

void PMSND_AllPlayerVolumeSet( u8 volume, u32 bitmask )
{
  int i;

  for(i=0; i<PMSND_PLAYER_MAX; i++){
    if(bitmask & (1<<i)){
      NNS_SndPlayerSetPlayerVolume( i, volume );
    }
  }
}

#ifdef REVERB_USE
//============================================================================================
/**
 *
 * @brief キャプチャー関数
 *
 */
//============================================================================================
static void PMSND_InitCaptureReverb( void )
{
  reverbStatus.active = FALSE;
  reverbStatus.samplingRate = 16000;
  reverbStatus.volume = 0;
  reverbStatus.depth = CAPTURE_BUFSIZE;
  reverbStatus.stopFrames = 0;
}

//------------------------------------------------------------------
/**
 * @brief リバーブ設定
 */
//------------------------------------------------------------------
void PMSND_EnableCaptureReverb( u32 depth, u32 samplingRate, int volume, int stopFrames )
{
  BOOL result;

  if(reverbStatus.active == TRUE){ return; }

  reverbStatus.samplingRate = samplingRate;
  if(volume > 63){ volume = 63; }
  reverbStatus.volume = volume;
  if(depth > CAPTURE_BUFSIZE){ depth = CAPTURE_BUFSIZE; }
  reverbStatus.depth = depth;
  reverbStatus.stopFrames = stopFrames;

  result = NNS_SndCaptureStartReverb( captureBuffer, 
                    reverbStatus.depth,
                    NNS_SND_CAPTURE_FORMAT_PCM16, 
                    reverbStatus.samplingRate,
                    reverbStatus.volume);
  if(result == TRUE){ reverbStatus.active = TRUE; }
}

void PMSND_DisableCaptureReverb( void )
{
  if(reverbStatus.active == FALSE){ return; }

  NNS_SndCaptureStopReverb(reverbStatus.stopFrames);
  reverbStatus.active = FALSE;
}

void PMSND_ChangeCaptureReverb( u32 depth, u32 samplingRate, int volume, int stopFrames )
{
  BOOL restartFlag = FALSE;

  if(depth != PMSND_NOEFFECT){
    reverbStatus.depth = depth;
    restartFlag = TRUE;
  }
  if(samplingRate != PMSND_NOEFFECT){
    reverbStatus.samplingRate = samplingRate;
    restartFlag = TRUE;
  }
  if(volume != PMSND_NOEFFECT){
    reverbStatus.volume = volume;
    restartFlag = TRUE;
  }
  if(stopFrames != PMSND_NOEFFECT){
    reverbStatus.stopFrames = stopFrames;
  }
  if(restartFlag == TRUE){
    NNS_SndCaptureStopReverb(0);
    reverbStatus.active = FALSE;
    PMSND_EnableCaptureReverb(  reverbStatus.depth, reverbStatus.samplingRate, 
                  reverbStatus.volume, reverbStatus.stopFrames );
  }
}
#endif





//============================================================================================
/**
 *
 *
 * @brief サウンド再生可否判定
 *
 *
 */
//============================================================================================
static BOOL checkPlaySound( u32 soundIdx )
{
  // ロードスレッド動作中は再生しない
  if( PMSND_IsLoading() == FALSE ) { return TRUE; }

  if(playableCallBackFunc == NULL){ return FALSE; }

  return playableCallBackFunc(soundIdx);
}

void PMSND_SetPlayableCallBack( PMSND_PLAYABLE_CALLBACK func )
{
  playableCallBackFunc = func;
}

void PMSND_ResetPlayableCallBack( void )
{
  playableCallBackFunc = NULL;
}

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief ＢＧＭサウンド関数（各appから呼び出される）
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief サウンド停止
 */
//------------------------------------------------------------------
static void PMSND_StopBGM_CORE( void )
{
  NNS_SndPlayerStopSeq(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0);
  SOUNDMAN_UnloadHierarchyPlayer();
}
//------------------------------------------------------------------
void  PMSND_StopBGM( void )
{
  PMSND_ResetSystemFadeBGM();
  PMSND_StopBGM_CORE();
}

//------------------------------------------------------------------
/**
 * @brief サウンド再生
 */
//------------------------------------------------------------------
static BOOL PMSND_PlayBGM_CORE( u32 soundIdx, u16 trackBit )
{
  BOOL result;

  // ロードスレッド動作中は再生しない
  //if( PMSND_IsLoading() ) { return FALSE; }
  // 再生可否判定
  if(checkPlaySound(soundIdx) == FALSE){ return FALSE; }

  PMSND_StopBGM_CORE();
  result = SOUNDMAN_LoadHierarchyPlayer(soundIdx);
  if( result == FALSE){ return FALSE; }

  // サウンド再生開始
  result = NNS_SndArcPlayerStartSeqEx
          (SOUNDMAN_GetHierarchyPlayerSndHandle(), PLAYER_BGM, -1, -1, soundIdx);
  if( result == FALSE){ return FALSE; }

  if( trackBit != 0xffff ){ PMSND_ChangeBGMtrack( trackBit ); }

  return result;
}

//------------------------------------------------------------------
void  PMSND_PlayBGM_EX( u32 soundIdx, u16 trackBit )
{
  PMSND_ResetSystemFadeBGM();

  PMSND_PlayBGM_CORE( soundIdx, trackBit );
}

//------------------------------------------------------------------
void  PMSND_PlayNextBGM_EX( u32 soundIdx, u16 trackBit, u8 fadeOutFrame, u8 fadeInFrame )
{
  PMSND_SetSystemFadeBGM(soundIdx, trackBit, fadeOutFrame, fadeInFrame );
}

//------------------------------------------------------------------
void  PMSND_PlayBGM_WideChannel( u32 soundIdx )
{
  BOOL result;
  PMSND_ResetSystemFadeBGM();

  // 再生可否判定
  if(checkPlaySound(soundIdx) == FALSE){ return; }
  PMSND_StopBGM_CORE();
  result = SOUNDMAN_LoadHierarchyPlayer(soundIdx);
  if( result == FALSE){ return; }

  // サウンド再生開始
  result = NNS_SndArcPlayerStartSeqEx
          (SOUNDMAN_GetHierarchyPlayerSndHandle(), PLAYER_BGM2, -1, -1, soundIdx);
}

//------------------------------------------------------------------
/**
 * @brief サウンド一時停止
 */
//------------------------------------------------------------------
void  PMSND_PauseBGM( BOOL pauseFlag )
{
  PMSND_CancelSystemFadeBGM();
  NNS_SndPlayerPause(SOUNDMAN_GetHierarchyPlayerSndHandle(), pauseFlag);
}

//------------------------------------------------------------------
/**
 * @brief サウンドフェードイン
 */
//------------------------------------------------------------------
void  PMSND_FadeInBGM( u16 frames )
{
#if 0
  PMSND_CancelSystemFadeBGM();
  // 現在のvolumeを即時更新
  NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0, 0);
  // 再度目標値を設定
  NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 127, frames);

  bgmFadeCounter = frames;
#else
	PMSND_FadeInBGMEx( frames, 127 );
#endif
}

void  PMSND_FadeInBGMEx( u16 frames, u8 vol )
{
  PMSND_CancelSystemFadeBGM();
  // 現在のvolumeを即時更新
  NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0, 0);
  // 再度目標値を設定
  NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), vol, frames);

  bgmFadeCounter = frames;
}

//------------------------------------------------------------------
/**
 * @brief サウンドフェードアウト
 */
//------------------------------------------------------------------
void  PMSND_FadeOutBGM( u16 frames )
{
  PMSND_CancelSystemFadeBGM();
  NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0, frames);

  bgmFadeCounter = frames;
}

//------------------------------------------------------------------
/**
 * @brief サウンドフェードアウト検出
 */
//------------------------------------------------------------------
BOOL  PMSND_CheckFadeOnBGM( void )
{
  if( bgmFadeCounter ){
    return TRUE;
  } else {
    return FALSE;
  }
}

//------------------------------------------------------------------
/**
 * @brief サウンド状態保管
 */
//------------------------------------------------------------------
void  PMSND_PushBGM( void )
{
  PMSND_CancelSystemFadeBGM();

  // 保管の際はトラックパラメータはリセットしておく。
  // ※そのままの状態で保管出来るのだが、各パラメータ値の取得が出来なく
  //  sound_manager側に管理させるとトラック全ての情報を持つことになってしまうので
  // 現状では設定管理はユーザー側で行うようにする。
  NNS_SndPlayerSetTrackMute(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0xffff, FALSE );
  NNS_SndPlayerSetTrackModDepth(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0xffff, 0 );
  NNS_SndPlayerSetTrackModSpeed(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0xffff, 16 );

  SOUNDMAN_PushHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief サウンド状態取り出し
 */
//------------------------------------------------------------------
void  PMSND_PopBGM( void )
{
  PMSND_CancelSystemFadeBGM();
  if( SOUNDMAN_GetHierarchyPlayerPlayerNoIdx() == 0 ){ return; }

  PMSND_StopBGM_CORE();
  SOUNDMAN_PopHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief ＢＧＭ再生トラック変更
 */
//------------------------------------------------------------------
void  PMSND_ChangeBGMtrack( u16 trackBit )
{
  u16   bitMask = trackBit^0xffff;

  NNS_SndPlayerSetTrackMute(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0xffff, FALSE );
  if(bitMask){
    NNS_SndPlayerSetTrackMuteEx
      (SOUNDMAN_GetHierarchyPlayerSndHandle(), bitMask, NNS_SND_SEQ_MUTE_NO_STOP );
  }
}

//------------------------------------------------------------------
/**
 * @brief ＢＧＭステータス変更
 */
//------------------------------------------------------------------
void  PMSND_SetStatusBGM_EX( u16 trackBit, int tempoRatio, int pitch, int pan )
{
  NNSSndHandle* pBgmHandle = SOUNDMAN_GetHierarchyPlayerSndHandle();

  if(tempoRatio != PMSND_NOEFFECT){
    NNS_SndPlayerSetTempoRatio(pBgmHandle, tempoRatio);
  }
  if(pitch != PMSND_NOEFFECT){
    NNS_SndPlayerSetTrackPitch(pBgmHandle, trackBit, pitch);
  }
  if(pan != PMSND_NOEFFECT){
    NNS_SndPlayerSetTrackPan(pBgmHandle, trackBit, pan);
  }
}

//------------------------------------------------------------------
/**
 * @brief   ＢＧＭボリューム変更
 */ 
//------------------------------------------------------------------
void PMSND_ChangeBGMVolume( u16 trackBit, int volume )
{
  NNSSndHandle* pBgmHandle = SOUNDMAN_GetHierarchyPlayerSndHandle();
  NNS_SndPlayerSetTrackVolume( pBgmHandle, trackBit, volume );
}

//------------------------------------------------------------------
/**
 * @brief ＢＧＭ終了検出
 */
//------------------------------------------------------------------
BOOL  PMSND_CheckPlayBGM( void )
{
  u32 soundIdx = SOUNDMAN_GetHierarchyPlayerSoundIdx();
  int count;

  if(soundIdx == 0){ return FALSE; }

  count = NNS_SndPlayerCountPlayingSeqBySeqNo(SOUNDMAN_GetHierarchyPlayerSoundIdx());

  if( count ){
    return TRUE;
  } else {
    return FALSE;
  }
}


//------------------------------------------------------------------
/**
 *
 * @brief システムフェードＢＧＭ
 *
 */
//------------------------------------------------------------------
static void PMSND_InitSystemFadeBGM( void )
{
  fadeStatus.fadeInFrame = 60;
  fadeStatus.fadeOutFrame = 60;
  PMSND_ResetSystemFadeBGM();
}

static void PMSND_SetSystemFadeBGM( u32 nextSoundIdx, u16 trackBit, u8 outFrame, u8 inFrame )
{
  fadeStatus.nextTrackBit = trackBit;
  fadeStatus.nextSoundIdx = nextSoundIdx;
  fadeStatus.fadeInFrame = inFrame;
  fadeStatus.fadeOutFrame = outFrame;
  fadeStatus.volumeCounter = outFrame;

  deleteSoundPlayThread();

  fadeStatus.seq = 0;
  fadeStatus.active = TRUE;
}

static void PMSND_ResetSystemFadeBGM( void )
{
  fadeStatus.active = FALSE;
  fadeStatus.nextTrackBit = 0xffff;
  fadeStatus.nextSoundIdx = 0;
  fadeStatus.volumeCounter = 0;//fadeStatus.fadeFrames;
  //最大値に更新
  NNS_SndPlayerSetVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 127);

  fadeStatus.seq = 0;
  deleteSoundPlayThread();
}

static void PMSND_CancelSystemFadeBGM( void )
{
  if(fadeStatus.active == TRUE){
    u32 nowSoundIdx = SOUNDMAN_GetHierarchyPlayerSoundIdx();
    if( nowSoundIdx != fadeStatus.nextSoundIdx ){
      while(fadeStatus.seq != 0){
        PMSND_SystemFadeBGM();
        //OS_Printf("サウンド読み込みが完了していないのに一時停止や階層操作をしようとしている\n");
      }
      PMSND_PlayBGM_EX(fadeStatus.nextSoundIdx, fadeStatus.nextTrackBit);
    }
    PMSND_ResetSystemFadeBGM();
  }
}

//------------------------------------------------------------------
//int debugCounter;

static void PMSND_SystemFadeBGM( void )
{
  NNSSndHandle* pBgmHandle = SOUNDMAN_GetHierarchyPlayerSndHandle();
  u32       nowSoundIdx = SOUNDMAN_GetHierarchyPlayerSoundIdx();
  BOOL      bgmSetFlag = FALSE;

  if( fadeStatus.active == FALSE ){ return; }

  switch( fadeStatus.seq ){
  case 0:
    if(nowSoundIdx){
      if( nowSoundIdx != fadeStatus.nextSoundIdx ){
        //FADEOUT
        if( fadeStatus.volumeCounter > 0 ){
          fadeStatus.volumeCounter--;
          NNS_SndPlayerSetVolume(pBgmHandle, fadeStatus.volumeCounter*127/fadeStatus.fadeOutFrame);
          return;
        }
        NNS_SndPlayerSetVolume(pBgmHandle, 0);
      } else {                  
        //FADEIN
        if( fadeStatus.volumeCounter < fadeStatus.fadeInFrame ){
          fadeStatus.volumeCounter++;
          NNS_SndPlayerSetVolume(pBgmHandle, fadeStatus.volumeCounter*127/fadeStatus.fadeInFrame);
          return;
        }
        PMSND_ResetSystemFadeBGM();
        return;     //終了
      }
    }
    PMSND_StopBGM_CORE();

    if(fadeStatus.nextSoundIdx == 0){
      PMSND_ResetSystemFadeBGM();
      return;     //終了
    }
    NNS_SndArcSetLoadBlockSize(BGM_BLOCKLOAD_SIZE); //分割ロード指定

    createSoundPlayThread(fadeStatus.nextSoundIdx, THREADLOAD_SEQBANK);
    fadeStatus.volumeCounter = 0;
    fadeStatus.seq = 1;
    break;
  case 1:
    if(checkEndSoundPlayThread() == TRUE){
      SOUNDMAN_LoadHierarchyPlayer_forThread_heapsvSB();// サウンド階層構造用設定

      createSoundPlayThread( fadeStatus.nextSoundIdx, THREADLOAD_WAVE);
      fadeStatus.seq = 2;
    } else {
      OS_Sleep(1);
    }
    break;
  case 2:
    if(checkEndSoundPlayThread() == TRUE){
      NNS_SndArcSetLoadBlockSize(0);  //分割ロードなしに復帰

      SOUNDMAN_LoadHierarchyPlayer_forThread_end(fadeStatus.nextSoundIdx);

      // サウンド再生開始
      NNS_SndArcPlayerStartSeqEx
          (SOUNDMAN_GetHierarchyPlayerSndHandle(), PLAYER_BGM, -1, -1, fadeStatus.nextSoundIdx);
      NNS_SndPlayerSetVolume(pBgmHandle, 0);
      fadeStatus.seq = 0;
    } else {
      OS_Sleep(1);
    }
    break;
  }
}

//------------------------------------------------------------------
/**
 * @brief システムフェードフレーム設定
 */
//------------------------------------------------------------------
void PMSND_SetSystemFadeFrames( int fadeOutFrame, int fadeInFrame )
{
  fadeStatus.fadeInFrame = fadeInFrame;
  fadeStatus.fadeOutFrame = fadeOutFrame;
}





//============================================================================================
/**
 *
 *
 *
 * @brief ＳＥサウンド関数（各appから呼び出される）
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief ＳＥプレーヤーデータ初期化
 */
//------------------------------------------------------------------
static void PMSND_InitSEplayer( void )
{
  int i;

  for( i=0; i<SEPLAYER_MAX; i++ ){
    sePlayerData[i].soundIdx = 0;
    NNS_SndHandleInit(&sePlayerData[i].sndHandle);
  }
}

//------------------------------------------------------------------
/**
 * @brief ＳＥプレーヤーハンドル取得
 */
//------------------------------------------------------------------
NNSSndHandle* PMSND_GetSE_SndHandle( SEPLAYER_ID sePlayerID )
{
  return &sePlayerData[sePlayerID].sndHandle;
}

//------------------------------------------------------------------
/**
 * @brief ＳＥプレーヤー初期設定ＩＤ取得
 */
//------------------------------------------------------------------
SEPLAYER_ID PMSND_GetSE_DefaultPlayerID( u32 soundIdx )
{
  const NNSSndSeqParam* seqParam;
  if (soundIdx < PMSND_SE_START || soundIdx >= PMSND_SE_END) {
    GF_ASSERT_MSG(0, "ID(%d) is not SE ID!!", soundIdx);
    return SEPLAYER_SE1;
  }
  seqParam = NNS_SndArcGetSeqParam( soundIdx );

  return (SEPLAYER_ID)(seqParam->playerNo - PLAYER_SE_SYS);
}

//------------------------------------------------------------------
/**
 * @brief ＳＥサウンド再生関数
 */
//------------------------------------------------------------------
void  PMSND_PlaySE_byPlayerID( u32 soundIdx, SEPLAYER_ID sePlayerID )
{
  int playerNo = sePlayerID + PLAYER_SE_SYS;
  BOOL result;

  // ロードスレッド動作中は再生しない
  //if( PMSND_IsLoading() ) { return; }
  // 再生可否判定
  if(checkPlaySound(soundIdx) == FALSE){ return; }

  sePlayerData[sePlayerID].soundIdx = 0;
  result = NNS_SndArcPlayerStartSeqEx
    (&sePlayerData[sePlayerID].sndHandle, playerNo, -1, -1, soundIdx);
  if(result == TRUE){ sePlayerData[sePlayerID].soundIdx = soundIdx; }
}

static void pmsnd_PlaySECore( u32 soundIdx, u32 volume )
{
  BOOL result;
  SEPLAYER_ID sePlayerID;

  // ロードスレッド動作中は再生しない
  //if( PMSND_IsLoading() ) { return; }
  // 再生可否判定
  if(checkPlaySound(soundIdx) == FALSE){ return; }

  sePlayerID = PMSND_GetSE_DefaultPlayerID(soundIdx);
  sePlayerData[sePlayerID].soundIdx = 0;
  result = NNS_SndArcPlayerStartSeq(&sePlayerData[sePlayerID].sndHandle, soundIdx);
  if(result == TRUE){
    sePlayerData[sePlayerID].soundIdx = soundIdx;
    if(volume < 128){
      PMSND_PlayerSetVolume( sePlayerID, volume );
    }
  }
}

void  PMSND_PlaySE( u32 soundIdx )
{
  pmsnd_PlaySECore( soundIdx, 0xFFFFFFFF );
}
/*
 *  @brief  SEをボリューム指定付きで再生
 *
 *  @param  soundIdx  再生したいSENo
 *  @param  volume    指定ボリューム(有効値 0-127 デフォルトは127)
 */
void  PMSND_PlaySEVolume( u32 soundIdx, u32 volume )
{
  pmsnd_PlaySECore( soundIdx, volume );
}

//------------------------------------------------------------------
/**
 * @brief ＳＥサウンド停止関数
 */
//------------------------------------------------------------------
void  PMSND_StopSE_byPlayerID( SEPLAYER_ID sePlayerID )
{
  NNS_SndPlayerStopSeq(&sePlayerData[sePlayerID].sndHandle, 0);
}

void  PMSND_StopSE( void )
{
  int i;

  for( i=0; i<SEPLAYER_MAX; i++ ){ PMSND_StopSE_byPlayerID((SEPLAYER_ID)i); }
}

//------------------------------------------------------------------
/**
 * @brief ＳＥ終了検出
 */
//------------------------------------------------------------------
BOOL  PMSND_CheckPlaySE_byPlayerID( SEPLAYER_ID sePlayerID )
{
  int playerNo = sePlayerID + PLAYER_SE_SYS;
  int count = NNS_SndPlayerCountPlayingSeqByPlayerNo(playerNo);

  if( count ) { return TRUE; }
  return FALSE;
}

BOOL  PMSND_CheckPlaySE( void )
{
  int i;

  for( i=0; i<SEPLAYER_MAX; i++ ){
    if( PMSND_CheckPlaySE_byPlayerID((SEPLAYER_ID)i) == TRUE ){ return TRUE; }
  }
  return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief ＳＥ再生検出
 */
//------------------------------------------------------------------
BOOL  PMSND_CheckPlayingSEIdx( u32 soundIdx )
{
  int count = NNS_SndPlayerCountPlayingSeqBySeqNo( soundIdx );

  if( count ) { return TRUE; }
  return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief ＳＥステータス変更
 */
//------------------------------------------------------------------
void  PMSND_SetStatusSE_byPlayerID( SEPLAYER_ID sePlayerID, int tempoRatio, int pitch, int pan )
{
  if(tempoRatio != PMSND_NOEFFECT){
    NNS_SndPlayerSetTempoRatio(&sePlayerData[sePlayerID].sndHandle, tempoRatio);
  }
  if(pitch != PMSND_NOEFFECT){
    NNS_SndPlayerSetTrackPitch(&sePlayerData[sePlayerID].sndHandle, 0xffff, pitch);
  }
  if(pan != PMSND_NOEFFECT){
    NNS_SndPlayerSetTrackPan(&sePlayerData[sePlayerID].sndHandle, 0xffff, pan);
  }
}

void  PMSND_SetStatusSE( int tempoRatio, int pitch, int pan )
{
  int i;

  for( i=0; i<SEPLAYER_MAX; i++ ){
    PMSND_SetStatusSE_byPlayerID((SEPLAYER_ID)i, tempoRatio, pitch, pan );
  }
}



/***
 * ボリューム操作 
 *  SEのボリューム
 *    SEのボリュームは、シーケンスボリューム＋イニシャルボリューム　
 *    2つのボリュームで操作することが出来ます。
 *
 * シーケンスボリューム操作
 *    PMSND_PlayerSetVolume
 *    シーケンスのボリュームを設定します。
 * 
 * イニシャルボリューム操作
 *    PMSND_PlayerSetInitialVolume
 *    サウンド製作者が設定したボリュームをプログラム側で上書きすることが出来ます。
 * 
 * 
 * "イニシャルボリュームを操作すると、サウンド製作者の意図したボリュームバランスが変更されて
 * しまいます。イニシャルボリューム使用時には、サウンド製作者の確認を取るようにしてください。"
 *  
 ****/
//--------------------------------------------------------------
/**
 * @brief シーケンスのボリューム設定
 *
 * @param p   サウンドハンドルのアドレス
 * @param vol   ボリューム(0-127)
 *
 * @retval  none
 */
//--------------------------------------------------------------
void PMSND_PlayerSetVolume( SEPLAYER_ID sePlayerID, u32 vol )
{
  //エラー回避
  if( vol > 127 ){
    vol = 127;
  }

  NNS_SndPlayerSetVolume( &sePlayerData[sePlayerID].sndHandle, vol );
}


//--------------------------------------------------------------
/**
 * @brief シーケンスのイニシャルボリューム設定
 *
 * @param p   サウンドハンドルのアドレス
 * @param vol   ボリューム(0-127)
 *
 * @retval  none
 */
//--------------------------------------------------------------
void PMSND_PlayerSetInitialVolume( SEPLAYER_ID sePlayerID, u32 vol )
{
  //エラー回避
  if( vol > 127 ){
    vol = 127;
  }

  NNS_SndPlayerSetInitialVolume( &sePlayerData[sePlayerID].sndHandle, vol );
}


#if 0
//============================================================================================
/**
 *
 *
 * @brief プリセット関数ラッパー
 *          システムでハンドルを１つだけ用意し簡易登録可能にする
 *
 *
 */
//============================================================================================
void  PMDSND_PresetSoundTbl( const u32* soundIdxTbl, u32 tblNum )
{
  PMSND_ReleasePreset();
  usrPresetHandle1 = SOUNDMAN_PresetSoundTbl(soundIdxTbl, tblNum);
}

void  PMSND_PresetGroup( u32 groupIdx )
{
  PMSND_ReleasePreset();
  usrPresetHandle1 = SOUNDMAN_PresetGroup(groupIdx);
}

void PMSND_ReleasePreset( void )
{
  PMSND_StopBGM_CORE();

  if( usrPresetHandle1 != NULL ){
    SOUNDMAN_ReleasePresetData(usrPresetHandle1);
    usrPresetHandle1 = NULL;
  }
}
#endif

//============================================================================================
/**
 *
 *
 * @brief サウンドファイル分割ロードスレッド
 *
 *
 */
//============================================================================================
//スレッド関数
/// BGMの分割読み込みのサイズ  
//--------------------------------------------------------------------------------------------
static void _loadSeqBankThread( void* arg )
{
  THREAD_ARG* arg1 = (THREAD_ARG*)arg;
  BOOL result;

  // サウンドデータ（seq, bank）読み込み
  result = NNS_SndArcLoadSeqEx
    (arg1->soundIdx, NNS_SND_ARC_LOAD_SEQ | NNS_SND_ARC_LOAD_BANK, PmSndHeapHandle);
  //if( result == FALSE){ OS_Printf("sound seqbank load error!\n"); }
}

static void _loadWaveThread( void* arg )
{
  THREAD_ARG* arg1 = (THREAD_ARG*)arg;
  BOOL result;

  // サウンドデータ（wave）読み込み
  result = NNS_SndArcLoadSeqEx(arg1->soundIdx, NNS_SND_ARC_LOAD_WAVE, PmSndHeapHandle);
  //if( result == FALSE){ OS_Printf("sound wave load error!\n"); }
}

static void createSoundPlayThread( u32 soundIdx, THREADLOAD_MODE mode )
{
  threadArg.soundIdx = soundIdx;

  deleteSoundPlayThread();
  switch( mode ){
  case THREADLOAD_SEQBANK:
    OS_CreateThread(&soundLoadThread, 
                    _loadSeqBankThread,
                    &threadArg,
                    threadStack + (THREAD_STACKSIZ/sizeof(u64)),
                    THREAD_STACKSIZ,
                    17);
    break;
  case THREADLOAD_WAVE:
    OS_CreateThread(&soundLoadThread, 
                    _loadWaveThread,
                    &threadArg,
                    threadStack + (THREAD_STACKSIZ/sizeof(u64)),
                    THREAD_STACKSIZ,
                    17);
    break;
  }
  OS_WakeupThreadDirect(&soundLoadThread);
  pSoundLoadThread = &soundLoadThread;
}

static void deleteSoundPlayThread( void )
{
  if(pSoundLoadThread == NULL){ return; }
  if( OS_IsThreadTerminated(&soundLoadThread) == FALSE ){
    OS_DestroyThread(&soundLoadThread);
    pSoundLoadThread = NULL;
  }
}

static BOOL checkEndSoundPlayThread( void )
{
  if(pSoundLoadThread == NULL){ return TRUE; }
  return OS_IsThreadTerminated(&soundLoadThread);
}


//-------------------------------------------------------------------------------------------- 
/**
 * @brief 分割ロード・再生
 *
 * @param no    再生するBGMナンバー
 * @param seq   シーケンス管理ポインタ
 * @param start TRUE で分割ロード開始, FALSE でシーケンスを進める
 *
 * @return 再生を開始したら TRUE
 */
//--------------------------------------------------------------------------------------------
BOOL PMSND_PlayBGMdiv(u32 no, u32* seq, BOOL start)
{
  if(start == TRUE){
    PMSND_StopBGM_CORE();
    deleteSoundPlayThread();
    (*seq) = 0; 
  } else {
    switch(*seq){
    case 0:
      NNS_SndArcSetLoadBlockSize(BGM_BLOCKLOAD_SIZE); //分割ロード指定
      createSoundPlayThread(no, THREADLOAD_SEQBANK);
      (*seq)++;
      break;
    case 1:
      if(checkEndSoundPlayThread() == TRUE){
        SOUNDMAN_LoadHierarchyPlayer_forThread_heapsvSB();// サウンド階層構造用設定

        createSoundPlayThread( no, THREADLOAD_WAVE);
        (*seq)++;
      } else {
        OS_Sleep(2);
      }
      break;
    case 2:
      if(checkEndSoundPlayThread() == TRUE){
        NNS_SndArcSetLoadBlockSize(0);  //分割ロードなしに復帰
  
        SOUNDMAN_LoadHierarchyPlayer_forThread_end(no);
  
        // サウンド再生開始
        {
          NNSSndHandle* pBgmHandle = SOUNDMAN_GetHierarchyPlayerSndHandle();

          NNS_SndArcPlayerStartSeqEx(pBgmHandle, PLAYER_BGM, -1, -1, no);
          //NNS_SndPlayerSetVolume(pBgmHandle, 0);
        }
        (*seq)++;
        return TRUE;
      } else {
        OS_Sleep(2);
      }
      break;
    }
  }
  return FALSE;
} 


//============================================================================================
/**
 *
 *
 * @brief 外部音楽データ再生
 *
 *
 */
//============================================================================================
#if 0
BOOL  PMDSND_PresetExtraMusic( void* seqAdrs, void* bnkAdrs, u32 waveNo )
{
  SOUNDMAN_LoadHierarchyPlayer_forThread_heapsvSB();

  // 事前に波形読み込み
  if(NNS_SndArcLoadWaveArc(waveNo, PmSndHeapHandle) == FALSE ){
    return FALSE;   // 波形読み込み失敗
  }
  // ダミー音楽のFileIDを入力アドレスに置き換える
  {
    u32 bnkFileID = NNS_SndArcGetBankInfo(BANK_MUS_WB_SHINKA)->fileId;
    u32 seqFileID = NNS_SndArcGetSeqInfo(SEQ_BGM_SHINKA)->fileId;

    NNS_SndArcSetFileAddress(bnkFileID, bnkAdrs); 
    NNS_SndArcSetFileAddress(seqFileID, seqAdrs); 
  }
  SOUNDMAN_LoadHierarchyPlayer_forThread_end(SEQ_BGM_SHINKA);
  {
    // 無理やりバンク情報を書き換える
    NNSSndArcBankInfo* bnkInfo = (NNSSndArcBankInfo*)NNS_SndArcGetBankInfo(BANK_MUS_WB_SHINKA);
    bnkInfo->waveArcNo[0] = waveNo;
    bnkInfo->waveArcNo[1] = 0xffff;
    bnkInfo->waveArcNo[2] = 0xffff;
    bnkInfo->waveArcNo[3] = 0xffff;
  }
  return TRUE;
}
#endif

BOOL  PMDSND_PresetExtraMusic( void* seqAdrs, void* bnkAdrs, u32 dummyNo )
{
  const NNSSndArcSeqInfo* seqInfo = NNS_SndArcGetSeqInfo(dummyNo);
  u32 seqFileID = seqInfo->fileId;
  u32 bnkNo = seqInfo->param.bankNo;

  const NNSSndArcBankInfo* bnkInfo = NNS_SndArcGetBankInfo(bnkNo);
  u32 bnkFileID = bnkInfo->fileId;
  int i;

  SOUNDMAN_LoadHierarchyPlayer_forThread_heapsvSB();

  // 事前に波形読み込み
  for(i=0; i<4; i++){
    if(bnkInfo->waveArcNo[i] != 0xFFFF ){
      if(NNS_SndArcLoadWaveArc(bnkInfo->waveArcNo[i], PmSndHeapHandle) == FALSE ){
        return FALSE;   // 波形読み込み失敗
      }
      //OS_Printf("外部BGM再生...事前波形ロード waveID(%d)\n", bnkInfo->waveArcNo[i]);
    }
  }
  // ダミー音楽のFileIDを入力アドレスに置き換える
  NNS_SndArcSetFileAddress(bnkFileID, bnkAdrs); 
  NNS_SndArcSetFileAddress(seqFileID, seqAdrs); 

  SOUNDMAN_LoadHierarchyPlayer_forThread_end(dummyNo);

  return TRUE;
}

BOOL  PMDSND_ChangeWaveData
      ( u32 waveArcDstID, u32 waveDstIdx, void* waveArcSrcAdrs, u32 waveSrcIdx)
{
  const NNSSndArcWaveArcInfo* waveInfo;
  SNDWaveArc*                 waveArcDst;
  SNDWaveArc*                 waveArcSrc = (SNDWaveArc*)waveArcSrcAdrs;
  const SNDWaveData*          waveData;

  // 波形ファイルID取得
  waveInfo = NNS_SndArcGetWaveArcInfo( waveArcDstID );
  if( waveInfo == NULL ){ return FALSE; }

  // 差し替え先の波形アドレス取得
  waveArcDst = (SNDWaveArc*)NNS_SndArcGetFileAddress( waveInfo->fileId );
  if(waveArcDst == NULL){ return FALSE; }
  // 差し替え元の波形アドレス取得
  waveData = SND_GetWaveDataAddress( waveArcSrc, waveSrcIdx );  

  // 波形アドレス書き換え
  SND_SetWaveDataAddress( waveArcDst, waveDstIdx, waveData );

  return TRUE;
}

BOOL  PMDSND_PlayExtraMusic( u32 dummyNo )
{
  return NNS_SndArcPlayerStartSeq( SOUNDMAN_GetHierarchyPlayerSndHandle(), dummyNo );
}

void  PMDSND_StopExtraMusic( void )
{
  NNS_SndPlayerStopSeq(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0);
  SOUNDMAN_UnloadHierarchyPlayer();
}

void  PMDSND_ReleaseExtraMusic( void )
{
  // ダミー音楽のFileIDを元に戻す
  {
    u32 bnkFileID = NNS_SndArcGetBankInfo(BANK_MUS_WB_SHINKA)->fileId;
    u32 seqFileID = NNS_SndArcGetSeqInfo(SEQ_BGM_SHINKA)->fileId;

    NNS_SndArcSetFileAddress(bnkFileID, NULL);
    NNS_SndArcSetFileAddress(seqFileID, NULL);
  }
}


