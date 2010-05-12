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
#include "system/debug_print.h"
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
#include "savedata/etc_save.h"

#include "msg/msg_gtsnego.h"
#include "../../field/event_gtsnego.h"
#include "gtsnego.naix"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK
#include "gtsnego_local.h"
#include "system/net_err.h"
#include "net/dwc_error.h"


#define _NO2   (2)
#define _NO3   (3)

#define _WORK_HEAPSIZE (0x1000)  // �������K�v


//--------------------------------------------
// ��ʍ\����`
//--------------------------------------------

//BG�ʂƃp���b�g�ԍ�

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
  int changeMode;    //�L�[�ɂ���l ����ł����Ƃ�������  CHANGEMODE_FRIEND
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
  BOOL bSingle;
  DWC_TOOL_BADWORD_WORK aBadWork;
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
static void _messageEndCheck(GTSNEGO_WORK* pWork);
static void _cancelFlash(GTSNEGO_WORK* pWork);

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
static void _recvMatchData(const int netID, const int size, const void* pData, void* pWk, GFL_NETHANDLE* pNetHandle);

///�ʐM�R�}���h�e�[�u��
static const NetRecvFuncTable _PacketTbl[] = {
  {_recvInfomationData,   NULL},    ///_NETCMD_INFOSEND
  {_recvMystatusData, NULL},    ///_NETCMD_MYSTATUSSEND
  {_recvMatchData, NULL},    ///_NETCMD_MATCH
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
  OHNO_Printf("GTStype %d %d\n",pEv->aUser[1].selectType ,pEv->aUser[1].selectLV);
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

  GFL_UI_SetTouchOrKey(GFL_APP_KTST_TOUCH);

  if(pWork->bSingle == TRUE){
    PMSND_PlaySystemSE(_SE_CANCEL);
    return TRUE;
  }
  if(bttnid==0){
    pWork->key1 = _CROSSCUR_TYPE_MAINUP;
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, pWork->pAppWin,pWork->key1);
    _buttonDecide(pWork, pWork->key1);
    _CHANGE_STATE(pWork,_modeSelectMenuFlash);

  }
  else{
    pWork->key1 = _CROSSCUR_TYPE_MAINDOWN;
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, pWork->pAppWin,pWork->key1);
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


static void _wipeEnd( GTSNEGO_WORK *pWork )
{
  if(WIPE_SYS_EndCheck()){
    _CHANGE_STATE(pWork, NULL);        // �I���
  }
}


static void _messageEnd( GTSNEGO_WORK *pWork )
{
  pWork->timer--;
  if(pWork->timer<=0){
    WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
    _CHANGE_STATE(pWork, _wipeEnd);
  }
}



