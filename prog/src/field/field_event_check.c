//======================================================================
/**
 * @file  field_event_check.c
 * @brief フィールドでのイベント起動チェック処理群
 * @author  tamada GAME FREAK inc.
 * @date  2009.06.09
 *
 * 2009.06.09 fieldmap.cからイベント起動チェック部分を独立させた
 */
//======================================================================

#include <gflib.h>
#include "gamesystem/gamesystem.h"
#include "gamesystem/playerwork.h"
#include "gamesystem/game_event.h"
#include "field/zonedata.h"
//#include "field/areadata.h"

#include "field_event_check.h"

#include "fieldmap.h"
#include "field/fieldmap_proc.h"
#include "field/eventdata_system.h"
#include "field/eventdata_sxy.h"
#include "eventdata_local.h"

#include "system/wipe.h"      //WIPE_SYS_EndCheck
#include "event_mapchange.h"
#include "field_comm/intrude_field.h"
#include "event_debug_menu.h"     //DEBUG_EVENT_DebugMenu
#include "event_battle.h"         //EVENT_Battle
#include "event_beacon_detail.h"         //EVENT_BeaconDetail
#include "event_gpower.h"         //EVENT_CheckGPower
#include "event_fieldtalk.h"      //EVENT_FieldTalk
#include "event_fieldmap_menu.h"  //EVENT_FieldMapMenu
#include "rail_editor.h"
#include "script.h"     //SCRIPT_SetEventScript
#include "net_app/union/union_event_check.h"
#include "event_comm_common.h"
#include "event_comm_result.h"
#include "event_rail_slipdown.h"    //EVENT_RailSlipDown
#include "event_intrude_subscreen.h"
#include "event_subscreen.h"      //EVENT_ChangeSubScreen
#include "event_shortcut_menu.h"  //EVENT_ShortCutMenu
#include "event_research_radar.h"  //EVENT_ResearchRadar
#include "event_seatemple.h"  //EVENT_SeaTemple〜

#include "system/main.h"    //HEAPID_FIELDMAP
#include "isdbglib.h"

#include "field_encount.h"      //FIELD_ENCOUNT_CheckEncount
#include "effect_encount.h"

#include "fieldmap_ctrl_grid.h"
#include "fieldmap_ctrl_nogrid_work.h"

#include "map_attr.h"

#include "event_trainer_eye.h"
#include "fieldmap/zone_id.h"
#include "net_app/union/union_main.h"
#include "net_app/union/union_chara.h"
#include "event_comm_error.h"
#include "./c_gear/c_gear.h"             //CGEAR_IsDoEventWireless

#include "../../../resource/fldmapdata/script/bg_attr_def.h" //SCRID_BG_MSG_〜
#include "../../../resource/fldmapdata/script/hiden_def.h"
#include "../../../resource/fldmapdata/script/tv_scr_def.h"  //for SCRID_TV
#include "../../../resource/fldmapdata/script/debug_scr_def.h"  //for SCRID_DEBUG_

#include "field_gimmick.h"   //for FLDGMK_GimmickCodeCheck
#include "field_gimmick_def.h"  //for FLD_GIMMICK_GYM_〜
#include "gym_elec.h"     //for GYM_ELEC_CreateMoveEvt
#include "gym_fly.h"     //for GYM_FLY_CreateShotEvt
#include "gym_insect.h"     //for GYM_INSECT_CreatePushWallEvt
#include "field_gimmick_d06.h"

#include "../../../resource/fldmapdata/flagwork/flag_define.h"

#include "waza_tool/wazano_def.h"
#include "field/field_comm/intrude_main.h"
#include "field/field_comm/intrude_mission.h"

#include "savedata/encount_sv.h"
#include "../../../resource/fldmapdata/script/eggevent_scr_def.h"   // for SCRID_EGG_BIRTH
#include "../../../resource/fldmapdata/script/pasokon_def.h" // for SCRID_PC
#include "../../../resource/fldmapdata/script/field_ev_scr_def.h" // for SCRID_FLE_EV_SPRAY_EFFECT_END
#include "../../../resource/fldmapdata/script/vending_machine_def.h" // for SCRID_VENDING_MACHINE01

#include "debug/debug_flg.h" //DEBUG_FLG_〜

#include "field/field_const.h"  // for FX32_TO_GRID

#include "event_fourkings.h"  // 

#include "pleasure_boat.h"    //for PL_BOAT_

#include "event_autoway.h" // for EVENT_PlayerMoveOnAutoWay
#include "event_current.h" // for EVENT_PlayerMoveOnCurrent

#include "field_status_local.h" //FIELD_STATUS_GetReservedScript

#include "event_intrude.h"
#include "event_symbol.h"
#include "field_comm/intrude_work.h"

#include "scrcmd_intrude.h"

#include "savedata/situation.h"
#include "gamesystem/g_power.h" //GPOWER_Calc_Hatch

#include "savedata/intrude_save_field.h"  //ISC_SAVE_CheckItem
#include "event_intrude_secret_item.h"    //
#include "field_diving_data.h"  //DIVINGSPOT_Check
#include "poke_tool/natsuki.h"  //NATSUKI_CalcTsurearuki
#include "poke_tool/tokusyu_def.h" //TOKUSYU_xxxx


#ifdef PM_DEBUG
extern BOOL DebugBGInitEnd;    //BG初期化監視フラグ             宣言元　fieldmap.c
extern BOOL MapFadeReqFlg;    //マップフェードリクエストフラグ  宣言元　script.c

#endif


//======================================================================
//======================================================================
//#define DEBUG_SPEED_CHECK_ENABLE
#include "debug_speed_check.h"

//======================================================================
//======================================================================
#define FRONT_TALKOBJ_HEIGHT_DIFF (FX32_CONST(16)) //話しかけ可能なOBJのY値差分

//--------------------------------------------------------------
/**
 * @brief 判定チェック用パラメータ
 */
//--------------------------------------------------------------
typedef struct {
  HEAPID heapID;              ///<ヒープID
  GAMESYS_WORK * gsys;        ///<ゲームシステムへのポインタ
  GAMEDATA * gamedata;        ///<ゲームデータへのポインタ
  EVENTDATA_SYSTEM *evdata;   ///<参照しているイベントデータ
  u16 map_id;                 ///<現在のマップID
  //↑変化がないパラメータ　

  //↓毎回変化する可能性があるパラメータ
  FIELDMAP_WORK * fieldWork;
  FIELD_PLAYER * field_player;
  PLAYER_MOVE_STATE player_state;
  PLAYER_MOVE_VALUE player_value;
  u16 player_dir;                   ///<自機の向き
  u16 reserved_scr_id;        ///<予約スクリプトのID
  int key_trg;                ///<キー情報（トリガー）
  int key_cont;               ///<キー情報（）
  u16 key_direction;          ///<キー入力の方向
  const VecFx32 * now_pos;        ///<現在のプレイヤー位置
  u32 mapattr;                  ///<足元のアトリビュート情報

  u32 talkRequest:1; ///<話しかけキー操作があったかどうか
  u32 menuRequest:1; ///<メニューオープン操作があったかどうか
  u32 moveRequest:1; ///<一歩移動終了タイミングかどうか
  u32 stepRequest:1; ///<振り向きor一歩移動終了タイミングかどうか
  u32 dirPosRequest:1; ///<方向つきPOSイベントのチェックをするタイミングか？
  u32 pushRequest:1; ///<押し込み操作があったかどうか
  u32 convRequest:1; ///<便利ボタン操作があったかどうか
  u32 subscreenRequest:1; ///<サブスクリーンイベントを起動してよいタイミングか？

  u32 debugRequest:1;  ///<デバッグ操作があったかどうか
  u32 exMoveRequest:1;  ///<波乗り開始時等moveReqが検出しない一歩移動の検出

#ifdef BUGFIX_GFBTS1975_100713
  u32 commRequest:1;  ///<通信エラー画面を呼び出していいタイミングか？
  u32 req_dmy:21;
#else
  u32 req_dmy:22;
#endif

}EV_REQUEST;

//======================================================================
//======================================================================
//--------------------------------------------------------------
//
//--------------------------------------------------------------
static void setupRequest(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork);

static void stopTVTSE( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap );
static void incWalkCount( GAMEDATA * gamedata );
//--------------------------------------------------------------
//  個別イベントチェック
//--------------------------------------------------------------
static GMEVENT * checkMoveEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork);
static GMEVENT* CheckSodateya(
    const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata );
static GMEVENT* CheckSpray( FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata );
static GMEVENT* CheckEffectEncount( FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata );
static GMEVENT* CheckGPowerEffectEnd( GAMESYS_WORK* gsys );
static u8 getHatchCountUpdateValue( POKEPARTY* party );
static void updateEggHatchCount( POKEPARTY* party );
static void updatePartyEgg( GAMEDATA * gamedata, POKEPARTY* party );
static BOOL checkPartyEgg( POKEPARTY* party );
static void updateFriendyStepCount( GAMEDATA * gamedata, FIELDMAP_WORK * fieldmap );

static GMEVENT* CheckSeaTemple( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, u16 zone_id );


static GMEVENT * checkExit(EV_REQUEST * req,
    FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos );
static GMEVENT * checkAttribute(EV_REQUEST * req, FIELDMAP_WORK *fieldWork );
static GMEVENT * checkPushExit(EV_REQUEST * req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT * checkPushGimmick(const EV_REQUEST * req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT * checkNormalEncountEvent( const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );


static void setFrontPos(const EV_REQUEST * req, VecFx32 * pos);
static BOOL checkConnectExitDir(const CONNECT_DATA* cnct, u16 player_dir);
static BOOL checkConnectIsFreeExit( const CONNECT_DATA * cnct );
static int getConnectID(const EV_REQUEST * req, const VecFx32 * now_pos);
static void rememberExitInfo(EV_REQUEST * req, int idx, const VecFx32 * pos);
static GMEVENT * getChangeMapEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx,
    const VecFx32 * pos);
static GMEVENT * DEBUG_checkKeyEvent(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork);

static GMEVENT * checkEvent_ConvenienceButton( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );

static GMEVENT * checkEvent_PlayerNaminoriStart( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );

static GMEVENT * checkEvent_PlayerNaminoriEnd( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );

static MMDL * getFrontTalkOBJ( EV_REQUEST *req, FIELDMAP_WORK *fieldMap );

static MMDL * getRailFrontTalkOBJ( EV_REQUEST *req, FIELDMAP_WORK *fieldMap );

static u16 checkTalkAttrEvent( EV_REQUEST *req, FIELDMAP_WORK *fieldMap);

static int checkPokeWazaGroup( GAMEDATA *gdata, u32 waza_no );

static GMEVENT* checkSpecialEvent( EV_REQUEST * req );

static GMEVENT* checkPosEvent( EV_REQUEST* req );
static GMEVENT* checkPosEvent_core( EV_REQUEST * req, u16 dir );
static GMEVENT* checkPosEvent_OnlyDirection( EV_REQUEST * req );
static GMEVENT* checkPosEvent_prefetchDirection( EV_REQUEST * req );
static GMEVENT * checkPosEvent_IntrudeDirection( EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork );
static GMEVENT* checkPosEvent_sandstream( EV_REQUEST* req );

//--------------------------------------------------------------
//レール用
//--------------------------------------------------------------
static GMEVENT * eventCheckNoGrid( GAMESYS_WORK *gsys, void *work );

static GMEVENT * checkRailExit(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork);
static GMEVENT * checkRailPushExit(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork, const RAIL_LOCATION* front_location);
static GMEVENT * checkRailSlipDown(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork);
static void rememberExitRailInfo(const EV_REQUEST * req, int idx, const RAIL_LOCATION* loc);
static GMEVENT * getRailChangeMapEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx,
    const RAIL_LOCATION* loc);
static BOOL checkRailFrontMove( const FIELD_PLAYER* cp_player, const EV_REQUEST * req );


//--------------------------------------------------------------
//--------------------------------------------------------------
static BOOL NaminoriEnd_CheckMMdl( const EV_REQUEST* req, FIELDMAP_WORK* fieldWork );


//======================================================================
//
//
//    イベント起動チェック本体
//
//
//======================================================================

//--------------------------------------------------------------
/**
 * @brief　イベント起動チェック：通常用
 * @param gsys GAMESYS_WORK
 * @param work FIELDMAP_WORK
 * @retval GMEVENT NULL=イベント無し
 */
