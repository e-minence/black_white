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
//      ���L�̊֐���save_control.c, save_control_intr.c�ł����g�p���Ȃ��͂��B
//--------------------------------------------------------------
extern BOOL SaveData_GetNowSaveModeSetup(SAVE_CONTROL_WORK *sv);
extern void SaveData_SetNowSaveModeSetupON(SAVE_CONTROL_WORK *sv);
extern void SaveData_SetNowSaveModeSetupOFF(SAVE_CONTROL_WORK *sv);

//--------------------------------------------------------------
//  �O���Z�[�u�֘A
//--------------------------------------------------------------
extern BOOL SaveData_CheckExtraMagicKey(SAVE_CONTROL_WORK *sv, SAVE_EXTRA_ID extra_id);
extern void SaveData_SetExtraMagicKey(SAVE_CONTROL_WORK *sv, SAVE_EXTRA_ID extra_id);
extern void SaveData_ClearExtraMagicKey(SAVE_CONTROL_WORK *sv, SAVE_EXTRA_ID extra_id);
extern u32 * SaveData_GetExtraLinkPtr(SAVE_CONTROL_WORK *sv, SAVE_EXTRA_ID extra_id);

#endif	//__PLAYER_DATA_H__
