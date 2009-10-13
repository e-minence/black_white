//======================================================================
/**
 * @file  gym_anti.c
 * @brief  �A���`�W��
 * @author  Saito
 */
//======================================================================
#include "fieldmap.h"
#include "gym_anti_sv.h"
#include "gym_anti.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/fieldmap/gym_anti.naix"

#include "../../../resource/fldmapdata/gimmick/gym_anti/gym_anti_local_def.cdat"


#define GYM_ANTI_UNIT_IDX (0)
#define GYM_ANTI_TMP_ASSIGN_ID  (1)

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)

#define SW1_X  (SW1_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW1_Z  (SW1_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW2_X  (SW2_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW2_Z  (SW2_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW3_X  (SW3_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW3_Z  (SW3_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW4_X  (SW4_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW4_Z  (SW4_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW5_X  (SW5_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW5_Z  (SW5_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW6_X  (SW6_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW6_Z  (SW6_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW7_X  (SW7_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW7_Z  (SW7_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW8_X  (SW8_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW8_Z  (SW8_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW9_X  (SW9_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define SW9_Z  (SW9_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

#define CTN1_X  (CTN1_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CTN1_Z  (CTN1_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CTN2_X  (CTN2_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CTN2_Z  (CTN2_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CTN3_X  (CTN3_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define CTN3_Z  (CTN3_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

//OBJ�̂x���W
#define OBJ3D_Y  (0*FIELD_CONST_GRID_FX32_SIZE)



//�W���������̈ꎞ���[�N
typedef struct GYM_ANTI_TMP_tag
{
  u8 SwIdx;
  u8 DoorIdx;
  u8 dummy[2];
}GYM_ANTI_TMP;

//���\�[�X�̕��я���
enum {
  RES_ID_SW1_MDL = 0,
  RES_ID_SW2_MDL,
  RES_ID_SW3_MDL,

  RES_ID_CTN1_MDL,
  RES_ID_CTN2_MDL,
  RES_ID_CTN3_MDL,

  RES_ID_SW1_MOV1,
  RES_ID_SW1_MOV2,
  RES_ID_SW2_MOV1,
  RES_ID_SW2_MOV2,
  RES_ID_SW3_MOV1,
  RES_ID_SW3_MOV2,

  RES_ID_CTN1_MOV,
  RES_ID_CTN2_MOV,
  RES_ID_CTN3_MOV,

};


//�n�a�i�C���f�b�N�X
enum {
  OBJ_SW_1 = 0,
  OBJ_SW_2,
  OBJ_SW_3,
  OBJ_SW_4,
  OBJ_SW_5,
  OBJ_SW_6,
  OBJ_SW_7,
  OBJ_SW_8,
  OBJ_SW_9,
  OBJ_DOOR_1,
  OBJ_DOOR_2,
  OBJ_DOOR_3,
};

