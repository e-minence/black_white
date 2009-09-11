#include <gflib.h>
#include "fieldmap.h"
#include "field_gimmick_h01.h"
#include "arc/fieldmap/buildmodel_outdoor.naix"
#include "savedata/gimmickwork.h"
#include "field_gimmick_def.h"
#include "gamesystem/iss_3ds_unit.h"
#include "gamesystem/iss_3ds_sys.h"
#include "sound/pm_sndsys.h"
#include "arc/iss.naix"
#include "system/ica_anime.h"   // TEST:
#include "arc/debug_obata.naix"// TEST:

#define ICA_NUM (4)//TEST:

//======================================================================
/**
 * @breif �萔
 */
//======================================================================

//------------
// �g���[���[
//------------ 
// �ړ��͈�
#define TR_Z_MIN (1200 << FX32_SHIFT)
#define TR_Z_MAX (4230 << FX32_SHIFT) 
#define TR_X_01 (1756 << FX32_SHIFT)
#define TR_X_02 (1895 << FX32_SHIFT)
#define TR_Y    (490 << FX32_SHIFT) 
// �ړ�����
#define TR_FRAME (300)

//-----
// �D
//-----
// �ړ��͈�
#define SHIP_X_MIN (0 << FX32_SHIFT) 
#define SHIP_X_MAX (4000 << FX32_SHIFT) 
#define SHIP_Y     (-35 << FX32_SHIFT)
#define SHIP_Z     (2386 << FX32_SHIFT)  
// �ړ�����
#define SHIP_FRAME (1000)

//-------------
// FLD_EXP_OBJ
//-------------
#define EXPOBJ_UNIT_IDX (0)  // �g���I�u�W�F�N�g�̃��j�b�g�ԍ�

// 3D���\�[�X
static const GFL_G3D_UTIL_RES res_table[] = 
{
  { ARCID_BMODEL_OUTDOOR, NARC_output_buildmodel_outdoor_trailer_01_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_BMODEL_OUTDOOR, NARC_output_buildmodel_outdoor_trailer_01_nsbmd, GFL_G3D_UTIL_RESARC },
  { ARCID_BMODEL_OUTDOOR, NARC_output_buildmodel_outdoor_h01_ship_nsbmd, GFL_G3D_UTIL_RESARC },
};

// 3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ obj_table[] = 
{
  { 0, 0, 0, NULL, 0 },
  { 1, 0, 1, NULL, 0 },
  { 2, 0, 2, NULL, 0 },
};

// UTIL�Z�b�g�A�b�v���
static GFL_G3D_UTIL_SETUP setup = { res_table, NELEMS(res_table), obj_table, NELEMS(obj_table) };


//----------------------
// �����I�u�W�F�N�g�ԍ�
//----------------------
typedef enum
{
  SOUNDOBJ_TR_L,
  SOUNDOBJ_TR_R,
  SOUNDOBJ_SHIP,
  SOUNDOBJ_NUM
} SOUNDOBJ_INDEX;


//============================================================================================
/**
 * @breif �����I�u�W�F�N�g(�g���[���E�D�Ȃ�)
 */
//============================================================================================
typedef struct
{
  SOUNDOBJ_INDEX     index; // ���g��ID
  u8         iss3dsUnitIdx; // 3D�T�E���h���j�b�g�̊Ǘ��C���f�b�N�X
  ISS_3DS_UNIT* iss3dsUnit; // 3D�T�E���h���j�b�g
  u16                frame; // ���݃t���[��
  u16             endFrame; // �ŏI�t���[��
  VecFx32         startPos; // �J�n�ʒu
  VecFx32           endPos; // �ŏI�ʒu 
}
SOUNDOBJ;


//============================================================================================
/**
 * @breif �M�~�b�N�Ǘ����[�N
 */
