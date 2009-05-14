
//=============================================================================
/**
 * @file	network_define.h
 * @brief	WBのネットワーク定義を書くヘッダーファイル
 * @author	Katsumi Ohno
 * @date    2008.07.29
 */
//=============================================================================

#ifndef __NETWORK_DEFINE_H__
#define __NETWORK_DEFINE_H__

#define NET_WORK_ON (1)   // サンプルプログラム上で通信を動かす場合定義　動かさない場合未定義にする


//==============================================================================
//	サーバー接続先定義
//==============================================================================
///認証サーバーの開発用サーバー、製品版サーバーの切り替え
#ifdef PM_DEBUG	//--
///有効にしているとデバッグサーバーへ接続
#define DEBUG_SERVER
#endif	//-- PM_DEBUG


#ifdef DEBUG_SERVER
//開発用サーバー
#define GF_DWC_CONNECTINET_AUTH_TYPE	(DWC_CONNECTINET_AUTH_TEST)
//不思議な贈り物　定義を有効にすると本番サーバへ接続
//#define USE_AUTHSERVER_RELEASE			// 本番サーバへ接続
// dpw_common.h のDPW_SERVER_PUBLICの定義は直接ファイル内で変更する必要があります。
//DPW_SERVER_PUBLIC

//Wi-Fiロビーサーバ
#define GF_DWC_LOBBY_CHANNEL_PREFIX		(PPW_LOBBY_CHANNEL_PREFIX_DEBUG)

#else	//---------- DEBUG_SERVER

//製品版用サーバー
#define GF_DWC_CONNECTINET_AUTH_TYPE	(DWC_CONNECTINET_AUTH_RELEASE)
//不思議な贈り物　定義を有効にすると本番サーバへ接続
#define USE_AUTHSERVER_RELEASE			// 本番サーバへ接続
// dpw_common.h のDPW_SERVER_PUBLICの定義は直接ファイル内で変更する必要があります。
//DPW_SERVER_PUBLIC
//Wi-Fiロビーサーバ
#define GF_DWC_LOBBY_CHANNEL_PREFIX		(DWC_LOBBY_CHANNEL_PREFIX_RELEASE)

#endif	//---------- DEBUG_SERVER


//==============================================================================
//	定義
//==============================================================================

extern void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo,void* pWork);
extern u32 GFLR_NET_GetGGID(void);

//==============================================================================
//	型宣言
//==============================================================================

/// ゲームの種類を区別する為の定義   初期化構造体のgsidとして渡してください
enum NetworkServiceID_e {
  WB_NET_NOP_SERVICEID = 0,
  WB_NET_BATTLE_SERVICEID,       ///< バトル用ID
  WB_NET_FIELDMOVE_SERVICEID,  ///< サンプルのフィールド移動
  WB_NET_BOX_DOWNLOAD_SERVICEID,  ///< ダウンロード通信のボックス取得
  WB_NET_DEBUG_OHNO_SERVICEID,
  WB_NET_SERVICEID_DEBUG_TAYA,
  WB_NET_DEBUG_MATSUDA_SERVICEID,
  WB_NET_TRADE_SERVICEID,			///<ポケモン交換
  WB_NET_MINIGAME_TOOL,				///<WIFI広場ミニゲームツール
  WB_NET_MYSTERY,					///<不思議な贈り物
  WB_NET_BALLOON,					///<風船割り(ソーナンス)
  WB_NET_BUCKET,					///<球入れ(マルノーム)
  WB_NET_BALANCEBALL,				///<バランスボール(マネネ)
  WB_NET_WIFICLUB,                 ///<WIFICLUB
  WB_NET_COMPATI_CHECK,				///<相性チェック（テスト版）
  WB_NET_IRCBATTLE,               ///< IRC>WIRELESS>接続
  WB_NET_WIFILOBBY,                 ///<WIFI広場
	WB_NET_IRCCOMPATIBLE,			///相性チェックメニュー
  WB_NET_SERVICEID_MAX   // 終端
};

///通信コマンドを区別する為の定義  コールバックテーブルを渡すときにIDとして定義してください
/// 上の定義と同じ並びで書いてください
enum NetworkCommandHeaderNo_e {
  GFL_NET_CMD_BASE = (WB_NET_NOP_SERVICEID<<8),  ///< ベースコマンド開始番号
  GFL_NET_CMD_BATTLE = (WB_NET_BATTLE_SERVICEID<<8),   ///< バトル開始番号
  GFL_NET_CMD_FIELD = (WB_NET_FIELDMOVE_SERVICEID<<8),   ///< フィールドのベース開始番号
  GFL_NET_CMD_BOX_DOWNLOAD = (WB_NET_BOX_DOWNLOAD_SERVICEID<<8),   ///< フィールドのベース開始番号
  GFL_NET_CMD_DEBUG_OHNO = (WB_NET_DEBUG_OHNO_SERVICEID<<8),
  GFL_NET_CMD_DEBUG_TAYA = (WB_NET_SERVICEID_DEBUG_TAYA<<8),
  GFL_NET_CMD_DEBUG_MATSUDA = (WB_NET_DEBUG_MATSUDA_SERVICEID<<8),
  GFL_NET_CMD_TRADE = (WB_NET_TRADE_SERVICEID<<8),			///<ポケモン交換の開始番号
  GFL_NET_CMD_MINIGAME_TOOL = (WB_NET_MINIGAME_TOOL<<8),	///<WIFI広場ミニゲームツール
  GFL_NET_CMD_MYSTERY = (WB_NET_MYSTERY<<8),				///<不思議な贈り物
  GFL_NET_CMD_BALLOON = (WB_NET_BALLOON<<8),				///<風船割り
  GFL_NET_CMD_BUCKET = (WB_NET_BUCKET<<8),					///<球入れ
  GFL_NET_CMD_BALANCEBALL = (WB_NET_BALANCEBALL<<8),		///<バランスボール
  GFL_NET_CMD_WIFICLUB = (WB_NET_WIFICLUB<<8),				///<WIFICLUB
  GFL_NET_CMD_COMPATI_CHECK = (WB_NET_COMPATI_CHECK<<8),	///<相性チェック（テスト版）
  GFL_NET_CMD_IRCBATTLE = (WB_NET_IRCBATTLE<<8),  ///< IRC>WIRELESS>接続
  GFL_NET_CMD_WIFILOBBY = (WB_NET_WIFILOBBY<<8),			///<WIFI広場
	GFL_NET_CMD_IRCCOMPATIBLE	= (WB_NET_IRCCOMPATIBLE<<8),	///<相性チェックメニュー
};



#endif //__NETWORK_DEFINE_H__

