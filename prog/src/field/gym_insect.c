//======================================================================
/**
 * @file  gym_insect.c
 * @brief  ���W��
 * @author  Saito
 * @date  09.11.05
 */
//======================================================================

#include "fieldmap.h"
#include "gym_insect_sv.h"
#include "gym_insect.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/fieldmap/gym_insect.naix"
#include "system/main.h"    //for HEAPID_FIELDMAP
#include "script.h"     //for SCRIPT_CallScript
#include "../../../resource/fldmapdata/script/c03gym0101_def.h"  //for SCRID_�`

#include "../../../resource/fldmapdata/gimmick/gym_insect/gym_insect_local_def.h"

//#include "sound/pm_sndsys.h"

//#include "gym_onsect_se_def.h"

#include "event_fldmmdl_control.h"  //for EVENT_ObjAnime

#define GYM_INSECT_UNIT_IDX (0)
#define GYM_INSECT_TMP_ASSIGN_ID  (1)

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)


#define SW_ANM_NUM  (2)
#define PL_ANM_NUM  (2)
#define EFF_ANM_NUM  (2)

#define WALL_BRK_ANM_NUM   (4)

#define WALL_CHG_ANM_NUM   (4)

#define WALL_ANM_NUM   (4+WALL_CHG_ANM_NUM)

#define TRAP_NUM  (2)

#define BAND_WIDTH  (32)

#define CONT_TIME   (2)   //�������ݒl�����Z�����x�^�����̎���

#define PAIR_NONE (-1)

//�|�[�����W
#define PL1_X (PL1_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL1_Z (PL1_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL2_X (PL2_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL2_Z (PL2_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL3_X (PL3_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL3_Z (PL3_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL4_X (PL4_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL4_Z (PL4_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL5_X (PL5_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL5_Z (PL5_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL6_X (PL6_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL6_Z (PL6_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL7_X (PL7_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL7_Z (PL7_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL8_X (PL8_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL8_Z (PL8_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL9_X (PL9_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL9_Z (PL9_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL10_X (PL10_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define PL10_Z (PL10_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

//�X�C�b�`���W
#define SW1_X (SW1_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW1_Z (SW1_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW2_X (SW2_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW2_Z (SW2_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW3_X (SW3_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW3_Z (SW3_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW4_X (SW4_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW4_Z (SW4_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW5_X (SW5_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW5_Z (SW5_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW6_X (SW6_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW6_Z (SW6_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW7_X (SW7_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW7_Z (SW7_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW8_X (SW8_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW8_Z (SW8_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

//�o���h���W
#define BD1_X (BD1_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD1_Z (BD1_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD2_X (BD2_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD2_Z (BD2_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD3_X (BD3_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD3_Z (BD3_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD4_X (BD4_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD4_Z (BD4_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD5_X (BD5_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD5_Z (BD5_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD6_X (BD6_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD6_Z (BD6_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD7_X (BD7_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define BD7_Z (BD7_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

static const VecFx32 SwPos[INSECT_SW_MAX] = {
  {SW1_X,0,SW1_Z},
  {SW2_X,0,SW2_Z},
  {SW3_X,0,SW3_Z},
  {SW4_X,0,SW4_Z},
  {SW5_X,0,SW5_Z},
  {SW6_X,0,SW6_Z},
  {SW7_X,0,SW7_Z},
  {SW8_X,0,SW8_Z},
};

static const VecFx32 PolePos[INSECT_PL_MAX] = {
  {PL1_X,0,PL1_Z},
  {PL2_X,0,PL2_Z},
  {PL3_X,0,PL3_Z},
  {PL4_X,0,PL4_Z},
  {PL5_X,0,PL5_Z},
  {PL6_X,0,PL6_Z},
  {PL7_X,0,PL7_Z},
  {PL8_X,0,PL8_Z},
  {PL9_X,0,PL9_Z},
  {PL10_X,0,PL10_Z},
};

static const VecFx32 WallPos[INSECT_WALL_MAX] = {
  {BD1_X,0,BD1_Z},
  {BD2_X,0,BD2_Z},
  {BD3_X,0,BD3_Z},
  {BD4_X,0,BD4_Z},
  {BD5_X,0,BD5_Z},
  {BD6_X,0,BD6_Z},
  {BD7_X,0,BD7_Z},
};

//�|�[�����Ƃ̊֘A�ǃC���f�b�N�X�ꗗ
static const u8 WallIdxCheck[INSECT_PL_MAX] = {
  0,0,
  1,1,
  2,2,
  3,
  4,
  5,5
};    //���E�H�[���C���f�b�N�X6�̓|�[���Ƃ̐ڑ����Ȃ�

//�|�[�����Ƃ̑Ί֘A�C���f�b�N�X�ꗗ
static const s8 PolePairIdx[INSECT_PL_MAX] = {
  1,  //0
  0,  //1
  3,  //2
  2,  //3
  5,  //4
  4,  //5
  PAIR_NONE,  //6 �΂Ȃ�
  PAIR_NONE,  //7 �΂Ȃ�
  9,   //8
  8,  //9
};

//�g���b�v�̈ʒu
static const VecFx32 TrapPos[TRAP_NUM] = 
{
  {
    TRAP1_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE,
    0,
    TRAP1_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE
  },
  {
    TRAP2_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE,
    0,
    TRAP2_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE
  },
};

//�W���������̈ꎞ���[�N
typedef struct GYM_INSECT_TMP_tag
{
  u8 SwIdx;
  u8 PlIdx;
  u8 TrEvtIdx;
  u8 WallIdx;
  u8 BrkWallIdx;
  u8 Val;
  u8 Dir;
  u8 Cont;
  VecFx32 BasePos;
  GFL_TCB *PushAnmTcb;
  BOOL PushEnd;
  BOOL PushStepStart;
}GYM_INSECT_TMP;