//============================================================================================
typedef struct
{
  ISS_3DS_SYS*          iss3DSSys;  // 3D�T�E���h�V�X�e��
  SOUNDOBJ soundObj[SOUNDOBJ_NUM];  // �����I�u�W�F�N�g
  int                windTrackBit;  // �����̃g���b�N�}�X�N
  fx32               windRangeMin;  // �������ŏ��ɂȂ鍂��
  fx32               windRangeMax;  // �������ő�ɂȂ鍂��
  ICA_ANIME* icaAnime[ICA_NUM]; // TEST:
}
H01WORK;


//============================================================================================
/**
 * @brief �v���g�^�C�v�錾
 */
//============================================================================================
static void Load3DSoundInfo( H01WORK* work, HEAPID heap_id );
static void MoveSoundObj( FIELDMAP_WORK* fieldmap, SOUNDOBJ* sobj );
static void UpdateWindVolume( FIELDMAP_WORK* fieldmap, H01WORK* work );



//======================================================================
/**
 * @brief �M�~�b�N�o�^�֐�
 */
//======================================================================

//--------------------------------------------------------------------
/**
 * @brief �Z�b�g�A�b�v�֐�
 */
//--------------------------------------------------------------------
void H01_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
  HEAPID          heap_id = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldmap ); 
  GAMESYS_WORK*      gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK*   sv = GAMEDATA_GetSaveControlWork( gdata );
  GIMMICKWORK*    gmkwork = SaveData_GetGimmickWork( sv );
  u16 frame[SOUNDOBJ_NUM] = {0, 0, 0};
  SOUNDOBJ* sobj;
  GFL_G3D_OBJSTATUS* status;
  H01WORK* h01work;
  int work_adrs;

  // �M�~�b�N�Ǘ����[�N���쐬
  h01work = GFL_HEAP_AllocMemory( heap_id, sizeof( H01WORK ) );

  // �Z�[�u�f�[�^���`�F�b�N
  if( GIMMICKWORK_GetAssignID( gmkwork ) == FLD_GIMMICK_H01 )
  {
    work_adrs = (int)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H01 );
    frame[0] = *( (u16*)( work_adrs + 4 ) );
    frame[1] = *( (u16*)( work_adrs + 6 ) );
    frame[2] = *( (u16*)( work_adrs + 8 ) );
  }
  else  // ����Z�b�g�A�b�v��
  {
    work_adrs = (int)GIMMICKWORK_Assign( gmkwork, FLD_GIMMICK_H01 );
  }

  // �M�~�b�N���[�N�ɊǗ����[�N�̃A�h���X��ۑ�
  *( (H01WORK**)work_adrs ) = h01work;

  // �g���I�u�W�F�N�g�̃��j�b�g���쐬
  FLD_EXP_OBJ_AddUnit( ptr, &setup, EXPOBJ_UNIT_IDX );

  // �M�~�b�N�Ǘ����[�N������
  {
    // 3D�T�E���h�V�X�e���쐬
    FIELD_CAMERA* fc = FIELDMAP_GetFieldCamera( fieldmap );
    const GFL_G3D_CAMERA* camera = FIELD_CAMERA_GetCameraPtr( fc );
    h01work->iss3DSSys = ISS_3DS_SYS_Create( heap_id, SOUNDOBJ_NUM, camera );

    // TEST:
    {
      int i;
      for( i=0; i<ICA_NUM; i++ )
      {
        h01work->icaAnime[i] = ICA_ANIME_CreateStreaming( 
            heap_id, ARCID_OBATA_DEBUG, NARC_debug_obata_ica_test_data2_bin, 10 );
      }
    }
  }

  // �e�I�u�W�F�N�g�̏����X�e�[�^�X��ݒ�
  sobj = &h01work->soundObj[SOUNDOBJ_TR_L];
  sobj->index = SOUNDOBJ_TR_L;
  sobj->frame = frame[SOUNDOBJ_TR_L];
  sobj->endFrame = TR_FRAME;
  VEC_Set( &sobj->startPos, TR_X_01, TR_Y, TR_Z_MIN );
  VEC_Set( &sobj->endPos,   TR_X_01, TR_Y, TR_Z_MAX );
  status = FLD_EXP_OBJ_GetUnitObjStatus( ptr, EXPOBJ_UNIT_IDX, SOUNDOBJ_TR_L );
  VEC_Set( &status->trans, 0, 0, 0 );
  VEC_Set( &status->scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_RotY33( &status->rotate, FX_SinIdx( 32768 ), FX_CosIdx( 32768 ) );	  // y��180�x��]
  sobj->iss3dsUnitIdx = ISS_3DS_SYS_AddUnit( h01work->iss3DSSys );
  sobj->iss3dsUnit = ISS_3DS_SYS_GetUnit( h01work->iss3DSSys, sobj->iss3dsUnitIdx );

  sobj = &h01work->soundObj[SOUNDOBJ_TR_R];
  sobj->index = SOUNDOBJ_TR_R;
  sobj->frame = frame[SOUNDOBJ_TR_R];
  sobj->endFrame = TR_FRAME;
  VEC_Set( &sobj->startPos, TR_X_02, TR_Y, TR_Z_MAX );
  VEC_Set( &sobj->endPos,   TR_X_02, TR_Y, TR_Z_MIN );
  status = FLD_EXP_OBJ_GetUnitObjStatus( ptr, EXPOBJ_UNIT_IDX, SOUNDOBJ_TR_R );
  VEC_Set( &status->trans, 0, 0, 0 );
  VEC_Set( &status->scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &status->rotate );
  sobj->iss3dsUnitIdx = ISS_3DS_SYS_AddUnit( h01work->iss3DSSys );
  sobj->iss3dsUnit = ISS_3DS_SYS_GetUnit( h01work->iss3DSSys, sobj->iss3dsUnitIdx );

  sobj = &h01work->soundObj[SOUNDOBJ_SHIP];
  sobj->index = SOUNDOBJ_SHIP;
  sobj->frame = frame[SOUNDOBJ_SHIP];
  sobj->endFrame = SHIP_FRAME;
  VEC_Set( &sobj->startPos, SHIP_X_MIN, SHIP_Y, SHIP_Z );
  VEC_Set( &sobj->endPos,   SHIP_X_MAX, SHIP_Y, SHIP_Z );
  status = FLD_EXP_OBJ_GetUnitObjStatus( ptr, EXPOBJ_UNIT_IDX, SOUNDOBJ_SHIP );
  VEC_Set( &status->trans, 0, 0, 0 );
  VEC_Set( &status->scale, FX32_ONE, FX32_ONE, FX32_ONE );
  MTX_Identity33( &status->rotate );
  sobj->iss3dsUnitIdx = ISS_3DS_SYS_AddUnit( h01work->iss3DSSys );
  sobj->iss3dsUnit = ISS_3DS_SYS_GetUnit( h01work->iss3DSSys, sobj->iss3dsUnitIdx );

  // 3D�T�E���h�����擾
  Load3DSoundInfo( h01work, heap_id );
}

