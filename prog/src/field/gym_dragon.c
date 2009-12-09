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

#define DRAGON_PARTS_SET  (5)   //���E���r�E�E�r�E���{�^���E�E�{�^����4��
#define ANM_PLAY_MAX  (1)

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

//��̏�ԕʂ̓K�p�A�j���e�[�u��
static const int NeckAnmTbl[HEAD_DIR_MAX][HEAD_DIR_MAX] =
{
  //UP,DOWN,LEFT,RIGHT
  {-1, -1, 0, 1},     //UP
  {-1, -1, 2, 3},     //DOWN
  {4 ,  5,-1,-1},     //LEFT
  {6 ,  7,-1,-1},     //RIGHT
};

//�r�̏�ԕʂ̓K�p�A�j���e�[�u��
static const int ArmAnmTbl[3/*DRA_ARM_MAX*/][ARM_DIR_MAX] =
{
  //UP,DOWN
  {0, 1},     //LEFT ARM
  {2, 3},     //RIGHT ARM
  {0,0},
};

typedef struct ANM_PLAY_WORK_tag
{
  u8 ObjIdx;
  u8 AnmNum;
  u8 AnmOfs[ANM_PLAY_MAX];

  u8 OldAnmNum;
  u8 OldAnmOfs[ANM_PLAY_MAX];
}ANM_PLAY_WORK;


//�W���������̈ꎞ���[�N
typedef struct GYM_DRAGON_TMP_tag
{
  int TrgtHead;
  DRA_ARM TrgtArm;
  DRAGON_WORK *DraWk;
}GYM_DRAGON_TMP;

//���\�[�X�̕��я���
enum {
  RES_ID_HEAD_MDL = 0,
  RES_ID_L_ARM_MDL,
  RES_ID_R_ARM_MDL,
  RES_ID_FLOOR_MDL,
  RES_ID_BUTTON_MDL,
  RES_ID_HEAD_ANM1,
  RES_ID_HEAD_ANM2,
  RES_ID_HEAD_ANM3,
  RES_ID_HEAD_ANM4,
  RES_ID_HEAD_ANM5,
  RES_ID_HEAD_ANM6,
  RES_ID_HEAD_ANM7,
  RES_ID_HEAD_ANM8,
  RES_ID_ARM_ANM1,
  RES_ID_ARM_ANM2,
  RES_ID_ARM_ANM3,
  RES_ID_ARM_ANM4,
  RES_ID_BUTTON_ANM,
};

//�n�a�i�C���f�b�N�X
enum {
  OBJ_HEAD_1 = 0,
  OBJ_L_ARM_1,
  OBJ_R_ARM_1,
  OBJ_BUTTON_L_1,
  OBJ_BUTTON_R_1,
  OBJ_HEAD_2,
  OBJ_L_ARM_2,
  OBJ_R_ARM_2,
  OBJ_BUTTON_L_2,
  OBJ_BUTTON_R_2,
  OBJ_HEAD_3,
  OBJ_L_ARM_3,
  OBJ_R_ARM_3,
  OBJ_BUTTON_L_3,
  OBJ_BUTTON_R_3,
  OBJ_FLOOR,
};



//--���\�[�X�֘A--
//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_dragon_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_DRAGON, NARC_gym_dragon_in31_l_arm_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_DRAGON, NARC_gym_dragon_in31_r_arm_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_DRAGON, NARC_gym_dragon_m_gym0802f_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_DRAGON, NARC_gym_dragon_in31_botton_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_DRAGON, NARC_gym_dragon_in31_ue_rneck_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_ue_rneck2_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_ue_lneck_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_ue_lneck2_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_si_rneck_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_si_rneck2_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_si_lneck_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_si_lneck2_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_r_ue_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_r_sita_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_l_ue_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_l_sita_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_botton_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
};

