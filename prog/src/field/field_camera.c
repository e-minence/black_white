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
	
#include "arc_def.h"
#include "arc/fieldmap/field_camera_data.naix"
//============================================================================================
//============================================================================================

#define FIELD_CAMERA_DELAY	(6)
#define FIELD_CAMERA_TRACE_BUFF	(FIELD_CAMERA_DELAY+1)



/*---------------------------------------------------------------------------*
	�J�����g���[�X�\����
 *---------------------------------------------------------------------------*/
typedef struct 
{
	int bufsize;
	int CamPoint;
	int TargetPoint;
	int Delay;			//�x������
	BOOL UpdateFlg;		//�X�V�J�n�t���O
	BOOL ValidX;		//X�x����L���ɂ��邩�̃t���O
	BOOL ValidY;		//Y�x����L���ɂ��邩�̃t���O
	BOOL ValidZ;		//Z�x����L���ɂ��邩�̃t���O

  VecFx32 * targetBuffer;
  VecFx32 * camPosBuffer;

}CAMERA_TRACE;

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�J�����^�C�v
 */
//------------------------------------------------------------------
typedef enum {
	FIELD_CAMERA_TYPE_GRID,
	FIELD_CAMERA_TYPE_H01,
	FIELD_CAMERA_TYPE_C03,

  FIELD_CAMERA_TYPE_POKECEN,
  FIELD_CAMERA_TYPE_H01P01,

  FIELD_CAMERA_TYPE_PLP01,

	FIELD_CAMERA_TYPE_MAX,
}FIELD_CAMERA_TYPE;

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
	FIELD_CAMERA_MODE mode;			///<�J�������[�h

  const VecFx32 * default_target;
	const VecFx32 *		watch_target;	///<�ǐ����钍���_�ւ̃|�C���^
  const VecFx32 *   watch_camera; ///<�ǐ�����J�����ʒu�ւ̃|�C���^

  VecFx32       camPos;             ///<�J�����ʒu�p���[�N

	VecFx32				target;			        ///<�����_�p���[�N
	VecFx32				target_offset;			///<�����_�p�␳���W
  VecFx32       target_before;

  u16         angle_pitch;
  u16         angle_yaw;
  fx32        angle_len;

  u16         fovy;
  u16         pad;
#ifdef PM_DEBUG
  u32 debug_subscreen_type;

  // �^�[�Q�b�g����p
  u16         debug_target_pitch;
  u16         debug_target_yaw;
  fx32        debug_target_len;
  VecFx32     debug_target;

  fx32        debug_far;
#endif

  CAMERA_TRACE * Trace;
};


//============================================================================================
//============================================================================================
#include "../resource/fldmapdata/camera_data/fieldcameraformat.h"



static void createTraceData(const int inHistNum, const int inDelay,
						const int inTraceMask, const int inHeapID,
						FIELD_CAMERA * ioCamera);
static void deleteTraceData(FIELD_CAMERA * camera_ptr);
static void updateTraceData(CAMERA_TRACE * trace,
    const VecFx32 * inTarget, const VecFx32 * inCamPos, VecFx32 * outTarget, VecFx32 * outCamPos);
static void traceUpdate(FIELD_CAMERA * camera);
//------------------------------------------------------------------
//------------------------------------------------------------------
static void loadCameraParameters(FIELD_CAMERA * camera);

static void ControlParameter( FIELD_CAMERA * camera, u16 key_cont );
static void ControlParameter_CalcCamera( FIELD_CAMERA * camera, u16 key_cont );
static void ControlParameter_CalcTarget( FIELD_CAMERA * camera, u16 key_cont );
static void ControlParameter_Direct( FIELD_CAMERA * camera, u16 key_cont );


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	������
 */
