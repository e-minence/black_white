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

static const PMSND_PLSETUP systemPlayer[] = {
	{ 0x0000, 0x0000, 1 },
	{ 0x6000, 0x0000, 1 },
//	{ 0x6000, PLAYER_MUSIC_CH, 1 },
//	{ 0x6000, PLAYER_PMVOICE_CH, 2 },

	{ PMSND_PLSETUP_TBLEND, 0x0000, 0 },	// TABLE END
};

#define PLAYER_MUSIC_HEAPSIZ	(0x4000)
#define PLAYER_MUSIC_CH			(0xa7fe)
// �a�f�l�v���[���[��`
static const SOUNDMAN_HIERARCHY_PLAYER_DATA pmHierarchyPlayerData = {
	PLAYER_MUSIC_HEAPSIZ, PLAYER_DEFAULT_MAX, PLAYER_MUSIC_CH,
};

//------------------------------------------------------------------
/**
 * @brief	�v���[���[����`
 */
//------------------------------------------------------------------
typedef struct {
	SNDPlayerInfo			playerInfo;
	SNDTrackInfo			trackInfo[16];
}PMSND_PLAYERSTATUS;

//------------------------------------------------------------------
/**
 * @brief	�V�X�e���t�F�[�h�\����`
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

static PMSND_FADESTATUS	fadeStatus;

static PMSND_PL_UNIT systemPlayerUnit;
SOUNDMAN_PRESET_HANDLE* systemPresetHandle;

static const u32 systemPresetSoundIdxTbl[] = {
	1500, 1501, 1504, 1505, 1508, 1509, 1512, 1513,
};

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
 * @brief	�Z�b�g�A�b�v��������
 *
 */
