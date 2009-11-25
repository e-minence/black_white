//==============================================================================
/**
 * @file    monolith_tool.h
 * @brief   ���m���X��ʗp�c�[��
 * @author  matsuda
 * @date    2009.11.24(��)
 */
//==============================================================================
#pragma once


//==============================================================================
//  �萔��`
//==============================================================================
///�p�l���̃T�C�Y
typedef enum{
  PANEL_SIZE_LARGE,         ///<�傫���p�l��
  PANEL_SIZE_SMALL,         ///<�������p�l��
}PANEL_SIZE;

///�p�l�����WX
#define PANEL_POS_X     (128)
///�p�l��(��)��BMPFONT�`��\�T�C�Y(�L�����N�^�P��)
#define PANEL_LARGE_CHARSIZE_X  (22)
///�p�l��(��)��BMPFONT�`��\�T�C�Y(�L�����N�^�P��)
#define PANEL_SMALL_CHARSIZE_X  (18)
///�p�l����BMPFONT�`��\�T�C�Y(�L�����N�^�P��) : �召���p
#define PANEL_CHARSIZE_Y        (2)


//==============================================================================
//  �\���̒�`
//==============================================================================
///�p�l���A�N�^�[�\����
typedef struct{
  GFL_CLWK *cap;
  BMPOAM_ACT_PTR bmpoam;
  GFL_BMP_DATA *bmp;
  u8 trans_flag;
  u8 padding[3];
}PANEL_ACTOR;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void MonolithTool_Panel_Create(MONOLITH_SETUP *setup, PANEL_ACTOR *dest, COMMON_RESOURCE_INDEX res_index, PANEL_SIZE size, int y, u32 msg_id);
extern void MonolithTool_Panel_Delete(PANEL_ACTOR *panel);
extern BOOL MonolithTool_Panel_TransUpdate(MONOLITH_SETUP *setup, PANEL_ACTOR *panel);

