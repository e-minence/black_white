//=============================================================================================
/**
 * @file  btl_client.h
 * @brief �|�P����WB �o�g���V�X�e�� �N���C�A���g���W���[��
 * @author  taya
 *
 * @date  2008.09.06  �쐬
 */
//=============================================================================================
#ifndef __BTL_CLIENT_H__
#define __BTL_CLIENT_H__

//--------------------------------------------------------------
/**
 *  �N���C�A���g���W���[���n���h���^��`
 */
//--------------------------------------------------------------
typedef struct _BTL_CLIENT    BTL_CLIENT;


//--------------------------------------------------------------
/**
 *  �����E�����֎~�R�[�h
 */
//--------------------------------------------------------------

typedef enum {

  BTL_CANTESC_START = 0,

  BTL_CANTESC_KAGEFUMI = BTL_CANTESC_START,
  BTL_CANTESC_ARIJIGOKU,
  BTL_CANTESC_JIRYOKU,

  BTL_CANTESC_MAX,
  BTL_CANTESC_NULL = BTL_CANTESC_MAX,

}BtlCantEscapeCode;


//----------------------------------------------------------------------
// �Q�ƃw�b�_ include
//----------------------------------------------------------------------
#include "battle/battle.h"
#include "btl_main.h"
#include "btl_adapter.h"
#include "btl_pokeparam.h"
#include "btlv/btlv_core.h"
#include "app/b_bag.h"

extern BTL_CLIENT* BTL_CLIENT_Create(
  BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokecon,
  BtlCommMode commMode, GFL_NETHANDLE* netHandle, u16 clientID, u16 numCoverPos,
  BtlThinkerType clientType, BtlBagMode bagMode,
  HEAPID heapID );

extern void BTL_CLIENT_Delete( BTL_CLIENT* wk );
extern u8 BTL_CLIENT_GetEscapeClientID( const BTL_CLIENT* wk );


extern BTL_ADAPTER* BTL_CLIENT_GetAdapter( BTL_CLIENT* client );

extern void BTL_CLIENT_AttachViewCore( BTL_CLIENT* wk, BTLV_CORE* viewCore );

extern BOOL BTL_CLIENT_Main( BTL_CLIENT* client );


extern u8 BTL_CLIENT_GetClientID( const BTL_CLIENT* client );
extern const BTL_PARTY* BTL_CLIENT_GetParty( const BTL_CLIENT* client );
extern BtlPokePos BTL_CLIENT_GetProcPokePos( const BTL_CLIENT* client );





#endif
