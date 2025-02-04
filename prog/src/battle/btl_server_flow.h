//=============================================================================================
/**
 * @file  btl_server_flow.h
 * @brief ポケモンWB バトルシステム サーバコマンド生成処理
 * @author  taya
 *
 * @date  2009.03.06  作成
 */
//=============================================================================================
#ifndef __BTL_SERVER_FLOW_H__
#define __BTL_SERVER_FLOW_H__

#include "waza_tool/wazadata.h"

#include "btl_main.h"
#include "btl_adapter.h"
#include "btl_server.h"
#include "btl_server_cmd.h"
#include "btl_string.h"
#include "btl_sideeff.h"
#include "btl_poseff.h"
#include "btl_field.h"
#include "btl_wazarec.h"
#include "btl_deadrec.h"
#include "btl_action.h"
#include "btl_calc.h"

#include "app/b_bag.h"

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
typedef enum {
  SVFLOW_RESULT_DEFAULT = 0,  ///< そのまま続行
  SVFLOW_RESULT_POKE_CHANGE,  ///< ターン途中のポケモン入れ替えリクエスト
  SVFLOW_RESULT_POKE_COVER,   ///< 空き位置にポケモンを投入するリクエスト
  SVFLOW_RESULT_LEVELUP,      ///< レベルアップ処理
  SVFLOW_RESULT_BTL_SHOWDOWN, ///< バトル終了（決着）
  SVFLOW_RESULT_BTL_QUIT,     ///< バトル終了（逃げ）
  SVFLOW_RESULT_POKE_GET,     ///< バトル終了（ポケモン捕獲）
}SvflowResult;


/*--------------------------------------------------------------------------*/
/* Structures                                                               */
/*--------------------------------------------------------------------------*/

/**
 *  サーバ本体、サーバフロー双方から参照するクライアントパラメータ
 */

struct _SVCL_WORK {

  BTL_ADAPTER*    adapter;
  BTL_PARTY*      party;
  u8        memberCount;
  u8        numCoverPos;
  u8        isLocalClient;
  u8        myID;
};

/**
 *  サーバフロー＆ハンドラから参照するワザパラメータ
 */

typedef struct {

  WazaID          wazaID;
  WazaID          orgWazaID;    ///< 派生元のワザID
  PokeTypePair    userType;
  PokeType        wazaType;
  WazaDamageType  damageType;
  WazaTarget      targetType;

  union {
    u32 flags;

    struct {
      u32             fMagicCoat : 1; ///< マジックコート処理中
      u32             fYokodori  : 1; ///< よこどり処理中
      u32             fReqWaza   : 1; ///< 派生ワザ処理中
      u32             _padding   : 29;
    };
  };

}SVFL_WAZAPARAM;

/*--------------------------------------------------------------------------*/
/* Typedefs                                                                 */
/*--------------------------------------------------------------------------*/
#include "btl_server_flow_def.h"


// server -> server_flow
extern BTL_SVFLOW_WORK* BTL_SVFLOW_InitSystem(
  BTL_SERVER* server, BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokeCon,
  BTL_SERVER_CMD_QUE* que, BtlBagMode bagMode, HEAPID heapID );

extern void BTL_SVFLOW_ResetSystem( BTL_SVFLOW_WORK* wk );
extern void BTL_SVFLOW_QuitSystem( BTL_SVFLOW_WORK* wk );

extern BOOL BTL_SVFLOW_StartBtlIn( BTL_SVFLOW_WORK* wk );

extern void BTL_SVFLOW_StartTurn_Boot( BTL_SVFLOW_WORK* wk );
extern SvflowResult BTL_SVFLOW_StartTurn( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction );

extern void BTL_SVFLOW_ContinueAfterPokeChange_Boot( BTL_SVFLOW_WORK* wk );
extern SvflowResult BTL_SVFLOW_ContinueAfterPokeChange( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction );

extern void BTL_SVFLOW_StartAfterPokeIn_Boot( BTL_SVFLOW_WORK* wk );
extern SvflowResult BTL_SVFLOW_StartAfterPokeIn( BTL_SVFLOW_WORK* wk, const BTL_SVCL_ACTION* clientAction );



extern BOOL BTL_SVFLOW_MakeShooterChargeCommand( BTL_SVFLOW_WORK* wk );
extern BOOL BTL_SVFLOW_MakePlayerEscapeCommand( BTL_SVFLOW_WORK* wk );

//------------------------------------------------
// サーバ本体との連絡関数
//------------------------------------------------
// server_flow -> server
extern SVCL_WORK* BTL_SERVER_GetClientWork( BTL_SERVER* server, u8 clientID );
extern SVCL_WORK* BTL_SERVER_GetClientWorkIfEnable( BTL_SERVER* server, u8 clientID );
extern BOOL BTL_SERVER_IsClientEnable( const BTL_SERVER* server, u8 clientID );
extern u8 BTL_SVCL_GetNumActPoke( SVCL_WORK* clwk );
extern const BTL_ACTION_PARAM* BTL_SVCL_GetPokeAction( SVCL_WORK* clwk, u8 posIdx );
extern BTL_ACTION_PARAM BTL_SVCL_ACTION_Get( const BTL_SVCL_ACTION* clientAction, u8 clientID, u8 posIdx );


