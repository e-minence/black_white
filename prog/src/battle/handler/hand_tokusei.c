//=============================================================================================
/**
 * @file	hand_tokusei.c
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g�t�@�N�^�[[�Ƃ�����]�ǉ�����
 * @author	taya
 *
 * @date	2008.11.11	�쐬
 */
//=============================================================================================

#include "poke_tool/poketype.h"

#include "../btl_common.h"
#include "../btl_calc.h"
#include "../btl_event_factor.h"

#include "hand_tokusei.h"

#include "tokusei/hand_tokusei_common.h"


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_Iromagane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );





BTL_EVENT_FACTOR*  BTL_HANDLER_TOKUSEI_Add( const BTL_POKEPARAM* pp )
{
	static const struct {
		PokeTokusei				tokusei;
		pTokuseiEventAddFunc	func;
	}funcTbl[] = {
		{ POKETOKUSEI_IKAKU,					HAND_TOK_ADD_Ikaku },
		{ POKETOKUSEI_KURIABODY,			HAND_TOK_ADD_ClearBody },
		{ POKETOKUSEI_SIROIKEMURI,		HAND_TOK_ADD_ClearBody },	// ���낢���ނ�=�N���A�{�f�B�Ɠ���
		{ POKETOKUSEI_SEISINRYOKU,		HAND_TOK_ADD_Seisinryoku },
		{ POKETOKUSEI_FUKUTUNOKOKORO,	HAND_TOK_ADD_Fukutsuno },
		{ POKETOKUSEI_ATUISIBOU,			HAND_TOK_ADD_AtuiSibou },
		{ POKETOKUSEI_KAIRIKIBASAMI,	HAND_TOK_ADD_KairikiBasami },
		{ POKETOKUSEI_TIKARAMOTI,			HAND_TOK_ADD_Tikaramoti },
		{ POKETOKUSEI_YOGAPAWAA,			HAND_TOK_ADD_Tikaramoti },	// ���K�p���[ = ����������Ɠ���
		{ POKETOKUSEI_IROMEGANE,			HAND_TOK_ADD_Iromegane },
		{ POKETOKUSEI_KASOKU,					HAND_TOK_ADD_Kasoku },

	};

	int i;
	u16 tokusei = BTL_POKEPARAM_GetValue( pp, BPP_TOKUSEI );

	for(i=0; i<NELEMS(funcTbl); i++)
	{
		if( funcTbl[i].tokusei == tokusei )
		{
			u16 agi = BTL_POKEPARAM_GetValue( pp, BPP_AGILITY );
			u8 pokeID = BTL_POKEPARAM_GetID( pp );

			return funcTbl[i].func( agi, pokeID );
		}
	}

	BTL_Printf("�Ƃ�����(%d)�̓n���h�����p�ӂ���Ă��Ȃ�\n", tokusei);
	return NULL;

//	BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, agi, pokeID, 
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u����߂��ˁv
 */
//------------------------------------------------------------------------------
// �_���[�W�v�Z�ŏI�i�K�̃n���h��
static void handler_Iromagane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U������������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �����C�}�C�`�̎�
		BtlTypeAffAbout aff = BTL_CALC_TypeAffAbout( BTL_EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) );
		if( aff == BTL_TYPEAFF_ABOUT_DISADVANTAGE )
		{
			// �_���[�W�Q�{
			u32 dmg = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
			dmg *= 2;
			BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, dmg );
			BTL_Printf("�|�P[%d]�� ����߂��� �Ń_���[�W�Q�{\n", pokeID);
		}
	}
}

BTL_EVENT_FACTOR*  HAND_TOK_ADD_Iromegane( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_PROC2, handler_Iromagane },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�������v
 */
//------------------------------------------------------------------------------
// �^�[���`�F�b�N�̃n���h��
static void handler_Kasoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	BtlPokePos myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );

	BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, myPos );
	BTL_SERVER_RECEPT_RankUpEffect( flowWk, myPos, BPP_AGILITY, 1 );
	BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, myPos );
}

BTL_EVENT_FACTOR*  HAND_TOK_ADD_Kasoku( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_TURNCHECK, handler_Kasoku },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
