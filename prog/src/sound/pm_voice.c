//============================================================================================
/**
 * @file	pm_voice.c
 * @brief	�|�P���������g�`�Đ���{�V�X�e��	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

#include "sound/pm_sndsys.h"

#include "sound/pm_voice.h"
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
#define PMVOICE_WAVESIZE_MAX	(23600)

#define VOICE_RATE_DEFAULT		(13379)
#define VOICE_SPEED_DEFAULT		(25825)	//seq�f�[�^(pm_voice.sseq)��((u16)0x1c)�𔲐�

#define VOICE_VOLUME			(120)	//seq�ݒ�(sound_data.sarc)��(PV_VOL)���Q��
//------------------------------------------------------------------
/**
 * @brief	�g�`�v���[���[��`
 */
//------------------------------------------------------------------
typedef struct {
	BOOL				active;
	void*				waveData;
	u32					waveSize;
	int					waveRate;
	void*				waveDataBegin;

	NNSSndWaveOutHandle waveHandle;	
	NNSSndWaveOutHandle waveHandleSub;	
	s8					volume;
	s8					volumeSubDiff;
	u8					channel:4;
	u8					channelSub:4;
	int					speed;
	int					speedSubDiff;
	u8					pan;
	BOOL				subWaveUse;
	BOOL				heapReserveFlag;
}PMVOICE_PLAYER;

static  u8 staticWaveData[PMVOICE_WAVESIZE_MAX];
//------------------------------------------------------------------
/**
 * @brief	�V�X�e����`
 */
//------------------------------------------------------------------
#define VOICE_PLAYER_NUM (3)

typedef struct {
	HEAPID			          heapID;
	PMVOICE_PLAYER	      voicePlayer[VOICE_PLAYER_NUM];
	u16				            voicePlayerRef;
	u16				            voicePlayerEnableNum;
	PMVOICE_CB_GET_WVIDX*	CallBackGetWaveIdx;
	PMVOICE_CB_GET_WVDAT*	CallBackCustomWave;
  u8                    masterVolume;
}PMVOICE_SYS;

//------------------------------------------------------------------
/**
 * @brief	�`�����l���g�p�f�[�^
 *				���m�ۂ���`�����l���̎g�p��
 */
//------------------------------------------------------------------
static const u16 useChannnelPriority[VOICE_PLAYER_NUM][2] = 
	{ {15, 14}, {13, 12}, {11, 10} };

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�|�P���������g�`�v���[���[
 *
 *
 *
 *
 *
 */
//============================================================================================
static PMVOICE_SYS pmvSys;

static u16	getPlayerIdx( void );
static void initPlayerWork( PMVOICE_PLAYER* voicePlayer );
static BOOL loadWave( PMVOICE_PLAYER* voicePlayer, int waveNo );
static BOOL playWave( PMVOICE_PLAYER* voicePlayer );
static void stopWave( PMVOICE_PLAYER* voicePlayer );

static void reverseBuf( u8* buf, u32 size );
//============================================================================================
/**
 *
 *
 * @brief	�V�X�e���imain.c������Ăяo�����j
 *
 *
 */
//============================================================================================

//============================================================================================
/**
 * @brief	������
 */
//============================================================================================
void	PMVOICE_Init
		( HEAPID heapID,							// �g�pheapID 
		  PMVOICE_CB_GET_WVIDX* CallBackGetWaveIdx,	// waveIdx�擾�p�R�[���o�b�N
		  PMVOICE_CB_GET_WVDAT* CallBackCustomWave 	// �g�`�J�X�^�}�C�Y�R�[���o�b�N
		)
{
	PMVOICE_PLAYER* voicePlayer;
	int i;

	pmvSys.heapID = heapID;
  pmvSys.masterVolume = 127;

	for( i=0; i<VOICE_PLAYER_NUM; i++ ){
		voicePlayer = &pmvSys.voicePlayer[i];

		voicePlayer->active = FALSE;
		voicePlayer->channel = useChannnelPriority[i][0];
		voicePlayer->channelSub = useChannnelPriority[i][1];
		initPlayerWork(voicePlayer);
		// player0 �����͐ÓI�m�ۂ���
		if(i == 0){
			voicePlayer->waveData = staticWaveData;
			voicePlayer->waveDataBegin = voicePlayer->waveData;
		}
	}
	pmvSys.voicePlayerEnableNum = 1;
	pmvSys.voicePlayerRef = 0;

	pmvSys.CallBackGetWaveIdx = CallBackGetWaveIdx;
	pmvSys.CallBackCustomWave = CallBackCustomWave;
}

