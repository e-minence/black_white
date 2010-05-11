//============================================================================
/**
 *  @file   manual_list.h
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *
 *  ���W���[�����FMANUAL_LIST
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
  MANUAL_LIST_RESULT_RETURN,  // �^�b�`�o�[�̃��^�[���A�C�R����I��ŏI��
  MANUAL_LIST_RESULT_ITEM,    // ���ڂ̂ǂꂩ��I��ŏI��
}
MANUAL_LIST_RESULT;

typedef enum
{
  MANUAL_LIST_ICON_NONE,    // �A�C�R���Ȃ�
  MANUAL_LIST_ICON_NEW,     // NEW�A�C�R������
}
MANUAL_LIST_ICON;

typedef enum
{
  MANUAL_LIST_TYPE_CATEGORY,
  MANUAL_LIST_TYPE_TITLE,
}
MANUAL_LIST_TYPE;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���ڂ̏��
//=====================================
typedef struct
{
  u16                    no;          // �J�e�S���̃��X�g�̂Ƃ���cate_no�A�^�C�g���̃��X�g�̂Ƃ���serial_no
  u16                    str_id;      // gmm���̕\�����镶�͂�ID
  MANUAL_LIST_ICON       icon;
}
MANUAL_LIST_ITEM;

//-------------------------------------
/// �p�����[�^
//=====================================
typedef struct
{
  // in
  MANUAL_LIST_TYPE   type;        // �^�C�v
  u16                num;         // �S���ڐ�
  MANUAL_LIST_ITEM*  item;        // �S���ڂ�item[num]
  // in,out
  u16                head_pos;    // �S���ڂ̒��ł̔ԍ�(��ʂ�1�ԏ�(UP_HALF�͏���)�ɂ��鍀�ڂ̔ԍ�)  // �X�N���[���o�[�̃J�[�\���̈ʒu�͂���Ō��܂�
  u16                cursor_pos;  // �S���ڂ̒��ł̔ԍ�
  // out 
  MANUAL_LIST_RESULT result;
}
MANUAL_LIST_PARAM;


//-------------------------------------
/// ���[�N
//=====================================
typedef struct _MANUAL_LIST_WORK MANUAL_LIST_WORK;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// ����������
extern  MANUAL_LIST_WORK*  MANUAL_LIST_Init(
    MANUAL_LIST_PARAM*   param,
    MANUAL_COMMON_WORK*  cmn_wk
);
// �I������
extern  void  MANUAL_LIST_Exit(
    MANUAL_LIST_WORK*    work
);
// �又��(�I�������Ƃ�TRUE��Ԃ�)
extern  BOOL  MANUAL_LIST_Main(
    MANUAL_LIST_WORK*    work
);

