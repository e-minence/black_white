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

/*
 *  @brief  �퓬�p�����[�^���[�N����
 *  @param  heapID  ���[�N���������m�ۂ���heapID
 *
 *  ��BATTLE_SETUP_PARAM�\���̗̈���A���P�[�g���A���������܂�
 *  ���f�t�H���g�̃p�����[�^�ō\���̂����������܂��B�o�g���^�C�v�ɉ����āA�K�v�ȏ�������ǉ��ōs���Ă�������
 *  ���K�� BATTLE_PARAM_Delete()�ŉ�����Ă�������
 */
extern BATTLE_SETUP_PARAM* BATTLE_PARAM_Create( int heapID );

/*
 *  @brief  �퓬�p�����[�^���[�N���
 *  @param  bp  BATTLE_PARAM_Create()�Ő������ꂽBATTLE_SETUP_PARAM�\���̌^���[�N�ւ̃|�C���^
 */
extern void BATTLE_PARAM_Delete( BATTLE_SETUP_PARAM* bp );

/**
 *  @brief  �퓬�p�����[�^���[�N�̓�������������
 *  @param  bp  �m�ۍς݂�BATTLE_SETUP_PARAM�\���̌^���[�N�ւ̃|�C���^
 *
 *  ���f�t�H���g�̃p�����[�^�ō\���̂����������܂��B�o�g���^�C�v�ɉ����āA�K�v�ȏ�������ǉ��ōs���Ă�������
 *  ���g���I�������K��BATTLE_PARAM_Release()�֐��ŉ�����������Ă�������
 */
extern void BATTLE_PARAM_Init( BATTLE_SETUP_PARAM* bp );

/*
 *  @brief  �퓬�p�����[�^���[�N�̓����A���P�[�V��������������ƃN���A
 *  @param  bp  �m�ۍς݂�BATTLE_SETUP_PARAM�\���̌^���[�N�ւ̃|�C���^
 */
extern void BATTLE_PARAM_Release( BATTLE_SETUP_PARAM* bp );

extern void BP_SETUP_Wild( BATTLE_SETUP_PARAM* bp, GAMEDATA* gameData, int heapID, const BtlRule rule, 
  const POKEPARTY* partyEnemy, const BtlLandForm landForm, const BtlWeather weather );

extern void BTL_SETUP_Single_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlLandForm landForm, BtlWeather weather, TrainerID trID );

extern void BTL_SETUP_Single_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode );

extern void BTL_SETUP_Double_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlLandForm landForm, BtlWeather weather, TrainerID trID );

extern void BTL_SETUP_Double_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode );

extern void BTL_SETUP_Multi_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode, u8 commPos );

extern void BTL_SETUP_Triple_Trainer( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  POKEPARTY* partyEnemy, BtlLandForm landForm, BtlWeather weather, TrainerID trID );

extern void BTL_SETUP_Triple_Comm( BATTLE_SETUP_PARAM* dst, GAMEDATA* gameData,
  GFL_NETHANDLE* netHandle, BtlCommMode commMode );
