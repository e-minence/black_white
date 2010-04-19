//============================================================================================
/**
 * @file	field_gimmick.c
 * @brief	�}�b�v�ŗL�̎d�|���𔭓����邩�ǂ����𔻒肷��\�[�X
 * @author	saito
 * @date	2006.02.09
 *
 */
//============================================================================================

#include "include/savedata/gimmickwork.h"
#include "field_gimmick_def.h"
#include "field_gimmick.h"
#include "fieldmap.h"
#include "field_gimmick_h01.h"
#include "field_gimmick_h03.h"
#include "field_gimmick_gate.h"
#include "gym_elec.h"
#include "gym_normal.h"
#include "gym_anti.h"
#include "gym_fly.h"
#include "gym_insect.h"
#include "gym_ground.h"
#include "gym_ground_ent.h"
#include "gym_ice.h"
#include "gym_dragon.h"
#include "field_gimmick_d06.h"
#include "field_gimmick_league_front01.h"
#include "field_gimmick_league_front02.h"
#include "field_gimmick_bgate_jet.h"
#include "field_gimmick_bsubway.h"
#include "field_gimmick_r04d03.h"
#include "sp_poke_gimmick.h"
#include "palace_gimmick.h"
#include "palace_map_gimmick.h"
#include "field_gimmick_d12.h"
#include "legend_gmk.h"

extern void GYM_SetupTest(FIELDMAP_WORK *fieldWork);
extern void GYM_EndTest(FIELDMAP_WORK *fieldWork);
extern void GYM_MoveTest(FIELDMAP_WORK *fieldWork);

FS_EXTERN_OVERLAY(field_gym_insect);
FS_EXTERN_OVERLAY(field_gym_elec);
FS_EXTERN_OVERLAY(field_gym_normal);
FS_EXTERN_OVERLAY(field_gym_fly);
FS_EXTERN_OVERLAY(field_gym_anti);
FS_EXTERN_OVERLAY(field_gimmick_bgate);
FS_EXTERN_OVERLAY(field_gym_ground);
FS_EXTERN_OVERLAY(field_gym_ground_ent);
FS_EXTERN_OVERLAY(field_gym_ice);
FS_EXTERN_OVERLAY(field_gym_dragon);
FS_EXTERN_OVERLAY(field_gimmick_gate);
FS_EXTERN_OVERLAY(field_gimmick_lfront01);
FS_EXTERN_OVERLAY(field_gimmick_lfront02);
FS_EXTERN_OVERLAY(field_gimmick_h01);
FS_EXTERN_OVERLAY(field_gimmick_h03);
FS_EXTERN_OVERLAY(field_gimmick_r04d03);
FS_EXTERN_OVERLAY(sppoke_gimmick);
FS_EXTERN_OVERLAY(palace_gimmick);
FS_EXTERN_OVERLAY(palace_map_gimmick);
FS_EXTERN_OVERLAY(field_gimmick_d12);
FS_EXTERN_OVERLAY(field_gimmick_d06);
FS_EXTERN_OVERLAY(field_gimmick_bsubway);
FS_EXTERN_OVERLAY(legend_gimmick);

