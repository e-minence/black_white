//=============================================================================
/**
 * @file	vchat.c
 * @bfief	�{�C�X�`���b�g���b�p�[�B
 * @author	kazuki yoshihara
 * @date	06/02/27
 */
//=============================================================================

#include "gflib.h"
#include "system\ds_system.h"

#include "net/dwc_rap.h"
#include "net/dwc_rapcommon.h"
#include "vchat.h"
#include <vct.h>

#define MYNNSFORMAT NNS_SND_STRM_FORMAT_PCM16

//----------------- ���̕����̓f�t�H���g�Ō��܂��Ă��܂��B���M�ʂƂ͊֌W���܂���
#define VCHAT_SAMPLING_RATE       (8000)   // Hz
#define SAMPLING_BYTE        (2)      // byte = 16bit
#define MAX_SAMPLING_TIME   (68)     // ms
#define MAX_CHANNELS 1
#define VCHAT_WAVE_SAMPLE ((int) (VCHAT_SAMPLING_RATE * MAX_SAMPLING_TIME * SAMPLING_BYTE) / 1000)  //1088
//-----------------
#define _SOUND_CHANNEL (0) //HGSS�d�l

#define VCFINFO_SHOW
//#define VCT_ERROR_NONE (VCT_SUCCESS)


// �f�o�b�O�o�͂��ʂɓf���o���ꍇ��`
#if defined(DEBUG_ONLY_FOR_ohno)
#define DEBUGPRINT_ON (0)
#else
#define DEBUGPRINT_ON (0)
#endif


#if DEBUGPRINT_ON
#define VCT_PRINT(...) \
	(void) ((OS_TPrintf(__VA_ARGS__)))
#else   //DEBUGPRINT_ON
#define VCT_PRINT(...)           ((void) 0)
#endif  // DEBUGPRINT_ON



// �}�C�N�Q�C����ݒ�
#define MY_AMPGAIN PM_AMPGAIN_160

// ���t���[���܂ŗ����������āA�`�F�b�N���邩
#define HAWLING_CHECKFRAME 16

// �d�b�������Ă�����x���g���C���鎞��
#define VCT_START_TIMEOUT_NUM 60

#define _MAX_PLAYER_NUM  (2)

typedef struct{
	u8 sRecBuffer[VCHAT_WAVE_SAMPLE * 2 * MAX_CHANNELS];
	u8* pAudioBuffer;
	u8 sPlayBuffer[VCHAT_WAVE_SAMPLE * 2 * MAX_CHANNELS];
	// �����T�E���h�Ƃ��ēn���p�̃o�b�t�@�i��ɂO�j
	u8 sSilentBuffer[VCHAT_WAVE_SAMPLE * 2 * MAX_CHANNELS];
	void (*disconnectCallback)();
	VCTSession sSession[(_MAX_PLAYER_NUM-1)];
	BOOL bConf[_MAX_PLAYER_NUM];  //�Θb���[�h���ǂ���
	int mode;     // vct�̉�b���[�h
	int state;
	int off_flag;
	int heapID;
	struct NNSSndStrm sSndStream;
	VCTSession *session;
	MICAutoParam micParam;
	u16 vctTime;  //���N�G�X�g�̎��Ԍv��
	u8 bSendVoice;
	u8 firstCallback;

}MYVCT_WORK;

enum{
	VCTSTATE_INIT,
	VCTSTATE_WAIT,
	VCTSTATE_CALLING
};

static MYVCT_WORK* _vWork = NULL;

static void StartSoundLoop();
static void ClearSession(VCTSession *session);

// �����������B�e�q���ʁB�Z�b�V�����m����ɌĂ΂��B
//
// �����n�̏������i�ŏ��̂P��̂݁j
//

static OSTick _tick_time;
static int _difftime = 0;


//----------------------------------------------------------------------------
/**
 *	@brief	�������ɌĂ΂�鏈��
 *  @retval �Ȃ�
 */
