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

#include "savedata/bsubway_savedata.h"
#include "savedata/battle_examination.h"
#include "waza_tool/wazano_def.h"
#include "battle/bsubway_battle_data.h"

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
static void _debug_bsubData(BSUBWAY_PARTNER_DATA* pData,BOOL bSingle);


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
  SYASHI_NETWORK_GGID,  //ggid  DP=0x333,RANGER=0x178,WII=0x346
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
//��������s�v�c�ȑ��蕨
//--------------------------------------------------------------------
#include "item/itemsym.h"

//�s�v�c�ȑ��蕨�̃f�[�^���Z�b�g
static void _fushigiDataSet(DEBUG_OHNO_CONTROL * pDOC)
{
  static const int sc_debug_data_max  = 2;

  DOWNLOAD_GIFT_DATA* pDG;

  static const int sc_lang_tbl[sc_debug_data_max]  =
  { 
    LANG_JAPAN,
    LANG_ENGLISH,
  };

  static const u32 sc_version_tbl[sc_debug_data_max]  =
  { 
    1<<VERSION_BLACK,
    1<<VERSION_WHITE,
  };
  int i;

  
  pDOC->aInit.NetDevID = WB_NET_MYSTERY;   // //�ʐM���

  pDOC->aInit.dataNum = sc_debug_data_max;
  pDOC->aInit.ConfusionID = 12;
  pDOC->aInit.heapID = HEAPID_OHNO_DEBUG;

  for( i = 0; i <sc_debug_data_max; i++ )
  { 
    pDOC->aInit.data[i].datasize = sizeof(DOWNLOAD_GIFT_DATA);   //�f�[�^�S�̃T�C�Y
    pDOC->aInit.data[i].pData = GFL_HEAP_AllocClearMemory(HEAPID_OHNO_DEBUG,pDOC->aInit.data[i].datasize);     // �f�[�^
    pDOC->aInit.data[i].LangCode  = sc_lang_tbl[ i ];
    pDOC->aInit.data[i].version   = sc_version_tbl[ i ];
    pDG = (DOWNLOAD_GIFT_DATA* )pDOC->aInit.data[i].pData;

    GFL_STD_MemClear( pDG, sizeof(DOWNLOAD_GIFT_DATA) );
    DEBUG_MYSTERY_SetGiftItemData(&pDG->data, ITEM_RIBATHITIKETTO );
    DEBUG_MYSTERY_SetGiftCommonData( &pDG->data, MYSTERY_DATA_EVENT_LIBERTY, TRUE );
    pDG->LangCode = pDOC->aInit.data[i].LangCode;
    pDG->version  = pDOC->aInit.data[i].version;
    pDG->event_text[0] = L'��';
    pDG->event_text[1] = L'�o';
    pDG->event_text[2] = L'�e';
    pDG->event_text[3] = L'�B';
    pDG->event_text[4] = L'�`';
    pDG->event_text[5] = L'�P';
    pDG->event_text[6] = L'�b';
    pDG->event_text[7] = L'�g';
    pDG->event_text[8] = L'��';
    pDG->event_text[9] = L'��';
    pDG->event_text[10] = L'��';
    pDG->event_text[11] = L'��';
    pDG->event_text[12] = L'��';
    pDG->event_text[13] = L'��';
    pDG->event_text[14] = 0xffff;
    pDG->crc  = GFL_STD_CrcCalc( pDOC->aInit.data[i].pData, sizeof(DOWNLOAD_GIFT_DATA) - 2 );

    {
      int k,j;
      for(j=0;j<pDOC->aInit.data[0].datasize;){
        for(k=0;k<16;k++){
          OS_TPrintf("%x ",pDOC->aInit.data[0].pData[j]);
          j++;
        }
        OS_TPrintf("\n");
      }
    }
  }
}

//�e�@�Ȃ̂Ńr�[�R�����M���s���Ă��邾��
static BOOL _loop(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;
  DELIVERY_BEACON_Main(pDOC->pDBWork);
  return FALSE;
}