//���\�[�X�̕��я���
enum {
  RES_ID_PL_LEFT_MDL = 0,
  RES_ID_PL_RIGHT_MDL,
  RES_ID_EFFECT_MDL,
  RES_ID_SW_MDL,
  RES_ID_WALL_MDL,
  RES_ID_WALL_BRK_MDL,
  RES_ID_WALL_LOW_MDL,
  RES_ID_WALL_BRK_LOW_MDL,

  RES_ID_PL_LEFT_MOV1,
  RES_ID_PL_LEFT_MOV2,
  RES_ID_PL_RIGHT_MOV1,
  RES_ID_PL_RIGHT_MOV2,
  RES_ID_EFF_MOV1,
  RES_ID_EFF_MOV2,
  RES_ID_SW_MOV1,
  RES_ID_SW_MOV2,
  RES_ID_WALL1_MOV_N,
  RES_ID_WALL1_MOV_S,
  RES_ID_WALL2_MOV_N,
  RES_ID_WALL2_MOV_S,
  RES_ID_WALL3_MOV1,
  RES_ID_WALL3_MOV2,
  RES_ID_WALL3_MOV3,
  RES_ID_WALL3_MOV4,

  RES_ID_WALL1_BRK_MOV_N,
  RES_ID_WALL1_BRK_MOV_S,
  RES_ID_WALL2_BRK_MOV_N,
  RES_ID_WALL2_BRK_MOV_S,
};

//�n�a�i�C���f�b�N�X
enum {
  OBJ_SW1= 0,
  OBJ_SW2,
  OBJ_SW3,
  OBJ_SW4,
  OBJ_SW5,
  OBJ_SW6,
  OBJ_SW7,
  OBJ_SW8,

  OBJ_PL1,
  OBJ_PL2,
  OBJ_PL3,
  OBJ_PL4,
  OBJ_PL5,
  OBJ_PL6,
  OBJ_PL7,
  OBJ_PL8,
  OBJ_PL9,
  OBJ_PL10,

  OBJ_WALL1,
  OBJ_WALL2,
  OBJ_WALL3,
  OBJ_WALL4,
  OBJ_WALL5,
  OBJ_WALL6,
  OBJ_WALL7,

  OBJ_WALL_BRK1,
  OBJ_WALL_BRK2,
  OBJ_WALL_BRK3,
  OBJ_WALL_BRK4,
  OBJ_WALL_BRK5,
  OBJ_WALL_BRK6,
  OBJ_WALL_BRK7,

  OBJ_EFF,
  OBJ_WALL_DETAIL,
  OBJ_WALL_BRK_DETAIL,
};

//�ǃA�j���̏���
enum{
  ANM_WALL_N = 0,   //�k�����˔j
  ANM_WALL_S,       //������˔j
  ANM_WALL_SIDE_N,  //�k�����T�C�h
  ANM_WALL_SIDE_S,  //������T�C�h
  ANM_WALL_CHG1,    //�ǕύX1(���Ȃ��ǂ̂�)
  ANM_WALL_CHG2,    //�ǕύX2(���Ȃ��ǂ̂�)
  ANM_WALL_CHG3,    //�ǕύX3(���Ȃ��ǂ̂�)
  ANM_WALL_CHG4,    //�ǕύX4(���Ȃ��ǂ̂�)
};




//--���\�[�X�֘A--
//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_GYM_INSECT, NARC_gym_insect_c03_pole1_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_pole2_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_effect1_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_switch1_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall2_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1s_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall2s_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD


  { ARCID_GYM_INSECT, NARC_gym_insect_c03_pole1_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_pole1_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_pole2_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_pole2_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_effect1_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_effect1_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_switch1_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_switch1_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP

  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1_1a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1_1a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1_2a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1_2b_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1_3_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1_3_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1_3_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1_3_nsbma, GFL_G3D_UTIL_RESARC }, //IMA

  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall2_1a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall2_1b_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall2_2a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall2_2b_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
};

