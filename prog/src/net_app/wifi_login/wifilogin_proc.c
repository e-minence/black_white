//============================================================================================
/**
 * @file  gtsnego_proc.c
 * @bfief GTS�l�S�V�G�[�V����ProcState
 * @author  k.ohno
 * @date  2009.11.14
 */
//============================================================================================

#include <gflib.h>
#include <dwc.h>

#include "arc_def.h"
#include "net_app/wifi_login.h"
#include "net/network_define.h"
#include "net/dwc_rap.h"

#include "infowin/infowin.h"
#include "system/main.h"
#include "system/wipe.h"
#include "gamesystem/msgspeed.h" // MSGSPEED_GetWait

#include "message.naix"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"

#include "system/bmp_menuwork.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"

#include "savedata/wifilist.h"

//#include "msg/msg_gtsnego.h"
#include "msg/msg_wifi_system.h"
#include "../../field/event_gtsnego.h"
#include "wifi_login.naix"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK
#include "wifilogin_local.h"

#if PM_DEBUG
#define _NET_DEBUG (1)  //�f�o�b�O���͂P
#else
#define _NET_DEBUG (0)  //�f�o�b�O���͂P
#endif

#define _WORK_HEAPSIZE (0x1000)  // �������K�v


//--------------------------------------------
// ��ʍ\����`
//--------------------------------------------

//BG�ʂƃp���b�g�ԍ�(���ݒ�
#define _SUBSCREEN_BGPLANE	(GFL_BG_FRAME3_S)
#define _SUBSCREEN_PALLET	(0xE)

#define STOP_TIME_ (60)

//--------------------------------------------
// �ʐM�������\����
//--------------------------------------------

static void _connectCallBack(void* pWk, int netID);
static void _RecvFirstData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void* _BeaconGetFunc(void* pWork);
static int _BeaconGetSizeFunc(void* pWork);
static BOOL _BeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo);
static void* _getMyUserData(void* pWork);  //DWCUserData
static void* _getFriendData(void* pWork);  //DWCFriendData
static void _deleteFriendList(int deletedIndex,int srcIndex, void* pWork);


///�ʐM�R�}���h�e�[�u��
static const NetRecvFuncTable _PacketTbl[] = {
  {_RecvFirstData,         NULL},    ///NET_CMD_FIRST
};

#define _MAXNUM   (2)         // �ő�ڑ��l��
#define _MAXSIZE  (100)        // �ő呗�M�o�C�g��
#define _BCON_GET_NUM (16)    // �ő�r�[�R�����W��

