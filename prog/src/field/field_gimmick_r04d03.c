//======================================================================
/**
 * @file  field_gimmick_r04d03.c
 * @brief  4�ԓ��H+���]�[�g�f�U�[�g �M�~�b�N
 * @author  Saito
 */
//======================================================================

#include <gflib.h>
#include "fieldmap.h" 
#include "field_gimmick_r04d03.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "sound/pm_sndsys.h"
#include "system/gfl_use.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/arc_def.h"
#include "arc/h01.naix"
#include "gmk_tmp_wk.h"

#include "field_gimmick_r04d03_se_def.h"

#include "../../../resource/fldmapdata/gimmick/r04d03/gimmick_se_range_def.h"

#define EXPOBJ_UNIT_IDX (0)
#define ARCID (ARCID_H01_GIMMICK) // �M�~�b�N�f�[�^�̃A�[�J�C�uID
#define R04D03_TMP_ASSIGN_ID  (1)

#define TRAILER_TAIL_OFS (6*FIELD_CONST_GRID_FX32_SIZE)
#define TRAILER_Y (FIELD_CONST_GRID_FX32_SIZE*5)
#define TRAILER_RIGHT_X (454*FIELD_CONST_GRID_FX32_SIZE)
#define TRAILER_LEFT_X (375*FIELD_CONST_GRID_FX32_SIZE)
#define TRAILER_TAIL_RIGHT_X  (TRAILER_RIGHT_X+TRAILER_TAIL_OFS)
#define TRAILER_TAIL_LEFT_X   (TRAILER_LEFT_X-TRAILER_TAIL_OFS)


#define TRAILER1_Z ((526+2)*FIELD_CONST_GRID_FX32_SIZE)
#define TRAILER2_Z ((521+2)*FIELD_CONST_GRID_FX32_SIZE)


#define TRAILER_SPEED (FIELD_CONST_GRID_FX32_SIZE)

#define TRAILER_MAX (2)

#define SE_BAND_Z_MIN ( SE_BAND_Z_MIN_GRID * FIELD_CONST_GRID_FX32_SIZE )   //SE�̕������镝�y�ŏ��l
#define SE_BAND_Z_MAX ( SE_BAND_Z_MAX_GRID * FIELD_CONST_GRID_FX32_SIZE )   //SE�̕������镝�y�ő�l
#define SE_RANGE_X ( SE_RANGE_X_GRID * FIELD_CONST_GRID_FX32_SIZE )      //SE�̕������鋗��



//==========================================================================================
// ���M�~�b�N���[�N
//==========================================================================================
typedef struct GNK_OBJ_tag
{
  int Frame;
  int Wait;
  BOOL SeFlg;
}GMK_OBJ;

