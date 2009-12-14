//======================================================================
/**
 * @file  gym_ice.c
 * @brief  �X�W��
 * @author  Saito
 * @date  09.11.26
 */
//======================================================================

#include "fieldmap.h"
#include "gym_ice_sv.h"
#include "gym_ice.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/fieldmap/gym_ice.naix"
#include "system/main.h"    //for HEAPID_FIELDMAP

#include "../../../resource/fldmapdata/gimmick/gym_ice/gym_ice_local_def.h"

//#include "sound/pm_sndsys.h"
#include "gym_ice_se_def.h"

#include "field/fieldmap_proc.h"    //for FLDMAP_CTRLTYPE
#include "fieldmap_ctrl_hybrid.h" //for FIELDMAP_CTRL_HYBRID

#define GYM_ICE_UNIT_IDX (0)
#define GYM_ICE_TMP_ASSIGN_ID  (1)

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)

#define WALL_ANM_NUM  (2)

#define RAIL_CHECK_MAX  (8)

#define WALL1_X (WALL1_GX*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define WALL2_X (WALL2_GX*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define WALL3_X (WALL3_GX*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define WALL1_Y (WALL1_GY*FIELD_CONST_GRID_FX32_SIZE)
#define WALL2_Y (WALL2_GY*FIELD_CONST_GRID_FX32_SIZE)
#define WALL3_Y (WALL3_GY*FIELD_CONST_GRID_FX32_SIZE)
#define WALL1_Z (WALL1_GZ*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define WALL2_Z (WALL2_GZ*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define WALL3_Z (WALL3_GZ*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define SW1_X (SW1_GX*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define SW2_X (SW2_GX*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define SW3_X (SW3_GX*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define SW1_Y (SW1_GY*FIELD_CONST_GRID_FX32_SIZE)
#define SW2_Y (SW2_GY*FIELD_CONST_GRID_FX32_SIZE)
#define SW3_Y (SW3_GY*FIELD_CONST_GRID_FX32_SIZE)
#define SW1_Z (SW1_GZ*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define SW2_Z (SW2_GZ*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define SW3_Z (SW3_GZ*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)


static const VecFx32 WallPos[WALL_NUM_MAX] = {
  { WALL1_X, WALL1_Y, WALL1_Z },
  { WALL2_X, WALL2_Y, WALL2_Z },
  { WALL3_X, WALL3_Y, WALL3_Z },
};

static const VecFx32 SwPos[WALL_NUM_MAX] = {
  { SW1_X, SW1_Y, SW1_Z },
  { SW2_X, SW2_Y, SW2_Z },
  { SW3_X, SW3_Y, SW3_Z },
};

//���\�[�X�̕��я���
enum {
  RES_ID_WALL1_MDL = 0,
  RES_ID_WALL2_MDL,
  RES_ID_SW_MDL,
  RES_ID_WALL1_ANM_A,
  RES_ID_WALL1_ANM_B,
  RES_ID_WALL2_ANM_A,
  RES_ID_WALL2_ANM_B,
  RES_ID_SW_ANM_A,
  RES_ID_SW_ANM_B,
};

//�n�a�i�C���f�b�N�X
enum {
  OBJ_WALL_1 = 0,
  OBJ_WALL_2,
  OBJ_WALL_3,
  OBJ_SW_1,
  OBJ_SW_2,
  OBJ_SW_3,
};

typedef enum{
  OBJ_KIND_SW,
  OBJ_KIND_WALL,
}OBJ_KIND;

//�W���������̈ꎞ���[�N
typedef struct GYM_ICE_TMP_tag
{
  int TargetIdx;
}GYM_ICE_TMP;

//--���\�[�X�֘A--
//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_GYM_ICE, NARC_gym_ice_ice_karakuri_01_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_ICE, NARC_gym_ice_ice_karakuri_02_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_ICE, NARC_gym_ice_swich_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD

  { ARCID_GYM_ICE, NARC_gym_ice_ice_karakuri_01_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_ICE, NARC_gym_ice_ice_karakuri_01_2_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_ICE, NARC_gym_ice_ice_karakuri_02_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_ICE, NARC_gym_ice_ice_karakuri_02_2_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_GYM_ICE, NARC_gym_ice_swich_rb_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_ICE, NARC_gym_ice_swich_br_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
};

