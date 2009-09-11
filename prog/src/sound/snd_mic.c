//=============================================================================
/**
 *
 *	@file		snd_mic.c
 *	@brief  �}�C�N�֘A���W���[��
 *	@author	hosaka genya (GS����ڐA)
 *	@data		2009.09.10
 *
 */
//=============================================================================
#include "gflib.h"

#include "sound/snd_mic.h"

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================
#define SOUND_MIC_WAIT (3*30) ///< AMP��ON�ɂ��Ă���}�C�N���g�p�\�ɂȂ�܂�

#define SWAVE_BUFFER_SIZE		(2000)						//2k * 8bit

// �y���b�v
#define PERAP_SAMPLING_RATE		(2000)									//�T���v�����O���[�g
#define PERAP_SAMPLING_TIME		(1)										//�T���v�����O���鎞��
#define PERAP_SAMPLING_SIZE		(PERAP_SAMPLING_RATE * PERAP_SAMPLING_TIME)	//�K�v�ȃf�[�^��


//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
//--------------------------------------------------------------
/// �}�C�N�p���[�N
//==============================================================
typedef struct {
  // �I�[�g�T���v�����O�p
  MICAutoParam  AutoParamBuff;
  BOOL          isAutoSampStart;
	u32           amp_init_flag:1;		// Amp�ݒ�t���O
  u32           amp_wait_cnt:31;
  // �y���b�v�g�`�ۑ����[�N
  void*         p_PerapBuff;
} SND_MIC_WORK;