//3D�A�j���@��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_wall[] = {
	{ RES_ID_WALL1_MOV_N,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL1_MOV_S,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL2_MOV_N,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL2_MOV_S,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL3_MOV1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL3_MOV2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL3_MOV3,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL3_MOV4,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�����
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_wall_brk[] = {
	{ RES_ID_WALL1_BRK_MOV_N,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL1_BRK_MOV_S,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL2_BRK_MOV_N,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL2_BRK_MOV_S,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�X�C�b�`
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_sw[] = {
  { RES_ID_SW_MOV1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_SW_MOV2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�|�[��1
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_plL[] = {
  { RES_ID_PL_LEFT_MOV1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_PL_LEFT_MOV2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�|�[��2
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_plR[] = {
  { RES_ID_PL_RIGHT_MOV1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_PL_RIGHT_MOV2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�g���b�v�G�t�F�N�g
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_eff[] = {
  { RES_ID_EFF_MOV1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_EFF_MOV2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};



//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
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
  //�X�C�b�`4
  {
		RES_ID_SW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw),	//�A�j�����\�[�X��
	},
  //�X�C�b�`5
  {
		RES_ID_SW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw),	//�A�j�����\�[�X��
	},
  //�X�C�b�`6
  {
		RES_ID_SW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw),	//�A�j�����\�[�X��
	},
  //�X�C�b�`7
  {
		RES_ID_SW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw),	//�A�j�����\�[�X��
	},
  //�X�C�b�`8
  {
		RES_ID_SW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw),	//�A�j�����\�[�X��
	},

  //�|�[��1   ��
  {
		RES_ID_PL_LEFT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_PL_LEFT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_plL,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_plL),	//�A�j�����\�[�X��
	},
  //�|�[��2   �E
  {
		RES_ID_PL_RIGHT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_PL_RIGHT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_plR,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_plR),	//�A�j�����\�[�X��
	},
  //�|�[��3   ��
  {
		RES_ID_PL_LEFT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_PL_LEFT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_plL,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_plL),	//�A�j�����\�[�X��
	},
  //�|�[��4   �E
  {
		RES_ID_PL_RIGHT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_PL_RIGHT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_plR,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_plR),	//�A�j�����\�[�X��
	},
  //�|�[��5   ��
  {
		RES_ID_PL_LEFT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_PL_LEFT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_plL,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_plL),	//�A�j�����\�[�X��
	},
  //�|�[��6   �E
  {
		RES_ID_PL_RIGHT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_PL_RIGHT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_plR,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_plR),	//�A�j�����\�[�X��
	},
  //�|�[��7   �E
  {
		RES_ID_PL_RIGHT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_PL_RIGHT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_plR,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_plR),	//�A�j�����\�[�X��
	},
  //�|�[��8   �E
  {
		RES_ID_PL_RIGHT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_PL_RIGHT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_plR,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_plR),	//�A�j�����\�[�X��
	},
  //�|�[��9   ��
  {
		RES_ID_PL_LEFT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_PL_LEFT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_plL,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_plL),	//�A�j�����\�[�X��
	},
  //�|�[��10    �E
  {
		RES_ID_PL_RIGHT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_PL_RIGHT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_plR,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_plR),	//�A�j�����\�[�X��
	},

  //��1
  {
		RES_ID_WALL_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //��2
  {
		RES_ID_WALL_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //��3
  {
		RES_ID_WALL_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //��4
  {
		RES_ID_WALL_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //��5
  {
		RES_ID_WALL_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //��6
  {
		RES_ID_WALL_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //��7
  {
		RES_ID_WALL_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},

  //�󂹂��1
  {
		RES_ID_WALL_BRK_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //�󂹂��2
  {
		RES_ID_WALL_BRK_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //�󂹂��3
  {
		RES_ID_WALL_BRK_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //�󂹂��4
  {
		RES_ID_WALL_BRK_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //�󂹂��5
  {
		RES_ID_WALL_BRK_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //�󂹂��6
  {
		RES_ID_WALL_BRK_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
  //�󂹂��7
  {
		RES_ID_WALL_BRK_LOW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_LOW_MDL, 	//�e�N�X�`�����\�[�XID
		NULL,			//�A�j���e�[�u��(�����w��̂���)
		0,	//�A�j�����\�[�X��
	},
#if 0
  //��1
  {
		RES_ID_WALL_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall),	//�A�j�����\�[�X��
	},
  //��2
  {
		RES_ID_WALL_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall),	//�A�j�����\�[�X��
	},
  //��3
  {
		RES_ID_WALL_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall),	//�A�j�����\�[�X��
	},
  //��4
  {
		RES_ID_WALL_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall),	//�A�j�����\�[�X��
	},
  //��5
  {
		RES_ID_WALL_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall),	//�A�j�����\�[�X��
	},
  //��6
  {
		RES_ID_WALL_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall),	//�A�j�����\�[�X��
	},
  //��7
  {
		RES_ID_WALL_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall),	//�A�j�����\�[�X��
	},

  //�󂹂��1
  {
		RES_ID_WALL_BRK_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall_brk,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall_brk),	//�A�j�����\�[�X��
	},
  //�󂹂��2
  {
		RES_ID_WALL_BRK_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall_brk,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall_brk),	//�A�j�����\�[�X��
	},
  //�󂹂��3
  {
		RES_ID_WALL_BRK_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall_brk,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall_brk),	//�A�j�����\�[�X��
	},
  //�󂹂��4
  {
		RES_ID_WALL_BRK_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall_brk,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall_brk),	//�A�j�����\�[�X��
	},
  //�󂹂��5
  {
		RES_ID_WALL_BRK_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall_brk,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall_brk),	//�A�j�����\�[�X��
	},
  //�󂹂��6
  {
		RES_ID_WALL_BRK_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall_brk,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall_brk),	//�A�j�����\�[�X��
	},
  //�󂹂��7
  {
		RES_ID_WALL_BRK_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall_brk,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall_brk),	//�A�j�����\�[�X��
	},
#endif  
  //�g���b�v�G�t�F�N�g
  {
		RES_ID_EFFECT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_EFFECT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_eff,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_eff),	//�A�j�����\�[�X��
	},
  //�A�j���t����
  {
		RES_ID_WALL_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall),	//�A�j�����\�[�X��
	},
  //�A�j���t���󂹂��
  {
		RES_ID_WALL_BRK_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_BRK_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall_brk,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall_brk),	//�A�j�����\�[�X��
	},

};

static const GFL_G3D_UTIL_SETUP Setup = {
  g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};

static const MMDL_ACMD_LIST anime_up_table[] = {
  AC_WALK_U_8F, 1,
  ACMD_END, 0
};
static const MMDL_ACMD_LIST anime_down_table[] = {
  AC_WALK_D_8F, 1,
  ACMD_END, 0
};
static const MMDL_ACMD_LIST anime_up_push_table[] = {
  AC_STAY_WALK_U_32F, 1,
  ACMD_END, 0
};
static const MMDL_ACMD_LIST anime_down_push_table[] = {
  AC_STAY_WALK_D_32F, 1,
  ACMD_END, 0
};

