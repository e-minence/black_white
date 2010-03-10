//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  スクリプトコマンド：通信関連
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

#include "system/ds_system.h" //DS_SYSTEM_IsAvailableWireless

#include "script_def.h"

//====================================================================
// ■プロトタイプ
//====================================================================
static BOOL VM_WAIT_FUNC_FieldCommExitWait( VMHANDLE* core, void* wk );

typedef struct _FIELD_COMM_EXIT_WORK{
  u16*  ret_work;
}FIELD_COMM_EXIT_WORK;

//--------------------------------------------------------------------
/**
 * @brief フィールド通信の終了待ち関数
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

  // 通信終了処理
  exit = FIELDCOMM_ExitWait( game_comm ); 
  switch( exit )
  {
  case FIELDCOMM_EXIT_CONTINUE: // 通信終了未完了
    OBATA_Printf( "VM_WAIT_FUNC_FieldCommExitWait: 通信終了未完了\n" );
    return FALSE;
  case FIELDCOMM_EXIT_END:      // 通信終了完了
    OBATA_Printf( "VM_WAIT_FUNC_FieldCommExitWait: 通信終了完了\n" );
    GFL_HEAP_FreeMemory(fcew);
    return TRUE;
  case FIELDCOMM_EXIT_ERROR:    // エラー発生
#if 0
    NetErr_ErrorSet();
    { // イベント呼び出し
      GMEVENT* parent_event = GAMESYSTEM_GetEvent( gsys ); //現在のイベント
      GMEVENT*        event = EVENT_FieldCommErrorProc( gsys, fieldmap );
      GMEVENT_CallEvent( parent_event, event );
      *fcew->ret_work = SCR_FIELD_COMM_EXIT_ERROR;
    }
    OBATA_Printf( "VM_WAIT_FUNC_FieldCommExitWait: エラー発生\n" );
    GFL_HEAP_FreeMemory(fcew);
#else
    *fcew->ret_work = SCR_FIELD_COMM_EXIT_ERROR;
    OBATA_Printf( "VM_WAIT_FUNC_FieldCommExitWait: エラー発生\n" );
    GFL_HEAP_FreeMemory(fcew);
#endif
    return TRUE;
  }

  return FALSE;
}

//--------------------------------------------------------------------
/**
 * @brief フィールド通信を終了する
 * @param core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
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

//--------------------------------------------------------------------
/**
 * @brief DSで通信許可設定されているかどうかを返す
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdAvailableWireless( VMHANDLE * core, void *wk )
{
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );

  *ret_wk = DS_SYSTEM_IsAvailableWireless();
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------------
/**
 * @brief 常時通信時のビーコンサーチを再開する
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdRebootBeaconSearch( VMHANDLE * core, void * wk )
{
  SCRCMD_WORK * work = wk;
  GAMESYS_WORK * gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAME_COMM_SYS_PTR gcsp = GAMESYSTEM_GetGameCommSysPtr(gsys);

  if ( GAMESYSTEM_GetAlwaysNetFlag( gsys ) == TRUE )
  {
    if ( GameCommSys_BootCheck( gcsp ) == GAME_COMM_NO_NULL )
    {
      GameCommSys_Boot( gcsp, GAME_COMM_NO_FIELD_BEACON_SEARCH, gsys );
    }
    else
    {
      OS_TPrintf("!!!起動不可の状態でビーコンサーチを起動しようとした!!!\n");
    }
  }
  return VMCMD_RESULT_SUSPEND;
}




