#include <wchar.h>    // wcslen
#include <gflib.h>
#include "debug_obata_elboard_setup.h"
#include "system/ica_anime.h"
#include "system/main.h"
#include "arc/arc_def.h"
#include "arc/debug_obata.naix"
#include "system/el_scoreboard.h"


//============================================================================================
// �����\�[�X
//============================================================================================

// ���\�[�X�C���f�b�N�X
enum
{
  RES_ELBOARD_NSBMD,
  RES_ELBOARD_NSBTX,
  RES_ELBOARD_NSBTA_1,
  RES_ELBOARD_NSBTA_2,
  RES_ELBOARD_NSBTA_3,
  RES_ELBOARD_NSBTA_4,
  RES_ELBOARD_NSBTA_5,
  RES_ELBOARD_NSBTA_6,
  RES_NUM
}; 
static const GFL_G3D_UTIL_RES res_table[] = 
{
  { ARCID_OBATA_DEBUG, NARC_debug_obata_gelboard01_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_OBATA_DEBUG, NARC_debug_obata_gelboard01_nsbtx, GFL_G3D_UTIL_RESARC },
  { ARCID_OBATA_DEBUG, NARC_debug_obata_gelboard01_1_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_OBATA_DEBUG, NARC_debug_obata_gelboard01_2_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_OBATA_DEBUG, NARC_debug_obata_gelboard01_3_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_OBATA_DEBUG, NARC_debug_obata_gelboard01_4_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_OBATA_DEBUG, NARC_debug_obata_gelboard01_5_nsbta, GFL_G3D_UTIL_RESARC },
  { ARCID_OBATA_DEBUG, NARC_debug_obata_gelboard01_6_nsbta, GFL_G3D_UTIL_RESARC },
};

// �A�j���C���f�b�N�X
enum
{
  ANM_ELBOARD_NEWS_1,
  ANM_ELBOARD_NEWS_2,
  ANM_ELBOARD_NEWS_3,
  ANM_ELBOARD_NEWS_4,
  ANM_ELBOARD_NEWS_5,
  ANM_ELBOARD_NEWS_6,
  ANM_NUM
}; 
static const GFL_G3D_UTIL_ANM anm_table[] = 
{
  // �A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ELBOARD_NSBTA_1, 0 },
  { RES_ELBOARD_NSBTA_2, 0 },
  { RES_ELBOARD_NSBTA_3, 0 },
  { RES_ELBOARD_NSBTA_4, 0 },
  { RES_ELBOARD_NSBTA_5, 0 },
  { RES_ELBOARD_NSBTA_6, 0 },
};

// �I�u�W�F�N�g�C���f�b�N�X
enum
{
  OBJ_ELBOARD,
  OBJ_NUM
};
static const GFL_G3D_UTIL_OBJ obj_table[] = 
{
  // ���f�����\�[�XID, 
  // ���f���f�[�^ID(���\�[�X����INDEX), 
  // �e�N�X�`�����\�[�XID,
  // �A�j���e�[�u��, 
  // �A�j�����\�[�X��
  { RES_ELBOARD_NSBMD, 0, RES_ELBOARD_NSBTX, anm_table, NELEMS(anm_table) },
}; 

// �Z�b�g�A�b�v�ԍ�
enum
{
  SETUP_INDEX_ELBOARD,
  SETUP_INDEX_MAX
}; 
static const GFL_G3D_UTIL_SETUP setup[] =
{
  { res_table, NELEMS(res_table), obj_table, NELEMS(obj_table) },
};


//============================================================================================
// ���萔
//============================================================================================

// �j���[�X�ԍ�
typedef enum
{
  NEWS_1,
  NEWS_2,
  NEWS_3,
  NEWS_4,
  NEWS_5,
  NEWS_6,
  NEWS_NUM,
  NEWS_MAX = NEWS_NUM-1
} NEWS_INDEX; 

// �e�N�X�`����
static const char* tex_name[NEWS_NUM] = 
{
  "gelboard_1",
  "gelboard_2",
  "gelboard_3",
  "gelboard_4",
  "gelboard_5",
  "gelboard_6",
};
// �p���b�g��
static const char* plt_name[NEWS_NUM] = 
{
  "gelboard_1_pl",
  "gelboard_2_pl",
  "gelboard_3_pl",
  "gelboard_4_pl",
  "gelboard_5_pl",
  "gelboard_6_pl",
};
// �������ޕ�����
static const STRCODE* news_str[NEWS_NUM] = 
{
  L"����������",
  L"����������",
  L"����������",
  L"�����Ă�",
  L"�Ȃɂʂ˂�",
  L"�͂Ђӂւ�",
};

// �j���[�X�A�j���E�C���f�b�N�X
static const int news_anm_index[NEWS_NUM] = 
{
  ANM_ELBOARD_NEWS_1, 
  ANM_ELBOARD_NEWS_2, 
  ANM_ELBOARD_NEWS_3, 
  ANM_ELBOARD_NEWS_4, 
  ANM_ELBOARD_NEWS_5, 
  ANM_ELBOARD_NEWS_6, 
};


//============================================================================================
// �����[�N
//============================================================================================
typedef struct
{
  // �t���[���J�E���^
  u32 frame;  

  // G3D_xxxx
  GFL_G3D_UTIL* g3dUtil;
  u16 unitIndex[ SETUP_INDEX_MAX ];

  // �d���f���̃e�N�X�`��
  EL_SCOREBOARD_TEX* elboardTex[NEWS_NUM];

  // �J����
  GFL_G3D_CAMERA* camera;

  // �\������e�N�X�`��
  int newsIndex;
}
PROC_WORK;


