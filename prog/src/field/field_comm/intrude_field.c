//==============================================================================
/**
 * @file    intrude_field.c
 * @brief   侵入でフィールド時に行うメイン：フィールドオーバーレイに配置
 * @author  matsuda
 * @date    2009.09.03(木)
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "infowin/infowin.h"
#include "gamesystem/gamesystem.h"
#include "field/field_player.h"
#include "test/performance.h"
#include "system/main.h"
#include "field/field_comm_actor.h"
#include "gamesystem/game_comm.h"
#include "fieldmap/zone_id.h"
#include "field/event_mapchange.h"
#include "intrude_types.h"
#include "intrude_work.h"
#include "intrude_field.h"
#include "field/intrude_comm.h"
#include "intrude_comm_command.h"
#include "system/net_err.h"
#include "field/zonedata.h"
#include "field/field_g3d_mapper.h"
#include "intrude_main.h"
#include "intrude_mission.h"
#include "field/intrude_secret_item.h"
#include "field/event_mission_secretitem_ex.h"

#include "message.naix"
#include "msg/msg_d_field.h"
#include "msg/msg_invasion.h"
#include "fieldmap/map_matrix.naix"  //MATRIX_ID

#include "field/event_intrude.h"
#include "savedata/symbol_save.h"
#include "field/event_symbol.h"

#include "field/fldmmdl.h"
#include "../../../resource/fldmapdata/script/palace01_def.h"  //SCRID_〜
#include "field/script_def.h"



//==============================================================================
//  構造体定義
//==============================================================================
///デバッグ用
typedef struct{
  VecFx32 pos;
  INTRUDE_COMM_SYS_PTR intcomm;
  FIELDMAP_WORK *fieldmap;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  ZONEID zoneID;
  u16 wait;
  u8 padding[2];
}DEBUG_SIOEND_WARP;  //DEBUG_SIOEND_MAPCHANGE;

///デバッグ用
typedef struct{
  INTRUDE_COMM_SYS_PTR intcomm;
  FIELDMAP_WORK *fieldmap;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  u16 wait;
  u8 padding[2];
}DEBUG_SIOEND_CHILD;

///NGメッセージイベント用ワーク
typedef struct{
  FIELDMAP_WORK *fieldWork;
  MMDL *player_mmdl;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  u16 dir;
}EVENT_PALACE_NGWIN;

///バリアーイベント用ワーク
typedef struct{
  FIELDMAP_WORK *fieldWork;
  MMDL *player_mmdl;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  WORDSET *wordset;
  u16 dir;
  u8 no_change;
  u8 padding;
  INTRUDE_EVENT_DISGUISE_WORK ev_diswork;
}EVENT_PALACE_BARRIER;


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static GMEVENT * EVENT_ChangeMapPosCommEnd(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		u16 zone_id, const VecFx32 * pos, INTRUDE_COMM_SYS_PTR intcomm );
static GMEVENT_RESULT DebugEVENT_MapChangeCommEnd(GMEVENT * event, int *seq, void*work);
static GMEVENT * DEBUG_EVENT_ChildCommEnd(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, INTRUDE_COMM_SYS_PTR intcomm);
static GMEVENT_RESULT DebugEVENT_ChildCommEnd(GMEVENT * event, int *seq, void*work);
static void _PalaceMapCommBootCheck(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, GAME_COMM_SYS_PTR game_comm, FIELD_PLAYER *pcActor);
static void _PalaceFieldPlayerWarp(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor, INTRUDE_COMM_SYS_PTR intcomm);
static GMEVENT * EVENT_PalaceNGWin( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *fld_player, u16 dir );
static GMEVENT_RESULT _EventPalaceNGWinEvent( GMEVENT *event, int *seq, void *wk );
static GMEVENT * EVENT_PalaceBarrierMove( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *fld_player, u16 dir );
static GMEVENT_RESULT _EventPalaceBarrierMove( GMEVENT *event, int *seq, void *wk );


//==============================================================================
//  データ
//==============================================================================
///パレスへワープしてきたときの出現座標
static const VecFx32 PalaceWarpPos = {
  PALACE_MAP_LEN/2 - 1*FX32_ONE,
  32*FX32_ONE,
  488 * FX32_ONE,
};

//--------------------------------------------------------------
//  外部データ
//--------------------------------------------------------------
#include "palace_zone_setting.cdat"


//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
// フィールド通信システム更新
//  @param  FIELDMAP_WORK フィールドワーク
//  @param  GAMESYS_WORK  ゲームシステムワーク(PLAYER_WORK取得用
//  @param  PC_ACTCONT    プレイヤーアクター(プレイヤー数値取得用
//  自分のキャラの数値を取得して通信用に保存
//  他キャラの情報を取得し、通信から設定
//--------------------------------------------------------------
void IntrudeField_UpdateCommSystem( FIELDMAP_WORK *fieldWork ,
        GAMESYS_WORK *gameSys , FIELD_PLAYER *pcActor )
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSys);
  INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(game_comm);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gameSys);
  int i, my_net_id;
  BOOL update_ret;

#if 0
  //※check　デバッグ
  if(ZONEDATA_IsBingo(FIELDMAP_GetZoneID(fieldWork)) && (GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT)){
    DEBUG_INTRUDE_Pokemon_Add(fieldWork);
  }
#endif

  //パレス通信自動起動ON/OFF処理
  _PalaceMapCommBootCheck(fieldWork, gameSys, game_comm, pcActor);

  if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_INVASION || intcomm == NULL){
    return;
  }

  if(intcomm->palace_in == FALSE){
    PLAYER_WORK *my_player = GAMEDATA_GetMyPlayerWork(gamedata);
    if(ZONEDATA_IsPalace( PLAYERWORK_getZoneID( my_player ) ) == TRUE){
      intcomm->palace_in = TRUE;
    }
  }
  IntrudeField_ConnectMap(fieldWork, gameSys, intcomm);

  if(intcomm->comm_status != INTRUDE_COMM_STATUS_UPDATE){
    return;
  }

  my_net_id = GAMEDATA_GetIntrudeMyID(gamedata);

  if(Intrude_SetSendStatus(intcomm) == TRUE){
    intcomm->send_status = TRUE;
  }

  CommPlayer_Update(intcomm->cps);
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(i == my_net_id){
      continue;
    }
    if(intcomm->recv_profile & (1 << i)){
      if(CommPlayer_CheckOcc(intcomm->cps, i) == FALSE){
        MYSTATUS *myst = GAMEDATA_GetMyStatusPlayer(gamedata, i);
        u16 obj_code = Intrude_GetObjCode(&intcomm->intrude_status[i], 
          GAMEDATA_GetMyStatusPlayer(gamedata, i));
        CommPlayer_Add(intcomm->cps, i, obj_code, &intcomm->intrude_status[i].player_pack);
      }
      else{
        CommPlayer_SetParam(intcomm->cps, i, &intcomm->intrude_status[i].player_pack);
        //バトル or ビンゴバトル中なら「！」を表示
        if(intcomm->intrude_status[i].action_status == INTRUDE_ACTION_BATTLE
            || intcomm->intrude_status[i].action_status == INTRUDE_ACTION_BINGO_BATTLE){
          CommPlayer_SetGyoeTask(intcomm->cps, i);
        }
      }
    }
  }
  
  _PalaceFieldPlayerWarp(fieldWork, gameSys, pcActor, intcomm);
}

//==================================================================
/**
 * 自分の正面座標に対しての通信プレイヤー話しかけチェック
 *
 * @param   intcomm		
 * @param   fld_player		
 * @param   hit_netid		  ヒットした場合、対象プレイヤーのNetIDが代入される
 *
 * @retval  BOOL		TRUE:通信プレイヤーヒット。　FALSE:相手が居ない
 */
