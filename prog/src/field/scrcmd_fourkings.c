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

  // �C�x���g���Ăяo��
  SCRIPT_CallEvent( scw, EVENT_FourKings_CircleWalk(gsys, fieldWork, index) );
  return VMCMD_RESULT_SUSPEND;
}


