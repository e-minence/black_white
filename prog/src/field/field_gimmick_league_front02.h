////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �M�~�b�N����(�|�P�������[�O�t�����g02)
 * @file   field_gimmick_league_front.h
 * @author obata
 * @date   2009.12.01
 */
//////////////////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>
#include "field/fieldmap_proc.h"


//==========================================================================================
// ���g���I�u�W�F�N�g
//==========================================================================================
//----------------------------------------
// �A�j���[�V����(���t�g)
//----------------------------------------
typedef enum {
  LIFT_ANM_TA, // �e�N�X�`���A�j���[�V����
  LIFT_ANM_NUM
} LIFT_ANM_INDEX;
//----------------------------------------
// �A�j���[�V����(���t�g�̉ғ��G�t�F�N�g)
//----------------------------------------
typedef enum {
  LIFT_EFFECT_ANM_TA, // �e�N�X�`���A�j���[�V����
  LIFT_EFFECT_ANM_MA, // �}�e���A���A�j���[�V����
  LIFT_EFFECT_ANM_NUM
} LIFT_EFFECT_ANM_INDEX;
//-------------
// �I�u�W�F�N�g
//-------------
typedef enum {
  LF02_EXOBJ_LIFT,         // ���t�g
  LF02_EXOBJ_LIFT_EFFECT,  // ���t�g�̉ғ��G�t�F�N�g
  LF02_EXOBJ_NUM
} OBJ_INDEX;
//----------
// ���j�b�g
//----------
typedef enum {
  LF02_EXUNIT_GIMMICK,
  LF02_EXUNIT_NUM
} UNIT_INDEX;


//==========================================================================================
// ���M�~�b�N�o�^�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
extern void LEAGUE_FRONT_02_GIMMICK_Setup( FIELDMAP_WORK* fieldmap );

//------------------------------------------------------------------------------------------
/**
 * @brief �I���֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
extern void LEAGUE_FRONT_02_GIMMICK_End( FIELDMAP_WORK* fieldmap );

//------------------------------------------------------------------------------------------
/**
 * @brief ����֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
extern void LEAGUE_FRONT_02_GIMMICK_Move( FIELDMAP_WORK* fieldmap );


//==========================================================================================
// ���M�~�b�N����֐�
//==========================================================================================

extern void LEAGUE_FRONT_02_GIMMICK_StartLiftAnime( FIELDMAP_WORK* fieldmap );
extern void LEAGUE_FRONT_02_GIMMICK_StopLiftAnime( FIELDMAP_WORK* fieldmap );
extern void LEAGUE_FRONT_02_GIMMICK_ShowLiftEffect( FIELDMAP_WORK* fieldmap );
extern void LEAGUE_FRONT_02_GIMMICK_HideLiftEffect( FIELDMAP_WORK* fieldmap );
