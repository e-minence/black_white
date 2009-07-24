//============================================================================================
/**
 * @file	game_data.h
 * @brief	ゲーム進行データ管理
 * @date	2008.11.04
 *
 * 基本的にゲーム進行データはセーブデータとして存在するが、
 * 通信時の複数プレイヤー情報やフィールドマップ情報などセーブデータに
 * のるとは限らない情報もある。
 * そのようなセーブデータ・非セーブデータへ並列にアクセスするインターフェイスを
 * 各パートごとに構成するとパートごとにアクセス手法が違ったり、また同じ機能の
 * ものが複数存在するなどプログラム全体の複雑さが増してしまう可能性が高い。
 * それを避けるため、共通インターフェイスを作ることによって簡略化する試み
 * …試みなので途中で方針を変えるかも。
 */
//============================================================================================
#pragma once

#include "gamesystem/playerwork.h"
#include "field/eventdata_system.h"
#include "field/location.h"
#include "field/rail_location.h"  //RAIL_LOCATION
#include "savedata/myitem_savedata.h"  //BAG_CURSOR MYITEM
#include "poke_tool/pokeparty.h"
#include "field/field_rail_loader.h"    //FIELD_RAIL_LOADER
#include "field/fldmmdl.h"
#include "field/eventwork.h"

//============================================================================================
//============================================================================================
enum {
	PLAYER_MAX = 5,
	
	PLAYER_ID_MINE = PLAYER_MAX - 1,    ///<自分自身のプレイヤーID
};

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	GAMEDATA型定義
 *
 * _GAMEDATAの実体はgame_data.c内に定義されている
 */
//------------------------------------------------------------------
typedef struct _GAMEDATA GAMEDATA;

//============================================================================================
//	ゲームデータ本体の生成・破棄処理
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	GAMEDATAの生成処理
 * @param	heapID		使用するヒープ指定ID
 * @return	GAMEDATA	生成したGAMEDATAへのポインタ
 */
//------------------------------------------------------------------
extern GAMEDATA * GAMEDATA_Create(HEAPID heapID);
//------------------------------------------------------------------
/**
 * @brief	GAMEDATAの破棄処理
 * @param	gamedata	破棄するGAMEDATAへのポインタ
 */
//------------------------------------------------------------------
extern void GAMEDATA_Delete(GAMEDATA * gamedata);

//============================================================================================
//	PLAYER_WORKへのアクセス
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	プレイヤーデータを取得する
 * @param	gamedata	GAMEDATAへのポインタ
 * @param	player_id	プレイヤー指定ID
 * @return	PLAYER_WORK	プレイヤーデータへのポインタ
 */
//------------------------------------------------------------------
extern PLAYER_WORK * GAMEDATA_GetPlayerWork(GAMEDATA * gamedata, u32 player_id);
//------------------------------------------------------------------
/**
 * @brief	自分のプレイヤーデータを取得する
 * @param	gamedata	GAMEDATAへのポインタ
 * @return	PLAYER_WORK	自分のプレイヤーデータへのポインタ
 */
//------------------------------------------------------------------
extern PLAYER_WORK * GAMEDATA_GetMyPlayerWork(GAMEDATA * gamedata);

//============================================================================================
//	EVENTDATA_SYSTEMへのアクセス
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	イベント起動データシステムへのポインタを取得する
 * @param	gamedata			GAMEDATAへのポインタ
 * @return	EVENTDATA_SYSTEM	イベント起動データシステムへのポインタ
 */
//------------------------------------------------------------------
extern EVENTDATA_SYSTEM * GAMEDATA_GetEventData(GAMEDATA * gamedata);

//------------------------------------------------------------------
/**
 * @brief	開始位置の取得
 * @param	gamedata			GAMEDATAへのポインタ
 * @return	LOCATION		開始位置情報へのポインタ
 */
//------------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetStartLocation(const GAMEDATA * gamedata);
//------------------------------------------------------------------
/**
 * @brief	開始位置のセット
 * @param	gamedata
 * @param	loc
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetStartLocation(GAMEDATA * gamedata, const LOCATION * loc);

//------------------------------------------------------------------
/**
 * @brief	直前の出入口情報の取得
 * @param	gamedata			GAMEDATAへのポインタ
 * @return	LOCATION		直前の出入口情報へのポインタ
 */
