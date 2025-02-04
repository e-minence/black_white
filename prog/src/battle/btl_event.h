//=============================================================================================
/**
 * @file    btl_event.h
 * @brief   ポケモンWB バトルシステム イベント＆ハンドラ処理
 * @author  taya
 *
 * @date  2008.10.23  作成
 */
//=============================================================================================
#ifndef __BTL_EVENT_H__
#define __BTL_EVENT_H__

#include "waza_tool/wazadata.h"

#include "btl_server.h"
#include "btl_server_flow.h"
#include "btl_calc.h"





//--------------------------------------------------------------
/**
* イベントタイプ
*/
//--------------------------------------------------------------
typedef enum {
  BTL_EVENT_NULL = 0,

  BTL_EVENT_ACTPROC_START,          ///< アクション処理１件開始
  BTL_EVENT_ACTPROC_END,            ///< アクション処理１件終了
  BTL_EVENT_WAZASEQ_START,          ///< ワザ処理開始
  BTL_EVENT_WAZASEQ_END,            ///< ワザ処理終了
  BTL_EVENT_MIGAWARI_EXCLUDE,       ///< みがわり除外チェック
  BTL_EVENT_CHECK_DELAY_WAZA,       ///< 遅延発動ワザ準備チェック
  BTL_EVENT_DECIDE_DELAY_WAZA,      ///< 遅延発動ワザ準備完了
  BTL_EVENT_WAZASEQ_ROB,            ///< ワザ乗っ取り確定
  BTL_EVENT_WAZASEQ_REFRECT,        ///< ワザ跳ね返し確定
  BTL_EVENT_CHECK_CHANGE,           ///< いれかえチェック
  BTL_EVENT_SKIP_NIGERU_CALC,       ///< にげる確率計算スキップチェック
  BTL_EVENT_NIGERU_FORBID,          ///< にげる封じチェック
  BTL_EVENT_NIGERU_EXMSG,           ///< にげる特殊メッセージ
  BTL_EVENT_CHECK_INEMURI,          ///< いねむり成否チェック
  BTL_EVENT_CHECK_SP_PRIORITY,      ///< 特殊優先度チェック
  BTL_EVENT_WORKED_SP_PRIORITY,     ///< 特殊優先度効果発生した
  BTL_EVENT_GET_WAZA_PRI,           ///< ワザプライオリティ取得
  BTL_EVENT_CHECK_FLYING,           ///< 浮遊チェック
  BTL_EVENT_CALC_AGILITY,           ///< すばやさ計算
  BTL_EVENT_CHECK_CONF,             ///< 混乱チェック
  BTL_EVENT_BEFORE_FIGHT,           ///< ターン最初のワザシーケンス直前
  BTL_EVENT_REQWAZA_FOR_ACT_ORDER,  ///< 優先順計算に使う他ワザ呼び出し
  BTL_EVENT_REQWAZA_CHECKFAIL,      ///< 他ワザ呼び出し失敗チェック
  BTL_EVENT_REQWAZA_PARAM,          ///< 他ワザ呼び出し時パラメータ
  BTL_EVENT_REQWAZA_MSG,            ///< 他ワザ呼び出し時メッセージ
  BTL_EVENT_CHECK_WAZA_ROB,         ///< ワザ乗っ取りチェック
  BTL_EVENT_WAZA_NOEFF_BY_FLYING,   ///< 浮遊状態による地面ワザ無効化
  BTL_EVENT_SKIP_AVOID_CHECK,       ///< ワザ命中判定スキップチェック
  BTL_EVENT_WAZA_FAIL_THREW,        ///< ワザ失敗要因の無視判定
  BTL_EVENT_WAZA_EXECUTE_CHECK_1ST, ///< ワザ実行チェック（ワザメッセージ前・混乱より優先）
  BTL_EVENT_WAZA_EXECUTE_CHECK_2ND, ///< ワザ実行チェック（ワザメッセージ前・混乱より後）
  BTL_EVENT_WAZA_EXECUTE_CHECK_3RD, ///< ワザ実行チェック（ワザメッセージ後）
  BTL_EVENT_WAZA_EXECUTE_FAIL,      ///< ワザ出し失敗
  BTL_EVENT_WAZA_CALL_DECIDE,       ///< ワザ出し（ワザ名宣言=PP減少）確定
  BTL_EVENT_WAZA_EXE_DECIDE,        ///< ワザ出し確定（ふういん等に除外されず実行が確定）
  BTL_EVENT_WAZA_EXE_START,         ///< ワザ出し処理開始
  BTL_EVENT_WAZA_EXECUTE_EFFECTIVE, ///< ワザ出し成功（効果あり）
  BTL_EVENT_WAZA_EXECUTE_NO_EFFECT, ///< ワザ出し成功（効果なし）
  BTL_EVENT_WAZA_EXECUTE_DONE,      ///< ワザ出し終了
  BTL_EVENT_WAZA_PARAM,             ///< ワザパラメータチェック
  BTL_EVENT_DECIDE_TARGET,          ///< ワザ対象決定
  BTL_EVENT_TEMPT_TARGET,           ///< ワザ対象を自分に引き寄せ
  BTL_EVENT_NOEFFECT_CHECK_L1,      ///< ワザ無効化チェック（lv1 = 必中には負ける）ハンドラ
  BTL_EVENT_NOEFFECT_CHECK_L2,      ///< ワザ無効化チェック（lv2 = 必中にも勝つ）ハンドラ
  BTL_EVENT_NOEFFECT_CHECK_L3,      ///< ワザ無効化チェック（lv3 = 必中にも勝つ＆まもるより後）ハンドラ
  BTL_EVENT_CHECK_MAMORU_BREAK,     ///< まもる無効化チェック
  BTL_EVENT_WAZA_AVOID,             ///< ワザ外れた
  BTL_EVENT_DMG_TO_RECOVER_CHECK,   ///< ワザダメージ->回復化チェックハンドラ
  BTL_EVENT_DMG_TO_RECOVER_FIX,     ///< ワザダメージ->回復化決定ハンドラ
  BTL_EVENT_CALC_HIT_CANCEL,        ///< 命中率計算を飛ばすかチェック
  BTL_EVENT_WAZA_HIT_RANK,          ///< 命中率・回避率の決定ハンドラ
  BTL_EVENT_WAZA_HIT_RATIO,         ///< ワザの命中率補正ハンドラ
  BTL_EVENT_WAZA_HIT_COUNT,         ///< ヒット回数決定（複数回ヒットワザのみ）
  BTL_EVENT_CRITICAL_CHECK,         ///< クリティカルチェック
  BTL_EVENT_WAZA_POWER_BASE,        ///< ワザ威力基本値ハンドラ
  BTL_EVENT_WAZA_POWER,             ///< ワザ威力補正ハンドラ
  BTL_EVENT_ATTACKER_POWER_PREV,    ///< 攻撃側の能力値（こうげきorとくこう）取得前ハンドラ
  BTL_EVENT_DEFENDER_GUARD_PREV,    ///< 防御側の能力値（ぼうぎょorとくぼう）取得前ハンドラ
  BTL_EVENT_ATTACKER_POWER,         ///< 攻撃側の能力値（こうげきorとくこう）補正ハンドラ
  BTL_EVENT_DEFENDER_GUARD,         ///< 防御側の能力値（ぼうぎょorとくぼう）補正ハンドラ
  BTL_EVENT_CHECK_AFFINITY_ENABLE,  ///< 相性による無効チェックを行うか確認
  BTL_EVENT_CHECK_AFFINITY,         ///< 相性計算
  BTL_EVENT_ATTACKER_TYPE,          ///< 攻撃側ポケタイプ
  BTL_EVENT_TYPEMATCH_CHECK,        ///< 攻撃側タイプ一致チェック
  BTL_EVENT_TYPEMATCH_RATIO,        ///< 攻撃側タイプ一致時倍率
  BTL_EVENT_WAZA_ATK_TYPE,          ///< 攻撃側ポケタイプチェックハンドラ
  BTL_EVENT_WAZA_DEF_TYPE,          ///< 防御側ポケタイプチェックハンドラ
  BTL_EVENT_PREV_WAZA_DMG,          ///< ダメージ演出直後
  BTL_EVENT_WAZA_DMG_DETERMINE,     ///< ダメージワザが当たること確定した（計算前）
  BTL_EVENT_WAZA_DMG_PROC1,         ///< タイプ計算前ダメージ補正ハンドラ
  BTL_EVENT_WAZA_DMG_PROC2,         ///< タイプ計算後ダメージ補正ハンドラ
  BTL_EVENT_WAZA_DMG_PROC_END,      ///< ダメージ計算最終
  BTL_EVENT_WAZA_DMG_LAST,          ///< 最終ダメージ補正ハンドラ
  BTL_EVENT_WAZA_DMG_REACTION_PREV, ///< ダメージ処理直前チェック
  BTL_EVENT_WAZA_DMG_REACTION,      ///< ダメージ処理１体ごとの反応
  BTL_EVENT_WAZA_DMG_REACTION_L2,   ///< ダメージ処理１体ごとの反応（２段階目）
  BTL_EVENT_WAZADMG_SIDE_AFTER,     ///< ダメージを対象全部に与えた後の追加処理（１回のみ）
  BTL_EVENT_DECREMENT_PP,           ///< 使用ワザの減少PP値を取得
  BTL_EVENT_DECREMENT_PP_DONE,      ///< 使用ワザのPP値を減少後
  BTL_EVENT_CALC_KICKBACK,          ///< 反動計算ハンドラ
  BTL_EVENT_ADD_RANK_TARGET,        ///< ワザを受けた側への追加効果
  BTL_EVENT_ADD_RANK_USER,          ///< ワザを使った側への追加効果
  BTL_EVENT_MENBERCHANGE_INTR,      ///< メンバー入れ替え割り込み
  BTL_EVENT_MEMBER_OUT_FIXED,       ///< 個別ポケ退場確定後
  BTL_EVENT_MEMBER_IN,              ///< 個別ポケ入場直後
  BTL_EVENT_MEMBER_IN_PREV,         ///< 全ポケ入場イベント直前
  BTL_EVENT_MEMBER_IN_COMP,         ///< 全ポケ入場イベント処理後
  BTL_EVENT_ROTATION_IN,            ///< ローテーション両陣営終了後
  BTL_EVENT_GET_RANKEFF_VALUE,      ///< ワザによる能力ランク増減値チェック
  BTL_EVENT_RANKVALUE_CHANGE,       ///< ランク増減値の最終チェック
  BTL_EVENT_RANKEFF_LAST_CHECK,     ///< ランク増減成否チェック
  BTL_EVENT_RANKEFF_FAILED,         ///< 能力ランク増減失敗確定
  BTL_EVENT_RANKEFF_FIXED,          ///< 能力ランク増減処理後
  BTL_EVENT_WAZA_RANKEFF_FIXED,     ///< ワザによるランク増減効果成功
  BTL_EVENT_WAZASICK_TURN_COUNT,    ///< 特殊処理の状態異常ターン数
  BTL_EVENT_WAZASICK_SPECIAL,       ///< 特殊処理の状態異常ID決定
  BTL_EVENT_WAZASICK_STR,           ///< ワザによる通常の状態異常決定時テキスト
  BTL_EVENT_WAZASICK_PARAM,         ///< ワザによる状態異常のパラメータチェック
  BTL_EVENT_ADD_SICK_TYPE,          ///< ワザの追加効果による状態異常のタイプ調整
  BTL_EVENT_ADD_SICK,               ///< ワザの追加効果による状態異常の発生チェック
  BTL_EVENT_ADDSICK_CHECKFAIL,      ///< 状態異常の失敗チェック
  BTL_EVENT_ADDSICK_FIX,            ///< 状態異常確定
  BTL_EVENT_ADDSICK_FAILED,         ///< 状態異常失敗
  BTL_EVENT_POKESICK_FIXED,         ///< 基本状態異常確定
  BTL_EVENT_WAZASICK_FIXED,         ///< ワザ系状態異常確定
  BTL_EVENT_IEKI_FIXED,             ///< いえきによる特性無効化の確定
  BTL_EVENT_SICK_DAMAGE,            ///< 状態異常によるダメージ計算
  BTL_EVENT_WAZA_SHRINK_PER,        ///< ワザによるひるみ確率計算
  BTL_EVENT_SHRINK_CHECK,           ///< ひるみ計算
  BTL_EVENT_SHRINK_FAIL,            ///< ひるみ失敗
  BTL_EVENT_SHRINK_FIX,             ///< ひるみ確定後
  BTL_EVENT_ICHIGEKI_CHECK,         ///< 一撃必殺チェック
  BTL_EVENT_NOT_WAZA_DAMAGE,        ///< ワザ以外のダメージチェック
  BTL_EVENT_USE_ITEM,               ///< 装備アイテム使用
  BTL_EVENT_USE_ITEM_TMP,           ///< 装備アイテム使用（一時処理）
  BTL_EVENT_KORAERU_CHECK,          ///<「こらえる」チェック
  BTL_EVENT_KORAERU_EXE,            ///<「こらえる」発動
  BTL_EVENT_TURNCHECK_BEGIN,        ///< ターンチェック（先頭）
  BTL_EVENT_TURNCHECK_END,          ///< ターンチェック（終端）
  BTL_EVENT_TURNCHECK_DONE,         ///< ターンチェック（終了後）
  BTL_EVENT_NOTIFY_AIRLOCK,         ///< エアロック開始
  BTL_EVENT_WEATHER_CHECK,          ///< 天候のチェック
  BTL_EVENT_WEIGHT_RATIO,           ///< 体重変化率チェック
  BTL_EVENT_WAZA_WEATHER_TURNCNT,   ///< ワザによる天候変化時のターン数チェック
  BTL_EVENT_WEATHER_CHANGE,         ///< 天候の変化（直前）
  BTL_EVENT_WEATHER_CHANGE_AFTER,   ///< 天候の変化（直後）
  BTL_EVENT_WEATHER_REACTION,       ///< 天候ダメージに対する反応
  BTL_EVENT_SIMPLE_DAMAGE_ENABLE,   ///< ワザ以外ダメージ有効判定
  BTL_EVENT_DAMAGEPROC_START,       ///< ダメージワザシーケンス開始
  BTL_EVENT_DAMAGEPROC_END_HIT_PREV,///< ダメージワザシーケンス終了直前
  BTL_EVENT_DAMAGEPROC_END_HIT_REAL,///< ダメージワザシーケンス終了（ダメージを受けたポケモン（みがわり除く）が居る）
  BTL_EVENT_DAMAGEPROC_END_HIT_L1,  ///< ダメージワザシーケンス終了（ダメージを受けたポケモン（みがわり含む）が居る）
  BTL_EVENT_DAMAGEPROC_END_HIT_L2,  ///< ダメージワザシーケンス終了（ダメージを受けたポケモン（みがわり含む）が居る）
  BTL_EVENT_DAMAGEPROC_END_HIT_L3,  ///< ダメージワザシーケンス終了（ダメージを受けたポケモン（みがわり含む）が居る）
  BTL_EVENT_DAMAGEPROC_END_HIT_L4,  ///< ダメージワザシーケンス終了（ダメージを受けたポケモン（みがわり含む）が居る）
  BTL_EVENT_DAMAGEPROC_END,         ///< ダメージワザシーケンス終了（常に呼び出し）
  BTL_EVENT_CHANGE_TOKUSEI_BEFORE,  ///< とくせい書き換え直前（書き換え確定）
  BTL_EVENT_CHANGE_TOKUSEI_AFTER,   ///< とくせい書き換え後
  BTL_EVENT_CHECK_PUSHOUT,          ///< ふきとばし系ワザチェック
  BTL_EVENT_CALC_DRAIN,             ///< ドレイン系ワザ回復量計算
  BTL_EVENT_CALC_DRAIN_END,         ///< ドレイン系ワザ回復量計算
  BTL_EVENT_CALC_SPECIAL_DRAIN,     ///< ドレイン系ワザ回復量計算
  BTL_EVENT_RECOVER_HP_RATIO,       ///< HP回復ワザの回復率計算
  BTL_EVENT_CHECK_ITEMEQUIP_FAIL,   ///< 装備アイテム使用可否チェック
  BTL_EVENT_CHECK_ITEM_REACTION,    ///< アイテム反応チェック
  BTL_EVENT_ITEM_CONSUMED,          ///< 装備アイテム消費後
  BTL_EVENT_CHECK_TAMETURN_FAIL,    ///< 溜めターン失敗判定
  BTL_EVENT_CHECK_TAMETURN_SKIP,    ///< 溜めターンスキップ判定
  BTL_EVENT_TAME_START,             ///< 溜め開始
  BTL_EVENT_TAME_START_FIXED,       ///< 溜め開始確定
  BTL_EVENT_TAME_SKIP,              ///< 溜めスキップ確定
  BTL_EVENT_TAME_RELEASE,           ///< 溜め解放
  BTL_EVENT_CHECK_POKE_HIDE,        ///< ポケモンが消えている状態のワザ命中チェック
  BTL_EVENT_ITEMSET_CHECK,          ///< アイテム書き換え前の成否チェック
  BTL_EVENT_ITEMSET_FAILED,         ///< アイテム書き換え失敗
  BTL_EVENT_ITEMSET_DECIDE,         ///< アイテム書き換え確定
  BTL_EVENT_ITEMSET_FIXED,          ///< アイテム書き換え完了
  BTL_EVENT_FIELD_EFFECT_CALL,      ///< フィールドエフェクト追加
  BTL_EVENT_CHECK_SIDEEFF_PARAM,    ///< サイドエフェクトパラメータ調整
  BTL_EVENT_UNCATEGORIZE_WAZA,            ///< 未分類ワザ処理
  BTL_EVENT_UNCATEGORIZE_WAZA_NO_TARGET,  ///< 未分類ワザ処理（ターゲットいないワザ）
  BTL_EVENT_COMBIWAZA_CHECK,        ///< 合体ワザ成立チェック
  BTL_EVENT_NOTIFY_DEAD,            ///< 死亡直前
  BTL_EVENT_AFTER_MOVE,             ///< ムーブ動作直後


}BtlEventType;

















