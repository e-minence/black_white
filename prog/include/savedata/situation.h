//==============================================================================
/**
 * @file	situation.h
 * @brief	�󋵃f�[�^�A�N�Z�X�̃w�b�_
 * @author	matsuda
 * @date	2009.01.08(��)
 */
//==============================================================================
#ifndef __SITUATION_H__
#define __SITUATION_H__

#include "savedata/save_control.h"	//SAVE_CONTROL_WORK�Q�Ƃ̂���
#include "gamesystem/playerwork.h"
#include "field/location.h"


//==============================================================================
//	�^��`
//==============================================================================
///�󋵃f�[�^�̕s��`�A�N�Z�X�^
typedef struct _SITUATION SITUATION;

//==============================================================================
//	�\���̒�`
//==============================================================================
///PLAYER_WORK�\���̂���Z�[�u�ɕK�v�Ȃ��̂������܂Ƃ߂��\����
typedef struct{
	ZONEID zoneID;
	VecFx32 position;
  RAIL_LOCATION railposition;
	s16 direction;
  u8 pos_type;       ///<LOCATION_POS_TYPE
  u8 pad;
}PLAYERWORK_SAVE;


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern int Situation_GetWorkSize(void);
extern void Situation_Init(SITUATION * st);
extern LOCATION * Situation_GetStartLocation(SITUATION * st);
extern LOCATION * Situation_GetEntranceLocation(SITUATION * st);
extern LOCATION * Situation_GetSpecialLocation(SITUATION * st);
extern LOCATION * Situation_GetEscapeLocation(SITUATION * st);
extern u16 * Situation_GetWarpID( SITUATION * st );
extern SITUATION * SaveData_GetSituation(SAVE_CONTROL_WORK * sv);
extern void SaveData_SituationDataUpdate(SAVE_CONTROL_WORK *sv, const PLAYER_WORK *pw);
extern void SaveData_SituationDataLoad(SAVE_CONTROL_WORK *sv, PLAYER_WORK *pw);
extern void SaveData_SituationLoad_PlayerWorkSave(SAVE_CONTROL_WORK *sv, PLAYERWORK_SAVE *plsv);


#endif	//__SITUATION_H__
