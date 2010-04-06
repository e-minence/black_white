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
//#define DEBUG_KAWADA
//#define SET_PARTICLE_Y_MODE  // ���ꂪ��`����Ă���Ƃ��A�p�[�e�B�N����Y�l��ҏW�ł��郂�[�h�ɂȂ�


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/mcss.h"
#include "system/mcss_tool.h"
#include "poke_tool/monsno_def.h"

#include "egg_demo_view.h"


// �A�[�J�C�u
#include "arc_def.h"
#include "../../../../resource/egg_demo/egg_demo_setup.h"
#include "egg_demo_particle.naix"
// �T�E���h
#include "sound/pm_voice.h"
#include "sound/wb_sound_data.sadl"
#include "sound/pm_sndsys.h"


//=============================================================================
/**
 *  �萔��`
 */
//=============================================================================
// �X�e�b�v
typedef enum
{
  STEP_EGG_START,       // �^�}�S�J�n�҂������f��
  STEP_EGG_DEMO,        // �^�}�S�f��
  STEP_EGG_WHITE,       // �^�}�S����
  STEP_EGG_ALPHA,       // �^�}�S������
  STEP_EGG_END,         // �^�}�S�I����
  STEP_MON_READY,       // �|�P����������
  STEP_MON_WHITE,       // �|�P�������ő҂�
  STEP_MON_START,       // �|�P�����J�n�҂�������
  STEP_MON_CRY,         // �|�P�������I���̂�҂������I������
  STEP_MON_END,         // �|�P�����I����
}
STEP;

// �p�[�e�B�N���Ή�
#define POKE_TEX_ADRS_FOR_PARTICLE  (0x30000)

// �|�P����
#define POKE_SCALE       (16.0f)
#define POKE_SIZE_MAX    (96.0f)

// �}�i�t�B�ȊO�̃|�P����
// �p�[�e�B�N���̃t���[���ƃ|�P�����A�j���[�V�����̃t���[����A�g������
#define PARTICLE_BURST_FRAME   (380)  // �Ђт������Ă���G�̍ŏI�t���[��  // �S����381�t���[����������
#define POKE_ANIME_INDEX (1)

#define EGG_CRACK_FRAME_01      ( 24 -4)  // �q�r���J�n
#define EGG_CRACK_FRAME_02      (112 -3)  // �q�r���J�n 
#define EGG_CRACK_FRAME_03      (227 -9)  // �q�r��J�n

// �}�i�t�B
// �p�[�e�B�N���̃t���[���ƃ|�P�����A�j���[�V�����̃t���[����A�g������
#define MANAFI_PARTICLE_BURST_FRAME   (300)  // �p�[�e�B�N���G�t�F�N�gDEMO_MANA01, DEMO_MANA02�͑S����300�t���[������炵���B
#define MANAFI_POKE_ANIME_INDEX (0)

#define MANAFI_EGG_SE_FRAME_01  ( 10)  // 1��
//#define MANAFI_EGG_SE_FRAME_02  (100)  // 2��  // 2�ڂ�1�ڂ��I����Ă��炷���炷���Ƃɂ���

typedef enum
{
  MANAFI_EGG_SE_STATE_BEFORE_PLAY,  // �Đ��O�ŉ����Đ����Ă��Ȃ�
  MANAFI_EGG_SE_STATE_01,           // 1�ڍĐ���
  MANAFI_EGG_SE_STATE_02,           // 2�ڍĐ���
  MANAFI_EGG_SE_STATE_AFTER_PLAY,   // �Đ���ŉ����Đ����Ă��Ȃ�
}
MANAFI_EGG_SE_STATE;


