//============================================================================================
/**
 * @file  wifilogin_proc.c
 * @bfief GTS�l�S�V�G�[�V����ProcState
 * @author  k.ohno
 * @date  2009.11.14
 */
//============================================================================================

#include <gflib.h>
#include <dwc.h>

#include "arc_def.h"
#include "net_app/wifi_login.h"
#include "net_app/connect_anm.h"
#include "net/network_define.h"
#include "net/dwc_rap.h"
#include "net/nhttp_rap.h"
#include "net/dwc_error.h"

#include "system/net_err.h"
#include "system/main.h"
#include "system/wipe.h"
#include "system/ds_system.h"
#include "gamesystem/msgspeed.h" // MSGSPEED_GetWait

#include "message.naix"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"
#include "print/str_tool.h"

#include "app/app_printsys_common.h"

#include "system/bmp_menuwork.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menulist.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"

#include "savedata/wifilist.h"
#include "savedata/system_data.h"
#include "savedata/regulation.h"
#include "savedata/battle_box_save.h"

//#include "msg/msg_gtsnego.h"
#include "msg/msg_wifi_system.h"
#include "../../field/event_gtsnego.h"
#include "wifi_login.naix"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK
#include "wifilogin_local.h"

#if DEBUG_ONLY_FOR_ohno
#define _NET_DEBUG (1)
#else
#define _NET_DEBUG (0)
#endif

#define _WORK_HEAPSIZE (0x1000)  // �������K�v


//--------------------------------------------
// ��ʍ\����`
//--------------------------------------------

//BG�ʂƃp���b�g�ԍ�
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
static void _modeFadeStart(WIFILOGIN_WORK* pWork);
static void _callbackFunciton(WIFILOGIN_WORK* pWork);
static void _logoutStart(WIFILOGIN_WORK* pWork);
static void _logoutWait(WIFILOGIN_WORK* pWork);


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
  SYACHI_NETWORK_GGID,  
  GFL_HEAPID_APP,  //���ɂȂ�heapid
  HEAPID_NETWORK,  //�ʐM�p��create�����HEAPID
  HEAPID_WIFI,  //wifi�p��create�����HEAPID
  HEAPID_IRC,   //�ԊO���ʐM�p��create�����HEAPID
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
  NHTTP_RAP_WORK* pNHTTP;
  SAVE_CONTROL_WORK* pSave;
  WIFILOGIN_YESNO_WORK* pSelectWork;  //�I����
  WIFILOGIN_DISP_WORK* pDispWork;  // �`��n
  WIFILOGIN_MESSAGE_WORK* pMessageWork; //���b�Z�[�W�n
  WIFI_LIST* pList;
  WIFILOGIN_PARAM * dbw;  //�e�̃��[�N
  GAMEDATA* gamedata;
};



//-----------------------------------------------
//static ��`
//-----------------------------------------------
static void _changeState(WIFILOGIN_WORK* pWork,StateFunc* state);
static void _changeStateDebug(WIFILOGIN_WORK* pWork,StateFunc* state, int line);
static void _profileIDCheck(WIFILOGIN_WORK* pWork);
static void _modeSvlGetStart(WIFILOGIN_WORK* pWork);
static void _modeSvlGetMain(WIFILOGIN_WORK* pWork);
static void _modeErrorRetry(WIFILOGIN_WORK* pWork);

static void _checkError( WIFILOGIN_WORK* pWork ); //�V�[�P���X���Ŏg�p���ĉ�����


#if PM_DEBUG
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
  NET_PRINT("log: %d\n",line);
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
  return WifiList_GetMyUserInfo(GAMEDATA_GetWiFiList(wk->gamedata));
}

static void* _getFriendData(void* pWork)  //DWCFriendData
{
  WIFILOGIN_WORK *wk = pWork;
  return WifiList_GetDwcDataPtr(GAMEDATA_GetWiFiList(wk->gamedata),0);
}

