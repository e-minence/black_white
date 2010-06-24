//==============================================================================
/**
 * @file    monolith_tool_notwifi.h
 * @brief   not_wifi�ɔz�u�����c�[����
 * @author  matsuda
 * @date    2010.06.24(��)
 */
//==============================================================================
#pragma once

#include "gamesystem/g_power.h"

//==============================================================================
//  �萔��`
//==============================================================================
///G�p���[�K����
typedef enum{
  MONO_USE_POWER_OK,        ///<�K�����Ă���
  MONO_USE_POWER_SOMEMORE,  ///<���Ə����ŏK��
  MONO_USE_POWER_NONE,      ///<�K�����Ă��Ȃ�
  
  MONO_USE_POWER_MAX,
}MONO_USE_POWER;


//==============================================================================
//  �O���֐��錾
//==============================================================================
extern MONO_USE_POWER MonolithToolEx_CheckUsePower(const POWER_CONV_DATA *powerdata, GPOWER_ID gpower_id, const OCCUPY_INFO *occupy, const u8 *gpower_distribution_bit);
extern u32 MonolithToolEx_CountUsePower(const POWER_CONV_DATA *powerdata, const OCCUPY_INFO *occupy, const u8 *gpower_distribution_bit);
