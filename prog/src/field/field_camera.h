//============================================================================================
/**
 *	@file	field_camera.h
 *	@brief
 */
//============================================================================================
#pragma once

#include "../../../../resource/fldmapdata/camera_data/fieldcameraformat.h"


//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�J��������\���̂ւ̕s���S�^��`
 */
//------------------------------------------------------------------
typedef struct _FIELD_CAMERA	FIELD_CAMERA;


//�J�����g���[�X�}�X�N��`
#define CAM_TRACE_MASK_X	(1)
#define CAM_TRACE_MASK_Y	(2)
#define CAM_TRACE_MASK_Z	(4)
#define CAM_TRACE_MASK_ALL	(CAM_TRACE_MASK_X|CAM_TRACE_MASK_Y|CAM_TRACE_MASK_Z)


//-----------------------------------------------------------------------------
/**
 *			�t�B�[���h�J�������[�h
 */
//-----------------------------------------------------------------------------
typedef enum
{
	FIELD_CAMERA_MODE_CALC_CAMERA_POS,		// �J�������W���^�[�Q�b�g���W�ƃA���O������v�Z����
	FIELD_CAMERA_MODE_CALC_TARGET_POS,		// �^�[�Q�b�g���W���J�������W�ƃA���O������v�Z����
	FIELD_CAMERA_MODE_DIRECT_POS,					// �J�������W�A�^�[�Q�b�g���W�@���ڎw��

	FIELD_CAMERA_MODE_MAX,								// ���[�h�ő吔�i�V�X�e�����Ŏg�p�j
} FIELD_CAMERA_MODE;


//============================================================================================
//	�����E���C���E����
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
extern FIELD_CAMERA* FIELD_CAMERA_Create(
		u8 type,
		FIELD_CAMERA_MODE mode,
		GFL_G3D_CAMERA * cam,
		const VecFx32 * target,
		HEAPID heapID);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_CAMERA_Delete( FIELD_CAMERA* camera );

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_CAMERA_Main( FIELD_CAMERA* camera, u16 key_cont);



//============================================================================================
//============================================================================================
//------------------------------------------------------------------
//	�J�������[�h�̕ύX
//------------------------------------------------------------------
extern void FIELD_CAMERA_SetMode( FIELD_CAMERA * camera, FIELD_CAMERA_MODE mode );
extern FIELD_CAMERA_MODE FIELD_CAMERA_GetMode( const FIELD_CAMERA * camera );

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void FIELD_CAMERA_SetNear(FIELD_CAMERA * camera, fx32 near);
extern fx32 FIELD_CAMERA_GetNear(const FIELD_CAMERA * camera);
extern void FIELD_CAMERA_SetFar(FIELD_CAMERA * camera, fx32 far);
extern fx32 FIELD_CAMERA_GetFar(const FIELD_CAMERA * camera);


//-----------------------------------------------------------------------------
/**
 *			angle����
 *	�L���J�������[�h
 *					FIELD_CAMERA_MODE_CALC_CAMERA_POS,		// �J�������W���^�[�Q�b�g���W�ƃA���O������v�Z����
 *					FIELD_CAMERA_MODE_CALC_TARGET_POS,		// �^�[�Q�b�g���W���J�������W�ƃA���O������v�Z����
 *
 *	�wFIELD_CAMERA_MODE_CALC_CAMERA_POS�x
 *		�^�[�Q�b�g���W���猩���A�J�������W�̃A���O���ݒ�
 *
 *	�wFIELD_CAMERA_MODE_CALC_TARGET_POS�x
 *		�J�������W���猩���A�^�[�Q�b�g���W�̃A���O���ݒ�
 */
//-----------------------------------------------------------------------------
// �^�[�Q�b�g�𒆐S�Ƃ����A�A���O������
// Pitch  ����������]
// Yaw    ����������]
extern u16 FIELD_CAMERA_GetAnglePitch(const FIELD_CAMERA * camera );
extern void FIELD_CAMERA_SetAnglePitch(FIELD_CAMERA * camera, u16 angle );
extern u16 FIELD_CAMERA_GetAngleYaw(const FIELD_CAMERA * camera );
extern void FIELD_CAMERA_SetAngleYaw(FIELD_CAMERA * camera, u16 angle );
extern const u16 * FIELD_CAMERA_GetAngleYawAddress( const FIELD_CAMERA *camera );
extern fx32 FIELD_CAMERA_GetAngleLen(const FIELD_CAMERA * camera );
extern void FIELD_CAMERA_SetAngleLen(FIELD_CAMERA * camera, fx32 length );

//------------------------------------------------------------------
/**
 * @brief	�J���������_�̎擾
 * @param	camera		FIELD�J��������|�C���^
 * @param	pos			�J���������_���󂯎��VecFx32�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void	FIELD_CAMERA_GetTargetPos( const FIELD_CAMERA* camera, VecFx32* pos );

//------------------------------------------------------------------
/**
 * @brief	�J���������_�̐ݒ�
 * @param	camera		FIELD�J��������|�C���^
 * @param	pos			�J���������_��n��VecFx32�ւ̃|�C���^
 *
 *	�L���J�������[�h
 *					FIELD_CAMERA_MODE_CALC_CAMERA_POS,		// �J�������W���^�[�Q�b�g���W�ƃA���O������v�Z����
 *					FIELD_CAMERA_MODE_DIRECT_POS,					// �J�������W�A�^�[�Q�b�g���W�@���ڎw��
 */
