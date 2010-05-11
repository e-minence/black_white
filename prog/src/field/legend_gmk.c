//======================================================================
/**
 * @file  legend_gmk.h
 * @brief  �`���|�P�����M�~�b�N
 *
 * @note    �z�z�n�Ŏg�p���Ă��郂���X�^�[�{�[���A�j���[�V�����������ł̓��\�[�X�̂݋��L���ăv���O�������쐬���Ă��܂�
 * @note    �{���͓����������������̂ł����A������͊J���I�Ղ܂Œ���������\�������邽�߁A�e�h�w���Ă���z�z�n�{�[���A�j���[�V�����Ɛ؂藣���܂�
 * @note    ���ʁA��������������R�[�h�����܂��\��������܂����A�؂藣�����ƂŁA�z�z�n�{�[���A�j���[�V�����̓����ۏ؂��邱�Ƃ�D�悵�܂�
 * @author  Saito
 */
//======================================================================

#include "fieldmap.h"
#include "legend_gmk.h"
#include "field_gimmick_def.h"
#include "savedata/gimmickwork.h"
#include "gmk_tmp_wk.h"

#include "field/field_const.h"  //for FIELD_CONST_GRID_FX32_SIZE

#include "arc/fieldmap/legend_gmk.naix"
#include "arc/fieldmap/sppoke.naix"
#include "sound/pm_sndsys.h"
#include "legend_gmk_se_def.h"
#include "sp_poke_gimmick_se_def.h"

#include "../../../resource/fldmapdata/gimmick/legend/leg_gmk_timing_def.h"

#define LEGEND_GMK_ASSIGN_ID    (1)
#define LEGEND_UNIT_IDX    (0)                      //���j�b�g�C���f�b�N�X
#define STONE_ANM_NUM  (4)

#define FLASH_FRAME_FX32  ( FLASH_FRAME*FX32_ONE )

#define FADE_SPEED    (0)

#define BALL_ANM_NUM  (3)
#define BALL_OUT_TIMMING  (5)

typedef struct LEG_GMK_WK_tag
{
  u32 Fade;
  int Count;
  BOOL Se0;
  BOOL Se1;
  BOOL Se2;
  VecFx32 BallStart;
  VecFx32 BallEnd;
  fx32 Height;
  u16 Sync;
  u16 NowSync;

}LEG_GMK_WK;


//==========================================================================
/**
 *  �֐��O���錾
*/
//==========================================================================
static GMEVENT_RESULT StoneEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT WaitBallAnmEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT WaitPokeAppFrmEvt( GMEVENT* event, int* seq, void* work );
static GMEVENT_RESULT BallMoveEvt( GMEVENT* event, int* seq, void* work );

//���\�[�X�̕��я���
enum {
  RES_ID_STONE_MDL = 0,

  RES_ID_STONE_ANM1,
  RES_ID_STONE_ANM2,
  RES_ID_STONE_ANM3,
  RES_ID_STONE_ANM4,

  RES_ID_BALL_OUT_MDL,
  RES_ID_BALL_OUT_ANM1,
  RES_ID_BALL_OUT_ANM2,
  RES_ID_BALL_OUT_ANM3,
};

//�n�a�i�C���f�b�N�X
enum {
  OBJ_STONE = 0,
  OBJ_BALL_OUT,
};

//==========================================================================
/**
 ���\�[�X����
*/
//==========================================================================

//3D�A�j���@��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl[] = {
  { RES_ID_STONE_ANM1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_STONE_ANM2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_STONE_ANM3,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_STONE_ANM4,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};