//------------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetEntranceLocation(const GAMEDATA * gamedata);
//------------------------------------------------------------------
/**
 * @brief	直前の出入口情報のセット
 * @param	gamedata
 * @param	loc
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetEntranceLocation(GAMEDATA * gamedata, const LOCATION * loc);

//------------------------------------------------------------------
/**
 * @brief	特殊出入口情報の取得
 * @param	gamedata		GAMEDATAへのポインタ
 * @return	LOCATION		特殊出入口情報へのポインタ
 */
//------------------------------------------------------------------
extern const LOCATION * GAMEDATA_GetSpecialLocation(const GAMEDATA * gamedata);
//------------------------------------------------------------------
/**
 * @brief	特殊出入口情報のセット
 * @param	gamedata	GAMEDATAへのポインタ
 * @param	loc			特殊出入口情報へのポインタ
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetSpecialLocation(GAMEDATA * gamedata, const LOCATION * loc);

//--------------------------------------------------------------
//--------------------------------------------------------------
extern const RAIL_LOCATION * GAMEDATA_GetRailLocation(const GAMEDATA * gamedata);
//--------------------------------------------------------------
//--------------------------------------------------------------
extern void GAMEDATA_SetRailLocation(GAMEDATA * gamedata, const RAIL_LOCATION * railLoc);

//--------------------------------------------------------------
/**
 * @brief   バッグカーソルのポインタ取得
 * @param   gamedata		GAMEDATAへのポインタ
 * @retval  バッグカーソルポインタ
 */
//--------------------------------------------------------------
extern BAG_CURSOR* GAMEDATA_GetBagCursor(const GAMEDATA * gamedata);
//--------------------------------------------------------------
/**
 * @brief   手持ちアイテムへのポインタ取得
 * @param   gamedata		GAMEDATAへのポインタ
 * @retval  手持ちアイテムへのポインタ
 */