//============================================================================================
void	PMSND_Init( void )
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
	PMSND_CreatePlayerUnit(systemPlayer, &systemPlayerUnit);

	PMSND_InitSystemFade();

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
void	PMSND_Main( void )
{
	if( fadeStatus.active == TRUE ){ PMSND_SystemFade(); }

	NNS_SndMain();

	// �T�E���h�h���C�o���X�V
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
 * @brief	�I���������j��
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
 * @brief	�σv���[���[�Z�b�g�A�b�v
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
static void PMSND_CreatePlayerUnit
			( const PMSND_PLSETUP* setupTbl, PMSND_PL_UNIT* playerUnit )
{
	BOOL result;
	int playerNo;
	u32 playerSize;
	int i;

	// �v���[���[���j�b�g�폜�̍ۂɕ��A����q�[�v��Ԃk�u��ۑ�
	playerUnit->heapLvDelete = NNS_SndHeapSaveState(PmSndHeapHandle);

	// �v���[���[���J�E���g
	i = 0;
	while(setupTbl[i].size != PMSND_PLSETUP_TBLEND){ i++; }
	playerUnit->playerNum = i;

	// �v���[���[�z��쐬
	playerUnit->playerDataArray = (PMSND_PLAYER_DATA*)NNS_SndHeapAlloc
			(PmSndHeapHandle, sizeof(PMSND_PLAYER_DATA) * playerUnit->playerNum, NULL, 0, 0 );

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
NNSSndHandle* PMSND_GetBGMhandlePointer( void )
{
	return SOUNDMAN_GetHierarchyPlayerSndHandle();
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h�Đ�
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

	SOUNDMAN_StopHierarchyPlayer();
	result = SOUNDMAN_PlayHierarchyPlayer(soundIdx);
	if( trackBit != 0xffff ){ PMSND_ChangeBGMtrack( trackBit ); }

	PMSND_ResetSystemFade();

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
// �Đ��g���b�N�ύX
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
 * @brief	�T�E���h��~
 */
//------------------------------------------------------------------
void	PMSND_StopBGM( void )
{
	SOUNDMAN_StopHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h�ꎞ��~
 */
//------------------------------------------------------------------
void	PMSND_PauseBGM( BOOL pauseFlag )
{
	NNS_SndPlayerPause(SOUNDMAN_GetHierarchyPlayerSndHandle(), pauseFlag);
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h�t�F�[�h�C��
 */
//------------------------------------------------------------------
void	PMSND_FadeInBGM( u16 frames )
{
	// ���݂�volume�𑦎��X�V
	NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0, 0);
	// �ēx�ڕW�l��ݒ�
	NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 127, frames);
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h�t�F�[�h�A�E�g
 */
//------------------------------------------------------------------
void	PMSND_FadeOutBGM( u16 frames )
{
	NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0, frames);
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h��ԕۊ�
 */
//------------------------------------------------------------------
void	PMSND_PushBGM( void )
{
	SOUNDMAN_PushHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h��Ԏ��o��
 */
//------------------------------------------------------------------
void	PMSND_PopBGM( void )
{
	SOUNDMAN_PopHierarchyPlayer();
}





//============================================================================================
/**
 *
 *
 *
 * @brief	���o�T�E���h�֐�
 *
 *
 *
 */
//============================================================================================
static BOOL playSoundEffect( PMSND_PLAYER_DATA* ppd, u32 soundIdx, u32 channel, int pri )
{
	NNS_SndPlayerStopSeq(&ppd->sndHandle, 0);
	// �v���C���[�g�p�`�����l���ݒ�
	NNS_SndPlayerSetAllocatableChannel(ppd->playerNo, channel);

	return NNS_SndArcPlayerStartSeqEx(&ppd->sndHandle, ppd->playerNo, -1, pri, soundIdx);
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
BOOL	PMSND_PlaySystemSE( u32 soundIdx )
{
	PMSND_PLAYER_DATA* playerData = &systemPlayerUnit.playerDataArray[PLAYER_SYSSE];

	if( playSoundEffect( playerData, soundIdx, PLAYER_SE_CH, 126) == FALSE ){
		OS_Printf("system sound_Effect start Error... soundIdx = %d\n", soundIdx);
		return FALSE;
	}
	return TRUE;
}

BOOL	PMSND_PlaySE( u32 soundIdx )
{
	PMSND_PLAYER_DATA* playerData = &systemPlayerUnit.playerDataArray[PLAYER_SE];

	if( playSoundEffect( playerData, soundIdx, PLAYER_SE_CH, 126) == FALSE ){
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
BOOL	PMSND_PlayVoice( u32 pokeNum )
{
	PMSND_PLAYER_DATA* playerData = &systemPlayerUnit.playerDataArray[PLAYER_SE];

	if( playSoundEffect( playerData, pokeNum, PLAYER_PMVOICE_CH, 127) == FALSE ){
		OS_Printf("voice start Error... pokeNum = %d\n", pokeNum);
		return FALSE;
	}
	return TRUE;
}





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�V�X�e���t�F�[�h
 *
 *
 *
 *
 *
 */
//============================================================================================
static void PMSND_InitSystemFade( void )
{
	fadeStatus.fadeFrames = 60;
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
	u32 nowSoundIdx = SOUNDMAN_GetHierarchyPlayerSoundIdx();
	BOOL bgmSetFlag = FALSE;

	if( nowSoundIdx ){
		if( nowSoundIdx != fadeStatus.nextSoundIdx ){
			// FadeOut
			NNS_SndPlayerMoveVolume
				(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0, fadeStatus.fadeFrames );
			if( fadeStatus.volumeCounter ){
				fadeStatus.volumeCounter--;
			} else {
				bgmSetFlag = TRUE; 
			}
		} else {
			// FadeIn
			if( fadeStatus.volumeCounter < fadeStatus.fadeFrames ){
				NNS_SndPlayerMoveVolume
					(SOUNDMAN_GetHierarchyPlayerSndHandle(), 127, fadeStatus.fadeFrames );
				fadeStatus.volumeCounter++;
			} else {
				NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 127, 0 );
				PMSND_ResetSystemFade();
			}
		}
	} else {
		bgmSetFlag = TRUE;
	}

	if(bgmSetFlag == TRUE){
		SOUNDMAN_StopHierarchyPlayer();
		SOUNDMAN_PlayHierarchyPlayer(fadeStatus.nextSoundIdx);
		// ���݂�volume�𑦎��X�V
		NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0, 0);
		fadeStatus.volumeCounter = 0;

		if( fadeStatus.nextTrackBit != 0xffff ){
			PMSND_ChangeBGMtrack(fadeStatus.nextTrackBit);
		}
	}
}

//------------------------------------------------------------------
/**
 * @brief	�V�X�e���t�F�[�h�t���[���ݒ�
 */
//------------------------------------------------------------------
void PMSND_SetSystemFadeFrames( int frames )
{
	fadeStatus.fadeFrames = frames;
}




