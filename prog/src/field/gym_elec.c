//======================================================================
/**
 * @file  gym_elec.c
 * @bfief  �d�C�W��
 * @author  Saito
 * @date  09.09.02
 */
//======================================================================

#include "fieldmap.h"
#include "gym_elec.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"
#include "system/ica_anime.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/fieldmap/gym_elec.naix"
#include "system/main.h"    //for HEAPID_FIELDMAP
#include "script.h"     //for SCRIPT_CallScript
#include "../../../resource/fldmapdata/script/c04gym0101_def.h"  //for SCRID_�`

#define GYM_ELEC_UNIT_IDX (0)
#define GYM_ELEC_TMP_ASSIGN_ID  (1)

#define STOP_NUM_MAX  (2)
#define RALE_NUM_MAX  (8)
#define LEVER_NUM_MAX (4)
#define CAPSULE_NUM_MAX (4)
#define PLATFORM_NUM_MAX (9)

#define PLATFORM_NONE (0xff)
#define PLATFORM_NO_STOP (-1)

#define GYM_ELEC_GMK_OBJ_NUM  (10)
#define RALE_PLATFORM_NUM_MAX (2)   //1���[���Ŏ~�܂�v���b�g�z�[���̐�

#define STOP_TIME (90)
#define CAP_OPEN_FRAME  (15*FX32_ONE)   //�W���J������t���[���i�f�[�^�ˑ��j

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)


//�J�v�Z���������W
#define CAP1_X  (22*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAP1_Z  (37*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAP2_X  (41*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAP2_Z  (38*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAP3_X  (34*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAP3_Z  (6*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAP4_X  (5*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAP4_Z  (37*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
//�X�C�b�`���W
#define SW1_X (29*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW1_Z (33*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW2_X (13*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW2_Z (16*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW3_X (46*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW3_Z (41*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW4_X (25*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW4_Z (9*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
//OBJ�̂x���W
#define OBJ3D_Y  (2*FIELD_CONST_GRID_FX32_SIZE)


static const VecFx32 CapPos[CAPSULE_NUM_MAX] = {
  {CAP1_X,OBJ3D_Y,CAP1_Z},
  {CAP2_X,OBJ3D_Y,CAP2_Z},
  {CAP3_X,OBJ3D_Y,CAP3_Z},
  {CAP4_X,OBJ3D_Y,CAP4_Z},
};

static const int RaleAnm[RALE_NUM_MAX] = {
  NARC_gym_elec_gym04_rail_02a_bin,
  NARC_gym_elec_gym04_rail_02b_bin,
  NARC_gym_elec_gym04_rail_03a_bin,
  NARC_gym_elec_gym04_rail_03b_bin,
  NARC_gym_elec_gym04_rail_04a_bin,
  NARC_gym_elec_gym04_rail_04b_bin,
  NARC_gym_elec_gym04_rail_01a_bin,
  NARC_gym_elec_gym04_rail_01b_bin,
};



#define FRAME_POS_SIZE  (4*3*360)   //���W�w�x�y�e4�o�C�g��360�t���[��
#define HEADER_SIZE  (8)   //�t���[���T�C�Y4�o�C�g+�i�[���3�o�C�g�̃w�b�_�[��񂪂��邽�߃A���C�����g����������8�o�C�g�m�ۂ���

typedef struct CAPSULE_DAT_tag
{
  int RaleIdx;      //���ݑ��s���̃��[���C���f�b�N�X
  fx32 AnmFrame;    //���[���̌��݃A�j���t���[��
}CAPSULE_DAT;

typedef struct ELEC_GYM_TASK_WK_tag
{
  u32 Timer;
}ELEC_GYM_TASK_WK;

//�W���������̈ꎞ���[�N
typedef struct GYM_ELEC_TMP_tag
{
  GFL_TCB * MoveTcbPtr;     //�J�v�Z���Ǐ]�s�b�a
  GFL_TCB * CapStopTcbPtr;   //�J�v�Z����~����s�b�a
  u8 RadeRaleIdx;          //���@�����s���Ă��郌�[���C���f�b�N�X
  u8 LeverIdx;              //���삵�悤�Ƃ��Ă��郌�o�[�C���f�b�N�X
  u8 ChgingSw;
  u8 RideEvt;            //��~�C�x���g���J�v�Z���ԍ�
  ELEC_GYM_TASK_WK  TaskWk[CAPSULE_NUM_MAX];
  ICA_ANIME* IcaAnmPtr;
  u8 FramePosDat[FRAME_POS_SIZE+HEADER_SIZE];
  BOOL AltoMove;

  GFL_TCB *PlayerBack;
  GFL_TCB *TrGo;
}GYM_ELEC_TMP;

//�d�C�W���Z�[�u���[�N
typedef struct GYM_ELEC_SV_WORK_tag
{
  CAPSULE_DAT CapDat[CAPSULE_NUM_MAX];
  u8 LeverSw[LEVER_NUM_MAX];    //0or1
  u8 RaleChgReq[CAPSULE_NUM_MAX];
  u8 NowRaleIdx[CAPSULE_NUM_MAX];     //���ݑ��s���Ă��郌�[���̃C���f�b�N�X(�X�C�b�`��ˑ�)
  u8 RideFlg[CAPSULE_NUM_MAX];      //�J�v�Z���ɏ�������Ƃ����邩�H
  u8 StopPlatformIdx[CAPSULE_NUM_MAX];
}GYM_ELEC_SV_WORK;

typedef struct LEVER_SW_tag
{
  u16 OffOnRaleIdx[2];    //�Y����0�����o�[�I�t�@1�����o�[�I��
}LEVER_SW;

typedef struct STOP_DAT_tag
{
  int PlatformIdx;      //��ԃv���b�g�t�H�[��
  int StopStart;        //��ԊJ�n�t���[��
  int StopEnd;          //��ԏI���t���[��
}STOP_DAT;

typedef struct RALE_DAT_tag
{
  STOP_DAT StopDat[STOP_NUM_MAX];

}RALE_DAT;

//�g���[�i�[�G���J�E���g�O�A�j��
static const MMDL_ACMD_LIST AcmdList[] = {
  {AC_WALK_D_8F, 2},
  {ACMD_END, 0},
};

//���[�����Ƃ̒�ԃv���b�g�z�[��
static const s8 RaleStopPlatform[RALE_NUM_MAX][STOP_NUM_MAX] = {
  {1, 0},
  {1, 2},
  {4, -1},
  {4, 3},
  {6, -1},
  {6, 5},
  {8, -1},
  {8, 7}
};

//�v���b�g�z�[�����W���X�g(�O���b�h���W)
static const u16 PlatformXZ[PLATFORM_NUM_MAX][2] = {
  {14,39},
  {22,39},
  {16,27},
  {16,15},
  {41,40},
  {49,40},
  {34,8},
  {22,8},
  {5,39},
};

//�v���b�g�z�[���ʒ�ԃJ�v�Z���C���f�b�N�X���X�g
static const u8 PlatStpCapIdx[PLATFORM_NUM_MAX] = {
  0,0,0,1,1,2,2,3,3
};

//1�̃��o�[�Ő؂�ւ��郌�[���Ԃŋ��ʂŒ�Ԃ���v���b�g�z�[��
static const u8 CommPlatFormIdx[CAPSULE_NUM_MAX] = {
  1,4,6,8
};