static void _messagePMS( GTSNEGO_WORK *pWork )
{
  PMS_DATA data = {1,1,{1,1}};
  PMS_DATA* pPMS;
  
  if(!GFL_NET_IsInit()){
    pPMS = &data;
  }
  else{
    pPMS=&pWork->MatchData.pms;
  }

  pWork->timer--;

  if(pWork->timer==20){
    GTSNEGO_MESSAGE_InfoMessageDispLine(pWork->pMessageWork,GTSNEGO_044);
  }
  if(pWork->timer==0){
    EVENT_GTSNEGO_WORK *pParent = pWork->dbw;
//    pParent->result = TRUE;
    pWork->timer = _FRIEND_GREE_DOWN_TIME;
//    GTSNEGO_MESSAGE_MainMessageDisp(pWork->pMessageWork, GTSNEGO_040);
    GTSNEGO_MESSAGE_DispCountryInfo(pWork->pMessageWork, GTSNEGO_040);
    GTSNEGO_MESSAGE_DeleteCountryMsg(pWork->pMessageWork);

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
  int no=0;
  MYSTATUS* pMy;
  int num = 2;

  if(!GFL_NET_IsInit()){
    pMy = GAMEDATA_GetMyStatus(pEv->gamedata);
  }
  else{
    pMy = pEv->pStatus[1];
    num = pWork->MatchData.num;
    pEv->profileID[pEv->count] = MyStatus_GetProfileID( pMy );
    pEv->count++;
    if(pEv->count >= EVENT_GTSNEGO_RECONNECT_NUM){
      pEv->count = 0;
    }
  }

  GTSNEGO_MESSAGE_FindPlayer(pWork->pMessageWork, pMy, num);
  GTSNEGO_DISP_SearchEndPeopleDispSet(pWork->pDispWork, MyStatus_GetTrainerView(pMy));
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
  MYSTATUS* pTargetSt;
  MYSTATUS* pMy = pEv->pStatus[1];
  BOOL res  = FALSE;   // �s��������������

  if(DWC_TOOL_BADWORD_Wait( &pWork->aBadWork, &res)){
    if(res){  //�s���̏ꍇ
      STRBUF  *badword  = DWC_TOOL_CreateBadNickName( HEAPID_IRCBATTLE );
      MyStatus_SetMyNameFromString(pMy, badword);  //OK�Ȃ��̂������
      GFL_STR_DeleteBuffer(badword);
    }
    pTargetSt = GAMEDATA_GetMyStatusPlayer(pWork->pGameData, 1-GFL_NET_SystemGetCurrentID());
    MyStatus_Copy(pMy,pTargetSt);
    {
      int no = MyStatus_GetID(pMy);
      EtcSave_SetAcquaintance( SaveData_GetEtc( pWork->pSave ), no);
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
  STRBUF  *wordCheck;
  
  if(!GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_NO3, WB_NET_GTSNEGO)){
    return;
  }

  // �s����������
  wordCheck  = MyStatus_CreateNameString( pMy, HEAPID_IRCBATTLE );
  DWC_TOOL_BADWORD_Start( &pWork->aBadWork, wordCheck,  HEAPID_IRCBATTLE );
  GFL_STR_DeleteBuffer(wordCheck);
  
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
  if(!GFL_NET_IsInit()){
    _CHANGE_STATE(pWork,_timingCheck2);
  }
  else{
    if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_NO2, WB_NET_GTSNEGO)){
      EVENT_GTSNEGO_WORK* pEv=pWork->dbw;
      OHNO_Printf("GTStype %d %d\n",pEv->aUser[0].selectType ,pEv->aUser[0].selectLV);
      
      if(GFL_NET_SendData(GFL_NET_HANDLE_GetCurrentHandle(),_NETCMD_INFOSEND, sizeof(EVENT_GTSNEGO_USER_DATA),&pEv->aUser[0])){
        _CHANGE_STATE(pWork,_pmssendCheck);
      }
    }
  }
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
  if(!GFL_NET_IsInit()){
    _CHANGE_STATE(pWork,_timingCheck);
    return;
  }

  if(pWork->timer>0){
    pWork->timer--;
  }
  else if(GFL_NET_HANDLE_GetNumNegotiation()==2){
    EVENT_GTSNEGO_WORK *pParent = pWork->dbw;
    pParent->aUser[0].selectLV = pWork->chageLevel;
    pParent->aUser[0].selectType = pWork->myChageType;
    OHNO_Printf("myChageType %d %d\n",pWork->chageLevel,pWork->myChageType);

    WIFI_NEGOTIATION_SV_GetMsg(GAMEDATA_GetWifiNegotiation(pWork->pGameData),&pWork->myMatchData.pms);
    pWork->myMatchData.num = WIFI_NEGOTIATION_SV_GetChangeCount(GAMEDATA_GetWifiNegotiation(pWork->pGameData));
    GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_NO2, WB_NET_GTSNEGO);
    _CHANGE_STATE(pWork,_timingCheck);
  }
}



