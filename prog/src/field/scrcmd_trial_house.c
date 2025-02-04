//======================================================================
/**
 * @file  scrcmd_trial_house.c
 * @brief  スクリプトコマンド：トライアルハウス
 * @author  Saito
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"

#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "event_fieldmap_control.h"
#include "scrcmd_trial_house.h"
#include "trial_house.h"

#include "savedata/record.h"

//--------------------------------------------------------------
/**
 * トライアルハウス開始
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_Start( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );

  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  if ( *ptr == NULL ) *ptr = TRIAL_HOUSE_Start(gsys);
  else GF_ASSERT_MSG(0,"TRIAL_HOUSE_ALLOC_ERROR");

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * トライアルハウス終了
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_End( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);
  TRIAL_HOUSE_End(gsys, ptr);

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * トライアルハウス プレイモードのセット
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_SetPlayMode( VMHANDLE *core, void *wk )
{
  u16 play_mode;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  play_mode = SCRCMD_GetVMWorkValue( core, work );
  NOZOMU_Printf("TH playmode = %d\n",play_mode);
  TRIAL_HOUSE_SetPlayMode(*ptr, play_mode );
  return VMCMD_RESULT_CONTINUE;

}

//--------------------------------------------------------------
/**
 * トライアルハウス ポケモン選択
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_SelectPoke( VMHANDLE *core, void *wk )
{
  u16 *ret;
  u16 reg_type;
  u16 party_type;
  GMEVENT* event;

  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  reg_type = SCRCMD_GetVMWorkValue( core, work );
  party_type = SCRCMD_GetVMWorkValue( core, work );
  ret = SCRCMD_GetVMWork( core, work );
  
  event = TRIAL_HOUSE_CreatePokeSelEvt(gsys, *ptr, reg_type, party_type, ret );
  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;

}

//--------------------------------------------------------------
/**
 * トライアルハウス トレーナーセット
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_SetTrainer( VMHANDLE *core, void *wk )
{
  u16 *obj_id;
  u16 btl_count;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  btl_count = SCRCMD_GetVMWorkValue( core, work );
  obj_id = SCRCMD_GetVMWork( core, work );

  *obj_id = TRIAL_HOUSE_MakeTrainer( gsys, *ptr, btl_count );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * トライアルハウス 対戦前メッセージ表示
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_DispBeforeMsg( VMHANDLE *core, void *wk )
{
  GMEVENT* event;
  u16 tr_idx;
  u16 obj_id;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);
  tr_idx = SCRCMD_GetVMWorkValue( core, work );
  obj_id = SCRCMD_GetVMWorkValue( core, work );

  event = TRIAL_HOUSE_CreateBeforeMsgEvt(gsys, *ptr, tr_idx, obj_id);

  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;

}

//--------------------------------------------------------------
/**
 * トライアルハウス 戦闘開始
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_CallBattle( VMHANDLE *core, void *wk )
{
  GMEVENT* event;

  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  event = TRIAL_HOUSE_CreateBtlEvt(gsys, *ptr);

  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;

}

//--------------------------------------------------------------
/**
 * ダウンロードデータで遊ぶかどうかのフラグをセット
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_SetDLFlg( VMHANDLE *core, void *wk )
{
  u16 dl;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  dl = SCRCMD_GetVMWorkValue( core, work );

  TRIAL_HOUSE_SetDLFlg(*ptr, dl);

  return VMCMD_RESULT_CONTINUE;
}


//--------------------------------------------------------------
/**
 * トライアルハウス ランクセット
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_SetRank( VMHANDLE *core, void *wk )
{
  u16 rank;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );

  rank = SCRCMD_GetVMWorkValue( core, work );
  {
    RECORD *rec;
    rec = SaveData_GetRecord( GAMEDATA_GetSaveControlWork( gamedata ) );
    RECORD_SetThRank(rec, rank);
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * トライアルハウス ランク取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_GetRank( VMHANDLE *core, void *wk )
{
  u16 *rank;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );

  rank = SCRCMD_GetVMWork( core, work );

  {
    RECORD *rec;
    rec = SaveData_GetRecord( GAMEDATA_GetSaveControlWork( gamedata ) );
    *rank = RECORD_GetThRank(rec);
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * トライアルハウス ビーコンサーチ
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_SearchBeacon( VMHANDLE *core, void *wk )
{
  u16 *ret;
  GMEVENT* event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  ret = SCRCMD_GetVMWork( core, work );

  event = TRIAL_HOUSE_CreateBeaconSearchEvt( gsys, *ptr, ret );
  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * トライアルハウス 採点
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_CalcBtlResult( VMHANDLE *core, void *wk )
{
  u16 *rank;
  u16 *point;
  GMEVENT* event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  rank = SCRCMD_GetVMWork( core, work );
  point = SCRCMD_GetVMWork( core, work );
  //計算してランクを返すセーブデータも書く
  TRIAL_HOUSE_CalcBtlResult( gsys, *ptr, rank, point );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * トライアルハウス ダウンロードデータタイプ取得
 * @note シングル or ダブル or データ無し
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_GetDLDataType( VMHANDLE *core, void *wk )
{
  u16 *type;
  GMEVENT* event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  type = SCRCMD_GetVMWork( core, work );
  //データのタイプを返す
  *type = TRIAL_HOUSE_GetDLDataType( gsys, *ptr );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * トライアルハウス ダウンロードデータクリア
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_ClearDLData( VMHANDLE *core, void *wk )
{
  u16 clear_type;
  GMEVENT* event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  clear_type = SCRCMD_GetVMWorkValue( core, work );

  //データクリア
  event = TRIAL_HOUSE_InvalidDLData(gsys, *ptr, clear_type);
  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;

}

//--------------------------------------------------------------
/**
 * トライアルハウス ランク確認アプリコール
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_CallRankApp( VMHANDLE *core, void *wk )
{
  u16 dl;
  u16 me;
  GMEVENT* event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData( gsys );
  TRIAL_HOUSE_WORK_PTR *ptr = GAMEDATA_GetTrialHouseWorkPtr(gamedata);

  dl = SCRCMD_GetVMWorkValue( core, work );
  me = SCRCMD_GetVMWorkValue( core, work );

  event = TRIAL_HOUSE_CreateRankAppEvt( gsys, dl, me );
  SCRIPT_CallEvent( sc, event );

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * トライアルハウス ランクデータ状況取得
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdTH_GetRankDataState( VMHANDLE *core, void *wk )
{
  u16* state;
  GMEVENT* event;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  state = SCRCMD_GetVMWork( core, work );
  *state = TRIAL_HOUSE_GetRankDataState( gsys );

  return VMCMD_RESULT_CONTINUE;

}








