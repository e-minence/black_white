
//=============================================================================
/**
 * @file  network_define.h
 * @brief WBのネットワーク定義を書くヘッダーファイル
 * @author  Katsumi Ohno
 * @date    2008.07.29
 */
//=============================================================================

#pragma once

#define NET_WORK_ON (1)   // サンプルプログラム上で通信を動かす場合定義　動かさない場合未定義にする


#define GAME_NAME					"syachi2ds"		// 使用するゲーム名
#define GAME_SECRET_KEY		"tXH2sN"					// 使用するシークレットキー
#define GAME_PRODUCTID		12230							// 使用するプロダクトID
#define GAME_ID           2911              // ゲームID


#define WIFI_ND_TIMEOUT_SYNC   (60*60*2)     //２分のタイムアウト


#if 0

●GameSpy設定情報（IRAJ/IRBJ 両バージョン共通です）
Game Name     syachi2ds 
Secret Key    tXH2sN 
Product ID    12230 
Game ID       2911 
Master Index  10

●ダウンロードサーバ（両バージョン共通です）
ダウンロードサーバ接続用ゲームコード・
ダウンロードサーバ管理画面ユーザー名 
IRAJ 

−開発用ダウンロードサーバ
管理画面URL   https://dls1ctl.test.nintendowifi.net/control
管理画面パスワード  wG7Q2xL4
ゲーム用パスワード  JDXNcp79mr54RUga 

−製品用ダウンロードサーバ
管理画面：（製品用サーバへアップする際は吉原までご相談下さい）
ゲーム用パスワード  fZrUY6SVMLp4tdmB 


●エラーシミュレーションサーバ
・IRAJ
エラーシミュレーションサーバーのID TWL-IRA 
エラーシミュレーションサーバーのパスワード cmMWFX 

・IRBJ
エラーシミュレーションサーバーのID TWL-IRB 
エラーシミュレーションサーバーのパスワード NvT4rI 

//また、GGIDにつきましても発行いたしました。
(1)0x00001380　通常通信用
(2)0x00001381　データビーコン配信用
(3)0x00001382　WCSレギュレーション配信用








/*
◆GDSの切り替えについて

poke_netライブラリの初期化関数"POKE_NET_GDS_Initialize"の引数に渡しているURLを、製品／デバッグで切り替えて頂く事になります。
下記を参照頂けますでしょうか。

（テストサーバ：デバッグ版）
アドレス･･･pkgdstest.nintendo.co.jp
ポート･･･12401

（本番サーバ：製品版）
アドレス･･･pkgdsprod.nintendo.co.jp
ポート･･･12401


◆不正検査サーバの製品版URL

https://pkvldtprod.nintendo.co.jp/pokemon/validate
*/

#endif

/// 分けたかったのですが、CGEARの仕様によりGGIDを統一します
/// 2010.01.08 k.ohno
#define SYACHI_NETWORK_GGID  (0x00001380)    //通常はこれ
#define SYACHI_DOWNLOAD_GGID  (0x00001381)   //映画２０１０専用 MB不具合用
//#define SYACHI_LEG_GGID (0x00001380)         //大会レギュレーション配信専用

//#define SYACHI_NETWORK_GGID  (0x00001511)  //syachi試遊台のGGID 2010.06.29



#define NET_DREAMWORLD_VER (1)  //ドリームワールド通信バーション（かならず整数）


/*
>４．IRC_InitのID
>これは懸念点ではございません。
>IRC_Initのマニュアルに御社から頂くコードを入れてくださいと
>書いてあるのですが、発行していただけますでしょうか？

　こちらですが、Syachiは
3
でお願い致します。
*/
#define POKEMON_IRC_CONNECTCODE (3)  //赤外線接続コード 確定

//==============================================================================
//  サーバー接続先定義
//==============================================================================
///認証サーバーの開発用サーバー、製品版サーバーの切り替え
#ifdef PM_DEBUG //--
///有効にしているとデバッグサーバーへ接続
#define DEBUG_SERVER
#endif  //-- PM_DEBUG

//DWC_Init〜で仕様する、GameCode
#if ( PM_VERSION == VERSION_WHITE )
#define GF_DWC_GAMECODE		('IRAJ')
#define GF_DWC_ND_LOGIN		"IRAJ"
#else //defined(VERSION_WHITE)
#define GF_DWC_GAMECODE		('IRAJ')
#define GF_DWC_ND_LOGIN		"IRAJ"
#endif