//============================================================================================
/**
 * @brief	���Z�b�g
 */
//============================================================================================
void	PMVOICE_Reset( void )
{
	PMVOICE_PLAYER* voicePlayer;
	int i;

	for( i=0; i<VOICE_PLAYER_NUM; i++ ){
		voicePlayer = &pmvSys.voicePlayer[i];
		if(voicePlayer->active == TRUE){ stopWave(voicePlayer); }
	}
}

//============================================================================================
/**
 * @brief	�t���[�����[�N
 */
//============================================================================================
void	PMVOICE_Main()
{
	PMVOICE_PLAYER* voicePlayer;
	BOOL result, resultSub;
	int i;

	for( i=0; i<pmvSys.voicePlayerEnableNum; i++ ){
		voicePlayer = &pmvSys.voicePlayer[i];

		if(voicePlayer->active == TRUE){
			result = NNS_SndWaveOutIsPlaying(voicePlayer->waveHandle);

			if( voicePlayer->subWaveUse == TRUE ){
				resultSub = NNS_SndWaveOutIsPlaying(voicePlayer->waveHandleSub);
			} else {
				resultSub = FALSE;
			}

			if( (result == FALSE)&&(resultSub == FALSE) ){
				stopWave(voicePlayer); 
			}
		}
	}
}

//============================================================================================
/**
 * @brief	�I���������j��
 */
//============================================================================================
void	PMVOICE_Exit( void )
{
	PMVOICE_PLAYER* voicePlayer;
	int i;

	for( i=0; i<VOICE_PLAYER_NUM; i++ ){ 
		voicePlayer = &pmvSys.voicePlayer[i];

		if(voicePlayer->active == TRUE){ stopWave(voicePlayer); }
		// ���J���o�b�t�@������ΊJ��
		if((i != 0)&&(voicePlayer->waveData != NULL)){
			GFL_HEAP_FreeMemory(voicePlayer->waveData);
		}
		voicePlayer->active = FALSE;
		initPlayerWork(voicePlayer);
	}
}

//============================================================================================
/**
 * @brief	�v���[���[�ғ���ԃ`�F�b�N
 */
//============================================================================================
BOOL	PMVOICE_CheckBusy( void )
{
	PMVOICE_PLAYER* voicePlayer;
	int i;

	for( i=0; i<VOICE_PLAYER_NUM; i++ ){
		voicePlayer = &pmvSys.voicePlayer[i];
		if(voicePlayer->active == TRUE){ return TRUE; }
	}
	return FALSE;
}

//============================================================================================
/**
 * @brief	�v���[���[�pwave�o�b�t�@���O�m��
 */
//============================================================================================
void	PMVOICE_PlayerHeapReserve( int num, HEAPID heapID )
{
	PMVOICE_PLAYER* voicePlayer;
	int i;

	PMVOICE_Reset();

	if( num > (VOICE_PLAYER_NUM-1) ){ num = VOICE_PLAYER_NUM-1; }

	for( i=1; i<num+1; i++ ){ 
		voicePlayer = &pmvSys.voicePlayer[i];
		if(voicePlayer->waveData == NULL){
			voicePlayer->waveData = GFL_HEAP_AllocClearMemory(heapID, PMVOICE_WAVESIZE_MAX);
			voicePlayer->waveDataBegin = voicePlayer->waveData;
			voicePlayer->heapReserveFlag = TRUE;
		}
	}
	pmvSys.voicePlayerEnableNum = num + 1;
	pmvSys.voicePlayerRef = 0;
}

