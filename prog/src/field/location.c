//===========================================================================
/**
 * @file	location.c
 * @brief	�}�b�v�ʒu�̏������Ȃ�
 * @date	2006.06.02
 * @author	tamada GAME FREAK inc.
 *
 * 2008.11.21	DP�v���W�F�N�g����ڐA
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

//#include "mapdefine.h"		//ZONE_ID_�`
//#include "fieldobj_code.h"	//DIR_DOWN

//#include "player.h"
//#include "situation_local.h"

#include "arc/fieldmap/zone_id.h"
//===========================================================================
//===========================================================================
enum {
	ZONE_ID_GAMESTART = ZONE_ID_T01R0102,		//�Ƃ肠����
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
 * @brief	LOCATION�̏�����
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
 * @brief	LOCATION�̃Z�b�g
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
 *	@brief  ���P�[�V�����Z�b�g�@���[�����P�[�V����
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
  loc->location_pos.railpos.key         = FIELD_RAIL_TOOL_ConvertDirToRailKey( getExitDir_Dir(dir) );
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
 * @brief	LOCATION�̃Z�b�g�i���ڈʒu�w��j
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
 * @brief	LOCATION�Ƀ��[�����P�[�V�������Z�b�g�i���ڈʒu�w��j
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
  loc->location_pos.railpos.key         = FIELD_RAIL_TOOL_ConvertDirToRailKey( getExitDir_Dir(dir) );
}

//----------------------------------------------------------------------------
/**
 * @brief	LOCATION���̍��W�^�C�v���擾
 */
//-----------------------------------------------------------------------------
LOCATION_POS_TYPE LOCATION_GetPosType( const LOCATION * loc )
{
  return loc->location_pos.type;
}


//--------------------------------------------------------------
/**
 * @brief	LOCATION����A�RD���W�̎擾
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
 *	@brief  �RD���W�̐ݒ�
 */
//-----------------------------------------------------------------------------
void LOCATION_Set3DPos( LOCATION * loc, const VecFx32 * pos )
{
  loc->location_pos.type  = LOCATION_POS_TYPE_3D;
  loc->location_pos.pos   = *pos;
}

//--------------------------------------------------------------
/**
 * @brief	LOCATION����A���[�����P�[�V�����̎擾
 */
//--------------------------------------------------------------
void LOCATION_GetRailLocation( const LOCATION * loc, RAIL_LOCATION * location )
{
  GF_ASSERT( loc->location_pos.type == LOCATION_POS_TYPE_RAIL );
  *location = loc->location_pos.railpos;
}

//--------------------------------------------------------------
/**
 * @brief	LOCATION�ɁA���[�����P�[�V�����̐ݒ�
 */
//--------------------------------------------------------------
void LOCATION_SetRailLocation( LOCATION * loc, const RAIL_LOCATION * location )
{
  loc->location_pos.type = LOCATION_POS_TYPE_RAIL;
  loc->location_pos.railpos = *location;
}



#ifdef PM_DEBUG
//----------------------------------------------------------------------------
/**
 *	@brief  �����ȃ��[���}�b�v�`�F�b�N  �i�n�C�u���b�h���܂܂Ȃ��j
 *	  �f�o�b�N�p�̃`�F�b�N�Ȃ̂ŁA�����OK�Ƃ���
 */
//-----------------------------------------------------------------------------
static BOOL IsRailMap( u16 zone_id )
{
  switch( zone_id )
  {
  case ZONE_ID_C03:
  case ZONE_ID_H01:
  case ZONE_ID_C03P02:
  case ZONE_ID_D09:
    return TRUE;
  }

  return FALSE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void LOCATION_DEBUG_SetDefaultPos(LOCATION * loc, u16 zone_id)
{
  VecFx32 pos;
  
	LOCATION_Init(loc);
	loc->type = LOCATION_TYPE_DIRECT;
	loc->zone_id = zone_id;
	loc->exit_id = DOOR_ID_JUMP_CODE;
	loc->dir_id = 0;
  loc->exit_ofs = LOCATION_DEFAULT_EXIT_OFS;
  if( IsRailMap(zone_id) )
  {
    // ���[���p������
    ZONEDATA_GetStartRailPos(zone_id, &pos);
    loc->location_pos.type = LOCATION_POS_TYPE_RAIL;
    loc->location_pos.railpos.type        = FIELD_RAIL_TYPE_LINE;
    loc->location_pos.railpos.rail_index  = pos.x;
    loc->location_pos.railpos.line_grid   = pos.y;
    loc->location_pos.railpos.width_grid  = pos.z;
  }
  else
  {
    // �O���b�h�p������
    ZONEDATA_GetStartPos(zone_id, &pos);
    loc->location_pos.type = LOCATION_POS_TYPE_3D;
    loc->location_pos.pos = pos;
  }
}
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------
void LOCATION_SetGameStart(LOCATION * loc)
{
	//LOCATION_DEBUG_SetDefaultPos(loc, ZONE_ID_GAMESTART);
#if 1
  loc->type = LOCATION_TYPE_DIRECT;
  loc->zone_id = ZONE_ID_GAMESTART;
  loc->exit_id = DOOR_ID_JUMP_CODE;
  loc->dir_id = EXIT_DIR_DOWN;
  loc->exit_ofs = LOCATION_DEFAULT_EXIT_OFS;
  loc->location_pos.type = LOCATION_POS_TYPE_3D;
  loc->location_pos.pos.x = 72 * FX32_ONE;
  loc->location_pos.pos.y =  0 * FX32_ONE;
  loc->location_pos.pos.z = 88 * FX32_ONE;
#endif
}

#if 0
//===========================================================================
//===========================================================================
//------------------------------------------------------------------
//	�Q�[���J�n�ʒu�̒�`
//------------------------------------------------------------------
enum {
	//GAME_START_ZONE = ZONE_ID_T01R0102,
	GAME_START_ZONE = ZONE_ID_T01R0202,		//06.03.07�ύX
	GAME_START_X = 4,
	GAME_START_Z = 6,

	DEBUG_START_ZONE = ZONE_ID_C01,
	DEBUG_START_X = 174,
	DEBUG_START_Z = 764,

	GAME_START_EZONE = ZONE_ID_T01,
	GAME_START_EX = 116,
	GAME_START_EZ = 886,
};


#endif


//----------------------------------------------------------------------------
/**
 *	@brief  exit_dir�̕������擾
 *
 *	@param	exit_dir 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static u16 getExitDir_Dir( EXIT_DIR exit_dir )
{
  u16 normal_dir;

  // dir��ϊ�
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
    normal_dir = DIR_DOWN;
    break;
  }
  return normal_dir;
}

