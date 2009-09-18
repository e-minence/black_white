//==============================================================================
/**
 * @file    colosseum_tool.c
 * @brief   コロシアムツール類：オーバーレイに配置
 * @author  matsuda
 * @date    2009.07.18(土)
 */
//==============================================================================
#include <gflib.h>
#include "net_app/union/colosseum.h"
#include "net_app/union/colosseum_types.h"
#include "field\fieldmap_proc.h"
#include "app\trainer_card.h"
#include "colosseum_types.h"
#include "system/main.h"
#include "comm_player.h"
#include "field\field_player.h"
#include "field\fieldmap.h"
#include "colosseum_comm_command.h"
#include "colosseum_tool.h"
#include "fieldmap/zone_id.h"


//==============================================================================
//  構造体定義
//==============================================================================
///s16のポイント型
typedef struct{
  s16 x;
  s16 z;
}POINT_S16;

//==============================================================================
//  データ
//==============================================================================
//--------------------------------------------------------------
//  立ち位置座標
//--------------------------------------------------------------
///1vs1の時の立ち位置座標(グリッド座標)
static const POINT_S16 ColosseumStandingPos_1vs1[] = {
  {4, 5},   //左
  {8, 5},  //右
};

///マルチの時の立ち位置座標(グリッド座標)
static const POINT_S16 ColosseumStandingPos_multi[] = {
  {4, 4},     //左上
  {8, 4},     //右上
  {4, 6},     //左下
  {8, 6},     //右下
};

//--------------------------------------------------------------
//  出口座標
//--------------------------------------------------------------
///1vs1の時の立ち位置座標
static const POINT_S16 ColosseumWayOutPos_1vs1[] = {
  {88, 168},
  {104, 168},
  {120, 168},
};

///マルチの時の立ち位置座標
static const POINT_S16 ColosseumWayOutPos_multi[] = {
  {88, 168},
  {104, 168},
  {120, 168},
};



//==================================================================
/**
 * 全員分の基本情報が受け取れているか調べる
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:全員分受信している
 */
//==================================================================
BOOL ColosseumTool_AllReceiveCheck_BasicStatus(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i, count;
  
  count = 0;
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->basic_status[i].occ == TRUE){
      count++;
    }
  }
  if(count >= GFL_NET_GetConnectNum()){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * 全員分のトレーナーカードが受け取れているか調べる
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:全員分受信している
 */
//==================================================================
BOOL ColosseumTool_AllReceiveCheck_TrainerCard(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i, count;
  
  count = 0;
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->recvbuf.tr_card_occ[i] == TRUE){
      count++;
    }
  }
  if(count >= GFL_NET_GetConnectNum()){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * 全員分のPOKEPARTYが受け取れているか調べる
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:全員分受信している
 */
//==================================================================
BOOL ColosseumTool_AllReceiveCheck_Pokeparty(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i, count;
  
  count = 0;
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->recvbuf.pokeparty_occ[i] == TRUE){
      count++;
    }
  }
  if(count >= GFL_NET_GetConnectNum()){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * 受信バッファのクリア：POKEPARTY
 *
 * @param   clsys		
 * @param   except_for_mine   TRUE:自分のデータはクリアしない(受信フラグは落とす)
 */
//==================================================================
void ColosseumTool_Clear_ReceivePokeParty(COLOSSEUM_SYSTEM_PTR clsys, BOOL except_for_mine)
{
  int i, my_net_id;
  
  my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(except_for_mine == FALSE || my_net_id != i){
      PokeParty_InitWork(clsys->recvbuf.pokeparty[i]);
    }
    clsys->recvbuf.pokeparty_occ[i] = FALSE;
  }
}

//==================================================================
/**
 * 全員分の立ち位置が受け取れているか調べる
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:OK
 */
