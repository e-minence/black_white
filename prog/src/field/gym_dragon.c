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
#include "script.h"     //for SCRIPT_CallScript
//#include "../../../resource/fldmapdata/gimmick/gym_dragon/gym_dragon_local_def.h"

#include "sound/pm_sndsys.h"
#include "gym_dragon_se_def.h"

//#include "field/fieldmap_proc.h"    //for FLDMAP_CTRLTYPE
//#include "fieldmap_ctrl_hybrid.h" //for FIELDMAP_CTRL_HYBRID
#include "../../../resource/fldmapdata/eventdata/define/total_header.h" //for OBJID
#include "rail_line/c08gym0101.h" //for RAIL

#define GYM_DRAGON_UNIT_IDX (0)
#define GYM_DRAGON_TMP_ASSIGN_ID  (1)

#define DRAGON_PARTS_SET  (5)   //���E���r�E�E�r�E���{�^���E�E�{�^����4��
#define ANM_PLAY_MAX  (1)
#define DRAGON_ANM_NUM  (8)
#define ARM_ANM_NUM  (2)
#define BTN_ANM_NUM  (1)

#define JUMP_COUNT  (8)
#define JUMP_MOVE_X  (FX32_ONE*2)

#define CHECK_RAIL_NUM  (6)

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)

#define FLOOR_VISIBLE_HEIGHT    (4*FIELD_CONST_GRID_FX32_SIZE)
#define FLOOR_VANISH_X          (16*FIELD_CONST_GRID_FX32_SIZE)
#define FLOOR_VANISH_Z          (30*FIELD_CONST_GRID_FX32_SIZE)


#define DRAGON1_X (12*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define DRAGON1_Y (0*FIELD_CONST_GRID_FX32_SIZE)
#define DRAGON1_Z (29*FIELD_CONST_GRID_FX32_SIZE)
#define DRAGON_HEAD1_Z (31*FIELD_CONST_GRID_FX32_SIZE)

#define DRAGON2_X (42*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define DRAGON2_Y (0*FIELD_CONST_GRID_FX32_SIZE)
#define DRAGON2_Z (23*FIELD_CONST_GRID_FX32_SIZE)
#define DRAGON_HEAD2_Z (21*FIELD_CONST_GRID_FX32_SIZE)

#define DRAGON3_X (27*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define DRAGON3_Y (6*FIELD_CONST_GRID_FX32_SIZE)
#define DRAGON3_Z (28*FIELD_CONST_GRID_FX32_SIZE)
#define DRAGON_HEAD3_Z (26*FIELD_CONST_GRID_FX32_SIZE)

#define FLOOR_X (32*FIELD_CONST_GRID_FX32_SIZE)
#define FLOOR_Y (0*FIELD_CONST_GRID_FX32_SIZE)
#define FLOOR_Z (32*FIELD_CONST_GRID_FX32_SIZE)

#define BUTTON1_X (17*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define BUTTON1_Y (0*FIELD_CONST_GRID_FX32_SIZE)
#define BUTTON1_Z (29*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define BUTTON2_X (7*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define BUTTON2_Y (0*FIELD_CONST_GRID_FX32_SIZE)
#define BUTTON2_Z (29*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define BUTTON3_X (37*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define BUTTON3_Y (0*FIELD_CONST_GRID_FX32_SIZE)
#define BUTTON3_Z (22*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define BUTTON4_X (47*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define BUTTON4_Y (0*FIELD_CONST_GRID_FX32_SIZE)
#define BUTTON4_Z (22*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define BUTTON5_X (22*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define BUTTON5_Y (6*FIELD_CONST_GRID_FX32_SIZE)
#define BUTTON5_Z (27*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define BUTTON6_X (32*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define BUTTON6_Y (6*FIELD_CONST_GRID_FX32_SIZE)
#define BUTTON6_Z (27*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

static const VecFx32 DragonPos[DRAGON_NUM_MAX] = {
  { DRAGON1_X, DRAGON1_Y, DRAGON_HEAD1_Z },
  { DRAGON2_X, DRAGON2_Y, DRAGON_HEAD2_Z },
  { DRAGON3_X, DRAGON3_Y, DRAGON_HEAD3_Z },
};

static const VecFx32 ArmPos[DRAGON_NUM_MAX] = {
  { DRAGON1_X, DRAGON1_Y, DRAGON1_Z },
  { DRAGON2_X, DRAGON2_Y, DRAGON2_Z },
  { DRAGON3_X, DRAGON3_Y, DRAGON3_Z },
};

static const VecFx32 ButtonPos[DRAGON_NUM_MAX*2] = {
  { BUTTON1_X, BUTTON1_Y, BUTTON1_Z },
  { BUTTON2_X, BUTTON2_Y, BUTTON2_Z },
  { BUTTON3_X, BUTTON3_Y, BUTTON3_Z },
  { BUTTON4_X, BUTTON4_Y, BUTTON4_Z },
  { BUTTON5_X, BUTTON5_Y, BUTTON5_Z },
  { BUTTON6_X, BUTTON6_Y, BUTTON6_Z },
};


static const u16 DragonRad[DRAGON_NUM_MAX] = {
  0x8000,
  0,
  0
};

typedef struct ANM_PLAY_WORK_tag
{
  u8 ObjIdx;
  u8 AnmNum;
  u8 AllAnmNum;
  u8 JumpCount;
  fx32 JumpDownX;
  fx32 JumpDownBaseY;
  int AnmOfs[ANM_PLAY_MAX];
  int SeNo[2];
}ANM_PLAY_WORK;

