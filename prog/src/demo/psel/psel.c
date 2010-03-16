//============================================================================
/**
 *  @file   psel.c
 *  @brief  �O�C�I��
 *  @author Koji Kawada
 *  @data   2010.03.12
 *  @note   
 *  ���W���[�����FPSEL
 */
//============================================================================
// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/ica_anime.h"
#include "system/ica_camera.h"
#include "system/pokegra.h"
#include "system/poke2dgra.h"
#include "gamesystem/msgspeed.h"
#include "app/app_keycursor.h"
#include "app/app_taskmenu.h"

#include "psel_graphic.h"
#include "demo/psel.h"

// �A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "script_message.naix"
#include "msg/script/msg_t01r0102.h"
#include "psel.naix"

// �T�E���h
#include "sound/pm_sndsys.h"

// �I�[�o�[���C


//=============================================================================
/**
*  �萔��`
*/
//=============================================================================
#define HEAP_SIZE              (0x100000)               ///< �q�[�v�T�C�Y

// ���C��BG�t���[��
#define BG_FRAME_M_THREE       (GFL_BG_FRAME0_M)     // 3D
#define BG_FRAME_M_REAR        (GFL_BG_FRAME1_M)
#define BG_FRAME_M_WIN         (GFL_BG_FRAME2_M)
#define BG_FRAME_M_TEXT        (GFL_BG_FRAME3_M)     // APP_TASKMENU�������ɏo��

// ���C��BG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_M_THREE   (2)
#define BG_FRAME_PRI_M_REAR    (3)
#define BG_FRAME_PRI_M_WIN     (1)
#define BG_FRAME_PRI_M_TEXT    (0)

// ���C��BG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_M_REAR                   = 11,
  BG_PAL_NUM_M_APP_TASKMENU           =  2,
  BG_PAL_NUM_M_WIN                    =  1,
  BG_PAL_NUM_M_TEXT                   =  1,
};
// �ʒu
enum
{
  BG_PAL_POS_M_REAR            =   0,
  BG_PAL_POS_M_APP_TASKMENU    =  11,  // 2�{
  BG_PAL_POS_M_WIN             =  13,
  BG_PAL_POS_M_TEXT            =  14,
};

// ���C��OBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_M_FINGER          = 1,
};
// �ʒu
enum
{
  OBJ_PAL_POS_M_FINGER          = 0,
};

// �T�uBG�t���[��
#define BG_FRAME_S_INSIDE_BALL (GFL_BG_FRAME0_S)
#define BG_FRAME_S_WIN         (GFL_BG_FRAME1_S)  // BG_FRAME_S_WIN��BG_FRAME_S_TEXT�͋��ʂ̃L�����̈���g�p
#define BG_FRAME_S_TEXT        (GFL_BG_FRAME2_S)  // BG_FRAME_S_WIN��BG_FRAME_S_TEXT�͋��ʂ̃L�����̈���g�p

// �T�uBG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_S_INSIDE_BALL    (2)
#define BG_FRAME_PRI_S_WIN            (1)
#define BG_FRAME_PRI_S_TEXT           (0)

// �T�uBG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_S_WIN                    =  1,
  BG_PAL_NUM_S_TEXT                   =  1,
  BG_PAL_NUM_S_INSIDE_BALL            =  8,
};
// �ʒu
enum
{
  BG_PAL_POS_S_WIN             =  0,
  BG_PAL_POS_S_TEXT            =  1,
  BG_PAL_POS_S_INSIDE_BALL     =  8,
};

// �T�uOBJ�p���b�g
// �{��
enum
{
  OBJ_PAL_NUM_S_MIZU      = 1,  // small��big�����̃p���b�g����g��
  OBJ_PAL_NUM_S_HONOO     = 1,
  OBJ_PAL_NUM_S_KUSA      = 1,
};
// �ʒu
enum
{
  OBJ_PAL_POS_S_MIZU      = 0,
  OBJ_PAL_POS_S_HONOO     = 1,
  OBJ_PAL_POS_S_KUSA      = 2,
};


// �T�E���h
#define PSELSND_DECIDE  ( SEQ_SE_DECIDE1 )
#define PSELSND_CANCEL  ( SEQ_SE_CANCEL1 )
#define PSELSND_MOVE    ( SEQ_SE_SELECT1 )
#define PSELSND_OPEN    ( SEQ_SE_OPEN1 )


// ProcMain�̃V�[�P���X
enum
{
  SEQ_START            = 0,
  SEQ_LOAD,
  SEQ_S01_INIT,
  SEQ_S01_FADE_IN,
  SEQ_S01_MAIN,
  SEQ_S01_FADE_OUT,
  SEQ_S01_EXIT,
  SEQ_S02_INIT,
  SEQ_S02_FADE_IN,
  SEQ_S02_MAIN,
  SEQ_S02_FADE_OUT,
  SEQ_S02_EXIT,
  SEQ_UNLOAD,
  SEQ_END,
  SEQ_MAX,
};


// �e�L�X�g  // ��납��m�ۂ��邱��
enum
{
  TEXT_LETTER,     // MAIN
  TEXT_EXPLAIN,    // MAIN
  TEXT_POKE_INFO,  // SUB
  TEXT_MAX,
};

#define TEXT_COLOR_L (15)  // MAIN��SUB�������\���ɂ��Ă���
#define TEXT_COLOR_S ( 2)
#define TEXT_COLOR_B ( 0)

static const u8 bmpwin_setup[TEXT_MAX][9] =
{
  // frmnum           posx  posy  sizx  sizy  palnum             dir                    x  y (x,y�͖������ăZ���^�����O���邱�Ƃ�����)
  {  BG_FRAME_M_TEXT,    1,    3,   30,   18, BG_PAL_POS_M_TEXT, GFL_BMP_CHRAREA_GET_B, 0, 0 },
  {  BG_FRAME_M_TEXT,    1,   19,   30,    2, BG_PAL_POS_M_TEXT, GFL_BMP_CHRAREA_GET_B, 0, 0 },
  {  BG_FRAME_S_TEXT,    1,   18,   30,    4, BG_PAL_POS_S_TEXT, GFL_BMP_CHRAREA_GET_B, 0, 0 },
};

// ������
#define TEXT_POKE_INFO_LEN_MAX    (64)  // �|�P�����̃^�C�v�Ǝ푰��(�s�J�`���E�Ƃ�)�̏��  // EOM���܂߂�������

// ����J�[�\���f�[�^
static const u8 ALIGN4 sc_skip_cursor_character[128] = {
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,
  0xF2,0xFF,0xFF,0xFF,0x20,0xFF,0xFF,0x2F, 0x00,0xF2,0xFF,0x22,0x00,0x20,0x2F,0x02,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x2F,0x00,0x00,0x00,
  0x22,0x00,0x00,0x00,0x02,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,

  0x00,0x00,0x22,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};


// �t�F�[�h
#define S01_FADE_IN_WAIT  (0)
#define S01_FADE_OUT_WAIT (-1)
#define S02_FADE_IN_WAIT  (2)
#define S02_FADE_OUT_WAIT (0)


// 2D OBJ
enum
{
  TWO_OBJ_RES_NCG,
  TWO_OBJ_RES_NCL,
  TWO_OBJ_RES_NCE,
  TWO_OBJ_RES_MAX,
  TWO_OBJ_RES_NAN        = TWO_OBJ_RES_MAX,
  TWO_OBJ_RES_DARK_NCL,
  TWO_OBJ_RES_FILE_MAX,
};

#define OBJ_BG_PRI_S_POKE     (BG_FRAME_PRI_S_INSIDE_BALL)
#define OBJ_BG_PRI_M_FINGER   (BG_FRAME_PRI_M_THREE)


// �I��Ώ�
typedef enum
{
  TARGET_MIZU       = 0,                  // ��     // �����珇�Ԃɕ��ׂĂ���
  TARGET_HONOO,                           // ����
  TARGET_KUSA,                            // �E
  TARGET_POKE_MAX,                        // �|�P������
  TARGET_BUTTON     = TARGET_POKE_MAX,    // �m�F�{�^��
  TARGET_MAX
}
TARGET;

// �^�b�`�̓����蔻��
static const GFL_UI_TP_HITTBL target_tp_hittbl[TARGET_MAX +1] =
{
  { GFL_UI_TP_USE_CIRCLE,   88,  144,   24 },  // circle
  { GFL_UI_TP_USE_CIRCLE,  128,  120,   24 },  // circle
  { GFL_UI_TP_USE_CIRCLE,  168,  144,   24 },  // circle
  { 21*8, 24*8 -1, 9*8, 23*8 -1 },  // rect
  { GFL_UI_TP_HIT_END, 0, 0, 0 },  // �e�[�u���I��
};

// �w�w���J�[�\���̈ʒu
static const u8 finger_pos[TARGET_POKE_MAX][2] =
{
  {  88, 120 },
  { 128,  96 },
  { 168, 120 },
};

// �|�P�����召�Z�b�g
// monsno
static const u16 poke_monsno[TARGET_POKE_MAX] =
{
  MONSNO_557,  // MONSNO_???��???�̓O���t�B�b�N�i���o�[�A��`����Ă���l��monsno
  MONSNO_554,
  MONSNO_551,
};
// �ʒu
static const u8 poke_pos[TARGET_POKE_MAX][2] =
{
  {  64, 128 },
  { 128,  64 },
  { 192, 128 },
};
// ���\�[�X
static const u8 poke_res_big[TARGET_POKE_MAX][TWO_OBJ_RES_FILE_MAX] =
{
  { NARC_psel_psel_557_m_NCGR, NARC_psel_psel_557_n_NCLR, NARC_psel_psel_557_m_NCER, NARC_psel_psel_557_m_NANR, NARC_psel_psel_557_x_NCLR },
  { NARC_psel_psel_554_m_NCGR, NARC_psel_psel_554_n_NCLR, NARC_psel_psel_554_m_NCER, NARC_psel_psel_554_m_NANR, NARC_psel_psel_554_x_NCLR },
  { NARC_psel_psel_551_m_NCGR, NARC_psel_psel_551_n_NCLR, NARC_psel_psel_551_m_NCER, NARC_psel_psel_551_m_NANR, NARC_psel_psel_551_x_NCLR },
};
// �p���b�g
static const u8 poke_obj_pal_pos_s[TARGET_POKE_MAX] =
{
  OBJ_PAL_POS_S_MIZU, 
  OBJ_PAL_POS_S_HONOO,
  OBJ_PAL_POS_S_KUSA,
};

// �{�[�������C���[�W�����T�u���BG
enum
{
  INSIDE_BALL_MIZU,    // TARGET�Ɠ������тɂ��Ă���
  INSIDE_BALL_HONOO,
  INSIDE_BALL_KUSA,
  INSIDE_BALL_NONE,
  INSIDE_BALL_MAX,
};

// �{�[�������C���[�W�����T�u���BG�̃p���b�g�f�[�^
static const u8 inside_ball_res[INSIDE_BALL_MAX] =
{
  NARC_psel_psel_bg02_w_NCLR, 
  NARC_psel_psel_bg02_f_NCLR, 
  NARC_psel_psel_bg02_l_NCLR, 
  NARC_psel_psel_bg02_NCLR, 
};

// �{�[�������C���[�W�����T�u���BG�̃p���b�g
enum
{
  INSIDE_BALL_PAL_START,  // �J�n�F
  INSIDE_BALL_PAL_TRANS,  // ���ݐF
  INSIDE_BALL_PAL_MAX,
};

// �p���b�g�A�j���̏��
typedef enum
{
  POKE_PAL_ANIME_STATE_COLOR,              // �J���[�ɂ������Ƃ���Psel_PalPokeAnimeStart�Ɉ����Ƃ��ēn������
  POKE_PAL_ANIME_STATE_COLOR_TO_DARK,
  POKE_PAL_ANIME_STATE_DARK,               // �_�[�N�ɂ������Ƃ���Psel_PalPokeAnimeStart�Ɉ����Ƃ��ēn������
  POKE_PAL_ANIME_STATE_DARK_TO_COLOR,
}
POKE_PAL_ANIME_STATE;
typedef enum
{
  INSIDE_BALL_PAL_ANIME_STATE_END,
  INSIDE_BALL_PAL_ANIME_STATE_START_TO_END,
}
INSIDE_BALL_PAL_ANIME_STATE;

// �|�P�����召
enum
{
  POKE_SMALL,
  POKE_BIG,
  POKE_MAX,
};
// �|�P�����召�p���b�g
enum
{
  POKE_PAL_COLOR,  // �{���̐F
  POKE_PAL_DARK,   // ��I�����̈ÐF
  POKE_PAL_TRANS,  // ���ݐF
  POKE_PAL_MAX,
};


