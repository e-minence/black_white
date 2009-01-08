//==============================================================================
/**
 * @file	player_data.h
 * @brief	�v���C���[�֘A�Z�[�u�f�[�^�̃w�b�_
 * @author	matsuda
 * @date	2009.01.08(��)
 */
//==============================================================================
#ifndef __PLAYER_DATA_H__
#define __PLAYER_DATA_H__

#include "savedata/save_control.h"	//SAVE_CONTROL_WORK�Q�Ƃ̂���
#include "gamesystem/playerwork.h"


//==============================================================================
//	�O���֐��錾
//==============================================================================
extern void SaveData_PlayerDataUpdate(SAVE_CONTROL_WORK * sv, const PLAYER_WORK *pw);
extern void SaveData_PlayerDataLoad(SAVE_CONTROL_WORK * sv, PLAYER_WORK *pw);


#endif	//__PLAYER_DATA_H__