//�W���������̈ꎞ���[�N
typedef struct GYM_DRAGON_TMP_tag
{
  int TrgtHead;
  DRA_ARM TrgtArm;
  DRAGON_WORK *DraWk;
  ANM_PLAY_WORK AnmPlayWk;
  BOOL FloorVanish;
}GYM_DRAGON_TMP;

//���\�[�X�̕��я���
enum {
  RES_ID_HEAD_MDL = 0,
  RES_ID_L_ARM_MDL,
  RES_ID_R_ARM_MDL,
  RES_ID_FLOOR_MDL,
  RES_ID_BUTTON_MDL,
  RES_ID_HEAD_ANM_UR,
  RES_ID_HEAD_ANM_RU,
  RES_ID_HEAD_ANM_UL,
  RES_ID_HEAD_ANM_LU,
  RES_ID_HEAD_ANM_DR,
  RES_ID_HEAD_ANM_RD,
  RES_ID_HEAD_ANM_DL,
  RES_ID_HEAD_ANM_LD,
  RES_ID_ARM_ANM1,
  RES_ID_ARM_ANM2,
  RES_ID_ARM_ANM3,
  RES_ID_ARM_ANM4,
  RES_ID_BUTTON_ANM,
  RES_ID_FLOOR_ANM,
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
  { ARCID_GYM_DRAGON, NARC_gym_dragon_in31_l_ue_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_l_sita_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_r_ue_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
	{ ARCID_GYM_DRAGON, NARC_gym_dragon_in31_r_sita_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_DRAGON, NARC_gym_dragon_in31_botton_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
  { ARCID_GYM_DRAGON, NARC_gym_dragon_m_gym0802f_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
};

//3D�A�j���@��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_head[] = {
  { RES_ID_HEAD_ANM_UR,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM_RU,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM_UL,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM_LU,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM_DR,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM_RD,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM_DL,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_HEAD_ANM_LD,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@���r
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_l_arm[] = {
  { RES_ID_ARM_ANM1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)  �r��
  { RES_ID_ARM_ANM2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)�@�r��
};
//3D�A�j���@�E�r
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_r_arm[] = {
  { RES_ID_ARM_ANM3,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)  �r��
  { RES_ID_ARM_ANM4,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)�@�r��
};

//3D�A�j���@�{�^��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_btn[] = {
  { RES_ID_BUTTON_ANM,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_floor[] = {
  { RES_ID_FLOOR_ANM,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
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
		g3Dutil_anmTbl_btn,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_btn),	//�A�j�����\�[�X��
	},
  //�{�^��
	{
		RES_ID_BUTTON_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BUTTON_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_btn,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_btn),	//�A�j�����\�[�X��
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
		g3Dutil_anmTbl_btn,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_btn),	//�A�j�����\�[�X��
	},
  //�{�^��
	{
		RES_ID_BUTTON_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BUTTON_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_btn,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_btn),	//�A�j�����\�[�X��
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
		g3Dutil_anmTbl_btn,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_btn),	//�A�j�����\�[�X��
	},
  //�{�^��
	{
		RES_ID_BUTTON_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BUTTON_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_btn,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_btn),	//�A�j�����\�[�X��
	},

  //��
	{
		RES_ID_FLOOR_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_FLOOR_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_floor,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_floor),	//�A�j�����\�[�X��
	},
};

static const GFL_G3D_UTIL_SETUP Setup = {
  g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};

//��̏�ԕʂ̓K�p�A�j���e�[�u��
static const int NeckAnmTbl[HEAD_DIR_MAX][HEAD_DIR_MAX] =
{
  //UP,DOWN,LEFT,RIGHT  (NEXT)

  {-1, -1, RES_ID_HEAD_ANM_UL, RES_ID_HEAD_ANM_UR},     //NOW UP
  {-1, -1, RES_ID_HEAD_ANM_DL, RES_ID_HEAD_ANM_DR},     //NOW DOWN
  {RES_ID_HEAD_ANM_LU, RES_ID_HEAD_ANM_LD, -1, -1},     //NOW LEFT
  {RES_ID_HEAD_ANM_RU, RES_ID_HEAD_ANM_RD, -1, -1},     //NOW RIGHT
};

//�r�̏�ԕʂ̓K�p�A�j���e�[�u��
static const int ArmAnmTbl[3/*DRA_ARM_MAX*/][ARM_DIR_MAX] =
{
  //UP,DOWN
  {0, 1},     //LEFT ARM
  {2, 3},     //RIGHT ARM
  {0,0},
};

static const int JumpY[JUMP_COUNT] = { 8,16,8,0,-8,-16,-24,-32 };

