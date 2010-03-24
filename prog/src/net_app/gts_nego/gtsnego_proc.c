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
#include "net_app/gtsnego.h"
#include "net/network_define.h"
#include "net/dwc_rap.h"
#include "net/dwc_tool.h"
#include "../../field/event_gtsnego.h"

#include "infowin/infowin.h"
#include "system/main.h"
#include "system/wipe.h"
#include "gamesystem/msgspeed.h" // MSGSPEED_GetWait
#include "msg/msg_namein.h"   //�f�t�H���g����

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
#include "savedata/wifi_negotiation.h"
#include "savedata/system_data.h"

#include "msg/msg_gtsnego.h"
#include "../../field/event_gtsnego.h"
#include "gtsnego.naix"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK
#include "gtsnego_local.h"


#define _NO2   (2)
#define _NO3   (3)

#define _WORK_HEAPSIZE (0x1000)  // �������K�v


//--------------------------------------------
// ��ʍ\����`
//--------------------------------------------

//BG�ʂƃp���b�g�ԍ�(���ݒ�

#define _SUBSCREEN_PALLET	(0xE)

#define STOP_TIME_ (60)
#define _FRIEND_LOOKAT_DOWN_TIME  (60*3)
#define _FRIEND_GREE_DOWN_TIME  (60*3)

//--------------------------------------------
// �ʐM�������\����
//--------------------------------------------




//--------------------------------------------
// �������[�N
//--------------------------------------------


enum _IBMODE_SELECT {
  _SELECTMODE_GSYNC = 0,
  _SELECTMODE_UTIL,
  _SELECTMODE_EXIT
};

enum _CHANGEMODE_SELECT {
  _CHANGEMODE_SELECT_ANYONE=0,
  _CHANGEMODE_SELECT_FRIEND,
};

enum _CHANGEMODE_LEVEL {
  _CHANGEMODE_LEVEL_ALL,
  _CHANGEMODE_LEVEL_1_49,
  _CHANGEMODE_LEVEL_50,
  _CHANGEMODE_LEVEL_51_100,
  _CHANGEMODE_LEVEL_MAX,
};

enum _CHANGEMODE_IMAGE {
  _CHANGEMODE_IMAGE_ALL,
  _CHANGEMODE_IMAGE_COOL,  //����������
  _CHANGEMODE_IMAGE_PRETTY,       //		���킢��
  _CHANGEMODE_IMAGE_SCARY,   //  		���킢
  _CHANGEMODE_IMAGE_GEEK,   //  		�ւ��
  _CHANGEMODE_IMAGE_MAX,        		
};



enum _GTSNEGO_MATCHKEY {
  _MATCHKEY_PROFILE,   //Profile
  _MATCHKEY_TYPE,     //�����^�C�v �m�荇�����ǂ���
  _MATCHKEY_LEVEL,    //����LV
  _MATCHKEY_IMAGE_MY,  //�����̊�]
  _MATCHKEY_IMAGE_FRIEND,  //����ɑ΂��Ă̊�]
  _MATCHKEY_ROMCODE,    //ROM�o�[�W����
  _MATCHKEY_DEBUG,    //�f�o�b�O�����i��
  _MATCHKEY_MAX,
};



static char matchkeystr[_MATCHKEY_MAX][2]={"p1","ty","lv","my","fr","rm","db"};


typedef void (StateFunc)(GTSNEGO_WORK* pState);
typedef BOOL (TouchFunc)(int no, GTSNEGO_WORK* pState);

// �F�B���w��s�A�}�b�`���C�N�p�ǉ��L�[�f�[�^�\����
typedef struct tagGameMatchExtKeys
{
    int ivalue;       // �L�[�ɑΉ�����l�iint�^�j
    u8  keyID;        // �}�b�`���C�N�p�L�[ID
    u8 pad;          // �p�f�B���O
    char keyStr[3];  // �}�b�`���C�N�p�L�[������
} GameMatchExtKeys;


typedef struct{   //��������f�[�^�̍\����
  PMS_DATA pms;
  int num;
} MATCH_DATA;




struct _GTSNEGO_WORK {
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
  GAMEDATA* pGameData;
  SAVE_CONTROL_WORK* pSave;
  APP_TASKMENU_WIN_WORK *pAppWin;
  APP_TASKMENU_WORK* pAppTask;
  GTSNEGO_DISP_WORK* pDispWork;  // �`��n
  GTSNEGO_MESSAGE_WORK* pMessageWork; //���b�Z�[�W�n
  WIFI_LIST* pList;
  EVENT_GTSNEGO_WORK * dbw;  //�e�̃��[�N
  GAMESYS_WORK *gameSys_;
  FIELDMAP_WORK *fieldWork_;
  GMEVENT* event_;
  GameMatchExtKeys aMatchKey[_MATCHKEY_MAX];
  char filter[128];
  int changeMode;    //�L�[�ɂ���l ����ł����Ƃ�������
  int myChageType;   //�����������������^�C�v
  int friendChageType;  //����Ɍ������Ă��炢�����^�C�v
  int chageLevel;    // �|�P�������x���͈�
  int selectFriendIndex; // �I�񂾂܂����킹�̂Ђ�
  s32 profileID;
  SCROLLPANELCURSOR scrollPanelCursor;
  CROSSCUR_TYPE key1;  //���C��
  CROSSCUR_TYPE key2;  //����ł��p
  CROSSCUR_TYPE key3;  //�܂����킹�悤
  MATCH_DATA myMatchData;
  MATCH_DATA MatchData;
  BOOL keyMode;
  DWC_TOOL_BADWORD_WORK aBadWork;
  STRBUF  *wordCheck;
};


///�ʐM�R�}���h
typedef enum {
  _NETCMD_INFOSEND = GFL_NET_CMD_GTSNEGO,
  _NETCMD_MYSTATUSSEND,
  _NETCMD_MATCH,
} _BATTLEIRC_SENDCMD;


//-----------------------------------------------
//static ��`
//-----------------------------------------------
static void _changeState(GTSNEGO_WORK* pWork,StateFunc* state);
static void _changeStateDebug(GTSNEGO_WORK* pWork,StateFunc* state, int line);

static void _modeSelectMenuInit(GTSNEGO_WORK* pWork);
static void _modeSelectMenuWait(GTSNEGO_WORK* pWork);
static void _profileIDCheck(GTSNEGO_WORK* pWork);

