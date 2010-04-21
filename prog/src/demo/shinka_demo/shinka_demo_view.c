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
  STEP_EVO_CRY_START,                // ���J�n�҂�������
  STEP_EVO_CRY,                      // ����
  STEP_EVO_CHANGE_SATRT,             // �i���J�n�҂������i��
  STEP_EVO_CHANGE_TO_WHITE_START,    // �i����  // ��������
  STEP_EVO_CHANGE_TO_WHITE,          // �i����  // �����Ȃ蒆
  STEP_EVO_CHANGE_TRANSFORM,         // �i����  // �ό`
  STEP_EVO_CHANGE_CANCEL,            // �i����  // �L�����Z��
  STEP_EVO_CHANGE_FROM_WHITE_START,  // �i����  // �F�t���ɖ߂�
  STEP_EVO_CHANGE_FROM_WHITE,        // �i����  // �F�t���ɖ߂蒆
  STEP_EVO_CHANGE_CRY_START,         // �i����  // ���J�n�҂���
  STEP_EVO_CHANGE_CRY,               // �i����  // ����
  STEP_EVO_END,                      // �I����

  // AFTER
  STEP_AFTER,                       // �i���ォ��X�^�[�g
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

// �g��k������ւ����o
#define ANM_ADD_START        (0x400)
#define ANM_VAL_MAX          (0x10000)

  static const int anm_add_tbl[] =  // ANM_VAL_MAX/2 ��菬���Ȓl��
  {                    // disp_poke
    ANM_ADD_START,     // BEFORE
    0x1000,
    0x1000,            // AFTER
    0x400,
    0x400,             // BEFORE
    0x1000,
    0x1000,            // AFTER
    0x400,
    0x400,             // BEFORE
    0x1000,
    0x1000,            // AFTER
    0x400,

    0x800,             // BEFORE
    0x1000,
    0x1000,            // AFTER
    0x800,
    0x800,             // BEFORE
    0x1000,
    0x1000,            // AFTER
    0x800,

    0x1000,            // BEFORE
    0x1000,
    0x1000,            // AFTER
    0x1000,
    0x1000,            // BEFORE
    0x1000,
    0x1000,            // AFTER
    0x1000,
    0x1000,            // BEFORE
    0x1000,
    0x1000,            // AFTER
    0x1000,
 
    0x1000,            // BEFORE
    0x800,
    0x800,             // AFTER
    0x1000,

    0x1000,            // BEFORE  // ANM_REPEAT_MAX-6
    0x400,
    0x400,             // AFTER   // ANM_REPEAT_MAX-4
    0x1000,
    0x1000,            // BEFORE  // ANM_REPEAT_MAX-2
    0x400,
  };
#define ANM_REPEAT_MAX       ( sizeof(anm_add_tbl) / sizeof(anm_add_tbl[0]) )

#define SCALE_MAX            (16.0f)
#define SCALE_MIN            ( 4.0f)
#define POKE_SIZE_MAX        (96.0f)

// �p�[�e�B�N���Ή�
#define POKE_TEX_ADRS_FOR_PARTICLE (0x30000)


// �|�P������X,Y���W
#define POKE_X  (FX_F32_TO_FX32(256.0f))  // �����Ȃ��ʒu�ɒu���Ă����B�{���͉��̃R�����g�A�E�g����Ă���l�B
//#define POKE_X  (FX_F32_TO_FX32(-3.0f))
#define POKE_Y  (FX_F32_TO_FX32(-16.0f))

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
// �S�������킹��1���ƌ����Ƃ��̃T�C�Y
#define INDEPENDENT_PANEL_TOTAL_W  (36)
#define INDEPENDENT_PANEL_TOTAL_H  (36)
// �S�������킹��1���ƌ����Ƃ��̍ŏ��ő�̃e�N�X�`�����W
#define INDEPENDENT_PANEL_TOTAL_MIN_S  ( 16)
#define INDEPENDENT_PANEL_TOTAL_MAX_S  (112)
#define INDEPENDENT_PANEL_TOTAL_MIN_T  ( 16)
#define INDEPENDENT_PANEL_TOTAL_MAX_T  (112)

// �|���S��ID
#define INDEPENDENT_BEFORE_POLYGON_ID  (60)
#define INDEPENDENT_AFTER_POLYGON_ID   (61)