const static FSOverlayID FldGimmickOverlay[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
  NULL,
  FS_OVERLAY_ID(field_gimmick_h01),
  FS_OVERLAY_ID(field_gym_elec),
  FS_OVERLAY_ID(field_gym_normal),
  FS_OVERLAY_ID(field_gym_anti),
  FS_OVERLAY_ID(field_gym_fly),
  FS_OVERLAY_ID(field_gym_insect),
  FS_OVERLAY_ID(field_gym_ground),
  FS_OVERLAY_ID(field_gym_ground_ent),
  FS_OVERLAY_ID(field_gym_ice),                         //10
  FS_OVERLAY_ID(field_gym_dragon),
  FS_OVERLAY_ID(field_gimmick_h03),
  FS_OVERLAY_ID(field_gimmick_gate),  // C04R0601
  FS_OVERLAY_ID(field_gimmick_gate),  // C08R0601
  FS_OVERLAY_ID(field_gimmick_gate),  // R13R0201
  FS_OVERLAY_ID(field_gimmick_gate),  // R02R0101
  FS_OVERLAY_ID(field_gimmick_gate),  // C04R0701
  FS_OVERLAY_ID(field_gimmick_gate),  // C04R0801
  FS_OVERLAY_ID(field_gimmick_gate),  // C02R0701
  FS_OVERLAY_ID(field_gimmick_gate),  // R14R0101         //20
  FS_OVERLAY_ID(field_gimmick_gate),  // C08R0501
  FS_OVERLAY_ID(field_gimmick_gate),  // C08R0701
  FS_OVERLAY_ID(field_gimmick_gate),  // H01R0101
  FS_OVERLAY_ID(field_gimmick_gate),  // H01R0201
  FS_OVERLAY_ID(field_gimmick_gate),  // C03R0601
  FS_OVERLAY_ID(field_gimmick_d06),   // D06_GIMMICK_Setup,
  FS_OVERLAY_ID(field_gimmick_d06),   // D06_GIMMICK_Setup,
  FS_OVERLAY_ID(field_gimmick_d06),   // D06_GIMMICK_Setup,
  FS_OVERLAY_ID(field_gimmick_lfront01),//LEAGUE_FRONT_01_GIMMICK_Setup,
  FS_OVERLAY_ID(field_gimmick_lfront02),//LEAGUE_FRONT_02_GIMMICK_Setup,    //30
  FS_OVERLAY_ID(field_gimmick_gate),  // D03R0201
  FS_OVERLAY_ID(field_gimmick_bgate),
  FS_OVERLAY_ID(field_gimmick_bsubway),//C04R01002
  FS_OVERLAY_ID(field_gimmick_bsubway),//C04R01003
  FS_OVERLAY_ID(field_gimmick_bsubway),//C04R01004
  FS_OVERLAY_ID(field_gimmick_bsubway),//C04R01005
  FS_OVERLAY_ID(field_gimmick_bsubway),//C04R01006
  FS_OVERLAY_ID(field_gimmick_bsubway),//C04R01007
  FS_OVERLAY_ID(field_gimmick_bsubway),//C04R01008
  FS_OVERLAY_ID(field_gimmick_bsubway),//C04R01010              //40
  FS_OVERLAY_ID(field_gimmick_bsubway),//C04R01011
  FS_OVERLAY_ID(field_gimmick_gate),  // R01R0101
  FS_OVERLAY_ID(field_gimmick_gate),  // R15R0101
  FS_OVERLAY_ID(field_gimmick_gate),  // R11R0101
  FS_OVERLAY_ID(field_gimmick_gate),  // R12R0101
  FS_OVERLAY_ID(field_gimmick_r04d03),  // R04 and D03
  FS_OVERLAY_ID(sppoke_gimmick),  // R08R0101
  FS_OVERLAY_ID(sppoke_gimmick),  // C03R0401
  FS_OVERLAY_ID(sppoke_gimmick),  // C03R0901
  FS_OVERLAY_ID(palace_gimmick),  //                          //50
  FS_OVERLAY_ID(palace_map_gimmick),  //
  FS_OVERLAY_ID(field_gimmick_bsubway),//C04R0109
  FS_OVERLAY_ID(field_gimmick_d12), //D12
  FS_OVERLAY_ID(legend_gimmick), //N01R0502
};

