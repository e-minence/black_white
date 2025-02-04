//=============================================================================================
/**
 * @file  btl_server_cmd.h
 * @brief ポケモンWB バトルシステム サーバコマンド生成，解釈処理
 * @author  taya
 *
 * @date  2008.10.30  作成
 */
//=============================================================================================
#ifndef __BTL_SERVER_CMD_H__
#define __BTL_SERVER_CMD_H__

#include "btl_string.h"

enum {
  BTL_SERVER_CMD_QUE_SIZE = 3000,
  BTL_SERVERCMD_ARG_MAX = 16,
};




//--------------------------------------------------------------
/**
 *    サーバコマンド
 */
//--------------------------------------------------------------
typedef enum {
  SC_NULL = 0,

  SC_OP_HP_MINUS,           ///< HPマイナス  [ClientID, マイナス量]
  SC_OP_HP_PLUS,            ///< HPプラス    [ClientID, プラス量]
  SC_OP_HP_ZERO,            ///< HP0 にする [ pokeID ]
  SC_OP_PP_MINUS,           ///< PPマイナス （仮ワザワーク用）
  SC_OP_PP_MINUS_ORG,       ///< PPマイナス （真ワザワーク用）
  SC_OP_WAZA_USED,          ///< ワザ使用記録
  SC_OP_PP_PLUS,            ///< PPプラス   （仮ワザワーク用）
  SC_OP_PP_PLUS_ORG,        ///< PPプラス   （真ワザワーク用）
  SC_OP_RANK_UP,            ///< ステータスランクアップ  [ClientID, StatusType, プラス量]
  SC_OP_RANK_DOWN,          ///< ステータスランクダウン  [ClientID, StatusType, マイナス量]
  SC_OP_RANK_SET7,          ///< ステータスランク７種セット[ pokeID, atk, def, sp_atk, sp_def, agi ]
  SC_OP_RANK_RECOVER,       ///< ステータスランク（７種）下がっているもののみフラットに
  SC_OP_RANK_RESET,         ///< ステータスランク（７種）全てをフラットに
  SC_OP_ADD_CRITICAL,       ///< クリティカルランク加算[ pokeID, (int)value ]
  SC_OP_SICK_SET,           ///< 状態異常 [PokeID, Sick, contParam]
  SC_OP_CURE_POKESICK,      ///< ポケモン系状態異常を回復 [PokeID ]
  SC_OP_CURE_WAZASICK,      ///< ワザ系状態異常を回復 [PokeID, SickID ]
  SC_OP_MEMBER_IN,          ///< メンバー入場
  SC_OP_SET_STATUS,         ///< 能力値（攻撃、防御等）を書き換え
  SC_OP_SET_WEIGHT,         ///< 体重設定
  SC_OP_WAZASICK_TURNCHECK, ///<
  SC_OP_CHANGE_POKETYPE,    ///< 【計算】ポケモンのタイプ変更（ pokeID, type ）
  SC_OP_CONSUME_ITEM,       ///< 所有アイテム削除
  SC_OP_UPDATE_USE_WAZA,    ///< ワザプロセス終了情報
  SC_OP_SET_CONTFLAG,       ///< 永続フラグセット
  SC_OP_RESET_CONTFLAG,     ///< 永続フラグリセット
  SC_OP_SET_TURNFLAG,       ///< ターンフラグセット
  SC_OP_RESET_TURNFLAG,     ///< ターンフラグ強制リセット
  SC_OP_CHANGE_TOKUSEI,     ///< とくせい書き換え
  SC_OP_SET_ITEM,           ///< アイテム書き換え
  SC_OP_UPDATE_WAZANUMBER,  ///< ワザ書き換え
  SC_OP_OUTCLEAR,           ///< 退場時クリア
  SC_OP_ADD_FLDEFF,         ///< フィールドエフェクト追加
  SC_OP_ADD_FLDEFF_DEPEND,  ///< フィールドエフェクトの依存ポケを追加
  SC_OP_DEL_FLDEFF_DEPEND,  ///< フィールドエフェクトの依存ポケを削除
  SC_OP_REMOVE_FLDEFF,      ///< フィールドエフェクト削除
  SC_OP_SET_POKE_COUNTER,   ///< ポケモンカウンタ値セット
  SC_OP_BATONTOUCH,         ///< バトンタッチ
  SC_OP_MIGAWARI_CREATE,    ///< みがわり作成
  SC_OP_MIGAWARI_DELETE,    ///< みがわり削除
  SC_OP_SHOOTER_CHARGE,     ///< シューターエネルギーチャージ
  SC_OP_SET_ILLUSION,       ///< イリュージョン用参照ポケモン変更
  SC_OP_CLEAR_CONSUMED_ITEM,///< アイテム消費情報のクリア
  SC_OP_CURESICK_DEPEND_POKE,///< ポケモン依存状態異常のクリア
  SC_OP_WAZADMG_REC,        ///< ワザダメージ記録
  SC_OP_TURN_CHECK,         ///< ポケモン１体ターンチェック
  SC_OP_TURN_CHECK_FIELD,   ///< フィールド効果ターンチェック
  SC_ACT_WAZA_EFFECT,
  SC_ACT_TAMEWAZA_HIDE,     ///< 【アクション】そらをとぶなどで画面から消える・現れる設定
  SC_ACT_WAZA_DMG,          ///< 【アクション】[ AtClient, DefClient, wazaIdx, Affinity ]
  SC_ACT_WAZA_DMG_PLURAL,   ///< 【アクション】複数体同時ダメージ処理 [ pokeCnt, affAbout, ... ]
  SC_ACT_WAZA_ICHIGEKI,     ///< 【アクション】一撃ワザ処理
  SC_ACT_SICK_ICON,         ///< 【アクション】ゲージに状態異常アイコン表示
  SC_ACT_CONF_DMG,          ///< 【アクション】こんらん自爆ダメージ [ pokeID ]
  SC_ACT_RANKUP,            ///< 【ランクアップ効果】 ○○の×××があがった！[ ClientID, statusType, volume ]
  SC_ACT_RANKDOWN,          ///< 【ランクダウン効果】 ○○の×××がさがった！[ ClientID, statusType, volume ]
  SC_ACT_DEAD,              ///< 【ポケモンひんし】
  SC_ACT_RELIVE,            ///< 【ポケモン生き返り】
  SC_ACT_MEMBER_OUT_MSG,    ///< 【ポケモン退場メッセージ】[ ClientID, pokeID ]
  SC_ACT_MEMBER_OUT,        ///< 【ポケモン退場】[ ClientID, memberIdx ]
  SC_ACT_MEMBER_IN,         ///< 【ポケモンイン】[ ClientID, posIdx, memberIdx ]
  SC_ACT_WEATHER_DMG,       ///< 天候による一斉ダメージ処理[ weather, pokeCnt ]
  SC_ACTOP_WEATHER_START,   ///< 天候変化
  SC_ACTOP_WEATHER_END,       ///< ターンチェックで天候終了
  SC_ACT_SIMPLE_HP,         ///< シンプルなHPゲージ増減処理
  SC_ACT_KINOMI,            ///< きのみを食べる
  SC_ACT_KILL,              ///< 強制瀕死演出（みちづれ、一撃ワザなど）
  SC_ACTOP_MOVE,            ///< ムーブ
  SC_ACT_EXP,               ///< 経験値取得
  SC_ACT_BALL_THROW,        ///< ボール投げ
  SC_ACT_BALL_THROW_TR,     ///< ボール投げ（トレーナー戦）
  SC_ACT_ROTATION,          ///< ローテーション
  SC_ACTOP_CHANGE_TOKUSEI,  ///< とくせい変更
  SC_ACTOP_SWAP_TOKUSEI,    ///< とくせいスワップ
  SC_ACT_FAKE_DISABLE,      ///< イリュージョン解除
  SC_ACT_EFFECT_SIMPLE,     ///< エフェクト発動（パラメータ指定なし）
  SC_ACT_EFFECT_BYPOS,      ///< 指定位置にエフェクト発動
  SC_ACT_EFFECT_BYVECTOR,   ///< 指定発動位置＆終点位置でエフェクト発動
  SC_ACT_CHANGE_FORM,       ///< フォルムナンバーチェンジ
  SC_ACT_RESET_MOVE,        ///< リセットムーブカットイン
  SC_ACT_MIGAWARI_CREATE,   ///< みがわり作成
  SC_ACT_MIGAWARI_DELETE,   ///< みがわり消去
  SC_ACT_HENSIN,            ///< へんしん
  SC_ACT_MIGAWARI_DAMAGE,   ///< みがわりダメージ
  SC_ACT_WIN_BGM,           ///< 勝利BGM再生
  SC_ACT_MSGWIN_HIDE,       ///< メッセージウィンドウフェードアウト
  SC_TOKWIN_IN,             ///< とくせいウィンドウ表示イン [ClientID]
  SC_TOKWIN_OUT,            ///< とくせいウィンドウ表示アウト [ClientID]
  SC_MSG_WAZA,              ///< ワザメッセージ表示[ ClientID, wazaIdx ]
  SC_MSG_STD,               ///< メッセージ表示 [MsgID, numArgs, arg1, arg2, ... ]
  SC_MSG_SET,               ///< メッセージ表示 [MsgID, numArgs, arg1, arg2, ... ]
  SC_MSG_STD_SE,            ///< メッセージ表示＆SE [MsgID, SENo, numArgs, arg1, arg2, ... ]
  SC_MSG_SET_SE,            ///< メッセージ表示＆SE [MsgID, SENo, numArgs, arg1, arg2, ... ]

  SC_MAX,

  SCEX_RESERVE,

}ServerCmd;

