//============================================================================================
/**
 * @file	field_camera.c
 * @brief	�t�B�[���h�J��������
 * @author	tamada GAME FREAK inc.
 */
//============================================================================================
#include <gflib.h>
#include "field_g3d_mapper.h"
#include "field_common.h"
#include "field_camera.h"
	
//#include "arc_def.h"
//============================================================================================
//============================================================================================

//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
struct _FIELD_CAMERA {
	HEAPID				heapID;			///<�g�p����q�[�v�w��ID
	FIELD_MAIN_WORK * fieldWork;		///<�t�B�[���h�}�b�v�ւ̃|�C���^
	GFL_G3D_CAMERA * g3Dcamera;			///<�J�����\���̂ւ̃|�C���^

	FIELD_CAMERA_TYPE	type;			///<�J�����̃^�C�v�w��

	const VecFx32 *		watch_target;	///<�ǐ����钍���_�ւ̃|�C���^

	VecFx32				target;			        ///<�����_�p���[�N
	VecFx32				target_offset;			///<�����_�p�␳���W

  u16         angle_pitch;
  u16         angle_yaw;
  fx32        angle_len;
};


#if 0
//------------------------------------------------------------------
/**
 * @brief	�J�����p�����[�^
 */
//------------------------------------------------------------------
typedef struct {
	fx32			Distance;
	VecFx32			Angle;
	u16				View;
	u16				PerspWay;
	fx32			Near;
	fx32			Far;
	VecFx32			Shift;
}FIELD_CAMERA_PARAM;

static const FIELD_CAMERA_PARAM FieldCameraParam[] = {
	{
	},
};
#endif


//------------------------------------------------------------------
/**
 * @brief	��ʂ��Ƃ̃J��������֐��e�[�u����`
 */
//------------------------------------------------------------------
typedef struct {
	void (*init_func)(FIELD_CAMERA * camera);
	void (*control)(FIELD_CAMERA * camera, u16 key_cont);
}CAMERA_FUNC_TABLE;