//==================================================================
BOOL ColosseumTool_ReceiveCheck_StandingPos(COLOSSEUM_SYSTEM_PTR clsys)
{
  if(clsys->recvbuf.stand_position_occ == TRUE){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * 受信バッファのクリア：全員分の立ち位置受信
 *
 * @param   clsys		
 */
//==================================================================
void ColosseumTool_Clear_ReceiveStandingPos(COLOSSEUM_SYSTEM_PTR clsys)
{
  clsys->recvbuf.stand_position_occ = FALSE;
}

//==================================================================
/**
 * ポケモンリスト選択完了人数を取得する
 *
 * @param   clsys		
 *
 * @retval  u8		  選択完了人数(自分含む)
 */
//==================================================================
u8 ColosseumTool_ReceiveCheck_PokeListSeletedNum(COLOSSEUM_SYSTEM_PTR clsys)
{
  return clsys->recvbuf.pokelist_selected_num;
}

//==================================================================
/**
 * 受信バッファのクリア：ポケモンリスト選択完了人数
 *
 * @param   clsys		
 */
//==================================================================
void ColosseumTool_Clear_ReceivePokeListSelected(COLOSSEUM_SYSTEM_PTR clsys)
{
  clsys->recvbuf.pokelist_selected_num = 0;
}

//==================================================================
/**
 * 立ち位置座標にいるかチェック
 *
 * @param   fieldWork		
 * @param   entry_num		参加人数
 * @param   stand_pos		結果代入先(立ち位置にいる場合、何番目の立ち位置かが返る)
 *
 * @retval  TRUE:立ち位置にいる。　FALSE:立ち位置にいない
 */
//==================================================================
BOOL ColosseumTool_CheckStandingPosition(FIELD_MAIN_WORK *fieldWork, int entry_num, int *stand_pos)
{
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  int i;
  s16 grid_x, grid_z;
  MMDL *player_mmdl;
  
  *stand_pos = 0;
  
  player_mmdl = FIELD_PLAYER_GetMMdl( player );
  grid_x = MMDL_GetGridPosX(player_mmdl);
  grid_z = MMDL_GetGridPosZ(player_mmdl);

  if(entry_num == 2){
    for(i = 0; i < NELEMS(ColosseumStandingPos_1vs1); i++){
      if(grid_x == ColosseumStandingPos_1vs1[i].x && grid_z == ColosseumStandingPos_1vs1[i].z){
        *stand_pos = i;
        return TRUE;
      }
    }
  }
  else{
    for(i = 0; i < NELEMS(ColosseumStandingPos_multi); i++){
      if(grid_x == ColosseumStandingPos_multi[i].x && grid_z == ColosseumStandingPos_multi[i].z){
        *stand_pos = i;
        return TRUE;
      }
    }
  }
  
  return FALSE;
}

//==================================================================
/**
 * 全員分の「退出します」が受け取れているか調べる
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:全員分受信している
 */
//==================================================================
BOOL ColosseumTool_AllReceiveCheck_Leave(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i, count;
  
  count = 0;
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->recvbuf.leave[i] == TRUE){
      count++;
    }
  }
  if(count >= GFL_NET_GetConnectNum()){
    return TRUE;
  }
  return FALSE;
}

//==================================================================
/**
 * 誰かから「退出します」フラグが来ていないか調べる
 *
 * @param   clsys		
 *
 * @retval  BOOL		TRUE:フラグが来ている
 */
//==================================================================
BOOL ColosseumTool_ReceiveCheck_Leave(COLOSSEUM_SYSTEM_PTR clsys)
{
  int i;
  for(i = 0; i < COLOSSEUM_MEMBER_MAX; i++){
    if(clsys->recvbuf.leave[i] == TRUE){
      return TRUE;
    }
  }
  return FALSE;
}

//==================================================================
/**
 * コロシアムの出口座標にいるかチェック
 *
 * @param   fieldWork		
 *
 * @retval  BOOL		TRUE:出口座標にいる
 */
//==================================================================
BOOL ColosseumTool_CheckWayOut(FIELD_MAIN_WORK *fieldWork)
{
  VecFx32 pos;
  FIELD_PLAYER * player = FIELDMAP_GetFieldPlayer(fieldWork);
  const POINT_S16 *postbl;
  int i, tbl_max;
  
  FIELD_PLAYER_GetPos(player, &pos);
  pos.x >>= FX32_SHIFT;
  pos.z >>= FX32_SHIFT;

  if(FIELDMAP_GetZoneID(fieldWork) == ZONE_ID_CLOSSEUM){
    postbl = ColosseumWayOutPos_1vs1;
    tbl_max = NELEMS(ColosseumWayOutPos_1vs1);
  }
  else{
    postbl = ColosseumWayOutPos_multi;
    tbl_max = NELEMS(ColosseumWayOutPos_multi);
  }
  
  for(i = 0; i < tbl_max; i++){
    if(pos.x == postbl[i].x && pos.z == postbl[i].z){
      return TRUE;
    }
  }
  return FALSE;
}

//==================================================================
/**
 * 通信プレイヤーの座標更新
 *
 * @param   clsys		    
 */
//==================================================================
void ColosseumTool_CommPlayerUpdate(COLOSSEUM_SYSTEM_PTR clsys)
{
  int net_id, my_net_id;
  COMM_PLAYER_PACKAGE pospack;
  
  my_net_id = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
  for(net_id = 0; net_id < COLOSSEUM_MEMBER_MAX; net_id++){
    if(my_net_id == net_id){
      continue;
    }
    if(Colosseum_GetCommPlayerPos(clsys, net_id, &pospack) == TRUE){
      if(CommPlayer_CheckOcc(clsys->cps, net_id) == FALSE){
        CommPlayer_Add(clsys->cps, net_id, clsys->basic_status[net_id].sex, &pospack);
      }
      else{
        CommPlayer_SetParam(clsys->cps, net_id, &pospack);
      }
    }
  }
}

