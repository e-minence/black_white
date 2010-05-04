////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N ( ���Ȃق�G�t�F�N�g )
 * @file   field_task_anahori_effect.h
 * @author obata
 * @date   2010.05.03
 */
////////////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "fieldmap.h"
#include "field_task.h"

//------------------------------------------------------------------------------------------
/**
 * @brief ���@��G�t�F�N�g�\���^�X�N�𐶐�����
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param mmdl     ����Ώۂ̓��샂�f��
 * @param frame    �^�X�N����t���[����
 * @param interval �G�t�F�N�g�\���Ԋu
 *
 * @return �쐬�����t�B�[���h�^�X�N
 */
//------------------------------------------------------------------------------------------
extern FIELD_TASK* FIELD_TASK_AnahoriEffect( 
    FIELDMAP_WORK* fieldmap, MMDL* mmdl, int frame, int interval );
