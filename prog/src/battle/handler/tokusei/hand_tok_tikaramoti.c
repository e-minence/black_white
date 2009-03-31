//=============================================================================================
/**
 * @file	hand_tok_tikaramoti.c
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g�t�@�N�^�[[�Ƃ�����]�F�w��������������K�p���[�x
 * @author	taya
 *
 * @date	2009.03.12	�쐬
 */
//=============================================================================================
#include "hand_tokusei_common.h"

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );


//--------------------------------------------------------------
/**
 * �n���h���e�[�u��
*/
//--------------------------------------------------------------
static const BtlEventHandlerTable HandlerTable[] = {
	{ BTL_EVENT_ATTACKER_POWER, handler_AtkPower },
	{ BTL_EVENT_NULL, NULL },
};
//BTL_EVENT_FACTOR_Remove

BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tikaramoti( u16 pri, u16 tokID, u8 pokeID )
{
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}


// BTL_EVENT_ATTACKER_POWER: �U�����̍U���l����
static void handler_AtkPower( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U������̂�����
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID)
	{
		WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
		// �_���[�W�^�C�v������
		if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
		{
				u32 power = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
				power *= 2;
				BTL_EVENTVAR_SetValue( BTL_EVAR_POWER, power );
				BTL_Printf("pokeID:%d �Ƃ�����[���������or���K�p���[]�ɂ��A�U���З͔{��\n", pokeID);
		}
	}
}



