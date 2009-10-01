//=============================================================================================
/**
 * @file  btl_setup.h
 * @brief �|�P����WB �o�g���p�����[�^�ȈՃZ�b�g�A�b�v����
 * @author  taya
 *
 * @date  2009.10.01 �쐬
 */
//=============================================================================================
#pragma once

#include "battle/battle.h"
#include "gamesystem/game_data.h"

extern void BTL_SETUP_Single_Wild( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlLandForm landForm, BtlWeather weather );

extern void BTL_SETUP_Single_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlLandForm landForm, BtlWeather weather, TrainerID trID );

extern void BTL_SETUP_Single_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode );

extern void BTL_SETUP_Double_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlLandForm landForm, BtlWeather weather, TrainerID trID );

extern void BTL_SETUP_Double_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode );

extern void BTL_SETUP_Triple_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlLandForm landForm, BtlWeather weather, TrainerID trID );
