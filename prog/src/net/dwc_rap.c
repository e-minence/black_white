//=============================================================================
/**
 * @file	  dwc_rap.c
 * @brief	  DWC���b�p�[�B�I�[�o�[���C���W���[���ɒu������
 * @author	kazuki yoshihara
 * @date    06/02/23
 */
//=============================================================================

#include <gflib.h>
#include <nitro.h>
#include <nnsys.h>
#include <gflib.h>
#include "sc/dwc_sc.h"
#include "gdb/dwc_gdb.h"
#include "net/network_define.h"
#include "net/nhttp_rap.h"
#include "savedata/wifilist.h"

#if GFL_NET_WIFI

// �f�o�b�O�o�͂��ʂɓf���o���ꍇ��`
#if defined(DEBUG_ONLY_FOR_ohno)
#define DEBUGPRINT_ON (0)
#elif defined(DEBUG_ONLY_FOR_toru_nagihashi)
#define DEBUGPRINT_ON (1)
#else
#define DEBUGPRINT_ON (0)
#endif

#ifdef PM_DEBUG
#ifdef WIFIMATCH_RATE_AUTO
#undef DEBUGPRINT_ON
#define DEBUGPRINT_ON (1)
#endif
#endif

#include "net/dwc_rap.h"
#include "vchat.h"
#include <vct.h>


// ���b�ԒʐM���͂��Ȃ��ƃ^�C���A�E�g�Ɣ��f���邩
#define MYDWC_TIMEOUTSEC (10)


// ���t���[�����M���Ȃ��ƁAKEEP_ALIVE�g�[�N���𑗂邩�B
#define KEEPALIVE_TOKEN_TIME 240

#define MYDWC_DEBUGPRINT NET_PRINT


// �t�����h���X�g�̃T�C�Y
#define FRIENDLIST_MAXSIZE (WIFILIST_FRIEND_MAX)

// �P�t���[���ɉ��l���̃f�[�^���X�V���邩�B
#define FRIENDINFO_UPDATA_PERFRAME (1)

#define SIZE_RECV_BUFFER (4 * 1024)
#define SIZE_SEND_BUFFER 256

#define _MATCHSTRINGNUM (128)

#define _WIFI_NUM_MAX (2)  // �ڑ�MAX��


typedef struct
{
  u8 sendBuffer[ SIZE_SEND_BUFFER ];
  //	u8* recvBuffer;
  DWCFriendData* pFriendData;  // DWC�`���̗F�B���X�g
  DWCFriendsMatchControl stDwcCnt;    // DWC����\����
  DWCUserData* pUserData;		// DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  DWCInetControl stConnCtrl;
  void *recvPtr[_WIFI_NUM_MAX];  //��M�o�b�t�@��32�o�C�g�A���C�����g���Ă��Ȃ��|�C���^
  int deletedIndex[FRIENDLIST_MAXSIZE];
  int srcIndex[FRIENDLIST_MAXSIZE];
  //  DWCUserData* pSaveDataUserData;
  //  DWCFriendData* pSaveDataFriendData;
  GFL_NET_MYDWCReceiverFunc serverCallback;
  GFL_NET_MYDWCReceiverFunc clientCallback;
  MYDWCDisconnectFunc disconnectCallback;
  void* pDisconnectWork;
  Callback_DisconnectError* pDisconnectErrorCallback;  //CleanUp���ɌĂт����ꍇ����
  void* pDisconnectErrorUserWork;
  MYDWCConnectFunc connectCallback;
  void* pConnectWork;
  GFL_NET_MYDWCConnectModeCheckFunc connectModeCheck;

  void (*fetalErrorCallback)(int);
  u8 randommatch_query[_MATCHSTRINGNUM];
  u8 friend_status[FRIENDLIST_MAXSIZE];   //WC_GetFriendStatusData�̖߂�l
  u8 friendinfo[FRIENDLIST_MAXSIZE][MYDWC_STATUS_DATA_SIZE_MAX]; //WC_GetFriendStatusData�œ�����l
  u32 friendupdate_index;
  u32 connectionUserData;// �V�@�\ �q�����̃��[�U�[�f�[�^������4�o�C�g

  int state;
  int stepMatchResult;
  int matching_type;

  int sendbufflag;

  //	int op_aid;			// �����aid
  int maxConnectNum;  //�ő�ڑ��l��
  u32 backupBitmap;

  int isvchat;
  int friendindex;   // ������ڑ�����t�����h�̃��X�g�C���f�b�N�X
  int newFriendConnect;  // �ڑ����Ă�����TRUE 2006.05.24 k.ohno
  BOOL bConnectCallback;
  int vchatcodec;
  s16 canceled_matching;
  s16 bVChat;     // VCHATONOFF

  int sendintervaltime[_WIFI_NUM_MAX];		// �O��f�[�^�𑗐M���Ă���̃t���[�����B
  int setupErrorCount;  //�G���[���������J�E���g

  int opvchaton;				// ����̃{�C�X�`���b�g���I�����I�t��
  u16 myvchaton;				// �����̃{�C�X�`���b�g���I�����I�t��
  u16 myvchat_send;			// �����f�[�^���M�t���O

  u32 BlockUse_BackupBitmap;

  u8 myseqno;				// ���������M����p�P�b�g�̃V�[�P���XNo
  u8 opseqno;				// ���肪�Ō�ɑ����Ă����p�P�b�g�̃V�[�P���XNo
  u8 bHeapError;  // HEAP�m�ێ��s�̏ꍇ
  u8 pausevchat; //vchat�ꎞ��~
  u8 blockClient;   // �N���C�A���g��ڑ��֎~�ɂ���
  u8 closedflag;		// ConnectionClosedCallback �Ńz�X�g����1�ɂȂ�����ؒf�����ɑJ�ڂ���̂��@TRUE�Őؒf�����ɑJ�ځ@080602 tomoya
  u8 saveing;  //�Z�[�u����1
  u8 bAutoDisconnect;  //�ؒf���������ꍇ�ɃG���[�ɂ��鎞��TRUE
  u8 bWiFiFriendGroup;  ///< �F�B�ƍs���T�[�r�X���ǂ���
  u8 bConnectEnable;  ///< �ڑ����Ƌ֎~���s��
  u8 sendFinish;  //ack
  u8 sendAck;     //ack
  u8 timeoutflag;
  u8 friendMatchOn;  //�t�����h�}�b�`�R���g���[�� 
  u8 dummy2;
  u8 dummy3;
} MYDWC_WORK;

// �e�@��AID
#define _WIFI_PARENT_AID (0)

enum  _blockStatus{
  _BLOCK_NONE,
  _BLOCK_START,
  _BLOCK_CALLBACK,
};

// �{�C�X�`���b�g�̃g�[�N���ƍ������Ȃ��悤�ɂ��邽�߁A
// �{�C�X�`���b�g�ƈႤ�R�Q�o�C�g�̐������p�P�b�g�̐擪�ɂ���B
#define MYDWC_PACKETYPE_MASK 0x000000ff
#define MYDWC_PACKET_VCHAT_MASK 0x00000100
#define MYDWC_PACKET_VCHAT_SHIFT 8
#define MYDWC_PACKET_SEQNO_POS 2

#define MYDWC_GAME_PACKET 0x0001
// �^�C���A�E�g������h�����߁A��莞�ԑ��M���Ȃ��ꍇ�A
// ����̃f�[�^�𑗂�B���̂Ƃ��̐���
#define MYDWC_KEEPALIVE_PACKET 0x0002

#define MYDWC_CHECK_PACKET 0x0003


enum{
  MDSTATE_INIT,
  MDSTATE_CONNECTING,
  MDSTATE_CONNECTED,
  MDSTATE_TRYLOGIN,
  MDSTATE_UPDATESERVERS,
  MDSTATE_UPDATESERVERSASYNC,
  MDSTATE_LOGIN,
  MDSTATE_MATCHING,
  MDSTATE_CANCEL,
  MDSTATE_MATCHED,
  MDSTATE_PLAYING,
  MDSTATE_CANCELFINISH,
  MDSTATE_FAIL,			// 2006.7.4 yoshihara�ǉ�
  MDSTATE_FAILFINISH,
  MDSTATE_ERROR,
  MDSTATE_ERROR_FM,
  MDSTATE_ERROR_DISCONNECT,
  MDSTATE_ERROR_FETAL,
  MDSTATE_DISCONNECTTING,
  MDSTATE_DISCONNECT,
  MDSTATE_TIMEOUT,
  MDSTATE_LAST
};

enum {
  MDTYPE_RANDOM,
  MDTYPE_PARENT,
  MDTYPE_CHILD
};


static MYDWC_WORK *_dWork = NULL;


static void LoginCallback(DWCError error, int profileID, void *param);
static void UpdateServersCallback(DWCError error, BOOL isChanged, void* param);
static void FriendStatusCallback(int index, u8 status, const char* statusString, void* param);
static void DeleteFriendListCallback(int deletedIndex, int srcIndex, void* param);
static void BuddyFriendCallback(int index, void* param);

static void ConnectToAnybodyCallback(DWCError error, BOOL cancel, BOOL self,BOOL isServer,int index, void *param);
static void SendDoneCallback( int size, u8 aid, void* param);
static void UserRecvCallback( u8 aid, u8* buffer, int size,void* param );
static void ConnectionClosedCallback(DWCError error, BOOL isLocal, BOOL isServer, u8  aid, int index, void* param);
//static int handleError();
static int EvaluateAnybodyCallback(int index, void* param);
static int mydwc_step(void);

static void recvTimeoutCallback(u8 aid, void* param);
static BOOL _isSendableReliable(void);
static void mydwc_releaseRecvBuff(int aid);
static void mydwc_releaseRecvBuffAll(void);
static void mydwc_allocRecvBuff(int i);

static void mydwc_updateFriendInfo( void );
static void _DWC_StartVChat(int heapID);

static void _FuncNonSave(void);
static void _sendAckMain(void);
static int _SendToAck(void *data, int size);


#if PM_DEBUG

static void _setStateDebug(int state,int line)
{
  NET_PRINT("rap %d %d\n",state,line);
  _dWork->state = state;
}
#define   _CHANGE_STATE(state)  _setStateDebug(state, __LINE__)

#else  //PM_DEBUG

static void _setState(int state)
{
  _dWork->state = state;
}
#define   _CHANGE_STATE(state)  _setState(state)

#endif //PM_DEBUG





//==============================================================================
/**
 * @brief   MYDWC���������ς݂��ǂ���
 * @param   void
 * @retval  TRUE  �������ς�
 */
//==============================================================================

BOOL GFL_NET_DWC_IsInit(void)
{
  return (_dWork!=NULL);
}


//==============================================================================
/**
 * �C���^�[�l�b�g�֐ڑ��J�n
 * @param   pSaveData      �Z�[�u�f�[�^�ւ̃|�C���^
 * @param   heapSize       �q�[�v�T�C�Y
 * @param   maxConnectNum  �ő�ڑ��l��
 * @retval  MYDWC_STARTCONNECT_OK �c OK
 * @retval  MYDWC_STARTCONNECT_FIRST �c ���߂Đڑ�����ꍇ�B�i���b�Z�[�W�\���̕K�v�L
 * @retval  MYDWC_STARTCONNECT_DIFFERENTDS �c �قȂ�c�r�Őڑ����悤���Ă�ꍇ�B�i�v�x���j
 */
//==============================================================================
int GFL_NET_DWC_startConnect(DWCUserData* pUserData, DWCFriendData* pFriendData)
{
  void* pTemp;
  GFLNetInitializeStruct* pNetInit = GFL_NET_GetNETInitStruct();

  // �q�[�v�̈悩�烏�[�N�̈���m�ہB
  GF_ASSERT_HEAVY( _dWork == NULL );
#ifdef DEBUGPRINT_ON
  DWC_SetReportLevel(DWC_REPORTFLAG_ALL);
#else
  DWC_SetReportLevel(0);
#endif

  NET_PRINT("mydwc_start %d %d\n",sizeof(MYDWC_WORK) + 32, pNetInit->maxBeaconNum);

  _dWork = GFL_NET_Align32Alloc(pNetInit->netHeapID, sizeof(MYDWC_WORK));

  {
    int i;
    for(i=0;i<FRIENDLIST_MAXSIZE;i++){
      _dWork->deletedIndex[i] = -1;
    }
  }


  _CHANGE_STATE(MDSTATE_INIT);
  //  _dWork->state = MDSTATE_INIT;

  _dWork->vchatcodec = VCHAT_NONE;
  _dWork->friendindex = -1;
  _dWork->maxConnectNum = pNetInit->maxConnectNum;
  _dWork->newFriendConnect = -1;
  _dWork->bVChat = FALSE;
  _dWork->bHeapError = FALSE;

  _dWork->myvchaton = 1;
  _dWork->opvchaton = 1;
  _dWork->myvchat_send = 1;

  _dWork->pUserData = pUserData;
  _dWork->pFriendData = pFriendData;
  // 2008.06.02 tomoya ClosedCallback�Őؒf�����ɑJ�ڂ���悤�ɂ���̂����t���O�ł��肩�����悤�ɕύX(Wi-Fi�N���u�S�l��W��ʗp)
  _dWork->closedflag = TRUE;

#ifdef PM_DEBUG
  DWC_ReportUserData(_dWork->pUserData);
#endif

  {
    int i;
    for( i = 0; i < FRIENDLIST_MAXSIZE; i++ )
    {
      _dWork->friend_status[i] = DWC_STATUS_OFFLINE;
    }
  }

#ifdef PM_DEBUG
  // ���[�U�f�[�^�̏�Ԃ��`�F�b�N�B
  GFL_NET_DWC_showFriendInfo();
#endif

  if( !DWC_CheckHasProfile( _dWork->pUserData ) )
  {
    // �܂����̃f�[�^�ň�x���v���e���Ɍq���ł��Ȃ��B
    return MYDWC_STARTCONNECT_FIRST;
  }

  if( !DWC_CheckValidConsole( _dWork->pUserData ) )
  {
    // �{�̏�񂪈Ⴄ���Ⴄ�c�r�Őڑ����悤�Ƃ��Ă���B
    return 	MYDWC_STARTCONNECT_DIFFERENTDS;
  }



  return MYDWC_STARTCONNECT_OK;
}

//==============================================================================
/**
 * dwc_rap.c���m�ۂ����̈���J������B
 * @param   none
 * @retval  none
 */
//==============================================================================
void GFL_NET_DWC_free()
{
  if(_dWork){
    GFLNetInitializeStruct* pNetInit = GFL_NET_GetNETInitStruct();
    mydwc_releaseRecvBuffAll();
    //        GFL_HEAP_DeleteHeap(pNetInit->wifiHeapID);
    GFL_NET_Align32Free( _dWork  );
    _dWork = NULL;
  }
  DWC_ClearError();
}

