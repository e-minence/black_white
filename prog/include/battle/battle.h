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

#include <gflib.h>
#include <net.h>
#include <procsys.h>

#include "system/timezone.h"
#include "system/pms_data.h"
#include "field/field_light_status.h"
#include "poke_tool/pokeparty.h"
#include "tr_tool/trno_def.h"
#include "savedata/zukan_savedata.h"
#include "savedata/mystatus.h"
#include "savedata/myitem_savedata.h"
#include "savedata/config.h"
#include "gamesystem/comm_player_support.h"
#include "gamesystem/playerwork.h"
#include "item/shooter_item.h"

#include "buflen.h"

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
  BTL_COMPETITOR_TRAINER, ///< ゲーム内トレーナー
  BTL_COMPETITOR_SUBWAY,  ///< バトルサブウェイトレーナー
  BTL_COMPETITOR_COMM,    ///< 通信対戦

  BTL_COMPETITOR_DEMO_CAPTURE,  ///< 捕獲デモ

  BTL_COMPETITOR_MAX,

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

}BtlCommMode_tag;

typedef u8 BtlCommMode;

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

  BTL_WEATHER_MAX,

}BtlWeather_tag;

typedef u8 BtlWeather;

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
  BTL_RESULT_COMM_ERROR,  ///< 通信エラーによる

  BTL_RESULT_MAX,

}BtlResult;

typedef enum{
  BTL_CLIENT_PLAYER,
  BTL_CLIENT_ENEMY1,
  BTL_CLIENT_PARTNER,
  BTL_CLIENT_ENEMY2,
  BTL_CLIENT_NUM,
}BTL_CLIENT_ID;

//-----------------------------------------------------------------------------------
/**
 * フィールドの状態から決定されるバトルシチュエーションデータ
 */
//-----------------------------------------------------------------------------------
typedef struct {

  BtlBgType   bgType;
  BtlBgAttr   bgAttr;
  BtlWeather  weather;
  u8          season;

  // ライト設定用パラメータ
  ZONEID      zoneID;
  u8          hour;
  u8          minute;

  u32 dummy;  //旧録画データのサイズに合わせる為のダミー @todo ※check
}BTL_FIELD_SITUATION;

//-----------------------------------------------------------------------------------
/**
 * フィールドからバトルに引き渡すべきフラグ群
 */
//-----------------------------------------------------------------------------------
typedef enum {
  //フラグ数が8をこえるときはBATTLE_SETUP_PARAM->btl_status_flagの型をu8から拡張してください
  BTL_STATUS_FLAG_FISHING = 0x01,     ///<釣り戦闘ならTRUE
  BTL_STATUS_FLAG_PCNAME_OPEN = 0x02, ///<パソコンの名前がオープンされているか？
  BTL_STATUS_FLAG_LEGEND = 0x04,      ///<伝説級ポケモンとのエンカウントならTRUE
  BTL_STATUS_FLAG_MOVE_POKE = 0x08,   ///<移動ポケモンとのエンカウントならTRUE
  BTL_STATUS_FLAG_SYMBOL = 0x10,      ///<PDW連動のシンボルエンカウントならTRUE
  BTL_STATUS_FLAG_HIGH_LV_ENC = 0x20, ///<野生ハイレベルエンカウントならTRUE
  BTL_STATUS_FLAG_BOXFULL = 0x40,     ///<手持ち・ボックスが満杯でもう捕獲できない時にTRUE
}BTL_STATUS_FLAG;

//-----------------------------------------------------------------------------------
/**
 * トレーナーデータ構造体
 */
//-----------------------------------------------------------------------------------
enum {
  BSP_TRAINERDATA_ITEM_MAX = 4,
};

typedef struct {
  u32     tr_id;
  u32     tr_type;          //トレーナー分類
  u32     ai_bit;
  u16     use_item[BSP_TRAINERDATA_ITEM_MAX];      //使用道具

  STRBUF*   name; //トレーナー名

  //バトルサブウェイ他 Wifi-DLトレーナーとの対戦時にのみ必要
  PMS_DATA  win_word;   //戦闘終了時勝利メッセージ
  PMS_DATA  lose_word;  //戦闘終了時負けメッセージ

}BSP_TRAINER_DATA;

//--------------------------------------------------------------
/**
 *  マルチモード
 */
//--------------------------------------------------------------

typedef enum {

  BTL_MULTIMODE_NONE = 0, ///< 非マルチ
  BTL_MULTIMODE_PP_PP,    ///< プレイヤー２人 vs プレイヤー２人（通信マルチ）
  BTL_MULTIMODE_PP_AA,    ///< プレイヤー２人 vs AI２人（AI通信マルチ）
  BTL_MULTIMODE_PA_AA,    ///< プレイヤー１人＆AI１人 vs AI２人（AIマルチ）
  BTL_MULTIMODE_P_AA,     ///< プレイヤー１人 VS AI２人（AIタッグ）

}BtlMultiMode_tag;

typedef u8 BtlMultiMode;

