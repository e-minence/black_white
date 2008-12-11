//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		minigame_commcomand.c
 *	@brief		ミニゲームツール通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.11.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "common.h"
#include "communication/communication.h"
#include "minigame_commcomand_func.h"
#include "minigame_commcomand.h"

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

// 対応するコールバック関数
static const CommPacketTbl _CommBucketTbl[] = {
	//-------------------------------------
	//　ゲーム用
	//=====================================
	{ CommMNGMRetryYes, CommMNGMGetZeroSize, NULL },			///< 通信開始
	{ CommMNGMRetryNo, CommMNGMGetZeroSize, NULL },			///< 通信終了
	{ CommMNGMRetryOk, CommMNGMGetZeroSize, NULL },				///< 木の実通信
	{ CommMNGMRetryNg, CommMNGMGetZeroSize, NULL },			///< 得点通信
	{ CommMNGMRareGame, CommMNGMGetu32Size, NULL },			///< Rareゲームタイプ
};

//----------------------------------------------------------------------------
/**
 *	@brief		コマンドテーブルを取得
 */
//-----------------------------------------------------------------------------
const CommPacketTbl* MNGM_CommCommandTclGet( void )
{
	return _CommBucketTbl;
}

//----------------------------------------------------------------------------
/**
 *	@brief	コマンドテーブル要素数を取得
 */
//-----------------------------------------------------------------------------
int MNGM_CommCommandTblNumGet( void )
{
    return sizeof(_CommBucketTbl)/sizeof(CommPacketTbl);
}


