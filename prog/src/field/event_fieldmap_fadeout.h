/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �t�F�[�h�A�E�g�C�x���g�̍쐬
 * @author obata
 * @date   2009.08.18
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//---------------------------------------------------------------------------------------
/**
 * @breif �t�F�[�h�A�E�g�E�C�x���g���쐬����
 *
 * @param parent    �e�C�x���g
 * @param gsys      �Q�[���V�X�e��
 * @param gdata     �Q�[���f�[�^
 * @param fieldmap  �t�B�[���h�}�b�v
 * @param location  �J�ڐ�w��
 * @param exit_type �J�ڃ^�C�v�w��
 *
 * @return �쐬�����C�x���g
 */
//---------------------------------------------------------------------------------------
GMEVENT* EVENT_SwitchFieldFadeOut( GMEVENT* parent, 
                                   GAMESYS_WORK* gsys,
                                   GAMEDATA* gdata, 
                                   FIELD_MAIN_WORK* fieldmap, 
                                   LOCATION location, 
                                   EXIT_TYPE exit_type );
