//======================================================================
/**
 * @file  field_gimmick_t01.c
 * @brief  �s01 �M�~�b�N
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "fieldmap.h" 
#include "field_gimmick_t01.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "system/gfl_use.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/arc_def.h"
#include "arc/fieldmap/t01_gmk.naix"
#include "gmk_tmp_wk.h"
#include "system/rtc_tool.h"  //for PM_RTC_GetTimeZone
#include "field_gimmick_t01_sv.h"
#include "field_gimmick_t01_se_def.h"

#define EXPOBJ_UNIT_IDX (0)
#define T01_TMP_ASSIGN_ID  (1)

#define BIRD_X  (FIELD_CONST_GRID_FX32_SIZE * 784)
#define BIRD_Y  (FX32_ONE * 0)
#define BIRD_Z  (FIELD_CONST_GRID_FX32_SIZE * 752)

#define BIRD_ANM_NUM  (2)

#define DAY_SE_PLAY_FRM (40 * FX32_ONE)   //�P�H�x��ĉH�΂����t���[��

//==========================================================================================
// ��3D���\�[�X
//==========================================================================================
// ���\�[�X
typedef enum {
  RES_BIRD_NSBMD,   //���̃��f��
  RES_BIRD_NSBCA,   //���A�j��
  RES_BIRD_NSBTP,   //���A�j��
} RES_INDEX;

//���p
static const GFL_G3D_UTIL_RES res_table_morning[] = 
{
  { ARCID_T01_GMK, NARC_t01_gmk_t01_bird01_nsbmd, GFL_G3D_UTIL_RESARC },  // ���̃��f��
  { ARCID_T01_GMK, NARC_t01_gmk_t01_bird01_nsbca, GFL_G3D_UTIL_RESARC },  // ���̃A�j��
  { ARCID_T01_GMK, NARC_t01_gmk_t01_bird01_nsbtp, GFL_G3D_UTIL_RESARC },  // ���̃A�j��
};

//��p
static const GFL_G3D_UTIL_RES res_table_night[] = 
{
  { ARCID_T01_GMK, NARC_t01_gmk_t01_bird02_nsbmd, GFL_G3D_UTIL_RESARC },  // ���̃��f��
  { ARCID_T01_GMK, NARC_t01_gmk_t01_bird02_nsbca, GFL_G3D_UTIL_RESARC },  // ���̃A�j��
  { ARCID_T01_GMK, NARC_t01_gmk_t01_bird02_nsbtp, GFL_G3D_UTIL_RESARC },  // ���̃A�j��
};

// �I�u�W�F�N�g
typedef enum {
  OBJ_BIRD,  // ��
} OBJ_INDEX;

//3D�A�j��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTb[] = {
  { RES_BIRD_NSBCA,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_BIRD_NSBTP,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};


static const GFL_G3D_UTIL_OBJ obj_table[] = 
{
  { RES_BIRD_NSBMD, 0, RES_BIRD_NSBMD, g3Dutil_anmTb, NELEMS(g3Dutil_anmTb) },  // ��
};

//���p
static const GFL_G3D_UTIL_SETUP SetupMorning = {
  res_table_morning,				//���\�[�X�e�[�u��
	NELEMS(res_table_morning),		//���\�[�X��
	obj_table,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(obj_table),		//�I�u�W�F�N�g��
};

//��p
static const GFL_G3D_UTIL_SETUP SetupNight = {
  res_table_night,				//���\�[�X�e�[�u��
	NELEMS(res_table_night),		//���\�[�X��
	obj_table,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(obj_table),		//�I�u�W�F�N�g��
};

typedef struct
{ 
  BOOL Move;
  BOOL IsNight;
} T01_GMK_WORK;

static GMEVENT_RESULT EndChkEvt( GMEVENT* event, int* seq, void* work );


//------------------------------------------------------------------------------------------
/**
 * @brief �������֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void T01_GIMMICK_Setup( FIELDMAP_WORK* fieldmap )
{
  int i;
  T01_SV_WORK *gmk_sv_work;
  T01_GMK_WORK* gmk_work;  // �M�~�b�N�Ǘ����[�N
  FLD_EXP_OBJ_CNT_PTR exobj_cnt;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldmap ) );
  {
    GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
    gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_T01 );
  }

  exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldmap, T01_TMP_ASSIGN_ID, FIELDMAP_GetHeapID(fieldmap), sizeof(T01_GMK_WORK));
  gmk_work = GMK_TMP_WK_GetWork(fieldmap, T01_TMP_ASSIGN_ID);

  //�Z�b�g�A�b�v���̓M�~�b�N�𓮍삳���Ȃ�
  gmk_work->Move = FALSE;

  //���ԑтŕ���
  {
    int season, time_zone;
    season = GAMEDATA_GetSeasonID(gamedata);
    time_zone = PM_RTC_GetTimeZone(season);
    // �g���I�u�W�F�N�g�̃��j�b�g��ǉ�
    if ( (time_zone == TIMEZONE_NIGHT) || (time_zone == TIMEZONE_MIDNIGHT) )
    {
      FLD_EXP_OBJ_AddUnitByHandle( exobj_cnt, &SetupNight, EXPOBJ_UNIT_IDX );
      gmk_work->IsNight = TRUE;
    }
    else
    {
      FLD_EXP_OBJ_AddUnitByHandle(exobj_cnt, &SetupMorning, EXPOBJ_UNIT_IDX);
      gmk_work->IsNight = FALSE;
    }
  }

  {
    VecFx32 pos = { BIRD_X, BIRD_Y, BIRD_Z };
    GFL_G3D_OBJSTATUS *status = FLD_EXP_OBJ_GetUnitObjStatus(exobj_cnt, EXPOBJ_UNIT_IDX, 0);
    status->trans = pos;
    //�J�����O����
    FLD_EXP_OBJ_SetCulling(exobj_cnt, EXPOBJ_UNIT_IDX, 0, TRUE);
    {
      BOOL vanish;
      if ( gmk_sv_work->Disp ) vanish = FALSE;   //�\��
      else vanish = TRUE;                      //��\��
      FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, 0, vanish);
    }
  }

  //�A�j���̏�Ԃ�������
  for (i=0;i<BIRD_ANM_NUM;i++)
  {
    BOOL valid;
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BIRD, i);
    //1��Đ�
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    //�A�j����~
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    if ( gmk_sv_work->Disp )
    {
      //�L��
      valid = TRUE;
    }
    else
    {
      valid = FALSE;
    }
    FLD_EXP_OBJ_ValidCntAnm(exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BIRD, i, valid);
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �I���֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void T01_GIMMICK_End( FIELDMAP_WORK* fieldmap )
{
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  //���j�b�g�j��
  FLD_EXP_OBJ_DelUnit( exobj_cnt, EXPOBJ_UNIT_IDX );
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldmap, T01_TMP_ASSIGN_ID);
}

//------------------------------------------------------------------------------------------
/**
 * @brief ����֐�
 *
 * @param fieldmap �M�~�b�N����t�B�[���h�}�b�v
 */
