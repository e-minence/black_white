//==============================================================================
/**
 * @file    intrude_mission_field.c
 * @brief   ミッションでフィールドでのみ使用するツール類　※FIELDMAPオーバーレイに配置
 * @author  matsuda
 * @date    2010.01.26(火)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "intrude_types.h"
#include "intrude_mission.h"
#include "intrude_comm_command.h"
#include "msg/msg_invasion.h"
#include "intrude_main.h"
#include "intrude_work.h"
#include "print/wordset.h"

#include "mission.naix"
#include "intrude_mission_field.h"




//==================================================================
/**
 * 現在のミッション状況を調べる
 * @param   mission		
 * @retval  MISSION_STATUS		
 */
//==================================================================
MISSION_STATUS MISSION_FIELD_CheckStatus(MISSION_SYSTEM *mission)
{
  const MISSION_DATA *md;
  BOOL recv_result;
  
  recv_result = MISSION_CheckRecvResult(mission);
  if(recv_result == TRUE){
    return MISSION_STATUS_RESULT;
  }
  
  md = MISSION_GetRecvData(mission);
  if(md == NULL){
    return MISSION_STATUS_NULL;
  }
  if(md->variable.ready_timer == 0){
    if(mission->mine_entry == TRUE){
      return MISSION_STATUS_EXE;
    }
    return MISSION_STATUS_NOT_ENTRY;
  }
  if(mission->mine_entry == TRUE){
    return MISSION_STATUS_READY;
  }
  return MISSION_STATUS_NOT_ENTRY;
}

//==================================================================
/**
 * 指定NetIDのTALK_TYPEを取得する
 *
 * @param   intcomm		
 * @param   net_id		
 *
 * @retval  TALK_TYPE		
 */
//==================================================================
TALK_TYPE MISSION_FIELD_GetTalkType(INTRUDE_COMM_SYS_PTR intcomm, NetID net_id)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  INTRUDE_STATUS *intstatus;
  
  if(net_id == GAMEDATA_GetIntrudeMyID(gamedata)){
    intstatus = &intcomm->intrude_status_mine;
  }
  else{
    intstatus = &intcomm->intrude_status[net_id];
  }
  
  switch(intstatus->disguise_no){
  case DISGUISE_NO_NULL:      //変装無し
    {
      GAMEDATA *gamedata = GameCommSys_GetGameData( intcomm->game_comm );
      MYSTATUS *myst = GAMEDATA_GetMyStatusPlayer( gamedata, net_id );
      if(MyStatus_GetMySex(myst) == PM_MALE){
        return TALK_TYPE_MAN;
      }
      return TALK_TYPE_WOMAN;
    }
    break;
  case DISGUISE_NO_NORMAL:    //パレス標準の変装姿
  default:
    return intstatus->disguise_type;
  }
}
