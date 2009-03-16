//=============================================================================================
/**
 * @file	btlv_core.h
 * @brief	ポケモンWB バトル描画担当メインモジュール
 * @author	taya
 *
 * @date	2008.10.02	作成
 */
//=============================================================================================
#ifndef __BTLV_CORE_H__
#define __BTLV_CORE_H__


//----------------------------------------------------------------------
/**
 *	描画メインモジュールハンドラ型定義
 */
//----------------------------------------------------------------------
typedef  struct _BTLV_CORE		BTLV_CORE;


//----------------------------------------------------------------------
// 参照ヘッダ include 
//----------------------------------------------------------------------
#include "battle/btl_main.h"
#include "battle/btl_client.h"
#include "battle/btl_action.h"
#include "battle/btl_calc.h"
#include "battle/btl_string.h"
#include "battle/btl_pokeselect.h"
#include "btlv_common.h"

//----------------------------------------------------------------------
/**
 *	描画コマンド
 */
//----------------------------------------------------------------------
typedef enum {

	BTLV_CMD_SETUP,
	BTLV_CMD_SELECT_ACTION,
	BTLV_CMD_SELECT_POKEMON,
	BTLV_CMD_MSG,

	BTLV_CMD_MAX,
	BTLV_CMD_NULL = BTLV_CMD_MAX,

}BtlvCmd;



//=============================================================================================
/**
 * 描画メインモジュールの生成
 *
 * @param   mainModule		システムメインモジュールのハンドラ
 * @param   heapID			生成先ヒープID
 *
 * @retval  BTLV_CORE*		生成された描画メインモジュールのハンドラ
 */
//=============================================================================================
extern BTLV_CORE*  BTLV_Create( BTL_MAIN_MODULE* mainModule, const BTL_CLIENT* client, const BTL_POKE_CONTAINER* pokeCon, HEAPID heapID );

//=============================================================================================
/**
 * 描画メインモジュールの破棄
 *
 * @param   core		描画メインモジュールのハンドラ
 */
//=============================================================================================
extern void BTLV_Delete( BTLV_CORE* core );

//=============================================================================================
/**
 * 描画メインループ
 *
 * @param   core		描画メインモジュールのハンドラ
 */
//=============================================================================================
extern void BTLV_CORE_Main( BTLV_CORE* core );

//=============================================================================================
/**
 * BTLV_StartCommand で受け付けた描画コマンドの終了を待つ
 *
 * @param   wk			描画メインモジュールのハンドラ
 *
 * @retval  BOOL		終了していたらTRUE
 */
//=============================================================================================
extern void BTLV_StartCommand( BTLV_CORE* btlv, BtlvCmd cmd );

//=============================================================================================
/**
 * BTLV_StartCommand で受け付けた描画コマンドの終了を待つ
 *
 * @param   wk			描画メインモジュールのハンドラ
 *
 * @retval  BOOL		終了していたらTRUE
 */
//=============================================================================================
extern BOOL BTLV_WaitCommand( BTLV_CORE* btlv );



extern void BTLV_UI_SelectAction_Start( BTLV_CORE* core, BTL_ACTION_PARAM* dest );
extern BOOL BTLV_UI_SelectAction_Wait( BTLV_CORE* core );

extern void BTLV_StartMemberChangeAct( BTLV_CORE* wk, BtlPokePos pos, u8 clientID, u8 memberIdx );
extern BOOL BTLV_WaitMemberChangeAct( BTLV_CORE* wk );


//=============================================================================================
/**
 * ポケモン選択処理を開始
 *
 * @param   core			[in]  描画メインモジュールハンドラ
 * @param   param			[in]  選択処理パラメータポインタ
 * @param   result		[out] 選択結果格納構造体ポインタ
 */
//=============================================================================================
extern void BTLV_StartPokeSelect( BTLV_CORE* core, const BTL_POKESELECT_PARAM* param, BTL_POKESELECT_RESULT* result );

//=============================================================================================
/**
 * ポケモン選択処理の終了待ち
 *
 * @param   core			[in]  描画メインモジュールハンドラ
 *
 * @retval  BOOL			終了したらTRUE
 */
//=============================================================================================
extern BOOL BTLV_WaitPokeSelect( BTLV_CORE* core );



//=============================================================================================
/**
 * 
 *
 * @param   wk		
 * @param   strID		
 * @param   clientID		
 *
 * @retval  extern void		
 */
//=============================================================================================
extern void BTLV_StartMsgStd( BTLV_CORE* wk, u16 strID, const int* args );
extern void BTLV_StartMsgSet( BTLV_CORE* wk, u16 strID, const int* args );
extern void BTLV_StartMsgWaza( BTLV_CORE* wk, u8 pokeID, u16 waza );
extern BOOL BTLV_WaitMsg( BTLV_CORE* wk );

extern void BTLV_ACT_WazaEffect_Start( BTLV_CORE* wk, BtlPokePos atPokePos, BtlPokePos defPokePos, WazaID waza );
extern BOOL BTLV_ACT_WazaEffect_Wait( BTLV_CORE* wk );

extern void BTLV_ACT_DamageEffectSingle_Start( BTLV_CORE* wk, BtlPokePos defPokePos, u16 damage, BtlTypeAff aff );
extern BOOL BTLV_ACT_DamageEffectSingle_Wait( BTLV_CORE* wk );

extern void BTLV_ACT_DamageEffectDouble_Start( BTLV_CORE* wk, BtlPokePos defPokePos1, BtlPokePos defPokePos2,
		u16 damage1, u16 damage2, BtlTypeAff aff );
extern BOOL BTLV_ACT_DamageEffectDouble_Wait( BTLV_CORE* wk );

extern void BTLV_StartDeadAct( BTLV_CORE* wk, BtlPokePos pokePos );
extern BOOL BTLV_WaitDeadAct( BTLV_CORE* wk );
extern void BTLV_ACT_MemberOut_Start( BTLV_CORE* wk, u8 clientID, u8 memberIdx );
extern BOOL BTLV_ACT_MemberOut_Wait( BTLV_CORE* wk );
extern void BTLV_ACT_SimpleHPEffect_Start( BTLV_CORE* wk, BtlPokePos pokePos );
extern BOOL BTLV_ACT_SimpleHPEffect_Wait( BTLV_CORE* wk );


extern void BTLV_StartTokWin( BTLV_CORE* wk, BtlPokePos clientID );
extern void BTLV_QuitTokWin( BTLV_CORE* wk, BtlPokePos clientID );
extern void BTLV_StartRankDownEffect( BTLV_CORE* wk, u8 pokeID, BppValueID statusType );
extern void BTLV_StartCommWait( BTLV_CORE* wk );
extern BOOL BTLV_WaitCommWait( BTLV_CORE* wk );
extern void BTLV_ResetCommWaitInfo( BTLV_CORE* wk );


//-----------------------------------------------------------
// 各種下請けから呼び出される関数

extern u8 BTLV_CORE_GetPlayerClientID( const BTLV_CORE* core );

#endif
