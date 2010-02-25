//============================================================================================
/**
 * @file  leader_board.c
 * @brief リーダーボード画面（Wifiバトルサブウェイダウンロードデータ）
 * @author  Akito Mori / Hiroyuki Nakamura
 * @date  09.12.03
 */
//============================================================================================
#ifndef __LEADER_BOARD_H__
#define __LEADER_BOARD_H__

#include "gamesystem/game_data.h"
#include "savedata/bsubway_savedata.h"

//============================================================================================
//  定数定義
//============================================================================================

// 外部設定データ
typedef struct {
  GAMEDATA  *gamedata;  // セーブデータ
  int       rank_no;    // ランクNO
  int       train_no;   // トレインNO
//  BSUBWAY_WIFI_DATA   *bSubwayWifiData;   // バトルサブウェイデータ
//  BSUBWAY_LEADER_DATA *bSubwayLeaderData; // バトルサブウェイリーダーデータ
}LEADERBOARD_PARAM;



//============================================================================================
//  プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * プロセス関数：初期化
 *
 * @param proc  プロセスデータ
 * @param seq   シーケンス
 *
 * @return  処理状況
 */
//--------------------------------------------------------------------------------------------
extern GFL_PROC_RESULT LeaderBoardProc_Init( GFL_PROC * proc, int *seq, void *pwk, void *mywk );

//--------------------------------------------------------------------------------------------
/**
 * プロセス関数：メイン
 *
 * @param proc  プロセスデータ
 * @param seq   シーケンス
 *
 * @return  処理状況
 */
//--------------------------------------------------------------------------------------------
extern GFL_PROC_RESULT LeaderBoardProc_Main( GFL_PROC * proc, int *seq, void *pwk, void *mywk );

//--------------------------------------------------------------------------------------------
/**
 * プロセス関数：終了
 *
 * @param proc  プロセスデータ
 * @param seq   シーケンス
 *
 * @return  処理状況
 */
//--------------------------------------------------------------------------------------------
extern GFL_PROC_RESULT LeaderBoardProc_End( GFL_PROC * proc, int *seq, void *pwk, void *mywk );



extern const GFL_PROC_DATA LeaderBoardProcData;
FS_EXTERN_OVERLAY(leader_board);

#endif  // __LEADER_BOARD_H__
