//============================================================================================
/**
 * @file	pm_sndsys.c
 * @brief	
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
//------------------------------------------------------------------
/**
 * @brief	�����v���[���[��`
 */
//------------------------------------------------------------------
typedef struct {
	BOOL				active;
	NNSSndWaveOutHandle waveHandle;	
	u8					volume;
	u8					channel;
	void*				waveData;
	u32					waveSize;
	BOOL				waveReferred;
}PMVOICE_PLAYER;

#define PMVOICE_ERROR	(0xffffffff)
//------------------------------------------------------------------
/**
 * @brief	�V�X�e����`
 */
//------------------------------------------------------------------
#define VOICE_PLAYER_NUM (8)

typedef struct {
	HEAPID			heapID;
	PMVOICE_PLAYER	voicePlayer[VOICE_PLAYER_NUM];
	u16				voicePlayerRef;
	u16				voicePlayerEnableNum;
	BOOL			voicePlayerHeapReserveFlag;
}PMVOICE_SYS;

//------------------------------------------------------------------
/**
 * @brief	�`�����l���g�p�f�[�^
 *				���m�ۂ���`�����l���̎g�p��
 */
//------------------------------------------------------------------
static const u16 useChannnelPriority[16] = { 15, 14, 13, 12, 11, 10, 9, 8 };

//============================================================================================
/**
 *
 *
 *
 *
 *
 * @brief	�|�P���������Đ��i�g�`�Đ��j
 *
 *
 *
 *
 *
 */
//============================================================================================
static PMVOICE_SYS pmvSys;

static BOOL playWave( PMVOICE_PLAYER* voicePlayer );
static void stopWave( PMVOICE_PLAYER* voicePlayer );
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
void	PMVOICE_Init( HEAPID heapID )
{
	PMVOICE_PLAYER* voicePlayer;
	int i;

	pmvSys.heapID = heapID;

	for( i=0; i<VOICE_PLAYER_NUM; i++ ){
		voicePlayer = &pmvSys.voicePlayer[i];
		voicePlayer->active = FALSE;
		voicePlayer->volume = 0;
		voicePlayer->channel = useChannnelPriority[i];
		voicePlayer->waveData = NULL;
		voicePlayer->waveSize = 0;
		voicePlayer->waveReferred = FALSE;
	}
	pmvSys.voicePlayerRef = 0;
	pmvSys.voicePlayerEnableNum = VOICE_PLAYER_NUM;
	pmvSys.voicePlayerHeapReserveFlag = FALSE;
}

//============================================================================================
/**
 * @brief	�t���[�����[�N
 */
