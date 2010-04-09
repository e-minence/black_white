/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file	 scrcmd_league_front.c
 * @brief	 �X�N���v�g�R�}���h�F�|�P�������[�O �t�����g�֘A
 * @date   2009.12.14
 * @author obata
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "system/vm_cmd.h"
#include "scrcmd_league_front.h"
#include "field_gimmick_league_front01.h"
#include "scrcmd_work.h"
#include "script_local.h"
#include "scrcmd.h"
#include "event_league_front01.h"


//---------------------------------------------------------------------------------------
/**
 * @brief ���t�g�~���C�x���g���J�n����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param  wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//---------------------------------------------------------------------------------------
VMCMD_RESULT EvCmdLeagueFrontLiftDown( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*     script = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys ); 
  GMEVENT* event;

  // �C�x���g�Ăяo��
  event = EVENT_LFRONT01_LiftDown( gsys, fieldmap );
  SCRIPT_CallEvent( script, event );
  return VMCMD_RESULT_SUSPEND;
} 
