//======================================================================
/**
 * @file	scrcmd_hiden.c
 * @brief	�X�N���v�g�R�}���h�F��`�Z�n
 * @date  2009.09.22
 * @author	tamada GAMEFREAK inc.
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "scrcmd_hiden.h"

#include "fieldmap.h"
#include "field_player_grid.h"

//======================================================================
//  ��`�Z
//======================================================================
//--------------------------------------------------------------
/**
 * ��`�Z�g���J�n�@�E�F�C�g����
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  BOOL TRUE=�I��
 */
//--------------------------------------------------------------
static BOOL EvWaitNaminori( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GFL_TCB *tcb = SCRCMD_WORK_GetCallProcTCB( work ); 
  
  if( FIELD_PLAYER_GRID_CheckEventNaminoriStart(tcb) == TRUE ){
    FIELD_PLAYER_GRID_DeleteEventNaminoriStart( tcb );
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * ��`�Z�g���J�n
 * @param  core    ���z�}�V������\���̂ւ̃|�C���^
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdNaminori( VMHANDLE *core, void *wk )
{
  GFL_TCB *tcb;
  VecFx32 pos;
  MAPATTR attr;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
  u16 dir = MMDL_GetDirDisp( mmdl );
  FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );
  HEAPID heapID = SCRCMD_WORK_GetHeapID( work );

  FIELD_PLAYER_GetDirPos( fld_player, dir, &pos );
  attr = MAPATTR_GetAttribute( mapper, &pos );

  tcb = FIELD_PLAYER_GRID_SetEventNaminoriStart(
      fld_player, dir, attr, heapID );
  SCRCMD_WORK_SetCallProcTCB( work, tcb );
  VMCMD_SetWait( core, EvWaitNaminori );
  
  return( VMCMD_RESULT_SUSPEND );
}
