//============================================================================================
/**
 * @file	sound_manager.c
 * @brief	サウンド管理
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "sound/sound_manager.h"

static NNSSndHeapHandle*	pSndHeapHandle = NULL;
//#define STATUS_PRINT
//============================================================================================
/**
 *
 *
 *
 * @brief	サウンド管理システム初期化
 *
 *
 *
 */
//============================================================================================
void	SOUNDMAN_Init(NNSSndHeapHandle* pHeapHandle)
{
	// サウンドヒープハンドル設定
	pSndHeapHandle		= pHeapHandle;
}





//============================================================================================
/**
 *
 *
 *
 * @brief	プレーヤー階層構造関数
 *
 *
 *
 */
//============================================================================================
typedef struct _PLAYER_HIERARCHY	PLAYER_HIERARCHY;

struct _PLAYER_HIERARCHY{
	BOOL				active;
	u16					soundIdx;
	u16					playerNo;
	NNSSndHandle		sndHandle;
	int					heapLvDelete;
	int					heapLvReset;
	int					heapLvPush;
	int					heapLvFull;
};

//--------------------------------------------------------------------------------------------
#define PLAYER_HIERARCHY_NUM			(6)
#define PLAYER_HIERARCHY_EMPTY			(0xffff)
#define PLAYER_HIERARCHY_HEAPLV_NULL	(0xffffffff)
static PLAYER_HIERARCHY	sndHierarchyArray[PLAYER_HIERARCHY_NUM];
static int				sndHierarchyArrayPos;

static u16	hierarchyPlayerSize;
static u16	hierarchyPlayerStartNo;
static u16	playerPlayableChannel;

//--------------------------------------------------------------------------------------------
/**
 * @brief	階層ワーク初期化orリセット
 */
//--------------------------------------------------------------------------------------------
static void initHierarchyPlayerWork( int arrayIdx )
{
	sndHierarchyArray[arrayIdx].active = FALSE;

	sndHierarchyArray[arrayIdx].soundIdx = PLAYER_HIERARCHY_EMPTY;
	NNS_SndHandleReleaseSeq(&sndHierarchyArray[arrayIdx].sndHandle);
	sndHierarchyArray[arrayIdx].heapLvDelete = PLAYER_HIERARCHY_HEAPLV_NULL;
	sndHierarchyArray[arrayIdx].heapLvReset = PLAYER_HIERARCHY_HEAPLV_NULL;
	sndHierarchyArray[arrayIdx].heapLvPush = PLAYER_HIERARCHY_HEAPLV_NULL;
	sndHierarchyArray[arrayIdx].heapLvFull = PLAYER_HIERARCHY_HEAPLV_NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	階層プレーヤー作成
 */
//--------------------------------------------------------------------------------------------
static void createHierarchyPlayer( int heapLvDelete )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];
	BOOL result;

	// プレーヤー削除の際に復帰するヒープ状態ＬＶを保存
	player->heapLvDelete = heapLvDelete;

	// プレーヤーリセットの際に復帰するヒープ状態ＬＶを保存
	player->heapLvReset = heapLvDelete;

	// チャンネル設定
	NNS_SndPlayerSetAllocatableChannel(player->playerNo, playerPlayableChannel);

	player->active = TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	階層プレーヤーリセット
 */
