//============================================================================================
/**
 * @file	pm_sndsys.c
 * @brief	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"
#include "sound/sound_manager.h"

#include "sound/wb_sound_data.sadl"		//サウンドラベルファイル
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
//仮
BOOL debugBGMsetFlag;

//------------------------------------------------------------------
/**
 * @brief	プレーヤー定義
 */
//------------------------------------------------------------------
typedef struct {
	u32 size;
	u16 channelBit;
	u8	playableNum;
}PMSND_PLSETUP;

#define PMSND_PLSETUP_TBLEND (0xffffffff)

typedef struct {
	NNSSndHandle	sndHandle;
	u16				playerNo;
}PMSND_PLAYER_DATA;

typedef struct {
	PMSND_PLAYER_DATA*	playerDataArray;
	u16						playerNum;
	int						heapLvDelete;
	int						heapLvReset;
}PMSND_PL_UNIT;

//------------------------------------------------------------------
/**
 * @brief	プレーヤー設定定義
 */
//------------------------------------------------------------------
#define PLAYER_PMVOICE_CH	(0xc000)
#define PLAYER_SE_CH		(0xd800)

enum {
	PLAYER_SYSSE = 0,
	PLAYER_SE,

	PLAYER_DEFAULT_MAX,
};

static const PMSND_PLSETUP systemPlayer[] = {
	{ 0x0000, 0x0000, 1 },
	{ 0x6000, 0x0000, 1 },

	{ PMSND_PLSETUP_TBLEND, 0x0000, 0 },	// TABLE END
};

#define PLAYER_MUSIC_HEAPSIZ	(0x0000)
#define PLAYER_MUSIC_CH			(0xa7fe)
// ＢＧＭプレーヤー定義
static const SOUNDMAN_HIERARCHY_PLAYER_DATA pmHierarchyPlayerData = {
	PLAYER_MUSIC_HEAPSIZ, PLAYER_DEFAULT_MAX, PLAYER_MUSIC_CH,
};

//------------------------------------------------------------------
/**
 * @brief	プレーヤー情報定義
 */
//------------------------------------------------------------------
typedef struct {
	SNDPlayerInfo			playerInfo;
	SNDTrackInfo			trackInfo[16];
}PMSND_PLAYERSTATUS;

//------------------------------------------------------------------
/**
 * @brief	システムフェード構造定義
 */
//------------------------------------------------------------------
typedef struct {
	BOOL	active;
	u16		volumeCounter;
	u16		nextTrackBit;
	u32		nextSoundIdx;
	int		fadeFrames;
}PMSND_FADESTATUS;

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	サウンドシステム（main.c等から呼び出される）
 *
 *
 *
 *
 *
 */
//============================================================================================
#define	SOUND_HEAP_SIZE	(0x0b0000)

static u8				PmSoundHeap[SOUND_HEAP_SIZE];
static NNSSndArc		PmSoundArc;

static NNSSndHeapHandle		PmSndHeapHandle;
static u32					playerNumber;

static PMSND_FADESTATUS	fadeStatus;

static PMSND_PL_UNIT systemPlayerUnit;
SOUNDMAN_PRESET_HANDLE* systemPresetHandle;

static PMSND_PLAYERSTATUS bgmPlayerInfo;

static void PMSND_InitSystemFade( void );
static void PMSND_ResetSystemFade( void );
static void PMSND_SystemFade( void );
static void PMSND_CreatePlayerUnit
		( const PMSND_PLSETUP* setupTbl, PMSND_PL_UNIT* playerUnit );
static void PMSND_DeletePlayerUnit
		( PMSND_PL_UNIT* playerUnit );
//============================================================================================
/**
 *
 * @brief	セットアップ＆初期化
 *
 */
//============================================================================================
void	PMSND_Init( void )
{
	u32 size1, size2;

    // サウンドシステム初期化
 	NNS_SndInit();
	PmSndHeapHandle = NNS_SndHeapCreate(PmSoundHeap, SOUND_HEAP_SIZE);
	size1 = NNS_SndHeapGetFreeSize(PmSndHeapHandle);

    // サウンドの設定
	NNS_SndArcInitWithResult( &PmSoundArc, "wb_sound_data.sdat", PmSndHeapHandle, FALSE );

    // サウンド管理初期化
	SOUNDMAN_Init(&PmSndHeapHandle);
    // サウンド階層構造初期化
	SOUNDMAN_InitHierarchyPlayer(&pmHierarchyPlayerData);

    // サウンドの設定
	playerNumber = 0;
	PMSND_CreatePlayerUnit(systemPlayer, &systemPlayerUnit);

	PMSND_InitSystemFade();

	// 常駐サウンドデータ読み込み
	systemPresetHandle = SOUNDMAN_PresetGroup(GROUP_GLOBAL);

	size2 = NNS_SndHeapGetFreeSize(PmSndHeapHandle);

	OS_Printf("setup Sound... size(%x) heapRemains(%x)\n", size1 - size2, size2);

	debugBGMsetFlag = FALSE;
}

