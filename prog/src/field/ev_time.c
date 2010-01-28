//============================================================================================
/**
 * @file	ev_time.c
 * @brief	�Q�[�������Ԑ���֘A
 * @author	tamada
 * @date	2006.02.03
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

#include "poke_tool/pokerus.h"
#include "eventwork.h"

//============================================================================================
//============================================================================================

//============================================================================================
//============================================================================================
static void UpdateDayEvent(GAMEDATA * gdata, s32 diff_days);
static void UpdateMinuteEvent(GAMEDATA * gdata, s32 diff_minute, const RTCTime * now);

static void UpdateDateCheck(GAMEDATA * gdata, GMTIME * tm, const RTCDate * now_date);
static void UpdateMinuteCheck(GAMEDATA * gdata, GMTIME * tm,
		const RTCDate * now_date, const RTCTime * now_time);

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
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_SYSTEM_DATA)
	SVLD_SetCrc(GMDATA_ID_SYSTEM_DATA);
#endif //CRC_LOADCHECK	
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
#if 0
		if(diff_sec > 0){
		}
#endif
		//��
		diff_minute = diff_sec / 60;
		if (diff_minute > 0) {
			GMTIME_CountDownPenaltyTime(tm, diff_minute);
			UpdateMinuteEvent(gdata, diff_minute, now_time);

			tm->sv_date = *now_date;
			tm->sv_time = *now_time;
		}
	}
#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_SYSTEM_DATA)
	SVLD_SetCrc(GMDATA_ID_SYSTEM_DATA);
#endif //CRC_LOADCHECK	
}

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	���Ԃɂ��f�[�^�X�V�F�����P��
 * @param	gdata			�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @param	diff_days		�o�߂������ԁi�����j
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
//	TimeEventFlagClear(gdata);

	// �����̎�O���[�v�X�V
//	RandomGroup_Update(SaveData_GetRandomGroup(gdata->savedata), diff_days);

	//�G���J�E���g�֘A�����_���̎�X�V
  EncDataSave_UpdateGenerate( save );

	//�|�P���X�����̌o�߂�����
	{
		POKEPARTY *ppt;

		ppt = GAMEDATA_GetMyPokemon( gdata );
		POKERUS_DecCounter( ppt, diff_days );
	}

	//�|�P�����N�W
	{
//		SysWorkUpdatePokeLot(gdata->savedata, diff_days);
	}

  // WFBC�l���A�A�C�e��
  {
    // HEAPID_PROC�@�e���|�����Ƃ��Ďg�p
    FIELD_WFBC_CORE_CalcOneDataStart( gdata, diff_days, HEAPID_PROC );
  }

	//WIFI�ʐM�����f�[�^�̍X�V����
//	WIFIHISTORY_Update(SaveData_GetWifiHistory(gdata->savedata));

	if(!is_penalty){	//�y�i���e�B�^�C���łȂ��Ƃ�������������
	}
}

//------------------------------------------------------------------
/**
 * @brief	���Ԃɂ��f�[�^�X�V�F���P��
 * @param	gdata			�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @param	diff_minute		�o�߂������ԁi���P�ʁj
 *
 * ������GAMEDATA���Ƃ��Ă��邪�A��{�I�ɂ�SAVEDATA�݂̂�
 * �X�V�ł���悤�Ȏd�g�݂̊֐����ĂԂׂ��B
 */
//------------------------------------------------------------------
static void UpdateMinuteEvent(GAMEDATA * gdata, s32 diff_minute, const RTCTime * now)
{
	//��ɂȂ���(�o�߂���)�Ȃ�Ες�����莝���V�F�C�~�̃t�H������߂�
	{
#if 0
		POKEPARTY *ppt;

		ppt=SaveData_GetTemotiPokemon(gdata->savedata);
		PokePartySheimiNormalFormChange_TimeUpdate(ppt, diff_minute, now);
#endif
	}
}




//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	���ԑт̎擾
 * @param	gdata	�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return	int		���ԑсifield/timezone.h���Q�Ɓj
 */
//------------------------------------------------------------------
int EVTIME_GetTimeZone(const GAMEDATA * gdata)
{
	GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
	return GFL_RTC_ConvertHourToTimeZone(tm->sv_time.hour);

}