//==============================================================================
/**
 * �C���^�[�l�b�g�ڑ����ɖ��t���[���Ăяo�����֐�
 * @param   userdata �����̃��O�C���f�[�^�ւ̃|�C���^
 * @param   list �t�����h���X�g�̐擪�|�C���^
 * @retval  ���c�ڑ������B�O�c�ڑ����B���c�G���[�i�G���[�R�[�h���Ԃ�j
 */
//==============================================================================
int GFL_NET_DWC_connect()
{
  int result;
  //    NET_PRINT("mydwc_connect %d\n",_dWork->state);
  switch( _dWork->state )
  {
  case MDSTATE_INIT:
    // �������
    {
      GFLNetInitializeStruct* pNetInit = GFL_NET_GetNETInitStruct();
      // �l�b�g�ڑ�������
      DWC_InitInetEx(&_dWork->stConnCtrl, GFL_DMA_NET_NO, _NETWORK_POWERMODE, _NETWORK_SSL_PRIORITY);

      //DWC_SetAuthServer(GF_DWC_CONNECTINET_AUTH_TYPE);

      // �񓯊��Ƀl�b�g�ɐڑ�
      DWC_ConnectInetAsync();

      _CHANGE_STATE(MDSTATE_CONNECTING);
      //_dWork->state = MDSTATE_CONNECTING;
      _dWork->isvchat = 0;
    }

  case MDSTATE_CONNECTING:
    {
      // ���肵���󋵂ɂȂ�܂ő҂B
      if( DWC_CheckInet() )
      {
        if( DWC_GetInetStatus() == DWC_CONNECTINET_STATE_CONNECTED )
        {
          _CHANGE_STATE(MDSTATE_CONNECTED);
          //_dWork->state = MDSTATE_CONNECTED;
        }
        else
        {
          _CHANGE_STATE(MDSTATE_ERROR);
          //_dWork->state = MDSTATE_ERROR;
        }
      }
      else
      {
        // �����ڑ�������
        DWC_ProcessInet();
        break;
      }
    }

  case MDSTATE_CONNECTED:
    // �G���[���������Ă��Ȃ�������
    {
      int ret = mydwc_HandleError();
      if( ret != 0 ){
        return ret;
      }
    }
    {
      int max = 0;
      // �I�[�o�[���C���Ă�ꍇ DWC_Init���ēxCALL���Ă����Ȃ��Ƃ����Œ�~
      // �t�����h���C�u����������
      if(_dWork->pFriendData){
        max = FRIENDLIST_MAXSIZE;
      }
      DWC_InitFriendsMatch( _dWork->pUserData,
                            GAME_PRODUCTID,
                            GAME_SECRET_KEY, 0, 0,
                            _dWork->pFriendData, max);
      _dWork->friendMatchOn = TRUE;
    }

    {// IPL�̃��[�U�����g���ă��O�C��
      // �����̃��[�U�������k�B
      OSOwnerInfo info;
      u16 dummy[2]={0x0000,0x0000};
      OS_GetOwnerInfo( &info );
      DWC_LoginAsync( dummy, NULL, LoginCallback, NULL);
    }
    _CHANGE_STATE(MDSTATE_TRYLOGIN);
    //_dWork->state = MDSTATE_TRYLOGIN;

  case MDSTATE_TRYLOGIN:
    // ���O�C������������܂ő҂B
    DWC_ProcessFriendsMatch();
    break;

  case MDSTATE_UPDATESERVERS:
     if(_dWork->pFriendData){
       // �F�ؐ����A�v���t�@�C��ID�擾
       result = DWC_UpdateServersAsync(NULL, //�i�ߋ��Ƃ̌݊����̂��߁A�K��NULL)
                                       UpdateServersCallback, _dWork->pUserData,
                                       FriendStatusCallback, &_dWork,
                                       DeleteFriendListCallback, &_dWork);
       
       if (result == FALSE){
         // �Ă�ł�������ԁi���O�C�����������Ă��Ȃ���ԁj�ŌĂ񂾎��̂�
         // FALSE���Ԃ��Ă���̂ŁA���ʂ�TRUE
         MYDWC_DEBUGPRINT("DWC_UpdateServersAsync error teminated.\n");
//         GFL_NET_StateSetError(GFL_NET_ERROR_RESET_SAVEPOINT);
         GFL_NET_StateSetWifiError( 0, 0, 0, ERRORCODE_HEAP );  //�G���[�ɂȂ�
       }
       else{
         // GameSpy�T�[�o��o�f�B�����R�[���o�b�N��o�^����
         DWC_SetBuddyFriendCallback(BuddyFriendCallback, NULL);
         DWC_ProcessFriendsMatch();
       }
     //  _CHANGE_STATE(MDSTATE_UPDATESERVERSASYNC);
     }
    _CHANGE_STATE(MDSTATE_UPDATESERVERSASYNC);
//     else{
//       _CHANGE_STATE(MDSTATE_LOGIN);
//     }
    break;
  case MDSTATE_UPDATESERVERSASYNC:
    DWC_ProcessFriendsMatch();
    if(_dWork->pFriendData==NULL){
      _CHANGE_STATE(MDSTATE_LOGIN);
    }
    break;

  case MDSTATE_LOGIN:
    DWC_ProcessFriendsMatch();   // 2006.04.07 k.ohno ���O�C�����������̏�Ԃ����������
    _dWork->stepMatchResult = STEPMATCH_CONNECT;
    return STEPMATCH_CONNECT;
  }
  return mydwc_HandleError();
}

//==============================================================================
/**
 * ��M�����Ƃ��̃R�[���o�b�N��ݒ肷��֐�
 * @param client �c �N���C�A���g�Ƃ��ẴR�[���o�b�N�֐�
 * @param server �c �T�[�o�Ƃ��ẴR�[���o�b�N�֐�
 * @retval  none
 */
//==============================================================================
void GFL_NET_DWC_setReceiver( GFL_NET_MYDWCReceiverFunc server, GFL_NET_MYDWCReceiverFunc client )
{
  _dWork->serverCallback = server;
  _dWork->clientCallback = client;
}

//==============================================================================
/**
 * �ؒf�����m�������ɌĂ΂��֐����Z�b�g����
 * @param 	pFunc  �ؒf���ɌĂ΂��֐�
 * @param 	pWork  ���[�N�G���A
 * @retval  none
 */
//==============================================================================
void GFL_NET_DWC_SetDisconnectCallback( MYDWCDisconnectFunc pFunc, void* pWork )
{
  _dWork->disconnectCallback =pFunc;
  _dWork->pDisconnectWork =pWork;
}


//==============================================================================
/**
 * @brief   �ڑ��̃R�[���o�b�N���Ŕ��肷��֐��Z�b�g
 * @param 	pFunc  �ڑ����ɌĂ΂��֐�
 * @retval  none
 */
//==============================================================================
void GFL_NET_DWC_SetConnectModeCheckCallback( GFL_NET_MYDWCConnectModeCheckFunc pFunc )
{
  _dWork->connectModeCheck = pFunc;
}

//==============================================================================
/**
 * �ڑ������m�������ɌĂ΂��֐����Z�b�g����
 * @param 	pFunc  �ڑ����ɌĂ΂��֐�
 * @param 	pWork  ���[�N�G���A
 * @retval  none
 */
//==============================================================================
void GFL_NET_DWC_SetConnectCallback( MYDWCConnectFunc pFunc, void* pWork )
{
  _dWork->connectCallback =pFunc;
  _dWork->pConnectWork =pWork;
}


//------------------------------------------------------------------------------
/**
 * @brief   �}�b�`���C�N�ŁA���ł��Ă���O���[�v�ɐV���ɕʂ̃N���C�A���g���ڑ����J�n�������ɌĂ΂��
 * @param   index  �V�K�ڑ��N���C�A���g�̗F�B���X�g�C���f�b�N�X�B
                   �V�K�ڑ��N���C�A���g���F�B�łȂ����-1�ƂȂ�B
 * @param 	param  	�R�[���o�b�N�p�p�����[�^
 * @retval  none
 */
//------------------------------------------------------------------------------


static void RandomNewClientCallback(int index,void *param)
{
}

//------------------------------------------------------------------------------
/**
 * @brief   �V�K�ڑ��N���C�A���g�̎Q�������肷��ŏI�i�K�ŁA�A�v���P�[�V�������ɔ��f�����߂邽�߂ɌĂяo�����R�[���o�b�N�ł��B
 * @param   newClientUserData
             �V�K�ڑ��N���C�A���g���A
            DWC_ConnectToAnybodyAsync/DWC_ConnectToFriendsAsync/DWC_SetupGameServer/
            DWC_ConnectToGameServerAsync/DWC_ConnectToGameServerByGroupID
            �֐���connectionUserData�ɐݒ肵���l���i�[���Ă���o�b�t�@�ւ̃|�C���^�B
            u8[DWC_CONNECTION_USERDATA_LEN]���̃T�C�Y�B
 * @param 	param  	�R�[���o�b�N�p���[�N
 * @retval  TRUE  	�V�K�ڑ��N���C�A���g���󂯓����B
 * @retval  FALSE 	�V�K�ڑ��N���C�A���g�̎󂯓�������ۂ���B
 */
//------------------------------------------------------------------------------


static BOOL attemptCallback(u8 *newClientUserData,void *param)
{
  return _dWork->bConnectEnable;
}

//==============================================================================
/**
 * �����_���ΐ���s���֐��B�����_���}�b�`���͐��\�t���[���������Ԃ��Ă��Ȃ����Ƃ�����B
 * @retval  ���c�����B�O�c���s�B
 */
//==============================================================================
//-----#if TESTOHNO
const static char randommatch_query[] = "%s = \'%s\'";


#define PPW_LOBBY_MATCHMAKING_KEY  "a"  //@@OO����

int GFL_NET_DWC_StartMatch( u8* keyStr,int numEntry, BOOL bParent, u32 timelimit )
{



  GF_ASSERT_HEAVY( _dWork != NULL );

  if( _dWork->state != MDSTATE_LOGIN )
  {
    return 0;
  }
  mydwc_releaseRecvBuffAll();


  /*   { //@@OO TWL�ɂ͊֐������������̂ō폜
   *     int result;
   *     DWCMatchOptMinComplete moc={TRUE, 2, {0,0}, 0};
   *     moc.timeout = timelimit*1000;
   *     //        DWCMatchOptMinComplete moc={TRUE, 2, {0,0}, _RANDOMMATCH_TIMEOUT*1000};
   *     //	    if(!bParent){
   *     //            moc.timeout = 1; // �q�@�Ɏ��Ԃ̎哱�����Ȃ��悤�ɒZ���ݒ肷��
   *     //        }
   *     result = DWC_SetMatchingOption(DWC_MATCH_OPTION_MIN_COMPLETE,&moc,sizeof(moc));
   *     GF_ASSERT_HEAVY( result == DWC_SET_MATCH_OPT_RESULT_SUCCESS );
   *   }
   */
  {
    BOOL ret = DWC_AddMatchKeyString(0,PPW_LOBBY_MATCHMAKING_KEY,(const char *)keyStr);
    GF_ASSERT_HEAVY(ret!=0);
  }
  {
    MI_CpuClear8(_dWork->randommatch_query,_MATCHSTRINGNUM);
    sprintf((char*)_dWork->randommatch_query,randommatch_query,PPW_LOBBY_MATCHMAKING_KEY,keyStr);
    GF_ASSERT_HEAVY(GFL_STD_StrLen((const char*)_dWork->randommatch_query) < _MATCHSTRINGNUM);
  }
  if(bParent){
    DWC_AddMatchKeyString(1,(const char*)_dWork->randommatch_query,(const char*)_dWork->randommatch_query);
  }
  {
    int i;
    for(i=0;i<numEntry; i++){
      mydwc_allocRecvBuff(i);
    }
  }

  _CHANGE_STATE(MDSTATE_MATCHING);
  // _dWork->state = MDSTATE_MATCHING;

  MYDWC_DEBUGPRINT("mydwc_startmatch %d ",numEntry);
  _dWork->maxConnectNum = numEntry;

  if(FALSE==DWC_ConnectToAnybodyAsync
     (
       DWC_TOPOLOGY_TYPE_FULLMESH,
       numEntry,
       (const char*)_dWork->randommatch_query,
       ConnectToAnybodyCallback,
       GFL_NET_GetWork(),
       RandomNewClientCallback,
       GFL_NET_GetWork(),
       EvaluateAnybodyCallback,
       GFL_NET_GetWork(),
       attemptCallback,
       (u8*)&_dWork->connectionUserData,
       GFL_NET_GetWork()
       )){
    return 0;
  }
  _dWork->matching_type = MDTYPE_RANDOM;
  // ���M�R�[���o�b�N�̐ݒ�
  DWC_SetUserSendCallback( SendDoneCallback,NULL );

  // ��M�R�[���o�b�N�̐ݒ�
  DWC_SetUserRecvCallback( UserRecvCallback,NULL );

  // �R�l�N�V�����N���[�Y�R�[���o�b�N��ݒ�
  DWC_SetConnectionClosedCallback(ConnectionClosedCallback, NULL);

  // �^�C���A�E�g�R�[���o�b�N�̐ݒ�
  DWC_SetUserRecvTimeoutCallback( recvTimeoutCallback,NULL );

  _dWork->sendbufflag = 0;

  _dWork->closedflag = TRUE;	// 080602 tomoya
  return 1;
}
//----#endif //TESTOHNO


//==============================================================================
/**
 * �����_���ΐ���s���֐��B�����_���}�b�`���͐��\�t���[���������Ԃ��Ă��Ȃ����Ƃ�����B
 * @retval  ���c�����B�O�c���s�B
 */
//==============================================================================

