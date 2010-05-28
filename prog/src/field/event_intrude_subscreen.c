//==============================================================================
/**
 * @file    event_intrude_subscreen.c
 * @brief   �N���ɂ��T�u�X�N���[���C�x���g
 * @author  matsuda
 * @date    2009.10.18(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "field/field_subscreen.h"
#include "event_intrude_subscreen.h"
#include "fieldmap.h"
#include "event_mapchange.h"
#include "print/wordset.h"
#include "field_comm/intrude_types.h"
#include "field_comm/intrude_main.h"
#include "field_comm/intrude_work.h"
#include "field_comm/intrude_mission.h"
#include "field_comm/intrude_message.h"
#include "field_comm/intrude_field.h"
#include "msg/msg_invasion.h"
#include "msg/msg_mission_msg.h"
#include "fieldmap/zone_id.h"





//==============================================================================
//  ���[�v�C�x���g
//==============================================================================
//==================================================================
/**
 * �N���ɂ�郏�[�v�C�x���g�N��
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   partner     TRUE:���͎�(�\�t�B�[���h����p���X�֍s�����̂ݎg�p)
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_IntrudeTownWarp(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, u16 zone_id, BOOL partner)
{
  GMEVENT * event;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  VecFx32 pos;
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  int palace_area;
  
  if(IntrudeField_GetPalaceTownZoneID(zone_id, &pos) == FALSE){
    return NULL;
  }

  //���t�B�[���h�ȊO����A�p���X�֔�ԏꍇ�A��p�̃C�x���g���N��(�߂����L�^�A���[�v�s����)
  if (GAMEDATA_GetIntrudeReverseArea(gamedata) == FALSE && zone_id == ZONE_ID_PALACE01 ){
    event = EVENT_ChangeMapFldToPalace( gsys, zone_id, &pos, partner );
  }
  else{
    event = EVENT_ChangeMapPalace_to_Palace( gsys, zone_id, &pos );
  }
  return event;
}

//==================================================================
/**
 * �N���ɂ��T�u�X�N���[���؂�ւ��C�x���g�N��
 *
 * @param   gsys		
 * @param   fieldWork		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * EVENT_IntrudePlayerWarp(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, int player_netid)
{
  GMEVENT * event;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  ZONEID warp_zone_id;
  VecFx32 pos;
	INTRUDE_COMM_SYS_PTR intcomm;
  const PALACE_ZONE_SETTING *zonesetting;
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL || (intcomm->recv_profile & (1 << player_netid)) == 0){
    return NULL;
  }
  
  warp_zone_id = intcomm->intrude_status[player_netid].zone_id;
  zonesetting = IntrudeField_GetZoneSettingData(warp_zone_id);
  if(zonesetting == NULL){
    return NULL;
  }
  warp_zone_id = zonesetting->warp_zone_id;
  
  pos = intcomm->intrude_status[player_netid].player_pack.pos;
  pos.z += 32 << FX32_SHIFT;
  event = EVENT_ChangeMapPos(gsys, fieldWork, warp_zone_id, &pos, DIR_UP, FALSE);
  return event;
}


