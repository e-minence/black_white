//============================================================================================
/**
 * @file	rtc_tool.c
 * @brief	RTC�̒��ŁA�Q�[���Ɉˑ��������
 * @author	tamada	GAME FREAK inc.
 * @date	2005.11.18
 */
//============================================================================================

#include <gflib.h>
#include "system/rtc_tool.h"

#include "timezone.cdat"


//============================================================================================
//
//		�c�[���֐�
//
//============================================================================================

//--------------------------------------------------------------
/**
 * @brief	���ԑт̎擾
 * @return	int	���ԑсitimezone.h���Q�Ɓj
 */
//--------------------------------------------------------------
int PM_RTC_GetTimeZone(int season)
{
	RTCTime time;
	GFL_RTC_GetTime(&time);
	return PM_RTC_ConvertHourToTimeZone(season, time.hour);
}

//--------------------------------------------------------------
/**
 * @brief	���Ԃ��玞�ԑтւ̕ϊ�
 * @param	season �G�� gamesystem/pm_season.h�Q��
 * @param	hour	���ԁi�O�|�Q�R�j
 * @return	int		���ԑсitimezone.h���Q�Ɓj
 */
//--------------------------------------------------------------
int PM_RTC_ConvertHourToTimeZone(int season, int hour)
{
  GF_ASSERT( season < PMSEASON_TOTAL );
	GF_ASSERT(0 <= hour && hour < 24);
	return sc_SEASON_TIMEZONE[season][hour];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G�߂̎��ԑѕύX�@���Ԃ��擾
 *
 *	@param	season      �G��
 *	@param	timezone    �^�C���]�[��
 *
 *	@return �ς�鎞��
 */
//-----------------------------------------------------------------------------
int PM_RTC_GetTimeZoneChangeHour( int season, TIMEZONE timezone )
{
  GF_ASSERT( season < PMSEASON_TOTAL );
  GF_ASSERT( timezone < TIMEZONE_MAX );

  return sc_SEASON_TIMEZONE_CHANGE[ season ][ timezone ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  �G�߂̎��ԑт��ς��܂ł̎��Ԃ��擾
 *
 *	@param	season    �G��
 *	@param	timezone  �^�C���]�[��
 *
 *	@return �ς��܂ł̎��ԁi-12���ԁ`11���ԁj
 */
//-----------------------------------------------------------------------------
int PM_RTC_GetTimeZoneChangeMargin( int season, TIMEZONE timezone )
{
	RTCTime time;
  int change_time;
  int time_second;
  int sub_second;
  
  GF_ASSERT( season < PMSEASON_TOTAL );
  GF_ASSERT( timezone < TIMEZONE_MAX );
  
  GFL_RTC_GetTime( &time );

  change_time = sc_SEASON_TIMEZONE_CHANGE[season][timezone];
  time_second = (time.hour * 60 * 60) + (time.minute * 60) + time.second;
  sub_second = (change_time*60*60) - time_second;
  if( sub_second > (12*60*60) )
  {
    sub_second = (24*60*60) - sub_second;
  }
  return sub_second;
}

//--------------------------------------------------------------
/**
 * @brief	�邩�ǂ����̔���
 * @retval	TRUE	���͖�
 * @retval	FALSE	���͒�
 */
//--------------------------------------------------------------
BOOL PM_RTC_IsNightTime( int season )
{
	switch ( PM_RTC_GetTimeZone(season) ) {
	case TIMEZONE_MIDNIGHT:
	case TIMEZONE_NIGHT:
		return TRUE;
	}
	return FALSE;
}


#if 0
//--------------------------------------------------------------
/**
 * @brief	���Ԃ��玞�ԑтւ̕ϊ�
 * @param	hour	���ԁi�O�|�Q�R�j
 * @return	int		���ԑсitimezone.h���Q�Ɓj
 */
//--------------------------------------------------------------
int GFL_RTC_ConvertHourToTimeZone(int hour)
{
  return PM_RTC_ConvertHourToTimeZone( PM_RTC_TIMEZONE_DEFAULT_SEASON, hour );
}
#endif


//--------------------------------------------------------------
/**
 * @brief	�o�ߎ��Ԃ̌v�Z
 * @param	start_sec
 * @param	end_sec
 */
//--------------------------------------------------------------
s64 GFL_RTC_GetPassTime(s64 start_sec, s64 end_sec)
{
	/** RTC���ێ�����ő�̎��Ԃ�b���ɕϊ������l */
	enum{
		RTC_MAX_SECOND = 3155759999,
	};

	RTCDate date = { 99, 12, 31, 0};
	RTCTime time = { 23, 59, 59 };
	s64 MaxSec = RTC_ConvertDateTimeToSecond(&date, &time);
	GF_ASSERT(MaxSec == RTC_MAX_SECOND);

	if (start_sec < end_sec) {
		return end_sec - start_sec;
	}
	return end_sec + (RTC_MAX_SECOND - start_sec);
}

