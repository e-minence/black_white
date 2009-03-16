//=============================================================================================
/**
 * @file	hand_tok_seisinryoku.c
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g�t�@�N�^�[[�Ƃ�����]�F�w���������傭�x
 * @author	taya
 *
 * @date	2009.03.11	�쐬
 */
//=============================================================================================
#include "hand_tokusei_common.h"

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_ShrinkCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWork, u8 pokeID, int* work );


//--------------------------------------------------------------
/**
 * �n���h���e�[�u��
*/
//--------------------------------------------------------------
static const BtlEventHandlerTable HandlerTable[] = {
	{ BTL_EVENT_SHRINK_CHECK, handler_ShrinkCheck },
	{ BTL_EVENT_NULL, NULL },
};
//BTL_EVENT_RemoveFactor

BTL_EVENT_FACTOR*  HAND_TOK_ADD_Seisinryoku( u16 pri, u8 pokeID )
{
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}


// BTL_EVENT_SHRINK_CHECK:�Ђ�ݔ����`�F�b�N
static void handler_ShrinkCheck( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		// �����m����0�ɏ�������
		BTL_EVENTVAR_SetValue( BTL_EVAR_ADD_PER, 0 );
	}
}

