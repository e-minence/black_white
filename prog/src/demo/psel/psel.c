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
//#define DEBUG_POS_SET_MODE_MAIN  // ���ꂪ��`����Ă���Ƃ��A�ʒu�������[�h�ɂȂ�
//#define DEBUG_POS_SET_MODE_SUB   // ���ꂪ��`����Ă���Ƃ��A�ʒu�������[�h�ɂȂ�


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
#include "sound/pm_wb_voice.h"  // wb�ł�pm_voice�ł͂Ȃ���������g��

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
#define BG_FRAME_S_INSIDE_BALL (GFL_BG_FRAME0_S)  // BG_FRAME_S_INSIDE_BALL��BG_FRAME_S_SPOT�͋��ʂ̃L�����̈���g�p
#define BG_FRAME_S_SPOT        (GFL_BG_FRAME1_S)  // BG_FRAME_S_SPOT�̕\����\����BG_FRAME_S_WIN��BG_FRAME_S_TEXT�ɍ��킹��
#define BG_FRAME_S_WIN         (GFL_BG_FRAME2_S)  // BG_FRAME_S_WIN��BG_FRAME_S_TEXT�͋��ʂ̃L�����̈���g�p���悤���Ǝv�������A�ʁX�ɂ���
#define BG_FRAME_S_TEXT        (GFL_BG_FRAME3_S)  // (S_WIN�͑O����AS_TEXT�͌�납������肾����)

// �T�uBG�t���[���̃v���C�I���e�B
#define BG_FRAME_PRI_S_INSIDE_BALL    (3)
#define BG_FRAME_PRI_S_SPOT           (2)
#define BG_FRAME_PRI_S_WIN            (1)
#define BG_FRAME_PRI_S_TEXT           (0)

// �T�uBG�p���b�g
// �{��
enum
{
  BG_PAL_NUM_S_WIN                    =  1,
  BG_PAL_NUM_S_TEXT                   =  1,
  BG_PAL_NUM_S_INSIDE_BALL            =  8,  // BG_FRAME_S_INSIDE_BALL��BG_FRAME_S_SPOT�͋��ʂ̃p���b�g�̈���g�p
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
  OBJ_PAL_NUM_S_KUSA      = 1,  // small��big�����̃p���b�g����g��
  OBJ_PAL_NUM_S_HONOO     = 1,
  OBJ_PAL_NUM_S_MIZU      = 1,
};
// �ʒu
enum
{
  OBJ_PAL_POS_S_KUSA      = 0,
  OBJ_PAL_POS_S_HONOO     = 1,
  OBJ_PAL_POS_S_MIZU      = 2,
};


// �T�E���h
#define PSELSND_DECIDE  ( SEQ_SE_DECIDE1 )
#define PSELSND_CANCEL  ( SEQ_SE_CANCEL1 )
#define PSELSND_MOVE    ( SEQ_SE_SELECT1 )

#define PSELSND_DEMO_RIBBON_LOOSE  ( SEQ_SE_SDEMO_01 )
#define PSELSND_DEMO_BOX_OPEN      ( SEQ_SE_SDEMO_02 )
#define PSELSND_DEMO_ZOOM_UP       ( SEQ_SE_SDEMO_03 )


// �t���[�����킹
// 3D��30�t���[���ł����Ă���A����60�t���[���ł����Ă���

// �C���g
//#define THREE_INTERRUPT (1)  // 60fps
#define THREE_INTERRUPT (2)  // 30fps

// 3D�̃A�j����1�t���[���łǂꂾ���i�߂邩
//#define THREE_ADD (FX32_ONE)  // 60fps
#define THREE_ADD (FX32_HALF)  // 30fps

// ���̃A�v����60�t���[���œ������̂ŁA3D�𔼕��̑����ōĐ�����悤�ɂ���

// �t���[���ݒ�֐���1�킹�Ă���
static void Psel_Three_ICA_ANIME_SetAnimeFrame( ICA_ANIME* anime, fx32 frame );
static void Psel_Three_ICA_ANIME_SetAnimeFrame( ICA_ANIME* anime, fx32 frame )
{
  ICA_ANIME_SetAnimeFrame( anime, frame/THREE_INTERRUPT );
}




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
  //{  BG_FRAME_M_TEXT,    4,    4,   24,   16, BG_PAL_POS_M_TEXT, GFL_BMP_CHRAREA_GET_B, 0, 0 },  // �㉺1�L�������]�T������
  {  BG_FRAME_M_TEXT,    4,    5,   24,   14, BG_PAL_POS_M_TEXT, GFL_BMP_CHRAREA_GET_B, 0, 0 },  // �㉺�ɗ]�T���Ȃ�
  {  BG_FRAME_M_TEXT,    1,   19,   30,    2, BG_PAL_POS_M_TEXT, GFL_BMP_CHRAREA_GET_B, 0, 0 },
  {  BG_FRAME_S_TEXT,    1,   18,   30,    4, BG_PAL_POS_S_TEXT, GFL_BMP_CHRAREA_GET_B, 0, 0 },
};

// ������
#define TEXT_POKE_INFO_LEN_MAX    (64)  // �|�P�����̃^�C�v�Ǝ푰��(�s�J�`���E�Ƃ�)�̏��  // EOM���܂߂�������


// �t�F�[�h
#define S01_FADE_IN_WAIT  (0)
#define S01_FADE_OUT_WAIT (3)//(1*THREE_INTERRUPT)  // 60�t���[���Ő^�����ɂ������̂ŁA�Ȃ�ׂ�����ɋ߂��Ȃ�悤�ɁA�����������ʂ�*THREE_INTERRUPT���Ȃ�
#define S02_FADE_IN_WAIT  (2)
#define S02_FADE_OUT_WAIT (1*THREE_INTERRUPT)

#define FADE_SPEED_INSIDE_DEMO   (1)     // ���̃f���ł̃t�F�[�h���x�W��


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

#define OBJ_BG_PRI_S_POKE          (BG_FRAME_PRI_S_INSIDE_BALL)
#define OBJ_BG_PRI_S_POKE_SPOT     (BG_FRAME_PRI_S_SPOT)
#define OBJ_BG_PRI_M_FINGER        (BG_FRAME_PRI_M_THREE)


// �I��Ώ�
typedef enum
{
  TARGET_KUSA       = PSEL_RESULT_KUSA,   // ��     // �����珇�Ԃɕ��ׂĂ���  // PSEL_RESULT�Ɠ������я��ɂ��Ă�������  // psel.c����PSEL_RESULT�̕��т𗘗p����̂͂��������ŁA���͂���TARGET�̕��т𗘗p����悤�ɂ��Ă���(����ŁAPSEL_RESULT��ύX������𓾂Ȃ��Ƃ��ɍŏ����̕ύX�ōςނ���)
  TARGET_HONOO      = PSEL_RESULT_HONOO,  // ����
  TARGET_MIZU       = PSEL_RESULT_MIZU,   // �E
  TARGET_POKE_MAX,                        // �|�P������
  TARGET_BUTTON     = TARGET_POKE_MAX,    // �m�F�{�^��
  TARGET_MAX,
  TARGET_NONE       = TARGET_MAX,         // �ǂ���I�����Ă��Ȃ��Ƃ�
  TARGET_POKE_START = TARGET_KUSA,        // TARGET_POKE_START<= <=TARGET_POKE_END
  TARGET_POKE_END   = TARGET_MIZU,
  TARGET_POKE_DEFAULT   = TARGET_HONOO,  // �L�[����̂Ƃ��ɍŏ��ɑI�΂��f�t�H���g
}
TARGET;

// �^�b�`�̓����蔻��
static const GFL_UI_TP_HITTBL target_tp_hittbl[TARGET_MAX +1] =
{
  { GFL_UI_TP_USE_CIRCLE,   66,   96,   32 },  // circle
  { GFL_UI_TP_USE_CIRCLE,  128,   66,   26 },  // circle
  { GFL_UI_TP_USE_CIRCLE,  189,   96,   32 },  // circle
  { 21*8, 24*8 -1, 9*8, 23*8 -1 },  // rect
  { GFL_UI_TP_HIT_END, 0, 0, 0 },  // �e�[�u���I��
};

// �w�w���J�[�\���̈ʒu
static const u8 finger_pos[TARGET_POKE_MAX][2] =
{
  {  66,  39 },
  { 128,  13 },
  { 189,  39 },
};

// �|�P�����召�Z�b�g
// monsno
static const u16 poke_monsno[TARGET_POKE_MAX] =
{
  MONSNO_551,  // MONSNO_???��???�̓O���t�B�b�N�i���o�[�A��`����Ă���l��monsno
  MONSNO_554,
  MONSNO_557,
};
// gmm
static const u32 poke_str_id[TARGET_POKE_MAX] =
{
  msg_t01r0102_ball_grass_01,
  msg_t01r0102_ball_fire_01,
  msg_t01r0102_ball_water_01,
};

// ���\�[�X
static const u8 poke_res_big[TARGET_POKE_MAX][TWO_OBJ_RES_FILE_MAX] =
{
  { NARC_psel_psel_551_m_NCGR, NARC_psel_psel_551_n_NCLR, NARC_psel_psel_551_m_NCER, NARC_psel_psel_551_m_NANR, NARC_psel_psel_551_x_NCLR },
  { NARC_psel_psel_554_m_NCGR, NARC_psel_psel_554_n_NCLR, NARC_psel_psel_554_m_NCER, NARC_psel_psel_554_m_NANR, NARC_psel_psel_554_x_NCLR },
  { NARC_psel_psel_557_m_NCGR, NARC_psel_psel_557_n_NCLR, NARC_psel_psel_557_m_NCER, NARC_psel_psel_557_m_NANR, NARC_psel_psel_557_x_NCLR },
};
// �p���b�g
static const u8 poke_obj_pal_pos_s[TARGET_POKE_MAX] =
{
  OBJ_PAL_POS_S_KUSA,
  OBJ_PAL_POS_S_HONOO,
  OBJ_PAL_POS_S_MIZU, 
};

// �|�P�����̓���
typedef struct
{
  u8 p0_x;             // p0�̈ʒu
  u8 p0_y;             // p0�͈�ԉ��̊J�n�ꏊ
  u8 p1_x;             // p1�̈ʒu
  u8 p1_y;             // p1�̓W�����v�Œ��n�������Ԓn�_
  u8 p2_x;             // p2�̈ʒu
  u8 p2_y;             // p2�͈�Ԏ�O�̒���
  u8 p0_scale;         // p0�ɂ�����big�̃X�P�[��(p0_scale/16�Ƃ��Ďg��)
  u8 p1_scale;         // p1�ɂ�����big�̃X�P�[��(p0_scale/16�Ƃ��Ďg��)
  u8 p2_scale;         // p2�ɂ�����big�̃X�P�[��(p0_scale/16�Ƃ��Ďg��)
  u8 p1_frame;         // p1�ő҂t���[����
  u8 p0p1_frame;       // p0p1�Ԃ̈ړ��ɂ�����t���[����
  u8 p1p2_frame;       // p1p2�Ԃ̈ړ��ɂ�����t���[����
  u8 p0p1_jump;        // p0p1�Ԃ̍ő�̃W�����v�̍���
  u8 p1p2_jump;        // p1p2�Ԃ̍ő�̃W�����v�̍���
}
POKE_MOVE_DATA;

#if 0
  __ ...p0p1_jump
 /  \
+    \   __ ...p1p2_jump
p0    \ /  \
       +    \
       p1    \
              +
              p2
#endif

static const POKE_MOVE_DATA poke_move_data[TARGET_POKE_MAX] =
{
  {
     53,  73,  90,  83, 128,  94,
     8, 12, 16,
     8,//16,
     4,  4,
    32, 32,
  },
  {
    128,  68, 128,  81, 128,  94,
     8, 12, 16,
     8,//16,
     4,  4,
    32, 32,
  },
  {
    202,  73, 165,  83, 128,  94,
     8, 12, 16,
     8,//16,
     4,  4,
    32, 32,
  },
};

typedef enum
{
  POKE_MOVE_STEP_P0_STOP,   // 1�t���[��
  POKE_MOVE_STEP_P0_TO_P1,  // p0p1_frame
  POKE_MOVE_STEP_P1_WAIT,   // p1_frame
  POKE_MOVE_STEP_P1_TO_P2,  // p1p2_frame
  POKE_MOVE_STEP_P2_STOP,   // 1�t���[��
}
POKE_MOVE_STEP;

typedef enum
{
  POKE_MOVE_REQ_NONE,        // �v���Ȃ�
  POKE_MOVE_REQ_P0_TO_P2,    // �������O�ֈړ�����悤�v��
  POKE_MOVE_REQ_P2_TO_P0,    // ��O���牜�ֈړ�����悤�v��
}
POKE_MOVE_REQ;

// �`��D��x
#define POKE_SOFTPRI_BACK   (2)  // ��
#define POKE_SOFTPRI_MIDDLE (1)  // ��
#define POKE_SOFTPRI_FRONT  (0)  // ��O

// �A���t�@�A�j���̏��
typedef enum
{
  POKE_SET_ALPHA_ANIME_STATE_NONE,
  POKE_SET_ALPHA_ANIME_STATE_0,
  POKE_SET_ALPHA_ANIME_STATE_ANIME,
  POKE_SET_ALPHA_ANIME_STATE_16,
}
POKE_SET_ALPHA_ANIME_STATE;


// �{�[�������C���[�W�����T�u���BG
enum
{
  INSIDE_BALL_KUSA    = TARGET_KUSA,    // TARGET�Ɠ������я��ɂ��Ă���
  INSIDE_BALL_HONOO   = TARGET_HONOO,
  INSIDE_BALL_MIZU    = TARGET_MIZU,
  INSIDE_BALL_NONE,
  INSIDE_BALL_MAX,
};

