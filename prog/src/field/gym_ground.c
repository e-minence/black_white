//======================================================================
/**
 * @file  gym_ground.c
 * @brief  �n�ʃW��
 * @author  Saito
 * @date  09.11.11
 */
//======================================================================

#include "fieldmap.h"
#include "gym_ground_sv.h"
#include "gym_ground.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/fieldmap/gym_ground.naix"
#include "system/main.h"    //for HEAPID_FIELDMAP

#include "../../../resource/fldmapdata/gimmick/gym_ground/gym_ground_local_def.h"

#include "sound/pm_sndsys.h"

#include "gym_ground_se_def.h"

#include "height_ex.h"    //for ExHeight

#define GYM_GROUND_UNIT_IDX (0)
#define GYM_GROUND_TMP_ASSIGN_ID  (1)

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)

#define FLOOR_RECT_NUM  (8)
#define LIFT_ANM_NUM    (2)

#define EXIT_LIFT_MOVE_SPD (2*FX32_ONE)

#define LIFT_MOVE_SPD (LIFT_MOVE_SPEED*FX32_ONE)
#define SP_LIFT_MOVE_SPD1 (SP_LIFT_MOVE_SPEED1*FX32_ONE)
#define SP_LIFT_MOVE_SPD2 (SP_LIFT_MOVE_SPEED2*FX32_ONE)

#define SP_LIFT_IDX (5)     //���C�����t�g�C���f�b�N�X
#define EXIT_LIFT_IDX (0xff)     //�o�����t�g�C���f�b�N�X�i���ʈ����j

#define LIFT_ON_MAX (5) //���t�g�ɏ���ő�l���i�����l�Ԃ��܂ށj

typedef struct SHAKE_WORK_tag
{
  u8 Seq;
  u8 AddCount;
  u8 ShakeCount;
  u8 ShakeMargin;
  fx32 AddVal;
  FIELD_CAMERA *Camera;
  const VecFx32 *Watch;
  VecFx32 OrgOffset;
}SHAKE_WORK;

//�W���������̈ꎞ���[�N
typedef struct GYM_GROUND_TMP_tag
{
  int TargetLiftIdx;
  fx32 NowHeight;
  fx32 DstHeight;
  fx32 AddVal;
  const VecFx32 *Watch;
  BOOL Exit;
  BOOL LiftMvStop;
  u16 StopTime;
  u16 MainLiftSeq;
  SHAKE_WORK ShakeWork;
  BOOL FogFadeFlg;
  int WallAnmWatch;
  MMDL *LiftMmdl[LIFT_ON_MAX];
}GYM_GROUND_TMP;

typedef struct FLOOR_RECT_tag
{
  int X;
  int Z;
  int W;
  int H;
  fx32 Height;
}FLOOR_RECT;

typedef struct LIFT_RECT_tag
{
  int X;
  int Z;
  int W;
  int H;
  fx32 Height[2];
}LIFT_RECT;

#define F0_HEIGHT ( HEIGHT0*FIELD_CONST_GRID_FX32_SIZE )
#define F1_HEIGHT ( HEIGHT1*FIELD_CONST_GRID_FX32_SIZE )
#define F2_HEIGHT ( HEIGHT2*FIELD_CONST_GRID_FX32_SIZE )
#define F3_HEIGHT ( HEIGHT3*FIELD_CONST_GRID_FX32_SIZE )
#define F4_HEIGHT ( HEIGHT4*FIELD_CONST_GRID_FX32_SIZE )

#define WALL_HEIGHT ( WALL_Y_POS*FIELD_CONST_GRID_FX32_SIZE )
#define WALL_OPEN_HEIGHT ( WALL_ANM_START_HEIHGT*FIELD_CONST_GRID_FX32_SIZE )

#define F1_1_X  (7)
#define F1_1_Z  (19)
#define F1_1_W  (17)
#define F1_1_H  (1)
#define F1_2_X  (7)
#define F1_2_Z  (23)
#define F1_2_W  (17)
#define F1_2_H  (1)
#define F1_3_X  (13)
#define F1_3_Z  (24)
#define F1_3_W  (5)
#define F1_3_H  (3)

#define F2_1_X  (7)
#define F2_1_Z  (19)
#define F2_1_W  (17)
#define F2_1_H  (1)
#define F2_2_X  (7)
#define F2_2_Z  (20)
#define F2_2_W  (1)
#define F2_2_H  (3)
#define F2_3_X  (7)
#define F2_3_Z  (23)
#define F2_3_W  (17)
#define F2_3_H  (1)

#define F3_1_X  (7)
#define F3_1_Z  (19)
#define F3_1_W  (6)
#define F3_1_H  (1)
#define F3_2_X  (7)
#define F3_2_Z  (23)
#define F3_2_W  (11)
#define F3_2_H  (1)

#define LIFT_WIDTH (5)
#define LIFT_HEIGHT (3)
#define SP_LIFT_WIDTH (3)
#define SP_LIFT_HEIGHT (3)

#define LIFT_1_X  (18)
#define LIFT_1_Z  (24)

#define LIFT_2_X  (18)
#define LIFT_2_Z  (20)

#define LIFT_3_X  (8)
#define LIFT_3_Z  (24)

#define LIFT_4_X  (13)
#define LIFT_4_Z  (24)

#define LIFT_5_X  (8)
#define LIFT_5_Z  (20)

#define LIFT_6_X  (14)
#define LIFT_6_Z  (20)

//�z�u���W
#define LIFT_0_GX (15*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_0_GY (F1_HEIGHT)
#define LIFT_0_GZ (25*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define LIFT_1_GX (20*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_1_GY (F1_HEIGHT)
#define LIFT_1_GZ (25*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_2_GX (20*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_2_GY (F2_HEIGHT)
#define LIFT_2_GZ (21*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_3_GX (10*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_3_GY (F2_HEIGHT)
#define LIFT_3_GZ (25*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_4_GX (15*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_4_GY (F3_HEIGHT)
#define LIFT_4_GZ (25*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_5_GX (10*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_5_GY (F3_HEIGHT)
#define LIFT_5_GZ (21*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_6_GX (15*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_6_GY (F1_HEIGHT)
#define LIFT_6_GZ (21*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define WALL_GX (15*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define WALL_GY (WALL_HEIGHT)
#define WALL_GZ (24*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