//--------------------------------------------------------------
/**
* イベント変数ラベル
*/
//--------------------------------------------------------------
typedef enum {
  BTL_EVAR_NULL = 0,
  BTL_EVAR_SYS_SEPARATE,
  //---ここから上はシステム予約領域-------------------------
  BTL_EVAR_POKEID,
  BTL_EVAR_POKEID_ATK,
  BTL_EVAR_POKEID_DEF,
  BTL_EVAR_TARGET_POKECNT,
  BTL_EVAR_POKEID_TARGET1,
  BTL_EVAR_POKEID_TARGET2,
  BTL_EVAR_POKEID_TARGET3,
  BTL_EVAR_POKEID_TARGET4,
  BTL_EVAR_POKEID_TARGET5,
  BTL_EVAR_POKEID_TARGET6,
  BTL_EVAR_ACTION,
  BTL_EVAR_POKEPOS,
  BTL_EVAR_POKEPOS_ORG,
  BTL_EVAR_TOKUSEI_PREV,
  BTL_EVAR_TOKUSEI_NEXT,
  BTL_EVAR_SP_PRIORITY,
  BTL_EVAR_WAZAID,
  BTL_EVAR_EFFECT_WAZAID,
  BTL_EVAR_ORG_WAZAID,
  BTL_EVAR_POKE_TYPE,
  BTL_EVAR_WAZA_TYPE,
  BTL_EVAR_WAZA_IDX,
  BTL_EVAR_WAZA_PRI,
  BTL_EVAR_WAZA_SERIAL,
  BTL_EVAR_DAMAGE_TYPE,
  BTL_EVAR_TARGET_TYPE,
  BTL_EVAR_USER_TYPE,
  BTL_EVAR_SICKID,
  BTL_EVAR_SICK_CONT,
  BTL_EVAR_STATUS_TYPE,
  BTL_EVAR_VOLUME,
  BTL_EVAR_POKE_HIDE,
  BTL_EVAR_FAIL_CAUSE,
  BTL_EVAR_CONF_DMG,
  BTL_EVAR_TURN_COUNT,
  BTL_EVAR_ADD_DEFAULT_PER,
  BTL_EVAR_ADD_PER,
  BTL_EVAR_HIT_RANK,
  BTL_EVAR_AVOID_RANK,
  BTL_EVAR_HITCOUNT_MAX,
  BTL_EVAR_HITCOUNT,
  BTL_EVAR_HIT_PER,
  BTL_EVAR_CRITICAL_RANK,
  BTL_EVAR_ITEM,
  BTL_EVAR_ITEM_REACTION,
  BTL_EVAR_AGILITY,
  BTL_EVAR_WAZA_POWER,
  BTL_EVAR_WAZA_POWER_RATIO,
  BTL_EVAR_DAMAGE,
  BTL_EVAR_POWER,
  BTL_EVAR_GUARD,
  BTL_EVAR_RATIO,
  BTL_EVAR_RATIO_EX,
  BTL_EVAR_FIX_DAMAGE,
  BTL_EVAR_TYPEAFF,
  BTL_EVAR_WEATHER,
  BTL_EVAR_KORAERU_CAUSE,
  BTL_EVAR_SWAP_POKEID,
  BTL_EVAR_VID,
  BTL_EVAR_VID_SWAP_CNT,
  BTL_EVAR_ENABLE_MODE,
  BTL_EVAR_WORK_ADRS,
  BTL_EVAR_NOEFFECT_FLAG,
  BTL_EVAR_FAIL_FLAG,
  BTL_EVAR_AVOID_FLAG,
  BTL_EVAR_ALMOST_FLAG,
  BTL_EVAR_TYPEMATCH_FLAG,
  BTL_EVAR_CRITICAL_FLAG,
  BTL_EVAR_MIGAWARI_FLAG,
  BTL_EVAR_RINPUNGUARD_FLG,
  BTL_EVAR_TIKARAZUKU_FLG,
  BTL_EVAR_ITEMUSE_FLAG,
  BTL_EVAR_TRICK_FLAG,
  BTL_EVAR_FLAT_FLAG,
  BTL_EVAR_FLATMASTER_FLAG,
  BTL_EVAR_DELAY_ATTACK_FLAG,
  BTL_EVAR_MAGICCOAT_FLAG,
  BTL_EVAR_MSG_FLAG,
  BTL_EVAR_FIX_FLAG,
  BTL_EVAR_GEN_FLAG,
  BTL_EVAR_SIDE,
  BTL_EVAR_SIDE_EFFECT,

  BTL_EVAR_MAX,

}BtlEvVarLabel;


