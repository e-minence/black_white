///////////////////////////////////////////////////////////////////////////////////////
/**
 *
 * @brief ポケモン系の画面呼び出しイベント
 * @file  event_poke_status.c
 * @author ariizumi
 * @date   2009.10.22
 *
 */
///////////////////////////////////////////////////////////////////////////////////////

#pragma once

//-------------------------------------------------------------------------------------
/**
 * @brief ポケモン選択イベント
 *
 * @param gsys      ゲームシステム
 * @param retDecide 選択結果の格納先ワーク
 * @param retValue  選択位置の格納先ワーク
 *
 * @return イベント
 */
//-------------------------------------------------------------------------------------
extern GMEVENT * EVENT_CreatePokeSelect( 
    GAMESYS_WORK * gsys, u16* retDecide , u16* retPos );

//------------------------------------------------------------------
/**
 * @brief ポケモン選択イベント：わざ覚え
 *
 * @param gsys      ゲームシステム
 * @param fieldmap  フィールドマップ
 * @param retDecide 選択結果の格納先ワーク
 * @param retValue  選択位置の格納先ワーク
 * @param learnBit  覚えられるポケモンを示すビット
 * @param wazano    覚える対象のわざナンバー
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_CreatePokeSelectWazaOboe( 
    GAMESYS_WORK * gsys, u16* retDecide , u16* retPos, u8 learnBit, u16 wazano );

//-------------------------------------------------------------------------------------
/**
 * @brief ポケモン選択イベント：ミュージカル
 *
 * @param gsys      ゲームシステム
 * @param retDecide 選択結果の格納先ワーク
 * @param retValue  選択位置の格納先ワーク
 *
 * @return イベント
 */
//-------------------------------------------------------------------------------------
extern GMEVENT * EVENT_CreatePokeSelectMusical( 
    GAMESYS_WORK * gsys, u16* retDecide , u16* retPos );

//------------------------------------------------------------------
/**
 * @brief ポケモン選択イベント
 *
 * @param gsys      ゲームシステム
 * @param fieldmap  フィールドマップ
 * @param pokePos   ポケモンの位置
 * @param retDecide 選択結果の格納先ワーク
 * @param retPos    選択位置の格納先ワーク
 * @param canSelHiden 秘伝選択可能
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_CreateWazaSelect( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, u16 pokePos , u16* retDecide , u16* retPos , const BOOL canSelHiden, u16 wazaNo );