//------------------------------------------------------------------
FIELD_CAMERA* FIELD_CAMERA_Create(
		FIELD_MAIN_WORK * fieldWork,
		u8 type,
		FIELD_CAMERA_MODE mode,
		GFL_G3D_CAMERA * cam,
		const VecFx32 * target,
		HEAPID heapID)
{
	FIELD_CAMERA* camera = GFL_HEAP_AllocClearMemory( heapID, sizeof(FIELD_CAMERA) );

	enum { CAMERA_LENGTH = 16 };

	GF_ASSERT(cam != NULL);
	TAMADA_Printf("FIELD CAMERA TYPE = %d\n", type);
	camera->fieldWork = fieldWork;
	camera->type = type;
	camera->mode = mode;
	camera->g3Dcamera = cam;
  camera->default_target = target;
	camera->watch_target = target;
  camera->watch_camera = NULL;
	camera->heapID = heapID;

  VEC_Set( &camera->camPos, 0, 0, 0 );
	VEC_Set( &camera->target, 0, 0, 0 );
  VEC_Set( &camera->target_before, 0, 0, 0 );

  camera->angle_yaw = 0;
  camera->angle_pitch = 0;
  camera->angle_len = 0x0078;

  // �p�[�X���J�����̏�񂩂�擾
  if( GFL_G3D_CAMERA_GetProjectionType( cam ) == GFL_G3D_PRJPERS ){
    fx32 fovyCos;
    GFL_G3D_CAMERA_GetfovyCos( cam, &fovyCos );
    camera->fovy = FX_AcosIdx( fovyCos );
  }

  loadCameraParameters(camera);

	createTraceData(	FIELD_CAMERA_TRACE_BUFF, FIELD_CAMERA_DELAY,
							CAM_TRACE_MASK_Y, heapID, camera);

	return camera;
}
//------------------------------------------------------------------
/**
 * @brief	�I��
 */
//------------------------------------------------------------------
void	FIELD_CAMERA_Delete( FIELD_CAMERA* camera )
{
  deleteTraceData( camera );
	GFL_HEAP_FreeMemory( camera );
}

//------------------------------------------------------------------
/**
 * @brief	���C��
 */
//------------------------------------------------------------------
void FIELD_CAMERA_Main( FIELD_CAMERA* camera, u16 key_cont)
{
  ControlParameter(camera, key_cont);
}


//----------------------------------------------------------------------------
/**
 *	@brief	�J�������[�h�@�ݒ�
 *	
 *	@param	camera		�J����
 *	@param	mode			���[�h
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetMode( FIELD_CAMERA * camera, FIELD_CAMERA_MODE mode )
{
	GF_ASSERT( camera );
	GF_ASSERT( mode < FIELD_CAMERA_MODE_MAX );
	camera->mode = mode;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�������[�h�@�擾
 *
 *	@param	camera	�J����
 *
 *	@return	�J�������[�h
 */
