//==============================================================================
/**
 * @file    event_colosseum_battle.c
 * @brief   イベント：コロシアムバトル
 * @author  matsuda
 * @date    2009.07.22(水)
 */
//==============================================================================
#include <gflib.h>
#include "net/network_define.h"

#include "system\main.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"

#include "field/fieldmap.h"
#include "field/field_sound.h"

#include "./event_fieldmap_control.h"

//------------------------------------------------------------------
//------------------------------------------------------------------
#include "sound/pm_sndsys.h"
#include "battle/battle.h"
#include "poke_tool/monsno_def.h"

#include "poke_tool/pokeparty.h"
#include "poke_tool/poke_tool.h"

#include "net_app/union/union_types.h"
#include "net_app/union/colosseum_types.h"
#include "field/event_fldmmdl_control.h"
#include "field/event_colosseum_battle.h"
#include "gamesystem\btl_setup.h"


//============================================================================================
//============================================================================================

//----------------------------------------------------------------
// バトル用定義
extern const NetRecvFuncTable BtlRecvFuncTable[];
//----------------------------------------------------------------

FS_EXTERN_OVERLAY(battle);


//==============================================================================
//  構造体定義
//==============================================================================
typedef struct{
  GAMESYS_WORK * gsys;
  FIELDMAP_WORK * fieldmap;
  BATTLE_SETUP_PARAM para;
}EVENT_COLOSSEUM_BATTLE_WORK;