//DWC_Init〜で仕様する、GameName
#define GF_DWC_GAMENAME		("syachi2ds")

#ifdef DEBUG_SERVER
//開発用サーバー
#define GF_DWC_CONNECTINET_AUTH_TYPE  (DWC_CONNECTINET_AUTH_TEST)
//不思議な贈り物　定義を有効にすると本番サーバへ接続
//#define USE_AUTHSERVER_RELEASE      // 本番サーバへ接続
// dpw_common.h のDPW_SERVER_PUBLICの定義は直接ファイル内で変更する必要があります。
//DPW_SERVER_PUBLIC

//Wi-Fiロビーサーバ
//#define GF_DWC_LOBBY_CHANNEL_PREFIX   (PPW_LOBBY_CHANNEL_PREFIX_DEBUG)
#define GF_DWC_LOBBY_CHANNEL_PREFIX   (PPW_LOBBY_CHANNEL_PREFIX_DEBUG5) //金銀デバッグ中の為、WBは9

//レポート送信
#define GF_DWC_GDB_AUTH_TYPE          (DWC_GDB_SSL_TYPE_NONE)

#define MATCHINGKEY (0)

///GDSサーバー(デバッグ版)
#define GF_GDS_SERVER_URL     "pkgdstest.nintendo.co.jp"
#define GF_GDS_SERVER_PORT    (12401)

#else //---------- DEBUG_SERVER

//製品版用サーバー
#define GF_DWC_CONNECTINET_AUTH_TYPE  (DWC_CONNECTINET_AUTH_RELEASE)
//不思議な贈り物　定義を有効にすると本番サーバへ接続
#define USE_AUTHSERVER_RELEASE      // 本番サーバへ接続
// dpw_common.h のDPW_SERVER_PUBLICの定義は直接ファイル内で変更する必要があります。
//DPW_SERVER_PUBLIC
//Wi-Fiロビーサーバ
#define GF_DWC_LOBBY_CHANNEL_PREFIX   (DWC_LOBBY_CHANNEL_PREFIX_RELEASE)

//レポート送信
#define GF_DWC_GDB_AUTH_TYPE          (DWC_GDB_SSL_TYPE_SERVER_AUTH)

#define MATCHINGKEY (1)

///GDSサーバー(製品版)
#define GF_GDS_SERVER_URL     ("pkgdsprod.nintendo.co.jp")
#define GF_GDS_SERVER_PORT    (12401)

#endif  //---------- DEBUG_SERVER

//不思議な贈り物でしようするパスワード
#ifdef USE_AUTHSERVER_RELEASE
#define WIFI_ND_LOGIN_PASSWD		"fZrUY6SVMLp4tdmB"		// パスワード(本番サーバ)
#else
#define WIFI_ND_LOGIN_PASSWD		"JDXNcp79mr54RUga"    // パスワード(テストサーバ)
#endif


#ifdef DEBUG_SERVER
#define LIBDPW_SERVER_TYPE ( DPW_SERVER_DEBUG )   ///< PDW接続先(デバッグ版)
#else
#define LIBDPW_SERVER_TYPE ( DPW_SERVER_RELEASE )   ///< PDW接続先 （リリース用）
#endif


//==============================================================================
//  定義
//==============================================================================

extern void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo,void* pWork);
extern u32 GFLR_NET_GetGGID(void);

//==============================================================================
//  型宣言
//==============================================================================

