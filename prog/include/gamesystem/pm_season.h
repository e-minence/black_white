//============================================================================================
/**
 * @file	pm_season.h
 * @brief	季節の定義
 * @author	tamada GAME FREAK Inc.
 * @date	09.03.17
 *
 */
//============================================================================================

#pragma once

#ifdef	__cplusplus
extern "C" {
#endif

//============================================================================================
//============================================================================================

#define	PMSEASON_SPRING		(0)			/**!< 春 */
#define	PMSEASON_SUMMER		(1)			/**!< 夏 */
#define	PMSEASON_AUTUMN		(2)			/**!< 秋 */
#define	PMSEASON_WINTER		(3)			/**!< 冬 */

#define	PMSEASON_TOTAL		(4)			/**!< 当たり前だが四季は４つ */


#ifndef __ASM_NO_DEF_

//--------------------------------------------------------------------------------------------
/**
 * @brief 現在時刻の季節を計算する
 *
 * @param start_date ゲーム開始日時[月/日]
 * 
 * @return 現在時刻の季節( PMSEASON_SPRING など )
 */
//--------------------------------------------------------------------------------------------
u8 PMSEASON_CalcSeasonID_byDate( const RTCDate* start_date );

//--------------------------------------------------------------------------------------------
/**
 * @brief 現在時刻の季節を計算する
 *
 * @param start_sec ゲーム開始時刻[sec]
 * 
 * @return 現在時刻の季節( PMSEASON_SPRING など )
 */
//--------------------------------------------------------------------------------------------
u8 PMSEASON_CalcSeasonID_bySec( s64 start_sec );

#endif  // __ASM_NO_DEF_


#ifdef	__cplusplus
} /* extern "C" */
#endif

