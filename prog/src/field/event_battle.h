//======================================================================
/**
 * @file	event_battle.h
 * @brief	イベント：バトル呼び出し
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
#include "savedata/tradepoke_after_save.h"    //for TRPOKE_AFTER_SAVE_TYPE

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

extern GMEVENT * EVENT_TradeAfterTrainerBattle(
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldmap, int rule, int partner_id, int tr_id0, int tr_id1, u32 flags, TRPOKE_AFTER_SAVE_TYPE trade_type );

//--------------------------------------------------------------
/**
 * @brief 「敗北処理」とするべきかどうかの判定
 * @param result  バトルシステムが返す戦闘結果
 * @param competitor  対戦相手の種類
 * @return  BOOL  TRUEのとき、敗北処理をするべき
 */
//--------------------------------------------------------------
extern BOOL FIELD_BATTLE_IsLoseResult(BtlResult result, BtlCompetitor competitor);

//--------------------------------------------------------------
/**
 * @brief 野生戦　再戦コードチェック
 * @param result  バトルシステムが返す戦闘結果
 * @return  SCR_WILD_BTL_RES_XXXX
 */
//--------------------------------------------------------------
extern u8 FIELD_BATTLE_GetWildBattleRevengeCode(BtlResult result);

