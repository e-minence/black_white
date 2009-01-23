//=============================================================================================
/**
 * @file	hand_tok_clearbody.c
 * @brief	�|�P����WB �o�g���V�X�e��  �C�x���g�t�@�N�^�[[�Ƃ�����]�F�w�N���A�{�f�B�����낢���ނ�x
 * @author	taya
 *
 * @date	2008.11.11	�쐬
 */
//=============================================================================================

#include "../../btl_string.h"
#include "hand_tokusei_common.h"

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_BeforeRankDown( BTL_SERVER* server, u8 pokeID, int* work );



//--------------------------------------------------------------
/**
 * �n���h���e�[�u��
*/
//--------------------------------------------------------------
static const BtlEventHandlerTable HandlerTable[] = {
	{ BTL_EVENT_BEFORE_RANKDOWN, handler_BeforeRankDown },
	{ BTL_EVENT_NULL, NULL },
};


BTL_EVENT_FACTOR*  HAND_TOK_ADD_ClearBody( u16 pri, u8 pokeID )
{
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}


static void handler_BeforeRankDown( BTL_SERVER* server, u8 pokeID, int* work )
{
	if( BTL_SERVER_RECEPT_GetEventArg(server, BTL_EVARG_COMMON_POKEID) == pokeID )
	{
		u8 myPos = BTL_SERVER_RECEPT_GetEventArg(server, BTL_EVARG_COMMON_POKEPOS);

		BTL_Printf("�N���A�{�f�B�������N�_�E���O�ɌĂ΂ꂽ�� pos=%d\n", myPos);

		BTL_SERVER_RECEPT_SetEventArg( server, BTL_EVARG_RANKDOWN_FAIL_FLAG, TRUE );
		BTL_SERVER_RECEPT_TokuseiWinIn( server, myPos );
		BTL_SERVER_RECTPT_SetMessage( server, BTL_STRID_SET_RankdownFail, myPos );
		BTL_SERVER_RECEPT_TokuseiWinOut( server, myPos );
	}
}