static void _deleteFriendList(int deletedIndex,int srcIndex, void* pWork)
{
  WIFILOGIN_WORK *wk = pWork;
  WifiList_DataMarge(GAMEDATA_GetWiFiList(wk->gamedata), deletedIndex, srcIndex);
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
    _CHANGE_STATE(pWork, _callbackFunciton);
  }
}

static void _exitEnd2( WIFILOGIN_WORK* pWork)
{
	if(!WIPE_SYS_EndCheck()){
    return;
  }
  if( !WIFILOGIN_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork) ){
    return;
  }
  if(GFL_UI_KEY_GetTrg() & APP_PRINTSYS_COMMON_TRG_KEY){
    _CHANGE_STATE(pWork, _callbackFunciton);
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
  if(GFL_UI_KEY_GetTrg() & APP_PRINTSYS_COMMON_TRG_KEY){
    pWork->dbw->result  = WIFILOGIN_RESULT_CANCEL;
    WIFILOGIN_MESSAGE_InfoMessageDisp(pWork->pMessageWork, dwc_message_0011);
    _CHANGE_STATE(pWork, _exitExiting);

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
  if(GFL_UI_KEY_GetTrg() & APP_PRINTSYS_COMMON_TRG_KEY){
    WIFILOGIN_MESSAGE_SystemMessageEnd(pWork->pMessageWork);
    if( pWork->dbw->bg == WIFILOGIN_BG_NORMAL )
    {
      WIFILOGIN_MESSAGE_TitleDisp( pWork->pMessageWork, dwc_title_0000);
    }
    if(pWork->dbw->pSvl){
      _CHANGE_STATE(pWork, _modeSvlGetStart);  //�F��
    }
    else{
      _CHANGE_STATE(pWork, _callbackFunciton);  //�ڑ�����
    }
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

      WIFILOGIN_MESSAGE_TitleEnd(pWork->pMessageWork);
      WIFILOGIN_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
      WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0004);
      _CHANGE_STATE(pWork, _saveEndWait);
    }
    else{
      if(pWork->dbw->pSvl){
        _CHANGE_STATE(pWork, _modeSvlGetStart);  //�F��
      }
      else{
        _CHANGE_STATE(pWork, _callbackFunciton);  //�ڑ�����
      }
    }
  }
  else{
    _checkError(pWork);
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

    SAVE_RESULT result = GAMEDATA_SaveAsyncMain(pWork->gamedata);
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
//    _connectingCommonWait(pWork);  //�����ŏ�Ԃ��J�ڂ��Ă��܂����� �G���[������Ȃ�    _checkError(pWork);
  }
  else{
    MyStatus_SetProfileID( GAMEDATA_GetMyStatus(pWork->gamedata), WifiList_GetMyGSID(pWork->pList) );
    
    GAMEDATA_SaveAsyncStart(pWork->gamedata);

//    GFL_NET_DWC_SaveAsyncInit(pWork->pSave);
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
  if( GFL_NET_DWC_GetSaving() && pWork->bSaving==FALSE 
      && pWork->dbw->mode != WIFILOGIN_MODE_NOTSAVE
      ) {  //�Z�[�u�̕K�v��������
    WIFILOGIN_MESSAGE_InfoMessageDispWaitIcon(pWork->pMessageWork, dwc_message_0015);
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
    aGFLNetInit.heapSize = GFL_NET_DWCLOBBY_HEAPSIZE;
  }
  else{
    aGFLNetInit.heapSize = GFL_NET_DWC_HEAPSIZE;
  }
  if(pWork->dbw->nsid == WB_NET_BSUBWAY){
    aGFLNetInit.heapSize = GFL_NET_DWC_BSUBWAY_HEAPSIZE;
  }
  else if(pWork->dbw->nsid == WB_NET_WIFIMATCH){
    aGFLNetInit.heapSize = GFL_NET_DWC_RNDMATCH_HEAPSIZE;
  }

  if(pWork->dbw->nsid != WB_NET_WIFICLUB){
    aGFLNetInit.keyList=NULL;
  }
  
  GFL_NET_Init(&aGFLNetInit, NULL, pWork);	//�ʐM������
  GFL_NET_StateWifiEnterLogin();
  GFL_NET_ReloadIcon();

  if( pWork->dbw->display == WIFILOGIN_DISPLAY_DOWN )
  { 
    GFL_NET_WirelessIconEasy_HoldLCD(FALSE, pWork->heapID);
  }
  else
  { 
    GFL_NET_WirelessIconEasy_HoldLCD(TRUE, pWork->heapID);
  }
  GFL_NET_ReloadIcon();

  WIFILOGIN_MESSAGE_InfoMessageDispWaitIcon(pWork->pMessageWork, dwc_message_0008);

  WIFILOGIN_DISP_StartSmoke(pWork->pDispWork);

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
  if(WIFILOGIN_MESSAGE_YesNoIsFinish(pWork->pSelectWork)){
    int selectno = WIFILOGIN_MESSAGE_YesNoGetCursorPos(pWork->pSelectWork);

    if(selectno==0){
      pWork->dbw->result  = WIFILOGIN_RESULT_LOGIN;
      _CHANGE_STATE(pWork,_connectionStart);
    }
    else{
      GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
      pWork->dbw->result  = WIFILOGIN_RESULT_CANCEL;
      _CHANGE_STATE(pWork,_callbackFunciton);
    }
    WIFILOGIN_MESSAGE_SystemMessageEnd(pWork->pMessageWork);
    if( pWork->dbw->bg == WIFILOGIN_BG_NORMAL )
    {
      WIFILOGIN_MESSAGE_TitleDisp(pWork->pMessageWork, dwc_title_0000);
    }
    WIFILOGIN_MESSAGE_YesNoEnd(pWork->pSelectWork);
    pWork->pSelectWork=NULL;
    if(pWork->dbw->bg==WIFILOGIN_BG_NORMAL){
  //    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ , 0 );
    }
  }
}