//--------------------------------------------------------------
/**
 *    サーバキュー構造
 */
//--------------------------------------------------------------
typedef struct {
  u32   writePtr;
  u32   readPtr;
  u8    buffer[BTL_SERVER_CMD_QUE_SIZE];
}BTL_SERVER_CMD_QUE;

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
static inline void SCQUE_Init( BTL_SERVER_CMD_QUE* que )
{
  que->writePtr = 0;
  que->readPtr = 0;
}
static inline void SCQUE_Setup( BTL_SERVER_CMD_QUE* que, const void* data, u16 dataLength )
{
  GF_ASSERT(dataLength<BTL_SERVER_CMD_QUE_SIZE);

  GFL_STD_MemCopy32( data, que->buffer, dataLength );
  que->writePtr = dataLength;
  que->readPtr = 0;
}

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
extern void scque_put1byte( BTL_SERVER_CMD_QUE* que, u8 data );
extern u8 scque_read1byte( BTL_SERVER_CMD_QUE* que );
extern void scque_put2byte( BTL_SERVER_CMD_QUE* que, u16 data );
extern u16 scque_read2byte( BTL_SERVER_CMD_QUE* que );
extern void scque_put3byte( BTL_SERVER_CMD_QUE* que, u32 data );
extern u32 scque_read3byte( BTL_SERVER_CMD_QUE* que );
extern void scque_put4byte( BTL_SERVER_CMD_QUE* que, u32 data );
extern u32 scque_read4byte( BTL_SERVER_CMD_QUE* que );

