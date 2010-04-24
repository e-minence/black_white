//============================================================================
/**
 *  @file   shinka_demo_effect.c
 *  @brief  �i���f���̃p�[�e�B�N���Ɣw�i
 *  @author Koji Kawada
 *  @data   2010.04.09
 *  @note   
 *
 *  ���W���[�����FSHINKADEMO_EFFECT
 */
//============================================================================
//#define DEBUG_CODE
//#define SET_PARTICLE_Z_MODE  // ���ꂪ��`����Ă���Ƃ��A�p�[�e�B�N����Z�l��ҏW�ł��郂�[�h�ɂȂ�
//#define SET_REAR_FRAME_LENGTH_COLOR_MODE  // ���ꂪ��`����Ă���Ƃ��A���ɂ���܂ł̃t���[������ҏW�ł��郂�[�h�ɂȂ�


// �C���N���[�h
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "sound/wb_sound_data.sadl"
#include "sound/pm_sndsys.h"

#include "shinka_demo_effect.h"

// �p�[�e�B�N��
#include "arc_def.h"
#include "../../../../resource/shinka_demo/shinka_demo_setup.h"
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
  STEP_EVO_START,             // �J�n�҂������f��
  STEP_EVO_DEMO,              // �f��
  STEP_EVO_TO_WHITE_START,    // ��������
  STEP_EVO_TO_WHITE,          // �����Ȃ蒆
  STEP_EVO_WHITE,             // ��
  STEP_EVO_FROM_WHITE,        // ������߂�
  STEP_EVO_END,               // �I����

  // AFTER
  STEP_AFTER,                 // �i���ォ��X�^�[�g
}
STEP;


// 3D
// 3D��
static const GFL_G3D_UTIL_RES rear_res_tbl[] =
{
  { ARCID_SHINKA_DEMO, NARC_shinka_demo_particle_demo_sinka01_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_SHINKA_DEMO, NARC_shinka_demo_particle_demo_sinka01_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_SHINKA_DEMO, NARC_shinka_demo_particle_demo_sinka01_nsbca, GFL_G3D_UTIL_RESARC },
  { ARCID_SHINKA_DEMO, NARC_shinka_demo_particle_demo_sinka01_nsbma, GFL_G3D_UTIL_RESARC },
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

// REAR�̃t���[��
#define REAR_FRAME_LENGTH_COLOR      ( 620 )  // STEP_EVO_DEMO�ɂȂ��Ă��炱�̃t���[�������o�߂����甒�ɂ���
#define REAR_FRAME_LENGTH_WHITE      (  60 )  // STEP_EVO_WHITE�ɂȂ��Ă��炱�̃t���[�������o�߂�����J���[�ɂ���  // REAR_WHITE_ANIME_STATE_WHITE�ɂȂ��Ă��炱�̃t���[�������o�߂�����REAR_WHITE_ANIME_STATE_WHITE_TO_COLOR�ɂ���


// �p�[�e�B�N����Y���W
#define PARTICLE_Y  (FX_F32_TO_FX32(0.0f))


#ifdef SET_PARTICLE_Z_MODE
f32 particle_z = 0.0f;
#endif

#ifdef SET_REAR_FRAME_LENGTH_COLOR_MODE
u32 rear_frame_length_color = REAR_FRAME_LENGTH_COLOR;
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
  PARTICLE_SPA_FILE_0,      // ARCID_SHINKA_DEMO    // NARC_shinka_demo_particle_shinka_demo_spa
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
  { ARCID_SHINKA_DEMO, NARC_shinka_demo_particle_shinka_demo_spa },
};