static void _friendGreeStateParent( GTSNEGO_WORK *pWork )
{
  if(!GFL_NET_IsInit()){
    _CHANGE_STATE(pWork,_friendGreeState);
  }
  else{
    if(GFL_NET_HANDLE_GetNumNegotiation() != 0){
      if(GFL_NET_HANDLE_RequestNegotiation()){
        _CHANGE_STATE(pWork,_friendGreeState);
      }
    }
  }
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
    if(GFL_NET_SystemGetCurrentID() != GFL_NET_NO_PARENTMACHINE){  // �q�@�Ƃ��Đڑ�����������
      if(GFL_NET_HANDLE_RequestNegotiation()){
        pWork->timer = _FRIEND_GREE_DOWN_TIME;
        _CHANGE_STATE(pWork,_friendGreeState);
      }
    }
    else{
      pWork->timer = _FRIEND_GREE_DOWN_TIME;
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
  GTSNEGO_MESSAGE_ButtonWindowMain(pWork->pMessageWork);

  if(GFL_UI_KEY_GetTrg()==PAD_BUTTON_CANCEL)
  {
    GTSNEGO_MESSAGE_CancelButtonDecide(pWork->pMessageWork);
    if(GFL_NET_IsInit()){
      GFL_NET_StateWifiMatchEnd(TRUE);
    }
    PMSND_PlaySystemSE(_SE_CANCEL);
    _CHANGE_STATE(pWork,_cancelFlash);
  }

  
  //�ڑ�������\�����Č�����
  if(!GFL_NET_IsInit()){
    if(GFL_UI_KEY_GetTrg()!=PAD_BUTTON_DECIDE){
      return;
    }
  }
  else{
    if(GFL_NET_STATE_MATCHED != GFL_NET_StateGetWifiStatus()){
      return;
    }
  }

  {
    GTSNEGO_MESSAGE_InfoMessageEnd(pWork->pMessageWork);
    GTSNEGO_MESSAGE_InfoMessageDispLine(pWork->pMessageWork,GTSNEGO_021);
    pWork->timer = _FRIEND_LOOKAT_DOWN_TIME;
    GTSNEGO_MESSAGE_CancelButtonDeleteForce(pWork->pMessageWork);
    GFL_NET_SetAutoErrorCheck(TRUE);
    GFL_NET_SetNoChildErrorCheck(TRUE);
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
  
  if(pWork->changeMode==_CHANGEMODE_SELECT_FRIEND){//�Ƃ�����
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
#if DEBUG_ONLY_FOR_ohno
#else
      for(i=0;i<EVENT_GTSNEGO_RECONNECT_NUM;i++){
        if(profile == pEv->profileID[i]){
          value = 0;
          break;
        }
      }
#endif
    }
  }
  OHNO_Printf("�]���R�[���o�b�N %d %d %d %d %d\n",value,pWork->changeMode,targetlv,targetmy,targetfriend);
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
    GTSNEGO_MESSAGE_CancelButtonCreate(pWork->pMessageWork, &_cancelButtonCallback, pWork );
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
      OHNO_Printf("AddMatchKey failed %d.\n",i);
    }
#endif
  }

  //���̏����͊m�� matchkeystr�����킹�ĕύX���鎖 
  STD_TSPrintf( pWork->filter, "ty=%d And db=%d", pWork->changeMode,MATCHINGKEY);

  if( GFL_NET_DWC_StartMatchFilter( pWork->filter, 2 ,&_evalcallback, pWork) != 0){
    GFL_NET_StateStartWifiRandomMatch_Filter();
    GFL_NET_DWC_SetVChat(FALSE);

    GTSNEGO_MESSAGE_CancelButtonCreate(pWork->pMessageWork, &_cancelButtonCallback, pWork );
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
  if(!APP_TASKMENU_WIN_IsFinish(pWork->pAppWin)){
    return;
  }

  APP_TASKMENU_WIN_Delete(pWork->pAppWin);
  pWork->pAppWin = NULL;
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_S );

  GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_019);
  GTSNEGO_DISP_SearchPeopleDispSet(pWork->pDispWork);
  GTSNEGO_MESSAGE_TitleMessage(pWork->pMessageWork,GTSNEGO_018);

 // GTSNEGO_MESSAGE_CancelButtonCreate(pWork->pMessageWork, &_cancelButtonCallback, pWork );

  GTSNEGO_DISP_PaletteFade(pWork->pDispWork, TRUE, _TOUCHBAR_PAL1);

  pWork->dbw->result = EVENT_GTSNEGO_LV;

  
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
  { 24*8-32 , 24*8, 128-48, 128+48},
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

  if(GFL_UI_KEY_GetTrg()){
    if(GFL_UI_CheckTouchOrKey()==GFL_APP_KTST_TOUCH){
      GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, pWork->key2);
      GFL_UI_SetTouchOrKey(GFL_APP_KTST_KEY);
      return;
    }
  }
  
  if(GFL_UI_KEY_GetTrg() == PAD_KEY_LEFT){
    _LevelKeyCallback(FALSE, pWork);
  }
  if(GFL_UI_KEY_GetTrg() == PAD_KEY_RIGHT){
    _LevelKeyCallback(TRUE, pWork);
  }
  
  if(GFL_UI_KEY_GetTrg() == PAD_KEY_UP){
    PMSND_PlaySystemSE(_SE_CUR);
    bHit=TRUE;
    if(pWork->key2 != _CROSSCUR_TYPE_ANY1){
      pWork->key2--;
    }
  }
  if(GFL_UI_KEY_GetTrg() == PAD_KEY_DOWN){
    PMSND_PlaySystemSE(_SE_CUR);
    bHit=TRUE;
    if(pWork->key2 != _CROSSCUR_TYPE_ANY4){
      pWork->key2++;
    }
  }
  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
    if(pWork->key2 == _CROSSCUR_TYPE_ANY4){
      PMSND_PlaySystemSE(_SE_DECIDE);
      GTSNEGO_DISP_CrossIconFlash(pWork->pDispWork , pWork->key2);
      APP_TASKMENU_WIN_SetDecide(pWork->pAppWin,TRUE);
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
    APP_TASKMENU_WIN_SetDecide(pWork->pAppWin,TRUE);
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
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, pWork->pAppWin, _CROSSCUR_TYPE_NONE);
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
  pWork->changeMode = _CHANGEMODE_SELECT_ANYONE;


  GTSNEGO_MESSAGE_DispClear(pWork->pMessageWork);
  GTSNEGO_DISP_LevelInputInit(pWork->pDispWork);
  pWork->touch = &_LevelButtonCallback;

  GTSNEGO_MESSAGE_DispLevel(pWork->pMessageWork, &_BttnCallBack, pWork);

  GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_025);

  GF_ASSERT(pWork->pAppWin==NULL);

  pWork->pAppWin = GTSNEGO_MESSAGE_SearchButtonStart(pWork->pMessageWork,GTSNEGO_023);

  if(GFL_UI_CheckTouchOrKey()==GFL_APP_KTST_KEY){
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, pWork->key2);
    if(pWork->key2 == _CROSSCUR_TYPE_ANY4){
      APP_TASKMENU_WIN_SetActive( pWork->pAppWin , TRUE);
    }
  }
  else{
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, _CROSSCUR_TYPE_NONE);
  }

  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);

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

  count = WIFI_NEGOTIATION_SV_GetCount(GAMEDATA_GetWifiNegotiation(pGameData));
  if((index >= WIFI_NEGOTIATION_DATAMAX) || (index < 0)){
    return NULL;
  }

  j = count - 1 - index;
  if(j<0){
    j+=WIFI_NEGOTIATION_DATAMAX;
  }
  j = j % WIFI_NEGOTIATION_DATAMAX;
  pMyStatus = WIFI_NEGOTIATION_SV_GetMyStatus(GAMEDATA_GetWifiNegotiation(pGameData), j);
  return pMyStatus;
}