//-----------------------------------------------------------------------------

static void InitFirst(void)
{
	VCT_PRINT("Init sound system\n");
	// �}�C�N�֘A�̏�����
	MIC_Init();
	PM_Init();

	{
		u32 ret;
		ret = PM_SetAmp(PM_AMP_ON);
		if( ret == PM_RESULT_SUCCESS )
		{
			VCT_PRINT("AMP���I���ɂ��܂����B\n");
		}
		else
		{
			VCT_PRINT("AMP�̏������Ɏ��s�i%d�j", ret);
		}
	}

	{
		u32 ret;
		ret = PM_SetAmpGain(MY_AMPGAIN);
		if( ret == PM_RESULT_SUCCESS )
		{
			VCT_PRINT("AMP�̃Q�C����ݒ肵�܂����B\n");
		}
		else
		{
			VCT_PRINT("AMP�̃Q�C���ݒ�Ɏ��s�i%d�j", ret);
		}
	}

	VCT_EnableVAD( TRUE );
	// �T�E���h�V�X�e���̏�����
	//
	NNS_SndInit();
	NNS_SndStrmInit(&_vWork->sSndStream);

	MI_CpuClearFast(_vWork->sSilentBuffer, sizeof(_vWork->sSilentBuffer) );
	VCT_PRINT("Init sound system done.\n");

	_difftime = 0;

	VCT_EnableEchoCancel(TRUE);
}

//
// ���������R�[���o�b�N�iNitroSystem NNS_SndStrm�𗘗p�j
//
static void micCallback(MICResult result, void *arg)
{
#pragma unused(result, arg)
  NET_PRINT("MIC %d\n",result);

}

static void SndCallback(NNSSndStrmCallbackStatus sts,
												int nChannels,
												void* buffer[],
												u32 length,
												NNSSndStrmFormat format,
												void* arg)
{
#pragma unused(format)

	OSTick      start;
	const void *micAddr;
	u32         offset;
	u8*         micSrc;
	u32         ch;


	micSrc = (u8*)arg;

	if (sts == NNS_SND_STRM_CALLBACK_SETUP) {
		for (ch = 0; ch < nChannels; ++ch) {
			MI_CpuClear8(buffer[ch], length);
		}
		return;
	}

#if (defined(SDK_TWL))  //�����炪�{��

  if(!DS_SYSTEM_IsRunOnTwl()){
    if (_vWork->firstCallback) {
      GF_ASSERT(MIC_RESULT_SUCCESS == MIC_StartAutoSamplingAsync( &(_vWork->micParam), micCallback, NULL));
      _vWork->firstCallback = 0;
    }
  }
  else{
    if (_vWork->firstCallback) {
      GF_ASSERT(MIC_RESULT_SUCCESS == MIC_StartLimitedSamplingAsync( &(_vWork->micParam), micCallback, NULL));
      _vWork->firstCallback = 0;
    }
  }
#else

  if (_vWork->firstCallback) {
    GF_ASSERT(MIC_RESULT_SUCCESS == MIC_StartAutoSamplingAsync( &(_vWork->micParam), micCallback, NULL));
    _vWork->firstCallback = 0;
  }

#endif

  
	micAddr = MIC_GetLastSamplingAddress();
	offset  = (u32)((u8*)micAddr - micSrc);

	if ( offset < length ) {
		micSrc = micSrc + length;
	}



	if( PAD_DetectFold() )
	{
		// DS������ꍇ
		micSrc = _vWork->sSilentBuffer;
	}

	// ��������M�͎�M�͏�ɍs���܂��BVAD��SSP�̃X�e�[�g�Ǘ��ɂ���āA�Z�b�V����������Ƃ��̂�
	// ���ۂɑ��M�E��M���s���܂��B
	//
	// length�͌��݃I�[�f�B�I�̃T���v�����O���[�g�E�r�b�g���ɉ�����32ms���ɂ̂ݑΉ����Ă��܂��B
	// �i��F8KHz, 8Bit ��256�o�C�g�j�B����ȊO�̃T�C�Y��n����assertion���܂��B
	//

  for (ch = 0; ch < nChannels; ++ch) {
		if( !VCT_ReceiveAudio(buffer[ch], length, NULL) )
		{
			VCT_PRINT("�����f�[�^���󂯎��܂���\n");
		}
	}
	if( _vWork->off_flag == 0 )
	{
    if(VCT_SendAudio(micSrc, length)){
      VCT_PRINT("�� length 0x%x\n", length);
		}
  }else{
    //   	    VCT_PRINT("�~");
	}
	_vWork->bSendVoice = 2;
	return;
}




