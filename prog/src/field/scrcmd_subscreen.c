//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		scrcmd_subscreen.c
 *	@brief
 *	@author	 
 *	@date		2010.03.10
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

#include "scrcmd_subscreen.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

static const u32 sc_SUBSCREEN_MODE[ SCR_EV_SUBSCREEN_MODE_MAX ] = 
{
  FIELD_SUBSCREEN_REPORT,
};

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
 *	@brief  �T�u�X�N���[���@�ύX����
 */
//-----------------------------------------------------------------------------
//-------------------------------------
///	���[�N
//=====================================
typedef struct {
  
  FIELD_SUBSCREEN_WORK* subscreen;
  int change_index;
  
} EV_SUBSCREEN_WORK;

// �ύX�҂�����
static GMEVENT_RESULT EVSUBSCREEN_ChangeWait( GMEVENT * event, int *seq, void*work )
{
  EV_SUBSCREEN_WORK* p_wk = work;
  
  switch( *seq ){
  case 0:
    if( FIELD_SUBSCREEN_CanChange( p_wk->subscreen ) ){
      FIELD_SUBSCREEN_Change( p_wk->subscreen, p_wk->change_index );
      (*seq) ++;
    }
    break;

  case 1:
    if( FIELD_SUBSCREEN_CanChange( p_wk->subscreen ) ){
      return GMEVENT_RES_FINISH;
    }
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �T�u�X�N���[���@�ύX����
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdSubScreenChnage( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  u16 index = SCRCMD_GetVMWorkValue( core, work );  // �R�}���h��1����
  FIELD_SUBSCREEN_WORK* subscreen = FIELDMAP_GetFieldSubscreenWork( fparam->fieldMap );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
	GMEVENT * event = GMEVENT_Create( gsys, NULL, EVSUBSCREEN_ChangeWait, sizeof(EV_SUBSCREEN_WORK) );
	EV_SUBSCREEN_WORK * p_wk = GMEVENT_GetEventWork(event);

  GF_ASSERT( index < SCR_EV_SUBSCREEN_MODE_MAX );
  
  p_wk->subscreen = subscreen;
  p_wk->change_index = sc_SUBSCREEN_MODE[ index ];

  SCRIPT_CallEvent( sc, event );

  // ���d�Ăяo��
  GF_ASSERT( SCREND_CHK_CheckBit( SCREND_CHK_SUBSCREEN_CHANGE ) == FALSE );
  SCREND_CHK_SetBitOn( SCREND_CHK_SUBSCREEN_CHANGE );

  return VMCMD_RESULT_SUSPEND;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �T�u�X�N���[�������ɖ߂�
 */
//-----------------------------------------------------------------------------
VMCMD_RESULT EvCmdSubScreenBack( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  FIELD_SUBSCREEN_WORK* subscreen = FIELDMAP_GetFieldSubscreenWork( fparam->fieldMap );
  GAMESYS_WORK* gsys = SCRCMD_WORK_GetGameSysWork( work );
	GMEVENT * event = GMEVENT_Create( gsys, NULL, EVSUBSCREEN_ChangeWait, sizeof(EV_SUBSCREEN_WORK) );
	EV_SUBSCREEN_WORK * p_wk = GMEVENT_GetEventWork(event);


  // ���d�Ăяo��
  GF_ASSERT( SCREND_CHK_CheckBit( SCREND_CHK_SUBSCREEN_CHANGE ) == TRUE );

  p_wk->subscreen = subscreen;
  p_wk->change_index = FIELD_SUBSCREEN_NORMAL;

  SCRIPT_CallEvent( sc, event );

  SCREND_CHK_SetBitOff( SCREND_CHK_SUBSCREEN_CHANGE );

  return VMCMD_RESULT_SUSPEND;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �T�u�X�N���[�������ɖ߂�
 *
 *	@param	end_check   ���[�N
 *	@param	seq         �V�[�P���X
 *
 *	@retval TRUE    ����
 *	@retval FALSE   �r��
 */
//-----------------------------------------------------------------------------
BOOL SCREND_CheckEndSubScreenChange( SCREND_CHECK *end_check, int *seq )
{
  SCRIPT_WORK *sc = end_check->ScrWk;
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  FIELD_SUBSCREEN_WORK* subscreen = FIELDMAP_GetFieldSubscreenWork( fparam->fieldMap );

  switch( *seq )
  {
  case 0:
    if( FIELD_SUBSCREEN_CanChange( subscreen ) ){
      FIELD_SUBSCREEN_Change( subscreen, FIELD_SUBSCREEN_NORMAL );
      SCREND_CHK_SetBitOff( SCREND_CHK_SUBSCREEN_CHANGE );

      (*seq) ++;
    }
    break;

  case 1:
    if( FIELD_SUBSCREEN_CanChange( subscreen ) ){
      return TRUE;
    }
    break;
  }
  return FALSE;
}