// �{�[�������C���[�W�����T�u���BG�̃p���b�g�f�[�^
static const u8 inside_ball_res[INSIDE_BALL_MAX] =
{
  NARC_psel_psel_bg02_l_NCLR, 
  NARC_psel_psel_bg02_f_NCLR, 
  NARC_psel_psel_bg02_w_NCLR, 
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
  { ARCID_PSEL,        NARC_psel_psel_mb_nsbca,           GFL_G3D_UTIL_RESARC },
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
  { 4, 0 },
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
  MB_L     = TARGET_KUSA,  // TARGET�Ɠ������я��ɂ��Ă���
  MB_C     = TARGET_HONOO,
  MB_R     = TARGET_MIZU,
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
#define THREE_RES_MAX              (5)   // s02��mb�𓯎��ɓǂݍ��񂾂Ƃ��̑���
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
  THREE_USER_OBJ_IDX_MB_L,  // MB_(MB_MAX)�Ɠ������я��ɂ��Ă���
  THREE_USER_OBJ_IDX_MB_C,
  THREE_USER_OBJ_IDX_MB_R,
  THREE_USER_OBJ_IDX_MAX,
};


// 3D�t���[����
// �f�U�C�i�[�����3D�I�[�T�����O�c�[����ł�1�t���[������X�^�[�g���Ă���悤�����A�v���O�����ł͊J�n�t���[����0�Ƃ��ď�������
#define TIMETABLE_S01_CAMERA_MOVE      (  0*THREE_INTERRUPT)
#define TIMETABLE_S01_CARD_COME        ( 40*THREE_INTERRUPT)
#define TIMETABLE_S01_CARD_READ        ( 69*THREE_INTERRUPT)  // ���̃t���[���Ŏ~�߂Ă���
#define TIMETABLE_S01_CARD_VANISH      ( 70*THREE_INTERRUPT)
#define TIMETABLE_S01_RIBBON_LOOSE     ( 90*THREE_INTERRUPT)
#define TIMETABLE_S01_WO_START         (100*THREE_INTERRUPT)  // �z���C�g�A�E�g�̊J�n�t���[��  // 100-129
#define TIMETABLE_S01_SCENE_END        (129*THREE_INTERRUPT)  // ���̃t���[���Ŏ~�߂Ă���

#define TIMETABLE_S02_BOX_OPEN         (  0*THREE_INTERRUPT)
#define TIMETABLE_S02_BOX_STOP         ( 39*THREE_INTERRUPT)  // ���̃t���[���Ŏ~�߂Ă���

enum
{
  TIMETABLE_MB_ZOOM_START,
  TIMETABLE_MB_ZOOM_END,        // ���̃t���[���Ŏ~�߂Ă���
  TIMETABLE_MB_DECIDE_START,
  TIMETABLE_MB_DECIDE_END,      // ���̃t���[���Ŏ~�߂Ă���
  TIMETABLE_MB_WO_START,        // �z���C�g�A�E�g�̊J�n�t���[��  // 100-129, 160-189, 220-249
  TIMETABLE_MB_MAX,
};

static const u16 timetable_mb[MB_MAX][TIMETABLE_MB_MAX] =
{
  {  40*THREE_INTERRUPT,  49*THREE_INTERRUPT,  70*THREE_INTERRUPT, 129*THREE_INTERRUPT, 100*THREE_INTERRUPT },
  {  50*THREE_INTERRUPT,  59*THREE_INTERRUPT, 130*THREE_INTERRUPT, 189*THREE_INTERRUPT, 160*THREE_INTERRUPT },
  {  60*THREE_INTERRUPT,  69*THREE_INTERRUPT, 190*THREE_INTERRUPT, 249*THREE_INTERRUPT, 220*THREE_INTERRUPT },
};

// VBlank���̃��N�G�X�g
enum
{
  VBLANK_REQ_NONE                         = 0,
  VBLANK_REQ_S01_TEXT_M_VISIBLE_ON        = 1<< 0,
  VBLANK_REQ_S01_TEXT_M_VISIBLE_OFF       = 1<< 1,
  VBLANK_REQ_S02_TEXT_WIN_M_VISIBLE_ON    = 1<< 2,
  VBLANK_REQ_S02_TEXT_WIN_M_VISIBLE_OFF   = 1<< 3,
  VBLANK_REQ_S02_TEXT_WIN_S_VISIBLE_ON    = 1<< 4,
  VBLANK_REQ_S02_TEXT_WIN_S_VISIBLE_OFF   = 1<< 5,
  VBLANK_REQ_S02_TEXT_M_VISIBLE_ON        = 1<< 6,
  VBLANK_REQ_S02_TEXT_M_VISIBLE_OFF       = 1<< 7,
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

  // ����
  u16              move_step_count;   // POKE_MOVE_STEP�e�i�K�ɂ�����J�E���g
  POKE_MOVE_STEP   move_step;
  POKE_MOVE_REQ    move_req;
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
  // �V�[�P���X�t���[��
  u32                         sub_seq_frame;  // sub_seq���؂�ւ������0����X�^�[�g������sub_seq�̃t���[�������J�E���g����Ȃǂ̎g���������Ă����A���A�g��Ȃ��Ă������B�g���ꍇ�͎g���ꏊ�Ŏ����ŏ��������ĉ������B

  // �O���t�B�b�N�A�t�H���g�Ȃ�
  PSEL_GRAPHIC_WORK*          graphic;
  GFL_FONT*                   font;
  PRINT_QUE*                  print_que;

  // VBlank��TCB
  GFL_TCB*                    vblank_tcb;
  u32                         vblank_req;

  // 3D�S��
  GFL_G3D_UTIL*               three_util;
  u16                         three_unit_idx[THREE_SETUP_IDX_MAX];             // GFL_G3D_UTIL������U��ԍ�        // �Y������THREE_SETUP_IDX_(THREE_SETUP_IDX_MAX)
  u16                         three_obj_prop_tbl_idx[THREE_USER_OBJ_IDX_MAX];  // three_obj_prop_tbl�̃C���f�b�N�X  // �Y������THREE_USER_OBJ_IDX_(THREE_USER_OBJ_IDX_MAX)
  u16                         three_obj_prop_num;
  THREE_OBJ_PROPERTY*         three_obj_prop_tbl;
  ICA_ANIME*                  three_ica_anime;

  // �����X�^�[�{�[���ɑ΂��āA�Y�[�������茈�肵���肷��A�j��
  TARGET                      three_mb_anime_target;
  BOOL                        three_mb_anime_reverse;
  u32                         three_mb_anime_frame;
  
  // �t���[��
  u32                         three_frame;  // �V�[�����؂�ւ������0����X�^�[�g�������A3D�̃t���[�����ƈ�v����悤�ɂ��Ă���

  // 2D OBJ
  // �|�P�����召�Z�b�g
  POKE_SET                    poke_set[TARGET_POKE_MAX];
  int                         poke_set_ev1;
  POKE_SET_ALPHA_ANIME_STATE  poke_set_alpha_anime_state; 
  // �w�w���J�[�\��
  u32                         finger_res[TWO_OBJ_RES_MAX];
  GFL_CLWK*                   finger_clwk;
  
  // �I��ł���|�P����
  TARGET                      select_target_poke;  // TARGET_POKE_START<= <=TARGET_POKE_END�����TARGET_NONE�̂ǂꂩ
  // �`�J�`�J�I���A�j�������Ă��郂���X�^�[�{�[��
  TARGET                      only_mb_select_anime_target;  // TARGET_POKE_START<= <=TARGET_POKE_END�����TARGET_NONE�̂ǂꂩ
                                  // �I��ł���̂�select_target_poke�����A
                                  // �I��ł���|�P�������ς�����Ƃ��ɁA
                                  // ����܂Ń`�J�`�J�I���A�j�������Ă��郂���X�^�[�{�[���̃A�j�����~�߂Ȃ���΂Ȃ�Ȃ��̂ŁA
                                  // �����X�^�[�{�[����p�ɂǂ���A�j�������Ă��邩�o���Ă����ϐ���p�ӂ��Ă����B

  // POKE_INFO�e�L�X�g�������t���O
  BOOL                        poke_info_print;  // ���ꂪTRUE�̂Ƃ��A�|�P�����̃^�C�v�Ǝ푰�����T�u��ʂɏ���(TRUE�ɂȂ����t���[���ŏ�������������͏����Ȃ�������FALSE�ɂ���̂ŁATRUE�ɂȂ��Ă���̂�1�t���[������)

  // �e�L�X�g
  GFL_MSGDATA*                msgdata;
  GFL_BMPWIN*                 text_bmpwin[TEXT_MAX];
  BOOL                        text_finish[TEXT_MAX];  // bmpwin�̓]�����ς�ł����TRUE
  PRINT_STREAM*               print_stream;
  GFL_TCBLSYS*                ps_tcblsys;
  STRBUF*                     ps_strbuf;

  // APP_TASKMENU
  APP_TASKMENU_RES*           app_taskmenu_res;
  APP_TASKMENU_WIN_WORK*      app_taskmenu_win_wk;  // �^�b�`�̂Ƃ��ɏo��u����Ɍ��߂��v�{�^��
  APP_TASKMENU_WORK*          app_taskmenu_wk;      // �^�b�`�ł��L�[�ł��o��ŏI�m�F�́u�͂��v�u�������v�{�^��

  // �u�����h
  u8                          ev1;  // G2_SetBlendAlpha��ev1  // 0<=ev1<=16  // ev2=16-ev1

  // �{�[�������C���[�W�����T�u���BG�̃p���b�g
  // �I���W�i���p���b�g�f�[�^
  u16                         inside_ball_pal[INSIDE_BALL_MAX][16*BG_PAL_NUM_S_INSIDE_BALL];
  u16                         inside_ball_st_pal[INSIDE_BALL_PAL_MAX][16*BG_PAL_NUM_S_INSIDE_BALL];
  // �p���b�g�A�j���̏��
  INSIDE_BALL_PAL_ANIME_STATE inside_ball_pal_anime_state;
  u8                          inside_ball_pal_anime_count;
  u8                          inside_ball_pal_anime_end;