//--------------------------------------------------------------
static GMEVENT * FIELD_EVENT_CheckNormal(
    GAMESYS_WORK *gsys, void *work, BOOL *eff_delete_flag, BOOL *menu_open_flag )
{
  
  EV_REQUEST req;
  GMEVENT *event;
  FIELDMAP_WORK *fieldWork = work;

  INIT_CHECK();
  *eff_delete_flag = FALSE;  //まずはリクエストなし
  *menu_open_flag = FALSE;

  SET_CHECK("ev_check:setupRequest");
  //リクエスト更新
  setupRequest( &req, gsys, fieldWork );


  SET_CHECK("ev_check:debug_key_event");
//☆☆☆デバッグ用チェックを最優先とする
#ifdef  PM_DEBUG
  event = DEBUG_checkKeyEvent( &req, gsys, fieldWork );
  if (event != NULL) {
    return event;
  }
#endif //PM_DEBUG

  
  SET_CHECK("ev_check:special event");
//☆☆☆特殊スクリプト起動チェックがここに入る
#ifdef  PM_DEBUG
  if (DEBUG_FLG_GetFlg(DEBUG_FLG_DisableEvents) == FALSE)
#endif
  {
    event = checkSpecialEvent( &req );
    if ( event )
    {
      *eff_delete_flag = TRUE;  //エフェクトエンカウント消去リクエスト
      return event;
    }
  }

#ifdef PM_DEBUG
  GF_ASSERT_MSG(DebugBGInitEnd,"ERROR:BG NOT INITIALIZE");    //BG初期化監視フラグ
  GF_ASSERT_MSG(!MapFadeReqFlg,"ERROR:CALLED MAP FADE REQ");   //マップフェードリクエストフラグ
#endif  

  //遊覧船時間監視
  {
    PL_BOAT_WORK_PTR *wk_ptr = GAMEDATA_GetPlBoatWorkPtr(req.gamedata);
    event = PL_BOAT_CheckEvt(gsys, *wk_ptr);
    if (event != NULL) return event;
  }



  SET_CHECK("ev_check:trainer eye");
//☆☆☆トレーナー視線チェックがここに入る
#ifdef  PM_DEBUG
  if( !(req.debugRequest) && DEBUG_FLG_GetFlg(DEBUG_FLG_DisableTrainerEye) == FALSE )
#endif
  {
    event = EVENT_CheckTrainerEye( fieldWork );
     
    if( event != NULL ){
      *eff_delete_flag = TRUE;  //エフェクトエンカウント消去リクエスト
      return event;
    }
  }


  SET_CHECK("ev_check:move event");
//☆☆☆一歩移動チェックがここから
  //歩数をカウント
  if( req.moveRequest )
  {
    incWalkCount( req.gamedata );
  }
  //座標イベントチェック
  if( req.moveRequest )
  {
    // POSイベントチェック
    event = checkPosEvent( &req );
    if( event != NULL ){
      *eff_delete_flag = TRUE;  //エフェクトエンカウント消去リクエスト
      return event;
    } 
    //座標接続チェック
    event = checkExit(&req, fieldWork, req.now_pos);
    if( event != NULL ){
      return event;
    }
    // アトリビュートチェック
    event = checkAttribute( &req, fieldWork );
    if( event != NULL ) {
      return event;
    }
  }
  //汎用一歩移動イベントチェック群
  event = checkMoveEvent(&req, fieldWork);
  if ( event != NULL) {
    return event;
  }

  SET_CHECK("ev_check:step event");
//☆☆☆ステップチェック（一歩移動or振り向き）がここから
  if (req.stepRequest )
  {
    //POSイベント：向き指定ありをチェック
    event = checkPosEvent_OnlyDirection( &req );
    if (event)
    {
      return event;
    }
  }
  //POSイベント：方向指定ありチェックその２
  {
    event = checkPosEvent_prefetchDirection( &req );
    if (event)
    {
      return event;
    }
  }
  //POSイベント：方向指定ありチェック：侵入
  {
    event = checkPosEvent_IntrudeDirection( &req, gsys, fieldWork );
    if( event != NULL ){
      return event;
    }
  }
  
  //戦闘移行チェック
  { //ノーマルエンカウントイベント起動チェック
    GMEVENT* enc_event = checkNormalEncountEvent( &req, gsys, fieldWork );
    if(enc_event != NULL){
      return enc_event;
    }
  }


  SET_CHECK("ev_check:player event");
//☆☆☆自機状態イベントチェック
#ifdef  PM_DEBUG
  if( !(req.debugRequest) )
#endif
  {
    PLAYER_EVENTBIT evbit = PLAYER_EVENTBIT_NON;
     EVENTWORK *ev = GAMEDATA_GetEventWork( req.gamedata );
    
    if( EVENTWORK_CheckEventFlag(ev,SYS_FLAG_KAIRIKI) == TRUE ){
      evbit |= PLAYER_EVENTBIT_KAIRIKI;
    }
    
    event = FIELD_PLAYER_CheckMoveEvent(
        req.field_player, req.key_trg, req.key_cont, evbit );
    
    if( event != NULL ){
      return event;
    }
  }
  
//☆☆☆会話チェック

  SET_CHECK("ev_check:game_comm");
  ///通信用会話処理
  {
    GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
    INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
    MMDL *fmmdl_player = FIELD_PLAYER_GetMMdl( req.field_player );
    u32 talk_netid;
    
    //侵入先のROM相手が切断されていれば自分のパレスへワープ
    if(IntrudeField_CheckIntrudeShutdown(gsys, req.map_id) == TRUE){
      return EVENT_IntrudeForceWarpMyPalace(gsys, MISSION_FORCEWARP_MSGID_NULL);
    }
    if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_INVASION && intcomm != NULL){
      //裏フィールドにいて自分がミッションターゲットにされているなら表へ戻す
      if(GAMEDATA_GetIntrudeReverseArea(req.gamedata) == TRUE 
          && IntrudeField_Check_Tutorial_OR_TargetMine(intcomm) == TRUE){
        return EVENT_MissionTargetWarp(gsys, fieldWork );
      }
      //シンボルマップにいて所有者の更新イベントが発動していないかチェック
      if(IntrudeSymbol_CheckSymbolDataChange(intcomm, req.map_id) == TRUE){
        return EVENT_SymbolMapWarpEasy( gsys, DIR_NOT, GAMEDATA_GetSymbolMapID( req.gamedata ) );
      }
      //話しかけられていないかチェック
    #ifdef BUGFIX_GFBTS1979_100714
      if(req.commRequest == TRUE && Intrude_CheckTalkedTo(intcomm, &talk_netid) == TRUE){
    #else
      if(Intrude_CheckTalkedTo(intcomm, &talk_netid) == TRUE){
    #endif
        FIELD_PLAYER_ForceStop( req.field_player );
        return EVENT_CommCommon_Talked(gsys, fieldWork, intcomm, fmmdl_player, talk_netid, req.heapID);
      }
    #if 0
      //ミッション結果を受信していないかチェック
      if(MISSION_CheckRecvResult(&intcomm->mission) == TRUE){
        FIELD_PLAYER_ForceStop( req.field_player );
        return EVENT_CommMissionResult(gsys, fieldWork, req.heapID);
      }
    #else
      #if 0 //侵入通信メインで管理するように変更 2010.05.29(土)
      //ミッションタイムアウトで失敗
      if(MISSION_CheckResultTimeout(&intcomm->mission) == TRUE){
        return EVENT_IntrudeForceWarpMyPalace(gsys, MISSION_FORCEWARP_MSGID_NULL);
      }
      #endif
    #endif
      //隠しアイテムの目的地到達チェック
      if (req.stepRequest ){
        event = IntrudeField_CheckSecretItemEvent(gsys, intcomm, req.field_player);
        if(event != NULL){
          return event;
        }
      }
      //話しかける
      if( req.talkRequest ){
        if(IntrudeField_CheckTalk(intcomm, req.field_player, &talk_netid) == TRUE){
          FIELD_PLAYER_ForceStop( req.field_player );
          return EVENT_CommCommon_Talk(gsys, fieldWork, intcomm, fmmdl_player, talk_netid, req.heapID);
        }
      }
    }
  }

  SET_CHECK("ev_check:talk event");
  //フィールド話し掛けチェック
  if( req.talkRequest )
  {
    // 自機一歩前の座標を求める
    VecFx32 pos;
    u16 dir = FIELD_PLAYER_GetDir( req.field_player );
    FIELD_PLAYER_GetPos( req.field_player, &pos );
    MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 );
    
    { //OBJ話し掛け
      MMDL *fmmdl_talk = getFrontTalkOBJ( &req, fieldWork );
      if( fmmdl_talk != NULL )
      {
        u32 scr_id = MMDL_GetEventID( fmmdl_talk );
        if ( SCRIPT_IsValidScriptID( scr_id ) == TRUE )
        {
          MMDL *fmmdl_player = FIELD_PLAYER_GetMMdl( req.field_player );
          FIELD_PLAYER_ForceStop( req.field_player );
          *eff_delete_flag = TRUE;  //エフェクトエンカウント消去リクエスト
          return EVENT_FieldTalk( gsys, fieldWork,
            scr_id, fmmdl_player, fmmdl_talk, req.heapID );
        }
      }
    }
    
    { //BG話し掛け
      u16 id;
      EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
      
      id = EVENTDATA_CheckTalkBGEvent( req.evdata, evwork, &pos, dir );
      
      if( id != EVENTDATA_ID_NONE ){ //座標イベント起動
        event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
        return event;
      }
    }

    { //BG Attribute 話しかけ
      u16 id = checkTalkAttrEvent( &req, fieldWork );
      if( id != EVENTDATA_ID_NONE ){ //座標イベント起動
        event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
        return event;
      }
    }

    { //隠されアイテムチェック
      INTRUDE_SAVE_WORK * intsave;
      s16 gx,gy,gz;
      int result;
      gx = FX32_TO_GRID( pos.x );
      gy = FX32_TO_GRID( pos.y );
      gz = FX32_TO_GRID( pos.z );
      intsave = SaveData_GetIntrude( GAMEDATA_GetSaveControlWork( req.gamedata ) );
      result = ISC_SAVE_CheckItem( intsave, FIELDMAP_GetZoneID( fieldWork ), gx, gy, gz );
      if ( result != ISC_SAVE_SEARCH_NONE )
      {
        return EVENT_IntrudeSecretItem( gsys, req.heapID, result );
      }
    }

    { //ダイビングチェック
      u16 dummy;
      if ( DIVINGSPOT_Check( fieldWork, &dummy ) == TRUE )
      {
        return SCRIPT_SetEventScript( gsys, SCRID_HIDEN_DIVING, NULL, req.heapID );
      }
    }

    { //BG ダミーポジション 話しかけ  イベントデータ節約のための特殊処理　

      if( ZONEDATA_IsSeaTempleDungeon( req.map_id ) ){ // 現在海底神殿のみ
        EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
        u16 id = EVENTDATA_CheckDummyPosEvent( req.evdata, evwork, &pos );
        if( id != EVENTDATA_ID_NONE ){ //座標イベント起動
          event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
          return event;
        }
      }
    }

  }


  SET_CHECK("ev_check:naminori");
  { //波乗りテスト

    if( req.player_state == PLAYER_MOVE_STATE_END ||
        req.player_state == PLAYER_MOVE_STATE_OFF )
    {
      #ifdef PM_DEBUG
      if( !(req.debugRequest) ){
        event = checkEvent_PlayerNaminoriEnd( &req, gsys, fieldWork );
        if( event != NULL ){
         return event;
       }
      }
      #else
      event = checkEvent_PlayerNaminoriEnd( &req, gsys, fieldWork );
      if( event != NULL ){
        return event;
      }
      #endif
    }
  }

  
  SET_CHECK("ev_check:push event");
//☆☆☆押し込み操作チェック（マットでのマップ遷移など）
  //キー入力接続チェック
  if (req.pushRequest) {
    event = checkPushExit(&req, gsys, fieldWork);
    if( event != NULL ){
      return event;
    }
    event = checkPushGimmick(&req, gsys, fieldWork);
    if( event != NULL ){
      return event;
    }
  }
  
  SET_CHECK("ev_check:menu");
//☆☆☆自機位置に関係ないキー入力イベントチェック
  //便利ボタンチェック
  if( req.convRequest ){
    if(WIPE_SYS_EndCheck()){
      FIELD_SUBSCREEN_WORK* subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
      //メニューの終了後の１フレーム程度にYボタンを起動すると、
      //先にYボタンメニューの初期化が走り、その後メニューの終了処理で上画面のスクリーンをクリアして
      //しまうため、メニューの終了を待ってから動作させる
      if( EVENT_ShortCutMenu_IsOpen(gsys) && FIELD_SUBSCREEN_CanChange( subscreen) )
      { 
        event = EVENT_ShortCutMenu( gsys, fieldWork, req.heapID );
        if( event != NULL ){
          return event;
        }
      }
    }
  }
  
  //通信エラー画面呼び出しチェック(※メニュー起動チェックの真上に配置する事！)
  if( GAMESYSTEM_GetFieldCommErrorReq(gsys) == TRUE ){
#ifdef BUGFIX_GFBTS1975_100713
    if(WIPE_SYS_EndCheck() && req.commRequest){
#else
    if(WIPE_SYS_EndCheck()){
#endif
      return EVENT_FieldCommErrorProc(gsys, fieldWork);
    }
  }
  
  //メニュー起動チェック
  if( req.menuRequest ){
    if(WIPE_SYS_EndCheck()){
      {
        FIELD_SUBSCREEN_WORK* subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
        if( FIELD_SUBSCREEN_GetMode( subscreen ) == FIELD_SUBSCREEN_DOWSING )  // ダウジングマシンを使っているときは、ダウジングマシンを終了させるだけ
        {
          return NULL;
        }
      }
      *menu_open_flag = TRUE;
      return EVENT_FieldMapMenu( gsys, fieldWork, req.heapID );
    }
  }
  
  SET_CHECK("ev_check:subscreen");
  //新サブスクリーンからのイベント起動チェック
  if(WIPE_SYS_EndCheck()){
    event = FIELD_SUBSCREEN_EventCheck( FIELDMAP_GetFieldSubscreenWork(fieldWork), req.subscreenRequest );
    if(event != NULL){
      return event;
    }
  }

#ifdef BUGFIX_GFBTS1975_100713
  if( req.subscreenRequest && WIPE_SYS_EndCheck() ){
    //Gパワー効果終了チェック
    event = CheckGPowerEffectEnd( gsys );
    if( event != NULL ){
      return event;
    }
  }
#else
  //Gパワー効果終了チェック
  event = CheckGPowerEffectEnd( gsys );
  if( event != NULL ){
    return event;
  }
#endif
  
  SET_CHECK("ev_check:gpower & intrude");
  //パレス座標イベント
  event =  Intrude_CheckPosEvent(fieldWork, gsys, req.field_player);
  if(event != NULL){
    return event;
  }
#ifdef  DEBUG_SPEED_CHECK_ENABLE
  if ( (req.key_trg & PAD_BUTTON_R) || (req.key_cont & PAD_BUTTON_L) )
  {
    OSTick end_tick;
    TAIL_CHECK(&end_tick);
    OS_TPrintf("ev_check:total:%08x", end_tick);
    OS_TPrintf("(%d msec)\n", OS_TicksToMicroSeconds(end_tick) );
    PUT_CHECK();
  }
#endif

  return NULL;
}

//--------------------------------------------------------------
/**
 * イベント起動チェック と, イベント生成時の処理
 * @param gsys GAMESYS_WORK
 * @param work FIELDMAP_WORK
 * @retval GMEVENT NULL=イベント無し
 */
