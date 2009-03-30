//============================================================================================
/**
 * @file	pm_voice.c
 * @brief	�|�P���������g�`�Đ���{�V�X�e��	
 * @author	
 * @date	
 */
//============================================================================================
#include "gflib.h"

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
#define PMVOICE_WAVESIZE_MAX	(26000)

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

	NNSSndWaveOutHandle waveHandle;	
	NNSSndWaveOutHandle waveHandleSub;	
	u8					volume;
	u8					volumeSub;
	u8					channel:4;
	u8					channelSub:4;
	int					speed;
	int					speedSub;
	u8					pan;
	BOOL				subWaveUse;
}PMVOICE_PLAYER;

//------------------------------------------------------------------
/**
 * @brief	�V�X�e����`
 */
//------------------------------------------------------------------
#define VOICE_PLAYER_NUM (3)

typedef struct {
	HEAPID			heapID;
	PMVOICE_PLAYER	voicePlayer[VOICE_PLAYER_NUM];
	u16				voicePlayerRef;
	u16				voicePlayerEnableNum;
	BOOL			voicePlayerHeapReserveFlag;
	PMVOICE_CB_GET_WVIDX*	CallBackGetWaveIdx;
	PMVOICE_CB_GET_WVDAT*	CallBackCustomWave;
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

	for( i=0; i<VOICE_PLAYER_NUM; i++ ){
		voicePlayer = &pmvSys.voicePlayer[i];

		voicePlayer->active = FALSE;
		voicePlayer->channel = useChannnelPriority[i][0];
		voicePlayer->channelSub = useChannnelPriority[i][1];
		initPlayerWork(voicePlayer);
	}
	pmvSys.voicePlayerRef = 0;
	pmvSys.voicePlayerEnableNum = VOICE_PLAYER_NUM;
	pmvSys.voicePlayerHeapReserveFlag = FALSE;

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
		if(voicePlayer->waveData != NULL){ GFL_HEAP_FreeMemory(voicePlayer->waveData); }

		voicePlayer->active = FALSE;
		initPlayerWork(voicePlayer);
	}
}

//============================================================================================
/**
 * @brief	�v���[���[�pwave�o�b�t�@���O�m��
 */
