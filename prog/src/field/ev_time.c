//============================================================================================
/**
 * @file  ev_time.c
 * @brief �Q�[�������Ԑ���֘A
 * @author  tamada
 * @date  2006.02.03
 */
//============================================================================================
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_event.h"
#include "system/main.h"    //HEAPID_FIELDMAP
#include "system/rtc_tool.h"    //HEAPID_FIELDMAP

#include "field/zonedata.h"
#include "fieldmap/zone_id.h"

#include "fieldmap.h"
#include "event_debug_menu.h"     //DEBUG_EVENT_DebugMenu
#include "script.h"     //SCRIPT_SetEventScript
#include "isdbglib.h"
#include "map_attr.h"

#include "../../../resource/fldmapdata/flagwork/flag_define.h"

#include "savedata/encount_sv.h"
#include "field_encount.h"      //FIELD_ENCOUNT_CheckEncount
#include "effect_encount.h"

#include "debug/debug_flg.h" //DEBUG_FLG_�`
#include "ev_time.h"

#include "savedata/gametime.h"
#include "savedata/encount_sv.h"
#include "savedata/wifihistory.h"

#include "poke_tool/pokerus.h"
#include "field/eventwork.h"

#include "sheimi_normalform.h"

#include "savedata/irc_compatible_savedata.h"

//============================================================================================
//============================================================================================
//============================================================================================
//============================================================================================
static void UpdateDayEvent(GAMEDATA * gdata, s32 diff_days);
static void UpdateMinuteEvent(GAMEDATA * gdata, s32 diff_minute, const RTCTime * now);

static BOOL changeTimeCalc( GMTIME * tm,
    const RTCDate * now_date, const RTCTime * now_time, s32 * diff_day, s32 * diff_minute );
#if 0
static void UpdateDateCheck(GAMEDATA * gdata, GMTIME * tm, const RTCDate * now_date);
static void UpdateMinuteCheck(GAMEDATA * gdata, GMTIME * tm,
    const RTCDate * now_date, const RTCTime * now_time);
#endif

//============================================================================================
//============================================================================================
#ifdef  DEBUG_ONLY_FOR_tamada
static void DEBUG_DATE_Print( const RTCDate * date, const RTCTime * time )
{
  TAMADA_Printf("20%02d %02d/%02d(%d) %02d:%02d:%02d\n",
      date->year, date->month, date->day, date->week,
      time->hour, time->minute, time->second );
}

static void DEBUG_All_Printf( GMTIME * tm, const RTCDate * now_date, const RTCTime * now_time )
{
  TAMADA_Printf("SAVE (%04d) ", RTC_ConvertDateToDay(&tm->sv_date) );
  DEBUG_DATE_Print( &tm->sv_date, &tm->sv_time );
  TAMADA_Printf("NOW  (%04d) ", RTC_ConvertDateToDay(now_date) );
  DEBUG_DATE_Print( now_date, now_time );
}
#else
#define DEBUG_All_Printf(...)   ((void)0)
#endif  //DEBUG_ONLY_FOR_tamada
//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void EVTIME_Update(GAMEDATA * gdata)
{
  RTCDate now_date;
  RTCTime now_time;
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( gdata ) );
  s32 diff_day, diff_minute;

  if (tm->use_flag == FALSE) {
    return;
  }

  GFL_RTC_GetDateTime(&now_date, &now_time);

  if ( changeTimeCalc( tm, &now_date, &now_time, &diff_day, &diff_minute ) == FALSE )
  { //���Ԍo�߂Ɉُ킪�������ꍇ�̓X���[
    return;
  }
  if ( diff_minute == 0 && diff_day != 0 )
  { //�����i��ł��Ȃ��̂ɓ����i�񂾂�A�T�[�g
    GF_ASSERT( 0 );
    return;
  }

  if ( diff_minute )
  {
    GMTIME_CountDownPenaltyTime( tm, diff_minute );
    UpdateMinuteEvent( gdata, diff_minute, &now_time );
  }
  if ( diff_day )
  {
    UpdateDayEvent( gdata, diff_day ); 
  }
}

//------------------------------------------------------------------
/**
 * @brief �o�ߎ��Ԃ̎Z�o����
 * @param tm            ���Ԍo�ߊǗ����[�N
 * @param now_date      ���݂̓��t
 * @param now_time      ���݂̎���
 * @param diff_day      �ߋ����猻�݂ւ̌o�ߎ��ԁi���P�ʁj
 * @param diff_minute   �ߋ����猻�݂ւ̌o�ߎ��ԁi���P�ʁj
 * @return  BOOL  ����擾���ꂽ���ǂ����iTRUE=����AFALSE=�ُ͈�Ȃ̂ōX�V���������s���Ȃ��j
 *
 * @todo  now_time�̕b���O�ɐ؂�̂Ăĉ��Z���邱�Ƃ�RTC�Ǝ��ԍX�V�̌덷��
 * ����ȊO�P�b�����ɂ��邱�Ƃ��\�i�����ǌ��؎��Ԃ�����Ȃ��̂ł��Ȃ��j
 */
