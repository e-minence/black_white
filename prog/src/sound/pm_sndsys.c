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
	u32				soundIdx;
	NNSSndHandle	sndHandle;
	u16				playerNo;
}PMSND_PLAYER_DATA;

typedef struct {
	PMSND_PLAYER_DATA*	playerDataArray;
	u16					playerNum;
	int					heapLvDelete;
	int					heapLvReset;
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
	PLAYER_SEVOICE,
	PLAYER_ECHOCHORUS,

	PLAYER_DEFAULT_MAX,
};

static const PMSND_PLSETUP systemPlayer[] = {
	{ 0x0000, 0x0000, 1 },	// �V�X�e���r�d
	{ 0x6000, 0x0000, 1 },	// �r�d�������B�G�R�[�R�[���X�p��2seq�ݒ�
	{ 0x6000, 0x0000, 1 },	// �G�R�[�R�[���X�p

	{ PMSND_PLSETUP_TBLEND, 0x0000, 0 },	// TABLE END
};

#define PLAYER_MUSIC_HEAPSIZ	(0x0000)
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

//------------------------------------------------------------------
/**
 * @brief	�V�X�e���G�R�[�R�[���X�\����`
 */
//------------------------------------------------------------------
typedef struct {
	BOOL	active;
	u32		soundIdx;
	u16		volume;
	int		pitch;
	int		waitCounter;
	int		waitFrames;
	NNSSndHandle	sndHandle;
}PMSND_ECHOCHORUSSTATUS;

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

static u32					bgmFadeCounter;

static PMSND_FADESTATUS			fadeStatus;
static PMSND_ECHOCHORUSSTATUS	echoChorusStatus;

static PMSND_PL_UNIT systemPlayerUnit;
SOUNDMAN_PRESET_HANDLE* systemPresetHandle;

static PMSND_PLAYERSTATUS bgmPlayerInfo;

static void PMSND_CreatePlayerUnit
		( const PMSND_PLSETUP* setupTbl, PMSND_PL_UNIT* playerUnit );
static void PMSND_DeletePlayerUnit
		( PMSND_PL_UNIT* playerUnit );

static void PMSND_InitSystemFadeBGM( void );
static void PMSND_ResetSystemFadeBGM( void );
static void PMSND_CancelSystemFadeBGM( void );
static void PMSND_SystemFadeBGM( void );

static void PMSND_InitSystemEchoChorusSE( void );
static void PMSND_ResetSystemEchoChorusSE( void );
static void PMSND_SystemEchoChorusSE( void );
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

	PMSND_InitSystemFadeBGM();
	PMSND_InitSystemEchoChorusSE();

	bgmFadeCounter = 0;

	// �풓�T�E���h�f�[�^�ǂݍ���
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
	if( fadeStatus.active == TRUE ){ PMSND_SystemFadeBGM(); }
	if( echoChorusStatus.active == TRUE ){ PMSND_SystemEchoChorusSE(); }

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
	if( bgmFadeCounter ){ bgmFadeCounter--; }
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

		playerUnit->playerDataArray[i].soundIdx = 0;
		NNS_SndHandleInit(&playerUnit->playerDataArray[i].sndHandle);
		playerUnit->playerDataArray[i].playerNo = playerNo;
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

	PMSND_ResetSystemFadeBGM();

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
	PMSND_ResetSystemFadeBGM();
	SOUNDMAN_StopHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h�ꎞ��~
 */
