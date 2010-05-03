//============================================================================
/**
 *  @file   zukan_detail_map.c
 *  @brief  �}�ӏڍ׉�ʂ̎p���
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *  ���W���[�����FZUKAN_DETAIL_MAP
 */
//============================================================================


//#define DEBUG_SET_ANIME_AND_POS  // ���ꂪ��`����Ă���Ƃ��A��������ꏊ�ɒu��OBJ�̃A�j���w��A�ʒu�w��p�̃f�[�^�������[�h�ɂȂ�
//#define DEBUG_KAWADA


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "ui/ui_easy_clwk.h"
#include "pokeicon/pokeicon.h"
#include "gamesystem/gamedata_def.h"
#include "gamesystem/game_data.h"
#include "gamesystem/pm_season.h"
#include "savedata/zukan_savedata.h"
#include "system/bmp_winframe.h"
#include "print/printsys.h"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "sound/pm_sndsys.h"
#include "field/zonedata.h"
#include "app/townmap_data_sys.h"
#include "app/townmap_util.h"

#include "../../../../../resource/fldmapdata/zonetable/zone_id.h"

#if	PM_VERSION == VERSION_BLACK
#include "../../../../../resource/zukan_data/zkn_area_b_zone_group.cdat"
#else
#include "../../../../../resource/zukan_data/zkn_area_w_zone_group.cdat"
#endif

#include "zukan_detail_def.h"
#include "zukan_detail_common.h"
#include "zukan_detail_graphic.h"
#include "zukan_detail_map.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_zkn.h"
#include "townmap_gra.naix"
//#include "zukan_area_w.naix"  // �����ɒ�`����Ă���enum�l��
//#include "zukan_area_b.naix"  // �g���Ă��Ȃ��̂ŕs�v�ł��B
#include "zukan_gra.naix"

// �T�E���h

// �I�[�o�[���C
FS_EXTERN_OVERLAY(ui_common);
FS_EXTERN_OVERLAY(townmap);


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// ���C��BG�t���[��
#define BG_FRAME_M_ROOT           (GFL_BG_FRAME2_M)
#define BG_FRAME_M_MAP            (GFL_BG_FRAME3_M)
#define BG_FRAME_M_TEXT           (GFL_BG_FRAME0_M)
// ���C��BG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_M_ROOT       (2)
#define BG_FRAME_PRI_M_MAP        (3)
#define BG_FRAME_PRI_M_TEXT       (1)

// ���C��BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_TEXT          = 1,
};
// �ʒu
enum
{
  BG_PAL_POS_M_TEXT          = 0,
};

// ���C��OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_TM           = 5,
  OBJ_PAL_NUM_M_ZUKAN        = 2,
  OBJ_PAL_NUM_M_EXIST        = 1,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_TM           = 0,
  OBJ_PAL_POS_M_ZUKAN        = 5,
  OBJ_PAL_POS_M_EXIST        = 7,
};

// �T�uBG�t���[��
#define BG_FRAME_S_TEXT           (GFL_BG_FRAME2_S)
#define BG_FRAME_S_PLACE          (GFL_BG_FRAME3_S)
#define BG_FRAME_S_REAR           (GFL_BG_FRAME0_S)
// �T�uBG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_S_TEXT       (1)
#define BG_FRAME_PRI_S_PLACE      (2)
#define BG_FRAME_PRI_S_REAR       (3)

// �T�uBG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_S_PLACE             = 8,
  BG_PAL_NUM_S_TEXT              = 1,
  BG_PAL_NUM_S_REAR              = ZKNDTL_COMMON_REAR_BG_PAL_NUM,
};
// �ʒu
enum
{
  BG_PAL_POS_S_PLACE             = 0,
  BG_PAL_POS_S_TEXT              = 8,
  BG_PAL_POS_S_REAR              = 9,
};

// �T�uOBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_S_POKEICON         = 3,
};
// �ʒu
enum
{
  OBJ_PAL_POS_S_POKEICON         = 0,
};


// ProcMain�̃V�[�P���X
enum
{
  SEQ_START      = 0,
  SEQ_PREPARE,
  SEQ_FADE_IN,
  SEQ_MAIN,
  SEQ_FADE_OUT,
  SEQ_END,
};

// �I������
typedef enum
{
  END_CMD_NONE,
  END_CMD_INSIDE,
  END_CMD_OUTSIDE,
}
END_CMD;

// BG�ʐݒ�
typedef struct
{
	u32									frame;
	GFL_BG_BGCNT_HEADER	bgcnt_header;
	u32									mode;
	BOOL								is_visible;
}
BGSETUP;

// GFL_BG_FRAME2_M��GFL_BG_FRAME3_M���A�t�B���g��BG�ɂ���
static const BGSETUP sc_bgsetup_affineex[]	=
{	
	//MAIN------------------------
	{	
		GFL_BG_FRAME2_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x0800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x128,//��ذ��ް��A����ް��A��׻���
          // ��납����Ȃ����ƁI 256x64�����E�I  // �{����GFL_BG_CHRSIZ256_256x128���w�肵�Ă��ׂ��Ȃ񂾂낤�Ȃ�
          // GFL_BG_CHRSIZ_256x128��0x4000�Ȃ̂ł��傤�ǃT�C�Y����v���Ă���
          // 0x10000�`0x14000    64�o�C�g*32�L����*8�L����
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
          // GFL_BG_MODE_256X16�ł�bgExtPltt���g�p���Ȃ��̂łĂ��Ɓ[�Ȓl
		},
		GFL_BG_MODE_256X16,//BG�̎��
		TRUE,	//�����\��
	},
	{	
		GFL_BG_FRAME3_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,//GFL_BG_CHRSIZ256_256x256,//��ذ��ް��A����ް��A��׻���
          // ��납����Ȃ����ƁI 256x192�����E�I  // �{����GFL_BG_CHRSIZ256_256x256���w�肵�Ă��ׂ��Ȃ񂾂낤�Ȃ�
          // GFL_BG_CHRSIZ_256x256��0x8000�Ȃ̂ŃT�C�Y��0xC000�ɂ�0x4000�قǑ���Ȃ������v���ȁH
          // GFL_BG_CHRSIZ256_256x256��0x10000�Ȃ̂�128K���͂ݏo���Ă��܂��̂ŁA�͂ݏo�����͑���Ȃ��ق����������ȁH
          // 0x14000�`0x20000    64�o�C�g*32�L����*24�L����
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
          // GFL_BG_MODE_256X16�ł�bgExtPltt���g�p���Ȃ��̂�GX_BG_EXTPLTT_01�͂Ă��Ɓ[�Ȓl
		},
		GFL_BG_MODE_256X16,//BG�̎��
		TRUE,	//�����\��
	},
};

