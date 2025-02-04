////////////////////////////////////////////////////////////////////////////////
/**
 * @brief  ポケセン回復アニメーションイベント
 * @file   event_pc_recovery.h
 * @author obata_toshihiro
 * @date   2009.09.18
 */
//////////////////////////////////////////////////////////////////////////////// 
#pragma once 

//------------------------------------------------------------------------------
/**
 * @brief ポケセン回復アニメーションイベントを作成する
 *
 * @param gsys        ゲームシステム
 * @param parent      親イベント
 * @param pokemon_num 手持ちポケモンの数
 *
 * @return 作成した回復アニメイベント
 */
//------------------------------------------------------------------------------
GMEVENT* EVENT_PcRecoveryAnime( GAMESYS_WORK* gsys, GMEVENT* parent, u8 pokemon_num );