int GFL_NET_DWC_StartMatchFilter( char* filterStr,int numEntry,DWCEvalPlayerCallback evalcallback,void* pWork)
{
  GF_ASSERT_HEAVY( _dWork != NULL );
  if( _dWork->state != MDSTATE_LOGIN )
  {
    return 0;
  }
  mydwc_releaseRecvBuffAll();
  {
    int i;
    for(i=0;i<numEntry; i++){
      mydwc_allocRecvBuff(i);
    }
  }

  _CHANGE_STATE(MDSTATE_MATCHING);
  _dWork->maxConnectNum = numEntry;
  if(FALSE==DWC_ConnectToAnybodyAsync
     (
       DWC_TOPOLOGY_TYPE_STAR,
       numEntry,
       (const char*)filterStr,
       ConnectToAnybodyCallback,
       NULL,
       RandomNewClientCallback,
       NULL,
       evalcallback,
       pWork,
       NULL,
       NULL,
       NULL
       )){
    return 0;
  }
  _dWork->matching_type = MDTYPE_RANDOM;
  // ���M�R�[���o�b�N�̐ݒ�
  DWC_SetUserSendCallback( SendDoneCallback,NULL );

  // ��M�R�[���o�b�N�̐ݒ�
  DWC_SetUserRecvCallback( UserRecvCallback,NULL );

  // �R�l�N�V�����N���[�Y�R�[���o�b�N��ݒ�
  DWC_SetConnectionClosedCallback(ConnectionClosedCallback, NULL);

  // �^�C���A�E�g�R�[���o�b�N�̐ݒ�
  DWC_SetUserRecvTimeoutCallback( recvTimeoutCallback,NULL );

  _dWork->sendbufflag = 0;

  _dWork->closedflag = TRUE;	// 080602 tomoya

  _dWork->stepMatchResult = STEPMATCH_CONNECT;
  return 1;
}


// 2006.7.4 yoshihara �ǉ�
static void finishcancel()
{
  if( _dWork->state == MDSTATE_FAIL ){
    _CHANGE_STATE(MDSTATE_FAILFINISH);
    //_dWork->state = MDSTATE_FAILFINISH;
  }
  else
  {
    _CHANGE_STATE(MDSTATE_CANCELFINISH);
    //  _dWork->state = MDSTATE_CANCELFINISH;
  }
}

//==============================================================================
/**
 * �����_���ΐ�}�b�`���O���ɖ��t���[���Ă΂��֐��B
 * @retval
 	STEPMATCH_CONTINUE�c�}�b�`���O��
 	STEPMATCH_SUCCESS�c����
 	STEPMATCH_CANCEL�c�L�����Z��
 	STEPMATCH_FAIL  �c���肪�e����߂����߁A�ڑ��𒆒f
 	���c�G���[����
 */
//==============================================================================

int GFL_NET_DWC_stepmatch( int isCancel )
{
  _sendAckMain();


  switch ( _dWork->state ){
  case MDSTATE_INIT:
  case MDSTATE_CONNECTING:
  case MDSTATE_CONNECTED:
  case MDSTATE_TRYLOGIN:
  case MDSTATE_UPDATESERVERS:
  case MDSTATE_UPDATESERVERSASYNC:

    return GFL_NET_DWC_connect();
  case MDSTATE_MATCHING:
    // ���ݒT����
    if( isCancel )
    {
      _CHANGE_STATE(MDSTATE_CANCEL);
      // _dWork->state = MDSTATE_CANCEL;
    }
    // 2006.7.4 yoshihara�C�� ��������
    // �q�@�̏ꍇ�A�Ȃ��ɂ����Ă�e�����݂��T�[�o���[�h���ǂ����`�F�b�N����B
    if( _dWork->matching_type == MDTYPE_CHILD )
    {
      if( _dWork->friendindex >= 0 ) // ����if�͔O�̂���
      {
        if( _dWork->friend_status[_dWork->friendindex] != DWC_STATUS_MATCH_SC_SV )
        {
          MYDWC_DEBUGPRINT("���肪�T�[�o����߂Ă��܂����̂ŁA�L�����Z�����܂��B\n");
          // ���ɐe�łȂ��Ȃ��Ă��܂��Ă���B�L�����Z���ֈڍ�
          _CHANGE_STATE(MDSTATE_FAIL);
          // _dWork->state = MDSTATE_FAIL;
        }
      }
    }
    // 2006.7.4 yoshihara�C�� �����܂�

    break;

  case MDSTATE_CANCEL:
  case MDSTATE_FAIL:
    {
      DWC_CloseAllConnectionsHard();
      finishcancel();	  // �R�[���o�b�N����STATE��ς��Ȃ��悤�ɏC�������� RETURN�Ɋ֌W�Ȃ���ԕύX����
    }
    break;    //   06.05.12�ǉ�
  case MDSTATE_MATCHED:
    // �����B
    {
      GFLNetInitializeStruct* pNetInit = GFL_NET_GetNETInitStruct();
      if(_dWork->bVChat){
        _DWC_StartVChat(pNetInit->netHeapID);
      }
      _dWork->myvchaton = 0;

      _CHANGE_STATE(MDSTATE_PLAYING);
      //      _dWork->state = MDSTATE_PLAYING;
      _dWork->stepMatchResult = STEPMATCH_SUCCESS;
      return STEPMATCH_SUCCESS;//_dWork->stepMatchResult;
    }
    break;
  case MDSTATE_CANCELFINISH:
    // ���O�C������̏�Ԃ�
    _CHANGE_STATE(MDSTATE_LOGIN);
    //    _dWork->state = MDSTATE_LOGIN;
    _dWork->sendbufflag = 0;
    _dWork->newFriendConnect = -1;
    MYDWC_DEBUGPRINT("�L�����Z����������\n");
    _dWork->stepMatchResult = STEPMATCH_CANCEL;
    return STEPMATCH_CANCEL;
  case MDSTATE_FAILFINISH:
    // ���O�C������̏�Ԃ�
    _CHANGE_STATE(MDSTATE_LOGIN);
    //    _dWork->state = MDSTATE_LOGIN;
    _dWork->sendbufflag = 0;
    _dWork->newFriendConnect = -1;
    MYDWC_DEBUGPRINT("�t�B�j�b�V����������\n");
    _dWork->stepMatchResult=STEPMATCH_FAIL;
    return STEPMATCH_FAIL;
  case MDSTATE_ERROR:
    return  mydwc_HandleError();
  case MDSTATE_DISCONNECTTING:
    // �{�C�X�`���b�g�̏I����҂��Ă�����
    if( _dWork->isvchat == 0 ) {
      // �{�C�X�`���b�g�̐ؒf�����B
      // �ʐM���̂�؂�B
      MYDWC_DEBUGPRINT("�ؒf����\n");
      DWC_CloseAllConnectionsHard( );
      _CHANGE_STATE(MDSTATE_DISCONNECT);
      // _dWork->state = MDSTATE_DISCONNECT;
      break;
    }
  case MDSTATE_LOGIN:
    {
      int ret = mydwc_step();
      if(0==ret){
        _dWork->stepMatchResult = STEPMATCH_CONNECT;
        return STEPMATCH_CONNECT;
      }
      return ret;
    }
    break;
  default:
    break;
    //			return handleError();
  }
  return mydwc_step();

  //    DWC_ProcessFriendsMatch();  // DWC�ʐM�����X�V
  //	return handleError();
}



//==============================================================================
/**
 * �T�[�o�Ƀf�[�^���M���s�����Ƃ��ł��邩
 * @retval  1 - �����@ 0 - ���s�i���M�o�b�t�@���l�܂��Ă��铙�j
 */
//==============================================================================
BOOL GFL_NET_DWC_canSendToServer()
{
  return ( DWC_GetMyAID() == 0 || ( _dWork->sendbufflag == 0 && _isSendableReliable() ) );
}

//==============================================================================
/**
 * �N���C�A���g�Ƀf�[�^���M���s�����Ƃ��ł��邩
 * @retval  1 - �����@ 0 - ���s�i���M�o�b�t�@���l�܂��Ă��铙�j
 */
//==============================================================================
BOOL GFL_NET_DWC_canSendToClient()
{
  return ( _dWork->sendbufflag == 0 && _isSendableReliable() );
}

//==============================================================================
/**
 * �T�[�o�Ƀf�[�^���M���s���֐�
 * @param   data - ���M����f�[�^�ւ̃|�C���^�Bsize - ���M����f�[�^�̃T�C�Y
 * @retval  1 - �����@ 0 - ���s�i���M�o�b�t�@���l�܂��Ă��铙�j
 */
//==============================================================================
int GFL_NET_DWC_sendToServer(void *data, int size)
{
  if( !(size < SIZE_SEND_BUFFER) ){
    return 0;
  }
  if(FALSE == GFL_NET_DWC_canSendToServer()){
    return 0;
  }
  if(!_dWork->sendFinish){
    return 0;
  }

  MYDWC_DEBUGPRINT("mydwc_sendToServer(data=%d)\n", *((u32*)data));

  if( DWC_GetMyAID() == 0 )
  {
    // �������e
    // �R�[���o�b�N���ŏ�����������\�����l���Adata���R�s�[���Ă����B
    //		void *buf = mydwc_AllocFunc( NULL, size, 32 );
    //		MI_CpuCopy8	( data, buf, size );

    // �������g�̃T�[�o��M�R�[���o�b�N���Ăяo���B
    if( _dWork->serverCallback != NULL ) _dWork->serverCallback(0, data, size);

    return 1;
  }
  else
  {
    // ���肪�e�B����ɑ΂��ăf�[�^���M�B
    if( _dWork->sendbufflag || !_isSendableReliable() ) // ���M�o�b�t�@���`�F�b�N�B
    {
      // ���M�o�b�t�@�������ς��Ȃǂő���Ȃ��B
      return 0;
    }

    // ���M�o�b�t�@�ɃR�s�[
    *((u32*)&(_dWork->sendBuffer[0])) = MYDWC_GAME_PACKET | (_dWork->myvchaton << MYDWC_PACKET_VCHAT_SHIFT);
    _dWork->sendBuffer[MYDWC_PACKET_SEQNO_POS] = ++_dWork->myseqno;
    MI_CpuCopy8( data, &(_dWork->sendBuffer[4]), size );
    _dWork->sendbufflag = 1;

    // �e�@�Ɍ����Ă̂ݑ��M
    DWC_SendReliableBitmap( 0x01, &(_dWork->sendBuffer[0]), size + 4);

    _dWork->sendFinish=FALSE;

    //		MYDWC_DEBUGPRINT("-");
    //OHNO_PRINT("-");
    return 1;
  }
}

//==============================================================================
/**
 * �e�@���q�@�Ƀf�[�^���M���s���֐�
 * @param   data - ���M����f�[�^�ւ̃|�C���^�Bsize - ���M����f�[�^�̃T�C�Y
 * @retval  1 - �����@ 0 - ���s�i���M�o�b�t�@���l�܂��Ă��铙�j
 */
//==============================================================================
int GFL_NET_DWC_sendToClient(void *data, int size)
{
  if( !(size < SIZE_SEND_BUFFER) ){
    return 0;
  }
  // �e�@�������̓���͍s��Ȃ��͂��B
  if( !(DWC_GetMyAID() == 0) ){
    return 0;
  }

  MYDWC_DEBUGPRINT("sendToClient(data=%d)\n", *((u32*)data));

  {
    // ����ɑ΂��ăf�[�^���M�B
    if( _dWork->sendbufflag || !_isSendableReliable() ) // ���M�o�b�t�@���`�F�b�N�B
    {
      //            MYDWC_DEBUGPRINT("wifi failed %d %d\n",_dWork->sendbufflag,_isSendableReliable());
      // ���M�o�b�t�@�������ς��Ȃǂő���Ȃ��B
      return 0;
    }
    if(!_dWork->sendFinish){
      return 0;
    }

    // ���M�o�b�t�@�ɃR�s�[
    *((u32*)&(_dWork->sendBuffer[0])) = MYDWC_GAME_PACKET | (_dWork->myvchaton << MYDWC_PACKET_VCHAT_SHIFT);
    _dWork->sendBuffer[MYDWC_PACKET_SEQNO_POS] = ++_dWork->myseqno;
    MI_CpuCopy8( data, &(_dWork->sendBuffer[4]), size );
    _dWork->sendbufflag = 1;

    // �q�@�Ɍ����đ��M
    //   MYDWC_DEBUGPRINT("wifi send %z\n",DWC_GetAIDBitmap());
    if(!DWC_SendReliableBitmap(DWC_GetAIDBitmap(), &(_dWork->sendBuffer[0]), size + 4)){
      MYDWC_DEBUGPRINT("wifi SCfailed %d\n",size);
      // ���M�o�b�t�@�������ς��Ȃǂő���Ȃ��B
      _dWork->sendbufflag = 0;
      return 0;
    }
    _dWork->sendFinish=FALSE;
    //		MYDWC_DEBUGPRINT("-");
    //OHNO_PRINT(".");
  }

  {
    // �����ɑ΂��đ��M
    //		// �R�[���o�b�N���ŏ�����������\�����l���Adata���R�s�[���Ă����B
    //		void *buf = mydwc_AllocFunc( NULL, size, 32 );
    //		MI_CpuCopy8	( data, buf, size );

    // �������g�̃T�[�o��M�R�[���o�b�N���Ăяo���B
    if( _dWork->clientCallback != NULL ) _dWork->clientCallback(0, data, size);

  }

  return 1;
}





//==============================================================================
/**
 * @brief    ���M���s���֐� �x�[�X
 * @param   data - ���M����f�[�^�ւ̃|�C���^�Bsize - ���M����f�[�^�̃T�C�Y
 * @retval  1 - �����@ 0 - ���s�i���M�o�b�t�@���l�܂��Ă��铙�j
 */
//==============================================================================
static int _SendToBase(void *data, int size, int header)
{
  u16 bitmap;

  if( !(size < SIZE_SEND_BUFFER) ){
    return 0;
  }
  {
    // ����ɑ΂��ăf�[�^���M�B
    //   if( _dWork->sendbufflag || !_isSendableReliable() ) // ���M�o�b�t�@���`�F�b�N�B
    if( _dWork->sendbufflag ) // ���M�t���O���`�F�b�N�B
    {
      return 0;
    }

    // ���M�o�b�t�@�ɃR�s�[
    *((u32*)&(_dWork->sendBuffer[0])) = header | (_dWork->myvchaton << MYDWC_PACKET_VCHAT_SHIFT);
    _dWork->sendBuffer[MYDWC_PACKET_SEQNO_POS] = ++_dWork->myseqno;
    MI_CpuCopy8( data, &(_dWork->sendBuffer[4]), size );
    _dWork->sendbufflag = 1;

    bitmap = DWC_GetAIDBitmap();
    if(bitmap != DWC_SendReliableBitmap(bitmap, &(_dWork->sendBuffer[0]), size + 4)){
      MYDWC_DEBUGPRINT("wifi SOFailed %d\n",size);
      // ���M�o�b�t�@�������ς��Ȃǂő���Ȃ��B
      _dWork->sendbufflag = 0;
      return 0;
    }

  }
  return 1;
}


//==============================================================================
/**
 * ���̑���ɑ��M���s���֐�
 * @param   data - ���M����f�[�^�ւ̃|�C���^�Bsize - ���M����f�[�^�̃T�C�Y
 * @retval  1 - �����@ 0 - ���s�i���M�o�b�t�@���l�܂��Ă��铙�j
 */
