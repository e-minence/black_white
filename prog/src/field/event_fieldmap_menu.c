//======================================================================
/**
 *
 * @file  event_fieldmap_menu.c
 * @brief �t�B�[���h�}�b�v���j���[�̐���C�x���g
 * @author  kagaya
 * @date  2008.11.13
 *
 * ���W���[�����FFIELD_MENU
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"
#include "arc_def.h"


#include "message.naix"
#include "msg/msg_fldmapmenu.h"
#include "test/easy_pokelist.h"

#include "system/wipe.h"
#include "gamesystem/game_comm.h"
#include "gamesystem/game_data.h"
#include "gamesystem/game_event.h"
#include "field/field_msgbg.h"
#include "field/event_fieldmap_menu.h"
#include "field/fieldmap.h"
#include "field/map_attr.h"

#include "savedata/sp_ribbon_save.h"

#include "event_fieldmap_control.h" //EVENT_FieldSubProc
#include "app/config_panel.h"   //ConfigPanelProcData
#include "app/trainer_card.h"   //TrainerCardSysProcData
#include "app/bag/itemmenu_local.h" //ItemMenuProcData
#include "app/pokelist.h"   //PokeList_ProcData�EPLIST_DATA
#include "app/p_status.h"   //PokeList_ProcData�EPLIST_DATA

extern const GFL_PROC_DATA DebugAriizumiMainProcData;
extern const GFL_PROC_DATA TrainerCardProcData;
FS_EXTERN_OVERLAY(bag);
FS_EXTERN_OVERLAY(poke_status);

//======================================================================
//  define
//======================================================================
#define BGFRAME_MENU  (GFL_BG_FRAME1_M) //�g�p�t���[��
#define MENU_PANO (14)            //���j���[�p���b�gNo
#define FONT_PANO (15)            //�t�H���g�p���b�gNo
#define MENU_CHARSIZE_X (8)         //���j���[����
#define MENU_CHARSIZE_Y (16)        //���j���[�c��

//--------------------------------------------------------------
/// FMENURET
//--------------------------------------------------------------
typedef enum
{
  FMENURET_CONTINUE,
  FMENURET_FINISH,
  FMENURET_NEXTEVENT,
}FMENURET;

typedef enum
{
  FMENUSTATE_INIT,
  FMENUSTATE_MAIN,
  FMENUSTATE_DECIDE_ITEM, //���ڂ����肳�ꂽ
  FMENUSTATE_WAIT_RETURN,
  FMENUSTATE_RETURN_MENU,
  FMENUSTATE_EXIT_MENU,
  
  //�t�B�[���h�o����n
  FMENUSTATE_FIELD_FADEOUT,
  FMENUSTATE_FIELD_CLOSE,
  FMENUSTATE_FIELD_OPEN,
  FMENUSTATE_FIELD_FADEIN,

  //�qProc����
  FMENUSTATE_CALL_SUB_PROC,
  FMENUSTATE_CALL_WAIT_SUB,
  
}FMENU_STATE;

typedef enum
{
  //���j���[����Ă΂�����
  FMENU_APP_POKELIST,
  FMENU_APP_ZUKAN,
  FMENU_APP_BAG,
  FMENU_APP_TRAINERCARD,
  FMENU_APP_CONFIG,
  
  //���j���[����Ă΂����̂���Ă΂�����
  FMENU_APP_STATUS,
  FMENU_APP_TOWNMAP,
  
  FMENU_APP_MAX,
}FMENU_APP_TYPE;

//======================================================================
//  typedef struct
//======================================================================
/// FMENU_EVENT_WORK
typedef struct _TAG_FMENU_EVENT_WORK FMENU_EVENT_WORK;

//--------------------------------------------------------------
/// ���j���[�Ăяo���֐�
/// BOOL TRUE=�C�x���g�p�� FALSE==�t�B�[���h�}�b�v���j���[�C�x���g�I��
//--------------------------------------------------------------
typedef BOOL (* FMENU_CALLPROC)(FMENU_EVENT_WORK*);
typedef const BOOL (* FMENU_RETURN_PROC_FUNC)(FMENU_EVENT_WORK*);

//--------------------------------------------------------------
/// FMENU_EVENT_WORK
//--------------------------------------------------------------
struct _TAG_FMENU_EVENT_WORK
{
  HEAPID heapID;
  GMEVENT *gmEvent;
  GAMESYS_WORK *gmSys;
  FIELD_MAIN_WORK *fieldWork;
  
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
  
  FMENU_STATE state;
  
  FMENU_APP_TYPE subProcType;
  void *subProcWork;
  
  FIELD_SUBSCREEN_MODE return_subscreen_mode;
};


typedef struct
{
  FSOverlayID ovId;
	const GFL_PROC_DATA *procData;
  FMENU_RETURN_PROC_FUNC retFunc;
}FMENU_SUBPROC_DATA;

//======================================================================
//  proto
//======================================================================
static GMEVENT_RESULT FldMapMenuEvent( GMEVENT *event, int *seq, void *wk );

static BOOL FMenuCallProc_Zukan( FMENU_EVENT_WORK *mwk );
static BOOL FMenuCallProc_PokeStatus( FMENU_EVENT_WORK *mwk );
static BOOL FMenuCallProc_Bag( FMENU_EVENT_WORK *mwk );
static BOOL FMenuCallProc_MyTrainerCard( FMENU_EVENT_WORK *mwk );
static BOOL FMenuCallProc_Report( FMENU_EVENT_WORK *mwk );
static BOOL FMenuCallProc_Config( FMENU_EVENT_WORK *mwk );

static void FMenu_SetNextSubProc( FMENU_EVENT_WORK* mwk,FMENU_APP_TYPE type , void *procWork );

static GMEVENT * createFMenuMsgWinEvent(
  GAMESYS_WORK *gsys, u32 heapID, u32 strID, FLDMSGBG *msgBG );
static GMEVENT_RESULT FMenuMsgWinEvent( GMEVENT *event, int *seq, void *wk );

static GMEVENT * createFMenuReportEvent(
  GAMESYS_WORK *gsys, FIELD_MAIN_WORK *fieldWork, u32 heapID, FLDMSGBG *msgBG );
static GMEVENT_RESULT FMenuReportEvent( GMEVENT *event, int *seq, void *wk );

static const BOOL FMenuReturnProc_PokeList(FMENU_EVENT_WORK* mwk);
static const BOOL FMenuReturnProc_PokeStatus(FMENU_EVENT_WORK* mwk);
static const BOOL FMenuReturnProc_Bag(FMENU_EVENT_WORK* mwk);

//--------------------------------------------------------------
/// �t�B�[���h�}�b�v���j���[���X�g
//--------------------------------------------------------------
static const FMENU_CALLPROC FldMapMenu_CallProcList[FMIT_MAX] =
{
  { NULL },
  { FMenuCallProc_PokeStatus },
  { FMenuCallProc_Zukan },
  { FMenuCallProc_Bag },
  { FMenuCallProc_MyTrainerCard },
  { FMenuCallProc_Report },
  { FMenuCallProc_Config },
};

//--------------------------------------------------------------
/// �qProc�f�[�^�e�[�u��
//--------------------------------------------------------------

static const FMENU_SUBPROC_DATA FldMapMenu_SubProcData[FMENU_APP_MAX] =
{
  { //  FMENU_APP_POKELIST,
    FS_OVERLAY_ID(pokelist) , 
    &PokeList_ProcData ,
    FMenuReturnProc_PokeList 
  },
  { //  FMENU_APP_ZUKAN,
    0 , NULL , NULL
  },
  { //  FMENU_APP_BAG,
    FS_OVERLAY_ID(bag) , 
    &ItemMenuProcData  ,
    FMenuReturnProc_Bag
  },
  { //  FMENU_APP_TRAINERCARD,
    TRCARD_OVERLAY_ID , 
    &TrCardSysProcData ,
    NULL 
  },
  { //  FMENU_APP_CONFIG,
    FS_OVERLAY_ID(config_panel) , 
    &ConfigPanelProcData ,
    NULL
  },
  
  //���Ăт�������
  { //  FMENU_APP_STATUS,
    FS_OVERLAY_ID(poke_status) , 
    &PokeStatus_ProcData ,
    FMenuReturnProc_PokeStatus
  },
  { //  FMENU_APP_TOWNMAP,
    0 , NULL , NULL
  },
};

//======================================================================
//  �C�x���g�F�t�B�[���h�}�b�v���j���[
//======================================================================
//--------------------------------------------------------------
/**
 * �t�B�[���h�}�b�v���j���[�C�x���g�N��
 * @param gsys  GAMESYS_WORK
 * @param fieldWork FIELD_MAIN_WORK
 * @param heapID  HEAPID
 * @retval  GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_FieldMapMenu(
  GAMESYS_WORK *gsys, FIELD_MAIN_WORK *fieldWork, HEAPID heapID )
{
  FMENU_EVENT_WORK *mwk;
  GMEVENT * event;
  event = GMEVENT_Create(
    gsys, NULL, FldMapMenuEvent, sizeof(FMENU_EVENT_WORK));
  
  mwk = GMEVENT_GetEventWork(event);
  MI_CpuClear8( mwk, sizeof(FMENU_EVENT_WORK) );
  
  mwk->gmSys = gsys;
  mwk->gmEvent = event;
  mwk->fieldWork = fieldWork;
  mwk->heapID = heapID;
  mwk->state = FMENUSTATE_INIT;
  mwk->subProcWork = NULL;
  mwk->return_subscreen_mode = FIELD_SUBSCREEN_NORMAL;
  
  return event;
}

//--------------------------------------------------------------
/**
 * ���j�I�����[���p�t�B�[���h�}�b�v���j���[�C�x���g�N��
 * @param gsys  GAMESYS_WORK
 * @param fieldWork FIELD_MAIN_WORK
 * @param heapID  HEAPID
 * @retval  GMEVENT*
 */
