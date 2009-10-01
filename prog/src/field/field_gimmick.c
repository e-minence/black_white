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
#include "gym_elec.h"
#include "gym_normal.h"
#include "gym_anti.h"

extern void GYM_SetupTest(FIELDMAP_WORK *fieldWork);
extern void GYM_EndTest(FIELDMAP_WORK *fieldWork);
extern void GYM_MoveTest(FIELDMAP_WORK *fieldWork);


const static FLD_GMK_SETUP_FUNC FldGimmickSetupFunc[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
  GYM_SetupTest,
  H01_GIMMICK_Setup,
  GYM_ELEC_Setup,
  GYM_NORMAL_SetupRoom1,
  GYM_NORMAL_SetupRoom2,
  GYM_ANTI_Setup,

};

const static FLD_GMK_END_FUNC FldGimmickEndFunc[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
  GYM_EndTest,
  H01_GIMMICK_End,
  GYM_ELEC_End,
  GYM_NORMAL_End,
  GYM_NORMAL_End,
  GYM_ANTI_End,

};

const static FLD_GMK_END_FUNC FldGimmickMoveFunc[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
  GYM_MoveTest,
  H01_GIMMICK_Move,
  GYM_ELEC_Move,
  GYM_NORMAL_Move,
  GYM_NORMAL_Move,
  GYM_ANTI_Move,
};


const static FLD_GMK_HIT_CHECK FldGimmickHitCheck[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,
  NULL,

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
  SAVE_CONTROL_WORK* sv= GAMEDATA_GetSaveControlWork(gamedata);
	
	//�M�~�b�N���[�N�擾
	work = SaveData_GetGimmickWork(sv);
	//�M�~�b�N�R�[�h���擾
	id = GIMMICKWORK_GetAssignID(work);
	
	if (id == FLD_GIMMICK_NONE){
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
  SAVE_CONTROL_WORK* sv= GAMEDATA_GetSaveControlWork(gamedata);
	
  //�M�~�b�N���[�N�擾
	work = SaveData_GetGimmickWork(sv);
	//�M�~�b�N�R�[�h���擾
	id = GIMMICKWORK_GetAssignID(work);
	
	if (id == FLD_GIMMICK_NONE){
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
  SAVE_CONTROL_WORK* sv= GAMEDATA_GetSaveControlWork(gamedata);
	
	//�M�~�b�N���[�N�擾
	work = SaveData_GetGimmickWork(sv);
	//�M�~�b�N�R�[�h���擾
	id = GIMMICKWORK_GetAssignID(work);
	
	if (id == FLD_GIMMICK_NONE){
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
	
	if (id == FLD_GIMMICK_NONE){
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
  SAVE_CONTROL_WORK* sv= GAMEDATA_GetSaveControlWork(gamedata);
	
	//�M�~�b�N���[�N�擾
	work = SaveData_GetGimmickWork(sv);
	//�M�~�b�N�R�[�h���擾
	id = GIMMICKWORK_GetAssignID(work);

	if(gimmick_id == id){
		return TRUE;
	}
	return FALSE;
}
