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
static void handler_decPP( BTL_SERVER* server, u8 pokeID, int* work );



//--------------------------------------------------------------
/**
 * �n���h���e�[�u��
*/
//--------------------------------------------------------------
static const BtlEventHandlerTable HandlerTable[] = {
	{ BTL_EVENT_DECREMENT_PP, handler_decPP },
	{ BTL_EVENT_NULL, NULL },
};


BTL_EVENT_FACTOR*  HAND_TOK_ADD_Pressure( u16 pri, u8 pokeID )
{
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

// BTL_EVENT_DECREMENT_PP:���U���g�����|�P������PP�f�N�������g�v�Z
static void handler_decPP( BTL_SERVER* server, u8 pokeID, int* work )
{
	// work[0] �𔭓��t���O�Ƃ��Ďg�p
	if( work[0] == 0 )
	{
		BtlPokePos myPos = BTL_SERVER_CheckExistFrontPokeID( server, pokeID );
//	BTL_SERVER_RECEPT_SetDecrementPP( server, 2 );
		BTL_SERVER_RECEPT_TokuseiWinIn( server, myPos );
		BTL_SERVER_RECEPT_RankDownEffect( server, EXPOS_MAKE(BTL_EXPOS_ENEMY_ALL, myPos), BPP_ATTACK, 1 );
		BTL_SERVER_RECEPT_TokuseiWinOut( server, myPos );

		work[0] = 1;
	}
}

