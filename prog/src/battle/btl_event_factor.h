//=============================================================================================
/**
 * @file  btl_event_factor.h
 * @brief �|�P����WB �o�g���V�X�e�� �C�x���g���n���h�������iFACTOR���ɂ̂݌��J�j
 * @author  taya
 *
 * @date  2008.10.23  �쐬
 */
//=============================================================================================
#ifndef __BTL_EVENT_FACTOR_H__
#define __BTL_EVENT_FACTOR_H__

#include "waza_tool\wazadata.h"

#include "btl_event.h"
#include "btl_server_flow.h"
#include "btl_server_local.h"
#include "btl_server.h"
#include "btl_pokeparam.h"


enum {
  EVENT_HANDLER_WORK_ELEMS = 8,
};

//--------------------------------------------------------------
/**
* �C�x���g���̌^
*/
//--------------------------------------------------------------
typedef struct _BTL_EVENT_FACTOR  BTL_EVENT_FACTOR;

//--------------------------------------------------------------
/**
* �C�x���g�n���h���^
*
* factor  ���g�폜�p�̃n���h��
* flowWk  �T�[�o�t���[���[�N
* pokeID  ��̂ƂȂ�|�P������ID
* work  ���[�N�p�z��i�v�f����EVENT_HANDLER_WORK_ELEMS, ����Ăяo�����[���N���A���ۏ؂����j
*/
//--------------------------------------------------------------
typedef void (*BtlEventHandler)( BTL_EVENT_FACTOR* factor, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );

//--------------------------------------------------------------
/**
* �C�x���g�n���h�����^�C�v�̊֘A�t���e�[�u��
*/
//--------------------------------------------------------------
typedef struct {
  BtlEventType    eventType;
  BtlEventHandler   handler;
}BtlEventHandlerTable;



extern BTL_EVENT_FACTOR* BTL_EVENT_AddFactor( BtlEventFactor factorType, u16 subID, u16 priority, u8 pokeID, const BtlEventHandlerTable* handlerTable );
extern BTL_EVENT_FACTOR* BTL_EVENT_SeekFactor( BtlEventFactor factorType, u8 pokeID );
extern BTL_EVENT_FACTOR* BTL_EVENT_GetNextFactor( BTL_EVENT_FACTOR* factor );
extern void BTL_EVENT_FACTOR_Remove( BTL_EVENT_FACTOR* factor );
extern void BTL_EVENT_FACTOR_ChangePokeParam( BTL_EVENT_FACTOR* factor, u8 pokeID, u16 pri );
extern u16 BTL_EVENT_FACTOR_GetSubID( const BTL_EVENT_FACTOR* factor );
extern int BTL_EVENT_FACTOR_GetWorkValue( const BTL_EVENT_FACTOR* factor, u8 workIdx );
extern void BTL_EVENT_FACTOR_SetWorkValue( BTL_EVENT_FACTOR* factor, u8 workIdx, int value );


//=============================================================================================
/**
 * ����^�C�v���T�uID�̃t�@�N�^�[���P�^�[���x�~������
 *
 * @param   type
 * @param   subID
 *
 */
//=============================================================================================
extern void BTL_EVENT_SleepFactor( BtlEventFactor type, u16 subID );

#endif