static GFLNetInitializeStruct aGFLNetInit = {
  _PacketTbl,  // ��M�֐��e�[�u��
  NELEMS(_PacketTbl), // ��M�e�[�u���v�f��
  NULL,    ///< �n�[�h�Őڑ��������ɌĂ΂��
  NULL,//_connectCallBack,    ///< �l�S�V�G�[�V�����������ɃR�[��
  NULL,   // ���[�U�[���m����������f�[�^�̃|�C���^�擾�֐�
  NULL,   // ���[�U�[���m����������f�[�^�̃T�C�Y�擾�֐�
  _BeaconGetFunc,  // �r�[�R���f�[�^�擾�֐�
  _BeaconGetSizeFunc,  // �r�[�R���f�[�^�T�C�Y�擾�֐�
  _BeaconCompFunc,  // �r�[�R���̃T�[�r�X���r���Čq���ŗǂ����ǂ������f����
  NULL,            // ���ʂ̃G���[���N�������ꍇ �ʐM�I��
  FatalError_Disp,  // �ʐM�s�\�ȃG���[���N�������ꍇ�Ă΂�� �ؒf���邵���Ȃ�
  NULL, //_endCallBack,  // �ʐM�ؒf���ɌĂ΂��֐�
  NULL,  // �I�[�g�ڑ��Őe�ɂȂ����ꍇ
  NULL,     ///< wifi�ڑ����Ɏ����̃f�[�^���Z�[�u����K�v������ꍇ�ɌĂ΂��֐�
  NULL, ///< wifi�ڑ����Ƀt�����h�R�[�h�̓���ւ����s���K�v������ꍇ�Ă΂��֐�
  _deleteFriendList,  ///< wifi�t�����h���X�g�폜�R�[���o�b�N
  _getFriendData,   ///< DWC�`���̗F�B���X�g
  _getMyUserData,  ///< DWC�̃��[�U�f�[�^�i�����̃f�[�^�j
  GFL_NET_DWC_HEAPSIZE,   ///< DWC�ւ�HEAP�T�C�Y
  TRUE,        ///< �f�o�b�N�p�T�[�o�ɂȂ����ǂ���
  SYASHI_NETWORK_GGID,  
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_IRC,   //��check�@�ԊO���ʐM�p��create�����HEAPID
  GFL_WICON_POSX, GFL_WICON_POSY,        // �ʐM�A�C�R��XY�ʒu
  _MAXNUM,     // �ő�ڑ��l��
  _MAXSIZE,  //�ő呗�M�o�C�g��
  _BCON_GET_NUM,    // �ő�r�[�R�����W��
  TRUE,     // CRC�v�Z
  FALSE,     // MP�ʐM���e�q�^�ʐM���[�h���ǂ���
  GFL_NET_TYPE_WIFI,  //wifi�ʐM���s�����ǂ���
  TRUE,     // �e���ēx�����������ꍇ�A�Ȃ���Ȃ��悤�ɂ���ꍇTRUE
  WB_NET_COMPATI_CHECK,  //GameServiceID
  0xfffe,	// �ԊO���^�C���A�E�g����
  0,//MP�ʐM�e�@���M�o�C�g��
  0,//dummy
};



//--------------------------------------------
// �������[�N
//--------------------------------------------


enum _IBMODE_SELECT {
  _SELECTMODE_GSYNC = 0,
  _SELECTMODE_UTIL,
  _SELECTMODE_EXIT
};




typedef void (StateFunc)(WIFILOGIN_WORK* pState);
typedef BOOL (TouchFunc)(int no, WIFILOGIN_WORK* pState);


struct _WIFILOGIN_WORK {
  StateFunc* state;      ///< �n���h���̃v���O�������
  TouchFunc* touch;
  int selectType;   // �ڑ��^�C�v
  HEAPID heapID;
  int connect_bit;
  int timer;
  BOOL connect_ok;
  BOOL receive_ok;
  BOOL bInitMessage;
  BOOL bSaving;
  SAVE_CONTROL_WORK* pSave;
  APP_TASKMENU_WORK* pAppTask;
  WIFILOGIN_DISP_WORK* pDispWork;  // �`��n
  WIFILOGIN_MESSAGE_WORK* pMessageWork; //���b�Z�[�W�n
  WIFI_LIST* pList;
  EVENT_GTSNEGO_WORK * dbw;  //�e�̃��[�N
  GAMESYS_WORK *gameSys_;
  FIELDMAP_WORK *fieldWork_;
  GMEVENT* event_;
};



//-----------------------------------------------
//static ��`
//-----------------------------------------------
static void _changeState(WIFILOGIN_WORK* pWork,StateFunc* state);
static void _changeStateDebug(WIFILOGIN_WORK* pWork,StateFunc* state, int line);

static void _modeSelectMenuInit(WIFILOGIN_WORK* pWork);
static void _modeSelectMenuWait(WIFILOGIN_WORK* pWork);
static void _profileIDCheck(WIFILOGIN_WORK* pWork);

static void _modeReportInit(WIFILOGIN_WORK* pWork);
static void _modeReportWait(WIFILOGIN_WORK* pWork);
static void _modeReportWait2(WIFILOGIN_WORK* pWork);
static BOOL _modeSelectMenuButtonCallback(int bttnid,WIFILOGIN_WORK* pWork);
static void _modeSelectBattleTypeInit(WIFILOGIN_WORK* pWork);




#ifdef _NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeStateDebug(pWork ,state, __LINE__)
#else  //_NET_DEBUG
#define   _CHANGE_STATE(pWork, state)  _changeState(pWork ,state)
#endif //_NET_DEBUG