//--------------------------------------------------------------
/**
* イベント変数スタック
*/
//--------------------------------------------------------------
extern void BTL_EVENTVAR_PopInpl( u32 line );
extern void BTL_EVENTVAR_PushInpl( u32 line );
#define BTL_EVENTVAR_Pop()  BTL_EVENTVAR_PopInpl( __LINE__ )
#define BTL_EVENTVAR_Push() BTL_EVENTVAR_PushInpl( __LINE__ )

extern void BTL_EVENTVAR_CheckStackCleared( void );
extern void BTL_EVENTVAR_SetValue( BtlEvVarLabel label, int value );
extern void BTL_EVENTVAR_SetConstValue( BtlEvVarLabel label, int value );
extern void BTL_EVENTVAR_SetRewriteOnceValue( BtlEvVarLabel label, int value );
extern void BTL_EVENTVAR_SetMulValue( BtlEvVarLabel label, int value, fx32 mulMin, fx32 mulMax );
extern BOOL BTL_EVENTVAR_RewriteValue( BtlEvVarLabel label, int value );
extern void BTL_EVENTVAR_MulValue( BtlEvVarLabel label, fx32 value );
extern int  BTL_EVENTVAR_GetValue( BtlEvVarLabel label );
extern BOOL BTL_EVENTVAR_GetValueIfExist( BtlEvVarLabel label, int* value );