//==============================================================================
static int _SendToAck(void *data, int size)
{
  u16 bitmap;

  if(0==_SendToBase(data, size, MYDWC_CHECK_PACKET)){
    return 0;
  }
  return 1;
}


//==============================================================================
/**
 * ���̑���ɑ��M���s���֐�
 * @param   data - ���M����f�[�^�ւ̃|�C���^�Bsize - ���M����f�[�^�̃T�C�Y
 * @retval  1 - �����@ 0 - ���s�i���M�o�b�t�@���l�܂��Ă��铙�j
 */
//==============================================================================
int GFL_NET_DWC_SendToOther(void *data, int size)
{
  if(!_dWork->sendFinish){
    //OS_TPrintf("sendFinish NONE\n");
    return 0;
  }
  if(0==_SendToBase(data, size, MYDWC_GAME_PACKET)){
    return 0;
  }
  //OS_TPrintf("++send data\n");
  _dWork->sendFinish=FALSE;
  // �������g�̎�M�R�[���o�b�N���Ăяo���B
  if( _dWork->clientCallback != NULL ){
    _dWork->clientCallback(DWC_GetMyAID() , data, size);
  }
  return 1;
}




/*---------------------------------------------------------------------------*

 *---------------------------------------------------------------------------*/
static void _sendAckMain(void)
{
  if(_dWork->sendAck){
    int data=0;
    if( _SendToAck(&data, 4)){
      //OS_TPrintf("++sendAck \n");
      _dWork->sendAck = FALSE;
    }
  }
}

/*---------------------------------------------------------------------------*
  ���O�C���R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void LoginCallback(DWCError error, int profileID, void *param)
{
  BOOL result;

  // stUserData���X�V����Ă��邩�ǂ������m�F�B
  if ( DWC_CheckDirtyFlag( _dWork->pUserData) )
  {
    // �K�����̃^�C�~���O�Ń`�F�b�N���āAdirty flag���L���ɂȂ��Ă�����A
    // DWCUserData��DS�J�[�h�̃o�b�N�A�b�v�ɕۑ�����悤�ɂ��Ă��������B
    DWCUserData *userdata = NULL;
    DWC_ClearDirtyFlag(_dWork->pUserData);
    _dWork->saveing = 1;  //�Z�[�u����1
    MYDWC_DEBUGPRINT("�����̃t�����h�R�[�h���ύX\n");
  }

  if (error == DWC_ERROR_NONE){
    NET_PRINT("MDSTATE_UPDATESERVERS \n");
    _CHANGE_STATE(MDSTATE_UPDATESERVERS);
  }
  else
  {
    // �F�؎��s
    _CHANGE_STATE(MDSTATE_ERROR);
  }
}

/*---------------------------------------------------------------------------*
  �^�C���A�E�g�R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void recvTimeoutCallback(u8 aid, void* param)
{
  MYDWC_DEBUGPRINT("DWC�^�C���A�E�g - %d",aid);
  if(aid >= 2){
    return;
  }
  // �R�l�N�V���������
  if( _dWork->timeoutflag )
  {
    MYDWC_DEBUGPRINT("�ڑ���ؒf���܂�\n");
    DWC_CloseAllConnectionsHard( );
    _dWork->newFriendConnect = -1;
    // �^�C���A�E�g
    _CHANGE_STATE(MDSTATE_TIMEOUT);
    //   _dWork->state = MDSTATE_TIMEOUT;
  }
}

/*---------------------------------------------------------------------------*
  GameSpy�T�[�o�A�b�v�f�[�g�R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void UpdateServersCallback(DWCError error, BOOL isChanged, void* param)
{
#pragma unused(param)
  if (error == DWC_ERROR_NONE){
    if (isChanged){
      // �F�B���X�g���ύX����Ă���
    }
    _CHANGE_STATE(MDSTATE_LOGIN);
    //  _dWork->state = MDSTATE_LOGIN;		// ���O�C������
  }
  else {
    // ���O�C�����s�����ɂ��Ƃ��H
    _CHANGE_STATE(MDSTATE_ERROR);
    //   _dWork->state = MDSTATE_ERROR;
  }
}


/*---------------------------------------------------------------------------*
  �F�B��ԕω��ʒm�R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void FriendStatusCallback(int index, u8 status, const char* statusString, void* param)
{
#pragma unused(param)

  MYDWC_DEBUGPRINT("Friend[%d] changed status -> %d (statusString : %s).\n",
                   index, status, statusString);

}


/*---------------------------------------------------------------------------*
  �F�B���X�g�폜�R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void DeleteFriendListCallback(int deletedIndex, int srcIndex, void* param)
{
#pragma unused(param)
  int i;
  GFLNetInitializeStruct* pNetInit = GFL_NET_GetNETInitStruct();

  // �t�����h���X�g���폜���ꂽ�B
  MYDWC_DEBUGPRINT("friend[%d] was deleted (equal friend[%d]).\n", deletedIndex, srcIndex);

  for(i = 0;i < FRIENDLIST_MAXSIZE; i++){
    if(_dWork->deletedIndex[i] == -1){
      _dWork->deletedIndex[i] = deletedIndex;
      _dWork->srcIndex[i] = srcIndex;
    }
  }
}


/*---------------------------------------------------------------------------*
  GameSpy�o�f�B�����R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void BuddyFriendCallback(int index, void* param)
{
#pragma unused(param)

  MYDWC_DEBUGPRINT("I was authorized by friend[%d].\n", index);
}

/*---------------------------------------------------------------------------*
  �^�C���A�E�g���Ԃ̃N���A
 *---------------------------------------------------------------------------*/
static void clearTimeoutBuff(void)
{
  MI_CpuClear8(_dWork->sendintervaltime, sizeof(_dWork->sendintervaltime));
}
/*---------------------------------------------------------------------------*
  �^�C���A�E�g���Ԃ̐ݒ�
 *---------------------------------------------------------------------------*/
static void setTimeoutTime(void)
{
  int i;
  BOOL ret;

  for(i = 0 ;i < _WIFI_NUM_MAX; i++){
    DWC_SetRecvTimeoutTime( i, 0 );
  }


  if(DWC_GetMyAID() == _WIFI_PARENT_AID){
    for(i = 0 ;i < _dWork->maxConnectNum; i++){
      if(DWC_GetMyAID() != i){
        if(DWC_GetAIDBitmap() & (0x01<<i) ){
          ret = DWC_SetRecvTimeoutTime( i, MYDWC_TIMEOUTSEC * 1000 );
          GF_ASSERT_MSG_HEAVY(ret,"DWC_SetRecvTimeoutTime\n");
          MYDWC_DEBUGPRINT("setTimeOut %d\n",i);
        }
      }
    }
  }
  else{
    ret = DWC_SetRecvTimeoutTime( _WIFI_PARENT_AID, MYDWC_TIMEOUTSEC * 1000 );
    GF_ASSERT_MSG_HEAVY(ret,"DWC_SetRecvTimeoutTime\n");
    MYDWC_DEBUGPRINT("setTimeOut 0\n");
  }

  // 080523 tomoya takahashi
  // BTS:�ʐMNo.167
  // �e���牽����M���Ȃ���timeoutflag�������Ȃ����߁A
  // �q�͂��܂ł��^�C���A�E�g��ԂɂȂ�Ȃ��\��������B
  // ���ׁ̈A�}�b�`���O������������A������timeoutflag�����悤��
  // �ύX���Ă݂�B
  // _dWork->timeoutflag = 0;	<-���X
  _dWork->timeoutflag = 1;
  clearTimeoutBuff();
}


static void setTimerAndFlg(int index)
{
  int i,j;

 // OS_TPrintf("�ڑ����܂���%s%d\n",__FILE__,__LINE__);
  //  _dWork->state = MDSTATE_MATCHED;
  _CHANGE_STATE(MDSTATE_MATCHED);

  setTimeoutTime();
}

/*---------------------------------------------------------------------------*
  �F�B���w��ڑ��R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void ConnectToAnybodyCallback(DWCError error,
                                     BOOL cancel,
                                     BOOL self,
                                     BOOL isServer,
                                     int index,
                                     void *param)
{
#pragma unused(param)

  if (error == DWC_ERROR_NONE){
    if (!cancel){
      // ���m��ʐl�����Ƃ̃R�l�N�V�����ݗ�����
      MYDWC_DEBUGPRINT("�ڑ����� %d\n",index);
      setTimerAndFlg(index);
    }
    else {
      MYDWC_DEBUGPRINT("�L�����Z������\n");
      // ���O�C�����Ԃɖ߂�

      // 2006.7.4 yoshihara �C��
      finishcancel();
    }
  }
  else {
    MYDWC_DEBUGPRINT("�}�b�`���O���ɃG���[���������܂����B %d\n\n", error);
    //    _dWork->state = MDSTATE_ERROR;
    _CHANGE_STATE(MDSTATE_ERROR);
  }
  if( _dWork->connectCallback ){
    // ���l�ƂȂ����Ă���񂵂��Ă΂�Ȃ��̂Ŏ�����ID�����Ă���
    _dWork->connectCallback(DWC_GetMyAID(), _dWork->pConnectWork);
  }
}

/*---------------------------------------------------------------------------*
  �F�B���w��}�b�`���O���v���C���[�]���R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static int  EvaluateAnybodyCallback(int index, void* param)
{
#pragma unused(index,param)

  return 1;
}


/** -------------------------------------------------------------------------
  ���M�����R�[���o�b�N
  ---------------------------------------------------------------------------*/
static void SendDoneCallback( int size, u8 aid,void* param )
{
#pragma unused(size)
  if(aid >= 2){
    return;
  }
  // ���M�o�b�t�@��������
  _dWork->sendbufflag = 0;
  _dWork->sendintervaltime[aid] = 0;
  //	MYDWC_DEBUGPRINT( "���M\n" );
  //	MYDWC_DEBUGPRINT( "*" );
}



static void _setOpVchat(u32 topcode)
{

  //  if(!CommLocalIsWiFiQuartetGroup(CommStateGetServiceNo())){
  if( topcode & MYDWC_PACKET_VCHAT_MASK )
  {
    // ����̃{�C�X�`���b�g�̓I��
    _dWork->opvchaton = 1;
  }
  else
  {
    // ����̃{�C�X�`���b�g�̓I��
    _dWork->opvchaton = 0;
  }
  //    }
}



/** -------------------------------------------------------------------------
  ��M�����R�[���o�b�N
  ---------------------------------------------------------------------------*/
static void UserRecvCallback( u8 aid, u8* buffer, int size,void* param )
{
  u32 topcode;
  topcode = (buffer[3] << 24) | (buffer[2] << 16) | (buffer[1] << 8) | buffer[0];

  //	MYDWC_DEBUGPRINT("[%d,%d,%d,%d]", buffer[0], buffer[1], buffer[2], buffer[3]);
  //  MYDWC_DEBUGPRINT("-��M-\n");
  GF_ASSERT_HEAVY(aid < 2);  //BTS 3869 ���C�u��������0xff���A���Ă��Ă�̂��m�F������
  if(aid >= 2){
    return;
  }

  // ��x��M���Ă͂��߂ă^�C���A�E�g��ݒ肷��B
  _dWork->timeoutflag = 1;

  // �܂��A�擪�̂S�o�C�g���݂āA�Q�[���̏�񂩂ǂ����𔻒f
  if( (topcode & MYDWC_PACKETYPE_MASK) == MYDWC_GAME_PACKET ){
    _setOpVchat( topcode );
    _dWork->opseqno = buffer[MYDWC_PACKET_SEQNO_POS];
    _dWork->sendAck = TRUE;
  }
  else if( (topcode & MYDWC_PACKETYPE_MASK) == MYDWC_CHECK_PACKET ){  //arc return�R�}���h
    _dWork->sendFinish = TRUE;
    return;
  }
  else {
    if( myvct_checkData( aid, buffer,size ) ) return;
    // ���Ӗ��ȏ��i�R�l�N�V������ێ����邽�߂̂��̂Ǝv����j
    return;
  }

  {
    u16 *temp = (u16*)buffer + 2;

    if( DWC_GetMyAID() == 0 )
    {
      // �������e�̏ꍇ�c�N���C���g����T�[�o�ɑ΂��đ����Ă������̂Ɣ��f�B
      // �T�[�o�p��M�֐����Ăяo���B
      if( _dWork->serverCallback != NULL ) _dWork->serverCallback(aid, temp, size-4);
    } else {
      // �T�[�o����N���C�A���g�ɑ΂��đ����Ă������̂Ɣ��f�B
      if( _dWork->clientCallback != NULL ){
        _dWork->clientCallback(aid, temp, size-4);
      }
    }
  }
}

/*---------------------------------------------------------------------------*
  �R�l�N�V�����N���[�Y�R�[���o�b�N�֐�
error  	DWC�G���[��ʁB���e�̏ڍׂ́ADWC_GetLastErrorEx���Q�Ƃ��Ă��������B
isLocal
TRUE:�������N���[�Y�����B
FALSE:���̒N�����N���[�Y�����B �}�b�`���C�N���ɉ����̂Ȃ��z�X�g��ؒf���鎞��TRUE�ɂȂ�B
isServer
TRUE: �T�[�o�N���C�A���g�}�b�`���C�N�̃T�[�oDS���N���[�Y�����B�������T�[�oDS��isLocal = TRUE �̎���TRUE�ɂȂ�B
FALSE:����ȊO�B�T�[�o�N���C�A���g�}�b�`���C�N�ȊO�ł͕K��FALSE�ƂȂ�B
aid 	�N���[�Y�����v���C���[��AID
index 	�N���[�Y�����v���C���[�̗F�B���X�g�C���f�b�N�X�B�N���[�Y�����v���C���[���F�B�łȂ����-1�ƂȂ�B
param 	DWC_SetConnectionClosedCallback�֐��Ŏw�肵���R�[���o�b�N�p�p�����[�^
 *---------------------------------------------------------------------------*/
