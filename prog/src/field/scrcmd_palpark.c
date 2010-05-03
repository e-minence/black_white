//======================================================================
/**
 * @file scr_musical.c
 * @brief �X�N���v�g�R�}���h�@�~���[�W�J���֘A
 * @author	Satoshi Nohara
 * @date	06.06.26
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
#include "scrcmd_palpark.h"


#include "../../../resource/fldmapdata/script/palpark_scr_local.h"
#include "savedata/save_control.h"
#include "savedata/misc.h"
#include "event_fieldmap_control.h"

#include "multiboot/mb_parent_sys.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================

//======================================================================
//  �X�N���v�g�R�}���h�@�~���[�W�J��
//======================================================================
//--------------------------------------------------------------
/**
 * �p���p�[�N�F�p���p�[�N�Ăяo��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPalparkCall( VMHANDLE *core, void *wk )
{
  GMEVENT *event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  
  MB_PARENT_INIT_WORK *param;
  
  param = GFL_HEAP_AllocMemory( HEAPID_PROC, sizeof(MB_PARENT_INIT_WORK) ); 
  param->mode    = MPM_POKE_SHIFTER;
  param->gameData    = gdata;

  event = EVENT_FieldSubProcNoFade_Callback( gsys, fieldmap, 
                              FS_OVERLAY_ID(multiboot), &MultiBoot_ProcData, param ,
                              NULL , param );

  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * �p���p�[�N�F�p���p�[�N�n���l�擾
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdGetPalparkValue( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;

  u8   type   = VMGetU8( core );
  u16* ret_wk = SCRCMD_GetVMWork( core, work );

  GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  SAVE_CONTROL_WORK *svWork = GAMEDATA_GetSaveControlWork( gdata );
  MISC *miscSave = SaveData_GetMisc( svWork );
  
  switch(type)
  {
  case 0:
    OS_TPrintf("�p���p�[�N���l�擾 �I���X�e�[�g\n");
    *ret_wk = MISC_GetPalparkFinishState( miscSave );
    //PALPARK_FINISH_NORMAL    (0)  //�ߊl����
    //PALPARK_FINISH_HIGHSOCRE (1)  //�ߊl�����{�n�C�X�R�A
    //PALPARK_FINISH_NO_GET    (2)  //�ߊl�ł��Ȃ�����
    //PALPARK_FINISH_ERROR     (3)  //�G���[�I��
    OS_TPrintf("state = %d\n",*ret_wk);
    break;
  case 1:
    OS_TPrintf("�p���p�[�N���l�擾 �n�C�X�R�A\n");
    *ret_wk = MISC_GetPalparkHighscore( miscSave );
    break;
  }

  return VMCMD_RESULT_CONTINUE;
}

