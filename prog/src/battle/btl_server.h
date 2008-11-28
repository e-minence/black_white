//=============================================================================================
/**
 * @file	btl_server.h
 * @brief	�|�P����WB �o�g���V�X�e��	�T�[�o���W���[��
 * @author	taya
 *
 * @date	2008.09.06	�쐬
 */
//=============================================================================================
#ifndef __BTL_SERVER_H__
#define __BTL_SERVER_H__

#include "btl_common.h"
#include "btl_main.h"
#include "btl_adapter.h"

typedef struct _BTL_SERVER		BTL_SERVER;


typedef enum {

	SC_RES_CONTINUE,		///< ���̃^�[���ցB
	SC_RES_WAIT_CHANGE,		///< ����ւ��҂�
	SC_RES_ESCAPE,			///< �ǂ��炩�������ďI��
	SC_RES_CLOSE,			///< �ǂ��炩�������ďI��


}ServerCmdResult;


extern BTL_SERVER* BTL_SERVER_Create( BTL_MAIN_MODULE* mainWork, HEAPID heapID );
extern void BTL_SERVER_Delete( BTL_SERVER* wk );


extern void BTL_SERVER_AttachLocalClient( BTL_SERVER* server, BTL_ADAPTER* adapter, BTL_PARTY* party, u16 clientID );
extern void BTL_SERVER_ReceptionNetClient( BTL_SERVER* server, BtlCommMode commMode, GFL_NETHANDLE* netHandle, u16 clientID );
extern BOOL BTL_SERVER_Main( BTL_SERVER* server );

// ���U���s���R
typedef enum {

	SCARG_WAZAFAIL_OTHER = 0,

	SCARG_WAZAFAIL_MAHI,
	SCARG_WAZAFAIL_HIRUMI,
	SCARG_WAZAFAIL_MEROMERO,
	SCARG_WAZAFAIL_CHOUHATU,


}ServerArgWazaFailReason;





static inline u8 BTL_SERVER_MakeArg8_44( u8 arg1, u8 arg2 )
{
	GF_ASSERT((arg1&0x0f)==0);
	GF_ASSERT((arg2&0x0f)==0);

	return ((arg1&0x0f)<<4) | (arg2&0x0f);
}



//------------------------------------------------
// �C�x���g�n���h���Ƃ̘A���֐�
//------------------------------------------------
extern u8 BTL_SERVER_RECEPT_PokeIDtoClientID( const BTL_SERVER* sv, u8 pokeID );

extern int BTL_SERVER_RECEPT_GetEventArg( const BTL_SERVER* server, u8 idx );
extern void BTL_SERVER_RECEPT_SetEventArg( BTL_SERVER* server, u8 idx, int value );

extern void BTL_SERVER_RECTPT_SetMessage( BTL_SERVER* server, u16 msgID, u8 clientID );
extern void BTL_SERVER_RECEPT_TokuseiWinIn( BTL_SERVER* server, u8 pokeID );
extern void BTL_SERVER_RECEPT_TokuseiWinOut( BTL_SERVER* server, u8 pokeID );

extern void BTL_SERVER_RECEPT_RankDownEffect( BTL_SERVER* server, BtlExClientID exID, BppValueID statusType, u8 volume );

#endif
