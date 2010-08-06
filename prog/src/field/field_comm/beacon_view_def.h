/*
 *  @file   beacon_view_def.h
 *  @brief  �r�[�R�����C����ʁ@���e������`
 *  @author Miyuki Iwasawa
 *  @date   10.03.04
 */

#pragma once

///���C���V�[�P���X
enum{
  SEQ_MAIN,
  SEQ_LOG_ENTRY,
  SEQ_VIEW_UPDATE,
  SEQ_LIST_SCROLL_REPEAT,
  SEQ_GPOWER_USE,
  SEQ_THANK_YOU,
  SEQ_EV_REQ_WAIT,
  SEQ_END,
};

///�T�u�V�[�P���X
enum{
 SSEQ_THANKS_ICON_ANM,
 SSEQ_THANKS_ICON_ANM_WAIT,
 SSEQ_THANKS_MAIN,
 SSEQ_THANKS_VIEW_UPDATE,
 SSEQ_THANKS_DECIDE,
 SSEQ_THANKS_DECIDE_WAIT,
 SSEQ_THANKS_END,
#ifdef BUGFIX_AF_BTS8022_20100806
 SSEQ_THANKS_END_WAIT,
#endif
};

///�C�x���g���N�G�X�gID
typedef enum{
 EV_NONE,
 EV_RETURN_CGEAR,
 EV_RETURN_CGEAR_INTRUDE,     //�n�C�����N�N���Ăяo��
 EV_RETURN_CGEAR_WIRELESS_TR, //���C�u�L���X�^�[�Ăяo��
 EV_CALL_DETAIL_VIEW,
 EV_GPOWER_USE,
 EV_GPOWER_CHECK_TMENU_YN,
 EV_GPOWER_CHECK_TMENU_CHK,
 EV_CALL_TALKMSG_INPUT,

#ifdef PM_DEBUG
 EV_DEBUG_MENU_CALL,
#endif  //PM_DEBUG
}BEACON_DETAIL_EVENT;

///�X�y�V����G�p���[���N�G�X�gID
typedef enum{
  SP_GPOWER_REQ_NULL,
  SP_GPOWER_REQ_HATCH_UP,
  SP_GPOWER_REQ_NATSUKI_UP,
  SP_GPOWER_REQ_SALE,
  SP_GPOWER_REQ_EXP_UP,
  SP_GPOWER_REQ_MONEY_UP,
  SP_GPOWER_REQ_CAPTURE_UP,
  SP_GPOWER_REQ_MAX = SP_GPOWER_REQ_CAPTURE_UP,
}SP_GPOWER_REQ;

/////////////////////////////////////
//���e����
#define FRM_MENUMSG ( GFL_BG_FRAME0_S )
#define FRM_MENU   ( GFL_BG_FRAME1_S )
#define FRM_POPUP  ( GFL_BG_FRAME2_S )
#define FRM_BACK   ( GFL_BG_FRAME3_S )

//BG�A���t�@�ݒ�
#define ALPHA_1ST_NORMAL  (GX_BLEND_PLANEMASK_BG2)
#define ALPHA_1ST_PASSIVE (GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG2)
#define ALPHA_2ND         (GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_OBJ)
#define ALPHA_EV2_NORMAL  (3)
#define ALPHA_EV1_NORMAL  (16-ALPHA_EV2_NORMAL)
#define ALPHA_EV2_PASSIVE  (6)
#define ALPHA_EV1_PASSIVE  (16-ALPHA_EV2_PASSIVE)

///�p���b�g�W�J�ʒu
#define FONT_PAL_POPUP    (1)
#define FONT_PAL_LOGNUM   (4)   //���O�l���\��bmp �p���b�g
#define BG_PAL_LOCALIZE   (12)  //���[�J���C�Y�p�󂫃p���b�g
#define FONT_PAL          (13)  //�t�H���g�p���b�g��L
#define BG_PAL_TASKMENU   (14)  //TaskMenu��2�{��L
#define BG_PAL_TASKMENU_2 (15)  //��

#define BG_PALANM_AREA    (FONT_PAL+1)  //�t�H���g�p���b�g�p�̈�܂Ńp���b�g�A�j���e�����ɒu��

///�A�N�^�[�p���b�g�W�J�ʒu
#define ACT_PAL_FONT      (3)
#define ACT_PAL_PANEL     (4)   //5�{��L
#define ACT_PAL_UNION     (9)   //5�{��L
#define ACT_PAL_WMI       (14)  //�ʐM�A�C�R����L�̈�
#define ACT_PAL_LOCALIZE  (15)  //���[�J���C�Y�p�󂫃p���b�g

