////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �G�ߕ\���C�x���g
 * @file   event_season_display.h
 * @author obata
 * @date   2009.11.25
 */
////////////////////////////////////////////////////////////////////////////////
#pragma once

//------------------------------------------------------------------------------
/**
 * @brief �l�G�\���C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param fieldmap 
 * @param start    �J�n�G��
 * @param end      �ŏI�G�� 
 *
 * @return �}�b�v�`�F���W�Ƃ��ċ@�\����l�G�\���C�x���g
 */
//------------------------------------------------------------------------------
extern GMEVENT* EVENT_SeasonDisplay( 
    GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, u8 start, u8 end );

//------------------------------------------------------------------------------
/**
 * @brief �l�G�\���C�x���g�𐶐�����
 *
 * @param gameSystem
 * @param start    �J�n�G��
 * @param end      �ŏI�G�� 
 *
 * @return �P�Ɠ����z�肵���l�G�\���C�x���g
 */
//------------------------------------------------------------------------------
extern GMEVENT* EVENT_SimpleSeasonDisplay( GAMESYS_WORK* gameSystem, u8 start, u8 end );