//���[�����Ƃ̒�ԃA�j���t���[���i1���[��2���j
//1�ӏ������~�܂�Ȃ����[����2�f�[�^�Ƃ������l������BICA�f�[�^�ˑ�
static const fx32 StopFrame[RALE_NUM_MAX][RALE_PLATFORM_NUM_MAX] = {
  {0, FX32_ONE*15},     //���[��1
  {0, FX32_ONE*33},   //���[��2
  {0, FX32_ONE*0},     //���[��3
  {0, FX32_ONE*125},     //���[��4
  {0, FX32_ONE*0},     //���[��5
  {0, FX32_ONE*57},     //���[��6
  {0, FX32_ONE*0},     //���[��7
  {0, FX32_ONE*113},     //���[��8
};

//�J�v�Z�����Ƃ̃��[���ړ��A�j�����ʕ����t���[���i�A�j���f�[�^�ˑ��j
static const u16 CommFrame[CAPSULE_NUM_MAX] = {
  4,120,40,70
};

//���\�[�X�̕��я���
enum {
  RES_ID_CAP_MDL = 0,
  RES_ID_SW_MDL,
  RES_ID_RL1_MDL,
  RES_ID_RL2_MDL,
  RES_ID_RL3_MDL,
  RES_ID_RL4_MDL,

  RES_ID_CAP_OPCL,

  RES_ID_CAP_MOV1,
  RES_ID_CAP_MOV2,
  RES_ID_CAP_MOV3,
  RES_ID_CAP_MOV4,
  RES_ID_CAP_MOV5,
  RES_ID_CAP_MOV6,
  RES_ID_CAP_MOV7,
  RES_ID_CAP_MOV8,

  RES_ID_SW1_A,
  RES_ID_SW1_B,
  RES_ID_SW2_A,
  RES_ID_SW2_B,
  RES_ID_SW3_A,
  RES_ID_SW3_B,
  RES_ID_SW4_A,
  RES_ID_SW4_B,

  RES_ID_RL1_A,
  RES_ID_RL1_B,
  RES_ID_RL2_A,
  RES_ID_RL2_B,
  RES_ID_RL3_A,
  RES_ID_RL3_B,
  RES_ID_RL4_A,
  RES_ID_RL4_B,

};

//�n�a�i�C���f�b�N�X
enum {
  OBJ_CAP_1 = 0,
  OBJ_CAP_2,
  OBJ_CAP_3,
  OBJ_CAP_4,
  OBJ_SW_1,
  OBJ_SW_2,
  OBJ_SW_3,
  OBJ_SW_4,
  OBJ_RL_1,
  OBJ_RL_2,
  OBJ_RL_3,
  OBJ_RL_4,
};


//�J�v�Z���A�j���̏���
enum{
  ANM_CAP_OPCL = 0,    //�J���A����

  ANM_CAP_MOV1,        //���[��1
  ANM_CAP_MOV2,        //���[��2
  ANM_CAP_MOV3,        //���[��3
  ANM_CAP_MOV4,        //���[��4
  ANM_CAP_MOV5,        //���[��5
  ANM_CAP_MOV6,        //���[��6
  ANM_CAP_MOV7,        //���[��7
  ANM_CAP_MOV8,        //���[��8
  
//  ANM_SW2_A,          //�X�C�b�`2
//  ANM_SW2_B,          //�X�C�b�`2
//  ANM_SW3_A,          //�X�C�b�`3
//  ANM_SW3_B,          //�X�C�b�`3
//  ANM_SW4_A,          //�X�C�b�`4
//  ANM_SW4_B,          //�X�C�b�`4
};

//--���\�[�X�֘A--
//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_GYM_ELEC, NARC_gym_elec_gym04_cart_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@�J�v�Z���{��
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_switch_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@�X�C�b�`�{��
  
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@���[��1�{��
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_03_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@���[��2�{��
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_04_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@���[��3�{��
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_01_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@���[��4�{��

  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_cart_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@�J�v�Z���J������

  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�J�v�Z���ړ�1
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02b_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�J�v�Z���ړ�2
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_03a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�J�v�Z���ړ�3
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_03b_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�J�v�Z���ړ�4
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_04a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�J�v�Z���ړ�5
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_04b_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�J�v�Z���ړ�6
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_01a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�J�v�Z���ړ�7
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_01b_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�J�v�Z���ړ�8

  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_s_ao_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@�X�C�b�`1
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_s_mizu_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@�X�C�b�`1
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_s_kimi_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@�X�C�b�`2
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_s_kiiro_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@�X�C�b�`2
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_s_mura_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@�X�C�b�`3
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_s_ore_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@�X�C�b�`3
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_s_aka_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@�X�C�b�`4
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_s_pin_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@�X�C�b�`4

  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02a_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@���[��1
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02b_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@���[��1
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_03a_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@���[��2
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_03b_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@���[��2
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_04a_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@���[��3
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_04b_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@���[��3
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_01a_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@���[��4
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_01b_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@���[��4

};

//3D�A�j���@�J�v�Z��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_cap[] = {
	{ RES_ID_CAP_OPCL,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)

  { RES_ID_CAP_MOV1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAP_MOV2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAP_MOV3,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAP_MOV4,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAP_MOV5,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAP_MOV6,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAP_MOV7,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAP_MOV8,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)

};

//3D�A�j���@�X�C�b�`
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_sw1[] = {
  { RES_ID_SW1_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_SW1_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�X�C�b�`
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_sw2[] = {
  { RES_ID_SW2_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_SW2_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�X�C�b�`
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_sw3[] = {
  { RES_ID_SW3_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_SW3_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�X�C�b�`
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_sw4[] = {
  { RES_ID_SW4_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_SW4_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@���[��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_rl1[] = {
  { RES_ID_RL1_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_RL1_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@���[��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_rl2[] = {
  { RES_ID_RL2_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_RL2_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@���[��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_rl3[] = {
  { RES_ID_RL3_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_RL3_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@���[��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_rl4[] = {
  { RES_ID_RL4_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_RL4_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
  //�J�v�Z��1
	{
		RES_ID_CAP_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_CAP_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_cap,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_cap),	//�A�j�����\�[�X��
	},
  //�J�v�Z��2
  {
		RES_ID_CAP_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_CAP_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_cap,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_cap),	//�A�j�����\�[�X��
	},
  //�J�v�Z��3
  {
		RES_ID_CAP_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_CAP_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_cap,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_cap),	//�A�j�����\�[�X��
	},
  //�J�v�Z��4
  {
		RES_ID_CAP_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_CAP_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_cap,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_cap),	//�A�j�����\�[�X��
	},

  //�X�C�b�`1
  {
		RES_ID_SW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw1,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw1),	//�A�j�����\�[�X��
	},
  //�X�C�b�`2
  {
		RES_ID_SW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw2,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw2),	//�A�j�����\�[�X��
	},
  //�X�C�b�`3
  {
		RES_ID_SW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw3,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw3),	//�A�j�����\�[�X��
	},
  //�X�C�b�`4
  {
		RES_ID_SW_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw4,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw4),	//�A�j�����\�[�X��
	},

  //���[��1
  {
		RES_ID_RL1_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_RL1_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_rl1,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_rl1),	//�A�j�����\�[�X��
	},
  //���[��2
  {
		RES_ID_RL2_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_RL2_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_rl2,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_rl2),	//�A�j�����\�[�X��
	},
  //���[��3
  {
		RES_ID_RL3_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_RL3_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_rl3,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_rl3),	//�A�j�����\�[�X��
	},
  //���[��4
  {
		RES_ID_RL4_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_RL4_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_rl4,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_rl4),	//�A�j�����\�[�X��
	},
};

static const GFL_G3D_UTIL_SETUP Setup = {
  g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};


