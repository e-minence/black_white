//============================================================================================
/**
 * @brief	playtime.h
 * @brief	�v���C����
 * @date	2006.03.09
 * @author	tamada
 */
//============================================================================================

#ifndef	__PLAYTIME_H__
#define	__PLAYTIME_H__

#include "savedata/save_control.h"

//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief	�v���C���Ԃ̕s���S�^��`
 */
//---------------------------------------------------------------------------
typedef struct PLAYTIME PLAYTIME;

///�Z�[�u��������
typedef struct{
	u32	save_year:7;			// �Z�[�u���̓��t�i�N�j
	u32	save_month:4;			// �Z�[�u���̓��t�i���j
	u32	save_day:5;				// �Z�[�u���̓��t�i���j
	u32	save_hour:5;			// �Z�[�u���̎��ԁi���j
	u32	save_minute:6;		// �Z�[�u���̎��ԁi���j
	u32	dmy:5;						// ���܂�
}PLAYTIME_SAVE_DATE;

//---------------------------------------------------------------------------
//		�v���C���Ԃ̍ő�l�̒�`
//---------------------------------------------------------------------------
#define	PTIME_HOUR_MAX		(999)			///<�v���C���ԁi���j�̍ő�l
#define	PTIME_MINUTE_MAX	(59)			///<�v���C���ԁi���j�̍ő�l
#define	PTIME_SECOND_MAX	(59)			///<�v���C���ԁi�b�j�̍ő�l

//============================================================================================
//============================================================================================

extern PLAYTIME * SaveData_GetPlayTime(SAVE_CONTROL_WORK * sv);

extern PLAYTIME * PLAYTIME_Allock( HEAPID heapID );
extern void PLAYTIME_Init(PLAYTIME * coin);
extern void PLAYTIME_CountUp(PLAYTIME * ptime, u32 value);
extern void PLAYTIME_SetAllData( PLAYTIME * ptime, const PLAYTIME * data );
extern u16 PLAYTIME_GetHour(const PLAYTIME * ptime);
extern u8 PLAYTIME_GetMinute(const PLAYTIME * ptime);
extern u8 PLAYTIME_GetSecond(const PLAYTIME * ptime);

extern void PLAYTIME_Set(PLAYTIME * ptime, u32 hour, u32 min, u32 sec);

extern void PLAYTIME_SetSaveTime( PLAYTIME * ptime );
extern u32 PLAYTIME_GetSaveYear( const PLAYTIME * ptime );
extern u32 PLAYTIME_GetSaveMonth( const PLAYTIME * ptime );
extern u32 PLAYTIME_GetSaveDay( const PLAYTIME * ptime );
extern u32 PLAYTIME_GetSaveHour( const PLAYTIME * ptime );
extern u32 PLAYTIME_GetSaveMinute( const PLAYTIME * ptime );
extern void PLAYTIME_GetSaveDate( const PLAYTIME * ptime, PLAYTIME_SAVE_DATE *dest );
extern void PLAYTIME_SetSaveDate( PLAYTIME * ptime, const PLAYTIME_SAVE_DATE *src );

#endif	//__PLAYTIME_H__