///�A�N�^�[�p�l���F�����[�h�p���b�g�W�J�ʒu
#define ACT_PANEL_PAL_FRM_V_BLACK (0) //�t���[���F(�u���b�N�o�[�W����)
#define ACT_PANEL_PAL_FRM_V_WHITE (1) //�t���[���F(�z���C�g�o�[�W����)
#define ACT_PANEL_PAL_FRM_V_ELSE  (2) //�t���[���F(���̑��o�[�W����)
#define ACT_PANEL_PAL_RANK_MARK   (3) //�����N�}�[�NOn/Off�؂�ւ��p

///�p�l���A�N�^�[�@�����N�}�[�N�F�]���J���[�I�t�Z�b�g
#define ACT_PANEL_COL_OFS_RANK    (8)

///�t�H���g�J���[
#define	FCOL_FNTOAM   ( PRINTSYS_MACRO_LSB(1,2,0) )	 ///<OAM�t�H���g����
#define FCOL_FNTOAM_W ( PRINTSYS_MACRO_LSB(15,3,0))  ///<Oam������
#define FCOL_FNTOAM_G ( PRINTSYS_MACRO_LSB(3,5,0))  ///<Oam�D����
#define FCOL_WHITE_N  ( PRINTSYS_MACRO_LSB(15,2,0) ) ///<BG������
#define FCOL_POPUP_BASE (9)
#define FCOL_POPUP_MAIN (1)
#define FCOL_POPUP_SDW  (2)
#define FCOL_POPUP      ( PRINTSYS_MACRO_LSB(FCOL_POPUP_MAIN,FCOL_POPUP_SDW,FCOL_POPUP_BASE))  //BG�|�b�v�A�b�v
#define FCOL_LOGNUM_BASE  (1)
#define FCOL_LOGNUM     ( PRINTSYS_MACRO_LSB(15,14,FCOL_LOGNUM_BASE))  //���O�l���\��

///���O���D�F�\������܂Ōo�ߎ���
#define BEACON_TIMEOUT_FRAME  (3*60*60) //60sec*3

///�\�����郍�O����
#define VIEW_LOG_MAX    (4)

#define BS_LOG_MAX  (30)  //���O�Ǘ���
#define PANEL_MAX   (5)   //�����`�悳���p�l����
#define PANEL_VIEW_START  (1)
#define PANEL_VIEW_END    (4)
#define PANEL_VIEW_MAX    (4)   //��ʓ��ɕ`�悳���p�l����
#define PANEL_LINE_END    (5)

#define PANEL_DATA_BLANK (0xFF)

///�v�����g�L���[��1sync�̏������~�b�g�ݒ�l
#define PRINT_QUE_LIMMIT_DEFAULT (500)
#define PRINT_QUE_LIMMIT_UP      (2000)

///�|�b�v�A�b�v���b�Z�[�W�o�b�t�@��
#define BUFLEN_POPUP_MSG  (18*6*2+EOM_SIZE)
#define BUFLEN_TMP_MSG    (BUFLEN_POPUP_MSG)

///�p�l��������o�b�t�@��
#define BUFLEN_PANEL_MSG  (10+EOM_SIZE)
///�g���[�i�[���o�b�t�@��
#define BUFLEN_TR_NAME  (PERSON_NAME_SIZE+EOM_SIZE)

///���j���[
typedef enum{
 MENU_F_POWER = 1,
 MENU_F_HELLO = 2,
 MENU_F_THANKS = 4,
 MENU_F_RETURN = 8,
 MENU_F_ALL = 0xF,
}MENU_FLAG;

typedef enum{
 MENU_POWER,
 MENU_HELLO,
 MENU_THANKS,
 MENU_RETURN,
 MENU_ALL,
}MENU_ID;

typedef enum{
 MENU_ST_ON,
 MENU_ST_ANM,
 MENU_ST_OFF,
}MENU_STATE;

///////////////////////////////////////////////////
//�p�l���\���R�[�h
typedef enum{
  PANEL_DRAW_UPDATE,
  PANEL_DRAW_NEW,
  PANEL_DRAW_INI,
}PANEL_DRAW_TYPE;

///////////////////////////////////////////////////
//SE�֘A
#define BVIEW_SE_DECIDE (SEQ_SE_DECIDE1)
#define BVIEW_SE_MENU   (SEQ_SE_SYS_79)
#define BVIEW_SE_SELECT (SEQ_SE_SELECT1)
#define BVIEW_SE_CANCEL (SEQ_SE_CANCEL1)
#define BVIEW_SE_UPDOWN (SEQ_SE_SELECT4)
#define BVIEW_SE_NEW_PLAYER (SEQ_SE_SYS_11)
#define BVIEW_SE_ICON   (SEQ_SE_SELECT2)

