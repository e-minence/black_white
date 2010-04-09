//======================================================================
/**
 * @file	musical_camera_def.h
 * @brief	�~���[�W�J��3D�J�����n�ݒ�
 * @author	ariizumi
 * @data	09/03/05
 */
//======================================================================

#ifndef MUSICAL_CAMERA_DEF_H__
#define MUSICAL_CAMERA_DEF_H__

// �J�����̐ݒ���e�p�[�g��musical_mcss�Ƌ��L����K�v������̂�
// �w�b�_�Ƃ��Ē�`�B

#define MUSICAL_CAMERA_POS_X (FX32_CONST(0.0f))
#define MUSICAL_CAMERA_POS_Y (FX32_CONST(0.0f))
#define MUSICAL_CAMERA_POS_Z (FX32_CONST(301.0f))
#define MUSICAL_CAMERA_TRG_X (FX32_CONST(0.0f))
#define MUSICAL_CAMERA_TRG_Y (FX32_CONST(0.0f))
#define MUSICAL_CAMERA_TRG_Z (FX32_CONST(0.0f))

#define MUSICAL_CAMERA_POS {MUSICAL_CAMERA_POS_X,MUSICAL_CAMERA_POS_Y,MUSICAL_CAMERA_POS_Z}
#define MUSICAL_CAMERA_TRG {MUSICAL_CAMERA_TRG_X,MUSICAL_CAMERA_TRG_Y,MUSICAL_CAMERA_TRG_Z}
#define MUSICAL_CAMERA_UP  {0,FX32_ONE,0}

#define MUSICAL_CAMERA_NEAR (FX32_ONE)
#define MUSICAL_CAMERA_FAR  (FX32_ONE*1000)

//BBD�p���W�ϊ�(�J�����̕�����v�Z
#define MUSICAL_POS_X(val)		FX32_CONST((val)/16.0f)
#define MUSICAL_POS_Y(val)		FX32_CONST((192.0f-(val))/16.0f)
#define MUSICAL_POS_X_FX(val)	((val)/16)
#define MUSICAL_POS_Y_FX(val)	(FX32_CONST(192.0f)-(val))/16

#endif MUSICAL_CAMERA_DEF_H__

