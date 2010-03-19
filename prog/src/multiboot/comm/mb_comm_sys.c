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
#define MB_COMM_TPrintf(...) (void)((MB_TPrintf(__VA_ARGS__)))
#else
#define MB_COMM_TPrintf(...) (void)((MB_TPrintf(__VA_ARGS__)))
#endif //MULTI_BOOT_MAKE
#else

#define MB_COMM_TPrintf(...) (void)((MB_TPrintf(__VA_ARGS__)))
#endif//�f�o�b�O�o�͐؂�ւ�

#define MB_COMM_PPP_PACK_SIZE (POKETOOL_GetPPPWorkSize()*MB_CAP_POKE_NUM + sizeof(MB_COMM_PPP_PACK_HEADER))

#define MB_COMM_TIMMING_EXIT (64)
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

  MCS_WAIT_DISCONNECT,
  MCS_DISCONNECT,
  
  //�R�R���牺�͂Ȃ����Ă����Ԃ�STATE�Ƃ���
  MCS_CONNECT,
  
  MCS_MAX,
}MB_COMM_STATE;

//���M�̎��
typedef enum
{
  MCST_FLG = GFL_NET_CMD_MULTIBOOT,
  MCST_INIT_DATA,
  MCST_POKE_DATA,
  
  MCST_MAX,
}MB_COMM_SEND_TYPE;

//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
typedef struct
{
  u8 num;
  u8 pad;

  u16 score;
}MB_COMM_PPP_PACK_HEADER;

struct _MB_COMM_WORK
{
  HEAPID heapId;
  
  MB_COMM_STATE state;
  
  MB_COMM_CHILD_STATE newChildState;
  MB_COMM_CHILD_STATE childState;
  
  void  *pppPackData;  //�w�b�_�{PPP6���̃o�b�t�@(�����M�͕K�v���I
  //�q�@�̂�
  MB_COMM_INIT_DATA *initData;
  
  //�m�F�t���O�n
  u16    isPostInitData:1;
  u16    isPostPokeData:1;

  u16    isPost_PostPokeData:1;

  u16    isReadyChildStatSave:1;
  u16    isFinishChildFirstSave:1;
  u16    isFinishChildSecondSave:1;
  u16    isFinishChildSave:1;
  u16    isPermitStartSave:1;
  u16    isPermitFirstSave:1;
  u16    isPermitSecondSave:1;
  u16    isPermitFinishSave:1;
  u16    isInitLowerData:1;
  u16    isPostBoxLeast:1;    //13
  //�f��z�M
  u16    isPostMoviePokeConfirm:1;
  u16    isPostMoviePokeNum:1;
  u16    isPostMoviePokeSendFinish:1; //16
  
  u16    isPostMovieHaveLockCapsule:1; //������Z�b�g
  u16    isMovieHaveLockCapsule:1;
  u16    isPostMovieTransLockCapsule:1;
  u16    isMovieTransLockCapsule:1;
  u16    isMovieFinishMachine:1;
  u16    pad:11;
  
  u8      saveWaitCnt;
  u16     boxLeast;
  u8      moviePokeConfirm;
  u32     moviePokeNum; //���2bit�̓A�C�e���̏�ԁA���8bit�͔�`�����̐�
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
static void MB_COMM_Post_PokeData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );
static u8*    MUS_COMM_Post_PokeDataBuff( int netID, void* pWork , int size );

static const NetRecvFuncTable MultiBootCommPostTable[] = {
  { MB_COMM_Post_Flag     , NULL },
  { MB_COMM_Post_InitData , MUS_COMM_Post_InitDataBuff },
  { MB_COMM_Post_PokeData , MUS_COMM_Post_PokeDataBuff },
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
  commWork->isInitLowerData = FALSE;

  commWork->childState = MCCS_NONE;
  commWork->newChildState = MCCS_NONE;
  
  commWork->pppPackData = GFL_HEAP_AllocClearMemory( commWork->heapId , MB_COMM_PPP_PACK_SIZE );
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
  if( commWork->isInitLowerData == TRUE )
  {
    GFL_NET_LDATA_ExitSystem();
  }

  GFL_HEAP_FreeMemory( commWork->pppPackData );
  GFL_HEAP_FreeMemory( commWork );
}