// 3D��
// �O�C�I��
// s01
static const GFL_G3D_UTIL_RES s01_res_tbl[] = 
{
  { ARCID_PSEL,        NARC_psel_psel_s01_nsbmd,          GFL_G3D_UTIL_RESARC },
  { ARCID_PSEL,        NARC_psel_psel_s01_nsbca,          GFL_G3D_UTIL_RESARC },
  { ARCID_PSEL,        NARC_psel_psel_s01_nsbma,          GFL_G3D_UTIL_RESARC },
  { ARCID_PSEL,        NARC_psel_psel_s01_nsbta,          GFL_G3D_UTIL_RESARC },
};
static const GFL_G3D_UTIL_ANM s01_anm_tbl[] = 
{
  { 1, 0 },
  { 2, 0 },
  { 3, 0 },
};
static const GFL_G3D_UTIL_OBJ s01_obj_tbl[] = 
{
  {
    0,                         // ���f�����\�[�XID
    0,                         // ���f���f�[�^ID(���\�[�X����INDEX)
    0,                         // �e�N�X�`�����\�[�XID
    s01_anm_tbl,               // �A�j���e�[�u��(�����w��̂���)
    NELEMS(s01_anm_tbl),       // �A�j�����\�[�X��
  },
};

// s02
static const GFL_G3D_UTIL_RES s02_res_tbl[] = 
{
  // s02
  { ARCID_PSEL,        NARC_psel_psel_s02_nsbmd,          GFL_G3D_UTIL_RESARC },
  { ARCID_PSEL,        NARC_psel_psel_s02_nsbca,          GFL_G3D_UTIL_RESARC },
  // mb
  { ARCID_PSEL,        NARC_psel_psel_mb_nsbmd,           GFL_G3D_UTIL_RESARC },
  { ARCID_PSEL,        NARC_psel_psel_mb_nsbma,           GFL_G3D_UTIL_RESARC },
};
static const GFL_G3D_UTIL_ANM s02_anm_tbl[] = 
{
  // s02
  { 1, 0 },
};
static const GFL_G3D_UTIL_ANM mb_anm_tbl[] = 
{
  // mb
  { 3, 0 },
};
static const GFL_G3D_UTIL_OBJ s02_obj_tbl[] = 
{
  // s02
  {
    0,                         // ���f�����\�[�XID
    0,                         // ���f���f�[�^ID(���\�[�X����INDEX)
    0,                         // �e�N�X�`�����\�[�XID
    s02_anm_tbl,               // �A�j���e�[�u��(�����w��̂���)
    NELEMS(s02_anm_tbl),       // �A�j�����\�[�X��
  },
  // mb
  {
    2,                         // ���f�����\�[�XID
    0,                         // ���f���f�[�^ID(���\�[�X����INDEX)
    2,                         // �e�N�X�`�����\�[�XID
    mb_anm_tbl,                // �A�j���e�[�u��(�����w��̂���)
    NELEMS(mb_anm_tbl),        // �A�j�����\�[�X��
  },
  {
    2,                         // ���f�����\�[�XID
    0,                         // ���f���f�[�^ID(���\�[�X����INDEX)
    2,                         // �e�N�X�`�����\�[�XID
    mb_anm_tbl,                // �A�j���e�[�u��(�����w��̂���)
    NELEMS(mb_anm_tbl),        // �A�j�����\�[�X��
  },
  {
    2,                         // ���f�����\�[�XID
    0,                         // ���f���f�[�^ID(���\�[�X����INDEX)
    2,                         // �e�N�X�`�����\�[�XID
    mb_anm_tbl,                // �A�j���e�[�u��(�����w��̂���)
    NELEMS(mb_anm_tbl),        // �A�j�����\�[�X��
  },
};
enum
{
  MB_L,
  MB_C,
  MB_R,
  MB_MAX
};
static const VecFx32 three_mb_trans[MB_MAX] =
{
  { FX_F32_TO_FX32(-12.5f), FX_F32_TO_FX32(29.0f), FX_F32_TO_FX32( 3.6f) },
  { FX_F32_TO_FX32(  0.0f), FX_F32_TO_FX32(29.0f), FX_F32_TO_FX32(-7.0f) },
  { FX_F32_TO_FX32( 12.5f), FX_F32_TO_FX32(29.0f), FX_F32_TO_FX32( 3.6f) },
};

// 3D�S��
// �ő吔
#define THREE_RES_MAX              (4)   // s02��mb�𓯎��ɓǂݍ��񂾂Ƃ��̑���
#define THREE_OBJ_MAX              (4)   // s02��mb�𓯎��ɓǂݍ��񂾂Ƃ��̑���
#define THREE_ICA_ANIME_INTERVAL   (10)
// �Z�b�g�A�b�v�ԍ�
enum
{
  THREE_SETUP_IDX_S01,
  THREE_SETUP_IDX_S02,
  THREE_SETUP_IDX_MAX
};
// �Z�b�g�A�b�v�f�[�^
static const GFL_G3D_UTIL_SETUP three_setup_tbl[THREE_SETUP_IDX_MAX] =
{
  { s01_res_tbl,   NELEMS(s01_res_tbl),   s01_obj_tbl,   NELEMS(s01_obj_tbl)   },
  { s02_res_tbl,   NELEMS(s02_res_tbl),   s02_obj_tbl,   NELEMS(s02_obj_tbl)   },
};
// ���[�U(���̃\�[�X�̃v���O�������������l)�����߂��I�u�W�F�N�g�ԍ�
enum
{
  THREE_USER_OBJ_IDX_S01,
  THREE_USER_OBJ_IDX_S02,
  THREE_USER_OBJ_IDX_MB_L,  // MB_(MB_MAX)�Ɠ�������
  THREE_USER_OBJ_IDX_MB_C,
  THREE_USER_OBJ_IDX_MB_R,
  THREE_USER_OBJ_IDX_MAX,
};


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// 3D�I�u�W�F�N�g�̃v���p�e�B
//=====================================
typedef struct
{
  u16                         idx;        // GFL_G3D_UTIL������U��ԍ�
  GFL_G3D_OBJSTATUS           objstatus;
  BOOL                        draw;       // TRUE�̂Ƃ��`�悷��
}
THREE_OBJ_PROPERTY;
static THREE_OBJ_PROPERTY* ThreeObjPropertyCreateArray( HEAPID heap_id, u16 num );
static void ThreeObjPropertyDeleteArray( THREE_OBJ_PROPERTY* prop );
static THREE_OBJ_PROPERTY* ThreeObjPropertyCreateArray( HEAPID heap_id, u16 num )
{
  u16 i;
  THREE_OBJ_PROPERTY* prop_array = GFL_HEAP_AllocClearMemory( heap_id, sizeof(THREE_OBJ_PROPERTY) * num );
  // 0�ȊO�̒l�ŏ�����������̂ɂ��ď��������s�� 
  for( i=0; i<num; i++ )
  {
    THREE_OBJ_PROPERTY* prop = &(prop_array[i]);
    VEC_Set( &(prop->objstatus.trans), 0, 0, 0 );
    VEC_Set( &(prop->objstatus.scale), FX32_ONE, FX32_ONE, FX32_ONE );
    MTX_Identity33( &(prop->objstatus.rotate) );
  }
  return prop_array;
}
static void ThreeObjPropertyDeleteArray( THREE_OBJ_PROPERTY* prop_array )
{
  GFL_HEAP_FreeMemory( prop_array );
}

//-------------------------------------
/// �|�P�����召�Z�b�g
//=====================================
typedef struct
{
  u32        res[POKE_MAX][TWO_OBJ_RES_MAX];
  GFL_CLWK*  clwk[POKE_MAX];
  
  // �I���W�i���p���b�g�f�[�^
  u16        pal[POKE_PAL_MAX][16];
  
  // �p���b�g�A�j���̏��
  u16        pal_anime_state;
  u16        pal_anime_count;
}
POKE_SET;


//-------------------------------------
/// PROC ���[�N
//=====================================
typedef struct
{
  // �q�[�v�A�p�����[�^�Ȃ�
  HEAPID                      heap_id;
  PSEL_PARAM*                 param;

  // �^�b�`or�L�[
  int                         ktst;  // GFL_APP_END_KEY(GFL_APP_KTST_KEY) or GFL_APP_END_TOUCH(GFL_APP_KTST_TOUCH)

  // �V�[�P���X
  int                         sub_seq;

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  PSEL_GRAPHIC_WORK*          graphic;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;

  // 3D�S��
  GFL_G3D_UTIL*               three_util;
  u16                         three_unit_idx[THREE_SETUP_IDX_MAX];             // GFL_G3D_UTIL������U��ԍ�        // �Y������THREE_SETUP_IDX_(THREE_SETUP_IDX_MAX)
  u16                         three_obj_prop_tbl_idx[THREE_USER_OBJ_IDX_MAX];  // three_obj_prop_tbl�̃C���f�b�N�X  // �Y������THREE_USER_OBJ_IDX_(THREE_USER_OBJ_IDX_MAX)
  u16                         three_obj_prop_num;
  THREE_OBJ_PROPERTY*         three_obj_prop_tbl;
  ICA_ANIME*                  three_ica_anime;

  // �t���[��
  u32                         frame;  // �V�[�����؂�ւ������0����X�^�[�g�������A3D�̃t���[�����ƈ�v����悤�ɂ��Ă���

  // 2D OBJ
  // �|�P�����召�Z�b�g
  POKE_SET                    poke_set[TARGET_POKE_MAX];
  // �w�w���J�[�\��
  u32                         finger_res[TWO_OBJ_RES_MAX];
  GFL_CLWK*                   finger_clwk;

  // �e�L�X�g
  GFL_MSGDATA*                msgdata;
  GFL_BMPWIN*                 text_bmpwin[TEXT_MAX];
  BOOL                        text_finish[TEXT_MAX];  // bmpwin�̓]�����ς�ł����TRUE
  PRINT_STREAM*               print_stream;
  GFL_TCBLSYS*                ps_tcblsys;
  STRBUF*                     ps_strbuf;
  APP_KEYCURSOR_WORK*         app_keycursor_wk;

  // APP_TASKMENU
  APP_TASKMENU_RES*           app_taskmenu_res;
  APP_TASKMENU_WIN_WORK*      app_taskmenu_win_wk;  // �^�b�`�̂Ƃ��ɏo��u����Ɍ��߂��v�{�^��
  APP_TASKMENU_WORK*          app_taskmenu_wk;      // �^�b�`�ł��L�[�ł��o��ŏI�m�F�́u�͂��v�u�������v�{�^��

  // �{�[�������C���[�W�����T�u���BG�̃p���b�g
  // �I���W�i���p���b�g�f�[�^
  u16                         inside_ball_pal[INSIDE_BALL_MAX][16*BG_PAL_NUM_S_INSIDE_BALL];
  u16                         inside_ball_st_pal[INSIDE_BALL_PAL_MAX][16*BG_PAL_NUM_S_INSIDE_BALL];
  // �p���b�g�A�j���̏��
  INSIDE_BALL_PAL_ANIME_STATE inside_ball_pal_anime_state;
  u8                          inside_ball_pal_anime_count;
  u8                          inside_ball_pal_anime_end;
}
PSEL_WORK;


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Psel_VBlankFunc( GFL_TCB* tcb, void* wk );

// 3D�`��(GRAPHIC_3D_StartDraw��PSEL_GRAPHIC_3D_EndDraw�̊ԂŌĂ�)
static void Psel_ThreeDraw( PSEL_WORK* work );

// 3D�S��
static void Psel_ThreeInit( PSEL_WORK* work );
static void Psel_ThreeExit( PSEL_WORK* work );

// 3D��
static void Psel_ThreeS01Init( PSEL_WORK* work );
static void Psel_ThreeS01Exit( PSEL_WORK* work );
static void Psel_ThreeS01Main( PSEL_WORK* work );

static void Psel_ThreeS02Init( PSEL_WORK* work );
static void Psel_ThreeS02Exit( PSEL_WORK* work );
static void Psel_ThreeS02Main( PSEL_WORK* work );

// �|�P�����召�Z�b�g
static void Psel_PokeSetInit( PSEL_WORK* work );
static void Psel_PokeSetExit( PSEL_WORK* work );

// �w�w���J�[�\��
static void Psel_FingerInit( PSEL_WORK* work );
static void Psel_FingerExit( PSEL_WORK* work );

// BG�S��
static void Psel_BgInit( PSEL_WORK* work );
static void Psel_BgExit( PSEL_WORK* work );

