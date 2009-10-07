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


//==============================================================================
//  定数定義
//==============================================================================
///パレスマップの範囲(この座標外に出た場合、ワープさせる必要がある)
enum{
  PALACE_MAP_RANGE_LEFT_X = 136,  //1016/4,
  PALACE_MAP_RANGE_RIGHT_X = 2952,  //2056+PALACE_MAP_RANGE_LEFT_X,

  PALACE_MAP_RANGE_LEN = 2056-1016,     ///<パレスマップのX長
};

//==============================================================================
//  外部関数宣言
//==============================================================================
extern void IntrudeField_UpdateCommSystem( FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor);


//--------------------------------------------------------------
//  デバッグ用
//--------------------------------------------------------------
extern GMEVENT * DEBUG_IntrudeTreeMapWarp(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor, INTRUDE_COMM_SYS_PTR intcomm);
extern GMEVENT * DEBUG_PalaceJamp(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor);
