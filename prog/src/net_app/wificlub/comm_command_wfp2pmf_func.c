//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		comm_command_wfp2pmf_func.c
 *	@brief		２〜４人専用待合室	通信コマンド
 *	@author		tomoya takahashi
 *	@data		2007.05.24
 *
 *	このソースは
 *	wifi_p2pmatch.c	wifi_p2pmatchfour.c	どちらからも呼ばれるので常駐におきます。
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>

#include "wifi_p2pmatch_local.h"
#include "comm_command_wfp2pmf.h"
#include "comm_command_wfp2pmf_func.h"


#define WIFI_P2PMATCH_NUM_MAX	(4)	// 最大４人マッチング

enum{
	WFP2PMF_CON_NG,		// 通信NG
	WFP2PMF_CON_OK,		// 通信OK
	WFP2PMF_CON_LOCK,	// 今親が忙しい
};

typedef struct _WFP2P_WK WFP2P_WK;


// 通信データ
typedef struct {
	u16 netID;	// ネットID
	u32 flag;	// WFP2PMF_CON_〜
} WFP2PMF_COMM_RESULT;

// VCHATデータ
typedef struct {
	u8 vchat_now[WIFI_P2PMATCH_NUM_MAX];
} WFP2PMF_COMM_VCHAT;

extern void WFP2PMF_CommResultRecv( WFP2P_WK* p_wk, const WFP2PMF_COMM_RESULT* cp_data );
extern void WFP2PMF_CommStartRecv( WFP2P_WK* p_wk );
extern void WFP2PMF_CommVchatRecv( WFP2P_WK* p_wk, const WFP2PMF_COMM_VCHAT* cp_data );


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


//----------------------------------------------------------------------------
/**
 *	@brief	通信コマンド設定	wifi_p2pmatch.c内でテーブル設定用
 */
//-----------------------------------------------------------------------------
void CommCommandWFP2PMF_MatchStartInitialize( WIFIP2PMATCH_WORK *wk )
{
	GFL_NET_AddCommandTable(GFL_NET_CMD_WIFICLUB, WFP2PMF_CommCommandTclGet(), 
			WFP2PMF_CommCommandTblNumGet(), wk );
}

int CommWFP2PMFGetZeroSize( void )
{
	return 0;
}


//----------------------------------------------------------------------------
/**
 *	@brief	通信コマンド解除	
 */
//-----------------------------------------------------------------------------
void CommCommandWFP2PMF_MatchStartFinalize( WIFIP2PMATCH_WORK *wk )
{
	GFL_NET_DelCommandTable(GFL_NET_CMD_WIFICLUB);
}


//----------------------------------------------------------------------------
/**
 *	@brief	結果データサイズ
 *	@return	データサイズ
 */
//-----------------------------------------------------------------------------
int CommWFP2PMFGetWFP2PMF_COMM_RESULTSize( void )
{
	return sizeof(WFP2PMF_COMM_RESULT);
}

//----------------------------------------------------------------------------
/**
 *	@brief	VCHATデータサイズ
 *	@return	データサイズ
 */
//-----------------------------------------------------------------------------
int CommWFP2PMFGetWFP2PMF_COMM_VCHATSize( void )
{
	return sizeof(WFP2PMF_COMM_VCHAT);
}

//----------------------------------------------------------------------------
/**
 *	@brief	親からの通信OK通知
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommWFP2PMFGameResult(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	WFP2P_WK* p_wk = pWork;
	const WFP2PMF_COMM_RESULT* p_data = pData;

	if( p_wk == NULL ){
		return ;
	}
	
	//WFP2PMF_CommResultRecv( p_wk, p_data );
}

//----------------------------------------------------------------------------
/**
 *	@brief	親からのゲーム開始を通知
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommWFP2PMFGameStart(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	WFP2P_WK* p_wk = pWork;

	if( p_wk == NULL ){
		return ;
	}
	
	// ゲーム開始
	//WFP2PMF_CommStartRecv( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	親からVCHATフラグ取得
 *
 *	@param	netID		ネットID
 *	@param	size		サイズ
 *	@param	pBuff		バッファ
 *	@param	pWork		ワーク
 */
//-----------------------------------------------------------------------------
void CommWFP2PMFGameVchat(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
	WFP2P_WK* p_wk = pWork;

	if( p_wk == NULL ){
		return ;
	}
	
	// VCHATフラグ取得
//	WFP2PMF_CommVchatRecv( p_wk, pData );
}
