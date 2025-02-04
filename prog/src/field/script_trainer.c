//============================================================================================
/**
 * @file	script_trainer.c
 * @brief	スクリプト：トレーナー関連
 * @date  2010.03.15
 * @author  tamada
 */
//============================================================================================
#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_trainer.h"

#include "scrcmd_work.h"
#include "eventwork_def.h"


#include "trainer_eye_data.h"   //SCR_TRAINER_HITDATA

#include "battle/battle.h"




//============================================================================================
//
//	トレーナーフラグ関連
//	・スクリプトIDから、トレーナーIDを取得して、フラグチェック
//
//============================================================================================
//ID_TRAINER_2VS2_OFFSET, ID_TRAINER_OFFSET
#include "../../../resource/fldmapdata/script/scrid_offset/scr_offset_id.h"

#define GET_TRAINER_FLAG(id)	( TR_FLAG_START+(id) )

//--------------------------------------------------------------
/**
 * トレーナー視線情報を格納 事前にSCRIPT_SetEventScript()を起動しておく事
 * @param	event SCRIPT_SetEventScript()戻り値。
 * @param	tr_no		何番目にヒットしたトレーナーなのか
 * @param hitdata セットする視線情報データ
 */
//--------------------------------------------------------------
void SCRIPT_TRAINER_SetHitData( GMEVENT * event, u32 tr_no, const TRAINER_HITDATA * hitdata )
{
  SCRIPT_WORK *sc =  SCRIPT_GetScriptWorkFromEvent( event );
	SCR_TRAINER_HITDATA *eye = SCRIPT_GetTrainerHitData( sc, tr_no );
  eye->hitdata = *hitdata;
  eye->ev_eye_move = NULL;
}

//--------------------------------------------------------------
/**
 * スクリプトIDから、トレーナーIDを取得
 *
 * @param   scr_id		スクリプトID
 *
 * @retval  "トレーナーID = フラグインデックス"
 */
//--------------------------------------------------------------
u16 SCRIPT_GetTrainerID_ByScriptID( u16 scr_id )
{
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return (scr_id - ID_TRAINER_OFFSET);		//0オリジン
	}else{
		return (scr_id - ID_TRAINER_2VS2_OFFSET);		//0オリジン
	}
}

//--------------------------------------------------------------
/**
 * トレーナーIDから、スクリプトIDを取得
 *
 * @param   tr_id		トレーナーID
 *
 * @retval  "スクリプトID"
 */
//--------------------------------------------------------------
u16 SCRIPT_GetTrainerScriptID_ByTrainerID( u16 tr_id )
{
  return (tr_id + ID_TRAINER_OFFSET);		//0オリジン
}

//--------------------------------------------------------------
/**
 * トレーナーIDから、ダブルバトル用のスクリプトIDを取得
 *
 * @param   tr_id		トレーナーID
 *
 * @retval  "スクリプトID"
 */
//--------------------------------------------------------------
u16 SCRIPT_GetTrainerScriptID_By2vs2TrainerID( u16 tr_id )
{
  return (tr_id + ID_TRAINER_2VS2_OFFSET);		//0オリジン
}

//--------------------------------------------------------------
/**
 * スクリプトIDから、左右どちらのトレーナーか取得
 * @param   scr_id		スクリプトID
 * @retval  "0=左、1=右"
 */
//--------------------------------------------------------------
BOOL SCRIPT_GetTrainerLR_ByScriptID( u16 scr_id )
{
	if( scr_id < ID_TRAINER_2VS2_OFFSET ){
		return 0;
	}else{
		return 1;
	}
}

//--------------------------------------------------------------
/**
 * トレーナーIDから、ダブルバトルタイプか取得
 * @param   tr_id		トレーナーID
 * @retval  "0=シングルバトル、1=ダブルバトル"
 */
//--------------------------------------------------------------
BOOL SCRIPT_CheckTrainer2vs2Type( u16 tr_id )
{
  u32 rule = TT_TrainerDataParaGet( tr_id, ID_TD_fight_type );
  
  //OS_Printf( "check 2v2 TRID=%d, type = %d\n", tr_id, rule );
  return (rule == BTL_RULE_DOUBLE );
}

//--------------------------------------------------------------
/**
 * トレーナーIDから、トリプル/ローテバトルタイプか取得
 *
 * @param   tr_id		トレーナーID
 *
 * @retval  0 2vs2以下
 * @retval  1 3vs3
 */
//--------------------------------------------------------------
BOOL SCRIPT_CheckTrainer3vs3Type( u16 tr_id )
{
  u32 rule = TT_TrainerDataParaGet( tr_id, ID_TD_fight_type );
  
  switch( rule ){
  case BTL_RULE_TRIPLE:
  case BTL_RULE_ROTATION:
    return( 1 );
  }
  
  return( 0 );
}

//--------------------------------------------------------------
/**
 * トレーナーIDからバトルルールを取得
 * @param   tr_id		トレーナーID
 * @retval  BtlRule型 BTL_RULE_SINGLE他 
 */
//--------------------------------------------------------------
u8 SCRIPT_GetTrainerBtlRule( u16 tr_id )
{
  return TT_TrainerDataParaGet( tr_id, ID_TD_fight_type );
}

//------------------------------------------------------------------
/**
 * @brief	トレーナーイベントフラグをチェックする
 * @param	fsys		FLDCOMMON_WORKのポインタ
 * @param	tr_id		トレーナーID
 * @retval	"1 = フラグON"
 * @retval	"0 = フラグOFF"
 */
//------------------------------------------------------------------
BOOL SCRIPT_CheckEventFlagTrainer( EVENTWORK *ev, u16 tr_id )
{
  return EVENTWORK_CheckEventFlag(ev,GET_TRAINER_FLAG(tr_id) );
}

//------------------------------------------------------------------
/**
 * @brief	トレーナーイベントフラグをセットする
 *
 * @param	fsys		FLDCOMMON_WORKのポインタ
 * @param	tr_id		トレーナーID
 *
 * @return	none
 */
//------------------------------------------------------------------
void SCRIPT_SetEventFlagTrainer( EVENTWORK *ev, u16 tr_id )
{
  EVENTWORK_SetEventFlag( ev, GET_TRAINER_FLAG(tr_id) );
}

//------------------------------------------------------------------
/**
 * @brief	トレーナーイベントフラグをリセットする
 *
 * @param	fsys		FLDCOMMON_WORKのポインタ
 * @param	tr_id		トレーナーID
 *
 * @return	none
 */
//------------------------------------------------------------------
void SCRIPT_ResetEventFlagTrainer( EVENTWORK *ev, u16 tr_id )
{
	EVENTWORK_ResetEventFlag( ev, GET_TRAINER_FLAG(tr_id) );
}