//============================================================================================
/**
 *
 * @brief	フレームワーク
 *
 */
//============================================================================================
void	PMSND_Main( void )
{
	if( fadeStatus.active == TRUE ){ PMSND_SystemFade(); }

	NNS_SndMain();

	// サウンドドライバ情報更新
	NNS_SndUpdateDriverInfo();
	{
		NNSSndHandle* pBgmHandle = SOUNDMAN_GetHierarchyPlayerSndHandle();
		int i;

		NNS_SndPlayerReadDriverPlayerInfo(pBgmHandle, &bgmPlayerInfo.playerInfo);
		for( i=0; i<16; i++ ){
			NNS_SndPlayerReadDriverTrackInfo( pBgmHandle, i, &bgmPlayerInfo.trackInfo[i]);
		}
	}
}

//============================================================================================
/**
 *
 * @brief	終了処理＆破棄
 *
 */
//============================================================================================
void	PMSND_Exit( void )
{
	SOUNDMAN_ReleasePresetData(systemPresetHandle);

	PMSND_DeletePlayerUnit(&systemPlayerUnit);
	NNS_SndHeapDestroy(PmSndHeapHandle);
}





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	可変プレーヤーセットアップ
 *
 *
 *
 *
 *
 */
//============================================================================================
//============================================================================================
/**
 *
 * @brief	プレーヤーユニット作成
 *
 */
//============================================================================================
static void PMSND_CreatePlayerUnit
			( const PMSND_PLSETUP* setupTbl, PMSND_PL_UNIT* playerUnit )
{
	BOOL result;
	int playerNo;
	u32 playerSize;
	int i;

	// プレーヤーユニット削除の際に復帰するヒープ状態ＬＶを保存
	playerUnit->heapLvDelete = NNS_SndHeapSaveState(PmSndHeapHandle);

	// プレーヤー数カウント
	i = 0;
	while(setupTbl[i].size != PMSND_PLSETUP_TBLEND){ i++; }
	playerUnit->playerNum = i;

	// プレーヤー配列作成
	playerUnit->playerDataArray = (PMSND_PLAYER_DATA*)NNS_SndHeapAlloc
			(PmSndHeapHandle, sizeof(PMSND_PLAYER_DATA) * playerUnit->playerNum, NULL, 0, 0 );

	for( i=0; i<playerUnit->playerNum; i++ ){
		playerNo = i + playerNumber;
		playerSize = setupTbl[i].size;
		// プレーヤー作成
		if(playerSize){
			result = NNS_SndPlayerCreateHeap(playerNo, PmSndHeapHandle, playerSize);
			GF_ASSERT( result == TRUE );
		}
		// チャンネル設定
		NNS_SndPlayerSetAllocatableChannel(playerNo, setupTbl[i].channelBit);
		NNS_SndPlayerSetPlayableSeqCount(playerNo, setupTbl[i].playableNum);

		NNS_SndHandleInit(&playerUnit->playerDataArray[i].sndHandle);
		playerUnit->playerDataArray[i].playerNo = playerNo;
	}
	playerNumber += playerUnit->playerNum; 

	// プレーヤーリセットの際に復帰するヒープ状態ＬＶを保存
	playerUnit->heapLvReset = NNS_SndHeapSaveState(PmSndHeapHandle);
}

//============================================================================================
/**
 *
 * @brief	プレーヤーユニット削除
 *
 */
//============================================================================================
static void PMSND_DeletePlayerUnit( PMSND_PL_UNIT* playerUnit )
{
	int i;

	for( i=0; i<playerUnit->playerNum; i++ ){
		NNS_SndHandleReleaseSeq(&playerUnit->playerDataArray[i].sndHandle);
	}
	NNS_SndHeapLoadState(PmSndHeapHandle, playerUnit->heapLvDelete);
}


//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	サウンド関数（各appから呼び出される）
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	サウンドハンドル取得
 */
//------------------------------------------------------------------
NNSSndHandle* PMSND_GetBGMhandlePointer( void )
{
	return SOUNDMAN_GetHierarchyPlayerSndHandle();
}

