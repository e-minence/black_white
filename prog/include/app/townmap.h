//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		townmap.h
 *	@brief	タウンマップ外部公開ヘッダ
 *	@author	Toru=Nagihashi
 *	@date		2009.07.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once
#include <gflib.h>
#include "gamesystem/gamesystem.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	タウンマップ起動モード
//=====================================
typedef enum
{
	TOWNMAP_MODE_SKY,	//そらをとぶモード
	TOWNMAP_MODE_MAP,	//タウンマップ見るモード
	TOWNMAP_MODE_DEBUGSKY,		//デバッグ空を飛ぶモード

	TOWNMAP_MODE_MAX
} TOWNMAP_MODE;

//-------------------------------------
///	タウンマップ選択
//=====================================
typedef enum
{
	TOWNMAP_SELECT_RETURN,	//何もせず戻る
	TOWNMAP_SELECT_CLOSE,		//何もせず終了する
	TOWNMAP_SELECT_SKY,			//飛ぶ先を選択した

	TOWNMAP_SELECT_MAX
} TOWNMAP_SELECT;


//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	タウンマップPROC用パラメータ
//=====================================
typedef struct 
{
	TOWNMAP_MODE		mode;			//[in]			動作モード
	TOWNMAP_SELECT	select;		//[out]			選択の種類
	u16							zoneID;		//[in/out]	[in]主人公が今いるゾーンID[out]空を飛ぶゾーンID
	u16							escapeID;	//[in]			戻りゾーンID
	GFL_POINT				grid;			//[out]			空を飛んだ先の座標
	GAMESYS_WORK		*p_gamesys;//[in]			ゲームシステム
} TOWNMAP_PARAM;


//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
///	PROCデータ外部公開
//=====================================
extern const GFL_PROC_DATA	TownMap_ProcData;
