//===========================================================================
/**
 * @file	location.c
 * @brief	マップ位置の初期化など
 * @date	2006.06.02
 * @author	tamada GAME FREAK inc.
 *
 * 2008.11.21	DPプロジェクトから移植
 */
//===========================================================================

//#include "common.h"

#include <gflib.h>

#include "field/zonedata.h"
#include "field/rail_location.h"
#include "field/location.h"

#include "field/field_const.h"
#include "field/field_dir.h"

#include "field/eventdata_type.h"

#include "field_rail.h"

//#include "mapdefine.h"		//ZONE_ID_〜
//#include "fieldobj_code.h"	//DIR_DOWN

//#include "player.h"
//#include "situation_local.h"

#include "arc/fieldmap/zone_id.h"
//===========================================================================
//===========================================================================
enum {
	ZONE_ID_GAMESTART = ZONE_ID_T01R0102,		//「さいしょから」の開始ゾーンID
};
//--------------------------------------------------------------
//--------------------------------------------------------------
#define DOOR_ID_JUMP_CODE	(-1)


//--------------------------------------------------------------
//prototype
//--------------------------------------------------------------
static u16 getExitDir_Dir( EXIT_DIR exit_dir );

//===========================================================================
//===========================================================================
//--------------------------------------------------------------
/**
 * @brief	LOCATIONの初期化
 */
//--------------------------------------------------------------
void LOCATION_Init(LOCATION * loc)
{
	loc->type = LOCATION_TYPE_INIT;
	loc->zone_id = 0;
	loc->exit_id = 0;
	loc->dir_id = 0;
  loc->exit_ofs = LOCATION_DEFAULT_EXIT_OFS;
	loc->location_pos.type = LOCATION_POS_TYPE_3D;
	loc->location_pos.pos.x = 0;
	loc->location_pos.pos.y = 0;
	loc->location_pos.pos.z = 0;
}

//--------------------------------------------------------------
/**
 * @brief	LOCATIONのセット
 */
