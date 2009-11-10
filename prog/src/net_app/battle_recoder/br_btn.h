//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_btn.h
 *	@brief	バトルレコーダー	ボタン管理
 *	@author	Toru=Nagihashi
 *	@data		2009.11.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	バトルレコーダー状態
//=====================================
typedef enum
{
	BR_BTN_SYS_STATE_WAIT,	//タッチ待ち
	BR_BTN_SYS_STATE_MOVE,	//移動中
} BR_BTN_SYS_STATE;

//-------------------------------------
///	バトルレコーダーボタン状態
//=====================================
typedef enum
{
	BR_BTN_SYS_INPUT_CHANGESEQ
	BR_BTN_SYS_INPUT_EXIT
} BR_BTN_SYS_INPUT;

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	バトルレコーダー　ボタン管理
//=====================================
typedef struct BR_BTN_SYS_WORK BR_BTN_SYS_WORK;
//=============================================================================
/**
 *					PUBILIC関数
*/
//=============================================================================
extern BR_BTN_SYS_WORK *BR_BTN_SYS_Init( HEAPID heapID );
extern void BR_BTN_SYS_Exit( BR_BTN_SYS_WORK *p_wk );
extern void BR_BTN_SYS_Main( BR_BTN_SYS_WORK *p_wk );
extern BR_BTN_SYS_INPUT BR_BTN_SYS_GetInput( const BR_BTN_SYS_WORK *cp_wk, u32 *p_seq );
extern BR_BTN_SYS_STATE BR_BTN_SYS_GetState( const BR_BTN_SYS_WORK *cp_wk );
