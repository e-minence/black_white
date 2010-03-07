//=============================================================================
/**
 * @file	net_devwifi.c
 * @bfief	WIFI�ʐM�h���C�o�[
 * @author	ohno_katsumi@gamefreak.co.jp
 * @date	08/12/22
 */
//=============================================================================

#include <nitro.h>
#include "gflib.h"
#include "net/network_define.h"

#define _WIFILOBBY (0)

#include "net/dwc_rap.h"
#include "dwc_rapinitialize.h"
#include "net/dwc_rapcommon.h"
#include "dwc_rapfriend.h"


extern GFLNetDevTable *NET_GetWifiDeviceTable(void);

static void _DevBootFunc(HEAPID heapID, NetErrorFunc errorFunc);
static BOOL _DevInitFunc(HEAPID heapID, GFL_NETSYS* pNet,NetDevEndCallback callback, void* pUserWork);
static int _DevStartFunc(NetDevEndCallback callback);
static int _DevMainFunc(u16 bitmap);
static BOOL _DevEndFunc(BOOL bForce,NetDevEndCallback callback);
static BOOL _DevExitFunc(NetDevEndCallback callback);
static BOOL _DevChangeOverModeConnectFunc(u8* keyStr,int numEntry, BOOL bParent, u32 timelimit,NetDevEndCallback callback);
static BOOL _DevWifiConnectFunc(int index, int maxnum, BOOL bVCT);
static BOOL _DevModeDisconnectFunc(BOOL bForce, NetDevEndCallback callback);
static BOOL _DevSendDataFunc(void* data,int size, int no,NetDevEndCallback callback);
static BOOL _DevSetRecvCallbackFunc(PTRCommRecvLocalFunc recvCallback);
static BOOL _DevIsStartFunc(void);
static BOOL _DevIsConnectFunc(void);
static BOOL _DevIsEndFunc(void);
static BOOL _DevIsIdleFunc(void); ///<�A�C�h����Ԃ��ǂ���
static u32 _DevGetBitmapFunc(void);
static u32 _DevGetCurrentIDFunc(void);
static int _DevGetIconLevelFunc(void);
static int _DevGetError(void);  ///< �G���[�𓾂�
static void _DevSetNoChildErrorSet(BOOL bOn);
static BOOL _DevIsConnectable(int index);
static BOOL _DevIsVChat(void);
static BOOL _DevIsNewPlayer(void);
static void _DevSetClinetConnect(BOOL bEnable);
static BOOL _DevLobbyLogin(const void* cp_loginprofile);
static void _DevDebugSetRoom( u32 locktime, u32 random, u8 roomtype, u8 season );
static BOOL _DevLobbyUpdateErrorCheck(void);
static BOOL _DevLobbyLoginWait(void);
static void _DevLobbyLogout(void);
static BOOL _DevLobbyLogoutWait(void);
static BOOL _DevLobbyMgCheckRecruit( int type );
static BOOL _DevLobbyMgStartRecruit( int type, u32 maxnum );
static BOOL _DevLobbyMgEntry( int type );
static BOOL _DevLobbyMgForceEnd( void );
static void _DevLobbyMgEndConnect( void );
static BOOL _DevLobbyMgMyParent( void );
static void _DevLobbyMgEndRecruit( void );


//--------------------------------------------
// �\����
//--------------------------------------------