static void ConnectionClosedCallback(DWCError error,
                                     BOOL isLocal,
                                     BOOL isServer,
                                     u8  aid,
                                     int index,
                                     void* param)
{
#pragma unused(param, index)
  MYDWC_DEBUGPRINT("ConnectionClosedCallback %d %d %d %d %d\n",error,isLocal, isServer,aid,_dWork->closedflag);

  if(aid >= 2){
    return;
  }
  
  _dWork->sendbufflag = 0;
  _dWork->setupErrorCount = 0;
  _dWork->bConnectCallback = FALSE;

  MYDWC_DEBUGPRINT("ConnectionClosedCallback\n");
  if (error == DWC_ERROR_NONE){

    if(((_dWork->closedflag == TRUE) && (DWC_GetNumConnectionHost() == 1))){
      // �S�ẴR�l�N�V�������N���[�Y���ꂽ�ꍇ
      // �L�����Z�����Ȃ�΁A�L�����Z�������������B
      if(	_dWork->state == MDSTATE_CANCEL )
      {
        MYDWC_DEBUGPRINT("MDSTATE_CANCELFINISH\n");
      }
      else {
        _CHANGE_STATE(MDSTATE_DISCONNECTTING);
        MYDWC_DEBUGPRINT("MDSTATE_DISCONNECTTING\n");
      }
      if( _dWork->isvchat )
      {
        // �܂��{�C�X�`���b�g���C�u������������Ă��Ȃ��ꍇ�B
        GFL_NET_DWC_StopVChat();
      }
    }
    if (isLocal){
      MYDWC_DEBUGPRINT("Closed connection to aid %d (friendListIndex = %d) Rest %d.\n",
                       aid, index, DWC_GetNumConnectionHost());
    }
    else {
      MYDWC_DEBUGPRINT("Connection to aid %d (friendListIndex = %d) was closed. Rest %d.\n",
                       aid, index, DWC_GetNumConnectionHost());
      _CHANGE_STATE(MDSTATE_DISCONNECTTING);
    }
  }
  else {
    MYDWC_DEBUGPRINT("Failed to close connections. %d\n\n", error);
  }

  if(_dWork->bAutoDisconnect){
    const GFL_NETSTATE_DWCERROR* cp_error =GFL_NET_StateGetWifiError();
    if( cp_error->errorUser == 0 )
    {
      GFL_NET_StateSetWifiError( 
        cp_error->errorCode, 
        cp_error->errorType, 
        cp_error->errorRet, 
        ERRORCODE_DISCONNECT );
    }
  }
  GFI_NET_HANDLE_Delete(aid);  // �n���h���폜
  if( _dWork->disconnectCallback )
  {
    _dWork->disconnectCallback(aid, _dWork->pDisconnectWork);
  }
  OS_TPrintf("�ؒf�R�[���o�b�N %d\n",_dWork->bAutoDisconnect);
}

//==============================================================================
/**
 * �G���[���N�����Ă��Ȃ����`�F�b�N���A�G���[�ɉ����ĒʐM��������鏈��������B
 *  ���E�����A���E�o�g���^���[�ŌĂяo�����߂ɑ������� k.ohno 06.06.08
 * @retval  �O�c�G���[�Ȃ��B���c�G���[  �� return�G���[
 */
//==============================================================================
int mydwc_HandleError(void)
{
  int errorCode;
  DWCErrorType myErrorType;
  int ret;
  int returnNo = 0;


  ret = DWC_GetLastErrorEx( &errorCode, &myErrorType );


  if( ret != 0 ){
    // ���炩�̃G���[�������B
    MYDWC_DEBUGPRINT("error occured!(%d, %d, %d)\n", ret, errorCode, myErrorType);

    if(_dWork->pDisconnectErrorCallback){
      _dWork->pDisconnectErrorCallback(_dWork->pDisconnectErrorUserWork, errorCode, myErrorType, ret);
    }

    
//    NHTTP_RAP_DisconnectCallbackCall(errorCode, myErrorType, ret);
    // DWC_GetLastErrorEx�̐����ɂ̂��Ƃ�  2008.5.23 -> 2010.1.5 nagihashi update
    // �Ԃ����̂͊�{�I��errorCode�ł��邪
    // �G���[�R�[�h�� 0 �̏ꍇ��G���[�����^�C�v�� DWC_ETYPE_LIGHT �̏ꍇ�́A�Q�[���ŗL�̕\���݂̂Ȃ̂� ret��Ԃ�
    // heap�G���[���������ł���ɕύX
    returnNo = errorCode;
    if((errorCode == 0) || (myErrorType == DWC_ETYPE_LIGHT)){
      returnNo = ret;
    }
    switch(myErrorType)
    {
    case DWC_ETYPE_LIGHT:
      // �Q�[���ŗL�̕\���݂̂ŁA�G���[�R�[�h�\���͕K�v�Ȃ��B
      // DWC_ClearError()���Ăяo���΁A���A�\�B
      DWC_ClearError();

      //�O����ClearError���Ă�dwc_rap������Ԃ�ERROR����ς�炸�A
      //�I�������ł͂܂�ꍇ�����邽�߁A�G���[��Ԃ̏ꍇ�͉�������
      if(_dWork){
        if( _dWork->state == MDSTATE_ERROR )
        {
          _CHANGE_STATE(MDSTATE_LOGIN);
        }
      }
      break;

    case DWC_ETYPE_SHOW_ERROR:  // �G���[�\�����K�v

      if( -40000 >= errorCode && errorCode >= -41999 )
      {
        DWC_GdbShutdown();           // �ȈՃf�[�^�x�[�X���C�u�����̏I��
      }
      if( -43000 >= errorCode && errorCode >= -44999 )
      {
        //DWC_ScShutdown(); //���܂��ŌĂԂ̂ŌĂ΂Ȃ�
      }

      //DWC_ClearError(); //
      //�O����ClearError���Ă�dwc_rap������Ԃ�ERROR����ς�炸�A
      //�I�������ł͂܂�ꍇ�����邽�߁A�G���[��Ԃ̏ꍇ�͉�������
      if(_dWork){
        if( _dWork->state == MDSTATE_ERROR )
        {
          _CHANGE_STATE(MDSTATE_LOGIN);
        }
      }
      break;

    case DWC_ETYPE_SHUTDOWN_GHTTP:
      //�ȉ��̊e���C�u��������֐���K�v�ɉ����ČĂяo���Ă��������B
      //DWC_RnkShutdown�֐�(�ėp�����L���O���C�u����)
      //�G���[�R�[�h��\�����Ă��������B
      //
      if( -40000 >= errorCode && errorCode >= -41999 )
      {
        DWC_GdbShutdown();           // �ȈՃf�[�^�x�[�X���C�u�����̏I��
      }
      if( -43000 >= errorCode && errorCode >= -44999 )
      {
        //DWC_ScShutdown(); //���܂��ŌĂԂ̂ŌĂ΂Ȃ�
      }

      //DWC_ClearError();
      break;

    case DWC_ETYPE_SHUTDOWN_ND:
      //DWC_NdCleanupAsync�֐����Ăяo���āA�_�E�����[�h���C�u�������I������K�v������܂��B
      //�G���[�R�[�h��\�����Ă��������B
      DWC_NdCleanupAsync();
      //DWC_ClearError();
      break;

    case DWC_ETYPE_SHUTDOWN_FM:
      // DWC_ShutdownFriendsMatch()���Ăяo���āAFriendsMatch�������I������K�v������B
      // �G���[�R�[�h�̕\�����K�v�B
      // ���̏ꍇ�ADWC_ETYPE_DISCONNECT�Ɠ������������Ă����B
    case DWC_ETYPE_DISCONNECT:
      //FriendsMatch�������Ȃ�DWC_ShutdownFriendsMatch()���Ăяo���A
      //�X��DWC_CleanupInet()�ŒʐM�̐ؒf���s���K�v������B
      //�G���[�R�[�h�̕\�����K�v�B

      MYDWC_DEBUGPRINT("Disconnect Error!!\n");

      if(_dWork){
        switch( _dWork->state )
        {
        case MDSTATE_TRYLOGIN:
        case MDSTATE_LOGIN:
        case MDSTATE_MATCHING:
        case MDSTATE_CANCEL:
        case MDSTATE_MATCHED:
        case MDSTATE_PLAYING:
        case MDSTATE_CANCELFINISH:
        case MDSTATE_ERROR:
        case MDSTATE_TIMEOUT:
        case MDSTATE_DISCONNECTTING:
        case MDSTATE_DISCONNECT:
          if( _dWork->friendMatchOn == TRUE){
            DWC_ShutdownFriendsMatch();
            _dWork->friendMatchOn=FALSE;
          }
        case MDSTATE_INIT:
        case MDSTATE_CONNECTING:
        case MDSTATE_CONNECTED:
          // �܂����̎��_�ł́A�t�����h�}�b�`���C�u�����͌Ă΂�Ă��Ȃ��B
          // WIFI�L��̏ꍇ�O�ŌĂ�
          //                    if(CommStateGetServiceNo() != COMM_MODE_LOBBY_WIFI){
          DWC_CleanupInet( );
          //                  }
        }

        //DWC_ClearError();
      }
      if(_dWork){
        //    _dWork->state = MDSTATE_ERROR_DISCONNECT;
        _CHANGE_STATE(MDSTATE_ERROR_DISCONNECT);
      }
      break;
    case DWC_ETYPE_FATAL:
      // FatalError�����Ȃ̂ŁA�d��OFF�𑣂��K�v������B
      if(_dWork){
        //        _dWork->state = MDSTATE_ERROR_FETAL;
        _CHANGE_STATE(MDSTATE_ERROR_FETAL);
        // ���̃R�[���o�b�N���珈�����������Ă��Ȃ��͂��B
        if( _dWork->fetalErrorCallback != NULL ){
          _dWork->fetalErrorCallback( -errorCode );
        }
      }
      break;
    }
  }
  if(_dWork->bHeapError){
    //���̂܂�returnNo������Ă��܂��ƁA
    //ERRORCODE_HEAP��TIMEOUT�ȊO�̃��m�������Ă��܂��̂�
    //���m�����炢���
    returnNo = ERRORCODE_HEAP;
    GFL_NET_StateSetWifiError( errorCode, myErrorType, ret, returnNo );
  }
  else if(_dWork->state==MDSTATE_TIMEOUT){
    MYDWC_DEBUGPRINT("TimeOut Error!! %d %d %d\n",errorCode,myErrorType,ret);
    returnNo = ERRORCODE_TIMEOUT;
    GFL_NET_StateSetWifiError( errorCode, myErrorType, ret, returnNo );
  }
  else if( ret != 0 ){
    GFL_NET_StateSetWifiError( errorCode, myErrorType, ret, 0 );
  }
  return returnNo;
}


//Reliable���M�\���ǂ������`�F�b�N����֐��ł��B

static BOOL _isSendableReliable(void)
{
  int i;
  BOOL bRet=FALSE;

  for(i=0;i< _dWork->maxConnectNum;i++){
    if(DWC_GetMyAID()==i){
      continue;
    }
    if(DWC_IsValidAID(i)){
      bRet=TRUE;  // ���M���肪�����ꍇTRUE
      if(!DWC_IsSendableReliable(i)){
        return FALSE;
      }
    }
  }
  return bRet;
}

/*
static int _buffer;

static void _sendData(int param)
{
	_buffer = param;
	if( !DWC_IsSendableReliable( _dWork->op_aid ) ) // ���M�o�b�t�@���`�F�b�N�B
	{
		DWC_SendReliable( _dWork->op_aid, &(_buffer), 4 );
	}
}
 */


static BOOL sendKeepAlivePacket(int i)
{
  if( (_dWork->sendbufflag == 0) && _isSendableReliable() && (0xfffe & DWC_GetAIDBitmap()) ) // ���M�o�b�t�@���`�F�b�N�B
  {
    MYDWC_DEBUGPRINT("SEND KEEP ALIVE PACKET  %d %d %d\n",_dWork->sendbufflag,_isSendableReliable() , DWC_GetAIDBitmap());
    _dWork->sendbufflag = 1;
    *((u32*)&(_dWork->sendBuffer[0])) = MYDWC_KEEPALIVE_PACKET | (_dWork->myvchaton << MYDWC_PACKET_VCHAT_SHIFT);

    DWC_SendReliableBitmap(DWC_GetAIDBitmap(), &(_dWork->sendBuffer[0]), 4);

    _dWork->sendintervaltime[i] = 0;
    //MYDWC_DEBUGPRINT("SEND KEEP ALIVE PACKET\n");
    //OHNO_PRINT("KEEP ALIVE %d\n",sys.vsync_counter);
    return TRUE;
  }
  return FALSE;
}


// �ʐM�m����A���t���[���Ăяo���Ă��������B
//==============================================================================
/**
 * DWC�ʐM�����X�V���s��
 * @param none
 * @retval 0�c����, ���c�G���[���� 1�c�^�C���A�E�g 2�c���肩��ؒf���ꂽ
 */
//==============================================================================
static int mydwc_step(void)
{
  int i,ret;
  int errorCode;
  DWCErrorType myErrorType;

  _FuncNonSave();  //�Z�[�u�f�[�^�̕��Ƀt�����h�f�[�^���ڂ��ς���

  DWC_ProcessFriendsMatch();  // DWC�ʐM�����X�V
  mydwc_updateFriendInfo();


  if( _dWork->isvchat )
  {

    if( (_dWork->myvchaton == 1) && (_dWork->opvchaton == 1) && (_dWork->myvchat_send == TRUE) )
    {
      myvct_main(FALSE);
    }
    else
    {
      myvct_main(TRUE);
    }

    if(_dWork->backupBitmap != DWC_GetAIDBitmap()){
      if(!_dWork->pausevchat && _dWork->bVChat){
        if(myvct_AddConference(DWC_GetAIDBitmap(), DWC_GetMyAID())){
          _dWork->backupBitmap = DWC_GetAIDBitmap();
        }
      }
    }
  }

  if( _dWork->state == MDSTATE_TIMEOUT ){  // �^�C���A�E�g�X�e�[�g�̎��͓����ɃG���[���Ď�����
    ret = mydwc_HandleError();
    if(ret != 0){
      return ret;
    }
    return STEPMATCH_TIMEOUT;
  }
  if( _dWork->state == MDSTATE_DISCONNECT ) return STEPMATCH_DISCONNECT;

  if( _dWork->state == MDSTATE_MATCHED  ||  _dWork->state == MDSTATE_PLAYING) {
    for(i=0 ; i< _dWork->maxConnectNum; i++){
      if( _dWork->sendintervaltime[i]++ >= KEEPALIVE_TOKEN_TIME && _dWork->sendbufflag == 0) {
        if(sendKeepAlivePacket(i)){
          clearTimeoutBuff();
          break;
        }
      }
    }
  }

  return mydwc_HandleError();
}

//==============================================================================
/**
 * aid��Ԃ��܂��B�ڑ�����܂ł�-1��Ԃ��܂��B
 * @retval  aid�B�������ڑ��O��0xffffffff
 */
//==============================================================================
u32 GFL_NET_DWC_GetAid(void)
{
  if(_dWork){
    if( _dWork->state == MDSTATE_MATCHED || _dWork->state == MDSTATE_PLAYING || _dWork->state == MDSTATE_DISCONNECTTING )
    {
      return DWC_GetMyAID();
    }
  }
  return 0xffffffff;
}

