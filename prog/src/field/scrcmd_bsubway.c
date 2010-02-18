//======================================================================
/**
 * @file  scrcmd_bsybway.c
 * @brief  バトルサブウェイ　スクリプトコマンド専用ソース
 * @author  Miyuki Iwasawa
 * @date  2006.05.23
 *
 * 2007.05.24  Satoshi Nohara*
 *
 * @note plから移植 kagaya
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/vm_cmd.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_event.h"

#include "poke_tool/poke_regulation.h"
#include "savedata/save_tbl.h"
#include "../../../resource/fldmapdata/flagwork/flag_define.h" //SYS_FLAG_SPEXIT_REQUEST
#include "fieldmap.h"

#include "script.h"
#include "script_local.h"
#include "script_def.h"
#include "scrcmd.h"
#include "scrcmd_work.h"

#include "bsubway_scr.h"
#include "bsubway_scrwork.h"
#include "scrcmd_bsubway.h"

#include "event_bsubway.h"

#include "fldmmdl.h"

#include "field_gimmick_bsubway.h"

#include "../../../resource/fldmapdata/zonetable/zone_id.h"

//======================================================================
//  define
//======================================================================

//======================================================================
//  struct
//======================================================================

//======================================================================
//  proto
//======================================================================
static BOOL evCommTimingSync( VMHANDLE *core, void *wk );
static BOOL evCommEntryMenuPerent( VMHANDLE *core, void *wk );
static BOOL evCommEntryMenuChild( VMHANDLE *core, void *wk );
static BOOL evCommRecvData( VMHANDLE *core, void *wk );

static BOOL bsway_CheckEntryPokeNum(
    u16 num, GAMESYS_WORK *gsys, BOOL item_flag );
static BOOL bsway_CheckRegulation( int mode, GAMESYS_WORK *gsys );

static const FLDEFF_BTRAIN_TYPE data_TrainModeType[BSWAY_MODE_MAX];
static const VecFx32 data_TrainPosTbl[BTRAIN_POS_MAX];


//======================================================================
//  バトルサブウェイ　スクリプト関連
//======================================================================
//--------------------------------------------------------------
/**
 *  @brief  バトルサブウェイ用ワーク作成と初期化
 *  @param  init  初期化モード指定
 *          BSWAY_PLAY_NEW:new game
 *          BSWAY_PLAY_CONTINUE:new game
 *  @param  mode  プレイモード指定:BSWAY_MODE_〜
 *  @return 0
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBSubwayWorkCreate( VMHANDLE* core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  u16 init = VMGetU16(core);
  u16 mode = SCRCMD_GetVMWorkValue( core, work );
  BSUBWAY_SCRWORK *bsw_scr = BSUBWAY_SCRWORK_CreateWork( gsys, init, mode );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 *  @brief  バトルサブウェイ制御用ワークポインタ初期化
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBSubwayWorkClear( VMHANDLE* core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  BSUBWAY_SCRWORK_ClearWork( gsys );
  
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 *  @brief  バトルサブウェイ用ワーク解放
 *  @return 0
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBSubwayWorkRelease( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  BSUBWAY_SCRWORK *bsw_scr;
  
  bsw_scr = GAMEDATA_GetBSubwayScrWork( gdata );
  BSUBWAY_SCRWORK_ReleaseWork( gsys, bsw_scr );
  return VMCMD_RESULT_CONTINUE;
}

//--------------------------------------------------------------
/**
 *  @brief  バトルサブウェイ用コマンド群呼び出しインターフェース
 *  @param  com_id    u16:コマンドID
 *  @param  retwk_id  u16:返り値を格納するワークのID
 */