static GMEVENT_RESULT SwitchEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT PoleEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT TrTrapEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT PushWallEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT WallReverseEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT PushWallSideEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT PushWallNoBreakEvt( GMEVENT* event, int* seq, void* work );
static int GetWallInfo( GYM_INSECT_SV_WORK *gmk_sv_work,
                        const int inX, const int inZ,
                        GYM_INSECT_WALL_ST *outSt, BOOL *outCenter  );

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_INSECT_Setup(FIELDMAP_WORK *fieldWork)
{
  int i;
  GYM_INSECT_SV_WORK *gmk_sv_work;
  GYM_INSECT_TMP *tmp;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_INSECT );
  }

  {
    int size = GFI_HEAP_GetHeapFreeSize(HEAPID_FIELDMAP);
    OS_Printf("bfore_size = %x\n",size);
  }

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldWork, GYM_INSECT_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_INSECT_TMP));
  //�K�v�ȃ��\�[�X�̗p��
  FLD_EXP_OBJ_AddUnit(ptr, &Setup, GYM_INSECT_UNIT_IDX );
{
    int size = GFI_HEAP_GetHeapFreeSize(HEAPID_FIELDMAP);
    OS_Printf("bfore_size = %x\n",size);
  }

  //���W�Z�b�g
  for (i=0;i<INSECT_SW_MAX;i++)
  {
    int j;
    int idx = OBJ_SW1 + i;
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, idx);
    status->trans = SwPos[i];
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_INSECT_UNIT_IDX, idx, TRUE);

    for (j=0;j<SW_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      if ( gmk_sv_work->Sw[i] ){
        fx32 last = FLD_EXP_OBJ_GetAnimeLastFrame(anm);
        //���X�g�t���[��
        FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_INSECT_UNIT_IDX, idx, j, last );
        //�A�j����~
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
      }
    }
  }
  for (i=0;i<INSECT_PL_MAX;i++)
  {
    int j;
    int idx = OBJ_PL1 + i;
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, idx);
    status->trans = PolePos[i];
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_INSECT_UNIT_IDX, idx, TRUE);
    for (j=0;j<SW_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      if ( gmk_sv_work->Pl[i] ){
        fx32 last = FLD_EXP_OBJ_GetAnimeLastFrame(anm);
        //���X�g�t���[��
        FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_INSECT_UNIT_IDX, idx, j, last );
        //�A�j����~
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
      }      
    }
  }
  for (i=0;i<INSECT_WALL_MAX;i++)
  {
    GFL_G3D_OBJSTATUS *status;
    int j;
    int nrm_idx = OBJ_WALL1 + i;
    int brk_idx = OBJ_WALL_BRK1 + i;
    status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, nrm_idx);
    status->trans = WallPos[i];
    status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, brk_idx);
    status->trans = WallPos[i];

    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_INSECT_UNIT_IDX, nrm_idx, TRUE);
    FLD_EXP_OBJ_SetCulling(ptr, GYM_INSECT_UNIT_IDX, brk_idx, TRUE);
    
    //�ǂ̏�Ԃ̂�蕪��
    switch(gmk_sv_work->WallSt[i]){
    case WALL_ST_NORMAL:
      //�󂹂�ǂ�����
      FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, brk_idx, TRUE );
      break;
    case WALL_ST_WEAKNESS:
      //�ʏ�ǂ�����
      FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, nrm_idx, TRUE );
      break;
    default:
      GF_ASSERT(0);
    }
  }
  
  {
    int j;
    int eff_idx = OBJ_EFF;
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_INSECT_UNIT_IDX, eff_idx, TRUE);
    for (j=0;j<EFF_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, eff_idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      //�A�j����~
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    }
  }

  //�A�j���t����
  {
    int j;
    int nrm_idx = OBJ_WALL_DETAIL;
    int brk_idx = OBJ_WALL_BRK_DETAIL;
    for (j=0;j<WALL_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, nrm_idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      //������
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, nrm_idx, j, FALSE);
    }
    for (j=0;j<WALL_BRK_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, brk_idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    }
    FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, nrm_idx, TRUE );
    FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, brk_idx, TRUE );
  }
  //�X�C�b�`�̃Z�b�g�A�b�v
  //�E�H�[���o���h�̃Z�b�g�A�b�v
  //�|�[���̃Z�b�g�A�b�v
  {
    int size = GFI_HEAP_GetHeapFreeSize(HEAPID_FIELDMAP);
    OS_Printf("after_size = %x\n",size);
  }
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_INSECT_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);

  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, GYM_INSECT_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_INSECT_Move(FIELDMAP_WORK *fieldWork)
{
  int i;
  GYM_INSECT_SV_WORK *gmk_sv_work;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_INSECT );
  }

  //�t�B�[���h�̎c��q�[�v���v�����g
  if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R ){
    int size = GFI_HEAP_GetHeapFreeSize(HEAPID_FIELDMAP);
    OS_Printf("size = %x\n",size);
  }

  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );
}

//--------------------------------------------------------------
/**
 * �Ǐ��擾
 * @param
 * @param
 * @param
 * @param
 * @param
 * @return    
 */
//--------------------------------------------------------------
static int GetWallInfo( GYM_INSECT_SV_WORK *gmk_sv_work,
                        const int inX, const int inZ,
                        GYM_INSECT_WALL_ST *outSt, BOOL *outCenter  )
{
  int i;
  for(i=0;i<INSECT_WALL_MAX;i++)
  {
    BOOL hit;
    int x,z;
    x = WallPos[i].x / FIELD_CONST_GRID_FX32_SIZE;
    z = WallPos[i].z / FIELD_CONST_GRID_FX32_SIZE;
    hit = FALSE;
    if ( (x == inX) && (z == inZ) )
    {
      //�ǐ^��
      *outCenter = TRUE;
      hit = TRUE;
    }
    else if ( (x-1 == inX) && (z == inZ) )
    {
      //�T�C�h
      *outCenter = FALSE;
      hit = TRUE;
    }
    else if( (x+1 == inX) && (z == inZ) )
    {
      //�T�C�h
      *outCenter = FALSE;
      hit = TRUE;
    }

    if (hit){
      *outSt = gmk_sv_work->WallSt[i];
      return i;
    }
  }

  //������Ȃ������Ƃ�
  *outCenter = FALSE;
  *outSt = WALL_ST_NORMAL;    //<<�s��l�h�~
  return INSECT_WALL_MAX;
}

//--------------------------------------------------------------
/**
 * �ǉ������݃C�x���g�쐬
 * @param	      gsys     �Q�[���V�X�e���|�C���^
 * @return      none
 */
