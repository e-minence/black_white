//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wf2dmap_scroll.c
 *	@brief		スクロールデータ管理+スクロール座標管理モジュール
 *	@author		tomoya takahashi
 *	@data		2007.03.19
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <gflib.h>

#include "net_app/wifi2dmap/wf2dmap_scroll.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------
#define WF2DMAP_SCROLL_CENTER_X	( 112 )	// 中心座標
#define WF2DMAP_SCROLL_CENTER_Y ( 96 )	// 中心座標

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *	@brief	スクロールデータ計算
 *
 *	@param	p_data		スクロールデータワーク
 *	@param	cp_objwk	オブジェクト管理システム
 */
//-----------------------------------------------------------------------------
void WF2DMAP_SCRContSysMain( WF2DMAP_SCROLL* p_data, const WF2DMAP_OBJWK* cp_objwk )
{
	WF2DMAP_POS hero_pos;

	// オブジェクト
	hero_pos = WF2DMAP_OBJWkFrameMatrixGet( cp_objwk );

	// hero_pos
	WF2DMAP_SCROLLSysDataSet( p_data, 
			hero_pos.y - WF2DMAP_SCROLL_CENTER_Y,
			hero_pos.x - WF2DMAP_SCROLL_CENTER_X
			);
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロールデータ初期化
 *	
 *	@param	p_sys	ワーク
 */
//-----------------------------------------------------------------------------
void WF2DMAP_SCROLLSysDataInit( WF2DMAP_SCROLL* p_sys )
{
	memset( p_sys, 0, sizeof(WF2DMAP_SCROLL) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロールデータ設定
 *
 *	@param	p_sys		ワーク
 *	@param	draw_top	スクロール上座標
 *	@param	draw_left	スクロール左座標
 */
//-----------------------------------------------------------------------------
void WF2DMAP_SCROLLSysDataSet( WF2DMAP_SCROLL* p_sys, s16 draw_top, s16 draw_left )
{
	p_sys->draw_top = draw_top;
	p_sys->draw_left = draw_left;
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロールデータ取得
 *
 *	@param	cp_sys		ワーク
 *
 *	@return	上座標
 */
//-----------------------------------------------------------------------------
s16 WF2DMAP_SCROLLSysTopGet( const WF2DMAP_SCROLL* cp_sys )
{
	return cp_sys->draw_top;
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロールデータ取得
 *
 *	@param	cp_sys		ワーク
 *
 *	@return	左座標
 */
//-----------------------------------------------------------------------------
s16 WF2DMAP_SCROLLSysLeftGet( const WF2DMAP_SCROLL* cp_sys )
{
	return cp_sys->draw_left;
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロール上座標を足す
 *	
 *	@param	p_sys		ワーク
 *	@param	add			足す値
 */
//-----------------------------------------------------------------------------
void WF2DMAP_SCROLLSysTopAdd( WF2DMAP_SCROLL* p_sys, s16 add )
{
	p_sys->draw_top += add;
}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロール左座標を足す
 *
 *	@param	p_sys		ワーク
 *	@param	add			足す値
 */
//-----------------------------------------------------------------------------
void WF2DMAP_SCROLLSysLeftAdd( WF2DMAP_SCROLL* p_sys, s16 add )
{
	p_sys->draw_left += add;
}

