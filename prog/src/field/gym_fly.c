//======================================================================
/**
 * @file  gym_fly.c
 * @brief  ��s�W��
 * @author  Saito
 */
//======================================================================
#include "fieldmap.h"
#include "gym_fly_sv.h"
#include "gym_fly.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"
#include "system/ica_anime.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/fieldmap/gym_fly.naix"

#include "system/main.h"    //for HEAPID_FIELDMAP
#include "script.h"     //for SCRIPT_CallScript
#include "../../../resource/fldmapdata/script/c06gym0101_def.h"  //for SCRID_�`



#define GYM_FLY_UNIT_IDX (0)
#define GYM_FLY_TMP_ASSIGN_ID  (1)

#define FRAME_POS_SIZE  (4*3*150)   //���W�w�x�y�e4�o�C�g��150�t���[��
#define HEADER_SIZE  (8)   //�t���[���T�C�Y4�o�C�g+�i�[���3�o�C�g�̃w�b�_�[��񂪂��邽�߃A���C�����g����������8�o�C�g�m�ۂ���

#define CANNON_NUM_MAX  (10)

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)

//�C��O���b�h���W
#define CAN1_GX (15)
#define CAN1_GY (0)
#define CAN1_GZ (28)
#define CAN2_GX (15)
#define CAN2_GY (0)
#define CAN2_GZ (9)
#define CAN3_GX (23)
#define CAN3_GY (0)
#define CAN3_GZ (17)
#define CAN4_GX (20)
#define CAN4_GY (0)
#define CAN4_GZ (9)
#define CAN5_GX (11)
#define CAN5_GY (0)
#define CAN5_GZ (13)
#define CAN6_GX (8)
#define CAN6_GY (0)
#define CAN6_GZ (4)
#define CAN7_GX (28)
#define CAN7_GY (0)
#define CAN7_GZ (4)
#define CAN8_GX (26)
#define CAN8_GY (0)
#define CAN8_GZ (22)
#define CAN9_GX (6)
#define CAN9_GY (0)
#define CAN9_GZ (20)
#define CAN10_GX (2)
#define CAN10_GY (0)
#define CAN10_GZ (13)


//�C����W
#define CAN1_X  (CAN1_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN1_Y  (CAN1_GY*FIELD_CONST_GRID_FX32_SIZE)
#define CAN1_Z  (CAN1_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

#define CAN2_X  (CAN2_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN2_Y  (1*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN2_Z  (CAN2_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

#define CAN3_X  (CAN3_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN3_Y  (1*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN3_Z  (CAN3_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

#define CAN4_X  (CAN4_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN4_Y  (1*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN4_Z  (CAN4_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

#define CAN5_X  (CAN5_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN5_Y  (1*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN5_Z  (CAN5_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

#define CAN6_X  (CAN6_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN6_Y  (1*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN6_Z  (CAN6_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

#define CAN7_X  (CAN7_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN7_Y  (1*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN7_Z  (CAN7_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

#define CAN8_X  (CAN8_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN8_Y  (1*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN8_Z  (CAN8_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

#define CAN9_X  (CAN9_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN9_Y  (1*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN9_Z  (CAN9_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

#define CAN10_X  (CAN10_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN10_Y  (1*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN10_Z  (CAN10_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

//���n���W
#define STAND1_GX (15)
#define STAND1_GY (2)
#define STAND1_GZ (19)

//�W���������̈ꎞ���[�N
typedef struct GYM_FLY_TMP_tag
{
  u8 ShotIdx;
  u8 ShotDir;
  u8 FrameSetStart;
  u8 dummy;

  u8 FramePosDat[FRAME_POS_SIZE+HEADER_SIZE];
  ICA_ANIME* IcaAnmPtr;
  u16 NowIcaAnmFrmIdx;
  u16 MaxIcaAnmFrm;
}GYM_FLY_TMP;

