//=============================================================================================
/**
 * @file  btl_server.c
 * @brief �|�P����WB �o�g���V�X�e�� �T�[�o���W���[��
 * @author  taya
 *
 * @date  2008.09.06  �쐬
 */
//=============================================================================================
#include <gflib.h>
#include "waza_tool\wazadata.h"
#include "gamesystem\game_beacon.h"
#include "sound\pm_sndsys.h"

#include "btl_common.h"
#include "btl_client.h"
#include "btl_adapter.h"
#include "btl_action.h"
#include "btl_pokeparam.h"
#include "btl_calc.h"
#include "btl_event.h"
#include "btl_util.h"
#include "btl_string.h"
#include "btl_rec.h"

#include "btl_server_cmd.h"
#include "btl_field.h"
#include "btl_server_flow.h"
#include "btl_server_local.h"
#include "handler\hand_tokusei.h"

#include "btl_server.h"



/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  SERVER_CMD_SIZE_MAX = 1024,
  CLIENT_DISABLE_ID = 0xff,
  SERVER_STRBUF_SIZE = 32,
};

enum {
  QUITSTEP_NONE = 0,    ///
  QUITSTEP_REQ,
  QUITSTEP_CORE,
};



//--------------------------------------------------------------
/**
 *  �T�[�o���C�����[�v�֐��^  - �o�g���I�����̂� TRUE ��Ԃ� -
 */
//--------------------------------------------------------------
typedef BOOL (*ServerMainProc)(BTL_SERVER*, int*);


//--------------------------------------------------------------
/**
 *
 */
//--------------------------------------------------------------
struct _BTL_SERVER {
  ServerMainProc    mainProc;
  int               seq;

  BTL_MAIN_MODULE*      mainModule;
  BTL_POKE_CONTAINER*   pokeCon;
  SVCL_WORK             client[ BTL_CLIENT_MAX ];
  BTL_SVFLOW_WORK*      flowWork;
  SvflowResult          flowResult;
  BtlBagMode            bagMode;
  GFL_STD_RandContext   randContext;
  BTL_RECTOOL           recTool;
  STRBUF*               strbuf;

  u32         escapeClientID;
  u32         exitTimer;
  u8          enemyPutPokeID;
  u8          quitStep;


  BTL_SERVER_CMD_QUE  queBody;
  BTL_SERVER_CMD_QUE* que;

  u8                  changePokePos[ BTL_POS_MAX ];
  u8                  changePokeCnt;
  u8                  giveupClientID[ BTL_CLIENT_MAX ];
  u8                  giveupClientCnt;

  HEAPID        heapID;
};