//------------------------------------------------------------------------------------------
/**
* イベントハンドラ用ワーク型＆定数
*/
//------------------------------------------------------------------------------------------

/**
 *  ワザ出し失敗ハンドラ用ワーク
 */
typedef struct {

  /** 各種効果の有効bitフラグ */
  union {
    u8 bitFlags;
    struct {
      u8    flag_rankEffect : 1;  ///< ランク増減効果
      u8    flag_padding    : 7;
    };
  };

  WazaRankEffect  rankType;   ///< ランク効果種類
  s16             rankVolume; ///< ランク効果増減値

  u8              tokWinFlag; ///< とくせいウィンドウ出しフラグ

}BTL_EVWK_WAZAEXE_FAIL;

/**
 *  ワザダメージ->回復化チェックハンドラ用ワーク
 */
typedef struct {
  u8  pokeID;                 ///< 対象ポケモンID
  u8  tokFlag;                ///< とくせいウインドウ表示
  u16 recoverHP;              ///< 回復量

  WazaRankEffect  rankEffect; ///< 回復に加えランク効果が発生する場合に設定
  s16             rankVolume; ///< ランク効果増減値

}BTL_EVWK_DMG_TO_RECOVER;


/**
 *  ワザダメージ受けた後のハンドラ反応タイプ
 */
