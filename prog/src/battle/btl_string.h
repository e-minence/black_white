//=============================================================================================
/**
 * @file	btl_string.h
 * @brief	�|�P����WB �o�g�� �����񐶐�����
 * @author	taya
 *
 * @date	2008.10.06	�쐬
 */
//=============================================================================================
#ifndef __BTL_STRING_H__
#define __BTL_STRING_H__

//--------------------------------------------------------------
/**
 *	�ΏۃN���C�A���g���l�������ɐ����ł��镶����ID�Q
 */
//--------------------------------------------------------------
typedef enum {

	BTL_STRFMT_ENCOUNT,
	BTL_STRFMT_SELECT_ACTION_READY,

	BTL_STRID_GEN_MAX,

}BtlGenStrID;

//--------------------------------------------------------------
/**
 *	�ΏۃN���C�A���g�ɂ�萶���������e���قȂ镶����ID�Q
 */
//--------------------------------------------------------------
typedef enum {
	BTL_STRID_ESCAPE,			///< �y���b�Z�[�W�z �����o���� [ClientID]
	BTL_STRID_CONF_ANNOUNCE,	///< �y���b�Z�[�W�z �����񂵂Ă��� [ClientID]
	BTL_STRID_CONF_ON,			///< �y���b�Z�[�W�z �킯���킩�炸 [ClientID]
	BTL_STRID_DEAD,				///< �y���b�Z�[�W�z �����ꂽ [ClientID]
	BTL_STRID_WAZA_FAIL,		///< �y���b�Z�[�W�z ���U���o���Ȃ�����[ ClientID, Reason ]
	BTL_STRID_WAZA_AVOID,		///< �y���b�Z�[�W�z ���ǁ@�͂��ꂽ�I[ ClientID ]
	BTL_STRID_WAZA_HITCOUNT,	///< �y���b�Z�[�W�z �����񂠂������I[ hitCount ]
	BTL_STRID_WAZA_ANNOUNCE,	///<  �����́~�~���������I
	BTL_STRID_RANKDOWN,			///< �����́~�~�����������I[ ClientID, StatusType ]
	BTL_STRID_RANKUP,			///< �����́~�~�����������I[ ClientID, StatusType ]
	BTL_STRID_RANKDOWN_FAIL,	///< �����͂̂���傭��������Ȃ��I [ClientID]

}BtlSpStrID;

#include "btl_main.h"
#include "btl_client.h"


extern void BTL_STR_InitSystem( const BTL_MAIN_MODULE* mainModule, const BTL_CLIENT* client, HEAPID heapID );
extern void BTL_STR_MakeStringGeneric( STRBUF* buf, BtlGenStrID strID );
extern void BTL_STR_MakeStringSpecific( STRBUF* buf, BtlSpStrID strID, const int* args );


#endif

