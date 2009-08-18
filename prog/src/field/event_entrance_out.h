/////////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @breif  ��������̑ޏo���̃C�x���g�쐬
 * @author obata
 * @date   2009.08.18
 *
 */
/////////////////////////////////////////////////////////////////////////////////////////
#pragma once


//---------------------------------------------------------------------------------------
/**
 * @brief �����ޏo�C�x���g���쐬����
 *
 * @param parent    �e�C�x���g
 * @param gsys      �Q�[���V�X�e��
 * @param gdata     �Q�[���f�[�^
 * @param fieldmap  �t�B�[���h�}�b�v
 * @param location  �J�ڐ�w��
 *
 * @return �쐬�����C�x���g
 */
//---------------------------------------------------------------------------------------
GMEVENT* EVENT_EntranceOut( GMEVENT* parent, 
                           GAMESYS_WORK* gsys,
                           GAMEDATA* gdata, 
                           FIELD_MAIN_WORK* fieldmap, 
                           LOCATION location );