//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
static inline BOOL SCQUE_IsFinishRead( const BTL_SERVER_CMD_QUE* que )
{
  return que->readPtr == que->writePtr;
}


//----------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------
extern void SCQUE_PUT_Common( BTL_SERVER_CMD_QUE* que, ServerCmd cmd, ... );

//---------------------------------------------
static inline void SCQUE_PUT_OP_HpMinus( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 value )
{
  SCQUE_PUT_Common( que, SC_OP_HP_MINUS, pokeID, value );
}

static inline void SCQUE_PUT_OP_HpPlus( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 value )
{
  SCQUE_PUT_Common( que, SC_OP_HP_PLUS, pokeID, value );
}
static inline void SCQUE_PUT_OP_HpZero( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_HP_ZERO, pokeID );
}

static inline void SCQUE_PUT_OP_PPMinus( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 wazaIdx, u8 value )
{
  SCQUE_PUT_Common( que, SC_OP_PP_MINUS, pokeID, wazaIdx, value );
}
static inline void SCQUE_PUT_OP_PPMinus_Org( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 wazaIdx, u8 value )
{
  SCQUE_PUT_Common( que, SC_OP_PP_MINUS_ORG, pokeID, wazaIdx, value );
}
static inline void SCQUE_PUT_OP_WazaUsed( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 wazaIdx )
{
  SCQUE_PUT_Common( que, SC_OP_WAZA_USED, pokeID, wazaIdx );
}
static inline void SCQUE_PUT_OP_PPPlus( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 wazaIdx, u8 value )
{
  SCQUE_PUT_Common( que, SC_OP_PP_PLUS, pokeID, wazaIdx, value );
}
static inline void SCQUE_PUT_OP_PPPlus_Org( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 wazaIdx, u8 value )
{
  SCQUE_PUT_Common( que, SC_OP_PP_PLUS_ORG, pokeID, wazaIdx, value );
}