static void _modeReportInit(GTSNEGO_WORK* pWork);
static void _modeReportWait(GTSNEGO_WORK* pWork);
static void _modeReportWait2(GTSNEGO_WORK* pWork);
static BOOL _modeSelectMenuButtonCallback(int bttnid,GTSNEGO_WORK* pWork);
static void _modeSelectBattleTypeInit(GTSNEGO_WORK* pWork);
static void _friendSelectWait( GTSNEGO_WORK *pWork );

static void _levelSelect( GTSNEGO_WORK *pWork );
static void _friendSelect( GTSNEGO_WORK *pWork );
static int _buttonDecide(GTSNEGO_WORK* pWork, int key);
static void _modeSelectMenuFlash(GTSNEGO_WORK* pWork);

static void _recvInfomationData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _recvMystatusData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);
static void _cancelButtonCallback(u32 bttnid, u32 event,void* p_work);

///�ʐM�R�}���h�e�[�u��
static const NetRecvFuncTable _PacketTbl[] = {
  {_recvInfomationData,   NULL},    ///_NETCMD_INFOSEND
  {_recvMystatusData, NULL},    ///_NETCMD_MYSTATUSSEND
  {_recvMystatusData, NULL},    ///_NETCMD_MATCH
};



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

static void _changeState(GTSNEGO_WORK* pWork,StateFunc state)
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
static void _changeStateDebug(GTSNEGO_WORK* pWork,StateFunc state, int line)
{
  NET_PRINT("neg: %d\n",line);
  _changeState(pWork, state);
}
#endif

//_NETCMD_INFOSEND
//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�R�[���o�b�N _NETCMD_INFOSEND
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _recvInfomationData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  GTSNEGO_WORK *pWork = pWk;
  EVENT_GTSNEGO_WORK* pEv=pWork->dbw;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return;//�����͍̂��͎󂯎��Ȃ�
  }
  GFL_STD_MemCopy(pData,&pEv->aUser[1],sizeof(EVENT_GTSNEGO_USER_DATA));
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�R�[���o�b�N _NETCMD_MYSTATUSSEND
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _recvMystatusData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  GTSNEGO_WORK *pWork = pWk;
  EVENT_GTSNEGO_WORK* pEv=pWork->dbw;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return;//�����͍̂��͎󂯎��Ȃ�
  }
  GFL_STD_MemCopy(pData,pEv->pStatus[1], MyStatus_GetWorkSize());
}

//------------------------------------------------------------------------------
/**
 * @brief   �ʐM�R�[���o�b�N�ȈՉ�b _NETCMD_MATCH
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _recvMatchData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle)
{
  GTSNEGO_WORK *pWork = pWk;
  EVENT_GTSNEGO_WORK* pEv=pWork->dbw;

  if(pNetHandle != GFL_NET_HANDLE_GetCurrentHandle()){
    return;	//�����̃n���h���ƈ�v���Ȃ��ꍇ�A�e�Ƃ��Ẵf�[�^��M�Ȃ̂Ŗ�������
  }
  if(netID == GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle())){
    return;//�����͍̂��͎󂯎��Ȃ�
  }
  GFL_STD_MemCopy(pData,&pWork->MatchData, sizeof(MATCH_DATA));
}


//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _AnyoneOrFriendButtonCallback(int bttnid,GTSNEGO_WORK* pWork)
{
  pWork->changeMode = bttnid;
  if(bttnid==0){
    pWork->key1 = _CROSSCUR_TYPE_MAINUP;
    _buttonDecide(pWork, pWork->key1);
    _CHANGE_STATE(pWork,_modeSelectMenuFlash);

  }
  else{
    pWork->key1 = _CROSSCUR_TYPE_MAINDOWN;
    _buttonDecide(pWork, pWork->key1);
    _CHANGE_STATE(pWork,_modeSelectMenuFlash);
  }
  return TRUE;
}


//------------------------------------------------------------------------------
/**
 * @brief   �ق����p�I�����C���N�������g����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _IncMyChageType(int num,GTSNEGO_WORK* pWork)
{
  pWork->myChageType+=num;
  if(_CHANGEMODE_IMAGE_MAX <= pWork->myChageType){
    pWork->myChageType = 0;
  }
  else if(0 > pWork->myChageType){
    pWork->myChageType = _CHANGEMODE_IMAGE_MAX - 1;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �ق����p�I�����C���N�������g����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _IncFriendChageType(int num,GTSNEGO_WORK* pWork)
{
  pWork->friendChageType+=num;
  if(_CHANGEMODE_IMAGE_MAX <= pWork->friendChageType){
    pWork->friendChageType = 0;
  }
  else if(0 > pWork->friendChageType){
    pWork->friendChageType = _CHANGEMODE_IMAGE_MAX - 1;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �ق���LV�I�����C���N�������g����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _IncLevelChageType(int num,GTSNEGO_WORK* pWork)
{
  pWork->chageLevel += num;
  if(_CHANGEMODE_LEVEL_MAX <= pWork->chageLevel){
    pWork->chageLevel = 0;
  }
  else if(0 > pWork->chageLevel){
    pWork->chageLevel = _CHANGEMODE_LEVEL_MAX - 1;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _LevelButtonCallback(int bttnid,GTSNEGO_WORK* pWork)
{
  switch(bttnid){
  case _ARROW_LEVEL_U:
    _IncLevelChageType(-1,pWork);
    GTSNEGO_MESSAGE_DispLevelChange(pWork->pMessageWork,pWork->chageLevel);
    break;
  case _ARROW_LEVEL_D:
    _IncLevelChageType(1,pWork);
    GTSNEGO_MESSAGE_DispLevelChange(pWork->pMessageWork,pWork->chageLevel);
    break;
  case _ARROW_MY_U:
    _IncMyChageType(-1,pWork);
    GTSNEGO_MESSAGE_DispMyChange(pWork->pMessageWork,pWork->myChageType);
    break;
  case _ARROW_MY_D:
    _IncMyChageType(1,pWork);
    GTSNEGO_MESSAGE_DispMyChange(pWork->pMessageWork,pWork->myChageType);
    break;
  case _ARROW_FRIEND_U:
    _IncFriendChageType(-1,pWork);
    GTSNEGO_MESSAGE_DispFriendChange(pWork->pMessageWork,pWork->friendChageType);
    break;
  case _ARROW_FRIEND_D:
    _IncFriendChageType(1,pWork);
    GTSNEGO_MESSAGE_DispFriendChange(pWork->pMessageWork,pWork->friendChageType);
    break;
  }
  if((bttnid >= _ARROW_LEVEL_U) && (bttnid <= _ARROW_FRIEND_D)){
    PMSND_PlaySystemSE(_SE_CUR);
    GTSNEGO_DISP_ArrowAnim(pWork->pDispWork, bttnid);
  }
  return TRUE;
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃL�[����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _LevelKeyCallback(BOOL bRight,GTSNEGO_WORK* pWork)
{
  int key[]={
    _ARROW_LEVEL_U,
    _ARROW_LEVEL_D,
    _ARROW_MY_U,
    _ARROW_MY_D,
    _ARROW_FRIEND_U,
    _ARROW_FRIEND_D,
  };
  int no = pWork->key2 - _CROSSCUR_TYPE_ANY1;

  if((pWork->key2<_CROSSCUR_TYPE_ANY1) && (pWork->key2>_CROSSCUR_TYPE_ANY3)){
    return;
  }

  no = no * 2 + bRight;
  _LevelButtonCallback(no, pWork);
}






static void _messageEnd( GTSNEGO_WORK *pWork )
{
  if(GFL_UI_TP_GetTrg() || GFL_UI_KEY_GetTrg()){
    _CHANGE_STATE(pWork, NULL);
  }
}



static void _messagePMS( GTSNEGO_WORK *pWork )
{
#if _DISP_DEBUG
  PMS_DATA data = {1,1,{1,1}};
  PMS_DATA* pPMS = &data;
#else
  PMS_DATA* pPMS=&pWork->MatchData.pms;
#endif

  pWork->timer--;
  if(pWork->timer==0){
    GTSNEGO_MESSAGE_MainMessageDisp(pWork->pMessageWork, GTSNEGO_040);
    GTSNEGO_MESSAGE_PMSDrawInit(pWork->pMessageWork, pWork->pDispWork);
    GTSNEGO_MESSAGE_PMSDisp( pWork->pMessageWork, pPMS);
    _CHANGE_STATE(pWork, _messageEnd);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	��������+INFO���M
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------

static void _timingCheck2( GTSNEGO_WORK *pWork )
{
  EVENT_GTSNEGO_WORK* pEv=pWork->dbw;
  int i;

#if _DISP_DEBUG
  MYSTATUS* pMy = GAMEDATA_GetMyStatus(pEv->gamedata);
  int num =2;
  
#else
  MYSTATUS* pMy = pEv->pStatus[1];
  int num = pWork->MatchData.num;

  pEv->profileID[pEv->count] = MyStatus_GetProfileID( pMy );
  pEv->count++;
  if(pEv->count >= EVENT_GTSNEGO_RECONNECT_NUM){
    pEv->count = 0;
  }
#endif

  GTSNEGO_MESSAGE_FindPlayer(pWork->pMessageWork, pMy, num);
  GTSNEGO_DISP_SearchEndPeopleDispSet(pWork->pDispWork);
  pWork->timer = _FRIEND_GREE_DOWN_TIME;
  _CHANGE_STATE(pWork,_messagePMS);
}


//----------------------------------------------------------------------------
/**
 *	@brief	�s����������
 */
