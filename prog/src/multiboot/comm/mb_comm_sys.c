//======================================================================
/**
 * @file  mb_comm_sys.c
 * @brief �}���`�u�[�g �ʐM�V�X�e��
 * @author  ariizumi
 * @data  09/11/13
 *
 * ���W���[�����FMB_COMM
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "net/network_define.h"

#include "multiboot/mb_comm_sys.h"
#include "multiboot/mb_local_def.h"

//======================================================================
//  define
//======================================================================
#pragma mark [> define

#if 1 //�f�o�b�O�o�͐؂�ւ�

#ifndef MULTI_BOOT_MAKE
#include "test/ariizumi/ari_debug.h"
#define MB_COMM_TPrintf(...) (void)((ARI_TPrintf(__VA_ARGS__)))
#else
#define MB_COMM_TPrintf(...) (void)((OS_TPrintf(__VA_ARGS__)))
#endif //MULTI_BOOT_MAKE
#else

#define MB_COMM_TPrintf(...) (void)((OS_TPrintf(__VA_ARGS__)))
#endif//�f�o�b�O�o�͐؂�ւ�

#define MB_COMM_TICK_TEST

#ifdef MB_COMM_TICK_TEST
static OSTick gameDataAllTick = 0;
#endif

#define MB_COMM_GAMEDATA_SEND_SIZE (0x7000)
//======================================================================
//  enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MCS_NONE,
  MCS_REQ_NEGOTIATION,
  MCS_WAIT_NEGOTIATION,
  MCS_WAIT_CAN_SEND,
  
  //�R�R���牺�͂Ȃ����Ă����Ԃ�STATE�Ƃ���
  MCS_CONNECT,
  
  //�e�@�f�[�^���M
  MCS_SEND_GAMEDATA_INIT,
  MCS_SEND_GAMEDATA_SEND,
  MCS_SEND_GAMEDATA_WAIT,
  
  //�q�@�f�[�^���M
  MCS_POST_GAMEDATA_WAIT,

  MCS_MAX,
}MB_COMM_STATE;

//���M�̎��
typedef enum
{
  MCST_FLG = GFL_NET_CMD_MULTIBOOT,
  MCST_INIT_DATA,
  MCST_GAME_DATA,
  
  MCST_MAX,
}MB_COMM_SEND_TYPE;

//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
struct _MB_COMM_WORK
{
  HEAPID heapId;
  
  MB_COMM_STATE state;
  
  //GAMEDATA�n
  void *gameData;
  u32  gameDataSize;
  u8   dataIdx;
  
  //�q�@�̂�
  MB_COMM_INIT_DATA *initData;
  
  //�m�F�t���O�n
  BOOL    isPostInitData;
  BOOL    isPostGameData;
  BOOL    isPostGameDataSize;
};


//======================================================================
//  proto
//======================================================================
#pragma mark [> proto

static void*  MB_COMM_GetBeaconDataDummy(void* pWork);
static int    MB_COMM_GetBeaconSizeDummy(void* pWork);

static void MB_COMM_Post_Flag( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
static void MB_COMM_Post_InitData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
static u8*    MUS_COMM_Post_InitDataBuff( int netID, void* pWork , int size );
static void MB_COMM_Post_GameData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
static u8*    MUS_COMM_Post_GameDataBuff( int netID, void* pWork , int size );

static const NetRecvFuncTable MultiBootCommPostTable[] = {
  { MB_COMM_Post_Flag     , NULL },
  { MB_COMM_Post_InitData , MUS_COMM_Post_InitDataBuff },
  { MB_COMM_Post_GameData , MUS_COMM_Post_GameDataBuff },
};

//--------------------------------------------------------------
//  ���[�N�̍쐬
//--------------------------------------------------------------
MB_COMM_WORK* MB_COMM_CreateSystem( const HEAPID heapId )
{
  MB_COMM_WORK* commWork = GFL_HEAP_AllocClearMemory( heapId , sizeof( MB_COMM_WORK ) );
  
  commWork->heapId = heapId;
  commWork->state = MCS_NONE;
  commWork->initData = NULL;
  
  return commWork;
}

//--------------------------------------------------------------
//  ���[�N�̊J��
//--------------------------------------------------------------
void MB_COMM_DeleteSystem( MB_COMM_WORK* commWork )
{
  if( commWork->initData != NULL )
  {
    GFL_HEAP_FreeMemory( commWork->initData );
  }
  GFL_HEAP_FreeMemory( commWork );
}

//--------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------
void MB_COMM_UpdateSystem( MB_COMM_WORK* commWork )
{
  switch( commWork->state )
  {
  case MCS_NONE:
    break;
  case MCS_REQ_NEGOTIATION:
    if( GFL_NET_HANDLE_RequestNegotiation() == TRUE )
    {
      commWork->state = MCS_WAIT_NEGOTIATION;
    }
    break;
  case MCS_WAIT_NEGOTIATION:
    {
      GFL_NETHANDLE *handle = GFL_NET_HANDLE_GetCurrentHandle();
      if( GFL_NET_HANDLE_IsNegotiation( handle ) == TRUE )
      {
        commWork->state = MCS_WAIT_CAN_SEND;
      }
    }
    break;
  case MCS_WAIT_CAN_SEND:
    {
      GFL_NETHANDLE *handle = GFL_NET_HANDLE_GetCurrentHandle();
      if( GFL_NET_IsSendEnable( handle ) == TRUE )
      {
        commWork->state = MCS_CONNECT;
      }
    }
    break;
  case MCS_CONNECT:
    //���̏�Ԃ�MB_COMM_IsSendEnable�̃`�F�b�N�Ɏg���Ă�B
    break;
    
  //�e�@�f�[�^���M
  case MCS_SEND_GAMEDATA_INIT:
    if( MB_COMM_Send_Flag( commWork , MCFT_GAMEDATA_SIZE , commWork->gameDataSize ) == TRUE )
    {
      commWork->state = MCS_SEND_GAMEDATA_SEND;
    }
    break;
    
  case MCS_SEND_GAMEDATA_SEND:
    if( commWork->isPostGameDataSize == TRUE )
    {
      u32 size;
      u32 topAdd = (u32)commWork->gameData + MB_COMM_GAMEDATA_SEND_SIZE * commWork->dataIdx;
      if( commWork->gameDataSize < MB_COMM_GAMEDATA_SEND_SIZE * (commWork->dataIdx+1) ) 
      {
        size = commWork->gameDataSize % MB_COMM_GAMEDATA_SEND_SIZE;
      }
      else
      {
        size = MB_COMM_GAMEDATA_SEND_SIZE;
      }
      
      if( MB_COMM_Send_GameData( commWork , (void*)topAdd , size ) == TRUE )
      {
        commWork->state = MCS_SEND_GAMEDATA_WAIT;
      }
    }
    break;

  case MCS_SEND_GAMEDATA_WAIT:
    break;
    
  //�q�@�f�[�^���M
  case MCS_POST_GAMEDATA_WAIT:
    if( commWork->isPostGameData == TRUE )
    {
      if( MB_COMM_Send_Flag( commWork , MCFT_GAMEDATA_POST , commWork->dataIdx ) == TRUE )
      {
        commWork->isPostGameData = FALSE;
        commWork->dataIdx++;
        if( commWork->gameDataSize <= commWork->dataIdx * MB_COMM_GAMEDATA_SEND_SIZE )
        {
          commWork->state = MCS_CONNECT;
#ifdef MB_COMM_TICK_TEST
          MB_COMM_TPrintf("FinishGameDataPost[%dms]\n",OS_TicksToMilliSeconds(OS_GetTick()-gameDataAllTick));
#endif
        }
      }
    }
    break;
  }
}

//--------------------------------------------------------------
//  �ʐM�̊J�n
//--------------------------------------------------------------
void MB_COMM_InitComm( MB_COMM_WORK* commWork )
{
  GFLNetInitializeStruct aGFLNetInit = {
    MultiBootCommPostTable, //NetSamplePacketTbl,  // ��M�֐��e�[�u��
    NELEMS(MultiBootCommPostTable), // ��M�e�[�u���v�f��
    NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
    NULL,    ///< �l�S�V�G�[�V�����������ɃR�[��
    NULL, // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    NULL, // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    MB_COMM_GetBeaconDataDummy,  // �r�[�R���f�[�^�擾�֐�  
    MB_COMM_GetBeaconSizeDummy,  // �r�[�R���f�[�^�T�C�Y�擾�֐� 
    NULL, // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL, // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� 
    NULL,  //Fatal�G���[���N�������ꍇ
    NULL,  // �ʐM�ؒf���ɌĂ΂��֐�
    NULL, // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,   ///< DWC�`���̗F�B���X�g  
    NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,   ///< DWC�ւ�HEAP�T�C�Y
    TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
    MB_DEF_GGID,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    0,  //���ɂȂ�heapid
    HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,  //wifi�p��create�����HEAPID
    HEAPID_WIFI,  //�ԊO���p��create�����HEAPID
    GFL_WICON_POSX,GFL_WICON_POSY,  // �ʐM�A�C�R��XY�ʒu
    2,//_MAXNUM,  //�ő�ڑ��l��
    48,//_MAXSIZE,  //�ő呗�M�o�C�g��
    2,//_BCON_GET_NUM,  // �ő�r�[�R�����W��
    TRUE,   // CRC�v�Z
    TRUE,    // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_WIRELESS,    //  NET�ʐM�^�C�v �� wifi�ʐM���s�����ǂ���
    FALSE,    // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_MULTIBOOT_SERVICEID,//GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD, // �ԊO���^�C���A�E�g����
#endif
    512 - 24*2,//MP�e�ő�T�C�Y 512�܂�
    0,//dummy
  };

  aGFLNetInit.baseHeapID = commWork->heapId;
  GFL_NET_Init( &aGFLNetInit , NULL , (void*)commWork );
  
  commWork->isPostInitData = FALSE;
  commWork->isPostGameData = FALSE;
  commWork->isPostGameDataSize = FALSE;
  commWork->gameData = NULL;
  commWork->dataIdx = 0;
}

//--------------------------------------------------------------
// �ʐM�I��
//--------------------------------------------------------------
void MB_COMM_ExitComm( MB_COMM_WORK* commWork )
{
  GFL_NET_Exit(NULL);
}

//--------------------------------------------------------------
// �����������`�F�b�N
//--------------------------------------------------------------
const BOOL MB_COMM_IsInitComm( MB_COMM_WORK* commWork )
{
  return GFL_NET_IsInit();
}

//--------------------------------------------------------------
// �I�����������`�F�b�N
//--------------------------------------------------------------
const BOOL MB_COMM_IsFinishComm( MB_COMM_WORK* commWork )
{
  return GFL_NET_IsExit();
}

//--------------------------------------------------------------
// �e�@�ʐM�J�n
//--------------------------------------------------------------
void MB_COMM_InitParent( MB_COMM_WORK* commWork )
{
  GFL_NET_InitServer();
  commWork->state = MCS_REQ_NEGOTIATION;
}

//--------------------------------------------------------------
// �q�@�ʐM�J�n
//--------------------------------------------------------------
void MB_COMM_InitChild( MB_COMM_WORK* commWork , u8 *macAddress )
{
  GFL_NET_InitClientAndConnectToParent( macAddress );
  commWork->state = MCS_REQ_NEGOTIATION;
}

#pragma mark [>Afetr Connect
MB_COMM_INIT_DATA* MB_COMM_GetInitData( MB_COMM_WORK* commWork )
{
  return commWork->initData;
}

#pragma mark [>Check func
const BOOL MB_COMM_IsSendEnable( const MB_COMM_WORK* commWork )
{
  if( commWork->state >= MCS_CONNECT )
  {
    return TRUE;
  }
  return FALSE;
}

const BOOL MB_COMM_IsPostInitData( const MB_COMM_WORK* commWork )
{
  return commWork->isPostInitData;
}
#pragma mark [>SendDataFunc
//--------------------------------------------------------------
// �ł����T�C�Y���M�p
//--------------------------------------------------------------
void MB_COMM_InitSendGameData( MB_COMM_WORK* commWork , void* gameData , u32 size )
{
  commWork->gameData = gameData;
  commWork->gameDataSize = size;
  commWork->dataIdx = 0;
  commWork->state = MCS_SEND_GAMEDATA_INIT;
}

#pragma mark [>SendFunc
//--------------------------------------------------------------
// �t���O���M
//--------------------------------------------------------------
typedef struct
{
  u32 value;
  u8 type;
  u8 pad[3];
}MB_COMM_FLG_PACKET;

const BOOL MB_COMM_Send_Flag( MB_COMM_WORK *commWork , const MB_COMM_FLG_TYPE type , const u32 value )
{
  BOOL ret;
  GFL_NETHANDLE *selfHandle;
  MB_COMM_FLG_PACKET pkt;

  if( GFL_NET_IsParentMachine() == TRUE )
  {
    selfHandle = GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER);
  }
  else
  {
    selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
  }

  pkt.type = type;
  pkt.value = value;
  MB_COMM_TPrintf("Send Flg[%d:%d].\n",pkt.type,pkt.value);

  ret = GFL_NET_SendDataEx( selfHandle , GFL_NET_SENDID_ALLUSER , 
                            MCST_FLG , sizeof( MB_COMM_FLG_PACKET ) , 
                            (void*)&pkt , TRUE , FALSE , FALSE );
  if( ret == FALSE )
  {
    MB_COMM_TPrintf("Send Flg is failued!!\n");
  }
  return ret;
}

//--------------------------------------------------------------
// �t���O��M
//--------------------------------------------------------------
static void MB_COMM_Post_Flag( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
  MB_COMM_WORK *commWork = (MB_COMM_WORK*)pWork;
  MB_COMM_FLG_PACKET *pkt = (MB_COMM_FLG_PACKET*)pData;

  MB_COMM_TPrintf("Post Flg[%d:%d(0x%x)] To [%d].\n",pkt->type,pkt->value,pkt->value,netID);
  switch( pkt->type )
  {
  case MCFT_GAMEDATA_SIZE:
    commWork->isPostGameDataSize = TRUE;
    if( GFL_NET_IsParentMachine() == FALSE )
    {
      if( commWork->gameData == NULL )
      {
        commWork->gameData = GFL_NET_Align32Alloc( HEAPID_MULTIBOOT_DATA , pkt->value );
        commWork->gameDataSize = pkt->value;
        commWork->dataIdx = 0;
        commWork->state = MCS_POST_GAMEDATA_WAIT;
        commWork->isPostGameData = FALSE;
#ifdef MB_COMM_TICK_TEST
        gameDataAllTick = OS_GetTick();
#endif
      }
    }
    break;
  
  case MCFT_GAMEDATA_POST:
    if( GFL_NET_IsParentMachine() == TRUE )
    {
      commWork->dataIdx++;
      if( commWork->gameDataSize <= commWork->dataIdx*MB_COMM_GAMEDATA_SEND_SIZE )
      {
        commWork->state = MCS_CONNECT;
      }
      else
      {
        commWork->state = MCS_SEND_GAMEDATA_SEND;
      }
    }
    break;
  }
}

//--------------------------------------------------------------
// �������p�f�[�^���M
//--------------------------------------------------------------
const BOOL MB_COMM_Send_InitData( MB_COMM_WORK *commWork , MB_COMM_INIT_DATA *initData )
{
  BOOL ret;
  GFL_NETHANDLE *parentHandle = GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER);
  //GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
  MB_COMM_TPrintf("Send InitData.\n");

  //�q�@�ɂ�������Ηǂ�
  ret = GFL_NET_SendDataEx( parentHandle , 1 , 
                            MCST_INIT_DATA , sizeof( MB_COMM_INIT_DATA ) , 
                            (void*)initData , TRUE , FALSE , TRUE );
  if( ret == FALSE )
  {
    MB_COMM_TPrintf("Send InitData is failued!!\n");
  }
  return ret;
  
}

//--------------------------------------------------------------
// �������p�f�[�^��M
//--------------------------------------------------------------
static void MB_COMM_Post_InitData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
  MB_COMM_WORK *commWork = (MB_COMM_WORK*)pWork;
  MB_COMM_TPrintf("Post InitData.\n");
  commWork->isPostInitData = TRUE;
  
}
static u8*    MUS_COMM_Post_InitDataBuff( int netID, void* pWork , int size )
{
  MB_COMM_WORK *commWork = (MB_COMM_WORK*)pWork;
  if( commWork->initData == NULL )
  {
    commWork->initData = GFL_HEAP_AllocClearMemory( commWork->heapId , sizeof( MB_COMM_INIT_DATA ) );
  }
  return (u8*)commWork->initData;
}


//--------------------------------------------------------------
// �Q�[���p�f�[�^���M
//--------------------------------------------------------------
const BOOL MB_COMM_Send_GameData( MB_COMM_WORK *commWork , void *gameData , const u32 size )
{
  BOOL ret;
  GFL_NETHANDLE *parentHandle = GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER);
  //GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
  MB_COMM_TPrintf("Send GameData.\n");

  //�q�@�ɂ�������Ηǂ�
  ret = GFL_NET_SendDataEx( parentHandle , 1 , 
                            MCST_GAME_DATA , size , 
                            gameData , FALSE , TRUE , TRUE );
  if( ret == FALSE )
  {
    MB_COMM_TPrintf("Send GameData is failued!!\n");
  }
  return ret;
  
}

//--------------------------------------------------------------
// �Q�[���p�f�[�^��M
//--------------------------------------------------------------
#ifdef MB_COMM_TICK_TEST
static OSTick gameDataTick = 0;
#endif
static void MB_COMM_Post_GameData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
  MB_COMM_WORK *commWork = (MB_COMM_WORK*)pWork;
#ifdef MB_COMM_TICK_TEST
  MB_COMM_TPrintf("Post GameData[%d][%dms].\n",size,OS_TicksToMilliSeconds(OS_GetTick()-gameDataTick));
#else
  MB_COMM_TPrintf("Post GameData[%d].\n",size);
#endif
  commWork->isPostGameData = TRUE;
}
static u8*    MUS_COMM_Post_GameDataBuff( int netID, void* pWork , int size )
{
  MB_COMM_WORK *commWork = (MB_COMM_WORK*)pWork;
  u32 topAdr = (u32)commWork->gameData + commWork->dataIdx*MB_COMM_GAMEDATA_SEND_SIZE;
#ifdef MB_COMM_TICK_TEST
  gameDataTick = OS_GetTick();
#endif
  MB_COMM_TPrintf("Post GameDataBuff[%x].\n",topAdr);
  return (u8*)topAdr;
}

//--------------------------------------------------------------
/**
 *  �e��r�[�R���p�R�[���o�b�N�֐�(�_�~�[
 */
//--------------------------------------------------------------
void* MB_COMM_GetBeaconDataDummy(void* pWork)
{
  static u8 dummy[2];
  return (void*)&dummy;
}
int MB_COMM_GetBeaconSizeDummy(void* pWork)
{
  return sizeof(u8)*2;
}