// GFL_BG_FRAME2_M��GFL_BG_FRAME3_M���e�L�X�gBG�ɂ���
static const BGSETUP sc_bgsetup_text[]	=
{	
	//MAIN------------------------
	{	
		GFL_BG_FRAME2_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x0800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
	{	
		GFL_BG_FRAME3_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x18000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
};

/*
TWL_ProgramingManual.pdf

p43
3.2 TWL�v���Z�b�T��������
�} 3-15 BG �g���p���b�g�X���b�g �������}�b�v
0x00008000
0x00006000  �X���b�g3
0x00004000  �X���b�g2
0x00002000  �X���b�g1
0x00000000  �X���b�g0

p100
6.2.2 BG�R���g���[��
�g���p���b�g���C�l�[�u���̂Ƃ��ABG2 �͊g���p���b�g�X���b�g2 ���g�p���ABG3 �͊g���p���b�g�X���b�g3 ���g�p��
�܂��BBG2 �����3 �ɂ����āA�g�p����g���p���b�gNo.��ύX���邱�Ƃ͂ł��܂���B
*/


// ���b�Z�[�W�f�[�^
enum
{
  MSG_ZUKAN,
  MSG_PLACE,
  MSG_MAX
};

// �e�L�X�g
enum
{
  TEXT_POKENAME,
  TEXT_PLACENAME,
  TEXT_SEASON,
  TEXT_UNKNOWN,
  TEXT_MAX
};

// �e�L�X�g�\���ɕK�v�ȃ_�~�[�̃r�b�g�}�b�v�E�B���h�E
enum
{
  TEXT_DUMMY_UP,
  TEXT_DUMMY_DOWN,
  TEXT_DUMMY_MAX
};

// ������
#define STRBUF_POKENAME_LENGTH  ( 64 )  // [�|�P�����푰��]�̂Ԃ��  // buflen.h�Ŋm�F����K�v����I

// �|�P�A�C�R���̈ʒu
static const u8 pokeicon_pos[4] = { 56, 112 +8*8,     // �X�N���[���O(����A����A�ނ�A�C�R���������Ă��Ȃ��Ƃ�)
                                    56, 112       };  // �X�N���[����(����A����A�ނ�A�C�R���������Ă���Ƃ�)

// ���
typedef enum
{
  STATE_TOP,
  STATE_SELECT,
}
STATE;

typedef enum
{
  SCROLL_STATE_DOWN,
  SCROLL_STATE_DOWN_TO_UP,
  SCROLL_STATE_UP,
  SCROLL_STATE_UP_TO_DOWN,
}
SCROLL_STATE;

#define SCROLL_VEL (8)  // ���ʂ̃X�N���[���̑��x(1�t���[���ɂ��ꂾ���ړ�����)

#define SEASON_R_ARROW_POS_X  (17*8)
#define SEASON_R_ARROW_POS_Y  (0)
#define SEASON_R_ARROW_SIZE_X (3*8)
#define SEASON_R_ARROW_SIZE_Y (2*8)

#define SEASON_R_ARROW_ANMSEQ_NORMAL (2)
#define SEASON_R_ARROW_ANMSEQ_SELECT (3)

#define SEASON_L_ARROW_POS_X  (0*8)
#define SEASON_L_ARROW_POS_Y  (0)
#define SEASON_L_ARROW_SIZE_X (3*8)
#define SEASON_L_ARROW_SIZE_Y (2*8)

#define SEASON_L_ARROW_ANMSEQ_NORMAL (4)
#define SEASON_L_ARROW_ANMSEQ_SELECT (5)

#define SEASON_PANEL_POS_X  (0)
#define SEASON_PANEL_POS_Y  (0)
#define SEASON_PANEL_SIZE_X (20*8)
#define SEASON_PANEL_SIZE_Y (3*8)

#define DATA_IDX_NONE (0xFF)

// �J�[�\���̈ړ��\�͈� MIN<= <=MAX
#define CURSOR_MOVE_RECT_X_MIN (0)                     // prog/src/app/townmap/townmap.c  // CURSOR_MOVE_LIMIT_TOP�Ȃǂ��Q�l�ɂ����B
#define CURSOR_MOVE_RECT_Y_MIN (8)//(16)
#define CURSOR_MOVE_RECT_X_MAX (256-8)//(256 -16 -1)
#define CURSOR_MOVE_RECT_Y_MAX (168)//(168 -1)
// �J�[�\���̃f�t�H���g�ʒu
#define CURSOR_DEFAULT_POS_X (128)
#define CURSOR_DEFAULT_POS_Y (96)

// �J�[�\���̈ړ�
#define CURSOR_ADD_SPEED		(3)              // �J�[�\���̈ړ���
#define PLACE_PULL_R_SQ			(12*12)          // �J�[�\�����z�����ޔ͈͂̓��
#define PLACE_PULL_STRONG		(FX32_CONST(1))	 // �z�����ދ����i�J�[�\��CURSOR_ADD_SPEED���キ�j
#define PLACE_PULL_FORCE    (FX32_SQRT3)     // �J�[�\���������Ă��Ȃ��Ƃ��A���ꖢ���̋����Ȃ狭���I�ɋz�����܂���(sqrt(2)< <2�𖞂����l)

// �����O�̃A�j��
#define RING_CUR_ANMSEQ (5)

// OBJ
enum
{
  OBJ_RES_TM_NCG,     // pokemon_wb/prog/src/app/townmap/townmap_grh.c �Q�l
  OBJ_RES_TM_NCL,
  OBJ_RES_TM_NCE,
  OBJ_RES_ZUKAN_NCG,
  OBJ_RES_ZUKAN_NCL,
  OBJ_RES_ZUKAN_NCE,
  OBJ_RES_EXIST_NCG,
  OBJ_RES_EXIST_NCL,
  OBJ_RES_EXIST_NCE,
  OBJ_RES_MAX,
};
enum
{
  OBJ_ZUKAN_START,                          // OBJ_ZUKAN_START<= <OBJ_ZUKAN_END
  OBJ_SEASON          = OBJ_ZUKAN_START,
  OBJ_SEASON_L,
  OBJ_SEASON_R,
  OBJ_UNKNOWN,
  OBJ_ZUKAN_END,

  OBJ_TM_START        = OBJ_ZUKAN_END,      // OBJ_TM_START<= <OBJ_TM_END
  OBJ_CURSOR          = OBJ_TM_START,
  OBJ_RING_CUR,
  OBJ_HERO,
  OBJ_TM_END,

  OBJ_MAX             = OBJ_TM_END,
};

// obj_exist�̃A���t�@�A�j���[�V����sin�g���̂�0�`0xFFFF�̃��[�v
#define OBJ_EXIST_ALPHA_ANIME_COUNT_ADD (0x400)    // cos�g���̂�0�`0xFFFF�̃��[�v
#define OBJ_EXIST_ALPHA_ANIME_COUNT_MAX (0x10000)  // �܂܂Ȃ�
#define OBJ_EXIST_ALPHA_MIN  ( 1)  // 0<= <=16
#define OBJ_EXIST_ALPHA_MAX  (15)  // OBJ_EXIST_ALPHA_MIN <= <= OBJ_EXIST_ALPHA_MAX


// �e�|�P�����̏��
// ��N�����G�߂��Ƃ�
typedef enum
{
  APPEAR_RULE_YEAR,
  APPEAR_RULE_SEASON,
}
APPEAR_RULE;
// �����n
typedef enum
{
  HABITAT_UNKNOWN,  // �����n�s��
  HABITAT_KNOWN,    // �����n����
}
HABITAT;

// �����A���A�ނ�̃A�C�R��
enum
{
  PLACE_ICON_GROUND,
  PLACE_ICON_WATER,
  PLACE_ICON_FISHING,
  PLACE_ICON_MAX,
};
// �����A���A�ނ�̃A�C�R���̃p���b�g
static u8 place_icon_bg_pal_pos[PLACE_ICON_MAX][2] =  // 0=OFF, 1=ON
{
  { BG_PAL_POS_S_PLACE   + 5,    BG_PAL_POS_S_PLACE   + 2 },
  { BG_PAL_POS_S_PLACE   + 6,    BG_PAL_POS_S_PLACE   + 0 },
  { BG_PAL_POS_S_PLACE   + 7,    BG_PAL_POS_S_PLACE   + 4 },
};
// �����A���A�ނ�̃A�C�R���̃X�N���[���̃L�������W�A�T�C�Y
static u8 place_icon_screen_chr[PLACE_ICON_MAX][4] =  // 0=X, 1=Y, 2=W, 3=H
{
  {  7, 28,  4,  3 },
  { 14, 28,  4,  3 },
  { 21, 28,  4,  3 },
};


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���z�f�[�^(�|�P����1�C��)
//=====================================
enum
{
  PLACE_BITFLAG_NONE         = 0,
  PLACE_BITFLAG_GROUND_L     = 1<< 0,
  PLACE_BITFLAG_GROUND_H     = 1<< 1, 
  PLACE_BITFLAG_GROUND_SP    = 1<< 2,
  PLACE_BITFLAG_WATER        = 1<< 3,
  PLACE_BITFLAG_WATER_SP     = 1<< 4,
  PLACE_BITFLAG_FISHING      = 1<< 5,
  PLACE_BITFLAG_FISHING_SP   = 1<< 6,

  PLACE_BITFLAG_GROUND_ALL   = ( PLACE_BITFLAG_GROUND_L | PLACE_BITFLAG_GROUND_H | PLACE_BITFLAG_GROUND_SP ),
  PLACE_BITFLAG_WATER_ALL    = ( PLACE_BITFLAG_WATER | PLACE_BITFLAG_WATER_SP ),
  PLACE_BITFLAG_FISHING_ALL  = ( PLACE_BITFLAG_FISHING | PLACE_BITFLAG_FISHING_SP ),
};
typedef struct
{
  u8    unknown;  // 0�̂Ƃ��s���łȂ��A1�̂Ƃ��s��
  u8    place_bitflag[TOWNMAP_DATA_MAX];  // TOWNMAP_DATA�̃C���f�b�N�X��z��Y�����Ɏg����悤�ɕ��ׂĂ���
}
AREA_SEASON_DATA;
typedef struct
{
  u8                 year;  // 0�̂Ƃ���N�������łȂ��A1�̂Ƃ���N������(�Ȃ̂ŏt�̃f�[�^��������΂悢)
  AREA_SEASON_DATA   season_data[PMSEASON_TOTAL];  // PMSEASON_SPRING PMSEASON_SUMMER PMSEASON_AUTUMN PMSEASON_WINTER
}
AREA_DATA;

//-------------------------------------
/// �B���X�|�b�g
//=====================================
typedef enum
{
  HIDE_STATE_NO,          // �B���X�|�b�g�ł͂Ȃ�
  HIDE_STATE_HIDE_TRUE,   // �B���X�|�b�g�ŉB����
  HIDE_STATE_HIDE_FALSE,  // �B���X�|�b�g�ŕ\����
}
HIDE_STATE;

typedef struct
{
  HIDE_STATE    state;
  GFL_CLWK*     obj_clwk;  // state��HIDE_STATE_NO�܂���HIDE_STATE_HIDE_TRUE�̂Ƃ���NULL
}
HIDE_SPOT;


//-------------------------------------
/// PROC ���[�N
//=====================================
typedef struct
{
  // ���̂Ƃ��납��ؗp�������
  GFL_CLUNIT*                 clunit;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que;

  ZKNDTL_COMMON_WORK*         cmn;  // �e�֐��Ɉ����œn�����cmn�Ɠ������́BVBlank�֐������痘�p�������̂Ń��[�N�Ɏ������Ă����B

  // �����ō쐬�������
  ZKNDTL_COMMON_REAR_WORK*    rear_wk_s;

  // ���b�Z�[�W�ƃe�L�X�g
  GFL_MSGDATA*                msgdata[MSG_MAX];
  GFL_BMPWIN*                 text_bmpwin[TEXT_MAX];
  GFL_BMPWIN*                 text_dummy_bmpwin[TEXT_DUMMY_MAX];

  // ����
  UI_EASY_CLWK_RES            pokeicon_res;    // pokeicon_clwk��NULL�̂Ƃ��A�g�p���Ă��Ȃ�
  GFL_CLWK*                   pokeicon_clwk;   // NULL�̂Ƃ��Ȃ�
  GFL_ARCUTIL_TRANSINFO       place_tinfo;

  // ���
  STATE                       state;
  SCROLL_STATE                scroll_state;
  SCROLL_STATE                pokeicon_scroll_state;
  int                         ktst;  // GFL_APP_END_KEY(GFL_APP_KTST_KEY) or GFL_APP_END_TOUCH(GFL_APP_KTST_TOUCH)
  u8                          season_id;  // PMSEASON_SPRING PMSEASON_SUMMER PMSEASON_AUTUMN PMSEASON_WINTER
  u8                          select_data_idx;  // �X���_���W���������H���I��ł��Ȃ��Ƃ�DATA_IDX_NONE

  // �e�|�P�����̏��
  APPEAR_RULE                 appear_rule;
  HABITAT                     habitat;
  AREA_DATA*                  area_data;

  // OBJ
  u32                         obj_res[OBJ_RES_MAX];
  GFL_CLWK*                   obj_clwk[OBJ_MAX];
  GFL_CLWK*                   obj_exist_clwk[TOWNMAP_DATA_MAX];
  GFL_CLUNIT*                 usual_clunit;  // �s�K�v�����������A�r���Ńe�X�g���~�߂��̂ōŏI���_�܂ł͏o���Ă��Ȃ��B
  GFL_CLSYS_REND*             usual_rend;    // �����_���[�S�̂ɑ΂��Ĕ������ݒ肪���f�����Ǝv���Ă��Ă��������́B
  int                         obj_exist_ev1;
  u16                         obj_exist_alpha_anime_count;

  // �B���X�|�b�g
  HIDE_SPOT                   hide_spot[TOWNMAP_DATA_MAX];

  // �^�E���}�b�v�f�[�^
  TOWNMAP_DATA*               townmap_data;

  // �^�E���}�b�v�f�[�^�̃C���f�b�N�X����zkn_area_monsno_???.dat���̃C���f�b�N�X�𓾂�z��
  u8 townmap_data_idx_to_zkn_area_idx[TOWNMAP_DATA_MAX];
  // zukan_area_?.narc�̃n���h����ێ����Ă���
  ARCHANDLE* area_handle;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;
  u8                          vblank_req;  // 0=�Ȃ�; 1=GENERAL; 2=MAP;
  
  // �t�F�[�h
  ZKNDTL_COMMON_FADE_WORK*    fade_wk_m;
  ZKNDTL_COMMON_FADE_WORK*    fade_wk_s;

  // �I������
  END_CMD                     end_cmd;
}
ZUKAN_DETAIL_MAP_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Zukan_Detail_Map_VBlankFunc( GFL_TCB* tcb, void* wk );

// �^�E���}�b�v�f�[�^�̃C���f�b�N�X����zkn_area_monsno_???.dat���̃C���f�b�N�X�𓾂�z����쐬����
static void Zukan_Detail_Map_TownmapDataIdxToZknAreaIdxArrayMake( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �A�t�B���g��BG
static void Zukan_Detail_Map_AffineExBGInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_AffineExBGExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// OBJ
static void Zukan_Detail_Map_ObjInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_ObjExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_ObjExistInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_ObjExistExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
// obj_exist�̃A���t�@�A�j���[�V����
static void Zukan_Detail_Map_ObjExistAlphaInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_ObjExistAlphaExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_ObjExistAlphaMain( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_ObjExistAlphaReset( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �B���X�|�b�g
static void Zukan_Detail_Map_HideSpotInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_HideSpotExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// ���b�Z�[�W�ƃe�L�X�g
static void Zukan_Detail_Map_MsgTextInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_MsgTextExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// ����
static void Zukan_Detail_Map_PlaceInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_PlaceExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_PlaceMain( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_PlaceMainScroll( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_PlaceMainIcon( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �����
static void Zukan_Detail_Map_SeasonArrowMain( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �|�P�A�C�R��
static GFL_CLWK* PokeiconInit( UI_EASY_CLWK_RES* res, GFL_CLUNIT* clunit, HEAPID heap_id, u32 mons, u32 form_no, u32 sex, BOOL egg, u8 x, u8 y );
static void PokeiconExit( UI_EASY_CLWK_RES* res, GFL_CLWK* clwk );
// �}�N��
#define BLOCK_POKEICON_EXIT(res,clwk)                     \
    {                                                     \
      if( clwk ) PokeiconExit( res, clwk );               \
      clwk = NULL;                                        \
    }
// NULL�������Y��Ȃ��悤�Ƀ}�N�����g���悤�ɂ��Ă���

// �v���C���[�̈ʒu�̃f�[�^�C���f�b�N�X�𓾂�
static u8 Zukan_Detail_Map_GetPlayerPosDataIdx( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
// �v���C���[�̈ʒu��OBJ��z�u����
static void Zukan_Detail_Map_SetPlayer( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
// �J�[�\����z�u����
static void Zukan_Detail_Map_SetCursor( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
// �v���C���[�̈ʒu�ɃJ�[�\����z�u����
static void Zukan_Detail_Map_SetCursorOnPlayer( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �w�肵�����W�ƃq�b�g����f�[�^������΂��̃f�[�^�C���f�b�N�X��Ԃ�
static u8 Zukan_Detail_Map_Hit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                u8 x, u8 y );
// �w�肵�����W�͋z�����ݔ͈͓���
static BOOL Zukan_Detail_Map_IsPullHit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                        u8 data_idx, u8 x, u8 y, u32* distance_sq );
// �w�肵�����W���z�����ݔ͈͓��Ɏ��f�[�^������΂��̃f�[�^�C���f�b�N�X��Ԃ�
static u8 Zukan_Detail_Map_PullHit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                u8 x, u8 y );

// ����
static void Zukan_Detail_Map_Input( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
// �}�b�v��̋��_�̑I�𑀍�
static void Zukan_Detail_Map_InputSelect( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// ��Ԃ�J�ڂ�����
static void Zukan_Detail_Map_ChangeState( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                 STATE state );
static void Zukan_Detail_Map_ChangePoke( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_ChangePlace( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_ChangeSeason( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn, BOOL b_next );

// ���z�f�[�^
static AREA_DATA* Zukan_Detail_Map_AreaDataLoad( u16 monsno, HEAPID heap_id, ARCHANDLE* handle );
static void Zukan_Detail_Map_AreaDataUnload( AREA_DATA* area_data );

// ���x���o�Ă��鏈��
static void Zukan_Detail_Map_UtilPrintSeason( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_UtilDrawSeasonArea( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_UtilBrightenPlaceIcon( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �A���t�@�ݒ�
static void Zukan_Detail_Map_AlphaInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_AlphaExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Map_ProcInit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static ZKNDTL_PROC_RESULT Zukan_Detail_Map_ProcExit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static ZKNDTL_PROC_RESULT Zukan_Detail_Map_ProcMain( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Map_CommandFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn, ZKNDTL_COMMAND cmd );

const ZKNDTL_PROC_DATA    ZUKAN_DETAIL_MAP_ProcData =
{
  Zukan_Detail_Map_ProcInit,
  Zukan_Detail_Map_ProcMain,
  Zukan_Detail_Map_ProcExit,
  Zukan_Detail_Map_CommandFunc,
  NULL,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief           
 *
 *  @param[in,out]   
 *
 *  @retval          
 */
//------------------------------------------------------------------
void             ZUKAN_DETAIL_MAP_InitParam(
                            ZUKAN_DETAIL_MAP_PARAM*  param,
                            HEAPID                    heap_id )
{
  param->heap_id = heap_id;
}


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Map_ProcInit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_MAP_PARAM*    param    = (ZUKAN_DETAIL_MAP_PARAM*)pwk;
  ZUKAN_DETAIL_MAP_WORK*     work;

  const GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "Zukan_Detail_Map_ProcInit\n" );
  }
#endif

  // �I�[�o�[���C
  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );
  GFL_OVERLAY_Load( FS_OVERLAY_ID(townmap) );

  // �q�[�v
  {
    work = ZKNDTL_PROC_AllocWork(proc, sizeof(ZUKAN_DETAIL_MAP_WORK), param->heap_id);
    GFL_STD_MemClear( work, sizeof(ZUKAN_DETAIL_MAP_WORK) ); 
  }

  // ���̂Ƃ��납��ؗp�������
  {
    ZUKAN_DETAIL_GRAPHIC_WORK* graphic = ZKNDTL_COMMON_GetGraphic(cmn);
    work->clunit      = ZUKAN_DETAIL_GRAPHIC_GetClunit(graphic);
    work->font        = ZKNDTL_COMMON_GetFont(cmn);
    work->print_que   = ZKNDTL_COMMON_GetPrintQue(cmn);

    work->cmn         = cmn;
  }

  // NULL������
  {
    work->pokeicon_clwk = NULL;
    work->area_data     = NULL;
  }

  // ��ԏ�����
  {
    work->state                    = STATE_TOP;
    work->scroll_state             = SCROLL_STATE_DOWN;
    work->pokeicon_scroll_state    = SCROLL_STATE_DOWN;
    work->ktst                     = GFL_UI_CheckTouchOrKey();
    work->season_id                = GAMEDATA_GetSeasonID(gamedata);
    work->select_data_idx          = DATA_IDX_NONE;
  }

  // �^�E���}�b�v�f�[�^
  work->townmap_data = TOWNMAP_DATA_Alloc( param->heap_id );

  // �^�E���}�b�v�f�[�^�̃C���f�b�N�X����zkn_area_monsno_???.dat���̃C���f�b�N�X�𓾂�z����쐬����
  Zukan_Detail_Map_TownmapDataIdxToZknAreaIdxArrayMake( param, work, cmn );
  // zukan_area_?.narc�̃n���h����ێ����Ă���
  work->area_handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_AREA, param->heap_id );  // arc_tool.lst�ɂ���āA�o�[�W�����ɂ����ARCID_ZUKAN_AREA�œǂݍ��ރt�@�C�����ς��悤�ɂȂ��Ă���B

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zukan_Detail_Map_VBlankFunc, work, 1 );
  work->vblank_req = 0;

  // �t�F�[�h
  {
    work->fade_wk_m = ZKNDTL_COMMON_FadeInit( param->heap_id );
    work->fade_wk_s = ZKNDTL_COMMON_FadeInit( param->heap_id );
   
    ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_m );
    ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_s );

    ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_M, work->fade_wk_m );
    ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_S, work->fade_wk_s );
  }

  // �I�����
  work->end_cmd = END_CMD_NONE;

  return ZKNDTL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Map_ProcExit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_MAP_PARAM*    param    = (ZUKAN_DETAIL_MAP_PARAM*)pwk;
  ZUKAN_DETAIL_MAP_WORK*     work     = (ZUKAN_DETAIL_MAP_WORK*)mywk;

  // ���z�f�[�^
  Zukan_Detail_Map_AreaDataUnload( work->area_data );

  // �|�P�A�C�R��
  BLOCK_POKEICON_EXIT( &work->pokeicon_res, work->pokeicon_clwk )

  // ����
  Zukan_Detail_Map_PlaceExit( param, work, cmn );
  // ���b�Z�[�W�ƃe�L�X�g 
  Zukan_Detail_Map_MsgTextExit( param, work, cmn );
 
  // �Ŕw��
  ZKNDTL_COMMON_RearExit( work->rear_wk_s );

  // OBJ
  Zukan_Detail_Map_HideSpotExit( param, work, cmn );
  Zukan_Detail_Map_ObjExistExit( param, work, cmn );
  Zukan_Detail_Map_ObjExit( param, work, cmn );

  // �t�F�[�h
  {
    ZKNDTL_COMMON_FadeExit( work->fade_wk_s );
    ZKNDTL_COMMON_FadeExit( work->fade_wk_m );
  }

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �ێ����Ă���zukan_area_?.narc�̃n���h��
  GFL_ARC_CloseDataHandle( work->area_handle );

  // �^�E���}�b�v�f�[�^
  TOWNMAP_DATA_Free( work->townmap_data );

  // ���
  GFL_UI_SetTouchOrKey( work->ktst );

  // �q�[�v
  ZKNDTL_PROC_FreeWork( proc );

  // �I�[�o�[���C
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(townmap) );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "Zukan_Detail_Map_ProcExit\n" );
  }
#endif

  return ZKNDTL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Map_ProcMain( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_MAP_PARAM*    param    = (ZUKAN_DETAIL_MAP_PARAM*)pwk;
  ZUKAN_DETAIL_MAP_WORK*     work     = (ZUKAN_DETAIL_MAP_WORK*)mywk;

  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  ZUKAN_DETAIL_HEADBAR_WORK*   headbar  = ZKNDTL_COMMON_GetHeadbar(cmn);
 
  switch(*seq)
  {
  case SEQ_START:
    {
      *seq = SEQ_PREPARE;

      // �؂�ւ�
      {
        // �O���t�B�b�N�X���[�h�ݒ�
        GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS, GX_BGMODE_5, GX_BG0_AS_2D );
        
        // BG�ʐݒ�
        {	
          int i;
          for( i = 0; i < NELEMS(sc_bgsetup_affineex); i++ )
          {	
            GFL_BG_FreeBGControl( sc_bgsetup_affineex[i].frame );
            
            GFL_BG_SetBGControl( sc_bgsetup_affineex[i].frame, &sc_bgsetup_affineex[i].bgcnt_header, sc_bgsetup_affineex[i].mode );
            GFL_BG_ClearFrame( sc_bgsetup_affineex[i].frame );
            GFL_BG_SetVisible( sc_bgsetup_affineex[i].frame, sc_bgsetup_affineex[i].is_visible );
          }
        }
      }

      // BG
      {
        // �N���A
        u8 i;
        for( i=GFL_BG_FRAME0_M; i<=GFL_BG_FRAME3_S; i++ )
        {
          if(    i != ZKNDTL_BG_FRAME_M_TOUCHBAR
              && i != ZKNDTL_BG_FRAME_S_HEADBAR )
          {
            GFL_BG_SetScroll( i, GFL_BG_SCROLL_X_SET, 0 );
            GFL_BG_SetScroll( i, GFL_BG_SCROLL_Y_SET, 0 );
			      GFL_BG_ClearFrame(i);
          }
        }

        // ���C��BG
        GFL_BG_SetPriority( BG_FRAME_M_ROOT, BG_FRAME_PRI_M_ROOT );
        GFL_BG_SetPriority( BG_FRAME_M_MAP,  BG_FRAME_PRI_M_MAP );
        GFL_BG_SetPriority( BG_FRAME_M_TEXT, BG_FRAME_PRI_M_TEXT );
        
        // �T�uBG
        GFL_BG_SetPriority( BG_FRAME_S_TEXT,  BG_FRAME_PRI_S_TEXT );
        GFL_BG_SetPriority( BG_FRAME_S_PLACE, BG_FRAME_PRI_S_PLACE );
        GFL_BG_SetPriority( BG_FRAME_S_REAR,  BG_FRAME_PRI_S_REAR );
      }

      // �A�t�B���g��BG
      Zukan_Detail_Map_AffineExBGInit( param, work, cmn );
      // OBJ
      Zukan_Detail_Map_ObjInit( param, work, cmn );
      Zukan_Detail_Map_ObjExistInit( param, work, cmn );
      Zukan_Detail_Map_HideSpotInit( param, work, cmn );
      
      // �Ŕw��
      work->rear_wk_s = ZKNDTL_COMMON_RearInit( param->heap_id, ZKNDTL_COMMON_REAR_TYPE_MAP,
          BG_FRAME_S_REAR, BG_PAL_POS_S_REAR +0, BG_PAL_POS_S_REAR +1 );

      // ���b�Z�[�W�ƃe�L�X�g 
      Zukan_Detail_Map_MsgTextInit( param, work, cmn );
      // ����
      Zukan_Detail_Map_PlaceInit( param, work, cmn );

      // �v���C���[�̈ʒu��OBJ��z�u����
      Zukan_Detail_Map_SetPlayer( param, work, cmn );

      // ���̉�ʂɗ����Ƃ��ɑI�΂�Ă����|�P������\������
      Zukan_Detail_Map_ChangePoke( param, work, cmn );
    }
    break;
  case SEQ_PREPARE:
    {
      *seq = SEQ_FADE_IN;

      // �t�F�[�h
      ZKNDTL_COMMON_FadeSetBlackToColorless( work->fade_wk_m );
      ZKNDTL_COMMON_FadeSetBlackToColorless( work->fade_wk_s );

      // �^�b�`�o�[
      if( ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) != ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR )
      {
        ZUKAN_DETAIL_TOUCHBAR_SetType(
            touchbar,
            ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL,
            ZUKAN_DETAIL_TOUCHBAR_DISP_MAP );
        ZUKAN_DETAIL_TOUCHBAR_Appear( touchbar, ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE );
      }
      else
      {
        ZUKAN_DETAIL_TOUCHBAR_SetDispOfGeneral(
            touchbar,
            ZUKAN_DETAIL_TOUCHBAR_DISP_MAP );
      }
      {
        GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
        ZUKAN_DETAIL_TOUCHBAR_SetCheckFlipOfGeneral(
            touchbar,
            GAMEDATA_GetShortCut( gamedata, SHORTCUT_ID_ZUKAN_MAP ) );
      }
      // �^�C�g���o�[
      if( ZUKAN_DETAIL_HEADBAR_GetState( headbar ) != ZUKAN_DETAIL_HEADBAR_STATE_APPEAR )
      {
        ZUKAN_DETAIL_HEADBAR_SetType(
            headbar,
            ZUKAN_DETAIL_HEADBAR_TYPE_MAP );
        ZUKAN_DETAIL_HEADBAR_Appear( headbar );
      }
    }
    break;
  case SEQ_FADE_IN:
    {
      if(    (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_m ))
          && (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_s ))
          && ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) == ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR
          && ZUKAN_DETAIL_HEADBAR_GetState( headbar ) == ZUKAN_DETAIL_HEADBAR_STATE_APPEAR )
      {
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );

        Zukan_Detail_Map_ObjExistAlphaInit( param, work, cmn );
        Zukan_Detail_Map_AlphaInit( param, work, cmn );

        *seq = SEQ_MAIN;
      }
    }
    break;
  case SEQ_MAIN:
    {
      if( work->end_cmd != END_CMD_NONE )
      {
        Zukan_Detail_Map_AlphaExit( param, work, cmn );
        Zukan_Detail_Map_ObjExistAlphaExit( param, work, cmn );

        *seq = SEQ_FADE_OUT;

        // �t�F�[�h
        ZKNDTL_COMMON_FadeSetColorlessToBlack( work->fade_wk_m );
        ZKNDTL_COMMON_FadeSetColorlessToBlack( work->fade_wk_s );
        
        // �^�C�g���o�[
        ZUKAN_DETAIL_HEADBAR_Disappear( headbar );
        
        if( work->end_cmd == END_CMD_OUTSIDE )
        {
          // �^�b�`�o�[
          ZUKAN_DETAIL_TOUCHBAR_Disappear( touchbar, ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE );
        }
      }
      else
      {
        Zukan_Detail_Map_ObjExistAlphaMain( param, work, cmn );

        // ����
        Zukan_Detail_Map_Input( param, work, cmn );
      }
    }
    break;
  case SEQ_FADE_OUT:
    {
      if(    (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_m ))
          && (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_s ))
          && ZUKAN_DETAIL_HEADBAR_GetState( headbar ) == ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR )
      {
        if( work->end_cmd == END_CMD_OUTSIDE )
        {
          if( ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) == ZUKAN_DETAIL_TOUCHBAR_STATE_DISAPPEAR )
          {
            *seq = SEQ_END;
          }
        }
        else
        {
          *seq = SEQ_END;
        }
      }
    }
    break;
  case SEQ_END:
    {
      // �A�t�B���g��BG
      Zukan_Detail_Map_AffineExBGExit( param, work, cmn );
      
      // �؂�ւ�
      {
        // �O���t�B�b�N�X���[�h�ݒ�
        GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D );

        // BG�ʐݒ�
        {	
          int i;
          for( i = 0; i < NELEMS(sc_bgsetup_text); i++ )
          {	
            GFL_BG_FreeBGControl( sc_bgsetup_text[i].frame );
            
            GFL_BG_SetBGControl( sc_bgsetup_text[i].frame, &sc_bgsetup_text[i].bgcnt_header, sc_bgsetup_text[i].mode );
            GFL_BG_ClearFrame( sc_bgsetup_text[i].frame );
            GFL_BG_SetVisible( sc_bgsetup_text[i].frame, sc_bgsetup_text[i].is_visible );
          }
        }
      }

      return ZKNDTL_PROC_RES_FINISH;
    }
    break;
  }

  if( *seq >= SEQ_PREPARE )
  {
    // �Ŕw��
    ZKNDTL_COMMON_RearMain( work->rear_wk_s );
    // ����
    Zukan_Detail_Map_PlaceMain( param, work, cmn );
    // �����
    Zukan_Detail_Map_SeasonArrowMain( param, work, cmn );
  }

  // �t�F�[�h
  ZKNDTL_COMMON_FadeMain( work->fade_wk_m, work->fade_wk_s );