//-----------------------------------------------------------------------------


static void _timingCheck4( GTSNEGO_WORK *pWork )
{
  EVENT_GTSNEGO_WORK* pEv=pWork->dbw;
  MYSTATUS* pMy = pEv->pStatus[1];
  BOOL res;   // �s��������������
#if (PM_VERSION==VERSION_WHITE)
  int msg = NAMEIN_DEF_NAME_000;
#else
  int msg = NAMEIN_DEF_NAME_001;
#endif

  

  if(DWC_TOOL_BADWORD_Wait( &pWork->aBadWork, &res)){
    if(res){  //�s���̏ꍇ
      MyStatus_SetMyNameFromString(pMy, pWork->wordCheck);
      GFL_STR_DeleteBuffer(pWork->wordCheck);
    }
    _CHANGE_STATE(pWork,_timingCheck2);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�s����������
 */
//-----------------------------------------------------------------------------

static void _timingCheck5( GTSNEGO_WORK *pWork )
{
  EVENT_GTSNEGO_WORK* pEv=pWork->dbw;
  MYSTATUS* pMy = pEv->pStatus[1];
  
  if(!GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_NO3, WB_NET_GTSNEGO)){
    return;
  }

  // �s����������
  pWork->wordCheck  = MyStatus_CreateNameString( pMy, HEAPID_IRCBATTLE );
  DWC_TOOL_BADWORD_Start( &pWork->aBadWork, pWork->wordCheck,  HEAPID_IRCBATTLE );
  
  _CHANGE_STATE(pWork,_timingCheck4);
}


//----------------------------------------------------------------------------
/**
 *	@brief	MYSTATUS���M
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------

static void _statussendCheck( GTSNEGO_WORK *pWork )
{
  GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_NO3, WB_NET_GTSNEGO);
  _CHANGE_STATE(pWork,_timingCheck5);
}

//----------------------------------------------------------------------------
/**
 *	@brief	MYSTATUS���M
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------

static void _infosendCheck( GTSNEGO_WORK *pWork )
{
  EVENT_GTSNEGO_WORK* pEv=pWork->dbw;
  if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_MYSTATUSSEND,
                      MyStatus_GetWorkSize(),pEv->pStatus[0])){
    _CHANGE_STATE(pWork,_statussendCheck);
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief	PMS���M
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------

static void _pmssendCheck( GTSNEGO_WORK *pWork )
{
  if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_MATCH, sizeof(MATCH_DATA),
                      &pWork->myMatchData )){
    _CHANGE_STATE(pWork,_infosendCheck);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	��������+INFO���M
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------

static void _timingCheck( GTSNEGO_WORK *pWork )
{
#if _DISP_DEBUG
    _CHANGE_STATE(pWork,_timingCheck2);
#else

  if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_NO2, WB_NET_GTSNEGO)){
    EVENT_GTSNEGO_WORK* pEv=pWork->dbw;
    if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_INFOSEND, sizeof(EVENT_GTSNEGO_USER_DATA),&pEv->aUser[0])){
      _CHANGE_STATE(pWork,_pmssendCheck);
    }
  }
#endif
}



//----------------------------------------------------------------------------
/**
 *	@brief	���������\��
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------

static void _friendGreeState( GTSNEGO_WORK *pWork )
{
  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }

  if(pWork->timer>0){
    pWork->timer--;
  }
  else{
#if _DISP_DEBUG
    _CHANGE_STATE(pWork,_timingCheck);
#else
    EVENT_GTSNEGO_WORK *pParent = pWork->dbw;

    pParent->aUser[0].selectLV = pWork->myChageType;
    pParent->aUser[0].selectType = pWork->changeMode;
    WIFI_NEGOTIATION_SV_GetMsg(GAMEDATA_GetWifiNegotiation(pWork->pGameData),&pWork->myMatchData.pms);
    pWork->myMatchData.num = WIFI_NEGOTIATION_SV_GetChangeCount(GAMEDATA_GetWifiNegotiation(pWork->pGameData));
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_NO2, WB_NET_GTSNEGO);
    
    _CHANGE_STATE(pWork,_timingCheck);
#endif
  }
}



static void _friendGreeStateParent( GTSNEGO_WORK *pWork )
{
#if _DISP_DEBUG
  _CHANGE_STATE(pWork,_friendGreeState);
#else
  if(GFL_NET_HANDLE_GetNumNegotiation() != 0){
    if(GFL_NET_HANDLE_RequestNegotiation()){
      _CHANGE_STATE(pWork,_friendGreeState);
    }
  }
#endif
}



//----------------------------------------------------------------------------
/**
 *	@brief	���������\��
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------

static void _lookatDownState( GTSNEGO_WORK *pWork )
{
  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  if(pWork->timer>0){
    pWork->timer--;
  }
  else{
    pWork->timer = _FRIEND_GREE_DOWN_TIME;
    if(GFL_NET_SystemGetCurrentID() != GFL_NET_NO_PARENTMACHINE){  // �q�@�Ƃ��Đڑ�����������
      if(GFL_NET_HANDLE_RequestNegotiation()){
        _CHANGE_STATE(pWork,_friendGreeState);
      }
    }
    else{
      _CHANGE_STATE(pWork,_friendGreeStateParent);
    }
  }


}



//----------------------------------------------------------------------------
/**
 *	@brief	�ڑ���
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------

static void _matchingState( GTSNEGO_WORK *pWork )
{
#if _DISP_DEBUG
  if(GFL_UI_KEY_GetTrg()==0){
    return;
  }

  
  if(1){
#else

  GTSNEGO_MESSAGE_ButtonWindowMain(pWork->pMessageWork);

  //�ڑ�������\�����Č�����
//  if(STEPMATCH_SUCCESS == GFL_NET_DWC_GetStepMatchResult()){
  if(GFL_NET_STATE_MATCHED == GFL_NET_StateGetWifiStatus()){

#endif
    
    GTSNEGO_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
    GTSNEGO_MESSAGE_InfoMessageDispLine(pWork->pMessageWork,GTSNEGO_021);

    pWork->timer = _FRIEND_LOOKAT_DOWN_TIME;

    _CHANGE_STATE(pWork, _lookatDownState);
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ڑ��ׂ̈̌���
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------

static int _evalcallback(int index, void* param)
{
  GTSNEGO_WORK *pWork=param;
  EVENT_GTSNEGO_WORK* pEv=pWork->dbw;
  int value = -1;
  int targetlv,targetfriend,targetmy,profile;
  int i;

  targetlv = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_LEVEL].keyStr,-1);
  targetfriend = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_IMAGE_FRIEND].keyStr,-1);
  targetmy = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_IMAGE_MY].keyStr,-1);
  profile = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_PROFILE].keyStr,-1);
  
  if(pWork->changeMode==1){//�Ƃ�����
    if( WIFI_NEGOTIATION_SV_IsCheckFriend( WIFI_NEGOTIATION_SV_GetSaveData(pWork->pSave) ,profile )){
      value=100;
    }
  }
  else{
    if(pWork->chageLevel==targetlv){
      if(pWork->myChageType==targetfriend){
        value+=10;
      }
      if(pWork->friendChageType==targetmy){
        value+=10;
      }
      for(i=0;i<EVENT_GTSNEGO_RECONNECT_NUM;i++){
        if(profile == pEv->profileID[i]){
          value = 0;
          break;
        }
      }
    }
  }
  OS_TPrintf("�]���R�[���o�b�N %d %d %d %d %d\n",value,pWork->changeMode,targetlv,targetmy,targetfriend);
  return value;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�L�[������Đڑ��J�n
 *	@param	bttnid		�{�^��ID
 *	@param	event		�C�x���g���
 *	@param	p_work		���[�N
 */
//-----------------------------------------------------------------------------
static void _matchKeyMake( GTSNEGO_WORK *pWork )
{
  int buff[_MATCHKEY_MAX];
  int i;

  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  if(!GFL_NET_IsInit()){
    _CHANGE_STATE(pWork,_matchingState);
    return;
  }
  

  GFL_NET_SetWifiBothNet(FALSE);
  
  GFL_NET_DWC_GetMySendedFriendCode(pWork->pList, (DWCFriendData*)&buff[0]);

  buff[0] = pWork->profileID;
  buff[1] = pWork->changeMode;
  buff[2] = pWork->chageLevel;
  buff[3] = pWork->myChageType;
  buff[4] = pWork->friendChageType;
  buff[5] = PM_VERSION + (PM_LANG<<16); 
  buff[6] = MATCHINGKEY;

  for(i = 0; i < _MATCHKEY_MAX; i++)
  {
    GameMatchExtKeys* pMatchKey = &pWork->aMatchKey[i];
    GFL_STD_MemCopy(&matchkeystr[i], pMatchKey->keyStr,2);
    pMatchKey->ivalue = buff[i];
    pMatchKey->keyID  = DWC_AddMatchKeyInt(pMatchKey->keyID, pMatchKey->keyStr, &pMatchKey->ivalue);
#if PM_DEBUG
    if (pMatchKey->keyID == 0){
      NET_PRINT("AddMatchKey failed %d.\n",i);
    }
#endif
  }

  //���̏����͊m�� matchkeystr�����킹�ĕύX���鎖 
  STD_TSPrintf( pWork->filter, "ty=%d And db=%d", pWork->changeMode,MATCHINGKEY);

  if( GFL_NET_DWC_StartMatchFilter( pWork->filter, 2 ,&_evalcallback, pWork) != 0){

    GFL_NET_DWC_SetVChat(FALSE);

    _CHANGE_STATE(pWork,_matchingState);
  }
  else{
    _CHANGE_STATE(pWork,NULL);  //�G���[�\��
  }

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
  GTSNEGO_WORK *pWork = p_work;
  u32 friendNo;

  switch( event ){
  case GFL_BMN_EVENT_TOUCH:		///< �G�ꂽ�u��
    if(pWork->touch!=NULL){
      GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,pWork->pAppWin, _CROSSCUR_TYPE_NONE);
      pWork->keyMode=FALSE;
      if(pWork->touch(bttnid, pWork)){
        return;
      }
    }
    break;
  default:
    break;
  }
}



