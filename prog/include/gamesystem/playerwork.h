//======================================================================
/**
 * @file	playerwork.h
 * @brief	ポケモンゲームシステム・データ定義
 * @author	tamada GAME FREAK Inc.
 * @date	08.10.22
 */
//======================================================================
#pragma once
#ifdef	__cplusplus
extern "C" {
#endif

#include "savedata/mystatus.h"
#include "savedata/mystatus_local.h"

//======================================================================
//  define
//======================================================================
typedef u16 ZONEID; ///<ZONEID

//--------------------------------------------------------------
/// PLAYER_MOVE_FORM
//--------------------------------------------------------------
typedef enum
{
  PLAYER_MOVE_FORM_NORMAL = 0,   ///<自機二足歩行
  PLAYER_MOVE_FORM_CYCLE,  ///<自機自転車移動
  PLAYER_MOVE_FORM_SWIM, ///<自機波乗り移動
  PLAYER_MOVE_FORM_MAX,
}PLAYER_MOVE_FORM;

//======================================================================
//  struct
//======================================================================
//--------------------------------------------------------------
/**
 * @brief	PLAYER_WORK 自機データ定義
 * @note 最低限の「自機」をゲーム内に存在させるためのデータ。
 * 通信同期時などで他プレイヤーのデータなども保持する。
 */
//--------------------------------------------------------------
typedef struct {
	ZONEID zoneID;
	VecFx32 position;
	u16 direction;
  u8 palace_area;       ///<※check　パレステストの為、一時的にここに配置
  u8 padding;
	MYSTATUS mystatus;
  PLAYER_MOVE_FORM move_form;
}PLAYER_WORK;

//======================================================================
//  extern
//======================================================================
//--------------------------------------------------------------
/**
 * @brief	プレイヤーワークの初期化
 * @param	player	PLAYER_WORKへのポインタ
 */
//--------------------------------------------------------------
extern void PLAYERWORK_init(PLAYER_WORK * player);

//--------------------------------------------------------------
/**
 * @brief	位置をセットする
 * @param	player	PLAYER_WORKへのポインタ
 * @param	pos		位置を指定する値
 */
//--------------------------------------------------------------
extern void PLAYERWORK_setPosition(PLAYER_WORK * player, const VecFx32 * pos);

//--------------------------------------------------------------
/**
 * @brief	位置を取得する
 * @param	player	PLAYER_WORKへのポインタ
 * @return	VecFx32* 位置を指定するVecFx32へのポインタ
 */
//--------------------------------------------------------------
extern const VecFx32 * PLAYERWORK_getPosition(const PLAYER_WORK * player);

//--------------------------------------------------------------
/**
 * @brief	位置を取得する
 * @param	player	PLAYER_WORKへのポインタ
 * @param	zoneid	マップを指定する値
 */
//--------------------------------------------------------------
extern void PLAYERWORK_setZoneID(PLAYER_WORK * player, ZONEID zoneid);

//--------------------------------------------------------------
/**
 * @brief	位置を取得する
 * @param	player	PLAYER_WORKへのポインタ
 * @return	ZONEID	マップを指定する値
 */
//--------------------------------------------------------------
extern ZONEID PLAYERWORK_getZoneID(const PLAYER_WORK * player);

//--------------------------------------------------------------
/**
 * @brief	位置を取得する
 * @param	player		PLAYER_WORKへのポインタ
 * @param	direction	方向を指定する値
 */
//--------------------------------------------------------------
extern void PLAYERWORK_setDirection(PLAYER_WORK * player, u16 direction);

//--------------------------------------------------------------
/**
 * @brief	方向を取得する
 * @param	player	PLAYER_WORKへのポインタ
 * @return	方向を指定する値
 */
//--------------------------------------------------------------
extern u16 PLAYERWORK_getDirection(const PLAYER_WORK * player);

//--------------------------------------------------------------
/**
 * @brief	パレスエリア番号を設定する
 * @param	player	      PLAYER_WORKへのポインタ
 * @param	palace_area   パレス番号
 * 
 * ※check　パレステストの為の暫定関数
 */
//--------------------------------------------------------------
extern void PLAYERWORK_setPalaceArea(PLAYER_WORK * player, u8 palace_area);

//--------------------------------------------------------------
/**
 * @brief	パレスエリア番号を取得する
 * @param	player	PLAYER_WORKへのポインタ
 * @return	パレスエリア番号
 * 
 * ※check　パレステストの為の暫定関数
 */
//--------------------------------------------------------------
extern u8 PLAYERWORK_getPalaceArea(const PLAYER_WORK * player);

//--------------------------------------------------------------
/**
 * PLAYER_MOVE_FORM取得
 * @param player PLAYER_WORKへのポインタ
 * @retval PLAYER_MOVE_FORM
 */
//--------------------------------------------------------------
extern PLAYER_MOVE_FORM PLAYERWORK_GetMoveForm( const PLAYER_WORK *player );

//--------------------------------------------------------------
/**
 * PLAYER_MOVE_FORMセット
 * @param player PLAYER_WORKへのポインタ
 * @param form PLAYER_MOVE_FORM
 * @retval none
 */
//--------------------------------------------------------------
extern void PLAYERWORK_SetMoveForm( PLAYER_WORK *player, PLAYER_MOVE_FORM form );

#ifdef	__cplusplus
} /* extern "C" */
#endif
