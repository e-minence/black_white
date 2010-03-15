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

//#include "../../../resource/fldmapdata/gimmick/gym_anti/gym_anti_local_def.h"


#define SCR_BALL_ANM_TYPE_OUT   (0)
#define SCR_BALL_ANM_TYPE_IN   (1)


#define SPPOKE_GMK_ASSIGN_ID    (1)
#define SPPOKE_TRIO_UNIT_IDX    (0)   //�O�e�m�C�x���g���j�b�g�C���f�b�N�X

typedef struct SPPOKE_GMK_WK_tag
{
  BALL_ANM_TYPE AnmType;    //�^�[�Q�b�g�ƂȂ�A�j���^�C�v���i�[����
  VecFx32 BallStart;
  VecFx32 BallEnd;
  fx32 Hieght;
  u16 Sync;
  u16 NowSync;

  void *Work;
}SPPOKE_GMK_WK;

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


static GMEVENT_RESULT BallMoveEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT WaitPokeAppFrmEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT WaitBallOutAnmEvt( GMEVENT* event, int* seq, void* work );

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void SPPOKE_GMK_Setup(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�ėp���[�N�m��
  GMK_TMP_WK_AllocWork
      (fieldWork, SPPOKE_GMK_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(SPPOKE_GMK_WK));
  //�O�e�m�C�x���g�p���[�N�̃A���P�[�V����
  ;
  //�K�v�ȃ��\�[�X�̗p��
//  FLD_EXP_OBJ_AddUnit(ptr, &SetupTrio, SPPOKE_TRIO_UNIT_IDX );
  
  //�{�[��������
  {
    //�n�߂͔�\��
  }
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	    fieldWork   �t�B�[���h���[�N�|�C���^
 * @return    none
 */
//--------------------------------------------------------------
void SPPOKE_GMK_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�O�e�m�C�x���g�p���[�N���
  
  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);
  //�n�a�i���
//  FLD_EXP_OBJ_DelUnit( ptr, SPPOKE_TRIO_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void SPPOKE_GMK_Move(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  //�A�j���[�V�����Đ�
//  FLD_EXP_OBJ_PlayAnime( ptr );
}

//--------------------------------------------------------------
/**
 * �{�[���A�j���C�x���g�쐬
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
GMEVENT *SPPOKE_GMK_StartBallAnm( GAMESYS_WORK *gsys, const BALL_ANM_TYPE inType, const VecFx32 *inStart, const VecFx32 *inEnd , const u32 inSync)
{
  GMEVENT *event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  SPPOKE_GMK_WK *gmk_wk = GMK_TMP_WK_GetWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�X�^�[�g���W�ɕ\����ԂŃ{�[���z�u
  ;
  //�A�j���̏�Ԃ�������
  ;
  //�ړ��ɕK�v�ȃp�����[�^�̃Z�b�g
  ;
  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, BallMoveEvt, 0 );
  //�A�j���^�C�v���i�[
  gmk_wk->AnmType = inType;

  return event;
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
  GMEVENT * event = GMEVENT_Create( gsys, NULL, WaitBallOutAnmEvt, 0 );
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
//  SPPOKE_GMK_WK *tmp = GMK_TMP_WK_GetWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);

  //�ړI�n�Ɍ������Ĕ��
  //
  //�����������H
  if (1)
  {
    //�^�C�v�����ăA�j���J�n
    if (gmk_wk->AnmType == BALL_ANM_TYPE_OUT);
    else ;
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
//  SPPOKE_GMK_WK *tmp = GMK_TMP_WK_GetWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);

  //�{�[���A�j���̌��݃t���[�����擾
  ;
  //�|�P�����o���Ă����t���[�����H
  if (1)
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
static GMEVENT_RESULT WaitBallOutAnmEvt( GMEVENT* event, int* seq, void* work )
{
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
//  SPPOKE_GMK_WK *tmp = GMK_TMP_WK_GetWork(fieldWork, SPPOKE_GMK_ASSIGN_ID);

  //�{�[���A�j���̌��݃t���[�����擾
  ;
  //�A�j���I�����H
  if (1)
  {
    //�C�x���g�I��
    return GMEVENT_RES_FINISH;
  }
  return GMEVENT_RES_CONTINUE;
}
