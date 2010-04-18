//==============================================================================
/**
 * @file    intrude_field.h
 * @brief   侵入でフィールド時に行うメインのヘッダ
 * @author  matsuda
 * @date    2009.09.03(木)
 */
//==============================================================================
#pragma once

#include "field/field_comm/intrude_types.h"
#include "field/field_const.h"


//==============================================================================
//  定数定義
//==============================================================================
///パレスマップの範囲(この座標外に出た場合、ワープさせる必要がある)
enum{
  PALACE_MAP_LEFT = 0 << FX32_SHIFT,              ///<パレスマップ左端
  PALACE_MAP_RIGHT = 1024 << FX32_SHIFT,          ///<パレスマップ右端
  PALACE_MAP_LEN = PALACE_MAP_RIGHT - PALACE_MAP_LEFT,  ///<パレスマップ幅
  PALACE_MAP_LEN_GRID = PALACE_MAP_LEN / FIELD_CONST_GRID_FX32_SIZE,
  
  PALACE_MAP_WARP_OFFSET = 0, //(PALACE_MAP_LEN / 4) * 3,///残りこれ以内の座標になったらワープ
  
//  PALACE_MAP_RANGE_LEN = (2056-1016) << FX32_SHIFT,     ///<パレスマップのX長
  
  PALACE_MAP_TREE_X = 504 << FX32_SHIFT,            ///<パレスの木 配置座標
  PALACE_MAP_TREE_Y = 32 << FX32_SHIFT,
  PALACE_MAP_TREE_Z = 440 << FX32_SHIFT,
  PALACE_MAP_MONOLITH_X = 488 << FX32_SHIFT,       ///<モノリス配置開始座標X
  PALACE_MAP_MONOLITH_Y = 32,
  PALACE_MAP_MONOLITH_Z = 472 << FX32_SHIFT,
  PALACE_MAP_MONOLITH_NUM = 3,                      ///<モノリスの連続設置数
};

//==============================================================================
//  構造体定義
//==============================================================================
///パレスゾーン設定データ
typedef struct{
  u16 zone_id;            ///<このデータ対象のゾーンID
  u16 warp_zone_id;       ///<ワープ先ゾーンID
  s16 warp_grid_x;        ///<ワープ先X座標
  s16 warp_grid_y;        ///<ワープ先Y座標
  s16 warp_grid_z;        ///<ワープ先Z座標
  u8 sub_x;               ///<下画面座標X
  u8 sub_y;               ///<下画面座標Y
}PALACE_ZONE_SETTING;


//==============================================================================
//  外部関数宣言
//==============================================================================
extern void IntrudeField_UpdateCommSystem( FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor);
extern BOOL IntrudeField_CheckTalk(INTRUDE_COMM_SYS_PTR intcomm, const FIELD_PLAYER *fld_player, u32 *hit_netid);
extern BOOL IntrudeField_CheckTalkedTo(INTRUDE_COMM_SYS_PTR intcomm, u32 *hit_netid);
extern void IntrudeField_ConnectMapInit(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, INTRUDE_COMM_SYS_PTR intcomm);
extern void IntrudeField_ConnectMap(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, INTRUDE_COMM_SYS_PTR intcomm);
extern void IntrudeField_ConnectMapOne(FIELDMAP_WORK *fieldWork);
extern void IntrudeField_PlayerDisguise(INTRUDE_COMM_SYS_PTR intcomm, GAMESYS_WORK *gsys, u32 disguise_code, u8 disguise_type, u8 disguise_sex);
extern GMEVENT * IntrudeField_CheckSecretItemEvent(GAMESYS_WORK *gsys, INTRUDE_COMM_SYS_PTR intcomm, const FIELD_PLAYER *pcActor);
extern GMEVENT * Intrude_CheckPosEvent(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor);
extern GMEVENT * Intrude_CheckPushEvent(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *pcActor, const VecFx32 *now_pos, const VecFx32 *front_pos, u16 player_dir);
extern BOOL IntrudeField_GetPalaceTownZoneID(u16 zone_id, VecFx32 *vec);
extern const PALACE_ZONE_SETTING * IntrudeField_GetZoneSettingData(u16 zone_id);
extern void IntrudeField_MonolithStatus_Init(MONOLITH_STATUS *monost);
extern BOOL IntrudeField_MonolithStatus_CheckOcc(const MONOLITH_STATUS *monost);
extern void IntrudeField_PalaceMMdlAllAdd(FIELDMAP_WORK *fieldWork);


//--------------------------------------------------------------
//  デバッグ用
//--------------------------------------------------------------
extern GMEVENT * DEBUG_PalaceJamp(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor);

#ifdef PM_DEBUG
extern GMEVENT * DEBUG_EVENT_PalaceBarrierMove(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *fld_player, u16 dir);
#endif
