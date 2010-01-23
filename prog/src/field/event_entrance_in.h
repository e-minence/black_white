/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file   event_entrance_in.h
 * @breif  �o�����ւ̐i�����̃C�x���g�쐬
 * @author obata
 * @date   2009.08.18
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//---------------------------------------------------------------------------------------
/**
 * @brief �o�����i���C�x���g���쐬����
 *
 * @param parent            �e�C�x���g
 * @param gameSystem
 * @param gameData
 * @param fieldmap
 * @param nextLocation      �J�ڐ�w��
 * @param exitType          �J�ڃ^�C�v�w��
 * @param seasonDisplayFlag �G�ߕ\�����s�����ǂ���
 *
 * @return �쐬�����C�x���g
 */
//---------------------------------------------------------------------------------------
extern GMEVENT* EVENT_EntranceIn( GMEVENT* parent, 
                                  GAMESYS_WORK* gameSystem,
                                  GAMEDATA* gameData, 
                                  FIELDMAP_WORK* fieldmap, 
                                  LOCATION nextLocation, 
                                  EXIT_TYPE exitType,
                                  BOOL seasonDisplayFlag );