//------------------------------------------------------------------
void	PMSND_PauseBGM( BOOL pauseFlag )
{
	PMSND_CancelSystemFadeBGM();
	NNS_SndPlayerPause(SOUNDMAN_GetHierarchyPlayerSndHandle(), pauseFlag);
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h�t�F�[�h�C��
 */
//------------------------------------------------------------------
void	PMSND_FadeInBGM( u16 frames )
{
	PMSND_CancelSystemFadeBGM();
	// ���݂�volume�𑦎��X�V
	NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0, 0);
	// �ēx�ڕW�l��ݒ�
	NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 127, frames);

	bgmFadeCounter = frames;
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h�t�F�[�h�A�E�g
 */
//------------------------------------------------------------------
void	PMSND_FadeOutBGM( u16 frames )
{
	PMSND_CancelSystemFadeBGM();
	NNS_SndPlayerMoveVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0, frames);

	bgmFadeCounter = frames;
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h�t�F�[�h�A�E�g���o
 */
//------------------------------------------------------------------
BOOL	PMSND_CheckFadeOnBGM( void )
{
	if( bgmFadeCounter ){
		return TRUE;
	} else {
		return FALSE;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h��ԕۊ�
 */
//------------------------------------------------------------------
void	PMSND_PushBGM( void )
{
	PMSND_CancelSystemFadeBGM();
	SOUNDMAN_PushHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h��Ԏ��o��
 */
//------------------------------------------------------------------
void	PMSND_PopBGM( void )
{
	PMSND_CancelSystemFadeBGM();
	SOUNDMAN_PopHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief	�a�f�l�X�e�[�^�X�ύX
 */
//------------------------------------------------------------------
void	PMSND_SetStatusBGM( int tempoRatio, int pitch, int pan )
{
	NNSSndHandle* pBgmHandle = SOUNDMAN_GetHierarchyPlayerSndHandle();

	if(tempoRatio != PMSND_NOEFFECT){
		NNS_SndPlayerSetTempoRatio(pBgmHandle, tempoRatio);
	}
	if(pitch != PMSND_NOEFFECT){
		NNS_SndPlayerSetTrackPitch(pBgmHandle, 0xffff, pitch);
	}
	if(pan != PMSND_NOEFFECT){
		NNS_SndPlayerSetTrackPan(pBgmHandle, 0xffff, pan);
	}
}

//------------------------------------------------------------------
/**
 * @brief	�a�f�l�I�����o
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
 * @brief	���o�T�E���h�֐��i�eapp����Ăяo�����j
 *
 *
 *
 */
//============================================================================================
static void resetSoundEffect( PMSND_PLAYER_DATA* ppd, u32 channel )
{
	NNS_SndPlayerStopSeq(&ppd->sndHandle, 0);
	// �v���C���[�g�p�`�����l���ݒ�
	NNS_SndPlayerSetAllocatableChannel(ppd->playerNo, channel);
	ppd->soundIdx = 0;
}
	
//------------------------------------------------------------------
/**
 * @brief	�V�X�e���r�d�T�E���h�֐�
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
 * @brief	�r�d�T�E���h�֐�
 */
//------------------------------------------------------------------
BOOL	PMSND_PlaySE( u32 soundIdx )
{
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_SEVOICE];

	resetSoundEffect( ppd, PLAYER_SE_CH );

	ppd->soundIdx = soundIdx;
	PMSND_ResetSystemEchoChorusSE();

	return  NNS_SndArcPlayerStartSeqEx(&ppd->sndHandle, ppd->playerNo, -1, 126, soundIdx);
}

//------------------------------------------------------------------
/**
 * @brief	�����T�E���h�֐�
 */
//------------------------------------------------------------------
BOOL	PMSND_PlayVoice( u32 pokeNum )
{
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_SEVOICE];

	resetSoundEffect( ppd, PLAYER_PMVOICE_CH );

	ppd->soundIdx = pokeNum;
	PMSND_ResetSystemEchoChorusSE();

	return NNS_SndArcPlayerStartSeqEx(&ppd->sndHandle, ppd->playerNo, pokeNum, 127, SEQ_PV);
}

//------------------------------------------------------------------
/**
 * @brief	�r�d�I�����o
 */
//------------------------------------------------------------------
BOOL	PMSND_CheckPlaySEVoice( void )
{
	PMSND_PLAYER_DATA* playerData = &systemPlayerUnit.playerDataArray[PLAYER_SEVOICE];
	int count = NNS_SndPlayerCountPlayingSeqByPlayerNo(playerData->playerNo);

	if( count ){
		return TRUE;
	} else {
		return FALSE;
	}
}

//------------------------------------------------------------------
/**
 * @brief	�r�d�X�e�[�^�X�ύX
 */
//------------------------------------------------------------------
void	PMSND_SetStatusSEVoice( int tempoRatio, int pitch, int pan )
{
	PMSND_PLAYER_DATA* playerData = &systemPlayerUnit.playerDataArray[PLAYER_SEVOICE];

	if(tempoRatio != PMSND_NOEFFECT){
		NNS_SndPlayerSetTempoRatio(&playerData->sndHandle, tempoRatio);
	}
	if(pitch != PMSND_NOEFFECT){
		NNS_SndPlayerSetTrackPitch(&playerData->sndHandle, 0xffff, pitch);
	}
	if(pan != PMSND_NOEFFECT){
		NNS_SndPlayerSetTrackPan(&playerData->sndHandle, 0xffff, pan);
	}
}





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�V�X�e���t�F�[�h�a�f�l
 *
 *
 *
 *
 *
 */
//============================================================================================
static void PMSND_InitSystemFadeBGM( void )
{
	fadeStatus.fadeFrames = 180;
	PMSND_ResetSystemFadeBGM();
}

static void PMSND_ResetSystemFadeBGM( void )
{
	fadeStatus.active = FALSE;
	fadeStatus.nextTrackBit = 0xffff;
	fadeStatus.nextSoundIdx = 0;
	fadeStatus.volumeCounter = fadeStatus.fadeFrames;
	//�ő�l�ɍX�V
	NNS_SndPlayerSetVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 127);
}

static void PMSND_CancelSystemFadeBGM( void )
{
	if(fadeStatus.active == TRUE){
		u32	nowSoundIdx = SOUNDMAN_GetHierarchyPlayerSoundIdx();
		if( nowSoundIdx != fadeStatus.nextSoundIdx ){
			PMSND_PlayBGM_EX(fadeStatus.nextSoundIdx, fadeStatus.nextTrackBit);
		} else {
			PMSND_ResetSystemFadeBGM();
		}
	}
}

//------------------------------------------------------------------
static void PMSND_SystemFadeBGM( void )
{
	NNSSndHandle*	pBgmHandle = SOUNDMAN_GetHierarchyPlayerSndHandle();
	u32				nowSoundIdx = SOUNDMAN_GetHierarchyPlayerSoundIdx();
	BOOL			bgmSetFlag = FALSE;

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
				PMSND_ResetSystemFadeBGM();
				return;			//�I��
			}
		}
	} else {
		bgmSetFlag = TRUE;	// �ŏ��ɖ�o���ꍇ
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
		int volume = fadeStatus.volumeCounter * 127 / fadeStatus.fadeFrames;

		NNS_SndPlayerSetVolume(pBgmHandle, volume);
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





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�V�X�e���G�R�[���R�[���X�r�d
 *
 *
 *
 *
 *
 */
//============================================================================================
static void PMSND_InitSystemEchoChorusSE( void )
{
	echoChorusStatus.active = FALSE;
	echoChorusStatus.volume = 127;
	echoChorusStatus.pitch = 0;
	echoChorusStatus.waitFrames = 0;

	PMSND_ResetSystemEchoChorusSE();
	NNS_SndHandleInit(&echoChorusStatus.sndHandle);
}

static void PMSND_ResetSystemEchoChorusSE( void )
{
	PMSND_PLAYER_DATA* ppd_org = &systemPlayerUnit.playerDataArray[PLAYER_SEVOICE];
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_ECHOCHORUS];
	ppd->soundIdx = ppd_org->soundIdx;
	echoChorusStatus.soundIdx = ppd_org->soundIdx;
	echoChorusStatus.waitCounter = echoChorusStatus.waitFrames;
}

//------------------------------------------------------------------
static void PMSND_SystemEchoChorusSE( void )
{
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_ECHOCHORUS];
	u32 soundIdx = echoChorusStatus.soundIdx;
	BOOL result;

	if(ppd->soundIdx == 0){ return; }

	if(echoChorusStatus.waitCounter){
		echoChorusStatus.waitCounter--;
		return;
	}
	NNS_SndPlayerStopSeq(&ppd->sndHandle, 0);

	if((ppd->soundIdx >= PMSND_SE_START)&&(ppd->soundIdx <= PMSND_SE_END)){
		resetSoundEffect( ppd, PLAYER_SE_CH );
		result = NNS_SndArcPlayerStartSeqEx
			(&ppd->sndHandle, ppd->playerNo, -1, 127, ppd->soundIdx);
		NNS_SndPlayerSetTrackPitch(&ppd->sndHandle, 0xffff, echoChorusStatus.pitch);
		if(result == FALSE){ OS_Printf("echochorus error... play se\n"); }

	} else if((ppd->soundIdx >= PMSND_VOICE_START)&&(ppd->soundIdx <= PMSND_VOICE_END)){
		resetSoundEffect( ppd, PLAYER_PMVOICE_CH );
		result = NNS_SndArcPlayerStartSeqEx
			(&ppd->sndHandle, ppd->playerNo, ppd->soundIdx, 127, SEQ_PV);
		NNS_SndPlayerSetTrackPitch(&ppd->sndHandle, 0xffff, echoChorusStatus.pitch);
		if(result == FALSE){ OS_Printf("echochorus error... play voice\n"); }
	}
	ppd->soundIdx = 0;
	echoChorusStatus.soundIdx = 0;
}

//------------------------------------------------------------------
/**
 * @brief	�V�X�e���G�R�[���R�[���X�r�d�ݒ�
 */
//------------------------------------------------------------------
void PMSND_EnableSystemEchoChorus( int volume, int pitch, int waitFrames )
{
	echoChorusStatus.active = TRUE;
	echoChorusStatus.volume = volume;
	echoChorusStatus.pitch = pitch;
	echoChorusStatus.waitFrames = waitFrames;

	echoChorusStatus.active = FALSE;
}

void PMSND_DisableSystemEchoChorus( void )
{
	echoChorusStatus.active = FALSE;
}




