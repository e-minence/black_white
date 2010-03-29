///////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ���O���̓C�x���g
 * @file   event_name_input.h
 * @author obata
 * @date   2009.10.20
 */
///////////////////////////////////////////////////////////////////////////////////////
#pragma once


//-------------------------------------------------------------------------------------
/**
 * @brief �莝���|�P���������͉�ʌĂяo���C�x���g
 * 
 * @param gameSystem
 * @param ret_wk      ���͌��ʂ��󂯎�郏�[�N ( TRUE:�m��, FALSE:�L�����Z�� )
 * @param party_index ���O����͂���|�P�����̃p�[�e�B�[���C���f�b�N�X
 * @param fade_flag   �A�v���Ăяo���O��ɉ�ʃt�F�[�h����Ԃ��ǂ���
 *
 * @return �C�x���g
 */
//-------------------------------------------------------------------------------------
extern GMEVENT* EVENT_NameInput_PartyPoke( 
    GAMESYS_WORK* gameSystem, u16* ret_wk, u16 party_index, BOOL fade_flag );