//--------------------------------------------------------------
VMCMD_RESULT EvCmdBSubwayTool( VMHANDLE *core, void *wk )
{
  void **buf;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( gdata );
  EVENTWORK *event = GAMEDATA_GetEventWork( gdata );
  BSUBWAY_SCRWORK *bsw_scr = GAMEDATA_GetBSubwayScrWork( gdata );
  BSUBWAY_PLAYDATA *playData =
    SaveControl_DataPtrGet( save, GMDATA_ID_BSUBWAY_PLAYDATA );
  u32 play_mode = (u32)BSUBWAY_PLAYDATA_GetData(
        playData, BSWAY_PLAYDATA_ID_playmode, NULL );
  BSUBWAY_SCOREDATA *scoreData = SaveControl_DataPtrGet(
      save, GMDATA_ID_BSUBWAY_SCOREDATA );
  FIELDMAP_WORK *fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  u16 com_id = VMGetU16( core );
  u16 param0 = SCRCMD_GetVMWorkValue( core, work );
  u16 param1 = SCRCMD_GetVMWorkValue( core, work );
  u16 retwk_id = VMGetU16( core );
  u16 *ret_wk = SCRIPT_GetEventWork( sc, gdata, retwk_id );
   
#ifdef DEBUG_ONLY_FOR_kagaya  
  if( com_id < BSWTOOL_END_NO )
  {
    KAGAYA_Printf( "BSUBWAY_TOOL() cmd type BSWTOOL : cmd No %d\n",
        com_id );
  }
  else if(
      com_id >= BSWTOOL_WIFI_START_NO && com_id < BSWTOOL_WIFI_END_NO )
  {
    KAGAYA_Printf( "BSUBWAY_TOOL() cmd type BSWTOOL_WIFI : cmd No %d\n",
        com_id - BSWTOOL_WIFI_START_NO );
  }
  else if(
      com_id >= BSWSUB_START_NO && com_id < BSWSUB_END_NO )
  {
    KAGAYA_Printf( "BSUBWAY_TOOL() cmd type BSWSUB : cmd No %d\n",
        com_id - BSWSUB_START_NO );
  }
  else if(
      com_id >= BSWSUB_COMM_START_NO && com_id < BSWSUB_COMM_END_NO )
  {
    KAGAYA_Printf( "BSUBWAY_TOOL() cmd type BSWSUB_COMM : cmd No %d\n",
        com_id - BSWSUB_COMM_START_NO );
  }
  else
  {
    GF_ASSERT( 0 );
  }
#endif
  
  if( bsw_scr == NULL && //バグチェック　ワーク依存コマンド
      ((com_id >= BSWSUB_START_NO && com_id < BSWSUB_END_NO) ||
      (com_id >= BSWSUB_COMM_START_NO && com_id < BSWSUB_COMM_END_NO)) ){
    GF_ASSERT( 0 );
    return( VMCMD_RESULT_CONTINUE );
  }
  
  switch( com_id ){
  //ゾーンID別プレイモード取得
  case BSWTOOL_GET_ZONE_PLAY_MODE:
    {
      u16 zone_id = FIELDMAP_GetZoneID( fieldmap );
      switch( zone_id ){
      case ZONE_ID_C04R0102:
        *ret_wk = BSWAY_MODE_SINGLE;
        break;
      case ZONE_ID_C04R0103:
        *ret_wk = BSWAY_MODE_S_SINGLE;
        break;
      case ZONE_ID_C04R0104:
        *ret_wk = BSWAY_MODE_DOUBLE;
        break;
      case ZONE_ID_C04R0105:
        *ret_wk = BSWAY_MODE_S_DOUBLE;
        break;
      case ZONE_ID_C04R0106:
        *ret_wk = BSWAY_MODE_MULTI;
        break;
      case ZONE_ID_C04R0107:
        *ret_wk = BSWAY_MODE_S_MULTI;
        break;
      case ZONE_ID_C04R0108:
        *ret_wk = BSWAY_MODE_WIFI;
        break;
      default:
        GF_ASSERT( 0 );
        *ret_wk = BSWAY_MODE_SINGLE;
      }
    }
    break;
  //リセット
  case BSWTOOL_SYSTEM_RESET:
    OS_ResetSystem( 0 );
    break;
  //プレイデータクリア
  case BSWTOOL_CLEAR_PLAY_DATA:
    BSUBWAY_PLAYDATA_Init( playData );
    break;
  //手持ちポケモン数チェック
  case BSWTOOL_CHK_ENTRY_POKE_NUM:
    if( param0 == 0 ){
      GF_ASSERT( bsw_scr != NULL );
      *ret_wk = bsway_CheckEntryPokeNum( bsw_scr->member_num, gsys, 1 );
    }else{
      *ret_wk = bsway_CheckEntryPokeNum( param0, gsys, 1 );
    }
    break;
  //セーブされているか
  case BSWTOOL_IS_SAVE_DATA_ENABLE:
    *ret_wk = BSUBWAY_PLAYDATA_GetSaveFlag( playData );
    break;
  //復帰位置設定
  case BSWTOOL_PUSH_NOW_LOCATION:
    {
      VecFx32 pos;
      LOCATION loc;
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      
      FIELD_PLAYER_GetPos( fld_player, &pos );
      
      {
        static const u16 DIR_ROT[DIR_MAX4] = {0,0x8000,0x4000,0xC000};
        u16 dir = FIELD_PLAYER_GetDir( fld_player );
        dir = DIR_ROT[dir];
        LOCATION_SetDirect( &loc, FIELDMAP_GetZoneID(fieldmap),
            dir, pos.x, pos.y, pos.z );
      }
      
      GAMEDATA_SetSpecialLocation( gdata, &loc );
      EVENTWORK_SetEventFlag( event, SYS_FLAG_SPEXIT_REQUEST );
      
      KAGAYA_Printf("復帰位置　方向=%d\n",FIELD_PLAYER_GetDir(fld_player));
    }
    break;
  //復帰位置無効
  case BSWTOOL_POP_NOW_LOCATION:
    EVENTWORK_ResetEventFlag( event, SYS_FLAG_SPEXIT_REQUEST );
    break;
  //エラー時のスコアセット
  case BSWTOOL_SET_NG_SCORE:
    *ret_wk = BSUBWAY_SCRWORK_SetNGScore( gsys );
    break;
  //連勝数取得
  case BSWTOOL_GET_RENSHOU_CNT:
    *ret_wk = BSUBWAY_SCOREDATA_GetRenshou( scoreData, param0 );
    if( (s16)(*ret_wk) < 0 ){ 
      *ret_wk = 0;
    }
    break;
  //自機OBJコード取得
  case BSWTOOL_GET_MINE_OBJ:
    {
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      int sex = FIELD_PLAYER_GetSex( fld_player );
      *ret_wk = FIELD_PLAYER_GetDrawFormToOBJCode(
          sex, PLAYER_DRAW_FORM_NORMAL );
    }
    break;
  //自機非表示
  case BSWTOOL_PLAYER_VANISH:
    {
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      MMDL *mmdl = FIELD_PLAYER_GetMMdl( fld_player );
      MMDL_SetStatusBitVanish( mmdl, param0 );
    }
    break;
  //レギュレーションチェック
  case BSWTOOL_CHK_REGULATION:
    *ret_wk = 0; //ok
    if( bsway_CheckRegulation(param0,gsys) == FALSE ){
      *ret_wk = 1;
    }
    break;
  //現在ラウンド数取得
  case BSWTOOL_GET_NOW_ROUND:
    *ret_wk = BSUBWAY_PLAYDATA_GetRoundNo( playData ) + 1;
    break;
  //ラウンド数増加
  case BSWTOOL_INC_ROUND:
    BSUBWAY_PLAYDATA_IncRoundNo( playData );
    BSUBWAY_SCOREDATA_IncRenshou( scoreData, play_mode );
    *ret_wk = BSUBWAY_SCOREDATA_GetRenshou( scoreData, play_mode );
    break;
  //次のラウンド数取得
  case BSWTOOL_GET_NEXT_ROUND:
    *ret_wk = BSUBWAY_PLAYDATA_GetRoundNo( bsw_scr->playData );
    (*ret_wk) += 1; //0 org
    break;
  //ボスクリア済みフラグ取得
  case BSWTOOL_GET_BOSS_CLEAR_FLAG:
    *ret_wk = BSUBWAY_SCOREDATA_SetFlag( scoreData,
        BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_SINGLE + param0,
        BSWAY_SETMODE_get );
    break;
  //スーパーボスクリア済みフラグ取得
  case BSWTOOL_GET_S_BOSS_CLEAR_FLAG:
    *ret_wk = BSUBWAY_SCOREDATA_SetFlag( scoreData,
        BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_SINGLE + param0,
        BSWAY_SETMODE_get );
    break;
  //ボスクリア済みフラグセット
  case BSWTOOL_SET_BOSS_CLEAR_FLAG:
    *ret_wk = BSUBWAY_SCOREDATA_SetFlag( scoreData,
        BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_SINGLE + param0,
        BSWAY_SETMODE_set );
    break;
  //スーパーボスクリア済みフラグ取得
  case BSWTOOL_SET_S_BOSS_CLEAR_FLAG:
    *ret_wk = BSUBWAY_SCOREDATA_SetFlag( scoreData,
        BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_SINGLE + param0,
        BSWAY_SETMODE_set );
    break;
  //サポート遭遇フラグ取得
  case BSWTOOL_GET_SUPPORT_ENCOUNT_END:
    *ret_wk = BSUBWAY_SCOREDATA_SetFlag( scoreData,
        BSWAY_SCOREDATA_FLAG_SUPPORT_ENCOUNT_END,
        BSWAY_SETMODE_get );
    break;
  //サポート遭遇フラグセット
  case BSWTOOL_SET_SUPPORT_ENCOUNT_END:
    BSUBWAY_SCOREDATA_SetFlag( scoreData,
        BSWAY_SCOREDATA_FLAG_SUPPORT_ENCOUNT_END,
        BSWAY_SETMODE_set );
    break;
  //列車配置 param0 = 種類(modeから変換) param1 = 座標種類
  case BSWTOOL_SET_TRAIN:
    {
      const VecFx32 *pos;
      FLDEFF_BTRAIN_TYPE type;
      type = data_TrainModeType[param0];
      pos = &data_TrainPosTbl[param1];
      BSUBWAY_GIMMICK_SetTrain( fieldmap, type, pos );
    }
    break;
  //列車アニメ
  case BSWTOOL_SET_TRAIN_ANM:
    {
      FLDEFF_TASK *task = BSUBWAY_GIMMICK_GetTrainTask( fieldmap );
      FLDEFF_BTRAIN_SetAnime( task, param0 );
    }
    break;
  //列車非表示
  case BSWTOOL_SET_TRAIN_VANISH:
    {
      FLDEFF_TASK *task = BSUBWAY_GIMMICK_GetTrainTask( fieldmap );
      FLDEFF_BTRAIN_SetVanishFlag( task, param0 );
    }
    break;
  //BSUBWAY_PLAYDATAからプレイモード取得
  case BSWTOOL_GET_DATA_PLAY_MODE:
    {
      *ret_wk = (u16)BSUBWAY_PLAYDATA_GetData(
        playData, BSWAY_PLAYDATA_ID_playmode, NULL );
    }
    break;
  //指定モードがマルチかどうか
  case BSWTOOL_CHK_MODE_MULTI:
    *ret_wk = FALSE;
    switch( param0 ){
    case BSWAY_MODE_MULTI:
    case BSWAY_MODE_COMM_MULTI:
    case BSWAY_MODE_S_MULTI:
    case BSWAY_MODE_S_COMM_MULTI:
      *ret_wk = TRUE;
      break;
    }
    break;
  //指定OBJの高さ取得をOFF
  case BSWTOOL_OBJ_HEIGHT_OFF:
    {
      u16 id = param0;
      fx32 height = GRID_SIZE_FX32( param1 );
      MMDLSYS *mmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
      MMDL *mmdl = MMDLSYS_SearchOBJID( mmdlsys, id );
      GF_ASSERT( mmdl != NULL );
      if( mmdl != NULL ){
        VecFx32 pos;
        MMDL_GetVectorPos( mmdl, &pos );
        pos.y = height;
        MMDL_InitPosition( mmdl, &pos, MMDL_GetDirDisp(mmdl) );
        MMDL_SetStatusBitHeightGetOFF( mmdl, TRUE );
      }
    }
    break;
  //指定OBJの高さ取得をON
  case BSWTOOL_OBJ_HEIGHT_ON:
    {
      u16 id = param0;
      MMDLSYS *mmdlsys = FIELDMAP_GetMMdlSys( fieldmap );
      MMDL *mmdl = MMDLSYS_SearchOBJID( mmdlsys, id );
      GF_ASSERT( mmdl != NULL );
      if( mmdl != NULL ){
        MMDL_SetStatusBitHeightGetOFF( mmdl, FALSE );
      }
    }
    break;
  //----TOOL Wifi関連
  //Wifiアップロードフラグをセット
  case BSWTOOL_SET_WIFI_UPLOAD_FLAG:
    if( param0 == 0 ){ //リセット
      BSUBWAY_SCOREDATA_SetFlag( scoreData,
          BSWAY_SCOREDATA_FLAG_WIFI_UPLOAD, BSWAY_SETMODE_reset );
    }else{ //セット
      BSUBWAY_SCOREDATA_SetFlag( scoreData,
          BSWAY_SCOREDATA_FLAG_WIFI_UPLOAD, BSWAY_SETMODE_set );
    }
    break;
  //Wifiアップロードフラグを取得
  case BSWTOOL_GET_WIFI_UPLOAD_FLAG:
    *ret_wk = BSUBWAY_SCOREDATA_SetFlag(
          scoreData, BSWAY_SCOREDATA_FLAG_WIFI_UPLOAD, BSWAY_SETMODE_get );
    break;
  //Wifi接続
  case BSWTOOL_WIFI_CONNECT:
    #if 0
    EventCmd_BTowerWifiCall(core->event_work,param,retwk_id,*ret_wk);
    return 1;
    #else
    GF_ASSERT( 0 && "BSWTOOL_WIFI_CONNECT WB未作成コマンドです" );
    #endif
    break;
  //----ワーク依存
  //プレイモード別復帰位置セット
  case BSWSUB_SET_PLAY_MODE_LOCATION:
    {
      VecFx32 pos;
      LOCATION loc;
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
      
      FIELD_PLAYER_GetPos( fld_player, &pos );
      
      switch( bsw_scr->play_mode ){
      case BSWAY_MODE_SINGLE:
        LOCATION_SetDirect( &loc, ZONE_ID_C04R0102, DIR_RIGHT, 11, 0, 15 );
        break;
      case BSWAY_MODE_S_SINGLE:
        LOCATION_SetDirect( &loc, ZONE_ID_C04R0103, DIR_RIGHT, 11, 0, 15 );
        break;
      case BSWAY_MODE_DOUBLE:
        LOCATION_SetDirect( &loc, ZONE_ID_C04R0104, DIR_RIGHT, 11, 0, 15 );
        break;
      case BSWAY_MODE_S_DOUBLE:
        LOCATION_SetDirect( &loc, ZONE_ID_C04R0105, DIR_RIGHT, 11, 0, 15 );
        break;
      case BSWAY_MODE_MULTI:
      case BSWAY_MODE_COMM_MULTI:
        LOCATION_SetDirect( &loc, ZONE_ID_C04R0106, DIR_RIGHT, 11, 0, 15 );
        break;
      case BSWAY_MODE_S_MULTI:
      case BSWAY_MODE_S_COMM_MULTI:
        LOCATION_SetDirect( &loc, ZONE_ID_C04R0107, DIR_RIGHT, 11, 0, 15 );
        break;
      case BSWAY_MODE_WIFI:
        LOCATION_SetDirect( &loc, ZONE_ID_C04R0108, DIR_RIGHT, 11, 0, 15 );
        break;
      default:
        GF_ASSERT( 0 );
      }
      
      GAMEDATA_SetSpecialLocation( gdata, &loc );
      EVENTWORK_SetEventFlag( event, SYS_FLAG_SPEXIT_REQUEST );
    }
    break;
  //ポケモン選択画面へ
  case BSWSUB_SELECT_POKE:
    SCRIPT_CallEvent( sc, BSUBWAY_EVENT_SetSelectPokeList(bsw_scr,gsys) );
    return VMCMD_RESULT_SUSPEND;
  //選択ポケモン取得
  case BSWSUB_GET_ENTRY_POKE:
    *ret_wk = BSUBWAY_SCRWORK_GetEntryPoke( bsw_scr, gsys );
    break;
  //参加指定した手持ちポケモンの条件チェック
  case BSWSUB_CHK_ENTRY_POKE:
    #if 0
    *ret_wk = TowerScr_CheckEntryPoke(bsw_scr,core->fsys->savedata);
    #else
    OS_Printf( "BSWSUB_GET_ENTRY_POKE WB未作成" );
    *ret_wk = 0; //0=OK 1=同じポケモン 2=同じアイテム
    #endif
    break;
  //クリアしているか
  case BSWSUB_IS_CLEAR:
    if( BSUBWAY_SCRWORK_IsClear(bsw_scr) == TRUE ){
      *ret_wk = TRUE;
    }else{
      *ret_wk = FALSE;
    }
    break;
  //敗北時のスコアセット
  case BSWSUB_SET_LOSE_SCORE:
    BSUBWAY_SCRWORK_SetLoseScore( gsys, bsw_scr );
    break;
  //クリア時のスコアセット
  case BSWSUB_SET_CLEAR_SCORE:
    BSUBWAY_SCRWORK_SetClearScore( bsw_scr, gsys );
    break;
  //休む際のプレイデータ処理
  case BSWSUB_SAVE_REST_PLAY_DATA:
    BSUBWAY_SCRWORK_SaveRestPlayData( bsw_scr );
    break;
  //対戦トレーナー抽選
  case BSWSUB_CHOICE_BTL_PARTNER:
    BSUBWAY_SCRWORK_ChoiceBattlePartner( bsw_scr );
    break;
  //敵トレーナーOBJコード取得
  case BSWSUB_GET_ENEMY_OBJ:
    *ret_wk = BSUBWAY_SCRWORK_GetTrainerOBJCode( bsw_scr, param0 );
    break;
  //バトル呼び出し
	case BSWSUB_LOCAL_BTL_CALL:
    SCRIPT_CallEvent(
        sc, BSUBWAY_EVENT_TrainerBattle(bsw_scr,gsys,fieldmap) );
    return VMCMD_RESULT_SUSPEND;
  //現在のプレイモードを取得
  case BSWSUB_GET_PLAY_MODE:
    *ret_wk = bsw_scr->play_mode;
    break;
  //ボスクリアフラグをセット
  case BSWSUB_SET_BOSS_CLEAR_FLAG:
    bsw_scr->boss_f = param0;
    break;
  //ボスクリアフラグを取得
  case BSWSUB_GET_BOSS_CLEAR_FLAG:
    *ret_wk = bsw_scr->boss_f;
    break;
  //バトルポイント加算
  case BSWSUB_ADD_BATTLE_POINT:
    *ret_wk = BSUBWAY_SCRWORK_AddBattlePoint( bsw_scr );
    break;
  //パートナー番号セット
  case BSWSUB_SET_PARTNER_NO:
    bsw_scr->partner = param0;
    break;
  //パートナー番号取得
  case BSWSUB_GET_PARTNER_NO:
    *ret_wk = bsw_scr->partner;
    break;
  //戦闘トレーナーセット
  case BSWSUB_BTL_TRAINER_SET:
    BSUBWAY_SCRWORK_SetBtlTrainerNo( bsw_scr );
    break;
  //選択ポケモン番号取得
  case BSWSUB_GET_SELPOKE_IDX:
    *ret_wk = bsw_scr->member[param0];
    break;
  //(BTS通信142)変更の対象
  case BSWSUB_WIFI_RANK_UP:
    #if 0
    *ret_wk = TowerScr_SetWifiRank(bsw_scr,core->fsys->savedata,1);
    #else
    GF_ASSERT( 0 && "BSWSUB_WIFI_RANK_UP WB未作成" );
    #endif
    break;
  //受信バッファクリア
  case BSWSUB_RECV_BUF_CLEAR:
    MI_CpuClear8( bsw_scr->recv_buf, BSWAY_SIO_BUF_LEN );
    break;
  //トレーナー対戦前メッセージ表示
  case BSWSUB_TRAINER_BEFORE_MSG:
    SCRIPT_CallEvent(
        sc, BSUBWAY_EVENT_TrainerBeforeMsg(bsw_scr,gsys,param0) );
    return( VMCMD_RESULT_SUSPEND );
  //ゲームクリア時のプレイデータをセーブ
  case BSWSUB_SAVE_GAMECLEAR:
    BSUBWAY_SCRWORK_SaveGameClearPlayData( bsw_scr );
    break;
  //ポケモンメンバーロード
  case BSWSUB_LOAD_POKEMON_MEMBER:
    BSUBWAY_SCRWORK_LoadPokemonMember( bsw_scr, gsys );
    break;
  //バトルボックス使用するか
  case BSWSUB_SET_USE_BBOX:
    {
      u8 buf = param0;
      BSUBWAY_PLAYDATA_SetData( playData,
          BSWAY_PLAYDATA_ID_use_battle_box, &buf );
    }
    break;
  //通信マルチモードにワークを変更
  case BSWSUB_CHG_WORK_COMM_MULTI_MODE:
    BSUBWAY_SCRWORK_ChangeCommMultiMode( bsw_scr );
    break;
  //スーパーモードか
  case BSWSUB_CHK_S_MODE:
    if( play_mode == BSWAY_MODE_S_SINGLE ||
        play_mode == BSWAY_MODE_S_DOUBLE ||
        play_mode == BSWAY_MODE_S_MULTI ||
        play_mode == BSWAY_MODE_S_COMM_MULTI ){
      *ret_wk = TRUE;
    }else{
      *ret_wk = FALSE;
    }
    break;
  //Wifiランクダウン
  case BSWSUB_WIFI_RANK_DOWN:
    *ret_wk = BSUBWAY_SCRWORK_SetWifiRank(
            bsw_scr, gsys, BSWAY_SETMODE_dec );
    break;
  //Wifiランク取得
  case BSWSUB_GET_WIFI_RANK:
    *ret_wk = BSUBWAY_SCRWORK_SetWifiRank(
            bsw_scr, gsys, BSWAY_SETMODE_get );
    break;
  //ホーム、OBJセット
  case BSWSUB_SET_HOME_OBJ:
    break;
  //----ワーク依存　通信関連
  //通信開始
  case BSWSUB_COMM_START:
    BSUBWAY_COMM_Init( bsw_scr );
    break;
  //通信終了
  case BSWSUB_COMM_END:
    BSUBWAY_COMM_Exit( bsw_scr );
    break;
  //通信同期
  case BSWSUB_COMM_TIMSYNC:
    bsw_scr->comm_timing_no = param0;
    BSUBWAY_COMM_TimingSyncStart( bsw_scr->comm_timing_no );
    VMCMD_SetWait( core, evCommTimingSync );
    return( VMCMD_RESULT_SUSPEND );
  //親機選択メニュー表示
  case BSWSUB_COMM_ENTRY_PARENT_MENU:
    {
      bsw_scr->pCommEntryResult = ret_wk;
      bsw_scr->pCommEntryMenu = CommEntryMenu_Setup(
          GAMEDATA_GetMyStatus(gdata),
          fieldmap, 2, 2, HEAPID_PROC,
          COMM_ENTRY_MODE_PARENT,
          COMM_ENTRY_GAMETYPE_SUBWAY, NULL );
      VMCMD_SetWait( core, evCommEntryMenuPerent );
    }
    return( VMCMD_RESULT_SUSPEND );
  //子機選択メニュー表示
  case BSWSUB_COMM_ENTRY_CHILD_MENU:
    {
      bsw_scr->pCommEntryResult = ret_wk;
      bsw_scr->pCommEntryMenu = CommEntryMenu_Setup(
          GAMEDATA_GetMyStatus(gdata),
          fieldmap, 2, 2, HEAPID_PROC, 
          COMM_ENTRY_MODE_CHILD,
          COMM_ENTRY_GAMETYPE_SUBWAY, NULL );
      VMCMD_SetWait( core, evCommEntryMenuChild );
    }
    return( VMCMD_RESULT_SUSPEND );
  //通信データ送信
  case BSWSUB_COMM_SEND_BUF:
    BSUBWAY_SCRWORK_CommSendData( bsw_scr, param0, param1 );
    break;
  //通信データ受信
  case BSWSUB_COMM_RECV_BUF:
    BSUBWAY_SCRWORK_CommRecieveDataStart( bsw_scr, param0 );
    bsw_scr->ret_work_id = retwk_id;
    VMCMD_SetWait( core, evCommRecvData );
    return( VMCMD_RESULT_SUSPEND );
    break;
  //自分の通信IDを取得
  case BSWSUB_COMM_GET_CURRENT_ID:
    *ret_wk = GFL_NET_SystemGetCurrentID();
    break;
  //未対応コマンドエラー
  default:
    OS_Printf( "渡されたcom_id = %d\n", com_id );
    GF_ASSERT( 0 && "com_idが未対応です！" );
    *ret_wk = 0;
    break;
  }
  
  return( VMCMD_RESULT_CONTINUE );
}