  // �t�F�[�h���x�W��
  int                         fade_speed_outside_demo;  // ���̃f���ɓ���O�̃t�F�[�h���x�W�����o���Ă���
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

static void Psel_ThreeS02MbZoomAnimeStart( PSEL_WORK* work, TARGET target, BOOL reverse );
static BOOL Psel_ThreeS02MbZoomAnimeIsEnd( PSEL_WORK* work );
static void Psel_ThreeS02MbZoomAnimeMain( PSEL_WORK* work );
static void Psel_ThreeS02MbDecideAnimeStart( PSEL_WORK* work, TARGET target );
static void Psel_ThreeS02MbDecideAnimeMain( PSEL_WORK* work );

static void Psel_ThreeS02OnlyMbSelectAnimeStart( PSEL_WORK* work, TARGET target );
static void Psel_ThreeS02OnlyMbSelectAnimeMain( PSEL_WORK* work );  // ���t���[���Ă�ł���

// �|�P�����召�Z�b�g
static void Psel_PokeSetInit( PSEL_WORK* work );
static void Psel_PokeSetExit( PSEL_WORK* work );
static void Psel_PokeSetMain( PSEL_WORK* work );
static void Psel_PokeSetSelectStart( PSEL_WORK* work, TARGET target_poke );

// �|�P�����召�Z�b�g�A���t�@�A�j��
static void Psel_PokeSetAlphaAnimeStart( PSEL_WORK* work );
static BOOL Psel_PokeSetIsAlphaAnimeEnd( PSEL_WORK* work );
static void Psel_PokeSetAlphaAnimeMain( PSEL_WORK* work );

// �w�w���J�[�\��
static void Psel_FingerInit( PSEL_WORK* work );
static void Psel_FingerExit( PSEL_WORK* work );
static void Psel_FingerDrawEnable( PSEL_WORK* work, BOOL on_off );
static void Psel_FingerUpdatePos( PSEL_WORK* work );

// BG�S��
static void Psel_BgInit( PSEL_WORK* work );
static void Psel_BgExit( PSEL_WORK* work );

// BG��
static void Psel_BgS01Init( PSEL_WORK* work );
static void Psel_BgS01Exit( PSEL_WORK* work );

static void Psel_BgS01SubInit( PSEL_WORK* work );
static void Psel_BgS01SubExit( PSEL_WORK* work );

static void Psel_BgS02Init( PSEL_WORK* work );
static void Psel_BgS02Exit( PSEL_WORK* work );

static void Psel_InsideBallInit( PSEL_WORK* work );
static void Psel_InsideBallExit( PSEL_WORK* work );

// �e�L�X�g�S��
static void Psel_TextInit( PSEL_WORK* work );
static void Psel_TextExit( PSEL_WORK* work );
static void Psel_TextMain( PSEL_WORK* work );
static BOOL Psel_TextTransWait( PSEL_WORK* work );  // TRUE���Ԃ��ė�����]���҂��Ȃ�
static void Psel_TextExplainStreamStart( PSEL_WORK* work, u32 str_id );
static BOOL Psel_TextExplainStreamWait( PSEL_WORK* work );  // TRUE���Ԃ��ė�����I��
static void Psel_TextExplainPrintStart( PSEL_WORK* work, u32 str_id );
static void Psel_TextExplainClear( PSEL_WORK* work );
static void Psel_TextPokeInfoPrintStart( PSEL_WORK* work, TARGET target );
static void Psel_TextPokeInfoClear( PSEL_WORK* work );

// �e�L�X�g��
static void Psel_TextS01Init( PSEL_WORK* work );
static void Psel_TextS01Exit( PSEL_WORK* work );

// APP_TASKMENU
static void Psel_AppTaskmenuResInit( PSEL_WORK* work );
static void Psel_AppTaskmenuResExit( PSEL_WORK* work );

static void Psel_AppTaskmenuWinInit( PSEL_WORK* work );
static void Psel_AppTaskmenuWinExit( PSEL_WORK* work );
static void Psel_AppTaskmenuWinMain( PSEL_WORK* work );
static void Psel_AppTaskmenuWinDecideStart( PSEL_WORK* work );
static BOOL Psel_AppTaskmenuWinDecideIsEnd( PSEL_WORK* work );

static void Psel_AppTaskmenuInit( PSEL_WORK* work );
static void Psel_AppTaskmenuExit( PSEL_WORK* work );
static void Psel_AppTaskmenuMain( PSEL_WORK* work );
static BOOL Psel_AppTaskmenuIsEnd( PSEL_WORK* work );
static BOOL Psel_AppTaskmenuIsYes( PSEL_WORK* work );

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
 *  @param[in,out]   evwk          �I������(�X�N���v�g�̔ԍ� 0=����, 1=�ق̂�, 2=�݂� �Ɉ�v������)
 *
 *  @retval          PSEL_PARAM
 */
//------------------------------------------------------------------
PSEL_PARAM*  PSEL_AllocParam(
                                HEAPID               heap_id,
                                u16*                 evwk
                           )
{
  PSEL_PARAM* param = GFL_HEAP_AllocMemory( heap_id, sizeof( PSEL_PARAM ) );
  PSEL_InitParam(
      param,
      evwk
      );
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
 *  @param[in,out]   evwk          �I������(�X�N���v�g�̔ԍ� 0=����, 1=�ق̂�, 2=�݂� �Ɉ�v������)
 *
 *  @retval          
 */
//------------------------------------------------------------------
void  PSEL_InitParam(
                  PSEL_PARAM*      param,
                  u16*             evwk
                         )
{
  param->result             = PSEL_RESULT_KUSA;
  param->evwk               = evwk;
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

  // �t�F�[�h���x�W��
  work->fade_speed_outside_demo = GFL_FADE_GetFadeSpeed();  // ���̃f���ɓ���O�̃t�F�[�h���x�W�����o���Ă���
  GFL_FADE_SetFadeSpeed( FADE_SPEED_INSIDE_DEMO );

  // �^�b�`or�L�[
  work->ktst = GFL_UI_CheckTouchOrKey();

  // �I��ł���|�P����
  work->select_target_poke = TARGET_NONE;
  // �`�J�`�J�I���A�j�������Ă��郂���X�^�[�{�[��
  work->only_mb_select_anime_target = TARGET_NONE;

  // POKE_INFO�e�L�X�g�������t���O
  work->poke_info_print = FALSE;

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

  // �t�F�[�h���x�W��
  GFL_FADE_SetFadeSpeed( work->fade_speed_outside_demo );  // ���̃f���ɓ���O�̃t�F�[�h���x�W���ɖ߂��Ă���

  // �q�[�v�A�p�����[�^�Ȃ�
  {
    // �I�����ʂ��p�����[�^�ɐݒ肵�Ď����A��
    {
      // �I������(�X�N���v�g�̔ԍ� 0=����, 1=�ق̂�, 2=�݂� �Ɉ�v������)
      switch(work->select_target_poke)
      {
      case TARGET_KUSA:
        *(work->param->evwk) = 0;
        break;
      case TARGET_HONOO:
        *(work->param->evwk) = 1;
        break;
      case TARGET_MIZU:
        *(work->param->evwk) = 2;
        break;
      default:
        *(work->param->evwk) = 0;
        break;
      }

      // �I������
      work->param->result  = work->select_target_poke;  // PSEL_RESULT��TARGET�͓������я�
    }

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

  if( work->vblank_req & VBLANK_REQ_S01_TEXT_M_VISIBLE_ON )
  {
    GFL_BG_SetVisible( BG_FRAME_M_TEXT, VISIBLE_ON );
  }

  if( work->vblank_req & VBLANK_REQ_S01_TEXT_M_VISIBLE_OFF )
  {
    GFL_BG_SetVisible( BG_FRAME_M_TEXT, VISIBLE_OFF );
  }

  if( work->vblank_req & VBLANK_REQ_S02_TEXT_WIN_M_VISIBLE_ON )
  {
    GFL_BG_SetVisible( BG_FRAME_M_TEXT, VISIBLE_ON );
    GFL_BG_SetVisible( BG_FRAME_M_WIN, VISIBLE_ON );
  }

  if( work->vblank_req & VBLANK_REQ_S02_TEXT_WIN_M_VISIBLE_OFF )
  {
    GFL_BG_SetVisible( BG_FRAME_M_TEXT, VISIBLE_OFF );
    GFL_BG_SetVisible( BG_FRAME_M_WIN, VISIBLE_OFF );
  }

  if( work->vblank_req & VBLANK_REQ_S02_TEXT_WIN_S_VISIBLE_ON )
  {
    GFL_BG_SetVisible( BG_FRAME_S_TEXT, VISIBLE_ON );
    GFL_BG_SetVisible( BG_FRAME_S_WIN, VISIBLE_ON );
    GFL_BG_SetVisible( BG_FRAME_S_SPOT, VISIBLE_ON );
  }

  if( work->vblank_req & VBLANK_REQ_S02_TEXT_WIN_S_VISIBLE_OFF )
  {
    GFL_BG_SetVisible( BG_FRAME_S_TEXT, VISIBLE_OFF );
    GFL_BG_SetVisible( BG_FRAME_S_WIN, VISIBLE_OFF );
    GFL_BG_SetVisible( BG_FRAME_S_SPOT, VISIBLE_OFF );
  }

  if( work->vblank_req & VBLANK_REQ_S02_TEXT_M_VISIBLE_ON )
  {
    GFL_BG_SetVisible( BG_FRAME_M_TEXT, VISIBLE_ON );
  }
  
  if( work->vblank_req & VBLANK_REQ_S02_TEXT_M_VISIBLE_OFF )
  {
    GFL_BG_SetVisible( BG_FRAME_M_TEXT, VISIBLE_OFF );
  }

  work->vblank_req = VBLANK_REQ_NONE;
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

  // �t���[��
  // �f�U�C�i�[�����3D�I�[�T�����O�c�[����ł�1�t���[������X�^�[�g���Ă���悤�����A�v���O�����ł͊J�n�t���[����0�Ƃ��ď�������
  work->three_frame = 0;
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
  const fx32 anime_speed = THREE_ADD;  // �������iFX32_ONE�łP�t���[���i�߂�j
  
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

  // �t���[��
  work->three_frame++;
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

  // �{�b�N�X�̃A�j���[�V���������L����
  {
    THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[ work->three_obj_prop_tbl_idx[THREE_USER_OBJ_IDX_S02] ]);
    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
    u16 anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
    u16 j;
    for( j=0; j<anime_count; j++ )
    {
      GFL_G3D_OBJECT_EnableAnime( obj, j );
    }
  }
  
  // �J�����A�j��
  {
    work->three_ica_anime = ICA_ANIME_CreateStreamingAlloc(
        work->heap_id, ARCID_PSEL, NARC_psel_psel_s02_cam_bin, THREE_ICA_ANIME_INTERVAL );
    
    ICA_CAMERA_SetCameraStatus( PSEL_GRAPHIC_GetCamera( work->graphic ), work->three_ica_anime );
  }

  // �t���[��
  // �f�U�C�i�[�����3D�I�[�T�����O�c�[����ł�1�t���[������X�^�[�g���Ă���悤�����A�v���O�����ł͊J�n�t���[����0�Ƃ��ď�������
  work->three_frame = 0;
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
  const fx32 anime_speed = THREE_ADD;  // �������iFX32_ONE�łP�t���[���i�߂�j

  // �{�b�N�X�̃A�j���[�V���������X�V
  {
    THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[ work->three_obj_prop_tbl_idx[THREE_USER_OBJ_IDX_S02] ]);
    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
    u16 anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
    u16 j;
    for( j=0; j<anime_count; j++ )
    {
      GFL_G3D_OBJECT_LoopAnimeFrame( obj, j, anime_speed );
    }
  }

  // �J�����A�j��
  {
    ICA_ANIME_IncAnimeFrame( work->three_ica_anime, anime_speed );
    ICA_CAMERA_SetCameraStatus( PSEL_GRAPHIC_GetCamera( work->graphic ), work->three_ica_anime );
  }

  // �t���[��
  work->three_frame++;
}

static void Psel_ThreeS02MbZoomAnimeStart( PSEL_WORK* work, TARGET target, BOOL reverse )
{
  u16 start_frame = timetable_mb[target][(reverse)?(TIMETABLE_MB_ZOOM_END):(TIMETABLE_MB_ZOOM_START)];

  // �J�����A�j��
  {
    Psel_Three_ICA_ANIME_SetAnimeFrame( work->three_ica_anime, FX32_CONST(start_frame) );  // ������ĂԑO�̏�Ԃ̃J�����Ɠ����ʒu�̃J�����̂͂�
    ICA_CAMERA_SetCameraStatus( PSEL_GRAPHIC_GetCamera( work->graphic ), work->three_ica_anime );
  }

  work->three_mb_anime_target   = target;
  work->three_mb_anime_reverse  = reverse;
  work->three_mb_anime_frame    = start_frame;
}
static BOOL Psel_ThreeS02MbZoomAnimeIsEnd( PSEL_WORK* work )
{
  u16 end_frame = timetable_mb[work->three_mb_anime_target][(work->three_mb_anime_reverse)?(TIMETABLE_MB_ZOOM_START):(TIMETABLE_MB_ZOOM_END)];

  if( work->three_mb_anime_frame == end_frame ) return TRUE;
  return FALSE;
}
static void Psel_ThreeS02MbZoomAnimeMain( PSEL_WORK* work )
{
  u16 end_frame = timetable_mb[work->three_mb_anime_target][(work->three_mb_anime_reverse)?(TIMETABLE_MB_ZOOM_START):(TIMETABLE_MB_ZOOM_END)];
  
  if( work->three_mb_anime_frame != end_frame )
  {
    fx32 anime_speed;
    anime_speed = (work->three_mb_anime_reverse)?(-THREE_ADD):(THREE_ADD);  // �������iFX32_ONE�łP�t���[���i�߂�j

    // �J�����A�j��
    {
      ICA_ANIME_IncAnimeFrame( work->three_ica_anime, anime_speed );
      ICA_CAMERA_SetCameraStatus( PSEL_GRAPHIC_GetCamera( work->graphic ), work->three_ica_anime );
    }

    // �t���[��
    (work->three_mb_anime_reverse)?(work->three_mb_anime_frame--):(work->three_mb_anime_frame++);

    if( work->three_mb_anime_frame == end_frame )  // �t���[����i�߂����ƂŁA�I���t���[���ɂȂ�����
    {
      if( work->three_mb_anime_reverse )  // �ǂ̃����X�^�[�{�[�����ΏۂƂ��Ă��Ȃ��J�����ɖ߂�
      {
        // �J�����A�j��
        {
          Psel_Three_ICA_ANIME_SetAnimeFrame( work->three_ica_anime, FX32_CONST(TIMETABLE_S02_BOX_STOP) );  // ������ĂԑO�̏�Ԃ̃J�����Ɠ����ʒu�̃J�����̂͂�
          ICA_CAMERA_SetCameraStatus( PSEL_GRAPHIC_GetCamera( work->graphic ), work->three_ica_anime );
        }
      }
    }
  }
}

static void Psel_ThreeS02MbDecideAnimeStart( PSEL_WORK* work, TARGET target )
{
  u16 start_frame = timetable_mb[target][TIMETABLE_MB_DECIDE_START];

  // �J�����A�j��
  {
    Psel_Three_ICA_ANIME_SetAnimeFrame( work->three_ica_anime, FX32_CONST(start_frame) );  // ������ĂԑO�̏�Ԃ̃J�����Ɠ����ʒu�̃J�����̂͂�
    ICA_CAMERA_SetCameraStatus( PSEL_GRAPHIC_GetCamera( work->graphic ), work->three_ica_anime );
  }

  work->three_mb_anime_target   = target;
  work->three_mb_anime_reverse  = FALSE;
  work->three_mb_anime_frame    = start_frame;
}
static void Psel_ThreeS02MbDecideAnimeMain( PSEL_WORK* work )
{
  u16 end_frame = timetable_mb[work->three_mb_anime_target][TIMETABLE_MB_DECIDE_END];
  
  if( work->three_mb_anime_frame != end_frame )
  {
    fx32 anime_speed = THREE_ADD;  // �������iFX32_ONE�łP�t���[���i�߂�j

    // �J�����A�j��
    {
      ICA_ANIME_IncAnimeFrame( work->three_ica_anime, anime_speed );
      ICA_CAMERA_SetCameraStatus( PSEL_GRAPHIC_GetCamera( work->graphic ), work->three_ica_anime );
    }

    // �t���[��
    work->three_mb_anime_frame++;
  }
}

static void Psel_ThreeS02OnlyMbSelectAnimeStart( PSEL_WORK* work, TARGET target )
{
  // �����X�^�[�{�[���̂�
  u8 i;

  if(    work->only_mb_select_anime_target != TARGET_NONE
      && work->only_mb_select_anime_target != target )
  {
    // ����܂Ń`�J�`�J�I���A�j�������Ă��郂���X�^�[�{�[���̃A�j�����~�߂�
    {
      THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[ work->three_obj_prop_tbl_idx[THREE_USER_OBJ_IDX_MB_L +work->only_mb_select_anime_target] ]);
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
      u16 anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      u16 j;
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_DisableAnime( obj, j );
      }
    }
  }

  if(    target != TARGET_NONE
      && work->only_mb_select_anime_target != target )
  {
    // �Ώۂ̃����X�^�[�{�[�����`�J�`�J�I���A�j��������
    {
      THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[ work->three_obj_prop_tbl_idx[THREE_USER_OBJ_IDX_MB_L +target] ]);
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
      u16 anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      u16 j;
      int anm_frm = 0;
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_EnableAnime( obj, j );
	      GFL_G3D_OBJECT_SetAnimeFrame( obj, j, &anm_frm );  // �K��0�t���[���ڂ���Đ������悤�ɂ��Ă���
      }

      GFL_CLACT_WK_SetAnmIndex( work->finger_clwk, 0 );  // �K��0�t���[���ڂ���n�܂�悤�ɂ��Ă���  // 3D�̃{�[���I���̃A�j���[�V�����Ɠ��������킹�邽�߂ɁA3D�̃{�[���I���̃A�j���[�V������0�t���[����ݒ肳���Ƃ��ɁA�ꏏ�Ɏw�w���J�[�\���ɂ�0�t���[����ݒ肵�Ă��炤�B

    }
  }

  work->only_mb_select_anime_target = target;
}
static void Psel_ThreeS02OnlyMbSelectAnimeMain( PSEL_WORK* work )  // ���t���[���Ă�ł���
{
  // �����X�^�[�{�[���̂�
  if( work->only_mb_select_anime_target != TARGET_NONE )
  {
    fx32 anime_speed = THREE_ADD;  // �������iFX32_ONE�łP�t���[���i�߂�j

    // �Ώۂ̃����X�^�[�{�[���̃A�j���[�V���������X�V
    {
      THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[ work->three_obj_prop_tbl_idx[THREE_USER_OBJ_IDX_MB_L +work->only_mb_select_anime_target] ]);
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
      u16 anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      u16 j;
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_LoopAnimeFrame( obj, j, anime_speed );
      }
    }
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

      cldata.pos_x    = poke_move_data[i].p0_x;
      cldata.pos_y    = poke_move_data[i].p0_y;
      cldata.anmseq   = 0;
      cldata.softpri  = POKE_SOFTPRI_BACK;
      cldata.bgpri    = OBJ_BG_PRI_S_POKE;

      work->poke_set[i].clwk[j] = GFL_CLACT_WK_Create(
          PSEL_GRAPHIC_GetClunit( work->graphic ),
          work->poke_set[i].res[j][TWO_OBJ_RES_NCG],
          work->poke_set[i].res[j][TWO_OBJ_RES_NCL],
          work->poke_set[i].res[j][TWO_OBJ_RES_NCE],
          &cldata, CLSYS_DEFREND_SUB, work->heap_id );
      
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

      claffinedata.clwkdata.pos_x    = poke_move_data[i].p0_x;
      claffinedata.clwkdata.pos_y    = poke_move_data[i].p0_y;
      claffinedata.clwkdata.anmseq   = 0;
      claffinedata.clwkdata.softpri  = POKE_SOFTPRI_BACK;
      claffinedata.clwkdata.bgpri    = OBJ_BG_PRI_S_POKE;

      claffinedata.affinepos_x    = 0;
      claffinedata.affinepos_y    = 0;
      claffinedata.scale_x        = FX_F32_TO_FX32( ((f32)(poke_move_data[i].p0_scale))/16.0f );
      claffinedata.scale_y        = FX_F32_TO_FX32( ((f32)(poke_move_data[i].p0_scale))/16.0f );
      claffinedata.rotation       = 0;  // ��]�p�x(0�`0xffff 0xffff��360�x)
      claffinedata.affine_type    = CLSYS_AFFINETYPE_NORMAL;  // �k���������Ȃ��̂ŁA�{�p�łȂ��Ă悢

      work->poke_set[i].clwk[j] = GFL_CLACT_WK_CreateAffine(
          PSEL_GRAPHIC_GetClunit( work->graphic ),
          work->poke_set[i].res[j][TWO_OBJ_RES_NCG],
          //work->poke_set[i].res[j][TWO_OBJ_RES_NCL],
          work->poke_set[i].res[POKE_SMALL][TWO_OBJ_RES_NCL],
          work->poke_set[i].res[j][TWO_OBJ_RES_NCE],
          &claffinedata, CLSYS_DEFREND_SUB, work->heap_id );
      
      GFL_CLACT_WK_SetDrawEnable( work->poke_set[i].clwk[j], FALSE );
    }
  }

  // ����
  {
    for( i=0; i<TARGET_POKE_MAX; i++ )
    {
      work->poke_set[i].move_step_count   = 0;
      work->poke_set[i].move_step         = POKE_MOVE_STEP_P0_STOP;
      work->poke_set[i].move_req          = POKE_MOVE_REQ_NONE;
    }
  }

  // �A���t�@�A�j��
  {
    work->poke_set_ev1 = 0;
    work->poke_set_alpha_anime_state = POKE_SET_ALPHA_ANIME_STATE_NONE;
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

static s16 PokeSetCalcPosScale( GFL_CLACTPOS* pos, GFL_CLSCALE* scale,
                                 u8 pos_s_x, u8 pos_s_y, u8 pos_e_x, u8 pos_e_y,
                                 u8 scale_s, u8 scale_e,
                                 u8 total_frame, u8 jump_y,
                                 u16 count );
static s16 PokeSetCalcPosScale( GFL_CLACTPOS* pos, GFL_CLSCALE* scale,
                                 u8 pos_s_x, u8 pos_s_y, u8 pos_e_x, u8 pos_e_y,
                                 u8 scale_s, u8 scale_e,
                                 u8 total_frame, u8 jump_y,
                                 u16 count )
{
  // �W�����v������O��y�̒l��Ԃ�
  s16           ret_y;

  f32           f_x       = ( (f32)pos_e_x - (f32)pos_s_x ) * (f32)count / (f32)total_frame + (f32)pos_s_x;
  f32           f_y       = ( (f32)pos_e_y - (f32)pos_s_y ) * (f32)count / (f32)total_frame + (f32)pos_s_y;
          
  // ���߂鍂��                                y
  // �ő�̃W�����v�̍���                      h
  // �W�����v�����Ԃ̈ړ��ɂ�����t���[����  f
  // ���݂̃t���[����(0<=t<f)                  t
  // t=0�̂Ƃ�y=0, t=f�̂Ƃ�y=0, t=f/2�̂Ƃ�y=h�ƂȂ�悤�ɂ���
  // y = -h * ( (t-(f/2))/(f/2) )*( (t-(f/2))/(f/2) ) + h
  f32           f_y_ofs   = - ((f32)jump_y) \
                            * ( (f32)count - (f32)total_frame/2.0f ) * ( (f32)count - (f32)total_frame/2.0f ) \
                            * 4.0f / (f32)total_frame / (f32)total_frame
                            + (f32)jump_y;
          
  f32           f_scale   = ( (f32)scale_e - (f32)scale_s ) * (f32)count / (f32)total_frame + (f32)scale_s;
  s16           s_x       = (s16)f_x;
  s16           s_y       = (s16)(f_y - f_y_ofs);  // ���ɂ����قǐ��Ȃ̂Ń}�C�i�X����
  fx32          fx_scale  = FX_F32_TO_FX32( f_scale / 16.0f );
  
  pos->x    = s_x;
  pos->y    = s_y;
  scale->x  = scale->y  = fx_scale;

  ret_y = (s16)f_y;
  return ret_y;
} 

static void Psel_PokeSetMain( PSEL_WORK* work )
{
  // �|�P�����召�Z�b�g�̓����̍X�V(�p���b�g�A�j����Psel_PokeSetPalMain�ɔC���Ă���)

  u8 i;
  u8 j;
  s16 before_jump_y[TARGET_POKE_MAX];
  BOOL before_jump_y_get[TARGET_POKE_MAX] = { FALSE, FALSE, FALSE };

  for( i=0; i<TARGET_POKE_MAX; i++ )
  {
    POKE_SET*               p = &(work->poke_set[i]);
    const POKE_MOVE_DATA*   d = &(poke_move_data[i]);
    
    s8 count_add = 0;

    if( p->move_req == POKE_MOVE_REQ_NONE ) continue;

    switch( p->move_req )
    {
    case POKE_MOVE_REQ_NONE:     count_add =  0; break;
    case POKE_MOVE_REQ_P0_TO_P2: count_add = +1; break;
    case POKE_MOVE_REQ_P2_TO_P0: count_add = -1; break;
    }

    // �߂�͈�u��
    {
      if( count_add < 0 )
      {
        p->move_step_count = 0;
        p->move_step = POKE_MOVE_STEP_P0_TO_P1;
      }
    }

    switch( p->move_step )
    {
    case POKE_MOVE_STEP_P0_STOP:
      {
        if( count_add > 0 )
        {
          GFL_CLACT_WK_SetDrawEnable( p->clwk[POKE_SMALL], FALSE );
          GFL_CLACT_WK_SetDrawEnable( p->clwk[POKE_BIG], TRUE );

          p->move_step_count = 0;
          p->move_step = POKE_MOVE_STEP_P0_TO_P1;
          
          // break;���Ȃ�
        }
        else
        {
          if( p->move_req == POKE_MOVE_REQ_P2_TO_P0 ) p->move_req = POKE_MOVE_REQ_NONE;
          
          break;
        }
      }
    case POKE_MOVE_STEP_P0_TO_P1:
      {
        if( p->move_step_count == 0 )
        {
          GFL_CLACTPOS  pos;
          GFL_CLSCALE   scale;
          pos.x    = d->p0_x;
          pos.y    = d->p0_y;
          scale.x  = scale.y  = FX_F32_TO_FX32( (f32)d->p0_scale / 16.0f );
          GFL_CLACT_WK_SetPos( p->clwk[POKE_BIG], &pos, CLSYS_DEFREND_SUB );
          GFL_CLACT_WK_SetScale( p->clwk[POKE_BIG], &scale );
        }
        else
        {
          GFL_CLACTPOS  pos;
          GFL_CLSCALE   scale;
          before_jump_y[i] = PokeSetCalcPosScale( &pos, &scale,
                               d->p0_x, d->p0_y, d->p1_x, d->p1_y,
                               d->p0_scale, d->p1_scale,
                               d->p0p1_frame, d->p0p1_jump,
                               p->move_step_count );
          before_jump_y_get[i] = TRUE;
          GFL_CLACT_WK_SetPos( p->clwk[POKE_BIG], &pos, CLSYS_DEFREND_SUB );
          GFL_CLACT_WK_SetScale( p->clwk[POKE_BIG], &scale );
        }

        if( p->move_step_count == 0 && count_add < 0 )
        {
          GFL_CLACT_WK_SetDrawEnable( p->clwk[POKE_SMALL], TRUE );
          GFL_CLACT_WK_SetDrawEnable( p->clwk[POKE_BIG], FALSE );
            
          p->move_step_count = 0;
          p->move_step = POKE_MOVE_STEP_P0_STOP;
        }
        else
        {
          p->move_step_count = p->move_step_count + count_add;
          if( p->move_step_count == d->p0p1_frame )
          {
            p->move_step_count = 0;
            p->move_step = POKE_MOVE_STEP_P1_WAIT;
          }
        }
      }
      break;
    case POKE_MOVE_STEP_P1_WAIT:
      {
        GFL_CLACTPOS  pos;
        GFL_CLSCALE   scale;
        pos.x    = d->p1_x;
        pos.y    = d->p1_y;
        scale.x  = scale.y  = FX_F32_TO_FX32( (f32)d->p1_scale / 16.0f );
        GFL_CLACT_WK_SetPos( p->clwk[POKE_BIG], &pos, CLSYS_DEFREND_SUB );
        GFL_CLACT_WK_SetScale( p->clwk[POKE_BIG], &scale );

        if( p->move_step_count == 0 && count_add < 0 )
        {
          p->move_step_count = d->p0p1_frame -1;
          p->move_step = POKE_MOVE_STEP_P0_TO_P1;
        }
        else
        {
          p->move_step_count = p->move_step_count + count_add;
          if( p->move_step_count == d->p1_frame )
          {
            p->move_step_count = 0;
            p->move_step = POKE_MOVE_STEP_P1_TO_P2;
          }
        }
      }
      break;
    case POKE_MOVE_STEP_P1_TO_P2:
      {
        if( p->move_step_count == 0 )
        {
          GFL_CLACTPOS  pos;
          GFL_CLSCALE   scale;
          pos.x    = d->p1_x;
          pos.y    = d->p1_y;
          scale.x  = scale.y  = FX_F32_TO_FX32( (f32)d->p1_scale / 16.0f );
          GFL_CLACT_WK_SetPos( p->clwk[POKE_BIG], &pos, CLSYS_DEFREND_SUB );
          GFL_CLACT_WK_SetScale( p->clwk[POKE_BIG], &scale );
        }
        else
        {
          GFL_CLACTPOS  pos;
          GFL_CLSCALE   scale;
          before_jump_y[i] = PokeSetCalcPosScale( &pos, &scale,
                               d->p1_x, d->p1_y, d->p2_x, d->p2_y,
                               d->p1_scale, d->p2_scale,
                               d->p1p2_frame, d->p1p2_jump,
                               p->move_step_count );
          before_jump_y_get[i] = TRUE;
          GFL_CLACT_WK_SetPos( p->clwk[POKE_BIG], &pos, CLSYS_DEFREND_SUB );
          GFL_CLACT_WK_SetScale( p->clwk[POKE_BIG], &scale );
        }

        if( p->move_step_count == 0 && count_add < 0 )
        {
          p->move_step_count = d->p1_frame -1;
          p->move_step = POKE_MOVE_STEP_P1_WAIT;
        }
        else
        {
          p->move_step_count = p->move_step_count + count_add;
          if( p->move_step_count == d->p1p2_frame )
          {
            p->move_step_count = 0;
            p->move_step = POKE_MOVE_STEP_P2_STOP;
          }
        }
      }
      break;
    case POKE_MOVE_STEP_P2_STOP:
      {
        GFL_CLACTPOS  pos;
        GFL_CLSCALE   scale;
        pos.x    = d->p2_x;
        pos.y    = d->p2_y;
        scale.x  = scale.y  = FX_F32_TO_FX32( (f32)d->p2_scale / 16.0f );
        GFL_CLACT_WK_SetPos( p->clwk[POKE_BIG], &pos, CLSYS_DEFREND_SUB );
        GFL_CLACT_WK_SetScale( p->clwk[POKE_BIG], &scale );
        
        if( count_add < 0 )
        {
          GFL_CLACT_WK_SetBgPri( p->clwk[POKE_BIG], OBJ_BG_PRI_S_POKE );  // bgpri
          p->move_step_count = d->p1p2_frame -1;
          p->move_step = POKE_MOVE_STEP_P1_TO_P2;
        }
        else
        {
          if( p->move_req == POKE_MOVE_REQ_P0_TO_P2 )
          {
            GFL_CLACT_WK_SetBgPri( p->clwk[POKE_BIG], OBJ_BG_PRI_S_POKE_SPOT );  // bgpri
            p->move_req = POKE_MOVE_REQ_NONE;
            work->poke_info_print = TRUE;  // �|�P�����̃^�C�v�Ǝ푰��������
          }
        }
      }
      break;
    }
  }

  // softpri
  {
    u8 order[TARGET_POKE_MAX] = { TARGET_KUSA, TARGET_HONOO, TARGET_MIZU };  // order[0]=y�ő��TARGET_???; order[1]=y����TARGET_???; order[2]=y�ŏ���TARGET_???;
    s16 y[TARGET_POKE_MAX];  // y[TARGET_KUSA]=TARGET_KUSA��y; y[TARGET_HONOO]=TARGET_HONOO��y; y[TARGET_MIZU]=TARGET_MIZU��y;
    GFL_CLACTPOS pos;
    for( i=0; i<TARGET_POKE_MAX; i++ )
    {
      // �W�����v��̒l�ł͂��ĂɂȂ�Ȃ��̂ŁA�W�����v�O�̒l��p����
      if( before_jump_y_get[i] )
      {
        y[i] = before_jump_y[i];
      }
      else
      {
        GFL_CLACT_WK_GetPos( work->poke_set[i].clwk[POKE_BIG], &pos, CLSYS_DEFREND_SUB );
        y[i] = pos.y;
      }
    }
    for( i=0; i<TARGET_POKE_MAX; i++ )
    {
      for( j=TARGET_POKE_MAX -1; j>i; j-- )
      {
        if( y[order[j]] > y[order[j-1]] )
        {
          u8 temp = order[j-1];
          order[j-1] = order[j];
          order[j] = temp;
        }
      }
    }
    for( i=0; i<TARGET_POKE_MAX; i++ )
    {
      u8 softpri;
      if( order[0] == i )      softpri=POKE_SOFTPRI_FRONT;
      else if( order[1] == i ) softpri=POKE_SOFTPRI_MIDDLE;
      else                     softpri=POKE_SOFTPRI_BACK;    // if( order[2] == i )
      GFL_CLACT_WK_SetSoftPri( work->poke_set[i].clwk[POKE_BIG], softpri );
    }
  }
}

static void Psel_PokeSetSelectStart( PSEL_WORK* work, TARGET target_poke )  // ����Ă�ł��A�A�j���̓r���ŌĂ�ł��A���v�Ȋ拭�Ȃ���ɂȂ��Ă���
{
  u8 i;

  for( i=0; i<TARGET_POKE_MAX; i++ )
  {
    // �p���b�g�A�j��
    {
      if( i == target_poke )
      {
        Psel_PokeSetPalAnimeStart( work, i, POKE_PAL_ANIME_STATE_COLOR );
      }
      else
      {
        Psel_PokeSetPalAnimeStart( work, i, POKE_PAL_ANIME_STATE_DARK );
      }
    }

    // ����
    {
      if( i == target_poke )
      {
        work->poke_set[i].move_req   = POKE_MOVE_REQ_P0_TO_P2;
        //GFL_CLACT_WK_SetSoftPri( work->poke_set[i].clwk[POKE_BIG], POKE_SOFTPRI_FRONT );  // ���ڂ��Ă��Ȃ��|�P�����̂ق�����O�̂��Ƃ�����̂ŁA���t���[���ݒ肷�邱�Ƃɂ����B
      }
      else
      {
        work->poke_set[i].move_req   = POKE_MOVE_REQ_P2_TO_P0;
        //GFL_CLACT_WK_SetSoftPri( work->poke_set[i].clwk[POKE_BIG], POKE_SOFTPRI_BACK );  // ���ڂ��Ă��Ȃ��|�P�����̂ق�����O�̂��Ƃ�����̂ŁA���t���[���ݒ肷�邱�Ƃɂ����B
      }
    }
  }
}


//-------------------------------------
/// �|�P�����召�Z�b�g�A���t�@�A�j��
//=====================================
static void Psel_PokeSetAlphaAnimeStart( PSEL_WORK* work )
{
  u8 i;
  u8 j;
  // small
  j = POKE_SMALL;
  // CLWK������
  {
    for( i=0; i<TARGET_POKE_MAX; i++ )
    {
      GFL_CLACT_WK_SetObjMode( work->poke_set[i].clwk[j], GX_OAM_MODE_XLU );
    }
  }

  // �A���t�@�A�j��
  work->poke_set_ev1 = 0;
  work->poke_set_alpha_anime_state = POKE_SET_ALPHA_ANIME_STATE_0;

  G2S_SetBlendAlpha(
      GX_BLEND_PLANEMASK_NONE,
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
      work->poke_set_ev1, 16 - work->poke_set_ev1 );
}
static BOOL Psel_PokeSetIsAlphaAnimeEnd( PSEL_WORK* work )
{
  if( work->poke_set_alpha_anime_state == POKE_SET_ALPHA_ANIME_STATE_NONE )  return TRUE;
  else                                                                       return FALSE;
}
static void Psel_PokeSetAlphaAnimeMain( PSEL_WORK* work )
{
  switch( work->poke_set_alpha_anime_state )
  {
  case POKE_SET_ALPHA_ANIME_STATE_0:
    {
      u8 i;
      u8 j;
      // small
      j = POKE_SMALL;
      // CLWK�`��
      {
        for( i=0; i<TARGET_POKE_MAX; i++ )
        {
          GFL_CLACT_WK_SetDrawEnable( work->poke_set[i].clwk[j], TRUE );
        }
      }

      work->poke_set_alpha_anime_state = POKE_SET_ALPHA_ANIME_STATE_ANIME;
    }
    break;
  case POKE_SET_ALPHA_ANIME_STATE_ANIME:
    {
      if( work->poke_set_ev1 < 16 )
      {
        work->poke_set_ev1++;
          
        if( work->poke_set_ev1 == 16 )
        {
          // �A���t�@�A�j�������ς�
          u8 i;
          u8 j;
          // small
          j = POKE_SMALL;
          // CLWK�s����
          {
            for( i=0; i<TARGET_POKE_MAX; i++ )
            {
              GFL_CLACT_WK_SetObjMode( work->poke_set[i].clwk[j], GX_OAM_MODE_NORMAL );
            }
          }
      
          work->poke_set_alpha_anime_state = POKE_SET_ALPHA_ANIME_STATE_16;
        }
      
        G2S_SetBlendAlpha(
            GX_BLEND_PLANEMASK_NONE,
            GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
            work->poke_set_ev1, 16 - work->poke_set_ev1 );
      }
      else
      {
        work->poke_set_alpha_anime_state = POKE_SET_ALPHA_ANIME_STATE_16;
      }
    }
    break;
  case POKE_SET_ALPHA_ANIME_STATE_16:
    {
      G2S_SetBlendAlpha(
        GX_BLEND_PLANEMASK_BG2/* | GX_BLEND_PLANEMASK_BG3*/,
        GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
        8, 8 );
      
      work->poke_set_alpha_anime_state = POKE_SET_ALPHA_ANIME_STATE_NONE;
    }
    break;
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

    cldata.pos_x    = finger_pos[TARGET_POKE_START][0];
    cldata.pos_y    = finger_pos[TARGET_POKE_START][1];
    cldata.anmseq   = 0;
    cldata.softpri  = 0;
    cldata.bgpri    = OBJ_BG_PRI_M_FINGER;

    work->finger_clwk = GFL_CLACT_WK_Create(
        PSEL_GRAPHIC_GetClunit( work->graphic ),
        work->finger_res[TWO_OBJ_RES_NCG], work->finger_res[TWO_OBJ_RES_NCL], work->finger_res[TWO_OBJ_RES_NCE],
        &cldata, CLSYS_DEFREND_MAIN, work->heap_id );
    GFL_CLACT_WK_SetAutoAnmFlag( work->finger_clwk, TRUE );

    //GFL_CLACT_WK_SetDrawEnable( work->finger_clwk, FALSE );  // 3D�̃{�[���I���̃A�j���[�V�����Ɠ��������킹�邽�߂ɁA���ۂɂ͏����������Ȃ��ʒu�ɒu���Ă���
    Psel_FingerDrawEnable( work, FALSE );
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
static void Psel_FingerDrawEnable( PSEL_WORK* work, BOOL on_off )
{
  //GFL_CLACT_WK_SetDrawEnable( work->finger_clwk, on_off );  // 3D�̃{�[���I���̃A�j���[�V�����Ɠ��������킹�邽�߂ɁA���ۂɂ͏����������Ȃ��ʒu�ɒu���Ă���
  //if( on_off ) Psel_FingerUpdatePos( work );

  if( on_off )
  {
    Psel_FingerUpdatePos( work );
  }
  else
  {
    // 3D�̃{�[���I���̃A�j���[�V�����Ɠ��������킹�邽�߂ɁA���ۂɂ͏����������Ȃ��ʒu�ɒu���Ă���
    GFL_CLACTPOS pos;
    pos.x = 400;
    pos.y = 300;
    GFL_CLACT_WK_SetPos( work->finger_clwk, &pos, CLSYS_DEFREND_MAIN );
  }
}
static void Psel_FingerUpdatePos( PSEL_WORK* work )
{
  GFL_CLACTPOS pos;
  pos.x = finger_pos[work->select_target_poke][0];
  pos.y = finger_pos[work->select_target_poke][1];
  GFL_CLACT_WK_SetPos( work->finger_clwk, &pos, CLSYS_DEFREND_MAIN );
  //GFL_CLACT_WK_SetAnmIndex( work->finger_clwk, 0 );  // �K��0�t���[���ڂ���n�܂�悤�ɂ��Ă���  // 3D�̃{�[���I���̃A�j���[�V�����Ɠ��������킹�邽�߂ɁA3D�̃{�[���I���̃A�j���[�V������0�t���[����ݒ肳���Ƃ��ɁA�ꏏ�Ɏw�w���J�[�\���ɂ�0�t���[����ݒ肵�Ă��炤�B
}

//-------------------------------------
/// BG
//=====================================
static void Psel_BgInit( PSEL_WORK* work )
{
  // �E�B���h�E�p�̃p���b�g�ƃL������p�ӂ���

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
  // �E�B���h�E�p�̃X�N���[����p�ӂ���

  // �X�N���[���̍쐬���]��
  {
    u16* scr = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(u16) * 32*24 );
    u8 i, j;
    u16 h = 0;
    for(j=0; j<24; j++)       // TWL_ProgramingManual.pdf TWL�v���O���~���O�}�j���A��
    {                         // 6.2.3.2.2 �X�N���[���f�[�^�̃A�h���X�}�b�s���O
      for(i=0; i<32; i++)     // �X�N���[���T�C�Y��256�~256 �h�b�g�̂Ƃ�
      {                       // ���Q�l�ɂ����B
        u16 chara_name = (4<=j&&j<20)?1:0;  // 1�L������:0�L������
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

static void Psel_BgS01SubInit( PSEL_WORK* work )
{
  ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_PSEL, work->heap_id );

  GFL_ARCHDL_UTIL_TransVramPaletteEx(
      handle,
      NARC_psel_psel_bg01_NCLR,
      PALTYPE_SUB_BG,
      BG_PAL_POS_S_INSIDE_BALL * 0x20,
      BG_PAL_POS_S_INSIDE_BALL * 0x20,
      BG_PAL_NUM_S_INSIDE_BALL * 0x20,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramBgCharacter(
      handle,
      NARC_psel_psel_bg01_NCGR,
      BG_FRAME_S_INSIDE_BALL,
			0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  GFL_ARCHDL_UTIL_TransVramScreen(
      handle,
      NARC_psel_psel_bg01_NSCR,
      BG_FRAME_S_INSIDE_BALL,
      0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenReq( BG_FRAME_S_INSIDE_BALL );
}
static void Psel_BgS01SubExit( PSEL_WORK* work )
{
}

static void Psel_BgS02Init( PSEL_WORK* work )
{
  // �E�B���h�E�p�̃X�N���[����p�ӂ���

  // ���C�����
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

  // �T�u���
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
  u16 chara_name = 0;  // 0�L������
  u16 flip_h     = 0;
  u16 flip_v     = 0;
  u16 pal;
  u16 scr;

  // ���C�����
  {
    pal        = BG_PAL_POS_M_WIN;
    scr = ( pal << 12 ) | ( flip_v << 11 ) | ( flip_h << 10 ) | ( chara_name << 0 );
    GFL_BG_ClearScreenCodeVReq( BG_FRAME_M_WIN, scr );
  }

  // �T�u���
  {
    pal        = BG_PAL_POS_S_WIN;
    scr = ( pal << 12 ) | ( flip_v << 11 ) | ( flip_h << 10 ) | ( chara_name << 0 );
    GFL_BG_ClearScreenCodeVReq( BG_FRAME_S_WIN, scr );
  }
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

  GFL_ARCHDL_UTIL_TransVramScreen(
      handle,
      NARC_psel_psel_bg02a_NSCR,
      BG_FRAME_S_SPOT,
      0,
      0,  // �S�]��
      FALSE,
      work->heap_id );

  GFL_ARC_CloseDataHandle( handle );

  GFL_BG_LoadScreenReq( BG_FRAME_S_INSIDE_BALL );
  GFL_BG_LoadScreenReq( BG_FRAME_S_SPOT );
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

  // TCBL
  GFL_TCBL_Exit( work->ps_tcblsys );

  // ���b�Z�[�W
  GFL_MSG_Delete( work->msgdata );
}
static void Psel_TextMain( PSEL_WORK* work )
{
  GFL_TCBL_Main( work->ps_tcblsys );
  Psel_TextTransWait( work );
}

static BOOL Psel_TextTransWait( PSEL_WORK* work )  // TRUE���Ԃ��ė�����]���҂��Ȃ�
{
  // ���̊֐��͉��x�Ăяo���Ă������͕ς��Ȃ��̂ŁA���x�Ăяo���Ă������B
  // ���x���Ăяo��������Ƃ����Ă͂₭�������s����킯�ł͂Ȃ����A������]�������肷��킯�ł͂Ȃ��̂ŁB

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

static void Psel_TextExplainStreamStart( PSEL_WORK* work, u32 str_id )
{
  GFL_BMPWIN* bmpwin = work->text_bmpwin[TEXT_EXPLAIN];
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
  }

  // �Z���^�����O
  {
    u16     str_width   = (u16)( PRINTSYS_GetStrWidth( work->ps_strbuf, work->font, 0 ) );
    u16     str_height  = (u16)( PRINTSYS_GetStrHeight( work->ps_strbuf, work->font ) ); 

    u16     bmp_width   = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp(bmpwin) );
    u16     bmp_height  = GFL_BMP_GetSizeY( GFL_BMPWIN_GetBmp(bmpwin) );

    // �Z���^�����O
    if( bmp_width > str_width )
    {
      x = (s16)( ( bmp_width - str_width ) / 2 );
    }
    if( bmp_height > str_height )
    {
      y = (s16)( ( bmp_height - str_height ) / 2 );
    }
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

  // �X�N���[���������Ă����Ȃ��ƕ\������Ȃ�
  GFL_BMPWIN_MakeTransWindow_VBlank( bmpwin );
}
static BOOL Psel_TextExplainStreamWait( PSEL_WORK* work )  // TRUE���Ԃ��ė�����I��
{
  BOOL ret = FALSE;

  switch( PRINTSYS_PrintStreamGetState( work->print_stream ) )
  {
  case PRINTSTREAM_STATE_RUNNING:
    if( ( GFL_UI_KEY_GetCont() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || GFL_UI_TP_GetCont() )
    {
      PRINTSYS_PrintStreamShortWait( work->print_stream, 0 );
      
      if( GFL_UI_KEY_GetCont() & (PAD_BUTTON_A|PAD_BUTTON_B) ) work->ktst = GFL_APP_KTST_KEY;
      else                                                     work->ktst = GFL_APP_KTST_TOUCH;
    }
    break;
  case PRINTSTREAM_STATE_PAUSE:
    if( ( GFL_UI_KEY_GetTrg() & ( PAD_BUTTON_A | PAD_BUTTON_B ) ) || GFL_UI_TP_GetTrg() )
    { 
      PRINTSYS_PrintStreamReleasePause( work->print_stream );

      if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ) work->ktst = GFL_APP_KTST_KEY;
      else                                                    work->ktst = GFL_APP_KTST_TOUCH;
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

static void Psel_TextExplainPrintStart( PSEL_WORK* work, u32 str_id )
{
  GFL_BMPWIN* bmpwin = work->text_bmpwin[TEXT_EXPLAIN];
  BOOL*       finish = &(work->text_finish[TEXT_EXPLAIN]);
  STRBUF* strbuf;
  u16     str_width;
  u16     str_height; 
  u16     bmp_width;
  u16     bmp_height;
  u16     x = 0;
  u16     y = 0;

  // ��U����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmpwin), TEXT_COLOR_B );
  
  // �O�̂�����
  if( work->print_stream ) PRINTSYS_PrintStreamDelete( work->print_stream );  // ���̊֐��Ń^�X�N���폜���Ă����̂ŁA�����ɓ����^�X�N��1�ōς�
  work->print_stream = NULL;  // ���̊֐���print_stream�͎g��Ȃ��̂�NULL�����Ă������ƁB
  if( work->ps_strbuf ) GFL_STR_DeleteBuffer( work->ps_strbuf );
  work->ps_strbuf = NULL;  // ���̊֐���ps_strbuf�͎g��Ȃ��̂�NULL�����Ă������ƁB

  // ������쐬
  strbuf      = GFL_MSG_CreateString( work->msgdata, str_id );

  str_width   = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
  str_height  = (u16)( PRINTSYS_GetStrHeight( strbuf, work->font ) ); 

  bmp_width   = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp(bmpwin) );
  bmp_height  = GFL_BMP_GetSizeY( GFL_BMPWIN_GetBmp(bmpwin) );

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
      GFL_BMPWIN_GetBmp(bmpwin),
      (s16)x, (s16)y,
      strbuf,
      work->font,
      PRINTSYS_LSB_Make(TEXT_COLOR_L,TEXT_COLOR_S,TEXT_COLOR_B) );

  GFL_STR_DeleteBuffer( strbuf );

  *finish = FALSE;

  // �����ɓ]���ł��邩������Ȃ��̂ŁA������1�x�Ă�ł���
  Psel_TextTransWait( work );
}
static void Psel_TextExplainClear( PSEL_WORK* work )
{
  // ����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_EXPLAIN]), TEXT_COLOR_B );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[TEXT_EXPLAIN] );

  // �X�g���[����\�����Ă���Ƃ���������Ȃ��̂ŁA�X�g���[�����폜 
  if( work->print_stream ) PRINTSYS_PrintStreamDelete( work->print_stream );  // ���̊֐��Ń^�X�N���폜���Ă����̂ŁA�����ɓ����^�X�N��1�ōς�
  work->print_stream = NULL;  // ���̊֐���print_stream�͎g��Ȃ��̂�NULL�����Ă������ƁB
  if( work->ps_strbuf ) GFL_STR_DeleteBuffer( work->ps_strbuf );
  work->ps_strbuf = NULL;  // ���̊֐���ps_strbuf�͎g��Ȃ��̂�NULL�����Ă������ƁB
}

static void Psel_TextPokeInfoPrintStart( PSEL_WORK* work, TARGET target )
{
  GFL_BMPWIN* bmpwin = work->text_bmpwin[TEXT_POKE_INFO];
  BOOL*       finish = &(work->text_finish[TEXT_POKE_INFO]);
  u16     str_width;
  u16     str_height; 
  u16     bmp_width;
  u16     bmp_height;
  u16     x = 0;
  u16     y = 0;

  WORDSET*  wordset       = WORDSET_Create( work->heap_id );
  STRBUF*   src_strbuf    = GFL_MSG_CreateString( work->msgdata, poke_str_id[target] );
  STRBUF*   strbuf        = GFL_STR_CreateBuffer( TEXT_POKE_INFO_LEN_MAX, work->heap_id );
  WORDSET_RegisterPokeMonsNameNo( wordset, 1, poke_monsno[target] );
  WORDSET_ExpandStr( wordset, strbuf, src_strbuf );
  GFL_STR_DeleteBuffer( src_strbuf );
  WORDSET_Delete( wordset );

  // ��U����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(bmpwin), TEXT_COLOR_B );

  str_width   = (u16)( PRINTSYS_GetStrWidth( strbuf, work->font, 0 ) );
  str_height  = (u16)( PRINTSYS_GetStrHeight( strbuf, work->font ) ); 

  bmp_width   = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp(bmpwin) );
  bmp_height  = GFL_BMP_GetSizeY( GFL_BMPWIN_GetBmp(bmpwin) );

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
      GFL_BMPWIN_GetBmp(bmpwin),
      (s16)x, (s16)y,
      strbuf,
      work->font,
      PRINTSYS_LSB_Make(TEXT_COLOR_L,TEXT_COLOR_S,TEXT_COLOR_B) );

  GFL_STR_DeleteBuffer( strbuf );

  *finish = FALSE;

  // �����ɓ]���ł��邩������Ȃ��̂ŁA������1�x�Ă�ł���
  Psel_TextTransWait( work );
}
static void Psel_TextPokeInfoClear( PSEL_WORK* work )
{
  // ����
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_POKE_INFO]), TEXT_COLOR_B );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[TEXT_POKE_INFO] );
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

  // �����ɓ]���ł��邩������Ȃ��̂ŁA������1�x�Ă�ł���
  Psel_TextTransWait( work );
}
static void Psel_TextS01Exit( PSEL_WORK* work )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(work->text_bmpwin[TEXT_LETTER]), TEXT_COLOR_B );
  GFL_BMPWIN_MakeTransWindow_VBlank( work->text_bmpwin[TEXT_LETTER] );
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
static void Psel_AppTaskmenuWinDecideStart( PSEL_WORK* work )
{
  APP_TASKMENU_WIN_SetActive( work->app_taskmenu_win_wk, TRUE );
  APP_TASKMENU_WIN_SetDecide( work->app_taskmenu_win_wk, TRUE );
}
static BOOL Psel_AppTaskmenuWinDecideIsEnd( PSEL_WORK* work )
{
  return APP_TASKMENU_WIN_IsFinish( work->app_taskmenu_win_wk );
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
static BOOL Psel_AppTaskmenuIsEnd( PSEL_WORK* work )
{
  return APP_TASKMENU_IsFinish( work->app_taskmenu_wk );
}
static BOOL Psel_AppTaskmenuIsYes( PSEL_WORK* work )
{
  u8 pos = APP_TASKMENU_GetCursorPos( work->app_taskmenu_wk );
  if( pos == 0 ) return TRUE;
  return FALSE;
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
          
          // �߂�͈�u��
          {
            work->poke_set[i].pal_anime_count = 16;
            need_trans = TRUE;
          }
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

static void Psel_PokeSetPalAnimeStart( PSEL_WORK* work, TARGET target_poke, POKE_PAL_ANIME_STATE state )  // ����Ă�ł��A�A�j���̓r���ŌĂ�ł��A���v�Ȋ拭�Ȃ���ɂȂ��Ă���
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

static void Psel_InsideBallPalAnimeStart( PSEL_WORK* work, u8 end )  // ����Ă�ł��A�A�j���̓r���ŌĂ�ł��A���v�Ȋ拭�Ȃ���ɂȂ��Ă���
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
  GFL_BG_SetPriority( BG_FRAME_S_SPOT,            BG_FRAME_PRI_S_SPOT );
  GFL_BG_SetPriority( BG_FRAME_S_WIN,             BG_FRAME_PRI_S_WIN );
  GFL_BG_SetPriority( BG_FRAME_S_TEXT,            BG_FRAME_PRI_S_TEXT );

  // VBlank��TCB
  work->vblank_tcb = GFUser_VIntr_CreateTCB( Psel_VBlankFunc, work, 1 );
  work->vblank_req = VBLANK_REQ_NONE;

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

  // ����
  Psel_ThreeS01Init( work );
  Psel_BgS01Init( work );
  Psel_BgS01SubInit( work );
  Psel_TextS01Init( work );

  // �e�L�X�g�͔�\��
  work->vblank_req |= VBLANK_REQ_S01_TEXT_M_VISIBLE_OFF;

  // �u�����h
  work->ev1 = 0; 
  G2_SetBlendAlpha(
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2,
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
      work->ev1, 16-work->ev1 );
  // 3D�̃A���t�@�����ꂢ�ɏo�����߁A��1�Ώۖʂ�BG0���w�肵�Ă���
  // ev1��ev2�͎g���Ȃ�  // TWL�v���O���~���O�}�j���A���u2D�ʂƂ̃��u�����f�B���O�v�Q�l

  // �V�[�P���X�t���[��
  work->sub_seq_frame = 0;
  return SEQ_S01_FADE_IN;
}
static int Psel_S01FadeIn  ( PSEL_WORK* work, int* seq )
{
  if( work->sub_seq_frame >= 1 ) Psel_ThreeS01Main( work );  // 3D�̃A�j���J�n�������x�点��

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
        // �V�[�P���X�t���[��
        work->sub_seq_frame = 0;
        return SEQ_S01_MAIN;
      }
    }
    break;
  }

  // �V�[�P���X�t���[��
  work->sub_seq_frame++;
  return SEQ_S01_FADE_IN;
}
static int Psel_S01Main    ( PSEL_WORK* work, int* seq )
{
  BOOL blend_m = FALSE;

  switch(*seq)
  {
  case 0:  // �`3D�̃A�j�����莆��ǂޒi�K��4�t���[���O�ɂȂ�܂�
    {
      Psel_ThreeS01Main( work );
      if( work->three_frame == TIMETABLE_S01_CARD_READ -4 )
      {
        (*seq)++;
      }
    }
    break;
  case 1:  // �`3D�̃A�j�����莆��ǂޒi�K�ɂȂ�܂�
    {
      // �u�����h
      if( work->ev1 < 8 )
      {
        work->ev1++;
        blend_m = TRUE;
      }

      Psel_ThreeS01Main( work );
      if( work->three_frame == TIMETABLE_S01_CARD_READ )
      {
        (*seq)++;
      }
    }
    break;
  case 2:  // �`�莆�̃e�L�X�g��\��
    {
      // �u�����h
      if( work->ev1 < 8 )
      {
        work->ev1++; 
        blend_m = TRUE;
      }
      else
      {
        // �e�L�X�g�͕\��
        work->vblank_req |= VBLANK_REQ_S01_TEXT_M_VISIBLE_ON;
        (*seq)++;
      }
    }
    break;
  case 3:  // �莆�̃e�L�X�g���m���ɕ\�������悤1�t���[���҂�
    {
      (*seq)++;
    }
    break;
  case 4:  // �v���C���[�̓��͑҂�
    {
      if(    ( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) )
          || ( GFL_UI_TP_GetTrg() ) )
      {
        if( GFL_UI_KEY_GetTrg() & (PAD_BUTTON_A|PAD_BUTTON_B) ) work->ktst = GFL_APP_KTST_KEY;
        else                                                    work->ktst = GFL_APP_KTST_TOUCH;
        
        PMSND_PlaySE( PSELSND_DECIDE );
        
        (*seq)++;
      }
    }
    break;
  case 5:  // �莆�̃e�L�X�g���\��
    {
      // �e�L�X�g�͔�\��
      work->vblank_req |= VBLANK_REQ_S01_TEXT_M_VISIBLE_OFF;

      // �u�����h
      if( work->ev1 != 0 )
      {
        work->ev1--; 
        blend_m = TRUE;
      }
      (*seq)++;
    }
    break;
  case 6:  // �`�莆�̃e�L�X�g�̃E�B���h�E�̔�\����4�t���[���i�ނ܂�
    {
      // �u�����h
      if( work->ev1 != 0 )
      {
        work->ev1--; 
        blend_m = TRUE;
      }

      if( work->ev1 == 4 )
      {
        (*seq)++;
      }
    }
    break;
  case 7:  // �`3D�̃A�j�����z���C�g�A�E�g�J�n��1�t���[���O�ɂȂ�܂�(�����̃t���[���� S01_FADE_OUT_WAIT �Ɗ֌W����)
    {
      // �u�����h
      if( work->ev1 != 0 )
      {
        work->ev1--;
        blend_m = TRUE;
      }

      Psel_ThreeS01Main( work );
      if( work->three_frame == TIMETABLE_S01_RIBBON_LOOSE )
      {
        PMSND_PlaySE( PSELSND_DEMO_RIBBON_LOOSE );
      }
      if( work->three_frame == TIMETABLE_S01_WO_START -1 )
      {
        // �V�[�P���X�t���[��
        work->sub_seq_frame = 0;
        return SEQ_S01_FADE_OUT;
      }
    }
    break;
  }

  // �u�����h
  if( blend_m )
  {
    G2_SetBlendAlpha(
        GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2,
        GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
        work->ev1, 16-work->ev1 );
    // 3D�̃A���t�@�����ꂢ�ɏo�����߁A��1�Ώۖʂ�BG0���w�肵�Ă���
    // ev1��ev2�͎g���Ȃ�  // TWL�v���O���~���O�}�j���A���u2D�ʂƂ̃��u�����f�B���O�v�Q�l
  }
 
  // �V�[�P���X�t���[��
  work->sub_seq_frame++;
  return SEQ_S01_MAIN;
}
static int Psel_S01FadeOut ( PSEL_WORK* work, int* seq )
{
  if( work->three_frame != TIMETABLE_S01_SCENE_END ) Psel_ThreeS01Main( work );  // 3D�̃A�j�����~�߂�

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
        // �V�[�P���X�t���[��
        work->sub_seq_frame = 0;
        return SEQ_S01_EXIT;
      }
    }
    break;
  }

  // �V�[�P���X�t���[��
  work->sub_seq_frame++;
  return SEQ_S01_FADE_OUT;
}
static int Psel_S01Exit    ( PSEL_WORK* work, int* seq )
{
  // �u�����h
  G2_BlendNone();
  G2S_BlendNone();

  Psel_TextS01Exit( work );
  Psel_BgS01SubExit( work );
  Psel_BgS01Exit( work );
  Psel_ThreeS01Exit( work );

  // �V�[�P���X�t���[��
  work->sub_seq_frame = 0;
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
  Psel_PalInit( work );

  // �e�L�X�g�ƃE�B���h�E�͔�\��
  work->vblank_req |= VBLANK_REQ_S02_TEXT_WIN_M_VISIBLE_OFF;
  work->vblank_req |= VBLANK_REQ_S02_TEXT_WIN_S_VISIBLE_OFF;

  // �u�����h
  G2_SetBlendAlpha(
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2/* | GX_BLEND_PLANEMASK_BG3*/,
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
      8, 8 );
  // 3D�̃A���t�@�����ꂢ�ɏo�����߁A��1�Ώۖʂ�BG0���w�肵�Ă���
  // ev1��ev2�͎g���Ȃ�  // TWL�v���O���~���O�}�j���A���u2D�ʂƂ̃��u�����f�B���O�v�Q�l

  G2S_SetBlendAlpha(
      GX_BLEND_PLANEMASK_BG2/* | GX_BLEND_PLANEMASK_BG3*/,
      GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
      8, 8 );

  // �V�[�P���X�t���[��
  work->sub_seq_frame = 0;
  return SEQ_S02_FADE_IN;
}
static int Psel_S02FadeIn  ( PSEL_WORK* work, int* seq )
{
  if( work->sub_seq_frame >= 16 ) Psel_ThreeS02Main( work );  // 3D�̃A�j���J�n�������x�点��

  //if( work->three_frame == TIMETABLE_S02_BOX_OPEN )
  if( work->sub_seq_frame == 15 )
  {
    PMSND_PlaySE( PSELSND_DEMO_BOX_OPEN );
  }

  Psel_ThreeS02OnlyMbSelectAnimeMain( work ); 
  Psel_PokeSetMain( work );
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
        // �V�[�P���X�t���[��
        work->sub_seq_frame = 0;
        return SEQ_S02_MAIN;
      }
    }
    break;
  }

  // �V�[�P���X�t���[��
  work->sub_seq_frame++;
  return SEQ_S02_FADE_IN;
}
static int Psel_S02Main    ( PSEL_WORK* work, int* seq )
{
  enum
  {
    S02_MAIN_SEQ_BOX_OPEN_STOP_WAIT,
    S02_MAIN_SEQ_EXPLAIN_INIT,
    S02_MAIN_SEQ_EXPLAIN_WAIT,
    S02_MAIN_SEQ_SELECT_INIT,
    S02_MAIN_SEQ_SELECT_WAIT,
    S02_MAIN_SEQ_TOUCH_CONFIRM_INIT,
    S02_MAIN_SEQ_TOUCH_CONFIRM_WAIT,
    S02_MAIN_SEQ_TOUCH_CONFIRM_WAIT_DECIDE,
    S02_MAIN_SEQ_ZOOM_IN_ANIME_INIT,
    S02_MAIN_SEQ_ZOOM_IN_ANIME_WAIT,
    S02_MAIN_SEQ_ANSWER_INIT,
    S02_MAIN_SEQ_ANSWER_WAIT,
    S02_MAIN_SEQ_ZOOM_OUT_ANIME_INIT,
    S02_MAIN_SEQ_ZOOM_OUT_ANIME_WAIT,
    S02_MAIN_SEQ_DECIDE_ANIME_INIT,
    S02_MAIN_SEQ_DECIDE_ANIME_WAIT,
    S02_MAIN_SEQ_END,
  };

  int hit = GFL_UI_TP_HIT_NONE;
  BOOL select_change = FALSE;  // �I�����Ă�����̂��ς������TRUE  // �|�P�����̃^�C�v�Ǝ푰���������̂Ɏg�p����ϐ�


#ifdef DEBUG_POS_SET_MODE_SUB
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    // �T�u��ʂ�OBJ�𓮂����Ĉʒu�𒲐�����
    GFL_CLACTPOS finger_pos;
    GFL_CLACT_WK_GetPos( work->finger_clwk, &finger_pos, CLSYS_DEFREND_MAIN );

    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    {
      OS_Printf( "finger_pos = ( %d, %d )\n", finger_pos.x, finger_pos.y );
    }
    else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
    {
      if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )    finger_pos.y -= 1;
      if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )  finger_pos.y += 1;
      if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )  finger_pos.x -= 1;
      if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT ) finger_pos.x += 1;
    }
    else if( GFL_UI_KEY_GetCont() & PAD_KEY_UP )    finger_pos.y -= 1;
    else if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )  finger_pos.y += 1;
    else if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )  finger_pos.x -= 1;
    else if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT ) finger_pos.x += 1;
    
    GFL_CLACT_WK_SetPos( work->finger_clwk, &finger_pos, CLSYS_DEFREND_MAIN );
    
    return SEQ_S02_MAIN;
  }
