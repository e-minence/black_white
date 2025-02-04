//======================================================================
/**
 * @file	script_local.h
 * @brief	スクリプト制御：
 * @date  2009.09.13
 * @author  tamada GAMEFREAK inc.
 *
 * script.hから、スクリプトシステムやコマンド以外に公開する
 * 必然性がないものを移動した
 */
//======================================================================
#pragma once

#include "script.h"
#include "field/fieldmap_proc.h"
#include "field/field_msgbg.h"


#include "scrend_check_bit.h"

#include "field_saveanime.h"      //for FIELD_SAVEANIME
#include "event_movepoke.h"      //for EV_MOVEPOKE_WORK

//--------------------------------------------------------------
///	SCRIPT_FLDPARAM スクリプト制御ワーク　フィールドパラメータ
//--------------------------------------------------------------
typedef struct
{
	FLDMSGBG *msgBG;
  FIELDMAP_WORK *fieldMap;
}SCRIPT_FLDPARAM;

//--------------------------------------------------------------
// トレーナーデータ管理用
//--------------------------------------------------------------
enum{
  TRAINER_EYE_HIT0 = 0,
  TRAINER_EYE_HIT1,

  TRAINER_EYE_HITMAX,  ///<トレーナー視線データ最大数
};


//仮想マシンのナンバー
typedef enum{
	VMHANDLE_ID_MAIN = 0,		//メイン
	VMHANDLE_ID_SUB1,			//サブ
  VMHANDLE_ID_SUB2,
	VMHANDLE_ID_MAX,			//最大数
}VMHANDLE_ID;

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief スクリプト用グローバルワークの生成
 * @param main_heapID
 * @param gsys
 * @param event
 * @param	scr_id		スクリプトID
 * @param	ret_wk		スクリプト結果を代入するワークのポインタ
 * @param scr_type  スクリプトの種別指定
 * @return	SCRIPT_WORK			SCRIPT型のポインタ
 *
 * スクリプトコマンド全般からグローバルアクセス可能なデータを保持する
 */
//--------------------------------------------------------------
extern SCRIPT_WORK * SCRIPTWORK_Create( HEAPID main_heapID,
    GAMESYS_WORK * gsys, GMEVENT * event, u16 scr_id, void* ret_wk, SCRIPT_TYPE scr_type );
extern void SCRIPTWORK_Delete( SCRIPT_WORK * sc );

//======================================================================
//
//
//  スクリプト内部制御・コマンドから呼ばれる関数
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief スクリプトシステム：他のイベント呼び出し
 * @param sc
 * @param call_event  呼び出すイベントへのポインタ
 *
 * スクリプトコマンドから使用する。
 * 指定したイベントが終了した後は、現在のスクリプトに復帰し実行される。
 *
 */
//--------------------------------------------------------------
extern void SCRIPT_CallEvent( SCRIPT_WORK *sc, GMEVENT * call_event );

//--------------------------------------------------------------
/**
 * @brief スクリプトシステム：スクリプト終了→他のイベントへ切り替え
 * @param sc
 * @param next_event  スクリプト終了後に稼動するイベントへのポインタ
 *
 * スクリプトコマンドから使用する。
 * スクリプトが終了した後、指定したイベントに切り替わる。
 */
//--------------------------------------------------------------
extern void SCRIPT_EntryNextEvent( SCRIPT_WORK *sc, GMEVENT * next_event );

//--------------------------------------------------------------
/**
 * @brief スクリプトシステム：仮想マシンの追加
 * @param sc
 * @param zone_id 起動するゾーン指定ID
 * @param scr_id  起動するスクリプト指定ID
 *
 * @return VMHANDLE_ID   仮想マシンID
 *
 *  主にスクリプトコマンドから使用する。
 *  スクリプトを実行する仮想マシンを生成する。
 */
//--------------------------------------------------------------
extern  VMHANDLE_ID SCRIPT_AddVMachine( SCRIPT_WORK *sc, u16 zone_id, u16 scr_id );

//--------------------------------------------------------------
/**
 * @brief スクリプトシステム：仮想マシンの存在チェック
 * @param sc
 * @param vm_id   仮想マシン指定ID
 * @retval  TRUE    指定したIDの仮想マシンが存在する
 * @retval  FALSE   指定したIDの仮想マシンは存在しない
 */
//--------------------------------------------------------------
extern BOOL SCRIPT_GetVMExists( SCRIPT_WORK *sc, VMHANDLE_ID vm_id );

//--------------------------------------------------------------
/**
 * @brief 特殊スクリプトかどうかの判定
 * @param scr_type  スクリプト種別
 * @return  BOOL  TRUEのとき特殊スクリプト
 */
