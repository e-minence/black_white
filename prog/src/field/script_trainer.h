//======================================================================
/**
 * @file	script_trainer.h
 * @brief	スクリプト・トレーナー関連
 * @author	tamada GAMEFREAK inc.
 * @date	2010.03.15
 *
 */
//======================================================================
#pragma once

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"
#include "field/eventwork.h"

#include "trainer_eye_data.h"

//--------------------------------------------------------------
/**
 * @brief スクリプトID --> トレーナーID
 * @param   scr_id		スクリプトID
 * @retval  "トレーナーID = フラグインデックス"
 */
//--------------------------------------------------------------
extern u16 SCRIPT_GetTrainerID_ByScriptID( u16 scr_id );

//--------------------------------------------------------------
/**
 * @brief トレーナーID --> スクリプトID
 * @param   tr_id		トレーナーID
 * @retval  "スクリプトID"
 */
//--------------------------------------------------------------
extern u16 SCRIPT_GetTrainerScriptID_ByTrainerID( u16 tr_id );

//--------------------------------------------------------------
/**
 * @brief トレーナーID --> ダブルバトル（サブ）のスクリプトID
 * @param   tr_id		トレーナーID
 * @retval  "スクリプトID"
 */
//--------------------------------------------------------------
extern u16 SCRIPT_GetTrainerScriptID_By2vs2TrainerID( u16 tr_id );

//--------------------------------------------------------------
/**
 * スクリプトIDから、左右どちらのトレーナーか取得
 *
 * @param   scr_id		スクリプトID
 *
 * @retval  "0=左、1=右"
 */
//--------------------------------------------------------------
extern BOOL SCRIPT_GetTrainerLR_ByScriptID( u16 scr_id );

//--------------------------------------------------------------
/**
 * トレーナーIDから、ダブルバトルタイプか取得
 * @param   tr_id		トレーナーID
 * @retval  "0=シングルバトル、1=ダブルバトル"
 */
//--------------------------------------------------------------
extern BOOL SCRIPT_CheckTrainer2vs2Type( u16 tr_id );

//--------------------------------------------------------------
/**
 * トレーナーIDから、トリプル/ローテバトルタイプか取得
 * @param   tr_id		トレーナーID
 * @retval  0 2vs2以下
 * @retval  1 3vs3
 */
//--------------------------------------------------------------
extern BOOL SCRIPT_CheckTrainer3vs3Type( u16 tr_id );

//--------------------------------------------------------------
/**
 * トレーナーIDからバトルルールを取得
 * @param   tr_id		トレーナーID
 * @retval  BtlRule型 BTL_RULE_SINGLE他 
 */
//--------------------------------------------------------------
extern u8 SCRIPT_GetTrainerBtlRule( u16 tr_id );

//------------------------------------------------------------------
/**
 * @brief	トレーナーイベントフラグをチェックする
 * @param	tr_id		トレーナーID
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_CheckEventFlagTrainer( EVENTWORK *ev, u16 tr_id );

//------------------------------------------------------------------
/**
 * @brief	トレーナーイベントフラグをセットする
 * @param	tr_id		トレーナーID
 * @return	none
 */
//------------------------------------------------------------------
extern void SCRIPT_SetEventFlagTrainer( EVENTWORK *ev, u16 tr_id );

//------------------------------------------------------------------
/**
 * @brief	トレーナーイベントフラグをリセットする
 * @param	tr_id		トレーナーID
 * @return	none
 */
//------------------------------------------------------------------
extern void SCRIPT_ResetEventFlagTrainer( EVENTWORK *ev, u16 tr_id );


//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern void SCRIPT_TRAINER_SetHitData(
    GMEVENT * event, u32 tr_no, const TRAINER_HITDATA * hitdata );

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern SCR_TRAINER_HITDATA * SCRIPT_GetTrainerHitData( SCRIPT_WORK * sc, u32 tr_no );


