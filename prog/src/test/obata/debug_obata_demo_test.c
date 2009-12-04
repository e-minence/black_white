#include <gflib.h>
#include "debug_obata_demo_test_setup.h"
#include "system/ica_anime.h"
#include "system/ica_camera.h"
#include "system/main.h"
#include "arc/arc_def.h"
#include "arc/debug_obata.naix"



//============================================================================================
/**
 * @brief 3D�f�[�^
 */
//============================================================================================

// �z�E�I�E
static const GFL_G3D_UTIL_RES res_table_houou[] = 
{
  { ARCID_OBATA_DEBUG, NARC_debug_obata_houou_test_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_OBATA_DEBUG, NARC_debug_obata_houou_test_nsbca, GFL_G3D_UTIL_RESARC },
  { ARCID_OBATA_DEBUG, NARC_debug_obata_houou_test_nsbta, GFL_G3D_UTIL_RESARC },
};
static const GFL_G3D_UTIL_ANM anm_table_houou[] = 
{
  { 1, 0 },
  { 2, 0 },
};
static const GFL_G3D_UTIL_OBJ obj_table_houou[] = 
{
  {
    0,                         // ���f�����\�[�XID
    0,                         // ���f���f�[�^ID(���\�[�X����INDEX)
    0,                         // �e�N�X�`�����\�[�XID
    anm_table_houou,           // �A�j���e�[�u��(�����w��̂���)
    NELEMS(anm_table_houou),   // �A�j�����\�[�X��
  },
}; 

// �Z�b�g�A�b�v�ԍ�
enum
{
  SETUP_INDEX_HOUOU,
  SETUP_INDEX_MAX
};

// �Z�b�g�A�b�v�f�[�^
static const GFL_G3D_UTIL_SETUP setup[] =
{
  { res_table_houou, NELEMS(res_table_houou), obj_table_houou, NELEMS(obj_table_houou) },
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


//============================================================================================
/**
 * @brief �������֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_DEMO_TEST_MainProcFunc_Init( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	PROC_WORK* work = NULL;

  // �q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OBATA_DEBUG, 0x100000 );

	// ���[�N���쐬
	work = GFL_PROC_AllocWork( proc, sizeof( PROC_WORK ), HEAPID_OBATA_DEBUG );

  // ����������
  DEBUG_OBATA_DEMO_TEST_Init( HEAPID_OBATA_DEBUG );
  Initialize( work );

	return GFL_PROC_RES_FINISH;
}


//============================================================================================
/**
 * @brief ���C���֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_DEMO_TEST_MainProcFunc_Main( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
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
static GFL_PROC_RESULT DEBUG_OBATA_DEMO_TEST_MainProcFunc_End( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	PROC_WORK* work = mywk;

  // �I������
  ICA_ANIME_Delete( work->icaAnime );
  Finalize( work );

	// ���[�N��j��
	GFL_PROC_FreeWork( proc );

  DEBUG_OBATA_DEMO_TEST_Exit();

  // �q�[�v��j��
  GFL_HEAP_DeleteHeap( HEAPID_OBATA_DEBUG );

	return GFL_PROC_RES_FINISH;
} 


//============================================================================================
/**
 * @brief �v���Z�X��`�f�[�^
 */
//============================================================================================
const GFL_PROC_DATA DebugObataDemoTestMainProcData = 
{
	DEBUG_OBATA_DEMO_TEST_MainProcFunc_Init,
	DEBUG_OBATA_DEMO_TEST_MainProcFunc_Main,
	DEBUG_OBATA_DEMO_TEST_MainProcFunc_End,
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
      HEAPID_OBATA_DEBUG, ARCID_OBATA_DEBUG, NARC_debug_obata_ica_test_data2_bin, 10 );

  // �J�����쐬
  {
    VecFx32    pos = { 0, 0, 0 };
    VecFx32 target = { 0, 0, 0 };
    work->camera   = GFL_G3D_CAMERA_CreateDefault( &pos, &target, HEAPID_OBATA_DEBUG );
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
  static fx32 frame = 0;
  static fx32 anime_speed = FX32_ONE;
  GFL_G3D_OBJSTATUS status;

  VEC_Set( &status.trans, 0, 0, 0 );
  VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &status.rotate );

  // �J�����X�V
  ICA_CAMERA_SetCameraStatus( work->camera, work->icaAnime );
  GFL_G3D_CAMERA_Switching( work->camera );

  // TEMP: �J�����ݒ�
  {
    fx32 far = FX32_ONE * 4096;
    GFL_G3D_CAMERA_SetFar( work->camera, &far );
  }

  // �A�j���[�V�����X�V
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      int j;
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[i] );
      int anime_count = GFL_G3D_OBJECT_GetAnimeCount( obj );
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_LoopAnimeFrame( obj, j, anime_speed );
      }
    }
  }

  // �`��
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    int i;
    for( i=0; i<SETUP_INDEX_MAX; i++ )
    {
      GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[i] );
      GFL_G3D_DRAW_DrawObject( obj, &status );
    }
  }
  GFL_G3D_DRAW_End();

  frame += anime_speed;
  ICA_ANIME_IncAnimeFrame( work->icaAnime, anime_speed );
}