static GFLNetDevTable netDevTbl={
  _DevBootFunc,
  _DevInitFunc,
  _DevStartFunc,
  _DevMainFunc,
  _DevEndFunc,
  _DevExitFunc,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  _DevChangeOverModeConnectFunc,
  _DevWifiConnectFunc,
  _DevModeDisconnectFunc,
  NULL,
  NULL,
  NULL,
  _DevSendDataFunc,
  _DevSetRecvCallbackFunc,
  _DevIsStartFunc,
  _DevIsConnectFunc,
  _DevIsEndFunc,
  _DevIsIdleFunc,
  _DevGetBitmapFunc,
  _DevGetCurrentIDFunc,
  _DevGetIconLevelFunc,
  _DevGetError,
  _DevSetNoChildErrorSet,
  _DevIsConnectable,
  _DevIsVChat,
  _DevIsNewPlayer,
  NULL, //_DevIrcMoveFunc

  NULL,	//DevIsConnectSystemFunc
  NULL,	//DevGetSendLockFlagFunc
  NULL,	//DevConnectWorkInitFunc
  NULL,	//DevGetSendLockFlagFunc
  NULL,	//DevConnectWorkInitFunc
	_DevSetClinetConnect, //DevSetClientConnectFunc DevSetClientConnect; ///< �q�@���Ȃ����Ă悢���ǂ����n�[�h���x���Œ���
  NULL, //DevCrossScanFunc
  NULL, //DevCrossScanChangeSpeedFunc

#if 0
  //-- �ȉ��AWi-Fi�L���p
  _DevLobbyLogin,	//DevLobbyLoginFunc
  _DevDebugSetRoom,	//DevDebugSetRoomFunc
  _DevLobbyUpdateErrorCheck,	//DevLobbyUpdateErrorCheckFunc
  _DevLobbyLoginWait,	//DevLobbyLoginWaitFunc
  _DevLobbyLogout,	//DevLobbyLogoutFunc
  _DevLobbyLogoutWait,	//DevLobbyLogoutWaitFunc
  _DevLobbyMgCheckRecruit,    //DevLobbyMgCheckRecruitFunc
  _DevLobbyMgStartRecruit,    //DevLobbyMgStartRecruitFunc
  _DevLobbyMgEntry,   //DevLobbyMgEntryFunc
  _DevLobbyMgForceEnd,    //DevLobbyMgForceEndFunc
  _DevLobbyMgEndConnect,    //DevLobbyMgEndConnectFunc
  _DevLobbyMgMyParent,    //DevLobbyMgMyParentFunc
  _DevLobbyMgEndRecruit,    //DevLobbyMgEndRecruitFunc
#endif
};

//--------------------------------------------
// �������[�N
//--------------------------------------------

typedef struct{
  GFL_NETSYS* pNet;
  NetDevEndCallback callback;
} NetWork;


static NetWork* _pWork = NULL;


//------------------------------------------------------------------------------
/**
 * @brief   DS�N�����ɍs������
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _DevBootFunc(HEAPID heapID, NetErrorFunc errorFunc)
{
//DWC5.3�ł͎g��Ȃ�
//  DWC_SetAuthServer(GF_DWC_CONNECTINET_AUTH_TYPE);
  GFL_NET_WifiStart( heapID, errorFunc );

  //@todo �u���������K�v
  GF_ASSERT(DWC_ERROR_FRIENDS_SHORTAGE == __ERROR_FRIENDS_SHORTAGE_DWC);

}

//------------------------------------------------------------------------------
/**
 * @brief   �f�o�C�X������
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevInitFunc(HEAPID heapID, GFL_NETSYS* pNet,NetDevEndCallback callback, void* pUserWork)
{
  if(_pWork){
    return FALSE;  //���łɏ������ς�
  }
  {
    GFLNetInitializeStruct* pNetInit = GFL_NET_GetNETInitStruct();
    DWC_RAPCOMMON_SetHeapID(pNetInit->wifiHeapID, pNetInit->heapSize);
  }
  _pWork = GFL_HEAP_AllocClearMemory(heapID, sizeof(NetWork));
  _pWork->pNet = pNet;
  return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ�  WIFI�ڑ��܂ł��
 * @retval  none
 */
//------------------------------------------------------------------------------
static int _DevStartFunc(NetDevEndCallback callback)
{
  int ret = GFL_NET_DWC_startConnect( GFI_NET_GetMyDWCUserData() ,GFI_NET_GetMyDWCFriendData());
  if(callback)
    callback(TRUE);
  //    mydwc_setFetalErrorCallback(CommFatalErrorFunc);   //@@OO �G���[�����ǉ��K�v 07/02/22
  return ret;
}

//------------------------------------------------------------------------------
/**
 * @brief   ���C�����[�v
 * @retval  none
 */
//------------------------------------------------------------------------------

