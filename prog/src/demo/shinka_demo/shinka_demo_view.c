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


// �|�P������Y���W
#define POKE_Y  (FX_F32_TO_FX32(-24.0f))


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
    // �����ňꎞ�I�ɐ�������
    work->after_pp = PP_Create( 1, 1, 0, work->heap_id );
    POKETOOL_CopyPPtoPP( (POKEMON_PARAM*)(work->pp), work->after_pp );
    PP_ChangeMonsNo( work->after_pp, work->after_monsno );  // �i��
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
  // MCSS
  {
    ShinkaDemo_View_PokeExit( work );
    ShinkaDemo_View_McssExit( work );
  }

  // �����ňꎞ�I�ɐ����������̂�j������
  {
    GFL_HEAP_FreeMemory( work->after_pp );
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
  MCSS_Draw( work->mcss_sys_wk );
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
      work->poke_set[i].wk = MCSS_Add( work->mcss_sys_wk, 0, POKE_Y, 0, &add_wk );
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
    work->poke_set[i].wk = MCSS_Add( work->mcss_sys_wk, 0, POKE_Y, 0, &add_wk );
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
/*
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
      ofs_y = ( POKE_SIZE_MAX - size_y ) / 2.0f;
      ofs.x = 0;  ofs.y = FX_F32_TO_FX32(ofs_y);  ofs.z = 0;
      MCSS_SetOfsPosition( work->poke_set[i].wk, &ofs );
    }
  }
*/
}

