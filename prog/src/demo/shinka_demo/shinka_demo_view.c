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

#include "shinka_demo_view.h"


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
  STEP_EVO_START,             // �J�n�҂�������������
  STEP_EVO_WHITE,             // �����Ȃ�̂�҂������g��k������ւ����o���J�n����
  STEP_EVO_SCALE,             // �g��k������ւ����o�����F��t����
  STEP_EVO_COLOR,             // �F���t���̂�҂������I������
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


//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// ���[�N
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

  // �X�e�b�v
  STEP                     step;

  // �t���O
  BOOL                     b_start;
  BOOL                     b_cancel;  // �i���L�����Z��

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
      }
    }
    break;
  case STEP_EVO_COLOR:
    {
      if( !MCSS_CheckExecutePaletteFade( work->poke_set[work->disp_poke].wk ) )
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
  if(
         work->step == STEP_EVO_COLOR
      || work->step == STEP_EVO_END
      || work->step == STEP_AFTER
  ) 
    return TRUE;
  return FALSE;
}


//=============================================================================
/**
*  ���[�J���֐���`
*/
//=============================================================================
//-------------------------------------
/// 
//=====================================
static void ShinkaDemo_View_McssInit( SHINKADEMO_VIEW_WORK* work )
{
  work->mcss_sys_wk = MCSS_Init( POKE_MAX, work->heap_id );
  MCSS_SetTextureTransAdrs( work->mcss_sys_wk, 0 );
  MCSS_SetOrthoMode( work->mcss_sys_wk );
}
//-------------------------------------
/// 
//=====================================
static void ShinkaDemo_View_McssExit( SHINKADEMO_VIEW_WORK* work )
{
  MCSS_Exit( work->mcss_sys_wk );
}

//-------------------------------------
/// 
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
        int sex = PP_Get( work->pp, ID_PARA_sex, NULL );
        MCSS_TOOL_MakeMAWParam( work->after_monsno, 0, sex, 0, 0, &add_wk, MCSS_DIR_FRONT );
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
/// 
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
/// 
//=====================================
static u8 NotDispPoke( u8 disp_poke )
{
  if( disp_poke == POKE_BEFORE ) return POKE_AFTER;
  else                           return POKE_BEFORE;
}

//-------------------------------------
/// 
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
/// 
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

