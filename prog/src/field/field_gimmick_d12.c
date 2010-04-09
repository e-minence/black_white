//======================================================================
/**
 * @file  field_gimmick_d12.c
 * @brief  �r�N�e�B�̓� �M�~�b�N
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "fieldmap.h" 
#include "field_gimmick_d12.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "system/gfl_use.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/arc_def.h"
#include "arc/fieldmap/gmk_d12.naix"
#include "field_gimmick_d12_sv.h"

#define EXPOBJ_UNIT_IDX (0)

#define OBJ_NUM (1)

#define SHIP_X  (FIELD_CONST_GRID_FX32_SIZE * 321 + (FIELD_CONST_GRID_FX32_SIZE/2))
#define SHIP_Y  (FX32_ONE * 8)
#define SHIP_Z  (FIELD_CONST_GRID_FX32_SIZE * 768 + (FIELD_CONST_GRID_FX32_SIZE/2))

//==========================================================================================
// ��3D���\�[�X
//==========================================================================================
// ���\�[�X
typedef enum {
  RES_SHIP_NSBMD,   //�D�̃��f��
  RES_SHIP_NSBTA,   //�D�A�j��
} RES_INDEX;

static const GFL_G3D_UTIL_RES res_table[] = 
{
  { ARCID_GMK_D12, NARC_gmk_d12_c03_ship_03_nsbmd, GFL_G3D_UTIL_RESARC },  // �D�̃��f��
  { ARCID_GMK_D12, NARC_gmk_d12_c03_ship_01_nsbta, GFL_G3D_UTIL_RESARC },  // �D�̃A�j��
};

// �I�u�W�F�N�g
typedef enum {
  OBJ_SHIP,  // �D
} OBJ_INDEX;

//3D�A�j��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTb[] = {
  { RES_SHIP_NSBTA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};


static const GFL_G3D_UTIL_OBJ obj_table[OBJ_NUM] = 
{
  { RES_SHIP_NSBMD, 0, RES_SHIP_NSBMD, g3Dutil_anmTb, NELEMS(g3Dutil_anmTb) },  // �D
};

static const GFL_G3D_UTIL_SETUP Setup = {
  res_table,				//���\�[�X�e�[�u��
	NELEMS(res_table),		//���\�[�X��
	obj_table,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(obj_table),		//�I�u�W�F�N�g��
};

//------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void D12_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
  D12_SV_WORK *gmk_sv_work;
  FLD_EXP_OBJ_CNT_PTR exobj_cnt;
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldmap ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_D12 );
  }

  exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
  FLD_EXP_OBJ_AddUnit( exobj_cnt, &Setup, EXPOBJ_UNIT_IDX );

  {
    VecFx32 pos = { SHIP_X, SHIP_Y, SHIP_Z };
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(exobj_cnt, EXPOBJ_UNIT_IDX, 0);
    status->trans = pos;
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(exobj_cnt, EXPOBJ_UNIT_IDX, 0, TRUE);
    //�A�j���Đ�
    FLD_EXP_OBJ_ValidCntAnm(exobj_cnt, EXPOBJ_UNIT_IDX, 0, 0, TRUE);
    if (gmk_sv_work->Vanish)
    {
      FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, 0, TRUE);
    }
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �I���֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void D12_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  //���j�b�g�j��
  FLD_EXP_OBJ_DelUnit( exobj_cnt, EXPOBJ_UNIT_IDX );  
}

//------------------------------------------------------------------------------------------
/**
 * @brief ����֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void D12_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( exobj_cnt );
}
