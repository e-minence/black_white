//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �X�N���v�g�R�}���h�F�ʐM�֘A
 * @file   scrcmd_network.c
 * @author obata
 * @date   2009.10.19
 *
 */
//////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/main.h"
#include "system/vm_cmd.h"
#include "scrcmd.h"
#include "scrcmd_network.h"
#include "fieldmap.h" //FIELDCOMM_xxxx
#include "event_comm_error.h" // EVENT_FieldCommErroProc
#include "system/net_err.h" //NetErr_ErrorSet();

#include "script_def.h"

//====================================================================
// ���v���g�^�C�v
//====================================================================
static BOOL VM_WAIT_FUNC_FieldCommExitWait( VMHANDLE* core, void* wk );

typedef struct _FIELD_COMM_EXIT_WORK{
  u16*  ret_work;
}FIELD_COMM_EXIT_WORK;

//--------------------------------------------------------------------
/**
 * @brief �t�B�[���h�ʐM�̏I���҂��֐�
 */
//--------------------------------------------------------------------
static BOOL VM_WAIT_FUNC_FieldCommExitWait( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*            work = (SCRCMD_WORK*)wk;
  GAMESYS_WORK*           gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK             *sc = SCRCMD_WORK_GetScriptWork( work );
  GAME_COMM_SYS_PTR       game_comm = GAMESYSTEM_GetGameCommSysPtr( gsys );
  FIELDMAP_WORK*          fieldmap = GAMESYSTEM_GetFieldMapWork( gsys ); 
  FIELDCOMM_EXIT          exit;
  void** scr_subproc_work = SCRIPT_SetSubProcWorkPointerAdrs( sc );
  FIELD_COMM_EXIT_WORK* fcew = (FIELD_COMM_EXIT_WORK*)*scr_subproc_work;

  // �ʐM�I������
  exit = FIELDCOMM_ExitWait( game_comm ); 
  switch( exit )
  {
  case FIELDCOMM_EXIT_CONTINUE: // �ʐM�I��������
    OBATA_Printf( "VM_WAIT_FUNC_FieldCommExitWait: �ʐM�I��������\n" );
    return FALSE;
  case FIELDCOMM_EXIT_END:      // �ʐM�I������
    OBATA_Printf( "VM_WAIT_FUNC_FieldCommExitWait: �ʐM�I������\n" );
    GFL_HEAP_FreeMemory(fcew);
    return TRUE;
  case FIELDCOMM_EXIT_ERROR:    // �G���[����
#if 0
    NetErr_ErrorSet();
    { // �C�x���g�Ăяo��
      GMEVENT* parent_event = GAMESYSTEM_GetEvent( gsys ); //���݂̃C�x���g
      GMEVENT*        event = EVENT_FieldCommErrorProc( gsys, fieldmap );
      GMEVENT_CallEvent( parent_event, event );
      *fcew->ret_work = SCR_FIELD_COMM_EXIT_ERROR;
    }
    OBATA_Printf( "VM_WAIT_FUNC_FieldCommExitWait: �G���[����\n" );
    GFL_HEAP_FreeMemory(fcew);
#else
    *fcew->ret_work = SCR_FIELD_COMM_EXIT_ERROR;
    OBATA_Printf( "VM_WAIT_FUNC_FieldCommExitWait: �G���[����\n" );
    GFL_HEAP_FreeMemory(fcew);
#endif
    return TRUE;
  }

  return FALSE;
}

//--------------------------------------------------------------------
/**
 * @brief �t�B�[���h�ʐM���I������
 * @param core    ���z�}�V������\���̂ւ̃|�C���^
 * @param wk      SCRCMD_WORK�ւ̃|�C���^
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdFieldCommExitWait( VMHANDLE * core, void *wk )
{
  SCRCMD_WORK*      work = (SCRCMD_WORK*)wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  void** scr_subproc_work = SCRIPT_SetSubProcWorkPointerAdrs( sc );
  FIELD_COMM_EXIT_WORK* fcew;
  u16*              ret_wk = SCRCMD_GetVMWork( core, work );

  *scr_subproc_work = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(FIELD_COMM_EXIT_WORK) );
  fcew = *scr_subproc_work;

  fcew = (FIELD_COMM_EXIT_WORK*)*scr_subproc_work;
  fcew->ret_work = ret_wk;
  *fcew->ret_work = SCR_FIELD_COMM_EXIT_OK;

  VMCMD_SetWait( core, VM_WAIT_FUNC_FieldCommExitWait );
  OBATA_Printf( "EvCmdFieldCommExitWait\n" );
  return VMCMD_RESULT_SUSPEND;
}
