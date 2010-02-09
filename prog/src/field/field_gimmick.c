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
FS_EXTERN_OVERLAY(field_gimmick_h01);
FS_EXTERN_OVERLAY(field_gimmick_h03);

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
  FS_OVERLAY_ID(field_gym_ice),
  FS_OVERLAY_ID(field_gym_dragon),
  FS_OVERLAY_ID(field_gimmick_h03),
  FS_OVERLAY_ID(field_gimmick_gate),  // C04R0601
  FS_OVERLAY_ID(field_gimmick_gate),  // C08R0601
  FS_OVERLAY_ID(field_gimmick_gate),  // R13R0201
  FS_OVERLAY_ID(field_gimmick_gate),  // R02R0101
  FS_OVERLAY_ID(field_gimmick_gate),  // C04R0701
  FS_OVERLAY_ID(field_gimmick_gate),  // C04R0801
  FS_OVERLAY_ID(field_gimmick_gate),  // C02R0701
  FS_OVERLAY_ID(field_gimmick_gate),  // R14R0101
  FS_OVERLAY_ID(field_gimmick_gate),  // C08R0501
  FS_OVERLAY_ID(field_gimmick_gate),  // C08R0701
  FS_OVERLAY_ID(field_gimmick_gate),  // H01R0101
  FS_OVERLAY_ID(field_gimmick_gate),  // H01R0201
  FS_OVERLAY_ID(field_gimmick_gate),  // C03R0601
  NULL,//D06_GIMMICK_Setup,
  NULL,//D06_GIMMICK_Setup,
  NULL,//D06_GIMMICK_Setup,
  NULL,//LEAGUE_FRONT_01_GIMMICK_Setup,
  NULL,//LEAGUE_FRONT_02_GIMMICK_Setup,
  FS_OVERLAY_ID(field_gimmick_gate),  // D03R0201
  FS_OVERLAY_ID(field_gimmick_bgate),
  NULL,//C04R01002
  NULL,//C04R01003
  NULL,//C04R01004
  NULL,//C04R01005
  NULL,//C04R01006
  NULL,//C04R01007
  NULL,//C04R01008
  NULL,//C04R01010
  NULL,//C04R01011
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
};

#if 0
const static FLD_GMK_HIT_CHECK FldGimmickHitCheck[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,

};
#endif

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