static const FLOOR_RECT FloorRect[FLOOR_RECT_NUM] = {
  {F1_1_X, F1_1_Z, F1_1_W, F1_1_H, F1_HEIGHT},
  {F1_2_X, F1_2_Z, F1_2_W, F1_2_H, F1_HEIGHT},
  {F1_3_X, F1_3_Z, F1_3_W, F1_3_H, F1_HEIGHT},

  {F2_1_X, F2_1_Z, F2_1_W, F2_1_H, F2_HEIGHT},
  {F2_2_X, F2_2_Z, F2_2_W, F2_2_H, F2_HEIGHT},
  {F2_3_X, F2_3_Z, F2_3_W, F2_3_H, F2_HEIGHT},

  {F3_1_X, F3_1_Z, F3_1_W, F3_1_H, F3_HEIGHT},
  {F3_2_X, F3_2_Z, F3_2_W, F3_2_H, F3_HEIGHT},
};

static const LIFT_RECT LiftRect[LIFT_NUM_MAX] = {
  {LIFT_1_X, LIFT_1_Z, LIFT_WIDTH, LIFT_HEIGHT, { F1_HEIGHT, F2_HEIGHT} },
  {LIFT_2_X, LIFT_2_Z, LIFT_WIDTH, LIFT_HEIGHT, { F2_HEIGHT, F1_HEIGHT} },
  {LIFT_3_X, LIFT_3_Z, LIFT_WIDTH, LIFT_HEIGHT, { F2_HEIGHT, F3_HEIGHT} },
  {LIFT_4_X, LIFT_4_Z, LIFT_WIDTH, LIFT_HEIGHT, { F3_HEIGHT, F2_HEIGHT} },
  {LIFT_5_X, LIFT_5_Z, LIFT_WIDTH, LIFT_HEIGHT, { F3_HEIGHT, F1_HEIGHT} },
  {LIFT_6_X, LIFT_6_Z, SP_LIFT_WIDTH, SP_LIFT_HEIGHT, { F1_HEIGHT, F4_HEIGHT} },
};

static const VecFx32 LiftBasePos[LIFT_NUM_MAX] = {
  { LIFT_1_GX, LIFT_1_GY, LIFT_1_GZ },
  { LIFT_2_GX, LIFT_2_GY, LIFT_2_GZ },
  { LIFT_3_GX, LIFT_3_GY, LIFT_3_GZ },
  { LIFT_4_GX, LIFT_4_GY, LIFT_4_GZ },
  { LIFT_5_GX, LIFT_5_GY, LIFT_5_GZ },
  { LIFT_6_GX, LIFT_6_GY, LIFT_6_GZ },
};

//���t�g���Ƃ̊�ʒu���㉺�ǂ���Ȃ̂����Ǘ����郊�X�g ��̏ꍇ��TRUE
static const BOOL LiftBaseUP[LIFT_NUM_MAX] = {
  TRUE,
  FALSE,
  TRUE,
  FALSE,
  FALSE,
  TRUE,
};

//���\�[�X�̕��я���
enum {
  RES_ID_LIFT_MDL = 0,
  RES_ID_SP_LIFT_MDL,
  RES_ID_WALL_MDL,
  RES_ID_LIFT_ANM_A,
  RES_ID_LIFT_ANM_B,
  RES_ID_SP_LIFT_ANM_A,
  RES_ID_SP_LIFT_ANM_B,
  RES_ID_WALL_ANM,
};

//�n�a�i�C���f�b�N�X
enum {
  OBJ_LIFT_1 = 0,
  OBJ_LIFT_2,
  OBJ_LIFT_3,
  OBJ_LIFT_4,
  OBJ_LIFT_5,
  OBJ_SP_LIFT,
  OBJ_WALL,
  OBJ_LIFT_0,
};

//--���\�[�X�֘A--
//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_GYM_GROUND, NARC_gym_ground_rif_anim01_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_GROUND, NARC_gym_ground_mainrif_anim01_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD
  { ARCID_GYM_GROUND, NARC_gym_ground_kakuheki_anim01_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD

  { ARCID_GYM_GROUND, NARC_gym_ground_rif_anim01_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_GROUND, NARC_gym_ground_rif_anim02_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_GROUND, NARC_gym_ground_mainrif_anim01_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_GROUND, NARC_gym_ground_mainrif_anim02_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_GROUND, NARC_gym_ground_kakuheki_anim01_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
};

//3D�A�j���@���t�g
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_lift[] = {
	{ RES_ID_LIFT_ANM_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_LIFT_ANM_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_splift[] = {
	{ RES_ID_SP_LIFT_ANM_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_SP_LIFT_ANM_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�u��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_wall[] = {
  { RES_ID_WALL_ANM,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
  //���t�g1
	{
		RES_ID_LIFT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_LIFT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_lift,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_lift),	//�A�j�����\�[�X��
	},
  //���t�g2
	{
		RES_ID_LIFT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_LIFT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_lift,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_lift),	//�A�j�����\�[�X��
	},
  //���t�g3
	{
		RES_ID_LIFT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_LIFT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_lift,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_lift),	//�A�j�����\�[�X��
	},
  //���t�g4
	{
		RES_ID_LIFT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_LIFT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_lift,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_lift),	//�A�j�����\�[�X��
	},
  //���t�g5
	{
		RES_ID_LIFT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_LIFT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_lift,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_lift),	//�A�j�����\�[�X��
	},
  //���C�����t�g
	{
		RES_ID_SP_LIFT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_SP_LIFT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_splift,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_splift),	//�A�j�����\�[�X��
	},
  //�u��
	{
		RES_ID_WALL_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_WALL_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_wall,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_wall),	//�A�j�����\�[�X��
	},
  //���t�g0
	{
		RES_ID_LIFT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_LIFT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_lift,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_lift),	//�A�j�����\�[�X��
	},
};

static const GFL_G3D_UTIL_SETUP Setup = {
  g3Dutil_resTbl,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};

static GMEVENT_RESULT UpDownEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT ExitLiftEvt( GMEVENT* event, int* seq, void* work );
static BOOL CheckArrive(GYM_GROUND_TMP *tmp);
static BOOL CheckLiftHit(u8 inLiftIdx, const int inX, const int inZ);
static void SetupLiftAnm( GYM_GROUND_SV_WORK *gmk_sv_work,
                          FLD_EXP_OBJ_CNT_PTR ptr,
                          const int inLiftIdx);
static int GetWatchLiftAnmIdx(GYM_GROUND_SV_WORK *gmk_sv_work, const int inLiftIdx);
static void FuncMainLiftOnly(GAMESYS_WORK *gsys);
static void SetExitLiftDefaultSt(FLD_EXP_OBJ_CNT_PTR ptr);

