//============================================================================
/**
 *  @file   zukan_detail_form.c
 *  @brief  �}�ӏڍ׉�ʂ̎p���
 *  @author Koji Kawada
 *  @data   2010.02.02
 *  @note   
 *  ���W���[�����FZUKAN_DETAIL_FORM
 */
//============================================================================

#define DEF_SCMD_CHANGE  // ���ꂪ��`����Ă���Ƃ��ACMD�����s�����܂ő҂�����SCMD�����s���ꂽ�Ƃ��ɕύX���J�n����
#define DEF_MCSS_HANDLE  // ���ꂪ��`����Ă���Ƃ��AMCSS�̃n���h���ǂݍ��݂𗘗p����
#define DEF_MCSS_TCBSYS  // ���ꂪ��`����Ă���Ƃ��AMCSS��TCBSYS�O���w��𗘗p����
#define DEF_MINIMUM_LOAD  // ���ꂪ��`����Ă���Ƃ��A�|�P�����ύX��t�H�����ύX�ł��̂Ƃ������Ă���ŏ����K�v�Ȃ��̂����ǂݍ��܂Ȃ�


//#define DEBUG_KAWADA


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/mcss.h"
#include "system/mcss_tool.h"
#include "ui/ui_easy_clwk.h"
#include "pokeicon/pokeicon.h"
#include "gamesystem/gamedata_def.h"
#include "gamesystem/game_data.h"
#include "savedata/zukan_savedata.h"
#include "poke_tool/poke_personal.h"
#include "system/bmp_winframe.h"
#include "print/printsys.h"
#include "print/gf_font.h"
#include "print/wordset.h"
#include "print/global_msg.h"
#include "sound/pm_sndsys.h"

#include "zukan_detail_def.h"
#include "zukan_detail_common.h"
#include "zukan_detail_graphic.h"
#include "zukan_detail_form.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_zkn.h"
#include "msg/msg_zkn_form.h"
#include "zukan_gra.naix"
#include "zukan_data.naix"

// �T�E���h

// �I�[�o�[���C


/*
�\�����[��


1 �t�H�����Ⴂ���Ȃ��āA�I�X���X���Ȃ��Ƃ�

�~���E�̂�����
�~���E�̂�����  (���낿����)


2 �t�H�����Ⴂ���Ȃ��āA�I�X���X(�I�X�ƃ��X�ŃO���t�B�b�N��
  �Ⴂ�����낤�ƂȂ��낤�ƁA�I�X�ƃ��X��\������)������Ƃ�

�I�X�̂�����
�I�X�̂�����  (���낿����)
���X�̂�����
���X�̂�����  (���낿����)

���ӁI�F�I�X���X�ǂ��炩�Е��������Ȃ��Ƃ�������B
�Ⴆ�΁A�I�X�������Ȃ��Ƃ���

�I�X�̂�����
�I�X�̂�����  (���낿����)

�ƂȂ�B


3 �t�H�����Ⴂ������Ƃ�(�I�X���X�͂��낤�ƂȂ��낤�ƁA��������)

�m�[�}���t�H����
�m�[�}���t�H����  (���낿����)
�A�^�b�N�t�H����
�A�^�b�N�t�H����  (���낿����)
�f�B�t�F���X�t�H����
�f�B�t�F���X�t�H����  (���낿����)
�X�s�[�h�t�H����
�X�s�[�h�t�H����  (���낿����)


*/


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
// MCSS�|�P�����̃t�F�[�h
#define POKE_MCSS_FADE_WAIT  (-2)//-1�������̃t�F�[�h�ɔ�ׂĒx��(-1)  // 0�̂Ƃ����t���[��1�i��; 1�̂Ƃ�oxox�Ƃ����ӂ���1�t���[��������1�i��;
                                   // -1�̂Ƃ����t���[��2�i��; -2�̂Ƃ����t���[��3�i��;

// ���C��BG�t���[��
#define BG_FRAME_M_POKE          (GFL_BG_FRAME0_M)
#define BG_FRAME_M_TEXT          (GFL_BG_FRAME2_M)
#define BG_FRAME_M_REAR          (GFL_BG_FRAME3_M)
// ���C��BG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_M_POKE      (0)
#define BG_FRAME_PRI_M_TEXT      (2)
#define BG_FRAME_PRI_M_REAR      (3)

#define BG_FRAME_PRI_M_TOUCHBAR  (2)  // ���̉�ʂł̓^�b�`�o�[�̃v���C�I���e�B��3D�ʂ̃v���C�I���e�B��艺���Ă���
                                      // ���̂�1�ł́A�^�b�`�o�[BG����O��3D�����邪�A�^�b�`�o�[��̃A�C�R��������3D�����Ă��܂��B

// ���C��BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_TEXT              = 1,
  BG_PAL_NUM_M_REAR              = ZKNDTL_COMMON_REAR_BG_PAL_NUM,
};
// �ʒu
enum
{
  BG_PAL_POS_M_TEXT              = 0,
  BG_PAL_POS_M_REAR              = 1,
};

// ���C��OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_BUTTON_BAR       = 3,  // �O��{�^���A�Đ��{�^���A�X�N���[���o�[�A�o�[�J�[�\��
  OBJ_PAL_NUM_M_FIELD            = 1,  // �t�H�������̔w�ʃt�B�[���h
  OBJ_PAL_NUM_M_ARROW            = 2,  // �g�b�v��ʂŃt�H������؂�ւ��邽�߂̖��
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_BUTTON_BAR       = 0,
  OBJ_PAL_POS_M_FIELD            = 3,
  OBJ_PAL_POS_M_ARROW            = 4,
};


// �T�uBG�t���[��
#define BG_FRAME_S_TEXT           (GFL_BG_FRAME2_S)
#define BG_FRAME_S_PANEL          (GFL_BG_FRAME3_S)
#define BG_FRAME_S_REAR           (GFL_BG_FRAME0_S)
// �T�uBG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_S_TEXT       (1)
#define BG_FRAME_PRI_S_PANEL      (2)
#define BG_FRAME_PRI_S_REAR       (3)