static u8 GetPlatformIdx(const int inX, const int inZ);
static u8 GetRaleIdxByCapIdx(GYM_ELEC_SV_WORK *gmk_sv_work, const u8 inCapIdx);
static u8 GetRaleIdxByPlatformIdx(GYM_ELEC_SV_WORK *gmk_sv_work, const u8 inPlatformIdx);
static u8 GetCapIdxByPlatformIdx(const u8 inPlatformIdx);
static BOOL IsStopCapsuleToPlatForm(GYM_ELEC_SV_WORK *gmk_sv_work, const u8 inPlatformIdx);
static fx32 GetAnimeFrame(FLD_EXP_OBJ_CNT_PTR ptr, const u16 inObjIdx, const u16 inAnmIdx);
static void SetAnimeFrame
         (FLD_EXP_OBJ_CNT_PTR ptr, const u16 inObjIdx, const u16 inAnmIdx, const int inFrame);
static GMEVENT_RESULT ChangePointEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT CapMoveEvt(GMEVENT* event, int* seq, void* work);
static GMEVENT_RESULT TrEncEvt(GMEVENT* event, int* seq, void* work);

static void ChgRale(FIELDMAP_WORK *fieldWork, FLD_EXP_OBJ_CNT_PTR ptr);
static void ChgRaleAnm(FLD_EXP_OBJ_CNT_PTR ptr, const u8 inSw, const u8 inCapIdx);
static BOOL CheckCapStopFrame(const u8 inRaleIdx, const fx32 inFrm,  u8 *outStopIdx);
static BOOL CheckChangableRaleAtOnce(FLD_EXP_OBJ_CNT_PTR ptr, const u8 inCapIdx, const u8 inRaleIdx);

static void CapStopTcbFunc(GFL_TCB* tcb, void* work);


static BOOL CheckCapTrEnc(GYM_ELEC_SV_WORK *gmk_sv_work, const u8 inCapIdx);

BOOL test_GYM_ELEC_ChangePoint(GAMESYS_WORK *gsys, const u8 inLeverIdx);
BOOL test_GYM_ELEC_CallMoveEvt(GAMESYS_WORK *gsys);

//--------------------------------------------------------------
/**
 * ���ݍ��W����v���b�g�z�[���C���f�b�N�X���擾����
 * @param	
 * @return
 */
//--------------------------------------------------------------
static u8 GetPlatformIdx(const int inX, const int inZ)
{
  u8 i;
  for (i=0;i<PLATFORM_NUM_MAX;i++){
    if ( (PlatformXZ[i][0] == inX) &&
         (PlatformXZ[i][1] == inZ) ){
      return i;
    }
  }
  return PLATFORM_NONE;
}

//--------------------------------------------------------------
/**
 * �J�v�Z���̃C���f�b�N�X��n���A�Z�[�u�f�[�^���瑖�s���̃��[���C���f�b�N�X���擾����
 * @param	
 * @return
 */
//--------------------------------------------------------------
static u8 GetRaleIdxByCapIdx(GYM_ELEC_SV_WORK *gmk_sv_work, const u8 inCapIdx)
{
  //���[���C���f�b�N�X�擾
  return gmk_sv_work->NowRaleIdx[inCapIdx];
}


//--------------------------------------------------------------
/**
 * ���[���A�j���[�V�����؂�ւ��s�b�a
 * @param	
 * @return
 */
//--------------------------------------------------------------
static BOOL RaleChgTcb()
{
  u8 i;
  //�S���̃J�v�Z���̃��[���A�j���؂�ւ����N�G�X�g�𒲂ׂ�
  for(i=0;i<LEVER_NUM_MAX;i++){
    if (0){//���N�G�X�g���������Ă��邩�H
      if (0){ //���N�G�X�g�ɓ�������v���b�g�z�[���ɒ�Ԓ����H
        //�A�j���ύX
        ;
        //���N�G�X�g�I�t
        ;
      }
    }
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_ELEC_Setup(FIELDMAP_WORK *fieldWork)
{
  GYM_ELEC_SV_WORK *gmk_sv_work;
  GYM_ELEC_TMP *tmp;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );
  }

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldWork, GYM_ELEC_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_ELEC_TMP));
  //�M�~�b�N���[�N�ɃA�N�Z�X
  ;  
  //�K�v�ȃ��\�[�X�̗p��
  FLD_EXP_OBJ_AddUnit(ptr, &Setup, GYM_ELEC_UNIT_IDX );  
  //���W�Z�b�g�@�J�v�Z��
  {
    VecFx32 pos = {CAP1_X, OBJ3D_Y, CAP1_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ELEC_UNIT_IDX, OBJ_CAP_1);
    status->trans = pos;
  }
  {
    VecFx32 pos = {CAP2_X, OBJ3D_Y, CAP2_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ELEC_UNIT_IDX, OBJ_CAP_2);
    status->trans = pos;
  }
  {
    VecFx32 pos = {CAP3_X, OBJ3D_Y, CAP3_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ELEC_UNIT_IDX, OBJ_CAP_3);
    status->trans = pos;
  }
  {
    VecFx32 pos = {CAP4_X, OBJ3D_Y, CAP4_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ELEC_UNIT_IDX, OBJ_CAP_4);
    status->trans = pos;
  }
  //���W�Z�b�g�@�X�C�b�`
  {
    VecFx32 pos = {SW1_X, OBJ3D_Y, SW1_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ELEC_UNIT_IDX, OBJ_SW_1);
    status->trans = pos;
  }
  {
    VecFx32 pos = {SW2_X, OBJ3D_Y, SW2_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ELEC_UNIT_IDX, OBJ_SW_2);
    status->trans = pos;
  }
  {
    VecFx32 pos = {SW3_X, OBJ3D_Y, SW3_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ELEC_UNIT_IDX, OBJ_SW_3);
    status->trans = pos;
  }
  {
    VecFx32 pos = {SW4_X, OBJ3D_Y, SW4_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ELEC_UNIT_IDX, OBJ_SW_4);
    status->trans = pos;
  }

  //���W�Z�b�g�@���[��
  {
    VecFx32 pos = {CAP1_X, OBJ3D_Y, CAP1_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ELEC_UNIT_IDX, OBJ_RL_1);
    status->trans = pos;
  }
  {
    VecFx32 pos = {CAP2_X, OBJ3D_Y, CAP2_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ELEC_UNIT_IDX, OBJ_RL_2);
    status->trans = pos;
  }
  {
    VecFx32 pos = {CAP3_X, OBJ3D_Y, CAP3_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ELEC_UNIT_IDX, OBJ_RL_3);
    status->trans = pos;
  }
  {
    VecFx32 pos = {CAP4_X, OBJ3D_Y, CAP4_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ELEC_UNIT_IDX, OBJ_RL_4);
    status->trans = pos;
  }




  //�Z�[�u����Ă���f�[�^�ŁA�A�j���[�V�����̓��o��
#if 0
  {
    //���o�[1��Đ��ݒ�
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, 4, 0);
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, 4, 1);
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, 4, 1, FALSE);
  }