//------------------------------------------------------------------
static BOOL changeTimeCalc( GMTIME * tm,
    const RTCDate * now_date, const RTCTime * now_time, s32 * diff_day, s32 * diff_minute )
{
  s64  sv_sec = RTC_ConvertDateTimeToSecond( &tm->sv_date, &tm->sv_time );
  s64 now_sec = RTC_ConvertDateTimeToSecond( now_date, now_time );
  s32 now_day = RTC_ConvertDateToDay( now_date );
  s32 diff;

  *diff_day = 0;
  *diff_minute = 0;

  if ( now_sec < sv_sec ) {
    //���݁@���@�ߋ��@�c���肦�Ȃ��͂�
    //���ݎ��Ԃ��Z�b�g���邾���Ŗ߂�
    DEBUG_All_Printf( tm, now_date, now_time );
    TAMADA_Printf(" Reverse Time!!\n" );
    tm->sv_date = *now_date;
    tm->sv_time = *now_time;
    tm->sv_day  = now_day;
    return FALSE;
  }

  diff = ( now_sec - sv_sec ) / 60;
  if (diff > 0) {
    //���̍X�V���������ꍇ
    *diff_minute = diff;
    DEBUG_All_Printf( tm, now_date, now_time );
    TAMADA_Printf(" Update Minute %d\n", *diff_minute );
    tm->sv_date = *now_date;
    tm->sv_time = *now_time;

    //���̍X�V���������������A���t�̍X�V�`�F�b�N���s��
    if ( now_day - tm->sv_day > 0 )
    {
      *diff_day = now_day - tm->sv_day;
      tm->sv_day = now_day;
      TAMADA_Printf(" Update Day %d\n", *diff_day );
    }
  }
  return TRUE;
}

#if 0
//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
void EVTIME_Update(GAMEDATA * gdata)
{
  RTCDate now_date;
  RTCTime now_time;
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( gdata ) );
  s32 diff_day, diff_minute;

  if (tm->use_flag == FALSE) {
    return;
  }

  GFL_RTC_GetDateTime(&now_date, &now_time);

  UpdateDateCheck(gdata, tm, &now_date);
  UpdateMinuteCheck(gdata, tm, &now_date, &now_time);
}

//------------------------------------------------------------------
///�����o�߂�����
//------------------------------------------------------------------
static void UpdateDateCheck(GAMEDATA * gdata, GMTIME * tm, const RTCDate * now_date)
{
  s32 now_day;
  now_day = RTC_ConvertDateToDay(now_date);

  if (now_day < tm->sv_day) {
    //���݁@���@�ߋ��@�c���肦�Ȃ��͂�
    //���ݎ��Ԃ��Z�b�g���邾���Ŗ߂�
    tm->sv_day = now_day;
  }
  else if (now_day > tm->sv_day) {
    UpdateDayEvent(gdata, now_day - tm->sv_day);
    tm->sv_day = now_day;
  }
}