//============================================================================================
/**
 * @brief	�v���[���[�pwave�o�b�t�@�J��
 */
//============================================================================================
void	PMVOICE_PlayerHeapRelease( void )
{
	PMVOICE_PLAYER* voicePlayer;
	int i;

	PMVOICE_Reset();

	for( i=0; i<pmvSys.voicePlayerEnableNum; i++ ){ 
		voicePlayer = &pmvSys.voicePlayer[i];
		if((voicePlayer->waveData != NULL)&&(voicePlayer->heapReserveFlag == TRUE)){
			GFL_HEAP_FreeMemory(voicePlayer->waveData);
			initPlayerWork(voicePlayer);
		}
	}
	pmvSys.voicePlayerEnableNum = 1;
	pmvSys.voicePlayerRef = 0;
}

//============================================================================================
/**
 * @brief	�}�X�^�[�{�����[���ύX
 */
//============================================================================================
void	PMVOICE_SetMasterVolume( u8 volume )
{
  pmvSys.masterVolume = volume;
}

void	PMVOICE_ResetMasterVolume( void )
{
  pmvSys.masterVolume = 127;
}

static u8	getVolume( u8 volumeOrg )
{
  return volumeOrg * pmvSys.masterVolume / 127;
}

//============================================================================================
/**
 *
 *
 * @brief	�v���[���[�֐�
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�v���[���[�h�c�w�擾
 */
//------------------------------------------------------------------
static u16 getPlayerIdx( void )
{
	PMVOICE_PLAYER* voicePlayer;
	u16 voicePlayerIdx = pmvSys.voicePlayerRef;

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	if(voicePlayer->active == TRUE){ stopWave(voicePlayer); }

	if( pmvSys.voicePlayerRef < pmvSys.voicePlayerEnableNum-1){ 
		pmvSys.voicePlayerRef++;
	} else {
		pmvSys.voicePlayerRef = 0;
	}
	return voicePlayerIdx;
}

//------------------------------------------------------------------
/**
 * @brief	�v���[���[���[�N������
 */
//------------------------------------------------------------------
static void resetPlayerWork( PMVOICE_PLAYER* voicePlayer )
{
	voicePlayer->waveSize = 0;
	voicePlayer->waveRate = VOICE_RATE_DEFAULT;
	voicePlayer->volume = 0;
	voicePlayer->volumeSubDiff = 0;
	voicePlayer->speed = VOICE_SPEED_DEFAULT;
	voicePlayer->speedSubDiff = 0;
	voicePlayer->pan = 64;
	voicePlayer->subWaveUse = FALSE;
	voicePlayer->waveDataBegin = voicePlayer->waveData;
}

static void initPlayerWork( PMVOICE_PLAYER* voicePlayer )
{
	voicePlayer->waveData = NULL;
	voicePlayer->heapReserveFlag = FALSE;
	resetPlayerWork(voicePlayer);
}

//------------------------------------------------------------------
/**
 * @brief	�g�`���[�h
 */
