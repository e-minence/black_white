//============================================================================================
/**
 * @file     event_wifibattlematch.c
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
#include "net_app/wifibattlematch.h"
#include "net_app/wifi_login.h"

//�O�����J
#include "field/event_wifibattlematch.h"



//============================================================================================
//
//    �萔
//
//============================================================================================

//-------------------------------------
///	�C�x���g���[�N
//=====================================
typedef struct 
{
  GAMESYS_WORK      * gsys;
  FIELDMAP_WORK     * fieldmap;
  void              * p_sub_wk;
  WIFIBATTLEMATCH_MODE mode;
  WIFIBATTLEMATCH_POKE  poke;
  WIFIBATTLEMATCH_BTLRULE btl_rule;
} EVENT_WIFIBTLMATCH_WORK;


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
static GMEVENT_RESULT EVENT_WifiBattleMatchMain(GMEVENT * event, int *  seq, void * work)
{
  enum{
    _FIELD_COMM,
    _FIELD_CLOSE,
    //_CALL_WIFILOGIN,
    //_WAIT_WIFILOGIN,
    _CALL_WIFIBTLMATCH,
    _WAIT_WIFIBTLMATCH,
    _WAIT_NET_END,
    _FIELD_OPEN,
    _FIELD_END
  };

  EVENT_WIFIBTLMATCH_WORK * dbw = work;
  GAMESYS_WORK * gsys = dbw->gsys;


  switch (*seq) {
  case _FIELD_COMM:
    if(GAME_COMM_NO_NULL == GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gsys)))
    {
      (*seq)++;
    }
    break;
  case _FIELD_CLOSE:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, dbw->fieldmap));
    (*seq)++;
    break;
/*  case _CALL_WIFILOGIN:
    { 
      WIFILOGIN_PARAM *p_param;
      dbw->p_sub_wk = GFL_HEAP_AllocClearMemory(HEAPID_PROC,sizeof(WIFILOGIN_PARAM));
      p_param = dbw->p_sub_wk;
      p_param->gamedata = GAMESYSTEM_GetGameData(gsys);
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
      else if( p_param->result == WIFILOGIN_RESULT_CANCEL )
      { 
        (*seq)  = _WAIT_NET_END;
      }
      GFL_HEAP_FreeMemory( dbw->p_sub_wk );
    }
    break;
*/  case _CALL_WIFIBTLMATCH:
    { 
      WIFIBATTLEMATCH_PARAM *p_param;
      dbw->p_sub_wk = GFL_HEAP_AllocClearMemory(HEAPID_PROC,sizeof(WIFIBATTLEMATCH_PARAM));
      p_param = dbw->p_sub_wk;
      p_param->mode = dbw->mode;
      p_param->p_game_data  = GAMESYSTEM_GetGameData(gsys);
      p_param->btl_rule     = dbw->btl_rule;
      p_param->poke         = dbw->poke;
      GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID(wifibattlematch_sys), &WifiBattleMaptch_ProcData, dbw->p_sub_wk );
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
 *	@brief  WiFi���E�ΐ�R�[��
 *
 *	@param	GAMESYS_WORK * gsys �Q�[���V�X�e��
 *	@param	* fieldmap          �t�B�[���h�}�b�v
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_WifiBattleMatch( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, WIFIBATTLEMATCH_MODE mode, WIFIBATTLEMATCH_POKE  poke, WIFIBATTLEMATCH_BTLRULE btl_rule )
{
  GMEVENT * event = GMEVENT_Create(gsys, NULL, EVENT_WifiBattleMatchMain, sizeof(EVENT_WIFIBTLMATCH_WORK));
  if(GAME_COMM_NO_NULL!= GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gsys))){
    GameCommSys_ExitReq(GAMESYSTEM_GetGameCommSysPtr(gsys));
  }

  { 
    EVENT_WIFIBTLMATCH_WORK * dbw;

    dbw = GMEVENT_GetEventWork(event);
    dbw->gsys = gsys;
    dbw->fieldmap = fieldmap;
    dbw->mode   = mode;
    dbw->poke   = poke;
    dbw->btl_rule   = btl_rule;
  }

  return event;
}