// BG��
static void Psel_BgS01Init( PSEL_WORK* work );
static void Psel_BgS01Exit( PSEL_WORK* work );
static void Psel_BgS01Main( PSEL_WORK* work );

static void Psel_BgS02Init( PSEL_WORK* work );
static void Psel_BgS02Exit( PSEL_WORK* work );
static void Psel_BgS02Main( PSEL_WORK* work );

static void Psel_InsideBallInit( PSEL_WORK* work );
static void Psel_InsideBallExit( PSEL_WORK* work );

// �e�L�X�g�S��
static void Psel_TextInit( PSEL_WORK* work );
static void Psel_TextExit( PSEL_WORK* work );
static void Psel_TextMain( PSEL_WORK* work );
static BOOL Psel_TextTransWait( PSEL_WORK* work );  // TRUE���Ԃ��ė�����]���҂��Ȃ�
static void Psel_TextStreamStart( PSEL_WORK* work, GFL_BMPWIN* bmpwin, u32 str_id );
static BOOL Psel_TextStreamWait( PSEL_WORK* work );  // TRUE���Ԃ��ė�����I��

// �e�L�X�g��
static void Psel_TextS01Init( PSEL_WORK* work );
static void Psel_TextS01Exit( PSEL_WORK* work );
static void Psel_TextS01Main( PSEL_WORK* work );

static void Psel_TextS02Init( PSEL_WORK* work );
static void Psel_TextS02Exit( PSEL_WORK* work );
static void Psel_TextS02Main( PSEL_WORK* work );

// APP_TASKMENU
static void Psel_AppTaskmenuResInit( PSEL_WORK* work );
static void Psel_AppTaskmenuResExit( PSEL_WORK* work );

static void Psel_AppTaskmenuWinInit( PSEL_WORK* work );
static void Psel_AppTaskmenuWinExit( PSEL_WORK* work );
static void Psel_AppTaskmenuWinMain( PSEL_WORK* work );

static void Psel_AppTaskmenuInit( PSEL_WORK* work );
static void Psel_AppTaskmenuExit( PSEL_WORK* work );
static void Psel_AppTaskmenuMain( PSEL_WORK* work );

// �p���b�g
static void U16ToRGB( u16 rgb, u16* r, u16* g, u16* b );
static void Psel_PokeSetPalInit( PSEL_WORK* work );
static void Psel_PokeSetPalExit( PSEL_WORK* work );
static void Psel_PokeSetPalMain( PSEL_WORK* work );
static void Psel_InsideBallPalInit( PSEL_WORK* work );
static void Psel_InsideBallPalExit( PSEL_WORK* work );
static void Psel_InsideBallPalMain( PSEL_WORK* work );
static void Psel_PalInit( PSEL_WORK* work );
static void Psel_PalExit( PSEL_WORK* work );
static void Psel_PalMain( PSEL_WORK* work );
static void Psel_PokeSetPalAnimeStart( PSEL_WORK* work, TARGET target_poke, POKE_PAL_ANIME_STATE state );
static void Psel_InsideBallPalAnimeStart( PSEL_WORK* work, u8 end );


// �V�[�P���X
typedef int (*PSEL_FUNC)( PSEL_WORK* work, int* seq );

static int Psel_Start      ( PSEL_WORK* work, int* seq );
static int Psel_Load       ( PSEL_WORK* work, int* seq );
static int Psel_S01Init    ( PSEL_WORK* work, int* seq );
static int Psel_S01FadeIn  ( PSEL_WORK* work, int* seq );
static int Psel_S01Main    ( PSEL_WORK* work, int* seq );
static int Psel_S01FadeOut ( PSEL_WORK* work, int* seq );
static int Psel_S01Exit    ( PSEL_WORK* work, int* seq );
static int Psel_S02Init    ( PSEL_WORK* work, int* seq );
static int Psel_S02FadeIn  ( PSEL_WORK* work, int* seq );
static int Psel_S02Main    ( PSEL_WORK* work, int* seq );
static int Psel_S02FadeOut ( PSEL_WORK* work, int* seq );
static int Psel_S02Exit    ( PSEL_WORK* work, int* seq );
static int Psel_Unload     ( PSEL_WORK* work, int* seq );
static int Psel_End        ( PSEL_WORK* work, int* seq );

static const PSEL_FUNC func_tbl[SEQ_MAX] =
{
  Psel_Start,
  Psel_Load,
  Psel_S01Init,
  Psel_S01FadeIn,
  Psel_S01Main,
  Psel_S01FadeOut,
  Psel_S01Exit,
  Psel_S02Init,
  Psel_S02FadeIn,
  Psel_S02Main,
  Psel_S02FadeOut,
  Psel_S02Exit,
  Psel_Unload,
  Psel_End,
};


//=============================================================================
/**
*  PROC
*/
//=============================================================================
static GFL_PROC_RESULT Psel_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Psel_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk );
static GFL_PROC_RESULT Psel_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk );

const GFL_PROC_DATA    PSEL_ProcData =
{
  Psel_ProcInit,
  Psel_ProcMain,
  Psel_ProcExit,
};


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^����
 *
 *  @param[in]       heap_id       �q�[�vID
 *  @param[in]       dummy         �_�~�[
 *
 *  @retval          PSEL_PARAM
 */
//------------------------------------------------------------------
PSEL_PARAM*  PSEL_AllocParam(
                                HEAPID               heap_id,
                                u32                  dummy
                           )
{
  PSEL_PARAM* param = GFL_HEAP_AllocMemory( heap_id, sizeof( PSEL_PARAM ) );
  PSEL_InitParam(
      param,
      dummy );
  return param;
}

//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^���
 *
 *  @param[in,out]   param      �p�����[�^�����Ő�����������
 *
 *  @retval          
 */
//------------------------------------------------------------------
void            PSEL_FreeParam(
                            PSEL_PARAM*  param )
{
  GFL_HEAP_FreeMemory( param );
}

//------------------------------------------------------------------
/**
 *  @brief           �p�����[�^��ݒ肷��
 *
 *  @param[in,out]   param         PSEL_PARAM
 *  @param[in]       dummy         �_�~�[
 *
 *  @retval          
 */
//------------------------------------------------------------------
void  PSEL_InitParam(
                  PSEL_PARAM*      param,
                  u32              dummy
                         )
{
  param->dummy             = dummy;
}

//------------------------------------------------------------------
/**
 *  @brief           �I�����ʂ𓾂�
 *
 *  @param[in]       param         PSEL_PARAM
 *
 *  @retval          �I������
 */
//------------------------------------------------------------------
PSEL_RESULT  PSEL_GetResult(
                  const PSEL_PARAM*      param )
{
  return param->result;
}


//=============================================================================
/**
*  ���[�J���֐���`(PROC)
*/
//=============================================================================
//-------------------------------------
/// PROC ����������
//=====================================
static GFL_PROC_RESULT Psel_ProcInit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  PSEL_WORK*     work;

  // ��
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 16, 0 );

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PSEL, HEAP_SIZE );
    work = GFL_PROC_AllocWork( proc, sizeof(PSEL_WORK), HEAPID_PSEL );
    GFL_STD_MemClear( work, sizeof(PSEL_WORK) );
    
    work->heap_id       = HEAPID_PSEL;
    work->param         = (PSEL_PARAM*)pwk;
  }

  // �^�b�`or�L�[
  work->ktst = GFL_UI_CheckTouchOrKey();

  // �V�[�P���X
  work->sub_seq = 0;

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �I������
//=====================================
static GFL_PROC_RESULT Psel_ProcExit( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  PSEL_WORK*     work      = (PSEL_WORK*)mywk;
  
//  // ��
//  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 16, 0 );
  // ��
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 16, 0 );

  // �V�[�P���X
  // �������Ȃ�

  // �^�b�`or�L�[
  GFL_UI_SetTouchOrKey( work->ktst );

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    GFL_PROC_FreeWork( proc );
    GFL_HEAP_DeleteHeap( HEAPID_PSEL );
  }

  return GFL_PROC_RES_FINISH;
}

//-------------------------------------
/// PROC �又��
//=====================================
static GFL_PROC_RESULT Psel_ProcMain( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
  PSEL_WORK*     work      = (PSEL_WORK*)mywk;
  
  int            seq_prev;
  
  seq_prev = *seq;
  *seq = (func_tbl[*seq])(work, &(work->sub_seq));
  if( *seq != SEQ_END )
  {
    if( *seq != seq_prev )
    {
      work->sub_seq = 0;
    }
  }
  else
  {
    return GFL_PROC_RES_FINISH;
  }

  if( SEQ_S01_INIT <= (*seq) && (*seq) <= SEQ_S02_EXIT )  // ���̎��_��*seq�͎��̃V�[�P���X�ɂȂ��Ă���
  {
    // ���C��
    PRINTSYS_QUE_Main( work->print_que );
    
    Psel_TextMain( work );

    // 2D�`��
    PSEL_GRAPHIC_2D_Draw( work->graphic );

    // 3D�`��
    PSEL_GRAPHIC_3D_StartDraw( work->graphic );
    Psel_ThreeDraw( work );
    PSEL_GRAPHIC_3D_EndDraw( work->graphic );
  }

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// VBlank�֐�
//=====================================
static void Psel_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  PSEL_WORK* work = (PSEL_WORK*)wk;
}

//-------------------------------------
/// 3D�`��(GRAPHIC_3D_StartDraw��PSEL_GRAPHIC_3D_EndDraw�̊ԂŌĂ�)
//=====================================
static void Psel_ThreeDraw( PSEL_WORK* work )
{
  u16 i;
  for( i=0; i<work->three_obj_prop_num; i++ )
  {
    THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[i]);
    if( prop->draw )
    {
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
      GFL_G3D_DRAW_DrawObject( obj, &(prop->objstatus) );
    }
  }
}

//-------------------------------------
/// 3D�S��
//=====================================
static void Psel_ThreeInit( PSEL_WORK* work )
{
  // 3D�Ǘ����[�e�B���e�B�[�̃Z�b�g�A�b�v
  work->three_util = GFL_G3D_UTIL_Create( THREE_RES_MAX, THREE_OBJ_MAX, work->heap_id );

  // NULL�A�[��������
  work->three_obj_prop_num = 0;
}
static void Psel_ThreeExit( PSEL_WORK* work )
{
  // 3D�Ǘ����[�e�B���e�B�[�̔j��
  GFL_G3D_UTIL_Delete( work->three_util );
}

//-------------------------------------
/// 3D��
//=====================================
static void Psel_ThreeS01Init( PSEL_WORK* work )
{
  // ���j�b�g�ǉ�
  {
    u16 i = THREE_SETUP_IDX_S01;
    {
      work->three_unit_idx[i] = GFL_G3D_UTIL_AddUnit( work->three_util, &three_setup_tbl[i] );
    }
  }

  // �I�u�W�F�N�g�S��
  {
    work->three_obj_prop_num = 1;
    work->three_obj_prop_tbl = ThreeObjPropertyCreateArray( work->heap_id, work->three_obj_prop_num );
  }

  // �I�u�W�F�N�g
  {
    u16 h = 0;

    u16 i = THREE_SETUP_IDX_S01;
    {
      u16 head_obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( work->three_util, work->three_unit_idx[i] );
      u16 head_user_obj_idx = THREE_USER_OBJ_IDX_S01;
      u16 j = 0;
      {
        THREE_OBJ_PROPERTY* prop;
        work->three_obj_prop_tbl_idx[head_user_obj_idx +j] = h;
        prop = &(work->three_obj_prop_tbl[h]);
        prop->idx  = head_obj_idx +j;
        prop->draw = TRUE;
        h++;
      }
    }
  }

  // �A�j���[�V�����L����
  {
    u16 i;
    for( i=0; i<work->three_obj_prop_num; i++ )
    {
      THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[i]);
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
      u16 anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      u16 j;
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_EnableAnime( obj, j );
      }
    }
  }

  // �J�����A�j��
  {
    work->three_ica_anime = ICA_ANIME_CreateStreamingAlloc(
        work->heap_id, ARCID_PSEL, NARC_psel_psel_s01_cam_bin, THREE_ICA_ANIME_INTERVAL );
    
    ICA_CAMERA_SetCameraStatus( PSEL_GRAPHIC_GetCamera( work->graphic ), work->three_ica_anime );
  }
}
static void Psel_ThreeS01Exit( PSEL_WORK* work )
{
  // ���j�b�g�j��
  {
    u16 i = THREE_SETUP_IDX_S01;
    {
      GFL_G3D_UTIL_DelUnit( work->three_util, work->three_unit_idx[i] );
    }
  }

  // �I�u�W�F�N�g�S��
  {
    ThreeObjPropertyDeleteArray( work->three_obj_prop_tbl );
    work->three_obj_prop_num = 0;
  }

  // �J�����A�j��
  {
    ICA_ANIME_Delete( work->three_ica_anime );
  }
}
static void Psel_ThreeS01Main( PSEL_WORK* work )
{
  const fx32 anime_speed = FX32_ONE;  // �������iFX32_ONE�łP�t���[���i�߂�j
  
  // �A�j���[�V�����X�V
  {
    u16 i;
    for( i=0; i<work->three_obj_prop_num; i++ )
    {
      THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[i]);
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
      u16 anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      u16 j;
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_LoopAnimeFrame( obj, j, anime_speed );
      }
    }
  }

  // �J�����A�j��
  {
    ICA_ANIME_IncAnimeFrame( work->three_ica_anime, anime_speed );
    ICA_CAMERA_SetCameraStatus( PSEL_GRAPHIC_GetCamera( work->graphic ), work->three_ica_anime );
  }
}