//���[���i���s�e�[�u��
static const int RailSt[DRAGON_NUM_MAX][HEAD_DIR_MAX][CHECK_RAIL_NUM] = {
  //ON, ON, OFF, OFF, OFF, OFF
  {
    { RE_LINE_38, RE_LINE_32, RE_LINE_33, RE_LINE_33_new, RE_LINE_34, RE_LINE_34_new }, //��̌�����
    { RE_LINE_38, RE_LINE_32, RE_LINE_33, RE_LINE_33_new, RE_LINE_34, RE_LINE_34_new }, //��̌�����   ���ۂ͂ǂ̕����ɂ������Ȃ�
    { RE_LINE_34, RE_LINE_34_new, RE_LINE_33, RE_LINE_33_new, RE_LINE_38, RE_LINE_32 }, //��̌�����
    { RE_LINE_33, RE_LINE_33_new, RE_LINE_34, RE_LINE_34_new, RE_LINE_38, RE_LINE_32 }, //��̌����E
  },
  {
    { RE_LINE_11, RE_LINE_14, RE_LINE_12, RE_LINE_12_new, RE_LINE_13, RE_LINE_13_new }, //��̌�����
    { RE_LINE_11, RE_LINE_14, RE_LINE_12, RE_LINE_12_new, RE_LINE_13, RE_LINE_13_new }, //��̌�����  ���ۂ͂ǂ̕����ɂ������Ȃ�
    { RE_LINE_13, RE_LINE_13_new, RE_LINE_11, RE_LINE_14, RE_LINE_12, RE_LINE_12_new }, //��̌�����
    { RE_LINE_12, RE_LINE_12_new, RE_LINE_11, RE_LINE_14, RE_LINE_13, RE_LINE_13_new }, //��̌����E
  },
  {
    { RE_LINE_53, RE_LINE_54, RE_LINE_55, RE_LINE_55_new, RE_LINE_56, RE_LINE_56_new }, //��̌�����
    { RE_LINE_53, RE_LINE_54, RE_LINE_55, RE_LINE_55_new, RE_LINE_56, RE_LINE_56_new }, //��̌�����  ���ۂ͂ǂ̕����ɂ������Ȃ�
    { RE_LINE_56, RE_LINE_56_new, RE_LINE_53, RE_LINE_54, RE_LINE_55, RE_LINE_55_new }, //��̌�����
    { RE_LINE_55, RE_LINE_55_new, RE_LINE_53, RE_LINE_54, RE_LINE_56, RE_LINE_56_new }, //��̌����E
  },
};


static void SetupMdl(FLD_EXP_OBJ_CNT_PTR ptr,
    const int inIdx, const VecFx32 *inPos, const u16 *inRad,
    const int inAnmNum, const BOOL inCulling);
static void SetupAnm(GYM_DRAGON_SV_WORK *gmk_sv_work, FLD_EXP_OBJ_CNT_PTR ptr, const int inIdx);

static GMEVENT_RESULT SwitchMoveEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT ArmMoveEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT HeadMoveEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT AnmEvt( GMEVENT* event, int* seq, void* work );
static  HEAD_DIR GetHeadDirByArm(DRAGON_WORK *wk);
static int GetHeadAnmIdx(DRAGON_WORK *wk, const HEAD_DIR inNextDir);
static GMEVENT_RESULT JumpDownEvt( GMEVENT* event, int* seq, void* work );

static void VanishFloor(FLD_EXP_OBJ_CNT_PTR ptr, FIELDMAP_WORK *fieldWork, const BOOL inVanish);
static MMDL *SeatchMmdl(FIELDMAP_WORK *fieldWork, const int inObjID);
static void SetObjVanish(FIELDMAP_WORK *fieldWork, const int inObjID, BOOL inVanish);
static void SetRailSt(FIELDMAP_WORK *fieldWork, const int inHeadIdx, const HEAD_DIR inDir);
static BOOL CheckFloorVanish(const VecFx32 *inVec);

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
    int idx;
    u16 rad = 0;
    rad = DragonRad[i];
    //��
    idx = OBJ_HEAD_1 + (i*DRAGON_PARTS_SET);
    SetupMdl(ptr, idx, &DragonPos[i], &rad, DRAGON_ANM_NUM, FALSE);

    //���r
    idx = OBJ_L_ARM_1 + (i*DRAGON_PARTS_SET);
    SetupMdl(ptr, idx, &ArmPos[i], &rad, ARM_ANM_NUM, FALSE);
   
    //�E�r
    idx = OBJ_R_ARM_1 + (i*DRAGON_PARTS_SET);
    SetupMdl(ptr, idx, &ArmPos[i], &rad, ARM_ANM_NUM, FALSE);

    //�{�^��
    idx = OBJ_BUTTON_L_1 + (i*DRAGON_PARTS_SET);
    SetupMdl(ptr, idx, &ButtonPos[i*2], NULL, BTN_ANM_NUM, TRUE);
    
    //�{�^��
    idx = OBJ_BUTTON_R_1 + (i*DRAGON_PARTS_SET);
    SetupMdl(ptr, idx, &ButtonPos[i*2+1], NULL, BTN_ANM_NUM, TRUE);

    //�Z�[�u�f�[�^�����āA�A�j����Ԃ��Z�b�g
    SetupAnm(gmk_sv_work, ptr, i);
  }

  //�����W�Z�b�g
  {
    int idx = OBJ_FLOOR;
    VecFx32 pos = {FLOOR_X,FLOOR_Y,FLOOR_Z};
    SetupMdl(ptr, idx, &pos, NULL, 0, FALSE);
    //�A�j������������
    FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_DRAGON_UNIT_IDX, OBJ_FLOOR, 0, TRUE);
  }

  //�Z�b�g�A�b�v��2�K�\������
  {
    VecFx32 pos;
    BOOL vanish;
    FIELD_PLAYER *fld_player;
    fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    FIELD_PLAYER_GetPos( fld_player, &pos );
    if ( CheckFloorVanish(&pos) ) vanish = TRUE;     //2�K�̏��Ɨ����\��
    else vanish = FALSE;       //2�K�̏��Ɨ���\��

    VanishFloor(ptr, fieldWork, vanish);

    tmp->FloorVanish = vanish;
  }

  //���[���̐i���s�ݒ�
  for(i=0;i<DRAGON_NUM_MAX;i++){
    DRAGON_WORK *wk = &gmk_sv_work->DraWk[i];
    SetRailSt(fieldWork, i, wk->HeadDir);
  }
}

//--------------------------------------------------------------
/**
 * ���f���̃Z�b�g�A�b�v�֐�
 * @param	      ptr       �n�a�i�|�C���^
 * @param       inIdx     �n�a�i�C���f�b�N�X
 * @param       inPos     �\�����W
 * @param       inRad     ��]�l�|�C���^�i�m�t�k�k�̂Ƃ��͌v�Z���Ȃ��j
 * @param       inAnmNum  �A�j����
 * @param       inCulling �J�����O�L��
 * @return      none
 */
