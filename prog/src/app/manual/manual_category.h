//============================================================================
/**
 *  @file   manual_category.h
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *
 *  ���W���[�����FMANUAL_CATEGORY
 */
//============================================================================
#pragma once


// �C���N���[�h
#include <gflib.h>

#include "gamesystem/gamedata_def.h"

#include "manual_common.h"


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
typedef enum
{
  MANUAL_CATEGORY_RESULT_RETURN,
  MANUAL_CATEGORY_RESULT_TITLE,
}
MANUAL_CATEGORY_RESULT;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// �p�����[�^
//=====================================
typedef struct
{
  // in,out
  u16                     head_pos;     // �S���ڂ̒��ł̔ԍ�(��ʂ�1�ԏ�(UP_HALF�͏���)�ɂ��鍀�ڂ̔ԍ�)  // �X�N���[���o�[�̃J�[�\���̈ʒu�͂���Ō��܂�
  u16                     cursor_pos;   // �S���ڂ̒��ł̔ԍ�
  // out 
  MANUAL_CATEGORY_RESULT  result;
  u16                     cate_no;      // result��MANUAL_CATEGORY_RESULT_TITLE�̂Ƃ��̂ݎg�p����
}
MANUAL_CATEGORY_PARAM;

//-------------------------------------
/// ���[�N
//=====================================
typedef struct _MANUAL_CATEGORY_WORK MANUAL_CATEGORY_WORK;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// ����������
extern  MANUAL_CATEGORY_WORK*  MANUAL_CATEGORY_Init(
    MANUAL_CATEGORY_PARAM*   param,
    MANUAL_COMMON_WORK*      cmn_wk
);
// �I������
extern  void  MANUAL_CATEGORY_Exit(
    MANUAL_CATEGORY_WORK*    work
);
// �又��(�I�������Ƃ�TRUE��Ԃ�)
extern  BOOL  MANUAL_CATEGORY_Main(
    MANUAL_CATEGORY_WORK*    work
);

