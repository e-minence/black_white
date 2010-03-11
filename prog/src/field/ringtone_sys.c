//============================================================================================
/**
 * @file  ringtone_sys.c
 * @date  2010.03.11
 * @author  tamada GAME FREAK inc.
 * @brief   TV�g�����V�[�o�[���M������
 */
//============================================================================================

#include <gflib.h>
#include "ringtone_sys.h"

#include "sound/pm_sndsys.h"
#include "player_volume_fader.h"

#define USE_LOOP_SE

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief ���M������V�X�e�����[�N��`
 */
//------------------------------------------------------------------
struct _RINGTONE_SYS{
  BOOL ring_flag;     ///<���M�������Ă��邩�ǂ���
  BOOL isFoldMode;    ///<DS����Ă��邩�ǂ���

  u16  ringtone_status;
  u16  call_count;

  PLAYER_VOLUME_FADER * fader;  ///<NNS�̃v���C���[�{�����[������V�X�e���ւ̃|�C���^

};
//------------------------------------------------------------------
//------------------------------------------------------------------
enum {
  //RINGTONE_SE_NO = SEQ_SE_OPEN1,
  RINGTONE_SE_NO = SEQ_SE_SYS_35, 
  //RINGTONE_SE_NO = SEQ_SE_SYS_85, 
  RINGTONE_SEPLAYER = SEPLAYER_SE3,

  RINGTONE_LENGTH = 60 * 15,
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
//------------------------------------------------------------------
RINGTONE_SYS * RINGTONE_SYS_Create( HEAPID heapID, PLAYER_VOLUME_FADER * fader )
{
  RINGTONE_SYS * rtone = GFL_HEAP_AllocClearMemory( heapID, sizeof(RINGTONE_SYS) );
  rtone->ring_flag = FALSE;
  rtone->isFoldMode = FALSE;
  rtone->ringtone_status = 0;
  rtone->call_count = 0;
  rtone->fader = fader;

  //���M������Ή���DS�J���T�E���h�R�[���o�b�N��o�^
  GFL_UI_SleepSoundGoSetFunc( ringTone_sleepSoundGoFunc, rtone );
  GFL_UI_SleepSoundReleaseSetFunc( ringTone_sleepSoundReleaseFunc, rtone );

  ringToneSysPtr = rtone;
  return rtone;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void RINGTONE_SYS_Delete( RINGTONE_SYS * rtone )
{
  GFL_HEAP_FreeMemory( rtone );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void RINGTONE_SYS_Main( RINGTONE_SYS * rtone )
{
  if ( rtone->ring_flag == TRUE )
  {
    BOOL flag = ringTone_PlayMain( rtone );
    if ( flag == FALSE )
    {
      rtone->ring_flag = FALSE;
      //�W����Ă��Ȃ��ꍇ��PlayerVolume�̑�����s��
      if (rtone->isFoldMode == FALSE)
      {
        PLAYER_VOLUME_FADER_SetMasterVolume( rtone->fader, 127 );
      }
    }
  }
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void RINGTONE_SYS_RequestSE( RINGTONE_SYS * rtone )
{
  if ( rtone->ring_flag == FALSE )
  {
    PMSND_PlaySE_byPlayerID( RINGTONE_SE_NO, RINGTONE_SEPLAYER );
    rtone->ring_flag = TRUE;
#ifdef  USE_LOOP_SE
    rtone->ringtone_status = 0;
    rtone->call_count = RINGTONE_LENGTH;
#endif
    //�W����Ă��Ȃ��ꍇ��PlayerVolume�̑�����s��
    if (rtone->isFoldMode == FALSE)
    {
      PLAYER_VOLUME_FADER_SetMasterVolume( rtone->fader, 0 );
    }
  }
}


//------------------------------------------------------------------
//------------------------------------------------------------------
static BOOL ringTone_PlayMain( RINGTONE_SYS * rtone )
{
#ifdef  USE_LOOP_SE
  rtone->call_count --;
  if ( rtone->call_count == 0 )
  {
    if (PMSND_CheckPlaySE_byPlayerID( RINGTONE_SEPLAYER ) == TRUE )
    {
      PMSND_StopSE_byPlayerID( RINGTONE_SEPLAYER );
    } 
    return FALSE;
  } else {
    if (PMSND_CheckPlaySE_byPlayerID( RINGTONE_SEPLAYER ) == FALSE )
    {
      PMSND_PlaySE_byPlayerID( RINGTONE_SE_NO, RINGTONE_SEPLAYER );
      rtone->ringtone_status ++;
    }
    return TRUE;
  }
#else
  return PMSND_CheckPlaySE_byPlayerID( RINGTONE_SEPLAYER );
#endif
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void ringTone_sleepSoundGoFunc( void * work )
{
  RINGTONE_SYS * rtone = ringToneSysPtr;
  //RINGTONE_SYS * rtone = (RINGTONE_SYS *)work;
  PLAYER_VOLUME_FADER_SetMasterVolume( rtone->fader, 0 );
  rtone->isFoldMode = TRUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void ringTone_sleepSoundReleaseFunc( void * work )
{
  RINGTONE_SYS * rtone = ringToneSysPtr;
  //RINGTONE_SYS * rtone = (RINGTONE_SYS *)work;
  if ( rtone->ring_flag == FALSE )
  { //���Ă��Ȃ��ꍇ�AVolume�����Ƃɖ߂�
    PLAYER_VOLUME_FADER_SetMasterVolume( rtone->fader, 127 );
  }
  rtone->isFoldMode = FALSE;
}