//--------------------------------------------------------------
static void SetupMdl(FLD_EXP_OBJ_CNT_PTR ptr,
    const int inIdx, const VecFx32 *inPos, const u16 *inRad, const int inAnmNum, const BOOL inCulling)
{
  int i;
  GFL_G3D_OBJSTATUS *status; 
  status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_DRAGON_UNIT_IDX, inIdx);
  status->trans = *inPos;
  if (inRad!=NULL) MTX_RotY33(&status->rotate, FX_SinIdx(*inRad), FX_CosIdx(*inRad));
  //�J�����O�ݒ�
  FLD_EXP_OBJ_SetCulling(ptr, GYM_DRAGON_UNIT_IDX, inIdx, inCulling);
  //�A�j���ݒ�
  for (i=0;i<inAnmNum;i++)
  {
    //1��Đ��ݒ�
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_DRAGON_UNIT_IDX, inIdx, i);
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    //�A�j����~
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    //������
    FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_DRAGON_UNIT_IDX, inIdx, i, FALSE);
  }
/**
  //�A���t�@�ݒ�e�X�g
  {
    GFL_G3D_UTIL *util = FLD_EXP_OBJ_GetUtil(ptr);
    GFL_G3D_OBJ* obj = GFL_G3D_UTIL_GetObjHandle( util, inIdx );
    GFL_G3D_RND* rnd = GFL_G3D_OBJECT_GetG3Drnd( obj );
    GFL_G3D_RES* res = GFL_G3D_RENDER_GetG3DresMdl( rnd );
    NNSG3dResFileHeader* res_head = GFL_G3D_GetResourceFileHeader( res );
    NNSG3dResMdlSet* ms = NNS_G3dGetMdlSet(res_head);
		NNSG3dResMdl* mdl = NNS_G3dGetMdlByIdx(ms, 0);
    NNS_G3dMdlSetMdlAlphaAll((NNSG3dResMdl*)mdl, 24);
  }
*/  
}

//--------------------------------------------------------------
/**
 * �A�j���̃Z�b�g�A�b�v�֐�
 * @param	      ptr       �n�a�i�|�C���^
 * @param       inIdx     �h���S���̃C���f�b�N�X
 * @return      none
 */
//--------------------------------------------------------------
static void SetupAnm(GYM_DRAGON_SV_WORK *gmk_sv_work, FLD_EXP_OBJ_CNT_PTR ptr, const int inIdx)
{
  int anm_ofs;
  int res_base;
  int idx;
  fx32 last;
  EXP_OBJ_ANM_CNT_PTR anm;

  DRAGON_WORK *wk = &gmk_sv_work->DraWk[inIdx];

  res_base = RES_ID_HEAD_ANM_UR;
  //��
  switch(wk->HeadDir){
  case HEAD_DIR_UP:
    //�E����̃A�j���̃��X�g�t���[�����Z�b�g���Ă���
    anm_ofs = RES_ID_HEAD_ANM_RU - res_base;
    break;
  case HEAD_DIR_DOWN:
    //�E�����̃A�j���̃��X�g�t���[�����Z�b�g���Ă���
    anm_ofs = RES_ID_HEAD_ANM_RD - res_base;
    break;
  case HEAD_DIR_RIGHT:
    //�い�E�̃A�j���̃��X�g�t���[�����Z�b�g���Ă���
    anm_ofs = RES_ID_HEAD_ANM_UR - res_base;
    break;
  case HEAD_DIR_LEFT:
    //�い���̃A�j���̃��X�g�t���[�����Z�b�g���Ă���
    anm_ofs = RES_ID_HEAD_ANM_UL - res_base;
    break;
  default:
    GF_ASSERT(0);
    //�E����̃A�j���̃��X�g�t���[�����Z�b�g���Ă���
    anm_ofs = RES_ID_HEAD_ANM_RU - res_base;
  }

  {
    idx = OBJ_HEAD_1 + (inIdx*DRAGON_PARTS_SET);
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_DRAGON_UNIT_IDX, idx, anm_ofs);
    //�A�j����~����
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
    //����������
    FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_DRAGON_UNIT_IDX, idx, anm_ofs, TRUE);
    //�ŏI�t���[�����Z�b�g
    last = FLD_EXP_OBJ_GetAnimeLastFrame(anm);
    //���X�g�t���[��
    FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_DRAGON_UNIT_IDX, idx, anm_ofs, last );
  }

  //���r
  {
    idx = OBJ_L_ARM_1 + (inIdx*DRAGON_PARTS_SET);
    if (wk->ArmDir[DRA_ARM_LEFT] == ARM_DIR_UP) anm_ofs = ARM_DIR_UP;
    else anm_ofs = ARM_DIR_DOWN;

    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_DRAGON_UNIT_IDX, idx, anm_ofs);
    //�A�j����~����
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
    //����������
    FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_DRAGON_UNIT_IDX, idx, anm_ofs, TRUE);
    //�ŏI�t���[�����Z�b�g
    last = FLD_EXP_OBJ_GetAnimeLastFrame(anm);
    //���X�g�t���[��
    FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_DRAGON_UNIT_IDX, idx, anm_ofs, last );
  }

  //�E�r
  {
    idx = OBJ_R_ARM_1 + (inIdx*DRAGON_PARTS_SET);
    if (wk->ArmDir[DRA_ARM_RIGHT] == ARM_DIR_UP) anm_ofs = ARM_DIR_UP;
    else anm_ofs = ARM_DIR_DOWN;

    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_DRAGON_UNIT_IDX, idx, anm_ofs);
    //�A�j����~����
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
    //����������
    FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_DRAGON_UNIT_IDX, idx, anm_ofs, TRUE);
    //�ŏI�t���[�����Z�b�g
    last = FLD_EXP_OBJ_GetAnimeLastFrame(anm);
    //���X�g�t���[��
    FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_DRAGON_UNIT_IDX, idx, anm_ofs, last );
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
    if ( CheckFloorVanish(&pos) ) vanish = TRUE;     //2�K�̏��Ɨ����\��
    else vanish = FALSE;       //2�K�̏��Ɨ���\��

    if (tmp->FloorVanish != vanish)
    {
      VanishFloor(ptr, fieldWork, vanish);
      tmp->FloorVanish = vanish;
    }
  }

  if (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG){
    GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
    GMEVENT *event;
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT ){
      event = GYM_DRAGON_CreateGmkEvt(gsys, 1,0);
      GAMESYSTEM_SetEvent(gsys, event);
    }else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START ){
      event = GYM_DRAGON_CreateGmkEvt(gsys, 1,1);
      GAMESYSTEM_SetEvent(gsys, event);
    }
  }
}