// �T�uBG�p���b�g
enum
{
  BG_PAL_NUM_S_TEXT              = 1,
  BG_PAL_NUM_S_PANEL             = 1,
  BG_PAL_NUM_S_REAR              = ZKNDTL_COMMON_REAR_BG_PAL_NUM,
};
// �ʒu
enum
{
  BG_PAL_POS_S_TEXT              = 0,
  BG_PAL_POS_S_PANEL             = 1,
  BG_PAL_POS_S_REAR              = 2,
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
  SEQ_FADE_CHANGE_BEFORE,
  SEQ_PREPARE_0,
  SEQ_PREPARE,
  SEQ_FADE_IN,
  SEQ_MAIN,
  SEQ_POST,
  SEQ_FADE_OUT,
  SEQ_FADE_CHANGE_AFTER,
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

// ���
typedef enum
{
  STATE_TOP,
  STATE_TOP_OSHIDASHI,
  STATE_TOP_TO_EXCHANGE,
  STATE_TOP_TO_EXCHANGE_TOUCHBAR_CHANGE,
  STATE_EXCHANGE,
  STATE_EXCHANGE_IREKAE,
  STATE_EXCHANGE_TO_TOP,
  STATE_EXCHANGE_TO_TOP_TOUCHBAR_CHANGE,
}
STATE;

// MCSS�|�P����
typedef enum
{
  POKE_CURR_F,    // ���̃t�H����(�O����)
  POKE_CURR_B,    // ���̃t�H����(�����)
  POKE_COMP_F,    // ��r�t�H����(�O����)
  POKE_COMP_B,    // ��r�t�H����(�����)
  POKE_MAX,
}
POKE_INDEX;

// �|�P�����̑傫��
#define POKE_SCALE    (16.0f)
#define POKE_SIZE_MAX (96.0f)

// �|�P�����̈ʒu
typedef enum
{
  POKE_CURR_POS_CENTER,  // ���̃t�H�����̃|�P�����̈ʒu
  POKE_CURR_POS_LEFT, 
  POKE_COMP_RPOS,        // ��r�t�H�����̃|�P�����̈ʒu(POKE_CURR_POS_???����Ƃ������Έʒu)
  POKE_POS_MAX,
}
POKE_POS_INDEX;

static VecFx32 poke_pos[POKE_POS_MAX] =
{
  { FX_F32_TO_FX32(  0.0f), FX_F32_TO_FX32(-24.0f), FX_F32_TO_FX32(0.0f) },
  { FX_F32_TO_FX32(-64.0f), FX_F32_TO_FX32(-24.0f), FX_F32_TO_FX32(0.0f) },
  { FX_F32_TO_FX32(128.0f), FX_F32_TO_FX32(  0.0f), FX_F32_TO_FX32(0.0f) },
};

// �|�P�����̃A�j���[�V�����̃��[�v�̍ő吔
#define POKE_MCSS_ANIME_LOOP_MAX  (2)


#ifdef DEF_MCSS_TCBSYS
  #define MCSS_TCBSYS_TASK_MAX (4)  // MCSS1�ɂ�TCB1�ł����炵���B�ő�4��MCSS�𓯎��ɕێ�����̂ŁB
#endif


// �|�P�A�C�R���̈ʒu
static const u8 pokeicon_pos[2] = { 128, 128 };

// �Ⴄ�p
typedef enum
{
  LOOKS_DIFF_ONE,   // ���ʂȂ��A�t�H�����Ȃ�
  LOOKS_DIFF_SEX,   // ���ʂ���A�t�H�����Ȃ�
  LOOKS_DIFF_FORM,  // ���ʕs��A�t�H��������
}
LOOKS_DIFF;

typedef enum
{
  COLOR_DIFF_NONE,     // �F�Ⴂ�Ȃ�
  COLOR_DIFF_NORMAL,   // �F�Ⴂ����Œʏ�F
  COLOR_DIFF_SPECIAL,  // �F�Ⴂ����œ��ʐF
}
COLOR_DIFF;

enum
{
  SEX_MALE,
  SEX_FEMALE,
};

// �Ⴄ�p�̐�
#define DIFF_MAX (64)  // ����ő���邩�H

// �e�L�X�g
enum
{
  TEXT_POKENAME,
  TEXT_UP_NUM,
  TEXT_UP_LABEL,
  TEXT_POKE_CURR,
  TEXT_POKE_COMP,
  TEXT_MAX,
};
static const u16 text_up_label_num_y[2] = { 1, 17 };
// ������
#define STRBUF_NUM_LENGTH       (  8 )  // ??  // ��F�������p�̐�  99  �����̗��99�̕�����
#define STRBUF_POKENAME_LENGTH  ( 64 )  // [�|�P�����푰��]�̂�����  // buflen.h�Ŋm�F����K�v����I

#define TEXT_POKE_POS_Y_1_LINE   (  9 )
#define TEXT_POKE_POS_Y_2_LINE_0 (  1 )
#define TEXT_POKE_POS_Y_2_LINE_1 ( 17 )

// �悭�g���e�L�X�g
enum
{
  TEXT_FREQUENT_SUGATA_NUM,       // �p�̐�
  TEXT_FREQUENT_IROCHIGAI_NUM,    // �F�Ⴂ�̐�
  TEXT_FREQUENT_POKE_SUGATA,      // �|�P�������̎p
  TEXT_FREQUENT_MALE_SUGATA,      // �I�X�̎p
  TEXT_FREQUENT_FEMALE_SUGATA,    // ���X�̎p
  TEXT_FREQUENT_NOTE_IROCHIGAI,   // (�F�Ⴂ)
  TEXT_FREQUENT_MAX,
};
static const u16 text_frequent_str_id[TEXT_FREQUENT_MAX] =
{
  ZKN_FORME_TEXT_03,
  ZKN_FORME_TEXT_04,
  ZKN_FORMNAME_10,
  ZNK_ZUKAN_GRAPH_00,
  ZNK_ZUKAN_GRAPH_01,
  ZKN_FORME_TEXT_05,
};

// OBJ  // �O��{�^���A�Đ��{�^���A�X�N���[���o�[�A�o�[�J�[�\��  // �t�H�������̔w�ʃt�B�[���h
enum
{
  OBJ_RES_BUTTON_BAR_NCG,  // �O��{�^���A�Đ��{�^���A�X�N���[���o�[�A�o�[�J�[�\�� 
  OBJ_RES_BUTTON_BAR_NCL,
  OBJ_RES_BUTTON_BAR_NCE,
  OBJ_RES_FIELD_NCG,       // �t�H�������̔w�ʃt�B�[���h
  OBJ_RES_FIELD_NCL,
  OBJ_RES_FIELD_NCE,
  OBJ_RES_ARROW_NCG,       // �g�b�v��ʂŃt�H������؂�ւ��邽�߂̖��
  OBJ_RES_ARROW_NCL,
  OBJ_RES_ARROW_NCE,
  OBJ_RES_MAX,
};
enum
{
  OBJ_BAR_RANGE,          // �X�N���[���o�[
  OBJ_BAR_CURSOR,         // �o�[�J�[�\��
  OBJ_FIELD_CURR,         // �t�H�������̔w�ʃt�B�[���h
  OBJ_FIELD_COMP,         // �t�H�������̔w�ʃt�B�[���h
  OBJ_MAX,
};
#define BAR_Y_BASE (6)
static const u8 obj_setup_info[OBJ_MAX][8] =
{
  //  pos_x,         pos_y,  anmseq,  softpri,  bgpri,               ncg,                    ncl,                    nce
  {    18*8,  BAR_Y_BASE*8,      21,        1,  BG_FRAME_PRI_M_REAR, OBJ_RES_BUTTON_BAR_NCG, OBJ_RES_BUTTON_BAR_NCL, OBJ_RES_BUTTON_BAR_NCE },
  { 18*8+12,  BAR_Y_BASE*8,      22,        0,  BG_FRAME_PRI_M_REAR, OBJ_RES_BUTTON_BAR_NCG, OBJ_RES_BUTTON_BAR_NCL, OBJ_RES_BUTTON_BAR_NCE },
  {     0*8,           0*8,       0,        0,  BG_FRAME_PRI_M_REAR, OBJ_RES_FIELD_NCG,      OBJ_RES_FIELD_NCL,      OBJ_RES_FIELD_NCE      },
  {    16*8,           0*8,       0,        0,  BG_FRAME_PRI_M_REAR, OBJ_RES_FIELD_NCG,      OBJ_RES_FIELD_NCL,      OBJ_RES_FIELD_NCE      },
};

// �{�^���̏��
typedef enum
{
  BUTTON_STATE_ACTIVE,       // �������Ƃ��ł���
  BUTTON_STATE_PUSH_START,   // �������u��(1�t���[��)
  BUTTON_STATE_PUSH_ANIME,   // �������A�j����
  BUTTON_STATE_PUSH_END,     // �������A�j�����I�������u��(1�t���[��)
}
BUTTON_STATE;
// �{�^��OBJ
typedef enum
{
  BUTTON_OBJ_FRONT_BACK,  // �O��{�^��
  BUTTON_OBJ_PLAY,        // �Đ��{�^��
  BUTTON_OBJ_ARROW_L,     // �g�b�v��ʂŃt�H������؂�ւ��邽�߂̖��
  BUTTON_OBJ_ARROW_R,     // �g�b�v��ʂŃt�H������؂�ւ��邽�߂̖��
  BUTTON_OBJ_MAX,

  // �^�b�`�p
  BUTTON_OBJ_NONE,        // �{�^���������Ă��Ȃ�
  BUTTON_OBJ_IGNORE,      // �{�^�������������������ꂽ(��ɑ��̃{�^���������Ă�����A�������{�^�������ɉ������������肵���Ƃ�)
}
BUTTON_OBJ;

// �X�N���[���o�[
// �o�[�J�[�\��
#define  BAR_CURSOR_POS_Y      (BAR_Y_BASE*8)
#define  BAR_CURSOR_POS_X_MIN  (18*8+12)                 // BAR_CURSOR_POS_X_MIN<= <=BAR_CURSOR_POS_X_MAX
#define  BAR_CURSOR_POS_X_MAX  (18*8+12 +8*8)
// �X�N���[���o�[
#define  BAR_RANGE_TOUCH_X_MIN  (18*8)                   // BAR_RANGE_TOUCH_X_MIN<= <=BAR_RANGE_TOUCH_X_MAX
#define  BAR_RANGE_TOUCH_X_MAX  (18*8 +11*8-1)
#define  BAR_RANGE_TOUCH_Y_MIN  (BAR_Y_BASE*8)           // BAR_RANGE_TOUCH_Y_MIN<= <=BAR_RANGE_TOUCH_Y_MAX
#define  BAR_RANGE_TOUCH_Y_MAX  (BAR_Y_BASE*8 +1*8-1)


// ����ւ�
typedef enum
{
  IREKAE_STATE_NONE,    // ����ւ����Ă��Ȃ�
  IREKAE_STATE_MOVE,    // ����ւ���
}
IREKAE_STATE;

#define IREKAE_THETA_ADD    (0x400)  // ������  // FX_SinIdx
#define IREKAE_THETA_MAX   (0x8000)  // ����    // FX_SinIdx


// �����o��
typedef enum
{
  OSHIDASHI_STATE_NONE,    // �����o�����Ă��Ȃ�
  OSHIDASHI_STATE_MOVE,    // �����o����
  OSHIDASHI_STATE_CHANGE_COMPARE_FORM,    // �����o�������������̂ŁA��r�t�H������ύX����
}
OSHIDASHI_STATE;
typedef enum
{
  OSHIDASHI_DIRECT_L_TO_R,  // ������E��
  OSHIDASHI_DIRECT_R_TO_L,  // �E���獶��
}
OSHIDASHI_DIRECT;

#define OSHIDASHI_SPEED  (8.0f)  // �����o�����x(1�t���[���ł��ꂾ���ړ�����)


// �K�w�ύX  // TOP_TO_EXCHANGE or EXCHANGE_TO_TOP
#define KAISOU_CURR_SPEED  (2.0f)  // �K�w�ύX�̍ۂ�curr�̑��x(1�t���[���ł��ꂾ���ړ�����)
#define KAISOU_COMP_SPEED  (8.0f)  // �K�w�ύX�̍ۂ�comp�̑��x(1�t���[���ł��ꂾ���ړ�����)


// OBJ�������ւ���ۂɗ���Ȃ��悤�ɁA2�����݂ɕ\������
typedef enum
{
  OBJ_SWAP_0,
  OBJ_SWAP_1,
  OBJ_SWAP_MAX,
}
OBJ_SWAP;


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
// �{�^��
typedef struct
{
  s16           set_pos_x;  // �z�u���W
  s16           set_pos_y;
  u32           set_ncg;    // ���\�[�X
  u32           set_ncl;
  u32           set_nce;
  u8            pos_x;   // �s�N�Z��  // ����  // �^�b�`���W
  u8            pos_y; 
  u8            size_x;  // �s�N�Z��           // �^�b�`�T�C�Y
  u8            size_y;
  u8            anmseq_active;  // �������Ƃ��ł���Ƃ��̃A�j���ԍ�
  u8            anmseq_push;    // �������A�j�����̃A�j���ԍ�
  u32           key;
  u32           se;
  BUTTON_STATE  state;
  GFL_CLWK*     clwk;
}
BUTTON;

// NNS_G2dSetAnimCtrlCallBackFunctor�ɓn�����[�U��`���param
typedef struct
{
  POKE_INDEX  poke_idx;
  void*       zukan_detail_form_work;  // (ZUKAN_DETAIL_FORM_WORK*)�Ƃ����ӂ��ɃL���X�g���Ďg���ĉ�����
  u8          count;   // �A�j���[�V���������񃋁[�v������
  BOOL        stop;    // TRUE�̂Ƃ��A�A�j���[�V�������~�߂�K�v����
}
POKE_MCSS_CALL_BACK_DATA;

// �|�P����MCSS
typedef struct
{
  MCSS_WORK*                 poke_wk;              // NULL�̂Ƃ��Ȃ�
  POKE_MCSS_CALL_BACK_DATA*  poke_call_back_data;  // poke_wk�ƑΉ����Ă���Apoke_wk��NULL�̂Ƃ������NULL
#ifdef DEF_MINIMUM_LOAD 
  u16                        diff_no;              // ����poke_wk��diff_info_list[diff_no]�ł���
                                                   // poke_wk�ƑΉ����Ă���Apoke_wk��NULL�̂Ƃ������DIFF_MAX
#endif
}
POKE_MCSS_WORK;

// �Ⴄ�p
typedef struct
{
  LOOKS_DIFF looks_diff;
  COLOR_DIFF color_diff;
  u16        other_diff;   // LOOKS_DIFF_ONE�̂Ƃ��A�g�p���Ȃ�
                           // LOOKS_DIFF_SEX�̂Ƃ��ASEX_MALE or SEX_FEMALE
                           // LOOKS_DIFF_FORM�̂Ƃ��Aformno

  // ZUKANSAVE_CheckPokeSeeForm�ɗ^��������
  u8         sex;          // PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN  // prog/include/poke_tool/poke_tool.h
  u8         rare;
  u16        form;
}
DIFF_INFO;


//-------------------------------------
/// PROC ���[�N
//=====================================
typedef struct
{
  // ���̂Ƃ��납��ؗp�������
  GFL_CLUNIT*                 clunit;
  GFL_FONT*                   font;
  //PRINT_QUE*                  print_que;  // ��p��print_que��p�ӂ��邱�Ƃɂ���

  // �����ō쐬�������
  MCSS_SYS_WORK*              mcss_sys_wk;
  POKE_MCSS_WORK              poke_mcss_wk[POKE_MAX];
#ifdef DEF_MCSS_TCBSYS
  void*                       mcss_tcbsys_wk;
  GFL_TCBSYS*                 mcss_tcbsys;
#endif
  // FRONT or BACK
  BOOL                        is_poke_front;  // TRUE�̂Ƃ�FRONT��\�����AFALSE�̂Ƃ�BACK��\����

  ARCHANDLE*                  pokeicon_handle;
  u32                         pokeicon_ncg[OBJ_SWAP_MAX];    // pokeicon_clwk[i]��NULL�łȂ��Ƃ�pokeicon_ncg[i]�͗L��
  u32                         pokeicon_ncl;
  u32                         pokeicon_nce;
  GFL_CLWK*                   pokeicon_clwk[OBJ_SWAP_MAX];   // �g���Ă��Ȃ��Ƃ���pokeicon_clwk[i]��NULL�ɂ��Ă���
  OBJ_SWAP                    pokeicon_swap_curr;            // ���\�����Ă���pokeicon_clwk��pokeicon_clwk[pokeicon_swap_curr]

  DIFF_INFO                   diff_info_list[DIFF_MAX];  // �����ɂ͍��̃t�H�������܂�ł���
  u16                         diff_num;  // �ő�DIFF_MAX
  u16                         diff_sugata_num;  // diff_num = diff_sugata_num + diff_irochigai_num
  u16                         diff_irochigai_num;
  u16                         diff_curr_no;  // diff_info_list[diff_curr_no]�����̃t�H����
  u16                         diff_comp_no;  // diff_info_list[diff_comp_no]����r�t�H����

  GFL_BMPWIN*                 text_bmpwin[TEXT_MAX];
  BOOL                        text_trans[TEXT_MAX];  // �]������K�v������ꍇTRUE
  PRINT_QUE*                  print_que[TEXT_MAX];
  GFL_MSGDATA*                text_msgdata;

  u16*                        next_form_pos_list;  // ���̃t�H�����f�[�^�̈ʒu���Q�Ƃł��郊�X�g
  GFL_MSGDATA*                form_msgdata;

  STRBUF*                     text_frequent_strbuf[TEXT_FREQUENT_MAX];

  GFL_ARCUTIL_TRANSINFO       panel_s_tinfo;

  ZKNDTL_COMMON_REAR_WORK*    rear_wk_m;
  ZKNDTL_COMMON_REAR_WORK*    rear_wk_s;

  // OBJ  // �O��{�^���A�Đ��{�^���A�X�N���[���o�[�A�o�[�J�[�\��  // �t�H�������̔w�ʃt�B�[���h
  u32                         obj_res[OBJ_RES_MAX];
  GFL_CLWK*                   obj_clwk[OBJ_MAX];
  BUTTON                      button[BUTTON_OBJ_MAX];
  BUTTON_OBJ                  push_button;  // ���݉�����Ă���{�^��������Ƃ�BUTTON_OBJ_NONE�ȊO�ƂȂ�B
  BOOL                        bar_cursor_move_by_key;    // TRUE�̂Ƃ��A�L�[�Ńo�[�J�[�\���𓮂�����
  BOOL                        bar_cursor_move_by_touch;  // TRUE�̂Ƃ��A�^�b�`�Ńo�[�J�[�\���𓮂�����

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // �t�F�[�h
  ZKNDTL_COMMON_FADE_WORK*    fade_wk_m;
  ZKNDTL_COMMON_FADE_WORK*    fade_wk_s;
  // �p���b�g�t�F�[�h
  ZKNDTL_COMMON_PF_WORK*      pf_wk;

  // �I������
  END_CMD                     end_cmd;

  // ���͉s��
  BOOL                        input_enable;  // ���͉̂Ƃ�TRUE

  // ���
  STATE                       state;

  // ����ւ�
  IREKAE_STATE                irekae_state;
  int                         irekae_theta;  // FX_SinIdx�̈���

  // �����o��
  OSHIDASHI_STATE             oshidashi_state;
  OSHIDASHI_DIRECT            oshidashi_direct;

  // �K�w�ύX  // TOP_TO_EXCHANGE or EXCHANGE_TO_TOP
  BOOL                        kaisou_curr_end;  // TRUE�̂Ƃ��Acurr�͏���̈ʒu�ɓ������Ă���
  BOOL                        kaisou_comp_end;  // TRUE�̂Ƃ��Acomp�͏���̈ʒu�ɓ������Ă���

#ifdef DEF_SCMD_CHANGE
  // �^�b�`�o�[�̍��E�A�C�R���Ńt�H������ύX����Ƃ������g�p����ϐ�
  BOOL                        change_form_by_cur_l_r_flag;  // TRUE�̂Ƃ��A�^�b�`�o�[�̍��E�A�C�R���Ńt�H������ύX����
                                                            // (bar_cursor_move_by_key�Ɠ����^�C�~���O�ŏ����������B1�t���[������TRUE�ɂȂ�B)

  // �^�b�`�o�[�̓���ւ��A�C�R����curr��comp�̃t�H���������ւ���Ƃ������g�p����ϐ�
  BOOL                        irekae_by_exchange_flag;         // TRUE�̂Ƃ��A�^�b�`�o�[�̓���ւ��A�C�R���Ńt�H���������ւ��Ă���
  BOOL                        irekae_by_exchange_poke_finish;  // �|�P�����̃t�H�����̓���ւ����������Ă�����TRUE
  BOOL                        irekae_by_exchange_icon_finish;  // �^�b�`�o�[�̓���ւ��A�C�R���̃A�j�����������Ă�����TRUE
#endif
}
ZUKAN_DETAIL_FORM_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Zukan_Detail_Form_VBlankFunc( GFL_TCB* tcb, void* wk );

// �e�L�X�g
static void Zukan_Detail_Form_CreateTextBase( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_DeleteTextBase( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_InitPrintQue( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_ExitPrintQue( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_MainPrintQue( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_MainText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn, u8 i );
static void Zukan_Detail_Form_WriteUpText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_WritePokeText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                u8 i, PRINT_QUE* print_que, GFL_BMPWIN* bmpwin, BOOL* trans_flag, u16 diff_no );
static void Zukan_Detail_Form_WritePokeCurrText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_WritePokeCompText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_ScrollPokeText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// OBJ  // �O��{�^���A�Đ��{�^���A�X�N���[���o�[�A�o�[�J�[�\��  // �t�H�������̔w�ʃt�B�[���h  // �g�b�v��ʂŃt�H������؂�ւ��邽�߂̖��
static void Zukan_Detail_Form_ObjBaseCreate( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_ObjBaseDelete( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

static BUTTON_OBJ Zukan_Detail_Form_ObjButtonCheckPush( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_ObjButtonArrowSetup( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_ObjButtonMain( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

static void Zukan_Detail_Form_ObjFieldSetup( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

static void Zukan_Detail_Form_ObjBarSetup( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_ObjBarMain( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static BOOL Zukan_Detail_Form_ObjBarMainTouch( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static BOOL Zukan_Detail_Form_ObjBarMainKey( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_ObjBarGetPosX( u16 num, u16 no, u8* a_pos_x_min, u8* a_pos_x_center, u8* a_pos_x_max );
static u16 Zukan_Detail_Form_ObjBarGetNo( u16 num, u8 x );
static u16 Zukan_Detail_Form_GetNoExceptOne( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                             u16 except_idx, u16 target_idx );
static u16 Zukan_Detail_Form_GetDiffInfoListIdx( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                                 u16 except_idx, u16 no );

// MCSS�|�P����
static void Zukan_Detail_Form_McssSysInit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_McssSysExit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static MCSS_WORK* PokeInit( MCSS_SYS_WORK* mcss_sys_wk,
                      int mons_no, int form_no, int sex, int rare, BOOL egg, int dir,
                      u32 personal_rnd );  // personal_rnd��mons_no==MONSNO_PATTIIRU�̂Ƃ��̂ݎg�p�����

static void PokeExit( MCSS_SYS_WORK* mcss_sys_wk, MCSS_WORK* poke_wk );
// �}�N��
#define BLOCK_POKE_EXIT(sys,wk)                     \
    {                                               \
      if( wk ) PokeExit( sys, wk );                 \
      wk = NULL;                                    \
    }
// NULL�������Y��Ȃ��悤�Ƀ}�N�����g���悤�ɂ��Ă���
static void PokeAdjustOfsPos( MCSS_WORK* poke_wk );
static void PokeAdjustOfsPosX( MCSS_WORK* poke_wk );
static void PokeGetCompareRelativePosition( MCSS_WORK* poke_wk, VecFx32* pos );

static void PokeMcssWorkInit( POKE_MCSS_WORK* poke_mcss_wk, HEAPID heap_id,
                              MCSS_SYS_WORK* mcss_sys_wk,
                              int mons_no, int form_no, int sex, int rare, BOOL egg, int dir,
                              u32 personal_rnd );  // personal_rnd��mons_no==MONSNO_PATTIIRU�̂Ƃ��̂ݎg�p�����
static void PokeMcssWorkExit( POKE_MCSS_WORK* poke_mcss_wk, MCSS_SYS_WORK* mcss_sys_wk );
static void PokeMcssCallBackDataInit( POKE_MCSS_CALL_BACK_DATA* poke_mcss_call_back_data,
                                      POKE_INDEX poke_idx, ZUKAN_DETAIL_FORM_WORK* work );

static void PokeMcssAnimeStart( POKE_MCSS_WORK* poke_mcss_wk );
static void PokeMcssAnimeMain( POKE_MCSS_WORK* poke_mcss_wk );
static void Zukan_Detail_Form_PokeMcssCallBackFunctor( u32 data, fx32 currentFrame );

static void Zukan_Detail_Form_FlipFrontBack( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �|�P�A�C�R��
static void PokeiconBaseInit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void PokeiconBaseExit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static GFL_CLWK* PokeiconInit( ARCHANDLE* pokeicon_handle, u32* ncg, u32 ncl, u32 nce, GFL_CLUNIT* clunit, HEAPID heap_id, u32 mons, u32 form_no, u32 sex, BOOL egg );
static void PokeiconExit( u32 ncg, GFL_CLWK* clwk );
// �}�N��
#define BLOCK_POKEICON_EXIT(ncg,clwk)                     \
    {                                                     \
      if( clwk ) PokeiconExit( ncg, clwk );               \
      clwk = NULL;                                        \
    }
// NULL�������Y��Ȃ��悤�Ƀ}�N�����g���悤�ɂ��Ă���

// �Ⴄ�p�����擾����
static void Zukan_Detail_Form_GetDiffInfoWithoutWork( ZUKAN_DETAIL_FORM_PARAM* param, ZKNDTL_COMMON_WORK* cmn,
                                                      DIFF_INFO*   a_diff_info_list,  // �v�f��a_diff_info_list[DIFF_MAX]��
                                                      u16*         a_diff_num,
                                                      u16*         a_diff_sugata_num,
                                                      u16*         a_diff_irochigai_num,
                                                      u16*         a_diff_curr_no,
                                                      u16*         a_diff_comp_no );
static void Zukan_Detail_Form_GetDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �Ⴄ�p��񂩂�MCSS�|�P�����𐶐�����
static void Zukan_Detail_Form_PokeInitFromDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                POKE_INDEX poke_f, POKE_INDEX poke_b, POKE_POS_INDEX pos, u16 diff_no );
// �Ⴄ�p��񂩂�|�P�A�C�R���𐶐�����
static void Zukan_Detail_Form_PokeiconInitFromDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                u16 diff_no );
// �Ⴄ�p��񂩂�|�P�����̎p�̐���STRBUF�𓾂�
static void Zukan_Detail_Form_GetPokeTextStrbufFromDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                STRBUF** a_strbuf0, STRBUF** a_strbuf1, BOOL* a_can_delete_strbuf0, BOOL* a_can_delete_strbuf1, u16 diff_no );
                // *a_can_delete_strbuf0��TRUE�̂Ƃ��Ăяo������a_strbuf0���폜���ׂ�

// �|�P�����ύX
static void Zukan_Detail_Form_ChangePoke( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

#ifdef DEF_MINIMUM_LOAD
// �|�P�����̑O���ύX�����ۂɁA�ύX���MCSS�����݂��Ȃ�������쐬����
static void Zukan_Detail_Form_MakePokeWhenFlipFrontBack( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
#endif

// ��r�t�H�����ύX
static void Zukan_Detail_Form_ChangeCompareForm( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �|�P�A�C�R���ύX
static void Zukan_Detail_Form_ChangePokeicon( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// ����
static void Zukan_Detail_Form_Input( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// ��Ԃ�J�ڂ�����
static void Zukan_Detail_Form_ChangeState( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                STATE state );

// ����ւ�
static void Zukan_Detail_Form_IrekaeMain( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �����o��
static void Zukan_Detail_Form_OshidashiChangeCompareForm( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_OshidashiSetPosCompareForm( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_OshidashiMain( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �K�w�ύX  // TOP_TO_EXCHANGE or EXCHANGE_TO_TOP
static void Zukan_Detail_Form_KaisouMain( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );

// �A���t�@�ݒ�
static void Zukan_Detail_Form_AlphaInit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_AlphaExit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn );


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Form_ProcInit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static ZKNDTL_PROC_RESULT Zukan_Detail_Form_ProcExit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static ZKNDTL_PROC_RESULT Zukan_Detail_Form_ProcMain( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );
static void Zukan_Detail_Form_CommandFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn, ZKNDTL_COMMAND cmd );
static void Zukan_Detail_Form_Draw3DFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn );

const ZKNDTL_PROC_DATA    ZUKAN_DETAIL_FORM_ProcData =
{
  Zukan_Detail_Form_ProcInit,
  Zukan_Detail_Form_ProcMain,
  Zukan_Detail_Form_ProcExit,
  Zukan_Detail_Form_CommandFunc,
  Zukan_Detail_Form_Draw3DFunc,
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
void             ZUKAN_DETAIL_FORM_InitParam(
                            ZUKAN_DETAIL_FORM_PARAM*  param,
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
static ZKNDTL_PROC_RESULT Zukan_Detail_Form_ProcInit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_FORM_PARAM*    param    = (ZUKAN_DETAIL_FORM_PARAM*)pwk;
  ZUKAN_DETAIL_FORM_WORK*     work;

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "Zukan_Detail_Form_ProcInit\n" );
  }
#endif

  // �q�[�v
  {
    work = ZKNDTL_PROC_AllocWork(proc, sizeof(ZUKAN_DETAIL_FORM_WORK), param->heap_id);
    GFL_STD_MemClear( work, sizeof(ZUKAN_DETAIL_FORM_WORK) ); 
  }

  // ���̂Ƃ��납��ؗp�������
  {
    ZUKAN_DETAIL_GRAPHIC_WORK* graphic = ZKNDTL_COMMON_GetGraphic(cmn);
    work->clunit      = ZUKAN_DETAIL_GRAPHIC_GetClunit(graphic);
    work->font        = ZKNDTL_COMMON_GetFont(cmn);
    //work->print_que   = ZKNDTL_COMMON_GetPrintQue(cmn);  // ��p��print_que��p�ӂ��邱�Ƃɂ���
  }

  // NULL������
  {
    u8 i;
    for( i=0; i<POKE_MAX; i++ )
    {
      work->poke_mcss_wk[i].poke_wk = NULL;
      work->poke_mcss_wk[i].poke_call_back_data = NULL;
#ifdef DEF_MINIMUM_LOAD 
      work->poke_mcss_wk[i].diff_no = DIFF_MAX;
#endif
    }

    for( i=0; i<OBJ_SWAP_MAX; i++ )
    {
      work->pokeicon_clwk[i] = NULL;  // NULL�ŏ�����
    }
    work->pokeicon_swap_curr = OBJ_SWAP_0;
  }
  // FRONT or BACK�̏�����
  work->is_poke_front = TRUE;

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Zukan_Detail_Form_VBlankFunc, work, 1 );

  // �e�L�X�g
  Zukan_Detail_Form_InitPrintQue( param, work, cmn );

  // �t�F�[�h
  {
    work->fade_wk_m = ZKNDTL_COMMON_FadeInit( param->heap_id );
    work->fade_wk_s = ZKNDTL_COMMON_FadeInit( param->heap_id );
   
    ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_m );
    ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_s );

    ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_M, work->fade_wk_m );
    ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_S, work->fade_wk_s );
  }
  // �p���b�g�t�F�[�h
  {
    work->pf_wk = ZKNDTL_COMMON_PfInit( param->heap_id );
  }

  // �I�����
  work->end_cmd = END_CMD_NONE;

  // ���͉s��
  work->input_enable = TRUE;

  // ���
  work->state = STATE_TOP;

#ifdef DEF_SCMD_CHANGE
  // �^�b�`�o�[�̍��E�A�C�R���Ńt�H������ύX����Ƃ������g�p����ϐ�
  work->change_form_by_cur_l_r_flag = FALSE;

  // �^�b�`�o�[�̓���ւ��A�C�R����curr��comp�̃t�H���������ւ���Ƃ������g�p����ϐ�
  work->irekae_by_exchange_flag        = FALSE;
  work->irekae_by_exchange_poke_finish = TRUE;
  work->irekae_by_exchange_icon_finish = TRUE;
#endif

  return ZKNDTL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Form_ProcExit( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_FORM_PARAM*    param    = (ZUKAN_DETAIL_FORM_PARAM*)pwk;
  ZUKAN_DETAIL_FORM_WORK*     work     = (ZUKAN_DETAIL_FORM_WORK*)mywk;
  
  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);

  // �Ŕw��
  ZKNDTL_COMMON_RearExit( work->rear_wk_s );
  ZKNDTL_COMMON_RearExit( work->rear_wk_m );

  // BG�p�l�� 
  ZKNDTL_COMMON_PanelDelete(
           BG_FRAME_S_PANEL,
           work->panel_s_tinfo );

  // OBJ  // �O��{�^���A�Đ��{�^���A�X�N���[���o�[�A�o�[�J�[�\��  // �t�H�������̔w�ʃt�B�[���h
  Zukan_Detail_Form_ObjBaseDelete( param, work, cmn );
  
  // �|�P�A�C�R��
  {
    u8 i;
    for( i=0; i<OBJ_SWAP_MAX; i++ )
    {
      // NULL�`�F�b�N�̓}�N�����ōs���Ă���
      BLOCK_POKEICON_EXIT( work->pokeicon_ncg[i], work->pokeicon_clwk[i] )
    }
  }
  
  // �|�P�A�C�R���̕s�ϕ�
  PokeiconBaseExit( param, work, cmn );

  // �e�L�X�g
  Zukan_Detail_Form_DeleteTextBase( param, work, cmn );

  // ���̉�ʂł̓^�b�`�o�[�̃v���C�I���e�B��3D�ʂ̃v���C�I���e�B��艺���Ă������̂ŁA���ɖ߂��Ă���
  ZUKAN_DETAIL_TOUCHBAR_SetBgPriority( touchbar, ZKNDTL_BG_FRAME_PRI_M_TOUCHBAR );

  // �p���b�g�t�F�[�h
  {
    ZKNDTL_COMMON_PfExit( work->pf_wk );
  }
  // �t�F�[�h
  {
    ZKNDTL_COMMON_FadeExit( work->fade_wk_s );
    ZKNDTL_COMMON_FadeExit( work->fade_wk_m );
  }

  // �e�L�X�g
  Zukan_Detail_Form_ExitPrintQue( param, work, cmn );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �q�[�v
  ZKNDTL_PROC_FreeWork( proc );

#ifdef DEBUG_KAWADA
  {
    OS_Printf( "Zukan_Detail_Form_ProcExit\n" );
  }
#endif

  return ZKNDTL_PROC_RES_FINISH;
}


//-------------------------------------
/// PROC �又��
//=====================================
static ZKNDTL_PROC_RESULT Zukan_Detail_Form_ProcMain( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_FORM_PARAM*    param    = (ZUKAN_DETAIL_FORM_PARAM*)pwk;
  ZUKAN_DETAIL_FORM_WORK*     work     = (ZUKAN_DETAIL_FORM_WORK*)mywk;

  ZUKAN_DETAIL_GRAPHIC_WORK*   graphic  = ZKNDTL_COMMON_GetGraphic(cmn);

  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  ZUKAN_DETAIL_HEADBAR_WORK*   headbar  = ZKNDTL_COMMON_GetHeadbar(cmn);
 
  switch(*seq)
  {
  case SEQ_START:
    {
      *seq = SEQ_FADE_CHANGE_BEFORE;

      // �؂�ւ�
      {
        // �O���t�B�b�N�X���[�h�ݒ�
        GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_3D );
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
        GFL_BG_SetPriority( BG_FRAME_M_POKE,  BG_FRAME_PRI_M_POKE );
        GFL_BG_SetPriority( BG_FRAME_M_TEXT,  BG_FRAME_PRI_M_TEXT );
        GFL_BG_SetPriority( BG_FRAME_M_REAR,  BG_FRAME_PRI_M_REAR );
        
        // �T�uBG
        GFL_BG_SetPriority( BG_FRAME_S_TEXT,  BG_FRAME_PRI_S_TEXT  );
        GFL_BG_SetPriority( BG_FRAME_S_REAR,  BG_FRAME_PRI_S_REAR  );
        GFL_BG_SetPriority( BG_FRAME_S_PANEL, BG_FRAME_PRI_S_PANEL );

        // ���̉�ʂł̓^�b�`�o�[�̃v���C�I���e�B��3D�ʂ̃v���C�I���e�B��艺���Ă���
        ZUKAN_DETAIL_TOUCHBAR_SetBgPriority( touchbar, BG_FRAME_PRI_M_TOUCHBAR );
      }

      // 3D
      ZUKAN_DETAIL_GRAPHIC_Init3D( graphic, param->heap_id );

      // MCSS�|�P����
      Zukan_Detail_Form_McssSysInit( param, work, cmn );

      // �e�L�X�g
      Zukan_Detail_Form_CreateTextBase( param, work, cmn );

      // OBJ  // �O��{�^���A�Đ��{�^���A�X�N���[���o�[�A�o�[�J�[�\��  // �t�H�������̔w�ʃt�B�[���h
      Zukan_Detail_Form_ObjBaseCreate( param, work, cmn );
     
      // �Ⴄ�p�����擾����
      Zukan_Detail_Form_GetDiffInfo( param, work, cmn );
    
      // �|�P�A�C�R���̕s�ϕ�
      PokeiconBaseInit( param, work, cmn );

      // �ŏ��̃|�P������\������
      Zukan_Detail_Form_ChangePoke( param, work, cmn );
      // �|�P�A�C�R���̐ݒ��ύX���Ă���
      GFL_CLACT_WK_SetObjMode( work->pokeicon_clwk[work->pokeicon_swap_curr], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�

      // �|�P�������\���ɂ����ɂ���
      MCSS_SetVanishFlag( work->poke_mcss_wk[POKE_CURR_F].poke_wk );
      MCSS_SetPaletteFade( work->poke_mcss_wk[POKE_CURR_F].poke_wk, 16, 16, 0, 0x0000 );
      if( work->poke_mcss_wk[POKE_COMP_F].poke_wk )
      {
        MCSS_SetVanishFlag( work->poke_mcss_wk[POKE_COMP_F].poke_wk );
        MCSS_SetPaletteFade( work->poke_mcss_wk[POKE_COMP_F].poke_wk, 16, 16, 0, 0x0000 );
      }

      // BG�p�l�� 
      work->panel_s_tinfo = ZKNDTL_COMMON_PanelCreate(
                              ZKNDTL_COMMON_PANEL_CREATE_TYPE_TRANS_PAL_CHARA,
                              param->heap_id,
                              BG_FRAME_S_PANEL,
                              BG_PAL_NUM_S_PANEL,
                              BG_PAL_POS_S_PANEL,
                              0,
                              ARCID_ZUKAN_GRA,
                              NARC_zukan_gra_info_info_bgu_NCLR,
                              NARC_zukan_gra_info_info_bgu_NCGR,
                              NARC_zukan_gra_info_formewin_bgu_NSCR,
                              0 );

      // �Ŕw��
      work->rear_wk_m = ZKNDTL_COMMON_RearInit( param->heap_id, ZKNDTL_COMMON_REAR_TYPE_FORM,
          BG_FRAME_M_REAR, BG_PAL_POS_M_REAR +0, BG_PAL_POS_M_REAR +1 );
      work->rear_wk_s = ZKNDTL_COMMON_RearInit( param->heap_id, ZKNDTL_COMMON_REAR_TYPE_FORM,
          BG_FRAME_S_REAR, BG_PAL_POS_S_REAR +0, BG_PAL_POS_S_REAR +1 );

      // �p���b�g�t�F�[�h
      {
        ZKNDTL_COMMON_PfSetPaletteDataFromVram( work->pf_wk );
        ZKNDTL_COMMON_PfSetBlackImmediately( work->pf_wk );
      }
    }
    break;
  case SEQ_FADE_CHANGE_BEFORE:
    {
      *seq = SEQ_PREPARE_0;

      // �t�F�[�h
      ZKNDTL_COMMON_FadeSetColorlessImmediately( ZKNDTL_COMMON_FADE_DISP_M, work->fade_wk_m );
      ZKNDTL_COMMON_FadeSetColorlessImmediately( ZKNDTL_COMMON_FADE_DISP_S, work->fade_wk_s );
     
      // ZKNDTL_COMMON_FadeSetColorlessImmediately��G2_SetBlendBrightnessExt��G2S_SetBlendBrightnessExt��ݒ肵�Ă���̂ŁA
      // ���̌��Zukan_Detail_Form_AlphaInit���ĂԂ���
      Zukan_Detail_Form_AlphaInit( param, work, cmn );
    }
    break;
  case SEQ_PREPARE_0:
    {
      // MCSS�|�P����
      // �|�P���������ɂȂ�̂�҂��߂ɁA1�t���[���]�v�ɑ҂�
      *seq = SEQ_PREPARE;
    }
    break;
  case SEQ_PREPARE:
    {
      // MCSS�|�P����
      // �|�P���������ɂȂ�̂�҂�
      BOOL poke_fade = TRUE;
      if( MCSS_CheckExecutePaletteFade( work->poke_mcss_wk[POKE_CURR_F].poke_wk ) )
      {
        poke_fade = FALSE;
      }
      if( work->poke_mcss_wk[POKE_COMP_F].poke_wk )
      {
        if( MCSS_CheckExecutePaletteFade( work->poke_mcss_wk[POKE_COMP_F].poke_wk ) )
        {
          poke_fade = FALSE;
        }
      }

      if( poke_fade )
      {
        *seq = SEQ_FADE_IN;

        //// �t�F�[�h
        //ZKNDTL_COMMON_FadeSetBlackToColorless( work->fade_wk_m );
        //ZKNDTL_COMMON_FadeSetBlackToColorless( work->fade_wk_s );

        // �p���b�g�t�F�[�h
        {
          ZKNDTL_COMMON_PfSetBlackToColorless( work->pf_wk );
        }

        // �^�b�`�o�[
        if( ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) != ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR )
        {
          ZUKAN_DETAIL_TOUCHBAR_SetType(
              touchbar,
              ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL,
              ZUKAN_DETAIL_TOUCHBAR_DISP_FORM,
              (ZKNDTL_COMMON_GetPokeNum(cmn)>1)?TRUE:FALSE );
          ZUKAN_DETAIL_TOUCHBAR_Appear( touchbar, ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE );
        }
        else
        {
          ZUKAN_DETAIL_TOUCHBAR_SetDispOfGeneral(
              touchbar,
              ZUKAN_DETAIL_TOUCHBAR_DISP_FORM );
        }
        ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, FALSE );  // ZUKAN_DETAIL_TOUCHBAR_SetType�̂Ƃ���Unlock��ԂȂ̂�
        {
          GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
          ZUKAN_DETAIL_TOUCHBAR_SetCheckFlipOfGeneral(
              touchbar,
              GAMEDATA_GetShortCut( gamedata, SHORTCUT_ID_ZUKAN_FORM ) );
        }
        // �^�C�g���o�[
        if( ZUKAN_DETAIL_HEADBAR_GetState( headbar ) != ZUKAN_DETAIL_HEADBAR_STATE_APPEAR )
        {
          ZUKAN_DETAIL_HEADBAR_SetType(
              headbar,
              ZUKAN_DETAIL_HEADBAR_TYPE_FORM );
          ZUKAN_DETAIL_HEADBAR_Appear( headbar );
        }

        // MCSS�|�P����
        // �|�P������\�����t�F�[�h�C��(�����J���[)
        MCSS_ResetVanishFlag( work->poke_mcss_wk[POKE_CURR_F].poke_wk );
        MCSS_SetPaletteFade( work->poke_mcss_wk[POKE_CURR_F].poke_wk, 16, 0, POKE_MCSS_FADE_WAIT, 0x0000 );
        if( work->poke_mcss_wk[POKE_COMP_F].poke_wk )
        {
          MCSS_ResetVanishFlag( work->poke_mcss_wk[POKE_COMP_F].poke_wk );
          MCSS_SetPaletteFade( work->poke_mcss_wk[POKE_COMP_F].poke_wk, 16, 0, POKE_MCSS_FADE_WAIT, 0x0000 );
        }
      }
    }
    break;
  case SEQ_FADE_IN:
    {
      // MCSS�|�P����
      // �|�P�������J���[�ɂȂ�̂�҂�
      BOOL poke_fade = TRUE;
      if( MCSS_CheckExecutePaletteFade( work->poke_mcss_wk[POKE_CURR_F].poke_wk ) )
      {
        poke_fade = FALSE;
      }
      if( work->poke_mcss_wk[POKE_COMP_F].poke_wk )
      {
        if( MCSS_CheckExecutePaletteFade( work->poke_mcss_wk[POKE_COMP_F].poke_wk ) )
        {
          poke_fade = FALSE;
        }
      }

      if( //   (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_m ))
          //&& (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_s ))
             (!ZKNDTL_COMMON_PfIsExecute( work->pf_wk ))
          && ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) == ZUKAN_DETAIL_TOUCHBAR_STATE_APPEAR
          && ZUKAN_DETAIL_HEADBAR_GetState( headbar ) == ZUKAN_DETAIL_HEADBAR_STATE_APPEAR
          && poke_fade )
      {
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
        ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, TRUE );  // ZUKAN_DETAIL_TOUCHBAR_SetType�̂Ƃ���Unlock��ԂȂ̂�

        //Zukan_Detail_Form_AlphaInit( param, work, cmn );

        *seq = SEQ_MAIN;
      }
    }
    break;
  case SEQ_MAIN:
    {
      if( work->end_cmd != END_CMD_NONE )
      {
        *seq = SEQ_POST;

        // �|�P�������J���[�ɂ���
        {
          u8 i;
          for( i=0; i<POKE_MAX; i++ )
          {
            if( work->poke_mcss_wk[i].poke_wk )
              MCSS_SetPaletteFade( work->poke_mcss_wk[i].poke_wk, 0, 0, 0, 0x0000 );
          }
        }
      }
      else
      {
        // ����
        Zukan_Detail_Form_Input( param, work, cmn );
      }
    }
    break;
  case SEQ_POST:
    {
      // MCSS�|�P����
      // �|�P�������J���[�ɂȂ�̂�҂�
      BOOL poke_fade = TRUE;
      {
        u8 i;
        for( i=0; i<POKE_MAX; i++ )
        {
          if( work->poke_mcss_wk[i].poke_wk )
          {
            if( MCSS_CheckExecutePaletteFade( work->poke_mcss_wk[i].poke_wk ) )
            {
              poke_fade = FALSE;
              break;
            }
          }
        }
      }

      if( poke_fade )
      {
        //Zukan_Detail_Form_AlphaExit( param, work, cmn );

        *seq = SEQ_FADE_OUT;

        //// �t�F�[�h
        //ZKNDTL_COMMON_FadeSetColorlessToBlack( work->fade_wk_m );
        //ZKNDTL_COMMON_FadeSetColorlessToBlack( work->fade_wk_s );
       
        // �p���b�g�t�F�[�h
        {
          ZKNDTL_COMMON_PfSetColorlessToBlack( work->pf_wk );
        }

        // �^�C�g���o�[
        ZUKAN_DETAIL_HEADBAR_Disappear( headbar );
        
        if( work->end_cmd == END_CMD_OUTSIDE )
        {
          // �^�b�`�o�[
          ZUKAN_DETAIL_TOUCHBAR_Disappear( touchbar, ZUKAN_DETAIL_TOUCHBAR_SPEED_OUTSIDE );
        }

        // MCSS�|�P����
        // �|�P������\�����t�F�[�h�A�E�g(�J���[����)
        {
          u8 i;
          for( i=0; i<POKE_MAX; i++ )
          {
            if( work->poke_mcss_wk[i].poke_wk )
            {
              MCSS_SetPaletteFade( work->poke_mcss_wk[i].poke_wk, 0, 16, POKE_MCSS_FADE_WAIT, 0x0000 );
            }
          }
        }
      }
    }
    break;
  case SEQ_FADE_OUT:
    {
      BOOL b_next_seq = FALSE;

      // MCSS�|�P����
      // �|�P���������ɂȂ�̂�҂�
      BOOL poke_fade = TRUE;
      {
        u8 i;
        for( i=0; i<POKE_MAX; i++ )
        {
          if( work->poke_mcss_wk[i].poke_wk )
          {
            if( MCSS_CheckExecutePaletteFade( work->poke_mcss_wk[i].poke_wk ) )
            {
              poke_fade = FALSE;
              break;
            }
          }
        }
      }

      if( //   (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_m ))
          //&& (!ZKNDTL_COMMON_FadeIsExecute( work->fade_wk_s ))
             (!ZKNDTL_COMMON_PfIsExecute( work->pf_wk ))
          && ZUKAN_DETAIL_HEADBAR_GetState( headbar ) == ZUKAN_DETAIL_HEADBAR_STATE_DISAPPEAR
          && poke_fade )
      {
        if( work->end_cmd == END_CMD_OUTSIDE )
        {
          if( ZUKAN_DETAIL_TOUCHBAR_GetState( touchbar ) == ZUKAN_DETAIL_TOUCHBAR_STATE_DISAPPEAR )
          {
            b_next_seq = TRUE;
          }
        }
        else
        {
          b_next_seq = TRUE;
        }
      }

      if( b_next_seq )
      {
        *seq = SEQ_FADE_CHANGE_AFTER;
      }
    }
    break;
  case SEQ_FADE_CHANGE_AFTER:
    {
      *seq = SEQ_END;

      // ZKNDTL_COMMON_FadeSetColorlessImmediately��G2_SetBlendBrightnessExt��G2S_SetBlendBrightnessExt��ݒ肵�Ă���̂ŁA
      // ���̑O��Zukan_Detail_Form_AlphaExit���ĂԂ���
      Zukan_Detail_Form_AlphaExit( param, work, cmn );
      
      // �t�F�[�h
      ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_M, work->fade_wk_m );
      ZKNDTL_COMMON_FadeSetBlackImmediately( ZKNDTL_COMMON_FADE_DISP_S, work->fade_wk_s );
    }
    break;
  case SEQ_END:
    {
      // MCSS�|�P����
      {
        u8 i;
        for( i=0; i<POKE_MAX; i++ )
        {
          //BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_mcss_wk[i].poke_wk )
          PokeMcssWorkExit( &work->poke_mcss_wk[i], work->mcss_sys_wk );
        }
      }
      Zukan_Detail_Form_McssSysExit( param, work, cmn );

      // 3D
      ZUKAN_DETAIL_GRAPHIC_Exit3D( graphic );

      // �؂�ւ�
      {
        // �O���t�B�b�N�X���[�h�ݒ�
        GX_SetGraphicsMode( GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BG0_AS_2D );
      }

      return ZKNDTL_PROC_RES_FINISH;
    }
    break;
  }

  // MCSS�|�P����
  if( work->mcss_sys_wk )
  {
    u8 i;
    for( i=0; i<POKE_MAX; i++ )
    {
      PokeMcssAnimeMain( &work->poke_mcss_wk[i] );  // ���̊֐�����NULL�`�F�b�N�͂��Ă���Ă���
    }

#ifdef DEF_MCSS_TCBSYS
    GFL_TCB_Main( work->mcss_tcbsys );  // DEF_MCSS_TCBSYS����`����Ă���DEF_MINIMUM_LOAD����`����Ă��Ȃ��Ƃ��A
                                        // GFL_TCB_Main, MCSS_Main�̏��ɂ��Ă����Ȃ��ƁA����������Y�ꂪ�o�Ă��܂��B
#endif
    
    MCSS_Main( work->mcss_sys_wk );
  } 

  if( *seq >= SEQ_PREPARE )
  {
    // �O��{�^���A�Đ��{�^��
    Zukan_Detail_Form_ObjButtonMain( param, work, cmn );
 
    // ����ւ� 
    Zukan_Detail_Form_IrekaeMain( param, work, cmn );  // Zukan_Detail_Form_IrekaeMain�ŃX�N���[���o�[���X�V���閽�߂��o���Ă���̂ŁA���Â�

    // �X�N���[���o�[
    Zukan_Detail_Form_ObjBarMain( param, work, cmn );  // �Â����K��Zukan_Detail_Form_IrekaeMain��Zukan_Detail_Form_ObjBarMain�̏��ŁB

    // �����o��
    Zukan_Detail_Form_OshidashiMain( param, work, cmn );

    // �K�w�ύX  // TOP_TO_EXCHANGE or EXCHANGE_TO_TOP
    Zukan_Detail_Form_KaisouMain( param, work, cmn );

    // �Ŕw��
    ZKNDTL_COMMON_RearMain( work->rear_wk_m );
    ZKNDTL_COMMON_RearMain( work->rear_wk_s );
  }

  // �t�F�[�h
  ZKNDTL_COMMON_FadeMain( work->fade_wk_m, work->fade_wk_s );
  // �p���b�g�t�F�[�h
  {
    ZKNDTL_COMMON_PfMain( work->pf_wk );
  }

  // �e�L�X�g
  Zukan_Detail_Form_MainPrintQue( param, work, cmn );

  return ZKNDTL_PROC_RES_CONTINUE;
}

//-------------------------------------
/// PROC ���ߏ���
//=====================================
static void Zukan_Detail_Form_CommandFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn, ZKNDTL_COMMAND cmd )
{
  if( mywk )
  {
    ZUKAN_DETAIL_FORM_PARAM*    param    = (ZUKAN_DETAIL_FORM_PARAM*)pwk;
    ZUKAN_DETAIL_FORM_WORK*     work     = (ZUKAN_DETAIL_FORM_WORK*)mywk;

    ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);

    BOOL b_valid_cmd = FALSE;  // cmd��ZKNDTL_CMD_NONE�ȊO��ZKNDTL_CMD_???�̂Ƃ�TRUE�BZKNDTL_CMD_NONE��ZKNDTL_SCMD_???�̂Ƃ�FALSE�B

    work->bar_cursor_move_by_key = FALSE;

#ifdef DEF_SCMD_CHANGE    
    work->change_form_by_cur_l_r_flag = FALSE;
#endif

    // ���͕s��
    switch( cmd )
    {
    case ZKNDTL_SCMD_CLOSE:
    case ZKNDTL_SCMD_RETURN:
    case ZKNDTL_SCMD_INFO:
    case ZKNDTL_SCMD_MAP:
    case ZKNDTL_SCMD_VOICE:
    case ZKNDTL_SCMD_CUR_D:
    case ZKNDTL_SCMD_CUR_U:
    case ZKNDTL_SCMD_CHECK:
    case ZKNDTL_SCMD_FORM_CUR_D:
    case ZKNDTL_SCMD_FORM_CUR_U:
    case ZKNDTL_SCMD_FORM_CUR_R:
    case ZKNDTL_SCMD_FORM_CUR_L:
    case ZKNDTL_SCMD_FORM_EXCHANGE:
      {
        work->input_enable = FALSE;
      }
      break;
    }
    // ���͉�
    switch( cmd )
    {
    case ZKNDTL_CMD_CLOSE:
    case ZKNDTL_CMD_RETURN:
    case ZKNDTL_CMD_INFO:
    case ZKNDTL_CMD_MAP:
    case ZKNDTL_CMD_VOICE:
    case ZKNDTL_CMD_CUR_D:
    case ZKNDTL_CMD_CUR_U:
    case ZKNDTL_CMD_CHECK:
    case ZKNDTL_CMD_FORM_CUR_D:
    case ZKNDTL_CMD_FORM_CUR_U:
    case ZKNDTL_CMD_FORM_CUR_R:
    case ZKNDTL_CMD_FORM_CUR_L:
    case ZKNDTL_CMD_FORM_EXCHANGE:
      {
        work->input_enable = TRUE;
        b_valid_cmd = TRUE;
      }
      break;
    }


#ifdef DEF_SCMD_CHANGE
    switch( cmd )
    {
    case ZKNDTL_SCMD_CUR_D:
    case ZKNDTL_SCMD_FORM_CUR_D:
      {
        if( work->state == STATE_TOP )
        {
          u16 monsno_curr;
          u16 monsno_go;
          monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
          ZKNDTL_COMMON_GoToNextPoke(cmn);
          monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
          if( monsno_curr != monsno_go )
          {
            Zukan_Detail_Form_ChangePoke(param, work, cmn);
          }
        }
        else if( work->state == STATE_EXCHANGE )
        {
          DIFF_INFO   diff_info_list[DIFF_MAX];
          u16         diff_num;
          u16         diff_sugata_num;
          u16         diff_irochigai_num;
          u16         diff_curr_no;
          u16         diff_comp_no;
          u16 monsno_curr;
          u16 monsno_go;
          monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
          ZKNDTL_COMMON_GoToNextPoke(cmn);
          monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
          while( monsno_curr != monsno_go )
          {
            Zukan_Detail_Form_GetDiffInfoWithoutWork( param, cmn,
                                                      diff_info_list,
                                                      &diff_num,
                                                      &diff_sugata_num,
                                                      &diff_irochigai_num,
                                                      &diff_curr_no,
                                                      &diff_comp_no );
            if( diff_num >= 2 )
            {
              break;
            }
            else
            {
              ZKNDTL_COMMON_GoToNextPoke(cmn);
              monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
            }
          }
          if( monsno_curr != monsno_go )
          {
            Zukan_Detail_Form_ChangePoke(param, work, cmn);
            if( diff_num >= 3 )
            {
              ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR( touchbar, TRUE );
            }
            else
            {
              ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR( touchbar, FALSE );
            }
          }
        }
      }
      break;
    case ZKNDTL_SCMD_CUR_U:
    case ZKNDTL_SCMD_FORM_CUR_U:
      {
        if( work->state == STATE_TOP )
        {
          u16 monsno_curr;
          u16 monsno_go;
          monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
          ZKNDTL_COMMON_GoToPrevPoke(cmn);
          monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
          if( monsno_curr != monsno_go )
          {
            Zukan_Detail_Form_ChangePoke(param, work, cmn);
          }
        }
        else if( work->state == STATE_EXCHANGE )
        {
          DIFF_INFO   diff_info_list[DIFF_MAX];
          u16         diff_num;
          u16         diff_sugata_num;
          u16         diff_irochigai_num;
          u16         diff_curr_no;
          u16         diff_comp_no;
          u16 monsno_curr;
          u16 monsno_go;
          monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
          ZKNDTL_COMMON_GoToPrevPoke(cmn);
          monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
          while( monsno_curr != monsno_go )
          {
            Zukan_Detail_Form_GetDiffInfoWithoutWork( param, cmn,
                                                      diff_info_list,
                                                      &diff_num,
                                                      &diff_sugata_num,
                                                      &diff_irochigai_num,
                                                      &diff_curr_no,
                                                      &diff_comp_no );
            if( diff_num >= 2 )
            {
              break;
            }
            else
            {
              ZKNDTL_COMMON_GoToPrevPoke(cmn);
              monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
            }
          }
          if( monsno_curr != monsno_go )
          {
            Zukan_Detail_Form_ChangePoke(param, work, cmn);
            if( diff_num >= 3 )
            {
              ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR( touchbar, TRUE );
            }
            else
            {
              ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR( touchbar, FALSE );
            }
          }
        }
      }
      break;
    case ZKNDTL_SCMD_FORM_CUR_R:
      {
        if( work->diff_num >= 3 )
        {
          work->diff_comp_no++;
          work->diff_comp_no %= work->diff_num;
          if( work->diff_curr_no == work->diff_comp_no )
          {
            work->diff_comp_no++;
            work->diff_comp_no %= work->diff_num;
          }

          Zukan_Detail_Form_ChangeCompareForm( param, work, cmn );
          work->bar_cursor_move_by_key = TRUE;
          work->change_form_by_cur_l_r_flag = TRUE;
        }
      }
      break;
    case ZKNDTL_SCMD_FORM_CUR_L:
      {
        if( work->diff_num >= 3 )
        {
          work->diff_comp_no += work->diff_num -1;
          work->diff_comp_no %= work->diff_num;
          if( work->diff_curr_no == work->diff_comp_no )
          {
            work->diff_comp_no += work->diff_num -1;
            work->diff_comp_no %= work->diff_num;
          }

          Zukan_Detail_Form_ChangeCompareForm( param, work, cmn );
          work->bar_cursor_move_by_key = TRUE;
          work->change_form_by_cur_l_r_flag = TRUE;
        }
      }
      break;


    case ZKNDTL_SCMD_FORM_EXCHANGE:
      {
        if( work->diff_num >= 2 )
        {
          Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_EXCHANGE_IREKAE );

          // �^�b�`�o�[�̓���ւ��A�C�R����curr��comp�̃t�H���������ւ���Ƃ������g�p����ϐ�
          work->irekae_by_exchange_flag        = TRUE;
          work->irekae_by_exchange_poke_finish = FALSE;
          work->irekae_by_exchange_icon_finish = FALSE;
        }
        else
        {
          // �^�b�`�o�[�̓���ւ��A�C�R����curr��comp�̃t�H���������ւ���Ƃ������g�p����ϐ�
          work->irekae_by_exchange_flag        = TRUE;
          work->irekae_by_exchange_poke_finish = TRUE;
          work->irekae_by_exchange_icon_finish = FALSE;
        }
      }
      break;


    }
#endif


    // �R�}���h
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
    case ZKNDTL_CMD_MAP:
    case ZKNDTL_CMD_VOICE:
      {
        work->end_cmd = END_CMD_INSIDE;
      }
      break;
    case ZKNDTL_CMD_CUR_D:
    case ZKNDTL_CMD_FORM_CUR_D:
      {
#ifndef DEF_SCMD_CHANGE
        if( work->state == STATE_TOP )
        {
          u16 monsno_curr;
          u16 monsno_go;
          monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
          ZKNDTL_COMMON_GoToNextPoke(cmn);
          monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
          if( monsno_curr != monsno_go )
          {
            Zukan_Detail_Form_ChangePoke(param, work, cmn);
          }
        }
        else if( work->state == STATE_EXCHANGE )
        {
          DIFF_INFO   diff_info_list[DIFF_MAX];
          u16         diff_num;
          u16         diff_sugata_num;
          u16         diff_irochigai_num;
          u16         diff_curr_no;
          u16         diff_comp_no;
          u16 monsno_curr;
          u16 monsno_go;
          monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
          ZKNDTL_COMMON_GoToNextPoke(cmn);
          monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
          while( monsno_curr != monsno_go )
          {
            Zukan_Detail_Form_GetDiffInfoWithoutWork( param, cmn,
                                                      diff_info_list,
                                                      &diff_num,
                                                      &diff_sugata_num,
                                                      &diff_irochigai_num,
                                                      &diff_curr_no,
                                                      &diff_comp_no );
            if( diff_num >= 2 )
            {
              break;
            }
            else
            {
              ZKNDTL_COMMON_GoToNextPoke(cmn);
              monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
            }
          }
          if( monsno_curr != monsno_go )
          {
            Zukan_Detail_Form_ChangePoke(param, work, cmn);
            if( diff_num >= 3 )
            {
              ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR( touchbar, TRUE );
            }
            else
            {
              ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR( touchbar, FALSE );
            }
          }
        }
#endif

        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_CUR_U:
    case ZKNDTL_CMD_FORM_CUR_U:
      {
#ifndef DEF_SCMD_CHANGE
        if( work->state == STATE_TOP )
        {
          u16 monsno_curr;
          u16 monsno_go;
          monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
          ZKNDTL_COMMON_GoToPrevPoke(cmn);
          monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
          if( monsno_curr != monsno_go )
          {
            Zukan_Detail_Form_ChangePoke(param, work, cmn);
          }
        }
        else if( work->state == STATE_EXCHANGE )
        {
          DIFF_INFO   diff_info_list[DIFF_MAX];
          u16         diff_num;
          u16         diff_sugata_num;
          u16         diff_irochigai_num;
          u16         diff_curr_no;
          u16         diff_comp_no;
          u16 monsno_curr;
          u16 monsno_go;
          monsno_curr = ZKNDTL_COMMON_GetCurrPoke(cmn);
          ZKNDTL_COMMON_GoToPrevPoke(cmn);
          monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
          while( monsno_curr != monsno_go )
          {
            Zukan_Detail_Form_GetDiffInfoWithoutWork( param, cmn,
                                                      diff_info_list,
                                                      &diff_num,
                                                      &diff_sugata_num,
                                                      &diff_irochigai_num,
                                                      &diff_curr_no,
                                                      &diff_comp_no );
            if( diff_num >= 2 )
            {
              break;
            }
            else
            {
              ZKNDTL_COMMON_GoToPrevPoke(cmn);
              monsno_go = ZKNDTL_COMMON_GetCurrPoke(cmn);
            }
          }
          if( monsno_curr != monsno_go )
          {
            Zukan_Detail_Form_ChangePoke(param, work, cmn);
            if( diff_num >= 3 )
            {
              ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR( touchbar, TRUE );
            }
            else
            {
              ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR( touchbar, FALSE );
            }
          }
        }
#endif
        
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_CHECK:
      {
        GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
        GAMEDATA_SetShortCut(
            gamedata,
            SHORTCUT_ID_ZUKAN_FORM,
            ZUKAN_DETAIL_TOUCHBAR_GetCheckFlipOfGeneral( touchbar ) );
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_FORM_CUR_R:
      {
#ifndef DEF_SCMD_CHANGE
        if( work->diff_num >= 3 )
        {
          work->diff_comp_no++;
          work->diff_comp_no %= work->diff_num;
          if( work->diff_curr_no == work->diff_comp_no )
          {
            work->diff_comp_no++;
            work->diff_comp_no %= work->diff_num;
          }

          Zukan_Detail_Form_ChangeCompareForm( param, work, cmn );
          work->bar_cursor_move_by_key = TRUE;
        }
#endif
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_FORM_CUR_L:
      {
#ifndef DEF_SCMD_CHANGE
        if( work->diff_num >= 3 )
        {
          work->diff_comp_no += work->diff_num -1;
          work->diff_comp_no %= work->diff_num;
          if( work->diff_curr_no == work->diff_comp_no )
          {
            work->diff_comp_no += work->diff_num -1;
            work->diff_comp_no %= work->diff_num;
          }

          Zukan_Detail_Form_ChangeCompareForm( param, work, cmn );
          work->bar_cursor_move_by_key = TRUE;
        }
#endif
        ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
      }
      break;
    case ZKNDTL_CMD_FORM_RETURN:
      {
        //Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_TOP );
        if( work->state == STATE_EXCHANGE )
        {
          Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_EXCHANGE_TO_TOP );
        }
      }
      break;
    case ZKNDTL_CMD_FORM_EXCHANGE:
      {
#ifndef DEF_SCMD_CHANGE
        if( work->diff_num >= 2 )
        {
          Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_EXCHANGE_IREKAE );
        }
        else
        {
          ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
        }
#endif

#ifdef DEF_SCMD_CHANGE
        if( work->irekae_by_exchange_flag )
        {
          work->irekae_by_exchange_icon_finish = TRUE;
          if( work->irekae_by_exchange_poke_finish )
          {
            work->irekae_by_exchange_flag = FALSE;
            ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
          }
        }
#endif
      }
      break;
    default:
      {
        if( b_valid_cmd )
        {
          ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
        }
      }
      break;
    }
  }
}

//-------------------------------------
/// PROC 3D�`��
//=====================================
static void Zukan_Detail_Form_Draw3DFunc( ZKNDTL_PROC* proc, int* seq, void* pwk, void* mywk, ZKNDTL_COMMON_WORK* cmn )
{
  if( mywk )
  {
    ZUKAN_DETAIL_FORM_PARAM*    param    = (ZUKAN_DETAIL_FORM_PARAM*)pwk;
    ZUKAN_DETAIL_FORM_WORK*     work     = (ZUKAN_DETAIL_FORM_WORK*)mywk;

    if( work->mcss_sys_wk )
      MCSS_Draw( work->mcss_sys_wk );
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
static void Zukan_Detail_Form_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  ZUKAN_DETAIL_FORM_WORK*     work     = (ZUKAN_DETAIL_FORM_WORK*)wk;

  // �p���b�g�t�F�[�h
  {
    ZKNDTL_COMMON_PfTrans( work->pf_wk );
  }
}

//-------------------------------------
/// �Ⴄ�p��񂩂�|�P�����̎p�̐���STRBUF�𓾂�
    // �Ăяo�����ŉ�����邱��(NULL�̂Ƃ��͂Ȃ�)
//=====================================
static void Zukan_Detail_Form_GetPokeTextStrbufFromDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                STRBUF** a_strbuf0, STRBUF** a_strbuf1, BOOL* a_can_delete_strbuf0, BOOL* a_can_delete_strbuf1, u16 diff_no )
                // *a_can_delete_strbuf0��TRUE�̂Ƃ��Ăяo������a_strbuf0���폜���ׂ�
{
  STRBUF* strbuf0 = NULL;
  STRBUF* strbuf1 = NULL;
  BOOL    can_delete_strbuf0 = FALSE;
  BOOL    can_delete_strbuf1 = FALSE;
  
  u16 monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);

  switch( work->diff_info_list[diff_no].looks_diff )
  {
  case LOOKS_DIFF_ONE:
    {
      WORDSET* wordset;
      STRBUF* src_strbuf;
      wordset = WORDSET_Create( param->heap_id );
      src_strbuf = work->text_frequent_strbuf[TEXT_FREQUENT_POKE_SUGATA];//GFL_MSG_CreateString( work->text_msgdata, ZKN_FORMNAME_10 );
      strbuf0 = GFL_STR_CreateBuffer( STRBUF_POKENAME_LENGTH, param->heap_id );
      WORDSET_RegisterPokeMonsNameNo( wordset, 0, monsno );
      WORDSET_ExpandStr( wordset, strbuf0, src_strbuf );
      //GFL_STR_DeleteBuffer( src_strbuf );
      WORDSET_Delete( wordset );
      can_delete_strbuf0 = TRUE;
    }
    break;
  case LOOKS_DIFF_SEX:
    {
      switch( work->diff_info_list[diff_no].other_diff )
      {
      case SEX_MALE:
        {
          strbuf0 = work->text_frequent_strbuf[TEXT_FREQUENT_MALE_SUGATA];
          can_delete_strbuf0 = FALSE;
        }
        break;
      case SEX_FEMALE:
        {
          strbuf0 = work->text_frequent_strbuf[TEXT_FREQUENT_FEMALE_SUGATA];
          can_delete_strbuf0 = FALSE;
        }
        break;
      }
    }
    break;
  case LOOKS_DIFF_FORM:
    {
      u16 form_no = work->diff_info_list[diff_no].other_diff;

      // ���̃t�H�����f�[�^�̈ʒu���Q�Ƃł��郊�X�g�𗘗p���āA�t�H�����f�[�^�̈ʒu�𓾂�
      u16 pos = monsno;
      {
        u16 formno_count = 0;
        while( formno_count != form_no )
        {
          pos = work->next_form_pos_list[pos];
          if( pos == 0 ) break;
          formno_count++;
        }
        GF_ASSERT_MSG( pos > 0, "ZUKAN_DETAIL_FORM : �t�H�����ԍ����ُ�ł��B\n" );
      }

      strbuf0 = GFL_MSG_CreateString( work->form_msgdata, pos );
      can_delete_strbuf0 = TRUE;
    }
    break;
  }

  switch( work->diff_info_list[diff_no].color_diff )
  {
  case COLOR_DIFF_NONE:
    {
      // �������Ȃ�
    }
    break;
  case COLOR_DIFF_NORMAL:
    {
      // �������Ȃ�
    }
    break;
  case COLOR_DIFF_SPECIAL:
    {
      strbuf1 = work->text_frequent_strbuf[TEXT_FREQUENT_NOTE_IROCHIGAI];
      can_delete_strbuf1 = FALSE;
    }
    break;
  }

  *a_strbuf0 = strbuf0;
  *a_strbuf1 = strbuf1;
  *a_can_delete_strbuf0 = can_delete_strbuf0;
  *a_can_delete_strbuf1 = can_delete_strbuf1;
}

//-------------------------------------
/// �e�L�X�g
//=====================================
static void Zukan_Detail_Form_CreateTextBase( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 bmpwin_setup[TEXT_MAX][7] =
  {
    { BG_FRAME_S_TEXT, 19, 16,  8,  2, BG_PAL_POS_S_TEXT, GFL_BMP_CHRAREA_GET_B },
    { BG_FRAME_S_TEXT, 27, 19,  2,  4, BG_PAL_POS_S_TEXT, GFL_BMP_CHRAREA_GET_B },
    { BG_FRAME_S_TEXT,  3, 19, 23,  4, BG_PAL_POS_S_TEXT, GFL_BMP_CHRAREA_GET_B },
    { BG_FRAME_M_TEXT,  0,  1, 16,  4, BG_PAL_POS_M_TEXT, GFL_BMP_CHRAREA_GET_B },
    { BG_FRAME_M_TEXT, 16,  1, 16,  4, BG_PAL_POS_M_TEXT, GFL_BMP_CHRAREA_GET_B },
  };

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
       BG_PAL_POS_S_TEXT * 0x20,
       BG_PAL_NUM_S_TEXT * 0x20,
       param->heap_id );

  // �r�b�g�}�b�v�E�B���h�E
  {
    u8 i;
    for( i=0; i<TEXT_MAX; i++ )
    {
      work->text_bmpwin[i] = GFL_BMPWIN_Create( bmpwin_setup[i][0],
                                         bmpwin_setup[i][1], bmpwin_setup[i][2], bmpwin_setup[i][3], bmpwin_setup[i][4],
                                         bmpwin_setup[i][5],
                                         bmpwin_setup[i][6] );

      // �N���A
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->text_bmpwin[i] ), 0 );
      GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[i] );
    }
  }

  // ���b�Z�[�W
  work->text_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
                           ARCID_MESSAGE, NARC_message_zkn_dat,
                           param->heap_id );

