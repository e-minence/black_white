//============================================================================
/**
 *  @file   shinka_demo_view.c
 *  @brief  �i���f���̉��o
 *  @author Koji Kawada
 *  @data   2010.01.19
 *  @note   
 *
 *  ���W���[�����FSHINKADEMO_VIEW
 */
//============================================================================
//#define DEBUG_CODE


#define DEF_SPEED_UP_A  // ���ꂪ��`����Ă���Ƃ��A�X�s�[�h�A�b�v����Ă���
#define DEF_SPEED_UP_B  // ���ꂪ��`����Ă���Ƃ��A�X�s�[�h�A�b�v����Ă���(MTX_RotY43)(���܂��X�s�[�h�A�b�v�ł��Ă��炸�������Ēx������)
//C�͒�`���Ȃ��ق�������#define DEF_SPEED_UP_C  // ���ꂪ��`����Ă���Ƃ��A�X�s�[�h�A�b�v����Ă���(VEC_Normalize)(���܂��X�s�[�h�A�b�v�ł��Ă��炸�������Ēx������)
#define DEF_SPEED_UP_D  // ���ꂪ��`����Ă���Ƃ��A�X�s�[�h�A�b�v����Ă���(FX_InvSqrt)(���܂��X�s�[�h�A�b�v�ł��Ă��炸�������Ēx������)


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/mcss.h"
#include "system/mcss_tool.h"

#include "sound/pm_wb_voice.h"  // wb�ł�pm_voice�ł͂Ȃ���������g��
#include "sound/wb_sound_data.sadl"
#include "sound/pm_sndsys.h"

#include "shinka_demo_view.h"

// �p�[�e�B�N��
#include "arc_def.h"
#include "shinka_demo_particle.naix"


//=============================================================================
/**
 *  �萔��`
 */
//=============================================================================
// �X�e�b�v
typedef enum
{
  // EVO
  STEP_EVO_CRY_START,                                     // ���J�n�҂�������
  STEP_EVO_CRY,                                           // ����
  STEP_EVO_CHANGE_START,                                  // �i���J�n�҂������i��

  STEP_EVO_CHANGE_OPENING_COLOR_TO_WHITE_START,           // �i����  // ��������
  STEP_EVO_CHANGE_OPENING_COLOR_TO_WHITE,                 // �i����  // �����Ȃ蒆
  STEP_EVO_CHANGE_OPENING_BEFORE_REPLACE_WAIT,            // �i����  // ���΂��҂�
  STEP_EVO_CHANGE_OPENING_REPLACE_MCSS_WITH_INDEPENDENT,  // �i����  // MCSS������INDEPENDENT���o��������
  STEP_EVO_CHANGE_OPENING_AFTER_REPLACE_WAIT,             // �i����  // ���΂��҂�
  STEP_EVO_CHANGE_OPENING_WHITE_TO_COLOR_START,           // �i����  // �F�t���ɖ߂�
  STEP_EVO_CHANGE_OPENING_WHITE_TO_COLOR,                 // �i����  // �F�t���ɖ߂蒆

  STEP_EVO_CHANGE_TRANSFORM,                              // �i����  // �ό`
  STEP_EVO_CHANGE_TRANSFORM_FINISH,                       // �i����  // �ό`���������̂ŁA���΂炭���G�������ė��߂�
  STEP_EVO_CHANGE_CANCEL,                                 // �i����  // �L�����Z��

  STEP_EVO_CHANGE_ENDING_BEFORE_REPLACE_WAIT,             // �i����  // ���΂��҂�
  STEP_EVO_CHANGE_ENDING_REPLACE_INDEPENDENT_WITH_MCSS,   // �i����  // INDEPENDENT������MCSS���o��������
  STEP_EVO_CHANGE_ENDING_AFTER_REPLACE_WAIT,              // �i����  // ���΂��҂�
  STEP_EVO_CHANGE_ENDING_WHITE_TO_COLOR_START,            // �i����  // �F�t���ɖ߂�
  STEP_EVO_CHANGE_ENDING_WHITE_TO_COLOR,                  // �i����  // �F�t���ɖ߂蒆
  
  STEP_EVO_CHANGE_CRY_START_WAIT,                         // �i����  // ���J�n�O�̑҂�����
  STEP_EVO_CHANGE_CRY_START,                              // �i����  // ���J�n�҂���
  STEP_EVO_CHANGE_CRY,                                    // �i����  // ����
  STEP_EVO_END,                                           // �I����

  // AFTER
  STEP_AFTER,                                             // �i���ォ��X�^�[�g
}
STEP;

// MCSS�|�P����
typedef enum
{
  POKE_BEFORE,
  POKE_AFTER,
  POKE_MAX,
}
POKE;

#define SCALE_MAX            (16.0f)
#define SCALE_MIN            ( 4.0f)
#define POKE_SIZE_MAX        (96.0f)

// �p�[�e�B�N���Ή�
#define POKE_TEX_ADRS_FOR_PARTICLE (0x30000)


// �|�P������X,Y���W
#define POKE_X_HIDE    (FX_F32_TO_FX32(256.0f))
#define POKE_X_CENTER  (FX_F32_TO_FX32(0.0f))//(FX_F32_TO_FX32(-1.0f))//(FX_F32_TO_FX32(-3.0f))
#define POKE_Y         (FX_F32_TO_FX32(-18.0f))//(FX_F32_TO_FX32(-16.0f))

#define POKE_Y_MONSNO_564 (FX_F32_TO_FX32(-27.0f))
#define POKE_Y_MONSNO_565 (FX_F32_TO_FX32(-39.0f))

#ifdef BUGFIX_BTS7721_100712
  #define POKE_Y_MONSNO_YUNGERAA (FX_F32_TO_FX32(-17.9f))
#endif  // BUGFIX_BTS7721_100712

// �|�P������Y���W����
#define POKE_Y_ADJUST (0.33f)  // ���̒l���|����


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// MCSS�|�P����
//=====================================
typedef struct
{
  MCSS_WORK*           wk;
}
POKE_SET;


//-------------------------------------
/// ����|���S���|�P����
//=====================================
#define	POKEGRA_LZ	// �|�P�O�����k�L����`

#define TEXSIZ_S  (128)  // dot
#define TEXSIZ_T  (128)  // dot
#define TEXVRAMSIZ  (TEXSIZ_S/8 * TEXSIZ_T/8 * 0x20)  // chrNum_x * chrNum_y * chrSiz

#define	INDEPENDENT_POKE_TEX_ADRS	(0x26000)  // mcss�����̃A�h���X�ɂ���ƃ_���݂����B
#define	INDEPENDENT_POKE_TEX_SIZE	(0x2000)   // 1�L����32�o�C�g��16�L����x16�L��������(128�h�b�gx128�h�b�g)
#define	INDEPENDENT_POKE_PAL_ADRS	(0x960)    // mcss�����̃A�h���X�ɂ���ƃ_���݂����B
#define	INDEPENDENT_POKE_PAL_SIZE	(0x0020)

// ���������邩
#define INDEPENDENT_PANEL_NUM_X    (18)
#define INDEPENDENT_PANEL_NUM_Y    (18)
//#define INDEPENDENT_PANEL_NUM_X    (4)  // �p�l���̂��傤�ǌp���ڂ̉ӏ����G�������Ȃ邱�Ƃ����������B
//#define INDEPENDENT_PANEL_NUM_Y    (4)  // ���āA�ǂ��΍􂷂邩�B
//#define INDEPENDENT_PANEL_NUM_X    (28)  // �㉺���E��1�s�N�Z�����傫�ȃp�l���ɂ���ƁA�p�l��1�����傫���Ȃ��Ă��܂��̂ŁA
//#define INDEPENDENT_PANEL_NUM_Y    (28)  // ���̑傫���ƍ��킹��ɂ͂��ꂭ�炢�������˂΂Ȃ�Ȃ����A�����������������Ȃ�B

// �����Ӂ� INDEPENDENT_PANEL_NUM_Y��ύX������IndependentOneMain����panel_h��tex_h���ύX���邱��!
// �����Ӂ� INDEPENDENT_PANEL_NUM_Y��ύX������IndependentOneMain����independent_one_pixel���ύX���邱��!


// �S�������킹��1���ƌ����Ƃ��̃T�C�Y
//#define INDEPENDENT_PANEL_TOTAL_W  (36)  // �̗p
//#define INDEPENDENT_PANEL_TOTAL_H  (36)  // �̗p
#define INDEPENDENT_PANEL_TOTAL_W  (36.39702342662f)//(36.397f)  // �v�Z����1
#define INDEPENDENT_PANEL_TOTAL_H  (36.39702342662f)//(36.397f)  // �v�Z����1
//#define INDEPENDENT_PANEL_TOTAL_W  (96.0f)  // �v�Z����2
//#define INDEPENDENT_PANEL_TOTAL_H  (96.0f)  // �v�Z����2
//#define INDEPENDENT_PANEL_TOTAL_W  (96.0f)  // ���ˉe�v�Z����1
//#define INDEPENDENT_PANEL_TOTAL_H  (96.0f)  // ���ˉe�v�Z����1
//#define INDEPENDENT_PANEL_TOTAL_W  //(37.4f)//(ii38.0f)//(36.8f)//(36.2f)//(ii35.8f)//(35)//(37)  // �e�X�g
//#define INDEPENDENT_PANEL_TOTAL_H  //(37.4f)//(ii38.0f)//(36.8f)//(36.2f)//(ii35.8f)//(35)//(37)  // �e�X�g

// �S�������킹��1���ƌ����Ƃ��̍ŏ��ő�̃e�N�X�`�����W
#define INDEPENDENT_PANEL_TOTAL_MIN_S  ( 16)
#define INDEPENDENT_PANEL_TOTAL_MAX_S  (112)
#define INDEPENDENT_PANEL_TOTAL_MIN_T  ( 16)
#define INDEPENDENT_PANEL_TOTAL_MAX_T  (112)

// �����Ӂ� INDEPENDENT_PANEL_TOTAL_MIN_T, INDEPENDENT_PANEL_TOTAL_MAX_T��ύX������IndependentOneMain����independent_one_pixel��panel_pixel���ύX���邱��!


// �|���S��ID
#define INDEPENDENT_BEFORE_POLYGON_ID  (60)
#define INDEPENDENT_AFTER_POLYGON_ID   (61)
#define INDEPENDENT_ONE_POLYGON_ID     (59)

typedef enum
{
  // �i������
  INDEPENDENT_STATE_EVO_START,
  INDEPENDENT_STATE_EVO_DEMO,
  INDEPENDENT_STATE_EVO_END,

  // �i�����I�������
  INDEPENDENT_STATE_AFTER,
}
INDEPENDENT_STATE;

typedef enum
{
  INDEPENDENT_PAL_STATE_COLOR,                  // �F�t��
  INDEPENDENT_PAL_STATE_START_COLOR_TO_WHITE,   // �F�t�����甒�ւ̕ω����J�n����̂�҂��Ă���
  INDEPENDENT_PAL_STATE_COLOR_TO_WHITE,         // �F�t�����甒�֕ω���
  INDEPENDENT_PAL_STATE_WHITE,                  // ��
  INDEPENDENT_PAL_STATE_START_WHITE_TO_COLOR,   // ������F�t���ւ̕ω����J�n����̂�҂��Ă���
  INDEPENDENT_PAL_STATE_WHITE_TO_COLOR,         // ������F�t���֕ω���
}
INDEPENDENT_PAL_STATE;

typedef enum
{
  INDEPENDENT_SPIRAL_STATE_BEFORE_START,  // 1�������o���Ă��Ȃ�
  INDEPENDENT_SPIRAL_STATE_START,         // 1�ȏ�ʏ퓮���łȂ��������̂��̂�����
  INDEPENDENT_SPIRAL_STATE_STEADY,        // ���蓮����
  INDEPENDENT_SPIRAL_STATE_END,           // 1�ȏ�ʏ퓮���łȂ��������̂��̂�����
  INDEPENDENT_SPIRAL_STATE_AFTER_END,     // 1�������Ă��Ȃ�
}
INDEPENDENT_SPIRAL_STATE;

typedef enum
{
  INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_BEFORE_START,  // �����o���Ă��Ȃ�
  INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_START,         // �ʏ퓮���łȂ�������
  INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_STEADY,        // ���蓮����
  INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_END,           // �ʏ퓮���łȂ�������
  INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_AFTER_END,     // �����Ă��Ȃ�
}
INDEPENDENT_SPIRAL_INDIVIDUAL_STATE;

#define INDEPENDENT_SPIRAL_ZX_START_SPEED (1)   // INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_START��Ԃ�ZX�����̃X�s�[�h
#define INDEPENDENT_SPIRAL_Y_START_SPEED  (1)   // INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_START��Ԃ�Y�����̃X�s�[�h
#define INDEPENDENT_SPIRAL_ZX_END_SPEED   (1)   // INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_END��Ԃ�ZX�����̃X�s�[�h
#define INDEPENDENT_SPIRAL_Y_END_SPEED    (1)   // INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_END��Ԃ�Y�����̃X�s�[�h


// ���O�Ɍv�Z�ł�����̂͌v�Z���Ă�����
#define INDEPENDENT_SPIRAL_ZX_START_SPEED_SQ_E_FX32 ((fx32) 0x000011acL)  // ( FX32_CONST( INDEPENDENT_SPIRAL_ZX_START_SPEED * INDEPENDENT_SPIRAL_ZX_START_SPEED ) +INDEPENDENT_SPIRAL_E_FX32 )
#define INDEPENDENT_SPIRAL_Y_START_SPEED_SQ_E_FX32  ((fx32) 0x000011acL)  // ( FX32_CONST( INDEPENDENT_SPIRAL_Y_START_SPEED * INDEPENDENT_SPIRAL_Y_START_SPEED ) +INDEPENDENT_SPIRAL_E_FX32 )

#define INDEPENDENT_SPIRAL_Y_START_SPEED_FX32  ((fx32) 0x00001000L)  // ( FX32_CONST( INDEPENDENT_SPIRAL_Y_START_SPEED ) )

#define INDEPENDENT_SPIRAL_ZX_END_SPEED_SQ_FX32  ((fx32) 0x00001000L)  // ( FX32_CONST( INDEPENDENT_SPIRAL_ZX_END_SPEED * INDEPENDENT_SPIRAL_ZX_END_SPEED ) )
#define INDEPENDENT_SPIRAL_Y_END_SPEED_SQ_FX32   ((fx32) 0x00001000L)  // ( FX32_CONST( INDEPENDENT_SPIRAL_Y_END_SPEED * INDEPENDENT_SPIRAL_Y_END_SPEED ) )

#define INDEPENDENT_SPIRAL_Y_END_SPEED_FX32  ((fx32) 0x00001000L)  // ( FX32_CONST( INDEPENDENT_SPIRAL_Y_END_SPEED ) )

#define INDEPENDENT_SPIRAL_ZX_END_SPEED_SQ_E_FX32 ((fx32) 0x000011acL)  // ( FX32_CONST( INDEPENDENT_SPIRAL_ZX_END_SPEED * INDEPENDENT_SPIRAL_ZX_END_SPEED ) +INDEPENDENT_SPIRAL_E_FX32 )
#define INDEPENDENT_SPIRAL_Y_END_SPEED_SQ_E_FX32  ((fx32) 0x000011acL)  // ( FX32_CONST( INDEPENDENT_SPIRAL_Y_END_SPEED * INDEPENDENT_SPIRAL_Y_END_SPEED ) +INDEPENDENT_SPIRAL_E_FX32 )
// ���O�Ɍv�Z�ł�����̂͌v�Z���Ă�����


#define INDEPENDENT_SPIRAL_ZX_FIRST_VEL   (1)  // ����
#define INDEPENDENT_SPIRAL_Y_FIRST_VEL    (1)

#define INDEPENDENT_SPIRAL_PREV_RATE      (1)   // �O�̓������ǂꂭ�炢�̊����c����
#define INDEPENDENT_SPIRAL_CURR_RATE      (9)   // ����̓������ǂꂭ�炢�̊����o����

#ifdef DEF_SPEED_UP_A
  #define INDEPENDENT_SPIRAL_PREV_RATE_FX32 ((fx32) 0x00000199L)  // ����؂�Ă��Ȃ��̂ŁA
  #define INDEPENDENT_SPIRAL_CURR_RATE_FX32 ((fx32) 0x00000e00L)  // �����Ă�FX32_ONE�ɂ͂Ȃ�܂���B
#endif

#define INDEPENDENT_SPIRAL_E_FX32         (FX32_SIN6)  // ������

typedef struct
{
  int      polygon_id;      // �|���S��ID
  fx32     s0, s1, t0, t1;  // �e�N�X�`�����W
  u8       alpha;           // 0<= <=31
  VecFx16  vtx[4];          // ���_���W
  u32      count;           // �o�߃t���[��

  // �ŏ�����эŌ�̈ʒu
  VecFx32  start_end_pos;
  // ����
  VecFx32  first_vel;
  // �����]�ꏊ
  int      steady_theta_idx;  // 0<= <2PI�̃C���f�b�N�X
  fx32     steady_r;          // ���a
  fx32     steady_pos_y;      // ����
  VecFx32  steady_pos;        // steady_theta_idx, steady_r, steady_pos_y���狁�߂��l(������v�Z����̂��C���Ȃ̂ł��炩���ߋ��߂Ă���)

  // ���� 
  INDEPENDENT_SPIRAL_INDIVIDUAL_STATE  state;
  VecFx32                              pos;

  // �O��
  VecFx32                              pos_prev;
}
INDEPENDENT_PANEL_WORK;