//------------------------------------------------------------------
///�b���o�߂�����
//------------------------------------------------------------------
static void UpdateMinuteCheck(GAMEDATA * gdata, GMTIME * tm,
    const RTCDate * now_date, const RTCTime * now_time)
{
  s64 now, sv;
  s32 diff_sec,diff_minute;
  now = RTC_ConvertDateTimeToSecond(now_date, now_time);
  sv = RTC_ConvertDateTimeToSecond(&tm->sv_date, &tm->sv_time);
  if (now < sv) {
    //���݁@���@�ߋ��@�c���肦�Ȃ��͂�
    //���ݎ��Ԃ��Z�b�g���邾���Ŗ߂�
    tm->sv_date = *now_date;
    tm->sv_time = *now_time;
  }
  else{
    //�b
    diff_sec = (now-sv);
    //��
    diff_minute = diff_sec / 60;
    if (diff_minute > 0) {
      GMTIME_CountDownPenaltyTime(tm, diff_minute);
      UpdateMinuteEvent(gdata, diff_minute, now_time);

      tm->sv_date = *now_date;
      tm->sv_time = *now_time;
    }
  }
}
#endif

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief ���Ԃɂ��f�[�^�X�V�F�����P��
 * @param gdata     �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @param diff_days   �o�߂������ԁi�����j
 *
 * ������GAMEDATA���Ƃ��Ă��邪�A��{�I�ɂ�SAVEDATA�݂̂�
 * �X�V�ł���悤�Ȏd�g�݂̊֐����ĂԂׂ��B
 */
//------------------------------------------------------------------
static void UpdateDayEvent(GAMEDATA * gdata, s32 diff_days)
{
  BOOL is_penalty = EVTIME_IsPenaltyMode(gdata);
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata );

  TAMADA_Printf("Update Day Event\n");
  //���L�̂悤�Ȋ����Ŏ��Ԋ֘A�C�x���g�X�V������ǉ�����
  //
  //TVData_Update(gdata, diff_days);
  //ZukanHyouka_Update
  //�c

  //1�����ɃN���A����t���O
  EVENTWORK_ClearTimeFlags( GAMEDATA_GetEventWork( gdata ) );
//  TimeEventFlagClear(gdata);

  // �����̎�O���[�v�X�V
//  RandomGroup_Update(SaveData_GetRandomGroup(gdata->savedata), diff_days);

  //�G���J�E���g�֘A�����_���̎�X�V
  EncDataSave_UpdateGenerate( save );

  //�|�P���X�����̌o�߂�����
  {
    POKEPARTY *ppt;

    ppt = GAMEDATA_GetMyPokemon( gdata );
    POKERUS_DecCounter( ppt, diff_days );
  }

  // ���R�[�h�f�[�^1��1��X�V����
  RECORD_1day_Update( GAMEDATA_GetRecordPtr(gdata) );

  // �W�I�l�b�g1��1��X�V����
  WIFIHISTORY_Update(SaveData_GetWifiHistory(save));

  //�|�P�����N�W
  {
//    SysWorkUpdatePokeLot(gdata->savedata, diff_days);
  }

  // WFBC�l���A�A�C�e��
  {
    // HEAPID_PROC�@�e���|�����Ƃ��Ďg�p
    FIELD_WFBC_CORE_CalcOneDataStart( gdata, diff_days, HEAPID_PROC );
  }

  //WIFI�ʐM�����f�[�^�̍X�V����
//  WIFIHISTORY_Update(SaveData_GetWifiHistory(gdata->savedata));

  //�������[�_�[�F�{���̂���Ⴂ�l�������v�Ɉړ�
  QuestionnaireWork_DateChangeUpdate( SaveData_GetQuestionnaire(save) );

  //�t�B�[�����O�`�F�b�N�F�P���P��`�F�b�N������
  IRC_COMPATIBLE_SV_ClearDayFlag( IRC_COMPATIBLE_SV_GetSavedata(save) );
  
  if(!is_penalty){  //�y�i���e�B�^�C���łȂ��Ƃ�������������
  }
}

//------------------------------------------------------------------
/**
 * @brief ���Ԃɂ��f�[�^�X�V�F���P��
 * @param gdata     �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @param diff_minute   �o�߂������ԁi���P�ʁj
 *
 * ������GAMEDATA���Ƃ��Ă��邪�A��{�I�ɂ�SAVEDATA�݂̂�
 * �X�V�ł���悤�Ȏd�g�݂̊֐����ĂԂׂ��B
 */
//------------------------------------------------------------------
static void UpdateMinuteEvent(GAMEDATA * gdata, s32 diff_minute, const RTCTime * now)
{  
  //��ɂȂ���(�o�߂���)�Ȃ�Ες�����莝���V�F�C�~�̃t�H������߂�
  {
    POKEPARTY *ppt;
    int season;
    SAVE_CONTROL_WORK* sv = GAMEDATA_GetSaveControlWork(gdata);
    ppt = SaveData_GetTemotiPokemon(sv);
    season = GAMEDATA_GetSeasonID(gdata);
    SHEIMI_NFORM_ChangeNormal_TimeUpdate(gdata, ppt, diff_minute, now, season);
  }  

  // �d���f���̃`�����s�I���j���[�X�\���c�莞�Ԃ��f�N�������g
  {
    SAVE_CONTROL_WORK* sv = GAMEDATA_GetSaveControlWork(gdata);
    MISC* misc = SaveData_GetMisc( sv );
    MISC_DecChampNewsMinutes( misc, diff_minute );
  }
}




//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief   �C�x���g���ԁFRTCDate�̎擾
 * @param gdata �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return  RTCDate�ւ̃|�C���^
 */
//------------------------------------------------------------------
const RTCDate * EVTIME_GetRTCDate( const GAMEDATA * gdata )
{
  const GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
  return &tm->sv_date;
}

//------------------------------------------------------------------
/**
 * @brief ���ԑт̎擾
 * @param gdata �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return  int   ���ԑсifield/timezone.h���Q�Ɓj
 *
 *�f�t�H���g�G�߂̎��ԑт��擾���܂��B
 * 
 * EVTIME_GetTimeZone���g�p���邩�A
 * EVTIME_GetSeasonTimeZone���g�p���邩�́A���҂Ƒ��k���Ă��������B
 */
//------------------------------------------------------------------
int EVTIME_GetTimeZone(const GAMEDATA * gdata)
{
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
  return PM_RTC_ConvertHourToTimeZone( PM_RTC_TIMEZONE_DEFAULT_SEASON, tm->sv_time.hour);
}

//------------------------------------------------------------------
/**
 * @brief ���ԑт̎擾
 * @param gdata �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return  int   ���ԑсifield/timezone.h���Q�Ɓj
 *
 * �G�߂̎��ԑтɂȂ�܂��B
 *
 * EVTIME_GetTimeZone���g�p���邩�A
 * EVTIME_GetSeasonTimeZone���g�p���邩�́A���҂Ƒ��k���Ă��������B
 */
//------------------------------------------------------------------
int EVTIME_GetSeasonTimeZone(const GAMEDATA * gdata)
{
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
  u16 season = GAMEDATA_GetSeasonID( gdata );
  return PM_RTC_ConvertHourToTimeZone( season, tm->sv_time.hour);

}

//------------------------------------------------------------------
/**
 * @brief �C�x���g���ԁi���j�̎擾
 * @param gdata �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return  int   ��
 */
//------------------------------------------------------------------
int EVTIME_GetMonth(const GAMEDATA * gdata)
{
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
  return tm->sv_date.month;
}
//------------------------------------------------------------------
/**
 * @brief �C�x���g���ԁi���j�̎擾
 * @param gdata �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return  int   ��
 */
//------------------------------------------------------------------
int EVTIME_GetDay(const GAMEDATA * gdata)
{
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
  return tm->sv_date.day;
}
//------------------------------------------------------------------
/**
 * @brief �C�x���g���ԁi�j���j�̎擾
 * @param gdata �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return  int   �j��
 */
//------------------------------------------------------------------
int EVTIME_GetWeek(const GAMEDATA * gdata)
{
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
  return tm->sv_date.week;
}
//------------------------------------------------------------------
/**
 * @brief �C�x���g���ԁi���j�̎擾
 * @param gdata �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return  int   ����
 */
//------------------------------------------------------------------
int EVTIME_GetHour(const GAMEDATA * gdata)
{
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
  return tm->sv_time.hour;
}
//------------------------------------------------------------------
/**
 * @brief �C�x���g���ԁi���j�̎擾
 * @param gdata �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return  int   ��
 */
//------------------------------------------------------------------
int EVTIME_GetMinute(const GAMEDATA * gdata)
{
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
  return tm->sv_time.minute;
}


//------------------------------------------------------------------
/**
 * @brief �Q�[���J�n�̓��t�����Ԏ擾
 * @param gdata �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @param date  �Q�[���J�n�̓��t���󂯎�邽�߂�RTCDate�^�ւ̃|�C���^
 * @param time  �Q�[���J�n�̎��Ԃ��󂯎�邽�߂�RTCTime�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
void EVTIME_GetGameStartDateTime(const GAMEDATA * gdata, RTCDate * date, RTCTime * time)
{
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
  RTC_ConvertSecondToDateTime(date, time, tm->start_sec);
}

//------------------------------------------------------------------
/**
 * @brief �Q�[���N���A�̓��t�����Ԏ擾
 * @param gdata �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @param date  �Q�[���J�n�̓��t���󂯎�邽�߂�RTCDate�^�ւ̃|�C���^
 * @param time  �Q�[���J�n�̎��Ԃ��󂯎�邽�߂�RTCTime�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
void EVTIME_GetGameClearDateTime(const GAMEDATA * gdata, RTCDate * date, RTCTime * time)
{
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
  RTC_ConvertSecondToDateTime(date, time, tm->clear_sec);
}

//------------------------------------------------------------------
/**
 * @brief �Q�[���N���A�̓��t�����ԃZ�b�g
 * @param gdata �t�B�[���h���䃏�[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
void EVTIME_SetGameClearDateTime(const GAMEDATA * gdata)
{
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
  tm->clear_sec = GFL_RTC_GetDateTimeBySecond();

}

//------------------------------------------------------------------
/**
 * @brief DS�ݒ�ύX�̃y�i���e�B���[�h���ǂ����̔���
 * @param gdata �t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return  BOOL  TRUE�̂Ƃ��A�y�i���e�B��
 */
//------------------------------------------------------------------
BOOL EVTIME_IsPenaltyMode(GAMEDATA * gdata)
{
  GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( gdata ) );
  return GMTIME_IsPenaltyMode(tm);
}


