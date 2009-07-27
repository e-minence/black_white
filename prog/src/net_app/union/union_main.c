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
#include "colosseum.h"


void Union_Main(GAME_COMM_SYS_PTR game_comm, FIELD_MAIN_WORK *fieldmap)
{
  UNION_SYSTEM_PTR unisys;
  
  if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_UNION){
    return;
  }
  
  unisys = GameCommSys_GetAppWork(game_comm);
  GF_ASSERT(unisys != NULL);
  
  //PROCがフィールドの時のみ実行する処理
  if(Union_FieldCheck(unisys) == TRUE){
    switch(unisys->my_situation.play_category){
    case UNION_PLAY_CATEGORY_UNION:
    case UNION_PLAY_CATEGORY_TALK:
      if(UnionSubProc_IsExits(unisys) == FALSE){  //サブPROCが無い時のみ実行
        //データ受信によるイベント起動
        UnionReceive_BeaconInterpret(unisys);
        
        //キー操作によるイベント起動
        UnionOneself_Update(unisys, fieldmap);
        
        //OBJ反映
        UNION_CHAR_Update(unisys, unisys->uniparent->game_data);
        
        //下画面反映
      }
      break;
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_50:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_SINGLE_STANDARD:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_50:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_FREE:
    case UNION_PLAY_CATEGORY_COLOSSEUM_1VS1_DOUBLE_STANDARD:
    case UNION_PLAY_CATEGORY_COLOSSEUM_MULTI:
      if(UnionSubProc_IsExits(unisys) == FALSE){  //サブPROCが無い時のみ実行
        //キー操作によるイベント起動
        UnionOneself_Update(unisys, fieldmap);
      }
      break;
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
  if(GAMESYSTEM_CheckFieldMapWork(unisys->uniparent->gsys) == TRUE){
    if(FIELDMAP_IsReady(GAMESYSTEM_GetFieldMapWork(unisys->uniparent->gsys)) == TRUE){
      return TRUE;
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
void UnionMain_Callback_FieldCreate(void *pwk, void *app_work, FIELD_MAIN_WORK *fieldWork)
{
  UNION_PARENT_WORK *uniparent = pwk;
  UNION_SYSTEM_PTR unisys = app_work;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;
  
  if(unisys->player_pause == TRUE){
    FIELDMAP_CTRL_GRID_SetPlayerPause( fieldWork, TRUE );
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
void UnionMain_Callback_FieldDelete(void *pwk, void *app_work, FIELD_MAIN_WORK *fieldWork)
{
  UNION_PARENT_WORK *uniparent = pwk;
  UNION_SYSTEM_PTR unisys = app_work;
  COLOSSEUM_SYSTEM_PTR clsys = unisys->colosseum_sys;

  if(clsys != NULL){
    if(clsys->colosseum_leave == TRUE){ //コロシアム退出処理
      Colosseum_ExitSystem(unisys->colosseum_sys);
      unisys->colosseum_sys = NULL;
    }
    else{
      if(clsys->comm_ready == TRUE){
        CommPlayer_Push(clsys->cps);
      }
    }
  }
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