//--------------------------------------------------------------
GMEVENT* GYM_INSECT_CreatePushWallEvt(GAMESYS_WORK *gsys, const int inX, const int inZ)
{
  GMEVENT* event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );

  int idx;
  GYM_INSECT_WALL_ST wall_st;
  BOOL weak;
  u16 dir;

  GYM_INSECT_SV_WORK *gmk_sv_work;
  {
    FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_INSECT );
  }

  dir = FIELD_PLAYER_GetDir( fld_player );

  if ( (dir!=DIR_UP)&&(dir!=DIR_DOWN) ) return NULL;

  //�l�����W�͉󂹂�ǂ̂ǂ̕������𒲂ׂ�
  idx = GetWallInfo(gmk_sv_work, inX,inZ, &wall_st, &weak);
  if (idx < INSECT_WALL_MAX)
  {
    if (weak)
    {
      if (wall_st == WALL_ST_WEAKNESS)
      {
        //��������Ă����
        event = GMEVENT_Create( gsys, NULL, PushWallEvt, 0 );
      }
      else
      {
        //��������Ă��Ȃ���
        event = GMEVENT_Create( gsys, NULL, PushWallNoBreakEvt, 0 );
      }
    }
    else
    {
      //�ǃT�C�h
      event = GMEVENT_Create( gsys, NULL, PushWallSideEvt, 0 );
    }
  }
  else
  {
    //�󂹂�ǂɃq�b�g���Ă��Ȃ�
    return NULL;
  }

  //�C�x���g�J�n���̃Z�b�g�A�b�v
  {
    tmp->Val = 0;
    tmp->Cont = 0;
    //���@�̌����擾
    tmp->Dir = dir;
    //�ǃC���f�b�N�X
    tmp->WallIdx = idx;
    //�s�b�a������
    tmp->PushAnmTcb = NULL;
    tmp->PushEnd = FALSE;
    tmp->PushStepStart = FALSE;
  }

  return event;
}


