//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		fld_scenearea_loader_func.h
 *	@brief	�V�[���G���A�@���[�_�[�@�֐�ID
 *	@author	tomoya takahashi
 *	@date		2009.07.09
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif


//-----------------------------------------------------------------------------
/**
 *					�͈̓`�F�b�N�֐�
*/
//-----------------------------------------------------------------------------
enum
{
	// scenearea_conv.pl�p�R�����g�@�C�����Ȃ��ŁB
	//AREACHECK_FUNC_ID_START
	FLD_SCENEAREA_AREACHECK_CIRCLE,

	FLD_SCENEAREA_AREACHECK_MAX,
};

//-----------------------------------------------------------------------------
/**
 *					���X�V�֐�
*/
//-----------------------------------------------------------------------------
enum
{
	// scenearea_conv.pl�p�R�����g�@�C�����Ȃ��ŁB
	//UPDATE_FUNC_ID_START
	FLD_SCENEAREA_UPDATE_CIRCLE,
	FLD_SCENEAREA_UPDATE_FXCAMERA,

	FLD_SCENEAREA_UPDATE_MAX,
};


//-----------------------------------------------------------------------------
/**
 *					�p�����[�^���
*/
//-----------------------------------------------------------------------------
//-------------------------------------
/// ��]�ړ���{	
//
//�Ή��֐�
//FLD_SCENEAREA_AREACHECK_CIRCLE
//FLD_SCENEAREA_UPDATE_CIRCLE
//=====================================
typedef struct {
  u32   rot_start;
  u32   rot_end;
  fx32  dist_min;
  fx32  dist_max;

	// �J�������S
  fx32  center_x;	
  fx32  center_y;
  fx32  center_z;

  u32   pitch;
  fx32  length;

} FLD_SCENEAREA_CIRCLE_PARAM;

//-------------------------------------
/// ��]�ړ� FIXCAMERA
//
//�Ή��֐�
//FLD_SCENEAREA_AREACHECK_CIRCLE
//FLD_SCENEAREA_UPDATE_CIRCLE
//FLD_SCENEAREA_UPDATE_FXCAMERA,
//=====================================
typedef struct {
  u32   rot_start;
  u32   rot_end;
  fx32  dist_min;
  fx32  dist_max;

	// �J�������S
  fx32  center_x;	
  fx32  center_y;
  fx32  center_z;

  u32   pitch;
  fx32  length;

  // �Œ�J�����ɕK�v�ȃp�����[�^
  fx32  target_x;
  fx32  target_y;
  fx32  target_z;
  fx32  camera_x;
  fx32  camera_y;
  fx32  camera_z;
} FLD_SCENEAREA_CIRCLE_FIXCAMERA_PARAM;


#ifdef _cplusplus
}	// extern "C"{
#endif



