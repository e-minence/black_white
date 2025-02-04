////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  デバッグメニュー『数値入力』 ( すれ違い調査関連 )
 * @file   debug_numinput_research.c
 * @author obata
 * @date   2010.03.13
 */
//////////////////////////////////////////////////////////////////////////////// 
#pragma once
#include <gflib.h>
#include "gamesystem/gamesystem.h" // for GAMESYS_WORK
#include "gamesystem/game_data.h" // for GAMEDATA
#include "event_debug_numinput.h" // for DEBUG_NUMINPUT_XXXX
#include "savedata/questionnaire_save.h"

//------------------------------------------------------------------------------
/**
 * @brief 数値取得関数 ( 数値入力 → 今日のすれ違い調査人数 → Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param questionID 取得する質問のID ( QUESTION_ID_xxxx )
 *
 * @return 質問に対する, 今日の回答人数
 */
//------------------------------------------------------------------------------
extern u32 DEBUG_GetTodayCountOfQuestion( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 questionID ); 
//------------------------------------------------------------------------------
/**
 * @brief 数値設定関数 ( 数値入力 → 今日のすれ違い調査人数 → Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param questionID 設定する質問のID ( QUESTION_ID_xxxx )
 * @param count      設定する数値
 */
//------------------------------------------------------------------------------
extern void DEBUG_SetTodayCountOfQuestion( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 questionID, u32 count );
//------------------------------------------------------------------------------
/**
 * @brief 数値取得関数 ( 数値入力 → いままでのすれ違い調査人数 → Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param questionID 取得する質問のID ( QUESTION_ID_xxxx )
 *
 * @return 質問に対する, いままでの回答人数
 */
//------------------------------------------------------------------------------
extern u32 DEBUG_GetTotalCountOfQuestion( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 questionID ); 
//------------------------------------------------------------------------------
/**
 * @brief 数値設定関数 ( 数値入力 → いままでのすれ違い調査人数 → Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param questionID 設定する質問のID ( QUESTION_ID_xxxx )
 * @param count      設定する数値
 */
//------------------------------------------------------------------------------
extern void DEBUG_SetTotalCountOfQuestion( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 questionID, u32 count );
//------------------------------------------------------------------------------
/**
 * @brief 数値取得関数 ( 数値入力 → 今日のすれ違い回答人数 → Qx )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID 取得する質問のID ( ANSWER_ID_xxxx )
 *
 * @return 質問に対する, 今日の回答人数
 */
//------------------------------------------------------------------------------
extern u32 DEBUG_GetTodayCountOfAnswer( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID ); 
//------------------------------------------------------------------------------
/**
 * @brief 数値設定関数 ( 数値入力 → 今日のすれ違い回答人数 → Ax )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID 設定する質問のID ( ANSWER_ID_xxxx )
 * @param count      設定する数値
 */
//------------------------------------------------------------------------------
extern void DEBUG_SetTodayCountOfAnswer( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID, u32 count );
//------------------------------------------------------------------------------
/**
 * @brief 数値取得関数 ( 数値入力 → いままでのすれ違い回答人数 → Ax )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID 取得する回答のID ( ANSWER_ID_xxxx )
 *
 * @return 質問に対する, いままでの回答人数
 */
//------------------------------------------------------------------------------
extern u32 DEBUG_GetTotalCountOfAnswer( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID ); 
//------------------------------------------------------------------------------
/**
 * @brief 数値設定関数 ( 数値入力 → いままでのすれ違い回答人数 → Ax )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID   設定する回答のID ( ANSWER_ID_xxxx )
 * @param count      設定する数値
 */
//------------------------------------------------------------------------------
extern void DEBUG_SetTotalCountOfAnswer( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID, u32 count ); 
//------------------------------------------------------------------------------
/**
 * @brief 数値取得関数 ( 数値入力 → すれ違い調査隊 → 隊員ランク )
 *
 * @param gameSystem
 * @param gameData
 * @param rank       隊員ランク ( RESEARCH_TEMP_RANK_xxxx )
 *
 * @return 隊員ランク
 */
//------------------------------------------------------------------------------
extern u32 DEBUG_GetResearchTeamRank( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 rank ); 
//------------------------------------------------------------------------------
/**
 * @brief 数値設定関数 ( 数値入力 → すれ違い調査隊 → 隊員ランク )
 *
 * @param gameSystem
 * @param gameData
 * @param answerID   設定する回答のID ( ANSWER_ID_xxxx )
 * @param rank       設定する数値
 */
//------------------------------------------------------------------------------
extern void DEBUG_SetResearchTeamRank( 
    GAMESYS_WORK* gameSystem, GAMEDATA* gameData, u32 answerID, u32 rank );