//------------------------------------------------------------------
extern void	FIELD_CAMERA_SetTargetPos( FIELD_CAMERA* camera, const VecFx32* target );


//------------------------------------------------------------------
/**
 * @brief	�J���������_�@�␳���W�̎擾
 * @param	camera		    FIELD�J��������|�C���^
 * @param	target_offset	�J���������_��␳������W�󂯎��VecFx32�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void	FIELD_CAMERA_GetTargetOffset( const FIELD_CAMERA* camera, VecFx32* target_offset );


//------------------------------------------------------------------
/**
 * @brief	�J���������_�@�␳���W�̎擾
 * @param	camera		        FIELD�J��������|�C���^
 * @param	target_offset			�J���������_��␳������W�n��VecFx32�ւ̃|�C���^
 */
//------------------------------------------------------------------
extern void	FIELD_CAMERA_SetTargetOffset( FIELD_CAMERA* camera, const VecFx32* target_offset );

//------------------------------------------------------------------
//  �J�����ʒu�̎擾�E�Z�b�g
//  �J���������Ԃɂ���Ă͐��������f����Ă��Ȃ��̂Œ���
//------------------------------------------------------------------
extern void FIELD_CAMERA_GetCameraPos( const FIELD_CAMERA * camera, VecFx32 * camPos);
//
//�L���J�������[�h
//	FIELD_CAMERA_MODE_CALC_TARGET_POS,		// �^�[�Q�b�g���W���J�������W�ƃA���O������v�Z����
//	FIELD_CAMERA_MODE_DIRECT_POS,					// �J�������W�A�^�[�Q�b�g���W�@���ڎw��
extern void FIELD_CAMERA_SetCameraPos( FIELD_CAMERA * camera, const VecFx32 * camPos);

//------------------------------------------------------------------
//------------------------------------------------------------------
extern const GFL_G3D_CAMERA * FIELD_CAMERA_GetCameraPtr(const FIELD_CAMERA * camera);


//------------------------------------------------------------------
//�L���J�������[�h
//	FIELD_CAMERA_MODE_CALC_CAMERA_POS,		// �J�������W���^�[�Q�b�g���W�ƃA���O������v�Z����
//	FIELD_CAMERA_MODE_DIRECT_POS,					// �J�������W�A�^�[�Q�b�g���W�@���ڎw��
//------------------------------------------------------------------
extern void FIELD_CAMERA_BindTarget(FIELD_CAMERA * camera, const VecFx32 * watch_target);
extern void FIELD_CAMERA_BindDefaultTarget(FIELD_CAMERA * camera);
extern void FIELD_CAMERA_FreeTarget(FIELD_CAMERA * camera);
extern BOOL FIELD_CAMERA_IsBindDefaultTarget( const FIELD_CAMERA * camera );

//------------------------------------------------------------------
//	�J�������W�X�V�̒�~�i�f�o�b�N�p�H�j
//	DIRECT_POS���[�h�ɕύX���邱�ƂőΉ��\
//------------------------------------------------------------------
//extern void FIELD_CAMERA_BindNoCamera(FIELD_CAMERA * camera, BOOL flag);

//------------------------------------------------------------------
//	�J�������W�̃o�C���h
//�L���J�������[�h
//	FIELD_CAMERA_MODE_CALC_TARGET_POS,		// �^�[�Q�b�g���W���J�������W�ƃA���O������v�Z����
//	FIELD_CAMERA_MODE_DIRECT_POS,					// �J�������W�A�^�[�Q�b�g���W�@���ڎw��
//------------------------------------------------------------------
extern void FIELD_CAMERA_BindCamera(FIELD_CAMERA * camera, const VecFx32 * watch_camera);
extern void FIELD_CAMERA_FreeCamera(FIELD_CAMERA * camera);

#ifdef  PM_DEBUG
typedef enum{
  FIELD_CAMERA_DEBUG_BIND_NONE,
  FIELD_CAMERA_DEBUG_BIND_CAMERA_POS,
  FIELD_CAMERA_DEBUG_BIND_TARGET_POS,
} FIELD_CAMERA_DEBUG_BIND_TYPE;
//------------------------------------------------------------------
//  �f�o�b�O�p�F����ʑ���Ƃ̃o�C���h
//------------------------------------------------------------------
extern void FIELD_CAMERA_DEBUG_BindSubScreen(FIELD_CAMERA * camera, void * param, FIELD_CAMERA_DEBUG_BIND_TYPE type);
extern void FIELD_CAMERA_DEBUG_ReleaseSubScreen(FIELD_CAMERA * camera);
extern void FIELD_CAMERA_DEBUG_GetBindSubScreenTarget( const FIELD_CAMERA * camera, VecFx32* p_target );


//------------------------------------------------------------------
//  �f�t�H���g�^�[�Q�b�g����
//------------------------------------------------------------------
extern void FIELD_CAMERA_DEBUG_SetDefaultTarget( FIELD_CAMERA* camera, const VecFx32* target );
extern const VecFx32* FIELD_CAMERA_DEBUG_GetDefaultTarget( const FIELD_CAMERA* camera );

extern void FIELD_CAMERA_GetInitialParameter( const FIELD_CAMERA* camera, FLD_CAMERA_PARAM * result);
#endif