///////////////////////////////////////////////////
//BMP�֘A

//�p�l�����b�Z�[�W�\��oam�E�B���h�E
#define BMP_PANEL_OAM_SX  (12)  //�p�l��OAM
#define BMP_PANEL_OAM_SY  (2)

//�|�b�v�A�b�v�E�B���h�E
#define BMP_POPUP_PX (2)
#define BMP_POPUP_PY (22)
#define BMP_POPUP_SX  (28)
#define BMP_POPUP_SY  (6)
#define BMP_POPUP_FRM (FRM_POPUP)
#define BMP_POPUP_PAL (FONT_PAL_POPUP)
//���j���[�o�[�E�B���h�E
#define BMP_MENU_PX  (1)
#define BMP_MENU_PY  (21)
#define BMP_MENU_SX  (15)
#define BMP_MENU_SY  (3)
#define BMP_MENU_FRM (FRM_MENUMSG)
#define BMP_MENU_PAL (FONT_PAL)
//���O���\���E�B���h�E
#define BMP_LOGNUM_PX (0)
#define BMP_LOGNUM_PY (19)
#define BMP_LOGNUM_SX (6)   //���O���\��OAM
#define BMP_LOGNUM_SY (2)
#define BMP_LOGNUM_FRM (FRM_BACK)
#define BMP_LOGNUM_PAL (FONT_PAL_LOGNUM)

enum{
 WIN_POPUP,
 WIN_MENU,
 WIN_LOGNUM,
 WIN_MAX,
};

//�X�N���[��������`
typedef enum{
  SCROLL_UP,
  SCROLL_DOWN,
  SCROLL_RIGHT,
}SCROLL_DIR;

///�p�l���X�N���[���t���[��
#define PANEL_SCROLL_FRAME (8)

///�A�C�R���|�b�v�^�C��
#define ICON_POP_TIME (30*3)

///�|�b�v�A�b�v�X�N���[��
#define POPUP_HEIGHT  (8*8)
#define POPUP_DIFF    (16)
#define POPUP_COUNT   (POPUP_HEIGHT/POPUP_DIFF)
#define POPUP_WAIT    (30*3)

//G�p���[�|�b�v�A�b�v�^�C�v
enum{
 GPOWER_USE_MINE, //����
 GPOWER_USE_BEACON, //���l
 GPOWER_USE_SPECIAL, //����f���p���[
 GPOWER_USE_MAX,
};

//�^�X�N���j���[��
enum{
 TMENU_YN_CHECK,
 TMENU_YN_YES,
 TMENU_YN_NO,
 TMENU_YN_MAX,
};
enum{
 TMENU_CHECK_CALL,
 TMENU_CHECK_CLOSE,
 TMENU_CHECK_MAX,
};
#define TMENU_PY (21)
#define TMENU_W  (8)
#define TMENU_H  (APP_TASKMENU_PLATE_HEIGHT)

#define TMENU_YN_PX (8)
#define TMENU_CHECK_PX (16)

enum{
 TMENU_ID_YESNO,
 TMENU_ID_CHECK,
};

////////////////////////////////////////////////////
//�A�N�^�[�֘A��`

#define ACT_RENDER_ID (0)

//OBJ���\�[�X�Q�ƃR�[�h
enum{
 OBJ_RES_PLTT,
 OBJ_RES_CGR,
 OBJ_RES_CELLANIM,
 OBJ_RES_MAX,
};

#define UNION_CHAR_MAX      (16)  ///<���j�I���L�����N�^�[max
#define BEACON_VIEW_OBJ_MAX (64) ///<��ʓ��ɕ\������OBJ�̓o�^max��

///OBJ BG�v���C�I���e�B
#define OBJ_BG_PRI (3)
#define OBJ_MENU_BG_PRI (1)

///OBJ�\�t�g�E�F�A�v���C�I���e�B
enum{
 OBJ_SPRI_MSG = 0,
 OBJ_SPRI_MENU = 1,
 OBJ_SPRI_ICON = OBJ_SPRI_MSG + PANEL_MAX,
 OBJ_SPRI_UNION = OBJ_SPRI_ICON + PANEL_MAX,
 OBJ_SPRI_RANK = OBJ_SPRI_UNION + PANEL_MAX,
 OBJ_SPRI_PANEL = OBJ_SPRI_RANK + PANEL_MAX,
};

///�A�N�^�[ID
enum{
 ACT_POWER,
 ACT_HELLO,
 ACT_THANKS,
 ACT_RETURN,
 ACT_UP,
 ACT_DOWN,
 ACT_MAX,
};