/*--------------------------------------------------------------------------*/
/* Prototypes                                                               */
/*--------------------------------------------------------------------------*/
static void setMainProc( BTL_SERVER* sv, ServerMainProc mainProc );
static void setMainProc_Root( BTL_SERVER* server );
static BOOL ServerMain_WaitReady( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectRotation( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectAction( BTL_SERVER* server, int* seq );
static BOOL ServerMain_ConfirmChangeOrEscape( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectPokemonCover( BTL_SERVER* server, int* seq );
static BOOL Irekae_IsNeedConfirm( BTL_SERVER* server );
static u8 Irekae_GetEnemyPutPokeID( BTL_SERVER* server );
static BOOL ServerMain_SelectPokemonChange( BTL_SERVER* server, int* seq );
static BOOL ServerMain_BattleTimeOver( BTL_SERVER* server, int* seq );
static BOOL ServerMain_ExitBattle( BTL_SERVER* server, int* seq );
static BOOL ServerMain_ExitBattle_ForTrainer( BTL_SERVER* server, int* seq );
static BOOL SendActionRecord( BTL_SERVER* server );
static BOOL SendRotateRecord( BTL_SERVER* server, const BtlRotateDir* dirAry );
static void* MakeSelectActionRecord( BTL_SERVER* server, u32* dataSize );
static void* MakeRotationRecord( BTL_SERVER* server, u32* dataSize, const BtlRotateDir* dirAry );
static void SetAdapterCmd( BTL_SERVER* server, BtlAdapterCmd cmd );
static void SetAdapterCmdEx( BTL_SERVER* server, BtlAdapterCmd cmd, const void* sendData, u32 dataSize );
static void SetAdapterCmdSingle( BTL_SERVER* server, BtlAdapterCmd cmd, u8 clientID, const void* sendData, u32 dataSize );
static BOOL WaitAllAdapterReply( BTL_SERVER* server );
static void ResetAdapterCmd( BTL_SERVER* server );
static void Svcl_Clear( SVCL_WORK* clwk );
static BOOL Svcl_IsEnable( const SVCL_WORK* clwk );
static void Svcl_Setup( SVCL_WORK* clwk, u8 clientID, BTL_ADAPTER* adapter, BOOL fLocalClient );
static void Svcl_SetParty( SVCL_WORK* client, BTL_PARTY* party, u8 numCoverPos );





//=============================================================================================
/**
 * �T�[�o����
 *
 * @param   mainModule   ���C�����W���[���̃n���h��
 * @param   randContext  �����R���e�L�X�g�i�^��f�[�^�쐬�p�ɃN���C�A���g�ɒʒm����B�����������͊��ɍs���Ă���̂ŃT�[�o���s���K�v�͖����B�j
 * @param   pokeCon      �|�P�����f�[�^�R���e�i�n���h��
 * @param   bagMode      �o�b�O���[�h
 * @param   heapID       �q�[�vID
 *
 * @retval  BTL_SERVER*
 */
//=============================================================================================
BTL_SERVER* BTL_SERVER_Create( BTL_MAIN_MODULE* mainModule, const GFL_STD_RandContext* randContext,
  BTL_POKE_CONTAINER* pokeCon, BtlBagMode bagMode, HEAPID heapID )
{
  BTL_SERVER* sv = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SERVER) );

  sv->mainModule = mainModule;
  sv->pokeCon = pokeCon;
  sv->heapID = heapID;
  sv->que = &sv->queBody;
  sv->quitStep = QUITSTEP_NONE;
  sv->flowWork = NULL;
  sv->changePokeCnt = 0;
  sv->giveupClientCnt = 0;
  sv->bagMode = bagMode;
  sv->escapeClientID = BTL_CLIENTID_NULL;
  sv->randContext = *randContext;
  sv->strbuf = GFL_STR_CreateBuffer( SERVER_STRBUF_SIZE, heapID );


  {
    int i;
    for(i=0; i<BTL_CLIENT_MAX; ++i){
      Svcl_Clear( &sv->client[i] );
    }
  }

  setMainProc( sv, ServerMain_WaitReady );

  return sv;
}

//--------------------------------------------------------------------------------------
/**
 * �T�[�o�Ɠ���}�V����ɂ���N���C�A���g�Ƃ̃A�_�v�^��ڑ�����
 *
 * @param   server      �T�[�o�n���h��
 * @param   adapter     �A�_�v�^�i���ɃN���C�A���g�Ɋ֘A�t�����Ă���j
 * @param   clientID    �N���C�A���gID
 * @param   numCoverPos �N���C�A���g���󂯎��퓬���|�P������
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_AttachLocalClient( BTL_SERVER* server, BTL_ADAPTER* adapter, u8 clientID, u8 numCoverPos )
{
  GF_ASSERT(clientID < BTL_CLIENT_MAX);
  GF_ASSERT(server->client[clientID].adapter==NULL);

  {
    SVCL_WORK* client;
    BTL_PARTY* party;

    party = BTL_POKECON_GetPartyData( server->pokeCon, clientID );
    client = &server->client[ clientID ];

    Svcl_Setup( client, clientID, adapter, TRUE );
    Svcl_SetParty( client, party, numCoverPos );
  }
}
//--------------------------------------------------------------------------------------
/**
 * �T�[�o�ƈقȂ�}�V����ɂ���N���C�A���g�Ƃ̃A�_�v�^�𐶐�����
 *
 * @param   server      �T�[�o�n���h��
 * @param   commMode    �ʐM���[�h
 * @param   netHandle   �l�b�g���[�N�n���h��
 * @param   clientID    �N���C�A���gID
 * @param   numCoverPos �N���C�A���g���󂯎��퓬���|�P������
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_ReceptionNetClient( BTL_SERVER* server, BtlCommMode commMode, GFL_NETHANDLE* netHandle, u8 clientID, u8 numCoverPos )
{
  GF_ASSERT(clientID < BTL_CLIENT_MAX);
  GF_ASSERT(server->client[clientID].adapter==NULL);

  {
    SVCL_WORK* client;
    BTL_PARTY* party;
    BTL_ADAPTER* adapter;

    party = BTL_POKECON_GetPartyData( server->pokeCon, clientID );
    adapter = BTL_ADAPTER_Create( netHandle, clientID, TRUE, server->heapID );

    client = &server->client[ clientID ];
    Svcl_Setup( client, clientID, adapter, FALSE );
    Svcl_SetParty( client, party, numCoverPos );
  }
}
//--------------------------------------------------------------------------------------
/**
 * �S�N���C�A���g�����E�ڑ�������̃X�^�[�g�A�b�v����
 *
 * @param   server
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_Startup( BTL_SERVER* server )
{
  GF_ASSERT(server->flowWork==NULL);
  server->flowWork = BTL_SVFLOW_InitSystem( server, server->mainModule, server->pokeCon,
        &server->queBody, server->bagMode, server->heapID );
}
//--------------------------------------------------------------------------------------
/**
 * �T�[�o�폜
 *
 * @param   wk
 *
 */
//--------------------------------------------------------------------------------------
void BTL_SERVER_Delete( BTL_SERVER* wk )
{
  int i;

  GFL_STR_DeleteBuffer( wk->strbuf );

  BTL_SVFLOW_QuitSystem( wk->flowWork );

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( (wk->client[i].myID != CLIENT_DISABLE_ID)
    &&  (wk->client[i].isLocalClient == FALSE)
    ){
      // ����}�V����ɂ���N���C�A���g�łȂ���΁A
      // �T�[�o���A�_�v�^��p�ӂ����n�Y�Ȃ̂Ŏ����ō폜
      BTL_ADAPTER_Delete( wk->client[i].adapter );
    }
  }
  GFL_HEAP_FreeMemory( wk );
}
//--------------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v
 *
 * @param   sv      �T�[�o�n���h��
 *
 * @retval  BOOL    �I���� TRUE ��Ԃ�
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SERVER_Main( BTL_SERVER* sv )
{
  if( sv->quitStep != QUITSTEP_CORE )
  {
    if( sv->mainProc( sv, &sv->seq ) )
    {
      sv->escapeClientID = BTL_SVFLOW_GetEscapeClientID( sv->flowWork );
      SetAdapterCmdEx( sv, BTL_ACMD_QUIT_BTL, &sv->escapeClientID, sizeof(sv->escapeClientID) );
      BTL_N_Printf( DBGSTR_SV_SendQuitACmad, BTL_ACMD_QUIT_BTL );
      sv->quitStep = QUITSTEP_CORE;
    }
  }
  else
  {
    if( WaitAllAdapterReply(sv) ){
      BTL_N_Printf( DBGSTR_SV_ReplyQuitACmad );
      return TRUE;
    }
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �I�����A�������N���C�A���g��ID���擾
 *
 * @param   sv
 *
 * @retval  u8    �������N���C�A���gID�i�N�������Ă��Ȃ��Ȃ�BTL_CLIENTID_NULL�j
 */
//----------------------------------------------------------------------------------
u8 BTL_SERVER_GetEscapeClientID( const BTL_SERVER* sv )
{
  return sv->escapeClientID;
}
//----------------------------------------------------------------------------------
/**
 * ���C�����[�v�֐��؂�ւ�
 *
 * @param   sv
 * @param   mainProc
 */
//----------------------------------------------------------------------------------
static void setMainProc( BTL_SERVER* sv, ServerMainProc mainProc )
{
  sv->mainProc = mainProc;
  sv->seq = 0;
}
//----------------------------------------------------------------------------------
/**
 * �󋵂ɉ����ă��[�g���C�����[�v�֐��؂蕪��
 *
 * @param   sv
 */
//----------------------------------------------------------------------------------
static void setMainProc_Root( BTL_SERVER* server )
{
  // ���[�e�[�V�����o�g��������^�[���ȊO�Ȃ�A�A�N�V�����̑O�Ƀ��[�e�[�V�����I������
  if( (BTL_MAIN_GetRule(server->mainModule) == BTL_RULE_ROTATION)
  &&  (BTL_SVFTOOL_GetTurnCount(server->flowWork) != 0 )
  ){
    setMainProc( server, ServerMain_SelectRotation );
  }else{
    setMainProc( server, ServerMain_SelectAction );
  }
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v�F��ʍ\�z�A�����|�P����G�t�F�N�g�܂ŏI���҂�
 *
 * @param   server
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL ServerMain_WaitReady( BTL_SERVER* server, int* seq )
{
  switch( *seq ){
  case 0:
    BTL_EVENT_InitSystem();
    SetAdapterCmd( server, BTL_ACMD_WAIT_SETUP );
    (*seq)++;
    /* fallthru */
  case 1:
    if( WaitAllAdapterReply(server) )
    {
      ResetAdapterCmd( server );

      // ���ꉉ�o������A�ߊl�f���ȊO�̏���
      if( BTL_MAIN_GetCompetitor(server->mainModule) != BTL_COMPETITOR_DEMO_CAPTURE )
      {
        BTL_SVFLOW_Start_AfterPokemonIn( server->flowWork );
        if( server->que->writePtr )
        {
          SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
          (*seq)++;
        }
        else
        {
          (*seq)+=2;
        }
      }
      // �ߊl�f���Ȃ�
      else
      {
        setMainProc( server, ServerMain_ExitBattle );
      }
    }
    break;
  case 2:
    if( WaitAllAdapterReply(server) )
    {
      ResetAdapterCmd( server );
      (*seq)++;
    }
    break;
  case 3:
    setMainProc_Root( server );
    break;
  }
  return FALSE;
}


//----------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v�F���[�e�[�V�����I���i���[�e�[�V�����o�g���̂݁j
 *
 * @param   server
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL ServerMain_SelectRotation( BTL_SERVER* server, int* seq )
{
  switch( *seq ){
  case 0:
    {
      static const BtlRotateDir dirTbl[] = {
//        BTL_ROTATEDIR_STAY,
        BTL_ROTATEDIR_R,
        BTL_ROTATEDIR_L,
      };
      BtlRotateDir  dirAry[ BTL_CLIENT_MAX ];
      u32 i;

      ResetAdapterCmd( server );
      SCQUE_Init( server->que );
      for(i=0; i<BTL_CLIENT_MAX; ++i)
      {
        if( Svcl_IsEnable(&server->client[i]) )
        {
          u8 rnd = BTL_CALC_GetRand( NELEMS(dirTbl) );
          dirAry[ i ] = dirTbl [rnd ];
          BTL_SVFLOW_CreateRotationCommand( server->flowWork, i, dirAry[ i ] );
        }
        else
        {
          dirAry[ i ] = BTL_ROTATEDIR_NONE;
        }
      }

      if( SendRotateRecord(server, dirAry) ){
        (*seq)++;
      }else{
        (*seq) += 2;
      }
    }
    break;

  case 1:
    if( WaitAllAdapterReply(server) )
    {
      ResetAdapterCmd( server );
      (*seq)++;
    }
    break;

  case 2:
    SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
    (*seq)++;
    break;

  case 3:
    if( WaitAllAdapterReply(server) )
    {
      ResetAdapterCmd( server );
      setMainProc( server, ServerMain_SelectAction );
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v�F�A�N�V�����I��
 *
 * @param   server
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL ServerMain_SelectAction( BTL_SERVER* server, int* seq )
{
  switch( *seq ){
  case 0:
    if( BTL_SVFLOW_MakeShooterChargeCommand(server->flowWork) ){
      BTL_N_Printf( DBGSTR_SERVER_SendShooterChargeCmd );
      SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
      (*seq)++;
    }else{
      (*seq) += 2;
    }
    break;

  case 1:
    if( WaitAllAdapterReply(server) )
    {
      BTL_Printf( DBGSTR_SERVER_ShooterChargeCmdDoneAll );
      ResetAdapterCmd( server );
      (*seq)++;
    }
    break;

  case 2:
    BTL_N_Printf( DBGSTR_SERVER_SendActionSelectCmd );
    SetAdapterCmd( server, BTL_ACMD_SELECT_ACTION );
    (*seq)++;
    break;

  case 3:
    if( WaitAllAdapterReply(server) )
    {
      BTL_N_Printf( DBGSTR_SERVER_ActionSelectDoneAll );
      ResetAdapterCmd( server );

      // �����������Ԑ؂�̃`�F�b�N
      if( BTL_MAIN_CheckGameLimitTimeOver(server->mainModule) )
      {
        setMainProc( server, ServerMain_BattleTimeOver );
        break;
      }
      server->flowResult = BTL_SVFLOW_StartTurn( server->flowWork );
      BTL_N_Printf( DBGSTR_SERVER_FlowResult, server->flowResult);

      if( SendActionRecord(server) ){
        (*seq)++;
      }else{
        (*seq) += 2;  /// ���炩�̗��R�Ř^��f�[�^�����Ɏ��s������X�L�b�v
      }
    }
    break;

  case 4:
    if( WaitAllAdapterReply(server) )
    {
      BTL_N_Printf( DBGSTR_SVFL_RecDataSendComped );
      ResetAdapterCmd( server );
      (*seq)++;
    }
    break;

  case 5:
      BTL_N_Printf( DBGSTR_SVFL_SendServerCmd, server->flowResult);
      SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
      (*seq)++;
      break;

  case 6:
    if( WaitAllAdapterReply(server) )
    {
      BTL_N_Printf( DBGSTR_SVFL_AllClientCmdPlayComplete, server->flowResult);
      BTL_MAIN_SyncServerCalcData( server->mainModule );
      ResetAdapterCmd( server );

      switch( server->flowResult ){
      case SVFLOW_RESULT_DEFAULT:
        setMainProc_Root( server );
        break;
      case SVFLOW_RESULT_POKE_COVER:
        BTL_N_Printf( DBGSTR_SV_PokeInReqForEmptyPos );
        {
          BtlCompetitor competitor = BTL_MAIN_GetCompetitor( server->mainModule );
          BtlRule rule = BTL_MAIN_GetRule( server->mainModule );
          if( (competitor == BTL_COMPETITOR_WILD) &&  (rule == BTL_RULE_SINGLE) ){
            setMainProc( server, ServerMain_ConfirmChangeOrEscape );
          }else{
            setMainProc( server, ServerMain_SelectPokemonCover );
          }
        }
        break;
      case SVFLOW_RESULT_POKE_CHANGE:
        BTL_N_Printf( DBGSTR_SV_ChangePokeOnTheTurn );
        GF_ASSERT( server->changePokeCnt );
        setMainProc( server, ServerMain_SelectPokemonChange );
        break;
      case SVFLOW_RESULT_POKE_GET:
        {
          BtlPokePos pos = BTL_SVFLOW_GetCapturedPokePos( server->flowWork );
          BTL_MAIN_NotifyCapturedPokePos( server->mainModule, pos );
          setMainProc( server, ServerMain_ExitBattle );
          return FALSE;
        }
      case SVFLOW_RESULT_BTL_SHOWDOWN:
        setMainProc( server, ServerMain_ExitBattle );
        return FALSE;
      default:
        GF_ASSERT(0);
        /* fallthru */
      case SVFLOW_RESULT_BTL_QUIT:
        BTL_N_Printf( DBGSTR_SVFL_GotoQuit );
        return TRUE;
      }
    }
    break;
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v�F�����̃|�P�������|�ꂽ���A�����邩����ւ��邩��I���i�쐶�V���O���̂݁j
 *
 * @param   server
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL ServerMain_ConfirmChangeOrEscape( BTL_SERVER* server, int* seq )
{
  switch( *seq ){
  case 0:
    SetAdapterCmd( server, BTL_ACMD_SELECT_CHANGE_OR_ESCAPE );
    (*seq)++;
    break;
  case 1:
    if( WaitAllAdapterReply(server) ){
      u8 clientID = BTL_MAIN_GetPlayerClientID( server->mainModule );
      const u8* result;

      ResetAdapterCmd( server );
      result = BTL_ADAPTER_GetReturnData( server->client[clientID].adapter, NULL );
      if( (*result) == BTL_CLIENTASK_CHANGE ){
        setMainProc( server, ServerMain_SelectPokemonCover );
      }else{
        server->flowResult = BTL_SVFLOW_CreatePlayerEscapeCommand( server->flowWork );
        SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
        (*seq)++;
      }
    }
    break;
  case 2:
    if( WaitAllAdapterReply(server) ){
      ResetAdapterCmd( server );

      if( server->flowResult ){
        return TRUE;
      }else{
        setMainProc( server, ServerMain_SelectPokemonCover );
      }
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v�F���S�E�����Ԃ�ŋ󂫈ʒu�Ƀ|�P�����𓊓�����
 *
 * @param   server
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL ServerMain_SelectPokemonCover( BTL_SERVER* server, int* seq )
{
  switch( *seq ){
  case 0:
    BTL_Printf("����|�P�����I����  ��ւ����|�P��=%d\n", server->changePokeCnt);
    SetAdapterCmdEx( server, BTL_ACMD_SELECT_POKEMON_FOR_COVER, server->changePokePos,
        server->changePokeCnt*sizeof(server->changePokePos[0]) );
    (*seq)++;
    break;

  case 1:
    if( WaitAllAdapterReply(server) )
    {
      if( Irekae_IsNeedConfirm(server) )
      {
        server->enemyPutPokeID = Irekae_GetEnemyPutPokeID( server );
        SetAdapterCmdSingle( server, BTL_ACMD_CONFIRM_IREKAE, BTL_CLIENTID_SA_PLAYER,
                &server->enemyPutPokeID, sizeof(server->enemyPutPokeID) );
      }
      (*seq)++;
    }
    break;

  case 2:
    if( WaitAllAdapterReply(server) )
    {
      ResetAdapterCmd( server );

      SCQUE_Init( server->que );
      server->flowResult = BTL_SVFLOW_StartAfterPokeIn( server->flowWork );
      BTL_N_Printf( DBGSTR_SERVER_FlowResult, server->flowResult );

      if( SendActionRecord(server) ){
        (*seq)++;
      }else{
        (*seq) += 2;  /// ���炩�̗��R�Ř^��f�[�^�����Ɏ��s������X�L�b�v
      }
    }
    break;

  case 3:
    if( WaitAllAdapterReply(server) ){
      ResetAdapterCmd( server );
      (*seq)++;
    }
    break;

  case 4:
    SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
    (*seq)++;
    break;

  case 5:
    if( WaitAllAdapterReply(server) )
    {
      ResetAdapterCmd( server );
      BTL_MAIN_SyncServerCalcData( server->mainModule );

      switch( server->flowResult ){
      case SVFLOW_RESULT_POKE_COVER:
        (*seq) = 0;
        break;
      case SVFLOW_RESULT_BTL_SHOWDOWN:
        setMainProc( server, ServerMain_ExitBattle );
        break;
      case SVFLOW_RESULT_BTL_QUIT:
        BTL_N_Printf( DBGSTR_SVFL_GotoQuit );
        return TRUE;
      default:
        setMainProc_Root( server );
        break;
      }
    }
    break;
  }

  return FALSE;
}
/**
 *  �X�^���h�A��������ւ���ɂāA�v���C���[���ɓ���ւ����m�F����K�v�����邩����
 */
static BOOL Irekae_IsNeedConfirm( BTL_SERVER* server )
{
  if( BTL_MAIN_IsIrekaeMode(server->mainModule) )
  {
    if( server->changePokeCnt )
    {
      u32 i;
      for(i=0; i<server->changePokeCnt; ++i)
      {
        // �v���C���[�����m�����N�G�X�g������ꍇ�A����ւ��m�F�̕K�v�Ȃ�
        if( BTL_MAIN_BtlPosToClientID(server->mainModule, server->changePokePos[i]) == BTL_CLIENTID_SA_PLAYER){
          return FALSE;
        }
      }
      return TRUE;
    }
  }
  return FALSE;
}
/**
 *  �X�^���h�A��������ւ���ɂāA�G�������ɂ���o���|�P����ID���擾
 */
static u8 Irekae_GetEnemyPutPokeID( BTL_SERVER* server )
{
  SVCL_WORK*  svcl = &server->client[ BTL_CLIENTID_SA_ENEMY1 ];

  if( Svcl_IsEnable(svcl) )
  {
    const BTL_ACTION_PARAM* actParam = BTL_ADAPTER_GetReturnData( svcl->adapter, NULL );
    if( BTL_ACTION_GetAction(actParam) == BTL_ACTION_CHANGE )
    {
      const BTL_POKEPARAM* bpp = BTL_POKECON_GetClientPokeDataConst(
                  server->pokeCon, BTL_CLIENTID_SA_ENEMY1, actParam->change.memberIdx );
      return BPP_GetID( bpp );
    }
  }
  return BTL_POKEID_NULL;
}

//----------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v�F�^�[���r���œ���ւ�����������̏���
 *
 * @param   server
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL ServerMain_SelectPokemonChange( BTL_SERVER* server, int* seq )
{
  switch( *seq ){
  case 0:
    BTL_N_Printf( DBGSTR_SV_StartChangePokeInfo, server->changePokeCnt);
    SetAdapterCmdEx( server, BTL_ACMD_SELECT_POKEMON_FOR_CHANGE, server->changePokePos,
        server->changePokeCnt*sizeof(server->changePokePos[0]) );
    (*seq)++;
    break;

  case 1:
    if( WaitAllAdapterReply(server) )
    {
      ResetAdapterCmd( server );
      SCQUE_Init( server->que );
      server->flowResult = BTL_SVFLOW_ContinueAfterPokeChange( server->flowWork );
      BTL_N_Printf( DBGSTR_SERVER_FlowResult, server->flowResult );

      if( SendActionRecord(server) ){
        (*seq)++;
      }else{
        (*seq) += 2;  /// ���炩�̗��R�Ř^��f�[�^�����Ɏ��s������X�L�b�v
      }
    }
    break;

  case 2:
    if( WaitAllAdapterReply(server) ){
      ResetAdapterCmd( server );
      (*seq)++;
    }
    break;

  case 3:
    SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
    (*seq)++;
    break;

  case 4:
    if( WaitAllAdapterReply(server) )
    {
      ResetAdapterCmd( server );
      BTL_MAIN_SyncServerCalcData( server->mainModule );

      switch( server->flowResult ){
      case SVFLOW_RESULT_POKE_CHANGE:
        (*seq) = 0;
        break;

      case SVFLOW_RESULT_POKE_COVER:
        setMainProc( server, ServerMain_SelectPokemonCover );
        break;
      case SVFLOW_RESULT_BTL_SHOWDOWN:
        setMainProc( server, ServerMain_ExitBattle );
        break;
      case SVFLOW_RESULT_BTL_QUIT:
        BTL_N_Printf( DBGSTR_SVFL_GotoQuit );
        return TRUE;
      default:
        setMainProc_Root( server );
        break;
      }
    }
    break;
  }

  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v�F�������ԏI��
 *
 * @param   server
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL ServerMain_BattleTimeOver( BTL_SERVER* server, int* seq )
{
  switch( *seq ){
  case 0:
    SetAdapterCmd( server, BTL_ACMD_NOTIFY_TIMEUP );
    (*seq)++;
    break;
  case 1:
    if( WaitAllAdapterReply(server) )
    {
      ResetAdapterCmd( server );
      setMainProc( server, ServerMain_ExitBattle );
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v�F�o�g���I������
 *
 * @param   server
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL ServerMain_ExitBattle( BTL_SERVER* server, int* seq )
{
  // @todo �{���͏���or�����A�쐶or�g���[�i�[��Ȃǂŕ��򂷂�
  switch( *seq ){
  case 0:
    if( BTL_MAIN_ChecBattleResult(server->mainModule) == BTL_RESULT_WIN )
    {
      PMSND_PlayBGM( SEQ_BGM_WIN1 );
      if( BTL_MAIN_GetCompetitor(server->mainModule) == BTL_COMPETITOR_TRAINER ){
        setMainProc( server, ServerMain_ExitBattle_ForTrainer );
      }else{
        (*seq)++;
      }
    }
    else
    {
      (*seq)++;
    }
    break;
  case 1:
    {
      u8 touch = FALSE;
      if( (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B))
      ||  (GFL_UI_TP_GetTrg())
      ){
        touch = TRUE;
      }
      if( touch ){
        return TRUE;
      }
    }
    break;
  }
  return FALSE;
}
//----------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v�F�o�g���I���i�Q�[�����ʏ�g���[�i�[�Ƃ̑ΐ�ɏ����j
 *
 * @param   server
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL ServerMain_ExitBattle_ForTrainer( BTL_SERVER* server, int* seq )
{
  switch( *seq ){
  case 0:
    SetAdapterCmd( server, BTL_ACMD_EXIT_WIN_TRAINER );
    (*seq)++;
    break;
  case 1:
    if( WaitAllAdapterReply(server) ){
      ResetAdapterCmd( server );
      (*seq)++;
    }
    break;
  case 2:
    {
      u8 touch = FALSE;
      if( (GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B))
      ||  (GFL_UI_TP_GetTrg())
      ){
        touch = TRUE;
      }
      if( touch ){
        return TRUE;
      }
    }
    break;
  }
  return FALSE;
}

//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------


/**
 *  �A�N�V�����L�^�f�[�^���M�J�n
 */
static BOOL SendActionRecord( BTL_SERVER* server )
{
  void* recData;
  u32   recDataSize;
  recData = MakeSelectActionRecord( server, &recDataSize );
  if( recData != NULL ){
//  BTL_Printf("�A�N�V�����L�^�f�[�^�𑗐M���� (%dbytes)\n", recDataSize);
    SetAdapterCmdEx( server, BTL_ACMD_RECORD_DATA, recData, recDataSize );
    return TRUE;
  }
  return FALSE;
}

/**
 *  ���[�e�[�V�����L�^�f�[�^���M�J�n
 */
static BOOL SendRotateRecord( BTL_SERVER* server, const BtlRotateDir* dirAry )
{
  void* recData;
  u32   recDataSize;
  recData = MakeRotationRecord( server, &recDataSize, dirAry );
  if( recData != NULL ){
    SetAdapterCmdEx( server, BTL_ACMD_RECORD_DATA, recData, recDataSize );
    return TRUE;
  }
  return FALSE;
}

/**
 * �A�N�V�����L�^�f�[�^�𐶐�
 *
 * @param   server
 * @param   dataSize   [out] �������ꂽ�f�[�^�T�C�Y
 *
 * @retval  void*   �����������ł����瑗�M�f�[�^�|�C���^ / �ł��Ȃ��ꍇNULL
 */
static void* MakeSelectActionRecord( BTL_SERVER* server, u32* dataSize )
{
  u32 ID;

  BTL_RECTOOL_Init( &server->recTool );

  for(ID=0; ID<BTL_CLIENT_MAX; ++ID)
  {
    if( server->client[ID].myID != CLIENT_DISABLE_ID )
    {
      u32 returnDataSize, numAction;
      const BTL_ACTION_PARAM* action = BTL_ADAPTER_GetReturnData( server->client[ID].adapter, &returnDataSize );

      numAction = returnDataSize / sizeof(BTL_ACTION_PARAM);
      BTL_RECTOOL_PutSelActionData( &server->recTool, ID, action, numAction );
    }
  }

  return BTL_RECTOOL_FixSelActionData( &server->recTool, dataSize );
}
/**
 * ���[�e�[�V�����L�^�f�[�^�𐶐�
 *
 * @param   server
 * @param   dataSize   [out] �������ꂽ�f�[�^�T�C�Y
 *
 * @retval  void*   �����������ł����瑗�M�f�[�^�|�C���^ / �ł��Ȃ��ꍇNULL
 */
static void* MakeRotationRecord( BTL_SERVER* server, u32* dataSize, const BtlRotateDir* dirAry )
{
  u32 ID;

  BTL_RECTOOL_Init( &server->recTool );

  for(ID=0; ID<BTL_CLIENT_MAX; ++ID)
  {
    if( server->client[ID].myID != CLIENT_DISABLE_ID )
    {
      BTL_RECTOOL_PutRotationData( &server->recTool, ID, dirAry[ID] );
    }
  }

  return BTL_RECTOOL_FixRotationData( &server->recTool, dataSize );
}


//=================================================================================================
//=================================================================================================

//----------------------------------------------------------------------------------
/**
 * �S�N���C�A���g�փR�}���h���N�G�X�g
 *
 * @param   server
 * @param   cmd
 */
//----------------------------------------------------------------------------------
static void SetAdapterCmd( BTL_SERVER* server, BtlAdapterCmd cmd )
{
  SetAdapterCmdEx( server, cmd, NULL, 0 );
}
//----------------------------------------------------------------------------------
/**
 * �S�N���C�A���g�փR�}���h���N�G�X�g�i�g����񂠂�j
 *
 * @param   server
 * @param   cmd
 * @param   sendData
 * @param   dataSize
 */
//----------------------------------------------------------------------------------
static void SetAdapterCmdEx( BTL_SERVER* server, BtlAdapterCmd cmd, const void* sendData, u32 dataSize )
{
  int i;

  BTL_ADAPTERSYS_BeginSetCmd();

  for(i=0; i<BTL_CLIENT_MAX; i++)
  {
    if( Svcl_IsEnable(&server->client[i]) ){
      BTL_ADAPTER_SetCmd( server->client[i].adapter, cmd, sendData, dataSize );
    }
  }

  BTL_ADAPTERSYS_EndSetCmd();
}
//----------------------------------------------------------------------------------
/**
 * �w��N���C�A���g�ւ̂݃R�}���h���N�G�X�g
 *
 * @param   server
 * @param   cmd
 * @param   clientID
 */
//----------------------------------------------------------------------------------
static void SetAdapterCmdSingle( BTL_SERVER* server, BtlAdapterCmd cmd, u8 clientID, const void* sendData, u32 dataSize )
{
  SVCL_WORK* svcl = &server->client[ clientID ];

  if( Svcl_IsEnable(svcl) ){
    BTL_ADAPTER_ResetCmd( svcl->adapter );
    BTL_ADAPTER_SetCmd( svcl->adapter, cmd, sendData, dataSize );
  }
  else{
    GF_ASSERT(0);
  }
}
//----------------------------------------------------------------------------------
/**
 * �ڑ����Ă���S�N���C�A���g����̕ԐM��҂�
 *
 * @param   server
 *
 * @retval  BOOL    �S�N���C�A���g����̕ԐM����������TRUE
 */
//----------------------------------------------------------------------------------
static BOOL WaitAllAdapterReply( BTL_SERVER* server )
{
  int i;
  BOOL ret = TRUE;

  for(i=0; i<BTL_CLIENT_MAX; i++)
  {
    if( Svcl_IsEnable(&server->client[i]) )
    {
      if( !BTL_ADAPTER_WaitCmd( server->client[i].adapter ) ){
        ret = FALSE;
      }
    }
  }

  return ret;
}
//----------------------------------------------------------------------------------
/**
 * �R�}���h���N�G�X�g�������Z�b�g
 *
 * @param   server
 */
//----------------------------------------------------------------------------------
static void ResetAdapterCmd( BTL_SERVER* server )
{
  int i;
  for(i=0; i<BTL_CLIENT_MAX; i++)
  {
    if( Svcl_IsEnable(&server->client[i]) ){
      BTL_ADAPTER_ResetCmd( server->client[i].adapter );
    }
  }
}


//=================================================================================================
//=================================================================================================

//----------------------------------------------------------------------------------
/**
 * �T�[�o�ێ��p�N���C�A���g���[�N�F������
 *
 * @param   clwk
 */
//----------------------------------------------------------------------------------
static void Svcl_Clear( SVCL_WORK* clwk )
{
  clwk->myID = CLIENT_DISABLE_ID;
}
//----------------------------------------------------------------------------------
/**
 * �T�[�o�ێ��p�N���C�A���g���[�N�F�L������
 *
 * @param   clwk
 */
//----------------------------------------------------------------------------------
static BOOL Svcl_IsEnable( const SVCL_WORK* clwk )
{
  return clwk->myID != CLIENT_DISABLE_ID;
}
//----------------------------------------------------------------------------------
/**
 * �T�[�o�ێ��p�N���C�A���g���[�N�F��{�ݒ�
 *
 * @param   clwk
 * @param   adapter
 * @param   fLocalClient
 */
//----------------------------------------------------------------------------------
static void Svcl_Setup( SVCL_WORK* clwk, u8 clientID, BTL_ADAPTER* adapter, BOOL fLocalClient )
{
  clwk->myID = clientID;
  clwk->adapter = adapter;
  clwk->isLocalClient = fLocalClient;
}
//--------------------------------------------------------------------------
/**
 * �T�[�o�ێ��p�N���C�A���g���[�N�F�p�[�e�B�ݒ�
 *
 * @param   client        �N���C�A���g���[�N
 * @param   party         �p�[�e�B�f�[�^
 * @param   numCoverPos   �N���C�A���g���󂯎��퓬���|�P������
 */
//--------------------------------------------------------------------------
static void Svcl_SetParty( SVCL_WORK* client, BTL_PARTY* party, u8 numCoverPos )
{
  u16 i;

  client->party = party;
  client->memberCount = BTL_PARTY_GetMemberCount( party );
  client->numCoverPos = numCoverPos;
}

//--------------------------------------------------------------------------------------
/**
 * �T�[�o�ێ��p�N���C�A���g���[�N�ւ̃|�C���^��Ԃ��i���݂��Ȃ���ASSERT���s�j
 *
 * @param   server
 * @param   clientID
 *
 * @retval  BTL_SERVER_CLWORK*
 */
//--------------------------------------------------------------------------------------
SVCL_WORK* BTL_SERVER_GetClientWork( BTL_SERVER* server, u8 clientID )
{
  if( (clientID < NELEMS(server->client))
  &&  (server->client[clientID].myID != CLIENT_DISABLE_ID)
  ){
    return &server->client[ clientID ];
  }
  return NULL;
}
//--------------------------------------------------------------------------------------
/**
 * �T�[�o�ێ��p�N���C�A���g���[�N�ւ̃|�C���^��Ԃ��i���݂��Ȃ����NULL��Ԃ��j
 *
 * @param   server
 * @param   clientID
 *
 * @retval  SVCL_WORK*
 */
//--------------------------------------------------------------------------------------
SVCL_WORK* BTL_SERVER_GetClientWorkIfEnable( BTL_SERVER* server, u8 clientID )
{
  if( server->client[clientID].myID != CLIENT_DISABLE_ID )
  {
    return &(server->client[clientID]);
  }
  return NULL;
}
//--------------------------------------------------------------------------------------
/**
 * �w��ID�̃N���C�A���g�����݂��邩����
 *
 * @param   server
 * @param   clientID
 *
 * @retval  BOOL    ���݂����TRUE
 */
//--------------------------------------------------------------------------------------
BOOL BTL_SERVER_IsClientEnable( const BTL_SERVER* server, u8 clientID )
{
  if( clientID < BTL_CLIENT_MAX ){
    return Svcl_IsEnable( &server->client[ clientID ] );
  }
  return FALSE;
}





u8 BTL_SVCL_GetNumActPoke( SVCL_WORK* clwk )
{
  GF_ASSERT(clwk->adapter);
  {
    u32 dataSize = BTL_ADAPTER_GetReturnDataSize( clwk->adapter );
    GF_ASSERT((dataSize%sizeof(BTL_ACTION_PARAM))==0);
    return dataSize / sizeof(BTL_ACTION_PARAM);
  }
}
const BTL_ACTION_PARAM* BTL_SVCL_GetPokeAction( SVCL_WORK* clwk, u8 posIdx )
{
  GF_ASSERT(clwk->adapter);
  GF_ASSERT_MSG(posIdx<clwk->numCoverPos, "posIdx=%d, numCoverPos=%d", posIdx, clwk->numCoverPos);

  {
    const BTL_ACTION_PARAM* action = BTL_ADAPTER_GetReturnData( clwk->adapter, NULL );
    action += posIdx;
    return action;
  }
}



//----------------------------------------------
void BTL_SERVER_NotifyPokemonLevelUp( BTL_SERVER* server, const BTL_POKEPARAM* bpp )
{
  const POKEMON_PARAM* pp;
  pp = BPP_GetSrcData( bpp );
  PP_Get( pp, ID_PARA_nickname, server->strbuf );
  GAMEBEACON_Set_PokemonLevelUp( server->strbuf );
}
void BTL_SERVER_NotifyPokemonCapture( BTL_SERVER* server, const BTL_POKEPARAM* bpp )
{
  const POKEMON_PARAM* pp;
  pp = BPP_GetSrcData( bpp );
  GAMEBEACON_Set_PokemonGet( PP_Get( pp, ID_PARA_monsno, NULL ) );
}

void BTL_SERVER_AddBonusMoney( BTL_SERVER* server, u32 volume )
{
  BTL_MAIN_AddBonusMoney( server->mainModule, volume );
}

void BTL_SERVER_InitChangePokemonReq( BTL_SERVER* server )
{
  server->changePokeCnt = 0;
  server->giveupClientCnt = 0;
}

void BTL_SERVER_RequestChangePokemon( BTL_SERVER* server, BtlPokePos pos )
{
  GF_ASSERT(server->changePokeCnt < BTL_POS_MAX);
  {
    u32 i;
    for(i=0; i<server->changePokeCnt; ++i){
      if( server->changePokePos[i] == pos ){
        return;
      }
    }
    server->changePokePos[ server->changePokeCnt++ ] = pos;
  }
}

