//============================================================================================
/**
 * @file	sound_manager.c
 * @brief	�T�E���h�Ǘ�
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
 * @brief	�T�E���h�Ǘ��V�X�e��������
 *
 *
 *
 */
//============================================================================================
void	SOUNDMAN_Init(NNSSndHeapHandle* pHeapHandle)
{
	// �T�E���h�q�[�v�n���h���ݒ�
	pSndHeapHandle		= pHeapHandle;
}





//============================================================================================
/**
 *
 *
 *
 * @brief	�v���[���[�K�w�\���֐�
 *
 *
 *
 */
//============================================================================================
typedef struct _PLAYER_HIERARCHY	PLAYER_HIERARCHY;

struct _PLAYER_HIERARCHY{
	BOOL					active;
	u16						soundIdx;
	NNSSndHandle	sndHandle;
	int						heapLvDelete;
	int						heapLvReset;
	int						heapLvPush;
	int						heapLvFull;
};

//--------------------------------------------------------------------------------------------
#define PLAYER_HIERARCHY_NUM					(6)
#define PLAYER_HIERARCHY_EMPTY				(0xffff)
#define PLAYER_HIERARCHY_HEAPLV_NULL	(0xffffffff)
static PLAYER_HIERARCHY	sndHierarchyArray[PLAYER_HIERARCHY_NUM];
static int	sndHierarchyArrayPos;
static u16	sndHierarchyPlayerNo;

//--------------------------------------------------------------------------------------------
/**
 * @brief	�K�w���[�N������or���Z�b�g
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
 * @brief	�K�w�v���[���[�쐬
 */
