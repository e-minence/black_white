//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		imc_hand.h
 *	@brief		ハンドシステム	タッチパネルの動きにより動作させるシステム	
 *	@author		tomoya takahashi
 *	@data		2005.09.23
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __IMC_HAND_H__
#define __IMC_HAND_H__

#include <nnsys.h>

#undef GLOBAL
#ifdef	__IMC_HAND_H_GLOBAL
#define	GLOBAL	/* */
#else
#define	GLOBAL	extern
#endif

//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	
//	ハンド 構造体
//	
//=====================================
typedef struct _IMC_HAND{

	void*	hand_w;		// ワーク

	void (*push)( struct _IMC_HAND* work );		// 押されたとき
	void (*pop)( struct _IMC_HAND* work );		// 離されたとき
	void (*push_ing)( struct _IMC_HAND* work );	// 押され続けているとき


	void (*dest)( struct _IMC_HAND* work );		// 破棄関数
	
	s16		old_x;		// 前の座標
	s16		old_y;		// 前の座標
	u8		old_cont;	// 前のデータが押されていたか
} IMC_HAND;

//-----------------------------------------------------------------------------
/**
*					型宣言
*/
//-----------------------------------------------------------------------------
typedef void ( *IMC_HAND_FUNC )( IMC_HAND* work );


//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *
 *	@brief	ハンド構造体初期化 / 破棄
 *
 *	@param	hand	ハンド構造体
 *
 *	@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
GLOBAL void IMC_HAND_Init( IMC_HAND* hand );

//----------------------------------------------------------------------------
/**
 *
 *	@brief	ハンド構造体を破棄
 *
 *	@param	hand	ハンド構造体
 *
 *	@return	none
 *	
 *	ハンド構造体内に設定されている破棄関数を実行します。
 *
 *	なでこのようにしたか。
 *		今どんなハンドシステムを使用しているかを覚えておかなくても
 *		この関数を実行さえすれば完全に破棄されるようにするため
 *
 */
//-----------------------------------------------------------------------------
GLOBAL void IMC_HAND_Delete( IMC_HAND* hand );

//----------------------------------------------------------------------------
/**
 *
 *	@brief	ハンドシステムメイン関数
 *	
 *	@param	hand	ハンド構造体
 *
 *	@return	none
 *
 *
 */
//-----------------------------------------------------------------------------
GLOBAL void IMC_HAND_Main( IMC_HAND* hand );

#undef	GLOBAL
#endif		// __IMC_HAND_H__