// 3D
// 3D��
static const GFL_G3D_UTIL_RES rear_res_tbl[] =
{
  { ARCID_EGG_DEMO, NARC_egg_demo_particle_demo_sinka02_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_EGG_DEMO, NARC_egg_demo_particle_demo_sinka02_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_EGG_DEMO, NARC_egg_demo_particle_demo_sinka02_nsbca, GFL_G3D_UTIL_RESARC },
  { ARCID_EGG_DEMO, NARC_egg_demo_particle_demo_sinka02_nsbma, GFL_G3D_UTIL_RESARC },
};
enum
{
  REAR_ANM_T,
  REAR_ANM_C,
  REAR_ANM_M,
  REAR_ANM_MAX,
};
static const GFL_G3D_UTIL_ANM rear_anm_tbl[REAR_ANM_MAX] = 
{
  { 1, 0 },
  { 2, 0 },
  { 3, 0 },
};
static const GFL_G3D_UTIL_OBJ rear_obj_tbl[] = 
{
  {
    0,                         // ���f�����\�[�XID
    0,                         // ���f���f�[�^ID(���\�[�X����INDEX)
    0,                         // �e�N�X�`�����\�[�XID
    rear_anm_tbl,               // �A�j���e�[�u��(�����w��̂���)
    NELEMS(rear_anm_tbl),       // �A�j�����\�[�X��
  },
};
#define REAR_COLOR_FRAME  (0)    // REAR_COLOR_FRAME�ŕ��ʂ̐F�ɂȂ�
#define REAR_WHITE_FRAME  (100)  // REAR_WHITE_FRAME�Ő^�����ɂȂ�
// 3D�S��
#define THREE_RES_MAX              (4)   // ��x�ɓǂݍ��ޑ���
#define THREE_OBJ_MAX              (1)   // ��x�ɓǂݍ��ޑ���
// �Z�b�g�A�b�v�ԍ�
enum
{
  THREE_SETUP_IDX_REAR,
  THREE_SETUP_IDX_MAX
};
// �Z�b�g�A�b�v�f�[�^
static const GFL_G3D_UTIL_SETUP three_setup_tbl[THREE_SETUP_IDX_MAX] =
{
  { rear_res_tbl,   NELEMS(rear_res_tbl),   rear_obj_tbl,   NELEMS(rear_obj_tbl)   },
};
// ���[�U(���̃\�[�X�̃v���O�������������l)�����߂��I�u�W�F�N�g�ԍ�
enum
{
  THREE_USER_OBJ_IDX_REAR,
  THREE_USER_OBJ_IDX_MAX,
};
// 3D�̃A�j����1�t���[���łǂꂾ���i�߂邩
#define THREE_ADD (FX32_ONE)  // 60fps
// REAR�𔒂���΂��A�j���̏��
typedef enum
{
  REAR_WHITE_ANIME_STATE_COLOR,
  REAR_WHITE_ANIME_STATE_COLOR_TO_WHITE,
  REAR_WHITE_ANIME_STATE_WHITE,
  REAR_WHITE_ANIME_STATE_WHITE_TO_COLOR,
}
REAR_WHITE_ANIME_STATE;


// �|�P�����ƃp�[�e�B�N����Y���W
#define POKE_Y      (FX_F32_TO_FX32(-100.0f))
#define PARTICLE_Y  (FX_F32_TO_FX32(-0.5f))


#ifdef SET_PARTICLE_Y_MODE
f32 particle_y = 0.0f;
#endif


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
  PARTICLE_SPA_FILE_0,      // ARCID_EGG_DEMO    // NARC_egg_demo_particle_egg_demo_spa
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
  { ARCID_EGG_DEMO, NARC_egg_demo_particle_egg_demo_spa  },
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
static const PARTICLE_PLAY_DATA particle_play_data_tbl[] =
{
  {    PARTICLE_BURST_FRAME,     PARTICLE_SPA_FILE_0,        DEMO_EGG01 },
  {    PARTICLE_BURST_FRAME,     PARTICLE_SPA_FILE_0,        DEMO_EGG02 },
  {    PARTICLE_BURST_FRAME,     PARTICLE_SPA_FILE_0,        DEMO_EGG03 },
};

//   0 �q�r�Ȃ�
//  24 �q�r���J�n
//  28 �q�r���g��
//  78 �q�r���J�n
//  82 �q�r���g��
// 132 �q�r��J�n
// 202 = 0

static const PARTICLE_PLAY_DATA manafi_particle_play_data_tbl[] =
{
  {    0,                               PARTICLE_SPA_FILE_0,        DEMO_MANA01 },  // �Đ��J�n�t���[�����������ɕ��ׂĂ�������
  {    0,                               PARTICLE_SPA_FILE_0,        DEMO_MANA02 },
  {    MANAFI_PARTICLE_BURST_FRAME,     PARTICLE_SPA_FILE_0,        DEMO_EGG01  },
  {    MANAFI_PARTICLE_BURST_FRAME,     PARTICLE_SPA_FILE_0,        DEMO_EGG02  },
  {    MANAFI_PARTICLE_BURST_FRAME,     PARTICLE_SPA_FILE_0,        DEMO_EGG03  },
};


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
/// ���[�N
//=====================================
struct _EGG_DEMO_VIEW_WORK
{
  // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B
  HEAPID                   heap_id;
  const POKEMON_PARAM*     pp;
  u16                      monsno;
  
  // �X�e�b�v
  STEP                     step;