//���\�[�X�̕��я���
enum {
  RES_ID_CAN_MDL = 0,
  RES_ID_CAN_ANM1_ICA,
  RES_ID_CAN_ANM1_IMA,
  RES_ID_CAN_ANM2_ICA,
  RES_ID_CAN_ANM2_IMA,
  RES_ID_CAN_ANM3_ICA,
  RES_ID_CAN_ANM3_IMA,
  RES_ID_CAN_ANM4_ICA,
  RES_ID_CAN_ANM4_IMA,
};

//�n�a�i�C���f�b�N�X
enum {
  OBJ_CAN_1 = 0,
  OBJ_CAN_2,
  OBJ_CAN_3,
  OBJ_CAN_4,
  OBJ_CAN_5,
  OBJ_CAN_6,
  OBJ_CAN_7,
  OBJ_CAN_8,
  OBJ_CAN_9,
  OBJ_CAN_10,
};

//�A�j���̏���
enum{
  ANM_CAN_ANM1 = 0,    //��̉�����
  ANM_CAN_ANM2,    //��̉E����
  ANM_CAN_ANM3,    //��̏ォ��
  ANM_CAN_ANM4,    //��̍�����
};

//��C���Ƃ̊����̉�]
static u8 CannoneRot[CANNON_NUM_MAX] = {
  0,    //��]�Ȃ�      �����
  0,    //��]�Ȃ�      �����
  1,    //�����v90�x    ������
  3,    //�����v270�x   �E����
  1,    //�����v90�x    ������
  2,    //�����v180�x   ������
  3,    //�����v270�x   �E����
  0,    //��]�Ȃ�      �����
  1,    //�����v90�x    ������
  2,    //�����v180�x   ������
};

//��C�ʒu
static const u16 CanPos[CANNON_NUM_MAX][3] = {
  {CAN1_GX, CAN1_GY, CAN1_GZ},
  {CAN2_GX, CAN2_GY, CAN2_GZ},
  {CAN3_GX, CAN3_GY, CAN3_GZ},
  {CAN4_GX, CAN4_GY, CAN4_GZ},
  {CAN5_GX, CAN5_GY, CAN5_GZ},
  {CAN6_GX, CAN6_GY, CAN6_GZ},
  {CAN7_GX, CAN7_GY, CAN7_GZ},
  {CAN8_GX, CAN8_GY, CAN8_GZ},
  {CAN9_GX, CAN9_GY, CAN9_GZ},
  {CAN10_GX, CAN10_GY, CAN10_GZ},

};

//--���\�[�X�֘A--
//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {  
	{ ARCID_GYM_FLY, NARC_gym_fly_cannon_01_00_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@��C�{��
  
  { ARCID_GYM_FLY, NARC_gym_fly_cannon_01_00_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@��C�A�j��1
  { ARCID_GYM_FLY, NARC_gym_fly_cannon_01_00_nsbma, GFL_G3D_UTIL_RESARC }, //IMA�@��C�A�j��1
  { ARCID_GYM_FLY, NARC_gym_fly_cannon_01_00_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@��C�A�j��2
  { ARCID_GYM_FLY, NARC_gym_fly_cannon_01_00_nsbma, GFL_G3D_UTIL_RESARC }, //IMA�@��C�A�j��2
  { ARCID_GYM_FLY, NARC_gym_fly_cannon_01_00_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@��C�A�j��3
  { ARCID_GYM_FLY, NARC_gym_fly_cannon_01_00_nsbma, GFL_G3D_UTIL_RESARC }, //IMA�@��C�A�j��3
  { ARCID_GYM_FLY, NARC_gym_fly_cannon_01_00_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@��C�A�j��4
  { ARCID_GYM_FLY, NARC_gym_fly_cannon_01_00_nsbma, GFL_G3D_UTIL_RESARC }, //IMA�@��C�A�j��4
#if 0  
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@��C�A�j��2
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02b_nsbca, GFL_G3D_UTIL_RESARC }, //ITP�@��C�A�j��2
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@��C�A�j��3
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02b_nsbca, GFL_G3D_UTIL_RESARC }, //ITP�@��C�A�j��3
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@��C�A�j��4
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02b_nsbca, GFL_G3D_UTIL_RESARC }, //ITP�@��C�A�j��4
#endif
  { 0, 0, GFL_G3D_UTIL_RESARC },
};

//3D�A�j���@��C
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl[] = {
	{ RES_ID_CAN_ANM1_ICA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM1_IMA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM2_ICA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM2_IMA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM3_ICA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM3_IMA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM4_ICA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM4_IMA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
  //��C1
	{
		RES_ID_CAN_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_CAN_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl),	//�A�j�����\�[�X��
	},
  //��C2
  {
		RES_ID_CAN_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_CAN_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl),	//�A�j�����\�[�X��
	},
  //��C3
  {
		RES_ID_CAN_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_CAN_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl),	//�A�j�����\�[�X��
	},
  //��C4
  {
		RES_ID_CAN_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_CAN_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl),	//�A�j�����\�[�X��
	},
};

