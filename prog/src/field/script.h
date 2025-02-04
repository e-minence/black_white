//======================================================================
/**
 * @file	script.h
 * @brief	スクリプト制御メイン部分
 * @author	Satoshi Nohara
 * @date	05.08.04
 *
 * 01.03.28	Sousuke Tamada
 * 03.04.15	Satoshi Nohara
 * 05.04.25 Hiroyuki Nakamura
 */
//======================================================================
#ifndef SCRIPT_H
#define SCRIPT_H

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fldmmdl.h"    //MMDL

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
//	debug
//--------------------------------------------------------------
#define SCRIPT_PL_NULL	//PL箇所無効化

#ifdef PM_DEBUG
#define DEBUG_SCRIPT		//定義でデバッグ機能有効
#endif
//--------------------------------------------------------------
//--------------------------------------------------------------
enum {
  SCRID_NULL = 0
};
//========================================================================
//	struct
//========================================================================
//--------------------------------------------------------------
//	スクリプト制御ワークへの不完全型ポインタ
//--------------------------------------------------------------
typedef struct _SCRIPTSYS SCRIPTSYS;

typedef	struct _TAG_SCRIPT_WORK SCRIPT_WORK;

//--------------------------------------------------------------
//	スクリプトイベント用ワーク
//--------------------------------------------------------------
typedef struct
{
	SCRIPT_WORK *sc; //スクリプト用ワーク
  SCRIPTSYS * scrsys; //制御ワーク
}EVENT_SCRIPT_WORK;

//--------------------------------------------------------------
/// スクリプト種別定義
//--------------------------------------------------------------
typedef enum {
  SCRIPT_TYPE_NORMAL = 0, ///<通常スクリプト（イベントスクリプト）
  SCRIPT_TYPE_FIELDINIT,  ///<フィールド初期化スクリプト（FIELD_INIT/FIELD_RECOVER)
  SCRIPT_TYPE_NO_FIELD,   ///<フィールド非依存スクリプト（ZONE_CHANGE/GAMECLEAR/INITなど）

  SCRIPT_TYPE_MAX
}SCRIPT_TYPE;

//======================================================================
//	proto
//======================================================================
//--------------------------------------------------------------
/**
 * スクリプトイベントセット
 * @param	scr_id		スクリプトID
 * @param	obj			話しかけ対象OBJのポインタ(ない時はNULL)
 * @retval	none
 */
//--------------------------------------------------------------
extern GMEVENT * SCRIPT_SetEventScript(
		GAMESYS_WORK *gsys, u16 scr_id, MMDL *obj,
		HEAPID temp_heapID );

//--------------------------------------------------------------
/**
 * スクリプトイベントコール
 * @param	event		GMEVENT_CONTROL型のポインタ
 * @param	scr_id		スクリプトID
 * @param	obj			話しかけ対象OBJのポインタ(ない時はNULL)
 * @param	ret_script_wk	スクリプト結果を代入するワークのポインタ
 * @retval	none
 */
//--------------------------------------------------------------
extern SCRIPT_WORK * SCRIPT_CallScript( GMEVENT *event,
	u16 scr_id, MMDL *obj, void *ret_script_wk, HEAPID temp_heapID );

//--------------------------------------------------------------
/**
 * スクリプトイベント切替
 * @param	event		GMEVENT_CONTROL型のポインタ
 * @param	scr_id		スクリプトID
 * @param	obj			話しかけ対象OBJのポインタ(ない時はNULL)
 * @retval	SCRIPT_WORK
 * 他のイベントからスクリプトへの切替を行う
 */
//--------------------------------------------------------------
extern SCRIPT_WORK * SCRIPT_ChangeScript( GMEVENT *event,
		u16 scr_id, MMDL *obj, HEAPID temp_heapID );

//--------------------------------------------------------------
/**
 * スクリプトイベントからスクリプトワークを取得
 * @param	GMEVENT *event
 * @retval  SCRIPT_WORK*
 */
//--------------------------------------------------------------
extern SCRIPT_WORK* SCRIPT_GetScriptWorkFromEvent( GMEVENT *event );

//--------------------------------------------------------------
/// スクリプトイベントからスクリプト制御ワークを取得
//--------------------------------------------------------------
extern SCRIPTSYS * SCRIPT_GetScriptSysFromEvent( GMEVENT *event );

//--------------------------------------------------------------
/// スクリプトワークからスクリプト制御ワークを取得
//--------------------------------------------------------------
extern SCRIPTSYS * SCRIPT_GetScriptSys( SCRIPT_WORK * sc );


//------------------------------------------------------------------
/**
 * @brief スクリプトIDのチェック
 * @param script_id
 * @return  BOOL  FALSEのとき、実行可能なスクリプト
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_IsValidScriptID( u16 script_id );

//======================================================================
//
//
//    特殊スクリプト関連
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * 特殊スクリプト呼び出し
 * @param gsys
 * @param sc
 * @param heapID
 * @param script_id
 */