static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_GYM_ANTI, NARC_gym_anti_gym01_b_f_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@�X�C�b�`��
  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_b_w_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@�X�C�b�`��
  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_b_l_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@�X�C�b�`��

  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_c_f_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@�J�[�e����
  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_c_w_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@�J�[�e����
  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_c_l_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@�J�[�e����

  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_b_f_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�X�C�b�`��
  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_b_f_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@�X�C�b�`��
  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_b_w_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�X�C�b�`��
  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_b_w_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@�X�C�b�`��
  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_b_l_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�X�C�b�`��
  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_b_l_nsbta, GFL_G3D_UTIL_RESARC }, //ITA�@�X�C�b�`��

  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_c_f_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�J�[�e����
  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_c_w_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�J�[�e����
  { ARCID_GYM_ANTI, NARC_gym_anti_gym01_c_l_nsbca, GFL_G3D_UTIL_RESARC }, //ICA�@�J�[�e����
};

//3D�A�j���@�X�C�b�`
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_sw1[] = {
  { RES_ID_SW1_MOV1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_SW1_MOV2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�X�C�b�`
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_sw2[] = {
  { RES_ID_SW2_MOV1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_SW2_MOV2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�X�C�b�`
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_sw3[] = {
  { RES_ID_SW3_MOV1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_SW3_MOV2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�J�[�e��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_ctn1[] = {
  { RES_ID_CTN1_MOV,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�J�[�e��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_ctn2[] = {
  { RES_ID_CTN2_MOV,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�J�[�e��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_ctn3[] = {
  { RES_ID_CTN3_MOV,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
  //�X�C�b�`1�i������@�΁j
  {
		RES_ID_SW1_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW1_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw1,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw1),	//�A�j�����\�[�X��
	},
  //�X�C�b�`2�i������@���j
  {
		RES_ID_SW2_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW2_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw2,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw2),	//�A�j�����\�[�X��
	},
  //�X�C�b�`3�i������@���j
  {
		RES_ID_SW3_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW3_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw3,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw3),	//�A�j�����\�[�X��
	},

  //�X�C�b�`4�i�n�Y���@�΁j
  {
		RES_ID_SW1_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW1_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw1,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw1),	//�A�j�����\�[�X��
	},
  //�X�C�b�`5�i�n�Y���@���j
  {
		RES_ID_SW2_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW2_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw2,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw2),	//�A�j�����\�[�X��
	},
  //�X�C�b�`6�i�n�Y���@���j
  {
		RES_ID_SW3_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW3_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw3,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw3),	//�A�j�����\�[�X��
	},

  //�X�C�b�`7�i�n�Y���@�΁j
  {
		RES_ID_SW1_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW1_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw1,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw1),	//�A�j�����\�[�X��
	},
  //�X�C�b�`8�i�n�Y���@���j
  {
		RES_ID_SW2_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW2_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw2,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw2),	//�A�j�����\�[�X��
	},
  //�X�C�b�`9�i�n�Y���@���j
  {
		RES_ID_SW3_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SW3_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_sw3,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_sw3),	//�A�j�����\�[�X��
	},
  //�J�[�e��1�i�΁j
	{
		RES_ID_CTN1_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_CTN1_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_ctn1,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_ctn1),	//�A�j�����\�[�X��
	},
  //�J�[�e��2�i���j
  {
		RES_ID_CTN2_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_CTN2_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_ctn2,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_ctn2),	//�A�j�����\�[�X��
	},
  //�J�[�e��3�i���j
  {
		RES_ID_CTN3_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_CTN3_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_ctn3,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_ctn3),	//�A�j�����\�[�X��
	},
};

static const GFL_G3D_UTIL_SETUP Setup = {
  g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};




static GMEVENT_RESULT OpenDoorEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT PushSwEvt( GMEVENT* event, int* seq, void* work );

#ifdef PM_DEBUG
static BOOL test_GYM_ANTI_SwOn(GAMESYS_WORK *gsys, const u8 inSwIdx);
static BOOL test_GYM_ANTI_OpenDoor(GAMESYS_WORK *gsys, const u8 inDoorIdx);
#endif

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_ANTI_Setup(FIELDMAP_WORK *fieldWork)
{
  GYM_ANTI_SV_WORK *gmk_sv_work;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ANTI );
  }

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldWork, GYM_ANTI_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_ANTI_TMP));
  //�K�v�ȃ��\�[�X�̗p��
  FLD_EXP_OBJ_AddUnit(ptr, &Setup, GYM_ANTI_UNIT_IDX );
  //�X�C�b�`���W�Z�b�g
  {
    u8 i;
    const VecFx32 pos[ANTI_SW_NUM_MAX] = {
      {SW1_X, OBJ3D_Y, SW1_Z},
      {SW2_X, OBJ3D_Y, SW2_Z},
      {SW3_X, OBJ3D_Y, SW3_Z},
      {SW4_X, OBJ3D_Y, SW4_Z},
      {SW5_X, OBJ3D_Y, SW5_Z},
      {SW6_X, OBJ3D_Y, SW6_Z},
      {SW7_X, OBJ3D_Y, SW7_Z},
      {SW8_X, OBJ3D_Y, SW8_Z},
      {SW9_X, OBJ3D_Y, SW9_Z},
    };

    for (i=0;i<ANTI_SW_NUM_MAX;i++){
      u8 obj_idx = OBJ_SW_1+i;
      GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ANTI_UNIT_IDX, obj_idx);
      status->trans = pos[i];
    }
  }

  //�J�[�e�����W�Z�b�g
  {
    u8 i;
    const VecFx32 pos[ANTI_SW_NUM_MAX] = {
      {CTN1_X, OBJ3D_Y, CTN1_Z},
      {CTN2_X, OBJ3D_Y, CTN2_Z},
      {CTN3_X, OBJ3D_Y, CTN3_Z},
    };
    for (i=0;i<ANTI_DOOR_NUM_MAX;i++){
      u8 obj_idx = OBJ_DOOR_1+i;
      GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ANTI_UNIT_IDX, obj_idx);
      status->trans = pos[i];
    }
  }

  //�Z�[�u�󋵂ɂ�鏉���A�j�����Z�b�g
  {
    u8 i;
    for (i=0;i<ANTI_SW_NUM_MAX;i++){
      if ( gmk_sv_work->Sw[i] ){
        //�A�j���Z�b�g
        EXP_OBJ_ANM_CNT_PTR anm;
        u8 door_obj_idx;
        door_obj_idx = OBJ_DOOR_1 + i;
        {
          fx32 last_frm;
          //�A�j����L��
          FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ANTI_UNIT_IDX, door_obj_idx, 0, TRUE);
          anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ANTI_UNIT_IDX, door_obj_idx, 0);
          FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
          //1��Đ��ݒ�
          FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
          //�ŏI�t���[���ɂ���
          last_frm = FLD_EXP_OBJ_GetAnimeLastFrame(anm);
          FLD_EXP_OBJ_SetObjAnmFrm(ptr,GYM_ANTI_UNIT_IDX, door_obj_idx, 0, last_frm);
        }
      }
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
void GYM_ANTI_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GYM_ANTI_TMP_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, GYM_ANTI_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_ANTI_Move(FIELDMAP_WORK *fieldWork)
{
  int i;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  {
    GAMESYS_WORK *gsys  = FIELDMAP_GetGameSysWork( fieldWork );
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){
      test_GYM_ANTI_SwOn(gsys, 0);
    }
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
      test_GYM_ANTI_OpenDoor(gsys, 0);
    }
  }



  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );

}