// �p�[�e�B�N���̃|���S��ID
#define PARTICLE_FIX_POLYGON_ID ( 5)
#define PARTICLE_MIN_POLYGON_ID ( 6)
#define PARTICLE_MAX_POLYGON_ID (59)


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
  {    0,     PARTICLE_SPA_FILE_0,         DEMO_SINKA01A },
  {    0,     PARTICLE_SPA_FILE_0,         DEMO_SINKA01 },
  {    0,     PARTICLE_SPA_FILE_0,         DEMO_SINKA02 },
  {    0,     PARTICLE_SPA_FILE_0,         DEMO_SINKA03 },
  {    0,     PARTICLE_SPA_FILE_0,         DEMO_SINKA04 },
  {  500,     PARTICLE_SPA_FILE_0,         DEMO_SINKA05 },
  {  500,     PARTICLE_SPA_FILE_0,         DEMO_SINKA06 },
  {  570,     PARTICLE_SPA_FILE_0,         DEMO_SINKA07 },
  {  840,     PARTICLE_SPA_FILE_0,         DEMO_SINKA08 },
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
struct _SHINKADEMO_EFFECT_WORK
{
  // ���̂Ƃ���̂��o���Ă��邾���Ő�����j���͂��Ȃ��B
  HEAPID                   heap_id;
  
  // SHINKADEMO_EFFECT_Init�̈���
  SHINKADEMO_EFFECT_LAUNCH   launch;