//--------------------------------------------------------------
extern BOOL SCRIPT_IsSpecialScriptType( SCRIPT_TYPE scr_type );

//======================================================================
//	スクリプト制御ワークのメンバーアクセス
//======================================================================
extern GMEVENT * SCRIPT_GetEvent( SCRIPT_WORK * sc );
extern GAMESYS_WORK * SCRIPT_GetGameSysWork( SCRIPT_WORK * sc );
extern SCRIPT_FLDPARAM * SCRIPT_GetFieldParam( SCRIPT_WORK * sc );
extern WORDSET * SCRIPT_GetWordSet( SCRIPT_WORK * sc );
extern STRBUF * SCRIPT_GetMsgBuffer( SCRIPT_WORK * sc );
extern STRBUF * SCRIPT_GetMsgTempBuffer( SCRIPT_WORK * sc );

extern void SCRIPT_SetFontHandle( SCRIPT_WORK * sc, GFL_FONT* fontHandle );
extern GFL_FONT* SCRIPT_GetFontHandle( const SCRIPT_WORK * sc );

extern u8 * SCRIPT_GetSoundSeFlag( SCRIPT_WORK * sc );

extern MMDL * SCRIPT_GetTargetObj( SCRIPT_WORK * sc );
extern void SCRIPT_SetTargetObj( SCRIPT_WORK * sc, MMDL * obj );

extern void * SCRIPT_GetScrCmdGlobal( SCRIPT_WORK * sc );

//======================================================================
//	
//======================================================================
//------------------------------------------------------------------
/**
 * @brief	イベントワークアドレスを取得
 *
 * @param	work_no		ワークナンバー
 *
 * @return	"ワークのアドレス"
 *
 * @li	work_no < 0x8000	通常のセーブワーク
 * @li	work_no >= 0x8000	スクリプト制御ワークの中に確保しているワーク
 */
//------------------------------------------------------------------
extern u16 * SCRIPT_GetEventWork( SCRIPT_WORK *sc, GAMEDATA *gdata, u16 work_no );

//------------------------------------------------------------------
/**
 * @brief	イベントワークの値を取得
 *
 * @param	work_no		ワークナンバー
 *
 * @return	"ワークの値"
 */
//------------------------------------------------------------------
extern u16 SCRIPT_GetEventWorkValue( SCRIPT_WORK *sc, GAMEDATA *gdata, u16 work_no );

//------------------------------------------------------------------
/**
 * @brief	イベントワークの値をセット
 *
 * @param	work_no		ワークナンバー
 * @param	value		セットする値
 *
 * @return	"TRUE=セット出来た、FALSE=セット出来なかった"
 */
//------------------------------------------------------------------
extern BOOL SCRIPT_SetEventWorkValue(
		SCRIPT_WORK* sc, u16 work_no, u16 value );

//--------------------------------------------------------------
/**
 * @brief 仮想マシンローカルな変数領域のバックアップ
 */
//--------------------------------------------------------------
extern void * SCRIPT_BackupUserWork( SCRIPT_WORK * sc );

//--------------------------------------------------------------
/**
 * @brief 仮想マシンローカルな変数領域の復帰
 */
//--------------------------------------------------------------
extern void SCRIPT_RestoreUserWork( SCRIPT_WORK * sc, void *backup_work );

//--------------------------------------------------------------
/**
 * @brief   開始時のスクリプトID取得
 * @param	sc		    SCRIPT_WORKのポインタ
 * @return  u16   スクリプトID
 */
//--------------------------------------------------------------
extern u16 SCRIPT_GetStartScriptID( const SCRIPT_WORK * sc );



//----------------------------------------------------------------------------
/**
 *	@brief  セーブBGアニメーションワークの管理
 */
//-----------------------------------------------------------------------------
extern void SCRIPT_SetSaveAnimeWork( SCRIPT_WORK * sc, FIELD_SAVEANIME * wk );
extern FIELD_SAVEANIME * SCRIPT_GetSaveAnimeWork( SCRIPT_WORK * sc ); 

//----------------------------------------------------------------------------
/**
 *	@brief  移動ポケモンアニメーションワークの管理
 */
//-----------------------------------------------------------------------------
extern void SCRIPT_SetMovePokeWork( SCRIPT_WORK * sc, EV_MOVEPOKE_WORK * wk );
extern EV_MOVEPOKE_WORK * SCRIPT_GetMovePokeWork( SCRIPT_WORK * sc ); 


//----------------------------------------------------------------------------
/**
 *	@brief  再戦トレーナーワークの管理
 */
//-----------------------------------------------------------------------------
extern void SCRIPT_SetReBattleTrainerData( SCRIPT_WORK * sc, void * wk );
extern void * SCRIPT_GetReBattleTrainerData( SCRIPT_WORK * sc ); 


