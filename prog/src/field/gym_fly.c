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

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

//#include "arc/fieldmap/gym_fly.naix"

#include "system/main.h"    //for HEAPID_FIELDMAP
#include "script.h"     //for SCRIPT_CallScript
//#include "../../../resource/fldmapdata/script/c04gym0101_def.h"  //for SCRID_�`



#define GYM_FLY_UNIT_IDX (0)
#define GYM_FLY_TMP_ASSIGN_ID  (1)

#define FRAME_POS_SIZE  (4*3*360)   //���W�w�x�y�e4�o�C�g��360�t���[��
#define HEADER_SIZE  (8)   //�t���[���T�C�Y4�o�C�g+�i�[���3�o�C�g�̃w�b�_�[��񂪂��邽�߃A���C�����g����������8�o�C�g�m�ۂ���

#define CANNON_NUM_MAX  (10)

//�C��O���b�h���W
#define CAN1_GX (22)
#define CAN1_GZ (13)
#define CAN2_GX (15)
#define CAN2_GZ (9)
#define CAN3_GX (23)
#define CAN3_GZ (17)
#define CAN4_GX (20)
#define CAN4_GZ (9)
#define CAN5_GX (11)
#define CAN5_GZ (13)
#define CAN6_GX (8)
#define CAN6_GZ (4)
#define CAN7_GX (28)
#define CAN7_GZ (4)
#define CAN8_GX (26)
#define CAN8_GZ (22)
#define CAN9_GX (6)
#define CAN9_GZ (20)
#define CAN10_GX (2)
#define CAN10_GZ (13)


//�C����W
#define CAN1_X  (CAN1_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CAN1_Y  (1*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
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

//�W���������̈ꎞ���[�N
typedef struct GYM_FLY_TMP_tag
{
  u8 ShotIdx;
  u8 ShotDir;
  u8 FrameSetStart;
  u8 dummy;

  u8 FramePosDat[FRAME_POS_SIZE+HEADER_SIZE];

}GYM_FLY_TMP;

//���\�[�X�̕��я���
enum {
  RES_ID_CAN_MDL = 0,
  RES_ID_CAN_ANM1_ICA,
  RES_ID_CAN_ANM1_ITP,
  RES_ID_CAN_ANM2_ICA,
  RES_ID_CAN_ANM2_ITP,
  RES_ID_CAN_ANM3_ICA,
  RES_ID_CAN_ANM3_ITP,
  RES_ID_CAN_ANM4_ICA,
  RES_ID_CAN_ANM4_ITP,
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
static const u16 CanPos[CANNON_NUM_MAX][2] = {
  {CAN1_GX, CAN1_GZ},
  {CAN2_GX, CAN2_GZ},
  {CAN3_GX, CAN3_GZ},
  {CAN4_GX, CAN4_GZ},
  {CAN5_GX, CAN5_GZ},
  {CAN6_GX, CAN6_GZ},
  {CAN7_GX, CAN7_GZ},
  {CAN8_GX, CAN8_GZ},
  {CAN9_GX, CAN9_GZ},
  {CAN10_GX, CAN10_GZ},

};

//--���\�[�X�֘A--
//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
#if 0  
	{ ARCID_GYM_FLY, NARC_gym_elec_gym04_cart_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@��C�{��
  
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02a_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@��C�A�j��1
  { ARCID_GYM_ELEC, NARC_gym_elec_gym04_rail_02b_nsbca, GFL_G3D_UTIL_RESARC }, //ITP�@��C�A�j��1
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
  { RES_ID_CAN_ANM1_ITP,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM2_ICA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM2_ITP,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM3_ICA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM3_ITP,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM4_ICA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_CAN_ANM4_ITP,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)

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

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_FLY_Setup(FIELDMAP_WORK *fieldWork)
{
  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldWork, GYM_FLY_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_FLY_TMP));
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

}

//--------------------------------------------------------------
/**
 * �X�C�b�`�C�x���g
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
    //�ړ��f�[�^���[�h
    ;
    //���@�̌����ɂ�蕪��
    //��C�h�A�I�[�v���t���[���܂ōĐ��J�n
    ;
    (*seq)++;
    break;
  case 1:
    //�I�[�v���t���[�����B�Ď�
    if (1){
      //���@�A��C�ɓ���
      (*seq)++;
    }
    break;
  case 2:
    //���@�A�j���I���҂�
    if(1){
      //��C�A�j���ۊJ�n
      (*seq)++;
    }
    break;
  case 3:
    //���˃t���[�����B�`�F�b�N
    if (1){
      //�t���[���ǂݎ��J�n
      tmp->FrameSetStart = 1;
      (*seq)++;
    }
    break;
  case 4:
    //��C�A�j���I���`�F�b�N���t���[���ǂݎ��I���`�F�b�N
    if (1){
      //�t���[���ǂݎ��I��
      tmp->FrameSetStart = 0;
      //��l���ŏI�ʒu�Z�b�g
      ;
      (*seq)++;
    }
    break;
  case 5:
    //�t���[���ǂݎ����
    ;
    return GMEVENT_RES_FINISH;
  }

  //�t���[���ǂݎ�菈������
  if (tmp->FrameSetStart){      //�ǂݎ��t���O�������Ă���΁A���s
    ;
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