const static FLD_GMK_SETUP_FUNC FldGimmickSetupFunc[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
  GYM_SetupTest,
  H01_GIMMICK_Setup,
  GYM_ELEC_Setup,
  GYM_NORMAL_Setup,
  GYM_ANTI_Setup,
  GYM_FLY_Setup,
  GYM_INSECT_Setup,
  GYM_GROUND_Setup,
  GYM_GROUND_ENT_Setup,
  GYM_ICE_Setup,
  GYM_DRAGON_Setup,
  H03_GIMMICK_Setup,
  GATE_GIMMICK_Setup,  // C04R0601
  GATE_GIMMICK_Setup,  // C08R0601
  GATE_GIMMICK_Setup,  // R13R0201
  GATE_GIMMICK_Setup,  // R02R0101
  GATE_GIMMICK_Setup,  // C04R0701
  GATE_GIMMICK_Setup,  // C04R0801
  GATE_GIMMICK_Setup,  // C02R0701
  GATE_GIMMICK_Setup,  // R14R0101
  GATE_GIMMICK_Setup,  // C08R0501
  GATE_GIMMICK_Setup,  // C08R0701
  GATE_GIMMICK_Setup,  // H01R0101
  GATE_GIMMICK_Setup,  // H01R0201
  GATE_GIMMICK_Setup,  // C03R0601
  D06_GIMMICK_Setup,
  D06_GIMMICK_Setup,
  D06_GIMMICK_Setup,
  LEAGUE_FRONT_01_GIMMICK_Setup,
  LEAGUE_FRONT_02_GIMMICK_Setup,
  GATE_GIMMICK_Setup,  // D03R0201
  FLD_GIMMICK_JetBadgeGate_Setup,  //R10R0701
  BSUBWAY_GIMMICK_Setup, //C04R0102
  BSUBWAY_GIMMICK_Setup, //C04R0103
  BSUBWAY_GIMMICK_Setup, //C04R0104
  BSUBWAY_GIMMICK_Setup, //C04R0105
  BSUBWAY_GIMMICK_Setup, //C04R0106
  BSUBWAY_GIMMICK_Setup, //C04R0107
  BSUBWAY_GIMMICK_Setup, //C04R0108
  BSUBWAY_GIMMICK_Setup, //C04R0110
  BSUBWAY_GIMMICK_Setup, //C04R0111
  GATE_GIMMICK_Setup,  // R01R0101
  GATE_GIMMICK_Setup,  // R15R0101
  GATE_GIMMICK_Setup,  // R11R0101
  GATE_GIMMICK_Setup,  // R12R0101
  R04D03_GIMMICK_Setup,    //R04 and D03
  SPPOKE_GMK_SetupTrio,    //R08R0101
  SPPOKE_GMK_SetupZoroa,    //C03R0401
  SPPOKE_GMK_SetupMerodhia,    //C03R0901
  PALACE_GMK_Setup,    //
  PALACE_MAP_GMK_Setup,    //�p���X�]�[���̃}�b�v
  BSUBWAY_GIMMICK_Setup, //C04R0109
  D12_GIMMICK_Setup,    //D12
  LEGEND_GMK_Setup,     //N01R0502
};

const static FLD_GMK_END_FUNC FldGimmickEndFunc[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
  GYM_EndTest,
  H01_GIMMICK_End,
  GYM_ELEC_End,
  GYM_NORMAL_End,
  GYM_ANTI_End,
  GYM_FLY_End,
  GYM_INSECT_End,
  GYM_GROUND_End,
  GYM_GROUND_ENT_End,
  GYM_ICE_End,
  GYM_DRAGON_End,
  H03_GIMMICK_End,
  GATE_GIMMICK_End,  // C04R0601
  GATE_GIMMICK_End,  // C08R0601
  GATE_GIMMICK_End,  // R13R0201
  GATE_GIMMICK_End,  // R02R0101
  GATE_GIMMICK_End,  // C04R0701
  GATE_GIMMICK_End,  // C04R0801
  GATE_GIMMICK_End,  // C02R0701
  GATE_GIMMICK_End,  // R14R0101
  GATE_GIMMICK_End,  // C08R0501
  GATE_GIMMICK_End,  // C08R0701
  GATE_GIMMICK_End,  // H01R0101
  GATE_GIMMICK_End,  // H01R0201
  GATE_GIMMICK_End,  // C03R0601
  D06_GIMMICK_End,
  D06_GIMMICK_End,
  D06_GIMMICK_End,
  LEAGUE_FRONT_01_GIMMICK_End,
  LEAGUE_FRONT_02_GIMMICK_End,
  GATE_GIMMICK_End,  // D03R0201
  FLD_GIMMICK_JetBadgeGate_End,  //R10R0701
  BSUBWAY_GIMMICK_End, //C04R0102
  BSUBWAY_GIMMICK_End, //C04R0103
  BSUBWAY_GIMMICK_End, //C04R0104
  BSUBWAY_GIMMICK_End, //C04R0105
  BSUBWAY_GIMMICK_End, //C04R0106
  BSUBWAY_GIMMICK_End, //C04R0107
  BSUBWAY_GIMMICK_End, //C04R0108
  BSUBWAY_GIMMICK_End, //C04R0110
  BSUBWAY_GIMMICK_End, //C04R0111
  GATE_GIMMICK_End,  // R01R0101
  GATE_GIMMICK_End,  // R15R0101
  GATE_GIMMICK_End,  // R11R0101
  GATE_GIMMICK_End,  // R12R0101
  R04D03_GIMMICK_End,    //R04 and D03
  SPPOKE_GMK_EndTrio,     //R08R0101
  SPPOKE_GMK_EndZoroa,    //C03R0401
  SPPOKE_GMK_EndMerodhia,    //C03R0901
  PALACE_GMK_End,    //
  PALACE_MAP_GMK_End,    //�p���X�]�[���̃}�b�v
  BSUBWAY_GIMMICK_End, //C04R0109
  D12_GIMMICK_End,    //D12
  LEGEND_GMK_End,     //N01R0502
};

