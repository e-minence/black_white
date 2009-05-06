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
//------------------------------------------------------------------
typedef enum {  
  FIELD_CAMERA_CALC_XZ,
  FIELD_CAMERA_CALC_ANGLE,

  FIELD_CAMERA_CALC_TYPE_MAX,

}FIELD_CAMERA_CALC_TYPE;

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

  FIELD_CAMERA_CALC_TYPE calc_type;

	const VecFx32 *		watch_target;	///<�ǐ����钍���_�ւ̃|�C���^

	VecFx32				target;			///<�����_�p���[�N

	fx32				xzHeight;
	u16					xzLength;
	u16					xzDir;

  u16         angle_h;
  u16         angle_v;
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

  camera->calc_type = FIELD_CAMERA_CALC_XZ;

	VEC_Set( &camera->target, 0, 0, 0 );
	camera->xzHeight = 0;
	camera->xzLength = CAMERA_LENGTH;
	camera->xzDir = 0;

  camera->angle_v = 0;
  camera->angle_h = 0;
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
	//{ 0x78, 0xd8000 },	//DP�ۂ�
	camera->xzLength = 0x0078;
	camera->xzHeight = 0xd8000;
	FIELD_CAMERA_SetFar( camera, (1024 << FX32_SHIFT) );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void initC3Parameter(FIELD_CAMERA * camera)
{
	VecFx32 trans = {0x2f6f36, 0, 0x301402};

	camera->xzLength = 0x0308;
	camera->xzHeight = 0x07c000;
	camera->xzDir = 0;
	FIELD_CAMERA_SetTargetPos(camera, &trans);

	FIELD_CAMERA_SetFar(camera, (512 + 256 + 128) << FX32_SHIFT);
	camera->watch_target = NULL;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void initBridgeParameter(FIELD_CAMERA * camera)
{
	enum { CAMERA_LENGTH = 16 };
	camera->xzLength = 0x0080 + CAMERA_LENGTH;
	camera->xzHeight = 0x0003a000;

	FIELD_CAMERA_SetFar(camera, 4096 << FX32_SHIFT );
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
/**
 * @brief	XZ���ʂł̃J��������
 *
 * �����_�AY���ł̃J�����̍����A�����_����̐����������p�����[�^�Ƃ���
 * �J�����̈ʒu�����肷��
 */
//------------------------------------------------------------------
static void updateXZTargetCamera(FIELD_CAMERA * camera)
{
	enum { CAMERA_TARGET_HEIGHT = 4 };
	VecFx32	pos, target;

	target = pos = camera->target;
	target.y += CAMERA_TARGET_HEIGHT * FX32_ONE;

	pos.x = pos.x + camera->xzLength * FX_SinIdx(camera->xzDir);
	pos.y = pos.y + camera->xzHeight;
	pos.z = pos.z + camera->xzLength * FX_CosIdx(camera->xzDir);

	GFL_G3D_CAMERA_SetTarget( camera->g3Dcamera, &target );
	GFL_G3D_CAMERA_SetPos( camera->g3Dcamera, &pos );
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void updateAngleCamera(FIELD_CAMERA * camera)
{ 
  enum {  PITCH_LIMIT = 0x200 };
  VecFx32 cameraPos;
	fx16 sinYaw = FX_SinIdx(camera->angle_v);
	fx16 cosYaw = FX_CosIdx(camera->angle_v);
	fx16 sinPitch = FX_SinIdx(camera->angle_h);
	fx16 cosPitch = FX_CosIdx(camera->angle_h);

	if(cosPitch < 0){ cosPitch = -cosPitch; }	// �����肵�Ȃ��悤�Ƀ}�C�i�X�l�̓v���X�l�ɕ␳
	if(cosPitch < PITCH_LIMIT){ cosPitch = PITCH_LIMIT; }	// 0�l�ߕӂ͕s���\���ɂȂ邽�ߕ␳

	// �J�����̍��W�v�Z
	VEC_Set( &cameraPos, sinYaw * cosPitch, sinPitch * FX16_ONE, cosYaw * cosPitch);
	VEC_Normalize(&cameraPos, &cameraPos);
	VEC_MultAdd( camera->angle_len, &cameraPos, &camera->target, &cameraPos );
  //cameraPos = cameraPos * length + camera->target

	GFL_G3D_CAMERA_SetTarget( camera->g3Dcamera, &camera->target );
	GFL_G3D_CAMERA_SetPos( camera->g3Dcamera, &cameraPos );
}
//------------------------------------------------------------------
//------------------------------------------------------------------
static void updateCameraCalc(FIELD_CAMERA * camera)
{ 
  GF_ASSERT(camera->calc_type < FIELD_CAMERA_CALC_TYPE_MAX);
  switch (camera->calc_type)
  { 
  case FIELD_CAMERA_CALC_XZ:
    updateXZTargetCamera(camera);
    break;
  case FIELD_CAMERA_CALC_ANGLE:
    updateAngleCamera(camera);
    break;
  }
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
		camera->xzDir -= RT_SPEED/2;
	}
	if( key & PAD_BUTTON_L ){
		camera->xzDir += RT_SPEED/2;
	}
#if 0
	if( key & PAD_BUTTON_B ){
		if( camera->xzLength > 8 ){
			camera->xzLength -= 8;
		}
		//vecMove.y = -MV_SPEED;
	}
	if( key & PAD_BUTTON_A ){
		if( camera->xzLength < 4096 ){
			camera->xzLength += 8;
		}
		//vecMove.y = MV_SPEED;
	}
	if( key & PAD_BUTTON_Y ){
		camera->xzHeight -= MV_SPEED;
	}
	if( key & PAD_BUTTON_X ){
		camera->xzHeight += MV_SPEED;
	}
#endif
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void ControlGridParameter(FIELD_CAMERA * camera, u16 key_cont)
{
	updateTargetBinding(camera);
  updateCameraCalc(camera);
	//updateXZTargetCamera(camera);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void ControlBridgeParameter(FIELD_CAMERA * camera, u16 key_cont)
{
	debugControl(camera, key_cont);
	updateTargetBinding(camera);
	//updateXZTargetCamera(camera);
  updateCameraCalc(camera);
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void ControlC3Parameter(FIELD_CAMERA * camera, u16 key_cont)
{
	updateTargetBinding(camera);
	//updateXZTargetCamera(camera);
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
 * @brief	XZ���ʏ�̕������擾����
 * @param	camera		FIELD�J��������|�C���^
 */
//------------------------------------------------------------------
u16 FIELD_CAMERA_GetDirectionOnXZ( const FIELD_CAMERA * camera )
{
#if 0
	u16 value;
	VecFx32 vec, camPos, target;
	
	GFL_G3D_CAMERA_GetPos( camera->g3Dcamera, &camPos );
	GFL_G3D_CAMERA_GetTarget( camera->g3Dcamera, &target );

	VEC_Subtract( &target, &camPos, &vec );
	value = FX_Atan2Idx( -vec.z, vec.x ) - 0x4000;
	if(value != camera->xzDir) {
		TAMADA_Printf("calc value = %04x, hold value = %04x\n",value, camera->xzDir);
	}
	return value;
#endif
	return camera->xzDir;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_CAMERA_SetDirectionOnXZ(FIELD_CAMERA * camera, u16 dir)
{
	camera->xzDir = dir;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_CAMERA_SetLengthOnXZ( FIELD_CAMERA *camera, u16 leng )
{
	camera->xzLength = leng;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
u16	FIELD_CAMERA_GetLengthOnXZ( const FIELD_CAMERA *camera )
{
	return camera->xzLength;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
void	FIELD_CAMERA_SetHeightOnXZ( FIELD_CAMERA *camera, fx32 height )
{
	camera->xzHeight = height;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
fx32	FIELD_CAMERA_GetHeightOnXZ( const FIELD_CAMERA *camera )
{
	 return camera->xzHeight;
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
      &camera->angle_v, &camera->angle_h, &camera->angle_len);

  camera->calc_type = FIELD_CAMERA_CALC_ANGLE;
}

void FIELD_CAMERA_DEBUG_ReleaseSubScreen(FIELD_CAMERA * camera)
{ 
  camera->calc_type = FIELD_CAMERA_CALC_XZ;
}
#endif  //PM_DEBUG

