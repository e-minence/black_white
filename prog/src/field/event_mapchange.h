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
#include "field/eventdata_sxy.h"

//======================================================================
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	�Q�[���J�n�C�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_SetFirstMapIn(GAMESYS_WORK * gsys, GAME_INIT_WORK * game_init_work);

//------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�p�F�Q�[���I��
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_GameEnd( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap);

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_ChangeMapByConnect(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    const CONNECT_DATA * cnct);

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�J�ڃC�x���g�����i���W�w��j
 * @param	gsys		�Q�[���V�X�e���ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�V�X�e���ւ̃|�C���^
 * @param	zone_id		�J�ڂ���}�b�v��ZONE�w��
 * @param	pos			�J�ڂ���}�b�v�ł̍��W�w��
 * @param	dir			�J�ڂ���}�b�v�ł̕����w��
 * @return	GMEVENT		���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_ChangeMapPos(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		u16 zone_id, const VecFx32 * pos, u16 dir );

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�J�ڃC�x���g�����i�f�t�H���g���W�j
 * @param	gsys		�Q�[���V�X�e���ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�V�X�e���ւ̃|�C���^
 * @param	zone_id		�J�ڂ���}�b�v��ZONE�w��
 * @return	GMEVENT		���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_ChangeMapDefaultPos(GAMESYS_WORK * gsys,
		FIELDMAP_WORK * fieldmap, u16 zone_id);

//------------------------------------------------------------------
/**
 * @brief	�}�b�v�J�ڃC�x���g�����i�f�t�H���g���W�j
 * @param	gsys		�Q�[���V�X�e���ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�V�X�e���ւ̃|�C���^
 * @param	zone_id		�J�ڂ���}�b�v��ZONE�w��
 * @return	GMEVENT		���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_ChangeMapDefaultPosByWarp(GAMESYS_WORK * gsys,
		FIELDMAP_WORK * fieldmap, u16 zone_id);

//------------------------------------------------------------------
/**
 * @brief	�����ɂ��}�b�v�J�ڃC�x���g�����i���W�w��j
 * @param	gsys		�Q�[���V�X�e���ւ̃|�C���^
 * @param	fieldmap	�t�B�[���h�V�X�e���ւ̃|�C���^
 * @param disappear_pos �������S�_�̍��W
 * @param	zone_id		�J�ڂ���}�b�v��ZONE�w��
 * @param appear �J�ڐ�ł̍��W
 * @return	GMEVENT		���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_ChangeMapBySandStream(
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, 
    const VecFx32* disappear_pos, u16 zone_id, const VecFx32* appear_pos );

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_ChangeToNextMap(GAMESYS_WORK * gsys, FIELDMAP_WORK *fieldmap);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void MAPCHANGE_setPlayerVanish(FIELDMAP_WORK * fieldmap, BOOL vanish_flag);

