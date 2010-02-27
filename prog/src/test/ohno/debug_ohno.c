//============================================================================================
/**
 * @file	debug_ohno.c
 * @brief	�f�o�b�O�p�֐�
 * @author	ohno
 * @date	2006.11.29
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "ui.h"
#include "debug_ohno.h"
#include "debug_field.h"
#include "system/main.h"
#include "net/network_define.h"
#include "net/dwc_raputil.h"
#include "net/delivery_beacon.h"
#include "net/delivery_irc.h"
#include "wmi.naix"

#include "savedata/mystery_data.h"
#include "debug/debug_mystery_card.h"

#define _MAXNUM   (4)         // �ő�ڑ��l��
#define _MAXSIZE  (80)        // �ő呗�M�o�C�g��
#define _BCON_GET_NUM (16)    // �ő�r�[�R�����W��


FS_EXTERN_OVERLAY(dev_wifi);

typedef BOOL (*NetTestFunc)(void* pCtl);


struct _DEBUG_OHNO_CONTROL{
  u32 debug_heap_id;
  GFL_PROCSYS * psys;
  NetTestFunc funcNet;
  int bMoveRecv;
  BOOL bParent;
  int beaconIncCount;  //�r�[�R���̒��g��ւ���J�E���^
  DELIVERY_BEACON_WORK* pDBWork;
  DELIVERY_BEACON_INIT aInit;
  int counter;
  DELIVERY_IRC_WORK* pIRCWork;
  DELIVERY_IRC_INIT aIRCInit;
};


//------------------------------------------------------------------
//  ��M�e�[�u��
//------------------------------------------------------------------

static void _RecvMoveData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);
static void _RecvTalkData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle);



static BOOL NetTestNone(void* pCtl);
static BOOL NetTestSendTiming(void* pCtl);
static BOOL NetTestRecvTiming(void* pCtl);
static BOOL NetTestEndStart(void* pCtl);
static BOOL NetTestMoveStart(void* pCtl);
static BOOL NetTestMoveSend(void* pCtl);
static BOOL NetTestEnd(void* pCtl);

static void _connectCallBack(void* pWork);
static void* _netBeaconGetFunc(void* pWork);
static int _netBeaconGetSizeFunc(void* pWork);
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo);
//static void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo, void* pWork);
static void _netConnectFunc(void* pWork,int hardID);    ///< �n�[�h�Őڑ��������ɌĂ΂��
static void _netNegotiationFunc(void* pWork,int hardID);    ///< �l�S�V�G�[�V�����������ɃR�[��
static BOOL _NetTestChild(void* pCtl);
static BOOL _NetTestParent(void* pCtl);

static void _endCallBack(void* pWork);
static u8* _recvMemory(int netID, void* pWork, int size);



static const NetRecvFuncTable _CommPacketTbl[] = {
  {_RecvMoveData,         _recvMemory},    ///NET_CMD_MOVE
  {_RecvTalkData,         NULL},    ///NET_CMD_TALK
};


typedef struct{
  int gameNo;   ///< �Q�[�����
} _testBeaconStruct;

typedef struct{
  int x;
  int y;
  int z;
  int rot;
  int anm;
} _testMoveStruct;

static _testBeaconStruct _testBeacon = { WB_NET_DEBUG_OHNO_SERVICEID };


static GFLNetInitializeStruct aGFLNetInit = {
  _CommPacketTbl,  // ��M�֐��e�[�u��
  NELEMS(_CommPacketTbl), // ��M�e�[�u���v�f��
  _netConnectFunc,    ///< �n�[�h�Őڑ��������ɌĂ΂��
  _netNegotiationFunc,    ///< �l�S�V�G�[�V�����������ɃR�[��
  NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  _netBeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
  _netBeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
  _netBeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
  FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  _endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
  NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
#if GFL_NET_WIFI
  NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  NULL,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
  NULL,   ///< DWC�`���̗F�B���X�g
  NULL,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  0,   ///< DWC�ւ�HEAP�T�C�Y
  TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
#endif  //GFL_NET_WIFI
  0x444,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_NETWORK,  //IRC�p��create�����HEAPID
  GFL_WICON_POSX,GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
  4,                            // �ő�ڑ��l��
  88,                  //�ő呗�M�o�C�g��
  _BCON_GET_NUM,    // �ő�r�[�R�����W��
  TRUE,     // CRC�v�Z
  FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_WIRELESS,  //�ʐM���
  TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_DEBUG_OHNO_SERVICEID,  //GameServiceID
#if GFL_NET_IRC
  IRC_TIMEOUT_STANDARD,	// �ԊO���^�C���A�E�g����
#endif
  500,
  0,
};

//--------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�擾�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------

static void* _netBeaconGetFunc(void* pWork)
{
  return &_testBeacon;
}

///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int _netBeaconGetSizeFunc(void* pWork)
{
  return sizeof(_testBeacon);
}

///< �r�[�R���f�[�^�擾�֐�
static BOOL _netBeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
  if(myNo != beaconNo){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   �ړ��f�[�^��M�̂��߂̗̈�m�ۊ֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------
static u8 dummymem[4*sizeof(_testBeacon)];

static u8* _recvMemory(int netID, void* pWork, int size)
{
  return &dummymem[netID*sizeof(_testBeacon)];
}

//--------------------------------------------------------------
/**
 * @brief   �r�[�R���f�[�^�擾�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------
/*
static void FatalError_Disp(GFL_NETHANDLE* pNet,int errNo, void* pWork)
{
	// �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
	OS_TPrintf("�ʐM�s�\�G���[������ ErrorNo = %d\n",errNo);
}
 */
