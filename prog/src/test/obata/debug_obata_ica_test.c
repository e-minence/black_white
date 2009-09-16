#include <gflib.h>
#include "debug_obata_ica_test_setup.h"
#include "system/ica_anime.h"
#include "system/main.h"
#include "arc/arc_def.h"
#include "arc/debug_obata.naix"


//============================================================================================
/**
 * @brief 3D�f�[�^
 */
//============================================================================================

// �T�C�R��
static const GFL_G3D_UTIL_RES res_table_dice[] = 
{
  { ARCID_OBATA_DEBUG, NARC_debug_obata_dice_nsbmd, GFL_G3D_UTIL_RESARC },
};
static const GFL_G3D_UTIL_OBJ obj_table_dice[] = 
{
  {
    0,     // ���f�����\�[�XID
    0,     // ���f���f�[�^ID(���\�[�X����INDEX)
    0,     // �e�N�X�`�����\�[�XID
    NULL,  // �A�j���e�[�u��(�����w��̂���)
    0,     // �A�j�����\�[�X��
  },
}; 

// �Z�b�g�A�b�v�ԍ�
enum
{
  SETUP_INDEX_DICE,
  SETUP_INDEX_MAX
};

// �Z�b�g�A�b�v�f�[�^
static const GFL_G3D_UTIL_SETUP setup[] =
{
  { res_table_dice, NELEMS(res_table_dice), obj_table_dice, NELEMS(obj_table_dice) },
};

// �A�j���[�V�����f�[�^
typedef enum
{
  ANIME_SRT,
  ANIME_SR,
  ANIME_ST,
  ANIME_RT,
  ANIME_S,
  ANIME_R,
  ANIME_T,
  ANIME_MODE_NUM
} ANIME_MODE;

ARCDATID anime_dat_id[] = 
{
  NARC_debug_obata_dice_anime_srt_bin,
  NARC_debug_obata_dice_anime_sr_bin,
  NARC_debug_obata_dice_anime_st_bin,
  NARC_debug_obata_dice_anime_rt_bin,
  NARC_debug_obata_dice_anime_s_bin,
  NARC_debug_obata_dice_anime_r_bin,
  NARC_debug_obata_dice_anime_t_bin,
};


//============================================================================================
/**
 * @breif ���[�N
 */
//============================================================================================
typedef struct
{
  GFL_G3D_UTIL* g3dUtil;
  u16 unitIndex[ SETUP_INDEX_MAX ];

  ANIME_MODE animeMode;
  ICA_ANIME* icaAnime;
  GFL_G3D_CAMERA* camera;
}
PROC_WORK;


//============================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//============================================================================================
static void Initialize( PROC_WORK* work );
static void Finalize( PROC_WORK* work );
static BOOL Main( PROC_WORK* work );
static void Draw( PROC_WORK* work );
static void DrawDice( GFL_G3D_OBJ* obj, ICA_ANIME* anime );
static void SetAnimeMode( PROC_WORK* work, ANIME_MODE );


//============================================================================================
/**
 * @brief �������֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_ICA_TEST_MainProcFunc_Init( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	PROC_WORK* work = NULL;

  // �q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OBATA_DEBUG, 0x100000 );

	// ���[�N���쐬
	work = GFL_PROC_AllocWork( proc, sizeof( PROC_WORK ), HEAPID_OBATA_DEBUG );

  // ����������
  DEBUG_OBATA_ICA_TEST_Init( HEAPID_OBATA_DEBUG );
  Initialize( work );
  SetAnimeMode( work, ANIME_SRT );

	return GFL_PROC_RES_FINISH;
}


//============================================================================================
/**
 * @brief ���C���֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_ICA_TEST_MainProcFunc_Main( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	PROC_WORK* work = mywk;
  BOOL end = FALSE;

  switch( *seq )
  {
  case 0:
    end = Main( work );
    Draw( work );
    if( end )
    {
      ++( *seq );
    }
    break;
  case 1:
    return GFL_PROC_RES_FINISH;
  }

  return GFL_PROC_RES_CONTINUE;
}


//============================================================================================
/**
 * @brief �I���֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_ICA_TEST_MainProcFunc_End( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	PROC_WORK* work = mywk;

  // �I������
  Finalize( work );

	// ���[�N��j��
  ICA_ANIME_Delete( work->icaAnime );
	GFL_PROC_FreeWork( proc );

  DEBUG_OBATA_ICA_TEST_Exit();

  // �q�[�v��j��
  GFL_HEAP_DeleteHeap( HEAPID_OBATA_DEBUG );

	return GFL_PROC_RES_FINISH;
} 


//============================================================================================
/**
 * @brief �v���Z�X��`�f�[�^
 */
//============================================================================================
const GFL_PROC_DATA DebugObataIcaTestMainProcData = 
{
	DEBUG_OBATA_ICA_TEST_MainProcFunc_Init,
	DEBUG_OBATA_ICA_TEST_MainProcFunc_Main,
	DEBUG_OBATA_ICA_TEST_MainProcFunc_End,
};


//============================================================================================
/**
 * @brief ����J�֐��̒�`
 */
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @breif ������
 */