  work->form_msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
                           ARCID_MESSAGE, NARC_message_zkn_form_dat,
                           param->heap_id );

  // ���̃t�H�����f�[�^�̈ʒu���Q�Ƃł��郊�X�g
  {
    u32 size;
    u16 next_form_pos_num;

    work->next_form_pos_list = GFL_ARC_UTIL_LoadEx( ARCID_ZUKAN_DATA, NARC_zukan_data_zkn_next_form_pos_dat, FALSE, param->heap_id, &size );
    next_form_pos_num = (u16)( size / sizeof(u16) );

    GF_ASSERT_MSG( next_form_pos_num > MONSNO_END+1+1, "ZUKAN_DETAIL_FORM : ���̃t�H�����̃f�[�^�̈ʒu���Q�Ƃł����̌�������܂���B\n" );
  }

  // �ύX����邱�Ƃ̂Ȃ��Œ�e�L�X�g TEXT_UP_LABEL
  {
    STRBUF* strbuf;
    GFL_BMP_DATA*	bmp_data = GFL_BMPWIN_GetBmp( work->text_bmpwin[TEXT_UP_LABEL] );
    
    strbuf = GFL_MSG_CreateString( work->text_msgdata, ZKN_FORME_TEXT_01 );
    PRINTSYS_PrintQueColor( work->print_que[TEXT_UP_LABEL], bmp_data,
                            0, text_up_label_num_y[0], strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
    GFL_STR_DeleteBuffer( strbuf );

    strbuf = GFL_MSG_CreateString( work->text_msgdata, ZKN_FORME_TEXT_02 );
    PRINTSYS_PrintQueColor( work->print_que[TEXT_UP_LABEL], bmp_data,
                            0, text_up_label_num_y[1], strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
    GFL_STR_DeleteBuffer( strbuf );

    work->text_trans[TEXT_UP_LABEL] = TRUE;

    // ���ɍς�ł��邩������Ȃ��̂ŁAMain��1�x�Ă�ł���
    Zukan_Detail_Form_MainText( param, work, cmn, TEXT_UP_LABEL );
  }

  // �悭�g���e�L�X�g
  {
    u8 i;
    for( i=0; i<TEXT_FREQUENT_MAX; i++ )
    {
      work->text_frequent_strbuf[i] = GFL_MSG_CreateString( work->text_msgdata, text_frequent_str_id[i] );
    }
  }
}
static void Zukan_Detail_Form_DeleteTextBase( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;
  for( i=0; i<TEXT_MAX; i++ )
  {
    work->text_trans[i] = FALSE;
    PRINTSYS_QUE_Clear( work->print_que[i] );
    
    GFL_BMPWIN_Delete( work->text_bmpwin[i] );
  }

  // �悭�g���e�L�X�g
  {
    u8 i;
    for( i=0; i<TEXT_FREQUENT_MAX; i++ )
    {
      GFL_STR_DeleteBuffer( work->text_frequent_strbuf[i] );
    }
  }

  GFL_MSG_Delete( work->text_msgdata );

  GFL_MSG_Delete( work->form_msgdata );

  // ���̃t�H�����f�[�^�̈ʒu���Q�Ƃł��郊�X�g
  {
    GFL_HEAP_FreeMemory( work->next_form_pos_list );
  }
}
static void Zukan_Detail_Form_InitPrintQue( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;
  for( i=0; i<TEXT_MAX; i++ )
  {
    work->print_que[i] = PRINTSYS_QUE_Create( param->heap_id );

    work->text_trans[i] = FALSE;
  }
}
static void Zukan_Detail_Form_ExitPrintQue( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;
  for( i=0; i<TEXT_MAX; i++ )
  {
    work->text_trans[i] = FALSE;
    
    PRINTSYS_QUE_Clear( work->print_que[i] );
    PRINTSYS_QUE_Delete( work->print_que[i] );
  }
}
static void Zukan_Detail_Form_MainPrintQue( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;
  for( i=0; i<TEXT_MAX; i++ )
  {
    PRINTSYS_QUE_Main( work->print_que[i] );

    Zukan_Detail_Form_MainText( param, work, cmn, i );
  }
}
static void Zukan_Detail_Form_MainText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn, u8 i )
{
  if( work->text_trans[i] )
  {
    if( !PRINTSYS_QUE_IsExistTarget( work->print_que[i], GFL_BMPWIN_GetBmp(work->text_bmpwin[i]) ) )
    {
      GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[i] );
      work->text_trans[i] = FALSE;
    }
  }
}
static void Zukan_Detail_Form_WriteUpText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u8 i;

  // �O��̃e�L�X�g���N���A
  {
    for( i=TEXT_POKENAME; i<=TEXT_UP_NUM; i++ )
    {
      work->text_trans[i] = FALSE;
      PRINTSYS_QUE_Clear( work->print_que[i] );
      
      // �N���A
      GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->text_bmpwin[i] ), 0 );
      //GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[i] );  // ������̂ŃR�����g�A�E�g
    }
  }

  // ����̃e�L�X�g��`��
  {
    u16 monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);
    
    {
      GFL_BMP_DATA*	bmp_data  = GFL_BMPWIN_GetBmp( work->text_bmpwin[TEXT_POKENAME] );

      STRBUF* strbuf = GFL_MSG_CreateString( GlobalMsg_PokeName, monsno );

      u16 str_width = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
      u16 bmp_width = GFL_BMP_GetSizeX( bmp_data );
      u16 x = ( bmp_width - str_width ) / 2;

/*
      u16 str_height = (u16)( PRINTSYS_GetStrHeight( strbuf, work->font ) );
      u16 bmp_height = GFL_BMP_GetSizeY( bmp_data );
      u16 y = ( bmp_height - str_height ) / 2;
*/
      u16 y = 1;  // �v�Z�ŏo�����ʒu�͒��S����Ɍ�����̂�

      PRINTSYS_PrintQueColor( work->print_que[TEXT_POKENAME], bmp_data,
                              x, y, strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
      work->text_trans[TEXT_POKENAME] = TRUE;

      GFL_STR_DeleteBuffer( strbuf );
    }

    {
      WORDSET* wordset;
      STRBUF* src_strbuf;
      STRBUF* strbuf;
      GFL_BMP_DATA*	bmp_data = GFL_BMPWIN_GetBmp( work->text_bmpwin[TEXT_UP_NUM] );

      wordset = WORDSET_Create( param->heap_id );
      src_strbuf = work->text_frequent_strbuf[TEXT_FREQUENT_SUGATA_NUM];//GFL_MSG_CreateString( work->text_msgdata, ZKN_FORME_TEXT_03 );
      strbuf = GFL_STR_CreateBuffer( STRBUF_NUM_LENGTH, param->heap_id );
      WORDSET_RegisterNumber( wordset, 0, work->diff_sugata_num, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
      WORDSET_ExpandStr( wordset, strbuf, src_strbuf );
      PRINTSYS_PrintQueColor( work->print_que[TEXT_UP_NUM], bmp_data,
                              0, text_up_label_num_y[0], strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
      GFL_STR_DeleteBuffer( strbuf );
      //GFL_STR_DeleteBuffer( src_strbuf );
      WORDSET_Delete( wordset );

      wordset = WORDSET_Create( param->heap_id );
      src_strbuf = work->text_frequent_strbuf[TEXT_FREQUENT_IROCHIGAI_NUM];//GFL_MSG_CreateString( work->text_msgdata, ZKN_FORME_TEXT_04 );
      strbuf = GFL_STR_CreateBuffer( STRBUF_NUM_LENGTH, param->heap_id );
      WORDSET_RegisterNumber( wordset, 0, work->diff_irochigai_num, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
      WORDSET_ExpandStr( wordset, strbuf, src_strbuf );
      PRINTSYS_PrintQueColor( work->print_que[TEXT_UP_NUM], bmp_data,
                              0, text_up_label_num_y[1], strbuf, work->font, PRINTSYS_LSB_Make(15,2,0) );
      GFL_STR_DeleteBuffer( strbuf );
      //GFL_STR_DeleteBuffer( src_strbuf );
      WORDSET_Delete( wordset );

      work->text_trans[TEXT_UP_NUM] = TRUE;
    }

    for( i=TEXT_POKENAME; i<=TEXT_UP_NUM; i++ )
    {
      // ���ɍς�ł��邩������Ȃ��̂ŁAMain��1�x�Ă�ł���
      Zukan_Detail_Form_MainText( param, work, cmn, i );
    }
  }
}

static void Zukan_Detail_Form_WritePokeText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                u8 i, PRINT_QUE* print_que, GFL_BMPWIN* bmpwin, BOOL* trans_flag, u16 diff_no )
{
  GFL_BMP_DATA*	bmp_data = GFL_BMPWIN_GetBmp( bmpwin );
  u16 bmp_width = GFL_BMP_GetSizeX( bmp_data );

  // ����̃e�L�X�g��`��
  {
    STRBUF* strbuf0;
    STRBUF* strbuf1;
    BOOL    can_delete_strbuf0;
    BOOL    can_delete_strbuf1;

    u16 str_width;
    u16 x;
    u16 y[2];

    Zukan_Detail_Form_GetPokeTextStrbufFromDiffInfo( param, work, cmn,
         &strbuf0, &strbuf1, &can_delete_strbuf0, &can_delete_strbuf1, diff_no );

    if( strbuf1 )
    {
      y[0] = TEXT_POKE_POS_Y_2_LINE_0;
      y[1] = TEXT_POKE_POS_Y_2_LINE_1;
    }
    else
    {
      y[0] = TEXT_POKE_POS_Y_1_LINE;
    }

    str_width = (u16)( PRINTSYS_GetStrWidth( strbuf0, work->font, 0 ) );
    x = ( bmp_width - str_width ) / 2;
    PRINTSYS_PrintQueColor( print_que, bmp_data,
                            x, y[0], strbuf0, work->font, PRINTSYS_LSB_Make(15,2,0) );

    if( strbuf1 )
    {
      str_width = (u16)( PRINTSYS_GetStrWidth( strbuf1, work->font, 0 ) );
      x = ( bmp_width - str_width ) / 2;
      PRINTSYS_PrintQueColor( print_que, bmp_data,
                              x, y[1], strbuf1, work->font, PRINTSYS_LSB_Make(15,2,0) );
    }

    if( strbuf0 && can_delete_strbuf0 ) GFL_STR_DeleteBuffer( strbuf0 );
    if( strbuf1 && can_delete_strbuf1 ) GFL_STR_DeleteBuffer( strbuf1 );

    *trans_flag = TRUE;

    // ���ɍς�ł��邩������Ȃ��̂ŁAMain��1�x�Ă�ł���
    Zukan_Detail_Form_MainText( param, work, cmn, i );
  }
}
static void Zukan_Detail_Form_WritePokeCurrText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �O��̃e�L�X�g���N���A
  {
    work->text_trans[TEXT_POKE_CURR] = FALSE;
    PRINTSYS_QUE_Clear( work->print_que[TEXT_POKE_CURR] );

    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->text_bmpwin[TEXT_POKE_CURR] ), 0 );
    //GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_POKE_CURR] );  // ������̂ŃR�����g�A�E�g
  }

  // ����̃e�L�X�g��`��
  Zukan_Detail_Form_WritePokeText( param, work, cmn,
               TEXT_POKE_CURR, work->print_que[TEXT_POKE_CURR], work->text_bmpwin[TEXT_POKE_CURR], &(work->text_trans[TEXT_POKE_CURR]), work->diff_curr_no );
 }
