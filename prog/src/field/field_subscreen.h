//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_subscreen.h
 *	@brief		InforBerの初期化・破棄　
 *	@data		2009.03.26	fieldmapから移植
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//BG面とパレット番号(仮設定
#define FIELD_SUBSCREEN_BGPLANE	(GFL_BG_FRAME0_S)
#define FIELD_SUBSCREEN_PALLET	(0xE)

///下画面のモード指定（デバッグでしかつかわないはず）
typedef enum {	
	FIELD_SUBSCREEN_NORMAL = 0,
	FIELD_SUBSCREEN_DEBUG_LIGHT,
	FIELD_SUBSCREEN_DEBUG_TOUCHCAMERA,
	FIELD_SUBSCREEN_DEBUG_SOUNDVIEWER,

	FIELD_SUBSCREEN_MODE_MAX,
}FIELD_SUBSCREEN_MODE;

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

typedef struct _FIELD_SUBSCREEN_WORK FIELD_SUBSCREEN_WORK;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern FIELD_SUBSCREEN_WORK* FIELD_SUBSCREEN_Init( u32 heapID, FIELD_SUBSCREEN_MODE mode );
extern void FIELD_SUBSCREEN_Exit( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Main( FIELD_SUBSCREEN_WORK* pWork );
extern void FIELD_SUBSCREEN_Change( FIELD_SUBSCREEN_WORK* pWork, FIELD_SUBSCREEN_MODE new_mode);
extern FIELD_SUBSCREEN_MODE FIELD_SUBSCREEN_GetMode(const FIELD_SUBSCREEN_WORK * pWork);

#ifdef	PM_DEBUG
extern void * FIELD_SUBSCREEN_DEBUG_GetControl(FIELD_SUBSCREEN_WORK * pWork);
#endif	//PM_DEBUG

#ifdef _cplusplus
}	// extern "C"{
#endif



