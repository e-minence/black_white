//======================================================================
/**
 * @file  event_mapchange.h
 * @brief マップ遷移関連イベント
 * @date  2008.11.05
 * @author  tamada GAME FREAK inc.
 *
 * @note
 * 基本的にはイベントのみを外部公開して複雑な初期化やフラグ制御は
 * この内部に納めたい。（現在のところ例外はMAPCHG_GameOver）
 */
//======================================================================
#pragma once

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "field/fieldmap.h"
#include "field/eventdata_sxy.h"
//======================================================================
//======================================================================
//------------------------------------------------------------------
/**
 * @brief ゲーム開始イベント
 */
//------------------------------------------------------------------
extern GMEVENT * EVENT_CallGameStart(GAMESYS_WORK * gsys, GAME_INIT_WORK * game_init_work);

#ifdef  PM_DEBUG
//------------------------------------------------------------------
/**
 * @brief デバッグ用：ゲーム終了
 */
//------------------------------------------------------------------
extern GMEVENT * DEBUG_EVENT_CallGameEnd( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap);
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapByConnect( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                          const CONNECT_DATA* connectData, LOC_EXIT_OFS exitOfs );
extern GMEVENT* EVENT_ChangeMapByConnectIntrude( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                   const CONNECT_DATA* connectData, LOC_EXIT_OFS exitOfs );

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成 ( 座標指定 )
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID             遷移先マップのZONE指定
 * @param pos                遷移先マップでの座標指定
 * @param dir                遷移先マップでの方向指定
 * @param seasonUpdateEnable 季節の更新処理を行うかどうか
 *
 * @return 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapPos( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                    u16 zoneID, const VecFx32* pos, u16 dir, 
                                    BOOL seasonUpdateEnable );

//------------------------------------------------------------------
/**
 * @brief 空を飛ぶマップ遷移イベント生成 ( 座標指定 )
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID      遷移先マップのZONE指定
 * @param loc         遷移先マップでのロケーション指定
 * @param dir         遷移先マップでの方向指定
 *
 * @return 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapSorawotobu( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                           u16 zoneID, const LOCATION* loc, u16 dir );

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成 ( レール座標指定 )
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID    遷移先マップのZONE指定
 * @param rail_loc  遷移先マップでのレール座標指定
 * @param dir       遷移先マップでの方向指定
 *
 * @return 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapRailLocation( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                                   u16 zoneID, const RAIL_LOCATION* rail_loc, u16 dir, BOOL seasonUpdateEnable );

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成 ( レール座標指定 )ノーフェード版
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID    遷移先マップのZONE指定
 * @param rail_loc  遷移先マップでのレール座標指定
 * @param dir       遷移先マップでの方向指定
 *
 * @return 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapRailLocNoFade( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                                   u16 zoneID, const RAIL_LOCATION* rail_loc, u16 dir, BOOL seasonUpdateEnable );

#ifdef  PM_DEBUG
//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成 ( デフォルト座標 )
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID      遷移先マップのZONE指定
 *
 * @return 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_ChangeMapDefaultPos( GAMESYS_WORK* gameSystem, 
                                                 FIELDMAP_WORK* fieldmap, u16 zoneID );

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成 ( デフォルト座標 / フェード短縮 )
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID    遷移先マップのZONE指定
 *
 * @return 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
extern GMEVENT* DEBUG_EVENT_QuickChangeMapDefaultPos( GAMESYS_WORK * gameSystem,
                                                      FIELDMAP_WORK* fieldmap, u16 zoneID );
extern GMEVENT * DEBUG_EVENT_QuickChangeMapAppoint( GAMESYS_WORK * gameSystem,
                                    FIELDMAP_WORK* fieldmap, u16 zoneID,
                                    const VecFx32 *pos );
#endif

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成（ 流砂 ）
 *
 * @param gameSystem
 * @param fieldmap
 * @param disappearPos  流砂中心点の座標
 * @param zoneID        遷移先マップのZONE指定
 * @param appearPos     遷移先での座標指定
 *
 * @return GMEVENT 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapBySandStream( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,  
                                             const VecFx32* disappearPos, 
                                             u16 zoneID, const VecFx32* appearPos );

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成 ( あなぬけのヒモ / あなをほる / テレポート/海底神殿からの退場 )
 *
 * @param gameSystem
 *
 * @return GMEVENT 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapByAnanukenohimo( FIELDMAP_WORK* fieldWork, GAMESYS_WORK* gameSystem );
extern GMEVENT* EVENT_ChangeMapByAnawohoru( GAMESYS_WORK* gameSystem );
extern GMEVENT* EVENT_ChangeMapByTeleport( GAMESYS_WORK* gameSystem );
extern GMEVENT* EVENT_ChangeMapBySeaTempleUp( GAMESYS_WORK* gameSystem );
extern GMEVENT* EVENT_ChangeMapBySeaTempleDown( GAMESYS_WORK* gameSystem, u16 zone_id );

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成（ ワープ )
 * @param gameSystem
 * @param fieldmap
 * @param zoneID 
 * @return GMEVENT 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapByWarp( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                       u16 zoneID , const VecFx32 * pos, u16 dir );

//------------------------------------------------------------------
/**
 * @brief ユニオンへのマップ遷移イベント生成
 *
 * @param  gameSystem
 * @param  fieldmap
 *
 * @return 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapToUnion( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap );
extern GMEVENT* EVENT_ChangeMapFromUnion( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap );

//------------------------------------------------------------------
/**
 * @brief パレス・裏フィールドへのマップ遷移イベント生成
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapFldToPalace( GAMESYS_WORK* gsys, u16 zone_id, const VecFx32* pos, BOOL partner );
extern GMEVENT * EVENT_ChangeMapFromPalace( GAMESYS_WORK * gameSystem );
extern GMEVENT * EVENT_ChangeMapPalace_to_Palace( GAMESYS_WORK* gameSystem, u16 zoneID, const VecFx32* pos );

//------------------------------------------------------------------
/**
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapPosNoFade( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                          u16 zoneID, const VecFx32* pos, u16 dir );

//------------------------------------------------------------------
/**
 * @brief マップ遷移イベント生成 ( BGM変更なし )
 *
 * @param gameSystem
 * @param fieldmap
 * @param zoneID     遷移先マップのZONE指定
 * @param pos        遷移先マップでの座標指定
 * @param dir        遷移先マップでの方向指定
 *
 * @return 生成したマップ遷移イベント
 */
//------------------------------------------------------------------
extern GMEVENT* EVENT_ChangeMapBGMKeep( GAMESYS_WORK* gameSystem, FIELDMAP_WORK* fieldmap,
                                        u16 zoneID, const VecFx32* pos, u16 dir );

//------------------------------------------------------------------
//------------------------------------------------------------------
extern void MAPCHANGE_setPlayerVanish(FIELDMAP_WORK * fieldmap, BOOL vanish_flag);

//--------------------------------------------------------------
/**
 * @brief 全滅時のマップ遷移処理（フィールド非生成時）
 */
//--------------------------------------------------------------
extern GMEVENT * EVENT_CallGameOver( GAMESYS_WORK * gsys ); 
