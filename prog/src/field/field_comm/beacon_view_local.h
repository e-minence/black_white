/**
 *  @file   beacon_view_local.h
 *  @brief  ����Ⴂ�ʐM��ʃ��[�J���w�b�_
 *  @author Miyuki Iwasawa
 *  @date   10.01.19
 */

#pragma once

#include "gamesystem/game_beacon.h"
#include "gamesystem/game_beacon_types.h"
#include "gamesystem/game_beacon_accessor.h"
#include "gamesystem/beacon_status.h"

enum{
  SEQ_MAIN,

};

/////////////////////////////////////
//���e����
#define HEAPID_BVIEW_TMP      GFL_HEAP_LOWID( HEAPID_FIELDMAP )
#define HEAPID_BEACON_VIEW    (HEAPID_FIELDMAP)

///�p���b�g�W�J�ʒu
#define FONT_PAL    (15)
#define ACT_PAL_FONT  (3)
#define ACT_PAL_PANEL (4)
#define ACT_PAL_UNION (9)

#define	FCOL_FNTOAM	( PRINTSYS_LSB_Make(1,2,0) )		// �t�H���g�J���[�FOAM�t�H���g����

///�\�����郍�O����
#define VIEW_LOG_MAX    (4)

#define BS_LOG_MAX  (30)  //���O�Ǘ���
#define PANEL_MAX   (5)   //�����`�悳���p�l����
#define PANEL_VIEW_MAX    (4)   //��ʓ��ɕ`�悳���p�l����
#define PANEL_LINE_END    (5)

#define PANEL_DATA_BLANK (0xFF)

///�p�l��������o�b�t�@��
#define BUFLEN_PANEL_MSG  (10+EOM_SIZE)
///�g���[�i�[���o�b�t�@��
#define BUFLEN_TR_NAME  (PERSON_NAME_SIZE+EOM_SIZE)

#define ACT_RENDER_ID (0)

//OBJ���\�[�X�Q�ƃR�[�h
enum{
 OBJ_RES_PLTT,
 OBJ_RES_CGR,
 OBJ_RES_CELLANIM,
 OBJ_RES_MAX,
};

#define UNION_CHAR_MAX      (16)  ///<���j�I���L�����N�^�[max
#define BEACON_VIEW_OBJ_MAX (5*8) ///<��ʓ��ɕ\������OBJ�̓o�^max��


#define FRM_POPUP  ( GFL_BG_FRAME1_S )
#define FRM_PANEL  ( GFL_BG_FRAME2_S )
#define FRM_BACK  ( GFL_BG_FRAME3_S )

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
#define ACT_PANEL_PX  (-ACT_PANEL_OX)
#define ACT_PANEL_PY  (-ACT_PANEL_OY)

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

//�X�N���[��������`
#define SCROLL_UP   (0)
#define SCROLL_DOWN (1)
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
  u8  id; //�p�l��ID
  u8  data_ofs; //�f�[�^�Q�ƃI�t�Z�b�g
  u8  data_idx; //�f�[�^�z��Q�Ǝ�index

  u8  n_line; //���݂̃��C��
  s16 px;
  s16 py;
  GFL_CLWK* cPanel;
  GFL_CLWK* cUnion;
  GFL_CLWK* cIcon;
  FONT_OAM  msgOam;
	STRBUF* str;
	STRBUF* name;

  struct _PANEL_WORK* next;
  struct _PANEL_WORK* prev;
}PANEL_WORK;

typedef struct _LOG_CTRL{
  u8  max;  //���O��
  u8  top;  //���`�悳��Ă���擪index
  u8  next_panel; //���Ƀf�[�^���������������ރp�l��index
  u8  view_top;
  u8  view_btm;
  u8  view_max;

  u8  panel_list[PANEL_VIEW_MAX];
}LOG_CTRL;

///����Ⴂ�ʐM��ԎQ�Ɖ�ʊǗ����[�N
typedef struct _BEACON_VIEW{
  GAMESYS_WORK *gsys;
  GAMEDATA* gdata;
  FIELD_SUBSCREEN_WORK *subscreen;

  ////////////////////////////////////////
  BEACON_STATUS* b_status;
  GAMEBEACON_INFO_TBL*  infoStack;  //�X�^�b�N���[�N
  GAMEBEACON_INFO_TBL*  infoLog;    //���O�e�[�u��
  GAMEBEACON_INFO*      tmpInfo;
  u16                   tmpTime;

  BOOL      active;
  int       seq;
  LOG_CTRL  ctrl;

  ////////////////////////////////////////
  HEAPID      heapID;
  HEAPID      tmpHeapID;
  ARCHANDLE*  arc_handle;

  GFL_TCBLSYS*  pTcbSys;

  GFL_FONT *fontHandle;
  PRINT_QUE *printQue;
  WORDSET *wordset;
  GFL_MSGDATA *mm_status;
  STRBUF *strbuf_temp;
  STRBUF *strbuf_expand;

  BMPOAM_SYS_PTR bmpOam;
  GFL_CLUNIT* cellUnit;
  GFL_CLSYS_REND* cellRender;
  GFL_CLACTPOS  cellSurfacePos;
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
  RES2D_PLTT  resPlttPanel;
}BEACON_VIEW;


