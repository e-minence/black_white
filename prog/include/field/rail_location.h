//============================================================================================
/**
 * @file  rail_location.h
 * @author  tamada GAME FREAK inc.
 * @date  2009.05.27
 */
//============================================================================================
#pragma once

//------------------------------------------------------------------
/// ���[���ʒu���
//  8byte
//------------------------------------------------------------------
typedef struct {
  u16 rail_index; //index of point or line
  u8 type;        // FIELD_RAIL_TYPE
  u8 key;         // RAIL_KEY
  s8 width_grid;   // RAIL�O���b�h
  s8 line_grid;    // RAIL�O���b�h

  u8 pad[2];
} RAIL_LOCATION;

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void RAIL_LOCATION_Init(RAIL_LOCATION * railLoc);


//------------------------------------------------------------------
// RAIL_KEY��DIR�̕ϊ�
//  
//  DIR_�`��RAIL_KEY_�`�ɕϊ�
//------------------------------------------------------------------
// RAIL_KEY to DIR
extern u32 FIELD_RAIL_TOOL_ConvertRailKeyToDir( u32 key );
// DIR to RAIL_KEY
extern u32 FIELD_RAIL_TOOL_ConvertDirToRailKey( u32 dir );

