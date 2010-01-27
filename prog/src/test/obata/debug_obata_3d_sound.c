#include <gflib.h>
#include "debug_obata_3d_sound_setup.h"
#include "system/main.h"
#include "arc/arc_def.h"
#include "arc/debug_obata.naix"
#include "gamesystem/iss_3ds_sys.h"
#include "sound/pm_sndsys.h"


// BGM�g���b�N�ԍ��}�X�N
#define TRACK_MASK_WIND (1<<(7-1))
#define TRACK_MASK_SHIP (1<<(8-1))
#define TRACK_MASK_CAR_1 (1<<(9-1))
#define TRACK_MASK_CAR_2 (1<<(10-1))


//============================================================================================
/**
 * @brief 3D�f�[�^
 */
//============================================================================================

// �g���[��
static const GFL_G3D_UTIL_RES res_table_tr[] = 
{
  { ARCID_OBATA_DEBUG, NARC_debug_obata_trailer_nsbmd, GFL_G3D_UTIL_RESARC },
};
static const GFL_G3D_UTIL_OBJ obj_table_tr[] = 
{
  {
    0,    // ���f�����\�[�XID
    0,    // ���f���f�[�^ID(���\�[�X����INDEX)
    0,    // �e�N�X�`�����\�[�XID
    NULL, // �A�j���e�[�u��(�����w��̂���)
    0,    // �A�j�����\�[�X��
  },
}; 

// �D
static const GFL_G3D_UTIL_RES res_table_ship[] = 
{
  { ARCID_OBATA_DEBUG, NARC_debug_obata_h01_ship_nsbmd, GFL_G3D_UTIL_RESARC },
};
static const GFL_G3D_UTIL_OBJ obj_table_ship[] = 
{
  {
    0,    // ���f�����\�[�XID
    0,    // ���f���f�[�^ID(���\�[�X����INDEX)
    0,    // �e�N�X�`�����\�[�XID
    NULL, // �A�j���e�[�u��(�����w��̂���)
    0,    // �A�j�����\�[�X��
  },
}; 

// ��
static const GFL_G3D_UTIL_RES res_table_house[] = 
{
  { ARCID_OBATA_DEBUG, NARC_debug_obata_t1_house_01_nsbmd, GFL_G3D_UTIL_RESARC },
};
static const GFL_G3D_UTIL_OBJ obj_table_house[] = 
{
  {
    0,    // ���f�����\�[�XID
    0,    // ���f���f�[�^ID(���\�[�X����INDEX)
    0,    // �e�N�X�`�����\�[�XID
    NULL, // �A�j���e�[�u��(�����w��̂���)
    0,    // �A�j�����\�[�X��
  },
}; 

// �Z�b�g�A�b�v�ԍ�
enum
{
  SETUP_INDEX_TRAILER,
  SETUP_INDEX_SHIP,
  SETUP_INDEX_HOUSE,
  SETUP_INDEX_MAX
};

// �Z�b�g�A�b�v�f�[�^
static const GFL_G3D_UTIL_SETUP setup[] =
{
  { res_table_tr, NELEMS(res_table_tr), obj_table_tr, NELEMS(obj_table_tr) },
  { res_table_ship, NELEMS(res_table_ship), obj_table_ship, NELEMS(obj_table_ship) },
  { res_table_house, NELEMS(res_table_house), obj_table_house, NELEMS(obj_table_house) },
};


//============================================================================================
/**
 * @brief �萔
 */
//============================================================================================
// �e�X�g���[�h
typedef enum
{
  TESTMODE_ALL,  // �S�\��
  TESTMODE_TR,   // �g���[���[�̂�
  TESTMODE_TR1,  // �g���[��1�̂�
  TESTMODE_TR2,  // �g���[��2�̂�
  TESTMODE_SHIP, // �D�̂�
  TESTMODE_NUM
} TESTMODE;

// �����I�u�W�F�N�g�ԍ�
typedef enum
{
  SOUNDOBJ_TR_1,
  SOUNDOBJ_TR_2,
  SOUNDOBJ_SHIP,
  SOUNDOBJ_NUM
} SOUNDOBJ_INDEX;

//============================================================================================
/**
 * @breif �I�u�W�F�N�g(�g���[���E�D�Ȃ�)
 */