static void Zukan_Detail_Form_WritePokeCompText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �O��̃e�L�X�g���N���A
  {
    work->text_trans[TEXT_POKE_COMP] = FALSE;
    PRINTSYS_QUE_Clear( work->print_que[TEXT_POKE_COMP] );

    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->text_bmpwin[TEXT_POKE_COMP] ), 0 );
    //GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_POKE_COMP] );  // ������̂ŃR�����g�A�E�g
  }

  // ����̃e�L�X�g��`��
  switch(work->state)
  {
  case STATE_TOP:
    {
      // �`�悵�Ȃ�
      GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_POKE_COMP] );  // ��ł�����̂ŃR�����g�A�E�g���Ă����̂ŁA�����œ]��
    }
    break;
  case STATE_EXCHANGE:
    {
      if( work->diff_num >= 2 )
      {
        Zukan_Detail_Form_WritePokeText( param, work, cmn,
               TEXT_POKE_COMP, work->print_que[TEXT_POKE_COMP], work->text_bmpwin[TEXT_POKE_COMP], &(work->text_trans[TEXT_POKE_COMP]), work->diff_comp_no );
      }
      // 2�p�ȏ�Ȃ���΁A�`�悵�Ȃ�
    }
    break;
  }
}
static void Zukan_Detail_Form_ScrollPokeText( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  switch(work->state)
  {
  case STATE_TOP:
    {
      //GFL_BG_SetScroll( BG_FRAME_M_TEXT, GFL_BG_SCROLL_X_SET, -64 );
      GFL_BG_SetScrollReq( BG_FRAME_M_TEXT, GFL_BG_SCROLL_X_SET, -64 );
    }
    break;
  case STATE_EXCHANGE:
    {
      if( work->diff_num >= 2 )
      {
        //GFL_BG_SetScroll( BG_FRAME_M_TEXT, GFL_BG_SCROLL_X_SET, 0 );
        GFL_BG_SetScrollReq( BG_FRAME_M_TEXT, GFL_BG_SCROLL_X_SET, 0 );
      }
      else
      {
        //GFL_BG_SetScroll( BG_FRAME_M_TEXT, GFL_BG_SCROLL_X_SET, -64 );
        GFL_BG_SetScrollReq( BG_FRAME_M_TEXT, GFL_BG_SCROLL_X_SET, -64 );
      }
    }
    break;
  }
}

//-------------------------------------
/// MCSS�|�P����
//=====================================
static void Zukan_Detail_Form_McssSysInit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  work->mcss_sys_wk = MCSS_Init( POKE_MAX, param->heap_id );
  MCSS_SetTextureTransAdrs( work->mcss_sys_wk, 0 );  // ���Ɉꏏ�ɏo��3D�͂Ȃ��̂�
  MCSS_SetOrthoMode( work->mcss_sys_wk );

#ifdef DEF_MCSS_HANDLE
  MCSS_OpenHandle( work->mcss_sys_wk, POKEGRA_GetArcID() );
#endif

#ifdef DEF_MCSS_TCBSYS
  work->mcss_tcbsys_wk = GFL_HEAP_AllocMemory( param->heap_id, GFL_TCB_CalcSystemWorkSize(MCSS_TCBSYS_TASK_MAX) );
  work->mcss_tcbsys = GFL_TCB_Init( MCSS_TCBSYS_TASK_MAX, work->mcss_tcbsys_wk );
  MCSS_SetTCBSys( work->mcss_sys_wk, work->mcss_tcbsys );
#endif
}
static void Zukan_Detail_Form_McssSysExit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
#ifdef DEF_MCSS_HANDLE
  MCSS_CloseHandle( work->mcss_sys_wk );
#endif

  MCSS_Exit( work->mcss_sys_wk );

#ifdef DEF_MCSS_TCBSYS
  //MCSS_SetTCBSys( work->mcss_sys_wk, NULL );  // NULL��n����tcb���͂�����
  GFL_TCB_Exit( work->mcss_tcbsys );
  GFL_HEAP_FreeMemory( work->mcss_tcbsys_wk );
#endif
}

static MCSS_WORK* PokeInit( MCSS_SYS_WORK* mcss_sys_wk,
                      int mons_no, int form_no, int sex, int rare, BOOL egg, int dir,
                      u32 personal_rnd )  // personal_rnd��mons_no==MONSNO_PATTIIRU�̂Ƃ��̂ݎg�p�����
{
  VecFx32 scale =
  {
    FX_F32_TO_FX32(POKE_SCALE),
    FX_F32_TO_FX32(POKE_SCALE),
    FX32_ONE, 
  };

  MCSS_ADD_WORK add_wk;
  MCSS_WORK*    poke_wk;

  if( mons_no == MONSNO_PATTIIRU )  // MCSS_TOOL_AddPokeMcss�̒��g�ɍ��킹�Ă���
  {
    MCSS_TOOL_SetMakeBuchiCallback( mcss_sys_wk, personal_rnd );
  }
  MCSS_TOOL_MakeMAWParam( mons_no, form_no, sex, rare, egg, &add_wk, dir );
  poke_wk = MCSS_Add( mcss_sys_wk, 0, 0, 0, &add_wk );
  
  MCSS_SetAnmStopFlag( poke_wk );
  MCSS_SetScale( poke_wk, &scale );
  //PokeAdjustOfsPos( poke_wk );  // �n�ʌ��_�Ŗ��Ȃ��̂ŁA�I�t�Z�b�g���Ȃ��B
  PokeAdjustOfsPosX( poke_wk );

  return poke_wk;
}
static void PokeExit( MCSS_SYS_WORK* mcss_sys_wk, MCSS_WORK* poke_wk )
{
  MCSS_SetVanishFlag( poke_wk );
  MCSS_Del( mcss_sys_wk, poke_wk );
}
static void PokeAdjustOfsPos( MCSS_WORK* poke_wk )
{
  f32 size_y = (f32)MCSS_GetSizeY( poke_wk );
  f32 ofs_y;
  VecFx32 ofs;
  if( size_y > POKE_SIZE_MAX ) size_y = POKE_SIZE_MAX;
  ofs_y = ( POKE_SIZE_MAX - size_y ) / 2.0f;
  ofs.x = 0;  ofs.y = FX_F32_TO_FX32(ofs_y);  ofs.z = 0;
  MCSS_SetOfsPosition( poke_wk, &ofs );
}
static void PokeAdjustOfsPosX( MCSS_WORK* poke_wk )
{
  f32      offset_x = (f32)MCSS_GetOffsetX( poke_wk );  // �E�ɂ���Ă���Ƃ�+, ���ɂ���Ă���Ƃ�-
  f32      ofs_position_x;
  VecFx32  ofs_position;

  ofs_position_x = - offset_x;
    
  ofs_position.x = FX_F32_TO_FX32(ofs_position_x);  ofs_position.y = 0;  ofs_position.z = 0;
  MCSS_SetOfsPosition( poke_wk, &ofs_position );
}

static void PokeGetCompareRelativePosition( MCSS_WORK* poke_wk, VecFx32* pos )
{
  VecFx32 origin;
  MCSS_GetPosition( poke_wk, &origin );

  pos->x = origin.x + poke_pos[POKE_COMP_RPOS].x;
  pos->y = origin.y + poke_pos[POKE_COMP_RPOS].y;
  pos->z = origin.z + poke_pos[POKE_COMP_RPOS].z;
}

static void PokeMcssWorkInit( POKE_MCSS_WORK* poke_mcss_wk, HEAPID heap_id,
                              MCSS_SYS_WORK* mcss_sys_wk,
                              int mons_no, int form_no, int sex, int rare, BOOL egg, int dir,
                              u32 personal_rnd )  // personal_rnd��mons_no==MONSNO_PATTIIRU�̂Ƃ��̂ݎg�p�����
{
  poke_mcss_wk->poke_wk = PokeInit( mcss_sys_wk,
                                    mons_no, form_no, sex, rare, egg, dir,
                                    personal_rnd );
  poke_mcss_wk->poke_call_back_data = GFL_HEAP_AllocClearMemory( heap_id, sizeof(POKE_MCSS_CALL_BACK_DATA) );

  {
    NNSG2dMultiCellAnimation* mcss_anim_ctrl = MCSS_GetAnimCtrl( poke_mcss_wk->poke_wk );
    NNS_G2dSetAnimCtrlCallBackFunctor(
        NNS_G2dGetMCAnimAnimCtrl(mcss_anim_ctrl),
        NNS_G2D_ANMCALLBACKTYPE_LAST_FRM,
        (u32)poke_mcss_wk->poke_call_back_data,
        Zukan_Detail_Form_PokeMcssCallBackFunctor );
  }
}
static void PokeMcssWorkExit( POKE_MCSS_WORK* poke_mcss_wk, MCSS_SYS_WORK* mcss_sys_wk )
{
  BLOCK_POKE_EXIT( mcss_sys_wk, poke_mcss_wk->poke_wk )
  if( poke_mcss_wk->poke_call_back_data ) GFL_HEAP_FreeMemory( poke_mcss_wk->poke_call_back_data );
  poke_mcss_wk->poke_call_back_data = NULL;
#ifdef DEF_MINIMUM_LOAD 
  poke_mcss_wk->diff_no = DIFF_MAX;
#endif
}
static void PokeMcssCallBackDataInit( POKE_MCSS_CALL_BACK_DATA* poke_mcss_call_back_data,
                                      POKE_INDEX poke_idx, ZUKAN_DETAIL_FORM_WORK* work )
{
  poke_mcss_call_back_data->poke_idx               = poke_idx;
  poke_mcss_call_back_data->zukan_detail_form_work = (void*)work;
  poke_mcss_call_back_data->count                  = 0;
  poke_mcss_call_back_data->stop                   = FALSE;
}

static void PokeMcssAnimeStart( POKE_MCSS_WORK* poke_mcss_wk )
{
  if( poke_mcss_wk->poke_wk && poke_mcss_wk->poke_call_back_data )
  {
    NNSG2dMultiCellAnimation* mcss_anim_ctrl = MCSS_GetAnimCtrl( poke_mcss_wk->poke_wk );
    poke_mcss_wk->poke_call_back_data->count = 0;
    poke_mcss_wk->poke_call_back_data->stop  = FALSE;
    NNS_G2dRestartMCAnimation( mcss_anim_ctrl );
    MCSS_ResetAnmStopFlag( poke_mcss_wk->poke_wk );
  }
}
static void PokeMcssAnimeMain( POKE_MCSS_WORK* poke_mcss_wk )
{
  if( poke_mcss_wk->poke_wk && poke_mcss_wk->poke_call_back_data )
  {
    NNSG2dMultiCellAnimation* mcss_anim_ctrl = MCSS_GetAnimCtrl( poke_mcss_wk->poke_wk );
    if( poke_mcss_wk->poke_call_back_data->stop )
    {
      MCSS_SetAnmStopFlag( poke_mcss_wk->poke_wk );
      NNS_G2dRestartMCAnimation( mcss_anim_ctrl );
      poke_mcss_wk->poke_call_back_data->stop = FALSE;
    }
  }
}
static void Zukan_Detail_Form_PokeMcssCallBackFunctor( u32 data, fx32 currentFrame )
{
  POKE_MCSS_CALL_BACK_DATA* poke_call_back_data = (POKE_MCSS_CALL_BACK_DATA*)data;
  ZUKAN_DETAIL_FORM_WORK*   work                = (ZUKAN_DETAIL_FORM_WORK*)poke_call_back_data->zukan_detail_form_work;

  poke_call_back_data->count++;
  if( poke_call_back_data->count >= POKE_MCSS_ANIME_LOOP_MAX )
  {
    poke_call_back_data->stop = TRUE;
  }
}

static void Zukan_Detail_Form_FlipFrontBack( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
#ifndef DEF_MINIMUM_LOAD
  // ���̏�Ԃ����āA�O���ύX����
  if( work->is_poke_front )
  {
    MCSS_SetVanishFlag( work->poke_mcss_wk[POKE_CURR_F].poke_wk );
    MCSS_ResetVanishFlag( work->poke_mcss_wk[POKE_CURR_B].poke_wk );
    if( work->diff_num >= 2 )
    {
      MCSS_SetVanishFlag( work->poke_mcss_wk[POKE_COMP_F].poke_wk );
      MCSS_ResetVanishFlag( work->poke_mcss_wk[POKE_COMP_B].poke_wk );
    }
    // �ύX��̏��
    work->is_poke_front = FALSE;
  }
  else
  {
    MCSS_SetVanishFlag( work->poke_mcss_wk[POKE_CURR_B].poke_wk );
    MCSS_ResetVanishFlag( work->poke_mcss_wk[POKE_CURR_F].poke_wk );
    if( work->diff_num >= 2 )
    {
      MCSS_SetVanishFlag( work->poke_mcss_wk[POKE_COMP_B].poke_wk );
      MCSS_ResetVanishFlag( work->poke_mcss_wk[POKE_COMP_F].poke_wk );
    }
    // �ύX��̏��
    work->is_poke_front = TRUE;
  }
#else
  // ���̏�Ԃ����āA�O���ύX����
  if( work->is_poke_front )
  {
    MCSS_SetVanishFlag( work->poke_mcss_wk[POKE_CURR_F].poke_wk );
    if( work->state == STATE_EXCHANGE && work->diff_num >= 2 )
    {
      MCSS_SetVanishFlag( work->poke_mcss_wk[POKE_COMP_F].poke_wk );
    }
    // �ύX��̏��
    work->is_poke_front = FALSE;
  }
  else
  {
    MCSS_SetVanishFlag( work->poke_mcss_wk[POKE_CURR_B].poke_wk );
    if( work->state == STATE_EXCHANGE && work->diff_num >= 2 )
    {
      MCSS_SetVanishFlag( work->poke_mcss_wk[POKE_COMP_B].poke_wk );
    }
    // �ύX��̏��
    work->is_poke_front = TRUE;
  }

  Zukan_Detail_Form_MakePokeWhenFlipFrontBack( param, work, cmn );

  // �ύX��̏�Ԃ����āA�O���ύX����
  if( work->is_poke_front )
  {
    MCSS_ResetVanishFlag( work->poke_mcss_wk[POKE_CURR_F].poke_wk );
    if( work->state == STATE_EXCHANGE && work->diff_num >= 2 )
    {
      MCSS_ResetVanishFlag( work->poke_mcss_wk[POKE_COMP_F].poke_wk );
    }
  }
  else
  {
    MCSS_ResetVanishFlag( work->poke_mcss_wk[POKE_CURR_B].poke_wk );
    if( work->state == STATE_EXCHANGE && work->diff_num >= 2 )
    {
      MCSS_ResetVanishFlag( work->poke_mcss_wk[POKE_COMP_B].poke_wk );
    }
  }
#endif
}


//-------------------------------------
/// �|�P�A�C�R��
//=====================================
static void PokeiconBaseInit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �s�ϕ��̐���
  work->pokeicon_handle = GFL_ARC_OpenDataHandle( ARCID_POKEICON, param->heap_id );

/*
  work->pokeicon_ncl = GFL_CLGRP_PLTT_RegisterEx(
                           work->pokeicon_handle,
                           POKEICON_GetPalArcIndex(),
                           CLSYS_DRAW_SUB,
                           OBJ_PAL_POS_S_POKEICON * 0x20,
                           0,
                           OBJ_PAL_NUM_S_POKEICON,
                           param->heap_id );
*/
  work->pokeicon_ncl = GFL_CLGRP_PLTT_RegisterComp(  // �|�P�A�C�R���̃p���b�g�͈��k����Ă���
                           work->pokeicon_handle,
                           POKEICON_GetPalArcIndex(),
                           CLSYS_DRAW_SUB,
                           OBJ_PAL_POS_S_POKEICON * 0x20,
                           param->heap_id );

  work->pokeicon_nce = GFL_CLGRP_CELLANIM_Register(
                           work->pokeicon_handle,
                           POKEICON_GetCellSubArcIndex(),
                           POKEICON_GetAnmSubArcIndex(),
                           param->heap_id );
}
static void PokeiconBaseExit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �s�ϕ��̐���
  GFL_CLGRP_CELLANIM_Release( work->pokeicon_nce );
  GFL_CLGRP_PLTT_Release( work->pokeicon_ncl );
  
  GFL_ARC_CloseDataHandle( work->pokeicon_handle );
}

