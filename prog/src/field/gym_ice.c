//======================================================================
/**
 * @file  gym_ice.c
 * @brief  �X�W��
 * @author  Saito
 * @date  09.11.26
 */
//======================================================================

#include "fieldmap.h"
#include "gym_ice_sv.h"
#include "gym_ice.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/fieldmap/gym_ice.naix"
#include "system/main.h"    //for HEAPID_FIELDMAP

#include "../../../resource/fldmapdata/gimmick/gym_ice/gym_ice_local_def.h"

//#include "sound/pm_sndsys.h"
//#include "gym_ice_se_def.h"

#define GYM_ICE_UNIT_IDX (0)
#define GYM_ICE_TMP_ASSIGN_ID  (1)

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)

#define WALL_ANM_NUM  (1)

#define WALL1_X (WALL1_GX*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define WALL2_X (WALL2_GX*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define WALL3_X (WALL3_GX*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define WALL1_Y (WALL1_GY*FIELD_CONST_GRID_FX32_SIZE)
#define WALL2_Y (WALL2_GY*FIELD_CONST_GRID_FX32_SIZE)
#define WALL3_Y (WALL3_GY*FIELD_CONST_GRID_FX32_SIZE)
#define WALL1_Z (WALL1_GZ*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define WALL2_Z (WALL2_GZ*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define WALL3_Z (WALL3_GZ*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define SW1_X (SW1_GX*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define SW2_X (SW2_GX*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define SW3_X (SW3_GX*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define SW1_Y (SW1_GY*FIELD_CONST_GRID_FX32_SIZE)
#define SW2_Y (SW2_GY*FIELD_CONST_GRID_FX32_SIZE)
#define SW3_Y (SW3_GY*FIELD_CONST_GRID_FX32_SIZE)
#define SW1_Z (SW1_GZ*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define SW2_Z (SW2_GZ*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define SW3_Z (SW3_GZ*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)


static const VecFx32 WallPos[WALL_NUM_MAX] = {
  { WALL1_X, WALL1_Y, WALL1_Z },
  { WALL2_X, WALL2_Y, WALL2_Z },
  { WALL3_X, WALL3_Y, WALL3_Z },
};

static const VecFx32 SwPos[WALL_NUM_MAX] = {
  { SW1_X, SW1_Y, SW1_Z },
  { SW2_X, SW2_Y, SW2_Z },
  { SW3_X, SW3_Y, SW3_Z },
};

//���\�[�X�̕��я���
enum {
  RES_ID_WALL1_MDL = 0,
  RES_ID_WALL2_MDL,
  RES_ID_SW_MDL,
  RES_ID_WALL1_ANM,
  RES_ID_WALL2_ANM,
  RES_ID_SW_ANM_A,
  RES_ID_SW_ANM_B,
};

//�n�a�i�C���f�b�N�X
enum {
  OBJ_WALL_1 = 0,
  OBJ_WALL_2,
  OBJ_WALL_3,
  OBJ_SW_1,
  OBJ_SW_2,
  OBJ_SW_3,
};

//�W���������̈ꎞ���[�N
typedef struct GYM_ICE_TMP_tag
{
  int dummy;
}GYM_ICE_TMP;

//--���\�[�X�֘A--
//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_GYM_ICE, NARC_gym_ice_ice_karakuri_01_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_ICE, NARC_gym_ice_ice_karakuri_02_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_ICE, NARC_gym_ice_swich_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD

  { ARCID_GYM_ICE, NARC_gym_ice_ice_karakuri_01_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_ICE, NARC_gym_ice_ice_karakuri_02_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_ICE, NARC_gym_ice_swich_br_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_ICE, NARC_gym_ice_swich_br_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
};

//3D�A�j���@���t�g
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_wall1[] = {
	{ RES_ID_WALL1_ANM,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_wall2[] = {
	{ RES_ID_WALL2_ANM,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_sw[] = {
	{ RES_ID_SW_ANM_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_SW_ANM_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};


//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
  //��1
	{
		RES_ID_WALL2_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL2_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall2,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall2),	//�A�j�����\�[�X��
	},
  //��2
	{
		RES_ID_WALL1_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL1_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall1,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall1),	//�A�j�����\�[�X��
	},
  //��3
	{
		RES_ID_WALL2_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL2_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall2,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall2),	//�A�j�����\�[�X��
	},
  //�X�C�b�`1
	{
		RES_ID_SW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw),	//�A�j�����\�[�X��
	},
  //�X�C�b�`2
	{
		RES_ID_SW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw),	//�A�j�����\�[�X��
	},
  //�X�C�b�`3
	{
		RES_ID_SW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw),	//�A�j�����\�[�X��
	},
};

static const GFL_G3D_UTIL_SETUP Setup = {
  g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};


static void SetupWallSwAnm(GYM_ICE_SV_WORK *gmk_sv_work, FLD_EXP_OBJ_CNT_PTR ptr, const int inWallIdx);

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_ICE_Setup(FIELDMAP_WORK *fieldWork)
{
  int i;
  GYM_ICE_SV_WORK *gmk_sv_work;
  GYM_ICE_TMP *tmp;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ICE );
  }

  //�ėp���[�N�m��
  tmp = GMK_TMP_WK_AllocWork
      (fieldWork, GYM_ICE_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_ICE_TMP));

  //�K�v�ȃ��\�[�X�̗p��
  FLD_EXP_OBJ_AddUnit(ptr, &Setup, GYM_ICE_UNIT_IDX );

  //���W�Z�b�g
  for (i=0;i<WALL_NUM_MAX;i++)
  {
    int j;
    int idx;
    GFL_G3D_OBJSTATUS *status;
    //��
    idx = OBJ_WALL_1 + i;
    status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ICE_UNIT_IDX, idx);
    status->trans = WallPos[i];
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_ICE_UNIT_IDX, idx, TRUE);
    for (j=0;j<WALL_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ICE_UNIT_IDX, idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      //�A�j����~
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    }

    //�X�C�b�`
    idx = OBJ_SW_1 + i;
    status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ICE_UNIT_IDX, idx);
    status->trans = SwPos[i];
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_ICE_UNIT_IDX, idx, TRUE);
    for (j=0;j<WALL_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ICE_UNIT_IDX, idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      //�A�j����~
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    }
    
    SetupWallSwAnm(gmk_sv_work, ptr, i);
  }
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_ICE_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ICE_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ICE_TMP_ASSIGN_ID);

  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GYM_ICE_TMP_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, GYM_ICE_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_ICE_Move(FIELDMAP_WORK *fieldWork)
{
  int i;
  GYM_ICE_SV_WORK *gmk_sv_work;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ICE_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ICE_TMP_ASSIGN_ID);
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ICE );
  }
  
  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );
}


//--------------------------------------------------------------
/**
 * �ǁA�X�C�b�`�A�j������
 * @param   gmk_sv_work   �M�~�b�N�Z�[�u���[�N
 * @param   ptr       �g���n�a�i�R���g���[���|�C���^
 * @param	  inWallIdx �ǃC���f�b�N�X
 * @return  none
 */
//--------------------------------------------------------------
static void SetupWallSwAnm(GYM_ICE_SV_WORK *gmk_sv_work, FLD_EXP_OBJ_CNT_PTR ptr, const int inWallIdx)
{
}