#endif  
  //�J�v�Z���E�X�C�b�`�����A�j������
  {
    u8 i;
    for (i=0;i<CAPSULE_NUM_MAX;i++){
      u8 obj_idx;
      u8 anm_idx;

      obj_idx = OBJ_CAP_1+i;
      anm_idx = ANM_CAP_MOV1+(i*2);
      //���[���ړ��̌���(�����C���f�b�N�X��L��)
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, obj_idx, anm_idx, TRUE);
      //�W�A�j�����Z�b�g���Ď~�߂Ă���
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, obj_idx, ANM_CAP_OPCL, TRUE);
      {
        EXP_OBJ_ANM_CNT_PTR anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, ANM_CAP_OPCL);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
        //1��Đ��ݒ�
        FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      }
      //�X�C�b�`�A�j���̓K�p
      obj_idx = OBJ_SW_1+i;
      anm_idx = gmk_sv_work->LeverSw[i];
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, obj_idx, anm_idx, TRUE);
      {
        //1��Đ��ݒ�
        EXP_OBJ_ANM_CNT_PTR anm;
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, 0);
        FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, 1);
        FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      }
      //���[���A�j���̓K�p
      obj_idx = OBJ_RL_1+i;
      anm_idx = gmk_sv_work->LeverSw[i];
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, obj_idx, anm_idx, TRUE);
      {
        //1��Đ��ݒ�
        EXP_OBJ_ANM_CNT_PTR anm;
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, 0);
        FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, 1);
        FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      }
    }
  }
  
  //���[���A�j�����ύX�N�G�X�g�����s�b�a�쐬
  ;
  //���N�G�X�g�`�F�b�N��s�b�a���s
  ;
  tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);
  //�J�v�Z����~����s�b�a�ǉ�
  {
    GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldWork );
    // TCB��ǉ�
    tmp->CapStopTcbPtr = GFL_TCB_AddTask( tcbsys, CapStopTcbFunc, fieldWork, 0 );
  }

  //�C�x���g���N����ԃZ�b�g
  tmp->RideEvt = -1;


  //�C�j�b�g�֐��ɓ���鏈���@�����������ɂ���
  {
    gmk_sv_work->NowRaleIdx[0] = 0;
    gmk_sv_work->NowRaleIdx[1] = 2;
    gmk_sv_work->NowRaleIdx[2] = 4;
    gmk_sv_work->NowRaleIdx[3] = 6;

    gmk_sv_work->StopPlatformIdx[0] = PLATFORM_NO_STOP;
    gmk_sv_work->StopPlatformIdx[1] = PLATFORM_NO_STOP;
    gmk_sv_work->StopPlatformIdx[2] = PLATFORM_NO_STOP;
    gmk_sv_work->StopPlatformIdx[3] = PLATFORM_NO_STOP;
  }
}

static void CapStopTcbFunc(GFL_TCB* tcb, void* work)
{
  u8 i;
  FIELDMAP_WORK *fieldWork;
  GYM_ELEC_SV_WORK *gmk_sv_work;
  FLD_EXP_OBJ_CNT_PTR obj_cnt_ptr;
  GYM_ELEC_TMP *tmp;
  
  fieldWork = (FIELDMAP_WORK *)(work);
  obj_cnt_ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );
  }

  //----���N�G�X�g������----
  for (i=0;i<CAPSULE_NUM_MAX;i++){
    //���N�G�X�g�`�F�b�N
    if ( gmk_sv_work->RaleChgReq[i] ){
      //���݂̃X�C�b�`�̋t���`�F�b�N���ׂ��X�C�b�`
      u8 old_sw = (gmk_sv_work->LeverSw[i] +1 ) % 2;
      u8 obj_idx = OBJ_CAP_1+i;
      u8 anm_idx = ANM_CAP_MOV1 + (i * 2) + old_sw;
      fx32 frm = GetAnimeFrame(obj_cnt_ptr, obj_idx, anm_idx);
      //�A�j���؂�ւ������ėǂ��v���b�g�t�H�[���ɒ�Ԓ����H
      if( frm == 0 ){
        //�A�j���؂�ւ�
        ChgRaleAnm(obj_cnt_ptr, old_sw, i);
        //���s���[���ۑ�
        gmk_sv_work->NowRaleIdx[i] = (i*2)+gmk_sv_work->LeverSw[i];
        //���N�G�X�g�t���O�𗎂Ƃ�
        gmk_sv_work->RaleChgReq[i] = 0;
      }
    }
  }

  //----�J�v�Z����~���䕔----
  for(i=0;i<CAPSULE_NUM_MAX;i++){
    u8 obj_idx = OBJ_CAP_1+i;
    u8 rale_idx = gmk_sv_work->NowRaleIdx[i];
    u8 anm_idx = ANM_CAP_MOV1 + rale_idx;
    EXP_OBJ_ANM_CNT_PTR anm = FLD_EXP_OBJ_GetAnmCnt( obj_cnt_ptr, GYM_ELEC_UNIT_IDX, obj_idx, anm_idx);

///    if (i!=3){continue;}//�e�X�g
    
    //�^�C�}�[���Z�b�g����Ă���ꍇ�͎��ԏ���
    if (tmp->TaskWk[i].Timer){
      //��~�C�x���g���̓^�C�}�[�J�E���g���t�b�N
      if ( tmp->RideEvt == i ){
        continue;
      }
      //���ԏ���
      tmp->TaskWk[i].Timer--;
      //���ԏ��������������H
      if ( tmp->TaskWk[i].Timer == 0 ){
        //�A�j���X�g�b�v������
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
        gmk_sv_work->StopPlatformIdx[i] = PLATFORM_NO_STOP;
      }
    }else{
      u8 stop_idx;
      fx32 frame = FLD_EXP_OBJ_GetObjAnmFrm( obj_cnt_ptr, GYM_ELEC_UNIT_IDX, obj_idx, anm_idx );
      //��~���ׂ��t���[�����𒲂ׂ�
      if ( CheckCapStopFrame(rale_idx, frame, &stop_idx) ){
        //�A�j�����X�g�b�v
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
        //�w�莞�ԂƂ߂邽�߃^�C�}�[���Z�b�g
        tmp->TaskWk[i].Timer = STOP_TIME;
        gmk_sv_work->StopPlatformIdx[i] = RaleStopPlatform[rale_idx][stop_idx];
      }
    }
    OS_Printf("%d::stop_plat %d\n",i, gmk_sv_work->StopPlatformIdx[i]);
  } //end for

  //--���@�����ړ���--
  if (tmp->AltoMove){
    fx32 frame;
    u8 cap_idx;
    u8 anm_idx;
    int frm_idx;
    VecFx32 dst_vec = {0,0,0};
    //���[���C���f�b�N�X/2�ŃJ�v�Z���C���f�b�N�X�ɂȂ�
    cap_idx = tmp->RadeRaleIdx / 2;
    anm_idx = ANM_CAP_MOV1 + tmp->RadeRaleIdx;
    //�A�j���t���[���擾
    frame = FLD_EXP_OBJ_GetObjAnmFrm( obj_cnt_ptr, GYM_ELEC_UNIT_IDX, cap_idx, anm_idx );
    frm_idx = frame / FX32_ONE;
    
    //���W�擾
    if ( ICA_ANIME_GetTranslateAt( tmp->IcaAnmPtr, &dst_vec, frm_idx ) ){
      dst_vec.x += CapPos[cap_idx].x;
      dst_vec.y += CapPos[cap_idx].y;
      dst_vec.z += CapPos[cap_idx].z;
      //���@���W�Z�b�g
      FIELD_PLAYER_SetPos( FIELDMAP_GetFieldPlayer( fieldWork ), &dst_vec );
    }
  }
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_ELEC_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ELEC_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);

  //�Ď��s�b�a�폜
  GFL_TCB_DeleteTask( tmp->CapStopTcbPtr );
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, GYM_ELEC_UNIT_IDX );

}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_ELEC_Move(FIELDMAP_WORK *fieldWork)
{
  int i;
  GYM_ELEC_SV_WORK *gmk_sv_work;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );
  }

  //�e�X�g
  {
    GAMESYS_WORK *gsys  = FIELDMAP_GetGameSysWork( fieldWork );
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){
      test_GYM_ELEC_ChangePoint(gsys, 3);
    }
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
      test_GYM_ELEC_CallMoveEvt(gsys);
    }
    if (GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT ){
      FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
      GYM_ELEC_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);
      tmp->LeverIdx = 2;
      ChgRale(fieldWork, ptr);
    }
  }

  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );

  //�A�j���t���[�����Z�[�u�f�[�^�ɕۑ�
  for (i=0;i<CAPSULE_NUM_MAX;i++){
    u8 obj_idx;
    u8 anm_idx;
    obj_idx = OBJ_CAP_1 + i;
    anm_idx = ANM_CAP_MOV1 + gmk_sv_work->CapDat[i].RaleIdx;
    gmk_sv_work->CapDat[i].AnmFrame = GetAnimeFrame(ptr, obj_idx, anm_idx);
  }
}

