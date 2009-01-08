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
#include "savedata/myitem_savedata.h"
#include "poke_tool/pokeparty.h"

//============================================================================================
//============================================================================================
enum {
	PLAYER_MAX = 5,
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
 * @brief   ゲームデータが持つ情報を元にセーブを実行
 *
 * @param   gamedata		ゲームデータへのポインタ
 *
 * @retval  セーブ結果
 */
//--------------------------------------------------------------
extern SAVE_RESULT GAMEDATA_Save(GAMEDATA *gamedata);
