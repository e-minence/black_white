//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		debug_ui.h
 *	@brief  UI　上書き　デバック機能
 *	@author	tomoya takahashi
 *	@date		2010.04.29
 *
 *	モジュール名：
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#ifdef PM_DEBUG

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	起動タイプ
//=====================================
typedef enum {
  DEBUG_UI_NONE,            // なし
  DEBUG_UI_AUTO_UPDOWN,     // 自動　上下
  DEBUG_UI_AUTO_LEFTRIGHT,  // 自動　左右
  DEBUG_UI_AUTO_USER_INPUT, // 自動　入力データ再生

  DEBUG_UI_TYPE_MAX,
} DEBUG_UI_TYPE;

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

extern void DEBUG_UI_SetUp( DEBUG_UI_TYPE type );
extern DEBUG_UI_TYPE DEBUG_UI_GetType( void );


#endif // PM_DEBUG



#ifdef _cplusplus
}	// extern "C"{
#endif



