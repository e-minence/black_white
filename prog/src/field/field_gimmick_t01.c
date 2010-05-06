//======================================================================
/**
 * @file  field_gimmick_t01.c
 * @brief  �s01 �M�~�b�N
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "fieldmap.h" 
#include "field_gimmick_t01.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "system/gfl_use.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/arc_def.h"
#include "arc/fieldmap/t01_gmk.naix"
#include "field_gimmick_t01_sv.h"

#include "system/rtc_tool.h"  //for PM_RTC_GetTimeZone

#define EXPOBJ_UNIT_IDX (0)

#define BIRD_X  (FIELD_CONST_GRID_FX32_SIZE * 784)
#define BIRD_Y  (FX32_ONE * 0)
#define BIRD_Z  (FIELD_CONST_GRID_FX32_SIZE * 752)

#define BIRD_ANM_NUM  (2)

//==========================================================================================
// ��3D���\�[�X
//==========================================================================================
// ���\�[�X
typedef enum {
  RES_BIRD_NSBMD,   //���̃��f��
  RES_BIRD_NSBCA,   //���A�j��
  RES_BIRD_NSBTP,   //���A�j��
} RES_INDEX;

//���p
static const GFL_G3D_UTIL_RES res_table_morning[] = 
{
  { ARCID_T01_GMK, NARC_t01_gmk_t01_bird01_nsbmd, GFL_G3D_UTIL_RESARC },  // ���̃��f��
  { ARCID_T01_GMK, NARC_t01_gmk_t01_bird01_nsbca, GFL_G3D_UTIL_RESARC },  // ���̃A�j��
  { ARCID_T01_GMK, NARC_t01_gmk_t01_bird01_nsbtp, GFL_G3D_UTIL_RESARC },  // ���̃A�j��
};

//��p
static const GFL_G3D_UTIL_RES res_table_night[] = 
{
  { ARCID_T01_GMK, NARC_t01_gmk_t01_bird02_nsbmd, GFL_G3D_UTIL_RESARC },  // ���̃��f��
  { ARCID_T01_GMK, NARC_t01_gmk_t01_bird02_nsbca, GFL_G3D_UTIL_RESARC },  // ���̃A�j��
  { ARCID_T01_GMK, NARC_t01_gmk_t01_bird02_nsbtp, GFL_G3D_UTIL_RESARC },  // ���̃A�j��
};

// �I�u�W�F�N�g
typedef enum {
  OBJ_BIRD,  // ��
} OBJ_INDEX;

//3D�A�j��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTb[] = {
  { RES_BIRD_NSBCA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_BIRD_NSBTP,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};


static const GFL_G3D_UTIL_OBJ obj_table[] = 
{
  { RES_BIRD_NSBMD, 0, RES_BIRD_NSBMD, g3Dutil_anmTb, NELEMS(g3Dutil_anmTb) },  // ��
};

//���p
static const GFL_G3D_UTIL_SETUP SetupMorning = {
  res_table_morning,				//���\�[�X�e�[�u��
	NELEMS(res_table_morning),		//���\�[�X��
	obj_table,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(obj_table),		//�I�u�W�F�N�g��
};

//��p
static const GFL_G3D_UTIL_SETUP SetupNight = {
  res_table_night,				//���\�[�X�e�[�u��
	NELEMS(res_table_night),		//���\�[�X��
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
void T01_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
  int i;
  T01_SV_WORK *gmk_sv_work;
  FLD_EXP_OBJ_CNT_PTR exobj_cnt;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldmap ) );
  {
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_T01 );
  }
  exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  //���ԑтŕ���
  {
    int season, time_zone;
    season = GAMEDATA_GetSeasonID(gamedata);
    time_zone = PM_RTC_GetTimeZone(season);
    // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
    if ( (time_zone == TIMEZONE_NIGHT) || (time_zone == TIMEZONE_MIDNIGHT) )
    {
      FLD_EXP_OBJ_AddUnit( exobj_cnt, &SetupNight, EXPOBJ_UNIT_IDX );
    }
    else
    {
      FLD_EXP_OBJ_AddUnit( exobj_cnt, &SetupMorning, EXPOBJ_UNIT_IDX );
    }
  }

  {
    VecFx32 pos = { BIRD_X, BIRD_Y, BIRD_Z };
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(exobj_cnt, EXPOBJ_UNIT_IDX, 0);
    status->trans = pos;
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(exobj_cnt, EXPOBJ_UNIT_IDX, 0, TRUE);
    //�A�j���Đ�
    FLD_EXP_OBJ_ValidCntAnm(exobj_cnt, EXPOBJ_UNIT_IDX, 0, 0, TRUE);
    if ( gmk_sv_work->Move )
    {
      //�\��
      FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, 0, FALSE);
    }
    else
    {
      //��\��
      FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, 0, TRUE);
    }
  }

  //�A�j���̏�Ԃ�������
  for (i=0;i<BIRD_ANM_NUM;i++)
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BIRD, i);
    //1��Đ�
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    if ( gmk_sv_work->Move )
    {
      //�A�j���Đ�
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
      //�L��
      FLD_EXP_OBJ_ValidCntAnm(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BIRD, i, TRUE);
    }
    else
    {
      //�A�j����~
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
      //������
      FLD_EXP_OBJ_ValidCntAnm(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BIRD, i, FALSE);
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
void T01_GIMMICK_End( FIELDMAP_WORK* fieldmap )
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
void T01_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  T01_SV_WORK *gmk_sv_work;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldmap ) );
  GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_T01 );

  if ( gmk_sv_work->Move )
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    //�A�j���[�V�����Đ�
    FLD_EXP_OBJ_PlayAnime( exobj_cnt );

    //�C���f�b�N�X0�̃A�j���ŏI�����肷��
    anm = FLD_EXP_OBJ_GetAnmCnt( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BIRD, 0);
    //�I���`�F�b�N
    if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
    {
      //�n�a�i��\��
      FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, 0, TRUE);
      gmk_sv_work->Move = FALSE;    //�M�~�b�N��~
    }    
  }
}
