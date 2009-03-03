//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		comm_bct_command.c
 *	@brief		バケットミニゲーム　通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.06.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "net\network_define.h"
#include "comm_bct_command_func.h"
#include "comm_bct_command.h"


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
static const NetRecvFuncTable _CommBucketTbl[] = {
	//-------------------------------------
	//　ゲーム用
	//=====================================
	{ CommBCTGameStart, CommBCTGetZeroSize, NULL },			///< 通信開始
	{ CommBCTGameEnd, CommBCTGetZeroSize, NULL },			///< 通信終了
	{ CommBCTNuts, CommBCTGetNutsSize, NULL },				///< 木の実通信
	{ CommBCTScore, CommBCTGetScoreSize, NULL },			///< 得点通信
	{ CommBCTAllScore, CommBCTGetAllScoreSize, NULL },		///< みんなの得点通信
	{ CommBCTGameDataIdx, CommBCTGetGameDataIdxSize, NULL },///< ゲームレベルの送信		親ー＞子
	{ CommBCTMiddleScore, CommBCTGetScoreSize, NULL },		///< 途中の自分の得点				子ー＞子
	{ CommBCTMiddleScoreOk, CommBCTGetZeroSize, NULL },		///< 全員の得点がきた		親ー＞子
};

//----------------------------------------------------------------------------
/**
 *	@brief		コマンドテーブルを取得
 */
//-----------------------------------------------------------------------------
const NetRecvFuncTable* BCT_CommCommandTclGet( void )
{
	return _CommBucketTbl;
}

//----------------------------------------------------------------------------
/**
 *	@brief	コマンドテーブル要素数を取得
 */
//-----------------------------------------------------------------------------
int BCT_CommCommandTblNumGet( void )
{
    return sizeof(_CommBucketTbl)/sizeof(NetRecvFuncTable);
}

