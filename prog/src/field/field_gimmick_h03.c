/////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �M�~�b�N�o�^�֐� ( H03 )
 * @file   field_gimmick_h01.c
 * @author obata
 * @date   2009.10.21
 */
/////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "gmk_tmp_wk.h"
#include "fieldmap.h"
#include "field_gimmick_h03.h" 
#include "gamesystem/iss_3ds_sys.h"
#include "sound_obj.h" 
#include "savedata/gimmickwork.h"
#include "field_gimmick_def.h"
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"
#include "arc/h03.naix"


//==========================================================================================
// ���萔
//==========================================================================================

#define ARCID (ARCID_H03_GIMMICK) // �M�~�b�N�f�[�^�̃A�[�J�C�uID
#define ANIME_BUF_INTVL  (10) // �A�j���[�V�����f�[�^�̓ǂݍ��݊Ԋu[frame]
#define EXPOBJ_UNIT_IDX  (0)  // �t�B�[���h�g���I�u�W�F�N�g�̃��j�b�g�o�^�C���f�b�N�X
#define ISS_3DS_UNIT_NUM (10) // 3D���j�b�g��
#define GIMMICK_WORK_ASSIGN_ID (0) // �M�~�b�N���[�N�̃A�T�C��ID

// �����I�u�W�F�N�g�̃C���f�b�N�X
typedef enum {
  SOBJ_TRAIN_1, // �d��1
  SOBJ_TRAIN_2, // �d��2
  SOBJ_NUM 
} SOBJ_INDEX;


//==========================================================================================
// ��3D���\�[�X
//==========================================================================================
// ���\�[�X
typedef enum {
  RES_TRAIN_NSBMD,  // �d�Ԃ̃��f��
  RES_NUM
} RES_INDEX;
static const GFL_G3D_UTIL_RES res_table[RES_NUM] = 
{
  { ARCID, NARC_h03_h03_train1_nsbmd, GFL_G3D_UTIL_RESARC },  // �d�Ԃ̃��f��
};

// �I�u�W�F�N�g
typedef enum {
  OBJ_TRAIN_1,  // �d��1
  OBJ_TRAIN_2,  // �d��2
  OBJ_NUM
} OBJ_INDEX;
static const GFL_G3D_UTIL_OBJ obj_table[OBJ_NUM] = 
{
  { RES_TRAIN_NSBMD, 0, 0, NULL, 0 }, // �d��1
  { RES_TRAIN_NSBMD, 0, 0, NULL, 0 }, // �d��2
};

// �Z�b�g�A�b�v���
static GFL_G3D_UTIL_SETUP setup = { res_table, RES_NUM, obj_table, OBJ_NUM };


//==========================================================================================
// ���M�~�b�N���[�N
//==========================================================================================
typedef struct
{ 
  HEAPID            heapID; // �g�p����q�[�vID
  ISS_3DS_SYS*   iss3dsSys; // 3D�T�E���h�V�X�e��
  SOUNDOBJ* sobj[SOBJ_NUM]; // �����I�u�W�F�N�g
  int       wait[SOBJ_NUM]; // �����I�u�W�F�N�g�̓���ҋ@�J�E���^
  int    minWait[SOBJ_NUM]; // �ŒZ�҂�����
  int    maxWait[SOBJ_NUM]; // �Œ��҂�����
} H03WORK;


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static void InitWork( H03WORK* work, FIELDMAP_WORK* fieldmap );
static void LoadWaitTime( H03WORK* work );
static void SaveGimmick( H03WORK* work, FIELDMAP_WORK* fieldmap );
static void LoadGimmick( H03WORK* work, FIELDMAP_WORK* fieldmap );
static void SetStandBy( H03WORK* work, SOBJ_INDEX index );
static void MoveStart( H03WORK* work, SOBJ_INDEX index );


//==========================================================================================
// ���M�~�b�N�o�^�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void H03_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
  u32* gmk_save;  // �M�~�b�N�̎��Z�[�u�f�[�^
  H03WORK* work;  // H03�M�~�b�N�Ǘ����[�N
  HEAPID                heap_id = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &setup, EXPOBJ_UNIT_IDX );

  // �M�~�b�N�Ǘ����[�N���쐬
  work = (H03WORK*)GMK_TMP_WK_AllocWork( fieldmap, GIMMICK_WORK_ASSIGN_ID, heap_id, sizeof(H03WORK) );

  // �M�~�b�N�Ǘ����[�N�������� 
  InitWork( work, fieldmap );

  // ���[�h
  LoadGimmick( work, fieldmap );

  // ��ISS�̃}�X�^�[�{�����[���𕜋A
  ISS_3DS_SYS_SetMasterVolume( work->iss3dsSys, 127 );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �I���֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void H03_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  int i;
  H03WORK* work = (H03WORK*)GMK_TMP_WK_GetWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );

  // ��ISS�̃}�X�^�[�{�����[�����i��
  ISS_3DS_SYS_SetMasterVolume( work->iss3dsSys, 0 );

  // �Z�[�u
  SaveGimmick( work, fieldmap );

  // �����I�u�W�F�N�g��j��
  for( i=0; i<SOBJ_NUM; i++ )
  {
    SOUNDOBJ_Delete( work->sobj[i] );
  }

  // �M�~�b�N�Ǘ����[�N��j��
  GMK_TMP_WK_FreeWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );
}