static void InitLiftShake(  const u8 inLiftIdx,
                            const fx32 inAddVal,
                            const BOOL inStart,
                            FIELD_CAMERA *camera,
                            SHAKE_WORK *outWork );
static BOOL ShakeLift(SHAKE_WORK *work);

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_GROUND_Setup(FIELDMAP_WORK *fieldWork)
{
  int i;
  EHL_PTR exHeightPtr;
  GYM_GROUND_SV_WORK *gmk_sv_work;
  GYM_GROUND_TMP *tmp;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  FIELD_FOG_WORK* fog = FIELDMAP_GetFieldFog( fieldWork );
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_GROUND );
  }

  //�ėp���[�N�m��
  tmp = GMK_TMP_WK_AllocWork
      (fieldWork, GYM_GROUND_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_GROUND_TMP));

  {
    FLDMAPPER * mapper = FIELDMAP_GetFieldG3Dmapper( fieldWork );
    exHeightPtr = FLDMAPPER_GetExHegihtPtr( mapper );
  }

  //�������Z�b�g
  for(i=0;i<FLOOR_RECT_NUM;i++)
  {
    EXH_SetUpHeightData( i,
              FloorRect[i].X, FloorRect[i].Z, FloorRect[i].W, FloorRect[i].H,
							FloorRect[i].Height,
              0x800000,   //�e����,
							exHeightPtr );
  }

  //���t�g�����Z�b�g
  for(i=0;i<LIFT_NUM_MAX;i++)
  {
    u8 height_idx = FLOOR_RECT_NUM+i;
    u8 idx;
    //�Z�[�u�f�[�^�ŕ���
    if ( gmk_sv_work->LiftMoved[i] ) idx = 1;
    else idx = 0;
      
    EXH_SetUpHeightData( height_idx,
              LiftRect[i].X, LiftRect[i].Z, LiftRect[i].W, LiftRect[i].H,
							LiftRect[i].Height[idx],
              0x800000,   //�e����
							exHeightPtr );
  }


  //�K�v�ȃ��\�[�X�̗p��
  FLD_EXP_OBJ_AddUnitByHandle(ptr, &Setup, GYM_GROUND_UNIT_IDX);
  //���W�Z�b�g
  for (i=0;i<LIFT_NUM_MAX;i++)
  {
    int j;
    int idx = OBJ_LIFT_1 + i;
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_GROUND_UNIT_IDX, idx);
    status->trans = LiftBasePos[i];
    //�Z�[�u�f�[�^�ŕ��򂵂ă��t�g�̂x���W���Z�b�g
    {
      u8 height;
      if ( gmk_sv_work->LiftMoved[i] ) height = 1;
      else height = 0;
      status->trans.y = LiftRect[i].Height[height];
    }

    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_GROUND_UNIT_IDX, idx, TRUE);

    for (j=0;j<LIFT_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_GROUND_UNIT_IDX, idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      //�A�j����~
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    }
    
    SetupLiftAnm(gmk_sv_work, ptr, i);
  }

  {
    int j;
    EXP_OBJ_ANM_CNT_PTR anm;
    VecFx32 pos = {LIFT_0_GX, LIFT_0_GY, LIFT_0_GZ};
    int obj_idx = OBJ_LIFT_0;
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_GROUND_UNIT_IDX, obj_idx);
    status->trans = pos;
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_GROUND_UNIT_IDX, obj_idx, TRUE);
    for (j=0;j<LIFT_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_GROUND_UNIT_IDX, obj_idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      //�A�j����~
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    }
    SetExitLiftDefaultSt(ptr);
  }
  
  //�u�Ǎ��W�Z�b�g
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    VecFx32 wall_pos = {WALL_GX, WALL_GY, WALL_GZ};
    int obj_idx = OBJ_WALL;
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_GROUND_UNIT_IDX, obj_idx);
    status->trans = wall_pos;
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_GROUND_UNIT_IDX, obj_idx, TRUE);

    //�A�j���L��
    FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_GROUND_UNIT_IDX, obj_idx, 0, TRUE);
    
    //1��Đ��ݒ�
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_GROUND_UNIT_IDX, obj_idx, 0);
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    //�A�j����~
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    //���Ɋu�ǊJ���Ă�����A�ŏI�t���[�����Z�b�g
    if ( gmk_sv_work->WallOpen ){
      fx32 last = FLD_EXP_OBJ_GetAnimeLastFrame(anm);
      //���X�g�t���[��
      FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_GROUND_UNIT_IDX, obj_idx, 0, last );
    }
  }

  //���@���u�ǂ�艺�ɂ���ꍇ�̓t�H�O�t�F�[�h��̐ݒ�ɏ�������
  {
    VecFx32 pos;
    FIELD_PLAYER *fld_player;
    fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    FIELD_PLAYER_GetPos( fld_player, &pos );
    if (pos.y <= WALL_HEIGHT)
    {
      FIELD_FOG_SetOffset( fog, FOG_OFFSET );
      FIELD_FOG_SetSlope( fog, FOG_SLOPE );
    }
  }
  
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
void GYM_GROUND_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_GROUND_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_GROUND_TMP_ASSIGN_ID);

  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GYM_GROUND_TMP_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, GYM_GROUND_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_GROUND_Move(FIELDMAP_WORK *fieldWork)
{
  int i;
  GYM_GROUND_SV_WORK *gmk_sv_work;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_GROUND_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_GROUND_TMP_ASSIGN_ID);
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_GROUND );
  }
#ifdef PM_DEBUG
  //�t�B�[���h�̎c��q�[�v���v�����g
  if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R ){
    int size = GFI_HEAP_GetHeapFreeSize(HEAPID_FIELDMAP);
    OS_Printf("size = %x\n",size);
  }
  
  if (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG){
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){
      GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
      GMEVENT *event;
      {
        u8 i;
        s16 gx,gz;
        const MMDL *fmmdl = FIELD_PLAYER_GetMMdl( FIELDMAP_GetFieldPlayer( fieldWork ) );
        gx = MMDL_GetGridPosX( fmmdl );
        gz = MMDL_GetGridPosZ( fmmdl );
        for (i=0;i<LIFT_NUM_MAX;i++){
          if ( CheckLiftHit(i, gx, gz) ) break;
        }
        if (i != LIFT_NUM_MAX)
        {
          event = GYM_GROUND_CreateLiftMoveEvt(gsys, i);
          GAMESYSTEM_SetEvent(gsys, event);
        }
      }
    }else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_A){
      GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
      GMEVENT *event;
      event = GYM_GROUND_CreateExitLiftMoveEvt(gsys, TRUE);
      GAMESYSTEM_SetEvent(gsys, event);
    }
  }
  else if (GFL_UI_KEY_GetTrg() & PAD_BUTTON_L)
  {
    //���@�̍�����ύX
    {
      VecFx32 pos;
      FIELD_PLAYER *fld_player;
      fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
      FIELD_PLAYER_GetPos( fld_player, &pos );
      pos.y -=  10*FIELD_CONST_GRID_FX32_SIZE;
      FIELD_PLAYER_SetPos( fld_player, &pos );
    }
  }