//--------------------------------------------------------------
/**
 * �M�~�b�N�����C�x���g�쐬
 * @param	      gsys        �Q�[���V�X�e���|�C���^
 * @param       inDragonIdx   �Ώۃh���S���C���f�b�N�X0�`2
 * @param       inArmIdx   �Ώۘr�C���f�b�N�X0�`1�@0�F���@1�F�E
 * 
 * @return      GMEVENT     �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *GYM_DRAGON_CreateGmkEvt(GAMESYS_WORK *gsys, const int inDragonIdx, const int inArmIdx)
{
  GMEVENT * event;

  GYM_DRAGON_TMP *tmp;
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  {
    FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
    tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);
  }

  tmp->TrgtHead = inDragonIdx;
  tmp->TrgtArm = inArmIdx;
  tmp->DraWk = &gmk_sv_work->DraWk[inDragonIdx];

  OS_Printf("head=%d arm=%d\n",inDragonIdx, inArmIdx);

  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, SwitchMoveEvt, 0 );

  return event;
}

//--------------------------------------------------------------
/**
 * �r�M�~�b�N�����C�x���g�쐬
 * @param	      gsys        �Q�[���V�X�e���|�C���^
 * @param       inDragonIdx   �Ώۃh���S���C���f�b�N�X0�`2
 * @param       inArmIdx   �Ώۘr�C���f�b�N�X0�`1�@0�F���@1�F�E
 * 
 * @return      GMEVENT     �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *GYM_DRAGON_CreateArmMoveEvt(GAMESYS_WORK *gsys, const int inDragonIdx, const int inArmIdx)
{
  GMEVENT * event;

  GYM_DRAGON_TMP *tmp;
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  {
    FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
    tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);
  }

  tmp->TrgtHead = inDragonIdx;
  tmp->TrgtArm = inArmIdx;
  tmp->DraWk = &gmk_sv_work->DraWk[inDragonIdx];

  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, ArmMoveEvt, 0 );

  return event;
}

//--------------------------------------------------------------
/**
 * ��M�~�b�N�����C�x���g�쐬
 * @param	      gsys        �Q�[���V�X�e���|�C���^
 * @param       inDragonIdx   �Ώۃh���S���C���f�b�N�X0�`2
 * 
 * @return      GMEVENT     �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *GYM_DRAGON_CreateHeadMoveEvt(GAMESYS_WORK *gsys, const int inDragonIdx)
{
  GMEVENT * event;

  GYM_DRAGON_TMP *tmp;
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  {
    FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
    tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);
  }

  tmp->TrgtHead = inDragonIdx;
  tmp->DraWk = &gmk_sv_work->DraWk[inDragonIdx];

  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, HeadMoveEvt, 0 );

  return event;
}

//--------------------------------------------------------------
/**
 * ���@�A�j���C�x���g�쐬
 * @param	      gsys        �Q�[���V�X�e���|�C���^
 * @param       inDir       ���@�̌���
 * 
 * @return      GMEVENT     �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *GYM_DRAGON_CreateJumpDownEvt(GAMESYS_WORK *gsys, const int inDir)
{
  GMEVENT * event;

  GYM_DRAGON_TMP *tmp;
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
    tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);
  }

  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, JumpDownEvt, 0 );
  {
    ANM_PLAY_WORK *play_work = &tmp->AnmPlayWk;
    play_work->JumpCount = 0;
    play_work->JumpDownX = JUMP_MOVE_X;
    {
      VecFx32 pos;
      FIELD_PLAYER *fld_player;
      fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
      FIELD_PLAYER_GetPos( fld_player, &pos );
      play_work->JumpDownBaseY = pos.y;
    }
    if (inDir == DIR_LEFT) play_work->JumpDownX *= -1;
  }

  return event;

}

//--------------------------------------------------------------
/**
 * �h���S���̘r�̏�Ԃ��玟�̎�̈ʒu�����肷��
 * @param	      wk        �h���S���̃f�[�^���[�N
 * @return      HEAD_DIR    ���̌���
 */
//--------------------------------------------------------------
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

//--------------------------------------------------------------
/**
 * ���݂̎�̈ʒu�Ǝ��̎�̈ʒu����A�Đ������̃A�j���[�V���������肷��
 * @param	      wk        �h���S���̃f�[�^���[�N
 * @param       inNextDir    ���̌���
 * @return       int       �A�j���C���f�b�N�X
 */
