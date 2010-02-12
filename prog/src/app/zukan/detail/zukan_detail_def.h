//============================================================================
/**
 *  @file   zukan_detail_def.h
 *  @brief  �}�ӏڍ׉�ʋ���
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *
 *  ���W���[�����FZKNDTL
 */
//============================================================================
#pragma once


// �C���N���[�h
#include <gflib.h>

#include "zknd_tbar.h"


//=============================================================================
/**
 *  �萔��`
 */
//=============================================================================
// ���C��BG�t���[��
#define ZKNDTL_BG_FRAME_M_TOUCHBAR              (GFL_BG_FRAME1_M)
// ���C��BG�t���[���̃v���C�I���e�B
#define ZKNDTL_BG_FRAME_PRI_M_TOUCHBAR          (0)

// ���C��BG�p���b�g
// �{��
enum
{
  ZKNDTL_BG_PAL_NUM_M_TOUCHBAR     =  ZKND_TBAR_BG_PLT_NUM,  // 1�{
};
// �ʒu
enum
{
  ZKNDTL_BG_PAL_POS_M_TOUCHBAR     =  13,
};

// ���C��OBJ�p���b�g
// �{��
enum
{
  ZKNDTL_OBJ_PAL_NUM_M_TOUCHBAR    =  ZKND_TBAR_OBJ_PLT_NUM + 3,  // 3�{+3�{
};
// �ʒu
enum
{
  ZKNDTL_OBJ_PAL_POS_M_TOUCHBAR    =   8,
};


// �T�uBG�t���[��
#define ZKNDTL_BG_FRAME_S_HEADBAR               (GFL_BG_FRAME1_S)
// �T�uBG�t���[���̃v���C�I���e�B
#define ZKNDTL_BG_FRAME_PRI_S_HEADBAR           (0)

// �T�uBG�p���b�g
// �{��
enum
{
  ZKNDTL_BG_PAL_NUM_S_HEADBAR     =  1,
};
// �ʒu
enum
{
  ZKNDTL_BG_PAL_POS_S_HEADBAR     =  13,
};

// �T�uOBJ�p���b�g
// �{��
enum
{
  ZKNDTL_OBJ_PAL_NUM_S_HEADBAR    =  0,
};
// �ʒu
enum
{
  ZKNDTL_OBJ_PAL_POS_S_HEADBAR    =  0,
};


// �Z���̓ǂ݂킯�Ɏg���}�b�s���O���[�h
#define ZKNDTL_OBJ_MAPPING_M                (APP_COMMON_MAPPING_128K)
#define ZKNDTL_OBJ_MAPPING_S                (APP_COMMON_MAPPING_128K)


// �R�}���h
typedef enum
{
  ZKNDTL_CMD_NONE,
  ZKNDTL_CMD_CLOSE,
  ZKNDTL_CMD_RETURN,
  ZKNDTL_CMD_CUR_D,
  ZKNDTL_CMD_CUR_U,
  ZKNDTL_CMD_CHECK,
  ZKNDTL_CMD_INFO,
  ZKNDTL_CMD_MAP,
  ZKNDTL_CMD_VOICE,
  ZKNDTL_CMD_FORM,
  ZKNDTL_CMD_MAP_RETURN,
  ZKNDTL_CMD_FORM_RETURN,
  ZKNDTL_CMD_FORM_CUR_R,
  ZKNDTL_CMD_FORM_CUR_L,
  ZKNDTL_CMD_FORM_CUR_D,
  ZKNDTL_CMD_FORM_CUR_U,
  ZKNDTL_CMD_FORM_EXCHANGE,
}
ZKNDTL_COMMAND;

