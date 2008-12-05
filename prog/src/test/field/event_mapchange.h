//======================================================================
/**
 * @file	event_mapchange.h
 * @brief
 * @date	2008.11.05
 * @author	tamada GAME FREAK inc.
 */
//======================================================================
#pragma once

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "field/fieldmap.h"

//------------------------------------------------------------------
/**
 * @brief	�Q�[���J�n�C�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_SetFirstMapIn(GAMESYS_WORK * gsys, GAME_INIT_WORK * game_init_work);

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_ChangeMap(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,
		const LOCATION * loc_req);
//------------------------------------------------------------------
/**
 * @brief	�}�b�v�J�ڃC�x���g�����i�o�����w��j
 * @param	gsys		�Q�[���V�X�e���ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�V�X�e���ւ̃|�C���^
 * @param	zone_id		�J�ڂ���}�b�v��ZONE�w��
 * @param	exit_id		�J�ڂ���}�b�v�̏o�����w��
 * @return	GMEVENT		���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_ChangeMap(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,
		u16 zone_id, s16 exit_id);

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�J�ڃC�x���g�����i���W�w��j
 * @param	gsys		�Q�[���V�X�e���ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�V�X�e���ւ̃|�C���^
 * @param	zone_id		�J�ڂ���}�b�v��ZONE�w��
 * @param	pos			�J�ڂ���}�b�v�ł̍��W�w��
 * @return	GMEVENT		���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_ChangeMapPos(GAMESYS_WORK * gsys, FIELD_MAIN_WORK * fieldmap,
		u16 zone_id, const VecFx32 * pos);

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_ChangeToNextMap(GAMESYS_WORK * gsys, FIELD_MAIN_WORK *fieldmap);

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern void DEBUG_EVENT_ChangeEventMapChange( GAMESYS_WORK *gsys, GMEVENT *event,FIELD_MAIN_WORK *fieldmap, ZONEID zone_id );

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_FieldSample(GAMESYS_WORK * gsys, FIELD_MAIN_WORK *fieldmap);

