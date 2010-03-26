//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		event_gts.c
 *	@brief  GTS�ڑ��C�x���g
 *	@author	Toru=Nagihashi
 *	@data		2010.03.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

//�t�B�[���h
#include "field/fieldmap.h"
#include "sound/pm_sndsys.h"
#include "event_fieldmap_control.h"

//�v���Z�X
#include "net_app/worldtrade.h"

//�O�����J
#include "event_gts.h"

FS_EXTERN_OVERLAY(worldtrade);

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
/// GTS�p���[�N
//=====================================
typedef struct
{
  GAMESYS_WORK        *p_gamesys;
  FIELDMAP_WORK       *p_field;
  WORLDTRADE_PARAM    param;
  BOOL                is_debug;
} EVENT_GTS_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
static GMEVENT_RESULT Event_CallGtsMain( GMEVENT *p_event, int *p_seq, void *p_wk_adrs );
//----------------------------------------------------------------------------
/**
 *	@brief  GTS�ڑ��C�x���g
 *
 *	@param	GAMESYS_WORK * gsys �Q�[���V�X�e��
 *	@param	* fieldmap          �t�B�[���h�}�b�v
 *	@param  is_debug            �f�o�b�O���ǂ���
 */
//-----------------------------------------------------------------------------
GMEVENT* EVENT_Gts( GAMESYS_WORK * p_gamesys, FIELDMAP_WORK * p_fieldmap, BOOL is_debug )
{ 
  GAMEDATA        *p_gamedata      = GAMESYSTEM_GetGameData(p_gamesys);

  GMEVENT         *p_event;

  //�C�x���g�쐬
  p_event = GMEVENT_Create( p_gamesys, NULL, Event_CallGtsMain, sizeof(EVENT_GTS_WORK) );


  //�r�[�R���ؒf
  if(GAME_COMM_NO_NULL != GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(p_gamesys))){
    GameCommSys_ExitReq(GAMESYSTEM_GetGameCommSysPtr(p_gamesys));
  }

  //������
  { 
    EVENT_GTS_WORK  *p_wk;
    p_wk    = GMEVENT_GetEventWork( p_event );
    GFL_STD_MemClear( p_wk, sizeof(EVENT_GTS_WORK) );

    p_wk->p_gamesys  = p_gamesys;
    p_wk->p_field    = p_fieldmap;

#ifdef PM_DEBUG
    p_wk->is_debug   = is_debug;
#endif //PM_DEBUG

    p_wk->param.savedata         = GAMEDATA_GetSaveControlWork( p_gamedata );
    p_wk->param.worldtrade_data  = SaveData_GetWorldTradeData(p_wk->param.savedata);
    p_wk->param.systemdata       = SaveData_GetSystemData(p_wk->param.savedata);
    p_wk->param.myparty          = SaveData_GetTemotiPokemon(p_wk->param.savedata);
    p_wk->param.mybox            = GAMEDATA_GetBoxManager(p_gamedata);
    p_wk->param.zukanwork        = GAMEDATA_GetZukanSave( p_gamedata );
    p_wk->param.wifilist         = GAMEDATA_GetWiFiList(p_gamedata);
    p_wk->param.wifihistory      = SaveData_GetWifiHistory(p_wk->param.savedata);
    p_wk->param.mystatus         = SaveData_GetMyStatus(p_wk->param.savedata);
    p_wk->param.config           = SaveData_GetConfig(p_wk->param.savedata);
    p_wk->param.record           = SaveData_GetRecord(p_wk->param.savedata);
    p_wk->param.myitem           = GAMEDATA_GetMyItem( p_gamedata );
    p_wk->param.gamesys          = p_gamesys;

    p_wk->param.zukanmode        = ZUKANSAVE_GetZenkokuZukanFlag( p_wk->param.zukanwork );
    p_wk->param.profileId        = WifiList_GetMyGSID( p_wk->param.wifilist );
    p_wk->param.contestflag      = FALSE;
    p_wk->param.connect          = 0;
  }

  return p_event;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �ڑ��C�x���g���C��
 *
 *	@param	GMEVENT *p_event  �C�x���g
 *	@param	*p_seq      �V�[�P���X
 *	@param	*p_wk_adrs  ���[�N�A�h���X 
 *
 *	@return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static GMEVENT_RESULT Event_CallGtsMain( GMEVENT *p_event, int *p_seq, void *p_wk_adrs )
{
  enum
  {
    SEQ_WAIT_COMM,
    SEQ_FADEIN,
    SEQ_FLDCLOSE,
    SEQ_CALL_GTS,
    SEQ_FLDCOPEN,
    SEQ_FADEOUT,
    SEQ_EXIT,
  };

  EVENT_GTS_WORK  *p_wk = p_wk_adrs;

  switch(*p_seq )
  {
  case SEQ_WAIT_COMM:
    if(GAME_COMM_NO_NULL == GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(p_wk->p_gamesys)))
    {
      *p_seq  = SEQ_FADEIN;
    }
    break;

  case SEQ_FADEIN:
    if(p_wk->is_debug)
    { 
      GMEVENT_CallEvent( p_event,
        EVENT_FieldFadeOut_Black(p_wk->p_gamesys,p_wk->p_field,FIELD_FADE_WAIT) );
    }
    *p_seq  = SEQ_FLDCLOSE;
    break;
  case SEQ_FLDCLOSE:
    GMEVENT_CallEvent( p_event, EVENT_FieldClose(p_wk->p_gamesys,p_wk->p_field));
    *p_seq  = SEQ_CALL_GTS;
    break;

  case SEQ_CALL_GTS:
    GAMESYSTEM_CallProc(p_wk->p_gamesys, FS_OVERLAY_ID(worldtrade), &WorldTrade_ProcData, &p_wk->param );
    *p_seq  = SEQ_FLDCOPEN;
    break;

  case SEQ_FLDCOPEN:
    if (GAMESYSTEM_IsProcExists(p_wk->p_gamesys) == GFL_PROC_MAIN_NULL)
    {
      GMEVENT_CallEvent(p_event, EVENT_FieldOpen(p_wk->p_gamesys));
      *p_seq  = SEQ_FADEOUT;
    }
    break;
  case SEQ_FADEOUT:
    if( p_wk->is_debug )
    { 
      GMEVENT_CallEvent( p_event,
        EVENT_FieldFadeIn_Black(p_wk->p_gamesys,p_wk->p_field,FIELD_FADE_WAIT) );
    }
    *p_seq  = SEQ_EXIT;
    break;

  case SEQ_EXIT:
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE ;
}