//==================================================================
BOOL IntrudeField_CheckTalk(INTRUDE_COMM_SYS_PTR intcomm, const FIELD_PLAYER *fld_player, u32 *hit_netid)
{
  s16 check_gx, check_gy, check_gz;
  u32 out_index;
  
  if(intcomm == NULL || intcomm->cps == NULL){
    return FALSE;
  }
  
  FIELD_PLAYER_GetFrontGridPos(fld_player, &check_gx, &check_gy, &check_gz);
  if(CommPlayer_SearchGridPos(intcomm->cps, check_gx, check_gz, &out_index) == TRUE){
    OS_TPrintf("Talkターゲット発見! net_id = %d, gx=%d, gz=%d\n", 
      out_index, check_gx, check_gz);
    *hit_netid = out_index;
    return TRUE;
  }
  
  return FALSE;
}

//==================================================================
/**
 * 通信プレイヤーから話しかけられていないかチェック
 *
 * @param   intcomm		
 * @param   fld_player		
 * @param   hit_netid		  話しかけられている場合、対象プレイヤーのNetIDが代入される
 *
 * @retval  BOOL		TRUE:通信プレイヤーから話しかけられている　FALSE:話しかけられていない
 */
//==================================================================
BOOL IntrudeField_CheckTalkedTo(INTRUDE_COMM_SYS_PTR intcomm, u32 *hit_netid)
{
  s16 check_gx, check_gy, check_gz;
  u32 out_index;
  
  if(intcomm == NULL || intcomm->cps == NULL){
    return FALSE;
  }
  
  //自分から話しかけていないのに値がNULL以外なのは話しかけられたから
  if(intcomm->talk.talk_netid != INTRUDE_NETID_NULL){
    *hit_netid = intcomm->talk.talk_netid;
    return TRUE;
  }
  
  return FALSE;
}

//==============================================================================
//
//  橋の特定位置まで来て、誰とも通信していないなら注意メッセージを出して自機を戻す
//
//==============================================================================
//--------------------------------------------------------------
/**
 * 橋の特定位置まで来て、誰とも通信していないなら注意メッセージを出して自機を戻す
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   fld_player		
 * @param   dir		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static GMEVENT * EVENT_PalaceNGWin( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *fld_player, u16 dir )
{
  EVENT_PALACE_NGWIN *ngwin;
  GMEVENT * event;
  
  event = GMEVENT_Create(gsys, NULL, _EventPalaceNGWinEvent, sizeof(EVENT_PALACE_NGWIN));
  
  ngwin = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear( ngwin, sizeof(EVENT_PALACE_NGWIN) );
  
  ngwin->fieldWork = fieldWork;
  ngwin->player_mmdl = FIELD_PLAYER_GetMMdl(fld_player);
  ngwin->dir = dir;
  
  return event;
}

//--------------------------------------------------------------
/**
 * 橋の特定位置まで来て、誰とも通信していないなら注意メッセージを出して自機を戻す
 *
 * @param   event		
 * @param   seq		
 * @param   wk		
 *
 * @retval  GMEVENT_RESULT		
 */
//--------------------------------------------------------------
static GMEVENT_RESULT _EventPalaceNGWinEvent( GMEVENT *event, int *seq, void *wk )
{
  EVENT_PALACE_NGWIN *ngwin = wk;
  
  switch(*seq){
  case 0:
    {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(ngwin->fieldWork);
      ngwin->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      ngwin->msgWin = FLDMSGWIN_AddTalkWin( msgBG, ngwin->msgData );
      FLDMSGWIN_Print( ngwin->msgWin, 0, 0, plc_connect_06 );
      (*seq)++;
    }
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(ngwin->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 2:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
        (*seq)++;
      }
    }
    break;
  case 3:
    FLDMSGWIN_Delete( ngwin->msgWin );
    GFL_MSG_Delete( ngwin->msgData );
    (*seq)++;
    break;
  case 4:
    if(MMDL_CheckPossibleAcmd(ngwin->player_mmdl) == TRUE){
      u16 code = (ngwin->dir == DIR_RIGHT) ? AC_WALK_L_16F : AC_WALK_R_16F;
      MMDL_SetAcmd(ngwin->player_mmdl, code);
      (*seq)++;
    }
    break;
  case 5:
    if(MMDL_CheckEndAcmd(ngwin->player_mmdl) == TRUE){
      MMDL_EndAcmd(ngwin->player_mmdl);
      return( GMEVENT_RES_FINISH );
    }
    break;
  }

  return( GMEVENT_RES_CONTINUE );
}

//==============================================================================
//
//  
//
//==============================================================================
//--------------------------------------------------------------
/**
 * イベント：バリアー移動
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   fld_player		
 * @param   dir		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static GMEVENT * EVENT_PalaceBarrierMove( GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *fld_player, u16 dir )
{
  EVENT_PALACE_BARRIER *barrier;
  GMEVENT * event;
  
  event = GMEVENT_Create(gsys, NULL, _EventPalaceBarrierMove, sizeof(EVENT_PALACE_BARRIER));
  
  barrier = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear( barrier, sizeof(EVENT_PALACE_BARRIER) );
  
  barrier->fieldWork = fieldWork;
  barrier->player_mmdl = FIELD_PLAYER_GetMMdl(fld_player);
  barrier->dir = dir;
  
  return event;
}

//--------------------------------------------------------------
/**
 * イベント：バリアー移動
 *
 * @param   event		
 * @param   seq		
 * @param   wk		
 *
 * @retval  GMEVENT_RESULT		
 */
