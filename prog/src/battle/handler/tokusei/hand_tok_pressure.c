//=============================================================================================
/**
 * @file	hand_tok_pressure.c
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g�t�@�N�^�[[�Ƃ�����]�F�w�v���b�V���[�x
 * @author	taya
 *
 * @date	2009.1.26	�쐬
 */
//=============================================================================================

#include "hand_tokusei_common.h

/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_decPP( BTL_EVENT_FACTOR* myHandle, BTL_SERVER* server, u8 pokeID, int* work );



//--------------------------------------------------------------
/**
 * �n���h���e�[�u��
*/
//--------------------------------------------------------------
static const BtlEventHandlerTable HandlerTable[] = {
	{ BTL_EVENT_DECREMENT_PP, handler_decPP },
	{ BTL_EVENT_NULL, NULL },
};


BTL_EVENT_FACTOR*  HAND_TOK_ADD_Pressure( u16 pri, u16 tokID, u8 pokeID )
{
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, tokID, pri, pokeID, HandlerTable );
}

// BTL_EVENT_DECREMENT_PP:���U���g�����|�P������PP�f�N�������g�v�Z
static void handler_decPP( BTL_EVENT_FACTOR* myHandle, BTL_SERVER* server, u8 pokeID, int* work )
{
	u8 pokeCnt = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGED_POKECNT );
	u8 i;
	for(i=0; i<pokeCnt; ++i)
	{
		if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DAMAGED1+i) == pokeID )
		{
			u16 volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
			BTL_EVENTVAR_RewriteValue( BTL_EVAR_VOLUME, ++volume );
			BTL_Printf("�_���[�W�󂯂�%d�̖ڂ������iID=%d�j�Ȃ̂ŁA�v���b�V���[��PP�𑽂����炵�܂�\n", i, pokeID);
		}
	}
}

