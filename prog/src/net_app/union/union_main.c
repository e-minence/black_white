//==============================================================================
/**
 * @file    union_main.c
 * @brief   ユニオンルーム：常駐物
 * @author  matsuda
 * @date    2009.07.02(木)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_comm.h"
#include "union_types.h"
#include "savedata/mystatus.h"
#include "net_app/union/union_main.h"
#include "union_local.h"
#include "union_receive.h"
#include "union_chara.h"
#include "union_oneself.h"
#include "union_subproc.h"
#include "field/fieldmap_ctrl_grid.h"
#include "field/fieldmap_call.h"  //FIELDMAP_IsReady
#include "colosseum.h"
#include "gamesystem/game_beacon.h"
#include "field/zonedata.h"


//--------------------------------------------------------------
//  オーバーレイ定義
//--------------------------------------------------------------
//FS_EXTERN_OVERLAY(union_room);



//==================================================================
/**
 * ユニオン通信を起動する
 *
 * @param   gsys		
 */
//==================================================================
void UNION_CommBoot(GAMESYS_WORK *gsys)
{
  GAME_COMM_SYS_PTR game_comm;
  GAMEDATA *gdata;
  
  game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  gdata = GAMESYSTEM_GetGameData( gsys );
  
  GF_ASSERT_HEAVY(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_NULL);

  {
    UNION_PARENT_WORK *upw;
    
    OS_TPrintf("union_system heap size = 0x%x\n", sizeof(UNION_PARENT_WORK));
    upw = GFL_HEAP_AllocClearMemory(HEAPID_APP_CONTROL, sizeof(UNION_PARENT_WORK));
    upw->mystatus = GAMEDATA_GetMyStatus(gdata);
    upw->game_comm = game_comm;
    upw->game_data = gdata;
    upw->gsys = gsys;
    GameCommSys_Boot(game_comm, GAME_COMM_NO_UNION, upw);
	}

  GAMEBEACON_Set_UnionIn();
}

//==================================================================
/**
 * ユニオン更新処理
 *
 * @param   game_comm		
 * @param   fieldmap		
 */
//==================================================================
void Union_Main(GAME_COMM_SYS_PTR game_comm, FIELDMAP_WORK *fieldmap)
{
  UNION_SYSTEM_PTR unisys;
  
  if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_UNION){
    return;
  }
  
  unisys = GameCommSys_GetAppWork(game_comm);
  GF_ASSERT_HEAVY(unisys != NULL);

  if(GAMESYSTEM_GetFieldCommErrorReq(FIELDMAP_GetGameSysWork(fieldmap)) == TRUE){
    return; //フィールドに対して通信エラーリクエストが発生している場合はここで終了
  }

  if(GAMESYSTEM_EVENT_IsExists(unisys->uniparent->gsys) == TRUE){
    return; //イベント起動中はここで終了
  }
  
  //PROCがフィールドの時のみ実行する処理
  if(Union_FieldCheck(unisys) == TRUE){
    if(unisys->my_situation.play_category >= UNION_PLAY_CATEGORY_COLOSSEUM_START
        && unisys->my_situation.play_category <= UNION_PLAY_CATEGORY_COLOSSEUM_END){
      if(UnionSubProc_IsExits(unisys) == FALSE){  //サブPROCが無い時のみ実行
        //キー操作によるイベント起動
        UnionOneself_Update(unisys, fieldmap);
      }
    }
    else{
      switch(unisys->my_situation.play_category){
      case UNION_PLAY_CATEGORY_UNION:
      case UNION_PLAY_CATEGORY_TALK:
        if(UnionSubProc_IsExits(unisys) == FALSE){  //サブPROCが無い時のみ実行
          //データ受信によるイベント起動
          UnionReceive_BeaconInterpret(unisys);
          
          //キー操作によるイベント起動
          UnionOneself_Update(unisys, fieldmap);
          
          //OBJ反映
          UNION_CHAR_Update(unisys, unisys->uniparent->game_data, fieldmap);
          
          //下画面反映
        }
        break;
      }
    }
  }
}

//==================================================================
/**
 * PROCがフィールドの状態か調べる
 *
 * @param   unisys		
 *
 * @retval  BOOL		TRUE:フィールドである
 */
//==================================================================
BOOL Union_FieldCheck(UNION_SYSTEM_PTR unisys)
{
  if(unisys->overlay_load == TRUE){
    if(GAMESYSTEM_CheckFieldMapWork(unisys->uniparent->gsys) == TRUE){
      if(FIELDMAP_IsReady(GAMESYSTEM_GetFieldMapWork(unisys->uniparent->gsys)) == TRUE){
        return TRUE;
      }
    }
  }
  return FALSE;
}