//3D�A�j���@���t�g
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_wall1[] = {
	{ RES_ID_WALL1_ANM_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL1_ANM_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_wall2[] = {
	{ RES_ID_WALL2_ANM_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_WALL2_ANM_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_sw[] = {
	{ RES_ID_SW_ANM_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_SW_ANM_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};


//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
  //��1
	{
		RES_ID_WALL2_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL2_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall2,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall2),	//�A�j�����\�[�X��
	},
  //��2
	{
		RES_ID_WALL1_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL1_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall1,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall1),	//�A�j�����\�[�X��
	},
  //��3
	{
		RES_ID_WALL2_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL2_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall2,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall2),	//�A�j�����\�[�X��
	},
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
};

static const GFL_G3D_UTIL_SETUP Setup = {
  g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};

typedef struct RAIL_CHECK_tag
{
  u16 SwIdx;      //0�`2
  u16 SwState;    //0or1  0�F�f�t�H���g�@1�F�ύX��
}RAIL_CHECK;

//0�F�X�C�b�`���f�t�H���g�̂Ƃ��@1�F�X�C�b�`���������Ƃ� ���[�����L���ɂȂ�
const RAIL_CHECK RailCheck[RAIL_CHECK_MAX] = {
  {0,0},    //�`�F�b�N�C���f�b�N�X0
  {0,1},
  {1,0},
  {1,1},
  {2,0},
  {2,0},
  {2,1},
  {2,1},    //�`�F�b�N�C���f�b�N�X7
};

static BOOL CheckChgRail(GYM_ICE_SV_WORK *gmk_sv_work, const int inCheckIdx);
static void SetupWallSwAnm(FLD_EXP_OBJ_CNT_PTR ptr, const BOOL inMoved, const int inTargetIdx, const OBJ_KIND inKind);
static int GetWatchAnmIdx(GYM_ICE_SV_WORK *gmk_sv_work, const int inTargetIdx);

static GMEVENT_RESULT SwitchEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT WallEvt( GMEVENT* event, int* seq, void* work );

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_ICE_Setup(FIELDMAP_WORK *fieldWork)
{
  int i;
  GYM_ICE_SV_WORK *gmk_sv_work;
  GYM_ICE_TMP *tmp;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ICE );
  }

  //�ėp���[�N�m��
  tmp = GMK_TMP_WK_AllocWork
      (fieldWork, GYM_ICE_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_ICE_TMP));

  //�K�v�ȃ��\�[�X�̗p��
  FLD_EXP_OBJ_AddUnit(ptr, &Setup, GYM_ICE_UNIT_IDX );

  //���W�Z�b�g
  for (i=0;i<WALL_NUM_MAX;i++)
  {
    int j;
    int idx;
    GFL_G3D_OBJSTATUS *status;
    //��
    idx = OBJ_WALL_1 + i;
    status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ICE_UNIT_IDX, idx);
    status->trans = WallPos[i];
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_ICE_UNIT_IDX, idx, TRUE);
    for (j=0;j<WALL_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ICE_UNIT_IDX, idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      //�A�j����~
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    }

    //�X�C�b�`
    idx = OBJ_SW_1 + i;
    status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_ICE_UNIT_IDX, idx);
    status->trans = SwPos[i];
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_ICE_UNIT_IDX, idx, TRUE);
    for (j=0;j<WALL_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ICE_UNIT_IDX, idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      //�A�j����~
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    }
    
    SetupWallSwAnm(ptr, gmk_sv_work->WallMoved[i], i, OBJ_KIND_WALL);
    SetupWallSwAnm(ptr, gmk_sv_work->WallMoved[i], i, OBJ_KIND_SW);
  }
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_ICE_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ICE_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ICE_TMP_ASSIGN_ID);

  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GYM_ICE_TMP_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, GYM_ICE_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_ICE_Move(FIELDMAP_WORK *fieldWork)
{
  int i;
  GYM_ICE_SV_WORK *gmk_sv_work;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ICE_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ICE_TMP_ASSIGN_ID);
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ICE );
  }
#ifdef PM_DEBUG
  if (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG){
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
      GMEVENT *event;
      GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
      event = GYM_ICE_CreateSwEvt(gsys, 0);
      GAMESYSTEM_SetEvent(gsys, event);
    }else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
      GMEVENT *event;
      GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
      event = GYM_ICE_CreateWallEvt(gsys, 0);
      GAMESYSTEM_SetEvent(gsys, event);
    }else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_START){
      GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
      GYM_ICE_ChangeRailMap(gsys, 0);
    }
  }
#endif
  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );
}

