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
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapByConnect( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                          const CONNECT_DATA* connectData, LOC_EXIT_OFS exitOfs );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g���� ( ���W�w�� )
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID             �J�ڐ�}�b�v��ZONE�w��
 * @param pos                �J�ڐ�}�b�v�ł̍��W�w��
 * @param dir                �J�ڐ�}�b�v�ł̕����w��
 * @param seasonUpdateEnable �G�߂̍X�V�������s�����ǂ���
 *
 * @return ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapPos( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                    u16 zoneID, const VecFx32* pos, u16 dir, 
                                    BOOL seasonUpdateEnable );

//------------------------------------------------------------------
/**
 * @brief ����ԃ}�b�v�J�ڃC�x���g���� ( ���W�w�� )
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID      �J�ڐ�}�b�v��ZONE�w��
 * @param loc         �J�ڐ�}�b�v�ł̃��P�[�V�����w��
 * @param dir         �J�ڐ�}�b�v�ł̕����w��
 *
 * @return ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapSorawotobu( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                           u16 zoneID, const LOCATION* loc, u16 dir );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g���� ( ���[�����W�w�� )
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID    �J�ڐ�}�b�v��ZONE�w��
 * @param rail_loc  �J�ڐ�}�b�v�ł̃��[�����W�w��
 * @param dir       �J�ڐ�}�b�v�ł̕����w��
 *
 * @return ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_ChangeMapRailLocation( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                                   u16 zoneID, const RAIL_LOCATION* rail_loc, u16 dir );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g���� ( �f�t�H���g���W )
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID      �J�ڐ�}�b�v��ZONE�w��
 *
 * @return ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_ChangeMapDefaultPos( GAMESYS_WORK* gameSystem, 
                                                 FIELDMAP_WORK* fieldmap, u16 zoneID );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g���� ( �f�t�H���g���W / �t�F�[�h�Z�k )
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID    �J�ڐ�}�b�v��ZONE�w��
 *
 * @return ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_QuickChangeMapDefaultPos( GAMESYS_WORK * gameSystem,
                                                      FIELDMAP_WORK* fieldmap, u16 zoneID );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i ���� �j
 *
 * @param gameSystem
 * @param fieldmap
 * @param disappearPos  �������S�_�̍��W
 * @param zoneID        �J�ڐ�}�b�v��ZONE�w��
 * @param appearPos     �J�ڐ�ł̍��W�w��
 *
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapBySandStream( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,  
                                             const VecFx32* disappearPos, 
                                             u16 zoneID, const VecFx32* appearPos );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g���� ( ���Ȃʂ��̃q�� / ���Ȃ��ق� / �e���|�[�g/�C��_�a����̑ޏ� )
 *
 * @param gameSystem
 *
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapByAnanukenohimo( FIELDMAP_WORK* fieldWork, GAMESYS_WORK* gameSystem );
extern GMEVENT* EVENT_ChangeMapByAnawohoru( GAMESYS_WORK* gameSystem );
extern GMEVENT* EVENT_ChangeMapByTeleport( GAMESYS_WORK* gameSystem );
extern GMEVENT* EVENT_ChangeMapBySeaTemple( GAMESYS_WORK* gameSystem );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g�����i ���[�v )
 * @param gameSystem
 * @param fieldmap
 * @param zoneID 
 * @return GMEVENT ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapByWarp( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                       u16 zoneID , const VecFx32 * pos, u16 dir );

//------------------------------------------------------------------
/**
 * @brief ���j�I���ւ̃}�b�v�J�ڃC�x���g����
 *
 * @param  gameSystem
 * @param  fieldmap
 *
 * @return ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapToUnion( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap );
extern GMEVENT* EVENT_ChangeMapFromUnion( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap );

//------------------------------------------------------------------
/**
 * @brief �p���X�E���t�B�[���h�ւ̃}�b�v�J�ڃC�x���g����
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapFldToPalace( GAMESYS_WORK* gsys, u16 zone_id, const VecFx32* pos );
extern GMEVENT * EVENT_ChangeMapFromPalace( GAMESYS_WORK * gameSystem );
extern GMEVENT * EVENT_ChangeMapPalace_to_Palace( GAMESYS_WORK* gameSystem, u16 zoneID, const VecFx32* pos );

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_ChangeToNextMap( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap );
extern GMEVENT* EVENT_ChangeMapPosNoFade( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                          u16 zoneID, const VecFx32* pos, u16 dir );

//------------------------------------------------------------------
/**
 * @brief �}�b�v�J�ڃC�x���g���� ( BGM�ύX�Ȃ� )
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID     �J�ڐ�}�b�v��ZONE�w��
 * @param pos        �J�ڐ�}�b�v�ł̍��W�w��
 * @param dir        �J�ڐ�}�b�v�ł̕����w��
 *
 * @return ���������}�b�v�J�ڃC�x���g
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapBGMKeep( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                        u16 zoneID, const VecFx32* pos, u16 dir );

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void MAPCHANGE_setPlayerVanish(FIELDMAP_WORK * fieldmap, BOOL vanish_flag);

//--------------------------------------------------------------
/**
 * @brief �S�Ŏ��̃}�b�v�J�ڏ����i�t�B�[���h�񐶐����j
 */
//--------------------------------------------------------------
extern GMEVENT * EVENT_CallGameOver( GAMESYS_WORK * gsys ); 