// �{�C�X�`���b�g�̐ؒf�R�[���o�b�N�B
static void vct_endcallback(){
  _dWork->isvchat = 0;
}

//==============================================================================
/**
 * �ʐM�m����A�{�C�X�`���b�g���J�n���܂��B
 * @param   heapID  �������m�ۂ���HEAPID
 * @retval  none
 */
//==============================================================================
void GFL_NET_DWC_StartVChat(void)
{
  _dWork->myvchaton = 1;
}

static void _DWC_StartVChat(int heapID)
{
  int late;
  int num = 1;

  // OS_TPrintf("VCTON\n");
  // �f�o�b�N�v�����gOFF
#ifndef DEBUGPRINT_ON
  //VCT_SetReportLevel( VCT_REPORTLEVEL_NONE );
#else
  //VCT_SetReportLevel( VCT_REPORTLEVEL_ALL );
#endif
#if 0
  if(bFourGame){
    num = GFL_NET_GetConnectNum()-1;
    MYDWC_DEBUGPRINT("VCTON�l��%d\n",num);
    if(num < 1){
      num = 1;
    }
    _dWork->myvchaton = 1;
    _dWork->opvchaton = 1;
    _dWork->myvchat_send = 1;
  }
#endif
  _dWork->myvchaton = 1;
  _dWork->opvchaton = 1;
  _dWork->myvchat_send = 1;

  if( _dWork->isvchat==0 ){
    switch( _dWork->vchatcodec ){
    case VCHAT_G711_ULAW:
      late = VCT_CODEC_G711_ULAW;
      break;
    case VCHAT_2BIT_ADPCM:
      late = VCT_CODEC_2BIT_ADPCM;
      break;
    case VCHAT_3BIT_ADPCM:
      late = VCT_CODEC_3BIT_ADPCM;
      break;
    case VCHAT_4BIT_ADPCM:
      late = VCT_CODEC_4BIT_ADPCM;
      break;
    default:
      //       if(!bFourGame){
      late = VCT_CODEC_4BIT_ADPCM;   //  DP
      //     }
      //   else{
      //     late = VCT_CODEC_3BIT_ADPCM;
      //            }
      break;
    }
    myvct_init( heapID, late, num );
    myvct_setDisconnectCallback( vct_endcallback );
    _dWork->isvchat = 1;
  }
}


void DWCRAP_StartVChat(void)
{
  GFLNetInitializeStruct* pNetInit = GFL_NET_GetNETInitStruct();

  _DWC_StartVChat(pNetInit->netHeapID);
}

//==============================================================================
/**
 * @brief  �{�C�X�`���b�g��OFF�ɂ��܂�
 * @param  none
 * @retval none
 */
//==============================================================================

void DWCRAP_StopVChat(void)
{
  if(_dWork->isvchat){
    myvct_endConnection();
  }
}


BOOL DWCRAP_IsVChat(void)
{
  return _dWork->isvchat;
}


//==============================================================================
/**
 * �{�C�X�`���b�g�̃R�[�f�b�N���w�肵�܂��B
 * �R�l�N�V�����m���O�ɃR�[�f�b�N���w�肵�Ă����ƁA
 * �R�l�N�V�����m�����Ɏ����I�Ƀ{�C�X�`���b�g���J�n���܂��B
 * @param �R�[�f�b�N�̃^�C�v
 * @retval none
 */
//==============================================================================
void GFL_NET_DWC_SetVchat(int codec)
{
  _dWork->vchatcodec = codec;

  if( _dWork->isvchat ){
    switch( _dWork->vchatcodec ){
    case VCHAT_G711_ULAW:
      myvct_setCodec( VCT_CODEC_G711_ULAW );
      break;
    case VCHAT_2BIT_ADPCM:
      myvct_setCodec( VCT_CODEC_2BIT_ADPCM );
      break;
    case VCHAT_3BIT_ADPCM:
      myvct_setCodec( VCT_CODEC_3BIT_ADPCM );
      break;
    case VCHAT_4BIT_ADPCM:
      myvct_setCodec( VCT_CODEC_4BIT_ADPCM );
      break;
    case VCHAT_8BIT_RAW:
      myvct_setCodec( VCT_CODEC_8BIT_RAW );
      break;
    }
  }
}

//==============================================================================
/**
 * @brief   �{�C�X�`���b�g���~���܂�
 * @param   void
 * @retval  void
 */
//==============================================================================

void GFL_NET_DWC_StopVChat(void)
{
  myvct_free();
  if(_dWork != NULL){
    _dWork->isvchat = 0;
    _dWork->backupBitmap = 0;
    _dWork->myvchaton = 0;
  }
}

//==============================================================================
/**
 * @brief   �{�C�X�`���b�g�̈ꎞ��~�{�������s���܂�
 * @param   bPause   �ꎞ��~=TRUE  ����=FALSE
 * @retval  void
 */
//==============================================================================

void GFL_NET_DWC_pausevchat(BOOL bPause)
{
  if(bPause){
    myvct_DelConference(DWC_GetMyAID());
    _dWork->pausevchat = TRUE;
  }
  else{
    _dWork->pausevchat = FALSE;
  }
  VCHAT_PauesFlg(_dWork->pausevchat);
}



//==============================================================================
/**
 * DWC�G���[�ԍ��ɑΉ����郁�b�Z�[�W��ID��Ԃ��܂��B
 * @param code - �G���[�R�[�h�i���̒l�����Ă�������
 * @retval ���b�Z�[�W�^�C�v
 */
//==============================================================================
int GFL_NET_DWC_ErrorType(int code, int type)
{
  int code100 = code / 100;
  int code1000 = code / 1000;

  if( code == 20101 ) return 1;
  if( code1000 == 23 ) return 1;
  if( code == 20108 ) return 2;
  if( code == 20110 ) return 3;
  if( code100 == 512 ) return 4;
  if( code100 == 500 ) return 5;
  if( code == 51103 ) return 6;   ///����
  if( code100 == 510 ) return 6;
  if( code100 == 511 ) return 6;
  if( code100 == 513 ) return 6;
  
  if( code >= 52000 && code <= 52003 ) return 8;
  if( code >= 52010 && code <= 52012 ) return 8;
  if( code >= 52100 && code <= 52103 ) return 8;
  if( code >= 52110 && code <= 52112 ) return 8;
  if( code >= 52200 && code <= 52203 ) return 8;
  if( code >= 52210 && code <= 52212 ) return 8;
  if( code >= 52400 && code <= 52403 ) return 8;
  if( code >= 52410 && code <= 52412 ) return 8;
  if( code >= 52500 && code <= 52503 ) return 8;
  if( code >= 52510 && code <= 52512 ) return 8;
  if( code >= 52700 && code <= 52703 ) return 8;
  if( code >= 52710 && code <= 52712 ) return 8;

  if( code == 80430 ) return 9;

  if( code1000 == 20 ) return 0;
  if( code100 == 520 ) return 0;
  if( code100 == 521 ) return 0;
  if( code100 == 522 ) return 0;
  if( code100 == 523 ) return 0;
  if( code100 == 530 ) return 0;
  if( code100 == 531 ) return 0;
  if( code100 == 532 ) return 0;

  if( code < 10000 ) return 14; // �G���[�R�[�h���P�O�O�O�O�ȉ��̏ꍇ�́A�G���[�R�[�h�̕\���̕K�v���Ȃ��B

  if( code1000 == 31 ) return 12; // �_�E�����[�h���s
  // ���̑��̃G���[��TYPE�Ŕ��f

  switch(type){
  case DWC_ETYPE_NO_ERROR:
  case DWC_ETYPE_LIGHT:
  case DWC_ETYPE_SHOW_ERROR:
    return 14;  //
  case DWC_ETYPE_SHUTDOWN_FM:
  case DWC_ETYPE_SHUTDOWN_GHTTP:
  case DWC_ETYPE_SHUTDOWN_ND:
  case DWC_ETYPE_DISCONNECT:
    return 11;  //10����11�ɕύX
  default:
    GF_ASSERT_HEAVY(0); //DWC���Ԃ�type�������Ă���Ȃ猩�������K�v
    break;
  case DWC_ETYPE_FATAL:
    return 15;
  }
  return -1;
}

//==============================================================================
/**
 * �ʐM��ؒf���܂��B
 * @param sync �c 0 = ��������ؒf���ɂ����B1 = ���肪�ؒf����̂�҂B
 * @retval �@�@�@ 1 = �ڑ����������B0 = �ؒf�������B
 */
//==============================================================================
int GFL_NET_DWC_disconnect( int sync )
{
  int errorCode;
  DWCErrorType myErrorType;
  int ret = DWC_GetLastErrorEx( &errorCode, &myErrorType );
  if( ret != 0 ){
    DWC_ClearError();
  }

  if( sync == 0 ){
    MYDWC_DEBUGPRINT(" mydwc_disconnect state %d \n",_dWork->state);
    switch( _dWork->state )	{
    case MDSTATE_MATCHING:   // k.ohno 06.07.08  �ǉ�
    case MDSTATE_MATCHED:
    case MDSTATE_PLAYING:
      if( _dWork->isvchat ){
        MYDWC_DEBUGPRINT("�{�C�X�`���b�g�ғ��� �~�߂�\n");
        myvct_endConnection();
      }
      _CHANGE_STATE(MDSTATE_DISCONNECTTING);
      //      _dWork->state = MDSTATE_DISCONNECTTING;
      break;
    case MDSTATE_LOGIN:     //�e�@�ؒf���ɓ��������킹�邽�߂ɒǉ� k.ohno 06.07.04
    case MDSTATE_ERROR_DISCONNECT:
    case MDSTATE_DISCONNECT:
    case MDSTATE_TIMEOUT:
      return 1;
    }
  }
  else {
    switch( _dWork->state ) {
    case MDSTATE_LOGIN:     //�e�@�ؒf���ɓ��������킹�邽�߂ɒǉ� k.ohno 06.07.04
    case MDSTATE_DISCONNECT:
    case MDSTATE_TIMEOUT:
      return 1;
    }
  }
  return 0;
}

//==============================================================================
/**
 * �ʐM���ؒf������A���̊֐����ĂԂ��Ƃœ�����Ԃ����O�C������̏�Ԃɂ��܂��B
 * @param 	nonte
 * @retval  1 = �����B0 = ���s�B
 */
//==============================================================================
int GFL_NET_DWC_returnLobby()
{
  if( _dWork->state == MDSTATE_DISCONNECT ||
      _dWork->state == MDSTATE_TIMEOUT ||
      _dWork->state == MDSTATE_ERROR_DISCONNECT ||
      _dWork->state == MDSTATE_LOGIN) {
    //        _dWork->op_aid = -1;
    _CHANGE_STATE(MDSTATE_LOGIN);
    _dWork->stepMatchResult = STEPMATCH_CONNECT;
    //    _dWork->state = MDSTATE_LOGIN;
    _dWork->newFriendConnect = -1;
    //GFL_NET_DWC_ResetClientBlock();
    return 1;
  }
  return 0;
}

//==============================================================================
/**
 * fetal error�������ɌĂ΂��֐��A���̃R�[���o�b�N�̒��ŏ������Ƃ߂ĉ������B
 * @param 	nonte
 * @retval  1 = �����B0 = ���s�B
 */
//==============================================================================
void GFL_NET_DWC_setFetalErrorCallback( void (*func)(int) )
{
  if(_dWork){
    _dWork->fetalErrorCallback = func;
  }
}



//-----2006.04.11 k.ohno
//==============================================================================
/**
 * ���O�A�E�g����
 * @param 	none
 * @retval  none
 */
//==============================================================================

void GFL_NET_DWC_Logout(void)
{
  if(_dWork){
    MYDWC_DEBUGPRINT("LOGOUT\n");
    DWC_ClearError();
    if(_dWork->friendMatchOn){
      DWC_ShutdownFriendsMatch();
    }
    _dWork->friendMatchOn=FALSE;
    DWC_CleanupInet();
    GFL_NET_DWC_StopVChat();
    GFL_NET_DWC_free();
  }
}

//-------------------------------------------------------------------------------
/**
 * @brief   �P�t���[����FRIENDINFO_UPDATA_PERFRAME�l���̃f�[�^���X�V����
 * @param 	none
 * @retval  none
 */
//-------------------------------------------------------------------------------

static void mydwc_updateFriendInfo( void )
{
  int i;

  if(_dWork->pFriendData==NULL){
    return;
  }
  for(i = 0; i < FRIENDINFO_UPDATA_PERFRAME; i++)
  {
    int index = _dWork->friendupdate_index % FRIENDLIST_MAXSIZE;
    int size;

    if( DWC_IsBuddyFriendData( &(_dWork->pFriendData[index]) ) ){
      u8 backup = _dWork->friend_status[index];
      u8 nowstate = DWC_GetFriendStatusData(&_dWork->pFriendData[ index ],(char*)_dWork->friendinfo[index],&size);
      if(size < 1){//0��-1�����肦��
        _dWork->friend_status[index] = DWC_STATUS_OFFLINE;
        GFL_STD_MemClear(&_dWork->friendinfo[index],MYDWC_STATUS_DATA_SIZE_MAX);
      }
      else{
        _dWork->friend_status[index] = nowstate;
#if PM_DEBUG
        GF_ASSERT_HEAVY(size == MYDWC_STATUS_DATA_SIZE_MAX);
///        if(backup!=nowstate){
//          MYDWC_DEBUGPRINT("FRIEND CHANGE %d => %d  %d\n",backup, nowstate,index);
//        }
#endif
      }
    }
  }
  _dWork->friendupdate_index = _dWork->friendupdate_index + FRIENDINFO_UPDATA_PERFRAME;
}

//==============================================================================
/**
 * �����̏�Ԃ��T�[�o�ɃA�b�v���܂��B
 * @param 	data - �f�[�^�ւ̃|�C���^
 * @param 	size  - �f�[�^�T�C�Y
 * @retval  �����̉�
 */
//==============================================================================
BOOL GFL_NET_DWC_SetMyInfo( const void *data, int size )
{
  MYDWC_DEBUGPRINT("upload status change(%p, %d)\n", data, size);

  GF_ASSERT_HEAVY(size <= MYDWC_STATUS_DATA_SIZE_MAX);
  return DWC_SetOwnStatusData( data, size );
}

//==============================================================================
/**
 * �F�B���T�[�o�ɃA�b�v���Ă�������擾���܂��i���[�J���ɃL���b�V�����Ă�����̂�\�����܂��j�B
 * @param 	index �t�����h���X�g��̔ԍ�
 * @retval  �f�[�^�ւ̃|�C���^�B���g�͏��������Ȃ��ŉ������B
 */
