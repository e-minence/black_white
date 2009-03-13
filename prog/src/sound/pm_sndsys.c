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

#include "sound/wb_sound_data.sadl"		//�T�E���h���x���t�@�C��
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
//��
BOOL debugBGMsetFlag;

//------------------------------------------------------------------
/**
 * @brief	�v���[���[��`
 */
//------------------------------------------------------------------
typedef struct {
	u32 size;
	u16 channelBit;
	u8	playableNum;
}PMSNDSYS_PLSETUP;

#define PMSNDSYS_PLSETUP_TBLEND (0xffffffff)

typedef struct {
	NNSSndHandle	sndHandle;
	u16				playerNo;
}PMSNDSYS_PLAYER_DATA;

typedef struct {
	PMSNDSYS_PLAYER_DATA*	playerDataArray;
	u16						playerNum;
	int						heapLvDelete;
	int						heapLvReset;
}PMSNDSYS_PL_UNIT;

//------------------------------------------------------------------
/**
 * @brief	�v���[���[�ݒ��`
 */
//------------------------------------------------------------------
#define PLAYER_PMVOICE_CH	(0xc000)
#define PLAYER_SE_CH		(0xd800)

enum {
	PLAYER_SYSSE = 0,
	PLAYER_SE,
//	PLAYER_BASEBGM,
//	PLAYER_PMVOICE,

	PLAYER_DEFAULT_MAX,
};

static const PMSNDSYS_PLSETUP systemPlayer[] = {
	{ 0x6000, 0x0000, 1 },
	{ 0x0000, 0x0000, 1 },
//	{ 0x6000, PLAYER_MUSIC_CH, 1 },
//	{ 0x6000, PLAYER_PMVOICE_CH, 2 },

	{ PMSNDSYS_PLSETUP_TBLEND, 0x0000, 0 },	// TABLE END
};

#define PLAYER_MUSIC_HEAPSIZ	(0x4000)
#define PLAYER_MUSIC_CH			(0xa7fe)
// �a�f�l�v���[���[��`
static const SOUNDMAN_HIERARCHY_PLAYER_DATA pmHierarchyPlayerData = {
	PLAYER_MUSIC_HEAPSIZ, PLAYER_DEFAULT_MAX, PLAYER_MUSIC_CH,
};

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�T�E���h�V�X�e���imain.c������Ăяo�����j
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

static PMSNDSYS_PL_UNIT systemPlayerUnit;
SOUNDMAN_PRESET_HANDLE* systemPresetHandle;

static const u32 systemPresetSoundIdxTbl[] = {
	1500, 1501, 1504, 1505, 1508, 1509, 1512, 1513,
};

static void PMSNDSYS_CreatePlayerUnit
		( const PMSNDSYS_PLSETUP* setupTbl, PMSNDSYS_PL_UNIT* playerUnit );
static void PMSNDSYS_DeletePlayerUnit
		( PMSNDSYS_PL_UNIT* playerUnit );
//============================================================================================
/**
 *
 * @brief	�Z�b�g�A�b�v��������
 *
 */
//============================================================================================
void	PMSNDSYS_Init( void )
{
	u32 size1, size2;


    // �T�E���h�V�X�e��������
 	NNS_SndInit();
	PmSndHeapHandle = NNS_SndHeapCreate(PmSoundHeap, SOUND_HEAP_SIZE);
	size1 = NNS_SndHeapGetFreeSize(PmSndHeapHandle);

    // �T�E���h�̐ݒ�
	NNS_SndArcInitWithResult( &PmSoundArc, "wb_sound_data.sdat", PmSndHeapHandle, FALSE );

    // �T�E���h�Ǘ�������
	SOUNDMAN_Init(&PmSndHeapHandle);
    // �T�E���h�K�w�\��������
	SOUNDMAN_InitHierarchyPlayer(&pmHierarchyPlayerData);

    // �T�E���h�̐ݒ�
	playerNumber = 0;
	PMSNDSYS_CreatePlayerUnit(systemPlayer, &systemPlayerUnit);

	// �풓�T�E���h�f�[�^�ǂݍ���
	//systemPresetHandle = SOUNDMAN_PresetSoundTbl
	//						(systemPresetSoundIdxTbl, NELEMS(systemPresetSoundIdxTbl));
	systemPresetHandle = SOUNDMAN_PresetGroup(GROUP_GLOBAL);

	size2 = NNS_SndHeapGetFreeSize(PmSndHeapHandle);

	OS_Printf("setup Sound... size(%x) heapRemains(%x)\n", size1 - size2, size2);

	debugBGMsetFlag = FALSE;
}

