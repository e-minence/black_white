/////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  .ica�t�@�C���̃A�j���[�V�����f�[�^�������֐��Q
 * @file   ica_data.c
 * @author obata
 * @date   2009.09.01
 *
 */
/////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "ica_data.h"


//===================================================================================
/**
 * @brief �萔
 */
//===================================================================================
// �l�̎��
typedef enum 
{
  VALUE_TYPE_SCALE_X,     // �g�k
  VALUE_TYPE_SCALE_Y,     // 
  VALUE_TYPE_SCALE_Z,     //
  VALUE_TYPE_ROTATE_X,    // ��]
  VALUE_TYPE_ROTATE_Y,    // 
  VALUE_TYPE_ROTATE_Z,    // 
  VALUE_TYPE_TRANSLATE_X, // ���s�ړ�
  VALUE_TYPE_TRANSLATE_Y, // 
  VALUE_TYPE_TRANSLATE_Z, // 
  VALUE_TYPE_NUM
}
VALUE_TYPE;

//===================================================================================
/**
 * @brief �A�j���[�V��������
 *          �l�̔z����ǂ̂悤�ɎQ�Ƃ���̂������߂鑮��
 */
//===================================================================================
typedef struct 
{
  u32 frameStep; // �Ԉ����
  u32 dataSize; // �L���f�[�^��
  u32 dataHead; // �擪�f�[�^�C���f�b�N�X
}
ANIME_ATTR;

//===================================================================================
/**
 * @brief �A�j���[�V�����f�[�^
 */
//===================================================================================
struct _ICA_DATA
{
  u32 frameSize;      // ���t���[����
  u32 scaleDataSize;  // �X�P�[���f�[�^��
  u32 rotateDataSize; // ��]�f�[�^��
  u32 transDataSize;  // ���s�ړ��f�[�^��

  // �l�^�C�v���Ƃ̃A�j���[�V��������
  ANIME_ATTR anime_attr[ VALUE_TYPE_NUM ];

  // �A�[�J�C�u�f�[�^�n���h��
  ARCHANDLE* arcHandle;
  ARCDATID datID;
};


//===================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//===================================================================================
static void LoadIcaData( ICA_DATA* p_data, HEAPID heap_id, ARCID arc_id, ARCDATID dat_id );
static float GetValue( ICA_DATA* p_data, VALUE_TYPE anime_type, fx32 now_frame );
static float CalcLinearValue( ICA_DATA* ica_data, VALUE_TYPE val_type, const float now_frame );
static float GetDataByHandle( ICA_DATA* ica_data, VALUE_TYPE val_type, int val_index );


