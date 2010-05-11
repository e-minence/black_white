//============================================================================
/**
 *  @file   manual_touchbar.h
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *
 *  ���W���[�����FMANUAL_TOUCHBAR
 */
//============================================================================
#pragma once


// �C���N���[�h
#include <gflib.h>

#include "gamesystem/gamedata_def.h"


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// �^�C�v
typedef enum
{
  MANUAL_TOUCHBAR_TYPE_TOP,
  MANUAL_TOUCHBAR_TYPE_CATEGORY,
  MANUAL_TOUCHBAR_TYPE_TITLE,
  MANUAL_TOUCHBAR_TYPE_EXPLAIN,
  MANUAL_TOUCHBAR_TYPE_MAX,

  MANUAL_TOUCHBAR_TYPE_INIT        = MANUAL_TOUCHBAR_TYPE_MAX,      // �ŏ���1��̂�
}
MANUAL_TOUCHBAR_TYPE;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
//=====================================
typedef struct _MANUAL_TOUCHBAR_WORK MANUAL_TOUCHBAR_WORK;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// ����������
extern  MANUAL_TOUCHBAR_WORK*  MANUAL_TOUCHBAR_Init(
    void*                    cmn_wk  // �L���X�g(MANUAL_COMMON_WORK*)���Ďg�p
);
// �I������
extern  void  MANUAL_TOUCHBAR_Exit(
    MANUAL_TOUCHBAR_WORK*     work
);
// �又��
extern  void  MANUAL_TOUCHBAR_Main(
    MANUAL_TOUCHBAR_WORK*     work
);

// �^�C�v�ݒ�
extern void MANUAL_TOUCHBAR_SetType(
    MANUAL_TOUCHBAR_WORK*     work,
    MANUAL_TOUCHBAR_TYPE      type
);

// �y�[�W�ݒ�(MANUAL_TOUCHBAR_TYPE_EXPLAIN�̂Ƃ��̂ݗL��)
extern void MANUAL_TOUCHBAR_SetPage(
    MANUAL_TOUCHBAR_WORK*     work,
    u8                        page_num,   // �y�[�W��(0�̂Ƃ��y�[�W�\���Ȃ�)
    u8                        page_no     // ���݂̃y�[�W(0<=page_no<page_num)
);

// TOUCHBAR��킹�������̊֐�
extern TOUCHBAR_ICON MANUAL_TOUCHBAR_GetTrg(
    MANUAL_TOUCHBAR_WORK*     work
);
extern TOUCHBAR_ICON MANUAL_TOUCHBAR_GetTouch(
    MANUAL_TOUCHBAR_WORK*     work
);

