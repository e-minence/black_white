//======================================================================
/**
 * @file  sp_poke_gimmick.c
 * @brief  �z�z�n�|�P�����M�~�b�N
 * @author  Saito
 */
//======================================================================

#include "fieldmap.h"
#include "sp_poke_gimmick.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/fieldmap/sppoke.naix"

#include "sound/pm_sndsys.h"
#include "sp_poke_gimmick_se_def.h"

#define SPPOKE_GMK_ASSIGN_ID    (1)
#define SPPOKE_UNIT_IDX    (0)                      //����|�P�C�x���g���j�b�g�C���f�b�N�X
#define SPPOKE_TRIO_UNIT_IDX    (SPPOKE_UNIT_IDX)   //�O�e�m�C�x���g���j�b�g�C���f�b�N�X
#define SPPOKE_MERO_UNIT_IDX    (SPPOKE_UNIT_IDX)   //�����f�B�A�C�x���g���j�b�g�C���f�b�N�X
#define BALL_ANM_NUM  (3)

#define BALL_OUT_TIMMING  (5)
#define BALL_IN_TIMMING (34)

typedef struct SPPOKE_GMK_WK_tag
{
  BALL_ANM_TYPE AnmType;    //�^�[�Q�b�g�ƂȂ�A�j���^�C�v���i�[����
  VecFx32 BallStart;
  VecFx32 BallEnd;
  fx32 Height;
  u16 Sync;
  u16 NowSync;

  void *Work;
}SPPOKE_GMK_WK;

//==============================================================================================
/**
 @note    ���O�e�m�ƃ����f�B�A�Ń{�[�����\�[�X�͋��L����̂ŁA���\�[�X�ԍ��͈�v����悤�ɂ���
*/
//==============================================================================================

//���\�[�X�̕��я���
enum {
  RES_ID_BALL_OUT_MDL = 0,
  RES_ID_BALL_IN_MDL,

  RES_ID_BALL_OUT_ANM1,
  RES_ID_BALL_OUT_ANM2,
  RES_ID_BALL_OUT_ANM3,
  RES_ID_BALL_IN_ANM1,
  RES_ID_BALL_IN_ANM2,
  RES_ID_BALL_IN_ANM3,
};


//�n�a�i�C���f�b�N�X
enum {
  OBJ_BALL_OUT = 0,
  OBJ_BALL_IN,
};

//==========================================================================
/**
 ���\�[�X���ʕ���
*/
//==========================================================================

//3D�A�j���@�{�[���A�E�g
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_ball_out[] = {
  { RES_ID_BALL_OUT_ANM1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_BALL_OUT_ANM2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_BALL_OUT_ANM3,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�{�[���A�E�g
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_ball_in[] = {
  { RES_ID_BALL_IN_ANM1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_BALL_IN_ANM2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_BALL_IN_ANM3,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//==========================================================================
/**
 ���\�[�X
*/
//==========================================================================

static const GFL_G3D_UTIL_RES g3Dutil_resTbl_trio[] = {
	{ ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD �{�[���A�E�g
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_in_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD �{�[���C��

  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbca, GFL_G3D_UTIL_RESARC }, //ICA �{�[���A�E�g�A�j��
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbma, GFL_G3D_UTIL_RESARC }, //IMA
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_in_nsbca, GFL_G3D_UTIL_RESARC }, //ICA �{�[���C���A�j��
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_in_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_in_nsbma, GFL_G3D_UTIL_RESARC }, //IMA
};

//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl_trio[] = {
  //�o�������X�^�[�{�[��
  {
		RES_ID_BALL_OUT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BALL_OUT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_ball_out,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_ball_out),	//�A�j�����\�[�X��
	},
  //�i�[�����X�^�[�{�[��
  {
		RES_ID_BALL_IN_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BALL_IN_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_ball_in,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_ball_in),	//�A�j�����\�[�X��
	},
};

static const GFL_G3D_UTIL_SETUP SetupTrio = {
  g3Dutil_resTbl_trio,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl_trio),		//���\�[�X��
	g3Dutil_objTbl_trio,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl_trio),		//�I�u�W�F�N�g��
};

//==========================================================================
/**
 �����f�B�A�֘A���\�[�X
*/
//==========================================================================
static const GFL_G3D_UTIL_RES g3Dutil_resTbl_mero[] = {
	{ ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD �{�[���A�E�g
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_in_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD �{�[���C��

  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbca, GFL_G3D_UTIL_RESARC }, //ICA �{�[���A�E�g�A�j��
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbma, GFL_G3D_UTIL_RESARC }, //IMA
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_in_nsbca, GFL_G3D_UTIL_RESARC }, //ICA �{�[���C���A�j��
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_in_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_in_nsbma, GFL_G3D_UTIL_RESARC }, //IMA
};

//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl_mero[] = {
  //�o�������X�^�[�{�[��
  {
		RES_ID_BALL_OUT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BALL_OUT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_ball_out,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_ball_out),	//�A�j�����\�[�X��
	},
  //�i�[�����X�^�[�{�[��
  {
		RES_ID_BALL_IN_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BALL_IN_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_ball_in,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_ball_in),	//�A�j�����\�[�X��
	},
};

