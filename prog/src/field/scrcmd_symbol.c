//==============================================================================
/**
 * @file    scrcmd_symbol.c
 * @brief   �N���F�V���{���|�P����
 * @author  matsuda
 * @date    2010.03.16(��)
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

#include "scrcmd_symbol.h"
#include "savedata/symbol_save.h"
#include "event_symbol.h"



//==================================================================
/**
 * �V���{���|�P�����o�g��
 *
 * @param   core		
 * @param   wk		
 *
 * @retval  VMCMD_RESULT		
 *
 * �ߊl�����ꍇ�A���ʂ�TRUE���B�ߊl���Ȃ������ꍇ�AFALSE�ɂȂ�܂�
 */
//==================================================================
VMCMD_RESULT EvCmdSymbolPokeBattle( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  MMDL *mmdl = SCRIPT_GetTargetObj( scw );  //�b���������̂ݗL��
  u16 obj_code = MMDL_GetOBJCode( mmdl );
  ZONEID zone_id = SCRCMD_WORK_GetZoneID( work );
  u16* ret_wk = SCRCMD_GetVMWork(core,work);  //���ʂ�Ԃ����[�N
  static SYMBOL_POKEMON test_symbol_poke = {
    10, 20, 0, 0, 
  };
  
  SCRIPT_CallEvent( scw, 
    EVENT_SymbolPokeBattle(gsys, fieldWork, &test_symbol_poke, ret_wk, HEAPID_PROC) );
  return VMCMD_RESULT_SUSPEND;
}