static inline void SCQUE_PUT_OP_RankUp( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 statusType, u8 volume )
{
  SCQUE_PUT_Common( que, SC_OP_RANK_UP, pokeID, statusType, volume );
}

static inline void SCQUE_PUT_OP_RankDown( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 statusType, u8 volume )
{
  SCQUE_PUT_Common( que, SC_OP_RANK_DOWN, pokeID, statusType, volume );
}

static inline void SCQUE_PUT_OP_RankSet7( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 atk, u8 def, u8 sp_atk, u8 sp_def, u8 agi, u8 hit, u8 avoid )
{
  SCQUE_PUT_Common( que, SC_OP_RANK_SET7, pokeID, atk, def, sp_atk, sp_def, agi, hit, avoid );
}
static inline void SCQUE_PUT_OP_RankRecover( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_RANK_RECOVER, pokeID );
}
static inline void SCQUE_PUT_OP_RankReset( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_RANK_RESET, pokeID );
}

static inline void SCQUE_PUT_OP_AddCritical( BTL_SERVER_CMD_QUE* que, u8 pokeID, int value )
{
  SCQUE_PUT_Common( que, SC_OP_ADD_CRITICAL, pokeID, value );
}


static inline void SCQUE_PUT_OP_SetSick( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 sick, u32 contParam )
{
  SCQUE_PUT_Common( que, SC_OP_SICK_SET, pokeID, sick, contParam );
}

static inline void SCQUE_PUT_OP_CurePokeSick( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_CURE_POKESICK, pokeID );
}
static inline void SCQUE_PUT_OP_CureWazaSick( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 sickID )
{
  SCQUE_PUT_Common( que, SC_OP_CURE_WAZASICK, pokeID, sickID );
}
static inline void SCQUE_PUT_OP_MemberIn( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 posIdx, u8 memberIdx, u16 turnCount )
{
  SCQUE_PUT_Common( que, SC_OP_MEMBER_IN, clientID, posIdx, memberIdx, turnCount );
}
static inline void SCQUE_PUT_OP_SetStatus( BTL_SERVER_CMD_QUE* que, u8 pokeID, BppValueID vid, u8 value )
{
  SCQUE_PUT_Common( que, SC_OP_SET_STATUS, pokeID, vid, value );
}
static inline void SCQUE_PUT_OP_SetWeight( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 weight )
{
  SCQUE_PUT_Common( que, SC_OP_SET_WEIGHT, pokeID, weight );
}

static inline void SCQUE_PUT_OP_WazaSickTurnCheck( BTL_SERVER_CMD_QUE* que, u8 pokeCnt, u8 maxSickIdx, u32 packID )
{
  SCQUE_PUT_Common( que, SC_OP_WAZASICK_TURNCHECK, pokeCnt, maxSickIdx, packID );
}

static inline void SCQUE_PUT_OP_ChangePokeType( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 typePair )
{
  SCQUE_PUT_Common( que, SC_OP_CHANGE_POKETYPE, pokeID, typePair );
}
static inline void SCQUE_PUT_OP_ConsumeItem( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 itemID  )
{
  SCQUE_PUT_Common( que, SC_OP_CONSUME_ITEM, pokeID, itemID );
}

static inline void SCQUE_PUT_OP_UpdateWazaProcResult( BTL_SERVER_CMD_QUE* que,
    u8 pokeID, BtlPokePos actTargetPos, BOOL fActEnable, PokeType actWazaType, WazaID actWaza, WazaID orgWaza )
{
  SCQUE_PUT_Common( que, SC_OP_UPDATE_USE_WAZA, pokeID, actTargetPos, fActEnable, actWazaType, actWaza, orgWaza );

}


static inline void SCQUE_PUT_OP_SetTurnFlag( BTL_SERVER_CMD_QUE* que, u8 pokeID, BppTurnFlag flag )
{
  SCQUE_PUT_Common( que, SC_OP_SET_TURNFLAG, pokeID, flag );
}
static inline void SCQUE_PUT_OP_ResetTurnFlag( BTL_SERVER_CMD_QUE* que, u8 pokeID, BppTurnFlag flag )
{
  SCQUE_PUT_Common( que, SC_OP_RESET_TURNFLAG, pokeID, flag );
}



