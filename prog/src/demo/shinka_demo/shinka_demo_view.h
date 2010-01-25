//============================================================================
/**
 *  @file   shinka_demo_view.h
 *  @brief  �i���f���̉��o
 *  @author Koji Kawada
 *  @data   2010.01.19
 *  @note   
 *
 *  ���W���[�����FSHINKADEMO_VIEW
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
  SHINKADEMO_VIEW_LAUNCH_EVO,        ///< �i������Ƃ��납��X�^�[�g
  SHINKADEMO_VIEW_LAUNCH_AFTER,      ///< �i���ォ��X�^�[�g
}
SHINKADEMO_VIEW_LAUNCH;

//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
typedef struct _SHINKADEMO_VIEW_WORK SHINKADEMO_VIEW_WORK;

//=============================================================================
/**
 *  �֐��錾
 */
//=============================================================================
//-------------------------------------
/// ����������
//=====================================
extern SHINKADEMO_VIEW_WORK* SHINKADEMO_VIEW_Init(
                               HEAPID                   heap_id,
                               SHINKADEMO_VIEW_LAUNCH   launch,        // EVO                | AFTER
                               const POKEMON_PARAM*     pp,            // �i���O�|�P����     | �i����|�P����
                               u16                      after_monsno   // �i����|�P����     | �g�p���Ȃ�
                             );

//-------------------------------------
/// �I������
//=====================================
extern void SHINKADEMO_VIEW_Exit( SHINKADEMO_VIEW_WORK* work );

//-------------------------------------
/// �又��
//=====================================
extern void SHINKADEMO_VIEW_Main( SHINKADEMO_VIEW_WORK* work );

//-------------------------------------
/// �`�揈��
//      GRAPHIC_3D_StartDraw��GRAPHIC_3D_EndDraw�̊ԂŌĂԂ���
//=====================================
extern void SHINKADEMO_VIEW_Draw( SHINKADEMO_VIEW_WORK* work );

//-------------------------------------
/// �i���L�����Z��
//=====================================
extern BOOL SHINKADEMO_VIEW_CancelShinka( SHINKADEMO_VIEW_WORK* work );

//-------------------------------------
/// �X�^�[�g
//=====================================
extern void SHINKADEMO_VIEW_StartShinka( SHINKADEMO_VIEW_WORK* work );

//-------------------------------------
/// BGM���Đ����Ă��悢��
//=====================================
extern BOOL SHINKADEMO_VIEW_IsBGMPlay( SHINKADEMO_VIEW_WORK* work );

//-------------------------------------
/// ������΂����o�̂��߂̃p���b�g�t�F�[�h�����ė~������(1�t���[������TRUE�ɂȂ�Ȃ�)
//=====================================
extern BOOL SHINKADEMO_VIEW_ToFromWhite( SHINKADEMO_VIEW_WORK* work );

//-------------------------------------
/// �I�����Ă��邩
//=====================================
extern BOOL SHINKADEMO_VIEW_IsEndShinka( SHINKADEMO_VIEW_WORK* work );