//--------------------------------------------------------------------------------------------
static void createHierarchyPlayer( int heapLvDelete )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];
	BOOL result;

	// �v���[���[�폜�̍ۂɕ��A����q�[�v��Ԃk�u��ۑ�
	player->heapLvDelete = heapLvDelete;

	// �e���A�q�[�v��Ԃk�u��������
	player->heapLvReset = heapLvDelete;
	player->heapLvPush = heapLvDelete;
	player->heapLvFull = heapLvDelete;

	player->active = TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�K�w�v���[���[�폜
 */
//--------------------------------------------------------------------------------------------
static void deleteHierarchyPlayer( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	// �T�E���h�n���h������V�[�P���X��؂藣��
	NNS_SndHandleReleaseSeq(&player->sndHandle);
	// �v���[���[�쐬�O�̏�ԂɃT�E���h�q�[�v�𕜌�
	NNS_SndHeapLoadState(*pSndHeapHandle, player->heapLvDelete);

	// �v���[���[�K�w���[�N���Z�b�g
	initHierarchyPlayerWork( sndHierarchyArrayPos );
}


//--------------------------------------------------------------------------------------------
/**
 * 
 * @brief	�K�w�\��������
 *
 */
//--------------------------------------------------------------------------------------------
void	SOUNDMAN_InitHierarchyPlayer( u16	hierarchyPlayerNo )
{
	PLAYER_HIERARCHY* player;
	int i;

	GF_ASSERT(pSndHeapHandle);

	// �v���[���[�i���o�[�ݒ�
	sndHierarchyPlayerNo = hierarchyPlayerNo;

	// �K�w�v���[���[�\��������
	sndHierarchyArrayPos = 0;
	for( i=0; i<PLAYER_HIERARCHY_NUM; i++ ){
		initHierarchyPlayerWork(i);
		player = &sndHierarchyArray[i];
		// �T�E���h�n���h���͈�x����������
		NNS_SndHandleInit(&player->sndHandle);
	}
	NNS_SndPlayerSetPlayableSeqCount(sndHierarchyPlayerNo, PLAYER_HIERARCHY_NUM);

	// �����K�w�v���[���[�쐬
	createHierarchyPlayer(NNS_SndHeapSaveState(*pSndHeapHandle));
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	�K�w�v���[���[��Ԏ擾
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

u32		SOUNDMAN_GetHierarchyPlayerPlayerNoIdx( void )
{
	return sndHierarchyArrayPos;
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
 * @brief	�K�w�v���[���[��Ԑݒ�
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
 * @brief	�K�w�v���[���[��ԕ��A
 *				�����ݍĐ����̃T�E���h�f�[�^���ǂݍ��܂ꂽ���
 */
//--------------------------------------------------------------------------------------------
void	SOUNDMAN_RecoverHierarchyPlayerState( void )
{
	NNS_SndHeapLoadState(*pSndHeapHandle, SOUNDMAN_GetHierarchyPlayerSoundHeapLv());
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	�T�E���h�Đ�
 *
 */
//--------------------------------------------------------------------------------------------
BOOL	SOUNDMAN_PlayHierarchyPlayer( u32 soundIdx )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];
	BOOL result;
#ifdef STATUS_PRINT
	u32	 heapSize;
#endif
	GF_ASSERT(pSndHeapHandle);

	// ���݂̊K�w�Ɋ֘A�t������Ă���T�E���h�̒�~
	SOUNDMAN_StopHierarchyPlayer();
#ifdef STATUS_PRINT
	heapSize = NNS_SndHeapGetFreeSize(*pSndHeapHandle);
#endif
	// �T�E���h�f�[�^�iseq, bank�j�ǂݍ���
	result = NNS_SndArcLoadSeqEx
				(soundIdx, NNS_SND_ARC_LOAD_SEQ | NNS_SND_ARC_LOAD_BANK, *pSndHeapHandle);
	if( result == FALSE){ return FALSE; }

	player->heapLvPush = NNS_SndHeapSaveState(*pSndHeapHandle);

	// �T�E���h�f�[�^�iwave�j�ǂݍ���
	result = NNS_SndArcLoadSeqEx(soundIdx, NNS_SND_ARC_LOAD_WAVE, *pSndHeapHandle);
	if( result == FALSE){ return FALSE; }

	// �T�E���h�Đ��J�n
	result = NNS_SndArcPlayerStartSeqEx(&player->sndHandle, sndHierarchyPlayerNo, -1, -1, soundIdx);
	if( result == FALSE){ return FALSE; }

	player->heapLvFull = NNS_SndHeapSaveState(*pSndHeapHandle);
#ifdef STATUS_PRINT
	OS_Printf("sound hierarchy start player(%d), seqno(%d), ", sndHierarchyArrayPos, soundIdx);
	OS_Printf("soundHeap(%d->%d), ", heapSize, NNS_SndHeapGetFreeSize(*pSndHeapHandle));
	OS_Printf("soundHeapSaveLv delete(%d), reset(%d), push(%d), full(%d)\n", 
			player->heapLvDelete, player->heapLvReset, player->heapLvPush, player->heapLvFull);
#endif
	player->soundIdx = soundIdx;
	return result;
}

//--------------------------------------------------------------------------------------------
void	SOUNDMAN_PlayHierarchyPlayer_forThread_heapsv( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];
	GF_ASSERT(pSndHeapHandle);

	player->heapLvPush = NNS_SndHeapSaveState(*pSndHeapHandle);
}

BOOL	SOUNDMAN_PlayHierarchyPlayer_forThread_play( u32 soundIdx )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];
	BOOL result;

	GF_ASSERT(pSndHeapHandle);

	// �T�E���h�Đ��J�n
	result = NNS_SndArcPlayerStartSeqEx(&player->sndHandle, sndHierarchyPlayerNo, -1, -1, soundIdx);
	if( result == FALSE){ return FALSE; }

	player->heapLvFull = NNS_SndHeapSaveState(*pSndHeapHandle);
	player->soundIdx = soundIdx;

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	�T�E���h��~
 *
 */
//--------------------------------------------------------------------------------------------
void	SOUNDMAN_StopHierarchyPlayer( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	GF_ASSERT(pSndHeapHandle);

	if( player->active == FALSE ){ return; }

	// ���݂̃T�E���h�n���h���Ɍ��ѕt����ꂽ�V�[�P���X�𑦎���~
	NNS_SndPlayerStopSeq(&player->sndHandle, 0);
	player->soundIdx = PLAYER_HIERARCHY_EMPTY;
	NNS_SndHeapLoadState(*pSndHeapHandle, player->heapLvReset);
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	�T�E���h�ꎞ��~
 *
 */
//--------------------------------------------------------------------------------------------
void	SOUNDMAN_PauseHierarchyPlayer( BOOL flag )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	GF_ASSERT(pSndHeapHandle);

	if( player->active == FALSE ){ return; }
	// ���݂̃T�E���h�n���h���Ɍ��ѕt����ꂽ�V�[�P���X���ꎞ��~
	NNS_SndPlayerPause(&player->sndHandle, flag);
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	�T�E���h�K�w����
 *
 */
//--------------------------------------------------------------------------------------------
BOOL	SOUNDMAN_PushHierarchyPlayer( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];
	int nextHeapLvDelete;

	GF_ASSERT(pSndHeapHandle);

	if( sndHierarchyArrayPos >= PLAYER_HIERARCHY_NUM -1 ){ return FALSE; }	// �K�w�ő�

	if(player->heapLvPush != PLAYER_HIERARCHY_HEAPLV_NULL){
		// �T�E���h�f�[�^�iwave�j������Ή��
		NNS_SndHeapLoadState(*pSndHeapHandle, player->heapLvPush);
		player->heapLvFull = PLAYER_HIERARCHY_HEAPLV_NULL;
		nextHeapLvDelete = player->heapLvPush;
	} else {
		nextHeapLvDelete = player->heapLvReset;
	}
	// �K�wUp
	sndHierarchyArrayPos++;
	createHierarchyPlayer(nextHeapLvDelete);

	return TRUE;
}

//------------------------------------------------------------------
BOOL	SOUNDMAN_PopHierarchyPlayer( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	GF_ASSERT(pSndHeapHandle);

	if( sndHierarchyArrayPos <= 0 ){ return FALSE; }	// �K�w�ŏ�

	// ���݂̊K�w�Ɋ֘A�t������Ă���T�E���h�̒�~
	SOUNDMAN_StopHierarchyPlayer();
	
	// �K�wDown
	deleteHierarchyPlayer();
	sndHierarchyArrayPos--;

	// �O�w�̃T�E���h�f�[�^�iwave�j������Γǂݍ��݂��ĕ���
	player = &sndHierarchyArray[sndHierarchyArrayPos];
	if( player->soundIdx != PLAYER_HIERARCHY_EMPTY ){
		BOOL result;
		result = NNS_SndArcLoadSeqEx
			(player->soundIdx, NNS_SND_ARC_LOAD_WAVE | NNS_SND_ARC_LOAD_BANK, *pSndHeapHandle);
		player->heapLvFull = NNS_SndHeapSaveState(*pSndHeapHandle);
	}

	return TRUE;
}





//============================================================================================
/**
 *
 *
 *
 * @brief	�T�E���h�v���Z�b�g�֐�
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
 * @brief	�T�E���h�f�[�^�v���Z�b�g
 *
 */
//--------------------------------------------------------------------------------------------
SOUNDMAN_PRESET_HANDLE* SOUNDMAN_PresetSoundTbl( const u32* soundIdxTbl, u32 tblNum )
{
	SOUNDMAN_PRESET_HANDLE* handle;
	int heapLvNew;
	int i;

	GF_ASSERT(pSndHeapHandle);

	// ����Đ�����Ă���a�f�l�͋�����~
	SOUNDMAN_StopHierarchyPlayer();

	handle = NNS_SndHeapAlloc(*pSndHeapHandle, sizeof(SOUNDMAN_PRESET_HANDLE), NULL, 0, 0);

	for( i=0; i<tblNum; i++ ){ NNS_SndArcLoadSeq(soundIdxTbl[i], *pSndHeapHandle); }
	handle->soundNum = tblNum;

	// �����[�X�̍ۂɕ��A����q�[�v��Ԃk�u��ۑ�
	handle->heapLvRelease = SOUNDMAN_GetHierarchyPlayerSoundHeapLv();

	// ���݂̊K�w�v���[���[�̃q�[�v���x���ݒ���X�V
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

	// ����Đ�����Ă���a�f�l�͋�����~
	SOUNDMAN_StopHierarchyPlayer();

	handle = NNS_SndHeapAlloc(*pSndHeapHandle, sizeof(SOUNDMAN_PRESET_HANDLE), NULL, 0, 0);

	groupInfo = NNS_SndArcGetGroupInfo(groupIdx);
	NNS_SndArcLoadGroup(groupIdx, *pSndHeapHandle);
	handle->soundNum = groupInfo->count;

	// �����[�X�̍ۂɕ��A����q�[�v��Ԃk�u��ۑ�
	handle->heapLvRelease = SOUNDMAN_GetHierarchyPlayerSoundHeapLv();

	// ���݂̊K�w�v���[���[�̃q�[�v���x���ݒ���X�V
	SOUNDMAN_UpdateHierarchyPlayerSoundHeapLv();

	return handle;
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	�T�E���h�f�[�^�����[�X
 *
 */
//--------------------------------------------------------------------------------------------
void SOUNDMAN_ReleasePresetData( SOUNDMAN_PRESET_HANDLE* handle )
{
	GF_ASSERT(pSndHeapHandle);

	NNS_SndHeapLoadState(*pSndHeapHandle, handle->heapLvRelease);
}


