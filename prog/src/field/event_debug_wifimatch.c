//============================================================================================
/**
 * @file     event_wifi_match.c
 * @brief	  �C�x���g�FWIFI���E�ΐ�
 * @author   Toru=Nagihashi
 * @date     2009.11.24
 */
//============================================================================================
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/main.h"      //GFL_HEAPID_APP�Q��

//�t�B�[���h
#include "event_fieldmap_control.h"

//PROC
#include "net_app/debug_wifi_match.h"
#include "net_app/wifi_login.h"

//�O�����J
#include "field/event_debug_wifimatch.h"



FS_EXTERN_OVERLAY(wifibattlematch_core);

//============================================================================================
//
//    �萔
//
//============================================================================================
//-------------------------------------
///	�V�[�P���X
//=====================================
enum{
  _FIELD_COMM,
  _FIELD_FADEOUT,
  _FIELD_CLOSE,
  _CALL_WIFILOGIN,
  _WAIT_WIFILOGIN,
  _CALL_WIFIBTLMATCH,
  _WAIT_WIFIBTLMATCH,
  _WAIT_NET_END,
  _FIELD_OPEN,
  _FIELD_FADEIN,
  _FIELD_END
};

//-------------------------------------
///	�C�x���g���[�N
//=====================================
typedef struct 
{
  GAMESYS_WORK      * gsys;
  FIELDMAP_WORK     * fieldmap;
  SAVE_CONTROL_WORK * ctrl;
  void              * p_sub_wk;
  DEBUG_WIFIBATTLEMATCH_MODE mode;
} EVENT_DEBUGWIFIMATCH_WORK;


//============================================================================================
//
//    �T�u�C�x���g
//
//============================================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �C�x���g���C������
 *
 *	@param	* event �C�x���g
 *	@param	*  seq  �V�[�P���X
 *	@param	* work  �����œn�����A�h���X
 *
 *	@return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT EVENT_DebugWifiMatchMain(GMEVENT * event, int *  seq, void * work)
{
  EVENT_DEBUGWIFIMATCH_WORK * dbw = work;
  GAMESYS_WORK * gsys = dbw->gsys;


  switch (*seq) {
  case _FIELD_COMM:
    if(GAME_COMM_NO_NULL == GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gsys)))
    {
      (*seq)++;
    }
    break;
  case _FIELD_FADEOUT:
    GMEVENT_CallEvent(event, EVENT_FieldFadeOut(gsys, dbw->fieldmap, FIELD_FADE_BLACK, FIELD_FADE_WAIT));
    (*seq)++;
    break;
  case _FIELD_CLOSE:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, dbw->fieldmap));
    (*seq)++;
    break;
  case _CALL_WIFILOGIN:
    { 
      WIFILOGIN_PARAM *p_param;
      dbw->p_sub_wk = GFL_HEAP_AllocClearMemory(HEAPID_PROC,sizeof(WIFILOGIN_PARAM));
      p_param = dbw->p_sub_wk;
      p_param->gamedata = GAMESYSTEM_GetGameData( gsys );
      p_param->nsid = WB_NET_WIFIMATCH;
      GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, dbw->p_sub_wk);
    }
    (*seq)++;
    break;
  case _WAIT_WIFILOGIN:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){

      WIFILOGIN_PARAM *p_param  = dbw->p_sub_wk;
      if( p_param->result == WIFILOGIN_RESULT_LOGIN )
      { 
        (*seq) ++;
      }
      else
      { 
        (*seq)  = _WAIT_NET_END;
      }
      GFL_HEAP_FreeMemory( dbw->p_sub_wk );
    }
    break;
  case _CALL_WIFIBTLMATCH:
    { 
      DEBUG_WIFIBATTLEMATCH_PARAM *p_param;
      dbw->p_sub_wk = GFL_HEAP_AllocClearMemory(HEAPID_PROC,sizeof(DEBUG_WIFIBATTLEMATCH_PARAM));
      p_param = dbw->p_sub_wk;
      p_param->mode   = dbw->mode;
      p_param->p_save = dbw->ctrl;
      p_param->p_gamedata = GAMESYSTEM_GetGameData( gsys );
      GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(wifibattlematch_core), &DEBUG_WifiBattleMatch_ProcData, dbw->p_sub_wk );
    }
    (*seq)++;
    break;
  case _WAIT_WIFIBTLMATCH:
    if (GAMESYSTEM_IsProcExists(gsys) == GFL_PROC_MAIN_NULL){
      GFL_HEAP_FreeMemory( dbw->p_sub_wk );
      (*seq) ++;
    }
    break;
  case _WAIT_NET_END:
      (*seq) ++;
    break;
  case _FIELD_OPEN:
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq) ++;
    break;
  case _FIELD_FADEIN:
    GMEVENT_CallEvent(event,EVENT_FieldFadeIn_Black(gsys, dbw->fieldmap, FIELD_FADE_WAIT ));
    (*seq) ++;
    break;
  case _FIELD_END:
    return GMEVENT_RES_FINISH;
  default:
    GF_ASSERT(0);
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�OWiFi���E�ΐ�R�[��
 *
 *	@param	GAMESYS_WORK * gsys �Q�[���V�X�e��
 *	@param	* fieldmap          �t�B�[���h�}�b�v
 *	@param  mode                ���[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT* EVENT_DEBUG_WifiMatch( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, DEBUG_WIFIBATTLEMATCH_MODE mode )
{
  GMEVENT * event = GMEVENT_Create(gsys, NULL, EVENT_DebugWifiMatchMain, sizeof(EVENT_DEBUGWIFIMATCH_WORK));
  if(GAME_COMM_NO_NULL!= GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gsys))){
    GameCommSys_ExitReq(GAMESYSTEM_GetGameCommSysPtr(gsys));
  }

  { 
    EVENT_DEBUGWIFIMATCH_WORK * dbw;

    dbw = GMEVENT_GetEventWork(event);
    dbw->gsys = gsys;
    dbw->fieldmap = fieldmap;
    dbw->mode = mode;
    dbw->ctrl = GAMEDATA_GetSaveControlWork(GAMESYSTEM_GetGameData(gsys));
  }

  return event;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�OWiFi���E�ΐ�R�[��
///	GMEVENT_CreateOverlayEventCall�֐��p�R�[���o�b�N�֐�
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_DEBUG_CallWifiMatch( GAMESYS_WORK * gsys, void* work )
{
  EV_DEBUG_WIFIMATCH* param = work;

  return EVENT_DEBUG_WifiMatch( gsys, param->fieldmap, param->mode );
}