//============================================================================================
//
//    サブイベント
//
//============================================================================================
static GMEVENT_RESULT EVENT_ColosseumBattleMain(GMEVENT * event, int *  seq, void * work)
{
  EVENT_COLOSSEUM_BATTLE_WORK * cbw = work;
  GAMESYS_WORK * gsys = cbw->gsys;

  switch (*seq) {
  case 0:
    GMEVENT_CallEvent( event, EVENT_ObjPauseAll(gsys, cbw->fieldmap) );
    (*seq)++;
    break;
  case 1:
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeOut_Black(gsys, cbw->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq)++;
    break;
  case 2:
    GMEVENT_CallEvent(event, EVENT_FieldClose(gsys, cbw->fieldmap));
    (*seq)++;
    break;
  case 3:
    {
      GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );
      GFL_NET_AddCommandTable(GFL_NET_CMD_BATTLE, BtlRecvFuncTable, BTL_NETFUNCTBL_ELEMS, NULL);
      GFL_NET_TimingSyncStart(GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_BATTLE_ADD_CMD_TBL_AFTER);
      OS_TPrintf("戦闘用通信コマンドテーブルをAddしたので同期取り\n");
      (*seq) ++;
    }
    break;
  case 4:
    if(GFL_NET_IsTimingSync(GFL_NET_HANDLE_GetCurrentHandle(), UNION_TIMING_BATTLE_ADD_CMD_TBL_AFTER)){
      OS_TPrintf("戦闘用通信コマンドテーブルをAdd後の同期取り完了\n");
      (*seq) ++;
    }
    break;
  case 5:
    {
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PushPlayEventBGM( fsnd, cbw->para.musicDefault );
    }

    GAMESYSTEM_CallProc(gsys, NO_OVERLAY_ID, &BtlProcData, &cbw->para);
//    GFL_FADE_SetMasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 1);
    (*seq)++;
    break;
  case 6:
    if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
      break;
    }
    OS_TPrintf("バトル完了\n");
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );
    BATTLE_PARAM_Release( &cbw->para );
    (*seq)++;
    break;

  case 7:
    OS_TPrintf("_FIELD_OPEN\n");
    GMEVENT_CallEvent(event, EVENT_FieldOpen(gsys));
    (*seq) ++;
    break;
  case 8:
    OS_TPrintf("_FIELD_FADEIN\n");
    {
      GMEVENT* fade_event;
      fade_event = EVENT_FieldFadeIn_Black(gsys, cbw->fieldmap, FIELD_FADE_WAIT);
      GMEVENT_CallEvent(event, fade_event);
    }
    (*seq) ++;
    break;
  case 9:
    {
      GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
      FIELD_SOUND *fsnd = GAMEDATA_GetFieldSound( gdata );
      FIELD_SOUND_PopBGM( fsnd );
    }
    PMSND_FadeInBGM(60);
    return GMEVENT_RES_FINISH;
  default:
    GF_ASSERT(0);
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
GMEVENT* EVENT_ColosseumBattle(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, UNION_PLAY_CATEGORY play_category, const COLOSSEUM_BATTLE_SETUP *setup)
{
  BATTLE_SETUP_PARAM * para;
  EVENT_COLOSSEUM_BATTLE_WORK * cbw;
  BtlRule rule;
  int multi;
  GMEVENT *event;

  event = GMEVENT_Create(
    gsys, NULL, EVENT_ColosseumBattleMain, sizeof(EVENT_COLOSSEUM_BATTLE_WORK));
  cbw = GMEVENT_GetEventWork(event);
  cbw->gsys = gsys;
  cbw->fieldmap = fieldmap;
  para = &cbw->para;

  switch(play_category){
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50:         //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50_SHOOTER:         //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:       //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE_SHOOTER:       //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD:   //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD_SHOOTER:   //コロシアム
    BTL_SETUP_Single_Comm(para, GAMESYSTEM_GetGameData(gsys), 
      GFL_NET_HANDLE_GetCurrentHandle(), BTL_COMM_DS, HEAPID_PROC );
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50:         //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50_SHOOTER:         //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:       //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE_SHOOTER:       //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD:   //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD_SHOOTER:   //コロシアム
    BTL_SETUP_Double_Comm(para, GAMESYSTEM_GetGameData(gsys), 
      GFL_NET_HANDLE_GetCurrentHandle(), BTL_COMM_DS, HEAPID_PROC );
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_50:         //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_50_SHOOTER:         //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE:       //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_FREE_SHOOTER:       //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_STANDARD:   //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_TRIPLE_STANDARD_SHOOTER:   //コロシアム
    BTL_SETUP_Triple_Comm(para, GAMESYSTEM_GetGameData(gsys), 
      GFL_NET_HANDLE_GetCurrentHandle(), BTL_COMM_DS, HEAPID_PROC );
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_50:         //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_50_SHOOTER:         //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE:       //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_FREE_SHOOTER:       //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_STANDARD:   //コロシアム
  case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_ROTATION_STANDARD_SHOOTER:   //コロシアム
    //※check まだローテーションの定義が無いのでとりあえずトリプル指定
    BTL_SETUP_Triple_Comm(para, GAMESYSTEM_GetGameData(gsys), 
      GFL_NET_HANDLE_GetCurrentHandle(), BTL_COMM_DS, HEAPID_PROC );
    break;
  case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI:      //コロシアム
    BTL_SETUP_Multi_Comm(para, GAMESYSTEM_GetGameData(gsys), 
      GFL_NET_HANDLE_GetCurrentHandle(), BTL_COMM_DS, setup->standing_pos, HEAPID_PROC );
    break;
  default:
    GF_ASSERT_MSG(0, "play_category = %d\n", play_category);
    return NULL;
  }
  BATTLE_PARAM_SetPokeParty( para, setup->partyPlayer, BTL_CLIENT_PLAYER );
#if 0 //戦闘内部でシェアしあうので、ここで指定はいらない
  para->partyEnemy1 = setup->partyEnemy1;   ///< 1vs1時の敵AI, 2vs2時の１番目敵AI用
  para->partyPartner = setup->partyPartner; ///< 2vs2時の味方AI（不要ならnull）
  para->partyEnemy2 = setup->partyEnemy2;    ///< 2vs2時の２番目敵AI用（不要ならnull）
#endif
  return event;
}