//--------------------------------------------------------------
static GMEVENT_RESULT _EventPalaceBarrierMove( GMEVENT *event, int *seq, void *wk )
{
  EVENT_PALACE_BARRIER *barrier = wk;
  GAMESYS_WORK * gsys = GMEVENT_GetGameSysWork(event);
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  enum{
    _SEQ_INIT,
    _SEQ_INIT_WAIT,
    _SEQ_MSG_INIT,
    _SEQ_MSG_WAIT,
    _SEQ_MSG_LASTKEY_WAIT,
    _SEQ_FINISH,
  };
  
  switch(*seq){
  case _SEQ_INIT:
    if(MMDL_CheckPossibleAcmd(barrier->player_mmdl) == FALSE){
      break;  
    }

    if(intcomm == NULL){  //変身前にエラーチェック：通信切断状態ならばここで終了
      *seq = _SEQ_FINISH;
      break;
    }

    // ミッション用の見た目
    if(MISSION_GetMissionPlay(&intcomm->mission) == TRUE)
    {  
      barrier->no_change = TRUE;

      // 変身なし演出
      IntrudeEvent_Sub_BarrierDisguiseEffectSetup(
        &barrier->ev_diswork, gsys, barrier->fieldWork, intcomm, DISGUISE_NO_NULL, 0, 0, barrier->dir, FALSE); //<-FALSEで変身なし
    }
    // 自分のエリアへ戻る
    else if( Intrude_CheckNextPalaceAreaMine( game_comm, gamedata, barrier->dir ) == TRUE ){ // 移動先が自分のエリアか？
      
      IntrudeEvent_Sub_BarrierDisguiseEffectSetup(
        &barrier->ev_diswork, gsys, barrier->fieldWork, intcomm, DISGUISE_NO_NULL, 0, 0, barrier->dir, TRUE);
    }
    // 通常進入
    else{

      u16 disguise_code;
      u8 disguise_type, disguise_sex;
      Intrude_GetNormalDisguiseObjCode(
        GAMEDATA_GetMyStatus(gamedata), &disguise_code, &disguise_type, &disguise_sex);

      IntrudeEvent_Sub_BarrierDisguiseEffectSetup(&barrier->ev_diswork, gsys, barrier->fieldWork, intcomm,
        disguise_code, disguise_type, disguise_sex, barrier->dir, TRUE);
    }
    (*seq)++;
    break;

  case _SEQ_INIT_WAIT:
    // BARRIER見た目変更処理
    if(IntrudeEvent_Sub_BarrierDisguiseEffectMain(&barrier->ev_diswork, intcomm) == TRUE){
      (*seq)++;
    }
    break;
  case _SEQ_MSG_INIT:
    if(intcomm == NULL){  //メッセージ前にエラーチェック：通信切断状態ならばここで終了
      *seq = _SEQ_FINISH;
      break;
    }
    
    {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(barrier->fieldWork);
      barrier->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      barrier->msgWin = FLDMSGWIN_AddTalkWin( msgBG, barrier->msgData );
      barrier->wordset = WORDSET_Create(HEAPID_PROC);
      
      if(intcomm->intrude_status_mine.palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
        if(barrier->no_change == TRUE){
          FLDMSGWIN_Print( barrier->msgWin, 0, 0, plc_connect_03 );
        }
        else{
          FLDMSGWIN_Print( barrier->msgWin, 0, 0, plc_connect_02 );
        }
      }
      else{
        STRBUF *expand_buf = GFL_STR_CreateBuffer(256, HEAPID_FIELDMAP);
        STRBUF *src_buf;
        
        WORDSET_RegisterPlayerName( barrier->wordset, 0, 
          GAMEDATA_GetMyStatusPlayer(gamedata, intcomm->intrude_status_mine.palace_area) );
        
        if(barrier->no_change == TRUE){
          src_buf = GFL_MSG_CreateString( barrier->msgData, plc_connect_04 );
        }
        else{
          src_buf = GFL_MSG_CreateString( barrier->msgData, plc_connect_05 );
        }
        WORDSET_ExpandStr(barrier->wordset, expand_buf, src_buf);
        FLDMSGWIN_PrintStrBuf( barrier->msgWin, 0, 0, expand_buf );
        GFL_STR_DeleteBuffer(src_buf);
        GFL_STR_DeleteBuffer(expand_buf);
      }
      (*seq)++;
    }
    break;
  case _SEQ_MSG_WAIT:
    if( FLDMSGWIN_CheckPrintTrans(barrier->msgWin) == TRUE ){
      FIELD_PLACE_NAME_DisplayForce( 
        FIELDMAP_GetPlaceNameSys(barrier->fieldWork), FIELDMAP_GetZoneID(barrier->fieldWork) );
      (*seq)++;
    } 
    break;
  case _SEQ_MSG_LASTKEY_WAIT:
    {
      int trg = GFL_UI_KEY_GetTrg();
      if( trg & (PAD_BUTTON_A|PAD_BUTTON_B) ){
        (*seq)++;
      }
    }
    break;
  case _SEQ_FINISH:
    if(barrier->msgWin != NULL){
      FLDMSGWIN_Delete( barrier->msgWin );
      GFL_MSG_Delete( barrier->msgData );
      WORDSET_Delete(barrier->wordset);
    }
    return( GMEVENT_RES_FINISH );
  }

  return( GMEVENT_RES_CONTINUE );
}