typedef struct
{
  // �]���摜�W�J�p
  NNSG2dCharacterData* chr;
  NNSG2dPaletteData*   pal;
  void* chr_buf;
  void* pal_buf;
  GFL_BMP_DATA* bmp_data;

  u32 tex_adr;
  u32 pal_adr;

  // �S�̂̒l
  VecFx32  pos;

  INDEPENDENT_PAL_STATE   pal_state;
  u16      pal_curr[16];    // �p���b�g���ݒl
  u16      pal_color;       // �p���b�g�ύX��̐F(0x7fff�Ƃ�)
  s16      pal_rate_start;  // �J�n�l
  s16      pal_rate_end;    // �I���l
  s16      pal_wait;        // �p���b�g�҂��t���[��(0=���t���[��pal_speed�����ύX�����B1=xoxo...�B2=xxoxxo...�B)
  s16      pal_speed;       // �p���b�g�ύX�X�s�[�h�B1��̕ύX�łǂꂾ��pal_rate��i�߂邩�Bpal_rate_start��pal_rate_end�̒l�̑傫���ɂ���Đ��������܂�B0�̂Ƃ��ύX���N���Ȃ��̂ŏI���B(-31<= <=31)
  s16      pal_rate;        // ���X�̐F��pal_color�̔䗦���ݒl(0<= <=31)�B0=���X�̐F100%�A31=pal_color100%�B
  s16      pal_wait_count;  // pal_wait�̃J�E���g

//  u16      change_no;     // POKE_BEFORE��POKE_AFTER�̓���ւ��ς݂�panel_wk�̃C���f�b�N�X(�����̃C���f�b�N�X�������ւ���)
//                          // (y=change_no/INDEPENDENT_PANEL_NUM_X, x=change_no%INDEPENDENT_PANEL_NUM_X)

  s16      change_no_down_x;  // POKE_BEFORE��POKE_AFTER�̓���ւ��ς݂�panel_wk�̃C���f�b�N�X(�����̃C���f�b�N�X�������ւ���)
  s16      change_no_down_y;  // ��: 0<=x<max_x, 0<=y<max_y  (max_x=8, max_y=8)
  s16      change_no_up_x;    // (down_x, down_y)=(3, 3)����X�^�[�g���A(2, 3),(1,3),(0,3),(7,2),(6,2),...,(0,0)�Ɛi��ł����B�܂܂Ȃ�(-1,-1)�ɒB������I��
  s16      change_no_up_y;    // (up_x, up_y)=(4,3)����X�^�[�g���A(5,3),(6,3),(7,3),(0,4),(1,4),...,(7,7)�Ɛi��ł����B�܂܂Ȃ�(max_x,max_y)�ɒB������I��
  
  INDEPENDENT_SPIRAL_STATE  spi_state;      // �S�̗̂����̏��
  int                       spi_theta_idx;  // 0<= <2PI�̃C���f�b�N�X �őS�̂𗆐��ŉ񂷂���
  int                       spi_theta_idx_add;  // spi_theta_idx�̑�����
  u32                       spi_count;

  // �p�l���X
  INDEPENDENT_PANEL_WORK  panel_wk[INDEPENDENT_PANEL_NUM_Y][INDEPENDENT_PANEL_NUM_X];

  // y����ڈȍ~�͔�\��
  u8 not_disp_y;  // 0�̂Ƃ��S����\���AINDEPENDENT_PANEL_NUM_Y�̂Ƃ��S���\��
}
INDEPENDENT_POKE_WORK;

typedef struct
{
  // �]���摜�W�J�p
  u32 tex_adr;  // INDEPENDENT_POKE_WORK���ǂݍ��񂾂��̂�q�؂��Ă��邾��
  u32 pal_adr;

  // �S�̂̒l
  VecFx32  pos;
  VecFx32  scale;                              // ( INDEPENDENT_PANEL_TOTAL_W, INDEPENDENT_PANEL_TOTAL_H, 1 )�ɌŒ�
  int      polygon_id;      // �|���S��ID
  fx32     s0, s1, t0, t1;  // �e�N�X�`�����W
  u8       alpha;           // 0<= <=31
  VecFx16  vtx[4];          // ���_���W        // -0.5<= <=0.5�ɂ��Ă���(�傫��1�ɂ��Ă���)

  // y����ڈȍ~�͕\��
  u8 not_disp_y;  // 0�̂Ƃ��S���\���AINDEPENDENT_PANEL_NUM_Y�̂Ƃ��S����\��  // ����ɍ��킹��vtx��s0,s1,t0,t1��ϓ�������
}
INDEPENDENT_ONE_WORK;

typedef struct
{
  INDEPENDENT_STATE       state;
  GFL_G3D_CAMERA*         camera;             // INDEPENDENT��p�J����
  INDEPENDENT_POKE_WORK*  poke_wk[POKE_MAX];  // NULL�̂Ƃ��͂Ȃ�
  INDEPENDENT_ONE_WORK*   one_wk;             // NULL�̂Ƃ��͂Ȃ�
}
INDEPENDENT_POKE_MANAGER;

// �|�P������X,Y,Z���W
#define INDEPENDENT_POKE_X_HIDE    (FX32_CONST(256))
#define INDEPENDENT_POKE_X_CENTER  (FX32_CONST(0))
#define INDEPENDENT_POKE_Y         (-FX32_HALF)  // �ʒu�����������Ă���̂ŁA�J�����̐^���ʂ�INDEPENDENT�p�l��������킯�ł͂Ȃ��B�r���{�[�h�̂悤�ɃJ���������Ɍ����Ă����Ȃ��B1�p�l�����ł͂Ȃ��S�̂��J���������Ɍ����鏈�������ꂽ�ق����������낤���H
#define INDEPENDENT_POKE_Z         (FX32_CONST(0))


//-------------------------------------
/// ���[�N
//=====================================
struct _SHINKADEMO_VIEW_WORK
{
  // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B
  HEAPID                   heap_id;
  const POKEMON_PARAM*     pp;
  
  // SHINKADEMO_VIEW_Init�̈���
  SHINKADEMO_VIEW_LAUNCH   launch;
  u16                      after_monsno;

  // �|�P���� 
  POKEMON_PARAM*           after_pp;  // �����ňꎞ�I�ɐ�������
  
  // �X�e�b�v
  STEP                     step;
  u32                      wait_count;

  // �t���O
  BOOL                     b_cry_start;                // �i���O�̖��X�^�[�g���Ă����Ƃ���TRUE
  BOOL                     b_cry_end;                  // �i���O�̖����������Ă�����TRUE
  BOOL                     b_change_start;             // �i���X�^�[�g���Ă����Ƃ���TRUE
  BOOL                     b_change_to_white;          // ��ʂ𔒂���΂��ė~�����Ȃ�����TRUE
  BOOL                     b_change_replace_start;     // INDEPENDENT��MCSS�ɓ���ւ��Ă����Ƃ���TRUE
  BOOL                     b_change_from_white_start;  // �|�P�����𔒂���߂��Ă����Ƃ���TRUE
  BOOL                     b_change_end;               // �i�����������Ă�����TRUE
  BOOL                     b_change_bgm_shinka_start;    // BGM SHINKA�Ȃ��J�n���Ă��悢��
  BOOL                     b_change_bgm_shinka_push;     // BGM SHINKA�Ȃ�push���Ă��悢��
  BOOL                     b_change_cancel;  // �i���L�����Z�����Ă�����TRUE

  // �g��k������ւ����o
  f32                      scale;
  u8                       disp_poke;

  // MCSS
  MCSS_SYS_WORK*           mcss_sys_wk;
  POKE_SET                 poke_set[POKE_MAX];

  // ����|���S���|�P����
  INDEPENDENT_POKE_MANAGER*  independent_poke_mgr;

  // MCSS��INDEPENDENT�ǂ����`�悷�邩
  BOOL      is_mcss_draw;  // MCSS��`�悷��Ƃ�TRUE
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
static void ShinkaDemo_View_McssInit( SHINKADEMO_VIEW_WORK* work );
static void ShinkaDemo_View_McssExit( SHINKADEMO_VIEW_WORK* work );

static void ShinkaDemo_View_PokeInit( SHINKADEMO_VIEW_WORK* work );
static void ShinkaDemo_View_PokeExit( SHINKADEMO_VIEW_WORK* work );

static u8 NotDispPoke( u8 disp_poke );

static void ShinkaDemo_View_AdjustPokePos( SHINKADEMO_VIEW_WORK* work );

static void ShinkaDemo_View_PokeSetPosX( SHINKADEMO_VIEW_WORK* work, fx32 pos_x );

//-------------------------------------
/// ����|���S���|�P����
//=====================================
static void IndependentPokeManagerInit( SHINKADEMO_VIEW_WORK* work );
static void IndependentPokeManagerExit( SHINKADEMO_VIEW_WORK* work );
static void IndependentPokeManagerMain( SHINKADEMO_VIEW_WORK* work );
static void IndependentPokeManagerDraw( SHINKADEMO_VIEW_WORK* work );

static void IndependentPokeManagerStart( SHINKADEMO_VIEW_WORK* work );
static BOOL IndependentPokeManagerIsEnableCancel( SHINKADEMO_VIEW_WORK* work );
static void IndependentPokeManagerCancel( SHINKADEMO_VIEW_WORK* work );

static void IndependentPokeManagerSetPosXYZ( SHINKADEMO_VIEW_WORK* work, fx32 pos_x, fx32 pos_y, fx32 pos_z );
static void IndependentPokeSetPosXYZ( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id, fx32 pos_x, fx32 pos_y, fx32 pos_z );

static void IndependentPokeManagerPalStartWhiteToColor( SHINKADEMO_VIEW_WORK* work );
static BOOL IndependentPokeManagerPalIsColor( SHINKADEMO_VIEW_WORK* work );
static BOOL IndependentPokeManagerPalIsStartWhiteToColor( SHINKADEMO_VIEW_WORK* work );

static INDEPENDENT_POKE_WORK* IndependentPokeInit(
    int mons_no, int form_no, int sex, int rare, int dir, BOOL egg,
    u32 tex_adr, u32 pal_adr, HEAPID heap_id );
static void IndependentPokeExit( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );

static void IndependentPokeInitEvo( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );
static void IndependentPokeInitAfter( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );

static void IndependentPokeMain( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );
static void IndependentPokeDraw( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );

static void IndependentPokeManagerMainSpiral( SHINKADEMO_VIEW_WORK* work );
static void IndependentPokeMainSpiral( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );

static void IndependentPokePalStart( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id,
    u16 pal_color, s16 pal_rate_start, s16 pal_rate_end, s16 pal_wait, s16 pal_speed );
static BOOL IndependentPokePalIsEnd( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );
static void IndependentPokePalUpdate( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );
static void IndependentPokePalTrans( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );

static BOOL IndependentPokePalIsColor( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );
static BOOL IndependentPokePalIsStartColorToWhite( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );
static void IndependentPokePalStartColorToWhite( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );
static BOOL IndependentPokePalIsWhite( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );
static BOOL IndependentPokePalIsStartWhiteToColor( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );
static void IndependentPokePalStartWhiteToColor( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );
static void IndependentPokePalMain( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id );

// INDEPENDENT_ONE
static INDEPENDENT_ONE_WORK* IndependentOneInit( u32 tex_adr, u32 pal_adr, HEAPID heap_id );
static void IndependentOneExit( INDEPENDENT_ONE_WORK* one_wk, HEAPID heap_id );
static void IndependentOneMain( INDEPENDENT_ONE_WORK* one_wk, HEAPID heap_id );
static void IndependentOneDraw( INDEPENDENT_ONE_WORK* one_wk, HEAPID heap_id );

static void IndependentOneSetPosXYZ( INDEPENDENT_ONE_WORK* one_wk, HEAPID heap_id, fx32 pos_x, fx32 pos_y, fx32 pos_z );


//=============================================================================
/**
*  �O�����J�֐���`
*/
//=============================================================================
//-----------------------------------------------------------------------------
/**
 *  @brief         ���������� 
 *
 *  @param[in,out] 
 *
 *  @retval        SHINKADEMO_VIEW_WORK
 */
//-----------------------------------------------------------------------------
SHINKADEMO_VIEW_WORK* SHINKADEMO_VIEW_Init(
                               HEAPID                   heap_id,
                               SHINKADEMO_VIEW_LAUNCH   launch,
                               const POKEMON_PARAM*     pp,
                               u16                      after_monsno
                             )
{
  SHINKADEMO_VIEW_WORK* work;

  // ���[�N
  {
    work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(SHINKADEMO_VIEW_WORK) );
  }

  // ����
  {
    work->heap_id      = heap_id;
    work->launch       = launch;
    work->pp           = pp;
    work->after_monsno = after_monsno;
  }
 
  // �|�P����
  {
    work->after_pp = NULL;

    if( work->launch == SHINKADEMO_VIEW_LAUNCH_EVO )
    {
      // �����ňꎞ�I�ɐ�������
      work->after_pp = GFL_HEAP_AllocClearMemory( work->heap_id, POKETOOL_GetWorkSize() );
      POKETOOL_CopyPPtoPP( (POKEMON_PARAM*)(work->pp), work->after_pp );
      PP_ChangeMonsNo( work->after_pp, work->after_monsno );  // �i��
    }
  }
  
  // �X�e�b�v
  {
    if( work->launch == SHINKADEMO_VIEW_LAUNCH_EVO )
      work->step         = STEP_EVO_CRY_START;
    else
      work->step         = STEP_AFTER;
    
    work->wait_count = 0;
  }

  // �t���O
  {
    work->b_cry_start                = FALSE;
    work->b_cry_end                  = FALSE; 
    work->b_change_start             = FALSE;
    work->b_change_end               = FALSE;
    work->b_change_bgm_shinka_start  = FALSE;
    work->b_change_bgm_shinka_push   = FALSE;
    work->b_change_cancel            = FALSE;
  }
  
  // �g��k������ւ����o
  {
    work->scale        = SCALE_MAX;
    
    if( work->launch == SHINKADEMO_VIEW_LAUNCH_EVO )
      work->disp_poke    = POKE_BEFORE;
    else
      work->disp_poke    = POKE_AFTER;
  }

  // MCSS
  {
    ShinkaDemo_View_McssInit( work );
    ShinkaDemo_View_PokeInit( work );
  }

  IndependentPokeManagerInit( work );

  // MCSS��INDEPENDENT�ǂ����`�悷�邩
  work->is_mcss_draw = TRUE;

