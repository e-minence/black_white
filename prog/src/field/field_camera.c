//============================================================================================
/**
 * @file	field_camera.c
 * @brief	�t�B�[���h�J��������
 * @author	tamada GAME FREAK inc.
 */
//============================================================================================
#include <gflib.h>
#include <calctool.h>

#include "field/field_const.h"

#include "field_g3d_mapper.h"
#include "field_common.h"
#include "field_camera.h"
	
#include "arc_def.h"
#include "arc/fieldmap/field_camera_data.naix"


// �f�o�b�N�@�\���Ⴄ�\�[�X�ɂ��邽�߂ɁA��`��ʃw�b�_�[�Ɉڂ��܂����B
#include "field_camera_local.h"


//============================================================================================
//============================================================================================

// x = CAMERA_TRACE*
#define TRACEDATA_GET_FIRST_POINT(x) ((x)->Delay)


static void createTraceData(const int inHistNum, const int inDelay,
						const int inTraceMask, const int inHeapID,
						FIELD_CAMERA * ioCamera);
static void deleteTraceData(FIELD_CAMERA * camera_ptr);
static void updateTraceData(CAMERA_TRACE * trace,
    const VecFx32 * inTarget, const VecFx32 * inCamPos, VecFx32 * outTarget, VecFx32 * outCamPos);
static void traceUpdate(FIELD_CAMERA * camera);
static void traceStop( CAMERA_TRACE * trace );


//------------------------------------------------------------------
//------------------------------------------------------------------
static void loadCameraParameters(FIELD_CAMERA * camera, const VecFx32* watch_target);
static void loadInitialParameter( const FIELD_CAMERA* camera, FLD_CAMERA_PARAM * result );

static void ControlParameter_CalcCamera( FIELD_CAMERA * camera, u16 key_cont );
static void ControlParameter_CalcTarget( FIELD_CAMERA * camera, u16 key_cont );
static void ControlParameter_Direct( FIELD_CAMERA * camera, u16 key_cont );

static void ControlParameterInit_CalcCamera( FIELD_CAMERA * camera );
static void ControlParameterInit_CalcTarget( FIELD_CAMERA * camera );
static void ControlParameterInit_Direct( FIELD_CAMERA * camera );


//------------------------------------------------------------------
//------------------------------------------------------------------
static void modeChange_SetVecAngel( FIELD_CAMERA * camera, const VecFx32* cp_vec );
static void modeChange_CalcCameraPos( FIELD_CAMERA * camera );
static void modeChange_CalcTargetPos( FIELD_CAMERA * camera );
static void modeChange_CalcDirectPos( FIELD_CAMERA * camera );


//------------------------------------------------------------------
//------------------------------------------------------------------
static void updateGlobalAngle( FIELD_CAMERA * camera, const VecFx32* cp_targetPos, const VecFx32* cp_cameraPos );

static void getGosaCalcAngleParam( const VecFx32* n_dist, u16 yaw, u16 pitch, u16* ret_yaw, u16* ret_pitch );

//------------------------------------------------------------------
// �J�������G���A����
//------------------------------------------------------------------
static void cameraArea_UpdateTarget( const FIELD_CAMERA_AREA * cp_area, VecFx32* p_target );
static void cameraArea_UpdateCamPos( const FIELD_CAMERA_AREA * cp_area, VecFx32* p_camera );
static BOOL cameraArea_IsAreaRect( const FIELD_CAMERA_AREA* cp_area, const VecFx32* cp_pos, VecFx32* p_pos );
static BOOL cameraArea_IsAreaCircle( const FIELD_CAMERA_AREA* cp_area, const VecFx32* cp_pos, VecFx32* p_pos );
static BOOL cameraArea_IsAreaRectIn( const FIELD_CAMERA_AREA* cp_area, const VecFx32* cp_pos, VecFx32* p_pos );

static BOOL (*pIsAreaFunc[])( const FIELD_CAMERA_AREA* cp_area, const VecFx32* cp_pos, VecFx32* p_pos ) = 
{
  NULL,
  cameraArea_IsAreaRect,
  cameraArea_IsAreaCircle,
  cameraArea_IsAreaRectIn,
};

//------------------------------------------------------------------
// ���`�J�����ړ��֘A����
//------------------------------------------------------------------
static void SetNowCameraParam(const FIELD_CAMERA * inCamera, FLD_CAM_MV_PARAM_CORE *outParam);
static void SetNowCameraParamAreaOv(const FIELD_CAMERA * inCamera, FLD_CAM_MV_PARAM_CORE *outParam);
static void SetLinerParamCommon(
    FIELD_CAMERA * camera, const FLD_CAM_MV_PARAM *param, const u16 inFrame);
#if 0
static void RecvModeCamera(FIELD_CAMERA * camera);
static void RecvModeTarget(FIELD_CAMERA * camera);
static void RecvModeDirect(FIELD_CAMERA * camera);
#endif
static void LinerMoveFunc(FIELD_CAMERA * camera, void *work);
static void DirectLinerMoveFunc(FIELD_CAMERA * camera, void *work);

static void VecSubFunc(
    const VecFx32 *inSrc, const VecFx32 *inDst, const u16 inCostFrm, const u16 inNowFrm, VecFx32 *outVec);
static fx32 SubFuncFx(const fx32 *inSrc, const fx32 *inDst, const u16 inCostFrm, const u16 inNowFrm);
static u16 SubFuncU16(const u16 *inSrc, const u16 *inDst, const u16 inCostFrm, const u16 inNowFrm);


#ifdef PM_DEBUG

static void DumpCameraParam(FIELD_CAMERA * camera);

#endif //PM_DEBUG

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	������
 */
//------------------------------------------------------------------
FIELD_CAMERA* FIELD_CAMERA_Create(
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
	camera->type = type;
	camera->mode = mode;
	camera->g3Dcamera = cam;
  camera->watch_camera = NULL;
	camera->heapID = heapID;

  VEC_Set( &camera->camPos, 0, 0, 0 );
	VEC_Set( &camera->target, 0, 0, 0 );
  VEC_Set( &camera->target_before, 0, 0, 0 );
  VEC_Set( &camera->campos_offset, 0, 0, 0 );

  camera->angle_yaw = 0;
  camera->angle_pitch = 0;
  camera->angle_len = 0x0078;

  camera->camera_area_active = TRUE;


  // �p�[�X���J�����̏�񂩂�擾
  if( GFL_G3D_CAMERA_GetProjectionType( cam ) == GFL_G3D_PRJPERS ){
    fx32 fovyCos;
    GFL_G3D_CAMERA_GetfovyCos( cam, &fovyCos );
    camera->fovy = FX_AcosIdx( fovyCos );
  }

  // �n���h���I�[�v��
  {
    camera->init_param_handle   = GFL_ARC_OpenDataHandle(ARCID_FIELD_CAMERA, camera->heapID);
    camera->camera_area_handle  = GFL_ARC_OpenDataHandle(ARCID_FLD_CAMSCRLL, camera->heapID);

    camera->camera_area_id = 0xffff;
  }


  loadCameraParameters(camera, target);

	createTraceData(	FIELD_CAMERA_TRACE_BUFF, FIELD_CAMERA_DELAY,
							CAM_TRACE_MASK_Y, heapID, camera);

  //�R�[���o�b�N�֐��N���A
  camera->CallBack = NULL;

	return camera;
}
//------------------------------------------------------------------
/**
 * @brief	�I��
 */
//------------------------------------------------------------------
void	FIELD_CAMERA_Delete( FIELD_CAMERA* camera )
{
  // �n���h���N���[�Y
  {
    GFL_ARC_CloseDataHandle(camera->init_param_handle);
    GFL_ARC_CloseDataHandle(camera->camera_area_handle);
  }
  
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
  if (camera->CallBack != NULL){
    camera->CallBack(camera, camera->MoveData.CallBackWorkPtr);
  }
  FIELD_CAMERA_LOCAL_ControlParameter(camera, key_cont);
}


//----------------------------------------------------------------------------
/**
 *	@brief	�J�������[�h�@�ݒ�
 *	
 *	@param	camera		�J����
 *	@param	mode			���[�h
 *
 *
//	  �ύX���̒��ӓ_
//	    �e���[�h�ւ̕ύX�ɂ��A�����ȏ��̈��p����������邽�߁A
//	    �ȉ��̏�񂪏���������܂��B
//      �yFIELD_CAMERA_MODE_CALC_CAMERA_POS�z
//          FIELD_CAMERA_BindCamera()�Őݒ肵��watch_camera
//          
//      �yFIELD_CAMERA_MODE_CALC_TARGET_POS�z
//          FIELD_CAMERA_BindTarget()�Őݒ肵��watch_target
//          
//      �yFIELD_CAMERA_MODE_DIRECT_POS�z
//          �A���O�����
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetMode( FIELD_CAMERA * camera, FIELD_CAMERA_MODE mode )
{
  static void (*pInit[])( FIELD_CAMERA* camera ) = 
  {
    ControlParameterInit_CalcCamera,
    ControlParameterInit_CalcTarget,
    ControlParameterInit_Direct,
  };
  
	GF_ASSERT( camera );
	GF_ASSERT( mode < FIELD_CAMERA_MODE_MAX );
	camera->mode = mode;

  // �e���[�h�̏���������
  pInit[ mode ]( camera );
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

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������[�h�@���[�h�̕ύX
 *
 *	@param	camera
 *	@param	mode 
 *
//	  �ύX���̒��ӓ_
//	    �e���[�h�ւ̕ύX�ɂ��A�����ȏ��̈��p����������邽�߁A
//	    �ȉ��̏�񂪏���������܂��B
//      �yFIELD_CAMERA_MODE_CALC_CAMERA_POS�z
//          FIELD_CAMERA_BindCamera()�Őݒ肵��watch_camera
//          
//      �yFIELD_CAMERA_MODE_CALC_TARGET_POS�z
//          FIELD_CAMERA_BindTarget()�Őݒ肵��watch_target
//          
//      �yFIELD_CAMERA_MODE_DIRECT_POS�z
//          �A���O�����
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_ChangeMode( FIELD_CAMERA * camera, FIELD_CAMERA_MODE mode )
{
  static void (*pFunc[ FIELD_CAMERA_MODE_MAX ])( FIELD_CAMERA* camera ) = {
    modeChange_CalcCameraPos,
    modeChange_CalcTargetPos,
    modeChange_CalcDirectPos,
  };
  
	GF_ASSERT( camera );
  GF_ASSERT( mode < FIELD_CAMERA_MODE_MAX );
  
  if( camera->mode != mode )
  {
    pFunc[ mode ]( camera );

    FIELD_CAMERA_SetMode( camera, mode );
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J������ZONE�̃f�t�H���g�p�����[�^��ݒ肷��B
 *
 *	@param	camera  �J����
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetDefaultParameter( FIELD_CAMERA * camera )
{
  // �J�������[�h��ύX
  FIELD_CAMERA_SetMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
  
  // �]�[���̃J������ǂݍ���
  loadCameraParameters( camera, camera->default_target );

  // ���̑��p�����[�^��������
  camera->camera_area_active = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����̃f�t�H���g�^�C�v��ύX���܂��B
 *
 *	@param	camera        �J����
 *	@param	camera_type   �^�C�v�iresource/fldmapdata/camera_data�̃G�N�Z���̃J���������Ă���l�ł��B�j
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetCameraType( FIELD_CAMERA * camera, u32 camera_type )
{
  camera->type = camera_type;
  FIELD_CAMERA_SetDefaultParameter( camera );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�t�H���g�̃J�����^�C�v���擾���܂��B�E
 *
 *	@param	camera  �J����
 *
 *	@return �J�����^�C�v
 */