///�m�[�}��OBJ �A�j��ID
enum{
 ACTANM_PANEL,
 ACTANM_POWER_ON,
 ACTANM_POWER_ANM,
 ACTANM_POWER_OFF,
 ACTANM_HELLO_ON,
 ACTANM_HELLO_ANM,
 ACTANM_HELLO_OFF,
 ACTANM_THANKS_ON,
 ACTANM_THANKS_ANM,
 ACTANM_THANKS_OFF,
 ACTANM_RETURN_ON,
 ACTANM_RETURN_ANM,
 ACTANM_RETURN_OFF,
 ACTANM_UP_DEF,
 ACTANM_UP_ANM,
 ACTANM_UP_ON,
 ACTANM_UP_OFF,
 ACTANM_DOWN_DEF,
 ACTANM_DOWN_ANM,
 ACTANM_DOWN_ON,
 ACTANM_DOWN_OFF,
};

#define ACT_ANM_SET (3)

#define ACT_PANEL_OX  (13)
#define ACT_PANEL_OY  (5*8)
#define ACT_PANEL_PX  (-ACT_PANEL_OX)
#define ACT_PANEL_PY  (-ACT_PANEL_OY)
#define ACT_PANEL_SI_SX (-26*8)  //�X���C�h�C�����̃X�^�[�g�|�W�V����
#define ACT_PANEL_SI_SY (0)

#define ACT_UNION_OX  (4*8)
#define ACT_UNION_OY  (3*8)
#define ACT_ICON_OX   (ACT_UNION_OX+28)
#define ACT_ICON_OY   (ACT_UNION_OY)
#define ACT_MSG_OX    (9*8)
#define ACT_MSG_OY    (2*8)
#define ACT_RANK_OX   (19*8)
#define ACT_RANK_OY   (4*8)

#define ACT_MENU_PX (16*8+4)
#define ACT_MENU_PY (21*8+4)
#define ACT_MENU_OX (4*8)
#define ACT_MENU_OY (0)
#define ACT_MENU_NUM  (4)
#define ACT_MENU_SX (24)
#define ACT_MENU_SY (24)

#define ACT_UP_PX (26*8)
#define ACT_UP_PY (1*8)
#define ACT_DOWN_PX (27*8)
#define ACT_DOWN_PY (5*8)


/////////////////////////////////////////////
//�^�b�`�p�l��
#define IO_INTERVAL (30*5)

//�p�l���̋�` LT,LB,RB,RT
#define TP_PANEL_X1 (0)
#define TP_PANEL_Y1 (5)
#define TP_PANEL_X2 (TP_PANEL_X1+12)
#define TP_PANEL_Y2 (TP_PANEL_Y1+38)
#define TP_PANEL_X3 (TP_PANEL_X2+198)
#define TP_PANEL_Y3 (TP_PANEL_Y2)
#define TP_PANEL_X4 (TP_PANEL_X1+198)
#define TP_PANEL_Y4 (TP_PANEL_Y1)

/*
  0
1   4
 2 3
*/
#define TP_UP_X1  (6 + ACT_UP_PX )
#define TP_UP_Y1  (3 + ACT_UP_PY )
#define TP_UP_X2  (1 + ACT_UP_PX )
#define TP_UP_Y2  (12 + ACT_UP_PY )
#define TP_UP_X3  (9 + ACT_UP_PX )
#define TP_UP_Y3  (31 + ACT_UP_PY )
#define TP_UP_X4  (31 + ACT_UP_PX )
#define TP_UP_Y4  (TP_UP_Y3 + ACT_UP_PY )
#define TP_UP_X5  (24 + ACT_UP_PX )
#define TP_UP_Y5  (10 + ACT_UP_PY )

/*
 0 4
1   3 
  2
*/
#define TP_DOWN_X1  (1  + ACT_DOWN_PX )
#define TP_DOWN_Y1  (1  + ACT_DOWN_PY )
#define TP_DOWN_X2  (8  + ACT_DOWN_PX )
#define TP_DOWN_Y2  (22 + ACT_DOWN_PY )
#define TP_DOWN_X3  (23 + ACT_DOWN_PX )
#define TP_DOWN_Y3  (30 + ACT_DOWN_PY )
#define TP_DOWN_X4  (31 + ACT_DOWN_PX )
#define TP_DOWN_Y4  (22 + ACT_DOWN_PY )
#define TP_DOWN_X5  (24 + ACT_DOWN_PX )
#define TP_DOWN_Y5  (1  + ACT_DOWN_PY )