  return work;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �I������ 
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_VIEW_Exit( SHINKADEMO_VIEW_WORK* work )
{
  IndependentPokeManagerExit( work );

  // MCSS
  {
    ShinkaDemo_View_PokeExit( work );
    ShinkaDemo_View_McssExit( work );
  }

  // �����ňꎞ�I�ɐ����������̂�j������
  {
    if( work->after_pp )
    {
      GFL_HEAP_FreeMemory( work->after_pp );
    }
  }

  // ���[�N
  {
    GFL_HEAP_FreeMemory( work );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �又�� 
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_VIEW_Main( SHINKADEMO_VIEW_WORK* work )
{
  switch(work->step)
  {
  // EVO
  case STEP_EVO_CRY_START:
    {
      if( work->b_cry_start )
      {
        // ����
        work->step = STEP_EVO_CRY;

        {
          u32 monsno  = PP_Get( work->pp, ID_PARA_monsno, NULL );
          u32 form_no = PP_Get( work->pp, ID_PARA_form_no, NULL );
          PMV_PlayVoice( monsno, form_no );
        }
      }
    }
    break;
  case STEP_EVO_CRY:
    {
      if( !PMV_CheckPlay() )
      {
        // ����
        work->step = STEP_EVO_CHANGE_START;
        
        work->b_cry_end = TRUE;
      }
    }
    break;
  case STEP_EVO_CHANGE_START:
    {
      if( work->b_change_start )
      {
        // ����
        work->step = STEP_EVO_CHANGE_OPENING_COLOR_TO_WHITE_START;
      }
    }
    break;

  case STEP_EVO_CHANGE_OPENING_COLOR_TO_WHITE_START:
    {
      // ����
      work->step = STEP_EVO_CHANGE_OPENING_COLOR_TO_WHITE;

      MCSS_SetPaletteFade( work->poke_set[work->disp_poke].wk, 0, 16, 0, 0x7fff );
      MCSS_SetPaletteFade( work->poke_set[NotDispPoke(work->disp_poke)].wk, 16, 16, 0, 0x7fff );

      MCSS_SetAnmStopFlag( work->poke_set[work->disp_poke].wk );
    }
    break;
  case STEP_EVO_CHANGE_OPENING_COLOR_TO_WHITE:
    {
      if( !MCSS_CheckExecutePaletteFade( work->poke_set[work->disp_poke].wk ) )
      {
        // ����
        work->step = STEP_EVO_CHANGE_OPENING_BEFORE_REPLACE_WAIT;
      }
    }
    break;
  case STEP_EVO_CHANGE_OPENING_BEFORE_REPLACE_WAIT:
    {
      if( work->wait_count >= 8 )
      {
        // ����
        work->step = STEP_EVO_CHANGE_OPENING_REPLACE_MCSS_WITH_INDEPENDENT;
        work->wait_count = 0;
      }
      else
      {
        work->wait_count++;
      }
    }
    break;
  case STEP_EVO_CHANGE_OPENING_REPLACE_MCSS_WITH_INDEPENDENT:
    {
      // ����
      work->step = STEP_EVO_CHANGE_OPENING_WHITE_TO_COLOR_START;

      ShinkaDemo_View_PokeSetPosX( work, POKE_X_HIDE );
      IndependentPokeManagerSetPosXYZ( work, INDEPENDENT_POKE_X_CENTER, INDEPENDENT_POKE_Y, INDEPENDENT_POKE_Z );
      work->is_mcss_draw = FALSE;

      {
        NNSG2dMultiCellAnimation* anim_ctrl = MCSS_GetAnimCtrl( work->poke_set[work->disp_poke].wk );
        NNS_G2dRestartMCAnimation( anim_ctrl );
      }
    }
    break;
  case STEP_EVO_CHANGE_OPENING_AFTER_REPLACE_WAIT:
    {
      if( work->wait_count >= 1 )
      {
        // ����
        work->step = STEP_EVO_CHANGE_OPENING_WHITE_TO_COLOR_START;
        work->wait_count = 0;
      }
      else
      {
        work->wait_count++;
      }
    }
    break;
  case STEP_EVO_CHANGE_OPENING_WHITE_TO_COLOR_START:
    {
      // ����
      work->step = STEP_EVO_CHANGE_OPENING_WHITE_TO_COLOR;

      IndependentPokeManagerPalStartWhiteToColor( work );
    }
    break;
  case STEP_EVO_CHANGE_OPENING_WHITE_TO_COLOR:
    {
      if( IndependentPokeManagerPalIsColor( work ) )
      {
        // ����
        work->step = STEP_EVO_CHANGE_TRANSFORM;
        
        work->b_change_bgm_shinka_start = TRUE;
        IndependentPokeManagerStart( work );
      }
    }
    break;

  case STEP_EVO_CHANGE_TRANSFORM:
    {
      if( IndependentPokeManagerPalIsStartWhiteToColor( work ) )
      {
        // ����
        work->step = STEP_EVO_CHANGE_TRANSFORM_FINISH;
      }
    }
    break;
  case STEP_EVO_CHANGE_TRANSFORM_FINISH:
    {
      if( work->wait_count >= 30 )
      {
        // ����
        work->step = STEP_EVO_CHANGE_ENDING_BEFORE_REPLACE_WAIT;
        work->wait_count = 0;
        
        work->b_change_bgm_shinka_push = TRUE;
        work->b_change_to_white = TRUE;
      }
      else
      {
        work->wait_count++;
      }
    }
    break;
  case STEP_EVO_CHANGE_CANCEL:
    {
      if( IndependentPokeManagerPalIsStartWhiteToColor( work ) )
      {
        // ����
        work->step = STEP_EVO_CHANGE_ENDING_BEFORE_REPLACE_WAIT;

        work->b_change_bgm_shinka_push = TRUE;
        work->b_change_to_white = TRUE;
      }
    }
    break;

  case STEP_EVO_CHANGE_ENDING_BEFORE_REPLACE_WAIT:
    {
      if( work->b_change_replace_start )
      {
        // ����
        work->step = STEP_EVO_CHANGE_ENDING_REPLACE_INDEPENDENT_WITH_MCSS;
      }
    }
    break;
  case STEP_EVO_CHANGE_ENDING_REPLACE_INDEPENDENT_WITH_MCSS:
    {
      // ����
      work->step = STEP_EVO_CHANGE_ENDING_AFTER_REPLACE_WAIT;

      if(work->b_change_cancel)
      {
        // �ς���Ă��Ȃ��̂ŁA�������Ȃ�
        work->wait_count = 0;
      }
      else
      {
        // disp_poke��NotDispPoke�����ւ���
        work->disp_poke = NotDispPoke( work->disp_poke );
        // �Vdisp_poke��\������
        MCSS_ResetVanishFlag( work->poke_set[work->disp_poke].wk );
        // �VNotDispPoke���\���ɂ���
        MCSS_SetVanishFlag( work->poke_set[NotDispPoke(work->disp_poke)].wk );
        
        work->wait_count = 30;
      }

      ShinkaDemo_View_PokeSetPosX( work, POKE_X_CENTER );
      IndependentPokeManagerSetPosXYZ( work, INDEPENDENT_POKE_X_HIDE, INDEPENDENT_POKE_Y, INDEPENDENT_POKE_Z );
      work->is_mcss_draw = TRUE;
    }
    break;
  case STEP_EVO_CHANGE_ENDING_AFTER_REPLACE_WAIT:
    {
      if( work->b_change_from_white_start )
      {
        // ����
        work->step = STEP_EVO_CHANGE_ENDING_WHITE_TO_COLOR_START;
      }
    }
    break;
  case STEP_EVO_CHANGE_ENDING_WHITE_TO_COLOR_START:
    {
      // ����
      work->step = STEP_EVO_CHANGE_ENDING_WHITE_TO_COLOR;
    
      MCSS_SetPaletteFade( work->poke_set[work->disp_poke].wk, 16, 0, 1, 0x7fff );
    }
    break;
  case STEP_EVO_CHANGE_ENDING_WHITE_TO_COLOR:
    {
      if( !MCSS_CheckExecutePaletteFade( work->poke_set[work->disp_poke].wk ) )
      {
        // ����
        work->step = STEP_EVO_CHANGE_CRY_START_WAIT;

        MCSS_ResetAnmStopFlag( work->poke_set[work->disp_poke].wk );
      }
    }
    break;

  case STEP_EVO_CHANGE_CRY_START_WAIT:
    {
      if( work->wait_count == 0 )  // �i�������Ƃ��Ɛi�����Ȃ������Ƃ��ő҂����Ԃ��قȂ�̂ŁA�����̓}�C�i�X���Ă������Ƃɂ����B
      {
        // ����
        work->step = STEP_EVO_CHANGE_CRY_START;
        work->wait_count = 0;
      }
      else
      {
        work->wait_count--;
      }
    }
    break;
  case STEP_EVO_CHANGE_CRY_START:
    {
      {
        // ����
        work->step = STEP_EVO_CHANGE_CRY;

        {
          const POKEMON_PARAM* curr_pp = (work->b_change_cancel)?(work->pp):(work->after_pp);
          u32 monsno  = PP_Get( curr_pp, ID_PARA_monsno, NULL );
          u32 form_no = PP_Get( curr_pp, ID_PARA_form_no, NULL );
          PMV_PlayVoice( monsno, form_no );
        }
      }
    }
    break;
  case STEP_EVO_CHANGE_CRY:
    {
      //if( work->wait_count >= 100 )  // ME��炷�u�Ԃɉ�ʂ��ł܂�̂ŁA�G�t�F�N�g���o�Ă���Ƃ��ꂪ�ڗ����Ă��܂��B
      {                             // ������G�t�F�N�g���o�I���̂�҂��Ƃɂ����B
        if( !PMV_CheckPlay() )
        {
          // ����
          work->step = STEP_EVO_END;
          work->wait_count = 0;
          
          work->b_change_end = TRUE;
        }
      }
      //else  // BGM�̕������[�h�𗘗p���Ă݂邱�Ƃɂ����̂ŃR�����g�A�E�g
      //{
      //  work->wait_count++;
      //}
    }
    break;
  case STEP_EVO_END:
    {
      // �������Ȃ�
    }
    break;

  // AFTER
  case STEP_AFTER:
    {
      // �������Ȃ�
    }
    break;
  }

#ifndef DEF_SPEED_UP_A
  MCSS_Main( work->mcss_sys_wk );
#else
  //if( STEP_EVO_CHANGE_OPENING_WHITE_TO_COLOR_START <= work->step && work->step <= STEP_EVO_CHANGE_ENDING_BEFORE_REPLACE_WAIT )
  if( !(work->is_mcss_draw) )
  {
    // �������Ȃ�
  }
  else
  {
    MCSS_Main( work->mcss_sys_wk );
  }
#endif

  IndependentPokeManagerMain( work );
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �`�揈�� 
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 *
 *  @note          GRAPHIC_3D_StartDraw��GRAPHIC_3D_EndDraw�̊ԂŌĂԂ���
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_VIEW_Draw( SHINKADEMO_VIEW_WORK* work )
{
#if 1
  if( work->is_mcss_draw )
  {
    MCSS_Draw( work->mcss_sys_wk );
  }
  else
  {
    IndependentPokeManagerDraw( work );
  }
#else
  ������ɂ���ƁAIndependentPokeManagerDraw�̊G���w�i�ɂ��ė���Ă��܂�
  IndependentPokeManagerDraw( work );

  MCSS_Draw( work->mcss_sys_wk );

  ����
  {
    // TwlSDK/build/demos/gx/UnitTours/3D_Pol_Tex16_Plett/src/main.c
    // ���Q�l�ɂ����B
	  G3_MtxMode( GX_MTXMODE_TEXTURE );
    G3_Identity();
    // Use an identity matrix for the texture matrix for simplicity
	  G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
  }
  �������΂��Ȃ������B�����Ă͂��Ȃ��B
#endif
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �i���O�̖��X�^�[�g
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_VIEW_CryStart( SHINKADEMO_VIEW_WORK* work )
{
  work->b_cry_start = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �i���O�̖����������Ă��邩
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �������Ă���Ƃ�TRUE��Ԃ�
 */
//-----------------------------------------------------------------------------
BOOL SHINKADEMO_VIEW_CryIsEnd( SHINKADEMO_VIEW_WORK* work )
{
  return work->b_cry_end;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �i���X�^�[�g
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_VIEW_ChangeStart( SHINKADEMO_VIEW_WORK* work )
{
  work->b_change_start = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         ��ʂ𔒂���΂��ė~������
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        ��ʂ𔒂���΂��ė~�����Ƃ�TRUE��Ԃ�
 */
//-----------------------------------------------------------------------------
BOOL SHINKADEMO_VIEW_ChangeIsToWhite( SHINKADEMO_VIEW_WORK* work )
{
  return work->b_change_to_white;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         INDEPENDENT��MCSS�ɓ���ւ��X�^�[�g
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_VIEW_ChangeReplaceStart( SHINKADEMO_VIEW_WORK* work )
{
  work->b_change_replace_start = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �|�P�����𔒂���߂��̂��X�^�[�g
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_VIEW_ChangeFromWhiteStart( SHINKADEMO_VIEW_WORK* work )
{
  work->b_change_from_white_start = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �i�����������Ă��邩
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �������Ă���Ƃ�TRUE��Ԃ�
 */
//-----------------------------------------------------------------------------
BOOL SHINKADEMO_VIEW_ChangeIsEnd( SHINKADEMO_VIEW_WORK* work )
{
  return work->b_change_end;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         BGM SHINKA�Ȃ��J�n���Ă��悢��
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �J�n���Ă��悢�Ƃ�TRUE��Ԃ�
 */
//-----------------------------------------------------------------------------
BOOL SHINKADEMO_VIEW_ChangeIsBgmShinkaStart( SHINKADEMO_VIEW_WORK* work )
{
  return work->b_change_bgm_shinka_start;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         BGM SHINKA�Ȃ�push���Ă��悢��
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        push���Ă��悢�Ƃ�TRUE��Ԃ�
 */
//-----------------------------------------------------------------------------
BOOL SHINKADEMO_VIEW_ChangeIsBgmShinkaPush( SHINKADEMO_VIEW_WORK* work )
{
  return work->b_change_bgm_shinka_push;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �i���L�����Z��
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        ����������TRUE��Ԃ��A�����ɐi���L�����Z���̃t���[�ɐ؂�ւ��
 */
//-----------------------------------------------------------------------------
BOOL SHINKADEMO_VIEW_ChangeCancel( SHINKADEMO_VIEW_WORK* work )
{
  if( IndependentPokeManagerIsEnableCancel(work) )
  {
    work->b_change_cancel = TRUE;
    work->step = STEP_EVO_CHANGE_CANCEL;
    IndependentPokeManagerCancel( work );
    return TRUE;
  }
  return FALSE;
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// MCSS�V�X�e������������
//=====================================
static void ShinkaDemo_View_McssInit( SHINKADEMO_VIEW_WORK* work )
{
  work->mcss_sys_wk = MCSS_Init( POKE_MAX, work->heap_id );
  MCSS_SetTextureTransAdrs( work->mcss_sys_wk, POKE_TEX_ADRS_FOR_PARTICLE );  // �p�[�e�B�N���ƈꏏ�Ɏg������
  MCSS_SetOrthoMode( work->mcss_sys_wk );
  //MCSS_ResetOrthoMode( work->mcss_sys_wk );
}
//-------------------------------------
/// MCSS�V�X�e���I������
//=====================================
static void ShinkaDemo_View_McssExit( SHINKADEMO_VIEW_WORK* work )
{
  MCSS_Exit( work->mcss_sys_wk );
}

//-------------------------------------
/// MCSS�|�P��������������
//=====================================
static fx32 ShinkaDemo_View_PokeGetY( const POKEMON_PARAM* pp );
static fx32 ShinkaDemo_View_PokeGetY( const POKEMON_PARAM* pp )
{
  u32  monsno = PP_Get( pp, ID_PARA_monsno, NULL );
  fx32 poke_y = POKE_Y;
  switch( monsno )
  {
  case MONSNO_564:  //�����v���[
    {
      poke_y = POKE_Y_MONSNO_564;
    }
    break;
  case MONSNO_565:  //�V�����f��
    {
      poke_y = POKE_Y_MONSNO_565;
    }
    break;
#ifdef BUGFIX_BTS7721_100712
  case MONSNO_YUNGERAA:
    {
      poke_y = POKE_Y_MONSNO_YUNGERAA;
    }
    break;
#endif  // BUGFIX_BTS7721_100712
  }
  return poke_y;
}

static void ShinkaDemo_View_PokeInit( SHINKADEMO_VIEW_WORK* work )
{
  VecFx32  scale;

  {
    u8 i;
    for(i=0; i<POKE_MAX; i++)
    {
      work->poke_set[i].wk        = NULL;        // NULL�ŏ�����
    }

    scale.x = FX_F32_TO_FX32(work->scale);
    scale.y = FX_F32_TO_FX32(work->scale);
    scale.z = FX32_ONE;
  }

  if( work->launch == SHINKADEMO_VIEW_LAUNCH_EVO )
  {
    u8 i;
    //MCSS_ADD_WORK add_wk;

    for(i=0; i<POKE_MAX; i++)
    {
      if(i == POKE_BEFORE)
      {
        //MCSS_TOOL_MakeMAWPP( work->pp, &add_wk, MCSS_DIR_FRONT );
        work->poke_set[i].wk = MCSS_TOOL_AddPokeMcss( work->mcss_sys_wk, work->pp, MCSS_DIR_FRONT,
                                                      POKE_X_CENTER, ShinkaDemo_View_PokeGetY( work->pp ), 0 );
      }
      else
      {
        //MCSS_TOOL_MakeMAWPP( work->after_pp, &add_wk, MCSS_DIR_FRONT );
        work->poke_set[i].wk = MCSS_TOOL_AddPokeMcss( work->mcss_sys_wk, work->after_pp, MCSS_DIR_FRONT,
                                                      POKE_X_CENTER, ShinkaDemo_View_PokeGetY( work->after_pp ), 0 );
      }
      //work->poke_set[i].wk = MCSS_Add( work->mcss_sys_wk, POKE_X_CENTER, POKE_Y, 0, &add_wk );
      MCSS_SetShadowVanishFlag( work->poke_set[i].wk, TRUE );  // �e�������Ă����Ȃ��ƁA�����ȓ_�Ƃ��ĉe���\������Ă��܂��B 
      MCSS_TOOL_SetAnmRestartCallback( work->poke_set[i].wk );  // 1���[�v������A�j���[�V�������Z�b�g���ĂԂ��߂̃R�[���o�b�N�Z�b�g
      MCSS_SetScale( work->poke_set[i].wk, &scale );

      if(i == POKE_BEFORE)
      {
        // �������Ȃ�
      }
      else
      {
/*
  alpha�͂��ꂢ�Ɍ����Ȃ��̂ł�߂Ă���(�������Ɍ����Ȃ����A�X�v���C�g���Ƃ̐؂�ڂ������邵(Z�ʒu�̑O��֌W�̂����H�A�J�����̂����H))
        MCSS_SetAlpha( work->poke_set[i].wk, 0 );
*/
        MCSS_SetVanishFlag( work->poke_set[i].wk );
        MCSS_SetAnmStopFlag( work->poke_set[i].wk );
      }
    } 
  }
  else
  {
    u8 i;
    //MCSS_ADD_WORK add_wk;

    i = POKE_AFTER;

    //MCSS_TOOL_MakeMAWPP( work->pp, &add_wk, MCSS_DIR_FRONT );
    //work->poke_set[i].wk = MCSS_Add( work->mcss_sys_wk, POKE_X_CENTER, POKE_Y, 0, &add_wk );
    work->poke_set[i].wk = MCSS_TOOL_AddPokeMcss( work->mcss_sys_wk, work->pp, MCSS_DIR_FRONT,
                                                  POKE_X_CENTER, ShinkaDemo_View_PokeGetY( work->pp ), 0 );
    MCSS_SetShadowVanishFlag( work->poke_set[i].wk, TRUE );  // �e�������Ă����Ȃ��ƁA�����ȓ_�Ƃ��ĉe���\������Ă��܂��B 
    MCSS_TOOL_SetAnmRestartCallback( work->poke_set[i].wk );  // 1���[�v������A�j���[�V�������Z�b�g���ĂԂ��߂̃R�[���o�b�N�Z�b�g
    MCSS_SetScale( work->poke_set[i].wk, &scale );
  }

  ShinkaDemo_View_AdjustPokePos(work);
}
//-------------------------------------
/// MCSS�|�P�����I������
//=====================================
static void ShinkaDemo_View_PokeExit( SHINKADEMO_VIEW_WORK* work )
{
  u8 i;
  for(i=0; i<POKE_MAX; i++)
  {
    if(work->poke_set[i].wk)
    {
      MCSS_SetVanishFlag( work->poke_set[i].wk );
      MCSS_Del( work->mcss_sys_wk, work->poke_set[i].wk );
    }
  }
}

//-------------------------------------
/// disp_poke�ł͂Ȃ��ق��̃|�P�����𓾂�
//=====================================
static u8 NotDispPoke( u8 disp_poke )
{
  if( disp_poke == POKE_BEFORE ) return POKE_AFTER;
  else                           return POKE_BEFORE;
}

//-------------------------------------
/// �|�P�����̈ʒu�𒲐�����
//=====================================
static void ShinkaDemo_View_AdjustPokePos( SHINKADEMO_VIEW_WORK* work )
{
#if 0
  u8 i;
  for(i=0; i<POKE_MAX; i++)
  {
    if( work->poke_set[i].wk != NULL )
    {
      f32 size_y = (f32)MCSS_GetSizeY( work->poke_set[i].wk );
      f32 ofs_y;
      VecFx32 ofs;
      size_y *= work->scale / SCALE_MAX;
      if( size_y > POKE_SIZE_MAX ) size_y = POKE_SIZE_MAX;
      ofs_y = ( POKE_SIZE_MAX - size_y ) / 2.0f * POKE_Y_ADJUST;
      ofs.x = 0;  ofs.y = FX_F32_TO_FX32(ofs_y);  ofs.z = 0;
      MCSS_SetOfsPosition( work->poke_set[i].wk, &ofs );
    }
  }
#else

  u8 i;
  for(i=0; i<POKE_MAX; i++)
  {
    if( work->poke_set[i].wk != NULL )
    {
      f32      size_y   = (f32)MCSS_GetSizeY( work->poke_set[i].wk );
      f32      offset_y = (f32)MCSS_GetOffsetY( work->poke_set[i].wk );  // �����Ă���Ƃ�-, ����ł���Ƃ�+
      f32      offset_x = (f32)MCSS_GetOffsetX( work->poke_set[i].wk );  // �E�ɂ���Ă���Ƃ�+, ���ɂ���Ă���Ƃ�-
      f32      ofs_position_y;
      f32      ofs_position_x;
      VecFx32  ofs_position;

      size_y   *= work->scale / SCALE_MAX;
      if( size_y > POKE_SIZE_MAX ) size_y = POKE_SIZE_MAX;
      
      offset_y *= work->scale / SCALE_MAX;
      offset_x *= work->scale / SCALE_MAX;

      ofs_position_y = ( ( POKE_SIZE_MAX - size_y ) / 2.0f + offset_y ) * POKE_Y_ADJUST;
      ofs_position_x = - offset_x * POKE_Y_ADJUST;
      
      ofs_position.x = FX_F32_TO_FX32(ofs_position_x);  ofs_position.y = FX_F32_TO_FX32(ofs_position_y);  ofs_position.z = 0;
      MCSS_SetOfsPosition( work->poke_set[i].wk, &ofs_position );
    }
  }

#endif
}

//-------------------------------------
/// �|�P������X�ʒu��ݒ肷��
//=====================================
static void ShinkaDemo_View_PokeSetPosX( SHINKADEMO_VIEW_WORK* work, fx32 pos_x )
{
  u8 i;
  for(i=0; i<POKE_MAX; i++)
  {
    if( work->poke_set[i].wk != NULL )
    {
      VecFx32 pos;
      MCSS_GetPosition( work->poke_set[i].wk, &pos );
      pos.x = pos_x;
      MCSS_SetPosition( work->poke_set[i].wk, &pos );
    }
  }
}


//-------------------------------------
/// ����|���S���|�P����
//=====================================
static void IndependentPokeManagerInit( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr;

  u32 tex_adr = INDEPENDENT_POKE_TEX_ADRS;
  u32 pal_adr = INDEPENDENT_POKE_PAL_ADRS;

  u8 i, j;

  independent_poke_mgr = GFL_HEAP_AllocClearMemory( work->heap_id, sizeof(INDEPENDENT_POKE_MANAGER) );
  independent_poke_mgr->poke_wk[POKE_BEFORE] = NULL;
  independent_poke_mgr->poke_wk[POKE_AFTER]  = NULL;
  independent_poke_mgr->one_wk               = NULL;

  if( work->launch == SHINKADEMO_VIEW_LAUNCH_EVO )
  {
    independent_poke_mgr->state = INDEPENDENT_STATE_EVO_START;
  }
  else
  {
    independent_poke_mgr->state = INDEPENDENT_STATE_AFTER;
  }

  {
    u32  monsno  = PP_Get( work->pp, ID_PARA_monsno, NULL );
    u32  form_no = PP_Get( work->pp, ID_PARA_form_no, NULL );
    u8   sex     = PP_GetSex( work->pp );
    BOOL rare    = PP_CheckRare( work->pp );

    independent_poke_mgr->poke_wk[POKE_BEFORE] = IndependentPokeInit(
        monsno, form_no, sex, rare, POKEGRA_DIR_FRONT, FALSE,
        tex_adr, pal_adr, work->heap_id );

    // POKE_BEFORE�����̏�����
    {
      INDEPENDENT_POKE_WORK* poke_wk = independent_poke_mgr->poke_wk[POKE_BEFORE];

      if( work->launch == SHINKADEMO_VIEW_LAUNCH_EVO )
      {
        IndependentPokeInitEvo( poke_wk, work->heap_id );
        poke_wk->pal_state =INDEPENDENT_PAL_STATE_WHITE;
        IndependentPokePalStart( poke_wk, work->heap_id,
            0x7fff, 31, 31, 0, 31 );
      }
      else
      {
        IndependentPokeInitAfter( poke_wk, work->heap_id );
      }
    }
  }

  tex_adr += INDEPENDENT_POKE_TEX_SIZE;
  pal_adr += INDEPENDENT_POKE_PAL_SIZE;

  if( work->launch == SHINKADEMO_VIEW_LAUNCH_EVO )
  {
    u32  monsno  = PP_Get( work->after_pp, ID_PARA_monsno, NULL );
    u32  form_no = PP_Get( work->after_pp, ID_PARA_form_no, NULL );
    u8   sex     = PP_GetSex( work->after_pp );
    BOOL rare    = PP_CheckRare( work->after_pp );

    independent_poke_mgr->poke_wk[POKE_AFTER] = IndependentPokeInit(
        monsno, form_no, sex, rare, POKEGRA_DIR_FRONT, FALSE,
        tex_adr, pal_adr, work->heap_id );

    // POKE_AFTER�����̏�����
    {
      INDEPENDENT_POKE_WORK* poke_wk = independent_poke_mgr->poke_wk[POKE_AFTER];

      IndependentPokeInitEvo( poke_wk, work->heap_id );
      poke_wk->pal_state =INDEPENDENT_PAL_STATE_WHITE;
      IndependentPokePalStart( poke_wk, work->heap_id,
          0x7fff, 31, 31, 0, 31 );

      {
        u8 i, j;
        for( j=0; j<INDEPENDENT_PANEL_NUM_Y; j++ )
        {
          for( i=0; i<INDEPENDENT_PANEL_NUM_X; i++ )
          {
            INDEPENDENT_PANEL_WORK* panel_wk = &(poke_wk->panel_wk[j][i]);
            panel_wk->alpha = 0;
          }
        }
      }

    }
  }

  // INDEPENDENT_ONE
  if( work->launch == SHINKADEMO_VIEW_LAUNCH_EVO )
  {
    independent_poke_mgr->one_wk = IndependentOneInit( INDEPENDENT_POKE_TEX_ADRS, INDEPENDENT_POKE_PAL_ADRS, work->heap_id );
  }

  work->independent_poke_mgr = independent_poke_mgr;
  
  {
    // ���̊֐��̒���work->independent_poke_mgr���g�p���Ă���̂ŁAwork->independent_poke_mgr�ւ̑���AIndependentPokeManagerSetPosXYZ�Ăяo���̏��ŁB
    IndependentPokeManagerSetPosXYZ( work, INDEPENDENT_POKE_X_HIDE, INDEPENDENT_POKE_Y, INDEPENDENT_POKE_Z );
  }

  // INDEPENDENT��p�J����
  {
    // �ˉe
    VecFx32 pos    = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 100.0f ) };
    VecFx32 up     = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 1.0f ), FX_F32_TO_FX32(   0.0f ) };
    VecFx32 target = { FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32( 0.0f ), FX_F32_TO_FX32(   0.0f ) };

/*
//�̗p
    independent_poke_mgr->camera = GFL_G3D_CAMERA_Create(
        GFL_G3D_PRJPERS, 
        FX_SinIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT ),
        FX_CosIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT ),
        defaultCameraAspect, 0,
        defaultCameraNear, defaultCameraFar, 0,
        &pos, &up, &target, work->heap_id );
//#define INDEPENDENT_PANEL_TOTAL_W  (36)
//#define INDEPENDENT_PANEL_TOTAL_H  (36)
*/
/*
//���ˉe�e�X�g
    independent_poke_mgr->camera = GFL_G3D_CAMERA_CreateOrtho(
			  //chiiFX32_CONST(96), -FX32_CONST(96), -FX32_CONST(128), FX32_CONST(128),
        //dekaFX32_CONST(6), -FX32_CONST(6), -FX32_CONST(8), FX32_CONST(8), 
			  //chiiFX32_CONST(48), -FX32_CONST(48), -FX32_CONST(64), FX32_CONST(64),
			  //dekaFX32_CONST(24), -FX32_CONST(24), -FX32_CONST(32), FX32_CONST(32),
			  //dekaFX32_CONST(27), -FX32_CONST(27), -FX32_CONST(36), FX32_CONST(36),
			  FX32_CONST(36), -FX32_CONST(36), -FX32_CONST(48), FX32_CONST(48),  // ���������������ǂ�����Ɛ�������
        defaultCameraNear, defaultCameraFar, 0,
        &pos, &up, &target, work->heap_id );
*/

//�v�Z����1
    independent_poke_mgr->camera = GFL_G3D_CAMERA_Create(
        GFL_G3D_PRJPERS, 
        FX_SinIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT ),
        FX_CosIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT ),
        defaultCameraAspect, 0,
        defaultCameraNear, defaultCameraFar, 0,
        &pos, &up, &target, work->heap_id );
    // ��ʂ̏cscreen_h���s���̂Ƃ�(�|�P�����̏cpoke_h���s��)
    // ��ʂ̏cscreen_h = 2 * defaultCameraNear * tan(defaultCameraFovy/2);
    // �|�P�����̏cpoke_h = screen_h * 96/192;
    // ���ۂɒl���������
    // screen_h = 2 * 1 * tan(40/2);  (40degree)
    // screen_h = 0.72794;
    // poke_h = 0.72794 * 96/192;
    // poke_h = 0.36397;
//#define INDEPENDENT_PANEL_TOTAL_W  (36.4f)
//#define INDEPENDENT_PANEL_TOTAL_H  (36.4f)

/*
//�v�Z����2
    independent_poke_mgr->camera = GFL_G3D_CAMERA_Create(
        GFL_G3D_PRJPERS, 
        FX_SinIdx( (int)( 87.66172f / 2.0f * 0x10000 / 360.0f ) ),
        FX_CosIdx( (int)( 87.66172f / 2.0f * 0x10000 / 360.0f ) ),
        defaultCameraAspect, 0,
        defaultCameraNear, defaultCameraFar, 0,
        &pos, &up, &target, work->heap_id );
    // ��ʂ̏cscreen_h�����܂��Ă���theta(defaultCameraFovy�ɑ�������Ƃ���̂���)���s���̂Ƃ�(�|�P�����̏cpoke_h�����܂��Ă���)
    // tan(theta/2) = screen_h / 2 / defaultCameraNear;
    // ���ۂɒl���������
    // screen_h = 1.92;
    // poke_h = 0.96;
    // tan(theta/2) = 1.92 / 2 / 1;
    // theta = 43.83086 * 2 = 87.66172;  (degree)
//#define INDEPENDENT_PANEL_TOTAL_W  (96.0f)
//#define INDEPENDENT_PANEL_TOTAL_H  (96.0f)
*/
/*
//���ˉe�v�Z����1
    independent_poke_mgr->camera = GFL_G3D_CAMERA_CreateOrtho(
			  FX32_CONST(96), -FX32_CONST(96), -FX32_CONST(128), FX32_CONST(128),
        defaultCameraNear, defaultCameraFar, 0,
        &pos, &up, &target, work->heap_id );
//#define INDEPENDENT_PANEL_TOTAL_W  (96.0f)
//#define INDEPENDENT_PANEL_TOTAL_H  (96.0f)
*/
  }
}
static void IndependentPokeManagerExit( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;

  u8 i;

  // INDEPENDENT��p�J����
  {
    GFL_G3D_CAMERA_Delete( independent_poke_mgr->camera );
  }

  // INDEPENDENT_ONE
  if( independent_poke_mgr->one_wk )
  {
    IndependentOneExit( independent_poke_mgr->one_wk, work->heap_id  );
  }

  for( i=0; i<POKE_MAX; i++ )
  {
    if( independent_poke_mgr->poke_wk[i] )
    {
      IndependentPokeExit( independent_poke_mgr->poke_wk[i], work->heap_id  );
    }
  }

  GFL_HEAP_FreeMemory( independent_poke_mgr );
}
static void IndependentPokeManagerMain( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;
  HEAPID                    heap_id = work->heap_id;

  switch( independent_poke_mgr->state )
  {
  case INDEPENDENT_STATE_EVO_START:
    {
      // �������Ȃ�
    }
    break;
  case INDEPENDENT_STATE_EVO_DEMO:
    {
      IndependentPokeManagerMainSpiral( work );
    }
    break;
  case INDEPENDENT_STATE_EVO_END:
    {
      // �������Ȃ�
    }
    break;
  case INDEPENDENT_STATE_AFTER:
    {
      // �������Ȃ�
    }
    break;
  }

  // �p���b�g�A�j��
  {
    u8 i;
    for( i=0; i<POKE_MAX; i++ )
    {
      if( independent_poke_mgr->poke_wk[i] )
      {
        INDEPENDENT_POKE_WORK* poke_wk = independent_poke_mgr->poke_wk[i];

        // �i���L�����Z��
        if( work->b_change_cancel )
        {
          if( IndependentPokePalIsWhite( poke_wk, heap_id ) )
          {
            poke_wk->pal_state = INDEPENDENT_PAL_STATE_START_WHITE_TO_COLOR;
          }
        }
        IndependentPokePalMain( poke_wk, heap_id );
      } 
    }
  }
}
static void IndependentPokeManagerDraw( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;

  u8 i;

  GFL_G3D_PROJECTION  projection;
  GFL_G3D_LOOKAT      lookAt;

  // INDEPENDENT��p�J�����ɐ؂�ւ���
  {
    // �ŏ��̓r���{�[�h���������̂���
    // ���S�ɋ󂫂��ł��Ă��܂����̂ŁA
    // �r���{�[�h�͂�����߂�
    // ��p�J������p���邱�Ƃɂ����B

    GFL_G3D_GetSystemProjection(&projection);
    GFL_G3D_GetSystemLookAt(&lookAt);

    {
      VecFx32 scale = { FX32_ONE, FX32_ONE, FX32_ONE };
      MtxFx33 rot;
      VecFx32 trans = { 0, 0, 0 };

      MTX_Identity33( &rot );

      NNS_G3dGlbSetBaseTrans( &trans );
      NNS_G3dGlbSetBaseRot( &rot );
      NNS_G3dGlbSetBaseScale( &scale );
    }

    GFL_G3D_CAMERA_Switching(independent_poke_mgr->camera);
    GFL_G3D_DRAW_SetLookAt();
    NNS_G3dGeFlushBuffer();
  }

  {
    // TwlSDK/build/demos/gx/UnitTours/3D_Pol_Tex16_Plett/src/main.c
    // ���Q�l�ɂ����B
	  G3_MtxMode( GX_MTXMODE_TEXTURE );
    G3_Identity();
    // Use an identity matrix for the texture matrix for simplicity
	  G3_MtxMode( GX_MTXMODE_POSITION_VECTOR );
  }

  {
    G3_MaterialColorDiffAmb(
							GX_RGB(31, 31, 31),
							GX_RGB(31, 31, 31),
              FALSE
        );

    G3_MaterialColorSpecEmi(GX_RGB(31, 31, 31), GX_RGB(31, 31, 31), FALSE);  // ���C�g�Ȃ��p�̐ݒ�emission GX_RGB(31, 31, 31)
  }

  for( i=0; i<POKE_MAX; i++ )
  {
    if( independent_poke_mgr->poke_wk[i] )
    {
      IndependentPokeDraw( independent_poke_mgr->poke_wk[i], work->heap_id );
    }
  }

  // INDEPENDENT_ONE
  {
    if( independent_poke_mgr->one_wk )
    {
      IndependentOneDraw( independent_poke_mgr->one_wk, work->heap_id );
    }
  }

  // INDEPENDENT��p�J��������߂�
  {
    GFL_G3D_SetSystemProjection(&projection);
    GFL_G3D_SetSystemLookAt(&lookAt);
    GFL_G3D_DRAW_SetLookAt();
    NNS_G3dGeFlushBuffer();
  }
}

static void IndependentPokeManagerStart( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;
  
  if( independent_poke_mgr->state == INDEPENDENT_STATE_EVO_START )
  {
    independent_poke_mgr->state = INDEPENDENT_STATE_EVO_DEMO;
  }
}
static BOOL IndependentPokeManagerIsEnableCancel( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;

  if(    independent_poke_mgr->state != INDEPENDENT_SPIRAL_STATE_BEFORE_START  // ���߂�
      && independent_poke_mgr->state != INDEPENDENT_SPIRAL_STATE_AFTER_END )   // �x�߂�
  {
    return TRUE;
  }
  return FALSE;
}
static void IndependentPokeManagerCancel( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;
 
  // �܂������Ȃ��Ă��Ȃ��Ȃ狭���I�ɔ�������
  {
    INDEPENDENT_POKE_WORK* poke_wk = independent_poke_mgr->poke_wk[POKE_BEFORE];

    if(    poke_wk->pal_state != INDEPENDENT_PAL_STATE_START_COLOR_TO_WHITE
        && poke_wk->pal_state != INDEPENDENT_PAL_STATE_COLOR_TO_WHITE
        && poke_wk->pal_state != INDEPENDENT_PAL_STATE_WHITE )
    {
      poke_wk->pal_state = INDEPENDENT_PAL_STATE_START_COLOR_TO_WHITE;
    }
  }
}

static void IndependentPokeManagerSetPosXYZ( SHINKADEMO_VIEW_WORK* work, fx32 pos_x, fx32 pos_y, fx32 pos_z )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;

  u8 i;
  for( i=0; i<POKE_MAX; i++ )
  {
    if( independent_poke_mgr->poke_wk[i] )
    {
      IndependentPokeSetPosXYZ( independent_poke_mgr->poke_wk[i], work->heap_id, pos_x, pos_y, pos_z );
    }
  }

  // INDEPENDENT_ONE
  {
    if( independent_poke_mgr->one_wk )
    {
      IndependentOneSetPosXYZ( independent_poke_mgr->one_wk, work->heap_id, pos_x, pos_y, pos_z );
    }
  }
}

static void IndependentPokeSetPosXYZ( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id, fx32 pos_x, fx32 pos_y, fx32 pos_z )
{
  poke_wk->pos.x = pos_x;
  poke_wk->pos.y = pos_y;
  poke_wk->pos.z = pos_z;
}

static void IndependentPokeManagerPalStartWhiteToColor( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;
  HEAPID                    heap_id = work->heap_id;

  u8 i;
  for( i=0; i<POKE_MAX; i++ )
  {
    if( independent_poke_mgr->poke_wk[i] )
    {
      INDEPENDENT_POKE_WORK* poke_wk = independent_poke_mgr->poke_wk[i];
      poke_wk->pal_state = INDEPENDENT_PAL_STATE_WHITE_TO_COLOR;
      IndependentPokePalStart( poke_wk, heap_id,
          0x7fff, 31, 0, 0, -1 );
    }
  }
}

static BOOL IndependentPokeManagerPalIsColor( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;
  HEAPID                    heap_id = work->heap_id;

  BOOL ret = TRUE;

  u8 i;
  for( i=0; i<POKE_MAX; i++ )
  {
    if( independent_poke_mgr->poke_wk[i] )
    {
      INDEPENDENT_POKE_WORK* poke_wk = independent_poke_mgr->poke_wk[i];

      if( !IndependentPokePalIsColor( poke_wk, heap_id ) )
      {
        ret = FALSE;
      }
    }
  }

  return ret;
}

static BOOL IndependentPokeManagerPalIsStartWhiteToColor( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;
  HEAPID                    heap_id = work->heap_id;

  BOOL ret = TRUE;

  u8 i;
  for( i=0; i<POKE_MAX; i++ )
  {
    if( independent_poke_mgr->poke_wk[i] )
    {
      INDEPENDENT_POKE_WORK* poke_wk = independent_poke_mgr->poke_wk[i];

      if( !IndependentPokePalIsStartWhiteToColor( poke_wk, heap_id ) )
      {
        ret = FALSE;
      }
    }
  }

  return ret;
}

static INDEPENDENT_POKE_WORK* IndependentPokeInit(
    int mons_no, int form_no, int sex, int rare, int dir, BOOL egg,
    u32 tex_adr, u32 pal_adr, HEAPID heap_id )
{
  INDEPENDENT_POKE_WORK* wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(INDEPENDENT_POKE_WORK) );
  
  wk->bmp_data = GFL_BMP_Create( TEXSIZ_S/8, TEXSIZ_T/8, GFL_BMP_16_COLOR, heap_id );

  wk->tex_adr = tex_adr;
  wk->pal_adr = pal_adr;

  // �|�P�����O���t�B�b�N�擾
  {
    u32 cgr, clr;
    // ���\�[�X�󂯎��
    cgr	= POKEGRA_GetCgrArcIndex( POKEGRA_GetArcID(), mons_no, form_no, sex, rare, dir, egg );
    clr = POKEGRA_GetPalArcIndex( POKEGRA_GetArcID(), mons_no, form_no, sex, rare, dir, egg );
    wk->chr = NULL;
    wk->pal = NULL;
    // ���\�[�X��OBJ�Ƃ��č���Ă���̂ŁALoadOBJ����Ȃ��Ɠǂݍ��߂Ȃ�
#ifdef	POKEGRA_LZ
    wk->chr_buf = GFL_ARC_UTIL_LoadOBJCharacter( POKEGRA_GetArcID(), cgr, TRUE, &wk->chr, heap_id );
#else	// POKEGRA_LZ
    wk->chr_buf = GFL_ARC_UTIL_LoadOBJCharacter( POKEGRA_GetArcID(), cgr, FALSE, &wk->chr, heap_id );
#endif	// POKEGRA_LZ
    wk->pal_buf = GFL_ARC_UTIL_LoadPalette( POKEGRA_GetArcID(), clr, &wk->pal, heap_id );
  }

  // 12x12chr�|�P�O����16x16chr�̐^�񒆂ɓ\��t����
  {
    u8	*src, *dst;
    int x, y;
    int	chrNum = 0;

    src = wk->chr->pRawData;
		dst = GFL_BMP_GetCharacterAdrs(wk->bmp_data);
    GFL_STD_MemClear32((void*)dst, TEXVRAMSIZ);

    dst += ( (2*16+2) * 0x20 );
    // �L�����f�[�^��8x8�A4x8�A8x4�A4x4�̏��Ԃ�1D�}�b�s���O����Ă���
    for (y=0; y<8; y++){
      for(x=0; x<8; x++){
        GFL_STD_MemCopy32((void*)(&src[chrNum*0x20]), (void*)(&dst[(y*16+x)*0x20]), 0x20);
        chrNum++;
      }
    }
    for (y=0; y<8; y++){
      for(x=8; x<12; x++){
        GFL_STD_MemCopy32((void*)(&src[chrNum*0x20]), (void*)(&dst[(y*16+x)*0x20]), 0x20);
        chrNum++;
      }
    }
    for (y=8; y<12; y++){
      for(x=0; x<8; x++){
        GFL_STD_MemCopy32((void*)(&src[chrNum*0x20]), (void*)(&dst[(y*16+x)*0x20]), 0x20);
        chrNum++;
      }
    }
    for (y=8; y<12; y++){
      for(x=8; x<12; x++){
        GFL_STD_MemCopy32((void*)(&src[chrNum*0x20]), (void*)(&dst[(y*16+x)*0x20]), 0x20);
        chrNum++;
      }
    }
  }

  // 16x16chr�t�H�[�}�b�g�f�[�^���a�l�o�f�[�^�ɕϊ�
  GFL_BMP_DataConv_to_BMPformat(wk->bmp_data, FALSE, heap_id);
  
  // �]��
  {
    BOOL rc;
    void*	src;
		u32		dst;
    src = (void*)GFL_BMP_GetCharacterAdrs(wk->bmp_data);
		dst = tex_adr;
    rc = NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_3D_TEX_VRAM, dst, src, TEXVRAMSIZ);
    GF_ASSERT(rc);
    src = wk->pal->pRawData;
		dst = pal_adr;
    rc = NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_3D_TEX_PLTT, dst, src, 32);
    GF_ASSERT(rc);
  }
  
  return wk;
}
static void IndependentPokeExit( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  GFL_BMP_Delete( poke_wk->bmp_data );
  GFL_HEAP_FreeMemory( poke_wk->chr_buf );
  GFL_HEAP_FreeMemory( poke_wk->pal_buf );

  GFL_HEAP_FreeMemory( poke_wk );
}

static void IndependentPokeInitEvo( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  IndependentPokeInitAfter( poke_wk, heap_id );

  {
    u8 i;
    poke_wk->pal_state = INDEPENDENT_PAL_STATE_COLOR;
    for( i=0; i<16; i++ )
    {
      poke_wk->pal_curr[i] = 0x7fff;
    }
    poke_wk->pal_color = 0x7fff;
    poke_wk->pal_rate = 0;
    poke_wk->pal_wait = 0;
    poke_wk->pal_speed = 0;
    poke_wk->pal_wait_count = 0;
  }

  {
//    poke_wk->change_no = 0;
    
    poke_wk->change_no_down_x = INDEPENDENT_PANEL_NUM_X -1;
    poke_wk->change_no_down_y = INDEPENDENT_PANEL_NUM_Y /2 -1;
    poke_wk->change_no_up_x   = 0;
    poke_wk->change_no_up_y   = poke_wk->change_no_down_y +1;
  }

  {
    poke_wk->spi_state = INDEPENDENT_SPIRAL_STATE_BEFORE_START;
    poke_wk->spi_theta_idx = 0;  // 0<= <2PI�̃C���f�b�N�X �őS�̂𗆐��ŉ񂷂���
    poke_wk->spi_theta_idx_add = 0;  // spi_theta_idx�̑�����
    poke_wk->spi_count = 0;
  }

  {
    poke_wk->not_disp_y = 0;
  }

  {
    u8 i, j;
    for( j=0; j<INDEPENDENT_PANEL_NUM_Y; j++ )
    {
      for( i=0; i<INDEPENDENT_PANEL_NUM_X; i++ )
      {
        INDEPENDENT_PANEL_WORK* panel_wk = &(poke_wk->panel_wk[j][i]);

        panel_wk->count = 0;
        panel_wk->start_end_pos = panel_wk->pos;

        // ���̊Ԋu���������āA�؂�Ă��Ȃ������S�̔炪����Ă���悤�ɂ���
        {
          u8 no;
          const u8 rep = 6;
          const u8 compo = rep / 2;  // ������1��͌��G���s�ō\������Ă��邩
          const f32 height  = 44.0f;          // �����ɂ����Ƃ��̍���
          {
            // a b c d e f a b c d e f a b c d e f ... �ƒu���Ă���
            // a ���G%3==0�s�̑O����
            // b ���G%3==0�s�̌㔼��
            // c ���G%3==1�s�̑O����
            // d ���G%3==1�s�̌㔼��
            // e ���G%3==2�s�̑O����
            // f ���G%3==2�s�̌㔼��  // �����6�����Ȃ̂ŁArep��6
            if( j % 3 == 0 )  // %3==0�s
            {
              if( i < INDEPENDENT_PANEL_NUM_X /2 )  // �O����
              {
                no = i * rep;
              }
              else  // �㔼��
              {
                no = ( i - INDEPENDENT_PANEL_NUM_X/2 ) * rep + 1;
              }
            }
            else if( j % 3 == 1 )  // %3==1�s
            {
              if( i < INDEPENDENT_PANEL_NUM_X /2 )  // �O����
              {
                no = i * rep + 2;
              }
              else  // �㔼��
              {
                no = ( i - INDEPENDENT_PANEL_NUM_X/2 ) * rep + 3;
              }
            }
            else  // %3==2�s
            {
              if( i < INDEPENDENT_PANEL_NUM_X /2 )  // �O����
              {
                no = i * rep + 4;
              }
              else  // �㔼��
              {
                no = ( i - INDEPENDENT_PANEL_NUM_X/2 ) * rep + 5;
              }
            }
            //panel_wk->steady_theta_idx = 0x10000 / ( INDEPENDENT_PANEL_NUM_X * compo ) * no;
            panel_wk->steady_theta_idx = 0x10000 * no / ( INDEPENDENT_PANEL_NUM_X * compo );  // �v�Z�̏��Ԃ����ւ���
          }
          {
            const f32 h_start = height / 2.0f;  // �����ɂ����Ƃ��̍����̊J�n�ʒu(��ԍ����Ƃ���)
            const f32 h_sep   = height * compo / INDEPENDENT_PANEL_NUM_X;  // �����ɂ����Ƃ��̏c�̊Ԋu(���Gcompo�s�ŗ�����1��ɂȂ�)
            u8  retsu   = j / compo;          // �����ɂ����Ƃ�����ڂ�
            
            panel_wk->steady_pos_y = FX_F32_TO_FX32( h_start - h_sep * retsu - h_sep / ( INDEPENDENT_PANEL_NUM_X * compo ) * no );
          }
          {
            const f32 r_max = 20.0f;
            const f32 r_min = 10.0f;
            f32 height_from_center = FX_FX32_TO_F32( panel_wk->steady_pos_y );  // 2�悷��̂Ő��ł����ł�����
            //f32 r = ( r_max - r_min ) * ( height_from_center / (height/2.0f) ) * ( height_from_center / (height/2.0f) ) + r_min;
            f32 r = ( r_max - r_min ) *  height_from_center * height_from_center / height / height * 2.0f * 2.0f + r_min;  // ���ʂ��͂����������œ����v�Z
            panel_wk->steady_r = FX_F32_TO_FX32(r);
          }
          {
            panel_wk->steady_pos.x = FX_MUL( panel_wk->steady_r, FX_CosIdx( panel_wk->steady_theta_idx ) );
            panel_wk->steady_pos.z = FX_MUL( panel_wk->steady_r, FX_SinIdx( panel_wk->steady_theta_idx ) );

            panel_wk->steady_pos.y = panel_wk->steady_pos_y;
          }
        }
        
        // ����
        {
          VecFx32 dir;
          fx32 dis_zx_sq;
          fx32 dis_y_sq;
          fx32 dis_zx;
          fx32 dis_y;

          dir.x = panel_wk->steady_pos.x - panel_wk->pos.x;
          dir.y = panel_wk->steady_pos.y - panel_wk->pos.y;
          dir.z = panel_wk->steady_pos.z - panel_wk->pos.z;

          dis_zx_sq = FX_MUL( dir.x, dir.x ) + FX_MUL( dir.z, dir.z );
          dis_y_sq = FX_MUL( dir.y, dir.y );
          
          dis_zx = FX_Sqrt( dis_zx_sq );
          dis_y = FX_Sqrt( dis_y_sq );

          if( dis_zx > FX32_CONST(INDEPENDENT_SPIRAL_ZX_FIRST_VEL) )
          {
            panel_wk->first_vel.x = FX_Div( dir.x, dis_zx ) * INDEPENDENT_SPIRAL_ZX_FIRST_VEL;
            panel_wk->first_vel.z = FX_Div( dir.z, dis_zx ) * INDEPENDENT_SPIRAL_ZX_FIRST_VEL;
          }
          else
          {
            panel_wk->first_vel.x = dir.x;
            panel_wk->first_vel.z = dir.z;
          }

          if( dis_y > FX32_CONST(INDEPENDENT_SPIRAL_Y_FIRST_VEL) )
          {
            panel_wk->first_vel.y = FX_Div( dir.y, dis_y ) * INDEPENDENT_SPIRAL_Y_FIRST_VEL;
          }
          else
          {
            panel_wk->first_vel.y = dir.y;
          }
        }

        panel_wk->state = INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_BEFORE_START;
      }
    }
  }
}
static void IndependentPokeInitAfter( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  {
    poke_wk->pos.x = 0;
    poke_wk->pos.y = 0;
    poke_wk->pos.z = 0;
  }

  {
    poke_wk->not_disp_y = INDEPENDENT_PANEL_NUM_Y;
  }

  {
    int  i;
    int  max;
    f32  w, h;
    f32  sw, th;
    f32  start_x, start_y;
    f32  epsilon_x, epsilon_y;  // ���ʂ��Ȃ��̂ŁA�g���̂�߂��B
    f32  s_for_1pixel, t_for_1pixel;  // �e�N�X�`��1�s�N�Z�����̃e�N�X�`���̕��A�e�N�X�`���̍���
    f32  w_for_1pixel, h_for_1pixel;  // �e�N�X�`��1�s�N�Z�����̃p�l���̕��A�p�l���̍���

    max = INDEPENDENT_PANEL_NUM_X * INDEPENDENT_PANEL_NUM_Y;

    w = ( (f32)INDEPENDENT_PANEL_TOTAL_W ) / INDEPENDENT_PANEL_NUM_X;  // 1���̃p�l���̃T�C�Y
    h = ( (f32)INDEPENDENT_PANEL_TOTAL_H ) / INDEPENDENT_PANEL_NUM_Y;

    sw = ( (f32)( INDEPENDENT_PANEL_TOTAL_MAX_S - INDEPENDENT_PANEL_TOTAL_MIN_S ) ) / INDEPENDENT_PANEL_NUM_X;  // 1���̃p�l���̃e�N�X�`���̕�
    th = ( (f32)( INDEPENDENT_PANEL_TOTAL_MAX_T - INDEPENDENT_PANEL_TOTAL_MIN_T ) ) / INDEPENDENT_PANEL_NUM_Y;  // 1���̃p�l���̃e�N�X�`���̍���

    start_x = - INDEPENDENT_PANEL_TOTAL_W / 2.0f + w / 2.0f;  // �E����
    start_y =   INDEPENDENT_PANEL_TOTAL_H / 2.0f - h / 2.0f;  // �オ��

    //epsilon_x =   ( (f32)( INDEPENDENT_PANEL_TOTAL_MAX_S - INDEPENDENT_PANEL_TOTAL_MIN_S ) ) / TEXSIZ_S / INDEPENDENT_PANEL_NUM_X \
    //            / ( ( INDEPENDENT_PANEL_TOTAL_MAX_S - INDEPENDENT_PANEL_TOTAL_MIN_S ) * INDEPENDENT_PANEL_NUM_X );  // 1�s�N�Z���̓e�N�X�`�����W�n�łǂꂾ���̑傫����
    //epsilon_x = 1.0f / TEXSIZ_S;  // ��L�̌v�Z���܂Ƃ߂�Ƃ����Ȃ�
    //epsilon_y = 1.0f / TEXSIZ_T;
    epsilon_x = 1.0f;  // �e�N�X�`�����W�n��0<= <=1����Ȃ���0<= <=128�ł����݂����B
    epsilon_y = 1.0f;  // ������1�s�N�Z����1�̂܂܁B

    s_for_1pixel = 0.0f;//0.05f;//1.0f;  // 1.0f�s�N�Z������Ȃ��Ă�OK�Ȃ悤�ɂ����Ă���
    t_for_1pixel = 0.0f;//0.05f;//1.0f;
    w_for_1pixel = w / sw * s_for_1pixel;
    h_for_1pixel = h / th * t_for_1pixel;

    for( i=0; i<max; i++ )
    {
      int x, y;
      INDEPENDENT_PANEL_WORK* panel_wk;

      x = i%INDEPENDENT_PANEL_NUM_X;
      y = i/INDEPENDENT_PANEL_NUM_X;

      panel_wk = &(poke_wk->panel_wk[y][x]);

      panel_wk->polygon_id = INDEPENDENT_BEFORE_POLYGON_ID;
      panel_wk->alpha = 31;
      
      //panel_wk->vtx[0].x = FX_F32_TO_FX16( - w /2.0f );  panel_wk->vtx[0].y = FX_F32_TO_FX16(   h /2.0f );  panel_wk->vtx[0].z = 0;
      //panel_wk->vtx[1].x = FX_F32_TO_FX16( - w /2.0f );  panel_wk->vtx[1].y = FX_F32_TO_FX16( - h /2.0f );  panel_wk->vtx[1].z = 0;
      //panel_wk->vtx[2].x = FX_F32_TO_FX16(   w /2.0f );  panel_wk->vtx[2].y = FX_F32_TO_FX16( - h /2.0f );  panel_wk->vtx[2].z = 0;
      //panel_wk->vtx[3].x = FX_F32_TO_FX16(   w /2.0f );  panel_wk->vtx[3].y = FX_F32_TO_FX16(   h /2.0f );  panel_wk->vtx[3].z = 0;

      panel_wk->vtx[0].x = FX_F32_TO_FX16( - w /2.0f - w_for_1pixel );  panel_wk->vtx[0].y = FX_F32_TO_FX16(   h /2.0f + h_for_1pixel);  panel_wk->vtx[0].z = 0;
      panel_wk->vtx[1].x = FX_F32_TO_FX16( - w /2.0f - w_for_1pixel );  panel_wk->vtx[1].y = FX_F32_TO_FX16( - h /2.0f - h_for_1pixel);  panel_wk->vtx[1].z = 0;
      panel_wk->vtx[2].x = FX_F32_TO_FX16(   w /2.0f + w_for_1pixel );  panel_wk->vtx[2].y = FX_F32_TO_FX16( - h /2.0f - h_for_1pixel);  panel_wk->vtx[2].z = 0;
      panel_wk->vtx[3].x = FX_F32_TO_FX16(   w /2.0f + w_for_1pixel );  panel_wk->vtx[3].y = FX_F32_TO_FX16(   h /2.0f + h_for_1pixel);  panel_wk->vtx[3].z = 0;

      panel_wk->pos = poke_wk->pos;
      panel_wk->pos.x += FX_F32_TO_FX32( start_x + w * x );
      panel_wk->pos.y += FX_F32_TO_FX32( start_y - h * y );

      //panel_wk->s0 = FX_F32_TO_FX32( x * sw + INDEPENDENT_PANEL_TOTAL_MIN_S );
      //panel_wk->t0 = FX_F32_TO_FX32( y * th + INDEPENDENT_PANEL_TOTAL_MIN_T );
      //panel_wk->s1 = panel_wk->s0 + FX_F32_TO_FX32(sw);
      //panel_wk->t1 = panel_wk->t0 + FX_F32_TO_FX32(th);

      //panel_wk->s0 = FX_F32_TO_FX32( x * sw + INDEPENDENT_PANEL_TOTAL_MIN_S + epsilon_x/10 );
      //panel_wk->t0 = FX_F32_TO_FX32( y * th + INDEPENDENT_PANEL_TOTAL_MIN_T + epsilon_y/10 );
      //panel_wk->s1 = panel_wk->s0 + FX_F32_TO_FX32(sw - epsilon_x/10);
      //panel_wk->t1 = panel_wk->t0 + FX_F32_TO_FX32(th - epsilon_y/10);

      panel_wk->s0 = FX_F32_TO_FX32( x * sw + INDEPENDENT_PANEL_TOTAL_MIN_S - s_for_1pixel );
      panel_wk->t0 = FX_F32_TO_FX32( y * th + INDEPENDENT_PANEL_TOTAL_MIN_T - t_for_1pixel );
      panel_wk->s1 = panel_wk->s0 + FX_F32_TO_FX32(sw + s_for_1pixel*2.0f);
      panel_wk->t1 = panel_wk->t0 + FX_F32_TO_FX32(th + t_for_1pixel*2.0f);

      panel_wk->pos_prev = panel_wk->pos;
    }
  }
}

static void IndependentPokeMain( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  // �������Ȃ�
}

static void IndependentPokeDraw( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  {
    G3_TexImageParam(
        GX_TEXFMT_PLTT16,
        GX_TEXGEN_TEXCOORD,
        GX_TEXSIZE_S128,
        GX_TEXSIZE_T128,
        GX_TEXREPEAT_ST,
        GX_TEXFLIP_NONE,
        GX_TEXPLTTCOLOR0_TRNS,//GX_TEXPLTTCOLOR0_TRNS,//GX_TEXPLTTCOLOR0_USE,
        poke_wk->tex_adr );

    G3_TexPlttBase( poke_wk->pal_adr, GX_TEXFMT_PLTT16 );
  }

  {
    int i;

    G3_PushMtx();

/*
���ʂȂ��Ƃ�Manager��1�񂾂��ōς܂�
    G3_MaterialColorDiffAmb(
							GX_RGB(31, 31, 31),
							GX_RGB(31, 31, 31),
              FALSE
        );

    //G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);
    G3_MaterialColorSpecEmi(GX_RGB(31, 31, 31), GX_RGB(31, 31, 31), FALSE);  // ���C�g�Ȃ��p�̐ݒ�emission GX_RGB(31, 31, 31)
*/

/*
�s�v
    //���C�g�J���[
    G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));
*/

    // �S�̂̈ʒu�ݒ�
    G3_Translate(poke_wk->pos.x, poke_wk->pos.y, poke_wk->pos.z);

#ifdef DEF_SPEED_UP_A
    G3_PolygonAttr( 
        GX_LIGHTMASK_NONE,//GX_LIGHTMASK_0,			  // ���C�g�𔽉f  // ���C�g�Ȃ��p�̐ݒ�GX_LIGHTMASK_NONE
        GX_POLYGONMODE_MODULATE,	  // ���W�����[�V�����|���S�����[�h
        GX_CULL_BACK,             // �J�����O
        0,                         // �|���S���h�c �O
        31,					  // �A���t�@�l  // �p�[�e�B�N������O�ɕ`�悷��ɂ͔������ɂ��邵���肪�Ȃ��E�E�E
        GX_POLYGON_ATTR_MISC_NONE );
#endif

    {
      //int max;
      //max = INDEPENDENT_PANEL_NUM_X * INDEPENDENT_PANEL_NUM_Y;
      int x, y;

      //for( i=0; i<max; i++ )
      //for( y=0; y<INDEPENDENT_PANEL_NUM_Y; y++ )
      for( y=0; y<poke_wk->not_disp_y; y++ )
        for( x=0; x<INDEPENDENT_PANEL_NUM_X; x++ )
      {
        //int x, y;
        INDEPENDENT_PANEL_WORK* panel_wk;
        GXCull cull; 

        //x = i%INDEPENDENT_PANEL_NUM_X;  // %��/���g�����y,x�̓�d���[�v�ɂ����ق���
        //y = i/INDEPENDENT_PANEL_NUM_X;  // �����������C�������̂ŁA���̂悤�ɕύX�����B

        panel_wk = &(poke_wk->panel_wk[y][x]);
        
        // �|���S���A�g���r���[�g�ݒ�
        if( panel_wk->alpha == 0 )  // �A���t�@0�̂܂ܕ`�悵�Ă��܂��ƁA���C���[�t���[���ŕ\������Ă��܂��̂ŁB
        {
          cull = GX_CULL_ALL;
          continue;
        }
        else
        {
          cull = GX_CULL_BACK;
        }

        G3_PushMtx();
	      
#ifndef DEF_SPEED_UP_A
        G3_PolygonAttr(
            GX_LIGHTMASK_NONE,//GX_LIGHTMASK_0,			  // ���C�g�𔽉f  // ���C�g�Ȃ��p�̐ݒ�GX_LIGHTMASK_NONE
            GX_POLYGONMODE_MODULATE,	  // ���W�����[�V�����|���S�����[�h
            cull,             // �J�����O
            panel_wk->polygon_id,                         // �|���S���h�c �O
            panel_wk->alpha,					  // �A���t�@�l  // �p�[�e�B�N������O�ɕ`�悷��ɂ͔������ɂ��邵���肪�Ȃ��E�E�E
            GX_POLYGON_ATTR_MISC_NONE );
#endif

        // �ʒu�ݒ�
		    G3_Translate(panel_wk->pos.x, panel_wk->pos.y, panel_wk->pos.z);

/*
�s�v
        // �X�P�[���ݒ�
		    G3_Scale(FX32_ONE, FX32_ONE, FX32_ONE);
*/

/*
�s�v
        {
          MtxFx33 mtx;
          MTX_RotY33(&mtx, FX_SinIdx(0), FX_CosIdx(0));
          G3_MultMtx33(&mtx);
          MTX_RotZ33(&mtx, FX_SinIdx(0), FX_CosIdx(0));
          G3_MultMtx33(&mtx);
        }
*/

        G3_Begin( GX_BEGIN_QUADS );

        G3_Normal( 0, 0, FX32_ONE );

        G3_TexCoord(panel_wk->s0, panel_wk->t0);
	      G3_Vtx( panel_wk->vtx[0].x, panel_wk->vtx[0].y, panel_wk->vtx[0].z );
        G3_TexCoord(panel_wk->s0, panel_wk->t1);
	      G3_Vtx( panel_wk->vtx[1].x, panel_wk->vtx[1].y, panel_wk->vtx[1].z );
        G3_TexCoord(panel_wk->s1, panel_wk->t1);
	      G3_Vtx( panel_wk->vtx[2].x, panel_wk->vtx[2].y, panel_wk->vtx[2].z );
        G3_TexCoord(panel_wk->s1, panel_wk->t0);
	      G3_Vtx( panel_wk->vtx[3].x, panel_wk->vtx[3].y, panel_wk->vtx[3].z );

        G3_End();
        G3_PopMtx(1);
      }
    }

    G3_PopMtx(1);
  }
}

static void IndependentPokeManagerMainSpiral( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;
  HEAPID                    heap_id = work->heap_id;

  switch( independent_poke_mgr->state )
  {
  case INDEPENDENT_STATE_EVO_DEMO:
    {
      IndependentPokeMainSpiral( independent_poke_mgr->poke_wk[POKE_BEFORE], heap_id );
      //IndependentPokeMainSpiral( independent_poke_mgr->poke_wk[POKE_AFTER], heap_id );

      {
        INDEPENDENT_POKE_WORK* poke_wk_src = independent_poke_mgr->poke_wk[POKE_BEFORE];
        INDEPENDENT_POKE_WORK* poke_wk_dst = independent_poke_mgr->poke_wk[POKE_AFTER];

        u8 i, j;

        poke_wk_dst->spi_theta_idx = poke_wk_src->spi_theta_idx;

        for( j=0; j<INDEPENDENT_PANEL_NUM_Y; j++ )
        {
          for( i=0; i<INDEPENDENT_PANEL_NUM_X; i++ )
          {
            INDEPENDENT_PANEL_WORK* panel_wk_src = &(poke_wk_src->panel_wk[j][i]);
            INDEPENDENT_PANEL_WORK* panel_wk_dst = &(poke_wk_dst->panel_wk[j][i]);

            panel_wk_dst->pos = panel_wk_src->pos;
          }
        }

        {
/*
          while( poke_wk_src->change_no > poke_wk_dst->change_no )
          {
            u8 y = poke_wk_dst->change_no / INDEPENDENT_PANEL_NUM_X; 
            u8 x = poke_wk_dst->change_no % INDEPENDENT_PANEL_NUM_X; 
            INDEPENDENT_PANEL_WORK* change_panel_wk = &(poke_wk_dst->panel_wk[y][x]);
            change_panel_wk->alpha = 31;
            poke_wk_dst->change_no++;
          }
*/

          s16 x, y;

          // down
          for( y=poke_wk_dst->change_no_down_y; y>=poke_wk_src->change_no_down_y; y-- )
          {
            if( y==poke_wk_dst->change_no_down_y ) x=poke_wk_dst->change_no_down_x;
            else                                   x=INDEPENDENT_PANEL_NUM_X -1;
            while( x>=0 )
            {
              BOOL a = FALSE;
              if( y==poke_wk_src->change_no_down_y )
              {
                if( x>poke_wk_src->change_no_down_x )
                {
                  a = TRUE;
                }
                else
                {
                  break;
                }
              }
              else
              {
                a = TRUE;
              }
              if( a )
              {
                INDEPENDENT_PANEL_WORK* change_panel_wk = &(poke_wk_dst->panel_wk[y][x]);
                change_panel_wk->alpha = 31;
              }
              x--;
            }
          }
          poke_wk_dst->change_no_down_x = poke_wk_src->change_no_down_x;
          poke_wk_dst->change_no_down_y = poke_wk_src->change_no_down_y;

          // up
          for( y=poke_wk_dst->change_no_up_y; y<=poke_wk_src->change_no_up_y; y++ )
          {
            if( y==poke_wk_dst->change_no_up_y ) x=poke_wk_dst->change_no_up_x;
            else                                 x=0;
            while( x<INDEPENDENT_PANEL_NUM_X )
            {
              BOOL a = FALSE;
              if( y==poke_wk_src->change_no_up_y )
              {
                if( x<poke_wk_src->change_no_up_x )
                {
                  a = TRUE;
                }
                else
                {
                  break;
                }
              }
              else
              {
                a = TRUE;
              }
              if( a )
              {
                INDEPENDENT_PANEL_WORK* change_panel_wk = &(poke_wk_dst->panel_wk[y][x]);
                change_panel_wk->alpha = 31;
              }
              x++;
            }
          }
          poke_wk_dst->change_no_up_x = poke_wk_src->change_no_up_x;
          poke_wk_dst->change_no_up_y = poke_wk_src->change_no_up_y;
        }

        poke_wk_dst->not_disp_y = poke_wk_src->not_disp_y;
      }

      // �p���b�g�A�j��
      {
        INDEPENDENT_POKE_WORK* poke_wk_src = independent_poke_mgr->poke_wk[POKE_BEFORE];
        INDEPENDENT_POKE_WORK* poke_wk_dst = independent_poke_mgr->poke_wk[POKE_AFTER];

        poke_wk_dst->pal_state = poke_wk_src->pal_state;

        if( IndependentPokePalIsStartColorToWhite( poke_wk_src, heap_id ) )
        {
          IndependentPokePalStartColorToWhite( poke_wk_src, heap_id );
          IndependentPokePalStartColorToWhite( poke_wk_dst, heap_id );
        }
        if( IndependentPokePalIsStartWhiteToColor( poke_wk_src, heap_id ) )
        {
          //IndependentPokePalStartWhiteToColor( poke_wk_src, heap_id );
          //IndependentPokePalStartWhiteToColor( poke_wk_dst, heap_id );
        }
      }

      // INDEPENDENT_ONE
      {
        if( independent_poke_mgr->one_wk )
        {
          independent_poke_mgr->one_wk->not_disp_y = independent_poke_mgr->poke_wk[POKE_BEFORE]->not_disp_y;
          IndependentOneMain( independent_poke_mgr->one_wk, heap_id );
        }
      }

    }
    break;
  }
}
static void IndependentPokeMainSpiral( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  s16 i, j;
  
  poke_wk->spi_theta_idx += poke_wk->spi_theta_idx_add;
  //poke_wk->spi_theta_idx %= 0x10000;
  if( poke_wk->spi_theta_idx >= 0x10000 ) poke_wk->spi_theta_idx -= 0x10000;

  // �O��
  for( j=0; j<INDEPENDENT_PANEL_NUM_Y; j++ )
  {
    for( i=0; i<INDEPENDENT_PANEL_NUM_X; i++ )
    {
      INDEPENDENT_PANEL_WORK* panel_wk = &(poke_wk->panel_wk[j][i]);
      panel_wk->pos_prev = panel_wk->pos;
    }
  }
 
  switch( poke_wk->spi_state )
  {
  case INDEPENDENT_SPIRAL_STATE_BEFORE_START:
    {
      //if( poke_wk->spi_count >= 60 )
      if( poke_wk->spi_count >= 0 )
      {
        poke_wk->spi_state = INDEPENDENT_SPIRAL_STATE_START;
        poke_wk->spi_count = 0;
        poke_wk->spi_theta_idx_add = 0x600;
      }
      else
      {
        poke_wk->spi_count++;
      }
    }
    break;
  case INDEPENDENT_SPIRAL_STATE_START:
    {
      BOOL b_next = TRUE;
      BOOL b_first = TRUE;
      BOOL b_second = TRUE;

      for( j=0; j<INDEPENDENT_PANEL_NUM_Y; j++ )
      {
        for( i=0; i<INDEPENDENT_PANEL_NUM_X; i++ )
        {
          INDEPENDENT_PANEL_WORK* panel_wk = &(poke_wk->panel_wk[j][i]);
          VecFx32 ideal_pos;
          
          //int total_theta_idx = ( panel_wk->steady_theta_idx + poke_wk->spi_theta_idx ) % 0x10000;
          int total_theta_idx = ( panel_wk->steady_theta_idx + poke_wk->spi_theta_idx );
          if( total_theta_idx >= 0x10000 ) total_theta_idx -= 0x10000;

#ifndef DEF_SPEED_UP_B
          ideal_pos.x = FX_MUL( panel_wk->steady_r, FX_CosIdx( total_theta_idx ) );
          ideal_pos.z = FX_MUL( panel_wk->steady_r, FX_SinIdx( total_theta_idx ) );
#else
          {
            MtxFx43 mtx;
            VecFx32 vec;
            MTX_RotY43( &mtx, FX_SinIdx( total_theta_idx ), FX_CosIdx( total_theta_idx ) );
            vec.x = panel_wk->steady_r;
            vec.y = 0;
            vec.z = 0;
            MTX_MultVec43( &vec, &mtx, &(ideal_pos) );
          }
#endif

          ideal_pos.y = panel_wk->steady_pos_y;

          if( panel_wk->state == INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_BEFORE_START )
          {
            if( b_first || b_second )
            {
              // ��Ԏ肾������������
              if( panel_wk->count >= 0 )
              {
                panel_wk->count = 0;
                panel_wk->state = INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_START;
              }
              else
              {
                panel_wk->count++;
              }
              if( b_first )
              {
                b_first = FALSE;
              }
              else
              {
                b_second = FALSE;
              }
            }
            else
            {
              // �������Ȃ��ŁA���Ԃ�����̂�҂�
            }
            b_next = FALSE;
          }
          else if( panel_wk->state == INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_START )
          {
            if( panel_wk->count == 0 )  // �ŏ��͏����Ŕ�яo��
            {
              panel_wk->pos.x += panel_wk->first_vel.x;
              panel_wk->pos.y += panel_wk->first_vel.y; 
              panel_wk->pos.z += panel_wk->first_vel.z;
              
              panel_wk->count++;

              poke_wk->not_disp_y = j + 1;
            }
            else  // ������͈���O����ڎw��
            {
              VecFx32 dir;
              fx32 dis_zx_sq;
              fx32 dis_y_sq;

              dir.x = panel_wk->steady_pos.x - panel_wk->pos.x;
              dir.y = panel_wk->steady_pos.y - panel_wk->pos.y;
              dir.z = panel_wk->steady_pos.z - panel_wk->pos.z;
              
              dis_zx_sq = FX_MUL( dir.x, dir.x ) + FX_MUL( dir.z, dir.z );
              dis_y_sq = FX_MUL( dir.y, dir.y );

              if(    ( dis_zx_sq < INDEPENDENT_SPIRAL_ZX_START_SPEED_SQ_E_FX32 )
                  && ( dis_y_sq < INDEPENDENT_SPIRAL_Y_START_SPEED_SQ_E_FX32 ) )
              {
                // ����O���ɏ����
                panel_wk->pos.x = panel_wk->steady_pos.x;
                panel_wk->pos.z = panel_wk->steady_pos.z;

                panel_wk->pos.y = panel_wk->steady_pos.y; 
         
                {
                  int total_theta_idx_prev = total_theta_idx - poke_wk->spi_theta_idx_add;  // ���ł���
                  if( 1 )  // �����͕K���^�ɂ����ق������R�Ɍ������B�������Ȃ��ƁA���΂炭�p�l���������ꏊ�Ɏ~�܂��đ҂��Ă��܂��̂ŕs���R�B
                  //if(    total_theta_idx_prev <= panel_wk->steady_theta_idx
                  //    && panel_wk->steady_theta_idx <= total_theta_idx )  // ���傤�ǒʂ�߂����Ƃ�
                  {
                    // ����O���̎����̏ꏊ�ɂȂ���
                    panel_wk->pos.x = ideal_pos.x;
                    panel_wk->pos.z = ideal_pos.z;

                    panel_wk->pos.y = ideal_pos.y; 

                    panel_wk->count = 0;
                    panel_wk->state = INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_STEADY;
                  }
                  else
                  {
                    panel_wk->count++;
                  }
                }
              }
              else
              {
                BOOL zx_finish = FALSE;
                BOOL y_finish  = FALSE;
                if( dis_zx_sq < INDEPENDENT_SPIRAL_ZX_START_SPEED_SQ_E_FX32 ) 
                {
                  // ZX�����͈���O���ɏ����
                  zx_finish = TRUE;
                }
                else if( dis_y_sq < INDEPENDENT_SPIRAL_Y_START_SPEED_SQ_E_FX32 )
                {
                  // Y�����͈���O���ɏ����
                  y_finish  = TRUE;
                }
                
                if( zx_finish )
                {
                  // ZX�����͈���O���ɏ����
                  panel_wk->pos.x = panel_wk->steady_pos.x;
                  panel_wk->pos.z = panel_wk->steady_pos.z;
                }
                else
                {
                  // ZX����
                  {
#ifndef DEF_SPEED_UP_C
  #ifndef DEF_SPEED_UP_D
                    fx32 dis_zx = FX_Sqrt( dis_zx_sq );
                    fx32 add_x = FX_Div( dir.x, dis_zx ) * INDEPENDENT_SPIRAL_ZX_START_SPEED;
                    fx32 add_z = FX_Div( dir.z, dis_zx ) * INDEPENDENT_SPIRAL_ZX_START_SPEED;
  #else
                    fx32 inv_dis_zx = FX_InvSqrt( dis_zx_sq ) * INDEPENDENT_SPIRAL_ZX_START_SPEED;
                    fx32 add_x = FX_MUL( dir.x, inv_dis_zx );
                    fx32 add_z = FX_MUL( dir.z, inv_dis_zx );
  #endif
#else
                    VecFx32  dir_zx;
                    VecFx32  add;
                    fx32     add_x;
                    fx32     add_z;
                    dir_zx.x = dir.x;
                    dir_zx.y = 0;
                    dir_zx.z = dir.z;
                    VEC_Normalize( &dir_zx, &add );
                    add_x = add.x * INDEPENDENT_SPIRAL_ZX_START_SPEED;
                    add_z = add.z * INDEPENDENT_SPIRAL_ZX_START_SPEED;
#endif

#ifndef DEF_SPEED_UP_A
                    panel_wk->pos.x = panel_wk->pos.x \
                        + FX_Div( FX_MUL( panel_wk->pos.x - panel_wk->pos_prev.x, FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE) ), FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE+INDEPENDENT_SPIRAL_CURR_RATE) ) \
                        + FX_Div( FX_MUL( add_x, FX32_CONST(INDEPENDENT_SPIRAL_CURR_RATE) ), FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE+INDEPENDENT_SPIRAL_CURR_RATE) );
                    panel_wk->pos.z = panel_wk->pos.z \
                        + FX_Div( FX_MUL( panel_wk->pos.z - panel_wk->pos_prev.z, FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE) ), FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE+INDEPENDENT_SPIRAL_CURR_RATE) ) \
                        + FX_Div( FX_MUL( add_z, FX32_CONST(INDEPENDENT_SPIRAL_CURR_RATE) ), FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE+INDEPENDENT_SPIRAL_CURR_RATE) );
#else
                    panel_wk->pos.x = panel_wk->pos.x \
                        + FX_MUL( panel_wk->pos.x - panel_wk->pos_prev.x, INDEPENDENT_SPIRAL_PREV_RATE_FX32 ) \
                        + FX_MUL( add_x, INDEPENDENT_SPIRAL_CURR_RATE_FX32 );
                    panel_wk->pos.z = panel_wk->pos.z \
                        + FX_MUL( panel_wk->pos.z - panel_wk->pos_prev.z, INDEPENDENT_SPIRAL_PREV_RATE_FX32 ) \
                        + FX_MUL( add_z, INDEPENDENT_SPIRAL_CURR_RATE_FX32 );
#endif
                  }
                }

                if( y_finish )
                {
                  // Y�����͈���O���ɏ����
                  panel_wk->pos.y = panel_wk->steady_pos.y; 
                }
                else
                {
                  // Y����
                  {
#ifndef DEF_SPEED_UP_A
                    fx32 dis_y = FX_Sqrt( dis_y_sq );
                    fx32 add_y = FX_Div( dir.y, dis_y ) * INDEPENDENT_SPIRAL_Y_START_SPEED;
                    
                    panel_wk->pos.y = panel_wk->pos.y \
                        + FX_Div( FX_MUL( panel_wk->pos.y - panel_wk->pos_prev.y, FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE) ), FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE+INDEPENDENT_SPIRAL_CURR_RATE) ) \
                        + FX_Div( FX_MUL( add_y, FX32_CONST(INDEPENDENT_SPIRAL_CURR_RATE) ), FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE+INDEPENDENT_SPIRAL_CURR_RATE) );
#else
                    fx32     add_y;
                    if( dir.y >= 0 )
                    {
                      add_y = INDEPENDENT_SPIRAL_Y_START_SPEED_FX32;
                    }
                    else
                    {
                      add_y = - INDEPENDENT_SPIRAL_Y_START_SPEED_FX32;  //FX32_CONST( - INDEPENDENT_SPIRAL_Y_START_SPEED );
                    }
                    panel_wk->pos.y = panel_wk->pos.y \
                        + FX_MUL( panel_wk->pos.y - panel_wk->pos_prev.y, INDEPENDENT_SPIRAL_PREV_RATE_FX32 ) \
                        + FX_MUL( add_y, INDEPENDENT_SPIRAL_CURR_RATE_FX32 );
#endif
                  }
                }

