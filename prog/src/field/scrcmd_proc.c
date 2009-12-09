//======================================================================
/**
 * @file    scrcmd_proc.c
 * @brief   スクリプトコマンド用関数　各種アプリ呼び出し系(常駐)
 * @author  Miyuki Iwasawa 
 * @date    09.10.22
 *
 * フェード＆フィールド解放＆生成までをコモンのイベントシーケンス
 * (EVENT_FieldSubProc,EVENT_FieldSubProc_Callbackなど)に任せる場合、プロセス呼び出しコマンド自体を
 * 常駐に配置する必要がないので、scrcmd_proc_fld.c(フィールド常駐)に置いてください
 *
 * フェードやフィールドプロセスコントロールを自前で行うプロセス呼び出し法を取る必要がある場合
 * このソースにおいてください。特に必要がなければ、常駐メモリ節約のため、EVENT_FieldSubProc等
 * 用意されたプロセス呼び出しイベントを利用してください
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"

#include "scrcmd.h"
#include "scrcmd_work.h"

#include "msgdata.h"
#include "print/wordset.h"

#include "sound/pm_sndsys.h"
//#include "sound/wb_sound_data.sadl"

#include "fieldmap.h"
#include "field_sound.h"
#include "field_player.h"
#include "event_fieldmap_control.h"

#include "scrcmd_proc.h"
#include "../../../resource/fldmapdata/script/usescript.h"

#include "app/bag.h"

#include "field_comm\intrude_main.h"
#include "field/field_comm/intrude_work.h"
#include "field/monolith_main.h"
#include "field\field_comm\intrude_mission.h"

#include "demo/demo3d.h"  //Demo3DProcData etc.

////////////////////////////////////////////////////////////////
//プロトタイプ
////////////////////////////////////////////////////////////////
static BOOL callproc_WaitSubProcEnd( VMHANDLE *core, void *wk );


////////////////////////////////////////////////////////////////
//フィールド生成＆破棄
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------
/**
 * @brief フィールドマッププロセス生成
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFieldOpen( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  SCRIPT_CallEvent( sc, EVENT_FieldOpen( gsys ));
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief フィールドマッププロセス破棄
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFieldClose( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );

  SCRIPT_CallEvent( sc, EVENT_FieldClose(gsys, GAMESYSTEM_GetFieldMapWork(gsys)));
  
  return VMCMD_RESULT_SUSPEND;
}

////////////////////////////////////////////////////////////////
//サブプロセス呼び出し
////////////////////////////////////////////////////////////////

//--------------------------------------------------------------
/*
 *  @brief  サブプロセスコールとWait関数のセットアップ(メモリ解放バージョン)
 *
 *  @param  core
 *  @param  work
 *  @param  ov_id         サブプロセスのオーバーレイID
 *  @param  proc_data     サブプロセスの関数指定
 *  @param  proc_work     サブプロセス用ワーク。NULL指定可
 *  @param  callback      サブプロセス終了時に呼び出すコールバック関数ポインタ。NULL指定可
 *  @param  callback_work 汎用ワーク。NULL指定可
 *
 *  @note
 *  callbackにNULLを指定した場合、サブプロセス終了時に
 *  proc_workとcallback_workに対して GFL_HEAP_FreeMemory()を呼び出します。
 *  callbackを指定した場合は、callback中で明示的に解放をしてください
 */
//--------------------------------------------------------------
void EVFUNC_CallSubProc( VMHANDLE *core, SCRCMD_WORK *work,
    FSOverlayID ov_id, const GFL_PROC_DATA * proc_data, void* proc_work,
    void (*callback)(CALL_PROC_WORK* cpw), void* callback_work )
{
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  void** scr_subproc_work = SCRIPT_SetSubProcWorkPointerAdrs( sc );

  CALL_PROC_WORK* cpw = GFL_HEAP_AllocClearMemory(HEAPID_PROC,sizeof(CALL_PROC_WORK));

  cpw->proc_work = proc_work;
  cpw->cb_work = callback_work;
  cpw->cb_func = callback;
  GAMESYSTEM_CallProc( gsys, ov_id, proc_data, proc_work ); 
 
  *scr_subproc_work = cpw;
  VMCMD_SetWait( core, callproc_WaitSubProcEnd );
}

//--------------------------------------------------------------
/**
 * @brief サブプロセスの終了を待ちます(ワーク解放処理有り)
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  TRUE    終了
 * @retval  FALSE   待ち
 *
 * EVFUNC_CallSubProc()とセットのサブプロセスWait関数です
 */
//--------------------------------------------------------------
static BOOL callproc_WaitSubProcEnd( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  CALL_PROC_WORK* cpw = (CALL_PROC_WORK*)SCRIPT_SetSubProcWorkPointer( sc );

	if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
    return FALSE;
  }
  if(cpw->cb_func != NULL){
    (cpw->cb_func)(cpw);
  }else{
    if(cpw->proc_work != NULL){
      GFL_HEAP_FreeMemory(cpw->proc_work);
    }
    if(cpw->cb_work != NULL){
      GFL_HEAP_FreeMemory(cpw->cb_work);
    }
  }
  SCRIPT_FreeSubProcWorkPointer(sc);

  return TRUE;
}
//--------------------------------------------------------------
/**
 * @brief サブプロセスの終了を待ちます(ワーク解放処理無し)
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  TRUE    終了
 * @retval  FALSE   待ち
 *
 * ＊使用は玉田さんの許可制です。
 * 　プロセスコールは原則として、呼び出し〜戻り値取得までをOneコマンドで実現する設計にすること！
 *   WaitSubProcEndにコールバックを指定できます
 * ＊こちらで待った場合、明示的に EvCmdFreeSubProcWork( VMHANDLE *core, void *wk ) を使って解放してください
 */