typedef enum {
  BTL_EV_AFTER_DAMAGED_REACTION_NONE,

  BTL_EV_AFTER_DAMAGED_REACTION_SICK,       ///< 対象ポケモンを状態異常にする
  BTL_EV_AFTER_DAMAGED_REACTION_RANK,       ///< 対象ポケモンのランク増減効果
  BTL_EV_AFTER_DAMAGED_REACTION_DAMAGE,     ///< 対象ポケモンにダメージを与える
  BTL_EV_AFTER_DAMAGED_REACTION_POKETYPE,   ///< 対象ポケモンのタイプ変更
  BTL_EV_AFTER_DAMAGED_REACTION_MAX,

}BtlEvAfterDamagedReactionType;

/**
 *  ワザダメージ受けた後のハンドラワーク
 */
typedef struct {

  BtlEvAfterDamagedReactionType   reaction; ///< 反応タイプ

  u8  damagedPokeID;    ///< ダメージを受けたポケモンID
  u8  targetPokeID;     ///< 効果対象ポケモンID
  u8  tokFlag;          ///< とくせいウインドウ表示
  u8  itemUseFlag;      ///< アイテム使用フラグ

  WazaSick        sick;       ///< 対象ポケモンに与える状態異常効果
  BPP_SICK_CONT   sickCont;   ///< 対象ポケモンに与える状態異常の継続パターン
  PokeType        pokeType;   ///< 対象ポケモンを変化させるタイプ
  WazaRankEffect  rankType;   ///< 対象ポケモンに与えるランク効果タイプ
  int             rankVolume; ///< 対象ポケモンに与えるランク効果値
  u16             damage;     ///< 対象ポケモンに与えるダメージ量

}BTL_EVWK_DAMAGE_REACTION;