//-------------------------------------------------------------------------------------------- 
static void Initialize( PROC_WORK* work )
{
  // 3D�Ǘ����[�e�B���e�B�[�̃Z�b�g�A�b�v
  work->g3dUtil = GFL_G3D_UTIL_Create( 10, 10, HEAPID_OBATA_DEBUG );

  // ���j�b�g�ǉ�
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      work->unitIndex[i] = GFL_G3D_UTIL_AddUnit( work->g3dUtil, &setup[i] );
    }
  }

  // �A�j���[�V�����L����
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      int j;
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[i] );
      int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_EnableAnime( obj, j );
      }
    }
  }

  // ica�f�[�^�����[�h
  work->icaAnime = ICA_ANIME_CreateStreamingAlloc(
      HEAPID_OBATA_DEBUG, ARCID_OBATA_DEBUG, NARC_debug_obata_dice_anime_srt_bin, 10 );

  // �J�����쐬
  {
    VecFx32    pos = { 0, 0, 50 * FX32_ONE };
    VecFx32 target = { 0, 0, 0 };
    VecFx32     up = { 0, FX32_ONE, 0 };
    fx32       far = FX32_ONE * 4096;
    work->camera   = GFL_G3D_CAMERA_CreateDefault( &pos, &target, HEAPID_OBATA_DEBUG );
    GFL_G3D_CAMERA_GetCamUp( work->camera, &up );
    GFL_G3D_CAMERA_SetFar( work->camera, &far );
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @breif �I��
 */
//-------------------------------------------------------------------------------------------- 
static void Finalize( PROC_WORK* work )
{ 
  // �J�����j��
  GFL_G3D_CAMERA_Delete( work->camera );

  // ���j�b�g�j��
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      GFL_G3D_UTIL_DelUnit( work->g3dUtil, work->unitIndex[i] );
    }
  }

  // 3D�Ǘ����[�e�B���e�B�[�̔j��
  GFL_G3D_UTIL_Delete( work->g3dUtil );
}

//--------------------------------------------------------------------------------------------
/**
 * @breif ���C������ 
 */
//-------------------------------------------------------------------------------------------- 
static BOOL Main( PROC_WORK* work )
{
  int trg = GFL_UI_KEY_GetTrg();
  int key = GFL_UI_KEY_GetCont();

  // ���[�h�ؑ�
  if( trg & PAD_BUTTON_A )
  {
    ANIME_MODE next = (work->animeMode + 1) % ANIME_MODE_NUM;
    SetAnimeMode( work, next );
  }
  // �Z���N�g�ŏI��
  if( trg & PAD_BUTTON_SELECT ) return TRUE;
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @breif �`��
 */
//-------------------------------------------------------------------------------------------- 
static void Draw( PROC_WORK* work )
{
  const fx32 anime_speed = FX32_ONE;
  GFL_G3D_OBJSTATUS status;

  VEC_Set( &status.trans, 0, 0, 0 );
  VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &status.rotate );

  // �J�����X�V
  GFL_G3D_CAMERA_Switching( work->camera );

  // �`��
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[SETUP_INDEX_DICE] );
    DrawDice( obj, work->icaAnime );
  }
  GFL_G3D_DRAW_End();

  // �A�j���[�V������i�߂�
  ICA_ANIME_IncAnimeFrame( work->icaAnime, anime_speed );
}


//--------------------------------------------------------------------------------------------
/**
 * @breif �T�C�R���`��
 */
//-------------------------------------------------------------------------------------------- 
static void DrawDice( GFL_G3D_OBJ* obj, ICA_ANIME* anime )
{
  VecFx32 scaleval, rotval, transval;
  BOOL havescale, haverot, havetrans;
  GFL_G3D_OBJSTATUS status;

  VEC_Set( &status.trans, 0, 0, 0 );
  VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &status.rotate );

  havetrans = ICA_ANIME_GetTranslate( anime, &transval );
  haverot   = ICA_ANIME_GetRotate( anime, &rotval );
  havescale = ICA_ANIME_GetScale( anime, &scaleval );

  if( havetrans )
  {
    VEC_Set( &status.trans, transval.x, transval.y, transval.z );
  }
  if( haverot )
  {
    float x, y, z;
    u16 rx, ry, rz;
    x = FX_FX32_TO_F32( rotval.x );
    y = FX_FX32_TO_F32( rotval.y );
    z = FX_FX32_TO_F32( rotval.z );
    while( x < 0 ) x += 360.0f;
    while( y < 0 ) y += 360.0f;
    while( z < 0 ) z += 360.0f;
    rx = x / 360.0f * 0xffff;
    ry = y / 360.0f * 0xffff;
    rz = z / 360.0f * 0xffff; 
    GFL_CALC3D_MTX_CreateRot( rx, ry, rz, &status.rotate );
  }
  if( havescale )
  {
    VEC_Set( &status.scale, scaleval.x, scaleval.y, scaleval.z );
  }

  // �`��
  GFL_G3D_DRAW_DrawObject( obj, &status );
}

//--------------------------------------------------------------------------------------------
/**
 * @breif �T�C�R���`��
 */
//-------------------------------------------------------------------------------------------- 
static void SetAnimeMode( PROC_WORK* work, ANIME_MODE mode )
{ 
  work->animeMode = mode;
  ICA_ANIME_Delete( work->icaAnime );
  work->icaAnime = ICA_ANIME_CreateStreamingAlloc(
      HEAPID_OBATA_DEBUG, ARCID_OBATA_DEBUG, anime_dat_id[ mode ], 10 );
}
