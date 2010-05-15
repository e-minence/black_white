//============================================================================================
/**
 * @file     event_gtsnego.c
 * @brief    �C�x���g�FGTS�l�S�V�G�[�V����
 * @author   k.ohno
 * @date     2009.11.15
 */
//============================================================================================


#include <gflib.h>
#include "net/network_define.h"
#include "app/mailbox.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "net_app/pokemontrade.h"
#include "field/fieldmap.h"
#include "sound/pm_sndsys.h"

#include "./event_fieldmap_control.h"
#include "field/event_gtsnego.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "poke_tool/monsno_def.h"
#include "system/main.h"      //GFL_HEAPID_APP�Q��

#include "net_app/gtsnego.h"
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"

#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"
//============================================================================================
//============================================================================================
FS_EXTERN_OVERLAY(pokemon_trade);
FS_EXTERN_OVERLAY(app_mail);

//----------------------------------------------------------------

#define _TIMINGDISCONNECT (1)
#define HEAPID_CORE GFL_HEAPID_APP

enum _EVENT_GTSNEGO {
  _FIELD_FADE_START,
  _FIELD_CLOSE,
  _CALL_WIFILOGIN,
  _WAIT_WIFILOGIN,
  _CALL_WIFINEGO,
  _WAIT_WIFINEGO,
  _CALL_TRADE,
  _WAIT_TRADE,
  _DISCONNECT_TRADE,
  _SEQ_EVOLUTION,
  _SEQ_EVOLUTIONEND,
  _CALL_MAIL,
  _WAIT_MAIL,
  _WAIT_NET_END,
  _WAIT_NET_END2,
  _FIELD_OPEN,
  _FIELD_FADEIN,
  _FIELD_END
};

//-------------------------------------
///	EVENT_GTSNEGO�p�ڑ����[�N
//=====================================
typedef struct 
{
  int soundNo;
  DWCSvlResult aSVL;
  GAMESYS_WORK        *gsys;
  FIELDMAP_WORK       *fieldmap;
  EVENT_GTSNEGO_WORK  gts;
  POKEMONTRADE_PARAM aPokeTr;
  WIFILOGIN_PARAM     login;
  WIFILOGOUT_PARAM   logout;
  
  BOOL bFieldEnd;
#if PM_DEBUG
  BOOL is_debug;
#endif
} EVENT_GTSNEGO_LINK_WORK;