//--------------------------------------------------------------
/**
 *  デバッグフラグ
 */
//--------------------------------------------------------------
typedef enum {

  BTL_DEBUGFLAG_MUST_TUIKA = 0, ///< 追加効果の発生率を100％にする
  BTL_DEBUGFLAG_MUST_TOKUSEI,   ///< 一部とくせいの効果発生率を100％にする
  BTL_DEBUGFLAG_MUST_ITEM,      ///< 一部アイテムの効果発生率を100％にする
  BTL_DEBUGFLAG_MUST_CRITICAL,  ///< 打撃ワザのクリティカル発生率を100％にする
  BTL_DEBUGFLAG_HP_CONST,       ///< HP減らない
  BTL_DEBUGFLAG_PP_CONST,       ///< PP減らない
  BTL_DEBUGFLAG_HIT100PER,      ///< 命中率が常に100％
  BTL_DEBUGFLAG_DMG_RAND_OFF,   ///< ダメージのランダム補正をオフ
  BTL_DEBUGFLAG_SKIP_BTLIN,     ///< 入場エフェクトスキップ（非通信時のみ）

  BTL_DEBUGFLAG_MAX,

}BtlDebugFlag;


//-----------------------------------------------------------------------------------
/**
 *  バトルセットアップパラメータ
 */
//-----------------------------------------------------------------------------------
typedef struct {

  //バトルルール
  BtlCompetitor   competitor;
  BtlRule         rule;

  //フィールドの状態から決定されるバトルシチュエーションデータ
  BTL_FIELD_SITUATION   fieldSituation;
  u16             musicDefault;   ///< デフォルト時のBGMナンバー
  u16             musicPinch;     ///< ピンチ時のBGMナンバー

  //通信データ
  GFL_NETHANDLE*  netHandle;
  BtlCommMode     commMode;
  u8              commPos;     ///< 通信対戦なら自分の立ち位置（非通信時は常に0を指定）
  BtlMultiMode    multiMode;   ///< マルチ対戦モード（ダブルバトル時のみ有効）
  u8              fRecordPlay; ///< TRUEだと録画データ再生モード

  // [io] パーティデータ・対戦相手ステータス
  //（通信バトル後、録画データ生成のために操作プレイヤー以外のデータが格納される）
  POKEPARTY*        party[ BTL_CLIENT_NUM ];
  MYSTATUS*         playerStatus[ BTL_CLIENT_NUM ];

  // トレーナーデータ
  BSP_TRAINER_DATA*  tr_data[BTL_CLIENT_NUM];

  // セーブデータ系
  const CONFIG*     configData;   ///< コンフィグデータ
  MYITEM*           itemData;     ///< アイテムデータ
  BAG_CURSOR*       bagCursor;    ///< バッグカーソルデータ
  ZUKAN_SAVEDATA*   zukanData;    ///< 図鑑データ

  // ゲームデータ系
  COMM_PLAYER_SUPPORT*  commSupport;  ///< 通信プレイヤーサポート

  // 制限時間設定
  u16         LimitTimeGame;        ///< 試合制限時間（秒）         [ 0 = 無制限 ]
  u16         LimitTimeCommand;     ///< コマンド選択制限時間（秒） [ 0 = 無制限 ]

  // 各種ステータスフラグ
  SHOOTER_ITEM_BIT_WORK  shooterBitWork;  ///< シューター使用制限パラメータ
  u8          btl_status_flag;  ///< 各種ステータスフラグ(定義値 BTL_STATUS_FLAG型)
  u8          badgeCount;       ///< プレイヤー取得済みバッジ数
  u16         commNetIDBit;     ///< 通信対戦時のNetIDBitフラグ

  // デバッグ系
  u16         DebugFlagBit;          ///< 各種デバッグフラグ


  //----- 以下、バトルの結果格納パラメータ ----
  BtlResult   result;           ///< 勝敗結果
  u16         getMoney;         ///< 手に入れたお小遣い
  u8          capturedPokeIdx;  ///< 捕獲したポケモンのメンバー内インデックス（partyEnemy1 内）

  u8*         recBuffer;        ///< 録画データ生成先
  u32         recDataSize;      ///< 録画データサイズ
  GFL_STD_RandContext  recRandContext;    ///< 録画データ用乱数コンテキスト

  u8 TurnNum;       //かかったターン数
  u8 PokeChgNum;    //交代回数
  u8 VoidAtcNum;    //効果がない技を出した回数
  u8 WeakAtcNum;    //ばつぐんの技を出した回数
  u8 ResistAtcNum;  //いまひとつの技を出した回数
  u8 VoidNum;       //効果がない技を受けた回数
  u8 ResistNum;     //いまひとつの技を受けた回数
  u8 WinTrainerNum; //倒したトレーナー数
  u8 WinPokeNum;    //倒したポケモン数
  u8 LosePokeNum;   //倒されたポケモン数
  u8 UseWazaNum;    //使用した技の数


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
  BTL_NETFUNCTBL_ELEMS = 8,
};


#endif