//--------------------------------------------------------------
/**
 * �o���h�������݌�C�x���g
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
static GMEVENT_RESULT PushWallEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_INSECT_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);

  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );

  u8 obj_idx;
  u8 detail_obj_idx;
  u8 anm_idx;
  EXP_OBJ_ANM_CNT_PTR anm;

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_INSECT );
  }

  obj_idx = OBJ_WALL_BRK1+tmp->WallIdx;
  detail_obj_idx = OBJ_WALL_BRK_DETAIL;
  anm_idx = ANM_WALL_N;
  //�����ŃA�j���C���f�b�N�X�ɃI�t�Z�b�g������
  if (tmp->Dir == DIR_DOWN){
    anm_idx += 1;
  }
  anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx);

  switch(*seq){
  case 0:
    {
      MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
      //���@�A�j���I���҂�
      if ( MMDL_CheckEndAcmd( mmdl ) && MMDL_CheckPossibleAcmd( mmdl ) )
      {
        //�����݃X�^�[�g
        tmp->PushStepStart = TRUE;
        FIELD_PLAYER_GetPos( fld_player, &tmp->BasePos );

        //�A�j�����Ă��Ȃ��ǂ��\���ɂ���
        FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, obj_idx, TRUE );
        //�A�j������ǂ�\��
        FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, FALSE );
        //���W�Z�b�g
        {
          GFL_G3D_OBJSTATUS *status;
          GFL_G3D_OBJSTATUS *status_detail;
          status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, obj_idx);
          status_detail = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx);
          status_detail->trans = status->trans;
        }
        //�A�j���L����
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx, TRUE);
        //���o��
        FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx, 0 );
        //�A�j����~
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
        (*seq)++;
      }
    }
    break;
  case 1:
    {
      int check_key;
      int accel;
      //�L�[���͌��o
      if ( tmp->Dir == DIR_UP ){
        check_key = PAD_KEY_UP;
        accel = -1;
      }else{
        check_key = PAD_KEY_DOWN;
        accel = 1;
      }

      if ( GFL_UI_KEY_GetCont() & check_key )
      {
        VecFx32 pos;
        tmp->Cont++;
        if (tmp->Cont>=CONT_TIME)
        {
          tmp->Cont = 0;
          tmp->Val++;
          FIELD_PLAYER_GetPos( fld_player, &pos );
          pos.z += (accel*FX32_ONE);
          FIELD_PLAYER_SetPos( fld_player, &pos );
          //�A�j���t���[���i�߂�
          FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_INSECT_UNIT_IDX,
                                    detail_obj_idx, anm_idx, (tmp->Val/2)*FX32_ONE );

          if ( tmp->Val >= BAND_WIDTH ) (*seq)++;
          
        } //end if (tmp->Cont>=CONT_TIME)
      } //end if ( GFL_UI_KEY_GetCont() & check_key )
      else
      {
        GMEVENT *next_evt;
        //�L�[���͂�߂�
        next_evt = GMEVENT_Create(gsys, NULL, WallReverseEvt, 0);
        GMEVENT_ChangeEvent(event, next_evt);
        return GMEVENT_RES_CONTINUE;
      }
    }
    break;
  case 2:
    if ( tmp->PushAnmTcb != NULL )
    {
      if( MMDL_CheckEndAcmdList(tmp->PushAnmTcb) == TRUE )
      {
        MMDL_EndAcmdList( tmp->PushAnmTcb );
		    tmp->PushAnmTcb = NULL;
        tmp->PushEnd = TRUE;
        (*seq)++;
      }
    }else{
      tmp->PushEnd = TRUE;
      (*seq)++;
    }

    if (tmp->PushEnd){
      //�c��A�j���Đ�
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
      //�A�j���R�}���h���s
      {
        GMEVENT * child;
        void *tbl;
        if ( tmp->Dir == DIR_UP ){
          tbl = (void*)anime_up_table;
        }else{
          tbl = (void*)anime_down_table;
        }
        child = EVENT_ObjAnime( gsys, fieldWork, MMDL_ID_PLAYER, tbl );
        GMEVENT_CallEvent(event, child);
      }
    }
    break;
  case 3:
    //�A�j���I���҂�
    {
      fx32 frm;
      frm = FLD_EXP_OBJ_GetObjAnmFrm(ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx );
      OS_Printf("%x :: %d\n", frm, frm/FX32_ONE);
    }
    if ( FLD_EXP_OBJ_ChkAnmEnd(anm) ){
      //�A�j��������
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx, FALSE);
      //�A�j������ǂ��\��
      FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, TRUE);
      //�A�j�����Ȃ��ǂ�\��
      FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, obj_idx, FALSE);
      return GMEVENT_RES_FINISH;
    }
  }

  //�������݃A�j��
  if ( (tmp->PushStepStart) &&(!tmp->PushEnd) )
  {
    MMDL *fmmdl;
    fmmdl = FIELD_PLAYER_GetMMdl( fld_player );

    if ( tmp->PushAnmTcb == NULL)
    {
      void *tbl;
      if ( tmp->Dir == DIR_UP ) tbl = (void*)anime_up_push_table;
      else tbl = (void*)anime_down_push_table;
      tmp->PushAnmTcb = MMDL_SetAcmdList( fmmdl, tbl );
    }
    else{
      if( MMDL_CheckEndAcmdList(tmp->PushAnmTcb) == TRUE )
      {
        MMDL_EndAcmdList( tmp->PushAnmTcb );
			  tmp->PushAnmTcb = NULL;
      }
    }
  }

  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �e����C�x���g
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
static GMEVENT_RESULT WallReverseEvt( GMEVENT* event, int* seq, void* work )
{
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);

  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );

  u8 obj_idx = OBJ_WALL_BRK1+tmp->WallIdx;
  u8 detail_obj_idx = OBJ_WALL_BRK_DETAIL;
  u8 anm_idx = ANM_WALL_N;
  //�����ŃA�j���C���f�b�N�X�ɃI�t�Z�b�g������
  if (tmp->Dir == DIR_DOWN){
    anm_idx += 1;
  }

  switch(*seq){
  case 0:
    //�A�j���I���҂�
    if ( tmp->PushAnmTcb != NULL)
    {
      if( MMDL_CheckEndAcmdList(tmp->PushAnmTcb) == TRUE )
      {
        MMDL_EndAcmdList( tmp->PushAnmTcb );
			  tmp->PushAnmTcb = NULL;
        tmp->Val = 0;
        (*seq)++;
      }
    }
    else
    {
      tmp->Val = 0;
      (*seq)++;
    }
    break;
  case 1:
    {
      BOOL arrive;
      VecFx32 pos;
      fx32 diff;
      int accel;
      FIELD_PLAYER_GetPos( fld_player, &pos );
      //�߂�l�Z�b�g
      tmp->Val++;
      arrive = FALSE;
      if ( tmp->Dir == DIR_UP ){
        accel = 1;
        pos.z += (accel*tmp->Val*FX32_ONE);
        //��_�n�_�܂Ŗ߂������H
        if ( pos.z >= tmp->BasePos.z )
        {
          pos.z = tmp->BasePos.z;
          arrive = TRUE; //���B
        }
        diff = tmp->BasePos.z - pos.z;
      }else{
        accel = -1;
        pos.z += (accel*tmp->Val*FX32_ONE);
        //��_�n�_�܂Ŗ߂������H
        if ( pos.z <= tmp->BasePos.z )
        {
          pos.z = tmp->BasePos.z;
          arrive = TRUE; //���B
        }
        diff = pos.z - tmp->BasePos.z;
      }

      FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx, (diff/2) );
      FIELD_PLAYER_SetPos( fld_player, &pos );

      if (arrive)
      {
        //�A�j��������
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx, FALSE);
        //�A�j������ǂ��\��
        FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, TRUE);
        //�A�j�����Ȃ��ǂ�\��
        FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, obj_idx, FALSE);
        
        return GMEVENT_RES_FINISH;
      }
    }
  }

  return GMEVENT_RES_CONTINUE;
}


//--------------------------------------------------------------
/**
 * �o���h�T�C�h�������݃C�x���g
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
static GMEVENT_RESULT PushWallSideEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_INSECT_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);

  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );

  u8 obj_idx;
  u8 detail_obj_idx;
  u8 anm_idx;
  EXP_OBJ_ANM_CNT_PTR anm;

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_INSECT );
  }

  //�˔j�ł���ǂ��ŕ���
  if(gmk_sv_work->WallSt[tmp->WallIdx] == WALL_ST_NORMAL){
    obj_idx = OBJ_WALL1+tmp->WallIdx;
    detail_obj_idx = OBJ_WALL_DETAIL;
  }else{
    obj_idx = OBJ_WALL_BRK1+tmp->WallIdx;
    detail_obj_idx = OBJ_WALL_BRK_DETAIL;
  }

  anm_idx = ANM_WALL_SIDE_N;
  //�����ŃA�j���C���f�b�N�X�ɃI�t�Z�b�g������
  if (tmp->Dir == DIR_DOWN){
    anm_idx += 1;
  }

  anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx);

  switch(*seq){
  case 0:
    //�A�j�����Ă��Ȃ��ǂ��\���ɂ���
    FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, obj_idx, TRUE );
    //�A�j������ǂ�\��
    FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, FALSE );
    //���W�Z�b�g
    {
       GFL_G3D_OBJSTATUS *status;
       GFL_G3D_OBJSTATUS *status_detail;
       status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, obj_idx);
       status_detail = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx);
       status_detail->trans = status->trans;
    }
    //�ǂ������A�j���i�T�C�h�Ȃ̂ŁA�˔j�ł��Ȃ��j
    //�A�j�����J�n
    FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx, TRUE);
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
    //���o��
    FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx, 0 );
    
    (*seq)++;
    break;
  case 1:
    //�A�j���I���҂�
    if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
    {
      //�A�j�����I������疳�������Ă���
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx, FALSE);
      //�A�j������ǂ��\��
      FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, TRUE);
      //�A�j�����Ȃ��ǂ�\��
      FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, obj_idx, FALSE);
      return GMEVENT_RES_FINISH;
    }
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �˔j�ł��Ȃ��o���h�������݃C�x���g
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
static GMEVENT_RESULT PushWallNoBreakEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_INSECT_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);

  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );

  u8 obj_idx;
  u8 detail_obj_idx;
  u8 anm_idx;
  EXP_OBJ_ANM_CNT_PTR anm;

  obj_idx = OBJ_WALL1+tmp->WallIdx;
  detail_obj_idx = OBJ_WALL_DETAIL;

  anm_idx = ANM_WALL_N;
  //�����ŃA�j���C���f�b�N�X�ɃI�t�Z�b�g������
  if (tmp->Dir == DIR_DOWN){
    anm_idx += 1;
  }

  anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx);

  switch(*seq){
  case 0:
    //�A�j�����Ă��Ȃ��ǂ��\���ɂ���
    FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, obj_idx, TRUE );
    //�A�j������ǂ�\��
    FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, FALSE );
    //���W�Z�b�g
    {
       GFL_G3D_OBJSTATUS *status;
       GFL_G3D_OBJSTATUS *status_detail;
       status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, obj_idx);
       status_detail = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx);
       status_detail->trans = status->trans;
    }
    //�ǂ������A�j��
    //�A�j�����J�n
    FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx, TRUE);
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
    //���o��
    FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx, 0 );
    (*seq)++;
    break;
  case 1:
    //�A�j���I���҂�
    if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
    {
      //�A�j�����I������疳�������Ă���
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, anm_idx, FALSE);
      //�A�j������ǂ��\��
      FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, detail_obj_idx, TRUE);
      //�A�j�����Ȃ��ǂ�\��
      FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, obj_idx, FALSE);
      return GMEVENT_RES_FINISH;
    }
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �X�C�b�`�����C�x���g
 * @param	      gsys     �Q�[���V�X�e���|�C���^
 * @return      none
 */
