////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N(���@�̕`��I�t�Z�b�g�ړ�)
 * @file   field_task_player_drawoffset.h
 * @author obata
 * @date   2009.11.18
 */
////////////////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------------------
/**
 * @brief �v���C���[�̕`��I�t�Z�b�g�ړ��^�X�N���쐬����
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param move     �ړ��x�N�g��
 *
 * @return �쐬�����t�B�[���h�^�X�N
 */
//------------------------------------------------------------------------------------------
extern FIELD_TASK* FIELD_TASK_TransDrawOffset( FIELDMAP_WORK* fieldmap, int frame, const VecFx32* vec ); 

//------------------------------------------------------------------------------------------
/**
 * @brief �w�肵�����샂�f���̃I�t�Z�b�g�ړ��^�X�N���쐬����
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param move     �ړ��x�N�g��
 * @param mmdl     ����Ώۂ̓��샂�f��
 *
 * @return �쐬�����t�B�[���h�^�X�N
 */
//------------------------------------------------------------------------------------------
extern FIELD_TASK* FIELD_TASK_TransDrawOffsetEX( FIELDMAP_WORK* fieldmap, int frame, const VecFx32* vec, MMDL* mmdl ); 

//------------------------------------------------------------------------------------------
/**
 * @brief �w�肵�����샂�f���̈ړ��^�X�N���쐬����
 *
 * @param fieldmap �^�X�N����Ώۂ̃t�B�[���h�}�b�v
 * @param frame    �^�X�N����t���[����
 * @param move     �ړ��x�N�g��
 * @param mmdl     ����Ώۂ̓��샂�f��
 *
 * @return �쐬�����t�B�[���h�^�X�N
 */
//------------------------------------------------------------------------------------------
extern FIELD_TASK* FIELD_TASK_TransPos( FIELDMAP_WORK* fieldmap, int frame, const VecFx32* vec, MMDL* mmdl );
