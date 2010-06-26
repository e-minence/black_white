//============================================================================================
/**
 * @file  ringtone_sys.c
 * @date  2010.03.11
 * @author  tamada GAME FREAK inc.
 * @brief   TV�g�����V�[�o�[���M����DS�J�ł̃{�����[������
 *
 * ���M���V�X�e���̓t�B�[���h�T�E���h�V�X�e���A�v���C���[�{�����[������V�X�e����
 * �ˑ����Ă���A���ڌĂяo����邱�Ƃ͂Ȃ��B
 *
 *
 * 2010.3.18  ���M�����Ȃ��Ă��Ȃ��ꍇ�̓}�X�^�[�{�����[���𐧌䂷��悤�ɏC��
 */
//============================================================================================

#include <gflib.h>
#include "ringtone_sys.h"

#include "sound/pm_sndsys.h"
#include "player_volume_fader.h"


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief �V�X�e���̏�Ԓ�`
 */
//------------------------------------------------------------------
typedef enum {
  STAT_OPEN_OFF = 0, ///<�W�J���Ă�F���MOFF
  STAT_OPEN_ON,      ///<�W�J���Ă�F���MON
  STAT_CLOSE_OFF,    ///<�W���Ă�F���MOFF
  STAT_CLOSE_ON,     ///<�W���Ă�F���MON

  STAT_MAX,
}RINGTONE_STAT;

//------------------------------------------------------------------
/**
 * @brief ���N�G�X�g�̒�`
 */
//------------------------------------------------------------------
typedef enum {
  REQ_STOP = 0,   ///<���N�G�X�g�F���MOFF
  REQ_PLAY,       ///<���N�G�X�g�F���MON
  REQ_OPEN,       ///<���N�G�X�g�F�ӂ��J����
  REQ_CLOSE,      ///<���N�G�X�g�F�ӂ�����

  REQ_MAX,
}RINGTONE_REQ;

//------------------------------------------------------------------
/**
 * @brief ���M������V�X�e�����[�N��`
 */
//------------------------------------------------------------------
struct _RINGTONE_SYS{

  RINGTONE_STAT state;  ///<����V�X�e���̏��
  u16  ringWait;       ///<���M���������ԗp�J�E���^


  PLAYER_VOLUME_FADER * fader;  ///<NNS�̃v���C���[�{�����[������V�X�e���ւ̃|�C���^

};
//------------------------------------------------------------------
//------------------------------------------------------------------
enum {

  ///���M��SE�̃i���o�[
  RINGTONE_SE_NO = SEQ_SE_SYS_35, 

  ///���M��SE�Ɏg�p����v���C���[�̎w��ID
  RINGTONE_SEPLAYER = SEPLAYER_SE3,

  ///���M�����葱���钷��
  RINGTONE_LENGTH = 60 * 15,

  MIN_VOLUME = 0,
  MAX_VOLUME = 127,
};

//============================================================================================
//============================================================================================
static RINGTONE_SYS * ringToneSysPtr = NULL;

//============================================================================================
//============================================================================================

static void ringTone_sleepSoundGoFunc( void * work );
static void ringTone_sleepSoundReleaseFunc( void * work );
static BOOL ringTone_PlayMain( RINGTONE_SYS * rtone );

static void changeStatus( RINGTONE_SYS * rtone, RINGTONE_REQ req );
static void updateStatus( RINGTONE_SYS * rtone );


