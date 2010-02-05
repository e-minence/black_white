//==============================================================================
/**
 * @file	undata_update.h
 * @brief	���A�f�[�^�X�V����
 * @author	saito
 * @date	2010.02.03
 */
//==============================================================================
#pragma once

#include "unitednations_def.h" 
#include "wifihistory.h"

extern void UNDATAUP_Update(WIFI_HISTORY * wh, UNITEDNATIONS_SAVE *add_data);
extern u32 UNDATAUP_GetDataNum(const UNITEDNATIONS_SAVE *un_data);