#endif
#ifdef DEBUG_POS_SET_MODE_MAIN
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L )
  {
    // ���C����ʂ�OBJ�𓮂����Ĉʒu�𒲐�����
    GFL_CLWK* small_clwk;
    GFL_CLWK* big_clwk;
    GFL_CLACTPOS pos;

    small_clwk = work->poke_set[TARGET_KUSA].clwk[POKE_SMALL];
    big_clwk = work->poke_set[TARGET_KUSA].clwk[POKE_BIG];

    GFL_CLACT_WK_GetPos( big_clwk, &pos, CLSYS_DEFREND_SUB );

    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    {
      OS_Printf( "main pos = ( %d, %d )\n", pos.x, pos.y );
    }
    else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
    {
      if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )    pos.y -= 1;
      if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )  pos.y += 1;
      if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )  pos.x -= 1;
      if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT ) pos.x += 1;
    }
    else if( GFL_UI_KEY_GetCont() & PAD_KEY_UP )    pos.y -= 1;
    else if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )  pos.y += 1;
    else if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )  pos.x -= 1;
    else if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT ) pos.x += 1;
    
    GFL_CLACT_WK_SetPos( small_clwk, &pos, CLSYS_DEFREND_SUB );
    GFL_CLACT_WK_SetPos( big_clwk, &pos, CLSYS_DEFREND_SUB );
    
    return SEQ_S02_MAIN;
  }
