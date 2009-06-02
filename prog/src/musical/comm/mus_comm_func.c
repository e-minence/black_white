//======================================================================
/**
 * @file	mus_comm_func.c
 * @brief	�~���[�W�J���ʐM�@�\
 * @author	ariizumi
 * @data	09/05/29
 *
 * ���W���[�����FMUS_COMM_FUNC
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"

#include "net/network_define.h"

#include "mus_comm_func.h"
#include "mus_comm_define.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define MUS_COMM_BEACON_MAX (4)

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum
typedef enum
{
  MCST_FLG,
  
  MCST_MAX,
}MUS_COMM_SEND_TYPE;

//�ʐM���
typedef enum
{
  MCS_NONE,

  MCS_PARENT_REQ_NEGOTIATION,
  MCS_PARENT_WAIT_NEGOTIATION,
  MCS_PARENT_COLLECT,

  MCS_CHILD_SEARCH,
  MCS_CHILD_REQ_NEGOTIATION,
  MCS_CHILD_WAIT_NEGOTIATION,
  MCS_CHILD_CONNECT,
  
}MUS_COMM_STATE;

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

struct _MUS_COMM_WORK
{
  HEAPID heapId;
  
  BOOL isInitComm;
  BOOL isRefreshUserData;
  u8   befMemberNum;
  
  MUS_COMM_BEACON beacon;
  MUS_COMM_MODE mode;   //�e���q���H
  MUS_COMM_STATE commState; //�����ƍׂ�������

  GAME_COMM_SYS_PTR gameComm;
  MYSTATUS *myStatus;
};

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static void MUS_COMM_FinishNetInitCallback( void* pWork );
static void MUS_COMM_FinishNetTermCallback( void* pWork );

//static const BOOL MUS_COMM_Send_Flag( MUS_COMM_WORK *work );
static void MUS_COMM_Post_Flag( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle );

static void* MUS_COMM_GetUserData(void* pWork);
static int MUS_COMM_GetUserDataSize(void* pWork);
static void MUS_COMM_CheckUserData( MUS_COMM_WORK *work );

static void*  MUS_COMM_GetBeaconData(void* pWork);
static int MUS_COMM_GetBeaconSize(void *pWork);
static BOOL MUS_COMM_BeacomCompare(GameServiceID GameServiceID1, GameServiceID GameServiceID2);

inline static void MUS_COMM_SetCommState( MUS_COMM_WORK *work , const MUS_COMM_STATE commState );

static const NetRecvFuncTable MusCommRecvTable[MCST_MAX] = 
{
  { MUS_COMM_Post_Flag   ,NULL  },
};

//--------------------------------------------------------------
//	���[�N�쐬
//--------------------------------------------------------------
MUS_COMM_WORK* MUS_COMM_CreateWork( HEAPID heapId , GAME_COMM_SYS_PTR gameComm , SAVE_CONTROL_WORK *saveCtrl )
{
  MUS_COMM_WORK* work = GFL_HEAP_AllocClearMemory( heapId , sizeof( MUS_COMM_WORK ));
  work->gameComm = gameComm;
  work->mode = MCM_NONE;
  work->isInitComm = FALSE;
  work->myStatus = SaveData_GetMyStatus(saveCtrl);

  MUS_COMM_SetCommState( work , MCS_NONE);

  return work;
}

//--------------------------------------------------------------
// ���[�N�폜
//--------------------------------------------------------------
void MUS_COMM_DeleteWork( MUS_COMM_WORK* work )
{
  if( work->isInitComm == TRUE )
  {
    GFL_NET_Exit(NULL);
  }

  GFL_HEAP_FreeMemory( work );
}

//--------------------------------------------------------------
// �ʐM�J�n
//--------------------------------------------------------------
void MUS_COMM_InitComm( MUS_COMM_WORK* work )
{
  GFLNetInitializeStruct aGFLNetInit = 
  {
    MusCommRecvTable, //NetSamplePacketTbl,  // ��M�֐��e�[�u��
    NELEMS(MusCommRecvTable), // ��M�e�[�u���v�f��
    NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
    NULL, ///< �l�S�V�G�[�V�����������ɃR�[��
    MUS_COMM_GetUserData, // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
    MUS_COMM_GetUserDataSize, // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
    MUS_COMM_GetBeaconData,   // �r�[�R���f�[�^�擾�֐�  
    MUS_COMM_GetBeaconSize,   // �r�[�R���f�[�^�T�C�Y�擾�֐� 
    MUS_COMM_BeacomCompare, // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
    NULL, // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂��
    NULL, //FatalError
    NULL, // �ʐM�ؒf���ɌĂ΂��֐�(�I����
    NULL,                   // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
    NULL,                   ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
    NULL,                   ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
    NULL,                   ///< wifi�t�����h���X�g�폜�R�[���o�b�N
    NULL,                   ///< DWC�`���̗F�B���X�g  
    NULL,                   ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
    0,                      ///< DWC�ւ�HEAP�T�C�Y
    TRUE,                   ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
    0x333,                  //ggid  DP=0x333,RANGER=0x178,WII=0x346
    GFL_HEAPID_APP,         //���ɂȂ�heapid
    HEAPID_NETWORK,         //�ʐM�p��create�����HEAPID
    HEAPID_WIFI,            //wifi�p��create�����HEAPID
    HEAPID_NETWORK,         //
    GFL_WICON_POSX,GFL_WICON_POSY,  // �ʐM�A�C�R��XY�ʒu
    MUSICAL_COMM_MEMBER_NUM,        //_MAXNUM,  //�ő�ڑ��l��
    48,                     //_MAXSIZE, //�ő呗�M�o�C�g��
    MUS_COMM_BEACON_MAX,    //_BCON_GET_NUM,  // �ő�r�[�R�����W��
    TRUE,                   // CRC�v�Z
    FALSE,                  // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
    GFL_NET_TYPE_WIRELESS,  //�ʐM�^�C�v�̎w��
    TRUE,                   // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
    WB_NET_MUSICAL,     //GameServiceID
#if GFL_NET_IRC
    IRC_TIMEOUT_STANDARD, // �ԊO���^�C���A�E�g����
#endif
  };  
  
  work->befMemberNum = 0;
  work->isRefreshUserData = FALSE;
  
  GFL_NET_Init( &aGFLNetInit , MUS_COMM_FinishNetInitCallback , (void*)work );

}

//--------------------------------------------------------------
// �ʐM�I��
//--------------------------------------------------------------
void MUS_COMM_ExitComm( MUS_COMM_WORK* work )
{
  work->mode = MCM_NONE;
  GFL_NET_Exit(MUS_COMM_FinishNetInitCallback);
}

//--------------------------------------------------------------
// �������p�R�[���o�b�N
//--------------------------------------------------------------
static void MUS_COMM_FinishNetInitCallback( void* pWork )
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  work->isInitComm = TRUE;
}

//--------------------------------------------------------------
// �I���p�R�[���o�b�N
//--------------------------------------------------------------
static void MUS_COMM_FinishNetTermCallback( void* pWork )
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  work->isInitComm = FALSE;
}

//--------------------------------------------------------------
// �ʐM�X�V
//--------------------------------------------------------------
void MUS_COMM_UpdateComm( MUS_COMM_WORK* work )
{
  switch( work->commState )
  {
    //�e�@
  case MCS_PARENT_REQ_NEGOTIATION:
    if( GFL_NET_HANDLE_RequestNegotiation() == TRUE )
    {
      MUS_COMM_SetCommState( work , MCS_PARENT_WAIT_NEGOTIATION);
    }
    break;
    
  case MCS_PARENT_WAIT_NEGOTIATION:
    {
      GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
      if( GFL_NET_HANDLE_IsNegotiation( selfHandle ) == TRUE )
      {
        MUS_COMM_SetCommState( work , MCS_PARENT_COLLECT);
      }
    }
    break;
    
  case MCS_PARENT_COLLECT:
    MUS_COMM_CheckUserData( work );
    break;

    //�q�@
  case MCS_CHILD_SEARCH:
    {
      u16 i=0;
      void* beacon;
      do
      {
        beacon = GFL_NET_GetBeaconData( i );
        i++;
      }while( beacon == NULL && i < MUS_COMM_BEACON_MAX );
      
      if( beacon != NULL )
      {
        u8 *macAdd = GFL_NET_GetBeaconMacAddress( i-1 );
        GFL_NET_ConnectToParent( macAdd );
        MUS_COMM_SetCommState( work , MCS_CHILD_REQ_NEGOTIATION);
      }
    }
    break;

  case MCS_CHILD_REQ_NEGOTIATION:
    if( GFL_NET_HANDLE_RequestNegotiation() == TRUE )
    {
      MUS_COMM_SetCommState( work , MCS_CHILD_WAIT_NEGOTIATION);
    }
    break;

  case MCS_CHILD_WAIT_NEGOTIATION:
    {
      GFL_NETHANDLE *selfHandle = GFL_NET_HANDLE_GetCurrentHandle();
      if( GFL_NET_HANDLE_IsNegotiation( selfHandle ) == TRUE )
      {
        MUS_COMM_SetCommState( work , MCS_CHILD_CONNECT);
      }
    }
    break;
  case MCS_CHILD_CONNECT:
    MUS_COMM_CheckUserData( work );
    break;

  }
}

//--------------------------------------------------------------
// �e�@�ʐM�J�n
//--------------------------------------------------------------
void MUS_COMM_StartParent( MUS_COMM_WORK* work )
{
  work->mode = MCM_PARENT;
  MUS_COMM_SetCommState(work , MCS_PARENT_REQ_NEGOTIATION);
  GFL_NET_InitServer();
}

//--------------------------------------------------------------
// �q�@�ʐM�J�n
//--------------------------------------------------------------
void MUS_COMM_StartChild( MUS_COMM_WORK* work )
{
  work->mode = MCM_CHILD;
  MUS_COMM_SetCommState( work , MCS_CHILD_SEARCH);
  GFL_NET_StartBeaconScan();
}

#pragma mark [> userData func
//--------------------------------------------------------------
// ���[�U�[�f�[�^�ݒ�
//--------------------------------------------------------------
static void* MUS_COMM_GetUserData(void* pWork)
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  
  //TODO �O�̂��ߖ��O�������ĂȂ��Ƃ��ɗ����Ȃ��悤�ɂ��Ă���
  if( MyStatus_CheckNameClear( work->myStatus ) == TRUE )
  {
    u16 tempName[7] = { L'N',L'o',L'N',L'a',L'm',L'e',0xFFFF };
    MyStatus_SetMyName( work->myStatus , tempName );
  }

  
  return work->myStatus;
}

static int MUS_COMM_GetUserDataSize(void* pWork)
{
  return MyStatus_GetWorkSize();
}

static void MUS_COMM_CheckUserData( MUS_COMM_WORK *work )
{
  //�V���������o�[�����邩�H
  if( work->befMemberNum != GFL_NET_GetConnectNum() )
  {
    work->befMemberNum = GFL_NET_GetConnectNum();
    work->isRefreshUserData = TRUE;
  }
}

const BOOL MUS_COMM_IsRefreshUserData( MUS_COMM_WORK *work )
{
  return work->isRefreshUserData;
}
void MUS_COMM_ResetRefreshUserData( MUS_COMM_WORK *work )
{
  work->isRefreshUserData = FALSE;
}

//--------------------------------------------------------------
// �}�C�X�e�[�^�X�擾
//--------------------------------------------------------------
MYSTATUS* MUS_COMM_GetPlayerMyStatus( MUS_COMM_WORK* work , u8 idx )
{
  if( work->commState != MCS_NONE )
  {
    const GFL_NETHANDLE *handle = GFL_NET_GetNetHandle( idx );
    if( handle == NULL )
    {
      return NULL;
    }
    else
    {
      return (MYSTATUS*)GFL_NET_HANDLE_GetInfomationData( handle );
    }
  }
  return NULL;
}


#pragma mark [> beacon func
//--------------------------------------------------------------
// �r�[�R���f�[�^�擾�֐�  
//--------------------------------------------------------------
static void*  MUS_COMM_GetBeaconData(void* pWork)
{
  MUS_COMM_WORK *work = (MUS_COMM_WORK*)pWork;
  return (void*)&work->beacon;
}

//--------------------------------------------------------------
// �r�[�R���f�[�^�T�C�Y�擾�֐� 
//--------------------------------------------------------------
static int MUS_COMM_GetBeaconSize(void *pWork)
{
  return sizeof( MUS_COMM_BEACON );
}

//--------------------------------------------------------------
// �ڑ��`�F�b�N�p��r
//--------------------------------------------------------------
static BOOL MUS_COMM_BeacomCompare(GameServiceID GameServiceID1, GameServiceID GameServiceID2)
{
  if( GameServiceID1 == GameServiceID2 )
  {
    return TRUE;
  }
  return FALSE;
}


#pragma mark [> send/post func
static void MUS_COMM_Post_Flag( const int netID, const int size , const void* pData , void* pWork , GFL_NETHANDLE *pNetHandle )
{
  
}

#pragma mark [> outer func

GAME_COMM_SYS_PTR MUS_COMM_GetGameComm( MUS_COMM_WORK* work )
{
  return work->gameComm;
}

const BOOL MUS_COMM_IsInitComm( MUS_COMM_WORK* work )
{
  return work->isInitComm;
}

MUS_COMM_MODE MUS_COMM_GetMode( MUS_COMM_WORK* work )
{
  return work->mode;
}

#pragma mark [> other func

inline static void MUS_COMM_SetCommState( MUS_COMM_WORK *work , const MUS_COMM_STATE commState )
{
  ARI_TPrintf("MusCommState[%d]\n",commState);
  work->commState = commState;
}