#endif  //PM_DEBUG

  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );
}

//--------------------------------------------------------------
/**
 * ���~�C�x���g�쐬
 * @param	      gsys        �Q�[���V�X�e���|�C���^
 * @param       inLiftIdx   ���t�g�C���f�b�N�X
 * @return      GMEVENT     �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *GYM_GROUND_CreateLiftMoveEvt(GAMESYS_WORK *gsys, const int inLiftIdx)
{
  fx32 speed;
  GMEVENT * event;
  GYM_GROUND_TMP *tmp;
  GYM_GROUND_SV_WORK *gmk_sv_work;
  {
    FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_GROUND );
    tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_GROUND_TMP_ASSIGN_ID);
  }

  if ( inLiftIdx >= LIFT_NUM_MAX ){
    GF_ASSERT_MSG(0, "LIFT_INDEX_OVER %d",inLiftIdx);
    return NULL;
  }

  tmp->TargetLiftIdx = inLiftIdx;
  //�Ώۃ��t�g�̏�Ԃ��擾
  if ( gmk_sv_work->LiftMoved[ inLiftIdx ] )
  {
    //�ړ���ʒu����ʒu
    tmp->NowHeight = LiftRect[inLiftIdx].Height[1];
    tmp->DstHeight = LiftRect[inLiftIdx].Height[0];
  }
  else
  {
    //��ʒu���ړ���ʒu
    tmp->NowHeight = LiftRect[inLiftIdx].Height[0];
    tmp->DstHeight = LiftRect[inLiftIdx].Height[1];
  }

  if (inLiftIdx == SP_LIFT_IDX){
    speed = SP_LIFT_MOVE_SPD1;
    //�t�H�O�t�F�[�h���N�G�X�g����
    tmp->FogFadeFlg = TRUE;
  }else{
    speed = LIFT_MOVE_SPD;
    //�t�H�O�t�F�[�h���N�G�X�g���Ȃ�
    tmp->FogFadeFlg = FALSE;
  }
  if ( tmp->DstHeight - tmp->NowHeight < 0 ) tmp->AddVal = -speed;
  else tmp->AddVal = speed;

  
  //�u�ǃA�j���Ď��t���O�I�t
  tmp->WallAnmWatch = 0;

  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, UpDownEvt, 0 );

  return event;
}

//--------------------------------------------------------------
/**
 * �o�����~�C�x���g�쐬
 * @param	      gsys        �Q�[���V�X�e���|�C���^
 * @param       inExit      �W������o��Ƃ� TRUE
 * @return      GMEVENT     �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *GYM_GROUND_CreateExitLiftMoveEvt(GAMESYS_WORK *gsys, const BOOL inExit)
{
  GMEVENT * event;
  GYM_GROUND_TMP *tmp;
  GYM_GROUND_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_GROUND );
    tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_GROUND_TMP_ASSIGN_ID);
  }

  tmp->Exit = inExit;

  //�Ώۃ��t�g�̏�Ԃ��擾
  if ( inExit )
  {
    //��ʒu���ړ���ʒu
    tmp->NowHeight = F1_HEIGHT;
    tmp->DstHeight = F0_HEIGHT;
  }
  else
  {
    //�ړ���ʒu����ʒu
    tmp->NowHeight = F0_HEIGHT;
    tmp->DstHeight = F1_HEIGHT;
    {
      int obj_idx = OBJ_LIFT_0;
      FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_GROUND_UNIT_IDX, obj_idx, 1, TRUE);
      FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_GROUND_UNIT_IDX, obj_idx, 0, FALSE);
    }
  }
  if ( tmp->DstHeight - tmp->NowHeight < 0 ) tmp->AddVal = -EXIT_LIFT_MOVE_SPD;
  else tmp->AddVal = EXIT_LIFT_MOVE_SPD;
  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, ExitLiftEvt, 0 );
  return event;
}

//--------------------------------------------------------------
/**
 * ���~�C�x���g
 * @param	  event   �C�x���g�|�C���^
 * @param   seq     �V�[�P���T
 * @param   work    ���[�N�|�C���^
 * @return  GMEVENT_RESULT  �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT UpDownEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_GROUND_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_GROUND_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_GROUND_TMP_ASSIGN_ID);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_GROUND );
  }

  switch(*seq){
  case 0:
    //�t�B�[���h�n�a�i�S����
    {
      VecFx32 player_pos;
      int i;
      int count = 0;
      u32 no = 0;
      MMDL *mmdl;
      MMDLSYS * mmdlsys = FIELDMAP_GetMMdlSys( fieldWork );

      for (i=0;i<LIFT_ON_MAX;i++)
      {
        tmp->LiftMmdl[i] = NULL;
      }
      {
        FIELD_PLAYER *fld_player;
        fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
        FIELD_PLAYER_GetPos( fld_player, &player_pos );
      }
      while( MMDLSYS_SearchUseMMdl(mmdlsys, &mmdl, &no) == TRUE )
      {
        //�O���b�h���W���擾
        s16 x = MMDL_GetGridPosX( mmdl );
        s16 z = MMDL_GetGridPosZ( mmdl );
        //���O����
        if ( CheckLiftHit(tmp->TargetLiftIdx, x, z) )
        {
          VecFx32 pos;
          MMDL_GetVectorPos( mmdl, &pos );

          //���@�Ɠ��������̂n�a�i���G���g���i�����̂�����l�����㉺1�O���b�h�̗P�\�������Ƃɂ���j
          if ( (player_pos.y-FIELD_CONST_GRID_FX32_SIZE < pos.y) &&
              (pos.y < player_pos.y+FIELD_CONST_GRID_FX32_SIZE)  )
          {
            if ( count >= LIFT_ON_MAX ){
              GF_ASSERT(0);
            }else{
              tmp->LiftMmdl[count++] = mmdl;
            }
          }
        }
      }
    }
    InitLiftShake(tmp->TargetLiftIdx, tmp->AddVal, TRUE, camera, &tmp->ShakeWork);
    (*seq)++;
    break;
  case 1:
    {
      SHAKE_WORK *shake = &tmp->ShakeWork;
      if ( ShakeLift(shake) ) (*seq)++;
    }
    break;
  case 2:
    //�J�����g���[�X��؂�
    FIELD_CAMERA_StopTraceRequest(camera);
    (*seq)++;
    break;
  case 3:
    {
      //�J�����g���[�X�������Ă���Ԃ͏�����i�߂Ȃ�
      if ( FIELD_CAMERA_CheckTrace(camera) ) break;
      PMSND_PlaySE(GYM_GROUND_SE_LIFT_MOVE);
      //�g���[�X�I��
      (*seq)++;
    }
    //NO BREAK
  case 4:
    //�ϓ���̍����ɏ�������
    if (!tmp->LiftMvStop){
      tmp->NowHeight += tmp->AddVal;
    }
    //�ړI�̍����ɓ��B�������H
    if(  CheckArrive(tmp) )
    {
      //�ړI�����ŏ㏑��
      tmp->NowHeight = tmp->DstHeight;
      //���t�g�U���Z�b�g�A�b�v
      InitLiftShake(tmp->TargetLiftIdx, tmp->AddVal, FALSE, camera, &tmp->ShakeWork);
      //���t�g��~��
      PMSND_PlaySE(GYM_GROUND_SE_LIFT_STOP);
      //���̃V�[�P���X��
      (*seq)++;
    }

    //�Ώۃ��t�g�����X�V
    {
      int obj_idx = OBJ_LIFT_1 + tmp->TargetLiftIdx;
      GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_GROUND_UNIT_IDX, obj_idx);
      status->trans.y = tmp->NowHeight;
    }

    {
      int i;
      for (i=0;i<LIFT_ON_MAX;i++){
        if ( tmp->LiftMmdl[i] != NULL){
          //������������
          MMDL *mmdl;
          const VecFx32 *mmdl_pos;
          VecFx32 pos;
          int gy;
          mmdl = tmp->LiftMmdl[i];
          //���W�X�V
          mmdl_pos = MMDL_GetVectorPosAddress( mmdl );
          pos = *mmdl_pos;
          pos.y = tmp->NowHeight;
          gy = SIZE_GRID_FX32( pos.y );
          MMDL_SetGridPosY( mmdl, gy );
          MMDL_SetVectorPos( mmdl, &pos );
        }
      }
    }
#if 0    
    //�t�B�[���h�n�a�i�S����
    {
      u32 no = 0;
      MMDL *mmdl;
      MMDLSYS * mmdlsys = FIELDMAP_GetMMdlSys( fieldWork );
      while( MMDLSYS_SearchUseMMdl(mmdlsys, &mmdl, &no) == TRUE )
      {
        //�O���b�h���W���擾
        s16 x = MMDL_GetGridPosX( mmdl );
        s16 z = MMDL_GetGridPosZ( mmdl );
        //���O����
        if ( CheckLiftHit(tmp->TargetLiftIdx, x, z) )
        {
          const VecFx32 *mmdl_pos;
          VecFx32 pos;
          int gy;
          //���W�X�V
          mmdl_pos = MMDL_GetVectorPosAddress( mmdl );
          pos = *mmdl_pos;
          pos.y = tmp->NowHeight;
          gy = SIZE_GRID_FX32( pos.y );
          MMDL_SetGridPosY( mmdl, gy );
          MMDL_SetVectorPos( mmdl, &pos );
        }
      }
    }
#endif    
    break;
  case 5:
    {
      SHAKE_WORK *shake = &tmp->ShakeWork;
      if ( ShakeLift(shake) ) (*seq)++;
    }
    break;
  case 6:
    //�J�����g���[�X�ĊJ
    FIELD_CAMERA_RestartTrace(camera);
    //�A�j���J�n
    {
      EXP_OBJ_ANM_CNT_PTR anm;
      int obj_idx;
      int anm_idx;
      obj_idx = OBJ_LIFT_1+tmp->TargetLiftIdx;
      anm_idx = GetWatchLiftAnmIdx(gmk_sv_work, tmp->TargetLiftIdx);
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_GROUND_UNIT_IDX, obj_idx, anm_idx );
      //�A�j����~����
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
    }
    (*seq)++;
    break;
  case 7:
    {
      EXP_OBJ_ANM_CNT_PTR anm;
      int obj_idx;
      int anm_idx;
      obj_idx = OBJ_LIFT_1+tmp->TargetLiftIdx;
      anm_idx = GetWatchLiftAnmIdx(gmk_sv_work, tmp->TargetLiftIdx);
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_GROUND_UNIT_IDX, obj_idx, anm_idx );
      //�A�j���҂�
      if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
      {
        u8 idx;
        //�Ώۃ��t�g�̃M�~�b�N�f�[�^��������
        if ( gmk_sv_work->LiftMoved[ tmp->TargetLiftIdx ] )
        {
          gmk_sv_work->LiftMoved[ tmp->TargetLiftIdx ] = FALSE;
          idx = 0;
        }
        else
        {
          gmk_sv_work->LiftMoved[ tmp->TargetLiftIdx ] = TRUE;
          idx = 1;
        }
        //�Ώۃ��t�g�̊g����������������
        {
          u8 height_idx;
          EHL_PTR exHeightPtr;
          {
            FLDMAPPER * mapper = FIELDMAP_GetFieldG3Dmapper( fieldWork );
            exHeightPtr = FLDMAPPER_GetExHegihtPtr( mapper );
          }
          height_idx = FLOOR_RECT_NUM + tmp->TargetLiftIdx;
          EXH_SetHeight( height_idx,
              					 LiftRect[tmp->TargetLiftIdx].Height[idx],
                         exHeightPtr );
        }

        //����ɔ����A�A�j���؂�ւ�
        SetupLiftAnm(gmk_sv_work, ptr, tmp->TargetLiftIdx);
        //OBJ�̃|�[�Y������
        ;
        //���C�����t�g�̂Ƃ��̂݃t�H�O�t�F�[�h�҂��V�[�P���X�ֈڍs
        if (tmp->TargetLiftIdx != SP_LIFT_IDX) return GMEVENT_RES_FINISH;
        else (*seq)++;
      }
    }
    break;
  case 8:
    {
      FIELD_FOG_WORK* fog = FIELDMAP_GetFieldFog( fieldWork );
      //���C�����t�g�̂Ƃ��̂݃t�H�O�t�F�[�h�҂�
      if ( FIELD_FOG_FADE_IsFade( fog ) ) break;    //�t�H�O�t�F�[�h��

      return GMEVENT_RES_FINISH;
    }
  }

  //���C�����t�g�̂Ƃ��݂̂̏���
  FuncMainLiftOnly( gsys );

  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �o�����~�C�x���g
 * @param	  event   �C�x���g�|�C���^
 * @param   seq     �V�[�P���T
 * @param   work    ���[�N�|�C���^
 * @return  GMEVENT_RESULT  �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT ExitLiftEvt( GMEVENT* event, int* seq, void* work )
{
  GYM_GROUND_SV_WORK *gmk_sv_work;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_GROUND_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_GROUND_TMP_ASSIGN_ID);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_GROUND );
  }

  switch(*seq){
  case 0:
    //@note Exit �� FLASE�̂Ƃ��� GYM_GROUND_SetEnterPos�֐��ŏ������s���Ă���͂��Ȃ̂ŁA�t�b�N����
    if (tmp->Exit)
    {
      //���݃E�H�b�`�^�[�Q�b�g��ۑ�
      tmp->Watch = FIELD_CAMERA_GetWatchTarget(camera);
      //�J�����̃o�C���h��؂�
      FIELD_CAMERA_FreeTarget(camera);
    }    
    //���t�g�ړ��r�d
    if ( !tmp->Exit ) PMSND_PlaySE(GYM_GROUND_SE_LIFT_MOVE_IN);
    else PMSND_PlaySE(GYM_GROUND_SE_LIFT_MOVE);
    (*seq)++;
    break;
  case 1:
    {
      //�ϓ���̍����ɏ�������
      tmp->NowHeight += tmp->AddVal;
      //�ړI�̍����ɓ��B�������H
      if(  CheckArrive(tmp) )
      {
        //�ړI�����ŏ㏑��
        tmp->NowHeight = tmp->DstHeight;
        //���t�g�U���Z�b�g�A�b�v
        if (!tmp->Exit)
        {
          InitLiftShake(EXIT_LIFT_IDX, tmp->AddVal, FALSE, camera, &tmp->ShakeWork);
          //���t�g��~�r�d
          PMSND_PlaySE(GYM_GROUND_SE_LIFT_STOP);
        }
        //���̃V�[�P���X��
        (*seq)++;
      }
      //�Ώۃ��t�g�����X�V
      {
        int obj_idx = OBJ_LIFT_0;
        GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_GROUND_UNIT_IDX, obj_idx);
        status->trans.y = tmp->NowHeight;
      }
      //���@�̍������X�V
      {
        VecFx32 pos;
        FIELD_PLAYER *fld_player;
        fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
        FIELD_PLAYER_GetPos( fld_player, &pos );
        pos.y = tmp->NowHeight;
        FIELD_PLAYER_SetPos( fld_player, &pos );
      }
    }
    break;
  case 2:
    if (tmp->Exit) return GMEVENT_RES_FINISH;
    else
    {
      //�U���I���܂ŏ������t�b�N
      {
        SHAKE_WORK *shake = &tmp->ShakeWork;
        if ( !ShakeLift(shake) ) break;
      }

      //�J�������ăo�C���h
      if ( (tmp->Watch != NULL) ){
        FIELD_CAMERA_BindTarget(camera, tmp->Watch);
      }
      //�A�j���J�n
      {
        EXP_OBJ_ANM_CNT_PTR anm;
        int obj_idx;
        int anm_idx;
        obj_idx = OBJ_LIFT_0;
        anm_idx = 1;
        anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_GROUND_UNIT_IDX, obj_idx, anm_idx );
        //�A�j����~����
        FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
      }
      //���̃V�[�P���X��
      (*seq)++;
    }
    break;
  case 3:
    {
      EXP_OBJ_ANM_CNT_PTR anm;
      int obj_idx;
      int anm_idx;
      obj_idx = OBJ_LIFT_0;
      anm_idx = 1;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_GROUND_UNIT_IDX, obj_idx, anm_idx );
      //�A�j���҂�
      if( FLD_EXP_OBJ_ChkAnmEnd(anm) ){
        SetExitLiftDefaultSt(ptr);
        return GMEVENT_RES_FINISH;
      }
    }
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ���t�g���B�`�F�b�N
 * @param	  GYM_GROUND_TMP *tmp
 * @return  BOOL      TRUE�œ��B
 */