//--------------------------------------------------------------
/**
 * @brief   �ړ��R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------

static void _RecvMoveData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
  DEBUG_OHNO_CONTROL* pDOC = pWork;

  pDOC->bMoveRecv = TRUE;
}

//--------------------------------------------------------------
/**
 * @brief   ��b�R�}���h��M�֐�
 * @param   netID      �����Ă���ID
 * @param   size       �p�P�b�g�T�C�Y
 * @param   pData      �f�[�^
 * @param   pWork      ���[�N�G���A
 * @param   pHandle    �󂯎�鑤�̒ʐM�n���h��
 * @retval  none
 */
//--------------------------------------------------------------

static void _RecvTalkData(const int netID, const int size, const void* pData, void* pWork, GFL_NETHANDLE* pNetHandle)
{
}


//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//==============================================================================

static void _changeState(DEBUG_OHNO_CONTROL* pDOC, NetTestFunc state)
{
  pDOC->funcNet = state;
}

//==============================================================================
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//==============================================================================
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(DEBUG_OHNO_CONTROL* pDOC,NetTestFunc state, int line)
{
  NET_PRINT("df_state: %d\n",line);
  _changeState(pDOC, state);
}
#endif

#ifdef GFL_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeStateDebug(pDOC ,state,  __LINE__)
#else  //GFL_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeState(pDOC ,state )
#endif //GFL_NET_DEBUG



