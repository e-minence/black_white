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

//--------------------------------------------------------------
//  ����Z�[�u�Z�b�g�A�b�v�Ǘ��@
//      ��check ���L�̊֐���save_control.c, save_control_intr.c�ł����g�p���Ȃ��͂��B
//              �}�X�^�[�A�b�v�O�ɂ����̉ӏ��ňӐ}�����g�p����Ă��Ȃ����m�F����
//--------------------------------------------------------------
extern BOOL SaveData_GetNowSaveModeSetup(SAVE_CONTROL_WORK *sv);
extern void SaveData_SetNowSaveModeSetupON(SAVE_CONTROL_WORK *sv);
extern void SaveData_SetNowSaveModeSetupOFF(SAVE_CONTROL_WORK *sv);


#endif	//__PLAYER_DATA_H__
