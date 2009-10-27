//============================================================================================
/**
 * @file	situation.c
 * @brief	�󋵃f�[�^�A�N�Z�X
 * @author	tamada GAME FREAK inc.
 * @date	2005.12.03
 */
//============================================================================================
#include <gflib.h>
#include "savedata/save_tbl.h"
#include "savedata/situation.h"
#include "gamesystem/playerwork.h"


//============================================================================================
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	�󋵃f�[�^�^��`
 */
//----------------------------------------------------------
struct _SITUATION {
	LOCATION start_loc;     ///<
	LOCATION entrance_loc;  ///<���O�̏o����LOCATION
	LOCATION special_loc;   ///<����ڑ���LOCATION
  LOCATION escape_loc;    ///<�E�o��LOCATION
  u16 warp_id;            ///<���[�v��ѐ�w��ID

	//PLAYER_WORK����Z�[�u�ɕK�v�Ȃ��̂����𔲐�
	PLAYERWORK_SAVE plsv;
};


//============================================================================================
//
//	�Z�[�u�f�[�^�V�X�e�����ˑ�����֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	�󋵃��[�N�̃T�C�Y�擾
 * @return	int		�T�C�Y�i�o�C�g�P�ʁj
 */
//----------------------------------------------------------
int Situation_GetWorkSize(void)
{
	return sizeof(SITUATION);
}

//============================================================================================
//
//	�󋵃f�[�^����̂��߂̊֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	�󋵃f�[�^�̏�����
 * @param	st	�󋵃��[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
void Situation_Init(SITUATION * st)
{
	GFL_STD_MemClear(st, sizeof(SITUATION));
}

//----------------------------------------------------------
/**
 * @brief	���݂�LOCATION��Ԃ�
 * @param	st	�󋵃��[�N�ւ̃|�C���^
 * @return	LOCATION�ւ̃|�C���^
 */
//----------------------------------------------------------
LOCATION * Situation_GetStartLocation(SITUATION * st)
{
	return &st->start_loc;
}

//----------------------------------------------------------
/**
 * @brief	���O�ɓ������o����������LOCATION�ւ̃|�C���^
 * @param	st	�󋵃��[�N�ւ̃|�C���^
 * @return	LOCATION�ւ̃|�C���^
 */
//----------------------------------------------------------
LOCATION * Situation_GetEntranceLocation(SITUATION * st)
{
	return &st->entrance_loc;
}

//----------------------------------------------------------
/**
 * @brief	����ڑ����̎擾
 * @param	st	�󋵃��[�N�ւ̃|�C���^
 * @return	LOCATION	����ڑ����ւ̃|�C���^
 */
//----------------------------------------------------------
LOCATION * Situation_GetSpecialLocation(SITUATION * st)
{
	return &st->special_loc;
}

//----------------------------------------------------------
/**
 * @brief	�E�o�p�ڑ����̎擾
 * @param	st	�󋵃��[�N�ւ̃|�C���^
 * @return	LOCATION	�E�o�p�ڑ����ւ̃|�C���^
 */
//----------------------------------------------------------
LOCATION * Situation_GetEscapeLocation(SITUATION * st)
{
	return &st->special_loc;
}

//----------------------------------------------------------
/**
 * @brief ���[�v�ڑ���ID�̎擾
 * @param	st	�󋵃��[�N�ւ̃|�C���^
 * @return  u16 * ���[�vID�i�[�G���A�ւ̃|�C���^
 */
//----------------------------------------------------------
u16 * Situation_GetWarpID(SITUATION * st)
{
  return &st->warp_id;
}

//============================================================================================
//
//	�Z�[�u�f�[�^�擾�̂��߂̊֐�
//
//============================================================================================
//----------------------------------------------------------
/**
 * @brief	�󋵃f�[�^�ւ̃|�C���^�擾
 * @param	sv			�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @return	SITUATION	�󋵕ێ����[�N�ւ̃|�C���^
 */
//----------------------------------------------------------
SITUATION * SaveData_GetSituation(SAVE_CONTROL_WORK * sv)
{
	return SaveControl_DataPtrGet(sv, GMDATA_ID_SITUATION);
}


//==============================================================================
//	
//==============================================================================
//--------------------------------------------------------------
/**
 * @brief   PLAYER_WORK�\���̂̃f�[�^������SITUATION���X�V����
 *
 * @param   sv		�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @param   pw		PLAYER_DATA
 */
//--------------------------------------------------------------
void SaveData_SituationDataUpdate(SAVE_CONTROL_WORK *sv, const PLAYER_WORK *pw)
{
	SITUATION *st = SaveData_GetSituation(sv);
	
	st->plsv.zoneID = pw->zoneID;
	st->plsv.position = pw->position;
	st->plsv.railposition = pw->railposition;
	st->plsv.direction = pw->direction;
  st->plsv.pos_type = pw->pos_type;
}

//--------------------------------------------------------------
/**
 * @brief   �Z�[�u�f�[�^����PALYER_WORK���K�v�ȏ������[�h���Ă���
 *
 * @param   sv		�Z�[�u�f�[�^�ێ����[�N�ւ̃|�C���^
 * @param   pw		PLAYER_DATA
 */
//--------------------------------------------------------------
void SaveData_SituationDataLoad(SAVE_CONTROL_WORK *sv, PLAYER_WORK *pw)
{
	SITUATION *st = SaveData_GetSituation(sv);
	
	pw->zoneID = st->plsv.zoneID;
	pw->position = st->plsv.position;
	pw->railposition = st->plsv.railposition;
	pw->direction = st->plsv.direction;
	pw->pos_type = st->plsv.pos_type;
}

//--------------------------------------------------------------
/**
 * @brief   SITUATION�Z�[�u�f�[�^����PLAYERWORK_SAVE�̂݃��[�h����
 *
 * @param   sv			
 * @param   plsv		�����
 */
//--------------------------------------------------------------
void SaveData_SituationLoad_PlayerWorkSave(SAVE_CONTROL_WORK *sv, PLAYERWORK_SAVE *plsv)
{
	SITUATION *st = SaveData_GetSituation(sv);
	
	*plsv = st->plsv;
}