//--------------------------------------------------------------
/**
 * �X�C�b�`�C�x���g�쐬
 * @param	      gsys        �Q�[���V�X�e���|�C���^
 * @param       inTargetIdx   �M�~�b�N�ΏۃC���f�b�N�X
 * @return      GMEVENT     �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *GYM_ICE_CreateSwEvt(GAMESYS_WORK *gsys, const int inTargetIdx)
{
  GMEVENT * event;
  GYM_ICE_TMP *tmp;
  {
    FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ICE_TMP_ASSIGN_ID);
  }

  if ( inTargetIdx >= WALL_NUM_MAX ){
    GF_ASSERT_MSG(0, "TARGET_INDEX_OVER %d",inTargetIdx);
    return NULL;
  }

  tmp->TargetIdx = inTargetIdx;

  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, SwitchEvt, 0 );

  return event;
}

//--------------------------------------------------------------
/**
 * �ǃC�x���g�쐬
 * @param	      gsys        �Q�[���V�X�e���|�C���^
 * @param       inTargetIdx   �M�~�b�N�ΏۃC���f�b�N�X
 * @return      GMEVENT     �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *GYM_ICE_CreateWallEvt(GAMESYS_WORK *gsys, const int inTargetIdx)
{
  GMEVENT * event;
  GYM_ICE_TMP *tmp;
  {
    FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ICE_TMP_ASSIGN_ID);
  }

  if ( inTargetIdx >= WALL_NUM_MAX ){
    GF_ASSERT_MSG(0, "TARGET_INDEX_OVER %d",inTargetIdx);
    return NULL;
  }

  tmp->TargetIdx = inTargetIdx;

  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, WallEvt, 0 );

  return event;
}

//--------------------------------------------------------------
/**
 * ���[���}�b�v�ւ̐؂�ւ�
 * @param	      gsys        �Q�[���V�X�e���|�C���^
 * @param       inCheckIdx  ���[���}�b�v�؂�ւ�����C���f�b�N�X�i�o���ʒu�C���f�b�N�X�j0�`7
 * @return      none
 */
//--------------------------------------------------------------
void GYM_ICE_ChangeRailMap(GAMESYS_WORK *gsys, const int inCheckIdx)
{
  GYM_ICE_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLDMAP_CTRLTYPE type = FIELDMAP_GetMapControlType( fieldWork );
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ICE );
  }

  if (inCheckIdx >= RAIL_CHECK_MAX)
  {
    GF_ASSERT(0);
    return;
  }

  if ( type == FLDMAP_CTRLTYPE_HYBRID )
  {
    //���[���ɐ؂�ւ��邩�ǂ��������݂̃X�C�b�`�󋵂Ŕ��f����
    if ( CheckChgRail(gmk_sv_work, inCheckIdx) )
    {
      FIELDMAP_CTRL_HYBRID *ctrl_work;
      ctrl_work = FIELDMAP_GetMapCtrlWork( fieldWork );
      FIELDMAP_CTRL_HYBRID_ChangeBaseSystem( ctrl_work, fieldWork );
    }
  }
  else
  {
    GF_ASSERT(0);
  }
}

//--------------------------------------------------------------
/**
 * ���[���}�b�v�؂�ւ��`�F�b�N
 *   
 * @param   gmk_sv_work �M�~�b�N�Z�[�u���[�N�|�C���^
 * @param   inCheckIdx    �`�F�b�N���W�C���f�b�N�X  0�`7
 * @return  BOOL      TRUE�Ń��[���}�b�v�ɕύX����
 */
//--------------------------------------------------------------
static BOOL CheckChgRail(GYM_ICE_SV_WORK *gmk_sv_work, const int inCheckIdx)
{
  u16 state;
  u16 target_sw = RailCheck[inCheckIdx].SwIdx;
  BOOL moved = gmk_sv_work->WallMoved[target_sw];
  if (moved) state = 1;
  else state = 0;

  if ( RailCheck[inCheckIdx].SwState == state ) return TRUE;

  return FALSE;
}

//--------------------------------------------------------------
/**
 * �ǁA�X�C�b�`�A�j������
 *   
 * @param   ptr       �g���n�a�i�R���g���[���|�C���^
 * @param   inMoved   ��ʒu���瓮���Ă��邩�H
 * @param	  inTargetIdx �ΏۃC���f�b�N�X
 * @param   inKind      �n�a�i���
 * @return  none
 */
//--------------------------------------------------------------
static void SetupWallSwAnm(FLD_EXP_OBJ_CNT_PTR ptr, const BOOL inMoved, const int inTargetIdx, const OBJ_KIND inKind)
{
  EXP_OBJ_ANM_CNT_PTR anm;
  int obj_idx;
  int valid_anm_idx, invalid_anm_idx;

  if ( inMoved )
  {
    valid_anm_idx = 1;
    invalid_anm_idx = 0;
  }
  else {
    valid_anm_idx = 0;
    invalid_anm_idx = 1;
  }

  if ( inKind == OBJ_KIND_WALL ) obj_idx = OBJ_WALL_1+inTargetIdx;
  else  obj_idx = OBJ_SW_1+inTargetIdx;

  
  FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ICE_UNIT_IDX, obj_idx, valid_anm_idx, TRUE);
  FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_ICE_UNIT_IDX, obj_idx, invalid_anm_idx, FALSE);
  //������
  FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_ICE_UNIT_IDX, obj_idx, valid_anm_idx, 0 );
  //��~�ɂ��Ă���
  anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ICE_UNIT_IDX, obj_idx, valid_anm_idx);
  FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);

}