//------------------------------------------------------------------
/**
 * @brief	�C�x���g���ԁi���j�̎擾
 * @param	gdata	�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return	int		��
 */
//------------------------------------------------------------------
int EVTIME_GetMonth(const GAMEDATA * gdata)
{
	GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
	return tm->sv_date.month;
}
//------------------------------------------------------------------
/**
 * @brief	�C�x���g���ԁi���j�̎擾
 * @param	gdata	�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return	int		��
 */
//------------------------------------------------------------------
int EVTIME_GetDay(const GAMEDATA * gdata)
{
	GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
	return tm->sv_date.day;
}
//------------------------------------------------------------------
/**
 * @brief	�C�x���g���ԁi�j���j�̎擾
 * @param	gdata	�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return	int		�j��
 */
//------------------------------------------------------------------
int EVTIME_GetWeek(const GAMEDATA * gdata)
{
	GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
	return tm->sv_date.week;
}
//------------------------------------------------------------------
/**
 * @brief	�C�x���g���ԁi���j�̎擾
 * @param	gdata	�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return	int		����
 */
//------------------------------------------------------------------
int EVTIME_GetHour(const GAMEDATA * gdata)
{
	GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
	return tm->sv_time.hour;
}
//------------------------------------------------------------------
/**
 * @brief	�C�x���g���ԁi���j�̎擾
 * @param	gdata	�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return	int		��
 */
//------------------------------------------------------------------
int EVTIME_GetMinute(const GAMEDATA * gdata)
{
	GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
	return tm->sv_time.minute;
}


//------------------------------------------------------------------
/**
 * @brief	�Q�[���J�n�̓��t�����Ԏ擾
 * @param	gdata	�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @param	date	�Q�[���J�n�̓��t���󂯎�邽�߂�RTCDate�^�ւ̃|�C���^
 * @param	time	�Q�[���J�n�̎��Ԃ��󂯎�邽�߂�RTCTime�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
void EVTIME_GetGameStartDateTime(const GAMEDATA * gdata, RTCDate * date, RTCTime * time)
{
	GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
	RTC_ConvertSecondToDateTime(date, time, tm->start_sec);
}

//------------------------------------------------------------------
/**
 * @brief	�Q�[���N���A�̓��t�����Ԏ擾
 * @param	gdata	�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @param	date	�Q�[���J�n�̓��t���󂯎�邽�߂�RTCDate�^�ւ̃|�C���^
 * @param	time	�Q�[���J�n�̎��Ԃ��󂯎�邽�߂�RTCTime�^�ւ̃|�C���^
 */
//------------------------------------------------------------------
void EVTIME_GetGameClearDateTime(const GAMEDATA * gdata, RTCDate * date, RTCTime * time)
{
	GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
	RTC_ConvertSecondToDateTime(date, time, tm->clear_sec);
}

//------------------------------------------------------------------
/**
 * @brief	�Q�[���N���A�̓��t�����ԃZ�b�g
 * @param	gdata	�t�B�[���h���䃏�[�N�ւ̃|�C���^
 */
//------------------------------------------------------------------
void EVTIME_SetGameClearDateTime(const GAMEDATA * gdata)
{
	GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( (GAMEDATA*)gdata ) );
	tm->clear_sec = GFL_RTC_GetDateTimeBySecond();

#if (CRC_LOADCHECK && CRCLOADCHECK_GMDATA_ID_SYSTEM_DATA)
	SVLD_SetCrc(GMDATA_ID_SYSTEM_DATA);
#endif //CRC_LOADCHECK
}

//------------------------------------------------------------------
/**
 * @brief	DS�ݒ�ύX�̃y�i���e�B���[�h���ǂ����̔���
 * @param	gdata	�t�B�[���h���䃏�[�N�ւ̃|�C���^
 * @return	BOOL	TRUE�̂Ƃ��A�y�i���e�B��
 */
//------------------------------------------------------------------
BOOL EVTIME_IsPenaltyMode(GAMEDATA * gdata)
{
	GMTIME * tm = SaveData_GetGameTime( GAMEDATA_GetSaveControlWork( gdata ) );
	return GMTIME_IsPenaltyMode(tm);
}