  // �t���O�Ȃ�
  BOOL                     b_start;
  BOOL                     b_white;
  u32                      voicePlayerIdx;
  u32                      wait_count;
 
  // �}�i�t�B��SE�̏��
  MANAFI_EGG_SE_STATE      manafi_egg_se_state;

  // VBlank��TCB
  GFL_TCB*                 vblank_tcb;

  // MCSS
  MCSS_SYS_WORK*           mcss_sys_wk;
  MCSS_WORK*               mcss_wk;
  BOOL                     mcss_anime_end;
  BOOL                     mcss_alpha_anime;
  u8                       mcss_alpha_end;

  // �p�[�e�B�N��
  PARTICLE_MANAGER*        particle_mgr;

  // 3D
  GFL_G3D_UTIL*            three_util;
  u16                      three_unit_idx[THREE_SETUP_IDX_MAX];             // GFL_G3D_UTIL������U��ԍ�        // �Y������THREE_SETUP_IDX_(THREE_SETUP_IDX_MAX)
  u16                      three_obj_prop_tbl_idx[THREE_USER_OBJ_IDX_MAX];  // three_obj_prop_tbl�̃C���f�b�N�X  // �Y������THREE_USER_OBJ_IDX_(THREE_USER_OBJ_IDX_MAX)
  u16                      three_obj_prop_num;
  THREE_OBJ_PROPERTY*      three_obj_prop_tbl;
  // 3D�t���[��
  u32                      three_frame;
  REAR_WHITE_ANIME_STATE   rear_white_anime_state;
};


//=============================================================================
/**
*  ���[�J���֐��̃v���g�^�C�v�錾
*/
//=============================================================================
// VBlank�֐�
static void Egg_Demo_View_VBlankFunc( GFL_TCB* tcb, void* wk );