//--------------------------------------------------------------
static BOOL CheckArrive(GYM_GROUND_TMP *tmp)
{
  if (tmp->AddVal < 0){
    if ( tmp->DstHeight >= tmp->NowHeight ) return TRUE;
  }else{
    if ( tmp->DstHeight <= tmp->NowHeight ) return TRUE;
  }

  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���t�g��`���`�F�b�N
 * @param	  inLiftIdx ���t�g�C���f�b�N�X
 * @param   inX       �Ώۂw�O���b�h���W
 * @param   inZ       �Ώ�Z�O���b�h���W
 * @return  BOOL      TRUE�ŋ�`��
 */
//--------------------------------------------------------------
static BOOL CheckLiftHit(u8 inLiftIdx, const int inX, const int inZ)
{
  int x = LiftRect[inLiftIdx].X;
  int z = LiftRect[inLiftIdx].Z;
  int width = LiftRect[inLiftIdx].W;
  int height = LiftRect[inLiftIdx].H;

  if ( (x<=inX)&&(inX<=x+width-1)&&
       (z<=inZ)&&(inZ<=z+height-1) ){
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
/**
 * ���t�g�A�j������
 * @param   gmk_sv_work   �M�~�b�N�Z�[�u���[�N
 * @param   ptr       �g���n�a�i�R���g���[���|�C���^
 * @param	  inLiftIdx ���t�g�C���f�b�N�X
 * @return  none
 */
//--------------------------------------------------------------
static void SetupLiftAnm(GYM_GROUND_SV_WORK *gmk_sv_work, FLD_EXP_OBJ_CNT_PTR ptr, const int inLiftIdx)
{
  EXP_OBJ_ANM_CNT_PTR anm;
  int valid_anm_idx, invalid_anm_idx;
  int obj_idx;

  obj_idx = OBJ_LIFT_1 + inLiftIdx;
  if ( gmk_sv_work->LiftMoved[inLiftIdx] )
  {
    if ( LiftBaseUP[inLiftIdx] )
    {
      //������A�j���L��
      valid_anm_idx = 0;
      invalid_anm_idx = 1;
    }
    else
    {
      //�と���A�j���L��
      valid_anm_idx = 1;
      invalid_anm_idx = 0;
    }
  }
  else
  {
    if ( LiftBaseUP[inLiftIdx] )
    {
      //�と���A�j���L��
      valid_anm_idx = 1;
      invalid_anm_idx = 0;
    }
    else
    {
      //������A�j���L��
      valid_anm_idx = 0;
      invalid_anm_idx = 1;
    }
  }
  
  FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_GROUND_UNIT_IDX, obj_idx, valid_anm_idx, TRUE);
  FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_GROUND_UNIT_IDX, obj_idx, invalid_anm_idx, FALSE);
  //������
  FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_GROUND_UNIT_IDX, obj_idx, valid_anm_idx, 0 );
  //��~�ɂ��Ă���
  anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_GROUND_UNIT_IDX, obj_idx, valid_anm_idx);
  FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
}

//--------------------------------------------------------------
/**
 * �I���Ď����郊�t�g�A�j���C���f�b�N�X���擾
 * @param   gmk_sv_work   �M�~�b�N�Z�[�u���[�N
 * @param	  inLiftIdx ���t�g�C���f�b�N�X
 * @return  int     �I���Ď�����A�j���C���f�b�N�X�@0�`1
 */
//--------------------------------------------------------------
static int GetWatchLiftAnmIdx(GYM_GROUND_SV_WORK *gmk_sv_work, const int inLiftIdx)
{
  int valid_anm_idx;

  if ( gmk_sv_work->LiftMoved[inLiftIdx] )
  {
    if ( LiftBaseUP[inLiftIdx] )
    {
      //������A�j���L��
      valid_anm_idx = 0;
    }
    else
    {
      //�と���A�j���L��
      valid_anm_idx = 1;
    }
  }
  else
  {
    if ( LiftBaseUP[inLiftIdx] )
    {
      //�と���A�j���L��
      valid_anm_idx = 1;
    }
    else
    {
      //������A�j���L��
      valid_anm_idx = 0;
    }
  }

  return valid_anm_idx;
}

//--------------------------------------------------------------
/**
 * ���C�����t�g�݂̂̏���
 * @param   gsys      �Q�[���V�X�e���|�C���^
 * @return  none
 */
//--------------------------------------------------------------
static void FuncMainLiftOnly(GAMESYS_WORK *gsys)
{
  GYM_GROUND_SV_WORK *gmk_sv_work;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_GROUND_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_GROUND_TMP_ASSIGN_ID);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );
  {
    GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldWork ) );
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_GYM_GROUND );
  }
  
  //���C�����t�g�݂̂̏���
  if (tmp->TargetLiftIdx != SP_LIFT_IDX) return;
  
  //���t�g�̍������u�ǃI�[�v���J�n���鍂���ɒB�������H
  if (!gmk_sv_work->WallOpen && tmp->NowHeight<=WALL_OPEN_HEIGHT){
    EXP_OBJ_ANM_CNT_PTR anm;
    int obj_idx = OBJ_WALL;
    gmk_sv_work->WallOpen = TRUE;
    {
      //�u�ǃA�j���[�V�����J�n
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_GROUND_UNIT_IDX, obj_idx, 0);
      //�A�j���J�n
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
      //�u�ǊJ����
      PMSND_PlaySE(GYM_GROUND_SE_WALL_OPEN);
    }
    tmp->NowHeight = WALL_OPEN_HEIGHT;
    tmp->LiftMvStop = TRUE;
    tmp->StopTime = 0;
    tmp->MainLiftSeq = 0;

    //�u�ǃA�j���I���Ď��t���O�I��
    tmp->WallAnmWatch = 1;
  }

  if ( tmp->LiftMvStop ){
    switch(tmp->MainLiftSeq){
    case 0:
      InitLiftShake(tmp->TargetLiftIdx, tmp->AddVal, FALSE, camera, &tmp->ShakeWork);
      //���t�g��~�r�d
      PMSND_PlaySE(GYM_GROUND_SE_LIFT_STOP);
      tmp->MainLiftSeq++;
      break;
    case 1:
      {
        SHAKE_WORK *shake = &tmp->ShakeWork;
        if ( ShakeLift(shake) )
        {
          //��~�^�C�}�[�J�E���g
          tmp->StopTime++;
          if (tmp->StopTime >= WALL_ANM_WAIT)
          {
            tmp->AddVal = -SP_LIFT_MOVE_SPD2;   //�X�s�[�h�ύX
            tmp->LiftMvStop = FALSE;            //�X�g�b�v����
            //���t�g�ړ��J�n�ĊJ
            PMSND_PlaySE(GYM_GROUND_SE_LIFT_MOVE);
          }
        }
      }
    }   //end switch
  }

  //�t�H�O����
  if (tmp->FogFadeFlg){
    //�t�H�O�t�F�[�h�J�n�ʒu�ɓ��B�������H
    BOOL fog_start = FALSE;
    s32 fog_offset, fog_slope;

    //�i�s�����ŕ���
    if ( tmp->AddVal>=0 )   //�㏸
    {
      if (tmp->NowHeight >= UP_FOG_FADE_START*FIELD_CONST_GRID_FX32_SIZE)
      {
        FIELD_ZONEFOGLIGHT * zone_fog = FIELDMAP_GetFieldZoneFog( fieldWork );
        fog_start = TRUE;
        fog_offset = FIELD_ZONEFOGLIGHT_GetOffset( zone_fog );
        fog_slope = FIELD_ZONEFOGLIGHT_GetSlope( zone_fog );
        NOZOMU_Printf("fog::%d,%d\n",fog_offset, fog_slope);
      }
    }
    else                    //���~
    {
      if (tmp->NowHeight <= DOWN_FOG_FADE_START*FIELD_CONST_GRID_FX32_SIZE)
      {
        fog_start = TRUE;
        fog_offset = FOG_OFFSET;
        fog_slope = FOG_SLOPE;
      }
    }

    if (fog_start)
    {
      u16 fog_sync;
      FIELD_FOG_WORK* fog = FIELDMAP_GetFieldFog( fieldWork );
      //���t�g�̈ړ����x�ŕ���
      if (tmp->AddVal == -SP_LIFT_MOVE_SPD2) fog_sync = FOG_FADE_SPEED_SLOW;
      else fog_sync = FOG_FADE_SPEED_FAST;

      FIELD_FOG_FADE_Init( fog, fog_offset, fog_slope, fog_sync );
      tmp->FogFadeFlg = FALSE;
    }
  }

  //�u�ǃA�j���Ď�
  if (tmp->WallAnmWatch)
  {
    if ( tmp->WallAnmWatch == 1 ) tmp->WallAnmWatch = 2;    //�t���O�𗧂Ă��V���N�͏������Ȃ�
    else
    {
      EXP_OBJ_ANM_CNT_PTR anm;
      int obj_idx = OBJ_WALL;
      //�u�ǃA�j���[�V�����I���Ď�
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_GROUND_UNIT_IDX, obj_idx, 0);
      if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
      {
        PMSND_StopSE_byPlayerID( PMSND_GetSE_DefaultPlayerID( GYM_GROUND_SE_WALL_OPEN ) );
        //�u�ǃA�j���Ď��t���O�I�t
        tmp->WallAnmWatch = 0;
      }
    }
  }
}

