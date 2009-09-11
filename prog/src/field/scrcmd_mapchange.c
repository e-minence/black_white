//======================================================================
/**
 * @file  scrcmd_mapchange.c
 * @bfief  スクリプトコマンド：マップ遷移関連
 * @date  2009.09.11
 * @author	tamada GAMEFREAK inc.
 */
//======================================================================
#include <gflib.h>
//#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
//#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "scrcmd_mapchange.h"

#include "event_mapchange.h"
#include "field/field_const.h"  //FIELD_CONST_GRID_SIZE
//======================================================================
//  define
//======================================================================
#define GRID_TO_FX32(gvalue)  ( (gvalue * FIELD_CONST_GRID_SIZE) << FX32_SHIFT )
//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================

//======================================================================
//  スクリプトコマンド：
//======================================================================
//--------------------------------------------------------------
/**
 * マップ遷移（流砂）
 * @param	core		仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapChangeBySandStream( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK * gsys = SCRIPT_GetGameSysWork( sc );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  //HEAPID heapID = SCRCMD_WORK_GetHeapID( work );
  //GAMEDATA *gdata = SCRCMD_WORK_GetGameData( work );
  VecFx32 disappear_pos;
  VecFx32 appear_pos;

  u16 zone_id = VMGetU16(core); //2byte read  ZONE指定
  u16 next_x = VMGetU16(core);  //2byte read  X位置グリッド単位
  u16 next_z = VMGetU16(core);  //2byte read  Z位置グリッド単位
  
  {
    FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
    MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
    u16 x = MMDL_GetGridPosX( mmdl );
    u16 y = MMDL_GetGridPosY( mmdl );
    u16 z = MMDL_GetGridPosZ( mmdl );
    VEC_Set(&disappear_pos, GRID_TO_FX32(x), GRID_TO_FX32(y), GRID_TO_FX32(z)); 
  }
  
  {
    GMEVENT * mapchange_event;
    GMEVENT * parent_event;
    u16 dir = 0;                  //仮  仕様としてはどうするべきなのか？

    VEC_Set(&appear_pos,
        GRID_TO_FX32(next_x),
        GRID_TO_FX32(0),      //todo 高さ（Y）も指定するべきかどうか？
        GRID_TO_FX32(next_z)
        ); 
    
    parent_event = GAMESYSTEM_GetEvent( gsys ); //現在のイベント
    mapchange_event = DEBUG_EVENT_ChangeMapBySandStream( gsys, fieldmap,
        &disappear_pos, zone_id, &appear_pos, dir );
    GMEVENT_CallEvent( parent_event, mapchange_event );
  }

  return VMCMD_RESULT_CONTINUE;
}