//============================================================================================
//
//    �O�����J�֐�
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief ���M������V�X�e���̐���
 * @param HEAPID  �g�p����q�[�v�̎w��ID
 * @param fader   PLAYER_VOLUME_FADER�ւ̃|�C���^
 * @return  RINGTONE_SYS  �����������M������V�X�e���̃|�C���^
 */
//------------------------------------------------------------------
RINGTONE_SYS * RINGTONE_SYS_Create( HEAPID heapID, PLAYER_VOLUME_FADER * fader )
{
  RINGTONE_SYS * rtone = GFL_HEAP_AllocClearMemory( heapID, sizeof(RINGTONE_SYS) );
  rtone->ringWait = 0;
  rtone->fader = fader;

  ringToneSysPtr = rtone;

  //���M������Ή���DS�J���T�E���h�R�[���o�b�N��o�^
  RINGTONE_SYS_SetSleepSoundFunc();

  return rtone;
}

//------------------------------------------------------------------
/**
 * @brief ���M������V�X�e���̍폜
 * @param RINGTONE_SYS  ���M������V�X�e���̃|�C���^
 */
//------------------------------------------------------------------
void RINGTONE_SYS_Delete( RINGTONE_SYS * rtone )
{
  GFL_HEAP_FreeMemory( rtone );
}

//------------------------------------------------------------------
/**
 * @brief ���M������V�X�e���̃��C������
 * @param RINGTONE_SYS  ���M������V�X�e���̃|�C���^
 */
//------------------------------------------------------------------
void RINGTONE_SYS_Main( RINGTONE_SYS * rtone )
{
  updateStatus( rtone );
}

//------------------------------------------------------------------
/**
 * @brief ���M���̊J�n
 * @param RINGTONE_SYS  ���M������V�X�e���̃|�C���^
 */
//------------------------------------------------------------------
void RINGTONE_SYS_Play( RINGTONE_SYS * rtone )
{
  changeStatus( rtone, REQ_PLAY );
}

//------------------------------------------------------------------
/**
 * @brief ���M���̒�~
 * @param RINGTONE_SYS  ���M������V�X�e���̃|�C���^
 */
//------------------------------------------------------------------
void RINGTONE_SYS_Stop( RINGTONE_SYS * rtone )
{
  changeStatus( rtone, REQ_STOP );
}

//------------------------------------------------------------------
/**
 * @brief ���M������Ή���DS�J���T�E���h�R�[���o�b�N��o�^
 */
//------------------------------------------------------------------
void RINGTONE_SYS_SetSleepSoundFunc( void )
{
  if ( ringToneSysPtr )
  {
    GFL_UI_SleepSoundGoSetFunc( ringTone_sleepSoundGoFunc, ringToneSysPtr );
    GFL_UI_SleepSoundReleaseSetFunc( ringTone_sleepSoundReleaseFunc, ringToneSysPtr );
  } else {
    //���M���V�X�e�����Ȃ��̂ɌĂяo���ꂽ�ꍇ
    GF_ASSERT( 0 );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  RINGTONE�̃X���[�v�T�E���h������g�p���Ă��邩�H
 *
 *	@retval TRUE    �g�p���Ă���
 *	@retval FALSE   �g�p���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL RINGTONE_SYS_IsSleepSoundFunc( void )
{
  if( GFL_UI_SleepSoundGoGetFunc() == ringTone_sleepSoundGoFunc ){
    GF_ASSERT( GFL_UI_SleepSoundReleaseGetFunc() == ringTone_sleepSoundReleaseFunc );
    return TRUE;
  }
  return FALSE;
}

//============================================================================================
//
//    �����g�p�֐�
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief   ���M���Đ�����
 * @param   rtone
 * @return  BOOL  TRUE�̂Ƃ��A�Ȃ葱���Ă���
 */
//------------------------------------------------------------------
static BOOL ringTone_PlayMain( RINGTONE_SYS * rtone )
{
  if ( rtone->ringWait )
  {
    rtone->ringWait --;
  }
  if ( rtone->ringWait == 0 )
  {
    if (PMSND_CheckPlaySE_byPlayerID( RINGTONE_SEPLAYER ) == TRUE )
    {
      PMSND_StopSE_byPlayerID( RINGTONE_SEPLAYER );
    } 
    return FALSE;
  }
  else
  {
    if (PMSND_CheckPlaySE_byPlayerID( RINGTONE_SEPLAYER ) == FALSE )
    {
      PMSND_PlaySE_byPlayerID( RINGTONE_SE_NO, RINGTONE_SEPLAYER );
    }
    return TRUE;
  }
}

//------------------------------------------------------------------
/**
 * @brief  �t�H�[���h��Ԃւ̈ڍs����
 */
//------------------------------------------------------------------
static void ringTone_sleepSoundGoFunc( void * work )
{
  RINGTONE_SYS * rtone = ringToneSysPtr;
  //RINGTONE_SYS * rtone = (RINGTONE_SYS *)work;
  changeStatus( rtone, REQ_CLOSE );
}

//------------------------------------------------------------------
/**
 * @brief �t�H�[���h��Ԃ���̕��A����
 */
//------------------------------------------------------------------
static void ringTone_sleepSoundReleaseFunc( void * work )
{
  RINGTONE_SYS * rtone = ringToneSysPtr;
  //RINGTONE_SYS * rtone = (RINGTONE_SYS *)work;
  changeStatus( rtone, REQ_OPEN );
}

//------------------------------------------------------------------
/**
 * @brief ���M���J�n
 */
//------------------------------------------------------------------
static void startSE( RINGTONE_SYS * rtone )
{
  if (PMSND_CheckPlaySE_byPlayerID( RINGTONE_SEPLAYER ) == FALSE )
  {
    PMSND_PlaySE_byPlayerID( RINGTONE_SE_NO, RINGTONE_SEPLAYER );
  }
  rtone->ringWait = RINGTONE_LENGTH;
}
//------------------------------------------------------------------
/**
 * @brief ���M�����~
 */
//------------------------------------------------------------------
static void stopSE( RINGTONE_SYS * rtone )
{
  PMSND_StopSE_byPlayerID( RINGTONE_SEPLAYER );
  rtone->ringWait = 0;
}
//------------------------------------------------------------------
/**
 * @brief ���̃v���C���[�̃~���[�g����
 */
//------------------------------------------------------------------
static void setMuteOther( RINGTONE_SYS * rtone, BOOL flag )
{
  if ( flag == TRUE ) {
    NNS_SndPlayerSetPlayerVolume( PLAYER_SE_SYS, MIN_VOLUME );
    NNS_SndPlayerSetPlayerVolume( PLAYER_SE_1, MIN_VOLUME );
    NNS_SndPlayerSetPlayerVolume( PLAYER_SE_2, MIN_VOLUME );
    NNS_SndPlayerSetPlayerVolume( PLAYER_SE_PSG, MIN_VOLUME );
  } else {
    NNS_SndPlayerSetPlayerVolume( PLAYER_SE_SYS, MAX_VOLUME );
    NNS_SndPlayerSetPlayerVolume( PLAYER_SE_1, MAX_VOLUME );
    NNS_SndPlayerSetPlayerVolume( PLAYER_SE_2, MAX_VOLUME );
    NNS_SndPlayerSetPlayerVolume( PLAYER_SE_PSG, MAX_VOLUME );
  }
  PLAYER_VOLUME_FADER_SetMute( rtone->fader, flag );
}

//------------------------------------------------------------------
/**
 * @brief �V�X�e���̏�ԕύX����
 * @param   rtone
 * @param   req     ��ԕύX���N�G�X�g
 */
//------------------------------------------------------------------
static void changeStatus( RINGTONE_SYS * rtone, RINGTONE_REQ req )
{
  static const u8 seqTbl[STAT_MAX][REQ_MAX] = {
    // STOP             PLAY            OPEN          CLOSE
    { STAT_OPEN_OFF,  STAT_OPEN_ON,   STAT_OPEN_OFF,  STAT_CLOSE_OFF }, //OPEN  & OFF
    { STAT_OPEN_OFF,  STAT_OPEN_ON,   STAT_OPEN_ON,   STAT_CLOSE_ON },  //OPEN  & ON
    { STAT_CLOSE_OFF, STAT_CLOSE_ON,  STAT_OPEN_OFF,  STAT_CLOSE_OFF }, //CLOSE & OFF
    { STAT_CLOSE_OFF, STAT_CLOSE_ON,  STAT_OPEN_ON,   STAT_CLOSE_ON },  //CLOSE & ON
  };

  RINGTONE_STAT new_state = seqTbl[rtone->state][req];

  //OS_TPrintf(" RINGTONE REQ(%d) STAT(%d --> %d)\n", req, rtone->state, new_state );

  switch ( new_state )
  {
  case STAT_OPEN_OFF:
    NNS_SndSetMasterVolume( MAX_VOLUME ); //MasterVol  == �L��
    setMuteOther( rtone, FALSE );         //����Player == �L��
    stopSE( rtone );
    break;

  case STAT_OPEN_ON:
    NNS_SndSetMasterVolume( MAX_VOLUME ); //MasterVol  == �L��
    setMuteOther( rtone, TRUE );          //����Player == ����
    startSE( rtone );
    break;

  case STAT_CLOSE_OFF:
    NNS_SndSetMasterVolume( MIN_VOLUME ); //MasterVol  == ����
    stopSE( rtone );
    break;

  case STAT_CLOSE_ON:
    NNS_SndSetMasterVolume( MAX_VOLUME ); //MasterVol  == �L��
    setMuteOther( rtone, TRUE );          //����Player == ����
    startSE( rtone );
    break;
  }

  rtone->state = new_state;
}

//------------------------------------------------------------------
/**
 * @brief �ʏ�X�V����
 * @param rtone
 */
//------------------------------------------------------------------
static void updateStatus( RINGTONE_SYS * rtone )
{
  BOOL flag;
  flag = ringTone_PlayMain( rtone );
  switch ( rtone->state )
  {
  case STAT_OPEN_OFF:
    break;
  case STAT_OPEN_ON:
    if ( flag == FALSE )
    {
      changeStatus( rtone, STAT_OPEN_OFF );
    }
    break;

  case STAT_CLOSE_OFF:
    break;
  case STAT_CLOSE_ON:
    if ( flag == FALSE )
    {
      changeStatus( rtone, STAT_CLOSE_OFF );
    }
    break;
  }
}






