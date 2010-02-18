/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @file   event_entrance_out.h
 * @breif  �o��������̑ޏo���̃C�x���g�쐬
 * @author obata
 * @date   2009.08.18
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//---------------------------------------------------------------------------------------
/**
 * @brief �o�����ޏo�C�x���g���쐬����
 *
 * @param parent            �e�C�x���g
 * @param gameSystem
 * @param gameData
 * @param fieldmap
 * @param location          �J�ڐ�w��
 * @param prevZoneID        �J�ڑO�̃]�[��
 * @param seasonDisplayFlag �G�ߕ\�����s�����ǂ���
 * @param startSeason       �ŏ��ɕ\������G��
 * @param endtSeason        �Ō�ɕ\������G��
 *
 * @return �쐬�����C�x���g
 */
//---------------------------------------------------------------------------------------
GMEVENT* EVENT_EntranceOut( GMEVENT* parent, 
                            GAMESYS_WORK* gameSystem,
                            GAMEDATA* gameData, 
                            FIELDMAP_WORK* fieldmap, 
                            LOCATION location,
                            u16 prevZoneID,
                            BOOL seasonDisplayFlag,
                            u8 startSeason,
                            u8 endSeason );