//----------------------------------------------------------------------------
/**
 *	@brief	���Ԃ�MYSTATUS�𓾂�  �A�C�R����������ɂȂ��Ă���
 *	@param	POKEMON_TRADE_WORK
 *	@return	none
 */
//-----------------------------------------------------------------------------

static MYSTATUS* GTSNEGO_GetMyStatusIconOnly( GAMEDATA* pGameData, int index)
{
  int i, j, count;
  MYSTATUS* pMyStatus;

  count = WIFI_NEGOTIATION_SV_GetCount(GAMEDATA_GetWifiNegotiation(pGameData));
//  if((index >= WIFI_NEGOTIATION_DATAMAX) || (index < 0)){
//    return NULL;
//  }
  if(index < 0){
    index = 0;
  }
  if(index >= count){
    index = count-1;
  }
  

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
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
      GFL_NET_StateWifiMatchEnd(TRUE);
      GTSNEGO_DISP_ResetDispSet(pWork->pDispWork);
      GTSNEGO_MESSAGE_ResetDispSet(pWork->pMessageWork);
      GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, _CROSSCUR_TYPE_NONE);
      
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

//static void _MatchingCancelState(GTSNEGO_WORK* pWork)
//{
//  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
//    return;
//  }
//  TOUCHBAR_SetVisible(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork), TOUCHBAR_ICON_RETURN, FALSE);
//  pWork->pAppTask = GTSNEGO_MESSAGE_YesNoStart(pWork->pMessageWork, GTSNEGO_YESNOTYPE_INFO);
//  _CHANGE_STATE(pWork,_MatchingCancelState2);
//}


static void _cancelFlash3(GTSNEGO_WORK* pWork)
{
  _modeSelectMenuInit(pWork);
//  GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ, TRUE);
  GFL_DISP_GXS_SetVisibleControlDirect(GX_PLANEMASK_BG0|GX_PLANEMASK_BG1|GX_PLANEMASK_BG2|GX_PLANEMASK_BG3|GX_PLANEMASK_OBJ);
}


static void _cancelFlash2(GTSNEGO_WORK* pWork)
{
  GTSNEGO_DISP_ResetDispSet(pWork->pDispWork);
  GTSNEGO_MESSAGE_ResetDispSet(pWork->pMessageWork);
  _CHANGE_STATE(pWork,_cancelFlash3);
}



static void _cancelFlash(GTSNEGO_WORK* pWork)
{
  if(!GTSNEGO_MESSAGE_CancelButtonDelete(pWork->pMessageWork)){
    return;
  }
//  GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ, FALSE);
  GFL_DISP_GXS_SetVisibleControlDirect(GX_PLANEMASK_BG3);
  GTSNEGO_MESSAGE_DeleteDispLevel(pWork->pMessageWork);
  GTSNEGO_DISP_LevelInputFree(pWork->pDispWork);
  GTSNEGO_DISP_PaletteFade(pWork->pDispWork, FALSE, _TOUCHBAR_PAL1);
  
  GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, _CROSSCUR_TYPE_NONE);
  GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_S );

  if(pWork->changeMode == _CHANGEMODE_SELECT_FRIEND){
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
    GTSNEGO_DISP_FriendSelectFree2(pWork->pDispWork);
  }

  _CHANGE_STATE(pWork,_cancelFlash2);
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

      GTSNEGO_MESSAGE_CancelButtonDecide(pWork->pMessageWork);
      GFL_NET_StateWifiMatchEnd(TRUE);
      PMSND_PlaySystemSE(_SE_CANCEL);
      _CHANGE_STATE(pWork,_cancelFlash);
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
      pWork->chageLevel=0;
      pWork->myChageType=0;
      pWork->friendChageType=0;
      pWork->dbw->result = EVENT_GTSNEGO_FRIEND;
      _CHANGE_STATE(pWork,_matchKeyMake);
      break;
    case 1:
        GTSNEGO_DISP_PaletteFade(pWork->pDispWork, FALSE, _TOUCHBAR_PAL1);

      GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_024);
      if(GFL_UI_CheckTouchOrKey()==GFL_APP_KTST_KEY){
        GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, pWork->key3);
      }
      else{
        GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, _CROSSCUR_TYPE_NONE);
      }
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
  GTSNEGO_DISP_PaletteFade(pWork->pDispWork, TRUE, _TOUCHBAR_PAL2);
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
}