//--------------------------------------------------------------
void LOCATION_Set(LOCATION * loc, int zone, s16 door, s16 dir, LOC_EXIT_OFS ofs, fx32 x, fx32 y, fx32 z)
{
	loc->type = LOCATION_TYPE_INIT;
	loc->zone_id = zone;
	loc->exit_id = door;
	loc->dir_id = dir;
  loc->exit_ofs = ofs;
	loc->location_pos.type = LOCATION_POS_TYPE_3D;
	loc->location_pos.pos.x = x;
	loc->location_pos.pos.y = y;
	loc->location_pos.pos.z = z;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ロケーションセット　レールロケーション
 */
//-----------------------------------------------------------------------------
void LOCATION_SetRail(LOCATION * loc, int zone, s16 door, s16 dir, LOC_EXIT_OFS ofs, u16 rail_index, u16 line_grid, s16 width_grid)
{
  u16 normal_dir;
  
	loc->type = LOCATION_TYPE_INIT;
	loc->zone_id = zone;
	loc->exit_id = door;
	loc->dir_id = dir;
  loc->exit_ofs = ofs;
	loc->location_pos.type = LOCATION_POS_TYPE_RAIL;
  loc->location_pos.railpos.type        = FIELD_RAIL_TYPE_LINE;
	loc->location_pos.railpos.rail_index  = rail_index;
	loc->location_pos.railpos.line_grid   = line_grid;
	loc->location_pos.railpos.width_grid  = width_grid;
  loc->location_pos.railpos.key         = FIELD_RAIL_TOOL_ConvertDirToRailKey( EXITDIR_toDIR(dir) );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
void LOCATION_SetID(LOCATION * loc, u16 zone_id, u16 exit_id, LOC_EXIT_OFS ofs)
{
	LOCATION_Init(loc);
	loc->zone_id = zone_id;
	loc->exit_id = exit_id;
  loc->exit_ofs = ofs;
}
//--------------------------------------------------------------
/**
 * @brief	LOCATIONのセット（直接位置指定）
 */
//--------------------------------------------------------------
void LOCATION_SetDirect(LOCATION * loc, int zone, s16 dir, fx32 x, fx32 y, fx32 z)
{
	loc->type = LOCATION_TYPE_DIRECT;
	loc->zone_id = zone;
	loc->exit_id = DOOR_ID_JUMP_CODE;
	loc->dir_id = dir;
  loc->exit_ofs = LOCATION_DEFAULT_EXIT_OFS;
  loc->location_pos.type  = LOCATION_POS_TYPE_3D;
	loc->location_pos.pos.x = x;
	loc->location_pos.pos.y = y;
	loc->location_pos.pos.z = z;
}

//----------------------------------------------------------------------------
/**
 * @brief	LOCATIONにレールロケーションをセット（直接位置指定）
 */
//-----------------------------------------------------------------------------
void LOCATION_SetDirectRail(LOCATION * loc, int zone, s16 dir, u16 rail_index, u16 line_grid, s16 width_grid)
{
	loc->type = LOCATION_TYPE_DIRECT;
	loc->zone_id = zone;
	loc->exit_id = DOOR_ID_JUMP_CODE;
	loc->dir_id = dir;
  loc->exit_ofs = LOCATION_DEFAULT_EXIT_OFS;
  loc->location_pos.type        = LOCATION_POS_TYPE_RAIL;
  loc->location_pos.railpos.type        = FIELD_RAIL_TYPE_LINE;
	loc->location_pos.railpos.rail_index  = rail_index;
	loc->location_pos.railpos.line_grid   = line_grid;
	loc->location_pos.railpos.width_grid  = width_grid;
  loc->location_pos.railpos.key         = FIELD_RAIL_TOOL_ConvertDirToRailKey( EXITDIR_toDIR(dir) );
}

//----------------------------------------------------------------------------
/**
 * @brief	LOCATION内の座標タイプを取得
 */
//-----------------------------------------------------------------------------
LOCATION_POS_TYPE LOCATION_GetPosType( const LOCATION * loc )
{
  return loc->location_pos.type;
}


//--------------------------------------------------------------
/**
 * @brief	LOCATIONから、３D座標の取得
 */
//--------------------------------------------------------------
void LOCATION_Get3DPos( const LOCATION * loc, VecFx32 * pos )
{
  GF_ASSERT( loc->location_pos.type == LOCATION_POS_TYPE_3D );
  VEC_Set( pos, 
      loc->location_pos.pos.x, loc->location_pos.pos.y, loc->location_pos.pos.z );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ３D座標の設定
 */
//-----------------------------------------------------------------------------
void LOCATION_Set3DPos( LOCATION * loc, const VecFx32 * pos )
{
  loc->location_pos.type  = LOCATION_POS_TYPE_3D;
  loc->location_pos.pos   = *pos;
}

//--------------------------------------------------------------
/**
 * @brief	LOCATIONから、レールロケーションの取得
 */
//--------------------------------------------------------------
void LOCATION_GetRailLocation( const LOCATION * loc, RAIL_LOCATION * location )
{
  GF_ASSERT( loc->location_pos.type == LOCATION_POS_TYPE_RAIL );
  *location = loc->location_pos.railpos;
}

//--------------------------------------------------------------
/**
 * @brief	LOCATIONに、レールロケーションの設定
 */
//--------------------------------------------------------------
void LOCATION_SetRailLocation( LOCATION * loc, const RAIL_LOCATION * location )
{
  loc->location_pos.type = LOCATION_POS_TYPE_RAIL;
  loc->location_pos.railpos = *location;
}




//------------------------------------------------------------------
// ZONEの基準位置にジャンプ
// 　空を飛ぶ、テレポートなどで使用
//------------------------------------------------------------------
void LOCATION_SetDefaultPos(LOCATION * loc, u16 zone_id)
{
  VecFx32 pos;
  
	LOCATION_Init(loc);
	loc->type = LOCATION_TYPE_DIRECT;
	loc->zone_id = zone_id;
	loc->exit_id = DOOR_ID_JUMP_CODE;
	loc->dir_id = EXIT_DIR_DOWN;
  loc->exit_ofs = LOCATION_DEFAULT_EXIT_OFS;
  if( ZONEDATA_IsRailOnlyMap(zone_id) )
  {
    // レール用初期化
    ZONEDATA_GetStartRailPos(zone_id, &pos);
    loc->location_pos.type = LOCATION_POS_TYPE_RAIL;
    loc->location_pos.railpos.type        = FIELD_RAIL_TYPE_LINE;
    loc->location_pos.railpos.rail_index  = pos.x;
    loc->location_pos.railpos.line_grid   = pos.y;
    loc->location_pos.railpos.width_grid  = pos.z;
  }
  else
  {
    // グリッド用初期化
    ZONEDATA_GetStartPos(zone_id, &pos);
    loc->location_pos.type = LOCATION_POS_TYPE_3D;
    loc->location_pos.pos = pos;
  }
}

//------------------------------------------------------------------
/**
 * @brief ゲーム開始位置をセット
 */
//------------------------------------------------------------------
void LOCATION_SetGameStart(LOCATION * loc)
{
  LOCATION_SetDefaultPos( loc, ZONE_ID_GAMESTART );
}


//----------------------------------------------------------------------------
/**
 *	@brief  exit_dir --> dirへの変換
 *	@param	exit_dir 
 *	@return u16
 */
//-----------------------------------------------------------------------------
u16 EXITDIR_toDIR( EXIT_DIR exit_dir )
{
  u16 normal_dir;

  // dirを変換
  switch( exit_dir )
  {
	case EXIT_DIR_UP:
    normal_dir = DIR_UP;
    break;
	case EXIT_DIR_DOWN:
    normal_dir = DIR_DOWN;
    break;
	case EXIT_DIR_LEFT:
    normal_dir = DIR_LEFT;
    break;
	case EXIT_DIR_RIGHT:
    normal_dir = DIR_RIGHT;
    break;
  default:
    GF_ASSERT( 0 );
  case EXIT_DIR_NON:
    normal_dir = DIR_DOWN;
    break;
  }
  return normal_dir;
}

//-----------------------------------------------------------------------------
/**
 * @brief dir --> exit_dirへの変換
 * @param dir
 * @return  EXIT_DIR
 */
//-----------------------------------------------------------------------------
EXIT_DIR EXITDIR_fromDIR( u16 dir )
{
  switch ( dir )
  {
  case DIR_UP:
    return EXIT_DIR_UP;
  case DIR_DOWN:
    return EXIT_DIR_DOWN;
  case DIR_LEFT:
    return EXIT_DIR_LEFT;
  case DIR_RIGHT:
    return EXIT_DIR_RIGHT;
  default:
    return EXIT_DIR_DOWN;
  }
}