                panel_wk->count++;
              }
            }
            b_next = FALSE;
          }
          else if( panel_wk->state == INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_STEADY )
          {
            panel_wk->pos.x = ideal_pos.x;
            panel_wk->pos.z = ideal_pos.z;
          
            panel_wk->pos.y = ideal_pos.y;
          }
        }
      }

      if( b_next )
      {
        poke_wk->spi_state = INDEPENDENT_SPIRAL_STATE_STEADY;
        if(    poke_wk->pal_state != INDEPENDENT_PAL_STATE_START_COLOR_TO_WHITE
            && poke_wk->pal_state != INDEPENDENT_PAL_STATE_COLOR_TO_WHITE )
        {
          poke_wk->pal_state = INDEPENDENT_PAL_STATE_START_COLOR_TO_WHITE;
        }
      }
    }
    break;
  case INDEPENDENT_SPIRAL_STATE_STEADY:
    {
      u16 max = INDEPENDENT_PANEL_NUM_X * INDEPENDENT_PANEL_NUM_Y;
      
      // ��]���x�A�b�v
      if( poke_wk->spi_count<100 )
      {
        //if( poke_wk->spi_count % 5 == 0 )
        {
          poke_wk->spi_theta_idx_add += 0x20;
        }
      }

      // ��]���x�_�E��
      //if( 140<=poke_wk->spi_count && poke_wk->spi_count<240 )
      if( 200<=poke_wk->spi_count && poke_wk->spi_count<240 )
      {
        //if( poke_wk->spi_count % 5 == 0 )
        {
          poke_wk->spi_theta_idx_add -= 0x20;
        }
      }

      for( j=0; j<INDEPENDENT_PANEL_NUM_Y; j++ )
      {
        for( i=0; i<INDEPENDENT_PANEL_NUM_X; i++ )
        {
          INDEPENDENT_PANEL_WORK* panel_wk = &(poke_wk->panel_wk[j][i]);
          
          //int total_theta_idx = ( panel_wk->steady_theta_idx + poke_wk->spi_theta_idx ) % 0x10000;
          int total_theta_idx = ( panel_wk->steady_theta_idx + poke_wk->spi_theta_idx );
          if( total_theta_idx >= 0x10000 ) total_theta_idx -= 0x10000;

#ifndef DEF_SPEED_UP_B
          panel_wk->pos.x = FX_MUL( panel_wk->steady_r, FX_CosIdx( total_theta_idx ) );
          panel_wk->pos.z = FX_MUL( panel_wk->steady_r, FX_SinIdx( total_theta_idx ) );
#else
          {
            MtxFx43 mtx;
            VecFx32 vec;
            MTX_RotY43( &mtx, FX_SinIdx( total_theta_idx ), FX_CosIdx( total_theta_idx ) );
            vec.x = panel_wk->steady_r;
            vec.y = 0;
            vec.z = 0;
            MTX_MultVec43( &vec, &mtx, &(panel_wk->pos) );
          }
#endif

          panel_wk->pos.y = panel_wk->steady_pos_y; 
        }
      }

/*
      // 1������ւ��Ă���
      if( IndependentPokePalIsWhite( poke_wk, heap_id ) && poke_wk->spi_count >= 30 )
      {
        u16 start_change_no = poke_wk->change_no;
        u16 end_change_no = poke_wk->change_no +2;  // start_change_no<= <end_change_no
        if( end_change_no > max )
        {
          end_change_no = max;
        }
        while( poke_wk->change_no < end_change_no )
        {
          u8 y = poke_wk->change_no / INDEPENDENT_PANEL_NUM_X; 
          u8 x = poke_wk->change_no % INDEPENDENT_PANEL_NUM_X; 
          INDEPENDENT_PANEL_WORK* change_panel_wk = &(poke_wk->panel_wk[y][x]);
          change_panel_wk->alpha = 0;
          poke_wk->change_no++;
        }
      }
*/
      // 1������ւ��Ă���
      if( IndependentPokePalIsWhite( poke_wk, heap_id ) && poke_wk->spi_count >= 30 )
      {
        s16 x, y;

        // down
        x = poke_wk->change_no_down_x;
        y = poke_wk->change_no_down_y;
        if( y >= 0 && x >= 0 )
        {
          INDEPENDENT_PANEL_WORK* change_panel_wk = &(poke_wk->panel_wk[y][x]);
          change_panel_wk->alpha = 0;
          if( x-1 < 0 )
          {
            poke_wk->change_no_down_x = INDEPENDENT_PANEL_NUM_X;
            poke_wk->change_no_down_y--;
          }
          else
          {
            poke_wk->change_no_down_x--;
            //y�͂��̂܂�
          }
        }

        // up
        x = poke_wk->change_no_up_x;
        y = poke_wk->change_no_up_y;
        if( y < INDEPENDENT_PANEL_NUM_Y && x < INDEPENDENT_PANEL_NUM_X )
        {
          INDEPENDENT_PANEL_WORK* change_panel_wk = &(poke_wk->panel_wk[y][x]);
          change_panel_wk->alpha = 0;
          if( x+1 >= INDEPENDENT_PANEL_NUM_X )
          {
            poke_wk->change_no_up_x = 0;
            poke_wk->change_no_up_y++;
          }
          else
          {
            poke_wk->change_no_up_x++;
            //y�͂��̂܂�
          }
        }
      }

      // ���� or �J�E���g�A�b�v
      //if( poke_wk->change_no >= max && poke_wk->spi_count >= 240 )
      if(    ( poke_wk->change_no_down_y < 0 && poke_wk->change_no_up_y >= INDEPENDENT_PANEL_NUM_Y )
          && ( poke_wk->spi_count >= 240 ) )
      {
        poke_wk->spi_state = INDEPENDENT_SPIRAL_STATE_END;
        poke_wk->spi_count = 0;
      }
      else
      {
        poke_wk->spi_count++;
      }
    }
    break;
  case INDEPENDENT_SPIRAL_STATE_END:
    {
      BOOL b_next = TRUE;
      BOOL b_first = TRUE;
      BOOL b_second = TRUE;

      for( j=INDEPENDENT_PANEL_NUM_Y-1; j>=0; j-- )
      {
        for( i=INDEPENDENT_PANEL_NUM_X-1; i>=0; i-- )
        {
          INDEPENDENT_PANEL_WORK* panel_wk = &(poke_wk->panel_wk[j][i]);
          
          //int total_theta_idx = ( panel_wk->steady_theta_idx + poke_wk->spi_theta_idx ) % 0x10000;
          int total_theta_idx = ( panel_wk->steady_theta_idx + poke_wk->spi_theta_idx );
          if( total_theta_idx >= 0x10000 ) total_theta_idx -= 0x10000;

          if( panel_wk->state == INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_STEADY )
          {
            if( b_first || b_second )
            {
              // ��Ԏ肾������������
              if( panel_wk->count >= 0 )
              {
                panel_wk->count = 0;
                panel_wk->state = INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_END;
              }
              else
              {
                panel_wk->count++;
              }
              if( b_first )
              {
                b_first = FALSE;
              }
              else
              {
                b_second = FALSE;
              }
            }
            else
            {
              // ���Ԃ�����̂�҂�
#ifndef DEF_SPEED_UP_B
              panel_wk->pos.x = FX_MUL( panel_wk->steady_r, FX_CosIdx( total_theta_idx ) );
              panel_wk->pos.z = FX_MUL( panel_wk->steady_r, FX_SinIdx( total_theta_idx ) );
#else
              {
                MtxFx43 mtx;
                VecFx32 vec;
                MTX_RotY43( &mtx, FX_SinIdx( total_theta_idx ), FX_CosIdx( total_theta_idx ) );
                vec.x = panel_wk->steady_r;
                vec.y = 0;
                vec.z = 0;
                MTX_MultVec43( &vec, &mtx, &(panel_wk->pos) );
              }
#endif

              panel_wk->pos.y = panel_wk->steady_pos_y;
            }
            b_next = FALSE;
          }
          else if( panel_wk->state == INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_END )
          {
            if( panel_wk->count == 0 )  // �ŏ��͊����Ŕ�яo��
            {
#ifndef DEF_SPEED_UP_B
              panel_wk->pos.x = FX_MUL( panel_wk->steady_r, FX_CosIdx( total_theta_idx ) );
              panel_wk->pos.z = FX_MUL( panel_wk->steady_r, FX_SinIdx( total_theta_idx ) );
#else
              {
                MtxFx43 mtx;
                VecFx32 vec;
                MTX_RotY43( &mtx, FX_SinIdx( total_theta_idx ), FX_CosIdx( total_theta_idx ) );
                vec.x = panel_wk->steady_r;
                vec.y = 0;
                vec.z = 0;
                MTX_MultVec43( &vec, &mtx, &(panel_wk->pos) );
              }
#endif

              panel_wk->pos.y = panel_wk->steady_pos_y;

              {
                VecFx32 dir;
                fx32 dis_zx_sq;
                fx32 dis_y_sq;
                fx32 dis_zx;
                fx32 dis_y;
                
                VecFx32 add;

                dir.x = panel_wk->pos.x - panel_wk->pos_prev.x;
                dir.y = panel_wk->pos.y - panel_wk->pos_prev.y;
                dir.z = panel_wk->pos.z - panel_wk->pos_prev.z;

                dis_zx_sq = FX_MUL( dir.x, dir.x ) + FX_MUL( dir.z, dir.z );
                dis_y_sq = FX_MUL( dir.y, dir.y );

#ifndef DEF_SPEED_UP_A
                dis_zx = FX_Sqrt( dis_zx_sq );
                dis_y = FX_Sqrt( dis_y_sq );

                if( dis_zx > FX32_CONST(INDEPENDENT_SPIRAL_ZX_END_SPEED) )
                {
                  add.x = FX_Div( dir.x, dis_zx ) * INDEPENDENT_SPIRAL_ZX_END_SPEED;
                  add.z = FX_Div( dir.z, dis_zx ) * INDEPENDENT_SPIRAL_ZX_END_SPEED;
                }
                else
                {
                  add.x = dir.x;
                  add.z = dir.z;
                }

                if( dis_y > FX32_CONST(INDEPENDENT_SPIRAL_Y_END_SPEED) )
                {
                  add.y = FX_Div( dir.y, dis_y ) * INDEPENDENT_SPIRAL_Y_END_SPEED;
                }
                else
                {
                  add.y = dir.y;
                }
#else
                if( dis_zx_sq > INDEPENDENT_SPIRAL_ZX_END_SPEED_SQ_FX32 )
                {
#ifndef DEF_SPEED_UP_C
  #ifndef DEF_SPEED_UP_D
                  dis_zx = FX_Sqrt( dis_zx_sq );
                  add.x = FX_Div( dir.x, dis_zx ) * INDEPENDENT_SPIRAL_ZX_END_SPEED;
                  add.z = FX_Div( dir.z, dis_zx ) * INDEPENDENT_SPIRAL_ZX_END_SPEED;
  #else
                  fx32 inv_dis_zx = FX_InvSqrt( dis_zx_sq ) * INDEPENDENT_SPIRAL_ZX_END_SPEED;
                  add.x = FX_MUL( dir.x, inv_dis_zx );
                  add.z = FX_MUL( dir.z, inv_dis_zx );
  #endif
#else
                  VecFx32  dir_zx;
                  VecFx32  add_tmp;
                  dir_zx.x = dir.x; 
                  dir_zx.y = 0; 
                  dir_zx.z = dir.z; 
                  VEC_Normalize( &dir_zx, &add_tmp );
                  add.x = add_tmp.x * INDEPENDENT_SPIRAL_ZX_END_SPEED;
                  add.z = add_tmp.z * INDEPENDENT_SPIRAL_ZX_END_SPEED;
#endif
                }
                else
                {
                  add.x = dir.x;
                  add.z = dir.z;
                }

                if( dis_y_sq > INDEPENDENT_SPIRAL_Y_END_SPEED_SQ_FX32 ) 
                {
                  if( dir.y >= 0 )
                  {
                    add.y = INDEPENDENT_SPIRAL_Y_END_SPEED_FX32;
                  }
                  else
                  {
                    add.y = - INDEPENDENT_SPIRAL_Y_END_SPEED_FX32;  // FX32_CONST( - INDEPENDENT_SPIRAL_Y_END_SPEED );
                  }
                }
                else
                {
                  add.y = dir.y;
                }
#endif

                panel_wk->pos.x += add.x;
                panel_wk->pos.y += add.y;
                panel_wk->pos.z += add.z;
              }

              panel_wk->count++;
            }
            else  // ������͔z�u�ʒu��ڎw��
            {
              VecFx32 dir;
              fx32 dis_zx_sq;
              fx32 dis_y_sq;
              fx32 dis_zx;
              fx32 dis_y;
                
              VecFx32 add;

              dir.x = panel_wk->start_end_pos.x - panel_wk->pos.x;
              dir.y = panel_wk->start_end_pos.y - panel_wk->pos.y;
              dir.z = panel_wk->start_end_pos.z - panel_wk->pos.z;

              dis_zx_sq = FX_MUL( dir.x, dir.x ) + FX_MUL( dir.z, dir.z );
              dis_y_sq = FX_MUL( dir.y, dir.y );

              if(    ( dis_zx_sq < INDEPENDENT_SPIRAL_ZX_END_SPEED_SQ_E_FX32 )
                  && ( dis_y_sq < INDEPENDENT_SPIRAL_Y_END_SPEED_SQ_E_FX32 ) )
              {
                // �z�u�ʒu�ɒ�����
                panel_wk->pos.x = panel_wk->start_end_pos.x;
                panel_wk->pos.z = panel_wk->start_end_pos.z;

                panel_wk->pos.y = panel_wk->start_end_pos.y; 

                panel_wk->count = 0;
                panel_wk->state = INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_AFTER_END;
              }
              else
              {
#ifndef DEF_SPEED_UP_A
                dis_zx = FX_Sqrt( dis_zx_sq );
                dis_y = FX_Sqrt( dis_y_sq );

                if( dis_zx > FX32_CONST(INDEPENDENT_SPIRAL_ZX_END_SPEED) )
                {
                  add.x = FX_Div( dir.x, dis_zx ) * INDEPENDENT_SPIRAL_ZX_END_SPEED;
                  add.z = FX_Div( dir.z, dis_zx ) * INDEPENDENT_SPIRAL_ZX_END_SPEED;
                }
                else
                {
                  add.x = dir.x;
                  add.z = dir.z;
                }

                if( dis_y > FX32_CONST(INDEPENDENT_SPIRAL_Y_END_SPEED) )
                {
                  add.y = FX_Div( dir.y, dis_y ) * INDEPENDENT_SPIRAL_Y_END_SPEED;
                }
                else
                {
                  add.y = dir.y;
                }
#else
                if( dis_zx_sq > INDEPENDENT_SPIRAL_ZX_END_SPEED_SQ_FX32 ) 
                {
#ifndef DEF_SPEED_UP_C
  #ifndef DEF_SPEED_UP_D
                  dis_zx = FX_Sqrt( dis_zx_sq );
                  add.x = FX_Div( dir.x, dis_zx ) * INDEPENDENT_SPIRAL_ZX_END_SPEED;
                  add.z = FX_Div( dir.z, dis_zx ) * INDEPENDENT_SPIRAL_ZX_END_SPEED;
  #else
                  fx32 inv_dis_zx = FX_InvSqrt( dis_zx_sq ) * INDEPENDENT_SPIRAL_ZX_END_SPEED;
                  add.x = FX_MUL( dir.x, inv_dis_zx );
                  add.z = FX_MUL( dir.z, inv_dis_zx );
  #endif
#else
                  VecFx32  dir_zx;
                  VecFx32  add_tmp;
                  dir_zx.x = dir.x; 
                  dir_zx.y = 0; 
                  dir_zx.z = dir.z; 
                  VEC_Normalize( &dir_zx, &add_tmp );
                  add.x = add_tmp.x * INDEPENDENT_SPIRAL_ZX_END_SPEED;
                  add.z = add_tmp.z * INDEPENDENT_SPIRAL_ZX_END_SPEED;
#endif
                }
                else
                {
                  add.x = dir.x;
                  add.z = dir.z;
                }

                if( dis_y_sq > INDEPENDENT_SPIRAL_Y_END_SPEED_SQ_FX32 )
                {
                  if( dir.y >= 0 )
                  {
                    add.y = INDEPENDENT_SPIRAL_Y_END_SPEED_FX32;
                  }
                  else
                  {
                    add.y = - INDEPENDENT_SPIRAL_Y_END_SPEED_FX32;  // FX32_CONST( - INDEPENDENT_SPIRAL_Y_END_SPEED );
                  }
                }
                else
                {
                  add.y = dir.y;
                }
#endif

                panel_wk->pos.x += add.x;
                panel_wk->pos.y += add.y;
                panel_wk->pos.z += add.z;
              }
            }
            b_next = FALSE;
          }
          else if( panel_wk->state == INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_AFTER_END )
          {
            // �������Ȃ�
          }
        }
      }

      if( b_next )
      {
        poke_wk->spi_state = INDEPENDENT_SPIRAL_STATE_AFTER_END;
        poke_wk->pal_state = INDEPENDENT_PAL_STATE_START_WHITE_TO_COLOR;
      }
    }
    break;
  case INDEPENDENT_SPIRAL_STATE_AFTER_END:
    {
    }
    break;
  }
}