static inline void SCQUE_PUT_OP_SetContFlag( BTL_SERVER_CMD_QUE* que, u8 pokeID, BppContFlag flag )
{
  SCQUE_PUT_Common( que, SC_OP_SET_CONTFLAG, pokeID, flag );
}
static inline void SCQUE_PUT_OP_ResetContFlag( BTL_SERVER_CMD_QUE* que, u8 pokeID, BppContFlag flag )
{
  SCQUE_PUT_Common( que, SC_OP_RESET_CONTFLAG, pokeID, flag );
}
static inline void SCQUE_PUT_OP_ChangeTokusei( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 tokID )
{
  SCQUE_PUT_Common( que, SC_OP_CHANGE_TOKUSEI, pokeID, tokID );
}
static inline void SCQUE_PUT_OP_SetItem( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 itemID )
{
  SCQUE_PUT_Common( que, SC_OP_SET_ITEM, pokeID, itemID );
}
static inline void SCQUE_PUT_OP_UpdateWaza( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 wazaIdx, u16 wazaID, u8 ppMax, u8 fPermanent )
{
  SCQUE_PUT_Common( que, SC_OP_UPDATE_WAZANUMBER, pokeID, wazaIdx, ppMax, fPermanent, wazaID );
}
static inline void SCQUE_PUT_OP_OutClear( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_OUTCLEAR, pokeID );
}
static inline void SCQUE_PUT_OP_AddFieldEffect( BTL_SERVER_CMD_QUE* que, u8 eff, u32 cont )
{
  SCQUE_PUT_Common( que, SC_OP_ADD_FLDEFF, eff, cont );
}
static inline void SCQUE_PUT_OP_AddFieldEffectDepend( BTL_SERVER_CMD_QUE* que, u8 eff, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_ADD_FLDEFF_DEPEND, eff, pokeID );
}
static inline void SCQUE_PUT_OP_DeleteDependPokeFieldEffect( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_DEL_FLDEFF_DEPEND, pokeID );
}
static inline void SCQUE_PUT_OP_RemoveFieldEffect( BTL_SERVER_CMD_QUE* que, u8 eff )
{
  SCQUE_PUT_Common( que, SC_OP_REMOVE_FLDEFF, eff );
}
static inline void SCQUE_PUT_OP_SetPokeCounter( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 counterID, u8 value )
{
  SCQUE_PUT_Common( que, SC_OP_SET_POKE_COUNTER, pokeID, counterID, value );
}
static inline void SCQUE_PUT_OP_BatonTouch( BTL_SERVER_CMD_QUE* que, u8 userPokeID, u8 targetPokeID )
{
  SCQUE_PUT_Common( que, SC_OP_BATONTOUCH, userPokeID, targetPokeID );
}
static inline void SCQUE_PUT_OP_MigawariCreate( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 migawariHP )
{
  SCQUE_PUT_Common( que, SC_OP_MIGAWARI_CREATE, pokeID, migawariHP );
}
static inline void SCQUE_PUT_OP_MigawariDelete( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_MIGAWARI_DELETE, pokeID );
}
static inline void SCQUE_PUT_OP_ShooterCharge( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 increment )
{
  SCQUE_PUT_Common( que, SC_OP_SHOOTER_CHARGE, clientID, increment );
}
static inline void SCQUE_PUT_OP_SetIllusionForParty( BTL_SERVER_CMD_QUE* que, u8 clientID )
{
  SCQUE_PUT_Common( que, SC_OP_SET_ILLUSION, clientID );
}
static inline void SCQUE_PUT_OP_ClearConsumedItem( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_CLEAR_CONSUMED_ITEM, pokeID );
}
static inline void SCQUE_PUT_OP_CureSickDependPoke( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 dependPokeID )
{
  SCQUE_PUT_Common( que, SC_OP_CURESICK_DEPEND_POKE, pokeID, dependPokeID );
}

static inline void SCQUE_OP_AddWazaDmgRec( BTL_SERVER_CMD_QUE* que, u8 defPokeID, u8 atkPokeID, BtlPokePos pokePos, PokeType wazaType, u16 wazaID, u16 damage )
{
  SCQUE_PUT_Common( que, SC_OP_WAZADMG_REC, defPokeID, atkPokeID, pokePos, wazaType, wazaID, damage );
}
static inline void SCQUE_PUT_OP_TurnCheck( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_OP_TURN_CHECK, pokeID );
}
static inline void SCQUE_PUT_OP_TurnCheckField( BTL_SERVER_CMD_QUE* que )
{
  SCQUE_PUT_Common( que, SC_OP_TURN_CHECK_FIELD, 0 );
}