//==============================================================================
void* GFL_NET_DWC_GetFriendInfo( int index )
{
  return (void*)&_dWork->friendinfo[index];
}

//==============================================================================
/**
 * �F�B���T�[�o�ɃA�b�v���Ă�������擾���܂��i���[�J���ɃL���b�V�����Ă�����̂�\�����܂��j�B
 * @param 	index �t�����h���X�g��̔ԍ�
 * @retval  �F�B�̏�ԁBDWC_GetFriendStatusData�̕Ԃ�l�Ɠ���
 */
//==============================================================================
u8 GFL_NET_DWC_getFriendStatus( int index )
{
  return _dWork->friend_status[index];
}


static void SetupGameServerCallback(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int  index, void* param);
static void ConnectToGameServerCallback(DWCError error, BOOL cancel, BOOL self, BOOL isServer, int  index, void* param);
static void NewClientCallback(int index, void* param);

//==============================================================================
/**
 * �Q�[����W�E�Q�����s���֐��B
 * @target   ���c�����ŃQ�[�����J�ÁB�O�ȏ�c�ڑ����ɍs������́A�t�����h���X�g��̈ʒu
 * @retval  ���c�����B�O�c���s�B
 */
//==============================================================================
int GFL_NET_DWC_StartGame( int target,int maxnum, BOOL bVCT )
{
  int ans,num = maxnum;

  if(GFL_NET_DWC_GetSaving()){
    return DWCRAP_STARTGAME_FIRSTSAVE;
  }

  if( _dWork->state != MDSTATE_LOGIN ){
    _dWork->setupErrorCount++;
    if(_dWork->setupErrorCount>120){
      return DWCRAP_STARTGAME_FAILED;
    }
    return DWCRAP_STARTGAME_NOTSTATE;
  }
  mydwc_releaseRecvBuffAll();
  _dWork->BlockUse_BackupBitmap = 0; //080703 ohno
  _dWork->closedflag = TRUE;	// 080602	tomoya
  _dWork->friendindex = target;
  _dWork->maxConnectNum = maxnum;
  _dWork->canceled_matching = -1;

  //    OHNO_PRINT("max %d\n",_dWork->maxConnectNum);
  if(bVCT){
    num=2;
  }
  _dWork->bConnectCallback = TRUE;
  if ( target < 0 ){
    ans = DWC_SetupGameServer(DWC_TOPOLOGY_TYPE_FULLMESH,
                              (u8)num,
                              SetupGameServerCallback, GFL_NET_GetWork(),
                              NewClientCallback, GFL_NET_GetWork(),
                              attemptCallback,
                              (u8*)&_dWork->connectionUserData,GFL_NET_GetWork()
                              );
    _dWork->matching_type = MDTYPE_PARENT;
  } else {
    ans = DWC_ConnectToGameServerAsync(DWC_TOPOLOGY_TYPE_FULLMESH,
                                       target,ConnectToGameServerCallback,NULL,NewClientCallback,GFL_NET_GetWork(),
                                       attemptCallback,
                                       (u8*)&_dWork->connectionUserData,
                                       GFL_NET_GetWork()
                                       );
    _dWork->matching_type = MDTYPE_CHILD;
  }



  if(!ans){
    _dWork->setupErrorCount++;
    if(_dWork->setupErrorCount>120){
      GFL_NET_StateSetWifiError( 0, 0, 0, ERRORCODE_DISCONNECT ); //BTS7100�ł͂Ȃ����C��
    }
    return DWCRAP_STARTGAME_RETRY;
  }
  _dWork->setupErrorCount = 0;//���Z�b�g���Ă���

  {
    int i;
    for(i=0;i<maxnum; i++){
      mydwc_allocRecvBuff(i);
    }
  }
  _CHANGE_STATE(MDSTATE_MATCHING);
  //_dWork->state = MDSTATE_MATCHING;

  // ���M�R�[���o�b�N�̐ݒ�
  DWC_SetUserSendCallback( SendDoneCallback,NULL );

  // ��M�R�[���o�b�N�̐ݒ�
  DWC_SetUserRecvCallback( UserRecvCallback,NULL );

  // �R�l�N�V�����N���[�Y�R�[���o�b�N��ݒ�
  DWC_SetConnectionClosedCallback(ConnectionClosedCallback, NULL);

  // �^�C���A�E�g�R�[���o�b�N�̐ݒ�
  DWC_SetUserRecvTimeoutCallback( recvTimeoutCallback,NULL );

  _dWork->sendbufflag = 0;

  return DWCRAP_STARTGAME_OK;
}

//==============================================================================
/**
 * ���ڑ����Ă���F�B�̃t�����h���X�g��̈ʒu��Ԃ��܂��B
 * �܂��A�ڑ����������Ă��Ȃ��Ă��F�B�ԍ���Ԃ����Ƃ�����܂��̂ŁA�ڑ���
 * �����������̔���ɂ͎g��Ȃ��ł��������B
 * @retval  �O�ȏ�c�F�B�ԍ��B�|�P�c�܂��ڑ����ĂȂ��B
 */
//==============================================================================
int GFL_NET_DWC_GetFriendIndex(void)
{
  if(_dWork){
    // ���ڑ����Ă���F�B�̃t�����h���X�g��̈ʒu��Ԃ��܂��B
    return _dWork->friendindex;
  }
  return -1;
}

/*---------------------------------------------------------------------------*
  �Q�[���T�[�o�N���R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void SetupGameServerCallback(DWCError error,
                                    BOOL cancel,
                                    BOOL self,
                                    BOOL isServer,
                                    int  index,
                                    void* param)
{
#pragma unused(isServer, param)
  BOOL bFriendOnly = FALSE;

  _dWork->bConnectCallback = FALSE;
  if (error == DWC_ERROR_NONE){
    if (!cancel){
      // �l�b�g���[�N�ɐV�K�N���C�A���g���������
      MYDWC_DEBUGPRINT("�F�B���ڑ����Ă��܂����B�i�C���f�b�N�X��%d�j\n", index);
      // �ڑ����m�������Ƃ��̂݁A
      // �������`�ɏC��
      // 080624
      // BTS�ʐM630�̑Ώ�	tomoya
      //			_dWork->friendindex = index;

      //            if(CommLocalIsWiFiFriendGroup(CommStateGetServiceNo()) && (index==-1)){
      if(_dWork->bWiFiFriendGroup && (index == -1)){
        bFriendOnly = TRUE;
      }

      if(_dWork->connectModeCheck){
        if(FALSE == _dWork->connectModeCheck(index,GFL_NET_GetWork())){
          //OS_TPrintf("�ؒfWIFIP2PModeCheck \n");
          bFriendOnly = TRUE;
        }
      }


      if (_dWork->blockClient || bFriendOnly){
        if(index!=-1){
          DWC_CloseConnectionHardFromServer(index);
          /*
���̃N���[�Y�͑���z�X�g�ɂ��ʒm����A����z�X�g�ł̓N���[�Y�R�[���o�b�N
DWCConnectionClosedCallback���Ăяo����܂��B
�������A���̃N���[�Y�ʒm��UDP�ʐM�ň�x�������M����Ȃ����߁A
�ʐM�H�̏󋵂Ȃǂɂ���Ă͑���ɓ͂��Ȃ��\��������܂��B
           */
        }
      }


      // �ڑ����m�������Ƃ��̂݁A
      // �������`�ɏC��
      // 080624
      // BTS�ʐM630�̑Ώ�	tomoya
      _dWork->friendindex = index;


      _dWork->BlockUse_BackupBitmap = DWC_GetAIDBitmap();
      if(_dWork->BlockUse_BackupBitmap==0x01){ //���������Ȃ������Ƃ��ɂ̓^�C���A�E�g�ɂ���
        _CHANGE_STATE(MDSTATE_CANCEL);
        //    _dWork->state = MDSTATE_CANCEL;
        MYDWC_DEBUGPRINT("�����^�C���A�E�g %x\n",_dWork->BlockUse_BackupBitmap);
      }
      else{
        setTimerAndFlg(index);
      }
    }
    else
    {
      if (self){
        // �������}�b�`���O���L�����Z������
        MYDWC_DEBUGPRINT("�}�b�`���O�̃L�����Z�����������܂����B\n\n");
        // ���O�C�����Ԃɖ߂�
        //				s_dwcstate = MYDWCSTATE_MATCH_CANCELFINISH;
      }
      else {
        // �q���ł����q�@���A�ڑ����L�����Z�������B
        MYDWC_DEBUGPRINT("Client (friendListIndex = %d) canceled matching.\n\n", index);
        // �q���ł����q�@�����Ȃ��Ȃ����B2006.7.3 yoshihara
        _dWork->canceled_matching = index;
        _dWork->newFriendConnect = -1;
      }
    }
  }
  else {
    // �G���[�����������B�G���[�R�[�h�́Astep�֐��̒��ŏE���B
    MYDWC_DEBUGPRINT("SetupGame server error occured. %d\n\n", error);

    //        s_dwcstate = MYDWCSTATE_MATCH_CANCELFINISH;
  }
}

/*---------------------------------------------------------------------------*
  �Q�[���T�[�o�ڑ��R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void ConnectToGameServerCallback(DWCError error,
                                        BOOL cancel,
                                        BOOL self,
                                        BOOL isServer,
                                        int  index,
                                        void* param)
{
#pragma unused(param)
  _dWork->bConnectCallback = FALSE;
  if (error == DWC_ERROR_NONE){
    if (!cancel){

      if (self){
        // �������Q�[���T�[�o�Ƃ����ɂł��Ă���l�b�g���[�N�ւ̐ڑ���
        // ���������ꍇ
        MYDWC_DEBUGPRINT("�ڑ��ɐ������܂���\n");
      }
      else {
        // �l�b�g���[�N�ɐV�K�N���C�A���g����������ꍇ�B
        // ��l�ΐ����Ȃ̂ŁA�����ɂ͂��Ȃ��͂��B
        MYDWC_DEBUGPRINT("�V�K�ɂȂ���܂���\n");
      }
      setTimerAndFlg(index);
    }
    else {
      if (self){
        // �������}�b�`���O���L�����Z������
        MYDWC_DEBUGPRINT("�L�����Z�����������܂����B\n\n");
        // ���O�C�����Ԃɖ߂�
        //                s_dwcstate = MYDWCSTATE_MATCH_CANCELFINISH;
      }
      else {
        if (isServer){
          // �Q�[���T�[�o���}�b�`���O���L�����Z������
          MYDWC_DEBUGPRINT("�e���ڑ����L�����Z�����܂���\n\n");
          // �}�b�`���O���I�����ă��O�C�����Ԃɖ߂�
          //                    s_dwcstate = MYDWCSTATE_MATCH_CANCELFINISH;
        }
        else {
          MYDWC_DEBUGPRINT("�q���ڑ����L�����Z�����܂���\n\n");
          // ���̃N���C�A���g���}�b�`���O���L�����Z�������B
          // ��l�ΐ�Ȃ炱���ɂ��Ȃ��͂��B
        }
      }
    }
  }
#ifdef PM_DEBUG
  else {
    // �G���[�����������B�G���[�R�[�h�́Astep�֐��̒��ŏE���B
    MYDWC_DEBUGPRINT("ConnectGame server error occured. %d\n\n", error);
    {
      int errorCode;
      DWCErrorType myErrorType;
      int ret = DWC_GetLastErrorEx( &errorCode, &myErrorType );
      if( ret != 0 ){
        MYDWC_DEBUGPRINT("error cb!(%d, %d, %d)\n", ret, errorCode, myErrorType);
      }
    }
    // ������x�ŏ�����B������������
    //        s_dwcstate = MYDWCSTATE_MATCH_ERRORFINISH;
  }
#endif

  //    s_blocking = 0;
}

/*---------------------------------------------------------------------------*
  �T�[�o�N���C�A���g�^�}�b�`���O���̐V�K�ڑ��N���C�A���g�ʒm�R�[���o�b�N�֐�
 *---------------------------------------------------------------------------*/
static void NewClientCallback(int index, void* param)
{
#pragma unused(param)
  // �V�K�ڑ��N���C�A���g�̐ڑ��������I���܂ł́A
  // �R���g���[��������֎~����
  //    s_blocking = 1;

  _dWork->newFriendConnect = index;

  MYDWC_DEBUGPRINT("�V�����l���ڑ����Ă��܂����B\n");
  if (index != -1){
    MYDWC_DEBUGPRINT("�F�B[%d].\n", index);
    //VCT�J���t�@�����X�J�n

  }
  else {
    MYDWC_DEBUGPRINT("�F�B�ł͂Ȃ�.\n");
  }
  if( _dWork->connectCallback ){  //
    _dWork->connectCallback(index, _dWork->pConnectWork);
  }
}

//==============================================================================
/**
 * ���݂̃t�����h���X�g��\�����܂��B�i�f�o�b�O�p�j
 * @param 	none
 * @retval  none
 */
//==============================================================================
#if PM_DEBUG
void GFL_NET_DWC_showFriendInfo()
{
  int i;

  if(_dWork->pFriendData==NULL){
    return;
  }

  
  if( !DWC_CheckHasProfile( _dWork->pUserData ) )
  {
    DWCFriendData token;
    u32 *ptr;

    DWC_CreateExchangeToken( _dWork->pUserData, &token );
    ptr = (u32*)&token;
    MYDWC_DEBUGPRINT("�܂��A�v���t�@�C���h�c�擾�O\n���O�C���h�c:(%d, %d, %d)\n\n", ptr[0], ptr[1], ptr[2] );
  }
  else
  {
    // �ڑ��ς�
    DWCFriendData token;
    DWC_CreateExchangeToken( _dWork->pUserData, &token );
    MYDWC_DEBUGPRINT("�v���t�@�C���h�c:%d \n\n", DWC_GetGsProfileId( _dWork->pUserData, &token ) );
  }

  for( i = 0; i < FRIENDLIST_MAXSIZE; i++ )
  {
    int ret = DWC_GetFriendDataType( &(_dWork->pFriendData[i]) );
    u32 *ptr = (u32*)(&_dWork->pFriendData[i]);
    switch(ret)
    {
    case DWC_FRIENDDATA_LOGIN_ID:
      MYDWC_DEBUGPRINT("%d:���O�C���h�c:(%d, %d, %d)",i, ptr[0], ptr[1], ptr[2] );
      break;

    case DWC_FRIENDDATA_FRIEND_KEY:
      MYDWC_DEBUGPRINT("%d:�t�����h�R�[�h:(%d)", i, DWC_GetGsProfileId( _dWork->pUserData, &_dWork->pFriendData[i] ) );
      break;

    case DWC_FRIENDDATA_GS_PROFILE_ID:
      MYDWC_DEBUGPRINT("%d:�v���t�@�C���h�c:(%d)", i, DWC_GetGsProfileId( _dWork->pUserData, &_dWork->pFriendData[ i ]) );
      break;

    case DWC_FRIENDDATA_NODATA:
    default:
      MYDWC_DEBUGPRINT("%d:��", i);
      break;
    }

    if( DWC_IsBuddyFriendData( &(_dWork->pFriendData[i]) ) )
    {
      MYDWC_DEBUGPRINT("(���v��)");
    }
    MYDWC_DEBUGPRINT("\n");
  }
}
#endif