static void IndependentPokePalStart( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id,
    u16 pal_color, s16 pal_rate_start, s16 pal_rate_end, s16 pal_wait, s16 pal_speed )
{
  poke_wk->pal_color = pal_color;
  poke_wk->pal_rate_start = pal_rate_start;
  poke_wk->pal_rate_end = pal_rate_end;
  poke_wk->pal_wait = pal_wait;
  poke_wk->pal_speed = pal_speed;

  // �����I�ɏC��
  poke_wk->pal_rate_start = MATH_CLAMP( poke_wk->pal_rate_start, 0, 31 );
  poke_wk->pal_rate_end = MATH_CLAMP( poke_wk->pal_rate_end, 0, 31 );
  poke_wk->pal_speed = MATH_CLAMP( poke_wk->pal_speed, -31, 31 );
  if( poke_wk->pal_rate_start < poke_wk->pal_rate_end )
  {
    if( poke_wk->pal_speed < 0 ) poke_wk->pal_speed *= -1;
  }
  else if( poke_wk->pal_rate_start > poke_wk->pal_rate_end )
  {
    if( poke_wk->pal_speed > 0 ) poke_wk->pal_speed *= -1;
  }
  else
  {
    // �I��
    poke_wk->pal_speed = 0;
  }

  poke_wk->pal_rate = poke_wk->pal_rate_start;
  poke_wk->pal_wait_count = poke_wk->pal_wait;

  // ���ݒl�������ē]��
  IndependentPokePalTrans( poke_wk, heap_id );
}
static BOOL IndependentPokePalIsEnd( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  if( poke_wk->pal_speed == 0 ) return TRUE;
  return FALSE;
}
static void IndependentPokePalUpdate( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  if( poke_wk->pal_speed == 0 ) return;

  if( poke_wk->pal_wait_count > 0 )
  {
    poke_wk->pal_wait_count--;
    return;
  }

  poke_wk->pal_rate += poke_wk->pal_speed;
  poke_wk->pal_rate = MATH_CLAMP( poke_wk->pal_rate, 0, 31 );
  
  if( poke_wk->pal_rate == poke_wk->pal_rate_end )
  {
    // �I��
    poke_wk->pal_speed = 0;
  }
  else
  {
    // �p��
    poke_wk->pal_wait_count = poke_wk->pal_wait;
  }
 
  // ���ݒl�������ē]��
  IndependentPokePalTrans( poke_wk, heap_id );
}