//�s�v�c�ȑ��蕨���M�ŏ�
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


//�g���C�A���n�E�X�_�~�[�f�[�^
static void _debug_bsubDataMain(DEBUG_OHNO_CONTROL * pDOC,int type, int no)
{
  BATTLE_EXAMINATION_SAVEDATA* pDG;
  BOOL bSingle = FALSE;

  
  pDOC->aInit.NetDevID = WB_NET_BATTLE_EXAMINATION;   // //�ʐM���
  pDOC->aInit.data[0].datasize = sizeof(BATTLE_EXAMINATION_SAVEDATA);   //�f�[�^�S�̃T�C�Y
  pDOC->aInit.data[0].pData = GFL_HEAP_AllocClearMemory(HEAPID_OHNO_DEBUG,pDOC->aInit.data[0].datasize);     // �f�[�^
  pDOC->aInit.data[0].LangCode  = CasetteLanguage;
  pDOC->aInit.data[0].version   = 1<<GET_VERSION();
  pDOC->aInit.dataNum = 1;
  pDOC->aInit.ConfusionID = 12;
  pDOC->aInit.heapID = HEAPID_OHNO_DEBUG;

  pDG = (BATTLE_EXAMINATION_SAVEDATA* )pDOC->aInit.data[0].pData;

  pDG->bActive = BATTLE_EXAMINATION_MAGIC_KEY;
  if(type==0){
    pDG->dataNo = no;  //�P�Q�͉��̊J�Ôԍ�  SINGLE
    bSingle=TRUE;
  }
  else{
    pDG->dataNo = 0x8000 + no;  //�P�Q�͉��̊J�Ôԍ�  DOUBLE
    bSingle=FALSE;
  }

  _debug_bsubData( &pDG->trainer[0],bSingle );
  _debug_bsubData( &pDG->trainer[1],bSingle );
  _debug_bsubData( &pDG->trainer[2],bSingle );
  _debug_bsubData( &pDG->trainer[3],bSingle );
  _debug_bsubData( &pDG->trainer[4],bSingle );
  pDG->titleName[0] = L'��';
  pDG->titleName[1] = L'��';
  pDG->titleName[2] = 0xffff;
  pDG->crc = GFL_STD_CrcCalc(pDOC->aInit.data[0].pData, pDOC->aInit.data[0].datasize-2);

}




//�g���C�A���n�E�X���r�[�R���z�M
static GFL_PROC_RESULT NetDeliveryTriSendProc_Init(GFL_PROC * proc, int * seq, void * pwk, void * mywk)
{
  DEBUG_OHNO_CONTROL * pDOC;
  DEBUG_TRIAL_PARAM* pTri = (DEBUG_TRIAL_PARAM*)pwk;
  
  
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OHNO_DEBUG, 0x30000 );
  pDOC = GFL_PROC_AllocWork( proc, sizeof(DEBUG_OHNO_CONTROL), HEAPID_OHNO_DEBUG );
  GFL_STD_MemClear(pDOC, sizeof(DEBUG_OHNO_CONTROL));

  {
    //@todo �����Ƀg���C�A���n�E�X�e�X�g�f�[�^������
    _debug_bsubDataMain(pDOC,pTri->trialType,pTri->trialNo);

    
    pDOC->pDBWork=DELIVERY_BEACON_Init(&pDOC->aInit);
    GF_ASSERT(DELIVERY_BEACON_SendStart(pDOC->pDBWork));
  }
  _CHANGE_STATE( _loop ); // 

  return GFL_PROC_RES_FINISH;
}



//-------------------------------------------------------------------WIRLESS



