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
#include "gamesystem/game_data.h"
#include "gamesystem/game_event.h"
#include "field/field_msgbg.h"
#include "field/event_fieldmap_menu.h"
#include "field/fieldmap.h"

#include "event_fieldmap_control.h" //EVENT_FieldSubProc
#include "app/config_panel.h"   //ConfigPanelProcData
#include "app/trainer_card.h"   //TrainerCardSysProcData

extern const GFL_PROC_DATA DebugAriizumiMainProcData;
extern const GFL_PROC_DATA TrainerCardProcData;

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
}FMENU_STATE;

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

//--------------------------------------------------------------
/// FMENU_LISTDATA
//--------------------------------------------------------------
typedef struct
{
  u16 charsize_x;
  u16 charsize_y;
  u32 max;
  const FLDMENUFUNC_LIST *list;
}FMENU_LISTDATA;

//--------------------------------------------------------------
/// FMENU_EVENT_WORK
//--------------------------------------------------------------
struct _TAG_FMENU_EVENT_WORK
{
  HEAPID heapID;
  GMEVENT *gmEvent;
  GAMESYS_WORK *gmSys;
  FIELD_MAIN_WORK *fieldWork;
  
  FIELD_MENU_ITEM_TYPE befType;
  
  int menu_num;
  FMENU_CALLPROC call_proc;
  
  GFL_MSGDATA *msgData;
  FLDMENUFUNC *menuFunc;
  
  void *sub_proc_parent;
};

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

static GMEVENT * createFMenuMsgWinEvent(
  GAMESYS_WORK *gsys, u32 heapID, u32 strID, FLDMSGBG *msgBG );
static GMEVENT_RESULT FMenuMsgWinEvent( GMEVENT *event, int *seq, void *wk );

static GMEVENT * createFMenuReportEvent(
  GAMESYS_WORK *gsys, FIELD_MAIN_WORK *fieldWork, u32 heapID, FLDMSGBG *msgBG );
static GMEVENT_RESULT FMenuReportEvent( GMEVENT *event, int *seq, void *wk );

//--------------------------------------------------------------
/// �t�B�[���h�}�b�v���j���[���X�g
//--------------------------------------------------------------
static const FLDMENUFUNC_LIST DATA_FldMapMenuList[] =
{
  { FLDMAPMENU_STR01, FMenuCallProc_Zukan },
  { FLDMAPMENU_STR02, FMenuCallProc_PokeStatus },
  { FLDMAPMENU_STR03, FMenuCallProc_Bag },
  { FLDMAPMENU_STR04, FMenuCallProc_MyTrainerCard },
  { FLDMAPMENU_STR05, FMenuCallProc_Report },
  { FLDMAPMENU_STR06, FMenuCallProc_Config },
  { FLDMAPMENU_STR07, NULL },
};

//--------------------------------------------------------------
/// �t�B�[���h�}�b�v���j���[���X�g�e�[�u��
//--------------------------------------------------------------
static const FMENU_LISTDATA DATA_FldMapMenuListTbl[] =
{
  {
    MENU_CHARSIZE_X,
    MENU_CHARSIZE_Y,
    NELEMS(DATA_FldMapMenuList),
    DATA_FldMapMenuList
  },
};

//���j���[�ő吔
#define MENULISTTBL_MAX (NELEMS(DATA_FldMapMenuListTbl))