// ���M�������ǂ�����Ԃ��܂�
BOOL GFL_NET_DWC_IsSendVoiceAndInc(void)
{
  return myvct_IsSendVoiceAndInc();
}


//==============================================================================
/**
 * �{�C�X�`���b�g��Ԃ��ǂ�����Ԃ��܂�   k.ohno 06.05.23 07.22 �t���O������
 * @retval  TRUE�c�{�C�X�`���b�g   FALSE�c�{�C�X�`���b�g�ł͂Ȃ�
 */
//==============================================================================
BOOL GFL_NET_DWC_IsVChat(void)
{
  if(_dWork){
    return _dWork->bVChat;
  }
  return FALSE;
}

//==============================================================================
/**
 * �ڑ����Ă������ǂ�����Ԃ��܂�    k.ohno 06.05.24
 * @retval  TRUE�c�ڑ��J�n�Ȃ̂ŃL�[������u���b�N   FALSE�c
 */
//==============================================================================
BOOL GFL_NET_DWC_IsNewPlayer(void)
{
  if(_dWork){
    return _dWork->newFriendConnect;
  }
  return FALSE;
}

//==============================================================================
/**
 * �ڑ����Ă������ǂ����t���O�𗎂Ƃ��܂� k.ohno 06.08.04
 * @retval  none
 */
//==============================================================================
void GFL_NET_DWC_ResetNewPlayer(void)
{
  if(_dWork){
    _dWork->newFriendConnect = -1;
  }
}


//==============================================================================
/**
 * �Ȃ��낤�Ƃ��Ă����q�@�̐ؒf���擾
 * @retval  �ؒf�����q�@INDEX
 */
//==============================================================================
s16 GFL_NET_DWC_GetDisconnectNewPlayer(void)
{
  if(_dWork){
    return _dWork->canceled_matching;
  }
  return -1;
}
//==============================================================================
/**
 * VCHAT��ONOFF     k.ohno 06.05.24
 */
//==============================================================================
void GFL_NET_DWC_SetVChat(BOOL bVChat)
{
  _dWork->bVChat = bVChat;
}

static void sendPacket()
{
  int i;

  if( _dWork->sendbufflag || !_isSendableReliable() ) // ���M�o�b�t�@���`�F�b�N�B
  {
    _dWork->sendbufflag = 1;
    *((u32*)&(_dWork->sendBuffer[0])) = MYDWC_KEEPALIVE_PACKET | (_dWork->myvchaton << MYDWC_PACKET_VCHAT_SHIFT);;

    DWC_SendReliableBitmap(DWC_GetAIDBitmap(), &(_dWork->sendBuffer[0]), 4);
    clearTimeoutBuff();
  }
}

void mydwc_VChatPause()
{
  if( _dWork->myvchaton != 0 )
  {
    _dWork->myvchaton = 0;
    sendPacket();
  }
}

void mydwc_VChatRestart()
{
  if( _dWork->myvchaton != 1 )
  {
    _dWork->myvchaton = 1;
    sendPacket();
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	VCHAT�f�[�^���M�ݒ�
 *
 *	@param	flag	TRUE:�����f�[�^�𑗐M����	FALSE:�����f�[�^�𑗐M���Ȃ�
 */
//-----------------------------------------------------------------------------
void mydwc_VChatSetSend( BOOL flag )
{
  _dWork->myvchat_send = flag;
}
BOOL mydwc_VChatGetSend( void )
{
  return _dWork->myvchat_send;
}


//  _dWork->blockClient = _BLOCK_CALLBACK;

static void _blockCallback(DWCSuspendResult result, BOOL suspend, void *data)
{
  if(DWC_SUSPEND_SUCCESS == result){
    if(suspend){
      _dWork->blockClient = _BLOCK_CALLBACK;
    }
    else{
      _dWork->blockClient = _BLOCK_NONE;
    }
  }
}




//==============================================================================
/**
 * @brief   �N���C�A���g�ڑ����ꎞ�I�ɒ��~���܂�
 * @param   none
 * @retval  none
 */
//==============================================================================
BOOL GFL_NET_DWC_SetClientBlock(void)
{
#if 0
  if(_dWork->blockClient==_BLOCK_NONE){
    _dWork->blockClient = _BLOCK_START;
    DWC_StopSCMatchingAsync(_blockCallback,NULL);
  }
  return (_dWork->blockClient==_BLOCK_CALLBACK);
#endif


  return DWC_RequestSuspendMatchAsync(TRUE,
                                      _blockCallback,
                                      NULL);

}

//==============================================================================
/**
 * @brief   �N���C�A���g�ڑ������ɖ߂��܂�
 * @param   none
 * @retval  none
 */
//==============================================================================
BOOL GFL_NET_DWC_ResetClientBlock(void)
{
  return DWC_RequestSuspendMatchAsync(FALSE,
                                      _blockCallback,
                                      NULL);
  //  _dWork->blockClient = _BLOCK_NONE;
}

//==============================================================================
/**
 * @brief   �]����Ԓ��̐ڑ�����Ԃ�
 * @param   none
 * @retval  ��
 */
//==============================================================================
int GFL_NET_DWC_AnybodyEvalNum(void)
{
  return DWC_GetNumConnectionHost();
}


//----------------------------------------------------------------------------
/**
 *	@brief	�ʐM�N���[�Y�����ŁA�ؒf�����ɑJ�ڂ���̂��̃t���O
 *
 *	@param	flag	�t���O		TRUE�Ȃ�ؒf�����ɑJ�ڂ���i�����lTRUE�j
 *
 * *Wi-Fi�N���u�S�l��W��ʗp�ɍ쐬
 */
//-----------------------------------------------------------------------------
void GFL_NET_DWC_SetClosedDisconnectFlag( BOOL flag )
{
  _dWork->closedflag = flag;
  MYDWC_DEBUGPRINT( "_dWork->closedflag = %d\n", flag );
}

//==============================================================================
/**
 * @brief   ��M�o�b�t�@�J��
 * @param
 * @retval  none
 */
//==============================================================================

void mydwc_releaseRecvBuff(int aid)
{
  if(_dWork->recvPtr[aid]!=NULL){
    //OHNO_PRINT("_SetRecvBuffer�������J�� %d\n",aid);
    GFL_NET_Align32Free(_dWork->recvPtr[aid]);
    _dWork->recvPtr[aid]=NULL;
  }
}

//==============================================================================
/**
 * @brief   ��M�o�b�t�@�m��
 * @param
 * @retval  none
 */
//==============================================================================

void mydwc_allocRecvBuff(int i)
{
  GFLNetInitializeStruct* pNetInit = GFL_NET_GetNETInitStruct();
  //	GFL_NETSYS* pNet = _GFL_NET_GetNETSYS();

  mydwc_releaseRecvBuff(i);

  if(_dWork->recvPtr[i]==NULL){
    NET_PRINT("_SetRecvBuffer�������m�� %d\n",i);
    //    _dWork->recvPtr[i] = GFL_NET_Align32Alloc(pNetInit->wifiHeapID, SIZE_RECV_BUFFER);
    _dWork->recvPtr[i] = GFL_NET_Align32Alloc(pNetInit->netHeapID, SIZE_RECV_BUFFER);
    DWC_SetRecvBuffer( i, _dWork->recvPtr[i], SIZE_RECV_BUFFER );
  }
}

//==============================================================================
/**
 * @brief   ��M�o�b�t�@���ׂĊJ��
 * @param   TRUE�Ȃ烏�[���h����m��
 * @retval  none
 */
//==============================================================================

void mydwc_releaseRecvBuffAll(void)
{
  int i;

  for(i = 0 ; i < _WIFI_NUM_MAX ; i++){
    mydwc_releaseRecvBuff(i);
  }
  _dWork->sendFinish = TRUE;

}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z�[�u���ėǂ����ǂ����m�F����t���O
 *	@param	TRUE �Z�[�u���Ă悢
 */
//-----------------------------------------------------------------------------
u8 GFL_NET_DWC_GetSaving(void)
{
  return _dWork->saveing;  //�Z�[�u����1
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z�[�u���ėǂ����ǂ����m�F����t���O������
 */
//-----------------------------------------------------------------------------
void GFL_NET_DWC_ResetSaving(void)
{
  _dWork->saveing = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�����Z���������s���ėǂ����ǂ���
 *	@param	FALSE �L�����Z�����Ă悢 TRUE�L�����Z���֎~
 */
//-----------------------------------------------------------------------------
BOOL GFL_NET_DWC_CancelDisable(void)
{
  return _dWork->bConnectCallback;
}

//----------------------------------------------------------------------------
/**
 *	@brief	LOGIN��Ԃ��ǂ���
 *	@param	TRUE LOGIN��Ԃł���
 */
//-----------------------------------------------------------------------------
BOOL GFL_NET_DWC_IsLogin(void)
{
  if(_dWork){
    return ( _dWork->state == MDSTATE_LOGIN );
  }
  return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�}�b�`���O��Ԃ��ǂ���
 *	@param	TRUE �}�b�`���O��Ԃł���
 */
//-----------------------------------------------------------------------------
BOOL GFL_NET_DWC_IsMatched(void)
{
  if(_dWork){
    if((_dWork->state == MDSTATE_MATCHED)  ||  (_dWork->state == MDSTATE_PLAYING)){
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�Z�[�u���łȂ����̏���
 *	@param
 */
//-----------------------------------------------------------------------------
static void _FuncNonSave(void)
{
  int i;


  for(i = 0;i < FRIENDLIST_MAXSIZE; i++){
    if(_dWork->deletedIndex[i] != -1){
      GFLNetInitializeStruct* pNetInit = GFL_NET_GetNETInitStruct();
      if(pNetInit->friendDeleteFunc){
        pNetInit->friendDeleteFunc( _dWork->deletedIndex[i], _dWork->srcIndex[i], GFL_NET_GetWork());
      }
      _dWork->deletedIndex[i] = -1;
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief   GFL_NET_DWC_stepmatch��������main�����Ƃ��Ă̋@�\������Ɨ�
 * @retval  none
 */
//--------------------------------------------------------------
void GFL_NET_DWC_MainStep( void )
{
  GFL_NET_DWC_stepmatch( 0 );
}


//--------------------------------------------------------------
/**
 * @brief   �L�����Z����ԂɈڍs����
            �O��ł�VCT�̏�Ԃ��I������ׂɕp�ɂɗp�����Ă��邪�A
            �}�b�`���O���L�����Z������ׂ̕��Ȃ̂ŁA�S�����L�����Z���ł���킯�ł͂Ȃ�
 * @retval  �L�����Z����ԂɈڍs������TRUE
 */
//--------------------------------------------------------------

BOOL GFL_NET_DWC_SetCancelState(void)
{
  if(_dWork->state == MDSTATE_MATCHING)
  {
    _CHANGE_STATE(MDSTATE_CANCEL);
    //    _dWork->state = MDSTATE_CANCEL;
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �X�e�b�v�}�b�`�̌��ʂ𓾂�
 * @retval  �X�e�b�v�}�b�`�̌���
 	          STEPMATCH_CONTINUE�c�}�b�`���O��
 	          STEPMATCH_SUCCESS�c����
            STEPMATCH_CANCEL�c�L�����Z��
            STEPMATCH_FAIL  �c���肪�e����߂����߁A�ڑ��𒆒f
 */
//--------------------------------------------------------------

int GFL_NET_DWC_GetStepMatchResult(void)
{
  return _dWork->stepMatchResult;
}

//------------------------------------------------------------------------------
/**
 * @brief   �q�@���Ȃ����Ă悢���ǂ����n�[�h���x���Œ�������
 * @param   bEnable TRUE=����
 * @retval  none
 */
//------------------------------------------------------------------------------

void GFL_NET_DWC_SetClinetConnect(BOOL bEnable)
{
  _dWork->bConnectEnable = bEnable;
}


//------------------------------------------------------------------------------
/**
 * @brief   �ڑ����鎞�ɂ킽���Ă���ID��ݒ�ł���
 * @param   data u32�^��ID
 * @retval  none
 */
//------------------------------------------------------------------------------

void GFL_NET_DWC_SetCconnectionUserData(u32 data)
{
  _dWork->connectionUserData = data;
}



//==============================================================================
/**
 * @brief   �ʐM���ؒf�������ǂ���
 * @retval  1 = �ؒf�B0 =����ȊO�B
 */
//==============================================================================
BOOL GFL_NET_DWC_IsDisconnect(void)
{
  if(!_dWork){
      return TRUE;
  }
  if( _dWork->state == MDSTATE_DISCONNECT ||
      _dWork->state == MDSTATE_ERROR_DISCONNECT ||
      _dWork->state == MDSTATE_TIMEOUT ){
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �q�@�����Ȃ��ꍇ�ɃG���[�ɂ��邩�ǂ�����ݒ�
 * @param   bOn ONOFF
 * @retval  none
 */
//------------------------------------------------------------------------------

void GFL_NET_DWC_SetNoChildErrorCheck(BOOL bOn)
{
  _dWork->bAutoDisconnect = bOn;
}


//------------------------------------------------------------------------------
/**
 * @brief   �ؒf���̃R�[���o�b�N��ݒ肷��
            �ؒf���ɂ͂��̃R�[���o�b�N���Ă΂�܂��̂ŁA���₩�Ƀ��[�N���J�����ĉ�����

            ���̊֐���
 * @param   Callback_NHTTPError* pFunc,  NHTTPError�R�[���o�b�N�֐�
 * @param   void* pUserWork,  ���[�U�[���[�N
 * @retval  none
 */
//------------------------------------------------------------------------------

void GFL_NET_DWC_SetErrDisconnectCallback(Callback_DisconnectError* pFunc,void* pUserWork )
{
  _dWork->pDisconnectErrorCallback = pFunc;
  _dWork->pDisconnectErrorUserWork = pUserWork;
}


#endif //GFL_NET_WIFI