//--------------------------------------------------------------
GMEVENT * FIELD_EVENT_CheckNormal_Wrap( GAMESYS_WORK *gsys, void *work )
{
  GMEVENT *event;
  FIELD_PLACE_NAME *place_name_sys;
  FIELDMAP_WORK * fieldmap_work = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_ENCOUNT* encount = FIELDMAP_GetEncount(fieldmap_work); 
  FIELD_CROWD_PEOPLE* crowdpeople;
  BOOL eff_delete_flag,menu_open_flag;
  //エフェクトエンカウントの　OBJとの接触によるエフェクト破棄チェック
  EFFECT_ENC_CheckObjHit( encount );

  // イベント起動チェック
  event = FIELD_EVENT_CheckNormal( gsys, work, &eff_delete_flag, &menu_open_flag );

  // イベント生成時の処理
  if( event != NULL )
  {
    if (eff_delete_flag)
    {
      EFFECT_ENC_EffectDelete( encount );
    }
    // 地名表示ウィンドウを消去
    place_name_sys  = FIELDMAP_GetPlaceNameSys( fieldmap_work );
    FIELD_PLACE_NAME_Hide( place_name_sys );
    
    // 自機が特殊表示の場合は元に戻す
    FIELD_PLAYER_CheckSpecialDrawForm(
        FIELDMAP_GetFieldPlayer(fieldmap_work), menu_open_flag );
    
    // 自機が特殊移動の際は元に戻す
    FIELD_PLAYER_ResetSpecialMove( FIELDMAP_GetFieldPlayer(fieldmap_work) );
    
    // 群集 windowを消去
    crowdpeople = FIELDMAP_GetCrowdPeopleSys( fieldmap_work );
    if( crowdpeople ){
      FIELD_CROWD_PEOPLE_ClearSubWindow( crowdpeople );
    }
    // トランシーバー音を消す
    stopTVTSE( gsys, fieldmap_work );
  }
  return event;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * イベント起動チェック：ユニオンorコロシアム
 *
 * @param   gsys    
 * @param   work    
 *
 * @retval  GMEVENT *   
 */
//--------------------------------------------------------------
GMEVENT * FIELD_EVENT_CheckUnion( GAMESYS_WORK *gsys, void *work )
{
  EV_REQUEST req;
  GMEVENT *event;
  FIELDMAP_WORK *fieldWork = work;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  UNION_SYSTEM_PTR unisys = GameCommSys_GetAppWork(game_comm);
  
  setupRequest( &req, gsys, fieldWork );

  if(unisys == NULL){
    //ユニオン終了
    if(NetErr_App_CheckError()){  //エラーが発生したままの状態の時はエラー画面を表示
      if(WIPE_SYS_EndCheck()){
        return EVENT_FieldCommErrorProc(gsys, fieldWork);
      }
      return NULL;
    }
    return EVENT_ChangeMapFromUnion( gsys, fieldWork );
  }
  
  if(UnionMain_GetFinishReq(unisys) == TRUE){
    if(UnionMain_GetFinishExe(unisys) == FALSE){
      //ユニオン終了処理開始
      GameCommSys_ExitReq(game_comm);
    }
    return NULL;
  }
  
  //デバッグ用チェック
#ifdef  PM_DEBUG
  event = DEBUG_checkKeyEvent( &req, gsys, fieldWork );
  if (event != NULL) {
    return event;
  }
#endif //debug
  
//☆☆☆特殊スクリプト起動チェックがここに入る

//☆☆☆一歩移動チェックがここから
  //座標イベントチェック
  if( req.moveRequest )
  {
    GMEVENT * event = checkPosEvent_core( &req, DIR_NOT );
    if ( event != NULL )
    {
      return event;
    }

#if 0
   //座標接続チェック
    event = checkExit(&req, fieldWork, req.now_pos);
    if( event != NULL ){
      return event;
    }
#endif
  }

//☆☆☆自機状態イベントチェックがここから
    /* 今はない */

//☆☆☆会話チェック

  //通信用会話処理
  {
    //通信イベント発生チェック
    event = UnionEvent_CommCheck( gsys, fieldWork );
    if(event != NULL){
      return event;
    }
    
    //通信話しかけイベント発生チェック
    if(req.talkRequest){
      event = UnionEvent_TalkCheck( gsys, fieldWork );
      if(event != NULL){
        return event;
      }
    }
  }

  //フィールド話し掛けチェック
  if( req.talkRequest )
  {
    if(UnionMain_CheckPlayerFreeMode(gsys) == TRUE)
    { //OBJ話し掛け
      MMDL *fmmdl_talk = getFrontTalkOBJ( &req, fieldWork );
      if( fmmdl_talk != NULL )
      {
        u32 scr_id = MMDL_GetEventID( fmmdl_talk );
        if ( SCRIPT_IsValidScriptID( scr_id ) == TRUE )
        {
          MMDL *fmmdl_player = FIELD_PLAYER_GetMMdl( req.field_player );
          if(UNION_CHARA_CheckCommPlayer(MMDL_GetOBJID(fmmdl_talk)) == FALSE){
            FIELD_PLAYER_ForceStop( req.field_player );
            //*eff_delete_flag = TRUE;  //エフェクトエンカウント消去リクエスト
            return EVENT_FieldTalk( gsys, fieldWork,
              scr_id, fmmdl_player, fmmdl_talk, req.heapID );
          }
        }
      }
    }
  }

  //通信エラー画面呼び出しチェック(※メニュー起動チェックの真上に配置する事！)
  if( GAMESYSTEM_GetFieldCommErrorReq(gsys) == TRUE ){
    if(WIPE_SYS_EndCheck()){
      return EVENT_FieldCommErrorProc(gsys, fieldWork);
    }
  }

  //++++ ここから下は自機が完全に動作フリーな状態でなければチェックしない ++++
  if(UnionMain_CheckPlayerFreeMode(gsys) == FALSE){
    return NULL;
  }

  //メニュー起動チェック
  if( req.menuRequest ){
    if(WIPE_SYS_EndCheck()){
        return EVENT_UnionMapMenu( gsys, fieldWork, req.heapID );
    }
  }

  return NULL;
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * イベント起動チェック：ノーグリッドマップ
 *
 *  @param  gsys
 *  @param  work 
 *
 *  @return
 */
//--------------------------------------------------------------
GMEVENT * FIELD_EVENT_CheckNoGrid( GAMESYS_WORK *gsys, void *work )
{
  GMEVENT *event;
  FIELDMAP_WORK *fieldmap_work;
  FIELD_PLACE_NAME *place_name_sys;
  FIELD_CROWD_PEOPLE* crowdpeople;

  // イベント起動チェック
  event = eventCheckNoGrid( gsys, work );

  // イベント生成時の処理
  if( event != NULL )
  {
    // 地名表示ウィンドウを消去
    fieldmap_work   = (FIELDMAP_WORK*)GAMESYSTEM_GetFieldMapWork( gsys );
    place_name_sys  = FIELDMAP_GetPlaceNameSys( fieldmap_work );
    FIELD_PLACE_NAME_Hide( place_name_sys );

    // 群集 windowを消去
    crowdpeople = FIELDMAP_GetCrowdPeopleSys( fieldmap_work );
    if( crowdpeople ){
      FIELD_CROWD_PEOPLE_ClearSubWindow( crowdpeople );
    }
    // トランシーバー音を消す
    stopTVTSE( gsys, fieldmap_work );
  }

  return event;
}

//--------------------------------------------------------------
/**
 *  @brief  ノーグリッドマップ、　イベントチェック
 */
//--------------------------------------------------------------
static GMEVENT * eventCheckNoGrid( GAMESYS_WORK *gsys, void *work )
{
  EV_REQUEST req;
  GMEVENT *event;
  FIELDMAP_WORK *fieldWork = work;
  RAIL_LOCATION front_location;
  const MMDL *player_fmmdl;

  INIT_CHECK();
  

  SET_CHECK("ev_check:setupRequest");
  setupRequest( &req, gsys, fieldWork );


  player_fmmdl = FIELD_PLAYER_GetMMdl( req.field_player );

  // 目の前のロケーションを取得
  SET_CHECK("ev_check:calc front location");
  MMDL_GetRailFrontLocation( player_fmmdl, &front_location );

  
  SET_CHECK("ev_check:debug_key_event");
  //デバッグ用チェック
#ifdef  PM_DEBUG
  event = DEBUG_checkKeyEvent( &req, gsys, fieldWork );
  if (event != NULL) {
    return event;
  }
#endif //debug

  SET_CHECK("ev_check:special event");
//☆☆☆特殊スクリプト起動チェックがここに入る
#ifdef  PM_DEBUG
  if (DEBUG_FLG_GetFlg(DEBUG_FLG_DisableEvents) == FALSE)
#endif
  {
    event = checkSpecialEvent( &req );
    if ( event )
    {
      return event;
    }
  }


#ifdef PM_DEBUG
  GF_ASSERT_MSG(DebugBGInitEnd,"ERROR:BG NOT INITIALIZE");    //BG初期化監視フラグ
  GF_ASSERT_MSG(!MapFadeReqFlg,"ERROR:CALLED MAP FADE REQ");   //マップフェードリクエストフラグ
#endif  


  SET_CHECK("ev_check:trainer eye");
//☆☆☆トレーナー視線チェックがここに入る
  if( !(req.debugRequest) ){
/*
    event = EVENT_CheckTrainerEye( fieldWork, FALSE );
    if( event != NULL ){
      return event;
    }
//*/
  }


  SET_CHECK("ev_check:move event");
//☆☆☆一歩移動チェックがここから
  //歩数をカウント
  if( req.moveRequest )
  {
    incWalkCount( req.gamedata );
  }
  //座標イベントチェック
  if( req.moveRequest )
  {
    //*
    RAIL_LOCATION pos;
    const MMDL* player = FIELD_PLAYER_GetMMdl( req.field_player );
    u16 id;
    EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
    MMDL_GetRailLocation( player, &pos );
    
    id = EVENTDATA_CheckPosEventRailLocation( req.evdata, evwork, &pos );
    
    if( id != EVENTDATA_ID_NONE ){ //座標イベント起動
      event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
      return event;
    }
    //*/

    //座標接続チェック
    event = checkRailExit(&req, gsys, fieldWork);
    if( event != NULL ){
      return event;
    }

    //汎用一歩移動イベントチェック群
    event = checkMoveEvent(&req, fieldWork);
    if ( event != NULL) {
      return event;
    }
  }



  
  SET_CHECK("ev_check:step event");
//☆☆☆ステップチェック（一歩移動or振り向き）がここから
  //戦闘移行チェック
  {
    GMEVENT* enc_event = checkNormalEncountEvent( &req, gsys, fieldWork );
    if(enc_event != NULL){
      return enc_event;
    }
  }

  
//☆☆☆自機状態イベントチェックがここから
    /* 今はない */

  SET_CHECK("ev_check:talk event");
//☆☆☆会話チェック
  //フィールド話し掛けチェック
  if( req.talkRequest )
  {
    { //OBJ話し掛け
      MMDL *fmmdl_talk = getRailFrontTalkOBJ( &req, fieldWork );
      if( fmmdl_talk != NULL )
      {
        u32 scr_id = MMDL_GetEventID( fmmdl_talk );
        if ( SCRIPT_IsValidScriptID( scr_id ) == TRUE )
        {
          MMDL *fmmdl_player = FIELD_PLAYER_GetMMdl( req.field_player );
          
          FIELD_PLAYER_ForceStop( req.field_player );
          return EVENT_FieldTalk( gsys, fieldWork,
            scr_id, fmmdl_player, fmmdl_talk, req.heapID );
        }
      }
    }
    
    { //BG話し掛け
      u16 id;
      RAIL_LOCATION pos;
      EVENTWORK *evwork = GAMEDATA_GetEventWork( req.gamedata );
      u16 dir = MMDL_GetDirDisp( player_fmmdl );
      
      pos = front_location;
      id = EVENTDATA_CheckTalkBGEventRailLocation( req.evdata, evwork, &pos, dir );
      
      if( id != EVENTDATA_ID_NONE ){ //座標イベント起動
        event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
        return event;
      }
    }
    /*
    { //BG Attribute 話しかけ
      u16 id = checkTalkAttrEvent( &req, fieldWork );
      if( id != EVENTDATA_ID_NONE ){ //座標イベント起動
        event = SCRIPT_SetEventScript( gsys, id, NULL, req.heapID );
        return event;
      }

    }
    */
  }



  SET_CHECK("ev_check:push event");
//☆☆☆押し込み操作チェック（マットでのマップ遷移など）
  //キー入力接続チェック
  if (req.pushRequest) {

    // 目の前が交通不可能出ない場合にはチェックしない
    if( !checkRailFrontMove( FIELDMAP_GetFieldPlayer( fieldWork ), &req ) )
    {
      event = checkRailPushExit(&req, gsys, fieldWork, &front_location);
      if( event != NULL ){
        return event;
      }

    }
  }
  event = checkRailSlipDown(&req, gsys, fieldWork);
  if( event != NULL ){
    return event;
  }

  
  SET_CHECK("ev_check:menu");
//☆☆☆自機位置に関係ないキー入力イベントチェック
  //便利ボタンチェック
  if( req.convRequest ){
    if(WIPE_SYS_EndCheck()){
      FIELD_SUBSCREEN_WORK* subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
      //メニューの終了後の１フレーム程度にYボタンを起動すると、
      //先にYボタンメニューの初期化が走り、その後メニューの終了処理で上画面のスクリーンをクリアして
      //しまうため、メニューの終了を待ってから動作させる
      if( EVENT_ShortCutMenu_IsOpen(gsys) && FIELD_SUBSCREEN_CanChange( subscreen) )
      { 
        event = EVENT_ShortCutMenu( gsys, fieldWork, req.heapID );
        if( event != NULL ){
          return event;
        }
      }
    }
  }
  
  //通信エラー画面呼び出しチェック(※メニュー起動チェックの真上に配置する事！)
  if( GAMESYSTEM_GetFieldCommErrorReq(gsys) == TRUE ){
#ifdef BUGFIX_GFBTS1975_100713
    if(WIPE_SYS_EndCheck() && req.commRequest){
#else
    if(WIPE_SYS_EndCheck()){
#endif
      return EVENT_FieldCommErrorProc(gsys, fieldWork);
    }
  }

  //メニュー起動チェック
  if( req.menuRequest ){
    if(WIPE_SYS_EndCheck()){
      {
        FIELD_SUBSCREEN_WORK* subscreen = FIELDMAP_GetFieldSubscreenWork(fieldWork);
        if( FIELD_SUBSCREEN_GetMode( subscreen ) == FIELD_SUBSCREEN_DOWSING )  // ダウジングマシンを使っているときは、ダウジングマシンを終了させるだけ
        {
          return NULL;
        }
      }
        return EVENT_FieldMapMenu( gsys, fieldWork, req.heapID );
    }
  }

  SET_CHECK("ev_check:subscreen");
  //新サブスクリーンからのイベント起動チェック
  {
    u8 ev_ok = FALSE;
    if( req.subscreenRequest && WIPE_SYS_EndCheck() ){
      ev_ok = TRUE;
    }
    event = FIELD_SUBSCREEN_EventCheck( FIELDMAP_GetFieldSubscreenWork(fieldWork), ev_ok );
    if(event != NULL){
      return event;
    }
  }

#ifdef BUGFIX_GFBTS1975_100713
  //Gパワー効果終了チェック
  if( req.subscreenRequest && WIPE_SYS_EndCheck() ){
    event = CheckGPowerEffectEnd( gsys );
    if( event != NULL ){
      return event;
    }
  }
#else
  //Gパワー効果終了チェック
  event = CheckGPowerEffectEnd( gsys );
  if( event != NULL ){
    return event;
  }
#endif

#ifdef  DEBUG_SPEED_CHECK_ENABLE
  if ( (req.key_trg & PAD_BUTTON_R) || (req.key_cont & PAD_BUTTON_L) )
  {
    OSTick end_tick;
    TAIL_CHECK(&end_tick);
    OS_TPrintf("ev_check rail:total:%08x", end_tick);
    OS_TPrintf("(%d msec)\n", OS_TicksToMicroSeconds(end_tick) );
    PUT_CHECK();
  }
#endif
  
  /*  
  //パレス座標イベント
  event =  Intrude_CheckPosEvent(fieldWork, gsys, req.field_player);
  if(event != NULL){
    return event;
  }
  //*/
  return NULL;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 *  @brief  イベント起動チェック：ハイブリッドマップ
 */
//--------------------------------------------------------------
GMEVENT * FIELD_EVENT_CheckHybrid( GAMESYS_WORK *gsys, void *work )
{
  FIELDMAP_WORK *fieldWork = work;
  u32 base_system;
  GMEVENT * event;
  
  // 現在のBaseシステムを取得 
  base_system = FIELDMAP_GetBaseSystemType( fieldWork );
  
  if( base_system == FLDMAP_BASESYS_GRID )
  {
    event = FIELD_EVENT_CheckNormal_Wrap( gsys, work );
  }
  else
  {
    event = FIELD_EVENT_CheckNoGrid( gsys, work );
  }

  return event;
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief 判定用パラメータの生成
 *
 *
 * @note
 * 現状チェックごとに生成しているが実はほとんどの値は初期化時に一度だけでもいいのかも。
 */
//--------------------------------------------------------------
static void setupRequest(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork)
{
  GFL_STD_MemClear( req, sizeof(EV_REQUEST) );
  req->heapID = FIELDMAP_GetHeapID(fieldWork);
  req->gsys = gsys;
  req->gamedata = GAMESYSTEM_GetGameData(gsys);
  req->evdata = GAMEDATA_GetEventData(req->gamedata);
  req->map_id = FIELDMAP_GetZoneID(fieldWork);

  req->key_trg = GFL_UI_KEY_GetTrg();
  req->key_cont = GFL_UI_KEY_GetCont();
  req->key_direction = DIR_NOT;
  if ( req->key_cont & PAD_KEY_UP    ) {
    req->key_direction = DIR_UP;
  } else if ( req->key_cont & PAD_KEY_DOWN  ) {
    req->key_direction = DIR_DOWN;
  } else if ( req->key_cont & PAD_KEY_LEFT  ) {
    req->key_direction = DIR_LEFT;
  } else if ( req->key_cont & PAD_KEY_RIGHT ) {
    req->key_direction = DIR_RIGHT;
  }


  req->fieldWork = fieldWork;
  req->field_player = FIELDMAP_GetFieldPlayer( fieldWork );
  //自機動作ステータス更新
  FIELD_PLAYER_UpdateMoveStatus( req->field_player );
  //自機の動作状態を取得
  req->player_value = FIELD_PLAYER_GetMoveValue( req->field_player );
  req->player_state = FIELD_PLAYER_GetMoveState( req->field_player );
  {
    MMDL *fmmdl = FIELD_PLAYER_GetMMdl( req->field_player );
    req->player_dir = MMDL_GetDirDisp( fmmdl );
    req->now_pos = MMDL_GetVectorPosAddress( fmmdl );
  }
  req->reserved_scr_id = FIELD_STATUS_GetReserveScript( GAMEDATA_GetFieldStatus(req->gamedata) );
  
  // アトリビュート取得
  req->mapattr = FIELD_PLAYER_GetMapAttr( req->field_player );

  if (req->player_state == PLAYER_MOVE_STATE_END || req->player_state == PLAYER_MOVE_STATE_OFF )
  {
#ifdef BUGFIX_GFBTS1975_100713
    //通信イベントを起動していいフラグをOn
    req->commRequest = TRUE;
#endif

    req->talkRequest = ((req->key_trg & PAD_BUTTON_A) != 0);

    req->menuRequest = ((req->key_trg & PAD_BUTTON_X) != 0);

    //作成中だと止まったりするので、一時的に入力不可にします
    req->convRequest = ((req->key_trg & PAD_BUTTON_Y) != 0);

    {
      //サブスクリーンイベント起動許可チェック
      u8 input = 0;

      input += req->talkRequest;
      input += req->menuRequest;
      input += req->convRequest;
      if( input == 0 ){
        req->subscreenRequest = TRUE;
      }
    }
  }
  
  req->moveRequest = 0;
  req->exMoveRequest = 0;

  if( req->player_value == PLAYER_MOVE_VALUE_WALK ){
    req->moveRequest = ((req->player_state == PLAYER_MOVE_STATE_END));
  }
  if(FIELD_PLAYER_CheckNaminoriEventEnd( req->field_player )){
    req->exMoveRequest = TRUE; 
  }
  req->stepRequest = ((req->player_state == PLAYER_MOVE_STATE_END));

  req->dirPosRequest = FALSE;
  if ( req->player_state != PLAYER_MOVE_STATE_END && req->player_value == PLAYER_MOVE_VALUE_TURN )
  {
    req->dirPosRequest = TRUE;
  } 
  if ( ( (req->player_dir == DIR_UP) && (req->key_cont & PAD_KEY_UP) )
      || ( (req->player_dir == DIR_DOWN) && (req->key_cont & PAD_KEY_DOWN) )
      || ( (req->player_dir == DIR_LEFT) && (req->key_cont & PAD_KEY_LEFT) )
      || ( (req->player_dir == DIR_RIGHT) && (req->key_cont & PAD_KEY_RIGHT) )
     )
  {
    req->pushRequest = TRUE;
  }
  else 
  {
    req->pushRequest = FALSE;
  }


#ifdef  PM_DEBUG
  req->debugRequest = ( (req->key_cont & PAD_BUTTON_R) != 0);
  if (req->debugRequest)
  {
    req->talkRequest = FALSE;
    //req->menuRequest = FALSE;
    req->stepRequest = FALSE;
    req->moveRequest = FALSE;
    req->pushRequest = FALSE;
    req->convRequest = FALSE;
  }
    
  /*if (req->key_cont & PAD_BUTTON_L) {
    TAMADA_Printf("KEY:%04x PSTATE:%d PVALUE:%d DIR:%d\n",
        req->key_cont & 0xff, req->player_state, req->player_value, req->player_dir );
  }*/
#endif
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/// トランシーバー音を消す
//--------------------------------------------------------------
static void stopTVTSE( GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap )
{
  FIELD_SUBSCREEN_WORK * subscreen = FIELDMAP_GetFieldSubscreenWork( fieldmap );
  C_GEAR_WORK * cgear = FIELD_SUBSCREEN_GetCGearWork( subscreen );

  //Cギアじゃないか、Cギアからワイヤレスに移行するイベントでない場合
  if( cgear == NULL ){
    
    // Cギアじゃない
    FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( GAMESYSTEM_GetGameData(gsys) );
    FSND_StopTVTRingTone( fsnd );
    
  }else if ( CGEAR_IsDoEventWireless( cgear ) == FALSE ){
    
    // Cギアからwireless以外への遷移
    FIELD_SOUND* fsnd = GAMEDATA_GetFieldSound( GAMESYSTEM_GetGameData(gsys) );
    FSND_StopTVTRingTone( fsnd );
    CGEAR_SetStopTVTRingTone( cgear );
    
  }
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/// 歩数カウンタをインクリメントする
//--------------------------------------------------------------
static void incWalkCount( GAMEDATA * gamedata )
{
  RECORD* record = GAMEDATA_GetRecordPtr( gamedata );
  RECORD_Inc( record, RECID_WALK_COUNT );
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * @brief 特殊スクリプト起動チェック
 *
 * @param req EV_REQUEST
 *
 * @return 起動したPOSイベント
 */
//--------------------------------------------------------------
static GMEVENT * checkSpecialEvent( EV_REQUEST * req )
{
  GMEVENT * event;
  if (req->reserved_scr_id != SCRID_NULL)
  {
    FIELD_STATUS_SetReserveScript( GAMEDATA_GetFieldStatus(req->gamedata), SCRID_NULL );
    event = SCRIPT_SetEventScript( req->gsys, req->reserved_scr_id, NULL, req->heapID );
    return event;
  }
  event = SCRIPT_SearchSceneScript( req->gsys, req->heapID );

  if (event){
    return event;
  }
  return NULL;
}

//======================================================================
//
//
//  イベント起動チェック：POSイベント関連
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * @brief POSイベントチェック
 *
 * @param req EV_REQUEST
 *
 * @return 起動したPOSイベント
 */
//--------------------------------------------------------------
static GMEVENT* checkPosEvent( EV_REQUEST* req )
{
  GMEVENT* event;

  // 流砂POSイベントチェック
  event = checkPosEvent_sandstream( req );
  if( event != NULL ) { 
    return event;
  }

  // 通常POSイベントチェック
  return checkPosEvent_core( req, DIR_NOT );
}
//--------------------------------------------------------------
/**
 * @brief POSイベントチェック
 * @param req EV_REQUEST
 * @param dir チェックする向きの指定
 * @return 起動したPOSイベント
 */
//--------------------------------------------------------------
static GMEVENT* checkPosEvent_core( EV_REQUEST * req, u16 dir )
{
  u16 id;
  VecFx32 pos;
  MAPATTR_VALUE attrval;
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( req->gsys );
  EVENTWORK *evwork = GAMEDATA_GetEventWork( req->gamedata ); 
  FIELD_PLAYER_GetPos( req->field_player, &pos );

  // アトリビュートをチェック
  { 
    FLDMAPPER* mapper = FIELDMAP_GetFieldG3Dmapper( fieldmap );
    MAPATTR attr = MAPATTR_GetAttribute( mapper, &pos );
    attrval = MAPATTR_GetAttrValue( attr );
  }
  // 流砂アトリビュートにいる場合はPOSイベントを無視する
  if( MAPATTR_VALUE_CheckSandStream(attrval) == TRUE ){ return NULL; }

  id = EVENTDATA_CheckPosEvent( req->evdata, evwork, &pos, dir ); 
  if( id != EVENTDATA_ID_NONE ) 
  { //座標イベント起動
    GMEVENT* event = SCRIPT_SetEventScript( req->gsys, id, NULL, req->heapID );
    return event;
  } 
  return NULL;
}

//--------------------------------------------------------------
/**
 * @brief POSイベントチェック：向き指定あり
 * @param req EV_REQUEST
 * @return 起動したPOSイベント
 *
 * @todo
 * 氷状態で強制移動・方向固定のままPOSに踏み込む状況で、キーを
 * 押しっぱなしにしていると発動しないタイミングが発見されている
 * （現状はそのような地形をなくすことで対処した）
 */
//--------------------------------------------------------------
static GMEVENT * checkPosEvent_OnlyDirection( EV_REQUEST * req )
{
  GMEVENT * event;
  u16 next_dir;

  //現在の向きに適合するイベントの有無をチェック
  event = checkPosEvent_core( req, req->player_dir );
  if (event)
  {
    return event;
  }

  next_dir = FIELD_PLAYER_GetKeyDir( req->field_player, req->key_cont );
  if ( next_dir == DIR_NOT || next_dir == req->player_dir )
  {
    return NULL;
  }
  //現在の向き意外に、キーの先読みで自機が向く方向もチェック
  event = checkPosEvent_core( req, next_dir );
  if ( event )
  {
    OS_Printf( " dir pos event!!\n" );
    return event;
  }
  return NULL;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * checkPosEvent_prefetchDirection( EV_REQUEST * req )
{
  GMEVENT * event;
  u16 key_dir = DIR_NOT;
  int st = req->player_state;
  int val = req->player_value;
  //if ( req->player_state == PLAYER_MOVE_STATE_END ) return NULL;
  //if ( req->player_value == PLAYER_MOVE_VALUE_WALK ) return NULL;

#ifdef  PM_DEBUG
  if ( req->debugRequest ) return NULL;
#endif

  if ( req->key_cont & PAD_KEY_UP    ) {
    key_dir = DIR_UP;
  } else if ( req->key_cont & PAD_KEY_DOWN  ) {
    key_dir = DIR_DOWN;
  } else if ( req->key_cont & PAD_KEY_LEFT  ) {
    key_dir = DIR_LEFT;
  } else if ( req->key_cont & PAD_KEY_RIGHT ) {
    key_dir = DIR_RIGHT;
  }

  if ( key_dir == req->player_dir ) return NULL;
  if ( key_dir == DIR_NOT ) return NULL;

  switch ( st )
  {
  //case PLAYER_MOVE_STATE_OFF:
  case PLAYER_MOVE_STATE_START:
  case PLAYER_MOVE_STATE_ON:
    return NULL;
  //case PLAYER_MOVE_STATE_END:
  }

  event = checkPosEvent_core( req, key_dir );

  return event;
}

//--------------------------------------------------------------
/**
 * @brief POSイベントチェック：キーの先読み
 * @param req EV_REQUEST
 * @return 起動したPOSイベント
 *
 * Pushイベントでは抜けるタイミングがあるのでPOSイベントのキー先読みでチェックしている
 */
//--------------------------------------------------------------
static GMEVENT * checkPosEvent_IntrudeDirection( EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  GMEVENT * event = NULL;
  u16 next_dir;

  if(ZONEDATA_IsPalace(req->map_id) == FALSE){
    return NULL;
  }
  
#if 0
  if(req->player_dir != DIR_NOT){
    event = Intrude_CheckPushEvent(gsys, fieldWork, req->field_player, req->now_pos, req->player_dir);
    if(event != NULL){
      return event;
    }
  }
#endif

  //キーを先読み
  next_dir = FIELD_PLAYER_GetKeyDir( req->field_player, req->key_cont );
  if ( next_dir == DIR_NOT )//|| next_dir == req->player_dir )
  {
    return NULL;
  }

  return Intrude_CheckPushEvent(gsys, fieldWork, req->field_player, req->now_pos, req->player_dir, next_dir);
}

//--------------------------------------------------------------
/**
 * @brief 流砂アトリビュートチェック
 *
 * @param req EV_REQUEST
 *
 * @return 足元が流砂アトリビュートだった場合,
 *         以下のどちらかの条件に合致する場合にPOSイベントを検索し, 
 *         実行すべきイベントを返す.
 *
 *         条件1.POSイベントの真上にいる
 *         条件2.周囲2グリッド以内にPOSイベントが存在する and
 *               (自転車に乗っている or 走っている)
 *
 *         実行すべきイベントが無い場合, NULL を返す.
 */
//--------------------------------------------------------------
static GMEVENT* checkPosEvent_sandstream( EV_REQUEST* req )
{
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( req->gsys );
  FIELD_PLAYER*  player   = FIELDMAP_GetFieldPlayer( fieldmap );

  const POS_EVENT_DATA* pos_event;
  VecFx32 pos;

  FIELD_PLAYER_GetPos( player, &pos );

  // アトリビュートをチェック
  {
    FLDMAPPER*    mapper  = FIELDMAP_GetFieldG3Dmapper( fieldmap );
    MAPATTR       attr    = MAPATTR_GetAttribute( mapper, &pos );
    MAPATTR_VALUE attrval = MAPATTR_GetAttrValue( attr );
    if( MAPATTR_VALUE_CheckSandStream(attrval) != TRUE ) { return NULL; } // 流砂アトリビュートじゃない
  }

  // 流砂POSイベントを取得
  {
    EVENTWORK* evwork = GAMEDATA_GetEventWork( req->gamedata );
    pos_event = EVENTDATA_GetPosEvent_XZ( req->evdata, evwork, &pos, DIR_NOT );
    if( !pos_event ) { return NULL; } // POSイベントが無い
    if( pos_event->pos_type != EVENTDATA_POSTYPE_GRID ) { return NULL; } // グリッド以外のPOSイベント発見
  }

  // 流砂の中心にいるかどうかチェック
  {
    const POS_EVENT_DATA_GPOS* pos_event_gpos;
    int centerGX, centerGZ;
    pos_event_gpos = (const POS_EVENT_DATA_GPOS*)pos_event->pos_buf;
    centerGX = pos_event_gpos->gx + pos_event_gpos->sx/2;
    centerGZ = pos_event_gpos->gz + pos_event_gpos->sz/2;
    // 中心にいたらPOSイベント起動
    if( (centerGX == FX32_TO_GRID(pos.x)) &&
        (centerGZ == FX32_TO_GRID(pos.z)) ) { 
      GMEVENT* event = SCRIPT_SetEventScript( req->gsys, pos_event->id, NULL, req->heapID );
      return event;
    }
  }

  // 主人公の状態チェック
  {
    PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( player );
    MMDL*            mmdl = FIELD_PLAYER_GetMMdl( player );
    u16              acmd = MMDL_GetAcmdCode( mmdl );
    OS_TFPrintf( 3, "acmd = 0x%x\n", acmd );
    // 歩き以外ならPOSイベント起動
    if( (form != PLAYER_MOVE_FORM_NORMAL) ||
        (acmd == AC_DASH_U_4F) || (acmd == AC_DASH_D_4F) ||
        (acmd == AC_DASH_L_4F) || (acmd == AC_DASH_R_4F) ||
        (acmd == AC_DASH_U_6F) || (acmd == AC_DASH_D_6F) ||
        (acmd == AC_DASH_L_6F) || (acmd == AC_DASH_R_6F) )
    { 
      GMEVENT* event = SCRIPT_SetEventScript( req->gsys, pos_event->id, NULL, req->heapID );
      return event;
    }
  }

  return NULL;
}

//======================================================================
//
//
//    イベント起動チェック：一歩移動関連
//
//
//======================================================================
//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * checkMoveEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork)
{
  HEAPID     heap_id = FIELDMAP_GetHeapID( fieldWork );
  GAMESYS_WORK* gsys = FIELDMAP_GetGameSysWork( fieldWork ); 
  GAMEDATA*    gdata = GAMESYSTEM_GetGameData( gsys );
  GMEVENT*     event = NULL;

  //波乗り開始時等の特殊一歩移動も検出
  if( req->moveRequest || req->exMoveRequest ){
  
    //エンカウントエフェクト起動監視
    event = CheckEffectEncount( fieldWork, gsys, gdata );
    if( event != NULL) return event;
  }

  //以下は通常一歩移動のみ検出
  if( !req->moveRequest ){
    return NULL;
  }
  updateFriendyStepCount( req->gamedata, fieldWork );

  //海底神殿チェック
  event = CheckSeaTemple( gsys, fieldWork, req->map_id );
  if( event != NULL) return event;

  //育て屋チェック
  event = CheckSodateya( req, fieldWork, gsys, gdata );
  if( event != NULL) return event;

  //虫除けスプレーチェック
  event = CheckSpray( fieldWork, gsys, gdata );
  if( event != NULL) return event;

  return NULL;
}

//--------------------------------------------------------------
/**
 * @brief 孵化カウンタの更新値を取得する
 *
 * @param party 手持ちポケモン
 *
 * @return 孵化カウンタの更新値
 */
//--------------------------------------------------------------
static u8 getHatchCountUpdateValue( POKEPARTY* party )
{
  int i;
  int partyCount;

  partyCount = PokeParty_GetPokeCount( party );

  for( i=0; i<partyCount; i++ )
  {
    POKEMON_PARAM* param;
    u32 tokusei, tamago_flag;

    param = PokeParty_GetMemberPointer( party, i );
    tokusei = PP_Get( param, ID_PARA_speabino, NULL );
    tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );

    // タマゴは無視
    if( tamago_flag ) { continue; }

    // 特性「ほのおのからだ」「マグマのよろい」を持つポケモンがいる
    if( (tokusei == TOKUSYU_HONOONOKARADA) || (tokusei == TOKUSYU_MAGUMANOYOROI) ) {
      return 2;
    }
  }

  return 1;
}

//--------------------------------------------------------------
/**
 * @brief 手持ちタマゴの孵化カウンタを更新する
 *
 * @param party
 */
//--------------------------------------------------------------
static void updateEggHatchCount( POKEPARTY* party )
{
  int i;
  int partyCount;
  u8 sub_value;

  partyCount = PokeParty_GetPokeCount( party );
  sub_value  = getHatchCountUpdateValue( party );

  // 手持ちタマゴの残り歩数を減らす
  for( i=0; i<partyCount; i++ )
  {
    POKEMON_PARAM* pp;
    u32 tamagoFlag, friend;

    pp         = PokeParty_GetMemberPointer( party, i );
    tamagoFlag = PP_Get( pp, ID_PARA_tamago_flag, NULL );
    friend     = PP_Get( pp, ID_PARA_friend, NULL ); 

    // タマゴを発見
    if( tamagoFlag == TRUE ) { 
      // なつき度を更新
      if( friend < sub_value ) {
        friend = 0;
      }
      else {
        friend -= sub_value;
      }
      PP_Put( pp, ID_PARA_friend, friend );
    }
  }
}

//--------------------------------------------------------------
/**
 * @brief 手持ちタマゴの孵化カウント更新
 * @param party 更新対象のポケパーティー
 *
 * ※なつき度 = タマゴの場合は孵化までの残り歩数
 */
//--------------------------------------------------------------
static void updatePartyEgg( GAMEDATA * gamedata, POKEPARTY* party )
{
  enum {
    ONE_STEP_COUNT = 0x00100, ///<一歩あたりの増分
    MAX_STEP_COUNT = 0x10000, ///<カウント最大値
  };
  SAVE_CONTROL_WORK* saveData;
  SITUATION* situation;
  u32 count;

  // 状況データ取得
  saveData  = GAMEDATA_GetSaveControlWork( gamedata );
  situation = SaveData_GetSituation( saveData );

  // タマゴ孵化カウンタを更新
  Situation_GetEggStepCount( situation, &count );
  count += GPOWER_Calc_Hatch( ONE_STEP_COUNT );

  // 手持ちのタマゴに反映
  if( MAX_STEP_COUNT < count ) {
    updateEggHatchCount( party ); // タマゴのなつき度を更新
    count = 0; // カウンタ初期化
  }

  // セーブデータに反映
  Situation_SetEggStepCount( situation, count ); 
}
//--------------------------------------------------------------
/**
 * @brief 手持ちタマゴの孵化チェック
 * @param party チェック対象のポケパーティー
 * @return 孵化する卵がある場合TRUE
 *
 * ※タマゴの間は, なつき度を孵化歩数カウンタとして利用する.
 */
//--------------------------------------------------------------
static BOOL checkPartyEgg( POKEPARTY* party )
{
  int i;
  int pokeCount = PokeParty_GetPokeCount( party );

  for( i=0; i<pokeCount; i++ )
  {
    POKEMON_PARAM* param = PokeParty_GetMemberPointer( party, i );
    u32      tamago_flag = PP_Get( param, ID_PARA_tamago_flag, NULL );
    u32           friend = PP_Get( param, ID_PARA_friend, NULL ); 

    if( (tamago_flag == TRUE) && (friend == 0) ) 
    { // カウンタ0のタマゴを発見
      return TRUE;
    }
  } 
  return FALSE;
}

//--------------------------------------------------------------
/*
 *  @brief  育て屋イベントチェック
 */
//--------------------------------------------------------------
static GMEVENT* CheckSodateya( 
    const EV_REQUEST * req, FIELDMAP_WORK* fieldmap, GAMESYS_WORK* gameSystem, GAMEDATA* gameData )
{
  HEAPID     heapID   = FIELDMAP_GetHeapID( fieldmap );
  POKEPARTY* party    = GAMEDATA_GetMyPokemon( gameData );
  SODATEYA*  sodateya = FIELDMAP_GetSodateya( fieldmap );

  // 育て屋: 経験値加算, 子作り判定など1歩分の処理
  if ( SODATEYA_BreedPokemon( sodateya ) == TRUE )
  {
    // タマゴが生まれた時の処理
  }

  // 手持ちタマゴ: 孵化カウンタ更新
  updatePartyEgg( gameData, party );

  // 手持ちタマゴ: 孵化チェック
  if( checkPartyEgg( party ) )
  {
    return SCRIPT_SetEventScript( gameSystem, SCRID_EGG_BIRTH, NULL, heapID );
  }
  return NULL;
}

//--------------------------------------------------------------
/**
 * @brief なつき度上昇処理
 * @param gamedata
 * @param fieldmap
 */
//--------------------------------------------------------------
static void updateFriendyStepCount( GAMEDATA * gamedata, FIELDMAP_WORK * fieldmap )
{
  enum {
    FRIENDLY_STEP_MAX = 128,
  };
  SITUATION * sit = SaveData_GetSituation( GAMEDATA_GetSaveControlWork( gamedata ) );
  u16 step_count;
  BOOL result = FALSE;
  step_count = Situation_GetFriendlyStepCount( sit );
  step_count ++;
  if ( step_count >= FRIENDLY_STEP_MAX )
  {
    step_count = 0;
    result = TRUE;
  }
  Situation_SetFriendlyStepCount( sit, step_count );
  if ( result )
  {
    POKEPARTY * party = GAMEDATA_GetMyPokemon( gamedata );
    u16 zone_id = FIELDMAP_GetZoneID( fieldmap );
    HEAPID heapID = FIELDMAP_GetHeapID( fieldmap );
    int count = PokeParty_GetPokeCount( party );
    int i;

    for ( i = 0; i < count; i++ )
    {
      POKEMON_PARAM * pp = PokeParty_GetMemberPointer( party, i );
      NATSUKI_CalcTsurearuki( pp, zone_id, heapID );
    }
  }
}



//======================================================================
//======================================================================
//----------------------------------------------------------------------------
/**
 *  @brief  海底神殿　歩数イベントチェック
 *
 *  @param  gsys  ゲームシステム
 *
 *  @return ゲームイベント
 */
//-----------------------------------------------------------------------------
static GMEVENT* CheckSeaTemple( GAMESYS_WORK* gsys, FIELDMAP_WORK* fieldmap, u16 zone_id )
{
  GAMEDATA* gdata = GAMESYSTEM_GetGameData( gsys );
  SAVE_CONTROL_WORK* saveData = GAMEDATA_GetSaveControlWork(gdata);
  SITUATION* situation = SaveData_GetSituation( saveData );
  u16 count;
  GMEVENT * event = NULL;

  if( ZONEDATA_IsSeaTempleDungeon( zone_id ) ){
    // 歩数のスクリプト起動
    event = EVENT_SeaTemple_GetStepEvent( gsys, fieldmap );
  
    // 歩数カウント
    count = Situation_GetSeaTempleStepCount( situation );
    count ++;
    Situation_SetSeaTempleStepCount( situation, count );
  }

  return event;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * 1歩ごとのスプレー効果減算
 */
//--------------------------------------------------------------
static GMEVENT* CheckSpray( FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata )
{
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork(gdata);
  u8 end_f;

  //セーブデータ取得
  end_f = EncDataSave_DecSprayCnt(EncDataSave_GetSaveDataPtr(save));
  //減算した結果0になったら効果切れメッセージ
  if( end_f ){
    OS_Printf("スプレー効果切れ\n");
     return SCRIPT_SetEventScript( gsys, SCRID_FLD_EV_SPRAY_EFFECT_END, NULL, FIELDMAP_GetHeapID( fieldWork ));
  }
  return NULL;
}

//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * 1歩ごとのエンカウントエフェクト起動チェック
 */
//--------------------------------------------------------------
static GMEVENT* CheckEffectEncount( FIELDMAP_WORK * fieldWork, GAMESYS_WORK* gsys, GAMEDATA* gdata )
{
  SAVE_CONTROL_WORK* save = GAMEDATA_GetSaveControlWork(gdata);
  FIELD_ENCOUNT * encount = FIELDMAP_GetEncount(fieldWork);
  GMEVENT* event = NULL;
  
  //エフェクトエンカウント接触チェック
  event = EFFECT_ENC_CheckEventApproch( encount );
  if( event != NULL){
    return event;
  }
  //エフェクトエンカウント起動チェック
  EFFECT_ENC_CheckEffectEncountStart( encount );
  return NULL;
}

//--------------------------------------------------------------
/**
 * リアルタイムクロックによるGパワー効果終了チェック
 */
//--------------------------------------------------------------
static GMEVENT* CheckGPowerEffectEnd( GAMESYS_WORK* gsys )
{
  if( GPOWER_Get_FinishWaitID() == GPOWER_ID_NULL ){
    return NULL;
  }
  return GMEVENT_CreateOverlayEventCall( gsys, FS_OVERLAY_ID( event_gpower), EVENT_GPowerEffectEnd, NULL );
}

//======================================================================
//
//
//    イベント起動チェック：デバッグ関連
//
//
//======================================================================
#ifdef  PM_DEBUG
//--------------------------------------------------------------
/**
 * @brief デバッグ用イベント起動チェック（主にキー入力による）
 */
//--------------------------------------------------------------
static GMEVENT * DEBUG_checkKeyEvent(EV_REQUEST * req, GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldWork)
{
  enum{
    resetCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START,
    chgCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_SELECT,

    railCont = PAD_BUTTON_R | PAD_BUTTON_B | PAD_BUTTON_A,

    fourkingsCont = PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_A,
  };

  //ソフトリセットチェック
  if( (req->key_cont & resetCont) == resetCont ){
#if 0
    return DEBUG_EVENT_GameEnd(gsys, fieldWork);
#endif
  }
  
  //マップ変更チェック
  if( (req->key_cont & chgCont) == chgCont ){
#if 0
    ISDPrintSetBlockingMode(1);
    GFL_HEAP_DEBUG_PrintSystemInfo();
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_PROC );
    GFL_HEAP_DEBUG_PrintExistMemoryBlocks( HEAPID_FIELDMAP );
    ISDPrintSetBlockingMode(0);
#endif
    //return DEBUG_EVENT_ChangeToNextMap(gsys, fieldWork);
  }
  
  //デバッグメニュー起動チェック
  if( req->menuRequest && req->debugRequest )
  {
    return DEBUG_EVENT_DebugMenu(gsys, fieldWork, 
        req->heapID, ZONEDATA_GetMapRscID(req->map_id));
  }

#if defined(DEBUG_ONLY_FOR_tomoya_takahashi) | defined(DEBUG_ONLY_FOR_nakatsui) | defined(DEBUG_ONLY_FOR_takahashi) | defined(DEBUG_ONLY_FOR_hozumi_yukiko) | defined(DEBUG_ONLY_FOR_iwao_kazumasa) | defined(DEBUG_ONLY_FOR_murakami_naoto) | defined(DEBUG_ONLY_FOR_nakamura_akira) | defined(DEBUG_ONLY_FOR_suginaka_katsunori)
  // レールエディタ起動
  if( (req->key_cont & railCont) == railCont )
  {
    return DEBUG_EVENT_RailEditor( gsys, fieldWork );
  }
#endif


#if defined(DEBUG_ONLY_FOR_ohkubo_tomohiko) | defined(DEBUG_ONLY_FOR_tomoya_takahashi)
  if( (req->key_cont & PAD_BUTTON_L) == PAD_BUTTON_L )
  {
    if( ZONEDATA_IsPalace( req->map_id ) ){
      return DEBUG_EVENT_PalaceBarrierMove( gsys, fieldWork, req->field_player, FIELD_PLAYER_GetDir( req->field_player ) );
    }
  }
#endif
  
  return NULL;
}
#endif

//======================================================================
//
//
//    イベント起動チェック：マップ遷移関連
//
//
//======================================================================
//--------------------------------------------------------------
/**
 * イベント マップ接続チェック
 *
 * @param req   イベントチェック用ワーク
 * @param fieldWork FIELDMAP_WORK
 * @param now_pos チェックする座標
 * @return GMEVENT  発生したイベントデータへのポインタ。 NULLの場合、接続なし
 */
//--------------------------------------------------------------
static GMEVENT * checkExit(EV_REQUEST * req,
    FIELDMAP_WORK *fieldWork, const VecFx32 *now_pos )
{
  int idx = getConnectID(req, now_pos);
  if (idx == EXIT_ID_NONE) {
    return NULL;
  }
  {
    const CONNECT_DATA * cnct;
    cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
    if ( CONNECTDATA_IsClosedExit(cnct) == TRUE || checkConnectIsFreeExit( cnct ) == FALSE )
    {
      return NULL;
    }
  }

  //マップ遷移発生の場合、出入口を記憶しておく
  rememberExitInfo(req, idx, now_pos);
  return getChangeMapEvent(req, fieldWork, idx, now_pos);
}

//--------------------------------------------------------------
/**
 * イベント アトリビュートチェック
 *
 * @param req   イベントチェック用ワーク
 * @param fieldWork FIELDMAP_WORK
 * @return GMEVENT  発生したイベントデータへのポインタ。 NULLの場合、イベントなし
 */
//--------------------------------------------------------------
static GMEVENT * checkAttribute(EV_REQUEST * req, FIELDMAP_WORK *fieldWork )
{
  VecFx32 pos;
  MAPATTR attr;
  MAPATTR_VALUE attrval;
  FLDMAPPER* mapper;

  // 足元のアトリビュートを取得
  FIELD_PLAYER_GetPos( req->field_player, &pos );
  mapper  = FIELDMAP_GetFieldG3Dmapper( fieldWork );
  attr    = MAPATTR_GetAttribute( mapper, &pos );
  attrval = MAPATTR_GetAttrValue( attr );

  if( CheckAttributeIsAutoWay(attrval) )
  { // 移動床
    return EVENT_PlayerMoveOnAutoWay( NULL, req->gsys, fieldWork );
  }
  else if( CheckAttributeIsCurrent(attrval) )
  { // 水流
    return EVENT_PlayerMoveOnCurrent( NULL, req->gsys, fieldWork );
  }
  return NULL;
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static GMEVENT * getChangeMapEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx,
    const VecFx32 * pos)
{
  LOC_EXIT_OFS exit_ofs;
  const CONNECT_DATA * cnct;
  cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
  if (pos != NULL) {
    exit_ofs = CONNECTDATA_GetExitOfs(cnct, pos);
  } else {
    exit_ofs = LOCATION_DEFAULT_EXIT_OFS;
  }

  if(CONNECTDATA_GetExitType( cnct ) == EXIT_TYPE_INTRUDE){
    return EVENT_ChangeMapByConnectIntrude(req->gsys, fieldWork, cnct, exit_ofs);
  }
  return EVENT_ChangeMapByConnect(req->gsys, fieldWork, cnct, exit_ofs);
}

//--------------------------------------------------------------
//マップ遷移発生の場合、出入口を記憶しておく
//--------------------------------------------------------------
static void rememberExitInfo(EV_REQUEST * req, int idx, const VecFx32 * pos)
{
  FIELDMAP_WORK * fieldWork = req->fieldWork;
  LOCATION ent_loc;
  LOCATION_Set( &ent_loc, FIELDMAP_GetZoneID(fieldWork), idx, 0, LOCATION_DEFAULT_EXIT_OFS,
      pos->x, pos->y, pos->z);
  GAMEDATA_SetEntranceLocation(req->gamedata, &ent_loc);
}

//--------------------------------------------------------------
//--------------------------------------------------------------
static int getConnectID(const EV_REQUEST * req, const VecFx32 * now_pos)
{
  const CONNECT_DATA * cnct;
  int idx = EVENTDATA_SearchConnectIDByPos(req->evdata, now_pos);
  
  if( idx == EXIT_ID_NONE ){
    return EXIT_ID_NONE;
  }
  cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
  if ( checkConnectIsFreeExit( cnct ) == FALSE )
  {
    if( checkConnectExitDir( cnct, req->player_dir ) == FALSE )
    {
      return EXIT_ID_NONE;
    }
  }
  return idx;
}
//--------------------------------------------------------------
//--------------------------------------------------------------
static void setFrontPos(const EV_REQUEST * req, VecFx32 * pos)
{
  enum { GRIDSIZE = 16 * FX32_ONE };
  *pos = *req->now_pos;
  
  switch( req->player_dir) {
  case DIR_UP:    pos->z -= GRIDSIZE; break;
  case DIR_DOWN:  pos->z += GRIDSIZE; break;
  case DIR_LEFT:  pos->x -= GRIDSIZE; break;
  case DIR_RIGHT: pos->x += GRIDSIZE; break;
  default:
                  GF_ASSERT(0);
  }
}

//--------------------------------------------------------------
/**
 * @brief 踏むだけ出入口かどうかのチェック
 * @param cnct
 * @retval  TRUE  踏むだけ
 * @retval  FALSE それ以外（方向や当たり判定を見る）
 */
//--------------------------------------------------------------
static BOOL checkConnectIsFreeExit( const CONNECT_DATA * cnct )
{
  EXIT_TYPE exit_type = CONNECTDATA_GetExitType( cnct );
  return ( exit_type == EXIT_TYPE_NONE
      || exit_type == EXIT_TYPE_WARP
      || exit_type == EXIT_TYPE_INTRUDE );
}

//--------------------------------------------------------------
/**
 * @brief 出入口の持つ方向指定チェック
 * @param cnct
 * @param player_dir
 * @retval  TRUE  適合している
 * @retval  FALSE 適合していない
 */
//--------------------------------------------------------------
static BOOL checkConnectExitDir(const CONNECT_DATA* cnct, u16 player_dir)
{
  return CONNECTDATA_IsEnableDir( cnct, player_dir );
}

//--------------------------------------------------------------
/**
 * イベント キー入力マップ遷移起動チェック
 *
 * @param req   イベントチェック用ワーク
 * @param gsys GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL イベント無し
 */
//--------------------------------------------------------------
static GMEVENT * checkPushExit(EV_REQUEST * req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  VecFx32 front_pos;
  int idx;
  
  //目の前が通行不可でない場合、チェックしない
  setFrontPos(req, &front_pos);
  {
    FLDMAPPER *g3Dmapper = FIELDMAP_GetFieldG3Dmapper(fieldWork);
    MAPATTR_FLAG flag = MAPATTR_GetAttrFlag( MAPATTR_GetAttribute(g3Dmapper, &front_pos) );
    if (!(flag & MAPATTR_FLAGBIT_HITCH))
    {
      return NULL;
    }
  }


  //足元チェック（マット）
  idx = getConnectID(req, req->now_pos);
  if (idx != EXIT_ID_NONE)
  {
    const CONNECT_DATA * cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
    if ( CONNECTDATA_IsClosedExit(cnct) == FALSE && CONNECTDATA_GetExitType(cnct) == EXIT_TYPE_MAT )
    {
      rememberExitInfo(req, idx, req->now_pos);
      return getChangeMapEvent(req, fieldWork, idx, req->now_pos);
    }
  }
  
  //目の前チェック（階段、壁、ドア）
  idx = getConnectID(req, &front_pos);
  if (idx != EXIT_ID_NONE) 
  {
    const CONNECT_DATA * cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
    if ( CONNECTDATA_IsClosedExit(cnct) == FALSE )
    {
      //マップ遷移発生の場合、出入口を記憶しておく
      //rememberExitInfo(req, idx, &front_pos);
      rememberExitInfo(req, idx, req->now_pos);
      return getChangeMapEvent(req, fieldWork, idx, &front_pos);
    }
  }


  // 無効イベントだった
  return NULL;
}

//--------------------------------------------------------------
/**
 * イベント キー入力ギミック起動チェック
 *
 * @param req   イベントチェック用ワーク
 * @param gsys GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL イベント無し
 */
//--------------------------------------------------------------
static GMEVENT * checkPushGimmick(const EV_REQUEST * req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  int idx;
  VecFx32 front_pos;
  
  setFrontPos(req, &front_pos);
  
  //ギミックが割り当てられているかを調べて、
  //ジムソースのオーバレイがなされていることを確認する
  if ( FLDGMK_GimmickCodeCheck(fieldWork, FLD_GIMMICK_GYM_INSECT) )
  {
    //虫ジム
    int x,z;
    x =front_pos.x / FIELD_CONST_GRID_FX32_SIZE;
    z =front_pos.z / FIELD_CONST_GRID_FX32_SIZE;
    return GYM_INSECT_CreatePushWallEvt(gsys, x, z);
  }
  
  //D06電気洞窟
  if( FLDGMK_GimmickCodeCheck(fieldWork, FLD_GIMMICK_D06R0101) ||
      FLDGMK_GimmickCodeCheck(fieldWork, FLD_GIMMICK_D06R0201) ||
      FLDGMK_GimmickCodeCheck(fieldWork, FLD_GIMMICK_D06R0301) )
  {
    return( D06_GIMMICK_CheckPushEvent(fieldWork,req->player_dir) );
  }
  
  { //進入禁止アトリビュートがあるか
    FLDMAPPER *g3Dmapper = FIELDMAP_GetFieldG3Dmapper(fieldWork);
    MAPATTR attr = MAPATTR_GetAttribute( g3Dmapper, &front_pos );
    MAPATTR_FLAG attr_flag =  MAPATTR_GetAttrFlag( attr );
    
    if( !(attr_flag & MAPATTR_FLAGBIT_HITCH) )
    {
      return NULL; //前方進入禁止アトリビュート無し
    }
    
    { //波乗り状態での滝下りチェック
      PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( req->field_player );
      
      if( form == PLAYER_MOVE_FORM_SWIM )
      {
        if( checkPokeWazaGroup(req->gamedata,WAZANO_TAKINOBORI) != 6 )
        {
          MAPATTR_VALUE attr_value = MAPATTR_GetAttrValue( attr );
        
          if( MAPATTR_VALUE_CheckWaterFall(attr_value) == TRUE )
          {
            fx32 t_height;
            VecFx32 t_pos = front_pos;
            MMDL *mmdl = FIELD_PLAYER_GetMMdl( req->field_player );
            MMDL_TOOL_AddDirVector( req->player_dir, &t_pos, GRID_FX32 );
          
            if( MMDL_GetMapPosHeight(mmdl,&t_pos,&t_height) == TRUE )
            {
              if( front_pos.y > t_height ) //下りなら滝下りイベント
              {
                return SCRIPT_SetEventScript(
                    req->gsys, SCRID_HIDEN_TAKIKUDARI, NULL, req->heapID );
              }
            }
          }
        }
      }
    }
    
    //壁の場合、電気ジムかを調べる
    //ギミックが割り当てられているかを調べて、
    //ジムソースのオーバレイがなされていることを確認する
    {
      idx = getConnectID(req, &front_pos);
      
      if( idx == EXIT_ID_NONE )
      {
        if( FLDGMK_GimmickCodeCheck(fieldWork,FLD_GIMMICK_GYM_ELEC) )
        { //電気ジム
          return GYM_ELEC_CreateMoveEvt(gsys);
        }
        else if( FLDGMK_GimmickCodeCheck(fieldWork,FLD_GIMMICK_GYM_FLY) )
        { //飛行ジム
          return GYM_FLY_CreateShotEvt(gsys);
        }
      }
    }
  }
  
  return NULL;
}

//--------------------------------------------------------------
/**
 * @brief ノーグリッドマップ、レールシステム上での出入口判定
 */
//--------------------------------------------------------------
static GMEVENT * checkRailExit(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork)
{
  int idx;
  const FIELD_PLAYER* cp_player = FIELDMAP_GetFieldPlayer( fieldWork );
  const MMDL* cp_mmdl = FIELD_PLAYER_GetMMdl( cp_player );
  RAIL_LOCATION pos;
  const CONNECT_DATA* cnct;

  // レールロケーション
  MMDL_GetRailLocation( cp_mmdl, &pos );

  // その場チェック　＝　マット
  idx = EVENTDATA_SearchConnectIDByRailLocation(req->evdata, &pos);

  if (idx == EXIT_ID_NONE){
    return NULL;
  }

  // 乗っただけで起動するのかチェック
  cnct = EVENTDATA_GetConnectByID( req->evdata, idx );
  if( CONNECTDATA_IsClosedExit(cnct) == TRUE || checkConnectIsFreeExit( cnct ) == FALSE  )
  {
    return NULL;
  }
  
  rememberExitRailInfo( req, idx, &pos );
  return getRailChangeMapEvent(req, fieldWork, idx, &pos);
}

//--------------------------------------------------------------
/**
 *  @brief  ノーグリッドマップ　レールシステム上での押し込み出入り口判定
 */
//--------------------------------------------------------------
static GMEVENT * checkRailPushExit(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork, const RAIL_LOCATION* front_location)
{
  int idx;
  RAIL_LOCATION pos;
  BOOL result;
  const FIELD_PLAYER* cp_player = FIELDMAP_GetFieldPlayer( fieldWork );
  const MMDL* cp_mmdl = FIELD_PLAYER_GetMMdl( cp_player );
  const CONNECT_DATA* cnct;


  MMDL_GetRailLocation( cp_mmdl, &pos );
   
  // その場チェック　＝　マット
  idx = EVENTDATA_SearchConnectIDByRailLocation(req->evdata, &pos);

  if (idx != EXIT_ID_NONE){
    cnct = EVENTDATA_GetConnectByID( req->evdata, idx );

    if( CONNECTDATA_IsClosedExit(cnct) == FALSE && checkConnectExitDir( cnct, req->player_dir ) )
    {
      if( CONNECTDATA_GetExitType( cnct ) == EXIT_TYPE_MAT)
      {
        // GO
        rememberExitRailInfo( req, idx, &pos );
        return getRailChangeMapEvent(req, fieldWork, idx, &pos);
      }
    }
  }

  // 目の前チェック = 階段、壁、ドア
  idx = EVENTDATA_SearchConnectIDByRailLocation(req->evdata, front_location);
  if(idx == EXIT_ID_NONE)
  {
    return NULL;
  }

  
  cnct = EVENTDATA_GetConnectByID( req->evdata, idx );

  TOMOYA_Printf( "player dir %d\n", req->player_dir );
  if( CONNECTDATA_IsClosedExit(cnct) == FALSE && checkConnectExitDir( cnct, req->player_dir ) )
  {
    rememberExitRailInfo( req, idx, &pos );  // front_posは移動不可なので、posにする
    return getRailChangeMapEvent(req, fieldWork, idx, &pos);
  }

  return NULL;
}


//======================================================================
//======================================================================
//--------------------------------------------------------------
/**
 * フィールドノーマルエンカウントイベント起動チェック
 * @param gsys GAMESYS_WORK
 * @param fieldWork FIELDMAP_WORK
 * @retval GMEVENT NULL イベント無し
 */
//--------------------------------------------------------------
static GMEVENT * checkNormalEncountEvent( const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  FIELD_ENCOUNT * encount = FIELDMAP_GetEncount(fieldWork);
  ENCOUNT_TYPE enc_type;
  
#ifdef PM_DEBUG
  if( req->debugRequest ){
    return NULL;
  }
#endif
  if( !(req->stepRequest) ){
    return NULL;
  }
  
  // WFBC
  if( ZONEDATA_IsWfbc( req->map_id ) )
  {
    FLDMAPPER * p_mapper = FIELDMAP_GetFieldG3Dmapper( fieldWork );
    FIELD_WFBC * p_wfbc = FLDMAPPER_GetWfbcWork( p_mapper );
    return (GMEVENT*)FIELD_ENCOUNT_CheckWfbcEncount(encount, p_wfbc );
  }
  
  enc_type = ENC_TYPE_NORMAL; //(ZONEDATA_IsBingo( req->map_id ) == TRUE) ? ENC_TYPE_BINGO : ENC_TYPE_NORMAL;
  return (GMEVENT*)FIELD_ENCOUNT_CheckEncount(encount, enc_type);
}

//--------------------------------------------------------------
/**
 *  @brief  レールずり落ちチェック
 */
//--------------------------------------------------------------
static GMEVENT * checkRailSlipDown(const EV_REQUEST * req, GAMESYS_WORK *gsys, FIELDMAP_WORK * fieldWork)
{
  const FIELD_PLAYER* cp_player = FIELDMAP_GetFieldPlayer( fieldWork );
  const MMDL* cp_mmdl = FIELD_PLAYER_GetMMdl( cp_player );
  u16 dir = FIELD_PLAYER_GetKeyDir( cp_player, req->key_cont );

  if( dir == DIR_DOWN ){
    // ずり落ち処理
    if( RAIL_ATTR_VALUE_CheckSlipDown( MAPATTR_GetAttrValue(req->mapattr) ) )
    {
      // その方向にいどうできないようになっているか？
      if( (MMDL_HitCheckRailMoveDir( cp_mmdl, dir ) & (MMDL_MOVEHITBIT_ATTR|MMDL_MOVEHITBIT_LIM)) )
      {
        // ずり落ち開始
        return EVENT_RailSlipDown( gsys, fieldWork, dir );
      }
    }
  }

  return NULL;
}

//--------------------------------------------------------------
/**
 *  @brief  レールマップの出入り口情報を保存
 */
//--------------------------------------------------------------
static void rememberExitRailInfo(const EV_REQUEST * req, int idx, const RAIL_LOCATION* loc)
{
  //マップ遷移発生の場合、出入口を記憶しておく
  FIELDMAP_WORK * fieldWork = req->fieldWork;
  LOCATION ent_loc;
  LOCATION_SetRail( &ent_loc, FIELDMAP_GetZoneID(fieldWork), idx, 0, LOCATION_DEFAULT_EXIT_OFS,
      loc->rail_index, loc->line_grid, loc->width_grid);
  GAMEDATA_SetEntranceLocation(req->gamedata, &ent_loc);
}

//--------------------------------------------------------------
/**
 *  @brief  レールのマップ変更イベントを取得する
 */
//--------------------------------------------------------------
static GMEVENT * getRailChangeMapEvent(const EV_REQUEST * req, FIELDMAP_WORK * fieldWork, int idx,
    const RAIL_LOCATION* loc)
{
  LOC_EXIT_OFS exit_ofs;
  const CONNECT_DATA * cnct;
  cnct = EVENTDATA_GetConnectByID(req->evdata, idx);
  exit_ofs = CONNECTDATA_GetRailExitOfs( cnct, loc );
  return EVENT_ChangeMapByConnect(req->gsys, fieldWork, cnct, exit_ofs);
}

//--------------------------------------------------------------
/**
 *  @brief  目の前が交通不可能かチェック
 *
 *  @retval TRUE    移動可能
 *  @retval FALSE   移動不可能
 */
//--------------------------------------------------------------
static BOOL checkRailFrontMove( const FIELD_PLAYER* cp_player, const EV_REQUEST * req )
{
  const MMDL* cp_mmdl = FIELD_PLAYER_GetMMdl( cp_player );
  
  if( MMDL_HitCheckRailMoveDir( cp_mmdl, req->player_dir ) & (MMDL_MOVEHITBIT_ATTR|MMDL_MOVEHITBIT_LIM) )
  {
    return FALSE;
  }
  return TRUE;
}

//--------------------------------------------------------------
/**
 * イベント起動チェック　決定ボタン入力時に発生するイベントチェック
 * @param fieldWork FIELDMAP_WORK
 * @param event 発生イベントプロセス格納先
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @retval BOOL TRUE=イベント起動
 */
//--------------------------------------------------------------
static BOOL event_CheckEventDecideButton( FIELDMAP_WORK *fieldWork,
    GMEVENT **event, const int key_trg, const int key_cont )
{
  return FALSE;
}

//--------------------------------------------------------------
/**
 * イベント起動チェック　自機動作終了時に発生するイベントチェック。
 * 移動終了、振り向き、壁衝突動作などで発生。
 * @param fieldWork FIELDMAP_WORK
 * @param event 発生イベントプロセス格納先
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @retval BOOL TRUE=イベント起動
 */
//--------------------------------------------------------------
static BOOL event_CheckEventMoveEnd( FIELDMAP_WORK *fieldWork,
    GMEVENT **event, const int key_trg, const int key_cont )
{
  return FALSE;
}

//--------------------------------------------------------------
/**
 * イベント起動チェック　一歩移動終了時に発生するイベントチェック
 * @param fieldWork FIELDMAP_WORK
 * @param event 発生イベントプロセス格納先
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @retval BOOL TRUE=イベント起動
 */
//--------------------------------------------------------------
static BOOL event_CheckEventOneStepMoveEnd( FIELDMAP_WORK *fieldWork,
    GMEVENT **event, const int key_trg, const int key_cont )
{
  return FALSE;
}

//--------------------------------------------------------------
/**
 * イベント起動チェック　十字キー入力中に発生するイベントチェック
  * @param fieldWork FIELDMAP_WORK
 * @param event 発生イベントプロセス格納先
 * @param key_trg キートリガ
 * @param key_cont キーコンティニュー
 * @retval BOOL TRUE=イベント起動
 */
//--------------------------------------------------------------
static BOOL event_CheckEventPushKey( FIELDMAP_WORK *fieldWork,
    GMEVENT **event, const int key_trg, const int key_cont )
{
  return FALSE;
}

//======================================================================
//  便利ボタンイベント
//======================================================================

//======================================================================
//  波乗りイベント
//======================================================================
#include "fldeff_namipoke.h"

typedef struct
{
  int wait;
  u16 dir;
  u16 attr_kishi_flag;
  GAMESYS_WORK *gsys;
  FIELDMAP_WORK *fieldWork;
}EVWORK_NAMINORI;

static GMEVENT_RESULT event_NaminoriStart(
    GMEVENT *event, int *seq, void *wk )
{
  EVWORK_NAMINORI *work = wk;

  FIELD_PLAYER *fld_player =
    FIELDMAP_GetFieldPlayer( work->fieldWork );
  PLAYER_MOVE_FORM form =
    FIELD_PLAYER_GetMoveForm( fld_player );
  MMDL *mmdl =
    FIELD_PLAYER_GetMMdl( fld_player );
  
  FLDEFF_TASK *task;
  
  switch( *seq )
  {
  case 0: //波乗りポケモン出現
    {
      u16 dir;
      s16 gx,gz;
      fx32 height;
      VecFx32 pos;
      dir = MMDL_GetDirDisp( mmdl );
      gx = MMDL_GetGridPosX( mmdl );
      gz = MMDL_GetGridPosZ( mmdl );
      MMDL_GetVectorPos( mmdl, &pos );
      MMDL_TOOL_GetCenterGridPos( gx, gz, &pos );
      
      {
        fx32 range = GRID_FX32; //１マス先
        
        if( work->attr_kishi_flag == TRUE ){
          range <<= 1; //２マス先
        }
      
        MMDL_TOOL_AddDirVector( dir, &pos, range );
      }
      
      height = 0;
      MMDL_GetMapPosHeight( mmdl, &pos, &height );
      pos.y = height;
     
      {
        FLDEFF_CTRL *fectrl;
        fectrl = FIELDMAP_GetFldEffCtrl( work->fieldWork );
        
        task = FLDEFF_NAMIPOKE_SetMMdl(
            fectrl, dir, &pos, mmdl, NAMIPOKE_JOINT_OFF );
        
        FIELD_PLAYER_SetEffectTaskWork( fld_player, task );
      }
    }
    
    (*seq)++;
    break;
  case 1:
    work->wait++;
    if( work->wait > 15 ){
      (*seq)++;
    }
    break;
  case 2:
    {
      u16 dir = MMDL_GetDirDisp( mmdl );
      u16 ac = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_1G_8F );
      
      if( work->attr_kishi_flag == TRUE ){
        ac = MMDL_ChangeDirAcmdCode( dir, AC_JUMP_U_2G_16F );
      }
      
      MMDL_SetAcmd( mmdl, ac );
    }
    (*seq)++;
    break;
  case 3:
    if( MMDL_CheckEndAcmd(mmdl) == TRUE ){
      MMDL_EndAcmd( mmdl );
      task = FIELD_PLAYER_GetEffectTaskWork( fld_player );
      FLDEFF_NAMIPOKE_SetJointFlag( task, NAMIPOKE_JOINT_ON );
      FIELD_PLAYER_SetNaminori( fld_player );
      (*seq)++;
    }
    break;
  case 4:
    return GMEVENT_RES_FINISH;
  }
  
  return GMEVENT_RES_CONTINUE;
}

static GMEVENT * eventSet_NaminoriStart( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  GMEVENT *event;
  EVWORK_NAMINORI *work;
  
  event = GMEVENT_Create(
      gsys, NULL, event_NaminoriStart, sizeof(EVWORK_NAMINORI) );
  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  work->fieldWork = fieldWork;
  return( event );
}

void FIELD_EVENT_ChangeNaminoriStart( GMEVENT *event,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  EVWORK_NAMINORI *work;
  GMEVENT_Change( event, event_NaminoriStart, sizeof(EVWORK_NAMINORI) );
  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  work->fieldWork = fieldWork;
}

//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT * checkEvent_PlayerNaminoriStart( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  { //イベントチェック
    FIELD_PLAYER *fld_player;
    PLAYER_MOVE_FORM form;
    fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
    form = FIELD_PLAYER_GetMoveForm( fld_player );
    if( form == PLAYER_MOVE_FORM_SWIM ){
      return NULL;
    }
  }
  
  { //イベント発行
    GMEVENT *event;
    event = eventSet_NaminoriStart( req, gsys, fieldWork );
    return( event );
  }
}

//======================================================================
//  波乗り上陸イベント
//======================================================================
static GMEVENT_RESULT event_NaminoriEnd(
    GMEVENT *event, int *seq, void *wk )
{
  EVWORK_NAMINORI *work = wk;

  FIELD_PLAYER *fld_player =
    FIELDMAP_GetFieldPlayer( work->fieldWork );
  PLAYER_MOVE_FORM form =
    FIELD_PLAYER_GetMoveForm( fld_player );
  MMDL *mmdl =
    FIELD_PLAYER_GetMMdl( fld_player );
  
  FLDEFF_TASK *task;
  
  switch( *seq )
  {
  case 0: //動作モデルポーズ＆入力された方向に向く
    MMDLSYS_PauseMoveProc( MMDL_GetMMdlSys(mmdl) );
    MMDL_SetDirDisp( mmdl, work->dir );
    (*seq)++;
    break;
  case 1: //波乗りポケモン切り離し
    task = FIELD_PLAYER_GetEffectTaskWork( fld_player );
    FLDEFF_NAMIPOKE_SetJointFlag( task, NAMIPOKE_JOINT_OFF );
    FIELD_PLAYER_SetRequest( fld_player, FIELD_PLAYER_REQBIT_NORMAL );
    FIELD_PLAYER_ForceUpdateRequest( fld_player );
    {
      u16 ac = MMDL_ChangeDirAcmdCode( work->dir, AC_JUMP_U_1G_8F );
      if( work->attr_kishi_flag == TRUE ){
        ac = MMDL_ChangeDirAcmdCode( work->dir, AC_JUMP_U_2G_16F );
      }
      MMDL_SetAcmd( mmdl, ac );
    }
    (*seq)++;
    break;
  case 2:
    if( MMDL_CheckEndAcmd(mmdl) == TRUE ){
      FIELD_PLAYER_SetNaminoriEnd( fld_player );
      (*seq)++;
    }
    break;
  case 3:
    MMDLSYS_ClearPauseMoveProc( MMDL_GetMMdlSys(mmdl) );
#ifdef BUGFIX_GFBTS1967_100713
    FIELD_PLAYER_SetNaminoriEventEnd( fld_player, TRUE );
#endif
    return GMEVENT_RES_FINISH;
  }
  
  return GMEVENT_RES_CONTINUE;
}

static GMEVENT * eventSet_NaminoriEnd( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork,
    u16 dir, BOOL attr_kishi_flag )
{
  GMEVENT *event;
  EVWORK_NAMINORI *work;
  
  event = GMEVENT_Create(
      gsys, NULL, event_NaminoriEnd, sizeof(EVWORK_NAMINORI) );
  work = GMEVENT_GetEventWork( event );
  work->gsys = gsys;
  work->fieldWork = fieldWork;
  work->dir = dir;
  work->attr_kishi_flag = attr_kishi_flag;
  return( event );
}

//--------------------------------------------------------------
/**
 * @param
 * @retval
 */
//--------------------------------------------------------------
static GMEVENT * checkEvent_PlayerNaminoriEnd( const EV_REQUEST *req,
    GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork )
{
  FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
  PLAYER_MOVE_FORM form = FIELD_PLAYER_GetMoveForm( fld_player );
    
  if( form == PLAYER_MOVE_FORM_SWIM ) //波乗り
  {
    BOOL kishi;
    VecFx32 pos;
    MAPATTR attr;
    MAPATTR_FLAG attr_flag;
    MAPATTR_VALUE attr_value;
    FLDMAPPER *mapper = FIELDMAP_GetFieldG3Dmapper( fieldWork );
    u16 dir = FIELD_PLAYER_GetKeyDir( req->field_player, req->key_cont );
    
    if( dir != DIR_NOT )
    {
      FIELD_PLAYER_GetDirPos( fld_player, dir, &pos );
      attr = MAPATTR_GetAttribute( mapper, &pos );
      attr_flag = MAPATTR_GetAttrFlag( attr );
      attr_value = MAPATTR_GetAttrValue( attr );
      
      kishi = FALSE;
      
      if( MAPATTR_VALUE_CheckShore(attr_value) == TRUE ){ //岸
        MMDL_TOOL_AddDirVector( dir, &pos, GRID_FX32 ); //もう一歩先
        attr = MAPATTR_GetAttribute( mapper, &pos );
        attr_flag = MAPATTR_GetAttrFlag( attr );
        kishi = TRUE;
      }

      if( NaminoriEnd_CheckMMdl(req, fieldWork) == TRUE &&   //前方に誰もいなくて
          (attr_flag&MAPATTR_FLAGBIT_HITCH) == 0 && //進入可能で
          (attr_flag&MAPATTR_FLAGBIT_WATER) == 0 ){ //水以外
        GMEVENT *event;
        event = eventSet_NaminoriEnd( req, gsys, fieldWork, dir, kishi );
        return( event );
      }
    }
  }
  
  return NULL;
}

//======================================================================
//
//======================================================================
//--------------------------------------------------------------
/**
 * 自機前方の会話OBJ取得
 * @param
 * @retval
 */
//--------------------------------------------------------------
static MMDL * getFrontTalkOBJ( EV_REQUEST *req, FIELDMAP_WORK *fieldMap )
{
  MMDL *mmdl,*jiki;
  VecFx32 pos;
  s16 gx,gy,gz;
  FLDMAPPER *mapper;
  MAPATTR attr;
  MAPATTR_VALUE attr_val;

  jiki = FIELD_PLAYER_GetMMdl( req->field_player );
  
  FIELD_PLAYER_GetFrontGridPos( req->field_player, &gx, &gy, &gz );
  MMDL_TOOL_GridPosToVectorPos( gx, gy, gz, &pos );
  
  mapper = FIELDMAP_GetFieldG3Dmapper( fieldMap );
  attr = MAPATTR_GetAttribute( mapper, &pos );
  attr_val = MAPATTR_GetAttrValue( attr );
  
  if( MAPATTR_VALUE_CheckCounter(attr_val) == TRUE ){
    u16 dir = MMDL_GetDirDisp( jiki );
    MMDL_TOOL_AddDirGrid( dir, &gx, &gz, 1 );
  }
  
  mmdl = MMDLSYS_SearchGridPosEx(
      FIELDMAP_GetMMdlSys(fieldMap), gx, gz, MMDL_GetVectorPosY(jiki), FRONT_TALKOBJ_HEIGHT_DIFF, FALSE );
  
  return( mmdl );
}


//--------------------------------------------------------------
/**
 *  @brief  レールマップでの、目の前のモデルを取得
 *
 *  @param  req
 *  @param  *fieldMap 
 *
 *  @return
 */
//--------------------------------------------------------------
static MMDL * getRailFrontTalkOBJ( EV_REQUEST *req, FIELDMAP_WORK *fieldMap )
{
  MMDL *mmdl;
  MMDL* player_mmdl;
  RAIL_LOCATION location;


  
  player_mmdl = FIELD_PLAYER_GetMMdl( req->field_player );
  if( MMDL_GetRailFrontLocation( player_mmdl, &location ) )
  {
    mmdl = MMDLSYS_SearchRailLocation(
        FIELDMAP_GetMMdlSys(fieldMap), &location, FALSE );
  }
  else
  {
    mmdl = NULL;
  }

  
  return( mmdl );
}

//--------------------------------------------------------------
//--------------------------------------------------------------
typedef struct {
  BOOL (*attr_check_func)(const MAPATTR_VALUE );
  u16 dir_code;
  u16 script_id;
}MAPATTR_EVENTDATA;

//--------------------------------------------------------------
/**
 * @brief BGアトリビュート話しかけチェック
 */
//--------------------------------------------------------------
static u16 checkTalkAttrEvent( EV_REQUEST *req, FIELDMAP_WORK *fieldMap)
{
  static const MAPATTR_EVENTDATA check_attr_data[] = {
    { MAPATTR_VALUE_CheckPC,            DIR_UP,  SCRID_PC },
    { MAPATTR_VALUE_CheckMap,           DIR_NOT, EVENTDATA_ID_NONE },
    { MAPATTR_VALUE_CheckTV,            DIR_UP,  SCRID_TV },
    { MAPATTR_VALUE_CheckBookShelf1,    DIR_NOT, SCRID_BG_MSG_BOOK1_01 },
    { MAPATTR_VALUE_CheckBookShelf2,    DIR_NOT, SCRID_BG_MSG_BOOK2_01 },
    { MAPATTR_VALUE_CheckBookShelf3,    DIR_NOT, SCRID_BG_MSG_BOOKRACK1_01 },
    { MAPATTR_VALUE_CheckBookShelf4,    DIR_NOT, SCRID_BG_MSG_BOOKRACK2_01 },
    { MAPATTR_VALUE_CheckVase,          DIR_NOT, SCRID_BG_MSG_SCRAP_01 },
    { MAPATTR_VALUE_CheckDustBox,       DIR_NOT, SCRID_BG_MSG_SCRAP_01 },
    { MAPATTR_VALUE_CheckShopShelf1,    DIR_NOT, SCRID_BG_MSG_SHOPRACK1_01 },
    { MAPATTR_VALUE_CheckShopShelf2,    DIR_NOT, SCRID_BG_MSG_SHOPRACK2_01 },
    { MAPATTR_VALUE_CheckShopShelf3,    DIR_NOT, SCRID_BG_MSG_SHOPRACK3_01 },
    { MAPATTR_VALUE_CheckVendorMachine, DIR_UP,  SCRID_VENDING_MACHINE01 },
  };
  int i;

  MAPATTR nattr,fattr;
  MAPATTR_VALUE fattr_val;

  if ( GAMEDATA_GetIntrudeReverseArea( req->gamedata ) == TRUE )
  { //パレスでは一切のアトリビュート話しかけを行わない
    return EVENTDATA_ID_NONE;
  }

  nattr = FIELD_PLAYER_GetMapAttr( req->field_player );
  fattr = FIELD_PLAYER_GetDirMapAttr( req->field_player, req->player_dir );
  fattr_val = MAPATTR_GetAttrValue( fattr );

  for (i = 0; i < NELEMS(check_attr_data); i++)
  {
    if ( check_attr_data[i].dir_code == req->player_dir || check_attr_data[i].dir_code == DIR_NOT )
    {
      if ( check_attr_data[i].attr_check_func(fattr_val) )
      {
        return check_attr_data[i].script_id;
      }
    }
  }

  ////////////////////////////////////////////////
  //ここから下フィールド技チェック

  //ユニオン/コロシアム/パレスでは起動しない
  if( ZONEDATA_CheckFieldSkillUse( req->map_id ) == FALSE ||
      GAMEDATA_GetIntrudeReverseArea( req->gamedata ) == TRUE ){
    return EVENTDATA_ID_NONE;
  }

  //波乗りアトリビュート話し掛けチェック
  if( FIELD_PLAYER_CheckNaminoriUse( req->field_player, nattr, fattr )){
    if( checkPokeWazaGroup(req->gamedata,WAZANO_NAMINORI) != 6 ){
      return SCRID_HIDEN_NAMINORI;
    }
  }
  //滝登りアトリビュート話しかけチェック
  if( FIELD_PLAYER_CheckTakinoboriUse( req->field_player, nattr, fattr ) )
  {
    //滝は話しかけ時のメッセージがあるので、技の所持チェックはスクリプト側に任せる
    return SCRID_HIDEN_TAKINOBORI;
  }
 
  return EVENTDATA_ID_NONE;
}

//--------------------------------------------------------------
/**
 * 手持ちポケモン技チェック
 * @param gdata GAMEDATA
 * @param waza_no チェックする技ナンバー
 * @retval int 6=技無し
 */
//--------------------------------------------------------------
static int checkPokeWazaGroup( GAMEDATA *gdata, u32 waza_no )
{
  int i,max;
  POKEMON_PARAM *pp;
  POKEPARTY *party = GAMEDATA_GetMyPokemon( gdata );
  
  max = PokeParty_GetPokeCount( party );
  
  for( i = 0; i < max; i++ ){
    pp = PokeParty_GetMemberPointer( party, i );
    
    //たまごチェック
    if( PP_Get(pp,ID_PARA_tamago_flag,NULL) != 0 ){
      continue;
    }
    
    //技リストからチェック
    if( PP_Get(pp,ID_PARA_waza1,NULL) == waza_no ||
        PP_Get(pp,ID_PARA_waza2,NULL) == waza_no ||
        PP_Get(pp,ID_PARA_waza3,NULL) == waza_no ||
        PP_Get(pp,ID_PARA_waza4,NULL) == waza_no ){
      return i;
    }
  }

#ifdef DEBUG_ONLY_FOR_kagaya  //test
  return 0;
#endif
   
  return 6;
}


//--------------------------------------------------------------
/**
 * 2vs2対戦が可能かどうかチェック
 * @param
 * @retval
 */
//--------------------------------------------------------------
u32 FIELD_EVENT_CountBattleMember( GAMESYS_WORK *gsys )
{
  int i,max,count;
  POKEMON_PARAM *pp;
  GAMEDATA *gdata = GAMESYSTEM_GetGameData( gsys );
  POKEPARTY *party = GAMEDATA_GetMyPokemon( gdata );
  
  max = PokeParty_GetPokeCount( party );
  
  for( i = 0, count = 0; i < max; i++ ){
    pp = PokeParty_GetMemberPointer( party, i );
    
    //たまごチェック
    if( PP_Get(pp,ID_PARA_tamago_flag,NULL) != 0 ){
      continue;
    }
    
    //HPチェック
    if( PP_Get(pp,ID_PARA_hp,NULL) == 0 ){
      continue;
    }

    count++;
  }
  
  return count;
}

//--------------------------------------------------------------
/**
 * @brief 波乗り終了時のNPC判定
 *
 * @param req
 * @param fieldWork
 *
 * @return 自機が降り立つ場所にNPCがいなければ TRUE
 *         そうでなければ FALSE
 */
//--------------------------------------------------------------
static BOOL NaminoriEnd_CheckMMdl( const EV_REQUEST* req, FIELDMAP_WORK* fieldWork )
{
  int i;
  MMDL *mmdl, *jiki;
  u16 dir;
  s16 gx, gy, gz;

  // 自機の位置を取得
  jiki = FIELD_PLAYER_GetMMdl( req->field_player );
  dir = req->key_direction; //MMDL_GetDirMove( jiki );
  gx = MMDL_GetGridPosX( jiki );
  gz = MMDL_GetGridPosZ( jiki );

  // 2歩先までチェックする
  for( i=0; i<2; i++ )
  {
    gx += MMDL_TOOL_GetDirAddValueGridX( dir );
    gz += MMDL_TOOL_GetDirAddValueGridZ( dir );

    // 動作モデルを検索
    mmdl = MMDLSYS_SearchGridPos(
        FIELDMAP_GetMMdlSys(fieldWork), gx, gz, TRUE );

    // 動作モデルを発見
    if( mmdl ) { return FALSE; }
  }

  return TRUE; 
}

