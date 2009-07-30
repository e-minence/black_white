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
#include "system/main.h"
#include "message.naix"
#include "msg/msg_invasion.h"
#include "system/bmp_oam.h"


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
  u8 entry_count_max; ///今までの最大参加人数
  u8 init_name_draw;

  GFL_CLUNIT *clunit;
  PALACE_DEBUG_NUMBER number;
  BMPOAM_SYS_PTR bmpoam_sys;
  BMPOAM_ACT_PTR bmpact;
  GFL_BMP_DATA *bmp;
}PALACE_SYS_WORK;


//==============================================================================
//  プロトタイプ宣言
//==============================================================================
static void PALACE_SYS_PosUpdate(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply, COMM_FIELD_SYS_PTR comm_field);
static void PALACE_DEBUG_UpdateNumber(PALACE_SYS_PTR palace, FIELD_MAIN_WORK *fieldWork, COMM_FIELD_SYS_PTR comm_field);
static void _DEBUG_NameDraw(PALACE_SYS_PTR palace, FIELD_MAIN_WORK *fieldWork, int area_no);


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
  palace->entry_count_max = 1;
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
void PALACE_SYS_Update(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply, COMM_FIELD_SYS_PTR comm_field, FIELD_MAIN_WORK *fieldWork)
{
  int net_num;
  int first_create;
  
  if(palace == NULL){
    return;
  }
  
  //始めて通信確立した時のパレスの初期位置設定
  //※check　ちょっとやっつけっぽい。
  //正しくは通信確立時のコールバック内でパレスのエリア初期位置をセットしてあげるのがよい
  first_create = PALACE_DEBUG_CreateNumberAct(palace, GFL_HEAP_LOWID(HEAPID_FIELDMAP), fieldWork);
  if(palace->clunit != NULL){
    if(GFL_NET_GetConnectNum() > 1){
      if(palace->area == PALACE_AREA_NO_NULL){
        palace->area = GFL_NET_GetNetID(GFL_NET_HANDLE_GetCurrentHandle());
        FIELD_COMM_SYS_SetInvalidNetID(comm_field, palace->area);
      }
      GFL_CLACT_WK_SetDrawEnable(palace->number.clact, TRUE);
    }
  }
  
  //ユーザーの増加確認
  net_num = GFL_NET_GetConnectNum();
  if(net_num > palace->entry_count){
    OS_TPrintf("palace:ユーザーが増えた old=%d, new=%d\n", palace->entry_count, net_num);
    palace->entry_count = net_num;
    palace->entry_count_max = net_num;
    //※check　親の場合、ここで増えたentry_countを一斉送信
    //         現在はGFL_NET_GetConnectNumで親子関係無く増やしている
    //         entry_countの意味が「参加したプレイヤー数(プレイヤーが抜けても減らない)」の為、
    //         親が管理し、送信する必要がある
      ;
  }
  else if(FIELD_COMM_SYS_GetExitReq(comm_field) == FALSE 
      && net_num < palace->entry_count && palace->entry_count_max > 1 ){
    OS_TPrintf("palace:ユーザーが減ったので強制エラー発動\n");
    GFL_NET_FatalErrorFunc(1);
    palace->entry_count_max = net_num;
    palace->entry_count = net_num;
  }
  
  //自機の位置確認し、ワープ処理
  PALACE_SYS_PosUpdate(palace, plwork, fldply, comm_field);

  PALACE_DEBUG_UpdateNumber(palace, fieldWork, comm_field);
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
static void PALACE_SYS_PosUpdate(PALACE_SYS_PTR palace, PLAYER_WORK *plwork, FIELD_PLAYER *fldply, COMM_FIELD_SYS_PTR comm_field)
{
  ZONEID zone_id;
  VecFx32 pos, new_pos;
  s32 offs_left, offs_right;
  int new_area, now_area;
  
  zone_id = PLAYERWORK_getZoneID(plwork);
  if(zone_id != ZONE_ID_PALACETEST || FIELD_PLAYER_CheckLiveMMdl(fldply) == FALSE){
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
    FIELD_COMM_SYS_SetInvalidNetID(comm_field, palace->area);
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
  if(right_area == left_area && friend_area == right_area){
    //二人の場合は右も左も同じエリアの為、座標で判定
    if(pos.x > ((PALACE_MAP_RANGE_LEFT_X + PALACE_MAP_RANGE_LEN/2) << FX32_SHIFT)){
      pos.x -= PALACE_MAP_RANGE_LEN << FX32_SHIFT;
    }
    else{
      pos.x += PALACE_MAP_RANGE_LEN << FX32_SHIFT;
    }
  }
  else if(friend_area == right_area){  //右隣のエリアにいる場合
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
BOOL PALACE_DEBUG_CreateNumberAct(PALACE_SYS_PTR palace, HEAPID heap_id, FIELD_MAIN_WORK *fieldWork)
{
  PALACE_DEBUG_NUMBER *number = &palace->number;

  if(palace->clunit != NULL){
    return FALSE;
  }

  palace->clunit = GFL_CLACT_UNIT_Create( 8, 10, heap_id );
  palace->bmpoam_sys = BmpOam_Init(heap_id, palace->clunit);

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
  
  //名前描画用BMPOAM
  {
    BMPOAM_ACT_DATA bmphead = {
      NULL, 
      64,
      8,
      0,
      0,
      0,
      0,
      CLSYS_DEFREND_MAIN,
      CLSYS_DRAW_MAIN,
    };
    
    palace->bmp = GFL_BMP_Create(10, 2, GFL_BMP_16_COLOR, heap_id);
    bmphead.bmp = palace->bmp;
    bmphead.pltt_index = number->pltt_id;
    palace->bmpact = BmpOam_ActorAdd(palace->bmpoam_sys, &bmphead);
    palace->init_name_draw = TRUE;
  }

  return TRUE;
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
  
  BmpOam_ActorDel(palace->bmpact);
  BmpOam_Exit(palace->bmpoam_sys);
  palace->bmpoam_sys = NULL;
  GFL_BMP_Delete(palace->bmp);

  GFL_CLACT_WK_Remove(number->clact);

  GFL_CLGRP_CGR_Release(number->cgr_id);
  GFL_CLGRP_PLTT_Release(number->pltt_id);
  GFL_CLGRP_CELLANIM_Release(number->cell_id);

  GFL_CLACT_UNIT_Delete(palace->clunit);
  palace->clunit = NULL;
}

//==================================================================
/**
 * デバッグ：パレスエリア番号表示アクターの表示・非表示設定
 *
 * @param   palace		
 * @param   enable		
 */
//==================================================================
void PALACE_DEBUG_EnableNumberAct(PALACE_SYS_PTR palace, BOOL enable)
{
  if(palace->clunit == NULL){
    return;
  }
  GFL_CLACT_WK_SetDrawEnable(palace->number.clact, enable);
}

//--------------------------------------------------------------
/**
 * デバッグ：パレスエリア番号アクター更新処理
 *
 * @param   palace		
 */
//--------------------------------------------------------------
static void PALACE_DEBUG_UpdateNumber(PALACE_SYS_PTR palace, FIELD_MAIN_WORK *fieldWork, COMM_FIELD_SYS_PTR comm_field)
{
  PALACE_DEBUG_NUMBER *number = &palace->number;
  int before_anmseq;
  
  if(FIELD_COMM_SYS_GetRecvProfile(comm_field, palace->area) == FALSE){
    return;
  }
  if(palace->init_name_draw){
    _DEBUG_NameDraw(palace, fieldWork, number->anmseq);
    palace->init_name_draw = 0;
    return;
  }
  if(palace->clunit == NULL || palace->area == number->anmseq || palace->area == PALACE_AREA_NO_NULL){
    return;
  }
  
  number->anmseq = palace->area % 4;  //数字アニメは4パターンしか用意していない
  GFL_CLACT_WK_SetAnmSeq( number->clact, number->anmseq );

  _DEBUG_NameDraw(palace, fieldWork, number->anmseq);
}

//--------------------------------------------------------------
/**
 * 名前描画
 *
 * @param   palace		
 * @param   fieldWork		
 * @param   area_no		
 */
//--------------------------------------------------------------
static void _DEBUG_NameDraw(PALACE_SYS_PTR palace, FIELD_MAIN_WORK *fieldWork, int area_no)
{
  FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG( fieldWork );
  GFL_FONT *fontHandle = FLDMSGBG_GetFontHandle( msgBG );
  GAMESYS_WORK *gsys = FIELDMAP_GetGameSysWork( fieldWork );
  GAMEDATA *gdata = GAMESYSTEM_GetGameData(gsys);
  MYSTATUS *myst = GAMEDATA_GetMyStatusPlayer(gdata, area_no);
  STRBUF *namebuf;
  
  OS_TPrintf("name draw area_no = %d\n", area_no);
  namebuf = MyStatus_CreateNameString(myst, HEAPID_FIELDMAP);
  GFL_BMP_Clear(palace->bmp, 0);
  PRINTSYS_Print(palace->bmp, 0, 0, namebuf, fontHandle);
  BmpOam_ActorBmpTrans(palace->bmpact);
  GFL_STR_DeleteBuffer(namebuf);
}

//==============================================================================
//
//  橋の特定位置まで来て、誰とも通信していないなら注意メッセージを出して自機を戻す
//
//==============================================================================
typedef struct{
  FIELD_MAIN_WORK *fieldWork;
  MMDL *player_mmdl;
  GFL_MSGDATA *msgData;
  FLDMSGWIN *msgWin;
  BOOL left_right;
}DEBUG_PALACE_NGWIN;

static GMEVENT_RESULT DebugPalaceNGWinEvent( GMEVENT *event, int *seq, void *wk );

GMEVENT * EVENT_DebugPalaceNGWin( GAMESYS_WORK *gsys, FIELD_MAIN_WORK *fieldWork, FIELD_PLAYER *fld_player, BOOL left_right )
{
  DEBUG_PALACE_NGWIN *ngwin;
  GMEVENT * event;
  
  event = GMEVENT_Create(gsys, NULL, DebugPalaceNGWinEvent, sizeof(DEBUG_PALACE_NGWIN));
  
  ngwin = GMEVENT_GetEventWork(event);
  GFL_STD_MemClear( ngwin, sizeof(DEBUG_PALACE_NGWIN) );
  
  ngwin->fieldWork = fieldWork;
  ngwin->player_mmdl = FIELD_PLAYER_GetMMdl(fld_player);
  ngwin->left_right = left_right;
  
  return event;
}

static GMEVENT_RESULT DebugPalaceNGWinEvent( GMEVENT *event, int *seq, void *wk )
{
  DEBUG_PALACE_NGWIN *ngwin = wk;
  
  switch(*seq){
  case 0:
    {
      FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG(ngwin->fieldWork);
      ngwin->msgData = FLDMSGBG_CreateMSGDATA( msgBG, NARC_message_invasion_dat );
      ngwin->msgWin = FLDMSGWIN_AddTalkWin( msgBG, ngwin->msgData );
      FLDMSGWIN_Print( ngwin->msgWin, 0, 0, msg_invasion_test08_01 );
      GXS_SetMasterBrightness(-16);
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
    GXS_SetMasterBrightness(0);
    (*seq)++;
    break;
  case 4:
    if(MMDL_CheckPossibleAcmd(ngwin->player_mmdl) == TRUE){
      u16 code = (ngwin->left_right == 0) ? AC_WALK_R_16F : AC_WALK_L_16F;
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

