//==============================================================================
/**
 * @file	gds_rap.h
 * @brief	GDSライブラリをラッパーしたもの
 * @author	matsuda
 * @date	2008.01.09(水)
 */
//==============================================================================
#ifndef __GDS_RAP_H__
#define __GDS_RAP_H__

#pragma once

#include <dwc.h>
#include "pm_define.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/poke_tool_def.h"
#include "net_app/gds/gds_battle_mode.h"
#include "print/wordset.h"


#include "battle\btl_common.h"
#include "savedata\battle_rec.h"
#include "savedata/battle_rec_local.h"
#include "musical\musical_define.h"
#include "net_app/gds/gds_profile_local.h"
#define __GT_GDS_DEFINE_H__
#define __GT_GDS_PROFILE_LOCAL_H__
#define __GT_GDS_BATTLE_REC_SUB_H__
//#define __GT_GDS_BATTLE_REC_H__
#define __GT_GDS_MUSICAL_SUB_H__
//#define __GT_GDS_MUSICAL_H__
#define __GDS_MIN_MAX_H__
#define ___POKE_NET_BUILD_DS___
#include <poke_net_gds.h>


// サーバのURLまたはIPアドレスとポート番号を定義
#define SYACHI_SERVER_URL	("pkgdstest.nintendo.co.jp")
#define SYACHI_SERVER_PORT	(12401)


//==============================================================================
//	定数定義
//==============================================================================
///GDSエラータイプ
enum{
	GDS_ERROR_TYPE_LIB,			///<ライブラリエラー(POKE_NET_GDS_LASTERROR)
	GDS_ERROR_TYPE_STATUS,		///<ステータスエラー(POKE_NET_GDS_STATUS)
	GDS_ERROR_TYPE_APP,			///<各アプリ毎のエラー
};

///バトルモードNo   ※BattleModeBitTblと並びを同じにしておくこと！！
typedef enum{
  BATTLE_MODE_NO_COLOSSEUM_SINGLE_NOSHOOTER,    ///<コロシアム シングル シューター無し
  BATTLE_MODE_NO_COLOSSEUM_SINGLE_SHOOTER,      ///<コロシアム シングル シューターあり
  BATTLE_MODE_NO_COLOSSEUM_DOUBLE_NOSHOOTER,    ///<コロシアム ダブル シューター無し
  BATTLE_MODE_NO_COLOSSEUM_DOUBLE_SHOOTER,      ///<コロシアム ダブル シューターあり
  BATTLE_MODE_NO_COLOSSEUM_TRIPLE_NOSHOOTER,    ///<コロシアム トリプル シューター無し
  BATTLE_MODE_NO_COLOSSEUM_TRIPLE_SHOOTER,      ///<コロシアム トリプル シューターあり
  BATTLE_MODE_NO_COLOSSEUM_ROTATION_NOSHOOTER,  ///<コロシアム ローテーション シューター無し
  BATTLE_MODE_NO_COLOSSEUM_ROTATION_SHOOTER,    ///<コロシアム ローテーション シューターあり
  BATTLE_MODE_NO_COLOSSEUM_MULTI_NOSHOOTER,     ///<コロシアム マルチ シューター無し
  BATTLE_MODE_NO_COLOSSEUM_MULTI_SHOOTER,       ///<コロシアム マルチ シューターあり
  BATTLE_MODE_NO_SUBWAY_SINGLE,                 ///<地下鉄 シングル
  BATTLE_MODE_NO_SUBWAY_DOUBLE,                 ///<地下鉄 ダブル
  BATTLE_MODE_NO_SUBWAY_MULTI,                  ///<地下鉄 マルチ
  BATTLE_MODE_NO_RANDOM_SINGLE,                 ///<ランダムマッチ シングル
  BATTLE_MODE_NO_RANDOM_DOUBLE,                 ///<ランダムマッチ ダブル
  BATTLE_MODE_NO_RANDOM_TRIPLE,                 ///<ランダムマッチ トリプル
  BATTLE_MODE_NO_RANDOM_ROTATION,               ///<ランダムマッチ ローテーション
  BATTLE_MODE_NO_RANDOM_SHOOTER,                ///<ランダムマッチ シューターバトル
  BATTLE_MODE_NO_CONTEST,                       ///<バトル大会
}GDS_BATTLE_MODE_NO;


//==============================================================================
//	構造体定義
//==============================================================================

///送信パラメータ構造体
typedef struct{
	//送信データ
	union{
    MUSICAL_SHOT_SEND mus_send; ///<ミュージカルショット送信データ
		BATTLE_REC_SEND *battle_rec_send_ptr;	///<送信データへのポインタ
		BATTLE_REC_SEARCH_SEND battle_rec_search;	///<ビデオ検索送信データ
	};

	//送信データに付随するオプションパラメータ
	union{
		//ミュージカルショットのサブパラメータ
		struct{
			u16 recv_monsno;	///<取得するカテゴリー番号(ポケモン番号)
			u16 mus_padding;
		}musical;
		//バトルビデオのサブパラメータ
		struct{
			u64 data_number;	///<データナンバー
		}rec;
	}sub_para;
}GDS_RAP_SEND_WORK;