static void Psel_ThreeS02Init( PSEL_WORK* work )
{
  // ���j�b�g�ǉ�
  {
    u16 i = THREE_SETUP_IDX_S02;
    {
      work->three_unit_idx[i] = GFL_G3D_UTIL_AddUnit( work->three_util, &three_setup_tbl[i] );
    }
  }

  // �I�u�W�F�N�g�S��
  {
    work->three_obj_prop_num = THREE_USER_OBJ_IDX_MB_R - THREE_USER_OBJ_IDX_S02 +1;
    work->three_obj_prop_tbl = ThreeObjPropertyCreateArray( work->heap_id, work->three_obj_prop_num );
  }

  // �I�u�W�F�N�g
  {
    u16 h = 0;
    
    u16 i = THREE_SETUP_IDX_S02;
    {
      u16 head_obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( work->three_util, work->three_unit_idx[i] );
      u16 head_user_obj_idx = THREE_USER_OBJ_IDX_S02;
      u16 j;
      for( j=0; j<THREE_USER_OBJ_IDX_MB_R - THREE_USER_OBJ_IDX_S02 +1; j++ )
      {
        THREE_OBJ_PROPERTY* prop;
        work->three_obj_prop_tbl_idx[head_user_obj_idx +j] = h;
        prop = &(work->three_obj_prop_tbl[h]);
        prop->idx  = head_obj_idx +j;
        prop->draw = TRUE;
        if( THREE_USER_OBJ_IDX_MB_L <= (head_user_obj_idx +j) && (head_user_obj_idx +j) <= THREE_USER_OBJ_IDX_MB_R )
          prop->objstatus.trans = three_mb_trans[(head_user_obj_idx +j) - THREE_USER_OBJ_IDX_MB_L];
        h++;
      }
    }
  }

  // �A�j���[�V�����L����
  {
    u16 i;
    for( i=0; i<work->three_obj_prop_num; i++ )
    {
      THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[i]);
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
      u16 anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      u16 j;
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_EnableAnime( obj, j );
      }
    }
  }

  // �J�����A�j��
  {
    work->three_ica_anime = ICA_ANIME_CreateStreamingAlloc(
        work->heap_id, ARCID_PSEL, NARC_psel_psel_s02_cam_bin, THREE_ICA_ANIME_INTERVAL );
    
    ICA_CAMERA_SetCameraStatus( PSEL_GRAPHIC_GetCamera( work->graphic ), work->three_ica_anime );
  }
}
static void Psel_ThreeS02Exit( PSEL_WORK* work )
{
  // ���j�b�g�j��
  {
    u16 i = THREE_SETUP_IDX_S02;
    {
      GFL_G3D_UTIL_DelUnit( work->three_util, work->three_unit_idx[i] );
    }
  }

  // �I�u�W�F�N�g�S��
  {
    ThreeObjPropertyDeleteArray( work->three_obj_prop_tbl );
    work->three_obj_prop_num = 0;
  }

  // �J�����A�j��
  {
    ICA_ANIME_Delete( work->three_ica_anime );
  }
}
static void Psel_ThreeS02Main( PSEL_WORK* work )
{
  const fx32 anime_speed = FX32_ONE;  // �������iFX32_ONE�łP�t���[���i�߂�j
  
  // �A�j���[�V�����X�V
  {
    u16 i;
    for( i=0; i<work->three_obj_prop_num; i++ )
    {
      THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[i]);
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
      u16 anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      u16 j;
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_LoopAnimeFrame( obj, j, anime_speed );
      }
    }
  }
  
  // �J�����A�j��
  {
    ICA_ANIME_IncAnimeFrame( work->three_ica_anime, anime_speed );
    ICA_CAMERA_SetCameraStatus( PSEL_GRAPHIC_GetCamera( work->graphic ), work->three_ica_anime );
  }
}

//-------------------------------------
/// �|�P�����召�Z�b�g
//=====================================
static void Psel_PokeSetInit( PSEL_WORK* work )
{
  u8 i;
  u8 j;

  int  formno       = 0;                  // 0<=formno<POKETOOL_GetPersonalParam( monsno, 0, POKEPER_ID_form_max )
  int  sex          = PTL_SEX_MALE;       // PTL_SEX_MALE, PTL_SEX_FEMALE, PTL_SEX_UNKNOWN
  int  rare         = 0;                  // 0(���A����Ȃ�)��1(���A)  // PP_CheckRare( pp ) 
  int  dir          = POKEGRA_DIR_FRONT;  // POKEGRA_DIR_FRONT, POKEGRA_DIR_BACK
  BOOL egg          = FALSE;              // FALSE(�^�}�S����Ȃ�)��TRUE(�^�}�S)
  u32  personal_rnd = 0;                  // ?

  // small
  j = POKE_SMALL;
  // ���\�[�X�̓ǂݍ���
  {
    ARCHANDLE* handle = POKE2DGRA_OpenHandle( work->heap_id );
    for( i=0; i<TARGET_POKE_MAX; i++ )
    {
      work->poke_set[i].res[j][TWO_OBJ_RES_NCG] = POKE2DGRA_OBJ_CGR_Register(
          handle,
          poke_monsno[i],
          formno, sex, rare, dir, egg,
          personal_rnd,
          CLSYS_DRAW_SUB,
          work->heap_id );

      work->poke_set[i].res[j][TWO_OBJ_RES_NCL] = POKE2DGRA_OBJ_PLTT_Register(
          handle,
          poke_monsno[i],
          formno, sex, rare, dir, egg,
          CLSYS_DRAW_SUB,
          poke_obj_pal_pos_s[i] * 0x20,
          work->heap_id );

      work->poke_set[i].res[j][TWO_OBJ_RES_NCE] = POKE2DGRA_OBJ_CELLANM_Register(
          poke_monsno[i],
          formno, sex, rare, dir, egg,
          APP_COMMON_MAPPING_32K,
          CLSYS_DRAW_SUB,
          work->heap_id );
    }
    GFL_ARC_CloseDataHandle( handle );
  }
  // CLWK�쐬
  {
    for( i=0; i<TARGET_POKE_MAX; i++ )
    {
      GFL_CLWK_DATA cldata;
      GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

      cldata.pos_x    = poke_pos[i][0];
      cldata.pos_y    = poke_pos[i][1];
      cldata.anmseq   = 0;
      cldata.softpri  = 0;
      cldata.bgpri    = OBJ_BG_PRI_S_POKE;

      work->poke_set[i].clwk[j] = GFL_CLACT_WK_Create(
          PSEL_GRAPHIC_GetClunit( work->graphic ),
          work->poke_set[i].res[j][TWO_OBJ_RES_NCG],
          work->poke_set[i].res[j][TWO_OBJ_RES_NCL],
          work->poke_set[i].res[j][TWO_OBJ_RES_NCE],
          &cldata, CLSYS_DEFREND_SUB, work->heap_id );

      //GFL_CLACT_WK_SetObjMode( work->poke_set[i].clwk[j], GX_OAM_MODE_XLU );  // ������OBJ�ɂ͂����A�p���b�g�A�j���ňÂ�����
    
      
      GFL_CLACT_WK_SetDrawEnable( work->poke_set[i].clwk[j], FALSE );
   
    
    }
  }

  // big
  j = POKE_BIG;
  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_PSEL, work->heap_id );
    for( i=0; i<TARGET_POKE_MAX; i++ )
    {
/*
      small��big�͓����p���b�g�Ȃ̂ŁAsamll�̂��g�p����
      work->poke_set[i].res[j][TWO_OBJ_RES_NCL] = GFL_CLGRP_PLTT_RegisterEx( 
          handle,
          poke_res_big[i][TWO_OBJ_RES_NCL],
          CLSYS_DRAW_SUB,
          poke_obj_pal_pos_s[i] * 0x20,
          0,  // �f�[�^�ǂݏo���J�n�ʒu��0�ɌŒ肵�Ă��邪���v���ȁA�f�U�C�i����ɂ��肢���Ă����Ȃ���
          1,  // �]���{����1�ɌŒ肵�Ă��邪���v���ȁA�f�U�C�i����ɂ��肢���Ă����Ȃ���
          work->heap_id );
*/
      work->poke_set[i].res[j][TWO_OBJ_RES_NCG] = GFL_CLGRP_CGR_Register(
          handle,
          poke_res_big[i][TWO_OBJ_RES_NCG],
          FALSE,
          CLSYS_DRAW_SUB,
          work->heap_id );
      work->poke_set[i].res[j][TWO_OBJ_RES_NCE] = GFL_CLGRP_CELLANIM_Register(
          handle,
          poke_res_big[i][TWO_OBJ_RES_NCE],
          poke_res_big[i][TWO_OBJ_RES_NAN],
          work->heap_id );
    }
    GFL_ARC_CloseDataHandle( handle );
  }
  // CLWK�쐬
  {
    for( i=0; i<TARGET_POKE_MAX; i++ )
    {
      GFL_CLWK_AFFINEDATA claffinedata;
      GFL_STD_MemClear( &claffinedata, sizeof(GFL_CLWK_AFFINEDATA) );

      claffinedata.clwkdata.pos_x    = poke_pos[i][0];
      claffinedata.clwkdata.pos_y    = poke_pos[i][1];
      claffinedata.clwkdata.anmseq   = 0;
      claffinedata.clwkdata.softpri  = 0;
      claffinedata.clwkdata.bgpri    = OBJ_BG_PRI_S_POKE;

      claffinedata.affinepos_x    = 0;
      claffinedata.affinepos_y    = 0;
      claffinedata.scale_x        = FX_F32_TO_FX32(1.0f);//FX_F32_TO_FX32(0.3f);
      claffinedata.scale_y        = FX_F32_TO_FX32(1.0f);//FX_F32_TO_FX32(0.3f);
      claffinedata.rotation       = 0;  // ��]�p�x(0�`0xffff 0xffff��360�x)
      claffinedata.affine_type    = CLSYS_AFFINETYPE_NORMAL;  // �k���������Ȃ��̂ŁA�{�p�łȂ��Ă悢

      work->poke_set[i].clwk[j] = GFL_CLACT_WK_CreateAffine(
          PSEL_GRAPHIC_GetClunit( work->graphic ),
          work->poke_set[i].res[j][TWO_OBJ_RES_NCG],
          //work->poke_set[i].res[j][TWO_OBJ_RES_NCL],
          work->poke_set[i].res[POKE_SMALL][TWO_OBJ_RES_NCL],
          work->poke_set[i].res[j][TWO_OBJ_RES_NCE],
          &claffinedata, CLSYS_DEFREND_SUB, work->heap_id );

      //GFL_CLACT_WK_SetObjMode( work->poke_set[i].clwk[j], GX_OAM_MODE_XLU );  // ������OBJ�ɂ͂����A�p���b�g�A�j���ňÂ�����
    }
  }
}
static void Psel_PokeSetExit( PSEL_WORK* work )
{
  u8 i;
  u8 j;
  for( i=0; i<TARGET_POKE_MAX; i++ )
  {
    for( j=0; j<POKE_MAX; j++ )
    {
      // CLWK�j��
      GFL_CLACT_WK_Remove( work->poke_set[i].clwk[j] );

      // ���\�[�X�j��
      GFL_CLGRP_CELLANIM_Release( work->poke_set[i].res[j][TWO_OBJ_RES_NCE] );
      GFL_CLGRP_CGR_Release( work->poke_set[i].res[j][TWO_OBJ_RES_NCG] );
      if( j != POKE_BIG ) GFL_CLGRP_PLTT_Release( work->poke_set[i].res[j][TWO_OBJ_RES_NCL] );  // small��big�͓����p���b�g�Ȃ̂ŁAsmall�̂����ǂݍ���ł��Ȃ�
    }
  }
}