#endif


  Psel_ThreeS02OnlyMbSelectAnimeMain( work ); 
  Psel_PokeSetMain( work );
  Psel_PokeSetAlphaAnimeMain( work );
  Psel_PalMain( work );

  switch(*seq)
  {
  case S02_MAIN_SEQ_BOX_OPEN_STOP_WAIT:
    {
      Psel_ThreeS02Main( work );
      if( work->three_frame == TIMETABLE_S02_BOX_STOP )  // �`�{�b�N�X���J���܂�
      {
        *seq = S02_MAIN_SEQ_EXPLAIN_INIT;
      }
    }
    break;
  case S02_MAIN_SEQ_EXPLAIN_INIT:
    {
      // �e�L�X�g�ƃE�B���h�E�͕\��
      work->vblank_req |= VBLANK_REQ_S02_TEXT_WIN_M_VISIBLE_ON;
      Psel_TextExplainStreamStart( work, msg_t01r0102_ball_select_01 );
      
      *seq = S02_MAIN_SEQ_EXPLAIN_WAIT;

      // �{�b�N�X���J������|�P�����̃A���t�@�A�j���J�n
      Psel_PokeSetAlphaAnimeStart( work );
    }
    break;
  case S02_MAIN_SEQ_EXPLAIN_WAIT:
    {
      if(    Psel_TextExplainStreamWait( work )     // �X�g���[���e�L�X�g���S�ĕ\�������̂�҂�
          && Psel_PokeSetIsAlphaAnimeEnd( work ) )  // �|�P�����̃A���t�@�A�j������������̂�҂�
      {
        // �e�L�X�g�ƃE�B���h�E�͕\�������ςȂ�
        *seq = S02_MAIN_SEQ_SELECT_INIT;

        // �K���^�b�`����ŊJ�n
        work->ktst = GFL_APP_KTST_TOUCH;
      }
    }
    break;
  case S02_MAIN_SEQ_SELECT_INIT:
    {
      // �L�[���쒆
      if( work->ktst == GFL_APP_KTST_KEY )
      {
        if( work->select_target_poke == TARGET_NONE )
        {
          work->select_target_poke = TARGET_POKE_DEFAULT;
          select_change = TRUE;  // �ǂ���I�΂�Ă��Ȃ���Ԃ���ŏ��̂�I��ł����ԂɂȂ����̂�TRUE�ɂ������A�O���ǂ���I�΂�Ă��Ȃ���ԂȂ̂ŉ���������Ă��Ȃ��͂�������ATRUE�ɂ��Ȃ��Ă����͂Ȃ�
        }
        Psel_FingerDrawEnable( work, TRUE );
        Psel_FingerUpdatePos( work );
        Psel_ThreeS02OnlyMbSelectAnimeStart( work, work->select_target_poke );
        Psel_PokeSetSelectStart( work, work->select_target_poke );
        Psel_InsideBallPalAnimeStart( work, (work->select_target_poke!=TARGET_NONE)?(work->select_target_poke):(INSIDE_BALL_NONE) );
      }
      // �^�b�`���쒆
      else
      {
        Psel_FingerDrawEnable( work, FALSE );

        // select_target_poke�͎w�w���J�[�\���\���p�Ɉʒu�������Ă��邩������Ȃ����A�^�b�`�Ȃ̂łǂ���I�΂�Ă��Ȃ���ԂŊJ�n���邱�Ƃɂ���
        Psel_ThreeS02OnlyMbSelectAnimeStart( work, TARGET_NONE );
        Psel_PokeSetSelectStart( work, TARGET_NONE );
        Psel_InsideBallPalAnimeStart( work, INSIDE_BALL_NONE );
        select_change = TRUE;  // �ǂ���I�΂�Ă��Ȃ���ԂȂ̂ŁA�|�P�����̃^�C�v�Ǝ푰���͏����Ă���
      }

      *seq = S02_MAIN_SEQ_SELECT_WAIT;
    }
    break;
  case S02_MAIN_SEQ_SELECT_WAIT:
    {
      // �L�[���쒆
      if( work->ktst == GFL_APP_KTST_KEY )
      {
        if( GFL_UI_KEY_GetTrg() )  // �L�[����̂܂�
        {
          BOOL move = FALSE;
          if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
          {
            PMSND_PlaySE( PSELSND_DECIDE );
            Psel_FingerDrawEnable( work, FALSE );  // ���̃V�[�P���X�֍s���̂ŁA�w�w���J�[�\���͏����Ă���
            
            *seq = S02_MAIN_SEQ_ZOOM_IN_ANIME_INIT;
          }
          else if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
          {
            if( work->select_target_poke > TARGET_POKE_START )
            {
              work->select_target_poke--;
              move = TRUE;
            }
          }
          else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
          {
            if( work->select_target_poke < TARGET_POKE_END )
            {
              work->select_target_poke++;
              move = TRUE;
            }
          }
          if( move )
          {
            PMSND_PlaySE( PSELSND_MOVE );
            Psel_FingerUpdatePos( work );
            Psel_ThreeS02OnlyMbSelectAnimeStart( work, work->select_target_poke );
            Psel_PokeSetSelectStart( work, work->select_target_poke );
            Psel_InsideBallPalAnimeStart( work, (work->select_target_poke!=TARGET_NONE)?(work->select_target_poke):(INSIDE_BALL_NONE) );
            select_change = TRUE;
          }
        }
        else if( GFL_UI_TP_GetTrg() )  // �^�b�`����ɐ؂�ւ�(��\������)
        {
          hit = GFL_UI_TP_HitTrg(target_tp_hittbl);
          if( TARGET_POKE_START<=hit && hit<=TARGET_POKE_END )
          {
            if( work->select_target_poke != hit )  // �I�����Ă�����̂��ύX���ꂽ�Ƃ�
            {
              select_change = TRUE;
            } 
            PMSND_PlaySystemSE( PSELSND_MOVE );  // �I�����Ă�����̂��ύX����悤�����܂����A�^�b�`����ɐ؂�ւ�莟�̃V�[�P���X�֐i�ނ̂ŁA����炷
            Psel_FingerDrawEnable( work, FALSE );
            work->select_target_poke = hit;
            Psel_ThreeS02OnlyMbSelectAnimeStart( work, work->select_target_poke );
            Psel_PokeSetSelectStart( work, work->select_target_poke );
            Psel_InsideBallPalAnimeStart( work, (work->select_target_poke!=TARGET_NONE)?(work->select_target_poke):(INSIDE_BALL_NONE) );
            work->ktst = GFL_APP_KTST_TOUCH;

            *seq = S02_MAIN_SEQ_TOUCH_CONFIRM_INIT;
          }
        }
      }
      // �^�b�`���쒆
      else
      {
        if( GFL_UI_KEY_GetTrg() )  // �L�[����ɐ؂�ւ�
        {
          work->ktst = GFL_APP_KTST_KEY;
          
          *seq = S02_MAIN_SEQ_SELECT_INIT;
        }
        else if( GFL_UI_TP_GetTrg() )  // �^�b�`����̂܂�
        {
          hit = GFL_UI_TP_HitTrg(target_tp_hittbl);
          if( TARGET_POKE_START<=hit && hit<=TARGET_POKE_END )
          {
            PMSND_PlaySystemSE( PSELSND_MOVE );
            work->select_target_poke = hit;
            Psel_ThreeS02OnlyMbSelectAnimeStart( work, work->select_target_poke );
            Psel_PokeSetSelectStart( work, work->select_target_poke );
            Psel_InsideBallPalAnimeStart( work, (work->select_target_poke!=TARGET_NONE)?(work->select_target_poke):(INSIDE_BALL_NONE) );
            select_change = TRUE;

            *seq = S02_MAIN_SEQ_TOUCH_CONFIRM_INIT;
          }
        }
      }
    }
    break;
  case S02_MAIN_SEQ_TOUCH_CONFIRM_INIT:
    {
      Psel_AppTaskmenuWinInit( work );
      // app_taskmenu_win�𐶐������Ƃ��̓]���́Aapp_taskmenu_win�̊֐�������Ă����

      *seq = S02_MAIN_SEQ_TOUCH_CONFIRM_WAIT;
    }
    break;
  case S02_MAIN_SEQ_TOUCH_CONFIRM_WAIT:
    {
      Psel_AppTaskmenuWinMain( work );
      if( GFL_UI_KEY_GetTrg() )  // �L�[����ɐ؂�ւ�
      {
        work->ktst = GFL_APP_KTST_KEY;
        
        if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
        {
          PMSND_PlaySystemSE( PSELSND_DECIDE );
          Psel_AppTaskmenuWinDecideStart( work );

          *seq = S02_MAIN_SEQ_TOUCH_CONFIRM_WAIT_DECIDE;
        }
        else
        {
          Psel_AppTaskmenuWinExit( work );
          // app_taskmenu_win���N���A�����̂ŁA�]�����āA�N���A��Ԃ������ڂɂ����f������
	        GFL_BG_LoadScreenV_Req( BG_FRAME_M_TEXT );

          *seq = S02_MAIN_SEQ_SELECT_INIT;
        }
      }
      else if( GFL_UI_TP_GetTrg() )  // �^�b�`����̂܂�
      {
        hit = GFL_UI_TP_HitTrg(target_tp_hittbl);
        if( hit == TARGET_BUTTON )
        {
          PMSND_PlaySystemSE( PSELSND_DECIDE );
          Psel_AppTaskmenuWinDecideStart( work );
          
          *seq = S02_MAIN_SEQ_TOUCH_CONFIRM_WAIT_DECIDE;
        }
        else if( TARGET_POKE_START<=hit && hit<=TARGET_POKE_END )
        {
          if( work->select_target_poke != hit )  // �I��ł�����̂��ς������
          {
            PMSND_PlaySystemSE( PSELSND_MOVE );
            work->select_target_poke = hit;
            Psel_ThreeS02OnlyMbSelectAnimeStart( work, work->select_target_poke );
            Psel_PokeSetSelectStart( work, work->select_target_poke );
            Psel_InsideBallPalAnimeStart( work, (work->select_target_poke!=TARGET_NONE)?(work->select_target_poke):(INSIDE_BALL_NONE) );
            select_change = TRUE;
          }
        }
      }
    }
    break;
  case S02_MAIN_SEQ_TOUCH_CONFIRM_WAIT_DECIDE:
    {
      Psel_AppTaskmenuWinMain( work );
      if( Psel_AppTaskmenuWinDecideIsEnd( work ) )
      {
        Psel_AppTaskmenuWinExit( work );
        // app_taskmenu_win���N���A�����̂ŁA�]�����āA�N���A��Ԃ������ڂɂ����f������
       GFL_BG_LoadScreenV_Req( BG_FRAME_M_TEXT );

        *seq = S02_MAIN_SEQ_ZOOM_IN_ANIME_INIT;
      }
    }
    break;
  case S02_MAIN_SEQ_ZOOM_IN_ANIME_INIT:
    {
      // �e�L�X�g�ƃE�B���h�E�͕\�������ςȂ��Ȃ̂ŁA�e�L�X�g�̂ݍX�V�����
      Psel_TextExplainStreamStart( work, msg_t01r0102_ball_select_02 );

      // �Y�[���C��
      Psel_ThreeS02MbZoomAnimeStart( work, work->select_target_poke, FALSE );

/*
���ꏊ�ړ�
      {
        // �|�P������
        u32 formno = 0;  // 0<=formno<POKETOOL_GetPersonalParam( monsno, 0, POKEPER_ID_form_max )
        PMV_PlayVoice( poke_monsno[work->select_target_poke], formno );
      }
*/

      *seq = S02_MAIN_SEQ_ZOOM_IN_ANIME_WAIT;
    }
    break;
  case S02_MAIN_SEQ_ZOOM_IN_ANIME_WAIT:
    {
      // �Y�[��
      Psel_ThreeS02MbZoomAnimeMain( work );
      
      if( Psel_TextExplainStreamWait( work ) )
      {
        // �Y�[��
        if( Psel_ThreeS02MbZoomAnimeIsEnd( work ) )
        {
          *seq = S02_MAIN_SEQ_ANSWER_INIT;
        }
      }
    }
    break;
  case S02_MAIN_SEQ_ANSWER_INIT:
    {
      // APP_TASKMENU���J���O�ɐݒ肵�Ă���
      // �^�b�`or�L�[
      GFL_UI_SetTouchOrKey( work->ktst );

      Psel_AppTaskmenuInit( work );

      *seq = S02_MAIN_SEQ_ANSWER_WAIT;
    }
    break;
  case S02_MAIN_SEQ_ANSWER_WAIT:
    {
      Psel_AppTaskmenuMain( work );
      if( Psel_AppTaskmenuIsEnd( work ) )
      {
        BOOL yes = Psel_AppTaskmenuIsYes( work );
        Psel_AppTaskmenuExit( work );

        // APP_TASKMENU�������ɐݒ肵�Ă���
        // �^�b�`or�L�[
        work->ktst = GFL_UI_CheckTouchOrKey();
     
        if( yes )
        {
          // �e�L�X�g�ƃE�B���h�E�͔�\��
          work->vblank_req |= VBLANK_REQ_S02_TEXT_WIN_M_VISIBLE_OFF;
          Psel_TextExplainClear( work );

          *seq = S02_MAIN_SEQ_DECIDE_ANIME_INIT;
        }
        else
        {
          // �e�L�X�g�ƃE�B���h�E�͕\�������ςȂ��Ȃ̂ŁA�e�L�X�g�̂ݍX�V�����
          Psel_TextExplainStreamStart( work, msg_t01r0102_ball_select_01 );

          *seq = S02_MAIN_SEQ_ZOOM_OUT_ANIME_INIT;
        }
      }
    }
    break;
  case S02_MAIN_SEQ_ZOOM_OUT_ANIME_INIT:
    {
      // �Y�[���A�E�g
      Psel_ThreeS02MbZoomAnimeStart( work, work->select_target_poke, TRUE );
      
      *seq = S02_MAIN_SEQ_ZOOM_OUT_ANIME_WAIT;
    }
    break;
  case S02_MAIN_SEQ_ZOOM_OUT_ANIME_WAIT:
    {
      // �Y�[��
      Psel_ThreeS02MbZoomAnimeMain( work );
      if( Psel_ThreeS02MbZoomAnimeIsEnd( work ) )
      {
        *seq = S02_MAIN_SEQ_SELECT_INIT;
      } 
    }
    break;
  case S02_MAIN_SEQ_DECIDE_ANIME_INIT:
    {
      // ����A�j��
      Psel_ThreeS02MbDecideAnimeStart( work, work->select_target_poke );
      PMSND_PlaySE( PSELSND_DEMO_ZOOM_UP );
      
      *seq = S02_MAIN_SEQ_DECIDE_ANIME_WAIT;
    }
    break;
  case S02_MAIN_SEQ_DECIDE_ANIME_WAIT:  // �`3D�̃A�j�����z���C�g�A�E�g�J�n��2�t���[���O�ɂȂ�܂�(�����̃t���[���� S02_FADE_OUT_WAIT �Ɗ֌W����)
    {
      // ����A�j��
      Psel_ThreeS02MbDecideAnimeMain( work );
      if( work->three_mb_anime_frame == timetable_mb[work->three_mb_anime_target][TIMETABLE_MB_WO_START] -2 )
      {
        *seq = S02_MAIN_SEQ_END;
      }
    }
    break;
  case S02_MAIN_SEQ_END:
    {
      // ����A�j��
      Psel_ThreeS02MbDecideAnimeMain( work );

      // �V�[�P���X�t���[��
      work->sub_seq_frame = 0;
      return SEQ_S02_FADE_OUT;
    }
    break;
  }

  // �|�P�����̃^�C�v�Ǝ푰��������or����
  if( select_change )  // ����
  {
    // �e�L�X�g�ƃE�B���h�E�͔�\��
    work->vblank_req |= VBLANK_REQ_S02_TEXT_WIN_S_VISIBLE_OFF;
    Psel_TextPokeInfoClear( work );
  }
  else if( work->poke_info_print )  // ����
  {
    // �e�L�X�g�ƃE�B���h�E�͕\��
    work->vblank_req |= VBLANK_REQ_S02_TEXT_WIN_S_VISIBLE_ON;
    Psel_TextPokeInfoPrintStart( work, work->select_target_poke );

    {
      // �|�P������
      u32 formno = 0;  // 0<=formno<POKETOOL_GetPersonalParam( monsno, 0, POKEPER_ID_form_max )
      PMV_PlayVoice( poke_monsno[work->select_target_poke], formno );
    }
  }
  work->poke_info_print = FALSE;

  // �V�[�P���X�t���[��
  work->sub_seq_frame++;
  return SEQ_S02_MAIN;
}
static int Psel_S02FadeOut ( PSEL_WORK* work, int* seq )
{
  // ����A�j��
  Psel_ThreeS02MbDecideAnimeMain( work );
  
  Psel_ThreeS02OnlyMbSelectAnimeMain( work ); 
  Psel_PokeSetMain( work );
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
        // �V�[�P���X�t���[��
        work->sub_seq_frame = 0;
        return SEQ_S02_EXIT;
      }
    }
    break;
  }

  // �V�[�P���X�t���[��
  work->sub_seq_frame++;
  return SEQ_S02_FADE_OUT;
}
static int Psel_S02Exit    ( PSEL_WORK* work, int* seq )
{
  // �u�����h
  G2_BlendNone();
  G2S_BlendNone();

  Psel_PalExit( work );
  Psel_InsideBallExit( work );
  Psel_BgS02Exit( work );
  Psel_FingerExit( work );
  Psel_PokeSetExit( work );
  Psel_ThreeS02Exit( work );

  // �V�[�P���X�t���[��
  work->sub_seq_frame = 0;
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
    PRINTSYS_QUE_Clear( work->print_que );
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