//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
#if 0
static BOOL _connectCallBack(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pWork;
  //   OS_TPrintf("GFL_NET_ChangeoverConnect OK\n");
  _CHANGE_STATE( NetTestSendTiming );
}
#endif
//--------------------------------------------------------------
/**
 * @brief   �����ڑ�
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestAutoConnect(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;

  if(pDOC->bParent){
    GFL_NET_InitServer();
    _CHANGE_STATE( _NetTestParent );
  }
  else{
    u8 mac[]={0xff,0xff,0xff,0xff,0xff,0xff};
    GFL_NET_InitClientAndConnectToParent(mac);

    //    GFL_NET_ChangeoverConnect(NULL);
    _CHANGE_STATE( _NetTestChild );
  }

  return FALSE;
}

//--------------------------------------------------------------
/**
 * @brief   �����ڑ��e�@��
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL _NetTestParent(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;

  if(GFL_NET_GetConnectNum()==4){
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,15,WB_NET_DEBUG_OHNO_SERVICEID);
    _CHANGE_STATE( NetTestRecvTiming );
  }
  return FALSE;
}



//--------------------------------------------------------------
/**
 * @brief   �����ڑ��q�@��
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------

static BOOL _NetTestChild2(void* pCtl)
{
   DEBUG_OHNO_CONTROL* pDOC = pCtl;
 
  if(GFL_NET_HANDLE_IsNegotiation( GFL_NET_HANDLE_GetCurrentHandle() ) ){
    int id = 1 - GFL_NET_HANDLE_GetNetHandleID( GFL_NET_HANDLE_GetCurrentHandle());
    if(GFL_NET_HANDLE_IsNegotiation( GFL_NET_GetNetHandle(id) ) ){
      GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,15,WB_NET_DEBUG_OHNO_SERVICEID);
      _CHANGE_STATE( NetTestRecvTiming );
    }
  }
  return FALSE;
}

static BOOL _NetTestChild(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;

  
  if(GFL_NET_HANDLE_RequestNegotiation()){
    _CHANGE_STATE( _NetTestChild2 );
  }
  return FALSE;
}



//--------------------------------------------------------------
/**
 * @brief   �����ڑ�
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestRecvTiming(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;

  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),15, WB_NET_DEBUG_OHNO_SERVICEID)){
    NET_PRINT("TIMOK\n");
    _CHANGE_STATE( NetTestMoveStart );
  }
  return FALSE;
}

//�^�C�~���O����葱����e�X�g

static int debugTiming = 1;
static BOOL _TimingTest(void* pCtl);

static BOOL _TimingTestNext(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;
  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),debugTiming, WB_NET_DEBUG_OHNO_SERVICEID)){
    NET_PRINT("TIMOK %d\n",debugTiming);
    debugTiming++;
    _CHANGE_STATE( _TimingTest );
  }
  GF_ASSERT(!GFL_NET_IsError());
  return FALSE;
}


static BOOL _TimingTest(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;
  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,debugTiming,WB_NET_DEBUG_OHNO_SERVICEID);

  GF_ASSERT(!GFL_NET_IsError());

  _CHANGE_STATE(_TimingTestNext);
  return FALSE;
}



//--------------------------------------------------------------
/**
 * @brief   �\���̓]���e�X�g
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestMoveStart(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;
  _testMoveStruct test={1,2,3,4,5};

  if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),NET_CMD_MOVE, sizeof(_testMoveStruct),&test)){
    _CHANGE_STATE( NetTestMoveSend );
  }
  return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief   �\���̓]���e�X�g
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestMoveSend(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;

  if(pDOC->bMoveRecv){
    NET_PRINT(" MOVE Recv\n");

    GFL_NET_SetNoChildErrorCheck(TRUE);
    _CHANGE_STATE( _TimingTest );

    //      _CHANGE_STATE( NetTestEndStart );
  }
  return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief   �����������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------

static void _endCallBack(void* pWork)
{
  DEBUG_OHNO_CONTROL* pDOC = pWork;
  NET_PRINT("endCallBack�I��\n");
  _CHANGE_STATE( NetTestEnd );
}

//--------------------------------------------------------------
/**
 * @brief   �����ڑ�
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestEndStart(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;
  //    GFL_NET_Exit(_endCallBack);
  GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),GFL_NET_CMD_EXIT_REQ, 0, NULL);
  //    GFL_NET_SendData();
  _CHANGE_STATE( NetTestNone );
  return FALSE;
}


//--------------------------------------------------------------
/**
 * @brief   �����������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------

static void _initCallBack(void* pWork)
{
  DEBUG_OHNO_CONTROL* pDOC = pWork;
  _CHANGE_STATE( NetTestAutoConnect );
}

//--------------------------------------------------------------
/**
 * @brief   �Ȃɂ����Ȃ�
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestNone(void* pCtl)
{
  return FALSE;
}

static BOOL netinit(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;

  if( GFL_NET_IsInit() == FALSE ){  // �����ʐM���Ă���ꍇ�I������
    GFL_NET_Init(&aGFLNetInit, _initCallBack, pDOC);
  }
  return FALSE;
}



//--------------------------------------------------------------
/**
 * @brief   �I��
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  PROC�I�����ɂ�TRUE
 */
//--------------------------------------------------------------
static BOOL NetTestEnd(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;
  _CHANGE_STATE( netinit );
  return FALSE;
}




//------------------------------------------------------------------
//  �f�o�b�O�p�������֐�
//------------------------------------------------------------------

static GFL_PROC_RESULT DebugOhnoMainProcInit(GFL_PROC * proc, int * seq, void * pwk, void * mywk,BOOL bParent)
{

  DEBUG_OHNO_CONTROL * pDOC;
  HEAPID			heapID = HEAPID_OHNO_DEBUG;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, heapID, 0x30000 );

  pDOC = GFL_PROC_AllocWork( proc, sizeof(DEBUG_OHNO_CONTROL), heapID );
  GFL_STD_MemClear(pDOC, sizeof(DEBUG_OHNO_CONTROL));
  pDOC->debug_heap_id = heapID;
  pDOC->bParent=bParent;
  
  _CHANGE_STATE( netinit );

  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------
