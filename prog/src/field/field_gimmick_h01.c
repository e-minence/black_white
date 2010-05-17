/////////////////////////////////////////////////////////////////////////////
/**
 * @brief  �M�~�b�N�o�^�֐� ( H01 )
 * @file   field_gimmick_h01.c
 * @author obata
 * @date   2009.10.21
 */
/////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "fieldmap.h" 
#include "field_gimmick_h01.h"
#include "field_gimmick_def.h"
#include "gmk_tmp_wk.h"
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
#define GIMMICK_WORK_ASSIGN_ID (0) // �M�~�b�N���[�N�̃A�T�C��ID
#define WIND_FADE_SPEED (16) // ���̉��ʂ̃t�F�[�h���x
#define WIND_MIN_VOLUME (0)   // ���̍ŏ��{�����[��
#define WIND_MAX_VOLUME (127) // ���̍ő�{�����[��

#define PAUL_SOUND_PLAY_SPEED ( FX32_ONE*100 ) // �ʉ߉����o���X�s�[�h
#define PAUL_SOUND_Y_TMP_NUM ( 32 ) // �ʉ߉��@Y���W�ێ���
#define PAUL_SOUND_NOTSE_STOP_POS_Z ( 1816<<FX32_SHIFT )        // �ʉ߉����Ƃ߂Ȃ�������W
#define PAUL_SOUND_PLAY_POS_Z_RUN ( 1884<<FX32_SHIFT )        // �ʉ߉����o��Z���W
#define PAUL_SOUND_PLAY_POS_Z_CYCLE ( 1980<<FX32_SHIFT )        // �ʉ߉����o��Z���W
#define PAUL_SOUND_PLAY_SE_RUN ( SEQ_SE_FLD_173 )        // �ʉ߉� �͂���
#define PAUL_SOUND_PLAY_SE_CYCLE ( SEQ_SE_FLD_171 )      // �ʉ߉� ���]��
#define PAUL_SOUND_FADE_SPEED ( 16 )      // �t�F�[�h�A�E�g�@�X�s�[�h�@���Z�l
#define PAUL_SOUND_FADE_START ( 127 )     // �t�F�[�h�A�E�g�J�n�@�{�����[��

//���ʉ߉�SEPlay�^�C�v
enum{
  PAUL_SOUND_PLAY_NONE,
  PAUL_SOUND_PLAY_PLAY,
  PAUL_SOUND_PLAY_FADEOUT,
};


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
  RES_SHIP_NSBMD,          // �D�̃��f��
  RES_SHIP_NSBTA,          // �D��ita
  RES_NUM
} RES_INDEX;
static const GFL_G3D_UTIL_RES res_table[RES_NUM] = 
{
  { ARCID, NARC_h01_trailer_01a_nsbmd, GFL_G3D_UTIL_RESARC },  // �g���[���[(�O)�̃��f��
  { ARCID, NARC_h01_trailer_01b_nsbmd, GFL_G3D_UTIL_RESARC },  // �g���[���[(��)�̃��f��
  { ARCID, NARC_h01_h01_ship_nsbmd,    GFL_G3D_UTIL_RESARC },  // �D�̃��f��
  { ARCID, NARC_h01_h01_ship_nsbta,    GFL_G3D_UTIL_RESARC },  // �D��ita
};

// �A�j���[�V����
typedef enum {
  SHIP_ANIME_ITA,
  SHIP_ANIME_NUM
} SHIP_ANIME;
static const GFL_G3D_UTIL_ANM ship_anime[ SHIP_ANIME_NUM ] = 
{
  { RES_SHIP_NSBTA, 0 }, // SHIP_ANIME_ITA
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
  { RES_SHIP_NSBMD,         0, RES_SHIP_NSBMD,         ship_anime, SHIP_ANIME_NUM },  // �D
};

// �Z�b�g�A�b�v���
static GFL_G3D_UTIL_SETUP setup = { res_table, RES_NUM, obj_table, OBJ_NUM };


//==========================================================================================
// ����
//==========================================================================================
typedef struct {

  u16 trackBit;     // ����Ώۃg���b�N�r�b�g
  u16 padding;
  float minHeight;  // ������o������
  float maxHeight;  // �����ő�ɂȂ鍂��

} WIND_DATA;


//==========================================================================================
// �����̊Ԃƒʂ鉹
//==========================================================================================
typedef struct {

  fx32 last_z;
  fx32 tmp_y[ PAUL_SOUND_Y_TMP_NUM ];
  s16  tmp_start_index;
  s16  tmp_count;

  // Volume fade out
  s16 se_fadeout;
  u16 se_play;
  u16 se_no;

} PAUL_SOUND;