//======================================================================
//  通信関連
//======================================================================
//--------------------------------------------------------------
/**
 * 通信同期待ち
 * @param core VMHANDLE
 * @param wk script work
 * @retval BOOL
 */
//--------------------------------------------------------------
static BOOL evCommTimingSync( VMHANDLE *core, void *wk )
{
  COMM_ENTRY_RESULT entry_ret;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  BSUBWAY_SCRWORK *bsw_scr = GAMEDATA_GetBSubwayScrWork( gdata );
  
  if( BSUBWAY_COMM_IsTimingSync(bsw_scr->comm_timing_no) == TRUE ){
    return( TRUE );
  }
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * 通信マルチ受付メニュー　親
 * @param core VMHANDLE
 * @param wk script work
 * @retval BOOL
 */
//--------------------------------------------------------------
static BOOL evCommEntryMenuPerent( VMHANDLE *core, void *wk )
{
  COMM_ENTRY_RESULT entry_ret;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( gdata );
  EVENTWORK *event = GAMEDATA_GetEventWork( gdata );
  BSUBWAY_SCRWORK *bsw_scr = GAMEDATA_GetBSubwayScrWork( gdata );
  BSUBWAY_PLAYDATA *playData =
    SaveControl_DataPtrGet( save, GMDATA_ID_BSUBWAY_PLAYDATA );
  u32 play_mode = (u32)BSUBWAY_PLAYDATA_GetData(
        playData, BSWAY_PLAYDATA_ID_playmode, NULL );
  BSUBWAY_SCOREDATA *scoreData = SaveControl_DataPtrGet(
      save, GMDATA_ID_BSUBWAY_SCOREDATA );
  
  entry_ret = CommEntryMenu_Update( bsw_scr->pCommEntryMenu ); 
  
  if( entry_ret != COMM_ENTRY_RESULT_NULL ){
    CommEntryMenu_Exit( bsw_scr->pCommEntryMenu );
    bsw_scr->pCommEntryMenu = NULL;
  }
  
  switch( entry_ret ){
  case COMM_ENTRY_RESULT_SUCCESS:      //メンバーが集まった
    (*bsw_scr->pCommEntryResult) = BSWAY_COMM_PERENT_ENTRY_OK;
    return( TRUE );
  case COMM_ENTRY_RESULT_CANCEL:       //キャンセルして終了
    (*bsw_scr->pCommEntryResult) = BSWAY_COMM_PERENT_ENTRY_CANCEL;
    return( TRUE );
  case COMM_ENTRY_RESULT_ERROR:        //エラーで終了
    (*bsw_scr->pCommEntryResult) = BSWAY_COMM_PERENT_ENTRY_ERROR;
    return( TRUE );
  }
   
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * 通信マルチ受付メニュー　子
 * @param core VMHANDLE
 * @param wk script work
 * @retval BOOL
 */
//--------------------------------------------------------------
static BOOL evCommEntryMenuChild( VMHANDLE *core, void *wk )
{
  COMM_ENTRY_RESULT entry_ret;
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  SCRIPT_FLDPARAM *fparam = SCRIPT_GetFieldParam( sc );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK *save = GAMEDATA_GetSaveControlWork( gdata );
  EVENTWORK *event = GAMEDATA_GetEventWork( gdata );
  BSUBWAY_SCRWORK *bsw_scr = GAMEDATA_GetBSubwayScrWork( gdata );
  BSUBWAY_PLAYDATA *playData =
    SaveControl_DataPtrGet( save, GMDATA_ID_BSUBWAY_PLAYDATA );
  u32 play_mode = (u32)BSUBWAY_PLAYDATA_GetData(
        playData, BSWAY_PLAYDATA_ID_playmode, NULL );
  BSUBWAY_SCOREDATA *scoreData = SaveControl_DataPtrGet(
      save, GMDATA_ID_BSUBWAY_SCOREDATA );
  
  entry_ret = CommEntryMenu_Update( bsw_scr->pCommEntryMenu ); 
  
  if( entry_ret != COMM_ENTRY_RESULT_NULL ){
    CommEntryMenu_Exit( bsw_scr->pCommEntryMenu );
    bsw_scr->pCommEntryMenu = NULL;
  }
  
  switch( entry_ret ){
  case ENTRY_PARENT_ANSWER_OK:      //エントリーOK
    (*bsw_scr->pCommEntryResult) = BSWAY_COMM_CHILD_ENTRY_OK;
    return( TRUE );
  case ENTRY_PARENT_ANSWER_NG:    //エントリーNG
    (*bsw_scr->pCommEntryResult) = BSWAY_COMM_CHILD_ENTRY_NG ;
    return( TRUE );
  }
  
  return( FALSE );
}

//--------------------------------------------------------------
/**
 * 通信受信待ち
 * @param
 * @retval
 */
//--------------------------------------------------------------
static BOOL evCommRecvData( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  GAMESYS_WORK *gsys = SCRIPT_GetGameSysWork( sc );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  BSUBWAY_SCRWORK *bsw_scr = GAMEDATA_GetBSubwayScrWork( gdata );
  u16 *ret_wk = SCRIPT_GetEventWork( sc, gdata, bsw_scr->ret_work_id );
  
  if( BSUBWAY_SCRWORK_CommRecieveData(bsw_scr,ret_wk) == TRUE ){
    return( TRUE );
  }
  
  return( FALSE );
}

//======================================================================
//  parts
//======================================================================
//--------------------------------------------------------------
/**
 * 参加可能なポケモン数のチェック
 * @param  num      参加に必要なポケモン数
 * @param  item_flag  アイテムチェックするかフラグ
 * @retval BOOL TRUE=参加可能
 */
//--------------------------------------------------------------
static BOOL bsway_CheckEntryPokeNum(
    u16 num, GAMESYS_WORK *gsys, BOOL item_flag )
{
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  const POKEPARTY *party =  GAMEDATA_GetMyPokemon( gdata );
  
  if( PokeParty_GetPokeCount(party) < num ){
    return FALSE;
  }
  
  return( TRUE );
}

//--------------------------------------------------------------
/**
 * レギュレーションチェック
 * @param mode プレイモード
 * @param gsys GAMESYS_WORK
 * @retval BOOL TRUE=OK
 */
//--------------------------------------------------------------
static BOOL bsway_CheckRegulation( int mode, GAMESYS_WORK *gsys )
{
  int reg_type,ret;
  GAMEDATA *gdata;
  POKEPARTY *party;
  REGULATION *reg_data;
  
  switch( mode ){
  case BSWAY_MODE_SINGLE:
    reg_type = REG_SUBWAY_SINGLE;
    break;
  case BSWAY_MODE_DOUBLE:
    reg_type = REG_SUBWAY_DOUBLE;
    break;
  default:
    GF_ASSERT( 0 );
    reg_type = REG_SUBWAY_DOUBLE;
  }

  reg_data = (REGULATION*)PokeRegulation_LoadDataAlloc(
      reg_type, HEAPID_PROC );
  
  gdata = GAMESYSTEM_GetGameData( gsys );
  party =  GAMEDATA_GetMyPokemon( gdata );
  ret = PokeRegulationMatchPartialPokeParty( reg_data, party );
  GFL_HEAP_FreeMemory( reg_data );
  
  if( ret == POKE_REG_OK || POKE_REG_TOTAL_LV_FAILED ){
    return( TRUE );
  }
  
  return( FALSE );
}

//======================================================================
//  data
//======================================================================
//--------------------------------------------------------------
/// バトルサブウェイ　プレイモード別　列車種類
//--------------------------------------------------------------
static const FLDEFF_BTRAIN_TYPE data_TrainModeType[BSWAY_MODE_MAX] =
{
  FLDEFF_BTRAIN_TYPE_01,
  FLDEFF_BTRAIN_TYPE_03,
  FLDEFF_BTRAIN_TYPE_05,
  FLDEFF_BTRAIN_TYPE_05,
  FLDEFF_BTRAIN_TYPE_07,

  FLDEFF_BTRAIN_TYPE_02,
  FLDEFF_BTRAIN_TYPE_04,
  FLDEFF_BTRAIN_TYPE_06,
  FLDEFF_BTRAIN_TYPE_06,
};

//--------------------------------------------------------------
/// バトルサブウェイ　列車位置テーブル
//--------------------------------------------------------------
static const VecFx32 data_TrainPosTbl[BTRAIN_POS_MAX] =
{
  {
    GRID_SIZE_FX32(45),
    GRID_SIZE_FX32(-2),
    GRID_SIZE_FX32(11) + GRID_HALF_FX32,
  },
  {
    GRID_SIZE_FX32(47),
    GRID_SIZE_FX32(-3),
    GRID_SIZE_FX32(9) + GRID_HALF_FX32,
  },
};

