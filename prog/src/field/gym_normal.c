//======================================================================
/**
 * @file  gym_normal.c
 * @brief  �m�[�}���W��
 * @author  Saito
 */
//======================================================================

//�����̃W����2�����ɃM�~�b�N�����邪�A�����̖{�I�͂ǂ���̕����ł�0�Ԗڂɓo�^����

#include "fieldmap.h"
#include "gym_normal_sv.h"
#include "gym_normal.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/fieldmap/gym_normal.naix"
#include "system/main.h"    //for HEAPID_FIELDMAP
#include "script.h"     //for SCRIPT_CallScript

#include "gym_normal_local_def.h"

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)

#define GYM_NORMAL_UNIT_IDX (0)
#define GYM_NORMAL_TMP_ASSIGN_ID  (1)

//�{�I���W
#define WALL0_X  (WALL0_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL0_Z  (WALL0_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL1_X  (WALL1_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL1_Z  (WALL1_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL2_X  (WALL2_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL2_Z  (WALL2_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL3_X  (WALL3_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL3_Z  (WALL3_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL4_X  (WALL4_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL4_Z  (WALL4_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL5_X  (WALL5_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL5_Z  (WALL5_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL6_X  (WALL6_GX*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)
#define WALL6_Z  (WALL6_GZ*FIELD_CONST_GRID_FX32_SIZE + GRID_HALF_SIZE)

//OBJ�̂x���W
#define OBJ3D_Y  (0*FIELD_CONST_GRID_FX32_SIZE)

#define WALL_NUM  (1)


typedef struct POS_XZ_tag
{
  fx32 x;
  fx32 z;
}POS_XZ;

//�W���������̈ꎞ���[�N
typedef struct GYM_NORMAL_TMP_tag
{
  u16 WallIdx;
  u16 dummy;
}GYM_NORMAL_TMP;

//�n�a�i�C���f�b�N�X
enum {
  OBJ_WALL_1 = 0,
};

//�A�j���̏���
enum{
  ANM_WALL_MOV = 0,
};

//���\�[�X�̕��я���
enum {
  RES_ID_WALL_MDL = 0,
  RES_ID_WALL_MOVE,
};

static const POS_XZ WallPos[WALL_NUM] = {
  {WALL0_X, WALL0_Z},
};

//--���\�[�X�֘A--
//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
  { ARCID_GYM_NORMAL, NARC_gym_normal_book_anim01_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD�@�{�I
  { ARCID_GYM_NORMAL, NARC_gym_normal_book_anim01_nsbca, GFL_G3D_UTIL_RESARC }, //IMD�@�{�I�A�j��
};

//3D�A�j��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl[] = {
	{ RES_ID_WALL_MOVE,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[WALL_NUM] = {
  //�{�I1
	{
		RES_ID_WALL_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_MDL, 	//�e�N�X�`�����\�[�XID
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

static GMEVENT_RESULT WallAnmEvt( GMEVENT* event, int* seq, void* work );


BOOL test_GYM_NORMAL_WallAnm(GAMESYS_WORK *gsys, const u8 inWallIdx);

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_NORMAL_Setup(FIELDMAP_WORK *fieldWork)
{
  u8 wall_num;
  const POS_XZ *pos;
  int gmk_id;
  GYM_NORMAL_SV_WORK *gmk_sv_work;
  GYM_NORMAL_TMP *tmp;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldWork, GYM_NORMAL_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_NORMAL_TMP));

  tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_NORMAL_TMP_ASSIGN_ID);

  {
    //�K�v�ȃ��\�[�X�̗p��
    FLD_EXP_OBJ_AddUnit(ptr, &Setup, GYM_NORMAL_UNIT_IDX );
    wall_num = WALL_NUM;
    pos = WallPos;
    gmk_id = FLD_GIMMICK_GYM_NORM;
  }

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, gmk_id );
  }

  //���W�Z�b�g
  {
    u8 i;
    for (i=0;i<wall_num;i++){
      VecFx32 pos_vec;
      GFL_G3D_OBJSTATUS *status;
      pos_vec.x = pos[i].x;
      pos_vec.y = OBJ3D_Y;
      pos_vec.z = pos[i].z;
      status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_NORMAL_UNIT_IDX, OBJ_WALL_1+i);
      status->trans = pos_vec;
    }
  }

  //�{�I�����A�j������
  {
    u8 i;
    for (i=0;i<wall_num;i++){
      u8 obj_idx;

      obj_idx = OBJ_WALL_1+i;
      //1���[�v�A�j���ݒ�
      {
        EXP_OBJ_ANM_CNT_PTR anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_NORMAL_UNIT_IDX, obj_idx, ANM_WALL_MOV);
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
        //1��Đ��ݒ�
        FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      }
      //�Z�[�u�f�[�^�����ăA�j���I���O���ǂ�������
      if (gmk_sv_work->Wall[i]){  //�I��
        //�ŏI�t���[���ŃX�g�b�v
        FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_NORMAL_UNIT_IDX, obj_idx, ANM_WALL_MOV, TRUE);
        //�ŏI�t���[���Z�b�g
        ;
      }
    }
  }

  //���ɃM�~�b�N�N����̏ꍇ�͐����̒I�̂ݍŏI�t���[���ŃX�g�b�v
  if (gmk_sv_work->GmkUnrock){
    ;
  }
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_NORMAL_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GYM_NORMAL_TMP_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, GYM_NORMAL_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	
 * @return
 */
