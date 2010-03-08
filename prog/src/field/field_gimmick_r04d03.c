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
//#include "gamesystem/iss_3ds_sys.h"
#include "savedata/gimmickwork.h"
//#include "sound/pm_sndsys.h"
#include "system/gfl_use.h"
//#include "sound_obj.h" 

#include "arc/arc_def.h"
#include "arc/h01.naix"

#define EXPOBJ_UNIT_IDX (0)
#define ARCID (ARCID_H01_GIMMICK) // �M�~�b�N�f�[�^�̃A�[�J�C�uID
//==========================================================================================
// ���M�~�b�N���[�N
//==========================================================================================
typedef struct
{ 
  HEAPID            heapID; // �g�p����q�[�vID
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

  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &setup, EXPOBJ_UNIT_IDX );
/**
  // �M�~�b�N�Ǘ����[�N���쐬
  work = (H01WORK*)GFL_HEAP_AllocMemory( heapID, sizeof(H01WORK) );

  // �M�~�b�N�Ǘ����[�N�������� 
  InitWork( work, fieldmap );

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
  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
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