//--------------------------------------------------------------
/**
 * �w��v���b�g�z�[���̌��݂̃��[���C���f�b�N�X���擾
 * @param	
 * @return
 */
//--------------------------------------------------------------
static u8 GetRaleIdxByPlatformIdx(GYM_ELEC_SV_WORK *gmk_sv_work, const u8 inPlatformIdx)
{
  u8 cap_idx;
  u8 rale_idx;
  //�w��v���b�g�t�H�[���ɒ�Ԃ���J�v�Z�����擾
  cap_idx = GetCapIdxByPlatformIdx(inPlatformIdx);
  //�擾�����J�v�Z�������s���Ă��郌�[���C���f�b�N�X���擾
  rale_idx = GetRaleIdxByCapIdx(gmk_sv_work, cap_idx);

  return rale_idx;
}

static u8 GetCapIdxByPlatformIdx(const u8 inPlatformIdx)
{
  u8 cap_idx;
  //�w��v���b�g�t�H�[���ɒ�Ԃ���J�v�Z�����擾
  cap_idx = PlatStpCapIdx[inPlatformIdx];
  return cap_idx;
}

//--------------------------------------------------------------
/**
 * �w��v���b�g�z�[���ɃJ�v�Z������Ԃ��Ă��邩�𒲂ׂ�
 * @param	
 * @return
 */
//--------------------------------------------------------------
static BOOL IsStopCapsuleToPlatForm(GYM_ELEC_SV_WORK *gmk_sv_work, const u8 inPlatformIdx)
{
  u8 i;
  u8 cap_idx;
  int platform_idx;

  //�w��v���b�g�z�[���̃J�v�Z���C���f�b�N�X���擾
  cap_idx = GetCapIdxByPlatformIdx(inPlatformIdx);
  //�擾�J�v�Z���C���f�b�N�X�̒�ԃv���b�g�z�[�����擾
  platform_idx = gmk_sv_work->StopPlatformIdx[cap_idx];
  OS_Printf("%d %d %d\n",platform_idx, cap_idx, gmk_sv_work->StopPlatformIdx[cap_idx]);
  if (platform_idx == PLATFORM_NO_STOP){
    //��Ԃ��Ă��Ȃ�
    return FALSE;
  }
  //�v���b�g�z�[���C���f�b�N�X��r
  if (platform_idx == inPlatformIdx){
    //�w��v���b�g�z�[���ɒ��
    return TRUE;
  }

  //�w��v���b�g�z�[���ɒ�Ԃ��Ă��Ȃ�
  return FALSE;
  
}

//--------------------------------------------------------------
/**
 * �w�背�[���̌��݃A�j���t���[�����擾
 * @param	
 * @return
 */
//--------------------------------------------------------------
static fx32 GetAnimeFrame(FLD_EXP_OBJ_CNT_PTR ptr, const u16 inObjIdx, const u16 inAnmIdx)
{
  BOOL rc;
  int frame = 0;
  GFL_G3D_OBJ *g3Dobj = FLD_EXP_OBJ_GetUnitObj(ptr, GYM_ELEC_UNIT_IDX, inObjIdx);
  rc = GFL_G3D_OBJECT_GetAnimeFrame( g3Dobj, inAnmIdx, &frame );
  GF_ASSERT(rc);
  return frame;
}

//--------------------------------------------------------------
/**
 * �w�背�[���̌��݃A�j���t���[�����擾
 * @param   ptr	        �g���n�a�i�R���g���[���|�C���^
 * @param   inObjIdx    OBJ�C���f�b�N�X
 * @param   inAnmIdx    �A�j���C���f�b�N�X
 * @param   inFrame     �A�j���t���[���@int�^�ł����A�����ł�fx32�^�ɃL���X�g����܂�
 *
 * @return  none
 */
//--------------------------------------------------------------
static void SetAnimeFrame(FLD_EXP_OBJ_CNT_PTR ptr, const u16 inObjIdx, const u16 inAnmIdx, const int inFrame)
{
  BOOL rc;
  GFL_G3D_OBJ *g3Dobj = FLD_EXP_OBJ_GetUnitObj(ptr, GYM_ELEC_UNIT_IDX, inObjIdx);
  rc = GFL_G3D_OBJECT_SetAnimeFrame( g3Dobj, inAnmIdx, &inFrame );
  GF_ASSERT(rc);
}

//--------------------------------------------------------------
/**
 * �ړ��J�v�Z���Ɏ��@�����Ă���悤�ɐ��䂷��s�b�a
 * @param	
 * @return
 */
//--------------------------------------------------------------
static BOOL CapsuleTraceTcb()
{
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���o�[�̐؂�ւ����s���C�x���g�N��
 * @param	
 * @return
 */
//--------------------------------------------------------------
GMEVENT *GYM_ELEC_ChangePoint(GAMESYS_WORK *gsys, const u8 inLeverIdx)
{
  GMEVENT * event;
  GYM_ELEC_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ELEC_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );

  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );

  //�C�x���g�R�[��
  event = GMEVENT_Create( gsys, NULL, ChangePointEvt, 0 );
    
  //����\�背�o�[�C���f�b�N�X���Z�b�g
  tmp->LeverIdx = inLeverIdx;

  return event;
}

//--------------------------------------------------------------
/**
 * ���o�[�̐؂�ւ����s���C�x���g�N��
 * @param	
 * @return
 */
//--------------------------------------------------------------
BOOL test_GYM_ELEC_ChangePoint(GAMESYS_WORK *gsys, const u8 inLeverIdx)
{
  GYM_ELEC_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ELEC_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );

  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );

  {
    //�C�x���g�Z�b�g
    GMEVENT * event = GMEVENT_Create( gsys, NULL, ChangePointEvt, 0/*sizeof(ENCEFF_WORK)*/ );
    GAMESYSTEM_SetEvent(gsys, event);
    //����\�背�o�[�C���f�b�N�X���Z�b�g
    tmp->LeverIdx = inLeverIdx;
    //GMEVENT_CallEvent(GMEVENT * parent, GMEVENT * child);
  }
  return TRUE;
  
}

//--------------------------------------------------------------
/**
 * ���o�[�̐؂�ւ��C�x���g
 * @param	
 * @return
 */
