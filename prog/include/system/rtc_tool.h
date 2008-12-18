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
#include "system/timezone.h"

//--------------------------------------------------------------
//	�c�[���֐�
//--------------------------------------------------------------
extern int GF_RTC_ConvertHourToTimeZone(int hour);
extern int GF_RTC_GetTimeZone(void);
extern int GF_RTC_GetDaysOffset(const RTCDate * date);
//�邩�ǂ����`�F�b�N
extern BOOL GF_RTC_IsNightTime(void);

static inline BOOL GF_RTC_IsLeapYear(u32 year)
{
	if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0) {
		return TRUE;
	} else {
		return FALSE;
	}
}

#endif	//WB_SYSTEM_RTC_TOOL_H__
