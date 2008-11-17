//=============================================================================================
/**
 * @file	btlv_core.h
 * @brief	�|�P����WB �o�g���`��S�����C�����W���[��
 * @author	taya
 *
 * @date	2008.10.02	�쐬
 */
//=============================================================================================
#ifndef __BTLV_CORE_H__
#define __BTLV_CORE_H__


//----------------------------------------------------------------------
/**
 *	�`�惁�C�����W���[���n���h���^��`
 */
//----------------------------------------------------------------------
typedef  struct _BTLV_CORE		BTLV_CORE;


//----------------------------------------------------------------------
// �Q�ƃw�b�_ include 
//----------------------------------------------------------------------
#include "btl_main.h"
#include "btl_client.h"
#include "btl_action.h"
#include "btl_calc.h"
#include "btl_string.h"

//----------------------------------------------------------------------
/**
 *	�`��R�}���h
 */
//----------------------------------------------------------------------
typedef enum {

	BTLV_CMD_SETUP,
	BTLV_CMD_SELECT_ACTION,
	BTLV_CMD_SELECT_POKEMON,
	BTLV_CMD_MSG,

	BTLV_CMD_MAX,
	BTLV_CMD_NULL = BTLV_CMD_MAX,

}BtlvCmd;



//=============================================================================================
/**
 * �`�惁�C�����W���[���̐���
 *
 * @param   mainModule		�V�X�e�����C�����W���[���̃n���h��
 * @param   heapID			������q�[�vID
 *
 * @retval  BTLV_CORE*		�������ꂽ�`�惁�C�����W���[���̃n���h��
 */
//=============================================================================================
extern BTLV_CORE*  BTLV_Create( BTL_MAIN_MODULE* mainModule, const BTL_CLIENT* client, HEAPID heapID );

//=============================================================================================
/**
 * BTLV_StartCommand �Ŏ󂯕t�����`��R�}���h�̏I����҂�
 *
 * @param   wk			�`�惁�C�����W���[���̃n���h��
 *
 * @retval  BOOL		�I�����Ă�����TRUE
 */
//=============================================================================================
extern void BTLV_StartCommand( BTLV_CORE* btlv, BtlvCmd cmd );

//=============================================================================================
/**
 * BTLV_StartCommand �Ŏ󂯕t�����`��R�}���h�̏I����҂�
 *
 * @param   wk			�`�惁�C�����W���[���̃n���h��
 *
 * @retval  BOOL		�I�����Ă�����TRUE
 */
//=============================================================================================
extern BOOL BTLV_WaitCommand( BTLV_CORE* btlv );


//=============================================================================================
/**
 * 
 *
 * @param   core		
 * @param   dst		
 *
 * @retval  extern void		
 */
//=============================================================================================
extern void BTLV_GetActionParam( const BTLV_CORE* core, BTL_ACTION_PARAM* dst );


extern void BTLV_StartMemberChangeAct( BTLV_CORE* wk, u8 clientID, u8 memberIdx );
extern BOOL BTLV_WaitMemberChangeAct( BTLV_CORE* wk );


//=============================================================================================
/**
 * 
 *
 * @param   wk		
 * @param   strID		
 * @param   clientID		
 *
 * @retval  extern void		
 */
//=============================================================================================
extern void BTLV_StartMsgSpecific( BTLV_CORE* wk, BtlSpStrID strID, const int* args );



extern void BTLV_StartWazaAct( BTLV_CORE* wk, u8 atClientID, u8 defClientID, WazaID waza, BtlTypeAff affinity );

extern void BTLV_StartTokWin( BTLV_CORE* wk, u8 clientID );
extern void BTLV_QuitTokWin( BTLV_CORE* wk, u8 clientID );
extern void BTLV_StartRankDownEffect( BTLV_CORE* wk, u8 clientID, BppValueID statusType );



#endif