//--------------------------------------------------------------
static GMEVENT_RESULT ChangePointEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_ELEC_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ELEC_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );
  }
  
  switch(*seq){
  case 0:  //���o�[�A�j���؂�ւ�
    {      
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 sw_obj_idx;
      u8 rl_obj_idx;
      tmp->ChgingSw = gmk_sv_work->LeverSw[tmp->LeverIdx];
      sw_obj_idx = OBJ_SW_1 + tmp->LeverIdx;
      rl_obj_idx = OBJ_RL_1 + tmp->LeverIdx;
    //����g�p�Ƃ��Ă��郌�o�[�C���f�b�N�X���猻�݂̓|����������āA�t�̃A�j�����Đ�����
      if (tmp->ChgingSw==0){
        //�A�j���𖳌�
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, sw_obj_idx, 0, FALSE);
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, rl_obj_idx, 0, FALSE);
        //�A�j�����J�n
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, sw_obj_idx, 1, TRUE);
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, rl_obj_idx, 1, TRUE);

        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, sw_obj_idx, 1);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, rl_obj_idx, 1);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
        //���o��
        FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_ELEC_UNIT_IDX, sw_obj_idx, 1, 0 );
        FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_ELEC_UNIT_IDX, rl_obj_idx, 1, 0 );
        tmp->ChgingSw = 1;
      }else{
        //�A�j���𖳌�
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, sw_obj_idx, 1, FALSE);
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, rl_obj_idx, 1, FALSE);
        //�A�j�����J�n
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, sw_obj_idx, 0, TRUE);
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, rl_obj_idx, 0, TRUE);
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, sw_obj_idx, 0);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, rl_obj_idx, 0);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
        //���o��
        FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_ELEC_UNIT_IDX, sw_obj_idx, 0, 0 );
        FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_ELEC_UNIT_IDX, rl_obj_idx, 0, 0 );
        tmp->ChgingSw = 0;
      }
    }    
    (*seq)++;
    break;
  case 1: //�A�j���҂�
#if 1    
    {
      
      u8 sw_obj_idx;
      u8 rl_obj_idx;
      EXP_OBJ_ANM_CNT_PTR sw_anm;
      EXP_OBJ_ANM_CNT_PTR rl_anm;
      sw_obj_idx = OBJ_SW_1 + tmp->LeverIdx;
      rl_obj_idx = OBJ_RL_1 + tmp->LeverIdx;
      sw_anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, sw_obj_idx, tmp->ChgingSw);
      rl_anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, rl_obj_idx, tmp->ChgingSw);
      if ( FLD_EXP_OBJ_ChkAnmEnd(sw_anm)&&FLD_EXP_OBJ_ChkAnmEnd(rl_anm) ){
        OS_Printf("�A�j���I��\n");
        //�H���|�C���g�؂�ւ��i�A�j���؂�ւ��j
        ChgRale(fieldWork, ptr);
        return GMEVENT_RES_FINISH;
      }
    }
#else
    {
      //�H���|�C���g�؂�ւ��i�A�j���؂�ւ��j
      ChgRale(fieldWork, ptr);
      return GMEVENT_RES_FINISH;
    }
#endif
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �v���b�g�z�[���Ԃ��ړ����Ă���Ԃ̃C�x���g
 * @param	
 * @return
 */
//--------------------------------------------------------------
GMEVENT *GYM_ELEC_CreateMoveEvt(GAMESYS_WORK *gsys)
{
  GMEVENT * event;
  GYM_ELEC_TMP *tmp_work;
  u8 platform_idx;
  
  GYM_ELEC_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );
  tmp_work = GMK_TMP_WK_GetWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);

  {
    int x,z;
    VecFx32 pos;
    FIELD_PLAYER *fld_player;
    fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    //���@��������łȂ��ꍇ�͏I��
    if ( FIELD_PLAYER_GetDir( fld_player ) != DIR_UP ){
      return NULL;
    }

    FIELD_PLAYER_GetPos( fld_player, &pos );
    x = pos.x / FIELD_CONST_GRID_FX32_SIZE;
    z = pos.z / FIELD_CONST_GRID_FX32_SIZE;
    //���@�̈ʒu����A�v���b�g�t�H�[���ԍ��𒲂ׂ�
    platform_idx = GetPlatformIdx(x,z);
    OS_Printf("platform = %d\n",platform_idx);
  }

  //�v���b�g�z�[����������Ȃ��ꍇ�͏I��
  if (platform_idx == PLATFORM_NONE){
    return NULL;
  }
  
  //�J�v�Z�����v���b�g�t�H�[���ɒ�~���Ă��邩�H
  if ( IsStopCapsuleToPlatForm(gmk_sv_work, platform_idx) ){ //��~���Ă���
    u8 cap_idx = GetCapIdxByPlatformIdx(platform_idx);
    //�J�v�Z�����g���[�i�[�G���J�E���g�`�F�b�N
    if ( CheckCapTrEnc(gmk_sv_work, cap_idx) ){
      //�J�v�Z�����g���[�i�[�Ƃ̃G���J�E���g�C�x���g�쐬
      event = GMEVENT_Create( gsys, NULL, TrEncEvt, 0 );
    }else{
      //���̃v���b�g�t�H�[���܂ł̈ړ��C�x���g���쐬����
      event = GMEVENT_Create( gsys, NULL, CapMoveEvt, 0 );
    }

    {
      //���@�����s���郌�[���C���f�b�N�X��ۑ�
      tmp_work->RadeRaleIdx = GetRaleIdxByPlatformIdx(gmk_sv_work, platform_idx);
      OS_Printf("RideRaleIdx %d\n",tmp_work->RadeRaleIdx);
      //�J�v�Z���ړ��A�j�����~
      {
        FLD_EXP_OBJ_CNT_PTR obj_cnt_ptr;
        EXP_OBJ_ANM_CNT_PTR anm;
        u8 obj_idx = OBJ_CAP_1+cap_idx;
        u8 anm_idx = ANM_CAP_MOV1 + tmp_work->RadeRaleIdx;
        obj_cnt_ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
        anm = FLD_EXP_OBJ_GetAnmCnt( obj_cnt_ptr, GYM_ELEC_UNIT_IDX, obj_idx, anm_idx);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
        //��~�J�n
        tmp_work->RideEvt = cap_idx;
      }
    }

    //������t���O�I��
    gmk_sv_work->RideFlg[cap_idx] = 1;

    return event;
  }
  return NULL;
}