//------------------------------------------------------------------------------------------
void T01_GIMMICK_Move( FIELDMAP_WORK* fieldmap )
{
  FLD_EXP_OBJ_CNT_PTR exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  T01_GMK_WORK* gmk_work = GMK_TMP_WK_GetWork(fieldmap, T01_TMP_ASSIGN_ID);

  if ( gmk_work->Move )
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    //�A�j���[�V�����Đ�
    FLD_EXP_OBJ_PlayAnime( exobj_cnt );

    if (!gmk_work->IsNight)
    {
      fx32 frm;
      //�A�j���t���[���擾
      frm = FLD_EXP_OBJ_GetObjAnmFrm( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BIRD, 0 );
      if ( frm == DAY_SE_PLAY_FRM ) PMSND_PlaySE( T01_GMK_DAY_SE02 );      //��2
    }

    //�C���f�b�N�X0�̃A�j���ŏI�����肷��
    anm = FLD_EXP_OBJ_GetAnmCnt( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BIRD, 0);
    //�I���`�F�b�N
    if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
    {
      //�n�a�i��\��
      FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, 0, TRUE);
      gmk_work->Move = FALSE;    //�M�~�b�N��~
      {
        T01_SV_WORK *gmk_sv_work;
        GAMEDATA *gamedata = GAMESYSTEM_GetGameData( FIELDMAP_GetGameSysWork( fieldmap ) );
        GIMMICKWORK *gmkwork = GAMEDATA_GetGimmickWork(gamedata);
        gmk_sv_work = GIMMICKWORK_Get( gmkwork, FLD_GIMMICK_T01 );
        gmk_sv_work->Disp = FALSE;    //��\��
      }
    }    
  }
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�A�j���J�n
 *
 * @param fieldmap �t�B�[���h�}�b�v�|�C���^
 */
//------------------------------------------------------------------------------------------
void T01_GIMMICK_Start( FIELDMAP_WORK* fieldmap )
{
  int i;
  FLD_EXP_OBJ_CNT_PTR exobj_cnt;
  T01_GMK_WORK* gmk_work;
  exobj_cnt = FIELDMAP_GetExpObjCntPtr( fieldmap );
  gmk_work = GMK_TMP_WK_GetWork(fieldmap, T01_TMP_ASSIGN_ID);
  gmk_work->Move = TRUE;    //����J�n
  //�\��
  FLD_EXP_OBJ_SetVanish(exobj_cnt, EXPOBJ_UNIT_IDX, 0, FALSE);
  //�A�j���̏�Ԃ�������
  for (i=0;i<BIRD_ANM_NUM;i++)
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( exobj_cnt, EXPOBJ_UNIT_IDX, OBJ_BIRD, i);
    //�A�j���Đ�
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
  }
  //SE�Đ�
  if ( gmk_work->IsNight ) PMSND_PlaySE( T01_GMK_NIGHT_SE );    //��
  else PMSND_PlaySE( T01_GMK_DAY_SE01 );      //��1
}

//------------------------------------------------------------------------------------------
/**
 * @brief �M�~�b�N�A�j���I���Ď��C�x���g�쐬
 *
 * @param gsys �Q�[���V�X�e���|�C���^
 */
//------------------------------------------------------------------------------------------
GMEVENT *T01_GIMMICK_CreateEndChkEvt( GAMESYS_WORK *gsys )
{
  GMEVENT * event;
  event = GMEVENT_Create( gsys, NULL, EndChkEvt, 0 );

  return event;
}

//--------------------------------------------------------------
/**
 * �A�j���I���Ď��C�x���g
 * @param     event	            �C�x���g�|�C���^
 * @param     seq               �V�[�P���T
 * @param     work              ���[�N�|�C���^
 * @return    GMEVENT_RESULT   �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT EndChkEvt( GMEVENT* event, int* seq, void* work )
{
  EXP_OBJ_ANM_CNT_PTR anm;
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  T01_GMK_WORK* gmk_work = GMK_TMP_WK_GetWork(fieldWork, T01_TMP_ASSIGN_ID);

  //�I���`�F�b�N
  if ( gmk_work->Move == FALSE ) return GMEVENT_RES_FINISH;    //�A�j���I��
  
  return GMEVENT_RES_CONTINUE;
}


