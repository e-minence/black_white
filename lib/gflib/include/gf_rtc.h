//============================================================================================
/**
 * @file	gf_rtc.h
 * @brief	RTC�A�N�Z�X���b�p�[�p�w�b�_
 * @author	tamada	GAME FREAK inc.
 * @date	2005.11.21
 *
 * ���ڂ�RTC���C�u�����g�p�͋֎~����B
 * ���̊֐��o�R�ł̃A�N�Z�X���s���B
 */
//============================================================================================
#ifndef	GFL_GF_STANDARD_GF_RTC_H__  // �C���N���[�h�K�[�h
#define	GFL_GF_STANDARD_GF_RTC_H__

#include <nitro.h>
#include <nnsys.h>

//--------------------------------------------------------------
//  �萔
//--------------------------------------------------------------
#define GFL_RTC_TIME_SECOND_MAX   (86399)   //GFL_RTC_GetTimeBySecond�֐��œ�����l�̍ő�l

//--------------------------------------------------------------
//	�����������C��
//--------------------------------------------------------------
extern void GFL_RTC_Init(void);
extern void GFL_RTC_Main(void);

//--------------------------------------------------------------
//	�擾�p�֐�
//--------------------------------------------------------------
extern void GFL_RTC_GetDateTime(RTCDate * date, RTCTime * time);
extern void GFL_RTC_GetTime(RTCTime * time);
extern void GFL_RTC_GetDate(RTCDate * date);
extern int GFL_RTC_GetTimeBySecond(void);
extern s64 GFL_RTC_GetDateTimeBySecond(void);

//�o�ߎ��Ԃ̎擾
extern s64 GFL_RTC_GetPassTime(s64 start_sec, s64 end_sec);
extern int GFL_RTC_GetDaysOffset(const RTCDate * date);

#ifdef	PM_DEBUG
//--------------------------------------------------------------
//	�f�o�b�O�p�֐�
//--------------------------------------------------------------
extern void GFL_RTC_DEBUG_StartFakeTime(int rate);
extern void GFL_RTC_DEBUG_StopFakeTime(void);
extern void GFL_RTC_DEBUG_StartFakeFixTime(int hour, int minute);
extern void GFL_RTC_DEBUG_SetDate( const RTCDate * date );

#endif

#endif	//GFL_GF_STANDARD_GF_RTC_H__