//--------------------------------------------------------------------
/**
 * @brief �I���֐�
 */
//--------------------------------------------------------------------
void H01_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{

  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldmap ); 
  GAMESYS_WORK*      gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK*   sv = GAMEDATA_GetSaveControlWork( gdata );
  GIMMICKWORK*    gmkwork = SaveData_GetGimmickWork( sv );
  int                 ofs = 0;
  int           work_adrs = (int)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H01 );
  H01WORK*        h01work = (H01WORK*)( *( (H01WORK**)work_adrs ) );

  // �d�|���p���[�N��������
  work_adrs = (int)GIMMICKWORK_Assign( gmkwork, FLD_GIMMICK_H01 );

  // �f�[�^��ۑ�
  ofs = 4;
  *( (u16*)(work_adrs + ofs) ) = h01work->soundObj[SOUNDOBJ_TR_L].frame;
  ofs += sizeof( u16 );
  *( (u16*)(work_adrs + ofs) ) = h01work->soundObj[SOUNDOBJ_TR_R].frame;
  ofs += sizeof( u16 );
  *( (u16*)(work_adrs + ofs) ) = h01work->soundObj[SOUNDOBJ_SHIP].frame;
  ofs += sizeof( u16 );

  // 3D�T�E���h�V�X�e���j��
  ISS_3DS_SYS_Delete( h01work->iss3DSSys );

  // TEST:
  {
    int i;

    for( i=0; i<ICA_NUM; i++ )
    {
      ICA_ANIME_Delete( h01work->icaAnime[i] );
    }
  }

  // �M�~�b�N�Ǘ����[�N�j��
  GFL_HEAP_FreeMemory( h01work );
} 