//--------------------------------------------------------------
/**
 * �o�����t�g�A�j��������ԃZ�b�g
 * @param	  FLD_EXP_OBJ_CNT_PTR ptr   �g���n�a�i�R���g���[���|�C���^
 * @return  none
 */
//--------------------------------------------------------------
static void SetExitLiftDefaultSt(FLD_EXP_OBJ_CNT_PTR ptr)
{
  EXP_OBJ_ANM_CNT_PTR anm;
  int obj_idx;
  int anm_idx;
  obj_idx = OBJ_LIFT_0;
  anm_idx = 0;

  FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_GROUND_UNIT_IDX, obj_idx, 0, TRUE);
  FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_GROUND_UNIT_IDX, obj_idx, 1, FALSE);
  //������
  FLD_EXP_OBJ_SetObjAnmFrm( ptr, GYM_GROUND_UNIT_IDX, obj_idx, anm_idx, 0 );
  //��~�ɂ��Ă���
  anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_GROUND_UNIT_IDX, obj_idx, anm_idx);
  FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
}

//--------------------------------------------------------------
/**
 * ���t�g�U��������
 * @param	  inLiftIdx   ���t�g�C���f�b�N�X
 * @param   inAddVal       ���~���x
 * @param   inStart     �N���� OR ������ TRUE�ŋN����
 * @param   camera      �t�B�[���h�J�����|�C���^
 * @param   outWork     �i�[�o�b�t�@
 * @return  none
 */
