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
#include "gamesystem/game_event.h"
#include "gamesystem/g_power.h"

#include "event_gpower.h"

#include "script_def.h"
#include "scrcmd.h"
#include "script.h"
#include "script_local.h"

#include "scrcmd_beacon.h"

//--------------------------------------------------------------------
/**
 * @brief �r�[�R�����M���N�G�X�g�Ăяo�� 
 *
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT:
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

//--------------------------------------------------------------------
/**
 * @brief �t�B�j�b�V���҂���G�p���[�`�F�b�N 
 *
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT:
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdCheckGPowerFinish( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*   work     = (SCRCMD_WORK*)wk;
  u16*           ret_power  = SCRCMD_GetVMWork( core, work );
  u16*           ret_next   = SCRCMD_GetVMWork( core, work );

  *ret_power = GPOWER_Get_FinishWaitID();

  if((*ret_power) != GPOWER_ID_NULL){
    POWER_CONV_DATA * p_data = GPOWER_PowerData_LoadAlloc( GFL_HEAP_LOWID(HEAPID_FIELDMAP) );
    GPOWER_Set_Finish( *ret_power, p_data );
    GPOWER_PowerData_Unload( p_data );
  
    //�������邩�`�F�b�N���Ă���
    *ret_next = (GPOWER_Get_FinishWaitID() != GPOWER_ID_NULL);
  }else{
    *ret_next = FALSE;
  }
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief G�p���[�t�B�[���h�G�t�F�N�g
 *
 * @param core ���z�}�V������\���̂ւ̃|�C���^
 * @param wk   SCRCMD_WORK�ւ̃|�C���^
 *
 * @retval VMCMD_RESULT:
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdGPowerUseEffect( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*   work     = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   sc       = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*  gsys     = SCRCMD_WORK_GetGameSysWork( work );

  SCRIPT_CallEvent( sc, EVENT_GPowerUseEffect( gsys ));

  return VMCMD_RESULT_SUSPEND;
}

