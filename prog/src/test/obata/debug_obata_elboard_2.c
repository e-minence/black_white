#ifdef PM_DEBUG

#include <wchar.h>    // wcslen
#include <gflib.h>
#include "debug_obata_elboard_setup.h"
#include "system/ica_anime.h"
#include "system/main.h"
#include "arc/arc_def.h"
#include "arc/debug_obata.naix"
#include "system/el_scoreboard.h"
#include "field/gimmick_obj_elboard.h"
#include "arc/arc_def.h"
#include "arc/message.naix"
#include "msg/msg_place_name.h"


//============================================================================================
// �����\�[�X
//============================================================================================

// ���\�[�X�C���f�b�N�X
typedef enum
{
  RES_ELBOARD_NSBMD,    // �f���̃��f��
  RES_ELBOARD_NSBTX,    // �f���̃e�N�X�`��
  RES_ELBOARD_NSBTA_1,  // �X�N���[���A�j���[�V����1
  RES_ELBOARD_NSBTA_2,  // �X�N���[���A�j���[�V����2
  RES_ELBOARD_NSBTA_3,  // �X�N���[���A�j���[�V����3
  RES_ELBOARD_NSBTA_4,  // �X�N���[���A�j���[�V����4
  RES_ELBOARD_NSBTA_5,  // �X�N���[���A�j���[�V����5
  RES_ELBOARD_NSBTA_6,  // �X�N���[���A�j���[�V����6
  RES_NUM
} RES_INDEX;
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
typedef enum
{
  ANM_ELBOARD_NEWS_1, // �j���[�X1
  ANM_ELBOARD_NEWS_2, // �j���[�X2
  ANM_ELBOARD_NEWS_3, // �j���[�X3
  ANM_ELBOARD_NEWS_4, // �j���[�X4
  ANM_ELBOARD_NEWS_5, // �j���[�X5
  ANM_ELBOARD_NEWS_6, // �j���[�X6
  ANM_NUM
} ANM_INDEX;
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
typedef enum
{
  OBJ_ELBOARD,  // �d���f����
  OBJ_NUM
} OBJ_INDEX;
static const GFL_G3D_UTIL_OBJ obj_table[] = 
{
  // ���f�����\�[�XID, 
  // ���f���f�[�^ID(���\�[�X����INDEX), 
  // �e�N�X�`�����\�[�XID,
  // �A�j���e�[�u��, 
  // �A�j�����\�[�X��
  { RES_ELBOARD_NSBMD, 0, RES_ELBOARD_NSBTX, anm_table, NELEMS(anm_table) },
}; 

// ���j�b�g�C���f�b�N�X
typedef enum
{
  UNIT_ELBOARD, // �d���f����
  UNIT_NUM
} UNIT_INDEX;
static const GFL_G3D_UTIL_SETUP unit[] =
{
  { res_table, RES_NUM, obj_table, OBJ_NUM },
};


