//=============================================================================================
/**
 * @file	hand_tok_fukutsuno.c
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g�t�@�N�^�[[�Ƃ�����]�F�w�ӂ��̂�����x
 * @author	taya
 *
 * @date	2009.03.11	�쐬
 */
//=============================================================================================
#include "hand_tokusei_common.h"

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_WazaExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );


//--------------------------------------------------------------
/**
 * �n���h���e�[�u��
*/
//--------------------------------------------------------------
static const BtlEventHandlerTable HandlerTable[] = {
	{ BTL_EVENT_WAZA_EXECUTE_FIX, handler_WazaExeFix },
	{ BTL_EVENT_NULL, NULL },
};
//BTL_EVENT_RemoveFactor

BTL_EVENT_FACTOR*  HAND_TOK_ADD_Fukutsuno( u16 pri, u8 pokeID )
{
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}


// BTL_EVENT_WAZA_EXECUTE_FIX:���U�o����������m��
static void handler_WazaExeFix( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// ���U���s���R=�Ђ��, �Ώۃ|�P=�����Ŕ���
	if( (BTL_EVENTVAR_GetValue(BTL_EVAR_FAIL_REASON) == SV_WAZAFAIL_SHRINK)
	&&	(BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
	){
		BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );
		BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, myPos );
		BTL_SERVER_RECEPT_RankUpEffect( flowWk, myPos, BPP_AGILITY, 1 );
		BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, myPos );
		BTL_Printf("�|�P[%d] �Ђ�܂��ꂽ�̂� �ӂ��̂����� ����\n", pokeID);
	}
}