static void _levelSelectDecide( GTSNEGO_WORK *pWork )
{
  APP_TASKMENU_WIN_Delete(pWork->pAppWin);
  pWork->pAppWin = NULL;

  GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_019);
  GTSNEGO_DISP_SearchPeopleDispSet(pWork->pDispWork);
  GTSNEGO_MESSAGE_TitleMessage(pWork->pMessageWork,GTSNEGO_018);

  GTSNEGO_MESSAGE_CancelButtonCreate(pWork->pMessageWork, &_cancelButtonCallback, pWork );
//  GTSNEGO_MESSAGE_CancelButtonCreate(pWork->pMessageWork);
//  pWork->pAppWin = GTSNEGO_MESSAGE_SearchButtonStart(pWork->pMessageWork,GTSNEGO_020);
  _CHANGE_STATE(pWork, _matchKeyMake);
}


//------------------------------------------------------------------
/**
 * $brief   ���x���m�F�҂�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

const static GFL_UI_TP_HITTBL _tp_data[]={
  { 24*8-32 , 24*8, 128-32, 128+32},
  {GFL_UI_TP_HIT_END,0,0,0},
};

static void _levelSelectWait( GTSNEGO_WORK *pWork )
{
  BOOL bHit=FALSE;
  BOOL bReturn=FALSE;
  
  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }

  GTSNEGO_MESSAGE_ButtonWindowMain(pWork->pMessageWork);

  if(GFL_UI_KEY_GetTrg() == PAD_KEY_LEFT){
    pWork->keyMode = TRUE;
    _LevelKeyCallback(FALSE, pWork);
  }
  if(GFL_UI_KEY_GetTrg() == PAD_KEY_RIGHT){
    pWork->keyMode = TRUE;
    _LevelKeyCallback(TRUE, pWork);
  }
  
  if(GFL_UI_KEY_GetTrg() == PAD_KEY_UP){
    PMSND_PlaySystemSE(_SE_CUR);
    bHit=TRUE;
    if(!pWork->keyMode){
      pWork->keyMode=TRUE;
    }
    else{
      if(pWork->key2 != _CROSSCUR_TYPE_ANY1){
        pWork->key2--;
      }
    }
  }
  if(GFL_UI_KEY_GetTrg() == PAD_KEY_DOWN){
    PMSND_PlaySystemSE(_SE_CUR);
    bHit=TRUE;
    if(!pWork->keyMode){
      pWork->keyMode=TRUE;
    }
    else{
      if(pWork->key2 != _CROSSCUR_TYPE_ANY4){
        pWork->key2++;
      }
    }
  }
  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
    if(pWork->key2 == _CROSSCUR_TYPE_ANY4){
      PMSND_PlaySystemSE(_SE_DECIDE);
      _CHANGE_STATE(pWork, _levelSelectDecide);
      return;
    }
  }

  
  if(bHit){
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, pWork->pAppWin, pWork->key2);
  }

  switch(GFL_UI_TP_HitTrg(_tp_data)){
  case 0:
    PMSND_PlaySystemSE(_SE_DECIDE);
    _CHANGE_STATE(pWork, _levelSelectDecide);
    return;
    break;
  }

  {
    TOUCHBAR_Main(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork));
    switch( TOUCHBAR_GetTrg(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork))){
    case TOUCHBAR_ICON_RETURN:
      pWork->keyMode = FALSE;
      bReturn = TRUE;
      break;
    default:
      break;
    }
  }
  if(bReturn){
    GTSNEGO_MESSAGE_DeleteDispLevel(pWork->pMessageWork);
    GTSNEGO_DISP_LevelInputFree(pWork->pDispWork);
    APP_TASKMENU_WIN_Delete(pWork->pAppWin);
    pWork->pAppWin = NULL;
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_S );
    _CHANGE_STATE(pWork,_modeSelectMenuInit);
  }
}

//------------------------------------------------------------------
/**
 * $brief   ���x���m�F
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static void _levelSelect( GTSNEGO_WORK *pWork )
{
  GTSNEGO_MESSAGE_DispClear(pWork->pMessageWork);
  GTSNEGO_DISP_LevelInputInit(pWork->pDispWork);
  pWork->touch = &_LevelButtonCallback;

  GTSNEGO_MESSAGE_DispLevel(pWork->pMessageWork, &_BttnCallBack, pWork);

  GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_025);
  
  pWork->pAppWin = GTSNEGO_MESSAGE_SearchButtonStart(pWork->pMessageWork,GTSNEGO_023);

  _CHANGE_STATE(pWork,_levelSelectWait);
}





//----------------------------------------------------------------------------
/**
 *	@brief	���Ԃ�MYSTATUS�𓾂�
 *	@param	POKEMON_TRADE_WORK
 *	@return	none
 */