//------------------------------------------------------------------
/**
 * @brief	サウンド再生
 */
//------------------------------------------------------------------
BOOL	PMSND_PlayBGM( u32 soundIdx )
{
	return PMSND_PlayBGM_EX(soundIdx, 0xffff);
}
//------------------------------------------------------------------
BOOL	PMSND_PlayBGM_EX( u32 soundIdx, u16 trackBit )
{
	BOOL	result;

	PMSND_ResetSystemFade();

	SOUNDMAN_StopHierarchyPlayer();
	result = SOUNDMAN_PlayHierarchyPlayer(soundIdx);
	if( trackBit != 0xffff ){ PMSND_ChangeBGMtrack( trackBit ); }

	return result;
}
//------------------------------------------------------------------
BOOL	PMSND_PlayNextBGM( u32 soundIdx )
{
	return PMSND_PlayNextBGM_EX(soundIdx, 0xffff);

}
//------------------------------------------------------------------
BOOL	PMSND_PlayNextBGM_EX( u32 soundIdx, u16 trackBit )
{
	fadeStatus.nextSoundIdx = soundIdx;
	fadeStatus.nextTrackBit = trackBit;
	fadeStatus.active = TRUE;

	return TRUE;
}
//------------------------------------------------------------------
// 再生トラック変更
void	PMSND_ChangeBGMtrack( u16 trackBit )
{
	u16		bitMask = trackBit^0xffff;

	NNS_SndPlayerSetTrackMute(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0xffff, FALSE );
	if(bitMask){
		NNS_SndPlayerSetTrackMuteEx
			(SOUNDMAN_GetHierarchyPlayerSndHandle(), bitMask, NNS_SND_SEQ_MUTE_NO_STOP );
	}
}

//------------------------------------------------------------------
/**
 * @brief	サウンド停止
 */
//------------------------------------------------------------------
void	PMSND_StopBGM( void )
{
	PMSND_ResetSystemFade();
	SOUNDMAN_StopHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief	サウンド一時停止
 */
//------------------------------------------------------------------
void	PMSND_PauseBGM( BOOL pauseFlag )
{
	PMSND_ResetSystemFade();
	NNS_SndPlayerPause(SOUNDMAN_GetHierarchyPlayerSndHandle(), pauseFlag);
	if(pauseFlag == TRUE){
		//一時停止をする際フェード中だった場合に備え、最大値に更新
		NNS_SndPlayerSetVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 127);
	}
}

//------------------------------------------------------------------
/**
 * @brief	サウンドフェードイン
 */
//------------------------------------------------------------------
void	PMSND_FadeInBGM( u16 frames )
{
	PMSND_ResetSystemFade();
	// 現在のvolumeを即時更新
	NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0, 0);
	// 再度目標値を設定
	NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 127, frames);
}

//------------------------------------------------------------------
/**
 * @brief	サウンドフェードアウト
 */
//------------------------------------------------------------------
void	PMSND_FadeOutBGM( u16 frames )
{
	PMSND_ResetSystemFade();
	NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0, frames);
}

//------------------------------------------------------------------
/**
 * @brief	サウンド状態保管
 */
//------------------------------------------------------------------
void	PMSND_PushBGM( void )
{
	PMSND_ResetSystemFade();
	SOUNDMAN_PushHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief	サウンド状態取り出し
 */
//------------------------------------------------------------------
void	PMSND_PopBGM( void )
{
	PMSND_ResetSystemFade();
	SOUNDMAN_PopHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief	ＢＧＭ終了検出
 */
//------------------------------------------------------------------
BOOL	PMSND_CheckPlayBGM( void )
{
	int count = NNS_SndPlayerCountPlayingSeqByPlayerNo(SOUNDMAN_GetHierarchyPlayerPlayerNo());

	if( count ){
		return TRUE;
	} else {
		return FALSE;
	}
}





//============================================================================================
/**
 *
 *
 *
 * @brief	演出サウンド関数（各appから呼び出される）
 *
 *
 *
 */
//============================================================================================
static void resetSoundEffect( PMSND_PLAYER_DATA* ppd, u32 channel )
{
	NNS_SndPlayerStopSeq(&ppd->sndHandle, 0);
	// プレイヤー使用チャンネル設定
	NNS_SndPlayerSetAllocatableChannel(ppd->playerNo, channel);
}
	
//------------------------------------------------------------------
/**
 * @brief	システムＳＥサウンド関数
 */
//------------------------------------------------------------------
BOOL	PMSND_PlaySystemSE( u32 soundIdx )
{
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_SYSSE];

	resetSoundEffect( ppd, PLAYER_SE_CH );

	return NNS_SndArcPlayerStartSeqEx(&ppd->sndHandle, ppd->playerNo, -1, 126, soundIdx);
}

//------------------------------------------------------------------
/**
 * @brief	ＳＥサウンド関数
 */
//------------------------------------------------------------------
BOOL	PMSND_PlaySE( u32 soundIdx )
{
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_SE];

	resetSoundEffect( ppd, PLAYER_SE_CH );

	return  NNS_SndArcPlayerStartSeqEx(&ppd->sndHandle, ppd->playerNo, -1, 126, soundIdx);
}