/**
 *  状態異常チェックハンドラの反応タイプ
 */
typedef enum {

  BTL_EV_SICK_REACTION_NONE = 0,  ///< 反応なし
  BTL_EV_SICK_REACTION_DISCARD,   ///< 効果を打ち消す
  BTL_EV_SICK_REACTION_REFRECT,   ///< 相手にもうつす

}BtlEvSickReaction;

/**
 *  状態異常チェックハンドラ用ワーク
 */
typedef struct {

  BtlEvSickReaction   reaction;           ///< 反応タイプ
  WazaSick            discardSickType;    ///< 特定の状態異常のみ打ち消す場合に指定
  u8                  fDiscardByTokusei;  ///< とくせいによる打ち消しならTRUEにする
  u8                  discardPokeID;      ///< 打ち消したポケモンID
  u8                  fItemResponce;      ///< アイテム使用する場合TRUEにする

}BTL_EVWK_ADDSICK;

/**
 *  ランク効果チェックハンドラ用ワーク
 */
typedef struct {

  u8              failFlag;           ///< とにかく失敗
  u8              failTokuseiFlag;    ///< 対象ポケモンのとくせいによる失敗
  u8              useItemFlag;        ///< アイテム発動フラグ
  WazaRankEffect  failSpecificType;   ///< 特定の効果のみ失敗させるなら指定

}BTL_EVWK_CHECK_RANKEFF;