//--------------------------------------------------------------
//  �X�V
//--------------------------------------------------------------
void MB_COMM_UpdateSystem( MB_COMM_WORK* commWork )
{
  static int befState = 0xFFFF;
  if( befState != commWork->state )
  {
    MB_COMM_TPrintf("MBCommState[%d]->[%d]\n",befState,commWork->state);
    befState = commWork->state;
  }
  
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
        GFL_NET_SetNoChildErrorCheck( TRUE );
      }
    }
    break;
  
  case MCS_WAIT_DISCONNECT:
    {
      GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
      if( GFL_NET_HANDLE_IsTimeSync( selfHandle , MB_COMM_TIMMING_EXIT , WB_NET_MULTIBOOT_SERVICEID ) == TRUE )
      {
        commWork->state = MCS_DISCONNECT;
        GFL_NET_SetNoChildErrorCheck( FALSE );
      }
    }
    break;
  case MCS_DISCONNECT:
    //None
    break;
  
  case MCS_CONNECT:
    //���̏�Ԃ�MB_COMM_IsSendEnable�̃`�F�b�N�Ɏg���Ă�B
    break;
    
  }
  
  if( commWork->childState != commWork->newChildState &&
      commWork->newChildState != MCCS_NONE )
  {
    const BOOL ret = MB_COMM_Send_Flag( commWork , MCFT_CHILD_STATE , commWork->newChildState );
    if( ret == TRUE )
    {
      commWork->childState = commWork->newChildState;
    }
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
    SYASHI_NETWORK_GGID,  //ggid  
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
  commWork->isReadyChildStatSave = FALSE;
  commWork->isFinishChildFirstSave = FALSE;
  commWork->isFinishChildSecondSave = FALSE;
  commWork->isFinishChildSave = FALSE;
  commWork->isPermitStartSave = FALSE;
  commWork->isPermitFirstSave = FALSE;
  commWork->isPermitSecondSave = FALSE;
  commWork->isPermitFinishSave = FALSE;
  commWork->isInitLowerData = TRUE;
  commWork->isPostMoviePokeNum = FALSE;
  commWork->isPostMoviePokeConfirm = FALSE;
  commWork->isPostMoviePokeSendFinish = FALSE;
  commWork->isPostMovieHaveLockCapsule = FALSE;
  commWork->isMovieHaveLockCapsule = FALSE;
  commWork->isPostMovieTransLockCapsule = FALSE;
  commWork->isMovieTransLockCapsule = FALSE;
  commWork->isMovieFinishMachine = FALSE;
  
  commWork->moviePokeConfirm = MCMV_POKETRANS_YES;
  commWork->moviePokeNum = 0;
  commWork->boxLeast = 0;
  commWork->saveWaitCnt = 0;

  GFL_NET_LDATA_InitSystem( commWork->heapId , FALSE );
}

//--------------------------------------------------------------
// �ʐM�I��
//--------------------------------------------------------------
void MB_COMM_ExitComm( MB_COMM_WORK* commWork )
{
  GFL_NET_Exit(NULL);
  GFL_NET_LDATA_ExitSystem();
  commWork->isInitLowerData = FALSE;
  
  commWork->state = MCS_NONE;
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

  GFL_NET_LDATA_CreatePostBuffer( 0x6F000 , 0 , HEAPID_MULTIBOOT_DATA );
}