///GDSラッパーワークのサブワーク
typedef struct _GDS_RAP_SUB_PROCCESS_WORK{
	int local_seq;
	int wait;
	int work;
}GDS_RAP_SUB_PROCCESS_WORK;

///GDSライブラリのエラー情報管理構造体
typedef struct _GDS_RAP_ERROR_INFO{
	BOOL occ;				///<TRUE:エラー情報あり。　FALSE:エラー情報無し
	int type;				///<エラータイプ(GDS_ERROR_TYPE_???)
	int req_code;			///<リクエストコード(POKE_NET_RESPONSE.ReqCode)
	int result;				///<結果(POKE_NET_RESPONSE.Result)
}GDS_RAP_ERROR_INFO;

///データ受信時のコールバック関数の型
typedef void (*GDSRAP_RESPONSE_FUNC)(void *, const GDS_RAP_ERROR_INFO *);
///通信エラーメッセージ表示のコールバック関数の型
typedef void (*GDSRAP_ERROR_WIDE_MSG_FUNC)(void *, STRBUF *);

///データ受信時のコールバック関数などをまとめた構造体
typedef struct{
	void *callback_work;	///<データ受信時のコールバック関数に引数として渡すポインタ

	///データ受信時のコールバック関数へのポインタ
	///ミュージカルショット登録(POKE_NET_GDS_REQCODE_MUSICALSHOT_REGIST)
	GDSRAP_RESPONSE_FUNC func_musicalshot_regist;
	
	///データ受信時のコールバック関数へのポインタ
	///ドレスアップショット取得(POKE_NET_GDS_REQCODE_MUSICALSHOT_GET)
	GDSRAP_RESPONSE_FUNC func_musicalshot_get;
	
	///データ受信時のコールバック関数へのポインタ
	///バトルビデオ登録(POKE_NET_GDS_REQCODE_BATTLEDATA_REGIST)
	GDSRAP_RESPONSE_FUNC func_battle_video_regist;

	///データ受信時のコールバック関数へのポインタ
	///バトルビデオ検索(詳細検索、最新、お気に入りランキング)(POKE_NET_GDS_REQCODE_BATTLEDATA_SEARCH)
	GDSRAP_RESPONSE_FUNC func_battle_video_search_get;

	///データ受信時のコールバック関数へのポインタ
	///バトルビデオデータ取得(POKE_NET_GDS_REQCODE_BATTLEDATA_GET)
	GDSRAP_RESPONSE_FUNC func_battle_video_data_get;

	///データ受信時のコールバック関数へのポインタ
	///バトルビデオお気に入り登録(POKE_NET_GDS_REQCODE_BATTLEDATA_FAVORITE)
	GDSRAP_RESPONSE_FUNC func_battle_video_favorite_regist;
}GDS_RAP_RESPONSE_CALLBACK;

///データ受信後に行うサブプロセス用のコールバック関数の型
typedef BOOL (*GDSRAP_RECV_SUB_PROCCESS_FUNC)(void *, void *);

///データ受信後に行うサブプロセス用ワーク
typedef struct _GDS_RECV_SUB_PROCCESS_WORK{
	GDSRAP_RESPONSE_FUNC user_callback_func;	///<データ受信後に呼び出すのコールバック関数
	u16 recv_save_seq0;
	u16 recv_save_seq1;
	u8 recv_sub_seq;
	GDSRAP_RECV_SUB_PROCCESS_FUNC recv_sub_proccess;
}GDS_RECV_SUB_PROCCESS_WORK;

