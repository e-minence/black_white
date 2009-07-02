//======================================================================
/**
 * @file	event_trainer_eye.h
 * @brief  �g���[�i�[�����C�x���g
 * @author  kagaya
 * @data  05.10.03
 */
//======================================================================
#pragma once
#include <gflib.h>

#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "field/fieldmap_proc.h"

//======================================================================
//	define
//======================================================================

//======================================================================
//	struct
//======================================================================

//======================================================================
//	extern
//======================================================================
extern GMEVENT * EVENT_CheckTrainerEye( FIELDMAP_WORK *fieldMap, BOOL vs2 );

extern GMEVENT * EVENT_SetTrainerEyeMove( FIELDMAP_WORK *fieldMap,
    MMDL *mmdl, FIELD_PLAYER *jiki,
    int dir, int range, int gyoe, int tr_type, int work_pos );
