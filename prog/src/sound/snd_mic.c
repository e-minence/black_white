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

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================
//--------------------------------------------------------------
/// �I�[�g�T���v�����O�p���[�N
//==============================================================
typedef struct {
  MICAutoParam  AutoParamBuff;
  BOOL          isAutoSampStart;
	u32           amp_init_flag:1;		// Amp�ݒ�t���O
  u32           amp_wait_cnt:31;
} SND_MIC_WORK;

// �V���O���g��
static SND_MIC_WORK* sp_SndMic = NULL;

//=============================================================================
/**
 *							�v���g�^�C�v�錾
 */
//=============================================================================
static void MicCallback( MICResult /*result*/, void* /*arg*/ );

//=============================================================================
/**
 *								�O�����J�֐�
 */
//=============================================================================

//-----------------------------------------------------------------------------
/**
 *	@brief  �}�C�N���W���[��������
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
 *	@brief  �}�C�N���W���[���J��
 *
 *	@param	void 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void SND_MIC_Exit( void )
{
  GF_ASSERT( sp_SndMic );

  GFL_HEAP_FreeMemory( sp_SndMic );
}

//-----------------------------------------------------------------------------
/**
 *	@brief
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
		OS_Printf( "�}�C�N�����T���v�����O�J�n�����s���܂����I\n" );
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
		OS_Printf( "�}�C�N�����T���v�����O��~�����s���܂����I\n" );
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

