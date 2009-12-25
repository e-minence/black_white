//============================================================================================
/**
 * @file	pm_season.h
 * @brief	季節の定義
 * @author	tamada GAME FREAK Inc.
 * @date	09.03.17
 *
 */
//============================================================================================

#include <gflib.h>
#include "gamesystem/pm_season.h"

//--------------------------------------------------------------------------------------------
/**
 * @brief 現在時刻の季節を計算する
 *
 * @return 現在時刻の季節( PMSEASON_SPRING など )
 */
//--------------------------------------------------------------------------------------------
u8 PMSEASON_GetCurrentSeasonID()
{
  RTCDate date;
  u8 season;

  // 現在時刻を取得
  GFL_RTC_GetDate( &date );

  // 季節を計算
  season = (date.month - 1) % PMSEASON_TOTAL;
  return season;
} 

//--------------------------------------------------------------------------------------------
/**
 * @brief 次の季節を求める
 *
 * @param now 現在の季節( PMSEASON_SPRING など )
 *
 * @return 次の季節( PMSEASON_SPRING など )
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
 * @brief 前の季節を求める
 *
 * @param now 現在の季節( PMSEASON_SPRING など )
 *
 * @return 前の季節( PMSEASON_SPRING など )
 */
//--------------------------------------------------------------------------------------------
extern u8 PMSEASON_GetPrevSeasonID( u8 now )
{
  u8 prev;
  prev = (now + PMSEASON_TOTAL - 1) % PMSEASON_TOTAL;
  return prev;
}
