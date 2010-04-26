//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		event_seatemple.h
 *	@brief  �C���Ձ@�C�x���g�N��
 *	@author	tomoya takahashi 
 *	@date		2010.03.11
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "field/fieldmap_proc.h"

//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------
extern GMEVENT * EVENT_SeaTemple_GetStepEvent(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap );


// �J��
extern GMEVENT * EVENT_SeaTemple_GetDivingDownEvent(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap );

extern GMEVENT * EVENT_SeaTemple_GetDivingUpEvent(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BOOL kurukuru );


#ifdef _cplusplus
}	// extern "C"{
#endif



