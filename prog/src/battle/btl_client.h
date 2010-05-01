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
  BTL_CANTESC_TOOSENBOU,

  BTL_CANTESC_MAX,
  BTL_CANTESC_NULL = BTL_CANTESC_MAX,

}BtlCantEscapeCode;

//--------------------------------------------------------------
/**
 *  �����E�����I���R�[�h
 */
//--------------------------------------------------------------
typedef enum {
  BTL_CLIENTASK_CHANGE = 0,
  BTL_CLIENTASK_ESCAPE,
}BtlClientChangeEscSelect;


//--------------------------------------------------------------
/**
 *  �ΐ폟�s����\����
 */
//--------------------------------------------------------------
typedef struct {
  u16 clientID;     ///< ���s���肵���}�V���̃N���C�A���gID
  u16 resultCode;   ///< ���s���肵���}�V���ł̏��s�R�[�h
}BTL_RESULT_CONTEXT;

//----------------------------------------------------------------------
// �Q�ƃw�b�_ include
//----------------------------------------------------------------------
#include "battle/battle.h"
#include "btl_main_def.h"
#include "btl_adapter.h"
#include "btl_pokeparam.h"
#include "btl_rec.h"
#include "btl_server.h"
#include "btlv/btlv_core_def.h"
#include "app/b_bag.h"

extern void BTL_CLIENTSYSTEM_Init( void );


extern BTL_CLIENT* BTL_CLIENT_Create(
  BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokecon,
  BtlCommMode commMode, GFL_NETHANDLE* netHandle, u16 clientID, u16 numCoverPos,
  BtlClientType clientType, BtlBagMode bagMode, BOOL fRecPlayMode,
  const GFL_STD_RandContext* randContext, HEAPID heapID );


//  wk->client[0] = BTL_CLIENT_Create( wk, &wk->pokeconForClient, BTL_COMM_NONE, sp->netHandle, 0, 1,
//              BTL_CLIENT_TYPE_UI, bagMode, sp->fRecordPlay, &wk->randomContext, wk->heapID );


extern void BTL_CLIENT_Delete( BTL_CLIENT* wk );

extern void BTL_CLIENT_GetEscapeInfo( const BTL_CLIENT* wk, BTL_ESCAPEINFO* dst );
extern BOOL BTL_CLIENT_IsGameTimeOver( const BTL_CLIENT* wk );


extern BTL_ADAPTER* BTL_CLIENT_GetAdapter( BTL_CLIENT* client );


extern void BTL_CLIENT_AttachViewCore( BTL_CLIENT* wk, BTLV_CORE* viewCore );
extern void BTL_CLIENT_AttachCmeCheckServer( BTL_CLIENT* wk, BTL_SERVER* server );


extern BOOL BTL_CLIENT_Main( BTL_CLIENT* client );


extern u8 BTL_CLIENT_GetClientID( const BTL_CLIENT* client );
extern const BTL_PARTY* BTL_CLIENT_GetParty( const BTL_CLIENT* client );
extern BtlPokePos BTL_CLIENT_GetProcPokePos( const BTL_CLIENT* client );
extern u8 BTL_CLIENT_GetShooterEnergy( const BTL_CLIENT* client );

extern const void* BTL_CLIENT_GetRecordData( BTL_CLIENT* wk, u32* size );
extern void BTL_CLIENT_SetRecordPlayerMode( BTL_CLIENT* wk, BTL_RECREADER* recReader );


extern void BTL_CLIENT_SetChapterSkip( BTL_CLIENT* wk, u32 nextTurnNum );
extern void BTL_CLIENT_StopChapterSkip( BTL_CLIENT* wk );
extern BOOL BTL_CLIENT_IsChapterSkipMode( const BTL_CLIENT* wk );
extern BOOL BTL_CLIENT_IsRecPlayerMode( const BTL_CLIENT* wk );
extern u32 BTL_CLIENT_GetRecPlayerMaxChapter( const BTL_CLIENT* wk );



#endif