/**   �f�o�b�O�p���C��
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugOhnoMainProcMain(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  DEBUG_OHNO_CONTROL * testmode = mywk;

  if(testmode->funcNet){
    if(testmode->funcNet(mywk)){
      return GFL_PROC_RES_FINISH;
    }
  }
  return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**  �f�o�b�O�pExit
 */
//------------------------------------------------------------------
static GFL_PROC_RESULT DebugOhnoMainProcEnd(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  GFL_PROC_FreeWork(proc);
  GFL_HEAP_DeleteHeap( HEAPID_OHNO_DEBUG );

  return GFL_PROC_RES_FINISH;
}


static GFL_PROC_RESULT NetFourAutoProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  return DebugOhnoMainProcInit(proc, seq, pwk, mywk,FALSE);
}


//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_PROC_DATA DebugOhnoMainProcData = {
  NetFourAutoProc_Init,
  DebugOhnoMainProcMain,
  DebugOhnoMainProcEnd,
};



///<
//--------------------------------------------------------------
/**
 * @brief   �n�[�h�Őڑ��������ɌĂ΂��
 * @param   pWork    �f�o�b�O���[�N
 * @param   �Ȃ������@���ID
 * @retval  none
 */
//--------------------------------------------------------------
void _netConnectFunc(void* pWork,int hardID)
{
  NET_PRINT("_netConnectFunc %d\n", hardID);

}

//--------------------------------------------------------------
/**
 * @brief   �l�S�V�G�[�V�����������ɃR�[��
 * @param   pWork    �f�o�b�O���[�N
 * @param   �Ȃ������@���ID
 * @retval  none
 */
//--------------------------------------------------------------
void _netNegotiationFunc(void* pWork,int NetID)
{

  NET_PRINT("_netNegotiationFunc %d\n", NetID);

}



static GFL_PROC_RESULT NetFourParentProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  return DebugOhnoMainProcInit(proc, seq, pwk, mywk,TRUE);
}

static GFL_PROC_RESULT NetFourChildProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  return DebugOhnoMainProcInit(proc, seq, pwk, mywk,FALSE);
}






// �v���Z�X��`�f�[�^
const GFL_PROC_DATA NetFourParentProcData = {
  NetFourParentProc_Init,
  DebugOhnoMainProcMain,
  DebugOhnoMainProcEnd,
};

// �v���Z�X��`�f�[�^
const GFL_PROC_DATA NetFourChildProcData = {
  NetFourChildProc_Init,
  DebugOhnoMainProcMain,
  DebugOhnoMainProcEnd,
};

//--------------------------------------------------------------------

static void _fushigiDataSet(DEBUG_OHNO_CONTROL * pDOC)
{
  DOWNLOAD_GIFT_DATA* pDG;

  
  pDOC->aInit.NetDevID = WB_NET_MYSTERY;   // //�ʐM���
  pDOC->aInit.datasize = sizeof(DOWNLOAD_GIFT_DATA);   //�f�[�^�S�̃T�C�Y
  pDOC->aInit.pData = GFL_HEAP_AllocClearMemory(HEAPID_OHNO_DEBUG,pDOC->aInit.datasize);     // �f�[�^
  pDOC->aInit.ConfusionID = 12;
  pDOC->aInit.heapID = HEAPID_OHNO_DEBUG;

  pDG = (DOWNLOAD_GIFT_DATA* )pDOC->aInit.pData;

  DEBUG_MYSTERY_SetGiftCommonData( &pDG->data, 12, FALSE );
  DEBUG_MYSTERY_SetGiftPokeData(&pDG->data,12);
  pDG->version = 12;
  pDG->event_text[0] = L'��';
  pDG->event_text[1] = L'��';
  pDG->event_text[2] = L'��';
  pDG->event_text[3] = 0xffff;

}


static BOOL _loop(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;
  DELIVERY_BEACON_Main(pDOC->pDBWork);
  return FALSE;
}