/**
 *  相性チェックハンドラ用ワーク
 */
typedef struct {

  BtlTypeAff      aff;              ///< 相性ID
  u8              weakedByItem;     ///< アイテムによって相性が弱められている
  u8              weakReserveByItem;///< アイテムによってダメージ値を半減させる
  u16             weakedItemID;     ///< 弱めるために使われたアイテム

}BTL_EVWK_CHECK_AFFINITY;


//--------------------------------------------------------------
/**
* イベントファクタータイプ
*/
//--------------------------------------------------------------
typedef enum {

  // 以下はタイプごとの実行プライオリティ順になっているため順番を入れ替えるのは慎重に。
  // （0が最も高プライオリティ）

  BTL_EVENT_FACTOR_WAZA,
  BTL_EVENT_FACTOR_POS,
  BTL_EVENT_FACTOR_SIDE,
  BTL_EVENT_FACTOR_FIELD,
  BTL_EVENT_FACTOR_TOKUSEI,
  BTL_EVENT_FACTOR_ITEM,

  BTL_EVENT_FACTOR_ISOLATE, // あらゆるタイプのハンドラからコンバートされる特殊タイプで、
                            // ポケモン・アイテムに依存しないので、死んだりアイテムを消費しても実行される。
                            // 毎ターン終了時にまとめて消去される。

  BTL_EVENT_FACTOR_MAX,

}BtlEventFactorType;