//==================================================================
/**
 * パレス：座標イベント
 *
 * @param   fieldWork		
 * @param   gameSys		
 * @param   pcActor		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * Intrude_CheckPosEvent(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor)
{
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
  ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
  VecFx32 pos;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSys);
  INTRUDE_COMM_SYS_PTR intcomm;
  int i;
  
  intcomm = Intrude_Check_CommConnect(game_comm);

  if(intcomm != NULL && intcomm->comm_status == INTRUDE_COMM_STATUS_UPDATE){
    if(intcomm->exit_recv == TRUE){
      return DEBUG_EVENT_ChildCommEnd(gameSys, fieldWork, intcomm);
    }
  }

  FIELD_PLAYER_GetPos( pcActor, &pos );
  
  if(ZONEDATA_IsBingo(zone_id) == TRUE){  //ビンゴマップから脱出監視
    if(pos.z >= FX32_CONST(680)){
    	pos.x = PALACE_MAP_LEN/2;
    	pos.y = 0;
    	pos.z = 408 << FX32_SHIFT;
      return EVENT_ChangeMapPos(gameSys, fieldWork, ZONE_ID_PALACE01, &pos, 0, FALSE);
    }
    return NULL;
  }
  
  
  
  //------------- ここから下はパレス時のみの処理 -----------------
  
  if(ZONEDATA_IsPalace(zone_id) == FALSE){
    return NULL;
  }
  
  //T01へワープ
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(pos.x >= FX32_CONST(504) + PALACE_MAP_LEN*i && pos.x <= FX32_CONST(504) + PALACE_MAP_LEN*i
        && pos.z >= FX32_CONST(712) && pos.z <= FX32_CONST(712)){
      pos.x = 12536 << FX32_SHIFT;
      pos.y = 0;
      pos.z = 12120 << FX32_SHIFT;
      
      if(intcomm != NULL 
//          && GFL_NET_GetConnectNum() > 1 
          && GameCommSys_BootCheck(GAMESYSTEM_GetGameCommSysPtr(gameSys)) == GAME_COMM_NO_INVASION
//          && GFL_NET_IsParentMachine() == TRUE
          && intcomm->intrude_status_mine.palace_area == GFL_NET_SystemGetCurrentID()){
        //親が通信状態で自分の街に入る場合は切断
        return EVENT_ChangeMapPosCommEnd(gameSys, fieldWork, ZONE_ID_T01, &pos, intcomm);
      }
      return EVENT_ChangeMapFromPalace(gameSys);
      //return EVENT_ChangeMapPos(gameSys, fieldWork, ZONE_ID_T01, &pos, 0, FALSE);
    }
  }
  //ビンゴマップへワープ
  for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
    if(pos.x >= FX32_CONST(488) + PALACE_MAP_LEN*i
        && pos.x <= FX32_CONST(536) + PALACE_MAP_LEN*i 
        && pos.z >= FX32_CONST(328) && pos.z <= FX32_CONST(344)){
      pos.x = 248 << FX32_SHIFT;
      pos.y = 0;
      pos.z = 360 << FX32_SHIFT;
      return EVENT_SymbolMapWarp(gameSys, fieldWork, NULL, &pos, 0, 5);
    }
  }
  
#if 0
  {//自機の座標を監視し、誰とも通信していないのにパレスの橋の一定位置まで来たら
   //注意メッセージを出して引き返させる
    BOOL left_right;
    
    if(GFL_NET_GetConnectNum() <= 1 && ((pos.x <= 136 && pos.x >= 136-16) || (pos.x >= 872 && pos.x <= 872+16)) && (pos.z >= 472 && pos.z <= 536)){
      left_right = pos.x <= (PALACE_MAP_LEN/2 >> FX32_SHIFT) ? 0 : 1;
      return EVENT_PalaceNGWin( gameSys, fieldWork, pcActor, left_right );
    }
  }
#endif
  return NULL;
}

//==================================================================
/**
 * 侵入：キー入力起動イベント
 *
 * @param   gsys		
 * @param   fieldWork		
 * @param   pcActor		
 * @param   now_pos		    現在の座標
 * @param   front_pos		  進行方向の座標
 * @param   player_dir		自機の移動方向
 *
 * @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * Intrude_CheckPushEvent(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *pcActor, const VecFx32 *now_pos, const VecFx32 *front_pos, u16 player_dir)
{
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  s32 now_gx;
  u16 event_dir = DIR_NOT;
  
  now_gx = FX32_TO_GRID(now_pos->x) % PALACE_MAP_LEN_GRID;
  
  //パレスマップの橋で左側へ行こうとしている
  if(player_dir == DIR_LEFT){
    if(now_gx == 0){
      event_dir = DIR_LEFT;
    }
  }
  else if(player_dir == DIR_RIGHT){
    if(now_gx == PALACE_MAP_LEN_GRID - 1){
      event_dir = DIR_RIGHT;
    }
  }
  
  if(event_dir != DIR_NOT){
    if(intcomm == NULL || GFL_NET_GetConnectNum() <= 1 || intcomm->member_num < 2){
      return EVENT_PalaceNGWin(gsys, fieldWork, pcActor, event_dir);
    }
    else{
      return EVENT_PalaceBarrierMove(gsys, fieldWork, pcActor, event_dir);
    }
  }
  return NULL;
}


//==================================================================
/**
 * 通信を終了させてからマップ移動
 *
 * @param   gsys		
 * @param   fieldmap		
 * @param   zone_id		
 * @param   pos		
 * @param   dir		
 *
 * @retval  GMEVENT *		
 */
//==================================================================
static GMEVENT * EVENT_ChangeMapPosCommEnd(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap,
		u16 zone_id, const VecFx32 * pos, INTRUDE_COMM_SYS_PTR intcomm )
{
  DEBUG_SIOEND_WARP *dsw;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, DebugEVENT_MapChangeCommEnd, sizeof(DEBUG_SIOEND_WARP));
	dsw = GMEVENT_GetEventWork(event);
	GFL_STD_MemClear(dsw, sizeof(DEBUG_SIOEND_WARP));
	dsw->fieldmap = fieldmap;
	dsw->zoneID = zone_id;
	dsw->pos = *pos;
	dsw->intcomm = intcomm;
	
	OS_TPrintf("親機：通信切断イベント起動\n");
	return event;
}

