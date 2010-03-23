//============================================================================
/**
 *  @file   egg_demo_view.c
 *  @brief  �^�}�S�z���f���̉��o
 *  @author Koji Kawada
 *  @data   2010.01.25
 *  @note   
 *
 *  ���W���[�����FEGG_DEMO_VIEW
 */
//============================================================================
#define DEBUG_KAWADA


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/mcss.h"
#include "system/mcss_tool.h"

#include "egg_demo_view.h"


// �A�[�J�C�u
#include "arc_def.h"
#include "../../../../resource/egg_demo/egg_demo_setup.h"
#include "egg_demo_particle.naix"
// �T�E���h
#include "sound/pm_voice.h"


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
  STEP_EGG_START,       // �^�}�S�J�n�҂������f��
  STEP_EGG_DEMO,        // �^�}�S�f��
  STEP_EGG_END,         // �^�}�S�I����
  STEP_MON_READY,       // �|�P����������
  STEP_MON_START,       // �|�P�����J�n�҂�������
  STEP_MON_CRY,         // �|�P�������I���̂�҂������I������
  STEP_MON_END,         // �|�P�����I����
}
STEP;

// �p�[�e�B�N���Ή�
#define POKE_TEX_ADRS_FOR_PARTICLE (0x30000)

// �|�P����
#define POKE_SCALE       (16.0f)
#define POKE_SIZE_MAX    (96.0f)

// �p�[�e�B�N���̃t���[���ƃ|�P�����A�j���[�V�����̃t���[����A�g������
#define PARTICLE_BURST_FRAME   (200)  // 0�`201�t���[���܂ł���A202�t���[������Ȃ��B
#define PARTICLE_LAST_FRAME    (210)  // case STEP_EGG_DEMO: work->wait_count++; �ő������񂾌��wait_count�̒l��
                                      // 202�̂Ƃ��͊��Ƀ^�}�S�������Ă��Ȃ��Ƃ����Ȃ��B
                                      // �f�[�^��24�t���[���ڂ���q�r�̊G�ɂȂ�Ƃ��A
                                      // case STEP_EGG_DEMO: work->wait_count++; �ő������񂾌��wait_count�̒l��
                                      // 24��������A���̎��_�ŉ�ʂɕ\������Ă���G�̓q�r�̊G�ɂȂ��Ă���B

//=============================================================================
/**
*  �\���̐錾
*/
//=============================================================================
//-------------------------------------
/// �p�[�e�B�N�����\�[�X�f�[�^
//=====================================
typedef enum
{
  PARTICLE_SPA_FILE_0,      // ARCID_EGG_DEMO    // NARC___particle_egg_demo_particle_egg_demo_normal_spa
  PARTICLE_SPA_FILE_1,      // ARCID_EGG_DEMO    // NARC___particle_egg_demo_particle_egg_demo_special_spa
  PARTICLE_SPA_FILE_MAX
}
PARTICLE_SPA_FILE;

typedef struct
{
  int  arc;
  int  idx;
}
PARTICLE_ARC;

static const PARTICLE_ARC particle_arc[PARTICLE_SPA_FILE_MAX] =
{
  { ARCID_EGG_DEMO, NARC___particle_egg_demo_particle_egg_demo_normal_spa  },
  { ARCID_EGG_DEMO, NARC___particle_egg_demo_particle_egg_demo_special_spa },
};

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

