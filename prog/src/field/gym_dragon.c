//======================================================================
/**
 * @file  gym_dragon.c
 * @brief  �h���S���W��
 * @author  Saito
 * @date  09.12.04
 */
//======================================================================
#include "fieldmap.h"
#include "gym_dragon_sv.h"
#include "gym_dragon.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/fieldmap/gym_dragon.naix"
#include "system/main.h"    //for HEAPID_FIELDMAP

//#include "../../../resource/fldmapdata/gimmick/gym_dragon/gym_dragon_local_def.h"

//#include "sound/pm_sndsys.h"
//#include "gym_dragon_se_def.h"

//#include "field/fieldmap_proc.h"    //for FLDMAP_CTRLTYPE
//#include "fieldmap_ctrl_hybrid.h" //for FIELDMAP_CTRL_HYBRID

#define GYM_DRAGON_UNIT_IDX (0)
#define GYM_DRAGON_TMP_ASSIGN_ID  (1)

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)

#define FLOOR_VISIBLE_HEIGHT    (5*FIELD_CONST_GRID_FX32_SIZE)


#define DRAGON1_X (10*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define DRAGON1_Y (0*FIELD_CONST_GRID_FX32_SIZE)
#define DRAGON1_Z (23*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define DRAGON2_X (29*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define DRAGON2_Y (0*FIELD_CONST_GRID_FX32_SIZE)
#define DRAGON2_Z (16*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define DRAGON3_X (18*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define DRAGON3_Y (6*FIELD_CONST_GRID_FX32_SIZE)
#define DRAGON3_Z (22*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define FLOOR_X (32*FIELD_CONST_GRID_FX32_SIZE)
#define FLOOR_Y (0*FIELD_CONST_GRID_FX32_SIZE)
#define FLOOR_Z (32*FIELD_CONST_GRID_FX32_SIZE)

static const VecFx32 DragonPos[DRAGON_NUM_MAX] = {
  { DRAGON1_X, DRAGON1_Y, DRAGON1_Z },
  { DRAGON2_X, DRAGON2_Y, DRAGON2_Z },
  { DRAGON3_X, DRAGON3_Y, DRAGON3_Z },
};

static const u16 DragonRad[DRAGON_NUM_MAX] = {
  0x8000,
  0,
  0
};


//�W���������̈ꎞ���[�N
typedef struct GYM_DRAGON_TMP_tag
{
  int dummy;
}GYM_DRAGON_TMP;

//���\�[�X�̕��я���
enum {
  RES_ID_HEAD_MDL = 0,
  RES_ID_ARM_MDL,
  RES_ID_FLOOR_MDL,
};

//�n�a�i�C���f�b�N�X
enum {
  OBJ_HEAD_1 = 0,
  OBJ_ARM_1,
  OBJ_HEAD_2,
  OBJ_ARM_2,
  OBJ_HEAD_3,
  OBJ_ARM_3,
  OBJ_FLOOR,
};



//--���\�[�X�֘A--
//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_ue_migi_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_DRAGON, NARC_gym_dragon_arm_01_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_DRAGON, NARC_gym_dragon_m_gym0802f_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
};

//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
  //��
	{
		RES_ID_HEAD_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_HEAD_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,//NELEMS(g3Dutil_anmTbl_wall2),	//�A�j�����\�[�X��
	},
  //�r
	{
		RES_ID_ARM_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_ARM_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,//NELEMS(g3Dutil_anmTbl_wall1),	//�A�j�����\�[�X��
	},

  //��
	{
		RES_ID_HEAD_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_HEAD_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,//NELEMS(g3Dutil_anmTbl_wall2),	//�A�j�����\�[�X��
	},
  //�r
	{
		RES_ID_ARM_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_ARM_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,//NELEMS(g3Dutil_anmTbl_wall1),	//�A�j�����\�[�X��
	},

  //��
	{
		RES_ID_HEAD_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_HEAD_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,//NELEMS(g3Dutil_anmTbl_wall2),	//�A�j�����\�[�X��
	},
  //�r
	{
		RES_ID_ARM_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_ARM_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,//NELEMS(g3Dutil_anmTbl_wall1),	//�A�j�����\�[�X��
	},

  //��
	{
		RES_ID_FLOOR_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_FLOOR_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},


};

