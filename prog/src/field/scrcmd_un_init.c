//======================================================================
/**
 * @file  scrcmd_un_init.c
 * @brief  �X�N���v�g�R�}���h�F���A�֘A
 * @author  Saito
 */
//======================================================================
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

#include "event_fieldmap_control.h"

#include "savedata/wifihistory.h"
#include "scrcmd_un.h"
#include "united_nations.h" 

//--------------------------------------------------------------
/**
 * ������OBJ�Z�b�g�A�b�v�̂��߂�OBJCODE��Ԃ�
 * �]�[���`�F���W���ɌĂԂ̂ŏ풓�ɒu��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdUn_GetRoomObjCode( VMHANDLE *core, void *wk )
{
  u8 obj_idx;
  u16 *ret;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata =  GAMESYSTEM_GetGameData(gsys);
  UNSV_WORK *unsv_work = GAMEDATA_GetUnsvWorkPtr(gdata);

  obj_idx = VMGetU16( core ); //0�`4
  ret = SCRCMD_GetVMWork( core, work );

  *ret = UN_GetRoomObjCode(unsv_work, obj_idx);

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �����̏��擾
 * �]�[���`�F���W���ɌĂԂ̂ŏ풓�ɒu��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdUn_GetRoomInfo( VMHANDLE *core, void *wk )
{
  u16 *val;
  u16 info_type;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata =  GAMESYSTEM_GetGameData(gsys);
  UNSV_WORK *unsv_work = GAMEDATA_GetUnsvWorkPtr(gdata);

  info_type = SCRCMD_GetVMWorkValue( core, work );
  val = SCRCMD_GetVMWork( core, work );
  
  *val = UN_GetRoomInfo(unsv_work, info_type);

  return VMCMD_RESULT_CONTINUE;
}