//---------------------------------------------
static inline void SCQUE_PUT_ACT_WazaEffect( BTL_SERVER_CMD_QUE* que, u8 atPokePos, u8 defPokePos, u16 waza, u8 arg )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_EFFECT, atPokePos, defPokePos, waza, arg );
}
/*
static inline void SCQUE_PUT_ACT_WazaEffectEx( BTL_SERVER_CMD_QUE* que, u8 atPokePos, u8 defPokePos, u16 waza, u8 arg )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_EFFECT_EX, atPokePos, defPokePos, waza, arg );
}
*/
// 溜めワザ（そらをとぶ等）の開始時に姿を消す・攻撃後に姿を現す
static inline void SCQUE_PUT_ACT_TameWazaHide( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 hideFlag )
{
  SCQUE_PUT_Common( que, SC_ACT_TAMEWAZA_HIDE, pokeID, hideFlag );
}



// 【アクション】単体ダメージ処理
static inline void SCQUE_PUT_ACT_WazaDamage( BTL_SERVER_CMD_QUE* que, u8 defPokeID, u8 affAbout, u16 wazaID )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_DMG, defPokeID, affAbout, wazaID );
}
// 【アクション】複数体同時ダメージ処理
static inline void SCQUE_PUT_ACT_WazaDamagePlural( BTL_SERVER_CMD_QUE* que, u8 pokeCnt, u8 affAbout, u16 wazaID )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_DMG_PLURAL, pokeCnt, affAbout, wazaID );
}

// 【アクション】一撃必殺ワザ処理
static inline void SCQUE_PUT_ACT_WazaIchigeki( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_WAZA_ICHIGEKI, pokeID );
}

// 【アクション】ゲージに状態異常アイコンを表示（POKESICK_NULLで表示オフ）
static inline void SCQUE_PUT_ACT_SickIcon( BTL_SERVER_CMD_QUE* que, u8 pokeID, PokeSick sick )
{
  SCQUE_PUT_Common( que, SC_ACT_SICK_ICON, pokeID, sick );
}

// 【アクション】こんらん自爆ダメージ
static inline void SCQUE_PUT_ACT_ConfDamage( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_CONF_DMG, pokeID );
}
// 【アクション】能力ランクアップ
static inline void SCQUE_PUT_ACT_RankUp( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 statusType, u8 volume )
{
  SCQUE_PUT_Common( que, SC_ACT_RANKUP, pokeID, statusType, volume );
}
// 【アクション】能力ランクダウン
static inline void SCQUE_PUT_ACT_RankDown( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 statusType, u8 volume )
{
  SCQUE_PUT_Common( que, SC_ACT_RANKDOWN, pokeID, statusType, volume );
}
// 【アクション】ポケモンひんし
static inline void SCQUE_PUT_ACT_Dead( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_DEAD, pokeID );
}
// 【アクション】ポケモン生き返り
static inline void SCQUE_PUT_ACT_RelivePoke( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_RELIVE, pokeID );
}
// 【アクション】ポケモン退場メッセージ表示
static inline void SCQUE_PUT_ACT_MemberOutMsg( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_MEMBER_OUT_MSG, clientID, pokeID );
}
// 【アクション】ポケモン退場
static inline void SCQUE_PUT_ACT_MemberOut( BTL_SERVER_CMD_QUE* que, BtlPokePos pos, u16 effectNo )
{
  SCQUE_PUT_Common( que, SC_ACT_MEMBER_OUT, pos, effectNo );
}
// 【アクション】ポケモン入場
static inline void SCQUE_PUT_ACT_MemberIn( BTL_SERVER_CMD_QUE* que, u8 clientID, u8 posIdx, u8 memberIdx, u8 fPutMsg )
{
  SCQUE_PUT_Common( que, SC_ACT_MEMBER_IN, clientID, posIdx, memberIdx, fPutMsg );
}
// 【アクション】天候による一斉ダメージ   weather:天候ID, pokeCnt:ダメージを受けるポケモン数
static inline void SCQUE_PUT_ACT_WeatherDamage( BTL_SERVER_CMD_QUE* que, u8 weather, u8 pokeCnt )
{
  SCQUE_PUT_Common( que, SC_ACT_WEATHER_DMG, weather, pokeCnt );
}