//-----------------------------------------------------------------------------
FIELD_CAMERA_MODE FIELD_CAMERA_GetMode( const FIELD_CAMERA * camera )
{
	GF_ASSERT( camera );
	return camera->mode;
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
static void loadCameraParameters(FIELD_CAMERA * camera)
{
  enum {
    FILE_ID = NARC_field_camera_data_field_camera_bin,
    ARC_ID  = ARCID_FIELD_CAMERA,
  };
  FLD_CAMERA_PARAM param;
  ARCHANDLE * handle = GFL_ARC_OpenDataHandle(ARC_ID, camera->heapID);
  u16 size = GFL_ARC_GetDataSizeByHandle(handle, FILE_ID);
  if ( camera->type * sizeof(FLD_CAMERA_PARAM) >= size )
  {
    OS_TPrintf("�J�����^�C�v�i%d�j���w��ł��܂���\n", camera->type);
    GF_ASSERT(0);
    camera->type = 0;
  }
  GFL_ARC_LoadDataOfsByHandle(handle, FILE_ID,
      camera->type * sizeof(FLD_CAMERA_PARAM), sizeof(FLD_CAMERA_PARAM), &param);
#if 0
  TAMADA_Printf("FIELD CAMERA INIT INFO\n");
  TAMADA_Printf("FIELD CAMERA TYPE =%d\n",camera->type);
  TAMADA_Printf("dist = %08x\n", param.Distance);
  TAMADA_Printf("angle = %08x,%08x,%08x\n", param.Angle.x, param.Angle.y, param.Angle.z );
  TAMADA_Printf("viewtype = %d, persp = %08x\n", param.View, param.PerspWay);
  TAMADA_Printf("near=%8x, far =%08x\n", param.Near, param.Far);
  TAMADA_Printf("shift = %08x,%08x,%08x\n", param.Shift.x, param.Shift.y, param.Shift.z );
  TAMADA_Printf("now near = %d\n", FX_Whole( FIELD_CAMERA_GetNear(camera) ) );
  TAMADA_Printf("now far = %d\n", FX_Whole( FIELD_CAMERA_GetFar(camera) ) );
#endif

  camera->angle_len = param.Distance * FX32_ONE;
  camera->angle_pitch = param.Angle.x;
  camera->angle_yaw = param.Angle.y;
  //camera->viewtype = param.ViewType;
  camera->fovy = param.PerspWay;
  camera->target_offset = param.Shift;
  FIELD_CAMERA_SetFar( camera, param.Far );
  FIELD_CAMERA_SetNear(camera, param.Near );
  switch (param.depthType)
  {
  case DEPTH_TYPE_ZBUF:
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );
    break;
  case DEPTH_TYPE_WBUF:
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_W );
    break;
  }
  if( GFL_G3D_CAMERA_GetProjectionType(camera->g3Dcamera) == GFL_G3D_PRJPERS ){
    GFL_G3D_CAMERA_SetfovySin( camera->g3Dcamera, FX_SinIdx( camera->fovy ) );
    GFL_G3D_CAMERA_SetfovyCos( camera->g3Dcamera, FX_CosIdx( camera->fovy ) );
  }

	GFL_ARC_CloseDataHandle(handle);
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
static BOOL updateCamPosBinding(FIELD_CAMERA * camera)
{
  if (camera->watch_camera)
  {
    (camera->camPos) = *(camera->watch_camera);
    return TRUE;
  }
  return FALSE;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void calcAnglePos( const VecFx32* cp_target, VecFx32* p_pos, u16 yaw, u16 pitch, fx32 len )
{
  enum {  PITCH_LIMIT = 0x200 };
  VecFx32 cameraPos;
	fx16 sinYaw = FX_SinIdx(yaw);
	fx16 cosYaw = FX_CosIdx(yaw);
	fx16 sinPitch = FX_SinIdx(pitch);
	fx16 cosPitch = FX_CosIdx(pitch);

	if(cosPitch < 0){ cosPitch = -cosPitch; }	// �����肵�Ȃ��悤�Ƀ}�C�i�X�l�̓v���X�l�ɕ␳
	if(cosPitch < PITCH_LIMIT){ cosPitch = PITCH_LIMIT; }	// 0�l�ߕӂ͕s���\���ɂȂ邽�ߕ␳

	// �J�����̍��W�v�Z
	VEC_Set( &cameraPos, FX_Mul(sinYaw, cosPitch), sinPitch, FX_Mul(cosYaw, cosPitch) );
	VEC_Normalize(&cameraPos, &cameraPos);
	VEC_MultAdd( len, &cameraPos, cp_target, &cameraPos );
  //cameraPos = cameraPos * length + camera->target
  *p_pos = cameraPos;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void updateAngleCameraPos(FIELD_CAMERA * camera)
{ 
  // �J�����|�W�V�����v�Z
	calcAnglePos( &camera->target, &camera->camPos,
			camera->angle_yaw, camera->angle_pitch, camera->angle_len );
}
static void updateAngleTargetPos(FIELD_CAMERA * camera)
{ 
  // �^�[�Q�b�g�|�W�V�����v�Z
	calcAnglePos( &camera->camPos, &camera->target,
			camera->angle_yaw, camera->angle_pitch, camera->angle_len );
}
	
//------------------------------------------------------------------
//------------------------------------------------------------------
static void updateG3Dcamera(FIELD_CAMERA * camera)
{
	VecFx32 cameraTarget;

#ifndef PM_DEBUG
	// �J�����^�[�Q�b�g�␳
  VEC_Add( &camera->target, &camera->target_offset, &cameraTarget );

	GFL_G3D_CAMERA_SetTarget( camera->g3Dcamera, &cameraTarget );
	GFL_G3D_CAMERA_SetPos( camera->g3Dcamera, &camera->camPos );

  traceUpdate( camera );
#else

  if( camera->debug_subscreen_type != FIELD_CAMERA_DEBUG_BIND_TARGET_POS )
  {
    // �ʏ�̃J�����^�[�Q�b�g
    // �J�����^�[�Q�b�g�␳
    VEC_Add( &camera->target, &camera->target_offset, &cameraTarget );

    GFL_G3D_CAMERA_SetTarget( camera->g3Dcamera, &cameraTarget );
    GFL_G3D_CAMERA_SetPos( camera->g3Dcamera, &camera->camPos );

		if( camera->debug_subscreen_type == FIELD_CAMERA_DEBUG_BIND_NONE )
		{
//			traceUpdate( camera );
		}
  }
  else
  {
    // �^�[�Q�b�g��ύX
    calcAnglePos( &camera->camPos, &camera->debug_target, camera->debug_target_yaw,
        camera->debug_target_pitch, camera->debug_target_len );

    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R )
    {
      VecFx32 target_offset;
      
      OS_TPrintf( "DEBUG Camera Target x[0x%x] y[0x%x] z[0x%x]\n", camera->debug_target.x, camera->debug_target.y, camera->debug_target.z  );
      OS_TPrintf( "DEBUG Camera Pos x[0x%x] y[0x%x] z[0x%x]\n", camera->camPos.x, camera->camPos.y, camera->camPos.z  );

      VEC_Subtract( &camera->debug_target, &camera->target, &target_offset );
      OS_TPrintf( "DEBUG Scroll Camera Target Offset x[0x%x] y[0x%x] z[0x%x]\n", target_offset.x, target_offset.y, target_offset.z  );
    }

    GFL_G3D_CAMERA_SetTarget( camera->g3Dcamera, &camera->debug_target );
    GFL_G3D_CAMERA_SetPos( camera->g3Dcamera, &camera->camPos );
  }

  // Far,Fovy���W�̐ݒ�
  if( camera->debug_subscreen_type != FIELD_CAMERA_DEBUG_BIND_NONE ){
    GFL_G3D_CAMERA_SetFar( camera->g3Dcamera, &camera->debug_far );
    if( GFL_G3D_CAMERA_GetProjectionType(camera->g3Dcamera) == GFL_G3D_PRJPERS ){
      GFL_G3D_CAMERA_SetfovySin( camera->g3Dcamera, FX_SinIdx( camera->fovy ) );
      GFL_G3D_CAMERA_SetfovyCos( camera->g3Dcamera, FX_CosIdx( camera->fovy ) );
    }
  }

#endif


}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void ControlParameter(FIELD_CAMERA * camera, u16 key_cont)
{
	static void (*const cp_Control[FIELD_CAMERA_MODE_MAX])( FIELD_CAMERA * camera, u16 key_cont ) = 
	{
		ControlParameter_CalcCamera,
		ControlParameter_CalcTarget,
		ControlParameter_Direct,
	};
	GF_ASSERT( camera->mode < FIELD_CAMERA_MODE_MAX );
	cp_Control[ camera->mode ]( camera, key_cont );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�����Ǘ�	�J�������W�v�Z���[�h
 */
//-----------------------------------------------------------------------------
static void ControlParameter_CalcCamera( FIELD_CAMERA * camera, u16 key_cont )
{
	updateTargetBinding(camera);
  updateAngleCameraPos(camera);
  updateG3Dcamera(camera);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�����Ǘ�	�^�[�Q�b�g���W�v�Z���[�h
 */
//-----------------------------------------------------------------------------
static void ControlParameter_CalcTarget( FIELD_CAMERA * camera, u16 key_cont )
{
	updateCamPosBinding( camera );
  updateAngleTargetPos(camera);
  updateG3Dcamera(camera);
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�����Ǘ�	�_�C���N�g���W���[�h
 */
//-----------------------------------------------------------------------------
static void ControlParameter_Direct( FIELD_CAMERA * camera, u16 key_cont )
{
	updateTargetBinding(camera);
	updateCamPosBinding( camera );
  updateG3Dcamera(camera);
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
	GF_ASSERT( (camera->mode == FIELD_CAMERA_MODE_CALC_CAMERA_POS)||(camera->mode == FIELD_CAMERA_MODE_CALC_TARGET_POS) );
	camera->watch_target = watch_target;
  camera->target_before = *watch_target;
  camera->target = *watch_target;
}
//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_CAMERA_FreeTarget(FIELD_CAMERA * camera)
{
	camera->watch_target = NULL;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
void FIELD_CAMERA_BindDefaultTarget(FIELD_CAMERA * camera)
{
  camera->watch_target = camera->default_target;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�t�H���g�^�[�Q�b�g�����邩�`�F�b�N
 *
 *	@retval	TRUE	����
 *	@retval	FALSE	�Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_CAMERA_IsBindDefaultTarget( const FIELD_CAMERA * camera )
{
	if( camera->default_target )
	{
		return TRUE;
	}
	return FALSE;
}


//------------------------------------------------------------------
//------------------------------------------------------------------
/*
void FIELD_CAMERA_BindNoCamera(FIELD_CAMERA * camera, BOOL flag)
{
	if(flag)
	{
		camera->watch_camera = &camera->camPos;
	}
	else
	{
		camera->watch_camera = NULL;
	}
}
//*/
//----------------------------------------------------------------------------
/**
 *	@brief	�J�������W�̃o�C���h
 *
 *	@param	camera				�J����
 *	@param	watch_camera	�J�������Â����W
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_BindCamera(FIELD_CAMERA * camera, const VecFx32 * watch_camera)
{
	GF_ASSERT( camera );
	GF_ASSERT( (camera->mode == FIELD_CAMERA_MODE_CALC_TARGET_POS) || (camera->mode == FIELD_CAMERA_MODE_DIRECT_POS) );
	camera->watch_camera = watch_camera;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J�������W�̃o�C���h�@�I��
 *
 *	@param	camera	�J����
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_FreeCamera(FIELD_CAMERA * camera)
{
	GF_ASSERT( camera );
	camera->watch_camera = NULL;
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
	GF_ASSERT( (camera->mode == FIELD_CAMERA_MODE_DIRECT_POS) || (camera->mode == FIELD_CAMERA_MODE_CALC_CAMERA_POS) );
	camera->target = *target;
  camera->target_before = * target;
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

//------------------------------------------------------------------
/**
 * @brief �J�����ʒu�̎擾
 * @param	camera		        FIELD�J��������|�C���^
 * @param camPos            �擾�ʒu���i�[����VecFx32�ւ̃|�C���^
 */
//------------------------------------------------------------------
void FIELD_CAMERA_GetCameraPos( const FIELD_CAMERA * camera, VecFx32 * camPos)
{
  *camPos = camera->camPos;
}
//------------------------------------------------------------------
/**
 * @brief �J�����ʒu�̃Z�b�g
 * @param	camera		        FIELD�J��������|�C���^
 * @param camPos            �ݒ肷��ʒu���
 *
 * FIELD_CAMERA�����ŃJ�����ʒu�����䂳��Ă���Ƃ��ɂ�
 * ���f����Ȃ��ꍇ������̂Œ���
 */
//------------------------------------------------------------------
void FIELD_CAMERA_SetCameraPos( FIELD_CAMERA * camera, const VecFx32 * camPos)
{
	GF_ASSERT( (camera->mode == FIELD_CAMERA_MODE_DIRECT_POS) || (camera->mode == FIELD_CAMERA_MODE_CALC_TARGET_POS) );
  camera->camPos = *camPos;
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
	GF_ASSERT( (camera->mode == FIELD_CAMERA_MODE_CALC_CAMERA_POS)||(camera->mode == FIELD_CAMERA_MODE_CALC_TARGET_POS) );
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
	GF_ASSERT( (camera->mode == FIELD_CAMERA_MODE_CALC_CAMERA_POS)||(camera->mode == FIELD_CAMERA_MODE_CALC_TARGET_POS) );
	camera->angle_yaw = angle;
}

const u16 * FIELD_CAMERA_GetAngleYawAddress( const FIELD_CAMERA *camera )
{
  return( &camera->angle_yaw );
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
	GF_ASSERT( (camera->mode == FIELD_CAMERA_MODE_CALC_CAMERA_POS)||(camera->mode == FIELD_CAMERA_MODE_CALC_TARGET_POS) );
	camera->angle_len = length;
}



#ifdef  PM_DEBUG
#include "test/camera_adjust_view.h"
extern fx32	fldWipeScale;
//------------------------------------------------------------------
//  �f�o�b�O�p�F����ʑ���Ƃ̃o�C���h
//------------------------------------------------------------------
void FIELD_CAMERA_DEBUG_BindSubScreen(FIELD_CAMERA * camera, void * param, FIELD_CAMERA_DEBUG_BIND_TYPE type)
{ 
  GFL_CAMADJUST * gflCamAdjust = param;

  // �t�@�[���W���擾
  GFL_G3D_CAMERA_GetFar( camera->g3Dcamera, &camera->debug_far );

  camera->debug_subscreen_type = type;
  if( type == FIELD_CAMERA_DEBUG_BIND_CAMERA_POS )
  {
	  FIELD_CAMERA_SetMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
    GFL_CAMADJUST_SetCameraParam(	gflCamAdjust,
																	&camera->angle_yaw, 
																	&camera->angle_pitch, 
																	&camera->angle_len, 
																	&camera->fovy, 
																	&camera->debug_far);  
    GFL_CAMADJUST_SetWipeParam(		gflCamAdjust, &fldWipeScale );
  }
  else if( type == FIELD_CAMERA_DEBUG_BIND_TARGET_POS )
  {
	  FIELD_CAMERA_SetMode( camera, FIELD_CAMERA_MODE_CALC_TARGET_POS );
    camera->debug_target_yaw    = camera->angle_yaw - 0x8000;
    camera->debug_target_pitch  = 0x10000 - camera->angle_pitch;
    camera->debug_target_len    = camera->angle_len;
    GFL_CAMADJUST_SetCameraParam(	gflCamAdjust,
																	&camera->debug_target_yaw, 
																	&camera->debug_target_pitch, 
																	&camera->debug_target_len, 
																	&camera->fovy, 
																	&camera->debug_far);  
    GFL_CAMADJUST_SetWipeParam(		gflCamAdjust, &fldWipeScale );
  }
}

void FIELD_CAMERA_DEBUG_ReleaseSubScreen(FIELD_CAMERA * camera)
{ 

  if( camera->debug_subscreen_type == FIELD_CAMERA_DEBUG_BIND_TARGET_POS ){

    OS_TPrintf( "DEBUG Camera Target x[0x%x] y[0x%x] z[0x%x]\n", camera->debug_target.x, camera->debug_target.y, camera->debug_target.z  );
    OS_TPrintf( "DEBUG Camera Pos x[0x%x] y[0x%x] z[0x%x]\n", camera->camPos.x, camera->camPos.y, camera->camPos.z  );

    VEC_Subtract( &camera->debug_target, &camera->target, &camera->target_offset );
    OS_TPrintf( "DEBUG Scroll Camera Target Offset x[0x%x] y[0x%x] z[0x%x]\n", camera->target_offset.x, camera->target_offset.y, camera->target_offset.z  );
  }

  camera->debug_subscreen_type = FIELD_CAMERA_DEBUG_BIND_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	���삵�Ă���^�[�Q�b�g���擾
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_DEBUG_GetBindSubScreenTarget( const FIELD_CAMERA * camera, VecFx32* p_target )
{
	*p_target = camera->debug_target;
}


//----------------------------------------------------------------------------
/**
 *	@brief	�f�t�H���g�^�[�Q�b�g�̐ݒ�
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_DEBUG_SetDefaultTarget( FIELD_CAMERA* camera, const VecFx32* target )
{
	camera->default_target = target;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�t�H���g�^�[�Q�b�g�̎擾
 */
//-----------------------------------------------------------------------------
const VecFx32* FIELD_CAMERA_DEBUG_GetDefaultTarget( const FIELD_CAMERA* camera )
{
	return camera->default_target;
}

#endif  //PM_DEBUG

//---------------------------------------------------------------------------
/**
 * @brief
 */
//---------------------------------------------------------------------------
static void updateTraceData(CAMERA_TRACE * trace,
    const VecFx32 * inTarget, const VecFx32 * inCamPos, VecFx32 * outTarget, VecFx32 * outCamPos)
{
	int cam_ofs;
	int tgt_ofs;

	if (trace==NULL)
  {
		*outTarget = *inTarget;
		*outCamPos = *inCamPos;
    return;
  }

  cam_ofs = trace->CamPoint;
  tgt_ofs = trace->TargetPoint;

  if (trace->UpdateFlg){
    //�����f�[�^������W�擾
    *outTarget = trace->targetBuffer[cam_ofs];
    *outCamPos = trace->camPosBuffer[cam_ofs];
  }
  else
  {
		*outTarget = *inTarget;
		*outCamPos = *inCamPos;
    if (cam_ofs==trace->Delay){
      trace->UpdateFlg = TRUE;
    }
  }

  //�Q�ƈʒu�X�V
  cam_ofs = (cam_ofs+1) % trace->bufsize;
  //�����ɐς�
  trace->targetBuffer[tgt_ofs] = *inTarget;
  trace->camPosBuffer[tgt_ofs] = *inCamPos;
  //���������ʒu�X�V
  tgt_ofs = (tgt_ofs+1) % trace->bufsize;
  
  trace->CamPoint = cam_ofs;
  trace->TargetPoint = tgt_ofs;

  //�g���[�X�f�[�^�̖������W���́A���̂܂܌��ݍ��W���̗p
  if (!trace->ValidX){	//����������
    outTarget->x = inTarget->x;
    outCamPos->x = inCamPos->x;
  }
  if (!trace->ValidY){	//����������
    outTarget->y = inTarget->y;
    outCamPos->y = inCamPos->y;
  }
  if (!trace->ValidZ){	//����������
    outTarget->z = inTarget->z;
    outCamPos->z = inCamPos->z;
  }
}


//---------------------------------------------------------------------------
/**
 * @brief	�g���[�X�\���̂̃A���P�[�V�����@�J�������Ώە��Ƀo�C���h�ς݂ł��邱��
 * 
 * @param	inHistNum		�o�b�t�@�T�C�Y
 * @param	inDelay			�x���x����
 * @param	inTraceMask		�g���[�X�}�X�N	camera.h�ɒ�`
 * @param	inHeapID		�q�[�vID
 * @param	ioCamera		�J�����|�C���^
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void createTraceData(const int inHistNum, const int inDelay,
						const int inTraceMask, const int inHeapID,
						FIELD_CAMERA * ioCamera)
{
	int i;
	CAMERA_TRACE *trace;
	//�o�C���h����Ă��Ȃ���΁A�������Ȃ�
	if (ioCamera->watch_target == NULL){
		return;
	}
	//�z��T�C�Y�̐������`�F�b�N
	GF_ASSERT_MSG(inDelay+1<=inHistNum, "�z�񐔂�����܂���");
	
	trace = GFL_HEAP_AllocMemory(inHeapID,sizeof(CAMERA_TRACE));
  trace->targetBuffer = GFL_HEAP_AllocMemory(inHeapID, sizeof(VecFx32)*inHistNum);
  trace->camPosBuffer = GFL_HEAP_AllocMemory(inHeapID, sizeof(VecFx32)*inHistNum);
	
	//�g���[�X�z��N���A
	for(i=0;i<inHistNum;i++){
    VEC_Set(&trace->targetBuffer[i], 0, 0, 0);
    VEC_Set(&trace->camPosBuffer[i], 0, 0, 0);
	}
	
	trace->bufsize = inHistNum;
	//�O�ԖڂɃJ�����Q�ƈʒu���Z�b�g
	trace->CamPoint = 0;
	//�Ώە��Q�ƈʒu�Z�b�g
	trace->TargetPoint = 0+inDelay;
	
	trace->Delay = inDelay;
	trace->UpdateFlg = FALSE;
	
	trace->ValidX = FALSE;
	trace->ValidY = FALSE;
	trace->ValidZ = FALSE;
	if (inTraceMask & CAM_TRACE_MASK_X){
		trace->ValidX = TRUE;
	}
	if (inTraceMask & CAM_TRACE_MASK_Y){
		trace->ValidY = TRUE;
	}
	if (inTraceMask & CAM_TRACE_MASK_Z){
		trace->ValidZ = TRUE;
	}
	
	ioCamera->Trace = trace;
}

//---------------------------------------------------------------------------
/**
 * @brief	�g���[�X�f�[�^�̉��
 * 
 * @param	camera_ptr		�J�����|�C���^
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void deleteTraceData(FIELD_CAMERA * camera_ptr)
{
	if (camera_ptr->Trace!=NULL){
    GFL_HEAP_FreeMemory(camera_ptr->Trace->targetBuffer);
    GFL_HEAP_FreeMemory(camera_ptr->Trace->camPosBuffer);
		GFL_HEAP_FreeMemory(camera_ptr->Trace);
		camera_ptr->Trace = NULL;
	}
}


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void traceUpdate(FIELD_CAMERA * camera)
{
  VecFx32 cam, tgt;
  VecFx32 outCam, outTgt;

  if (camera->watch_target == NULL) return;


  GFL_G3D_CAMERA_GetTarget(camera->g3Dcamera, &tgt);
  GFL_G3D_CAMERA_GetPos(camera->g3Dcamera, &cam);

  updateTraceData( camera->Trace, &tgt, &cam, &outTgt, &outCam );

  GFL_G3D_CAMERA_SetTarget( camera->g3Dcamera, &outTgt );
  GFL_G3D_CAMERA_SetPos( camera->g3Dcamera, &outCam );

  if (GFL_UI_KEY_GetCont() & PAD_BUTTON_L) {
    TAMADA_Printf("inTGT:%3d inCAM:%3d ", FX_Whole(tgt.y), FX_Whole(cam.y) );
    TAMADA_Printf("outTGT:%3d outCAM:%3d \n", FX_Whole(outTgt.y), FX_Whole(outCam.y) );
  }
}


