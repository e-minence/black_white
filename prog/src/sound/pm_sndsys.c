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
	{ 0x6000, 0x0000, 2 },	// �r�d�������B�G�R�[�R�[���X�p��2seq�ݒ�
	{ 0x0000, 0x0000, 1 },	// �G�R�[�R�[���X�p

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
 * @brief	���o�[�u�\����`
 */
//------------------------------------------------------------------
typedef struct {
	BOOL	active;
	u32		samplingRate;
	u16		volume;
	int		stopFrames;
}PMSND_REVERB;

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
#define CAPTURE_BUFSIZE (0x2000)

static u8				PmSoundHeap[SOUND_HEAP_SIZE];
static NNSSndArc		PmSoundArc;

static NNSSndHeapHandle	PmSndHeapHandle;
static u32				playerNumber;

static u8				captureBuffer[ CAPTURE_BUFSIZE ] ATTRIBUTE_ALIGN(32);

static BOOL				bgmContEnable;
static u32				bgmFadeCounter;

static PMSND_FADESTATUS	fadeStatus;
static PMSND_REVERB		reverbStatus;

static PMSND_PL_UNIT	systemPlayerUnit;
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

static void PMSND_InitCaptureReverb( void );
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
	PMSND_InitCaptureReverb();

	bgmFadeCounter = 0;

	// �풓�T�E���h�f�[�^�ǂݍ���
	systemPresetHandle = SOUNDMAN_PresetGroup(GROUP_GLOBAL);
#if 0
	{
		u32 presetTbl[] = {1501, 1502};
		systemPresetHandle = SOUNDMAN_PresetSoundTbl(presetTbl,2);
	}
#endif
	size2 = NNS_SndHeapGetFreeSize(PmSndHeapHandle);

	OS_Printf("setup Sound... size(%x) heapRemains(%x)\n", size1 - size2, size2);

	bgmContEnable = TRUE;

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
	PMSND_SystemFadeBGM();

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
 * @brief	�σv���[���[�Z�b�g�A�b�v
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�v���[���[���j�b�g�쐬
 */
//------------------------------------------------------------------
static void PMSND_CreatePlayerUnit
			( const PMSND_PLSETUP* setupTbl, PMSND_PL_UNIT* playerUnit )
{
	BOOL result;
	int playerNo;
	u32 playerSize;
	int i;

	// ����Đ�����Ă���a�f�l�͋�����~
	SOUNDMAN_StopHierarchyPlayer();

	// �v���[���[���j�b�g�폜�̍ۂɕ��A����q�[�v��Ԃk�u���擾
	playerUnit->heapLvDelete = SOUNDMAN_GetHierarchyPlayerSoundHeapLv();

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
		// �`�����l���ݒ�
		NNS_SndPlayerSetAllocatableChannel(playerNo, setupTbl[i].channelBit);
		NNS_SndPlayerSetPlayableSeqCount(playerNo, setupTbl[i].playableNum);
		// �v���[���[�q�[�v�쐬
		if(playerSize){
			int j;
			for( j=0; j<setupTbl[i].playableNum; j++ ){
				result = NNS_SndPlayerCreateHeap(playerNo, PmSndHeapHandle, playerSize);
				GF_ASSERT( result == TRUE );
			}
		}

		playerUnit->playerDataArray[i].soundIdx = 0;
		NNS_SndHandleInit(&playerUnit->playerDataArray[i].sndHandle);
		playerUnit->playerDataArray[i].playerNo = playerNo;
	}
	playerNumber += playerUnit->playerNum; 

	// �K�w�v���[���[���Z�b�g�̃q�[�v�k�u���X�V
	SOUNDMAN_UpdateHierarchyPlayerSoundHeapLv();
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[���j�b�g�폜
 */
//------------------------------------------------------------------
static void PMSND_DeletePlayerUnit( PMSND_PL_UNIT* playerUnit )
{
	int i;

	for( i=0; i<playerUnit->playerNum; i++ ){
		NNS_SndHandleReleaseSeq(&playerUnit->playerDataArray[i].sndHandle);
	}
	NNS_SndHeapLoadState(PmSndHeapHandle, playerUnit->heapLvDelete);
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[���Z�b�g
 */
//------------------------------------------------------------------
static void resetSoundEffect( PMSND_PLAYER_DATA* ppd, u32 channel )
{
	NNS_SndPlayerStopSeq(&ppd->sndHandle, 0);
	// �v���C���[�g�p�`�����l���ݒ�
	NNS_SndPlayerSetAllocatableChannel(ppd->playerNo, channel);
}
	
//============================================================================================
/**
 *
 * @brief	�L���v�`���[�֐�
 *
 */
//============================================================================================
static void PMSND_InitCaptureReverb( void )
{
	reverbStatus.active = FALSE;
	reverbStatus.samplingRate = 16000;
	reverbStatus.volume = 0;
	reverbStatus.stopFrames = 0;
}

//------------------------------------------------------------------
/**
 * @brief	���o�[�u�ݒ�
 */
//------------------------------------------------------------------
void PMSND_EnableCaptureReverb( u32 samplingRate, int volume, int stopFrames )
{
	BOOL result;

	if(reverbStatus.active == TRUE){ return; }

	reverbStatus.samplingRate = samplingRate;
	if(volume > 63) volume = 63;
	reverbStatus.volume = volume;
	reverbStatus.stopFrames = stopFrames;

	result = NNS_SndCaptureStartReverb(	captureBuffer, 
										CAPTURE_BUFSIZE, 
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





//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�a�f�l�T�E���h�֐��i�eapp����Ăяo�����j
 *
 *
 *
 *
 *
 */
//============================================================================================
static inline BOOL BGM_CONTENABLE_CHECK( void )
{
	if(bgmContEnable == FALSE){
		OS_Printf("cannot control BGM this timing\n");
		return FALSE;
	}
	return TRUE;
}

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
static BOOL PMSND_PlayBGM_CORE( u32 soundIdx, u16 trackBit )
{
	BOOL result;

	if( BGM_CONTENABLE_CHECK() == FALSE ){ return FALSE; }

	SOUNDMAN_StopHierarchyPlayer();
	result = SOUNDMAN_PlayHierarchyPlayer(soundIdx);
	if( trackBit != 0xffff ){ PMSND_ChangeBGMtrack( trackBit ); }

	return result;
}

//------------------------------------------------------------------
void	PMSND_PlayBGM_EX( u32 soundIdx, u16 trackBit )
{
	PMSND_ResetSystemFadeBGM();

	PMSND_PlayBGM_CORE( soundIdx, trackBit );
}

//------------------------------------------------------------------
void	PMSND_PlayNextBGM_EX( u32 soundIdx, u16 trackBit )
{
	fadeStatus.nextSoundIdx = soundIdx;
	fadeStatus.nextTrackBit = trackBit;
	fadeStatus.active = TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h��~
 */
//------------------------------------------------------------------
void	PMSND_StopBGM( void )
{
	if( BGM_CONTENABLE_CHECK() == FALSE ){ return; }

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
	if( BGM_CONTENABLE_CHECK() == FALSE ){ return; }

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
	if( BGM_CONTENABLE_CHECK() == FALSE ){ return; }

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
	if( BGM_CONTENABLE_CHECK() == FALSE ){ return; }

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
	if( BGM_CONTENABLE_CHECK() == FALSE ){ return; }

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
	if( BGM_CONTENABLE_CHECK() == FALSE ){ return; }

	PMSND_CancelSystemFadeBGM();
	SOUNDMAN_PopHierarchyPlayer();
}

//------------------------------------------------------------------
/**
 * @brief	�a�f�l�Đ��g���b�N�ύX
 */
//------------------------------------------------------------------
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

//------------------------------------------------------------------
/**
 *
 * @brief	�V�X�e���t�F�[�h�a�f�l
 *
 */
//------------------------------------------------------------------
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

	if( fadeStatus.active == FALSE ){ return; }

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
		PMSND_PlayBGM_CORE( fadeStatus.nextSoundIdx, fadeStatus.nextTrackBit);
		NNS_SndPlayerSetVolume(pBgmHandle, 0);
		fadeStatus.volumeCounter = 0;
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
 * @brief	���o�T�E���h�֐��i�eapp����Ăяo�����j
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�V�X�e���r�d�T�E���h�֐�
 */
//------------------------------------------------------------------
void	PMSND_PlaySystemSE( u32 soundIdx )
{
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_SYSSE];

	resetSoundEffect( ppd, PLAYER_SE_CH );
	ppd->soundIdx = soundIdx;

	NNS_SndArcPlayerStartSeqEx(&ppd->sndHandle, ppd->playerNo, -1, 126, soundIdx);
}

//------------------------------------------------------------------
/**
 * @brief	�r�d�T�E���h�֐�
 */
//------------------------------------------------------------------
void	PMSND_PlaySE( u32 soundIdx )
{
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_SEVOICE];

	resetSoundEffect( ppd, PLAYER_SE_CH );
	ppd->soundIdx = soundIdx;

	NNS_SndArcPlayerStartSeqEx(&ppd->sndHandle, ppd->playerNo, -1, 126, soundIdx);
}

//------------------------------------------------------------------
/**
 * @brief	�r�d�I�����o
 */
//------------------------------------------------------------------
BOOL	PMSND_CheckPlaySE( void )
{
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_SEVOICE];
	int count = NNS_SndPlayerCountPlayingSeqByPlayerNo(ppd->playerNo);

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
void	PMSND_SetStatusSE( int tempoRatio, int pitch, int pan )
{
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_SEVOICE];

	if(tempoRatio != PMSND_NOEFFECT){
		NNS_SndPlayerSetTempoRatio(&ppd->sndHandle, tempoRatio);
	}
	if(pitch != PMSND_NOEFFECT){
		NNS_SndPlayerSetTrackPitch(&ppd->sndHandle, 0xffff, pitch);
	}
	if(pan != PMSND_NOEFFECT){
		NNS_SndPlayerSetTrackPan(&ppd->sndHandle, 0xffff, pan);
	}
}





//============================================================================================
/**
 *
 *
 *
 * @brief	�����T�E���h�֐��i�eapp����Ăяo�����j
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�����T�E���h�֐�
 */
//------------------------------------------------------------------
void	PMSND_PlayVoice( u32 pokeNum )
{
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_SEVOICE];

	resetSoundEffect( ppd, PLAYER_PMVOICE_CH );
	ppd->soundIdx = pokeNum;

	NNS_SndArcPlayerStartSeqEx(&ppd->sndHandle, ppd->playerNo, pokeNum, 127, SEQ_PV);
}

//------------------------------------------------------------------
/**
 * @brief	�����I�����o
 */
//------------------------------------------------------------------
BOOL	PMSND_CheckPlayVoice( void )
{
	return PMSND_CheckPlaySE();	//����̓v���[���[�������ɐݒ肵�Ă���̂œ�������
}

//------------------------------------------------------------------
/**
 * @brief	�����X�e�[�^�X�ύX
 */
//------------------------------------------------------------------
void	PMSND_SetStatusVoice( int tempoRatio, int pitch, int pan )
{
	PMSND_SetStatusSE(tempoRatio, pitch, pan);//����̓v���[���[�������ɐݒ肵�Ă���̂œ�������
}

//------------------------------------------------------------------
/**
 * @brief	�����T�E���h�֐��i�R�[���X���ʕt���j
 *			���f�[�^���T�E���h�q�[�v�Ƀ��[�h�����L���邱�ƂŃR�[���X���ʂ������B
 *			�@�T�E���h�q�[�v�̏�ԕ������K�v�Ȃ̂�
 *			�@�J�n���I���҂������s���A�Ԃɂa�f�l��������Ȃ��悤�ɂ��邱��
 */
//------------------------------------------------------------------
void	PMSND_PlayVoiceChorus( u32 pokeNum, int chorusPitch, int chorusVolume )
{
	PMSND_PLAYER_DATA* ppd1 = &systemPlayerUnit.playerDataArray[PLAYER_SEVOICE];
	PMSND_PLAYER_DATA* ppd2 = &systemPlayerUnit.playerDataArray[PLAYER_ECHOCHORUS];
	BOOL result;

	//OS_Printf("soundHeap remains %x\n", NNS_SndHeapGetFreeSize(PmSndHeapHandle));

	resetSoundEffect( ppd1, PLAYER_PMVOICE_CH );
	resetSoundEffect( ppd2, PLAYER_PMVOICE_CH );
	ppd1->soundIdx = pokeNum;
	ppd2->soundIdx = pokeNum;

	// �T�E���h�f�[�^�iseq, bank�j�ǂݍ���
	NNS_SndArcLoadSeq(SEQ_PV, PmSndHeapHandle);
	NNS_SndArcLoadBank(pokeNum, PmSndHeapHandle);

	NNS_SndArcPlayerStartSeqEx(&ppd1->sndHandle, ppd1->playerNo, pokeNum, 127, SEQ_PV);
	NNS_SndArcPlayerStartSeqEx(&ppd2->sndHandle, ppd2->playerNo, pokeNum, 127, SEQ_PV);
	NNS_SndPlayerSetTrackPitch(&ppd2->sndHandle, 0xffff, chorusPitch);
	NNS_SndPlayerSetVolume(&ppd2->sndHandle, chorusVolume);

	// �T�E���h�q�[�v�ꎞ�ی�̂��߂a�f�l�֘A����s�\�ɂ���
	bgmContEnable = FALSE;
}

BOOL	PMSND_CheckPlayVoiceChorus( void )
{
	PMSND_PLAYER_DATA* ppd1 = &systemPlayerUnit.playerDataArray[PLAYER_SEVOICE];
	PMSND_PLAYER_DATA* ppd2 = &systemPlayerUnit.playerDataArray[PLAYER_ECHOCHORUS];
	int count1 = NNS_SndPlayerCountPlayingSeqByPlayerNo(ppd1->playerNo);
	int count2 = NNS_SndPlayerCountPlayingSeqByPlayerNo(ppd2->playerNo);

	if(count1 | count2){
		return TRUE;
	}
	NNS_SndPlayerStopSeq(&ppd1->sndHandle, 0);
	NNS_SndPlayerStopSeq(&ppd2->sndHandle, 0);

	// �K�w�v���[���[�����݂̂a�f�l�ǂݍ��ݒ���̏�Ԃɕ���
	SOUNDMAN_RecoverHierarchyPlayerState();

	// �T�E���h�q�[�v�ꎞ�ی����
	bgmContEnable = TRUE;
	return FALSE;
}