//--------------------------------------------------------------------
/**
 * @brief ����֐�
 */
//--------------------------------------------------------------------
void H01_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  int i;
  GAMESYS_WORK*      gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*         gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK*   sv = GAMEDATA_GetSaveControlWork( gdata );
  GIMMICKWORK*    gmkwork = SaveData_GetGimmickWork( sv );
  int           work_adrs = (int)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H01 );
  H01WORK*        h01work = (H01WORK*)( *( (H01WORK**)work_adrs ) );

  // ���ׂẲ����I�u�W�F�N�g�𓮂���
  for( i=0; i<SOUNDOBJ_NUM; i++ )
  {
    MoveSoundObj( fieldmap, &h01work->soundObj[i] );
  }

  // 3D�T�E���h�V�X�e������
  ISS_3DS_SYS_Main( h01work->iss3DSSys );

  // ���̉��ʂ𒲐�
  UpdateWindVolume( fieldmap, h01work );

  // TEST:
  {
    int i;
    for( i=0; i<ICA_NUM; i++ )
    {
      ICA_ANIME_IncAnimeFrame( h01work->icaAnime[i], FX32_ONE );
    }
  }
}

//--------------------------------------------------------------------------------------------
/**
 * @brief 3D�T�E���h�����擾����
 *
 * @param work    �擾�����f�[�^�𔽉f������M�~�b�N�Ǘ����[�N
 * @param heap_id �g�p����q�[�vID
 */
