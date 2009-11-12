//======================================================================
/**
 * @file  gym_ground_ent.c
 * @brief  �n�ʃW���G���g�����X
 * @author  Saito
 * @date  09.11.11
 */
//======================================================================

#include "fieldmap.h"
//#include "gym_ground_sv.h"
#include "gym_ground_ent.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/fieldmap/gym_ground.naix"
#include "system/main.h"    //for HEAPID_FIELDMAP

//#include "sound/pm_sndsys.h"

//#include "gym_ground_se_def.h"

#define GYM_GROUND_ENT_UNIT_IDX (0)
#define GYM_GROUND_ENT_TMP_ASSIGN_ID  (1)

#define GRID_HALF_SIZE ((FIELD_CONST_GRID_SIZE/2)*FX32_ONE)

#define LIFT_ANM_NUM    (2)

#define LIFT_0_GX (7*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)
#define LIFT_0_GY (0*FIELD_CONST_GRID_FX32_SIZE)
#define LIFT_0_GZ (4*FIELD_CONST_GRID_FX32_SIZE+GRID_HALF_SIZE)

#define LIFT_MOVE_SPD (2*FX32_ONE)

#define BASE_HEIGHT ( 0*FIELD_CONST_GRID_FX32_SIZE )
#define DOWN_HEIGHT ( -5*FIELD_CONST_GRID_FX32_SIZE )



//�W���������̈ꎞ���[�N
typedef struct GYM_GROUND_ENT_TMP_tag
{
  fx32 NowHeight;
  fx32 DstHeight;
  fx32 AddVal;
  const VecFx32 *Watch;
  BOOL Exit;
}GYM_GROUND_ENT_TMP;

//���\�[�X�̕��я���
enum {
  RES_ID_LIFT_MDL = 0,
  RES_ID_LIFT_ANM_A,
  RES_ID_LIFT_ANM_B,
};

//�n�a�i�C���f�b�N�X
enum {
  OBJ_LIFT_0 = 0,
};

//--���\�[�X�֘A--
//�ǂݍ���3D���\�[�X
static const GFL_G3D_UTIL_RES g3Dutil_resTbl[] = {
	{ ARCID_GYM_GROUND, NARC_gym_ground_rif_anim01_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD

  { ARCID_GYM_GROUND, NARC_gym_ground_rif_anim01_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_GYM_GROUND, NARC_gym_ground_rif_anim02_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
};

//3D�A�j���@���t�g
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_lift[] = {
	{ RES_ID_LIFT_ANM_A,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_LIFT_ANM_B,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
  //���t�g
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

static GMEVENT_RESULT ExitLiftEvt( GMEVENT* event, int* seq, void* work );
static BOOL CheckArrive(GYM_GROUND_ENT_TMP *tmp);

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_GROUND_ENT_Setup(FIELDMAP_WORK *fieldWork)
{
  int i;
  GYM_GROUND_ENT_TMP *tmp;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldWork, GYM_GROUND_ENT_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(GYM_GROUND_ENT_TMP));

  //�K�v�ȃ��\�[�X�̗p��
  FLD_EXP_OBJ_AddUnit(ptr, &Setup, GYM_GROUND_ENT_UNIT_IDX );

  //���W�Z�b�g
  {
    int j;
    EXP_OBJ_ANM_CNT_PTR anm;
    VecFx32 pos = {LIFT_0_GX, LIFT_0_GY, LIFT_0_GZ};
    int obj_idx = OBJ_LIFT_0;
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_GROUND_ENT_UNIT_IDX, obj_idx);
    status->trans = pos;
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(ptr, GYM_GROUND_ENT_UNIT_IDX, obj_idx, TRUE);
    for (j=0;j<LIFT_ANM_NUM;j++)
    {
      //1��Đ��ݒ�
      EXP_OBJ_ANM_CNT_PTR anm;
      anm = FLD_EXP_OBJ_GetAnmCnt( ptr, GYM_GROUND_ENT_UNIT_IDX, obj_idx, j);
      FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
      //�A�j����~
      FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    }
    
    FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_GROUND_ENT_UNIT_IDX, obj_idx, 1, TRUE);
    FLD_EXP_OBJ_ValidCntAnm(ptr, GYM_GROUND_ENT_UNIT_IDX, obj_idx, 0, FALSE);
  }
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_GROUND_ENT_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_GROUND_ENT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_GROUND_ENT_TMP_ASSIGN_ID);

  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, GYM_GROUND_ENT_TMP_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, GYM_GROUND_ENT_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork     �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void GYM_GROUND_ENT_Move(FIELDMAP_WORK *fieldWork)
{
  int i;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
//  GYM_GROUND_ENT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_GROUND_ENT_TMP_ASSIGN_ID);

  if (GFL_UI_KEY_GetCont() & PAD_BUTTON_DEBUG){
    if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){
      GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
      GMEVENT *event;
      {
        event = GYM_GROUND_ENT_CreateExitLiftMoveEvt(gsys, TRUE);
        GAMESYSTEM_SetEvent(gsys, event);
      }
    }else if ( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
      GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
      GMEVENT *event;
      {
        event = GYM_GROUND_ENT_CreateExitLiftMoveEvt(gsys, FALSE);
        GAMESYSTEM_SetEvent(gsys, event);
      }
    }
  }

  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );
}

