//=============================================================================================
/**
 * @file	hand_tok_fukutsuno.c
 * @brief	ポケモンWB バトルシステム	イベントファクター[とくせい]：『ふくつのこころ』
 * @author	taya
 *
 * @date	2009.03.11	作成
 */
//=============================================================================================
#include "hand_tokusei_common.h"

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_WazaExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );


//--------------------------------------------------------------
/**
 * ハンドラテーブル
*/
//--------------------------------------------------------------
static const BtlEventHandlerTable HandlerTable[] = {
	{ BTL_EVENT_WAZA_EXECUTE_FIX, handler_WazaExeFix },
	{ BTL_EVENT_NULL, NULL },
};
//BTL_EVENT_FACTOR_Remove

BTL_EVENT_FACTOR*  HAND_TOK_ADD_Fukutsuno( u16 pri, u16 tokID, u8 pokeID )
{
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}


// BTL_EVENT_WAZA_EXECUTE_FIX:ワザ出し成功判定確定
static void handler_WazaExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// ワザ失敗理由=ひるみ, 対象ポケ=自分で発動
	if( (BTL_EVENTVAR_GetValue(BTL_EVAR_FAIL_REASON) == SV_WAZAFAIL_SHRINK)
	&&	(BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
	){
		BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
		BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, pokeID );
		BTL_SERVER_RECEPT_RankUpEffect( flowWk, myPos, BPP_AGILITY, 1 );
		BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, pokeID );
		BTL_Printf("ポケ[%d] ひるまされたので ふくつのこころ 発生\n", pokeID);
	}
}