//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @param   state  �ς���X�e�[�g�̊֐�
 * @param   time   �X�e�[�g�ێ�����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _changeState(WIFILOGIN_WORK* pWork,StateFunc state)
{
  pWork->state = state;
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�Ǘ��X�e�[�g�̕ύX
 * @retval  none
 */
//------------------------------------------------------------------------------
#ifdef GFL_NET_DEBUG
static void _changeStateDebug(WIFILOGIN_WORK* pWork,StateFunc state, int line)
{
  NET_PRINT("neg: %d\n",line);
  _changeState(pWork, state);
}
#endif


//--------------------------------------------------------------
/**
 * @brief   �ڑ������R�[���o�b�N
 * @param   pCtl    �f�o�b�O���[�N
 * @retval  none
 */
//--------------------------------------------------------------
#if 0
static void _connectCallBack(void* pWk, int netID)
{
  WIFILOGIN_WORK* pWork = pWk;
  u32 temp;

  OS_TPrintf("�l�S�V�G�[�V�������� netID = %d\n", netID);
  pWork->connect_bit |= 1 << netID;
  temp = pWork->connect_bit;
  if(MATH_CountPopulation(temp) >= 2){
    OS_TPrintf("�S���̃l�S�V�G�[�V�������� �l��bit=%x\n", pWork->connect_bit);
    pWork->connect_ok = TRUE;
  }
}
#endif
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
static void _RecvFirstData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  WIFILOGIN_WORK *pWork = pWk;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if(netID == GFL_NET_SystemGetCurrentID()){
    return;	//�����̃f�[�^�͖���
  }
  pWork->receive_ok = TRUE;
  OS_TPrintf("�ŏ��̃f�[�^��M�R�[���o�b�N netID = %d\n", netID);
}



typedef struct{
  int gameNo;   ///< �Q�[�����
} _testBeaconStruct;

static _testBeaconStruct _testBeacon = { WB_NET_COMPATI_CHECK };


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

static void* _BeaconGetFunc(void* pWork)
{
  return &_testBeacon;
}

///< �r�[�R���f�[�^�T�C�Y�擾�֐�
static int _BeaconGetSizeFunc(void* pWork)
{
  return sizeof(_testBeacon);
}

///< �r�[�R���f�[�^�擾�֐�
static BOOL _BeaconCompFunc(GameServiceID myNo,GameServiceID beaconNo)
{
  if(myNo != beaconNo){
    return FALSE;
  }
  return TRUE;
}


static void* _getMyUserData(void* pWork)  //DWCUserData
{
  WIFILOGIN_WORK *wk = pWork;
  return WifiList_GetMyUserInfo(SaveData_GetWifiListData(wk->pSave));
}

static void* _getFriendData(void* pWork)  //DWCFriendData
{
  WIFILOGIN_WORK *wk = pWork;
  return WifiList_GetDwcDataPtr(SaveData_GetWifiListData(wk->pSave),0);
}

static void _deleteFriendList(int deletedIndex,int srcIndex, void* pWork)
{
  WIFILOGIN_WORK *wk = pWork;
  WifiList_DataMarge(SaveData_GetWifiListData(wk->pSave), deletedIndex, srcIndex);
}


//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���C�x���g�R�[���o�b�N
 *
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------
static void _BttnCallBack( u32 bttnid, u32 event, void* p_work )
{
  WIFILOGIN_WORK *pWork = p_work;
  u32 friendNo;

  switch( event ){
  case GFL_BMN_EVENT_TOUCH:		///< �G�ꂽ�u��
    if(pWork->touch!=NULL){
      if(pWork->touch(bttnid, pWork)){
        return;
      }
    }
    break;
  default:
    break;
  }
}


static void _exitEnd( WIFILOGIN_WORK* pWork)
{
  if( !WIFILOGIN_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork) ){
    return;
  }
  pWork->timer--;
  if(pWork->timer==0){
    _CHANGE_STATE(pWork, NULL);
  }
}

//------------------------------------------------------------------
/**
 * $brief   �I���m�F���b�Z�[�W  WIFIP2PMATCH_MODE_EXIT_WAIT
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static void _exitExiting( WIFILOGIN_WORK *pWork )
{
  if( !WIFILOGIN_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)  ){
    return;
  }
  if(pWork->timer == 1){
    pWork->timer = 0;
    GFL_NET_Exit(NULL);
  }
  if(!GFL_NET_IsInit()){
    WifiList_FormUpData(pWork->pList);  // �ؒf�������Ƀt�����h�R�[�h���l�߂�
    WIFILOGIN_MESSAGE_InfoMessageDisp(pWork->pMessageWork, dwc_message_0012);
    _CHANGE_STATE(pWork, _exitEnd);
    pWork->timer = STOP_TIME_;
  }
}


//------------------------------------------------------------------
/**
 * $brief   �{�^���������ƏI��  WIFIP2PMATCH_MODE_CHECK_AND_END
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static void _CheckAndEnd( WIFILOGIN_WORK *pWork )
{
  if(pWork->timer > 0){
    pWork->timer--;
    return;
  }
  if( GFL_UI_KEY_GetTrg() ){
    WIFILOGIN_MESSAGE_InfoMessageDisp(pWork->pMessageWork, dwc_message_0011);
    _CHANGE_STATE(pWork, _exitExiting);

  }
}

//------------------------------------------------------------------
/**
 * $brief   �G���[�\��
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static void _retryInit(WIFILOGIN_WORK* pWork)
{
}


//------------------------------------------------------------------
/**
 * $brief   �G���[�\��
 * @param   wk
 * @param   seq
 * @retval  int
 */
//------------------------------------------------------------------

static void _errorDisp(WIFILOGIN_WORK* pWork)
{
  GFL_NETSTATE_DWCERROR* pErr = GFL_NET_StateGetWifiError();
  int msgno,no = pErr->errorCode;
  int type =  GFL_NET_DWC_ErrorType(pErr->errorCode, pErr->errorType);

  if(type == 11){
    msgno = dwc_error_0015;
    type = 11;
  }
  else if(no == ERRORCODE_HEAP){
    msgno = dwc_error_0014;
    type = 12;
  }
  else{
    if( type >= 0 ){
      msgno = dwc_error_0001 + type;
    }else{
      msgno = dwc_error_0012;
    }
  }
  NET_PRINT("�G���[���b�Z�[�W %d \n",msgno);
  //EndMessageWindowOff(pWork);

  // �G���[���b�Z�[�W��\�����Ă����V���N��
  pWork->timer = STOP_TIME_;

  WIFILOGIN_MESSAGE_ErrorMessageDisp(pWork->pMessageWork, msgno, no);
//  WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, msgno);
  
  switch(type){
  case 1:
  case 4:
  case 5:
  case 11:
    _CHANGE_STATE(pWork,_retryInit);
    break;
  case 6:
  case 7:
  case 8:
  case 9:
    _CHANGE_STATE(pWork,_retryInit);
    break;
  case 10:
    _CHANGE_STATE(pWork,_retryInit);
    break;
  case 0:
  case 2:
  case 3:
  default:
    _CHANGE_STATE(pWork, _CheckAndEnd);
    break;
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �L�[��������I��
 * @retval  none
 */
//------------------------------------------------------------------------------


static void _saveEndWait(WIFILOGIN_WORK* pWork)
{
  if(GFL_UI_KEY_GetTrg()  || GFL_UI_TP_GetTrg()){
    WIFILOGIN_MESSAGE_SystemMessageEnd(pWork->pMessageWork);
    _CHANGE_STATE(pWork, NULL);  //�ڑ�����
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ��ėp�֐�
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _connectingCommonWait(WIFILOGIN_WORK* pWork)
{
  if(GFL_NET_StateIsWifiLoginState()){
    if( pWork->bInitMessage ){     // ����ڑ����ɂ̓Z�[�u��������������ʒm
      WIFILOGIN_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
      WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0004);
      _CHANGE_STATE(pWork, _saveEndWait);
    }
    else{
      _CHANGE_STATE(pWork, NULL);  //�ڑ�����
    }
  }
  else if(GFL_NET_StateIsWifiError() || (GFL_NET_StateGetWifiStatus() == GFL_NET_STATE_TIMEOUT)){
    _errorDisp(pWork);
  }
}


//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u��
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _saveingWait(WIFILOGIN_WORK* pWork)
{
  if(GFL_NET_DWC_GetSaving()){
    SAVE_RESULT result = GFL_NET_DWC_SaveAsyncMain(pWork->pSave);
    if (result != SAVE_RESULT_CONTINUE && result != SAVE_RESULT_LAST) {
      GFL_NET_DWC_ResetSaving();
    }
    else{
      return;  //�Z�[�u���ɑ��̑J�ڂ��ւ���
    }
  }
  _connectingCommonWait(pWork);
}


//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u�̃��b�Z�[�W���ŏI���܂ő҂�
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _saveingStart(WIFILOGIN_WORK* pWork)
{
  if(!WIFILOGIN_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    _connectingCommonWait(pWork);
  }
  else{
    GFL_NET_DWC_SaveAsyncInit(pWork->pSave);
    pWork->bSaving=TRUE;
    _CHANGE_STATE(pWork, _saveingWait);
    return;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ���
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _connectingWait(WIFILOGIN_WORK* pWork)
{
  if(!WIFILOGIN_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  if( GFL_NET_DWC_GetSaving() && pWork->bSaving==FALSE) {  //�Z�[�u�̕K�v��������
    WIFILOGIN_MESSAGE_InfoMessageDisp(pWork->pMessageWork, dwc_message_0015);
    _CHANGE_STATE(pWork, _saveingStart);
  }
  else{
    _connectingCommonWait(pWork);
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   �ڑ��J�n
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _connectionStart(WIFILOGIN_WORK* pWork)
{
  if( OS_IsRunOnTwl() ){//DSI�Ȃ�}�b�`���O�̃��������傫���Ƃ���ׁA�̈�𑽂����Ȃ��Ƃ����Ȃ�
    aGFLNetInit.heapSize = GFL_NET_DWCLOBBY_HEAPSIZE + 0x40000;
  }
  else{
    aGFLNetInit.heapSize = GFL_NET_DWC_HEAPSIZE + 0x40000;
  }
  GFL_NET_Init(&aGFLNetInit, NULL, pWork);	//�ʐM������
  GFL_NET_StateWifiEnterLogin();

  WIFILOGIN_MESSAGE_InfoMessageDisp(pWork->pMessageWork, dwc_message_0008);
  _CHANGE_STATE(pWork,_connectingWait);
  
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ��m�F�̂͂��������܂�
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeProfileWait2(WIFILOGIN_WORK* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){
      _CHANGE_STATE(pWork,_connectionStart);
    }
    else{
      GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
      _CHANGE_STATE(pWork,NULL);
    }
    WIFILOGIN_MESSAGE_SystemMessageEnd(pWork->pMessageWork);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ , 0 );
  }
}

static void _modeLoginWait2(WIFILOGIN_WORK* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){
      _CHANGE_STATE(pWork,_connectionStart);
    }
    else{
      GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
      _CHANGE_STATE(pWork,NULL);
    }
    WIFILOGIN_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ , 0 );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �ڑ��m�F�̃��b�Z�[�W�o�͑҂�
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeProfileWait(WIFILOGIN_WORK* pWork)
{
  if(!WIFILOGIN_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  pWork->pAppTask = WIFILOGIN_MESSAGE_YesNoStart(pWork->pMessageWork,WIFILOGIN_YESNOTYPE_SYS);
  _CHANGE_STATE(pWork,_modeProfileWait2);
}



static void _modeLoginWait(WIFILOGIN_WORK* pWork)
{
  if(!WIFILOGIN_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  pWork->pAppTask = WIFILOGIN_MESSAGE_YesNoStart(pWork->pMessageWork,WIFILOGIN_YESNOTYPE_INFO);
  _CHANGE_STATE(pWork,_modeLoginWait2);
}



//------------------------------------------------------------------------------
/**
 * @brief   �ŏ��̐ڑ��̃v���t�@�C������
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _profileIDCheck(WIFILOGIN_WORK* pWork)
{
  // ���߂Ă̏ꍇ
  if( !DWC_CheckHasProfile( WifiList_GetMyUserInfo(pWork->pList) ) )
  {
    pWork->bInitMessage=TRUE;
    WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0003);
    _CHANGE_STATE(pWork,_modeProfileWait);
  }
  else if( !DWC_CheckValidConsole(WifiList_GetMyUserInfo(pWork->pList)) )
  {  //��DS�̏ꍇ
    WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0005);
    _CHANGE_STATE(pWork,_modeProfileWait);
  }
  else  //���ʂ̐ڑ�
  {
    WIFILOGIN_MESSAGE_InfoMessageDisp(pWork->pMessageWork, dwc_message_0002);
    _CHANGE_STATE(pWork,_modeLoginWait);
  }

}


static void _modeSelectMenuInit(WIFILOGIN_WORK* pWork)
{ 
  int aMsgBuff[]={dwc_message_0013,dwc_message_0014};

  WIFILOGIN_MESSAGE_ButtonWindowCreate(NELEMS(aMsgBuff), aMsgBuff, pWork->pMessageWork, _BttnCallBack, pWork);

	pWork->touch = &_modeSelectMenuButtonCallback;

	_CHANGE_STATE(pWork,_modeSelectMenuWait);

}

//------------------------------------------------------------------------------
/**
 * @brief   �t�F�[�h�A�E�g����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeout(WIFILOGIN_WORK* pWork)
{
	if(WIPE_SYS_EndCheck()){
		_CHANGE_STATE(pWork, NULL);        // �I���
	}
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _modeSelectMenuButtonCallback(int bttnid,WIFILOGIN_WORK* pWork)
{
  switch( bttnid ){
  case _SELECTMODE_GSYNC:
		PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
    _CHANGE_STATE(pWork,_modeReportInit);
//    pWork->selectType = GAMESYNC_RETURNMODE_SYNC;
    return TRUE;
  case _SELECTMODE_UTIL:
    PMSND_PlaySystemSE(SEQ_SE_DECIDE1);
  //  pWork->selectType = GAMESYNC_RETURNMODE_UTIL;
    _CHANGE_STATE(pWork,_modeReportInit);
    return TRUE;
  case _SELECTMODE_EXIT:
		PMSND_PlaySystemSE(SEQ_SE_CANCEL1);
   // pWork->selectType = GAMESYNC_RETURNMODE_NONE;
		WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
										WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
    _CHANGE_STATE(pWork,_modeFadeout);        // �I���
    return TRUE;
  default:
    break;
  }
  return FALSE;
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuWait(WIFILOGIN_WORK* pWork)
{
	if(WIPE_SYS_EndCheck()){
		WIFILOGIN_MESSAGE_ButtonWindowMain( pWork->pMessageWork );
	}
}


//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u�m�F��ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportInit(WIFILOGIN_WORK* pWork)
{

  //    GAMEDATA_Save(GAMESYSTEM_GetGameData(GMEVENT_GetGameSysWork(event)));

  GFL_BG_ClearScreenCodeVReq(GFL_BG_FRAME1_S,0);
  
 // WIFILOGIN_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GAMESYNC_004);

  
  _CHANGE_STATE(pWork,_modeReportWait);
}


static void _FadeWait(WIFILOGIN_WORK* pWork)
{
  if(GFL_FADE_CheckFade()){
    return;
  }
  _CHANGE_STATE(pWork,NULL);
}



//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u��
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReporting(WIFILOGIN_WORK* pWork)
{

  if(!WIFILOGIN_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }


  {
    SAVE_RESULT svr = SaveControl_SaveAsyncMain(pWork->dbw->ctrl);

    if(svr == SAVE_RESULT_OK){
      WIFILOGIN_MESSAGE_InfoMessageEnd(pWork->pMessageWork);

      GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, 1);

      _CHANGE_STATE(pWork,_FadeWait);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u�m�F��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportWait2(WIFILOGIN_WORK* pWork)
{

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){


//      WIFILOGIN_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GAMESYNC_007);

      SaveControl_SaveAsyncInit(pWork->dbw->ctrl );
      _CHANGE_STATE(pWork,_modeReporting);
    }
    else{
      GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
   //   pWork->selectType = GAMESYNC_RETURNMODE_NONE;
      _CHANGE_STATE(pWork,NULL);
    }
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ , 0 );
  }
}



//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u�m�F��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportWait(WIFILOGIN_WORK* pWork)
{
  if(!WIFILOGIN_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  pWork->pAppTask = WIFILOGIN_MESSAGE_YesNoStart(pWork->pMessageWork,WIFILOGIN_YESNOTYPE_INFO);
  _CHANGE_STATE(pWork,_modeReportWait2);
}


//------------------------------------------------------------------------------
/**
 * @brief   PROC�X�^�[�g
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiLogin_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_GTSNEGO_WORK* pEv=pwk;
	
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x18000 );

  {
    WIFILOGIN_WORK *pWork = GFL_PROC_AllocWork( proc, sizeof( WIFILOGIN_WORK ), HEAPID_IRCBATTLE );
    GFL_STD_MemClear(pWork, sizeof(WIFILOGIN_WORK));
    pWork->heapID = HEAPID_IRCBATTLE;

    pWork->pDispWork = WIFILOGIN_DISP_Init(pWork->heapID);
    pWork->pMessageWork = WIFILOGIN_MESSAGE_Init(pWork->heapID, NARC_message_wifi_system_dat);
    pWork->pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(pEv->gsys));
    pWork->pList = SaveData_GetWifiListData(
      GAMEDATA_GetSaveControlWork(
        GAMESYSTEM_GetGameData(
          ((pEv->gsys))) ));


		{
			GAME_COMM_SYS_PTR pGC = GAMESYSTEM_GetGameCommSysPtr(pEv->gsys);
			INFOWIN_Init( _SUBSCREEN_BGPLANE , _SUBSCREEN_PALLET , pGC , pWork->heapID);
		}



    WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
									WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );

      pWork->dbw = pwk;
    
    if(GFL_NET_IsInit()){       // �ڑ���
      _CHANGE_STATE(pWork,_modeSelectMenuInit);
    }
    else{
      //�ڑ��J�n �v���t�@�C������
      _CHANGE_STATE(pWork,_profileIDCheck);
    }

  }
  
  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCMain
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiLogin_ProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  WIFILOGIN_WORK* pWork = mywk;
  GFL_PROC_RESULT retCode = GFL_PROC_RES_FINISH;

  StateFunc* state = pWork->state;
  if(state != NULL){
    state(pWork);
    retCode = GFL_PROC_RES_CONTINUE;
  }

  if(pWork->pAppTask){
    APP_TASKMENU_UpdateMenu(pWork->pAppTask);
  }

  WIFILOGIN_DISP_Main(pWork->pDispWork);
  WIFILOGIN_MESSAGE_Main(pWork->pMessageWork);

  
	INFOWIN_Update();

  return retCode;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCEnd
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiLogin_ProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_GTSNEGO_WORK* pEv=pwk;
  WIFILOGIN_WORK* pWork = mywk;

//  EVENT_IrcBattleSetType(pParentWork, pWork->selectType);

  GFL_PROC_FreeWork(proc);

	INFOWIN_Exit();
	GFL_BG_FreeBGControl(_SUBSCREEN_BGPLANE);


  WIFILOGIN_MESSAGE_End(pWork->pMessageWork);
  WIFILOGIN_DISP_End(pWork->pDispWork);

	GFL_HEAP_DeleteHeap(HEAPID_IRCBATTLE);
  //EVENT_IrcBattle_SetEnd(pParentWork);


	
  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA WiFiLogin_ProcData = {
  WiFiLogin_ProcInit,
  WiFiLogin_ProcMain,
  WiFiLogin_ProcEnd,
};


