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
#include "poke_tool/monsno_def.h"

#include "../btl_common.h"
#include "../btl_calc.h"
#include "../btl_event_factor.h"

#include "hand_tokusei.h"

#include "tokusei/hand_tokusei_common.h"


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_Iromagane( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_HardRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Kasoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void common_hpborder_powerup( BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType wazaType );
static void handler_Mouka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Gekiryu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Sinryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_MusinoSirase( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Konjou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_FusiginaUroko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_SkillLink( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Surudoime( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static void handler_Tanjun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );





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
		{ POKETOKUSEI_MOUKA,					HAND_TOK_ADD_Mouka },
		{ POKETOKUSEI_GEKIRYUU,				HAND_TOK_ADD_Gekiryu },
		{ POKETOKUSEI_SINRYOKU,				HAND_TOK_ADD_Sinryoku },
		{ POKETOKUSEI_MUSINOSIRASE,		HAND_TOK_ADD_MusinoSirase },
		{ POKETOKUSEI_KONJOU,					HAND_TOK_ADD_Konjou },
		{ POKETOKUSEI_SUKIRURINKU,		HAND_TOK_ADD_SkillLink },
		{ POKETOKUSEI_SURUDOIME,			HAND_TOK_ADD_Surudoime },
		{ POKETOKUSEI_TANJUN,					HAND_TOK_ADD_Tanjun },
		{ POKETOKUSEI_HAADOROKKU,			HAND_TOK_ADD_HardRock },
		{ POKETOKUSEI_FUSIGINAUROKO,	HAND_TOK_ADD_FusiginaUroko },
		{ POKETOKUSEI_TOUSOUSIN,			HAND_TOK_ADD_Tousousin },
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
 *	�Ƃ������u�n�[�h���b�N�v
 */
//------------------------------------------------------------------------------
// �_���[�W�v�Z�ŏI�i�K�̃n���h��
static void handler_HardRock( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �h�䑤��������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// �������o�c�O���̎�
		BtlTypeAffAbout aff = BTL_CALC_TypeAffAbout( BTL_EVENTVAR_GetValue(BTL_EVAR_TYPEAFF) );
		if( aff == BTL_TYPEAFF_ABOUT_ADVANTAGE )
		{
			// �_���[�W75��
			u32 dmg = BTL_EVENTVAR_GetValue( BTL_EVAR_DAMAGE );
			dmg = (dmg*75) / 100;
			BTL_EVENTVAR_SetValue( BTL_EVAR_DAMAGE, dmg );
			BTL_Printf("�|�P[%d]�� �n�[�h���b�N �Ń_���[�W75��\n", pokeID);
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_HardRock( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_PROC2, handler_HardRock },
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


//--------------------------------------------------------------------------
/**
 * HP��1/3�̎��ɁA����^�C�v�̃��U�З͂��グ��Ƃ������̋��ʏ���
 *
 * @param   flowWk		
 * @param   pokeID		
 * @param   wazaType		
 *
 */
//--------------------------------------------------------------------------
static void common_hpborder_powerup( BTL_SVFLOW_WORK* flowWk, u8 pokeID, PokeType wazaType )
{
	// �U������������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// HP 1/3 �ȉ���
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( BTL_POKEPARAM_GetHPBorder(bpp) <= BPP_HPBORDER_YELLOW )
		{
			// �g���̂��w��^�C�v���U�Ȃ�
			if( WAZADATA_GetType( BTL_EVENTVAR_GetValue(BTL_EVAR_WAZAID) ) == wazaType )
			{
				// �З͂Q�{
				u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
				pow *= 2;
				BTL_EVENTVAR_SetValue( BTL_EVAR_POWER, pow );
			}
		}
	}
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�������v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Mouka( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_hpborder_powerup( flowWk, pokeID, POKETYPE_FIRE );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Mouka( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Mouka },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u������イ�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Gekiryu( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_hpborder_powerup( flowWk, pokeID, POKETYPE_WATER );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Gekiryu( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Gekiryu },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�����傭�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Sinryoku( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_hpborder_powerup( flowWk, pokeID, POKETYPE_KUSA );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Sinryoku( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Sinryoku },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ނ��̂��点�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_MusinoSirase( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_hpborder_powerup( flowWk, pokeID, POKETYPE_MUSHI );
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_MusinoSirase( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_MusinoSirase },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���񂶂傤�v
 */
//------------------------------------------------------------------------------
// �U���З͌���̃n���h��
static void handler_Konjou( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U������������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// ��Ԉُ��
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( BTL_POKEPARAM_GetPokeSick(bpp) != POKESICK_NULL )
		{
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			// �_���[�W�^�C�v�������̎�
			if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_PHYSIC )
			{
				// �З͂Q�{
				u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_POWER );
				pow *= 2;
				BTL_EVENTVAR_SetValue( BTL_EVAR_POWER, pow );
				BTL_Printf("�|�P[%d]�� ���񂶂傤 �ňЗ͂Q�{\n", pokeID);
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Konjou( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_ATTACKER_POWER, handler_Konjou },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�Ƃ���������v
 */
//------------------------------------------------------------------------------
// ���U�З͌���̃n���h��
static void handler_Tousousin( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U����������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		const BTL_POKEPARAM* myParam = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		const BTL_POKEPARAM* targetParam = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk,
					BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) );

		u8 mySex = BTL_POKEPARAM_GetValue( myParam, BPP_SEX );
		u8 targetSex = BTL_POKEPARAM_GetValue( targetParam, BPP_SEX );

		// �݂��ɐ��ʕs������Ȃ��ꍇ
		if( (mySex!=PTL_SEX_UNKNOWN) && (targetSex!=PTL_SEX_UNKNOWN) )
		{
			u32 pow = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZA_POWER );
			if( mySex == targetSex ){
				pow = (pow * FX32_CONST(1.25f)) >> FX32_SHIFT;
			}else{
				pow = (pow * FX32_CONST(0.75f)) >> FX32_SHIFT;
			}
			BTL_EVENTVAR_SetValue( BTL_EVAR_WAZA_POWER, pow );
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tousousin( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_POWER, handler_Tousousin },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�ӂ����Ȃ��낱�v
 */
//------------------------------------------------------------------------------
///< �h��\�͌���̃n���h��
static void handler_FusiginaUroko( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U������������
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		// �~���J���X�ŏ�Ԉُ��
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( (BTL_POKEPARAM_GetPokeSick(bpp) != POKESICK_NULL)
		&&	(BTL_POKEPARAM_GetMonsNo(bpp) == MONSNO_MIROKAROSU)
		){
			WazaID waza = BTL_EVENTVAR_GetValue( BTL_EVAR_WAZAID );
			// �_���[�W�^�C�v������̎�
			if( WAZADATA_GetDamageType(waza) == WAZADATA_DMG_SPECIAL )
			{
				// �h��Q�{
				u32 guard = BTL_EVENTVAR_GetValue( BTL_EVAR_GUARD );
				guard *= 2;
				BTL_EVENTVAR_SetValue( BTL_EVAR_GUARD, guard );
				BTL_Printf("�|�P[%d]�� �ӂ����Ȃ܂��� �Ŗh��Q�{\n", pokeID);
			}
		}
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_FusiginaUroko( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_DEFENDER_GUARD, handler_FusiginaUroko },	///< �h��\�͌���̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u�X�L�������N�v
 */
//------------------------------------------------------------------------------
// �U���񐔌���̃n���h��
static void handler_SkillLink( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	// �U�����������̎�
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_ATK) == pokeID )
	{
		// �q�b�g�񐔂��ő��
		u16 max = BTL_EVENTVAR_GetValue( BTL_EVAR_HITCOUNT_MAX );
		BTL_EVENTVAR_SetValue( BTL_EVAR_HITCOUNT, max );
		BTL_Printf("POKE[%d]�� �X�L�������N �ŁA�q�b�g�񐔂��ő��%d�ɂ���\n", max);
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_SkillLink( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_HIT_COUNT, handler_SkillLink },	// �U���񐔌���̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}

//------------------------------------------------------------------------------
/**
 *	�Ƃ������u����ǂ��߁v
 */
//------------------------------------------------------------------------------
// �����N�_�E�����O�����̃n���h��
static void handler_Surudoime( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( (BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID)
	&&	(BTL_EVENTVAR_GetValue(BTL_EVAR_STATUS_TYPE) ==  WAZA_RANKEFF_HIT)
	){
		u8 myPos = BTL_SVFLOW_CheckExistFrontPokeID( flowWk, pokeID );

		BTL_EVENTVAR_SetValue( BTL_EVAR_FAIL_FLAG, TRUE );
		BTL_SERVER_RECEPT_TokuseiWinIn( flowWk, myPos );
		BTL_SERVER_RECTPT_SetMessage( flowWk, BTL_STRID_SET_RankdownFail_HIT, pokeID );
		BTL_SERVER_RECEPT_TokuseiWinOut( flowWk, myPos );
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Surudoime( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_BEFORE_RANKDOWN, handler_Surudoime },	// �����N�_�E�����O�����̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�Ƃ������u���񂶂��v
 */
//------------------------------------------------------------------------------
// �����N�_�E�����O�������N�A�b�v���O�̃n���h��
static void handler_Tanjun( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID) == pokeID )
	{
		u8 volume = BTL_EVENTVAR_GetValue( BTL_EVAR_VOLUME );
		volume *= 2;
		BTL_EVENTVAR_SetValue( BTL_EVAR_VOLUME, volume );
		BTL_Printf("�|�P[%d]�� ���񂶂�� �ɂ����ʔ{��\n");
	}
}
BTL_EVENT_FACTOR*  HAND_TOK_ADD_Tanjun( u16 pri, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_BEFORE_RANKDOWN, handler_Tanjun },	// �����N�_�E�����O�����̃n���h��
		{ BTL_EVENT_BEFORE_RANKUP, handler_Tanjun },		// �����N�A�b�v���O�����̃n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, pri, pokeID, HandlerTable );
}



