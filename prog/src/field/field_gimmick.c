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

extern void GYM_SetupTest(FIELDMAP_WORK *fieldWork);
extern void GYM_EndTest(FIELDMAP_WORK *fieldWork);
extern void GYM_MoveTest(FIELDMAP_WORK *fieldWork);


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
};

const static FLD_GMK_END_FUNC FldGimmickMoveFunc[FLD_GIMMICK_MAX] = {
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
