////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �t�B�[���h�^�X�N(���@�̉�]����)
 * @file   field_task_player_rot.h
 * @author obata
 * @date   2009.11.18
 */
////////////////////////////////////////////////////////////////////////////////////////////


//------------------------------------------------------------------------------------------
/**
 * @brief ���@�̓�����]�^�X�N���쐬����
 *
 * @param fieldmap �^�X�N��ǉ�����t�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param rot_num  ��]��
 */
//------------------------------------------------------------------------------------------
extern FIELD_TASK* FIELD_TASK_PlayerRotate( FIELDMAP_WORK* fieldmap, int frame, int rot_num );
extern FIELD_TASK* FIELD_TASK_PlayerRotateEX( FIELDMAP_WORK* fieldmap, int frame, int rot_num, MMDL* mmdl );

//------------------------------------------------------------------------------------------
/**
 * @brief ���@�̉�����]�^�X�N���쐬����
 *
 * @param fieldmap �^�X�N��ǉ�����t�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param rot_num  ��]��
 */
//------------------------------------------------------------------------------------------
extern FIELD_TASK* FIELD_TASK_PlayerRotate_SpeedUp( FIELDMAP_WORK* fieldmap, int frame, int rot_num );

//------------------------------------------------------------------------------------------
/**
 * @brief ���@�̌�����]�^�X�N���쐬����
 *
 * @param fieldmap �^�X�N��ǉ�����t�B�[���h�}�b�v
 * @param frame    ����t���[����
 * @param rot_num  ��]��
 */
//------------------------------------------------------------------------------------------
extern FIELD_TASK* FIELD_TASK_PlayerRotate_SpeedDown( FIELDMAP_WORK* fieldmap, int frame, int rot_num );
extern FIELD_TASK* FIELD_TASK_PlayerRotate_SpeedDownDir( FIELDMAP_WORK* fieldmap, int frame, int rot_num , int endDir );