static void _friendSelectBack( GTSNEGO_WORK *pWork )
{
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
  GTSNEGO_DISP_FriendSelectFree2(pWork->pDispWork);
  _CHANGE_STATE(pWork,_modeSelectMenuInit);
}



static int _upScrollFunc(GTSNEGO_WORK *pWork)
{
  int retcode = GTSNEGO_DISP_FriendListUpChk(pWork->pDispWork, &pWork->scrollPanelCursor);

  switch(retcode){
  case 0:
    break;
  case 1:
    if(pWork->key3 != _CROSSCUR_TYPE_FRIEND1){
      pWork->key3--;
    }
    break;
  case 2:
    GTSNEGO_DISP_PanelScrollStart(pWork->pDispWork,PANEL_UPSCROLL_);
    if(pWork->scrollPanelCursor.oamlistpos - 1 >= 0){
      GTSNEGO_MESSAGE_FriendListUpStart(pWork->pMessageWork, pWork->pGameData,
                                        pWork->scrollPanelCursor.oamlistpos-1 );
    }
    break;
  }
  return retcode;
}

static int _downScrollFunc(GTSNEGO_WORK *pWork)
{
  int retcode = GTSNEGO_DISP_FriendListDownChk(pWork->pDispWork, &pWork->scrollPanelCursor);
  
  switch( retcode ){
  case 0:
    break;
  case 1:
    // �J�[�\����������
    if(pWork->key3 != _CROSSCUR_TYPE_FRIEND3){
      pWork->key3++;
    }
    break;
  case 2:
    // �����S�̂̃X�N���[��
    GTSNEGO_DISP_PanelScrollStart(pWork->pDispWork,PANEL_DOWNSCROLL_);
    GTSNEGO_MESSAGE_FriendListDownStart(pWork->pMessageWork, pWork->pGameData,
                                        pWork->scrollPanelCursor.oamlistpos + 2);
    break;
  }
  return retcode;
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

    OHNO_Printf("GTSNEGO_DISP_UnionListUp %d\n",pWork->scrollPanelCursor.oamlistpos-2);

    GTSNEGO_DISP_UnionListUp(pWork->pDispWork, GTSNEGO_GetMyStatusIconOnly(pWork->pGameData ,pWork->scrollPanelCursor.oamlistpos-2));
  }
  else if( PANEL_DOWNSCROLL_ == scrollType){
    GTSNEGO_DISP_FriendSelectPlateView(pWork->pDispWork,pWork->pGameData, pWork->scrollPanelCursor.oamlistpos-2);
    GTSNEGO_MESSAGE_FriendListDownEnd(pWork->pMessageWork);
    OHNO_Printf("GTSNEGO_DISP_UnionListDown %d\n",pWork->scrollPanelCursor.oamlistpos + (SCROLL_PANEL_NUM-3));

    GTSNEGO_DISP_UnionListDown(pWork->pDispWork, GTSNEGO_GetMyStatusIconOnly(pWork->pGameData ,pWork->scrollPanelCursor.oamlistpos + (SCROLL_PANEL_NUM-3)));
  }
  if(ret != PANEL_NONESCROLL_){
    return;
  }

  if(GFL_UI_CheckTouchOrKey() == GFL_APP_KTST_TOUCH){
    if(GFL_UI_KEY_GetTrg()){
      GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, pWork->key3);
      GFL_UI_SetTouchOrKey(GFL_APP_KTST_KEY);
      return;
    }
  }

  if(GFL_UI_KEY_GetRepeat() == PAD_BUTTON_R){
    int i,ret;
    bHit = FALSE;
    
    for(i = 0; i < 3; i++){
      ret = _downScrollFunc(pWork);
      bHit |= ret;
      GTSNEGO_DISP_PanelScrollCancel(pWork->pDispWork);
      if(ret == 0){
        break;
      }
      if(ret == 1){
        i--;
        continue;
      }
      GTSNEGO_DISP_PanelScrollAdjust(pWork->pDispWork,TRUE);
      GTSNEGO_DISP_FriendSelectPlateView(pWork->pDispWork,pWork->pGameData, pWork->scrollPanelCursor.oamlistpos-2);
      GTSNEGO_MESSAGE_FriendListDownEnd(pWork->pMessageWork);
      OHNO_Printf("PAD_BUTTON_R %d\n",pWork->scrollPanelCursor.oamlistpos + (SCROLL_PANEL_NUM-3));
      GTSNEGO_DISP_UnionListDown(pWork->pDispWork, GTSNEGO_GetMyStatusIconOnly(pWork->pGameData ,pWork->scrollPanelCursor.oamlistpos + (SCROLL_PANEL_NUM-3)));
      GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, NULL, pWork->key3);
    }
  }
  if(GFL_UI_KEY_GetRepeat() == PAD_BUTTON_L){
    int i,ret;
    bHit = FALSE;
    for(i = 0; i < 3; i++){
      ret = _upScrollFunc(pWork);
      bHit |= ret;
      GTSNEGO_DISP_PanelScrollCancel(pWork->pDispWork);
      if(ret == 0){
        break;
      }
      if(ret == 1){
        i--;
        continue;
      }

      GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, NULL, pWork->key3);
      {
        GTSNEGO_DISP_PanelScrollAdjust(pWork->pDispWork,FALSE);
        GTSNEGO_DISP_FriendSelectPlateView(pWork->pDispWork,pWork->pGameData, pWork->scrollPanelCursor.oamlistpos-2);
        GTSNEGO_MESSAGE_FriendListUpEnd(pWork->pMessageWork);
        OHNO_Printf("PAD_BUTTON_L %d\n",pWork->scrollPanelCursor.oamlistpos-2);
        GTSNEGO_DISP_UnionListUp(pWork->pDispWork, GTSNEGO_GetMyStatusIconOnly(pWork->pGameData ,pWork->scrollPanelCursor.oamlistpos-2));
      }
    }
  }

  if(GFL_UI_KEY_GetRepeat() == PAD_KEY_UP){
    bHit = _upScrollFunc(pWork);
  }
  if(GFL_UI_KEY_GetRepeat() == PAD_KEY_DOWN){
    bHit = _downScrollFunc(pWork);
  }
  if(bHit){
    PMSND_PlaySystemSE(_SE_CUR);
    GTSNEGO_DISP_ScrollChipDisp(pWork->pDispWork,pWork->scrollPanelCursor.oamlistpos + pWork->key3 - _CROSSCUR_TYPE_FRIEND1,
                                pWork->scrollPanelCursor.listmax );
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, NULL, pWork->key3);
  }

  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){  // �L�[�ł̌���
    PMSND_PlaySystemSE(_SE_DECIDE);

    GTSNEGO_DISP_CrossIconFlash(pWork->pDispWork , pWork->key3);

    pWork->selectFriendIndex = pWork->scrollPanelCursor.oamlistpos + pWork->key3  - _CROSSCUR_TYPE_FRIEND1;
    OHNO_Printf("�I�񂾔ԍ� %d\n", pWork->selectFriendIndex);
    _CHANGE_STATE(pWork,_friendSelectDecide);
    return;
  }


  {  //TP�ŒN��I�񂾂̂�
    int trgindex=GFL_UI_TP_HitTrg(_tp_data2);
    switch(trgindex){
    case 2:
      if(pWork->scrollPanelCursor.listmax < 3){
        break;
      }
      //break throw
    case 1:
      if(pWork->scrollPanelCursor.listmax < 2){
        break;
      }
      //break throw
    case 0:

      GFL_UI_SetTouchOrKey(GFL_APP_KTST_TOUCH);
      
      pWork->key3 = trgindex  + _CROSSCUR_TYPE_FRIEND1;
      GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, NULL, pWork->key3);
      GTSNEGO_DISP_CrossIconFlash(pWork->pDispWork , pWork->key3);

      pWork->selectFriendIndex = pWork->scrollPanelCursor.oamlistpos + pWork->key3  - _CROSSCUR_TYPE_FRIEND1;
      OHNO_Printf("�I�񂾔ԍ� %d\n", pWork->selectFriendIndex);
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
        PMSND_PlaySystemSE(_SE_CUR);
      }
    }
    return;
  }
  TOUCHBAR_Main(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork));
  switch( TOUCHBAR_GetTrg(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork))){
  case TOUCHBAR_ICON_RETURN:
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_OFF );
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, pWork->pAppWin, _CROSSCUR_TYPE_NONE);
    GTSNEGO_DISP_FriendSelectFree(pWork->pDispWork);
    GTSNEGO_MESSAGE_DispClear(pWork->pMessageWork);
    _CHANGE_STATE(pWork,_friendSelectBack);
    break;
  default:
    break;
  }

}

