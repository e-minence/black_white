//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �X�N���v�g�R�}���h�F�r�[�R���ʐM/G�p���[�֘A
 * @file   scrcmd_beacon.c
 * @author iwasawa
 * @date   2010.02.15
 */
//////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/main.h"

#include "gamesystem/game_data.h"
#include "gamesystem/game_beacon.h"
#include "gamesystem/g_power.h"

#include "script_def.h"
#include "scrcmd.h"

//--------------------------------------------------------------------
/**
 * @brief �r�[�R�����M���N�G�X�g�Ăяo�� 
 *
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT:w
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdBeaconSetRequest( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*   work     = (SCRCMD_WORK*)wk;
  u16            beacon_id  = SCRCMD_GetVMWorkValue( core, work );
  u16            value      = SCRCMD_GetVMWorkValue( core, work );
  GAMESYS_WORK*  gsys     = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*      gamedata = GAMESYSTEM_GetGameData( gsys );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SCRIPT_WORK*   sc       = SCRCMD_WORK_GetScriptWork( work );

  switch( beacon_id ){
  case SCR_BEACON_SET_REQ_ITEM_GET:
    GAMEBEACON_Set_SpItemGet( value );
    break;
  }
  return VMCMD_RESULT_CONTINUE;
}


