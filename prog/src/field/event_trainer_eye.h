//======================================================================
/**
 * @file	event_trainer_eye.h
 * @brief  �g���[�i�[�����C�x���g
 * @author  kagaya
 * @date  05.10.03
 */
//======================================================================
#pragma once
#include <gflib.h>

#include "system/gfl_use.h"
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "field/fieldmap_proc.h"

#include "trainer_eye_data.h"

//======================================================================
//	define
//======================================================================
#define EYE_CHECK_NOHIT (-1) ///<�����͈̓`�F�b�N �q�b�g����

//======================================================================
//	struct
//======================================================================

//======================================================================
//	extern
//======================================================================
extern GMEVENT * EVENT_CheckTrainerEye( FIELDMAP_WORK *fieldMap, BOOL vs2 );
extern int EVENT_CheckTrainerEyeRange(
    const MMDL *mmdl, u16 eye_dir, int eye_range, const FIELD_ENCOUNT *enc );

extern u16 EVENT_GetTrainerEyeTrainerID( const MMDL *mmdl );

extern GMEVENT * EVENT_SetTrainerEyeMove(
    FIELDMAP_WORK *fieldMap, const TRAINER_HITDATA * hitdata, int gyoe, u32 work_pos );
