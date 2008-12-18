//=============================================================================================
/**
 * @file	btl_adapter.h
 * @brief	�|�P����WB �o�g���V�X�e��	�T�[�o-�N���C�A���g�ԃA�_�v�^�[
 * @author	taya
 *
 * @date	2008.09.25	�쐬
 */
//=============================================================================================
#ifndef __BTL_ADAPTER_H__
#define __BTL_ADAPTER_H__

#include "battle/battle.h"
#include "btl_common.h"


//--------------------------------------------------------------
/**
 *	�n���h���p�^��`
 */
//--------------------------------------------------------------
typedef struct _BTL_ADAPTER		BTL_ADAPTER;


//--------------------------------------------------------------
/**
 *	�A�_�v�^�[�^�C�v
 */
//--------------------------------------------------------------
typedef enum {
	BTL_ADAPTER_LOCAL,			///< �ʐM�����A�T�[�o�Ɠ���}�V����ɃN���C�A���g������
	BTL_ADAPTER_COMM_PARENT,	///< DS �����ʐM�𗘗p�i�e�@���j
	BTL_ADAPTER_COMM_CHILD,		///< DS �����ʐM���𗘗p�i�q�@���j
	BTL_ADAPTER_WIFI_PARENT,	///< DS Wi-Fi �@�\�𗘗p�i�e�@���j
	BTL_ADAPTER_WIFI_CHILD,		///< DS Wi-Fi �@�\�𗘗p�i�q�@���j
}BtlAdapterType;

//--------------------------------------------------------------
/**
 *	�A�_�v�^�[�R�}���h
 */
//--------------------------------------------------------------
typedef enum {
	BTL_ACMD_NONE = 0,
	BTL_ACMD_NOTIFY_POKEDATA,
	BTL_ACMD_WAIT_INITIALIZE,
	BTL_ACMD_SELECT_ACTION,
	BTL_ACMD_SELECT_POKEMON,
	BTL_ACMD_SERVER_CMD,

	BTL_ACMD_MAX,
}BtlAdapterCmd;


//--------------------------------------------------------------------------------------
/**
 * �V�X�e���������C�I���i�o�g���V�X�e���N������C�I�����O�ɂP�񂾂��Ăяo���j
 */
//--------------------------------------------------------------------------------------
extern void BTL_ADAPTERSYS_Init( BtlCommMode mode );
extern void BTL_ADAPTERSYS_Quit( void );

extern BTL_ADAPTER*  BTL_ADAPTER_Create( GFL_NETHANDLE* netHandle, HEAPID heapID, u16 adapterID );
extern void BTL_ADAPTER_Delete( BTL_ADAPTER* wk );


// for server
extern void BTL_ADAPTERSYS_BeginSetCmd( void );
extern void BTL_ADAPTERSYS_EndSetCmd( void );
extern void BTL_ADAPTER_SetCmd( BTL_ADAPTER* wk, BtlAdapterCmd cmd, const void* sendData, u32 sendDataSize );
extern BOOL BTL_ADAPTER_WaitCmd( BTL_ADAPTER* wk );
extern void BTL_ADAPTER_ResetCmd( BTL_ADAPTER *wk );
extern const void* BTL_ADAPTER_GetReturnData( BTL_ADAPTER* wk );
extern u32 BTL_ADAPTER_GetReturnDataSize( BTL_ADAPTER* wk );

// for clients
extern BtlAdapterCmd BTL_ADAPTER_RecvCmd( BTL_ADAPTER* wk );
extern u32 BTL_ADAPTER_GetRecvData( BTL_ADAPTER* wk, const void** ppRecv );
extern void BTL_ADAPTER_ReturnCmd( BTL_ADAPTER* wk, const void* data, u32 size );




#endif