/////////////////////////////////////////////////////////////////////////////////////
//
// �d�b��������B
//

static int startCall( u8 aid )
{
	BOOL ret;
	VCTSession *session;


	if (_vWork->session == NULL) {
		session = VCT_CreateSession(aid);
		if (session == NULL) {
			// �Z�b�V�������g���؂��Ă���A�܂���aid���������g�̏ꍇ
			// CraeteSession �����s���܂�
			VCT_PRINT("Can't create session!\n");
			return 0;
		}

		ret = VCT_Request(session, VCT_REQUEST_INVITE);  //send
	}
	else{
		session = _vWork->session;
		ret = VCT_Request(session, VCT_REQUEST_INVITE);  //������x����
	}
	if (ret != VCT_ERROR_NONE){
		VCT_PRINT("Can't request Invite [%d]\n", ret);
		VCT_DeleteSession(session);
		_vWork->session = NULL;
		return 0;
	}
	else {
		_vWork->session = session;
	}

	return 1;
}

static int receiveCall( u8 aid )
{
	BOOL ret;
	if ( _vWork->session != NULL && _vWork->session->state == VCT_STATE_INCOMING ) {
		// ���̒[�������b�v�����������ꍇ�A������OK��Ԃ��ăX�g���[�~���O���J�n

		ret = VCT_Response(_vWork->session, VCT_RESPONSE_OK);
		if (ret != VCT_ERROR_NONE){
			VCT_PRINT("Can't send response OK [%d]\n", ret);
			return 0;
		}

		if( !VCT_StartStreaming(_vWork->session) )
		{
			VCT_PRINT("can't start session! %d\n", _vWork->session->aid);
			return 0;
		} else {
			VCT_PRINT("�X�g���[�~���O���J�n���܂�%d\n", _vWork->session->aid);
		}


		return 1;
	}

	return 0;

}


