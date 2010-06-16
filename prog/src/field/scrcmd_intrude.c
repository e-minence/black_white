//==============================================================================
/**
 * @file    scrcmd_intrude.c
 * @brief   �X�N���v�g�R�}���h�F�N��
 * @author  matsuda
 * @date    2009.10.26(��)
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_intrude.h"

#include "field_comm\intrude_types.h"
#include "field_comm\intrude_work.h"
#include "field_comm\intrude_comm_command.h"
#include "field/field_comm/intrude_monolith.h"
#include "field/field_comm/intrude_mission.h"
#include "field/field_comm/intrude_message.h"
#include "field/field_comm/intrude_field.h"

#include "event_symbol.h"
#include "symbol_map.h"

#include "message.naix"
#include "msg/msg_invasion.h"
#include "msg/msg_mission_msg.h"


//==============================================================================
//  �\���̒�`
//==============================================================================
///�C�x���g���䃏�[�N�F�~�b�V�����J�n
typedef struct{
  INTRUDE_EVENT_MSGWORK iem;  ///<�v�����g����
  BOOL error;                 ///<TRUE:�G���[����
  u16 zone_id;                ///<�]�[��ID
  u8 monolith_type;           ///<���m���X�^�C�v
  u8 padding;
}EVENT_MISSION_START;




//==================================================================
/**
 * �p���X���̒ʐM���葤�ɂ���NPC���샂�f����o�^
 *
 * @param   core		���z�}�V������\���̂ւ̃|�C���^
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 */
//==================================================================
VMCMD_RESULT EvCmdIntrudePalaceMmdlSetting( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  
  IntrudeField_PalaceMMdlAllAdd(fieldWork);
  
  return VMCMD_RESULT_CONTINUE;
}

//==================================================================
/**
 * �p���X���̏����}�b�v�A���Z�b�e�B���O
 *
 * @param   core		���z�}�V������\���̂ւ̃|�C���^
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 */
//==================================================================
VMCMD_RESULT EvCmdIntrudeConnectMapSetting( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  INTRUDE_COMM_SYS_PTR intcomm = NULL;
  
  if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_INVASION){
    intcomm = GameCommSys_GetAppWork(game_comm);
  }

  if(intcomm != NULL){
    IntrudeField_ConnectMapInit(fieldWork, gsys, intcomm);
    IntrudeField_ConnectMap(fieldWork, gsys, intcomm);
  }
  else{
    IntrudeField_ConnectMapNum(fieldWork, 1); //��ʐM����1��
  }
  
  MONOLITH_AddConnectAllMap(fieldWork);

  return VMCMD_RESULT_CONTINUE;
}