//--------------------------------------------------------------
static int GetHeadAnmIdx(DRAGON_WORK *wk, const HEAD_DIR inNextDir)
{
  int idx = NeckAnmTbl[wk->HeadDir][inNextDir]-RES_ID_HEAD_ANM_UR;
#ifdef PM_DEBUG  
  switch(wk->HeadDir){
  case HEAD_DIR_UP:
    OS_Printf("���ݏ�\n");
    break;
  case HEAD_DIR_DOWN:
    OS_Printf("���݉�\n");
    break;
  case HEAD_DIR_LEFT:
    OS_Printf("���ݍ�\n");
    break;
  case HEAD_DIR_RIGHT:
    OS_Printf("���݉E\n");
    break;  
  }
#endif 
  GF_ASSERT_MSG(idx>=0, "now=%d next=%d",wk->HeadDir, inNextDir);
  return idx;
}

//--------------------------------------------------------------
/**
 * �h���S���̘r�̏�Ԃ��玟�̘r�̈ʒu�����肷��
 * @param	      wk        �h���S���̃f�[�^���[�N
 * @param       inArm     �r�C���f�b�N�X
 * @return       ARM_DIR   �r�̌���
 */
//--------------------------------------------------------------
static  ARM_DIR GetArmDir(DRAGON_WORK *wk, const DRA_ARM inArm)
{
  ARM_DIR dir, now_dir;

  now_dir = wk->ArmDir[inArm];
  if (now_dir == ARM_DIR_UP) dir = ARM_DIR_DOWN;
  else dir = ARM_DIR_UP;
  return dir;
}

//--------------------------------------------------------------
/**
 * �w��̘r�̈ړ�������A�j���̃C���f�b�N�X��Ԃ�
 * @param       inNextDir     �ړ���̘r�̕���
 * @param       inArm         �r�̃C���f�b�N�X
 * @return       int           �A�j���C���f�b�N�X
 */
//--------------------------------------------------------------
static int GetArmAnmIdx(const ARM_DIR inNextDir, const DRA_ARM inArm)
{
  return ArmAnmTbl[inArm][inNextDir];
}

