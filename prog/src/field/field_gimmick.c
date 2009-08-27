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

extern void GYM_SetupTest(FIELDMAP_WORK *fieldWork);
extern void GYM_EndTest(FIELDMAP_WORK *fieldWork);
extern void GYM_MoveTest(FIELDMAP_WORK *fieldWork);

const static FLD_GMK_SETUP_FUNC FldGimmickSetupFunc[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
  GYM_SetupTest,
/* 
	GYM_SetupGhostGym,		//1:�S�[�X�g�W��
	GYM_SetupCombatGym,		//2:�i���W��
	GYM_SetupElecGym,		//3:�d�C�W��		�J���g�[
	GYM_SetupSkyGym,		//4:��s�W��
	GYM_SetupInsectGym,		//5:���W��
	GYM_SetupDragonGym,		//6:�h���S���W��
	GYM_SetupPoisonGym,		//7:�ŃW��			�J���g�[
	GYM_SetupAlmGym,		//8:���\�W��		�J���g�[
	ArceusGmk_Setup,		//9:�V���g��ՃM�~�b�N
*/
};

const static FLD_GMK_END_FUNC FldGimmickEndFunc[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
  GYM_EndTest,
/*
	GYM_EndGhostGym,		//1:�S�[�X�g�W��
	GYM_EndCombatGym,		//2:�i���W��
	GYM_EndElecGym,			//3:�d�C�W��		�J���g�[
	NULL,					//4:��s�W��
	GYM_EndInsectGym,		//5:���W��
	GYM_EndDragonGym,		//6:�h���S���W��
	GYM_EndPoisonGym,		//7:�ŃW��			�J���g�[
	GYM_EndAlmGym,			//8:���\�W��		�J���g�[
	ArceusGmk_End,			//9:�V���g��ՃM�~�b�N
*/
};

const static FLD_GMK_END_FUNC FldGimmickMoveFunc[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
  GYM_MoveTest,
/*
	GYM_EndGhostGym,		//1:�S�[�X�g�W��
	GYM_EndCombatGym,		//2:�i���W��
	GYM_EndElecGym,			//3:�d�C�W��		�J���g�[
	NULL,					//4:��s�W��
	GYM_EndInsectGym,		//5:���W��
	GYM_EndDragonGym,		//6:�h���S���W��
	GYM_EndPoisonGym,		//7:�ŃW��			�J���g�[
	GYM_EndAlmGym,			//8:���\�W��		�J���g�[
	ArceusGmk_End,			//9:�V���g��ՃM�~�b�N
*/
};


const static FLD_GMK_HIT_CHECK FldGimmickHitCheck[FLD_GIMMICK_MAX] = {
	NULL,					//0:����
/*
	NULL,					//1:�S�[�X�g�W��
	NULL,					//2:�i���W��
	NULL,					//3:�d�C�W��		�J���g�[
	NULL,					//4:��s�W��
	NULL,					//5:���W��
	GYM_HitCheckDragonGym,	//6:�h���S���W��
	NULL,					//7:�ŃW��			�J���g�[
	NULL,					//8:���\�W��		�J���g�[
	NULL,					//9:�V���g��ՃM�~�b�N
*/
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