//===================================================================================
/**
 * @brief ���J�֐��̒�`
 */
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�����f�[�^���쐬����
 *
 * @param heap_id �g�p����q�[�vID
 * @param arc_id  �ǂݍ��ރA�[�J�C�u�t�@�C��
 * @param dat_id  �A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 *
 * @return �쐬�����A�j���[�V�����f�[�^
 */
//-----------------------------------------------------------------------------------
ICA_DATA* ICA_DATA_Create( HEAPID heap_id, ARCID arc_id, ARCDATID dat_id )
{
  ICA_DATA* data;

  // �C���X�^���X�𐶐�
  data = (ICA_DATA*)GFL_HEAP_AllocMemory( heap_id, sizeof(ICA_DATA) );

  // �f�[�^��ǂݍ���
  LoadIcaData( data, heap_id, arc_id, dat_id );

  // �A�[�J�C�u�f�[�^�n���h���I�[�v��
  data->arcHandle = GFL_ARC_OpenDataHandle( arc_id, heap_id );
  data->datID = dat_id;

  // �쐬�����C���X�^���X��Ԃ�
  return data;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�����f�[�^��j������
 *
 * @param p_data �j������f�[�^
 */
//-----------------------------------------------------------------------------------
void ICA_DATA_Delete( ICA_DATA* p_data )
{
  GFL_ARC_CloseDataHandle( p_data->arcHandle );
  GFL_HEAP_FreeMemory( p_data );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �w��t���[���ɂ����镽�s�ړ��f�[�^���擾����
 *
 * @param p_data   �A�j���[�V�����f�[�^
 * @param p_vec     �擾�������W�̊i�[��
 * @param now_frame ���݂̃t���[����
 */
//-----------------------------------------------------------------------------------
void ICA_DATA_GetTranslate( ICA_DATA* p_data, VecFx32* p_vec, fx32 now_frame )
{ 
  float x = GetValue( p_data, VALUE_TYPE_TRANSLATE_X, now_frame );
  float y = GetValue( p_data, VALUE_TYPE_TRANSLATE_Y, now_frame );
  float z = GetValue( p_data, VALUE_TYPE_TRANSLATE_Z, now_frame );

  VEC_Set( p_vec, FX_F32_TO_FX32(x), FX_F32_TO_FX32(y), FX_F32_TO_FX32(z) );
}

//---------------------------------------------------------------------------
/**
 * @brief �w��t���[���ɂ������]�f�[�^���擾����
 *
 * @param p_data     �A�j���[�V�����f�[�^
 * @param p_vec      �擾������]�̊i�[��
 * @param now_frame  ���݂̃t���[����
 */
//---------------------------------------------------------------------------
void ICA_DATA_GetRotate( ICA_DATA* p_data, VecFx32* p_vec, fx32 now_frame )
{ 
  float x = GetValue( p_data, VALUE_TYPE_ROTATE_X, now_frame );
  float y = GetValue( p_data, VALUE_TYPE_ROTATE_Y, now_frame );
  float z = GetValue( p_data, VALUE_TYPE_ROTATE_Z, now_frame );

  VEC_Set( p_vec, FX_F32_TO_FX32(x), FX_F32_TO_FX32(y), FX_F32_TO_FX32(z) );
}

//---------------------------------------------------------------------------
/**
 * @brief �w��t���[���ɂ���������f�[�^���擾����
 *
 * @param p_data        �A�j���[�V�����f�[�^
 * @param p_vec_forward  �擾�����O���x�N�g���̊i�[��
 * @param p_vec_upward   �擾��������x�N�g���̊i�[��
 * @param now_frame      ���݂̃t���[����
 */
//---------------------------------------------------------------------------
void ICA_DATA_GetDir( 
    ICA_DATA* p_data, VecFx32* p_vec_forward, VecFx32* p_vec_upward, fx32 now_frame )
{
  float x, y, z;
  u16 rx, ry, rz;
  MtxFx33 matrix;
  VecFx32 def_forward = { 0, 0, -FX32_ONE };
  VecFx32 def_upward  = { 0, FX32_ONE, 0 };

  // �w��t���[���̒l���擾
  x = GetValue( p_data, VALUE_TYPE_ROTATE_X, now_frame );
  y = GetValue( p_data, VALUE_TYPE_ROTATE_Y, now_frame );
  z = GetValue( p_data, VALUE_TYPE_ROTATE_Z, now_frame );

  // ���̒l�ɕ␳
  while( x < 0 ) x += 360.0f;
  while( y < 0 ) y += 360.0f;
  while( z < 0 ) z += 360.0f;

  // u16�ɕϊ�
  rx = x / 360.0f * 0xffff;
  ry = y / 360.0f * 0xffff;
  rz = z / 360.0f * 0xffff; 

  // ������\���x�N�g�����쐬
  GFL_CALC3D_MTX_CreateRot( rx, ry, rz, &matrix );
  MTX_MultVec33( &def_forward, &matrix, p_vec_forward );
  MTX_MultVec33( &def_upward, &matrix, p_vec_upward );
}

//---------------------------------------------------------------------------
/**
 * @brief �J�������W�E�^�[�Q�b�g���W���w��t���[���̏�Ԃɐݒ肷��
 *
 * @param p_data   �A�j���[�V�����f�[�^
 * @param p_camera  �ݒ�Ώۂ̃J����
 * @param now_frame ���݂̃t���[����
 */
//---------------------------------------------------------------------------
void ICA_DATA_SetCameraStatus( 
    ICA_DATA* p_data, GFL_G3D_CAMERA* p_camera, fx32 now_frame )
{
  VecFx32 pos, target, forward, upward;

  // ���W�E�����x�N�g�����擾
  ICA_DATA_GetTranslate( p_data, &pos, now_frame );
  ICA_DATA_GetDir( p_data, &forward, &upward, now_frame );
  VEC_Add( &pos, &forward, &target );

  // �J�����̐ݒ�
  GFL_G3D_CAMERA_SetPos( p_camera, &pos );
  GFL_G3D_CAMERA_SetTarget( p_camera, &target );
  GFL_G3D_CAMERA_SetCamUp( p_camera, &upward );
}


//===================================================================================
/**
 * @brief ����J�֐��̒�`
 */
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief �I���W�i���t�H�[�}�b�g�̃o�C�i���f�[�^�����[�h����
 *
 * @param p_data �ǂݍ��񂾃f�[�^�̊i�[��ϐ�
 * @param heap_id �g�p����q�[�vID
 * @param arc_id  �ǂݍ��ރA�[�J�C�u�t�@�C��
 * @param dat_id  �A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 */
//----------------------------------------------------------------------------------- 
static void LoadIcaData( ICA_DATA* p_data, HEAPID heap_id, ARCID arc_id, ARCDATID dat_id )
{
  int i;
  void* data = NULL;
  int size = 0;
  int ofs = 0;

  // �f�[�^���擾
  size = GFL_ARC_GetDataSize( arc_id, dat_id );
  data = GFL_HEAP_AllocMemoryLo( heap_id, size ); 
  GFL_ARC_LoadData( data, arc_id, dat_id );


  // �t���[���T�C�Y���擾
  p_data->frameSize = *( (u32*)( (int)data + ofs ) );
  ofs += sizeof( u32 );

  // �X�P�[���f�[�^�����擾
  p_data->scaleDataSize = *( (u32*)( (int)data + ofs ) );
  ofs += sizeof( u32 );
  OBATA_Printf( "scaleDataSize = %d\n", p_data->scaleDataSize );
  ofs += p_data->scaleDataSize * sizeof( float );

  // ��]�f�[�^�����擾
  p_data->rotateDataSize = *( (u32*)( (int)data + ofs ) );
  ofs += sizeof( u32 );
  OBATA_Printf( "rotateDataSize = %d\n", p_data->rotateDataSize );
  ofs += p_data->rotateDataSize * sizeof( float );

  // ���s�ړ��f�[�^�����擾
  p_data->transDataSize = *( (u32*)( (int)data + ofs ) );
  ofs += sizeof( u32 );
  OBATA_Printf( "transDataSize = %d\n", p_data->transDataSize );
  ofs += p_data->transDataSize * sizeof( float );

  // <node_anm>�v�f�̑����l���擾
  for( i=0; i<VALUE_TYPE_NUM; i++ )
  {
    p_data->anime_attr[i].frameStep = *( (u32*)( (int)data + ofs ) );
    ofs += sizeof( u32 );
    p_data->anime_attr[i].dataSize = *( (u32*)( (int)data + ofs ) );
    ofs += sizeof( u32 );
    p_data->anime_attr[i].dataHead = *( (u32*)( (int)data + ofs ) );
    ofs += sizeof( u32 );
    OBATA_Printf( "%d %d %d\n", 
        p_data->anime_attr[i].frameStep,
        p_data->anime_attr[i].dataSize,
        p_data->anime_attr[i].dataHead );
  }

  // ��n��
  GFL_HEAP_FreeMemory( data );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �w��t���[���ɂ�����l���v�Z����
 *
 * @param p_data    �A�j���[�V�����f�[�^
 * @param anime_type �擾����l�̃^�C�v
 * @param now_frame  ���݂̃t���[����
 *
 * @return �w��t���[�����̒l
 */
//-----------------------------------------------------------------------------------
static float GetValue( ICA_DATA* p_data, VALUE_TYPE anime_type, fx32 now_frame )
{
  // �l�����߂�
  return CalcLinearValue( p_data, anime_type, FX_FX32_TO_F32(now_frame) );
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���`�⊮�Ŏw��t���[���̒l�����߂�
 *
 * @param ica_data   �A�j���[�V�����E�f�[�^
 * @param val_type   �擾�l�̃^�C�v
 * @param now_frame  �t���[���w��
 *
 * @return �w��t���[���ɂ�����l
 */
//-----------------------------------------------------------------------------------
static float CalcLinearValue( ICA_DATA* ica_data, VALUE_TYPE val_type, const float now_frame )
{
  const int frame_size = ica_data->frameSize;
  const int frame_step = ica_data->anime_attr[ val_type ].frameStep;
  const int data_size  = ica_data->anime_attr[ val_type ].dataSize;
  const int data_head  = ica_data->anime_attr[ val_type ].dataHead;
  int frame;
  int shift, ival, iframe_last_interp;
  int weight0, weight1;
  float val0, val1;

  // ���[�v������
  frame = (int)now_frame % frame_size;

  if( data_size == 1 ) // �f�[�^���P�̎�
  {
    ival = data_head;
    return GetDataByHandle( ica_data, val_type, ival );
  }

  if( frame_step == 1 ) // frame_step���P�̎�
  {
    ival = data_head + frame;
    return GetDataByHandle( ica_data, val_type, ival );
  }

  shift = ( frame_step==2 ) ? 1 : 2;  // �V�t�g�l
  ival  = frame >> shift;             // ���݂̃t���[���ɑΉ�������f�[�^�ʒu
  ival += data_head;

  // �Ԉ��������� �S�t���[�������̂������ƂȂ�t���[��
  iframe_last_interp = ((frame_size - 1) >> shift) << shift;

  // �������ƂȂ�t���[���ȍ~�̏ꍇ�A���f�[�^�z�񂩂璼�ڕԂ�
  if( frame >= iframe_last_interp )
  {
    ival = ival + ( frame - iframe_last_interp );
    return GetDataByHandle( ica_data, val_type, ival );
  }

  // �������ƂȂ�t���[�����O�̏ꍇ�͕�Ԍv�Z�ŋ��߂�
  val0    = GetDataByHandle( ica_data, val_type, ival );
  val1    = GetDataByHandle( ica_data, val_type, ival+1 );
  weight1 = frame - ( ival << shift );
  weight0 = frame_step - weight1;

  // ���傤�ǃt���[����ɂ��鎞
  if( weight1 == 0 )
  {
    return val0;
  }
  // �ʏ�̕�ԏ���
  else 
  {
    return ( (val0 * weight0 + val1 * weight1) / ( weight0 + weight1 ) );
  }
} 

//-----------------------------------------------------------------------------------
/**
 * @brief �A�[�J�C�u�f�[�^�n���h������, �w��f�[�^���擾����
 *
 * @param ica_data  ica�f�[�^
 * @param val_type  �擾�l�̃^�C�v
 * @param val_index �擾�l�̃C���f�b�N�X
 *
 * @return �w��f�[�^�̎w��t���[���ɂ�����l
 */
//-----------------------------------------------------------------------------------
static float GetDataByHandle( ICA_DATA* ica_data, VALUE_TYPE val_type, int val_index )
{
  int ofs;
  float val;

  // �擾����l�̃f�[�^���I�t�Z�b�g���Z�o
  switch( val_type )
  {
  case VALUE_TYPE_SCALE_X:
  case VALUE_TYPE_SCALE_Y:
  case VALUE_TYPE_SCALE_Z:
    ofs = sizeof(u32) 
        + sizeof(u32) + ( val_index * sizeof(float) );
    break;
  case VALUE_TYPE_ROTATE_X:
  case VALUE_TYPE_ROTATE_Y:
  case VALUE_TYPE_ROTATE_Z: 
    ofs = sizeof(u32) 
        + sizeof(u32) + ( ica_data->scaleDataSize * sizeof(float) )
        + sizeof(u32) + ( val_index * sizeof(float) );
    break;
  case VALUE_TYPE_TRANSLATE_X:
  case VALUE_TYPE_TRANSLATE_Y:
  case VALUE_TYPE_TRANSLATE_Z:
    ofs = sizeof(u32) 
        + sizeof(u32) + ( ica_data->scaleDataSize * sizeof(float) )
        + sizeof(u32) + ( ica_data->rotateDataSize * sizeof(float) )
        + sizeof(u32) + ( val_index * sizeof(float) );
    break;
  }

  // �l���擾
  GFL_ARC_LoadDataOfsByHandle( ica_data->arcHandle, ica_data->datID, ofs, sizeof(float), &val );
  return val;
}
