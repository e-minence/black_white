//======================================================================
/**
 * @file	scr_trainer.c
 * @bfief	スクリプトコマンド：トレーナー関連
 * @author	Satoshi Nohara
 * @date	06.06.26
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "scrcmd_trainer.h"

#include "event_trainer_eye.h"

#include "poke_tool/trainer_tool.h"		//TT_TrainerMessageGet

#include "event_battle.h"

#if 0
#include "battle/battle_common.h"	//↓インクルードに必要
#include "ev_trainer.h"				//EvTrainer
#include "field_encount.h"			//DebugFieldEncount
#include "field_battle.h"			//BattleParam_IsWinResult
#include "ev_pokemon.h"				//EvPoke_Add
#endif

//======================================================================
//	プロロトタイプ宣言
//======================================================================

//======================================================================
//	コマンド
//======================================================================
//--------------------------------------------------------------
/**
 * 視線：トレーナー移動呼び出し
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdEyeTrainerMoveSet( VMHANDLE *core, void *wk )
{
	int *range;
	int *scr_id;
	int *tr_id;
	int *tr_type;
	int *dir;
  GMEVENT **ev_eye_move;
	MMDL **mmdl;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetMemberWork( sc, ID_EVSCR_WK_FLDPARAM );
	u16 pos = VMGetWorkValue( core, work ); //視線データの0,1か？
  
	if( pos == 0 ){ //視線0
		range	= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_RANGE );
		dir		= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_DIR );
		scr_id	= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_SCR_ID );
		tr_id	= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_ID );
		tr_type	= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_TYPE );
		mmdl = SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_FLDOBJ );
		ev_eye_move = SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_TCB );
	}else{ //視線1
		range	= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR1_RANGE );
		dir		= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR1_DIR );
		scr_id	= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR1_SCR_ID );
		tr_id	= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR1_ID );
		tr_type	= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR1_TYPE );
		mmdl = SCRIPT_GetMemberWork( sc, ID_EVSCR_TR1_FLDOBJ );
		ev_eye_move = SCRIPT_GetMemberWork( sc, ID_EVSCR_TR1_TCB );
	}
  
  *ev_eye_move = EVENT_SetTrainerEyeMove( fparam->fieldMap,
      *mmdl, FIELDMAP_GetFieldPlayer(fparam->fieldMap),
      *dir, *range, 0, *tr_type, pos );
	return 0;
}

//--------------------------------------------------------------
/**
 * トレーナー移動終了待ち　0
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL EvWaitTrainer0Move( VMHANDLE *core, void *wk )
{
  GMEVENT_RESULT res;
  GMEVENT **ev_eye_move;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

  ev_eye_move = SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_TCB );
  res = GMEVENT_Run( *ev_eye_move );
  
  if( res == GMEVENT_RES_FINISH ){
    GMEVENT_Delete( *ev_eye_move );
    *ev_eye_move = NULL;
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * トレーナー移動終了待ち　1
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL EvWaitTrainer1Move( VMHANDLE *core, void *wk )
{
  GMEVENT_RESULT res;
  GMEVENT **ev_eye_move;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  
  ev_eye_move = SCRIPT_GetMemberWork( sc, ID_EVSCR_TR1_TCB );
  res = GMEVENT_Run( *ev_eye_move );
  
  if( res == GMEVENT_RES_FINISH ){
    GMEVENT_Delete( *ev_eye_move );
    *ev_eye_move = NULL;
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * トレーナー移動終了待ち　0&1
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL EvWaitTrainer01Move( VMHANDLE *core, void *wk )
{
  GMEVENT **ev_eye_move0;
  GMEVENT **ev_eye_move1;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GMEVENT_RESULT res0,res1;

  ev_eye_move0 = SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_TCB );
  
  if( *ev_eye_move0 != NULL ){
    res0 = GMEVENT_Run( *ev_eye_move0 );
  }else{
    res0 = GMEVENT_RES_FINISH;
  }
  
  ev_eye_move1 = SCRIPT_GetMemberWork( sc, ID_EVSCR_TR1_TCB );
  
  if( *ev_eye_move1 != NULL ){
    res1 = GMEVENT_Run( *ev_eye_move1 );
  }else{
    res1 = GMEVENT_RES_FINISH;
  }
  
  if( res0 == GMEVENT_RES_FINISH && res1 == GMEVENT_RES_FINISH ){
    if( *ev_eye_move0 != NULL ){
      GMEVENT_Delete( *ev_eye_move0 );
    }
    if( *ev_eye_move1 != NULL ){
      GMEVENT_Delete( *ev_eye_move1 );
    }
    *ev_eye_move0 = NULL;
    *ev_eye_move1 = NULL;
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * 視線：トレーナー移動　単体
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdEyeTrainerMoveSingle( VMHANDLE *core, void *wk )
{
  GMEVENT **ev_eye_move;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 pos = VMGetWorkValue( core, work ); //視線データの0,1か？
  
	if( pos == SCR_EYE_TR_0 ){
		ev_eye_move = SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_TCB );
	}else{
		ev_eye_move = SCRIPT_GetMemberWork( sc, ID_EVSCR_TR1_TCB );
	}
  
	//登録されていない時
  if( *ev_eye_move == NULL ){
    return 0;
  }
  
  if( pos == SCR_EYE_TR_0 ){
    VMCMD_SetWait( core, EvWaitTrainer0Move );
  }else{
    VMCMD_SetWait( core, EvWaitTrainer1Move );
  }
   
  return 1;
}

//--------------------------------------------------------------
/**
 * 視線：トレーナー移動　ダブル
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdEyeTrainerMoveDouble( VMHANDLE *core, void *wk )
{
  GMEVENT **ev_eye_move0;
  GMEVENT **ev_eye_move1;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  
  ev_eye_move0 = SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_TCB );
	ev_eye_move1 = SCRIPT_GetMemberWork( sc, ID_EVSCR_TR1_TCB );
  
	//登録されていない時
  if( *ev_eye_move0 == NULL && *ev_eye_move1 == NULL ){
    return 0;
  }
  
  VMCMD_SetWait( core, EvWaitTrainer01Move );
  return 1;
}

//--------------------------------------------------------------
/**
 * 視線：トレーナータイプ取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"0"
 * TR0_TYPEに固定！
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdEyeTrainerTypeGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 *type	= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_TYPE ); //TR0_TYPE固定
	u16 *ret_wk	= VMGetWork( core, work );
	*ret_wk = *type;
	return 0;
}

//--------------------------------------------------------------
/**
 * 視線：トレーナーID取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdEyeTrainerIdGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 *tr_id_0	= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR0_ID );
	u16 *tr_id_1	= SCRIPT_GetMemberWork( sc, ID_EVSCR_TR1_ID );
	u16 pos = VMGetWorkValue( core, work ); //視線データの0,1か？
	u16 *ret_wk		= VMGetWork( core, work );
	*ret_wk = (pos == SCR_EYE_TR_0) ? (*tr_id_0) : (*tr_id_1);
  KAGAYA_Printf( "視線トレーナーID取得 %d\n", *ret_wk );
	return 0;
}

//======================================================================
//	戦闘関連(実験中)
//======================================================================
//--------------------------------------------------------------
/**
 * トレーナーID取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerIdGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetMemberWork( sc, ID_EVSCR_WK_FLDPARAM );
	u16 *script_id = SCRIPT_GetMemberWork( sc, ID_EVSCR_SCRIPT_ID );
	u16 *ret_wk = VMGetWork( core, work );
  
	//スクリプトIDから、トレーナーIDを取得
	*ret_wk = SCRIPT_GetTrainerID_ByScriptID( *script_id );
	return 0;
}

//--------------------------------------------------------------
/**
 * トレーナー戦闘呼び出し
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerBattleSet( VMHANDLE *core, void *wk )
{
#if 0 //pl null
	u32 fight_type;
	FIELDSYS_WORK * fsys	= core->fsys;
	u16* script_id			= SCRIPT_GetMemberWork( sc, ID_EVSCR_SCRIPT_ID );
	VMCMD_RESULT* win_flag			= SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_FLAG );
	u16 tr_id_0				= VMGetWorkValue(core);
	u16 tr_id_1				= VMGetWorkValue(core);
	u16 partner_id;
  
	partner_id = 0;
	if (SysFlag_PairCheck(SaveData_GetEventWork(core->fsys->savedata)) == 1) {
		partner_id = SysWork_PairTrainerIDGet( SaveData_GetEventWork(fsys->savedata) );
	}

	EventCmd_TrainerBattle(core->event_work, tr_id_0, tr_id_1, partner_id, HEAPID_WORLD, win_flag);
	return 1;
#else
  u32 fight_type;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 *script_id = SCRIPT_GetMemberWork( sc, ID_EVSCR_SCRIPT_ID );
	VMCMD_RESULT *win_flag = SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_FLAG );
	u16 tr_id_0 = VMGetWorkValue( core, work );
	u16 tr_id_1 = VMGetWorkValue( core, work );
	u16 partner_id;
  
	partner_id = 0;
	
  {
    GAMESYS_WORK **gsys =
      SCRIPT_GetMemberWork( sc, ID_EVSCR_WK_GAMESYS_WORK );
    GMEVENT **event =
      SCRIPT_GetMemberWork( sc, ID_EVSCR_WK_GMEVENT );
    SCRIPT_FLDPARAM *fparam =
      SCRIPT_GetMemberWork( sc, ID_EVSCR_WK_FLDPARAM );
    GMEVENT *ev_battle =
      DEBUG_EVENT_Battle( *gsys, fparam->fieldMap );
    
    GMEVENT_CallEvent( *event, ev_battle );
  }
	return 0;
#endif
}

//--------------------------------------------------------------
/**
 * トレーナー対戦呼び出し（マルチバトル）
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerMultiBattleSet( VMHANDLE *core, void *wk )
{
#if 0
	FIELDSYS_WORK * fsys = core->fsys;
	VMCMD_RESULT* win_flag			= SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_FLAG );
	u16 partner_id			= VMGetWorkValue(core);
	u16 tr_id_0				= VMGetWorkValue(core);
	u16 tr_id_1				= VMGetWorkValue(core);

	//OS_Printf( "partner_id = %d\n", partner_id );
	EventCmd_TrainerBattle(core->event_work, tr_id_0, tr_id_1, partner_id, HEAPID_WORLD, win_flag);
	return 1;
#else
  return 0;
#endif
}

//--------------------------------------------------------------
/**
 * トレーナー会話の種類取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerTalkTypeGet( VMHANDLE *core, void *wk )
{
	u16 btl_type,lr,start_type,after_type,one_type;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 *script_id			= SCRIPT_GetMemberWork( sc, ID_EVSCR_SCRIPT_ID );
	u16 *wk1				= VMGetWork( core, work );
	u16 *wk2				= VMGetWork( core, work );
	u16 *wk3				= VMGetWork( core, work );

	//スクリプトIDから、トレーナーIDを取得、ダブルバトルタイプか取得
	btl_type = SCRIPT_CheckTrainer2vs2Type(
      SCRIPT_GetTrainerID_ByScriptID(*script_id) );
  
	//シングルかダブルかチェック
	if( btl_type == 0 ){
		//シングル
		start_type = TRMSG_FIGHT_START;
		after_type = TRMSG_FIGHT_AFTER;
		one_type   = 0;
	}else{
		//ダブル
		lr = SCRIPT_GetTrainerLR_ByScriptID( *script_id );
    
		if( lr == 0 ){
			//左
			start_type = TRMSG_FIGHT_START_1;
			after_type = TRMSG_FIGHT_AFTER_1;
			one_type = TRMSG_POKE_ONE_1;
		}else{
			//右
			start_type = TRMSG_FIGHT_START_2;
			after_type = TRMSG_FIGHT_AFTER_2;
			one_type = TRMSG_POKE_ONE_2;
		}
	}

	*wk1 = start_type;
	*wk2 = after_type;
	*wk3 = one_type;
	//OS_Printf( "start_type = %d\n", *wk1 );
	//OS_Printf( "after_type = %d\n", *wk2 );
	//OS_Printf( "one_type = %d\n", *wk3 );
	return 0;
}

//--------------------------------------------------------------
/**
 * 再戦トレーナー会話の種類取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdRevengeTrainerTalkTypeGet( VMHANDLE *core, void *wk )
{
	u16 btl_type,lr,start_type,after_type,one_type;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 *script_id			= SCRIPT_GetMemberWork( sc, ID_EVSCR_SCRIPT_ID );
	u16 *wk1				= VMGetWork( core, work );
	u16 *wk2				= VMGetWork( core, work );
	u16 *wk3				= VMGetWork( core, work );
  
	//スクリプトIDから、トレーナーIDを取得、ダブルバトルタイプか取得
	btl_type = SCRIPT_CheckTrainer2vs2Type(
      SCRIPT_GetTrainerID_ByScriptID(*script_id) );

	//シングルかダブルかチェック
	if( btl_type == 0 ){
		//シングル
		start_type = TRMSG_REVENGE_FIGHT_START;
		after_type = 0;
		one_type   = 0;
	}else{
		//ダブル
		lr = SCRIPT_GetTrainerLR_ByScriptID( *script_id );

		if( lr == 0 ){
			//左
			start_type = TRMSG_REVENGE_FIGHT_START_1;
			after_type = 0;
			one_type   = TRMSG_POKE_ONE_1;
		}else{
			//右
			start_type = TRMSG_REVENGE_FIGHT_START_2;
			after_type = 0;
			one_type   = TRMSG_POKE_ONE_2;
		}
	}

	*wk1 = start_type;
	*wk2 = after_type;
	*wk3 = one_type;
	//OS_Printf( "start_type = %d\n", *wk1 );
	//OS_Printf( "after_type = %d\n", *wk2 );
	//OS_Printf( "one_type = %d\n", *wk3 );
	return 0;
}

//--------------------------------------------------------------
/**
 * トレーナータイプ取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerTypeGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16 *script_id	= SCRIPT_GetMemberWork( sc, ID_EVSCR_SCRIPT_ID );
	u16 *ret_wk		= VMGetWork( core, work );

	//スクリプトIDから、トレーナーIDを取得、ダブルバトルタイプか取得
	*ret_wk = SCRIPT_CheckTrainer2vs2Type(
      SCRIPT_GetTrainerID_ByScriptID(*script_id) );
	return 0;
}

//--------------------------------------------------------------
/**
 * トレーナーBGM再生
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerBgmSet( VMHANDLE *core, void *wk )
{
#if 0
	u16 tr_id = VMGetWorkValue(core);
	Snd_EyeBgmSet( Snd_EyeBgmGet(tr_id) );
	return 1;
#else //wb kari
  u16 tr_id = VMGetWorkValue(core,wk);
  return 0;
#endif
}

//--------------------------------------------------------------
/**
 * トレーナー敗北
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerLose( VMHANDLE *core, void *wk )
{
#if 0
	EventCmd_NormalLose( core->event_work );
	return 1;
#else //wb kari
  return 0;
#endif
}

//--------------------------------------------------------------
/**
 * トレーナー敗北チェック
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTrainerLoseCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	VMCMD_RESULT * win_flag	= SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_FLAG );
	u16 *ret_wk		= VMGetWork( core, work );
#if 0 //pl null
	*ret_wk = BattleParam_IsWinResult(*win_flag);
	OS_Printf( "*ret_wk = %d\n", *ret_wk );
	return 1;
#else //wb kari
  *ret_wk = 1; //仮で勝利固定
  return 0;
#endif
}

//--------------------------------------------------------------
/**
 * 隠しポケモン戦闘　再戦可不可チェック
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdSeacretPokeRetryCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	VMCMD_RESULT* win_flag	= SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_FLAG );
	u16* ret_wk		= VMGetWork( core, work );
#if 0
	*ret_wk = BattleParam_IsSeacretPokeRetry(*win_flag);
	return 1;
#else //wb kari
  *ret_wk = 0;
  return 0;
#endif
}

//--------------------------------------------------------------
/**
 * 配布ポケモン戦闘　再戦可不可チェック
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdHaifuPokeRetryCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	VMCMD_RESULT* win_flag	= SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_FLAG );
	u16* ret_wk		= VMGetWork( core, work );
#if 0
	*ret_wk = BattleParam_IsHaifuPokeRetry(*win_flag);
	return 1;
#else //wb kari
  *ret_wk = 0;
  return 0;
#endif
}

//--------------------------------------------------------------
/**
 * 手持ちチェック 2vs2が可能か取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmd2vs2BattleCheck( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	u16* ret_wk	= VMGetWork( core, work );
  
#if 0
	*ret_wk = EvPoke_Enable2vs2Battle(SaveData_GetTemotiPokemon(core->fsys->savedata));
#else //wb kari
  *ret_wk = FALSE; //不可
#endif
	return 0;
}

//--------------------------------------------------------------
/**
 * デバック戦闘呼び出し(実験中)
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDebugBattleSet( VMHANDLE *core, void *wk )
{
#if 0
	FIELDSYS_WORK * fsys = core->fsys;
	VMCMD_RESULT * win_flag			= SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_FLAG );

	//イベントが切り替わってしまうので仮！！！
	//DebugFieldEncount( fsys );
	
	//イベントコールなので、スクリプトに復帰します。
	//EventCmd_TrainerBattle( core->event_work, 0 );
	//EventCmd_TrainerBattle( core->event_work, 0, FIGHT_TYPE_TRAINER, HEAPID_WORLD ,win_flag );
	EventCmd_TrainerBattle( core->event_work, 1, 0, 0, HEAPID_WORLD ,win_flag );

	return 1;
#else //wb kari
  return 0;
#endif
}

#if 0
//--------------------------------------------------------------
/**
 * デバックトレーナーフラグセット(10月末ROM用、あとで削除します！)
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDebugTrainerFlagSet( VMHANDLE *core, void *wk )
{
	FIELD_OBJ_PTR* fldobj;
	FIELDSYS_WORK * fsys = core->fsys;

	fldobj = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );

	SetEventFlagTrainer( FieldOBJ_FieldSysWorkGet(*fldobj), FieldOBJ_OBJIDGet(*fldobj) );
	return 0;
}
#endif

#if 0
//--------------------------------------------------------------
/**
 * デバックトレーナーフラグセット(10月末ROM用、あとで削除します！)
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"1"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDebugTrainerFlagOnJump( VMHANDLE *core, void *wk )
{
	int ret;
	s32	pos;
	FIELD_OBJ_PTR* fldobj;
	FIELDSYS_WORK* fsys = core->fsys;

	fldobj = SCRIPT_GetMemberWork( sc, ID_EVSCR_TARGET_OBJ );

	pos = (s32)VMGetU32(core);

	ret = CheckEventFlagTrainer( FieldOBJ_FieldSysWorkGet(*fldobj), FieldOBJ_OBJIDGet(*fldobj) );

	//フラグがONならジャンプ
	if( ret == TRUE ){
		VMJump( core, (VM_CODE *)(core->PC+pos) );	//JUMP
		//VM_End( core );
		return 1;			//10月ROMのぎりぎりの変更に対処するため仮！！！！！
	}

	return 0;
}
#endif

//--------------------------------------------------------------
/**
 * 戦闘結果を取得
 * @param	core		仮想マシン制御構造体へのポインタ
 * @return	"0"
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBattleResultGet( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
	VMCMD_RESULT *win_flag	= SCRIPT_GetMemberWork( sc, ID_EVSCR_WIN_FLAG );
	u16* ret_wk		= VMGetWork( core, work );
  
#if 0
	*ret_wk = *win_flag;
	OS_Printf( "*ret_wk = %d\n", *ret_wk );
	return 1;
#else //wb kari
	*ret_wk = 0;
	OS_Printf( "*ret_wk = %d\n", *ret_wk );
	return 0;
#endif
}
