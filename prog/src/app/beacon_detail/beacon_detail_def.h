/*
 *  @file beacon_detail_def.h
 *  @brief  ����Ⴂ�r�[�R���ڍ׉�ʃ��e������`
 *  @author Miyuki Iwasawa
 *  @date   10.02.02
 */

#pragma once

#define  BEACON_DETAIL_HEAP_SIZE  (0x30000)  ///< �q�[�v�T�C�Y

enum{
 SEQ_INI_DRAW,
 SEQ_FADEIN,
 SEQ_MAIN,
 SEQ_EFF_WAIT,
 SEQ_POPUP,
 SEQ_FADEOUT,
 SEQ_EXIT,
};

//-------------------------------------
///	�t���[��
//=====================================
enum
{	
	BG_FRAME_BAR_M	= GFL_BG_FRAME0_M,
	BG_FRAME_POPUP_M	= GFL_BG_FRAME1_M,
	BG_FRAME_MAP01_M	= GFL_BG_FRAME2_M,
	BG_FRAME_MAP02_M	= GFL_BG_FRAME3_M,
	BG_FRAME_WIN01_S	= GFL_BG_FRAME0_S,
	BG_FRAME_WIN02_S	= GFL_BG_FRAME1_S,
	BG_FRAME_BASE_S	= GFL_BG_FRAME3_S,
};
//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	//���C��BG
	PLTID_BG_BACK_M				= 0,
	PLTID_BG_POKE_M				= 1,
	PLTID_BG_POPUP_M		  = 11, //�|�b�v�A�b�v�E�B���h�E�p���b�g
	PLTID_BG_TOUCHBAR_M		= 13, //�^�b�`�o�[2�{��L
	PLTID_BG_LOCALIZE_M		= 15, //���[�J���C�Y�p

	//�T�uBG
	PLTID_BG_WIN01_S			=	0,
	PLTID_BG_WIN02_S			=	1,
  PLTID_BG_PMS_S        = 10, //�ȈՉ�b�f�R���p5�{��L
  PLTID_BG_FONT_S       = 14, //�t�H���g
	PLTID_BG_LOCALIZE_S		= 15, //���[�J���C�Y�p

	//���C��OBJ
	PLTID_OBJ_NORMAL_M	= 0, //�ʏ�
  PLTID_OBJ_UNION_M = 10, //Union�I�u�W�F�p
	PLTID_OBJ_TOUCHBAR_M	= 11, // 3�{�g�p
	PLTID_OBJ_LOCALIZE_M	= 14,		//���[�J���C�Y�p
  PLTID_OBJ_WMI_M = 15,	  //�ʐM�A�C�R��

  //�T�uOBJ
  PLTID_OBJ_NORMAL_S = 0, //
  PLTID_OBJ_TRAINER_S = 8, // 2�{�g�p
  PLTID_OBJ_PMS_DRAW = 10, // 5�{�g�p
	PLTID_OBJ_LOCALIZE_S	= 15,		//���[�J���C�Y�p
};

#define BUFLEN_TMP_MSG  (18*3*2+EOM_SIZE)

#define	FCOL_FNTOAM   ( PRINTSYS_MACRO_LSB(1,2,0) )	 ///<OAM�t�H���g����
#define FCOL_FNTOAM_W ( PRINTSYS_MACRO_LSB(15,3,0))  ///<Oam������
#define FCOL_WHITE_N  ( PRINTSYS_MACRO_LSB(15,2,0) ) ///<BG������
#define FCOL_POPUP_BASE (15)
#define FCOL_POPUP_MAIN (7)
#define FCOL_POPUP_SDW  (8)
#define FCOL_POPUP      ( PRINTSYS_MACRO_LSB(FCOL_POPUP_MAIN,FCOL_POPUP_SDW,FCOL_POPUP_BASE))  //BG�|�b�v�A�b�v

#define FCOL_WIN_BASE1  (3)
#define FCOL_WIN_BASE2  (4)
#define FCOL_WIN_MAIN   (15)
#define FCOL_WIN_SDW    (14)

#define FCOL_WIN01 ( PRINTSYS_MACRO_LSB(FCOL_WIN_MAIN,FCOL_WIN_SDW,FCOL_WIN_BASE1))  //�r�[�R���E�B���h�E1
#define FCOL_WIN02 ( PRINTSYS_MACRO_LSB(FCOL_WIN_MAIN,FCOL_WIN_SDW,FCOL_WIN_BASE2))  //�r�[�R���E�B���h�E2

//BG�A���t�@�ݒ�
#define ALPHA_1ST_M (GX_BLEND_PLANEMASK_BG1)
#define ALPHA_2ND_M (GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_OBJ)
#define ALPHA_1ST_S (GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_OBJ)
#define ALPHA_2ND_S (GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_OBJ)

#define ALPHA_EV2_M  (5)
#define ALPHA_EV1_M  (16-ALPHA_EV2_M)
#define ALPHA_EV2_S  (3)
#define ALPHA_EV1_S  (16-ALPHA_EV2_S)

//�r�[�R���E�B���h�E�̐�
#define BEACON_WIN_MAX    (2)

#define UNION_CHAR_MAX  (16)