static GMEVENT_RESULT DebugEVENT_MapChangeCommEnd(GMEVENT * event, int *seq, void*work)
{
	DEBUG_SIOEND_WARP * dsw = work;
	GAMESYS_WORK  * gsys = GMEVENT_GetGameSysWork(event);
	FIELDMAP_WORK * fieldmap = dsw->fieldmap;
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	
	switch (*seq) {
	case 0:
    GameCommSys_ExitReq(game_comm); //通信終了リクエスト
	  {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(dsw->fieldmap);
      dsw->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      dsw->msgWin = FLDMSGWIN_AddTalkWin( msgBG, dsw->msgData );
      FLDMSGWIN_Print( dsw->msgWin, 0, 0, msg_invasion_test06_01 );
      GXS_SetMasterBrightness(-16);
      (*seq)++;
    }
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(dsw->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 2:
    dsw->wait++;
    if(dsw->wait > 60){
      if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_NULL){
        GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
        int i;
        PLAYER_WORK *plwork;
        VecFx32 pos = {0,0,0};
        
        for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
          plwork = GAMEDATA_GetPlayerWork( gamedata , i );
          PLAYERWORK_setPosition(plwork, &pos);
        }
        (*seq)++;
      }
    }
    break;
  case 3:
    FLDMSGWIN_Delete( dsw->msgWin );
    GFL_MSG_Delete( dsw->msgData );
    GXS_SetMasterBrightness(0);
    (*seq)++;
    break;
	case 4:
  	GMEVENT_CallEvent(
  	  //event, EVENT_ChangeMapPos(gsys, dsw->fieldmap, dsw->zoneID, &dsw->pos, 0, FALSE));
      event, EVENT_ChangeMapFromPalace(gsys) );
    (*seq)++;
    break;
	default:
		return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * ※デバッグ用　子機の通信終了
 *
 * @param   gsys		
 * @param   fieldmap		
 *
 * @retval  GMEVENT *		
 */
//--------------------------------------------------------------
static GMEVENT * DEBUG_EVENT_ChildCommEnd(GAMESYS_WORK * gsys, FIELDMAP_WORK * fieldmap, INTRUDE_COMM_SYS_PTR intcomm)
{
  DEBUG_SIOEND_CHILD *dsc;
	GMEVENT * event;

	event = GMEVENT_Create(gsys, NULL, DebugEVENT_ChildCommEnd, sizeof(DEBUG_SIOEND_CHILD));
	dsc = GMEVENT_GetEventWork(event);
	GFL_STD_MemClear(dsc, sizeof(DEBUG_SIOEND_CHILD));
	dsc->fieldmap = fieldmap;
	dsc->intcomm = intcomm;
	
	OS_TPrintf("子機：通信切断イベント起動\n");
	return event;
}

static GMEVENT_RESULT DebugEVENT_ChildCommEnd(GMEVENT * event, int *seq, void*work)
{
  DEBUG_SIOEND_CHILD *dsc = work;
	GAMESYS_WORK  * gsys = GMEVENT_GetGameSysWork(event);
	FIELDMAP_WORK * fieldmap = dsc->fieldmap;
	GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gsys);
	
	switch (*seq) {
	case 0:
    GameCommSys_ExitReq(game_comm); //通信終了リクエスト
	  {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(dsc->fieldmap);
      dsc->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      dsc->msgWin = FLDMSGWIN_AddTalkWin( msgBG, dsc->msgData );
      FLDMSGWIN_Print( dsc->msgWin, 0, 0, msg_invasion_test06_01 );
      GXS_SetMasterBrightness(-16);
      (*seq)++;
    }
    break;
  case 1:
    if( FLDMSGWIN_CheckPrintTrans(dsc->msgWin) == TRUE ){
      (*seq)++;
    } 
    break;
  case 2:
    dsc->wait++;
    if(dsc->wait > 60){
      if(GameCommSys_BootCheck(game_comm) == GAME_COMM_NO_NULL){
        GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
        int i;
        PLAYER_WORK *plwork;
        VecFx32 pos = {0,0,0};
        
        for(i = 0; i < FIELD_COMM_MEMBER_MAX; i++){
          plwork = GAMEDATA_GetPlayerWork( gamedata , i );
          PLAYERWORK_setPosition(plwork, &pos);
        }
        (*seq)++;
      }
    }
    break;
  case 3:
    FLDMSGWIN_Delete( dsc->msgWin );
    GFL_MSG_Delete( dsc->msgData );
    GXS_SetMasterBrightness(0);
    (*seq)++;
    break;
	default:
		return GMEVENT_RES_FINISH;
  }
	return GMEVENT_RES_CONTINUE;
}

//--------------------------------------------------------------
/**
 * 自分のパレス島の中にいるか、橋側にいるかを判定
 *
 * @param   player_pos		
 *
 * @retval  BOOL		    TRUE:橋　FALSE:パレス島
 */
//--------------------------------------------------------------
static BOOL _PlayerPosCheck_PalaceBridge(const VecFx32 *player_pos)
{
  if(player_pos->x >= 0x348000 || player_pos->x <= 0xb8000){
    return TRUE;  //橋の上
  }
  return FALSE;   //自分のパレス島の中
}

//--------------------------------------------------------------
/**
 * パレス島の中心付近にいるか、外側にいるかを判定
 *
 * @param   player_pos		
 *
 * @retval  BOOL		    TRUE:中心の外側　FALSE:中心
 *
 * 連結マップの生成タイミングに使用
 * 隣接したマップが見えないタイミングで連結する
 */
//--------------------------------------------------------------
static BOOL _PlayerPosCheck_PalaceCenter(const VecFx32 *player_pos)
{
  if(player_pos->x > 0x308000 || player_pos->x < 0xe8000){
    return TRUE;  //パレス島の中心から外れている
  }
  return FALSE;   //
}

//--------------------------------------------------------------
/**
 * パレス通信自動起動ON/OFF処理
 *
 * @param   fieldWork		
 * @param   gameSys		
 * @param   game_comm		
 * @param   pcActor		
 */
//--------------------------------------------------------------
static void _PalaceMapCommBootCheck(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, GAME_COMM_SYS_PTR game_comm, FIELD_PLAYER *pcActor)
{
  ZONEID zone_id;
  GAME_COMM_NO comm_no;

  VecFx32 player_pos;

  if(GameCommSys_CheckSystemWaiting(game_comm) == TRUE){
    return;
  }
  
  zone_id = PLAYERWORK_getZoneID(GAMEDATA_GetMyPlayerWork(GAMESYSTEM_GetGameData(gameSys)));
  if(ZONEDATA_IsPalace(zone_id) == FALSE){
    return;
  }
  
  FIELD_PLAYER_GetPos( pcActor, &player_pos );
  comm_no = GameCommSys_BootCheck(game_comm);
  switch(comm_no){
  case GAME_COMM_NO_NULL:
    if(_PlayerPosCheck_PalaceBridge(&player_pos) == TRUE){
      FIELD_INVALID_PARENT_WORK *invalid_parent;
      
      invalid_parent = GFL_HEAP_AllocClearMemory(
          GFL_HEAP_LOWID(HEAPID_PROC), sizeof(FIELD_INVALID_PARENT_WORK));
      invalid_parent->my_invasion = TRUE;
      invalid_parent->gsys = gameSys;
      invalid_parent->game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSys);
      GameCommSys_Boot(game_comm, GAME_COMM_NO_INVASION, invalid_parent);

      OS_TPrintf("パレス通信起動：親\n");
    }
    break;
  case GAME_COMM_NO_INVASION:
    //自分のパレス島にいて接続している相手もいない場合は通信終了
    if(_PlayerPosCheck_PalaceBridge(&player_pos) == FALSE
        && GFL_NET_SystemGetConnectNum() <= 1){
      INTRUDE_COMM_SYS_PTR intcomm = GameCommSys_GetAppWork(game_comm);
      if(intcomm->comm_status != INTRUDE_COMM_STATUS_BOOT_CHILD){ //子として親を探しに行っている場合は通信終了にはまだいかない
        if(GAMESYSTEM_GetAlwaysNetFlag(gameSys) == TRUE){
          GameCommSys_ChangeReq(game_comm, GAME_COMM_NO_FIELD_BEACON_SEARCH, gameSys);
          OS_TPrintf("パレス通信終了：ビーコンサーチへ\n");
        }
        else{
          GameCommSys_ExitReq(game_comm);
          OS_TPrintf("パレス通信終了：通信END\n");
        }
      }
    }
    break;
  case GAME_COMM_NO_FIELD_BEACON_SEARCH:
    //自分のパレス島にいて橋の上にいるならビーコンサーチをOFF
    if(_PlayerPosCheck_PalaceBridge(&player_pos) == TRUE){
      GameCommSys_ExitReq(game_comm);
      OS_TPrintf("ビーコンサーチ終了\n");
    }
    break;
  }
}

