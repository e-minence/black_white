//============================================================================================
/**
 * @file	  debug_fight_comm.c
 * @brief	  �f�o�b�O�ʐM�ڑ��֐�
 * @author	ohno
 * @date	  2009.12.25
 */
//============================================================================================


#include <gflib.h>
#include "procsys.h"
#include "system/main.h"
#include "net\network_define.h"
#include "net\dwc_raputil.h"
#include "wmi.naix"
#include "debug_fight_comm.h"
#include "system\net_err.h"

typedef BOOL (*NetTestFunc)(void* pCtl);

typedef struct {
  NetTestFunc funcNet;
  BOOL bParent;
  int num;
}DEBUG_OHNO_CONTROL;

static DEBUG_OHNO_CONTROL  DebugOhnoControl;

//------------------------------------------------------------------
//  ��M�e�[�u��
//------------------------------------------------------------------


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
static void _netConnectFunc(void* pWork,int hardID);    ///< �n�[�h�Őڑ��������ɌĂ΂��
static void _netNegotiationFunc(void* pWork,int hardID);    ///< �l�S�V�G�[�V�����������ɃR�[��
static BOOL _NetTestChild(void* pCtl);
static BOOL _NetTestParent(void* pCtl);

static void _endCallBack(void* pWork);
static u8* _recvMemory(int netID, void* pWork, int size);



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
  NET_PRINT("DFCS: %d\n",line);
  _changeState(pDOC, state);
}
#endif

#ifdef GFL_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeStateDebug(pDOC ,state,  __LINE__)
#else  //GFL_NET_DEBUG
#define   _CHANGE_STATE(state)  _changeState(pDOC ,state )
#endif //GFL_NET_DEBUG




//------------------------------------------------------------------
/**   �f�o�b�O�p���C��
 */
//------------------------------------------------------------------
BOOL DFC_NET_Process(void)
{
  DEBUG_OHNO_CONTROL * testmode =  &DebugOhnoControl;

  if(testmode->funcNet){
    if(testmode->funcNet(testmode)){
      return TRUE;
    }
    
    if(GFL_NET_SystemGetErrorCode()!=0){
      OS_TPrintf("Err%d\n",GFL_NET_SystemGetErrorCode());
    }
    
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
  DEBUG_OHNO_CONTROL* pDOC =  &DebugOhnoControl;

  if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),15)){
    NET_PRINT("TIMOK\n");
    _CHANGE_STATE( NULL );
    return TRUE;
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
  DEBUG_OHNO_CONTROL* pDOC =  &DebugOhnoControl;
 
  if(GFL_NET_HANDLE_IsNegotiation( GFL_NET_HANDLE_GetCurrentHandle() ) ){
    if(GFL_NET_HANDLE_IsNegotiation( GFL_NET_GetNetHandle(0) ) ){
      GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,15);
      _CHANGE_STATE( NetTestRecvTiming );
    }
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
  DEBUG_OHNO_CONTROL* pDOC =  &DebugOhnoControl;

  if(GFL_NET_GetConnectNum()==(pDOC->num-1)){
    if(GFL_NET_HANDLE_RequestNegotiation()){    
      GFL_NET_SetNoChildErrorCheck(TRUE);
      GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle() ,15);
      _CHANGE_STATE( NetTestRecvTiming );
    }
  }
  return FALSE;
}



static BOOL _NetTestChild(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC =  &DebugOhnoControl;

  if(NetErr_App_CheckError() != NET_ERR_STATUS_NULL)
  {
    //�Đڑ�
    GFLNetInitializeStruct pNetInit;
    void *pWork = GFL_NET_GetWork();
    GFL_STD_MemCopy( GFL_NET_GetNETInitStruct() , &pNetInit , sizeof(GFLNetInitializeStruct) );
    
    NetErr_App_ReqErrorDisp();
    NetErr_DispCall(FALSE);
    
    _CHANGE_STATE( NULL );
    DFC_NET_Init( &pNetInit , NULL , pWork , pDOC->bParent , pDOC->num );
  }

  
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
static BOOL NetTestAutoConnect(void* pCtl)
{
  DEBUG_OHNO_CONTROL* pDOC = &DebugOhnoControl;

  if(pDOC->bParent){
    GFL_NET_InitServer();
    _CHANGE_STATE( _NetTestParent );
  }
  else{
    u8 mac[]={0xff,0xff,0xff,0xff,0xff,0xff};
    GFL_NET_InitClientAndConnectToParent(mac);

    _CHANGE_STATE( _NetTestChild );
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

static void _initCallBack(void* pWork)
{
  DEBUG_OHNO_CONTROL* pDOC = &DebugOhnoControl;

  _CHANGE_STATE( NetTestAutoConnect );
}




void DFC_NET_Init(const GFLNetInitializeStruct* pNetInit, NetStepEndCallback callback, void* pWork, BOOL bParent,int num)
{
  DebugOhnoControl.bParent = bParent;
  DebugOhnoControl.num=num;
  GFL_NET_Init(pNetInit, _initCallBack, pWork);
  GFL_NET_SetNoChildErrorCheck(TRUE);
}


