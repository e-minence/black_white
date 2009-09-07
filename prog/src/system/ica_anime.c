/////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  �I���W�i��ica�A�j���[�V�����Ǘ�
 * @file   ica_anime.c
 * @author obata
 * @date   2009.09.04
 *
 */
/////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "ica_anime.h"


//===================================================================================
/**
 * @brief �萔
 */
//===================================================================================
#define DEF_BUFSIZE (10)


//===================================================================================
/**
 * @brief ica�f�[�^�Ǘ����[�N
 */
//===================================================================================
struct _ICA_ANIME
{
  HEAPID     heapID;        // �g�p����q�[�vID
  ARCHANDLE* arcHandle;     // �A�[�J�C�u�n���h��
  ARCDATID   datID;         // �A�[�J�C�u���f�[�^�C���f�b�N�X
  u32        frameSize;     // �A�j���[�V�����̑S�t���[����
  fx32       nowFrame;      // ���݂̃t���[����
  int        bufStartFrame; // �o�b�t�@�擪�f�[�^�̃t���[����
  int        bufSize;       // �o�b�t�@�T�C�Y
  VecFx32*   transBuf;      // ���s�ړ��o�b�t�@
  VecFx32*   rotateBuf;     // ��]�o�b�t�@
};


//===================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//===================================================================================
void UpdateBuf( ICA_ANIME* anime, fx32 start_frame );


//===================================================================================
/**
 * @breif ���J�֐��̒�`
 */