//--------------------------------------------------------------
void GYM_NORMAL_Move(FIELDMAP_WORK *fieldWork)
{
  int i;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�e�X�g
  {
    GAMESYS_WORK *gsys  = FIELDMAP_GetGameSysWork( fieldWork );
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){
      test_GYM_NORMAL_WallAnm(gsys, 0);
    }
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
    }
  }

  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );

}

//�X�C�b�`�N��
void GYM_NORMAL_Unrock(FIELDMAP_WORK *fieldWork)
{
  GYM_NORMAL_SV_WORK *gmk_sv_work;
  GYM_NORMAL_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_NORMAL_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_NORM );

  gmk_sv_work->GmkUnrock = TRUE;
}

//�X�C�b�`�`�F�b�N
BOOL GYM_NORMAL_CheckRock(FIELDMAP_WORK *fieldWork)
{
  GYM_NORMAL_SV_WORK *gmk_sv_work;
  GYM_NORMAL_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_NORMAL_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
  gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_NORM );

  return gmk_sv_work->GmkUnrock;
}

//�I�A�j���N��
GMEVENT * GYM_NORMAL_MoveWall(GAMESYS_WORK *gsys, const u8 inWallIdx)
{
  GMEVENT * event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  GYM_NORMAL_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_NORMAL_TMP_ASSIGN_ID);

  if (inWallIdx >= NRM_WALL_NUM_MAX){
    return NULL;
  }

  //�C�x���g�R�[��
  event = GMEVENT_Create( gsys, NULL, WallAnmEvt, 0 );
    
  //����\�背�o�[�C���f�b�N�X���Z�b�g
  tmp->WallIdx = inWallIdx;

  return event;

}

//--------------------------------------------------------------
/**
 * ���o�[�̐؂�ւ����s���C�x���g�N��
 * @param	
 * @return
 */
//--------------------------------------------------------------
BOOL test_GYM_NORMAL_WallAnm(GAMESYS_WORK *gsys, const u8 inWallIdx)
{
  int gmk_id;
  GYM_NORMAL_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_NORMAL_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_NORMAL_TMP_ASSIGN_ID);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
  GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );

  gmk_id = FLD_GIMMICK_GYM_NORM;

  gmk_sv_work = GIMMICKWORK_Get( gmkwork, gmk_id );
  tmp->WallIdx = inWallIdx;

  {
    //�C�x���g�Z�b�g
    GMEVENT * event = GMEVENT_Create( gsys, NULL, WallAnmEvt, 0/*sizeof(ENCEFF_WORK)*/ );
    GAMESYSTEM_SetEvent(gsys, event);
  }
  return TRUE;
  
}

//--------------------------------------------------------------
/**
 * �{�I�A�j���C�x���g
 * @param	
 * @return
 */
//--------------------------------------------------------------
static GMEVENT_RESULT WallAnmEvt( GMEVENT* event, int* seq, void* work )
{
  int gmk_id;
  GYM_NORMAL_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_NORMAL_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_NORMAL_TMP_ASSIGN_ID);

  gmk_id = FLD_GIMMICK_GYM_NORM;

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = SaveData_GetGimmickWork( GAMEDATA_GetSaveControlWork( gamedata ) );
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, gmk_id );
  }
  
  switch(*seq){
  case 0:  //�A�j���J�n
    {      
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 sw_obj_idx;
      sw_obj_idx = OBJ_WALL_1 + tmp->WallIdx;
      //�A�j�����J�n
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_NORMAL_UNIT_IDX, sw_obj_idx, ANM_WALL_MOV, TRUE);
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_NORMAL_UNIT_IDX, sw_obj_idx, ANM_WALL_MOV);
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
      //���o��
      FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_NORMAL_UNIT_IDX, sw_obj_idx, ANM_WALL_MOV, 0 );
    }    
    (*seq)++;
    break;
  case 1: //�A�j���҂�
    {
      u8 sw_obj_idx;
      EXP_OBJ_ANM_CNT_PTR sw_anm;
      sw_obj_idx = OBJ_WALL_1 + tmp->WallIdx;
      sw_anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_NORMAL_UNIT_IDX, sw_obj_idx, ANM_WALL_MOV);
      if ( FLD_EXP_OBJ_ChkAnmEnd(sw_anm)&&FLD_EXP_OBJ_ChkAnmEnd(sw_anm) ){
        OS_Printf("�A�j���I��\n");
        //�M�~�b�N�N���I���t���O�n�m
        gmk_sv_work->Wall[tmp->WallIdx] = 1;
        return GMEVENT_RES_FINISH;
      }
    }
  }
  return GMEVENT_RES_CONTINUE;
}