/// ゲームの種類を区別する為の定義   初期化構造体のgsidとして渡してください
typedef enum  {
  WB_NET_NOP_SERVICEID = 0,
  WB_NET_BATTLE_SERVICEID,       ///< バトル用ID
  WB_NET_PALACE_SERVICEID,       ///< パレス
  WB_NET_FIELDMOVE_SERVICEID,  ///< サンプルのフィールド移動
  WB_NET_MULTIBOOT_SERVICEID,  ///< ダウンロード通信のボックス取得
  WB_NET_DEBUG_OHNO_SERVICEID,
  WB_NET_SERVICEID_DEBUG_BATTLE,
  WB_NET_DEBUG_MATSUDA_SERVICEID,
  WB_NET_TRADE_SERVICEID,     ///<ポケモン交換
  WB_NET_MYSTERY,         ///<不思議な贈り物
  WB_NET_WIFICLUB,                 ///<WIFICLUB
  WB_NET_COMPATI_CHECK,       ///<相性チェック（テスト版）
  WB_NET_IRCBATTLE,               ///< IRC>WIRELESS>接続
  WB_NET_IRCCOMPATIBLE,     ///相性チェック
  WB_NET_IRCAURA,         ///相性チェック オーラチェック
  WB_NET_IRCRHYTHM,       ///相性チェック　RHYTHMチェック
  WB_NET_IRCMENU,       ///相性チェック　RHYTHMチェック
  WB_NET_MUSICAL,       ///ミュージカル      （IRC親機＋無線共通）        順番変更禁止
  WB_NET_MUSICAL_CHILD,       ///ミュージカル  (IRC子機)         順番変更禁止
  WB_NET_MUSICAL_LEADER,       ///ミュージカル  (IRC認証)         順番変更禁止
  WB_NET_UNION,                             ///<ユニオンルーム       ユニオンは順番で見ている部分がある wihdwc cg_wirlessmenu
  WB_NET_COLOSSEUM,       ///<コロシアム
  WB_NET_UNION_PICTURE,                     ///<お絵かき
  WB_NET_UNION_BATTLE_MULTI_FREE_SHOOTER,   ///<マルチ(シューター有)
  WB_NET_UNION_BATTLE_MULTI_FREE,           ///<マルチ
  WB_NET_UNION_BATTLE_MULTI_FLAT_SHOOTER,   ///<マルチフラット(シューター有)
  WB_NET_UNION_BATTLE_MULTI_FLAT,           ///<マルチフラット
  WB_NET_UNION_GURUGURU,                    ///<ぐるぐる交換
  WB_NET_IRCTRADE,       ///<赤外線ぽけ交換
  WB_NET_IRCFRIEND,       ///<赤外線ともだち交換
  WB_NET_WIFIGTS,         ///<GTS
  WB_NET_IRC_BATTLE,      ///<赤外線バトル・(大会用)
  WB_NET_COMM_TVT,         ///<通信TVT
  WB_NET_GDS,              ///<GDS
  WB_NET_GTSNEGO,          ///< GTSネゴシエーション
	WB_NET_DEBUG_BEACON_FRIENDCODE,	///<デバッグ　ビーコンでの友達コード交換
  WB_NET_WIFIMATCH,         ///<WIFI世界対戦
  WB_NET_IRCBATTLE_MULTI,  ///< 赤外線バトルマルチ
  WB_NET_IRCBATTLE_MULTI_IRC,  ///< 赤外線バトルマルチ赤外線
  WB_NET_COMM_ENTRY_MENU,   ///<通信エントリーメニュー
  WB_NET_GURUGURU,        ///<ぐるぐる交換
  WB_NET_PICTURE,         ///<お絵かき
  WB_NET_BATTLE_ADD_CMD,  ///< 通信バトル コマンド送信
  WB_NET_BTL_REC_SEL,      ///<通信対戦後の録画選択画面
  WB_NET_BSUBWAY,         ///<バトルサブウェイ
  WB_NET_POKELIST,         ///<ポケモンリスト(終了時同期コマンド用
  WB_NET_BATTLE_EXAMINATION,  ///<バトル検定配信
  WB_NET_WIFIP2P_SUB,  ///< WIFIバトルの中間
  WB_NET_IRC_REGULATION,  ///<ライブ大会レギュレーション配信
  WB_NET_GAMESYNC,  ///<GAMESYNC
  WB_NET_PDW_ACC,  ///<GAMESYNCアカウント
  WB_NET_GTS_NEGO,  ///<GTSネゴシエーション
  WB_NET_WIFIRNDMATCH,  ///<WIFIランダムマッチ（ゲームサービスIDをレコードカウントのためWIFIMATCHとわけることになった）
  
  WB_NET_SERVICEID_MAX   // 終端
} NetworkServiceID_e;

