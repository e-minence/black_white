//============================================================================================
/**
 * @file	trialhouse_save.h
 * @brief	�g���C�A���n�E�X�Z�[�u�f�[�^
 * @author	saito
 *
 */
//============================================================================================

#pragma once

#include "savedata/save_control.h"
#include "gamesystem/gamedata_def.h"  //GAMEDATA

//---------------------------------------------------------------------------
/**
 * @brief	���[�N�ւ̕s���S�^��`
 */
//---------------------------------------------------------------------------
typedef struct _THSV_WORK THSV_WORK;


extern int THSV_GetWorkSize(void);
extern void THSV_Init(THSV_WORK * work);
extern THSV_WORK * THSV_GetSvPtr( SAVE_CONTROL_WORK *sv );
