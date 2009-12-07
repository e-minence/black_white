/**
 *  @file   field_bg_def.h
 *  @brief  �t�B�[���hBG�ݒ�@�R����
 */

#pragma once

///���C���t���[������`
typedef enum{
  FLDBG_MFRM_3D = GFL_BG_FRAME0_M,      //�}�b�v(�t�B�[���h�풓)
  FLDBG_MFRM_MSG = GFL_BG_FRAME1_M,     //���b�Z�[�W(�t�B�[���h�풓)
  FLDBG_MFRM_EFF1 = GFL_BG_FRAME2_M,    //�e�탁�j���[(�t�B�[���h�풓)
  FLDBG_MFRM_EFF2 = GFL_BG_FRAME3_M,    //�e��G�t�F�N�g�A����(��풓)
}FLDBG_MAIN_FRAME;

///���C���t���[���f�t�H���g�v���C�I���e�B
typedef enum{
  FLDBG_MFRM_3D_PRI = 3,
  FLDBG_MFRM_MSG_PRI = 0,
  FLDBG_MFRM_EFF1_PRI = 2,
  FLDBG_MFRM_EFF2_PRI = 1,
}FLDBG_MAIN_PRIORITY;

/////////////////////////////////////////////////////////////
///�t�B�[���h�풓�t���[���@�ݒ�
/////////////////////////////////////////////////////////////

///���b�Z�[�W�t���[��
#define FLDBG_MFRM_MSG_COLORMODE  (GX_BG_COLORMODE_16)
#define FLDBG_MFRM_MSG_SCRBASE    (GX_BG_SCRBASE_0x0000)
#define FLDBG_MFRM_MSG_SCRSIZE    (0x800) //�ő�0x1000(�V���[�g�J�b�g���j���[�\���� <shortcut_menu.c)
#define FLDBG_MFRM_MSG_CHARBASE   (GX_BG_CHARBASE_0x10000)
#define FLDBG_MFRM_MSG_CHARSIZE   (0x8000)

//�e�탁�j���[�t���[��
#define FLDBG_MFRM_EFF1_COLORMODE  (GX_BG_COLORMODE_16)
#define FLDBG_MFRM_EFF1_SCRBASE    (GX_BG_SCRBASE_0xf800)
#define FLDBG_MFRM_EFF1_SCRSIZE    (0x800)
#define FLDBG_MFRM_EFF1_CHARBASE   (GX_BG_CHARBASE_0x18000)
#define FLDBG_MFRM_EFF1_CHARSIZE   (0x8000)