//--------------------------------------------------------------
/**
 * �o�����~�C�x���g�쐬
 * @param	      gsys        �Q�[���V�X�e���|�C���^
 * @param       inExit      �G���g�����X���o��i�W���ɍs���j�Ƃ� TRUE
 * @return      GMEVENT     �C�x���g�|�C���^
 */
//--------------------------------------------------------------
GMEVENT *GYM_GROUND_ENT_CreateExitLiftMoveEvt(GAMESYS_WORK *gsys, const BOOL inExit)
{
  GMEVENT * event;
  GYM_GROUND_ENT_TMP *tmp;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
    
  tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_GROUND_ENT_TMP_ASSIGN_ID);

  tmp->Exit = inExit;

  //�Ώۃ��t�g�̏�Ԃ��擾
  if ( inExit )
  {
    //��ʒu���ړ���ʒu
    tmp->NowHeight = BASE_HEIGHT;
    tmp->DstHeight = DOWN_HEIGHT;
  }
  else
  {
    //�ړ���ʒu����ʒu
    tmp->NowHeight = DOWN_HEIGHT;
    tmp->DstHeight = BASE_HEIGHT;
    
  }
  if ( tmp->DstHeight - tmp->NowHeight < 0 ) tmp->AddVal = -LIFT_MOVE_SPD;
  else tmp->AddVal = LIFT_MOVE_SPD;
  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, ExitLiftEvt, 0 );
  return event;
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
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  GYM_GROUND_ENT_TMP *tmp = GMK_TMP_WK_GetWork(fieldWork, GYM_GROUND_ENT_TMP_ASSIGN_ID);
  FIELD_CAMERA *camera = FIELDMAP_GetFieldCamera( fieldWork );

  switch(*seq){
  case 0:
    {
      //���݃E�H�b�`�^�[�Q�b�g��ۑ�
      tmp->Watch = FIELD_CAMERA_GetWatchTarget(camera);
      //�J�����̃o�C���h��؂�
      FIELD_CAMERA_FreeTarget(camera);
    }
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
        //���̃V�[�P���X��
        (*seq)++;
      }
      //�Ώۃ��t�g�����X�V
      {
        int obj_idx = OBJ_LIFT_0;
        GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, GYM_GROUND_ENT_UNIT_IDX, obj_idx);
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
    //�J�������ăo�C���h
    if ( (!tmp->Exit)&&(tmp->Watch != NULL) ){
      FIELD_CAMERA_BindTarget(camera, tmp->Watch);
    }
    return GMEVENT_RES_FINISH;
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
static BOOL CheckArrive(GYM_GROUND_ENT_TMP *tmp)
{
  if (tmp->AddVal < 0){
    if ( tmp->DstHeight >= tmp->NowHeight ) return TRUE;
  }else{
    if ( tmp->DstHeight <= tmp->NowHeight ) return TRUE;
  }

  return FALSE;
}


