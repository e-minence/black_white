//======================================================================
/**
 * @file	scrcmd_screeneffects.c
 *
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

#include "field_g3d_mapper.h" //FLDMAPPPER
#include "field_buildmodel.h"
#include "field_bmanime_tool.h"

#include "field/field_const.h"
#include "sound/pm_sndsys.h"      //PMSND_PlaySE, PMSND_CheckPlaySE

#include "event_fieldmap_control.h"   //for EVENT_Field〜
#include "system/main.h"  //for HEAPID_PROC

typedef enum {
  FADE_IN,
  FADE_OUT,
}FADE_IO;

typedef struct BRIGHT_CNT_WORK_tag
{
  GAMESYS_WORK* gsys;
  FIELDMAP_WORK* fieldmap;
	FIELD_FADE_TYPE fade_type;
  FIELD_FADE_WAIT_TYPE wait_type;
  FADE_IO   fade_io;
  int Seq;
}BRIGHT_CNT_WORK;

static BOOL EvWaitDispFade( VMHANDLE *core, void *wk );
static BOOL EvWaitMapFade( VMHANDLE *core, void *wk );
static void CreateBrightFadeTask( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, FADE_IO fade_io,
                             FIELD_FADE_TYPE type, FIELD_FADE_WAIT_TYPE wait );
static void BrightCntTcb( GFL_TCB* tcb, void* work );

#ifdef PM_DEBUG
extern BOOL MapFadeReqFlg;  //マップフェードリクエストフラグ  宣言元　script.c
#endif

extern void FIELDMAP_InitBG( FIELDMAP_WORK* fieldWork );

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
 * マップ遷移フェード終了チェック ウェイト部分
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval BOOL TRUE=終了
 */