//------------------------------------------------------------------
static BOOL loadWave( PMVOICE_PLAYER* voicePlayer, int waveNo )
{
	const NNSSndArcWaveArcInfo* waveArcInfo;
	u32 waveFileSize;
	BOOL result;

	// �g�`�A�[�J�C�u���\���̃|�C���^���擾
	waveArcInfo = NNS_SndArcGetWaveArcInfo(waveNo);
	if( waveArcInfo == NULL ){ return FALSE; }	// �g�`�f�[�^�擾���s

	// �t�@�C���T�C�Y�擾
	waveFileSize = NNS_SndArcGetFileSize(waveArcInfo->fileId);
	if( waveFileSize == 0 ){ return FALSE; }	// �t�@�C���h�c����

	// �g�`�f�[�^�ǂݍ���
	result = NNS_SndArcReadFile(waveArcInfo->fileId, voicePlayer->waveData, waveFileSize, 0);
	if( result == -1 ){ return FALSE; }				// �ǂݍ��ݎ��s
	if( result != waveFileSize ){ return FALSE; }	// �ǂݍ��݃T�C�Y�s����

	{
		SNDWaveArc*		waveArc = voicePlayer->waveData;
		SNDWaveData*	waveData = (SNDWaveData*)((u32)waveArc + waveArc->waveOffset[0]);
		u32 len, begin, end, waveCount;
		u32 waveDataIdx = 0;

		waveCount = SND_GetWaveDataCount( waveArc );
    begin = waveArc->waveOffset[0];
    end = waveArc->fileHeader.fileSize;
    len = end - begin;
		len -= sizeof(SNDWaveParam); 

		voicePlayer->waveDataBegin = (void*)(waveData->samples);
		voicePlayer->waveSize = len;

		voicePlayer->waveRate = waveData->param.rate;
		voicePlayer->speed = 
			(u64)SND_TIMER_CLOCK * 0x8000 / waveData->param.rate / waveData->param.timer;
	}

	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�g�`�Đ�
 */
//------------------------------------------------------------------
static BOOL playWave( PMVOICE_PLAYER* voicePlayer )
{
	BOOL result;

	if(voicePlayer->waveData == NULL){ return FALSE; } 
	if(voicePlayer->waveSize == 0){ return FALSE; } 

	// �g�p�`�����l���m��
	voicePlayer->waveHandle = NNS_SndWaveOutAllocChannel(voicePlayer->channel);		
	if( voicePlayer->waveHandle == NNS_SND_WAVEOUT_INVALID_HANDLE ){ return FALSE; } 

	//OS_Printf("soundData play rate %d, speed %d\n", voicePlayer->waveRate, voicePlayer->speed);

	result = NNS_SndWaveOutStart(	voicePlayer->waveHandle,	// �g�`�Đ��n���h��
									NNS_SND_WAVE_FORMAT_PCM8,	        // �g�`�f�[�^�t�H�[�}�b�g
									voicePlayer->waveDataBegin,		    // �g�`�f�[�^�A�h���X
									FALSE, 0,									        // ���[�v�t���O,�J�n�ʒu
									voicePlayer->waveSize,		        // �g�`�f�[�^�T���v����
									voicePlayer->waveRate,		        // �g�`�f�[�^�T���v�����O���[�g
									getVolume(voicePlayer->volume),		// �Đ�volume
									voicePlayer->speed,				        // �Đ�speed
									voicePlayer->pan );				        // �Đ�pan
	if( result == FALSE ){ return FALSE; }

	if( voicePlayer->subWaveUse == TRUE ){
		int	volumeSub;
		int	speedSub;
		// �g�p�`�����l���m��
		voicePlayer->waveHandleSub = NNS_SndWaveOutAllocChannel(voicePlayer->channelSub);		
		if( voicePlayer->waveHandleSub == NNS_SND_WAVEOUT_INVALID_HANDLE ){ return FALSE; } 

		volumeSub = voicePlayer->volume + voicePlayer->volumeSubDiff;
		if( volumeSub < 0 ){ volumeSub = 0; }
		if( volumeSub > 127 ){ volumeSub = 127; }
		speedSub = voicePlayer->speed + voicePlayer->speedSubDiff;

		result = NNS_SndWaveOutStart(	voicePlayer->waveHandleSub,	// �g�`�Đ��n���h��
										NNS_SND_WAVE_FORMAT_PCM8,	      // �g�`�f�[�^�t�H�[�}�b�g
										voicePlayer->waveDataBegin,	  	// �g�`�f�[�^�A�h���X
										FALSE, 0,									      // ���[�v�t���O,�J�n�ʒu
										voicePlayer->waveSize,		      // �g�`�f�[�^�T���v����
										voicePlayer->waveRate,		      // �g�`�f�[�^�T���v�����O���[�g
										getVolume(volumeSub),						// �Đ�volume
										speedSub,									      // �Đ�speed
										voicePlayer->pan );				      // �Đ�pan
		if( result == FALSE ){ return FALSE; }
	}
	voicePlayer->active = TRUE;

	return TRUE;
}
	