static GFL_CLWK* PokeiconInit( ARCHANDLE* pokeicon_handle, u32* ncg, u32 ncl, u32 nce, GFL_CLUNIT* clunit, HEAPID heap_id, u32 mons, u32 form_no, u32 sex, BOOL egg )
{
  GFL_CLWK* clwk;
  GFL_CLWK_DATA data;

  data.pos_x   = pokeicon_pos[0];
  data.pos_y   = pokeicon_pos[1];
  data.anmseq  = 1;  // �A�j���V�[�P���X�Ŏw��( 0=�m��, 1=HP�ő�, 2=HP��, 3=HP��, 4=HP��, 5=��Ԉُ� )
  data.softpri = 1;  // ��ɃA�C�e���A�C�R����`�悷��̂ŗD��x�������Ă���
  data.bgpri   = 0;

  *ncg = GFL_CLGRP_CGR_Register(
             pokeicon_handle,
             POKEICON_GetCgxArcIndexByMonsNumber( mons, form_no, sex, egg ),
             FALSE,
             CLSYS_DRAW_SUB,
             heap_id );
    
  clwk = GFL_CLACT_WK_Create(
             clunit,
             *ncg,
             ncl,
             nce,
             &data,
             CLSYS_DEFREND_SUB,
             heap_id );
   
  // �I�[�g�A�j�� OFF
  GFL_CLACT_WK_SetAutoAnmFlag( clwk, FALSE );

  {
    u8 pal_num = POKEICON_GetPalNum( mons, form_no, sex, egg );
    GFL_CLACT_WK_SetPlttOffs( clwk, pal_num, CLWK_PLTTOFFS_MODE_OAM_COLOR );
  }

  //GFL_CLACT_WK_SetObjMode( clwk, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
  GFL_CLACT_WK_SetObjMode( clwk, GX_OAM_MODE_NORMAL );  // BG�ƂƂ��ɂ���OBJ���Â��������̂́A�t�F�[�h�C���ƃt�F�[�h�A�E�g�̂Ƃ������ł���A
                                                        // �|�P������ύX���������̂Ƃ��́A�Â������蔼�����ɂ����肵�����Ȃ��B
                                                        // BG�ƂƂ��ɂ���OBJ���Â��������Ƃ��́A���̊֐��̌�ɐݒ肷�邱�ƁB

  return clwk;
}
static void PokeiconExit( u32 ncg, GFL_CLWK* clwk )
{
  // CLWK�j��
  GFL_CLACT_WK_Remove( clwk );
  // ���\�[�X�J��
  GFL_CLGRP_CGR_Release( ncg );
}

//-------------------------------------
/// �Ⴄ�p�����擾����
//=====================================
static void Zukan_Detail_Form_GetDiffInfoWithoutWork( ZUKAN_DETAIL_FORM_PARAM* param, ZKNDTL_COMMON_WORK* cmn,
                                                      DIFF_INFO*   a_diff_info_list,  // �v�f��a_diff_info_list[DIFF_MAX]��
                                                      u16*         a_diff_num,
                                                      u16*         a_diff_sugata_num,
                                                      u16*         a_diff_irochigai_num,
                                                      u16*         a_diff_curr_no,
                                                      u16*         a_diff_comp_no )
{
  const u8 sex_tbl_max = 3;
  const u32 sex_tbl[sex_tbl_max] =
  {
    PTL_SEX_MALE,
    PTL_SEX_FEMALE,
    PTL_SEX_UNKNOWN,
  };
  const u8 rare_tbl_max = 2;
  const BOOL rare_tbl[rare_tbl_max] =
  {
    FALSE,
    TRUE,
  };

  u16 monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);

  GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
  ZUKAN_SAVEDATA* zkn_sv = GAMEDATA_GetZukanSave( gamedata );

  // �|�P�����p�[�\�i���f�[�^�̏��
  u32 ppd_form_num;  // �t�H������
  u32 ppd_sex_vec;   // ���ʃx�N�g��

  // ���ݕ\������p
  u32  curr_sex;
  BOOL curr_rare;
  u32  curr_form;

  // �|�P�����p�[�\�i���f�[�^�̏��
  {
    POKEMON_PERSONAL_DATA* ppd = POKE_PERSONAL_OpenHandle( monsno, 0, param->heap_id );
    ppd_form_num = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_form_max );  // �ʃt�H�����Ȃ��̂Ƃ���1
    ppd_sex_vec  = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_sex );  // POKEPER_SEX_MALE, 1�`253, POKEPER_SEX_FEMALE, POKEPER_SEX_UNKNOWN  // prog/include/poke_tool/poke_personal.h
    POKE_PERSONAL_CloseHandle( ppd );

#ifdef DEBUG_KAWADA
    {
      OS_Printf( "ZUKAN_DETAIL_FORM : monsno=%d, ppd f=%d s=%d\n", monsno, ppd_form_num, ppd_sex_vec );
    }
#endif
  }

  // �p�[�\�i���̃f�[�^�ł͂Ȃ��A�}�ӂ������Ă���f�[�^���g�p����
  {
    ppd_form_num = ZUKANSAVE_GetFormMax( monsno );  // monsno�̃|�P�������}�ӂɓo�^���邱�Ƃ��ł���t�H�������𓾂���B
        // �v���C���[�����݂܂łɌ����t�H��������Ԃ��Ă���킯�ł͂Ȃ��B
        // �|�P����WB�ł́A0�ԃt�H��������A�Ԃŏo������B
        // �ԍ���є�т̃t�H�������o�����邱�Ƃ͂Ȃ��B
        // ����āA0<= <ZUKANSAVE_GetFormMax( monsno )�̃t�H�������������ǂ����`�F�b�N����΂����B
  }

  // ���ݕ\������p
  {
    ZUKANSAVE_GetDrawData(  zkn_sv, monsno, &curr_sex, &curr_rare, &curr_form, param->heap_id );

#ifdef DEBUG_KAWADA
    {
      OS_Printf( "ZUKAN_DETAIL_FORM : monsno=%d, curr f=%d, s=%d, r=%d\n", monsno, curr_form, curr_sex, curr_rare );
    }
#endif
  }

  // �v���C���[�����肵�����
  {
    u16 diff_num = 0;
    u16 diff_curr_no = 0;

    u32  sex;
    BOOL rare;
    u32  form;

    u8 sex_tbl_idx;
    u8 rare_tbl_idx;

    {
      // ���ʂŃ`�F�b�N���邵�Ȃ��𕪂���
      BOOL b_sex_check_tbl[sex_tbl_max] =
      {
        FALSE,
        FALSE,
        FALSE,
      };
      switch( ppd_sex_vec )
      {
        // ���ʂ���(�I�X�̂�)
      case POKEPER_SEX_MALE:
        {
          b_sex_check_tbl[0] = TRUE;
        }
        break;
        // ���ʂ���(���X�̂�)
      case POKEPER_SEX_FEMALE:
        {
          b_sex_check_tbl[1] = TRUE;
        }
        break;
        // ���ʂȂ�
      case POKEPER_SEX_UNKNOWN:
        {
          b_sex_check_tbl[2] = TRUE;
        }
        break;
        // ���ʂ���(�I�X���X)
      default:  // 1�`253
        {
          b_sex_check_tbl[0] = TRUE;
          b_sex_check_tbl[1] = TRUE;
        }
        break;
      }

      for( form=0; form<ppd_form_num; form++ )
      {
        BOOL b_rare_finish_tbl[rare_tbl_max] =  // �t�H�����Ⴂ������Ƃ��́A�I�X���X�͂��낤�ƂȂ��낤�ƁA��������
        {                                       // ���߂ɁA�m�[�}��/���A���ꂼ�ꂪ�ǂꂩ�̐��ʂœo�^����Ă�����A
          FALSE,                                // �����o�^���Ȃ��悤�ɂ��邽�߂̃t���O
          FALSE,
        };
        for( sex_tbl_idx=0; sex_tbl_idx<sex_tbl_max; sex_tbl_idx++ )
        {
          if( !b_sex_check_tbl[sex_tbl_idx] ) continue;
          sex = sex_tbl[sex_tbl_idx];
          for( rare_tbl_idx=0; rare_tbl_idx<rare_tbl_max; rare_tbl_idx++ )
          {
            BOOL b_regist = FALSE;
            rare = rare_tbl[rare_tbl_idx];
            if( curr_form == form && curr_rare == rare && curr_sex == sex )  // ���ݕ\������p
            {
              diff_curr_no = diff_num;
              b_regist = TRUE;
            }
            else if( ( ppd_form_num >= 2 ) && curr_form == form && curr_rare == rare )  // �t�H�����Ⴂ������Ƃ��́A�I�X���X�͂��낤�ƂȂ��낤�ƁA��������
            {
              // �������Ȃ�
            }
            else
            {
              if( ZUKANSAVE_CheckPokeSeeForm( zkn_sv, monsno, (int)sex, (int)rare, (int)form ) )
              {
                b_regist = TRUE;
              }
            }

            if( b_regist )
            {
              if(    ( ( ppd_form_num >= 2 ) && ( !b_rare_finish_tbl[rare_tbl_idx] ) )    // �t�H�����Ⴂ������Ƃ��́A�܂����̃m�[�}��/���A���ǂ̐��ʂł��o�^����Ă��Ȃ��Ƃ�
                  || ( ( ppd_form_num <  2 )                                         ) )  // �t�H�����Ⴂ���Ȃ��Ƃ��́A���ł��^
              {
                GF_ASSERT_MSG( diff_num < DIFF_MAX,  "ZUKAN_DETAIL_FORM : �Ⴄ�p�̐��������ł��B\n" );
                
                b_rare_finish_tbl[rare_tbl_idx] = TRUE;

                if( ppd_form_num >= 2 )
                {
                  a_diff_info_list[diff_num].looks_diff = LOOKS_DIFF_FORM;
                  a_diff_info_list[diff_num].color_diff = rare?COLOR_DIFF_SPECIAL:COLOR_DIFF_NORMAL;
                  a_diff_info_list[diff_num].other_diff = (u16)form;
                }
                else
                {
                  a_diff_info_list[diff_num].looks_diff = (ppd_sex_vec==POKEPER_SEX_UNKNOWN)?LOOKS_DIFF_ONE:LOOKS_DIFF_SEX;
                  a_diff_info_list[diff_num].color_diff = rare?COLOR_DIFF_SPECIAL:COLOR_DIFF_NORMAL;
                  a_diff_info_list[diff_num].other_diff = (sex==PTL_SEX_FEMALE)?SEX_FEMALE:SEX_MALE;  // LOOKS_DIFF_ONE�̂Ƃ��͎g�p����Ȃ��̂ŁA���������Ă��Ă�����
                }
                a_diff_info_list[diff_num].sex        = (u8)sex;
                a_diff_info_list[diff_num].rare       = (u8)rare;
                a_diff_info_list[diff_num].form       = (u16)form;

#ifdef DEBUG_KAWADA
                {
                  OS_Printf( "ZUKAN_DETAIL_FORM : monsno=%d, diff_no=%d, f=%d, s=%d, r=%d\n", monsno, diff_num, form, sex, rare );
                }
#endif

                diff_num++;
              }
            }
          }
        }
      }
    }

    GF_ASSERT_MSG( diff_num > 0,  "ZUKAN_DETAIL_FORM : �p����������܂���B\n" );

    *a_diff_num = diff_num;
    *a_diff_curr_no = diff_curr_no;
    //*a_diff_comp_no = (diff_curr_no!=0)?(0):(1%diff_num);  // 0�Ԗڂ�1�Ԗڂ̎p�ɂ��Ă���
    *a_diff_comp_no = (diff_curr_no+1)%diff_num;  // curr�̎��̎p�ɂ��Ă���
  }

  // �p�̐��A�F�Ⴂ�̐�
  {
    u8 i;

    *a_diff_sugata_num = 0;
    *a_diff_irochigai_num = 0;

    for( i=0; i<(*a_diff_num); i++ )
    {
      if( a_diff_info_list[i].color_diff == COLOR_DIFF_SPECIAL )
      {
        (*a_diff_irochigai_num)++;
      }
    }

    (*a_diff_sugata_num) = (*a_diff_num) - (*a_diff_irochigai_num);
  }

/*
  �p�[�\�i���̃f�[�^�ł͂Ȃ��A�}�ӂ������Ă���f�[�^���g�p����
  �悤�ɂ����̂ŁA�ȉ��̓��ʏ����͕s�v�B
  // ���ʏ���
  {
    // �t�H����0�Ԃ��������Ȃ��|�P����
    if(
           monsno == MONSNO_ARUSEUSU    // �A���Z�E�X
        || monsno == MONSNO_INSEKUTA    // �C���Z�N�^
    )
    {
      if( *a_diff_num > 0 )
      {
        // �u�t�H����0�ԁv�Ɓu�t�H����0�Ԃ̐F�Ⴂ�v�����c��
        DIFF_INFO   tmp_diff_info_list[2];
        u16         tmp_diff_num             = 0;
        s16         tmp_diff_curr_no         = -1;
        s16         tmp_diff_comp_no         = -1;
        u16 i;
        for( i=0; i<(*a_diff_num); i++ )
        {
          if( a_diff_info_list[i].form == 0 )
          {
            tmp_diff_info_list[tmp_diff_num] = a_diff_info_list[i];
            if( *a_diff_curr_no == i ) tmp_diff_curr_no = tmp_diff_num;
            if( *a_diff_comp_no == i ) tmp_diff_comp_no = tmp_diff_num;
            tmp_diff_num++;
            if( tmp_diff_num == 2 ) break;
          }
        }
        if( tmp_diff_num == 0 )  // �N���蓾�Ȃ��͂������O�̂���
        {
          tmp_diff_info_list[tmp_diff_num] = a_diff_info_list[0];
          tmp_diff_num++;
        }
        if( tmp_diff_curr_no == -1 )
        {
          tmp_diff_curr_no = (tmp_diff_curr_no==0)?(1):(0);
        }
        if( tmp_diff_comp_no == -1 )
        {
          tmp_diff_comp_no = (tmp_diff_curr_no==0)?(1):(0); 
        }

        // ���ʂ������A��
        for( i=0; i<tmp_diff_num; i++ )
        {
          a_diff_info_list[i] = tmp_diff_info_list[i];
        }
        *a_diff_num = tmp_diff_num;
        *a_diff_curr_no = (u16)tmp_diff_curr_no;
        *a_diff_comp_no = (u16)tmp_diff_comp_no;
        *a_diff_sugata_num = 0;
        *a_diff_irochigai_num = 0;
        for( i=0; i<(*a_diff_num); i++ )
        {
          if( a_diff_info_list[i].color_diff == COLOR_DIFF_SPECIAL )
          {
            (*a_diff_irochigai_num)++;
          }
        }
        (*a_diff_sugata_num) = (*a_diff_num) - (*a_diff_irochigai_num);
      }
    }
  }
*/
}
static void Zukan_Detail_Form_GetDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  Zukan_Detail_Form_GetDiffInfoWithoutWork( param, cmn,
                                            work->diff_info_list,
                                            &work->diff_num,
                                            &work->diff_sugata_num,
                                            &work->diff_irochigai_num,
                                            &work->diff_curr_no,
                                            &work->diff_comp_no );
}

//-------------------------------------
/// �Ⴄ�p��񂩂�MCSS�|�P�����𐶐�����
//=====================================
static void Zukan_Detail_Form_PokeInitFromDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                POKE_INDEX poke_f, POKE_INDEX poke_b, POKE_POS_INDEX pos, u16 diff_no )
{
  int mons_no;
  int form_no;
  int sex;
  int rare;
  u32 personal_rnd = 0;

  mons_no = ZKNDTL_COMMON_GetCurrPoke(cmn);

  form_no = work->diff_info_list[diff_no].form;
  sex = work->diff_info_list[diff_no].sex;
  rare = work->diff_info_list[diff_no].rare;

  if( mons_no == MONSNO_PATTIIRU )  // personal_rnd��mons_no==MONSNO_PATTIIRU�̂Ƃ��̂ݎg�p�����
  {
    GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
    ZUKAN_SAVEDATA* zkn_sv = GAMEDATA_GetZukanSave( gamedata );
    personal_rnd = ZUKANSAVE_GetPokeRandomFlag( zkn_sv, ZUKANSAVE_RANDOM_PACHI );
  }

#ifndef DEF_MINIMUM_LOAD
  //work->poke_mcss_wk[poke_f].poke_wk = PokeInit( work->mcss_sys_wk,
  //                            mons_no, form_no, sex, rare, FALSE, MCSS_DIR_FRONT );
  //work->poke_mcss_wk[poke_b].poke_wk = PokeInit( work->mcss_sys_wk,
  //                            mons_no, form_no, sex, rare, FALSE, MCSS_DIR_BACK );
  PokeMcssWorkInit( &work->poke_mcss_wk[poke_f], param->heap_id,
                    work->mcss_sys_wk,
                    mons_no, form_no, sex, rare, FALSE, MCSS_DIR_FRONT,
                    personal_rnd );
  PokeMcssWorkInit( &work->poke_mcss_wk[poke_b], param->heap_id,
                    work->mcss_sys_wk,
                    mons_no, form_no, sex, rare, FALSE, MCSS_DIR_BACK,
                    personal_rnd );

  {
    VecFx32 p;
    if( pos == POKE_COMP_RPOS )
    {
      PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );
    }
    else
    {
      p = poke_pos[pos];
    }
    MCSS_SetPosition( work->poke_mcss_wk[poke_f].poke_wk, &p );
    MCSS_SetPosition( work->poke_mcss_wk[poke_b].poke_wk, &p );
  }

  if( work->is_poke_front )
    MCSS_SetVanishFlag( work->poke_mcss_wk[poke_b].poke_wk );
  else
    MCSS_SetVanishFlag( work->poke_mcss_wk[poke_f].poke_wk );
#else
  {
    POKE_INDEX  poke_present;    // ���̌���
    POKE_INDEX  poke_opposite;   // ���̌����ł͂Ȃ��ق��̌���
    int         dir;             // ���̌���
    POKE_INDEX  poke_relative;   // ���̌����̈ʒu�̌�
    if( work->is_poke_front )
    {
      poke_present  = poke_f;
      poke_opposite = poke_b;
      dir           = MCSS_DIR_FRONT;
      poke_relative = POKE_CURR_F;
    }
    else
    {
      poke_present  = poke_b;
      poke_opposite = poke_f;
      dir           = MCSS_DIR_BACK;
      poke_relative = POKE_CURR_B;
    }
    PokeMcssWorkInit( &work->poke_mcss_wk[poke_present], param->heap_id,
                      work->mcss_sys_wk,
                      mons_no, form_no, sex, rare, FALSE, dir,
                      personal_rnd );
    work->poke_mcss_wk[poke_present].diff_no = diff_no;
    {
      VecFx32 p;
      if( pos == POKE_COMP_RPOS )
      {
        PokeGetCompareRelativePosition( work->poke_mcss_wk[poke_relative].poke_wk, &p );
      }
      else
      {
        p = poke_pos[pos];
      }
      MCSS_SetPosition( work->poke_mcss_wk[poke_present].poke_wk, &p );
    }
    if( work->poke_mcss_wk[poke_opposite].poke_wk ) MCSS_SetVanishFlag( work->poke_mcss_wk[poke_opposite].poke_wk );
  }
#endif
}

//-------------------------------------
/// �Ⴄ�p��񂩂�|�P�A�C�R���𐶐�����
//=====================================
static void Zukan_Detail_Form_PokeiconInitFromDiffInfo( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                u16 diff_no )
{
  u32 mons;
  u32 form_no;
  u32 sex;

  mons = ZKNDTL_COMMON_GetCurrPoke(cmn);

  form_no = work->diff_info_list[diff_no].form;
  sex = work->diff_info_list[diff_no].sex;

  work->pokeicon_clwk[work->pokeicon_swap_curr] = PokeiconInit(
                                                      work->pokeicon_handle,
                                                      &(work->pokeicon_ncg[work->pokeicon_swap_curr]),
                                                      work->pokeicon_ncl,
                                                      work->pokeicon_nce,
                                                      work->clunit,
                                                      param->heap_id,
                                                      mons,
                                                      form_no,
                                                      sex,
                                                      0 );
}

//-------------------------------------
/// �|�P�����ύX
//=====================================
static void Zukan_Detail_Form_ChangePoke( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �O�̂�j��
  u8 i;
  for( i=0; i<POKE_MAX; i++ )
  {
    //BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_mcss_wk[i].poke_wk )
    PokeMcssWorkExit( &work->poke_mcss_wk[i], work->mcss_sys_wk );
  }

  // ���̂𐶐�
  Zukan_Detail_Form_GetDiffInfo( param, work, cmn );

  {
    POKE_POS_INDEX pos = POKE_CURR_POS_CENTER;
    if( work->state == STATE_EXCHANGE && work->diff_num >= 2 ) 
    {
      pos = POKE_CURR_POS_LEFT;
    }
    Zukan_Detail_Form_PokeInitFromDiffInfo( param, work, cmn,
        POKE_CURR_F, POKE_CURR_B, pos, work->diff_curr_no );
#ifndef DEF_MINIMUM_LOAD
    PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data,
                              POKE_CURR_F, work );
    PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data,
                              POKE_CURR_B, work );
#else
    if( work->is_poke_front )
    {
      PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data,
                                POKE_CURR_F, work );
    }
    else
    {
      PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data,
                                POKE_CURR_B, work );
    }
#endif
  }

#ifndef DEF_MINIMUM_LOAD
  if( work->diff_num >= 2 )
  {
    Zukan_Detail_Form_PokeInitFromDiffInfo( param, work, cmn,
        POKE_COMP_F, POKE_COMP_B, POKE_COMP_RPOS, work->diff_comp_no );
    PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data,
                              POKE_COMP_F, work );
    PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data,
                              POKE_COMP_B, work );
  }
#else
  if( work->state == STATE_EXCHANGE && work->diff_num >= 2 )
  {
    Zukan_Detail_Form_PokeInitFromDiffInfo( param, work, cmn,
        POKE_COMP_F, POKE_COMP_B, POKE_COMP_RPOS, work->diff_comp_no );
    if( work->is_poke_front )
    {
      PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data,
                                POKE_COMP_F, work );
    }
    else
    {
      PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data,
                                POKE_COMP_B, work );
    }
  }
#endif

  // �|�P�A�C�R���ύX
  Zukan_Detail_Form_ChangePokeicon( param, work, cmn );

  // �e�L�X�g
  // �ύX���ꂽ�|�P�����ŁA���ʂ̃e�L�X�g������
  Zukan_Detail_Form_WriteUpText( param, work, cmn );
  // �ύX���ꂽ�|�P�����ŁA�e�L�X�g���X�N���[�������āA���̃t�H�����̃e�L�X�g�������A��r�t�H�����̃e�L�X�g������or����
  Zukan_Detail_Form_ScrollPokeText( param, work, cmn );
  Zukan_Detail_Form_WritePokeCurrText( param, work, cmn );
  Zukan_Detail_Form_WritePokeCompText( param, work, cmn );

  // �ύX���ꂽ�|�P�����p�ɁA�t�H�������̔w�ʃt�B�[���h��p�ӂ���
  Zukan_Detail_Form_ObjFieldSetup( param, work, cmn );

  // �ύX���ꂽ�|�P�����p�ɁA�X�N���[���o�[��p�ӂ���
  Zukan_Detail_Form_ObjBarSetup( param, work, cmn );

  // �ύX���ꂽ�|�P�����p�ɁA�g�b�v��ʂŃt�H������؂�ւ��邽�߂̖���p�ӂ���
  Zukan_Detail_Form_ObjButtonArrowSetup( param, work, cmn );

/*
  // �����o���p�֐��𗘗p����comp��curr�̎��̃t�H�����ɂ��Ă���
  // �����o���p�֐��𗘗p���Ĉʒu�ݒ�
  if( work->state == STATE_TOP && work->diff_num >= 2 ) 
  {
    work->oshidashi_direct = OSHIDASHI_DIRECT_R_TO_L;
    Zukan_Detail_Form_OshidashiChangeCompareForm( param, work, cmn );
  }
*/
  // Zukan_Detail_Form_GetDiffInfo��comp��curr�̎��̃t�H�����ɂ���悤�ɂ����̂ŁA
  // �����o���p�֐��𗘗p���Ĉʒu�ݒ肾���s�����Ƃɂ���
#ifndef DEF_MINIMUM_LOAD
  if( work->state == STATE_TOP && work->diff_num >= 2 ) 
  {
    // �����o���p�֐��𗘗p���Ĉʒu�ݒ�
    Zukan_Detail_Form_OshidashiSetPosCompareForm( param, work, cmn );
  }
#endif
}

#ifdef DEF_MINIMUM_LOAD
//-------------------------------------
/// �|�P�����̑O���ύX�����ۂɁA�ύX���MCSS�����݂��Ȃ�������쐬����
//=====================================
static void Zukan_Detail_Form_MakePokeWhenFlipFrontBack( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // work->is_poke_front�ɂ͂��ꂩ��쐬������������ɐݒ肳��Ă���

  // �O�̂�j��
  if( work->is_poke_front )
  {
    //BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_mcss_wk[POKE_CURR_F].poke_wk )
    //BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_mcss_wk[POKE_COMP_F].poke_wk )
    //PokeMcssWorkExit( &work->poke_mcss_wk[POKE_CURR_F], work->mcss_sys_wk );
    //PokeMcssWorkExit( &work->poke_mcss_wk[POKE_COMP_F], work->mcss_sys_wk );
    if( work->diff_curr_no != work->poke_mcss_wk[POKE_CURR_F].diff_no ) PokeMcssWorkExit( &work->poke_mcss_wk[POKE_CURR_F], work->mcss_sys_wk );
    if( work->diff_comp_no != work->poke_mcss_wk[POKE_COMP_F].diff_no ) PokeMcssWorkExit( &work->poke_mcss_wk[POKE_COMP_F], work->mcss_sys_wk );
  }
  else
  {
    //BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_mcss_wk[POKE_CURR_B].poke_wk )
    //BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_mcss_wk[POKE_COMP_B].poke_wk )
    //PokeMcssWorkExit( &work->poke_mcss_wk[POKE_CURR_B], work->mcss_sys_wk );
    //PokeMcssWorkExit( &work->poke_mcss_wk[POKE_COMP_B], work->mcss_sys_wk );
    if( work->diff_curr_no != work->poke_mcss_wk[POKE_CURR_B].diff_no ) PokeMcssWorkExit( &work->poke_mcss_wk[POKE_CURR_B], work->mcss_sys_wk );
    if( work->diff_comp_no != work->poke_mcss_wk[POKE_COMP_B].diff_no ) PokeMcssWorkExit( &work->poke_mcss_wk[POKE_COMP_B], work->mcss_sys_wk );
  }

  {
    POKE_POS_INDEX pos = POKE_CURR_POS_CENTER;
    if( work->state == STATE_EXCHANGE && work->diff_num >= 2 ) 
    {
      pos = POKE_CURR_POS_LEFT;
    }

    if(    ( ( (work->is_poke_front)) && (!(work->poke_mcss_wk[POKE_CURR_F].poke_wk)) )
        || ( (!(work->is_poke_front)) && (!(work->poke_mcss_wk[POKE_CURR_B].poke_wk)) ) )
    {
      Zukan_Detail_Form_PokeInitFromDiffInfo( param, work, cmn,
          POKE_CURR_F, POKE_CURR_B, pos, work->diff_curr_no );

      if( work->is_poke_front )
      {
        PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data,
                                  POKE_CURR_F, work );
      }
      else
      {
        PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data,
                                  POKE_CURR_B, work );
      }
    }
    // �ʒu��ݒ肷��(�����ŐV���ɍ쐬���Ă��Ȃ��ꍇ�́A�O�̈ʒu�̂܂܂̂��Ƃ�����̂�)(�V���ɍ쐬�����ꍇ�́A�ʒu�͑��v)
    else
    {
      VecFx32 p = poke_pos[pos];
      if( work->is_poke_front )
      {
        MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );
      }
      else
      {
        MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &p );
      }
    }
  }

  if( work->state == STATE_EXCHANGE && work->diff_num >= 2 )
  {
    if(    ( ( (work->is_poke_front)) && (!(work->poke_mcss_wk[POKE_COMP_F].poke_wk)) )
        || ( (!(work->is_poke_front)) && (!(work->poke_mcss_wk[POKE_COMP_B].poke_wk)) ) )
    { 
      Zukan_Detail_Form_PokeInitFromDiffInfo( param, work, cmn,
          POKE_COMP_F, POKE_COMP_B, POKE_COMP_RPOS, work->diff_comp_no );

      if( work->is_poke_front )
      {
        PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data,
                                  POKE_COMP_F, work );
      }
      else
      {
        PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data,
                                  POKE_COMP_B, work );
      }
    }
    // �ʒu��ݒ肷��(�����ŐV���ɍ쐬���Ă��Ȃ��ꍇ�́A�O�̈ʒu�̂܂܂̂��Ƃ�����̂�)(�V���ɍ쐬�����ꍇ�́A�ʒu�͑��v)
    else
    {
      VecFx32 p;
      if( work->is_poke_front )
      {
        PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );  // POKE_COMP_RPOS�Ȃ̂� 
        MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &p );
      }
      else
      {
        PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &p );  // POKE_COMP_RPOS�Ȃ̂� 
        MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &p );
      }
    }
  }
}
#endif

