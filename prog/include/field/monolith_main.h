//==============================================================================
/**
 * @file    monolith_main.h
 * @brief   モノリス画面メインのヘッダ
 * @author  matsuda
 * @date    2009.11.24(火)
 */
//==============================================================================
#pragma once

#include "field/intrude_common.h"
#include "field/field_comm/mission_types.h"


//==============================================================================
//  構造体定義
//==============================================================================
///モノリス画面呼び出しようのParentWork
typedef struct{
  GAMESYS_WORK *gsys;
  INTRUDE_COMM_SYS_PTR intcomm;
  MISSION_CHOICE_LIST list;     ///<選択候補のミッション一覧
  MISSION_DATA mdata;           ///<実行しているミッション
  
  u8 palace_area;
  u8 list_occ;                  ///<TRUE:ミッション一覧受信済み FALSE:未受信(通信相手が居ない)
  u8 padding[2];
}MONOLITH_PARENT_WORK;


//==============================================================================
//  外部データ
//==============================================================================
extern const GFL_PROC_DATA MonolithProcData;

//==============================================================================
//  OVERLAY
//==============================================================================
FS_EXTERN_OVERLAY(monolith);
