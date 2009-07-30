////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_unit_city.h
 * @brief  街ISSユニット
 * @author obata_toshihiro
 * @date   2009.07.16
 */
////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>
#include "gamesystem/playerwork.h"


// 街ISSユニット構造体の不完全型の宣言
typedef struct _ISS_UNIT_CITY ISS_UNIT_CITY;


//----------------------------------------------------------------------------
/**
 * @brief  街ISSユニットを作成する
 *
 * @param  p_player 監視対象のプレイヤー
 * @param  zone_id  ゾーンID
 * @param  heap_id  使用するヒープID
 * 
 * @return 街ISSユニット
 */
//----------------------------------------------------------------------------
ISS_UNIT_CITY* ISS_UNIT_CITY_Create( PLAYER_WORK* p_player, u16 zone_id, HEAPID heap_id );

//----------------------------------------------------------------------------
/**
 * @brief  街ISSユニットを破棄する
 *
 * @param p_unit 破棄する街ISSユニット 
 */
//----------------------------------------------------------------------------
void ISS_UNIT_CITY_Delete( ISS_UNIT_CITY* p_unit );

//----------------------------------------------------------------------------
/**
 * @brief プレイヤーを監視し, 音量を調整する
 *
 * @param p_unit 操作対象のユニット
 */
//----------------------------------------------------------------------------
void ISS_UNIT_CITY_Update( ISS_UNIT_CITY* p_unit );

//----------------------------------------------------------------------------
/**
 * @brief ゾーン切り替えを通知する
 *
 * @param p_unit       通知対象の街ISSユニット
 * @param next_zone_id 新しいゾーンID
 */
//----------------------------------------------------------------------------
void ISS_UNIT_CITY_ZoneChange( ISS_UNIT_CITY* p_unit, u16 next_zone_id );

//----------------------------------------------------------------------------
/**
 * @brief 動作状態を設定する
 *
 * @param p_unit 設定を変更するISSユニット
 * @param active 動作させるかどうか
 */
//----------------------------------------------------------------------------
extern void ISS_UNIT_CITY_SetActive( ISS_UNIT_CITY* p_unit, BOOL active );

//----------------------------------------------------------------------------
/**
 * @breif 動作状態を取得する
 *
 * @param p_unit 状態を調べるISSユニット
 * 
 * @return 動作中かどうか
 */
//----------------------------------------------------------------------------
extern BOOL ISS_UNIT_CITY_IsActive( const ISS_UNIT_CITY* p_unit );
