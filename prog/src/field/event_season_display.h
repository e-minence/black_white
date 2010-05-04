////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �G�ߕ\���C�x���g
 * @file   event_season_display.h
 * @author obata
 * @date   2009.11.25
 */
////////////////////////////////////////////////////////////////////////////////
#pragma once

// �R�[���o�b�N�֐��̌^
typedef void (*SEASON_DISP_CALLBACK_FUNC)(void* callback_param);


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

// �R�[���o�b�N�֐�����ver. ( �t�B�[���h��ʂ��t�F�[�h�C�����钼�O�ɃR�[������� )
extern GMEVENT* EVENT_SeasonDisplay_Callback( 
    GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, u8 start, u8 end,
    SEASON_DISP_CALLBACK_FUNC callback_func, void* callback_param );

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
