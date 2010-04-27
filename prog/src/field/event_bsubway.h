//======================================================================
/**
 * @file  event_bsubway.h
 * @brief  �o�g���T�u�E�F�C�@�C�x���g
 */
//======================================================================
#pragma once
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "event_fieldmap_control.h"
#include "app/pokelist.h"
#include "app/p_status.h"

#include "bsubway_scr.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern GMEVENT * BSUBWAY_EVENT_SetSelectPokeList(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys, BOOL btl_box );
extern GMEVENT * BSUBWAY_EVENT_TrainerBattle(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap );
extern GMEVENT * BSUBWAY_EVENT_CommBattle(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap );
extern GMEVENT * BSUBWAY_EVENT_TrainerBeforeMsg(
    BSUBWAY_SCRWORK *bsw_scr, GAMESYS_WORK *gsys,
    int tr_idx, u16 obj_id, u8 pos_type );

//======================================================================
//  ��ナ�[�_�[���̕\��
//======================================================================
extern GMEVENT * BSUBWAY_EVENT_CallLeaderBoard(
    GAMESYS_WORK *gsys );

extern GMEVENT * BSUBWAY_EVENT_MsgWifiHomeNPC(
    GAMESYS_WORK *gsys, u16 leader_no, u16 obj_id );