//--------------------------------------------------------------
/**
 * �X�C�b�`�A�j���C�x���g�N��
 * @param	
 * @return
 */
//--------------------------------------------------------------
GMEVENT *GYM_ANTI_SwOn(GAMESYS_WORK *gsys, const u8 inSwIdx)
{
  GMEVENT * event;
  GYM_ANTI_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ANTI_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ANTI_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );

  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ANTI );

  if (inSwIdx > ANTI_SW_NUM_MAX){
    return NULL;
  }

  //�C�x���g�R�[��
  event = GMEVENT_Create( gsys, NULL, PushSwEvt, 0 );
    
  //�X�C�b�`�C���f�b�N�X���Z�b�g
  tmp->SwIdx = inSwIdx;

  return event;
}

//--------------------------------------------------------------
/**
 * �h�A�I�[�v���A�j���C�x���g�N��
 * @param	
 * @return
 */
//--------------------------------------------------------------
GMEVENT *GYM_ANTI_OpenDoor(GAMESYS_WORK *gsys, const u8 inDoorIdx)
{
  GMEVENT * event;
  GYM_ANTI_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ANTI_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ANTI_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );

  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ANTI );

  if (inDoorIdx > ANTI_DOOR_NUM_MAX){
    return NULL;
  }

  //�C�x���g�R�[��
  event = GMEVENT_Create( gsys, NULL, OpenDoorEvt, 0 );
    
  //�h�A�C���f�b�N�X���Z�b�g
  tmp->DoorIdx = inDoorIdx;

  return event;

}

//--------------------------------------------------------------
/**
 * �X�C�b�`�C�x���g
 * @param	
 * @return
 */