//-----------------------------------------------------------------------------

MYSTATUS* GTSNEGO_GetMyStatus( GAMEDATA* pGameData, int index)
{
  int i, j, count;
  MYSTATUS* pMyStatus;

  if((index >= WIFI_NEGOTIATION_DATAMAX) || (index < 0)){
    return NULL;
  }
  
  count = WIFI_NEGOTIATION_SV_GetCount(GAMEDATA_GetWifiNegotiation(pGameData));

  j = count - 1 - index;
  if(j<0){
    j+=WIFI_NEGOTIATION_DATAMAX;
  }
  j = j % WIFI_NEGOTIATION_DATAMAX;
  pMyStatus = WIFI_NEGOTIATION_SV_GetMyStatus(GAMEDATA_GetWifiNegotiation(pGameData), j);
  return pMyStatus;
}



//------------------------------------------------------------------------------
/**
 * @brief   �L�����Z���{�^���̏���
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _MatchingCancelState2(GTSNEGO_WORK* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    GTSNEGO_MESSAGE_AppMenuClose(pWork->pAppTask);
    pWork->pAppTask=NULL;
    TOUCHBAR_SetVisible(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork), TOUCHBAR_ICON_RETURN, TRUE);
    switch(selectno){
    case 0:
      GFL_NET_StateWifiMatchEnd(TRUE);
      GTSNEGO_DISP_ResetDispSet(pWork->pDispWork);
      GTSNEGO_MESSAGE_ResetDispSet(pWork->pMessageWork);
      _CHANGE_STATE(pWork,_modeSelectMenuInit);
      break;
    case 1:
      GTSNEGO_MESSAGE_CancelButtonCreate(pWork->pMessageWork, &_cancelButtonCallback, pWork );
      
      _CHANGE_STATE(pWork,_matchingState);
    }
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �L�����Z���{�^���̏���
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _MatchingCancelState(GTSNEGO_WORK* pWork)
{
  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  TOUCHBAR_SetVisible(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork), TOUCHBAR_ICON_RETURN, FALSE);
  pWork->pAppTask = GTSNEGO_MESSAGE_YesNoStart(pWork->pMessageWork, GTSNEGO_YESNOTYPE_INFO);
  _CHANGE_STATE(pWork,_MatchingCancelState2);
}

//------------------------------------------------------------------------------
/**
 * @brief   �L�����Z���{�^���̃R�[���o�b�N
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _cancelButtonCallback(u32 bttnid, u32 event,void* p_work)
{
  GTSNEGO_WORK *pWork = p_work;
  
  switch( event ){
  case GFL_BMN_EVENT_TOUCH:		///< �G�ꂽ�u��
    //�}�b�`���O���������s
    if(pWork->state == &_matchingState){
      GTSNEGO_MESSAGE_CancelButtonDelete(pWork->pMessageWork);
      GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_043);
      _CHANGE_STATE(pWork , _MatchingCancelState);
    }
    break;
  }
}




static void _friendSelectDecide3( GTSNEGO_WORK *pWork )
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    GTSNEGO_MESSAGE_AppMenuClose(pWork->pAppTask);
    pWork->pAppTask=NULL;
    TOUCHBAR_SetVisible(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork), TOUCHBAR_ICON_RETURN, TRUE);
    switch(selectno){
    case 0:
      GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_019);
      GTSNEGO_DISP_SearchPeopleDispSet(pWork->pDispWork);

      GTSNEGO_MESSAGE_TitleMessage(pWork->pMessageWork,GTSNEGO_018);

//      GTSNEGO_MESSAGE_CancelButtonCreate(pWork->pMessageWork);
      GTSNEGO_MESSAGE_CancelButtonCreate(pWork->pMessageWork, &_cancelButtonCallback, pWork );

      _CHANGE_STATE(pWork,_matchKeyMake);
      break;
    case 1:
      G2S_BlendNone();
      _CHANGE_STATE(pWork,_friendSelectWait);
      break;
    }
  }
}



static void _friendSelectDecide2( GTSNEGO_WORK *pWork )
{
  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  TOUCHBAR_SetVisible(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork), TOUCHBAR_ICON_RETURN, FALSE);
  pWork->pAppTask = GTSNEGO_MESSAGE_MatchOrReturnStart(pWork->pMessageWork, GTSNEGO_YESNOTYPE_SYS);
  _CHANGE_STATE(pWork,_friendSelectDecide3);
}

//------------------------------------------------------------------
/**
 * $brief   �Ƃ������I������
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------
static void _friendSelectDecide( GTSNEGO_WORK *pWork )
{
  GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_036);

  
  _CHANGE_STATE(pWork,_friendSelectDecide2);


  //_CHANGE_STATE(pWork,_matchKeyMake);
}


//------------------------------------------------------------------
/**
 * $brief   ��������I��҂�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

const static GFL_UI_TP_HITTBL _tp_data2[]={
  { 8*3, 8*(3+6), 8*1, 8*(32-3)},
  { 8*(3+6), 8*(3+6*2), 8*1, 8*(32-3)},
  { 8*(3+6*2), 8*(3+6*3), 8*1, 8*(32-3)},
  {GFL_UI_TP_HIT_END,0,0,0},
};

const static GFL_UI_TP_HITTBL _tp_data3[]={
  { SCROLLBAR_TOP, SCROLLBAR_TOP+SCROLLBAR_LENGTH, 8*(32-3), 8*32},   //�X�N���[���o�[
  {GFL_UI_TP_HIT_END,0,0,0},
};



static void _friendSelectWait( GTSNEGO_WORK *pWork )
{
  BOOL bHit=FALSE;
  int scrollType=PANEL_NONESCROLL_,ret;
  
  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  ret = GTSNEGO_DISP_PanelScrollMain(pWork->pDispWork,&scrollType);
  if( PANEL_UPSCROLL_ == scrollType){
    GTSNEGO_DISP_FriendSelectPlateView(pWork->pDispWork,pWork->pGameData, pWork->scrollPanelCursor.oamlistpos-2);
    GTSNEGO_MESSAGE_FriendListUpEnd(pWork->pMessageWork);

    OS_TPrintf("GTSNEGO_DISP_UnionListUp %d\n",pWork->scrollPanelCursor.oamlistpos-2);

    
    GTSNEGO_DISP_UnionListUp(pWork->pDispWork, GTSNEGO_GetMyStatus(pWork->pGameData ,pWork->scrollPanelCursor.oamlistpos-2));
  }
  else if( PANEL_DOWNSCROLL_ == scrollType){
    GTSNEGO_DISP_FriendSelectPlateView(pWork->pDispWork,pWork->pGameData, pWork->scrollPanelCursor.oamlistpos-2);
    GTSNEGO_MESSAGE_FriendListDownEnd(pWork->pMessageWork);
    OS_TPrintf("GTSNEGO_DISP_UnionListDown %d\n",pWork->scrollPanelCursor.oamlistpos + (SCROLL_PANEL_NUM-3));

    GTSNEGO_DISP_UnionListDown(pWork->pDispWork, GTSNEGO_GetMyStatus(pWork->pGameData ,pWork->scrollPanelCursor.oamlistpos + (SCROLL_PANEL_NUM-3)));
  }
  if(ret != PANEL_NONESCROLL_){
    return;
  }

  if(GFL_UI_KEY_GetRepeat() == PAD_KEY_UP){
    bHit=TRUE;
    PMSND_PlaySystemSE(_SE_CUR);
    if(!pWork->keyMode){
      pWork->keyMode=TRUE;
    }
    else{
      if( GTSNEGO_DISP_FriendListUpChk(pWork->pDispWork, &pWork->scrollPanelCursor)){
        GTSNEGO_DISP_PanelScrollStart(pWork->pDispWork,PANEL_UPSCROLL_);
        if(pWork->scrollPanelCursor.oamlistpos - 1 >= 0){
          GTSNEGO_MESSAGE_FriendListUpStart(pWork->pMessageWork, pWork->pGameData,
                                              pWork->scrollPanelCursor.oamlistpos-1 );
        }
      }
      else if(pWork->key3 != _CROSSCUR_TYPE_FRIEND1){
        pWork->key3--;
      }
    }
  }
  if(GFL_UI_KEY_GetRepeat() == PAD_KEY_DOWN){
    bHit=TRUE;
    PMSND_PlaySystemSE(_SE_CUR);
    if(!pWork->keyMode){
      pWork->keyMode=TRUE;
    }
    else{
      if( GTSNEGO_DISP_FriendListDownChk(pWork->pDispWork, &pWork->scrollPanelCursor)){
        GTSNEGO_DISP_PanelScrollStart(pWork->pDispWork,PANEL_DOWNSCROLL_);
        GTSNEGO_MESSAGE_FriendListDownStart(pWork->pMessageWork, pWork->pGameData,
                                            pWork->scrollPanelCursor.oamlistpos + 2);
      }
      else if(pWork->key3 != _CROSSCUR_TYPE_FRIEND3){
        pWork->key3++;
      }
    }
  }
  if(bHit){
    GTSNEGO_DISP_ScrollChipDisp(pWork->pDispWork,pWork->scrollPanelCursor.oamlistpos + pWork->key3 - _CROSSCUR_TYPE_FRIEND1,
                                pWork->scrollPanelCursor.listmax );
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, NULL, pWork->key3);
  }

  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){  // �L�[�ł̌���
    PMSND_PlaySystemSE(_SE_DECIDE);

    GTSNEGO_DISP_CrossIconFlash(pWork->pDispWork , pWork->key3);

    pWork->selectFriendIndex = pWork->scrollPanelCursor.oamlistpos + pWork->key3  - _CROSSCUR_TYPE_FRIEND1;
    OS_TPrintf("�I�񂾔ԍ� %d\n", pWork->selectFriendIndex);
    _CHANGE_STATE(pWork,_friendSelectDecide);
    return;
  }


  {  //TP�ŒN��I�񂾂̂�
    int trgindex=GFL_UI_TP_HitTrg(_tp_data2);
    switch(trgindex){
    case 0:
    case 1:
    case 2:
      pWork->key3 = trgindex  + _CROSSCUR_TYPE_FRIEND1;
      GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, NULL, pWork->key3);
      pWork->selectFriendIndex = pWork->scrollPanelCursor.oamlistpos + pWork->key3  - _CROSSCUR_TYPE_FRIEND1;
      OS_TPrintf("�I�񂾔ԍ� %d\n", pWork->selectFriendIndex);
//      pWork->selectFriendIndex = trgindex;
      PMSND_PlaySystemSE(_SE_DECIDE);
      _CHANGE_STATE(pWork,_friendSelectDecide);
      return;
    }
  }
  if(GFL_UI_TP_HitCont(_tp_data3)==0){   //�^�b�`�p�l���̃X���C�h�o�[
    u32 x,y;
    if(GFL_UI_TP_GetPointCont(&x, &y)==TRUE){
      int no = GTSNEGO_DISP_ScrollChipDispMouse(pWork->pDispWork, y,
                                                pWork->scrollPanelCursor.listmax);
      if(pWork->scrollPanelCursor.oamlistpos!=no){
        pWork->scrollPanelCursor.oamlistpos = no;
        GTSNEGO_MESSAGE_FriendListRenew(pWork->pMessageWork, pWork->pGameData,
                                        pWork->scrollPanelCursor.oamlistpos-2 );
        GTSNEGO_DISP_UnionListRenew(pWork->pDispWork, pWork->pGameData,
                                    pWork->scrollPanelCursor.oamlistpos-2 );
        GTSNEGO_DISP_FriendSelectPlateView(pWork->pDispWork,pWork->pGameData,
                                           pWork->scrollPanelCursor.oamlistpos-2);
      }
    }
    return;
  }
  TOUCHBAR_Main(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork));
  switch( TOUCHBAR_GetTrg(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork))){
  case TOUCHBAR_ICON_RETURN:
    GTSNEGO_DISP_FriendSelectFree(pWork->pDispWork);
    GTSNEGO_MESSAGE_DispClear(pWork->pMessageWork);
    _CHANGE_STATE(pWork,_modeSelectMenuInit);
    break;
  default:
    break;
  }

}

//------------------------------------------------------------------
/**
 * $brief   ��������I��
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static void _friendSelect( GTSNEGO_WORK *pWork )
{
  int i;
  BOOL bCursor = TRUE;
  int a = WIFI_NEGOTIATION_SV_GetFriendNum(GAMEDATA_GetWifiNegotiation(pWork->pGameData));

  
  if(a==0){
    GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_034);
    _CHANGE_STATE(pWork,_modeSelectMenuWait);
    return;
  }
  pWork->scrollPanelCursor.listmax = a;

  GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_024);

  if(pWork->keyMode){
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, pWork->key3);
  }

  GTSNEGO_MESSAGE_DispClear(pWork->pMessageWork);

  if(a < 3){
    bCursor = FALSE;
  }
  
  GTSNEGO_DISP_FriendSelectInit(pWork->pDispWork, bCursor);
  
  GTSNEGO_MESSAGE_FriendListPlateDisp(pWork->pMessageWork,pWork->pGameData);


  
//  for(i=2;i<SCROLL_PANEL_NUM;i++){
//    MYSTATUS* pMy = GTSNEGO_GetMyStatus(pWork->pGameData, i-2);
//    GTSNEGO_DISP_UnionListDisp(pWork->pDispWork, pMy, i);
//  }
  GTSNEGO_DISP_UnionListRenew(pWork->pDispWork,pWork->pGameData, -2);

  GTSNEGO_DISP_FriendSelectPlateView(pWork->pDispWork,pWork->pGameData, -2);
  GTSNEGO_DISP_ScrollChipDisp(pWork->pDispWork,
                              pWork->scrollPanelCursor.oamlistpos + pWork->key3 - _CROSSCUR_TYPE_FRIEND1,
                              pWork->scrollPanelCursor.listmax );

  _CHANGE_STATE(pWork,_friendSelectWait);
}






//------------------------------------------------------------------------------
/**
 * @brief   �ڑ��J�n
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _connectionStart(GTSNEGO_WORK* pWork)
{

//  _CHANGE_STATE(pWork,_connectingWait);

}



//------------------------------------------------------------------------------
/**
 * @brief   ����ł������肠�����I��
 * @retval  none
 */
