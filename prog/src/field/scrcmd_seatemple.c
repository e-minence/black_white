//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_seatemple.c
 *	@brief  �C��_�a�X�N���v�g�R�}���h
 *	@author	tomoya takahashi
 *	@date		2010.02.02
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

#include "fieldmap_func.h"

#include "scrcmd_seatemple.h"
#include "field_seatemple.h"


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
//----------------------------------------------------------------------------
/**
 *	@brief  �C��_�a�@�\�����Ǘ������@�J�n
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdSeaTempleStart( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*  work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK*   scw = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* p_fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMAPFUNC_SYS* p_funcsys = FIELDMAP_GetFldmapFuncSys( p_fieldmap );

  FLDMAPFUNC_Create(FS_OVERLAY_ID(field_seatemple),  p_funcsys, &c_FLDMAPFUNC_SEATEMPLE);

  return VMCMD_RESULT_CONTINUE;
}




