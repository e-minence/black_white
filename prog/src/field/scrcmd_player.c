//======================================================================
/**
 *
 * @file 	 scrcmd_player.c
 * @brief	 �X�N���v�g�R�}���h�F�v���C���[�֘A
 * @date   2009.10.09
 * @author obata GAMEFREAK inc.
 *
 */
//======================================================================

#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_player.h"


//--------------------------------------------------------------
/**
 * @brief �������𑝂₷
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdAddGold( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK*   work = (SCRCMD_WORK*)wk;
  GAMEDATA*     gdata = SCRCMD_WORK_GetGameData( work );
  PLAYER_WORK* player = GAMEDATA_GetMyPlayerWork( gdata );
  MYSTATUS*  mystatus = &player->mystatus;
  u16             val = SCRCMD_GetVMWorkValue( core, wk );

  MISC_AddGold( GAMEDATA_GetMiscWork(gdata), val );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �����������炷
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSubtractGold( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK*   work = (SCRCMD_WORK*)wk;
  GAMEDATA*     gdata = SCRCMD_WORK_GetGameData( work );
  PLAYER_WORK* player = GAMEDATA_GetMyPlayerWork( gdata );
  MYSTATUS*  mystatus = &player->mystatus;
  u16             val = SCRCMD_GetVMWorkValue( core, wk );

  MISC_SubGold( GAMEDATA_GetMiscWork(gdata), val );

  OBATA_Printf( "EvCmdSubtractGold : %d\n", MISC_GetGold( GAMEDATA_GetMiscWork(gdata) ) );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief �������̕s���`�F�b�N
 * @param	core		���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCheckGold( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK*   work = (SCRCMD_WORK*)wk;
  u16*         ret_wk = SCRCMD_GetVMWork( core, work );
  u16             val = SCRCMD_GetVMWorkValue( core, wk );
  GAMEDATA*     gdata = SCRCMD_WORK_GetGameData( work );
  PLAYER_WORK* player = GAMEDATA_GetMyPlayerWork( gdata );
  MYSTATUS*  mystatus = &player->mystatus;
  u32            gold = MISC_GetGold( GAMEDATA_GetMiscWork(gdata) );

  if( val <= gold ) *ret_wk = TRUE;
  else              *ret_wk = FALSE; 

  OBATA_Printf( "EvCmdCheckGold : %d\n", *ret_wk );
  return VMCMD_RESULT_CONTINUE;
}
