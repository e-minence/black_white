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
#include "handler\hand_tokusei.h"

#include "btl_server_cmd.h"
#include "btl_field.h"
#include "btl_server_flow.h"
#include "btl_server_local.h"

#include "btl_server.h"

#define SOGA_DEBUG  //�b���soga���f�o�b�O�����ӏ�

/*--------------------------------------------------------------------------*/
/* Consts                                                                   */
/*--------------------------------------------------------------------------*/
enum {
  SERVER_CMD_SIZE_MAX = 1024,
  CLIENT_DISABLE_ID = 0xff,
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
  u8          numClient;
  u8          quitStep;
  u32         escapeClientID;


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
static inline void setup_client_members( SVCL_WORK* client, BTL_PARTY* party, u8 numCoverPos );
static void setMainProc( BTL_SERVER* sv, ServerMainProc mainProc );
static BOOL ServerMain_WaitReady( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectRotation( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectAction( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectPokemonIn( BTL_SERVER* server, int* seq );
static BOOL ServerMain_SelectPokemonChange( BTL_SERVER* server, int* seq );
static BOOL ServerMain_ExitBattle( BTL_SERVER* server, int* seq );
static BOOL checkBattleShowdown( BTL_SERVER* server );
static void SetAdapterCmd( BTL_SERVER* server, BtlAdapterCmd cmd );
static void SetAdapterCmdEx( BTL_SERVER* server, BtlAdapterCmd cmd, const void* sendData, u32 dataSize );
static BOOL WaitAdapterCmd( BTL_SERVER* server );
static void ResetAdapterCmd( BTL_SERVER* server );





//=============================================================================================
/**
 * �T�[�o����
 *
 * @param   mainModule  ���C�����W���[���̃n���h��
 * @param   pokeCon     �|�P�����f�[�^�R���e�i�n���h��
 * @param   bagMode     �o�b�O���[�h
 * @param   heapID      �q�[�vID
 *
 * @retval  BTL_SERVER*
 */
//=============================================================================================
BTL_SERVER* BTL_SERVER_Create( BTL_MAIN_MODULE* mainModule, BTL_POKE_CONTAINER* pokeCon, BtlBagMode bagMode, HEAPID heapID )
{
  BTL_SERVER* sv = GFL_HEAP_AllocClearMemory( heapID, sizeof(BTL_SERVER) );

  sv->mainModule = mainModule;
  sv->pokeCon = pokeCon;
  sv->numClient = 0;
  sv->heapID = heapID;
  sv->que = &sv->queBody;
  sv->quitStep = QUITSTEP_NONE;
  sv->flowWork = NULL;
  sv->changePokeCnt = 0;
  sv->giveupClientCnt = 0;
  sv->bagMode = bagMode;

  {
    int i;
    for(i=0; i<BTL_CLIENT_MAX; ++i)
    {
      sv->client[i].myID = CLIENT_DISABLE_ID;
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

    client = &server->client[ clientID ];
    party = BTL_POKECON_GetPartyData( server->pokeCon, clientID );

    client->adapter = adapter;
    client->myID = clientID;
    client->isLocalClient = TRUE;

    setup_client_members( client, party, numCoverPos );
  }

  server->numClient++;
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

    client = &server->client[ clientID ];
    party = BTL_POKECON_GetPartyData( server->pokeCon, clientID );

    client->adapter = BTL_ADAPTER_Create( netHandle, server->heapID, clientID );
    client->myID = clientID;
    client->isLocalClient = FALSE;

    setup_client_members( client, party, numCoverPos );
  }

  server->numClient++;
}
//--------------------------------------------------------------------------
/**
 * �N���C�A���g���[�N�̃����o�[�f�[�^����������
 *
 * @param   client        �N���C�A���g���[�N
 * @param   party         �p�[�e�B�f�[�^
 * @param   numCoverPos   �N���C�A���g���󂯎��퓬���|�P������
 */
//--------------------------------------------------------------------------
static inline void setup_client_members( SVCL_WORK* client, BTL_PARTY* party, u8 numCoverPos )
{
  u16 i;

  client->party = party;
  client->memberCount = BTL_PARTY_GetMemberCount( party );
  client->numCoverPos = numCoverPos;

  for(i=0; i<client->memberCount; i++)
  {
    client->member[i] = BTL_PARTY_GetMemberData( client->party, i );
  }
  for( ; i<NELEMS(client->member); i++)
  {
    client->member[i] = NULL;
  }

  {
    u16 numFrontPoke = (numCoverPos < client->memberCount)? numCoverPos : client->memberCount;
    for(i=0; i<numFrontPoke; i++)
    {
//      client->frontMember[i] = client->member[i];
    }
    for( ; i<NELEMS(client->frontMember); i++)
    {
      client->frontMember[i] = NULL;
    }
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
        &server->queBody, server->numClient, server->bagMode, server->heapID );
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
void BTL_SERVER_Main( BTL_SERVER* sv )
{
  if( sv->quitStep != QUITSTEP_CORE )
  {
    if( sv->mainProc( sv, &sv->seq ) )
    {
      sv->escapeClientID = BTL_SVFLOW_GetEscapeClientID( sv->flowWork );
      BTL_Printf( "�����N���C�A���gID=%d\n", sv->escapeClientID );
      SetAdapterCmdEx( sv, BTL_ACMD_QUIT_BTL, &sv->escapeClientID, sizeof(sv->escapeClientID) );
      sv->quitStep = QUITSTEP_CORE;
    }
  }
  else
  {
    WaitAdapterCmd( sv );
  }
}

static void setMainProc( BTL_SERVER* sv, ServerMainProc mainProc )
{
  sv->mainProc = mainProc;
  sv->seq = 0;
}


//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

static BOOL ServerMain_WaitReady( BTL_SERVER* server, int* seq )
{
  switch( *seq ){
  case 0:
    BTL_EVENT_InitSystem();
    BTL_Printf("�o�g���Z�b�g�A�b�v�R�}���h���s\n");
    SetAdapterCmd( server, BTL_ACMD_WAIT_SETUP );
    (*seq)++;
    /* fallthru */
  case 1:
    if( WaitAdapterCmd(server) )
    {
      ResetAdapterCmd( server );
      BTL_SVFLOW_Start_AfterPokemonIn( server->flowWork );
      if( server->que->writePtr )
      {
        BTL_Printf("�S�|�P�o���A�ŏ��̍Đ��R�}���h���s\n");
        SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
        (*seq)++;
      }
      else
      {
        (*seq)+=2;
      }
    }
    break;
  case 2:
    if( WaitAdapterCmd(server) )
    {
      ResetAdapterCmd( server );
      (*seq)++;
    }
    break;
  case 3:
    if( BTL_MAIN_GetRule(server->mainModule) != BTL_RULE_ROTATION ){
      setMainProc( server, ServerMain_SelectAction );
    }else{
      setMainProc( server, ServerMain_SelectRotation );
    }
    break;
  }
  return FALSE;
}
/**
 *  ���[�e�[�V�����I��
 */
static BOOL ServerMain_SelectRotation( BTL_SERVER* server, int* seq )
{
  switch( *seq ){
  case 0:
    if( BTL_SVFLOW_GetTurnCount(server->flowWork) == 0 ){
      setMainProc( server, ServerMain_SelectAction );
    }else{
      SetAdapterCmd( server, BTL_ACMD_SELECT_ROTATION );
      (*seq)++;
    }
    break;

  case 1:
    if( WaitAdapterCmd(server) )
    {
      const BtlRotateDir  *dir;
      BTL_PARTY* party;
      u32 i;

      ResetAdapterCmd( server );
      SCQUE_Init( server->que );
      for(i=0; i<server->numClient; ++i)
      {
        dir = BTL_ADAPTER_GetReturnData( server->client[i].adapter );
        SCQUE_PUT_ACT_Rotation( server->que, i, *dir );
        party = BTL_POKECON_GetPartyData( server->pokeCon, i );
        BTL_PARTY_RotateMembers( party, *dir );
      }
      SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
      (*seq)++;
    }
    break;

  case 2:
    if( WaitAdapterCmd(server) )
    {
      ResetAdapterCmd( server );
      return TRUE;
    }
    break;
  }
  return FALSE;
}


static BOOL ServerMain_SelectAction( BTL_SERVER* server, int* seq )
{
  switch( *seq ){
  case 0:
    BTL_Printf("�A�N�V�����I���R�}���h���s\n");
    SetAdapterCmd( server, BTL_ACMD_SELECT_ACTION );
    (*seq)++;
    break;

  case 1:
    if( WaitAdapterCmd(server) )
    {
      BTL_Printf("�A�N�V�����󂯕t������\n");
      ResetAdapterCmd( server );
      server->flowResult = BTL_SVFLOW_Start( server->flowWork );
      BTL_Printf("�T�[�o�R�}���h��������,���M���܂� ... result=%d\n", server->flowResult);
      SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
      (*seq)++;
    }
    break;

  case 2:
    if( WaitAdapterCmd(server) )
    {
      BTL_Printf("�S�N���C�A���g�̃R�}���h�Đ������܂���...result=%d\n", server->flowResult);
      BTL_MAIN_SyncServerCalcData( server->mainModule );
      ResetAdapterCmd( server );

      switch( server->flowResult ){
      case SVFLOW_RESULT_DEFAULT:
        (*seq)=0;
        break;
      case SVFLOW_RESULT_POKE_IN_REQ:
        BTL_Printf("�󂫈ʒu�ւ̐V�|�P�������N�G�X�g�󂯕t��\n");
        setMainProc( server, ServerMain_SelectPokemonIn );
        break;
      case SVFLOW_RESULT_POKE_CHANGE:
        BTL_Printf("����ւ����N�G�X�g�󂯕t��\n");
        GF_ASSERT( server->changePokeCnt );
        BTL_Printf("�^�[���r���̃|�P����ւ�\n");
        setMainProc( server, ServerMain_SelectPokemonChange );
        break;
      case SVFLOW_RESULT_POKE_GET:
        {
          BtlPokePos pos = BTL_SVFLOW_GetCapturedPokePos( server->flowWork );
          BTL_Printf("�|�P�߂܂��܂��� pos=%d\n", pos);
          BTL_MAIN_NotifyCapturedPokePos( server->mainModule, pos );
        }
        return TRUE;
      default:
        GF_ASSERT(0);
        /* fallthru */
      case SVFLOW_RESULT_BTL_QUIT:
        BTL_Printf("�o�g���I����\n");
        return TRUE;
      }
    }
    break;
  }

  return FALSE;
}

//----------------------------------------------------------------------------------
/**
 * �T�[�o���C�����[�v�F���S�E�����Ԃ�ŋ󂫈ʒu�Ƀ|�P�����𓊓���������̏���
 *
 * @param   server
 * @param   seq
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL ServerMain_SelectPokemonIn( BTL_SERVER* server, int* seq )
{
  switch( *seq ){
  case 0:
    BTL_Printf("����|�P�����I����  ��ւ����|�P��=%d\n", server->changePokeCnt);
    SetAdapterCmdEx( server, BTL_ACMD_SELECT_POKEMON, server->changePokePos,
        server->changePokeCnt*sizeof(server->changePokePos[0]) );
    (*seq)++;
    break;

  case 1:
    if( WaitAdapterCmd(server) )
    {
      BTL_Printf("����|�P�����I�����I�����\n");
      ResetAdapterCmd( server );
      if( checkBattleShowdown(server) ){
        BTL_Printf("�����������I\n");
        setMainProc( server, ServerMain_ExitBattle );
        return FALSE;
      }
      else{
        SCQUE_Init( server->que );
        server->flowResult = BTL_SVFLOW_StartAfterPokeIn( server->flowWork );
        BTL_Printf("�T�[�o�[��������=%d\n", server->flowResult );
        SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
        (*seq)++;
      }
    }
    break;

  case 2:
    if( WaitAdapterCmd(server) )
    {
      ResetAdapterCmd( server );
      BTL_MAIN_SyncServerCalcData( server->mainModule );

      if( server->flowResult != SVFLOW_RESULT_POKE_IN_REQ )
      {
        setMainProc( server, ServerMain_SelectAction );
      }
      else
      {
        (*seq) = 0;
      }
    }
    break;
  }

  return FALSE;
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
    BTL_Printf("����ւ��|�P�����I����  ��ւ����|�P��=%d\n", server->changePokeCnt);
    SetAdapterCmdEx( server, BTL_ACMD_SELECT_POKEMON, server->changePokePos,
        server->changePokeCnt*sizeof(server->changePokePos[0]) );
    (*seq)++;
    break;

  case 1:
    if( WaitAdapterCmd(server) )
    {
      BTL_Printf("����ւ��|�P�����I�����I�����\n");
      ResetAdapterCmd( server );
      SCQUE_Init( server->que );
      server->flowResult = BTL_SVFLOW_StartAfterPokeChange( server->flowWork );
      BTL_Printf("�T�[�o�[��������=%d\n", server->flowResult );
      SetAdapterCmdEx( server, BTL_ACMD_SERVER_CMD, server->que->buffer, server->que->writePtr );
      (*seq)++;
    }
    break;

  case 2:
    if( WaitAdapterCmd(server) )
    {
      ResetAdapterCmd( server );
      BTL_MAIN_SyncServerCalcData( server->mainModule );

      if( server->flowResult != SVFLOW_RESULT_POKE_IN_REQ )
      {
        setMainProc( server, ServerMain_SelectAction );
      }
      else
      {
        setMainProc( server, ServerMain_SelectPokemonIn );
      }
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
    PMSND_PlayBGM( SEQ_BGM_WIN1 );
    (*seq)++;
    break;
  case 1:
    {
      u8 touch = ( (GFL_UI_KEY_GetTrg() & PAD_BUTTON_A) != 0);
      u8 bgm_end = !PMSND_CheckPlayBGM();
      if( touch || bgm_end ){
        return TRUE;
      }
    }
    break;
  }
  return FALSE;
}


//----------------------------------------------------------------------------------
/**
 * �퓬�̌�����������Ԃ�����
 *
 * @param   server
 *
 * @retval  BOOL
 */
//----------------------------------------------------------------------------------
static BOOL checkBattleShowdown( BTL_SERVER* server )
{
  u8  pokeExist[2];
  u32 i;

  pokeExist[0] = pokeExist[1] = FALSE;

  for(i=0; i<BTL_CLIENT_MAX; ++i)
  {
    if( BTL_POKECON_IsExsitClient(server->pokeCon, i) )
    {
      BTL_PARTY* party = BTL_POKECON_GetPartyData( server->pokeCon, i );
      u8 side = BTL_MAIN_GetClientSide( server->mainModule, i );
      if( BTL_PARTY_GetAliveMemberCount(party) )
      {
        BTL_Printf("�N���C�A���g_%d (SIDE:%d) �̃|�P�͂܂����̂������Ă���\n", i, side);
        pokeExist[ side] = TRUE;
      }
      else{
        BTL_Printf("�N���C�A���g_%d (SIDE:%d) �̃|�P�͑S�ł���\n", i, side);
      }
    }
  }

  if( (pokeExist[0] == FALSE) || (pokeExist[1] == FALSE) ){
    return TRUE;
  }
  return FALSE;
}



//----------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------

static void SetAdapterCmd( BTL_SERVER* server, BtlAdapterCmd cmd )
{
  SetAdapterCmdEx( server, cmd, NULL, 0 );
}

static void SetAdapterCmdEx( BTL_SERVER* server, BtlAdapterCmd cmd, const void* sendData, u32 dataSize )
{
  int i;

  BTL_ADAPTERSYS_BeginSetCmd();

  for(i=0; i<server->numClient; i++)
  {
    BTL_ADAPTER_SetCmd( server->client[i].adapter, cmd, sendData, dataSize );
  }

  BTL_ADAPTERSYS_EndSetCmd();
}

static BOOL WaitAdapterCmd( BTL_SERVER* server )
{
  int i;
  BOOL ret = TRUE;
  for(i=0; i<server->numClient; i++)
  {
    if( !BTL_ADAPTER_WaitCmd( server->client[i].adapter ) )
    {
      ret = FALSE;
    }
  }
  return ret;
}

static void ResetAdapterCmd( BTL_SERVER* server )
{
  int i;
  for(i=0; i<server->numClient; i++)
  {
    BTL_ADAPTER_ResetCmd( server->client[i].adapter );
  }
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
u8 BTL_SVCL_GetNumActPoke( SVCL_WORK* clwk )
{
  GF_ASSERT(clwk->adapter);
  return BTL_ADAPTER_GetReturnDataCount( clwk->adapter );
}
const BTL_ACTION_PARAM* BTL_SVCL_GetPokeAction( SVCL_WORK* clwk, u8 posIdx )
{
  GF_ASSERT(clwk->adapter);
  GF_ASSERT_MSG(posIdx<clwk->numCoverPos, "posIdx=%d, numCoverPos=%d", posIdx, clwk->numCoverPos);

  {
    const BTL_ACTION_PARAM* action = BTL_ADAPTER_GetReturnData( clwk->adapter );
    action += posIdx;
    return action;
  }
}



//----------------------------------------------

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

void BTL_SERVER_NotifyGiveupClientID( BTL_SERVER* server, u8 clientID )
{
  GF_ASSERT(server->giveupClientCnt < BTL_CLIENT_MAX);
  {
    server->giveupClientID[ server->giveupClientCnt++ ] = clientID;
  }
}

