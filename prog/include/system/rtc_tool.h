//============================================================================================
/**
 * @file	rtc_tool.h
 * @brief	RTC�̒��ŁA�Q�[���Ɉˑ��������
 * @author	tamada	GAME FREAK inc.
 * @date	2005.11.21
 *
 * ���ڂ�RTC���C�u�����g�p�͋֎~����B
 * ���̊֐��o�R�ł̃A�N�Z�X���s���B
 */
//============================================================================================
#ifndef	WB_SYSTEM_RTC_TOOL_H__  // �C���N���[�h�K�[�h
#define	WB_SYSTEM_RTC_TOOL_H__

#include <gflib.h>
#include "gamesystem/pm_season.h"
#include "system/timezone.h"

//--------------------------------------------------------------
//	���ԑъ֐�
//--------------------------------------------------------------
// �G�߁@���ԑ�
// �G�߂��Ƃɕς�鎞�ԑтł��B
// ���f�ӏ���
//  �t�B�[���h���C�g
//  �z�u���f�����ԑуA�j��
extern int PM_RTC_GetTimeZone(int season);
extern int PM_RTC_ConvertHourToTimeZone(int season, int hour);
extern int PM_RTC_GetTimeZoneChangeHour( int season, TIMEZONE timezone );
extern int PM_RTC_GetTimeZoneChangeMargin( int season, TIMEZONE timezone );

// Default ���ԑ�  
#define PM_RTC_TIMEZONE_DEFAULT_SEASON  ( PMSEASON_SUMMER )
//�邩�ǂ����`�F�b�N
extern BOOL PM_RTC_IsNightTime( int season );

/// @todo ��L�ɒu�����������p�~����
static inline BOOL GFL_RTC_IsNightTime( void )
{
  return PM_RTC_IsNightTime( PM_RTC_TIMEZONE_DEFAULT_SEASON );
}

//extern int GFL_RTC_ConvertHourToTimeZone(int hour);

//--------------------------------------------------------------
//	�c�[���֐�
//--------------------------------------------------------------
extern int GFL_RTC_GetDaysOffset(const RTCDate * date);


static inline BOOL GFL_RTC_IsLeapYear(u32 year)
{
	if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

#endif	//WB_SYSTEM_RTC_TOOL_H__
