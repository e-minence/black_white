//=============================================================================================
/**
 * @file	hand_item.c
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g�t�@�N�^�[�i�A�C�e���j�ǉ�����
 * @author	taya
 *
 * @date	2009.04.21	�쐬
 */
//=============================================================================================
#include "poke_tool\poketype.h"
#include "poke_tool\monsno_def.h"
#include "waza_tool\wazano_def.h"
#include "item\itemsym.h"

#include "..\btl_common.h"
#include "..\btl_calc.h"
#include "..\btl_field.h"
#include "..\btl_client.h"
#include "..\btl_event_factor.h"

#include "hand_item.h"



/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void handler_KuraboNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KuraboNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_KagoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KagoNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_ChigoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ChigoNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_NanashiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NanashiNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_KiiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KiiNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_MomonNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MomonNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_RamNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RamNomi( u16 pri, u16 itemID, u8 pokeID );
static void common_sickReaction( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sickID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_OrenNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ObonNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_FiraNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_UiNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MagoNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_BanziNomi( u16 pri, u16 itemID, u8 pokeID );
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_IaNomi( u16 pri, u16 itemID, u8 pokeID );
static void handler_HPRecoverCommon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );




BTL_EVENT_FACTOR*  BTL_HANDLER_ITEM_Add( const BTL_POKEPARAM* pp )
{
	// �C�x���g�n���h���o�^�֐��̌^��`
	typedef BTL_EVENT_FACTOR* (*pEventAddFunc)( u16 pri, u16 itemID, u8 pokeID );

	static const struct {
		u32							itemID;
		pEventAddFunc		func;
	}funcTbl[] = {
		{ ITEM_KURABONOMI,	HAND_ADD_ITEM_KuraboNomi	},
		{ ITEM_KAGONOMI,		HAND_ADD_ITEM_KagoNomi		},
		{ ITEM_TIIGONOMI,		HAND_ADD_ITEM_ChigoNomi		},
		{ ITEM_NANASINOMI,	HAND_ADD_ITEM_NanashiNomi	},
		{ ITEM_RAMUNOMI,		HAND_ADD_ITEM_RamNomi			},
//		{ ITEM_HIMERINOMI,	HAND_ADD_ITEM_HimeriNomi	},
		{ ITEM_ORENNOMI,		HAND_ADD_ITEM_OrenNomi		},
		{ ITEM_OBONNOMI,		HAND_ADD_ITEM_ObonNomi		},
		{ ITEM_FIRANOMI,		HAND_ADD_ITEM_FiraNomi		},
		{ ITEM_UINOMI,			HAND_ADD_ITEM_UiNomi			},
		{ ITEM_MAGONOMI,		HAND_ADD_ITEM_MagoNomi		},
		{ ITEM_BANZINOMI,		HAND_ADD_ITEM_BanziNomi		},
		{ ITEM_IANOMI,			HAND_ADD_ITEM_IaNomi			 },

		{ 0, NULL },
	};

	u32 itemID = BTL_POKEPARAM_GetItem( pp );
	if( itemID != ITEM_DUMMY_DATA )
	{
		u32 i;
		for(i=0; i<NELEMS(funcTbl); ++i)
		{
			if( funcTbl[i].itemID == itemID )
			{
				u16 priority = BTL_POKEPARAM_GetValue_Base( pp, BPP_AGILITY );
				u8 pokeID = BTL_POKEPARAM_GetID( pp );
				return funcTbl[i].func( priority, itemID, pokeID );
			}
		}
	}

	return NULL;
}

//------------------------------------------------------------------------------
/**
 *	�N���{�̂�
 */
//------------------------------------------------------------------------------
static void handler_KuraboNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_sickReaction( flowWk, pokeID, WAZASICK_MAHI );
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KuraboNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK, handler_KuraboNomi },		// ��Ԉُ�`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�J�S�̂�
 */
//------------------------------------------------------------------------------
static void handler_KagoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_sickReaction( flowWk, pokeID, WAZASICK_NEMURI );
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KagoNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK, handler_KagoNomi },		// ��Ԉُ�`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�`�[�S�̂�
 */
//------------------------------------------------------------------------------
static void handler_ChigoNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_sickReaction( flowWk, pokeID, WAZASICK_YAKEDO );
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ChigoNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK, handler_ChigoNomi },		// ��Ԉُ�`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�i�i�V�̂�
 */
//------------------------------------------------------------------------------
static void handler_NanashiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_sickReaction( flowWk, pokeID, WAZASICK_KOORI );
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_NanashiNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK, handler_NanashiNomi },		// ��Ԉُ�`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�L�[�̂�
 */