//--------------------------------------------------------------
/**
 * �M�~�b�N�A�j���C�x���g
 * @param	  event   �C�x���g�|�C���^
 * @param   seq     �V�[�P���T
 * @param   work    ���[�N�|�C���^
 * @return  GMEVENT_RESULT  �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT SwitchMoveEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_DRAGON_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
  }

  switch(*seq){
  case 0:
    {
      GMEVENT * call_event;
      int obj_idx;
      if ( tmp->TrgtArm == DRA_ARM_LEFT ) obj_idx = OBJ_BUTTON_L_1;
      else obj_idx = OBJ_BUTTON_R_1;
      obj_idx += (tmp->TrgtHead*DRAGON_PARTS_SET);
      //�X�C�b�`�̂n�a�i�ƃA�j�����Z�b�g
      tmp->AnmPlayWk.ObjIdx = obj_idx;
      tmp->AnmPlayWk.AnmNum = 1;
      tmp->AnmPlayWk.AnmOfs[0] = 0;
      tmp->AnmPlayWk.AllAnmNum = BTN_ANM_NUM;
      tmp->AnmPlayWk.SeNo[0] = GYM_DRAGON_SE_SW;
      tmp->AnmPlayWk.SeNo[1] = NONE_SE;

      call_event = GMEVENT_Create(gsys, NULL, AnmEvt, 0);
      //�X�C�b�`�����A�j���C�x���g�R�[��
      GMEVENT_CallEvent(event, call_event);
      
    }
    (*seq)++;
    break;
  case 1:
    //�I��
    return GMEVENT_RES_FINISH;
  }
  
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �M�~�b�N�A�j���C�x���g
 * @param	  event   �C�x���g�|�C���^
 * @param   seq     �V�[�P���T
 * @param   work    ���[�N�|�C���^
 * @return  GMEVENT_RESULT  �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT ArmMoveEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_DRAGON_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
  }

  switch(*seq){
  case 0:
    {
      GMEVENT * call_event;
      int anm_idx;
      int obj_idx;
      ARM_DIR next_dir;
      //���ݏ󋵂��瓮�����r�A�j��������
      if ( tmp->DraWk->ArmDir[tmp->TrgtArm] == ARM_DIR_UP )
      {
        anm_idx = 1;  //���ɓ�����
        next_dir = ARM_DIR_DOWN;
        OS_Printf("�r�����ɓ�����\n");
      }
      else
      {
        anm_idx = 0;  //��ɓ�����
        next_dir = ARM_DIR_UP;
        OS_Printf("�r����ɓ�����\n");
      }

      if ( tmp->TrgtArm == DRA_ARM_LEFT ) obj_idx = OBJ_L_ARM_1;
      else obj_idx = OBJ_R_ARM_1;

      OS_Printf("�������r��%d obj= %d\n",tmp->TrgtArm,obj_idx);

      obj_idx += (tmp->TrgtHead*DRAGON_PARTS_SET);

      //�r�̂n�a�i�ƃA�j�����Z�b�g
      tmp->AnmPlayWk.ObjIdx = obj_idx;
      tmp->AnmPlayWk.AnmNum = 1;
      tmp->AnmPlayWk.AnmOfs[0] = anm_idx;
      tmp->AnmPlayWk.AllAnmNum = ARM_ANM_NUM;
      tmp->AnmPlayWk.SeNo[0] = GYM_DRAGON_SE_ARM_MOVE;
      tmp->AnmPlayWk.SeNo[1] = GYM_DRAGON_SE_ARM_STOP;
      
      call_event = GMEVENT_Create(gsys, NULL, AnmEvt, 0);
      //�r�A�j���C�x���g�R�[��
      GMEVENT_CallEvent(event, call_event);
      
      //�r�̏����X�V
      tmp->DraWk->ArmDir[tmp->TrgtArm] = next_dir;
    }
    (*seq)++;
    break;
  case 1:
    //�I��
    return GMEVENT_RES_FINISH;
  }
  
  return GMEVENT_RES_CONTINUE;
}


//--------------------------------------------------------------
/**
 * ��M�~�b�N�A�j���C�x���g
 * @param	  event   �C�x���g�|�C���^
 * @param   seq     �V�[�P���T
 * @param   work    ���[�N�|�C���^
 * @return  GMEVENT_RESULT  �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT HeadMoveEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_DRAGON_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
  }

  switch(*seq){
  case 0:
    {
      GMEVENT * call_event;
      HEAD_DIR next_dir;
      int anm_idx;
      int obj_idx;
      
      //�r�̏�Ԃ���A��A�j��������
      next_dir = GetHeadDirByArm(tmp->DraWk);
      anm_idx = GetHeadAnmIdx(tmp->DraWk, next_dir);

      OS_Printf("���̎�̓����́@%d anm=%d\n",next_dir, anm_idx);

      obj_idx = OBJ_HEAD_1+(tmp->TrgtHead*DRAGON_PARTS_SET);

      //��̂n�a�i�ƃA�j�����Z�b�g
      tmp->AnmPlayWk.ObjIdx = obj_idx;
      tmp->AnmPlayWk.AnmNum = 1;
      tmp->AnmPlayWk.AnmOfs[0] = anm_idx;
      tmp->AnmPlayWk.AllAnmNum = DRAGON_ANM_NUM;
      tmp->AnmPlayWk.SeNo[0] = GYM_DRAGON_SE_HEAD_MOVE;
      tmp->AnmPlayWk.SeNo[1] = GYM_DRAGON_SE_HEAD_STOP;

#ifdef PM_DEBUG
      {
        switch(anm_idx){
        case 0:
          OS_Printf("�い�E\n");
          break;
        case 1:
          OS_Printf("�E����\n");
          break;
        case 2:
          OS_Printf("�い��\n");
          break;
        case 3:
          OS_Printf("������\n");
          break;
        case 4:
          OS_Printf("�����E\n");
          break;
        case 5:
          OS_Printf("�E����\n");
          break;
        case 6:
          OS_Printf("������\n");
          break;
        case 7:
          OS_Printf("������\n");
          break;
        }
      }
#endif
      
      call_event = GMEVENT_Create(gsys, NULL, AnmEvt, 0);
      //��A�j���C�x���g�R�[��
      GMEVENT_CallEvent(event, call_event);
      //��̏��X�V
      tmp->DraWk->HeadDir = next_dir;
    }
    (*seq)++;
    break;
  case 1:
    {
      //��̌����Ƒ��삵����̃C���f�b�N�b�X���烌�[���̐i�s��Ԃ�ύX����
      SetRailSt(fieldWork, tmp->TrgtHead, tmp->DraWk->HeadDir);

      //�I��
      return GMEVENT_RES_FINISH;
    }
  }
  
  return GMEVENT_RES_CONTINUE;
}


//--------------------------------------------------------------
/**
 * �A�j���C�x���g
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
  ANM_PLAY_WORK *play_work = &tmp->AnmPlayWk;

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
  }

  switch(*seq){
  case 0:
    {
      int i;
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 obj_idx = play_work->ObjIdx;
      OS_Printf("obj_id = %d\n",obj_idx);
      for (i=0;i<play_work->AllAnmNum;i++)
      {
        u8 anm_ofs = i;
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_DRAGON_UNIT_IDX, obj_idx, i);
        //�A�j����~
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
        //�K�p����Ă���ł��낤�A�j���𖳌����i�K�p���Ă��Ȃ��A�j���������t���O���Z�b�g���ɂ����j
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_DRAGON_UNIT_IDX, obj_idx, anm_ofs, FALSE);
        OS_Printf("%d�𖳌�\n",anm_ofs);
      }

      for (i=0;i<play_work->AnmNum;i++)
      {
        u8 anm_ofs = play_work->AnmOfs[i];
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_DRAGON_UNIT_IDX, obj_idx, anm_ofs);
        //�A�j����~����
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
        //�A�j������������
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_DRAGON_UNIT_IDX, obj_idx, anm_ofs, TRUE);
        //���o��
        FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_DRAGON_UNIT_IDX, obj_idx, anm_ofs, 0 );
        OS_Printf("%d���Đ�\n",anm_ofs);

        //�r�d�Đ�
        PMSND_PlaySE(play_work->SeNo[0]);
      }
    }
    (*seq)++;
    break;
  case 1:
    {
      EXP_OBJ_ANM_CNT_PTR anm;
      int obj_idx;
      int anm_idx;
      obj_idx = play_work->ObjIdx;
      anm_idx = play_work->AnmOfs[0];//�擪�̃A�j���ŊĎ�
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_DRAGON_UNIT_IDX, obj_idx, anm_idx );
      //�A�j���҂�
      if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
      {
        OS_Printf("�A�j���I��\n");
        PMSND_StopSE();
        if (play_work->SeNo[1] != NONE_SE) PMSND_PlaySE(play_work->SeNo[1]);

        return GMEVENT_RES_FINISH;
      }
    }
  }

  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���@�̔�э~��A�j���C�x���g
 * @param	  event   �C�x���g�|�C���^
 * @param   seq     �V�[�P���T
 * @param   work    ���[�N�|�C���^
 * @return  GMEVENT_RESULT  �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT JumpDownEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_DRAGON_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_DRAGON_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_DRAGON_TMP_ASSIGN_ID);
  ANM_PLAY_WORK *play_work = &tmp->AnmPlayWk;
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_DRAGON );
  }

  switch(*seq){
  case 0:
    //�W�����vSE�Đ�
    PMSND_PlaySE( GYM_DRAAGON_SE_JUMP );
    (*seq)++;
    break;
  case 1:
    {
      VecFx32 pos;
      FIELD_PLAYER *fld_player;
      fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
      FIELD_PLAYER_GetPos( fld_player, &pos );
      pos.x += play_work->JumpDownX;
      pos.y = play_work->JumpDownBaseY + (JumpY[play_work->JumpCount]*FX32_ONE);
      FIELD_PLAYER_SetPos( fld_player, &pos );
      play_work->JumpCount++;
      if ( play_work->JumpCount >= JUMP_COUNT)
      {
        //���n�r�d(�X�C�b�`�r�d)
        PMSND_PlaySE( GYM_DRAGON_SE_SW );
        //�I��
        return GMEVENT_RES_FINISH;
      }
    }
  }
  
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �t���A�̕\���E��\��
 * @param	  ptr         �g���n�a�i�Ǘ��|�C���^
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 * @param   inVanish    �o�j�b�V���t���O�@�@TRUE�Ŕ�\��
 * @return  none
 */
