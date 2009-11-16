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

#define _WORK_HEAPSIZE (0x1000)  // �������K�v


//--------------------------------------------
// ��ʍ\����`
//--------------------------------------------

//BG�ʂƃp���b�g�ԍ�(���ݒ�
#define _SUBSCREEN_BGPLANE	(GFL_BG_FRAME3_S)
#define _SUBSCREEN_PALLET	(0xE)



//#define	_BUTTON_WIN_CGX_SIZE   ( 18+12 )
//#define	_BUTTON_WIN_CGX	( 2 )


//#define	_BUTTON_FRAME_CGX		( _BUTTON_WIN_CGX + ( 23 * 16 ) )	// �ʐM�V�X�e���E�B���h�E�]����








//--------------------------------------------
// �������[�N
//--------------------------------------------


enum _IBMODE_SELECT {
  _SELECTMODE_GSYNC = 0,
  _SELECTMODE_UTIL,
  _SELECTMODE_EXIT
};




typedef void (StateFunc)(GTSNEGO_WORK* pState);
typedef BOOL (TouchFunc)(int no, GTSNEGO_WORK* pState);


struct _GTSNEGO_WORK {
  StateFunc* state;      ///< �n���h���̃v���O�������
  TouchFunc* touch;
  int selectType;   // �ڑ��^�C�v
  HEAPID heapID;

  APP_TASKMENU_WORK* pAppTask;
  GTSNEGO_DISP_WORK* pDispWork;  // �`��n
  GTSNEGO_MESSAGE_WORK* pMessageWork; //���b�Z�[�W�n
  WIFI_LIST* pList;
  EVENT_GTSNEGO_WORK * dbw;  //�e�̃��[�N
  BOOL IsIrc;
  GAMESYS_WORK *gameSys_;
  FIELDMAP_WORK *fieldWork_;
  GMEVENT* event_;
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


//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u�m�F��ʑҋ@
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeProfileWait(GTSNEGO_WORK* pWork)
{
  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  pWork->pAppTask = GTSNEGO_MESSAGE_YesNoStart(pWork->pMessageWork);
  _CHANGE_STATE(pWork,_modeReportWait2);
}


//------------------------------------------------------------------------------
/**
 * @brief   �ŏ��̐ڑ��̃v���t�@�C������
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _profileIDCheck(GTSNEGO_WORK* pWork)
{
  // ���߂Ă̏ꍇ
  if( !DWC_CheckHasProfile( WifiList_GetMyUserInfo(pWork->pList) ) ){


  }
  else if( !DWC_CheckValidConsole(WifiList_GetMyUserInfo(pWork->pList)) ){  //��DS�̏ꍇ
  }
  else  //���ʂ̐ڑ�
  {

    GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork, GTSNEGO_001);
    _CHANGE_STATE(pWork,_modeProfileWait);
  }

}


static void _modeSelectMenuInit(GTSNEGO_WORK* pWork)
{ 
  int aMsgBuff[]={GTSNEGO_005,GTSNEGO_006};

  GTSNEGO_MESSAGE_ButtonWindowCreate(NELEMS(aMsgBuff), aMsgBuff, pWork->pMessageWork, _BttnCallBack, pWork);

	pWork->touch = &_modeSelectMenuButtonCallback;

	_CHANGE_STATE(pWork,_modeSelectMenuWait);

}

//------------------------------------------------------------------------------
/**
 * @brief   �t�F�[�h�A�E�g����
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _modeFadeout(GTSNEGO_WORK* pWork)
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
static BOOL _modeSelectMenuButtonCallback(int bttnid,GTSNEGO_WORK* pWork)
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
static void _modeSelectMenuWait(GTSNEGO_WORK* pWork)
{
	if(WIPE_SYS_EndCheck()){
		GTSNEGO_MESSAGE_ButtonWindowMain( pWork->pMessageWork );
	}
}


//------------------------------------------------------------------------------
/**
 * @brief   �Z�[�u�m�F��ʏ�����
 * @retval  none
 */
//------------------------------------------------------------------------------
static void _modeReportInit(GTSNEGO_WORK* pWork)
{

  //    GAMEDATA_Save(GAMESYSTEM_GetGameData(GMEVENT_GetGameSysWork(event)));

  GFL_BG_ClearScreenCodeVReq(GFL_BG_FRAME1_S,0);
  
 // GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GAMESYNC_004);

  
  _CHANGE_STATE(pWork,_modeReportWait);
}


static void _FadeWait(GTSNEGO_WORK* pWork)
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
static void _modeReporting(GTSNEGO_WORK* pWork)
{

  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }


  {
    SAVE_RESULT svr = SaveControl_SaveAsyncMain(pWork->dbw->ctrl);

    if(svr == SAVE_RESULT_OK){
      GTSNEGO_MESSAGE_InfoMessageEnd(pWork->pMessageWork);

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
static void _modeReportWait2(GTSNEGO_WORK* pWork)
{

  if(APP_TASKMENU_IsFinish(pWork->pAppTask)){
    int selectno = APP_TASKMENU_GetCursorPos(pWork->pAppTask);

    if(selectno==0){


//      GTSNEGO_MESSAGE_InfoMessageDisp(pWork->pMessageWork,GAMESYNC_007);

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
static void _modeReportWait(GTSNEGO_WORK* pWork)
{
  if(!GTSNEGO_MESSAGE_InfoMessageEndCheck(pWork->pMessageWork)){
    return;
  }
  pWork->pAppTask = GTSNEGO_MESSAGE_YesNoStart(pWork->pMessageWork);
  _CHANGE_STATE(pWork,_modeReportWait2);
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
	
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCBATTLE, 0x18000 );

  {
    GTSNEGO_WORK *pWork = GFL_PROC_AllocWork( proc, sizeof( GTSNEGO_WORK ), HEAPID_IRCBATTLE );
    GFL_STD_MemClear(pWork, sizeof(GTSNEGO_WORK));
    pWork->heapID = HEAPID_IRCBATTLE;

    pWork->pDispWork = GTSNEGO_DISP_Init(pWork->heapID);
    pWork->pMessageWork = GTSNEGO_MESSAGE_Init(pWork->heapID);

    pWork->pList = SaveData_GetWifiListData(
      GAMEDATA_GetSaveControlWork(
        GAMESYSTEM_GetGameData(
          ((pEv->gsys))) ));
    pWork->IsIrc=FALSE;


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

  
	INFOWIN_Update();

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

//  EVENT_IrcBattleSetType(pParentWork, pWork->selectType);

  GFL_PROC_FreeWork(proc);

	INFOWIN_Exit();
	GFL_BG_FreeBGControl(_SUBSCREEN_BGPLANE);


  GTSNEGO_MESSAGE_End(pWork->pMessageWork);
  GTSNEGO_DISP_End(pWork->pDispWork);

	GFL_HEAP_DeleteHeap(HEAPID_IRCBATTLE);
  //EVENT_IrcBattle_SetEnd(pParentWork);


	
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