#ifdef DEBUG_SET_ANIME_AND_POS
  {
    // ��������ꏊ�ɒu��OBJ�̃A�j���w��A�ʒu�w��
    if(    *seq >= SEQ_PREPARE
        && work->state == STATE_SELECT )
    {
      do
      {
        static u8 idx = 0;
      
        // ���� 
        u16 anmseq = GFL_CLACT_WK_GetAnmSeq( work->obj_exist_clwk[idx] );
        u16 anmseq_num = GFL_CLACT_WK_GetAnmSeqNum( work->obj_exist_clwk[idx] );
        GFL_CLACTPOS pos;
        GFL_CLACT_WK_GetPos( work->obj_exist_clwk[idx], &pos, CLSYS_DEFREND_MAIN );
      
        // ���\��
        if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
        {
          GFL_CLACT_WK_GetPos( work->obj_exist_clwk[idx], &pos, CLSYS_DEFREND_MAIN );
          OS_Printf( "idx=%d, zkn_anm=%d, zkn_pos_x=%d, zkn_pos_y=%d\n",
              idx,
              anmseq, 
              pos.x,
              pos.y
          );
        }
        // idx�؂�ւ�
        else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R )  // Up
        {
          idx++;
          if( idx >= TOWNMAP_DATA_MAX )
            idx = 0;
          break;  // idx��ύX�����̂ŏI��
        }
        else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L )  // Down
        {
          if( idx == 0 )
            idx = TOWNMAP_DATA_MAX -1;
          else
            idx--;
          break;  // idx��ύX�����̂ŏI��
        }
        // �A�j���؂�ւ�
        else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )  // Up
        {
          anmseq++;
          if( anmseq >= anmseq_num )
            anmseq = 0;
        }
        else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )  // Down
        {
          if( anmseq == 0 )
            anmseq = anmseq_num -1;
          else
            anmseq--;
        }
        // �ړ�
        else
        {
          if( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT )
          {
            if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
              pos.x++;
            if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
              pos.x--;
            if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
              pos.y--;
            if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
              pos.y++;
          }
          else
          {
            if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
              pos.x++;
            if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
              pos.x--;
            if( GFL_UI_KEY_GetCont() & PAD_KEY_UP )
              pos.y--;
            if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )
              pos.y++;
          }
        }

        // ��
        GFL_CLACT_WK_SetAnmSeq( work->obj_exist_clwk[idx], anmseq );
        GFL_CLACT_WK_SetPos( work->obj_exist_clwk[idx], &pos, CLSYS_DEFREND_MAIN );
      }
      while(0);
    }
  }
#endif


  return ZKNDTL_PROC_RES_CONTINUE;
}

//-------------------------------------
/// PROC ���ߏ���
//=====================================
static void Zukan_Detail_Map_CommandFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn, ZKNDTL_COMMAND cmd )
{
  if( mywk )
  {
    ZUKAN_DETAIL_MAP_PARAM*    param    = (ZUKAN_DETAIL_MAP_PARAM*)pwk;
    ZUKAN_DETAIL_MAP_WORK*     work     = (ZUKAN_DETAIL_MAP_WORK*)mywk;

    ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  
    switch( cmd )
    {
    case ZKNDTL_CMD_NONE:
      {
      }
      break;
    case ZKNDTL_CMD_CLOSE:
    case ZKNDTL_CMD_RETURN:
      {
        work->end_cmd = END_CMD_OUTSIDE;
      }
      break;
    case ZKNDTL_CMD_INFO:
    case ZKNDTL_CMD_VOICE:
    case ZKNDTL_CMD_FORM:
      {
        work->end_cmd = END_CMD_INSIDE;
      }
      break;
    case ZKNDTL_CMD_CUR_D:
      {
        u16 monsno_curr;
        u16 monsno_go;
        monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
        ZKNDTL_COMMON_GoToNextPoke(cmn);
        monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
        if( monsno_curr != monsno_go )
        {
          Zukan_Detail_Map_ChangePoke( param, work, cmn );
        }
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_CUR_U:
      {
        u16 monsno_curr;
        u16 monsno_go;
        monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
        ZKNDTL_COMMON_GoToPrevPoke(cmn);
        monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
        if( monsno_curr != monsno_go )
        {
          Zukan_Detail_Map_ChangePoke( param, work, cmn );
        }
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_CHECK:
      {
        GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
        GAMEDATA_SetShortCut(
            gamedata,
            SHORTCUT_ID_ZUKAN_MAP,
            ZUKAN_DETAIL_TOUCHBAR_GetCheckFlipOfGeneral( touchbar ) );
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_MAP_RETURN:
      {
        Zukan_Detail_Map_ChangeState( param, work, cmn, STATE_TOP );
      }
      break;
    default:
      {
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    }
  }
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void Zukan_Detail_Map_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZUKAN_DETAIL_MAP_WORK*       work     = (ZUKAN_DETAIL_MAP_WORK*)wk;

  ZKNDTL_COMMON_WORK*          cmn      = work->cmn;
  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);

  switch( work->vblank_req )
  {
  case 0:
    {
      // ���N�G�X�g�Ȃ�
    }
    break;
  case 1:
    {
      // �^�b�`�o�[
      ZUKAN_DETAIL_TOUCHBAR_SetType(
          touchbar,
          ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL,
          ZUKAN_DETAIL_TOUCHBAR_DISP_MAP );
      //������Unlock�����ق������������BSetType����Unlock��������?
    }
    break;
  case 2:
    {
      // �^�b�`�o�[
      ZUKAN_DETAIL_TOUCHBAR_SetType(
          touchbar,
          ZUKAN_DETAIL_TOUCHBAR_TYPE_MAP,
          ZUKAN_DETAIL_TOUCHBAR_DISP_MAP );
      //������Unlock�����ق������������BSetType����Unlock��������?
    }
    break;
  }
  work->vblank_req = 0;
}

//-------------------------------------
/// �^�E���}�b�v�f�[�^�̃C���f�b�N�X����zkn_area_monsno_???.dat���̃C���f�b�N�X�𓾂�z����쐬����
//=====================================
static void Zukan_Detail_Map_TownmapDataIdxToZknAreaIdxArrayMake( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;
  u8 j;

  // 1�x�����쐬����΂悢

  // �g����
  // u8 zkn_area_idx = townmap_data_idx_to_zkn_area_idx[townmap_data_idx];
  // townmap_data_idx��TOWNMAP_DATA_GetParam�ɓn��idx�Ɠ����B
  // zkn_area_idx��zkn_area_monsno_???.dat���̉��Ԗڂ̃f�[�^����\���B
  // �����AAREA_SEASON_DATA��place_bitflag[zkn_area_idx]�Ƃ������Ɏg����B
  // �^�E���}�b�v�f�[�^��zkn_area_monsno_???.dat���������т����Ă���Ȃ�
  // u8 townmap_data_idx_to_zkn_area_idx[TOWNMAP_DATA_MAX] = { 0, 1, 2, 3, ... , TOWNMAP_DATA_MAX-1 };
  // �ƂȂ�B
  // �����炭�������т��낤���A�O�̂��߂��̂悤�Ȕz����Ԃɂ͂��ނ��Ƃɂ����B

  GF_ASSERT_MSG( TOWNMAP_DATA_MAX == ZKN_AREA_ZONE_GROUP_MAX, "ZUKAN_DETAIL_MAP : �^�E���}�b�v�Ɛ}�Ӄf�[�^�ŋ��_�̐����قȂ�܂��B\n" );

  for( i=0; i<TOWNMAP_DATA_MAX; i++ )
  {
    BOOL exist   = FALSE;
    u16  zone_id = TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_ZONE_ID );
    for( j=0; j<ZKN_AREA_ZONE_GROUP_MAX; j++ )
    {
      if( zone_id == zkn_area_zone_group[j] )
      {
        work->townmap_data_idx_to_zkn_area_idx[i] = j;
        exist = TRUE;
        break;
      }
    }
    GF_ASSERT_MSG( exist, "ZUKAN_DETAIL_MAP : �^�E���}�b�v�ɂ��鋒�_���}�Ӄf�[�^�ɂ���܂���B\n" );
  }
}

//-------------------------------------
/// �A�t�B���g��BG
//=====================================
static void Zukan_Detail_Map_AffineExBGInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �ǂݍ���œ]��
  ARCHANDLE* handle;
  handle = GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, param->heap_id );

  // 2�ʂŎg�p�g���p���b�g�X���b�g���Ⴄ�̂�2��]�����Ă���
  GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_townmap_gra_tmap_bg_d_NCLR,
      PALTYPE_MAIN_BG_EX, 0x6000, 2*16*16*16, param->heap_id );  // BG3�p�ɃX���b�g3(0x6000)��  // 2�o�C�g*16�F*16���C��*16�y�[�W�̃p���b�g
  GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_townmap_gra_tmap_bg_d_NCLR,
      PALTYPE_MAIN_BG_EX, 0x4000, 2*16*16*16, param->heap_id );  // BG2�p�ɃX���b�g2(0x4000)��

  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_townmap_gra_tmap_bg_d_NCGR,
      BG_FRAME_M_ROOT, 0, 64*32*8, FALSE, param->heap_id );  // 32�L����*8�L����=64�o�C�g*32*8(���ꂪ���E�̍ő�l)
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_townmap_gra_tmap_map_d_NCGR,
      BG_FRAME_M_MAP, 0, 64*32*24, FALSE, param->heap_id );  // 32�L����*24�L����=64�o�C�g*32*24(���ꂪ���E�̍ő�l)

  GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_townmap_gra_tmap_root_d_NSCR,
      BG_FRAME_M_ROOT, 0, 0x800, FALSE, param->heap_id );  // 32�L����*32�L����=2�o�C�g*32*32=2K
  GFL_ARCHDL_UTIL_TransVramScreen( handle, NARC_townmap_gra_tmap_map_d_NSCR,
      BG_FRAME_M_MAP, 0, 0x800, FALSE, param->heap_id );  // 32�L����*32�L����=2�o�C�g*32*32=2K

  GFL_ARC_CloseDataHandle( handle );
}
static void Zukan_Detail_Map_AffineExBGExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // ���ɉ������Ȃ��Ă悢
}