//------------------------------------------------------------------------------------------
/**
 * @brief ����֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void H03_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  int i;
  H03WORK* work = (H03WORK*)GMK_TMP_WK_GetWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );

  // �ϑ��҂̈ʒu��ݒ�
  {
    FIELD_CAMERA* fieldCamera;
    const GFL_G3D_CAMERA* g3dCamera;
    VecFx32 cameraPos, targetPos;

    fieldCamera = FIELDMAP_GetFieldCamera( fieldmap );
    g3dCamera   = FIELD_CAMERA_GetCameraPtr( fieldCamera );
    GFL_G3D_CAMERA_GetPos( g3dCamera, &cameraPos );
    GFL_G3D_CAMERA_GetTarget( g3dCamera, &targetPos );
    ISS_3DS_SYS_SetObserverPos( work->iss3dsSys, &cameraPos, &targetPos );
  }

  // ���ׂẲ����I�u�W�F�N�g�𓮂���
  for( i=0; i<SOBJ_NUM; i++ )
  { 
    // �ҋ@��� ==> ���ԃJ�E���g�_�E��
    if( 0 < work->wait[i] )
    {
      // �J�E���g�_�E���I�� ==> ����
      if( --work->wait[i] <= 0 )
      {
        MoveStart( work, i );
      }
    }
    // ���쒆 ==> �A�j���[�V�������X�V
    else
    {
      // �A�j���[�V�����Đ����I�� ==> �ҋ@��Ԃ�
      if( SOUNDOBJ_IncAnimeFrame( work->sobj[i], FX32_ONE ) )
      {
        SetStandBy( work, i );
      }
    }
  }
}


//==========================================================================================
// �� ����J�֐�
//==========================================================================================

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�Ǘ����[�N������������
 *
 * @param work     �������Ώۃ��[�N
 * @param fieldmap �ˑ�����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
static void InitWork( H03WORK* work, FIELDMAP_WORK* fieldmap )
{
  int i;
  HEAPID                heapID = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exObjCnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // �q�[�vID���L��
  work->heapID = heapID;

  // 3D�T�E���h�V�X�e�����擾
  {
    GAMESYS_WORK* gameSystem = FIELDMAP_GetGameSysWork( fieldmap );
    ISS_SYS*      issSystem  = GAMESYSTEM_GetIssSystem( gameSystem );
    work->iss3dsSys = ISS_SYS_GetIss3DSSystem( issSystem );
  }

  // �����I�u�W�F�N�g���쐬
  {
    // �d��1
    { 
      u32* data;
      GFL_G3D_OBJSTATUS* status;
      SOUNDOBJ* sobj;

      // �ݒ�f�[�^�擾
      data = GFL_ARC_LoadDataAlloc( ARCID, NARC_h03_train1_3ds_unit_data_bin, heapID );
      OBATA_Printf( "train1 data[0] = %d\n", data[0] );
      OBATA_Printf( "train1 data[1] = %d\n", data[1] );

      // �쐬
      status = FLD_EXP_OBJ_GetUnitObjStatus( exObjCnt, 0, OBJ_TRAIN_1 );
      sobj = SOUNDOBJ_Create( fieldmap, status, ISS3DS_UNIT_TRACK09, data[0] << FX32_SHIFT, data[1] );
      SOUNDOBJ_SetAnime( sobj, ARCID, NARC_h03_train1_ica_data_bin, ANIME_BUF_INTVL );
      work->sobj[ SOBJ_TRAIN_1 ] = sobj;

      GFL_HEAP_FreeMemory( data );
    }
    // �d��2
    {
      u32* data;
      GFL_G3D_OBJSTATUS* status;
      SOUNDOBJ* sobj;

      // �ݒ�f�[�^�擾
      data = GFL_ARC_LoadDataAlloc( ARCID, NARC_h03_train2_3ds_unit_data_bin, heapID );
      OBATA_Printf( "train2 data[0] = %d\n", data[0] );
      OBATA_Printf( "train2 data[1] = %d\n", data[1] );

      // �쐬
      status = FLD_EXP_OBJ_GetUnitObjStatus( exObjCnt, 0, OBJ_TRAIN_2 );
      sobj = SOUNDOBJ_Create( fieldmap, status, ISS3DS_UNIT_TRACK10, data[0] << FX32_SHIFT, data[1] );
      SOUNDOBJ_SetAnime( sobj, ARCID, NARC_h03_train2_ica_data_bin, ANIME_BUF_INTVL );
      work->sobj[ SOBJ_TRAIN_2 ] = sobj;

      GFL_HEAP_FreeMemory( data );
    } 
  }

  // ����ҋ@�J�E���^
  for( i=0; i<SOBJ_NUM; i++ ){ work->wait[i] = 0; }

  // �ҋ@����
  LoadWaitTime( work );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �e�����I�u�W�F�N�g�̑҂����Ԃ�ǂݍ���
 *
 * @param work �ݒ�Ώۃ��[�N
 */