//--------------------------------------------------------------
static GMEVENT_RESULT PushSwEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_ANTI_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ANTI_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ANTI_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ANTI );
  }
  
  switch(*seq){
  case 0:  //�A�j���؂�ւ�
    {      
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 sw_obj_idx;
      sw_obj_idx = OBJ_SW_1 + tmp->SwIdx;
      {
        //�A�j�����J�n
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ANTI_UNIT_IDX, sw_obj_idx, 0, TRUE);
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ANTI_UNIT_IDX, sw_obj_idx, 0);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
        //���o��
        FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_ANTI_UNIT_IDX, sw_obj_idx, 0, 0 );
        //1��Đ��ݒ�
        FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      }
    }    
    (*seq)++;
    break;
  case 1: //�A�j���҂� 
    {
      
      u8 sw_obj_idx;
      EXP_OBJ_ANM_CNT_PTR sw_anm;
      sw_obj_idx = OBJ_SW_1 + tmp->SwIdx;
      sw_anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ANTI_UNIT_IDX, sw_obj_idx, 0);
      if ( FLD_EXP_OBJ_ChkAnmEnd(sw_anm) ){
        OS_Printf("�A�j���I��\n");
        return GMEVENT_RES_FINISH;
      }
    }
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �h�A�C�x���g
 * @param	
 * @return
 */
//--------------------------------------------------------------
static GMEVENT_RESULT OpenDoorEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_ANTI_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ANTI_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ANTI_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ANTI );
  }
  
  switch(*seq){
  case 0:  //�A�j���؂�ւ�
    {      
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 door_obj_idx;
      door_obj_idx = OBJ_DOOR_1 + tmp->DoorIdx;
      {
        //�A�j�����J�n
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ANTI_UNIT_IDX, door_obj_idx, 0, TRUE);
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ANTI_UNIT_IDX, door_obj_idx, 0);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
        //���o��
        FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_ANTI_UNIT_IDX, door_obj_idx, 0, 0 );
        //1��Đ��ݒ�
        FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      }
    }    
    (*seq)++;
    break;
  case 1: //�A�j���҂� 
    {
      
      u8 door_obj_idx;
      EXP_OBJ_ANM_CNT_PTR door_anm;
      door_obj_idx = OBJ_DOOR_1 + tmp->DoorIdx;
      door_anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ANTI_UNIT_IDX, door_obj_idx, 0);
      if ( FLD_EXP_OBJ_ChkAnmEnd(door_anm) ){
        OS_Printf("�A�j���I��\n");
        //�X�C�b�`�����t���O�I��
        gmk_sv_work->Sw[tmp->SwIdx] = 1;
        return GMEVENT_RES_FINISH;
      }
    }
  }
  return GMEVENT_RES_CONTINUE;
}

#ifdef PM_DEBUG
//--------------------------------------------------------------
/**
 * ���o�[�̐؂�ւ����s���C�x���g�N��
 * @param	
 * @return
 */
//--------------------------------------------------------------
static BOOL test_GYM_ANTI_SwOn(GAMESYS_WORK *gsys, const u8 inSwIdx)
{
  GYM_ANTI_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ANTI_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ANTI_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );

  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ANTI );

  {
    //�C�x���g�Z�b�g
    GMEVENT * event = GMEVENT_Create( gsys, NULL, PushSwEvt, 0 );
    GAMESYSTEM_SetEvent(gsys, event);
    //����\��C���f�b�N�X���Z�b�g
    tmp->SwIdx = inSwIdx;
  }
  return TRUE;
  
}

static BOOL test_GYM_ANTI_OpenDoor(GAMESYS_WORK *gsys, const u8 inDoorIdx)
{
  GYM_ANTI_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ANTI_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ANTI_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );

  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ANTI );

  {
    //�C�x���g�Z�b�g
    GMEVENT * event = GMEVENT_Create( gsys, NULL, OpenDoorEvt, 0 );
    GAMESYSTEM_SetEvent(gsys, event);
    //����\��C���f�b�N�X���Z�b�g
    tmp->DoorIdx = inDoorIdx;
  }
  return TRUE;
  
}

#endif  //PM_DEBUG