//-------------------------------------
/// �w�w���J�[�\��
//=====================================
static void Psel_FingerInit( PSEL_WORK* work )
{
  // ���\�[�X�ǂݍ���
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_PSEL, work->heap_id );
    work->finger_res[TWO_OBJ_RES_NCL] = GFL_CLGRP_PLTT_RegisterEx( 
        handle,
        NARC_psel_psel_cursol_NCLR,
        CLSYS_DRAW_MAIN,
        OBJ_PAL_POS_M_FINGER * 0x20,
        0,
        OBJ_PAL_NUM_M_FINGER,
        work->heap_id );
    work->finger_res[TWO_OBJ_RES_NCG] = GFL_CLGRP_CGR_Register(
        handle,
        NARC_psel_psel_cursol_NCGR,
        FALSE,
        CLSYS_DRAW_MAIN,
        work->heap_id );
    work->finger_res[TWO_OBJ_RES_NCE] = GFL_CLGRP_CELLANIM_Register(
        handle,
        NARC_psel_psel_cursol_NCER,
        NARC_psel_psel_cursol_NANR,
        work->heap_id );
    GFL_ARC_CloseDataHandle( handle );
  }

  // CLWK�쐬
  {
    GFL_CLWK_DATA cldata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    cldata.pos_x    = finger_pos[TARGET_MIZU][0];
    cldata.pos_y    = finger_pos[TARGET_MIZU][1];
    cldata.anmseq   = 0;
    cldata.softpri  = 0;
    cldata.bgpri    = OBJ_BG_PRI_M_FINGER;

    work->finger_clwk = GFL_CLACT_WK_Create(
        PSEL_GRAPHIC_GetClunit( work->graphic ),
        work->finger_res[TWO_OBJ_RES_NCG], work->finger_res[TWO_OBJ_RES_NCL], work->finger_res[TWO_OBJ_RES_NCE],
        &cldata, CLSYS_DEFREND_MAIN, work->heap_id );
/*
    GFL_CLACT_WK_SetAnmSeq( work->obj_clwk, anmseq );
    GFL_CLACT_WK_SetAutoAnmFlag( work->obj_clwk, TRUE );
    GFL_CLACT_WK_SetBgPri( work->obj_clwk, bgpri );
    GFL_CLACT_WK_SetSoftPri( work->obj_clwk, softpri );
    GFL_CLACT_WK_SetDrawEnable( work->obj_clwk, FALSE );
    GFL_CLACT_WK_SetObjMode( work->obj_clwk, GX_OAM_MODE_XLU );  // BG�ƂƂ��ɂ���OBJ���Â��������̂�
    {
      GFL_CLACTPOS pos;
      GFL_CLACT_WK_SetPos( work->obj_clwk, &pos, CLSYS_DEFREND_MAIN );
    }
*/
  }
}
static void Psel_FingerExit( PSEL_WORK* work )
{
  // CLWK�j��
  GFL_CLACT_WK_Remove( work->finger_clwk );
  
  // ���\�[�X�j��
  GFL_CLGRP_CELLANIM_Release( work->finger_res[TWO_OBJ_RES_NCE] );
  GFL_CLGRP_CGR_Release( work->finger_res[TWO_OBJ_RES_NCG] );
  GFL_CLGRP_PLTT_Release( work->finger_res[TWO_OBJ_RES_NCL] );
}

//-------------------------------------
/// BG
//=====================================
static void Psel_BgInit( PSEL_WORK* work )
{
  // �p���b�g�̍쐬���]��
  {
    u16* pal = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(u16) * 0x10 );
    pal[0x00] = 0x0000;  // ����
    pal[0x01] = 0x0000;  // ��
    pal[0x02] = 0x7fff;  // ��

    GFL_BG_LoadPalette( BG_FRAME_M_WIN, pal, 0x20, BG_PAL_POS_M_WIN * 0x20 );
    GFL_BG_LoadPalette( BG_FRAME_S_WIN, pal, 0x20, BG_PAL_POS_S_WIN * 0x20 );
    
    GFL_HEAP_FreeMemory( pal );
  }

  // �L�����̍쐬���]��
  {
    GFL_BMP_DATA* bmp = GFL_BMP_Create( 2, 1, GFL_BMP_16_COLOR, work->heap_id );
    GFL_BMP_Fill( bmp, 0, 0, 8, 8, 0x00 );  // 0�L������
    GFL_BMP_Fill( bmp, 8, 0, 8, 8, 0x01 );  // 1�L������

    GFL_BG_LoadCharacter( BG_FRAME_M_WIN,
        GFL_BMP_GetCharacterAdrs(bmp), GFL_BMP_GetBmpDataSize(bmp),
        0 );
    GFL_BG_LoadCharacter( BG_FRAME_S_WIN,
        GFL_BMP_GetCharacterAdrs(bmp), GFL_BMP_GetBmpDataSize(bmp),
        0 );

    GFL_BMP_Delete( bmp );
  }
}
static void Psel_BgExit( PSEL_WORK* work )
{
  // �������Ȃ�
}

static void Psel_BgS01Init( PSEL_WORK* work )
{
  // �X�N���[���̍쐬���]��
  {
    u16* scr = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(u16) * 32*24 );
    u8 i, j;
    u16 h = 0;
    for(j=0; j<24; j++)       // TWL_ProgramingManual.pdf TWL�v���O���~���O�}�j���A��
    {                         // 6.2.3.2.2 �X�N���[���f�[�^�̃A�h���X�}�b�s���O
      for(i=0; i<32; i++)     // �X�N���[���T�C�Y��256�~256 �h�b�g�̂Ƃ�
      {                       // ���Q�l�ɂ����B
        u16 chara_name = (3<=j&&j<21)?1:0;  // 1�L������:0�L������
        u16 flip_h     = 0;
        u16 flip_v     = 0;
        u16 pal        = BG_PAL_POS_M_WIN;
        scr[h] = ( pal << 12 ) | ( flip_v << 11 ) | ( flip_h << 10 ) | ( chara_name << 0 );
        h++;
      }
    }

    GFL_BG_WriteScreen( BG_FRAME_M_WIN, scr, 0, 0, 32, 24 );
    GFL_BG_LoadScreenV_Req( BG_FRAME_M_WIN );
    
    GFL_HEAP_FreeMemory( scr );
  }
}
static void Psel_BgS01Exit( PSEL_WORK* work )
{
  u16 chara_name = 0;  // 0�L������
  u16 flip_h     = 0;
  u16 flip_v     = 0;
  u16 pal        = BG_PAL_POS_M_WIN;
  u16 scr = ( pal << 12 ) | ( flip_v << 11 ) | ( flip_h << 10 ) | ( chara_name << 0 );
  GFL_BG_ClearScreenCodeVReq( BG_FRAME_M_WIN, scr );
}
static void Psel_BgS01Main( PSEL_WORK* work )
{
}

static void Psel_BgS02Init( PSEL_WORK* work )
{
  // �X�N���[���̍쐬���]��
  {
    u16* scr = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(u16) * 32*24 );
    u8 i, j;
    u16 h = 0;
    for(j=0; j<24; j++)       // TWL_ProgramingManual.pdf TWL�v���O���~���O�}�j���A��
    {                         // 6.2.3.2.2 �X�N���[���f�[�^�̃A�h���X�}�b�s���O
      for(i=0; i<32; i++)     // �X�N���[���T�C�Y��256�~256 �h�b�g�̂Ƃ�
      {                       // ���Q�l�ɂ����B
        u16 chara_name = (17<=j&&j<23)?1:0;  // 1�L������:0�L������
        u16 flip_h     = 0;
        u16 flip_v     = 0;
        u16 pal        = BG_PAL_POS_M_WIN;
        scr[h] = ( pal << 12 ) | ( flip_v << 11 ) | ( flip_h << 10 ) | ( chara_name << 0 );
        h++;
      }
    }

    GFL_BG_WriteScreen( BG_FRAME_M_WIN, scr, 0, 0, 32, 24 );
    GFL_BG_LoadScreenV_Req( BG_FRAME_M_WIN );
    
    GFL_HEAP_FreeMemory( scr );
  }


  // �X�N���[���̍쐬���]��
  {
    u16* scr = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(u16) * 32*24 );
    u8 i, j;
    u16 h = 0;
    for(j=0; j<24; j++)       // TWL_ProgramingManual.pdf TWL�v���O���~���O�}�j���A��
    {                         // 6.2.3.2.2 �X�N���[���f�[�^�̃A�h���X�}�b�s���O
      for(i=0; i<32; i++)     // �X�N���[���T�C�Y��256�~256 �h�b�g�̂Ƃ�
      {                       // ���Q�l�ɂ����B
        u16 chara_name = (17<=j&&j<23)?1:0;  // 1�L������:0�L������
        u16 flip_h     = 0;
        u16 flip_v     = 0;
        u16 pal        = BG_PAL_POS_S_WIN;
        scr[h] = ( pal << 12 ) | ( flip_v << 11 ) | ( flip_h << 10 ) | ( chara_name << 0 );
        h++;
      }
    }

    GFL_BG_WriteScreen( BG_FRAME_S_WIN, scr, 0, 0, 32, 24 );
    GFL_BG_LoadScreenV_Req( BG_FRAME_S_WIN );
    
    GFL_HEAP_FreeMemory( scr );
  }
}
static void Psel_BgS02Exit( PSEL_WORK* work )
{
  {
  u16 chara_name = 0;  // 0�L������
  u16 flip_h     = 0;
  u16 flip_v     = 0;
  u16 pal        = BG_PAL_POS_M_WIN;
  u16 scr = ( pal << 12 ) | ( flip_v << 11 ) | ( flip_h << 10 ) | ( chara_name << 0 );
  GFL_BG_ClearScreenCodeVReq( BG_FRAME_M_WIN, scr );
  }


  {
  u16 chara_name = 0;  // 0�L������
  u16 flip_h     = 0;
  u16 flip_v     = 0;
  u16 pal        = BG_PAL_POS_S_WIN;
  u16 scr = ( pal << 12 ) | ( flip_v << 11 ) | ( flip_h << 10 ) | ( chara_name << 0 );
  GFL_BG_ClearScreenCodeVReq( BG_FRAME_S_WIN, scr );
  }
}
static void Psel_BgS02Main( PSEL_WORK* work )
{
}

static void Psel_InsideBallInit( PSEL_WORK* work )
{
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_PSEL, work->heap_id );

  GFL_ARCHDL_UTIL_TransVramPaletteEx(
      handle,
      inside_ball_res[INSIDE_BALL_NONE],
      PALTYPE_SUB_BG,
      BG_PAL_POS_S_INSIDE_BALL * 0x20,
      BG_PAL_POS_S_INSIDE_BALL * 0x20,
      BG_PAL_NUM_S_INSIDE_BALL * 0x20,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      handle,
      NARC_psel_psel_bg02_NCGR,
      BG_FRAME_S_INSIDE_BALL,
			0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramScreen(
      handle,
      NARC_psel_psel_bg02_NSCR,
      BG_FRAME_S_INSIDE_BALL,
      0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenReq( BG_FRAME_S_INSIDE_BALL );
}
static void Psel_InsideBallExit( PSEL_WORK* work )
{
  // �������Ȃ�
}

//-------------------------------------
/// �e�L�X�g
//=====================================
static void Psel_TextInit( PSEL_WORK* work )
{
  u8 i;

  // �p���b�g
  GFL_ARC_UTIL_TransVramPalette(
      ARCID_FONT,
      NARC_font_default_nclr,
      PALTYPE_MAIN_BG,
      BG_PAL_POS_M_TEXT * 0x20,
      BG_PAL_NUM_M_TEXT * 0x20,
      work->heap_id );

  GFL_ARC_UTIL_TransVramPalette(
      ARCID_FONT,
      NARC_font_default_nclr,
      PALTYPE_SUB_BG,
      BG_PAL_POS_S_TEXT * 0x20,
      BG_PAL_NUM_S_TEXT * 0x20,
      work->heap_id );

  // ���b�Z�[�W
  work->msgdata = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, NARC_script_message_t01r0102_dat, work->heap_id );

  // TCBL
  work->ps_tcblsys = GFL_TCBL_Init( work->heap_id, work->heap_id, 1, 0 );

  // ���b�Z�[�W����L�[�J�[�\���A�C�R��
  work->app_keycursor_wk = APP_KEYCURSOR_CreateEx( TEXT_COLOR_B, TRUE, TRUE, work->heap_id, (u8*)sc_skip_cursor_character );

  // �t�H���g
  GFL_FONTSYS_SetColor( TEXT_COLOR_L, TEXT_COLOR_S, TEXT_COLOR_B );

  // �r�b�g�}�b�v�E�B���h�E
  for( i=0; i<TEXT_MAX; i++ )
  {
    work->text_bmpwin[i] = GFL_BMPWIN_Create(
                            bmpwin_setup[i][0],
                            bmpwin_setup[i][1], bmpwin_setup[i][2], bmpwin_setup[i][3], bmpwin_setup[i][4],
                            bmpwin_setup[i][5],
                            bmpwin_setup[i][6] );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[i]), TEXT_COLOR_B );
    GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[i] );
    work->text_finish[i] = TRUE;
  }

  // NULL�A�[��������
  work->print_stream = NULL;
  work->ps_strbuf = NULL;
}
static void Psel_TextExit( PSEL_WORK* work )
{
  u8 i;

  if( work->print_stream ) PRINTSYS_PrintStreamDelete( work->print_stream );
  if( work->ps_strbuf ) GFL_STR_DeleteBuffer( work->ps_strbuf );

  // �r�b�g�}�b�v�E�B���h�E
  for( i=0; i<TEXT_MAX; i++ )
  {
    GFL_BMPWIN_Delete( work->text_bmpwin[i] );
  }

  // ���b�Z�[�W����L�[�J�[�\���A�C�R��
  APP_KEYCURSOR_Delete( work->app_keycursor_wk );

  // TCBL
  GFL_TCBL_Exit( work->ps_tcblsys );

  // ���b�Z�[�W
  GFL_MSG_Delete( work->msgdata );
}
static void Psel_TextMain( PSEL_WORK* work )
{
  Psel_TextTransWait( work );
}

