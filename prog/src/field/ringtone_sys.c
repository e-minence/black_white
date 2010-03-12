//============================================================================================
/**
 * @file  ringtone_sys.c
 * @date  2010.03.11
 * @author  tamada GAME FREAK inc.
 * @brief   TV�g�����V�[�o�[���M������
 *
 * ���M���V�X�e���̓t�B�[���h�T�E���h�V�X�e���A�v���C���[�{�����[������V�X�e����
 * �ˑ����Ă���A���ڌĂяo����邱�Ƃ͂Ȃ��B
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
 * @brief ���M������V�X�e�����[�N��`
 */
//------------------------------------------------------------------
struct _RINGTONE_SYS{
  BOOL ringFlag;       ///<���M�������Ă��邩�ǂ���
  BOOL isFoldMode;     ///<DS����Ă��邩�ǂ���

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

//============================================================================================
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
  rtone->ringFlag = FALSE;
  rtone->isFoldMode = FALSE;
  rtone->ringWait = 0;
  rtone->fader = fader;

  //���M������Ή���DS�J���T�E���h�R�[���o�b�N��o�^
  GFL_UI_SleepSoundGoSetFunc( ringTone_sleepSoundGoFunc, rtone );
  GFL_UI_SleepSoundReleaseSetFunc( ringTone_sleepSoundReleaseFunc, rtone );

  ringToneSysPtr = rtone;
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
  if ( rtone->ringFlag == TRUE )
  {
    BOOL flag = ringTone_PlayMain( rtone );
    if ( flag == FALSE )
    {
      rtone->ringFlag = FALSE;
      //�W����Ă��Ȃ��ꍇ��PlayerVolume�̑�����s��
      if (rtone->isFoldMode == FALSE)
      {
        PLAYER_VOLUME_FADER_SetMasterVolume( rtone->fader, MAX_VOLUME );
      }
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief ���M���̊J�n
 * @param RINGTONE_SYS  ���M������V�X�e���̃|�C���^
 */
//------------------------------------------------------------------
void RINGTONE_SYS_Play( RINGTONE_SYS * rtone )
{
  if ( rtone->ringFlag == FALSE )
  {
    PMSND_PlaySE_byPlayerID( RINGTONE_SE_NO, RINGTONE_SEPLAYER );
    rtone->ringFlag = TRUE;
    rtone->ringWait = RINGTONE_LENGTH;
    //�W����Ă��Ȃ��ꍇ��PlayerVolume�̑�����s��
    if (rtone->isFoldMode == FALSE)
    {
      PLAYER_VOLUME_FADER_SetMasterVolume( rtone->fader, MIN_VOLUME );
    }
  }
}

//------------------------------------------------------------------
/**
 * @brief ���M���̒�~
 * @param RINGTONE_SYS  ���M������V�X�e���̃|�C���^
 */
//------------------------------------------------------------------
void RINGTONE_SYS_Stop( RINGTONE_SYS * rtone )
{
  if ( rtone->ringFlag == TRUE )
  {
    PMSND_StopSE_byPlayerID( RINGTONE_SEPLAYER );
    rtone->ringFlag = FALSE;
    rtone->ringWait = 0;
    //�W����Ă��Ȃ��ꍇ��PlayerVolume�̑�����s��
    if (rtone->isFoldMode == FALSE)
    {
      PLAYER_VOLUME_FADER_SetMasterVolume( rtone->fader, MAX_VOLUME );
    }
  }
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL ringTone_PlayMain( RINGTONE_SYS * rtone )
{
  rtone->ringWait --;
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
  //���M���ȊO�̃v���C���[�̃{�����[�����O�ɂ���
  PLAYER_VOLUME_FADER_SetMasterVolume( rtone->fader, MIN_VOLUME );
  rtone->isFoldMode = TRUE;
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
  if ( rtone->ringFlag == FALSE )
  { //���Ă��Ȃ��ꍇ�AVolume�����Ƃɖ߂�
    PLAYER_VOLUME_FADER_SetMasterVolume( rtone->fader, MAX_VOLUME );
  }
  rtone->isFoldMode = FALSE;
}