static const GFL_G3D_UTIL_SETUP SetupMero = {
  g3Dutil_resTbl_mero,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl_mero),		//���\�[�X��
	g3Dutil_objTbl_mero,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl_mero),		//�I�u�W�F�N�g��
};


static GMEVENT_RESULT BallMoveEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT WaitPokeAppFrmEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT WaitBallAnmEvt( GMEVENT* event, int* seq, void* work );

//============================================================================================================
//�O�e�m�֘A
//============================================================================================================

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void SPPOKE_GMK_SetupTrio(FIELDMAP_WORK *fieldWork)
{
  int i;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldWork, SPPOKE_GMK_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(SPPOKE_GMK_WK));
  //�O�e�m�C�x���g�p���[�N�̃A���P�[�V����
  ;
  //�K�v�ȃ��\�[�X�̗p��
  FLD_EXP_OBJ_AddUnit(ptr, &SetupTrio, SPPOKE_TRIO_UNIT_IDX );
  
  //�{�[��������
  //�A�j���̏�Ԃ�������
  for (i=0;i<BALL_ANM_NUM;i++)
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    //1��Đ��ݒ�
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, SPPOKE_TRIO_UNIT_IDX, OBJ_BALL_OUT, i);
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    //�A�j����~
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    //������
    FLD_EXP_OBJ_ValidCntAnm(ptr, SPPOKE_TRIO_UNIT_IDX, OBJ_BALL_OUT, i, FALSE);

    //1��Đ��ݒ�
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, SPPOKE_TRIO_UNIT_IDX, OBJ_BALL_IN, i);
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    //�A�j����~
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    //������
    FLD_EXP_OBJ_ValidCntAnm(ptr, SPPOKE_TRIO_UNIT_IDX, OBJ_BALL_IN, i, FALSE);
  }
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	    fieldWork   �t�B�[���h���[�N�|�C���^
 * @return    none
 */
//--------------------------------------------------------------
void SPPOKE_GMK_EndTrio(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�O�e�m�C�x���g�p���[�N���
  
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, SPPOKE_TRIO_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void SPPOKE_GMK_MoveTrio(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );
}

//============================================================================================================
//�����f�B�A�֘A
//============================================================================================================

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void SPPOKE_GMK_SetupMerodhia(FIELDMAP_WORK *fieldWork)
{
  int i;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldWork, SPPOKE_GMK_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(SPPOKE_GMK_WK));
  //�����f�B�A�C�x���g�p���[�N�̃A���P�[�V����
  ;
  //�K�v�ȃ��\�[�X�̗p��
  FLD_EXP_OBJ_AddUnit(ptr, &SetupTrio, SPPOKE_MERO_UNIT_IDX );
  
  //�{�[��������
  //�A�j���̏�Ԃ�������
  for (i=0;i<BALL_ANM_NUM;i++)
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    //1��Đ��ݒ�
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, SPPOKE_MERO_UNIT_IDX, OBJ_BALL_OUT, i);
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    //�A�j����~
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    //������
    FLD_EXP_OBJ_ValidCntAnm(ptr, SPPOKE_MERO_UNIT_IDX, OBJ_BALL_OUT, i, FALSE);

    //1��Đ��ݒ�
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, SPPOKE_MERO_UNIT_IDX, OBJ_BALL_IN, i);
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    //�A�j����~
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    //������
    FLD_EXP_OBJ_ValidCntAnm(ptr, SPPOKE_MERO_UNIT_IDX, OBJ_BALL_IN, i, FALSE);
  }
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	    fieldWork   �t�B�[���h���[�N�|�C���^
 * @return    none
 */
//--------------------------------------------------------------
void SPPOKE_GMK_EndMerodhia(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�����f�B�A�C�x���g�p���[�N���
  
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);
  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, SPPOKE_MERO_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void SPPOKE_GMK_MoveMerodhia(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );
}

//============================================================================================================
//�{�[���֘A�@�@���ʕ���
//============================================================================================================

//--------------------------------------------------------------
/**
 * �{�[���A�j���C�x���g�쐬
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
GMEVENT *SPPOKE_GMK_MoveBall( GAMESYS_WORK *gsys, const BALL_ANM_TYPE inType, const VecFx32 *inStart, const VecFx32 *inEnd,
                              const fx32 inHeight, const u32 inSync)
{
  int i;
  int obj;
  GMEVENT *event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  SPPOKE_GMK_WK *gmk_wk = GMK_TMP_WK_GetWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  if ( inType == BALL_ANM_TYPE_OUT )
  {
    obj = OBJ_BALL_OUT;
    //������SE�Đ�
    PMSND_PlaySE(SPPOKE_GMK_BALL_THROW);
  }
  else obj = OBJ_BALL_IN;

  //�X�^�[�g���W�ɕ\����ԂŃ{�[���z�u
  {
     GFL_G3D_OBJSTATUS *status;
     status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, SPPOKE_UNIT_IDX, obj);
     status->trans = *inStart;
  }

  //�{�[����\����Ԃɂ���
  FLD_EXP_OBJ_SetVanish(ptr, SPPOKE_UNIT_IDX, obj, FALSE);
  //�A�j���̏�Ԃ�������
  for (i=0;i<BALL_ANM_NUM;i++)
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, SPPOKE_UNIT_IDX, obj, i);
    //�A�j����~
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    //������
    FLD_EXP_OBJ_ValidCntAnm(ptr, SPPOKE_UNIT_IDX, obj, i, FALSE);
    //������
    FLD_EXP_OBJ_SetObjAnmFrm(ptr, SPPOKE_UNIT_IDX, obj, i, 0 );
  }

  //�ړ��ɕK�v�ȃp�����[�^�̃Z�b�g
  {
    gmk_wk->BallStart = *inStart;
    gmk_wk->BallEnd = *inEnd;
    gmk_wk->Sync = inSync;
    gmk_wk->NowSync = 0;
    gmk_wk->Height = inHeight;
  }
  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, BallMoveEvt, 0 );
  //�A�j���^�C�v���i�[
  gmk_wk->AnmType = inType;

  return event;
}

//--------------------------------------------------------------
/**
 * �{�[���A�j���C�x���g�쐬
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void SPPOKE_GMK_StartBallAnm( GAMESYS_WORK *gsys, const BALL_ANM_TYPE inType, const VecFx32 *inPos )
{
  int i;
  int obj;
  int se;
  GMEVENT *event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  SPPOKE_GMK_WK *gmk_wk = GMK_TMP_WK_GetWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  if ( inType == BALL_ANM_TYPE_OUT )
  {
    obj = OBJ_BALL_OUT;
    se = SPPOKE_GMK_BALL_OUT;
  }
  else
  {
    obj = OBJ_BALL_IN;
    se = SPPOKE_GMK_BALL_IN;
  }
  //SE�Đ�
  PMSND_PlaySE(se);

  //�{�[�����W���Z�b�g
  {
     GFL_G3D_OBJSTATUS   *status;
     status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, SPPOKE_UNIT_IDX, obj);
     status->trans = *inPos;
  }

  //�{�[����\����Ԃɂ���
  FLD_EXP_OBJ_SetVanish(ptr, SPPOKE_UNIT_IDX, obj, FALSE);
  //�A�j���̏�Ԃ�������
  for (i=0;i<BALL_ANM_NUM;i++)
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, SPPOKE_UNIT_IDX, obj, i);
    //�A�j����~����
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
    //����������
    FLD_EXP_OBJ_ValidCntAnm(ptr, SPPOKE_UNIT_IDX, obj, i, TRUE);
    //������
    FLD_EXP_OBJ_SetObjAnmFrm(ptr, SPPOKE_UNIT_IDX, obj, i, 0 );
  }
}

//--------------------------------------------------------------
/**
 * �|�P������\���E��\�����Ă����^�C�~���O��TRUE��Ԃ��C�x���g
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
GMEVENT *SPPOKE_GMK_WaitPokeAppear( GAMESYS_WORK *gsys, const BALL_ANM_TYPE inType )
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  SPPOKE_GMK_WK *gmk_wk = GMK_TMP_WK_GetWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);
  //�C�x���g�쐬
  GMEVENT * event = GMEVENT_Create( gsys, NULL, WaitPokeAppFrmEvt, 0 );
  //�A�j���^�C�v���i�[
  gmk_wk->AnmType = inType;

  return event;
}

//--------------------------------------------------------------
/**
 * �{�[���A�j���I���܂ő҂C�x���g�쐬
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
GMEVENT *SPPOKE_GMK_WaitBallAnmEnd( GAMESYS_WORK *gsys, const BALL_ANM_TYPE inType )
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  SPPOKE_GMK_WK *gmk_wk = GMK_TMP_WK_GetWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);
  //�C�x���g�쐬
  GMEVENT * event = GMEVENT_Create( gsys, NULL, WaitBallAnmEvt, 0 );
  //�A�j���^�C�v���i�[
  gmk_wk->AnmType = inType;
  return event;
}

//--------------------------------------------------------------
/**
 * �{�[���̓����A�߂�ɂ��ړ��C�x���g
 * @param       event             �C�x���g�|�C���^
 * @param       seq               �V�[�P���T
 * @param       work              ���[�N�|�C���^
 * @return      GMEVENT_RESULT    �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT BallMoveEvt( GMEVENT* event, int* seq, void* work )
{
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  SPPOKE_GMK_WK *gmk_wk = GMK_TMP_WK_GetWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);
  int obj;
  if ( gmk_wk->AnmType == BALL_ANM_TYPE_OUT ) obj = OBJ_BALL_OUT;
  else obj = OBJ_BALL_IN;

  //�ړI�n�Ɍ������Ĕ��
  {
    VecFx32 vec;
    fx32 diff;
    gmk_wk->NowSync++;
    diff = gmk_wk->BallEnd.x - gmk_wk->BallStart.x;
    vec.x = gmk_wk->BallStart.x + ( (diff*gmk_wk->NowSync) / gmk_wk->Sync );
    diff = gmk_wk->BallEnd.y - gmk_wk->BallStart.y;
    vec.y = gmk_wk->BallStart.y + ( (diff*gmk_wk->NowSync) / gmk_wk->Sync );
    diff = gmk_wk->BallEnd.z - gmk_wk->BallStart.z;
    vec.z = gmk_wk->BallStart.z + ( (diff*gmk_wk->NowSync) / gmk_wk->Sync );

    {
      int h_sync = (gmk_wk->Sync+1)/2;
      int now_h_sync;
      fx32 h;
      if ( gmk_wk->NowSync < h_sync ){
        now_h_sync = gmk_wk->NowSync;
      }else{
        now_h_sync = (gmk_wk->Sync-gmk_wk->NowSync);
      }
      h = (gmk_wk->Height * now_h_sync) / gmk_wk->Sync;
      vec.y += h;
    }

    {
     GFL_G3D_OBJSTATUS *status;
     status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, SPPOKE_UNIT_IDX, obj);
     status->trans = vec;
    }
  }
  //�����������H
  if (gmk_wk->NowSync >= gmk_wk->Sync)
  {
    //�߂�̏ꍇ�̓{�[�����\���ɂ���
    if ( gmk_wk->AnmType == BALL_ANM_TYPE_IN )
    {
      FLD_EXP_OBJ_SetVanish(ptr, SPPOKE_UNIT_IDX, obj, TRUE);
    }
    //�C�x���g�I��
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �|�P������\�����Ă����^�C�~���O��T�I������C�x���g
 * @param       event             �C�x���g�|�C���^
 * @param       seq               �V�[�P���T
 * @param       work              ���[�N�|�C���^
 * @return      GMEVENT_RESULT    �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT WaitPokeAppFrmEvt( GMEVENT* event, int* seq, void* work )
{
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  SPPOKE_GMK_WK *gmk_wk = GMK_TMP_WK_GetWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);
  fx32 frm, dst_frm;
  int obj;
  if ( gmk_wk->AnmType == BALL_ANM_TYPE_OUT )
  {
    dst_frm = BALL_OUT_TIMMING * FX32_ONE;
    obj = OBJ_BALL_OUT;
  }
  else
  {
    dst_frm = BALL_IN_TIMMING * FX32_ONE;
    obj = OBJ_BALL_IN;
  }


  //�{�[���A�j���̌��݃t���[�����擾
  frm = FLD_EXP_OBJ_GetObjAnmFrm(ptr, SPPOKE_UNIT_IDX, obj, 0 );
  //�|�P�����o���Ă����t���[�����H
  if (frm >= dst_frm)
  {
    //�C�x���g�I��
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * �{�[���A�j���I���ŏI������C�x���g
 * @param       event             �C�x���g�|�C���^
 * @param       seq               �V�[�P���T
 * @param       work              ���[�N�|�C���^
 * @return      GMEVENT_RESULT    �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT WaitBallAnmEvt( GMEVENT* event, int* seq, void* work )
{
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  SPPOKE_GMK_WK *gmk_wk = GMK_TMP_WK_GetWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);
  EXP_OBJ_ANM_CNT_PTR anm;
  int obj;

  if (gmk_wk->AnmType == BALL_ANM_TYPE_OUT) obj = OBJ_BALL_OUT;
  else obj = OBJ_BALL_IN;

  //�O�ԖڂɃA�j���ŏI�����肷��
  anm = FLD_EXP_OBJ_GetAnmCnt( ptr, SPPOKE_UNIT_IDX, obj, 0);
  if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
  {
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}