static const GFL_G3D_UTIL_SETUP Setup = {
  g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};

static GMEVENT_RESULT ShotEvt( GMEVENT* event, int* seq, void* work );
static u8 GetCanIdx(const int inX, const int inZ);

//@todo
BOOL test_GYM_FLY_Shot(GAMESYS_WORK *gsys, const u8 inShotIdx, const u8 inShotDir);

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_FLY_Setup(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldWork, GYM_FLY_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_FLY_TMP));

  //�K�v�ȃ��\�[�X�̗p��
  FLD_EXP_OBJ_AddUnit(ptr, &Setup, GYM_FLY_UNIT_IDX );

  //���W�Z�b�g�@��C
  {
    VecFx32 pos = {CAN1_X, CAN1_Y, CAN1_Z};
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_FLY_UNIT_IDX, OBJ_CAN_1);
    status->trans = pos;
  }

  {
    //1��Đ��ݒ�
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_FLY_UNIT_IDX, OBJ_CAN_1, 0);
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_FLY_UNIT_IDX, OBJ_CAN_1, 1);
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
  }

  
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_FLY_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GYM_FLY_TMP_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, GYM_FLY_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_FLY_Move(FIELDMAP_WORK *fieldWork)
{
  int i;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�e�X�g
  {
    GAMESYS_WORK *gsys  = FIELDMAP_GetGameSysWork( fieldWork );
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){
      test_GYM_FLY_Shot(gsys, 0, 0);
    }
  } 

  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );

}

//--------------------------------------------------------------
/**
 * �ł��o���C�x���g�N��
 * @param	
 * @return
 */
//--------------------------------------------------------------
GMEVENT *GYM_FLY_Shot(GAMESYS_WORK *gsys, const u8 inShotIdx, const u8 inShotDir)
{
  GMEVENT * event;
  GYM_FLY_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_FLY_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_FLY_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );

  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_FLY );

  //�C�x���g�R�[��
  event = GMEVENT_Create( gsys, NULL, ShotEvt, 0 );
    
  //�ł��o���C���f�b�N�X���Z�b�g
  tmp->ShotIdx = inShotIdx;
  //�ł��o���������Z�b�g
  tmp->ShotDir = inShotDir;

  return event;

}

//--------------------------------------------------------------
/**
 * �����C�x���g�N��
 * @param	
 * @return
 */
//--------------------------------------------------------------
BOOL test_GYM_FLY_Shot(GAMESYS_WORK *gsys, const u8 inShotIdx, const u8 inShotDir)
{
  GYM_FLY_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_FLY_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_FLY_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );

  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_FLY );

  {
    //�C�x���g�Z�b�g
    GMEVENT * event = GMEVENT_Create( gsys, NULL, ShotEvt, 0/*sizeof(ENCEFF_WORK)*/ );
    GAMESYSTEM_SetEvent(gsys, event);
    //�ł��o���C���f�b�N�X���Z�b�g
    tmp->ShotIdx = inShotIdx;
    //�ł��o���������Z�b�g
    tmp->ShotDir = inShotDir;
  }
  return TRUE;
  
}