// �V���O���g��
static SND_MIC_WORK* sp_SndMic = NULL;

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static void MicCallback( MICResult /*result*/, void* /*arg*/ );
static void* Snd_GetWaveBufAdrs( void );

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �}�C�N���W���[�� ������
 *
 *	@param	HEAPID heap_id  �q�[�vID
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void SND_MIC_Init( HEAPID heap_id )
{
  SND_MIC_WORK* wk;

  wk = GFL_HEAP_AllocMemory( heap_id, sizeof( SND_MIC_WORK ) );
  GFL_STD_MemClear( wk, sizeof(SND_MIC_WORK) );
  
  //	�y���b�v�o�b�t�@�쐬
	{	
		u32 size  = sizeof(u8)*SWAVE_BUFFER_SIZE + 32;	//	��Ƀo�b�t�@�A�h���X��32�o�C�g�A���C�����邽��+32
		u8 *p_buf	= GFL_HEAP_AllocMemory( heap_id, size );
		MI_CpuClear8( p_buf, size );
		wk->p_PerapBuff = p_buf;
  }

  {
    //MIC_Init�֐��̏������́A2��ڈȍ~�̌Ăяo���͖����ɂȂ�悤�ɁA
    //�����t���O�Ń`�F�b�N����Ă��܂�
    MIC_Init();								//MIC_API������
      
    //OS_Init()���Ăԏꍇ�́A���̒�����Ă΂�܂��̂ŕK�v����܂���B
    //PM_Init();							//PMIC�}�l�[�W��������(OS_INIT���ł��Ă΂�Ă���)
    
    (void)PM_SetAmp( PM_AMP_ON );			//�v���O���}�u���Q�C���A���v�̃X�C�b�`ON
    (void)PM_SetAmpGain( PM_AMPGAIN_80 );	//�v���O���}�u���Q�C���A���v�̃Q�C����ݒ�
    
    //AMP������
    wk->amp_init_flag	= 1;
  }

  sp_SndMic = wk;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �}�C�N���W���[�� �J��
 *
 *	@param	void 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void SND_MIC_Exit( void )
{
  GF_ASSERT( sp_SndMic );

  GFL_HEAP_FreeMemory( sp_SndMic->p_PerapBuff );
  GFL_HEAP_FreeMemory( sp_SndMic );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �}�C�N���W���[�� �又��
 *
 *	@param	void 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
void SND_MIC_Main( void )
{
  SND_MIC_WORK* wk = sp_SndMic;

  GF_ASSERT( sp_SndMic );

	//3�b���ׂ邽�߂̃J�E���g
	if( wk->amp_init_flag ) {
		if( !SND_MIC_IsAmpOnWaitFlag() ) {	
			wk->amp_wait_cnt++;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	AMP��ON�ɂ��Ă���̃J�E���g���N���A�[���܂�
 *					��{�I�ɂ��̊֐��͈ꕔ���炵���g���Ȃ��͂��ł�
 *
 */
//-----------------------------------------------------------------------------
void SND_MIC_ClearAmpOnWait( void )
{	
  GF_ASSERT( sp_SndMic );

	sp_SndMic->amp_wait_cnt	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	AMP��ON�ɂ��Ă���3�b�҂������ǂ����̃t���O�擾
 *					�}�C�N�͂��ꂪON�ɂȂ�Ȃ��Ǝg�p���Ă͂����Ȃ��i�C�V���K��j
 *
 *	@retval	TRUE�Ȃ�Ύg�p����
 *	@retval	FALSE�Ȃ�Ύg�p�s����
 */
//-----------------------------------------------------------------------------
BOOL SND_MIC_IsAmpOnWaitFlag( void )
{	
  GF_ASSERT( sp_SndMic );

	//3�b�ȏソ�������ǂ���
  return sp_SndMic->amp_wait_cnt > SOUND_MIC_WAIT;
}

//--------------------------------------------------------------
/**
 * @brief	�^���J�n
 *
 * @param	p		MICAutoParam�^�̃A�h���X
 *
 * @retval	"MIC_RESULT_SUCCESS		����������Ɋ���"
 * @retval	"����ȊO				���炩�̌����Ŏ��s"
 */
//--------------------------------------------------------------
MICResult SND_MIC_StartAutoSampling( MICAutoParam* p )
{
	MICResult ret;
  
  GF_ASSERT( sp_SndMic );

	ret = MIC_StartAutoSampling( p );

	if( ret != MIC_RESULT_SUCCESS ){
		OS_Printf( "StartMicAutoSamoling Failed ret=%d \n", ret );
	}

  // �I�[�g�T���v�����O�J�n
  sp_SndMic->isAutoSampStart = TRUE;
  sp_SndMic->AutoParamBuff  = *p;

	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	�^����~
 *
 * @param	none
 *
 * @retval	"MIC_RESULT_SUCCESS		����������Ɋ���"
 * @retval	"����ȊO				���炩�̌����Ŏ��s"
 */
//--------------------------------------------------------------
MICResult SND_MIC_StopAutoSampling(void)
{
	MICResult ret;
  
  GF_ASSERT( sp_SndMic );

  // �I�[�g�T���v�����O��~
  sp_SndMic->isAutoSampStart = FALSE;

	ret = MIC_StopAutoSampling();

	if( ret != MIC_RESULT_SUCCESS ){
		OS_Printf( "MicStopAutoSampling Failed ret=%d \n", ret );
	}

	return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief    �}�C�N�X���[�v�N������
 */
//-----------------------------------------------------------------------------
void SND_MIC_StopSleep(void)
{
  GF_ASSERT( sp_SndMic );
  
  if( sp_SndMic->isAutoSampStart )
  {
	  MICResult ret;
	  ret = MIC_StopAutoSampling();
    GF_ASSERT( ret == MIC_RESULT_SUCCESS );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief    �}�C�N�X���[�v���A����
 */
//-----------------------------------------------------------------------------
void SND_MIC_ReStartSleep(void)
{
  GF_ASSERT( sp_SndMic );

  if( sp_SndMic->isAutoSampStart )
  {
	  MICResult ret;
	  ret = MIC_StartAutoSampling( &sp_SndMic->AutoParamBuff );
    GF_ASSERT( ret == MIC_RESULT_SUCCESS );
  }

  SND_MIC_ClearAmpOnWait();
}

//--------------------------------------------------------------
/**
 * @brief	�蓮�^��
 * @param	type      �T���v�����O���[�g�̃^�C�v
 * @param	heap      �L�^�̈�
 * @param	callback  �L�^�����ۂ̃R�[���o�b�N
 * @param	arg       �R�[���o�b�N�ɓn�������|�C���^
 * @retval	"MIC_RESULT_SUCCESS		����������Ɋ���"
 * @retval	"����ȊO				���炩�̌����Ŏ��s"
 */
//--------------------------------------------------------------
MICResult SND_MIC_ManualSampling(MICSamplingType type ,void* heap,MICCallback callback,void* arg)
{
	MICResult ret;

  ret = MIC_DoSamplingAsync( type, heap, callback, arg);

	if( ret != MIC_RESULT_SUCCESS ){
        OS_Printf( "�}�C�N�蓮�T���v�����O�����s\n" );
	}
	return ret;
}

//--------------------------------------------------------------
/**
 * @brief	�y���b�v�f�[�^�^���J�n(�T�E���h�V�X�e���̗̈�Ɉꎞ�ۑ�����)
 *
 * @param	none
 *
 * @retval	"MIC_RESULT_SUCCESS		����������Ɋ���"
 * @retval	"����ȊO				���炩�̌����Ŏ��s"
 */
//--------------------------------------------------------------
MICResult SND_PERAP_VoiceRecStart( void )
{
	MICAutoParam mic;	//�}�C�N�p�����[�^

	//�g�`�Đ��p�`�����l�����m�ۂ���
	//Snd_WaveOutAllocChannel( WAVEOUT_CH_NORMAL );

	mic.type			= MIC_SAMPLING_TYPE_SIGNED_8BIT;	//�T���v�����O���

	//�o�b�t�@��32�o�C�g�A���C�����ꂽ�A�h���X�łȂ��ƃ_���I
	mic.buffer		= Snd_GetWaveBufAdrs();

	mic.size			= PERAP_SAMPLING_SIZE;

	if( (mic.size&0x1f) != 0 ){
		mic.size &= 0xffffffe0;
	}

	//��\�I�ȃT���v�����O���[�g��ARM7�̃^�C�}�[�����Ɋ��Z�����l�̒�`
	//mic.rate			= MIC_SAMPLING_RATE_8K;
	mic.rate			= HW_CPU_CLOCK_ARM7 / PERAP_SAMPLING_RATE;

	//�A���T���v�����O���Ƀo�b�t�@�����[�v������t���O
	mic.loop_enable		= FALSE;

	//�o�b�t�@���O�a�����ۂɌĂяo���R�[���o�b�N�֐��ւ̃|�C���^
	mic.full_callback	= NULL;

	//�o�b�t�@���O�a�����ۂɌĂяo���R�[���o�b�N�֐��֓n������
	mic.full_arg		= NULL;

	return SND_MIC_StartAutoSampling( &mic );		//�^���J�n
}

//--------------------------------------------------------------
/**
 * @brief	�y���b�v�f�[�^�^����~
 *
 * @param	none
 *
 * @retval	"MIC_RESULT_SUCCESS		����������Ɋ���"
 * @retval	"����ȊO				���炩�̌����Ŏ��s"
 */
//--------------------------------------------------------------
MICResult SND_PERAP_VoiceRecStop( void )
{
	return SND_MIC_StopAutoSampling();
}

//--------------------------------------------------------------
/**
 * @brief	�^�������f�[�^���Z�[�u�f�[�^�ɃZ�b�g
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
void SND_PERAP_VoiceDataSave( PERAPVOICE* perap )
{
	PERAPVOICE_SetVoiceData( perap, (const s8*)Snd_GetWaveBufAdrs() );
}

//=============================================================================
/**
 *								static�֐�
 */
//=============================================================================

//--------------------------------------------------------------
/**
 * @brief	�}�C�N�R�[���o�b�N�֐�
 *
 * @param	none
 *
 * @retval	none
 */
//--------------------------------------------------------------
static void MicCallback( MICResult /*result*/, void* /*arg*/ )
{
    OS_Printf( "Mic Callback Done\n" );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �g�`�ۑ��o�b�t�@�ւ̃A�N�Z�T
 *
 *	@param	none
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void* Snd_GetWaveBufAdrs( void )
{
  u8* p_buf = sp_SndMic->p_PerapBuff;

  GF_ASSERT( sp_SndMic );

  return (void*)MATH_ROUNDUP32( (int)(p_buf) );
}