//--------------------------------------------------------------
GMEVENT* GYM_INSECT_CreateSwitchEvt(GAMESYS_WORK *gsys, const int inSwIdx)
{
  GMEVENT* event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);

  if (inSwIdx >= INSECT_SW_MAX){
    return NULL;
  }

  //����\��X�C�b�`�C���f�b�N�X���Z�b�g
  tmp->SwIdx = inSwIdx;
  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, SwitchEvt, 0 );

  return event;
}

//--------------------------------------------------------------
/**
 * �|�[���A�j���C�x���g
 * @param	      gsys     �Q�[���V�X�e���|�C���^
 * @return      none
 */
//--------------------------------------------------------------
GMEVENT* GYM_INSECT_CreatePoleEvt(GAMESYS_WORK *gsys, const int inPoleIdx)
{
  GMEVENT* event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);

  if (inPoleIdx >= INSECT_PL_MAX){
    return NULL;
  }

  //����\��|�[���C���f�b�N�X���Z�b�g
  tmp->PlIdx = inPoleIdx;
  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, PoleEvt, 0 );

  return event;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�g���b�v�C�x���g�J�n
 * @param	      gsys     �Q�[���V�X�e���|�C���^
 * @return      none
 */
//--------------------------------------------------------------
GMEVENT* GYM_INSECT_CreateTrTrapEvt(GAMESYS_WORK *gsys,
                                    const int inTrEvtIdx)
{
  //�C�x���g�쐬
  GMEVENT* event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);

  //�g���b�v�C�x���g�C���f�b�N�X�Z�b�g
  tmp->TrEvtIdx = inTrEvtIdx;
  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, TrTrapEvt, 0 );

  return event;
}

