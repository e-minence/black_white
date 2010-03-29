//==============================================================================
/**
 * @file    intrude_work.c
 * @brief   侵入システムワークに外部から触る為の関数群
 * @author  matsuda
 * @date    2009.10.10(土)
 */
//==============================================================================
#include <gflib.h>
#include "system/main.h"
#include "gamesystem/game_data.h"
#include "gamesystem/game_comm.h"
#include "savedata/mystatus.h"
#include "system/net_err.h"
#include "net/network_define.h"
#include "field/intrude_comm.h"
#include "intrude_comm_command.h"
#include "net_app/union/comm_player.h"
#include "intrude_types.h"
#include "intrude_comm_command.h"
#include "intrude_main.h"
#include "bingo_types.h"
#include "intrude_work.h"
#include "field/field_status.h"
#include "field/field_status_local.h"


//==================================================================
/**
 * アクションステータスを設定
 *
 * @param   game_comm		
 * @param   action		
 */
//==================================================================
void IntrudeWork_SetActionStatus(GAME_COMM_SYS_PTR game_comm, INTRUDE_ACTION action)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return;
  }
  
  Intrude_SetActionStatus(intcomm, action);
  intcomm->send_status = TRUE;
}

//==================================================================
/**
 * ワープするプレイヤー相手を設定
 *
 * @param   game_comm		
 * @param   town_tblno		
 */
//==================================================================
void Intrude_SetWarpPlayerNetID(GAME_COMM_SYS_PTR game_comm, int net_id)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return;
  }
  
  intcomm->warp_player_netid = net_id;
}

//==================================================================
/**
 * ワープ先のプレイヤー相手を取得
 *
 * @param   game_comm		
 *
 * @retval  int		街番号(PALACE_TOWN_DATA_NULLの場合は無効)
 */
//==================================================================
int Intrude_GetWarpPlayerNetID(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return GAMEDATA_GetIntrudeMyID( GameCommSys_GetGameData(game_comm) );
  }
  return intcomm->warp_player_netid;
}

//==================================================================
/**
 * ワープする街番号を設定
 *
 * @param   game_comm		
 * @param   town_tblno		
 */
//==================================================================
void Intrude_SetWarpTown(GAME_COMM_SYS_PTR game_comm, int town_tblno)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return;
  }
  
  intcomm->warp_town_tblno = town_tblno;
}

//==================================================================
/**
 * ワープ先の街番号を取得
 *
 * @param   game_comm		
 *
 * @retval  int		街番号(PALACE_TOWN_DATA_NULLの場合は無効)
 */
//==================================================================
int Intrude_GetWarpTown(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm == NULL){
    return PALACE_TOWN_DATA_NULL;
  }
  return intcomm->warp_town_tblno;
}

//==================================================================
/**
 * 相手指定のタイミングコマンドが受信できているか確認します
 *
 * @param   intcomm		
 * @param   timing_no		    同期番号
 * @param   target_netid		同期相手のNetID
 *
 * @retval  BOOL		TRUE:同期取り完了。　FALSE:同期待ち
 */