///通信コマンドを区別する為の定義  コールバックテーブルを渡すときにIDとして定義してください
/// 上の定義と同じ並びで書いてください
enum NetworkCommandHeaderNo_e {
  GFL_NET_CMD_BASE = (WB_NET_NOP_SERVICEID<<8),  ///< ベースコマンド開始番号
  GFL_NET_CMD_BATTLE = (WB_NET_BATTLE_SERVICEID<<8),   ///< バトル開始番号
  GFL_NET_CMD_PALACE = (WB_NET_PALACE_SERVICEID<<8),   ///< パレスのベース開始番号
  GFL_NET_CMD_FIELD = (WB_NET_FIELDMOVE_SERVICEID<<8),   ///< フィールドのベース開始番号
  GFL_NET_CMD_MULTIBOOT = (WB_NET_MULTIBOOT_SERVICEID<<8),   ///< フィールドのベース開始番号
  GFL_NET_CMD_DEBUG_OHNO = (WB_NET_DEBUG_OHNO_SERVICEID<<8),
  GFL_NET_CMD_DEBUG_BATTLE = (WB_NET_SERVICEID_DEBUG_BATTLE<<8),
  GFL_NET_CMD_DEBUG_MATSUDA = (WB_NET_DEBUG_MATSUDA_SERVICEID<<8),
  GFL_NET_CMD_TRADE = (WB_NET_TRADE_SERVICEID<<8),      ///<ポケモン交換の開始番号
  GFL_NET_CMD_MYSTERY = (WB_NET_MYSTERY<<8),        ///<不思議な贈り物
  GFL_NET_CMD_WIFICLUB = (WB_NET_WIFICLUB<<8),        ///<WIFICLUB
  GFL_NET_CMD_COMPATI_CHECK = (WB_NET_COMPATI_CHECK<<8),  ///<相性チェック（テスト版）
  GFL_NET_CMD_IRCBATTLE = (WB_NET_IRCBATTLE<<8),  ///< IRC>WIRELESS>接続
  GFL_NET_CMD_IRCCOMPATIBLE = (WB_NET_IRCCOMPATIBLE<<8),  ///<相性チェック
  GFL_NET_CMD_IRCAURA = (WB_NET_IRCAURA<<8),  ///<相性チェック　オーラチェック
  GFL_NET_CMD_IRCRHYTHM = (WB_NET_IRCRHYTHM<<8),  ///<相性チェック　リズムチェック
  GFL_NET_CMD_IRCMENU = (WB_NET_IRCMENU<<8),  ///<相性チェック　メニュー
  GFL_NET_CMD_MUSICAL = (WB_NET_MUSICAL<<8),  ///ミュージカル
  GFL_NET_CMD_UNION = (WB_NET_UNION<<8),      ///<ユニオンルーム
  GFL_NET_CMD_COLOSSEUM = (WB_NET_COLOSSEUM<<8),      ///<コロシアム
  GFL_NET_CMD_UNION_PICTURE = (WB_NET_UNION_PICTURE),                     ///<お絵かき
  GFL_NET_CMD_UNION_BATTLE_MULTI_FREE_SHOOTER = (WB_NET_UNION_BATTLE_MULTI_FREE_SHOOTER), ///<マルチ(シューター有)
  GFL_NET_CMD_UNION_BATTLE_MULTI_FREE = (WB_NET_UNION_BATTLE_MULTI_FREE),  ///<マルチ
  GFL_NET_CMD_UNION_BATTLE_MULTI_FLAT_SHOOTER = (WB_NET_UNION_BATTLE_MULTI_FLAT_SHOOTER), ///<マルチフラット(シューター有)
  GFL_NET_CMD_UNION_BATTLE_MULTI_FLAT = (WB_NET_UNION_BATTLE_MULTI_FLAT),  ///<マルチフラット
  GFL_NET_CMD_UNION_GURUGURU = (WB_NET_UNION_GURUGURU),                    ///<ぐるぐる交換
  GFL_NET_CMD_IRCTRADE = (WB_NET_IRCBATTLE<<8),  ///< IRCぽけこうかん
  GFL_NET_CMD_IRCFRIEND = (WB_NET_IRCFRIEND<<8),  ///< IRCともだちこうかん
  GFL_NET_CMD_WIFIGTS   = (WB_NET_WIFIGTS<<8),    ///< GTS
  GFL_NET_CMD_IRC_BATTLE    = (WB_NET_IRC_BATTLE<<8),   ///< IRC
  GFL_NET_CMD_COMM_TVT   = (WB_NET_COMM_TVT<<8),    ///< 通信TVT
  GFL_NET_CMD_GDS   = (WB_NET_GDS<<8),    ///< GDS
  GFL_NET_CMD_GTSNEGO   = (WB_NET_GTSNEGO<<8),    ///< GTSネゴシエーション
	GFL_NET_CMD_DEBUG_BEACON_FRIENDCODE	= (WB_NET_DEBUG_BEACON_FRIENDCODE<<8),	///<デバッグ　ビーコンでの友達コード交換
  GFL_NET_CMD_WIFIMATCH = (WB_NET_WIFIMATCH<<8),         ///<WIFI世界対戦
  GFL_NET_CMD_IRCBATTLE_MULTI = (WB_NET_IRCBATTLE_MULTI<<8),         ///<赤外線バトルマルチ
  GFL_NET_CMD_COMM_ENTRY_MENU = (WB_NET_COMM_ENTRY_MENU<<8),  ///<通信エントリーメニュー
  GFL_NET_CMD_GURUGURU = (WB_NET_GURUGURU << 8),      ///<ぐるぐる交換
  GFL_NET_CMD_PICTURE = (WB_NET_PICTURE << 8),        ///<お絵かき
  GFL_NET_CMD_BSUBWAY = (WB_NET_BSUBWAY << 8),  ///<バトルサブウェイ
  GFL_NET_CMD_POKELIST = (WB_NET_POKELIST << 8),///<ポケモンリスト(終了時同期コマンド用
  GFL_NET_CMD_BATTLE_EXAMINATION = (WB_NET_BATTLE_EXAMINATION<<8),  ///<バトル検定配信
  GFL_NET_CMD_WIFIP2P_SUB = (WB_NET_WIFIP2P_SUB<<8), ///< WIFIバトルの中間
  GFL_NET_CMD_IRC_REGULATION  = (WB_NET_IRC_REGULATION<<8),  ///<ライブ大会レギュレーション配信
};