//-------------------------------------
/// MCSS
//=====================================
static void Egg_Demo_View_McssSysInit( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_McssSysExit( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_McssInit( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_McssExit( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_McssVanish( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_McssSetAnimeIndex( EGG_DEMO_VIEW_WORK* work, int index, BOOL b_last_stop );
static void Egg_Demo_View_McssCallBackFunctor( u32 data, fx32 currentFrame );
static void Egg_Demo_View_McssAlphaAnimeStart( EGG_DEMO_VIEW_WORK* work, u8 start_alpha, u8 end_alpha );
static void Egg_Demo_View_McssAlphaAnimeMain( EGG_DEMO_VIEW_WORK* work );
static BOOL Egg_Demo_View_McssAlphaAnimeIsEnd( EGG_DEMO_VIEW_WORK* work );

//-------------------------------------
/// �^�}�S
//=====================================
static void Egg_Demo_View_EggStopWhite( EGG_DEMO_VIEW_WORK* work );
static BOOL Egg_Demo_View_EggIsWhite( EGG_DEMO_VIEW_WORK* work );

//-------------------------------------
/// �|�P����
//=====================================
static void Egg_Demo_View_MonColor( EGG_DEMO_VIEW_WORK* work );

//-------------------------------------
/// �p�[�e�B�N��
//=====================================
static PARTICLE_MANAGER* Particle_Init( HEAPID heap_id, u16 data_num, const PARTICLE_PLAY_DATA* data_tbl );
static void Particle_Exit( PARTICLE_MANAGER* mgr );
static void Particle_Main( PARTICLE_MANAGER* mgr );
static void Particle_Draw( PARTICLE_MANAGER* mgr );
static void Particle_Start( PARTICLE_MANAGER* mgr );
static void Particle_Stop( PARTICLE_MANAGER* mgr, s32 stop_count );

//-------------------------------------
/// 3D
//=====================================
// 3D�S��
static void Egg_Demo_View_ThreeInit( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_ThreeExit( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_ThreeDraw( EGG_DEMO_VIEW_WORK* work );  // 3D�`��(GRAPHIC_3D_StartDraw��PSEL_GRAPHIC_3D_EndDraw�̊ԂŌĂ�)
// 3D��
static void Egg_Demo_View_ThreeRearInit( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_ThreeRearExit( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_ThreeRearMain( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_ThreeRearStartColorToWhite( EGG_DEMO_VIEW_WORK* work );
static BOOL Egg_Demo_View_ThreeRearIsWhite( EGG_DEMO_VIEW_WORK* work );
static void Egg_Demo_View_ThreeRearStartWhiteToColor( EGG_DEMO_VIEW_WORK* work );
static BOOL Egg_Demo_View_ThreeRearIsColor( EGG_DEMO_VIEW_WORK* work );

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
    work->monsno       = (u16)PP_Get( work->pp, ID_PARA_monsno, NULL );
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

  // �}�i�t�B��SE�̏��
  {
    work->manafi_egg_se_state = MANAFI_EGG_SE_STATE_BEFORE_PLAY;
  }

  // VBlank��TCB
  {
    work->vblank_tcb = GFUser_VIntr_CreateTCB( Egg_Demo_View_VBlankFunc, work, 1 );
  }
  
  // MCSS
  {
    Egg_Demo_View_McssSysInit( work );
    Egg_Demo_View_McssInit( work );
  }

  // �p�[�e�B�N��
  if( work->monsno == MONSNO_MANAFI )
  {
    work->particle_mgr = Particle_Init( work->heap_id, NELEMS(manafi_particle_play_data_tbl), manafi_particle_play_data_tbl );
  }
  else
  {
    work->particle_mgr = Particle_Init( work->heap_id, NELEMS(particle_play_data_tbl), particle_play_data_tbl );
  }

  // 3D
  Egg_Demo_View_ThreeInit( work );
  Egg_Demo_View_ThreeRearInit( work );

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
  // 3D
  Egg_Demo_View_ThreeRearExit( work );
  Egg_Demo_View_ThreeExit( work );

  // �p�[�e�B�N��
  Particle_Exit( work->particle_mgr );

  // MCSS
  {
    Egg_Demo_View_McssExit( work );
    Egg_Demo_View_McssSysExit( work );
  }

  // VBlank��TCB
  GFL_TCB_DeleteTask( work->vblank_tcb );

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
        Egg_Demo_View_McssSetAnimeIndex( work, (work->monsno==MONSNO_MANAFI)?(MANAFI_POKE_ANIME_INDEX):(POKE_ANIME_INDEX), TRUE );
        
        // �p�[�e�B�N��
        Particle_Start( work->particle_mgr );
      }
    }
    break;
  case STEP_EGG_DEMO:
    {
      u32 particle_burst_frame = (work->monsno==MONSNO_MANAFI)?(MANAFI_PARTICLE_BURST_FRAME):(PARTICLE_BURST_FRAME);

      work->wait_count++;
      if( work->monsno == MONSNO_MANAFI )
      {
        switch( work->manafi_egg_se_state )
        {
        case MANAFI_EGG_SE_STATE_BEFORE_PLAY:
          {
            if( work->wait_count == MANAFI_EGG_SE_FRAME_01 )
            {
              PMSND_PlaySE( SEQ_SE_EDEMO_02 );
              work->manafi_egg_se_state = MANAFI_EGG_SE_STATE_01;
            }
          }
          break;
        case MANAFI_EGG_SE_STATE_01:
          {
            if( !PMSND_CheckPlaySE() )
            {
              PMSND_PlaySE( SEQ_SE_EDEMO_03 );
              work->manafi_egg_se_state = MANAFI_EGG_SE_STATE_02;
            }
          }
          break;
        case MANAFI_EGG_SE_STATE_02:
          {
            if( !PMSND_CheckPlaySE() )
            {
              work->manafi_egg_se_state = MANAFI_EGG_SE_STATE_AFTER_PLAY;
            }
          }
          break;
        case MANAFI_EGG_SE_STATE_AFTER_PLAY:
          {
          }
          break;
        }
      }
      else
      {
        if( work->wait_count == EGG_CRACK_FRAME_01 )
        {
          PMSND_PlaySE( SEQ_SE_EDEMO_01 );
        }
        else if( work->wait_count == EGG_CRACK_FRAME_02 )
        {
          PMSND_PlaySE( SEQ_SE_EDEMO_01 );
        }
        else if( work->wait_count == EGG_CRACK_FRAME_03 )
        {
          PMSND_PlaySE( SEQ_SE_W181_01 );
        }
      }
      if( work->wait_count == particle_burst_frame )
      {
        Egg_Demo_View_EggStopWhite( work );
        Egg_Demo_View_ThreeRearStartColorToWhite( work );
        PMSND_PlaySE( SEQ_SE_TDEMO_011 );
        
        // ����
        work->step = STEP_EGG_WHITE;
      }
    }
    break;
  case STEP_EGG_WHITE:
    {
      if( Egg_Demo_View_EggIsWhite( work ) )
      {
        //aplha�͐g�̂̕��i�������Ă��܂��̂ł�߂�Egg_Demo_View_McssAlphaAnimeStart( work, 31, 0 );

        // ����
        work->step = STEP_EGG_ALPHA;
      }
    }
    break;
  case STEP_EGG_ALPHA:
    {
      if(    1//aplha�͐g�̂̕��i�������Ă��܂��̂ł�߂�Egg_Demo_View_McssAlphaAnimeIsEnd( work ) 
          && Egg_Demo_View_ThreeRearIsWhite( work ) )
      {
        Egg_Demo_View_McssVanish( work );
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
  case STEP_MON_WHITE:
    {
      work->wait_count++;
      if( work->wait_count >= 120 )
      {
        Egg_Demo_View_MonColor( work );
        Egg_Demo_View_ThreeRearStartWhiteToColor( work );

        // ����
        work->step = STEP_MON_START;
        work->wait_count = 0;
      }
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


#ifdef SET_PARTICLE_Y_MODE
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
  {
    OS_Printf( "particle_y = %f\n", particle_y );
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
  {
    particle_y += 0.1f;
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
  {
    particle_y -= 0.1f;
  }
#endif


  // MCSS
  Egg_Demo_View_McssAlphaAnimeMain( work );
  MCSS_Main( work->mcss_sys_wk );
  
  // �p�[�e�B�N��
  Particle_Main( work->particle_mgr );

  // 3D
  Egg_Demo_View_ThreeRearMain( work );
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

  // 3D
  Egg_Demo_View_ThreeDraw( work );

  // �p�[�e�B�N��
  Particle_Draw( work->particle_mgr );

  //����
  //Egg_Demo_View_ThreeDraw( work );
  //MCSS_Draw( work->mcss_sys_wk );
  //Particle_Draw( work->particle_mgr );
  //�̏��ɕ`�悵����A
  //�|�P����MCSS���AREAR3D�̃X�N���[���A�j���ɂ��ăX�N���[�����Ă��܂��Ă����B
  //
  //����
  //MCSS_Draw( work->mcss_sys_wk );
  //Particle_Draw( work->particle_mgr );
  //Egg_Demo_View_ThreeDraw( work );
  //�̏��ɕ`�悵����A
  //�p�[�e�B�N�����AREAR3D�̕`������ɂȂ��Ă��܂��Ă����B
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
 *  @brief         �������ł��邩(1�t���[������TRUE�ɂȂ�Ȃ�)
 *
 *  @param[in,out] work  EGG_DEMO_VIEW_Init�Ő����������[�N
 *
 *  @retval        �������1�t���[���ɂ����Ă���TRUE��Ԃ�
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
  Egg_Demo_View_McssSetAnimeIndex( work, 0, FALSE );

  // ���ɂ���
  MCSS_SetVanishFlag( work->mcss_wk );
  MCSS_SetPaletteFade( work->mcss_wk, 16, 16, 0, 0x7fff );
  
  // ��������
  //MCSS_SetPaletteFade( work->mcss_wk, 16, 0, 1, 0x7fff );

  work->step = STEP_MON_READY;
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
  work->step = STEP_MON_WHITE;
  work->wait_count = 0;
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
/// VBlank�֐�
//=====================================
static void Egg_Demo_View_VBlankFunc( GFL_TCB* tcb, void* wk )
{
  EGG_DEMO_VIEW_WORK* work = (EGG_DEMO_VIEW_WORK*)wk;
}

//-------------------------------------
/// MCSS�V�X�e������������
//=====================================
static void Egg_Demo_View_McssSysInit( EGG_DEMO_VIEW_WORK* work )
{
  work->mcss_sys_wk = MCSS_Init( 1, work->heap_id );
  MCSS_SetTextureTransAdrs( work->mcss_sys_wk, POKE_TEX_ADRS_FOR_PARTICLE );  // �p�[�e�B�N���ƈꏏ�Ɏg������
  MCSS_SetOrthoMode( work->mcss_sys_wk );

  work->mcss_alpha_anime = FALSE;
  work->mcss_alpha_end   = 0;
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
    work->mcss_wk = MCSS_TOOL_AddPokeMcss( work->mcss_sys_wk, work->pp, MCSS_DIR_FRONT,
                        0, POKE_Y, FX_F32_TO_FX32(-800.0f) );//-800.0f) );

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
static void Egg_Demo_View_McssSetAnimeIndex( EGG_DEMO_VIEW_WORK* work, int index, BOOL b_last_stop )
{
  NNSG2dMultiCellAnimation* mcss_anim_ctrl;

  MCSS_SetAnimeIndex( work->mcss_wk, index );
  mcss_anim_ctrl = MCSS_GetAnimCtrl( work->mcss_wk );
  NNS_G2dRestartMCAnimation( mcss_anim_ctrl );
  MCSS_ResetAnmStopFlag( work->mcss_wk );

  if( b_last_stop )  // �ŏI�t���[���ŃR�[���o�b�N���Ăԏꍇ
  {
    NNS_G2dSetAnimCtrlCallBackFunctor(
        NNS_G2dGetMCAnimAnimCtrl(mcss_anim_ctrl),
        NNS_G2D_ANMCALLBACKTYPE_LAST_FRM,
        (u32)work,
        Egg_Demo_View_McssCallBackFunctor );
    work->mcss_anime_end = FALSE;
  }
}
//-------------------------------------
/// MCSS�|�P�����ŏI�t���[���ŌĂ΂��R�[���o�b�N
//=====================================
static void Egg_Demo_View_McssCallBackFunctor( u32 data, fx32 currentFrame )
{
  EGG_DEMO_VIEW_WORK* work = (EGG_DEMO_VIEW_WORK*)data;

  MCSS_SetAnmStopFlag( work->mcss_wk );  // ���̃^�C�~���O����0�t���[���ɖ߂����Ƃ���Ŏ~�܂�̂ŁA1�t���[���x���Ďg�����ɂȂ�Ȃ�
  work->mcss_anime_end = TRUE;
}
//-------------------------------------
/// MCSS�|�P�����A���t�@�A�j���[�V����
//=====================================
static void Egg_Demo_View_McssAlphaAnimeStart( EGG_DEMO_VIEW_WORK* work, u8 start_alpha, u8 end_alpha )  // 0-31
{
  u8 curr_alpha = MCSS_GetAlpha( work->mcss_wk );
  work->mcss_alpha_end = end_alpha;
  if( start_alpha == end_alpha )
  {
    work->mcss_alpha_anime = FALSE;
    MCSS_SetAlpha( work->mcss_wk, end_alpha );
  }
  else
  {
    work->mcss_alpha_anime = TRUE;
    MCSS_SetAlpha( work->mcss_wk, start_alpha );
  }
}
static void Egg_Demo_View_McssAlphaAnimeMain( EGG_DEMO_VIEW_WORK* work )
{
  if( work->mcss_alpha_anime )
  {
    u8 curr_alpha = MCSS_GetAlpha( work->mcss_wk );
    if( curr_alpha > work->mcss_alpha_end )
    {
      curr_alpha--;
    }
    else  // if( curr_alpha < work->mcss_alpha_end )  // ( curr_alpha == end_alpha )�ɂȂ邱�Ƃ͂Ȃ�
    {
      curr_alpha++;
    }
    MCSS_SetAlpha( work->mcss_wk, curr_alpha );
    if( curr_alpha == work->mcss_alpha_end )
    {
      work->mcss_alpha_anime = FALSE;
    }
  }
}
static BOOL Egg_Demo_View_McssAlphaAnimeIsEnd( EGG_DEMO_VIEW_WORK* work )
{
  return !(work->mcss_alpha_anime);
}

//-------------------------------------
/// �^�}�S�̃A�j�����~�߁A��������
//=====================================
static void Egg_Demo_View_EggStopWhite( EGG_DEMO_VIEW_WORK* work )
{
  // �A�j�����~�߂�
  MCSS_SetAnmStopFlag( work->mcss_wk );
  // ���ʁ���
  MCSS_SetPaletteFade( work->mcss_wk, 0, 16, 0, 0x7fff );
}
//-------------------------------------
/// �^�}�S�������Ȃ��Ă�����true
//=====================================
static BOOL Egg_Demo_View_EggIsWhite( EGG_DEMO_VIEW_WORK* work )
{
  return !MCSS_CheckExecutePaletteFade( work->mcss_wk );
}

//-------------------------------------
/// �|�P������\�����A�����A�����畁�ʂ̐F�ɂ���A
//=====================================
static void Egg_Demo_View_MonColor( EGG_DEMO_VIEW_WORK* work )
{
  // ����������
  //aplha�͐g�̂̕��i�������Ă��܂��̂ł�߂�Egg_Demo_View_McssAlphaAnimeStart( work, 0, 31 );
  // �\��
  MCSS_ResetVanishFlag( work->mcss_wk );
  // ��������
  MCSS_SetPaletteFade( work->mcss_wk, 16, 0, 2, 0x7fff );
}

//-------------------------------------
/// �p�[�e�B�N��
//=====================================
static PARTICLE_MANAGER* Particle_Init( HEAPID heap_id, u16 data_num, const PARTICLE_PLAY_DATA* data_tbl )
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
    mgr->data_num        = data_num;
    mgr->data_tbl        = data_tbl;
    mgr->play            = FALSE;
    mgr->stop_count      = -1;
  }

  GFL_PTC_Init( heap_id );

  for(i=0; i<PARTICLE_SPA_FILE_MAX; i++)
  {
    void* arc_res;

    mgr->spa_set[i].ptc = GFL_PTC_Create( mgr->spa_set[i].wk, sizeof(mgr->spa_set[i].wk), TRUE, heap_id );
		//GFL_PTC_PersonalCameraDelete( mgr->spa_set[i].ptc );
		//GFL_PTC_PersonalCameraCreate( mgr->spa_set[i].ptc, &proj, DEFAULT_PERSP_WAY, &cam_eye, &cam_up, &cam_at, heap_id );
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
    VecFx32 pos = { 0, PARTICLE_Y, 0 };//FX_F32_TO_FX32(0.5f), 0 };
    GFL_EMIT_PTR emit;


#ifdef SET_PARTICLE_Y_MODE
    pos.y = FX_F32_TO_FX32(particle_y);
#endif


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

//-------------------------------------
/// 3D
//=====================================
// 3D�S��
static void Egg_Demo_View_ThreeInit( EGG_DEMO_VIEW_WORK* work )
{
  // �����_�����O�X���b�v�o�b�t�@
  GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );  // �}�j���A���\�[�g�ɂ���(EGG_DEMO_GRAPHIC�ł̓I�[�g�\�[�g�ɐݒ肵�Ă�����)

  // 3D�Ǘ����[�e�B���e�B�[�̃Z�b�g�A�b�v
  work->three_util = GFL_G3D_UTIL_Create( THREE_RES_MAX, THREE_OBJ_MAX, work->heap_id );

  // NULL�A�[��������
  work->three_obj_prop_num = 0;
}
static void Egg_Demo_View_ThreeExit( EGG_DEMO_VIEW_WORK* work )
{
  // 3D�Ǘ����[�e�B���e�B�[�̔j��
  GFL_G3D_UTIL_Delete( work->three_util );
}
static void Egg_Demo_View_ThreeDraw( EGG_DEMO_VIEW_WORK* work )  // 3D�`��(GRAPHIC_3D_StartDraw��PSEL_GRAPHIC_3D_EndDraw�̊ԂŌĂ�)
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
// 3D��
static void Egg_Demo_View_ThreeRearInit( EGG_DEMO_VIEW_WORK* work )
{
  // ���j�b�g�ǉ�
  {
    u16 i = THREE_SETUP_IDX_REAR;
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

    u16 i = THREE_SETUP_IDX_REAR;
    {
      u16 head_obj_idx = GFL_G3D_UTIL_GetUnitObjIdx( work->three_util, work->three_unit_idx[i] );
      u16 head_user_obj_idx = THREE_USER_OBJ_IDX_REAR;
      u16 j = 0;
      {
        THREE_OBJ_PROPERTY* prop;
        work->three_obj_prop_tbl_idx[head_user_obj_idx +j] = h;
        prop = &(work->three_obj_prop_tbl[h]);
        prop->idx  = head_obj_idx +j;
        VEC_Set( &(prop->objstatus.trans), 0, FX_F32_TO_FX32(0.0f), 0 );
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
      //u16 anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      //u16 j;
      //for( j=0; j<anime_count; j++ )
      //{
      //  GFL_G3D_OBJECT_EnableAnime( obj, j );
      //}
      GFL_G3D_OBJECT_EnableAnime( obj, REAR_ANM_T );
    }
  }

  // �t���[��
  // �v���O�����ł͊J�n�t���[����0�Ƃ��ď�������B�f�U�C�i�[�����3D�I�[�T�����O�c�[����ł�0�t���[������X�^�[�g���Ă���悤���B
  work->three_frame = 0;

  // REAR�𔒂���΂��A�j���̏��
  {
    THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[ work->three_obj_prop_tbl_idx[THREE_USER_OBJ_IDX_REAR] ]);
    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
    int anime_frame = REAR_COLOR_FRAME;
    work->rear_white_anime_state = REAR_WHITE_ANIME_STATE_COLOR;
    GFL_G3D_OBJECT_EnableAnime( obj, REAR_ANM_C );
    GFL_G3D_OBJECT_EnableAnime( obj, REAR_ANM_M );
	  GFL_G3D_OBJECT_SetAnimeFrame( obj, REAR_ANM_C, &anime_frame );
	  GFL_G3D_OBJECT_SetAnimeFrame( obj, REAR_ANM_M, &anime_frame );
  }
}
static void Egg_Demo_View_ThreeRearExit( EGG_DEMO_VIEW_WORK* work )
{
  // ���j�b�g�j��
  {
    u16 i = THREE_SETUP_IDX_REAR;
    {
      GFL_G3D_UTIL_DelUnit( work->three_util, work->three_unit_idx[i] );
    }
  }

  // �I�u�W�F�N�g�S��
  {
    ThreeObjPropertyDeleteArray( work->three_obj_prop_tbl );
    work->three_obj_prop_num = 0;
  }
}
static void Egg_Demo_View_ThreeRearMain( EGG_DEMO_VIEW_WORK* work )
{
  const fx32 anime_speed = THREE_ADD;  // �������iFX32_ONE�łP�t���[���i�߂�j
  
  // �A�j���[�V�����X�V
  {
    u16 i;
    for( i=0; i<work->three_obj_prop_num; i++ )
    {
      THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[i]);
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
      //u16 anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      //u16 j;
      //for( j=0; j<anime_count; j++ )
      //{
      //  GFL_G3D_OBJECT_LoopAnimeFrame( obj, j, anime_speed );
      //}
      GFL_G3D_OBJECT_LoopAnimeFrame( obj, REAR_ANM_T, anime_speed );
    }
  }
  
  // �t���[��
  work->three_frame++;

  // REAR�𔒂���΂��A�j���̏��
  {
    THREE_OBJ_PROPERTY* prop = &(work->three_obj_prop_tbl[ work->three_obj_prop_tbl_idx[THREE_USER_OBJ_IDX_REAR] ]);
    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->three_util, prop->idx );
    BOOL b_continue_c = TRUE;
    BOOL b_continue_m = TRUE;
    fx32 anime_add;  // �������iFX32_ONE�łP�t���[���i�߂�j
    if( work->rear_white_anime_state == REAR_WHITE_ANIME_STATE_COLOR_TO_WHITE )
    {
      anime_add = FX32_ONE * 5;
	    b_continue_c = GFL_G3D_OBJECT_IncAnimeFrame( obj, REAR_ANM_C, anime_add );
	    b_continue_m = GFL_G3D_OBJECT_IncAnimeFrame( obj, REAR_ANM_M, anime_add );
      if( (!b_continue_c) && (!b_continue_m) )
      {
        work->rear_white_anime_state = REAR_WHITE_ANIME_STATE_WHITE;
      }
    }
    else if( work->rear_white_anime_state == REAR_WHITE_ANIME_STATE_WHITE_TO_COLOR )
    {
      anime_add = FX32_ONE * (-2);
	    b_continue_c = GFL_G3D_OBJECT_IncAnimeFrame( obj, REAR_ANM_C, anime_add );
	    b_continue_m = GFL_G3D_OBJECT_IncAnimeFrame( obj, REAR_ANM_M, anime_add );
      if( (!b_continue_c) && (!b_continue_m) )
      {
        work->rear_white_anime_state = REAR_WHITE_ANIME_STATE_COLOR;
      }
    }
	  //GFL_G3D_OBJECT_SetAnimeFrame( obj, REAR_ANM_C, &work->rear_white_anime_frame );  // GFL_G3D_OBJECT_SetAnimeFrame���ƊG���X�V����Ȃ������̂ŁA
	  //GFL_G3D_OBJECT_SetAnimeFrame( obj, REAR_ANM_M, &work->rear_white_anime_frame );  // GFL_G3D_OBJECT_IncAnimeFrame�ɂ����B
  }
}
static void Egg_Demo_View_ThreeRearStartColorToWhite( EGG_DEMO_VIEW_WORK* work )
{
  work->rear_white_anime_state = REAR_WHITE_ANIME_STATE_COLOR_TO_WHITE;
}
static BOOL Egg_Demo_View_ThreeRearIsWhite( EGG_DEMO_VIEW_WORK* work )
{
  return ( work->rear_white_anime_state == REAR_WHITE_ANIME_STATE_WHITE );
}
static void Egg_Demo_View_ThreeRearStartWhiteToColor( EGG_DEMO_VIEW_WORK* work )
{
  work->rear_white_anime_state = REAR_WHITE_ANIME_STATE_WHITE_TO_COLOR;
}
static BOOL Egg_Demo_View_ThreeRearIsColor( EGG_DEMO_VIEW_WORK* work )
{
  return ( work->rear_white_anime_state == REAR_WHITE_ANIME_STATE_COLOR );
}