//==================================================================
/**
 * フィールド作成時に呼ばれるコールバック
 *
 * @param   unisys		
 */
//==================================================================
void UnionMain_Callback_FieldCreate(void *pwk, void *app_work, FIELDMAP_WORK *fieldWork)
{
  UNION_PARENT_WORK *uniparent = pwk;
  UNION_SYSTEM_PTR unisys = app_work;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  
#if 0
  if(unisys->overlay_load == FALSE){
    GFL_OVERLAY_Load( FS_OVERLAY_ID( union_room ) );
    unisys->overlay_load = TRUE;
    OS_TPrintf("オーバーレイLoad : union_room\n");
  }
#else
  unisys->overlay_load = TRUE;
#endif
  
  if(unisys->player_pause == TRUE){
    FIELDMAP_CTRL_GRID_SetPlayerPause( fieldWork, TRUE );
  }
  
  if(ZONEDATA_IsUnionRoom( FIELDMAP_GetZoneID( fieldWork ) ) == TRUE){//ユニオンルーム内でのみ実行
    UNION_CHAR_MarkRecover(unisys, GAMESYSTEM_GetGameData(unisys->uniparent->gsys), fieldWork);
  }
  
  if(clsys != NULL && clsys->comm_ready == TRUE){
    CommPlayer_Pop(clsys->cps);
  }
}

//==================================================================
/**
 * フィールド削除時に呼ばれるコールバック
 *
 * @param   unisys		
 */
//==================================================================
void UnionMain_Callback_FieldDelete(void *pwk, void *app_work, FIELDMAP_WORK *fieldWork)
{
  UNION_PARENT_WORK *uniparent = pwk;
  UNION_SYSTEM_PTR unisys = app_work;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;

  if(clsys != NULL){
    if(clsys->colosseum_leave == TRUE){ //コロシアム退出処理
      Colosseum_ExitSystem(unisys->colosseum_sys, unisys->uniparent->gsys);
      unisys->colosseum_sys = NULL;
    }
    else{
      if(clsys->comm_ready == TRUE){
        CommPlayer_Push(clsys->cps);
      }
    }
  }

#if 0
  if(unisys->overlay_load == TRUE){
    GFL_OVERLAY_Unload( FS_OVERLAY_ID( union_room ) );
    unisys->overlay_load = FALSE;
    OS_TPrintf("オーバーレイUnload : union_room\n");
  }
#else
  unisys->overlay_load = FALSE;
#endif
}

//==================================================================
/**
 * ユニオン終了リクエストフラグの取得
 *
 * @param   unisys		
 *
 * @retval  BOOL		
 */
//==================================================================
BOOL UnionMain_GetFinishReq(UNION_SYSTEM_PTR unisys)
{
  return unisys->finish;
}

//==================================================================
/**
 * ユニオン終了処理実行中フラグの取得
 *
 * @param   unisys		
 *
 * @retval  BOOL		
 */
//==================================================================
BOOL UnionMain_GetFinishExe(UNION_SYSTEM_PTR unisys)
{
  return unisys->finish_exe;
}

//==================================================================
/**
 * アピール番号をセット
 *
 * @param   unisys		
 * @param   appeal_no		アピール番号
 */
//==================================================================
void UnionMain_SetAppealNo(UNION_SYSTEM_PTR unisys, UNION_APPEAL appeal_no)
{
  unisys->my_situation.appeal_no = appeal_no;
}

//==================================================================
/**
 * アピール番号をセット
 *
 * @param   unisys		
 * @param   appeal_no		アピール番号
 */
//==================================================================
UNION_APPEAL UnionMain_GetAppealNo(UNION_SYSTEM_PTR unisys)
{
  return unisys->my_situation.appeal_no;
}

//==================================================================
/**
 * 自機が外部イベントを発動出来るフリーな状態か調べる
 * @param   gsys		
 * @retval  BOOL		TRUE:完全にフリー　FALSE:フリーではない
 */
//==================================================================
BOOL UnionMain_CheckPlayerFreeMode(GAMESYS_WORK *gsys)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  UNION_SYSTEM_PTR unisys = GameCommSys_GetAppWork(game_comm);
  
  if(unisys == NULL){
    return FALSE;
  }
  if((unisys->my_situation.union_status == UNION_STATUS_NORMAL
      || unisys->my_situation.union_status == UNION_STATUS_COLOSSEUM_NORMAL)
      && unisys->player_pause == FALSE){
    return TRUE;
  }
  return FALSE;
}