//--------------------------------------------------------------------------------------------
static void	resetHierarchyPlayer( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	NNS_SndHeapLoadState(*pSndHeapHandle, player->heapLvReset);
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	階層プレーヤー削除
 */
//--------------------------------------------------------------------------------------------
static void deleteHierarchyPlayer( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	// サウンドハンドルからシーケンスを切り離す
	NNS_SndHandleReleaseSeq(&player->sndHandle);
	// プレーヤー作成前の状態にサウンドヒープを復元
	NNS_SndHeapLoadState(*pSndHeapHandle, player->heapLvDelete);

	// プレーヤー階層ワークリセット
	initHierarchyPlayerWork( sndHierarchyArrayPos );
}


//--------------------------------------------------------------------------------------------
/**
 * 
 * @brief	階層構造初期化
 *
 */
//--------------------------------------------------------------------------------------------
void	SOUNDMAN_InitHierarchyPlayer( const SOUNDMAN_HIERARCHY_PLAYER_DATA* playerData )
{
	int i;

	GF_ASSERT(pSndHeapHandle);

	// 階層プレーヤー構造初期化
	sndHierarchyArrayPos = 0;
	for( i=0; i<PLAYER_HIERARCHY_NUM; i++ ){
		initHierarchyPlayerWork(i);
		// プレーヤーナンバー設定
		sndHierarchyArray[i].playerNo = playerData->hierarchyPlayerStartNo + i;
		// サウンドハンドルは一度だけ初期化
		NNS_SndHandleInit(&sndHierarchyArray[i].sndHandle);
	}
	// 階層プレーヤーステータス設定
	hierarchyPlayerSize		= playerData->hierarchyPlayerSize;
	hierarchyPlayerStartNo	= playerData->hierarchyPlayerStartNo;
	playerPlayableChannel	= playerData->playerPlayableChannel;

	// 初期階層プレーヤー作成
	createHierarchyPlayer(NNS_SndHeapSaveState(*pSndHeapHandle));
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	階層プレーヤー状態取得
 *
 */
//--------------------------------------------------------------------------------------------
u32		SOUNDMAN_GetHierarchyPlayerSoundIdx( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	if( player->active == FALSE ){ return 0; }
	if( player->soundIdx == PLAYER_HIERARCHY_EMPTY ){ return 0; }

	return player->soundIdx;
}

u32		SOUNDMAN_GetHierarchyPlayerPlayerNo( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	return player->playerNo;
}

NNSSndHandle*	SOUNDMAN_GetHierarchyPlayerSndHandle( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	return &player->sndHandle;
}

int		SOUNDMAN_GetHierarchyPlayerSoundHeapLv( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	if(player->heapLvFull == PLAYER_HIERARCHY_HEAPLV_NULL){
		return player->heapLvReset;
	}
	return player->heapLvFull;
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	階層プレーヤー状態設定
 *
 */
//--------------------------------------------------------------------------------------------
void	SOUNDMAN_UpdateHierarchyPlayerSoundHeapLv( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	player->heapLvReset = NNS_SndHeapSaveState(*pSndHeapHandle);
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	階層プレーヤー状態復帰
 *				※現在再生中のサウンドデータが読み込まれた状態
 */
//--------------------------------------------------------------------------------------------
void	SOUNDMAN_RecoverHierarchyPlayerState( void )
{
	NNS_SndHeapLoadState(*pSndHeapHandle, SOUNDMAN_GetHierarchyPlayerSoundHeapLv());
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	サウンド再生
 *
 */
//--------------------------------------------------------------------------------------------
BOOL	SOUNDMAN_PlayHierarchyPlayer( u32 soundIdx )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];
	BOOL result;
	BOOL playerCreateFlag = FALSE;
#ifdef STATUS_PRINT
	u32	 heapSize;
#endif
	GF_ASSERT(pSndHeapHandle);

	// 現在の階層に関連付けされているサウンドの停止
	SOUNDMAN_StopHierarchyPlayer();
#ifdef STATUS_PRINT
	heapSize = NNS_SndHeapGetFreeSize(*pSndHeapHandle);
#endif
	// サウンドデータ（seq, bank）読み込み
	result = NNS_SndArcLoadSeqEx
				(soundIdx, NNS_SND_ARC_LOAD_SEQ | NNS_SND_ARC_LOAD_BANK, *pSndHeapHandle);
	if( result == FALSE){ return FALSE; }

	player->heapLvPush = NNS_SndHeapSaveState(*pSndHeapHandle);

	// サウンドデータ（wave）読み込み
	result = NNS_SndArcLoadSeqEx(soundIdx, NNS_SND_ARC_LOAD_WAVE, *pSndHeapHandle);
	if( result == FALSE){ return FALSE; }

	// サウンドデータ（seq, bank）をプレーヤーヒープに読み込み、サウンド再生開始
	result = NNS_SndArcPlayerStartSeqEx
			(&player->sndHandle, player->playerNo, -1, -1, soundIdx);
	if( result == FALSE){ return FALSE; }

	player->heapLvFull = NNS_SndHeapSaveState(*pSndHeapHandle);
#ifdef STATUS_PRINT
	OS_Printf("sound hierarchy start player(%d), seqno(%d), ", player->playerNo, soundIdx);
	OS_Printf("soundHeap(0x%x->0x%x), ", heapSize, NNS_SndHeapGetFreeSize(*pSndHeapHandle));
	OS_Printf("soundHeapSaveLv delete(%d), reset(%d), push(%d), full(%d)\n", 
			player->heapLvDelete, player->heapLvReset, player->heapLvPush, player->heapLvFull);
#endif

	player->soundIdx = soundIdx;
	return result;
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	サウンド停止
 *
 */
//--------------------------------------------------------------------------------------------
void	SOUNDMAN_StopHierarchyPlayer( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	GF_ASSERT(pSndHeapHandle);

	if( player->active == FALSE ){ return; }

	// 現在のハンドルに関連付けされているサウンドの即時停止
	NNS_SndPlayerStopSeq(&player->sndHandle, 0);
	player->soundIdx = PLAYER_HIERARCHY_EMPTY;
	resetHierarchyPlayer();
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	サウンド階層操作
 *
 */
//--------------------------------------------------------------------------------------------
BOOL	SOUNDMAN_PushHierarchyPlayer( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];
	int nextHeapLvDelete;

	GF_ASSERT(pSndHeapHandle);

	if( sndHierarchyArrayPos >= PLAYER_HIERARCHY_NUM -1 ){ return FALSE; }	// 階層最大

	if(player->heapLvPush != PLAYER_HIERARCHY_HEAPLV_NULL){
		// サウンドデータ（wave）があれば解放
		NNS_SndHeapLoadState(*pSndHeapHandle, player->heapLvPush);
		nextHeapLvDelete = player->heapLvPush;
	} else {
		nextHeapLvDelete = player->heapLvReset;
	}
	// 階層Up
	sndHierarchyArrayPos++;
	createHierarchyPlayer(nextHeapLvDelete);

	return TRUE;
}

//------------------------------------------------------------------
BOOL	SOUNDMAN_PopHierarchyPlayer( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	GF_ASSERT(pSndHeapHandle);

	if( sndHierarchyArrayPos <= 0 ){ return FALSE; }	// 階層最少

	// 現在の階層に関連付けされているサウンドの停止
	SOUNDMAN_StopHierarchyPlayer();
	
	// 階層Down
	deleteHierarchyPlayer();
	sndHierarchyArrayPos--;

	// 前層のサウンドデータ（wave）があれば読み込みして復元
	player = &sndHierarchyArray[sndHierarchyArrayPos];
	if( player->soundIdx != PLAYER_HIERARCHY_EMPTY ){
		BOOL result;
		result = NNS_SndArcLoadSeqEx(player->soundIdx, NNS_SND_ARC_LOAD_WAVE, *pSndHeapHandle);
		player->heapLvFull = NNS_SndHeapSaveState(*pSndHeapHandle);
	}

	return TRUE;
}





//============================================================================================
/**
 *
 *
 *
 * @brief	サウンドプリセット関数
 *
 *
 *
 */
//============================================================================================
struct _SOUNDMAN_PRESET_HANDLE{
	u32					soundNum;
	int					heapLvRelease;
};

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	サウンドデータプリセット
 *
 */
//--------------------------------------------------------------------------------------------
SOUNDMAN_PRESET_HANDLE* SOUNDMAN_PresetSoundTbl( const u32* soundIdxTbl, u32 tblNum )
{
	SOUNDMAN_PRESET_HANDLE* handle;
	int heapLvNew;
	int i;

	GF_ASSERT(pSndHeapHandle);

	// 現状再生されているＢＧＭは強制停止
	SOUNDMAN_StopHierarchyPlayer();

	handle = NNS_SndHeapAlloc(*pSndHeapHandle, sizeof(SOUNDMAN_PRESET_HANDLE), NULL, 0, 0);

	for( i=0; i<tblNum; i++ ){ NNS_SndArcLoadSeq(soundIdxTbl[i], *pSndHeapHandle); }
	handle->soundNum = tblNum;

	// リリースの際に復帰するヒープ状態ＬＶを保存
	handle->heapLvRelease = SOUNDMAN_GetHierarchyPlayerSoundHeapLv();

	// 現在の階層プレーヤーのヒープレベル設定を更新
	SOUNDMAN_UpdateHierarchyPlayerSoundHeapLv();

	return handle;
}

//--------------------------------------------------------------------------------------------
SOUNDMAN_PRESET_HANDLE* SOUNDMAN_PresetGroup( u32 groupIdx )
{
	SOUNDMAN_PRESET_HANDLE* handle;
	const NNSSndArcGroupInfo* groupInfo;
	int heapLvRelease, heapLvNew;

	GF_ASSERT(pSndHeapHandle);

	// 現状再生されているＢＧＭは強制停止
	SOUNDMAN_StopHierarchyPlayer();

	handle = NNS_SndHeapAlloc(*pSndHeapHandle, sizeof(SOUNDMAN_PRESET_HANDLE), NULL, 0, 0);

	groupInfo = NNS_SndArcGetGroupInfo(groupIdx);
	NNS_SndArcLoadGroup(groupIdx, *pSndHeapHandle);
	handle->soundNum = groupInfo->count;

	// リリースの際に復帰するヒープ状態ＬＶを保存
	handle->heapLvRelease = SOUNDMAN_GetHierarchyPlayerSoundHeapLv();

	// 現在の階層プレーヤーのヒープレベル設定を更新
	SOUNDMAN_UpdateHierarchyPlayerSoundHeapLv();

	return handle;
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	サウンドデータリリース
 *
 */
//--------------------------------------------------------------------------------------------
void SOUNDMAN_ReleasePresetData( SOUNDMAN_PRESET_HANDLE* handle )
{
	GF_ASSERT(pSndHeapHandle);

	NNS_SndHeapLoadState(*pSndHeapHandle, handle->heapLvRelease);
}


