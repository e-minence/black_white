//============================================================================================
/**
 *	@file	field_camera.h
 *	@brief
 */
//============================================================================================
#pragma once

#include "../../../resource/fldmapdata/camera_data/fieldcameraformat.h"
#include "field_camera_mv_param.h"


//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�J��������\���̂ւ̕s���S�^��`
 */
//------------------------------------------------------------------
typedef struct _FIELD_CAMERA	FIELD_CAMERA;

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�J�����ړ��R�[���o�b�N�֐�
 */
//------------------------------------------------------------------
typedef void (*CAMERA_CALL_BACK)( FIELD_CAMERA* camera, void *work );

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


//-----------------------------------------------------------------------------
/**
 *			�J�������͈�
 */
//-----------------------------------------------------------------------------
// �͈̓^�C�v
typedef enum
{
// conv.pl�p��` [CAMERA_AREA]
  FIELD_CAMERA_AREA_NONE,     // �͈͂Ȃ�
  FIELD_CAMERA_AREA_RECT,     // ��`�͈�
  FIELD_CAMERA_AREA_CIRCLE,   // �~�͈�

  FIELD_CAMERA_AREA_MAX,   // �V�X�e�����Ŏg�p
} FIELD_CAMERA_AREA_TYPE;

// �Ǘ����W
typedef enum
{
// conv.pl�p��` [CAMERA_CONT]
  FIELD_CAMERA_AREA_CONT_TARGET,   // �J�����^�[�Q�b�g�̊Ǘ�
  FIELD_CAMERA_AREA_CONT_CAMERA,   // �J�������W�̊Ǘ�


  FIELD_CAMERA_AREA_CONT_MAX,   // �V�X�e�����Ŏg�p
} FIELD_CAMERA_AREA_CONT;

//-------------------------------------
///	��`�͈�
//=====================================
typedef struct {
  s32 x_min;  // �����_�Ȃ����W
  s32 x_max;
  s32 z_min;
  s32 z_max;
} FIELD_CAMERA_RECT;

//-------------------------------------
///	�~�͈�
//=====================================
typedef struct {
  fx32 center_x;
  fx32 center_z;
  fx32 r;         // ���͈͂̔��a
  u16  min_rot;   // min_rot == max_rot �Ȃ� �R�U�O�x�@���͈�
  u16  max_rot;   // min_rot > max_rot �Ȃ�A360�x��ʉ߂�������@���͈�
} FIELD_CAMERA_CIRCLE;

//-------------------------------------
///	�J�������͈͏��
//=====================================
typedef struct {

  // �͈̓^�C�v�w��
  FIELD_CAMERA_AREA_TYPE area_type;

  // �Ǘ����W�^�C�v�w��
  FIELD_CAMERA_AREA_CONT area_cont;

  // �͈͏��
  // 16byte
  union
  {
    // XZ���ʂ̋�`�͈�
    FIELD_CAMERA_RECT rect;

    // XZ���ʂ̉~�͈�
    FIELD_CAMERA_CIRCLE circle;
  } ;
  
} FIELD_CAMERA_AREA;


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
// �J�����̌v�Z���[�h��؂�ւ��܂��B
extern void FIELD_CAMERA_SetMode( FIELD_CAMERA * camera, FIELD_CAMERA_MODE mode );
// �J�������[�h���擾���܂��B
extern FIELD_CAMERA_MODE FIELD_CAMERA_GetMode( const FIELD_CAMERA * camera );
// �J�������[�h��ύX���܂��B
// 1�O�̃^�[�Q�b�g���W�{�J�������W����mode�p�̒l�̌v�Z���s���܂��B
extern void FIELD_CAMERA_ChangeMode( FIELD_CAMERA * camera, FIELD_CAMERA_MODE mode );

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

//----------------------------------------------------------------------------
/**
 *	@brief	�J��������p����
 */
//-----------------------------------------------------------------------------
extern u16 FIELD_CAMERA_GetFovy(const FIELD_CAMERA * camera );
extern void FIELD_CAMERA_SetFovy(FIELD_CAMERA * camera, u16 fovy );

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
extern const VecFx32 *FIELD_CAMERA_GetWatchTarget(FIELD_CAMERA * camera);
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



//-----------------------------------------------------------------------------
/**
 *      �J�������͈�
 */
//-----------------------------------------------------------------------------
extern void FIELD_CAMERA_SetCameraArea( FIELD_CAMERA * camera, const FIELD_CAMERA_AREA* cp_area );
extern void FIELD_CAMERA_ClearCameraArea( FIELD_CAMERA * camera );
extern FIELD_CAMERA_AREA_TYPE FIELD_CAMERA_GetCameraAreaType( const FIELD_CAMERA * camera );
extern FIELD_CAMERA_AREA_CONT FIELD_CAMERA_GetCameraAreaCont( const FIELD_CAMERA * camera );


//-----------------------------------------------------------------------------
/**
 *      ���`�J�����֘A
 */
//-----------------------------------------------------------------------------
extern void FIELD_CAMERA_SetRecvCamParam(FIELD_CAMERA * camera);
extern void FIELD_CAMERA_ClearRecvCamParam(FIELD_CAMERA * camera);
extern void FIELD_CAMERA_SetLinerParam(FIELD_CAMERA * camera, const FLD_CAM_MV_PARAM *param, const u16 inFrame);
extern void FIELD_CAMERA_RecvLinerParam(FIELD_CAMERA * camera, const FLD_CAM_MV_PARAM_CHK *chk, const u16 inFrame);
extern void FIELD_CAMERA_SetLinerParamDirect(FIELD_CAMERA * camera,
                                            const VecFx32 *inCamPos, const VecFx32 *inTrgtPos,
                                            const FLD_CAM_MV_PARAM_CHK *inChk,
                                            const u16 inFrame);
extern void FIELD_CAMERA_SetMvFuncCallBack(FIELD_CAMERA * camera, CAMERA_CALL_BACK func, void *work);
extern void FIELD_CAMERA_ClearMvFuncCallBack(FIELD_CAMERA * camera);
extern BOOL FIELD_CAMERA_CheckMvFunc(FIELD_CAMERA * camera);
extern FLD_CAM_MV_PARAM_CORE *FIELD_CAMERA_GetMoveDstPrmPtr(FIELD_CAMERA * camera);

//-----------------------------------------------------------------------------
/**
 *      �J�����g���[�X�֘A
 */
//-----------------------------------------------------------------------------
extern void FIELD_CAMERA_RestartTrace(FIELD_CAMERA * camera_ptr);
extern void FIELD_CAMERA_StopTraceRequest(FIELD_CAMERA * camera_ptr);
extern BOOL FIELD_CAMERA_CheckTrace(FIELD_CAMERA * camera_ptr);


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

