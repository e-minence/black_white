//============================================================================
/**
 *
 *	@file		demo3d.h
 *	@brief  3D�f���Đ��A�v��
 *	@author	genya hosaka -> miyuki iwasawa
 *	@data		2009.11.27
 *
 */
//============================================================================
#pragma once

//	lib
#include <gflib.h>

//system
#include "gamesystem/gamesystem.h"

#include "demo/demo3d_demoid.h" // DEMO3D_ID(�R���o�[�^���琶��)

// typedef�錾
typedef u32 DEMO3D_ID;

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
extern const GFL_PROC_DATA Demo3DProcData;

//=============================================================================
/**
 *								�萔��`
 */
//=============================================================================

//=============================================================================
/**
 *								�\���̒�`
 */
//=============================================================================

typedef enum
{ 
  DEMO3D_RESULT_NULL = 0,
  DEMO3D_RESULT_USER_END, ///< ���[�U�[�ɂ��I��
  DEMO3D_RESULT_FINISH,   ///< �f���̏I��
} DEMO3D_RESULT;

//-------------------------------------
///	PROC�ɓn������
//=====================================
typedef struct {	
  // [IN]
  DEMO3D_ID     demo_id;      ///< �f��ID
  u32           start_frame;  ///< �f���J�n�t���[��(1sync=1)

  u8            hour;         ///<�Ăяo������(��)
  u8            min;          ///<�Ăяo������(��)
  u8            season;       ///<�G��

  // [OUT]
  u32           end_frame;    ///< �f���I���t���[��(1sync=1)
  DEMO3D_RESULT result;       ///< �f���I���^�C�v

} DEMO3D_PARAM;

FS_EXTERN_OVERLAY(demo3d);

/*
 *  @brief  demo3d�Ăяo���p�����[�^�Z�b�g(�풓�֐��ł�)
 *
 *  @param  pp      DEMO3D_PARAM�^�\���̂ւ̃|�C���^
 *  @param  demo_id �Ăяo���f��ID DEMO3D_ID_NULL�` include/demo/demo3d_demoid.h 
 *  @param  s_frame �f���̃X�^�[�g�t���[���w��
 *  @param  t_hour  �f���̃��C�g�ݒ�Ɏg�����Ԏw��(��)
 *  @param  t_min   �f���̃��C�g�ݒ�Ɏg�����Ԏw��(��)
 *  @param  season  �f���̃��C�g�ݒ�Ɏg���G�� PMSEASON_SPRING�` include/gamesystem/pm_season.h
 */
extern void DEMO3D_PARAM_Set( DEMO3D_PARAM* pp, u8 demo_id, u32 s_frame, u8 t_hour, u8 t_min, u8 season );

/*
 *  @brief  demo3d�Ăяo���p�����[�^�Z�b�g(RTC�ˑ�)
 *
 *  @param  pp      DEMO3D_PARAM�^�\���̂ւ̃|�C���^
 *  @param  demo_id �Ăяo���f��ID DEMO3D_ID_NULL�` include/demo/demo3d_demoid.h 
 *  @param  s_frame �f���̃X�^�[�g�t���[���w��
 *
 *  �f���̃��C�g�ݒ�Ɏg�����ԁE�G�߂����A���^�C���N���b�N����Z�o���܂�
 */
extern void DEMO3D_PARAM_SetFromRTC( DEMO3D_PARAM* pp, u8 demo_id, u32 s_frame );

/*
 *  @brief  demo3d�Ăяo���p�����[�^�Z�b�g(EV-TIME�ˑ�)
 *
 *  @param  pp      DEMO3D_PARAM�^�\���̂ւ̃|�C���^
 *  @param  demo_id �Ăяo���f��ID DEMO3D_ID_NULL�` include/demo/demo3d_demoid.h 
 *  @param  s_frame �f���̃X�^�[�g�t���[���w��
 *  @param  gdata   GAMEDATA�̎Q�ƃ|�C���^
 *
 *  �f���̃��C�g�ݒ�Ɏg�����ԁE�G�߂�EV-TIME����Z�o���܂�
 */
extern void DEMO3D_PARAM_SetFromEvTime( DEMO3D_PARAM* pp, u8 demo_id, u32 s_frame, const GAMEDATA* gdata );