//--------------------------------------------------------------------------------------------
static void Load3DSoundInfo( H01WORK* work, HEAPID heap_id )
{
  void* data;
  int ofs = 0;
  int track, volume;
  float height0, height1, range;
  ISS_3DS_UNIT* unit;

  // �A�[�J�C�u�t�@�C���f�[�^��ǂݍ���
  data = GFL_ARC_LoadDataAlloc( ARCID_ISS_UNIT, NARC_iss_3d_sound_h01_bin, heap_id );

  // ��
  track   = *( (int*)((int)data + ofs) );   ofs += sizeof(int);
  height0 = *( (float*)((int)data + ofs) ); ofs += sizeof(float);
  height1 = *( (float*)((int)data + ofs) ); ofs += sizeof(float);
  work->windTrackBit = 1 << (track - 1);
  work->windRangeMin = FX_F32_TO_FX32( height0 );
  work->windRangeMax = FX_F32_TO_FX32( height1 );
  OBATA_Printf( "WIND : %d, %d, %d\n", track, (int)height0, (int)height1 );

  // �D
  track = *( (int*)((int)data + ofs) );   ofs += sizeof(int);
  range = *( (float*)((int)data + ofs) ); ofs += sizeof(float);
  volume = *( (int*)((int)data + ofs) );   ofs += sizeof(int);
  unit = work->soundObj[SOUNDOBJ_SHIP].iss3dsUnit;
  ISS_3DS_UNIT_SetTrackBit( unit, 1<<(track-1) );
  ISS_3DS_UNIT_SetRange( unit, FX_F32_TO_FX32(range) );
  ISS_3DS_UNIT_SetMaxVolume( unit, volume );
  OBATA_Printf( "SOUNDOBJ_SHIP : %d, %d, %d\n", track, (int)range, volume );

  // �g���[��L
  track = *( (int*)((int)data + ofs) );   ofs += sizeof(int);
  range = *( (float*)((int)data + ofs) ); ofs += sizeof(float);
  volume = *( (int*)((int)data + ofs) );   ofs += sizeof(int);
  unit = work->soundObj[SOUNDOBJ_TR_L].iss3dsUnit;
  ISS_3DS_UNIT_SetTrackBit( unit, 1<<(track-1) );
  ISS_3DS_UNIT_SetRange( unit, FX_F32_TO_FX32(range) );
  ISS_3DS_UNIT_SetMaxVolume( unit, volume );
  OBATA_Printf( "SOUNDOBJ_TR_L : %d, %d, %d\n", track, (int)range, volume );

  // �g���[��R
  track = *( (int*)((int)data + ofs) );   ofs += sizeof(int);
  range = *( (float*)((int)data + ofs) ); ofs += sizeof(float);
  volume = *( (int*)((int)data + ofs) );   ofs += sizeof(int);
  unit = work->soundObj[SOUNDOBJ_TR_R].iss3dsUnit;
  ISS_3DS_UNIT_SetTrackBit( unit, 1<<(track-1) );
  ISS_3DS_UNIT_SetRange( unit, FX_F32_TO_FX32(range) );
  ISS_3DS_UNIT_SetMaxVolume( unit, volume );
  OBATA_Printf( "SOUNDOBJ_TR_R : %d, %d, %d\n", track, (int)range, volume );

  // ��n��
  GFL_HEAP_FreeMemory( data );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief �����I�u�W�F�𓮂���
 *
 * @param fieldmap �t�B�[���h�}�b�v
 * @param sobj     �������I�u�W�F�N�g
 */
//--------------------------------------------------------------------------------------------
static void MoveSoundObj( FIELDMAP_WORK* fieldmap, SOUNDOBJ* sobj )
{
  fx32 w0, w1;
  VecFx32 v0, v1;
  FLD_EXP_OBJ_CNT_PTR   ptr = FIELDMAP_GetExpObjCntPtr( fieldmap ); 
  GFL_G3D_OBJSTATUS* status = FLD_EXP_OBJ_GetUnitObjStatus( ptr, EXPOBJ_UNIT_IDX, sobj->index );

  // �t���[���i�߂�
  sobj->frame = (sobj->frame + 1) % sobj->endFrame;

  // �ʒu�x�N�g������`���
  w1 = FX_Div( sobj->frame, sobj->endFrame );
  w0 = FX32_ONE - w1;
  GFL_CALC3D_VEC_MulScalar( &sobj->startPos, w0, &v0 );
  GFL_CALC3D_VEC_MulScalar( &sobj->endPos,   w1, &v1 );
  VEC_Add( &v0, &v1, &status->trans );

  // �����ʒu�����킹��
  ISS_3DS_UNIT_SetPos( sobj->iss3dsUnit, &status->trans );
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
  FIELD_CAMERA* camera = FIELDMAP_GetFieldCamera( fieldmap );

  // �J�������W���擾
  FIELD_CAMERA_GetCameraPos( camera, &pos );

  // ���̉��ʂ��Z�o
  if( pos.y <= work->windRangeMin ) 
  {
    volume = 0;
  }
  else if( work->windRangeMax <= pos.y )
  {
    volume = 127;
  }
  else
  {
    fx32    max = work->windRangeMax - work->windRangeMin;
    fx32 height = pos.y - work->windRangeMin;
    fx32 rate   = FX_Div( height, max );
    volume = 127 * FX_FX32_TO_F32( rate );
  }

  // ���ʂ𒲐�
  PMSND_ChangeBGMVolume( work->windTrackBit, volume );
}
