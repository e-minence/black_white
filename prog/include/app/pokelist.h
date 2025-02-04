//======================================================================
/**
 * @file  pokelist.h
 * @brief ポケモンリスト
 * @author  ariizumi
 * @data  09/06/10
 *
 * モジュール名：POKE_LIST
 */
//======================================================================
#pragma once

#include "field/field_skill.h"
#include "poke_tool/pokeparty.h"
#include "savedata/config.h"

//-----------------------------------------------
//以下GSより移植(09/06/10)
//enumにtypedefで型追加
//-----------------------------------------------

// リストタイプ
typedef enum
{
  PL_TYPE_SINGLE = 0, // シングル
  PL_TYPE_DOUBLE,     // ダブル
  PL_TYPE_MULTI,      // マルチ
  PL_TYPE_TRIPLE,     // マルチ  //追加
  PL_TYPE_ROTATION,   // ローテーション
}PL_LIST_TYPE;

// リストモード
typedef enum
{
  PL_MODE_FIELD = 0,      // フィールドメニュー
  PL_MODE_BATTLE,         // 戦闘
  PL_MODE_SELECT,         // 「けってい」あり
  PL_MODE_SET,            // 選択だけ（メニューなし）
  PL_MODE_IN,             // マルチバトル開始エフェクト
  PL_MODE_ITEMUSE,        // アイテム使用
  PL_MODE_WAZASET,        // 技覚え
  PL_MODE_WAZASET_RET,    // ステータス画面から技を選択して戻ってきたとき（技マシン）
  PL_MODE_LVUPWAZASET_RET,// ステータス画面から技を選択して戻ってきたとき（不思議なアメ）
  PL_MODE_ITEMSET,        // アイテム「もたせる」
  PL_MODE_ITEMSET_RET,    // ポケモンリスト「もたせる」バッグから
  PL_MODE_MAILSET,        // メール「もたせる」終了
  PL_MODE_MAILSET_BAG,    // ポケモンリスト メール「もたせる」終了（バッグから）
  PL_MODE_CONTEST,        // コンテスト参加選択
  PL_MODE_MAILBOX,        // メールボックスからメールを持たせる
  PL_MODE_KAPSEL_SET,     // カプセルセット用
  PL_MODE_SHINKA,         // 進化アイテム使用
  PL_MODE_BATTLE_TOWER,   // バトルタワー参加選択用
  PL_MODE_SODATEYA,       // 育て屋
  PL_MODE_NPC_TRADE,      // ゲーム内交換
  PL_MODE_PORUTO,         // ポルトを使用選択用
  PL_MODE_GURU2,          // ぐるぐる交換たまご選択
//  PL_MODE_BATTLE_STAGE,   // バトルステージ参加選択用
//  PL_MODE_BATTLE_CASTLE,  // バトルキャッスル参加選択用
//  PL_MODE_BATTLE_ROULETTE = (PL_MODE_BATTLE_CASTLE),  // バトルルーレット参加選択用

  PL_MODE_BATTLE_SUBWAY,        //バトルサブウェイ
  PL_MODE_BATTLE_EXAMINATION,   //トレーナー検定

  PL_MODE_SET_WAZA,       // 内部でPL_MODE_SETに置き換わります。
                          // 終了時にはPL_MODE_SET_WAZAに戻します。
                          // wazaLearnBitの設定を忘れないでください。
                          // 最強技・合体技は複数個の技をチェックするので
                          // wazaNoにPL_SP_WAZANO_***を入れてください
  PL_MODE_SET_MUSICAL,    // 内部でPL_MODE_SETに置き換わります。
                          // 終了時にはPL_MODE_SET_MUSICALに戻します。
  PL_MODE_BATTLE_WIFI,    // 内部でPL_MODE_BATTLEに置き換わります。
                          // 終了時にはPL_MODE_BATTLEに戻します。

}PL_MODE_TYPE;

#define PL_MODE_NO_CANCEL ( 0x80 )  // キャンセル禁止
#define PL_SP_WAZANO_STRONGEST (0xFFFE)   //最強技
#define PL_SP_WAZANO_COALESCENCE (0xFFFD) //合体技

// 選択されたもの
typedef enum
{
  PL_SEL_POS_POKE1 = 0, // ポケモン１
  PL_SEL_POS_POKE2,   // ポケモン２
  PL_SEL_POS_POKE3,   // ポケモン３
  PL_SEL_POS_POKE4,   // ポケモン４
  PL_SEL_POS_POKE5,   // ポケモン５
  PL_SEL_POS_POKE6,   // ポケモン６
  PL_SEL_POS_ENTER,   // 決定
  PL_SEL_POS_EXIT,    // 戻る

  PL_SEL_POS_EXIT2,   // 戻る(一気に戻るXボタンの方
  
  PL_SEL_POS_MAX,
  
}PL_SELECT_POS;