/// 通信送信基本サイズ 同じゲームが複数ある場合に統一した速度になるように指定してある
/// ４人でのゲームの想定
#define NET_SEND_SIZE_STANDARD (96)


#if 0
//>┌───┬──────────┬───────────┬─────────┬────┐
//>│モード│                    │送るデータ形式        │送るポケモン最大数│バイト数│
//>├───┼──────────┼───────────┼─────────┼────┤
//>│0     │ランダムマッチ      │POKEMON_PARAM         │6                 │220     │
//>├───┼──────────┼───────────┼─────────┼────┤
//>│1     │GTS                 │POKEMON_PARAM         │1                 │220     │
//>├───┼──────────┼───────────┼─────────┼────┤
//>│2     │GTSネゴシエーション │POKEMON_PARAM         │3                 │220     │
//>├───┼──────────┼───────────┼─────────┼────┤
//>│3     │バトルビデオ        │POKE_BATTLEVIDEO_PARAM│12                │108     │
//>├───┼──────────┼───────────┼─────────┼────┤
//>│4     │バトルサブウェイ    │_B_TOWER_POKEMON      │3                 │60      │
//>└───┴──────────┴───────────┴─────────┴────┘
#endif

typedef enum{
  NHTTP_POKECHK_RANDOMMATCH = 0,//ランダムマッチ      │POKEMON_PARAM         │6                 │220     │
  NHTTP_POKECHK_GTS,   //GTS                 │POKEMON_PASO_PARAM    │1                 │136     │
  NHTTP_POKECHK_GTSNEGO,       // GTSネゴシエーション │POKEMON_PASO_PARAM    │3                 │136     │
  NHTTP_POKECHK_VIDIO,   //     │バトルビデオ        │POKE_BATTLEVIDEO_PARAM│12                │108     │
  NHTTP_POKECHK_SUBWAY,  // │バトルサブウェイ    │_B_TOWER_POKEMON      │3                 │56      │
} NHTTP_POKECHK_ENUM;

#include "net_irc.h"

/// WiFiで使うHeapのサイズ
#define GFL_NET_DWC_HEAPSIZE        (0x45400)                           //WIFIクラブのHEAP量
#define GFL_NET_DWCLOBBY_HEAPSIZE   (0x60000)                           //WIFI広場のHEAP量
#define GFL_NET_DWC_BSUBWAY_HEAPSIZE  (0x50000)                        //バトルサブウェイのHEAP量
#define GFL_NET_DWC_RNDMATCH_HEAPSIZE (0x55000)                       //ランダムマッチングのHEAP量


#define GAMESYNC_CGEAR_DOWNLOAD "CGEAR_J"
#define GAMESYNC_MUS_DOWNLOAD "MUSICAL_J"
#define GAMESYNC_ZUK_DOWNLOAD "ZUKAN_J"

