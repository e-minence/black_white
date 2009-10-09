/////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  ica�A�j���[�V���� �J�������p�֐�
 * @file   ica_camera.c
 * @author obata
 * @date   2009.10.08
 *
 */
/////////////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/ica_anime.h"
#include "system/ica_camera.h"


//---------------------------------------------------------------------------
/**
 * @brief �J�������W�E�^�[�Q�b�g���W�����݃t���[���̏�Ԃɐݒ肷��
 *        anime �̎��� trans ���J�������W��,
 *                     rotate ���J�����̌����ɓK�p����.
 *
 * @param anime   �ݒ�A�j���[�V����
 * @param camera  �ݒ�Ώۂ̃J����
 */
//---------------------------------------------------------------------------
void ICA_CAMERA_SetCameraStatus( GFL_G3D_CAMERA* camera, const ICA_ANIME* anime )
{
  float x, y, z;
  u16 rx, ry, rz;
  MtxFx33 matrix;
  VecFx32 rotate;
  VecFx32 def_forward = { 0, 0, -FX32_ONE };
  VecFx32 def_upward  = { 0, FX32_ONE, 0 }; 
  VecFx32 pos, target, forward, upward;

  // ��]�s����쐬
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

  // �x�N�g����]�ŃJ�����̌������Z�o
  MTX_MultVec33( &def_forward, &matrix, &forward );
  MTX_MultVec33( &def_upward, &matrix, &upward );

  // �J�����E�����_�ʒu�����߂�
  ICA_ANIME_GetTranslate( anime, &pos );
  VEC_Add( &pos, &forward, &target );

  // �J�����̐ݒ�
  GFL_G3D_CAMERA_SetPos( camera, &pos );
  GFL_G3D_CAMERA_SetTarget( camera, &target );
  GFL_G3D_CAMERA_SetCamUp( camera, &upward );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �J�������W�����݃t���[���̏�Ԃɐݒ肷��
 *        anime �̎��� trans ���J�������W�ɔ��f������
 *
 * @param camera  �ݒ�Ώۂ̃J����
 * @param anime   �ݒ�A�j���[�V����
 */
//-----------------------------------------------------------------------------------
void ICA_CAMERA_SetCameraPos( GFL_G3D_CAMERA* camera, const ICA_ANIME* anime )
{
  VecFx32 pos;

  // �J�����ʒu�����߂�
  ICA_ANIME_GetTranslate( anime, &pos );

  // �J�����̐ݒ�
  GFL_G3D_CAMERA_SetPos( camera, &pos );
}

//-----------------------------------------------------------------------------------
/**
 * @brief �����_���W�����݃t���[���̏�Ԃɐݒ肷��
 *        anime �̎��� trans �𒍎��_���W�ɔ��f������
 *
 * @param camera  �ݒ�Ώۂ̃J����
 * @param anime   �ݒ�A�j���[�V����
 */
//-----------------------------------------------------------------------------------
void ICA_CAMERA_SetTargetPos( GFL_G3D_CAMERA* camera, const ICA_ANIME* anime )
{
  VecFx32 pos;

  // �����_�ʒu�����߂�
  ICA_ANIME_GetTranslate( anime, &pos );

  // �J�����̐ݒ�
  GFL_G3D_CAMERA_SetTarget( camera, &pos );
}
