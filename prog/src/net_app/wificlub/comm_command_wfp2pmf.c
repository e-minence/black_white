//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		comm_command_wfp2pmf.c
 *	@brief		２〜４人専用待合室	通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.05.24
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "wifi_p2pmatch_local.h"
#include "comm_command_wfp2pmf.h"
#include "comm_command_wfp2pmf_func.h"

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
static const NetRecvFuncTable _CommNutWFP2PMFPacketTbl[] = {
	//-------------------------------------
	//　ゲーム用
	//=====================================
	{ CommWFP2PMFGameResult, NULL },			///< 通信OK			親ー＞子
	{ CommWFP2PMFGameStart,  NULL },			///< ゲーム開始			親ー＞子CNM_WFP2PMF_START
	{ CommWFP2PMFGameVchat,  NULL },	///< VCHATデータ	親ー＞子CNM_WFP2PMF_VCHAT
  { WifiP2PMatchRecvGameStatus,  NULL },//CNM_WFP2PMF_STATUS
  { WifiP2PMatchRecvMyStatus,NULL},//CNM_WFP2PMF_MYSTATUS
  { WifiP2PMatchRecvDirectMode,NULL},//CNM_WFP2PMF_DIRECT_COMMAND
  { WifiP2PMatchRecvBattleRegulation,NULL},//CNM_WFP2PMF_REGLATION
  { WifiP2PMatchRecvPokeParty, WifiP2PMatchGetPokePartyWork},//CNM_WFP2PMF_POPEPARTY
  { WifiP2PMatchRecvVctOff, NULL},//CNM_WFP2PMF_VCHATOFF
};

//----------------------------------------------------------------------------
/**
 *	@brief		コマンドテーブルを取得
 */
//-----------------------------------------------------------------------------
const NetRecvFuncTable* WFP2PMF_CommCommandTclGet( void )
{
	return _CommNutWFP2PMFPacketTbl;
}

//----------------------------------------------------------------------------
/**
 *	@brief	コマンドテーブル要素数を取得
 */
//-----------------------------------------------------------------------------
int WFP2PMF_CommCommandTblNumGet( void )
{
    return (CNM_COMMAND_MAX-GFL_NET_CMD_WIFICLUB);
}