//-------------------------------------
/// OBJ
//-------------------------------------
static void Zukan_Detail_Map_ObjInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �����_���[�V�X�e���A�Z���A�N�^�[���j�b�g�쐬
  {
    const GFL_REND_SURFACE_INIT surface_init    = { 0, 0, 256, 192, CLSYS_DRAW_MAIN, CLSYS_REND_CULLING_TYPE_NORMAL };
    const u16                   clunit_work_num = 16; 
    
    // �����_���[�V�X�e��
    work->usual_rend = GFL_CLACT_USERREND_Create( &surface_init, 1, param->heap_id );

    // �Z���A�N�^�[���j�b�g
  	work->usual_clunit	= GFL_CLACT_UNIT_Create( clunit_work_num, 0, param->heap_id );
  	GFL_CLACT_UNIT_SetUserRend( work->usual_clunit, work->usual_rend );
    
    // CLSYS_DEFREND_MAIN�Ɠ���0�Ԃ��������Ă��Ȃ��̂�
    // GFL_CLACT_WK_SetPos( work->obj_clwk[i], &pos, CLSYS_DEFREND_MAIN );
    // �́ACLSYS_DEFREND_MAIN�̂܂ܕύX���Ȃ��čς�ł���B
  }

  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle;
    
    handle = GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, param->heap_id );
    work->obj_res[OBJ_RES_TM_NCL] = GFL_CLGRP_PLTT_RegisterEx( 
                                     handle,
                                     NARC_townmap_gra_tmap_obj_NCLR,
                                     CLSYS_DRAW_MAIN,
                                     OBJ_PAL_POS_M_TM*0x20,
                                     0,
                                     OBJ_PAL_NUM_M_TM,
                                     param->heap_id );	
    work->obj_res[OBJ_RES_TM_NCG] = GFL_CLGRP_CGR_Register(
                                     handle,
                                     NARC_townmap_gra_tmap_obj_d_NCGR,
                                     FALSE,
                                     CLSYS_DRAW_MAIN,
                                     param->heap_id );
    work->obj_res[OBJ_RES_TM_NCE] = GFL_CLGRP_CELLANIM_Register(
                                     handle,
                                     NARC_townmap_gra_tmap_obj_d_NCER,
                                     NARC_townmap_gra_tmap_obj_d_NANR,
                                     param->heap_id );
    GFL_ARC_CloseDataHandle( handle );

    handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, param->heap_id );
    work->obj_res[OBJ_RES_ZUKAN_NCL] = GFL_CLGRP_PLTT_RegisterEx( 
                                     handle,
                                     NARC_zukan_gra_info_mapwin_obj_NCLR,
                                     CLSYS_DRAW_MAIN,
                                     OBJ_PAL_POS_M_ZUKAN*0x20,
                                     0, 
                                     OBJ_PAL_NUM_M_ZUKAN,
                                     param->heap_id );	
    work->obj_res[OBJ_RES_ZUKAN_NCG] = GFL_CLGRP_CGR_Register(
                                     handle,
                                     NARC_zukan_gra_info_mapwin_obj_NCGR,
                                     FALSE,
                                     CLSYS_DRAW_MAIN,
                                     param->heap_id );
    work->obj_res[OBJ_RES_ZUKAN_NCE] = GFL_CLGRP_CELLANIM_Register(
                                     handle,
                                     NARC_zukan_gra_info_mapwin_obj_NCER,
                                     NARC_zukan_gra_info_mapwin_obj_NANR,
                                     param->heap_id );
    GFL_ARC_CloseDataHandle( handle );

    handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, param->heap_id );
    work->obj_res[OBJ_RES_EXIST_NCL] = GFL_CLGRP_PLTT_RegisterEx( 
                                     handle,
                                     NARC_zukan_gra_info_map_obj_NCLR,
                                     CLSYS_DRAW_MAIN,
                                     OBJ_PAL_POS_M_EXIST*0x20,
                                     0, 
                                     OBJ_PAL_NUM_M_EXIST,
                                     param->heap_id );	
    work->obj_res[OBJ_RES_EXIST_NCG] = GFL_CLGRP_CGR_Register(
                                     handle,
                                     NARC_zukan_gra_info_map_obj_NCGR,
                                     FALSE,
                                     CLSYS_DRAW_MAIN,
                                     param->heap_id );
    work->obj_res[OBJ_RES_EXIST_NCE] = GFL_CLGRP_CELLANIM_Register(
                                     handle,
                                     NARC_zukan_gra_info_map_obj_NCER,
                                     NARC_zukan_gra_info_map_obj_NANR,
                                     param->heap_id );
    GFL_ARC_CloseDataHandle( handle );
  }

  // CLWK�쐬
  {
    u8 anmseq[OBJ_MAX] =
    {
      // OBJ_ZUKAN_
      1,
      SEASON_L_ARROW_ANMSEQ_NORMAL,
      SEASON_R_ARROW_ANMSEQ_NORMAL,
      0,

      // OBJ_TM_
      4,
      RING_CUR_ANMSEQ,
      6,
    };
    u8 bgpri[OBJ_MAX] =
    {
      // OBJ_ZUKAN_
      BG_FRAME_PRI_M_ROOT,
      BG_FRAME_PRI_M_ROOT,
      BG_FRAME_PRI_M_ROOT,
      BG_FRAME_PRI_M_ROOT,

      // OBJ_TM_
      BG_FRAME_PRI_M_TEXT,
      BG_FRAME_PRI_M_TEXT,
      BG_FRAME_PRI_M_TEXT,
    };
    u8 softpri[OBJ_MAX] =
    {
      // OBJ_ZUKAN_
      2,
      1,
      0,
      3,

      // OBJ_TM_
      0,
      1,
      2,
    };

    u8 i;
    GFL_CLWK_DATA cldata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
   
    for( i=OBJ_TM_START; i<OBJ_TM_END; i++ )
    {	
      work->obj_clwk[i] = GFL_CLACT_WK_Create(
                             work->usual_clunit,
                             work->obj_res[OBJ_RES_TM_NCG], work->obj_res[OBJ_RES_TM_NCL], work->obj_res[OBJ_RES_TM_NCE],
                             &cldata, CLSYS_DEFREND_MAIN, param->heap_id );
    }
    
    for( i=OBJ_ZUKAN_START; i<OBJ_ZUKAN_END; i++ )
    {	
      work->obj_clwk[i] = GFL_CLACT_WK_Create(
                             work->usual_clunit,
                             work->obj_res[OBJ_RES_ZUKAN_NCG], work->obj_res[OBJ_RES_ZUKAN_NCL], work->obj_res[OBJ_RES_ZUKAN_NCE],
                             &cldata, CLSYS_DEFREND_MAIN, param->heap_id );
    }

    for( i=0; i<OBJ_MAX; i++ )
    {
      GFL_CLACT_WK_SetAnmSeq( work->obj_clwk[i], anmseq[i] );
      GFL_CLACT_WK_SetAutoAnmFlag( work->obj_clwk[i], TRUE );
      GFL_CLACT_WK_SetBgPri( work->obj_clwk[i], bgpri[i] );
      GFL_CLACT_WK_SetSoftPri( work->obj_clwk[i], softpri[i] );
      GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[i], FALSE );
      GFL_CLACT_WK_SetObjMode( work->obj_clwk[i], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    }

    // �ʐݒ�
    {
      GFL_CLACTPOS pos;
      pos.x = 16*8;
      pos.y = 12*8;
      for( i=OBJ_ZUKAN_START; i<OBJ_ZUKAN_END; i++ )
        GFL_CLACT_WK_SetPos( work->obj_clwk[i], &pos, CLSYS_DEFREND_MAIN );
    }
  }
}
static void Zukan_Detail_Map_ObjExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // CLWK�j��
  {
    u8 i;
    for( i=0; i<OBJ_MAX; i++ )
    {
      GFL_CLACT_WK_Remove( work->obj_clwk[i] );
    }
  }

  // ���\�[�X�j��
  {
    GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_RES_EXIST_NCE] );
    GFL_CLGRP_CGR_Release( work->obj_res[OBJ_RES_EXIST_NCG] );
    GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_RES_EXIST_NCL] );
    GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_RES_ZUKAN_NCE] );
    GFL_CLGRP_CGR_Release( work->obj_res[OBJ_RES_ZUKAN_NCG] );
    GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_RES_ZUKAN_NCL] );	
    GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_RES_TM_NCE] );
    GFL_CLGRP_CGR_Release( work->obj_res[OBJ_RES_TM_NCG] );
    GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_RES_TM_NCL] );
  }

  // �����_���[�V�X�e���A�Z���A�N�^�[���j�b�g�j��
  {
    // �Z���A�N�^�[���j�b�g
	  GFL_CLACT_UNIT_Delete( work->usual_clunit );
  	
    // �����_���[�V�X�e��
    GFL_CLACT_USERREND_Delete( work->usual_rend );
  }
}

static void Zukan_Detail_Map_ObjExistInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // CLWK�쐬
  {
    u8 softpri = 4;  // obj_clwk��softpri���v���C�I���e�B��Ⴍ���Ă����AHIDE_SPOT��obj_clwk��softpri���v���C�I���e�B���������Ă���

    u8 i;
    GFL_CLWK_DATA cldata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
 
    for( i=0; i<TOWNMAP_DATA_MAX; i++ )
    {
      cldata.pos_x = (s16)TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_ZKN_POS_X );
      cldata.pos_y = (s16)TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_ZKN_POS_Y );

      work->obj_exist_clwk[i] = GFL_CLACT_WK_Create(
                             work->clunit,
                             work->obj_res[OBJ_RES_EXIST_NCG], work->obj_res[OBJ_RES_EXIST_NCL], work->obj_res[OBJ_RES_EXIST_NCE],
                             &cldata, CLSYS_DEFREND_MAIN, param->heap_id );

      GFL_CLACT_WK_SetAnmSeq( work->obj_exist_clwk[i], TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_ZKN_ANM ) );
      GFL_CLACT_WK_SetAutoAnmFlag( work->obj_exist_clwk[i], TRUE );
      GFL_CLACT_WK_SetBgPri( work->obj_exist_clwk[i], BG_FRAME_PRI_M_ROOT );
      GFL_CLACT_WK_SetSoftPri( work->obj_exist_clwk[i], softpri );
      GFL_CLACT_WK_SetDrawEnable( work->obj_exist_clwk[i], FALSE );
      GFL_CLACT_WK_SetObjMode( work->obj_exist_clwk[i], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    }
  }
}
static void Zukan_Detail_Map_ObjExistExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // CLWK�j��
  {
    u8 i;
    for( i=0; i<TOWNMAP_DATA_MAX; i++ )
    {
      GFL_CLACT_WK_Remove( work->obj_exist_clwk[i] );
    }
  }
}

//-------------------------------------
/// �B���X�|�b�g
//-------------------------------------
static void Zukan_Detail_Map_HideSpotInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // Zukan_Detail_Map_ObjInit���ς�ł���ĂԂ���

  GAMEDATA*  gamedata  = ZKNDTL_COMMON_GetGamedata(cmn);
  u8 i;
  
  for( i=0; i<TOWNMAP_DATA_MAX; i++ )
  {
    u16 hide_flag	= TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_HIDE_FLAG );
    if( hide_flag != TOWNMAP_DATA_ERROR )
    {
      BOOL is_hide = !TOWNMAP_UTIL_CheckFlag( gamedata, hide_flag );
#ifdef DEBUG_SET_ANIME_AND_POS
      is_hide = FALSE;
#endif
      if( is_hide )
      {
        work->hide_spot[i].state    = HIDE_STATE_HIDE_TRUE;
        work->hide_spot[i].obj_clwk = NULL;
      }
      else
      {
        u16 placetype = TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_PLACE_TYPE );

        GFL_CLWK_DATA cldata;
        cldata.pos_x    = (s16)TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_POS_X );
        cldata.pos_y    = (s16)TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_POS_Y );
        cldata.anmseq   = (placetype==TOWNMAP_PLACETYPE_HIDE)?(14):(16);  // �U�̏ꍇ��TOWNMAP_PLACETYPE_HIDE_TOWN_S�ƂȂ��Ă���͂�
        cldata.softpri  = 5;
        cldata.bgpri    = BG_FRAME_PRI_M_ROOT;

        work->hide_spot[i].state    = HIDE_STATE_HIDE_FALSE;
        
        work->hide_spot[i].obj_clwk = GFL_CLACT_WK_Create(
            work->usual_clunit,
            work->obj_res[OBJ_RES_TM_NCG], work->obj_res[OBJ_RES_TM_NCL], work->obj_res[OBJ_RES_TM_NCE],
            &cldata, CLSYS_DEFREND_MAIN, param->heap_id );
        GFL_CLACT_WK_SetAutoAnmFlag( work->hide_spot[i].obj_clwk, TRUE );
        GFL_CLACT_WK_SetDrawEnable( work->hide_spot[i].obj_clwk, TRUE );
        GFL_CLACT_WK_SetObjMode( work->hide_spot[i].obj_clwk, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
      }
    }
    else
    {
      work->hide_spot[i].state    = HIDE_STATE_NO;
      work->hide_spot[i].obj_clwk = NULL;
    }
  }
}
static void Zukan_Detail_Map_HideSpotExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;
  for( i=0; i<TOWNMAP_DATA_MAX; i++ )
  {
    if( work->hide_spot[i].state == HIDE_STATE_HIDE_FALSE )
    {
      GFL_CLACT_WK_Remove( work->hide_spot[i].obj_clwk );
    }
  }
}