//--------------------------------------------------------------
static void VanishFloor(FLD_EXP_OBJ_CNT_PTR ptr, FIELDMAP_WORK *fieldWork, const BOOL inVanish)
{
  FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_FLOOR, inVanish );
  FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_HEAD_3, inVanish );
  FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_L_ARM_3, inVanish );
  FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_R_ARM_3, inVanish );
  FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_BUTTON_L_3, inVanish );
  FLD_EXP_OBJ_SetVanish( ptr, GYM_DRAGON_UNIT_IDX, OBJ_BUTTON_R_3, inVanish );
  
  SetObjVanish(fieldWork, C08GYM0101_WKOBJCODE00_01, inVanish);
  SetObjVanish(fieldWork, TR_C08GYM0101_F2_01, inVanish);
//  SetObjVanish(fieldWork, TR_C08GYM0101_F2_02, inVanish); <<�����@20100120
  SetObjVanish(fieldWork, TR_C08GYM0101_F2_03, inVanish);
  SetObjVanish(fieldWork, TR_C08GYM0101_F2_04, inVanish);
}

//--------------------------------------------------------------
/**
 * �t�B�[���h���f���̌���
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 * @param   inObjID     �n�a�i�h�c
 * @return  MMDL        ���f���|�C���^
 */
//--------------------------------------------------------------
static MMDL *SeatchMmdl(FIELDMAP_WORK *fieldWork, const int inObjID)
{
  u32 no = 0;
	MMDL *mmdl;
  MMDLSYS * mmdlsys = FIELDMAP_GetMMdlSys( fieldWork );
	while( MMDLSYS_SearchUseMMdl(mmdlsys,&mmdl,&no) == TRUE ){
		if( MMDL_GetOBJID(mmdl) == inObjID ){
			return( mmdl );
		}
	}
  return NULL;
}

//--------------------------------------------------------------
/**
 * �n�a�i�̕\���E��\��
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 * @param   inObjID     �n�a�i�h�c
 * @param   inVanish    �o�j�b�V���t���O�@�@TRUE�Ŕ�\��
 * @return  none
 */
//--------------------------------------------------------------
static void SetObjVanish(FIELDMAP_WORK *fieldWork, const int inObjID, BOOL inVanish)
{
  MMDL *mmdl;
  mmdl = SeatchMmdl(fieldWork, inObjID);
  if (mmdl != NULL) MMDL_SetStatusBitVanish(mmdl, inVanish);
}

//--------------------------------------------------------------
/**
 * ���[���̐i���s�𐧌�
 * @param   fieldWork   �t�B�[���h���[�N�|�C���^
 * @param   inHeadIdx   ���̎�C���f�b�N�X�@0�`2
 * @param   inDir       ��̌���
 * @return  none
 */
//--------------------------------------------------------------
static void SetRailSt(FIELDMAP_WORK *fieldWork, const int inHeadIdx, const HEAD_DIR inDir)
{
  int i;
  u32 line_idx[CHECK_RAIL_NUM];
  BOOL valid[CHECK_RAIL_NUM];
  FLDNOGRID_MAPPER* p_mapper = FIELDMAP_GetFldNoGridMapper( fieldWork );
  
  for (i=0;i<CHECK_RAIL_NUM;i++)
  {
    line_idx[i] = RailSt[inHeadIdx][inDir][i];
    valid[i] = FALSE;
  }

  if (inDir != HEAD_DIR_DOWN)
  {
    valid[0] = TRUE;
    valid[1] = TRUE;
  }

  for (i=0;i<CHECK_RAIL_NUM;i++)
  {
    FLDNOGRID_MAPPER_SetLineActive( p_mapper, line_idx[i], valid[i] );
    NOZOMU_Printf("line%d = %d  Valid=%d\n",i,line_idx[i],valid[i]);
  }
}

//--------------------------------------------------------------
/**
 * 2F��\�����邩�̃`�F�b�N
 * @param   inVec       �`�F�b�N���W
 * @return  BOOL        TRUE�Ŕ�\��
 */
//--------------------------------------------------------------
static BOOL CheckFloorVanish(const VecFx32 *inVec)
{
  if ( inVec->y > FLOOR_VISIBLE_HEIGHT ) return FALSE;
  else
  {
    if ( (inVec->x >= FLOOR_VANISH_X)&&(inVec->z >= FLOOR_VANISH_Z) ) return TRUE;
  }

  return FALSE;
}
