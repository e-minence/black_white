//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_crowd_people.c
 *	@brief
 *	@author	 
 *	@date		2010.01.27
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


#include <gflib.h>


#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"


#include "fieldmap.h"

#include "field_crowd_people.h"

#include "scrcmd_crowd_people.h"

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
 *	@brief  �Q�W�J�n
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdCrowdPeople( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  FLDMAPFUNC_SYS* p_funcsys = FIELDMAP_GetFldmapFuncSys( fparam->fieldMap );

  FIELD_CROWD_PEOPLE_Create( p_funcsys );

	return VMCMD_RESULT_CONTINUE;
}
