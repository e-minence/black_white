/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file	 scrcmd_gate.c
 * @brief	 �X�N���v�g�R�}���h�F�Q�[�g�֘A
 * @date   2009.11.05
 * @author obata
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/vm_cmd.h"
#include "scrcmd_gate.h"
#include "field_gimmick_gate.h"
#include "gimmick_obj_elboard.h"
#include "scrcmd_work.h"
#include "script_local.h"
#include "scrcmd.h"
#include "event_look_elboard.h"
#include "sound/pm_sndsys.h"


#if 0  // �j���[�X�̊Ǘ����@��ύX��, �s�v�ɂȂ������߂ɍ폜 2009.12.17
//---------------------------------------------------------------------------------------
/**
 * @brief ���펞�̃j���[�X���Z�b�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param  wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//---------------------------------------------------------------------------------------
VMCMD_RESULT EvCmdElboard_SetNormalNews( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  GATE_GIMMICK_Elboard_SetupNormalNews( fieldmap );
  return VMCMD_RESULT_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @brief �Վ��j���[�X��ǉ�����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param  wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//---------------------------------------------------------------------------------------
VMCMD_RESULT EvCmdElboard_AddSpecialNews( VMHANDLE *core, void *wk )
{ 
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  u16              str_id = SCRCMD_GetVMWorkValue( core, work );   // �R�}���h������
  SCRIPT_WORK*   scr_work = SCRCMD_WORK_GetScriptWork( work );
  WORDSET**       wordset = SCRIPT_GetMemberWork( scr_work, ID_EVSCR_WORDSET );

  GATE_GIMMICK_Elboard_AddSpecialNews( fieldmap, str_id, *wordset );
  return VMCMD_RESULT_CONTINUE;
}

//---------------------------------------------------------------------------------------
/**
 * @brief �f���𕜋A������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param  wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//---------------------------------------------------------------------------------------
VMCMD_RESULT EvCmdElboard_Recovery( VMHANDLE *core, void *wk )
{ 
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  GATE_GIMMICK_Elboard_Recovery( fieldmap );
  return VMCMD_RESULT_CONTINUE;
}
#endif


//---------------------------------------------------------------------------------------
/**
 * @brief �d���f��������C�x���g�Ăяo��
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @param  wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//---------------------------------------------------------------------------------------
VMCMD_RESULT EvCmdLookElboard( VMHANDLE *core, void *wk )
{ 
  SCRCMD_WORK*       work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*     script = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  u16               frame = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h������
  GMEVENT* event;

  PMSND_PlaySE( SEQ_SE_MESSAGE );

  // �C�x���g�Ăяo��
  event = EVENT_LookElboard( gsys, fieldmap, frame );
  SCRIPT_CallEvent( script, event );
  return VMCMD_RESULT_SUSPEND;
}
