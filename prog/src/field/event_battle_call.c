//=============================================================================
/**
 *
 *	@file		event_battle_call.c
 *	@brief  イベント：バトル呼び出し
 *	@author	hosaka genya
 *	@data		2010.01.22
 *
 */
//=============================================================================
#include <gflib.h>
#include "net/network_define.h"

#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap.h"
#include "field/field_sound.h"

#include "./event_fieldmap_control.h"

#include "sound/pm_sndsys.h"
#include "battle/battle.h"
#include "poke_tool/monsno_def.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"

#include "net_app/union/union_types.h"
#include "net_app/union/colosseum_types.h"
#include "field/event_fldmmdl_control.h"
#include "field/event_colosseum_battle.h"

#include "gamesystem/btl_setup.h"

#include "field/event_battle_call.h"


static GFL_PROC_RESULT CommBattleCallProc( GFL_PROC * proc, int *seq, void *pwk, void* mywk );

const GFL_PROC_DATA CommBattleCommProcData = 
{ 
  CommBattleCallProc,
};

//-----------------------------------------------------------------------------
/**
 *	@brief  通信バトル呼び出しPROC
 *
 *	@param	GFL_PROC *proc
 *	@param	*seq
 *	@param	pwk
 *	@param	mywk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT CommBattleCallProc(  GFL_PROC *proc, int *seq, void* pwk, void* mywk )
{
  enum
  {
    SEQ_CALL_START_DEMO = 0,  ///< バトル前デモ呼び出し
    SEQ_BATTLE_TIMING_INIT,
    SEQ_BATTLE_TIMING_WAIT,
    SEQ_BATTLE_INIT,
    SEQ_BATTLE_END,
    SEQ_CALL_END_DEMO,        ///< バトル後デモ呼び出し
    SEQ_CALL_BTL_REC_SEL,     ///< 通信対戦後の録画選択画面
    SEQ_BGM_POP,
    SEQ_END
  };

  EVENT_BATTLE_CALL_WORK * bcw = pwk;
  GAMESYS_WORK * gsys = bcw->gsys;

  switch (*seq) {
  case SEQ_CALL_START_DEMO:
    // 通信バトル前デモ呼び出し
    {
      // マルチバトル判定
      if( bcw->btl_setup_prm->multiMode == 0 )
      {
        bcw->demo_prm->type = COMM_BTL_DEMO_TYPE_NORMAL_START;
      }
      else
      {
        bcw->demo_prm->type = COMM_BTL_DEMO_TYPE_MULTI_START;
      }
      GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID( comm_btl_demo ), &CommBtlDemoProcData, &bcw->demo_prm);
    }
    (*seq)++;
    break;

  case SEQ_BATTLE_TIMING_INIT:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }
    {
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
      GFL_NET_AddCommandTable(GFL_NET_CMD_BATTLE, BtlRecvFuncTable, BTL_NETFUNCTBL_ELEMS, NULL);
      GFL_NET_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), EVENT_BATTLE_ADD_CMD_TBL_TIMING);
      OS_TPrintf("戦闘用通信コマンドテーブルをAddしたので同期取り\n");
      (*seq) ++;
    }
    break;
  case SEQ_BATTLE_TIMING_WAIT:
    if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(), EVENT_BATTLE_ADD_CMD_TBL_TIMING)){
      OS_TPrintf("戦闘用通信コマンドテーブルをAdd後の同期取り完了\n");
      (*seq) ++;
    }
    break;
  case SEQ_BATTLE_INIT:
//    GMEVENT_CallEvent(event, EVENT_FSND_PushPlayNextBGM(gsys, bcw->btl_setup_prm->musicDefault, FSND_FADE_SHORT, FSND_FADE_NONE)); 
    GAMESYSTEM_CallProc(gsys, NO_OVERLAY_ID, &BtlProcData, bcw->btl_setup_prm);
    (*seq)++;
    break;
  case SEQ_BATTLE_END:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }
    OS_TPrintf("バトル完了\n");
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
    (*seq)++;
    break;
  case SEQ_CALL_END_DEMO:
    // 通信バトル後デモ呼び出し
    {
      // マルチバトル判定
      if( bcw->btl_setup_prm->multiMode == 0 )
      {
        bcw->demo_prm->type = COMM_BTL_DEMO_TYPE_NORMAL_START;
      }
      else
      {
        bcw->demo_prm->type = COMM_BTL_DEMO_TYPE_MULTI_START;
      }

      // 勝敗設定
      switch( bcw->btl_setup_prm->result )
      {
        case BTL_RESULT_WIN :
          bcw->demo_prm->result = COMM_BTL_DEMO_RESULT_WIN;
          break;
        case BTL_RESULT_LOSE :
          bcw->demo_prm->result = COMM_BTL_DEMO_RESULT_LOSE;
          break;
        case BTL_RESULT_DRAW :
          bcw->demo_prm->result = COMM_BTL_DEMO_RESULT_DRAW;
          break;
        default : GF_ASSERT(0);
      }

      GAMESYSTEM_CallProc(gsys, FS_OVERLAY_ID( comm_btl_demo ), &CommBtlDemoProcData, &bcw->demo_prm);
      (*seq)++;
    }
    break;
  case SEQ_CALL_BTL_REC_SEL:
    {
      // 通信相手と自分のROMのサーバーバージョンを比較する
      BOOL b_rec = TRUE;  // TRUEのとき、「通信相手 <= 自分」となり録画できる。
      u8 trainer_num = ( bcw->btl_setup_prm->multiMode == 0 ) ? (2) : (4);
      u8 my_version = bcw->demo_prm->trainer_data[COMM_BTL_DEMO_TRDATA_A].server_version;
      int i; 
      for( i=COMM_BTL_DEMO_TRDATA_B; i<trainer_num; i++ )
      {
        u8 other_version = bcw->demo_prm->trainer_data[i].server_version;
        if( other_version > my_version )
        {
          b_rec = FALSE;
          break;
        }
      }
      // 通信対戦後の録画選択画面へ移行(録画しない人も移行します)
      {
        bcw->btl_rec_sel_param.gamedata  = GAMESYSTEM_GetGameData( gsys );
        bcw->btl_rec_sel_param.b_rec     = b_rec;
        GAMESYSTEM_CallProc( gsys, FS_OVERLAY_ID( btl_rec_sel ), &BTL_REC_SEL_ProcData, &bcw->btl_rec_sel_param );
      }
      (*seq)++;
    }
    break;
  case SEQ_BGM_POP:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }
    {
      // BGM復帰
//      GMEVENT_CallEvent(event, EVENT_FSND_PopBGM(gsys, FSND_FADE_SHORT, FSND_FADE_NORMAL));
      (*seq) ++;
    }
    break;
  case SEQ_END:
    return GFL_PROC_RES_FINISH;
  default:
    GF_ASSERT(0);
    break;
  }

  return GFL_PROC_RES_CONTINUE;
}

//============================================================================================
//============================================================================================

//----------------------------------------------------------------
// バトル用定義
extern const NetRecvFuncTable BtlRecvFuncTable[];
//----------------------------------------------------------------

FS_EXTERN_OVERLAY(battle);

//============================================================================================
//
//    サブイベント
//
//============================================================================================
//==================================================================
/**
 * イベント作成：通信バトル呼び出し
 *
 * @param   gsys		
 * @param   para		
 * @param   demo_prm		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_CommBattle(GAMESYS_WORK * gsys, BATTLE_SETUP_PARAM *btl_setup_prm, COMM_BTL_DEMO_PARAM *demo_prm)
{
  EVENT_BATTLE_CALL_WORK *bcw;
  GMEVENT * event;

  event = GMEVENT_Create( gsys, NULL, EVENT_CommBattleMain, sizeof(EVENT_BATTLE_CALL_WORK) );
  bcw = GMEVENT_GetEventWork(event);
  bcw->gsys = gsys;
  bcw->btl_setup_prm = btl_setup_prm;
  bcw->demo_prm = demo_prm;
  
  return event;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  イベント：通信バトル呼び出し
 *
 *	@param	* event
 *	@param	*  seq
 *	@param	* work
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
GMEVENT_RESULT EVENT_CommBattleMain(GMEVENT * event, int *  seq, void * work)
{
  EVENT_BATTLE_CALL_WORK * bcw = work;
  GAMESYS_WORK * gsys = bcw->gsys;

  switch (*seq) 
  {
  case 0:
    GAMESYSTEM_CallProc(gsys, NULL, &CommBattleCommProcData, bcw);
    (*seq)++;
    break;

  case 1:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }
    return GMEVENT_RES_FINISH;
  }
  
  return GMEVENT_RES_CONTINUE;
}

