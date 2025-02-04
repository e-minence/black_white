//======================================================================
/**
 * @file  scrcmd_mapchange.c
 * @brief  スクリプトコマンド：マップ遷移関連
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
#include "script_local.h"
//#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "scrcmd_mapchange.h"

#include "event_mapchange.h"
#include "field/field_const.h"  //GRID_TO_FX32
#include "eventdata_local.h"

#include "fldeff_gyoe.h"
#include "sound/pm_sndsys.h"

#include "system/main.h"    //HEAPID_FIELDMAP

//======================================================================
//  define
//======================================================================

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
 * マップ遷移（通常）
 * @param	core		仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapChange( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK * gsys = SCRIPT_GetGameSysWork( sc );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  VecFx32 appear_pos;

  u16 zone_id = VMGetU16(core); //2byte read  ZONE指定
  u16 next_x = VMGetU16(core);  //2byte read  X位置グリッド単位
  u16 next_z = VMGetU16(core);  //2byte read  Z位置グリッド単位
  u16 dir = VMGetU16(core);     //2byte read  方向
  
  {
    GMEVENT * mapchange_event;
    GMEVENT * parent_event;

    VEC_Set(&appear_pos,
        GRID_TO_FX32(next_x),
        GRID_TO_FX32(0),
        GRID_TO_FX32(next_z)
        ); 
    
    parent_event = GAMESYSTEM_GetEvent( gsys ); //現在のイベント
    mapchange_event = EVENT_ChangeMapPos( 
        gsys, fieldmap, zone_id, &appear_pos, dir, FALSE );
    GMEVENT_CallEvent( parent_event, mapchange_event );
  }

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * マップ遷移（BGM変更なし）
 * @param	core		仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapChangeBGMKeep( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*        work = wk;
  SCRIPT_WORK*  scriptWork = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gameSystem = SCRIPT_GetGameSysWork( scriptWork );
  FIELDMAP_WORK*  fieldmap = GAMESYSTEM_GetFieldMapWork( gameSystem );

  VecFx32 appear_pos;

  u16 zone_id = VMGetU16(core);  //2byte read  ZONE指定
  u16 next_x  = VMGetU16(core);  //2byte read  X位置グリッド単位
  u16 next_z  = VMGetU16(core);  //2byte read  Z位置グリッド単位
  u16 dir     = VMGetU16(core);  //2byte read  方向
  
  {
    GMEVENT * mapchange_event;
    GMEVENT * parent_event;

    VEC_Set(&appear_pos,
        GRID_TO_FX32(next_x),
        GRID_TO_FX32(0),
        GRID_TO_FX32(next_z)
        ); 
    
    parent_event = GAMESYSTEM_GetEvent( gameSystem ); //現在のイベント
    mapchange_event = EVENT_ChangeMapBGMKeep( 
        gameSystem, fieldmap, zone_id, &appear_pos, dir );
    GMEVENT_CallEvent( parent_event, mapchange_event );
  }

  return VMCMD_RESULT_SUSPEND;
}


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
  
  // 流砂中心位置を求める
  {
    VecFx32 pos;
    const POS_EVENT_DATA* pos_event;
    GAMEDATA *       gamedata = GAMESYSTEM_GetGameData( gsys );
    EVENTDATA_SYSTEM * evdata = GAMEDATA_GetEventData( gamedata );
    EVENTWORK *        evwork = GAMEDATA_GetEventWork( gamedata );
    FIELD_PLAYER *     player = FIELDMAP_GetFieldPlayer( fieldmap );
    FIELD_PLAYER_GetPos( player, &pos );
    pos_event = EVENTDATA_GetPosEvent_XZ( evdata, evwork, &pos, DIR_NOT );
    if( pos_event )
    {
      EVENTDATA_GetPosEventCenterPos( pos_event, &disappear_pos );
      // 直接アクセスできなくなったので上の関数に変更しました　091007 tomoya takahashi
//      disappear_pos.x = GRID_TO_FX32( pos_event->gx ) + FX_Div( GRID_TO_FX32( pos_event->sx ), 2<<FX32_SHIFT );
//      disappear_pos.z = GRID_TO_FX32( pos_event->gz ) + FX_Div( GRID_TO_FX32( pos_event->sz ), 2<<FX32_SHIFT );
    }
  }
  
  {
    GMEVENT * mapchange_event;
    GMEVENT * parent_event;

    VEC_Set(&appear_pos,
        GRID_TO_FX32(next_x),
        GRID_TO_FX32(0),
        GRID_TO_FX32(next_z)
        ); 
    
    parent_event = GAMESYSTEM_GetEvent( gsys ); //現在のイベント
    mapchange_event = EVENT_ChangeMapBySandStream( gsys, fieldmap,
        &disappear_pos, zone_id, &appear_pos );
    GMEVENT_CallEvent( parent_event, mapchange_event );
  }

#if 1
  { // びっくりマーク表示
    FIELD_PLAYER* player = FIELDMAP_GetFieldPlayer( fieldmap );
    MMDL*           mmdl = FIELD_PLAYER_GetMMdl( player );
    FLDEFF_CTRL*  fectrl = FIELDMAP_GetFldEffCtrl( fieldmap );
    FLDEFF_GYOE_SetMMdlNonDepend( fectrl, mmdl, FLDEFF_GYOETYPE_GYOE, TRUE );
  }
#endif


  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * 「ワープ」によるマップ遷移
 * @param	core		仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapChangeByWarp( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*        work = wk;
  SCRIPT_WORK*  scriptWork = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK* gameSystem = SCRIPT_GetGameSysWork( scriptWork );
  FIELDMAP_WORK*  fieldmap = GAMESYSTEM_GetFieldMapWork( gameSystem );

  VecFx32 appearPos;

  u16 zoneID = VMGetU16( core );  //2byte read  ZONE指定
  u16 gridX  = VMGetU16( core );  //2byte read  X位置グリッド単位
  u16 gridZ  = VMGetU16( core );  //2byte read  Z位置グリッド単位
  u16 dir    = VMGetU16( core );  //2byte read  方向
  
  {
    GMEVENT * event;
    GMEVENT * parentEvent;

    // 遷移先の座標を設定
    VEC_Set( &appearPos, GRID_TO_FX32(gridX), GRID_TO_FX32(0), GRID_TO_FX32(gridZ)); 
    
    parentEvent = GAMESYSTEM_GetEvent( gameSystem ); //現在のイベント
    event = EVENT_ChangeMapByWarp( 
        gameSystem, fieldmap, zoneID, &appearPos, dir );

    GMEVENT_CallEvent( parentEvent, event );
  }

  return VMCMD_RESULT_SUSPEND;
}


//--------------------------------------------------------------
/**
 * マップ遷移（ユニオンルーム）
 * @param	 core		仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapChangeToUnion( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK*       work = wk;
  SCRIPT_WORK*         sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK*      gsys = SCRIPT_GetGameSysWork( sc );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys ); 
  
  // イベント呼び出し
  {
    GMEVENT* mapchange_event;
    GMEVENT* parent_event;

    parent_event    = GAMESYSTEM_GetEvent( gsys ); //現在のイベント
    mapchange_event = EVENT_ChangeMapToUnion( gsys, fieldmap );
    GMEVENT_CallEvent( parent_event, mapchange_event );
  }

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * マップ遷移（フェードなし）
 * @param	core		仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapChangeNoFade( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK * gsys = SCRIPT_GetGameSysWork( sc );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  VecFx32 appear_pos;

  u16 zone_id = VMGetU16(core); //2byte read  ZONE指定
  u16 next_x = VMGetU16(core);  //2byte read  X位置グリッド単位
  u16 next_y = VMGetU16(core);  //2byte read  Y位置グリッド単位
  u16 next_z = VMGetU16(core);  //2byte read  Z位置グリッド単位
  u16 dir = VMGetU16(core);     //2byte read  方向
  
  {
    FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
    MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
  }
  
  {
    GMEVENT * mapchange_event;
    GMEVENT * parent_event;

    VEC_Set(&appear_pos,
        GRID_TO_FX32(next_x),
        GRID_TO_FX32(next_y),
        GRID_TO_FX32(next_z)
        ); 
    
    parent_event = GAMESYSTEM_GetEvent( gsys ); //現在のイベント
    mapchange_event = EVENT_ChangeMapPosNoFade( gsys, fieldmap,
         zone_id, &appear_pos, dir );
    GMEVENT_CallEvent( parent_event, mapchange_event );
  }

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * レールマップに遷移
 * @param	core		仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdRailMapChange( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK * gsys = SCRIPT_GetGameSysWork( sc );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  u16 zone_id = VMGetU16(core); //2byte read  ZONE指定
  u16 rail_idx = VMGetU16(core);  //2byte read  レールインデックス
  u16 front_idx = VMGetU16(core);  //2byte read  フロントインデックス
  u16 side_idx = VMGetU16(core);  //2byte read  サイドインデックス
  u16 dir = VMGetU16(core);     //2byte read  方向
  
  {
    GMEVENT * mapchange_event;
    GMEVENT * parent_event;
    RAIL_LOCATION loc;

    parent_event = GAMESYSTEM_GetEvent( gsys ); //現在のイベント
    {
      FLDNOGRID_MAPPER* nogrid = FIELDMAP_GetFldNoGridMapper( fieldmap );
      FLDNOGRID_MAPPER_ChangeNotMinusRailPosToRailLocation(
          nogrid, zone_id, rail_idx, front_idx, side_idx, &loc, HEAPID_FIELDMAP );
      mapchange_event = EVENT_ChangeMapRailLocation( gsys, fieldmap,
           zone_id, &loc, dir, FALSE );
    }         
    GMEVENT_CallEvent( parent_event, mapchange_event );
  }

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * レールマップに遷移ノーフェード
 * @param	core		仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdRailMapChangeNoFade( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK * gsys = SCRIPT_GetGameSysWork( sc );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );

  u16 zone_id = VMGetU16(core); //2byte read  ZONE指定
  u16 rail_idx = VMGetU16(core);  //2byte read  レールインデックス
  u16 front_idx = VMGetU16(core);  //2byte read  フロントインデックス
  u16 side_idx = VMGetU16(core);  //2byte read  サイドインデックス
  u16 dir = VMGetU16(core);     //2byte read  方向
  
  {
    GMEVENT * mapchange_event;
    GMEVENT * parent_event;
    RAIL_LOCATION loc;

    parent_event = GAMESYSTEM_GetEvent( gsys ); //現在のイベント
    {
      FLDNOGRID_MAPPER* nogrid = FIELDMAP_GetFldNoGridMapper( fieldmap );
      FLDNOGRID_MAPPER_ChangeNotMinusRailPosToRailLocation(
          nogrid, zone_id, rail_idx, front_idx, side_idx, &loc, HEAPID_FIELDMAP );
      mapchange_event = EVENT_ChangeMapRailLocNoFade( gsys, fieldmap,
           zone_id, &loc, dir, FALSE );
    }         
    GMEVENT_CallEvent( parent_event, mapchange_event );
  }

  return VMCMD_RESULT_SUSPEND;
}



//======================================================================
//
//      ツール関数
//
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------