static const GFL_G3D_UTIL_SETUP Setup = {
  g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};


//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_DRAGON_Setup(FIELDMAP_WORK *fieldWork)
{
  int i;
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  GYM_DRAGON_TMP *tmp;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
  }

  //�ėp���[�N�m��
  tmp = GMK_TMP_WK_AllocWork
      (fieldWork, GYM_DRAGON_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_DRAGON_TMP));

  //�K�v�ȃ��\�[�X�̗p��
  FLD_EXP_OBJ_AddUnit(ptr, &Setup, GYM_DRAGON_UNIT_IDX );

  //���W�Z�b�g
  for (i=0;i<DRAGON_NUM_MAX;i++)
  {
    int j;
    int idx;
    int rad = 0;
    GFL_G3D_OBJSTATUS *status;

    rad = DragonRad[i];
    //��
    idx = OBJ_HEAD_1 + (i*2);
    status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_DRAGON_UNIT_IDX, idx);
    status->trans = DragonPos[i];
    MTX_RotY33(&status->rotate, FX_SinIdx(rad), FX_CosIdx(rad));
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_DRAGON_UNIT_IDX, idx, TRUE);
/**    
    for (j=0;j<WALL_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ICE_UNIT_IDX, idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      //�A�j����~
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    }
*/
    //�r
    idx = OBJ_ARM_1 + (i*2);
    status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_DRAGON_UNIT_IDX, idx);
    status->trans = DragonPos[i];
    MTX_RotY33(&status->rotate, FX_SinIdx(rad), FX_CosIdx(rad));
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_DRAGON_UNIT_IDX, idx, TRUE);
/**    
    for (j=0;j<WALL_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ICE_UNIT_IDX, idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      //�A�j����~
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    }
*/    
//    SetupWallSwAnm(ptr, gmk_sv_work->WallMoved[i], i, OBJ_KIND_WALL);
//    SetupWallSwAnm(ptr, gmk_sv_work->WallMoved[i], i, OBJ_KIND_SW);
  }

  //�����W�Z�b�g
  {
    GFL_G3D_OBJSTATUS *status;
    int idx = OBJ_FLOOR;
    status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_DRAGON_UNIT_IDX, idx);
    status->trans.x = FLOOR_X;
    status->trans.y = FLOOR_Y;
    status->trans.z = FLOOR_Z;
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_DRAGON_UNIT_IDX, idx, TRUE);
  }

  //�Z�b�g�A�b�v��2�K�\������
  {
    VecFx32 pos;
    BOOL vanish;
    FIELD_PLAYER *fld_player;
    fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    FIELD_PLAYER_GetPos( fld_player, &pos );
    if (pos.y >= FLOOR_VISIBLE_HEIGHT) vanish = FALSE;     //2�K�̏��Ɨ���\��
    else vanish = TRUE;       //2�K�̏��Ɨ����\��

    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_FLOOR, vanish );
    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_HEAD_3, vanish );
    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_ARM_3, vanish );
  }
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_DRAGON_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_DRAGON_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);

  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, GYM_DRAGON_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_DRAGON_Move(FIELDMAP_WORK *fieldWork)
{
  int i;
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_DRAGON_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
  }

  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );

  //���@�̍������Ď�
  {
    VecFx32 pos;
    BOOL vanish;
    FIELD_PLAYER *fld_player;
    fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    FIELD_PLAYER_GetPos( fld_player, &pos );
    if (pos.y >= FLOOR_VISIBLE_HEIGHT) vanish = FALSE;     //2�K�̏��Ɨ���\��
    else vanish = TRUE;       //2�K�̏��Ɨ����\��

    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_FLOOR, vanish );
    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_HEAD_3, vanish );
    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_ARM_3, vanish );
  }
}

