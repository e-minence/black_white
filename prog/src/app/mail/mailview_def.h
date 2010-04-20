//=================================================================
/**
  @file   mailview_def.h
  @brier  ���[���`�� ���e������`
  @author Akito Mori
  @date   10.02.10
*/
//=================================================================
#ifndef __MAILVIEW_DEF_H__
#define __MAILVIEW_DEF_H__
//=======================================
///�A���t�@�l�ݒ�
//=======================================
#define ALPHA_SECOND  (28)
#define ALPHA_FIRST   (4)

//=======================================
///�p���b�g�A�j���֘A
//=========================================
#define PALANM_STARTPAL (33)

enum{
 MAILGRA_PAL,
 FONT_PAL,
 CFRAME_PAL,
 MENU_FONT_PAL,
 BMPL_MENU_WIN_PAL,
 TALK_FONT_PAL,
 BMPL_TALK_WIN_PAL,
 MAILVIEW_PALMAX,
};

///�ȈՕ��E�B���h�E01
#define MAIL_BMPCHAR_BASE (1023)
#define BMPL_MSG_PX   (  3 )
#define BMPL_MSG_SX   ( 26 )
#define BMPL_MSG_SY   (  4 )
#define BMPL_MSG_SIZ  (BMPL_MSG_SX*BMPL_MSG_SY)
#define BMPL_MSG_PAL  (FONT_PAL)
#define BMPL_MSG_FRM  (GFL_BG_FRAME1_M)

#define BMPL_M01_PY   (  3 )
#define BMPL_M01_CGX  (MAIL_BMPCHAR_BASE-BMPL_MSG_SIZ)
#define BMPL_M02_PY   (BMPL_M01_PY+BMPL_MSG_SY+1)
#define BMPL_M02_CGX  (BMPL_M01_CGX-BMPL_MSG_SIZ)
#define BMPL_M03_PY   (BMPL_M02_PY+BMPL_MSG_SY+1)
#define BMPL_M03_CGX  (BMPL_M02_CGX-BMPL_MSG_SIZ)

// �ꌾ�ȈՉ�b���[�h
#define BMPL_PMS_PX   ( 10 )
#define BMPL_PMS_PY   ( 19 )
#define BMPL_PMS_SX   (  9 )
#define BMPL_PMS_SY   (  2 )

// ���[���쐬�Җ�
#define BMPL_NAME_PX  (21)
#define BMPL_NAME_PY  (19)
#define BMPL_NAME_SX  (8)
#define BMPL_NAME_SY  (2)


//�����Ă��E��߂�E�B���h�E
#define BMPL_YN_PX  (21)
#define BMPL_YN01_PY  (18)
#define BMPL_YN02_PY  (21)
#define BMPL_YN_SX  (8)
#define BMPL_YN_SY  (2)
#define BMPL_YN_PAL (FONT_PAL)
#define BMPL_YN_FRM (GFL_BG_FRAME1_M)
#define BMPL_YN01_CGX (BMPL_M03_CGX-BMPL_YN_SX*BMPL_YN_SY)
#define BMPL_YN02_CGX (BMPL_YN01_CGX-BMPL_YN_SX*BMPL_YN_SY)
#define BPML_YN_W     ( APP_TASKMENU_PLATE_WIDTH_YN_WIN+2 )

//�t�H���g���E�B���h�E�t���[��
#define BMPL_WIN_FRM    (GFL_BG_FRAME0_M)

#define BMPL_TALK_PX  (2)
#define BMPL_TALK_PY  (1)
#define BMPL_TALK_SX  (28)
#define BMPL_TALK_SY  (4)
#define BMPL_TALK_CGX (BMPL_YN02_CGX-BMPL_TALK_SX*BMPL_TALK_SY)
#define BMPL_TALK_PAL (TALK_FONT_PAL)
#define BMPL_TALK_FRM (BMPL_WIN_FRM)

//YN�m�F�E�B���h�E
#define BMPL_YESNO_PX (24)
#define BMPL_YESNO_PY (10)
#define BMPL_YESNO_SX (32)
#define BMPL_YESNO_SY (4)
#define BMPL_YESNO_PAL  (MENU_FONT_PAL)
#define BMPL_YESNO_FRM  (BMPL_WIN_FRM)
#define BMPL_YESNO_CGX  (BMPL_TALK_CGX-BMPL_YESNO_SX*BMPL_YESNO_SY)

#define BMPL_MENU_WIN_CGX (1)
#define BMPL_TALK_WIN_CGX (BMPL_MENU_WIN_CGX+MENU_WIN_CGX_SIZ+3)

//=====================================================
///�^�b�`�̈��`
//=====================================================
#define TPMSG_DPX   (24)
#define TPMSG_DSX   (26*8)
#define TPMSG_DSY   (32)
#define TPMSG_OFSY  (TPMSG_DSY+8)
#define TPMSG_DPY01 (24)
#define TPMSG_DPY02 (TPMSG_DPY01+TPMSG_OFSY)
#define TPMSG_DPY03 (TPMSG_DPY02+TPMSG_OFSY)
#define TPMSG_PMS_X  ( BMPL_PMS_PX*8 )
#define TPMSG_PMS_Y  ( BMPL_PMS_PY*8 )
#define TPMSG_PMS_W  ( BMPL_PMS_SX*8 )
#define TPMSG_PMS_H  ( BMPL_PMS_SY*8 )


#define TPSW_DECIDE_X (BMPL_YN_PX*8)
#define TPSW_DECIDE_Y (BMPL_YN01_PY*8)
#define TPSW_DECIDE_W (BPML_YN_W*8)
#define TPSW_DECIDE_H (24)
#define TPSW_CANCEL_X (BMPL_YN_PX*8)
#define TPSW_CANCEL_Y (BMPL_YN02_PY*8) 
#define TPSW_CANCEL_W (BPML_YN_W*8)
#define TPSW_CANCEL_H (24)

//=====================================================
///�|�P�����A�C�R���֘A
//=====================================================
#define MAILVIEW_ICON_PRI (2) ///<���[��View�|�P�����A�C�R���v���C�I���e�B
#define ICONVIEW_WIDTH  (40)  ///<�A�C�R������ׂ�Ԋu
#define ICONVIEW_PX   (16*8)  ///<�A�C�R���\���̉E�[���W
#define ICONVIEW_PY   (160) ///<�A�C�R���\����Y���W

#endif // __MAILVIEW_DEF_H__