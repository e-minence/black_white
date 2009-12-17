//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fld_scenearea_loader.c
 *	@brief  �t�B�[���h�@����V�[���̈�O���f�[�^�ǂݍ���
 *	@author	tomoya takahashi
 *	@date		2009.07.09
 *
 *	���W���[�����FFLD_SCENEAREA_LOADER
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include "gflib.h"

#include "arc_def.h"


#include "field/field_const.h"
#include "fldmmdl.h"

#include "fld_scenearea_loader_func.h"
#include "fld_scenearea_loader.h"


//-----------------------------------------------------------------------------
/**
 *					�萔�錾
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					�\���̐錾
*/
//-----------------------------------------------------------------------------
//-------------------------------------
///	�ǂݍ��݃V�X�e��
//=====================================
struct _FLD_SCENEAREA_LOADER {
	void* p_data;
	u32 num;
};

//-----------------------------------------------------------------------------
//	�e��R�[���o�b�N�֐�
//-----------------------------------------------------------------------------
static BOOL C3_SCENEAREA_CheckArea( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_Update( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void C3_SCENEAREA_FxCamera( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );


static BOOL SCENEAREA_CheckGridRect( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );
static void SCENEAREA_UpdateGridAngleChange( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos );


static FLD_SCENEAREA_CHECK_AREA_FUNC* sp_FLD_SCENEAREA_CHECK_AREA_FUNC[FLD_SCENEAREA_AREACHECK_MAX] = 
{
	C3_SCENEAREA_CheckArea,

  SCENEAREA_CheckGridRect,
};

static FLD_SCENEAREA_UPDATA_FUNC* sp_FLD_SCENEAREA_UPDATA_FUNC[FLD_SCENEAREA_UPDATE_MAX] = 
{
	C3_SCENEAREA_Update,
	C3_SCENEAREA_FxCamera,

  SCENEAREA_UpdateGridAngleChange,
};

static const FLD_SCENEAREA_FUNC sc_FLD_SCENEAREA_FUNC = 
{
	sp_FLD_SCENEAREA_CHECK_AREA_FUNC,
	sp_FLD_SCENEAREA_UPDATA_FUNC,
	FLD_SCENEAREA_AREACHECK_MAX,
	FLD_SCENEAREA_UPDATE_MAX,
};

//-----------------------------------------------------------------------------
/**
 *					�v���g�^�C�v�錾
*/
//-----------------------------------------------------------------------------




//----------------------------------------------------------------------------
/**
 *	@brief	�V�[���G���A�f�[�^���[�_�[�@����
 *
 *	@param	heapID	�q�[�vID
 *
 *	@return	���[�_�[
 */
//-----------------------------------------------------------------------------
FLD_SCENEAREA_LOADER* FLD_SCENEAREA_LOADER_Create( HEAPID heapID )
{
	FLD_SCENEAREA_LOADER* p_sys;

	p_sys = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLD_SCENEAREA_LOADER) );
	return p_sys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�[���G���A���[�_�[	�j��
 *
 *	@param	p_sys		�V�X�e��
 */
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_LOADER_Delete( FLD_SCENEAREA_LOADER* p_sys )
{
	FLD_SCENEAREA_LOADER_Clear( p_sys );
	GFL_HEAP_FreeMemory( p_sys );
}


//----------------------------------------------------------------------------
/**
 *	@brief	�V�[���G���A���[�_�[�@�ǂݍ���
 *
 *	@param	p_sys			�V�X�e��
 *	@param	datano		�f�[�^�i���o�[
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_LOADER_Load( FLD_SCENEAREA_LOADER* p_sys, u32 datano, HEAPID heapID )
{
  FLD_SCENEAREA_LOADER_LoadOriginal( p_sys, ARCID_SCENEAREA_DATA, datano, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ARCID���w�肷��o�[�W����
 *
 *	@param	p_sys			�V�X�e��
 *	@param	arcID     �A�[�J�C�uID
 *	@param	datano		�f�[�^�i���o�[
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_LOADER_LoadOriginal( FLD_SCENEAREA_LOADER* p_sys, u32 arcID, u32 datano, HEAPID heapID )
{
	u32 size;
	
	GF_ASSERT( p_sys );

	p_sys->p_data = GFL_ARC_UTIL_LoadEx( arcID, datano, FALSE, heapID, &size );
	p_sys->num		= size / sizeof(FLD_SCENEAREA_DATA);

	TOMOYA_Printf( "SCENEAREA_size = %d\n", size );
	TOMOYA_Printf( "SCENEAREA num = %d\n",  p_sys->num );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�N���A����
 *
 *	@param	p_sys		�V�X�e��
 */
//-----------------------------------------------------------------------------
void FLD_SCENEAREA_LOADER_Clear( FLD_SCENEAREA_LOADER* p_sys )
{
	GF_ASSERT( p_sys );

	if( p_sys->p_data )
	{
		GFL_HEAP_FreeMemory( p_sys->p_data );
		p_sys->p_data = NULL;
		p_sys->num		= 0;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�[���G���A���̎擾
 *
	*	@param	cp_sys	�V�X�e��
 *
 *	@return	���
 */
//-----------------------------------------------------------------------------
const FLD_SCENEAREA_DATA* FLD_SCENEAREA_LOADER_GetData( const FLD_SCENEAREA_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->p_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�V�[���G���A��񐔂̎擾
 *
 *	@param	cp_sys	�V�X�e��
 *
	*	@return	���
 */
//-----------------------------------------------------------------------------
u32 FLD_SCENEAREA_LOADER_GetDataNum( const FLD_SCENEAREA_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );
	return cp_sys->num;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�֐��|�C���^�e�[�u���̎擾
 *
 *	@param	cp_sys	�V�X�e��
 *
 *	@return	�e�[�u��
 */
//-----------------------------------------------------------------------------
const FLD_SCENEAREA_FUNC* FLD_SCENEAREA_LOADER_GetFunc( const FLD_SCENEAREA_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );
	return &sc_FLD_SCENEAREA_FUNC;
}


// �f�o�b�N�@�\
#ifdef PM_DEBUG
void FLD_SCENEAREA_LOADER_LoadBinary( FLD_SCENEAREA_LOADER* p_sys, void* p_dat, u32 size )
{
	GF_ASSERT( p_sys );

	p_sys->p_data = p_dat;
	p_sys->num		= size / sizeof(FLD_SCENEAREA_DATA);

	// ���܂肪�Ȃ��H
	GF_ASSERT( (size % sizeof(FLD_SCENEAREA_DATA)) == 0 );

	TOMOYA_Printf( "SCENEAREA num = %d\n",  p_sys->num );
}

//----------------------------------------------------------------------------
/**
 *	@brief	scenearea���̎擾
 *
	*	@param	cp_sys	��[��
 *
 *	@return	���
 */
//-----------------------------------------------------------------------------
void* FLD_SCENEAREA_LOADER_DEBUG_GetData( const FLD_SCENEAREA_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );

	return cp_sys->p_data;
}

//----------------------------------------------------------------------------
/**
 *	@brief	scenearea���T�C�Y�̃V���g�N
 *
 *	@param	cp_sys	���[�N
 *
 *	@return	�T�C�Y
 */
//-----------------------------------------------------------------------------
u32 FLD_SCENEAREA_LOADER_DEBUG_GetDataSize( const FLD_SCENEAREA_LOADER* cp_sys )
{
	GF_ASSERT( cp_sys );

	return cp_sys->num * sizeof(FLD_SCENEAREA_DATA);
}

#endif





//-----------------------------------------------------------------------------
/**
 *				�e�R�[���o�b�N�֐��S
 */
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief  �V�[���G���A���  �G���A����
 *
 *	@param	cp_sys      �V�X�e��
 *	@param	cp_data     �f�[�^
 *	@param	cp_pos      ���@�ʒu
 *
 *	@retval TRUE    �G���A��
 *	@retval FALSE   �G���A�O
 */
//-----------------------------------------------------------------------------
static BOOL C3_SCENEAREA_CheckArea( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  VecFx32 npos;
  fx32 dist;
  u32 rotate;
  VecFx32 target;
  const FLD_SCENEAREA_CIRCLE_PARAM* cp_param = (FLD_SCENEAREA_CIRCLE_PARAM*)cp_data->area;

  VEC_Set( &target, cp_param->center_x, cp_param->center_y, cp_param->center_z );
  

  VEC_Subtract( cp_pos, &target, &npos );
  npos.y  = 0;
  dist    = VEC_Mag( &npos );
  VEC_Normalize( &npos, &npos );
  rotate  = FX_Atan2Idx( npos.x, npos.z );
  /*
  VEC_CrossProduct( &npos, &normal_vec, &normal );
  if( normal.y < 0 ){
    rotate = 0x10000 - rotate;
  }*/

  // �G���A������
  if( (cp_param->dist_min <= dist) && (cp_param->dist_max > dist) ){
    // rot_end - rot_start��180�x�ȏ�Ȃ�A�O��ʉ߂���P�W�O�̊p�x
    if( cp_param->rot_end < cp_param->rot_start )
    {
//      TOMOYA_Printf( "rot_end %d rot_start %d rotate %d\n", cp_param->rot_end, cp_param->rot_start, rotate );
      if( ((cp_param->rot_end >= rotate) && (rotate >= 0)) ||
          ((0x10000 > rotate) && (rotate >= cp_param->rot_start)) )
      {
        return TRUE;
      }
    }
    else
    {
      if( (cp_param->rot_start <= rotate) && (cp_param->rot_end > rotate) )
      {
        return TRUE;
      }
    }
  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �X�V����
 *
 *	@param	cp_sys      �V�X�e��
 *	@param	cp_data     �f�[�^
 *	@param	cp_pos      �ʒu���
 */
//-----------------------------------------------------------------------------
static void C3_SCENEAREA_Update( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  VecFx32 pos, target, n0, camera_pos;
  FIELD_CAMERA* p_camera;
  fx32 xz_dist;
  fx32 target_y;
  const FLD_SCENEAREA_CIRCLE_PARAM* cp_param = (const FLD_SCENEAREA_CIRCLE_PARAM*)cp_data->area;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );

	FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_DIRECT_POS );

  target.x = cp_param->center_x;
  target.y = cp_param->center_y;
  target.z = cp_param->center_z;
  target_y  = target.y;
  target.y  = 0;
  
  pos     = *cp_pos;
  pos.y   = 0;

  VEC_Subtract( &pos, &target, &pos );
  VEC_Normalize( &pos, &n0 );

  target.y = target_y;

  // �����x�N�g������A�J����angle�����߂�
  camera_pos.y = FX_Mul( FX_SinIdx( cp_param->pitch ), cp_param->length );
  xz_dist      = FX_Mul( FX_CosIdx( cp_param->pitch ), cp_param->length );
  camera_pos.x = FX_Mul( n0.x, xz_dist );
  camera_pos.z = FX_Mul( n0.z, xz_dist );
  camera_pos.x += target.x;
  camera_pos.y += target.y;
  camera_pos.z += target.z;
  
	FIELD_CAMERA_SetTargetPos( p_camera, &target );
	FIELD_CAMERA_SetCameraPos( p_camera, &camera_pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Œ�J�����́@�ݒ菈��
 *  
 *	@param	cp_sys    �V�X�e�����[�N
 *	@param	cp_data   �f�[�^
 *	@param	cp_pos    �ʒu���
 */
//-----------------------------------------------------------------------------
static void C3_SCENEAREA_FxCamera( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  FIELD_CAMERA* p_camera;
  const FLD_SCENEAREA_CIRCLE_FIXCAMERA_PARAM* cp_param = (const FLD_SCENEAREA_CIRCLE_FIXCAMERA_PARAM*)cp_data->area;
  VecFx32 target, camera_pos;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );

	FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_DIRECT_POS );

  // �^�[�Q�b�g�ʒu�ݒ�
  VEC_Set( &target, cp_param->target_x, cp_param->target_y, cp_param->target_z );
  VEC_Set( &camera_pos, cp_param->camera_x, cp_param->camera_y, cp_param->camera_z );
  FIELD_CAMERA_SetTargetPos( p_camera, &target );
  FIELD_CAMERA_SetCameraPos( p_camera, &camera_pos );
}



//----------------------------------------------------------------------------
/**
 *	@brief  �O���b�h�@��`�`�F�b�N
 *
 *	@param	cp_sys      �V�X�e��
 *	@param	cp_data     �f�[�^
 *	@param	cp_pos      �ʒu
 *
 *	@retval TRUE  �q�b�g* 
 *	@retval FALSE �O��  
 */
//-----------------------------------------------------------------------------
static BOOL SCENEAREA_CheckGridRect( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM* cp_param = (const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM*)cp_data->area;
  u16 grid_x, grid_z, grid_sizx, grid_sizz;
  VecFx32 local_pos;

  local_pos = *cp_pos;
  local_pos.x -= MMDL_VEC_X_GRID_OFFS_FX32;
  local_pos.z -= MMDL_VEC_Z_GRID_OFFS_FX32;

  grid_x = FX32_TO_GRID( local_pos.x );
  grid_z = FX32_TO_GRID( local_pos.z );

  // X �� Z�ǂ��炩��1�ł���K�v������
  GF_ASSERT( (cp_param->grid_sizx == 1) || (cp_param->grid_sizz == 1) );
  
  if( cp_param->grid_sizx == 1 )
  {
    grid_sizx = cp_param->grid_depth;
    grid_sizz = cp_param->grid_sizz;
  }
  else
  {
    grid_sizx = cp_param->grid_sizx;
    grid_sizz = cp_param->grid_depth;
  }

  if( (grid_x >= cp_param->grid_x) && (grid_x < cp_param->grid_x + grid_sizx) )
  {
    if( (grid_z >= cp_param->grid_z) && (grid_z < cp_param->grid_z + grid_sizz) )
    {
      return TRUE;
    }
  }
  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �O���b�h�@�A���O���X���[�Y�ύX
 *
 *	@param	cp_sys      �V�X�e��
 *	@param	cp_data     �f�[�^
 *	@param	cp_pos      �ʒu
 */
//-----------------------------------------------------------------------------
static void SCENEAREA_UpdateGridAngleChange( const FLD_SCENEAREA* cp_sys, const FLD_SCENEAREA_DATA* cp_data, const VecFx32* cp_pos )
{
  const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM* cp_param = (const FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM*)cp_data->area;
  FIELD_CAMERA* p_camera;
  VecFx32 target, camera_pos;
  fx32 dist;
  fx32 start_num;
  fx32 sub_num;
  s32 pitch;
  s32 yaw;
  fx32 length;

  VecFx32 local_pos;

  local_pos = *cp_pos;
  local_pos.x -= MMDL_VEC_X_GRID_OFFS_FX32;
  local_pos.z -= MMDL_VEC_Z_GRID_OFFS_FX32;

  p_camera = FLD_SCENEAREA_GetFieldCamera( cp_sys );

	FIELD_CAMERA_SetMode( p_camera, FIELD_CAMERA_MODE_CALC_CAMERA_POS );

  TOMOYA_Printf( "param\n" );
  TOMOYA_Printf( "start angle pitch=0x%x yaw=0x%x len=0x%x\n", cp_param->start_pitch, cp_param->start_yaw, cp_param->start_length );
  TOMOYA_Printf( "end angle pitch=0x%x yaw=0x%x len=0x%x\n", cp_param->end_pitch, cp_param->end_yaw, cp_param->end_length );

  // ���������̉��ւ̋�������A�J�����𓮂���
  dist = GRID_TO_FX32( cp_param->grid_depth );
  // ���H�@�c�H
  if( cp_param->grid_sizx == 1 )
  {
    // �E�ɉ��s��
    start_num = GRID_TO_FX32( cp_param->grid_x );
    sub_num   = local_pos.x - start_num;
  }
  else
  {
    // ��O�ɉ��s��
    start_num = GRID_TO_FX32( cp_param->grid_z );
    sub_num   = local_pos.z - start_num;
  }

  // �ړ���������A���O���Ƌ��������߂�
  pitch = cp_param->end_pitch - cp_param->start_pitch;
  pitch = (pitch * sub_num) / dist;
  pitch += cp_param->start_pitch;

  yaw = cp_param->end_yaw - cp_param->start_yaw;
  yaw = (yaw * sub_num) / dist;
  yaw += cp_param->start_yaw;

  length = cp_param->end_length - cp_param->start_length;
  length = FX_Div( FX_Mul(length, sub_num), dist );
  length += cp_param->start_length;

  TOMOYA_Printf( "set angle\n" );
  TOMOYA_Printf( "pitch 0x%x yaw 0x%x len 0x%x\n", (u16)pitch, (u16)yaw, length );

  // �A���O����ݒ�
  FIELD_CAMERA_SetAnglePitch( p_camera, pitch );
  FIELD_CAMERA_SetAngleYaw( p_camera, yaw );
  FIELD_CAMERA_SetAngleLen( p_camera, length );
}