//--------------------------------------------------------------
static void InitLiftShake(  const u8 inLiftIdx,
                            const fx32 inAddVal,
                            const BOOL inStart,
                            FIELD_CAMERA *camera,
                            SHAKE_WORK *outWork )
{
  SHAKE_WORK *work;

  work = outWork;
  work->Seq = 0;
  work->AddCount = 0;
  work->Camera = camera;
  //���݃E�H�b�`�^�[�Q�b�g��ۑ�
  work->Watch = FIELD_CAMERA_GetWatchTarget(camera);
  //���A�I�t�Z�b�g�l�ۑ�
  FIELD_CAMERA_GetTargetOffset( camera, &work->OrgOffset );

  if (inStart)
  {
    work->AddVal = LIFT_START_SHAKE_VAL*FX32_ONE;
    work->ShakeCount = LIFT_START_SHAKE_COUNT;
  }
  else
  {
    //���t�g�ʂɐU��������
    if (inLiftIdx == SP_LIFT_IDX)
    {
      work->AddVal = SP_LIFT_SHAKE_VAL*FX32_ONE;
      work->ShakeCount = SP_LIFT_SHAKE_COUNT;
    }
    else if ( inLiftIdx == EXIT_LIFT_IDX )
    {
      work->AddVal = LIFT_SHAKE_VAL*FX32_ONE;
      work->ShakeCount = LIFT_SHAKE_COUNT;
    }
    else
    {
      work->AddVal = LIFT_SHAKE_VAL*FX32_ONE;
      work->ShakeCount = LIFT_SHAKE_COUNT;
    }
  }
  //�i�s�����ŉ��Z�l�ύX
  if (inAddVal < 0) work->AddVal *= -1;
}

