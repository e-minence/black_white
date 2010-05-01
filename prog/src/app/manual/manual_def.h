//============================================================================
/**
 *  @file   manual_def.h
 *  @brief  �Q�[�����}�j���A��
 *  @author Koji Kawada
 *  @data   2010.04.26
 *  @note   
 *
 *  ���W���[�����FMANUAL_DEF
 */
//============================================================================
#pragma once


// �C���N���[�h
#include <gflib.h>

#include "ui/touchbar.h"


// �I�[�o�[���C
FS_EXTERN_OVERLAY(manual);


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// ���C��BG�t���[��
#define BG_FRAME_M_PIC       (GFL_BG_FRAME0_M)
// ���C��BG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_M_PIC   (0)
// ���C��BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_PIC       = 16,
};
// �ʒu
enum
{
  BG_PAL_POS_M_PIC       = 0,
};
// ���C��OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_         = 0,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_         = 0,
};

// �T�uBG�t���[��
#define BG_FRAME_S_REAR       (GFL_BG_FRAME0_S)
#define BG_FRAME_S_MAIN       (GFL_BG_FRAME1_S)
#define BG_FRAME_S_TB_BAR     (GFL_BG_FRAME2_S)
#define BG_FRAME_S_TB_TEXT    (GFL_BG_FRAME3_S)
// �T�uBG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_S_REAR     (3)
#define BG_FRAME_PRI_S_MAIN     (2)
#define BG_FRAME_PRI_S_TB_BAR   (1)
#define BG_FRAME_PRI_S_TB_TEXT  (0)
// �T�uBG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_S_REAR        = 2,
  BG_PAL_NUM_S_TEXT        = 1,
  BG_PAL_NUM_S_TB_BAR      = 1,  // TOUCHBAR_BG_PLT_NUM
  BG_PAL_NUM_S_ATM         = 2,  // APP_TASKMENU
};
// �ʒu
enum
{
  BG_PAL_POS_S_REAR        = 0,
  BG_PAL_POS_S_TEXT        = 2,
  BG_PAL_POS_S_TB_BAR      = 3,
  BG_PAL_POS_S_ATM         = 4,  // APP_TASKMENU
};
// �T�uOBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_S_CELL      =  1,
  OBJ_PAL_NUM_S_TB_ICON   =  3,  // TOUCHBAR_OBJ_PLT_NUM
};
// �ʒu
enum
{
  OBJ_PAL_POS_S_CELL      = 0,
  OBJ_PAL_POS_S_TB_ICON   = 1,
};