// 選択された処理
typedef enum
{
  PL_RET_NORMAL = 0,  // 通常
  PL_RET_STATUS,      // メニュー「つよさをみる」
  PL_RET_CHANGE,      // メニュー「いれかえる」
  PL_RET_ITEMSET,     // メニュー「もたせる」

  PL_RET_WAZASET,     // 技選択へ（技マシン）
  PL_RET_LVUP_WAZASET,// 技選択へ（不思議なアメ）
  PL_RET_MAILSET,     // メール入力へ
  PL_RET_MAILREAD,    // メールを読む画面へ

  PL_RET_ITEMSHINKA,  // 進化画面へ（アイテム進化）
  PL_RET_LVUPSHINKA,  // 進化画面へ（レベル進化）
  
  PL_RET_BAG,         // バッグから呼ばれた場合で、バッグへ戻る
  
/** ↓↓↓ field_skill.hのFLDSKILL_IDX_XXXと同じ並びで！ ↓↓↓ **/
  PL_RET_IAIGIRI,     // メニュー 技：いあいぎり
  PL_RET_NAMINORI,    // メニュー 技：なみのり
  PL_RET_TAKINOBORI,  // メニュー 技：たきのぼり
  PL_RET_KAIRIKI,     // メニュー 技：かいりき
  PL_RET_SORAWOTOBU,  // メニュー 技：そらをとぶ
  PL_RET_FLASH,       // メニュー 技：フラッシュ
  PL_RET_TELEPORT,    // メニュー 技：テレポート
  PL_RET_ANAWOHORU,   // メニュー 技：あなをほる
  PL_RET_AMAIKAORI,   // メニュー 技：あまいかおり
  PL_RET_OSYABERI,    // メニュー 技：おしゃべり
  PL_RET_DIVING,      // メニュー 技：ダイビング  //field_skillココまで
  PL_RET_MILKNOMI,    // メニュー 技：ミルクのみ
  PL_RET_TAMAGOUMI,   // メニュー 技：タマゴうみ
}PL_RETURN_TYPE;

//バトル選択時、通信タイプ設定
typedef enum{
  PL_COMM_NONE,    //非通信
  PL_COMM_SINGLE,  //シングル
  PL_COMM_MULTI,   //マルチ

  PL_COMM_TYPE_MAX,
}PL_COMM_TYPE;

//comm_battleの添え字
typedef enum{
  PL_COMM_PLAYER_TYPE_ALLY,       ///<味方
  PL_COMM_PLAYER_TYPE_ENEMY_A,    ///<敵A
  PL_COMM_PLAYER_TYPE_ENEMY_B,    ///<敵B
  
  PL_COMM_PLAYER_TYPE_MAX,
}PL_COMM_PLAYER_TYPE;

//通信対戦時の相手情報
typedef struct{
  POKEPARTY *pp;              // 対戦相手のポケモンデータ
  const STRCODE *name;        // 対戦相手の名前
  u8 sex;                     // 対戦相手の性別
  u8 padding[3];
}PL_COMM_BATTLE_PARAM;

//-----------------------------------------------
// ポケモンリスト外部作成データ
//-----------------------------------------------
typedef struct 
{
  POKEPARTY * pp;   // ポケモンデータ
  void * myitem;    // アイテムデータ
  void * mailblock; // メールデータ
  CONFIG * cfg;   // コンフィグデータ
  void * tvwk;    // テレビワーク //存在しない？
  void * reg;     // レギュレーションデータ
  void * zkn;     // 図鑑セーブ(フォルムチェンジ時登録処理

  FLDSKILL_CHECK_WORK scwk;    // 技使用チェックワーク

//  今回フィールドから取れないので必要なものを個別で持つ
//  void * fsys;    // フィールドワーク
  ZONEID zone_id;        //場所：アイテム使用時に参照

  PL_MODE_TYPE  mode;     // リストモード
  PL_LIST_TYPE  type;     // リストタイプ
  PL_SELECT_POS  ret_sel;    // 選択されたポケモン or 決定 or 戻る 兼 初期カーソル位置
  PL_RETURN_TYPE ret_mode;   // 選択された処理

  u16 item;     // アイテム
  u16 waza;     // 技
  u8  waza_pos;   // 技位置

  u8  in_num[6];    // 参加番号 0:無し 1〜:1番目〜
/*  レギュレーションから取る
  u8  in_min:4;   // 参加最小数
  u8  in_max:4;   // 参加最大数
  u8  in_lv;      // 参加レベル
*/
  int lv_cnt;     // レベルアップカウンタ

  u16 after_mons;   // 進化後のポケモン
  int shinka_cond;  // 進化条件ワーク
  
  //WBより追加
  u16 mail_pos;
  u8  wazaLearnBit; //イベントなど特殊な技覚えで外から覚えられるやつのbitをもらう
                    // PL_MODE_SELECT_WAZAの時のみ有効
  u8  season;       //シェイミ時間変化対応
  //通信処理用
  u16 time_limit;   //制限時間(秒)
  u8  comm_state;   //通信状態
  u8  isNetErr;
  
  //通信時相手見せ系
  PL_COMM_TYPE comm_type;
  PL_COMM_BATTLE_PARAM comm_battle[PL_COMM_PLAYER_TYPE_MAX];  //通信相手のパラメータ
  BOOL    is_disp_party;  //パーティーの表示設定
  BOOL    use_tile_limit;  //制限時間を使うか？
  u8 comm_selected_num;   // 通信時：既に選択が終わった人数(外側から操作)
  u8 padding;
}PLIST_DATA;

//-----------------------------------------------
//以上GSより移植(09/06/10)
//-----------------------------------------------

extern GFL_PROC_DATA PokeList_ProcData;

