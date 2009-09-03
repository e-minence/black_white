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
  u8 frameStep; // �Ԉ����
  u16 dataSize; // �L���f�[�^��
  u16 dataHead; // �擪�f�[�^�C���f�b�N�X
}
ANIME_ATTR;

//===================================================================================
/**
 * @brief �A�j���[�V�����f�[�^
 */
//===================================================================================
struct _ICA_DATA
{
  u16 frameSize;      // ���t���[����
  
  u16 scaleDataSize;  // �X�P�[���f�[�^��
  float*  scaleData;  // �X�P�[���f�[�^�z��

  u16 rotateDataSize; // ��]�f�[�^��
  float*  rotateData; // ��]�f�[�^�z��

  u16 transDataSize;  // ���s�ړ��f�[�^��
  float*  transData;  // ���s�ړ��f�[�^�z��

  // �A�j���[�V��������
  ANIME_ATTR anime_attr[ VALUE_TYPE_NUM ];
};


//===================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//===================================================================================
static void LoadIcaData( ICA_DATA* p_anime, HEAPID heap_id, ARCID arc_id, ARCDATID dat_id );
static float GetValue( ICA_DATA* p_anime, VALUE_TYPE anime_type, fx32 now_frame );
static float CalcLinearValue( 
    const int frame_size, const float now_frame, 
    const int frame_step, const int data_size, const int data_head, const float* value );


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
  GFL_HEAP_FreeMemory( p_data->scaleData );
  GFL_HEAP_FreeMemory( p_data->rotateData );
  GFL_HEAP_FreeMemory( p_data->transData );
  GFL_HEAP_FreeMemory( p_data );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �w��t���[���ɂ�����ʒu�f�[�^���擾����
 *
 * @param p_anime   �A�j���[�V�����f�[�^
 * @param p_vec     �擾�������W�̊i�[��
 * @param now_frame ���݂̃t���[����
 */
//-----------------------------------------------------------------------------------
void ICA_DATA_GetPos( ICA_DATA* p_anime, VecFx32* p_vec, fx32 now_frame )
{ 
  float x = GetValue( p_anime, VALUE_TYPE_TRANSLATE_X, now_frame );
  float y = GetValue( p_anime, VALUE_TYPE_TRANSLATE_Y, now_frame );
  float z = GetValue( p_anime, VALUE_TYPE_TRANSLATE_Z, now_frame );

  VEC_Set( p_vec, FX_F32_TO_FX32(x), FX_F32_TO_FX32(y), FX_F32_TO_FX32(z) );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �w��t���[���ɂ���������f�[�^���擾����
 *
 * @param p_anime   �A�j���[�V�����f�[�^
 * @param p_vec     �擾�����x�N�g���̊i�[��
 * @param now_frame ���݂̃t���[����
 */
//-----------------------------------------------------------------------------------
void ICA_DATA_GetDir( ICA_DATA* p_anime, VecFx32* p_vec, fx32 now_frame )
{
  float x, y, z;
  u16 rx, ry, rz;
  MtxFx33 matrix;
  VecFx32 vec_norm;

  // �w��t���[���̒l���擾
  x = GetValue( p_anime, VALUE_TYPE_ROTATE_X, now_frame );
  y = GetValue( p_anime, VALUE_TYPE_ROTATE_Y, now_frame );
  z = GetValue( p_anime, VALUE_TYPE_ROTATE_Z, now_frame );

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
  VEC_Set( &vec_norm, 0, 0, -FX32_ONE );
  MTX_MultVec33( &vec_norm, &matrix, p_vec );
}

//---------------------------------------------------------------------------
/**
 * @brief �J�������W�E�^�[�Q�b�g���W���w��t���[���̏�Ԃɐݒ肷��
 *
 * @param p_anime   �A�j���[�V�����f�[�^
 * @param p_camera  �ݒ�Ώۂ̃J����
 * @param now_frame ���݂̃t���[����
 */
//---------------------------------------------------------------------------
void ICA_DATA_SetCameraStatus( 
    ICA_DATA* p_anime, GFL_G3D_CAMERA* p_camera, fx32 now_frame )
{
  VecFx32 pos, target;

  // ���W�E�����x�N�g�����擾
  ICA_DATA_GetPos( p_anime, &pos, now_frame );
  ICA_DATA_GetDir( p_anime, &target, now_frame );
  VEC_Add( &pos, &target, &target );

  // �J�����̐ݒ�
  GFL_G3D_CAMERA_SetPos( p_camera, &pos );
  GFL_G3D_CAMERA_SetTarget( p_camera, &target );
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
 * @param p_anime �ǂݍ��񂾃f�[�^�̊i�[��ϐ�
 * @param heap_id �g�p����q�[�vID
 * @param arc_id  �ǂݍ��ރA�[�J�C�u�t�@�C��
 * @param dat_id  �A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 */
//-----------------------------------------------------------------------------------
static void LoadIcaData( ICA_DATA* p_anime, HEAPID heap_id, ARCID arc_id, ARCDATID dat_id )
{
  int i;
  int ofs = 0;  // �擪����̃I�t�Z�b�g
  
  // �t���[���T�C�Y���擾
  GFL_ARC_LoadDataOfs( &p_anime->frameSize, arc_id, dat_id, ofs, sizeof( u16 ) );      
  ofs += sizeof( u16 );

  // �X�P�[���f�[�^�����擾
  GFL_ARC_LoadDataOfs( &p_anime->scaleDataSize, arc_id, dat_id, ofs, sizeof( u16 ) );  
  ofs += sizeof( u16 );

  // �X�P�[���f�[�^���擾
  p_anime->scaleData = GFL_HEAP_AllocMemory( heap_id, sizeof( float ) * p_anime->scaleDataSize );
  for( i=0; i<p_anime->scaleDataSize; i++ )
  {
    GFL_ARC_LoadDataOfs( &p_anime->scaleData[i], arc_id, dat_id, ofs, sizeof(float) );  
    ofs += sizeof( float );
  }

  // ��]�f�[�^�����擾
  GFL_ARC_LoadDataOfs( &p_anime->rotateDataSize, arc_id, dat_id, ofs, sizeof( u16 ) );  
  ofs += sizeof( u16 );

  // ��]�f�[�^���擾
  p_anime->rotateData = GFL_HEAP_AllocMemory( heap_id, sizeof( float ) * p_anime->rotateDataSize );
  for( i=0; i<p_anime->rotateDataSize; i++ )
  {
    GFL_ARC_LoadDataOfs( &p_anime->rotateData[i], arc_id, dat_id, ofs, sizeof( float ) );  
    ofs += sizeof( float );
  }

  // ���s�ړ��f�[�^�����擾
  GFL_ARC_LoadDataOfs( &p_anime->transDataSize, arc_id, dat_id, ofs, sizeof( u16 ) ); 
  ofs += sizeof( u16 );

  // ���s�ړ��f�[�^���擾
  p_anime->transData = GFL_HEAP_AllocMemory( heap_id, sizeof( float ) * p_anime->transDataSize );
  for( i=0; i<p_anime->transDataSize; i++ )
  {
    GFL_ARC_LoadDataOfs( &p_anime->transData[i], arc_id, dat_id, ofs, sizeof( float ) );  
    ofs += sizeof( float );
  }

  // <node_anm>�v�f�̑����l���擾
  for( i=0; i<VALUE_TYPE_NUM; i++ )
  {
    GFL_ARC_LoadDataOfs( &p_anime->anime_attr[i].frameStep, arc_id, dat_id, ofs, sizeof( u8 ) );  
    ofs += sizeof( u8 );
    GFL_ARC_LoadDataOfs( &p_anime->anime_attr[i].dataSize, arc_id, dat_id, ofs, sizeof( u16 ) );  
    ofs += sizeof( u16 );
    GFL_ARC_LoadDataOfs( &p_anime->anime_attr[i].dataHead, arc_id, dat_id, ofs, sizeof( u16 ) );  
    ofs += sizeof( u16 );
  }
}

//-----------------------------------------------------------------------------------
/**
 * @brief �w��t���[���ɂ�����l���v�Z����
 *
 * @param p_anime    �A�j���[�V�����f�[�^
 * @param anime_type �擾����l�̃^�C�v
 * @param now_frame  ���݂̃t���[����
 *
 * @return �w��t���[�����̒l
 */
//-----------------------------------------------------------------------------------
static float GetValue( ICA_DATA* p_anime, VALUE_TYPE anime_type, fx32 now_frame )
{
  float*            p_value = NULL;
  ANIME_ATTR* p_attr = &p_anime->anime_attr[ anime_type ];

  // �擾�������l�̔z�������
  switch( anime_type )
  {
  case VALUE_TYPE_SCALE_X:
  case VALUE_TYPE_SCALE_Y:
  case VALUE_TYPE_SCALE_Z:
    p_value = p_anime->scaleData;
    break;
  case VALUE_TYPE_ROTATE_X:
  case VALUE_TYPE_ROTATE_Y:
  case VALUE_TYPE_ROTATE_Z:
    p_value = p_anime->rotateData;
    break;
  case VALUE_TYPE_TRANSLATE_X:
  case VALUE_TYPE_TRANSLATE_Y:
  case VALUE_TYPE_TRANSLATE_Z:
    p_value = p_anime->transData;
    break;
  default:
    return 0;
  }

  // �l�����߂�
  return CalcLinearValue(
      p_anime->frameSize, FX_FX32_TO_F32(now_frame),
      p_attr->frameStep, p_attr->dataSize, p_attr->dataHead, p_value );
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���`�⊮�Ŏw��t���[���̒l�����߂�
 *
 * @param frame_size ���t���[����
 * @param now_frame  ���݂̃t���[��
 * @param frame_step �Ԉ����
 * @param data_size  �f�[�^��
 * @param data_head  �擪�f�[�^�C���f�b�N�X
 * @param value      ���f�[�^�z��
 *
 * @return �w��t���[�����̒l
 */
//-----------------------------------------------------------------------------------
static float CalcLinearValue( 
    const int frame_size, const float now_frame, 
    const int frame_step, const int data_size, const int data_head, const float* value )
{
  int frame;
  int shift, ival, iframe_last_interp;
  int weight0, weight1;
  float val0, val1;

  // ���[�v������
  frame = (int)now_frame % frame_size;

  if( data_size == 1 ) // �f�[�^���P�̎�
  {
    return value[ data_head ];
  }

  if(frame_step == 1) // frame_step���P�̎�
  {
    return value[ data_head + frame ];
  }

  shift = ( frame_step==2 ) ? 1 : 2;  // �V�t�g�l
  ival  = frame >> shift;             // ���݂̃t���[���ɑΉ�������f�[�^�ʒu
  ival += data_head;

  // �Ԉ��������� �S�t���[�������̂������ƂȂ�t���[��
  iframe_last_interp = ((frame_size - 1) >> shift) << shift;

  // �������ƂȂ�t���[���ȍ~�̏ꍇ�A���f�[�^�z�񂩂璼�ڕԂ�
  if( frame >= iframe_last_interp )
  {
    return value[ival + (frame - iframe_last_interp)];
  }

  // �������ƂȂ�t���[�����O�̏ꍇ�͕�Ԍv�Z�ŋ��߂�
  val0    = value[ival ];
  val1    = value[ival + 1];
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