typedef enum
{
  // �i������
  INDEPENDENT_STATE_EVO_START,
  INDEPENDENT_STATE_EVO_DEMO,
  INDEPENDENT_STATE_EVO_CANCEL,
  INDEPENDENT_STATE_EVO_SUCCESS,
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

#define INDEPENDENT_SPIRAL_ZX_FIRST_VEL   (1)  // ����
#define INDEPENDENT_SPIRAL_Y_FIRST_VEL    (1)

#define INDEPENDENT_SPIRAL_PREV_RATE      (1)   // �O�̓������ǂꂭ�炢�̊����c����
#define INDEPENDENT_SPIRAL_CURR_RATE      (9)   // ����̓������ǂꂭ�炢�̊����o����

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

  u16      change_no;     // POKE_BEFORE��POKE_AFTER�̓���ւ��ς݂�panel_wk�̃C���f�b�N�X(�����̃C���f�b�N�X�������ւ���)
                          // (y=change_no/INDEPENDENT_PANEL_NUM_X, x=change_no%INDEPENDENT_PANEL_NUM_X)
  INDEPENDENT_SPIRAL_STATE  spi_state;      // �S�̗̂����̏��
  int                       spi_theta_idx;  // 0<= <2PI�̃C���f�b�N�X �őS�̂𗆐��ŉ񂷂���
  int                       spi_theta_idx_add;  // spi_theta_idx�̑�����
  u32                       spi_count;

  // �p�l���X
  INDEPENDENT_PANEL_WORK  panel_wk[INDEPENDENT_PANEL_NUM_Y][INDEPENDENT_PANEL_NUM_X];
}
INDEPENDENT_POKE_WORK;

typedef struct
{
  INDEPENDENT_STATE       state;
  INDEPENDENT_POKE_WORK*  poke_wk[POKE_MAX];  // NULL�̂Ƃ��͂Ȃ�
}
INDEPENDENT_POKE_MANAGER;


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
  BOOL                     b_cry_start;      // �i���O�̖��X�^�[�g���Ă����Ƃ���TRUE
  BOOL                     b_cry_end;        // �i���O�̖����������Ă�����TRUE
  BOOL                     b_change_start;   // �i���X�^�[�g���Ă����Ƃ���TRUE
  BOOL                     b_change_end;     // �i�����������Ă�����TRUE
  BOOL                     b_change_bgm_shinka_start;    // BGM SHINKA�Ȃ��J�n���Ă��悢��
  BOOL                     b_change_bgm_shinka_push;     // BGM SHINKA�Ȃ�push���Ă��悢��
  BOOL                     b_change_cancel;  // �i���L�����Z�����Ă�����TRUE

  // �g��k������ւ����o
  f32                      scale;
  u8                       disp_poke;
  int                      anm_val;
  int                      anm_add;
  u8                       count;

  // MCSS
  MCSS_SYS_WORK*           mcss_sys_wk;
  POKE_SET                 poke_set[POKE_MAX];

  // ����|���S���|�P����
  INDEPENDENT_POKE_MANAGER*  independent_poke_mgr;
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

static BOOL ShinkaDemo_View_Evolve( SHINKADEMO_VIEW_WORK* work );
static void ShinkaDemo_View_AdjustPokePos( SHINKADEMO_VIEW_WORK* work );

//-------------------------------------
/// ����|���S���|�P����
//=====================================
static void IndependentPokeManagerInit( SHINKADEMO_VIEW_WORK* work );
static void IndependentPokeManagerExit( SHINKADEMO_VIEW_WORK* work );
static void IndependentPokeManagerMain( SHINKADEMO_VIEW_WORK* work );
static void IndependentPokeManagerDraw( SHINKADEMO_VIEW_WORK* work );

