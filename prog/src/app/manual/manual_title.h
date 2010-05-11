//============================================================================
/**
 *  @file   manual_title.h
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *
 *  ���W���[�����FMANUAL_TITLE
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
  MANUAL_TITLE_RESULT_RETURN,
  MANUAL_TITLE_RESULT_ITEM,
}
MANUAL_TITLE_RESULT;

typedef enum
{
  MANUAL_TITLE_TYPE_CATEGORY,
  MANUAL_TITLE_TYPE_ALL,
}
MANUAL_TITLE_TYPE;


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
  // in
  MANUAL_TITLE_TYPE    type;
  u16                  cate_no;      // type��MANUAL_TITLE_TYPE_CATEGORY�̂Ƃ��̂ݎg�p����
  // in,out
  u16                  head_pos;     // �S���ڂ̒��ł̔ԍ�(��ʂ�1�ԏ�(UP_HALF�͏���)�ɂ��鍀�ڂ̔ԍ�)  // �X�N���[���o�[�̃J�[�\���̈ʒu�͂���Ō��܂�
  u16                  cursor_pos;   // �S���ڂ̒��ł̔ԍ�
  // out 
  MANUAL_TITLE_RESULT  result;
  u16                  serial_no;    // result��MANUAL_TITLE_RESULT_ITEM�̂Ƃ��̂ݎg�p����
}
MANUAL_TITLE_PARAM;

//-------------------------------------
/// ���[�N
//=====================================
typedef struct _MANUAL_TITLE_WORK MANUAL_TITLE_WORK;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// ����������
extern  MANUAL_TITLE_WORK*  MANUAL_TITLE_Init(
    MANUAL_TITLE_PARAM*   param,
    MANUAL_COMMON_WORK*   cmn_wk
);
// �I������
extern  void  MANUAL_TITLE_Exit(
    MANUAL_TITLE_WORK*    work
);
// �又��(�I�������Ƃ�TRUE��Ԃ�)
extern  BOOL  MANUAL_TITLE_Main(
    MANUAL_TITLE_WORK*    work
);