static inline void SCQUE_PUT_ACTOP_WeatherStart( BTL_SERVER_CMD_QUE* que, u8 weather, u8 turn )
{
  SCQUE_PUT_Common( que, SC_ACTOP_WEATHER_START, weather, turn );
}

static inline void SCQUE_PUT_ACTOP_WeatherEnd( BTL_SERVER_CMD_QUE* que, u8 weather )
{
  SCQUE_PUT_Common( que, SC_ACTOP_WEATHER_END, weather );
}

static inline void SCQUE_PUT_ACT_SimpleHP( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_SIMPLE_HP, pokeID );
}

static inline void SCQUE_PUT_ACT_KINOMI( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_KINOMI, pokeID );
}
static inline void SCQUE_PUT_ACT_Kill( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 effectType )
{
  SCQUE_PUT_Common( que, SC_ACT_KILL, pokeID, effectType );
}
static inline void SCQUE_PUT_ACTOP_MemberMove( BTL_SERVER_CMD_QUE* que, u8 clientID, BtlPokePos pos1, BtlPokePos pos2 )
{
  SCQUE_PUT_Common( que, SC_ACTOP_MOVE, clientID, pos1, pos2 );
}
static inline void SCQUE_PUT_ACT_AddExp( BTL_SERVER_CMD_QUE* que, u8 pokeID, u32 exp )
{
  SCQUE_PUT_Common( que, SC_ACT_EXP, pokeID, exp );
}
static inline void SCQUE_PUT_ACT_BallThrow( BTL_SERVER_CMD_QUE* que, BtlPokePos pos, u8 yureCnt,
    u8 fSuccess, u8 fZukanRegister, u8 fCritical, u16 ballItemID )
{
  SCQUE_PUT_Common( que, SC_ACT_BALL_THROW, pos, yureCnt, fSuccess, fZukanRegister, fCritical, ballItemID );
}
static inline void SCQUE_PUT_ACT_BallThrowTrainer( BTL_SERVER_CMD_QUE* que, BtlPokePos pos, u16 ballItemID )
{
  SCQUE_PUT_Common( que, SC_ACT_BALL_THROW_TR, pos, ballItemID );
}
static inline void SCQUE_PUT_ACT_Rotation( BTL_SERVER_CMD_QUE* que, u8 clientID, BtlRotateDir dir )
{
  SCQUE_PUT_Common( que, SC_ACT_ROTATION, clientID, dir );
}
static inline void SCQUE_PUT_ACTOP_ChangeTokusei( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 tokuseiID )
{
  SCQUE_PUT_Common( que, SC_ACTOP_CHANGE_TOKUSEI, pokeID, tokuseiID );
}
static inline void SCQUE_PUT_ACTOP_SwapTokusei( BTL_SERVER_CMD_QUE* que, u8 pokeID_1, u8 pokeID_2, u16 tokID_1, u16 tokID_2 )
{
  SCQUE_PUT_Common( que, SC_ACTOP_SWAP_TOKUSEI, pokeID_1, pokeID_2, tokID_1, tokID_2 );
}
static inline void SCQUE_PUT_ACT_FakeDisable( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_FAKE_DISABLE, pokeID );
}
static inline void SCQUE_PUT_ACT_EffectSimple( BTL_SERVER_CMD_QUE* que, u16 effectNo )
{
  SCQUE_PUT_Common( que, SC_ACT_EFFECT_SIMPLE, effectNo );
}
static inline void SCQUE_PUT_ACT_EffectByPos( BTL_SERVER_CMD_QUE* que, BtlPokePos pos, u16 effectNo )
{
  SCQUE_PUT_Common( que, SC_ACT_EFFECT_BYPOS, pos, effectNo );
}
static inline void SCQUE_PUT_ACT_EffectByVector( BTL_SERVER_CMD_QUE* que, BtlPokePos pos_from, BtlPokePos pos_to, u16 effectNo )
{
  SCQUE_PUT_Common( que, SC_ACT_EFFECT_BYVECTOR, pos_from, pos_to, effectNo );
}
static inline void SCQUE_PUT_ACT_ChangeForm( BTL_SERVER_CMD_QUE* que, u8 pokeID, u8 formNo )
{
  SCQUE_PUT_Common( que, SC_ACT_CHANGE_FORM, pokeID, formNo );
}
static inline void SCQUE_PUT_ACT_TripleResetMove( BTL_SERVER_CMD_QUE* que, u8 clientID_1, u8 posIdx1, u8 clientID_2, u8 posIdx2 )
{
  SCQUE_PUT_Common( que, SC_ACT_RESET_MOVE, clientID_1, posIdx1, clientID_2, posIdx2 );
}
static inline void SCQUE_PUT_ACT_MigawariCreate( BTL_SERVER_CMD_QUE* que, BtlPokePos pos )
{
  SCQUE_PUT_Common( que, SC_ACT_MIGAWARI_CREATE, pos );
}
static inline void SCQUE_PUT_ACT_MigawariDelete( BTL_SERVER_CMD_QUE* que, BtlPokePos pos )
{
  SCQUE_PUT_Common( que, SC_ACT_MIGAWARI_DELETE, pos );
}
static inline void SCQUE_PUT_ACT_Hensin( BTL_SERVER_CMD_QUE* que, u8 atkPokeID, u8 tgtPokeID )
{
  SCQUE_PUT_Common( que, SC_ACT_HENSIN, atkPokeID, tgtPokeID );
}
static inline void SCQUE_PUT_ACT_MigawariDamage( BTL_SERVER_CMD_QUE* que, u8 defPokeID, u8 affine, WazaID waza )
{
  SCQUE_PUT_Common( que, SC_ACT_MIGAWARI_DAMAGE, defPokeID, affine, waza );
}
static inline void SCQUE_PUT_ACT_PlayWinBGM( BTL_SERVER_CMD_QUE* que, u16 bgmNo )
{
  SCQUE_PUT_Common( que, SC_ACT_WIN_BGM, bgmNo );
}
static inline void SCQUE_PUT_ACT_MsgWinHide( BTL_SERVER_CMD_QUE* que, u8 dmy )
{
  SCQUE_PUT_Common( que, SC_ACT_MSGWIN_HIDE, dmy );
}




