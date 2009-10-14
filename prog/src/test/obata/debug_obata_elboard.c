#include <gflib.h>
#include "debug_obata_elboard_setup.h"
#include "system/ica_anime.h"
#include "system/main.h"
#include "arc/arc_def.h"
#include "arc/debug_obata.naix"


//============================================================================================
// �����\�[�X
//============================================================================================

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
static ARCDATID anime_dat_id[] = 
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
  u32 frame;
  GFL_G3D_UTIL* g3dUtil;
  u16 unitIndex[ SETUP_INDEX_MAX ];

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
static void UpdateCamera( PROC_WORK* work );


//============================================================================================
/**
 * @brief �������֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_ELBOARD_MainProcFunc_Init( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	PROC_WORK* work = NULL;

  // �q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OBATA_DEBUG, 0x100000 );

	// ���[�N���쐬
	work = GFL_PROC_AllocWork( proc, sizeof( PROC_WORK ), HEAPID_OBATA_DEBUG );

  // ����������
  DEBUG_OBATA_ELBOARD_Init( HEAPID_OBATA_DEBUG );
  Initialize( work );

	return GFL_PROC_RES_FINISH;
}


//============================================================================================
/**
 * @brief ���C���֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_ELBOARD_MainProcFunc_Main( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
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
static GFL_PROC_RESULT DEBUG_OBATA_ELBOARD_MainProcFunc_End( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
	PROC_WORK* work = mywk;

  // �I������
  Finalize( work );

	// ���[�N��j��
	GFL_PROC_FreeWork( proc );

  DEBUG_OBATA_ELBOARD_Exit();

  // �q�[�v��j��
  GFL_HEAP_DeleteHeap( HEAPID_OBATA_DEBUG );

	return GFL_PROC_RES_FINISH;
} 


//============================================================================================
/**
 * @brief �v���Z�X��`�f�[�^
 */
//============================================================================================
const GFL_PROC_DATA DebugObataElboardMainProcData = 
{
	DEBUG_OBATA_ELBOARD_MainProcFunc_Init,
	DEBUG_OBATA_ELBOARD_MainProcFunc_Main,
	DEBUG_OBATA_ELBOARD_MainProcFunc_End,
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

  // �J�����쐬
  {
    VecFx32    pos = { 0, 0, 5*FX32_ONE };
    VecFx32 target = { 0, 0, 0 };
    VecFx32     up = { 0, FX32_ONE, 0 };
    fx32       far = FX32_ONE * 4096;
    work->camera   = GFL_G3D_CAMERA_CreateDefault( &pos, &target, HEAPID_OBATA_DEBUG );
    GFL_G3D_CAMERA_GetCamUp( work->camera, &up );
    GFL_G3D_CAMERA_SetFar( work->camera, &far );
  }

  // ���̑�������
  work->frame = 0;
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
  if( trg & PAD_BUTTON_SELECT ) return TRUE; // �Z���N�g�ŏI��

  // �t���[���J�E���^�X�V
  work->frame++;
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @breif �`��
 */
//-------------------------------------------------------------------------------------------- 
static void Draw( PROC_WORK* work )
{
  GFL_G3D_OBJSTATUS status; 
  VEC_Set( &status.trans, 0, 0, 0 );
  VEC_Set( &status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &status.rotate );

  // �J�����X�V
  UpdateCamera( work );
  GFL_G3D_CAMERA_Switching( work->camera );

  // �`��
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[SETUP_INDEX_DICE] );
    GFL_G3D_DRAW_DrawObject( obj, &status );
  }
  GFL_G3D_DRAW_End();
} 

//--------------------------------------------------------------------------------------------
/**
 * @breif �J�������X�V����
 */
//-------------------------------------------------------------------------------------------- 
static void UpdateCamera( PROC_WORK* work )
{
  GFL_G3D_CAMERA_Switching( work->camera );
}
