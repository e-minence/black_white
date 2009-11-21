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

#include "msg/msg_gtsnego.h"
#include "../../field/event_gtsnego.h"
#include "gtsnego.naix"
#include "app/app_taskmenu.h"  //APP_TASKMENU_INITWORK
#include "gtsnego_local.h"


#if PM_DEBUG
#define _NET_DEBUG (1)  //�f�o�b�O���͂P
#else
#define _NET_DEBUG (0)  //�f�o�b�O���͂P
#endif

#define _NO2   (2)

#define _WORK_HEAPSIZE (0x1000)  // �������K�v


//--------------------------------------------
// ��ʍ\����`
//--------------------------------------------

//BG�ʂƃp���b�g�ԍ�(���ݒ�
#define _SUBSCREEN_BGPLANE	(GFL_BG_FRAME3_S)
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
  _MATCHKEY_PROFILE1,   //Profile1
  _MATCHKEY_PROFILE2,   //Profile2
  _MATCHKEY_PROFILE3,   //Profile3
  _MATCHKEY_TYPE,     //�����^�C�v �m�荇�����ǂ���
  _MATCHKEY_LEVEL,    //����LV
  _MATCHKEY_IMAGE_MY,  //�����̊�]
  _MATCHKEY_IMAGE_FRIEND,  //����ɑ΂��Ă̊�]
  _MATCHKEY_ROMCODE,    //ROM�o�[�W����
  _MATCHKEY_DEBUG,    //�f�o�b�O�����i��
  _MATCHKEY_MAX,
};



static char matchkeystr[_MATCHKEY_MAX][2]={"p1","p2","p3","ty","lv","my","fr","rm","db"};


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
  SAVE_CONTROL_WORK* pSave;
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

};



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

static void _levelSelect( GTSNEGO_WORK *pWork );



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


//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʃ^�b�`����
 * @retval  none
 */
//------------------------------------------------------------------------------
static BOOL _AnyoneOrFriendButtonCallback(int bttnid,GTSNEGO_WORK* pWork)
{

  pWork->changeMode = bttnid;
  _CHANGE_STATE(pWork, _levelSelect);
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
  return TRUE;
}


static void _timingCheck( GTSNEGO_WORK *pWork )
{
  if(GFL_NET_HANDLE_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(),_NO2)){
    _CHANGE_STATE(pWork,NULL);
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

  if(pWork->timer>0){
    pWork->timer--;
  }
  else{
    GFL_NET_HANDLE_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_NO2);
    _CHANGE_STATE(pWork,_timingCheck);
  }
}



static void _friendGreeStateParent( GTSNEGO_WORK *pWork )
{
  if(GFL_NET_HANDLE_GetNumNegotiation() != 0){
    if(GFL_NET_HANDLE_RequestNegotiation()){
      _CHANGE_STATE(pWork,_friendGreeState);
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
    pWork->timer = _FRIEND_GREE_DOWN_TIME;
    GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_022);
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
 //�ڑ�������\�����Č�����
  if(STEPMATCH_SUCCESS == GFL_NET_DWC_GetStepMatchResult()){
    
    GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_021);

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
  int value=0;
  int targetlv,targetfriend,targetmy;

  targetlv = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_LEVEL].keyStr,-1);
  targetfriend = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_IMAGE_FRIEND].keyStr,-1);
  targetmy = DWC_GetMatchIntValue(index,pWork->aMatchKey[_MATCHKEY_IMAGE_MY].keyStr,-1);
  
  if(pWork->changeMode==1){//�Ƃ�����
  }
  else{
    
    
    if(pWork->chageLevel==targetlv){
      if(pWork->myChageType==targetfriend){
        value+=10;
      }
      if(pWork->friendChageType==targetmy){
        value+=10;
      }
      //@todo���łɌ������������`�F�b�N
      
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
  GFL_NET_SetWifiBothNet(FALSE);
  //GFL_NET_StateWifiEnterLogin();

  
  GFL_NET_DWC_GetMySendedFriendCode(pWork->pList, (DWCFriendData*)&buff[0]);
  buff[3] = pWork->changeMode;
  buff[4] = pWork->chageLevel;
  buff[5] = pWork->myChageType;
  buff[6] = pWork->friendChageType;
  buff[7] = PM_VERSION + (PM_LANG<<16); 
  buff[8] = MATCHINGKEY;

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
    _CHANGE_STATE(pWork,_matchingState);
  }
  else{
    
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
      if(pWork->touch(bttnid, pWork)){
        return;
      }
    }
    break;
  default:
    break;
  }
}

//------------------------------------------------------------------
/**
 * $brief   ���x���m�F�҂�
 * @param   wk
 * @retval  none
 */
//------------------------------------------------------------------

static void _levelSelectWait( GTSNEGO_WORK *pWork )
{
  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }

  GTSNEGO_MESSAGE_ButtonWindowMain(pWork->pMessageWork);



  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);
    if(selectno==0){
      GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_019);
      
      _CHANGE_STATE(pWork,_matchKeyMake);
    }
    APP_TASKMENU_CloseMenu(pWork->pAppTask);
    pWork->pAppTask=NULL;
  }
  else{
    TOUCHBAR_Main(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork));
    switch( TOUCHBAR_GetTrg(GTSNEGO_DISP_GetTouchWork(pWork->pDispWork))){
    case TOUCHBAR_ICON_RETURN:
      _CHANGE_STATE(pWork,NULL);
      break;
    default:
      break;
    }
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
  
  pWork->pAppTask = GTSNEGO_MESSAGE_SearchButtonStart(pWork->pMessageWork);

  _CHANGE_STATE(pWork,_levelSelectWait);
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
  GTSNEGO_MESSAGE_DispAnyoneOrFriend(pWork->pMessageWork, &_BttnCallBack, pWork);
  pWork->touch = &_AnyoneOrFriendButtonCallback;
  GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GTSNEGO_024);
  _CHANGE_STATE(pWork,_modeSelectMenuWait);
}

//------------------------------------------------------------------------------
/**
 * @brief   ���[�h�Z���N�g��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeSelectMenuWait(GTSNEGO_WORK* pWork)
{

  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
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

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x38000 );

  {
    GTSNEGO_WORK *pWork = GFL_PROC_AllocWork( proc, sizeof( GTSNEGO_WORK ), HEAPID_IRCBATTLE );
    GFL_STD_MemClear(pWork, sizeof(GTSNEGO_WORK));
    pWork->heapID = HEAPID_IRCBATTLE;

    pWork->pDispWork = GTSNEGO_DISP_Init(pWork->heapID);
    pWork->pMessageWork = GTSNEGO_MESSAGE_Init(pWork->heapID, NARC_message_gtsnego_dat);
    pWork->pSave = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(pEv->gsys));
    pWork->pList = SaveData_GetWifiListData(
      GAMEDATA_GetSaveControlWork(
        GAMESYSTEM_GetGameData(
          ((pEv->gsys))) ));


    {
      //			GAME_COMM_SYS_PTR pGC = GAMESYSTEM_GetGameCommSysPtr(pEv->gsys);
      //		INFOWIN_Init( _SUBSCREEN_BGPLANE , _SUBSCREEN_PALLET , pGC , pWork->heapID);
    }



    WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN ,
                    WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , pWork->heapID );

    pWork->dbw = pwk;

    _CHANGE_STATE(pWork,_modeSelectMenuInit);

  }

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


