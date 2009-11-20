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
  EVENT_HANDLER_WORK_ELEMS = 8,       ///< �n���h�����ƂɊ��蓖�Ă��郍�[�J�����[�N�v�f��(int)
  EVENT_HANDLER_TMP_WORK_SIZE = 128,  ///< �S�n���h������Q�Ƃ����ꎞ�g�p���[�N�T�C�Y

  EVENT_WAZA_STICK_MAX = 8,   ///< �P�̂ɂ��\��t�����Ƃ��o���郏�U���̍ő�l
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
* myHandle  ���g�폜�p�̃n���h��
* flowWk    �T�[�o�t���[���[�N
* pokeID    ��̂ƂȂ�|�P������ID
* work      ���[�N�p�z��i�v�f����EVENT_HANDLER_WORK_ELEMS, ����Ăяo�����[���N���A���ۏ؂����j
*/
//--------------------------------------------------------------
typedef void (*BtlEventHandler)( BTL_EVENT_FACTOR* myHandle, BTL_SVFLOW_WORK* flowWk, u8 pokeID, int* work );


//--------------------------------------------------------------
/**
* �C�x���g�X�L�b�v�`�F�b�N�n���h���^
* ����̃n���h���Ăяo�����X�L�b�v���邩�ǂ�������ɔ�������ꍇ�A���̃n���h����ǉ��ŃA�^�b�`����
* �i������A�Ƃ������u������Ԃ�v������̂Ƃ������𖳌�������ꍇ�Ɏg�p���邱�Ƃ�z��j
*
* myHandle      �������g�̃n���h��
* factorType    �����t�@�N�^�[�^�C�v�i���U�A�Ƃ������A�A�C�e�����j
* eventType     �C�x���gID
* subID         �t�@�N�^�[�^�C�v���Ƃ̃T�uID�i���U�Ȃ�u�Ȃ݂̂�v�C�Ƃ������Ȃ�u�������v�Ȃǂ�ID)
* pokeID        ��̂ƂȂ�|�P������ID
*
* return  �X�L�b�v����ꍇTRUE
*/
//--------------------------------------------------------------
typedef BOOL (*BtlEventSkipCheckHandler)( BTL_EVENT_FACTOR* myHandle, BtlEventFactorType factorType, BtlEventType eventType, u16 subID, u8 pokeID );

//--------------------------------------------------------------
/**
* �C�x���g�n���h�����^�C�v�̊֘A�t���e�[�u��
*/
//--------------------------------------------------------------
typedef struct {
  BtlEventType    eventType;
  BtlEventHandler   handler;
}BtlEventHandlerTable;



extern BTL_EVENT_FACTOR* BTL_EVENT_AddFactor( BtlEventFactorType factorType, u16 subID, u16 priority, u8 pokeID, const BtlEventHandlerTable* handlerTable );
extern BTL_EVENT_FACTOR* BTL_EVENT_SeekFactorCore( BtlEventFactorType factorType );
extern BTL_EVENT_FACTOR* BTL_EVENT_SeekFactor( BtlEventFactorType factorType, u8 pokeID );
extern BTL_EVENT_FACTOR* BTL_EVENT_GetNextFactor( BTL_EVENT_FACTOR* factor );
extern void BTL_EVENT_FACTOR_Remove( BTL_EVENT_FACTOR* factor );
extern void BTL_EVENT_FACTOR_ChangePokeParam( BTL_EVENT_FACTOR* factor, u8 pokeID, u16 pri );
extern u16 BTL_EVENT_FACTOR_GetSubID( const BTL_EVENT_FACTOR* factor );
extern int BTL_EVENT_FACTOR_GetWorkValue( const BTL_EVENT_FACTOR* factor, u8 workIdx );
extern void BTL_EVENT_FACTOR_SetWorkValue( BTL_EVENT_FACTOR* factor, u8 workIdx, int value );
extern u8 BTL_EVENT_FACTOR_GetPokeID( const BTL_EVENT_FACTOR* factor );
extern void BTL_EVENT_FACTOR_AttachSkipCheckHandler( BTL_EVENT_FACTOR* factor, BtlEventSkipCheckHandler handler );
extern void BTL_EVENT_FACTOR_SetForceCallFlag( BTL_EVENT_FACTOR* factor );

//=============================================================================================
/**
 * ����^�C�v���T�uID�̃t�@�N�^�[���P�^�[���x�~������
 *
 * @param   type
 * @param   subID
 *
 */
//=============================================================================================
extern void BTL_EVENT_SleepFactor( BtlEventFactorType type, u16 subID );

#endif