//--------------------------------------------------------------
extern MYITEM_PTR GAMEDATA_GetMyItem(const GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   手持ちポケモンへのポインタ取得
 * @param   gamedata		GAMEDATAへのポインタ
 * @retval  手持ちアイテムへのポインタ
 */
//--------------------------------------------------------------
extern POKEPARTY * GAMEDATA_GetMyPokemon(const GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   MYSTATUSへのポインタ取得
 * @param   gamedata		GAMEDATAへのポインタ
 * @retval  MYSTATUSへのポインタ
 */
//--------------------------------------------------------------
extern MYSTATUS * GAMEDATA_GetMyStatus(GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief   MYSTATUSへのポインタ取得(プレイヤーID指定)
 * @param   gamedata		GAMEDATAへのポインタ
 * @param   player_id   プレイヤーID
 * @retval  MYSTATUSへのポインタ
 */
//--------------------------------------------------------------
extern MYSTATUS * GAMEDATA_GetMyStatusPlayer(GAMEDATA * gamedata, u32 player_id);

//--------------------------------------------------------------
/**
 * @brief FIELD_RAIL_LOADERへのポインタ取得
 * @param	gamedata	GAMEDATAへのポインタ
 * @return  FIELD_RAIL_LOADER レールデータローダーシステムへのポインタ
 */
//--------------------------------------------------------------
extern FIELD_RAIL_LOADER * GAMEDATA_GetFieldRailLoader(GAMEDATA * gamedata);

//--------------------------------------------------------------
/**
 * @brief 動作モデルへのポインタ取得
 * @param gamedata GAMEDATAへのポインタ
 * @retval MMDLSYSへのポインタ
 */
//--------------------------------------------------------------
extern MMDLSYS * GAMEDATA_GetMMdlSys(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief	季節の取得
 * @param   gamedata		GAMEDATAへのポインタ
 * @return	u8	季節ID（gamesystem/pm_season.h参照）
 */
//--------------------------------------------------------------
extern u8 GAMEDATA_GetSeasonID(const GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief	季節のセット
 * @param   gamedata	GAMEDATAへのポインタ
 * @param	season_id	季節ID（gamesystem/pm_season.h参照）
 *
 * 実際の機能として必要かどうかは疑問だがデバッグ用として追加
 */
//--------------------------------------------------------------
extern void GAMEDATA_SetSeasonID(GAMEDATA *gamedata, u8 season_id);

//--------------------------------------------------------------
/**
 * @brief	  フィールド下画面の取得
 * @param   gamedata		GAMEDATAへのポインタ
 * @return	subscreen_mode
 */
//--------------------------------------------------------------
extern u8 GAMEDATA_GetSubScreenMode(const GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief	  フィールド下画面の設定
 * @param   gamedata	GAMEDATAへのポインタ
 * @param	  subscreen_mode
 */
//--------------------------------------------------------------
extern void GAMEDATA_SetSubScreenMode(GAMEDATA *gamedata, u8 subscreen_mode);

//--------------------------------------------------------------
/**
 * @brief	  フィールド下画面タイプの取得
 * @param   gamedata		GAMEDATAへのポインタ
 * @return	subscreen_type
 */
//--------------------------------------------------------------
extern const u8 GAMEDATA_GetSubScreenType(const GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief	  フィールド下画面タイプの設定
 * @param   gamedata	GAMEDATAへのポインタ
 * @param	  subscreen_type
 */
//--------------------------------------------------------------
extern void GAMEDATA_SetSubScreenType(GAMEDATA *gamedata, const u8 subscreen_type);

//------------------------------------------------------------------
/**
 * @brief	  フレーム分割動作で動作する場合の許可フラグ設定
 * @param   gamedata	GAMEDATAへのポインタ
 * @param	  bEnable 分割する場合TRUE
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetFrameSpritEnable(GAMEDATA *gamedata,BOOL bEnable);

//------------------------------------------------------------------
/**
 * @brief	  フレーム分割動作で動作する場合のカウンタリセット
 * @param   gamedata	GAMEDATAへのポインタ
 */
//------------------------------------------------------------------
extern void GAMEDATA_ResetFrameSpritCount(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief	  フレーム分割動作で動作する場合のフレーム
 * @param   gamedata	GAMEDATAへのポインタ
 */
//------------------------------------------------------------------
extern u8 GAMEDATA_GetAndAddFrameSpritCount(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief	  フレーム分割動作で動作しているかどうか
 * @param   gamedata	GAMEDATAへのポインタ
 * @return  動作している場合TRUE
 */
//------------------------------------------------------------------
extern BOOL GAMEDATA_IsFrameSpritMode(GAMEDATA *gamedata);


//--------------------------------------------------------------
/**
 * @brief   ゲームデータが持つ情報を元にセーブを実行
 *
 * @param   gamedata		ゲームデータへのポインタ
 *
 * @retval  セーブ結果
 */
//--------------------------------------------------------------
extern SAVE_RESULT GAMEDATA_Save(GAMEDATA *gamedata);

//--------------------------------------------------------------
/**
 * @brief	EVENTWORKへのポインタ取得
 * @param	gamedata	GAMEDATAへのポインタ
 * @retval	EVENTWORKへのポインタ
 */
//--------------------------------------------------------------
extern EVENTWORK * GAMEDATA_GetEventWork(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief	  フィールドマップ歩数カウント取得
 * @param   gamedata	GAMEDATAへのポインタ
 * @return  int 歩数カウント
 */
//------------------------------------------------------------------
extern int GAMEDATA_GetFieldMapWalkCount(GAMEDATA *gamedata);

//------------------------------------------------------------------
/**
 * @brief	  フィールドマップ歩数カウントセット
 * @param   gamedata	GAMEDATAへのポインタ
 * @param   count セットするカウント
 * @return  nothing
 */
//------------------------------------------------------------------
extern void GAMEDATA_SetFieldMapWalkCount(GAMEDATA *gamedata,int count);

//============================================================================================
//	SAVE_CONTROL_WORKへのアクセス
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	  セーブコントロールワークを取得する
 * @param	  gamedata			GAMEDATAへのポインタ
 * @return	SAVE_CONTROL_WORK	のポインタ
 */
//------------------------------------------------------------------
extern SAVE_CONTROL_WORK * GAMEDATA_GetSaveControlWork(GAMEDATA * gamedata);



