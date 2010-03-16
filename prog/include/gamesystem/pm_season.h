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

#include <gflib.h>
#include "gamesystem.h"

//----------------------------------------------------------------------------
//　通常の季節取得関数は、gamesystem/game_data.h
//  GAMEDATA_GetSeasonID()
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
/**
 *	@brief  通信（進入）に対応した季節を取得
 *
 *	@param	gamedata  ゲームデータ
 *
 *	@return 通信（進入）に対応した季節を取得
 *
 *	＊使用箇所
 *	  エリアデータ−−−−−−−地形にかかわる情報
 *	  フィールドエフェクト−−−地面にかかわる演出
 */
//-----------------------------------------------------------------------------
extern u8 PMSEASON_GetConsiderCommSeason( GAMESYS_WORK* gamesys );


//----------------------------------------------------------------------------
/**
 *	@brief  季節の変更を取得  （進入中などを考慮して季節を変更させます。）
 *
 *	@param	gamesys     ゲームシステムデータ
 *	@param  prevSeason  今の季節　格納先
 *	@param  nextSeason  次の季節　格納先
 *
 *	@retval TRUE    変更あり
 *	@retval FALSE   変更なし    変更なしの場合、prevSeason == nextSeason
 */
//-----------------------------------------------------------------------------
extern BOOL PMSEASON_GetNextSeason( const GAMEDATA* gamedata, u16* prevSeason, u16* nextSeason );


//--------------------------------------------------------------------------------------------
/**
 * @brief 現在時刻の季節を計算する
 *
 * @return 現在時刻の季節( PMSEASON_SPRING など )
 */
//--------------------------------------------------------------------------------------------
extern u8 PMSEASON_GetRealTimeSeasonID(); 


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


//----------------------------------------------------------------------------
/**
 *	@brief  季節を設定
 *
 *	@param	gamedata    ゲームデータ
 *	@param  season      次の季節
 */
//-----------------------------------------------------------------------------
extern void PMSEASON_SetSeasonID( GAMEDATA* gamedata, u16 season );

#endif  // __ASM_NO_DEF_


#ifdef	__cplusplus
} /* extern "C" */
#endif

