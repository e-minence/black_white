//=============================================================================================
/**
 * @file	btl_event_factor.h
 * @brief	�|�P����WB �o�g���V�X�e��	�C�x���g���n���h�������iFACTOR���ɂ̂݌��J�j
 * @author	taya
 *
 * @date	2008.10.23	�쐬
 */
//=============================================================================================
#ifndef __BTL_EVENT_FACTOR_H__
#define __BTL_EVENT_FACTOR_H__

#include "btl_event.h"
#include "btl_server.h"
#include "btl_pokeparam.h"


enum {
	EVENT_HANDLER_WORK_ELEMS = 8,
};

//--------------------------------------------------------------
/**
*	�C�x���g�n���h���^
*
*	sv 		�T�[�o���W���[���|�C���^
*	pokeID	��̂ƂȂ�|�P������ID
*	work	���[�N�p�z��i�v�f����EVENT_HANDLER_WORK_ELEMS, ����Ăяo�����[���N���A���ۏ؂����j
*/
//--------------------------------------------------------------
typedef void (*BtlEventHandler)( BTL_SERVER* sv, u8 pokeID, int* work );

//--------------------------------------------------------------
/**
*	�C�x���g�n���h�����^�C�v�̊֘A�t���e�[�u��
*/
//--------------------------------------------------------------
typedef struct {
	BtlEventType		eventType;
	BtlEventHandler		handler;
}BtlEventHandlerTable;

//--------------------------------------------------------------
/**
*	�C�x���g���̌^
*/
//--------------------------------------------------------------
typedef struct _BTL_EVENT_FACTOR	BTL_EVENT_FACTOR;



extern BTL_EVENT_FACTOR* BTL_EVENT_AddFactor( BtlEventFactor factorType, u16 subPri, u8 pokeID, const BtlEventHandlerTable* handlerTable );
extern void BTL_EVENT_RemoveFactor( BTL_EVENT_FACTOR* factor );

#endif