//------------------------------------------------------------------
/**
 * $brief   �������肢�Ȃ��̂ł��Ƃɂ��ǂ�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static void _friendSelectCancel( GTSNEGO_WORK *pWork )
{
  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  pWork->timer--;
  if(pWork->timer <= 0){
    GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_037);
    _CHANGE_STATE(pWork,_modeSelectMenuWait);
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

  pWork->changeMode = _CHANGEMODE_SELECT_FRIEND;

  
  
  if(a==0){
    GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_034);
    pWork->timer = 30*2;
    _CHANGE_STATE(pWork,_friendSelectCancel);
    return;
  }
  pWork->scrollPanelCursor.listmax = a;

  GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_024);

  if(GFL_UI_CheckTouchOrKey()==GFL_APP_KTST_KEY){
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, pWork->key3);
  }
  else{
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, _CROSSCUR_TYPE_NONE);
  }

  GTSNEGO_MESSAGE_DispClear(pWork->pMessageWork);

  if(a < 3){
    bCursor = FALSE;
  }
  
  GTSNEGO_DISP_FriendSelectInit(pWork->pDispWork, bCursor);
  
  GTSNEGO_MESSAGE_FriendListPlateDisp(pWork->pMessageWork,pWork->pGameData);


  GTSNEGO_MESSAGE_FriendListRenew(pWork->pMessageWork, pWork->pGameData,
                                  pWork->scrollPanelCursor.oamlistpos-2 );
  GTSNEGO_DISP_UnionListRenew(pWork->pDispWork, pWork->pGameData,
                              pWork->scrollPanelCursor.oamlistpos-2 );
  GTSNEGO_DISP_FriendSelectPlateView(pWork->pDispWork,pWork->pGameData,
                                     pWork->scrollPanelCursor.oamlistpos-2);
  
//  for(i=2;i<SCROLL_PANEL_NUM;i++){
//    MYSTATUS* pMy = GTSNEGO_GetMyStatus(pWork->pGameData, i-2);
//    GTSNEGO_DISP_UnionListDisp(pWork->pDispWork, pMy, i);
//  }
//  GTSNEGO_DISP_UnionListRenew(pWork->pDispWork,pWork->pGameData, -2);

  //GTSNEGO_DISP_FriendSelectPlateView(pWork->pDispWork,pWork->pGameData, -2);
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



static void _modeSelectEnd(GTSNEGO_WORK* pWork)
{
  if(GFL_UI_KEY_GetTrg() || GFL_UI_TP_GetTrg()){
    GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_047);
    if(GFL_UI_CheckTouchOrKey()==GFL_APP_KTST_TOUCH){
      GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, _CROSSCUR_TYPE_NONE);
    }
    _CHANGE_STATE(pWork, _messageEndCheck);
  }
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

  if(GFL_UI_CheckTouchOrKey()==GFL_APP_KTST_KEY){
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, pWork->key1);
  }
  else{
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, _CROSSCUR_TYPE_NONE);
  }
  GTSNEGO_MESSAGE_DispAnyoneOrFriend(pWork->pMessageWork, &_BttnCallBack, pWork);
  pWork->touch = &_AnyoneOrFriendButtonCallback;

  {
    POKEPARTY* party = GAMEDATA_GetMyPokemon(pWork->pGameData);
    if(PokeParty_GetPokeCount(party) < 2 && 0 == BOXDAT_GetPokeExistCountTotal(GAMEDATA_GetBoxManager(pWork->pGameData))){
      pWork->bSingle = TRUE;
    }
  }

  if(!pWork->bSingle){
    GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_037);
  }
  else{
    GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_048);
  }
  GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_ON );
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
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, pWork->pAppWin, _CROSSCUR_TYPE_NONE);

    GTSNEGO_MESSAGE_DispClear(pWork->pMessageWork);
    GFL_BG_LoadScreenV_Req(GFL_BG_FRAME2_S);
    _CHANGE_STATE(pWork,_levelSelect);
  }
  else if(buttonNo==_CROSSCUR_TYPE_MAINDOWN){
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork, pWork->pAppWin, _CROSSCUR_TYPE_NONE);
    _CHANGE_STATE(pWork,_friendSelect);
  }
}

static void _messageEndCheck2(GTSNEGO_WORK* pWork)
{
  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    GTSNEGO_MESSAGE_AppMenuClose(pWork->pAppTask);
    pWork->pAppTask=NULL;
    switch(selectno){
    case 0:
      pWork->dbw->result = EVENT_GTSNEGO_EXIT;
      pWork->timer=0;
      _CHANGE_STATE(pWork,_messageEnd);
      break;
    case 1:

      if(!pWork->bSingle){
        GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_037);
      }
      else{
        GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_048);
      }
      TOUCHBAR_SetVisible(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork), TOUCHBAR_ICON_RETURN, TRUE);
      GTSNEGO_DISP_PaletteFade(pWork->pDispWork, FALSE, _TOUCHBAR_PAL1);
      _CHANGE_STATE(pWork,_modeSelectMenuWait);
      break;
    }
  }
}

static void _messageEndCheck(GTSNEGO_WORK* pWork)
{
  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  TOUCHBAR_SetVisible(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork), TOUCHBAR_ICON_RETURN, FALSE);
  pWork->pAppTask = GTSNEGO_MESSAGE_YesNoStart(pWork->pMessageWork, GTSNEGO_YESNOTYPE_SYS);

  GTSNEGO_DISP_PaletteFade(pWork->pDispWork, TRUE, _TOUCHBAR_PAL1);

  _CHANGE_STATE(pWork,_messageEndCheck2);
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

  if(GFL_UI_KEY_GetTrg()){
    if(GFL_UI_CheckTouchOrKey()==GFL_APP_KTST_TOUCH){
      GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, pWork->key1);
      GFL_UI_SetTouchOrKey(GFL_APP_KTST_KEY);
      pWork->keyMode=TRUE;
      return;
    }
  }

  if(GFL_UI_KEY_GetTrg() == PAD_BUTTON_DECIDE){
    if(!pWork->bSingle){
      _buttonDecide(pWork, pWork->key1);
      _CHANGE_STATE(pWork,_modeSelectMenuFlash);
      return;
    }
    else{
      PMSND_PlaySystemSE(_SE_CANCEL);
    }
  }
  
  if(GFL_UI_KEY_GetTrg() == PAD_KEY_UP){
    bHit=TRUE;
    PMSND_PlaySystemSE(_SE_CUR);
    pWork->key1=_CROSSCUR_TYPE_MAINUP;
  }
  if(GFL_UI_KEY_GetTrg() == PAD_KEY_DOWN){
    bHit=TRUE;
    PMSND_PlaySystemSE(_SE_CUR);
    pWork->key1=_CROSSCUR_TYPE_MAINDOWN;
  }
  if(bHit){
    GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, pWork->key1);
  }
  
  GTSNEGO_MESSAGE_ButtonWindowMain(pWork->pMessageWork);
  
  TOUCHBAR_Main(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork));
  switch( TOUCHBAR_GetTrg(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork))){
  case TOUCHBAR_ICON_RETURN:
    pWork->timer=1;
    GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_047);
    if(GFL_UI_CheckTouchOrKey()==GFL_APP_KTST_TOUCH){
      GTSNEGO_DISP_CrossIconDisp(pWork->pDispWork,NULL, _CROSSCUR_TYPE_NONE);
    }
    _CHANGE_STATE(pWork, _messageEndCheck);
    break;
  default:
    break;
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   �����}�b�`���O���Ă�Ȃ�K�؂ȏ��������ăg�b�v�ɖ߂�
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _checkReturnState(GTSNEGO_WORK* pWork)
{
  if(pWork->dbw->result != EVENT_GTSNEGO_EXIT){
    GFL_NET_SetAutoErrorCheck(FALSE);
    GFL_NET_SetNoChildErrorCheck(FALSE);
    GFL_NET_StateWifiMatchEnd(TRUE);
pWork->dbw->result=EVENT_GTSNEGO_EXIT;
    _CHANGE_STATE(pWork,_modeSelectMenuInit);
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
  pWork->dbw->result = EVENT_GTSNEGO_EXIT;
  pWork->key1=_CROSSCUR_TYPE_MAINUP;
  pWork->key2=_CROSSCUR_TYPE_ANY1;
  pWork->key3=_CROSSCUR_TYPE_FRIEND1;

  pWork->profileID = MyStatus_GetProfileID( GAMEDATA_GetMyStatus(pEv->gamedata) );
  
  if(GFL_NET_IsInit()){
    GFL_NET_AddCommandTable(GFL_NET_CMD_GTSNEGO,_PacketTbl,NELEMS(_PacketTbl), pWork);
  }


  WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                  WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );
  PMSND_PlayBGM(SEQ_BGM_GTS);

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

  if(GFL_NET_IsInit()){
    if(NET_ERR_CHECK_NONE != NetErr_App_CheckError()){

      GFL_NET_DWC_ERROR_RESULT  result;
      result  = GFL_NET_DWC_ERROR_ReqErrorDisp( TRUE );
      switch( result ){
      case GFL_NET_DWC_ERROR_RESULT_PRINT_MSG:   //���b�Z�[�W��`�悷�邾��
        _checkReturnState(pWork);
        break;
      case GFL_NET_DWC_ERROR_RESULT_RETURN_PROC: //PROC���甲���Ȃ���΂Ȃ�Ȃ�
        pWork->dbw->result = EVENT_GTSNEGO_ERROR;
        retCode = GFL_PROC_RES_FINISH;
        WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
        WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
        break;
      case GFL_NET_DWC_ERROR_RESULT_FATAL:       //�d���ؒf�̂��ߖ������[�v�ɂȂ�
        NetErr_App_FatalDispCall();
        break;
      }
    }
  }
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

  if(!WIPE_SYS_EndCheck()){
    return GFL_PROC_RES_CONTINUE;
  }
  
  if(pWork->pAppWin){
    APP_TASKMENU_WIN_Delete(pWork->pAppWin);
  }
  if(pWork->pAppTask){
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
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