//------------------------------------------------------------------
/**
 * @brief	�g�`��~
 */
//------------------------------------------------------------------
static void stopWave( PMVOICE_PLAYER* voicePlayer )
{
	NNS_SndWaveOutStop(voicePlayer->waveHandle);
	NNS_SndWaveOutWaitForChannelStop(voicePlayer->waveHandle);
	// �g�p�`�����l���J��
	NNS_SndWaveOutFreeChannel(voicePlayer->waveHandle);

	if( voicePlayer->subWaveUse == TRUE ){
		NNS_SndWaveOutStop(voicePlayer->waveHandleSub);
		NNS_SndWaveOutWaitForChannelStop(voicePlayer->waveHandleSub);
		// �g�p�`�����l���J��
		NNS_SndWaveOutFreeChannel(voicePlayer->waveHandleSub);
	}
#if 0
	// �g�`�f�[�^�o�b�t�@�J��
	if((voicePlayer->waveData != NULL)&&(voicePlayer->heapReserveFlag == FALSE)){
		GFL_HEAP_FreeMemory(voicePlayer->waveData); 
	}
#endif
	voicePlayer->active = FALSE;
	resetPlayerWork(voicePlayer);
}





//============================================================================================
/**
 *
 *
 * @brief	�����T�E���h�֐��i�eapp����Ăяo�����j
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�����Đ��֐�
 */
//------------------------------------------------------------------
u32		PMVOICE_Play
		(	u32			pokeNo,			// �|�P�����i���o�[
			u32			pokeFormNo,		// �|�P�����t�H�[���i���o�[
			u8			pan,			// ���(L:0 - 64 - 127:R)
			BOOL		chorus,			// �R�[���X�g�p�t���O
			int			chorusVolOfs,	// �R�[���X�{�����[����
			int			chorusSpOfs,	// �Đ����x��
			BOOL		reverse,		// �t�Đ��t���O
			u32			userParam		// ���[�U�[�p�����[�^�[	
		)		
{
	u16		voicePlayerIdx;

  // ���[�h�X���b�h���쒆�͏I���҂�(pm_sndsys�Q��)
	while( PMSND_IsLoading() == TRUE ) { OS_Sleep(2); }

	voicePlayerIdx = PMVOICE_LoadOnly
		(pokeNo, pokeFormNo, pan, chorus, chorusVolOfs, chorusSpOfs, reverse, userParam);
	PMVOICE_PlayOnly(voicePlayerIdx);

	return voicePlayerIdx;
}

u32		PMVOICE_LoadOnly
		(	u32			pokeNo,			// �|�P�����i���o�[
			u32			pokeFormNo,		// �|�P�����t�H�[���i���o�[
			u8			pan,			// ���(L:0 - 64 - 127:R)
			BOOL		chorus,			// �R�[���X�g�p�t���O
			int			chorusVolOfs,	// �R�[���X�{�����[����
			int			chorusSpOfs,	// �Đ����x��
			BOOL		reverse,		// �t�Đ��t���O
			u32			userParam		// ���[�U�[�p�����[�^�[	
		)		
{
	PMVOICE_PLAYER* voicePlayer;
	u16		voicePlayerIdx;
	u32		waveIdx;
	BOOL	waveLoadFlag;

	GF_ASSERT(pmvSys.CallBackGetWaveIdx);
	GF_ASSERT(pmvSys.CallBackCustomWave);

	// �g�`IDX�擾
	pmvSys.CallBackGetWaveIdx(pokeNo, pokeFormNo, &waveIdx);

	// �Đ��v���[���[�擾
	voicePlayerIdx = getPlayerIdx();
	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];

	if(voicePlayer->active == TRUE){ stopWave(voicePlayer); };