/**
 *  イベント処理順序
 */
typedef enum {

  BTL_EVPRI_WAZA_DEFAULT,

  BTL_EVPRI_POS_DEFAULT,

  BTL_EVPRI_SIDE_DEFAULT,

  BTL_EVPRI_FIELD_DEFAULT,

    BTL_EVPRI_TOKUSEI_dokusyu,
  BTL_EVPRI_TOKUSEI_DEFAULT,

  BTL_EVPRI_ITEM_DEFAULT,
    BTL_EVPRI_TOKUSEI_atodasi,

  BTL_EVPRI_MAX,

}BtlEventPriority;


extern void BTL_EVENT_InitSystem( void );
extern void BTL_EVENT_StartTurn( void );
extern void BTL_EVENT_CallHandlers( BTL_SVFLOW_WORK* flowWork, BtlEventType type );
extern void BTL_EVENT_ForceCallHandlers( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID );
extern void BTL_EVENT_CallHandlersTargetType( BTL_SVFLOW_WORK* flowWork, BtlEventType eventID, BtlEventFactorType type );
extern void BTL_EVENT_RemoveIsolateFactors( void );
extern void BTL_EVENT_ReOrderFactorsByPokeAgility( BTL_SVFLOW_WORK* flowWork );


extern void BTL_EVENT_SleepFactorMagicMirrorUser( u16 pokeID );
extern void BTL_EVENT_WakeFactorMagicMirrorUser( u16 pokeID );

extern void BTL_EVENT_SleepFactorRotation( u16 pokeID, BtlEventFactorType factorType );
extern BOOL BTL_EVENT_WakeFactorRotation( u16 pokeID, BtlEventFactorType factorType );


#endif