//------------------------------------------------------------------------------



static void _modeSelectMenuInit(GTSNEGO_WORK* pWork)
{
  GTSNEGO_DISP_ScrollReset(pWork->pDispWork);
  if(pWork->keyMode){
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, pWork->key1);
  }
  else{
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, _CROSSCUR_TYPE_NONE);
  }
  GTSNEGO_MESSAGE_DispAnyoneOrFriend(pWork->pMessageWork, &_BttnCallBack, pWork);
  pWork->touch = &_AnyoneOrFriendButtonCallback;

  GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_037);



  _CHANGE_STATE(pWork,_modeSelectMenuWait);
}


//------------------------------------------------------------------------------
/**
 * @brief   �{�^���̃t���b�V�����s���A�����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------

static int _buttonDecide(GTSNEGO_WORK* pWork, int key)
{
  if(GTSNEGO_DISP_CrossIconFlash(pWork->pDispWork, key)){
    PMSND_PlaySystemSE(_SE_DECIDE);
    return key;
  }
  return _CROSSCUR_TYPE_NONE;
}

//------------------------------------------------------------------------------
/**
 * @brief   �{�^���̃t���b�V�����s���A�����Ԃ�
 * @retval  none
 */
//------------------------------------------------------------------------------

static int _buttonCheck(GTSNEGO_WORK* pWork, int key)
{
  if(GTSNEGO_DISP_CrossIconFlashEnd(pWork->pDispWork) ){
    return key;
  }
  return _CROSSCUR_TYPE_NONE;
}



