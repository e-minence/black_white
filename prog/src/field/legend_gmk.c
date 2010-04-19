//======================================================================
/**
 * @file  legend_gmk.h
 * @brief  �`���|�P�����M�~�b�N
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

#include "sound/pm_sndsys.h"
//#include "sp_poke_gimmick_se_def.h"

#include "../../../resource/fldmapdata/gimmick/legend/leg_gmk_timing_def.h"

#define LEGEND_GMK_ASSIGN_ID    (1)
#define LEGEND_UNIT_IDX    (0)                      //���j�b�g�C���f�b�N�X
#define STONE_ANM_NUM  (4)

#define FLASH_FRAME_FX32  ( FLASH_FRAME*FX32_ONE )
#define LOOP_START_FRAME_FX32 ( LOOP_START_FRAME*FX32_ONE )

#define FADE_SPEED    (0)

typedef struct LEG_GMK_WK_tag
{
  BOOL OneLoop;   //�A�j�����P��������
  u32 Fade;
  int Count;
}LEG_GMK_WK;

static GMEVENT_RESULT StoneEvt( GMEVENT* event, int* seq, void* work );

//���\�[�X�̕��я���
enum {
  RES_ID_STONE_MDL = 0,

  RES_ID_STONE_ANM1,
  RES_ID_STONE_ANM2,
  RES_ID_STONE_ANM3,
  RES_ID_STONE_ANM4,
};

//�n�a�i�C���f�b�N�X
enum {
  OBJ_STONE = 0,
};

//==========================================================================
/**
 ���\�[�X���ʕ���
*/
//==========================================================================

//3D�A�j���@��
static const GFL_G3D_UTIL_ANM g3Dutil_anmTbl[] = {
  { RES_ID_STONE_ANM1,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_STONE_ANM2,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_STONE_ANM3,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
  { RES_ID_STONE_ANM4,0 }, //�A�j�����\�[�XID, �A�j���f�[�^ID(���\�[�X����INDEX)
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
};

static const GFL_G3D_UTIL_RES g3Dutil_resTbl_white[] = {
	{ ARCID_LEGEND_GMK, NARC_legend_gmk_shin_mu_p_nsbmd, GFL_G3D_UTIL_RESARC }, //IMD

  { ARCID_LEGEND_GMK, NARC_legend_gmk_shin_mu_p_nsbca, GFL_G3D_UTIL_RESARC }, //ICA
  { ARCID_LEGEND_GMK, NARC_legend_gmk_shin_mu_p_nsbma, GFL_G3D_UTIL_RESARC }, //IMA
  { ARCID_LEGEND_GMK, NARC_legend_gmk_shin_mu_p_nsbta, GFL_G3D_UTIL_RESARC }, //ITA
  { ARCID_LEGEND_GMK, NARC_legend_gmk_mu_p_nsbtp, GFL_G3D_UTIL_RESARC }, //ITP
};

//3D�I�u�W�F�N�g�ݒ�e�[�u��
static const GFL_G3D_UTIL_OBJ g3Dutil_objTbl[] = {
  {
		RES_ID_STONE_MDL, 	//���f�����\�[�XID
		0, 							  //���f���f�[�^ID(���\�[�X����INDEX)
		RES_ID_STONE_MDL, 	//�e�N�X�`�����\�[�XID
		g3Dutil_anmTbl,			//�A�j���e�[�u��(�����w��̂���)
		NELEMS(g3Dutil_anmTbl),	//�A�j�����\�[�X��
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
  gmk_wk->OneLoop = FALSE;
  gmk_wk->Count = 0;
  
  //�K�v�ȃ��\�[�X�̗p��(�o�[�W��������)
  if ( GET_VERSION() == VERSION_WHITE )
  {
    FLD_EXP_OBJ_AddUnit(ptr, &SetupWhite, LEGEND_UNIT_IDX );
    gmk_wk->Fade = GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN;
  }
  else
  {
    FLD_EXP_OBJ_AddUnit(ptr, &SetupBlack, LEGEND_UNIT_IDX );
    gmk_wk->Fade = GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN;
  }
  
  //3�c�n�a�i������
  //�A�j���̏�Ԃ�������
  for (i=0;i<STONE_ANM_NUM;i++)
  {
    EXP_OBJ_ANM_CNT_PTR anm;
    anm = FLD_EXP_OBJ_GetAnmCnt( ptr, LEGEND_UNIT_IDX, OBJ_STONE, i);
    //���[�v�Đ�
    FLD_EXP_OBJ_ChgAnmLoopFlg(anm, 1);
    //�A�j����~
    FLD_EXP_OBJ_ChgAnmStopFlg(anm, 1);
    //������
    FLD_EXP_OBJ_ValidCntAnm(ptr, LEGEND_UNIT_IDX, OBJ_STONE, i, FALSE);
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

  //SE��~
  PMSND_StopSE();

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

  //���[�v����
  {
    LEG_GMK_WK *gmk_wk;
    gmk_wk = GMK_TMP_WK_GetWork(fieldWork, LEGEND_GMK_ASSIGN_ID);
    if (gmk_wk->OneLoop)
    {
      fx32 frm;
      FLD_EXP_OBJ_CNT_PTR ptr = FIELDMAP_GetExpObjCntPtr( fieldWork );
      int obj = OBJ_STONE;
      //���݃t���[�����擾
      frm = FLD_EXP_OBJ_GetObjAnmFrm(ptr, LEGEND_UNIT_IDX, obj, 0 );
      if ( frm == 0 )
      {
        int i;
        for (i=0;i<STONE_ANM_NUM;i++)
        {
          FLD_EXP_OBJ_SetObjAnmFrm(ptr, LEGEND_UNIT_IDX, obj, i, LOOP_START_FRAME_FX32 );
        }
      }
    }
  }
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
    LEG_GMK_WK *gmk_wk;
    NOZOMU_Printf("�A�j���I��\n");
    gmk_wk = GMK_TMP_WK_GetWork(fieldWork, LEGEND_GMK_ASSIGN_ID);
    gmk_wk->OneLoop = TRUE;
    //�C�x���g�I��
    return GMEVENT_RES_FINISH;
  }

  return GMEVENT_RES_CONTINUE;
}


