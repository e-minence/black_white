//////////////////////////////////////////////////////////////////////////////////////
/**
 * @file   iss_sys.h
 * @brief  ISSシステム
 * @author obata_toshihiro
 * @date   2009.07.29
 */
//////////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <gflib.h>
#include "gamesystem/game_data.h"
#include "iss_switch_sys.h"  // for ISS_SWITCH_SYS


// ISSシステム構造体の不完全型の宣言
typedef struct _ISS_SYS ISS_SYS;


//----------------------------------------------------------------------------
/**
 * @brief  ISSシステムを作成する
 *
 * @param gdata	  監視対象ゲームデータ
 * @param heap_id 使用するヒープID
 * 
 * @return ISSシステム
 */
//----------------------------------------------------------------------------
extern ISS_SYS* ISS_SYS_Create( GAMEDATA* gdata, HEAPID heap_id );

//----------------------------------------------------------------------------
/**
 * @brief  ISSシステムを破棄する
 *
 * @param sys 破棄するISSシステム 
 */
//----------------------------------------------------------------------------
extern void ISS_SYS_Delete( ISS_SYS* sys );

//----------------------------------------------------------------------------
/**
 * @brief プレイヤーを監視し, 音量を調整する
 *
 * @param sys 操作対象のシステム
 */
//----------------------------------------------------------------------------
extern void ISS_SYS_Update( ISS_SYS* sys );

//----------------------------------------------------------------------------
/**
 * @brief ゾーン切り替えを通知する
 *
 * @param sys       通知対象のISSシステム
 * @param next_zone_id 新しいゾーンID
 */
//----------------------------------------------------------------------------
extern void ISS_SYS_ZoneChange( ISS_SYS* sys, u16 next_zone_id );

//----------------------------------------------------------------------------
/**
 * @brief スイッチISSシステムを取得する
 *
 * @param sys 取得対象システム
 *
 * @return スイッチISSシステム
 */
//----------------------------------------------------------------------------
extern ISS_SWITCH_SYS* ISS_SYS_GetIssSwitchSystem( const ISS_SYS* sys );