//============================================================================================
void	PMVOICE_Main()
{
	PMVOICE_PLAYER* voicePlayer;
	int i;

	for( i=0; i<pmvSys.voicePlayerEnableNum; i++ ){
		voicePlayer = &pmvSys.voicePlayer[i];

		if(voicePlayer->active == TRUE){
			if(NNS_SndWaveOutIsPlaying(voicePlayer->waveHandle) == FALSE){ 
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
		if(voicePlayer->waveData != NULL){ 
			GFL_HEAP_FreeMemory(voicePlayer->waveData); 
			voicePlayer->waveData = NULL;
		}
	}
}

//============================================================================================
/**
 * @brief	�v���[���[�pwave�o�b�t�@���O�m��
 */
//============================================================================================
#define PMVOICE_WAVESIZE_MAX	(26000)
void	PMVOICE_PlayerHeapReserve( int num )
{
	PMVOICE_PLAYER* voicePlayer;
	int i;

	PMVOICE_Stop( 0xffff );

	for( i=0; i<num; i++ ){ 
		voicePlayer = &pmvSys.voicePlayer[i];
		voicePlayer->waveData = GFL_HEAP_AllocClearMemory(pmvSys.heapID, PMVOICE_WAVESIZE_MAX);
	}
	pmvSys.voicePlayerEnableNum = num;
	pmvSys.voicePlayerHeapReserveFlag = TRUE;
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

	PMVOICE_Stop( 0xffff );

	for( i=0; i<pmvSys.voicePlayerEnableNum; i++ ){ 
		voicePlayer = &pmvSys.voicePlayer[i];
		if(voicePlayer->waveData != NULL){ GFL_HEAP_FreeMemory(voicePlayer->waveData); }
	}
	pmvSys.voicePlayerEnableNum = VOICE_PLAYER_NUM;
	pmvSys.voicePlayerHeapReserveFlag = FALSE;
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
static u16 getEmptyPlayer( void )
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
 * @brief	�g�`���[�h
 */
//------------------------------------------------------------------
static BOOL loadWave( PMVOICE_PLAYER* voicePlayer, int waveNo, int reverseFlag )
{
	const NNSSndSeqParam* seqInfo;
	const NNSSndArcWaveArcInfo* waveArcInfo;
	u32 waveFileSize;
	BOOL result;

	if(( waveNo < PMVOICE_START)&&( waveNo > PMVOICE_END) ){ return FALSE; }	//�w��͈͊O

	// �g�`�A�[�J�C�u���\���̃|�C���^���擾
	waveArcInfo = NNS_SndArcGetWaveArcInfo(waveNo);
	if( waveArcInfo == NULL ){ return FALSE; }	// �g�`�f�[�^�擾���s

	// �t�@�C���T�C�Y�擾
	waveFileSize = NNS_SndArcGetFileSize(waveArcInfo->fileId);
	if( waveFileSize == 0 ){ return FALSE; }	// �t�@�C���h�c����

	// �g�`�f�[�^�o�b�t�@�m��
	if( pmvSys.voicePlayerHeapReserveFlag == FALSE ){
		voicePlayer->waveData = GFL_HEAP_AllocClearMemory(pmvSys.heapID, waveFileSize);
	} else {
		if(voicePlayer->waveData == NULL ){ return FALSE; }	// �o�b�t�@�|�C���^�s����
	}

	// �g�`�f�[�^�ǂݍ���
	result = NNS_SndArcReadFile(waveArcInfo->fileId, voicePlayer->waveData, waveFileSize, 0);
	if( result == -1 ){ return FALSE; }				// �ǂݍ��ݎ��s
	if( result != waveFileSize ){ return FALSE; }	// �ǂݍ��݃T�C�Y�s����

	if( reverseFlag ){
		//�t�Đ��p�Ƀf�[�^���t���ɓ���ւ���( [0]-[MAX]�A[1]-[MAX-1]... )
		u8* buf = voicePlayer->waveData;
		u8	data;
		int i;

		for( i=0; i<(waveFileSize/2); i++ ){
			data = buf[i];
			buf[i] = buf[waveFileSize-1-i];
			buf[waveFileSize-1-i] = data;
		}
	}
	voicePlayer->waveSize = waveFileSize;

	// �V�[�P���X���\���̃|�C���^���擾
	seqInfo = NNS_SndArcGetSeqParam(SEQ_PV);
	voicePlayer->volume = seqInfo->volume;

	// �O���Q�ƃt���O���Z�b�g
	voicePlayer->waveReferred = FALSE;

	return TRUE;
}

//------------------------------------------------------------------
/**
 * @brief	�g�`�Đ�
 */
//------------------------------------------------------------------
#define VOICE_RATE_DEFAULT	(13379)
#define VOICE_SPEED_DEFAULT (25825)	//seq�f�[�^(pm_voice.sseq)��((u16)0x1c)�𔲐�

static BOOL playWave( PMVOICE_PLAYER* voicePlayer )
{
	BOOL result;

	if(voicePlayer->waveData == NULL){ return FALSE; } 
	if(voicePlayer->waveSize == 0){ return FALSE; } 

	// �g�p�`�����l���m��
	voicePlayer->waveHandle = NNS_SndWaveOutAllocChannel(voicePlayer->channel);		
	if( voicePlayer->waveHandle == NNS_SND_WAVEOUT_INVALID_HANDLE ){ return FALSE; } 

	result = NNS_SndWaveOutStart(	voicePlayer->waveHandle,	// �g�`�Đ��n���h��
									NNS_SND_WAVE_FORMAT_PCM8,	// �g�`�f�[�^�t�H�[�}�b�g
									voicePlayer->waveData,		// �g�`�f�[�^�A�h���X
									FALSE, 0,					// ���[�v�t���O,�J�n�ʒu
									voicePlayer->waveSize,		// �g�`�f�[�^�T���v����
									VOICE_RATE_DEFAULT,			// �g�`�f�[�^�T���v�����O���[�g
									voicePlayer->volume,		// �Đ�volume
									VOICE_SPEED_DEFAULT,		// �Đ�speed
									64 );						// �Đ�pan
	if( result == TRUE ){ voicePlayer->active = TRUE; }

	return result;
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
	// �g�`�f�[�^�o�b�t�@�J��
	if((voicePlayer->waveData != NULL)&&(voicePlayer->waveReferred == FALSE)){
		if( pmvSys.voicePlayerHeapReserveFlag == FALSE ){
			GFL_HEAP_FreeMemory(voicePlayer->waveData); 
			voicePlayer->waveData = NULL;
		}
	}
	voicePlayer->active = FALSE;
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
u32		PMVOICE_Play( u32 pokeNum, u16 option )
{
	PMVOICE_PLAYER* voicePlayer;
	u32 voicePlayerIdxBit = 0;
	u16 voicePlayerIdx;

	voicePlayerIdx = getEmptyPlayer();
	if(voicePlayerIdx == PMVOICE_ERROR){ return 0; }

	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];
	loadWave(voicePlayer, (pokeNum-1) + PMVOICE_POKE001, (option & PMVOICE_MODE_REVERSE));
	playWave(voicePlayer);
	voicePlayerIdxBit |= (0x0001<<voicePlayerIdx);
	
	if(option & PMVOICE_MODE_CHORUS){
		PMVOICE_PLAYER* voicePlayerSub;
		u16 voicePlayerIdxSub = getEmptyPlayer();
		if(voicePlayerIdxSub == PMVOICE_ERROR){ return voicePlayerIdxBit; }

		voicePlayerSub = &pmvSys.voicePlayer[voicePlayerIdxSub];
		voicePlayerSub->waveData = voicePlayer->waveData;
		voicePlayerSub->waveSize = voicePlayer->waveSize;
		voicePlayerSub->waveReferred = FALSE;
		voicePlayerSub->volume = voicePlayer->volume -10;

		// �O���Q�ƃt���O�Z�b�g
		voicePlayer->waveReferred = TRUE;

		playWave(voicePlayerSub);
		voicePlayerIdxBit |= (0x0001<<voicePlayerIdxSub);
	}

	OS_Printf("voicePlayerIdxBit = %x\n",voicePlayerIdxBit);
	return voicePlayerIdxBit;
}

//------------------------------------------------------------------
/**
 * @brief	������~�֐�
 */
//------------------------------------------------------------------
void	PMVOICE_Stop( u32 voicePlayerIdxBit )
{
	PMVOICE_PLAYER* voicePlayer;
	BOOL req;
	int i;

	for( i=0; i<pmvSys.voicePlayerEnableNum; i++ ){ 
		voicePlayer = &pmvSys.voicePlayer[i];
		req = (voicePlayerIdxBit >> i) & 0x0001;

		if((voicePlayer->active == TRUE)&&(req)){ stopWave(voicePlayer); }
	}
}

//------------------------------------------------------------------
/**
 * @brief	�����I�����o
 */
//------------------------------------------------------------------
BOOL	PMVOICE_CheckPlay( u32 voicePlayerIdxBit )
{
	PMVOICE_PLAYER* voicePlayer;
	BOOL req;
	int i;

	if(voicePlayerIdxBit){ return FALSE; } 

	for( i=0; i<pmvSys.voicePlayerEnableNum; i++ ){ 
		voicePlayer = &pmvSys.voicePlayer[i];
		req = (voicePlayerIdxBit >> i) & 0x0001;

		if(voicePlayer->active == TRUE){ return TRUE; }	// ���쒆
	}
	return FALSE;
}





#if 0
//============================================================================================
/**
 *
 *
 * @brief	�V�X�e���v���[���[���g�p���������T�E���h�֐��i�eapp����Ăяo�����j
 *
 *
 */
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�����Đ��֐�
 */
//------------------------------------------------------------------
void	PMVOICE_PlayVoiceByPlayer( u32 pokeNum )
{
	PMSND_PLAYER_DATA* ppd = &systemPlayerUnit.playerDataArray[PLAYER_SEVOICE];

	NNS_SndPlayerStopSeq(&ppd->sndHandle, 0);
	// �v���C���[�g�p�`�����l���ݒ�
	NNS_SndPlayerSetAllocatableChannel(ppd->playerNo, PLAYER_PMVOICE_CH);
	ppd->soundIdx = pokeNum;

	NNS_SndArcPlayerStartSeqEx(&ppd->sndHandle, ppd->playerNo, pokeNum, 127, SEQ_PV);
}

//------------------------------------------------------------------
/**
 * @brief	�����I�����o
 */
//------------------------------------------------------------------
BOOL	PMVOICE_CheckPlayByPlayer( void )
{
	return PMSND_CheckPlaySE();	//����̓v���[���[�������ɐݒ肵�Ă���̂œ�������
}
#endif