//--------------------------------------------------------------
/**
 * ���˃C�x���g
 * @param	
 * @return
 */
//--------------------------------------------------------------
static GMEVENT_RESULT ShotEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_FLY_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_FLY_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_FLY_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_FLY );
  }

  switch(*seq){
  case 0:
    {
      u16 arc_idx;
      arc_idx = 3;
      tmp->IcaAnmPtr = ICA_ANIME_Create( 
          GFL_HEAP_LOWID(HEAPID_FIELDMAP), ARCID_GYM_FLY, arc_idx, tmp->FramePosDat, FRAME_POS_SIZE
          );
      GF_ASSERT(tmp->IcaAnmPtr != NULL);
      tmp->MaxIcaAnmFrm = ICA_ANIME_GetMaxFrame(tmp->IcaAnmPtr);
    }

    //�ړ��f�[�^���[�h
    ;
    //���@�̌����ɂ�蕪��
    //��C�h�A�I�[�v���t���[���܂ōĐ��J�n
    ;
    {
      ///EXP_OBJ_ANM_CNT_PTR anm;
      u8 obj_idx;
      obj_idx = tmp->ShotIdx;
      //�A�j�����J�n
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_FLY_UNIT_IDX, obj_idx, 0, TRUE);
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_FLY_UNIT_IDX, obj_idx, 1, TRUE);
      //���o��
      FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_FLY_UNIT_IDX, obj_idx, 0, 0 );
      FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_FLY_UNIT_IDX, obj_idx, 1, 0 );  
    }
    (*seq)++;
    break;
  case 1:
    {
      u8 obj_idx;
      fx32 frm;
      obj_idx = tmp->ShotIdx;
      frm = FLD_EXP_OBJ_GetObjAnmFrm(ptr, GYM_FLY_UNIT_IDX, obj_idx, 0);
      //�I�[�v���t���[�����B�Ď�
      if (frm >= 15*FX32_ONE){
        //��C�A�j����~
        {
          EXP_OBJ_ANM_CNT_PTR anm;
          anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_FLY_UNIT_IDX, obj_idx, 0);
          FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
          anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_FLY_UNIT_IDX, obj_idx, 1);
          FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
        }
        //���@�A��C�ɓ���
        OS_Printf("�X�N���v�g�R�[��\n");
        SCRIPT_CallScript( event, SCRID_PRG_C06GYM0101_PLAYER_IN,
          NULL, NULL, GFL_HEAP_LOWID(HEAPID_FIELDMAP) );
        (*seq)++;
      }
    }
    break;
  case 2:
    //���@�A�j���I���҂�
    if(1){
      {
        MMDL * mmdl;
        FIELD_PLAYER *fld_player;
        fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
        mmdl = FIELD_PLAYER_GetMMdl( fld_player );
        //���@���\��
        MMDL_SetStatusBitVanish(mmdl, TRUE);
        //���@�̍����擾���֎~
        MMDL_OnStatusBit( mmdl, MMDL_STABIT_HEIGHT_GET_OFF );
      }
      //��C�A�j���ĊJ
      {
        u8 obj_idx;
        EXP_OBJ_ANM_CNT_PTR anm;
        obj_idx = tmp->ShotIdx;
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_FLY_UNIT_IDX, obj_idx, 0);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_FLY_UNIT_IDX, obj_idx, 1);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
      }
      (*seq)++;
    }
    break;
  case 3:
    {
      u8 obj_idx;
      fx32 frm;
      obj_idx = tmp->ShotIdx;
      frm = FLD_EXP_OBJ_GetObjAnmFrm(ptr, GYM_FLY_UNIT_IDX, obj_idx, 0);
      //���˃t���[�����B�`�F�b�N
      if (frm >= 30*FX32_ONE){
        //�t���[���ǂݎ��J�n
        tmp->FrameSetStart = 1;
        tmp->NowIcaAnmFrmIdx = 30;    //@todo
        (*seq)++;
      }
    }
    break;
  case 4:
    //��C�A�j���I���`�F�b�N���t���[���ǂݎ��I���`�F�b�N
    if (!tmp->FrameSetStart){
      //��l���ŏI�ʒu�Z�b�g
      {
        FIELD_PLAYER *fld_player;
        fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
        //��l���̃J�v�Z���o����W�Z�b�g
        {
          VecFx32 pos = {0,0,0};
          pos.x = STAND1_GX * FIELD_CONST_GRID_FX32_SIZE; //@todo
          pos.x += GRID_HALF_SIZE;
          pos.y = STAND1_GY * FIELD_CONST_GRID_FX32_SIZE; //@todo
          pos.z = STAND1_GZ * FIELD_CONST_GRID_FX32_SIZE; //@todo
          pos.z += GRID_HALF_SIZE;
          FIELD_PLAYER_SetPos( fld_player, &pos );
        }        
      }      
      (*seq)++;
    }
    break;
  case 5:
    {
      MMDL * mmdl;
      FIELD_PLAYER *fld_player;
      fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
      mmdl = FIELD_PLAYER_GetMMdl( fld_player );
      //���@�̍����擾���J�n
      MMDL_OffStatusBit( mmdl, MMDL_STABIT_HEIGHT_GET_OFF );
    }
    //�t���[���ǂݎ����
    ICA_ANIME_Delete( tmp->IcaAnmPtr );
    return GMEVENT_RES_FINISH;
  }

  //�t���[���ǂݎ�菈������
  if (tmp->FrameSetStart){      //�ǂݎ��t���O�������Ă���΁A���s
    u8 obj_idx;
    u8 anm_idx;
    VecFx32 dst_vec = {0,0,0};

    obj_idx = tmp->ShotIdx;

    //@todo
    if (tmp->NowIcaAnmFrmIdx == 70){
      //���@�\��
      MMDL * mmdl;
      FIELD_PLAYER *fld_player;
      fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
      mmdl = FIELD_PLAYER_GetMMdl( fld_player );
      MMDL_SetStatusBitVanish(mmdl, FALSE);
    }

    //���W�擾
    if ( ICA_ANIME_GetTranslateAt( tmp->IcaAnmPtr, &dst_vec, tmp->NowIcaAnmFrmIdx ) ){
      dst_vec.x += (CanPos[obj_idx][0]*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE);
      dst_vec.y += (CanPos[obj_idx][1]*FIELD_CONST_GRID_FX32_SIZE);
      dst_vec.z += (CanPos[obj_idx][2]*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE);
      //���@���W�Z�b�g
      FIELD_PLAYER_SetPos( FIELDMAP_GetFieldPlayer( fieldWork ), &dst_vec );
    }

    //�A�j���t���[���X�V
    if ( tmp->NowIcaAnmFrmIdx < tmp->MaxIcaAnmFrm ){
      tmp->NowIcaAnmFrmIdx++;
    }else{
      tmp->FrameSetStart = 0;
    }
  }

  return GMEVENT_RES_CONTINUE;
}

//�w��O���b�h���W�ɑ�C�����邩�𒲂ׂ�
static u8 GetCanIdx(const int inX, const int inZ)
{
  u8 i;
  for(i=0;i<CANNON_NUM_MAX;i++){
    if ( (CanPos[i][0] == inX)&&(CanPos[i][1] == inZ) ){
      return i;
    }
  }
  return i;
}

//��C�A�j�����̎��@�̍��W�X�V
static void PlayerMove()
{
  ;
}


