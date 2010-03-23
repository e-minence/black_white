//======================================================================
/**
 * @file	event_battle.h
 * @brief	�C�x���g�F�o�g���Ăяo��
 * @author	tamada GAMEFREAK inc.
 * @date	2009.01.19
 */
//======================================================================
#pragma once
#include <gflib.h>

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "battle/battle_bg_def.h"
#include "encount_data.h"    //for ENCOUNT_TYPE

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  extern
//======================================================================
extern GMEVENT * EVENT_WildPokeBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BATTLE_SETUP_PARAM* bp, BOOL sub_event_f, ENCOUNT_TYPE enc_type );

extern GMEVENT * EVENT_TrainerBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, int rule, int partner_id, int tr_id0, int tr_id1, u32 flags );

extern GMEVENT * EVENT_BSubwayTrainerBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BATTLE_SETUP_PARAM *bp );

extern GMEVENT * EVENT_TrialHouseTrainerBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, BATTLE_SETUP_PARAM *bp );

extern GMEVENT * EVENT_CaptureDemoBattle( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, HEAPID heapID );

//--------------------------------------------------------------
/**
 * @brief �u�s�k�����v�Ƃ���ׂ����ǂ����̔���
 * @param result  �o�g���V�X�e�����Ԃ��퓬����
 * @param competitor  �ΐ푊��̎��
 * @return  BOOL  TRUE�̂Ƃ��A�s�k����������ׂ�
 */
//--------------------------------------------------------------
extern BOOL FIELD_BATTLE_IsLoseResult(BtlResult result, BtlCompetitor competitor);

//--------------------------------------------------------------
/**
 * @brief �쐶��@�Đ�R�[�h�`�F�b�N
 * @param result  �o�g���V�X�e�����Ԃ��퓬����
 * @return  SCR_WILD_BTL_RES_XXXX
 */
//--------------------------------------------------------------
extern u8 FIELD_BATTLE_GetWildBattleRevengeCode(BtlResult result);