// ������͎�M�e�X�g
static BOOL _getTime(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = pCtl;
  int i,j;

  DELIVERY_BEACON_Main(pDOC->pDBWork);
  
  pDOC->counter++;

  if(DELIVERY_BEACON_RecvCheck(pDOC->pDBWork) ){  // �����ʐM���Ă���ꍇ�I������
    OS_TPrintf("��M���� %d\n",pDOC->counter);

    for(j=0;j<pDOC->aInit.data[0].datasize;){
      for(i=0;i<16;i++){
        OS_TPrintf("%x ",pDOC->aInit.data[0].pData[j]);
        j++;
      }
      OS_TPrintf("\n");
    }
    DELIVERY_BEACON_End(pDOC->pDBWork);
    _CHANGE_STATE(NetTestNone);
  }
  return FALSE;
}

//��M�̂��߂ɗ̈�m��
static void _fushigiDataRecv(DEBUG_OHNO_CONTROL * pDOC)
{
  DOWNLOAD_GIFT_DATA* pDG;

  pDOC->aInit.NetDevID = WB_NET_MYSTERY;   // //�ʐM���
  pDOC->aInit.data[0].datasize = sizeof(DOWNLOAD_GIFT_DATA);   //�f�[�^�S�̃T�C�Y
  pDOC->aInit.data[0].pData = GFL_HEAP_AllocClearMemory(HEAPID_OHNO_DEBUG,pDOC->aInit.data[0].datasize);     // �f�[�^
  pDOC->aInit.data[0].LangCode  = CasetteLanguage;
  pDOC->aInit.data[0].version   = 1<<GET_VERSION();
  pDOC->aInit.dataNum = 1;
  pDOC->aInit.ConfusionID = 12;
  pDOC->aInit.heapID = HEAPID_OHNO_DEBUG;

}

//��M�̂��߂ɗ̈�m��
static void _traialDataRecv(DEBUG_OHNO_CONTROL * pDOC)
{
  DOWNLOAD_GIFT_DATA* pDG;

  pDOC->aInit.NetDevID = WB_NET_BATTLE_EXAMINATION;   // //�ʐM���
  pDOC->aInit.data[0].datasize = sizeof(BATTLE_EXAMINATION_SAVEDATA);   //�f�[�^�S�̃T�C�Y
  pDOC->aInit.data[0].pData = GFL_HEAP_AllocClearMemory(HEAPID_OHNO_DEBUG,pDOC->aInit.data[0].datasize);     // �f�[�^
  pDOC->aInit.data[0].LangCode  = CasetteLanguage;
  pDOC->aInit.data[0].version   = 1<<GET_VERSION();
  pDOC->aInit.dataNum = 1;
  pDOC->aInit.ConfusionID = 12;
  pDOC->aInit.heapID = HEAPID_OHNO_DEBUG;

}

//��M�̏�����
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


// �v���Z�X��`�f�[�^ �r�[�R���z�M
const GFL_PROC_DATA NetDeliverySendProcData = {
  NetDeliverySendProc_Init,
  DebugOhnoMainProcMain,
  DebugOhnoMainProcEnd,
};


// �v���Z�X��`�f�[�^  �r�[�R����M
const GFL_PROC_DATA NetDeliveryRecvProcData = {
  NetDeliveryRecvProc_Init,
  DebugOhnoMainProcMain,
  DebugOhnoMainProcEnd,
};