static BOOL CheckCapTrEnc(GYM_ELEC_SV_WORK *gmk_sv_work, const u8 inCapIdx)
{
  u8 check = 0;
  if (inCapIdx == 1){
    check = 1;
  }else if(inCapIdx == 2){
    check =1;
  }

  if (check){
    if ( !gmk_sv_work->RideFlg[inCapIdx] ){
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * �v���b�g�z�[���Ԃ��ړ����Ă���Ԃ̃C�x���g
 * @param	
 * @return
 */
//--------------------------------------------------------------
BOOL test_GYM_ELEC_CallMoveEvt(GAMESYS_WORK *gsys)
{
  GYM_ELEC_TMP *tmp_work;
  u8 platform_idx;
  int x,z;
  GYM_ELEC_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );
  tmp_work = GMK_TMP_WK_GetWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);

  platform_idx = 8;

  //�J�v�Z�����v���b�g�t�H�[���ɒ�~���Ă��邩�H
  if ( IsStopCapsuleToPlatForm(gmk_sv_work, platform_idx) ){ //��~���Ă���
    //���̃v���b�g�t�H�[���܂ł̈ړ��C�x���g���쐬����
    GMEVENT * event = GMEVENT_Create( gsys, NULL, CapMoveEvt, 0 );
    GAMESYSTEM_SetEvent(gsys, event);

    //���@�����s���郌�[���C���f�b�N�X��ۑ�
    tmp_work->RadeRaleIdx = GetRaleIdxByPlatformIdx(gmk_sv_work, platform_idx);
    //�J�v�Z���ړ��A�j�����~
    {
      FLD_EXP_OBJ_CNT_PTR obj_cnt_ptr;
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 cap_idx = GetCapIdxByPlatformIdx(platform_idx);
      u8 obj_idx = OBJ_CAP_1+cap_idx;
      u8 anm_idx = ANM_CAP_MOV1 + tmp_work->RadeRaleIdx;
      obj_cnt_ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
      anm = FLD_EXP_OBJ_GetAnmCnt( obj_cnt_ptr, GYM_ELEC_UNIT_IDX, obj_idx, anm_idx);
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    }

    return TRUE;
  }
  return FALSE;
}

static GMEVENT_RESULT CapMoveEvt(GMEVENT* event, int* seq, void* work)
{
  GYM_ELEC_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ELEC_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);

  //���[���C���f�b�N�X/2�ŃJ�v�Z���C���f�b�N�X�ɂȂ�
  u8 cap_idx = tmp->RadeRaleIdx / 2;
  u8 obj_idx = OBJ_CAP_1+cap_idx;

  EXP_OBJ_ANM_CNT_PTR anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, ANM_CAP_OPCL);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );
  }

  switch(*seq){
  case 0:
    {
      u16 arc_idx;
      //���@�ړ��f�[�^���[�h
      OS_Printf("���[�h���[��%d\n",tmp->RadeRaleIdx);
      arc_idx = RaleAnm[tmp->RadeRaleIdx];
      tmp->IcaAnmPtr = ICA_ANIME_Create( 
          GFL_HEAP_LOWID(HEAPID_FIELDMAP), ARCID_GYM_ELEC, arc_idx, tmp->FramePosDat, FRAME_POS_SIZE
          );

      GF_ASSERT(tmp->IcaAnmPtr != NULL);

      //�J�v�Z���J���A�j���X�^�[�g
      {
        //�t���[��������
        SetAnimeFrame(ptr, obj_idx, ANM_CAP_OPCL, 0);
        //�Đ�
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
      }
    }
    (*seq)++;
    break;
  case 1:
    {
      fx32 frame;
      frame = GetAnimeFrame(ptr, obj_idx, ANM_CAP_OPCL);
      //�J�v�Z���J���A�j���҂�
      if (frame >= CAP_OPEN_FRAME){
        //�J�v�Z���J���A�j���~�߂�
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
        //��l���J�v�Z���ɓ���A�j��(��Ɉړ�)
        {
          MMDL * mmdl;
          mmdl = FIELD_PLAYER_GetMMdl( FIELDMAP_GetFieldPlayer( fieldWork ) );
          MMDL_SetAcmd( mmdl, AC_WALK_U_8F );
        }
        (*seq)++;
      }
    }
    break;
  case 2:
    {
      MMDL * mmdl;
      mmdl = FIELD_PLAYER_GetMMdl( FIELDMAP_GetFieldPlayer( fieldWork ) );
      //��l���A�j���I���҂�
      if( MMDL_CheckEndAcmd(mmdl) ){
        //��l������
        MMDL_SetStatusBitVanish(mmdl, TRUE);

        //�J�v�Z���܂�A�j���X�^�[�g
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
        (*seq)++;
      }
    }
    break;
  case 3:
    //�J�v�Z���܂�A�j���҂�
    if( FLD_EXP_OBJ_ChkAnmEnd(anm) ){
      EXP_OBJ_ANM_CNT_PTR cap_anm;
      u8 anm_idx;
      //�J�v�Z���W�A�j���~�߂�
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
      //�J�v�Z���ړ��A�j���ĊJ
      anm_idx = ANM_CAP_MOV1 + tmp->RadeRaleIdx;
      cap_anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, anm_idx);
      FLD_EXP_OBJ_ChgAnmStopFlg(cap_anm, 0);

      //�^�C�}�[�N���A
      tmp->TaskWk[cap_idx].Timer = 0;
      gmk_sv_work->StopPlatformIdx[cap_idx] = PLATFORM_NO_STOP;

      //��~�I��
      tmp->RideEvt = -1;

      //���@�����ړ��J�n
      tmp->AltoMove = TRUE;

      (*seq)++;
    }
    break;
  case 4:
    //�v���b�g�z�[�������҂�
    if( gmk_sv_work->StopPlatformIdx[cap_idx] != PLATFORM_NO_STOP ){
      //�v���[�����͑҂�
      if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN ){   //���͂���
        (*seq)++;
      }
    }
    break;
  case 5:
    {
      EXP_OBJ_ANM_CNT_PTR cap_anm;
      u8 anm_idx = ANM_CAP_MOV1 + tmp->RadeRaleIdx;
      cap_anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, anm_idx);

      //��~�J�n
      tmp->RideEvt = cap_idx;
      //�J�v�Z���ړ��A�j����~
      FLD_EXP_OBJ_ChgAnmStopFlg(cap_anm, 1);
      //�J�v�Z���J���A�j���X�^�[�g
      {
        //�t���[��������
        SetAnimeFrame(ptr, obj_idx, ANM_CAP_OPCL, 0);
        //�Đ�
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
      }
    }
    (*seq)++;
    break;
  case 6:
    {
      fx32 frame;
      frame = GetAnimeFrame(ptr, obj_idx, ANM_CAP_OPCL);
      //�J�v�Z���J���A�j���҂�
      if (frame >= CAP_OPEN_FRAME){
        //�J�v�Z���J���A�j���~�߂�
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
        {
          FIELD_PLAYER *fld_player;
          fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
          //��l���̃J�v�Z���o����W�Z�b�g
          {
            u8 platform_idx;
            VecFx32 pos = {0,OBJ3D_Y,0};
            platform_idx = gmk_sv_work->StopPlatformIdx[cap_idx];
            pos.x = PlatformXZ[platform_idx][0] * FIELD_CONST_GRID_FX32_SIZE;
            pos.x += GRID_HALF_SIZE;
            pos.z = (PlatformXZ[platform_idx][1]-1) * FIELD_CONST_GRID_FX32_SIZE;
            pos.z += GRID_HALF_SIZE;
            FIELD_PLAYER_SetPos( fld_player, &pos );
          }
        }
        //���@�����ړ��I��
        tmp->AltoMove = FALSE;
        (*seq)++;
      }
    }
    break;
  case 7:
    {
      MMDL * mmdl;
      FIELD_PLAYER *fld_player;
      fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
      mmdl = FIELD_PLAYER_GetMMdl( fld_player );
      //��l���\��
      MMDL_SetStatusBitVanish(mmdl, FALSE);
      //��l���J�v�Z������ł�A�j��
      MMDL_SetAcmd( mmdl, AC_WALK_D_8F );
    }
    (*seq)++;
    break;
  case 8:
    {
      MMDL * mmdl;
      mmdl = FIELD_PLAYER_GetMMdl( FIELDMAP_GetFieldPlayer( fieldWork ) );
      //��l���A�j���I���҂�
      if( MMDL_CheckEndAcmd(mmdl) ){
        //�J�v�Z���܂�A�j���X�^�[�g
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
        (*seq)++;
      }
    }
    break;
  case 9:
    //�J�v�Z���܂�A�j���҂�
    if( FLD_EXP_OBJ_ChkAnmEnd(anm) ){
      EXP_OBJ_ANM_CNT_PTR cap_anm;
      u8 anm_idx;
      //�J�v�Z���W�A�j���~�߂�
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
      //�J�v�Z���ړ��A�j���ĊJ
      anm_idx = ANM_CAP_MOV1 + tmp->RadeRaleIdx;
      cap_anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, anm_idx);
      FLD_EXP_OBJ_ChgAnmStopFlg(cap_anm, 0);

      //�^�C�}�[�N���A
      tmp->TaskWk[cap_idx].Timer = 0;
      gmk_sv_work->StopPlatformIdx[cap_idx] = PLATFORM_NO_STOP;

      ICA_ANIME_Delete( tmp->IcaAnmPtr );
      
      //��~�I��
      tmp->RideEvt = -1;
      return GMEVENT_RES_FINISH;
    }
  }
  return GMEVENT_RES_CONTINUE;
}