static GFL_PROC_RESULT NetDeliverySendProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  DEBUG_OHNO_CONTROL * pDOC;

  
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OHNO_DEBUG, 0x30000 );
  pDOC = GFL_PROC_AllocWork( proc, sizeof(DEBUG_OHNO_CONTROL), HEAPID_OHNO_DEBUG );
  GFL_STD_MemClear(pDOC, sizeof(DEBUG_OHNO_CONTROL));

  
  {

    _fushigiDataSet(pDOC);




    
    pDOC->pDBWork=DELIVERY_BEACON_Init(&pDOC->aInit);
    GF_ASSERT(DELIVERY_BEACON_SendStart(pDOC->pDBWork));
  }
  _CHANGE_STATE( _loop ); // 

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------------------------------------WIRLESS




static BOOL _getTime(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;
  int i,j;

  DELIVERY_BEACON_Main(pDOC->pDBWork);
  
  pDOC->counter++;

  if(DELIVERY_BEACON_RecvCheck(pDOC->pDBWork) ){  // �����ʐM���Ă���ꍇ�I������
    OS_TPrintf("��M���� %d\n",pDOC->counter);

    for(j=0;j<pDOC->aInit.datasize;){
      for(i=0;i<16;i++){
        OS_TPrintf("%x ",pDOC->aInit.pData[j]);
        j++;
      }
      OS_TPrintf("\n");
    }
    DELIVERY_BEACON_End(pDOC->pDBWork);
    _CHANGE_STATE(NetTestNone);
  }
  return FALSE;
}


static void _fushigiDataRecv(DEBUG_OHNO_CONTROL * pDOC)
{
  DOWNLOAD_GIFT_DATA* pDG;

  pDOC->aInit.NetDevID = WB_NET_MYSTERY;   // //�ʐM���
  pDOC->aInit.datasize = sizeof(DOWNLOAD_GIFT_DATA);   //�f�[�^�S�̃T�C�Y
  pDOC->aInit.pData = GFL_HEAP_AllocClearMemory(HEAPID_OHNO_DEBUG,pDOC->aInit.datasize);     // �f�[�^
  pDOC->aInit.ConfusionID = 12;
  pDOC->aInit.heapID = HEAPID_OHNO_DEBUG;

}



static GFL_PROC_RESULT NetDeliveryRecvProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  DEBUG_OHNO_CONTROL * pDOC;

  
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OHNO_DEBUG, 0x30000 );
  pDOC = GFL_PROC_AllocWork( proc, sizeof(DEBUG_OHNO_CONTROL), HEAPID_OHNO_DEBUG );
  GFL_STD_MemClear(pDOC, sizeof(DEBUG_OHNO_CONTROL));

  
  {
    _fushigiDataRecv(pDOC);
    
    pDOC->pDBWork=DELIVERY_BEACON_Init(&pDOC->aInit);
    GF_ASSERT(DELIVERY_BEACON_RecvStart(pDOC->pDBWork));
  }
  pDOC->counter=0;
  _CHANGE_STATE( _getTime ); // 


  return GFL_PROC_RES_FINISH;
}


// �v���Z�X��`�f�[�^
const GFL_PROC_DATA NetDeliverySendProcData = {
  NetDeliverySendProc_Init,
  DebugOhnoMainProcMain,
  DebugOhnoMainProcEnd,
};



// �v���Z�X��`�f�[�^
const GFL_PROC_DATA NetDeliveryRecvProcData = {
  NetDeliveryRecvProc_Init,
  DebugOhnoMainProcMain,
  DebugOhnoMainProcEnd,
};








//--------------------------------------------------------------------IRC

//-------------------------------------------------------------------IRC




static BOOL _getIRCTime(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;
  int i,j,k;

  DELIVERY_IRC_Main(pDOC->pIRCWork);
  
  pDOC->counter++;
  k = DELIVERY_IRC_RecvCheck(pDOC->pIRCWork);
  if( DELIVERY_IRC_RecvCheck(pDOC->pIRCWork) ){  // �����ʐM���Ă���ꍇ�I������
    OS_TPrintf("��M���� %d %d\n",pDOC->counter,k);

    for(j=0;j<pDOC->aInit.datasize;){
      for(i=0;i<16;i++){
        OS_TPrintf("%x ",pDOC->aIRCInit.pData[j]);
        j++;
      }
      OS_TPrintf("\n");
    }
    DELIVERY_IRC_End(pDOC->pIRCWork);
    _CHANGE_STATE(NetTestNone);
  }
  return FALSE;
}