//==================================================================
/**
 *   デバッグ用 パレスに飛ぶイベントを作成する
 *  @param   fieldWork		
 *  @param   gameSys		
 *  @param   pcActor		
 *  @retval  GMEVENT *		
 */
//==================================================================
GMEVENT * DEBUG_PalaceJamp(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor)
{
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
  ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
  VecFx32 pos;
  ZONEID jump_zone;
  
  if(GFL_UI_KEY_GetCont() & PAD_BUTTON_R){
  	pos.x = 184 << FX32_SHIFT;
  	pos.y = 0;
  	pos.z = 184 << FX32_SHIFT;
  	jump_zone = ZONE_ID_UNION;
  }
  else{
  	pos.x = PALACE_MAP_LEN/2;
  	pos.y = 0;
  	pos.z = 408 << FX32_SHIFT;
  	jump_zone = ZONE_ID_PALACE01;
  }
  return EVENT_ChangeMapPos(gameSys, fieldWork, jump_zone, &pos, 0, FALSE);
}

//--------------------------------------------------------------
/**
 * パレスマップの端に来たらマップループ処理を行う
 *
 * @param   fieldWork		
 * @param   gameSys		
 * @param   pcActor		
 * @param   intcomm		
 */
//--------------------------------------------------------------
static void _PalaceFieldPlayerWarp(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, FIELD_PLAYER *pcActor, INTRUDE_COMM_SYS_PTR intcomm)
{
  PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gameSys );
  ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
  u16 player_dir = PLAYERWORK_getDirection_Type(plWork);
  VecFx32 pos, new_pos;
  GAME_COMM_SYS_PTR game_comm = GAMESYSTEM_GetGameCommSysPtr(gameSys);
  int i, now_area, new_area;
  BOOL warp = FALSE;
  fx32 left_end, right_end;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gameSys);
  
#if 1
  if(GameCommSys_BootCheck(game_comm) != GAME_COMM_NO_INVASION || intcomm == NULL){
    return;
  }
#endif

  if(ZONEDATA_IsPalace(zone_id) == FALSE){
    return;
  }
  
  if(intcomm->member_num < 2){
    return;
  }
  
  FIELD_PLAYER_GetPos( pcActor, &pos );
  new_pos = pos;
  
  now_area = intcomm->intrude_status_mine.palace_area;
  new_area = now_area;
  
  left_end = 0;
  right_end = PALACE_MAP_LEN * (intcomm->connect_map_count + 1);  //intcomm->member_num;
  
  //マップループチェック
  if(pos.x < left_end){// && player_dir == DIR_LEFT){
    new_pos.x = right_end - (left_end - pos.x);
    warp = TRUE;
    OS_TPrintf("left warp now_pos.x = %d, new_pos.x = %d, left_end=%d, right_end=%d\n", pos.x>>FX32_SHIFT, new_pos.x>>FX32_SHIFT, left_end>>FX32_SHIFT, right_end>>FX32_SHIFT);
  }
  else if(pos.x > right_end){// && player_dir == DIR_RIGHT){
    new_pos.x = left_end + (pos.x - right_end);
    warp = TRUE;
    OS_TPrintf("right warp now_pos.x = %d, new_pos.x = %d, left_end=%d, right_end=%d\n", pos.x>>FX32_SHIFT, new_pos.x>>FX32_SHIFT, left_end>>FX32_SHIFT, right_end>>FX32_SHIFT);
  }
  
  //現在の座標からパレスエリアを判定
  new_area = pos.x / PALACE_MAP_LEN;

  if(warp == TRUE){
    FIELD_PLAYER_SetPos( pcActor, &new_pos );
    Intrude_PlayerPosRecalc(intcomm, new_pos.x);
  }
  
  new_area = new_area + GFL_NET_SystemGetCurrentID();
  if(new_area >= intcomm->member_num){
    new_area -= intcomm->member_num;
  }
  if(now_area != new_area){
    OS_TPrintf("new_palace_area = %d new_pos_x = %x\n", new_area, new_pos.x);
    intcomm->intrude_status_mine.palace_area = new_area;
    intcomm->send_status = TRUE;
  #if 0 //パレス島に来ただけでフラグONにするようにした 2010.03.10(水)
    if(GAMEDATA_GetIntrudeMyID(gamedata) == new_area){
      GAMEDATA_SetIntrudeReverseArea(gamedata, FALSE);
    }
    else{
      GAMEDATA_SetIntrudeReverseArea(gamedata, TRUE);
    }
  #endif

    // WFBCCOMMに新通信先を通知
    FIELD_WFBC_COMM_DATA_SetIntrudeNetID( &intcomm->wfbc_comm_data, new_area );
  }
}

//==================================================================
/**
 * 端分のマップワープ場所を連結
 *
 * @param   fieldWork		
 * @param   gameSys		
 * 
 * wb -- col wb wb wb -
 */
//==================================================================
void IntrudeField_ConnectMapInit(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, INTRUDE_COMM_SYS_PTR intcomm)
{
#if 0
  MAP_MATRIX *mmatrix = MAP_MATRIX_Create( HEAPID_WORLD );
  
  MAP_MATRIX_Init(mmatrix, NARC_map_matrix_palace01_mat_bin, ZONE_ID_PALACE01);
  
  OS_TPrintf("--- Map連結 default ----\n");
  FLDMAPPER_Connect( FIELDMAP_GetFieldG3Dmapper( fieldWork ), mmatrix );

  MAP_MATRIX_Delete( mmatrix );
#else
  //パレスマップに入った時点で1つは自動で連結しているが、
  //この処理を通る場合はIntrudeField_ConnectMapに連結数を任せる為、
  //0初期化する
  intcomm->connect_map_count = 0;
#endif
}

//==================================================================
/**
 * プロフィール受信人数分マップを連結
 *
 * @param   fieldWork		
 * @param   gameSys		
 * @param   intcomm		
 * 
 * wb -- col wb wb wb -
 */
