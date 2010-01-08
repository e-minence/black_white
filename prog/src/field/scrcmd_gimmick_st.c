/*
 *  @file   scrcmd_gimmick_st.c
 *  @brief  �X�N���v�g�R�}���h�@�t�B�[���h�M�~�b�N�p�풓
 *  @author Miyuki Iwasawa
 *  @date   10.01.07
 */

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

#include "field_gimmick_bgate_jet.h"

FS_EXTERN_OVERLAY(field_gimmick_bgate);

//--------------------------------------------------------------
/**
 * �W�F�b�g�o�b�W�Q�[�g�}�b�v�M�~�b�N������
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdJetBadgeGateGimmickInit( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  
  GFL_OVERLAY_Load( FS_OVERLAY_ID(field_gimmick_bgate) );		//�I�[�o�[���C���[�h
  FLD_GIMMICK_JetBadgeGate_Init( gsys );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(field_gimmick_bgate));		//�I�[�o�[���C�A�����[�h

  return VMCMD_RESULT_CONTINUE;
}

