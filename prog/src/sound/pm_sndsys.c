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
 * @brief	�r�d�v���[���[�ݒ��`
 */
//------------------------------------------------------------------
typedef struct {
	u32						soundIdx;
	NNSSndHandle	sndHandle;
}PMSND_SEPLAYER_DATA;
//------------------------------------------------------------------
/**
 * @brief	�V�X�e���t�F�[�h�\����`
 */
//------------------------------------------------------------------
typedef struct {
	BOOL	active;
	int		seq;
	u16		volumeCounter;
	u16		nextTrackBit;
	u32		nextSoundIdx;
	int		fadeInFrame;
	int		fadeOutFrame;
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
	u16		depth;
	int		stopFrames;
}PMSND_REVERB;

//------------------------------------------------------------------
/**
 * @brief	�X���b�h�����`
 */
//------------------------------------------------------------------
#define BGM_BLOCKLOAD_SIZE  (0x2000)

typedef enum {
	THREADLOAD_SEQBANK = 0,
	THREADLOAD_WAVE,
}THREADLOAD_MODE;

// �X���b�h�g�p�����\����
typedef struct {
	u32	soundIdx;
}THREAD_ARG;

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
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE //�ʏ펞����
#define	SOUND_HEAP_SIZE	(0x0a0000 - 30000 - 12288)   //640K - 30000 - 0x3000
#else                   //DL�q�@������
#define	SOUND_HEAP_SIZE	(0x030000)   //���ɓK��
#endif //MULTI_BOOT_MAKE
#define CAPTURE_BUFSIZE (0x2000)
#define TRACK_NUM	(16)

static u8												PmSoundHeap[SOUND_HEAP_SIZE];
static u8												captureBuffer[ CAPTURE_BUFSIZE ] ATTRIBUTE_ALIGN(32);
static NNSSndArc								PmSoundArc;
static NNSSndHeapHandle					PmSndHeapHandle;
static u32											bgmFadeCounter;
static PMSND_FADESTATUS					fadeStatus;
static PMSND_REVERB							reverbStatus;
static SOUNDMAN_PRESET_HANDLE*	systemPresetHandle;
static SOUNDMAN_PRESET_HANDLE*	usrPresetHandle1;
static PMSND_SEPLAYER_DATA			sePlayerData[SEPLAYER_MAX];
static SNDTrackInfo							trackInfo[TRACK_NUM];
static u16											trackActiveBit;	
//static OSMutex									sndTreadMutex;		
//
#ifdef PM_DEBUG
static u32 heapRemainsAfterSys;
static u32 heapRemainsAfterPlayer;
static u32 heapRemainsAfterPresetSE;
#endif

#define THREAD_STACKSIZ		(0x1000)
OSThread		soundLoadThread;
static u64	threadStack[ THREAD_STACKSIZ / sizeof(u64) ];
THREAD_ARG	threadArg;

static void PMSND_InitCore( void );

static void	PMSND_InitSEplayer( void );
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
 * @brief	�Z�b�g�A�b�v��������
 *
 */
//============================================================================================
void	PMSND_Init( void )
{
	// �T�E���h�V�X�e��������
	NNS_SndInit();
	PmSndHeapHandle = NNS_SndHeapCreate(PmSoundHeap, SOUND_HEAP_SIZE);

	// �T�E���h�̐ݒ�
	NNS_SndArcInitWithResult( &PmSoundArc, "wb_sound_data.sdat", PmSndHeapHandle, FALSE );

  PMSND_InitCore();
  
	// �풓�T�E���h�f�[�^�ǂݍ���
	systemPresetHandle = SOUNDMAN_PresetGroup(GROUP_GLOBAL);
	usrPresetHandle1 = NULL;
#ifdef PM_DEBUG
	heapRemainsAfterPresetSE = NNS_SndHeapGetFreeSize(PmSndHeapHandle);
#endif
	debugBGMsetFlag = FALSE;
  
}
void	PMSND_InitMultiBoot( void* sndData )
{
	// �T�E���h�V�X�e��������(�}���`�u�[�g�q�@
	// �������f�[�^��32�o�C�g�A���C�����g
	NNS_SndInit();
	PmSndHeapHandle = NNS_SndHeapCreate(PmSoundHeap, SOUND_HEAP_SIZE);

	// �T�E���h�̐ݒ�
	NNS_SndArcInitOnMemory( &PmSoundArc, sndData );

  PMSND_InitCore();

	debugBGMsetFlag = FALSE;
}

//---------------------------------------------------
//  �������R�A����
//---------------------------------------------------
static void PMSND_InitCore( void )
{
#ifdef PM_DEBUG
	heapRemainsAfterSys = NNS_SndHeapGetFreeSize(PmSndHeapHandle);
#endif
	// �T�E���h�v���[���[�̐ݒ�i�T�E���h�f�[�^�ɒ�`����Ă�����̂��Q�Ƃ���j
	NNS_SndArcPlayerSetup(PmSndHeapHandle);
#ifdef PM_DEBUG
	heapRemainsAfterPlayer = NNS_SndHeapGetFreeSize(PmSndHeapHandle);
#endif
	// �T�E���h�Ǘ�������
	SOUNDMAN_Init(&PmSndHeapHandle);

	// �K�w�v���[���[���Z�b�g�̃q�[�v�k�u���X�V
	SOUNDMAN_UpdateHierarchyPlayerSoundHeapLv();

	// �r�d�v���[���[�f�[�^������
	PMSND_InitSEplayer();
	// �T�E���h�K�w�\��������
	SOUNDMAN_InitHierarchyPlayer(PLAYER_BGM);

	PMSND_InitSystemFadeBGM();
	PMSND_InitCaptureReverb();

	// �r������pMutex������(NNS_SndMain���X���b�h�Z�[�t�݌v�ł͂Ȃ�����)
	//OS_InitMutex(&sndTreadMutex);		

	trackActiveBit = 0;	
	bgmFadeCounter = 0;
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
	// ���݂̃g���b�N��Ԃ��擾
	{
		NNSSndHandle* sndHandle = PMSND_GetBGMhandlePointer();
		int i;

		trackActiveBit = 0;
		for( i=0; i<TRACK_NUM; i++ ){
			if(NNS_SndPlayerReadDriverTrackInfo(sndHandle, i, &trackInfo[i]) == TRUE){
				trackActiveBit |= (0x0001 << i);	// �L��/������BitField�ɕϊ�
			}
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
//�}���`�u�[�g�p���蕪��
#ifndef MULTI_BOOT_MAKE  //�ʏ펞����
	SOUNDMAN_ReleasePresetData(systemPresetHandle);
#endif //MULTI_BOOT_MAKE

	NNS_SndHeapDestroy(PmSndHeapHandle);
}

//============================================================================================
/**
 *
 * @brief	���擾
 *
 */
//============================================================================================
#ifdef PM_DEBUG
u32 PMSND_GetSndHeapSize( void )									{ return SOUND_HEAP_SIZE; }
u32 PMSND_GetSndHeapRemainsAfterSys( void )				{ return heapRemainsAfterSys; }
u32 PMSND_GetSndHeapRemainsAfterPlayer( void )		{ return heapRemainsAfterPlayer; }
u32 PMSND_GetSndHeapRemainsAfterPresetSE( void )	{ return heapRemainsAfterPresetSE; }
u32 PMSND_GetSEPlayerNum( void )									{ return SEPLAYER_MAX; }
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
	return	SOUNDMAN_GetHierarchyPlayerPlayerNoIdx() + 1;
}
 
BOOL PMSND_CheckOnReverb( void )
{
	return reverbStatus.active;
}

u8 PMSND_GetBGMTrackVolume( int trackNo )
{
	if(trackNo >= TRACK_NUM){ return 0; }
	if(trackActiveBit	& (0x0001 << trackNo)){
		return trackInfo[TRACK_NUM].volume;
	}
	return 0;
}

BOOL PMSND_IsLoading( void )
{
	return OS_IsThreadTerminated(&soundLoadThread) == FALSE;
}

//============================================================================================
/**
 *
 * @brief	�v���[���[�̉��ʃR���g���[��(ON/OFF)
 *					������32bit = 32�v���[���[�܂őΉ�
 *					in: bitmask : �ŉ���bit����0�`�̃v���[���[�w��
 */
//============================================================================================
void PMSND_AllPlayerVolumeEnable( BOOL playerON, u32 bitmask )
{
	int i;
	int volume;

	if(playerON == TRUE){
		volume = 127;
	} else {
		volume = 0;
	}

	for(i=0; i<PMSND_PLAYER_MAX; i++){
		if(bitmask & (1<<i)){
			NNS_SndPlayerSetPlayerVolume( PLAYER_BGM, volume );
		}
	}
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
	reverbStatus.depth = CAPTURE_BUFSIZE;
	reverbStatus.stopFrames = 0;
}

//------------------------------------------------------------------
/**
 * @brief	���o�[�u�ݒ�
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

	result = NNS_SndCaptureStartReverb(	captureBuffer, 
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
		PMSND_EnableCaptureReverb(	reverbStatus.depth, reverbStatus.samplingRate, 
									reverbStatus.volume, reverbStatus.stopFrames );
	}
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
//------------------------------------------------------------------
/**
 * @brief	�T�E���h��~
 */
//------------------------------------------------------------------
static void	PMSND_StopBGM_CORE( void )
{
	NNS_SndPlayerStopSeq(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0);
	SOUNDMAN_UnloadHierarchyPlayer();
}
//------------------------------------------------------------------
void	PMSND_StopBGM( void )
{
	PMSND_ResetSystemFadeBGM();
	PMSND_StopBGM_CORE();
}

//------------------------------------------------------------------
/**
 * @brief	�T�E���h�Đ�
 */
//------------------------------------------------------------------
static BOOL PMSND_PlayBGM_CORE( u32 soundIdx, u16 trackBit )
{
	BOOL result;

  // ���[�h�X���b�h���쒆�͍Đ����Ȃ�
  if( PMSND_IsLoading() ) { return FALSE; }

	PMSND_StopBGM_CORE();
	result = SOUNDMAN_LoadHierarchyPlayer(soundIdx);
	if( result == FALSE){ return FALSE; }

	// �T�E���h�Đ��J�n
	result = NNS_SndArcPlayerStartSeqEx
					(SOUNDMAN_GetHierarchyPlayerSndHandle(), PLAYER_BGM, -1, -1, soundIdx);
	if( result == FALSE){ return FALSE; }

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
void	PMSND_PlayNextBGM_EX( u32 soundIdx, u16 trackBit, u8 fadeOutFrame, u8 fadeInFrame )
{
	PMSND_SetSystemFadeBGM(soundIdx, trackBit, fadeOutFrame, fadeInFrame );
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

	// �ۊǂ̍ۂ̓g���b�N�p�����[�^�̓��Z�b�g���Ă����B
	// �����̂܂܂̏�ԂŕۊǏo����̂����A�e�p�����[�^�l�̎擾���o���Ȃ�
	//	sound_manager���ɊǗ�������ƃg���b�N�S�Ă̏��������ƂɂȂ��Ă��܂��̂�
	// ����ł͐ݒ�Ǘ��̓��[�U�[���ōs���悤�ɂ���B
	NNS_SndPlayerSetTrackMute(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0xffff, FALSE );
	NNS_SndPlayerSetTrackModDepth(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0xffff, 0 );
	NNS_SndPlayerSetTrackModSpeed(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0xffff, 16 );

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
	if( SOUNDMAN_GetHierarchyPlayerPlayerNoIdx() == 0 ){ return; }

	PMSND_StopBGM_CORE();
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
void	PMSND_SetStatusBGM_EX( u16 trackBit, int tempoRatio, int pitch, int pan )
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
 * @brief   �a�f�l�{�����[���ύX
 */ 
//------------------------------------------------------------------
void PMSND_ChangeBGMVolume( u16 trackBit, int volume )
{
	NNSSndHandle* pBgmHandle = SOUNDMAN_GetHierarchyPlayerSndHandle();
	NNS_SndPlayerSetTrackVolume( pBgmHandle, trackBit, volume );
}

//------------------------------------------------------------------
/**
 * @brief	�a�f�l�I�����o
 */
//------------------------------------------------------------------
BOOL	PMSND_CheckPlayBGM( void )
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
 * @brief	�V�X�e���t�F�[�h�a�f�l
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
	//�ő�l�ɍX�V
	NNS_SndPlayerSetVolume(SOUNDMAN_GetHierarchyPlayerSndHandle(), 127);

	fadeStatus.seq = 0;
	deleteSoundPlayThread();
}

static void PMSND_CancelSystemFadeBGM( void )
{
	if(fadeStatus.active == TRUE){
		u32	nowSoundIdx = SOUNDMAN_GetHierarchyPlayerSoundIdx();
		if( nowSoundIdx != fadeStatus.nextSoundIdx ){
			while(fadeStatus.seq != 0){
				PMSND_SystemFadeBGM();
				OS_Printf("�T�E���h�ǂݍ��݂��������Ă��Ȃ��̂Ɉꎞ��~��K�w��������悤�Ƃ��Ă���\n");
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
	NNSSndHandle*	pBgmHandle = SOUNDMAN_GetHierarchyPlayerSndHandle();
	u32				nowSoundIdx = SOUNDMAN_GetHierarchyPlayerSoundIdx();
	BOOL			bgmSetFlag = FALSE;

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
				return;			//�I��
			}
		}
		PMSND_StopBGM_CORE();

		if(fadeStatus.nextSoundIdx == 0){
			PMSND_ResetSystemFadeBGM();
			return;			//�I��
		}
		NNS_SndArcSetLoadBlockSize(BGM_BLOCKLOAD_SIZE);	//�������[�h�w��

		createSoundPlayThread(fadeStatus.nextSoundIdx, THREADLOAD_SEQBANK);
		fadeStatus.volumeCounter = 0;
		fadeStatus.seq = 1;
		break;
	case 1:
		if(checkEndSoundPlayThread() == TRUE){
			SOUNDMAN_LoadHierarchyPlayer_forThread_heapsvSB();// �T�E���h�K�w�\���p�ݒ�

			createSoundPlayThread( fadeStatus.nextSoundIdx, THREADLOAD_WAVE);
			fadeStatus.seq = 2;
		} else {
			OS_Sleep(1);
		}
		break;
	case 2:
		if(checkEndSoundPlayThread() == TRUE){
			NNS_SndArcSetLoadBlockSize(0);	//�������[�h�Ȃ��ɕ��A

			SOUNDMAN_LoadHierarchyPlayer_forThread_end(fadeStatus.nextSoundIdx);

			// �T�E���h�Đ��J�n
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
 * @brief	�V�X�e���t�F�[�h�t���[���ݒ�
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
 * @brief	�r�d�T�E���h�֐��i�eapp����Ăяo�����j
 *
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�r�d�v���[���[�f�[�^������
 */
//------------------------------------------------------------------
static void	PMSND_InitSEplayer( void )
{
	int i;

	for( i=0; i<SEPLAYER_MAX; i++ ){
		sePlayerData[i].soundIdx = 0;
		NNS_SndHandleInit(&sePlayerData[i].sndHandle);
	}
}

//------------------------------------------------------------------
/**
 * @brief	�r�d�v���[���[�n���h���擾
 */
//------------------------------------------------------------------
NNSSndHandle* PMSND_GetSE_SndHandle( SEPLAYER_ID sePlayerID )
{
	return &sePlayerData[sePlayerID].sndHandle;
}

//------------------------------------------------------------------
/**
 * @brief	�r�d�v���[���[�����ݒ�h�c�擾
 */
//------------------------------------------------------------------
SEPLAYER_ID	PMSND_GetSE_DefaultPlayerID( u32 soundIdx )
{
	const NNSSndSeqParam* seqParam;
  if (soundIdx < PMSND_SE_START || soundIdx >= PMSND_SE_END) {
    GF_ASSERT_MSG(0, "ID(%d) is not SE ID!!", soundIdx);
    return SEPLAYER_SE1;
  }
  seqParam = NNS_SndArcGetSeqParam( soundIdx );

	return (SEPLAYER_ID)(seqParam->playerNo - PLAYER_SE_SYS);
}

//--------------------------------------------------------------
/**
 * @brief	�V�[�P���X�̏����{�����[���ݒ�
 *
 * @param	p		�T�E���h�n���h���̃A�h���X
 * @param	vol		�{�����[��(0-127)
 *
 * @retval	none
 *
 * �T�E���h�n���h���������̏ꍇ�́A�������܂���B 
 *
 * �{�����[���̃f�t�H���g�l�́A�ő��127�ł��B
 * ���̒l�̉e���̓V�[�P���X�S�̂ɂ�����܂��B
 *
 * �����ŌĂяo�����NNS_SndPlayerSetInitialVolume�֐��́A
 * NNS_SndArcPlayerStartSeq*�֐��� NNS_SndArcPlayerStartSeqArc*�֐�����
 * �Ăт�����Ă��܂��B�ēx�A���̊֐����Ăт����ƁA�ݒ肵���l���㏑������܂��B
 * �㏑���������Ȃ��ꍇ�́A NNS_SndPlayerSetVolume�֐��Ȃǂ��g���Ă��������B
 *
 * ��
 * PMSND_PlaySE( no );
 * PMSND_PlayerSetInitialVolume( handle. 30 );
 * �{�����[��30�ōĐ������
 *
 * ���̂��ƁA
 * PMSND_PMVoicePlay( no );
 * �f�t�H���g�̒l127�ōĐ������(���ɖ߂��Ă���)
 *
 * �t�ɂ����ƁA��Ƀ{�����[��30�ɂ�����������A
 * PMSND_PlayerSetInitialVolume( handle. 30 );
 * �𖈉�Z�b�g����
 */
//--------------------------------------------------------------
void PMSND_PlayerSetInitialVolume( SEPLAYER_ID sePlayerID, u32 vol )
{
	//�G���[���
	if( vol > 127 ){
		vol = 127;
	}

	//���̊֐��́ANNS_SndArcPlayerStartSeq*�֐��� NNS_SndArcPlayerStartSeqArc*�֐�����
	//�Ăт�����Ă��܂��B�ēx�A���̊֐����Ăт����ƁA�ݒ肵���l���㏑������܂��B
	//�㏑���������Ȃ��ꍇ�́A NNS_SndPlayerSetVolume�֐��Ȃǂ��g���Ă��������B
	NNS_SndPlayerSetInitialVolume( &sePlayerData[sePlayerID].sndHandle, vol );
	return;
}

//------------------------------------------------------------------
/**
 * @brief	�r�d�T�E���h�Đ��֐�
 */
//------------------------------------------------------------------
void	PMSND_PlaySE_byPlayerID( u32 soundIdx, SEPLAYER_ID sePlayerID )
{
	int playerNo = sePlayerID + PLAYER_SE_SYS;
	BOOL result;

	sePlayerData[sePlayerID].soundIdx = 0;
	result = NNS_SndArcPlayerStartSeqEx
		(&sePlayerData[sePlayerID].sndHandle, playerNo, -1, -1, soundIdx);
	if(result == TRUE){ sePlayerData[sePlayerID].soundIdx = soundIdx; }
}

static void pmsnd_PlaySECore( u32 soundIdx, u32 volume )
{
	BOOL result;
	SEPLAYER_ID	sePlayerID;

  // ���[�h�X���b�h���쒆�͍Đ����Ȃ�
  if( PMSND_IsLoading() ) { return; }

	sePlayerID = PMSND_GetSE_DefaultPlayerID(soundIdx);
	sePlayerData[sePlayerID].soundIdx = 0;
	result = NNS_SndArcPlayerStartSeq(&sePlayerData[sePlayerID].sndHandle, soundIdx);
	if(result == TRUE){
    sePlayerData[sePlayerID].soundIdx = soundIdx;
    if(volume < 128){
      PMSND_PlayerSetInitialVolume( sePlayerID, volume );
    }
  }
}

void	PMSND_PlaySE( u32 soundIdx )
{
  pmsnd_PlaySECore( soundIdx, 0xFFFFFFFF );
}
/*
 *  @brief  SE���{�����[���w��t���ōĐ�
 *
 *  @param  soundIdx  �Đ�������SENo
 *  @param  volume    �w��{�����[��(�L���l 0-127 �f�t�H���g��127)
 */
void	PMSND_PlaySEVolume( u32 soundIdx, u32 volume )
{
  pmsnd_PlaySECore( soundIdx, volume );
}

//------------------------------------------------------------------
/**
 * @brief	�r�d�T�E���h��~�֐�
 */
//------------------------------------------------------------------
void	PMSND_StopSE_byPlayerID( SEPLAYER_ID sePlayerID )
{
	NNS_SndPlayerStopSeq(&sePlayerData[sePlayerID].sndHandle, 0);
}

void	PMSND_StopSE( void )
{
	int i;

	for( i=0; i<SEPLAYER_MAX; i++ ){ PMSND_StopSE_byPlayerID((SEPLAYER_ID)i); }
}

//------------------------------------------------------------------
/**
 * @brief	�r�d�I�����o
 */
//------------------------------------------------------------------
BOOL	PMSND_CheckPlaySE_byPlayerID( SEPLAYER_ID sePlayerID )
{
	int playerNo = sePlayerID + PLAYER_SE_SYS;
	int count = NNS_SndPlayerCountPlayingSeqByPlayerNo(playerNo);

	if( count )	{ return TRUE; }
	return FALSE;
}

BOOL	PMSND_CheckPlaySE( void )
{
	int i;

	for( i=0; i<SEPLAYER_MAX; i++ ){
		if( PMSND_CheckPlaySE_byPlayerID((SEPLAYER_ID)i) == TRUE ){ return TRUE; }
	}
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�r�d�Đ����o
 */
//------------------------------------------------------------------
BOOL	PMSND_CheckPlayingSEIdx( u32 soundIdx )
{
	int count = NNS_SndPlayerCountPlayingSeqBySeqNo( soundIdx );

	if( count )	{ return TRUE; }
	return FALSE;
}

//------------------------------------------------------------------
/**
 * @brief	�r�d�X�e�[�^�X�ύX
 */
//------------------------------------------------------------------
void	PMSND_SetStatusSE_byPlayerID( SEPLAYER_ID sePlayerID, int tempoRatio, int pitch, int pan )
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

void	PMSND_SetStatusSE( int tempoRatio, int pitch, int pan )
{
	int i;

	for( i=0; i<SEPLAYER_MAX; i++ ){
		PMSND_SetStatusSE_byPlayerID((SEPLAYER_ID)i, tempoRatio, pitch, pan );
	}
}


//============================================================================================
/**
 *
 *
 * @brief	�v���Z�b�g�֐����b�p�[
 *					�Ƃ肠�����V�X�e���Ńn���h�����P�����p�ӂ��ȈՓo�^�\�ɂ���
 *
 *
 */
//============================================================================================
void	PMDSND_PresetSoundTbl( const u32* soundIdxTbl, u32 tblNum )
{
	PMSND_ReleasePreset();
	usrPresetHandle1 = SOUNDMAN_PresetSoundTbl(soundIdxTbl, tblNum);
}

void	PMSND_PresetGroup( u32 groupIdx )
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


//============================================================================================
/**
 *
 *
 * @brief	�T�E���h�t�@�C���������[�h�X���b�h
 *
 *
 */
//============================================================================================
//�X���b�h�֐�
/// BGM�̕����ǂݍ��݂̃T�C�Y  
//--------------------------------------------------------------------------------------------
static void	_loadSeqBankThread( void* arg )
{
	THREAD_ARG* arg1 = (THREAD_ARG*)arg;
	BOOL result;

	// �T�E���h�f�[�^�iseq, bank�j�ǂݍ���
	result = NNS_SndArcLoadSeqEx
		(arg1->soundIdx, NNS_SND_ARC_LOAD_SEQ | NNS_SND_ARC_LOAD_BANK, PmSndHeapHandle);
	if( result == FALSE){ OS_Printf("sound seqbank load error!\n"); }
}

static void	_loadWaveThread( void* arg )
{
	THREAD_ARG* arg1 = (THREAD_ARG*)arg;
	BOOL result;

	// �T�E���h�f�[�^�iwave�j�ǂݍ���
	result = NNS_SndArcLoadSeqEx(arg1->soundIdx, NNS_SND_ARC_LOAD_WAVE, PmSndHeapHandle);
	if( result == FALSE){ OS_Printf("sound wave load error!\n"); }
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
}

static void deleteSoundPlayThread( void )
{
	if( OS_IsThreadTerminated(&soundLoadThread) == FALSE ){
		OS_DestroyThread(&soundLoadThread);
	}
}

static BOOL checkEndSoundPlayThread( void )
{
	return OS_IsThreadTerminated(&soundLoadThread);
}


//-------------------------------------------------------------------------------------------- 
/**
 * @brief �������[�h�E�Đ�
 *
 * @param no    �Đ�����BGM�i���o�[
 * @param seq   �V�[�P���X�Ǘ��|�C���^
 * @param start TRUE �ŕ������[�h�J�n, FALSE �ŃV�[�P���X��i�߂�
 *
 * @return �Đ����J�n������ TRUE
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
			NNS_SndArcSetLoadBlockSize(BGM_BLOCKLOAD_SIZE);	//�������[�h�w��
			createSoundPlayThread(no, THREADLOAD_SEQBANK);
			(*seq)++;
			break;
		case 1:
			if(checkEndSoundPlayThread() == TRUE){
				SOUNDMAN_LoadHierarchyPlayer_forThread_heapsvSB();// �T�E���h�K�w�\���p�ݒ�

				createSoundPlayThread( no, THREADLOAD_WAVE);
				(*seq)++;
			} else {
				OS_Sleep(1);
			}
			break;
		case 2:
			if(checkEndSoundPlayThread() == TRUE){
				NNS_SndArcSetLoadBlockSize(0);	//�������[�h�Ȃ��ɕ��A
	
				SOUNDMAN_LoadHierarchyPlayer_forThread_end(no);
	
				// �T�E���h�Đ��J�n
				{
					NNSSndHandle*	pBgmHandle = SOUNDMAN_GetHierarchyPlayerSndHandle();

					NNS_SndArcPlayerStartSeqEx(pBgmHandle, PLAYER_BGM, -1, -1, no);
					//NNS_SndPlayerSetVolume(pBgmHandle, 0);
				}
				(*seq)++;
        return TRUE;
      } else {
        OS_Sleep(1);
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
 * @brief	�O�����y�f�[�^�Đ�
 *
 *
 */
//============================================================================================
#if 0
BOOL  PMDSND_PresetExtraMusic( void* seqAdrs, void* bnkAdrs, u32 waveNo )
{
	SOUNDMAN_LoadHierarchyPlayer_forThread_heapsvSB();

	// ���O�ɔg�`�ǂݍ���
	if(NNS_SndArcLoadWaveArc(waveNo, PmSndHeapHandle) == FALSE ){
		return FALSE;		// �g�`�ǂݍ��ݎ��s
	}
	// �_�~�[���y��FileID����̓A�h���X�ɒu��������
	{
		u32 bnkFileID = NNS_SndArcGetBankInfo(BANK_MUS_WB_SHINKA)->fileId;
		u32 seqFileID = NNS_SndArcGetSeqInfo(SEQ_BGM_SHINKA)->fileId;

		NNS_SndArcSetFileAddress(bnkFileID, bnkAdrs); 
		NNS_SndArcSetFileAddress(seqFileID, seqAdrs); 
	}
	SOUNDMAN_LoadHierarchyPlayer_forThread_end(SEQ_BGM_SHINKA);
	{
		// �������o���N��������������
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

	// ���O�ɔg�`�ǂݍ���
	for(i=0; i<4; i++){
		if(bnkInfo->waveArcNo[i] != 0xFFFF ){
			if(NNS_SndArcLoadWaveArc(bnkInfo->waveArcNo[i], PmSndHeapHandle) == FALSE ){
				return FALSE;		// �g�`�ǂݍ��ݎ��s
			}
			//OS_Printf("�O��BGM�Đ�...���O�g�`���[�h waveID(%d)\n", bnkInfo->waveArcNo[i]);
		}
	}
	// �_�~�[���y��FileID����̓A�h���X�ɒu��������
	NNS_SndArcSetFileAddress(bnkFileID, bnkAdrs); 
	NNS_SndArcSetFileAddress(seqFileID, seqAdrs); 

	SOUNDMAN_LoadHierarchyPlayer_forThread_end(dummyNo);

	return TRUE;
}

BOOL  PMDSND_ChangeWaveData
			( u32 waveArcDstID, u32 waveDstIdx, void* waveArcSrcAdrs, u32 waveSrcIdx)
{
	const NNSSndArcWaveArcInfo* waveInfo;
	SNDWaveArc*									waveArcDst;
	SNDWaveArc*									waveArcSrc = (SNDWaveArc*)waveArcSrcAdrs;
	const SNDWaveData*					waveData;

	// �g�`�t�@�C��ID�擾
	waveInfo = NNS_SndArcGetWaveArcInfo( waveArcDstID );
	if( waveInfo == NULL ){ return FALSE; }

	// �����ւ���̔g�`�A�h���X�擾
	waveArcDst = (SNDWaveArc*)NNS_SndArcGetFileAddress( waveInfo->fileId );
	if(waveArcDst == NULL){ return FALSE; }
	// �����ւ����̔g�`�A�h���X�擾
	waveData = SND_GetWaveDataAddress( waveArcSrc, waveSrcIdx );	

	// �g�`�A�h���X��������
	SND_SetWaveDataAddress( waveArcDst, waveDstIdx, waveData );

	return TRUE;
}

BOOL  PMDSND_PlayExtraMusic( u32 dummyNo )
{
	return NNS_SndArcPlayerStartSeq( SOUNDMAN_GetHierarchyPlayerSndHandle(), dummyNo );
}

void	PMDSND_StopExtraMusic( void )
{
	NNS_SndPlayerStopSeq(SOUNDMAN_GetHierarchyPlayerSndHandle(), 0);
	SOUNDMAN_UnloadHierarchyPlayer();
}

void  PMDSND_ReleaseExtraMusic( void )
{
	// �_�~�[���y��FileID�����ɖ߂�
	{
		u32 bnkFileID = NNS_SndArcGetBankInfo(BANK_MUS_WB_SHINKA)->fileId;
		u32 seqFileID = NNS_SndArcGetSeqInfo(SEQ_BGM_SHINKA)->fileId;

		NNS_SndArcSetFileAddress(bnkFileID, NULL);
		NNS_SndArcSetFileAddress(seqFileID, NULL);
	}
}