///WordSet
#define WORDSET_BDETAIL_BUFLEN  (WORDSET_COUNTRY_BUFLEN)

///////////////////////////////////////////////////
//SE�֘A

#define BDETAIL_SE_DECIDE (SEQ_SE_DECIDE1)
#define BDETAIL_SE_POPUP  (SEQ_SE_DECIDE2)

///////////////////////////////////////////////////
//BMP�֘A

/////////////////////////////////
//�r�[�R���E�B���h�E
#define BEACON_PROF_MAX   (4)
#define BEACON_HOME_MAX   (2)
#define BEACON_RECORD_MAX (2)

#define BMP_BEACON_FRM  (BG_FRAME_WIN01_S)

#define BMP_PROF_PX (2)
#define BMP_PROF_PY (1)
#define BMP_PROF_SX (19)
#define BMP_PROF_SY (2)
#define BMP_PROF_PAL ( PLTID_BG_WIN01_S )

#define BMP_HOME01_PX (2)
#define BMP_HOME01_PY (9)
#define BMP_HOME01_SX (19)
#define BMP_HOME01_SY (2)
#define BMP_HOME01_PAL ( PLTID_BG_WIN01_S )

#define BMP_HOME02_PX (BMP_HOME01_PX+1)
#define BMP_HOME02_PY (BMP_HOME01_PY+BMP_HOME01_SY)
#define BMP_HOME02_SX (28)
#define BMP_HOME02_SY (4)
#define BMP_HOME02_PAL ( PLTID_BG_WIN01_S )

#define BMP_PMS_PX (2)
#define BMP_PMS_PY (15)
#define BMP_PMS_SX (28)
#define BMP_PMS_SY (4)
#define BMP_PMS_PAL ( PLTID_BG_WIN01_S )

#define BMP_RECORD_PX (4)
#define BMP_RECORD_PY (19)
#define BMP_RECORD_SX (27)
#define BMP_RECORD_SY (4)
#define BMP_RECORD_PAL ( PLTID_BG_WIN01_S )

//�|�b�v�A�b�v�E�B���h�E
#define BMP_POPUP_PX (2)
#define BMP_POPUP_PY (22)
#define BMP_POPUP_SX  (28)
#define BMP_POPUP_SY  (6)
#define BMP_POPUP_FRM (BG_FRAME_POPUP_M)
#define BMP_POPUP_PAL (PLTID_BG_POPUP_M)

///////////////////////////////////////////
//�E�B���h�E���C�A�E�g

#define BMP_PROF01_DAT_PX  (6)
#define BMP_PROF01_DAT_SX  (BMP_PROF_SX-BMP_PROF01_DAT_PX)
#define BMP_PROF01_DAT_SY  (BMP_PROF_SY)

#define BMP_PROF02_DAT_PX  (9)
#define BMP_PROF02_DAT_SX  (BMP_PROF_SX-BMP_PROF02_DAT_PX)
#define BMP_PROF02_DAT_SY  (BMP_PROF_SY)

#define BMP_RECORD_DAT_PX  (18)
#define BMP_RECORD_DAT_SX  (BMP_RECORD_SX-BMP_RECORD_DAT_PX)
#define BMP_RECORD_DAT_SY  (BMP_RECORD_SY)

//////////////////////////////////////////////
//�A�N�^�[
enum{
 ACT_ICON_TR,
 ACT_ICON_EV,
 ACT_MAX
};

enum{
 ACT_SF_MAIN,
 ACT_SF_SUB,
 ACT_SF_MAX,
};

enum{
 ACTANM_ICON_TR,
 ACTANM_ICON_POKE,
 ACTANM_RANK01,
 ACTANM_RANK02,
 ACTANM_RANK03,
 ACTANM_RANK04,
 ACTANM_RANK05,
};

#define ACT_TRAINER_PX  (27*8+4)
#define ACT_TRAINER_PY  (5*8+4)
#define ACT_RANK_PX     (25*8)
#define ACT_RANK_PY     (10*8)
#define ACT_TRAINER_BGPRI (0)

#define ACT_ICON_BGPRI  (2)
#define ACT_ICON_SX   (24)
#define ACT_ICON_SY   (32)
#define ACT_ICON_SXH  (ACT_ICON_SX/2)
#define ACT_ICON_SYH  (ACT_ICON_SY/2)

//////////////////////////////////////////
//�G�t�F�N�g�֘A��`�l
enum{
 SCROLL_UP,
 SCROLL_DOWN,
};

///�|�b�v�A�b�v�X�N���[��
#define POPUP_HEIGHT  (8*8)
#define POPUP_DIFF    (8)
#define POPUP_COUNT   (POPUP_HEIGHT/POPUP_DIFF)
#define POPUP_WAIT    (30*5)


///�y�[�W�X�N���[��
#define PAGE_SCROLL_H   (192)
#define PAGE_SCROLL_PY  (-PAGE_SCROLL_H)
#define PAGE_SCROLL_DY  (8)
#define PAGE_SCROLL_TIME  (PAGE_SCROLL_H/PAGE_SCROLL_DY)
enum{
 SCROLL_POS_UP,
 SCROLL_POS_DEF,
 SCROLL_POS_DOWN,
};