//============================================================================================
//
//    �T�u�C�x���g
//
//============================================================================================
static GMEVENT_RESULT EVENT_GTSNegoMain(GMEVENT * event, int *  seq, void * work)
{
  EVENT_GTSNEGO_LINK_WORK * dbw = work;
  GAMESYS_WORK * gsys = dbw->gsys;
  GAMEDATA* gamedata=GAMESYSTEM_GetGameData(gsys);
  FIELDMAP_WORK * pFieldmap = GAMESYSTEM_GetFieldMapWork(gsys);

  switch (*seq) {
  case _FIELD_FADE_START:
    {
//      GMEVENT* fade_event;
  //    fade_event = EVENT_FieldFadeOut_Black(gsys, pFieldmap, FIELD_FADE_WAIT);
    //  GMEVENT_CallEvent(event, fade_event);
    }
    
    dbw->soundNo = PMSND_GetBGMsoundNo();
    PMSND_FadeOutBGM(6);
    (*seq) ++;
    break;
  case _FIELD_CLOSE:
    if(GAME_COMM_NO_NULL == GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gsys)))
    {
      GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, pFieldmap));
      (*seq)++;
    }
    break;
  case _CALL_WIFILOGIN:
    dbw->login.pSvl = &dbw->aSVL;
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, &dbw->login);
    (*seq)++;
    break;
  case _WAIT_WIFILOGIN:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){
      if( dbw->login.result == WIFILOGIN_RESULT_LOGIN )
      { 
        (*seq) ++;
      }
      else
      { 
        (*seq)  = _FIELD_OPEN;
      }
    }
    break;
  case _CALL_WIFINEGO:
    dbw->gts.result = FALSE;
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(gts_negotiate), &GtsNego_ProcData, &dbw->gts);
    (*seq)++;
    break;
  case _WAIT_WIFINEGO:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }
    if(dbw->gts.result == EVENT_GTSNEGO_EXIT){
      (*seq) = _WAIT_NET_END;
    }
    else if(dbw->gts.result == EVENT_GTSNEGO_ERROR){
      dbw->login.mode = WIFILOGIN_MODE_ERROR;
      (*seq) = _CALL_WIFILOGIN;
    }
    else{
      (*seq)++;
    }
    break;
  case _CALL_TRADE:
    dbw->aPokeTr.ret = POKEMONTRADE_MOVE_START;
    dbw->aPokeTr.gamedata = gamedata;
    dbw->aPokeTr.pNego = &dbw->gts;
    dbw->aPokeTr.pSvl = &dbw->aSVL;
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(pokemon_trade), &PokemonTradeWiFiProcData, &dbw->aPokeTr);
    (*seq)++;
    break;
  case _WAIT_TRADE:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){
      if(dbw->aPokeTr.ret == POKEMONTRADE_MOVE_EVOLUTION){
        (*seq) = _SEQ_EVOLUTION;
      }
      else if(dbw->aPokeTr.ret == POKEMONTRADE_MOVE_ERROR){
        (*seq) = _CALL_WIFINEGO;
      }
      else{
        GFL_NET_HANDLE_TimeSyncStart(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGDISCONNECT, WB_NET_IRCBATTLE);
        GFL_NET_SetAutoErrorCheck(FALSE);
        GFL_NET_SetNoChildErrorCheck(FALSE);
        (*seq) = _DISCONNECT_TRADE;
      }
    }
    break;
  case _DISCONNECT_TRADE:
    if(GFL_NET_HANDLE_IsTimeSync(GFL_NET_HANDLE_GetCurrentHandle(),_TIMINGDISCONNECT, WB_NET_IRCBATTLE)){
      GFL_NET_StateWifiMatchEnd(TRUE);
      (*seq) = _CALL_WIFINEGO;
    }
    break;
  case _SEQ_EVOLUTION:
    //GFL_OVERLAY_Load( FS_OVERLAY_ID(shinka_demo) );
    //dbw->aPokeTr.shinka_param = SHINKADEMO_AllocParam( HEAPID_PROC, dbw->aPokeTr.gamedata,
    //                                           dbw->aPokeTr.pParty,
    //                                           dbw->aPokeTr.after_mons_no,
    //                                           0, dbw->aPokeTr.cond, TRUE );
    {
      SHINKA_DEMO_PARAM* sdp = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof( SHINKA_DEMO_PARAM ) );
      sdp->gamedata          = dbw->aPokeTr.gamedata;
      sdp->ppt               = dbw->aPokeTr.pParty;
      sdp->after_mons_no     = dbw->aPokeTr.after_mons_no;
      sdp->shinka_pos        = 0;
      sdp->shinka_cond       = dbw->aPokeTr.cond;
      sdp->b_field           = TRUE;
      sdp->b_enable_cancel   = FALSE;
      dbw->aPokeTr.shinka_param  = sdp;
    }
    GAMESYSTEM_CallProc( gsys, FS_OVERLAY_ID(shinka_demo), &ShinkaDemoProcData, dbw->aPokeTr.shinka_param );
		(*seq) ++;
    break;
  case _SEQ_EVOLUTIONEND:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){
      //SHINKADEMO_FreeParam( dbw->aPokeTr.shinka_param );
      {
        SHINKA_DEMO_PARAM* sdp = dbw->aPokeTr.shinka_param;
        GFL_HEAP_FreeMemory( sdp );
      }
      //GFL_OVERLAY_Unload( FS_OVERLAY_ID(shinka_demo) );
      dbw->aPokeTr.ret = POKEMONTRADE_MOVE_EVOLUTION;
      (*seq) = _CALL_TRADE;
    }
    break;
  case _CALL_MAIL:
    dbw->aPokeTr.aMailBox.gamedata = gamedata;
    GAMESYSTEM_CallProc( gsys, FS_OVERLAY_ID(app_mail), &MailBoxProcData, &dbw->aPokeTr.aMailBox );
    (*seq)++;
    break;
  case _WAIT_MAIL:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){
      (*seq)=_CALL_TRADE;
    }
    break;
  case _WAIT_NET_END:
    GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(wifi_login), &WiFiLogout_ProcData, &dbw->logout);
    (*seq)++;
    break;
  case _WAIT_NET_END2:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){
      (*seq) ++;
    }
    break;
  case _FIELD_OPEN:
    GFL_HEAP_FreeMemory(dbw->gts.pStatus[0]);
    GFL_HEAP_FreeMemory(dbw->gts.pStatus[1]);
    GFL_HEAP_FreeMemory(dbw->aPokeTr.pParty);
    PMSND_PlayBGM(dbw->soundNo);
    PMSND_FadeInBGM(PMSND_FADE_NORMAL);
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq) ++;
    break;
  case _FIELD_FADEIN:
