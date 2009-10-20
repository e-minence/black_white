//============================================================================================
/**
 * @file	field_camera_mv_param.h
 * @brief	�J�������`�ړ��p�����[�^
 *
 */
//============================================================================================

#pragma once

//�J�����̌��ݏ��i���`��ԗp�j
typedef struct FLD_CAM_MV_PARAM_CORE_tag
{
  //�J�������W
  VecFx32 CamPos;
  //�^�[�Q�b�g���W
  VecFx32 TrgtPos;
  //�A���O���i���[�h�ˑ��j
  u16 AnglePitch;
  u16 AngleYaw;
  //�J�����ƃ^�[�Q�b�g�̋���
  fx32 Distance;
  //����p
  u16 Fovy;
  u16 dummy;
}FLD_CAM_MV_PARAM_CORE;

typedef struct FLD_CAM_MV_PARAM_CHK_tag
{
  BOOL Shift;
  BOOL Angle;
  BOOL Dist;
  BOOL Fovy;
}FLD_CAM_MV_PARAM_CHK;

typedef struct FLD_CAM_MV_PARAM_tag
{
  FLD_CAM_MV_PARAM_CORE Core;
  //�L���p�����[�^�w��
  FLD_CAM_MV_PARAM_CHK Chk;
}FLD_CAM_MV_PARAM;