#if 0
	// �g�`�f�[�^�o�b�t�@�m��
	if( voicePlayer->heapReserveFlag == FALSE ){
		// ��ɍő�l�Ŋm�ہi��ʂɂ���ĈقȂ�Ɛ��݃o�O�𐶂މ\�������邽�߁j
		voicePlayer->waveData = GFL_HEAP_AllocClearMemory(pmvSys.heapID, PMVOICE_WAVESIZE_MAX);
	}
#endif	
	// �g�`�f�[�^�J�X�^�}�C�Y(TRUE: �R�[���o�b�N���Ő������ꂽ)
	waveLoadFlag = pmvSys.CallBackCustomWave(	pokeNo, pokeFormNo, userParam,
													&voicePlayer->waveDataBegin, 
													&voicePlayer->waveSize, 
													&voicePlayer->waveRate, 
													&voicePlayer->speed,
													&voicePlayer->volume);
	// �R�[���o�b�N���Ő�������Ȃ������ꍇ�A�g�`�f�[�^�擾
	if( waveLoadFlag == FALSE ){ loadWave(voicePlayer, waveIdx); }

	// �t�Đ��p�f�[�^���H
	if( reverse ){reverseBuf(voicePlayer->waveDataBegin, voicePlayer->waveSize); }

	// �e��ݒ�
	//voicePlayer->volume = VOICE_VOLUME;
	voicePlayer->pan = pan;
	voicePlayer->subWaveUse = chorus;
	voicePlayer->volumeSubDiff = chorusVolOfs;
	voicePlayer->speedSubDiff = chorusSpOfs;

	return voicePlayerIdx;
}