const static FLD_GMK_MOVE_FUNC FldGimmickMoveFunc[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
  GYM_MoveTest,
  H01_GIMMICK_Move,
  GYM_ELEC_Move,
  GYM_NORMAL_Move,
  GYM_ANTI_Move,
  GYM_FLY_Move,
  GYM_INSECT_Move,
  GYM_GROUND_Move,
  GYM_GROUND_ENT_Move,
  GYM_ICE_Move,
  GYM_DRAGON_Move,
  H03_GIMMICK_Move,
  GATE_GIMMICK_Move,  // C04R0601 
  GATE_GIMMICK_Move,  // C08R0601 
  GATE_GIMMICK_Move,  // R13R0201 
  GATE_GIMMICK_Move,  // R02R0101 
  GATE_GIMMICK_Move,  // C04R0701 
  GATE_GIMMICK_Move,  // C04R0801 
  GATE_GIMMICK_Move,  // C02R0701 
  GATE_GIMMICK_Move,  // R14R0101 
  GATE_GIMMICK_Move,  // C08R0501 
  GATE_GIMMICK_Move,  // C08R0701 
  GATE_GIMMICK_Move,  // H01R0101 
  GATE_GIMMICK_Move,  // H01R0201 
  GATE_GIMMICK_Move,  // C03R0601 
  D06_GIMMICK_Move,
  D06_GIMMICK_Move,
  D06_GIMMICK_Move,
  LEAGUE_FRONT_01_GIMMICK_Move,
  LEAGUE_FRONT_02_GIMMICK_Move,
  GATE_GIMMICK_Move,  // D03R0201
  FLD_GIMMICK_JetBadgeGate_Move,  //R10R0701
  BSUBWAY_GIMMICK_Move, //C04R0102
  BSUBWAY_GIMMICK_Move, //C04R0103
  BSUBWAY_GIMMICK_Move, //C04R0104
  BSUBWAY_GIMMICK_Move, //C04R0105
  BSUBWAY_GIMMICK_Move, //C04R0106
  BSUBWAY_GIMMICK_Move, //C04R0107
  BSUBWAY_GIMMICK_Move, //C04R0108
  BSUBWAY_GIMMICK_Move, //C04R0110
  BSUBWAY_GIMMICK_Move, //C04R0111
  GATE_GIMMICK_Move,  // R01R0101
  GATE_GIMMICK_Move,  // R15R0101
  GATE_GIMMICK_Move,  // R11R0101
  GATE_GIMMICK_Move,  // R12R0101
  R04D03_GIMMICK_Move,    //R04 and D03
  SPPOKE_GMK_MoveTrio,    //R08R0101
  SPPOKE_GMK_MoveZoroa,    //C03R0401
  SPPOKE_GMK_MoveMerodhia,    //C03R0901
  PALACE_GMK_Move,    //
  PALACE_MAP_GMK_Move,    //�p���X�]�[���̃}�b�v
  BSUBWAY_GIMMICK_Move, //C04R0109
  D12_GIMMICK_Move,    //D12
  LEGEND_GMK_Move,     //N01R0502
};

