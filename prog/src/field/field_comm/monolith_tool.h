//==============================================================================
/**
 * @file    monolith_tool.h
 * @brief   ���m���X��ʗp�c�[��
 * @author  matsuda
 * @date    2009.11.24(��)
 */
//==============================================================================
#pragma once

#include "savedata/intrude_save.h"  //OCCUPY_INFO


//==============================================================================
//  �萔��`
//==============================================================================
//--------------------------------------------------------------
//  �p�l��
//--------------------------------------------------------------
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

///�p�l���̉����t�H�[�J�X���Ȃ��ꍇ�̎w��
#define PANEL_NO_FOCUS        (0xff)

//--------------------------------------------------------------
//  �L�����Z���A�C�R��
//--------------------------------------------------------------
///�L�����Z���A�C�R���̍��W
enum{
  CANCEL_POS_X = 256-16,
  CANCEL_POS_Y = 192-16,
  
  CANCEL_TOUCH_RANGE_HALF = 8,  ///<�L�����Z���A�C�R���̃q�b�g�͈͔��a
};

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

///���m���XBMPOAM�\����
typedef struct{
  GFL_BMP_DATA *bmp;
  BMPOAM_ACT_PTR bmpoam;
  u8 trans_flag;
  u8 padding[3];
}MONOLITH_BMPSTR;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern void MonolithTool_Panel_Create(MONOLITH_SETUP *setup, PANEL_ACTOR *dest, COMMON_RESOURCE_INDEX res_index, PANEL_SIZE size, int y, u32 msg_id);
extern void MonolithTool_Panel_Delete(PANEL_ACTOR *panel);
extern BOOL MonolithTool_Panel_TransUpdate(MONOLITH_SETUP *setup, PANEL_ACTOR *panel);

extern PANEL_COLORMODE MonolithTool_PanelColor_GetMode(MONOLITH_APP_PARENT *appwk);
extern void MonolithTool_Panel_ColorUpdate(MONOLITH_APP_PARENT *appwk, FADEREQ req);
extern void MonolithTool_Panel_Focus(MONOLITH_APP_PARENT *appwk, PANEL_ACTOR panel[], 
  int panel_max, int focus_no, FADEREQ req);
extern void MonolithTool_Panel_Flash(MONOLITH_APP_PARENT *appwk, PANEL_ACTOR panel[], 
  int panel_max, int focus_no, FADEREQ req);

extern void MonolithTool_Bmpoam_Create(MONOLITH_SETUP *setup, MONOLITH_BMPSTR *dest, COMMON_RESOURCE_INDEX res_index, int act_x, int act_y, int bmp_size_x, int bmp_size_y, u32 msg_id);
extern void MonolithTool_Bmpoam_Delete(MONOLITH_BMPSTR *bmpstr);
extern BOOL MonolithTool_Bmpoam_TransUpdate(MONOLITH_SETUP *setup, MONOLITH_BMPSTR *bmpstr);

extern OCCUPY_INFO * MonolithTool_GetOccupyInfo(MONOLITH_APP_PARENT *appwk);

extern GFL_CLWK * MonolithTool_TownIcon_Create(MONOLITH_SETUP *setup, 
  const OCCUPY_INFO *occupy, int town_no, const GFL_POINT *pos, COMMON_RESOURCE_INDEX res_index);
extern void MonolithTool_TownIcon_Delete(GFL_CLWK *cap);
extern void MonolithTool_TownIcon_Update(GFL_CLWK *cap, const OCCUPY_INFO *occupy, int town_no);

extern GFL_CLWK * MonolithTool_CancelIcon_Create(MONOLITH_SETUP *setup);
extern void MonolithTool_CancelIcon_Delete(GFL_CLWK *cap);
extern void MonolithTool_CancelIcon_Update(GFL_CLWK *cap);

