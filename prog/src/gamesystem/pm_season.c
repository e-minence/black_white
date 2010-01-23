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
 * @return ���ݎ����̋G��( PMSEASON_SPRING �Ȃ� )
 */
//--------------------------------------------------------------------------------------------
u8 PMSEASON_GetRealTimeSeasonID()
{
  RTCDate date;
  u8 season;

  // ���ݎ������擾
  GFL_RTC_GetDate( &date );

  // �G�߂��v�Z
  season = (date.month - 1) % PMSEASON_TOTAL;
  return season;
} 

//--------------------------------------------------------------------------------------------
/**
 * @brief ���̋G�߂����߂�
 *
 * @param now ���݂̋G��( PMSEASON_SPRING �Ȃ� )
 *
 * @return ���̋G��( PMSEASON_SPRING �Ȃ� )
 */
//--------------------------------------------------------------------------------------------
extern u8 PMSEASON_GetNextSeasonID( u8 now )
{
  u8 next;
  next = (now + 1) % PMSEASON_TOTAL;
  return next;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �O�̋G�߂����߂�
 *
 * @param now ���݂̋G��( PMSEASON_SPRING �Ȃ� )
 *
 * @return �O�̋G��( PMSEASON_SPRING �Ȃ� )
 */
//--------------------------------------------------------------------------------------------
extern u8 PMSEASON_GetPrevSeasonID( u8 now )
{
  u8 prev;
  prev = (now + PMSEASON_TOTAL - 1) % PMSEASON_TOTAL;
  return prev;
}