static void _modeLoginWait2(WIFILOGIN_WORK* pWork)
{
  if(WIFILOGIN_MESSAGE_YesNoIsFinish(pWork->pSelectWork)){
    int selectno = WIFILOGIN_MESSAGE_YesNoGetCursorPos(pWork->pSelectWork);

    if(selectno==0){
      pWork->dbw->result  = WIFILOGIN_RESULT_LOGIN;
      _CHANGE_STATE(pWork,_connectionStart);
    }
    else{
      pWork->dbw->result  = WIFILOGIN_RESULT_CANCEL;
      GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
      _CHANGE_STATE(pWork,_callbackFunciton);
    }
    WIFILOGIN_MESSAGE_SystemMessageEnd(pWork->pMessageWork);
    WIFILOGIN_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
    WIFILOGIN_MESSAGE_YesNoEnd(pWork->pSelectWork);
    pWork->pSelectWork=NULL;
    if(pWork->dbw->bg==WIFILOGIN_BG_NORMAL){
  //    G2S_SetBlendBrightness( GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_OBJ , 0 );

      WIFILOGIN_MESSAGE_TitleDisp(pWork->pMessageWork, dwc_title_0000);
    }
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
	if(!WIPE_SYS_EndCheck()){
    return;
  }
  if(!WIFILOGIN_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  pWork->pSelectWork = WIFILOGIN_MESSAGE_YesNoStart(pWork->pMessageWork,WIFILOGIN_YESNOTYPE_SYS,(pWork->dbw->bg==WIFILOGIN_BG_NORMAL),0);
  _CHANGE_STATE(pWork,_modeProfileWait2);
}

static void _modeErrorRetry(WIFILOGIN_WORK* pWork)
{
	if(!WIPE_SYS_EndCheck()){
    return;
  }
    WIFILOGIN_MESSAGE_TitleEnd(pWork->pMessageWork);
    pWork->pSelectWork = WIFILOGIN_MESSAGE_YesNoStart(pWork->pMessageWork,WIFILOGIN_YESNOTYPE_SYS,(pWork->dbw->bg==WIFILOGIN_BG_NORMAL),0);
    _CHANGE_STATE(pWork,_modeLoginWait2);
}

//------------------------------------------------------------------------------
/**
 * @brief   �G���[���`�F�b�N���A���o�����Ȃ�΃��b�Z�[�W�\����A���g���C�֍s������
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _checkError( WIFILOGIN_WORK* pWork )
{
  if( NetErr_App_CheckError() ){
    const GFL_NETSTATE_DWCERROR* cp_error  =  GFL_NET_StateGetWifiError();

    switch( cp_error->errorType )
    { 
    case DWC_ETYPE_LIGHT:
    case DWC_ETYPE_SHOW_ERROR:
    case DWC_ETYPE_DISCONNECT:
    case DWC_ETYPE_SHUTDOWN_FM:
    case DWC_ETYPE_SHUTDOWN_GHTTP:
    case DWC_ETYPE_SHUTDOWN_ND:
      //�G���[���
      WIFILOGIN_MESSAGE_TitleEnd(pWork->pMessageWork);
      WIFILOGIN_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
      NetErr_DispCallPushPop();
      WIFILOGIN_DISP_ResetErrorDisplay(pWork->pDispWork);

      //�G���[�N���A
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();

      //�����ؒf
      if( GFL_NET_IsInit() )
      {
        NetErr_ExitNetSystem();
      }

      //�Đڑ���
      WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0019);
      _CHANGE_STATE(pWork,_modeErrorRetry);
      break;

    case DWC_ETYPE_FATAL:
      //Fatal
      NetErr_DispCallFatal();
      break;
    }
  }
}

static void _modeLoginWait(WIFILOGIN_WORK* pWork)
{
	if(!WIPE_SYS_EndCheck()){
    return;
  }
  if(!WIFILOGIN_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
 pWork->pSelectWork = WIFILOGIN_MESSAGE_YesNoStart(pWork->pMessageWork,WIFILOGIN_YESNOTYPE_INFO,(pWork->dbw->bg==WIFILOGIN_BG_NORMAL),0);
  _CHANGE_STATE(pWork,_modeLoginWait2);
}

static void _modeDifferDSWait7(WIFILOGIN_WORK* pWork)
{
  if(WIFILOGIN_MESSAGE_YesNoIsFinish(pWork->pSelectWork)){
    int selectno = WIFILOGIN_MESSAGE_YesNoGetCursorPos(pWork->pSelectWork);

    if(selectno==0){

      REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( GAMEDATA_GetSaveControlWork(pWork->gamedata) );
      REGULATION_CARDDATA *p_reg_card = RegulationSaveData_GetRegulationCard( p_reg_sv, REGULATION_CARD_TYPE_WIFI );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( GAMEDATA_GetSaveControlWork(pWork->gamedata) );

      //���^�C�������ɂ��ăo�g���{�b�N�X�̃��b�N����
      BATTLE_BOX_SAVE_OffLockFlg( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI );
      Regulation_SetCardParam( p_reg_card, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_CHANGE_DS );

      pWork->dbw->result  = WIFILOGIN_RESULT_LOGIN;
      WifiList_Init(pWork->pList);
      _CHANGE_STATE(pWork,_connectionStart);
    }
    else{
      pWork->dbw->result  = WIFILOGIN_RESULT_CANCEL;
      GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
      _CHANGE_STATE(pWork,_callbackFunciton);
    }
    WIFILOGIN_MESSAGE_SystemMessageEnd(pWork->pMessageWork);
    if( pWork->dbw->bg == WIFILOGIN_BG_NORMAL )
    {
      WIFILOGIN_MESSAGE_TitleDisp(pWork->pMessageWork, dwc_title_0000);
    }
    WIFILOGIN_MESSAGE_YesNoEnd(pWork->pSelectWork);
    pWork->pSelectWork=NULL;
  }
}

static void _modeDifferDSWait6(WIFILOGIN_WORK* pWork)
{
  pWork->pSelectWork = WIFILOGIN_MESSAGE_YesNoStart(pWork->pMessageWork,WIFILOGIN_YESNOTYPE_SYS,(pWork->dbw->bg==WIFILOGIN_BG_NORMAL),1);
  _CHANGE_STATE(pWork,_modeDifferDSWait7);
}

static void _modeDifferDSWait5(WIFILOGIN_WORK* pWork)
{
  if(WIFILOGIN_MESSAGE_YesNoIsFinish(pWork->pSelectWork)){
    int selectno = WIFILOGIN_MESSAGE_YesNoGetCursorPos(pWork->pSelectWork);

    WIFILOGIN_MESSAGE_SystemMessageEnd(pWork->pMessageWork);
    if( pWork->dbw->bg == WIFILOGIN_BG_NORMAL )
    {
      WIFILOGIN_MESSAGE_TitleDisp(pWork->pMessageWork, dwc_title_0000);
    }
    WIFILOGIN_MESSAGE_YesNoEnd(pWork->pSelectWork);
    pWork->pSelectWork=NULL;

    if(selectno==0){

      REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( GAMEDATA_GetSaveControlWork(pWork->gamedata) );
      REGULATION_CARDDATA *p_reg_card = RegulationSaveData_GetRegulationCard( p_reg_sv, REGULATION_CARD_TYPE_WIFI );
      u32 cupNo   = Regulation_GetCardParam( p_reg_card, REGULATION_CARD_CUPNO );
      u32 status  = Regulation_GetCardParam( p_reg_card, REGULATION_CARD_STATUS );

      if( cupNo != 0 && (status == DREAM_WORLD_MATCHUP_SIGNUP || status ==DREAM_WORLD_MATCHUP_ENTRY ) )
      { 

        //WIFI���ɎQ�����Ă����������x�m�F
        WIFILOGIN_MESSAGE_TitleEnd(pWork->pMessageWork);
        WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0020);
        _CHANGE_STATE(pWork,_modeDifferDSWait6);
      }
      else
      { 
        pWork->dbw->result  = WIFILOGIN_RESULT_LOGIN;
        WifiList_Init(pWork->pList);
        _CHANGE_STATE(pWork,_connectionStart);
      }
    }
    else{
      pWork->dbw->result  = WIFILOGIN_RESULT_CANCEL;
      GFL_BG_ClearScreen(GFL_BG_FRAME3_M);
      _CHANGE_STATE(pWork,_callbackFunciton);
    }
  }
}



static void _modeDifferDSWait4(WIFILOGIN_WORK* pWork)
{

  pWork->pSelectWork = WIFILOGIN_MESSAGE_YesNoStart(pWork->pMessageWork,WIFILOGIN_YESNOTYPE_SYS,(pWork->dbw->bg==WIFILOGIN_BG_NORMAL),1);
  _CHANGE_STATE(pWork,_modeDifferDSWait5);
}

static void _modeDifferDSWait3(WIFILOGIN_WORK* pWork)
{
  if(WIFILOGIN_MESSAGE_YesNoIsFinish(pWork->pSelectWork)){
    int selectno = WIFILOGIN_MESSAGE_YesNoGetCursorPos(pWork->pSelectWork);

    if(selectno==0){
      pWork->dbw->result  = WIFILOGIN_RESULT_LOGIN;

      WIFILOGIN_MESSAGE_TitleEnd(pWork->pMessageWork);
      WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0007);
      _CHANGE_STATE(pWork,_modeDifferDSWait4);
    }
    else{
      pWork->dbw->result  = WIFILOGIN_RESULT_CANCEL;
      WIFILOGIN_MESSAGE_SystemMessageEnd(pWork->pMessageWork);
      if( pWork->dbw->bg == WIFILOGIN_BG_NORMAL )
      {
        WIFILOGIN_MESSAGE_TitleDisp(pWork->pMessageWork, dwc_title_0000);
      }
      _CHANGE_STATE(pWork,_callbackFunciton);
    }
    WIFILOGIN_MESSAGE_YesNoEnd(pWork->pSelectWork);
    pWork->pSelectWork=NULL;
  }
}



static void _modeDifferDSWait2(WIFILOGIN_WORK* pWork)
{


  pWork->pSelectWork = WIFILOGIN_MESSAGE_YesNoStart(pWork->pMessageWork,WIFILOGIN_YESNOTYPE_SYS,(pWork->dbw->bg==WIFILOGIN_BG_NORMAL),1);
  _CHANGE_STATE(pWork,_modeDifferDSWait3);
}


static void _modeDifferDSWait(WIFILOGIN_WORK* pWork)
{
	if(!WIPE_SYS_EndCheck()){
    return;
  }
  if(GFL_UI_KEY_GetTrg() & APP_PRINTSYS_COMMON_TRG_KEY){
    WIFILOGIN_MESSAGE_TitleEnd(pWork->pMessageWork);
    WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0006);
    _CHANGE_STATE(pWork,_modeDifferDSWait2);
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �ŏ��̐ڑ��̃v���t�@�C������
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _profileIDCheck(WIFILOGIN_WORK* pWork)
{
  if(!DS_SYSTEM_IsAvailableWireless() ) //�����ʐM�s��
  { 
    WIFILOGIN_MESSAGE_TitleEnd(pWork->pMessageWork);
    WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0021);
    pWork->dbw->result  = WIFILOGIN_RESULT_CANCEL;
    _CHANGE_STATE(pWork,_exitEnd2);
  }
  else if(!DS_SYSTEM_IsAgreeEULA()){  ///EULA����
    WIFILOGIN_MESSAGE_TitleEnd(pWork->pMessageWork);
    WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0018);
    pWork->dbw->result  = WIFILOGIN_RESULT_CANCEL;
    _CHANGE_STATE(pWork,_exitEnd2);
  }
  //�����Z�[�u����
  else if( pWork->dbw->mode == WIFILOGIN_MODE_NOTSAVE )
  { 
    WIFILOGIN_MESSAGE_InfoMessageDisp(pWork->pMessageWork, dwc_message_0002);
    _CHANGE_STATE(pWork,_modeLoginWait);
  }
  else if( !DWC_CheckHasProfile( WifiList_GetMyUserInfo(pWork->pList) ) )
  {
    // ���߂Ă̏ꍇ
    pWork->bInitMessage=TRUE;
    WIFILOGIN_MESSAGE_TitleEnd(pWork->pMessageWork);
    WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0003);
    _CHANGE_STATE(pWork,_modeProfileWait);
  }
  else if( !DWC_CheckValidConsole(WifiList_GetMyUserInfo(pWork->pList)) )
  {  //��DS�̏ꍇ
    WIFILOGIN_MESSAGE_TitleEnd(pWork->pMessageWork);
    WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0005);
    _CHANGE_STATE(pWork,_modeDifferDSWait);
  }
  else if( pWork->dbw->mode == WIFILOGIN_MODE_ERROR ) //�G���[�ōĂу��O�C����
  { 
    WIFILOGIN_MESSAGE_TitleEnd(pWork->pMessageWork);
    WIFILOGIN_MESSAGE_SystemMessageDisp(pWork->pMessageWork, dwc_message_0019);
    _CHANGE_STATE(pWork,_modeErrorRetry);
  }
  else  //���ʂ̐ڑ�
  {
    WIFILOGIN_MESSAGE_InfoMessageDisp(pWork->pMessageWork, dwc_message_0002);
    _CHANGE_STATE(pWork,_modeLoginWait);
  }

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
 * @brief   �R�[���o�b�N����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _callbackFunciton(WIFILOGIN_WORK* pWork)
{ 
  if( pWork->dbw->login_after_callback && pWork->dbw->result == WIFILOGIN_RESULT_LOGIN )
  { 
    WIFILOGIN_CALLBACK_RESULT result;

    result  = pWork->dbw->login_after_callback( pWork->pMessageWork, pWork->dbw->p_callback_wk );

    if( result != WIFILOGIN_CALLBACK_RESULT_CONTINUE )
    { 
      switch( result )
      { 
      case WIFILOGIN_CALLBACK_RESULT_SUCCESS:
        pWork->dbw->result  = WIFILOGIN_RESULT_LOGIN;
        _CHANGE_STATE(pWork,_modeFadeStart);
        break;
      case WIFILOGIN_CALLBACK_RESULT_FAILED:
        pWork->dbw->result  = WIFILOGIN_RESULT_CANCEL;
        _CHANGE_STATE(pWork,_logoutStart);
        break;
      }
    }

    _checkError(pWork);
  }
  else
  { 
    _CHANGE_STATE(pWork,_modeFadeStart);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���O�A�E�g�J�n
 */
//-----------------------------------------------------------------------------
static void _logoutStart(WIFILOGIN_WORK* pWork)
{ 
  if( GFL_NET_IsExit() )
  {
    _CHANGE_STATE(pWork,_modeFadeStart);
  }
  else
  { 
    //����������Ă�������
    if( GFL_NET_Exit( NULL ) )
    { 
      _CHANGE_STATE(pWork,_logoutWait);
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���O�A�E�g��
 */
//-----------------------------------------------------------------------------
static void _logoutWait(WIFILOGIN_WORK* pWork)
{ 
  if( GFL_NET_IsExit() )
  { 
    _CHANGE_STATE(pWork,_modeFadeStart);
  }
}
//------------------------------------------------------------------------------
/**
 * @brief   �t�F�[�h�A�E�g����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeStart(WIFILOGIN_WORK* pWork)
{
  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
  if( pWork->dbw->bg == WIFILOGIN_BG_NORMAL )
  { 
    if( pWork->dbw->bgm == WIFILOGIN_BGM_NORMAL )
    { 
      PMSND_FadeOutBGM( 8 );
    }
  }
  _CHANGE_STATE(pWork,_modeFadeout);        // �I���
}


static void _FadeWait(WIFILOGIN_WORK* pWork)
{
  if(!GFL_FADE_CheckFade()){
    return;
  }
  _CHANGE_STATE(pWork,NULL);
}


//------------------------------------------------------------------------------
/**
 * @brief   SVL�F��
 * @retval  none
 */
//------------------------------------------------------------------------------


static void _modeSvlStart2(WIFILOGIN_WORK* pWork)
{
  if( DWC_SVLGetTokenAsync("", pWork->dbw->pSvl) ){
    _CHANGE_STATE(pWork, _modeSvlGetMain);
  }
}


static void _modeSvlGetStart(WIFILOGIN_WORK* pWork)
{
//  pWork->pNHTTP = NHTTP_RAP_Init(pWork->heapID,
  //                               MyStatus_GetProfileID(GAMEDATA_GetMyStatus(pWork->gamedata)),
    //                             pWork->dbw->pSvl );
  _CHANGE_STATE(pWork, _modeSvlStart2);
}


static void _modeSvlGetMain(WIFILOGIN_WORK* pWork)
{
	DWCSvlState		state;
	DWCError		dwcerror;
	DWCErrorType	dwcerrortype;
	int				errorcode;
  DWCSvlResult* pSvl = pWork->dbw->pSvl;

  state = DWC_SVLProcess();
  if(state == DWC_SVL_STATE_SUCCESS) {
    NET_PRINT("Succeeded to get SVL Status\n");
    NET_PRINT("status = %s\n", pSvl->status==TRUE ? "TRUE" : "FALSE");
    NET_PRINT("svlhost = %s\n", pSvl->svlhost);
    NET_PRINT("svltoken = %s\n", pSvl->svltoken);
    _CHANGE_STATE(pWork, _callbackFunciton);
  }
  else if(state == DWC_SVL_STATE_ERROR) {
    dwcerror = DWC_GetLastErrorEx(&errorcode, &dwcerrortype);
    _CHANGE_STATE(pWork, _callbackFunciton);
    NET_PRINT("Failed to get SVL Token\n");
    NET_PRINT("DWCError = %d, errorcode = %d, DWCErrorType = %d\n", dwcerror, errorcode, dwcerrortype);
  }
  else if(state == DWC_SVL_STATE_CANCELED) {
    NET_PRINT("SVL canceled.\n");
    _CHANGE_STATE(pWork, _callbackFunciton);
  }
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
    SAVE_RESULT svr = GAMEDATA_SaveAsyncMain(pWork->gamedata);
    
    if(svr == SAVE_RESULT_OK){
      WIFILOGIN_MESSAGE_InfoMessageEnd(pWork->pMessageWork);

      GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, 1);

      _CHANGE_STATE(pWork,_FadeWait);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   PROC�X�^�[�g
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT WiFiLogin_ProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  WIFILOGIN_PARAM* pEv=pwk;
  WIFILOGIN_WORK *pWork;
	
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFILOGIN, 0x18000 );

  {
    pWork = GFL_PROC_AllocWork( proc, sizeof( WIFILOGIN_WORK ), HEAPID_WIFILOGIN );
    GFL_STD_MemClear(pWork, sizeof(WIFILOGIN_WORK));
    pWork->heapID = HEAPID_WIFILOGIN;
    pWork->gamedata = pEv->gamedata;
    pWork->pDispWork = WIFILOGIN_DISP_Init(pWork->heapID,pEv->bg,pEv->display);
    pWork->pMessageWork = WIFILOGIN_MESSAGE_Init(pWork->heapID, NARC_message_wifi_system_dat,pEv->display);
    pWork->pSave = GAMEDATA_GetSaveControlWork(pEv->gamedata);
    pWork->pList = GAMEDATA_GetWiFiList(pEv->gamedata);

//    GFL_NET_ChangeIconPosition

    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
									WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );

    pWork->dbw = pwk;

    if(GFL_NET_IsInit()){       // �ڑ���
      GF_ASSERT(0);

    }
    else{
      //�ڑ��J�n �v���t�@�C������
      _CHANGE_STATE(pWork,_profileIDCheck);
    }

  }

  if( pWork->dbw->bg == WIFILOGIN_BG_NORMAL )
  { 
    WIFILOGIN_MESSAGE_TitleDisp(pWork->pMessageWork, dwc_title_0000);

    if( pWork->dbw->bgm == WIFILOGIN_BGM_NORMAL )
    { 
      PMSND_PlayBGM( SEQ_BGM_WIFI_ACCESS );
    }
  }
  else{
    if( pWork->dbw->bgm == WIFILOGIN_BGM_NORMAL )
    { 
      PMSND_PlayBGM(SEQ_BGM_GAME_SYNC);
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
  WIFILOGIN_PARAM* pEv=pwk;
  GFL_PROC_RESULT retCode = GFL_PROC_RES_FINISH;

  //���C���v���Z�X�̉��ɂ���Ǔ����t���[���Ȃ���TRG���m���Ă��܂��̂ŁA
  //���C���v���Z�X�̏�ɂ����܂���
  WIFILOGIN_DISP_Main(pWork->pDispWork);
  WIFILOGIN_MESSAGE_Main(pWork->pMessageWork);

  //���C���v���Z�X
  { 
    StateFunc* state = pWork->state;
    if(state != NULL){
      state(pWork);
      retCode = GFL_PROC_RES_CONTINUE;
    }
  }

  //
  if(pWork->pSelectWork){
    WIFILOGIN_MESSAGE_YesNoUpdate(pWork->pSelectWork);
  }

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
  WIFILOGIN_PARAM* pEv=pwk;
  WIFILOGIN_WORK* pWork = mywk;

//  EVENT_IrcBattleSetType(pParentWork, pWork->selectType);

	GFL_BG_FreeBGControl(_SUBSCREEN_BGPLANE);


  WIFILOGIN_MESSAGE_End(pWork->pMessageWork);
  WIFILOGIN_DISP_End(pWork->pDispWork);

  GFL_PROC_FreeWork(proc);
	GFL_HEAP_DeleteHeap(HEAPID_WIFILOGIN);
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