//-------------------------------------
/// ���b�Z�[�W�ƃe�L�X�g
//-------------------------------------
static void Zukan_Detail_Map_MsgTextInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 dummy_bmpwin_setup[TEXT_DUMMY_MAX][7] =
  {
    { BG_FRAME_S_TEXT, 0, 0, 1, 1, BG_PAL_POS_S_TEXT, GFL_BMP_CHRAREA_GET_F },
    { BG_FRAME_M_TEXT, 0, 0, 1, 1, BG_PAL_POS_M_TEXT, GFL_BMP_CHRAREA_GET_F },
  };
  u8 bmpwin_setup[TEXT_MAX][7] =
  {
    { BG_FRAME_S_TEXT, 12, 21, 16,  3, BG_PAL_POS_S_TEXT, GFL_BMP_CHRAREA_GET_F },
    { BG_FRAME_S_TEXT,  9, 25, 16,  2, BG_PAL_POS_S_TEXT, GFL_BMP_CHRAREA_GET_F },
    { BG_FRAME_M_TEXT,  2,  0, 15,  2, BG_PAL_POS_M_TEXT, GFL_BMP_CHRAREA_GET_F },
    { BG_FRAME_M_TEXT,  8, 10, 16,  2, BG_PAL_POS_M_TEXT, GFL_BMP_CHRAREA_GET_F },
  };

  u8 i;

  // �p���b�g
  GFL_ARC_UTIL_TransVramPaletteEx( ARCID_FONT, NARC_font_default_nclr,
       PALTYPE_SUB_BG,
       0,
       BG_PAL_POS_S_TEXT * 0x20,
       BG_PAL_NUM_S_TEXT * 0x20,
       param->heap_id );
   GFL_ARC_UTIL_TransVramPaletteEx( ARCID_FONT, NARC_font_default_nclr,
       PALTYPE_MAIN_BG,
       0,
       BG_PAL_POS_M_TEXT * 0x20,
       BG_PAL_NUM_M_TEXT * 0x20,
       param->heap_id );

  // �e�L�X�g�\���ɕK�v�ȃ_�~�[�̃r�b�g�}�b�v�E�B���h�E
  for( i=0; i<TEXT_DUMMY_MAX; i++ )
  {
    work->text_dummy_bmpwin[i] = GFL_BMPWIN_Create(
                                     dummy_bmpwin_setup[i][0],
                                     dummy_bmpwin_setup[i][1], dummy_bmpwin_setup[i][2], dummy_bmpwin_setup[i][3], dummy_bmpwin_setup[i][4],
                                     dummy_bmpwin_setup[i][5], dummy_bmpwin_setup[i][6] );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_dummy_bmpwin[i]), 0 );
    GFL_BMPWIN_TransVramCharacter( work->text_dummy_bmpwin[i] );
  }

  // �r�b�g�}�b�v�E�B���h�E
  for( i=0; i<TEXT_MAX; i++ )
  {
    work->text_bmpwin[i] = GFL_BMPWIN_Create(
                                     bmpwin_setup[i][0],
                                     bmpwin_setup[i][1], bmpwin_setup[i][2], bmpwin_setup[i][3], bmpwin_setup[i][4],
                                     bmpwin_setup[i][5], bmpwin_setup[i][6] );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[i]), 0 );
    GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[i] );
  }

  // ���b�Z�[�W
  work->msgdata[MSG_ZUKAN] = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
                           ARCID_MESSAGE, NARC_message_zkn_dat,
                           param->heap_id );
  work->msgdata[MSG_PLACE] = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
                           ARCID_MESSAGE, NARC_message_place_name_dat,
                           param->heap_id );
}
static void Zukan_Detail_Map_MsgTextExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;
  for( i=0; i<MSG_MAX; i++ )
    GFL_MSG_Delete( work->msgdata[i] );
  for( i=0; i<TEXT_MAX; i++ )
    GFL_BMPWIN_Delete( work->text_bmpwin[i] );
  for( i=0; i<TEXT_DUMMY_MAX; i++ )
    GFL_BMPWIN_Delete( work->text_dummy_bmpwin[i] );
}

//-------------------------------------
/// ����
//-------------------------------------
static void Zukan_Detail_Map_PlaceInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  ARCHANDLE* handle;

  // BG PLACE
  handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, param->heap_id );
  // �p���b�g
  GFL_ARCHDL_UTIL_TransVramPaletteEx(
                               handle,
                               NARC_zukan_gra_info_info_bgu_NCLR,
                               PALTYPE_SUB_BG,
                               0 * 0x20,
                               BG_PAL_POS_S_PLACE * 0x20,
                               BG_PAL_NUM_S_PLACE * 0x20,
                               param->heap_id );
  // �L����
  work->place_tinfo = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
                            handle,
                            NARC_zukan_gra_info_info_bgu_NCGR,
                            BG_FRAME_S_PLACE,
                            0,
                            FALSE,
                            param->heap_id );
  GF_ASSERT_MSG( work->place_tinfo != GFL_ARCUTIL_TRANSINFO_FAIL, "ZUKAN_DETAIL_MAP : BG�L�����̈悪����܂���ł����B\n" );
  // �X�N���[��
  GFL_ARCHDL_UTIL_TransVramScreenCharOfs(
        handle,
        NARC_zukan_gra_info_mapwin_bgu_NSCR,
        BG_FRAME_S_PLACE,
        0,
        GFL_ARCUTIL_TRANSINFO_GetPos( work->place_tinfo ),
        0,
        FALSE,
        param->heap_id );

  GFL_ARC_CloseDataHandle( handle );
}
static void Zukan_Detail_Map_PlaceExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  GFL_BG_FreeCharacterArea( BG_FRAME_S_PLACE,
                            GFL_ARCUTIL_TRANSINFO_GetPos( work->place_tinfo ),
                            GFL_ARCUTIL_TRANSINFO_GetSize( work->place_tinfo ) );
}
static void Zukan_Detail_Map_PlaceMain( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  Zukan_Detail_Map_PlaceMainScroll( param, work, cmn );
  Zukan_Detail_Map_PlaceMainIcon( param, work, cmn );
}
static void Zukan_Detail_Map_PlaceMainScroll( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // BG�̃X�N���[���A�j���[�V����
  switch( work->scroll_state )
  {
  case SCROLL_STATE_DOWN:
    {
    }
    break;
  case SCROLL_STATE_DOWN_TO_UP:
    {
      int y = GFL_BG_GetScrollY( BG_FRAME_S_PLACE );
      if( y >= 8*8 )
      {
        GFL_BG_SetScrollReq( BG_FRAME_S_PLACE, GFL_BG_SCROLL_Y_SET, 8*8 );
        work->scroll_state = SCROLL_STATE_UP; 
      }
      else
      {
        GFL_BG_SetScrollReq( BG_FRAME_S_PLACE, GFL_BG_SCROLL_Y_INC, SCROLL_VEL );
        GFL_BG_SetScrollReq( BG_FRAME_S_TEXT, GFL_BG_SCROLL_Y_INC, SCROLL_VEL );
      }
    }
    break;
  case SCROLL_STATE_UP:
    {
    }
    break;
  case SCROLL_STATE_UP_TO_DOWN:
    {
      int y = GFL_BG_GetScrollY( BG_FRAME_S_PLACE );
      if( y <= 0 )
      {
        GFL_BG_SetScrollReq( BG_FRAME_S_PLACE, GFL_BG_SCROLL_Y_SET, 0 );
        work->scroll_state = SCROLL_STATE_DOWN; 
      }
      else
      {
        GFL_BG_SetScrollReq( BG_FRAME_S_PLACE, GFL_BG_SCROLL_Y_DEC, SCROLL_VEL );
        GFL_BG_SetScrollReq( BG_FRAME_S_TEXT, GFL_BG_SCROLL_Y_DEC, SCROLL_VEL );
      }
    }
    break;
  }

  // �|�P�A�C�R���̃X�N���[���A�j���[�V����
  if( work->pokeicon_clwk )
  {
    GFL_CLACTPOS pos;
    switch( work->pokeicon_scroll_state )
    {
    case SCROLL_STATE_DOWN:
      {
      }
      break;
    case SCROLL_STATE_DOWN_TO_UP:
      {
        GFL_CLACT_WK_GetPos( work->pokeicon_clwk, &pos, CLSYS_DEFREND_SUB );
        if( pos.y <= pokeicon_pos[3] )
        {
          pos.y = pokeicon_pos[3];
          work->pokeicon_scroll_state = SCROLL_STATE_UP; 
        }
        else
        {
          pos.y -= SCROLL_VEL;
        }
        GFL_CLACT_WK_SetPos( work->pokeicon_clwk, &pos, CLSYS_DEFREND_SUB );
      }
      break;
    case SCROLL_STATE_UP:
      {
      }
      break;
    case SCROLL_STATE_UP_TO_DOWN:
      {
        GFL_CLACT_WK_GetPos( work->pokeicon_clwk, &pos, CLSYS_DEFREND_SUB );
        if( pos.y >= pokeicon_pos[1] )
        {
          pos.y = pokeicon_pos[1];
          work->pokeicon_scroll_state = SCROLL_STATE_DOWN; 
        }
        else
        {
          pos.y += SCROLL_VEL;
        }
        GFL_CLACT_WK_SetPos( work->pokeicon_clwk, &pos, CLSYS_DEFREND_SUB );
      }
      break;
    }
  }
}
static void Zukan_Detail_Map_PlaceMainIcon( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // ����A����A�ނ�A�C�R���̖��ŃA�j���[�V����
}

//-------------------------------------
/// �����
//=====================================
static void Zukan_Detail_Map_SeasonArrowMain( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( GFL_CLACT_WK_GetDrawEnable( work->obj_clwk[OBJ_SEASON_R] ) )
  {
    if( GFL_CLACT_WK_GetAnmSeq( work->obj_clwk[OBJ_SEASON_R] ) == SEASON_R_ARROW_ANMSEQ_SELECT )
    {
      if( !GFL_CLACT_WK_CheckAnmActive( work->obj_clwk[OBJ_SEASON_R] ) )
      {
        GFL_CLACT_WK_SetAnmSeq( work->obj_clwk[OBJ_SEASON_R], SEASON_R_ARROW_ANMSEQ_NORMAL );
      }
    }
  }

  if( GFL_CLACT_WK_GetDrawEnable( work->obj_clwk[OBJ_SEASON_L] ) )
  {
    if( GFL_CLACT_WK_GetAnmSeq( work->obj_clwk[OBJ_SEASON_L] ) == SEASON_L_ARROW_ANMSEQ_SELECT )
    {
      if( !GFL_CLACT_WK_CheckAnmActive( work->obj_clwk[OBJ_SEASON_L] ) )
      {
        GFL_CLACT_WK_SetAnmSeq( work->obj_clwk[OBJ_SEASON_L], SEASON_L_ARROW_ANMSEQ_NORMAL );
      }
    }
  }
}

//-------------------------------------
/// �|�P�A�C�R��
//=====================================
static GFL_CLWK* PokeiconInit( UI_EASY_CLWK_RES* res, GFL_CLUNIT* clunit, HEAPID heap_id, u32 mons, u32 form_no, u32 sex, BOOL egg, u8 x, u8 y )
{
  GFL_CLWK* clwk;
  
  UI_EASY_CLWK_RES_PARAM prm;
  prm.draw_type    = CLSYS_DRAW_SUB;
  prm.comp_flg     = UI_EASY_CLWK_RES_COMP_NCLR;
  prm.arc_id       = ARCID_POKEICON;
  prm.pltt_id      = POKEICON_GetPalArcIndex();
  prm.ncg_id       = POKEICON_GetCgxArcIndexByMonsNumber( mons, form_no, sex, egg );
  prm.cell_id      = POKEICON_GetCellArcIndex(); 
  prm.anm_id       = POKEICON_GetAnmArcIndex();
  prm.pltt_line    = OBJ_PAL_POS_S_POKEICON;
  prm.pltt_src_ofs = 0;
  prm.pltt_src_num = OBJ_PAL_NUM_S_POKEICON;
 
  UI_EASY_CLWK_LoadResource( res, &prm, clunit, heap_id );
  
  // �A�j���V�[�P���X�Ŏw��( 0=�m��, 1=HP�ő�, 2=HP��, 3=HP��, 4=HP��, 5=��Ԉُ� )
  clwk = UI_EASY_CLWK_CreateCLWK( res, clunit, x, y, 1, heap_id );

  // ��ɃA�C�e���A�C�R����`�悷��̂ŗD��x�������Ă���
  GFL_CLACT_WK_SetSoftPri( clwk, 1 );

  // �I�[�g�A�j�� OFF
  GFL_CLACT_WK_SetAutoAnmFlag( clwk, FALSE );

  {
    u8 pal_num = POKEICON_GetPalNum( mons, form_no, sex, egg );
    GFL_CLACT_WK_SetPlttOffs( clwk, pal_num, CLWK_PLTTOFFS_MODE_OAM_COLOR );
  }

  GFL_CLACT_WK_SetObjMode( clwk, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�

  return clwk;
}
static void PokeiconExit( UI_EASY_CLWK_RES* res, GFL_CLWK* clwk )
{
  // CLWK�j��
  GFL_CLACT_WK_Remove( clwk );
  // ���\�[�X�J��
  UI_EASY_CLWK_UnLoadResource( res );
}

//-------------------------------------
/// �v���C���[�̈ʒu�̃f�[�^�C���f�b�N�X�𓾂�
//=====================================
static u8 Zukan_Detail_Map_GetPlayerPosDataIdx( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  GAMEDATA*       gamedata  = ZKNDTL_COMMON_GetGamedata(cmn);
  PLAYER_WORK*    player_wk = GAMEDATA_GetMyPlayerWork( gamedata );
  u16             zone_id   = PLAYERWORK_getZoneID( player_wk );
  u16             escape_id = GAMEDATA_GetEscapeLocation( gamedata )->zone_id;
 
  u8   player_pos_data_idx = DATA_IDX_NONE;

  u8 i;

  // �]�[���Ɗ��S�Ή�����ꏊ���擾
  if( player_pos_data_idx == DATA_IDX_NONE )
  {
	  for( i=0; i<TOWNMAP_DATA_MAX; i++ )
	  {	
	  	if( zone_id == TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_ZONE_ID ) )
      {
        player_pos_data_idx = i;
        break;
      }
    }
  }
  // ���ݒn���t�B�[���h�ł͂Ȃ��Ȃ�΁A�G�X�P�[�vID����
  if( player_pos_data_idx == DATA_IDX_NONE )
  {
	  for( i=0; i<TOWNMAP_DATA_MAX; i++ )
	  {	
	  	if( escape_id == TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_ZONE_ID ) )
      {
        player_pos_data_idx = i;
        break;
      }
    }
  }

  return player_pos_data_idx;
}

//-------------------------------------
/// �v���C���[�̈ʒu��OBJ��z�u����
//=====================================
static void Zukan_Detail_Map_SetPlayer( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  GFL_CLACTPOS pos;

  u8 player_pos_data_idx = Zukan_Detail_Map_GetPlayerPosDataIdx( param, work, cmn );
  
  if( player_pos_data_idx != DATA_IDX_NONE )
  {
    pos.x = (s16)TOWNMAP_DATA_GetParam( work->townmap_data, player_pos_data_idx, TOWNMAP_DATA_PARAM_POS_X );
    pos.y = (s16)TOWNMAP_DATA_GetParam( work->townmap_data, player_pos_data_idx, TOWNMAP_DATA_PARAM_POS_Y );
    GFL_CLACT_WK_SetPos( work->obj_clwk[OBJ_HERO], &pos, CLSYS_DEFREND_MAIN );
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_HERO], TRUE );
  }
}

//-------------------------------------
/// �J�[�\����z�u����
//=====================================
static void Zukan_Detail_Map_SetCursor( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  GFL_CLACTPOS pos = { CURSOR_DEFAULT_POS_X, CURSOR_DEFAULT_POS_Y };  // �f�t�H���g�ʒu

  // �ǂ��̋��_���I�����Ă��Ȃ��Ƃ��́A�v���C���[�̈ʒu�ɃJ�[�\�������킹�Ă���
  if( work->select_data_idx == DATA_IDX_NONE )
  {
    work->select_data_idx = Zukan_Detail_Map_GetPlayerPosDataIdx( param, work, cmn );
  }

  if( work->select_data_idx != DATA_IDX_NONE )
  {
    pos.x = (s16)TOWNMAP_DATA_GetParam( work->townmap_data, work->select_data_idx, TOWNMAP_DATA_PARAM_POS_X );
    pos.y = (s16)TOWNMAP_DATA_GetParam( work->townmap_data, work->select_data_idx, TOWNMAP_DATA_PARAM_POS_Y );
  }
  
  GFL_CLACT_WK_SetPos( work->obj_clwk[OBJ_CURSOR], &pos, CLSYS_DEFREND_MAIN );
  GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_CURSOR], TRUE );
}