///GDSライブラリ、NitroDWCに近い関係のワークの構造体
typedef struct _GDS_RAP_WORK{
	GAMEDATA *gamedata;
	SAVE_CONTROL_WORK *savedata;
	
	//アプリから渡される重要データ
	POKE_NET_REQUESTCOMMON_AUTH pokenet_auth;
	int heap_id;
	
	POKE_NET_GDS_STATUS stat;
	POKE_NET_GDS_STATUS laststat;
	
	GDS_RAP_RESPONSE_CALLBACK response_callback;	///データ受信時のコールバック関数
	GDS_RAP_ERROR_INFO error_info;			///<エラー情報格納用ワーク
	
	//※check　暫定
//	void *areanaLo;
	
	//送信データ
	GDS_RAP_SEND_WORK send_buf;				///<送信データ
	//受信データ
	void *response;		///<POKE_NET_GDS_RESPONSE_RESULT_DRESSUPSHOT_REGIST_xxxxが受信されます
	
	//内部使用ワーク
	int comm_initialize_ok;		///<TRUE:通信ライブラリ初期化済み
	BOOL gdslib_initialize;			///<TRUE:GDSライブラリ初期化済み
	BOOL connect_success;					///<TRUE:ネット接続中
	GDS_RAP_SUB_PROCCESS_WORK sub_work;		///<サブプロセス制御用ワーク

	int ConnectErrorNo;						///< DWC・またはサーバーからのエラー
	int ErrorRet;
	int ErrorCode;
	int ErrorType;
	
	int send_req;	///<POKE_NET_GDS_REQCODE_???(リクエスト無しの場合はPOKE_NET_GDS_REQCODE_LAST)
	int recv_wait_req;///<send_reqの結果受信データ待ち(POKE_NET_GDS_REQCODE_???)
	int send_req_option;	///<POKE_NET_GDS_REQCODE_???でさらに検索を分ける場合のオプション

	GFL_MSGDATA *msgman_wifisys;		///<メッセージデータマネージャのポインタ
	WORDSET *wordset;						///<Allocしたメッセージ用単語バッファへのポインタ
	STRBUF *ErrorString;					///<文字列展開バッファ

	GDS_RECV_SUB_PROCCESS_WORK recv_sub_work;	///<データ受信後のサブプロセス用ワーク
	
	u8 div_save_seq;			///<分割セーブ実行シーケンス
	u8 send_before_wait;		///<TRUE:送信前にメッセージを見せる為の一定ウェイト
}GDS_RAP_WORK;


///GDSRAP初期化時に引き渡す引数
typedef struct{
	int gs_profile_id;		///<GSプロファイルID
	
	int heap_id;
	GAMEDATA *gamedata;
	
	GDS_RAP_RESPONSE_CALLBACK response_callback;	///データ受信時のコールバック関数
}GDSRAP_INIT_DATA;


//==============================================================================
//	外部関数宣言
//==============================================================================
//--------------------------------------------------------------
//	メイン
//--------------------------------------------------------------
extern int GDSRAP_Init(GDS_RAP_WORK *gdsrap, const GDSRAP_INIT_DATA *init_data);
extern int GDSRAP_Main(GDS_RAP_WORK *gdsrap);
extern void GDSRAP_Exit(GDS_RAP_WORK *gdsrap);

//--------------------------------------------------------------
//	送信系
//--------------------------------------------------------------
extern int GDSRAP_Tool_Send_MusicalShotUpload(GDS_RAP_WORK *gdsrap, GDS_PROFILE_PTR gpp, const MUSICAL_SHOT_DATA *musshot);
extern int GDSRAP_Tool_Send_MusicalShotDownload(GDS_RAP_WORK *gdsrap, int monsno);
extern int GDSRAP_Tool_Send_BattleVideoUpload(GDS_RAP_WORK *gdsrap, GDS_PROFILE_PTR gpp);
extern int GDSRAP_Tool_Send_BattleVideoSearchDownload(GDS_RAP_WORK *gdsrap, u16 monsno, GDS_BATTLE_MODE_NO battle_mode_no, u8 country_code, u8 local_code);
extern int GDSRAP_Tool_Send_BattleVideoNewDownload(GDS_RAP_WORK *gdsrap);
extern int GDSRAP_Tool_Send_BattleVideoSubwayDownload(GDS_RAP_WORK *gdsrap);
extern int GDSRAP_Tool_Send_BattleVideoCommDownload(GDS_RAP_WORK *gdsrap);
extern int GDSRAP_Tool_Send_BattleVideo_DataDownload(GDS_RAP_WORK *gdsrap, u64 data_number);
extern int GDSRAP_Tool_Send_BattleVideo_FavoriteUpload(GDS_RAP_WORK *gdsrap, u64 data_number);

//--------------------------------------------------------------
//	処理系
//--------------------------------------------------------------
extern BOOL GDSRAP_MoveStatusAllCheck(GDS_RAP_WORK *gdsrap);

//--------------------------------------------------------------
//	エラー
//--------------------------------------------------------------
extern BOOL GDSRAP_ErrorInfoGet(GDS_RAP_WORK *gdsrap, GDS_RAP_ERROR_INFO **error_info);
extern void GDSRAP_ErrorInfoClear(GDS_RAP_WORK *gdsrap);

//--------------------------------------------------------------
//	デバッグ用
//--------------------------------------------------------------
extern void DEBUG_GDSRAP_SaveFlagReset(GDS_RAP_WORK *gdsrap);
extern void DEBUG_GDSRAP_SendVideoProfileFreeWordSet(GDS_RAP_WORK *gdsrap, u16 *set_code);

#endif	//__GDS_RAP_H__