//-------------------------------------
/// ��r�t�H�����ύX
//=====================================
static void Zukan_Detail_Form_ChangeCompareForm( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �O�̂�j��
  //BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_mcss_wk[POKE_COMP_F].poke_wk )
  //BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_mcss_wk[POKE_COMP_B].poke_wk )
  PokeMcssWorkExit( &work->poke_mcss_wk[POKE_COMP_F], work->mcss_sys_wk );
  PokeMcssWorkExit( &work->poke_mcss_wk[POKE_COMP_B], work->mcss_sys_wk );
  
  // ���̂𐶐�
  Zukan_Detail_Form_PokeInitFromDiffInfo( param, work, cmn,
      POKE_COMP_F, POKE_COMP_B, POKE_COMP_RPOS, work->diff_comp_no );
#ifndef DEF_MINIMUM_LOAD
  PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data,
                            POKE_COMP_F, work );
  PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data,
                            POKE_COMP_B, work );
#else
  if( work->is_poke_front )
  {
    PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data,
                              POKE_COMP_F, work );
  }
  else
  {
    PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data,
                              POKE_COMP_B, work );
  }
#endif

  // �e�L�X�g
  // �ύX���ꂽ��r�t�H�����Ńe�L�X�g������
  Zukan_Detail_Form_WritePokeCompText( param, work, cmn );

  // �e�L�X�g�ɍ��킹�āA�t�H�������̔w�ʃt�B�[���h��p�ӂ���
  Zukan_Detail_Form_ObjFieldSetup( param, work, cmn );
}

//-------------------------------------
/// �|�P�A�C�R���ύX
//=====================================
static void Zukan_Detail_Form_ChangePokeicon( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // ���\�����Ă�����̂��\���ɂ��A���̂��̂�\������悤�X���b�v�����ւ���
  {
    // ���\�����Ă�����̂��\���ɂ���
    if( work->pokeicon_clwk[work->pokeicon_swap_curr] )
    {
      GFL_CLACT_WK_SetDrawEnable( work->pokeicon_clwk[work->pokeicon_swap_curr], FALSE );
    }
    // ���̂��̂�\������悤�X���b�v�����ւ���
    work->pokeicon_swap_curr = ( work->pokeicon_swap_curr +1 ) %OBJ_SWAP_MAX;
  }

  // �O�̂�j��
  if( work->pokeicon_clwk[work->pokeicon_swap_curr] )
  {
    // NULL�`�F�b�N�̓}�N�����ōs���Ă���
    BLOCK_POKEICON_EXIT( work->pokeicon_ncg[work->pokeicon_swap_curr], work->pokeicon_clwk[work->pokeicon_swap_curr] )
  } 

  // ���̂𐶐�
  Zukan_Detail_Form_PokeiconInitFromDiffInfo( param, work, cmn, work->diff_curr_no );
}

//-------------------------------------
/// ����
//=====================================
static void Zukan_Detail_Form_Input( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  
  switch(work->state)
  {
  case STATE_TOP:
    {
      if( work->input_enable )
      {
        BUTTON_OBJ curr_push_button;

        // �O��{�^���A�Đ��{�^��  // �g�b�v��ʂŃt�H������؂�ւ��邽�߂̖��
        curr_push_button = Zukan_Detail_Form_ObjButtonCheckPush( param, work, cmn );
        if( curr_push_button != BUTTON_OBJ_NONE )
        {
          switch( curr_push_button )
          {
          case BUTTON_OBJ_FRONT_BACK:
            {
              Zukan_Detail_Form_FlipFrontBack( param, work, cmn );
            }
            break;
          case BUTTON_OBJ_PLAY:
            {
              u8 i;
              for( i=0; i<POKE_MAX; i++ )
              {
                PokeMcssAnimeStart( &work->poke_mcss_wk[i] );  // ���̊֐�����NULL�`�F�b�N�͂��Ă���Ă���
              }
            }
            break;
          case BUTTON_OBJ_ARROW_L:
            {
              work->oshidashi_direct = OSHIDASHI_DIRECT_R_TO_L;
              Zukan_Detail_Form_OshidashiChangeCompareForm( param, work, cmn );
              Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_TOP_OSHIDASHI );
            }
            break;
          case BUTTON_OBJ_ARROW_R:
            {
              work->oshidashi_direct = OSHIDASHI_DIRECT_L_TO_R;
              Zukan_Detail_Form_OshidashiChangeCompareForm( param, work, cmn );
              Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_TOP_OSHIDASHI );
            }
            break;
          }
        }
        else
        {
          u32 x, y;
          BOOL change_state = FALSE;
          int tk;  // Touch or Key
          
          if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
          {
            change_state = TRUE;
            tk = GFL_APP_END_KEY;
          }
          else
          {
	  	      if( GFL_UI_TP_GetPointTrg( &x, &y ) )
            {
              if( 0<=y&&y<168)
              {
                change_state = TRUE;
                tk = GFL_APP_END_TOUCH;
              }
            }
          }
          if( change_state )
          {
            if( work->diff_num >= 2 )
            {
#ifdef DEF_MINIMUM_LOAD
              if(    ( ( (work->is_poke_front)) && (!(work->poke_mcss_wk[POKE_COMP_F].poke_wk)) )
                  || ( (!(work->is_poke_front)) && (!(work->poke_mcss_wk[POKE_COMP_B].poke_wk)) ) )
              {
                work->oshidashi_direct = OSHIDASHI_DIRECT_R_TO_L;
                Zukan_Detail_Form_OshidashiChangeCompareForm( param, work, cmn );
              }
#endif

              //Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_EXCHANGE );
              Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_TOP_TO_EXCHANGE );
              PMSND_PlaySE( SEQ_SE_DECIDE1 );
              GFL_UI_SetTouchOrKey( tk );
              ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, FALSE );
            }
          }
        }
      }
    }
    break;
  case STATE_EXCHANGE:
    {
      if( work->input_enable )
      {
        // �^�b�`�p�����̏ꍇ�́A���ꂪ�ŗD��ƂȂ�
        if( work->bar_cursor_move_by_touch )
        {
          // �X�N���[���o�[�̑���`�F�b�N�͑��̊֐��ōs���Ă���̂ŁA�����ł͍s��Ȃ�
        }
        else
        {
          BUTTON_OBJ curr_push_button;

          // �O��{�^���A�Đ��{�^��
          curr_push_button = Zukan_Detail_Form_ObjButtonCheckPush( param, work, cmn );
          if( curr_push_button != BUTTON_OBJ_NONE )
          {
            switch( curr_push_button )
            {
            case BUTTON_OBJ_FRONT_BACK:
              {
                Zukan_Detail_Form_FlipFrontBack( param, work, cmn );
              }
              break;
            case BUTTON_OBJ_PLAY:
              {
                u8 i;
                for( i=0; i<POKE_MAX; i++ )
                {
                  PokeMcssAnimeStart( &work->poke_mcss_wk[i] );  // ���̊֐�����NULL�`�F�b�N�͂��Ă���Ă���
                }
              }
              break;
            }
          }
        }
      }
    }
    break;
  }
}

//-------------------------------------
/// ��Ԃ�J�ڂ�����
//=====================================
static void Zukan_Detail_Form_ChangeState( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                STATE state )
{
  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  ZUKAN_DETAIL_HEADBAR_WORK*   headbar  = ZKNDTL_COMMON_GetHeadbar(cmn);
  BOOL b_top_exchange = FALSE;  // STATE_TOP����STATE_EXCHANGE�ɕύX����Ƃ��A�������͂��̋t�̂Ƃ�TRUE

  // �J�ڑO�̏�� 
  switch(work->state)
  {
  case STATE_TOP:
    {
      // �J�ڌ�̏��
      switch(state)
      {
      case STATE_TOP_OSHIDASHI:
        {
          if( work->oshidashi_state == OSHIDASHI_STATE_NONE )
          {
            work->oshidashi_state = OSHIDASHI_STATE_MOVE;
          }
          else
          {
            // �������Ȃ�
          }
        }
        break;
      case STATE_TOP_TO_EXCHANGE:
        {
          work->kaisou_curr_end = FALSE;
          work->kaisou_comp_end = FALSE;

          // �����o���p�֐��𗘗p���Ĉʒu�ݒ�
          if( /*work->state == STATE_TOP &&*/ work->diff_num >= 2 ) 
          {
            work->oshidashi_direct = OSHIDASHI_DIRECT_R_TO_L;
            Zukan_Detail_Form_OshidashiSetPosCompareForm( param, work, cmn );
          }
        }
        break;
      }
    }
    break;
  case STATE_TOP_OSHIDASHI:
    {
      // �J�ڌ�̏��
      switch(state)
      {
      case STATE_TOP:
        {
          if( work->push_button == BUTTON_OBJ_NONE )  // �{�^���������̂Ƃ��́A���̉����������܂��Ă���A�N�e�B�u�ɖ߂�
          {
            ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, TRUE );
          }
        }
        break;
      }
    }
    break;
  case STATE_TOP_TO_EXCHANGE:
    {
      // �J�ڌ�̏��
      switch(state)
      {
/*
      case STATE_EXCHANGE:
        {
          b_top_exchange = TRUE;

          if( work->diff_num >= 2 )
          {
            VecFx32 p;

            MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &poke_pos[POKE_CURR_POS_LEFT] );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &poke_pos[POKE_CURR_POS_LEFT] );

            PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &p );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &p );
          }

          // �^�b�`�o�[
          ZUKAN_DETAIL_TOUCHBAR_SetType(
              touchbar,
              ZUKAN_DETAIL_TOUCHBAR_TYPE_FORM,
              ZUKAN_DETAIL_TOUCHBAR_DISP_FORM );
          if( work->diff_num >= 3 )
          {
            ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR( touchbar, TRUE );
          }
          else
          {
            ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR( touchbar, FALSE );
          }

          // �^�C�g���o�[
          ZUKAN_DETAIL_HEADBAR_SetType(
              headbar,
              ZUKAN_DETAIL_HEADBAR_TYPE_FORM_COMPARE );  
        }
        break;
*/
      case STATE_TOP_TO_EXCHANGE_TOUCHBAR_CHANGE:
        {
          // �^�b�`�o�[
          ZUKAN_DETAIL_TOUCHBAR_SetVisibleAll(
              touchbar,
              FALSE );
        }
        break;
      }
    }
    break;
  case STATE_TOP_TO_EXCHANGE_TOUCHBAR_CHANGE:
    {
      // �J�ڌ�̏��
      switch(state)
      {
      case STATE_EXCHANGE:
        {
          b_top_exchange = TRUE;

          if( work->diff_num >= 2 )
          {
            VecFx32 p;

#ifndef DEF_MINIMUM_LOAD
            MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &poke_pos[POKE_CURR_POS_LEFT] );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &poke_pos[POKE_CURR_POS_LEFT] );

            PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &p );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &p );
#else
            if( work->is_poke_front )
            {
              MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &poke_pos[POKE_CURR_POS_LEFT] );
              PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );
              MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &p );
            }
            else
            {
              MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &poke_pos[POKE_CURR_POS_LEFT] );
              PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &p );
              MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &p );
            }
#endif
          }

          // �^�b�`�o�[
          ZUKAN_DETAIL_TOUCHBAR_SetType(
              touchbar,
              ZUKAN_DETAIL_TOUCHBAR_TYPE_FORM,
              ZUKAN_DETAIL_TOUCHBAR_DISP_FORM,
              (ZKNDTL_COMMON_GetPokeNum(cmn)>1)?TRUE:FALSE );
          if( work->diff_num >= 3 )
          {
            ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR( touchbar, TRUE );
          }
          else
          {
            ZUKAN_DETAIL_TOUCHBAR_SetVisibleOfFormCurLR( touchbar, FALSE );
          }

          // �^�C�g���o�[
          ZUKAN_DETAIL_HEADBAR_SetType(
              headbar,
              ZUKAN_DETAIL_HEADBAR_TYPE_FORM_COMPARE );  
        }
        break;
      }
    }
    break;
  case STATE_EXCHANGE:
    {
      // �J�ڌ�̏��
      switch(state)
      {
      case STATE_EXCHANGE_IREKAE:
        {
          if( work->irekae_state == IREKAE_STATE_NONE )
          {
            work->irekae_state = IREKAE_STATE_MOVE;
            work->irekae_theta = 0;
          }
          else
          {
            // �������Ȃ�
          }
        }
        break;
      case STATE_EXCHANGE_TO_TOP:
        {
          work->kaisou_curr_end = FALSE;
          work->kaisou_comp_end = FALSE;
        }
        break;
      }
    }
    break;
  case STATE_EXCHANGE_IREKAE:
    {
      // �J�ڌ�̏��
      switch(state)
      {
      case STATE_EXCHANGE:
        {
#ifndef DEF_SCMD_CHANGE
          ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
#endif

#ifdef DEF_SCMD_CHANGE
          if( work->irekae_by_exchange_flag )
          {
            work->irekae_by_exchange_poke_finish = TRUE;
            if( work->irekae_by_exchange_icon_finish )
            {
              work->irekae_by_exchange_flag = FALSE;
              ZUKAN_DETAIL_TOUCHBAR_Unlock( touchbar );
            }
          }
#endif
        }
        break;
      }
    }
    break;
  case STATE_EXCHANGE_TO_TOP:
    {
      // �J�ڌ�̏��
      switch(state)
      {
/*
      case STATE_TOP:
        {
          b_top_exchange = TRUE;

          if( work->diff_num >= 2 )
          {
            VecFx32 p;

            MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &poke_pos[POKE_CURR_POS_CENTER] );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &poke_pos[POKE_CURR_POS_CENTER] );
      
            PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &p );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &p );
          }

          // �^�b�`�o�[
          ZUKAN_DETAIL_TOUCHBAR_SetType(
              touchbar,
              ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL,
              ZUKAN_DETAIL_TOUCHBAR_DISP_FORM );
          {
            GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
            ZUKAN_DETAIL_TOUCHBAR_SetCheckFlipOfGeneral(
                touchbar,
                GAMEDATA_GetShortCut( gamedata, SHORTCUT_ID_ZUKAN_FORM ) );
          }
          // �^�C�g���o�[
          ZUKAN_DETAIL_HEADBAR_SetType(
              headbar,
              ZUKAN_DETAIL_HEADBAR_TYPE_FORM );

          // �����o���p�֐��𗘗p���Ĉʒu�ݒ�
          if( work->diff_num >= 2 ) 
          {
            work->oshidashi_direct = OSHIDASHI_DIRECT_R_TO_L;
            Zukan_Detail_Form_OshidashiSetPosCompareForm( param, work, cmn );
          }
        }
        break;
*/
      case STATE_EXCHANGE_TO_TOP_TOUCHBAR_CHANGE:
        {
          // �^�b�`�o�[
          ZUKAN_DETAIL_TOUCHBAR_SetVisibleAll(
              touchbar,
              FALSE );
        }
        break;
      }
    }
    break;
  case STATE_EXCHANGE_TO_TOP_TOUCHBAR_CHANGE:
    {
      // �J�ڌ�̏��
      switch(state)
      {
      case STATE_TOP:
        {
          b_top_exchange = TRUE;

          if( work->diff_num >= 2 )
          {
            VecFx32 p;

#ifndef DEF_MINIMUM_LOAD
            MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &poke_pos[POKE_CURR_POS_CENTER] );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &poke_pos[POKE_CURR_POS_CENTER] );
      
            PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &p );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &p );
#else
            if( work->is_poke_front )
            {
              MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &poke_pos[POKE_CURR_POS_CENTER] );
              PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );
              MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &p );
            }
            else
            {
              MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &poke_pos[POKE_CURR_POS_CENTER] );
              PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &p );
              MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &p );
            }
#endif
          }

          // �^�b�`�o�[
          ZUKAN_DETAIL_TOUCHBAR_SetType(
              touchbar,
              ZUKAN_DETAIL_TOUCHBAR_TYPE_GENERAL,
              ZUKAN_DETAIL_TOUCHBAR_DISP_FORM,
              (ZKNDTL_COMMON_GetPokeNum(cmn)>1)?TRUE:FALSE ); 
          {
            GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
            ZUKAN_DETAIL_TOUCHBAR_SetCheckFlipOfGeneral(
                touchbar,
                GAMEDATA_GetShortCut( gamedata, SHORTCUT_ID_ZUKAN_FORM ) );
          }
          // �^�C�g���o�[
          ZUKAN_DETAIL_HEADBAR_SetType(
              headbar,
              ZUKAN_DETAIL_HEADBAR_TYPE_FORM );

          // �����o���p�֐��𗘗p���Ĉʒu�ݒ�
          if( /*work->state == STATE_TOP &&*/ work->diff_num >= 2 ) 
          {
            work->oshidashi_direct = OSHIDASHI_DIRECT_R_TO_L;
            Zukan_Detail_Form_OshidashiSetPosCompareForm( param, work, cmn );
          }
        }
        break;
      }
    }
    break;
  }

  // �J�ڌ�̏��
  work->state = state;

  if( b_top_exchange )
  {
    // �e�L�X�g
    // �J�ڂ��ꂽ��ԂŁA�e�L�X�g���X�N���[�������āA��r�t�H�����̃e�L�X�g������or����
    Zukan_Detail_Form_ScrollPokeText( param, work, cmn );
    Zukan_Detail_Form_WritePokeCompText( param, work, cmn );

    // �e�L�X�g�ɍ��킹�āA�t�H�������̔w�ʃt�B�[���h��p�ӂ���
    Zukan_Detail_Form_ObjFieldSetup( param, work, cmn );

    // �ύX���ꂽ��ԗp�ɁA�X�N���[���o�[��p�ӂ���
    Zukan_Detail_Form_ObjBarSetup( param, work, cmn );

    // �ύX���ꂽ��ԗp�ɁA�g�b�v��ʂŃt�H������؂�ւ��邽�߂̖���p�ӂ���
    Zukan_Detail_Form_ObjButtonArrowSetup( param, work, cmn );

    // ���͉s��{�^�������������ɖ߂�
    work->push_button = BUTTON_OBJ_NONE;
    ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, TRUE );
  }
}

//-------------------------------------
/// OBJ  // �O��{�^���A�Đ��{�^���A�X�N���[���o�[�A�o�[�J�[�\��  // �t�H�������̔w�ʃt�B�[���h
//=====================================
static void Zukan_Detail_Form_ObjBaseCreate( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, param->heap_id );

    work->obj_res[OBJ_RES_BUTTON_BAR_NCL] = GFL_CLGRP_PLTT_RegisterEx( 
                                     handle,
                                     NARC_zukan_gra_info_info_obj_NCLR,
                                     CLSYS_DRAW_MAIN,
                                     OBJ_PAL_POS_M_BUTTON_BAR*0x20,
                                     0,
                                     OBJ_PAL_NUM_M_BUTTON_BAR,
                                     param->heap_id );	
    work->obj_res[OBJ_RES_BUTTON_BAR_NCG] = GFL_CLGRP_CGR_Register(
                                     handle,
                                     NARC_zukan_gra_info_info_obj_NCGR,
                                     FALSE,
                                     CLSYS_DRAW_MAIN,
                                     param->heap_id );
    work->obj_res[OBJ_RES_BUTTON_BAR_NCE] = GFL_CLGRP_CELLANIM_Register(
                                     handle,
                                     NARC_zukan_gra_info_info_obj_NCER,
                                     NARC_zukan_gra_info_info_obj_NANR,
                                     param->heap_id );

    work->obj_res[OBJ_RES_FIELD_NCL] = GFL_CLGRP_PLTT_RegisterEx( 
                                     handle,
                                     NARC_zukan_gra_info_formewin_obj_NCLR,
                                     CLSYS_DRAW_MAIN,
                                     OBJ_PAL_POS_M_FIELD*0x20,
                                     0,
                                     OBJ_PAL_NUM_M_FIELD,
                                     param->heap_id );	
    work->obj_res[OBJ_RES_FIELD_NCG] = GFL_CLGRP_CGR_Register(
                                     handle,
                                     NARC_zukan_gra_info_formewin_obj_NCGR,
                                     FALSE,
                                     CLSYS_DRAW_MAIN,
                                     param->heap_id );
    work->obj_res[OBJ_RES_FIELD_NCE] = GFL_CLGRP_CELLANIM_Register(
                                     handle,
                                     NARC_zukan_gra_info_formewin_obj_NCER,
                                     NARC_zukan_gra_info_formewin_obj_NANR,
                                     param->heap_id );

    work->obj_res[OBJ_RES_ARROW_NCL] = GFL_CLGRP_PLTT_RegisterEx( 
                                     handle,
                                     NARC_zukan_gra_info_mapwin_obj_NCLR,
                                     CLSYS_DRAW_MAIN,
                                     OBJ_PAL_POS_M_ARROW*0x20,
                                     0,
                                     OBJ_PAL_NUM_M_ARROW,
                                     param->heap_id );	
    work->obj_res[OBJ_RES_ARROW_NCG] = GFL_CLGRP_CGR_Register(
                                     handle,
                                     NARC_zukan_gra_info_mapwin_obj_NCGR,
                                     FALSE,
                                     CLSYS_DRAW_MAIN,
                                     param->heap_id );
    work->obj_res[OBJ_RES_ARROW_NCE] = GFL_CLGRP_CELLANIM_Register(
                                     handle,
                                     NARC_zukan_gra_info_mapwin_obj_NCER,
                                     NARC_zukan_gra_info_mapwin_obj_NANR,
                                     param->heap_id );

    GFL_ARC_CloseDataHandle( handle );
  }

  // CLWK�쐬
  {
    u8 i;
    GFL_CLWK_DATA cldata;

    for( i=0; i<OBJ_MAX; i++ )
    {
      GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
      cldata.pos_x      = obj_setup_info[i][0];
      cldata.pos_y      = obj_setup_info[i][1];
      cldata.anmseq     = obj_setup_info[i][2];
      cldata.softpri    = obj_setup_info[i][3];
      cldata.bgpri      = obj_setup_info[i][4];

      work->obj_clwk[i] = GFL_CLACT_WK_Create(
                             work->clunit,
                             work->obj_res[obj_setup_info[i][5]], work->obj_res[obj_setup_info[i][6]], work->obj_res[obj_setup_info[i][7]],
                             &cldata,
                             CLSYS_DEFREND_MAIN,
                             param->heap_id );

      GFL_CLACT_WK_SetAutoAnmFlag( work->obj_clwk[i], TRUE );
      GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[i], FALSE );
      GFL_CLACT_WK_SetObjMode( work->obj_clwk[i], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    }
  }

  // �{�^��
  {
    work->button[BUTTON_OBJ_FRONT_BACK].set_pos_x              = 8*8;
    work->button[BUTTON_OBJ_FRONT_BACK].set_pos_y              = 19*8;
    work->button[BUTTON_OBJ_FRONT_BACK].set_ncg                = OBJ_RES_BUTTON_BAR_NCG;
    work->button[BUTTON_OBJ_FRONT_BACK].set_ncl                = OBJ_RES_BUTTON_BAR_NCL;
    work->button[BUTTON_OBJ_FRONT_BACK].set_nce                = OBJ_RES_BUTTON_BAR_NCE;
    work->button[BUTTON_OBJ_FRONT_BACK].pos_x                  = 8*8;
    work->button[BUTTON_OBJ_FRONT_BACK].pos_y                  = 19*8;
    work->button[BUTTON_OBJ_FRONT_BACK].size_x                 = 8*8;
    work->button[BUTTON_OBJ_FRONT_BACK].size_y                 = 8*2;
    work->button[BUTTON_OBJ_FRONT_BACK].anmseq_active          = 24;
    work->button[BUTTON_OBJ_FRONT_BACK].anmseq_push            = 26;
    work->button[BUTTON_OBJ_FRONT_BACK].key                    = PAD_BUTTON_SELECT;
    work->button[BUTTON_OBJ_FRONT_BACK].se                     = SEQ_SE_DECIDE1;
    work->button[BUTTON_OBJ_FRONT_BACK].state                  = BUTTON_STATE_ACTIVE;
    work->button[BUTTON_OBJ_FRONT_BACK].clwk                   = NULL;

    work->button[BUTTON_OBJ_PLAY].set_pos_x              = 16*8;
    work->button[BUTTON_OBJ_PLAY].set_pos_y              = 19*8;
    work->button[BUTTON_OBJ_PLAY].set_ncg                = OBJ_RES_BUTTON_BAR_NCG;
    work->button[BUTTON_OBJ_PLAY].set_ncl                = OBJ_RES_BUTTON_BAR_NCL;
    work->button[BUTTON_OBJ_PLAY].set_nce                = OBJ_RES_BUTTON_BAR_NCE;
    work->button[BUTTON_OBJ_PLAY].pos_x                  = 16*8;
    work->button[BUTTON_OBJ_PLAY].pos_y                  = 19*8;
    work->button[BUTTON_OBJ_PLAY].size_x                 = 8*8;
    work->button[BUTTON_OBJ_PLAY].size_y                 = 8*2;
    work->button[BUTTON_OBJ_PLAY].anmseq_active          = 23;
    work->button[BUTTON_OBJ_PLAY].anmseq_push            = 25;
    work->button[BUTTON_OBJ_PLAY].key                    = PAD_BUTTON_START;
    work->button[BUTTON_OBJ_PLAY].se                     = SEQ_SE_DECIDE1;
    work->button[BUTTON_OBJ_PLAY].state                  = BUTTON_STATE_ACTIVE;
    work->button[BUTTON_OBJ_PLAY].clwk                   = NULL;

    work->button[BUTTON_OBJ_ARROW_L].set_pos_x              = 128;
    work->button[BUTTON_OBJ_ARROW_L].set_pos_y              = 96;
    work->button[BUTTON_OBJ_ARROW_L].set_ncg                = OBJ_RES_ARROW_NCG;
    work->button[BUTTON_OBJ_ARROW_L].set_ncl                = OBJ_RES_ARROW_NCL;
    work->button[BUTTON_OBJ_ARROW_L].set_nce                = OBJ_RES_ARROW_NCE;
    work->button[BUTTON_OBJ_ARROW_L].pos_x                  = 0;
    work->button[BUTTON_OBJ_ARROW_L].pos_y                  = 0;
    work->button[BUTTON_OBJ_ARROW_L].size_x                 = 24;
    work->button[BUTTON_OBJ_ARROW_L].size_y                 = 16;
    work->button[BUTTON_OBJ_ARROW_L].anmseq_active          = 4;
    work->button[BUTTON_OBJ_ARROW_L].anmseq_push            = 5;
    work->button[BUTTON_OBJ_ARROW_L].key                    = PAD_BUTTON_L;
    work->button[BUTTON_OBJ_ARROW_L].se                     = SEQ_SE_SELECT3;
    work->button[BUTTON_OBJ_ARROW_L].state                  = BUTTON_STATE_ACTIVE;
    work->button[BUTTON_OBJ_ARROW_L].clwk                   = NULL;

    work->button[BUTTON_OBJ_ARROW_R].set_pos_x              = 228;
    work->button[BUTTON_OBJ_ARROW_R].set_pos_y              = 96;
    work->button[BUTTON_OBJ_ARROW_R].set_ncg                = OBJ_RES_ARROW_NCG;
    work->button[BUTTON_OBJ_ARROW_R].set_ncl                = OBJ_RES_ARROW_NCL;
    work->button[BUTTON_OBJ_ARROW_R].set_nce                = OBJ_RES_ARROW_NCE;
    work->button[BUTTON_OBJ_ARROW_R].pos_x                  = 232;
    work->button[BUTTON_OBJ_ARROW_R].pos_y                  = 0;
    work->button[BUTTON_OBJ_ARROW_R].size_x                 = 24;
    work->button[BUTTON_OBJ_ARROW_R].size_y                 = 16;
    work->button[BUTTON_OBJ_ARROW_R].anmseq_active          = 2;
    work->button[BUTTON_OBJ_ARROW_R].anmseq_push            = 3;
    work->button[BUTTON_OBJ_ARROW_R].key                    = PAD_BUTTON_R;
    work->button[BUTTON_OBJ_ARROW_R].se                     = SEQ_SE_SELECT3;
    work->button[BUTTON_OBJ_ARROW_R].state                  = BUTTON_STATE_ACTIVE;
    work->button[BUTTON_OBJ_ARROW_R].clwk                   = NULL;
  }

  // �{�^��CLWK�쐬
  {
    u8 i;
    GFL_CLWK_DATA cldata;

    for( i=0; i<BUTTON_OBJ_MAX; i++ )
    {
      GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
      cldata.pos_x      = work->button[i].set_pos_x;
      cldata.pos_y      = work->button[i].set_pos_y;
      cldata.anmseq     = work->button[i].anmseq_active;
      cldata.softpri    = 0;
      cldata.bgpri      = BG_FRAME_PRI_M_REAR;

      work->button[i].clwk = GFL_CLACT_WK_Create(
                             work->clunit,
                             work->obj_res[work->button[i].set_ncg], work->obj_res[work->button[i].set_ncl], work->obj_res[work->button[i].set_nce],
                             &cldata,
                             CLSYS_DEFREND_MAIN,
                             param->heap_id );

      GFL_CLACT_WK_SetAutoAnmFlag( work->button[i].clwk, TRUE );
      GFL_CLACT_WK_SetObjMode( work->button[i].clwk, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    }
  }

  work->push_button = BUTTON_OBJ_NONE;
}
static void Zukan_Detail_Form_ObjBaseDelete( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �{�^��CLWK�쐬
  {
    u8 i;
    for( i=0; i<BUTTON_OBJ_MAX; i++ )
    {
      GFL_CLACT_WK_Remove( work->button[i].clwk );
    }
  }

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
    GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_RES_BUTTON_BAR_NCL] );
    GFL_CLGRP_CGR_Release( work->obj_res[OBJ_RES_BUTTON_BAR_NCG] );
    GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_RES_BUTTON_BAR_NCE] );

    GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_RES_FIELD_NCL] );
    GFL_CLGRP_CGR_Release( work->obj_res[OBJ_RES_FIELD_NCG] );
    GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_RES_FIELD_NCE] );

    GFL_CLGRP_PLTT_Release( work->obj_res[OBJ_RES_ARROW_NCL] );
    GFL_CLGRP_CGR_Release( work->obj_res[OBJ_RES_ARROW_NCG] );
    GFL_CLGRP_CELLANIM_Release( work->obj_res[OBJ_RES_ARROW_NCE] );
  }
}