//============================================================================================
// ���f���p�����[�^
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
static char* tex_name[NEWS_NUM] = 
{
  "gelboard_1",
  "gelboard_2",
  "gelboard_3",
  "gelboard_4",
  "gelboard_5",
  "gelboard_6",
};
// �p���b�g��
static char* plt_name[NEWS_NUM] = 
{
  "gelboard_1_pl",
  "gelboard_2_pl",
  "gelboard_3_pl",
  "gelboard_4_pl",
  "gelboard_5_pl",
  "gelboard_6_pl",
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
// �e�j���[�X�̈��p���A�[�J�C�uID
static const ARCID news_arc_id[NEWS_NUM] = 
{
  ARCID_MESSAGE,
  ARCID_MESSAGE,
  ARCID_MESSAGE,
  ARCID_MESSAGE,
  ARCID_MESSAGE,
  ARCID_MESSAGE,
};
// �e�j���[�X�̈��p���A�[�J�C�u���C���f�b�N�X
static const ARCDATID news_dat_id[NEWS_NUM] = 
{
  NARC_message_place_name_dat, 
  NARC_message_place_name_dat, 
  NARC_message_place_name_dat, 
  NARC_message_place_name_dat, 
  NARC_message_place_name_dat, 
  NARC_message_place_name_dat, 
};
// �e�j���[�X�̃��b�Z�[�WID
static const u32 news_msg_id[NEWS_NUM] = 
{
  MAPNAME_T1KANOKO,
  MAPNAME_T2KARAKUSA,
  MAPNAME_C1SANYOU,
  MAPNAME_C2SIPPOU,
  MAPNAME_C3YAGURUMA,
  MAPNAME_C4RAIMON,
};


static NEWS_PARAM news_param[NEWS_NUM] = 
{
  { ANM_ELBOARD_NEWS_1, "gelboard_1", "gelboard_1_pl", 
    ARCID_MESSAGE, NARC_message_place_name_dat, MAPNAME_T1KANOKO },
  { ANM_ELBOARD_NEWS_2, "gelboard_2", "gelboard_2_pl", 
    ARCID_MESSAGE, NARC_message_place_name_dat, MAPNAME_T2KARAKUSA },
  { ANM_ELBOARD_NEWS_3, "gelboard_3", "gelboard_3_pl", 
    ARCID_MESSAGE, NARC_message_place_name_dat, MAPNAME_C1SANYOU },
  { ANM_ELBOARD_NEWS_4, "gelboard_4", "gelboard_4_pl", 
    ARCID_MESSAGE, NARC_message_place_name_dat, MAPNAME_C2SIPPOU },
  { ANM_ELBOARD_NEWS_5, "gelboard_5", "gelboard_5_pl", 
    ARCID_MESSAGE, NARC_message_place_name_dat, MAPNAME_C3YAGURUMA },
  { ANM_ELBOARD_NEWS_6, "gelboard_6", "gelboard_6_pl", 
    ARCID_MESSAGE, NARC_message_place_name_dat, MAPNAME_C4RAIMON },
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
  u16 unitIndex[ UNIT_NUM ];

  // �d���f����
  GOBJ_ELBOARD* elboard;

  // �J����
  GFL_G3D_CAMERA* camera;
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
static GFL_PROC_RESULT DEBUG_OBATA_ELBOARD2_MainProcFunc_Init( 
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
static GFL_PROC_RESULT DEBUG_OBATA_ELBOARD2_MainProcFunc_Main( 
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
static GFL_PROC_RESULT DEBUG_OBATA_ELBOARD2_MainProcFunc_End( 
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
const GFL_PROC_DATA DebugObataElboard2MainProcData = 
{
	DEBUG_OBATA_ELBOARD2_MainProcFunc_Init,
	DEBUG_OBATA_ELBOARD2_MainProcFunc_Main,
	DEBUG_OBATA_ELBOARD2_MainProcFunc_End,
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
    for( i=0; i<UNIT_NUM; i++ )
    {
      OBATA_Printf( "%d\n", i );
      work->unitIndex[i] = GFL_G3D_UTIL_AddUnit( work->g3dUtil, &unit[i] );
    }
  }

  // �d���f�����쐬
  {
    int i;
    ELBOARD_PARAM param;
    param.heapID       = HEAPID_OBATA_DEBUG;
    param.maxNewsNum   = NEWS_NUM;
    param.dispSize     = 8;
    param.newsInterval = 5;
    param.g3dObj       = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[UNIT_ELBOARD] );
    work->elboard      = GOBJ_ELBOARD_Create( &param ); 
    for( i=0; i<NEWS_NUM; i++ )
    {
      GOBJ_ELBOARD_AddNews( work->elboard, &news_param[i] );
    }
  }

  // �J�����쐬
  {
    VecFx32    pos = { 0*FX32_ONE, 50*FX32_ONE, 200*FX32_ONE };
    VecFx32 target = { 0*FX32_ONE, 50*FX32_ONE, 0*FX32_ONE };
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
  int i;

  // �J�����j��
  GFL_G3D_CAMERA_Delete( work->camera );

  // �d���f����j��
  GOBJ_ELBOARD_Delete( work->elboard );

  // ���j�b�g�j��
  {
    int i;
    for( i=0; i<UNIT_NUM; i++ )
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

  // �t���[���J�E���^�X�V
  work->frame++; 

  // �d���f���̃A�j���[�V�����X�V
  GOBJ_ELBOARD_Main( work->elboard, FX32_ONE );

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
      GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[UNIT_ELBOARD] );
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

#endif