//============================================================================================
typedef struct
{
  BOOL              active; // �\���t���O
  GFL_G3D_OBJ*         obj; // �`��I�u�W�F�N�g
  GFL_G3D_OBJSTATUS status; // �X�e�[�^�X
  //ISS_3DS_UNIT* iss3dsUnit; // 3D�T�E���h���j�b�g
  u16                frame; // ���݃t���[��
  u16             endFrame; // �ŏI�t���[��
  VecFx32         startPos; // �J�n�ʒu
  VecFx32           endPos; // �ŏI�ʒu 
}
SOUNDOBJ;


//============================================================================================
/**
 * @breif ���[�N
 */
//============================================================================================
typedef struct
{
  GFL_G3D_UTIL* g3dUtil;
  u16 unitIndex[ SETUP_INDEX_MAX ];

  // �J����
  GFL_G3D_CAMERA* camera;
  GFL_G3D_CAMERA* dammyCam;

  // 3D�T�E���h�V�X�e��
  ISS_3DS_SYS* iss3dsSys;

  // �g���[���[
  SOUNDOBJ soundObj[SOUNDOBJ_NUM];

  // �v���C���[�ʒu
  VecFx32 playerPos;

  // �J�����ʒu�I�t�Z�b�g
  VecFx32 camPosOffset;

  // �e�X�g���[�h
  TESTMODE mode;
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

static void InitSoundObj( PROC_WORK* work );
static void DrawSoundObj( SOUNDOBJ* sobj );
static void MoveSoundObj( SOUNDOBJ* sobj );
static void SetActiveOn( SOUNDOBJ* sobj );
static void SetActiveOff( SOUNDOBJ* sobj );

static void SetMode( PROC_WORK* work, TESTMODE next_mode );


//============================================================================================
/**
 * @brief �������֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_3D_SOUND_MainProcFunc_Init( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
#if 0
	PROC_WORK* work = NULL;

  // �q�[�v�쐬
  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_OBATA_DEBUG, 0x100000 );

	// ���[�N���쐬
	work = GFL_PROC_AllocWork( proc, sizeof( PROC_WORK ), HEAPID_OBATA_DEBUG );

  // ����������
  DEBUG_OBATA_3D_SOUND_Init( HEAPID_OBATA_DEBUG );
  Initialize( work );

#endif
	return GFL_PROC_RES_FINISH;
}


//============================================================================================
/**
 * @brief ���C���֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_3D_SOUND_MainProcFunc_Main( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
#if 0
	PROC_WORK* work = mywk;
  BOOL end = FALSE;

  switch( *seq )
  {
  case 0:
    PMSND_PlayBGM( SEQ_BGM_H_01 );
    ++( *seq );
  case 1:
    end = Main( work );
    ISS_3DS_SYS_Main( work->iss3dsSys );
    PMSND_ChangeBGMVolume( TRACK_MASK_WIND, 0 );  // ��OFF
    Draw( work );
    if( end )
    {
      ++( *seq );
    }
    break;
  case 2:
    return GFL_PROC_RES_FINISH;
  }

#endif
  return GFL_PROC_RES_CONTINUE;
}


//============================================================================================
/**
 * @brief �I���֐�
 */
//============================================================================================
static GFL_PROC_RESULT DEBUG_OBATA_3D_SOUND_MainProcFunc_End( GFL_PROC* proc, int* seq, void* pwk, void* mywk )
{
#if 0
	PROC_WORK* work = mywk;

  // �I������
  Finalize( work );

	// ���[�N��j��
	GFL_PROC_FreeWork( proc );

  DEBUG_OBATA_3D_SOUND_Exit();

  // �q�[�v��j��
  GFL_HEAP_DeleteHeap( HEAPID_OBATA_DEBUG );

#endif
	return GFL_PROC_RES_FINISH;
} 


//============================================================================================
/**
 * @brief �v���Z�X��`�f�[�^
 */
//============================================================================================
const GFL_PROC_DATA DebugObata3DSoundMainProcData = 
{
	DEBUG_OBATA_3D_SOUND_MainProcFunc_Init,
	DEBUG_OBATA_3D_SOUND_MainProcFunc_Main,
	DEBUG_OBATA_3D_SOUND_MainProcFunc_End,
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
#if 0
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

  // �J�����쐬
  {
    VecFx32    pos = { 0, 1000*FX32_ONE, 400*FX32_ONE };
    VecFx32 target = { 0, 0, 0 };
    work->camera   = GFL_G3D_CAMERA_CreateDefault( &pos, &target, HEAPID_OBATA_DEBUG );
  }
  // �J�����ݒ�
  {
    fx32 far = FX32_ONE * 4096;
    GFL_G3D_CAMERA_SetFar( work->camera, &far );
  }
  { // �_�~�[�J����
    VecFx32    pos = { 0, 0, 0 };
    VecFx32 target = { 0, 0, -FX32_ONE };
    VecFx32     up = { 0, FX32_ONE, 0 };
    work->dammyCam = GFL_G3D_CAMERA_CreateDefault( &pos, &target, HEAPID_OBATA_DEBUG );
    GFL_G3D_CAMERA_SetCamUp( work->dammyCam, &up );
  }

  // 3D�T�E���h�V�X�e���쐬
  work->iss3dsSys = ISS_3DS_SYS_Create( HEAPID_OBATA_DEBUG, 10 );
  ISS_3DS_SYS_SetObserver( work->iss3dsSys, work->camera );

  // �g���[���쐬
  InitSoundObj( work );

  // ���̑��̏�����
  SetMode( work, TESTMODE_ALL );
  VEC_Set( &work->playerPos, 0, 0, 0 );
  VEC_Set( &work->camPosOffset, FX32_ONE, 100*FX32_ONE, 300*FX32_ONE );
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * @breif �I��
 */
//-------------------------------------------------------------------------------------------- 
static void Finalize( PROC_WORK* work )
{ 
#if 0
  // 3D�T�E���h�V�X�e���j��
  ISS_3DS_SYS_ClearObserver( work->iss3dsSys );
  ISS_3DS_SYS_Delete( work->iss3dsSys );

  // �J�����j��
  GFL_G3D_CAMERA_Delete( work->camera );
  GFL_G3D_CAMERA_Delete( work->dammyCam );

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
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * @breif ���C������ 
 */
//-------------------------------------------------------------------------------------------- 
static BOOL Main( PROC_WORK* work )
{
#if 0
  int i;
  VecFx32 vel, pos;
  int trg = GFL_UI_KEY_GetTrg();
  int key = GFL_UI_KEY_GetCont();

  // �J�����I�t�Z�b�g�ړ�
  VEC_Set( &vel, 0, 0, 0 );
  if( key & PAD_BUTTON_X )  work->camPosOffset.y += FX32_ONE * 10;
  if( key & PAD_BUTTON_B )  work->camPosOffset.y -= FX32_ONE * 10;

  // ��l���ړ�
  VEC_Set( &vel, 0, 0, 0 );
  if( key & PAD_KEY_UP )    vel.z -= FX32_ONE * 10;
  if( key & PAD_KEY_DOWN )  vel.z += FX32_ONE * 10;
  if( key & PAD_KEY_LEFT )  vel.x -= FX32_ONE * 10;
  if( key & PAD_KEY_RIGHT ) vel.x += FX32_ONE * 10;
  VEC_Add( &work->playerPos, &vel, &work->playerPos );
  VEC_Add( &work->playerPos, &work->camPosOffset, &pos );
  GFL_G3D_CAMERA_SetPos( work->camera, &pos );
  GFL_G3D_CAMERA_SetTarget( work->camera, &work->playerPos );


  // �I�u�W�F�N�g�𓮂���
  for( i=0; i<SOUNDOBJ_NUM; i++ )
  { 
    MoveSoundObj( &work->soundObj[i] );
  }

  // �X�^�[�g�Ń��[�h�ؑ�
  if( trg & PAD_BUTTON_START ) SetMode( work, work->mode + 1 );

  // �Z���N�g�ŏI��
  if( trg & PAD_BUTTON_SELECT ) return TRUE;

#endif
  return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @breif �`��
 */
//-------------------------------------------------------------------------------------------- 
static void Draw( PROC_WORK* work )
{
#if 0
  int i;

  // �J�����X�V
  GFL_G3D_CAMERA_Switching( work->camera );

  // �`��
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();

  // �����I�u�W�F�`��
  for( i=0; i<SOUNDOBJ_NUM; i++ ) DrawSoundObj( &work->soundObj[i] );

  // �v���C���[�`��
  {
    GFL_G3D_OBJ* obj;
    GFL_G3D_OBJSTATUS status;
    VEC_Set( &status.trans, work->playerPos.x, work->playerPos.y, work->playerPos.z );
    VEC_Set( &status.scale, FX32_ONE>>1, FX32_ONE>>1, FX32_ONE>>1 );
    MTX_Identity33( &status.rotate );
    obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[SETUP_INDEX_HOUSE] );
    GFL_G3D_DRAW_DrawObject( obj, &status );
  }
  GFL_G3D_DRAW_End(); 
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F������������
 */
//--------------------------------------------------------------------------------------------
static void InitSoundObj( PROC_WORK* work )
{
#if 0
  int index;
  SOUNDOBJ* sobj;

  // �g���[���[1
  sobj = &work->soundObj[SOUNDOBJ_TR_1];
  sobj->obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[SETUP_INDEX_TRAILER] );
  VEC_Set( &sobj->status.trans, 0, 0, 0 );
  VEC_Set( &sobj->status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  GFL_CALC3D_MTX_CreateRot( 0, 180.0f/360.0f*0xffff, 0, &sobj->status.rotate );
  index = ISS_3DS_SYS_AddUnit( work->iss3dsSys );
  sobj->iss3dsUnit = ISS_3DS_SYS_GetUnit( work->iss3dsSys, index );
  ISS_3DS_UNIT_SetPos( sobj->iss3dsUnit, &sobj->status.trans );
  ISS_3DS_UNIT_SetRange( sobj->iss3dsUnit, 500*FX32_ONE );
  ISS_3DS_UNIT_SetMaxVolume( sobj->iss3dsUnit, 127 );
  ISS_3DS_UNIT_SetTrackBit( sobj->iss3dsUnit, TRACK_MASK_CAR_1 );
  sobj->frame = 0;
  sobj->endFrame = 500;
  VEC_Set( &sobj->startPos, 50*FX32_ONE, 0*FX32_ONE, -1000*FX32_ONE );
  VEC_Set( &sobj->endPos,   50*FX32_ONE, 0*FX32_ONE,  1000*FX32_ONE );

  // �g���[���[2
  sobj = &work->soundObj[SOUNDOBJ_TR_2];
  sobj->obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[SETUP_INDEX_TRAILER] );
  VEC_Set( &sobj->status.trans, 0, 0, 0 );
  VEC_Set( &sobj->status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &sobj->status.rotate );
  index = ISS_3DS_SYS_AddUnit( work->iss3dsSys );
  sobj->iss3dsUnit = ISS_3DS_SYS_GetUnit( work->iss3dsSys, index );
  ISS_3DS_UNIT_SetPos( sobj->iss3dsUnit, &sobj->status.trans );
  ISS_3DS_UNIT_SetRange( sobj->iss3dsUnit, 500*FX32_ONE );
  ISS_3DS_UNIT_SetMaxVolume( sobj->iss3dsUnit, 127 );
  ISS_3DS_UNIT_SetTrackBit( sobj->iss3dsUnit, TRACK_MASK_CAR_2 );
  sobj->frame = 0;
  sobj->endFrame = 500;
  VEC_Set( &sobj->startPos, -50*FX32_ONE, 0*FX32_ONE,  1000*FX32_ONE );
  VEC_Set( &sobj->endPos,   -50*FX32_ONE, 0*FX32_ONE, -1000*FX32_ONE );

  // �D
  sobj = &work->soundObj[SOUNDOBJ_SHIP];
  sobj->obj = GFL_G3D_UTIL_GetObjHandle( work->g3dUtil, work->unitIndex[SETUP_INDEX_SHIP] );
  VEC_Set( &sobj->status.trans, 0, 0, 0 );
  VEC_Set( &sobj->status.scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &sobj->status.rotate );
  index = ISS_3DS_SYS_AddUnit( work->iss3dsSys );
  sobj->iss3dsUnit = ISS_3DS_SYS_GetUnit( work->iss3dsSys, index );
  ISS_3DS_UNIT_SetPos( sobj->iss3dsUnit, &sobj->status.trans );
  ISS_3DS_UNIT_SetRange( sobj->iss3dsUnit, 3000*FX32_ONE );
  ISS_3DS_UNIT_SetMaxVolume( sobj->iss3dsUnit, 127 );
  ISS_3DS_UNIT_SetTrackBit( sobj->iss3dsUnit, TRACK_MASK_SHIP );
  sobj->frame = 0;
  sobj->endFrame = 1000;
  VEC_Set( &sobj->startPos, -800*FX32_ONE, 0*FX32_ONE, 0*FX32_ONE );
  VEC_Set( &sobj->endPos,    800*FX32_ONE, 0*FX32_ONE, 0*FX32_ONE );
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F��`�悷��
 */
//--------------------------------------------------------------------------------------------
static void DrawSoundObj( SOUNDOBJ* sobj )
{
#if 0
  // �L���ȃI�u�W�F�݂̂�`��
  if( sobj->active )
  {
    GFL_G3D_DRAW_DrawObject( sobj->obj, &sobj->status );
  }
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�𓮂���
 */
//--------------------------------------------------------------------------------------------
static void MoveSoundObj( SOUNDOBJ* sobj )
{
#if 0
  // �L���ȃI�u�W�F�݂̂𓮂���
  if( sobj->active )
  { 
    fx32 w0, w1;
    VecFx32 v0, v1;

    // �t���[���i�߂�
    sobj->frame = (sobj->frame + 1) % sobj->endFrame;

    // �ʒu�x�N�g������`���
    w1 = FX_Div( sobj->frame, sobj->endFrame );
    w0 = FX32_ONE - w1;
    GFL_CALC3D_VEC_MulScalar( &sobj->startPos, w0, &v0 );
    GFL_CALC3D_VEC_MulScalar( &sobj->endPos,   w1, &v1 );
    VEC_Add( &v0, &v1, &sobj->status.trans );

    // �����ʒu�����킹��
    ISS_3DS_UNIT_SetPos( sobj->iss3dsUnit, &sobj->status.trans );
  }
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F��L��������
 */
//--------------------------------------------------------------------------------------------
static void SetActiveOn( SOUNDOBJ* sobj )
{ 
#if 0
  sobj->active = TRUE;
  ISS_3DS_UNIT_SetMaxVolume( sobj->iss3dsUnit, 127 );
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�𖳌�������
 */
//--------------------------------------------------------------------------------------------
static void SetActiveOff( SOUNDOBJ* sobj )
{
#if 0
  sobj->active = FALSE;
  ISS_3DS_UNIT_SetMaxVolume( sobj->iss3dsUnit, 0 );
#endif
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���[�h��ύX����
 */
//--------------------------------------------------------------------------------------------
static void SetMode( PROC_WORK* work, TESTMODE next_mode )
{ 
#if 0
  // ���[�h�ύX
  work->mode = next_mode % TESTMODE_NUM;

  // ���[�h�ݒ�
  switch( work->mode )
  {
  case TESTMODE_ALL:
    SetActiveOn( &work->soundObj[SOUNDOBJ_TR_1] );
    SetActiveOn( &work->soundObj[SOUNDOBJ_TR_2] );
    SetActiveOn( &work->soundObj[SOUNDOBJ_SHIP] );
    break;
  case TESTMODE_TR:
    SetActiveOn( &work->soundObj[SOUNDOBJ_TR_1] );
    SetActiveOn( &work->soundObj[SOUNDOBJ_TR_2] );
    SetActiveOff( &work->soundObj[SOUNDOBJ_SHIP] );
    break;
  case TESTMODE_TR1:
    SetActiveOn( &work->soundObj[SOUNDOBJ_TR_1] );
    SetActiveOff( &work->soundObj[SOUNDOBJ_TR_2] );
    SetActiveOff( &work->soundObj[SOUNDOBJ_SHIP] );
    break;
  case TESTMODE_TR2:
    SetActiveOff( &work->soundObj[SOUNDOBJ_TR_1] );
    SetActiveOn( &work->soundObj[SOUNDOBJ_TR_2] );
    SetActiveOff( &work->soundObj[SOUNDOBJ_SHIP] );
    break;
  case TESTMODE_SHIP:
    SetActiveOff( &work->soundObj[SOUNDOBJ_TR_1] );
    SetActiveOff( &work->soundObj[SOUNDOBJ_TR_2] );
    SetActiveOn( &work->soundObj[SOUNDOBJ_SHIP] );
    break;
  }
#endif
}
