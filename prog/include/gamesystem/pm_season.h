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
 * @return 現在時刻の季節( PMSEASON_SPRING など )
 */
//--------------------------------------------------------------------------------------------
extern u8 PMSEASON_GetCurrentSeasonID(); 

//--------------------------------------------------------------------------------------------
/**
 * @brief 次の季節を求める
 *
 * @param now 現在の季節( PMSEASON_SPRING など )
 *
 * @return 次の季節( PMSEASON_SPRING など )
 */
//--------------------------------------------------------------------------------------------
extern u8 PMSEASON_GetNextSeasonID( u8 now );

//--------------------------------------------------------------------------------------------
/**
 * @brief 前の季節を求める
 *
 * @param now 現在の季節( PMSEASON_SPRING など )
 *
 * @return 前の季節( PMSEASON_SPRING など )
 */
//--------------------------------------------------------------------------------------------
extern u8 PMSEASON_GetPrevSeasonID( u8 now );
#endif  // __ASM_NO_DEF_


#ifdef	__cplusplus
} /* extern "C" */
#endif

