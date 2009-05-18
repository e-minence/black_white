//==============================================================================
/**
 * @file  palace_sys.c
 * @brief フィールド通信：パレス
 * @author  matsuda
 * @date  2009.05.01(金)
 */
//==============================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "field_comm_main.h"
#include "field_comm_func.h"
#include "field_comm_data.h"
#include "field_comm_sys.h"
#include "field/field_comm/palace_sys.h"

#include "arc_def.h"
#include "palace.naix"
#include "fieldmap/zone_id.h"


//==============================================================================
//  定数定義
//==============================================================================
///パレスマップの範囲(この座標外に出た場合、ワープさせる必要がある)
enum{
  PALACE_MAP_RANGE_LEFT_X = 512,
  PALACE_MAP_RANGE_RIGHT_X = 1024,

  PALACE_MAP_RANGE_LEN = 512,     ///<パレスマップのX長
};

///パレスエリア番号初期値(まだ通信が接続されていなくて、自分が何番目か分からない状態)
#define PALACE_AREA_NO_NULL     (128)

//==============================================================================
//  構造体定義
//==============================================================================
///デバッグ：パレスエリア番号表示管理構造体
typedef struct{
  GFL_CLWK *clact;
  u32 cgr_id;
  u32 pltt_id;
  u32 cell_id;
  u8 anmseq;
  u8 padding[3];
}PALACE_DEBUG_NUMBER;

///パレスシステムワーク
typedef struct _PALACE_SYS_WORK{
  u8 area;          ///<自機が居るエリア番号
  u8 entry_count;   ///<参加したプレイヤー数(プレイヤーが抜けても減らない)
  u8 padding[2];
  
  GFL_CLUNIT *clunit;
  PALACE_DEBUG_NUMBER number;
}PALACE_SYS_WORK;


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static void PALACE_SYS_PosUpdate(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply);
static void PALACE_DEBUG_UpdateNumber(PALACE_SYS_PTR palace);


//==============================================================================
//
//  
//
//==============================================================================
//==================================================================
/**
 * パレスシステムワーク確保
 * @param   heap_id		ヒープID
 * @retval  PALACE_SYS_PTR		確保したパレスワークシステムへのポインタ
 */
//==================================================================
PALACE_SYS_PTR PALACE_SYS_Alloc(HEAPID heap_id)
{
  PALACE_SYS_PTR palace;
  
  palace = GFL_HEAP_AllocClearMemory(heap_id, sizeof(PALACE_SYS_WORK));
  PALACE_SYS_InitWork(palace);
  
  return palace;
}

//==================================================================
/**
 * パレスシステムワーク初期設定
 *
 * @param   palace		
 */
//==================================================================
void PALACE_SYS_InitWork(PALACE_SYS_PTR palace)
{
  palace->entry_count = 1;  //自分
  palace->area = PALACE_AREA_NO_NULL;
}

//==================================================================
/**
 * パレスシステムワーク解放
 *
 * @param   palace		パレスシステムワークへのポインタ
 */
//==================================================================
void PALACE_SYS_Free(PALACE_SYS_PTR palace)
{
  PALACE_DEBUG_DeleteNumberAct(palace);
  
  GFL_HEAP_FreeMemory(palace);
}

//==================================================================
/**
 * パレス監視更新処理
 *
 * @param   palace		
 */
//==================================================================
void PALACE_SYS_Update(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply)
{
  int net_num;

  if(palace == NULL){
    return;
  }
  
  //始めて通信確立した時のパレスの初期位置設定
  //※check　ちょっとやっつけっぽい。
  //正しくは通信確立時のコールバック内でパレスのエリア初期位置をセットしてあげるのがよい
  if(palace->clunit != NULL){
    if(palace->area == PALACE_AREA_NO_NULL && GFL_NET_GetConnectNum() > 1){
      palace->area = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
      GFL_CLACT_WK_SetDrawEnable(palace->number.clact, TRUE);
    }
  }
  
  //ユーザーの増加確認
  net_num = GFL_NET_GetConnectNum();
  if(net_num > palace->entry_count){
    OS_TPrintf("palace:ユーザーが増えた old=%d, new=%d\n", palace->entry_count, net_num);
    palace->entry_count = net_num;
    //※check　親の場合、ここで増えたentry_countを一斉送信
    //         現在はGFL_NET_GetConnectNumで親子関係無く増やしている
    //         entry_countの意味が「参加したプレイヤー数(プレイヤーが抜けても減らない)」の為、
    //         親が管理し、送信する必要がある
      ;
  }
  
  //自機の位置確認し、ワープ処理
  PALACE_SYS_PosUpdate(palace, plwork, fldply);

  PALACE_DEBUG_UpdateNumber(palace);
}