/////////////////////////////////////////////////////////////////////////////////////
//
//  VoiceChat�̃C�x���g�R�[���o�b�N�֐� �J���t�@�����X���[�h
//
static void VoiceChatEventCallbackConference(u8 aid, VCTEvent event, VCTSession *session, void *data)
{
#pragma unused(aid, data)

	switch (event) {
	case VCT_EVENT_DISCONNECTED:
		VCT_PRINT("Disconnected\n");
		ClearSession(session);
		break;
	case VCT_EVENT_CONNECTED:
		VCT_StartStreaming(session);
		VCT_PRINT("Connected\n");
		break;
	case VCT_EVENT_ABORT:
		VCT_PRINT("Session abrot\n");
		ClearSession(session);
		break;
	default:
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//
// VoiceChat�̃C�x���g�R�[���o�b�N�֐� �d�b���[�h
//
static void VoiceChatEventCallbackPhone(u8 aid, VCTEvent event, VCTSession *session, void *data)
{
#pragma unused(aid, data)

	int ret;

	VCT_PRINT("event %d\n");
	switch (event) {
	case VCT_EVENT_INCOMING:
		if (_vWork->session) {
			VCT_PRINT("Return busy to %d\n", session->aid);
			ret = VCT_Response(session, VCT_RESPONSE_BUSY_HERE);
			VCT_DeleteSession(session);
			break;
		}

		VCT_PRINT("Invite From %d\n", session->aid);
		_vWork->session = session;
		break;

	case VCT_EVENT_RESPONDBYE:
		VCT_PRINT("Bye From %d\n", session->aid);
		ret = VCT_Response(session, VCT_RESPONSE_OK);
		if (ret != VCT_ERROR_NONE){
			VCT_PRINT("Can't send response Ok [%d]\n", ret);
		}
		ClearSession(session);
		GFL_NET_DWC_StopVChat();
		break;

	case VCT_EVENT_DISCONNECTED:
		VCT_PRINT("Disconnected\n");
		ClearSession(session);
		GFL_NET_DWC_StopVChat();
		break;

	case VCT_EVENT_CANCEL:
		VCT_PRINT("Cancel From %d\n", session->aid);
		ret = VCT_Response(session, VCT_RESPONSE_TERMINATED);
		if (ret != VCT_ERROR_NONE){
			VCT_PRINT("Can't send response RequestTerminated [%d]\n", ret);
		}
		ClearSession(session);
		break;

	case VCT_EVENT_CONNECTED:
		VCT_PRINT("200 OK From %d\n", session->aid);
		if(session->mode != _vWork->mode){
			ClearSession(session);
			VCT_PRINT("�Z�b�V�������Ⴄ���̂�����\n");
			return;
		}
		if( VCT_StartStreaming(session) )
		{
			VCT_PRINT("�X�g���[�~���O���J�n���܂�%d\n", session->aid);
			_vWork->state = VCTSTATE_CALLING;
		} else
		{
			VCT_PRINT("can't start session! %d\n", session->aid);
		}
		break;

	case VCT_EVENT_REJECT:
		VCT_PRINT("Reject From %d\n", session->aid);
		ClearSession(session);
		break;

	case VCT_EVENT_BUSY:
	case VCT_EVENT_TIMEOUT:
	case VCT_EVENT_ABORT:
		VCT_PRINT("Clear session by '%d'\n", event);
		ClearSession(session);
		break;

	default:
		VCT_PRINT(" not handler (%d)\n", event);
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////
//
// �Z�b�V�����̃N���A
//
static void ClearSession(VCTSession *session)
{
	VCT_StopStreaming(session);
	VCT_DeleteSession(session);
	_vWork->session = NULL;
}


// ���t���[���Ă΂��B
void myvct_main(BOOL offflg)
{

	_vWork->off_flag = offflg;
	{
		// �Q�[���t���[���Ɉ�x�Ăяo�����C���֐��B
		OSTick      start;
		start = OS_GetTick();
		_difftime += OS_TicksToMicroSeconds32(start - _tick_time) - 1000 * 1000 / 60;
		if( _difftime < -10000 ) _difftime = 0;
		_tick_time = start;
		VCT_Main();
		while( _difftime >= 1000 * 1000 / 60 )
		{
//			NET_PRINT("!");
			VCT_Main();
			_difftime -= 1000 * 1000 / 60 ;
		}
	}


	if(_vWork->mode != VCT_MODE_CONFERENCE){  // �J���t�@�����X�̏ꍇ���łɉ�b�������ɂ���

//    VCT_PRINT("vctst: %d \n",_vWork->state );

		switch( _vWork->state )
		{
		case VCTSTATE_INIT:
			{
				if( DWC_GetMyAID() == 0 ){
					// �e���d�b��������B
					if( startCall(1) ) {
						// ����̔����҂�
						_vWork->state = VCTSTATE_WAIT;
						_vWork->vctTime = VCT_START_TIMEOUT_NUM;
					}
				}
				else if(DWC_GetMyAID() == 1)
				{
					// �e����d�b���������Ă���̂�҂�
					if( receiveCall(0) )
					{
						// �������Ă���
						_vWork->state = VCTSTATE_CALLING;
					}
				}
				break;
			}

		case VCTSTATE_WAIT:
			// �q�@���d�b�ɏo��̂�҂��Ă���B
			_vWork->vctTime --;
			if(_vWork->vctTime ==0){
				_vWork->state = VCTSTATE_INIT; //������x�d�b��������
			}
			break;

		case VCTSTATE_CALLING:
			// �d�b��
			break;
		}
	}
}


BOOL myvct_checkData( int aid, void *data, int size )
{
	if( _vWork == NULL ) return FALSE;

	if( VCT_HandleData (aid, data, size ) ){
#ifdef VCFINFO_SHOW
		VCT_PRINT(".");
#endif
		return TRUE;
	}
	return FALSE;
}

void myvct_init( int heapID, int codec,int maxEntry )
{
//	u8 cArray[3] = {13, 13, 13};
	u8 cArray[3] = {_SOUND_CHANNEL, _SOUND_CHANNEL, _SOUND_CHANNEL};  //�T�E���h��SOUND_CHANNEL
	u32 length;
	BOOL ret;
	int size;
	int vctmode = VCT_MODE_PHONE;

	VCT_PRINT("myvct%d %d\n",codec,maxEntry);
	if( _vWork == NULL )
	{
		void* vWork_temp=NULL;

#if 1
		_vWork = DWC_RAPCOMMON_Alloc(heapID, sizeof(MYVCT_WORK),32);
		_vWork->pAudioBuffer = DWC_RAPCOMMON_Alloc(heapID, VCT_AUDIO_BUFFER_SIZE * maxEntry * VCT_DEFAULT_AUDIO_BUFFER_COUNT + 32 ,32);
#else
    _vWork = GFL_NET_Align32Alloc(heapID, sizeof(MYVCT_WORK));
		_vWork->pAudioBuffer = GFL_NET_Align32Alloc(heapID, VCT_AUDIO_BUFFER_SIZE * maxEntry * VCT_DEFAULT_AUDIO_BUFFER_COUNT + 32 );
#endif
    //	_vWork->heapID = heapID;
		_vWork->disconnectCallback = NULL;
		InitFirst();
	}

	// �}�C�N�̏�����

	length = (u32)VCHAT_WAVE_SAMPLE;

	{
		_vWork->micParam.type   = MIC_SAMPLING_TYPE_SIGNED_12BIT;
		_vWork->micParam.buffer = _vWork->sRecBuffer;
		_vWork->micParam.size   = length * 2;
    if( !DS_SYSTEM_IsRunOnTwl() ){//DSI�Ȃ�
      _vWork->micParam.rate = (u32)((NNS_SND_STRM_TIMER_CLOCK / VCHAT_SAMPLING_RATE) * 64);
    }
    else{
#if (defined(SDK_TWL))  //�����炪�{��
      _vWork->micParam.rate = (u32)MIC_SAMPLING_RATE_8180;
#else
      _vWork->micParam.rate = (u32)((NNS_SND_STRM_TIMER_CLOCK / VCHAT_SAMPLING_RATE) * 64);
#endif
    }
    _vWork->micParam.loop_enable = TRUE;
		_vWork->micParam.full_callback = NULL;
		_vWork->micParam.full_arg = NULL;
		_vWork->firstCallback = 1;
	}

	// �T�E���h�X�g���[���Đ��̏������B�P�΂P��b����
	GF_ASSERT(NNS_SndStrmAllocChannel(&_vWork->sSndStream, 1, cArray));
	NNS_SndStrmSetVolume(&_vWork->sSndStream, 0);

  VCT_PRINT("NNS_SndStrmSetup\n");

  {
    u32 num;

    if( DS_SYSTEM_IsRunOnTwl() ){//DSI�Ȃ�
      num = NNS_SND_STRM_TIMER_CLOCK / 8180;
    }
    else{
      num = NNS_SND_STRM_TIMER_CLOCK / VCHAT_SAMPLING_RATE;
    }

  
    ret = NNS_SndStrmSetup(&_vWork->sSndStream,
                           NNS_SND_STRM_FORMAT_PCM16,
                           _vWork->sPlayBuffer,
                           length * 2 * 1,
                           num,
                           2,
                           SndCallback,
                           _vWork->sRecBuffer);
    GF_ASSERT(ret);
  }

	_vWork->state = VCTSTATE_INIT;
	_vWork->session = NULL;

	{
		VCTConfig config;

		//        if(!CommLocalIsWiFiQuartetGroup(CommStateGetServiceNo())){   // �S�l�ڑ��̎��̓{�C�X�`���b�g�������N�����Ȃ�
		//    config.mode         = //VCT_MODE_PHONE;
		//  }
		//    else{
		//          config.mode         = //VCT_MODE_CONFERENCE;
		//        }
		config.mode = vctmode;
		_vWork->mode = config.mode;
		config.session      = _vWork->sSession;
		config.numSession   = maxEntry;
		config.aid          = DWC_GetMyAID();
		GF_ASSERT((config.aid != -1));
		if(_vWork->mode == VCT_MODE_CONFERENCE){
			config.callback = VoiceChatEventCallbackConference;
		}
		else{
			config.callback = VoiceChatEventCallbackPhone;
		}
		config.userData     = NULL;

		config.audioBuffer     = _vWork->pAudioBuffer;
		config.audioBufferSize = VCT_AUDIO_BUFFER_SIZE * maxEntry * VCT_DEFAULT_AUDIO_BUFFER_COUNT + 32;


		if (!VCT_Init(&config)) {
			VCT_PRINT("ERROR: Can't initialize VoiceChat!!!\n");
		}
	}

	_vWork->off_flag = 0;

	VCT_SetCodec( codec );
	StartSoundLoop();

	VCT_EnableEchoCancel( TRUE );

	VCT_PRINT("myvct_init\n");
	return;
}

static void StartSoundLoop()
{
	NNS_SndStrmStart(&_vWork->sSndStream);
}

void myvct_setCodec( int codec )
{
	VCT_SetCodec( codec );
}




//==============================================================================
/**
 * ��b�I���v���������܂��B�܂��ʘb�ł��Ă��Ȃ��Ƃ��͑����ɏI�����܂��B
 * myvct_setDisconnectCallback�Őݒ肳�ꂽ�R�[���o�b�N���Ăяo����܂��B
 * @param   none
 * @retval  none
 */
//==============================================================================
void myvct_endConnection(){
	int ret;

	// �܂���b�v�����o���O�ŁA�󂯎��O
	if( _vWork->session == NULL || _vWork->state == VCTSTATE_INIT ) {
		GFL_NET_DWC_StopVChat();
		return;
	}

	if( _vWork->state == VCTSTATE_WAIT )
	{
		// ��b�v�����o���Ă܂��Ԏ�������O�B
		ret = VCT_Request( _vWork->session, VCT_REQUEST_CANCEL );
		if (ret != VCT_ERROR_NONE){
			VCT_PRINT("Can't request Cancel [%d]\n", ret);
			GFL_NET_DWC_StopVChat();
			return;
		}
	}

	// ��b���B��b�I���v�����o���B
	ret = VCT_Request( _vWork->session, VCT_REQUEST_BYE );
	if (ret != VCT_ERROR_NONE){
		VCT_PRINT("Can't request Bye [%d]\n", ret);
		GFL_NET_DWC_StopVChat();
		return;
	}
	return;
}

//==============================================================================
/**
 * ��b�I�����������R�[���o�b�N��ݒ肵�܂��B
 * ���肩��؂�ꂽ�ꍇ���Ăяo����܂��B
 * ���̊֐����Ăяo����钼�O�ɁAvchat.c�p�̃��[�N���������܂��B
 * @param   none
 * @retval  none
 */
//==============================================================================
void myvct_setDisconnectCallback( void (*disconnectCallback)() )
{
	_vWork->disconnectCallback = disconnectCallback;
}

//==============================================================================
/**
 * ���C�u�����I������
 * @param   none
 * @retval  none
 */
//==============================================================================
void myvct_free(void){
	void (*callback)();

	if( _vWork != NULL )
	{
		callback = _vWork->disconnectCallback;

		// �}�C�N�̃T���v�����O�ƃX�g���[�����Ƃ߂�B
    if( !DS_SYSTEM_IsRunOnTwl() ){//DS�Ȃ�
      GF_ASSERT(MIC_RESULT_SUCCESS == MIC_StopAutoSampling());
    }
    else{
#if (defined(SDK_TWL))  //�����炪�{��
      GF_ASSERT(MIC_RESULT_SUCCESS == MIC_StopLimitedSampling());
#else
      GF_ASSERT(MIC_RESULT_SUCCESS == MIC_StopAutoSampling());
#endif
    }

  
		NNS_SndStrmStop(&_vWork->sSndStream);
		NNS_SndStrmFreeChannel(&_vWork->sSndStream);

		// VCT���C�u�����I������
		VCT_Cleanup();

		// �����������
		DWC_RAPCOMMON_Free( 0, _vWork->pAudioBuffer,0 );
		DWC_RAPCOMMON_Free( 0,_vWork,0  );
		_vWork = NULL;

		// �R�[���o�b�N�̌Ăяo���B
		if( callback != NULL ) callback();
	}
}

//==============================================================================
/**
 * �����E�����̂��ǂ����𒲂ׂ�
 * @param
 * @retval  �E������TRUE
 */
//==============================================================================

BOOL myvct_IsSendVoiceAndInc(void)
{
	if(_vWork){
		VCTVADInfo outInfo;
		VCT_GetVADInfo( &outInfo );
		if(outInfo.scale > 2){
			return outInfo.activity;
		}
	}
	return FALSE;
}

//==============================================================================
/**
 * VCT�J���t�@�����X�ɏ��҂���
 * @param   bitmap   �ڑ����Ă���CLIENT��BITMAP
 * @param   myAid    ������ID
 * @retval  TRUE �ݒ�ł���
 * @retval  FALSE ����̂܂� �������͕K�v���Ȃ�
 */
//==============================================================================

BOOL myvct_AddConference(int bitmap, int myAid)
{
	int i,ret;

	if(!(_vWork)  || (_vWork->mode != VCT_MODE_CONFERENCE)){
		return FALSE;
	}

	for(i = 0; i < _MAX_PLAYER_NUM;i++){
		if(i == myAid){
			continue;
		}
		if( bitmap & (1<<i)){  //����ꍇ
			if(_vWork->bConf[i]==TRUE){
				continue;
			}
			ret = VCT_AddConferenceClient(i);
			if (ret != VCT_ERROR_NONE){
				VCT_PRINT("AddConferenceError [%d]\n", ret);
				return FALSE;
			}
			else{
				_vWork->bConf[i] = TRUE;
				VCT_PRINT("AddConference�ݒ� %d\n", i);
			}
		}
	}
	return TRUE;
}

//==============================================================================
/**
 * VCT�J���t�@�����X����S���͂���
 * @param   bitmap   �ڑ����Ă���CLIENT��BITMAP
 * @param   myAid    ������ID
 * @retval  TRUE �ݒ�ł���
 * @retval  FALSE ����̂܂� �������͕K�v���Ȃ�
 */
//==============================================================================

BOOL myvct_DelConference(int myAid)
{
	int i,ret;

	if(!(_vWork)  || (_vWork->mode != VCT_MODE_CONFERENCE)){
		return FALSE;
	}

	for(i = 0; i < _MAX_PLAYER_NUM;i++){
		if(i == myAid){
			continue;
		}
		if(_vWork->bConf[i]==TRUE){
			ret = VCT_RemoveConferenceClient(i);
			if (ret != VCT_ERROR_NONE){
				VCT_PRINT("DELConferenceError [%d]\n", ret);
				return FALSE;
			}
			else{
				_vWork->bConf[i] = FALSE;
			}
		}
	}
	return TRUE;
}