//--------------------------------------------------------------
/**
 * �X�C�b�`�C�x���g
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
static GMEVENT_RESULT SwitchEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_INSECT_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_INSECT );
  }

  switch(*seq){
  case 0:  //�X�C�b�`�A�j���J�n
    {
      int i;
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 sw_obj_idx;
      sw_obj_idx = OBJ_SW1 + tmp->SwIdx;
      //�A�j�����J�n
      for (i=0;i<SW_ANM_NUM;i++){
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, sw_obj_idx, i, TRUE);
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, sw_obj_idx, i);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
      }
    }
    (*seq)++;
    break;
  case 1:
    {
      u8 sw_obj_idx;
      EXP_OBJ_ANM_CNT_PTR sw_anm;
      sw_obj_idx = OBJ_SW1 + tmp->SwIdx;
      //���A�j���I������͂������Ă���n�߃A�j���ōs�����Ƃɂ���
      sw_anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, sw_obj_idx, 0);
      //�X�C�b�`�A�j���I���҂�
      if ( FLD_EXP_OBJ_ChkAnmEnd(sw_anm) )
      {
        //�X�C�b�`�����t���O�I��
        gmk_sv_work->Sw[tmp->SwIdx] = TRUE;
        //�C�x���g�I��
        return GMEVENT_RES_FINISH;
      }
    }
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �|�[���C�x���g
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
static GMEVENT_RESULT PoleEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_INSECT_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_INSECT );
  }

  switch(*seq){
  case 0:  //�|�[���A�j���J�n
    {
      int i;
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 pl_obj_idx;
      pl_obj_idx = OBJ_PL1 + tmp->PlIdx;
      //�A�j�����J�n
      for (i=0;i<PL_ANM_NUM;i++){
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, pl_obj_idx, i, TRUE);
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, pl_obj_idx, i);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
      }
    }
    (*seq)++;
    break;
  case 1:
    {
      u8 pl_obj_idx;
      EXP_OBJ_ANM_CNT_PTR pl_anm;
      pl_obj_idx = OBJ_PL1 + tmp->PlIdx;
      //���A�j���I������͂������Ă���n�߃A�j���ōs�����Ƃɂ���
      pl_anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, pl_obj_idx, 0);
      //�|�[���A�j���I���҂�
      if ( FLD_EXP_OBJ_ChkAnmEnd(pl_anm) )
      {
        s8 pair;
        u8 check_idx;
        BOOL open;
        open = FALSE; //�|�[������t���O������
        //�|�[���t���O�I��
        gmk_sv_work->Pl[tmp->PlIdx] = TRUE;
        //�΂ɂȂ�|�[���̃t���O��Ԃ𒲂ׂ�
        pair = PolePairIdx[tmp->PlIdx];
        if (pair != PAIR_NONE){
          if (gmk_sv_work->Pl[pair] ) open = TRUE;
        }else{
          open = TRUE;
        }

        if (open)
        {
          //�E�H�[���o���h��̉��`�F�b�N
          check_idx = WallIdxCheck[ tmp->PlIdx ];
          if ( gmk_sv_work->WallSt[check_idx] == WALL_ST_NORMAL )
          {
            int i;
            EXP_OBJ_ANM_CNT_PTR anm;
            u8 wall_obj_idx;
            u8 detail_wall_obj_idx;
            u8 wall_anm_idx;
            wall_obj_idx = OBJ_WALL1 + check_idx;
            detail_wall_obj_idx = OBJ_WALL_DETAIL;
            //����ǂ̃C���f�b�N�X��ۑ�
            tmp->BrkWallIdx = check_idx;
            //�A�j�����Ă��Ȃ��ǂ��\���ɂ���
            FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, wall_obj_idx, TRUE );
            //�A�j������ǂ�\��
            FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, detail_wall_obj_idx, FALSE );
            //���W�Z�b�g
            {
              GFL_G3D_OBJSTATUS *status;
              GFL_G3D_OBJSTATUS *status_detail;
              status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, wall_obj_idx);
              status_detail = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, detail_wall_obj_idx);
              status_detail->trans = status->trans;
            }

            for (i=0;i<WALL_CHG_ANM_NUM;i++)
            {
              wall_anm_idx = ANM_WALL_CHG1+i;
              //��̉��A�j���J�n
              FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, detail_wall_obj_idx, wall_anm_idx, TRUE);
              anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, detail_wall_obj_idx, wall_anm_idx);
              FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
              //���o��
              FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_INSECT_UNIT_IDX, detail_wall_obj_idx, wall_anm_idx, 0 );
            }
            (*seq)++;
          }
          else
          {
            //�|�[�������S�������Ă��Ȃ��̂ŁA�C�x���g�I��
            return GMEVENT_RES_FINISH;
          }
        }
        else
        {
          //�ǂ̎�̉����Ȃ��̂ŁA�C�x���g�I��
          return GMEVENT_RES_FINISH;
        }
      }
    }
    break;
  case 2:
    {
      u8 wall_obj_idx;
      u8 detail_wall_obj_idx;
      u8 anm_idx;
      EXP_OBJ_ANM_CNT_PTR anm;
      wall_obj_idx = OBJ_WALL1 + tmp->BrkWallIdx;
      detail_wall_obj_idx = OBJ_WALL_DETAIL;
      anm_idx = ANM_WALL_CHG1;   //�A�j���I������p
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, detail_wall_obj_idx, anm_idx);
      //��̉��A�j���҂�
      if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
      {
        int i;
        //�A�j�����I������疳�������Ă���
        for (i=0;i<WALL_CHG_ANM_NUM;i++)
        {
          u8 wall_anm_idx = ANM_WALL_CHG1+i;
          FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, detail_wall_obj_idx, wall_anm_idx, FALSE);
        }
        //�E�H�[���X�e�[�g����̏�ԂɕύX
        gmk_sv_work->WallSt[tmp->BrkWallIdx] = WALL_ST_WEAKNESS;
        //�n�a�i�\���ؑ�
        {
          int nrm_idx = OBJ_WALL1 + tmp->BrkWallIdx;
          int brk_idx = OBJ_WALL_BRK1 + tmp->BrkWallIdx;
          //�ʏ�ǂ�����
          FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, nrm_idx, TRUE );
          //�󂹂�ǂ�\��
          FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, brk_idx, FALSE );
          //�A�j������ǂ��\��
          FLD_EXP_OBJ_SetVanish( ptr, GYM_INSECT_UNIT_IDX, detail_wall_obj_idx, TRUE);
        }
        //�C�x���g�I��
        return GMEVENT_RES_FINISH;
      }
    }
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �g���[�i�[�g���b�v�C�x���g
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
static GMEVENT_RESULT TrTrapEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_INSECT_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_INSECT );
  }

  switch(*seq){
  case 0:  //�g���[�i�[�o���A�j���J�n
    {
      int i;
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 obj_idx;
      obj_idx = OBJ_EFF;
      //�A�j�����J�n
      for (i=0;i<EFF_ANM_NUM;i++){
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_INSECT_UNIT_IDX, obj_idx, i, TRUE);
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, obj_idx, i);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
        //������
        FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_INSECT_UNIT_IDX, obj_idx, i, 0 );
      }
      //�ʒu�Z�b�g
      {
        GFL_G3D_OBJSTATUS *status;
        status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, obj_idx);
        status->trans = TrapPos[tmp->TrEvtIdx];
      }
    }
    (*seq)++;
    break;
  case 1: //�g���[�i�[�o���A�j���X�N���v�g�R�[��
    {      
      int scr_id;
      if ( tmp->TrEvtIdx ){
        scr_id = SCRID_PRG_C03GYM0101_TR2_APPEAR;
      }else{
        scr_id = SCRID_PRG_C03GYM0101_TR1_APPEAR;
      }
      SCRIPT_CallScript( event, scr_id,
          NULL, NULL, GFL_HEAP_LOWID(HEAPID_FIELDMAP) );      
    }
    (*seq)++;
    break;
  case 2:
    {
      u8 obj_idx;
      EXP_OBJ_ANM_CNT_PTR anm;
      obj_idx = OBJ_EFF;
      //���A�j���I������͂������Ă���n�߃A�j���ōs�����Ƃɂ���
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_INSECT_UNIT_IDX, obj_idx, 0);
      //�o���A�j���I���҂�
      if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
      {
        int scr_id;
        if ( tmp->TrEvtIdx ){
          scr_id = SCRID_PRG_C03GYM0101_TR2_BTL;
        }else{
          scr_id = SCRID_PRG_C03GYM0101_TR1_BTL;
        }
        //�g���[�i�[��X�N���v�g�`�F���W
        SCRIPT_ChangeScript( event, scr_id,
          NULL, GFL_HEAP_LOWID(HEAPID_FIELDMAP) );
        (*seq)++;
      }
    }
    break;
  case 3:
    GF_ASSERT(0);   //�C�x���g�`�F���W���Ă���̂ŁA�����ɂ͗��Ȃ�
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}