//------------------------------------------------------------------------------------------
static void LoadWaitTime( H03WORK* work )
{
  int i;
  ARCDATID dat_id[SOBJ_NUM] = 
  {
    NARC_h03_train1_wait_data_bin,
    NARC_h03_train2_wait_data_bin,
  };

  for( i=0; i<SOBJ_NUM; i++ )
  {
    u32* data = GFL_ARC_LoadDataAlloc( ARCID, dat_id[i], work->heapID );
    work->minWait[i] = data[0];
    work->maxWait[i] = data[1];
    GFL_HEAP_FreeMemory( data );
    // DEBUG:
    //OBATA_Printf( "minWait[%d] = %d\n", i, work->minWait[i] );
    //OBATA_Printf( "maxWait[%d] = %d\n", i, work->maxWait[i] );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�̏�Ԃ�ۑ�����
 *
 * @param work     �ۑ��ΏۃM�~�b�N�̊Ǘ����[�N
 * @param fieldmap �ˑ�����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
static void SaveGimmick( H03WORK* work, FIELDMAP_WORK* fieldmap )
{
  int i;
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  u32*         gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H03 );

  // �Z�[�u(�e�����I�u�W�F�N�g�̃A�j���[�V�����t���[����)
  for( i=0; i<SOBJ_NUM; i++ )
  {
    fx32 frame  = SOUNDOBJ_GetAnimeFrame( work->sobj[i] );
    gmk_save[i] = (frame >> FX32_SHIFT);
    OBATA_Printf( "SaveGimmick: gmk_save[%d] = %d\n", i, gmk_save[i] );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�̏�Ԃ𕜋A����
 *
 * @param work     ���A�ΏۃM�~�b�N�̊Ǘ����[�N
 * @param fieldmap �ˑ�����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
static void LoadGimmick( H03WORK* work, FIELDMAP_WORK* fieldmap )
{
  int i;
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  u32*         gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H03 );

  // �Z�[�u����
  {
    // �e�����I�u�W�F�N�g�̃A�j���[�V�����t���[�����𕜋A
    for( i=0; i<SOBJ_NUM; i++ )
    {
      fx32 frame = gmk_save[i] << FX32_SHIFT;
      SOUNDOBJ_SetAnimeFrame( work->sobj[i], frame );
      OBATA_Printf( "LoadGimmick: gmk_save[%d] = %d\n", i, gmk_save[i] );
      OBATA_Printf( "LoadGimmick: frame = %x\n", i, frame );
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g��ҋ@��Ԃɂ���
 *
 * @param work  ����Ώۃ��[�N
 * @param index �ҋ@��Ԃɂ���I�u�W�F�N�g���w��
 */
//------------------------------------------------------------------------------------------
static void SetStandBy( H03WORK* work, SOBJ_INDEX index )
{
  u32 range = work->maxWait[index] - work->minWait[index] + 1;      // �� = �Œ� - �ŒZ
  u32 time  = work->minWait[index] + GFUser_GetPublicRand0(range);  // �҂����� = �ŒZ + ��
  work->wait[index] = time;
}

//------------------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�N�g�𓮍��Ԃɂ���
 *
 * @param work  ����Ώۃ��[�N
 * @param index �ҋ@��Ԃɂ���I�u�W�F�N�g���w��
 */
//------------------------------------------------------------------------------------------
static void MoveStart( H03WORK* work, SOBJ_INDEX index )
{ 
  SOUNDOBJ_SetAnimeFrame( work->sobj[index], 0 );
}