//-----------------------------------------------------------------------------
u32 FIELD_CAMERA_GetCameraType( const FIELD_CAMERA * camera )
{
  return camera->type;
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
static void loadCameraParameters(FIELD_CAMERA * camera, const VecFx32* watch_target)
{
  FLD_CAMERA_PARAM param;

  // �p�����[�^�ǂݍ���
  loadInitialParameter( camera, &param );
  
#if 1
  TOMOYA_Printf("FIELD CAMERA INIT INFO\n");
  TOMOYA_Printf("FIELD CAMERA TYPE =%d\n",camera->type);
  TOMOYA_Printf("dist = %08x\n", param.Distance);
  TOMOYA_Printf("angle = %08x,%08x,%08x\n", param.Angle.x, param.Angle.y, param.Angle.z );
  TOMOYA_Printf("viewtype = %d, persp = %08x\n", param.View, param.PerspWay);
  TOMOYA_Printf("near=%8x, far =%08x\n", param.Near, param.Far);
  TOMOYA_Printf("playerLink%d\n", param.PlayerLink  );
  TOMOYA_Printf("shift = %08x,%08x,%08x\n", param.Shift.x, param.Shift.y, param.Shift.z );
  TOMOYA_Printf("now near = %d\n", FX_Whole( FIELD_CAMERA_GetNear(camera) ) );
  TOMOYA_Printf("now far = %d\n", FX_Whole( FIELD_CAMERA_GetFar(camera) ) );
#endif

#ifdef PM_DEBUG
  if( param.depthType == DEPTH_TYPE_ZBUF)
  {
    camera->debug_save_buffer_mode = 2; // CAMERA_DEBUG_BUFFER_MODE_ZBUFF_AUTO
  }
  else
  {
    camera->debug_save_buffer_mode = 3; // CAMERA_DEBUG_BUFFER_MODE_WBUFF_AUTO
  }

  // ��{�^�[�Q�b�g�I�t�Z�b�g�ݒ�
  camera->debug_default_target_offs = param.Shift;
  
#endif

  camera->angle_len = param.Distance * FX32_ONE;
  camera->angle_pitch = param.Angle.x;
  camera->angle_yaw = param.Angle.y;
  //camera->viewtype = param.ViewType;
  camera->fovy = param.PerspWay;
  FIELD_CAMERA_SetFar( camera, param.Far );
  FIELD_CAMERA_SetNear(camera, param.Near );
  switch (param.depthType)
  {
  case DEPTH_TYPE_ZBUF:
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
    break;
  case DEPTH_TYPE_WBUF:
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
    break;
  }
  if( GFL_G3D_CAMERA_GetProjectionType(camera->g3Dcamera) == GFL_G3D_PRJPERS ){
    GFL_G3D_CAMERA_SetfovySin( camera->g3Dcamera, FX_SinIdx( camera->fovy ) );
    GFL_G3D_CAMERA_SetfovyCos( camera->g3Dcamera, FX_CosIdx( camera->fovy ) );
  }

  // �v���C���[�Ƃ̃o�C���h�`�F�b�N
  if( param.PlayerLink )
  {
    // �o�C���h����ꍇ
    // watch_target���Z�b�g
    // Shift��target_offset�Ɋi�[
    camera->default_target  = watch_target;
    camera->watch_target    = watch_target;

    camera->target        = *watch_target;
    camera->target_offset = param.Shift;
  }
  else
  {
    // �o�C���h���Ȃ��ꍇ
    // watch_target���N���A
    // Shift��target�Ɋi�[
    camera->default_target  = FALSE;
    camera->watch_target    = FALSE;

    camera->target = param.Shift;
    VEC_Set( &camera->target_offset, 0,0,0 );
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�t�H���g�p�����[�^�̎擾
 *
 *	@param	camera
 *	@param	result 
 */
//-----------------------------------------------------------------------------
static void loadInitialParameter( const FIELD_CAMERA* camera, FLD_CAMERA_PARAM * result )
{
  enum {
    FILE_ID = NARC_field_camera_data_field_camera_bin,
  };
  u16 size = GFL_ARC_GetDataSizeByHandle(camera->init_param_handle, FILE_ID);
  if ( camera->type * sizeof(FLD_CAMERA_PARAM) >= size )
  {
    OS_TPrintf("�J�����^�C�v�i%d�j���w��ł��܂���\n", camera->type);
    GF_ASSERT(0);
  }
  GFL_ARC_LoadDataOfsByHandle(camera->init_param_handle, FILE_ID,
      camera->type * sizeof(FLD_CAMERA_PARAM), sizeof(FLD_CAMERA_PARAM), result);

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
void FIELD_CAMERA_LOCAL_calcAnglePos( const VecFx32* cp_target, VecFx32* p_pos, u16 yaw, u16 pitch, fx32 len )
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
  {
    VEC_Set( &cameraPos, FX_Mul(sinYaw, cosPitch), sinPitch, FX_Mul(cosYaw, cosPitch) );
    VEC_Normalize(&cameraPos, &cameraPos);
    VEC_MultAdd( len, &cameraPos, cp_target, &cameraPos );
  }

  //cameraPos = cameraPos * length + camera->target
  *p_pos = cameraPos;
}

//------------------------------------------------------------------
//------------------------------------------------------------------
static void updateAngleCameraPos(FIELD_CAMERA * camera)
{
  int i;
  // 
  camera->target_write = camera->target;

  if( camera->camera_area_active )
  {
    for( i=0; i<camera->camera_area_num; i++ )
    {
      cameraArea_UpdateTarget( &camera->camera_area[i], &camera->target_write );
    }
  }
  
  // �J�����|�W�V�����v�Z
	FIELD_CAMERA_LOCAL_calcAnglePos( &camera->target_write, &camera->camPos,
			camera->angle_yaw, camera->angle_pitch, camera->angle_len );

  // 
  camera->campos_write = camera->camPos;

  if( camera->camera_area_active )
  {
    for( i=0; i<camera->camera_area_num; i++ )
    {
      cameraArea_UpdateCamPos( &camera->camera_area[i], &camera->campos_write );
    }
  }
}
static void updateAngleTargetPos(FIELD_CAMERA * camera)
{ 
  int i;
  // 
  camera->campos_write = camera->camPos;

  if( camera->camera_area_active )
  {
    for( i=0; i<camera->camera_area_num; i++ )
    {
      cameraArea_UpdateCamPos( &camera->camera_area[i], &camera->campos_write );
    }
  }
    

  // �^�[�Q�b�g�|�W�V�����v�Z
	FIELD_CAMERA_LOCAL_calcAnglePos( &camera->campos_write, &camera->target,
			camera->angle_yaw, camera->angle_pitch, camera->angle_len );

  // 
  camera->target_write = camera->target;

  if( camera->camera_area_active )
  {
    for( i=0; i<camera->camera_area_num; i++ )
    {
      cameraArea_UpdateTarget( &camera->camera_area[i], &camera->target_write );
    }
  }
}

	
//------------------------------------------------------------------
//------------------------------------------------------------------
static void updateCameraArea(FIELD_CAMERA * camera)
{
  int i;
  
  // 
  camera->campos_write = camera->camPos;

  if( camera->camera_area_active )
  {
    for( i=0; i<camera->camera_area_num; i++ )
    {
      cameraArea_UpdateCamPos( &camera->camera_area[i], &camera->campos_write );
    }
  }

  // 
  camera->target_write = camera->target;
  if( camera->camera_area_active )
  {
    for( i=0; i<camera->camera_area_num; i++ )
    {
      cameraArea_UpdateTarget( &camera->camera_area[i], &camera->target_write );
    }
  }
}
	
//------------------------------------------------------------------
//------------------------------------------------------------------
static void updateG3Dcamera(FIELD_CAMERA * camera)
{
	VecFx32 cameraTarget;
	VecFx32 cameraPos;

#ifndef PM_DEBUG
	// �J�����^�[�Q�b�g�␳
  VEC_Add( &camera->target_write, &camera->target_offset, &cameraTarget );
  VEC_Add( &camera->campos_write, &camera->campos_offset, &cameraPos );
  

	GFL_G3D_CAMERA_SetTarget( camera->g3Dcamera, &cameraTarget );
	GFL_G3D_CAMERA_SetPos( camera->g3Dcamera, &cameraPos );

  traceUpdate( camera );
  updateGlobalAngle( camera, &cameraTarget, &camera->campos_write );
#else

  if( camera->debug_subscreen_type != FIELD_CAMERA_DEBUG_BIND_TARGET_POS )
  {
    // �ʏ�̃J�����^�[�Q�b�g
    // �J�����^�[�Q�b�g�␳
    VEC_Add( &camera->target_write, &camera->target_offset, &cameraTarget );
    VEC_Add( &camera->campos_write, &camera->campos_offset, &cameraPos );

    GFL_G3D_CAMERA_SetTarget( camera->g3Dcamera, &cameraTarget );
	  GFL_G3D_CAMERA_SetPos( camera->g3Dcamera, &cameraPos );

    updateGlobalAngle( camera, &cameraTarget, &camera->campos_write );

		if( !camera->debug_trace_off )
		{
			traceUpdate( camera );
		}
  }
  else
  {
    // �^�[�Q�b�g��ύX
    FIELD_CAMERA_LOCAL_calcAnglePos( &camera->camPos, &camera->debug_target, camera->debug_target_yaw,
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

    updateGlobalAngle( camera, &cameraTarget, &camera->campos_write );
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
void FIELD_CAMERA_LOCAL_ControlParameter( FIELD_CAMERA * camera, u16 key_cont )
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
  updateCameraArea( camera );
  updateG3Dcamera(camera);
}



//----------------------------------------------------------------------------
/**
 *	@brief  �J�����Ǘ��������@�J�������W�v�Z�@
 */
//-----------------------------------------------------------------------------
static void ControlParameterInit_CalcCamera( FIELD_CAMERA * camera )
{
  // watch_camera�Ƃ̃o�C���h������
  camera->watch_camera = NULL;
  //OS_TPrintf( "move change CalcCamera watch_camera set NULL\n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����Ǘ�������  �^�[�Q�b�g���W�v�Z
 */
//-----------------------------------------------------------------------------
static void ControlParameterInit_CalcTarget( FIELD_CAMERA * camera )
{
  // watch_target�Ƃ̃o�C���h������
  camera->watch_target = NULL;
  //OS_TPrintf( "move change CalcTarget watch_target set NULL\n" );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����Ǘ�����  �_�C���N�g���W�v�Z
 *
 *	@param	camera 
 */
//-----------------------------------------------------------------------------
static void ControlParameterInit_Direct( FIELD_CAMERA * camera )
{
  // �A���O�����̔j��
  camera->angle_pitch = 0;
  camera->angle_yaw = 0;
  camera->angle_len = 0;
  //OS_TPrintf( "move change Direct Angle set 0\n" );
}



//------------------------------------------------------------------
//------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �A���O�������߂�
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_LOCAL_modeChange_CalcVecAngel( const VecFx32* cp_vec, u16* p_pitch, u16* p_yaw, fx32* p_len )
{
  VecFx32 dist, xz_dist;
  fx32 len;
  fx32 xz_len;
  u16 pitch, yaw;
  
  // ����
  len = VEC_Mag( cp_vec );
  
  // �v�Z���K��
  VEC_Normalize( cp_vec, &dist );
  
  // ���ʂƍ����̔䗦�����߂�
  VEC_Set( &xz_dist, dist.x, 0, dist.z );
  xz_len = VEC_Mag( &xz_dist );

  // pitck
  pitch = FX_Atan2Idx( dist.y, xz_len );

  // yaw
  yaw = FX_Atan2Idx( dist.x, dist.z );

  // �덷��\��
  getGosaCalcAngleParam( &dist, yaw, pitch, &yaw, &pitch );
  

  *p_pitch = pitch;
  *p_yaw = yaw;
  *p_len = len;
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	camera
 *	@param	cp_vec 
 */
//-----------------------------------------------------------------------------
static void modeChange_SetVecAngel( FIELD_CAMERA * camera, const VecFx32* cp_vec )
{
  u16 pitch, yaw;
  fx32 len;

  FIELD_CAMERA_LOCAL_modeChange_CalcVecAngel( cp_vec, &pitch, &yaw, &len );
  // �ݒ�
  camera->angle_pitch   = pitch;
  camera->angle_yaw     = yaw;
  camera->angle_len     = len;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������[�h���J�������W�v�Z���[�h�ɕύX
 *
 *	@param	camera  �J�������[�N
 */
//-----------------------------------------------------------------------------
static void modeChange_CalcCameraPos( FIELD_CAMERA * camera )
{
  VecFx32 dist;

  // 1�O���A�^�[�Q�b�g�v�Z���[�h�̏ꍇ
  if( camera->mode == FIELD_CAMERA_MODE_CALC_TARGET_POS )
  {
    // �A���O���̔��]�łn�j
    camera->angle_yaw += 0x8000;
    camera->angle_pitch += 0x8000;
  }
  // ���l�̏ꍇ
  else
  {
    // �^�[�Q�b�g����J�����܂ł̃x�N�g�������߂�
    VEC_Subtract( &camera->camPos, &camera->target_write, &dist );

    modeChange_SetVecAngel( camera, &dist );
  }
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������[�h���^�[�Q�b�g���W�v�Z���[�h�ɕύX
 *
 *	@param	camera  �J�������[�N
 */
//-----------------------------------------------------------------------------
static void modeChange_CalcTargetPos( FIELD_CAMERA * camera )
{
  VecFx32 dist;

  // 1�O���A�J�����v�Z���[�h�̏ꍇ
  if( camera->mode == FIELD_CAMERA_MODE_CALC_CAMERA_POS )
  {
    // �A���O���̔��]�łn�j
    camera->angle_yaw += 0x8000;
    camera->angle_pitch += 0x8000;
  }
  // ���l�̏ꍇ
  else
  {
    // �J��������^�[�Q�b�g�܂ł̃x�N�g�������߂�
    VEC_Subtract( &camera->target, &camera->campos_write, &dist );

    modeChange_SetVecAngel( camera, &dist );
  }
  
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������[�h�𒼐ڎw�胂�[�h�ɕύX
 *
 *	@param	camera  �J�������[�N
 */
//-----------------------------------------------------------------------------
static void modeChange_CalcDirectPos( FIELD_CAMERA * camera )
{
  // �J�������[�h����A���߂�ׂ����W���v�Z
  if( camera->mode == FIELD_CAMERA_MODE_CALC_CAMERA_POS ){
    updateTargetBinding( camera );
    updateAngleCameraPos(camera);
  }else if( camera->mode == FIELD_CAMERA_MODE_CALC_TARGET_POS ){
    updateCamPosBinding( camera );
    updateAngleTargetPos(camera);
  }
}



//----------------------------------------------------------------------------
/**
 *	@brief  �^�[�Q�b�g���W�ƃJ�������W����A�O���[�o���A���O����ݒ�
 *
 *	@param	camera          �J�������[�N
 *	@param	cp_targetPos    �^�[�Q�b�g���W
 *	@param	cp_camraPos     �J�������W
 */
//-----------------------------------------------------------------------------
static void updateGlobalAngle( FIELD_CAMERA * camera, const VecFx32* cp_targetPos, const VecFx32* cp_cameraPos )
{
  VecFx32 way;

  VEC_Subtract( cp_cameraPos, cp_targetPos, &way );
  VEC_Normalize( &way, &way );

  camera->global_angle_yaw = FX_Atan2Idx( way.x, way.z );

  {
    fx32 dot;
    fx32 dot2;
    u16 new_pitch;

    { //XZ���ʂւ̖@���ƃJ�����x�N�g���̓���
      VecFx32 xz_normal = { 0, FX32_ONE, 0 };
      dot = VEC_DotProduct( &xz_normal, &way );
    }

    { //�J�����x�N�g����XZ���ʂɎˉe�����x�N�g���ƃJ�����x�N�g���̓���
      VecFx32 xz_vec;
      xz_vec = way;
      xz_vec.y = 0;
      VEC_Normalize( &xz_vec, &xz_vec );
      dot2 = VEC_DotProduct( &xz_vec, &way );
    }

    if ( dot2 != 0 ) {
      new_pitch = FX_Atan2Idx( dot, dot2 );
    } else {
      new_pitch = 0;
    }
#if 0
    if ( camera->global_angle_pitch != new_pitch )
    {
      OS_Printf("new pitch = %04x (x = %f y=%f, z =%f)\n", new_pitch,
          FX_FX32_TO_F32(way.x), FX_FX32_TO_F32(way.y), FX_FX32_TO_F32(way.z) );
      OS_Printf("dot = %f dot2 = %f\n", FX_FX32_TO_F32(dot), FX_FX32_TO_F32(dot2) );
    }
#endif
    camera->global_angle_pitch = new_pitch;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  �덷���l�������A���O���ɕύX
 *
 *	@param	n_dist    �A���O���̌v�Z�Ɏg�p�����P�ʃx�N�g��
 *	@param	yaw       �A���O��Yaw
 *	@param	pitch     �A���O��Pitch
 *	@param	ret_yaw   Yaw�߂�l
 *	@param	ret_pitch Pitch�߂�l
 */
//-----------------------------------------------------------------------------
static void getGosaCalcAngleParam( const VecFx32* n_dist, u16 yaw, u16 pitch, u16* ret_yaw, u16* ret_pitch )
{
  VecFx16 gosa = {0,0,0};
  VecFx16 gosa_1 = {0,0,0};
  VecFx16 gosa_2 = {0,0,0};
  fx16 gosa_y_1 = 0;
  fx16 gosa_y_2 = 0;
  fx16 sinYaw;
  fx16 cosYaw;
  fx16 sinPitch;
  fx16 cosPitch;

  // �߂�l
  *ret_yaw = yaw;
  *ret_pitch = pitch;

  TOMOYA_Printf( "before yaw %d  pitch %d\n", yaw, pitch );


  // �܂�����
  
  // ���傤��
  {
    sinYaw = FX_SinIdx( yaw );
    cosYaw = FX_CosIdx( yaw );
    sinPitch = FX_SinIdx( pitch );
    cosPitch = FX_CosIdx( pitch );
    gosa.x = FX_Mul( sinYaw, cosPitch );
    gosa.y = sinPitch;
    gosa.z = FX_Mul( cosYaw, cosPitch );
  }

  // -1�x
  {
    // ����
    sinYaw = FX_SinIdx( yaw - 182 );
    cosYaw = FX_CosIdx( yaw - 182 );
    sinPitch = FX_SinIdx( pitch );
    cosPitch = FX_CosIdx( pitch );
    gosa_1.x = FX_Mul( sinYaw, cosPitch );
    gosa_1.z = FX_Mul( cosYaw, cosPitch );

    // Y
    sinPitch = FX_SinIdx( pitch - 182 );
    gosa_y_1 = sinPitch;
  }

  // +1�x
  {
    // ����
    sinYaw = FX_SinIdx( yaw + 182 );
    cosYaw = FX_CosIdx( yaw + 182 );
    sinPitch = FX_SinIdx( pitch );
    cosPitch = FX_CosIdx( pitch );
    gosa_2.x = FX_Mul( sinYaw, cosPitch );
    gosa_2.z = FX_Mul( cosYaw, cosPitch );

    // Y
    sinPitch = FX_SinIdx( pitch + 182 );
    gosa_y_2 = sinPitch;
  }

  // yaw�̃`�F�b�N
  // 0�`90
  if( yaw < 0x4000 ){
    
    if( (gosa.x < n_dist->x) && (gosa.z > n_dist->z) ){
      // Yaw�}�C�i�X����
      if( MATH_ABS(gosa.x - n_dist->x) >= MATH_ABS(gosa.z - n_dist->z) ){
        yaw -= (182 * (n_dist->x - gosa.x)) / (gosa_1.x - gosa.x);
      }else{
        yaw -= (182 * (gosa.z - n_dist->z)) / (gosa.z - gosa_1.z);
      }
    }else if( (gosa.x > n_dist->x) && (gosa.z < n_dist->z) ){
      // Yaw�v���X����
      if( MATH_ABS(gosa.x - n_dist->x) >= MATH_ABS(gosa.z - n_dist->z) ){
        yaw += (182 * (gosa.x - n_dist->x)) / (gosa.x - gosa_2.x);
      }else{
        yaw += (182 * (n_dist->z - gosa.z)) / (gosa_2.z - gosa.z);
      }
    }
    
  // 90�`180
  }else if( yaw < 0x8000 ){

    if( (gosa.x > n_dist->x) && (gosa.z > n_dist->z) ){
      // Yaw�}�C�i�X����
      if( MATH_ABS(gosa.x - n_dist->x) >= MATH_ABS(gosa.z - n_dist->z) ){
        yaw -= (182 * (gosa.x - n_dist->x)) / (gosa.x - gosa_1.x);
      }else{
        yaw -= (182 * (gosa.z - n_dist->z)) / (gosa.z - gosa_1.z);
      }
    }else if( (gosa.x < n_dist->x) && (gosa.z < n_dist->z) ){
      // Yaw�v���X����
      if( MATH_ABS(gosa.x - n_dist->x) >= MATH_ABS(gosa.z - n_dist->z) ){
        yaw += (182 * (n_dist->x - gosa.x)) / (gosa_2.x - gosa.x);
      }else{
        yaw += (182 * (n_dist->z - gosa.z)) / (gosa_2.z - gosa.z);
      }
    }

  // 180�`270
  }else if( yaw < 0xC000 ){

    if( (gosa.x > n_dist->x) && (gosa.z < n_dist->z) ){
      // Yaw�}�C�i�X����
      if( MATH_ABS(gosa.x - n_dist->x) >= MATH_ABS(gosa.z - n_dist->z) ){
        yaw -= (182 * (gosa.x - n_dist->x)) / (gosa.x - gosa_1.x);
      }else{
        yaw -= (182 * (n_dist->z - gosa.z)) / (gosa_1.z - gosa.z);

      }
    }else if( (gosa.x < n_dist->x) && (gosa.z > n_dist->z) ){
      // Yaw�v���X����
      if( MATH_ABS(gosa.x - n_dist->x) >= MATH_ABS(gosa.z - n_dist->z) ){
        yaw += (182 * (n_dist->x - gosa.x)) / (gosa_2.x - gosa.x);
      }else{
        yaw += (182 * (gosa.z - n_dist->z)) / (gosa.z - gosa_2.z);
      }
    }

  // 270�`360
  }else{

    if( (gosa.x < n_dist->x) && (gosa.z < n_dist->z) ){
      // Yaw�}�C�i�X����
      if( MATH_ABS(gosa.x - n_dist->x) >= MATH_ABS(gosa.z - n_dist->z) ){
        yaw -= (182 * (n_dist->x - gosa.x)) / (gosa_1.x - gosa.x);
      }else{
        yaw -= (182 * (n_dist->z - gosa.z)) / (gosa_1.z - gosa.z);

      }
    }else if( (gosa.x > n_dist->x) && (gosa.z > n_dist->z) ){
      // Yaw�v���X����
      if( MATH_ABS(gosa.x - n_dist->x) >= MATH_ABS(gosa.z - n_dist->z) ){
        yaw += (182 * (gosa.x - n_dist->x)) / (gosa.x - gosa_2.x);
      }else{
        yaw += (182 * (gosa.z - n_dist->z)) / (gosa.z - gosa_2.z);
      }
    }
  }



  // Pitch�̃`�F�b�N
  if( pitch < 0x4000 ){
    
    if( (gosa.y > n_dist->y) ){

      // Yaw�}�C�i�X����
      pitch -= (182 * (gosa.y - n_dist->y)) / (gosa.y - gosa_y_1);

    }else if( (gosa.y < n_dist->y) ){
      // Yaw�v���X����
      pitch += (182 * (n_dist->y - gosa.y)) / (gosa_y_2 - gosa.y);
    }
    
  // 270�`360
  }else if( pitch >= 0xC000 ){

    if( (gosa.y > n_dist->y) ){

      // Yaw�}�C�i�X����
      pitch -= (182 * (gosa.y - n_dist->y)) / (gosa.y - gosa_y_1);

    }else if( (gosa.y < n_dist->y) ){
      // Yaw�v���X����
      pitch += (182 * (n_dist->y - gosa.y)) / (gosa_y_2 - gosa.y);
    }

  }

  TOMOYA_Printf( "after yaw %d  pitch %d\n", yaw, pitch );

  *ret_yaw = yaw;
  *ret_pitch = pitch;
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
const VecFx32 *FIELD_CAMERA_GetWatchTarget(FIELD_CAMERA * camera)
{
	return camera->watch_target;
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

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������W�̃o�C���h�����邩�`�F�b�N
 *
 *	@param	camera
 *
 *	@retval TRUE  ����
 *	@retval FALSE  �Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL FIELD_CAMERA_IsBindCamera(const FIELD_CAMERA * camera)
{
	GF_ASSERT( camera );
	if( camera->watch_camera )
  {
    return TRUE;
  }
  return FALSE;
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


//------------------------------------------------------------------
/**
 * @brief	�J�������W�@�␳���W�̎擾
 * @param	camera		    FIELD�J��������|�C���^
 * @param	campos_offset	�J�������W��␳������W�󂯎��VecFx32�ւ̃|�C���^
 */
//------------------------------------------------------------------
void	FIELD_CAMERA_GetCamPosOffset( const FIELD_CAMERA* camera, VecFx32* campos_offset )
{
  *campos_offset = camera->campos_offset;
}



//------------------------------------------------------------------
/**
 * @brief	�J�������W�@�␳���W�̎擾
 * @param	camera		        FIELD�J��������|�C���^
 * @param	campos_offset			�J�������W��␳������W�n��VecFx32�ւ̃|�C���^
 */
//------------------------------------------------------------------
void	FIELD_CAMERA_SetCamPosOffset( FIELD_CAMERA* camera, const VecFx32* campos_offset )
{
  camera->campos_offset = *campos_offset;
}



//----------------------------------------------------------------------------
/**
 *	@brief  �J�����������ݒ�
 *
 *	@param	camera    �J����
 *	@param	camUp     �J���������
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetCameraUp( FIELD_CAMERA * camera, const VecFx32 * camUp)
{
  GFL_G3D_CAMERA_SetCamUp( camera->g3Dcamera, camUp );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J����������̎擾
 *
 *	@param	camera  �J����
 *	@param	camUp   �����
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_GetCameraUp( const FIELD_CAMERA * camera, VecFx32 * camUp)
{
  GFL_G3D_CAMERA_GetCamUp( camera->g3Dcamera, camUp );
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

const u16 * FIELD_CAMERA_GetAnglePitchAddress( const FIELD_CAMERA *camera )
{
  return( &camera->global_angle_pitch );
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
  return( &camera->global_angle_yaw );
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

//-----------------------------------------------------------------------------
/**
 * @brief �A���O�������߂�
 *
 * @param cp_vec  �^�[�Q�b�g���J���� �x�N�g��
 * @param p_pitch ���߂��s�b�`���󂯎��u16�ւ̃|�C���^
 * @param p_yaw   ���߂����[���󂯎��u16�ւ̃|�C���^
 * @param p_len   ���߂��������󂯎��fx32�ւ̃|�C���^
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_CalcVecAngle( const VecFx32* cp_vec, u16* p_pitch, u16* p_yaw, fx32* p_len )
{
  FIELD_CAMERA_LOCAL_modeChange_CalcVecAngel( cp_vec, p_pitch, p_yaw, p_len );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�J��������p����@����p�擾
 *	@param  camera    �J�����|�C���^
 *
 *	@return u16       ����p
 */
//-----------------------------------------------------------------------------
u16 FIELD_CAMERA_GetFovy(const FIELD_CAMERA * camera )
{
  // �p�[�X���J�����̏�񂩂�擾
  if( GFL_G3D_CAMERA_GetProjectionType(camera->g3Dcamera) == GFL_G3D_PRJPERS ){
    return camera->fovy;
  }else{
    return 0;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J��������p����@����p�Z�b�g
 *	@param  camera    �J�����|�C���^
 *	@param  fovy       ����p
 *
 *	@return none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetFovy(FIELD_CAMERA * camera, u16 fovy )
{
	camera->fovy = fovy;
  GFL_G3D_CAMERA_SetfovySin( camera->g3Dcamera, FX_SinIdx( camera->fovy ) );
  GFL_G3D_CAMERA_SetfovyCos( camera->g3Dcamera, FX_CosIdx( camera->fovy ) );
}


//---------------------------------------------------------------------------
/**
 * @brief
 *
 *10.05.06 tomoya 
 * ������SMOOTH�ɂ��邽�߁A
 * �o�b�t�@�ɃJ�����A�^�[�Q�b�g���W�����܂肫��܂ł́A
 * �ŏ��Ɋi�[���ꂽ���W��Ԃ�������悤�ɕύX
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
  if (!trace->Valid)
  {
    *outTarget = *inTarget;
		*outCamPos = *inCamPos;
    return;
  }

  cam_ofs = trace->CamPoint;    // �Q�ƈʒu
  tgt_ofs = trace->TargetPoint; // �i�[�ʒu

  // �i�[����
  if( !trace->StopReq ){
    //�����ɐς�
    trace->targetBuffer[tgt_ofs] = *inTarget;
    trace->camPosBuffer[tgt_ofs] = *inCamPos;
    //���������ʒu�X�V
    tgt_ofs = (tgt_ofs+1) % trace->bufsize;
  }

  // �ݒ�����擾
  if (trace->UpdateFlg){
    //�����f�[�^������W�擾
    *outTarget = trace->targetBuffer[cam_ofs];
    *outCamPos = trace->camPosBuffer[cam_ofs];
  }
  else
  {
    // ���܂��Ă���Ԃ́ATRACEDATA_GET_FIRST_POINT�̈ʒu�̃J������n���B
    *outTarget = trace->targetBuffer[ TRACEDATA_GET_FIRST_POINT(trace) ];
    *outCamPos = trace->camPosBuffer[ TRACEDATA_GET_FIRST_POINT(trace) ];
    if ( cam_ofs==TRACEDATA_GET_FIRST_POINT(trace) ){
      trace->UpdateFlg = TRUE;
    }
  }


  //�X�g�b�v���N�G�X�g���o�Ă���ꍇ
  if (trace->StopReq)
  {
    //�^�[�Q�b�g�ɒǂ����܂ŎQ�ƈʒu�X�V
    if (cam_ofs!=tgt_ofs){
      cam_ofs = (cam_ofs+1) % trace->bufsize;

      //�ǂ�����
      if( cam_ofs == tgt_ofs )
      {
        //�g���[�X���X�g�b�v����
        traceStop( trace );
      }
    }
  }
  else
  {
    //�Q�ƈʒu�X�V
    cam_ofs = (cam_ofs+1) % trace->bufsize;
  }

  trace->CamPoint = cam_ofs;    // �Q�ƈʒu
  trace->TargetPoint = tgt_ofs; // �i�[�ʒu

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
	
	trace->Delay = inDelay;
	trace->UpdateFlg = FALSE;

	//�O�ԖڂɃJ�����Q�ƈʒu���Z�b�g
	trace->CamPoint = 0;
	//�Ώە��Q�ƈʒu�Z�b�g
	trace->TargetPoint = TRACEDATA_GET_FIRST_POINT(trace);
	
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

  trace->Valid = TRUE;
  trace->StopReq = FALSE;

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

  if (camera->watch_target == NULL) {
    return;
  }


  GFL_G3D_CAMERA_GetTarget(camera->g3Dcamera, &tgt);
  GFL_G3D_CAMERA_GetPos(camera->g3Dcamera, &cam);

  updateTraceData( camera->Trace, &tgt, &cam, &outTgt, &outCam );

  GFL_G3D_CAMERA_SetTarget( camera->g3Dcamera, &outTgt );
  GFL_G3D_CAMERA_SetPos( camera->g3Dcamera, &outCam );
}

//----------------------------------------------------------------------------
/**
 *	@brief  TRACE�X�g�b�v
 */
//-----------------------------------------------------------------------------
static void traceStop( CAMERA_TRACE * trace )
{
  trace->StopReq = FALSE;
  trace->Valid = FALSE;
  trace->UpdateFlg = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �g���[�X�����̍ĊJ
 *
 *	@param	camera_ptr  �J�����|�C���^
 *	@return none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_RestartTrace(FIELD_CAMERA * camera_ptr)
{
  CAMERA_TRACE *trace;
  trace = camera_ptr->Trace;
  if (trace == NULL){
    GF_ASSERT(0);
    return;
  }
  //�o�b�t�@�̏����ʒu�̐ݒ�
  {
    int i;
    //�g���[�X�z��N���A
	  for(i=0;i<trace->bufsize;i++){
      VEC_Set(&trace->targetBuffer[i], 0, 0, 0);
      VEC_Set(&trace->camPosBuffer[i], 0, 0, 0);
	  }
    //�O�ԖڂɃJ�����Q�ƈʒu���Z�b�g
    trace->CamPoint = 0;
    //�Ώە��Q�ƈʒu�Z�b�g
    trace->TargetPoint = TRACEDATA_GET_FIRST_POINT(trace);
  }

  trace->UpdateFlg = FALSE;
  //���C���L���t���O���n�m
  trace->Valid = TRUE;
  //Stop���N�G�X�g��������
  trace->StopReq = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �g���[�X�����̒�~���N�G�X�g
 *
 *	@param	camera_ptr  �J�����|�C���^
 *	@return none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_StopTraceRequest(FIELD_CAMERA * camera_ptr)
{
  CAMERA_TRACE *trace;
  trace = camera_ptr->Trace;
  if (trace == NULL){
    GF_ASSERT(0);
    return;
  }

  // ���I����ԃ`�F�b�N
  // ���߂Ă����Ԃł܂��P�����܂��Ă��Ȃ��ꍇ�A
  // ����~��ԂɁB
  if( trace->UpdateFlg == FALSE ){
    if( (trace->TargetPoint == TRACEDATA_GET_FIRST_POINT(trace)) &&
        (trace->CamPoint == 0) ){
      //�g���[�X���X�g�b�v����
      traceStop( trace );
      return ;
    }
  }
  
  // ���̑��̏ꍇ�ł͒�~���N�G�X�g
  trace->StopReq = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������Ƀg���[�X�������~���郊�N�G�X�g
 *
 *	@param	camera_ptr  �J�����|�C���^
 *	@return none
 *
 *	�g���[�X���W���Ⴄ�ꍇ�Ɏg�p����ƁA�J�������}�ɔ�Ԃ悤�ȓ�����s���܂��B
 *	���ӂ��Ďg�p���Ă��������B
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_StopTraceNow(FIELD_CAMERA * camera_ptr)
{
  CAMERA_TRACE *trace;
  trace = camera_ptr->Trace;
  if (trace == NULL){
    return;
  }

#ifdef PM_DEBUG
  // Y���W�ɍ����o�Ȃ����`�F�b�N
  {
    int i;
    s32 check_idx, last_check_idx;

    last_check_idx = trace->TargetPoint-1;
    if( last_check_idx < 0 )
    {
      last_check_idx += trace->bufsize;
    }
    for( i=0; i<trace->bufsize; i++ )
    {
      // �Q�ƃ|�W�V��������i�[�|�W�V�����܂ł�Y���W���ꏏ���`�F�b�N
      check_idx = (trace->CamPoint + i) % trace->bufsize;

      // �I������
      if( check_idx == last_check_idx )
      {
        break;
      }

      if( (trace->camPosBuffer[check_idx].y != trace->camPosBuffer[last_check_idx].y) ||
          (trace->targetBuffer[check_idx].y != trace->targetBuffer[last_check_idx].y) )
      {
        OS_TPrintf( "FIELD_CAMERA_StopTraceNow err �g���[�X���ɒ��g�������Ă��܂�\n" );
        OS_TPrintf( "target (%d == %d)\n", trace->targetBuffer[check_idx].y, trace->targetBuffer[last_check_idx].y );
        OS_TPrintf( "camera (%d == %d)\n", trace->camPosBuffer[check_idx].y, trace->camPosBuffer[last_check_idx].y );
        /*GF_ASSERT( (trace->camPosBuffer[check_idx].y == trace->camPosBuffer[last_check_idx].y) &&
           (trace->targetBuffer[check_idx].y == trace->targetBuffer[last_check_idx].y) );*/
      }
    }
  }
#endif

  //�g���[�X���X�g�b�v����
  trace->StopReq = FALSE;
  trace->Valid = FALSE;
  trace->UpdateFlg = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �g���[�X�����������Ă��邩�̃`�F�b�N
 *
 *	@param	camera_ptr  �J�����|�C���^
 *	@return BOOL        TRUE�ŉғ���
 */
//-----------------------------------------------------------------------------
BOOL FIELD_CAMERA_CheckTrace(FIELD_CAMERA * camera_ptr)
{
  CAMERA_TRACE *trace;
  trace = camera_ptr->Trace;
  if (trace == NULL){
    //GF_ASSERT(0); �����́A����Ȃ��̂ł́H tomoya 2010.3.26
    return FALSE;
  }
  return trace->Valid;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �g���[�X�V�X�e�����L���ł��邩���`�F�b�N
 *
 *	@param	camera_ptr  �J�����|�C���^
 *	@return BOOL        TRUE�ŉғ���
 */
//-----------------------------------------------------------------------------
BOOL FIELD_CAMERA_CheckTraceSys(FIELD_CAMERA * camera_ptr)
{
  CAMERA_TRACE *trace;

  //�E�H�b�`�^�[�Q�b�g���Ȃ��ꍇ�͉ғ������Ă��Ȃ��Ƃ݂Ȃ��B
  if (camera_ptr->watch_target == NULL){
		return FALSE;
	}
  trace = camera_ptr->Trace;
  if (trace == NULL){
    return FALSE;
  }
  return TRUE;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J�������G���A�̐ݒ�
 *
 *	@param	camera      �J����
 *	@param	cp_area     �G���A���
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetCameraArea( FIELD_CAMERA * camera, const FIELD_CAMERA_AREA_SET* cp_area )
{
  GF_ASSERT( cp_area->num < FIELD_CAMERA_AREA_DATA_MAX );
  GFL_STD_MemCopy( cp_area->buff, camera->camera_area, sizeof(FIELD_CAMERA_AREA) * cp_area->num );
  camera->camera_area_num = cp_area->num;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������G���A�̃N���A
 *
 *	@param	camera  �J����
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_ClearCameraArea( FIELD_CAMERA * camera )
{
  GF_ASSERT( camera );
  GFL_STD_MemClear( camera->camera_area, sizeof(FIELD_CAMERA_AREA) * FIELD_CAMERA_AREA_DATA_MAX );
  camera->camera_area_num = 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������G���A�̐����擾
 *
 *	@param	camera  �J����
 *
 *	@return �J�������G���A�̐�
 */
//-----------------------------------------------------------------------------
u32 FIELD_CAMERA_GetCameraAreaNum( const FIELD_CAMERA * camera )
{
  GF_ASSERT( camera );
  return camera->camera_area_num;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������G���A�@�͈̓^�C�v�̎擾
 *
 *	@param	camera  �J����
 *
 *	@return �͈̓^�C�v
 */
//-----------------------------------------------------------------------------
FIELD_CAMERA_AREA_TYPE FIELD_CAMERA_GetCameraAreaType( const FIELD_CAMERA * camera, u32 idx )
{
  GF_ASSERT( camera );
  GF_ASSERT( camera->camera_area_num > idx );
  return camera->camera_area[idx].area_type;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������G���A  �Ǘ����W�^�C�v�̎擾
 *
 *	@param	camera  �J����
 *
 *	@return �Ǘ����W�^�C�v
 */
//-----------------------------------------------------------------------------
FIELD_CAMERA_AREA_CONT FIELD_CAMERA_GetCameraAreaCont( const FIELD_CAMERA * camera, u32 idx )
{
  GF_ASSERT( camera );
  GF_ASSERT( camera->camera_area_num > idx );
  return camera->camera_area[idx].area_cont;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����G���A�̓ǂݍ���
 *
 *	@param	camera    �J�������[�N
 *	@param	area_id   �G���AID
 *	@param	heapID    �q�[�vID
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_LoadCameraArea( FIELD_CAMERA * camera, u32 area_id, HEAPID heapID )
{
  FIELD_CAMERA_AREA_SET* p_rect;
  GF_ASSERT( camera );

  if( camera->camera_area_id == area_id ){
    return ;
  }

  p_rect = GFL_ARCHDL_UTIL_Load( camera->camera_area_handle, 
      area_id, 
      FALSE, GFL_HEAP_LOWID(heapID) );

  TOMOYA_Printf( "camera area rect x_min[%d] x_max[%d] z_min[%d] z_max[%d]\n", 
      p_rect->buff[0].rect.x_min, p_rect->buff[0].rect.x_max,
      p_rect->buff[0].rect.z_min, p_rect->buff[0].rect.z_max );
  
  FIELD_CAMERA_SetCameraArea( camera, p_rect );
  GFL_HEAP_FreeMemory( p_rect );

  camera->camera_area_id = area_id;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������͈͏�����̃^�[�Q�b�g�|�W�V����
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_GetCameraAreaAfterTargetPos( const FIELD_CAMERA * camera, VecFx32* p_pos )
{
  GF_ASSERT( camera );
  GF_ASSERT( p_pos );
  *p_pos = camera->target_write;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������͈͏�����̃J�����|�W�V����
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_GetCameraAreaAfterCameraPos( const FIELD_CAMERA * camera, VecFx32* p_pos )
{
  GF_ASSERT( camera );
  GF_ASSERT( p_pos );
  *p_pos = camera->campos_write;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����G���A�@����t���O��ݒ�
 *
 *	@param	camera    �J����
 *	@param	flag      �t���O
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetCameraAreaActive( FIELD_CAMERA * camera, BOOL flag )
{
  camera->camera_area_active = flag;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����G���A�@����t���O���擾
 *
 *	@param	camera    �J����
 *
 *	@retval TRUE    ����
 *	@retval FALSE   ��~
 */
//-----------------------------------------------------------------------------
BOOL FIELD_CAMERA_GetCameraAreaActive( const FIELD_CAMERA * camera )
{
  return camera->camera_area_active;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J�����G���A���f
 *
 *	@param	cp_area       �G���A���
 *	@param	p_target      �^�[�Q�b�g���W
 */
//-----------------------------------------------------------------------------
static void cameraArea_UpdateTarget( const FIELD_CAMERA_AREA * cp_area, VecFx32* p_target )
{
  VecFx32* p_pos;
  int i;

  GF_ASSERT( cp_area->area_type < FIELD_CAMERA_AREA_MAX );
  GF_ASSERT( cp_area->area_cont < FIELD_CAMERA_AREA_CONT_MAX );

  if( cp_area->area_cont == FIELD_CAMERA_AREA_CONT_TARGET )
  {
    p_pos = p_target;

    if( pIsAreaFunc[cp_area->area_type] )
    {
      pIsAreaFunc[cp_area->area_type]( cp_area, p_pos, p_pos );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����G���A���f
 *
 *	@param	cp_area       �G���A���
 *	@param	p_camera      �J�������W
 */
//-----------------------------------------------------------------------------
static void cameraArea_UpdateCamPos( const FIELD_CAMERA_AREA * cp_area, VecFx32* p_camera )
{
  VecFx32* p_pos;

  GF_ASSERT( cp_area->area_type < FIELD_CAMERA_AREA_MAX );
  GF_ASSERT( cp_area->area_cont < FIELD_CAMERA_AREA_CONT_MAX );

  if( cp_area->area_cont == FIELD_CAMERA_AREA_CONT_CAMERA )
  {
    p_pos = p_camera;

    if( pIsAreaFunc[cp_area->area_type] )
    {
      pIsAreaFunc[cp_area->area_type]( cp_area, p_pos, p_pos );
    }
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������͈͌v�Z�@��`�͈͊O�@�X�g�b�v
 *
 *	@param	cp_area   ��`���
 *	@param  cp_pos	  ���ݍ��W
 *	@param	p_pos     �v�Z����W
 *
 *	@retval TRUE  �J��������͈͊O������
 *	@retval FALSE �J��������͈͓�������
 */
//-----------------------------------------------------------------------------
static BOOL cameraArea_IsAreaRect( const FIELD_CAMERA_AREA* cp_area, const VecFx32* cp_pos, VecFx32* p_pos )
{
  VecFx32 pos_data = *cp_pos;
  BOOL ret = FALSE;

  // Y���W�ݒ�
  p_pos->x = pos_data.x;
  p_pos->z = pos_data.z;
  p_pos->y = pos_data.y;
  
  // X�����͈͓��H
  if( (FX32_CONST(cp_area->rect.x_min) > pos_data.x) )
  {
    ret = TRUE;
    p_pos->x = FX32_CONST(cp_area->rect.x_min);
  }
  else if( (cp_area->rect.x_max<<FX32_SHIFT < pos_data.x) )
  {
    ret = TRUE;
    p_pos->x = FX32_CONST(cp_area->rect.x_max);
  }


  // Z�����͈͓��H
  if( (cp_area->rect.z_min<<FX32_SHIFT > pos_data.z) )
  {
    ret = TRUE;
    p_pos->z = FX32_CONST(cp_area->rect.z_min);
  }
  else if( (cp_area->rect.z_max<<FX32_SHIFT < pos_data.z) )
  {
    ret = TRUE;
    p_pos->z = FX32_CONST(cp_area->rect.z_max);
  }


  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�������͈͌v�Z�@�~�͈͊O�@�X�g�b�v
 *
 *	@param	cp_area   �~���
 *	@param	cp_pos    ���ݍ��W
 *	@param	p_pos     �v�Z����W
 *
 *	@retval TRUE  �J��������͈͊O������
 *	@retval FALSE �J��������͈͓�������
 */
//-----------------------------------------------------------------------------
static BOOL cameraArea_IsAreaCircle( const FIELD_CAMERA_AREA* cp_area, const VecFx32* cp_pos, VecFx32* p_pos )
{
  VecFx32 def_vec = { 0,0,FX32_ONE };
  VecFx32 pos_data = *cp_pos;
  VecFx32 pos_save = *cp_pos;
  VecFx32 center;
  VecFx32 way;
  fx32  len;
  u16 min_rot, max_rot, rot_now;
  VecFx32 normal; // ��]�������o�p
  BOOL ret = FALSE;

  p_pos->x = cp_pos->x;
  p_pos->y = cp_pos->y;
  p_pos->z = cp_pos->z;

  // ���S���W�����߂�
  center.x = cp_area->circle.center_x;
  center.y = 0;
  center.z = cp_area->circle.center_z;
  

  // �^�[�Q�b�g�𒆐S�Ƃ��������ɕύX
  pos_data.y = 0; // ���ʂŌ���
  VEC_Subtract( &pos_data, &center, &way ); 

  // ���������߂�
  len = VEC_Mag( &way );

  // �����̌덷�����
  // �؂�グ�B
  if( FX_Modf( len, &len ) > 0 ){
    len += FX32_ONE;
  }

  VEC_Normalize( &way, &way );

  // start end�Ƃ̊O�ς����߂�
  VEC_CrossProduct( &def_vec, &way, &normal );
  rot_now     = FX_AcosIdx( VEC_DotProduct( &def_vec, &way ) );

  if( normal.y < 0 )
  {
    rot_now = 0x10000 - rot_now;
  }

  // �����͈̔͊O�`�F�b�N
  if( len > cp_area->circle.r )
  {
    len = cp_area->circle.r;
    ret = TRUE;
  }

  // �p�x�͈̔͊O�`�F�b�N
  if( cp_area->circle.min_rot != cp_area->circle.max_rot )
  {
    if( cp_area->circle.min_rot > cp_area->circle.max_rot )
    {
      // 360�x��ʉ߂�������@���͈�
      min_rot = cp_area->circle.max_rot;
      max_rot = cp_area->circle.min_rot;

      //  �`�F�b�N
      if( (min_rot < rot_now) && (max_rot >= rot_now) )
      {
        // �߂��ق�
        if( MATH_ABS(min_rot - rot_now) > MATH_ABS(max_rot - rot_now) )
        {
          rot_now = max_rot;
          ret = TRUE;
        }
        else
        {
          rot_now = min_rot;
          ret = TRUE;
        }
      }
    }
    else
    {
      // �ʏ�́@���͈�
      min_rot = cp_area->circle.min_rot;
      max_rot = cp_area->circle.max_rot;

      //  �`�F�b�N
      if( min_rot > rot_now )
      {
        rot_now = min_rot;
        ret = TRUE;
      }
      else if( max_rot < rot_now )
      {
        rot_now = max_rot;
        ret = TRUE;
      }
    }
  }


//  TOMOYA_Printf( "rot_now=%d  min_rot=%d max_rot=%d\n", rot_now, min_rot, max_rot );


  // ���W�̍Čv�Z
  if( ret )
  {
    p_pos->x = FX_Mul( FX_SinIdx( rot_now ), len ) + center.x;
    p_pos->z = FX_Mul( FX_CosIdx( rot_now ), len ) + center.z;
    p_pos->y = pos_save.y;
  }

  return ret;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �J�������͈͌v�Z�@��`�@�͈͓��@�X�g�b�v
 *
 *	@param	cp_area   ��`���
 *	@param	cp_pos    ���ݍ��W
 *	@param	p_pos     �v�Z����W
 *
 *	@retval TRUE  �J��������͈͊O������
 *	@retval FALSE �J��������͈͓�������
 */
//-----------------------------------------------------------------------------
static BOOL cameraArea_IsAreaRectIn( const FIELD_CAMERA_AREA* cp_area, const VecFx32* cp_pos, VecFx32* p_pos )
{
  VecFx32 pos_data = *cp_pos;
  BOOL ret = FALSE;
  fx32 dist_min;
  u32 area_type;
  enum
  {
    AREA_X_MIN,
    AREA_X_MAX,
    AREA_Z_MIN,
    AREA_Z_MAX,

    AREA_NUM,
  };

  // Y���W�ݒ�
  p_pos->x = pos_data.x;
  p_pos->z = pos_data.z;
  p_pos->y = pos_data.y;
  
  // X�����͈͓��H
  if( (FX32_CONST(cp_area->rect.x_min) < pos_data.x) && (FX32_CONST(cp_area->rect.x_max) > pos_data.x) )
  {
    if( (FX32_CONST(cp_area->rect.z_min) < pos_data.z) && (FX32_CONST(cp_area->rect.z_max) > pos_data.z) )
    {
      ret = TRUE;

      // XorZ��ԋ߂��Ƃ���ɖ߂�
      dist_min  = p_pos->x - FX32_CONST(cp_area->rect.x_min);
      area_type = AREA_X_MIN;

      if( dist_min > FX32_CONST(cp_area->rect.x_max) - p_pos->x )
      {
        dist_min  = FX32_CONST(cp_area->rect.x_max) - p_pos->x;
        area_type = AREA_X_MAX;
      }

      if( dist_min > p_pos->z - FX32_CONST(cp_area->rect.z_min) )
      {
        dist_min  = p_pos->z - FX32_CONST(cp_area->rect.z_min);
        area_type = AREA_Z_MIN;
      }

      if( dist_min > FX32_CONST(cp_area->rect.z_max) - p_pos->z )
      {
        dist_min  = FX32_CONST(cp_area->rect.z_max) - p_pos->z;
        area_type = AREA_Z_MAX;
      }


      // ��ԋ߂��Ƃ���̍��W��ݒ�
      switch( area_type )
      {
      case AREA_X_MIN:
        p_pos->x = FX32_CONST(cp_area->rect.x_min);
        break;
      case AREA_X_MAX:
        p_pos->x = FX32_CONST(cp_area->rect.x_max);
        break;
      case AREA_Z_MIN:
        p_pos->z = FX32_CONST(cp_area->rect.z_min);
        break;
      case AREA_Z_MAX:
        p_pos->z = FX32_CONST(cp_area->rect.z_max);
        break;
      default:
        GF_ASSERT(0);
        break;
      }
    }
  }

  return ret;
}


/**
 *
 * ���`��Ԃ��^�J�����ړ��֘A����
 *
*/

//�J�������`�ړ���ԊJ�n�̂��߂̏���������
//�E�����܂Ńv�b�V���ł���悤�ɂ��邩

//�J�����f�[�^�v�b�V������
//�E�ۑ��������
//�@���[�h�A�^�[�Q�b�g���W�A�A���O���A�J�������W
//�E���[�h�ύX
//�E�ړ�
//�E�ۑ����Ă��郂�[�h�ɖ߂�
//�E���A���[�h�ʂɁA�ۑ����Ă���f�[�^���Z�b�g
//�E�I��
//
//�J�����ړ�����
//�E���݂̃J�������W�A�A���O���A����p���������ɃZ�b�g
//�E�ړI�̃J�������W�A�A���O���A����p���������ɃZ�b�g
//�E�ړ�����

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����p�����[�^�Z�b�g
 *
 *	@param	inCamera      �J�����|�C���^
 *	@param	outParam      �p�����[�^�i�[�o�b�t�@
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void SetNowCameraParam(const FIELD_CAMERA * inCamera, FLD_CAM_MV_PARAM_CORE *outParam)
{
  outParam->CamPos = inCamera->camPos;
  outParam->TrgtPos = inCamera->target;
  outParam->AnglePitch = inCamera->angle_pitch;
  outParam->AngleYaw = inCamera->angle_yaw;
  outParam->Distance = inCamera->angle_len;
  outParam->Fovy = inCamera->fovy;
  outParam->Shift = inCamera->target_offset;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����p�����[�^�Z�b�g�J�����G���A�I�[�o�[��
 *
 *	@param	inCamera      �J�����|�C���^
 *	@param	outParam      �p�����[�^�i�[�o�b�t�@
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void SetNowCameraParamAreaOv(const FIELD_CAMERA * inCamera, FLD_CAM_MV_PARAM_CORE *outParam)
{
  outParam->CamPos = inCamera->camPos;
  FIELD_CAMERA_GetCameraAreaAfterTargetPos(inCamera, &outParam->TrgtPos);
  outParam->AnglePitch = inCamera->angle_pitch;
  outParam->AngleYaw = inCamera->angle_yaw;
  outParam->Distance = inCamera->angle_len;
  outParam->Fovy = inCamera->fovy;
  outParam->Shift = inCamera->target_offset;
}


#if 0
static void RecvModeCamera(FIELD_CAMERA * camera)
{
  FLD_MOVE_CAM_DATA *data;
  FLD_CAM_PUSH_PARAM *push_param;
  data = &camera->MoveData;
  push_param = &data->PushParam;
  //�^�[�Q�b�g�ʒu
  ;
  //�A���O��
  ;
  //����
  ;
  //����p
  ;
}
static void RecvModeTarget(FIELD_CAMERA * camera)
{
  GF_ASSERT(0);
  //�J�����ʒu
  ;
  //�A���O��
  ;
  //����
  ;
  //����p
  ;
}
static void RecvModeDirect(FIELD_CAMERA * camera)
{
  //�^�[�Q�b�g�ʒu
  ;
  //�J�����ʒu
  ;
}
#endif


//----------------------------------------------------------------------------
/**
 *	@brief�@���A���̕ۑ�
 *
 *	@param	camera      �J�����|�C���^
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetRecvCamParam(FIELD_CAMERA * camera )
{
  FLD_MOVE_CAM_DATA *data;
  FLD_CAM_PUSH_PARAM *push_param;
  if (camera == NULL){
    GF_ASSERT( 0 );
    return;
  }

  data = &camera->MoveData;
  push_param = &data->PushParam;

  if (data->PushFlg){
    GF_ASSERT_MSG(0,"Already Exist PushData");
    return;
  }

  //�����v�b�V��
  push_param->BeforeMode = camera->mode;
  //��Ԉړ��̓J�����ʒu�s��^�݂̂̃T�|�[�g
  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
  
  SetNowCameraParam(camera, &push_param->RecvParam);
#if 0  
#ifdef DEBUG_ONLY_FOR_saitou
  OS_Printf("--DUMP_RECV_FIELD_CAMERA_PARAM--\n");
  DumpCameraParam(camera);
#endif
#endif
  //�v�b�V�������t���O
  data->PushFlg = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief�@���A���̕ۑ�
 *	@note   ���A�ꏊ���J�����G���A�I�[�o�[�̂Ƃ��Ɏg�p
 *
 *	@param	camera      �J�����|�C���^
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetRecvCamParamOv(FIELD_CAMERA * camera )
{
  FLD_MOVE_CAM_DATA *data;
  FLD_CAM_PUSH_PARAM *push_param;
  if (camera == NULL){
    GF_ASSERT( 0 );
    return;
  }

  data = &camera->MoveData;
  push_param = &data->PushParam;

  if (data->PushFlg){
    GF_ASSERT_MSG(0,"Already Exist PushData");
    return;
  }

  //�����v�b�V��
  push_param->BeforeMode = camera->mode;
  //��Ԉړ��̓J�����ʒu�s��^�݂̂̃T�|�[�g
  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );
  
  SetNowCameraParamAreaOv(camera, &push_param->RecvParam);

  //�v�b�V�������t���O
  data->PushFlg = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���A���̃N���A
 *
 *	@param	camera      �J�����|�C���^
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_ClearRecvCamParam(FIELD_CAMERA * camera)
{
  FLD_MOVE_CAM_DATA *data;
  FLD_CAM_PUSH_PARAM *push_param;

  if (camera == NULL){
    GF_ASSERT( 0 );
    return;
  }

  data = &camera->MoveData;
  if (!data->PushFlg){
    OS_Printf("No PushData\n");
    return;
  }
  
  push_param = &data->PushParam;
  //���[�h�𕜋A
  FIELD_CAMERA_ChangeMode( camera, push_param->BeforeMode );

  //�v�b�V���t���O�𗎂�
  data->PushFlg = FALSE;

}

//----------------------------------------------------------------------------
/**
 *	@brief  ���`�ړ��p�����[�^���Z�b�g���ē���J�n
 *
 *	@param	camera      �J�����|�C���^
 *	@param	param       �p�����[�^�i�[�o�b�t�@
 *	@param  inFrame     �t���[��
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void SetLinerParamCommon(
    FIELD_CAMERA * camera, const FLD_CAM_MV_PARAM *param, const u16 inFrame)
{
  FLD_MOVE_CAM_DATA *data;
  if (camera == NULL){
    GF_ASSERT( 0 );
    return;
  }

  data = &camera->MoveData;

  //��Ԉړ��̓J�����ʒu�s��^�݂̂̃T�|�[�g
  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

  data->DstParam = param->Core;
  data->NowFrm = 0;
  data->CostFrm = inFrame;
  data->Chk = param->Chk;

  //�R�[���o�b�N���Z�b�g
  camera->CallBack = LinerMoveFunc;
  data->CallBackWorkPtr = NULL;
  //���`��Ԉړ����J�n
  data->Valid = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���`�ړ��p�����[�^���Z�b�g���ē���J�n
 *
 *	@param	camera      �J�����|�C���^
 *	@param	param       �p�����[�^�i�[�o�b�t�@
 *	@param  inFrame     �t���[��
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetLinerParam(FIELD_CAMERA * camera, const FLD_CAM_MV_PARAM *param, const u16 inFrame)
{
  FLD_MOVE_CAM_DATA *data;
  if (camera == NULL){
    GF_ASSERT( 0 );
    return;
  }
  data = &camera->MoveData;
  SetLinerParamCommon( camera, param, inFrame);
  SetNowCameraParam(camera, &data->SrcParam);
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���`�ړ��p�����[�^���Z�b�g���ē���J�n
 *	@note �J�n�n�_���J�����G���A�Ɉ����������Ă���ꍇ�Ɏg�p
 *
 *	@param	camera      �J�����|�C���^
 *	@param	param       �p�����[�^�i�[�o�b�t�@
 *	@param  inFrame     �t���[��
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetLinerParamOv(FIELD_CAMERA * camera, const FLD_CAM_MV_PARAM *param, const u16 inFrame)
{
  FLD_MOVE_CAM_DATA *data;
  if (camera == NULL){
    GF_ASSERT( 0 );
    return;
  }
  data = &camera->MoveData;
  SetLinerParamCommon( camera, param, inFrame);
  SetNowCameraParamAreaOv(camera, &data->SrcParam);
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���`�ړ��p�����[�^���Z�b�g���ē���J�n(�_�C���N�g�^)
 *
 *	@param	camera      �J�����|�C���^
 *	@param	inCamPos    �J�����ʒu
 *	@param  inTrgtPos   �^�[�Q�b�g�ʒu
 *	@param  inFrame     �t���[��
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetLinerParamDirect(
    FIELD_CAMERA * camera,
    const VecFx32 *inCamPos, const VecFx32 *inTrgtPos,
    const FLD_CAM_MV_PARAM_CHK *inChk,
    const u16 inFrame)
{
  FLD_MOVE_CAM_DATA *data;
  
  if (camera == NULL){
    GF_ASSERT( 0 );
    return;
  }

  data = &camera->MoveData;

  //��Ԉړ��̓_�C���N�g�^�݂̂̃T�|�[�g
  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_DIRECT_POS );

  SetNowCameraParam(camera, &data->SrcParam);
  data->DstParam.CamPos = *inCamPos;
  data->DstParam.TrgtPos = *inTrgtPos;
  data->NowFrm = 0;
  data->CostFrm = inFrame;
  {
    FLD_CAM_MV_PARAM_CHK chk;
    //�_�C���N�g�����Ȃ̂ŁA�ʒu�Ǝ���p�ƒ����_�I�t�Z�b�g�ȊO�͕ω������Ȃ�
    chk.Pos = inChk->Pos;
    chk.Fovy = inChk->Fovy;
    chk.Shift = inChk->Shift;
    chk.Pitch = FALSE;
    chk.Yaw = FALSE;
    chk.Dist = FALSE;

    data->Chk = chk;
  }

  //�R�[���o�b�N���Z�b�g
  camera->CallBack = DirectLinerMoveFunc;
  data->CallBackWorkPtr = NULL;
  //���`��Ԉړ����J�n
  data->Valid = TRUE;

}

//----------------------------------------------------------------------------
/**
 *	@brief  ���J�o���[�֐�
 *
 *	@param	camera      �J�����|�C���^
 *	@param  chk         ���A�p�����[�^
 *	@param  inFrame     �t���[��
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_RecvLinerParam(
    FIELD_CAMERA * camera, const FLD_CAM_MV_PARAM_CHK *chk, const u16 inFrame)
{
  FLD_MOVE_CAM_DATA *data;
  if (camera == NULL){
    GF_ASSERT( 0 );
    return;
  }

  data = &camera->MoveData;
  if (!data->PushFlg){
    GF_ASSERT_MSG(0,"NO RECOVER PARAM\n");
    return;
  }

  //�t���[���w�肪0�̂Ƃ��͕s���Ƃ݂Ȃ��A�������Ȃ�
  if (inFrame == 0){
    GF_ASSERT(0);
    return;
  }

  //��Ԉړ��̓J�����ʒu�s��^�݂̂̃T�|�[�g
  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

  SetNowCameraParam(camera, &data->SrcParam);
  data->DstParam = data->PushParam.RecvParam;

  data->NowFrm = 0;
  data->CostFrm = inFrame;

  data->Chk = *chk;

  //�R�[���o�b�N���Z�b�g
  camera->CallBack = LinerMoveFunc;
  data->CallBackWorkPtr = NULL;
  //���`��Ԉړ����J�n
  data->Valid = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �f�t�H���g�]�[���J�����ݒ�ŕ��A���郊�J�o���[�֐�
 *
 *	@param	camera      �J�����|�C���^
 *	@param  inFrame     �t���[��
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_RecvLinerParamDefault(
    FIELD_CAMERA * camera, const u16 inFrame)
{
  FLD_MOVE_CAM_DATA *data;
  if (camera == NULL){
    GF_ASSERT( 0 );
    return;
  }

  data = &camera->MoveData;
  if (!data->PushFlg){
    GF_ASSERT_MSG(0,"NO RECOVER PARAM\n");
    return;
  }

  //�t���[���w�肪0�̂Ƃ��͕s���Ƃ݂Ȃ��A�������Ȃ�
  if (inFrame == 0){
    GF_ASSERT(0);
    return;
  }

  //��Ԉړ��̓J�����ʒu�s��^�݂̂̃T�|�[�g
  FIELD_CAMERA_ChangeMode( camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

  SetNowCameraParam(camera, &data->SrcParam);
  //�]�[���f�t�H���g�J�����p�����[�^���擾����
  {
    FLD_CAMERA_PARAM param;
    loadInitialParameter( camera, &param );
    data->DstParam.AnglePitch = param.Angle.x;
    data->DstParam.AngleYaw = param.Angle.y;
    data->DstParam.Distance = param.Distance * FX32_ONE;
    data->DstParam.Fovy = param.PerspWay;
    // �v���C���[�Ƃ̃o�C���h�`�F�b�N
    if( param.PlayerLink )
    {
      // �o�C���h����ꍇ
      data->DstParam.Shift = param.Shift;
      data->DstParam.TrgtPos = *camera->default_target;
    }
    else
    {
      // �o�C���h���Ȃ��ꍇ
      VEC_Set( &data->DstParam.Shift, 0,0,0 );
      data->DstParam.TrgtPos = param.Shift;
    }
  }
  data->NowFrm = 0;
  data->CostFrm = inFrame;

  data->Chk.Shift = TRUE;
  data->Chk.Pitch = TRUE;
  data->Chk.Yaw = TRUE;
  data->Chk.Dist = TRUE;
  data->Chk.Fovy = TRUE;
  data->Chk.Pos = TRUE;

  {
    OS_Printf("pitch = %x\n",data->PushParam.RecvParam.AnglePitch);
    OS_Printf("yaw = %x\n",data->PushParam.RecvParam.AngleYaw);
    OS_Printf("dis = %x\n",data->PushParam.RecvParam.Distance);
    OS_Printf("pers = %x\n",data->PushParam.RecvParam.Fovy);
    OS_Printf("pos = %x, %x, %x\n",
        data->PushParam.RecvParam.TrgtPos.x,
        data->PushParam.RecvParam.TrgtPos.y,
        data->PushParam.RecvParam.TrgtPos.z);
    OS_Printf("shift = %x, %x, %x\n",
        data->PushParam.RecvParam.Shift.x,
        data->PushParam.RecvParam.Shift.y,
        data->PushParam.RecvParam.Shift.z);

    OS_Printf("_pitch = %x\n",data->DstParam.AnglePitch);
    OS_Printf("_yaw = %x\n",data->DstParam.AngleYaw);
    OS_Printf("_dis = %x\n",data->DstParam.Distance);
    OS_Printf("_pers = %x\n",data->DstParam.Fovy);
    OS_Printf("_pos = %x, %x, %x\n",
        data->DstParam.TrgtPos.x,
        data->DstParam.TrgtPos.y,
        data->DstParam.TrgtPos.z);
    OS_Printf("_shift = %x, %x, %x\n",
        data->DstParam.Shift.x,
        data->DstParam.Shift.y,
        data->DstParam.Shift.z);
  }

  //�R�[���o�b�N���Z�b�g
  camera->CallBack = LinerMoveFunc;
  data->CallBackWorkPtr = NULL;
  //���`��Ԉړ����J�n
  data->Valid = TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���`�K�؈ړ��֐�
 *
 *	@param	camera      �J�����|�C���^
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void LinerMoveFunc(FIELD_CAMERA * camera, void *work)
{
  FLD_MOVE_CAM_DATA *data;
  FLD_CAM_MV_PARAM_CORE *src;
  FLD_CAM_MV_PARAM_CORE *dst;
  data = &camera->MoveData;
  
  //���N�G�X�g���Ȃ���Ώ����̓X���[����
  if (!data->Valid){
    return;
  }
  //�t���[������
  data->NowFrm++;

  src = &data->SrcParam;
  dst = &data->DstParam;
  //�A���O���s�b�`
  if (data->Chk.Pitch)
  {
///    OS_Printf("PITCH\n");
    camera->angle_pitch = SubFuncU16(&src->AnglePitch, &dst->AnglePitch, data->CostFrm, data->NowFrm);
  }
  //�A���O�����[
  if (data->Chk.Yaw)
  {
///    OS_Printf("YAW\n");
    camera->angle_yaw = SubFuncU16(&src->AngleYaw, &dst->AngleYaw, data->CostFrm, data->NowFrm);
  }
  //���W
  if (data->Chk.Pos)
  {
///    OS_Printf("POS\n");
    VecSubFunc(&src->TrgtPos, &dst->TrgtPos, data->CostFrm, data->NowFrm, &camera->target);
  }
  //����p
  if (data->Chk.Fovy)
  {
    camera->fovy = SubFuncU16(&src->Fovy, &dst->Fovy, data->CostFrm, data->NowFrm);
    GFL_G3D_CAMERA_SetfovySin( camera->g3Dcamera, FX_SinIdx( camera->fovy ) );
    GFL_G3D_CAMERA_SetfovyCos( camera->g3Dcamera, FX_CosIdx( camera->fovy ) );
  }
  //����
  if (data->Chk.Dist)
  {
///    OS_Printf("DIST\n");
    camera->angle_len = SubFuncFx(&src->Distance, &dst->Distance, data->CostFrm, data->NowFrm);

  }
  //�I�t�Z�b�g�i�V�t�g�j
  if (data->Chk.Shift)
  {
///    OS_Printf("SHIFT\n");
    VecSubFunc(&src->Shift, &dst->Shift, data->CostFrm, data->NowFrm, &camera->target_offset);
  }
  //�t���[��������t���[���ɓ��B�������H
  if(data->NowFrm >= data->CostFrm){
    //�ړ����s���t���O�I�t
    data->Valid = FALSE;
    //�R�[���o�b�N�N���A
    FIELD_CAMERA_ClearMvFuncCallBack(camera);
#ifdef DEBUG_ONLY_FOR_saitou
    OS_Printf("--DUMP_END_FIELD_CAMERA_PARAM--\n");
    DumpCameraParam(camera);
#endif
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���`�K�؈ړ��֐�
 *
 *	@param	camera      �J�����|�C���^
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void DirectLinerMoveFunc(FIELD_CAMERA * camera, void *work)
{
  FLD_MOVE_CAM_DATA *data;
  FLD_CAM_MV_PARAM_CORE *src;
  FLD_CAM_MV_PARAM_CORE *dst;
  data = &camera->MoveData;
  
  //���N�G�X�g���Ȃ���Ώ����̓X���[����
  if (!data->Valid){
    return;
  }
  //�t���[������
  data->NowFrm++;

  src = &data->SrcParam;
  dst = &data->DstParam;
  //���W
  if (data->Chk.Pos)
  {
///    OS_Printf("POS\n");
    VecSubFunc(&src->TrgtPos, &dst->TrgtPos, data->CostFrm, data->NowFrm, &camera->target);
    VecSubFunc(&src->CamPos, &dst->CamPos, data->CostFrm, data->NowFrm, &camera->camPos);
  }
  //����p
  if (data->Chk.Fovy)
  {
    camera->fovy = SubFuncU16(&src->Fovy, &dst->Fovy, data->CostFrm, data->NowFrm);
    GFL_G3D_CAMERA_SetfovySin( camera->g3Dcamera, FX_SinIdx( camera->fovy ) );
    GFL_G3D_CAMERA_SetfovyCos( camera->g3Dcamera, FX_CosIdx( camera->fovy ) );
  }
  //�I�t�Z�b�g�i�V�t�g�j
  if (data->Chk.Shift)
  {
///    OS_Printf("SHIFT\n");
    VecSubFunc(&src->Shift, &dst->Shift, data->CostFrm, data->NowFrm, &camera->target_offset);
  }
  //�t���[��������t���[���ɓ��B�������H
  if(data->NowFrm >= data->CostFrm){
    //�ړ����s���t���O�I�t
    data->Valid = FALSE;
    //�R�[���o�b�N�N���A
    FIELD_CAMERA_ClearMvFuncCallBack(camera);
#ifdef DEBUG_ONLY_FOR_saitou
    OS_Printf("--DUMP_END_FIELD_CAMERA_PARAM--\n");
    DumpCameraParam(camera);
#endif
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �x�N�g�������v�Z�֐�
 *
 *	@param	inSrc         �J�n�x�N�g��
 *	@param  inDst         �I���x�N�g��
 *	@param  inCostFrm     ����t���[��
 *	@param  inNowFrm      ���݃t���[��
 *	@param  outVec        �i�[�o�b�t�@
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void VecSubFunc(
    const VecFx32 *inSrc, const VecFx32 *inDst, const u16 inCostFrm, const u16 inNowFrm, VecFx32 *outVec)
{
  if (!inCostFrm){
    GF_ASSERT(0);
    *outVec = *inDst;
  }
  outVec->x = SubFuncFx(&inSrc->x, &inDst->x, inCostFrm, inNowFrm);
  outVec->y = SubFuncFx(&inSrc->y, &inDst->y, inCostFrm, inNowFrm);
  outVec->z = SubFuncFx(&inSrc->z, &inDst->z, inCostFrm, inNowFrm);
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Œ菬���^�����v�Z�֐�
 *
 *	@param	inSrc         �J�n�l
 *	@param  inDst         �I���l
 *	@param  inCostFrm     ����t���[��
 *	@param  inNowFrm      ���݃t���[��
 *
 *	@retval fx32          �v�Z����
 */
//-----------------------------------------------------------------------------
static fx32 SubFuncFx(const fx32 *inSrc, const fx32 *inDst, const u16 inCostFrm, const u16 inNowFrm)
{
  long src = (long)*inSrc;
  long dst = (long)*inDst;
  long dif;
  long tmp;
  long val;
  fx32 ans;

  if ( inCostFrm == inNowFrm ) ans = dst;
  else
  {
    dif = dst - src;
    tmp = (dif*inNowFrm);
    val = tmp / inCostFrm;
    ans = src+val;
  }
#if 0
  fx32 dif, val, ans;
  fx32 tmp;

  if (!inCostFrm){
    GF_ASSERT(0);
    return *inDst;
  }
  dif = (*inDst) - (*inSrc);
  tmp = (dif*inNowFrm);
  val = tmp / inCostFrm;
  ans = (*inSrc)+val;
#endif  
  return ans;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����Ȃ����������v�Z�֐�
 *
 *	@param	inSrc         �J�n�l
 *	@param  inDst         �I���l
 *	@param  inCostFrm     ����t���[��
 *	@param  inNowFrm      ���݃t���[��
 *
 *	@retval u16           �v�Z����
 */
//-----------------------------------------------------------------------------
static u16 SubFuncU16(const u16 *inSrc, const u16 *inDst, const u16 inCostFrm, const u16 inNowFrm)
{
  u16 dif, val;
  u16 ans;
  u16 samp_a, samp_b;

  if (!inCostFrm){
    GF_ASSERT(0);
    return *inDst;
  }
  samp_a = (*inDst) - (*inSrc);
  samp_b = 0x10000-samp_a;

  if (samp_a<= samp_b){
    dif = samp_a;
    val = (dif*inNowFrm)/inCostFrm;
    ans = (*inSrc)+val;
  }else{
    dif = samp_b;
    val = (dif*inNowFrm)/inCostFrm;
    ans = (*inSrc)-val;
  }
  return ans;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����ړ��R�[���o�b�N�֐��o�^
 *
 *	@param	camera      �J�����|�C���^
 *	@param	param       �p�����[�^�i�[�o�b�t�@
 *	@param  inFrame     �t���[��
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_SetMvFuncCallBack(FIELD_CAMERA * camera, CAMERA_CALL_BACK func, void *work)
{
  GF_ASSERT(camera->CallBack == NULL);
  camera->CallBack = func;
  camera->MoveData.CallBackWorkPtr = work;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����ړ��R�[���o�b�N�֐��N���A
 *
 *	@param	camera      �J�����|�C���^
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_ClearMvFuncCallBack(FIELD_CAMERA * camera)
{
  camera->CallBack = NULL;
  camera->MoveData.CallBackWorkPtr = NULL;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����ړ��R�[���o�b�N�������`�F�b�N����
 *
 *	@param	camera      �J�����|�C���^
 *
 *	@retval BOOL      TRUE�ŃR�[���o�b�N�֐��N����
 */
//-----------------------------------------------------------------------------
BOOL FIELD_CAMERA_CheckMvFunc(FIELD_CAMERA * camera)
{
  FLD_MOVE_CAM_DATA *data;
  data = &camera->MoveData;
  if (camera->CallBack != NULL)
  {
    if (data->Valid){
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �J�����ړ��ړI�p�����[�^�|�C���^���擾
 *
 *	@param	camera      �J�����|�C���^
 *
 *	@retval FLD_CAM_MV_PARAM_CORE �p�����[�^�|�C���^
 */
//-----------------------------------------------------------------------------
FLD_CAM_MV_PARAM_CORE *FIELD_CAMERA_GetMoveDstPrmPtr(FIELD_CAMERA * camera)
{
  return &camera->MoveData.DstParam;
}

#ifdef PM_DEBUG
//----------------------------------------------------------------------------
/**
 *	@brief  �J�������̃_���v
 *
 *	@param  camera        �J�����|�C���^
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
static void DumpCameraParam(FIELD_CAMERA * camera)
{
  u16 pitch, yaw;
  fx32 len;
  u16 fovy;
  VecFx32 shift, target;
  pitch = FIELD_CAMERA_GetAnglePitch(camera);
  yaw = FIELD_CAMERA_GetAngleYaw(camera);
  len = FIELD_CAMERA_GetAngleLen(camera);
  fovy = FIELD_CAMERA_GetFovy(camera);
  FIELD_CAMERA_GetTargetOffset( camera, &shift );
  FIELD_CAMERA_GetTargetPos( camera, &target );
      
  OS_Printf("%d,%d,%d,%d,%d,%d\n", pitch, yaw, len, target.x, target.y, target.z );
  OS_Printf("%d,%d,%d,%d\n", fovy, shift.x, shift.y, shift.z );
}


//------------------------------------------------------------------
//  WIPE��\������Ђ悤�����邩�H
//------------------------------------------------------------------
BOOL FIELD_CAMERA_DEBUG_IsWipeDraw( const FIELD_CAMERA* camera)
{
  if( camera->p_debug_wipe ){
    return TRUE;
  }
  return FALSE;
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

//----------------------------------------------------------------------------
/**
 *	@brief	�f�t�H���g�p�����[�^�̎擾
 */
//----------------------------------------------------------------------------- 
void FIELD_CAMERA_GetInitialParameter( const FIELD_CAMERA* camera, FLD_CAMERA_PARAM * result)
{
  // �p�����[�^�ǂݍ���
  loadInitialParameter( camera, result );
}




//----------------------------------------------------------------------------
/**
 *	@brief  ���`�J�����f�o�b�N�o�͗p�̃p�����[�^��ݒ�
 */
//-----------------------------------------------------------------------------
void FIELD_CAMERA_DEBUG_SetUpLinerCameraInfoData( FIELD_CAMERA* camera )
{
  VecFx32 set_target;
  VecFx32 move_camera;

  // �^�[�Q�b�g
  set_target = camera->target_offset;
  VEC_Add( &set_target, &camera->target, &set_target );
  VEC_Subtract( &set_target, &camera->debug_default_target_offs, &set_target );
  camera->target = set_target;

  // �I�t�Z�b�g
  camera->target_offset = camera->debug_default_target_offs;

  // �A���O��
  VEC_Subtract( &camera->camPos, &camera->target, &move_camera );
  FIELD_CAMERA_LOCAL_modeChange_CalcVecAngel( &move_camera, &camera->angle_pitch, &camera->angle_yaw, &camera->angle_len );
}

#endif  //PM_DEBUG