//3D�A�j���@��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_head[] = {
  { RES_ID_HEAD_ANM1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM3,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM4,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM5,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM6,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM7,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM8,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@���r
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_l_arm[] = {
  { RES_ID_ARM_ANM1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_ARM_ANM2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};
//3D�A�j���@�E�r
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_r_arm[] = {
  { RES_ID_ARM_ANM3,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_ARM_ANM4,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};


//3D�A�j���@�{�^��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_btn[] = {
  { RES_ID_BUTTON_ANM,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};


//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
  //��
	{
		RES_ID_HEAD_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_HEAD_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_head,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_head),	//�A�j�����\�[�X��
	},
  //���r
	{
		RES_ID_L_ARM_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_L_ARM_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_l_arm,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_l_arm),	//�A�j�����\�[�X��
	},
  //�E�r
	{
		RES_ID_R_ARM_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_R_ARM_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_r_arm,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_r_arm),	//�A�j�����\�[�X��
	},
  //�{�^��
	{
		RES_ID_BUTTON_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BUTTON_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //�{�^��
	{
		RES_ID_BUTTON_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BUTTON_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},

  //��
	{
		RES_ID_HEAD_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_HEAD_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_head,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_head),	//�A�j�����\�[�X��
	},
  //���r
	{
		RES_ID_L_ARM_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_L_ARM_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_l_arm,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_l_arm),	//�A�j�����\�[�X��
	},
  //�E�r
	{
		RES_ID_R_ARM_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_R_ARM_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_r_arm,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_r_arm),	//�A�j�����\�[�X��
	},
  //�{�^��
	{
		RES_ID_BUTTON_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BUTTON_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //�{�^��
	{
		RES_ID_BUTTON_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BUTTON_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},

  //��
	{
		RES_ID_HEAD_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_HEAD_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_head,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_head),	//�A�j�����\�[�X��
	},
  //���r
	{
		RES_ID_L_ARM_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_L_ARM_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_l_arm,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_l_arm),	//�A�j�����\�[�X��
	},
  //�E�r
	{
		RES_ID_R_ARM_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_R_ARM_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_r_arm,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_r_arm),	//�A�j�����\�[�X��
	},
  //�{�^��
	{
		RES_ID_BUTTON_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BUTTON_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //�{�^��
	{
		RES_ID_BUTTON_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BUTTON_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
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

static GMEVENT_RESULT AnmEvt( GMEVENT* event, int* seq, void* work );
static  HEAD_DIR GetHeadDirByArm(DRAGON_WORK *wk);
static int GetHeadAnmIdx(DRAGON_WORK *wk, const HEAD_DIR inNextDir);


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
    idx = OBJ_HEAD_1 + (i*DRAGON_PARTS_SET);
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
    //���r
    idx = OBJ_L_ARM_1 + (i*DRAGON_PARTS_SET);
    status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_DRAGON_UNIT_IDX, idx);
    status->trans = DragonPos[i];
    MTX_RotY33(&status->rotate, FX_SinIdx(rad), FX_CosIdx(rad));
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_DRAGON_UNIT_IDX, idx, TRUE);
    //�E�r
    idx = OBJ_R_ARM_1 + (i*DRAGON_PARTS_SET);
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
    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_L_ARM_3, vanish );
    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_R_ARM_3, vanish );
    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_BUTTON_L_3, vanish );
    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_BUTTON_R_3, vanish );
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
    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_L_ARM_3, vanish );
    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_R_ARM_3, vanish );
    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_BUTTON_L_3, vanish );
    FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_BUTTON_R_3, vanish );
  }
}

//�h���S���̘r�̏�Ԃ��玟�̎�̈ʒu�����肷��
static  HEAD_DIR GetHeadDirByArm(DRAGON_WORK *wk)
{
  HEAD_DIR dir;
  if (wk->ArmDir[DRA_ARM_LEFT] == ARM_DIR_UP)
  {
    if (wk->ArmDir[DRA_ARM_RIGHT] == ARM_DIR_UP) dir = HEAD_DIR_UP;   //���r:��@�E�r:��  �ˁ@��:��
    else dir = HEAD_DIR_RIGHT;    //���r:��@�E�r:��  �ˁ@��:�E
  }
  else{
    if (wk->ArmDir[DRA_ARM_RIGHT] == ARM_DIR_UP) dir = HEAD_DIR_LEFT; //���r:���@�E�r:��  �ˁ@��:��
    else dir = HEAD_DIR_DOWN;     //���r:���@�E�r:��  �ˁ@��:��
  }
  return dir;
}

//���݂̎�̈ʒu�Ǝ��̎�̈ʒu����A�Đ������̃A�j���[�V���������肷��
static int GetHeadAnmIdx(DRAGON_WORK *wk, const HEAD_DIR inNextDir)
{
  return NeckAnmTbl[wk->HeadDir][inNextDir];
}

//�h���S���̘r�̏�Ԃ��玟�̘r�̈ʒu�����肷��
static  ARM_DIR GetArmDir(DRAGON_WORK *wk, const DRA_ARM inArm)
{
  ARM_DIR dir, now_dir;

  now_dir = wk->ArmDir[inArm];
  if (now_dir == ARM_DIR_UP) dir = ARM_DIR_DOWN;
  else dir = ARM_DIR_UP;
  return dir;
}

//�w��̘r�̈ړ�������A�j���̃C���f�b�N�X��Ԃ�
static int GetArmAnmIdx(const ARM_DIR inNextDir, const DRA_ARM inArm)
{
  return ArmAnmTbl[inArm][inNextDir];
}


