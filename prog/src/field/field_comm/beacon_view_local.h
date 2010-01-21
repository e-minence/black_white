/**
 *  @file   beacon_view_local.h
 *  @brief  ����Ⴂ�ʐM��ʃ��[�J���w�b�_
 *  @author Miyuki Iwasawa
 *  @date   10.01.19
 */

#pragma once

/////////////////////////////////////
//���e����
#define HEAPID_BVIEW_TMP      GFL_HEAP_LOWID( HEAPID_FIELDMAP )
#define HEAPID_BEACON_VIEW    (HEAPID_FIELDMAP)

///�t�H���g�p���b�g�W�J�ʒu
#define FONT_PAL    (15)
#define ACT_PAL_FONT  (3)
#define ACT_PAL_UNION (9)

///�\�����郍�O����
#define VIEW_LOG_MAX    (4)

#define BS_LOG_MAX  (30)  //���O�Ǘ���
#define PANEL_MAX   (5)   //�����`�悳���p�l����
#define PANEL_LINE_MAX    (4)   //��ʓ��ɕ`�悳���p�l����


///�p�l��������o�b�t�@��
#define BUFLEN_PANEL_MSG  (10+EOM_SIZE)

//OBJ���\�[�X�Q�ƃR�[�h
enum{
 OBJ_RES_PLTT,
 OBJ_RES_CGR,
 OBJ_RES_CELLANIM,
 OBJ_RES_MAX,
};

#define UNION_CHAR_MAX      (16)  ///<���j�I���L�����N�^�[max
#define BEACON_VIEW_OBJ_MAX (5*8) ///<��ʓ��ɕ\������OBJ�̓o�^max��

///OBJ BG�v���C�I���e�B
#define OBJ_BG_PRI (3)

///OBJ�\�t�g�E�F�A�v���C�I���e�B
enum{
 OBJ_SPRI_MSG = 0,
 OBJ_SPRI_ICON = OBJ_SPRI_MSG + PANEL_MAX,
 OBJ_SPRI_UNION = OBJ_SPRI_ICON + PANEL_MAX,
 OBJ_SPRI_PANEL = OBJ_SPRI_UNION + PANEL_MAX,
};

///�m�[�}��OBJ �A�j��ID
enum{
 ACTANM_PANEL,
};

#define ACT_PANEL_OX  (13)
#define ACT_PANEL_OY  (5*8)
#define ACT_UNION_OX  (4*8)
#define ACT_UNION_OY  (3*8)
#define ACT_ICON_OX   (ACT_UNION_OX+28)
#define ACT_ICON_OY   (ACT_UNION_OY)
#define ACT_MSG_OX    (9*8)
#define ACT_MSG_OY    (2*8)

///�A�C�R���p�^�[����
enum{
  ICON_HELLO,
  ICON_BTL_START,
  ICON_BTL_WIN,
  ICON_POKE_GET,
  ICON_LV_UP,
  ICON_SHINKA,
  ICON_GPOWER,
  ICON_ITEM_GET,
  ICON_SPATIAL,
  ICON_UNION,
  ICON_CM,
  ICON_THANKS,
  ICON_MAX,
};

#define BMP_PANEL_OAM_SX  (15)
#define BMP_PANEL_OAM_SY  (2)

//==============================================================================
//  �\���̒�`
//==============================================================================
//�I�u�W�F���\�[�X�Ǘ�
typedef struct _OBJ_RES{
  u32   num;
  u32*  tbl;
}OBJ_RES;

typedef struct _OBJ_RES_SRC{
  u16 num;
  u16 srcID;
}OBJ_RES_SRC;

//�I�u�W�F���\�[�X�Ǘ��\����
typedef struct _OBJ_RES_TBL{
  OBJ_RES res[OBJ_RES_MAX];
}OBJ_RES_TBL;

//�L�����N�^�[���\�[�X
typedef struct _RES2D_CHAR{
  NNSG2dCharacterData* p_char;
  void* buf;
}RES2D_CHAR;

//�p���b�g���\�[�X
typedef struct _RES2D_PLTT{
  NNSG2dPaletteData* p_pltt;
  u16* dat;
  void* buf;
}RES2D_PLTT;

// OAM�t�H���g���[�N
typedef struct {
	BMPOAM_ACT_PTR oam;
	GFL_BMP_DATA * bmp;
}FONT_OAM;

//�p�l���Ǘ��\����
typedef struct _PANEL_WORK{
  u8  id;
  u8  data_idx;
  s16 px;
  s16 py;
  GFL_CLWK* cPanel;
  GFL_CLWK* cUnion;
  GFL_CLWK* cIcon;
  FONT_OAM  msgOam;
	STRBUF* str;

  struct _PANEL_WORK* next;
  struct _PANEL_WORK* prev;
}PANEL_WORK;

///����Ⴂ�ʐM��ԎQ�Ɖ�ʊǗ����[�N
typedef struct _BEACON_VIEW{
  GAMESYS_WORK *gsys;
  FIELD_SUBSCREEN_WORK *subscreen;

  HEAPID      heapID;
  HEAPID      tmpHeapID;
  ARCHANDLE*  arc_handle;

  GFL_FONT *fontHandle;
  PRINT_QUE *printQue;
  WORDSET *wordset;
  GFL_MSGDATA *mm_status;
  STRBUF *strbuf_temp;
  STRBUF *strbuf_expand;

  BMPOAM_SYS_PTR bmpOam;
  GFL_CLUNIT* cellUnit;
  OBJ_RES_TBL objResNormal;
  OBJ_RES_TBL objResUnion;
  OBJ_RES_TBL objResIcon;

  GFL_BMPWIN *win[VIEW_LOG_MAX];
  PRINT_UTIL print_util[VIEW_LOG_MAX];
 
  PANEL_WORK  panel[PANEL_MAX];
  u32 log_count;


  //���\�[�X
  RES2D_CHAR  resCharIcon[ICON_MAX];
  RES2D_CHAR  resCharUnion[UNION_CHAR_MAX];
  RES2D_PLTT  resPlttUnion;
}BEACON_VIEW;