extern void BTL_SVF_SickDamageRecall( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, WazaSick sickID, u32 damage );
extern const BTL_ESCAPEINFO* BTL_SVFLOW_GetEscapeInfoPointer( const BTL_SVFLOW_WORK* wk );
extern BtlPokePos BTL_SVFLOW_GetCapturedPokePos( const BTL_SVFLOW_WORK* wk );
extern BtlResult BTL_SVFLOW_ChecBattleResult( BTL_SVFLOW_WORK* wk );


extern u16 BTL_SVF_GetTypeAffCnt_PutVoid( const BTL_SVFLOW_WORK* wk );
extern u16 BTL_SVF_GetTypeAffCnt_PutAdvantage( const BTL_SVFLOW_WORK* wk );
extern u16 BTL_SVF_GetTypeAffCnt_PutDisadvantage( const BTL_SVFLOW_WORK* wk );
extern u16 BTL_SVF_GetTypeAffCnt_RecvVoid( const BTL_SVFLOW_WORK* wk );
extern u16 BTL_SVF_GetTypeAffCnt_RecvAdvantage( const BTL_SVFLOW_WORK* wk );
extern u16 BTL_SVF_GetTypeAffCnt_RecvDisadvantage( const BTL_SVFLOW_WORK* wk );


//------------------------------------------------
// イベントハンドラとの連絡関数
//------------------------------------------------
extern BtlRule BTL_SVFTOOL_GetRule( BTL_SVFLOW_WORK* wk );
extern BtlCompetitor BTL_SVFTOOL_GetCompetitor( BTL_SVFLOW_WORK* wk );
extern BtlPokePos BTL_SVFTOOL_GetExistFrontPokePos( BTL_SVFLOW_WORK* sv, u8 pokeID );
extern BtlPokePos BTL_SVFTOOL_GetPokeLastPos( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern u8 BTL_SVFTOOL_GetExistPokeID( BTL_SVFLOW_WORK* wk, BtlPokePos pos );
extern BOOL BTL_SVFTOOL_CheckExistTokuseiPokemon( BTL_SVFLOW_WORK* wk, PokeTokusei tokusei );
extern const BTL_POKEPARAM* BTL_SVFTOOL_GetPokeParam( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BOOL BTL_SVFTOOL_IsExistPokemon( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern u8 BTL_SVFTOOL_GetAllOpponentFrontPokeID( BTL_SVFLOW_WORK* wk, u8 basePokeID, u8* dst );
extern u16 BTL_SVFTOOL_GetTurnCount( BTL_SVFLOW_WORK* wk );
extern u8 BTL_SVFTOOL_GetPokeChangeCount( BTL_SVFLOW_WORK* wk, u8 clientID );
extern BOOL BTL_SVFTOOL_IsSimulationMode( BTL_SVFLOW_WORK* wk );
extern u32 BTL_SVFTOOL_GetPerappVoicePower( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern u8 BTL_SVFTOOL_GetClientCoverPosCount( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern u8 BTL_SVFTOOL_GetMyBenchIndex( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BOOL BTL_SVFTOOL_IsExistBenchPoke( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BOOL BTL_SVFTOOL_ReserveMemberChangeAction( BTL_SVFLOW_WORK* wk );
extern const BTL_WAZAREC* BTL_SVFTOOL_GetWazaRecord( BTL_SVFLOW_WORK* wk );
extern u16 BTL_SVFTOOL_ReserveQuePos( BTL_SVFLOW_WORK* wk, ServerCmd cmd );
extern const BTL_DEADREC* BTL_SVFTOOL_GetDeadRecord( BTL_SVFLOW_WORK* wk );
extern BtlTypeAff BTL_SVFTOOL_SimulationAffinity( BTL_SVFLOW_WORK* wk, u8 atkPokeID, u8 defPokeID, WazaID waza );
extern u32 BTL_SVFTOOL_SimulationDamage( BTL_SVFLOW_WORK* flowWk, u8 atkPokeID, u8 defPokeID, WazaID waza, BOOL fAffinity, BOOL fEnableRand );
extern BtlBgAttr BTL_SVFTOOL_GetLandForm( BTL_SVFLOW_WORK* wk );
extern u32 BTL_SVFTOOL_GetFrontPosNum( BTL_SVFLOW_WORK* wk );
extern const BTL_PARTY* BTL_SVFTOOL_GetPartyData( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern const BTL_PARTY* BTL_SVFTOOL_GetFriendPartyData( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BtlPokePos BTL_SVFTOOL_PokeIDtoPokePos( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BtlPokePos BTL_SVFTOOL_ReqWazaTargetAuto( BTL_SVFLOW_WORK* wk, u8 pokeID, WazaID waza );
extern u8 BTL_SVFTOOL_PokePosToPokeID( BTL_SVFLOW_WORK* wk, u8 pokePos );
extern BOOL BTL_SVFTOOL_GetThisTurnAction( BTL_SVFLOW_WORK* wk, u8 pokeID, BTL_ACTION_PARAM* dst );
extern BOOL BTL_SVFTOOL_IsMyActOrderLast( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern void* BTL_SVFTOOL_GetTmpWork( BTL_SVFLOW_WORK* wk, u32 size );
extern WazaID BTL_SVFTOOL_GetPrevExeWaza( BTL_SVFLOW_WORK* wk );
extern BOOL BTL_SVFTOOL_IsMultiMode( BTL_SVFLOW_WORK* wk );
extern BOOL BTL_SVFTOOL_CheckSinkaMae( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern u16 BTL_SVFTOOL_CalcAgility( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BOOL fTrickRoomEnable );
extern u16 BTL_SVFTOOL_CalcAgilityRank( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp, BOOL fTrickRoomEnable );
extern BOOL BTL_SVFTOOL_IsFloatingPoke( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BOOL BTL_SVFTOOL_CheckItemUsable( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BtlWeather BTL_SVFTOOL_GetWeather( BTL_SVFLOW_WORK* wk );
extern BOOL BTL_SVFTOOL_CheckFarPoke( BTL_SVFLOW_WORK* wk, u8 atkPokeID, u8 defPokeID, WazaID waza );
extern BOOL BTL_SVFTOOL_IsExistSideEffect( BTL_SVFLOW_WORK* wk, BtlSide side, BtlSideEffect sideEffect );
extern BOOL BTL_SVFTOOL_GetDebugFlag( BTL_SVFLOW_WORK* wk, BtlDebugFlag flag );
extern u32 BTL_SVFTOOL_GetSideEffectCount( BTL_SVFLOW_WORK* wk, BtlPokePos pos, BtlSideEffect sideEffect );
extern BOOL BTL_SVFTOOL_IsExistPosEffect( BTL_SVFLOW_WORK* wk, BtlPokePos pos, BtlPosEffect effect );
extern u8 BTL_SVFTOOL_GetFriendClientID( BTL_SVFLOW_WORK* wk, u8 clientID );
extern BOOL BTL_SVFTOOL_IsMemberOutIntr( BTL_SVFLOW_WORK* wk );
extern void BTL_SVFTOOL_AddMemberOutIntr( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BOOL BTL_SVFTOOL_IsTameHidePoke( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BOOL BTL_SVFTOOL_IsFreeFallUserPoke( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BOOL BTL_SVFTOOL_IsFreeFallPoke( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern u32 BTL_SVFTOOL_GetWeight( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern u32 BTL_SVFTOOL_GetRankEffSerial( BTL_SVFLOW_WORK* wk );
extern BOOL BTL_SVFTOOL_CheckShowDown( BTL_SVFLOW_WORK* wk );
extern void BTL_SVFRET_SetWazaEffectIndex( BTL_SVFLOW_WORK* wk, u8 effIndex );
extern BOOL BTL_SVFRET_AddBonusMoney( BTL_SVFLOW_WORK* wk, u32 volume, u8 pokeID );
extern void BTL_SVFRET_SetMoneyDblUp( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern void BTL_SVFRET_AddMagicCoatAction( BTL_SVFLOW_WORK* wk, u8 pokeID, u8 targetPokeID );
extern BOOL BTL_SVFRET_FreeFallStart( BTL_SVFLOW_WORK* wk, u8 atkPokeID, u8 targetPokeID, BOOL* fFailMsgDisped );
extern void BTL_SVFRET_FreeFallRelease( BTL_SVFLOW_WORK* wk, u8 atkPokeID );
extern void BTL_SVFRET_AddRelivePokeRecord( BTL_SVFLOW_WORK* wk, u8 pokeID );
extern BOOL BTL_SVFRET_UseItemEquip( BTL_SVFLOW_WORK* wk, BTL_POKEPARAM* bpp, u8* fFailMsgDisped );
extern void BTL_SVFRET_EventSkillCall( BTL_SVFLOW_WORK* wk, const BTL_POKEPARAM* bpp );



//----------------------------



/**
 * 特殊優先順コード（素早さより上位、ワザ優先順より下位：0〜7に収まるように！）
 */
typedef enum {

  BTL_SPPRI_LOW     = 0,
  BTL_SPPRI_DEFAULT = 1,
  BTL_SPPRI_HIGH    = 2,

}BtlSpecialPri;


/**
 *  状態異常指定の拡張コード
 */
typedef enum {

  // 全ての WazaSick コードも対象となります

  WAZASICK_EX_POKEFULL = WAZASICK_MAX,  ///< 全ポケモン系異常が対象
  WAZASICK_EX_POKEFULL_PLUS,            ///< 全ポケモン系異常＋こんらんも対象
  WAZASICK_EX_MENTAL,

}BtlWazaSickEx;

/**
 *  状態異常化失敗コード
 */
typedef enum {

  BTL_ADDSICK_FAIL_NULL = 0,
  BTL_ADDSICK_FAIL_ALREADY,     ///< すでに同じ状態になっている
  BTL_ADDSICK_FAIL_NO_EFFECT,   ///< 効果の無い相手
  BTL_ADDSICK_FAIL_OTHER,       ///< その他


}BtlAddSickFailCode;

/**
 *  状態異常上書きモード
 */
typedef enum {
  BTL_SICK_OW_NONE = 0,   ///< 上書きできない
  BTL_SICK_OW_POKESICK,   ///< 他の基本状態異常にかかっている時、無視して書き換え
  BTL_SICK_OW_STRONG,     ///< なんでも上書き
}BtlSickOverWriteMode_tag;

typedef u8 BtlSickOverWriteMode;


/**
 *  アイテム反応リクエストタイプ
 */
typedef enum {
  BTL_ITEMREACTION_GEN = 0, ///< 汎用
  BTL_ITEMREACTION_HP,      ///< HP減少による
  BTL_ITEMREACTION_PP,      ///< PP減少による
  BTL_ITEMREACTION_SICK,    ///< 状態異常による
}BtlItemReaction;

/**
 *  ワザ強制有効化モード
 */
typedef enum {
  BTL_WAZAENABLE_NONE = 0,
  BTL_WAZAENABLE_NORMAL,   ///< 通常
  BTL_WAZAENABLE_QUIT,     ///< 強制有効化後、処理中断
}BtlWazaForceEnableMode;

/**
 *  ハンドラ挙動（効果が表出するもの）
 */
typedef enum {

  BTL_HANDEX_USE_ITEM,            ///< アイテム使用
  BTL_HANDEX_USE_ITEM_EFFECT,     ///< アイテム使用エフェクトのみ実行
  BTL_HANDEX_TOKWIN_IN,           ///< とくせいウィンドウイン
  BTL_HANDEX_TOKWIN_OUT,          ///< とくせいウィンドウアウト
  BTL_HANDEX_MESSAGE,             ///< メッセージ表示
  BTL_HANDEX_RECOVER_HP,          ///< HP回復
  BTL_HANDEX_DRAIN,               ///< HP回復（ドレイン系）
  BTL_HANDEX_DAMAGE,              ///< ダメージを与える
  BTL_HANDEX_SHIFT_HP,            ///< HPを変更（ダメージ・回復として扱わない）
  BTL_HANDEX_RECOVER_PP,          ///< PP回復
  BTL_HANDEX_DECREMENT_PP,        ///< PP減少
  BTL_HANDEX_CURE_SICK,           ///< 状態異常を回復
  BTL_HANDEX_ADD_SICK,            ///< 状態異常にする
  BTL_HANDEX_SET_SICKCONT,        ///< 状態異常のパラメータ再設定
  BTL_HANDEX_RANK_EFFECT,         ///< ランク増減効果
  BTL_HANDEX_SET_RANK,            ///< ランクを指定地に強制書き換え
  BTL_HANDEX_RESET_RANK,          ///< ランク効果を全てフラットに
  BTL_HANDEX_SET_STATUS,          ///< 能力値（攻撃、防御等）を強制書き換え
  BTL_HANDEX_RECOVER_RANK,        ///< マイナスランク効果のみをフラットに
  BTL_HANDEX_KILL,                ///< 瀕死にする
  BTL_HANDEX_CHANGE_TYPE,         ///< ポケモンのタイプを変える
  BTL_HANDEX_SET_TURNFLAG,        ///< ターンフラグセット
  BTL_HANDEX_RESET_TURNFLAG,      ///< ターンフラグ強制リセット
  BTL_HANDEX_SET_CONTFLAG,        ///< 継続フラグセット
  BTL_HANDEX_RESET_CONTFLAG,      ///< 継続フラグリセット
  BTL_HANDEX_SIDEEFF_ADD,         ///< サイドエフェクト追加
  BTL_HANDEX_SIDEEFF_REMOVE,      ///< サイドエフェクト削除
  BTL_HANDEX_ADD_FLDEFF,          ///< フィールドエフェクト追加
  BTL_HANDEX_REMOVE_FLDEFF,       ///< フィールドエフェクト追加
  BTL_HANDEX_CHANGE_WEATHER,      ///< 天候変化
  BTL_HANDEX_POSEFF_ADD,          ///< 位置エフェクト追加
  BTL_HANDEX_CHANGE_TOKUSEI,      ///< とくせい書き換え
  BTL_HANDEX_SET_ITEM,            ///< アイテム書き換え
  BTL_HANDEX_CHECK_ITEM_EQUIP,    ///< アイテム装備効果発動チェック
  BTL_HANDEX_ITEM_SP,             ///< アイテム効果発動
  BTL_HANDEX_CONSUME_ITEM,        ///< 自分でアイテムを消費
  BTL_HANDEX_SWAP_ITEM,           ///< アイテム入れ替え
  BTL_HANDEX_UPDATE_WAZA,         ///< ワザ書き換え
  BTL_HANDEX_COUNTER,             ///< ポケモンカウンタ値書き換え
  BTL_HANDEX_DELAY_WAZADMG,       ///< 時間差ワザダメージ
  BTL_HANDEX_QUIT_BATTLE,         ///< バトル離脱
  BTL_HANDEX_CHANGE_MEMBER,       ///< メンバー交換
  BTL_HANDEX_BATONTOUCH,          ///< バトンタッチ（ランク効果等の引き継ぎ）
  BTL_HANDEX_ADD_SHRINK,          ///< ひるませる
  BTL_HANDEX_RELIVE,              ///< 生き返らせる
  BTL_HANDEX_SET_WEIGHT,          ///< 体重を設定
  BTL_HANDEX_PUSHOUT,             ///< 場から吹き飛ばす
  BTL_HANDEX_INTR_POKE,           ///< 指定ポケモンの割り込み行動を発生させる
  BTL_HANDEX_INTR_WAZA,           ///< 指定ワザを使おうとしているポケモンの割り込み行動を発生させる
  BTL_HANDEX_SEND_LAST,           ///< 指定ポケモンの行動を一番最後に回す
  BTL_HANDEX_SWAP_POKE,           ///< 場に出ているポケモン同士を入れ替える
  BTL_HANDEX_HENSIN,              ///< へんしんする
  BTL_HANDEX_FAKE_BREAK,          ///< イリュージョン解除
  BTL_HANDEX_JURYOKU_CHECK,       ///< じゅうりょく発動時のチェック処理
  BTL_HANDEX_TAMEHIDE_CANCEL,     ///< 特定の消え状態（そらをとぶなど）をキャンセル
  BTL_HANDEX_ADD_EFFECT,          ///< 位置指定してエフェクト発動
  BTL_HANDEX_VANISH_MSGWIN,       ///< メッセージウィンドウフェードアウト
  BTL_HANDEX_CHANGE_FORM,         ///< フォルムナンバーチェンジ
  BTL_HANDEX_SET_EFFECT_IDX,      ///< ワザエフェクトインデックス変更
  BTL_HANDEX_WAZAEFFECT_ENABLE,   ///< 強制ワザエフェクト有効
  BTL_HANDEX_FREEFALL,            ///< フリーフォール

  BTL_HANDEX_MAX,

}BtlEventHandlerExhibition;

/**
 *  ハンドラ挙動ワーク共有ヘッダ
 */
typedef struct {
  u32    equip          : 8; ///< ハンドラ挙動（BtlEventHandlerEquip）
  u32    userPokeID     : 5; ///< ハンドラ主体のポケモンID
  u32    size           :10; ///< 構造体サイズ
  u32    tokwin_flag    : 1; ///< 主体ポケモンのとくせい効果であることを表示する
  u32    failSkipFlag   : 1; ///< 直前の命令が失敗したらスキップされる
  u32    autoRemoveFlag : 1; ///< 主体ポケモンが死んでいたら自動削除
  u32    usingFlag      : 1; ///< 使用中フラグ
  u32    _padding       : 5; ///< パディング
}BTL_HANDEX_PARAM_HEADER;

/**
 *  ハンドラ用メッセージ出力パラメータ
 */
typedef struct {

  u16   ID;           ///< 文字列ID
  u16   type    : 8;  ///< 文字列タイプ（ BtlStrType  (btl_string.h) )
  u16   argCnt  : 7;  ///< 引数の数
  u16   fSEAdd  : 1;  ///< 引数終端にSEナンバー格納

  int   args[ BTL_STR_ARG_MAX ];  ///< 引数

}BTL_HANDEX_STR_PARAMS;

// クリア
extern void HANDEX_STR_Clear( BTL_HANDEX_STR_PARAMS* param );

// 内容があるか（クリア状態でないか）チェック
extern BOOL HANDEX_STR_IsEnable( const BTL_HANDEX_STR_PARAMS* param );

// 文字列ID設定
extern void HANDEX_STR_Setup( BTL_HANDEX_STR_PARAMS* param, BtlStrType type, u16 strID );

// 引数１件追加
extern void HANDEX_STR_AddArg( BTL_HANDEX_STR_PARAMS* param, int arg );

// 終端SE設定
extern void HANDEX_STR_AddSE( BTL_HANDEX_STR_PARAMS* param, u16 SENo );



typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u32     fSkipHPFull : 1;   // HP満タンなら実行しない
 u32     fUseDead    : 1;   // 死んでいても実行する
 u32     _pad        : 30;
}BTL_HANDEX_PARAM_USE_ITEM;


/**
 * メッセージ表示
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 BTL_HANDEX_STR_PARAMS  str;
}BTL_HANDEX_PARAM_MESSAGE;

/**
 * HP回復処理
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u16  recoverHP;                     ///< 回復HP量
 u8   pokeID;                        ///< 対象ポケモンID
 u8   fFailCheckThru;                ///< かいふくふうじ等のチェックを無視
 BTL_HANDEX_STR_PARAMS     exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_RECOVER_HP;

/**
 * HP吸収
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u16  recoverHP;                     ///< 回復HP量
 u8   recoverPokeID;                 ///< 回復対象ポケモンID
 u8   damagedPokeID;                 ///< ダメージを受けた側のポケモンID
 BTL_HANDEX_STR_PARAMS     exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_DRAIN;

/**
 * ダメージを与える
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u16  damage;                        ///< ダメージ量
 u8   pokeID;                        ///< 対象ポケモンID
 u8   fAvoidHidePoke  : 1;           ///< そらをとぶ状態など画面から消えているポケに効果なし
 u8   fExEffect       : 1;           ///< 成功時呼び出しエフェクト有効
 u8   _padd           : 6;
 u16  effectNo;                ///< 成功時呼び出しエフェクトナンバー
 u8   pos_from;                ///< 成功時呼び出しエフェクト開始位置（不要ならBTL_POS_NULL：初期値(0)はBTL_POS_NULLではないので注意）
 u8   pos_to;                  ///< 成功時呼び出しエフェクト終了位置（不要ならBTL_POS_NULL：初期値(0)はBTL_POS_NULLではないので注意）

 BTL_HANDEX_STR_PARAMS     exStr;    ///< 成功時メッセージ
}
BTL_HANDEX_PARAM_DAMAGE;

/**
 * HPを増減（ダメージ・回復とは見なされない）
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
  u8     poke_cnt;
  u8     fEffectDisable;              ///< ゲージエフェクト等無効
  u8     fItemReactionDisable;        ///< アイテム反応チェックを行わない
  u8     pokeID[ BTL_POS_MAX ];
  int    volume[ BTL_POS_MAX ];
}BTL_HANDEX_PARAM_SHIFT_HP;

/**
 * PPを回復
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u8   volume;                        ///< PP量
 u8   pokeID;                        ///< 対象ポケモンID
 u8   wazaIdx;                       ///< 対象ワザインデックス
 u8   fSurfacePP       : 1;          ///< 仮ワザワークのPPを対象にする
 u8   fDeadPokeEnable  : 1;          ///< ひん死のポケモンにも有効（トレーナー道具使用時）
 u8   _padd            : 6;
 BTL_HANDEX_STR_PARAMS     exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_PP;

/**
 * 対象の状態異常を回復
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;      ///< 共有ヘッダ
 BtlWazaSickEx   sickCode;              ///< 対応する状態異常コード（拡張可）
 u8   pokeID[ BTL_PARTY_MEMBER_MAX*2 ]; ///< 対象ポケモンID
 u8   poke_cnt;                         ///< 対象ポケモン数
 u8   fStdMsgDisable;                   ///< 成功時、標準メッセージを出力しない
 BTL_HANDEX_STR_PARAMS     exStr;       ///< 成功時特殊メッセージ
}BTL_HANDEX_PARAM_CURE_SICK;

/**
 * 対象を状態異常にする
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 WazaSick       sickID;              ///< 状態異常ID
 BPP_SICK_CONT  sickCont;            ///< 状態異常継続パラメータ
 u8   fAlmost;                       ///< ほぼ確定フラグ（特殊要因で失敗した場合に原因メッセージを表示する）
 u8   fStdMsgDisable;                ///< 標準メッセージオフ
 u8   fItemReactionDisable;          ///< アイテム反応オフ
 u8   pokeID;                        ///< 対象ポケモンID
 BtlSickOverWriteMode  overWriteMode;///< 上書きモード
 BTL_HANDEX_STR_PARAMS  exStr;       ///< 特殊メッセージ表示（標準メッセージはオフ）
}BTL_HANDEX_PARAM_ADD_SICK;

/**
 * ランク効果（上昇、下降）
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 WazaRankEffect rankType;            ///< ランク増減種類
 u32  effSerial;                     ///< ランク効果シリアル（不要なら0）
 s8   rankVolume;                    ///< ランク増減値(=0だと強制リセット）
 u8   fStdMsgDisable;                ///< 標準メッセージを表示しないフラグ
 u8   fAlmost;                       ///< ほぼ確定フラグ（特殊要因で失敗した場合に原因メッセージを表示する）
 u8   poke_cnt;                      ///< 対象ポケモン数
 u8   pokeID[ BTL_POS_MAX ];         ///< 対象ポケモンID
 BTL_HANDEX_STR_PARAMS  exStr;       ///< 特殊メッセージ詳細
}BTL_HANDEX_PARAM_RANK_EFFECT;

/**
 * ランク効果を特定値にセット
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u8 pokeID;                          ///< 対象ポケモンID
 s8 attack;
 s8 defence;
 s8 sp_attack;
 s8 sp_defence;
 s8 agility;
 s8 hit_ratio;
 s8 avoid_ratio;
}BTL_HANDEX_PARAM_SET_RANK;

/**
 * 対象の下がっているランク効果をフラットに戻す
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u8 pokeID;                          ///< 対象ポケモンID
}BTL_HANDEX_PARAM_RECOVER_RANK;

/**
 * 対象の全てのランク効果をフラットに戻す
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u8   poke_cnt;                      ///< 対象ポケモン数
 u8   pokeID[ BTL_POS_MAX ];         ///< 対象ポケモンID
}BTL_HANDEX_PARAM_RESET_RANK;

/**
 * 対象の能力値を直接書き換え
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u16  attack;
 u16  defence;
 u16  sp_attack;
 u16  sp_defence;
 u16  agility;
 u8  pokeID;        ///< 対象ポケモンID
 u8  fAttackEnable    : 1;
 u8  fDefenceEnable   : 1;
 u8  fSpAttackEnable  : 1;
 u8  fSpDefenceEnable : 1;
 u8  fAgilityEnable   : 1;
 u8  dmy : 3;
 BTL_HANDEX_STR_PARAMS     exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_SET_STATUS;

/**
 * 対象をひん死にする
 */
typedef struct {
 BTL_HANDEX_PARAM_HEADER   header;   ///< 共有ヘッダ
 u8   pokeID;
 u8   fDeadPokeEnable;
 BTL_HANDEX_STR_PARAMS      exStr;
}BTL_HANDEX_PARAM_KILL;

/**
 * タイプ変更処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  PokeTypePair    next_type;
  u8              pokeID;
  u8              fStdMsgDisable;
}BTL_HANDEX_PARAM_CHANGE_TYPE;

/**
 * ターンフラグセット処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BppTurnFlag     flag;
  u8              pokeID;
}BTL_HANDEX_PARAM_TURNFLAG;

/**
 * 永続フラグセット処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BppContFlag     flag;
  u8              pokeID;
}BTL_HANDEX_PARAM_SET_CONTFLAG;

typedef BTL_HANDEX_PARAM_SET_CONTFLAG  BTL_HANDEX_PARAM_RESET_CONTFLAG;

/**
 * サイドエフェクト追加処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlSideEffect   effect;
  BPP_SICK_CONT   cont;
  BtlSide         side;
  BTL_HANDEX_STR_PARAMS    exStr;
}BTL_HANDEX_PARAM_SIDEEFF_ADD;

/**
 * サイドエフェクト除去処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              flags[ BTL_SIDEEFF_BITFLG_BYTES ];
  u8              side;
}BTL_HANDEX_PARAM_SIDEEFF_REMOVE;

/**
 * フィールドエフェクト追加処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlFieldEffect           effect;
  BPP_SICK_CONT            cont;
  u8                       fAddDependPoke;  ///< 追加に失敗した時に依存対象ポケモンとして登録する

  BTL_HANDEX_STR_PARAMS    exStr;

}BTL_HANDEX_PARAM_ADD_FLDEFF;

/**
 * フィールドエフェクト除去処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlFieldEffect           effect;
}BTL_HANDEX_PARAM_REMOVE_FLDEFF;

/**
 * 天候変化処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlWeather               weather;
  u8                       turn;
  u8                       fAirLock;
  BTL_HANDEX_STR_PARAMS    exStr;

}BTL_HANDEX_PARAM_CHANGE_WEATHER;

/**
 * 位置エフェクト追加
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BtlPosEffect             effect;
  BtlPokePos               pos;
  int                      param[ BTL_POSEFF_PARAM_MAX ];
  u8                       param_cnt;
}BTL_HANDEX_PARAM_POSEFF_ADD;

/**
 *  とくせい書き換え処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u16                      tokuseiID;           ///< 書き換え後のとくせい（POKETOKUSEI_NULLならとくせいを消す）
  u8                       pokeID;              ///< 対象ポケモンID
  u8                       fSameTokEffective;   /// < おなじ「とくせい」を持つ対象にも効く
  u8                       fSkipMemberInEvent;  ///< メンバー入場イベント１回スキップ（トレース対策）
  BTL_HANDEX_STR_PARAMS    exStr;               ///< 成功時メッセージ
}BTL_HANDEX_PARAM_CHANGE_TOKUSEI;

/**
 *  アイテム書き換え処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u16             itemID;           ///< 書き換え後のアイテム（ITEM_DUMMY_DATA ならアイテムを消す）
  u8              pokeID;           ///< 対象ポケモンID
  u8              fClearConsume;    ///< 対象ポケモンの消費情報をクリア
  u8              fClearConsumeOtherPoke; ///< 対象ポケモン以外の消費情報クリア
  u8              clearConsumePokeID;     ///< 消費情報クリアするポケモンID
  BTL_HANDEX_STR_PARAMS  exStr;     ///< 成功時メッセージ
}BTL_HANDEX_PARAM_SET_ITEM;

/**
 *  アイテム入れ替え（ヘッダの使用ポケモンと）
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;             ///< 対象ポケモンID
  BTL_HANDEX_STR_PARAMS   exStr;      ///< 成功時メッセージ
  BTL_HANDEX_STR_PARAMS   exSubStr1;  ///< 成功時メッセージ
  BTL_HANDEX_STR_PARAMS   exSubStr2;  ///< 成功時メッセージ
}BTL_HANDEX_PARAM_SWAP_ITEM;

/**
 *  アイテム効果の発動チェック
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< 対象ポケモンID
  BtlItemReaction reactionType;
}BTL_HANDEX_PARAM_CHECK_ITEM_EQUIP;

/**
 *  アイテム効果の特殊使用（ついばむ、なげつけるなど対応）
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< 対象ポケモンID
  u16             itemID;
}BTL_HANDEX_PARAM_ITEM_SP;

/**
 *  アイテム消費処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u32                      fNoAction;
  BTL_HANDEX_STR_PARAMS    exStr;   ///< 成功時メッセージ
}BTL_HANDEX_PARAM_CONSUME_ITEM;


/**
 * ワザ書き換え処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< 対象ポケモンID
  u8              wazaIdx;
  u8              ppMax;             ///< PPMax値:0ならデフォルト
  u8              fPermanent;        ///< バトル後まで書き換えが残るフラグ
  WazaID          waza;
}BTL_HANDEX_PARAM_UPDATE_WAZA;

/**
 * カウンタ書き換え処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8              pokeID;            ///< 対象ポケモンID
  u8              counterID;         ///< カウンタID（BppCounter)
  u8              value;             ///< カウンタに設定する値
}BTL_HANDEX_PARAM_COUNTER;

/**
 * 時間差ワザダメージ処理用
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8                       attackerPokeID;
  u8                       targetPokeID;
  WazaID                   wazaID;
}BTL_HANDEX_PARAM_DELAY_WAZADMG;

/**
 * メンバー入れ替え処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  BTL_HANDEX_STR_PARAMS    preStr;  ///< 実行開始メッセージ
  BTL_HANDEX_STR_PARAMS    exStr;   ///< 成功時メッセージ
  u8                       pokeID;
  u8                       fIntrDisable;  ///< 割り込み（おいうちなど）禁止
}BTL_HANDEX_PARAM_CHANGE_MEMBER;

/**
 * バトンタッチ処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8  userPokeID;     ///< 状態を引き継がせるポケID
  u8  targetPokeID;   ///< 状態を引き継ぐポケID
}BTL_HANDEX_PARAM_BATONTOUCH;

/**
 * ひるませる処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8  pokeID;     ///< 対象ポケID
  u8  per;        ///< 確率
}BTL_HANDEX_PARAM_ADD_SHRINK;

/**
 * 生き返らせる処理
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;        ///< 対象ポケID
  u16  recoverHP;     ///< 回復HP量
  BTL_HANDEX_STR_PARAMS  exStr; ///< 成功時メッセージ
}BTL_HANDEX_PARAM_RELIVE;

/**
 * 体重を設定
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;        ///< 対象ポケID
  u16  weight;        ///< 体重
  BTL_HANDEX_STR_PARAMS  exStr; ///< 成功時メッセージ
}BTL_HANDEX_PARAM_SET_WEIGHT;

/**
 * ポケモンを場から吹き飛ばし
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u16  effectNo;                ///< 成功時エフェクトナンバー
  u8   pokeID;                  ///< 対象ポケID
  u8   fForceChange : 4;        ///< 強制入れ替えモードフラグ
  u8   fIgnoreLevel : 4;        ///< レベル差判定を無視するフラグ
  BTL_HANDEX_STR_PARAMS  exStr; ///< 成功時メッセージ
}BTL_HANDEX_PARAM_PUSHOUT;

/**
 * 指定ポケモン割り込み行動
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                  ///< 対象ポケID
  BTL_HANDEX_STR_PARAMS  exStr; ///< 成功時メッセージ
}BTL_HANDEX_PARAM_INTR_POKE;

/**
 * 指定ワザ使用ポケ割り込み行動
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  WazaID   waza;                ///< 対象ワザID
}BTL_HANDEX_PARAM_INTR_WAZA;

/**
 * 指定ポケモンの行動順を最後に回す
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                       ///< 対象ポケID
  BTL_HANDEX_STR_PARAMS    exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_SEND_LAST;

/**
 * 場に出ているポケモン同士を入れ替える（味方のみ）
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID1;                      ///< 対象ポケID 1
  u8   pokeID2;                      ///< 対象ポケID 2
  BTL_HANDEX_STR_PARAMS    exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_SWAP_POKE;

/**
 * へんしん（ヘッダ指定ポケモン）
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                       ///< 対象ポケID
  BTL_HANDEX_STR_PARAMS    exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_HENSIN;

/**
 * イリュージョン解除
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                       ///< 対象ポケID
  BTL_HANDEX_STR_PARAMS    exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_FAKE_BREAK;

/**
 * 溜めワザによる消え状態（そらをとぶ等）をキャンセルする
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8            pokeID;               ///< 対象ポケID
  BppContFlag   flag;                 ///< キャンセルする状態フラグ
  BTL_HANDEX_STR_PARAMS    exStr;    ///< 成功時メッセージ

}BTL_HANDEX_PARAM_TAMEHIDE_CANCEL;

/**
 * エフェクト呼び出し
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u16  effectNo;                      ///< エフェクトナンバー
  u8   pos_from;                      ///< エフェクト発動位置（不要ならBTL_POS_NULL：初期値(0)はBTL_POS_NULLではないので注意）
  u8   pos_to;                        ///< エフェクト終点位置（不要ならBTL_POS_NULL：初期値(0)はBTL_POS_NULLではないので注意）
  u16  reservedQuePos;                ///< Que予約領域
  u8   fQueReserve;                   ///< Que予約フラグ
  u8   fMsgWinVanish;                 ///< メッセージウィンドウ消去フラグ
  BTL_HANDEX_STR_PARAMS    exStr;    ///< エフェクト後メッセージ
}BTL_HANDEX_PARAM_ADD_EFFECT;

/**
 * フォルムナンバーチェンジ
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8   pokeID;                   ///< ポケモンID
  u8   formNo;                   ///< フォルムナンバー
  BTL_HANDEX_STR_PARAMS    exStr;    ///< 成功時メッセージ
}BTL_HANDEX_PARAM_CHANGE_FORM;

/**
 * ワザエフェクトIndexセット
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8     effIndex;
}BTL_HANDEX_PARAM_SET_EFFECT_IDX;

/**
 *  ワザエフェクトを強制的に有効にする
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
}BTL_HANDEX_PARAM_WAZAEFFECT_ENABLE;

/**
 *  ワザエフェクトを強制的に有効にする
 */
typedef struct {
  BTL_HANDEX_PARAM_HEADER  header;
  u8                       targetPokeID;
}BTL_HANDEX_PARAM_FREEFALL;



extern void* BTL_SVF_HANDEX_Push( BTL_SVFLOW_WORK* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID );
extern void BTL_SVF_HANDEX_PushRun( BTL_SVFLOW_WORK* wk, BtlEventHandlerExhibition eq_type, u8 userPokeID );
extern void BTL_SVF_HANDEX_Pop( BTL_SVFLOW_WORK* wk, void* handExWork );

extern u8 BTL_SVFTOOL_ExpandPokeID( BTL_SVFLOW_WORK* wk, BtlExPos exPos, u8* dst_pokeID );




#endif