//--------------------------------------------------------------
extern void SCRIPT_CallSpecialScript(
    GAMESYS_WORK *gsys, SCRIPT_WORK * sc, u16 script_id, SCRIPT_TYPE scr_type );

//------------------------------------------------------------------
/**
 * @brief	ゲーム開始 スクリプト初期設定の実行
 */
//------------------------------------------------------------------
extern void SCRIPT_CallGameStartInitScript( GAMESYS_WORK *gsys, HEAPID heapID );

#ifdef  PM_DEBUG
//------------------------------------------------------------------
/**
 * @brief	ゲーム開始 スクリプト初期設定の実行（デバッグスタート）
 */
//------------------------------------------------------------------
extern void SCRIPT_CallDebugGameStartInitScript( GAMESYS_WORK *gsys, HEAPID heapID );
#endif  //PM_DEBUG

//------------------------------------------------------------------
/**
 * @brief	ゲームクリア時処理のスクリプト実行
 */
//------------------------------------------------------------------
extern void SCRIPT_CallGameClearScript( GAMESYS_WORK *gsys, HEAPID heapID );

//------------------------------------------------------------------
/**
 * @brief 特殊スクリプト呼び出し：フィールド復帰（初回のみ）
 * @param gsys
 * @param heapID
 * @retval  TRUE    特殊スクリプトを実行した
 * @retval  FALSE   特殊スクリプトは存在しない
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_CallFieldInitScript( GAMESYS_WORK * gsys, HEAPID heapID );

//------------------------------------------------------------------
/**
 * @brief 特殊スクリプト呼び出し：ゾーン切り替え
 * @param gsys
 * @param heapID
 * @retval  TRUE    特殊スクリプトを実行した
 * @retval  FALSE   特殊スクリプトは存在しない
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_CallZoneChangeScript( GAMESYS_WORK * gsys, HEAPID heapID);

//------------------------------------------------------------------
/**
 * @brief 特殊スクリプト呼び出し：フィールド復帰
 * @param gsys
 * @param heapID
 * @retval  TRUE    特殊スクリプトを実行した
 * @retval  FALSE   特殊スクリプトは存在しない
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_CallFieldRecoverScript( GAMESYS_WORK * gsys, HEAPID heapID );

//------------------------------------------------------------------
/**
 * @brief	シーン起動スクリプトの検索
 * @param gsys
 * @param heapID
 * @retval  NULL      現在はシーン起動スクリプトが存在しない
 * @retval  GMEVENT   起動するべきスクリプトをキーに生成されたイベントへのポインタ
 */
//------------------------------------------------------------------
extern GMEVENT * SCRIPT_SearchSceneScript( GAMESYS_WORK * gsys, HEAPID heapID);


//--------------------------------------------------------------
/**
 * プログラムからスクリプトへの引数となるパラメータをセット
 * @param	sc SCRIPT_WORK
 * @param	prm0	パラメータ０（SCWK_PARAM0）
 * @param	prm1	パラメータ１（SCWK_PARAM1）
 * @param	prm2	パラメータ２（SCWK_PARAM2）
 * @param	prm3	パラメータ３（SCWK_PARAM3）
 * @retval none
 */
//--------------------------------------------------------------
extern void SCRIPT_SetScriptWorkParam( SCRIPT_WORK *sc, u16 prm0, u16 prm1, u16 prm2, u16 prm3 );

//--------------------------------------------------------------
/**
 * サブプロセス用ワークのポインターアドレスを取得
 * @param	sc SCRIPT_WORK
 * @retval none
 *
 * 使い終わったら必ずNULLクリアすること！
 */
//--------------------------------------------------------------
extern void** SCRIPT_SetSubProcWorkPointerAdrs( SCRIPT_WORK *sc );

//--------------------------------------------------------------
/**
 * サブプロセス用ワークのポインターを取得
 * @param	sc SCRIPT_WORK
 * @retval none
 *
 * 使い終わったら必ずNULLクリアすること！
 */
//--------------------------------------------------------------
extern void* SCRIPT_SetSubProcWorkPointer( SCRIPT_WORK *sc );

//--------------------------------------------------------------
/**
 * サブプロセス用ワーク領域の解放(ポインタがNULLでなければFree)
 * @param	sc SCRIPT_WORK
 * @retval none
 */
//--------------------------------------------------------------
extern void SCRIPT_FreeSubProcWorkPointer( SCRIPT_WORK *sc );


//======================================================================
//
//	トレーナー関連
//
//======================================================================
extern void * SCRIPT_GetTrainerEyeData( SCRIPT_WORK * sc, u32 tr_no );


#endif	/* SCRIPT_H */