static void _fushigiDataIRCRecv(DEBUG_OHNO_CONTROL * pDOC)
{
  DOWNLOAD_GIFT_DATA* pDG;

  pDOC->aIRCInit.NetDevID = WB_NET_MYSTERY;   // //�ʐM���
  pDOC->aIRCInit.datasize = sizeof(DOWNLOAD_GIFT_DATA);   //�f�[�^�S�̃T�C�Y
  pDOC->aIRCInit.pData = GFL_HEAP_AllocClearMemory(HEAPID_OHNO_DEBUG,pDOC->aIRCInit.datasize);     // �f�[�^
  pDOC->aIRCInit.ConfusionID = 12;
  pDOC->aIRCInit.heapID = HEAPID_OHNO_DEBUG;

}



static GFL_PROC_RESULT NetDeliveryIRCRecvProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  DEBUG_OHNO_CONTROL * pDOC;

  
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OHNO_DEBUG, 0x30000 );
  pDOC = GFL_PROC_AllocWork( proc, sizeof(DEBUG_OHNO_CONTROL), HEAPID_OHNO_DEBUG );
  GFL_STD_MemClear(pDOC, sizeof(DEBUG_OHNO_CONTROL));

  
  {
    _fushigiDataIRCRecv(pDOC);
    
    pDOC->pIRCWork=DELIVERY_IRC_Init(&pDOC->aIRCInit);
    GF_ASSERT(DELIVERY_IRC_RecvStart(pDOC->pIRCWork));
  }
  pDOC->counter=0;
  _CHANGE_STATE( _getIRCTime ); // 


  return GFL_PROC_RES_FINISH;
}



static void _fushigiDataIRCSet(DEBUG_OHNO_CONTROL * pDOC)
{
  DOWNLOAD_GIFT_DATA* pDG;

  pDOC->aIRCInit.NetDevID = WB_NET_MYSTERY;   // //�ʐM���
  pDOC->aIRCInit.datasize = sizeof(DOWNLOAD_GIFT_DATA);   //�f�[�^�S�̃T�C�Y
  pDOC->aIRCInit.pData = GFL_HEAP_AllocClearMemory(HEAPID_OHNO_DEBUG,pDOC->aIRCInit.datasize);     // �f�[�^
  pDOC->aIRCInit.ConfusionID = 12;
  pDOC->aIRCInit.heapID = HEAPID_OHNO_DEBUG;

  pDG = (DOWNLOAD_GIFT_DATA* )pDOC->aIRCInit.pData;

  DEBUG_MYSTERY_SetGiftCommonData( &pDG->data, 12, FALSE );
  DEBUG_MYSTERY_SetGiftPokeData(&pDG->data,12);
  pDG->version = 12;
  pDG->event_text[0] = L'I';
  pDG->event_text[1] = L'R';
  pDG->event_text[2] = L'C';
  pDG->event_text[3] = 0xffff;

}


static BOOL _loopirc(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;
  DELIVERY_IRC_Main(pDOC->pIRCWork);
  return FALSE;
}


static GFL_PROC_RESULT NetDeliveryIRCSendProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  DEBUG_OHNO_CONTROL * pDOC;

  
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OHNO_DEBUG, 0x30000 );
  pDOC = GFL_PROC_AllocWork( proc, sizeof(DEBUG_OHNO_CONTROL), HEAPID_OHNO_DEBUG );
  GFL_STD_MemClear(pDOC, sizeof(DEBUG_OHNO_CONTROL));

  {

    _fushigiDataIRCSet(pDOC);
    
    pDOC->pIRCWork=DELIVERY_IRC_Init(&pDOC->aIRCInit);
    GF_ASSERT(DELIVERY_IRC_SendStart(pDOC->pIRCWork));
  }
  _CHANGE_STATE( _loopirc ); // 

  return GFL_PROC_RES_FINISH;
}

// �v���Z�X��`�f�[�^
const GFL_PROC_DATA NetDeliveryIRCSendProcData = {
  NetDeliveryIRCSendProc_Init,
  DebugOhnoMainProcMain,
  DebugOhnoMainProcEnd,
};



// �v���Z�X��`�f�[�^
const GFL_PROC_DATA NetDeliveryIRCRecvProcData = {
  NetDeliveryIRCRecvProc_Init,
  DebugOhnoMainProcMain,
  DebugOhnoMainProcEnd,
};


