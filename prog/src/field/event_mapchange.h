//======================================================================
/**
 * @file  event_mapchange.h
 * @brief �}�b�v�J�ڊ֘A�C�x���g
 * @date  2008.11.05
 * @author  tamada GAME FREAK inc.
 *
 * @todo  DEBUG_�Ƃ������O�̖{�ԗp�֐������l�[������
 * @todo  �V�K�Q�[���ƃR���e�B�j���[�Ƃœ���֐����Ăяo���Ă���̂�؂蕪����
 *
 * @note
 * ��{�I�ɂ̓C�x���g�݂̂��O�����J���ĕ��G�ȏ�������t���O�����
 * ���̓����ɔ[�߂����B�i���݂̂Ƃ����O��MAPCHG_GameOver�j
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
 * @brief �Q�[���J�n�C�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_CallGameStart(GAMESYS_WORK * gsys, GAME_INIT_WORK * game_init_work);

//------------------------------------------------------------------
/**
 * @brief �f�o�b�O�p�F�Q�[���I��
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_CallGameEnd( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap);

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_ChangeMapByConnect(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    const CONNECT_DATA * cnct, LOC_EXIT_OFS exit_ofs);

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i���W�w��j
 * @param gsys    �Q�[���V�X�e���ւ̃|�C���^
 * @param fieldmap  �t�B�[���h�V�X�e���ւ̃|�C���^
 * @param zone_id   �J�ڂ���}�b�v��ZONE�w��
 * @param pos     �J�ڂ���}�b�v�ł̍��W�w��
 * @param dir     �J�ڂ���}�b�v�ł̕����w��
 * @return  GMEVENT   ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_ChangeMapPos(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    u16 zone_id, const VecFx32 * pos, u16 dir );

//------------------------------------------------------------------
/**
 * @brief ����ԃ}�b�v�J�ڃC�x���g�����i���W�w��j
 * @param gsys    �Q�[���V�X�e���ւ̃|�C���^
 * @param fieldmap  �t�B�[���h�V�X�e���ւ̃|�C���^
 * @param zone_id   �J�ڂ���}�b�v��ZONE�w��
 * @param pos     �J�ڂ���}�b�v�ł̍��W�w��
 * @param dir     �J�ڂ���}�b�v�ł̕����w��
 * @return  GMEVENT   ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_ChangeMapSorawotobu(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    u16 zone_id, const VecFx32 * pos, u16 dir );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i���[�����W�w��j
 * @param gsys    �Q�[���V�X�e���ւ̃|�C���^
 * @param fieldmap  �t�B�[���h�V�X�e���ւ̃|�C���^
 * @param zone_id   �J�ڂ���}�b�v��ZONE�w��
 * @param rail_loc  �J�ڂ���}�b�v�ł̃��[�����W�w��
 * @param dir     �J�ڂ���}�b�v�ł̕����w��
 * @return  GMEVENT   ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_ChangeMapRailLocation(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    u16 zone_id, const RAIL_LOCATION * rail_loc, u16 dir );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i�f�t�H���g���W�j
 * @param gsys    �Q�[���V�X�e���ւ̃|�C���^
 * @param fieldmap  �t�B�[���h�V�X�e���ւ̃|�C���^
 * @param zone_id   �J�ڂ���}�b�v��ZONE�w��
 * @return  GMEVENT   ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_ChangeMapDefaultPos(GAMESYS_WORK * gsys,
    FIELDMAP_WORK * fieldmap, u16 zone_id);

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i�f�t�H���g���W, �t�F�[�h�Z�k�Łj
 * @param gsys    �Q�[���V�X�e���ւ̃|�C���^
 * @param fieldmap  �t�B�[���h�V�X�e���ւ̃|�C���^
 * @param zone_id   �J�ڂ���}�b�v��ZONE�w��
 * @return  GMEVENT   ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_QuickChangeMapDefaultPos(GAMESYS_WORK * gsys,
    FIELDMAP_WORK * fieldmap, u16 zone_id);

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i ���� �j
 * @param gsys          �Q�[���V�X�e���ւ̃|�C���^
 * @param fieldmap      �t�B�[���h�V�X�e���ւ̃|�C���^
 * @param disappear_pos �������S�_�̍��W
 * @param zone_id       �J�ڂ���}�b�v��ZONE�w��
 * @param appear        �J�ڐ�ł̍��W
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_ChangeMapBySandStream(
    GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, 
    const VecFx32* disappear_pos, u16 zone_id, const VecFx32* appear_pos );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i ���Ȃʂ��̃q�� �j
 * @param gsys �Q�[���V�X�e���ւ̃|�C���^
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapByAnanukenohimo( FIELDMAP_WORK *fieldWork, GAMESYS_WORK * gsys );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i ���Ȃ��ق� )
 * @param gsys �Q�[���V�X�e���ւ̃|�C���^
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapByAnawohoru( GAMESYS_WORK * gsys );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i �e���|�[�g )
 * @param gsys �Q�[���V�X�e���ւ̃|�C���^
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapByTeleport( GAMESYS_WORK * gsys );

//------------------------------------------------------------------
/**
 * @brief ���j�I���ւ̃}�b�v�J�ڃC�x���g����
 * @param  gsys      �Q�[���V�X�e���ւ̃|�C���^
 * @param  fieldmap  �t�B�[���h�V�X�e���ւ̃|�C���^
 * @return GMEVENT   ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_ChangeMapToUnion( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap );

extern GMEVENT * EVENT_ChangeMapFromUnion( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap );

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_ChangeToNextMap(GAMESYS_WORK * gsys, FIELDMAP_WORK *fieldmap);

extern GMEVENT * EVENT_ChangeMapPosNoFade(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
    u16 zone_id, const VecFx32 * pos, u16 dir );

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void MAPCHANGE_setPlayerVanish(FIELDMAP_WORK * fieldmap, BOOL vanish_flag);

//--------------------------------------------------------------
/**
 * @brief �S�Ŏ��̃}�b�v�J�ڏ����i�t�B�[���h�񐶐����j
 */
//--------------------------------------------------------------
extern GMEVENT * EVENT_CallGameOver( GAMESYS_WORK * gsys );


