//======================================================================
/**
 * @file  scrcmd_pl_boat_init.c
 * @brief  �X�N���v�g�R�}���h�F�V���D�֘A
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

#include "field_sound.h"

#include "scrcmd_pl_boat.h"

#include "pleasure_boat.h"

#include "pl_boat_def.h"

//--------------------------------------------------------------
/**
 * �����̏����擾���� 
 * @note    �]�[���`�F���W���ɌĂԂ̂ŏ풓�֐��ɂ��Ă��܂�
 * @param   core          ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPlBoat_GetRoomInfo( VMHANDLE *core, void *wk )
{
  u16 room_idx;
  u16 info_kind;
  u16 param;
  u16 *ret;

  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  PL_BOAT_WORK_PTR *ptr = GAMEDATA_GetPlBoatWorkPtr(gamedata);

  room_idx = SCRCMD_GetVMWorkValue( core, work );
  info_kind = VMGetU16( core );
  param = SCRCMD_GetVMWorkValue( core, work );
  ret = SCRCMD_GetVMWork( core, work );

  switch(info_kind){
  case PL_BOAT_ROOM_INFO_OBJ_CODE:
    *ret = PL_BOAT_GetObjCode(*ptr, room_idx);
    break;
  case PL_BOAT_ROOM_INFO_NPC_TYPE:
    *ret = PL_BOAT_GetNpcType(*ptr, room_idx);
    break;
  case PL_BOAT_ROOM_INFO_TR_ID:
    *ret = PL_BOAT_GetTrID(*ptr, room_idx);
    break;
  case PL_BOAT_ROOM_INFO_MSG:
    *ret = PL_BOAT_GetMsg(*ptr, room_idx, param);
    break;
  case PL_BOAT_ROOM_INFO_BTL_FLG:
    if ( PL_BOAT_CheckBtlFlg(*ptr, room_idx) ) *ret = PL_BOAT_TR_BTL_ALREADY;  //�����
    else *ret = PL_BOAT_TR_BTL_YET;  //����ĂȂ�
    break;
  case PL_BOAT_ROOM_INFO_BTL_TYPE:
    if ( PL_BOAT_CheckDblBtl(*ptr, room_idx) ) *ret = PL_BOAT_BTL_TYPE_DOUBLE;  //�_�u���o�g���̕���
    else *ret = PL_BOAT_BTL_TYPE_SINGLE;  //�V���O���o�g���̕���
    break;
  default:
    GF_ASSERT(0);
    *ret = 0;
  }

  return VMCMD_RESULT_CONTINUE;
}




