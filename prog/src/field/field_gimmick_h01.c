#include <gflib.h>
#include "fieldmap.h" 
#include "field_gimmick_h01.h"
#include "field_gimmick_def.h"
#include "gamesystem/iss_3ds_sys.h"
#include "savedata/gimmickwork.h"
#include "sound/pm_sndsys.h"
#include "system/gfl_use.h"
#include "sound_obj.h" 

#include "arc/arc_def.h"
#include "arc/h01.naix"


//==========================================================================================
// ���萔
//==========================================================================================

#define ARCID (ARCID_H01_GIMMICK) // �M�~�b�N�f�[�^�̃A�[�J�C�uID
#define ANIME_BUF_INTVL  (10) // �A�j���[�V�����f�[�^�̓ǂݍ��݊Ԋu[frame]
#define EXPOBJ_UNIT_IDX  (0)  // �t�B�[���h�g���I�u�W�F�N�g�̃��j�b�g�o�^�C���f�b�N�X
#define ISS_3DS_UNIT_NUM (10) // 3D���j�b�g��
#define TAIL_INTERVAL    (1)  // �O���������˂��Ă���㕔�������˂���܂ł̊Ԋu

// �����I�u�W�F�N�g�̃C���f�b�N�X
typedef enum {
  SOBJ_TRAILER_1_HEAD, // �g���[���[1(�O)
  SOBJ_TRAILER_1_TAIL, // �g���[���[1(��)
  SOBJ_TRAILER_2_HEAD, // �g���[���[2(�O)
  SOBJ_TRAILER_2_TAIL, // �g���[���[2(��)
  SOBJ_SHIP,           // �D
  SOBJ_NUM 
} SOBJ_INDEX;


//==========================================================================================
// ��3D���\�[�X
//==========================================================================================
// ���\�[�X
typedef enum {
  RES_TRAILER_HEAD_NSBMD,  // �g���[���[(�O)�̃��f��
  RES_TRAILER_TAIL_NSBMD,  // �g���[���[(��)�̃��f��
  RES_SHIP_NSBMD,     // �D�̃��f��
  RES_NUM
} RES_INDEX;
static const GFL_G3D_UTIL_RES res_table[RES_NUM] = 
{
  { ARCID, NARC_h01_trailer_01a_nsbmd, GFL_G3D_UTIL_RESARC },  // �g���[���[(�O)�̃��f��
  { ARCID, NARC_h01_trailer_01b_nsbmd, GFL_G3D_UTIL_RESARC },  // �g���[���[(��)�̃��f��
  { ARCID, NARC_h01_h01_ship_nsbmd,    GFL_G3D_UTIL_RESARC },  // �D�̃��f��
};

// �I�u�W�F�N�g
typedef enum {
  OBJ_TRAILER_1_HEAD,  // �g���[���[1(�O)
  OBJ_TRAILER_1_TAIL,  // �g���[���[1(��)
  OBJ_TRAILER_2_HEAD,  // �g���[���[2(�O)
  OBJ_TRAILER_2_TAIL,  // �g���[���[2(��)
  OBJ_SHIP,       // �D
  OBJ_NUM
} OBJ_INDEX;
static const GFL_G3D_UTIL_OBJ obj_table[OBJ_NUM] = 
{
  { RES_TRAILER_HEAD_NSBMD, 0, RES_TRAILER_HEAD_NSBMD, NULL, 0 },  // �g���[���[1(�O)
  { RES_TRAILER_TAIL_NSBMD, 0, RES_TRAILER_TAIL_NSBMD, NULL, 0 },  // �g���[���[1(��)
  { RES_TRAILER_HEAD_NSBMD, 0, RES_TRAILER_HEAD_NSBMD, NULL, 0 },  // �g���[���[2(�O)
  { RES_TRAILER_TAIL_NSBMD, 0, RES_TRAILER_TAIL_NSBMD, NULL, 0 },  // �g���[���[2(��)
  { RES_SHIP_NSBMD,         0, RES_SHIP_NSBMD,         NULL, 0 },  // �D
};

// �Z�b�g�A�b�v���
static GFL_G3D_UTIL_SETUP setup = { res_table, RES_NUM, obj_table, OBJ_NUM };


//==========================================================================================
// ����
//==========================================================================================
typedef struct
{
  u16 trackBit;  // ����Ώۃg���b�N�r�b�g
  u16 padding;
  float minHeight;  // ������o������
  float maxHeight;  // �����ő�ɂȂ鍂��

} WIND_DATA;


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
  WIND_DATA      wind_data; // ���f�[�^
} H01WORK;