#if PM_DEBUG
    if(dbw->is_debug){
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, pFieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
#endif
    (*seq) ++;
    break;
  case _FIELD_END:
    return GMEVENT_RES_FINISH;
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

/*
 *  @brief  WiFi�N���u�Ăяo���p�����[�^�Z�b�g
 */
static EVENT_GTSNEGO_LINK_WORK* wifi_SetEventParam( GMEVENT* event, GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap,BOOL bFieldEnd )
{
  BATTLE_SETUP_PARAM * para;
  EVENT_GTSNEGO_LINK_WORK * dbw;

  if(GAME_COMM_NO_NULL!= GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gsys))){
    GameCommSys_ExitReq(GAMESYSTEM_GetGameCommSysPtr(gsys));
  }

  dbw = GMEVENT_GetEventWork(event);
  dbw->gsys = gsys;
//  dbw->fieldmap = fieldmap;
  dbw->bFieldEnd = bFieldEnd;
  dbw->aPokeTr.pParty = PokeParty_AllocPartyWork( HEAPID_PROC );

  //GTSNEGO�����̐ݒ�
  {
    GFL_STD_MemClear( &dbw->gts, sizeof(EVENT_GTSNEGO_WORK) );
    dbw->gts.gamedata = GAMESYSTEM_GetGameData(gsys);
    dbw->gts.pStatus[0] = GFL_HEAP_AllocClearMemory(HEAPID_PROC,MyStatus_GetWorkSize());
    dbw->gts.pStatus[1] = GFL_HEAP_AllocClearMemory(HEAPID_PROC,MyStatus_GetWorkSize());
    {
      MYSTATUS * pMy =GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(gsys));
      GFL_STD_MemCopy(pMy,dbw->gts.pStatus[0], MyStatus_GetWorkSize());
    }
  }
  //WIFI�����̐ݒ�
  { 
    GFL_STD_MemClear( &dbw->login, sizeof(WIFILOGIN_PARAM) );
    dbw->login.gamedata = GAMESYSTEM_GetGameData(gsys);
    dbw->login.bg = WIFILOGIN_BG_NORMAL;
    dbw->login.display = WIFILOGIN_DISPLAY_UP;
    dbw->logout.gamedata = GAMESYSTEM_GetGameData(gsys);
    dbw->logout.bg = WIFILOGIN_BG_NORMAL;
    dbw->logout.display = WIFILOGIN_DISPLAY_UP;
  }
  return dbw;
}

//------------------------------------------------------------------
/*
 *  @brief  WiFi�N���u�C�x���g�R�[��
 */
//------------------------------------------------------------------
static GMEVENT* EVENT_GTSNego( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap )
{
  GMEVENT * event = GMEVENT_Create(gsys, NULL, EVENT_GTSNegoMain, sizeof(EVENT_GTSNEGO_LINK_WORK));
  wifi_SetEventParam( event, gsys, fieldmap,FALSE );
  return event;
}


//-------------------------------------
///	GMEVENT_CreateOverlayEventCall�֐��p�R�[���o�b�N�֐�
//
//  void* work �ɂ� FIELDMAP_WORK*��n���B
//=====================================
GMEVENT* EVENT_CallGTSNego( GAMESYS_WORK* gsys, void* work )
{
  return EVENT_GTSNego( gsys, work );
}

#if PM_DEBUG

//------------------------------------------------------------------
/*
 *  @brief  WiFi�N���u�C�x���g�`�F���W
 */
//------------------------------------------------------------------
GMEVENT* EVENT_GTSNegoChangeDebug(GAMESYS_WORK * gsys,  void * work)
{
  EVENT_GTSNEGO_LINK_WORK* dbw ;
  FIELDMAP_WORK * fieldmap = work;
  GMEVENT * event = GMEVENT_Create(gsys, NULL, EVENT_GTSNegoMain, sizeof(EVENT_GTSNEGO_LINK_WORK));
  dbw = wifi_SetEventParam( event, gsys, fieldmap,TRUE );
  dbw->is_debug=TRUE;

  return event;
}
#endif