//------------------------------------------------------------------
//------------------------------------------------------------------
static void initGridParameter(FIELD_CAMERA * camera);
static void initBridgeParameter(FIELD_CAMERA * camera);
static void initC3Parameter(FIELD_CAMERA * camera);
static void ControlGridParameter(FIELD_CAMERA * camera, u16 key_cont);
static void ControlBridgeParameter(FIELD_CAMERA * camera, u16 key_cont);
static void ControlC3Parameter(FIELD_CAMERA * camera, u16 key_cont);


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	��ʂ��Ƃ̃J��������֐��e�[�u����`
 */
//------------------------------------------------------------------
static const CAMERA_FUNC_TABLE CameraFuncTable[] = {
	{
		initGridParameter,
		ControlGridParameter,
	},
	{
		initBridgeParameter,
		ControlBridgeParameter,
	},
	{
		initC3Parameter,
		ControlC3Parameter,
	}
};


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	������
 */
//------------------------------------------------------------------
FIELD_CAMERA* FIELD_CAMERA_Create(
		FIELD_MAIN_WORK * fieldWork,
		FIELD_CAMERA_TYPE type,
		GFL_G3D_CAMERA * cam,
		const VecFx32 * target,
		HEAPID heapID)
{
	FIELD_CAMERA* camera = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_CAMERA) );

	enum { CAMERA_LENGTH = 16 };

	GF_ASSERT(cam != NULL);
	GF_ASSERT(type < FIELD_CAMERA_TYPE_MAX);
	TAMADA_Printf("FIELD CAMERA TYPE = %d\n", type);
	camera->fieldWork = fieldWork;
	camera->type = type;
	camera->g3Dcamera = cam;
	camera->watch_target = target;
	camera->heapID = heapID;

	VEC_Set( &camera->target, 0, 0, 0 );

  camera->angle_yaw = 0;
  camera->angle_pitch = 0;
  camera->angle_len = 0x0078;

	CameraFuncTable[camera->type].init_func(camera);

	return camera;
}
//------------------------------------------------------------------
/**
 * @brief	�I��
 */
//------------------------------------------------------------------
void	FIELD_CAMERA_Delete( FIELD_CAMERA* camera )
{
	GFL_HEAP_FreeMemory( camera );
}

//------------------------------------------------------------------
/**
 * @brief	���C��
 */
//------------------------------------------------------------------
void FIELD_CAMERA_Main( FIELD_CAMERA* camera, u16 key_cont)
{
	CameraFuncTable[camera->type].control(camera, key_cont);
}


//============================================================================================
//
//
//		�J�����������p�֐�
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
static void initGridParameter(FIELD_CAMERA * camera)
{
	enum { 
		CAMERA_LENGTH = 0x78000,
		CAMERA_HEIGHT = 0xd8000,
	};
	//{ 0x78, 0xd8000 },	//DP�ۂ�
	// ����
	camera->angle_len = FX_Mul( CAMERA_LENGTH, CAMERA_LENGTH ) + FX_Mul( CAMERA_HEIGHT, CAMERA_HEIGHT );
	camera->angle_len = FX_Sqrt( camera->angle_len );

	// X���̊p�x
	{
		fx32 cos;
		VecFx32 vec0 = { 0,0,FX32_ONE };
		VecFx32 vec1 = { 0 };

		vec1.z = CAMERA_LENGTH;
		vec1.y = CAMERA_HEIGHT;

		VEC_Normalize( &vec0, &vec0 );
		VEC_Normalize( &vec1, &vec1 );

		cos = VEC_DotProduct( &vec0, &vec1 );
		camera->angle_pitch = FX_AcosIdx( cos );
	}
	
	FIELD_CAMERA_SetFar( camera, (1024 << FX32_SHIFT) );
	
  // �^�[�Q�b�g�␳�l
  VEC_Set( &camera->target_offset, 0, 0x4000, 0 );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void initC3Parameter(FIELD_CAMERA * camera)
{
	enum { 
		CAMERA_LENGTH = 0x308,
		CAMERA_HEIGHT = 0x07c,

		CAMERA_ANGLE_LENGTH = 0x315000,
	};
	VecFx32 trans = {0x2f6f36, 0x4000, 0x301402};

	
	// ����
	camera->angle_len = CAMERA_ANGLE_LENGTH;

	// X���̊p�x
	{
		fx32 cos;
		VecFx32 vec0 = { 0,0,FX32_ONE };
		VecFx32 vec1 = { 0 };

		vec1.z = CAMERA_LENGTH;
		vec1.y = CAMERA_HEIGHT;

		VEC_Normalize( &vec0, &vec0 );
		VEC_Normalize( &vec1, &vec1 );

		cos = VEC_DotProduct( &vec0, &vec1 );
		camera->angle_pitch = FX_AcosIdx( cos );
	}
	
	FIELD_CAMERA_SetTargetPos(camera, &trans);

	FIELD_CAMERA_SetFar(camera, 0x480000);
	FIELD_CAMERA_SetNear(camera, 0x64000);
	camera->watch_target = NULL;

  // �^�[�Q�b�g�␳�l
  VEC_Set( &camera->target_offset, 0, 0, 0 );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void initBridgeParameter(FIELD_CAMERA * camera)
{
	enum { 
		CAMERA_LENGTH = 0x90000,
		CAMERA_HEIGHT = 0x3a000,
	};


	// ����
	camera->angle_len = FX_Mul( CAMERA_LENGTH, CAMERA_LENGTH ) + FX_Mul( CAMERA_HEIGHT, CAMERA_HEIGHT );
	camera->angle_len = FX_Sqrt( camera->angle_len );

	// X���̊p�x
	{
		fx32 cos;
		VecFx32 vec0 = { 0,0,FX32_ONE };
		VecFx32 vec1 = { 0 };

		vec1.z = CAMERA_LENGTH;
		vec1.y = CAMERA_HEIGHT;

		VEC_Normalize( &vec0, &vec0 );
		VEC_Normalize( &vec1, &vec1 );

		cos = VEC_DotProduct( &vec0, &vec1 );
		camera->angle_pitch = FX_AcosIdx( cos );
	}

	FIELD_CAMERA_SetFar(camera, 4096 << FX32_SHIFT );

  // �^�[�Q�b�g�␳�l
  VEC_Set( &camera->target_offset, 0, 0x4000, 0 );
}

//============================================================================================
//
//
//		�J������ԃA�b�v�f�[�g�p�֐�
//
//
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	�����_��ێ�������W���[�N���Ď��A�ǐ�����
 */
//------------------------------------------------------------------
static void updateTargetBinding(FIELD_CAMERA * camera)
{
	if (camera->watch_target) {
		(camera->target) = *(camera->watch_target);
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void updateAngleCamera(FIELD_CAMERA * camera)
{ 
  enum {  PITCH_LIMIT = 0x200 };
  VecFx32 cameraPos;
	VecFx32 cameraTarget = {0};
	fx16 sinYaw = FX_SinIdx(camera->angle_yaw);
	fx16 cosYaw = FX_CosIdx(camera->angle_yaw);
	fx16 sinPitch = FX_SinIdx(camera->angle_pitch);
	fx16 cosPitch = FX_CosIdx(camera->angle_pitch);

	if(cosPitch < 0){ cosPitch = -cosPitch; }	// �����肵�Ȃ��悤�Ƀ}�C�i�X�l�̓v���X�l�ɕ␳
	if(cosPitch < PITCH_LIMIT){ cosPitch = PITCH_LIMIT; }	// 0�l�ߕӂ͕s���\���ɂȂ邽�ߕ␳

	// �J�����̍��W�v�Z
	VEC_Set( &cameraPos, FX_Mul(sinYaw, cosPitch), sinPitch, FX_Mul(cosYaw, cosPitch) );
	VEC_Normalize(&cameraPos, &cameraPos);
	VEC_MultAdd( camera->angle_len, &cameraPos, &camera->target, &cameraPos );
  //cameraPos = cameraPos * length + camera->target
	
	// �J�����^�[�Q�b�g�␳
  VEC_Add( &camera->target, &camera->target_offset, &cameraTarget );

	GFL_G3D_CAMERA_SetTarget( camera->g3Dcamera, &cameraTarget );
	GFL_G3D_CAMERA_SetPos( camera->g3Dcamera, &cameraPos );
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void updateCameraCalc(FIELD_CAMERA * camera)
{ 
   updateAngleCamera(camera);
}
//------------------------------------------------------------------
/**
 * @brief	�f�o�b�O�̂��߂̃L�[����
 */
//------------------------------------------------------------------
static void debugControl( FIELD_CAMERA * camera, int key)
{
	VecFx32	vecMove = { 0, 0, 0 };
	VecFx32	vecUD = { 0, 0, 0 };
	BOOL	mvFlag = FALSE;
	if( key & PAD_BUTTON_R ){
		camera->angle_yaw -= RT_SPEED/2;
	}
	if( key & PAD_BUTTON_L ){
		camera->angle_yaw += RT_SPEED/2;
	}
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void ControlGridParameter(FIELD_CAMERA * camera, u16 key_cont)
{
	updateTargetBinding(camera);
  updateCameraCalc(camera);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void ControlBridgeParameter(FIELD_CAMERA * camera, u16 key_cont)
{
	debugControl(camera, key_cont);
	updateTargetBinding(camera);
  updateCameraCalc(camera);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void ControlC3Parameter(FIELD_CAMERA * camera, u16 key_cont)
{
	updateTargetBinding(camera);
  updateCameraCalc(camera);
}


//============================================================================================
//
//
//			�J��������p�O�����J�֐�
//
//
//============================================================================================
//------------------------------------------------------------------
//------------------------------------------------------------------
const GFL_G3D_CAMERA * FIELD_CAMERA_GetCameraPtr(const FIELD_CAMERA * camera)
{
	return camera->g3Dcamera;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_CAMERA_BindTarget(FIELD_CAMERA * camera, const VecFx32 * watch_target)
{
	camera->watch_target = watch_target;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_CAMERA_FreeTarget(FIELD_CAMERA * camera)
{
	camera->watch_target = NULL;
}

//------------------------------------------------------------------
/**
 * @brief	near�N���b�v�ʂ܂ł̋�����ݒ�
 * @param	camera		FIELD�J��������|�C���^
 * @param	near		�ݒ肷�鋗���ifx32)
 */
//------------------------------------------------------------------
void FIELD_CAMERA_SetNear(FIELD_CAMERA * camera, fx32 near)
{
	GFL_G3D_CAMERA_SetNear(camera->g3Dcamera, &near);
}

//------------------------------------------------------------------
/**
 * @brief	near�N���b�v�ʂ܂ł̋������擾
 * @param	camera		FIELD�J��������|�C���^
 * @return	fx32		�擾��������
 */
//------------------------------------------------------------------
fx32 FIELD_CAMERA_GetNear(const FIELD_CAMERA * camera)
{
	fx32 near;
	GFL_G3D_CAMERA_GetNear( camera->g3Dcamera, &near);
	return near;
}

//------------------------------------------------------------------
/**
 * @brief	far�N���b�v�ʂ܂ł̋�����ݒ�
 * @param	camera		FIELD�J��������|�C���^
 * @param	far		�ݒ肷�鋗���ifx32)
 */
//------------------------------------------------------------------
void FIELD_CAMERA_SetFar(FIELD_CAMERA * camera, fx32 far)
{
	GFL_G3D_CAMERA_SetFar( camera->g3Dcamera, &far );
}

//------------------------------------------------------------------
/**
 * @brief	far�N���b�v�ʂ܂ł̋������擾
 * @param	camera		FIELD�J��������|�C���^
 * @return	fx32		�擾��������
 */
//------------------------------------------------------------------
fx32 FIELD_CAMERA_GetFar(const FIELD_CAMERA * camera)
{
	fx32 far;
	GFL_G3D_CAMERA_GetFar( camera->g3Dcamera, &far);
	return far;
}

//------------------------------------------------------------------
/**
 * @brief	�J�����ʒu�̎擾
 * @param	camera		FIELD�J��������|�C���^
 * @param	pos			�J�����ʒu��n��VecFx32�ւ̃|�C���^
 */
//------------------------------------------------------------------
void	FIELD_CAMERA_SetTargetPos( FIELD_CAMERA* camera, const VecFx32* target )
{
	camera->target = *target;
}
//------------------------------------------------------------------
/**
 * @brief	�J�����ʒu�̎擾
 * @param	camera		FIELD�J��������|�C���^
 * @param	pos			�J�����ʒu���󂯎��VecFx32�ւ̃|�C���^
 */
//------------------------------------------------------------------
void	FIELD_CAMERA_GetTargetPos( const FIELD_CAMERA* camera, VecFx32* pos )
{
	*pos = camera->target;
}

//------------------------------------------------------------------
/**
 * @brief	�J���������_�@�␳���W�̎擾
 * @param	camera		    FIELD�J��������|�C���^
 * @param	target_offset	�J���������_��␳������W�󂯎��VecFx32�ւ̃|�C���^
 */
//------------------------------------------------------------------
void	FIELD_CAMERA_GetTargetOffset( const FIELD_CAMERA* camera, VecFx32* target_offset )
{
  *target_offset = camera->target_offset;
}


//------------------------------------------------------------------
/**
 * @brief	�J���������_�@�␳���W�̎擾
 * @param	camera		        FIELD�J��������|�C���^
 * @param	target_offset			�J���������_��␳������W�n��VecFx32�ւ̃|�C���^
 */
//------------------------------------------------------------------
void	FIELD_CAMERA_SetTargetOffset( FIELD_CAMERA* camera, const VecFx32* target_offset )
{
  camera->target_offset = *target_offset;
}



//----------------------------------------------------------------------------
/**
 *	@brief	�J����angle�@����������]
 */
//-----------------------------------------------------------------------------
u16 FIELD_CAMERA_GetAnglePitch(const FIELD_CAMERA * camera )
{
	return camera->angle_pitch;
}
void FIELD_CAMERA_SetAnglePitch(FIELD_CAMERA * camera, u16 angle )
{
	camera->angle_pitch = angle;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J����angle�@����������]
 */
//-----------------------------------------------------------------------------
u16 FIELD_CAMERA_GetAngleYaw(const FIELD_CAMERA * camera )
{
	return camera->angle_yaw;
}
void FIELD_CAMERA_SetAngleYaw(FIELD_CAMERA * camera, u16 angle )
{
	camera->angle_yaw = angle;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J����angle�@�^�[�Q�b�g����̋�������
 */
//-----------------------------------------------------------------------------
fx32 FIELD_CAMERA_GetAngleLen(const FIELD_CAMERA * camera )
{
	return camera->angle_len;	
}
void FIELD_CAMERA_SetAngleLen(FIELD_CAMERA * camera, fx32 length )
{
	camera->angle_len = length;
}



#ifdef  PM_DEBUG
#include "test/camera_adjust_view.h"
//------------------------------------------------------------------
//  �f�o�b�O�p�F����ʑ���Ƃ̃o�C���h
//------------------------------------------------------------------
void FIELD_CAMERA_DEBUG_BindSubScreen(FIELD_CAMERA * camera, void * param)
{ 
  GFL_CAMADJUST * gflCamAdjust = param;
  GFL_CAMADJUST_SetCameraParam(gflCamAdjust,
      &camera->angle_yaw, &camera->angle_pitch, &camera->angle_len);

}

void FIELD_CAMERA_DEBUG_ReleaseSubScreen(FIELD_CAMERA * camera)
{ 
}
#endif  //PM_DEBUG