//------------------------------------------------------------------------------
static void handler_KiiNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_sickReaction( flowWk, pokeID, WAZASICK_KONRAN );
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_KiiNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK, handler_KiiNomi },		// ��Ԉُ�`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�������̂�
 */
//------------------------------------------------------------------------------
static void handler_MomonNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	common_sickReaction( flowWk, pokeID, WAZASICK_DOKU );
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MomonNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK, handler_MomonNomi },		// ��Ԉُ�`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�����̂�
 */
//------------------------------------------------------------------------------
static void handler_RamNomi( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( pokeID == BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) )
	{
		WazaSick sickID = BTL_EVENTVAR_GetValue( BTL_EVAR_SICKID );
		if( (sickID < POKESICK_MAX) || (sickID == WAZASICK_KONRAN) )
		{
			BTL_EVWK_ADDSICK* evwk = (BTL_EVWK_ADDSICK*) BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
			evwk->fItemResponce = TRUE;
		}
	}
}
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_RamNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_MAKE_POKESICK, handler_RamNomi },		// ��Ԉُ�`�F�b�N�n���h��
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}

//--------------------------------------------------------------------------
/**
 * ��Ԉُ�ɔ�������A�C�e���Q�̋��ʏ���
 *
 * @param   flowWk		
 * @param   pokeID		
 * @param   sickID		���������Ԉُ�ID
 *
 */
//--------------------------------------------------------------------------
static void common_sickReaction( BTL_SVFLOW_WORK* flowWk, u8 pokeID, WazaSick sickID )
{
	if(	(BTL_EVENTVAR_GetValue(BTL_EVAR_SICKID) == sickID)
	&&	(BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID)
	){
		BTL_EVWK_ADDSICK* evwk = (BTL_EVWK_ADDSICK*) BTL_EVENTVAR_GetValue( BTL_EVAR_WORK_ADRS );
		evwk->fItemResponce = TRUE;
	}
}
//------------------------------------------------------------------------------
/**
 *	�I�����̂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_OrenNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,					handler_HPRecoverCommon },
		{ BTL_EVENT_SIMPLE_DAMAGE_REACTION,	handler_HPRecoverCommon },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�I�{���̂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_ObonNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,					handler_HPRecoverCommon },
		{ BTL_EVENT_SIMPLE_DAMAGE_REACTION,	handler_HPRecoverCommon },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�t�B���̂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_FiraNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,					handler_HPRecoverCommon },
		{ BTL_EVENT_SIMPLE_DAMAGE_REACTION,	handler_HPRecoverCommon },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�E�C�̂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_UiNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,					handler_HPRecoverCommon },
		{ BTL_EVENT_SIMPLE_DAMAGE_REACTION,	handler_HPRecoverCommon },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�}�S�̂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_MagoNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,					handler_HPRecoverCommon },
		{ BTL_EVENT_SIMPLE_DAMAGE_REACTION,	handler_HPRecoverCommon },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�o���W�̂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_BanziNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,					handler_HPRecoverCommon },
		{ BTL_EVENT_SIMPLE_DAMAGE_REACTION,	handler_HPRecoverCommon },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}
//------------------------------------------------------------------------------
/**
 *	�C�A�̂�
 */
//------------------------------------------------------------------------------
static BTL_EVENT_FACTOR* HAND_ADD_ITEM_IaNomi( u16 pri, u16 itemID, u8 pokeID )
{
	static const BtlEventHandlerTable HandlerTable[] = {
		{ BTL_EVENT_WAZA_DMG_AFTER,					handler_HPRecoverCommon },
		{ BTL_EVENT_SIMPLE_DAMAGE_REACTION,	handler_HPRecoverCommon },
		{ BTL_EVENT_NULL, NULL },
	};
	return BTL_EVENT_AddFactor( BTL_EVENT_FACTOR_TOKUSEI, itemID, pri, pokeID, HandlerTable );
}

static void handler_HPRecoverCommon( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work )
{
	if( BTL_EVENTVAR_GetValue(BTL_EVAR_POKEID_DEF) == pokeID )
	{
		const BTL_POKEPARAM* bpp = BTL_SVFLOW_RECEPT_GetPokeParam( flowWk, pokeID );
		if( BTL_POKEPARAM_GetHPBorder(bpp) >= BPP_HPBORDER_RED ){
			BTL_EVENTVAR_SetValue( BTL_EVAR_ITEMUSE_FLAG, TRUE );
		}
	}
}

