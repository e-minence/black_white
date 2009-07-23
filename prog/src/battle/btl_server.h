//=============================================================================================
/**
 * @file  btl_server.h
 * @brief ポケモンWB バトルシステム サーバモジュール
 * @author  taya
 *
 * @date  2008.09.06  作成
 */
//=============================================================================================
#ifndef __BTL_SERVER_H__
#define __BTL_SERVER_H__

#include "btl_common.h"
#include "btl_main.h"
#include "btl_adapter.h"

typedef struct _BTL_SERVER    BTL_SERVER;


typedef enum {

  SC_RES_CONTINUE,    ///< 次のターンへ。
  SC_RES_WAIT_CHANGE, ///< 入れ替え待ち
  SC_RES_ESCAPE,      ///< どちらかが逃げて終了
  SC_RES_CLOSE,       ///< どちらかが勝って終了


}ServerCmdResult;


extern BTL_SERVER* BTL_SERVER_Create( BTL_MAIN_MODULE* mainWork, BTL_POKE_CONTAINER* pokeCon, HEAPID heapID );
extern void BTL_SERVER_AttachLocalClient( BTL_SERVER* server, BTL_ADAPTER* adapter, u8 clientID, u8 numCoverPos );
extern void BTL_SERVER_ReceptionNetClient( BTL_SERVER* server, BtlCommMode commMode, GFL_NETHANDLE* netHandle, u8 clientID, u8 numCoverPos );
extern void BTL_SERVER_Startup( BTL_SERVER* server );
extern void BTL_SERVER_Delete( BTL_SERVER* wk );

extern void BTL_SERVER_Main( BTL_SERVER* server );

//-------------------------------------
// server_flow -> server 呼び出し専用
//-------------------------------------
extern void BTL_SERVER_AddBonusMoney( BTL_SERVER* server, u32 volume );


#endif