//==================================================================
void IntrudeField_ConnectMap(FIELDMAP_WORK *fieldWork, GAMESYS_WORK *gameSys, INTRUDE_COMM_SYS_PTR intcomm)
{
  int use_num;
  
  if(intcomm == NULL || fieldWork == NULL || ZONEDATA_IsPalace(FIELDMAP_GetZoneID(fieldWork)) == FALSE){
    return;
  }
  
#if 0 //ループマップなのに1つも連結されていないのはチラつきの原因っぽいので
      //最低でも一つは連結しておく 2010.01.23(土)
  use_num = intcomm->member_num - 1;  // -1 = 自分の分は引く
#else
  use_num = intcomm->member_num - 1;
  if(use_num <= 0){
    use_num = 1;
  }
#endif
  if(intcomm->connect_map_count < use_num){
    MAP_MATRIX *mmatrix;
    
    //隣接するマップが見えている場合は連結処理はしない
    {
      VecFx32 player_pos;
      FIELD_PLAYER *fld_player = FIELDMAP_GetFieldPlayer( fieldWork );
      FIELD_PLAYER_GetPos( fld_player, &player_pos );
      player_pos.x %= PALACE_MAP_LEN;
      if(_PlayerPosCheck_PalaceCenter(&player_pos) == TRUE){
        return;
      }
    }

    mmatrix = MAP_MATRIX_Create( GFL_HEAP_LOWID(HEAPID_FIELDMAP) );
    MAP_MATRIX_Init(mmatrix,
        NARC_map_matrix_palace02_mat_bin, ZONE_ID_PALACE01, GFL_HEAP_LOWID( HEAPID_FIELDMAP ) );

    do{
      OS_TPrintf("--- Map連結 %d ----\n", intcomm->connect_map_count + 1);
      FLDMAPPER_Connect( fieldWork, FIELDMAP_GetFieldG3Dmapper( fieldWork ), mmatrix );
      intcomm->connect_map_count++;
    }while(intcomm->connect_map_count < use_num);

    MAP_MATRIX_Delete( mmatrix );
  }
}

//==================================================================
/**
 * マップ単品連結
 *
 * @param   fieldWork		
 */
//==================================================================
void IntrudeField_ConnectMapOne(FIELDMAP_WORK *fieldWork)
{
  MAP_MATRIX *mmatrix = MAP_MATRIX_Create( HEAPID_FIELDMAP );
  MAP_MATRIX_Init(mmatrix,
      NARC_map_matrix_palace02_mat_bin, ZONE_ID_PALACE01, GFL_HEAP_LOWID( HEAPID_FIELDMAP ));

  OS_TPrintf("--- Map連結 One ----\n");
  FLDMAPPER_Connect( fieldWork, FIELDMAP_GetFieldG3Dmapper( fieldWork ), mmatrix );

  MAP_MATRIX_Delete( mmatrix );
}

//==================================================================
/**
 * 自機を変装させる(OBJコードの変更)
 *
 * @param   intcomm		         NULLを渡した場合は通常の姿に戻します
 * @param   gsys		
 * @param   disguise_code      変装後のOBJCODE 
 *                  (DISGUISE_NO_NULLの場合は通常の姿、DISGUISE_NO_NORMALの場合はパレス時の標準姿)
 * @param   disguise_type      変装後のタイプ(TALK_TYPE_xxx)
 * @param   disguise_sex       変装後の性別
 */
//==================================================================
void IntrudeField_PlayerDisguise(INTRUDE_COMM_SYS_PTR intcomm, GAMESYS_WORK *gsys, u32 disguise_code, u8 disguise_type, u8 disguise_sex)
{
  FIELDMAP_WORK *fieldWork;
  FIELD_PLAYER *fld_player;
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  int obj_code;
  int sex;
  MYSTATUS *myst;
  
  if(GAMESYSTEM_CheckFieldMapWork( gsys ) == FALSE){
    return;
  }
  
  myst = GAMEDATA_GetMyStatus(GAMESYSTEM_GetGameData(gsys));
  sex = MyStatus_GetMySex(myst);
  
  if(intcomm != NULL){
    if(disguise_code == DISGUISE_NO_NORMAL 
        && GAMEDATA_GetIntrudeMyID(gamedata) == intcomm->intrude_status_mine.palace_area){
      disguise_code = DISGUISE_NO_NULL; //自分のパレスにいる場合はNORMALはNULLにする
    }
    
    if(disguise_code == DISGUISE_NO_NULL){
      disguise_type = sex == PM_MALE ? TALK_TYPE_MAN : TALK_TYPE_WOMAN;
      disguise_sex = sex;
    }

    intcomm->intrude_status_mine.disguise_no = disguise_code;
    intcomm->intrude_status_mine.disguise_type = disguise_type;
    intcomm->intrude_status_mine.disguise_sex = disguise_sex;
    intcomm->send_status = TRUE;

    obj_code = Intrude_GetObjCode(&intcomm->intrude_status_mine, myst);
  }
  else{
    disguise_code = DISGUISE_NO_NULL;
    obj_code = sex == PM_MALE ? HERO : HEROINE;
  }

  fieldWork = GAMESYSTEM_GetFieldMapWork(gsys);
  fld_player = FIELDMAP_GetFieldPlayer( fieldWork );

  FIELD_PLAYER_ChangeOBJCode( fld_player, obj_code );
  if(disguise_code == DISGUISE_NO_NULL){
    FIELD_PLAYER_ClearOBJCodeFix( fld_player );
  }
}

//--------------------------------------------------------------
/**
 * 隠しアイテムミッションが発動している時に目的地に到達したかチェック
 *
 * @param   gsys		
 * @param   intcomm		
 * @param   pcActor		
 *
 * @retval  MISSION_DATA *		到達した場合はミッションデータへのポインタ
 * @retval  MISSION_DATA *		到達していない場合はNULL
 */
//--------------------------------------------------------------
GMEVENT * IntrudeField_CheckSecretItemEvent(GAMESYS_WORK *gsys, INTRUDE_COMM_SYS_PTR intcomm, const FIELD_PLAYER *pcActor)
{
  if(MISSION_GetMissionEntry(&intcomm->mission) == TRUE
      && MISSION_GetMissionComplete(&intcomm->mission) == FALSE){
    const MISSION_DATA *mdata = MISSION_GetRecvData(&intcomm->mission);
    if(mdata != NULL && mdata->cdata.type == MISSION_TYPE_ITEM 
        && intcomm->intrude_status_mine.palace_area == mdata->target_info.net_id){
      const MISSION_TYPEDATA_ITEM *d_item = (void*)mdata->cdata.data;
      PLAYER_WORK *plWork = GAMESYSTEM_GetMyPlayerWork( gsys );
      ZONEID zone_id = PLAYERWORK_getZoneID( plWork );
    	const MMDL *fmmdl = FIELD_PLAYER_GetMMdl( pcActor );
    	MMDL_GRIDPOS grid_pos;
    	
      MMDL_GetGridPos( fmmdl, &grid_pos );
      if(IntrudeSecretItemReverseZoneTbl[d_item->secret_pos_tblno] == zone_id
          && IntrudeSecretItemPosDataTbl[d_item->secret_pos_tblno].grid_x == grid_pos.gx
          && IntrudeSecretItemPosDataTbl[d_item->secret_pos_tblno].grid_y == grid_pos.gy
          && IntrudeSecretItemPosDataTbl[d_item->secret_pos_tblno].grid_z == grid_pos.gz){
        return EVENT_Intrude_SecretItemArrivalEvent(gsys, intcomm, mdata);
      }
    }
  }
  
  return NULL;
}

//==================================================================
/**
 * ゾーンIDと一致するパレス設定データへのポインタを取得する
 *
 * @param   zone_id		
 *
 * @retval  const PALACE_ZONE_SETTING *		一致するデータが無い場合はNULL
 */
