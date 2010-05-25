//=============================================================================================
/**
 * @file  btl_adapter.h
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o-�N���C�A���g�ԃA�_�v�^�[
 * @author  taya
 *
 * @date  2008.09.25  �쐬
 */
//=============================================================================================
#ifndef __BTL_ADAPTER_H__
#define __BTL_ADAPTER_H__

#include "battle/battle.h"
#include "btl_common.h"


//--------------------------------------------------------------
/**
 *  �n���h���p�^��`
 */
//--------------------------------------------------------------
typedef struct _BTL_ADAPTER   BTL_ADAPTER;

//--------------------------------------------------------------
/**
 *  �A�_�v�^�[�R�}���h
 */
//--------------------------------------------------------------
typedef enum {
  BTL_ACMD_NONE = 0,
  BTL_ACMD_WAIT_SETUP,
  BTL_ACMD_SELECT_ROTATION,
  BTL_ACMD_SELECT_ACTION,
  BTL_ACMD_CONFIRM_IREKAE,
  BTL_ACMD_SELECT_POKEMON_FOR_COVER,
  BTL_ACMD_SELECT_POKEMON_FOR_CHANGE,
  BTL_ACMD_SELECT_CHANGE_OR_ESCAPE,
  BTL_ACMD_SERVER_CMD,
  BTL_ACMD_QUIT_BTL,
  BTL_ACMD_RECORD_DATA,
  BTL_ACMD_EXIT_NPC,
  BTL_ACMD_EXIT_SUBWAY_TRAINER,
  BTL_ACMD_EXIT_COMM,
  BTL_ACMD_EXIT_WIN_WILD,
  BTL_ACMD_EXIT_LOSE_WILD,
  BTL_ACMD_NOTIFY_TIMEUP,

  BTL_ACMD_MAX,
}BtlAdapterCmd_tag;
typedef u8 BtlAdapterCmd;

//--------------------------------------------------------------------------------------
/**
 * �V�X�e���������C�I���i�o�g���V�X�e���N������C�I�����O�ɂP�񂾂��Ăяo���j
 */
//--------------------------------------------------------------------------------------
extern void BTL_ADAPTERSYS_Init( BtlCommMode mode );
extern void BTL_ADAPTERSYS_Quit( void );

extern BTL_ADAPTER*  BTL_ADAPTER_Create( GFL_NETHANDLE* netHandle, u8 adapterID, BOOL fCommType, HEAPID heapID );
extern void BTL_ADAPTER_Delete( BTL_ADAPTER* wk );


// for server
extern void BTL_ADAPTERSYS_BeginSetCmd( void );
extern void BTL_ADAPTERSYS_EndSetCmd( void );
extern void BTL_ADAPTER_SetCmd( BTL_ADAPTER* wk, BtlAdapterCmd cmd, const void* sendData, u32 sendDataSize );
extern BOOL BTL_ADAPTER_WaitCmd( BTL_ADAPTER* wk );
extern void BTL_ADAPTER_ResetCmd( BTL_ADAPTER *wk );
extern const void* BTL_ADAPTER_GetReturnData( BTL_ADAPTER* wk, u32* size );
extern u32 BTL_ADAPTER_GetReturnDataSize( BTL_ADAPTER* wk );

// for clients
extern BtlAdapterCmd BTL_ADAPTER_RecvCmd( BTL_ADAPTER* wk );
extern u32 BTL_ADAPTER_GetRecvData( BTL_ADAPTER* wk, const void** ppRecv );
extern BOOL BTL_ADAPTER_ReturnCmd( BTL_ADAPTER* wk, const void* data, u32 size );




#endif