//-------------------------------------
/// �v���C���[�̈ʒu�ɃJ�[�\����z�u����
//=====================================
static void Zukan_Detail_Map_SetCursorOnPlayer( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // ��΂Ƀv���C���[�̈ʒu�ɃJ�[�\�������킹�����̂ŁA�v���C���[�̈ʒu�̋��_��I�����Ă���
  work->select_data_idx = Zukan_Detail_Map_GetPlayerPosDataIdx( param, work, cmn );

  Zukan_Detail_Map_SetCursor( param, work, cmn );
}

//-------------------------------------
/// �w�肵�����W�ƃq�b�g����f�[�^������΂��̃f�[�^�C���f�b�N�X��Ԃ�
//=====================================
static u8 Zukan_Detail_Map_Hit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                u8 x, u8 y )
{
  // �ŏ������̂��̂��c��
  fx32  distance_min;
  u8    distance_min_hit_data_idx = DATA_IDX_NONE;
  u8  i;

	GFL_COLLISION3D_CIRCLE		circle;
	VecFx32	v;

  v.x	= FX32_CONST( x );
  v.y	= FX32_CONST( y );
  v.z	= 0;
  GFL_COLLISION3D_CIRCLE_SetData( &circle, &v, 0 );

  for( i=0; i<TOWNMAP_DATA_MAX; i++ )
  {
	  GFL_COLLISION3D_CYLINDER	cylinder;
	  VecFx32	v1, v2;
	  fx32 w;
    GFL_COLLISION3D_CYLXCIR_RESULT  result;

    if( work->hide_spot[i].state == HIDE_STATE_HIDE_TRUE )  // �B���}�b�v�Ȃ��΂ɂ�����Ȃ�
    {
      continue;
    }
  	v1.x	= FX32_CONST( TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_HIT_START_X ) );
  	v1.y	= FX32_CONST( TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_HIT_START_Y ) );
  	v1.z	= 0;
  	v2.x	= FX32_CONST( TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_HIT_END_X ) );
  	v2.y	= FX32_CONST( TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_HIT_END_Y ) );
  	v2.z	= 0;
	  w			= FX32_CONST( TOWNMAP_DATA_GetParam( work->townmap_data, i, TOWNMAP_DATA_PARAM_HIT_WIDTH ) );
	  GFL_COLLISION3D_CYLINDER_SetData( &cylinder, &v1, &v2, w );

    // �ŏ�������
	  if( GFL_COLLISION3D_CYLXCIR_Check( &cylinder, &circle, &result ) )
    {
      if( distance_min_hit_data_idx == DATA_IDX_NONE )
      {
        distance_min = result.dist;
        distance_min_hit_data_idx = i;
      }
      else
      {
        if( distance_min > result.dist )
        {
          distance_min = result.dist;
          distance_min_hit_data_idx = i;
        }
      }
    }
  }

  return distance_min_hit_data_idx;
}

//-------------------------------------
/// �w�肵�����W�͋z�����ݔ͈͓���
//=====================================
static BOOL Zukan_Detail_Map_IsPullHit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                        u8 data_idx, u8 x, u8 y, u32* distance_sq )
{
  u32 data_x, data_y;
  u32 d2;

  if( work->hide_spot[data_idx].state == HIDE_STATE_HIDE_TRUE )  // �B���}�b�v�Ȃ��΂ɂ�����Ȃ�
  {
    return FALSE;
  }

  data_x = TOWNMAP_DATA_GetParam( work->townmap_data, data_idx, TOWNMAP_DATA_PARAM_CURSOR_X );
  data_y = TOWNMAP_DATA_GetParam( work->townmap_data, data_idx, TOWNMAP_DATA_PARAM_CURSOR_Y );

  d2 = ( data_x - x ) * ( data_x - x ) + ( data_y - y ) * ( data_y - y );

  if( d2 < PLACE_PULL_R_SQ )
  {
    if( distance_sq ) *distance_sq = d2;
    return TRUE;
  }

  return FALSE;
}

//-------------------------------------
/// �w�肵�����W���z�����ݔ͈͓��Ɏ��f�[�^������΂��̃f�[�^�C���f�b�N�X��Ԃ�
//=====================================
static u8 Zukan_Detail_Map_PullHit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                u8 x, u8 y )
{
	u32 now_distance_sq;
	u32 best_distance_sq;
  u8  best_idx           = DATA_IDX_NONE;
  u8  i;

	// �����Փ˂��Ă����Ȃ�Έ�ԋ߂��̂��̂�Ԃ�
  for( i=0; i<TOWNMAP_DATA_MAX; i++ )
  {
		if( Zukan_Detail_Map_IsPullHit( param, work, cmn, i, x, y, &now_distance_sq ) )
    {
      if( best_idx == DATA_IDX_NONE )
      {
        best_idx = i;
        best_distance_sq = now_distance_sq;
      }
      else
      {
        if( best_distance_sq > now_distance_sq )
        {
          best_idx = i;
          best_distance_sq = now_distance_sq;
        }
      }
    }
  }
  return best_idx;
}

//-------------------------------------
/// ����
//=====================================
static void Zukan_Detail_Map_Input( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  switch(work->state)
  {
  case STATE_TOP:
    {
      BOOL is_input = FALSE;
      u32 x, y;
      BOOL change_state = FALSE;

      // �L�[����
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
      {
        work->ktst = GFL_APP_KTST_KEY;
        change_state = TRUE;
        is_input = TRUE;
      }
      else if( work->appear_rule != APPEAR_RULE_YEAR )
      {
        if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_R )
        {
          work->ktst = GFL_APP_KTST_KEY;

          // �ύX��̋G�߂ɂ���
          work->season_id++;
          if( work->season_id >= PMSEASON_TOTAL ) work->season_id = PMSEASON_SPRING;
          
          Zukan_Detail_Map_ChangeSeason( param, work, cmn, TRUE );
          PMSND_PlaySE( SEQ_SE_SELECT3 );

          is_input = TRUE;
        }
        else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_L )
        {
          work->ktst = GFL_APP_KTST_KEY;
 
          // �ύX��̋G�߂ɂ���
          if( work->season_id == PMSEASON_SPRING ) work->season_id = PMSEASON_WINTER;
          else                                     work->season_id--;
          
          Zukan_Detail_Map_ChangeSeason( param, work, cmn, FALSE );
          PMSND_PlaySE( SEQ_SE_SELECT3 );
          
          is_input = TRUE;
        }
      }

      // �^�b�`����
      if( !is_input ) 
      {
		    if( GFL_UI_TP_GetPointTrg( &x, &y ) )
        {
          if( work->appear_rule == APPEAR_RULE_YEAR )
          {
            if( 0<=y&&y<168)
            {
              work->ktst = GFL_APP_KTST_TOUCH;
              change_state = TRUE;
            }
          }
          else
          {
            if(    SEASON_R_ARROW_POS_X<=x&&x<SEASON_R_ARROW_POS_X+SEASON_R_ARROW_SIZE_X
                && SEASON_R_ARROW_POS_Y<=y&&y<SEASON_R_ARROW_POS_Y+SEASON_R_ARROW_SIZE_Y )
            {
              work->ktst = GFL_APP_KTST_TOUCH;
              
              // �ύX��̋G�߂ɂ���
              work->season_id++;
              if( work->season_id >= PMSEASON_TOTAL ) work->season_id = PMSEASON_SPRING;

              Zukan_Detail_Map_ChangeSeason( param, work, cmn, TRUE );
              PMSND_PlaySE( SEQ_SE_SELECT3 );
            }
            else if(    SEASON_L_ARROW_POS_X<=x&&x<SEASON_L_ARROW_POS_X+SEASON_L_ARROW_SIZE_X
                     && SEASON_L_ARROW_POS_Y<=y&&y<SEASON_L_ARROW_POS_Y+SEASON_L_ARROW_SIZE_Y )
            {
              work->ktst = GFL_APP_KTST_TOUCH;
              
              // �ύX��̋G�߂ɂ���
              if( work->season_id == PMSEASON_SPRING ) work->season_id = PMSEASON_WINTER;
              else                                     work->season_id--;

              Zukan_Detail_Map_ChangeSeason( param, work, cmn, FALSE );
              PMSND_PlaySE( SEQ_SE_SELECT3 );
            }
            else if(    SEASON_PANEL_POS_X<=x&&x<SEASON_PANEL_POS_X+SEASON_PANEL_SIZE_X
                     && SEASON_PANEL_POS_Y<=y&&y<SEASON_PANEL_POS_Y+SEASON_PANEL_SIZE_Y )
            {
              // �G�ߖ���\�����Ă���ꏊ���^�b�`�����Ƃ��́A�������Ȃ�
            }
            else if(0<=y&&y<168)
            {
              work->ktst = GFL_APP_KTST_TOUCH;
              change_state = TRUE;
            }
          }
        }
      }
      if( change_state )
      {
        if( !( work->appear_rule == APPEAR_RULE_YEAR && work->habitat == HABITAT_UNKNOWN ) )  // ��N�������n�s���̏ꍇ�̓g�b�v����~����Ȃ�
        {
          Zukan_Detail_Map_ChangeState( param, work, cmn, STATE_SELECT );
          PMSND_PlaySE( SEQ_SE_DECIDE1 );
        }
      }
    }
    break;
  case STATE_SELECT:
    {
      BOOL is_input = FALSE;
      u32 x, y;
      BOOL select_enable = TRUE;

      // �L�[����
      if( work->appear_rule != APPEAR_RULE_YEAR )
      {
        if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_R )
        {
          work->ktst = GFL_APP_KTST_KEY;

          // �ύX��̋G�߂ɂ���
          work->season_id++;
          if( work->season_id >= PMSEASON_TOTAL ) work->season_id = PMSEASON_SPRING;
          
          Zukan_Detail_Map_ChangeSeason( param, work, cmn, TRUE );
          PMSND_PlaySE( SEQ_SE_SELECT3 );

          is_input = TRUE;
        }
        else if( GFL_UI_KEY_GetRepeat() & PAD_BUTTON_L )
        {
          work->ktst = GFL_APP_KTST_KEY;
 
          // �ύX��̋G�߂ɂ���
          if( work->season_id == PMSEASON_SPRING ) work->season_id = PMSEASON_WINTER;
          else                                     work->season_id--;
          
          Zukan_Detail_Map_ChangeSeason( param, work, cmn, FALSE );
          PMSND_PlaySE( SEQ_SE_SELECT3 );
          
          is_input = TRUE;
        }
      }
      if( is_input )  // �L�[���͂̌���
      {
        select_enable = FALSE;
      }
      // �^�b�`����
      else if( GFL_UI_TP_GetPointTrg( &x, &y ) )
      {
        if( work->appear_rule == APPEAR_RULE_SEASON )
        {
          if(    SEASON_R_ARROW_POS_X<=x&&x<SEASON_R_ARROW_POS_X+SEASON_R_ARROW_SIZE_X
              && SEASON_R_ARROW_POS_Y<=y&&y<SEASON_R_ARROW_POS_Y+SEASON_R_ARROW_SIZE_Y )
          {
            work->ktst = GFL_APP_KTST_TOUCH;

            // �ύX��̋G�߂ɂ���
            work->season_id++;
            if( work->season_id >= PMSEASON_TOTAL ) work->season_id = PMSEASON_SPRING;

            Zukan_Detail_Map_ChangeSeason( param, work, cmn, TRUE );
            PMSND_PlaySE( SEQ_SE_SELECT3 );
            select_enable = FALSE;
          }
          else if(    SEASON_L_ARROW_POS_X<=x&&x<SEASON_L_ARROW_POS_X+SEASON_L_ARROW_SIZE_X
                   && SEASON_L_ARROW_POS_Y<=y&&y<SEASON_L_ARROW_POS_Y+SEASON_L_ARROW_SIZE_Y )
          {
            work->ktst = GFL_APP_KTST_TOUCH;

            // �ύX��̋G�߂ɂ���
            if( work->season_id == PMSEASON_SPRING ) work->season_id = PMSEASON_WINTER;
            else                                     work->season_id--;

            Zukan_Detail_Map_ChangeSeason( param, work, cmn, FALSE );
            PMSND_PlaySE( SEQ_SE_SELECT3 );
            select_enable = FALSE;
          }
          else if(    SEASON_PANEL_POS_X<=x&&x<SEASON_PANEL_POS_X+SEASON_PANEL_SIZE_X
                   && SEASON_PANEL_POS_Y<=y&&y<SEASON_PANEL_POS_Y+SEASON_PANEL_SIZE_Y )
          {
            // �G�ߖ���\�����Ă���ꏊ���^�b�`�����Ƃ��́A�}�b�v��̋��_�̑I�𑀍�͂��Ă��Ȃ����̂Ƃ���
            select_enable = FALSE;
          }
        }
      }
      if( select_enable )
      {
        // �}�b�v��̋��_�̑I�𑀍�
        Zukan_Detail_Map_InputSelect( param, work, cmn );
      }
    }
    break;
  }
}