//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuFlash(GTSNEGO_WORK* pWork)
{
  int buttonNo = _buttonCheck(pWork, pWork->key1);
  if(buttonNo==_CROSSCUR_TYPE_MAINUP){
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, pWork->key2);
    _CHANGE_STATE(pWork,_levelSelect);
    return;
  }
  if(buttonNo==_CROSSCUR_TYPE_MAINDOWN){
    _CHANGE_STATE(pWork,_friendSelect);
    return;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuWait(GTSNEGO_WORK* pWork)
{
  BOOL bHit = FALSE;
  int buttonNo;
  
  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }

  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
    if(!pWork->keyMode){
      pWork->keyMode=TRUE;
    }
    else{
      _buttonDecide(pWork, pWork->key1);
      _CHANGE_STATE(pWork,_modeSelectMenuFlash);
      return;
    }
  }

  
  if(GFL_UI_KEY_GetTrg() == PAD_KEY_UP){
    bHit=TRUE;
    PMSND_PlaySystemSE(_SE_CUR);
    if(!pWork->keyMode){
      pWork->keyMode=TRUE;
    }
    else{
      pWork->key1=_CROSSCUR_TYPE_MAINUP;
    }
  }
  if(GFL_UI_KEY_GetTrg() == PAD_KEY_DOWN){
    bHit=TRUE;
    PMSND_PlaySystemSE(_SE_CUR);
    if(!pWork->keyMode){
      pWork->keyMode=TRUE;
    }
    else{
      pWork->key1=_CROSSCUR_TYPE_MAINDOWN;
    }
  }
  if(bHit){
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, pWork->key1);
  }
  
  GTSNEGO_MESSAGE_ButtonWindowMain(pWork->pMessageWork);
  
  TOUCHBAR_Main(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork));
  switch( TOUCHBAR_GetTrg(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork))){
  case TOUCHBAR_ICON_RETURN:
    _CHANGE_STATE(pWork,NULL);
    break;
  default:
    break;
  }


}