//--------------------------------------------------------------
/**
 * �I���Ď�����A�j���C���f�b�N�X���擾
 * @param   gmk_sv_work   �M�~�b�N�Z�[�u���[�N
 * @param	  inTargetIdx �M�~�b�N�ΏۃC���f�b�N�X 0�`2
 * @return  int     �I���Ď�����A�j���C���f�b�N�X�@0�`1
 */
//--------------------------------------------------------------
static int GetWatchAnmIdx(GYM_ICE_SV_WORK *gmk_sv_work, const int inTargetIdx)
{
  int valid_anm_idx;
  if ( gmk_sv_work->WallMoved[inTargetIdx] ) valid_anm_idx = 1;
  else valid_anm_idx = 0;

  return valid_anm_idx;
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
  GYM_ICE_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ICE_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ICE_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ICE );
  }

  switch(*seq){
  case 0:  //�X�C�b�`�A�j���J�n
    {
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 sw_obj_idx;
      u8 anm_idx;
      sw_obj_idx = OBJ_SW_1 + tmp->TargetIdx;
      anm_idx = GetWatchAnmIdx(gmk_sv_work, tmp->TargetIdx);
      //�A�j�����J�n
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ICE_UNIT_IDX, sw_obj_idx, anm_idx );
      //�A�j����~����
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
    }
    (*seq)++;
    break;
  case 1:
    {
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 obj_idx;
      u8 anm_idx;
      obj_idx = OBJ_SW_1 + tmp->TargetIdx;
      anm_idx = GetWatchAnmIdx(gmk_sv_work, tmp->TargetIdx);
      //���A�j���I������
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ICE_UNIT_IDX, obj_idx, anm_idx);
      //�X�C�b�`�A�j���I���҂�
      if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
      {
        //����ɔ����A�A�j���؂�ւ�
        SetupWallSwAnm(ptr, !gmk_sv_work->WallMoved[tmp->TargetIdx], tmp->TargetIdx, OBJ_KIND_SW);
        return GMEVENT_RES_FINISH; //�C�x���g�I��
      }
    }
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �ǉ�]�C�x���g
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
static GMEVENT_RESULT WallEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_ICE_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_ICE_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_ICE_TMP_ASSIGN_ID);

  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_ICE );
  }

  switch(*seq){
  case 0:  //�A�j���J�n
    {
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 obj_idx;
      u8 anm_idx;
      obj_idx = OBJ_WALL_1 + tmp->TargetIdx;
      anm_idx = GetWatchAnmIdx(gmk_sv_work, tmp->TargetIdx);
      //�A�j�����J�n
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ICE_UNIT_IDX, obj_idx, anm_idx );
      //�A�j����~����
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
      //SE�Đ�
      PMSND_PlaySE_byPlayerID( GYM_ICE_SE_GMK_MOVE, SEPLAYER_SE1 );

    }
    (*seq)++;
    break;
  case 1:
    {
      EXP_OBJ_ANM_CNT_PTR anm;
      u8 obj_idx;
      u8 anm_idx;

      obj_idx = OBJ_WALL_1 + tmp->TargetIdx;
      anm_idx = GetWatchAnmIdx(gmk_sv_work, tmp->TargetIdx);
      //���A�j���I������
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_ICE_UNIT_IDX, obj_idx, anm_idx);
      //�A�j���I���҂�
      if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
      {
        //SE�X�g�b�v
        PMSND_StopSE_byPlayerID( SEPLAYER_SE1 );
        //����ɔ����A�A�j���؂�ւ�
        SetupWallSwAnm(ptr, !gmk_sv_work->WallMoved[tmp->TargetIdx], tmp->TargetIdx, OBJ_KIND_WALL);
        //�Z�[�u���[�N��������
        gmk_sv_work->WallMoved[tmp->TargetIdx] = (!gmk_sv_work->WallMoved[tmp->TargetIdx]);
        return GMEVENT_RES_FINISH; //�C�x���g�I��
      }
    }
    break;
  }
  return GMEVENT_RES_CONTINUE;
}

