//============================================================================
/**
 *  @file   manual_explain.h
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *
 *  ���W���[�����FMANUAL_EXPLAIN
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
#define MANUAL_EXPLAIN_PAGE_MAX  (9)         // �ő�9�y�[�W

typedef enum
{
  MANUAL_EXPLAIN_RESULT_RETURN,
}
MANUAL_EXPLAIN_RESULT;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// 1�y�[�W�̏��
//=====================================
typedef struct
{
  u16         image;       // �摜�t�@�C���Ȃ��̂Ƃ�MANUAL_EXPLAIN_NO_IMAGE
  u16         str_id;      // gmm���̕\�����镶�͂�ID
}
MANUAL_EXPLAIN_PAGE;

//-------------------------------------
/// �p�����[�^
//=====================================
typedef struct
{
  // in
  u8                     page_num;                       // �y�[�W��  // page_num<=MANUAL_EXPLAIN_PAGE_MAX
  u16                    title_str_id;                   // gmm���̕\�����镶�͂�ID  // �^�C�g��
  MANUAL_EXPLAIN_PAGE    page[MANUAL_EXPLAIN_PAGE_MAX];  // �g�p����͓̂Y����0<= <page_num
  // out 
  MANUAL_EXPLAIN_RESULT  result;
}
MANUAL_EXPLAIN_PARAM;

//-------------------------------------
/// ���[�N
//=====================================
typedef struct _MANUAL_EXPLAIN_WORK MANUAL_EXPLAIN_WORK;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// ����������
extern  MANUAL_EXPLAIN_WORK*  MANUAL_EXPLAIN_Init(
    MANUAL_EXPLAIN_PARAM*    param,
    MANUAL_COMMON_WORK*      cmn_wk
);
// �I������
extern  void  MANUAL_EXPLAIN_Exit(
    MANUAL_EXPLAIN_WORK*     work
);
// �又��(�I�������Ƃ�TRUE��Ԃ�)
extern  BOOL  MANUAL_EXPLAIN_Main(
    MANUAL_EXPLAIN_WORK*     work
);