//--------------------------------------------------------------
// �I�����N�G�X�g
//--------------------------------------------------------------
void MB_COMM_ReqDisconnect( MB_COMM_WORK* commWork )
{
  GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
  GFL_NET_HANDLE_TimeSyncStart( selfHandle , MB_COMM_TIMMING_EXIT , WB_NET_MULTIBOOT_SERVICEID );

  commWork->state = MCS_WAIT_DISCONNECT;
}
const BOOL MB_COMM_IsDisconnect( MB_COMM_WORK* commWork )
{
  if( commWork->state == MCS_DISCONNECT )
  {
    return TRUE;
  }
  return FALSE;
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

const BOOL MB_COMM_IsPostGameData( const MB_COMM_WORK* commWork )
{
  return GFL_NET_LDATA_IsFinishPost( 0 );
}

//--------------------------------------------------------------
// �q�@�X�e�[�g�̐ݒ�E�擾
//--------------------------------------------------------------
void MB_COMM_SetChildState( MB_COMM_WORK* commWork , MB_COMM_CHILD_STATE state )
{
  //������������X�V���Ɏ����ő���
  commWork->newChildState = state;
}
const MB_COMM_CHILD_STATE MB_COMM_GetChildState( const MB_COMM_WORK* commWork )
{
  return commWork->childState;
}

//--------------------------------------------------------------
// �ۑ��n�X�e�[�g�̎哾
//--------------------------------------------------------------
void MB_COMM_ResetFlag( MB_COMM_WORK* commWork )
{
  commWork->isPostPokeData = FALSE;

  commWork->isPost_PostPokeData = FALSE;

  commWork->isReadyChildStatSave = FALSE;
  commWork->isFinishChildFirstSave = FALSE;
  commWork->isFinishChildSecondSave = FALSE;
  commWork->isFinishChildSave = FALSE;
  commWork->isPermitStartSave = FALSE;
  commWork->isPermitFirstSave = FALSE;
  commWork->isPermitSecondSave = FALSE;
  commWork->isPermitFinishSave = FALSE;
  commWork->isPostBoxLeast = FALSE;
  
  commWork->saveWaitCnt = 0;
  
}
const BOOL MB_COMM_GetIsReadyChildStartSave( const MB_COMM_WORK* commWork )
{
  return commWork->isReadyChildStatSave;
}
const BOOL MB_COMM_GetIsFinishChildFirstSave( const MB_COMM_WORK* commWork )
{
  return commWork->isFinishChildFirstSave;
}
const BOOL MB_COMM_GetIsFinishChildSecondSave( const MB_COMM_WORK* commWork )
{
  return commWork->isFinishChildFirstSave;
}
const BOOL MB_COMM_GetIsFinishChildSave( const MB_COMM_WORK* commWork )
{
  return commWork->isFinishChildSave;
}
const BOOL MB_COMM_GetIsPermitStartSave( const MB_COMM_WORK* commWork )
{
  return commWork->isPermitStartSave;
}
const BOOL MB_COMM_GetIsPermitFirstSave( const MB_COMM_WORK* commWork )
{
  return commWork->isPermitFirstSave;
}
const BOOL MB_COMM_GetIsPermitSecondSave( const MB_COMM_WORK* commWork )
{
  return commWork->isPermitSecondSave;
}
const BOOL MB_COMM_GetIsPermitFinishSave( const MB_COMM_WORK* commWork )
{
  return commWork->isPermitFinishSave;
}
//�����Z�[�u�̂��߁A�����o�Ă��瓯���t���[���҂��ăZ�[�u
const u8 MB_COMM_GetSaveWaitTime( const MB_COMM_WORK* commWork )
{
  return commWork->saveWaitCnt;
}

const BOOL MB_COMM_IsPostBoxLeast( const MB_COMM_WORK* commWork )
{
  return commWork->isPostBoxLeast;
}
const u16 MB_COMM_GetBoxLeast( const MB_COMM_WORK* commWork )
{
  return commWork->boxLeast;
}
const BOOL MB_COMM_IsPostMoviePokeNum( const MB_COMM_WORK* commWork )
{
  return commWork->isPostMoviePokeNum;
}
//���2bit�̓A�C�e���̏�ԁA���16bit�͔�`�����̐�
const u16 MB_COMM_GetMoviePokeNum( const MB_COMM_WORK* commWork )
{
  return commWork->moviePokeNum&0x0000FFFF;
}
const u16 MB_COMM_GetMoviePokeNumHiden( const MB_COMM_WORK* commWork )
{
  return (commWork->moviePokeNum&0x3FFF0000)>>16;
}
const BOOL MB_COMM_GetMoviePokeNumHaveItem( const MB_COMM_WORK* commWork )
{
  if( commWork->moviePokeNum&0x40000000 )
  {
    return TRUE;
  }
  return FALSE;
}
const BOOL MB_COMM_GetMoviePokeNumFullItem( const MB_COMM_WORK* commWork )
{
  if( commWork->moviePokeNum&0x80000000 )
  {
    return TRUE;
  }
  return FALSE;
}

const BOOL MB_COMM_IsPostMoviePokeConfirm( const MB_COMM_WORK* commWork )
{
  return commWork->isPostMoviePokeConfirm;
}
const u16 MB_COMM_GetMoviePokeConfirm( const MB_COMM_WORK* commWork )
{
  return commWork->moviePokeConfirm;
}
const BOOL MB_COMM_IsPostMoviePokeFinishSend( const MB_COMM_WORK* commWork )
{
  return commWork->isPostMoviePokeSendFinish;
}
const BOOL MB_COMM_IsPostMovieHaveLockCapsule( const MB_COMM_WORK* commWork )
{
  return commWork->isPostMovieHaveLockCapsule;
}
const BOOL MB_COMM_IsMovieHaveLockCapsule( const MB_COMM_WORK* commWork )
{
  return commWork->isMovieHaveLockCapsule;
}
const BOOL MB_COMM_IsPostMovieTransLockCapsule( const MB_COMM_WORK* commWork )
{
  return commWork->isPostMovieTransLockCapsule;
}
const BOOL MB_COMM_IsMovieTransLockCapsule( const MB_COMM_WORK* commWork )
{
  return commWork->isMovieTransLockCapsule;
}
const BOOL MB_COMM_IsPostMovieFinishMachine( const MB_COMM_WORK* commWork )
{
  return commWork->isMovieFinishMachine;
}
#pragma mark [>SendDataFunc
//--------------------------------------------------------------
// �ł����T�C�Y���M�p
//--------------------------------------------------------------
void MB_COMM_InitSendGameData( MB_COMM_WORK* commWork , void* gameData , u32 size )
{
  MB_TPrintf("Send GameData.\n");
  GFL_NET_LDATA_SetSendData( gameData , size , 0x02 , TRUE );
}

//--------------------------------------------------------------
// ����M�p�|�P�f�[�^�N���A
//--------------------------------------------------------------
void MB_COMM_ClearSendPokeData( MB_COMM_WORK* commWork )
{
  u8 i;
  MB_COMM_PPP_PACK_HEADER *packHeader = commWork->pppPackData;
  
  packHeader->num = 0;
  packHeader->score = 0;
  for( i=0;i<MB_CAP_POKE_NUM;i++ )
  {
    POKEMON_PASO_PARAM *ppp = (POKEMON_PASO_PARAM*)((u32)commWork->pppPackData + POKETOOL_GetPPPWorkSize()*i + sizeof(MB_COMM_PPP_PACK_HEADER));
    PPP_Clear( ppp );
  }

  commWork->isPostPokeData = FALSE;
  commWork->isPost_PostPokeData = FALSE;
}

//--------------------------------------------------------------
// ���M�p�|�P�f�[�^�ǉ�
//--------------------------------------------------------------
void MB_COMM_AddSendPokeData( MB_COMM_WORK* commWork , const POKEMON_PASO_PARAM *ppp )
{
  u8 i;
  MB_COMM_PPP_PACK_HEADER *packHeader = commWork->pppPackData;
  GF_ASSERT( packHeader->num < MB_CAP_POKE_NUM );
  
  {
    void* pppAdr = (void*)((u32)commWork->pppPackData + POKETOOL_GetPPPWorkSize()*packHeader->num + sizeof(MB_COMM_PPP_PACK_HEADER));

    GFL_STD_MemCopy( ppp , pppAdr , POKETOOL_GetPPPWorkSize() );
  }
  packHeader->num++;
}
void MB_COMM_SetScore( MB_COMM_WORK* commWork , const u16 score )
{
  MB_COMM_PPP_PACK_HEADER *packHeader = commWork->pppPackData;
  packHeader->score = score;
}

//--------------------------------------------------------------
// �|�P��M�m�F
//--------------------------------------------------------------
const BOOL MB_COMM_IsPostPoke( MB_COMM_WORK* commWork )
{
  return commWork->isPostPokeData;
}

//--------------------------------------------------------------
// ��M�|�P�������擾
//--------------------------------------------------------------
const u8 MB_COMM_GetPostPokeNum( MB_COMM_WORK* commWork )
{
  MB_COMM_PPP_PACK_HEADER *packHeader = commWork->pppPackData;
  
  return packHeader->num;
}

//--------------------------------------------------------------
// ��M�|�P�����擾
//--------------------------------------------------------------
const POKEMON_PASO_PARAM* MB_COMM_GetPostPokeData( MB_COMM_WORK* commWork , const u8 idx )
{
  MB_COMM_PPP_PACK_HEADER *packHeader = commWork->pppPackData;
  return (POKEMON_PASO_PARAM*)((u32)commWork->pppPackData + POKETOOL_GetPPPWorkSize()*idx + sizeof(MB_COMM_PPP_PACK_HEADER));
}

//--------------------------------------------------------------
// ��M�|�P����/�X�R�A�擾
//--------------------------------------------------------------
const u16 MB_COMM_GetScore( MB_COMM_WORK* commWork )
{
  MB_COMM_PPP_PACK_HEADER *packHeader = commWork->pppPackData;
  return packHeader->score;
}

//--------------------------------------------------------------
// �|�P��M�Q��M�m�F�m�F
//--------------------------------------------------------------
const BOOL MB_COMM_IsPost_PostPoke( MB_COMM_WORK* commWork )
{
  return commWork->isPost_PostPokeData;
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
  // �e���q----------------------------------
  case MCFT_POST_POKE:
    commWork->isPost_PostPokeData = TRUE;
    break;
  case MCFT_PERMIT_START_SAVE:
    commWork->isPermitStartSave = TRUE;
    break;
  case MCFT_PERMIT_FIRST_SAVE:
    commWork->isPermitFirstSave = TRUE;
    commWork->saveWaitCnt = pkt->value;
    break;
  case MCFT_PERMIT_SECOND_SAVE:
    commWork->isPermitSecondSave = TRUE;
    commWork->saveWaitCnt = pkt->value;
    break;
  case MCFT_PERMIT_FINISH_SAVE:
    commWork->isPermitFinishSave = TRUE;
    break;

  case MCFT_LEAST_BOX:
    commWork->boxLeast = pkt->value;
    commWork->isPostBoxLeast = TRUE;
    break;

  // �q���e----------------------------------
  case MCFT_CHILD_STATE:
    commWork->childState = pkt->value;
    break;

  case MCFT_READY_START_SAVE:
    commWork->isReadyChildStatSave = TRUE;
    break;
  case MCFT_FINISH_FIRST_SAVE:
    commWork->isFinishChildFirstSave = TRUE;
    break;
  case MCFT_FINISH_SECOND_SAVE:
    commWork->isFinishChildSecondSave = TRUE;
    break;
  case MCFT_FINISH_SAVE:
    commWork->isFinishChildSave = TRUE;
    break;

  //�f��p �e���q
  case MCFT_MOVIE_POKE_TRANS_CONFIRM:
    commWork->isPostMoviePokeConfirm = TRUE;
    commWork->moviePokeConfirm = pkt->value;
    break;
    
  case MCFT_MOVIE_LOCK_CAPSULE_TRANS_CONFIRM:
    commWork->isPostMovieTransLockCapsule = TRUE;
    commWork->isMovieTransLockCapsule = pkt->value;
    break;
    
  case MCFT_MOVIE_FINISH_MACHINE:
    commWork->isMovieFinishMachine = TRUE;
    break;

  //�f��z�M �q���e----------------------------------
  case MCFT_MOVIE_POKE_NUM:
    commWork->isPostMoviePokeNum = TRUE;
    commWork->moviePokeNum = pkt->value;
    break;
  
  case MCFT_MOVIE_FINISH_SEND_POKE:
    commWork->isPostMoviePokeSendFinish = TRUE;
    break;
  
  case MCFT_MOVIE_HAVE_LOCK_CAPSULE:
    commWork->isPostMovieHaveLockCapsule = TRUE;
    commWork->isMovieHaveLockCapsule = pkt->value;
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
// �|�P�����f�[�^���M
//--------------------------------------------------------------
const BOOL MB_COMM_Send_PokeData( MB_COMM_WORK *commWork )
{
  BOOL ret;
  MB_COMM_PPP_PACK_HEADER *packHeader = commWork->pppPackData;
  const u32 sendSize = packHeader->num * POKETOOL_GetPPPWorkSize() + sizeof(MB_COMM_PPP_PACK_HEADER);
  //GFL_NETHANDLE *parentHandle = GFL_NET_GetNetHandle(GFL_NET_NETID_SERVER);
  GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
  MB_COMM_TPrintf("Send PokeData.\n");

  //�e�@�ɂ�������Ηǂ�
  ret = GFL_NET_SendDataEx( selfHandle , 0 , 
                            MCST_POKE_DATA , sendSize , 
                            commWork->pppPackData , TRUE , FALSE , TRUE );
  if( ret == FALSE )
  {
    MB_COMM_TPrintf("Send PokeData is failued!!\n");
  }
  return ret;
  
}
//--------------------------------------------------------------
// �������p�f�[�^��M
//--------------------------------------------------------------
static void MB_COMM_Post_PokeData( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
  MB_COMM_WORK *commWork = (MB_COMM_WORK*)pWork;
  MB_COMM_TPrintf("Post PokeData.\n");
  commWork->isPostPokeData = TRUE;
  
}
static u8*    MUS_COMM_Post_PokeDataBuff( int netID, void* pWork , int size )
{
  MB_COMM_WORK *commWork = (MB_COMM_WORK*)pWork;
  return (u8*)commWork->pppPackData;
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