//--------------------------------------------------------------
GMEVENT * EVENT_UnionMapMenu(
  GAMESYS_WORK *gsys, FIELD_MAIN_WORK *fieldWork, HEAPID heapID )
{
  FMENU_EVENT_WORK *mwk;
  GMEVENT * event;
  
  event = EVENT_FieldMapMenu(gsys, fieldWork, heapID);
  
  mwk = GMEVENT_GetEventWork(event);
  mwk->return_subscreen_mode = FIELD_SUBSCREEN_UNION;

  return event;
}

//--------------------------------------------------------------
/**
 * �C�x���g�F�t�B�[���h�t�B�[���h�}�b�v���j���[
 * @param event GMEVENT
 * @param seq   �V�[�P���X
 * @param wk    event work
 * @retval  GMEVENT_RESULT
 */
//--------------------------------------------------------------
static GMEVENT_RESULT FldMapMenuEvent( GMEVENT *event, int *seq, void *wk )
{
  FMENU_EVENT_WORK *mwk = wk;
  switch (mwk->state) 
  {
  case FMENUSTATE_INIT:
    if( FIELD_SUBSCREEN_CanChange( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) ) == TRUE )
    {
      MMDLSYS *fldMdlSys = FIELDMAP_GetMMdlSys( mwk->fieldWork );
      GAMESYS_WORK *gameSys = GMEVENT_GetGameSysWork( event );
      GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
      MMDLSYS_PauseMoveProc( fldMdlSys );
      GAMEDATA_SetSubScreenType( gameData , FMIT_POKEMON );
      FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), FIELD_SUBSCREEN_TOPMENU);
      mwk->state = FMENUSTATE_MAIN;
    }
    break;
  case FMENUSTATE_MAIN:
    {
      const FIELD_SUBSCREEN_ACTION action = FIELD_SUBSCREEN_GetAction(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork));
      if( action == FIELD_SUBSCREEN_ACTION_TOPMENU_DECIDE )
      {
        //�������ڂ����肳�ꂽ
        mwk->state = FMENUSTATE_DECIDE_ITEM;
        FIELD_SUBSCREEN_ResetAction( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) );
      }
      else
      if( action == FIELD_SUBSCREEN_ACTION_TOPMENU_EXIT )
      {
        //�L�����Z��
        mwk->state = FMENUSTATE_EXIT_MENU;
        FIELD_SUBSCREEN_ResetAction( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) );
      }
    }
    break;
  case FMENUSTATE_DECIDE_ITEM:
    {
      GAMESYS_WORK *gameSys = GMEVENT_GetGameSysWork( event );
      GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
      const FIELD_MENU_ITEM_TYPE type = FIELD_SUBSCREEN_GetTopMenuItemNo( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) );
      GAMEDATA_SetSubScreenType( gameData , type );
      
      if( FldMapMenu_CallProcList[type] != NULL )
      {
        GF_ASSERT(mwk->subProcWork == NULL);
        if( FldMapMenu_CallProcList[type](mwk) == TRUE )
        {
          return( GMEVENT_RES_CONTINUE );
        }
      }
      else
      {
        mwk->state = FMENUSTATE_EXIT_MENU;
      }
     }
    break;

  case FMENUSTATE_WAIT_RETURN:
    //GF_ASSERT(0); // �J���͕ʂɏ��Ɉړ�
    //�}�ӂ⃌�|�[�g�̂Ƃ��ʂ�̂ŊJ���͎c���܂� Ari090728
    /* sub event �I���҂� */
    if(mwk->subProcWork != NULL)
    {
      GFL_HEAP_FreeMemory(mwk->subProcWork);
      mwk->subProcWork = NULL;
    }
    mwk->state = FMENUSTATE_RETURN_MENU;

    break;
    
  case FMENUSTATE_RETURN_MENU:
    mwk->state = FMENUSTATE_MAIN;
    break;
  
  case FMENUSTATE_EXIT_MENU:
    {
      MMDLSYS *fldMdlSys = FIELDMAP_GetMMdlSys( mwk->fieldWork );
      MMDLSYS_ClearPauseMoveProc( fldMdlSys );
      FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), mwk->return_subscreen_mode);
    }
    return( GMEVENT_RES_FINISH );
    break;
    
  //�ȉ��T�u�v���b�N�Ăяo���̂��߂̃t�B�[���h����
  
  case FMENUSTATE_FIELD_FADEOUT:
		GMEVENT_CallEvent(event, EVENT_FieldFadeOut(mwk->gmSys, mwk->fieldWork, 0));
    mwk->state = FMENUSTATE_FIELD_CLOSE;
    break;
    
  case FMENUSTATE_FIELD_CLOSE:
		GMEVENT_CallEvent(event, EVENT_FieldClose(mwk->gmSys, mwk->fieldWork));
    mwk->state = FMENUSTATE_CALL_SUB_PROC;
    break;
    
  case FMENUSTATE_FIELD_OPEN:
		GMEVENT_CallEvent(event, EVENT_FieldOpen(mwk->gmSys));
    mwk->state = FMENUSTATE_FIELD_FADEIN;
    break;
    
  case FMENUSTATE_FIELD_FADEIN:
		GMEVENT_CallEvent(event, EVENT_FieldFadeIn(mwk->gmSys, mwk->fieldWork, 0));
    mwk->state = FMENUSTATE_RETURN_MENU;
    break;

  case FMENUSTATE_CALL_SUB_PROC:
    if( FldMapMenu_SubProcData[mwk->subProcType].procData == NULL )
    {
      GF_ASSERT_MSG( FldMapMenu_SubProcData[mwk->subProcType].procData != NULL , "ProcData������");
      mwk->state = FMENUSTATE_FIELD_FADEIN;
      return( GMEVENT_RES_CONTINUE );
    }
    GAMESYSTEM_CallProc(mwk->gmSys, 
                        FldMapMenu_SubProcData[mwk->subProcType].ovId , 
                        FldMapMenu_SubProcData[mwk->subProcType].procData, 
                        mwk->subProcWork);
    mwk->state = FMENUSTATE_CALL_WAIT_SUB;
    break;
    
  case FMENUSTATE_CALL_WAIT_SUB:
    if( GAMESYSTEM_IsProcExists(mwk->gmSys)) break;


    
    if( FldMapMenu_SubProcData[mwk->subProcType].retFunc == NULL ||
        FldMapMenu_SubProcData[mwk->subProcType].retFunc(mwk) == FALSE )
    {
      if(mwk->subProcWork != NULL)
      {
        GFL_HEAP_FreeMemory(mwk->subProcWork);
        mwk->subProcWork = NULL;
      }
      mwk->state = FMENUSTATE_FIELD_OPEN;
    }
    else
    {
      mwk->state = FMENUSTATE_CALL_SUB_PROC;
    }
    break;
    
  }
  return( GMEVENT_RES_CONTINUE );

}