// �v���Z�X��`�f�[�^  �g���C�A���n�E�X�r�[�R���z�M
const GFL_PROC_DATA NetDeliveryTriSendProcData = {
  NetDeliveryTriSendProc_Init,
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
  if( k != DELIVERY_IRC_FUNC ){  // �����ʐM���Ă���ꍇ�I������
    OS_TPrintf("��M���� %d %d\n",pDOC->counter,k);

    for(j=0;j<pDOC->aIRCInit.data[0].datasize;){
      for(i=0;i<16;i++){
        OS_TPrintf("%x ",pDOC->aIRCInit.data[0].pData[j]);
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

  //��M���͂P�̃o�b�t�@��OK
  pDOC->aIRCInit.data[0].datasize = sizeof(DOWNLOAD_GIFT_DATA);   //�f�[�^�S�̃T�C�Y
  pDOC->aIRCInit.data[0].pData = GFL_HEAP_AllocClearMemory(HEAPID_OHNO_DEBUG,pDOC->aIRCInit.data[0].datasize);     // �f�[�^
  pDOC->aIRCInit.data[0].LangCode = CasetteLanguage;   //����R�[�h
  pDOC->aIRCInit.data[0].version  = 1<<VERSION_WHITE;   //�o�[�W�����̃r�b�g

  pDOC->aIRCInit.ConfusionID = 12;
  pDOC->aIRCInit.heapID = HEAPID_OHNO_DEBUG;
  pDOC->aIRCInit.dataNum = 1;
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

//�ԊO���p�e�X�g�f�[�^�Z�b�g
static void _fushigiDataIRCSet(DEBUG_OHNO_CONTROL * pDOC)
{
  static const int sc_lang_tbl[7]  =
  { 
    LANG_JAPAN,
    LANG_ENGLISH,
    LANG_FRANCE,
    LANG_ITALY,
    LANG_JAPAN,
    LANG_SPAIN,
    LANG_KOREA,
  };

  static const u32 sc_version_tbl[7]  =
  { 
    1<<VERSION_WHITE,
    1<<VERSION_WHITE,
    1<<VERSION_WHITE,
    1<<VERSION_WHITE,
    1<<VERSION_BLACK,
    1<<VERSION_BLACK,
    1<<VERSION_BLACK,
  };

  int i;

  DOWNLOAD_GIFT_DATA* pDG;

  pDOC->aIRCInit.NetDevID = WB_NET_MYSTERY;   // //�ʐM���
  pDOC->aIRCInit.ConfusionID = 12;
  pDOC->aIRCInit.heapID = HEAPID_OHNO_DEBUG;
  pDOC->aIRCInit.dataNum = 7;
  for( i = 0; i < 7; i++  )
  { 

    pDOC->aIRCInit.data[i].datasize = sizeof(DOWNLOAD_GIFT_DATA);   //�f�[�^�S�̃T�C�Y
    pDOC->aIRCInit.data[i].pData = GFL_HEAP_AllocClearMemory(HEAPID_OHNO_DEBUG,pDOC->aIRCInit.data[0].datasize);     // �f�[�^
    pDOC->aIRCInit.data[i].LangCode = sc_lang_tbl[i];
    pDOC->aIRCInit.data[i].version  = sc_version_tbl[i];

    pDG = (DOWNLOAD_GIFT_DATA* )pDOC->aIRCInit.data[i].pData;

    GFL_STD_MemClear( pDG, sizeof(DOWNLOAD_GIFT_DATA) );
    DEBUG_MYSTERY_SetGiftPokeData(&pDG->data);
    DEBUG_MYSTERY_SetGiftCommonData( &pDG->data, 12, FALSE );
    pDG->version = sc_version_tbl[i];
    pDG->LangCode = sc_lang_tbl[i];
    pDG->event_text[0] = L'��';
    pDG->event_text[1] = L'��';
    pDG->event_text[2] = L'��';
    pDG->event_text[3] = 0xffff;
    pDG->crc  = GFL_STD_CrcCalc( pDOC->aIRCInit.data[i].pData, sizeof(DOWNLOAD_GIFT_DATA) - 2 );

    OS_TPrintf("%d�ڃX�^�[�g�F���ꁁ%d �o�[�W����0x%x\n", i, pDOC->aIRCInit.data[i].LangCode, pDOC->aIRCInit.data[i].version);
    {
      int j,k;
      for(j=0;j<pDOC->aIRCInit.data[i].datasize;){
        for(k=0;k<16;k++){
          OS_TPrintf("%x ",pDOC->aIRCInit.data[i].pData[j]);
          j++;
        }
        OS_TPrintf("\n");
      }
   }

  }

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

// �v���Z�X��`�f�[�^   �ԊO���z�M����
const GFL_PROC_DATA NetDeliveryIRCSendProcData = {
  NetDeliveryIRCSendProc_Init,
  DebugOhnoMainProcMain,
  DebugOhnoMainProcEnd,
};



// �v���Z�X��`�f�[�^  �ԊO����M�e�X�g
const GFL_PROC_DATA NetDeliveryIRCRecvProcData = {
  NetDeliveryIRCRecvProc_Init,
  DebugOhnoMainProcMain,
  DebugOhnoMainProcEnd,
};




///------------------------------�o�g������








#if 0
//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�����p�g���[�i�[�f�[�^�\����
/// �g���[�i�[���A��b�f�[�^�́Agmm�ŊǗ��i�g���[�i�[ID����擾�j
//--------------------------------------------------------------
typedef struct
{
	u16		tr_type;				//�g���[�i�[�^�C�v
	u16		use_poke_cnt;			//�g�p�\�|�P������	
	u16		use_poke_table[1];		//�g�p�\�|�P����INDEX�e�[�u���i�ϒ��j
}BSUBWAY_TRAINER_ROM_DATA;

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�����p�|�P�����f�[�^�\����
//--------------------------------------------------------------
typedef struct
{
	u16		mons_no;				//�����X�^�[�i���o�[
	u16		waza[WAZA_TEMOTI_MAX];	//�����Z
	u8		exp_bit;				//�w�͒l�U�蕪���r�b�g
	u8		chr;					//���i
	u16		item_no;				//��������
	u16		form_no;				//�t�H�����i���o�[
}BSUBWAY_POKEMON_ROM_DATA;

//--------------------------------------------------------------
/// Wifi�f�[�^
/// �T�u�E�F�C�p�|�P�����f�[�^�^
/// �Z�[�u�f�[�^�Ƃ���肷��̂�savedata/b_tower.h��typedef��`��؂�
/// �s���S�|�C���^�ł����ł���悤�ɂ��Ă���
//(Dpw_Bt_PokemonData)
//--------------------------------------------------------------
struct _BSUBWAY_POKEMON
{
	union{
		struct{
			u16	mons_no:11;	///<�����X�^�[�i���o�[
			u16	form_no:5;	///<�����X�^�[�i���o�[
		};
		u16	mons_param;
	};
	u16	item_no;	///<��������

	u16	waza[WAZA_TEMOTI_MAX];		///<�����Z

	u32	id_no;					///<IDNo
	u32	personal_rnd;			///<������

	union{
		struct{
		u32	hp_rnd		:5;		///<HP�p���[����
		u32	pow_rnd		:5;		///<POW�p���[����
		u32	def_rnd		:5;		///<DEF�p���[����
		u32	agi_rnd		:5;		///<AGI�p���[����
		u32	spepow_rnd	:5;		///<SPEPOW�p���[����
		u32	spedef_rnd	:5;		///<SPEDEF�p���[����
		u32	ngname_f	:1;		///<NG�l�[���t���O
		u32				:1;		//1ch �]��
		};
		u32 power_rnd;
	};

	union{
		struct{
		u8	hp_exp;				///<HP�w�͒l
		u8	pow_exp;			///<POW�w�͒l
		u8	def_exp;			///<DEF�w�͒l
		u8	agi_exp;			///<AGI�w�͒l
		u8	spepow_exp;			///<SPEPOW�w�͒l
		u8	spedef_exp;			///<SPEDEF�w�͒l
		};
		u8 exp[6];
	};
	union{
		struct{
		u8	pp_count0:2;	///<�Z1�|�C���g�A�b�v
		u8	pp_count1:2;	///<�Z2�|�C���g�A�b�v
		u8	pp_count2:2;	///<�Z3�|�C���g�A�b�v
		u8	pp_count3:2;	///<�Z4�|�C���g�A�b�v
		};
		u8 pp_count;
	};

	u8	country_code;			///<���R�[�h

	u8	tokusei;				///<����
	u8	natuki;				///<�Ȃ��x

	///�j�b�N�l�[�� ((MONS_NAME_SIZE:10)+(EOM_SIZE:1))*(STRCODE:u16)=22
	STRCODE nickname[MONS_NAME_SIZE+EOM_SIZE];
};

//--------------------------------------------------------------
/// �T�u�E�F�C�g���[�i�[�f�[�^
//--------------------------------------------------------------
typedef struct _BSUBWAY_TRAINER
{
	u32		player_id;	///<�g���[�i�[��ID
	u16		tr_type;	///<�g���[�i�[�^�C�v
	u16		dummy;		///<�_�~�[
	STRCODE	name[PERSON_NAME_SIZE+EOM_SIZE];
	
	u16		appear_word[4];						//�o�ꃁ�b�Z�[�W	
	u16		win_word[4];						//�������b�Z�[�W	
	u16		lose_word[4];						//�s�ރ��b�Z�[�W
}BSUBWAY_TRAINER;

//--------------------------------------------------------------
/// �o�g���T�u�E�F�C�@�ΐ푊��f�[�^�\����
//--------------------------------------------------------------
struct _BSUBWAY_PARTNER_DATA
{
	BSUBWAY_TRAINER	bt_trd;			//�g���[�i�[�f�[�^
	struct _BSUBWAY_POKEMON	btpwd[4];		//�����|�P�����f�[�^
};


#endif


static void _debug_bsubData(BSUBWAY_PARTNER_DATA* pData, BOOL bSingle)
{
  int i;

  pData->bt_trd.player_id =10;
  pData->bt_trd.tr_type = 20;
  pData->bt_trd.name[0]= L'��';
  pData->bt_trd.name[1]= L'��';
  pData->bt_trd.name[2]= L'��';
  pData->bt_trd.name[3]= 0xffff;
  pData->bt_trd.appear_word[0]=1;
  pData->bt_trd.appear_word[1]=2;
  pData->bt_trd.appear_word[2]=3;
  pData->bt_trd.appear_word[3]=4;
  pData->bt_trd.win_word[0]=1;
  pData->bt_trd.win_word[1]=2;
  pData->bt_trd.win_word[2]=3;
  pData->bt_trd.win_word[3]=4;
  pData->bt_trd.lose_word[0]=1;
  pData->bt_trd.lose_word[1]=2;
  pData->bt_trd.lose_word[2]=3;
  pData->bt_trd.lose_word[3]=4;
  if(bSingle){
    for(i=0;i<3;i++){
      BSUBWAY_POKEMON* pPoke = &pData->btpwd[i];
      pPoke->mons_param = 101;
      pPoke->item_no=0;
      pPoke->waza[0]=WAZANO_KAMINARI;
      pPoke->waza[1]=WAZANO_NULL;
      pPoke->waza[2]=WAZANO_NULL;
      pPoke->waza[3]=WAZANO_NULL;
      pPoke->id_no = 12;
      pPoke->nickname[0]= L'��';
      pPoke->nickname[1]= 0xffff;
      pPoke->seikaku = 0;

      OS_TPrintf("�o�g���T�u�E�F�C %d\n" ,sizeof(BSUBWAY_POKEMON));

    }
  }
  else{
    for(i=0;i<4;i++){
      BSUBWAY_POKEMON* pPoke = &pData->btpwd[i];
      pPoke->mons_param = 1;
      pPoke->item_no=0;
      pPoke->waza[0]=WAZANO_HAPPAKATTAA;
      pPoke->waza[1]=WAZANO_NULL;
      pPoke->waza[2]=WAZANO_NULL;
      pPoke->waza[3]=WAZANO_NULL;
      pPoke->id_no = 12;
      pPoke->nickname[0]=0xffff;
    }
  }
}