//--------------------------------------------------------------
/**
 * ���t�g�U���֐�
 * @param	  work    �U�����[�N�|�C���^
 * @return  BOOL    TRUE�ŐU���I��
 */
//--------------------------------------------------------------
static BOOL ShakeLift(SHAKE_WORK *work)
{
  switch(work->Seq){
  case 0:
    //�J�����̃o�C���h��؂�
    FIELD_CAMERA_FreeTarget(work->Camera);
    
    work->Seq++;
    //none break
  case 1:
    if (work->ShakeMargin % SHAKE_MARGIN == 0)
    {
      //�l���Z
      {
        VecFx32 offset;
        FIELD_CAMERA_GetTargetOffset( work->Camera, &offset );
        offset.y += work->AddVal;
        FIELD_CAMERA_SetTargetOffset( work->Camera, &offset );
      }
      //�l�t�]
      work->AddVal *= -1;
      //���Z�񐔃C���N�������g
      work->AddCount++;
      //�I������
      if ( work->AddCount >= work->ShakeCount*2 ){
        //�J�������ăo�C���h
        if ( (work->Watch != NULL) ) FIELD_CAMERA_BindTarget(work->Camera, work->Watch);

        //�I�t�Z�b�g�߂�
        FIELD_CAMERA_SetTargetOffset( work->Camera, &work->OrgOffset );

        return TRUE;
      }
    }
    work->ShakeMargin = (work->ShakeMargin+1)%SHAKE_MARGIN;
  }

  return FALSE;
}

//--------------------------------------------------------------
/**
 * ������ʒu�Ƀ��t�g�Ǝ��@���ړ�������
 * @param	      gsys        �Q�[���V�X�e���|�C���^
 * @param       inLiftIdx   ���t�g�C���f�b�N�X
 * @return      GMEVENT     �C�x���g�|�C���^
 */
//--------------------------------------------------------------
void GYM_GROUND_SetEnterPos(GAMESYS_WORK *gsys)
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_GROUND_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_GROUND_TMP_ASSIGN_ID);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );
  
  //���݃E�H�b�`�^�[�Q�b�g��ۑ�
  tmp->Watch = FIELD_CAMERA_GetWatchTarget(camera);
  //�J�����̃o�C���h��؂�
  FIELD_CAMERA_FreeTarget(camera);
  
  //�Ώۃ��t�g�����X�V
  {
    int obj_idx = OBJ_LIFT_0;
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_GROUND_UNIT_IDX, obj_idx);
    status->trans.y = F0_HEIGHT;
  }
  //���@�̍������X�V
  {
    VecFx32 pos;
    FIELD_PLAYER *fld_player;
    fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    FIELD_PLAYER_GetPos( fld_player, &pos );
    pos.y = F0_HEIGHT;
    FIELD_PLAYER_SetPos( fld_player, &pos );
  }
}