//---------------------------------------------------------------------------
/**
 * @brief	�M�~�b�N�̃Z�b�g�A�b�v�֐�
 * 
 * @param	fieldWork	�t�B�[���h���[�N�|�C���^
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void FLDGMK_SetUpFieldGimmick(FIELDMAP_WORK *fieldWork)
{
	int id;
  GIMMICKWORK *work;

  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
	
	//�M�~�b�N���[�N�擾
	work = GAMEDATA_GetGimmickWork(gamedata);
	//�M�~�b�N�R�[�h���擾
	id = GIMMICKWORK_GetAssignID(work);
	
	if (id == GIMMICK_NO_ASSIGN){
		return;					//�M�~�b�N����
	}

  //�I�[�o�[���C���[�h
  if ( FldGimmickOverlay[id] != NULL )
  {
    GFL_OVERLAY_Load( FldGimmickOverlay[id] );
  }
	//�M�~�b�N�Z�b�g�A�b�v
	FldGimmickSetupFunc[id](fieldWork);
}

//---------------------------------------------------------------------------
/**
 * @brief	�M�~�b�N�̏I���֐�
 * 
 * @param	fieldWork	�t�B�[���h���[�N�|�C���^
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void FLDGMK_EndFieldGimmick(FIELDMAP_WORK *fieldWork)
{
	int id;
	GIMMICKWORK *work;

  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
	
  //�M�~�b�N���[�N�擾
	work = GAMEDATA_GetGimmickWork(gamedata);;
	//�M�~�b�N�R�[�h���擾
	id = GIMMICKWORK_GetAssignID(work);
	
	if (id == GIMMICK_NO_ASSIGN){
		return;					//�M�~�b�N����
	}

	//�M�~�b�N�I��
	if (FldGimmickEndFunc[id] != NULL){
		FldGimmickEndFunc[id](fieldWork);
	}
  //�I�[�o�[���C�A�����[�h
  if ( FldGimmickOverlay[id] != NULL )
  {
    GFL_OVERLAY_Unload( FldGimmickOverlay[id] );
  }
}

//---------------------------------------------------------------------------
/**
 * @brief	�M�~�b�N�̓���֐�
 * 
 * @param	fieldWork	�t�B�[���h���[�N�|�C���^
 * 
 * @return	none
 */
//---------------------------------------------------------------------------
void FLDGMK_MoveFieldGimmick(FIELDMAP_WORK *fieldWork)
{
  int id;
  GIMMICKWORK *work;

  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
	
	//�M�~�b�N���[�N�擾
	work = GAMEDATA_GetGimmickWork(gamedata);
	//�M�~�b�N�R�[�h���擾
	id = GIMMICKWORK_GetAssignID(work);
	
	if (id == GIMMICK_NO_ASSIGN){
		return;					//�M�~�b�N����
	}

  if (FldGimmickMoveFunc[id] != NULL){
    //�M�~�b�N�Z�b�g�A�b�v
	  FldGimmickMoveFunc[id](fieldWork);
  }
}


#if 0
//---------------------------------------------------------------------------
/**
 * @brief	�M�~�b�N�p���ꓖ���蔻�菈��
 * 
 * @param	fsys	�t�B�[���h�V�X�e���|�C���^
 * @param	inGridX		�O���b�h�w���W
 * @param	inGirdZ		�O���b�h�y���W
 * @param	inHeight	���ݍ���
 * @param	outHit		TRUE:�ʏ퓖���蔻����s�Ȃ�Ȃ�	FALSE�F�ʏ퓖���蔻����s��
 * 
 * @return	BOOL		TRUE:HIT		FALSE:NOHIT
 */
//---------------------------------------------------------------------------
BOOL FLDGMK_FieldGimmickHitCheck(	FIELDSYS_WORK *fsys,
									const int inGridX, const int inGridZ,
									const fx32 inHeight,
									BOOL *outHit)
{
	int id;
	GIMMICKWORK *work;
	//�M�~�b�N���[�N�擾
	work = SaveData_GetGimmickWork(GameSystem_GetSaveData(fsys));
	//�M�~�b�N�R�[�h���擾
	id = GIMMICKWORK_GetAssignID(work);
	
	if (id == GIMMICK_NO_ASSIGN){
		return FALSE;					//�M�~�b�N����
	}
	//�M�~�b�N�I��
	if (FldGimmickHitCheck[id] != NULL){
		BOOL rc;
		rc = FldGimmickHitCheck[id](fsys, inGridX, inGridZ, inHeight, outHit);
		return rc;
	}
	
	return FALSE;						//��������
}
#endif

//--------------------------------------------------------------
/**
 * @brief   �M�~�b�N�R�[�h���w��ID�̃R�[�h�Ɠ��ꂩ�`�F�b�N
 *
 * @param	fieldWork	�t�B�[���h���[�N�|�C���^
 * @param   gimmick_id		�M�~�b�N�R�[�h
 *
 * @retval  TRUE:��v�B�@FALSE:�s��v
 *
 */
//--------------------------------------------------------------
BOOL FLDGMK_GimmickCodeCheck(FIELDMAP_WORK *fieldWork, int gimmick_id)
{
  int id;
  GIMMICKWORK *work;

  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
	
	//�M�~�b�N���[�N�擾
	work = GAMEDATA_GetGimmickWork(gamedata);
	//�M�~�b�N�R�[�h���擾
	id = GIMMICKWORK_GetAssignID(work);

	if(gimmick_id == id){
		return TRUE;
	}
	return FALSE;
}