static void IndependentPokePalTrans( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  BOOL rc;
  void*	src;
  u32		dst;

  u16* raw_data = poke_wk->pal->pRawData;

  u8 i;

  s16 r_e = ( (poke_wk->pal_color) & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
  s16 g_e = ( (poke_wk->pal_color) & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
  s16 b_e = ( (poke_wk->pal_color) & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

  for( i=1; i<16; i++ )
  {
    s16 r_s = ( raw_data[i] & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
    s16 g_s = ( raw_data[i] & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
    s16 b_s = ( raw_data[i] & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

#ifndef DEF_SPEED_UP_A
    s16 r = (s16)( r_s + (r_e - r_s) * ( poke_wk->pal_rate ) / 31.0f );
    s16 g = (s16)( g_s + (g_e - g_s) * ( poke_wk->pal_rate ) / 31.0f );
    s16 b = (s16)( b_s + (b_e - b_s) * ( poke_wk->pal_rate ) / 31.0f );
    r = MATH_CLAMP( r, 0, 31 );
    g = MATH_CLAMP( g, 0, 31 );
    b = MATH_CLAMP( b, 0, 31 );
#else
    s16 r;
    s16 g;
    s16 b;
    if( poke_wk->pal_rate == 31 )
    {
      r = r_e;
      g = g_e;
      b = b_e;
    }
    else
    {
      r = r_s + ( ( (r_e - r_s) *  poke_wk->pal_rate ) >> 5 );
      g = g_s + ( ( (g_e - g_s) *  poke_wk->pal_rate ) >> 5 );
      b = b_s + ( ( (b_e - b_s) *  poke_wk->pal_rate ) >> 5 );
      r = MATH_CLAMP( r, 0, 31 );
      g = MATH_CLAMP( g, 0, 31 );
      b = MATH_CLAMP( b, 0, 31 );
    }
#endif

    poke_wk->pal_curr[i] = GX_RGB(r, g, b);
  }

  src = poke_wk->pal_curr;
  dst = poke_wk->pal_adr;
  rc = NNS_GfdRegisterNewVramTransferTask(NNS_GFD_DST_3D_TEX_PLTT, dst, src, 32);
  GF_ASSERT(rc);
}

static BOOL IndependentPokePalIsColor( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  if( poke_wk->pal_state == INDEPENDENT_PAL_STATE_COLOR ) return TRUE;
  return FALSE;
}
static BOOL IndependentPokePalIsStartColorToWhite( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  if( poke_wk->pal_state == INDEPENDENT_PAL_STATE_START_COLOR_TO_WHITE ) return TRUE;
  return FALSE;
}
static void IndependentPokePalStartColorToWhite( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  poke_wk->pal_state =INDEPENDENT_PAL_STATE_COLOR_TO_WHITE;
  IndependentPokePalStart( poke_wk, heap_id,
      0x7fff, 0, 31, 0, 1 );
}
static BOOL IndependentPokePalIsWhite( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  if( poke_wk->pal_state == INDEPENDENT_PAL_STATE_WHITE ) return TRUE;
  return FALSE;
}
static BOOL IndependentPokePalIsStartWhiteToColor( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  if( poke_wk->pal_state == INDEPENDENT_PAL_STATE_START_WHITE_TO_COLOR ) return TRUE;
  return FALSE;
}
static void IndependentPokePalStartWhiteToColor( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  poke_wk->pal_state =INDEPENDENT_PAL_STATE_WHITE_TO_COLOR;
  IndependentPokePalStart( poke_wk, heap_id,
      0x7fff, 31, 0, 0, -1 );
}
static void IndependentPokePalMain( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  switch( poke_wk->pal_state )
  {
  case INDEPENDENT_PAL_STATE_COLOR:
    {
    }
    break;
  case INDEPENDENT_PAL_STATE_START_COLOR_TO_WHITE:
    {
      // �O����ύX��������̂�҂��Ă���
    }
    break;
  case INDEPENDENT_PAL_STATE_COLOR_TO_WHITE:
    {
      if( IndependentPokePalIsEnd( poke_wk, heap_id ) )
      {
        poke_wk->pal_state = INDEPENDENT_PAL_STATE_WHITE;
      }
    }
    break;
  case INDEPENDENT_PAL_STATE_WHITE:
    {
    }
    break;
  case INDEPENDENT_PAL_STATE_START_WHITE_TO_COLOR:
    {
      // �O����ύX��������̂�҂��Ă���
    }
    break;
  case INDEPENDENT_PAL_STATE_WHITE_TO_COLOR:
    {
      if( IndependentPokePalIsEnd( poke_wk, heap_id ) )
      {
        poke_wk->pal_state = INDEPENDENT_PAL_STATE_COLOR;
      }
    }
    break;
  }

  IndependentPokePalUpdate( poke_wk, heap_id );
}


// INDEPENDENT_ONE
static INDEPENDENT_ONE_WORK* IndependentOneInit( u32 tex_adr, u32 pal_adr, HEAPID heap_id )
{
  INDEPENDENT_ONE_WORK* wk = GFL_HEAP_AllocClearMemory( heap_id, sizeof(INDEPENDENT_ONE_WORK) );

  wk->tex_adr = tex_adr;
  wk->pal_adr = pal_adr;

  wk->pos.x = wk->pos.y = wk->pos.z = 0;
 
  wk->scale.x = FX_F32_TO_FX32(INDEPENDENT_PANEL_TOTAL_W);
  wk->scale.y = FX_F32_TO_FX32(INDEPENDENT_PANEL_TOTAL_H);
  wk->scale.z = FX_F32_TO_FX32(1.0f);

  wk->polygon_id = INDEPENDENT_ONE_POLYGON_ID;

  wk->s0 = FX_F32_TO_FX32(INDEPENDENT_PANEL_TOTAL_MIN_S);
  wk->t0 = FX_F32_TO_FX32(INDEPENDENT_PANEL_TOTAL_MIN_T);
  wk->s1 = FX_F32_TO_FX32(INDEPENDENT_PANEL_TOTAL_MAX_S);
  wk->t1 = FX_F32_TO_FX32(INDEPENDENT_PANEL_TOTAL_MAX_T);

  wk->alpha = 31;

  wk->vtx[0].x = - FX16_HALF;    wk->vtx[0].y =   FX16_HALF;    wk->vtx[0].z = 0;
  wk->vtx[1].x = - FX16_HALF;    wk->vtx[1].y = - FX16_HALF;    wk->vtx[1].z = 0;
  wk->vtx[2].x =   FX16_HALF;    wk->vtx[2].y = - FX16_HALF;    wk->vtx[2].z = 0;
  wk->vtx[3].x =   FX16_HALF;    wk->vtx[3].y =   FX16_HALF;    wk->vtx[3].z = 0;

  wk->not_disp_y = 0;

  return wk;
}
static void IndependentOneExit( INDEPENDENT_ONE_WORK* one_wk, HEAPID heap_id )
{
  GFL_HEAP_FreeMemory( one_wk );
}
static void IndependentOneMain( INDEPENDENT_ONE_WORK* one_wk, HEAPID heap_id )
{
/*
  // ����Z����̂��C���Ȃ̂ŁA�e�[�u���ɂ��Ă���
  static const fx16 independent_one_top_vtx_y[INDEPENDENT_PANEL_NUM_Y] =  // 0.5 - 1/INDEPENDENT_PANEL_NUM_Y * i
  {
    FX16_HALF,                                                   //  0  //  0.5
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y *  1 ),  //  1  //  0.4444444
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y *  2 ),  //  2  //  0.3888888
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y *  3 ),  //  3  //  0.3333333
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y *  4 ),  //  4  //  0.2777777
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y *  5 ),  //  5  //  0.2222222
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y *  6 ),  //  6  //  0.1666666
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y *  7 ),  //  7  //  0.1111111
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y *  8 ),  //  8  //  0.0555555
    0,                                                           //  9  //  0
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y * 10 ),  // 10  // -0.0555555  
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y * 11 ),  // 11  // -0.1111111
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y * 12 ),  // 12  // -0.1666666
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y * 13 ),  // 13  // -0.2222222
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y * 14 ),  // 14  // -0.2777777
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y * 15 ),  // 15  // -0.3333333
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y * 16 ),  // 16  // -0.3888888
    FX_F32_TO_FX16( 0.5f - 1.0f/INDEPENDENT_PANEL_NUM_Y * 17 ),  // 17  // -0.4444444
  };
�ł��e�[�u���̓v���O�����T�C�Y���傫���Ȃ�̂Ŏ~�߂��B
*/

/*
  const fx16 panel_h = FX_F32_TO_FX16( 0.055555555555f );  // 1/INDEPENDENT_PANEL_NUM_Y = 1/18
  const fx32 tex_h   = FX_F32_TO_FX32( 5.333333333333f );  // ( INDEPENDENT_PANEL_TOTAL_MAX_T - INDEPENDENT_PANEL_TOTAL_MIN_T ) / INDEPENDENT_PANEL_NUM_Y = ( 112 - 16 ) / 18 = 96/18

  one_wk->vtx[0].y = one_wk->vtx[3].y = FX16_HALF - panel_h * one_wk->not_disp_y;
  one_wk->t0 = FX32_CONST( INDEPENDENT_PANEL_TOTAL_MIN_T ) + tex_h * one_wk->not_disp_y;
�摜���J�^�J�^�����B�����e�N�X�`�����W���v�Z���ʂ����̂܂ܗ��p���Ă���Ə�≺�ւ���Ă���̂��낤�B
*/ 

  // �s�N�Z���������ɂȂ�悤�ȃe�[�u��
  static const fx16 independent_one_top_vtx_y[INDEPENDENT_PANEL_NUM_Y +1] =
  {
    FX16_HALF,                        //  0
    FX_F32_TO_FX16(  0.447916667f ),  //  1
    FX_F32_TO_FX16(  0.395833333f ),  //  2
    FX_F32_TO_FX16(  0.333333333f ),  //  3
    FX_F32_TO_FX16(  0.28125f     ),  //  4
    FX_F32_TO_FX16(  0.229166667f ),  //  5
    FX_F32_TO_FX16(  0.166666667f ),  //  6
    FX_F32_TO_FX16(  0.114583333f ),  //  7
    FX_F32_TO_FX16(  0.0625f      ),  //  8
    0,                                //  9
    FX_F32_TO_FX16( -0.052083333f ),  // 10 
    FX_F32_TO_FX16( -0.104166667f ),  // 11
    FX_F32_TO_FX16( -0.166666667f ),  // 12
    FX_F32_TO_FX16( -0.21875f     ),  // 13
    FX_F32_TO_FX16( -0.270833333f ),  // 14
    FX_F32_TO_FX16( -0.333333333f ),  // 15
    FX_F32_TO_FX16( -0.385416667f ),  // 16
    FX_F32_TO_FX16( -0.4375f      ),  // 17
    - FX16_HALF,                      // 18
  };

  // �����ɂ����s�N�Z���ɍ��킹���e�N�X�`�����W�e�[�u��
  static const u8 independent_one_t0[INDEPENDENT_PANEL_NUM_Y +1] =
  {
     16,  //  0
     21,  //  1
     26,  //  2
     32,  //  3
     37,  //  4
     42,  //  5
     48,  //  6
     53,  //  7
     58,  //  8
     64,  //  9
     69,  // 10 
     74,  // 11
     80,  // 12
     85,  // 13
     90,  // 14
     96,  // 15
    101,  // 16
    106,  // 17
    112,  // 18
  };

  one_wk->vtx[0].y = one_wk->vtx[3].y = independent_one_top_vtx_y[one_wk->not_disp_y];
  one_wk->t0 = FX32_CONST( independent_one_t0[one_wk->not_disp_y] );
//�ł��e�[�u���̓v���O�����T�C�Y���傫���Ȃ�̂ŁA�ŏ����ɗ��߂����L��independent_one_pixel���g�p���邱�Ƃɂ����B

/*
  // �����s�N�Z���̃e�[�u��
  static const u8 independent_one_pixel[INDEPENDENT_PANEL_NUM_Y +1] =  // 5.333�𐮐��ɂ����̂�5�s�N�Z���i�ނƂ���6�s�N�Z���i�ނƂ�������B
  {
      0,  //  0
      5,  //  1
     10,  //  2
     16,  //  3
     21,  //  4
     26,  //  5
     32,  //  6
     37,  //  7
     42,  //  8
     48,  //  9
     53,  // 10 
     58,  // 11
     64,  // 12
     69,  // 13
     74,  // 14
     80,  // 15
     85,  // 16
     90,  // 17
     96,  // 18  // INDEPENDENT_PANEL_TOTAL_MAX_T - INDEPENDENT_PANEL_TOTAL_MIN_T = 112 - 16 = 96
  };

  const fx16 panel_pixel = FX_F32_TO_FX16( 0.010416666666f );  // 1/(INDEPENDENT_PANEL_TOTAL_MAX_T - INDEPENDENT_PANEL_TOTAL_MIN_T) = 1/(112-16) = 1/96

  one_wk->vtx[0].y = one_wk->vtx[3].y = FX16_HALF - panel_pixel * independent_one_pixel[one_wk->not_disp_y];
  one_wk->t0 = FX32_CONST( INDEPENDENT_PANEL_TOTAL_MIN_T + independent_one_pixel[one_wk->not_disp_y] );
���ꂾ�ƌ㔼���������Ƃ��낪�o�Ă��܂��B��͂�v�Z�����ɑS�e�[�u������L�̂悤�Ɏ������Ȃ����B
*/
}
static void IndependentOneDraw( INDEPENDENT_ONE_WORK* one_wk, HEAPID heap_id )
{
  if( one_wk->not_disp_y >= INDEPENDENT_PANEL_NUM_Y ) return;
  
  {
    G3_TexImageParam(
        GX_TEXFMT_PLTT16,
        GX_TEXGEN_TEXCOORD,
        GX_TEXSIZE_S128,
        GX_TEXSIZE_T128,
        GX_TEXREPEAT_ST,
        GX_TEXFLIP_NONE,
        GX_TEXPLTTCOLOR0_TRNS,//GX_TEXPLTTCOLOR0_TRNS,//GX_TEXPLTTCOLOR0_USE,
        one_wk->tex_adr );

    G3_TexPlttBase( one_wk->pal_adr, GX_TEXFMT_PLTT16 );
  }

  {
    GXCull cull; 
    
    G3_PushMtx();

    // �|���S���A�g���r���[�g�ݒ�
    if( one_wk->alpha == 0 )  // �A���t�@0�̂܂ܕ`�悵�Ă��܂��ƁA���C���[�t���[���ŕ\������Ă��܂��̂ŁB
    {
      cull = GX_CULL_ALL;
    }
    else
    {
      cull = GX_CULL_BACK;
    }

    G3_PolygonAttr(
        GX_LIGHTMASK_NONE,//GX_LIGHTMASK_0,			  // ���C�g�𔽉f  // ���C�g�Ȃ��p�̐ݒ�GX_LIGHTMASK_NONE
        GX_POLYGONMODE_MODULATE,	  // ���W�����[�V�����|���S�����[�h
        cull,             // �J�����O
        one_wk->polygon_id,                         // �|���S���h�c �O
        one_wk->alpha,					  // �A���t�@�l  // �p�[�e�B�N������O�ɕ`�悷��ɂ͔������ɂ��邵���肪�Ȃ��E�E�E
        GX_POLYGON_ATTR_MISC_NONE );

    // �ʒu�ݒ�
    G3_Translate(one_wk->pos.x, one_wk->pos.y, one_wk->pos.z);
    // �X�P�[���ݒ�
    G3_Scale(one_wk->scale.x, one_wk->scale.y, one_wk->scale.z);

    G3_Begin( GX_BEGIN_QUADS );

    G3_Normal( 0, 0, FX32_ONE );

    G3_TexCoord(one_wk->s0, one_wk->t0);
    G3_Vtx( one_wk->vtx[0].x, one_wk->vtx[0].y, one_wk->vtx[0].z );
    G3_TexCoord(one_wk->s0, one_wk->t1);
    G3_Vtx( one_wk->vtx[1].x, one_wk->vtx[1].y, one_wk->vtx[1].z );
    G3_TexCoord(one_wk->s1, one_wk->t1);
    G3_Vtx( one_wk->vtx[2].x, one_wk->vtx[2].y, one_wk->vtx[2].z );
    G3_TexCoord(one_wk->s1, one_wk->t0);
    G3_Vtx( one_wk->vtx[3].x, one_wk->vtx[3].y, one_wk->vtx[3].z );

    G3_End();

    G3_PopMtx(1);
  }
}

static void IndependentOneSetPosXYZ( INDEPENDENT_ONE_WORK* one_wk, HEAPID heap_id, fx32 pos_x, fx32 pos_y, fx32 pos_z )
{
  one_wk->pos.x = pos_x;
  one_wk->pos.y = pos_y;
  one_wk->pos.z = pos_z;
}

