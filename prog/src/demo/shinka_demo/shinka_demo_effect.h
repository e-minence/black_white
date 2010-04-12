//============================================================================
/**
 *  @file   shinka_demo_effect.h
 *  @brief  �i���f���̃p�[�e�B�N���Ɣw�i
 *  @author Koji Kawada
 *  @data   2010.04.09
 *  @note   
 *
 *  ���W���[�����FSHINKADEMO_EFFECT
 */
//============================================================================
#pragma once

// lib
#include <gflib.h>

// system
#include "gamesystem/gamesystem.h"

//=============================================================================
/**
 *  �萔��`
 */
//=============================================================================
//-------------------------------------
/// �N�����@
//=====================================
typedef enum
{
  SHINKADEMO_EFFECT_LAUNCH_EVO,        ///< �i������Ƃ��납��X�^�[�g
  SHINKADEMO_EFFECT_LAUNCH_AFTER,      ///< �i���ォ��X�^�[�g
}
SHINKADEMO_EFFECT_LAUNCH;

//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
typedef struct _SHINKADEMO_EFFECT_WORK SHINKADEMO_EFFECT_WORK;

//=============================================================================
/**
 *  �֐��錾
 */
//=============================================================================
//-------------------------------------
/// ����������
//=====================================
extern SHINKADEMO_EFFECT_WORK* SHINKADEMO_EFFECT_Init(
                               HEAPID                     heap_id,
                               SHINKADEMO_EFFECT_LAUNCH   launch
                             );

//-------------------------------------
/// �I������
//=====================================
extern void SHINKADEMO_EFFECT_Exit( SHINKADEMO_EFFECT_WORK* work );

//-------------------------------------
/// �又��
//=====================================
extern void SHINKADEMO_EFFECT_Main( SHINKADEMO_EFFECT_WORK* work );

//-------------------------------------
/// �`�揈��
//      GRAPHIC_3D_StartDraw��GRAPHIC_3D_EndDraw�̊ԂŌĂԂ���
//=====================================
extern void SHINKADEMO_EFFECT_Draw( SHINKADEMO_EFFECT_WORK* work );

//-------------------------------------
/// �i���L�����Z��
//=====================================
extern void SHINKADEMO_EFFECT_Cancel( SHINKADEMO_EFFECT_WORK* work );

//-------------------------------------
/// �X�^�[�g
//=====================================
extern void SHINKADEMO_EFFECT_Start( SHINKADEMO_EFFECT_WORK* work );