//--------------------------------------------------------------
static BOOL EvWaitMapFade( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );
  if( FIELDMAP_CheckMapFadeReqFlg( fieldWork ) == TRUE ){
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * マップチェンジ用　ブラックイン
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFade_BlackIn( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );

  CreateBrightFadeTask( gsys, fieldWork, FADE_IN, FIELD_FADE_BLACK, FIELD_FADE_WAIT );
#ifdef PM_DEBUG
  GF_ASSERT_MSG(MapFadeReqFlg,"ERROR:NOT CALL MAP_FADE_OUT");
  MapFadeReqFlg = FALSE;
#endif
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * マップチェンジ用　ブラックアウト
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFade_BlackOut( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );

  CreateBrightFadeTask( gsys, fieldWork, FADE_OUT, FIELD_FADE_BLACK, FIELD_FADE_WAIT );
#ifdef PM_DEBUG
  GF_ASSERT_MSG(!MapFadeReqFlg,"ERROR:ALREADY CALLED MAP_FADE_OUT");
  MapFadeReqFlg = TRUE;
#endif  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * マップチェンジ用　ホワイトイン
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFade_WhiteIn( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );

  CreateBrightFadeTask( gsys, fieldWork, FADE_IN, FIELD_FADE_WHITE, FIELD_FADE_WAIT );
#ifdef PM_DEBUG
  GF_ASSERT_MSG(MapFadeReqFlg,"ERROR:NOT CALL MAP_FADE_OUT");
  MapFadeReqFlg = FALSE;
#endif  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * マップチェンジ用　ホワイトアウト
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFade_WhiteOut( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldWork = GAMESYSTEM_GetFieldMapWork( gsys );

  CreateBrightFadeTask( gsys, fieldWork, FADE_OUT, FIELD_FADE_WHITE, FIELD_FADE_WAIT );
#ifdef PM_DEBUG
  GF_ASSERT_MSG(!MapFadeReqFlg,"ERROR:ALREADY CALLED MAP_FADE_OUT");
  MapFadeReqFlg = TRUE;
#endif  
  return VMCMD_RESULT_CONTINUE;
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

//--------------------------------------------------------------
/**
 * マップ遷移用フェード終了チェック
 * @param  core    仮想マシン制御構造体へのポインタ
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdMapFadeCheck( VMHANDLE *core, void *wk )
{
  VMCMD_SetWait( core, EvWaitMapFade );
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
 * @todo  配置モデルを検索し、その位置にアニメを適用したい。
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
  pos.z -= 48*FX32_ONE;  // TEMP: C01ポケセンでテスト
  //PLAYER_WORK *player = GAMEDATA_GetMyPlayerWork( gdata );
  
  parent = SCRIPT_GetEvent( sc );
  call_event = EVENT_PcRecoveryAnime( gsys, parent, &pos, pokecount );
  SCRIPT_CallEvent( sc, call_event );
  return VMCMD_RESULT_SUSPEND;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDoorAnimeSetOpened( VMHANDLE * core, void *wk )
{
  u16 gx = SCRCMD_GetVMWorkValue( core, wk );
  u16 gz = SCRCMD_GetVMWorkValue( core, wk );
  VecFx32 pos;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMAPPER * mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );
  FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager( mapper );

  G3DMAPOBJST * entry = NULL;

  pos.x = GRID_TO_FX32( gx );
  pos.y = 0;
  pos.z = GRID_TO_FX32( gz );

  entry = BMANIME_DIRECT_SearchDoor( bmodel_man, &pos );
  G3DMAPOBJST_setAnime( bmodel_man, entry, ANM_INDEX_DOOR_OPEN, BMANM_REQ_REVERSE_START );
  G3DMAPOBJST_setAnime( bmodel_man, entry, ANM_INDEX_DOOR_OPEN, BMANM_REQ_STOP );

  return VMCMD_RESULT_CONTINUE;
}

//======================================================================
//
//
//    ドアアニメ
//
//
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
typedef u16 MEMKEY;

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  MEMKEY key;
  void * memory;
}MEM_RES;

//--------------------------------------------------------------
/**
 * @brief ドアアニメを確保するためのキー
 * @todo  キーで管理するメモリ機構を用意し、そちらに移行する
 */
//--------------------------------------------------------------
static MEM_RES memRes = { 0, NULL };

//--------------------------------------------------------------
//--------------------------------------------------------------
static void reserveKey(void * mem, MEMKEY key)
{
  GF_ASSERT_MSG(memRes.key == 0, "アニメ管理領域が確保できません!!\n");
  memRes.key = key;
  memRes.memory = mem;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void releaseKey(MEMKEY key)
{
  GF_ASSERT_MSG( memRes.key == key, "不正な管理番号です\n");
  memRes.key = 0;
  memRes.memory = NULL;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static void * getMemory(MEMKEY key)
{
  GF_ASSERT_MSG( memRes.key == key, "不正な管理番号です\n");
  return memRes.memory;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL EvWaitDoorAnime( VMHANDLE *core, void *wk );
enum { DOOR_ANIME_KEY_01 = 0x03fa };

//--------------------------------------------------------------
/**
 * @brief ドアアニメ制御：ハンドル生成処理
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDoorAnimeCreate( VMHANDLE * core, void *wk )
{
  u16 * ret_wk = SCRCMD_GetVMWork( core, wk );
  u16 gx = SCRCMD_GetVMWorkValue( core, wk );
  u16 gz = SCRCMD_GetVMWorkValue( core, wk );
  MEMKEY key;
  VecFx32 pos;
  SCRCMD_WORK *work = wk;
  GAMESYS_WORK *gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK * fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FLDMAPPER * mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );
  FIELD_BMODEL_MAN * bmodel_man = FLDMAPPER_GetBuildModelManager( mapper );

  BMANIME_CONTROL_WORK * ctrl;

  pos.x = GRID_TO_FX32( gx );
  pos.y = 0;
  pos.z = GRID_TO_FX32( gz );

  ctrl = BMANIME_CTRL_Create( bmodel_man, &pos );

  reserveKey( ctrl, DOOR_ANIME_KEY_01 );

  *ret_wk = DOOR_ANIME_KEY_01;

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief   ドアアニメ制御：ハンドル削除処理
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDoorAnimeDelete( VMHANDLE * core, void *wk )
{
  u16 anime_id = SCRCMD_GetVMWorkValue( core, wk );

  BMANIME_CONTROL_WORK * ctrl;

  ctrl = getMemory( anime_id );
  if ( ctrl )
  {
    BMANIME_CTRL_Delete( ctrl );
  }
  releaseKey( anime_id );

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ドアアニメ制御：アニメセット
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDoorAnimeSet( VMHANDLE * core, void *wk )
{
  u16 anime_id = SCRCMD_GetVMWorkValue( core, wk );
  u16 anime_type = SCRCMD_GetVMWorkValue( core, wk );
  BMANIME_CONTROL_WORK * ctrl;
  u16 seNo;

  ctrl = getMemory( anime_id );
  if (ctrl != NULL)
  {
    BMANIME_CTRL_SetAnime( ctrl, anime_type );
    if( BMANIME_CTRL_GetSENo( ctrl, anime_type, &seNo) )
    {
      PMSND_PlaySE( seNo );
    }
  }

  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 * @brief ドアアニメ制御：アニメウェイト
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param wk      SCRCMD_WORKへのポインタ
 * @retval VMCMD_RESULT
 *
 * @todo
 * オブジェクト管理機構をどうするか？考える
 * キーを持ったリスト構造でワーク管理する？
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdDoorAnimeWait( VMHANDLE * core, void *wk )
{
  u16 anime_id = SCRCMD_GetVMWorkValue( core, wk );
  VMCMD_SetWait( core, EvWaitDoorAnime );

  return VMCMD_RESULT_SUSPEND;
}

//--------------------------------------------------------------
/**
 * @brief   ドアアニメ終了待ち関数
 * @param  core    仮想マシン制御構造体へのポインタ
 * @param   wk      SCRCMD_WORKへのポインタ
 * @return  BOOL  TRUEのとき終了
 */
//--------------------------------------------------------------
static BOOL EvWaitDoorAnime( VMHANDLE *core, void *wk )
{
  BMANIME_CONTROL_WORK * ctrl;
  ctrl = getMemory( DOOR_ANIME_KEY_01 );  //手抜き
  if ( ctrl != NULL && BMANIME_CTRL_WaitAnime( ctrl ) == FALSE)
  {
    return FALSE;
  }

  if ( PMSND_CheckPlaySE() == FALSE )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

//------------------------------------------------------------------
/**
 * @brief	ブライトネスフェードタスク生成
 * @param	gsys		  GAMESYS_WORKへのポインタ
 * @param	fieldmap  フィールドマップワークへのポインタ
 * @param fade_io   イン　or アウト
 * @param	type		  フェードの種類指定
 * @param wait      フェード完了を待つかどうか
 * @return	none
 */
//------------------------------------------------------------------
static void CreateBrightFadeTask( GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldmap, FADE_IO fade_io,
                             FIELD_FADE_TYPE type, FIELD_FADE_WAIT_TYPE wait )
{
  BRIGHT_CNT_WORK *task_wk;
  GFL_TCBSYS*  tcbsys = FIELDMAP_GetFieldmapTCBSys( fieldmap );
  //ワーク作成
  task_wk = GFL_HEAP_AllocClearMemory( HEAPID_PROC, sizeof(BRIGHT_CNT_WORK) );
  // TCBを追加
  GFL_TCB_AddTask( tcbsys, BrightCntTcb, task_wk, 0 );
  
  //リクエストフラグ立てる
  FIELDMAP_SetMapFadeReqFlg( fieldmap, TRUE );

  task_wk->Seq  = 0;
  task_wk->gsys      = gsys;
  task_wk->fieldmap  = fieldmap;
	task_wk->fade_type = type;
  task_wk->wait_type = wait;
  task_wk->fade_io = fade_io;
}

//--------------------------------------------------------------
/**
 * ブライトネスフェードタスク
 * @param   tcb        タスクポインタ
 * @param   work      ワーク 
 * @retval  none
 */
//--------------------------------------------------------------
static void BrightCntTcb( GFL_TCB* tcb, void* work )
{
  BRIGHT_CNT_WORK *wk = work;
  switch(wk->Seq){
  case 0:
    {
      int mode;
      int start, end;
      if ( wk->fade_io == FADE_IN )
      {
        start = 16;
        end = 0;
        //フェードインのときはＢＧ初期化する
        FIELDMAP_InitBG(wk->fieldmap);
      }
      else
      {
        start = 0;
        end = 16;
      }

      if ( wk->fade_type ==  FIELD_FADE_BLACK)
      {
        mode = GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN | GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB;
      }
      else
      {
        mode = GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN | GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB;
      }
      GFL_FADE_SetMasterBrightReq(mode, start, end, 0);
    }
    wk->Seq++;
    break;
  case 1:
    {
      BOOL rc = FALSE;
      if( wk->wait_type == FIELD_FADE_NO_WAIT ) rc = TRUE;
		  if( GFL_FADE_CheckFade() == FALSE ) rc = TRUE;

      if (rc) { //フェード終了
        FIELDMAP_SetMapFadeReqFlg( wk->fieldmap, FALSE ); //リクエスト落す
        GFL_TCB_DeleteTask( tcb );
        GFL_HEAP_FreeMemory( work );
      }
    }
		break;
  }
}

