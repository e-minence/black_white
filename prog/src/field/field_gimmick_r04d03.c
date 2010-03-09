//======================================================================
/**
 * @file  field_gimmick_r04d03.c
 * @brief  4�ԓ��H+���]�[�g�f�U�[�g
 * @author  Saito
 */
//======================================================================

#include <gflib.h>
#include "fieldmap.h" 
#include "field_gimmick_r04d03.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
//#include "sound/pm_sndsys.h"
#include "system/gfl_use.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/arc_def.h"
#include "arc/h01.naix"
#include "gmk_tmp_wk.h"

#define EXPOBJ_UNIT_IDX (0)
#define ARCID (ARCID_H01_GIMMICK) // �M�~�b�N�f�[�^�̃A�[�J�C�uID
#define R04D03_TMP_ASSIGN_ID  (1)

#define TRAINER_TAIL_OFS (6*FIELD_CONST_GRID_FX32_SIZE)
#define TRAINER_Y (FIELD_CONST_GRID_FX32_SIZE*1)
#define TRAINER_RIGHT_X (447*FIELD_CONST_GRID_FX32_SIZE)
#define TRAINER_LEFT_X (338*FIELD_CONST_GRID_FX32_SIZE)
#define TRAINER_TAIL_RIGHT_X  (TRAINER_RIGHT_X+TRAINER_TAIL_OFS)
#define TRAINER_TAIL_LEFT_X   (TRAINER_LEFT_X-TRAINER_TAIL_OFS)


#define TRAINER1_Z (526*FIELD_CONST_GRID_FX32_SIZE)
#define TRAINER2_Z (520*FIELD_CONST_GRID_FX32_SIZE)


#define TRAINER_SPEED (FIELD_CONST_GRID_FX32_SIZE)

#define TRAINER_MAX (2)

//==========================================================================================
// ���M�~�b�N���[�N
//==========================================================================================
typedef struct
{ 
  int Frame[TRAINER_MAX];
  int Wait[TRAINER_MAX];
} GMK_WORK;

//==========================================================================================
// ��3D���\�[�X
//==========================================================================================
// ���\�[�X
typedef enum {
  RES_TRAILER_HEAD_NSBMD,  // �g���[���[(�O)�̃��f��
  RES_TRAILER_TAIL_NSBMD,  // �g���[���[(��)�̃��f��
  RES_NUM
} RES_INDEX;

static const GFL_G3D_UTIL_RES res_table[RES_NUM] = 
{
  { ARCID, NARC_h01_trailer_01a_nsbmd, GFL_G3D_UTIL_RESARC },  // �g���[���[(�O)�̃��f��
  { ARCID, NARC_h01_trailer_01b_nsbmd, GFL_G3D_UTIL_RESARC },  // �g���[���[(��)�̃��f��
};

// �I�u�W�F�N�g
typedef enum {
  OBJ_TRAILER_1_HEAD,  // �g���[���[1(�O)
  OBJ_TRAILER_1_TAIL,  // �g���[���[1(��)
  OBJ_TRAILER_2_HEAD,  // �g���[���[2(�O)
  OBJ_TRAILER_2_TAIL,  // �g���[���[2(��)
  OBJ_NUM
} OBJ_INDEX;

static const GFL_G3D_UTIL_OBJ obj_table[OBJ_NUM] = 
{
  { RES_TRAILER_HEAD_NSBMD, 0, RES_TRAILER_HEAD_NSBMD, NULL, 0 },  // �g���[���[1(�O)
  { RES_TRAILER_TAIL_NSBMD, 0, RES_TRAILER_TAIL_NSBMD, NULL, 0 },  // �g���[���[1(��)
  { RES_TRAILER_HEAD_NSBMD, 0, RES_TRAILER_HEAD_NSBMD, NULL, 0 },  // �g���[���[2(�O)
  { RES_TRAILER_TAIL_NSBMD, 0, RES_TRAILER_TAIL_NSBMD, NULL, 0 },  // �g���[���[2(��)
};

// �Z�b�g�A�b�v���
static GFL_G3D_UTIL_SETUP setup = { res_table, RES_NUM, obj_table, OBJ_NUM };


static void InitWork( GMK_WORK* work, FIELDMAP_WORK* fieldmap );

//------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void R04D03_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
  u32* gmk_save;  // �M�~�b�N�̎��Z�[�u�f�[�^
  GMK_WORK* work;  // �M�~�b�N�Ǘ����[�N
  HEAPID                heapID = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  GAMESYS_WORK*            gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*               gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*          gmkwork = GAMEDATA_GetGimmickWork(gdata);

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldmap, R04D03_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldmap), sizeof(GMK_WORK));
  work = GMK_TMP_WK_GetWork(fieldmap, R04D03_TMP_ASSIGN_ID);
  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &setup, EXPOBJ_UNIT_IDX );

  // �M�~�b�N�Ǘ����[�N�������� 
  InitWork( work, fieldmap );