//���t���[
//�@�X�C�b�`�����A�j�������r�̌��݈ʒu����A�j�������聄���r�����A�j�������r�̌��݈ʒu�X�V
//�����r�̏�Ԃ��玟�̎�̈ʒu�����聄�����݂̎�̈ʒu����A�j�������聄���A�j���Đ�
//������̏��X�V�������݂̎�̏�Ԃ��烌�[���i���֎~���Z�b�g
//�������������̂��A2�K�̗��ŁA���ʏ���������ꍇ�Ɖ����������ꍇ�Ƀ��[���Z�b�g������������
//�����i���֎~�̏�������������̃��[���Ɉ����p��


//--------------------------------------------------------------
/**
 * �M�~�b�N�A�j���C�x���g
 * @param	  event   �C�x���g�|�C���^
 * @param   seq     �V�[�P���T
 * @param   work    ���[�N�|�C���^
 * @return  GMEVENT_RESULT  �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT AnmMoveEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_DRAGON_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);

  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
  }

  switch(*seq){
  case 0:
    {
      GMEVENT * call_event;
      //�X�C�b�`�̂n�a�i�ƃA�j�����Z�b�g
      ;
      call_event = GMEVENT_Create(gsys, NULL, AnmEvt, 0);
      //�X�C�b�`�����A�j���C�x���g�R�[��
      GMEVENT_CallEvent(event, call_event);
    }
    (*seq)++;
    break;
  case 1:
    {
      GMEVENT * call_event;
      int anm_idx;
      int obj_idx;
      //���ݏ󋵂��瓮�����r�A�j��������
      if ( tmp->TrgtArm == DRA_ARM_LEFT )
      {
        anm_idx = 0;  //@todo
        obj_idx = OBJ_L_ARM_1;
      }
      else
      {
        anm_idx = 0;  //@todo
        obj_idx = OBJ_R_ARM_1;
      }

      //�r�̂n�a�i�ƃA�j�����Z�b�g
      obj_idx += (tmp->TrgtHead*DRAGON_PARTS_SET);

      call_event = GMEVENT_Create(gsys, NULL, AnmEvt, 0);
      //�r�A�j���C�x���g�R�[��
      GMEVENT_CallEvent(event, call_event);
      //�r�̏����X�V
      tmp->DraWk->ArmDir[tmp->TrgtArm] = ARM_DIR_UP;  //@todo
    }
    (*seq)++;
    break;
  case 2:
    {
      GMEVENT * call_event;
      HEAD_DIR next_dir;
      int anm_idx;
      int obj_idx;
      
      //�r�̏�Ԃ���A��A�j��������
      next_dir = GetHeadDirByArm(tmp->DraWk);
      anm_idx = GetHeadAnmIdx(tmp->DraWk, next_dir);
      //��̂n�a�i�ƃA�j�����Z�b�g
      obj_idx = OBJ_HEAD_1+(tmp->TrgtHead*DRAGON_PARTS_SET);
      
      call_event = GMEVENT_Create(gsys, NULL, AnmEvt, 0);
      //��A�j���C�x���g�R�[��
      GMEVENT_CallEvent(event, call_event);
      //��̏��X�V
      tmp->DraWk->HeadDir = next_dir;
    }
    (*seq)++;
    break;
  case 3:
    {
      //���[���`�F���W����
      ;
    }
    (*seq)++;
    break;
  case 4:
    //�I��
    return GMEVENT_RES_FINISH;
  }
  
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �{�^���A�j���C�x���g
 * @param	  event   �C�x���g�|�C���^
 * @param   seq     �V�[�P���T
 * @param   work    ���[�N�|�C���^
 * @return  GMEVENT_RESULT  �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT AnmEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_DRAGON_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);


  ANM_PLAY_WORK *play_work;

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
  }

  switch(*seq){
  case 0:
    {
      int i;
      for (i=0;i<play_work->AnmNum;i++)
      {
        u8 anm_ofs = play_work->OldAnmOfs[i];
        u8 obj_idx = play_work->ObjIdx;
        //�K�p����Ă���ł��낤�A�j���𖳌����i�K�p���Ă��Ȃ��A�j���������t���O���Z�b�g���ɂ����j
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_DRAGON_UNIT_IDX, obj_idx, anm_ofs, FALSE);
      }

      for (i=0;i<play_work->AnmNum;i++)
      {
        u8 anm_ofs = play_work->AnmOfs[i];
        u8 obj_idx = play_work->ObjIdx;
        //�A�j�����J�n
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_DRAGON_UNIT_IDX, obj_idx, anm_ofs, TRUE);
        //���o��
        FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_DRAGON_UNIT_IDX, obj_idx, anm_ofs, 0 );
      }
    }
    (*seq)++;
    break;
  case 1:
    {
      //�A�j���I���҂�
      if(1) return GMEVENT_RES_FINISH;
    }
  }

  return GMEVENT_RES_CONTINUE;
}
