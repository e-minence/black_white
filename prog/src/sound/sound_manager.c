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
	BOOL				active;
	u16					soundIdx;
	u16					playerNo;
	NNSSndHandle		sndHandle;
//	int					heapLvDelete;
	int					heapLvReset;
	int					heapLvPush;
};

//--------------------------------------------------------------------------------------------
#define PLAYER_HIERARCHY_NUM			(8)
#define PLAYER_HIERARCHY_EMPTY			(0xffffffff)
#define PLAYER_HIERARCHY_HEAPLV_NULL	(0xffffffff)
static PLAYER_HIERARCHY	sndHierarchyArray[PLAYER_HIERARCHY_NUM];
static int				sndHierarchyArrayPos;

static u16	hierarchyPlayerSize;
static u16	hierarchyPlayerStartNo;
static u16	playerPlayableChannel;

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
//	sndHierarchyArray[arrayIdx].heapLvDelete = PLAYER_HIERARCHY_HEAPLV_NULL;
	sndHierarchyArray[arrayIdx].heapLvReset = PLAYER_HIERARCHY_HEAPLV_NULL;
	sndHierarchyArray[arrayIdx].heapLvPush = PLAYER_HIERARCHY_HEAPLV_NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	�K�w�v���[���[�쐬
 */
//--------------------------------------------------------------------------------------------
static void createHierarchyPlayer( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];
	BOOL result;

	if( player->active == TRUE ){ return; }

#if 0	
	// �v���[���[�폜�̍ۂɕ��A����q�[�v��Ԃk�u��ۑ�
	player->heapLvDelete = NNS_SndHeapSaveState(sndHeapHandle);
	// �v���[���[�쐬
	result = NNS_SndPlayerCreateHeap(player->playerNo, sndHeapHandle, hierarchyPlayerSize );
	if( result == FALSE ){
		GF_ASSERT(0);
	}
#endif
	// �`�����l���ݒ�
	NNS_SndPlayerSetAllocatableChannel(player->playerNo, playerPlayableChannel);

	// �v���[���[���Z�b�g�̍ۂɕ��A����q�[�v��Ԃk�u��ۑ�
	player->heapLvReset = NNS_SndHeapSaveState(*pSndHeapHandle);

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

	if( player->active == FALSE ){ return; }

	// �T�E���h�n���h������V�[�P���X��؂藣��
	NNS_SndHandleReleaseSeq(&player->sndHandle);
	// �v���[���[�쐬�O�̏�ԂɃT�E���h�q�[�v�𕜌�
#if 0
	NNS_SndHeapLoadState(*pSndHeapHandle, player->heapLvDelete);
#else
	NNS_SndHeapLoadState(*pSndHeapHandle, player->heapLvReset);
#endif
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
void	SOUNDMAN_InitHierarchyPlayer( const SOUNDMAN_HIERARCHY_PLAYER_DATA* playerData )
{
	int i;

	GF_ASSERT(pSndHeapHandle);

	// �K�w�v���[���[�\��������
	sndHierarchyArrayPos = 0;
	for( i=0; i<PLAYER_HIERARCHY_NUM; i++ ){
		initHierarchyPlayerWork(i);
		// �v���[���[�i���o�[�ݒ�
		sndHierarchyArray[i].playerNo = playerData->hierarchyPlayerStartNo + i;
		// �T�E���h�n���h���͈�x����������
		NNS_SndHandleInit(&sndHierarchyArray[i].sndHandle);
	}
	// �K�w�v���[���[�X�e�[�^�X�ݒ�
	hierarchyPlayerSize		= playerData->hierarchyPlayerSize;
	hierarchyPlayerStartNo	= playerData->hierarchyPlayerStartNo;
	playerPlayableChannel	= playerData->playerPlayableChannel;
}

//--------------------------------------------------------------------------------------------
/**
 *
 * @brief	�K�w�v���[���[��Ԏ擾
 *
 */
//--------------------------------------------------------------------------------------------
u16		SOUNDMAN_GetHierarchyPlayerSoundIdx( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	if( player->active == FALSE ){ return 0; }

	return player->soundIdx;
}

u16		SOUNDMAN_GetHierarchyPlayerPlayerNo( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	return player->playerNo;
}

NNSSndHandle*	SOUNDMAN_GetHierarchyPlayerSndHandle( void )
{
	PLAYER_HIERARCHY* player = &sndHierarchyArray[sndHierarchyArrayPos];

	return &player->sndHandle;
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

	GF_ASSERT(pSndHeapHandle);

	if( player->active == FALSE ){
		createHierarchyPlayer();
		//OS_Printf("create player no = %d, heapRemains = %x resetLv = %d, ", 
		//		player->playerNo, NNS_SndHeapGetFreeSize(*pSndHeapHandle), player->heapLvReset);
	}
	// ���݂̊K�w�Ɋ֘A�t������Ă���T�E���h�̒�~
	SOUNDMAN_StopHierarchyPlayer();

	// �T�E���h�f�[�^�iseq, bank�j�ǂݍ���
	result = NNS_SndArcLoadSeqEx
				(soundIdx, NNS_SND_ARC_LOAD_SEQ | NNS_SND_ARC_LOAD_BANK, *pSndHeapHandle);
	if( result == FALSE){ return FALSE; }

	player->heapLvPush = NNS_SndHeapSaveState(*pSndHeapHandle);

	// �T�E���h�f�[�^�iwave�j�ǂݍ���
	result = NNS_SndArcLoadSeqEx(soundIdx, NNS_SND_ARC_LOAD_WAVE, *pSndHeapHandle);
	if( result == FALSE){ return FALSE; }

	// �T�E���h�f�[�^�iseq, bank�j���v���[���[�q�[�v�ɓǂݍ��݁A�T�E���h�Đ��J�n
	result = NNS_SndArcPlayerStartSeqEx
			(&player->sndHandle, player->playerNo, -1, -1, soundIdx);
	if( result == FALSE){ return FALSE; }

	//OS_Printf("sound hierarchy start playerno = %d, pushLv = %d\n",
	//			player->playerNo, player->heapLvPush);

	player->soundIdx = soundIdx;
	return result;
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

	// ���݂̃n���h���Ɋ֘A�t������Ă���T�E���h�̑�����~
	NNS_SndPlayerStopSeq(&player->sndHandle, 0);

	player->soundIdx = PLAYER_HIERARCHY_EMPTY;

	// �q�[�v��Ԃ����Z�b�g
	NNS_SndHeapLoadState(*pSndHeapHandle, player->heapLvReset);
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

	GF_ASSERT(pSndHeapHandle);

	if( player->active == FALSE ){ return TRUE; }	// �K�wUp�̕K�v�͂Ȃ�
	if( sndHierarchyArrayPos >= PLAYER_HIERARCHY_NUM -1 ){ return FALSE; }	// �K�w�ő�

	// �T�E���h�f�[�^�iwave�j���
#if 0
	NNS_SndHeapLoadState(*pSndHeapHandle, player->heapLvReset);
#else
	NNS_SndHeapLoadState(*pSndHeapHandle, player->heapLvPush);
#endif

	// �K�wUp
	sndHierarchyArrayPos++;

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
		result = NNS_SndArcLoadSeqEx(player->soundIdx, NNS_SND_ARC_LOAD_WAVE, *pSndHeapHandle);
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
	int heapLvRelease;
	int i;

	GF_ASSERT(pSndHeapHandle);

	// �����[�X�̍ۂɕ��A����q�[�v��Ԃk�u��ۑ�
	heapLvRelease = NNS_SndHeapSaveState(*pSndHeapHandle);

	handle = NNS_SndHeapAlloc(*pSndHeapHandle, sizeof(SOUNDMAN_PRESET_HANDLE), NULL, 0, 0);

	for( i=0; i<tblNum; i++ ){ NNS_SndArcLoadSeq(soundIdxTbl[i], *pSndHeapHandle); }

	handle->soundNum = tblNum;
	handle->heapLvRelease = heapLvRelease;

	return handle;
}

//--------------------------------------------------------------------------------------------
SOUNDMAN_PRESET_HANDLE* SOUNDMAN_PresetGroup( u32 groupIdx )
{
	SOUNDMAN_PRESET_HANDLE* handle;
	const NNSSndArcGroupInfo* groupInfo;
	int heapLvRelease;

	GF_ASSERT(pSndHeapHandle);

	// �����[�X�̍ۂɕ��A����q�[�v��Ԃk�u��ۑ�
	heapLvRelease = NNS_SndHeapSaveState(*pSndHeapHandle);

	handle = NNS_SndHeapAlloc(*pSndHeapHandle, sizeof(SOUNDMAN_PRESET_HANDLE), NULL, 0, 0);

	groupInfo = NNS_SndArcGetGroupInfo(groupIdx);
	NNS_SndArcLoadGroup(groupIdx, *pSndHeapHandle);

	handle->soundNum = groupInfo->count;
	handle->heapLvRelease = heapLvRelease;

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


