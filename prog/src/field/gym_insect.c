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
//#include "script.h"     //for SCRIPT_CallScript
//#include "../../../resource/fldmapdata/script/c04gym0101_def.h"  //for SCRID_�`

#include "../../../resource/fldmapdata/gimmick/gym_insect/gym_insect_local_def.cdat"

//#include "sound/pm_sndsys.h"

//#include "gym_onsect_se_def.h"

#define GYM_INSECT_UNIT_IDX (0)
#define GYM_INSECT_TMP_ASSIGN_ID  (1)

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)


#define SW_ANM_NUM  (2)
#define PL_ANM_NUM  (2)

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
  {PL9_X,0,PL7_Z},
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

//�W���������̈ꎞ���[�N
typedef struct GYM_INSECT_TMP_tag
{
  u16 SwIdx;
  u16 PlIdx;
  u16 TrapX;
  u16 TrapZ;
}GYM_INSECT_TMP;


//���\�[�X�̕��я���
enum {
  RES_ID_PL_LEFT_MDL = 0,
  RES_ID_PL_RIGHT_MDL,
  RES_ID_EFFECT_MDL,
  RES_ID_SW_MDL,
  RES_ID_WALL_MDL,

  RES_ID_PL_LEFT_MOV1,
  RES_ID_PL_LEFT_MOV2,
  RES_ID_PL_RIGHT_MOV1,
  RES_ID_PL_RIGHT_MOV2,
  RES_ID_EFF_MOV1,
  RES_ID_EFF_MOV2,
  RES_ID_SW_MOV1,
  RES_ID_SW_MOV2,
  RES_ID_WALL1_MOV1,
  RES_ID_WALL1_MOV2,
  RES_ID_WALL2_MOV1,
  RES_ID_WALL2_MOV2,
  RES_ID_WALL3_MOV1,
  RES_ID_WALL3_MOV2,
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
};



//--���\�[�X�֘A--
//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_GYM_INSECT, NARC_gym_insect_c03_pole1_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_pole2_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_effect1_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_switch1_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD

  { ARCID_GYM_INSECT, NARC_gym_insect_c03_pole1_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_pole1_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_pole2_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_pole2_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_effect1_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_effect1_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_switch1_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_switch1_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1_1_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall1_2_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall2_1_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall2_2_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall3_1_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_INSECT, NARC_gym_insect_c03_wall3_2_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
};

//3D�A�j���@��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_wall[] = {
	{ RES_ID_WALL1_MOV1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL1_MOV2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL2_MOV1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL2_MOV2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL3_MOV1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL3_MOV2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
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
};

static const GFL_G3D_UTIL_SETUP Setup = {
  g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};


static GMEVENT_RESULT SwitchEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT PoleEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT TrTrapEvt( GMEVENT* event, int* seq, void* work );

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

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldWork, GYM_INSECT_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_INSECT_TMP));
  //�K�v�ȃ��\�[�X�̗p��
  FLD_EXP_OBJ_AddUnit(ptr, &Setup, GYM_INSECT_UNIT_IDX );

  //���W�Z�b�g
  for (i=0;i<INSECT_SW_MAX;i++)
  {
    int idx = OBJ_SW1 + i;
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, idx);
    status->trans = SwPos[i];
  }
  for (i=0;i<INSECT_PL_MAX;i++)
  {
    int idx = OBJ_PL1 + i;
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, idx);
    status->trans = PolePos[i];
  }
  for (i=0;i<INSECT_WALL_MAX;i++)
  {
    int idx = OBJ_WALL1 + i;
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_INSECT_UNIT_IDX, idx);
    status->trans = WallPos[i];
  }


  //�X�C�b�`�̃Z�b�g�A�b�v
  //�E�H�[���o���h�̃Z�b�g�A�b�v
  //�|�[���̃Z�b�g�A�b�v

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

  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );
}

//--------------------------------------------------------------
/**
 * �o���h�������݌�C�x���g
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_INSECT_PushWall(FIELDMAP_WORK *fieldWork)
{
  //�C�x���g�쐬
  ;
  //�C�x���g�R�[��
  ;

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
GMEVENT* GYM_INSECT_CreateTrTrapEvt(GAMESYS_WORK *gsys, const int inTrapX, const int inTrapZ)
{
  //�C�x���g�쐬
  GMEVENT* event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GYM_INSECT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_INSECT_TMP_ASSIGN_ID);

  //�g���b�v�ʒu�Z�b�g
  tmp->TrapX = inTrapX;
  tmp->TrapZ = inTrapZ;
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
        u8 check_idx;
        //�|�[���t���O�I��
        gmk_sv_work->Pl[tmp->PlIdx] = TRUE;
        //�E�H�[���o���h��̉��`�F�b�N
        check_idx = WallIdxCheck[ tmp->PlIdx ];
        if ( gmk_sv_work->WallSt[tmp->PlIdx] == WALL_ST_NORMAL )
        {
          //��̉��A�j���J�n
          ;
          (*seq)++;
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
    //��̉��A�j���҂�
    if (1)
    {
      //�E�H�[���X�e�[�g����̏�ԂɕύX
      gmk_sv_work->WallSt[tmp->PlIdx] = WALL_ST_WEAKNESS;
      //�C�x���g�I��
      return GMEVENT_RES_FINISH;
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
    (*seq)++;
    break;
  case 1: //�g���[�i�[�o���A�j���X�N���v�g�R�[��
    (*seq)++;
    break;
  case 2:
    //�o���A�j���I���҂�
    if (1){
      //�g���[�i�[��X�N���v�g�`�F���W
      ;
    }
  }
  return GMEVENT_RES_CONTINUE;
}