  // �X�e�b�v
  STEP                     step;
  u32                      wait_count;

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
static void ShinkaDemo_Effect_ThreeInit( SHINKADEMO_EFFECT_WORK* work );
static void ShinkaDemo_Effect_ThreeExit( SHINKADEMO_EFFECT_WORK* work );
static void ShinkaDemo_Effect_ThreeDraw( SHINKADEMO_EFFECT_WORK* work );  // 3D�`��(GRAPHIC_3D_StartDraw��PSEL_GRAPHIC_3D_EndDraw�̊ԂŌĂ�)
// 3D��
static void ShinkaDemo_Effect_ThreeRearInit( SHINKADEMO_EFFECT_WORK* work );
static void ShinkaDemo_Effect_ThreeRearExit( SHINKADEMO_EFFECT_WORK* work );
static void ShinkaDemo_Effect_ThreeRearMain( SHINKADEMO_EFFECT_WORK* work );
static void ShinkaDemo_Effect_ThreeRearStartColorToWhite( SHINKADEMO_EFFECT_WORK* work );
static BOOL ShinkaDemo_Effect_ThreeRearIsWhite( SHINKADEMO_EFFECT_WORK* work );
static void ShinkaDemo_Effect_ThreeRearStartWhiteToColor( SHINKADEMO_EFFECT_WORK* work );
static BOOL ShinkaDemo_Effect_ThreeRearIsColor( SHINKADEMO_EFFECT_WORK* work );


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
 *  @retval        SHINKADEMO_EFFECT_WORK
 */
//-----------------------------------------------------------------------------
SHINKADEMO_EFFECT_WORK* SHINKADEMO_EFFECT_Init(
                               HEAPID                     heap_id,
                               SHINKADEMO_EFFECT_LAUNCH   launch
                             )
{
  SHINKADEMO_EFFECT_WORK* work;

  // ���[�N
  {
    work = GFL_HEAP_AllocClearMemory( heap_id, sizeof(SHINKADEMO_EFFECT_WORK) );
  }

  // ����
  {
    work->heap_id      = heap_id;
    work->launch       = launch;
  }

  // �X�e�b�v
  {
    if( work->launch == SHINKADEMO_EFFECT_LAUNCH_EVO )
      work->step         = STEP_EVO_START;
    else
      work->step         = STEP_AFTER;
  }
  work->wait_count = 0;
 
  // �p�[�e�B�N��
  work->particle_mgr = Particle_Init( work->heap_id, NELEMS(particle_play_data_tbl), particle_play_data_tbl );

  // 3D
  ShinkaDemo_Effect_ThreeInit( work );
  ShinkaDemo_Effect_ThreeRearInit( work );

  return work;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �I������ 
 *
 *  @param[in,out] work  SHINKADEMO_EFFECT_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_EFFECT_Exit( SHINKADEMO_EFFECT_WORK* work )
{
  // 3D
  ShinkaDemo_Effect_ThreeRearExit( work );
  ShinkaDemo_Effect_ThreeExit( work );

  // �p�[�e�B�N��
  Particle_Exit( work->particle_mgr );

  // ���[�N
  {
    GFL_HEAP_FreeMemory( work );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �又�� 
 *
 *  @param[in,out] work  SHINKADEMO_EFFECT_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_EFFECT_Main( SHINKADEMO_EFFECT_WORK* work )
{
  switch(work->step)
  {
  // EVO
  case STEP_EVO_START:
    {
    }
    break;
  case STEP_EVO_DEMO:
    {
/*
      �O����SHINKADEMO_EFFECT_StartWhite�֐��Őݒ肷�邱�Ƃɂ���
      if( work->wait_count == 0 )
      {
        work->step = STEP_EVO_TO_WHITE_START;
      }
      else
      {
        work->wait_count--;
      }
*/
    }
    break;
  case STEP_EVO_TO_WHITE_START:
    {
      ShinkaDemo_Effect_ThreeRearStartColorToWhite( work );
      work->step = STEP_EVO_TO_WHITE;
    }
    break;
  case STEP_EVO_TO_WHITE:
    {
      if( ShinkaDemo_Effect_ThreeRearIsWhite( work ) )
      {
        work->step = STEP_EVO_WHITE;
        work->wait_count = REAR_FRAME_LENGTH_WHITE;
      }
    }
    break;
  case STEP_EVO_WHITE:
    {
      if( work->wait_count == 0 )
      {
        ShinkaDemo_Effect_ThreeRearStartWhiteToColor( work );
        work->step = STEP_EVO_FROM_WHITE;
      }
      else
      {
        work->wait_count--;
      }
    }
    break;
  case STEP_EVO_FROM_WHITE:
    {
      if( ShinkaDemo_Effect_ThreeRearIsColor( work ) )
      {
        work->step = STEP_EVO_END;
      }
    }
    break;
  case STEP_EVO_END:
    {
    }
    break;

  // AFTER
  case STEP_AFTER:
    {
    }
    break;
  }


#ifdef SET_PARTICLE_Z_MODE
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
  {
    OS_Printf( "particle_z = %f\n", particle_z );
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
  {
    particle_z += 0.1f;
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
  {
    particle_z -= 0.1f;
  }
#endif

#ifdef SET_REAR_FRAME_LENGTH_COLOR_MODE
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
  {
    OS_Printf( "rear_frame_length_color = %d\n", rear_frame_length_color );
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
  {
    rear_frame_length_color += 10;
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
  {
    rear_frame_length_color -= 10;
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
  {
    rear_frame_length_color += 1;
  }
  else if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
  {
    rear_frame_length_color -= 1;
  }
#endif

  // 3D
  ShinkaDemo_Effect_ThreeRearMain( work );

  // �p�[�e�B�N��
  Particle_Main( work->particle_mgr );
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �`�揈�� 
 *
 *  @param[in,out] work  SHINKADEMO_EFFECT_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 *
 *  @note          GRAPHIC_3D_StartDraw��GRAPHIC_3D_EndDraw�̊ԂŌĂԂ���
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_EFFECT_Draw( SHINKADEMO_EFFECT_WORK* work )
{
  // 3D
  ShinkaDemo_Effect_ThreeDraw( work );

  // �p�[�e�B�N��
  Particle_Draw( work->particle_mgr );
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �i���L�����Z��
 *
 *  @param[in,out] work  SHINKADEMO_EFFECT_Init�Ő����������[�N
 *
 *  @retval        �Ȃ�
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_EFFECT_Cancel( SHINKADEMO_EFFECT_WORK* work )
{
  // �p�[�e�B�N��
  Particle_Stop( work->particle_mgr, 30 );

  // 3D
  if( work->step == STEP_EVO_DEMO )
  {
    work->step = STEP_EVO_TO_WHITE_START;
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief         �X�^�[�g
 *
 *  @param[in,out] work  SHINKADEMO_EFFECT_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_EFFECT_Start( SHINKADEMO_EFFECT_WORK* work )
{
  // �p�[�e�B�N��
  Particle_Start( work->particle_mgr );

  work->step = STEP_EVO_DEMO;
  work->wait_count = REAR_FRAME_LENGTH_COLOR;

#ifdef SET_REAR_FRAME_LENGTH_COLOR_MODE
  work->wait_count = rear_frame_length_color;
#endif
}

//-----------------------------------------------------------------------------
/**
 *  @brief         REAR�𔒂���΂��A�j���J�n
 *
 *  @param[in,out] work  SHINKADEMO_EFFECT_Init�Ő����������[�N
 *
 *  @retval        �Ȃ� 
 */
//-----------------------------------------------------------------------------
void SHINKADEMO_EFFECT_StartWhite( SHINKADEMO_EFFECT_WORK* work )
{
  // 3D
  if( work->step == STEP_EVO_DEMO )
  {
    work->step = STEP_EVO_TO_WHITE_START;
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief         REAR�����S�ɔ������ł��邩
 *
 *  @param[in,out] work  SHINKADEMO_EFFECT_Init�Ő����������[�N
 *
 *  @retval        REAR�����S�ɔ������ł���Ƃ�TRUE 
 */
//-----------------------------------------------------------------------------
BOOL SHINKADEMO_EFFECT_IsWhite( SHINKADEMO_EFFECT_WORK* work )
{
  // 3D
  if( work->step == STEP_EVO_WHITE )
  {
    return TRUE;
  }
  return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief         REAR��������߂�n�߂Ă��邩
 *
 *  @param[in,out] work  SHINKADEMO_EFFECT_Init�Ő����������[�N
 *
 *  @retval        REAR��������߂�n�߂Ă���Ƃ�TRUE 
 */
//-----------------------------------------------------------------------------
BOOL SHINKADEMO_EFFECT_IsFromWhite( SHINKADEMO_EFFECT_WORK* work )
{
  // 3D
  if(    work->step == STEP_EVO_WHITE
      && work->wait_count <= 1 )
  {
    return TRUE;
  }
  else if( work->step == STEP_EVO_FROM_WHITE )
  {
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
/// �p�[�e�B�N��
//=====================================
static PARTICLE_MANAGER* Particle_Init( HEAPID heap_id, u16 data_num, const PARTICLE_PLAY_DATA* data_tbl )
{
  PARTICLE_MANAGER* mgr;
  u8 i;

  // �p�[�e�B�N����p�J����	
/*
  VecFx32 cam_eye = {            0,         0,   FX32_CONST(10) };
  VecFx32 cam_up  = {            0,  FX32_ONE,                0 };
  VecFx32 cam_at  = {            0,         0,                0 };
*/
/*
  VecFx32 cam_eye = {            0,         0,   FX32_CONST(2) };
  VecFx32 cam_up  = {            0,  FX32_ONE,                0 };
  VecFx32 cam_at  = {            0,         0,                0 };
*/

// �S�������Ȃ�
//  VecFx32 cam_eye = {            0,  FX32_CONST(10),  FX32_CONST(100) };
//  VecFx32 cam_up  = {            0,        FX32_ONE,                0 };
//  VecFx32 cam_at  = {            0,               0,                0 };

// ����������������
//  VecFx32 cam_eye = {            0,  FX32_CONST(1),  FX32_CONST(10) };
//  VecFx32 cam_up  = {            0,        FX32_ONE,                0 };
//  VecFx32 cam_at  = {            0,               0,                0 };

// ����������Ƌ߂��Ă���������
//  VecFx32 cam_eye = {            0,  FX32_CONST(0),  FX32_CONST(15) };
//  VecFx32 cam_up  = {            0,        FX32_ONE,                0 };
//  VecFx32 cam_at  = {            0,               0,                0 };

  VecFx32 cam_eye = {            0,  FX32_CONST(0),  FX32_CONST(13) };
  VecFx32 cam_up  = {            0,        FX32_ONE,                0 };
  VecFx32 cam_at  = {            0,               0,                0 };


/*
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
*/
/*
  GFL_G3D_PROJECTION proj; 
	{
		proj.type      = GFL_G3D_PRJPERS;
		proj.param1    = FX_SinIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT );
		proj.param2    = FX_CosIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT );
		proj.param3    = defaultCameraAspect;
		proj.param4    = 0;
		proj.near      = defaultCameraNear;
		proj.far       = defaultCameraFar;
		proj.scaleW    = 0;
	}
*/
/*
  GFL_G3D_PROJECTION proj; 
	{
		proj.type      = GFL_G3D_PRJPERS;
		proj.param1    = FX_SinIdx( 80/2 *PERSPWAY_COEFFICIENT );
		proj.param2    = FX_CosIdx( 80/2 *PERSPWAY_COEFFICIENT );
		proj.param3    = defaultCameraAspect;
		proj.param4    = 0;
		proj.near      = defaultCameraNear;
		proj.far       = defaultCameraFar;
		proj.scaleW    = 0;
	}
*/
  GFL_G3D_PROJECTION proj; 
	{
		proj.type      = GFL_G3D_PRJPERS;
		proj.param1    = FX_SinIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT );
		proj.param2    = FX_CosIdx( defaultCameraFovy/2 *PERSPWAY_COEFFICIENT );
		proj.param3    = defaultCameraAspect;
		proj.param4    = 0;
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

    //mgr->spa_set[i].ptc = GFL_PTC_Create( mgr->spa_set[i].wk, sizeof(mgr->spa_set[i].wk), TRUE, heap_id );
    
    //mgr->spa_set[i].ptc = GFL_PTC_Create2( mgr->spa_set[i].wk, sizeof(mgr->spa_set[i].wk), TRUE, heap_id, 5, 6, 63 );  // 3D�̃|���S��ID��5�ɂ��Ă��邪�A���ꂾ�ƃp�[�e�B�N����3D�ɂ���Č����Ă��܂����Ƃ�����B�P����Z���p�[�e�B�N���̂ق������Ȃ̂��낤���H
    //mgr->spa_set[i].ptc = GFL_PTC_Create2( mgr->spa_set[i].wk, sizeof(mgr->spa_set[i].wk), TRUE, heap_id, 5, 6, 59 );  // ���ꂾ�ƃp�[�e�B�N���������Ȃ��B6����59��肩�Ȃ菬�����͈͂��ƁA�p�[�e�B�N�����m�Ō����Ă��܂��B
    mgr->spa_set[i].ptc = GFL_PTC_CreateEx( mgr->spa_set[i].wk, sizeof(mgr->spa_set[i].wk), TRUE,
        PARTICLE_FIX_POLYGON_ID, PARTICLE_MIN_POLYGON_ID, PARTICLE_MAX_POLYGON_ID, heap_id );

    //GFL_PTC_PersonalCameraDelete( mgr->spa_set[i].ptc );
		//GFL_PTC_PersonalCameraCreate( mgr->spa_set[i].ptc, &proj, DEFAULT_PERSP_WAY, &cam_eye, &cam_up, &cam_at, heap_id );
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


static void Particle_EmitFunc( GFL_EMIT_PTR emit_ptr );
static void Particle_EmitFunc( GFL_EMIT_PTR emit_ptr )
{
  void* temp_ptr;


  VecFx32 pos = { 0, PARTICLE_Y, 0 };//FX_F32_TO_FX32(0.5f), 0 };

#ifdef SET_PARTICLE_Z_MODE
    pos.z = FX_F32_TO_FX32(particle_z);
#endif
 

  temp_ptr = GFL_PTC_GetTempPtr();


  GFL_PTC_SetEmitterPosition( emit_ptr, &pos );


  if( emit_ptr )
  {
    SPLEmitter* spl_emitter = emit_ptr;
    SPLParticle* spl_particle;
    spl_particle = spl_emitter->act_ptcl_list.p_begin;
    while( spl_particle )
    {
#ifdef DEBUG_CODE
      OS_Printf("%d\n", spl_particle->alp.current_polygonID );
#endif
      spl_particle = spl_particle->p_next;
    }    
  }
}


static void Particle_Main( PARTICLE_MANAGER* mgr )
{
  if( mgr->play )
  {
    VecFx32 pos = { 0, PARTICLE_Y, 0 };//FX_F32_TO_FX32(0.5f), 0 };
    GFL_EMIT_PTR emit;


#ifdef SET_PARTICLE_Z_MODE
    pos.z = FX_F32_TO_FX32(particle_z);
#endif
/*
    pos.z = FX_F32_TO_FX32(2.0f);
*/

    while( mgr->data_no < mgr->data_num )
    {
      if( mgr->frame == mgr->data_tbl[mgr->data_no].frame )
      {
        emit = GFL_PTC_CreateEmitter( mgr->spa_set[ mgr->data_tbl[mgr->data_no].spa_idx ].ptc, mgr->data_tbl[mgr->data_no].res_no, &pos );
        //emit = GFL_PTC_CreateEmitterCallback( mgr->spa_set[ mgr->data_tbl[mgr->data_no].spa_idx ].ptc, mgr->data_tbl[mgr->data_no].res_no, Particle_EmitFunc, NULL );

#ifdef DEBUG_CODE
        {
          OS_Printf( "SHINKADEMO_EFFECT : frame=%d, jspa_idx=%d, res_no=%d, emit=%p\n",
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
static void ShinkaDemo_Effect_ThreeInit( SHINKADEMO_EFFECT_WORK* work )
{
  // �����_�����O�X���b�v�o�b�t�@
  //GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_W );

  // 3D�Ǘ����[�e�B���e�B�[�̃Z�b�g�A�b�v
  work->three_util = GFL_G3D_UTIL_Create( THREE_RES_MAX, THREE_OBJ_MAX, work->heap_id );

  // NULL�A�[��������
  work->three_obj_prop_num = 0;
}
static void ShinkaDemo_Effect_ThreeExit( SHINKADEMO_EFFECT_WORK* work )
{
  // 3D�Ǘ����[�e�B���e�B�[�̔j��
  GFL_G3D_UTIL_Delete( work->three_util );
}
static void ShinkaDemo_Effect_ThreeDraw( SHINKADEMO_EFFECT_WORK* work )  // 3D�`��(GRAPHIC_3D_StartDraw��GRAPHIC_3D_EndDraw�̊ԂŌĂ�)
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
static void ShinkaDemo_Effect_ThreeRearInit( SHINKADEMO_EFFECT_WORK* work )
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
        VEC_Set( &(prop->objstatus.trans), 0, FX_F32_TO_FX32(0.0f), FX_F32_TO_FX32(0.0f) );
/*
        VEC_Set( &(prop->objstatus.trans), 0, FX_F32_TO_FX32(0.0f), FX_F32_TO_FX32(-600.0f) );
*/
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
static void ShinkaDemo_Effect_ThreeRearExit( SHINKADEMO_EFFECT_WORK* work )
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
static void ShinkaDemo_Effect_ThreeRearMain( SHINKADEMO_EFFECT_WORK* work )
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
  }
}
static void ShinkaDemo_Effect_ThreeRearStartColorToWhite( SHINKADEMO_EFFECT_WORK* work )
{
  work->rear_white_anime_state = REAR_WHITE_ANIME_STATE_COLOR_TO_WHITE;
}
static BOOL ShinkaDemo_Effect_ThreeRearIsWhite( SHINKADEMO_EFFECT_WORK* work )
{
  return ( work->rear_white_anime_state == REAR_WHITE_ANIME_STATE_WHITE );
}
static void ShinkaDemo_Effect_ThreeRearStartWhiteToColor( SHINKADEMO_EFFECT_WORK* work )
{
  work->rear_white_anime_state = REAR_WHITE_ANIME_STATE_WHITE_TO_COLOR;
}
static BOOL ShinkaDemo_Effect_ThreeRearIsColor( SHINKADEMO_EFFECT_WORK* work )
{
  return ( work->rear_white_anime_state == REAR_WHITE_ANIME_STATE_COLOR );
}

