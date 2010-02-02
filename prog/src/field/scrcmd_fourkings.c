//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_fourkings.c
 *	@brief  �l�V���X�N���v�g�R�}���h
 *	@author	tomoya takahashi
 *	@date		2009.11.25
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "script.h"
#include "script_def.h"
#include "script_local.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "fieldmap.h"

#include "scrcmd_fourkings.h"

#include "event_fourkings.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------


//======================================================================
//  �l�V�������C�x���g
//======================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �l�V�������C�x���g�����҂�
 *
 *	@param	core
 *	@param	*wk 
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdFourKings_WalkEvent( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  u16          index = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��1����

  GF_ASSERT( index > 0 );

  // ZONE ID �� ID�ł킽���Ă���i1�I���W���j�̂ŁA-1����
  index -= 1;

  // �C�x���g���Ăяo��
  SCRIPT_CallEvent( scw, EVENT_FourKings_CircleWalk(gsys, fieldWork, index) );
  return VMCMD_RESULT_SUSPEND;
}




//----------------------------------------------------------------------------
/**
 *	@brief  �l�V������J�����̎w��
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdFourKings_SetCameraParam( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  u16          index = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��1����
  FIELD_CAMERA* p_camera = FIELDMAP_GetFieldCamera( fieldWork );

  GF_ASSERT( index > 0 );

  // ZONE ID �� ID�ł킽���Ă���i1�I���W���j�̂ŁA-1����
  index -= 1;


  // �J�����̏�Ԃ𒸏�ɂ���
  FIELD_CAMERA_SetCameraType( p_camera, EVENT_FourKings_GetCameraID( index ) );

  return VMCMD_RESULT_CONTINUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �S�[�X�g�����@�ݒ�
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdFourKings_SetGhostSparkSound( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );

  // ����ON
  FIELDMAPFUNC_FourkingsGhostSparkSound( fieldWork );

  return VMCMD_RESULT_CONTINUE;
}