BOOL		PMVOICE_PlayOnly( u32 voicePlayerIdx)
{
	PMVOICE_PLAYER* voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];

	// �g�`�Đ�
	if( playWave(voicePlayer) == FALSE ){
		stopWave(voicePlayer);
		return FALSE;
	};
	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�����X�e�[�^�X�ύX�֐�
 */
//------------------------------------------------------------------
void	PMVOICE_SetPan( u32 voicePlayerIdx, u8 pan )
{
	PMVOICE_PLAYER* voicePlayer;

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	if(voicePlayer->active == FALSE){ return; }

	voicePlayer->pan = pan;
	NNS_SndWaveOutSetPan(voicePlayer->waveHandle, pan);
	if( voicePlayer->subWaveUse == TRUE ){
		NNS_SndWaveOutSetPan(voicePlayer->waveHandleSub, pan);
	}
}

void	PMVOICE_SetVolume( u32 voicePlayerIdx, s8 volume )
{
	PMVOICE_PLAYER* voicePlayer;
	int volumeTmp;

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	if(voicePlayer->active == FALSE){ return; }

	volumeTmp = voicePlayer->volume + volume;;
	if(volumeTmp < 0){ volumeTmp = 0; }
	if(volumeTmp > 127){ volumeTmp = 127; }
	voicePlayer->volume = volumeTmp;
	NNS_SndWaveOutSetVolume(voicePlayer->waveHandle, getVolume(voicePlayer->volume));

	if( voicePlayer->subWaveUse == TRUE ){
		volumeTmp = voicePlayer->volume + voicePlayer->volumeSubDiff;
		if(volumeTmp < 0){ volumeTmp = 0; }
		if(volumeTmp > 127){ volumeTmp = 127; }
		NNS_SndWaveOutSetVolume(voicePlayer->waveHandle, getVolume(volumeTmp));
	}
}

void	PMVOICE_SetSpeed( u32 voicePlayerIdx, int speed )
{
	PMVOICE_PLAYER* voicePlayer;

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	if(voicePlayer->active == FALSE){ return; }

	voicePlayer->speed += speed;
	NNS_SndWaveOutSetSpeed(voicePlayer->waveHandle, voicePlayer->speed);

	if( voicePlayer->subWaveUse == TRUE ){
		int speedSub = voicePlayer->speed + voicePlayer->speedSubDiff;
		NNS_SndWaveOutSetSpeed(voicePlayer->waveHandleSub, speedSub);
	}
}

u8	PMVOICE_GetPan( u32 voicePlayerIdx)
{	
	PMVOICE_PLAYER* voicePlayer;

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	if(voicePlayer->active == FALSE){ return 64; }

	return voicePlayer->pan;
}

s8	PMVOICE_GetVolume( u32 voicePlayerIdx )
{	
	PMVOICE_PLAYER* voicePlayer;

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	if(voicePlayer->active == FALSE){ return 0; }

	return voicePlayer->volume;
}

int	PMVOICE_GetSpeed( u32 voicePlayerIdx )
{	
	PMVOICE_PLAYER* voicePlayer;

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	if(voicePlayer->active == FALSE){ return VOICE_SPEED_DEFAULT; }

	return voicePlayer->speed;
}

void*	PMVOICE_GetWave( u32 voicePlayerIdx )
{	
	PMVOICE_PLAYER* voicePlayer;

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	if(voicePlayer->active == FALSE){ return NULL; }

	return voicePlayer->waveDataBegin;
}

u32	PMVOICE_GetWaveSize( u32 voicePlayerIdx )
{	
	PMVOICE_PLAYER* voicePlayer;

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	if(voicePlayer->active == FALSE){ return 0; }

	return voicePlayer->waveSize;
}

int	PMVOICE_GetWaveRate( u32 voicePlayerIdx )
{	
	PMVOICE_PLAYER* voicePlayer;

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	if(voicePlayer->active == FALSE){ return VOICE_RATE_DEFAULT; }

	return voicePlayer->waveRate;
}


//------------------------------------------------------------------
/**
 * @brief	������~�֐�
 */
//------------------------------------------------------------------
void	PMVOICE_Stop( u32 voicePlayerIdx )
{
	PMVOICE_PLAYER* voicePlayer;

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	if(voicePlayer->active == TRUE){ stopWave(voicePlayer); }
}

//------------------------------------------------------------------
/**
 * @brief	�����I�����o
 */
//------------------------------------------------------------------
BOOL	PMVOICE_CheckPlay( u32 voicePlayerIdx )
{
	PMVOICE_PLAYER* voicePlayer;

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	return voicePlayer->active;
}





//============================================================================================
/**
 *
 *
 * @brief	
 *
 *
 */
//============================================================================================
static void reverseBuf( u8* buf, u32 size )
{
	u8	data;
	int i;

	//�t�Đ��p�Ƀf�[�^���t���ɓ���ւ���( [0]-[MAX]�A[1]-[MAX-1]... )
	for( i=0; i<(size/2); i++ ){
		data = buf[i];
		buf[i] = buf[size-1-i];
		buf[size-1-i] = data;
	}
}


//============================================================================================
/**
 *
 *
 * @brief	�g�`�Đ��f�o�b�O�p
 *
 *
 */
u16 PMV_DBG_CustomVoicePlay(	void*, u32, int, int, s8 );
//============================================================================================
u16 PMV_DBG_CustomVoicePlay(	void*	wave,				// [in]�g�`�f�[�^
															u32		size,				// [in]�g�`�T�C�Y(MAX 26000)
															int		rate,				// [in]�g�`�Đ����[�g
															int		speed,			// [in]�g�`�Đ��X�s�[�h
															s8		volume)			// [in]�Đ��{�����[��
{
	PMVOICE_PLAYER* voicePlayer;
	u16		voicePlayerIdx;

	// �Đ��v���[���[�擾
	voicePlayerIdx = getPlayerIdx();
	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];

	if(voicePlayer->active == TRUE){ stopWave(voicePlayer); };

	voicePlayer->waveDataBegin = wave,		// �g�`�f�[�^�A�h���X
	voicePlayer->waveSize = size;
	voicePlayer->waveRate = rate; 
	voicePlayer->speed = speed;
	voicePlayer->volume = volume;

	// �e��ݒ�
	voicePlayer->pan = 64;
	voicePlayer->subWaveUse = FALSE;
	voicePlayer->volumeSubDiff = 0;
	voicePlayer->speedSubDiff = 0;

	playWave(voicePlayer);

	return voicePlayerIdx;
}