static void IndependentPokeManagerStart( SHINKADEMO_VIEW_WORK* work );
static void IndependentPokeManagerCancel( SHINKADEMO_VIEW_WORK* work );
static void IndependentPokeManagerSuccess( SHINKADEMO_VIEW_WORK* work );

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
      work->after_pp = PP_Create( 1, 1, 0, work->heap_id );
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

    work->anm_val    = 0;
    work->anm_add    = ANM_ADD_START;
    work->count      = 0;
  }

  // MCSS
  {
    ShinkaDemo_View_McssInit( work );
    ShinkaDemo_View_PokeInit( work );
  }

  IndependentPokeManagerInit( work );

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
        work->step = STEP_EVO_CHANGE_SATRT;
        
        work->b_cry_end = TRUE;
      }
    }
    break;
  case STEP_EVO_CHANGE_SATRT:
    {
      if( work->b_change_start )
      {
        // ����
        work->step = STEP_EVO_CHANGE_TO_WHITE_START;

        MCSS_SetPaletteFade( work->poke_set[work->disp_poke].wk, 0, 16, 3, 0x7fff );
        MCSS_SetPaletteFade( work->poke_set[NotDispPoke(work->disp_poke)].wk, 16, 16, 0, 0x7fff );

        IndependentPokeManagerStart( work );
      }
    }
    break;
  case STEP_EVO_CHANGE_TO_WHITE_START:
    {
      // ����
      work->step = STEP_EVO_CHANGE_TO_WHITE;

      MCSS_SetPaletteFade( work->poke_set[work->disp_poke].wk, 0, 16, 3, 0x7fff );
      MCSS_SetPaletteFade( work->poke_set[NotDispPoke(work->disp_poke)].wk, 16, 16, 0, 0x7fff );
    }
    break;
  case STEP_EVO_CHANGE_TO_WHITE:
    {
      if( !MCSS_CheckExecutePaletteFade( work->poke_set[work->disp_poke].wk ) )
      {
        // ����
        work->step = STEP_EVO_CHANGE_TRANSFORM;

        work->b_change_bgm_shinka_start = TRUE;
      }
    }
    break;
  case STEP_EVO_CHANGE_TRANSFORM:
    {
      if( ShinkaDemo_View_Evolve( work ) )
      {
        // ����
        work->step = STEP_EVO_CHANGE_FROM_WHITE_START;

        work->b_change_bgm_shinka_push = TRUE;
      } 
    }
    break;
  case STEP_EVO_CHANGE_CANCEL:
    {
      if( ShinkaDemo_View_Evolve( work ) )
      {
        // ����
        work->step = STEP_EVO_CHANGE_FROM_WHITE_START;

        work->b_change_bgm_shinka_push = TRUE;
      }
    }
    break;
  case STEP_EVO_CHANGE_FROM_WHITE_START:
    {
      // ����
      work->step = STEP_EVO_CHANGE_FROM_WHITE;

      MCSS_SetPaletteFade( work->poke_set[work->disp_poke].wk, 16, 0, 1, 0x7fff );

      if(work->b_change_cancel)
      {
        IndependentPokeManagerCancel( work );
      }
      else
      {
        IndependentPokeManagerSuccess( work );
      }
    }
    break;
  case STEP_EVO_CHANGE_FROM_WHITE:
    {
      if( !MCSS_CheckExecutePaletteFade( work->poke_set[work->disp_poke].wk ) )
      {
        // ����
        work->step = STEP_EVO_CHANGE_CRY_START;
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
      if( !PMV_CheckPlay() )
      {
        // ����
        work->step = STEP_EVO_END;

        work->b_change_end = TRUE;
      }
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

  MCSS_Main( work->mcss_sys_wk );

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
  MCSS_Draw( work->mcss_sys_wk );

  IndependentPokeManagerDraw( work );
#else
  ������ɂ���ƁAIndependentPokeManagerDraw�̊G���w�i�ɂ��ė���Ă��܂�
  IndependentPokeManagerDraw( work );

  MCSS_Draw( work->mcss_sys_wk );
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
  if( work->step == STEP_EVO_CHANGE_TRANSFORM )
  {
    if( work->count <= ANM_REPEAT_MAX-3 )
    {
      work->b_change_cancel = TRUE;
      work->step = STEP_EVO_CHANGE_CANCEL;
      return TRUE;
    }
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
static void ShinkaDemo_View_PokeInit( SHINKADEMO_VIEW_WORK* work )
{
  VecFx32 scale;

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
    MCSS_ADD_WORK add_wk;

    for(i=0; i<POKE_MAX; i++)
    {
      if(i == POKE_BEFORE)
      {
        MCSS_TOOL_MakeMAWPP( work->pp, &add_wk, MCSS_DIR_FRONT );
      }
      else
      {
        MCSS_TOOL_MakeMAWPP( work->after_pp, &add_wk, MCSS_DIR_FRONT );
      }
      work->poke_set[i].wk = MCSS_Add( work->mcss_sys_wk, POKE_X, POKE_Y, 0, &add_wk );
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
      }
    } 
  }
  else
  {
    u8 i;
    MCSS_ADD_WORK add_wk;

    i = POKE_AFTER;

    MCSS_TOOL_MakeMAWPP( work->pp, &add_wk, MCSS_DIR_FRONT );
    work->poke_set[i].wk = MCSS_Add( work->mcss_sys_wk, POKE_X, POKE_Y, 0, &add_wk );
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
/// �|�P�����̐i�����o
//=====================================
static BOOL ShinkaDemo_View_Evolve( SHINKADEMO_VIEW_WORK* work )
{
  BOOL b_finish = FALSE;
  int prev_anm_val = work->anm_val;

  // �A�j���[�V������i�߂�
  work->anm_val += work->anm_add;
  
  // ���傤�ǈ���I�������Ƃ�
  if( work->anm_val >= ANM_VAL_MAX )
  {
    work->count++;
    if(
           ( work->count >= ANM_REPEAT_MAX )
        || ( work->b_change_cancel && work->disp_poke == POKE_AFTER )
    )
    {
      work->anm_add = ANM_ADD_START;
      work->anm_val = 0;
      
      b_finish = TRUE;
    }
    else
    {
      work->anm_add = anm_add_tbl[work->count];
      work->anm_val -= ANM_VAL_MAX;
    }

    // disp_poke��NotDispPoke�����ւ��A�VNotDispPoke���\���ɂ���
    work->disp_poke = NotDispPoke( work->disp_poke );
    MCSS_SetVanishFlag( work->poke_set[NotDispPoke(work->disp_poke)].wk );

    // �L�����Z���������Ă���̂ɐi�����Ă��܂�Ȃ��悤�ɁA�O�̂���
    if( b_finish && work->b_change_cancel )
    {
      work->disp_poke = POKE_BEFORE;
      MCSS_ResetVanishFlag( work->poke_set[work->disp_poke].wk );
      MCSS_SetVanishFlag( work->poke_set[NotDispPoke(work->disp_poke)].wk );
    }
  }
  // ���傤�ǔ����I�������Ƃ�
  else if( prev_anm_val < ANM_VAL_MAX/2 && work->anm_val >= ANM_VAL_MAX/2 )
  {
    work->count++;
    work->anm_add = anm_add_tbl[work->count];

    // disp_poke���\���ɂ���
    MCSS_SetVanishFlag( work->poke_set[work->disp_poke].wk );  // alpha�͂��ꂢ�Ɍ����Ȃ��̂ł�߂Ă���(�������Ɍ����Ȃ����A�X�v���C�g���Ƃ̐؂�ڂ������邵(Z�ʒu�̑O��֌W�̂����H�A�J�����̂����H))
    // NotDispPoke��\������
    MCSS_ResetVanishFlag( work->poke_set[NotDispPoke(work->disp_poke)].wk );
  }

  // alpha�͂��ꂢ�Ɍ����Ȃ��̂ł�߂Ă���(�������Ɍ����Ȃ����A�X�v���C�g���Ƃ̐؂�ڂ������邵(Z�ʒu�̑O��֌W�̂����H�A�J�����̂����H))
  //        anm_val        cos01  scale         �|�P����0           �|�P����1
  //        0              1      SCALE_MAX     disp_poke   �\��    NotDispPoke ��\��
  //        |              |                    disp_poke   �\��    NotDispPoke ��\��
  //  ����  ANM_VAL_MAX/2  0      SCALE_MIN     disp_poke   �\��    NotDispPoke ��\��
  //  ���傤�ǔ����I�������Ƃ�                  disp_poke   ��\��  NotDispPoke �\��
  //        |              |                    disp_poke   ��\��  NotDispPoke �\��
  //  ���  ANM_VAL_MAX    1      SCALE_MAX     disp_poke   ��\��  NotDispPoke �\��
  //  ���傤�ǈ���I�������Ƃ�                  NotDispPoke ��\��  disp_poke   �\��
  
  // alpha����̂Ƃ�
  //        anm_val        cos01  scale         �|�P����0          a   �|�P����1          a
  //        0              1      SCALE_MAX     disp_poke   �\��   31  NotDispPoke ��\�� *
  //        |              |                    disp_poke   �\��   31  NotDispPoke ��\�� *
  //  ����  ANM_VAL_MAX/2  0      SCALE_MIN     disp_poke   �\��   31  NotDispPoke ��\�� *
  //  ���傤�ǔ����I�������Ƃ�                  disp_poke   �\��   31  NotDispPoke �\��   0
  //        |              |                    disp_poke   �\��   |   NotDispPoke �\��   |
  //        |              |                    disp_poke   �\��   |   NotDispPoke �\��   31
  //        |              |                    disp_poke   �\��   0   NotDispPoke �\��   |
  //        |              |                    disp_poke   �\��   |   NotDispPoke �\��   |
  //  ���  ANM_VAL_MAX    1      SCALE_MAX     disp_poke   �\��   0   NotDispPoke �\��   31
  //  ���傤�ǈ���I�������Ƃ�                  NotDispPoke ��\�� *   disp_poke   �\��   31

  // �X�P�[��
  {
    // 0�`1�͈̔͂ɂ����߂�cos�l
    fx16 cos01_fx16 = ( FX_CosIdx( work->anm_val ) + FX16_ONE ) /2;
    f32 cos01 = FX_FX16_TO_F32( cos01_fx16 );
    VecFx32 scale;
    u8 i;

    work->scale = ( SCALE_MAX - SCALE_MIN ) * cos01 + SCALE_MIN;

    scale.x = FX_F32_TO_FX32(work->scale);
    scale.y = FX_F32_TO_FX32(work->scale);
    scale.z = FX32_ONE;

    for(i=0; i<POKE_MAX; i++)
    {
      MCSS_SetScale( work->poke_set[i].wk, &scale );
    }
  }

  ShinkaDemo_View_AdjustPokePos(work);

  return b_finish;
}

//-------------------------------------
/// �|�P�����̈ʒu�𒲐�����
//=====================================
static void ShinkaDemo_View_AdjustPokePos( SHINKADEMO_VIEW_WORK* work )
{
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
  
  work->independent_poke_mgr = independent_poke_mgr;
}
static void IndependentPokeManagerExit( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;

  u8 i;
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
  case INDEPENDENT_STATE_EVO_CANCEL:
    {
      IndependentPokeManagerMainSpiral( work );

      independent_poke_mgr->state = INDEPENDENT_STATE_EVO_END;
    }
    break;
  case INDEPENDENT_STATE_EVO_SUCCESS:
    {
      IndependentPokeManagerMainSpiral( work );

      independent_poke_mgr->state = INDEPENDENT_STATE_EVO_END;
    }
    break;
  case INDEPENDENT_STATE_EVO_END:
    {
      IndependentPokeManagerMainSpiral( work );
    }
    break;
  case INDEPENDENT_STATE_AFTER:
    {
      // �������Ȃ�
    }
    break;
  }
}
static void IndependentPokeManagerDraw( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;

  u8 i;
  for( i=0; i<POKE_MAX; i++ )
  {
    if( independent_poke_mgr->poke_wk[i] )
    {
      IndependentPokeDraw( independent_poke_mgr->poke_wk[i], work->heap_id );
    }
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
static void IndependentPokeManagerCancel( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;
  
  if( independent_poke_mgr->state == INDEPENDENT_STATE_EVO_DEMO )
  {
    independent_poke_mgr->state = INDEPENDENT_STATE_EVO_CANCEL;
  }
}
static void IndependentPokeManagerSuccess( SHINKADEMO_VIEW_WORK* work )
{
  INDEPENDENT_POKE_MANAGER* independent_poke_mgr = work->independent_poke_mgr;
  
  if( independent_poke_mgr->state == INDEPENDENT_STATE_EVO_DEMO )
  {
    independent_poke_mgr->state = INDEPENDENT_STATE_EVO_SUCCESS;
  }
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
    cgr	= POKEGRA_GetCgrArcIndex( mons_no, form_no, sex, rare, dir, egg );
    clr = POKEGRA_GetPalArcIndex( mons_no, form_no, sex, rare, dir, egg );
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
    poke_wk->change_no = 0;
  }

  {
    poke_wk->spi_state = INDEPENDENT_SPIRAL_STATE_BEFORE_START;
    poke_wk->spi_theta_idx = 0;  // 0<= <2PI�̃C���f�b�N�X �őS�̂𗆐��ŉ񂷂���
    poke_wk->spi_theta_idx_add = 0;  // spi_theta_idx�̑�����
    poke_wk->spi_count = 0;
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
    int  i;
    int  max;
    f32  w, h;
    f32  sw, th;
    f32  start_x, start_y;

    max = INDEPENDENT_PANEL_NUM_X * INDEPENDENT_PANEL_NUM_Y;

    w = ( (f32)INDEPENDENT_PANEL_TOTAL_W ) / INDEPENDENT_PANEL_NUM_X;  // 1���̃p�l���̃T�C�Y
    h = ( (f32)INDEPENDENT_PANEL_TOTAL_H ) / INDEPENDENT_PANEL_NUM_Y;

    sw = ( (f32)( INDEPENDENT_PANEL_TOTAL_MAX_S - INDEPENDENT_PANEL_TOTAL_MIN_S ) ) / INDEPENDENT_PANEL_NUM_X;  // 1���̃p�l���̃e�N�X�`���̕�
    th = ( (f32)( INDEPENDENT_PANEL_TOTAL_MAX_T - INDEPENDENT_PANEL_TOTAL_MIN_T ) ) / INDEPENDENT_PANEL_NUM_Y;  // 1���̃p�l���̃e�N�X�`���̍���

    start_x = - INDEPENDENT_PANEL_TOTAL_W / 2.0f + w / 2.0f;  // �E����
    start_y =   INDEPENDENT_PANEL_TOTAL_H / 2.0f - h / 2.0f;  // �オ��

    for( i=0; i<max; i++ )
    {
      int x, y;
      INDEPENDENT_PANEL_WORK* panel_wk;

      x = i%INDEPENDENT_PANEL_NUM_X;
      y = i/INDEPENDENT_PANEL_NUM_X;

      panel_wk = &(poke_wk->panel_wk[y][x]);

      panel_wk->polygon_id = INDEPENDENT_BEFORE_POLYGON_ID;
      panel_wk->alpha = 31;
      
      panel_wk->vtx[0].x = FX_F32_TO_FX16( - w /2.0f );  panel_wk->vtx[0].y = FX_F32_TO_FX16(   h /2.0f );  panel_wk->vtx[0].z = 0;
      panel_wk->vtx[1].x = FX_F32_TO_FX16( - w /2.0f );  panel_wk->vtx[1].y = FX_F32_TO_FX16( - h /2.0f );  panel_wk->vtx[1].z = 0;
      panel_wk->vtx[2].x = FX_F32_TO_FX16(   w /2.0f );  panel_wk->vtx[2].y = FX_F32_TO_FX16( - h /2.0f );  panel_wk->vtx[2].z = 0;
      panel_wk->vtx[3].x = FX_F32_TO_FX16(   w /2.0f );  panel_wk->vtx[3].y = FX_F32_TO_FX16(   h /2.0f );  panel_wk->vtx[3].z = 0;

      panel_wk->pos = poke_wk->pos;
      panel_wk->pos.x += FX_F32_TO_FX32( start_x + w * x );
      panel_wk->pos.y += FX_F32_TO_FX32( start_y - h * y );

      panel_wk->s0 = ( x * sw + INDEPENDENT_PANEL_TOTAL_MIN_S ) * FX32_ONE;
      panel_wk->t0 = ( y * th + INDEPENDENT_PANEL_TOTAL_MIN_T ) * FX32_ONE;
      panel_wk->s1 = panel_wk->s0 + FX32_CONST(sw);
      panel_wk->t1 = panel_wk->t0 + FX32_CONST(th);

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

    G3_MaterialColorDiffAmb(
							GX_RGB(31, 31, 31),
							GX_RGB(31, 31, 31),
              FALSE
        );

    G3_MaterialColorSpecEmi(GX_RGB(16, 16, 16), GX_RGB(0, 0, 0), FALSE);

    //���C�g�J���[
    G3_LightColor(GX_LIGHTID_0, GX_RGB(31, 31, 31));

    {
      int max;
      max = INDEPENDENT_PANEL_NUM_X * INDEPENDENT_PANEL_NUM_Y;

      for( i=0; i<max; i++ )
      {
        int x, y;
        INDEPENDENT_PANEL_WORK* panel_wk;
        GXCull cull; 

        G3_PushMtx();

        x = i%INDEPENDENT_PANEL_NUM_X;
        y = i/INDEPENDENT_PANEL_NUM_X;

        panel_wk = &(poke_wk->panel_wk[y][x]);
        
        // �|���S���A�g���r���[�g�ݒ�
        if( panel_wk->alpha == 0 )  // �A���t�@0�̂܂ܕ`�悵�Ă��܂��ƁA���C���[�t���[���ŕ\������Ă��܂��̂ŁB
        {
          cull = GX_CULL_ALL;
        }
        else
        {
          cull = GX_CULL_BACK;
        }

	      G3_PolygonAttr(
            GX_LIGHTMASK_0,			  // ���C�g�𔽉f
            GX_POLYGONMODE_MODULATE,	  // ���W�����[�V�����|���S�����[�h
            cull,             // �J�����O
            panel_wk->polygon_id,                         // �|���S���h�c �O
            panel_wk->alpha,					  // �A���t�@�l  // �p�[�e�B�N������O�ɕ`�悷��ɂ͔������ɂ��邵���肪�Ȃ��E�E�E
            GX_POLYGON_ATTR_MISC_NONE );

        // �ʒu�ݒ�
		    G3_Translate(panel_wk->pos.x, panel_wk->pos.y, panel_wk->pos.z);
        // �X�P�[���ݒ�
		    G3_Scale(FX32_ONE, FX32_ONE, FX32_ONE);

        {
          MtxFx33 mtx;
          MTX_RotY33(&mtx, FX_SinIdx(0), FX_CosIdx(0));
          G3_MultMtx33(&mtx);
          MTX_RotZ33(&mtx, FX_SinIdx(0), FX_CosIdx(0));
          G3_MultMtx33(&mtx);
        }

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
  case INDEPENDENT_STATE_EVO_SUCCESS:
  case INDEPENDENT_STATE_EVO_END:
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
          while( poke_wk_src->change_no > poke_wk_dst->change_no )
          {
            u8 y = poke_wk_dst->change_no / INDEPENDENT_PANEL_NUM_X; 
            u8 x = poke_wk_dst->change_no % INDEPENDENT_PANEL_NUM_X; 
            INDEPENDENT_PANEL_WORK* change_panel_wk = &(poke_wk_dst->panel_wk[y][x]);
            change_panel_wk->alpha = 31;
            poke_wk_dst->change_no++;
          }
        }
      }

      // �p���b�g�A�j��
      {
        INDEPENDENT_POKE_WORK* poke_wk_src = independent_poke_mgr->poke_wk[POKE_BEFORE];
        INDEPENDENT_POKE_WORK* poke_wk_dst = independent_poke_mgr->poke_wk[POKE_AFTER];
        if( IndependentPokePalIsStartColorToWhite( poke_wk_src, heap_id ) )
        {
          IndependentPokePalStartColorToWhite( poke_wk_src, heap_id );
          IndependentPokePalStartColorToWhite( poke_wk_dst, heap_id );
        }
        if( IndependentPokePalIsStartWhiteToColor( poke_wk_src, heap_id ) )
        {
          IndependentPokePalStartWhiteToColor( poke_wk_src, heap_id );
          IndependentPokePalStartWhiteToColor( poke_wk_dst, heap_id );
        }
      }
    }
    break;
  case INDEPENDENT_STATE_EVO_CANCEL:
    {
      INDEPENDENT_POKE_WORK* poke_wk;
      u8 i, j;
      
      poke_wk = independent_poke_mgr->poke_wk[POKE_BEFORE];
      IndependentPokeInitAfter( poke_wk, heap_id );

      poke_wk = independent_poke_mgr->poke_wk[POKE_AFTER];
      for( j=0; j<INDEPENDENT_PANEL_NUM_Y; j++ )
      {
        for( i=0; i<INDEPENDENT_PANEL_NUM_X; i++ )
        {
          INDEPENDENT_PANEL_WORK* panel_wk = &(poke_wk->panel_wk[j][i]);
          panel_wk->alpha = 0;
        }
      }
    }
    break;
/*
  case INDEPENDENT_STATE_EVO_SUCCESS:
    {
      INDEPENDENT_POKE_WORK* poke_wk;
      u8 i, j;

      poke_wk = independent_poke_mgr->poke_wk[POKE_BEFORE];
      for( j=0; j<INDEPENDENT_PANEL_NUM_Y; j++ )
      {
        for( i=0; i<INDEPENDENT_PANEL_NUM_X; i++ )
        {
          INDEPENDENT_PANEL_WORK* panel_wk = &(poke_wk->panel_wk[j][i]);
          panel_wk->alpha = 0;
        }
      }

      poke_wk = independent_poke_mgr->poke_wk[POKE_AFTER];
      IndependentPokeInitAfter( poke_wk, heap_id );
    }
    break;
*/
  }

  // �p���b�g�A�j��
  {
    u8 i;
    for( i=0; i<POKE_MAX; i++ )
    {
      IndependentPokePalMain( independent_poke_mgr->poke_wk[i], heap_id );
    }
  }
}
static void IndependentPokeMainSpiral( INDEPENDENT_POKE_WORK* poke_wk, HEAPID heap_id )
{
  s16 i, j;
  
  poke_wk->spi_theta_idx += poke_wk->spi_theta_idx_add;
  poke_wk->spi_theta_idx %= 0x10000;

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
      if( poke_wk->spi_count >= 60 )
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
          int total_theta_idx = ( panel_wk->steady_theta_idx + poke_wk->spi_theta_idx ) % 0x10000;

          VecFx32 ideal_pos;

          ideal_pos.x = FX_MUL( panel_wk->steady_r, FX_CosIdx( total_theta_idx ) );
          ideal_pos.z = FX_MUL( panel_wk->steady_r, FX_SinIdx( total_theta_idx ) );
          
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

              if(    ( dis_zx_sq < FX32_CONST( INDEPENDENT_SPIRAL_ZX_START_SPEED * INDEPENDENT_SPIRAL_ZX_START_SPEED ) +INDEPENDENT_SPIRAL_E_FX32 )
                  && ( dis_y_sq < FX32_CONST( INDEPENDENT_SPIRAL_Y_START_SPEED * INDEPENDENT_SPIRAL_Y_START_SPEED ) +INDEPENDENT_SPIRAL_E_FX32 ) )
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
                if( dis_zx_sq < FX32_CONST( INDEPENDENT_SPIRAL_ZX_START_SPEED * INDEPENDENT_SPIRAL_ZX_START_SPEED ) +INDEPENDENT_SPIRAL_E_FX32 ) 
                {
                  // ZX�����͈���O���ɏ����
                  zx_finish = TRUE;
                }
                else if( dis_y_sq < FX32_CONST( INDEPENDENT_SPIRAL_Y_START_SPEED * INDEPENDENT_SPIRAL_Y_START_SPEED ) +INDEPENDENT_SPIRAL_E_FX32 )
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
                    fx32 dis_zx = FX_Sqrt( dis_zx_sq );
                    fx32 add_x = FX_Div( dir.x, dis_zx ) * INDEPENDENT_SPIRAL_ZX_START_SPEED;
                    fx32 add_z = FX_Div( dir.z, dis_zx ) * INDEPENDENT_SPIRAL_ZX_START_SPEED;

                    panel_wk->pos.x = panel_wk->pos.x \
                        + FX_Div( FX_MUL( panel_wk->pos.x - panel_wk->pos_prev.x, FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE) ), FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE+INDEPENDENT_SPIRAL_CURR_RATE) ) \
                        + FX_Div( FX_MUL( add_x, FX32_CONST(INDEPENDENT_SPIRAL_CURR_RATE) ), FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE+INDEPENDENT_SPIRAL_CURR_RATE) );
                    panel_wk->pos.z = panel_wk->pos.z \
                        + FX_Div( FX_MUL( panel_wk->pos.z - panel_wk->pos_prev.z, FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE) ), FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE+INDEPENDENT_SPIRAL_CURR_RATE) ) \
                        + FX_Div( FX_MUL( add_z, FX32_CONST(INDEPENDENT_SPIRAL_CURR_RATE) ), FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE+INDEPENDENT_SPIRAL_CURR_RATE) );
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
                    fx32 dis_y = FX_Sqrt( dis_y_sq );
                    fx32 add_y = FX_Div( dir.y, dis_y ) * INDEPENDENT_SPIRAL_Y_START_SPEED;
                    
                    panel_wk->pos.y = panel_wk->pos.y \
                        + FX_Div( FX_MUL( panel_wk->pos.y - panel_wk->pos_prev.y, FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE) ), FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE+INDEPENDENT_SPIRAL_CURR_RATE) ) \
                        + FX_Div( FX_MUL( add_y, FX32_CONST(INDEPENDENT_SPIRAL_CURR_RATE) ), FX32_CONST(INDEPENDENT_SPIRAL_PREV_RATE+INDEPENDENT_SPIRAL_CURR_RATE) );
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
        poke_wk->pal_state = INDEPENDENT_PAL_STATE_START_COLOR_TO_WHITE;
      }
    }
    break;
  case INDEPENDENT_SPIRAL_STATE_STEADY:
    {
      // ��]���x�A�b�v
      if( poke_wk->spi_count<100 )
      {
        //if( poke_wk->spi_count % 5 == 0 )
        {
          poke_wk->spi_theta_idx_add += 0x20;
        }
      }
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
          int total_theta_idx = ( panel_wk->steady_theta_idx + poke_wk->spi_theta_idx ) % 0x10000;

          panel_wk->pos.x = FX_MUL( panel_wk->steady_r, FX_CosIdx( total_theta_idx ) );
          panel_wk->pos.z = FX_MUL( panel_wk->steady_r, FX_SinIdx( total_theta_idx ) );

          panel_wk->pos.y = panel_wk->steady_pos_y; 
        }
      }

      // 1������ւ��Ă���
      if( poke_wk->spi_count >= 10 )
      {
        u16 start_change_no = poke_wk->change_no;
        u16 end_change_no = poke_wk->change_no +2;  // start_change_no<= <end_change_no
        if( end_change_no > INDEPENDENT_PANEL_NUM_X * INDEPENDENT_PANEL_NUM_Y )
        {
          end_change_no = INDEPENDENT_PANEL_NUM_X * INDEPENDENT_PANEL_NUM_Y;
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

      // ���� or �J�E���g�A�b�v
      if( poke_wk->spi_count >= 240 )
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
          int total_theta_idx = ( panel_wk->steady_theta_idx + poke_wk->spi_theta_idx ) % 0x10000;

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
              panel_wk->pos.x = FX_MUL( panel_wk->steady_r, FX_CosIdx( total_theta_idx ) );
              panel_wk->pos.z = FX_MUL( panel_wk->steady_r, FX_SinIdx( total_theta_idx ) );

              panel_wk->pos.y = panel_wk->steady_pos_y;
            }
            b_next = FALSE;
          }
          else if( panel_wk->state == INDEPENDENT_SPIRAL_INDIVIDUAL_STATE_END )
          {
            if( panel_wk->count == 0 )  // �ŏ��͊����Ŕ�яo��
            {
              panel_wk->pos.x = FX_MUL( panel_wk->steady_r, FX_CosIdx( total_theta_idx ) );
              panel_wk->pos.z = FX_MUL( panel_wk->steady_r, FX_SinIdx( total_theta_idx ) );

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

              if(    ( dis_zx_sq < FX32_CONST( INDEPENDENT_SPIRAL_ZX_END_SPEED * INDEPENDENT_SPIRAL_ZX_END_SPEED ) +INDEPENDENT_SPIRAL_E_FX32 )
                  && ( dis_y_sq < FX32_CONST( INDEPENDENT_SPIRAL_Y_END_SPEED * INDEPENDENT_SPIRAL_Y_END_SPEED ) +INDEPENDENT_SPIRAL_E_FX32 ) )
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

    s16 r = (s16)( r_s + (r_e - r_s) * ( poke_wk->pal_rate ) / 31.0f );
    s16 g = (s16)( g_s + (g_e - g_s) * ( poke_wk->pal_rate ) / 31.0f );
    s16 b = (s16)( b_s + (b_e - b_s) * ( poke_wk->pal_rate ) / 31.0f );
    r = MATH_CLAMP( r, 0, 31 );
    g = MATH_CLAMP( g, 0, 31 );
    b = MATH_CLAMP( b, 0, 31 );

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
      0x7fff, 0, 31, 0, 2 );
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

