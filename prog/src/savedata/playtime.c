//============================================================================================
/**
 * @brief	playtime.c
 * @brief	�v���C���ԑ���
 * @date	2006.03.09
 * @author	tamada
 */
//============================================================================================
#include <gflib.h>
#include "savedata/playtime.h"
#include "playtime_local.h"

enum {
	PLAYTIME_HOUR_MAX = 999,
	PLAYTIME_MINUTE_MAX = 59,
	PLAYTIME_SECOND_MAX = 59,
};
//============================================================================================
//============================================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  PLAYTIME�̃��������m��
 *
 *	@param	heapID  �q�[�vID
 *
 *	@return �m�ۂ���������
 */
//-----------------------------------------------------------------------------
PLAYTIME * PLAYTIME_Allock( HEAPID heapID )
{
  PLAYTIME* ptime;
  ptime = GFL_HEAP_AllocClearMemory( heapID, sizeof(PLAYTIME) );
  PLAYTIME_Init( ptime );
  return ptime;
}

//---------------------------------------------------------------------------
/**
 * @brief	�v���C���Ԃ̏���������
 * @param	ptime	�v���C���Ԃւ̃|�C���^
 */
//---------------------------------------------------------------------------
void PLAYTIME_Init(PLAYTIME * ptime)
{
	ptime->hour = 0;
	ptime->minute = 0;
	ptime->second = 0;

	ptime->save_date.save_year = 0;
	ptime->save_date.save_month = 0;
	ptime->save_date.save_day = 0;
	ptime->save_date.save_hour = 0;
	ptime->save_date.save_minute = 0;
}

//---------------------------------------------------------------------------
/**
 * @brief	�v���C���Ԃ̃Z�b�g
 * @param	ptime	�v���C���Ԃւ̃|�C���^
 * @param	hour	����
 * @param	min		��
 * @param	sec		�b
 *
 * �w�肵���v���C���ԂɃZ�b�g���鏈���B
 * �f�o�b�O�ł����g�p���Ȃ��H
 */
//---------------------------------------------------------------------------
void PLAYTIME_Set(PLAYTIME * ptime, u32 hour, u32 min, u32 sec)
{
	GF_ASSERT(hour <= PLAYTIME_HOUR_MAX); 
	GF_ASSERT(min <= PLAYTIME_MINUTE_MAX); 
	GF_ASSERT(sec <= PLAYTIME_SECOND_MAX); 
	ptime->hour = hour;
	ptime->minute = min;
	ptime->second = sec;
}

//---------------------------------------------------------------------------
/**
 * @brief
 * @param	ptime	�v���C���Ԃւ̃|�C���^
 * @return
 */
//---------------------------------------------------------------------------
void PLAYTIME_CountUp(PLAYTIME * ptime, u32 passed_sec)
{
	u32 hour, min, sec;

	if( (ptime->hour == PLAYTIME_HOUR_MAX)
	&&	(ptime->minute == PLAYTIME_MINUTE_MAX)
	&&	(ptime->second == PLAYTIME_SECOND_MAX)
	){
		return;
	}

	hour = ptime->hour;
	min = ptime->minute;
	sec = ptime->second + passed_sec;
	if( sec > PLAYTIME_SECOND_MAX )
	{
		min = ptime->minute + (sec / (PLAYTIME_SECOND_MAX+1));
		sec %= (PLAYTIME_SECOND_MAX+1);
		if( min > PLAYTIME_MINUTE_MAX )
		{
			hour = ptime->hour + (min / (PLAYTIME_MINUTE_MAX+1));
			min %= (PLAYTIME_MINUTE_MAX+1);
			if( hour >= PLAYTIME_HOUR_MAX )
			{
				hour = PLAYTIME_HOUR_MAX;
				min = PLAYTIME_MINUTE_MAX;
				sec = PLAYTIME_SECOND_MAX;
			}
		}
	}

	ptime->hour = hour;
	ptime->minute = min;
	ptime->second = sec;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ہX����ݒ�
 *
 *	@param	ptime   ���[�N
 *	@param	data    �f�[�^
 */
//-----------------------------------------------------------------------------
void PLAYTIME_SetAllData( PLAYTIME * ptime, const PLAYTIME * data )
{
  GFL_STD_MemCopy( data, ptime, sizeof(PLAYTIME) );
}

//---------------------------------------------------------------------------
/**
 * @brief
 * @param	ptime	�v���C���Ԃւ̃|�C���^
 * @return
 */
//---------------------------------------------------------------------------
u16 PLAYTIME_GetHour(const PLAYTIME * ptime)
{
	return ptime->hour;
}

//---------------------------------------------------------------------------
/**
 * @brief
 * @param	ptime	�v���C���Ԃւ̃|�C���^
 * @return
 */
//---------------------------------------------------------------------------
u8 PLAYTIME_GetMinute(const PLAYTIME * ptime)
{
	return ptime->minute;
}

//---------------------------------------------------------------------------
/**
 * @brief
 * @param	ptime	�v���C���Ԃւ̃|�C���^
 * @return
 */
//---------------------------------------------------------------------------
u8 PLAYTIME_GetSecond(const PLAYTIME * ptime)
{
	return ptime->second;
}


//---------------------------------------------------------------------------
/**
 * @brief		�Z�[�u�������t�Ǝ��Ԃ�ݒ�
 * @param		ptime		�v���C���Ԃւ̃|�C���^
 * @return	none
 */
//---------------------------------------------------------------------------
void PLAYTIME_SetSaveTime( PLAYTIME * ptime )
{
	RTCDate	date;
	RTCTime	time;

	RTC_GetDate( &date );
	RTC_GetTime( &time );

	ptime->save_date.save_year = date.year;
	ptime->save_date.save_month = date.month;
	ptime->save_date.save_day = date.day;
	ptime->save_date.save_hour = time.hour;
	ptime->save_date.save_minute = time.minute;
}

//---------------------------------------------------------------------------
/**
 * @brief		�Z�[�u�����N���擾
 * @param		ptime		�v���C���Ԃւ̃|�C���^
 * @return	�N
 */
//---------------------------------------------------------------------------
u32 PLAYTIME_GetSaveYear( const PLAYTIME * ptime )
{
	return ptime->save_date.save_year;
}

//---------------------------------------------------------------------------
/**
 * @brief		�Z�[�u���������擾
 * @param		ptime		�v���C���Ԃւ̃|�C���^
 * @return	��
 */
//---------------------------------------------------------------------------
u32 PLAYTIME_GetSaveMonth( const PLAYTIME * ptime )
{
	return ptime->save_date.save_month;
}

//---------------------------------------------------------------------------
/**
 * @brief		�Z�[�u���������擾
 * @param		ptime		�v���C���Ԃւ̃|�C���^
 * @return	��
 */
//---------------------------------------------------------------------------
u32 PLAYTIME_GetSaveDay( const PLAYTIME * ptime )
{
	return ptime->save_date.save_day;
}

//---------------------------------------------------------------------------
/**
 * @brief		�Z�[�u���������擾
 * @param		ptime		�v���C���Ԃւ̃|�C���^
 * @return	��
 */
//---------------------------------------------------------------------------
u32 PLAYTIME_GetSaveHour( const PLAYTIME * ptime )
{
	return ptime->save_date.save_hour;
}

//---------------------------------------------------------------------------
/**
 * @brief		�Z�[�u���������擾
 * @param		ptime		�v���C���Ԃւ̃|�C���^
 * @return	��
 */
//---------------------------------------------------------------------------
u32 PLAYTIME_GetSaveMinute( const PLAYTIME * ptime )
{
	return ptime->save_date.save_minute;
}

//==================================================================
/**
 * �Z�[�u�������t���擾
 *
 * @param   ptime		
 * @param   dest		�����
 */
//==================================================================
void PLAYTIME_GetSaveDate( const PLAYTIME * ptime, PLAYTIME_SAVE_DATE *dest )
{
  *dest = ptime->save_date;
}

//==================================================================
/**
 * �Z�[�u�������t���Z�b�g
 *
 * @param   ptime		
 * @param   src		  
 */
//==================================================================
void PLAYTIME_SetSaveDate( PLAYTIME * ptime, const PLAYTIME_SAVE_DATE *src )
{
  ptime->save_date = *src;
}