//-------------------------------------
/// �p�[�e�B�N���Đ��f�[�^
//=====================================
#if 0
static const PARTICLE_PLAY_DATA particle_play_data_tbl[] =
{
  {    0,     PARTICLE_SPA_FILE_0,        0 },  // �^�}�S�̏����Ȋk���������
  {   80,     PARTICLE_SPA_FILE_0,        1 },  // �^�}�S�̏����Ȋk���傫�����
  {  160,     PARTICLE_SPA_FILE_0,        2 },  // �^�}�S�̑傫�Ȋk�����ˏ�Ɍ��������
  {  240,     PARTICLE_SPA_FILE_0,        3 },  // ���F�̐��L���L��
  {  320,     PARTICLE_SPA_FILE_1,        0 },  // �����̑傫�ȉ~
  {  400,     PARTICLE_SPA_FILE_1,        1 },  // �����Ȕ��ʂ�����������ˏ�ɏo��
  {  480,     PARTICLE_SPA_FILE_1,        2 },  // ���ւ��L����
  {  560,     PARTICLE_SPA_FILE_1,        3 },  // ��ȐF�̃^�}�S
};
#elif 0
static const PARTICLE_PLAY_DATA particle_play_data_tbl[] =
{
  {                     120,     PARTICLE_SPA_FILE_0,        0 },  // �^�}�S�̏����Ȋk���������
  {                     210,     PARTICLE_SPA_FILE_0,        1 },  // �^�}�S�̏����Ȋk���傫�����
  {    PARTICLE_BURST_FRAME,     PARTICLE_SPA_FILE_0,        2 },  // �^�}�S�̑傫�Ȋk�����ˏ�Ɍ��������
  {  PARTICLE_LAST_FRAME -5,     PARTICLE_SPA_FILE_0,        3 },  // ���F�̐��L���L��
  {     PARTICLE_LAST_FRAME,     PARTICLE_SPA_FILE_0,        3 },  // ���F�̐��L���L��
};
#else
static const PARTICLE_PLAY_DATA particle_play_data_tbl[] =
{
  {                   78 -3,     PARTICLE_SPA_FILE_0,        0 },  // �^�}�S�̏����Ȋk���������
  {                  132 -3,     PARTICLE_SPA_FILE_0,        1 },  // �^�}�S�̏����Ȋk���傫�����
  { PARTICLE_BURST_FRAME -1,     PARTICLE_SPA_FILE_0,        2 },  // �^�}�S�̑傫�Ȋk�����ˏ�Ɍ��������
  {  PARTICLE_LAST_FRAME -5,     PARTICLE_SPA_FILE_0,        3 },  // ���F�̐��L���L��
  {     PARTICLE_LAST_FRAME,     PARTICLE_SPA_FILE_0,        3 },  // ���F�̐��L���L��
};
#endif

//   0 �q�r�Ȃ�
//  24 �q�r���J�n
//  28 �q�r���g��
//  78 �q�r���J�n
//  82 �q�r���g��
// 132 �q�r��J�n
// 202 = 0


//-------------------------------------
/// ���[�N
//=====================================
struct _EGG_DEMO_VIEW_WORK
{
  // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B
  HEAPID                   heap_id;
  const POKEMON_PARAM*     pp;
  
  // �X�e�b�v
  STEP                     step;

  // �t���O�Ȃ�
  BOOL                     b_start;
  BOOL                     b_white;
  u32                      voicePlayerIdx;
  u32                      wait_count;

  // MCSS
  MCSS_SYS_WORK*           mcss_sys_wk;
  MCSS_WORK*               mcss_wk;