static BUTTON_OBJ Zukan_Detail_Form_ObjButtonCheckPush( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �Ăяo�����ɂ́A�������u�Ԃɂ�work->push_button�Ɠ������̂�Ԃ��A�����łȂ��Ƃ���BUTTON_OBJ_NONE��BUTTON_OBJ_IGNORE��Ԃ��B
  
  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);
  
  u8 i;
  u32 x, y;
  BUTTON_OBJ curr_push_button = BUTTON_OBJ_NONE;  // ���������{�^��  // �߂�l
  int tk;  // Touch or Key

  // �L�[
  if( curr_push_button == BUTTON_OBJ_NONE )
  {
    for( i=0; i<BUTTON_OBJ_MAX; i++ )
    {
      BOOL on_off = GFL_CLACT_WK_GetDrawEnable( work->button[i].clwk );
      if( on_off )
      {
        if( GFL_UI_KEY_GetTrg() & work->button[i].key )
        {
          tk = GFL_APP_END_KEY;
          curr_push_button = i;
          break;
        }
        else
        {
          // �L�[���s�[�g�ɑΉ������{�^��
          if( i == BUTTON_OBJ_ARROW_L || i == BUTTON_OBJ_ARROW_R )
          {
            if( GFL_UI_KEY_GetRepeat() & work->button[i].key )
            {
              tk = GFL_APP_END_KEY;
              curr_push_button = i;
              break;
            }
          }
        }
      }
    }
  }

  // �^�b�`
  if( curr_push_button == BUTTON_OBJ_NONE )
  {
    if( GFL_UI_TP_GetPointTrg( &x, &y ) )
    {
      for( i=0; i<BUTTON_OBJ_MAX; i++ )
      {
        BOOL on_off = GFL_CLACT_WK_GetDrawEnable( work->button[i].clwk );
        if( on_off )
        {
          if(    work->button[i].pos_x <= x && x < work->button[i].pos_x + work->button[i].size_x
              && work->button[i].pos_y <= y && y < work->button[i].pos_y + work->button[i].size_y )
          {
            tk = GFL_APP_END_TOUCH;
            curr_push_button = i;
            break;
          }
        }
      }
    }
  }

  if( curr_push_button != BUTTON_OBJ_NONE )
  {
    // 2�̃{�^���͓����ɉ����Ȃ��A���A�������̃{�^���͉����Ȃ�
    if( work->push_button != BUTTON_OBJ_NONE )
    {
      curr_push_button = BUTTON_OBJ_IGNORE;
    }
    else
    {
      // �{�^�������m��
      GFL_UI_SetTouchOrKey( tk );
      ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, FALSE );
      
      work->button[curr_push_button].state = BUTTON_STATE_PUSH_START;
      GFL_CLACT_WK_SetAnmSeq( work->button[curr_push_button].clwk, work->button[curr_push_button].anmseq_push );
      PMSND_PlaySE( work->button[curr_push_button].se );

      // �A�j���[�V���������킹�Ă����K�v������{�^��
      {
        BUTTON_OBJ curr_pair_button = BUTTON_OBJ_NONE;
        if( curr_push_button == BUTTON_OBJ_ARROW_L )
        {
          curr_pair_button = BUTTON_OBJ_ARROW_R;
        }
        else if( curr_push_button == BUTTON_OBJ_ARROW_R )
        {
          curr_pair_button = BUTTON_OBJ_ARROW_L;
        }
        if( curr_pair_button != BUTTON_OBJ_NONE )
        {
          GFL_CLACT_WK_SetAutoAnmFlag( work->button[curr_pair_button].clwk, FALSE );
          GFL_CLACT_WK_SetAnmFrame( work->button[curr_pair_button].clwk, 0 );
        }
      }

      work->push_button = curr_push_button;
    }
  }

  return curr_push_button; 
}

static void Zukan_Detail_Form_ObjButtonArrowSetup( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // ���݂̏�ԂŁA�g�b�v��ʂŃt�H������؂�ւ��邽�߂̖�����������

  BOOL disp = FALSE;
  switch(work->state)
  {
  case STATE_TOP:
    {
      if( work->diff_num >= 2 )
      {
        disp = TRUE;  // �\������
      }
    }
    break;
  case STATE_EXCHANGE:
    {
      // �\�����Ȃ�
    }
    break;
  }

  if( disp )
  {
    GFL_CLACT_WK_SetDrawEnable( work->button[BUTTON_OBJ_ARROW_L].clwk, TRUE );
    GFL_CLACT_WK_SetDrawEnable( work->button[BUTTON_OBJ_ARROW_R].clwk, TRUE );
  }
  else
  {
    GFL_CLACT_WK_SetDrawEnable( work->button[BUTTON_OBJ_ARROW_L].clwk, FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->button[BUTTON_OBJ_ARROW_R].clwk, FALSE );
  }
}

static void Zukan_Detail_Form_ObjButtonMain( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);

  if( work->push_button == BUTTON_OBJ_NONE )
  {
    return;
  }

  {
    switch( work->button[work->push_button].state )
    {
    case BUTTON_STATE_ACTIVE:
      {
        // �������Ȃ�
      }
      break;
    case BUTTON_STATE_PUSH_START:  // ���̊֐��̌Ăяo���ʒu�ɂ���ẮA���̏�Ԃɂ�1�t���[�����Ȃ��Ă��Ȃ������B
      {
        work->button[work->push_button].state = BUTTON_STATE_PUSH_ANIME;
      }
      break;
    case BUTTON_STATE_PUSH_ANIME:
      {
        if( !GFL_CLACT_WK_CheckAnmActive( work->button[work->push_button].clwk ) )
        {
          work->button[work->push_button].state = BUTTON_STATE_PUSH_END;
        } 
      }
      break;
    case BUTTON_STATE_PUSH_END:
      {
        GFL_CLACT_WK_SetAnmSeq( work->button[work->push_button].clwk, work->button[work->push_button].anmseq_active );
        work->button[work->push_button].state = BUTTON_STATE_ACTIVE;

        // �A�j���[�V���������킹�Ă����K�v������{�^��
        {
          BUTTON_OBJ pair_button = BUTTON_OBJ_NONE;
          if( work->push_button == BUTTON_OBJ_ARROW_L )
          {
            pair_button = BUTTON_OBJ_ARROW_R;
          }
          else if( work->push_button == BUTTON_OBJ_ARROW_R )
          {
            pair_button = BUTTON_OBJ_ARROW_L;
          }
          if( pair_button != BUTTON_OBJ_NONE )
          {
            if( work->button[pair_button].state == BUTTON_STATE_ACTIVE )
            {
              GFL_CLACT_WK_SetAutoAnmFlag( work->button[pair_button].clwk, TRUE );
              GFL_CLACT_WK_SetAnmSeq( work->button[pair_button].clwk, work->button[pair_button].anmseq_active );
            }
          }
        }

        work->push_button = BUTTON_OBJ_NONE;

        if(    work->state == STATE_TOP
            || work->state == STATE_EXCHANGE )  // �ϓ����̏�Ԃ̂Ƃ��́A���̕ϓ��������܂��Ă���A�N�e�B�u�ɖ߂�
        {
          ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, TRUE );
        }
      }
      break;
    }
  }
}

static void Zukan_Detail_Form_ObjFieldSetup( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  GFL_CLACTPOS pos;
  u8           anmseq;
  BOOL         field_curr = TRUE;
  BOOL         field_comp = FALSE;

  switch(work->state)
  {
  case STATE_TOP:
    {
      field_curr = TRUE;
      field_comp = FALSE;
    }
    break;
  case STATE_EXCHANGE:
    {
      if( work->diff_num >= 2 )
      {
        field_curr = TRUE;
        field_comp = TRUE;
      }
      else
      {
        field_curr = TRUE;
        field_comp = FALSE;
      }
    }
    break;
  }

  if( field_curr && field_comp )
  {
    pos.x = 0;  pos.y = 0; 
    GFL_CLACT_WK_SetPos( work->obj_clwk[OBJ_FIELD_CURR], &pos, CLSYS_DEFREND_MAIN );
    pos.x = 16*8;  pos.y = 0; 
    GFL_CLACT_WK_SetPos( work->obj_clwk[OBJ_FIELD_COMP], &pos, CLSYS_DEFREND_MAIN );
  }
  else if( field_curr )
  {
    pos.x = 8*8;  pos.y = 0; 
    GFL_CLACT_WK_SetPos( work->obj_clwk[OBJ_FIELD_CURR], &pos, CLSYS_DEFREND_MAIN );
  }

  // field_curr
  {
    anmseq = ( work->diff_info_list[work->diff_curr_no].color_diff == COLOR_DIFF_SPECIAL ) ? (1):(0);
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_FIELD_CURR], TRUE );
    GFL_CLACT_WK_SetAnmSeq( work->obj_clwk[OBJ_FIELD_CURR], anmseq );
  }

  if( field_comp )
  {
    anmseq = ( work->diff_info_list[work->diff_comp_no].color_diff == COLOR_DIFF_SPECIAL ) ? (1):(0);
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_FIELD_COMP], TRUE );
    GFL_CLACT_WK_SetAnmSeq( work->obj_clwk[OBJ_FIELD_COMP], anmseq );
  }
  else
  {
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_FIELD_COMP], FALSE );
  }
}

static void Zukan_Detail_Form_ObjBarSetup( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // ���݂̏�ԂŃX�N���[���o�[����������

  BOOL disp = FALSE;
  switch(work->state)
  {
  case STATE_TOP:
    {
      // �\�����Ȃ�
    }
    break;
  case STATE_EXCHANGE:
    {
      if( work->diff_num >= 3 )
      {
        disp = TRUE;  // �\������
      }
      else
      {
        // �\�����Ȃ�
      }
    }
    break;
  }

  if( disp )
  {
    u8  pos_x_center;
    GFL_CLACTPOS pos;
    u16 no = Zukan_Detail_Form_GetNoExceptOne( param, work, cmn,
                 work->diff_curr_no, work->diff_comp_no );
    Zukan_Detail_Form_ObjBarGetPosX( work->diff_num -1, no, NULL, &pos_x_center, NULL );
    pos.x = pos_x_center;
    pos.y = BAR_CURSOR_POS_Y;
    GFL_CLACT_WK_SetPos( work->obj_clwk[OBJ_BAR_CURSOR], &pos, CLSYS_DEFREND_MAIN );
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BAR_RANGE], TRUE );
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BAR_CURSOR], TRUE );
    // ������ 
    work->bar_cursor_move_by_key    = FALSE;
#ifdef DEF_SCMD_CHANGE
    work->change_form_by_cur_l_r_flag = FALSE;
#endif
    work->bar_cursor_move_by_touch  = FALSE;
  }
  else
  {
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BAR_RANGE], FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk[OBJ_BAR_CURSOR], FALSE );
  }
}
static void Zukan_Detail_Form_ObjBarMain( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  BOOL ret = FALSE;

#ifndef DEF_SCMD_CHANGE
  if(    work->state == STATE_EXCHANGE
      && work->input_enable
      && work->push_button == BUTTON_OBJ_NONE )
#else
   if(    (    work->state == STATE_EXCHANGE
            && work->input_enable
            && work->push_button == BUTTON_OBJ_NONE )
       || ( work->change_form_by_cur_l_r_flag       ) )
#endif
  {
    // �^�b�`�p�����̏ꍇ�́A���ꂪ�ŗD��ƂȂ�
    // ������A�^�b�`�p�����̏ꍇ�́Abar_cursor_move_by_key��TRUE�ɂȂ�Ȃ��B
    // bar_cursor_move_by_key��TRUE�ɂȂ��Ă���Ƃ������Ƃ́A�^�b�`���Ă��Ȃ��Ƃ��ɃL�[���͂��������Ƃ������ƁB
    // �����炻�̂Ƃ��́A�����Еt���Ă���^�b�`�𒲂ׂ�悤�ɂ���B
    
    // �L�[
    if( !ret )
    {
      ret = Zukan_Detail_Form_ObjBarMainKey( param, work, cmn );
    }
    // �^�b�`
    if( !ret )
    {
      ret = Zukan_Detail_Form_ObjBarMainTouch( param, work, cmn );    
    }
  }
}

static BOOL Zukan_Detail_Form_ObjBarMainTouch( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  ZUKAN_DETAIL_TOUCHBAR_WORK*  touchbar = ZKNDTL_COMMON_GetTouchbar(cmn);

  BOOL b_se = FALSE;  // TRUE�̂Ƃ��A�^�b�`����炷
  u32 x, y;
  
  // �^�b�`�p����
  if( work->bar_cursor_move_by_touch )
  {
    if( GFL_UI_TP_GetPointCont( &x, &y ) )
    {
      // �󋵕ύX�Ȃ�
    }
    else
    {
      work->bar_cursor_move_by_touch = FALSE;
      ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, TRUE );
    }
  }
  // �V���ɃX�N���[���o�[���^�b�`���������o����
  else
  {
    if( GFL_UI_TP_GetPointTrg( &x, &y ) )
    {
      if(    BAR_RANGE_TOUCH_X_MIN<=x&&x<=BAR_RANGE_TOUCH_X_MAX
          && BAR_RANGE_TOUCH_Y_MIN<=y&&y<=BAR_RANGE_TOUCH_Y_MAX )
      {
        work->bar_cursor_move_by_touch = TRUE;
        ZUKAN_DETAIL_TOUCHBAR_SetUserActiveWhole( touchbar, FALSE );
        b_se = TRUE;
      }
    }
  }

  // �^�b�`���Ă���̂ŁA�o�[�J�[�\���𓮂����A�t�H�������ύX����
  if( work->bar_cursor_move_by_touch )
  {
    u16 diff_comp_no_prev = work->diff_comp_no;
    u16 no_except_curr;
  
    GFL_CLACTPOS pos;
    x = MATH_CLAMP( x, BAR_CURSOR_POS_X_MIN, BAR_CURSOR_POS_X_MAX );
    pos.x = x;
    pos.y = BAR_CURSOR_POS_Y;
    GFL_CLACT_WK_SetPos( work->obj_clwk[OBJ_BAR_CURSOR], &pos, CLSYS_DEFREND_MAIN );

    no_except_curr = Zukan_Detail_Form_ObjBarGetNo( work->diff_num-1, (u8)x );

    work->diff_comp_no = Zukan_Detail_Form_GetDiffInfoListIdx( param, work, cmn,
                                                               work->diff_curr_no, no_except_curr );

    // ��r�t�H�����ύX
    if( diff_comp_no_prev != work->diff_comp_no )
    {
      Zukan_Detail_Form_ChangeCompareForm( param, work, cmn );
      b_se = TRUE;
    }
  }

  if( b_se )
  {
    PMSND_PlaySE( SEQ_SE_SYS_06 );
  }
 
  if( work->bar_cursor_move_by_touch )
  {
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
  }

  return work->bar_cursor_move_by_touch;
}
static BOOL Zukan_Detail_Form_ObjBarMainKey( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  BOOL ret = FALSE;  // �L�[�ɂ����͂���������TRUE
  if( work->bar_cursor_move_by_key )
  {
    u8  pos_x_center;
    GFL_CLACTPOS pos;
    u16 no = Zukan_Detail_Form_GetNoExceptOne( param, work, cmn,
                 work->diff_curr_no, work->diff_comp_no );
    Zukan_Detail_Form_ObjBarGetPosX( work->diff_num -1, no, NULL, &pos_x_center, NULL );
    pos.x = pos_x_center;
    pos.y = BAR_CURSOR_POS_Y;
    GFL_CLACT_WK_SetPos( work->obj_clwk[OBJ_BAR_CURSOR], &pos, CLSYS_DEFREND_MAIN );
    ret = TRUE;
  } 
  return ret;
}
static void Zukan_Detail_Form_ObjBarGetPosX( u16 num, u16 no, u8* a_pos_x_min, u8* a_pos_x_center, u8* a_pos_x_max )
{
  // num�̃f�[�^����no�Ԗ�(0<=no<num)�̃f�[�^���w���X�N���[���o�[�̈ʒu�𓾂�
  // ���̈ʒu��pos_x_min<=  <pos_x_max�ƂȂ�
  // pos_x_min<=pos_x_center<pox_x_max
  // no==0�̂Ƃ��Apos_x_center=pos_x_min
  // no==num-1�̂Ƃ��Apos_x_center=pos_x_max

  u8 pos_x_min, pos_x_center, pos_x_max;

  pos_x_min = ( BAR_CURSOR_POS_X_MAX - BAR_CURSOR_POS_X_MIN +1 ) * no / num + BAR_CURSOR_POS_X_MIN;
  pos_x_max = ( BAR_CURSOR_POS_X_MAX - BAR_CURSOR_POS_X_MIN +1 ) * (no +1) / num + BAR_CURSOR_POS_X_MIN;

  if( no == 0 )                     pos_x_center = pos_x_min;
  else if( no == num -1 )           pos_x_center = pos_x_max -1;
  else if( pos_x_min >= pos_x_max ) pos_x_center = pos_x_min;
  else                              pos_x_center = ( pos_x_min + pos_x_max -1 ) / 2;

  if( a_pos_x_min )    *a_pos_x_min    = pos_x_min;
  if( a_pos_x_max )    *a_pos_x_max    = pos_x_max;
  if( a_pos_x_center ) *a_pos_x_center = pos_x_center;
}

static u16 Zukan_Detail_Form_ObjBarGetNo( u16 num, u8 x )
{
  // ���Wx��num�̃f�[�^����no�Ԗ�(0<=no<num)�̃f�[�^���𓾂�
  // x���͈͂̍ŏ���菬�����Ƃ���0��Ԃ��Ax���͈͂̍ő���傫���Ƃ���num-1��Ԃ�
  
  u16 no;
  u8  pos_x_min, pos_x_center, pos_x_max;
  for( no=0; no<num; no++ )
  {
    Zukan_Detail_Form_ObjBarGetPosX( num, no, &pos_x_min, &pos_x_center, &pos_x_max );
    if( no == 0 )
    {
      if( x < pos_x_max )
      {
        break;
      }
    }
    else if( no == num-1 )
    {
      //if( pos_x_min <= x )  // ���������܂�break�����ɗ����̂Ȃ�A���ꂵ���Ȃ��̂Ŗ�������
      {
        break;
      }
    }
    else
    {
      if( pos_x_min <= x && x < pos_x_max )
      {
        break;
      }
    }
  }

  return no;
}

static u16 Zukan_Detail_Form_GetNoExceptOne( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                             u16 except_idx, u16 target_idx )
{
  // diff_info_list[target_idx]�ƂȂ�target_idx��diff_info_list�����Ԗڂ������߂�
  // �������Adiff_info_list[except_idx]�ƂȂ�except_idx�͏����ĉ��Ԗڂ��𐔂���
  // �����Atarget_idx�̂܂܂��Atarget_idx-1�ƂȂ�

  u16 ret_no = 0;
  u16 i;
  for( i=0; i<work->diff_num; i++ )
  {
    if( i == except_idx )
    {
      // �������Ȃ�
    }
    else if( i== target_idx )
    {
      break;
    }
    else
    {
      ret_no++;
    }
  }
  return ret_no;
}

static u16 Zukan_Detail_Form_GetDiffInfoListIdx( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn,
                                                 u16 except_idx, u16 no )
{
  // no�Ԗڂ̃f�[�^��diff_info_list�̉��Ԗڂ̓Y�����ɓ����邩�𓾂�
  // �������Adiff_info_list[except_idx]�ƂȂ�except_idx�͏�����no�Ԗڂł���
  // �����A�߂�lret_idx��ret_idx=no��ret_idx=no+1�ƂȂ�diff_info_list[ret_idx]�ł���

  u16 ret_idx = 0;
  u16 no_count = 0;
  u16 i;
  for( i=0; i<work->diff_num; i++ )
  {
    if( i == except_idx )
    {
      // �������Ȃ�
    }
    else
    {
      if( no_count == no )
      {
        ret_idx = i;
        break;
      }
      no_count++;
    }
  }
  return ret_idx;
}

//-------------------------------------
/// ����ւ�
//=====================================
static void Zukan_Detail_Form_IrekaeMain( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->irekae_state == IREKAE_STATE_MOVE )
  {
    const f32  left_x  =  -64.0f;
    const f32  right_x =   64.0f;
    const f32  base_y  =  -24.0f;
    const f32  height  =   24.0f;
    const f32  curr_z  = -12800.0f;  // ���ɕ`�悳���悤�ɉ��̂ق��֔z�u
    const f32  comp_z  =  128.0f;    // �O�ɕ`�悳���悤�Ɏ�O�̂ق��֔z�u
    fx16 s_fx16;
    f32  s;

    work->irekae_theta += IREKAE_THETA_ADD;
    if( work->irekae_theta >= IREKAE_THETA_MAX )
    {
      work->irekae_theta = IREKAE_THETA_MAX;
      work->irekae_state = IREKAE_STATE_NONE;

      Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_EXCHANGE );


        if( work->diff_num >= 2 )
        {
          u16 no;
          MCSS_WORK* mw;
          POKE_MCSS_CALL_BACK_DATA* pmcbd;

#ifndef DEF_MINIMUM_LOAD
          // �ʒu����ւ�
          MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &poke_pos[POKE_CURR_POS_LEFT] );
          MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &poke_pos[POKE_CURR_POS_LEFT] );
          {
            VecFx32 p;
            PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &p );
            
            MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );
            MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &p );
          }
          
          // �ԍ��A�|�C���^����ւ�
          no = work->diff_curr_no;
          work->diff_curr_no = work->diff_comp_no;
          work->diff_comp_no = no;

          mw = work->poke_mcss_wk[POKE_CURR_F].poke_wk;
          work->poke_mcss_wk[POKE_CURR_F].poke_wk = work->poke_mcss_wk[POKE_COMP_F].poke_wk;
          work->poke_mcss_wk[POKE_COMP_F].poke_wk = mw;

          pmcbd = work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data;
          work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data = work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data;
          work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data = pmcbd;
          work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data->poke_idx = POKE_COMP_F;
          work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data->poke_idx = POKE_CURR_F;

          mw = work->poke_mcss_wk[POKE_CURR_B].poke_wk;
          work->poke_mcss_wk[POKE_CURR_B].poke_wk = work->poke_mcss_wk[POKE_COMP_B].poke_wk;
          work->poke_mcss_wk[POKE_COMP_B].poke_wk = mw;

          pmcbd = work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data;
          work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data = work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data;
          work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data = pmcbd;
          work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data->poke_idx = POKE_COMP_B;
          work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data->poke_idx = POKE_CURR_B;
