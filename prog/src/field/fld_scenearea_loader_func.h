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
	FLD_SCENEAREA_AREACHECK_CIRCLE,   // �~�`�`�`�F�b�N
	FLD_SCENEAREA_AREACHECK_GRID_RECT, // �O���b�h��`�`�F�b�N

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
	FLD_SCENEAREA_UPDATE_ANGLECHANGE,  // �X���[�Y�A���O���ύX
	FLD_SCENEAREA_UPDATE_ANGLECHANGE_IN,    // �X���[�Y�A���O�������
	FLD_SCENEAREA_UPDATE_ANGLECHANGE_OUT,   // �X���[�Y�A���O���o��
	FLD_SCENEAREA_UPDATE_ANGLEOFFSCHANGE,       // �X���[�Y�A���O���E�I�t�Z�b�g�ύX
	FLD_SCENEAREA_UPDATE_ANGLEOFFSCHANGE_IN,    // �X���[�Y�A���O���E�I�t�Z�b�g�����
	FLD_SCENEAREA_UPDATE_ANGLEOFFSCHANGE_OUT,   // �X���[�Y�A���O���E�I�t�Z�b�g�o��

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

//-------------------------------------
/// �O���b�h�A�@�J�����X���[�Y�ړ�
//
//�Ή��֐�
//FLD_SCENEAREA_AREACHECK_GRID
//FLD_SCENEAREA_UPDATE_ANGLECHANGE
//FLD_SCENEAREA_UPDATE_ANGLECHANGE_IN
//FLD_SCENEAREA_UPDATE_ANGLECHANGE_OUT
//=====================================
typedef struct {

  // �O���b�h������
  u16 grid_x;     // ������O���b�hX
  u16 grid_z;     // ������O���b�hZ
  u16 grid_sizx;  // ������O���b�h�T�C�YX
  u16 grid_sizz;  // ������O���b�h�T�C�YZ
  u16 grid_depth; // ���s��
  u16 pad;        // �p�f�B���O
  
  u16 start_pitch;// �J�n�s�b�`
  u16 start_yaw;// �J�n���[
  fx32 start_length;// �J�n�����O�X
  u16 end_pitch;// �I���s�b�`
  u16 end_yaw;// �I�����[
  fx32 end_length;// �I�������O�X
  u32 trace_off; //TRACEOFF�ݒ�

} FLD_SCENEAREA_GRIDCHANGEANGLE_PARAM;

//-------------------------------------
/// �O���b�h�A�@�J�����X���[�Y�ړ� �J�����I�t�Z�b�g�t��
//
//�Ή��֐�
//FLD_SCENEAREA_AREACHECK_GRID
//FLD_SCENEAREA_UPDATE_ANGLEOFFSCHANGE
//FLD_SCENEAREA_UPDATE_ANGLEOFFSCHANGE_IN
//FLD_SCENEAREA_UPDATE_ANGLEOFFSCHANGE_OUT
//=====================================
typedef struct {

  // �O���b�h������
  u16 grid_x;     // ������O���b�hX
  u16 grid_z;     // ������O���b�hZ
  u16 grid_sizx;  // ������O���b�h�T�C�YX
  u16 grid_sizz;  // ������O���b�h�T�C�YZ
  u16 grid_depth; // ���s��
  u16 pad;        // �p�f�B���O
  
  // �ړ��p�����[�^
  u16 start_pitch;// �J�n�s�b�`
  u16 start_yaw;// �J�n���[
  fx32 start_length;// �J�n�����O�X
  u16 end_pitch;// �I���s�b�`
  u16 end_yaw;// �I�����[
  fx32 end_length;// �I�������O�X
  u32 trace_off; //TRACEOFF�ݒ�

  // TargetOffs
  fx32 start_offs_x;// �J�n�I�t�Z�b�g��
  fx32 start_offs_y;// �J�n�I�t�Z�b�g��
  fx32 start_offs_z;// �J�n�I�t�Z�b�g��
  fx32 end_offs_x;// �I���I�t�Z�b�g��
  fx32 end_offs_y;// �I���I�t�Z�b�g��
  fx32 end_offs_z;// �I���I�t�Z�b�g��

  // fovy
  u16 start_fovy; // �J�n fovy
  u16 end_fovy;   // �I�� fovy

} FLD_SCENEAREA_GRIDCHANGEANGLEOFFS_PARAM;


#ifdef _cplusplus
}	// extern "C"{
#endif