//==========================================================================================
// ������J�֐��̃v���g�^�C�v�錾
//==========================================================================================
static void InitWork( H01WORK* work, FIELDMAP_WORK* fieldmap );
static void LoadWaitTime( H01WORK* work );
static void LoadWindData( H01WORK* work );
static void SaveGimmick( H01WORK* work, FIELDMAP_WORK* fieldmap );
static void LoadGimmick( H01WORK* work, FIELDMAP_WORK* fieldmap );
static void SetStandBy( H01WORK* work, SOBJ_INDEX index );
static void MoveStart( H01WORK* work, SOBJ_INDEX index );
static void UpdateWindVolume( FIELDMAP_WORK* fieldmap, H01WORK* work );


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
void H01_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
  u32* gmk_save;  // �M�~�b�N�̎��Z�[�u�f�[�^
  H01WORK* work;  // H01�M�~�b�N�Ǘ����[�N
  HEAPID                heapID = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  GAMESYS_WORK*            gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*               gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*          gmkwork = GAMEDATA_GetGimmickWork(gdata);

  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &setup, EXPOBJ_UNIT_IDX );

  // �M�~�b�N�Ǘ����[�N���쐬
  work = (H01WORK*)GFL_HEAP_AllocMemory( heapID, sizeof(H01WORK) );

  // �M�~�b�N�Ǘ����[�N�������� 
  InitWork( work, fieldmap );

  // ���[�h
  LoadGimmick( work, fieldmap );

  // �M�~�b�N�Ǘ����[�N�̃A�h���X��ۑ�
  gmk_save    = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H01 );
  gmk_save[0] = (int)work;

  // DEBUG:
  OBATA_Printf( "GIMMICK-H01: set up\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �I���֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void H01_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  int i;
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  u32*         gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H01 );
  H01WORK*         work = (H01WORK*)gmk_save[0]; // gmk_save[0]�̓M�~�b�N�Ǘ����[�N�̃A�h���X

  // �����~�߂�
  PMSND_ChangeBGMVolume( work->wind_data.trackBit, 0 );

  // �Z�[�u
  SaveGimmick( work, fieldmap );

  // �����I�u�W�F�N�g��j��
  for( i=0; i<SOBJ_NUM; i++ )
  {
    SOUNDOBJ_Delete( work->sobj[i] );
  }

  // �M�~�b�N�Ǘ����[�N��j��
  GFL_HEAP_FreeMemory( work );

  // DEBUG:
  OBATA_Printf( "GIMMICK-H01: end\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief ����֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void H01_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  int i;
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  u32*         gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H01 );
  H01WORK*         work = (H01WORK*)gmk_save[0]; // gmk_save[0]�̓M�~�b�N�Ǘ����[�N�̃A�h���X

  // �ϑ��҂̈ʒu��ݒ�
  {
    FIELD_CAMERA* fieldCamera;
    VecFx32 cameraPos, targetPos;

    fieldCamera = FIELDMAP_GetFieldCamera( fieldmap );
    FIELD_CAMERA_GetCameraPos( fieldCamera, &cameraPos );
    FIELD_CAMERA_GetTargetPos( fieldCamera, &targetPos );
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
        // �㕔����O�����ɒǏ]������
        switch(i)
        {
        case SOBJ_TRAILER_1_HEAD:
          work->wait[SOBJ_TRAILER_1_TAIL] = TAIL_INTERVAL;
          break;
        case SOBJ_TRAILER_2_HEAD:
          work->wait[SOBJ_TRAILER_2_TAIL] = TAIL_INTERVAL;
          break;
        }
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

  // �����X�V
  UpdateWindVolume( fieldmap, work );
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
static void InitWork( H01WORK* work, FIELDMAP_WORK* fieldmap )
{
  int i;
  HEAPID                heapID = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // �q�[�vID���L��
  work->heapID = heapID;

  // 3D�T�E���h�V�X�e�����擾
  {
    GAMESYS_WORK* gameSystem = FIELDMAP_GetGameSysWork( fieldmap );
    ISS_SYS*      issSystem  = GAMESYSTEM_GetIssSystem( gameSystem );
    work->iss3dsSys = ISS_SYS_GetIss3DSSystem( issSystem );
  }

  // �����I�u�W�F�N�g���쐬
  { // �g���[���[1(�O)
    u32* data;
    GFL_G3D_OBJSTATUS* status;
    SOUNDOBJ* sobj;

    // �ݒ�f�[�^�擾
    data = GFL_ARC_LoadDataAlloc( ARCID, NARC_h01_trailer1_head_3dsu_data_bin, heapID );
    OBATA_Printf( "trailer1 data[0] = %d\n", data[0] );
    OBATA_Printf( "trailer1 data[1] = %d\n", data[1] );

    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, 0, OBJ_TRAILER_1_HEAD );
    sobj = SOUNDOBJ_Create( fieldmap, status, ISS3DS_UNIT_TRACK09, data[0] << FX32_SHIFT, data[1] );
    SOUNDOBJ_SetAnime( sobj, ARCID, NARC_h01_trailer1_head_ica_data_bin, ANIME_BUF_INTVL );
    work->sobj[SOBJ_TRAILER_1_HEAD] = sobj;

    GFL_HEAP_FreeMemory( data );
     
  }
  { // �g���[���[1(��)
    GFL_G3D_OBJSTATUS* status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, 0, OBJ_TRAILER_1_TAIL );
    SOUNDOBJ* sobj = SOUNDOBJ_CreateDummy( fieldmap, status );
    SOUNDOBJ_SetAnime( sobj, ARCID, NARC_h01_trailer1_tail_ica_data_bin, ANIME_BUF_INTVL );
    work->sobj[SOBJ_TRAILER_1_TAIL] = sobj;
  }
  { // �g���[���[2(�O)
    u32* data;
    GFL_G3D_OBJSTATUS* status;
    SOUNDOBJ* sobj;

    // �ݒ�f�[�^�擾
    data = GFL_ARC_LoadDataAlloc( ARCID, NARC_h01_trailer2_head_3dsu_data_bin, heapID );
    OBATA_Printf( "trailer2 data[0] = %d\n", data[0] );
    OBATA_Printf( "trailer2 data[1] = %d\n", data[1] );

    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, 0, OBJ_TRAILER_2_HEAD );
    sobj = SOUNDOBJ_Create( fieldmap, status, ISS3DS_UNIT_TRACK10, data[0] << FX32_SHIFT, data[1] );
    SOUNDOBJ_SetAnime( sobj, ARCID, NARC_h01_trailer2_head_ica_data_bin, ANIME_BUF_INTVL );
    work->sobj[SOBJ_TRAILER_2_HEAD] = sobj;

    GFL_HEAP_FreeMemory( data );
  } 
  { // �g���[���[2(��)
    GFL_G3D_OBJSTATUS* status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, 0, OBJ_TRAILER_2_TAIL );
    SOUNDOBJ* sobj = SOUNDOBJ_CreateDummy( fieldmap, status );
    SOUNDOBJ_SetAnime( sobj, ARCID, NARC_h01_trailer2_tail_ica_data_bin, ANIME_BUF_INTVL );
    work->sobj[SOBJ_TRAILER_2_TAIL] = sobj;
  } 
  { // �D
    u32* data;
    GFL_G3D_OBJSTATUS* status;
    SOUNDOBJ* sobj;

    // �ݒ�f�[�^�擾
    data = GFL_ARC_LoadDataAlloc( ARCID, NARC_h01_ship_3dsu_data_bin, heapID );
    OBATA_Printf( "ship data[0] = %d\n", data[0] );
    OBATA_Printf( "ship data[1] = %d\n", data[1] );

    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, 0, OBJ_SHIP );
    sobj = SOUNDOBJ_Create( fieldmap, status, ISS3DS_UNIT_TRACK08, data[0] << FX32_SHIFT, data[1] );
    SOUNDOBJ_SetAnime( sobj, ARCID, NARC_h01_ship_ica_data_bin, ANIME_BUF_INTVL );
    work->sobj[SOBJ_SHIP] = sobj;

    GFL_HEAP_FreeMemory( data );
  }

  // ����ҋ@�J�E���^
  for( i=0; i<SOBJ_NUM; i++ ){ work->wait[i] = 0; }

  // �ҋ@����
  LoadWaitTime( work );
  // ���f�[�^
  LoadWindData( work );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �e�����I�u�W�F�N�g�̑҂����Ԃ�ǂݍ���
 *
 * @param work �ݒ�Ώۃ��[�N
 */
//------------------------------------------------------------------------------------------
static void LoadWaitTime( H01WORK* work )
{
  int i;
  ARCDATID dat_id[SOBJ_NUM] = 
  {
    NARC_h01_trailer1_head_wait_data_bin,
    NARC_h01_trailer1_tail_wait_data_bin,
    NARC_h01_trailer2_head_wait_data_bin,
    NARC_h01_trailer2_tail_wait_data_bin,
    NARC_h01_ship_wait_data_bin,
  };

  for( i=0; i<SOBJ_NUM; i++ )
  {
    u32* data = GFL_ARC_LoadDataAlloc( ARCID, dat_id[i], work->heapID );
    work->minWait[i] = data[0];
    work->maxWait[i] = data[1];
    GFL_HEAP_FreeMemory( data );
  }

  // DEBUG:
  OBATA_Printf( "GIMMICK-H01: load wait data\n" );
  for( i=0; i<SOBJ_NUM; i++ )
  {
    OBATA_Printf( "- minWait[%d] = %d\n", i, work->minWait[i] );
    OBATA_Printf( "- maxWait[%d] = %d\n", i, work->maxWait[i] );
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief ���f�[�^��ǂݍ���
 *
 * @param work �ݒ�Ώۃ��[�N
 */
//------------------------------------------------------------------------------------------
static void LoadWindData( H01WORK* work )
{
  GFL_ARC_LoadDataOfs( &work->wind_data,  
                       ARCID, NARC_h01_wind_data_bin, 0, sizeof(WIND_DATA) );
  // DEBUG:
  {
    int i;
    OBATA_Printf( "GIMMICK-H01: load wind data\n" );
    OBATA_Printf( "- trackBit = " );
    for( i=0; i<16; i++ )
    {
      if( work->wind_data.trackBit & (1<<(16-i-1)) ) OBATA_Printf( "��" );
      else                                           OBATA_Printf( "��" );
    }
    OBATA_Printf( "\n" );
    OBATA_Printf( "- minHeight = %d\n", (int)work->wind_data.minHeight );
    OBATA_Printf( "- maxHeight = %d\n", (int)work->wind_data.maxHeight );
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
static void SaveGimmick( H01WORK* work, FIELDMAP_WORK* fieldmap )
{
  int i;
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  u32*         gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H01 );

  // �Z�[�u(�e�����I�u�W�F�N�g�̃A�j���[�V�����t���[����)
  for( i=0; i<SOBJ_NUM; i++ )
  {
    fx32 frame  = SOUNDOBJ_GetAnimeFrame( work->sobj[i] );
    gmk_save[i] = (frame >> FX32_SHIFT);
    OBATA_Printf( "GIMMICK-H01: gimmick save[%d] = %d\n", i, gmk_save[i] );
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
static void LoadGimmick( H01WORK* work, FIELDMAP_WORK* fieldmap )
{
  int i;
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  u32*         gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H01 );

  // �Z�[�u����
  {
    // �e�����I�u�W�F�N�g�̃A�j���[�V�����t���[�����𕜋A
    for( i=0; i<SOBJ_NUM; i++ )
    {
      fx32 frame = gmk_save[i] << FX32_SHIFT;
      SOUNDOBJ_SetAnimeFrame( work->sobj[i], frame );
      OBATA_Printf( "GIMMICK-H01: gmk load[%d] = %d\n", i, gmk_save[i] );
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
static void SetStandBy( H01WORK* work, SOBJ_INDEX index )
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
static void MoveStart( H01WORK* work, SOBJ_INDEX index )
{ 
  SOUNDOBJ_SetAnimeFrame( work->sobj[index], 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���̉��ʂ𒲐�����
 *
 * @param fieldmap �t�B�[���h�}�b�v
 * @param work     �M�~�b�N�Ǘ����[�N
 */
//--------------------------------------------------------------------------------------------
static void UpdateWindVolume( FIELDMAP_WORK* fieldmap, H01WORK* work )
{
  int volume;
  VecFx32 pos;
  float cam_y;
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( fieldmap );

  // �J�������W���擾
  FIELD_CAMERA_GetCameraPos( camera, &pos );
  cam_y = FX_FX32_TO_F32( pos.y );

  // ���̉��ʂ��Z�o
  if( cam_y <= work->wind_data.minHeight )
  {
    volume = 0;
  }
  else if( work->wind_data.maxHeight < cam_y )
  {
    volume = 127;
  }
  else
  {
    float    max = work->wind_data.maxHeight - work->wind_data.minHeight;
    float height = cam_y - work->wind_data.minHeight;
    volume = 127 * height / max;
    // DEBUG:
    OBATA_Printf( "GIMMICK-H01: update wind volume => %d\n", volume );
  }

  // ���ʂ𒲐�
  PMSND_ChangeBGMVolume( work->wind_data.trackBit, volume );
}