//--------------------------------------------------------------
BOOL EVFUNC_WaitSubProcEndNonFree( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );

	if (GAMESYSTEM_IsProcExists(gsys) != GFL_PROC_MAIN_NULL){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * @brief 汎用サブプロセスワーク解放コマンド
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 *
 * ＊使用は玉田さんの許可制です。
 * 　プロセスコールは原則として、呼び出し〜戻り値取得までをOneコマンドで実現する設計にすること！
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdFreeSubProcWork( VMHANDLE *core, void *wk )
{
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( (SCRCMD_WORK*)wk );
  SCRIPT_FreeSubProcWorkPointer(sc);
  return VMCMD_RESULT_SUSPEND;
}


////////////////////////////////////////////////////////////////
//バッグプロセス呼び出し
////////////////////////////////////////////////////////////////
typedef struct _BAG_CALLBACK_WORK{
  u16* ret_mode;
  u16* ret_item;
}BAG_CALLBACK_WORK;

static void callback_BagProcFunc( CALL_PROC_WORK* cpw )
{
  BAG_PARAM* bp = (BAG_PARAM*)cpw->proc_work;
  BAG_CALLBACK_WORK* bcw = (BAG_CALLBACK_WORK*)cpw->cb_work;

  if(bp->next_proc == BAG_NEXTPROC_EXIT ){
    *bcw->ret_mode = SCR_PROC_RETMODE_EXIT;
  }else{
    *bcw->ret_mode = SCR_PROC_RETMODE_NORMAL;
  }
  *bcw->ret_item = bp->ret_item;

  GFL_HEAP_FreeMemory( cpw->cb_work );
  GFL_HEAP_FreeMemory( cpw->proc_work );
}

//--------------------------------------------------------------
/**
 * @brief   バッグプロセスを呼び出します
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 *
 * ＊サブプロセスワークの解放処理を行わないので、必ず明示的に EvCmdBagProcWorkFree()を呼び出すこと
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCallBagProc( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  BAG_PARAM* bp;
  BAG_CALLBACK_WORK* bcw;

  u16 mode = SCRCMD_GetVMWorkValue( core, work );
  u16* ret_mode = SCRCMD_GetVMWork( core, work );
  u16* ret_item = SCRCMD_GetVMWork( core, work );

  if( mode == SCR_BAG_MODE_SELL ){
    mode = BAG_MODE_SELL;
  }else{
    mode = BAG_MODE_FIELD;
  }
  bp = BAG_CreateParam( SCRCMD_WORK_GetGameData( work ), NULL, mode,  HEAPID_PROC );
  bcw = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(BAG_CALLBACK_WORK) );
  bcw->ret_mode = ret_mode;
  bcw->ret_item = ret_item;

  EVFUNC_CallSubProc( core, work,
    FS_OVERLAY_ID(bag), &ItemMenuProcData, bp, callback_BagProcFunc, bcw);
  
  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief   モノリス画面プロセスを呼び出します
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval  VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdCallMonolithProc( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  GAME_COMM_SYS_PTR game_comm= GAMESYSTEM_GetGameCommSysPtr(gsys);
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  MONOLITH_PARENT_WORK *parent;
  INTRUDE_COMM_SYS_PTR intcomm;
  const MISSION_CHOICE_LIST *list;
  int palace_area;
  
  intcomm = Intrude_Check_CommConnect(game_comm);
  GF_ASSERT(intcomm != NULL);
  
  parent = GFL_HEAP_AllocClearMemory(HEAPID_PROC, sizeof(MONOLITH_PARENT_WORK));

  if(intcomm != NULL 
      && MISSION_MissionList_CheckOcc(
      &intcomm->mission.list[Intrude_GetPalaceArea(intcomm)]) == TRUE){
    palace_area = Intrude_GetPalaceArea(intcomm);
    list = Intrude_GetChoiceList(intcomm, palace_area);
    parent->list = *list;
    parent->list_occ = TRUE;
  }
  else{
    OS_TPrintf("MonolithProc Call intcomm NULL!!\n");
    palace_area = 0;
    parent->list_occ = FALSE;
  }

  parent->gsys = gsys;
  parent->intcomm = intcomm;
  parent->palace_area = palace_area;
  
  EVFUNC_CallSubProc(core, work, FS_OVERLAY_ID(monolith), &MonolithProcData, parent, NULL, NULL);
  
  return VMCMD_RESULT_SUSPEND;
}

//======================================================================
//
//
//    3Dデモ関連
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief   デモ呼び出し
 * @param	core		仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDemoScene( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  DEMO3D_PARAM * param = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(DEMO3D_PARAM) );
  param->demo_id = SCRCMD_GetVMWorkValue( core, wk );

  EVFUNC_CallSubProc( core, work, FS_OVERLAY_ID(demo3d), &Demo3DProcData, param, NULL, NULL );

  return VMCMD_RESULT_SUSPEND;
}


