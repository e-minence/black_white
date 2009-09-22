//======================================================================
/**
 * @file	scrcmd_screeneffects.c
 * @brief	スクリプトコマンド：フィールド画面演出系
 * @date  2009.09.22
 * @author	tamada GAMEFREAK inc.
 *
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "script.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"
#include "script_local.h" //SCRIPT_GetEvent

#include "scrcmd_screeneffects.h"

#include "event_pc_recovery.h"  //EVENT_PcRecoveryAnime
#include "field_player.h"   //FIELD_PLAYER
#include "fieldmap.h"       //FIELDMAP_GetFieldPlayer

static BOOL EvWaitDispFade( VMHANDLE *core, void *wk );

//======================================================================
//  画面フェード
//======================================================================
//--------------------------------------------------------------
/**
 * 画面フェード
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDispFadeStart( VMHANDLE *core, void *wk )
{
  u16 mode = VMGetU16( core );
  u16 start_evy = VMGetU16( core );
  u16 end_evy = VMGetU16( core );
  u16 wait = VMGetU16( core );
  GFL_FADE_SetMasterBrightReq( mode, start_evy, end_evy, wait );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * 画面フェード終了チェック ウェイト部分
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval BOOL TRUE=終了
 */
//--------------------------------------------------------------
static BOOL EvWaitDispFade( VMHANDLE *core, void *wk )
{
  if( GFL_FADE_CheckFade() == TRUE ){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * 画面フェード終了チェック
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDispFadeCheck( VMHANDLE *core, void *wk )
{
  VMCMD_SetWait( core, EvWaitDispFade );
  return VMCMD_RESULT_SUSPEND;
}


//======================================================================
//
//    アニメーション演出
//
//======================================================================
//--------------------------------------------------------------
/**
 * ポケセン回復アニメ
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 * @todo  どんな表現をするとか、何も決まってないよ〜ということで空コマンドです
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdPokecenRecoverAnime( VMHANDLE * core, void *wk )
{
  VecFx32 pos;
  GMEVENT * call_event;
  GMEVENT * parent;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  u16 pokecount = SCRCMD_GetVMWorkValue( core, wk );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  FIELD_PLAYER_GetDirPos( fld_player, DIR_UP, &pos );
  //PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
  
  parent = SCRIPT_GetEvent( sc );
  call_event = EVENT_PcRecoveryAnime( gsys, parent, &pos, pokecount );
  SCRIPT_CallEvent( sc, call_event );
  return VMCMD_RESULT_SUSPEND;
}

