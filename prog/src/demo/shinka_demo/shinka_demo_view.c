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
// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/mcss.h"
#include "system/mcss_tool.h"

#include "sound/pm_voice.h"

#include "shinka_demo_view.h"

// �p�[�e�B�N��
#include "arc_def.h"
#include "shinka_demo_particle.naix"


//=============================================================================
/**
 *  �萔��`
 */
//=============================================================================
// BG�t���[��
#define BG_FRAME_M_POKEMON        (GFL_BG_FRAME0_M)            // �v���C�I���e�B1

// �X�e�b�v
typedef enum
{
  // EVO
  STEP_EVO_START,             // �J�n�҂�������
  STEP_EVO_CRY_BEFORE,        // ���I���̂�҂��������΂��҂�
  STEP_EVO_WAIT_BEFORE,       // ���΂��҂�������������
  STEP_EVO_WHITE,             // �����Ȃ�̂�҂������g��k������ւ����o���J�n����
  STEP_EVO_SCALE,             // �g��k������ւ����o�����F��t����
  STEP_EVO_COLOR,             // �F���t���̂�҂��������΂��҂�
  STEP_EVO_WAIT_AFTER,        // ���΂��҂�������
  STEP_EVO_CRY_AFTER,         // ���I���̂�҂������I������
  STEP_EVO_END,               // �I����

  // AFTER
  STEP_AFTER,                 // �i���ォ��X�^�[�g
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
#define ANM_REPEAT_MAX       (15)
#define SCALE_MAX            (16.0f)
#define SCALE_MIN            ( 4.0f)
#define POKE_SIZE_MAX        (96.0f)

// �p�[�e�B�N��
typedef enum
{
  PARTICLE_SPA_FILE_0,      // ARCID_SHINKA_DEMO    // NARC___particle_shinka_demo_particle_shinka_demo_spa
  PARTICLE_SPA_FILE_1,      // ARCID_SHINKA_DEMO    // NARC___particle_shinka_demo_particle_shinka_demo_dmy_spa
  PARTICLE_SPA_FILE_MAX
}
PARTICLE_SPA_FILE;

// �p�[�e�B�N���Ή�
#define POKE_TEX_ADRS_FOR_PARTICLE (0x30000)


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// �p�[�e�B�N��
//=====================================
typedef struct
{
  u16  frame;
  u8   spa_idx;
  u8   res_no;
}
PARTICLE_PLAY_DATA;

typedef struct
{
  u8             wk[PARTICLE_LIB_HEAP_SIZE];
  GFL_PTC_PTR    ptc;
  u8             res_num;
}
PARTICLE_SPA_SET;

typedef struct
{
  u16                          frame;
  u16                          data_no;
  u16                          data_num;
  const PARTICLE_PLAY_DATA*    data_tbl;
  PARTICLE_SPA_SET             spa_set[PARTICLE_SPA_FILE_MAX];
  BOOL                         play;  // TRUE�̂Ƃ����s��
  s32                          stop_count;  // stop_count>=0�̂Ƃ��J�E���g�_�E�����Astop_count==0�ɂȂ�������s���̂��̂�S�ď�������
}
PARTICLE_MANAGER;

static const PARTICLE_PLAY_DATA particle_play_data_tbl[] =
{
  {    0,     PARTICLE_SPA_FILE_0,         0 },
  {   40,     PARTICLE_SPA_FILE_0,         1 },
  {   80,     PARTICLE_SPA_FILE_0,         2 },
  {  120,     PARTICLE_SPA_FILE_0,         3 },
  {  160,     PARTICLE_SPA_FILE_0,         4 },
  {  200,     PARTICLE_SPA_FILE_0,         5 },
  {  240,     PARTICLE_SPA_FILE_0,         6 },
  {  260,     PARTICLE_SPA_FILE_0,         7 },
  {  280,     PARTICLE_SPA_FILE_0,         8 },
  {  300,     PARTICLE_SPA_FILE_0,         9 },
  {  320,     PARTICLE_SPA_FILE_0,        10 },
  {  340,     PARTICLE_SPA_FILE_0,        11 },
  {  360,     PARTICLE_SPA_FILE_0,        12 },
};


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

  // �����ňꎞ�I�ɐ�������
  POKEMON_PARAM*           after_pp;

  // �X�e�b�v
  STEP                     step;

  u32                      voicePlayerIdx;
  u32                      wait_count;

  // �t���O
  BOOL                     b_start;
  BOOL                     b_cancel;  // �i���L�����Z��
  BOOL                     b_white;

  // �g��k������ւ����o
  f32                      scale;
  u8                       disp_poke;
  int                      anm_val;
  int                      anm_add;
  u8                       count;

  // MCSS
  MCSS_SYS_WORK*           mcss_sys_wk;
  POKE_SET                 poke_set[POKE_MAX];

  // �p�[�e�B�N��
  PARTICLE_MANAGER*        particle_mgr;
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
/// �p�[�e�B�N��
//=====================================
static PARTICLE_MANAGER* Particle_Init( HEAPID heap_id );
static void Particle_Exit( PARTICLE_MANAGER* mgr );
static void Particle_Main( PARTICLE_MANAGER* mgr );
static void Particle_Draw( PARTICLE_MANAGER* mgr );
static void Particle_Start( PARTICLE_MANAGER* mgr );
static void Particle_Stop( PARTICLE_MANAGER* mgr, s32 stop_count );


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

  // �����ňꎞ�I�ɐ�������
  {
    work->after_pp = PP_Create( 1, 1, 0, work->heap_id );
    POKETOOL_CopyPPtoPP( (POKEMON_PARAM*)(work->pp), work->after_pp );
    PP_ChangeMonsNo( work->after_pp, work->after_monsno );  // �i��
  }
  
  // �X�e�b�v
  {
    if( work->launch == SHINKADEMO_VIEW_LAUNCH_EVO )
      work->step         = STEP_EVO_START;
    else
      work->step         = STEP_AFTER;
  }

  // �t���O
  {
    work->b_start      = FALSE;
    work->b_cancel     = FALSE;
    work->b_white      = FALSE;
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

  // �p�[�e�B�N��
  work->particle_mgr = Particle_Init( work->heap_id );

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
  // �p�[�e�B�N��
  Particle_Exit( work->particle_mgr );

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
  case STEP_EVO_START:
    {
      if( work->b_start )
      {
        // ����
        work->step = STEP_EVO_CRY_BEFORE;

        {
          u32 monsno  = PP_Get( work->pp, ID_PARA_monsno, NULL );
          u32 form_no = PP_Get( work->pp, ID_PARA_form_no, NULL );
          work->voicePlayerIdx = PMVOICE_Play( monsno, form_no, 64, FALSE, 0, 0, FALSE, 0 );
        }
      }
    }
    break;
  case STEP_EVO_CRY_BEFORE:
    {
      if( !PMVOICE_CheckPlay( work->voicePlayerIdx ) )
      {
        // ����
        work->step = STEP_EVO_WAIT_BEFORE;

        work->wait_count = 30;
      }
    }
    break;
  case STEP_EVO_WAIT_BEFORE:
    {
      work->wait_count--;
      if( work->wait_count == 0 )
      {
        // ����
        work->step = STEP_EVO_WHITE;
        
        MCSS_SetPaletteFade( work->poke_set[work->disp_poke].wk, 0, 16, 1, 0x7fff );
        MCSS_SetPaletteFade( work->poke_set[NotDispPoke(work->disp_poke)].wk, 16, 16, 0, 0x7fff );
      }
    }
    break;
  case STEP_EVO_WHITE:
    {
      if( !MCSS_CheckExecutePaletteFade( work->poke_set[work->disp_poke].wk ) )
      {
        // ����
        work->step = STEP_EVO_SCALE;
      }
    }
    break;
  case STEP_EVO_SCALE:
    {
      if( ShinkaDemo_View_Evolve( work ) )
      {
        // ����
        work->step = STEP_EVO_COLOR;
        
        MCSS_SetPaletteFade( work->poke_set[work->disp_poke].wk, 16, 0, 1, 0x7fff );

        work->b_white = TRUE;
      }
    }
    break;
  case STEP_EVO_COLOR:
    {
      work->b_white = FALSE;

      if( !MCSS_CheckExecutePaletteFade( work->poke_set[work->disp_poke].wk ) )
      {
        // ����
        work->step = STEP_EVO_WAIT_AFTER;

        work->wait_count = 30;
      }
    }
    break;
  case STEP_EVO_WAIT_AFTER:
    {
      work->wait_count--;
      if( work->wait_count == 0 )
      {
        // ����
        work->step = STEP_EVO_CRY_AFTER;

        {
          const POKEMON_PARAM* curr_pp = (work->b_cancel)?(work->pp):(work->after_pp);
          u32 monsno  = PP_Get( curr_pp, ID_PARA_monsno, NULL );
          u32 form_no = PP_Get( curr_pp, ID_PARA_form_no, NULL );
          work->voicePlayerIdx = PMVOICE_Play( monsno, form_no, 64, FALSE, 0, 0, FALSE, 0 );
        }
      }
    }
    break;
  case STEP_EVO_CRY_AFTER:
    {
      if( !PMVOICE_CheckPlay( work->voicePlayerIdx ) )
      {
        // ����
        work->step = STEP_EVO_END;
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

  // �p�[�e�B�N��
  Particle_Main( work->particle_mgr );
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

  // �p�[�e�B�N��
  Particle_Draw( work->particle_mgr );
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �i���L�����Z��
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval       �L�����Z���ł����Ƃ�TRUE
 */
//-----------------------------------------------------------------------------
BOOL SHINKADEMO_VIEW_CancelShinka( SHINKADEMO_VIEW_WORK* work )
{
  if( work->count <= ANM_REPEAT_MAX-2 )
  {
    work->b_cancel = TRUE;

    // �p�[�e�B�N��
    Particle_Stop( work->particle_mgr, 30 );

    return TRUE;
  }
  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �X�^�[�g
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_VIEW_StartShinka( SHINKADEMO_VIEW_WORK* work )
{
  work->b_start = TRUE;

  // �p�[�e�B�N��
  Particle_Start( work->particle_mgr );
}

//-----------------------------------------------------------------------------
/**
 *  @brief         BGM���Đ����Ă��悢��
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        BGM���Đ����Ă��悢�Ƃ�TRUE��Ԃ�
 */
//-----------------------------------------------------------------------------
BOOL SHINKADEMO_VIEW_IsBGMPlay( SHINKADEMO_VIEW_WORK* work )
{
  if(    STEP_EVO_WAIT_BEFORE <= work->step  
      && work->step < STEP_EVO_COLOR )
    return TRUE;
  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         ������΂����o�̂��߂̃p���b�g�t�F�[�h�����ė~������(1�t���[������TRUE�ɂȂ�Ȃ�)
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �p���b�g�t�F�[�h�����ė~����1�t���[���ɂ����Ă���TRUE��Ԃ�
 */
//-----------------------------------------------------------------------------
BOOL SHINKADEMO_VIEW_ToFromWhite( SHINKADEMO_VIEW_WORK* work )
{
  return work->b_white;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �I�����Ă��邩
 *
 *  @param[in,out] work  SHINKADEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �I�����Ă���Ƃ�TRUE��Ԃ�
 */
//-----------------------------------------------------------------------------
BOOL SHINKADEMO_VIEW_IsEndShinka( SHINKADEMO_VIEW_WORK* work )
{
  if( work->step >= STEP_EVO_END )
    return TRUE;
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
      work->poke_set[i].wk = MCSS_Add( work->mcss_sys_wk, 0, 0, 0, &add_wk );
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
    work->poke_set[i].wk = MCSS_Add( work->mcss_sys_wk, 0, 0, 0, &add_wk );
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
  const int anm_add_tbl[ANM_REPEAT_MAX] =  // ANM_VAL_MAX/2 ��菬���Ȓl��
  {                    // disp_poke
    ANM_ADD_START,     // BEFORE
    0x400,
    0x400,             // BEFORE
    0x800,
    0x800,             // BEFORE
    0x800,
    0x800,             // BEFORE
    0x1000,
    0x1000,            // BEFORE
    0x1000,
    0x1000,            // BEFORE
    0x1000,
    0x1000,            // BEFORE        // ANM_REPEAT_MAX-3
    0x1000,                             // ANM_REPEAT_MAX-2
    0x1000,            // BEFORE        // ANM_REPEAT_MAX-1
  };

  BOOL b_finish = FALSE;
  int prev_anm_val = work->anm_val;

  // �A�j���[�V������i�߂�
  work->anm_val += work->anm_add;
  
  // ���傤�ǈ���I�������Ƃ�
  if( work->anm_val >= ANM_VAL_MAX )
  {
    work->count++;
    if(
           ( work->count >= sizeof(anm_add_tbl)/sizeof(int) )
        || ( work->b_cancel && work->disp_poke == POKE_AFTER )
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
  }

  // ���傤�ǔ����I�������Ƃ�
  if( prev_anm_val < ANM_VAL_MAX/2 && work->anm_val >= ANM_VAL_MAX/2 )
  {
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

/*
  alpha�͂��ꂢ�Ɍ����Ȃ��̂ł�߂Ă���(�������Ɍ����Ȃ����A�X�v���C�g���Ƃ̐؂�ڂ������邵(Z�ʒu�̑O��֌W�̂����H�A�J�����̂����H))
  // �A���t�@
  if( work->anm_val < ANM_VAL_MAX/2 )  // ���񂾂񏬂���
  {
    MCSS_SetAlpha( work->poke_set[work->disp_poke].wk, 31 );
    MCSS_SetAlpha( work->poke_set[NotDispPoke(work->disp_poke)].wk, 0 );
  }
  else  // ���񂾂�傫��
  {
    int alpha_anm_val;
    fx16 alpha_cos01_fx16;
    f32 alpha_cos01;
    f32 alpha_f32;
    u8 alpha;
    {
      alpha_anm_val = ( work->anm_val - ANM_VAL_MAX/2 ) *2;
      if( alpha_anm_val > ANM_VAL_MAX/2 ) alpha_anm_val = ANM_VAL_MAX/2;
      alpha_cos01_fx16 = ( FX_CosIdx( alpha_anm_val ) + FX16_ONE ) /2;
      alpha_cos01 = FX_FX16_TO_F32( alpha_cos01_fx16 );
      alpha_f32 = 32.0f * alpha_cos01;
      if( alpha_f32 < 0.0f ) alpha_f32 = 0.0f;
      alpha = (u8)alpha_f32;
      if( alpha > 31 ) alpha = 31;
      MCSS_SetAlpha( work->poke_set[work->disp_poke].wk, alpha );
    }
    {
      alpha_anm_val = ( work->anm_val - ANM_VAL_MAX/2 ) *3;
      if( alpha_anm_val > ANM_VAL_MAX/2 ) alpha_anm_val = ANM_VAL_MAX/2;
      alpha_cos01_fx16 = ( FX_CosIdx( alpha_anm_val ) + FX16_ONE ) /2;
      alpha_cos01 = FX_FX16_TO_F32( alpha_cos01_fx16 );
      alpha_cos01 = 1.0f - alpha_cos01;
      alpha_f32 = 32.0f * alpha_cos01;
      if( alpha_f32 < 0.0f ) alpha_f32 = 0.0f;
      alpha = (u8)alpha_f32;
      if( alpha > 31 ) alpha = 31;
      MCSS_SetAlpha( work->poke_set[NotDispPoke(work->disp_poke)].wk, alpha );
    }
  }
*/

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
      ofs_y = ( POKE_SIZE_MAX - size_y ) / 2.0f;
      ofs.x = 0;  ofs.y = FX_F32_TO_FX32(ofs_y);  ofs.z = 0;
      MCSS_SetOfsPosition( work->poke_set[i].wk, &ofs );
    }
  }
}

//-------------------------------------
/// �p�[�e�B�N��
//=====================================
static PARTICLE_MANAGER* Particle_Init( HEAPID heap_id )
{
  const int spa_file_arc_idx[PARTICLE_SPA_FILE_MAX] =
  {
    NARC___particle_shinka_demo_particle_shinka_demo_spa,
    NARC___particle_shinka_demo_particle_shinka_demo_dmy_spa,
  };
  PARTICLE_MANAGER* mgr;
  u8 i;

  // �p�[�e�B�N����p�J����	
  VecFx32 cam_eye = {            0,         0,   FX32_CONST(70) };
  VecFx32 cam_up  = {            0,  FX32_ONE,                0 };
  VecFx32 cam_at  = {            0,         0,                0 };
  
  GFL_G3D_PROJECTION proj; 
	{
		proj.type      = GFL_G3D_PRJORTH;
		proj.param1    =  FX32_CONST(3);
		proj.param2    = -FX32_CONST(3);
		proj.param3    = -FX32_CONST(4);
		proj.param4    =  FX32_CONST(4);
		proj.near      = defaultCameraNear;
		proj.far       = defaultCameraFar;
		proj.scaleW    = 0;
	}

  // PARTICLE_MANAGER
  {
    mgr = GFL_HEAP_AllocClearMemory( heap_id, sizeof(PARTICLE_MANAGER) );
    mgr->frame           = 0;
    mgr->data_no         = 0;
    mgr->data_num        = NELEMS(particle_play_data_tbl);
    mgr->data_tbl        = particle_play_data_tbl;
    mgr->play            = FALSE;
    mgr->stop_count      = -1;
  }

  GFL_PTC_Init( heap_id );

  for(i=0; i<PARTICLE_SPA_FILE_MAX; i++)
  {
    void* arc_res;

    mgr->spa_set[i].ptc = GFL_PTC_Create( mgr->spa_set[i].wk, sizeof(mgr->spa_set[i].wk), TRUE, heap_id );
		GFL_PTC_PersonalCameraDelete( mgr->spa_set[i].ptc );
		GFL_PTC_PersonalCameraCreate( mgr->spa_set[i].ptc, &proj, DEFAULT_PERSP_WAY, &cam_eye, &cam_up, &cam_at, heap_id );
    arc_res = GFL_PTC_LoadArcResource( ARCID_SHINKA_DEMO, spa_file_arc_idx[i], heap_id );
    mgr->spa_set[i].res_num = GFL_PTC_GetResNum( arc_res );
    GFL_PTC_SetResource( mgr->spa_set[i].ptc, arc_res, TRUE, NULL );
  }

  return mgr;
}

static void Particle_Exit( PARTICLE_MANAGER* mgr )
{
  GFL_PTC_Exit();

  GFL_HEAP_FreeMemory( mgr );
}

static void Particle_Main( PARTICLE_MANAGER* mgr )
{
  if( mgr->play )
  {
    VecFx32 pos = { 0, 0, 0 };
    GFL_EMIT_PTR emit;

    while( mgr->data_no < mgr->data_num )
    {
      if( mgr->frame == mgr->data_tbl[mgr->data_no].frame )
      {
        emit = GFL_PTC_CreateEmitter( mgr->spa_set[ mgr->data_tbl[mgr->data_no].spa_idx ].ptc, mgr->data_tbl[mgr->data_no].res_no, &pos );
        mgr->data_no++;
      }
      else
      {
        break;
      }
    }
    mgr->frame++;
  }

  if( mgr->stop_count >= 0 )
  {
    if( mgr->stop_count == 0 )
    {
      u8 i;
      for(i=0; i<PARTICLE_SPA_FILE_MAX; i++)
      {
        GFL_PTC_DeleteEmitterAll( mgr->spa_set[i].ptc );
      }
    }
    mgr->stop_count--;
  }
}

static void Particle_Draw( PARTICLE_MANAGER* mgr )
{
  GFL_PTC_Main();
}

static void Particle_Start( PARTICLE_MANAGER* mgr )
{
  mgr->play = TRUE;
}

static void Particle_Stop( PARTICLE_MANAGER* mgr, s32 stop_count )
{
  mgr->play = FALSE;
  mgr->stop_count = stop_count;
}