/**
  // ���[�h
  LoadGimmick( work, fieldmap );

  // �M�~�b�N�Ǘ����[�N�̃A�h���X��ۑ�
  gmk_save    = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H01 );
  gmk_save[0] = (int)work;
*/
  // DEBUG:
  NOZOMU_Printf( "GIMMICK: set up\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief �I���֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void R04D03_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  int i;
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
/**
  u32*         gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H01 );
  GMK_WORK*         work = (GMK_WORK*)gmk_save[0]; // gmk_save[0]�̓M�~�b�N�Ǘ����[�N�̃A�h���X
*/

/**
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
*/
  //���j�b�g�j��
  FLD_EXP_OBJ_DelUnit( exobj_cnt, EXPOBJ_UNIT_IDX );  
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldmap, R04D03_TMP_ASSIGN_ID);
  // DEBUG:
  NOZOMU_Printf( "GIMMICK: end\n" );
}

//------------------------------------------------------------------------------------------
/**
 * @brief ����֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void R04D03_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  int i;
  int *frame;
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  GMK_WORK* work = GMK_TMP_WK_GetWork(fieldmap, R04D03_TMP_ASSIGN_ID);
  
  work->Frame[0]++;
  frame = &work->Frame[0];
  { // �g���[���[1(�O)
    GFL_G3D_OBJSTATUS* status;
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_1_HEAD );
    status->trans.x = TRAINER_LEFT_X + TRAINER_SPEED * (*frame);
    if ( status->trans.x >= TRAINER_RIGHT_X) (*frame) = 0;
  }
  { // �g���[���[1(��)
    GFL_G3D_OBJSTATUS* status;
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_1_TAIL );
    status->trans.x = TRAINER_TAIL_LEFT_X + TRAINER_SPEED * (*frame);
    if ( status->trans.x >= TRAINER_RIGHT_X) (*frame) = 0;
  }

  work->Frame[1]++;
  frame = &work->Frame[1];
  { // �g���[���[2(�O)
    GFL_G3D_OBJSTATUS* status;
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_2_HEAD );
    status->trans.x = TRAINER_RIGHT_X - TRAINER_SPEED * (*frame);
    if ( status->trans.x <= TRAINER_LEFT_X) (*frame) = 0;
  }
  { // �g���[���[2(��)
    GFL_G3D_OBJSTATUS* status;
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_2_TAIL );
    status->trans.x = TRAINER_TAIL_RIGHT_X - TRAINER_SPEED * (*frame);
    if ( status->trans.x <= TRAINER_LEFT_X) (*frame) = 0;
  }
/**  
  u32*         gmk_save = (u32*)GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_H01 );
  GMK_WORK*         work = (H01WORK*)gmk_save[0]; // gmk_save[0]�̓M�~�b�N�Ǘ����[�N�̃A�h���X
*/
/**
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
*/  
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�Ǘ����[�N������������
 *
 * @param work     �������Ώۃ��[�N
 * @param fieldmap �ˑ�����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
static void InitWork( GMK_WORK* work, FIELDMAP_WORK* fieldmap )
{
  int i;
  HEAPID                heapID = FIELDMAP_GetHeapID( fieldmap );
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // �I�u�W�F�N�g���쐬
  { // �g���[���[1(�O)
    GFL_G3D_OBJSTATUS* status;
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_1_HEAD );
    status->trans.x = TRAINER_LEFT_X;
    status->trans.y = TRAINER_Y;
    status->trans.z = TRAINER1_Z;
    //90����]
    {
      fx32 sin = FX_SinIdx(0x4000);
      fx32 cos = FX_CosIdx(0x4000);
      MTX_RotY33( &status->rotate, sin, cos );
    }
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_1_HEAD, TRUE);
  }  
  { // �g���[���[1(��)
    GFL_G3D_OBJSTATUS* status;
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_1_TAIL );
    status->trans.x = TRAINER_TAIL_LEFT_X;
    status->trans.y = TRAINER_Y;
    status->trans.z = TRAINER1_Z;
    //90����]
    {
      fx32 sin = FX_SinIdx(0x4000);
      fx32 cos = FX_CosIdx(0x4000);
      MTX_RotY33( &status->rotate, sin, cos );
    }
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_1_TAIL, TRUE);
  }
  { // �g���[���[2(�O)
    GFL_G3D_OBJSTATUS* status;
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_2_HEAD );
    status->trans.x = TRAINER_RIGHT_X;
    status->trans.y = TRAINER_Y;
    status->trans.z = TRAINER2_Z;
    //270����]
    {
      fx32 sin = FX_SinIdx(0x4000*3);
      fx32 cos = FX_CosIdx(0x4000*3);
      MTX_RotY33( &status->rotate, sin, cos );
    }
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_2_HEAD, TRUE);
  }  
  { // �g���[���[2(��)
    GFL_G3D_OBJSTATUS* status;
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_2_TAIL );
    status->trans.x = TRAINER_TAIL_RIGHT_X;
    status->trans.y = TRAINER_Y;
    status->trans.z = TRAINER2_Z;
    //270����]
    {
      fx32 sin = FX_SinIdx(0x4000*3);
      fx32 cos = FX_CosIdx(0x4000*3);
      MTX_RotY33( &status->rotate, sin, cos );
    }
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_2_TAIL, TRUE);
  }
#if 0  

  // ����ҋ@�J�E���^
  for( i=0; i<SOBJ_NUM; i++ ){ work->wait[i] = 0; }

  // �ҋ@����
  LoadWaitTime( work );
  // ���f�[�^
  LoadWindData( work );
#endif  
}