//-------------------------------------
/// �}�b�v��̋��_�̑I�𑀍�
//=====================================
static void Zukan_Detail_Map_InputSelect( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u32 x, y;

  // �����n���s���̂Ƃ��́A�}�b�v��̋��_��I�ׂȂ����Ƃɂ��Ă��� 
  if( work->habitat == HABITAT_UNKNOWN ) return;

  // �L�[ or �^�b�`�̕ύX
  if( work->ktst == GFL_APP_KTST_KEY )
  {
    if( GFL_UI_TP_GetPointTrg( &x, &y ) )
    {
      work->ktst = GFL_APP_KTST_TOUCH;
      // �J�[�\�����\���ɂ���
      {
        GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_CURSOR], FALSE );
      }
      // �^�b�`�ɕύX�ɂȂ����ꍇ�́A�^�b�`�̏����𑱂���
    }
  }
  else
  {
    if( GFL_UI_KEY_GetTrg() )
    {
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )  // �߂�{�^���̂Ƃ��͉������Ȃ��Ŗ߂�
      {
        work->ktst = GFL_APP_KTST_KEY;
        return;
      }
      else
      {
        work->ktst = GFL_APP_KTST_KEY;
        // �J�[�\����z�u����
        Zukan_Detail_Map_SetCursor( param, work, cmn );
        Zukan_Detail_Map_ChangePlace( param, work, cmn );
        // �L�[�ɕύX�ɂȂ����ꍇ�́A�L�[�ɕύX���������ŏ������I����
        return;
      }
    }
  }

  // �L�[���͂őI�𑀍�
  if( work->ktst == GFL_APP_KTST_KEY )
  {
    u8            select_data_idx_prev  = work->select_data_idx;
    GFL_CLACTPOS  pos;
    int           key_cont;
    BOOL          user_move             = FALSE;  // ���[�U�̏㉺���E�̓��͂ɂ��ړ�������Ȃ�TRUE
    BOOL          pull_move             = FALSE;  // �z�����݂ɂ��ړ�������Ȃ�TRUE
	  GFL_CLACTPOS  pull_pos;
    BOOL          pull_force            = FALSE;  // �����z�����݂����Ȃ��ꍇ�A�����I�ɋz�����܂��邩�ۂ�
    VecFx32	norm	= {0,0,0};
	  VecFx32	pull	= {0,0,0};

    // �ړ��O�̈ʒu
    GFL_CLACT_WK_GetPos( work->obj_clwk[OBJ_CURSOR], &pos, CLSYS_DEFREND_MAIN );

    // �ړ���̈ʒu
    // ���[�U�̏㉺���E�̓��͂ɂ��ړ�
    key_cont = GFL_UI_KEY_GetCont();
    if(    ( key_cont & PAD_KEY_RIGHT )
        || ( key_cont & PAD_KEY_LEFT )
        || ( key_cont & PAD_KEY_UP )
        || ( key_cont & PAD_KEY_DOWN ) )
    {
      user_move = TRUE;
      
      if( key_cont & PAD_KEY_RIGHT ) norm.x += FX32_ONE;
      if( key_cont & PAD_KEY_LEFT )  norm.x -= FX32_ONE;
      if( key_cont & PAD_KEY_UP )    norm.y -= FX32_ONE;
      if( key_cont & PAD_KEY_DOWN )  norm.y += FX32_ONE;
    }

    // �z�����܂�鏈��
    // �z�����ݔ͈͓��Ȃ�΋z�����܂��
    {
      u8 pull_data_idx = Zukan_Detail_Map_PullHit( param, work, cmn, (u8)pos.x, (u8)pos.y ); 
      if( pull_data_idx != DATA_IDX_NONE )
      {
        VecFx32  place_pos;
        VecFx32  now_pos;
        
        pull_move = TRUE;
        pull_pos.x = (s16)TOWNMAP_DATA_GetParam( work->townmap_data, pull_data_idx, TOWNMAP_DATA_PARAM_CURSOR_X );
        pull_pos.y = (s16)TOWNMAP_DATA_GetParam( work->townmap_data, pull_data_idx, TOWNMAP_DATA_PARAM_CURSOR_Y );

        place_pos.x = FX32_CONST( pull_pos.x );
        place_pos.y = FX32_CONST( pull_pos.y );

        place_pos.z = 0;

        now_pos.x = FX32_CONST( pos.x );
        now_pos.y = FX32_CONST( pos.y );
        now_pos.z = 0;

        VEC_Subtract( &place_pos, &now_pos, &pull );

        // �����z�����݂����Ȃ��ꍇ�A�����I�ɋz�����܂��邩�ۂ�
        if( VEC_Mag(&pull) < PLACE_PULL_FORCE )
        {
          pull_force = TRUE;
        }
        // �������ȏ�̋����Ȃ�΁A�ő�l�܂łɂ����߂�
        if( VEC_Mag(&pull) > PLACE_PULL_STRONG )
        {
          VEC_Normalize( &pull, &pull );
          GFL_CALC3D_VEC_MulScalar( &pull, PLACE_PULL_STRONG, &pull );
        }
      }
    }

    // �ړ��v�Z
    if( user_move || pull_move )
    {
      // �z�����݂����Ȃ��A�����I�ɋz�����܂���ꍇ
      if( (!user_move) && pull_move && pull_force )
      {
        pos = pull_pos;
      }
      else
      {
    		VecFx32	add;
	    	VEC_Normalize( &norm, &norm );

		    add.x	= (norm.x * CURSOR_ADD_SPEED)	+ pull.x;
		    add.y	= (norm.y * CURSOR_ADD_SPEED)	+ pull.y;
		    add.z	=	0;

	    	if( 0 < add.x && add.x < FX32_ONE )
		    {	
			    add.x	= FX32_ONE;
    		}
	    	else if( -FX32_ONE < add.x && add.x < 0 )
		    {	
		    	add.x = -FX32_ONE;
	    	}
	    	if( 0 < add.y && add.y < FX32_ONE )
		    {	
		    	add.y	= FX32_ONE;
	    	}
	    	else if( -FX32_ONE < add.y && add.y < 0 )
		    {	
			    add.y = -FX32_ONE;
    		}

	    	pos.x	+= add.x >> FX32_SHIFT; 
	    	pos.y	+= add.y >> FX32_SHIFT;
      }

      pos.x = MATH_CLAMP( pos.x, CURSOR_MOVE_RECT_X_MIN, CURSOR_MOVE_RECT_X_MAX );
      pos.y = MATH_CLAMP( pos.y, CURSOR_MOVE_RECT_Y_MIN, CURSOR_MOVE_RECT_Y_MAX );
      
      GFL_CLACT_WK_SetPos( work->obj_clwk[OBJ_CURSOR], &pos, CLSYS_DEFREND_MAIN );
    
      // �q�b�g����
      work->select_data_idx = Zukan_Detail_Map_Hit( param, work, cmn, (u8)pos.x, (u8)pos.y );

      if( work->select_data_idx != select_data_idx_prev )
      {
        // �ꏊ�X�V
        Zukan_Detail_Map_ChangePlace( param, work, cmn );
 
        if( work->select_data_idx != DATA_IDX_NONE )
        {
          PMSND_PlaySE( SEQ_SE_SELECT1 );
        }
      }
    }
  }
  // �^�b�`���͂őI�𑀍�
  else
  {
    if( GFL_UI_TP_GetPointTrg( &x, &y ) )
    {
      // �q�b�g����
      work->select_data_idx = Zukan_Detail_Map_Hit( param, work, cmn, (u8)x, (u8)y );

      // �ꏊ�X�V
      Zukan_Detail_Map_ChangePlace( param, work, cmn );
      
      if( work->select_data_idx != DATA_IDX_NONE )
      {
        PMSND_PlaySE( SEQ_SE_DECIDE1 );
      }
    }
  }
}

//-------------------------------------
/// ��Ԃ�J�ڂ�����
//=====================================
static void Zukan_Detail_Map_ChangeState( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                 STATE state )
{
  // �J�ڑO��STATE = work->state
  // �J�ڌ��STATE = state 
  
  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
 
  // �J�ڑO�̏�� 
  switch(work->state)
  {
  case STATE_TOP:
    {
      if( state == STATE_SELECT )  // �J�ڌ�̏�Ԃ�STATE_SELECT�Ȃ�
      {
        work->scroll_state             = SCROLL_STATE_DOWN_TO_UP;
        work->pokeicon_scroll_state    = SCROLL_STATE_DOWN_TO_UP;

        // �J�[�\����\������
        if( work->habitat == HABITAT_KNOWN )
        {
          if( work->ktst == GFL_APP_KTST_KEY )
          {
            Zukan_Detail_Map_SetCursorOnPlayer( param, work, cmn );
          }
          else
          {
            u32 x, y;
            if( GFL_UI_TP_GetPointTrg( &x, &y ) )
            {
              // �q�b�g����
              work->select_data_idx = Zukan_Detail_Map_Hit( param, work, cmn, (u8)x, (u8)y );
              if( work->select_data_idx != DATA_IDX_NONE )
              {
                // �����O��\������
                GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_RING_CUR], TRUE );
              }
            }
          }
          Zukan_Detail_Map_ChangePlace( param, work, cmn );
        }

        // �^�b�`�o�[
        //ZUKAN_DETAIL_TOUCHBAR_SetType(
        //    touchbar,
        //    ZUKAN_DETAIL_TOUCHBAR_TYPE_MAP,
        //    ZUKAN_DETAIL_TOUCHBAR_DISP_MAP );
        //���̊�Lock���Ă������ق�����������
        work->vblank_req = 2;
      }
    }
    break;
  case STATE_SELECT:
    {
      if( state == STATE_TOP )  // �J�ڌ�̏�Ԃ�STATE_TOP�Ȃ�
      {
        work->scroll_state             = SCROLL_STATE_UP_TO_DOWN;
        work->pokeicon_scroll_state    = SCROLL_STATE_UP_TO_DOWN;
       
        // �J�[�\�����\���ɂ���
        {
          GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_CURSOR], FALSE );
        }
        // �����O���\���ɂ���
        {
          GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_RING_CUR], FALSE );
        }

        // �^�b�`�o�[
        //ZUKAN_DETAIL_TOUCHBAR_SetType(
        //    touchbar,
        //    ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL,
        //    ZUKAN_DETAIL_TOUCHBAR_DISP_MAP );
        //���̊�Lock���Ă������ق�����������
        work->vblank_req = 1;
        {
          GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
          ZUKAN_DETAIL_TOUCHBAR_SetCheckFlipOfGeneral(
              touchbar,
              GAMEDATA_GetShortCut( gamedata, SHORTCUT_ID_ZUKAN_MAP ) );
        }
      }
    }
    break;
  }

  // �J�ڌ�̏��
  work->state = state;
}

static void Zukan_Detail_Map_ChangePoke( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �ύX�O�̃|�P���� = �擾�s��
  // �ύX��̃|�P���� = ZKNDTL_COMMON_GetCurrPoke(cmn)

  // �|�P������ύX�����Ƃ�

  // �ύX��̃|�P����
  u16 monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);

  Zukan_Detail_Map_AreaDataUnload( work->area_data );
  work->area_data = Zukan_Detail_Map_AreaDataLoad( monsno, param->heap_id, work->area_handle );

  work->appear_rule  = ( work->area_data->year == 0 )?( APPEAR_RULE_SEASON ):( APPEAR_RULE_YEAR );
  if( work->appear_rule == APPEAR_RULE_YEAR )
  {
    work->habitat      = ( work->area_data->season_data[PMSEASON_SPRING].unknown == 0 )?( HABITAT_KNOWN ):( HABITAT_UNKNOWN );  // ��N�������Ƃ��͏t�̃f�[�^�������Ȃ�
  }
  else
  {
    work->habitat      = ( work->area_data->season_data[work->season_id].unknown == 0 )?( HABITAT_KNOWN ):( HABITAT_UNKNOWN );
  }

  // �|�P�A�C�R��
  {
    // �O�̂̈ʒu�����̂ɂ����f����
    u8 x, y;
/* �������񉺂ɉ����Ȃ��ƃ|�P������ύX�ł��Ȃ��̂ŁA���̈ʒu��n���΂悢�B
    if( work->pokeicon_clwk )
    {
      GFL_CLACTPOS pos;
      GFL_CLACT_WK_GetPos( work->pokeicon_clwk, &pos, CLSYS_DEFREND_SUB );
      x = (u8)(pos.x);
      y = (u8)(pos.y);
    }
    else
*/
    {
      x = pokeicon_pos[0];
      y = pokeicon_pos[1];
    }

    // �O�̂�j��
    BLOCK_POKEICON_EXIT( &work->pokeicon_res, work->pokeicon_clwk )
    // ���̂𐶐�
    {
      u32 sex;
      BOOL rare;
      u32 form;

      GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
      ZUKAN_SAVEDATA* zkn_sv = GAMEDATA_GetZukanSave( gamedata );
      ZUKANSAVE_GetDrawData(  zkn_sv, monsno, &sex, &rare, &form, param->heap_id );

      work->pokeicon_clwk = PokeiconInit( &work->pokeicon_res, work->clunit, param->heap_id, monsno, form, sex, FALSE, x, y );
    }
  }

  // �|�P�������̂Ԃ��
  // �O�̂��N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_POKENAME]), 0 );
  GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_POKENAME] );
  // ���̂�\�� 
  {
    WORDSET* wordset;
    STRBUF* src_strbuf;
    STRBUF* strbuf;
    wordset = WORDSET_Create( param->heap_id );
    src_strbuf = GFL_MSG_CreateString( work->msgdata[MSG_ZUKAN], ZKN_MAP_TEXT_01 );
    strbuf = GFL_STR_CreateBuffer( STRBUF_POKENAME_LENGTH, param->heap_id );
    WORDSET_RegisterPokeMonsNameNo( wordset, 0, monsno );
    WORDSET_ExpandStr( wordset, strbuf, src_strbuf );
    GFL_STR_DeleteBuffer( src_strbuf );
    WORDSET_Delete( wordset );
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_POKENAME]),
                            0, 5, strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
    GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[TEXT_POKENAME] );
    GFL_STR_DeleteBuffer( strbuf );
  }

  // �G�ߕ\��
  // �O�̂��N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_SEASON]), 0 );
  GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_SEASON] );
  GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_SEASON], FALSE );
  GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_SEASON_R], FALSE );
  GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_SEASON_L], FALSE );

  // ���̂�\��
  // 1�N���������z�̂Ƃ�
  if( work->appear_rule == APPEAR_RULE_YEAR )
  {
    // �������Ȃ�
  }
  // �G�߂��Ƃɕ��z���ς��Ƃ�
  else
  {
    Zukan_Detail_Map_UtilPrintSeason( param, work, cmn );
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_SEASON], TRUE );

    GFL_CLACT_WK_SetAnmSeq( work->obj_clwk[OBJ_SEASON_R], SEASON_R_ARROW_ANMSEQ_NORMAL );
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_SEASON_R], TRUE );
    GFL_CLACT_WK_SetAnmSeq( work->obj_clwk[OBJ_SEASON_L], SEASON_L_ARROW_ANMSEQ_NORMAL );
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_SEASON_L], TRUE );
  }

  // ���̋G�߂̐����n��\������
  Zukan_Detail_Map_UtilDrawSeasonArea( param, work, cmn );
}

static void Zukan_Detail_Map_ChangePlace( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �ύX�O�̏ꏊ = �擾�s��
  // �ύX��̏ꏊ = work->select_data_idx
  
  // �ꏊ��ύX�����Ƃ�
  
  // �O�̂��N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_PLACENAME]), 0 );
  GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_PLACENAME] );
  GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_RING_CUR], FALSE );

  // ���̂�\��
  if( work->select_data_idx != DATA_IDX_NONE )
  {
    STRBUF* strbuf;
		u16 zone_id = TOWNMAP_DATA_GetParam( work->townmap_data, work->select_data_idx, TOWNMAP_DATA_PARAM_ZONE_ID );
		GF_ASSERT( zone_id != TOWNMAP_DATA_ERROR );
    strbuf = GFL_MSG_CreateString( work->msgdata[MSG_PLACE], ZONEDATA_GetPlaceNameID( zone_id ) );
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_PLACENAME]),
                            0, 1, strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
    GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[TEXT_PLACENAME] );
    GFL_STR_DeleteBuffer( strbuf );

    {
      GFL_CLACTPOS pos;
      pos.x = (s16)TOWNMAP_DATA_GetParam( work->townmap_data, work->select_data_idx, TOWNMAP_DATA_PARAM_POS_X );
      pos.y = (s16)TOWNMAP_DATA_GetParam( work->townmap_data, work->select_data_idx, TOWNMAP_DATA_PARAM_POS_Y );
      GFL_CLACT_WK_SetAnmSeq( work->obj_clwk[OBJ_RING_CUR], RING_CUR_ANMSEQ );
      GFL_CLACT_WK_SetPos( work->obj_clwk[OBJ_RING_CUR], &pos, CLSYS_DEFREND_MAIN );
      GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_RING_CUR], TRUE );
    }
  }

  // �����A���A�ނ�̃A�C�R���̖��邳��ύX����
  Zukan_Detail_Map_UtilBrightenPlaceIcon( param, work, cmn );
}