static BOOL Psel_TextTransWait( PSEL_WORK* work )  // TRUE���Ԃ��ė�����]���҂��Ȃ�
{
  BOOL ret = TRUE;
  u8 i;

  for( i=0; i<TEXT_MAX; i++ )
  {
    if( !(work->text_finish[i]) )
    {
      if( PRINTSYS_QUE_IsExistTarget( work->print_que, GFL_BMPWIN_GetBmp(work->text_bmpwin[i]) ) )
      {
        ret = FALSE;
      }
      else
      {
        GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[i] );
        work->text_finish[i] = TRUE;
      }
    }
  }

  return ret;
}

static void Psel_TextStreamStart( PSEL_WORK* work, GFL_BMPWIN* bmpwin, u32 str_id )
{
  s16 x = 0;
  s16 y = 0;

  // ��U����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmpwin), TEXT_COLOR_B );
  
  // �O�̂�����
  if( work->print_stream ) PRINTSYS_PrintStreamDelete( work->print_stream );  // ���̊֐��Ń^�X�N���폜���Ă����̂ŁA�����ɓ����^�X�N��1�ōς�
  if( work->ps_strbuf ) GFL_STR_DeleteBuffer( work->ps_strbuf );

  // ������쐬
  {
    work->ps_strbuf  = GFL_MSG_CreateString( work->msgdata, str_id );
    // �Z���^�����O����Ȃ炱����
  }
  
  // �X�g���[���J�n
  work->print_stream = PRINTSYS_PrintStream(
                           bmpwin,
                           x, y,
                           work->ps_strbuf,
                           work->font,
                           MSGSPEED_GetWait(),
                           work->ps_tcblsys, 2,
                           work->heap_id,
                           TEXT_COLOR_B );
}
static BOOL Psel_TextStreamWait( PSEL_WORK* work )  // TRUE���Ԃ��ė�����I��
{
  BOOL ret = FALSE;

  switch( PRINTSYS_PrintStreamGetState( work->print_stream ) )
  {
  case PRINTSTREAM_STATE_RUNNING:
    if( ( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || GFL_UI_TP_GetTrg() )
    {
      PRINTSYS_PrintStreamShortWait( work->print_stream, 0 );
    }
    break;
  case PRINTSTREAM_STATE_PAUSE:
    if( ( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || GFL_UI_TP_GetTrg() )
    { 
      PRINTSYS_PrintStreamReleasePause( work->print_stream );
    }
    break;
  case PRINTSTREAM_STATE_DONE:
    {
      ret = TRUE;
    }
    break;
  }
  
  return ret;
}

static void Psel_TextS01Init( PSEL_WORK* work )
{
  STRBUF* strbuf      = GFL_MSG_CreateString( work->msgdata, msg_t01r0102_letter_01 );

  u16     str_width   = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
  u16     str_height  = (u16)( PRINTSYS_GetStrHeight( strbuf, work->font ) ); 

  u16     bmp_width   = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_LETTER]) );
  u16     bmp_height  = GFL_BMP_GetSizeY( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_LETTER]) );
 
  u16     x = 0;
  u16     y = 0;

  // �Z���^�����O
  if( bmp_width > str_width )
  {
    x = ( bmp_width - str_width ) / 2;
  }
  if( bmp_height > str_height )
  {
    y = ( bmp_height - str_height ) / 2;
  }
  
  PRINTSYS_PrintQueColor(
      work->print_que,
      GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_LETTER]),
      x, y,
      strbuf,
      work->font,
      PRINTSYS_LSB_Make(TEXT_COLOR_L,TEXT_COLOR_S,TEXT_COLOR_B) );

  GFL_STR_DeleteBuffer( strbuf );

  work->text_finish[TEXT_LETTER] = FALSE;
}
static void Psel_TextS01Exit( PSEL_WORK* work )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_LETTER]), TEXT_COLOR_B );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[TEXT_LETTER] );
}
static void Psel_TextS01Main( PSEL_WORK* work )
{
  if( Psel_TextTransWait( work ) )
  {
    APP_KEYCURSOR_Write( work->app_keycursor_wk, GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_LETTER]), TEXT_COLOR_B );
    GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_LETTER] );
  }

  //APP_KEYCURSOR_Clear( work->app_keycursor_wk, GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_LETTER]), TEXT_COLOR_B );
  //GFL_BMPWIN_TransVramCharacter( work->text_bmpwin[TEXT_LETTER] );
}

static void Psel_TextS02Init( PSEL_WORK* work )
{
  {
  STRBUF* strbuf      = GFL_MSG_CreateString( work->msgdata, msg_t01r0102_ball_select_02 );

  u16     str_width   = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
  u16     str_height  = (u16)( PRINTSYS_GetStrHeight( strbuf, work->font ) ); 

  u16     bmp_width   = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_EXPLAIN]) );
  u16     bmp_height  = GFL_BMP_GetSizeY( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_EXPLAIN]) );
 
  u16     x = 0;
  u16     y = 0;

  // �Z���^�����O
  if( bmp_width > str_width )
  {
    x = ( bmp_width - str_width ) / 2;
  }
  if( bmp_height > str_height )
  {
    y = ( bmp_height - str_height ) / 2;
  }
  
  PRINTSYS_PrintQueColor(
      work->print_que,
      GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_EXPLAIN]),
      x, y,
      strbuf,
      work->font,
      PRINTSYS_LSB_Make(TEXT_COLOR_L,TEXT_COLOR_S,TEXT_COLOR_B) );

  GFL_STR_DeleteBuffer( strbuf );

  work->text_finish[TEXT_EXPLAIN] = FALSE;
  }


  {
  u16     str_width;
  u16     str_height; 

  u16     bmp_width;
  u16     bmp_height;
 
  u16     x = 0;
  u16     y = 0;

  WORDSET*  wordset       = WORDSET_Create( work->heap_id );
  STRBUF*   src_strbuf    = GFL_MSG_CreateString( work->msgdata, msg_t01r0102_ball_water_01 );
  STRBUF*   strbuf        = GFL_STR_CreateBuffer( TEXT_POKE_INFO_LEN_MAX, work->heap_id );
  WORDSET_RegisterPokeMonsNameNo( wordset, 1, poke_monsno[TARGET_MIZU] );
  WORDSET_ExpandStr( wordset, strbuf, src_strbuf );
  GFL_STR_DeleteBuffer( src_strbuf );
  WORDSET_Delete( wordset );

  str_width   = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
  str_height  = (u16)( PRINTSYS_GetStrHeight( strbuf, work->font ) ); 

  bmp_width   = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_POKE_INFO]) );
  bmp_height  = GFL_BMP_GetSizeY( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_POKE_INFO]) );
 
  x = 0;
  y = 0;

  // �Z���^�����O
  //if( bmp_width > str_width )            // gmm�ɃZ���^�����O�w������Ă���̂�
  //{                                      // �v���O�����ŉ��߂ăZ���^�����O����K�v�͂Ȃ��̂�
  //  x = ( bmp_width - str_width ) / 2;   // �R�����g�A�E�g����
  //}
  if( bmp_height > str_height )
  {
    y = ( bmp_height - str_height ) / 2;
  }
  
  PRINTSYS_PrintQueColor(
      work->print_que,
      GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_POKE_INFO]),
      x, y,
      strbuf,
      work->font,
      PRINTSYS_LSB_Make(TEXT_COLOR_L,TEXT_COLOR_S,TEXT_COLOR_B) );

  GFL_STR_DeleteBuffer( strbuf );

  work->text_finish[TEXT_POKE_INFO] = FALSE;
  }
}
static void Psel_TextS02Exit( PSEL_WORK* work )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_EXPLAIN]), TEXT_COLOR_B );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[TEXT_EXPLAIN] );


  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_POKE_INFO]), TEXT_COLOR_B );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[TEXT_POKE_INFO] );
}
static void Psel_TextS02Main( PSEL_WORK* work )
{
}

//-------------------------------------
/// APP_TASKMENU
//=====================================
static void Psel_AppTaskmenuResInit( PSEL_WORK* work )
{
  work->app_taskmenu_res = APP_TASKMENU_RES_Create( BG_FRAME_M_TEXT, BG_PAL_POS_M_APP_TASKMENU, work->font, work->print_que, work->heap_id );
}
static void Psel_AppTaskmenuResExit( PSEL_WORK* work )
{
  APP_TASKMENU_RES_Delete( work->app_taskmenu_res );	
}

static void Psel_AppTaskmenuWinInit( PSEL_WORK* work )
{
  // ���̐ݒ�
  APP_TASKMENU_ITEMWORK item;
  item.str       = GFL_MSG_CreateString( work->msgdata, msg_t01r0102_ball_select_03 );
  item.msgColor  = APP_TASKMENU_ITEM_MSGCOLOR;
  item.type      = APP_TASKMENU_WIN_TYPE_NORMAL;

  work->app_taskmenu_win_wk = APP_TASKMENU_WIN_Create(
      work->app_taskmenu_res,
      &item,
      9, 21,
      14,
      work->heap_id );

  // ��������
  GFL_STR_DeleteBuffer( item.str );
}
static void Psel_AppTaskmenuWinExit( PSEL_WORK* work )
{
  APP_TASKMENU_WIN_Delete( work->app_taskmenu_win_wk );
}
static void Psel_AppTaskmenuWinMain( PSEL_WORK* work )
{
  APP_TASKMENU_WIN_Update( work->app_taskmenu_win_wk );
}

static void Psel_AppTaskmenuInit( PSEL_WORK* work )
{
  int i;

  APP_TASKMENU_INITWORK init;
  APP_TASKMENU_ITEMWORK item[2];

  // ���̐ݒ�
  for( i=0; i<NELEMS(item); i++ )
  {
    item[i].str      = GFL_MSG_CreateString( work->msgdata, msg_t01r0102_ball_select_yes +i );
    item[i].msgColor = APP_TASKMENU_ITEM_MSGCOLOR;
    //item[i].type     = APP_TASKMENU_WIN_TYPE_NORMAL;  // NORMAL�ł��Ō���̃{�^����B�{�^�����V���[�g�J�b�g�{�^���ɂȂ��Ă���悤�����AA�{�^���ōŌ���̃{�^����I�Ԃƌ��艹����B�{�^���ōŌ���̃{�^����I�ԂƃL�����Z�������Ȃ�B
    item[i].type     = (i==0) ? APP_TASKMENU_WIN_TYPE_NORMAL : APP_TASKMENU_WIN_TYPE_RETURN;
  }

  // ������
  init.heapId    = work->heap_id;
  init.itemNum   = NELEMS(item);
  init.itemWork  = item;
  init.posType   = ATPT_RIGHT_DOWN;
  init.charPosX  = 32;  // 31�L�����܂ŕ`�����
  init.charPosY  = 17;  // 16�L�����܂ŕ`�����
  //init.w         = 7;  // ���������҂����肨���܂镝
  init.w         = 9;  // �������ƃ��^�[���}�[�N���҂����肨���܂镝  //APP_TASKMENU_PLATE_WIDTH;
  init.h         = APP_TASKMENU_PLATE_HEIGHT;
	
  work->app_taskmenu_wk = APP_TASKMENU_OpenMenu( &init, work->app_taskmenu_res );  // 60fps�p
  //work->app_taskmenu_wk = APP_TASKMENU_OpenMenuEx( &init, work->app_taskmenu_res );  // 30fps�p

  // ��������
  for( i=0; i<NELEMS(item); i++ )
  {
    GFL_STR_DeleteBuffer( item[i].str );
  }
}
static void Psel_AppTaskmenuExit( PSEL_WORK* work )
{
  APP_TASKMENU_CloseMenu( work->app_taskmenu_wk );
}
static void Psel_AppTaskmenuMain( PSEL_WORK* work )
{
  APP_TASKMENU_UpdateMenu( work->app_taskmenu_wk );
}