typedef struct
{ 
  int Frame[TRAILER_MAX];
  GMK_OBJ GmkObj[TRAILER_MAX];
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
  OBJ_TRAILER_1_HEAD,  // �g���[���[1(�O) ������E
  OBJ_TRAILER_1_TAIL,  // �g���[���[1(��) ������E
  OBJ_TRAILER_2_HEAD,  // �g���[���[2(�O) �E���獶
  OBJ_TRAILER_2_TAIL,  // �g���[���[2(��) �E���獶
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
  // �Z�[�u
  SaveGimmick( work, fieldmap );

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
  VecFx32 pos;
  fx32 tr1_x, tr2_x;

  GAMESYS_WORK*    gsys = FIELDMAP_GetGameSysWork( fieldmap );
  GAMEDATA*       gdata = GAMESYSTEM_GetGameData( gsys );
  GIMMICKWORK*  gmkwork = GAMEDATA_GetGimmickWork(gdata);
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  GMK_WORK* work = GMK_TMP_WK_GetWork(fieldmap, R04D03_TMP_ASSIGN_ID);
  GMK_OBJ *gmkobj;

  gmkobj = &work->GmkObj[0];
  gmkobj->Frame++;
  frame = &gmkobj->Frame;
  { // �g���[���[1(�O)
    GFL_G3D_OBJSTATUS* status;
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_1_HEAD );
    status->trans.x = TRAILER_LEFT_X + TRAILER_SPEED * (*frame);
    if ( status->trans.x >= TRAILER_RIGHT_X)
    {
      (*frame) = 0;
      work->GmkObj[0].SeFlg = FALSE;
    }
    tr1_x = status->trans.x;
  }
  { // �g���[���[1(��)
    GFL_G3D_OBJSTATUS* status;
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_1_TAIL );
    status->trans.x = TRAILER_TAIL_LEFT_X + TRAILER_SPEED * (*frame);
    if ( status->trans.x >= TRAILER_RIGHT_X) (*frame) = 0;
  }

  gmkobj = &work->GmkObj[1];
  gmkobj->Frame++;
  frame = &gmkobj->Frame;
  { // �g���[���[2(�O)
    GFL_G3D_OBJSTATUS* status;
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_2_HEAD );
    status->trans.x = TRAILER_RIGHT_X - TRAILER_SPEED * (*frame);
    if ( status->trans.x <= TRAILER_LEFT_X)
    {
      (*frame) = 0;
      work->GmkObj[1].SeFlg = FALSE;
    }
    tr2_x = status->trans.x;
  }
  { // �g���[���[2(��)
    GFL_G3D_OBJSTATUS* status;
    status = FLD_EXP_OBJ_GetUnitObjStatus( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_2_TAIL );
    status->trans.x = TRAILER_TAIL_RIGHT_X - TRAILER_SPEED * (*frame);
    if ( status->trans.x <= TRAILER_LEFT_X) (*frame) = 0;
  }

  //���@�̈ʒu���\���擾
  {
    FIELD_PLAYER *fld_player;
    fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
    FIELD_PLAYER_GetPos( fld_player, &pos );
  }

  //SE��炷�y�o���h���Ɏ��@�����邩�H
  if ( (SE_BAND_Z_MIN<=pos.z)&&(pos.z<=SE_BAND_Z_MAX) )      //����ꍇ
  {
    SEPLAYER_ID p_id;
    fx32 diff;
    FIELD_SOUND *fs = GAMEDATA_GetFieldSound( gdata );
    diff = pos.x - tr1_x;
    //���ڂ̃g���[���[�����@�����E�ɂ��āA�Ȃ����A�r�d�Đ������ɂ��Ă܂��r�d�Ȃ炵�ĂȂ����H
    if( (0 <= diff)&&(diff <= SE_RANGE_X)&&(!work->GmkObj[0].SeFlg) )   //YES
    {
      //�v���[���[�󂫃`�F�b�N
      p_id = PMSND_GetSE_DefaultPlayerID( FLD_GMK_R04D03_LR_SE );
      if ( !PMSND_CheckPlaySE_byPlayerID( p_id ) )
      {
        work->GmkObj[0].SeFlg = TRUE;
        FSND_PlayEnvSE( fs, FLD_GMK_R04D03_LR_SE );    //������E
      }
    }
    diff = tr2_x - pos.x;
    //���ڂ̃g���[���[�����@�������ɂ��āA�Ȃ����A�r�d�Đ������ɂ��Ă܂��r�d�Ȃ炵�ĂȂ����H
    if( (0 <= diff)&&(diff <= SE_RANGE_X)&&(!work->GmkObj[1].SeFlg) )   //YES
    {
      //�v���[���[�󂫃`�F�b�N
      p_id = PMSND_GetSE_DefaultPlayerID( FLD_GMK_R04D03_RL_SE );
      if ( !PMSND_CheckPlaySE_byPlayerID( p_id ) )
      {
        work->GmkObj[1].SeFlg = TRUE;
        FSND_PlayEnvSE( fs, FLD_GMK_R04D03_RL_SE );    //�E���獶
      }
    }
  }
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
    status->trans.x = TRAILER_LEFT_X;
    status->trans.y = TRAILER_Y;
    status->trans.z = TRAILER1_Z;
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
    status->trans.x = TRAILER_TAIL_LEFT_X;
    status->trans.y = TRAILER_Y;
    status->trans.z = TRAILER1_Z;
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
    status->trans.x = TRAILER_RIGHT_X;
    status->trans.y = TRAILER_Y;
    status->trans.z = TRAILER2_Z;
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
    status->trans.x = TRAILER_TAIL_RIGHT_X;
    status->trans.y = TRAILER_Y;
    status->trans.z = TRAILER2_Z;
    //270����]
    {
      fx32 sin = FX_SinIdx(0x4000*3);
      fx32 cos = FX_CosIdx(0x4000*3);
      MTX_RotY33( &status->rotate, sin, cos );
    }
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_TRAILER_2_TAIL, TRUE);
  }
}

