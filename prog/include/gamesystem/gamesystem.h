//============================================================================================
/**
 * @file	gamemsystem.h
 * @brief	ポケモンゲームシステム（ここが通常ゲームのトップレベル）
 * @author	tamada GAME FREAK Inc.
 * @date	08.10.21
 *
 */
//============================================================================================

#pragma once
#ifdef	__cplusplus
extern "C" {
#endif

#include "gamesystem/game_data.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_comm.h"
#include "gamesystem/iss_sys.h"
#include "field/fieldmap_proc.h"  //FIELDMAP_WORK

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
typedef struct _GAMESYS_WORK GAMESYS_WORK;

//------------------------------------------------------------------
/**
 * @brief
 */
//------------------------------------------------------------------
typedef struct _GMEVENT_CONTROL GMEVENT;


//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	ゲームメインプロセス定義データ
 */
//------------------------------------------------------------------
extern const GFL_PROC_DATA GameMainProcData;

//============================================================================================
//============================================================================================
//------------------------------------------------------------------
/**
 * @brief	ゲーム内プロセスの呼び出し（分岐）
 * @param	gsys		ゲーム制御ワークへのポインタ
 * @param	ov_id		オーバーレイ指定ID
 * @param	procdata	プロセスデータへのポインタ
 * @param	pwk			上位プロセスから渡すワーク
 */
//------------------------------------------------------------------
extern void GAMESYSTEM_SetNextProc(GAMESYS_WORK * gsys,
		FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk);

//------------------------------------------------------------------
/**
 * @brief	ゲーム内プロセスの呼び出し（コール）
 * @param	gsys		ゲーム制御ワークへのポインタ
 * @param	ov_id		オーバーレイ指定ID
 * @param	procdata	プロセスデータへのポインタ
 * @param	pwk			上位プロセスから渡すワーク
 */
//------------------------------------------------------------------
extern void GAMESYSTEM_CallProc(GAMESYS_WORK * gsys,
		FSOverlayID ov_id, const GFL_PROC_DATA *procdata, void * pwk);

//------------------------------------------------------------------
/**
 * @brief	フィールドマッププロセスの呼び出し
 * @param	gsys		ゲーム制御ワークへのポインタ
 */
//------------------------------------------------------------------
extern void GAMESYSTEM_CallFieldProc(GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	イベントの取得
 * @param	gsys			ゲーム制御ワークへのポインタ
 * @return	GMEVENT			イベントへのポインタ
 */
//------------------------------------------------------------------
extern GMEVENT * GAMESYSTEM_GetEvent(const GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	イベントのセット
 * @param	gsys	ゲーム制御ワークへのポインタ
 * @param	event	イベントへのポインタ
 */
//------------------------------------------------------------------
extern void GAMESYSTEM_SetEvent(GAMESYS_WORK * gsys, GMEVENT * event);

//------------------------------------------------------------------
/**
 * @brief	イベント起動中かどうかを返す
 * @param	gsys	ゲーム制御ワークへのポインタ
 * @retval	TRUE    起動中
 * @retval	FALSE   なし
 */
//------------------------------------------------------------------
extern BOOL GAMESYSTEM_IsEventExists(const GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	プロセス起動中かどうかを返す
 * @param	gsys	ゲーム制御ワークへのポインタ
 * @return	GFL_PROC_MAIN_STATUS
 */
//------------------------------------------------------------------
extern GFL_PROC_MAIN_STATUS GAMESYSTEM_IsProcExists(const GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	ゲームシステム用ヒープIDの取得
 * @param	gsys	ゲーム制御ワークへのポインタ
 */
//------------------------------------------------------------------
extern HEAPID GAMESYSTEM_GetHeapID(GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	フィールドマップワークへのポインタ取得
 * @param	gsys	ゲーム制御ワークへのポインタ
 * @return	void*	フィールドマップワークへのポインタ
 *
 * GAMESYS_WORKからFIELD_MAIN_WORKが取得できることには
 * やや問題がある（グローバル変数に近い）が、非常に頻繁に
 * セットでアクセスされるため、利便性を考慮して追加。
 */
//------------------------------------------------------------------
extern FIELDMAP_WORK * GAMESYSTEM_GetFieldMapWork(GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	フィールドマップワークのポインタセット
 * @param	gsys		ゲーム制御ワークへのポインタ
 * @param	fieldmap	フィールドマップワークへのポインタ
 *
 * フィールドマップを生成する部分以外からのアクセスは無いはず！
 */
//------------------------------------------------------------------
extern void GAMESYSTEM_SetFieldMapWork(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap);

//------------------------------------------------------------------
/**
 * @brief	フィールドマップワークのポインタチェック
 * @param	gsys		ゲーム制御ワークへのポインタ
 * @retval BOOL TRUE=フィールドマップワークがセットされている。
 */
//------------------------------------------------------------------
extern BOOL GAMESYSTEM_CheckFieldMapWork( const GAMESYS_WORK *gsys );

//==================================================================
/**
 * フィールドでの通信エラー画面表示リクエスト設定
 *
 * @param   fieldWork		FIELDMAP_WORK
 * @param   error_req		TRUE:エラー画面表示。　FALSE:リクエストをクリア
 */
//==================================================================
extern void GAMESYSTEM_SetFieldCommErrorReq( GAMESYS_WORK *gsys, BOOL error_req );

//==================================================================
/**
 * フィールドでの通信エラー画面表示リクエスト取得
 *
 * @param   fieldWork		FIELDMAP_WORK
 * @retval  TRUE:エラー画面表示。　FALSE:リクエストをクリア
 */
//==================================================================
extern BOOL GAMESYSTEM_GetFieldCommErrorReq( const GAMESYS_WORK *gsys );

//--------------------------------------------------------------
//	ISSシステム取得
//--------------------------------------------------------------
extern ISS_SYS * GAMESYSTEM_GetIssSystem( GAMESYS_WORK * gsys );

//------------------------------------------------------------------
/**
 * @brief	ゲームデータの取得
 * @param	gsys		ゲーム制御ワークへのポインタ
 * @return	GAMEDATA	ゲームデータへのポインタ
 */
//------------------------------------------------------------------
extern GAMEDATA * GAMESYSTEM_GetGameData(GAMESYS_WORK * gsys);

//------------------------------------------------------------------
/**
 * @brief	自分プレイヤーデータの取得
 * @param	gsys		ゲーム制御ワークへのポインタ
 * @return	PLAYER_WORK	自分プレイヤーデータへのポインタ
 *
 * 今は利便性のためGAMESYS_WORKから直接取得にしているが
 * 設計上はGAMESYS_WORK→GAMEDATA→PLAYER_WORKにするべきかも
 */
//------------------------------------------------------------------
extern PLAYER_WORK * GAMESYSTEM_GetMyPlayerWork(GAMESYS_WORK * gsys);

//==================================================================
/**
 * ゲーム通信管理ワークへのポインタを取得する
 * @param   gsys		ゲーム制御システムへのポインタ
 * @retval  GAME_COMM_SYS_PTR		ゲーム通信管理ワークへのポインタ
 */
//==================================================================
extern GAME_COMM_SYS_PTR GAMESYSTEM_GetGameCommSysPtr(GAMESYS_WORK *gsys);

//----------------------------------------------------------------------------
/**
 * @brief 通信不許可イベントフラグのセット
 * @param   gsys		ゲーム制御システムへのポインタ
 * @param   flag    TRUEのとき、通信不許可イベントの開始を意味する
 */
//----------------------------------------------------------------------------
extern void GAMESYSTEM_SetNetOffEventFlag( GAMESYS_WORK * gsys, BOOL flag );

//----------------------------------------------------------------------------
/**
 * @brief 通信不許可イベントフラグの取得
 * @param   gsys		ゲーム制御システムへのポインタ
 * @return  BOOL    TRUEの時、通信不許可イベント中
 */
//----------------------------------------------------------------------------
extern BOOL GAMESYSTEM_GetNetOffEventFlag( const GAMESYS_WORK * gsys );

//--------------------------------------------------------------
/**
 * @brief   常時通信フラグの取得
 * @param   gsys		ゲーム制御システムへのポインタ
 * @retval  TRUE:常時通信ON、　FALSE:常時通信OFF
 */
//--------------------------------------------------------------
extern BOOL GAMESYSTEM_GetAlwaysNetFlag(const GAMESYS_WORK * gsys);

//--------------------------------------------------------------
/**
 * @brief   常時通信フラグの設定
 * @param   gsys		ゲーム制御システムへのポインタ
 * @param	is_on			TRUEならば常時通信モードON FALSEならば常時通信モードOFF
 */
//--------------------------------------------------------------
extern void GAMESYSTEM_SetAlwaysNetFlag( GAMESYS_WORK * gsys, BOOL is_on );

//==================================================================
/**
 * 常時通信を起動してもよい状態かをチェックする
 *
 * @param   gsys		
 *
 * @retval  BOOL		TRUE:常時通信を起動してもよい
 * @retval  BOOL		FALSE:起動してはいけない
 */
//==================================================================
extern BOOL GAMESYSTEM_CommBootAlways_Check(GAMESYS_WORK *gsys);

//==================================================================
/**
 * 常時通信フラグをチェックした上で、常時通信を起動する
 *
 * @param   gsys		
 * @retval  BOOL    TRUE:通信起動した　FALSE:通信を起動しなかった
 */
//==================================================================
extern BOOL GAMESYSTEM_CommBootAlways( GAMESYS_WORK *gsys );

#ifdef PM_DEBUG
//extern GAMESYS_WORK * DEBUG_GameSysWorkPtrGet(void);
#endif

#ifdef	__cplusplus
} /* extern "C" */
#endif