//------------------------------------------------------------------
/**
 * @brief	鳴き声サウンド関数
 */
//------------------------------------------------------------------
BOOL	PMSND_PlayVoice( u32 pokeNum )
{
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_SE];

	resetSoundEffect( ppd, PLAYER_PMVOICE_CH );

	return NNS_SndArcPlayerStartSeqEx(&ppd->sndHandle, ppd->playerNo, pokeNum, 127, SEQ_PV);
}

//------------------------------------------------------------------
/**
 * @brief	ＳＥ終了検出
 */
//------------------------------------------------------------------
BOOL	PMSND_CheckPlaySEVoice( void )
{
	PMSND_PLAYER_DATA* playerData = &systemPlayerUnit.playerDataArray[PLAYER_SE];
	int count = NNS_SndPlayerCountPlayingSeqByPlayerNo(playerData->playerNo);

	if( count ){
		return TRUE;
	} else {
		return FALSE;
	}
}

//------------------------------------------------------------------
/**
 * @brief	ＳＥパン設定
 */
//------------------------------------------------------------------
BOOL	PMSND_SetPanSEVoice( int pan )
{
	PMSND_PLAYER_DATA* playerData = &systemPlayerUnit.playerDataArray[PLAYER_SE];

	NNS_SndPlayerSetTrackPan(&playerData->sndHandle, 0xffff, pan);
	return TRUE;
}





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	システムフェード
 *
 *
 *
 *
 *
 */
//============================================================================================
static void PMSND_InitSystemFade( void )
{
	fadeStatus.fadeFrames = 180;
	PMSND_ResetSystemFade();
}

static void PMSND_ResetSystemFade( void )
{
	fadeStatus.active = FALSE;
	fadeStatus.nextTrackBit = 0xffff;
	fadeStatus.nextSoundIdx = 0;
	fadeStatus.volumeCounter = fadeStatus.fadeFrames;
}

//------------------------------------------------------------------
static void PMSND_SystemFade( void )
{
	NNSSndHandle*	pBgmHandle = SOUNDMAN_GetHierarchyPlayerSndHandle();
	u32				nowSoundIdx = SOUNDMAN_GetHierarchyPlayerSoundIdx();
	BOOL			bgmSetFlag = FALSE;
	int				volume;

	if(nowSoundIdx){
		if( nowSoundIdx != fadeStatus.nextSoundIdx ){
			//FADEOUT
			if( fadeStatus.volumeCounter > 0 ){
				fadeStatus.volumeCounter--;
			} else {
				bgmSetFlag = TRUE;
			}
		} else {									
			//FADEIN
			if( fadeStatus.volumeCounter < fadeStatus.fadeFrames ){
				fadeStatus.volumeCounter++;
			} else {
				NNS_SndPlayerSetVolume(pBgmHandle, 127);
				PMSND_ResetSystemFade();
				return;			//終了
			}
		}
	} else {
		bgmSetFlag = TRUE;
		fadeStatus.volumeCounter = 0;	// 最初に鳴り出す場合
	}

	if(bgmSetFlag == TRUE){
		SOUNDMAN_StopHierarchyPlayer();
		SOUNDMAN_PlayHierarchyPlayer(fadeStatus.nextSoundIdx);
		NNS_SndPlayerSetVolume(pBgmHandle, 0);
		fadeStatus.volumeCounter = 0;

		if( fadeStatus.nextTrackBit != 0xffff ){
			PMSND_ChangeBGMtrack(fadeStatus.nextTrackBit);
		}
	} else {
		volume = fadeStatus.volumeCounter * 127 / fadeStatus.fadeFrames;

		NNS_SndPlayerSetVolume(pBgmHandle, volume);
	}
}

//------------------------------------------------------------------
/**
 * @brief	システムフェードフレーム設定
 */
//------------------------------------------------------------------
void PMSND_SetSystemFadeFrames( int frames )
{
	fadeStatus.fadeFrames = frames;
}