//===================================================================================

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V�������쐬����
 *
 * @param heap_id �g�p����q�[�vID
 * @param arc_id  �A�j���[�V�����f�[�^�̃A�[�J�C�u�t�@�C���w��
 * @param dat_id  �A�[�J�C�u�t�@�C����̃C���f�b�N�X�i���o�[
 *
 * @return �쐬�����A�j���[�V�����Ǘ��I�u�W�F�N�g
 */
//-----------------------------------------------------------------------------------
ICA_ANIME* ICA_ANIME_Create( HEAPID heap_id, ARCID arc_id, ARCDATID dat_id )
{
  ICA_ANIME* anime;

  // ���[�N���m��
  anime = (ICA_ANIME*)GFL_HEAP_AllocMemory( heap_id, sizeof(ICA_ANIME) );

  // ������
  anime->heapID        = heap_id;
  anime->arcHandle     = GFL_ARC_OpenDataHandle( arc_id, heap_id );
  anime->datID         = dat_id;
  anime->nowFrame      = 0;
  anime->bufStartFrame = 0;
  anime->bufSize       = DEF_BUFSIZE;
  anime->transBuf      = GFL_HEAP_AllocMemory( heap_id, sizeof(VecFx32) * anime->bufSize );
  anime->rotateBuf     = GFL_HEAP_AllocMemory( heap_id, sizeof(VecFx32) * anime->bufSize );
  GFL_ARC_LoadDataOfsByHandle( anime->arcHandle, anime->datID, 0, sizeof(u32), &anime->frameSize );
  UpdateBuf( anime, 0 );
  OBATA_Printf( "ICA_ANIME_Create\n" );
  return anime;
}

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V������j������
 *
 * @param anime �j������A�j���[�V����
 */
//-----------------------------------------------------------------------------------
void ICA_ANIME_Delete( ICA_ANIME* anime )
{
  GFL_ARC_CloseDataHandle( anime->arcHandle );
  GFL_HEAP_FreeMemory( anime->transBuf );
  GFL_HEAP_FreeMemory( anime->rotateBuf );
  GFL_HEAP_FreeMemory( anime );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �A�j���[�V������i�߂�
 *
 * @param anime �X�V����A�j���[�V����
 * @param frame �i�߂�t���[����
 */
//-----------------------------------------------------------------------------------
void ICA_ANIME_IncAnimeFrame( ICA_ANIME* anime, fx32 frame )
{
  // ���݃t���[�����X�V
  anime->nowFrame += frame;

  if( anime->frameSize <= (anime->nowFrame>>FX32_SHIFT) )
  {
    anime->nowFrame = 0;
    UpdateBuf( anime, anime->nowFrame );
    return;
  }

  // ���݃t���[�����o�b�t�@�����O�͈͂𒴂�����, �o�b�t�@���X�V����
  if( (anime->bufStartFrame + anime->bufSize) <= (anime->nowFrame >> FX32_SHIFT) )
  {
    UpdateBuf( anime, anime->nowFrame );
  }
  OBATA_Printf( "ICA_ANIME_IncAnimeFrame : %d\n", (anime->nowFrame>>FX32_SHIFT) );
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���݃t���[���ɂ����镽�s�ړ��f�[�^���擾����
 *
 * @param anime   �擾�ΏۃA�j���[�V����
 * @param vec_dst �擾�����f�[�^�̊i�[��
 */
//-----------------------------------------------------------------------------------
void ICA_ANIME_GetTranslate( ICA_ANIME* anime, VecFx32* vec_dst )
{
  // �o�b�t�@���C���f�b�N�X���v�Z
  int buf_index = (anime->nowFrame>>FX32_SHIFT) - anime->bufStartFrame;

  // ���݃t���[���̒l���擾
  *vec_dst = anime->transBuf[ buf_index ];
}

//-----------------------------------------------------------------------------------
/**
 * @brief ���݃t���[���ɂ������]�f�[�^���擾����
 *
 * @param anime   �擾�ΏۃA�j���[�V����
 * @param vec_dst �擾�����f�[�^�̊i�[��
 */
//-----------------------------------------------------------------------------------
void ICA_ANIME_GetRotate( ICA_ANIME* anime, VecFx32* vec_dst )
{
  // �o�b�t�@���C���f�b�N�X���v�Z
  int buf_index = (anime->nowFrame>>FX32_SHIFT) - anime->bufStartFrame;

  // ���݃t���[���̒l���擾
  *vec_dst = anime->rotateBuf[ buf_index ];
}

//---------------------------------------------------------------------------
/**
 * @brief �J�������W�E�^�[�Q�b�g���W���w��t���[���̏�Ԃɐݒ肷��
 *
 * @param anime   �ݒ�A�j���[�V����
 * @param camera  �ݒ�Ώۂ̃J����
 */
//---------------------------------------------------------------------------
void ICA_ANIME_SetCameraStatus( ICA_ANIME* anime, GFL_G3D_CAMERA* camera )
{
  float x, y, z;
  u16 rx, ry, rz;
  MtxFx33 matrix;
  VecFx32 rotate;
  VecFx32 def_forward = { 0, 0, -FX32_ONE };
  VecFx32 def_upward  = { 0, FX32_ONE, 0 }; 
  VecFx32 pos, target, forward, upward;

  ICA_ANIME_GetRotate( anime, &rotate );
  x = FX_FX32_TO_F32( rotate.x );
  y = FX_FX32_TO_F32( rotate.y );
  z = FX_FX32_TO_F32( rotate.z );
  while( x < 0 ) x += 360.0f;
  while( y < 0 ) y += 360.0f;
  while( z < 0 ) z += 360.0f;
  rx = x / 360.0f * 0xffff;
  ry = y / 360.0f * 0xffff;
  rz = z / 360.0f * 0xffff; 
  GFL_CALC3D_MTX_CreateRot( rx, ry, rz, &matrix );
  MTX_MultVec33( &def_forward, &matrix, &forward );
  MTX_MultVec33( &def_upward, &matrix, &upward );

  ICA_ANIME_GetTranslate( anime, &pos );
  VEC_Add( &pos, &forward, &target );

  // �J�����̐ݒ�
  GFL_G3D_CAMERA_SetPos( camera, &pos );
  GFL_G3D_CAMERA_SetTarget( camera, &target );
  GFL_G3D_CAMERA_SetCamUp( camera, &upward );
}


//===================================================================================
/**
 * @breif ���J�֐��̒�`
 */
//===================================================================================

//---------------------------------------------------------------------------
/**
 * @brief �o�b�t�@���X�V����
 * 
 * @param anime       �X�V�Ώۂ̃A�j���[�V�����Ǘ��I�u�W�F�N�g
 * @param start_frame �V�����f�[�^�̊J�n�t���[��
 */
//---------------------------------------------------------------------------
void UpdateBuf( ICA_ANIME* anime, fx32 start_frame )
{
  int    i, j;
  int    frame     = (start_frame >> FX32_SHIFT) % anime->frameSize;
  int    ofs       = 112 + sizeof(float) * 6 * frame;
  int    data_size = sizeof(float) * 6 * anime->bufSize;
  void* data       = GFL_HEAP_AllocMemoryLo( anime->heapID, data_size );

  // �w��t���[���ʒu����, ���t���[�������̃f�[�^���擾
  GFL_ARC_LoadDataOfsByHandle( anime->arcHandle, anime->datID, ofs, data_size, data );
  for( i=0, j=0; i<anime->bufSize; i++ )
  {
    //OBATA_Printf( "rotate = %d, %d, %d\n", (int)data[j+0], (int)data[j+1], (int)data[j+2] );
    //OBATA_Printf( "trans  = %d, %d, %d\n", (int)data[j+3], (int)data[j+4], (int)data[j+5] );
    anime->rotateBuf[i].x = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    anime->rotateBuf[i].y = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    anime->rotateBuf[i].z = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    anime->transBuf[i].x = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    anime->transBuf[i].y = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    anime->transBuf[i].z = FX_F32_TO_FX32( *( (float*)( (int)data + j*sizeof(float) ) ) );   j++;
    /*
    VEC_Set( &anime->rotateBuf[i], 
        FX_F32_TO_FX32(data[j++]),
        FX_F32_TO_FX32(data[j++]),
        FX_F32_TO_FX32(data[j++]) );

    VEC_Set( &anime->transBuf[i], 
        FX_F32_TO_FX32(data[j++]),
        FX_F32_TO_FX32(data[j++]),
        FX_F32_TO_FX32(data[j++]) );
        */
  }
  GFL_HEAP_FreeMemory( data );

  // �o�b�t�@�J�n�t���[���ʒu���L��
  anime->bufStartFrame = start_frame >> FX32_SHIFT;

  OBATA_Printf( "UpdateBuf\n" );
}