//--------------------------------------------------------------
/// ���j���[�w�b�_�[
//--------------------------------------------------------------
static const FLDMENUFUNC_HEADER DATA_FldMapMenuListHeader =
{
  1,    //���X�g���ڐ�
  9,    //�\���ő區�ڐ�
  0,    //���x���\���w���W
  13,   //���ڕ\���w���W
  0,    //�J�[�\���\���w���W
  0,    //�\���x���W
  1,    //�\�������F
  15,   //�\���w�i�F
  2,    //�\�������e�F
  0,    //�����Ԋu�w
  6,    //�����Ԋu�x
  FLDMENUFUNC_SKIP_LRKEY, //�y�[�W�X�L�b�v�^�C�v
  12,   //�����T�C�YX(�h�b�g
  12,   //�����T�C�YY(�h�b�g
  0,    //�\�����WX �L�����P��
  0,    //�\�����WY �L�����P��
  0,    //�\���T�C�YX �L�����P��
  0,    //�\���T�C�YY �L�����P��
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
  mwk->befType = FMIT_POKEMON;
  
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
  switch (*seq) 
  {
  case FMENUSTATE_INIT:
    if( FIELD_SUBSCREEN_CanChange( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) ) == TRUE )
    {
      GAMESYS_WORK *gameSys = GMEVENT_GetGameSysWork( event );
      GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
      GAMEDATA_SetSubScreenType( gameData , FMIT_POKEMON );
      FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), FIELD_SUBSCREEN_TOPMENU);
      //FIELD_SUBSCREEN_SetTopMenuItemNo( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) , mwk->befType );
      (*seq) = FMENUSTATE_MAIN;
      //WIPE_SYS_Start( WIPE_PATTERN_S , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
      //                WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , mwk->heapID );
    }
    break;
  case FMENUSTATE_MAIN:
    {
      const FIELD_SUBSCREEN_ACTION action = FIELD_SUBSCREEN_GetAction(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork));
      if( action == FIELD_SUBSCREEN_ACTION_TOPMENU_DECIDE )
      {
        //�������ڂ����肳�ꂽ
        (*seq) = FMENUSTATE_DECIDE_ITEM;
        FIELD_SUBSCREEN_ResetAction( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) );
      }
      else
      if( action == FIELD_SUBSCREEN_ACTION_TOPMENU_EXIT )
      {
        //�L�����Z��
        FIELD_SUBSCREEN_ResetAction( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) );
        FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), FIELD_SUBSCREEN_NORMAL);
        return( GMEVENT_RES_FINISH );
      }
    }
    break;
  case FMENUSTATE_DECIDE_ITEM:
    {
      GAMESYS_WORK *gameSys = GMEVENT_GetGameSysWork( event );
      GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
      const FIELD_MENU_ITEM_TYPE type = FIELD_SUBSCREEN_GetTopMenuItemNo( FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork) );
      mwk->befType = type;
      GAMEDATA_SetSubScreenType( gameData , type );
      
      switch( type )
      {
        case FMIT_POKEMON:
          mwk->call_proc = FMenuCallProc_PokeStatus;
          break;

        case FMIT_ZUKAN:
          mwk->call_proc = FMenuCallProc_Zukan;
          break;

        case FMIT_ITEMMENU:
          mwk->call_proc = FMenuCallProc_Bag;
          break;

        case FMIT_TRAINERCARD:
          mwk->call_proc = FMenuCallProc_MyTrainerCard;
          break;

        case FMIT_REPORT:
          mwk->call_proc = FMenuCallProc_Report;
          break;

        case FMIT_CONFING:
          mwk->call_proc = FMenuCallProc_Config;
          break;
      }
      if( mwk->call_proc != NULL ){
        GF_ASSERT(mwk->sub_proc_parent == NULL);
        if( mwk->call_proc(mwk) == TRUE ){
          mwk->call_proc = NULL;
          (*seq) = FMENUSTATE_WAIT_RETURN;
          return( GMEVENT_RES_CONTINUE );
        }
      }
      return( GMEVENT_RES_FINISH );
     }
    break;

  case FMENUSTATE_WAIT_RETURN:
    /* sub event �I���҂� */
    if(mwk->sub_proc_parent != NULL)
    {
      GFL_HEAP_FreeMemory(mwk->sub_proc_parent);
      mwk->sub_proc_parent = NULL;
    }
    (*seq) = FMENUSTATE_RETURN_MENU;
    break;
    
  case FMENUSTATE_RETURN_MENU:
    (*seq) = FMENUSTATE_MAIN;
    break;
  }
  return( GMEVENT_RES_CONTINUE );

