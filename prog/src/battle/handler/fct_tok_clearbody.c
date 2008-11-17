//=============================================================================================
/**
 * @file	fct_tok_clearbody.c
 * @brief	ポケモンWB バトルシステム  イベントファクター[とくせい]：『クリアボディ＆しろいけむり』
 * @author	taya
 *
 * @date	2008.11.11	作成
 */
//=============================================================================================

#include "../btl_string.h"
#include "fct_tokusei_common.h"

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_BeforeRankDown( BTL_SERVER* server, u8 pokeID, int* work );



//--------------------------------------------------------------
/**
 * ハンドラテーブル
*/
//--------------------------------------------------------------
static const BtlEventHandlerTable HandlerTable[] = {
	{ BTL_EVENT_BEFORE_RANKDOWN, handler_BeforeRankDown },
	{ BTL_EVENT_NULL, NULL },
};


BTL_EVENT_FACTOR*  FCT_TOK_ADD_ClearBody( u16 pri, u8 pokeID )
{
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}


static void handler_BeforeRankDown( BTL_SERVER* server, u8 pokeID, int* work )
{
	u8 clientID = BTL_SERVER_RECEPT_PokeIDtoClientID( server, pokeID );

	if( BTL_SERVER_RECEPT_GetEventArg(server, BTL_EVARG_COMMON_CLIENT_ID) == clientID )
	{
		BTL_SERVER_RECEPT_SetEventArg( server, BTL_EVARG_RANKDOWN_FAIL_FLAG, TRUE );
		BTL_SERVER_RECEPT_TokuseiWinIn( server, clientID );
		BTL_SERVER_RECTPT_SetMessage( server, BTL_STRID_RANKDOWN_FAIL, clientID );
		BTL_SERVER_RECEPT_TokuseiWinOut( server, clientID );
	}
}