//==================================================================
BOOL Intrude_GetTargetTimingNo(INTRUDE_COMM_SYS_PTR intcomm, u8 timing_no, u8 target_netid)
{
  u8 recv_timing_no = intcomm->recv_target_timing_no;
  
  if(timing_no == recv_timing_no && target_netid == intcomm->recv_target_timing_netid){
    intcomm->recv_target_timing_no = 0;
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * 指定したパレスエリアの占拠情報を取得する
 *
 * @param   intcomm		
 * @param   palace_area		  
 *
 * @retval  OCCUPY_INFO *		
 */
//==================================================================
OCCUPY_INFO * Intrude_GetOccupyInfo(INTRUDE_COMM_SYS_PTR intcomm, int palace_area)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  OCCUPY_INFO *occupy;
  
  if(palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    occupy = GAMEDATA_GetMyOccupyInfo(gamedata);
  }
  else{
    occupy = GAMEDATA_GetOccupyInfo(gamedata, palace_area);
  }
  
  return occupy;
}

//==================================================================
/**
 * 指定したNetIDのMYSTATUSを取得する
 *
 * @param   intcomm		
 * @param   net_id	  
 *
 * @retval  MYSTATUS *		
 */
//==================================================================
MYSTATUS * Intrude_GetMyStatus(INTRUDE_COMM_SYS_PTR intcomm, int net_id)
{
  GAMEDATA *gamedata = GameCommSys_GetGameData(intcomm->game_comm);
  MYSTATUS *myst;
  
  if(net_id == GAMEDATA_GetIntrudeMyID(gamedata)){
    myst = GAMEDATA_GetMyStatus(gamedata);
  }
  else{
    myst = GAMEDATA_GetMyStatusPlayer(gamedata, net_id);
  }
  
  return myst;
}

//==================================================================
/**
 * 指定パレスエリアのミッション選択候補リストのポインタを取得
 *
 * @param   intcomm		
 * @param   palace_area		パレスエリア
 *
 * @retval  MISSION_CHOICE_LIST *		ミッション選択候補リストへのポインタ
 */
//==================================================================
MISSION_CHOICE_LIST * Intrude_GetChoiceList(INTRUDE_COMM_SYS_PTR intcomm, int palace_area)
{
  return &intcomm->mission.list[palace_area];
}

//==================================================================
/**
 * 自分が今いるパレスエリアNoを取得する
 *
 * @param   intcomm		
 *
 * @retval  u8		パレスエリアNo
 */
//==================================================================
u8 Intrude_GetPalaceArea(INTRUDE_COMM_SYS_PTR intcomm)
{
  return intcomm->intrude_status_mine.palace_area;
}

//==================================================================
/**
 * WFBCパラメータを受信したかを取得
 *
 * @param   intcomm		
 *
 * @retval  BOOL		TRUE:受信した　FALSE:受信していない
 */
//==================================================================
BOOL Intrude_GetRecvWfbc(INTRUDE_COMM_SYS_PTR intcomm)
{
  return intcomm->wfbc_recv;
}

//==================================================================
/**
 * 新規ミッション受信フラグを取得   ※check 現状使用していないので削除候補 2010.02.02(火) 毎フレーム監視している使い方でないと機能しないので再度使う場合は注意
 *
 * @param   intcomm		
 *
 * @retval  BOOL		TRUE:受信した　FALSE:受信していない
 */
//==================================================================
BOOL Intrude_GetNewMissionRecvFlag(INTRUDE_COMM_SYS_PTR intcomm)
{
  BOOL new_mission = intcomm->new_mission_recv;
  intcomm->new_mission_recv = FALSE;
  return new_mission;
}

//==================================================================
/**
 * WFBC通信ワークを取得
 *
 * @param   intcomm		
 *
 * @retval  WFBC_COMM_DATA *		WFBC通信ワークへのポインタ
 */
//==================================================================
WFBC_COMM_DATA * Intrude_GetWfbcCommData(INTRUDE_COMM_SYS_PTR intcomm)
{
  return &intcomm->wfbc_comm_data;
}

//==================================================================
/**
 * 自分が現在いる侵入先のPM_VERSIONを取得
 *
 * @param   game_comm		
 *
 * @retval  int		自分がいるROMのPM_VERSION
 *                (通信していない、侵入していない、通信エラー中は自分のPM_VERSION)
 */
//==================================================================
u8 Intrude_GetRomVersion(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  const MYSTATUS *myst;
  u8 palace_area;
  
  if(intcomm == NULL){
    return GET_VERSION();
  }
  
  palace_area = Intrude_GetPalaceArea(intcomm);
  myst = Intrude_GetMyStatus(intcomm, palace_area);
  return MyStatus_GetRomCode( myst );
}

//==================================================================
/**
 * 自分が現在いる侵入先の季節を取得
 *
 * @param   game_comm		
 *
 * @retval  u8		自分がいるROMの季節ID
 *                (通信していない、侵入していない、通信エラー中は自分のROMの季節)
 */
//==================================================================
u8 Intrude_GetSeasonID(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  GAMEDATA *gamedata = GameCommSys_GetGameData(game_comm);
  u8 palace_area;
  
  if(intcomm == NULL){
    return GAMEDATA_GetSeasonID(gamedata);
  }
  
  palace_area = Intrude_GetPalaceArea(intcomm);
#if 0
  //※check まだ未作成
  return intcomm->intrude_status[palace_area].season_id;
#else
  return GAMEDATA_GetSeasonID(gamedata);
#endif
}

//==================================================================
/**
 * 隣のパレスが自分のパレスかを調べる
 *
 * @param   game_comm		
 * @param   gamedata		
 * @param   dir		      調べる隣の向き(DIR_LEFT or DIR_RIGHT)
 *
 * @retval  BOOL		    TRUE：自分のパレス　FALSE:自分ではない人のパレス
 */
//==================================================================
BOOL Intrude_CheckNextPalaceAreaMine(GAME_COMM_SYS_PTR game_comm, const GAMEDATA *gamedata, u16 dir)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  int palace_area;
  
  if(intcomm == NULL){
    return TRUE;
  }
  
  palace_area = intcomm->intrude_status_mine.palace_area;
  if(dir == DIR_LEFT){
    palace_area--;
    if(palace_area < 0){
      palace_area = intcomm->connect_map_count; //intcomm->member_num - 1;
    }
  }
  else{
    palace_area++;
    if(palace_area >= intcomm->connect_map_count + 1){ //intcomm->member_num){
      palace_area = 0;
    }
  }
  
  if(palace_area == GAMEDATA_GetIntrudeMyID(gamedata)){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * グレースケールに変更する必要があるマップかを調べる
 *
 * @param   game_comm		
 * @param   gamedata		
 *
 * @retval  BOOL		    TRUE:グレースケールにする必要がある
 * @retval  BOOL		    FALSE:通常のまま
 */
//==================================================================
BOOL Intrude_CheckGrayScaleMap(GAME_COMM_SYS_PTR game_comm, GAMEDATA *gamedata)
{
  u8 invasion_netid;
  
  if (FIELD_STATUS_GetMapMode( GAMEDATA_GetFieldStatus( gamedata) ) != MAPMODE_INTRUDE ) {
    return FALSE;
  }
  
  invasion_netid = GameCommStatus_GetPlayerStatus_InvasionNetID(game_comm, GAMEDATA_GetIntrudeMyID(gamedata));
  if(invasion_netid == GAMEDATA_GetIntrudeMyID(gamedata)){
    return FALSE;
  }
  
  return TRUE;
}

//==================================================================
/**
 * パレスで誰かと繋がっているかを取得する
 *
 * @param   game_comm		
 *
 * @retval  BOOL		    TRUE:パレスで誰かと接続状態
 * @retval  BOOL		    FALSE:誰とも繋がっていない
 */
//==================================================================
BOOL Intrude_CheckPalaceConnect(GAME_COMM_SYS_PTR game_comm)
{
  INTRUDE_COMM_SYS_PTR intcomm = Intrude_Check_CommConnect(game_comm);
  
  if(intcomm->recv_profile > 1 && GFL_NET_GetConnectNum() > 1){
    return TRUE;
  }
  return FALSE;
}