//�g���[�i�[��C�x���g
static GMEVENT_RESULT TrEncEvt(GMEVENT* event, int* seq, void* work)
{
  GYM_ELEC_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ELEC_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);

  //���[���C���f�b�N�X/2�ŃJ�v�Z���C���f�b�N�X�ɂȂ�
  u8 cap_idx = tmp->RadeRaleIdx / 2;
  u8 obj_idx = OBJ_CAP_1+cap_idx;

  EXP_OBJ_ANM_CNT_PTR anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, ANM_CAP_OPCL);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );
  }

  switch(*seq){
  case 0:
    //�J�v�Z���J���A�j���X�^�[�g
      {
        //�t���[��������
        SetAnimeFrame(ptr, obj_idx, ANM_CAP_OPCL, 0);
        //�Đ�
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
      }
      (*seq)++;
    break;
  case 1:
    {
      fx32 frame;
      frame = GetAnimeFrame(ptr, obj_idx, ANM_CAP_OPCL);
      //�J�v�Z���J���A�j���҂�
      if (frame >= CAP_OPEN_FRAME){
        //�J�v�Z���J���A�j���~�߂�
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
        //�A�j���X�N���v�g�R�[��
        OS_Printf("�X�N���v�g�R�[��\n");
        SCRIPT_CallScript( event, SCRID_GIMMICK_C04GYM0101_SCR01,
          NULL, NULL, GFL_HEAP_LOWID(HEAPID_FIELDMAP) );
        (*seq)++;
      }      
    }
    break;
  case 2:
    //�J�v�Z���܂�A�j���X�^�[�g
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
    (*seq)++;
    break;
  case 3:
    //�J�v�Z���܂�A�j���҂�
    if( FLD_EXP_OBJ_ChkAnmEnd(anm) ){
      EXP_OBJ_ANM_CNT_PTR cap_anm;
      u8 anm_idx;
      //�J�v�Z���W�A�j���~�߂�
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
      //�J�v�Z���ړ��A�j���ĊJ
      anm_idx = ANM_CAP_MOV1 + tmp->RadeRaleIdx;
      cap_anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, anm_idx);
      FLD_EXP_OBJ_ChgAnmStopFlg(cap_anm, 0);

      //�^�C�}�[�N���A
      tmp->TaskWk[cap_idx].Timer = 0;
      gmk_sv_work->StopPlatformIdx[cap_idx] = PLATFORM_NO_STOP;

      //��~�I��
      tmp->RideEvt = -1;

      //�X�N���v�g�R�[��
      OS_Printf("�X�N���v�g�R�[��\n");
      SCRIPT_CallScript( event, SCRID_GIMMICK_C04GYM0101_SCR02,
          NULL, NULL, GFL_HEAP_LOWID(HEAPID_FIELDMAP) );
      (*seq)++;
    }
    break;
  case 4:
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

static void ChgRale(FIELDMAP_WORK *fieldWork, FLD_EXP_OBJ_CNT_PTR ptr)
{
  u8 cap_idx;
  u8 obj;
  u8 sw;
  GYM_ELEC_SV_WORK *gmk_sv_work;
  GYM_ELEC_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ELEC_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );

  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ELEC );
  //���삵�悤�Ƃ��Ă��郌�o�[�̃X�C�b�`���ǂ���ɓ����Ă��邩�𒲂ׂ�  
  sw = gmk_sv_work->LeverSw[tmp->LeverIdx];
  obj = OBJ_CAP_1+tmp->LeverIdx;
  cap_idx = tmp->LeverIdx;    //�X�C�b�`�C���f�b�N�X���J�v�Z���C���f�b�N�X�ɑΉ�

  //�Z�[�u�f�[�^�̃X�C�b�`�؂�ւ�
  gmk_sv_work->LeverSw[tmp->LeverIdx] = (gmk_sv_work->LeverSw[tmp->LeverIdx]+1)%2;
  //�����Ƀ��[���A�j����؂�ւ�����Ȃ�Ες���
  if ( CheckChangableRaleAtOnce(ptr, cap_idx, gmk_sv_work->NowRaleIdx[tmp->LeverIdx]) ){
    ChgRaleAnm(ptr, sw, tmp->LeverIdx);
    //���s���[���ۑ�
    gmk_sv_work->NowRaleIdx[tmp->LeverIdx] =
      (tmp->LeverIdx*2)+gmk_sv_work->LeverSw[tmp->LeverIdx];
  }else{
    //�ς����Ȃ��ꍇ�̓��N�G�X�g���o��
    if (gmk_sv_work->RaleChgReq[tmp->LeverIdx]){
      //���Ƀ��N�G�X�g����Ă���Ȃ�΁A���N�G�X�g���L�����Z��
      gmk_sv_work->RaleChgReq[tmp->LeverIdx] = 0;
    }else{
      //���N�G�X�g����Ă��Ȃ��Ȃ�΁A���N�G�X�g����
      gmk_sv_work->RaleChgReq[tmp->LeverIdx] = 1;
    }
  }
}

static void ChgRaleAnm(FLD_EXP_OBJ_CNT_PTR ptr, const u8 inSw, const u8 inCapIdx)
{
  u8 obj_idx;
  u8 rale_anm_before;
  u8 rale_anm_next;
  fx32 frame;
  if (inSw==0){   //���݁A�f�t�H���g
    rale_anm_before = ANM_CAP_MOV1+(inCapIdx*2);
    rale_anm_next = rale_anm_before+1;
  }else{        //���݁A��f�t�H���g
    rale_anm_next = ANM_CAP_MOV1+(inCapIdx*2);
    rale_anm_before = rale_anm_next+1;
  }
  obj_idx = OBJ_CAP_1+inCapIdx;
  frame = GetAnimeFrame(ptr, obj_idx, rale_anm_before);
  FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, obj_idx, rale_anm_before, FALSE);
  FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ELEC_UNIT_IDX, obj_idx, rale_anm_next, TRUE);
  //�t���[���̈��p��
  SetAnimeFrame(ptr, obj_idx, rale_anm_next, frame);
  //�X�g�b�v�t���O���p��
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    u8 stop;
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, rale_anm_before);
    stop = FLD_EXP_OBJ_GetAnmStopFlg(anm);
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ELEC_UNIT_IDX, obj_idx, rale_anm_next);
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, stop);
  }

}

static BOOL CheckCapStopFrame(const u8 inRaleIdx, const fx32 inFrm, u8 *outStopIdx)
{
  u8 i;
  for (i=0;i<RALE_PLATFORM_NUM_MAX;i++){
    if ( inFrm == StopFrame[inRaleIdx][i] ){
      *outStopIdx = i;
      return TRUE;
    }
  }

  return FALSE;
}

//�����[���A�j���؂�ւ��ł��邩�H
static BOOL CheckChangableRaleAtOnce(FLD_EXP_OBJ_CNT_PTR ptr, const u8 inCapIdx, const u8 inRaleIdx)
{
  fx32 frame;
  //�w�背�[���C���f�b�N�X�̃��[���A�j���̌��݃A�j���t���[�����擾
  {
    u8 obj_idx;
    u16 anm_idx;
    //���o�[�ԍ����J�v�Z���ԍ�
    obj_idx = OBJ_CAP_1+inCapIdx;
    anm_idx = ANM_CAP_MOV1 + inRaleIdx;
    //�w�背�[���̃A�j���t���[�����擾
    frame = GetAnimeFrame(ptr, obj_idx, anm_idx);
    OS_Printf("frame=%x,%d\n",frame, frame/FX32_ONE);
  }
  
  if (frame <= CommFrame[inCapIdx]*FX32_ONE){
    return TRUE;
  }
  return FALSE;
}