//============================================================================================
/**
 *
 * @brief	�t���[�����[�N
 *
 */
//============================================================================================
void	PMSNDSYS_Main( void )
{
	NNS_SndMain();
}

//============================================================================================
/**
 *
 * @brief	�I���������j��
 *
 */
//============================================================================================
void	PMSNDSYS_Exit( void )
{
	SOUNDMAN_ReleasePresetData(systemPresetHandle);

	PMSNDSYS_DeletePlayerUnit(&systemPlayerUnit);
	NNS_SndHeapDestroy(PmSndHeapHandle);
}

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�σv���[���[�V�X�e��
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
 * @brief	�v���[���[���j�b�g�쐬
 *
 */
//============================================================================================
static void PMSNDSYS_CreatePlayerUnit
			( const PMSNDSYS_PLSETUP* setupTbl, PMSNDSYS_PL_UNIT* playerUnit )
{
	BOOL result;
	int playerNo;
	u32 playerSize;
	int i;

	// �v���[���[���j�b�g�폜�̍ۂɕ��A����q�[�v��Ԃk�u��ۑ�
	playerUnit->heapLvDelete = NNS_SndHeapSaveState(PmSndHeapHandle);

	// �v���[���[���J�E���g
	i = 0;
	while(setupTbl[i].size != PMSNDSYS_PLSETUP_TBLEND){ i++; }
	playerUnit->playerNum = i;

	// �v���[���[�z��쐬
	playerUnit->playerDataArray = (PMSNDSYS_PLAYER_DATA*)NNS_SndHeapAlloc
			(PmSndHeapHandle, sizeof(PMSNDSYS_PLAYER_DATA) * playerUnit->playerNum, NULL, 0, 0 );

	for( i=0; i<playerUnit->playerNum; i++ ){
		playerNo = i + playerNumber;
		playerSize = setupTbl[i].size;
		// �v���[���[�쐬
		if(playerSize){
			result = NNS_SndPlayerCreateHeap(playerNo, PmSndHeapHandle, playerSize);
			GF_ASSERT( result == TRUE );
		}
		// �`�����l���ݒ�
		NNS_SndPlayerSetAllocatableChannel(playerNo, setupTbl[i].channelBit);
		NNS_SndPlayerSetPlayableSeqCount(playerNo, setupTbl[i].playableNum);

		NNS_SndHandleInit(&playerUnit->playerDataArray[i].sndHandle);
		playerUnit->playerDataArray[i].playerNo = playerNo;
		OS_Printf("player %d handle = %x\n", playerNo, &playerUnit->playerDataArray[i].sndHandle);
	}
	playerNumber += playerUnit->playerNum; 

	// �v���[���[���Z�b�g�̍ۂɕ��A����q�[�v��Ԃk�u��ۑ�
	playerUnit->heapLvReset = NNS_SndHeapSaveState(PmSndHeapHandle);
}

//============================================================================================
/**
 *
 * @brief	�v���[���[���j�b�g�폜
 *
 */
//============================================================================================
static void PMSNDSYS_DeletePlayerUnit( PMSNDSYS_PL_UNIT* playerUnit )
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
 * @brief	�T�E���h�֐��i�eapp����Ăяo�����j
 *
 *
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�T�E���h�n���h���擾
 */
//------------------------------------------------------------------
NNSSndHandle* PMSNDSYS_GetBGMhandlePointer( void )
{
	return SOUNDMAN_GetHierarchyPlayerSndHandle();
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h�Đ�
 */
//------------------------------------------------------------------
BOOL	PMSNDSYS_PlayBGM( u32 soundIdx )
{
	BOOL result;

	SOUNDMAN_StopHierarchyPlayer();
	result = SOUNDMAN_PlayHierarchyPlayer(soundIdx);

	return result;
}
//------------------------------------------------------------------
BOOL	PMSNDSYS_PlayBGM_EX( u32 soundIdx, u16 trackBit )
{
	BOOL	result;

	SOUNDMAN_StopHierarchyPlayer();
	result = SOUNDMAN_PlayHierarchyPlayer(soundIdx);

	PMSNDSYS_ChangeBGMtrack( trackBit );

	return result;
}
//------------------------------------------------------------------
// �Đ��g���b�N�ύX
void	PMSNDSYS_ChangeBGMtrack( u16 trackBit )
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
 * @brief	�T�E���h��~
 */
//------------------------------------------------------------------
void	PMSNDSYS_StopBGM( void )
{
	SOUNDMAN_StopHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h�ꎞ��~
 */
//------------------------------------------------------------------
void	PMSNDSYS_PauseBGM( BOOL pauseFlag )
{
	NNS_SndPlayerPause(SOUNDMAN_GetHierarchyPlayerSndHandle(), pauseFlag);
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h��ԕۊ�
 */
//------------------------------------------------------------------
void	PMSNDSYS_PushBGM( void )
{
	SOUNDMAN_PushHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h��Ԏ��o��
 */
//------------------------------------------------------------------
void	PMSNDSYS_PopBGM( void )
{
	SOUNDMAN_PopHierarchyPlayer();
}



//============================================================================================
/**
 *
 *
 *
 * @brief	�r�d�T�E���h�֐��i�eapp����Ăяo�����j
 *
 *
 *
 */
//============================================================================================
BOOL	PMSNDSYS_PlaySE( u32 soundIdx )
{
	PMSNDSYS_PLAYER_DATA* playerData = &systemPlayerUnit.playerDataArray[PLAYER_SE];
	BOOL result;

	NNS_SndPlayerStopSeq(&playerData->sndHandle, 0);
	// �v���C���[�g�p�`�����l���ݒ�
	NNS_SndPlayerSetAllocatableChannel(playerData->playerNo, PLAYER_SE_CH);

	result = NNS_SndArcPlayerStartSeqEx
				(&playerData->sndHandle, playerData->playerNo, -1, 126, soundIdx);

	if(result == FALSE){
		OS_Printf("sound_Effect start Error... soundIdx = %d\n", soundIdx);
		return FALSE;
	}
	return TRUE;
}

//============================================================================================
/**
 *
 *
 *
 * @brief	�����T�E���h�֐��i�eapp����Ăяo�����j
 *
 *
 *
 */
//============================================================================================
BOOL	PMSNDSYS_PlayVoice( u32 pokeNum )
{
	PMSNDSYS_PLAYER_DATA* playerData = &systemPlayerUnit.playerDataArray[PLAYER_SE];
	BOOL result;

	NNS_SndPlayerStopSeq(&playerData->sndHandle, 0);
	// �v���C���[�g�p�`�����l���ݒ�
	NNS_SndPlayerSetAllocatableChannel(playerData->playerNo, PLAYER_PMVOICE_CH);

	result = NNS_SndArcPlayerStartSeqEx
				(&playerData->sndHandle, playerData->playerNo, pokeNum, 127, SEQ_PV);

	if(result == FALSE){
		OS_Printf("voice start Error... pokeNum = %d\n", pokeNum);
		return FALSE;
	}
	return TRUE;
}

