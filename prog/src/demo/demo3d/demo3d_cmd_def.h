//============================================================================
/**
 *
 *	@file		demo3d_cmd_def.h
 *	@brief  3D�f���R�}���h �R�}���h��`
 *	@author		hosaka genya
 *	@data		2009.12.09
 *
 */
//============================================================================
#pragma once

//--------------------------------------------------------------
///	3D�f���R�}���h��`
//==============================================================
typedef enum
{ 
//======================================================

//======================================================
  DEMO3D_CMD_TYPE_NULL = 0, ///< �R�}���h�Ȃ�

//======================================================
/**
 *	@brief  SE�Đ��R�}���h
 *
 *	@param	param[0] SE_Label
 *	@param	param[1] volume 0�`127 : DEMO3D_SE_PARAM_DEFAULT�Ȃ疳��
 *	@param	param[2] pan -128�`127 �FDEMO3D_SE_PARAM_DEFAULT�Ȃ疳��
 *	@param	param[3] pitch -32768�`32767(+/-64�Ŕ���)�FDEMO3D_SE_PARAM_DEFAULT�Ȃ疳��
 *	@param	param[4] playerNo 0�`3 : DEMO3D_SE_PARAM_DEFAULT�Ȃ疳��
 */
//======================================================
  DEMO3D_CMD_TYPE_SE_PLAY,

//======================================================
/**
 *	@brief  SE:�Đ���~
 *
 *	@param	param[0] SE_Label
 *	@param	param[1] playerNo 0�`3 : DEMO3D_SE_PARAM_DEFAULT�Ȃ疳��
 */
//======================================================
  DEMO3D_CMD_TYPE_SE_STOP,

//======================================================
/**
 *	@brief  SE�̃{�����[�����I�ω����N�G�X�g
 *
 *  @param  param[0]  SE_Label
 *  @param  param[1]  �X�^�[�g�̒l(0�`127) 
 *  @param  param[2]  �G���h�̒l(0�`127)
 *  @param  param[3]  �X�^�[�g�`�G���h�܂ł̃t���[����(1�`)
 *  @param  param[4]  �v���C���[No(0�`3 �f�t�H���g�ł�������DEMO3D_SE_PARAM_DEFAULT)
 */
//======================================================
  DEMO3D_CMD_TYPE_SE_VOLUME_EFFECT_REQ,

//======================================================
/**
 *	@brief  SE�̃p�����I�ω����N�G�X�g
 *
 *  @param  param[0]  SE_Label
 *  @param  param[1]  �X�^�[�g�̒l(-128�`127) 
 *  @param  param[2]  �G���h�̒l(-128�`127)
 *  @param  param[3]  �X�^�[�g�`�G���h�܂ł̃t���[����(1�`)
 *  @param  param[4]  �v���C���[No(0�`3 �f�t�H���g�ł�������DEMO3D_SE_PARAM_DEFAULT)
 */
//======================================================
  DEMO3D_CMD_TYPE_SE_PAN_EFFECT_REQ,

//======================================================
/**
 *	@brief  SE�̃s�b�`���I�ω����N�G�X�g
 *
 *  @param  param[0]  SE_Label
 *  @param  param[1]  �X�^�[�g�̒l(-32768�`32767 +/-64�Ŕ���) 
 *  @param  param[2]  �G���h�̒l(-32768�`32767 +/-64�Ŕ���) 
 *  @param  param[3]  �X�^�[�g�`�G���h�܂ł̃t���[����(1�`)
 *  @param  param[4]  �v���C���[No(0�`3 �f�t�H���g�ł�������DEMO3D_SE_PARAM_DEFAULT)
 */
//======================================================
  DEMO3D_CMD_TYPE_SE_PITCH_EFFECT_REQ,

//======================================================
/**
 *	@brief  BGM�Đ� 
 *
 *	@param	param[0] BGM_Label
 */
//======================================================
  DEMO3D_CMD_TYPE_BGM_PLAY,

//======================================================
/**
 *	@brief  BGM�X�g�b�v
 *
 *	@param  none	
 */
//======================================================
  DEMO3D_CMD_TYPE_BGM_STOP,

//======================================================
/**
 *	@brief  BGM�t�F�[�h
 *
 *	@param  param[0]	�t�F�[�h�p�^�[��(0:fadein,1:fadeout) 
 *  @param  param[1]  ��frame�Ńt�F�[�h���邩�H
 */
//======================================================
  DEMO3D_CMD_TYPE_BGM_FADE,

//======================================================
/**
 *	@brief  BGM�ύX���N�G�X�g
 *
 *  @param  param[0]  BGM-No(0�`65535)
 *  @param  param[1]  fadeout_frame(0�`65535) 
 *  @param  param[2]  fadein_frame(0�`65535)
 *  @param  param[3]  type("play"=0,"stop"=1,"change"=2,"push"=3,"pop" =4)
 *
 *  type��DEMO3D_BGM_CHG_TYPE�^(demo3d_cmd_def.h)
 */
//======================================================
  DEMO3D_CMD_TYPE_BGM_CHANGE_REQ,

//======================================================
/**
 *	@brief  �P�x�ω��G�t�F�N�g��p�����u���C�g�l�X 
 *
 *  @param  param[0]  �K�p������ < "main"(1),"sub"(2),"double"(3) >
 *  @param  param[1]  ��frame�Ńt�F�[�h���邩�H (0�͊J�n���P�x�l�𑦎����f)
 *  @param  param[2]  �t�F�[�h�J�n���̋P�x < -16�`16(���`��) >
 *  @param  param[3]  �t�F�[�h�I�����̋P�x < -16�`16(���`��) >
 */
//======================================================
  DEMO3D_CMD_TYPE_BRIGHTNESS_REQ,

//======================================================
/**
 *	@brief  �P�x�ω��G�t�F�N�g��p�����t���b�V�����N�G�X�g
 *
 *  @param  param[0]  �K�p������ < "main"(1),"sub"(2),"double"(3) >
 *  @param  param[1]  start�̋P�x < -16�`16(���`��) >
 *  @param  param[2]  max�P�x     < -16�`16(���`��) >
 *  @param  param[3]  end�̋P�x   < -16�`16(���`��) >
 *  @param  param[4]  start����max�܂ł̃t���[�� < 1�` >
 *  @param  param[5]  max����end�܂ł̃t���[�� < 1�` >
 *
 */
//======================================================
  DEMO3D_CMD_TYPE_FLASH_REQ,

//======================================================
/**
 *	@brief  ���C�g�J���[�ύX 
 *
 *	@param	param[0]  light_no(0�`3)
 *	@param  param[1]  light_col_r(0�`31)
 *	@param  param[2]  light_col_g(0�`31)
 *	@param  param[3]  light_col_b(0�`31)
 */
//======================================================
  DEMO3D_CMD_TYPE_LIGHT_COLOR_SET,

//======================================================
/**
 *	@brief  ���C�g�x�N�g���ύX 
 *
 *	@param	param[0]  light_no(0�`3)
 *	@param  param[1]  light_vec_x(fx16 -7.9�`7.9)
 *	@param  param[2]  light_vec_y(fx16 -7.9�`7.9)
 *	@param  param[3]  light_vec_z(fx16 -7.9�`7.9)
 */
//======================================================
  DEMO3D_CMD_TYPE_LIGHT_VECTOR_SET,

//======================================================
/**
 *	@brief   ��b�E�B���h�E�\��
 *
 *	@param	param[0]  ���b�Z�[�W�A�[�J�C�uID 
 *	@param	param[1]  ���b�Z�[�W���x��
 *	@param	param[2]  ���b�Z�[�W�Z�b�g��( ���ۂɕ\������郁�b�Z�[�WID = param[2]*�V�[��ID+param[1])
 *	@param	param[3]  �E�B���h�E��\������t���[����(1�`)
 *	@param	param[4]  �E�B���h�E����X���W(1�`30) 
 *	@param	param[5]  �E�B���h�E����Y���W(1�`21)
 *	@param	param[6]  �E�B���h�E�T�C�YX(2�`28)
 *	@param	param[7]  �E�B���h�E�T�C�YY(2�`22) 
 */
//======================================================
  DEMO3D_CMD_TYPE_TALKWIN_REQ,

//======================================================
/**
 *	@brief   BG�ꖇ�G�\��
 *
 *  @param  param[0]  �K�p������ < "ver_all"(0xFF),"ver_black"(VERSION_BLACK),"ver_white"(VERSION_WHITE),"ver_gray"(VERSION_WHITE+1) >
 *	@param	param[1]  �p���b�g�A�[�J�C�uID 
 *	@param	param[2]  �L�����N�^�A�[�J�C�uID
 *	@param	param[3]  �X�N���[���A�[�J�C�uID
 *	@param	param[4]  �E�B���h�E��\������t���[����(1�`)
 *	@param	param[5]  ���t�F�[�h�C���t���[���� 
 *	@param	param[6]  ���t�F�[�h�A�E�g�t���[����
 */
//======================================================
  DEMO3D_CMD_TYPE_SCR_DRAW_REQ,

//======================================================
/**
 *	@brief  ���[�V�����u���[ �J�n
 *
 *	@param	wk
 *	@param	param[0] ���[�V�����u���[�W�� �V�����u�����h����G�̃��W��
 *	@param	param[1] ���[�V�����u���[�W�� ���Ƀo�b�t�@����Ă���G�̃��W��
 *
 *  @com  2��ʃ��[�h�ł͎g���܂���
 */
//======================================================
  DEMO3D_CMD_TYPE_MOTIONBL_START,

//======================================================
/**
 *	@brief  ���[�V�����u���[ ��~
 *
 *  @com  2��ʃ��[�h�ł͎g���܂���
 */
//======================================================
  DEMO3D_CMD_TYPE_MOTIONBL_END,

  
  DEMO3D_CMD_TYPE_END, ///< �R�}���h�I��
  DEMO3D_CMD_TYPE_MAX,
} DEMO3D_CMD_TYPE;