#pragma mark [>MenuCallProc
//----------------------------------------------------------------------
//  �ȉ����j���[���莞����
//----------------------------------------------------------------------

//======================================================================
//  �t�B�[���h�}�b�v���j���[�Ăяo��
//======================================================================
//--------------------------------------------------------------
/**
 * ���j���[�Ăяo�� �}�ӃC�x���g
 * @param mwk FMENU_EVENT_WORK
 * @retval  BOOL  TRUE=�C�x���g�؂�ւ�
 */
//--------------------------------------------------------------
static BOOL FMenuCallProc_Zukan( FMENU_EVENT_WORK *mwk )
{
  GMEVENT * subevent = createFMenuMsgWinEvent( mwk->gmSys, mwk->heapID,
    FLDMAPMENU_STR08, FIELDMAP_GetFldMsgBG(mwk->fieldWork) );
  GMEVENT_CallEvent(mwk->gmEvent, subevent);
  mwk->state = FMENUSTATE_WAIT_RETURN;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���j���[�Ăяo�� �|�P�����X�e�[�^�X
 * @param mwk FMENU_EVENT_WORK
 * @retval  BOOL  TRUE=�C�x���g�؂�ւ�
 */
//--------------------------------------------------------------
static BOOL FMenuCallProc_PokeStatus( FMENU_EVENT_WORK *mwk )
{
  GMEVENT * newEvent;
  
  PLIST_DATA *plistData;
  plistData = GFL_HEAP_AllocClearMemory(HEAPID_PROC, sizeof(PLIST_DATA));
  plistData->pp = GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(mwk->gmSys));
  plistData->mode = PL_MODE_FIELD;
  plistData->ret_sel = PL_SEL_POS_POKE1;

  mwk->subProcWork = plistData;
/*  
  newEvent = EVENT_FieldSubProc(mwk->gmSys, mwk->fieldWork,
      FS_OVERLAY_ID(pokelist), &PokeList_ProcData, plistData);
    
  GMEVENT_CallEvent(mwk->gmEvent, newEvent);
*/

  mwk->subProcType = FMENU_APP_POKELIST;
  mwk->state = FMENUSTATE_FIELD_FADEOUT;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���j���[�Ăяo�� �o�b�O
 * @param mwk FMENU_EVENT_WORK
 * @retval  BOOL  TRUE=�C�x���g�؂�ւ�
 */
//--------------------------------------------------------------

static BOOL FMenuCallProc_Bag( FMENU_EVENT_WORK *mwk )
{
  GMEVENT * newEvent;
  FIELD_ITEMMENU_WORK *epp;
	VecFx32 aPos;
	const FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer(mwk->fieldWork);
	FLDMAPPER *g3Dmapper = FIELDMAP_GetFieldG3Dmapper(mwk->fieldWork);
	GAMEDATA* pGameData = GAMESYSTEM_GetGameData(mwk->gmSys);
  
  epp = GFL_HEAP_AllocClearMemory(GFL_HEAPID_APP, sizeof(FIELD_ITEMMENU_WORK));
  epp->ctrl = SaveControl_GetPointer();
  epp->gsys = mwk->gmSys;
  epp->fieldmap = mwk->fieldWork;
	epp->heapID = GFL_HEAPID_APP;
	epp->mode = BAG_MODE_FIELD;   //�t�B�[���h����Ă΂��
	FIELD_PLAYER_GetPos(fld_player, &aPos);
	epp->icwk.NowAttr = MAPATTR_GetAttribute(g3Dmapper, &aPos);
	FIELD_PLAYER_GetDirPos((FIELD_PLAYER*)fld_player, FIELD_PLAYER_GetDir(fld_player), &aPos);
	epp->icwk.FrontAttr = MAPATTR_GetAttribute(g3Dmapper, &aPos);
	epp->icwk.gsys = mwk->gmSys;
	//�]�[���h�c
	epp->icwk.zone_id = PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork(pGameData ));
	//�A�����
	epp->icwk.Companion = FALSE;
	epp->mystatus = GAMEDATA_GetMyStatus(pGameData);

  if ( PLAYERWORK_GetMoveForm(GAMEDATA_GetMyPlayerWork(pGameData)) == PLAYER_MOVE_FORM_CYCLE ){
    epp->cycle_flg = TRUE;
  }
  
  mwk->subProcWork = epp;

//	newEvent = EVENT_FieldSubProc(mwk->gmSys, mwk->fieldWork, FS_OVERLAY_ID(bag), &ItemMenuProcData, epp);
//  epp->event = newEvent;

//	GMEVENT_CallEvent(mwk->gmEvent, newEvent);
////  mwk->state = FMENUSTATE_WAIT_RETURN;


  mwk->subProcType = FMENU_APP_BAG;
  mwk->state = FMENUSTATE_FIELD_FADEOUT;


  
	return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���j���[�Ăяo�� ����
 * @param mwk FMENU_EVENT_WORK
 * @retval  BOOL  TRUE=�C�x���g�؂�ւ�
 */
//--------------------------------------------------------------
static BOOL FMenuCallProc_MyTrainerCard( FMENU_EVENT_WORK *mwk )
{
  //GMEVENT * subevent = createFMenuMsgWinEvent(mwk->gmSys, mwk->heapID, FLDMAPMENU_STR11);
  //GMEVENT_CallEvent(mwk->gmEvent, subevent);
  GMEVENT * newEvent;
  TRCARD_CALL_PARAM *callParam = TRAINERCASR_CreateCallParam_SelfData( 
                                    GAMESYSTEM_GetGameData( mwk->gmSys ), 
                                    HEAPID_PROC );

  newEvent = EVENT_FieldSubProc(mwk->gmSys, mwk->fieldWork,
      TRCARD_OVERLAY_ID, &TrCardSysProcData, callParam);
  GMEVENT_CallEvent(mwk->gmEvent, newEvent);
  mwk->state = FMENUSTATE_WAIT_RETURN;
  return TRUE;
}

//--------------------------------------------------------------
/**
 * ���j���[�Ăяo�� ���|�[�g
 * @param mwk FMENU_EVENT_WORK
 * @retval  BOOL  TRUE=�C�x���g�؂�ւ�
 */
//--------------------------------------------------------------
static BOOL FMenuCallProc_Report( FMENU_EVENT_WORK *mwk )
{
  GMEVENT * subevent = createFMenuReportEvent( mwk->gmSys, mwk->fieldWork, mwk->heapID,
      FIELDMAP_GetFldMsgBG(mwk->fieldWork) );
  GMEVENT_CallEvent(mwk->gmEvent, subevent);
  mwk->state = FMENUSTATE_WAIT_RETURN;
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * ���j���[�Ăяo�� �ݒ�
 * @param mwk FMENU_EVENT_WORK
 * @retval  BOOL  TRUE=�C�x���g�؂�ւ�
 */
//--------------------------------------------------------------
static BOOL FMenuCallProc_Config( FMENU_EVENT_WORK *mwk )
{
  //GMEVENT * subevent = createFMenuMsgWinEvent(mwk->gmSys, mwk->heapID, FLDMAPMENU_STR13);
  //GMEVENT_CallEvent(mwk->gmEvent, subevent);
  GMEVENT * newEvent;
  GAMEDATA *gameData = GAMESYSTEM_GetGameData( mwk->gmSys );
  SAVE_CONTROL_WORK *saveControl = GAMEDATA_GetSaveControlWork( gameData );
  CONFIG *config = SaveData_GetConfig( saveControl );
  newEvent = EVENT_FieldSubProc(mwk->gmSys, mwk->fieldWork,
      FS_OVERLAY_ID(config_panel), &ConfigPanelProcData, config);
  GMEVENT_CallEvent(mwk->gmEvent, newEvent);
  mwk->state = FMENUSTATE_WAIT_RETURN;
  return TRUE;
}

#pragma mark [>ReturnProc
//----------------------------------------------------------------------
//  �ȉ��qProc�㏈��
//----------------------------------------------------------------------
//--------------------------------------------------------------
/**
 * �qProc�㏈�� PokeList
 * @param mwk FMENU_EVENT_WORK
 * @retval  BOOL  TRUE=�ʂ�Proc���Ăяo��(mwk->subProcType�Ɏ���proc��ݒ肵�Ă�������
 *                FALSE=Field�ɖ߂�
 */
//--------------------------------------------------------------
static const BOOL FMenuReturnProc_PokeList(FMENU_EVENT_WORK* mwk)
{
  PLIST_DATA *plData = mwk->subProcWork;
  
  switch( plData->ret_mode )
  {
  case PL_RET_NORMAL:      // �ʏ�
    return FALSE;
    break;
    
  case PL_RET_STATUS:      // ���j���[�u�悳���݂�v
    {
      PSTATUS_DATA *psData = GFL_HEAP_AllocMemory( HEAPID_PROC , sizeof(PSTATUS_DATA) );
      GAMEDATA *gmData = GAMESYSTEM_GetGameData(mwk->gmSys);
      SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork( gmData );
      
      psData->ppd = (void*)plData->pp;
      psData->cfg = plData->cfg;
      psData->ribbon = (u8*)SP_RIBBON_SAVE_GetSaveData(svWork);

      psData->ppt = PST_PP_TYPE_POKEPARTY;
      psData->max = PokeParty_GetPokeCount( plData->pp );
      psData->mode = PST_MODE_NORMAL;
      psData->pos = plData->ret_sel;
      
      FMenu_SetNextSubProc( mwk ,FMENU_APP_STATUS , psData );
      return TRUE;
    }
    break;
  
  default:
    return FALSE;
    break;
    
/*
  case PL_RET_NORMAL:      // �ʏ�
  case PL_RET_STATUS:      // ���j���[�u�悳���݂�v
  case PL_RET_CHANGE:      // ���j���[�u���ꂩ����v
  case PL_RET_ITEMSET:     // ���j���[�u��������v

  case PL_RET_WAZASET:     // �Z�I���ցi�Z�}�V���j
  case PL_RET_LVUP_WAZASET:// �Z�I���ցi�s�v�c�ȃA���j
  case PL_RET_MAILSET:     // ���[�����͂�
  case PL_RET_MAILREAD:    // ���[����ǂމ�ʂ�

  case PL_RET_ITEMSHINKA:  // �i����ʂցi�A�C�e���i���j
  case PL_RET_LVUPSHINKA:  // �i����ʂցi���x���i���j
  
  case PL_RET_BAG:         // �o�b�O����Ă΂ꂽ�ꍇ�ŁA�o�b�O�֖߂�
  
  case PL_RET_IAIGIRI:     // ���j���[ �Z�F����������
  case PL_RET_SORAWOTOBU:  // ���j���[ �Z�F������Ƃ�
  case PL_RET_NAMINORI:    // ���j���[ �Z�F�Ȃ݂̂�
  case PL_RET_KAIRIKI:     // ���j���[ �Z�F�����肫
  case PL_RET_KIRIBARAI:   // ���j���[ �Z�F����΂炢
  case PL_RET_IWAKUDAKI:   // ���j���[ �Z�F���킭����
  case PL_RET_TAKINOBORI:  // ���j���[ �Z�F�����̂ڂ�
  case PL_RET_ROCKCLIMB:   // ���j���[ �Z�F���b�N�N���C��

  case PL_RET_FLASH:       // ���j���[ �Z�F�t���b�V��
  case PL_RET_TELEPORT:    // ���j���[ �Z�F�e���|�[�g
  case PL_RET_ANAWOHORU:   // ���j���[ �Z�F���Ȃ��ق�
  case PL_RET_AMAIKAORI:   // ���j���[ �Z�F���܂�������
  case PL_RET_OSYABERI:    // ���j���[ �Z�F������ׂ�
  case PL_RET_MILKNOMI:    // ���j���[ �Z�F�~���N�̂�
  case PL_RET_TAMAGOUMI:   // ���j���[ �Z�F�^�}�S����
*/        
  }
  
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �qProc�㏈�� PokeStatus
 * @param mwk FMENU_EVENT_WORK
 * @retval  BOOL  TRUE=�ʂ�Proc���Ăяo��(mwk->subProcType�Ɏ���proc��ݒ肵�Ă�������
 *                FALSE=Field�ɖ߂�
 */
//--------------------------------------------------------------
static const BOOL FMenuReturnProc_PokeStatus(FMENU_EVENT_WORK* mwk)
{
  PSTATUS_DATA *psData = mwk->subProcWork;
  
  switch( psData->ret_mode )
  {
  case PST_RET_DECIDE:
  case PST_RET_CANCEL:
    {
      PLIST_DATA *plData = GFL_HEAP_AllocMemory( HEAPID_PROC , sizeof(PLIST_DATA) );
      GAMEDATA *gmData = GAMESYSTEM_GetGameData(mwk->gmSys);
      SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork( gmData );
      
      plData->pp = GAMEDATA_GetMyPokemon(gmData);
      plData->mode = PL_MODE_FIELD;
      plData->ret_sel = psData->pos;

      FMenu_SetNextSubProc( mwk ,FMENU_APP_POKELIST , plData );
      return TRUE;
    }
    break;

  case PST_RET_EXIT:
    return FALSE;
    break;
  }
  return FALSE;
}


//--------------------------------------------------------------
/**
 * �qProc�㏈�� �ėp�t�B�[���h�߂�
 * @param mwk FMENU_EVENT_WORK
 * @retval  BOOL  TRUE=�ʂ�Proc���Ăяo��(mwk->subProcType�Ɏ���proc��ݒ肵�Ă�������
 *                FALSE=Field�ɖ߂�
 */
//--------------------------------------------------------------
static const BOOL FMenuReturnProc_ReturnField(FMENU_EVENT_WORK* mwk)
{
  return FALSE;
}


//--------------------------------------------------------------
/**
 * �qProc�㏈�� Bag
 * @param mwk FMENU_EVENT_WORK
 * @retval  BOOL  TRUE=�ʂ�Proc���Ăяo��(mwk->subProcType�Ɏ���proc��ݒ肵�Ă�������
 *                FALSE=Field�ɖ߂�
 */
//--------------------------------------------------------------
static const BOOL FMenuReturnProc_Bag(FMENU_EVENT_WORK* mwk)
{
  FIELD_ITEMMENU_WORK *pBag = mwk->subProcWork;
  
  switch( pBag->ret_code )
  {
  case BAG_NEXTPROC_EXIT:      // �ʏ�
    return FALSE;
  case BAG_NEXTPROC_RETURN:      // �ʏ�
    return FALSE;
  case BAG_NEXTPROC_HAVE:    // �������� => �|�P�������X�g�N��
    {
      PLIST_DATA *plData = GFL_HEAP_AllocMemory( HEAPID_PROC , sizeof(PLIST_DATA) );
      GAMEDATA *gmData = GAMESYSTEM_GetGameData(mwk->gmSys);
      
      plData->pp = GAMEDATA_GetMyPokemon(gmData);
      plData->ret_sel = 0;
      plData->mode = PL_MODE_ITEMSET;    //�A�C�e�����Z�b�g����Ăяo��
      plData->item = pBag->ret_item;     //�A�C�e��ID
      plData->myitem = pBag->pMyItem;    // �A�C�e���f�[�^

      FMenu_SetNextSubProc( mwk ,FMENU_APP_POKELIST , plData );
      return TRUE;
    }
  case BAG_NEXTPROC_EXITEM:
  default:
    break;
  }
  return FALSE;
}



#pragma mark [>UtilFunc
//--------------------------------------------------------------
/**
  ��Proc�Ăяo���p�B
  �ĂԂƒ��őO��̃��[�N��Free�����̂Œ��ӁI�I�I�I�I
 */
//--------------------------------------------------------------
static void FMenu_SetNextSubProc(FMENU_EVENT_WORK* mwk, FMENU_APP_TYPE type , void *procWork )
{
  if( mwk->subProcWork != NULL )
  {
    GFL_HEAP_FreeMemory( mwk->subProcWork );
  }
  mwk->subProcWork = procWork;
  mwk->subProcType = type;
  
}

#pragma mark [>event
//----------------------------------------------------------------------
//  �ȉ����p�C�x���g(���|�[�g�E���쐬Msg
//----------------------------------------------------------------------

//======================================================================
//  ���b�Z�[�W�E�B���h�E�\���C�x���g
//======================================================================
//--------------------------------------------------------------
/// FMENU_MSGWIN_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  u32 heapID;
  u32 strID;
  FLDMSGBG *msgBG;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
}FMENU_MSGWIN_EVENT_WORK;

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * createFMenuMsgWinEvent(
  GAMESYS_WORK *gsys, u32 heapID, u32 strID, FLDMSGBG *msgBG )
{
  GMEVENT * msgEvent;
  FMENU_MSGWIN_EVENT_WORK *work;
  
  msgEvent = GMEVENT_Create(
    gsys, NULL, FMenuMsgWinEvent, sizeof(FMENU_MSGWIN_EVENT_WORK));
  work = GMEVENT_GetEventWork( msgEvent );
  MI_CpuClear8( work, sizeof(FMENU_MSGWIN_EVENT_WORK) );
  work->msgBG = msgBG;
  work->heapID = heapID;
  work->strID = strID;
  return msgEvent;
}

//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT_RESULT FMenuMsgWinEvent( GMEVENT *event, int *seq, void *wk )
{
  FMENU_MSGWIN_EVENT_WORK *work = wk;
  
  switch( (*seq) ){
  case 0:
    work->msgData = FLDMSGBG_CreateMSGDATA(
      work->msgBG, NARC_message_fldmapmenu_dat );
    work->msgWin = FLDMSGWIN_AddTalkWin( work->msgBG, work->msgData );
    FLDMSGWIN_Print( work->msgWin, 0, 0, work->strID );
    GXS_SetMasterBrightness(-16);
    (*seq)++;
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(work->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 2:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
        (*seq)++;
      }
    }
    break;
  case 3:
    FLDMSGWIN_Delete( work->msgWin );
    GFL_MSG_Delete( work->msgData );
    GXS_SetMasterBrightness(0);
    {
      GAMESYS_WORK *gameSys = GMEVENT_GetGameSysWork( event );
      GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
      FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( gameSys );
       
      const FIELD_MENU_ITEM_TYPE type = GAMEDATA_GetSubScreenType( gameData );
      FIELD_SUBSCREEN_SetTopMenuItemNo( FIELDMAP_GetFieldSubscreenWork(fieldWork) , type );
    }
    return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}

//======================================================================
//  ���|�[�g�\���C�x���g
//======================================================================
//--------------------------------------------------------------
/// FMENU_REPORT_EVENT_WORK
//--------------------------------------------------------------
typedef struct
{
  u32 heapID;
  FLDMSGBG *msgBG;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  GAMESYS_WORK *gsys;
  FIELD_MAIN_WORK *fieldWork;
}FMENU_REPORT_EVENT_WORK;

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * createFMenuReportEvent(GAMESYS_WORK *gsys, FIELD_MAIN_WORK *fieldWork, u32 heapID, FLDMSGBG *msgBG )
{
  GMEVENT * msgEvent;
  FMENU_REPORT_EVENT_WORK *work;

  msgEvent = GMEVENT_Create(
    gsys, NULL, FMenuReportEvent, sizeof(FMENU_REPORT_EVENT_WORK));
  work = GMEVENT_GetEventWork( msgEvent );
  MI_CpuClear8( work, sizeof(FMENU_REPORT_EVENT_WORK) );
  work->msgBG = msgBG;
  work->heapID = heapID;
  work->gsys = gsys;
  work->fieldWork = fieldWork;
  return msgEvent;
}

//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT_RESULT FMenuReportEvent( GMEVENT *event, int *seq, void *wk )
{
  FMENU_REPORT_EVENT_WORK *work = wk;
  
  switch( (*seq) ){
  case 0:
    {
      GAME_COMM_SYS_PTR commSys = GAMESYSTEM_GetGameCommSysPtr( work->gsys );
      work->msgData = FLDMSGBG_CreateMSGDATA(
             work->msgBG, NARC_message_fldmapmenu_dat );
      
      work->msgWin = FLDMSGWIN_AddTalkWin( work->msgBG, work->msgData );
      
      if( GameCommSys_BootCheck(commSys) == GAME_COMM_NO_INVASION )
      {
        //�ʐM���Ń��|�[�g�������Ȃ�
        FLDMSGWIN_Print( work->msgWin, 0, 0, FLDMAPMENU_STR16 );
        GXS_SetMasterBrightness(-16);
        (*seq) = 20;
      }
      else
      {
        FLDMSGWIN_Print( work->msgWin, 0, 0, FLDMAPMENU_STR14 );
        //�{���Ȃ烌�|�[�g�p�ւ̐؂�ւ��H
        //FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), FIELD_SUBSCREEN_TOPMENU);
        GXS_SetMasterBrightness(-16);
        (*seq)++;
      }
    }
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(work->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 2:
    GAMEDATA_Save(GAMESYSTEM_GetGameData(GMEVENT_GetGameSysWork(event)));
    (*seq)++;
    break;
  case 3:
    FLDMSGWIN_Delete( work->msgWin );
    
    work->msgWin = FLDMSGWIN_AddTalkWin( work->msgBG, work->msgData );
    FLDMSGWIN_Print( work->msgWin, 0, 0, FLDMAPMENU_STR15 );
    (*seq)++;
    break;
  case 4:
    if( FLDMSGWIN_CheckPrintTrans(work->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 5:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
        (*seq)++;
      }
    }
    break;
  case 6:
    FLDMSGWIN_Delete( work->msgWin );
    GFL_MSG_Delete( work->msgData );
    //�{���Ȃ烁�j���[�ւ̐؂�ւ��H
    //FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), FIELD_SUBSCREEN_TOPMENU);
    GXS_SetMasterBrightness(0);
    {
      GAMESYS_WORK *gameSys = GMEVENT_GetGameSysWork( event );
      GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
      FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( gameSys );
       
      const FIELD_MENU_ITEM_TYPE type = GAMEDATA_GetSubScreenType( gameData );
      FIELD_SUBSCREEN_SetTopMenuItemNo( FIELDMAP_GetFieldSubscreenWork(fieldWork) , type );
    }
    return( GMEVENT_RES_FINISH );
    break;
    
    //�Z�[�u�ł��܂���
  case 20:
    if( FLDMSGWIN_CheckPrintTrans(work->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 21:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
        (*seq)++;
      }
    }
    break;
  case 22:
    FLDMSGWIN_Delete( work->msgWin );
    GFL_MSG_Delete( work->msgData );
    GXS_SetMasterBrightness(0);
    {
      GAMESYS_WORK *gameSys = GMEVENT_GetGameSysWork( event );
      GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
      FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork( gameSys );
      
      const FIELD_MENU_ITEM_TYPE type = GAMEDATA_GetSubScreenType( gameData );
      FIELD_SUBSCREEN_SetTopMenuItemNo( FIELDMAP_GetFieldSubscreenWork(fieldWork) , type );
    }
    return( GMEVENT_RES_FINISH );
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}