//==========================================================================================
// ���M�~�b�N���[�N
//==========================================================================================
typedef struct { 

  HEAPID       heapID;            // �g�p����q�[�vID
  ISS_3DS_SYS* iss3dsSys;         // 3D�T�E���h�V�X�e��
  SOUNDOBJ*    sobj[SOBJ_NUM];    // �����I�u�W�F�N�g
  int          wait[SOBJ_NUM];    // �����I�u�W�F�N�g�̓���ҋ@�J�E���^
  int          minWait[SOBJ_NUM]; // �ŒZ�҂�����
  int          maxWait[SOBJ_NUM]; // �Œ��҂�����
  WIND_DATA    wind_data;         // ���f�[�^
	int          wind_volume;       // ���̉���
  PAUL_SOUND   paul_sound;        // ���̊Ԃ�ʂ鉹

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
// ��
static void ApplyWindVolumeForBGM( const H01WORK* work ); // ���̉��ʂ�BGM�ɔ��f������
static void UpdateWindVolume( H01WORK* work, FIELDMAP_WORK* fieldmap ); // ���̉��ʂ��X�V����
static int CalcWindVolume( const H01WORK* work, FIELDMAP_WORK* fieldmap ); // ���̉��� ( �ڕW�l ) ���v�Z����
static void WindVolumeUp( H01WORK* work, int max_volume ); // ���̉��ʂ��グ��
static void WindVolumeDown( H01WORK* work, int min_volume ); // ���̉��ʂ�������
// ���̊Ԃ�ʂ�
static void InitPaulSound( PAUL_SOUND* work );
static void UpdatePaulSound( PAUL_SOUND* work, const FIELD_CAMERA* camera, const FIELD_PLAYER* cp_player );


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
  H01WORK* work;  // H01�M�~�b�N�Ǘ����[�N
  HEAPID              heapID    = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  FLD_EXP_OBJ_AddUnitByHandle( exobj_cnt, &setup, EXPOBJ_UNIT_IDX );

  // �D�̃A�j���[�V�����J�n
  {
    EXP_OBJ_ANM_CNT_PTR exobj_anm; 
    FLD_EXP_OBJ_ValidCntAnm( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_SHIP, SHIP_ANIME_ITA, TRUE ); 
    exobj_anm = FLD_EXP_OBJ_GetAnmCnt( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_SHIP, SHIP_ANIME_ITA );
    FLD_EXP_OBJ_ChgAnmStopFlg( exobj_anm, FALSE );
    FLD_EXP_OBJ_ChgAnmLoopFlg( exobj_anm, TRUE );
  }

  // �M�~�b�N�Ǘ����[�N���쐬
  work = (H01WORK*)GMK_TMP_WK_AllocWork( fieldmap, GIMMICK_WORK_ASSIGN_ID, heapID, sizeof(H01WORK) );

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
void H01_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  int i;
  H01WORK* work = (H01WORK*)GMK_TMP_WK_GetWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );

  // ��ISS�̃}�X�^�[�{�����[�����i��
  ISS_3DS_SYS_SetMasterVolume( work->iss3dsSys, 0 );

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
  GMK_TMP_WK_FreeWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );
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
  H01WORK* work = (H01WORK*)GMK_TMP_WK_GetWork( fieldmap, GIMMICK_WORK_ASSIGN_ID );
  FIELD_CAMERA* fieldCamera;
  fieldCamera = FIELDMAP_GetFieldCamera( fieldmap );

  // �ϑ��҂̈ʒu��ݒ�
  {
    VecFx32 cameraPos, targetPos;

    FIELD_CAMERA_GetCameraPos( fieldCamera, &cameraPos );
    FIELD_CAMERA_GetTargetPos( fieldCamera, &targetPos );
    ISS_3DS_SYS_SetObserverPos( work->iss3dsSys, &cameraPos, &targetPos );
  }

  // ���ׂẲ����I�u�W�F�N�g�𓮂���
  for( i=0; i<SOBJ_NUM; i++ )
  { 
    // �ҋ@��� ==> ���ԃJ�E���g�_�E��
    if( 0 < work->wait[i] ) {
      // �J�E���g�_�E���I�� ==> ����
      if( --work->wait[i] <= 0 ) {
        MoveStart( work, i );
        // �㕔����O�����ɒǏ]������
        switch(i) {
        case SOBJ_TRAILER_1_HEAD: work->wait[SOBJ_TRAILER_1_TAIL] = TAIL_INTERVAL; break;
        case SOBJ_TRAILER_2_HEAD: work->wait[SOBJ_TRAILER_2_TAIL] = TAIL_INTERVAL; break;
        }
      }
    }
    // ���쒆 ==> �A�j���[�V�������X�V
    else {
      // �A�j���[�V�����Đ����I�� ==> �ҋ@��Ԃ�
      if( SOUNDOBJ_IncAnimeFrame( work->sobj[i], FX32_ONE ) ) {
        SetStandBy( work, i );
      }
    }
  }

  // �����X�V
  UpdateWindVolume( work, fieldmap );
	ApplyWindVolumeForBGM( work );

  // �g���I�u�W�F�̃A�j���[�V�����Đ�
  {
    FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
    FLD_EXP_OBJ_PlayAnime( exobj_cnt );
  }

  // ���̊ԉ��f�[�^
  UpdatePaulSound(&work->paul_sound, fieldCamera, FIELDMAP_GetFieldPlayer( fieldmap ));

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

  work->heapID = heapID;
	work->wind_volume = 0;

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
  // ���̊ԉ��f�[�^
  InitPaulSound(&work->paul_sound);
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
 * @brief ���̉��ʂ�BGM�ɔ��f������
 *
 * @param work
 */
