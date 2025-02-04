//////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_dungeon_sys.h
 * @brief  ダンジョンISSシステム
 * @author obata_toshihiro
 * @date   2009.07.16
 */
//////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>
#include "gamesystem/playerwork.h"


//============================================================================
// ■ダンジョンISSシステムの不完全型
//============================================================================
typedef struct _ISS_DUNGEON_SYS ISS_DUNGEON_SYS;


//----------------------------------------------------------------------------
/**
 * @brief  ダンジョンISSシステムを作成する
 *
 * @param  gdata    ゲームデータ
 * @param  player   監視対象のプレイヤー
 * @param  heap_id  使用するヒープID
 * 
 * @return ダンジョンISSシステム
 */
//----------------------------------------------------------------------------
extern ISS_DUNGEON_SYS* ISS_DUNGEON_SYS_Create( 
    GAMEDATA* gdata, PLAYER_WORK* player, HEAPID heap_id );

//----------------------------------------------------------------------------
/**
 * @brief  ダンジョンISSシステムを破棄する
 *
 * @param sys 破棄するダンジョンISSシステム 
 */
//----------------------------------------------------------------------------
extern void ISS_DUNGEON_SYS_Delete( ISS_DUNGEON_SYS* sys );

//----------------------------------------------------------------------------
/**
 * @brief プレイヤーを監視し, 音量を調整する
 *
 * @param sys 操作対象のシステム
 */
//----------------------------------------------------------------------------
extern void ISS_DUNGEON_SYS_Update( ISS_DUNGEON_SYS* sys );

//----------------------------------------------------------------------------
/**
 * @brief ゾーン切り替えを通知する
 *
 * @param sys       通知対象のダンジョンISSシステム
 * @param next_zone_id 新しいゾーンID
 */
//----------------------------------------------------------------------------
extern void ISS_DUNGEON_SYS_ZoneChange( ISS_DUNGEON_SYS* sys, u16 next_zone_id );

//----------------------------------------------------------------------------
/**
 * @brief システムを起動する
 *
 * @param sys 起動するシステム
 */
//----------------------------------------------------------------------------
extern void ISS_DUNGEON_SYS_On( ISS_DUNGEON_SYS* sys );

//----------------------------------------------------------------------------
/**
 * @brief システムを停止させる
 *
 * @param sys 停止させるシステム
 */
//----------------------------------------------------------------------------
extern void ISS_DUNGEON_SYS_Off( ISS_DUNGEON_SYS* sys );

//----------------------------------------------------------------------------
/**
 * @breif 動作状態を取得する
 *
 * @param sys 状態を調べるISSシステム
 * 
 * @return 動作中かどうか
 */
//----------------------------------------------------------------------------
extern BOOL ISS_DUNGEON_SYS_IsOn( const ISS_DUNGEON_SYS* sys );

//----------------------------------------------------------------------------
/**
 * @breif 設定データの有無を調べる
 *
 * @param system
 * @param zone_id
 * 
 * @return 指定したゾーンの設定データを持っている場合 TRUE
 *         そうでなければ FALSE
 */
//----------------------------------------------------------------------------
extern BOOL ISS_DUNGEON_SYS_IsActiveAt( const ISS_DUNGEON_SYS* system, u16 zone_id );
