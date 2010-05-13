//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		field_camera_local.h
 *	@brief  �t�B�[���h�J�����O������J�@��`
 *	@author	tomoya takahashi
 *	@date		2010.04.19
 *
 *	���W���[�����F
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#pragma once

#ifdef _cplusplus
extern "C"{
#endif

#include <gflib.h>

#ifdef PM_DEBUG
// �J�����p�����[�^�@�f�o�b�N�\���p
#include "message.naix"

#include "print/wordset.h"
#include "print/gf_font.h"
#include "print/printsys.h"

#include "fld_wipe_3dobj.h"

#include "msg/msg_d_tomoya.h"

#include "font/font.naix"


//#define DEBUG_CAMERA_ALLCONTROL_MODE_2  // ����́A���@�Ƃ̃o�C���h���J�b�g�����Ƃ��ɁA�^�[�Q�b�g�I�t�Z�b�g��ύX���Ȃ����[�h



#endif

//============================================================================================
//============================================================================================

#define FIELD_CAMERA_DELAY	(6)
#define FIELD_CAMERA_TRACE_BUFF	(FIELD_CAMERA_DELAY+1)

/*---------------------------------------------------------------------------*
	�J�����g���[�X�\����
 *---------------------------------------------------------------------------*/
typedef struct 
{
	int bufsize;
	int CamPoint;
	int TargetPoint;
	int Delay;			//�x������
	BOOL UpdateFlg;		//�X�V�J�n�t���O
	BOOL ValidX;		//X�x����L���ɂ��邩�̃t���O
	BOOL ValidY;		//Y�x����L���ɂ��邩�̃t���O
	BOOL ValidZ;		//Z�x����L���ɂ��邩�̃t���O

  VecFx32 * targetBuffer;
  VecFx32 * camPosBuffer;

  BOOL Valid;
  BOOL StopReq;

}CAMERA_TRACE;

//------------------------------------------------------------------
/**
 * @brief	�t�B�[���h�J�����^�C�v
 */
//------------------------------------------------------------------
typedef enum {
	FIELD_CAMERA_TYPE_GRID,
	FIELD_CAMERA_TYPE_H01,
	FIELD_CAMERA_TYPE_C03,

  FIELD_CAMERA_TYPE_POKECEN,
  FIELD_CAMERA_TYPE_H01P01,

  FIELD_CAMERA_TYPE_PLP01,

	FIELD_CAMERA_TYPE_MAX,
}FIELD_CAMERA_TYPE;


//-----------------------------------------------------------------------------
/**
 *			�J�������͈�
 */
//-----------------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief	�J�������`�ړ��Ǘ��\����
 */
//------------------------------------------------------------------

//�J�����̕��A���
typedef struct FLD_CAM_PUSH_PARAM_tag
{
  FIELD_CAMERA_MODE BeforeMode;   //�v�b�V������O�̃��[�h
//  FIELD_CAMERA_MODE PushMode;     //���A�����v�b�V������Ƃ��̃��[�h
  FLD_CAM_MV_PARAM_CORE RecvParam;  //���A���
}FLD_CAM_PUSH_PARAM;

typedef struct FLD_MOVE_CAM_DATA_tag
{
  FLD_CAM_PUSH_PARAM PushParam;

  FLD_CAM_MV_PARAM_CORE SrcParam;   //�ړ��O�J�������
  FLD_CAM_MV_PARAM_CORE DstParam;   //�ړ���J�������
  u16 CostFrm;              //�ړ��ɂ�����t���[����
  u16 NowFrm;               //���݂̃t���[��
  BOOL PushFlg;
  BOOL Valid;   //���`��Ԓ����H
  FLD_CAM_MV_PARAM_CHK Chk;
  void *CallBackWorkPtr;
}FLD_MOVE_CAM_DATA;

//------------------------------------------------------------------
/**
 * @brief	�^�錾
 */
//------------------------------------------------------------------
struct _FIELD_CAMERA {
	HEAPID				heapID;			///<�g�p����q�[�v�w��ID
  ARCHANDLE * init_param_handle;
  ARCHANDLE * camera_area_handle;
	GFL_G3D_CAMERA * g3Dcamera;			///<�J�����\���̂ւ̃|�C���^

	FIELD_CAMERA_TYPE	type;			///<�J�����̃^�C�v�w��
	FIELD_CAMERA_MODE mode;			///<�J�������[�h

  const VecFx32 * default_target;
	const VecFx32 *		watch_target;	///<�ǐ����钍���_�ւ̃|�C���^
  const VecFx32 *   watch_camera; ///<�ǐ�����J�����ʒu�ւ̃|�C���^

  VecFx32       camPos;             ///<�J�����ʒu�p���[�N
	VecFx32				campos_offset;			///<�J�����ʒu�p�␳���W
  VecFx32       campos_write;       ///<�J�����ʒu�p���ۂɉ�ʔ��f�����l

	VecFx32				target;			        ///<�����_�p���[�N
	VecFx32				target_write;			  ///<�����_�p���ۂɉ�ʔ��f�����l
	VecFx32				target_offset;			///<�����_�p�␳���W
  VecFx32       target_before;

  u16         angle_pitch;
  u16         angle_yaw;
  fx32        angle_len;

  u16         fovy;
  u16         global_angle_yaw;  ///<�J�������W�A�^�[�Q�b�g���W�v�Z��̃J�����A���O��Yaw�@�J�������[�h�ɂ����E���ꂸ�A���FIELD_CAMERA_MODE_CALC_CAMERA_POS�̃A���O����Ԃ�
  u16         global_angle_pitch;

  // �J�������͈�
  FIELD_CAMERA_AREA camera_area[FIELD_CAMERA_AREA_DATA_MAX];  
  u8                camera_area_num;
  u8                camera_area_active;
  u16               camera_area_id;


  CAMERA_TRACE * Trace;

  FLD_MOVE_CAM_DATA MoveData;
  CAMERA_CALL_BACK CallBack;
  
#ifdef PM_DEBUG
  // �f�o�b�N�p


  u16 debug_subscreen_type;
  u16 debug_trace_off;

  // �^�[�Q�b�g����p
  u16         debug_target_pitch;
  u16         debug_target_yaw;
  fx32        debug_target_len;
  VecFx32     debug_target;

  fx32        debug_far;

  u16 debug_save_camera_mode;
  u16 debug_save_buffer_mode;


  // �f�o�b�N�\���p
  WORDSET*    p_debug_wordset;
  GFL_FONT*   p_debug_font;
  GFL_MSGDATA*  p_debug_msgdata;
  STRBUF*     p_debug_strbuff;
  STRBUF*     p_debug_strbuff_tmp;

  // �f�o�b�N�R���g���[���p���[�N
  VecFx32 debug_camera;
  u32 debug_control_on;

  // ���C�v�\��
  FLD_WIPEOBJ* p_debug_wipe;
  
  // ��{�^�[�Q�b�g�I�t�Z�b�g
  VecFx32 debug_default_target_offs;
#endif
};


//-----------------------------------------------------------------------------
/**
 *			�������J�֐�
 */
//-----------------------------------------------------------------------------

extern void FIELD_CAMERA_LOCAL_calcAnglePos( const VecFx32* cp_target, VecFx32* p_pos, u16 yaw, u16 pitch, fx32 len );
extern void FIELD_CAMERA_LOCAL_modeChange_CalcVecAngel( const VecFx32* cp_vec, u16* p_pitch, u16* p_yaw, fx32* p_len );
extern void FIELD_CAMERA_LOCAL_ControlParameter( FIELD_CAMERA * camera, u16 key_cont );





#ifdef _cplusplus
}	// extern "C"{
#endif