static void Zukan_Detail_Map_ChangeSeason( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn, BOOL b_next )
{
  // �ύX�O�̋G�� = �擾�s��
  // �ύX��̋G�� = work->season_id

  // �G�߂�ύX�����Ƃ�

  // 1�N���������z�̂Ƃ��́A���̊֐��͌Ăяo����Ȃ�

  // �ύX��̋G��
  if( work->appear_rule == APPEAR_RULE_YEAR )
  {
    work->habitat      = ( work->area_data->season_data[PMSEASON_SPRING].unknown == 0 )?( HABITAT_KNOWN ):( HABITAT_UNKNOWN );  // ��N�������Ƃ��͏t�̃f�[�^�������Ȃ�
  }
  else
  {
    work->habitat      = ( work->area_data->season_data[work->season_id].unknown == 0 )?( HABITAT_KNOWN ):( HABITAT_UNKNOWN );
  }

  // �G�ߕ\��
  // �O�̂��N���A
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_SEASON]), 0 );
  GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_SEASON] );

  // ���̂�\��
  Zukan_Detail_Map_UtilPrintSeason( param, work, cmn );
 
  if( b_next )
  {
    GFL_CLACT_WK_SetAnmSeq( work->obj_clwk[OBJ_SEASON_R], SEASON_R_ARROW_ANMSEQ_SELECT );
  }
  else
  {
    GFL_CLACT_WK_SetAnmSeq( work->obj_clwk[OBJ_SEASON_L], SEASON_L_ARROW_ANMSEQ_SELECT );
  }

  // �^�b�`���͂���Ă����ɗ����ꍇ�́A�J�[�\�����\���ɂ���
  if( work->ktst == GFL_APP_KTST_TOUCH )
  {
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_CURSOR], FALSE );
  }
  // �L�[���͂���Ă����ɗ����ꍇ�́A�J�[�\�������݂̈ʒu or �v���C���[�̈ʒu�ɕ\������
  else
  {
    if( work->state == STATE_SELECT )  // �}�b�v�̋��_��I�ׂ��ԂłȂ���΁A�J�[�\���͕\�����Ȃ��Ă���
    {
      Zukan_Detail_Map_SetCursor( param, work, cmn );
    }
  }

  // ���̋G�߂̐����n��\������
  Zukan_Detail_Map_UtilDrawSeasonArea( param, work, cmn );

  // �����A���A�ނ�̃A�C�R���̖��邳��ύX����
  Zukan_Detail_Map_UtilBrightenPlaceIcon( param, work, cmn );
}

//-------------------------------------
/// ���z�f�[�^
//=====================================
static AREA_DATA* Zukan_Detail_Map_AreaDataLoad( u16 monsno, HEAPID heap_id, ARCHANDLE* handle )
{
//#if 0
  // �{�f�[�^
  u32 size;
  //AREA_DATA* area_data = GFL_ARCHDL_UTIL_LoadEx( handle, NARC_zukan_area_w_zkn_area_w_monsno_001_dat + monsno -1, FALSE, heap_id, &size );
  //AREA_DATA* area_data = GFL_ARCHDL_UTIL_LoadEx( handle, NARC_zukan_area_b_zkn_area_b_monsno_001_dat + monsno -1, FALSE, heap_id, &size );
  AREA_DATA* area_data = GFL_ARCHDL_UTIL_LoadEx( handle, monsno -1, FALSE, heap_id, &size );
  GF_ASSERT_MSG( sizeof(AREA_DATA) == size, "ZUKAN_DETAIL_MAP : AREA_DATA�̃T�C�Y���قȂ�܂��B\n" );
  return area_data;
//#endif

#if 0
  // ���f�[�^
  AREA_DATA* area_data = GFL_HEAP_AllocClearMemory( heap_id, sizeof(AREA_DATA) );
  s32 case_no = monsno %3;
  if( monsno == 517 ) case_no = 0;
  if( monsno == 534 ) case_no = 1;
  if( monsno == 569 ) case_no = 2;
  switch( case_no )
  {
  case 0:
    {
      area_data->year = 0;
      area_data->season_data[PMSEASON_SPRING].unknown = 0;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[0] = PLACE_BITFLAG_GROUND_L | PLACE_BITFLAG_WATER | PLACE_BITFLAG_FISHING;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[1] = PLACE_BITFLAG_GROUND_L;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[2] = PLACE_BITFLAG_WATER;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[3] = PLACE_BITFLAG_FISHING;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[4] = PLACE_BITFLAG_WATER | PLACE_BITFLAG_FISHING;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[5] = PLACE_BITFLAG_GROUND_L | PLACE_BITFLAG_FISHING;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[6] = PLACE_BITFLAG_GROUND_L | PLACE_BITFLAG_WATER;
      area_data->season_data[PMSEASON_SUMMER].unknown = 0;
      area_data->season_data[PMSEASON_AUTUMN].unknown = 1;
    }
    break;
  case 1:
    {
      area_data->year = 1;
      area_data->season_data[PMSEASON_SPRING].unknown = 0;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[0] = PLACE_BITFLAG_GROUND_L | PLACE_BITFLAG_WATER | PLACE_BITFLAG_FISHING;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[1] = PLACE_BITFLAG_GROUND_L;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[2] = PLACE_BITFLAG_WATER;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[3] = PLACE_BITFLAG_FISHING;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[4] = PLACE_BITFLAG_WATER | PLACE_BITFLAG_FISHING;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[5] = PLACE_BITFLAG_GROUND_L | PLACE_BITFLAG_FISHING;
      area_data->season_data[PMSEASON_SPRING].place_bitflag[6] = PLACE_BITFLAG_GROUND_L | PLACE_BITFLAG_WATER;
    }
    break;
  case 2:
    {
      area_data->year = 1;
      area_data->season_data[PMSEASON_SPRING].unknown = 1;
    }
    break;
  }
  return area_data;
#endif


#ifdef DEBUG_SET_ANIME_AND_POS
  // ��������ꏊ�ɒu��OBJ�̃A�j���w��A�ʒu�w��p�̃f�[�^
  AREA_DATA* area_data = GFL_HEAP_AllocClearMemory( heap_id, sizeof(AREA_DATA) );
  u8 i;
  {
    area_data->year = 1;
    area_data->season_data[PMSEASON_SPRING].unknown = 0;

    for( i=0; i<TOWNMAP_DATA_MAX; i++ )
    {
      area_data->season_data[PMSEASON_SPRING].place_bitflag[i] = PLACE_BITFLAG_GROUND_ALL | PLACE_BITFLAG_WATER_ALL | PLACE_BITFLAG_FISHING_ALL;
    }
  }
  return area_data;
#endif


}
static void Zukan_Detail_Map_AreaDataUnload( AREA_DATA* area_data )
{
  if( area_data ) GFL_HEAP_FreeMemory( area_data );
  area_data = NULL;
}

//-------------------------------------
/// ���x���o�Ă��鏈��
//=====================================
static void Zukan_Detail_Map_UtilPrintSeason( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  STRBUF* strbuf;
  u16     str_width;
  u16     bmp_width;
  u16     x;
  strbuf = GFL_MSG_CreateString( work->msgdata[MSG_ZUKAN], ZKN_MAP_SPRING + work->season_id );
  str_width = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
  bmp_width = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_SEASON]) );
  x = ( bmp_width - str_width ) / 2;
  PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_SEASON]),
                          x, 1, strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[TEXT_SEASON] );
  GFL_STR_DeleteBuffer( strbuf );
}

static void Zukan_Detail_Map_UtilDrawSeasonArea( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;

  // �O�̂��N���A
  
  // �����n�s��
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_UNKNOWN]), 0 );
  GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_UNKNOWN] );
  GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_UNKNOWN], FALSE );

  // �����n����
  for( i=0; i<TOWNMAP_DATA_MAX; i++ )
  {
    GFL_CLACT_WK_SetDrawEnable( work->obj_exist_clwk[i], FALSE );
  }


  // ���̋G�߂̐����n��\������
  
  // ���̋G�߂̐����n���s���̂Ƃ�
  if( work->habitat == HABITAT_UNKNOWN )
  {
    STRBUF* strbuf;
    u16     str_width;
    u16     bmp_width;
    u16     x;
    strbuf = GFL_MSG_CreateString( work->msgdata[MSG_ZUKAN], ZNK_RANGE_00 );
    str_width = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
    bmp_width = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_UNKNOWN]) );
    x = ( bmp_width - str_width ) / 2;
    PRINTSYS_PrintQueColor( work->print_que, GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_UNKNOWN]),
                            x, 1, strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
    GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[TEXT_UNKNOWN] );
    GFL_STR_DeleteBuffer( strbuf );

    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_UNKNOWN], TRUE );
  }
  // ���̋G�߂̐����n���������Ă���Ƃ�
  else
  {
    u8 season_id = ( work->appear_rule == APPEAR_RULE_YEAR )?(PMSEASON_SPRING):(work->season_id);  // ��N�������Ƃ��͏t�̃f�[�^�������Ȃ�
        
    // �����ꏊ��OBJ��\������
    for( i=0; i<TOWNMAP_DATA_MAX; i++ )
    {
      u8 zkn_area_idx = work->townmap_data_idx_to_zkn_area_idx[i];
      if( work->area_data->season_data[season_id].place_bitflag[zkn_area_idx] != PLACE_BITFLAG_NONE )
      {
        if( work->hide_spot[i].state != HIDE_STATE_HIDE_TRUE )  // �B���}�b�v�Ȃ��΂ɂ�����Ȃ�
        {
          GFL_CLACT_WK_SetDrawEnable( work->obj_exist_clwk[i], TRUE );
        }
      }
    }
  }
}

static void Zukan_Detail_Map_UtilBrightenPlaceIcon( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �����A���A�ނ�̃A�C�R���̖��邳��ύX����

  u8 i;
  u8 brighten[PLACE_ICON_MAX] = { 0, 0, 0 };

  // ���̋G�߂̐����n���������Ă���Ƃ�
  if( work->habitat == HABITAT_KNOWN )
  {
    if( work->select_data_idx != DATA_IDX_NONE )
    {
      u8 season_id = ( work->appear_rule == APPEAR_RULE_YEAR )?(PMSEASON_SPRING):(work->season_id);  // ��N�������Ƃ��͏t�̃f�[�^�������Ȃ�
      u8 zkn_area_idx = work->townmap_data_idx_to_zkn_area_idx[work->select_data_idx];
      
      if( work->area_data->season_data[season_id].place_bitflag[zkn_area_idx] & PLACE_BITFLAG_GROUND_ALL )
      {
        brighten[PLACE_ICON_GROUND] = 1;
      }
      if( work->area_data->season_data[season_id].place_bitflag[zkn_area_idx] & PLACE_BITFLAG_WATER_ALL )
      {
        brighten[PLACE_ICON_WATER] = 1;
      }
      if( work->area_data->season_data[season_id].place_bitflag[zkn_area_idx] & PLACE_BITFLAG_FISHING_ALL )
      {
        brighten[PLACE_ICON_FISHING] = 1;
      }
    }
  }

  // �A�C�R���̖��邳��ύX����
  for( i=0; i<PLACE_ICON_MAX; i++ )
  {
    GFL_BG_ChangeScreenPalette(
        BG_FRAME_S_PLACE,
        place_icon_screen_chr[i][0],
        place_icon_screen_chr[i][1],
        place_icon_screen_chr[i][2],
        place_icon_screen_chr[i][3],
        place_icon_bg_pal_pos[i][brighten[i]] );
  }

  GFL_BG_LoadScreenV_Req( BG_FRAME_S_PLACE );
}

//-------------------------------------
/// obj_exist�̃A���t�@�A�j���[�V����
//=====================================
static void Zukan_Detail_Map_ObjExistAlphaInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;

  // �������ɂ��Ȃ���wnd�ɓ����Ă��܂�OBJ�̐ݒ��ύX����
  for( i=0; i<OBJ_MAX; i++ )
  {
    GFL_CLACT_WK_SetObjMode( work->obj_clwk[i], GX_OAM_MODE_NORMAL );  // �A���t�@�A�j���[�V�����̉e�����󂯂Ȃ��悤�ɂ���
  }
  for( i=0; i<TOWNMAP_DATA_MAX; i++ )
  {
    if( work->hide_spot[i].state == HIDE_STATE_HIDE_FALSE )
    {
      GFL_CLACT_WK_SetObjMode( work->hide_spot[i].obj_clwk, GX_OAM_MODE_NORMAL );  // �A���t�@�A�j���[�V�����̉e�����󂯂Ȃ��悤�ɂ���
    }
  }

  // wnd
  GX_SetVisibleWnd( GX_WNDMASK_W0 | GX_WNDMASK_W1 );
  G2_SetWnd0Position( SEASON_PANEL_POS_X, 0, SEASON_PANEL_POS_X+SEASON_PANEL_SIZE_X, 168 );
  G2_SetWnd1Position( SEASON_PANEL_POS_X+SEASON_PANEL_SIZE_X, 0, 0/*256*/, 168 );

  G2_SetWnd0InsidePlane(
    GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
    TRUE );
  G2_SetWnd1InsidePlane(
    GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
    TRUE );
  G2_SetWndOutsidePlane(
    GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ,
    FALSE );

  Zukan_Detail_Map_ObjExistAlphaReset( param, work, cmn );
}
static void Zukan_Detail_Map_ObjExistAlphaExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;

  // wnd
  GX_SetVisibleWnd( GX_WNDMASK_NONE );

  // �������ɂ��Ȃ���wnd�ɓ����Ă��܂�OBJ�̐ݒ�����ɖ߂�
  for( i=0; i<OBJ_MAX; i++ )
  {
    GFL_CLACT_WK_SetObjMode( work->obj_clwk[i], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
  }
  for( i=0; i<TOWNMAP_DATA_MAX; i++ )
  {
    if( work->hide_spot[i].state == HIDE_STATE_HIDE_FALSE )
    {
      GFL_CLACT_WK_SetObjMode( work->hide_spot[i].obj_clwk, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    }
  }

  // �ꕔ���t�F�[�h�̐ݒ�����ɖ߂�
  ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_m );
}
static void Zukan_Detail_Map_ObjExistAlphaMain( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �X�V
  {
    G2_SetBlendAlpha(
        GX_BLEND_PLANEMASK_NONE,/*GX_BLEND_PLANEMASK_OBJ,*/
        GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
        work->obj_exist_ev1,
        16 - work->obj_exist_ev1 );
  }

  // �J�E���g��i�߂�
  if( (u32)work->obj_exist_alpha_anime_count + OBJ_EXIST_ALPHA_ANIME_COUNT_ADD >= OBJ_EXIST_ALPHA_ANIME_COUNT_MAX )
  {
    work->obj_exist_alpha_anime_count = (u16)( (u32)work->obj_exist_alpha_anime_count + OBJ_EXIST_ALPHA_ANIME_COUNT_ADD - OBJ_EXIST_ALPHA_ANIME_COUNT_MAX );
  }
  else
  {
    work->obj_exist_alpha_anime_count += OBJ_EXIST_ALPHA_ANIME_COUNT_ADD;
  }

  // ev1�����߂� 
  {
    // 1�`0�ɕϊ�
    fx16 cos = ( FX_CosIdx(work->obj_exist_alpha_anime_count) + FX16_ONE ) /2;
    work->obj_exist_ev1 = OBJ_EXIST_ALPHA_MIN + ( ( (OBJ_EXIST_ALPHA_MAX - OBJ_EXIST_ALPHA_MIN) * cos ) >>FX16_SHIFT );
  }

// TWL�v���O���~���O�}�j���A��
// �� 1 �Ώۖʂ�OBJ=1 �̏ꍇ�́AOBJ �̎�ނɊւ�炸���ׂĂ�OBJ�ɑ΂��ď��������s����܂��B
// OBJ=0 �̏ꍇ�́A������OBJ�A�r�b�g�}�b�vOBJ�A3D �ʂ�`�悵��BG0 �ʂ̏ꍇ�̂ݏ��������s����܂��B
//
// �܂�AGX_BLEND_PLANEMASK_OBJ�ɂ��Ă��܂��ƁAGX_OAM_MODE_NORMAL���������ɂȂ��Ă��܂��B
// GX_BLEND_PLANEMASK_NONE�ɂ��Ă����΁AGX_OAM_MODE_XLU�������������ɂȂ�B
}
static void Zukan_Detail_Map_ObjExistAlphaReset( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  work->obj_exist_ev1                  = OBJ_EXIST_ALPHA_MIN;
  work->obj_exist_alpha_anime_count    = OBJ_EXIST_ALPHA_ANIME_COUNT_MAX / 2;  // cos��1�`0�ɂ��Ă���̂�
}

//-------------------------------------
/// �A���t�@�ݒ�
//=====================================
static void Zukan_Detail_Map_AlphaInit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  int ev1 = 12;
  G2S_SetBlendAlpha(
        GX_BLEND_PLANEMASK_BG3,
        GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
        ev1,
        16 - ev1 );
}
static void Zukan_Detail_Map_AlphaExit( ZUKAN_DETAIL_MAP_PARAM* param, ZUKAN_DETAIL_MAP_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �ꕔ���t�F�[�h�̐ݒ�����ɖ߂�
  ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_s );
}

