//============================================================================================
/**
 * @file	pm_season.h
 * @brief	�G�߂̒�`
 * @author	tamada GAME FREAK Inc.
 * @date	09.03.17
 *
 */
//============================================================================================

#include <gflib.h>
#include "gamesystem/pm_season.h"

//--------------------------------------------------------------------------------------------
/**
 * @brief ���ݎ����̋G�߂��v�Z����
 *
 * @param start_date �Q�[���J�n����[��/��]
 * 
 * @return ���ݎ����̋G��( PMSEASON_SPRING �Ȃ� )
 */
//--------------------------------------------------------------------------------------------
u8 PMSEASON_CalcSeasonID_byDate( const RTCDate* start_date )
{
  RTCDate now_date;
  u8 season; 

  GFL_RTC_GetDate( &now_date );
  season = (now_date.month + 12 - start_date->month) % PMSEASON_TOTAL;
  return season;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���ݎ����̋G�߂��v�Z����
 *
 * @param start_sec �Q�[���J�n����[sec]
 * 
 * @return ���ݎ����̋G��( PMSEASON_SPRING �Ȃ� )
 */
//--------------------------------------------------------------------------------------------
u8 PMSEASON_CalcSeasonID_bySec( s64 start_sec )
{
  RTCDate start_date;
  RTCTime start_time;

  // �J�n����[sec]�� ����, ���Ԃɕϊ�
  RTC_ConvertSecondToDateTime( &start_date, &start_time, start_sec );

  // �G�߂��v�Z
  return PMSEASON_CalcSeasonID_byDate( &start_date );
} 