//==================================================================
const PALACE_ZONE_SETTING * IntrudeField_GetZoneSettingData(u16 zone_id)
{
  int i;
  
  for(i = 0; i < NELEMS(PalaceZoneSetting); i++){
    if(PalaceZoneSetting[i].zone_id == zone_id){
      return &PalaceZoneSetting[i];
    }
  }
  return NULL;
}

//==================================================================
/**
 * パレスタウンへワープする為の座標取得
 *
 * @param   zone_id		ワープ先ゾーンID
 * @param   vec		    ワープ先座標代入先
 *
 * @retval  BOOL		  TRUE:正常終了　FALSE:パレスに関連するゾーンIDではない為、データ未取得
 */
//==================================================================
BOOL IntrudeField_GetPalaceTownZoneID(u16 zone_id, VecFx32 *vec)
{
  if(ZONEDATA_IsPalace(zone_id) != TRUE){
    const PALACE_ZONE_SETTING *zonesetting = IntrudeField_GetZoneSettingData(zone_id);
    if(zonesetting == NULL){
      GF_ASSERT_MSG(0, "zone_id = %d", zone_id);
      return FALSE;
    }
    VEC_Set(vec, GRID_SIZE_FX32(zonesetting->warp_grid_x), 
      GRID_SIZE_FX32(zonesetting->warp_grid_y), GRID_SIZE_FX32(zonesetting->warp_grid_z));
  }
  else{
    *vec = PalaceWarpPos;
  }
  return TRUE;
}

//==================================================================
/**
 * モノリスステータスを初期化する
 *
 * @param   monost		
 */
//==================================================================
void IntrudeField_MonolithStatus_Init(MONOLITH_STATUS *monost)
{
  GFL_STD_MemClear(monost, sizeof(MONOLITH_STATUS));
}

//==================================================================
/**
 * モノリスステータスにデータが入っているかを確かめる
 *
 * @param   monost		
 * @retval  BOOL		TRUE:入っている　FALSE:入っていない
 */
//==================================================================
BOOL IntrudeField_MonolithStatus_CheckOcc(const MONOLITH_STATUS *monost)
{
  return monost->occ;
}

//--------------------------------------------------------------
// 通信相手のパレスにいる爺さんや4姉妹の動作モデルヘッダー
//--------------------------------------------------------------
static const MMDL_HEADER data_MMdlHeader =
{
	100,	          ///<識別ID  fldmmdl_code.h
	                //  スクリプト上でX番の動作モデルをアニメ、といった判別に使用。
	                //  evファイル上で被らなければよい。
	OLDMAN1,      	///<表示するOBJコード fldmmdl_code.h
	MV_DOWN,	      ///<動作コード    
	EV_TYPE_NORMAL,	///<イベントタイプ
	0,	            ///<イベントフラグ  タイプ毎に使用されるフラグ。外側から取得する
	SCRID_PALACE01_OLDMAN1_01,///<イベントID 話しかけた時に起動するスクリプトID(*.ev の _EVENT_DATAの番号)
	DIR_DOWN,	      ///<指定方向  グリッド移動ならばDIR系 ノングリッドの場合は違う値
	0,	            ///<指定パラメタ 0  タイプ毎に使用されるパラメタ。外側から取得する
	0,            	///<指定パラメタ 1
	0,	            ///<指定パラメタ 2
	MOVE_LIMIT_NOT,	///<X方向移動制限
	MOVE_LIMIT_NOT,	///<Z方向移動制限
  MMDL_HEADER_POSTYPE_GRID,
  {0},            ///<ポジションバッファ ※check　グリッドマップの場合はここはMMDL_HEADER_GRIDPOS
};


//--------------------------------------------------------------
/**
 * 動作モデルをAdd
 *
 * @param   fieldWork		
 * @param   grid_x		グリッド座標X
 * @param   grid_z		グリッド座標Z
 * @param   fx_y		  Y座標(fx32)
 * @param   objcode		  OBJCODE
 * @param   event_id		イベントID
 * @param   id		      識別ID
 */
//--------------------------------------------------------------
static void IntrudeField_AddMMdl(FIELDMAP_WORK *fieldWork, u16 grid_x, u16 grid_z, fx32 fx_y, u16 objcode, u16 event_id, u16 id)
{
  MMDL_HEADER head;
  MMDL_HEADER_GRIDPOS *grid_pos;
  MMDLSYS *mmdl_sys;
  ZONEID zone_id;
  
  mmdl_sys = FIELDMAP_GetMMdlSys( fieldWork );
  zone_id = FIELDMAP_GetZoneID( fieldWork );
  
  head = data_MMdlHeader;
  head.id = id;
  head.obj_code = objcode;
  head.event_id = event_id;
  grid_pos = (MMDL_HEADER_GRIDPOS *)head.pos_buf;
  
  grid_pos->gx = grid_x;
  grid_pos->gz = grid_z;
  grid_pos->y = fx_y;
  
  MMDLSYS_AddMMdl(mmdl_sys, &head, zone_id);
}

//==================================================================
/**
 * 通信相手のパレスにいる爺さんや4姉妹の動作モデルをまとめて全てAdd
 *
 * @param   fieldWork		
 */
//==================================================================
void IntrudeField_PalaceMMdlAllAdd(FIELDMAP_WORK *fieldWork)
{
  int i, comm_no;
  static const struct{
    u16 objcode;
    u16 grid_x;
    u16 grid_z;
    u16 event_id;
    u16 obj_id;
  }PalaceMmdlData[] = {
    {OLDMAN1, 29, 30, SCRID_PALACE01_OLDMAN1_01, OBJID_PALACE_OLDMAN},
//    {GIRL4, 14, 29, SCRID_PALACE01_GIRL4_01},
//    {GIRL4, 48, 29, SCRID_PALACE01_GIRL4_02},
  };
  
  for(comm_no = 1; comm_no < FIELD_COMM_MEMBER_MAX; comm_no++){
    for(i = 0; i < NELEMS(PalaceMmdlData); i++){
      IntrudeField_AddMMdl(fieldWork, 
        PalaceMmdlData[i].grid_x + PALACE_MAP_LEN_GRID * comm_no, PalaceMmdlData[i].grid_z, 
        0x2001f, PalaceMmdlData[i].objcode, PalaceMmdlData[i].event_id, PalaceMmdlData[i].obj_id);
    }
  }
}



#ifdef PM_DEBUG
GMEVENT * DEBUG_EVENT_PalaceBarrierMove(GAMESYS_WORK *gsys, FIELDMAP_WORK *fieldWork, FIELD_PLAYER *fld_player, u16 dir)
{
  return EVENT_PalaceBarrierMove( gsys, fieldWork, fld_player, dir );
}

#endif