//-------------------------------------
/// �p���b�g
//=====================================
static void U16ToRGB( u16 rgb, u16* r, u16* g, u16* b )
{
  *r = ( rgb & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
  *g = ( rgb & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
  *b = ( rgb & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
}

static void Psel_PokeSetPalInit( PSEL_WORK* work )
{
  ARCHANDLE* handle;
  NNSG2dPaletteData* pal_data;
  void* buff;
  u8 i;

  // �I���W�i���p���b�g�f�[�^
  // �召�����p���b�g�Ȃ̂ŁA��̃p���b�g�����ɂ����蓖�Ă邱�Ƃɂ���
  handle = GFL_ARC_OpenDataHandle( ARCID_PSEL, work->heap_id );
  for( i=0; i<TARGET_POKE_MAX; i++ )
  {
    buff = GFL_ARCHDL_UTIL_LoadPalette( handle, poke_res_big[i][TWO_OBJ_RES_NCL], &pal_data, work->heap_id );
    GFL_STD_MemCopy( pal_data->pRawData, work->poke_set[i].pal[POKE_PAL_COLOR], 2*16 );  // 0�ԗ񂩂�����Ă���̂ł��̂܂܂ł���
    GFL_HEAP_FreeMemory( buff );
    
    buff = GFL_ARCHDL_UTIL_LoadPalette( handle, poke_res_big[i][TWO_OBJ_RES_DARK_NCL], &pal_data, work->heap_id );
    GFL_STD_MemCopy( pal_data->pRawData, work->poke_set[i].pal[POKE_PAL_DARK], 2*16 );  // 0�ԗ񂩂�����Ă���̂ł��̂܂܂ł���
    GFL_HEAP_FreeMemory( buff );
  }
  GFL_ARC_CloseDataHandle( handle );

  // �p���b�g�A�j���̏��
  for( i=0; i<TARGET_POKE_MAX; i++ )
  {
    // dark�ɂ���
    work->poke_set[i].pal_anime_state = POKE_PAL_ANIME_STATE_DARK;
    work->poke_set[i].pal_anime_count = 16;
  }

/*
  �Ȃ��Ă����v�̂悤��
  // �m���ɓǂݍ��݂��I���� 
  for( i=0; i<TARGET_POKE_MAX; i++ )
  {
    DC_FlushRange( work->poke_set[i].pal[POKE_PAL_COLOR], 2*16 );
    DC_FlushRange( work->poke_set[i].pal[POKE_PAL_DARK], 2*16 );
  }
*/
  
  // ������� 
  for( i=0; i<TARGET_POKE_MAX; i++ )
  {
    // dark�ɂ���
    GFL_STD_MemCopy( work->poke_set[i].pal[POKE_PAL_DARK], work->poke_set[i].pal[POKE_PAL_TRANS], 2*16 );
    NNS_GfdRegisterNewVramTransferTask(
        NNS_GFD_DST_2D_OBJ_PLTT_SUB,
        poke_obj_pal_pos_s[i] * 2*16,
        work->poke_set[i].pal[POKE_PAL_TRANS],
        2*16 );
  }
}
static void Psel_PokeSetPalExit( PSEL_WORK* work )
{
  // �������Ȃ�
}
static void Psel_PokeSetPalMain( PSEL_WORK* work )
{
  u8 i;
  u8 j;

  // 0=color, 16=dark
  
  for( i=0; i<TARGET_POKE_MAX; i++ )
  {
    BOOL need_trans = FALSE;

    switch( work->poke_set[i].pal_anime_state )
    {
    case POKE_PAL_ANIME_STATE_COLOR_TO_DARK:
      {
        if( work->poke_set[i].pal_anime_count == 16 )
        {
          work->poke_set[i].pal_anime_state = POKE_PAL_ANIME_STATE_DARK;
        }
        else
        {
          work->poke_set[i].pal_anime_count++;
          need_trans = TRUE;
        }
      }
      break;
    case POKE_PAL_ANIME_STATE_DARK_TO_COLOR:
      {
        if( work->poke_set[i].pal_anime_count == 0 )
        {
          work->poke_set[i].pal_anime_state = POKE_PAL_ANIME_STATE_COLOR;
        }
        else
        {
          work->poke_set[i].pal_anime_count--;
          need_trans = TRUE;
        }
      }
      break;
    }

    if( need_trans )
    {
      switch( work->poke_set[i].pal_anime_state )
      {
      case POKE_PAL_ANIME_STATE_COLOR_TO_DARK:
      case POKE_PAL_ANIME_STATE_DARK_TO_COLOR:
        {
          for( j=0; j<16; j++ )
          {
            u16 count = work->poke_set[i].pal_anime_count;
            u16 color = work->poke_set[i].pal[POKE_PAL_COLOR][j];
            u16 dark  = work->poke_set[i].pal[POKE_PAL_DARK][j];
            u16 color_r, color_g, color_b;
            u16 dark_r, dark_g, dark_b;
            u16 trans_r, trans_g, trans_b;
            U16ToRGB( color, &color_r, &color_g, &color_b );
            U16ToRGB( dark, &dark_r, &dark_g, &dark_b );
            trans_r = ( color_r*(16-count) + dark_r*(count) ) /16;
            trans_g = ( color_g*(16-count) + dark_g*(count) ) /16;
            trans_b = ( color_b*(16-count) + dark_b*(count) ) /16;
            work->poke_set[i].pal[POKE_PAL_TRANS][j] = GX_RGB( trans_r, trans_g, trans_b );
          }
        }
        break;
      }

      NNS_GfdRegisterNewVramTransferTask(
          NNS_GFD_DST_2D_OBJ_PLTT_SUB,
          poke_obj_pal_pos_s[i] * 2*16,
          work->poke_set[i].pal[POKE_PAL_TRANS],
          2*16 );
    }
  }
}

static void Psel_InsideBallPalInit( PSEL_WORK* work )
{
  ARCHANDLE* handle;
  NNSG2dPaletteData* pal_data;
  u16* raw_data;
  void* buff;
  u8 i;

  // �I���W�i���p���b�g�f�[�^
  handle = GFL_ARC_OpenDataHandle( ARCID_PSEL, work->heap_id );
  for( i=0; i<INSIDE_BALL_MAX; i++ )
  {
    buff = GFL_ARCHDL_UTIL_LoadPalette( handle, inside_ball_res[i], &pal_data, work->heap_id );
    raw_data = pal_data->pRawData;  // BG_PAL_POS_S_INSIDE_BALL�ԗ񂩂�����Ă���̂ŁA���ꂾ�����炵���ʒu����R�s�[����
    GFL_STD_MemCopy( &(raw_data[BG_PAL_POS_S_INSIDE_BALL*16]), work->inside_ball_pal[i], 2*16*BG_PAL_NUM_S_INSIDE_BALL );
    GFL_HEAP_FreeMemory( buff );
  }
  GFL_ARC_CloseDataHandle( handle );

  // �p���b�g�A�j���̏��
  // none�ɂ���
  work->inside_ball_pal_anime_state = INSIDE_BALL_PAL_ANIME_STATE_END;
  work->inside_ball_pal_anime_count = 16;
  work->inside_ball_pal_anime_end   = INSIDE_BALL_NONE;

/*
  �Ȃ��Ă����v�̂悤��
  // �m���ɓǂݍ��݂��I���� 
  for( i=0; i<INSIDE_BALL_MAX; i++ )
  {
    DC_FlushRange( work->inside_ball_pal[i], 2*16*BG_PAL_NUM_S_INSIDE_BALL );
  }
*/

  // �������
  // none�ɂ���
  GFL_STD_MemCopy( work->inside_ball_pal[INSIDE_BALL_NONE], work->inside_ball_st_pal[INSIDE_BALL_PAL_TRANS], 2*16*BG_PAL_NUM_S_INSIDE_BALL );
  NNS_GfdRegisterNewVramTransferTask(
      NNS_GFD_DST_2D_BG_PLTT_SUB,
      BG_PAL_POS_S_INSIDE_BALL * 2*16,
      work->inside_ball_st_pal[INSIDE_BALL_PAL_TRANS],
      2*16*BG_PAL_NUM_S_INSIDE_BALL );
}
static void Psel_InsideBallPalExit( PSEL_WORK* work )
{
  // �������Ȃ�
}
static void Psel_InsideBallPalMain( PSEL_WORK* work )
{
  u8 j;
  BOOL need_trans = FALSE;
  
  // 0=start, 16=end
  
  if( work->inside_ball_pal_anime_state == INSIDE_BALL_PAL_ANIME_STATE_START_TO_END )
  {
    if( work->inside_ball_pal_anime_count == 16 )
    {
      work->inside_ball_pal_anime_state = INSIDE_BALL_PAL_ANIME_STATE_END;
    }
    else
    {
      work->inside_ball_pal_anime_count++;
      need_trans = TRUE;
    }

    if( need_trans )
    {
      for( j=0; j<16*BG_PAL_NUM_S_INSIDE_BALL; j++ )
      {
        u16 count = work->inside_ball_pal_anime_count;
        u16 color = work->inside_ball_st_pal[INSIDE_BALL_PAL_START][j];
        u16 dark  = work->inside_ball_pal[ work->inside_ball_pal_anime_end ][j];
        u16 color_r, color_g, color_b;
        u16 dark_r, dark_g, dark_b;
        u16 trans_r, trans_g, trans_b;
        U16ToRGB( color, &color_r, &color_g, &color_b );
        U16ToRGB( dark, &dark_r, &dark_g, &dark_b );
        trans_r = ( color_r*(16-count) + dark_r*(count) ) /16;
        trans_g = ( color_g*(16-count) + dark_g*(count) ) /16;
        trans_b = ( color_b*(16-count) + dark_b*(count) ) /16;
        work->inside_ball_st_pal[INSIDE_BALL_PAL_TRANS][j] = GX_RGB( trans_r, trans_g, trans_b );
      }
      NNS_GfdRegisterNewVramTransferTask(
        NNS_GFD_DST_2D_BG_PLTT_SUB,
        BG_PAL_POS_S_INSIDE_BALL * 2*16,
        work->inside_ball_st_pal[INSIDE_BALL_PAL_TRANS],
        2*16*BG_PAL_NUM_S_INSIDE_BALL );
    }
  }
}

static void Psel_PalInit( PSEL_WORK* work )
{
  Psel_PokeSetPalInit( work );
  Psel_InsideBallPalInit( work );
}
static void Psel_PalExit( PSEL_WORK* work )
{
  Psel_PokeSetPalExit( work );
  Psel_InsideBallPalExit( work );
}
static void Psel_PalMain( PSEL_WORK* work )
{
  Psel_PokeSetPalMain( work );
  Psel_InsideBallPalMain( work );
}

static void Psel_PokeSetPalAnimeStart( PSEL_WORK* work, TARGET target_poke, POKE_PAL_ANIME_STATE state )
{
  switch( state )
  {
  case POKE_PAL_ANIME_STATE_COLOR:
    {
      if( work->poke_set[target_poke].pal_anime_state != POKE_PAL_ANIME_STATE_COLOR )
      {
        work->poke_set[target_poke].pal_anime_state = POKE_PAL_ANIME_STATE_DARK_TO_COLOR;
      }
    }
    break;
  case POKE_PAL_ANIME_STATE_DARK:
    {
      if( work->poke_set[target_poke].pal_anime_state != POKE_PAL_ANIME_STATE_DARK )
      {
        work->poke_set[target_poke].pal_anime_state = POKE_PAL_ANIME_STATE_COLOR_TO_DARK;
      }
    }
    break;
  }
}

static void Psel_InsideBallPalAnimeStart( PSEL_WORK* work, u8 end )
{
  if( work->inside_ball_pal_anime_end != end )
  {
    work->inside_ball_pal_anime_state = INSIDE_BALL_PAL_ANIME_STATE_START_TO_END;
    work->inside_ball_pal_anime_count = 0;
    work->inside_ball_pal_anime_end = end;

    // ���݂̐F���X�^�[�g�F�Ƃ��Ċo���Ă��� 
    GFL_STD_MemCopy( work->inside_ball_st_pal[INSIDE_BALL_PAL_TRANS], work->inside_ball_st_pal[INSIDE_BALL_PAL_START], 2*16*BG_PAL_NUM_S_INSIDE_BALL );
  }
}


//-------------------------------------
/// �V�[�P���X
//=====================================
static int Psel_Start( PSEL_WORK* work, int* seq )
{
  return SEQ_LOAD;
}
static int Psel_Load( PSEL_WORK* work, int* seq )
{
  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    work->graphic       = PSEL_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, work->heap_id );
    work->font          = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, work->heap_id );
    work->print_que     = PRINTSYS_QUE_Create( work->heap_id );
  }

  // ���C��BG
  GFL_BG_SetPriority( BG_FRAME_M_THREE,    BG_FRAME_PRI_M_THREE );
  GFL_BG_SetPriority( BG_FRAME_M_REAR,     BG_FRAME_PRI_M_REAR );
  GFL_BG_SetPriority( BG_FRAME_M_WIN,      BG_FRAME_PRI_M_WIN );
  GFL_BG_SetPriority( BG_FRAME_M_TEXT,     BG_FRAME_PRI_M_TEXT );
  
  // �T�uBG
  GFL_BG_SetPriority( BG_FRAME_S_INSIDE_BALL,     BG_FRAME_PRI_S_INSIDE_BALL );
  GFL_BG_SetPriority( BG_FRAME_S_WIN,             BG_FRAME_PRI_S_WIN );
  GFL_BG_SetPriority( BG_FRAME_S_TEXT,            BG_FRAME_PRI_S_TEXT );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Psel_VBlankFunc, work, 1 );
 
  // 3D�S��
  Psel_ThreeInit( work );
  // BG
  Psel_BgInit( work ); 
  // �e�L�X�g
  Psel_TextInit( work );
  // APP_TASKMENU
  Psel_AppTaskmenuResInit( work );

  return SEQ_S01_INIT;
}