#define DEMO3D_CMD_PARAM_MAX (8)  ///< �������p�����[�^�̍ő�l
#define DEMO3D_CMD_PARAM_NULL (0) ///< �����p�����[�^�̏������l
#define DEMO3D_CMD_SYNC_INIT (-1)     ///< frame�ɂ��̒l�������Ă����珉�����R�}���h�Ƃ݂Ȃ�

//SE_PLAY
#define DEMO3D_SE_PARAM_DEFAULT (0xFFFFFFFF)

typedef enum{
  DEMO3D_SE_EFF_VOLUME,
  DEMO3D_SE_EFF_PAN,
  DEMO3D_SE_EFF_PITCH,
}DEMO3D_SE_EFFECT;

typedef enum{
  DEMO3D_BGM_PLAY,
  DEMO3D_BGM_STOP,
  DEMO3D_BGM_CHANGE,
  DEMO3D_BGM_PUSH,
  DEMO3D_BGM_POP,
}DEMO3D_BGM_CHG_TYPE;

//--------------------------------------------------------------
///	3D�f���R�}���h�f�[�^
//==============================================================
typedef struct {
  DEMO3D_CMD_TYPE   type;     ///< �R�}���h�^�C�v
  int               frame;    ///< �Đ��t���[��
  int               param[ DEMO3D_CMD_PARAM_MAX ]; ///< �������p�����[�^
} DEMO3D_CMD_DATA;

