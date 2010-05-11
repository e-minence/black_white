//============================================================================
/**
 *  @file   manual_top.h
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *
 *  ���W���[�����FMANUAL_TOP
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
  MANUAL_TOP_RESULT_CLOSE,
  MANUAL_TOP_RESULT_RETURN,
  MANUAL_TOP_RESULT_CATEGORY,
  MANUAL_TOP_RESULT_ALL,
}
MANUAL_TOP_RESULT;


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
  u8                 cursor_pos;
  // out 
  MANUAL_TOP_RESULT  result;
}
MANUAL_TOP_PARAM;

//-------------------------------------
/// ���[�N
//=====================================
typedef struct _MANUAL_TOP_WORK MANUAL_TOP_WORK;


//=============================================================================
/**
*  �֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// ����������
extern  MANUAL_TOP_WORK*  MANUAL_TOP_Init(
    MANUAL_TOP_PARAM*    param,
    MANUAL_COMMON_WORK*  cmn_wk
);
// �I������
extern  void  MANUAL_TOP_Exit(
    MANUAL_TOP_WORK*     work
);
// �又��(�I�������Ƃ�TRUE��Ԃ�)
extern  BOOL  MANUAL_TOP_Main(
    MANUAL_TOP_WORK*     work
);