#else
          // �ʒu����ւ�
          {
            VecFx32 p;
            if( work->is_poke_front )
            {
              MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &poke_pos[POKE_CURR_POS_LEFT] );
              PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &p );
              MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );
            }
            else
            {
              MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &poke_pos[POKE_CURR_POS_LEFT] );
              PokeGetCompareRelativePosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &p );
              MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &p );
            } 
          }
          
          // �ԍ��A�|�C���^����ւ�
          no = work->diff_curr_no;
          work->diff_curr_no = work->diff_comp_no;
          work->diff_comp_no = no;

          if( work->is_poke_front )
          {
            mw = work->poke_mcss_wk[POKE_CURR_F].poke_wk;
            work->poke_mcss_wk[POKE_CURR_F].poke_wk = work->poke_mcss_wk[POKE_COMP_F].poke_wk;
            work->poke_mcss_wk[POKE_COMP_F].poke_wk = mw;

            pmcbd = work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data;
            work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data = work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data;
            work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data = pmcbd;
            work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data->poke_idx = POKE_COMP_F;
            work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data->poke_idx = POKE_CURR_F;
          }
          else
          {
            mw = work->poke_mcss_wk[POKE_CURR_B].poke_wk;
            work->poke_mcss_wk[POKE_CURR_B].poke_wk = work->poke_mcss_wk[POKE_COMP_B].poke_wk;
            work->poke_mcss_wk[POKE_COMP_B].poke_wk = mw;

            pmcbd = work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data;
            work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data = work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data;
            work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data = pmcbd;
            work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data->poke_idx = POKE_COMP_B;
            work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data->poke_idx = POKE_CURR_B;
          }
#endif

          // �|�P�A�C�R���ύX
          Zukan_Detail_Form_ChangePokeicon( param, work, cmn );

          // �e�L�X�g
          // �e�L�X�g��OBJ�ł͂Ȃ�BG�Ȃ̂ŁA����ւ��ł��Ȃ��̂ŊہX��������
          Zukan_Detail_Form_WritePokeCurrText( param, work, cmn );
          Zukan_Detail_Form_WritePokeCompText( param, work, cmn );

          // �e�L�X�g�ɍ��킹�āA�t�H�������̔w�ʃt�B�[���h��p�ӂ���
          Zukan_Detail_Form_ObjFieldSetup( param, work, cmn );

          // �}�ӃZ�[�u�f�[�^�ɉ{�����f�[�^�ݒ�
          {
            u16 monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);

            GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
            ZUKAN_SAVEDATA* zkn_sv = GAMEDATA_GetZukanSave( gamedata );

            u32  sex  = work->diff_info_list[work->diff_curr_no].sex;
            BOOL rare = (work->diff_info_list[work->diff_curr_no].rare==0)?FALSE:TRUE;
            u32  form = work->diff_info_list[work->diff_curr_no].form;

            ZUKANSAVE_SetDrawData(  zkn_sv, monsno, sex, rare, form );

#ifdef DEBUG_KAWADA
            {
              OS_Printf( "ZUKAN_DETAIL_FORM : monsno=%d, diff_no=%d, f=%d, s=%d, r=%d\n", monsno, work->diff_curr_no, form, sex, rare );
            }
#endif
#ifdef DEBUG_KAWADA
            {
              ZUKANSAVE_GetDrawData(  zkn_sv, monsno, &sex, &rare, &form, param->heap_id );
              OS_Printf( "ZUKAN_DETAIL_FORM : monsno=%d, ZUKANSAVE_GetDrawData, f=%d, s=%d, r=%d\n", monsno, form, sex, rare );
            }
#endif
          }

          if( work->diff_num >= 3 )
          {
            work->bar_cursor_move_by_key = TRUE;
          }
        }
   

    }
    else
    {
      s_fx16 = FX_SinIdx(work->irekae_theta);
      s = FX_FX16_TO_F32(s_fx16); 

#ifndef DEF_MINIMUM_LOAD
      // POKE_CURR_
      {
        f32 x = ( right_x - left_x ) * work->irekae_theta / (f32)IREKAE_THETA_MAX + left_x;
        f32 y = height * s + base_y;
        VecFx32 p;
        p.x = FX_F32_TO_FX32(x);
        p.y = FX_F32_TO_FX32(y);
        p.z = FX_F32_TO_FX32(curr_z);
        MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );
        MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &p );
      }
     
      // POKE_COMP_
      {
        f32 x = ( left_x - right_x ) * work->irekae_theta / (f32)IREKAE_THETA_MAX + right_x;
        f32 y = - height * s + base_y;
        VecFx32 p;
        p.x = FX_F32_TO_FX32(x);
        p.y = FX_F32_TO_FX32(y);
        p.z = FX_F32_TO_FX32(comp_z);
        MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &p );
        MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &p );
      }
#else
      // POKE_CURR_
      {
        f32 x = ( right_x - left_x ) * work->irekae_theta / (f32)IREKAE_THETA_MAX + left_x;
        f32 y = height * s + base_y;
        VecFx32 p;
        p.x = FX_F32_TO_FX32(x);
        p.y = FX_F32_TO_FX32(y);
        p.z = FX_F32_TO_FX32(curr_z);
        if( work->is_poke_front )
        {
          MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &p );
        }
        else
        {
          MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &p );
        }
      }
     
      // POKE_COMP_
      {
        f32 x = ( left_x - right_x ) * work->irekae_theta / (f32)IREKAE_THETA_MAX + right_x;
        f32 y = - height * s + base_y;
        VecFx32 p;
        p.x = FX_F32_TO_FX32(x);
        p.y = FX_F32_TO_FX32(y);
        p.z = FX_F32_TO_FX32(comp_z);
        if( work->is_poke_front )
        {
          MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &p );
        }
        else
        {
          MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &p );
        }
      }
#endif
    }
  }
}

//-------------------------------------
/// �����o��
//=====================================
static void Zukan_Detail_Form_OshidashiChangeCompareForm( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  u16 diff_comp_no_prev = work->diff_comp_no;

  if( work->oshidashi_direct == OSHIDASHI_DIRECT_L_TO_R )  // ������E��
  {
    if( work->diff_num >= 3 )
    {
      work->diff_comp_no = work->diff_curr_no + work->diff_num -1;
      work->diff_comp_no %= work->diff_num;
    }
  }
  else  // �E���獶��
  {
    if( work->diff_num >= 3 )
    {
      work->diff_comp_no = work->diff_curr_no +1;
      work->diff_comp_no %= work->diff_num;
    }
  }

#ifndef DEF_MINIMUM_LOAD
  if( work->diff_comp_no != diff_comp_no_prev )
#else
  if(    ( ( (work->is_poke_front)) && (!(work->poke_mcss_wk[POKE_COMP_F].poke_wk)) )
      || ( (!(work->is_poke_front)) && (!(work->poke_mcss_wk[POKE_COMP_B].poke_wk)) )
      || ( work->diff_comp_no != diff_comp_no_prev ) )
#endif
  {
    // �O�̂�j��
    //BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_mcss_wk[POKE_COMP_F].poke_wk )
    //BLOCK_POKE_EXIT( work->mcss_sys_wk, work->poke_mcss_wk[POKE_COMP_B].poke_wk )
    PokeMcssWorkExit( &work->poke_mcss_wk[POKE_COMP_F], work->mcss_sys_wk );
    PokeMcssWorkExit( &work->poke_mcss_wk[POKE_COMP_B], work->mcss_sys_wk );
  
    // ���̂𐶐�
    Zukan_Detail_Form_PokeInitFromDiffInfo( param, work, cmn,
        POKE_COMP_F, POKE_COMP_B, POKE_COMP_RPOS, work->diff_comp_no );
#ifndef DEF_MINIMUM_LOAD
    PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data,
                              POKE_COMP_F, work );
    PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data,
                              POKE_COMP_B, work );
#else
    if( work->is_poke_front )
    {
      PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data,
                                POKE_COMP_F, work );
    }
    else
    {
      PokeMcssCallBackDataInit( work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data,
                                POKE_COMP_B, work );
    }
#endif
  }
  
  // �����o���p�֐��𗘗p���Ĉʒu�ݒ�
  Zukan_Detail_Form_OshidashiSetPosCompareForm( param, work, cmn );
}
static void Zukan_Detail_Form_OshidashiSetPosCompareForm( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  const f32  left_x    =  -256.0f;
  const f32  center_x  =     0.0f;
  const f32  right_x   =   256.0f;
  const f32  base_y    =   -24.0f;
  const f32  base_z    =     0.0f;

  VecFx32 pos_fx32;

  if( work->oshidashi_direct == OSHIDASHI_DIRECT_L_TO_R )  // ������E��
  {
    // ���ɒu��
    pos_fx32.x = FX_F32_TO_FX32(left_x);
    pos_fx32.y = FX_F32_TO_FX32(base_y);
    pos_fx32.z = FX_F32_TO_FX32(base_z);
#ifndef DEF_MINIMUM_LOAD
    MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
    MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
#else
    if( work->is_poke_front )
    {
      MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
    }
    else
    {
      MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
    }
#endif
  }
  else  // �E���獶��
  {
    // �E�ɒu��
    pos_fx32.x = FX_F32_TO_FX32(right_x);
    pos_fx32.y = FX_F32_TO_FX32(base_y);
    pos_fx32.z = FX_F32_TO_FX32(base_z);
#ifndef DEF_MINIMUM_LOAD
    MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
    MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
#else
    if( work->is_poke_front )
    {
      MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
    }
    else
    {
      MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
    }
#endif
  }
}
static void Zukan_Detail_Form_OshidashiMain( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  if( work->oshidashi_state == OSHIDASHI_STATE_MOVE )
  {
    const f32  left_x    =  -256.0f;
    const f32  center_x  =     0.0f;
    const f32  right_x   =   256.0f;
    const f32  base_y    =   -24.0f;
    const f32  base_z    =     0.0f;

    VecFx32 pos_fx32;
    f32     pos_x;
    BOOL    move_end = FALSE;
    
#ifndef DEF_MINIMUM_LOAD
    MCSS_GetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
#else
    if( work->is_poke_front )
      MCSS_GetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
    else
      MCSS_GetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
#endif
    pos_x = FX_FX32_TO_F32( pos_fx32.x );

    if( work->oshidashi_direct == OSHIDASHI_DIRECT_L_TO_R )  // ������E��
    {
      pos_x += OSHIDASHI_SPEED;
      if( pos_x >= center_x )
      {
        move_end = TRUE;
      }
    }
    else  // �E���獶��
    {
      pos_x -= OSHIDASHI_SPEED;
      if( pos_x <= center_x )
      {
        move_end = TRUE;
      }
    }

    if( move_end )
    {
      pos_x = center_x;
      work->oshidashi_state = OSHIDASHI_STATE_CHANGE_COMPARE_FORM;
    }

    {
      pos_fx32.x = FX_F32_TO_FX32(pos_x);
#ifndef DEF_MINIMUM_LOAD
      MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
      MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
#else
      if( work->is_poke_front )
        MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
      else
        MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
#endif

#ifndef DEF_MINIMUM_LOAD
      MCSS_GetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &pos_fx32 );
#else
      if( work->is_poke_front )
        MCSS_GetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &pos_fx32 );
      else
        MCSS_GetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &pos_fx32 );
#endif
      pos_x = FX_FX32_TO_F32( pos_fx32.x );
      if( work->oshidashi_direct == OSHIDASHI_DIRECT_L_TO_R )  // ������E��
      {
        pos_x += OSHIDASHI_SPEED;
      }
      else  // �E���獶��
      {
        pos_x -= OSHIDASHI_SPEED;
      }
      pos_fx32.x = FX_F32_TO_FX32(pos_x);
#ifndef DEF_MINIMUM_LOAD
      MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &pos_fx32 );
      MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &pos_fx32 );
#else
      if( work->is_poke_front )
        MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &pos_fx32 );
      else
        MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &pos_fx32 );
#endif
    }

  }
  else if( work->oshidashi_state == OSHIDASHI_STATE_CHANGE_COMPARE_FORM )  // �����o�������������̂ŁA��r�t�H������ύX����
  {
    Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_TOP );

        {
          u16 no;
          MCSS_WORK* mw;
          POKE_MCSS_CALL_BACK_DATA* pmcbd;

          // �ԍ��A�|�C���^����ւ�
          no = work->diff_curr_no;
          work->diff_curr_no = work->diff_comp_no;
          work->diff_comp_no = no;

#ifndef DEF_MINIMUM_LOAD
          mw = work->poke_mcss_wk[POKE_CURR_F].poke_wk;
          work->poke_mcss_wk[POKE_CURR_F].poke_wk = work->poke_mcss_wk[POKE_COMP_F].poke_wk;
          work->poke_mcss_wk[POKE_COMP_F].poke_wk = mw;

          pmcbd = work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data;
          work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data = work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data;
          work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data = pmcbd;
          work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data->poke_idx = POKE_COMP_F;
          work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data->poke_idx = POKE_CURR_F;

          mw = work->poke_mcss_wk[POKE_CURR_B].poke_wk;
          work->poke_mcss_wk[POKE_CURR_B].poke_wk = work->poke_mcss_wk[POKE_COMP_B].poke_wk;
          work->poke_mcss_wk[POKE_COMP_B].poke_wk = mw;

          pmcbd = work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data;
          work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data = work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data;
          work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data = pmcbd;
          work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data->poke_idx = POKE_COMP_B;
          work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data->poke_idx = POKE_CURR_B;
#else
          if( work->is_poke_front )
          {
            mw = work->poke_mcss_wk[POKE_CURR_F].poke_wk;
            work->poke_mcss_wk[POKE_CURR_F].poke_wk = work->poke_mcss_wk[POKE_COMP_F].poke_wk;
            work->poke_mcss_wk[POKE_COMP_F].poke_wk = mw;

            pmcbd = work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data;
            work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data = work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data;
            work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data = pmcbd;
            work->poke_mcss_wk[POKE_CURR_F].poke_call_back_data->poke_idx = POKE_COMP_F;
            work->poke_mcss_wk[POKE_COMP_F].poke_call_back_data->poke_idx = POKE_CURR_F;
          }
          else
          {
            mw = work->poke_mcss_wk[POKE_CURR_B].poke_wk;
            work->poke_mcss_wk[POKE_CURR_B].poke_wk = work->poke_mcss_wk[POKE_COMP_B].poke_wk;
            work->poke_mcss_wk[POKE_COMP_B].poke_wk = mw;

            pmcbd = work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data;
            work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data = work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data;
            work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data = pmcbd;
            work->poke_mcss_wk[POKE_CURR_B].poke_call_back_data->poke_idx = POKE_COMP_B;
            work->poke_mcss_wk[POKE_COMP_B].poke_call_back_data->poke_idx = POKE_CURR_B;
          }
#endif

          // �|�P�A�C�R���ύX
          Zukan_Detail_Form_ChangePokeicon( param, work, cmn );

          // �e�L�X�g
          // �e�L�X�g��OBJ�ł͂Ȃ�BG�Ȃ̂ŁA����ւ��ł��Ȃ��̂ŊہX��������
          Zukan_Detail_Form_WritePokeCurrText( param, work, cmn );
          Zukan_Detail_Form_WritePokeCompText( param, work, cmn );

          // �e�L�X�g�ɍ��킹�āA�t�H�������̔w�ʃt�B�[���h��p�ӂ���
          Zukan_Detail_Form_ObjFieldSetup( param, work, cmn );

          // �}�ӃZ�[�u�f�[�^�ɉ{�����f�[�^�ݒ�
          {
            u16 monsno = ZKNDTL_COMMON_GetCurrPoke(cmn);

            GAMEDATA* gamedata = ZKNDTL_COMMON_GetGamedata(cmn);
            ZUKAN_SAVEDATA* zkn_sv = GAMEDATA_GetZukanSave( gamedata );

            u32  sex  = work->diff_info_list[work->diff_curr_no].sex;
            BOOL rare = (work->diff_info_list[work->diff_curr_no].rare==0)?FALSE:TRUE;
            u32  form = work->diff_info_list[work->diff_curr_no].form;

            ZUKANSAVE_SetDrawData(  zkn_sv, monsno, sex, rare, form );

#ifdef DEBUG_KAWADA
            {
              OS_Printf( "ZUKAN_DETAIL_FORM : monsno=%d, diff_no=%d, f=%d, s=%d, r=%d\n", monsno, work->diff_curr_no, form, sex, rare );
            }
#endif
#ifdef DEBUG_KAWADA
            {
              ZUKANSAVE_GetDrawData(  zkn_sv, monsno, &sex, &rare, &form, param->heap_id );
              OS_Printf( "ZUKAN_DETAIL_FORM : monsno=%d, ZUKANSAVE_GetDrawData, f=%d, s=%d, r=%d\n", monsno, form, sex, rare );
            }
#endif
          }
        }

      // �����o���p�֐��𗘗p����comp��curr�̎��̃t�H�����ɂ��Ă���
      // �����o���p�֐��𗘗p���Ĉʒu�ݒ�
#ifndef DEF_MINIMUM_LOAD
      MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &poke_pos[POKE_CURR_POS_CENTER] );
      MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &poke_pos[POKE_CURR_POS_CENTER] );
#else
      if( work->is_poke_front )
      {
        MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &poke_pos[POKE_CURR_POS_CENTER] );
      }
      else
      {
        MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &poke_pos[POKE_CURR_POS_CENTER] );
      }
#endif
      work->oshidashi_direct = OSHIDASHI_DIRECT_R_TO_L;
      Zukan_Detail_Form_OshidashiChangeCompareForm( param, work, cmn );
      // STATE_TOP�̏�Ԃ̂Ƃ���1�x�ł��t�H�����ύX���s���ƁAcomp�ɂ�curr�̎��̃t�H������ݒ肵�ďI���悤�ɂȂ�B
      // STATE_TOP�̏�Ԃ̂Ƃ��Ƀt�H�����ύX���s��Ȃ�������Acomp��STATE_EXCHANGE�̏�Ԃ̂Ƃ��ɐݒ肵�����̂̂܂܂ɂȂ�B
      
      work->oshidashi_state = OSHIDASHI_STATE_NONE;
  }
}

//-------------------------------------
/// �K�w�ύX  // TOP_TO_EXCHANGE or EXCHANGE_TO_TOP
//=====================================
static void Zukan_Detail_Form_KaisouMain( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  const f32  exchange_left_x  =   -64.0f;
  const f32  exchange_right_x =    64.0f;
  const f32  top_center_x     =     0.0f;
  const f32  top_right_x      =   256.0f;
  const f32  base_y           =   -24.0f;
  const f32  base_z           =     0.0f;

  if( work->diff_num < 2 )  // ����if���͐^�ɂȂ邱�Ƃ͂Ȃ��C������
  {
    if( work->state == STATE_TOP_TO_EXCHANGE )
    {
      //Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_EXCHANGE );
      Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_TOP_TO_EXCHANGE_TOUCHBAR_CHANGE );
    }
    else if( work->state == STATE_EXCHANGE_TO_TOP )
    {
      //Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_TOP );
      Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_EXCHANGE_TO_TOP_TOUCHBAR_CHANGE );
    }
    return;
  }

  if( work->state == STATE_TOP_TO_EXCHANGE )
  {
    VecFx32 pos_fx32;
    f32     pos_x;
    if( !work->kaisou_curr_end )
    {
#ifndef DEF_MINIMUM_LOAD
      MCSS_GetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &pos_fx32 );
#else
      if( work->is_poke_front )
        MCSS_GetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &pos_fx32 );
      else
        MCSS_GetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &pos_fx32 );
#endif
      pos_x = FX_FX32_TO_F32( pos_fx32.x );
      pos_x -= KAISOU_CURR_SPEED;
      if( pos_x <= exchange_left_x )
      {
        pos_x = exchange_left_x;
        work->kaisou_curr_end = TRUE;
      }
      pos_fx32.x = FX_F32_TO_FX32( pos_x );
#ifndef DEF_MINIMUM_LOAD
      MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &pos_fx32 );
      MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &pos_fx32 );
#else
      if( work->is_poke_front )
        MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &pos_fx32 );
      else
        MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &pos_fx32 );
#endif
    }
    if( !work->kaisou_comp_end )
    {
#ifndef DEF_MINIMUM_LOAD
      MCSS_GetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
#else
      if( work->is_poke_front )
        MCSS_GetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
      else
        MCSS_GetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
#endif
      pos_x = FX_FX32_TO_F32( pos_fx32.x );
      pos_x -= KAISOU_COMP_SPEED;
      if( pos_x <= exchange_right_x )
      {
        pos_x = exchange_right_x;
        work->kaisou_comp_end = TRUE;
      }
      pos_fx32.x = FX_F32_TO_FX32( pos_x );
#ifndef DEF_MINIMUM_LOAD
      MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
      MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
#else
      if( work->is_poke_front )
        MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
      else
        MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
#endif
    }
    if( work->kaisou_curr_end && work->kaisou_comp_end )
    {
      //Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_EXCHANGE );
      Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_TOP_TO_EXCHANGE_TOUCHBAR_CHANGE );
    }
  }
  else if( work->state == STATE_TOP_TO_EXCHANGE_TOUCHBAR_CHANGE )
  {
    Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_EXCHANGE );
  }
  else if( work->state == STATE_EXCHANGE_TO_TOP )
  {
    VecFx32 pos_fx32;
    f32     pos_x;
    if( !work->kaisou_curr_end )
    {
#ifndef DEF_MINIMUM_LOAD
      MCSS_GetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &pos_fx32 );
#else
      if( work->is_poke_front )
        MCSS_GetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &pos_fx32 );
      else
        MCSS_GetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &pos_fx32 );
#endif
      pos_x = FX_FX32_TO_F32( pos_fx32.x );
      pos_x += KAISOU_CURR_SPEED;
      if( pos_x >= top_center_x )
      {
        pos_x = top_center_x;
        work->kaisou_curr_end = TRUE;
      }
      pos_fx32.x = FX_F32_TO_FX32( pos_x );
#ifndef DEF_MINIMUM_LOAD
      MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &pos_fx32 );
      MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &pos_fx32 );
#else
      if( work->is_poke_front )
        MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_F].poke_wk, &pos_fx32 );
      else
        MCSS_SetPosition( work->poke_mcss_wk[POKE_CURR_B].poke_wk, &pos_fx32 );
#endif
    }
    if( !work->kaisou_comp_end )
    {
#ifndef DEF_MINIMUM_LOAD
      MCSS_GetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
#else
      if( work->is_poke_front )
        MCSS_GetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
      else
        MCSS_GetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
#endif
      pos_x = FX_FX32_TO_F32( pos_fx32.x );
      pos_x += KAISOU_COMP_SPEED;
      if( pos_x >= top_right_x )
      {
        pos_x = top_right_x;
        work->kaisou_comp_end = TRUE;
      }
      pos_fx32.x = FX_F32_TO_FX32( pos_x );
#ifndef DEF_MINIMUM_LOAD
      MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
      MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
#else
      if( work->is_poke_front )
        MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_F].poke_wk, &pos_fx32 );
      else
        MCSS_SetPosition( work->poke_mcss_wk[POKE_COMP_B].poke_wk, &pos_fx32 );
#endif
    }
    if( work->kaisou_curr_end && work->kaisou_comp_end )
    {
      //Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_TOP );
      Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_EXCHANGE_TO_TOP_TOUCHBAR_CHANGE );
    }
  }
  else if( work->state == STATE_EXCHANGE_TO_TOP_TOUCHBAR_CHANGE )
  {
    Zukan_Detail_Form_ChangeState( param, work, cmn, STATE_TOP );
  }
}

//-------------------------------------
/// �A���t�@�ݒ�
//=====================================
static void Zukan_Detail_Form_AlphaInit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  int ev1 = 12;

  // �������ɂ��Ȃ�OBJ�̐ݒ��ύX����
  {
    u8 i;

    for( i=0; i<OBJ_SWAP_MAX; i++ )
    {
      if( work->pokeicon_clwk[i] )
      {
        GFL_CLACT_WK_SetObjMode( work->pokeicon_clwk[i], GX_OAM_MODE_NORMAL );  // �A���t�@�A�j���[�V�����̉e�����󂯂Ȃ��悤�ɂ���
      }
    }

    GFL_CLACT_WK_SetObjMode( work->obj_clwk[OBJ_BAR_RANGE], GX_OAM_MODE_NORMAL );  // �A���t�@�A�j���[�V�����̉e�����󂯂Ȃ��悤�ɂ���
    GFL_CLACT_WK_SetObjMode( work->obj_clwk[OBJ_BAR_CURSOR], GX_OAM_MODE_NORMAL );  // �A���t�@�A�j���[�V�����̉e�����󂯂Ȃ��悤�ɂ���

    for( i=0; i<BUTTON_OBJ_MAX; i++ )
    {
      GFL_CLACT_WK_SetObjMode( work->button[i].clwk, GX_OAM_MODE_NORMAL );  // �A���t�@�A�j���[�V�����̉e�����󂯂Ȃ��悤�ɂ���
    }
  }

  G2_SetBlendAlpha(
        GX_BLEND_PLANEMASK_NONE,
        GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
        ev1,
        16 - ev1 );

  G2S_SetBlendAlpha(
        GX_BLEND_PLANEMASK_BG3,
        GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
        ev1,
        16 - ev1 );
}
static void Zukan_Detail_Form_AlphaExit( ZUKAN_DETAIL_FORM_PARAM* param, ZUKAN_DETAIL_FORM_WORK* work, ZKNDTL_COMMON_WORK* cmn )
{
  // �������ɂ��Ȃ�OBJ�̐ݒ�����ɖ߂�
  {
    u8 i;

    for( i=0; i<OBJ_SWAP_MAX; i++ )
    {
      if( work->pokeicon_clwk[i] )
      {
        GFL_CLACT_WK_SetObjMode( work->pokeicon_clwk[i], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
      }
    }

    GFL_CLACT_WK_SetObjMode( work->obj_clwk[OBJ_BAR_RANGE], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    GFL_CLACT_WK_SetObjMode( work->obj_clwk[OBJ_BAR_CURSOR], GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�

    for( i=0; i<BUTTON_OBJ_MAX; i++ )
    {
      GFL_CLACT_WK_SetObjMode( work->button[i].clwk, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    }
  }

  // �ꕔ���t�F�[�h�̐ݒ�����ɖ߂�
  ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_m );
  // �ꕔ���t�F�[�h�̐ݒ�����ɖ߂�
  ZKNDTL_COMMON_FadeSetPlaneDefault( work->fade_wk_s );
}


//=============================================================================
/**
 * 
 */
//=============================================================================
// �}�N��
#undef BLOCK_POKE_EXIT
#undef BLOCK_POKEICON_EXIT

