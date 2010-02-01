//==============================================================================
/**
 * @file    intrude_mission_field.c
 * @brief   �~�b�V�����Ńt�B�[���h�ł̂ݎg�p����c�[���ށ@��FIELDMAP�I�[�o�[���C�ɔz�u
 * @author  matsuda
 * @date    2010.01.26(��)
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
 * ���݂̃~�b�V�����󋵂𒲂ׂ�
 * @param   mission		
 * @retval  MISSION_STATUS		
 */
//==================================================================
MISSION_STATUS MISSION_FIELD_CheckStatus(MISSION_SYSTEM *mission)
{
  const MISSION_DATA *md;
  
  md = MISSION_GetRecvData(mission);
  if(md == NULL){
    return MISSION_STATUS_NULL;
  }
  if(md->ready_timer == 0){
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