  // �p�[�e�B�N��
  PARTICLE_MANAGER*        particle_mgr;
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
/// MCSS
//=====================================
static void Egg_Demo_View_McssSysInit( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_McssSysExit( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_McssInit( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_McssExit( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_McssVanish( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_McssSetAnimeIndex( EGG_DEMO_VIEW_WORK* work, int index );

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
 *  @retval        EGG_DEMO_VIEW_WORK
 */
//-----------------------------------------------------------------------------
EGG_DEMO_VIEW_WORK* EGG_DEMO_VIEW_Init(
                               HEAPID                   heap_id,
                               const POKEMON_PARAM*     pp
                    )
{
  EGG_DEMO_VIEW_WORK* work;

  // ���[�N
  {
    work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(EGG_DEMO_VIEW_WORK) );
  }

  // ����
  {
    work->heap_id      = heap_id;
    work->pp           = pp;
  }

  // �X�e�b�v
  {
    work->step         = STEP_EGG_START;
  }

  // �t���O�Ȃ�
  {
    work->b_start          = FALSE;
    work->b_white          = FALSE;
    work->voicePlayerIdx   = 0;
    work->wait_count       = 0;
  }
  
  // MCSS
  {
    Egg_Demo_View_McssSysInit( work );
    Egg_Demo_View_McssInit( work );
  }

  // �p�[�e�B�N��
  work->particle_mgr = Particle_Init( work->heap_id );

  return work;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �I������ 
 *
 *  @param[in,out] work  EGG_DEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void EGG_DEMO_VIEW_Exit( EGG_DEMO_VIEW_WORK* work )
{
  // �p�[�e�B�N��
  Particle_Exit( work->particle_mgr );

  // MCSS
  {
    Egg_Demo_View_McssExit( work );
    Egg_Demo_View_McssSysExit( work );
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
 *  @param[in,out] work  EGG_DEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void EGG_DEMO_VIEW_Main( EGG_DEMO_VIEW_WORK* work )
{
  switch(work->step)
  {
  case STEP_EGG_START:
    {
      if( work->b_start )
      {
        // ����
        work->step = STEP_EGG_DEMO;
        
        work->wait_count = 0;

        // �A�j���[�V����
        Egg_Demo_View_McssSetAnimeIndex( work, 1 );
        
        // �p�[�e�B�N��
        Particle_Start( work->particle_mgr );
      }
    }
    break;
  case STEP_EGG_DEMO:
    {
      work->wait_count++;
      if( work->wait_count == PARTICLE_BURST_FRAME +1 )
        Egg_Demo_View_McssVanish( work );
      if( work->wait_count == PARTICLE_LAST_FRAME +20 )
      {
        work->b_white = TRUE;

        // ����
        work->step = STEP_EGG_END;
      }
    }
    break;
  case STEP_EGG_END:
    {
      work->b_white = FALSE;
    }
    break;
  case STEP_MON_READY:
    {
    }
    break;
  case STEP_MON_START:
    {
      work->wait_count++;
      if( work->wait_count >= 65 )
      {
        // ����
        work->step = STEP_MON_CRY;

        {
          u32 monsno  = PP_Get( work->pp, ID_PARA_monsno, NULL );
          u32 form_no = PP_Get( work->pp, ID_PARA_form_no, NULL );
          work->voicePlayerIdx = PMVOICE_Play( monsno, form_no, 64, FALSE, 0, 0, FALSE, 0 );
        }
      }
    }
    break;
  case STEP_MON_CRY:
    {
      if( !PMVOICE_CheckPlay( work->voicePlayerIdx ) )
      {
        // ����
        work->step = STEP_MON_END;
      }
    }
    break;
  case STEP_MON_END:
    {
    }
    break;
  }

  // MCSS
  MCSS_Main( work->mcss_sys_wk );

  // �p�[�e�B�N��
  Particle_Main( work->particle_mgr );
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �`�揈�� 
 *
 *  @param[in,out] work  EGG_DEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 *
 *  @note          GRAPHIC_3D_StartDraw��GRAPHIC_3D_EndDraw�̊ԂŌĂԂ���
 */
//-----------------------------------------------------------------------------
void EGG_DEMO_VIEW_Draw( EGG_DEMO_VIEW_WORK* work )
{
  // MCSS
  MCSS_Draw( work->mcss_sys_wk );

  // �p�[�e�B�N��
  Particle_Draw( work->particle_mgr );
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �X�^�[�g
 *
 *  @param[in,out] work  EGG_DEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void EGG_DEMO_VIEW_Start( EGG_DEMO_VIEW_WORK* work )
{
  work->b_start = TRUE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         ������΂����o�̂��߂̃p���b�g�t�F�[�h�����ė~������(1�t���[������TRUE�ɂȂ�Ȃ�)
 *
 *  @param[in,out] work  EGG_DEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �p���b�g�t�F�[�h�����ė~����1�t���[���ɂ����Ă���TRUE��Ԃ�
 */
//-----------------------------------------------------------------------------
BOOL EGG_DEMO_VIEW_White( EGG_DEMO_VIEW_WORK* work )
{
  return work->b_white;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �^�}�S���|�P�����ɍ����ւ���
 *
 *  @param[in,out] work  EGG_DEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �Ȃ�
 */
//-----------------------------------------------------------------------------
void EGG_DEMO_VIEW_Hatch( EGG_DEMO_VIEW_WORK* work, const POKEMON_PARAM* pp )
{
  // �^�}�S�I��
  Egg_Demo_View_McssExit( work );

  // �|�P�����J�n
  work->pp = pp;
  Egg_Demo_View_McssInit( work );

  // ���ɂ���
  MCSS_SetVanishFlag( work->mcss_wk );
  MCSS_SetPaletteFade( work->mcss_wk, 16, 16, 0, 0x7fff );
  
  // ��������
  //MCSS_SetPaletteFade( work->mcss_wk, 16, 0, 1, 0x7fff );

  work->step = STEP_MON_READY;
  
  work->wait_count = 0;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �|�P�����̏������ł�����
 *
 *  @param[in,out] work  EGG_DEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �������ł��Ă���Ƃ�TRUE��Ԃ�
 */
//-----------------------------------------------------------------------------
BOOL EGG_DEMO_VIEW_IsReady( EGG_DEMO_VIEW_WORK* work )
{
  // ���\�[�X�̃��[�h����������܂Ńt�F�[�h�͎n�܂�Ȃ��̂ŁA����𗘗p���ă��\�[�X�ǂݍ��݊�����҂�
  return !MCSS_CheckExecutePaletteFade( work->mcss_wk );
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �|�P�����X�^�[�g
 *
 *  @param[in,out] work  EGG_DEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void EGG_DEMO_VIEW_MonStart( EGG_DEMO_VIEW_WORK* work )
{
  // ��������
  MCSS_ResetVanishFlag( work->mcss_wk );
  MCSS_SetPaletteFade( work->mcss_wk, 16, 0, 1, 0x7fff );

  work->step = STEP_MON_START;
  
  work->wait_count = 0;

#ifdef DEBUG_KAWADA
    {
      OS_Printf( "EGG_DEMO_VIEW : MCellAnmNum=%d, StopCellAnms=%d\n",
          MCSS_GetMCellAnmNum(work->mcss_wk),
          MCSS_GetStopCellAnms(work->mcss_wk) );
      if( MCSS_GetStopCellAnms(work->mcss_wk) > 0 )
          OS_Printf( "EGG_DEMO_VIEW : StopNode=%d\n", MCSS_GetStopNode(work->mcss_wk, 0) );
    }
#endif

}

//-----------------------------------------------------------------------------
/**
 *  @brief         �I�����Ă��邩
 *
 *  @param[in,out] work  EGG_DEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �I�����Ă���Ƃ�TRUE��Ԃ�
 */
//-----------------------------------------------------------------------------
BOOL EGG_DEMO_VIEW_IsEnd( EGG_DEMO_VIEW_WORK* work )
{
  if( work->step >= STEP_MON_END )
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
static void Egg_Demo_View_McssSysInit( EGG_DEMO_VIEW_WORK* work )
{
  work->mcss_sys_wk = MCSS_Init( 1, work->heap_id );
  MCSS_SetTextureTransAdrs( work->mcss_sys_wk, POKE_TEX_ADRS_FOR_PARTICLE );  // �p�[�e�B�N���ƈꏏ�Ɏg������
  MCSS_SetOrthoMode( work->mcss_sys_wk );
}
//-------------------------------------
/// MCSS�V�X�e���I������
//=====================================
static void Egg_Demo_View_McssSysExit( EGG_DEMO_VIEW_WORK* work )
{
  MCSS_Exit( work->mcss_sys_wk );
}
//-------------------------------------
/// MCSS�|�P��������������
//=====================================
static void Egg_Demo_View_McssInit( EGG_DEMO_VIEW_WORK* work )
{
  {
    MCSS_ADD_WORK add_wk;
    MCSS_TOOL_MakeMAWPP( work->pp, &add_wk, MCSS_DIR_FRONT );
    work->mcss_wk = MCSS_Add( work->mcss_sys_wk, 0, 0, 0, &add_wk );

#ifdef DEBUG_KAWADA
    {
      OS_Printf( "EGG_DEMO_VIEW : MCellAnmNum=%d, StopCellAnms=%d\n",
          MCSS_GetMCellAnmNum(work->mcss_wk),
          MCSS_GetStopCellAnms(work->mcss_wk) );
      if( MCSS_GetStopCellAnms(work->mcss_wk) > 0 )
          OS_Printf( "EGG_DEMO_VIEW : StopNode=%d\n", MCSS_GetStopNode(work->mcss_wk, 0) );
    }
#endif
  }

  {
    VecFx32 scale;
    scale.x = FX_F32_TO_FX32( POKE_SCALE );
    scale.y = FX_F32_TO_FX32( POKE_SCALE );
    scale.z = FX32_ONE;
    MCSS_SetScale( work->mcss_wk, &scale );
  }

  {
    f32 size_y = (f32)MCSS_GetSizeY( work->mcss_wk );
    f32 ofs_y;
    VecFx32 ofs;
    if( size_y > POKE_SIZE_MAX ) size_y = POKE_SIZE_MAX;
    ofs_y = ( POKE_SIZE_MAX - size_y ) / 2.0f;
    ofs.x = 0;  ofs.y = FX_F32_TO_FX32(ofs_y);  ofs.z = 0;
    MCSS_SetOfsPosition( work->mcss_wk, &ofs );
  }
}
//-------------------------------------
/// MCSS�|�P�����I������
//=====================================
static void Egg_Demo_View_McssExit( EGG_DEMO_VIEW_WORK* work )
{
  MCSS_SetVanishFlag( work->mcss_wk );
  MCSS_Del( work->mcss_sys_wk, work->mcss_wk );
}
//-------------------------------------
/// MCSS�|�P��������
//=====================================
static void Egg_Demo_View_McssVanish( EGG_DEMO_VIEW_WORK* work )
{
  MCSS_SetVanishFlag( work->mcss_wk );
}
//-------------------------------------
/// MCSS�|�P�����A�j���[�V�����ݒ�
//=====================================
static void Egg_Demo_View_McssSetAnimeIndex( EGG_DEMO_VIEW_WORK* work, int index )
{
  MCSS_SetAnimeIndex( work->mcss_wk, index );
}

//-------------------------------------
/// �p�[�e�B�N��
//=====================================
static PARTICLE_MANAGER* Particle_Init( HEAPID heap_id )
{
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
    arc_res = GFL_PTC_LoadArcResource( particle_arc[i].arc, particle_arc[i].idx, heap_id );
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
    VecFx32 pos = { 0, FX_F32_TO_FX32(0.5f), 0 };
    GFL_EMIT_PTR emit;

    while( mgr->data_no < mgr->data_num )
    {
      if( mgr->frame == mgr->data_tbl[mgr->data_no].frame )
      {
        emit = GFL_PTC_CreateEmitter( mgr->spa_set[ mgr->data_tbl[mgr->data_no].spa_idx ].ptc, mgr->data_tbl[mgr->data_no].res_no, &pos );

#ifdef DEBUG_KAWADA
        {
          OS_Printf( "EGG_DEMO_VIEW : frame=%d, jspa_idx=%d, res_no=%d, emit=%p\n",
              mgr->frame, mgr->data_tbl[mgr->data_no].spa_idx, mgr->data_tbl[mgr->data_no].res_no, emit );
        }
#endif

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