static int Psel_S01Init    ( PSEL_WORK* work, int* seq )
{
  // �o�b�N�O���E���h�J���[
  GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0x2f3b );
  GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0x2f3b );

  Psel_ThreeS01Init( work );
  Psel_BgS01Init( work );
  Psel_TextS01Init( work );

  // �u�����h
  G2_SetBlendAlpha(
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2/* | GX_BLEND_PLANEMASK_BG3*/,
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
      8, 8 );
  // 3D�̃A���t�@�����ꂢ�ɏo�����߁A��1�Ώۖʂ�BG0���w�肵�Ă���
  // ev1��ev2�͎g���Ȃ�  // TWL�v���O���~���O�}�j���A���u2D�ʂƂ̃��u�����f�B���O�v�Q�l

#if 0
  G2S_SetBlendAlpha(
      GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BG1/* | GX_BLEND_PLANEMASK_BG2*/,
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
      8, 8 );
#endif
  // OBJ�𔼓����ɂ���̂͂�߂āA�p���b�g���Â����邱�Ƃɂ����B
  G2S_SetBlendAlpha(
      GX_BLEND_PLANEMASK_BG1/* | GX_BLEND_PLANEMASK_BG2*/,
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
      8, 8 );

  return SEQ_S01_FADE_IN;
}
static int Psel_S01FadeIn  ( PSEL_WORK* work, int* seq )
{
  Psel_ThreeS01Main( work );
  Psel_BgS01Main( work );
  Psel_TextS01Main( work );

  switch(*seq)
  {
  case 0:
    {
      // �t�F�[�h�C��(����������)
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, S01_FADE_IN_WAIT );
      (*seq)++;
    }
    break;
  case 1:
    {
      if( !GFL_FADE_CheckFade() )
      {
        return SEQ_S01_MAIN;
      }
    }
    break;
  }
  return SEQ_S01_FADE_IN;
}
static int Psel_S01Main    ( PSEL_WORK* work, int* seq )
{
  Psel_ThreeS01Main( work );
  Psel_BgS01Main( work );
  Psel_TextS01Main( work );

  if(    ( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) )
      || ( GFL_UI_TP_GetTrg() ) )
  {
    if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ) work->ktst = GFL_APP_KTST_KEY;
    else                                                    work->ktst = GFL_APP_KTST_TOUCH;

    return SEQ_S01_FADE_OUT;
  }
  return SEQ_S01_MAIN;
}
static int Psel_S01FadeOut ( PSEL_WORK* work, int* seq )
{
  Psel_ThreeS01Main( work );
  Psel_BgS01Main( work );
  Psel_TextS01Main( work );

  switch(*seq)
  {
  case 0:
    {
      // �t�F�[�h�A�E�g(�����遨��)
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, S01_FADE_OUT_WAIT );
      (*seq)++;
    }
    break;
  case 1:
    {
      if( !GFL_FADE_CheckFade() )
      {
        return SEQ_S01_EXIT;
      }
    }
    break;
  }
  return SEQ_S01_FADE_OUT;
}
static int Psel_S01Exit    ( PSEL_WORK* work, int* seq )
{
  // �u�����h
  G2_BlendNone();
  G2S_BlendNone();

  Psel_TextS01Exit( work );
  Psel_BgS01Exit( work );
  Psel_ThreeS01Exit( work );
  return SEQ_S02_INIT;
}

static int Psel_S02Init    ( PSEL_WORK* work, int* seq )
{
  // ��ʂ̏㉺����ւ�
  GFL_DISP_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );

  // �o�b�N�O���E���h�J���[
  GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0x2f3b );
  GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_S, 0x2f3b );
 
  Psel_ThreeS02Init( work );
  Psel_PokeSetInit( work );
  Psel_FingerInit( work );
  Psel_BgS02Init( work );
  Psel_InsideBallInit( work );
  Psel_TextS02Init( work );
  Psel_AppTaskmenuWinInit( work );
  Psel_AppTaskmenuInit( work );
  Psel_PalInit( work );

  // �u�����h
  G2_SetBlendAlpha(
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2/* | GX_BLEND_PLANEMASK_BG3*/,
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
      8, 8 );
  // 3D�̃A���t�@�����ꂢ�ɏo�����߁A��1�Ώۖʂ�BG0���w�肵�Ă���
  // ev1��ev2�͎g���Ȃ�  // TWL�v���O���~���O�}�j���A���u2D�ʂƂ̃��u�����f�B���O�v�Q�l

#if 0
  G2S_SetBlendAlpha(
      GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BG1/* | GX_BLEND_PLANEMASK_BG2*/,
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
      8, 8 );
#endif
  // OBJ�𔼓����ɂ���̂͂�߂āA�p���b�g���Â����邱�Ƃɂ����B
  G2S_SetBlendAlpha(
      GX_BLEND_PLANEMASK_BG1/* | GX_BLEND_PLANEMASK_BG2*/,
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
      8, 8 );
  
  return SEQ_S02_FADE_IN;
}
static int Psel_S02FadeIn  ( PSEL_WORK* work, int* seq )
{
  Psel_ThreeS02Main( work );
  Psel_BgS02Main( work );
  Psel_TextS02Main( work );
  Psel_AppTaskmenuWinMain( work );
  Psel_AppTaskmenuMain( work );
  Psel_PalMain( work );

  switch(*seq)
  {
  case 0:
    {
      // �t�F�[�h�C��(����������)
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, S02_FADE_IN_WAIT );
      (*seq)++;
    }
    break;
  case 1:
    {
      if( !GFL_FADE_CheckFade() )
      {
        return SEQ_S02_MAIN;
      }
    }
    break;
  }
  return SEQ_S02_FADE_IN;
}
static int Psel_S02Main    ( PSEL_WORK* work, int* seq )
{
  int hit = GFL_UI_TP_HIT_NONE;

  Psel_ThreeS02Main( work );
  Psel_BgS02Main( work );
  Psel_TextS02Main( work );
  Psel_AppTaskmenuWinMain( work );
  Psel_AppTaskmenuMain( work );
  Psel_PalMain( work );

  // �^�b�`
  hit = GFL_UI_TP_HitTrg(target_tp_hittbl);
  switch(hit)
  {
  case TARGET_MIZU:
    {
      PMSND_PlaySystemSE( PSELSND_DECIDE );
    }
    break;
  case TARGET_HONOO:
    {
      PMSND_PlaySystemSE( PSELSND_CANCEL );
    }
    break;
  case TARGET_KUSA:
    {
      PMSND_PlaySystemSE( PSELSND_MOVE );
    }
    break;
  case TARGET_BUTTON:
    {
      PMSND_PlaySystemSE( PSELSND_OPEN );
    }
    break;
  }






  if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )  Psel_InsideBallPalAnimeStart( work, INSIDE_BALL_MIZU );
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )    Psel_InsideBallPalAnimeStart( work, INSIDE_BALL_HONOO );
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT ) Psel_InsideBallPalAnimeStart( work, INSIDE_BALL_KUSA );
  if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )  Psel_InsideBallPalAnimeStart( work, INSIDE_BALL_NONE );

  if( ( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
      && ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ) )  Psel_PokeSetPalAnimeStart( work, TARGET_MIZU, POKE_PAL_ANIME_STATE_COLOR );
  if( ( GFL_UI_KEY_GetCont() & PAD_KEY_UP )
      && ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ) )  Psel_PokeSetPalAnimeStart( work, TARGET_HONOO, POKE_PAL_ANIME_STATE_COLOR );
  if( ( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
      && ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ) )  Psel_PokeSetPalAnimeStart( work, TARGET_KUSA, POKE_PAL_ANIME_STATE_COLOR );

  if( ( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
      && ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ) )  Psel_PokeSetPalAnimeStart( work, TARGET_MIZU, POKE_PAL_ANIME_STATE_DARK );
  if( ( GFL_UI_KEY_GetCont() & PAD_KEY_UP )
      && ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ) )  Psel_PokeSetPalAnimeStart( work, TARGET_HONOO, POKE_PAL_ANIME_STATE_DARK );
  if( ( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
      && ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ) )  Psel_PokeSetPalAnimeStart( work, TARGET_KUSA, POKE_PAL_ANIME_STATE_DARK );







  if(    ( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) )
      /*|| ( GFL_UI_TP_GetTrg() )*/ )
  {
    if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ) work->ktst = GFL_APP_KTST_KEY;
    else                                                    work->ktst = GFL_APP_KTST_TOUCH;

    return SEQ_S02_FADE_OUT;
  }
  return SEQ_S02_MAIN;
}
static int Psel_S02FadeOut ( PSEL_WORK* work, int* seq )
{
  Psel_ThreeS02Main( work );
  Psel_BgS02Main( work );
  Psel_TextS02Main( work );
  Psel_AppTaskmenuWinMain( work );
  Psel_AppTaskmenuMain( work );
  Psel_PalMain( work );

  switch(*seq)
  {
  case 0:
    {
      // �t�F�[�h�A�E�g(�����遨��)
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, S02_FADE_OUT_WAIT );
      (*seq)++;
    }
    break;
  case 1:
    {
      if( !GFL_FADE_CheckFade() )
      {
        return SEQ_S02_EXIT;
      }
    }
    break;
  }
  return SEQ_S02_FADE_OUT;
}
static int Psel_S02Exit    ( PSEL_WORK* work, int* seq )
{
  // �u�����h
  G2_BlendNone();
  G2S_BlendNone();

  Psel_PalExit( work );
  Psel_AppTaskmenuExit( work );
  Psel_AppTaskmenuWinExit( work );
  Psel_TextS02Exit( work );
  Psel_InsideBallExit( work );
  Psel_BgS02Exit( work );
  Psel_FingerExit( work );
  Psel_PokeSetExit( work );
  Psel_ThreeS02Exit( work );
  
  return SEQ_UNLOAD;
}

static int Psel_Unload( PSEL_WORK* work, int* seq )
{
  // APP_TASKMENU
  Psel_AppTaskmenuResExit( work );
  // �e�L�X�g
  Psel_TextExit( work ); 
  // BG
  Psel_BgExit( work ); 
  // 3D�S��
  Psel_ThreeExit( work );

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  {
    PRINTSYS_QUE_Delete( work->print_que );
    GFL_FONT_Delete( work->font );
    PSEL_GRAPHIC_Exit( work->graphic );
  }

  return SEQ_END;
}
static int Psel_End( PSEL_WORK* work, int* seq )
{
  return SEQ_END;
}