//3D�A�j���@�{�[���A�E�g
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl_ball_out[] = {
  { RES_ID_BALL_OUT_ANM1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_BALL_OUT_ANM2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_BALL_OUT_ANM3,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
};


//==========================================================================
/**
 ���\�[�X
*/
//==========================================================================

static const GFL_G3D_UTIL_RES g3Dutil_resTbl_black[] = {
	{ ARCID_LEGEND_GMK, NARC_legend_gmk_shin_mu_p_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD

  { ARCID_LEGEND_GMK, NARC_legend_gmk_shin_mu_p_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_LEGEND_GMK, NARC_legend_gmk_shin_mu_p_nsbma, GFL_G3D_UTIL_RESARC }, //IMA
  { ARCID_LEGEND_GMK, NARC_legend_gmk_shin_mu_p_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_LEGEND_GMK, NARC_legend_gmk_shin_p_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP

  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD �{�[���A�E�g
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbca, GFL_G3D_UTIL_RESARC }, //ICA �{�[���A�E�g�A�j��
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbma, GFL_G3D_UTIL_RESARC }, //IMA
};

static const GFL_G3D_UTIL_RES g3Dutil_resTbl_white[] = {
	{ ARCID_LEGEND_GMK, NARC_legend_gmk_shin_mu_p_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD

  { ARCID_LEGEND_GMK, NARC_legend_gmk_shin_mu_p_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_LEGEND_GMK, NARC_legend_gmk_shin_mu_p_nsbma, GFL_G3D_UTIL_RESARC }, //IMA
  { ARCID_LEGEND_GMK, NARC_legend_gmk_shin_mu_p_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_LEGEND_GMK, NARC_legend_gmk_mu_p_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP

  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD �{�[���A�E�g
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbca, GFL_G3D_UTIL_RESARC }, //ICA �{�[���A�E�g�A�j��
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
  { ARCID_SPPOKE_GMK, NARC_sppoke_mb_out_nsbma, GFL_G3D_UTIL_RESARC }, //IMA

};

//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
  //��
  {
		RES_ID_STONE_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_STONE_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl),	//�A�j�����\�[�X��
	},
  //�o�������X�^�[�{�[��
  {
		RES_ID_BALL_OUT_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_BALL_OUT_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl_ball_out,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl_ball_out),	//�A�j�����\�[�X��
	},
};

static const GFL_G3D_UTIL_SETUP SetupWhite = {
  g3Dutil_resTbl_white,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl_white),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};

static const GFL_G3D_UTIL_SETUP SetupBlack = {
  g3Dutil_resTbl_black,				//���\�[�X�e�[�u��
	NELEMS(g3Dutil_resTbl_black),		//���\�[�X��
	g3Dutil_objTbl,				//�I�u�W�F�N�g�ݒ�e�[�u��
	NELEMS(g3Dutil_objTbl),		//�I�u�W�F�N�g��
};

//--------------------------------------------------------------
/**
 * �Z�b�g�A�b�v�֐�
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void LEGEND_GMK_Setup(FIELDMAP_WORK *fieldWork)
{
  int i;
  LEG_GMK_WK *gmk_wk;
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�ėp���[�N�m��
  gmk_wk = GMK_TMP_WK_AllocWork
      (fieldWork, LEGEND_GMK_ASSIGN_ID, FIELDMAP_GetHeapID(fieldWork), sizeof(LEG_GMK_WK));
  gmk_wk->Count = 0;
  
  //�K�v�ȃ��\�[�X�̗p��(�o�[�W��������)
  if ( GetVersion() == VERSION_WHITE )
  {
    FLD_EXP_OBJ_AddUnit(ptr, &SetupWhite, LEGEND_UNIT_IDX );
  }
  else
  {
    FLD_EXP_OBJ_AddUnit(ptr, &SetupBlack, LEGEND_UNIT_IDX );
  }
  gmk_wk->Fade = GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN;
  
  //3�c�n�a�i������
  //�A�j���̏�Ԃ�������
  for (i=0;i<STONE_ANM_NUM;i++)
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, LEGEND_UNIT_IDX, OBJ_STONE, i);
    //1��Đ�
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    //�A�j����~
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    //������
    FLD_EXP_OBJ_ValidCntAnm(ptr, LEGEND_UNIT_IDX, OBJ_STONE, i, FALSE);
  }

  //�A�j���̏�Ԃ�������
  for (i=0;i<BALL_ANM_NUM;i++)
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    //1��Đ��ݒ�
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, LEGEND_UNIT_IDX, OBJ_BALL_OUT, i);
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 0);
    //�A�j����~
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    //������
    FLD_EXP_OBJ_ValidCntAnm(ptr, LEGEND_UNIT_IDX, OBJ_BALL_OUT, i, FALSE);
  }
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	    fieldWork   �t�B�[���h���[�N�|�C���^
 * @return    none
 */
//--------------------------------------------------------------
void LEGEND_GMK_End(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  //�ėp���[�N���
  GMK_TMP_WK_FreeWork(fieldWork, LEGEND_GMK_ASSIGN_ID);

  //�n�a�i���
  FLD_EXP_OBJ_DelUnit( ptr, LEGEND_UNIT_IDX );
}

//--------------------------------------------------------------
/**
 * ����֐�
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void LEGEND_GMK_Move(FIELDMAP_WORK *fieldWork)
{
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  //�A�j���[�V�����Đ�
  FLD_EXP_OBJ_PlayAnime( ptr );
}

//--------------------------------------------------------------
/**
 * �M�~�b�N�J�n�C�x���g�쐬
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
GMEVENT *LEGEND_GMK_Start( GAMESYS_WORK *gsys )
{
  int i;
  int obj;
  GMEVENT *event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  
  obj = OBJ_STONE;
  //��l���̈ʒu�ɕ\����Ԃłn�a�i�z�u
  {
    VecFx32 pos;
    GFL_G3D_OBJSTATUS *status;
    FIELD_PLAYER *fld_player;
    fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    FIELD_PLAYER_GetPos( fld_player, &pos );
    status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, LEGEND_UNIT_IDX, obj);
    status->trans = pos;
  }

  //OBJ��\����Ԃɂ���
  FLD_EXP_OBJ_SetVanish(ptr, LEGEND_UNIT_IDX, obj, FALSE);
  //�A�j���̏�Ԃ�������
  for (i=0;i<STONE_ANM_NUM;i++)
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, LEGEND_UNIT_IDX, obj, i);
    //�A�j���Đ�
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
    //����������
    FLD_EXP_OBJ_ValidCntAnm(ptr, LEGEND_UNIT_IDX, obj, i, TRUE);
    //������
    FLD_EXP_OBJ_SetObjAnmFrm(ptr, LEGEND_UNIT_IDX, obj, i, 0 );
  }

  //�C�x���g�쐬
  event = GMEVENT_Create( gsys, NULL, StoneEvt, 0 );

  return event;
}

//--------------------------------------------------------------
/**
 * �΃A�j���C�x���g
 * @param       event             �C�x���g�|�C���^
 * @param       seq               �V�[�P���T
 * @param       work              ���[�N�|�C���^
 * @return      GMEVENT_RESULT    �C�x���g����
 */
//--------------------------------------------------------------
static GMEVENT_RESULT StoneEvt( GMEVENT* event, int* seq, void* work )
{
  GAMESYS_WORK *gsys = GMEVENT_GetGameSysWork(event);
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
  int obj;
  fx32 frm, last_frm;
  LEG_GMK_WK *gmk_wk;

  gmk_wk = GMK_TMP_WK_GetWork(fieldWork, LEGEND_GMK_ASSIGN_ID);
  obj = OBJ_STONE;

  //���݃t���[�����擾
  frm = FLD_EXP_OBJ_GetObjAnmFrm(ptr, LEGEND_UNIT_IDX, obj, 0 );
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, LEGEND_UNIT_IDX, OBJ_STONE, 0);
    last_frm = FLD_EXP_OBJ_GetAnimeLastFrame(anm );
  }
  NOZOMU_Printf( "frm = %x::%d  last= %x::%d\n",frm, frm/FX32_ONE, last_frm, last_frm/FX32_ONE );

  if ( (frm >= LEG_SE0_FRM*FX32_ONE)&&(!gmk_wk->Se0) ){
    //����~
    PMSND_StopSE();
    //�r�d�Đ�
    PMSND_PlaySE( LEG_GMK_BALL_APP );
    gmk_wk->Se0 = TRUE;
  }
  else if ( (frm >= LEG_SE1_FRM*FX32_ONE)&&(!gmk_wk->Se1) ){
    //����~
    PMSND_StopSE();
    //�r�d�Đ��i���[�v���j
    PMSND_PlaySE( LEG_GMK_ENERGY );
    gmk_wk->Se1 = TRUE;
  }
  else if ( (frm >= LEG_SE2_FRM*FX32_ONE)&&(!gmk_wk->Se2) )
  {
    //����~
    PMSND_StopSE();
    //�r�d�Đ�
    PMSND_PlaySE( LEG_GMK_BALL );
    gmk_wk->Se2 = TRUE;
  }

  switch(*seq){
  case 0:
    if ( frm == FLASH_FRAME_FX32 )
    {
      //�t�F�[�h�A�E�g�J�n
      GFL_FADE_SetMasterBrightReq( gmk_wk->Fade, 0, 16, FADE_SPEED );
      gmk_wk->Count = FADE_WAIT;
      (*seq)++;
    }
    break;
  case 1:
    //�t�F�[�h�A�E�g�����҂�
    if ( GFL_FADE_CheckFade() == FALSE )
    {
      if ( gmk_wk->Count <=0 )
      {
        //�t�F�[�h�C���J�n
        GFL_FADE_SetMasterBrightReq( gmk_wk->Fade, 16, 0, FADE_SPEED );
        (*seq)++;
      }
      else gmk_wk->Count--;
    }
    break;
  case 2:
    //�t�F�[�h�C�������҂�
    if ( GFL_FADE_CheckFade() == FALSE )
    {
      (*seq)++;
    }
    break;
  default:
    ;
  }

  //�I�����Ă����t���[�����H
  if (frm >= last_frm)
  {
    NOZOMU_Printf("�A�j���I��\n");
    //�C�x���g�I��
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}

//============================================================================================================
//�{�[���֘A
//============================================================================================================

//--------------------------------------------------------------
/**
 * �{�[���A�j���C�x���g�쐬
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
GMEVENT *LEGEND_GMK_MoveBall( GAMESYS_WORK *gsys, const VecFx32 *inStart, const VecFx32 *inEnd,
                              const fx32 inHeight, const u32 inSync)
{
  int i;
  int obj;
  GMEVENT *event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  LEG_GMK_WK *gmk_wk = GMK_TMP_WK_GetWork(fieldWork, LEGEND_GMK_ASSIGN_ID);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  obj = OBJ_BALL_OUT;
  //������SE�Đ�
  PMSND_PlaySE(SPPOKE_GMK_BALL_THROW);

  //�X�^�[�g���W�ɕ\����ԂŃ{�[���z�u
  {
     GFL_G3D_OBJSTATUS *status;
     status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, LEGEND_UNIT_IDX, obj);
     status->trans = *inStart;
  }

  //�{�[����\����Ԃɂ���
  FLD_EXP_OBJ_SetVanish(ptr, LEGEND_UNIT_IDX, obj, FALSE);
  //�A�j���̏�Ԃ�������
  for (i=0;i<BALL_ANM_NUM;i++)
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, LEGEND_UNIT_IDX, obj, i);
    //�A�j����~
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    //������
    FLD_EXP_OBJ_ValidCntAnm(ptr, LEGEND_UNIT_IDX, obj, i, FALSE);
    //������
    FLD_EXP_OBJ_SetObjAnmFrm(ptr, LEGEND_UNIT_IDX, obj, i, 0 );
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

  return event;
}

//--------------------------------------------------------------
/**
 * �{�[���A�j���C�x���g�쐬
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
void LEGEND_GMK_StartBallAnm( GAMESYS_WORK *gsys, const VecFx32 *inPos )
{
  int i;
  int obj;
  int se;
  GMEVENT *event;
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );

  obj = OBJ_BALL_OUT;
  se = SPPOKE_GMK_BALL_OUT;
  //SE�Đ�
  PMSND_PlaySE(se);

  //�{�[�����W���Z�b�g
  {
     GFL_G3D_OBJSTATUS   *status;
     status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, LEGEND_UNIT_IDX, obj);
     status->trans = *inPos;
  }

  //�{�[����\����Ԃɂ���
  FLD_EXP_OBJ_SetVanish(ptr, LEGEND_UNIT_IDX, obj, FALSE);
  //�A�j���̏�Ԃ�������
  for (i=0;i<BALL_ANM_NUM;i++)
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, LEGEND_UNIT_IDX, obj, i);
    //�A�j����~����
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 0);
    //����������
    FLD_EXP_OBJ_ValidCntAnm(ptr, LEGEND_UNIT_IDX, obj, i, TRUE);
    //������
    FLD_EXP_OBJ_SetObjAnmFrm(ptr, LEGEND_UNIT_IDX, obj, i, 0 );
  }
}

//--------------------------------------------------------------
/**
 * �|�P������\���E��\�����Ă����^�C�~���O��TRUE��Ԃ��C�x���g
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
GMEVENT *LEGEND_GMK_WaitPokeAppear( GAMESYS_WORK *gsys )
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  //�C�x���g�쐬
  GMEVENT * event = GMEVENT_Create( gsys, NULL, WaitPokeAppFrmEvt, 0 );

  return event;
}

//--------------------------------------------------------------
/**
 * �{�[���A�j���I���܂ő҂C�x���g�쐬
 * @param	      fieldWork   �t�B�[���h���[�N�|�C���^
 * @return      none
 */
//--------------------------------------------------------------
GMEVENT *LEGEND_GMK_WaitBallAnmEnd( GAMESYS_WORK *gsys )
{
  FIELDMAP_WORK *fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  //�C�x���g�쐬
  GMEVENT * event = GMEVENT_Create( gsys, NULL, WaitBallAnmEvt, 0 );
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
  LEG_GMK_WK *gmk_wk = GMK_TMP_WK_GetWork(fieldWork, LEGEND_GMK_ASSIGN_ID);
  int obj;
  obj = OBJ_BALL_OUT;

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
     status = FLD_EXP_OBJ_GetUnitObjStatus(ptr, LEGEND_UNIT_IDX, obj);
     status->trans = vec;
    }
  }
  //�����������H
  if (gmk_wk->NowSync >= gmk_wk->Sync)
  {
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
  fx32 frm, dst_frm;
  int obj;
  {
    dst_frm = BALL_OUT_TIMMING * FX32_ONE;
    obj = OBJ_BALL_OUT;
  }
  //�{�[���A�j���̌��݃t���[�����擾
  frm = FLD_EXP_OBJ_GetObjAnmFrm(ptr, LEGEND_UNIT_IDX, obj, 0 );
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
  EXP_OBJ_ANM_CNT_PTR anm;
  int obj;

  obj = OBJ_BALL_OUT;

  //�O�ԖڂɃA�j���ŏI�����肷��
  anm = FLD_EXP_OBJ_GetAnmCnt( ptr, LEGEND_UNIT_IDX, obj, 0);
  if ( FLD_EXP_OBJ_ChkAnmEnd(anm) )
  {
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}