//--------------------------------------------------------------
/**
 * 自機の位置確認し、ワープ処理
 *
 * @param   palace		
 * @param   plwork		
 * @param   fldply		
 */
//--------------------------------------------------------------
static void PALACE_SYS_PosUpdate(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply)
{
  ZONEID zone_id;
  VecFx32 pos, new_pos;
  s32 offs_left, offs_right;
  int new_area, now_area;
  
  zone_id = PLAYERWORK_getZoneID(plwork);
  if(zone_id != ZONE_ID_PALACETEST){
    return;
  }
  
  FIELD_PLAYER_GetPos( fldply, &pos );
  new_pos = pos;
  now_area = (palace->area == PALACE_AREA_NO_NULL) ? 0 : palace->area;
  new_area = now_area;
  
  offs_left = FX_Whole(pos.x) - PALACE_MAP_RANGE_LEFT_X;
  if(offs_left <= 0){
    new_pos.x = (PALACE_MAP_RANGE_RIGHT_X + offs_left) << FX32_SHIFT;
    new_area--;
  }
  else{
    offs_right = FX_Whole(pos.x) - PALACE_MAP_RANGE_RIGHT_X;
    if(offs_right > 0){
      new_pos.x = (PALACE_MAP_RANGE_LEFT_X + offs_right) << FX32_SHIFT;
      new_area++;
    }
  }

  if(now_area != new_area){
    if(new_area < 0){
      new_area = palace->entry_count - 1;
    }
    else if(new_area >= palace->entry_count){
      new_area = 0;
    }
    OS_TPrintf("palace:ワープ old_x=%d, new_x=%d, old_area=%d, new_area=%d\n", 
      FX_Whole(pos.x), FX_Whole(new_pos.x), now_area, new_area);
  }
  FIELD_PLAYER_SetPos( fldply, &new_pos );
  if(palace->area != PALACE_AREA_NO_NULL){
    palace->area = new_area;
  }
}

//==================================================================
/**
 * パレスのエリア番号を設定
 *
 * @param   palace		    パレスシステムワークへのポインタ
 * @param   palace_area   パレスのエリア番号
 */
//==================================================================
void PALACE_SYS_SetArea(PALACE_SYS_PTR palace, u8 palace_area)
{
  if(palace == NULL){
    return;
  }
  palace->area = palace_area;
}

//==================================================================
/**
 * パレスのエリア番号を取得
 *
 * @param   palace		パレスシステムワークへのポインタ
 *
 * @retval  u8    		エリア番号
 */
//==================================================================
u8 PALACE_SYS_GetArea(PALACE_SYS_PTR palace)
{
  if(palace == NULL){
    return 0;
  }
  return palace->area;
}

//==================================================================
/**
 * 友達の自機座標をパレスのエリアIDを判定して修正を行う
 *
 * @param   palace		      パレスシステムへのポインタ
 * @param   friend_area	  	友達のパレスエリア
 * @param   my_plwork		    自分のPLAYER_WORK
 * @param   friend_plwork		友達のPLAYER_WORK
 */