//============================================================================================
// ���v���g�^�C�v�錾
//============================================================================================
static void Initialize( PROC_WORK* work );
static void Finalize( PROC_WORK* work );
static BOOL Main( PROC_WORK* work );
static void Draw( PROC_WORK* work );
static void UpdateCamera( PROC_WORK* work );


//--------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_OBATA_ELBOARD_MainProcFunc_Init( 
    GFL_PROC* proc, int* seq, void* pwk, void* mywk )
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


//--------------------------------------------------------------------------------------------
/**
 * @brief ���C���֐�
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_OBATA_ELBOARD_MainProcFunc_Main( 
    GFL_PROC* proc, int* seq, void* pwk, void* mywk )
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


//--------------------------------------------------------------------------------------------
/**
 * @brief �I���֐�
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_OBATA_ELBOARD_MainProcFunc_End( 
    GFL_PROC* proc, int* seq, void* pwk, void* mywk )
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
// ���v���Z�X��`�f�[�^
//============================================================================================
const GFL_PROC_DATA DebugObataElboardMainProcData = 
{
	DEBUG_OBATA_ELBOARD_MainProcFunc_Init,
	DEBUG_OBATA_ELBOARD_MainProcFunc_Main,
	DEBUG_OBATA_ELBOARD_MainProcFunc_End,
};


//============================================================================================
// ������J�֐��̒�`
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
      OBATA_Printf( "%d\n", i );
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
      OBATA_Printf( "anime_count = %d\n", anime_count );
      for( j=0; j<anime_count; j++ )
      {
        GFL_G3D_OBJECT_DisableAnime( obj, j );
      }
      GFL_G3D_OBJECT_EnableAnime( obj, news_anm_index[NEWS_1] );
    }
  }

  // �����񏑂�����
  {
    int i;
    u16 obj_index_head = GFL_G3D_UTIL_GetUnitObjIdx( work->g3dUtil, SETUP_INDEX_ELBOARD );
    u16 obj_index_elboard = obj_index_head + OBJ_ELBOARD;
    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, obj_index_elboard );
    GFL_G3D_RND* rnd = GFL_G3D_OBJECT_GetG3Drnd( obj );
    GFL_G3D_RES* res = GFL_G3D_RENDER_GetG3DresTex( rnd );
    STRBUF* strbuf = GFL_STR_CreateBuffer( 128, HEAPID_OBATA_DEBUG );

    for( i=0; i<NEWS_NUM; i++ )
    {
      STRCODE strcode[128];
      int len = wcslen( news_str[i] );
      GFL_STD_MemCopy( news_str[i], strcode, sizeof( STRCODE )*len );
      strcode[len] = GFL_STR_GetEOMCode();
      GFL_STR_SetStringCode( strbuf, strcode );

      work->elboardTex[i] = ELBOARD_TEX_Add_Ex( 
          res, tex_name[i], plt_name[i], strbuf, 128, 2, HEAPID_OBATA_DEBUG );
    }
    GFL_HEAP_FreeMemory( strbuf );
  }

  // �J�����쐬
  { VecFx32    pos = { 0*FX32_ONE, 50*FX32_ONE, 200*FX32_ONE };
    VecFx32 target = { 0*FX32_ONE, 50*FX32_ONE, 0*FX32_ONE };
    VecFx32     up = { 0, FX32_ONE, 0 };
    fx32       far = FX32_ONE * 4096;
    work->camera   = GFL_G3D_CAMERA_CreateDefault( &pos, &target, HEAPID_OBATA_DEBUG );
    GFL_G3D_CAMERA_GetCamUp( work->camera, &up );
    GFL_G3D_CAMERA_SetFar( work->camera, &far );
  }

  // ���̑�������
  work->frame = 0;
  work->newsIndex = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @breif �I��
 */
//-------------------------------------------------------------------------------------------- 
static void Finalize( PROC_WORK* work )
{ 
  int i;

  // �J�����j��
  GFL_G3D_CAMERA_Delete( work->camera );

  // �d���f���̃e�N�X�`����j��
  for( i=0; i<NEWS_NUM; i++ )
  {
    ELBOARD_TEX_Delete( work->elboardTex[i] );
  }

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
  int i;
  int trg = GFL_UI_KEY_GetTrg();
  int key = GFL_UI_KEY_GetCont(); 
  if( trg & PAD_BUTTON_SELECT ) return TRUE; // �Z���N�g�ŏI��
  if( trg & PAD_BUTTON_A )
  { 
    // �A�j���[�V�����؂�ւ�
    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, SETUP_INDEX_ELBOARD );
    int prev = (work->newsIndex - 1 + NEWS_NUM) % NEWS_NUM;
    int next = (work->newsIndex + 1) % NEWS_NUM;
    OBATA_Printf( "prev = %d\n", prev );
    OBATA_Printf( "next = %d\n", next );
    GFL_G3D_OBJECT_DisableAnime( obj, news_anm_index[prev] );
    GFL_G3D_OBJECT_EnableAnime( obj, news_anm_index[next] );
    GFL_G3D_OBJECT_ResetAnimeFrame( obj, news_anm_index[next] );

    work->newsIndex = (work->newsIndex + 1) % NEWS_NUM;
  }

  // �t���[���J�E���^�X�V
  work->frame++;

  // �d���f���̃e�N�X�`���̓���
  for( i=0; i<NEWS_NUM; i++ )
  {
    ELBOARD_TEX_Main( work->elboardTex[i] );
  }

  // �d���f���̃A�j���[�V�����X�V
  {
    int i;
    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[SETUP_INDEX_ELBOARD] );
    for( i=0; i<NEWS_NUM; i++ )
    {
      GFL_G3D_OBJECT_LoopAnimeFrame( obj, news_anm_index[i], FX32_ONE );
    }
  }

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
    GFL_G3D_OBJ* obj = 
      GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[SETUP_INDEX_ELBOARD] );
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