//------------------------------------------------------------------------------
/**
 * @brief   PROC�X�^�[�g
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT GameSyncMenuProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_GTSNEGO_WORK* pEv=pwk;
  GTSNEGO_WORK *pWork;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x38000 );

  
  pWork = GFL_PROC_AllocWork( proc, sizeof( GTSNEGO_WORK ), HEAPID_IRCBATTLE );
  GFL_STD_MemClear(pWork, sizeof(GTSNEGO_WORK));
  pWork->heapID = HEAPID_IRCBATTLE;
  pWork->dbw = pEv;
  pWork->pDispWork = GTSNEGO_DISP_Init(pWork->heapID, pEv->gamedata);
  pWork->pMessageWork = GTSNEGO_MESSAGE_Init(pWork->heapID, NARC_message_gtsnego_dat);
  pWork->pGameData = pEv->gamedata;
  pWork->pSave = GAMEDATA_GetSaveControlWork(pEv->gamedata);
  pWork->pList = GAMEDATA_GetWiFiList(pEv->gamedata);

  pWork->key1=_CROSSCUR_TYPE_MAINUP;
  pWork->key2=_CROSSCUR_TYPE_ANY1;
  pWork->key3=_CROSSCUR_TYPE_FRIEND1;

  pWork->profileID = MyStatus_GetProfileID( GAMEDATA_GetMyStatus(pEv->gamedata) );
  
  if(GFL_NET_IsInit()){
    GFL_NET_AddCommandTable(GFL_NET_CMD_GTSNEGO,_PacketTbl,NELEMS(_PacketTbl), pWork);
  }


  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
  
  _CHANGE_STATE(pWork, _modeSelectMenuInit);

  return GFL_PROC_RES_FINISH;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCMain
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT GameSyncMenuProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  GTSNEGO_WORK* pWork = mywk;
  GFL_PROC_RESULT retCode = GFL_PROC_RES_FINISH;

  StateFunc* state = pWork->state;
  if(state != NULL){
    state(pWork);
    retCode = GFL_PROC_RES_CONTINUE;
  }

  if(pWork->pAppWin){
    APP_TASKMENU_WIN_Update( pWork->pAppWin );
  }
  if(pWork->pAppTask){
    APP_TASKMENU_UpdateMenu(pWork->pAppTask);
  }

  GTSNEGO_DISP_Main(pWork->pDispWork);
  GTSNEGO_MESSAGE_Main(pWork->pMessageWork);


  //INFOWIN_Update();

  return retCode;
}

//------------------------------------------------------------------------------
/**
 * @brief   PROCEnd
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_PROC_RESULT GameSyncMenuProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EVENT_GTSNEGO_WORK* pEv=pwk;
  GTSNEGO_WORK* pWork = mywk;

  TOUCHBAR_Exit(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork));

  if(pWork->pAppWin){
    APP_TASKMENU_WIN_Delete(pWork->pAppWin);
  }

  GTSNEGO_MESSAGE_End(pWork->pMessageWork);
  GTSNEGO_DISP_End(pWork->pDispWork);

  GFL_PROC_FreeWork(proc);



//  GFL_BG_FreeBGControl(_SUBSCREEN_BGPLANE);
  GFL_HEAP_DeleteHeap(HEAPID_IRCBATTLE);



  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA GtsNego_ProcData = {
  GameSyncMenuProcInit,
  GameSyncMenuProcMain,
  GameSyncMenuProcEnd,
};


