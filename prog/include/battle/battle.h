//=============================================================================================
/**
 * @file  battle.h
 * @brief ポケモンWB バトルシステム 外部システムとのインターフェイス
 * @author  taya
 *
 * @date  2008.09.22  作成
 */
//=============================================================================================
#ifndef __BATTLE_H__
#define __BATTLE_H__

#include <net.h>
#include <procsys.h>

#include "system/timezone.h"
#include "poke_tool/pokeparty.h"
#include "tr_tool/trno_def.h"
#include "savedata/zukan_savedata.h"
#include "savedata/mystatus.h"
#include "savedata/myitem_savedata.h"

#include "battle_bg_def.h"  //zonetableコンバータから参照させたいので定義を分離しました by iwasawa

//--------------------------------------------------------------
/**
 *  対戦ルール
 */
//--------------------------------------------------------------
typedef enum {

  BTL_RULE_SINGLE,    ///< シングル
  BTL_RULE_DOUBLE,    ///< ダブル
  BTL_RULE_TRIPLE,    ///< トリプル
  BTL_RULE_ROTATION,  ///< ローテーション

}BtlRule;

//--------------------------------------------------------------
/**
 *  対戦相手
 */
//--------------------------------------------------------------
typedef enum {

  BTL_COMPETITOR_WILD,    ///< 野生
  BTL_COMPETITOR_TRAINER,   ///< ゲーム内トレーナー
  BTL_COMPETITOR_COMM,    ///< 通信対戦

}BtlCompetitor;

//--------------------------------------------------------------
/**
 *  通信タイプ
 */
//--------------------------------------------------------------
typedef enum {

  BTL_COMM_NONE,  ///< 通信しない
  BTL_COMM_DS,    ///< DS無線通信
  BTL_COMM_WIFI,  ///< Wi-Fi通信

}BtlCommMode;


//--------------------------------------------------------------
/**
 *  天候
 */
//--------------------------------------------------------------
typedef enum {

  BTL_WEATHER_NONE = 0,   ///< 天候なし

  BTL_WEATHER_SHINE,    ///< はれ
  BTL_WEATHER_RAIN,     ///< あめ
  BTL_WEATHER_SNOW,     ///< あられ
  BTL_WEATHER_SAND,     ///< すなあらし
  BTL_WEATHER_MIST,     ///< きり

  BTL_WEATHER_MAX,

}BtlWeather;

//--------------------------------------------------------------
/**
 *  勝敗コード
 */
//--------------------------------------------------------------
typedef enum {

  BTL_RESULT_LOSE,        ///< 負けた
  BTL_RESULT_WIN,         ///< 勝った
  BTL_RESULT_DRAW,        ///< ひきわけ
  BTL_RESULT_RUN,         ///< 逃げた
  BTL_RESULT_RUN_ENEMY,   ///< 相手が逃げた（野生のみ）
  BTL_RESULT_CAPTURE,     ///< 捕まえた（野生のみ）

}BtlResult;

//-----------------------------------------------------------------------------------
/**
 *  地形
 */
//-----------------------------------------------------------------------------------
typedef struct {

  //バトルルール
  BtlCompetitor   competitor;
  BtlRule         rule;

  //フィールドの状態から決定されるバトルシチュエーションデータ
  BtlBgType       bgType;
  BtlLandForm     landForm;
  BtlWeather      weather;
  TIMEZONE        timezone;
  u16             musicDefault;   ///< デフォルト時のBGMナンバー
  u16             musicPinch;     ///< ピンチ時のBGMナンバー

  //通信データ
  GFL_NETHANDLE*  netHandle;
  BtlCommMode     commMode;
  u8              commPos;    ///< 通信対戦なら自分の立ち位置（非通信時は無視）
  u8              netID;      ///< NetID
  u8              multiMode;  ///< ダブルの時、１だとマルチバトル。

  //対戦データ
  POKEPARTY*      partyPlayer;  ///< プレイヤーのパーティ
  POKEPARTY*      partyPartner; ///< 2vs2時の味方AI（不要ならnull）
  POKEPARTY*      partyEnemy1;  ///< 1vs1時の敵AI, 2vs2時の１番目敵AI用
  POKEPARTY*      partyEnemy2;  ///< 2vs2時の２番目敵AI用（不要ならnull）
  TrainerID       trID;         ///<対戦相手トレーナーID（7/31ROMでトレーナーエンカウントを実現するための暫定）

  //セーブデータ系
  const MYSTATUS*   statusPlayer; ///< プレイヤーのステータス
  MYITEM*           itemData;     ///< アイテムデータ
  BAG_CURSOR*       bagCursor;    ///< バッグカーソルデータ
  ZUKAN_SAVEDATA*   zukanData;    ///< 図鑑データ

  //----- 以下、バトルの結果格納パラメータ ----

  BtlResult   result;           ///< 勝敗結果
  u8          capturedPokeIdx;  ///< 捕獲したポケモンのメンバー内インデックス（partyEnemy1 内）

}BATTLE_SETUP_PARAM;


//--------------------------------------------------------------
/**
 *  Proc Data
 */
//--------------------------------------------------------------
extern const GFL_PROC_DATA BtlProcData;

// オーバーレイモジュール宣言
FS_EXTERN_OVERLAY(battle);

//--------------------------------------------------------------
/**
 *  NetFunc Table
 */
//--------------------------------------------------------------
extern const NetRecvFuncTable BtlRecvFuncTable[];
enum {
  BTL_NETFUNCTBL_ELEMS = 6,
};


#endif