//============================================================================================
void	PMVOICE_PlayerHeapReserve( int num )
{
	PMVOICE_PLAYER* voicePlayer;
	int i;

	PMVOICE_Reset();

	if( num > VOICE_PLAYER_NUM ){ num = VOICE_PLAYER_NUM; }

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

	PMVOICE_Reset();

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
static void initPlayerWork( PMVOICE_PLAYER* voicePlayer )
{
	voicePlayer->waveData = NULL;
	voicePlayer->waveSize = 0;
	voicePlayer->waveRate = VOICE_RATE_DEFAULT;
	voicePlayer->volume = 0;
	voicePlayer->volumeSub = 0;
	voicePlayer->speed = VOICE_SPEED_DEFAULT;
	voicePlayer->speedSub = VOICE_SPEED_DEFAULT;
	voicePlayer->pan = 64;
	voicePlayer->subWaveUse = FALSE;
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

	voicePlayer->waveSize = waveFileSize;
	voicePlayer->waveRate = VOICE_RATE_DEFAULT;
	voicePlayer->speed = VOICE_SPEED_DEFAULT;

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

	result = NNS_SndWaveOutStart(	voicePlayer->waveHandle,	// �g�`�Đ��n���h��
									NNS_SND_WAVE_FORMAT_PCM8,	// �g�`�f�[�^�t�H�[�}�b�g
									voicePlayer->waveData,		// �g�`�f�[�^�A�h���X
									FALSE, 0,					// ���[�v�t���O,�J�n�ʒu
									voicePlayer->waveSize,		// �g�`�f�[�^�T���v����
									voicePlayer->waveRate,		// �g�`�f�[�^�T���v�����O���[�g
									voicePlayer->volume,		// �Đ�volume
									voicePlayer->speed,			// �Đ�speed
									voicePlayer->pan );			// �Đ�pan
	if( result == FALSE ){ return FALSE; }

	if( voicePlayer->subWaveUse == TRUE ){
		// �g�p�`�����l���m��
		voicePlayer->waveHandleSub = NNS_SndWaveOutAllocChannel(voicePlayer->channelSub);		
		if( voicePlayer->waveHandleSub == NNS_SND_WAVEOUT_INVALID_HANDLE ){ return FALSE; } 

		result = NNS_SndWaveOutStart(	voicePlayer->waveHandleSub,	// �g�`�Đ��n���h��
										NNS_SND_WAVE_FORMAT_PCM8,	// �g�`�f�[�^�t�H�[�}�b�g
										voicePlayer->waveData,		// �g�`�f�[�^�A�h���X
										FALSE, 0,					// ���[�v�t���O,�J�n�ʒu
										voicePlayer->waveSize,		// �g�`�f�[�^�T���v����
										voicePlayer->waveRate,		// �g�`�f�[�^�T���v�����O���[�g
										voicePlayer->volumeSub,		// �Đ�volume
										voicePlayer->speedSub,		// �Đ�speed
										voicePlayer->pan );			// �Đ�pan
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

	// �g�`�f�[�^�o�b�t�@�J��
	if(voicePlayer->waveData != NULL){
		if( pmvSys.voicePlayerHeapReserveFlag == FALSE ){
			GFL_HEAP_FreeMemory(voicePlayer->waveData); 
		}
	}
	voicePlayer->active = FALSE;
	initPlayerWork(voicePlayer);
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
	PMVOICE_PLAYER* voicePlayer;
	u16		voicePlayerIdx;
	u32		waveIdx;
	BOOL	waveLoadFlag;

	// �g�`IDX�擾
	pmvSys.CallBackGetWaveIdx(pokeNo, pokeFormNo, &waveIdx);

	// �Đ��v���[���[�擾
	voicePlayerIdx = getPlayerIdx();
	voicePlayer = &pmvSys.voicePlayer[voicePlayerIdx];

	// �g�`�f�[�^�o�b�t�@�m��
	if( pmvSys.voicePlayerHeapReserveFlag == FALSE ){
		// ��ɍő�l�Ŋm�ہi��ʂɂ���ĈقȂ�Ɛ��݃o�O�𐶂މ\�������邽�߁j
		voicePlayer->waveData = GFL_HEAP_AllocClearMemory(pmvSys.heapID, PMVOICE_WAVESIZE_MAX);
	}
	
	// �g�`�f�[�^�J�X�^�}�C�Y(TRUE: �R�[���o�b�N���Ő������ꂽ)
	waveLoadFlag = pmvSys.CallBackCustomWave(	pokeNo, pokeFormNo, userParam,
													&voicePlayer->waveData, 
													&voicePlayer->waveSize, 
													&voicePlayer->waveRate, 
													&voicePlayer->speed);
	// �R�[���o�b�N���Ő�������Ȃ������ꍇ�A�g�`�f�[�^�擾
	if( waveLoadFlag == FALSE ){ loadWave(voicePlayer, waveIdx); }

	// �t�Đ��p�f�[�^���H
	if( reverse ){ reverseBuf(voicePlayer->waveData, voicePlayer->waveSize); }

	// �e��ݒ�
	voicePlayer->volume = VOICE_VOLUME;
	voicePlayer->pan = pan;
	voicePlayer->subWaveUse = chorus;
	voicePlayer->volumeSub = voicePlayer->volume + chorusVolOfs;
	voicePlayer->speedSub = voicePlayer->speed + chorusSpOfs;

	// �g�`�Đ�
	if( playWave(voicePlayer) == FALSE ){ stopWave(voicePlayer); };
	
	OS_Printf("voicePlayerIdx = %x\n",voicePlayerIdx);
	return voicePlayerIdx;
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