static int _DevMainFunc(u16 bitmap)
{
  return GFL_NET_DWC_stepmatch(bitmap);
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM����U�I������
 * @param   bForce �����I���������ꍇTRUE
 * @retval  TRUE ���� FALSE ���s���Ăё�����K�v������
 */
//------------------------------------------------------------------------------
static BOOL _DevEndFunc(BOOL bForce, NetDevEndCallback callback)
{

  if(GFL_NET_DWC_disconnect( !bForce ) )
  {
    if(GFL_NET_DWC_returnLobby())
    {
      return TRUE;
    }
  }
  else
  {
    GFL_NET_DWC_stepmatch(bForce);
  }
  return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �������J���I�[�o�[���C�J��
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevExitFunc(NetDevEndCallback callback)
{
  if(_pWork){
    GFL_NET_DWC_Logout();  // �ؒf
    if(callback){
      callback(TRUE);
    }
    GFL_HEAP_FreeMemory(_pWork);
    _pWork = NULL;
  }
  DWC_RAPCOMMON_ResetHeapID();
  return TRUE;
}


//------------------------------------------------------------------------------
/**
 * @brief   �����_���ڑ��J�n
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevChangeOverModeConnectFunc(u8* keyStr,int numEntry, BOOL bParent, u32 timelimit, NetDevEndCallback callback)
{
  return GFL_NET_DWC_StartMatch( keyStr, numEntry,  bParent, timelimit );
}

//------------------------------------------------------------------------------
/**
 * @brief   WIFI�F�l�w��Őڑ��J�n
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevWifiConnectFunc(int index, int maxnum, BOOL bVCT)
{
  return GFL_NET_DWC_StartGame(index ,maxnum, bVCT );
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ����I���
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevModeDisconnectFunc(BOOL bForce, NetDevEndCallback callback)
{
  if(GFL_NET_DWC_disconnect( !bForce )){
    GFL_NET_DWC_returnLobby();
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �f�[�^�𑗂�
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevSendDataFunc(void* data, int size, int no, NetDevEndCallback callback)
{
  if(GFL_NET_DWC_SendToOther(data, size)){
    if(callback){
      callback(TRUE);
    }
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   ��M�R�[���o�b�N�̐ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevSetRecvCallbackFunc(PTRCommRecvLocalFunc recvCallback)
{
  GFL_NET_DWC_setReceiver(recvCallback,recvCallback);
  return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�ڑ����Ă邩�ǂ���
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevIsStartFunc(void)
{
  return GFL_NET_DWC_IsInit();
}

//------------------------------------------------------------------------------
/**
 * @brief   ����Ɛڑ����Ă邩�ǂ���
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevIsConnectFunc(void)
{
  return (MYDWC_NONE_AID != GFL_NET_DWC_GetAid());
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�I�����Ă邩�ǂ���
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevIsEndFunc(void)
{
  return !GFL_NET_DWC_IsInit();
}


//------------------------------------------------------------------------------
/**
 * @brief   �������Ă��Ȃ��@���̏�Ԃɂ������Ԃ��ǂ����H
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _DevIsIdleFunc(void)
{
  return GFL_NET_DWC_IsLogin();
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ���Ԃ�BITMAP�ŕԂ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static u32 _DevGetBitmapFunc(void)
{
  return DWC_GetAIDBitmap();
}



//------------------------------------------------------------------------------
/**
 * @brief   ������NetID��Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static u32 _DevGetCurrentIDFunc(void)
{
  return GFL_NET_DWC_GetAid();
}

//------------------------------------------------------------------------------
/**
 * @brief   �A�C�R�����x����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static int _DevGetIconLevelFunc(void)
{
  return (WM_LINK_LEVEL_3 - DWC_GetLinkLevel());
}

//------------------------------------------------------------------------------
/**
 * @brief   �G���[�ԍ���Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
static int _DevGetError(void)  ///< �G���[�𓾂�
{
  int errorCode;
  DWCErrorType myErrorType;
  int ret;

  ret = DWC_GetLastErrorEx( &errorCode, &myErrorType );

  if(ret == DWC_ERROR_NONE){
    return 0;
  }
  else if(errorCode==0){  //WIFI�ł͂O�Ԃ��A�邱�Ƃ����邽�ߕϊ����ĕԂ�
    return -10000;
  }
  return errorCode;
}

//------------------------------------------------------------------------------
/**
 * @brief   �q�@�����Ȃ��ꍇ�ɃG���[�ɂ��邩�ǂ�����ݒ�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _DevSetNoChildErrorSet(BOOL bOn)
{
  //    GFL_NET_WLSetDisconnectOtherError(bOn);
}


//------------------------------------------------------------------------------
/**
 * @brief   ���肪�Ȃ��ł悢��Ԃ��ǂ��� (�ق�friendWifi�p)
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevIsConnectable(int index)
{
  if( GFL_NET_DWC_getFriendStatus(index) != DWC_STATUS_MATCH_SC_SV ){
    return FALSE;
  }
  //    mydwc_setReceiver( CommRecvParentCallback, CommRecvCallback );
  return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �{�C�X�`���b�g��Ԃ��ǂ��� (�ق�friendWifi�p)
 * @retval  none
 */
//------------------------------------------------------------------------------

static BOOL _DevIsVChat(void)
{
  return GFL_NET_DWC_IsVChat();
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ����Ă������ǂ�����Ԃ��܂�
 * @retval  TRUE�c�ڑ��J�n�Ȃ̂ŃL�[������u���b�N   FALSE�c
 */
//------------------------------------------------------------------------------
static BOOL _DevIsNewPlayer(void)
{
  return GFL_NET_DWC_IsNewPlayer();
}

//------------------------------------------------------------------------------
/**
 * @brief   �q�@���Ȃ����Ă悢���ǂ����n�[�h���x���Œ�������
 * @param   bEnable TRUE=����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _DevSetClinetConnect(BOOL bEnable)
{
	GFL_NET_DWC_SetClinetConnect(bEnable);
}

//--------------------------------------------------------------
/**
 * @brief   Wi-Fi�L��Ƀ��O�C��	DWC_LOBBY_Login
 *
 * @param   cp_loginprofile
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	���s		���s�����ꍇ�G���[�^�C�v���擾����Logout���Ă�������
 */
//--------------------------------------------------------------
static BOOL _DevLobbyLogin(const void* cp_loginprofile)
{
#if _WIFILOBBY
  return DWC_LOBBY_Login(cp_loginprofile);
#else
  return FALSE;
#endif
}

//--------------------------------------------------------------
/**
 * @brief   �f�o�b�O�p �����f�[�^�ݒ�DWC_LOBBY_DEBUG_SetRoomData
 *
 * @param   locktime
 * @param   random
 * @param   roomtype
 * @param   season
 */
//--------------------------------------------------------------
static void _DevDebugSetRoom( u32 locktime, u32 random, u8 roomtype, u8 season )
{
#if _WIFILOBBY
  DWC_LOBBY_DEBUG_SetRoomData(locktime, random, roomtype, season);
#endif
}

//--------------------------------------------------------------
/**
 * @brief   DWC���r�[���X�V���A�߂�l�ŃG���[������TRUEorFALSE�ŕԂ� DWC_LOBBY_UpdateErrorCheck
 *
 * @retval  TRUE�F����@FALSE�F�G���[
 */
//--------------------------------------------------------------
static BOOL _DevLobbyUpdateErrorCheck(void)
{
#if _WIFILOBBY
  return DWC_LOBBY_UpdateErrorCheck();
#else
  return FALSE;
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ���O�C�������҂�	DWC_LOBBY_LoginWait
 *
 *	@retval	TRUE	���O�C������
 *	@retval	FALSE	���O�C����or���O�C���ȊO�̏��
 */
//--------------------------------------------------------------
static BOOL _DevLobbyLoginWait(void)
{
#if _WIFILOBBY
  return DWC_LOBBY_LoginWait();
#else
  return FALSE;
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ���r�[�T�[�o���烍�O�A�E�g		DWC_LOBBY_Logout
 */
//--------------------------------------------------------------
static void _DevLobbyLogout(void)
{
#if _WIFILOBBY
  DWC_LOBBY_Logout();
#endif
}

//--------------------------------------------------------------
/**
 * @brief   ���O�A�E�g�����҂�		DWC_LOBBY_LogoutWait
 *
 *	@retval		TRUE	����
 *	@retval		FALSE	���O�A�E�g��or���O�A�E�g��ԈȊO�̏��
 */
//--------------------------------------------------------------
static BOOL _DevLobbyLogoutWait(void)
{
#if _WIFILOBBY
  return DWC_LOBBY_LogoutWait();
#else
  return FALSE;
#endif
}

static BOOL _DevLobbyMgCheckRecruit( int type )
{
#if _WIFILOBBY
  return DWC_LOBBY_MG_CheckRecruit(type);
#else
  return FALSE;
#endif
}

static BOOL _DevLobbyMgStartRecruit( int type, u32 maxnum )
{
#if _WIFILOBBY
  return DWC_LOBBY_MG_StartRecruit(type, maxnum);
#else
  return FALSE;
#endif
}

static BOOL _DevLobbyMgEntry( int type )
{
#if _WIFILOBBY
  return DWC_LOBBY_MG_Entry(type);
#else
  return FALSE;
#endif
}

static BOOL _DevLobbyMgForceEnd( void )
{
#if _WIFILOBBY
  return DWC_LOBBY_MG_ForceEnd();
#else
  return FALSE;
#endif
}

static void _DevLobbyMgEndConnect( void )
{
#if _WIFILOBBY
  DWC_LOBBY_MG_EndConnect();
#else
#endif
}

static BOOL _DevLobbyMgMyParent( void )
{
#if _WIFILOBBY
  return DWC_LOBBY_MG_MyParent();
#else
  return FALSE;
#endif
}

static void _DevLobbyMgEndRecruit( void )
{
#if _WIFILOBBY
  DWC_LOBBY_MG_EndRecruit();
#else
#endif
}


//==============================================================================
//
//==============================================================================
//------------------------------------------------------------------------------
/**
 * @brief   ���C�����X�f�o�C�X�e�[�u����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------
GFLNetDevTable *NET_GetWifiDeviceTable(void)
{
  return &netDevTbl;
}