static inline void SCQUE_PUT_TOKWIN_IN( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_TOKWIN_IN, pokeID );
}
static inline void SCQUE_PUT_TOKWIN_OUT( BTL_SERVER_CMD_QUE* que, u8 pokeID )
{
  SCQUE_PUT_Common( que, SC_TOKWIN_OUT, pokeID );
}


static inline void SCQUE_PUT_MSG_WAZA( BTL_SERVER_CMD_QUE* que, u8 pokeID, u16 waza )
{
  SCQUE_PUT_Common( que, SC_MSG_WAZA, pokeID, waza );
}

//=====================================================
typedef u32 ScMsgArg;
enum {
  MSGARG_TERMINATOR = 0xffff0000,
};

#include <stdarg.h>

// ↓ 可変部分の最初の引数には必ず文字列IDを渡す。
extern void SCQUE_PUT_MsgImpl( BTL_SERVER_CMD_QUE* que, u8 scType, ... );
#define SCQUE_PUT_MSG_STD(que, ...) SCQUE_PUT_MsgImpl( que, SC_MSG_STD, __VA_ARGS__, MSGARG_TERMINATOR )
#define SCQUE_PUT_MSG_SET(que, ...) SCQUE_PUT_MsgImpl( que, SC_MSG_SET, __VA_ARGS__, MSGARG_TERMINATOR )
#define SCQUE_PUT_MSG_STD_SE(que, ...) SCQUE_PUT_MsgImpl( que, SC_MSG_STD_SE, __VA_ARGS__, MSGARG_TERMINATOR )
#define SCQUE_PUT_MSG_SET_SE(que, ...) SCQUE_PUT_MsgImpl( que, SC_MSG_SET_SE, __VA_ARGS__, MSGARG_TERMINATOR )

//=====================================================

extern u16  SCQUE_RESERVE_Pos( BTL_SERVER_CMD_QUE* que, ServerCmd cmd );
extern void SCQUE_PUT_ReservedPos( BTL_SERVER_CMD_QUE* que, u16 pos, ServerCmd cmd, ... );

extern ServerCmd SCQUE_Read( BTL_SERVER_CMD_QUE* que, int* args );

extern void SCQUE_PUT_ArgOnly( BTL_SERVER_CMD_QUE* que, u8 arg );
extern u8 SCQUE_READ_ArgOnly( BTL_SERVER_CMD_QUE* que );

#endif