//==================================================================
void PALACE_SYS_FriendPosConvert(PALACE_SYS_PTR palace, int friend_area, 
  PLAYER_WORK *my_plwork, PLAYER_WORK *friend_plwork)//, FIELD_PLAYER *friend_fldply)
{
  int left_area, right_area;
  VecFx32 pos;
  
  if(palace == NULL || palace->area == friend_area 
      || PLAYERWORK_getZoneID(my_plwork) != PLAYERWORK_getZoneID(friend_plwork)){
    return;
  }
  
  left_area = palace->area - 1;
  right_area = palace->area + 1;
  if(left_area < 0){
    left_area = palace->entry_count - 1;
  }
  if(right_area >= palace->entry_count){
    right_area = 0;
  }
  
//  FIELD_PLAYER_GetPos( friend_fldply, &pos );
  pos = *(PLAYERWORK_getPosition(friend_plwork));
  if(friend_area == right_area){  //右隣のエリアにいる場合
    pos.x += PALACE_MAP_RANGE_LEN << FX32_SHIFT;
  }
  else if(friend_area == left_area){ //左隣のエリアにいる場合
    pos.x -= PALACE_MAP_RANGE_LEN << FX32_SHIFT;
  }
  else{ //両隣のエリアではない場合
    pos.x += (PALACE_MAP_RANGE_LEN * 2) << FX32_SHIFT;
  }
//  FIELD_PLAYER_SetPos( friend_fldply, &pos );
  PLAYERWORK_setPosition(friend_plwork, &pos);
}

//--------------------------------------------------------------
/**
 * デバッグ：パレスエリア番号表示アクター生成
 *
 * @param   palace		
 * @param   heap_id		
 */
//--------------------------------------------------------------
void PALACE_DEBUG_CreateNumberAct(PALACE_SYS_PTR palace, HEAPID heap_id)
{
  PALACE_DEBUG_NUMBER *number = &palace->number;

  if(palace->clunit != NULL){
    return;
  }

  palace->clunit = GFL_CLACT_UNIT_Create( 1, 10, heap_id );
  
  {//リソース登録
    ARCHANDLE *handle;

    handle = GFL_ARC_OpenDataHandle(ARCID_PALACE, heap_id);
    
  	number->cgr_id = GFL_CLGRP_CGR_Register( 
  	  handle, NARC_palace_debug_areano_NCGR, FALSE, CLSYS_DRAW_MAIN, heap_id );
  	number->pltt_id = GFL_CLGRP_PLTT_RegisterEx( 
  	  handle, NARC_palace_debug_areano_NCLR, CLSYS_DRAW_MAIN, 15*32, 0, 1, heap_id );
  	number->cell_id = GFL_CLGRP_CELLANIM_Register( 
  	  handle, NARC_palace_debug_areano_NCER, NARC_palace_debug_areano_NANR, heap_id );
  	
  	GFL_ARC_CloseDataHandle(handle);
  }
  
  {//アクター生成
    static const GFL_CLWK_DATA clwkdata = {
      16, 16, 0, 1, 0,
    };
    
    number->clact = GFL_CLACT_WK_Create(
      palace->clunit, number->cgr_id, number->pltt_id, number->cell_id, 
      &clwkdata, CLSYS_DEFREND_MAIN, heap_id);
    GFL_CLACT_WK_SetAutoAnmFlag(number->clact, TRUE); //オートアニメON
    GFL_CLACT_WK_SetDrawEnable(number->clact, FALSE);
  }
}

//--------------------------------------------------------------
/**
 * デバッグ：パレスエリア番号表示アクター削除
 *
 * @param   palace		
 */
//--------------------------------------------------------------
void PALACE_DEBUG_DeleteNumberAct(PALACE_SYS_PTR palace)
{
  PALACE_DEBUG_NUMBER *number = &palace->number;
  
  if(palace->clunit == NULL){
    return;
  }
  
  GFL_CLACT_WK_Remove(number->clact);

  GFL_CLGRP_CGR_Release(number->cgr_id);
  GFL_CLGRP_PLTT_Release(number->pltt_id);
  GFL_CLGRP_CELLANIM_Release(number->cell_id);

  GFL_CLACT_UNIT_Delete(palace->clunit);
  palace->clunit = NULL;
}

//--------------------------------------------------------------
/**
 * デバッグ：パレスエリア番号アクター更新処理
 *
 * @param   palace		
 */
//--------------------------------------------------------------
static void PALACE_DEBUG_UpdateNumber(PALACE_SYS_PTR palace)
{
  PALACE_DEBUG_NUMBER *number = &palace->number;
  
  if(palace->clunit == NULL || palace->area == number->anmseq || palace->area == PALACE_AREA_NO_NULL){
    return;
  }
  
  number->anmseq = palace->area % 4;  //数字アニメは4パターンしか用意していない
  GFL_CLACT_WK_SetAnmSeq( number->clact, number->anmseq );
}