#if 0
  FMENU_EVENT_WORK *mwk = wk;
  
  switch (*seq) {
  case 0:
    {
      FLDMSGBG *msgBG;
      FLDMENUFUNC_HEADER head;
      FLDMENUFUNC_LISTDATA *listdata;
      const FLDMENUFUNC_LIST *menulist; 
      const FMENU_LISTDATA *fmenu_listdata;
      
      msgBG = FIELDMAP_GetFldMsgBG( mwk->fieldWork );
      mwk->msgData = FLDMSGBG_CreateMSGDATA(
        msgBG, NARC_message_fldmapmenu_dat );
      
      fmenu_listdata = &DATA_FldMapMenuListTbl[mwk->menu_num];
      menulist = fmenu_listdata->list;
      
      listdata = FLDMENUFUNC_CreateMakeListData(
        menulist, fmenu_listdata->max, mwk->msgData, mwk->heapID );
      
      head = DATA_FldMapMenuListHeader;
      FLDMENUFUNC_InputHeaderListSize( &head, fmenu_listdata->max,
        23, 1, fmenu_listdata->charsize_x, fmenu_listdata->charsize_y );
      
      mwk->menuFunc = FLDMENUFUNC_AddMenu( msgBG, &head, listdata );
      GFL_MSG_Delete( mwk->msgData );
    }
    
    (*seq)++;
    break;
  case 1:
    {
      u32 ret;
      ret = FLDMENUFUNC_ProcMenu( mwk->menuFunc );
      
      if( ret == FLDMENUFUNC_NULL ){  //���얳��
        break;
      }else if( ret == FLDMENUFUNC_CANCEL ){  //�L�����Z��
        (*seq)++;
      }else{              //����
        mwk->call_proc = (FMENU_CALLPROC)ret;
        (*seq)++;
      }
    }
    break;
  case 2:
    {
      FLDMENUFUNC_DeleteMenu( mwk->menuFunc );
      
      if( mwk->call_proc != NULL ){
        GF_ASSERT(mwk->sub_proc_parent == NULL);
        if( mwk->call_proc(mwk) == TRUE ){
          mwk->call_proc = NULL;
          (*seq)++;
          return( GMEVENT_RES_CONTINUE );
        }
      }
      
      return( GMEVENT_RES_FINISH );
    }
    break;
  case 3:
    /* sub event �I���҂� */
    if(mwk->sub_proc_parent != NULL){
      GFL_HEAP_FreeMemory(mwk->sub_proc_parent);
      mwk->sub_proc_parent = NULL;
    }
    (*seq) = 0;
    break;
  }
  return( GMEVENT_RES_CONTINUE );
#endif
}

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
  //GMEVENT * subevent = createFMenuMsgWinEvent(mwk->gmSys, mwk->heapID, FLDMAPMENU_STR09);
  //GMEVENT_CallEvent(mwk->gmEvent, subevent);
  GMEVENT * newEvent;
  EASY_POKELIST_PARENT *epp;
  
  epp = GFL_HEAP_AllocClearMemory(GFL_HEAPID_APP, sizeof(EASY_POKELIST_PARENT));
  epp->party = GAMEDATA_GetMyPokemon(GAMESYSTEM_GetGameData(mwk->gmSys));
  mwk->sub_proc_parent = epp;
  newEvent = EVENT_FieldSubProc(mwk->gmSys, mwk->fieldWork,
      FS_OVERLAY_ID(pokelist), &EasyPokeListData, epp);
  GMEVENT_CallEvent(mwk->gmEvent, newEvent);
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
  GMEVENT * subevent = createFMenuMsgWinEvent(mwk->gmSys, mwk->heapID,
      FLDMAPMENU_STR10, FIELDMAP_GetFldMsgBG(mwk->fieldWork) );
  GMEVENT_CallEvent(mwk->gmEvent, subevent);
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
  return TRUE;
}

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
    work->msgData = FLDMSGBG_CreateMSGDATA(
      work->msgBG, NARC_message_fldmapmenu_dat );
    
    work->msgWin = FLDMSGWIN_AddTalkWin( work->msgBG, work->msgData );
    FLDMSGWIN_Print( work->msgWin, 0, 0, FLDMAPMENU_STR14 );
    //�{���Ȃ烌�|�[�g�p�ւ̐؂�ւ��H
    //FIELD_SUBSCREEN_Change(FIELDMAP_GetFieldSubscreenWork(mwk->fieldWork), FIELD_SUBSCREEN_TOPMENU);
    GXS_SetMasterBrightness(-16);
    (*seq)++;
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
  }

  return( GMEVENT_RES_CONTINUE );
}