//--------------------------------------------------------------------------------------------
static void ApplyWindVolumeForBGM( const H01WORK* work )
{
  PMSND_ChangeBGMVolume( work->wind_data.trackBit, work->wind_volume );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���̉��ʂ𒲐�����
 *
 * @param fieldmap �t�B�[���h�}�b�v
 * @param work     �M�~�b�N�Ǘ����[�N
 */
//--------------------------------------------------------------------------------------------
static void UpdateWindVolume( H01WORK* work, FIELDMAP_WORK* fieldmap )
{
  int volume;

  // ���̉��ʂ��Z�o
	volume = CalcWindVolume( work, fieldmap );

  // ���ʂ𒲐� ( �t�F�[�h )
	if( work->wind_volume < volume ) {
		WindVolumeUp( work, volume );
	}
	else if( volume < work->wind_volume ) {
		WindVolumeDown( work, volume );
	} 
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���̉��� ( �ڕW�l ) ���v�Z����
 *
 * @param work     �M�~�b�N�Ǘ����[�N
 * @param fieldmap 
 *
 * @return ���݂̏󋵂ɂ����镗�̉���
 */
//--------------------------------------------------------------------------------------------
static int CalcWindVolume( const H01WORK* work, FIELDMAP_WORK* fieldmap )
{
  int volume;
  float cam_y;

  // �J�������W���擾
	{
		FIELD_CAMERA* camera;
		VecFx32 pos;

		camera = FIELDMAP_GetFieldCamera( fieldmap );
		FIELD_CAMERA_GetCameraPos( camera, &pos );
		cam_y = FX_FX32_TO_F32( pos.y );
	}

  // ���ʂ��Z�o
  if( cam_y <= work->wind_data.minHeight ) {
		// �ŏ�
    volume = WIND_MIN_VOLUME;
  }
  else if( work->wind_data.maxHeight < cam_y ) {
		// �ő�
    volume = WIND_MAX_VOLUME;
  }
  else {
		// ���
    float max = work->wind_data.maxHeight - work->wind_data.minHeight;
    float now = cam_y - work->wind_data.minHeight;
    volume = WIND_MIN_VOLUME + (WIND_MAX_VOLUME - WIND_MIN_VOLUME) * now / max;
  }

	return volume;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���̉��ʂ��グ��
 *
 * @param work       �M�~�b�N�Ǘ����[�N
 * @param max_volume �ő剹��
 */
//--------------------------------------------------------------------------------------------
static void WindVolumeUp( H01WORK* work, int max_volume )
{
	GF_ASSERT( work->wind_volume < max_volume );

	work->wind_volume += WIND_FADE_SPEED;

	// �ő�l�␳
	if( max_volume < work->wind_volume ) {
		work->wind_volume = max_volume;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief ���̉��ʂ�������
 *
 * @param work       �M�~�b�N�Ǘ����[�N
 * @param min_volume �ŏ�����
 */
//--------------------------------------------------------------------------------------------
static void WindVolumeDown( H01WORK* work, int min_volume )
{
	GF_ASSERT( min_volume < work->wind_volume );

	work->wind_volume -= WIND_FADE_SPEED;

	// �ŏ��l�␳
	if( work->wind_volume < min_volume ) {
		work->wind_volume = min_volume;
	}
}



//----------------------------------------------------------------------------
/**
 *	@brief  ���̊Ԃ�ʂ鉹�@������
 *
 *	@param	work  ���[�N
 */
//-----------------------------------------------------------------------------
static void InitPaulSound( PAUL_SOUND* work )
{
  GFL_STD_MemClear32( work, sizeof(PAUL_SOUND) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���̊Ԃ�ʂ鉹�@�X�V����
 *
 *	@param	work      ���[�N
 *	@param	camera    �J����
 */
//-----------------------------------------------------------------------------
static void UpdatePaulSound( PAUL_SOUND* work, const FIELD_CAMERA* camera, const FIELD_PLAYER* cp_player )
{
  VecFx32 camera_pos;
  fx32 dist_z;
  fx32 check_z;
  int se;
  u32 draw_form;


  // SE�̒�~�`�F�b�N
  if( (work->se_play == PAUL_SOUND_PLAY_PLAY) || (work->se_play == PAUL_SOUND_PLAY_FADEOUT) ){
    if( PMSND_CheckPlaySE_byPlayerID( PMSND_GetSE_DefaultPlayerID( SEQ_SE_FLD_171 ) ) == FALSE ){
      work->se_play = PAUL_SOUND_PLAY_NONE;
    }
    
    // �t�F�[�h�A�E�g����
    if( work->se_play == PAUL_SOUND_PLAY_FADEOUT ){
      work->se_fadeout -= PAUL_SOUND_FADE_SPEED;
      if( work->se_fadeout < 0 ){
        PMSND_StopSE_byPlayerID( PMSND_GetSE_DefaultPlayerID( work->se_no ) );// ��~
        work->se_play = PAUL_SOUND_PLAY_NONE; //�t�F�[�h�A�E�g����
      }else{
        PMSND_PlayerSetVolume( PMSND_GetSE_DefaultPlayerID( work->se_no ), work->se_fadeout );
      }

    }
  }
  

  // �ړ��X�s�[�h�v��
  FIELD_CAMERA_GetCameraPos( camera, &camera_pos );
  dist_z = MATH_ABS( camera_pos.z - work->last_z );
  if( dist_z > 0 ){
    // �ړ��t���[���B
    s32 set_index;

    if( work->tmp_count < PAUL_SOUND_Y_TMP_NUM ){
      set_index = (work->tmp_start_index + work->tmp_count) % PAUL_SOUND_Y_TMP_NUM;
      work->tmp_count ++;
    }else{
      set_index = work->tmp_start_index;
      work->tmp_start_index = (work->tmp_start_index + 1) % PAUL_SOUND_Y_TMP_NUM;
    }

    work->tmp_y[ set_index ] = camera_pos.y;
    //TOMOYA_Printf( "set index %d   count %d\n", set_index, work->tmp_count );
  }else{
    // �ړ��Ȃ��t���[��
    // �S�N���A
    work->tmp_count = 0;
    work->tmp_start_index = 0;

    // SE�t�F�[�h�A�E�g�J�n
    if( (work->se_play == PAUL_SOUND_PLAY_PLAY) && (camera_pos.z > PAUL_SOUND_NOTSE_STOP_POS_Z) ){
      work->se_play = PAUL_SOUND_PLAY_FADEOUT;
      work->se_fadeout = PAUL_SOUND_FADE_START;
      TOMOYA_Printf( "fadeout On z %d\n", camera_pos.z>>FX32_SHIFT );
    }
    //TOMOYA_Printf( "tmp y clear\n" );
  }


  
  // �J�����ʒu���炷�ꏊ��ʉ߂������`�F�b�N�B
  if( work->tmp_count > 0 ){

    // �ړ��X�s�[�h����A����p�Ǝ��]�ԗp��؂�ւ���B
    draw_form = FIELD_PLAYER_GetDrawForm( cp_player );
    if( draw_form == PLAYER_DRAW_FORM_NORMAL  ){
      check_z = PAUL_SOUND_PLAY_POS_Z_RUN;
      se     = PAUL_SOUND_PLAY_SE_RUN;
    }else{
      check_z = PAUL_SOUND_PLAY_POS_Z_CYCLE;
      se     = PAUL_SOUND_PLAY_SE_CYCLE;
    }

    // �ʉ߂����Ƃ��ɂ����炷�B
    if( ((camera_pos.z < check_z) && (work->last_z > check_z)) || 
        ((camera_pos.z > check_z) && (work->last_z < check_z)) ){
      fx32 dist;

      // ��Ԑ̂̍��W����ǂ̂��炢��������̂��H
      dist = work->tmp_y[ work->tmp_start_index ] - camera_pos.y;
    
      TOMOYA_Printf( "camera_speed 0x%x\n", dist );
      if( dist >= PAUL_SOUND_PLAY_SPEED ){
        PMSND_PlaySE( se );
        work->se_play = PAUL_SOUND_PLAY_PLAY;
        work->se_no = se;
        TOMOYA_Printf( "sound on\n" );
      }
    }
  }

  work->last_z = camera_pos.z;
}

