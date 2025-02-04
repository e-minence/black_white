//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery_net.h
 *	@brief  ふしぎなおくりもの通信処理
 *	@author	Toru=Nagihashi
 *	@data		2009.12.15
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#include "gamesystem/game_comm.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	通信システムへの状態リクエスト
//=====================================
typedef enum
{
  MYSTERY_NET_STATE_WAIT, //待機中

  //ビーコン
  MYSTERY_NET_STATE_START_BEACON,   //START終了時、MAINへ勝手に状態遷移
  MYSTERY_NET_STATE_MAIN_BEACON,    //リクエストは受付ない
  MYSTERY_NET_STATE_END_BEACON,     //ビーコン終了

  //WiFiダウンロード
  MYSTERY_NET_STATE_WIFI_DOWNLOAD,        //WIFIで受け取り
  MYSTERY_NET_STATE_CANCEL_WIFI_DOWNLOAD, //ダウンロードのキャンセル
  MYSTERY_NET_STATE_LOGOUT_WIFI, //WIFIログアウト  ( ログインは外部で行うため　)

  //ビーコン受け取り
  MYSTERY_NET_STATE_START_BEACON_DOWNLOAD,  //START終了時、MAINへ勝手に状態遷移
  MYSTERY_NET_STATE_MAIN_BEACON_DOWNLOAD,   //受け取り時、ENDへ勝手に状態遷移 リクエストは受付ない
  MYSTERY_NET_STATE_END_BEACON_DOWNLOAD,    //END終了時、WAITへ勝手に状態遷移 

  //赤外線受け取り
  MYSTERY_NET_STATE_START_IRC_DOWNLOAD,  //START終了時、MAINへ勝手に状態遷移
  MYSTERY_NET_STATE_MAIN_IRC_DOWNLOAD,   //受け取り時、ENDへ勝手に状態遷移 リクエストは受付ない
  MYSTERY_NET_STATE_END_IRC_DOWNLOAD,    //END終了時、WAITへ勝手に状態遷移

} MYSTERY_NET_STATE;

//-------------------------------------
///	受信状態
//=====================================
typedef enum
{
  MYSTERY_NET_RECV_STATUS_NONE,     //何も受信していない
  MYSTERY_NET_RECV_STATUS_SUCCESS,  //受信成功
  MYSTERY_NET_RECV_STATUS_FAILED,   //失敗
} MYSTERY_NET_RECV_STATUS;


//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	ふしぎなおくりもの通信ワーク
//=====================================
typedef struct _MYSTERY_NET_WORK  MYSTERY_NET_WORK;

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//初期化。破棄、メイン処理
extern MYSTERY_NET_WORK * MYSTERY_NET_Init( const SAVE_CONTROL_WORK *cp_sv, HEAPID heapID );
extern void MYSTERY_NET_Exit( MYSTERY_NET_WORK *p_wk );
extern void MYSTERY_NET_Main( MYSTERY_NET_WORK *p_wk );

//状態変移.状態取得
extern void MYSTERY_NET_ChangeStateReq( MYSTERY_NET_WORK *p_wk, MYSTERY_NET_STATE state );
extern MYSTERY_NET_STATE MYSTERY_NET_GetState( const MYSTERY_NET_WORK *cp_wk );

//現在の通信を検知
extern GAME_COMM_STATUS_BIT MYSTERY_NET_GetCommStatus( const MYSTERY_NET_WORK *cp_wk );

//ダウンロードしたデータうけとり
extern MYSTERY_NET_RECV_STATUS MYSTERY_NET_GetDownloadData( const MYSTERY_NET_WORK *cp_wk, void *p_data, u32 size );

//エラー修復戻り先
typedef enum
{ 
  MYSTERY_NET_ERROR_REPAIR_NONE,        //エラーなし
  MYSTERY_NET_ERROR_REPAIR_RETURN,      //１つ前の選択肢まで戻る
  MYSTERY_NET_ERROR_REPAIR_DISCONNECT,  //切断する
  MYSTERY_NET_ERROR_REPAIR_FATAL,       //電源切断
}MYSTERY_NET_ERROR_REPAIR_TYPE;
extern MYSTERY_NET_ERROR_REPAIR_TYPE MYSTERY_NET_GetErrorRepairType( const MYSTERY_NET_WORK *cp_wk );
extern void MYSTERY_NET_ClearError( MYSTERY_NET_WORK *p_wk );
