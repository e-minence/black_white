//////////////////////////////////////////////////////////////////////
/**
 *
 * @brief  スクリプトコマンド：ショップ関連
 * @file   scrcmd_shop.c
 * @author iwasawa, mori
 * @date   2009.10.20
 *
 */
//////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "system/vm_cmd.h"
#include "scrcmd.h"
#include "system/main.h"
#include "system/bmp_menuwork.h"
#include "system/bmp_menulist.h"
#include "system/bmp_winframe.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"
#include "item/item.h"    // ITEM_GetIndex
#include "item_icon.naix"
#include "message.naix"
#include "message.naix"
#include "script_message.naix"
#include "gamesystem/g_power.h"  //  GPOWER_Calc_Sale

#include "gamesystem/game_data.h"
#include "gamesystem/msgspeed.h"
#include "savedata/bsubway_savedata.h"
#include "savedata/save_tbl.h"
#include "savedata/record.h"
#include "item/itemsym.h"
#include "script_local.h"
//#include "script.h"     // SCRIPT_SetSubProcWorkPointerAdrs
#include "app/p_status.h"
#include "event_fieldmap_control.h"
#include "scrcmd_shop.h"
#include "fieldmap.h"
#include "script_def.h"
#include "field_camera.h"
#include "field/field_msgbg.h"
#include "msg/script/msg_shop.h"

#include "shop_data.cdat" // shop_data_table,    shop_itemnum_table
#include "bpshop.cdat"    // bp_shop_data_table, bp_shop_itemnum_table

#include "arc/shop_gra.naix"

#ifdef PM_DEBUG
#define SHOP_DEBUG_COMMAND  // LRでお金が手に入る。SELECTでバッジ８つのショップになる
#endif

//====================================================================
// ■定数宣言
//====================================================================

#define SHOP_BUY_OBJ_RES_NUM  ( 3 )
#define SHOP_YEN_KETA_MAX     ( 7 ) 

// BMPWIN
enum{
  SHOP_BUY_BMPWIN_OKODUKAI=0,
  SHOP_BUY_BMPWIN_MONEY,
  SHOP_BUY_BMPWIN_LIST,
  SHOP_BUY_BMPWIN_NUM,
  SHOP_BUY_BMPWIN_PRICE,
  SHOP_BUY_BMPWIN_ITEMINFO,
  SHOP_BUY_BMPWIN_TALKMSG,
  SHOP_BUY_BMPWIN_MAX,
};

// OBJ用
enum{
  SHOP_OBJ_CURSOR=0,
  SHOP_OBJ_ARROW_UP,
  SHOP_OBJ_ARROW_DOWN,
  SHOP_OBJ_ARROW_UPDOWN,
  SHOP_OBJ_ITEM,
  SHOP_OBJ_MAX,
};

// メニューウインドウ枠のパレットを転送する位置
#define MENU_WINDOW_PAL_OFFSET    (   9 )
#define MENU_WINDOW_CHARA_OFFSET  ( 228 )

// 所持金用バッファの長さ
#define SHOP_MYGOLD_STR_MAX   ( 10 )

// 道具説明バッファの長さ
#define SHOP_ITEMNUM_STR_MAX   ( 20 )

// 道具説明バッファの長さ
#define SHOP_STRING_MAX   ( 200 )

// スクリーンクリア関数で使用する定義
#define SCREEN_SUBMENU_AND_INFOWIN  ( 0 )
#define SCREEN_SUBMENU_ONLY         ( 1 )


#define SHOP_TYPE_NORMAL  ( 0 )   // どうぐを扱う
#define SHOP_TYPE_WAZA    ( 1 )   // 技マシンを扱う
#define SHOP_PAYMENT_MONEY    ( 0 )   // お金で払う
#define SHOP_PAYMENT_BP       ( 1 )   // BPで払う


//====================================================================
// ■構造体宣言１
//====================================================================

// イベント用構造体
typedef struct _SHOP_EVENT_WORK{
  void* bag_param;
  u16*  ret_work;
}SHOP_EVENT_WORK;

// ショップ買うアプリ制御用構造体
typedef struct{
  u8      seq;     // 買うアプリシーケンス制御用
  u8      next;    // 次のシーケンス予約
  u16     wait;    // ウェイト
  HEAPID  heapId;  // ヒープID
  GFL_FONT *font;  // 描画用のフォント
  
  MISC      *misc;    // 所持金にアクセスするため
  MYITEM    *myitem;  // 手持ちの道具アクセス
  RECORD    *record;  // レコードデータ
  BSUBWAY_SCOREDATA *BpData; // バトルポイントにアクセスするため
  FIELD_WFBC_CORE   *wfbc;    // WFBCワークへのポインタ
  
  SHOP_ITEM lineup[SHOP_ITEM_MAX];  // ショップで購入できるデータを格納するワーク
  u16       lineup_num;             // 購入できる商品の数
  u8        type;                   // ショップのタイプ（通常ショップorBPショップor技ショップ)
  u8        payment;                // 支払い方法 （お金 or BP）
  
  GFL_BMPWIN        *win[SHOP_BUY_BMPWIN_MAX];  // BMPWIN
  BMP_MENULIST_DATA *list;            // 買い物リストデータ
  BMPMENU_WORK      *yesnoWork;       // はい・いいえ画面ワーク
  GFL_MSGDATA       *shopMsgData;     // ショップ画面MSGDATA
  GFL_MSGDATA       *itemInfoMsgData; // どうぐ名MSGDATA
  WORDSET           *wordSet;         // 文字列展開ワーク
  STRBUF            *priceStr;        // 値段文字列
  STRBUF            *expandBuf;       // expand展開用
  STRBUF            *wazaStr;         // 技表示用文字列バッファ
  STRBUF            *HaveStr;         // 「ーーー」
  BMPMENULIST_WORK  *menuList;        // BMPMENULIST
  PRINT_UTIL         printUtil;       // 
  PRINT_QUE         *printQue;        // 
  GFL_TCBLSYS       *pMsgTcblSys;     // メッセージ表示用タスクSYS
  PRINT_STREAM      *printStream;     // 
  
  GFL_CLUNIT        *ClactUnit;                // セルアクターユニット
  GFL_CLWK*         ClactWork[SHOP_OBJ_MAX];   // OBJワークポインタ
  u32               clact_res[3][SHOP_BUY_OBJ_RES_NUM]; // OBJリソースハンドル

  ARCHANDLE         *itemiconHandle;    // アイテムアイコンハンドル

  u16               selectitem;       // 選択したどうぐ
  u32               price;            // 選択したどうぐの価格
  u32               buy_max;          // 購入できる最大数
  s16               item_multi;       // 購入する数
  u16               FirstTrans;       // リストBMPWINの転送をおこなっているか？


} SHOP_BUY_APP_WORK;

// ショップ買う起動時構造体
typedef struct {
  u8  seq;       // シーケンス制御ワーク
  u8  wait;      // ウェイト用
  u8  shopType;  // ショップタイプ(SCR_SHOPID_NULL, SHOP_TYPE_FIX, etc..)
  u8  shopId;    // ショップの場所ID（shop_data.cdat参照。SHOP_TYOE_FIXの際に使用する)
  int camera_flag_push; // カメラの範囲制限フラグをPUSHするためのワーク
  SHOP_BUY_APP_WORK wk; // ショップ画面ワーク
  
} SHOP_BUY_CALL_WORK;

// BMP登録用情報構造体
typedef struct {
  int frame;
  u8  x,y,w,h;
  u16 pal,chara;
}BMPWIN_DAT;


//====================================================================
// ■プロトタイプ宣言
//====================================================================

static BOOL EvShopBuyWait( VMHANDLE *core, void *wk );
static BOOL ShopCallFunc( GAMESYS_WORK *gsys, SHOP_BUY_APP_WORK *wk, int type, int id );
static int  init_work( SHOP_BUY_APP_WORK *wk, int type );
static void exit_work( SHOP_BUY_APP_WORK *wk );
static void shop_item_set( SHOP_BUY_APP_WORK *wk, int type, int id, int badge );
static void shop_item_release( SHOP_BUY_APP_WORK *wk );
static void bg_trans( SHOP_BUY_APP_WORK *wk );
static void bg_clear( SHOP_BUY_APP_WORK *wk );
static void obj_init( SHOP_BUY_APP_WORK *wk );
static void obj_del( SHOP_BUY_APP_WORK *wk );
static void itemicon_resource_change( SHOP_BUY_APP_WORK *wk, u16 itemno );
static void bmpwin_init( SHOP_BUY_APP_WORK *wk );
static void bmpwin_exit( SHOP_BUY_APP_WORK *wk );
static void print_mygold( SHOP_BUY_APP_WORK *wk );
static void print_carry_item( SHOP_BUY_APP_WORK *wk, u16 itemno );
static void print_iteminfo( SHOP_BUY_APP_WORK *wk, u16 itemno );
static void bmpwin_list_init( SHOP_BUY_APP_WORK *wk );
static void bmpwin_list_exit( SHOP_BUY_APP_WORK *wk );
static void line_callback(BMPMENULIST_WORK * wk, u32 param, u8 y );
static void line_callback_waza(BMPMENULIST_WORK * wk, u32 param, u8 y );
static void move_callback( BMPMENULIST_WORK * wk, u32 param, u8 mode);
static void ShopPrintMsg( SHOP_BUY_APP_WORK *wk, int strId, u16 itemno );
static void ShopDecideMsg( SHOP_BUY_APP_WORK *wk, int strId, u16 itemno, u32 price, u16 num );
static void ShopDecideWazaMsg( SHOP_BUY_APP_WORK *wk, int strId, u16 itemno, u32 price );
static void print_multiitem_price( SHOP_BUY_APP_WORK *wk, u16 number, int one_price );
static  int price_key_control( SHOP_BUY_APP_WORK *wk );
static void submenu_screen_clear( int type );
static  int shop_buy_printwait( SHOP_BUY_APP_WORK *wk );
static void bp_item_set( SHOP_BUY_APP_WORK *wk, int type );
static void ShopTypeDecideMsg( SHOP_BUY_APP_WORK *wk, int type );
static void string_alloc( SHOP_BUY_APP_WORK *wk, int payment );  
static void blackcity_shop_item_set( SHOP_BUY_APP_WORK *wk, int type );


// BMPWIN定義テーブル
#include "wbshopbmp.h"    // Window0BmpDataTable

//--------------------------------------------------------------------
/**
 * @brief ショップ購入イベント呼び出し 
 *
 * @param core 仮想マシン制御構造体へのポインタ
 * @param wk   SCRCMD_WORKへのポインタ
 *
 * @retval VMCMD_RESULT
 */
//--------------------------------------------------------------------
VMCMD_RESULT EvCmdCallShopProcBuy( VMHANDLE* core, void* wk )
{
  SCRCMD_WORK*   work     = (SCRCMD_WORK*)wk;
  u16            shop_id  = SCRCMD_GetVMWorkValue( core, work );
  u16*           ret_work = SCRCMD_GetVMWork( core, work );
  GAMESYS_WORK*  gsys     = SCRCMD_WORK_GetGameSysWork( work );
  GAMEDATA*      gamedata = GAMESYSTEM_GetGameData( gsys );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  SCRIPT_WORK*   sc       = SCRCMD_WORK_GetScriptWork( work );
  void** scr_subproc_work = SCRIPT_SetSubProcWorkPointerAdrs( sc );
  FLDMSGBG *     fldmsg   = FIELDMAP_GetFldMsgBG( fieldmap );
  FIELD_WFBC_CORE *wfbc   = GAMEDATA_GetMyWFBCCoreData( gamedata );

  // ショップ用ワーク確保
  SHOP_BUY_CALL_WORK* sbw = GFL_HEAP_AllocClearMemory(HEAPID_PROC,sizeof(SHOP_BUY_CALL_WORK));

//  shop_id = SCR_SHOPID_BP_WAZA;

  *scr_subproc_work = sbw;
  sbw->wk.font      = FLDMSGBG_GetFontHandle( fldmsg );
  sbw->wk.misc      = GAMEDATA_GetMiscWork( gamedata );
  sbw->wk.heapId    = HEAPID_FIELDMAP;
  sbw->wk.myitem    = GAMEDATA_GetMyItem( gamedata );
  sbw->wk.BpData    = SaveControl_DataPtrGet(GAMEDATA_GetSaveControlWork(gamedata),
                                                     GMDATA_ID_BSUBWAY_SCOREDATA);
  sbw->wk.record    = GAMEDATA_GetRecordPtr( gamedata );
  sbw->wk.wfbc      = wfbc;

  // スクリプトからのパラメータで分岐
  switch(shop_id){
  case SCR_SHOPID_NULL:       //変動ショップ呼び出し 
    sbw->shopType = shop_id;
    break;
  case SCR_SHOPID_BP_ITEM:    // BPアイテムショップ
    sbw->shopType = shop_id;
    break;
  case SCR_SHOPID_BP_WAZA:    // BP技マシンショップ
    sbw->shopType = shop_id;
    break;
                              // 通信データから生成されるブラックシティショップ
  case SCR_SHOPID_BLACK_CITY0:  case SCR_SHOPID_BLACK_CITY1:  case SCR_SHOPID_BLACK_CITY2:  
  case SCR_SHOPID_BLACK_CITY3:  case SCR_SHOPID_BLACK_CITY4:  
    sbw->shopType = shop_id;
    break;
  default:                    //固定ショップ呼び出し
    sbw->shopType = shop_id;
    sbw->shopId   = shop_id;
  } 
  // ショップ呼び出し
  VMCMD_SetWait( core, EvShopBuyWait );

  // BPショップ以外の場合は買い物ビーコン送信
  if(shop_id!=SCR_SHOPID_BP_ITEM && shop_id!=SCR_SHOPID_BP_WAZA){
    GAMEBEACON_Set_Shoping();
  }
  /*
    イベント終了時に ret_work に終了モードを返してください
    SCR_PROC_RETMODE_EXIT 一発終了
    SCR_PROC_RETMODE_NORMAL 継続
  */
  return VMCMD_RESULT_SUSPEND;
}


enum{
  SHOP_BUY_CALL_CAMERATRACE_STOP=0,
  SHOP_BUY_CALL_CAMERATRACE_STOP_WAIT,
  SHOP_BUY_CALL_CAMERA_MOVE,
  SHOP_BUY_CALL_CAMERA_MOVE_WAIT,
  SHOP_BUY_CALL,
  SHOP_BUY_CALL_CAMERA_RETURN,
  SHOP_BUY_CALL_CAMERA_RETURN_WAIT,
  SHOP_BUY_CALL_END,
};

// ショップで買い物を開始するときのカメラ移動のオフセットデータ
static const VecFx32 shopcamera_move_tbl[4]={
  {  FX32_ONE*16*4,  -FX32_ONE*8*0,   -FX32_ONE*16*1  },   // 上
  {  FX32_ONE*16*4,  -FX32_ONE*8*4,   0  },   // 下
  {  FX32_ONE*16*4,  -FX32_ONE*8*0,   0  },   // 左
  {  FX32_ONE*16*5,  -FX32_ONE*8*2,   0  },   // 右
};

//----------------------------------------------------------------------------------
/**
 * @brief カメラ移動処理開始設定
 *
 * @param   sbw         SHOP_BUY_CALL_WORK
 * @param   fieldmap    フィールドマップワーク
 * @param   fieldcamera カメラ設定
 */
//----------------------------------------------------------------------------------
static void Start_CameraMove( SHOP_BUY_CALL_WORK *sbw, FIELDMAP_WORK* fieldmap, FIELD_CAMERA *fieldcamera )
{
  FLD_CAM_MV_PARAM param;
  FIELD_PLAYER *fld_player;
  int dir;

  // カメラ移動制限処理を解除（ただし解除前の設定を保存しておく）
  sbw->camera_flag_push = FIELD_CAMERA_GetCameraAreaActive( fieldcamera );
  FIELD_CAMERA_SetCameraAreaActive( fieldcamera, FALSE );
  FIELD_CAMERA_SetRecvCamParamOv( fieldcamera );

  // 主人公の向きを取得
  fld_player = FIELDMAP_GetFieldPlayer( fieldmap );
  dir        = FIELD_PLAYER_GetDir( fld_player );  
  
  // カメラ現在位置を取得
  FIELD_CAMERA_GetCameraPos( fieldcamera, &param.Core.CamPos);
  FIELD_CAMERA_GetTargetPos( fieldcamera, &param.Core.TrgtPos );
  FIELD_CAMERA_FreeTarget(fieldcamera);

  // 向きに合わせて移動オフセット値の足しこみ
  param.Core.TrgtPos.x += shopcamera_move_tbl[dir].x;
  param.Core.TrgtPos.y += shopcamera_move_tbl[dir].y;
  param.Core.TrgtPos.z += shopcamera_move_tbl[dir].z;

  param.Chk.Shift = FALSE;
  param.Chk.Pitch = FALSE;
  param.Chk.Yaw   = FALSE;
  param.Chk.Dist  = FALSE;
  param.Chk.Fovy  = FALSE;
  param.Chk.Pos   = TRUE;

  // カメラ移動開始
  FIELD_CAMERA_SetLinerParamOv( fieldcamera, &param, 10 );

}
//----------------------------------------------------------------------------------
/**
 * @brief ショップ買うサブシーケンス
 *
 * @param   core  SCRCMD_WORK
 * @param   wk    SHOP_BUY_CALL_WORK
 *
 * @retval  BOOL  終了:TRUE   待ち:FALSE
 */
//----------------------------------------------------------------------------------
static BOOL EvShopBuyWait( VMHANDLE *core, void *wk )
{
  SCRCMD_WORK *work = wk;
  SCRIPT_WORK *sc = SCRCMD_WORK_GetScriptWork( work );
  void **scr_subproc_work = SCRIPT_SetSubProcWorkPointerAdrs( sc );
  SHOP_BUY_CALL_WORK *sbw = *scr_subproc_work;
  GAMESYS_WORK*     gsys = SCRCMD_WORK_GetGameSysWork( work );
  FIELDMAP_WORK* fieldmap = GAMESYSTEM_GetFieldMapWork( gsys );
  FIELD_CAMERA *fieldcamera = FIELDMAP_GetFieldCamera( fieldmap );

  // ショップ呼び出しイベントシーケンス
  switch(sbw->seq)
  {
    case SHOP_BUY_CALL_CAMERATRACE_STOP:  // カメラ移動停止（念のため）
      FIELD_CAMERA_StopTraceRequest( fieldcamera );
      sbw->seq++;
      break;
    case SHOP_BUY_CALL_CAMERATRACE_STOP_WAIT:
      if(FIELD_CAMERA_CheckTrace(fieldcamera)==FALSE){
        sbw->seq++;
      }
      break;
    case SHOP_BUY_CALL_CAMERA_MOVE:
      OS_Printf("ショップ起動\n");
      // カメラ移動開始
      Start_CameraMove( sbw, fieldmap, fieldcamera );   
      sbw->seq++;
      break;
    case SHOP_BUY_CALL_CAMERA_MOVE_WAIT:
      // カメラ移動終了待ち
      if(!FIELD_CAMERA_CheckMvFunc(fieldcamera)){
        sbw->seq++;
        sbw->wk.seq = 0;
        FLDMSGBG_SetBlendAlpha( FALSE );  // メッセージウインドウ半透明OFF
      }
      break;
    case SHOP_BUY_CALL:
      // ショップメイン処理
      if(ShopCallFunc( gsys, &sbw->wk, sbw->shopType, sbw->shopId  )){
        sbw->seq++;
      }
      break;
    case SHOP_BUY_CALL_CAMERA_RETURN:
      // 移動カメラ戻す処理
      {
        FLD_CAM_MV_PARAM_CHK chk = {FALSE, FALSE,FALSE,FALSE,FALSE,TRUE,};
        FIELD_CAMERA_RecvLinerParam(fieldcamera, &chk, 8);
      }
      sbw->seq++;
      sbw->wait=0;
      break;
    case SHOP_BUY_CALL_CAMERA_RETURN_WAIT:
      // カメラ移動終了待ち
      if(!FIELD_CAMERA_CheckMvFunc(fieldcamera)){
        FIELD_CAMERA_BindDefaultTarget(fieldcamera);
        FIELD_CAMERA_SetCameraAreaActive( fieldcamera, sbw->camera_flag_push );
        FIELD_CAMERA_RestartTrace( fieldcamera );
        FIELD_CAMERA_ClearRecvCamParam(fieldcamera);
        FLDMSGBG_SetBlendAlpha( TRUE );  // メッセージウインドウ半透明ON
        sbw->seq++;
      }
      break;
    // イベント終了
    case SHOP_BUY_CALL_END:
      GFL_HEAP_FreeMemory(*scr_subproc_work);
      return TRUE;
      break;
  }
  
  return FALSE;
}

// ここまでが呼び出し部。

//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------
//----------------------------------------------------------------------------------

// はい・いいえ画面描画処理
static const BMPWIN_YESNO_DAT yesno_data= {
  GFL_BG_FRAME1_M,
  24, 13, 
  11, 1+24
};

// ショップ買う画面遷移用定義
enum{
  SHOPBUY_SEQ_INIT=0,
  SHOPBUY_SEQ_MAIN,
  SHOPBUY_SEQ_SELECT,
  SHOPBUY_SEQ_DECIDE_NUM,
  SHOPBUY_SEQ_BACK,
  SHOPBUY_SEQ_ONLY1_YESNO,
  SHOPBUY_SEQ_YESNO,
  SHOPBUY_SEQ_SELECT_YESNO_WAIT,
  SHOPBUY_SEQ_BUY,
  SHOPBUY_SEQ_JUDGE_PREMIER_BALL,
  SHOPBUY_SEQ_MSG_WAIT,
  SHOPBUY_SEQ_END,
};

// テキストカラー定義
#define WHITE_TEXT_W_COL  (PRINTSYS_LSB_Make( 15,  2, 15))
#define BLACK_TEXT_W_COL  (PRINTSYS_LSB_Make(  1,  2, 15))
#define WHITE_TEXT_B_COL  (PRINTSYS_LSB_Make( 15,  2,  0))
#define BLUE_TEXT_COL     (PRINTSYS_LSB_Make( 12, 13,  0))


//----------------------------------------------------------------------------------
/**
 * @brief ショップ画面初期化
 *
 * @param   gsys  GAMESYS_WORK(セーブデータ引継ぎのため）
 * @param   wk    SHOP_BUY_APP_WORK
 * @param   type  ショップタイプ（変動型： 固定リスト）
 * @param   id    固定リストの際の参照データ番号
 */
//----------------------------------------------------------------------------------
static void shop_call_init( GAMESYS_WORK *gsys, SHOP_BUY_APP_WORK *wk, int type, int id )
{
  GAMEDATA *gamedata = GAMESYSTEM_GetGameData(gsys);
  MYSTATUS *my       = GAMEDATA_GetMyStatus(gamedata);


  // ワーク初期化
  wk->type = SHOP_TYPE_NORMAL;
  type     = init_work( wk, type );

  switch(type){
  case SCR_SHOPID_NULL:
  // 品揃え読み込み
#ifdef SHOP_DEBUG_COMMAND
  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_SELECT){
    shop_item_set( wk, type, id, 8);
  }else{
    shop_item_set( wk, type, id, MISC_GetBadgeCount(GAMEDATA_GetMiscWork(gamedata)));
  }
#else
    shop_item_set( wk, type, id, MISC_GetBadgeCount(GAMEDATA_GetMiscWork(gamedata)));
#endif
  break;
  case SCR_SHOPID_BP_ITEM:
    bp_item_set( wk, SHOP_TYPE_NORMAL  );
    wk->payment = SHOP_PAYMENT_BP;
    break;
  case SCR_SHOPID_BP_WAZA:
    wk->payment = SHOP_PAYMENT_BP;
    wk->type = SHOP_TYPE_WAZA;
    bp_item_set( wk, SHOP_TYPE_WAZA  );
    break;
  case SCR_SHOPID_BLACK_CITY0:  case SCR_SHOPID_BLACK_CITY1:  case SCR_SHOPID_BLACK_CITY2:
  case SCR_SHOPID_BLACK_CITY3:  case SCR_SHOPID_BLACK_CITY4:
    blackcity_shop_item_set( wk, type );
    break;
  default: 
    shop_item_set( wk, type, id, 0 );
    break;
  }

  string_alloc( wk, wk->payment );
  bg_trans( wk );     // BG転送
  bmpwin_init( wk );  // BMPWIN初期化
  obj_init( wk );     // OAM初期化
  print_mygold(wk);   // 所持金表示

  bmpwin_list_init( wk ); // 商品リスト表示
 
}

//----------------------------------------------------------------------------------
/**
 * @brief どうぐ所持の最大数を返す
 *
 * @param   item  どうぐNO
 *
 * @retval  int   アイテムナンバー
 */
//----------------------------------------------------------------------------------
static int get_item_max( int item )
{
  // 技マシンは99個までしか持てない
//  if(item>=ITEM_WAZAMASIN01 && item < ITEM_HIDENMASIN01){
  if( ITEM_CheckWazaMachine( item ) == TRUE ){
    return 99;
  }
  
  return 999;
}

//----------------------------------------------------------------------------------
/**
 * @brief 値段表示用文字列確保
 *
 * @param   wk    
 * @param   type    SHOP_PAYMENT_MONEYかSHOP_PAYMENT_BP
 */
//----------------------------------------------------------------------------------
static void string_alloc( SHOP_BUY_APP_WORK *wk, int payment )
{
  // ○○○円　か　○○ＢＰ
  if(payment==SHOP_PAYMENT_BP){
    wk->priceStr = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_02_07_01 );
  }else{
    wk->priceStr = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_02_07 );
  }
  // 「ーーー」（購入済のマーク）
  wk->HaveStr    = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_02_07_02 );
}

//----------------------------------------------------------------------------------
/**
 * @brief 現在の持っている通貨（お金・BP)を取得する
 *
 * @param   wk    
 *
 * @retval  int   どっちかの通貨
 */
//----------------------------------------------------------------------------------
static int _get_gold( SHOP_BUY_APP_WORK *wk )
{
  // お金か？
  if(wk->payment==SHOP_PAYMENT_MONEY){
    return MISC_GetGold(wk->misc);
  }
  // BPか
  return BSUBWAY_SCOREDATA_GetBattlePoint( wk->BpData );
}

//----------------------------------------------------------------------------------
/**
 * @brief 道具選択時に何個買えるかで処理を分岐させる
 *        (持ちきれない・買えません・複数個選択へ・１つしか買えないのですぐYESNOへ)
 *
 * @param   wk    SHOP_BUY_APP_WORK
 *
 * @retval  none
 */
//----------------------------------------------------------------------------------
static void can_player_buy_item( SHOP_BUY_APP_WORK *wk )
{
  int gold = _get_gold(wk);
  int num  = MYITEM_GetItemNum( wk->myitem, wk->selectitem, wk->heapId );

  // もちきれないですよ！
  if(MYITEM_AddCheck( wk->myitem, wk->selectitem, 1, wk->heapId )==FALSE)
  {
    ShopPrintMsg( wk, mes_shop_02_05, wk->selectitem );
    wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
    wk->next = SHOPBUY_SEQ_BACK;

  // 「買えませんよ！」
  }else if(gold<wk->price)
  {
    if(wk->payment==SHOP_PAYMENT_MONEY){
      // お金が足りない
      ShopPrintMsg( wk, mes_shop_02_01, wk->selectitem );
    }else{
      // BPが足りない
      ShopPrintMsg( wk, mes_shop_02_09, wk->selectitem );
    }
    wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
    wk->next = SHOPBUY_SEQ_BACK;

  // 一つだけ買えるのでそのまま「はい・いいえ」
  }else if(gold>=wk->price && gold<(wk->price*2))
  {
    ShopTypeDecideMsg( wk, wk->type );
    wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
    wk->next = SHOPBUY_SEQ_ONLY1_YESNO;

  // 複数買えるので個数選択へ
  }else{
    if(wk->type==SHOP_TYPE_NORMAL){
      ShopPrintMsg( wk, mes_shop_02_02, wk->selectitem );
      wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
      wk->next = SHOPBUY_SEQ_SELECT;
      wk->buy_max = gold/wk->price;
      if(wk->buy_max>99){   // 買える最大数は９９個
        wk->buy_max = 99;
      }
      // 持てる最大数を算出
      if(wk->buy_max+num > MYITEM_GetItemMax( wk->selectitem ) ){
        wk->buy_max = MYITEM_GetItemMax( wk->selectitem ) - num;
      }
    }else{
      ShopTypeDecideMsg( wk, wk->type );
      wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
      wk->next = SHOPBUY_SEQ_YESNO;
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief 買う画面どうぐ選択時メイン処理
 *
 * @param   wk    SHOP_BUY_APP_WORK
 *
 * @retval  none
 */
//----------------------------------------------------------------------------------
static void shop_main( SHOP_BUY_APP_WORK *wk )
{
  u32 ret;
  // printQueに処理が残っている場合はメインを通さない
  if(PRINTSYS_QUE_IsFinished( wk->printQue )==FALSE){
    return;
  }

  // BMPの転送待ちの場合があるので、BmpwinOnのタイミングをズラした
  if(wk->FirstTrans==0){
    GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_LIST] );
    wk->FirstTrans++;
  }

  // リスト処理メイン
  ret = BmpMenuList_Main( wk->menuList );
  if(ret!=BMPMENULIST_NULL)
  {
    if(ret==BMPMENULIST_CANCEL)
    {
      wk->seq = SHOPBUY_SEQ_END;
    } else {
      wk->selectitem = wk->lineup[ret].id;
      wk->price      = wk->lineup[ret].price; //ITEM_GetParam( ret, ITEM_PRM_PRICE, wk->heapId );
      wk->item_multi = 1;
      // 購入できるか判定（買えない・一個買える・複数買える）
      can_player_buy_item( wk );
    }
  }

}


//----------------------------------------------------------------------------------
/**
 * @brief 文字列表示待ち処理
 *
 * @param   printStream   
 *
 * @retval  int   次のシーケンスを指定
 */
//----------------------------------------------------------------------------------
static int shop_buy_printwait( SHOP_BUY_APP_WORK *wk )
{
  PRINTSTREAM_STATE state = PRINTSYS_PrintStreamGetState( wk->printStream );

  if( state == PRINTSTREAM_STATE_DONE )
  {
    PRINTSYS_PrintStreamDelete( wk->printStream );
    return wk->next;
  }else if( state == PRINTSTREAM_STATE_RUNNING)
  {
    if(GFL_UI_KEY_GetCont()&PAD_BUTTON_DECIDE || GFL_UI_KEY_GetCont()&PAD_BUTTON_CANCEL)
    {
      PRINTSYS_PrintStreamShortWait( wk->printStream, 0 );
    }
  }else if( state == PRINTSTREAM_STATE_PAUSE )
  {
    if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE || GFL_UI_KEY_GetTrg()&PAD_BUTTON_CANCEL){
      PRINTSYS_PrintStreamReleasePause( wk->printStream );
      PMSND_PlaySE( SEQ_SE_MESSAGE );
    }
  }
  
  return wk->seq;
}

//----------------------------------------------------------------------------------
/**
 * @brief 支払方法で引く対象を替える
 *
 * @param   wk    
 * @param   pay   支払う金額（お金かBP,,個数を掛け算済）
 */
//----------------------------------------------------------------------------------
static void _sub_payment( SHOP_BUY_APP_WORK *wk, int pay )
{
  // 支払い方法がお金か？
  if(wk->payment==SHOP_PAYMENT_MONEY){
    // お金
    MISC_SubGold( wk->misc , pay);
    // レコード：買い物した合計金額
    RECORD_Add( wk->record, RECID_SHOPPING_MONEY, pay);
  }else{
    // BP
    BSUBWAY_SCOREDATA_SubBattlePoint( wk->BpData, pay );
    // レコード：使った合計BP
    RECORD_Add( wk->record, RECID_USE_BP, pay);
  }

  // レコード：買った回数
  RECORD_Inc( wk->record, RECID_SHOPPING_CNT );
}
//----------------------------------------------------------------------------------
/**
 * @brief ショップ買うメインルーチン
 *
 * @param   gsys  GAMESYS_WORK
 * @param   wk    SHOP_BUY_APP_WORK
 * @param   type  ショップタイプ(SCR_SHOPID_NULL or SHOP_TYPE_FIX)
 * @param   id    固定ショップの際のID（shop_data.cdat参照)
 *
 * @retval  BOOL  継続:FALSE  終了:TRUE
 */
//----------------------------------------------------------------------------------
static BOOL ShopCallFunc( GAMESYS_WORK *gsys, SHOP_BUY_APP_WORK *wk, int type, int id )
{
  u32 ret;
  
  switch( wk->seq ){
  case SHOPBUY_SEQ_INIT:  // 初期化
    shop_call_init(gsys, wk, type, id);
    
    wk->seq++;
    break;
  case SHOPBUY_SEQ_MAIN:
    shop_main(wk);
    break;
  case SHOPBUY_SEQ_SELECT:
    // 複数買えるので個数選択へ
    print_carry_item( wk, wk->selectitem );
    print_multiitem_price( wk, wk->item_multi, wk->price );
    GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN], TRUE );
    wk->seq = SHOPBUY_SEQ_DECIDE_NUM;

    break;
  case SHOPBUY_SEQ_DECIDE_NUM:
    wk->seq = price_key_control( wk );
    break;
  case SHOPBUY_SEQ_BACK:    // 戻る
    submenu_screen_clear( SCREEN_SUBMENU_AND_INFOWIN );
    GFL_BMPWIN_MakeScreen( wk->win[SHOP_BUY_BMPWIN_LIST] );
    GFL_BMPWIN_MakeScreen( wk->win[SHOP_BUY_BMPWIN_ITEMINFO] );
    GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_M );
    GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN], FALSE );
    BmpMenuList_Rewrite( wk->menuList );
    wk->seq = SHOPBUY_SEQ_MAIN;
    break;
  case SHOPBUY_SEQ_ONLY1_YESNO:
    submenu_screen_clear( SCREEN_SUBMENU_ONLY );
    GFL_BMPWIN_MakeScreen( wk->win[SHOP_BUY_BMPWIN_LIST] );
    GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN], FALSE );
    wk->yesnoWork = BmpMenu_YesNoSelectInit(  &yesno_data, 1, MENU_WINDOW_PAL_OFFSET, 0, wk->heapId );

    // 技マシンじゃなかったら現在の所持数を表示
    if(wk->type != SHOP_TYPE_WAZA){
      print_carry_item( wk, wk->selectitem );
    }
    wk->seq = SHOPBUY_SEQ_SELECT_YESNO_WAIT;

    break;

  case SHOPBUY_SEQ_YESNO:
    submenu_screen_clear( SCREEN_SUBMENU_ONLY );
    GFL_BMPWIN_MakeScreen( wk->win[SHOP_BUY_BMPWIN_LIST] );
    GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN], FALSE );
    wk->yesnoWork = BmpMenu_YesNoSelectInit(  &yesno_data, 1, MENU_WINDOW_PAL_OFFSET, 0, wk->heapId );
    wk->seq = SHOPBUY_SEQ_SELECT_YESNO_WAIT;
    break;
  case SHOPBUY_SEQ_SELECT_YESNO_WAIT:
    ret = BmpMenu_YesNoSelectMain( wk->yesnoWork );
    if(ret!=BMPMENU_NULL){
      if(ret==0){
        wk->seq = SHOPBUY_SEQ_BUY;
        submenu_screen_clear( SCREEN_SUBMENU_ONLY );
        BmpMenuList_Rewrite( wk->menuList );
        GFL_BMPWIN_MakeScreen( wk->win[SHOP_BUY_BMPWIN_LIST] );
        GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_M );
      }else if(ret==BMPMENU_CANCEL){
        wk->seq = SHOPBUY_SEQ_BACK;
      }
    }
    break;
  case SHOPBUY_SEQ_BUY:
    PMSND_PlaySE( SEQ_SE_SYS_22 );
    ShopPrintMsg( wk, mes_shop_02_04, wk->selectitem );
    // 支払い方法によって引き算する対象を替える
    _sub_payment( wk, wk->price*wk->item_multi );
    print_mygold( wk );
    MYITEM_AddItem( wk->myitem, wk->selectitem, wk->item_multi, wk->heapId);
    wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
    wk->next = SHOPBUY_SEQ_JUDGE_PREMIER_BALL;
    break;

  // プレミアボール判定
  case SHOPBUY_SEQ_JUDGE_PREMIER_BALL:
    if( wk->selectitem==ITEM_MONSUTAABOORU && wk->item_multi>=10){
      ShopPrintMsg( wk, mes_shop_02_08, wk->selectitem );
      MYITEM_AddItem( wk->myitem, ITEM_PUREMIABOORU, 1, wk->heapId);

      // レコード：プレミアムボールを貰った回数
      RECORD_Inc( wk->record, RECID_PREMIUM_BALL );
      wk->seq  = SHOPBUY_SEQ_MSG_WAIT;
      wk->next = SHOPBUY_SEQ_BACK;
    }else{
      wk->seq  = SHOPBUY_SEQ_BACK;
    }
    break;

  // メッセージ表示待ち
  case SHOPBUY_SEQ_MSG_WAIT:    
    wk->seq = shop_buy_printwait( wk );
    break;

  case SHOPBUY_SEQ_END:
    bmpwin_list_exit(wk);   // BMPLIST解放
    obj_del( wk );          // OBJ削除
    bmpwin_exit( wk );      // BMPWIN解放
    bg_clear(wk);           // BG消去
    
    // 品揃えデータ削除
    shop_item_release( wk);

    // ワーク解放
    exit_work(wk);
    return TRUE;
    break;
  }

  PRINTSYS_QUE_Main( wk->printQue );
  PRINT_UTIL_Trans( &wk->printUtil, wk->printQue );

  GFL_TCBL_Main( wk->pMsgTcblSys ); // フィールド描画は１／３０なのでアプリと合わせるために２回呼ぶ
  GFL_TCBL_Main( wk->pMsgTcblSys );
  
  // 継続
  return FALSE;
}




//----------------------------------------------------------------------------------
/**
 * @brief どうぐショップか？技マシンショップか？（混在はできない）
 *
 * @param   wk->lineup      SHOP_ITEM構造体の配列
 * @param   wk->listup_num  登録されてる総数
 *
 * @retval  int   SHOP_TYPE_NORMAL, SHOP_TYPE_WAZA
 */
//----------------------------------------------------------------------------------
static int _lineup_check( SHOP_ITEM *list, int num )
{
  int i;
  int flag = ITEM_CheckWazaMachine(list[0].id);   // 商品の０個目はどっち？
  
  for(i=0;i<num;i++)
  {
    // 途中でどうぐと技マシンのフラグが変わったらアサート
    if(ITEM_CheckWazaMachine(list[i].id) != flag){
      GF_ASSERT_MSG(0, "通常のどうぐと技マシンが同じ商品リストに混在して登録されています\n" );
    }
  }
  
  return SHOP_TYPE_NORMAL+flag;    // SHOP_TYPE_NORMALかSHOP_TYPE_WAZAが返る
}


//----------------------------------------------------------------------------------
/**
 * @brief BMPメニューに単なる道具の時とわざマシンの時で違う文字列に整形して追加する
 *
 * @param   wk    
 * @param   type    
 * @param   id    
 */
//----------------------------------------------------------------------------------
static void _add_menuitem( SHOP_BUY_APP_WORK *wk, int i, int type, int id, GFL_MSGDATA *itemMsgData )
{

  // 技マシンショップの場合は技名をセット
  if(type==SHOP_TYPE_WAZA){
    WORDSET_RegisterNumber( wk->wordSet, 0, id-ITEM_WAZAMASIN01+1, 2, 
                            STR_NUM_DISP_ZERO, STR_NUM_CODE_ZENKAKU );
    WORDSET_RegisterWazaName( wk->wordSet, 1, ITEM_GetWazaNo(id) );
    WORDSET_ExpandStr( wk->wordSet, wk->expandBuf, wk->wazaStr );
    BmpMenuWork_ListAddString( &wk->list[i], wk->expandBuf, i, wk->heapId  );

  // 通常のどうぐ登録
  }else{
    BmpMenuWork_ListAddArchiveString( &wk->list[i], itemMsgData, 
                                      id,               // アイテムNO=GMMNO
                                      i,                // 商品テーブルのインデックス
                                      wk->heapId );
  }


}

// バッジの数でショップを変える為のテーブル
static const u8 normal_shop_offset[]={
 0,1,1,2,2,3,3,4,5,
};

// ショップコンバーターで出力した際に固定ショップが始まっている行番号
#define FIXSHOP_START_NUMBER ( 6 )

//----------------------------------------------------------------------------------
/**
 * @brief ショップアイテム登録
 *
 * @param   wk    SHOP_BUY_APP_WORK
 * @param   type  ショップタイプ(SCR_SHOPID_NULL,SCR_SHOPID_FIX)
 * @param   id    固定ショップの際のID（shop_data.cdat参照)
 * @param   badge バッジ取得総数(SCR_SHOPID_NULLの際に使用）
 */
//----------------------------------------------------------------------------------
static void shop_item_set( SHOP_BUY_APP_WORK *wk, int type, int id, int badge )
{
  int i,offset,itemnum;
  GFL_MSGDATA *itemMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                                             NARC_message_itemname_dat, wk->heapId );


  // 変動ショップ
  if(type==SCR_SHOPID_NULL)
  {
    offset = normal_shop_offset[badge];
  // 固定リストショップ
  } else {
    offset = id+FIXSHOP_START_NUMBER;
  }

  // 技マシンが入っているのであればワザショップで登録するための設定に切り替える
  if(ITEM_CheckWazaMachine(shop_data_table[offset][0])){
    wk->type = SHOP_TYPE_WAZA;
  }

  // どうぐ名登録・BMPMENULISTデータ作成
  itemnum = shop_itemnum_table[offset];
  wk->list = BmpMenuWork_ListCreate( itemnum+1, wk->heapId );
  OS_Printf("type=%d, offset=%d, badge=%d,itemnum=%d\n", type, offset, badge, itemnum);
  GF_ASSERT_MSG( itemnum<SHOP_ITEM_MAX, "ショップに一度に並べられる最大数を超えています\n" )


  for(i=0;i<itemnum;i++)
  {
    int id = shop_data_table[offset][i];
    OS_Printf("strId=%d\n", id);

    // 技マシンかどうぐ名かで文字列登録方法が違う
    _add_menuitem( wk, i, wk->type, id, itemMsgData );

    // 商品ラインナップテーブルにIDと値段を登録
    wk->lineup[i].id    = id;

    // Gパワー係数を掛けてから値段を代入する
    wk->lineup[i].price =  GPOWER_Calc_Sale(ITEM_GetParam( id, ITEM_PRM_PRICE, wk->heapId ));
  }
  wk->lineup_num = i;
  
  // どうぐか？技マシンか？（混在は×）
  _lineup_check( wk->lineup, wk->lineup_num );
  
  // 「やめる」登録
  BmpMenuWork_ListAddArchiveString( &wk->list[i], wk->shopMsgData, 
                                    mes_shop_02_06, 
                                    BMPMENULIST_CANCEL, 
                                    wk->heapId );
  GFL_MSG_Delete( itemMsgData );
}


//----------------------------------------------------------------------------------
/**
 * @brief バトルポイントショップアイテム登録
 *
 * @param   wk    SHOP_BUY_APP_WORK
 * @param   type  ショップタイプ(SCR_SHOPID_BP_ITEM,SCR_SHOPID_BP_WAZA)
 */
//----------------------------------------------------------------------------------
static void bp_item_set( SHOP_BUY_APP_WORK *wk, int type )
{
  int i,itemnum;
  GFL_MSGDATA *itemMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                                             NARC_message_itemname_dat, wk->heapId );
  const  SHOP_ITEM *itemlist;

  // 変動ショップ
  itemlist = bp_shop_data_table[type];

  // どうぐ名登録・BMPMENULISTデータ作成
  itemnum = bp_shop_itemnum_table[type];

  wk->list = BmpMenuWork_ListCreate( itemnum+1, wk->heapId );
  OS_Printf("type=%d, itemnum=%d\n", type, itemnum);
  GF_ASSERT_MSG( itemnum<SHOP_ITEM_MAX, "ショップに一度に並べられる最大数を超えています\n" )


  for(i=0;i<itemnum;i++)
  {
      int id = itemlist[i].id;
      OS_Printf("strId=%d\n", id);
      
      // 技マシンかどうぐ名かで文字列登録方法が違う
      _add_menuitem( wk, i, wk->type, itemlist[i].id, itemMsgData );
      
      // 商品ラインナップテーブルにIDと値段を登録
      wk->lineup[i].id    = itemlist[i].id;
      wk->lineup[i].price = itemlist[i].price;
  }
  wk->lineup_num = i;
  
  // 「やめる」登録
  BmpMenuWork_ListAddArchiveString( &wk->list[i], wk->shopMsgData, 
                                    mes_shop_02_06, 
                                    BMPMENULIST_CANCEL, 
                                    wk->heapId );

  GFL_MSG_Delete( itemMsgData );
}

//----------------------------------------------------------------------------------
/**
 * @brief バトルポイントショップアイテム登録
 *
 * @param   wk    SHOP_BUY_APP_WORK
 * @param   type  ショップタイプ(SCR_SHOPID_BP_ITEM,SCR_SHOPID_BP_WAZA)
 */
//----------------------------------------------------------------------------------
static void blackcity_shop_item_set( SHOP_BUY_APP_WORK *wk, int type )
{
  int i;
  GFL_MSGDATA *itemMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                                             NARC_message_itemname_dat, wk->heapId );
  GF_ASSERT_MSG( type>=SCR_SHOPID_BLACK_CITY0 && type<=SCR_SHOPID_BLACK_CITY4, 
             "BLACKCITYショップ指定0-4(0xf7-0xfb)の値になっていません\n");

  // BLACKCITY用の商品ラインナップをセット
  // BLACKCITY_SetShopItem( wk->lineup, &wk->lineup_num );
  FIELD_WFBC_CORE_SetShopData( wk->wfbc, wk->lineup, &wk->lineup_num, 
                               type-SCR_SHOPID_BLACK_CITY0, wk->heapId );


  GF_ASSERT_MSG( wk->lineup_num, "ショップの品物がひとつもありません\n" );

  wk->list = BmpMenuWork_ListCreate( wk->lineup_num+1, wk->heapId );
  for(i=0;i<wk->lineup_num;i++)
  {
      int id = wk->lineup[i].id;
      OS_Printf("strId=%d\n", id);
      
      // どうぐしか入らないはず
      _add_menuitem( wk, i, wk->type, wk->lineup[i].id, itemMsgData );
      
  }
  
  // 「やめる」登録
  BmpMenuWork_ListAddArchiveString( &wk->list[i], wk->shopMsgData, 
                                    mes_shop_02_06, 
                                    BMPMENULIST_CANCEL, 
                                    wk->heapId );

  GFL_MSG_Delete( itemMsgData );
}




//----------------------------------------------------------------------------------
/**
 * @brief ショップアイテム情報解放
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void shop_item_release( SHOP_BUY_APP_WORK *wk )
{

  // メニューリスト解放
  BmpMenuWork_ListDelete( wk->list );

}

//----------------------------------------------------------------------------------
/**
 * @brief ワーク初期化
 *
 * @param   wk    SHOP_BUY_APP_WORK
 */
//----------------------------------------------------------------------------------
static int init_work( SHOP_BUY_APP_WORK *wk, int type )
{
  // メッセージデータ確保

  wk->shopMsgData     = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, 
                                             NARC_script_message_shop_dat, wk->heapId );
  wk->itemInfoMsgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
                                             NARC_message_iteminfo_dat, wk->heapId );

  wk->wordSet   = WORDSET_Create( wk->heapId );

  // 展開用バッファ
  wk->expandBuf = GFL_STR_CreateBuffer( SHOP_STRING_MAX, wk->heapId );

  // 会話ウインドウ用タスク登録
  wk->pMsgTcblSys = GFL_TCBL_Init( wk->heapId, wk->heapId , 32 , 32 );

  // 技表示用文字列バッファ
  wk->wazaStr = GFL_MSG_CreateString( wk->shopMsgData,  mes_shop_09_01 );


#ifdef SHOP_DEBUG_COMMAND

  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_L){
    MISC_SetGold(wk->misc , 9999999);
    BSUBWAY_SCOREDATA_SetBattlePoint( wk->BpData, 5000 );
  }else if(GFL_UI_KEY_GetCont()&PAD_BUTTON_X){
    type = SCR_SHOPID_BP_ITEM;
  }else if(GFL_UI_KEY_GetCont()&PAD_BUTTON_Y){
    type = SCR_SHOPID_BP_WAZA;
  }else if(GFL_UI_KEY_GetCont()&PAD_KEY_UP){
    type = SCR_SHOPID_BLACK_CITY0;
  }else if(GFL_UI_KEY_GetCont()&PAD_BUTTON_R){
    MISC_SetGold(wk->misc , 5000);
  }
#endif

  return type;
}



//----------------------------------------------------------------------------------
/**
 * @brief ワーク解放
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void exit_work( SHOP_BUY_APP_WORK *wk )
{
  GFL_TCBL_Exit( wk->pMsgTcblSys );

  WORDSET_Delete(wk->wordSet);
  
  GFL_STR_DeleteBuffer( wk->wazaStr );
  GFL_STR_DeleteBuffer( wk->expandBuf );
  GFL_STR_DeleteBuffer( wk->HaveStr );
  GFL_STR_DeleteBuffer( wk->priceStr );

  // メッセージデータ解放
  GFL_MSG_Delete( wk->shopMsgData );
  GFL_MSG_Delete( wk->itemInfoMsgData );

}

// パレット転送本数(byte単位）
#define SHOP_BG_PLTT_NUM  ( 32*3 )

//----------------------------------------------------------------------------------
/**
 * @brief BG転送
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void bg_trans( SHOP_BUY_APP_WORK *wk )
{
  // 会話ウインドウ半透明OFF
//  FLDMSGBG_SetBlendAlpha( FALSE );
  G2_BlendNone();
  
  // 会話ウインドウがBG1面BG2面両方使用する事になったので、ショップ画面の背景は
  // VRAMの最初に方に入れてある会話ウインドウのリソースを避けて転送する事にした。
  // 会話ウインドウのキャラVRAMはBG1とBG2が同じにしてある。
    
  // ショップ画面リソース
  {
    ARCHANDLE* handle = GFL_ARC_OpenDataHandle( ARCID_SHOP_GRA, wk->heapId );
  
    // MAIN BG面Pltt
    GFL_ARCHDL_UTIL_TransVramPalette( handle, NARC_shop_gra_shop_bg_NCLR, PALTYPE_MAIN_BG, 
                                      0, SHOP_BG_PLTT_NUM, wk->heapId );
    // BG2面Char(背景）
    GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, NARC_shop_gra_shop_bg_NCGR, GFL_BG_FRAME2_M, 
                                      100, 0, 0, wk->heapId);
    // BG2面Screen(背景）
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs( handle, NARC_shop_gra_shop_bg1_NSCR, GFL_BG_FRAME2_M, 
                                            0, 100, 0, 0, wk->heapId);
    GFL_ARC_CloseDataHandle( handle );
  }
  
  // ウインドウ枠
  BmpWinFrame_GraphicSet( GFL_BG_FRAME1_M, MENU_WINDOW_CHARA_OFFSET, MENU_WINDOW_PAL_OFFSET, 
                          MENU_TYPE_SYSTEM, wk->heapId );

  // テキスト面消去
  GFL_BG_ClearScreen( GFL_BG_FRAME1_M );



  // BG2面のプライオリティを１に変更
  GFL_BG_SetPriority( GFL_BG_FRAME2_M, 1 );

}


//----------------------------------------------------------------------------------
/**
 * @brief BG消去
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void bg_clear( SHOP_BUY_APP_WORK *wk )
{
  // ショップBG面消去
  GFL_BG_ClearScreen( GFL_BG_FRAME1_M );
  GFL_BG_ClearScreen( GFL_BG_FRAME2_M );

  // ショップ用に変更していたBG2面のプライオリティを0に戻す
  GFL_BG_SetPriority( GFL_BG_FRAME2_M, 0 );

  // 会話ウインドウ半透明処理復帰
  FLDMSGBG_SetBlendAlpha( TRUE );

}

#define SHOP_BUY_OBJ_BG_PRI ( 1 )

enum{
  SHOP_OBJ_RES_PLTT=0,
  SHOP_OBJ_RES_CHAR,
  SHOP_OBJ_RES_CELL,
};

static const GFL_CLWK_DATA shop_obj_param[]={
  { 172,  22, 0, 0, 1 },// カーソル
  { 172,  92, 0, 0, 0 },// 上矢印
  { 172, 132, 1, 0, 0 },// 下矢印
  { 224, 128, 2, 0, 0 },// 上下カーソル
  {  22, 172, 0, 0, 1 },// どうぐアイコン
  // ｘ,ｙ座標,アニメーションシーケンス,ソフト優先順位  0>0xff, BG優先順位
};


//----------------------------------------------------------------------------------
/**
 * @brief ショップ画面OBJリソース読み込み
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void shop_resource_load( SHOP_BUY_APP_WORK *wk )
{
  ARCHANDLE *handle = GFL_ARC_OpenDataHandle( ARCID_SHOP_GRA, wk->heapId );

  // パレット
  wk->clact_res[0][ SHOP_OBJ_RES_PLTT ] = GFL_CLGRP_PLTT_RegisterEx( 
        handle, NARC_shop_gra_shop_obj_NCLR, CLSYS_DRAW_MAIN, 
        0, 0, 1, wk->heapId );

  // キャラ( 矢印 )
  wk->clact_res[0][ SHOP_OBJ_RES_CHAR ] = GFL_CLGRP_CGR_Register( 
        handle, NARC_shop_gra_shop_sel_NCGR, 0, CLSYS_DRAW_MAIN, wk->heapId );
  // キャラ( カーソル )
  wk->clact_res[1][ SHOP_OBJ_RES_CHAR ] = GFL_CLGRP_CGR_Register( 
        handle, NARC_shop_gra_shop_arw_NCGR, 0, CLSYS_DRAW_MAIN, wk->heapId );

  // セル（矢印）
  wk->clact_res[0][ SHOP_OBJ_RES_CELL ] = GFL_CLGRP_CELLANIM_Register( 
        handle, NARC_shop_gra_shop_sel_NCER, NARC_shop_gra_shop_sel_NANR, wk->heapId );
  // セル（カーソル）
  wk->clact_res[1][ SHOP_OBJ_RES_CELL ] = GFL_CLGRP_CELLANIM_Register( 
        handle, NARC_shop_gra_shop_arw_NCER, NARC_shop_gra_shop_arw_NANR, wk->heapId );

  GFL_ARC_CloseDataHandle( handle );
}


//----------------------------------------------------------------------------------
/**
 * @brief どうぐアイコン読み込み
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void itemicon_resource_load( SHOP_BUY_APP_WORK *wk )
{
  // パレット
  wk->clact_res[2][ SHOP_OBJ_RES_PLTT ] = GFL_CLGRP_PLTT_RegisterEx( 
        wk->itemiconHandle, ITEM_GetIndex( 1, ITEM_GET_ICON_PAL ), CLSYS_DRAW_MAIN, 
        32, 0, 1, wk->heapId );

  // キャラ
  wk->clact_res[2][ SHOP_OBJ_RES_CHAR ] = GFL_CLGRP_CGR_Register( 
        wk->itemiconHandle, ITEM_GetIndex( 1, ITEM_GET_ICON_CGX ), 0, CLSYS_DRAW_MAIN, wk->heapId );

  // セル
  wk->clact_res[2][ SHOP_OBJ_RES_CELL ] = GFL_CLGRP_CELLANIM_Register( 
        wk->itemiconHandle, NARC_item_icon_itemicon_NCER, NARC_item_icon_itemicon_NANR, wk->heapId );
 
}

//----------------------------------------------------------------------------------
/**
 * @brief どうぐアイコンリソース差し替え
 *
 * @param   wk      
 * @param   itemno  どうぐNO
 */
//----------------------------------------------------------------------------------
static void itemicon_resource_change( SHOP_BUY_APP_WORK *wk, u16 itemno )
{
  NNSG2dCharacterData* chara;
  NNSG2dPaletteData*   pltt;
  void *chara_buf,*pltt_buf;
  
  if(itemno<=ITEM_DATA_MAX){
    pltt_buf = GFL_ARCHDL_UTIL_LoadPalette( wk->itemiconHandle, 
                  ITEM_GetIndex( itemno, ITEM_GET_ICON_PAL ), &pltt, wk->heapId );
    chara_buf = GFL_ARCHDL_UTIL_LoadOBJCharacter( wk->itemiconHandle, 
                  ITEM_GetIndex( itemno, ITEM_GET_ICON_CGX ), 0, &chara, wk->heapId );
  
    GFL_CLGRP_PLTT_Replace( wk->clact_res[2][ SHOP_OBJ_RES_PLTT ], pltt, 1 );
    GFL_CLGRP_CGR_Replace( wk->clact_res[2][ SHOP_OBJ_RES_CHAR ], chara );
  
    GFL_HEAP_FreeMemory( pltt_buf );
    GFL_HEAP_FreeMemory( chara_buf );
    GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ITEM], TRUE );
  }else{
    GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ITEM], FALSE );
   
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief OBJ表示登録
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void obj_set( SHOP_BUY_APP_WORK *wk )
{
  // カーソル
  wk->ClactWork[SHOP_OBJ_CURSOR] = GFL_CLACT_WK_Create( wk->ClactUnit, 
      wk->clact_res[0][ SHOP_OBJ_RES_CHAR ], 
      wk->clact_res[0][ SHOP_OBJ_RES_PLTT ], 
      wk->clact_res[0][ SHOP_OBJ_RES_CELL ], 
      &shop_obj_param[0], CLSYS_DEFREND_MAIN, wk->heapId );

  // 矢印上
  wk->ClactWork[SHOP_OBJ_ARROW_UP] = GFL_CLACT_WK_Create( wk->ClactUnit, 
      wk->clact_res[1][ SHOP_OBJ_RES_CHAR ], 
      wk->clact_res[0][ SHOP_OBJ_RES_PLTT ], 
      wk->clact_res[1][ SHOP_OBJ_RES_CELL ], 
      &shop_obj_param[1], CLSYS_DEFREND_MAIN, wk->heapId );

  // 矢印下
  wk->ClactWork[SHOP_OBJ_ARROW_DOWN] = GFL_CLACT_WK_Create( wk->ClactUnit, 
      wk->clact_res[1][ SHOP_OBJ_RES_CHAR ], 
      wk->clact_res[0][ SHOP_OBJ_RES_PLTT ], 
      wk->clact_res[1][ SHOP_OBJ_RES_CELL ], 
      &shop_obj_param[2], CLSYS_DEFREND_MAIN, wk->heapId );

  // 矢印上下
  wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN] = GFL_CLACT_WK_Create( wk->ClactUnit, 
      wk->clact_res[1][ SHOP_OBJ_RES_CHAR ], 
      wk->clact_res[0][ SHOP_OBJ_RES_PLTT ], 
      wk->clact_res[1][ SHOP_OBJ_RES_CELL ], 
      &shop_obj_param[3], CLSYS_DEFREND_MAIN, wk->heapId );

  GFL_CLACT_WK_SetAutoAnmFlag( wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN], TRUE );

  // どうぐアイコン
  wk->ClactWork[SHOP_OBJ_ITEM] = GFL_CLACT_WK_Create( wk->ClactUnit, 
      wk->clact_res[2][ SHOP_OBJ_RES_CHAR ], 
      wk->clact_res[2][ SHOP_OBJ_RES_PLTT ], 
      wk->clact_res[2][ SHOP_OBJ_RES_CELL ], 
      &shop_obj_param[4], CLSYS_DEFREND_MAIN, wk->heapId );

  // 矢印は非表示
  GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_UP],   FALSE );
  GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_DOWN], FALSE );
  GFL_CLACT_WK_SetDrawEnable( wk->ClactWork[SHOP_OBJ_ARROW_UPDOWN], FALSE );

}

//----------------------------------------------------------------------------------
/**
 * @brief OBJ初期化
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void obj_init( SHOP_BUY_APP_WORK *wk )
{
  // CLACT_UNIT作成
  wk->ClactUnit = GFL_CLACT_UNIT_Create( SHOP_OBJ_MAX, 
                                         SHOP_BUY_OBJ_BG_PRI, wk->heapId );
  // システムに連結
  GFL_CLACT_UNIT_SetDrawEnable( wk->ClactUnit, TRUE );

  // ショップ画面OBJリソース読み込み
  shop_resource_load( wk );

  // アイテムアイコンのファイルハンドル開きっぱなし(obj_endで解放)
  wk->itemiconHandle = GFL_ARC_OpenDataHandle( ARCID_ITEMICON, wk->heapId );

  // 道具アイコンリソース読み込み
  itemicon_resource_load( wk );

  // OBJ登録
  obj_set( wk );
  
}



//----------------------------------------------------------------------------------
/**
 * @brief OBJ削除
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void obj_del( SHOP_BUY_APP_WORK *wk )
{

  // OBJリソース全解放
  GFL_CLGRP_CELLANIM_Release( wk->clact_res[2][ SHOP_OBJ_RES_CELL ] );
  GFL_CLGRP_CELLANIM_Release( wk->clact_res[1][ SHOP_OBJ_RES_CELL ] );
  GFL_CLGRP_CELLANIM_Release( wk->clact_res[0][ SHOP_OBJ_RES_CELL ] );

  GFL_CLGRP_CGR_Release( wk->clact_res[2][ SHOP_OBJ_RES_CHAR ] );
  GFL_CLGRP_CGR_Release( wk->clact_res[1][ SHOP_OBJ_RES_CHAR ] );
  GFL_CLGRP_CGR_Release( wk->clact_res[0][ SHOP_OBJ_RES_CHAR ] );

  GFL_CLGRP_PLTT_Release( wk->clact_res[2][ SHOP_OBJ_RES_PLTT ] );
  GFL_CLGRP_PLTT_Release( wk->clact_res[0][ SHOP_OBJ_RES_PLTT ] );

  // どうぐアイコンハンドル閉じる
  GFL_ARC_CloseDataHandle( wk->itemiconHandle );

  // ショップ用CLUNIT解放
  GFL_CLACT_UNIT_Delete( wk->ClactUnit );

}


//----------------------------------------------------------------------------------
/**
 * @brief BMPWIN初期化
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void bmpwin_init( SHOP_BUY_APP_WORK *wk )
{
  int i;
  const BMPWIN_DAT **tbl;
  const BMPWIN_DAT *dat;

  if(wk->payment == SHOP_PAYMENT_BP){
    tbl = Window1BmpDataTable;
  }else{
    tbl = Window0BmpDataTable;
  
  }

  for(i=0;i<SHOP_BUY_BMPWIN_MAX;i++){
    dat = tbl[i];
    wk->win[i] = GFL_BMPWIN_Create( dat->frame, dat->x, dat->y, dat->w, dat->h, 
                        dat->pal, GFL_BMP_CHRAREA_GET_B );
  }

  {
    // おこづかい
    STRBUF *str = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_05_01 );
    PRINTSYS_PrintColor( GFL_BMPWIN_GetBmp( wk->win[SHOP_BUY_BMPWIN_OKODUKAI]), 
                                            0, 0, str, wk->font, 
                                            WHITE_TEXT_W_COL );
    GFL_STR_DeleteBuffer( str );
  }

  // PRINTUTILと関連付け
  PRINT_UTIL_Setup( &wk->printUtil, wk->win[SHOP_BUY_BMPWIN_LIST] );
  wk->printQue = PRINTSYS_QUE_Create( wk->heapId );

  if(wk->payment == SHOP_PAYMENT_BP){
    GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_MONEY] );
  }else{
    GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_OKODUKAI] );
    GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_MONEY] );
  
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief BMPWIN解放
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void bmpwin_exit( SHOP_BUY_APP_WORK *wk )
{
  int i;

  PRINTSYS_QUE_Clear( wk->printQue );
  PRINTSYS_QUE_Delete( wk->printQue );

  for(i=0;i<SHOP_BUY_BMPWIN_MAX;i++){
    GFL_BMPWIN_Delete( wk->win[i] );
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief 所持金表示
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void print_mygold( SHOP_BUY_APP_WORK *wk )
{
  STRBUF *str;
  STRBUF *expand;
  
  if(wk->payment == SHOP_PAYMENT_BP){
    str    = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_05_03 );
    expand = GFL_STR_CreateBuffer( SHOP_MYGOLD_STR_MAX, wk->heapId );
  
    // 残BP取得
    WORDSET_RegisterNumber( wk->wordSet, 0, BSUBWAY_SCOREDATA_GetBattlePoint(wk->BpData), 4, 
                            STR_NUM_DISP_SPACE, STR_NUM_CODE_ZENKAKU );
  }else{
    str    = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_05_02 );
    expand = GFL_STR_CreateBuffer( SHOP_MYGOLD_STR_MAX, wk->heapId );
  
    // 残金取得
    WORDSET_RegisterNumber( wk->wordSet, 0, MISC_GetGold(wk->misc), SHOP_YEN_KETA_MAX, 
                            STR_NUM_DISP_SPACE, STR_NUM_CODE_ZENKAKU );
  }

  // 表示用文字列に展開
  WORDSET_ExpandStr( wk->wordSet, expand, str );

  // 描画
  {
    GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( wk->win[SHOP_BUY_BMPWIN_MONEY]);
    GFL_BMP_Clear( bmp, 0 );
    PRINTSYS_PrintColor( bmp,  0, 0, expand, wk->font, WHITE_TEXT_W_COL );
  
  }

  GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_MONEY] );

  GFL_STR_DeleteBuffer( expand );
  GFL_STR_DeleteBuffer( str );
}



//----------------------------------------------------------------------------------
/**
 * @brief 指定のアイテムを何個持っているか表示する
 *
 * @param   wk      
 * @param   itemno  どうぐNO
 */
//----------------------------------------------------------------------------------
static void print_carry_item( SHOP_BUY_APP_WORK *wk, u16 itemno )
{
  STRBUF *str    = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_06_01 );
  STRBUF *expand = GFL_STR_CreateBuffer( SHOP_ITEMNUM_STR_MAX, wk->heapId );


  BmpWinFrame_Write( wk->win[SHOP_BUY_BMPWIN_NUM], WINDOW_TRANS_OFF, 
                     MENU_WINDOW_CHARA_OFFSET, MENU_WINDOW_PAL_OFFSET );

  // 個数登録
  WORDSET_RegisterNumber( wk->wordSet, 0, MYITEM_GetItemNum(wk->myitem, itemno, wk->heapId), 
                          3, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU );
  WORDSET_ExpandStr( wk->wordSet, expand, str );

  // 描画
  {
    GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( wk->win[SHOP_BUY_BMPWIN_NUM]);
    GFL_BMP_Clear( bmp, 15 );
    PRINTSYS_PrintColor( bmp,  0, 0, expand, wk->font, BLACK_TEXT_W_COL );
  
  }

  GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_NUM] );

  GFL_STR_DeleteBuffer( expand );
  GFL_STR_DeleteBuffer( str );

}

//----------------------------------------------------------------------------------
/**
 * @brief どうぐ説明表示
 *
 * @param   wk    
 * @param   itemno    
 */
//----------------------------------------------------------------------------------
static void print_iteminfo( SHOP_BUY_APP_WORK *wk, u16 itemno )
{
  STRBUF *str;    

  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( wk->win[SHOP_BUY_BMPWIN_ITEMINFO]);
  GFL_BMP_Clear( bmp, 0 );

  // 描画(存在しない場合はクリア)
  if(itemno<=ITEM_DATA_MAX){
    str = GFL_MSG_CreateString( wk->itemInfoMsgData, itemno );
    PRINTSYS_PrintColor( bmp,  0, 0, str, wk->font, WHITE_TEXT_B_COL );
    GFL_STR_DeleteBuffer( str );
  }

  GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_ITEMINFO] );

}

// 支払う際の単位を切り替えるためのテーブル（円・BP）
static const payment_strtbl[]={
  mes_shop_07_02,
  mes_shop_07_03,
};

//----------------------------------------------------------------------------------
/**
 * @brief 「ｘ個数　？？？？？？円」表示
 *
 * @param   wk          
 * @param   number  どうぐの数
 * @param   one_price   道具一つの売値
 */
//----------------------------------------------------------------------------------
static void print_multiitem_price( SHOP_BUY_APP_WORK *wk, u16 number, int one_price )
{

  STRBUF *kake_str = GFL_MSG_CreateString( wk->shopMsgData, mes_shop_07_01 );
  STRBUF *yen_str   = GFL_MSG_CreateString( wk->shopMsgData, payment_strtbl[wk->payment]);
  STRBUF *expand = GFL_STR_CreateBuffer( SHOP_ITEMNUM_STR_MAX, wk->heapId );

  GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( wk->win[SHOP_BUY_BMPWIN_PRICE]);
  GFL_BMP_Clear( bmp, 15 );


  BmpWinFrame_Write( wk->win[SHOP_BUY_BMPWIN_PRICE], WINDOW_TRANS_OFF, 
                     MENU_WINDOW_CHARA_OFFSET, MENU_WINDOW_PAL_OFFSET );

  // 「ｘ？？？」
  WORDSET_RegisterNumber( wk->wordSet, 0, number, 
                          2, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU );
  WORDSET_ExpandStr( wk->wordSet, expand, kake_str );

  PRINTSYS_PrintColor( bmp,  0, 0, expand, wk->font, BLACK_TEXT_W_COL );

  // 「？？？円」
  WORDSET_RegisterNumber( wk->wordSet, 0, number*one_price, 
                          SHOP_YEN_KETA_MAX, STR_NUM_DISP_SPACE, STR_NUM_CODE_ZENKAKU );
  WORDSET_ExpandStr( wk->wordSet, expand, yen_str );

  if(wk->payment==SHOP_PAYMENT_MONEY){
    PRINTSYS_PrintColor( bmp,  11*3+2, 0, expand, wk->font, BLACK_TEXT_W_COL );
  }else{
    PRINTSYS_PrintColor( bmp,  9*3+2, 0, expand, wk->font, BLACK_TEXT_W_COL );
  }

  GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_PRICE] );

  GFL_STR_DeleteBuffer( expand );
  GFL_STR_DeleteBuffer( yen_str );
  GFL_STR_DeleteBuffer( kake_str );

  
}

// リスト描画開始Y位置
#define SHOP_LIST_LINEY   ( 8 )

//----------------------------------------------------------------------------------
/**
 * @brief BMPWINリスト初期化
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void bmpwin_list_init( SHOP_BUY_APP_WORK *wk )
{
  BMPMENULIST_HEADER header;

  
  header.list = wk->list;   //表示文字データポインタ
                            //カーソル移動ごとのコールバック関数
  header.call_back = move_callback;  // void  (*call_back)(BMPMENULIST_WORK * wk,u32 param,u8 mode);
                            //一列表示ごとのコールバック関数
  if(wk->type==SHOP_TYPE_NORMAL){
    header.icon      = line_callback;  // void  (*icon)(BMPMENULIST_WORK * wk,u32 param,u8 y);
  }else{
    header.icon      = line_callback_waza;  // void  (*icon)(BMPMENULIST_WORK * wk,u32 param,u8 y);
  }
  header.win       = wk->win[SHOP_BUY_BMPWIN_LIST];
  
  header.count     = BmpMenuWork_GetListMax(wk->list);    //リスト項目数
  header.line      = 7; //表示最大項目数
  header.label_x   = 0; //ラベル表示Ｘ座標
  header.data_x    = 0; //項目表示Ｘ座標
  header.cursor_x  = 0; //カーソル表示Ｘ座標
  header.line_y    = SHOP_LIST_LINEY; //表示Ｙ座標
  header.f_col     = 12; //表示文字色
  header.b_col     = 0;  //表示背景色
  header.s_col     = 13; //表示文字影色
  header.msg_spc   = 0;  //文字間隔Ｘ
  header.line_spc  = 0; //文字間隔Ｙ
  header.page_skip = BMPMENULIST_LRKEY_SKIP; //ページスキップタイプ
  header.font      = 0; //文字指定(本来はu8だけどそんなに作らないと思うので)
  header.c_disp_f  = 1; //ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
  header.work      = wk;// callback時に参照するワーク
  
  header.font_size_x  = 12;   //文字サイズX(ドット
  header.font_size_y  = 16;   //文字サイズY(ドット
  header.msgdata      = NULL; //表示に使用するメッセージバッファ
  header.print_util   = &wk->printUtil; //表示に使用するプリントユーティリティ
  header.print_que    = wk->printQue; //表示に使用するプリントキュー
  header.font_handle  = wk->font; //表示に使用するフォントハンドル
  

   wk->menuList = BmpMenuList_Set( &header, 0, 0, wk->heapId );
  
}

//----------------------------------------------------------------------------------
/**
 * @brief ハイリンク対策にprintQueを使用しない描画も混ぜたPrintColor
 *
 * @param   que   printQue
 * @param   bmp   BMP
 * @param   x     x座標
 * @param   y     y座標
 * @param   str   描画文字列
 * @param   font  GFL_FONT
 * @param   color PRINTSYS_LSB
 */
//----------------------------------------------------------------------------------
static void _callback_print( PRINT_QUE* que, GFL_BMP_DATA *bmp, int x, int y, STRBUF *str, GFL_FONT *font, PRINTSYS_LSB color )
{
  // コールバック内で画面にハミ出る部分を描画する時は、printQueではなく
  // PRINTSYSで直描きしないと間に合わないので分岐させる
  if(y<=SHOP_LIST_LINEY){
    PRINTSYS_PrintColor( bmp, x, y, str, font, color );
    
  }else{
    PRINTSYS_PrintQueColor( que, bmp, x, y, str, font, color );
  }
  
}

//----------------------------------------------------------------------------------
/**
 * @brief １行表示コールバック（主に値段表示処理)
 *
 * @param   wk      
 * @param   param   どうぐNO
 * @param   y       表示Y座標
 */
//----------------------------------------------------------------------------------
static void line_callback(BMPMENULIST_WORK * wk, u32 param, u8 y )
{
  SHOP_BUY_APP_WORK *sbw = BmpMenuList_GetWorkPtr( wk );

  // 「やめる」以外には値段を表示
  if(param!=BMPMENULIST_CANCEL){
    int length,bmp_w;
    // 値段セット
    WORDSET_RegisterNumber( sbw->wordSet, 1, sbw->lineup[param].price, 
                            5, STR_NUM_DISP_SPACE, STR_NUM_CODE_ZENKAKU );  
    WORDSET_ExpandStr( sbw->wordSet, sbw->expandBuf, sbw->priceStr );
    
    // 右揃え用の数値取得
    length = PRINTSYS_GetStrWidth( sbw->expandBuf, sbw->font, 0 );
    bmp_w  = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp( sbw->win[SHOP_BUY_BMPWIN_LIST]) );
    
    // 値段描画
    _callback_print(sbw->printQue, GFL_BMPWIN_GetBmp( sbw->win[SHOP_BUY_BMPWIN_LIST]), 
                            bmp_w-length, y, sbw->expandBuf, sbw->font, BLUE_TEXT_COL );
    OS_Printf("y=%d\n", y);
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief １行表示コールバック（主に値段表示処理)
 *
 * @param   wk      
 * @param   param   どうぐNO
 * @param   y       表示Y座標
 */
//----------------------------------------------------------------------------------
static void line_callback_waza(BMPMENULIST_WORK * wk, u32 param, u8 y )
{
  SHOP_BUY_APP_WORK *sbw = BmpMenuList_GetWorkPtr( wk );

  // 「やめる」以外には値段を表示
  if(param!=BMPMENULIST_CANCEL){
    // もう持ってる
    if(MYITEM_CheckItem( sbw->myitem, sbw->lineup[param].id, 1, sbw->heapId )){
      int length,bmp_w;
      length = PRINTSYS_GetStrWidth( sbw->HaveStr, sbw->font, 0 );
      bmp_w  = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp( sbw->win[SHOP_BUY_BMPWIN_LIST]) );
      // 値段描画
      _callback_print( sbw->printQue, GFL_BMPWIN_GetBmp( sbw->win[SHOP_BUY_BMPWIN_LIST]), 
                            bmp_w-length, y, sbw->HaveStr, sbw->font, BLUE_TEXT_COL );
    // 持ってない
    }else{
      int length,bmp_w;
      // 値段セット
      WORDSET_RegisterNumber( sbw->wordSet, 1, sbw->lineup[param].price, 
                              5, STR_NUM_DISP_SPACE, STR_NUM_CODE_ZENKAKU );  
      WORDSET_ExpandStr( sbw->wordSet, sbw->expandBuf, sbw->priceStr );
      
      // 右揃え用の数値取得
      length = PRINTSYS_GetStrWidth( sbw->expandBuf, sbw->font, 0 );
      bmp_w  = GFL_BMP_GetSizeX( GFL_BMPWIN_GetBmp( sbw->win[SHOP_BUY_BMPWIN_LIST]) );
      
      // 値段描画
      _callback_print( sbw->printQue, GFL_BMPWIN_GetBmp( sbw->win[SHOP_BUY_BMPWIN_LIST]), 
                            bmp_w-length, y, sbw->expandBuf, sbw->font, BLUE_TEXT_COL );
      
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief カーソル移動時のコールバック
 *
 * @param   wk      
 * @param   param   
 * @param   mode    
 */
//----------------------------------------------------------------------------------
static void move_callback( BMPMENULIST_WORK * wk, u32 param, u8 mode)
{
  SHOP_BUY_APP_WORK *sbw = BmpMenuList_GetWorkPtr( wk );
  GFL_CLACTPOS clpos;
  u16 list, cursor;
  
  // カーソル位置取得
  BmpMenuList_PosGet( wk, &list, &cursor );

  clpos.x = shop_obj_param[0].pos_x; 
  clpos.y = shop_obj_param[0].pos_y+16*cursor;

  GFL_CLACT_WK_SetPos( sbw->ClactWork[SHOP_OBJ_CURSOR], &clpos, CLSYS_DEFREND_MAIN );
  
  // どうぐ説明書き換え
  print_iteminfo( sbw, sbw->lineup[param].id );
  itemicon_resource_change( sbw, sbw->lineup[param].id );

}

//----------------------------------------------------------------------------------
/**
 * @brief BMPLIST終了・解放処理
 *
 * @param   wk    
 */
//----------------------------------------------------------------------------------
static void bmpwin_list_exit( SHOP_BUY_APP_WORK *wk )
{
  u16 list, cursor;
  
  // 
  BmpMenuList_Exit( wk->menuList, &list, &cursor );

}


//----------------------------------------------------------------------------------
/**
 * @brief プリントコールバック
 *
 * @param   param   
 *
 * @retval  BOOL    
 */
//----------------------------------------------------------------------------------
static BOOL printBuyMsgCallBack( u32 param )
{
  if(param==1){
//    PMSND_PlaySE( SEQ_SE_SYS_22 );
  }

  return FALSE;
}


//----------------------------------------------------------------------------------
/**
 * @brief メッセージ表示
 *
 * @param   wk    
 * @param   itemno    
 */
//----------------------------------------------------------------------------------
static void ShopPrintMsg( SHOP_BUY_APP_WORK *wk, int strId, u16 itemno )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win[SHOP_BUY_BMPWIN_TALKMSG]), 15 );
  BmpWinFrame_Write( wk->win[SHOP_BUY_BMPWIN_TALKMSG], WINDOW_TRANS_ON, 
                     MENU_WINDOW_CHARA_OFFSET, MENU_WINDOW_PAL_OFFSET );

  WORDSET_RegisterItemName( wk->wordSet, 0, itemno );
  WORDSET_RegisterItemPocketName( wk->wordSet, 1, 
                                    ITEM_GetParam( itemno, ITEM_PRM_POCKET, wk->heapId ) );
  {
    STRBUF *str    = GFL_MSG_CreateString( wk->shopMsgData, strId );
    WORDSET_ExpandStr( wk->wordSet, wk->expandBuf, str );
    GFL_STR_DeleteBuffer( str );
  }
  
  wk->printStream = PRINTSYS_PrintStreamCallBack(
    wk->win[SHOP_BUY_BMPWIN_TALKMSG], 0, 0, wk->expandBuf, wk->font,
    MSGSPEED_GetWait(), wk->pMsgTcblSys,
    0, wk->heapId, 0xffff, printBuyMsgCallBack );

  GFL_BMPWIN_MakeTransWindow(wk->win[SHOP_BUY_BMPWIN_TALKMSG]);

}

//----------------------------------------------------------------------------------
/**
 * @brief ショップのタイプにあわせて店員のメッセージを変える
 *
 * @param   wk    
 * @param   type    
 */
//----------------------------------------------------------------------------------
static void ShopTypeDecideMsg( SHOP_BUY_APP_WORK *wk, int type )
{
  // わざマシン購入の時のメッセージ
  if(wk->type==SHOP_TYPE_WAZA){
    if(wk->payment == SHOP_PAYMENT_BP){
      ShopDecideWazaMsg( wk, mes_shop_02_03_03, wk->selectitem, wk->price );
    }else{
      ShopDecideWazaMsg( wk, mes_shop_02_03_02, wk->selectitem, wk->price );
    }

  // 通常どうぐのメッセージ
  }else{
    if(wk->payment == SHOP_PAYMENT_BP){
      ShopDecideMsg(wk,  mes_shop_02_03_01, wk->selectitem, wk->price, wk->item_multi );
    }else{
      ShopDecideMsg(wk,  mes_shop_02_03, wk->selectitem, wk->price, wk->item_multi );
    } 
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief 文字描画用ウインドウクリア
 *
 * @param   win   
 */
//----------------------------------------------------------------------------------
static void _clear_msg_bmpwin( GFL_BMPWIN *win )
{
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( win ), 15 );
  BmpWinFrame_Write( win, WINDOW_TRANS_ON, 
                     MENU_WINDOW_CHARA_OFFSET, MENU_WINDOW_PAL_OFFSET );
}

//----------------------------------------------------------------------------------
/**
 * @brief 登録された文字列を展開・描画
 *
 * @param   wk    
 * @param   strId   
 */
//----------------------------------------------------------------------------------
static void _expand_and_print_msg( SHOP_BUY_APP_WORK *wk, int strId )
{
  STRBUF *str = GFL_MSG_CreateString( wk->shopMsgData, strId );
  WORDSET_ExpandStr( wk->wordSet, wk->expandBuf, str );
  GFL_STR_DeleteBuffer( str );
  
  wk->printStream = PRINTSYS_PrintStreamCallBack(
    wk->win[SHOP_BUY_BMPWIN_TALKMSG], 0, 0, wk->expandBuf, wk->font,
    MSGSPEED_GetWait(), wk->pMsgTcblSys,
    0, wk->heapId, 0xffff, NULL );

  GFL_BMPWIN_MakeTransWindow( wk->win[SHOP_BUY_BMPWIN_TALKMSG] );

}

//----------------------------------------------------------------------------------
/**
 * @brief 購入メッセージ表示
 *
 * @param   wk      
 * @param   strId   「●個で○○円（BP)になります」
 * @param   itemno  アイテムNO
 * @param   price   値段（円orBP）
 * @param   num     個数
 */
//----------------------------------------------------------------------------------
static void ShopDecideMsg( SHOP_BUY_APP_WORK *wk, int strId, u16 itemno, u32 price, u16 num )
{
  // BMPWINクリア
  _clear_msg_bmpwin( wk->win[SHOP_BUY_BMPWIN_TALKMSG] );

  // 表示文字列登録
  WORDSET_RegisterItemName( wk->wordSet, 0, itemno );
  WORDSET_RegisterNumber( wk->wordSet, 1, num, 
                            2, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU );  

  WORDSET_RegisterNumber( wk->wordSet, 2, num*price,
                          SHOP_YEN_KETA_MAX, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU );  

  // ウインドウ描画
  _expand_and_print_msg( wk, strId );
}

//----------------------------------------------------------------------------------
/**
 * @brief 技マシン購入を選んだ時のメッセージ表示
 *
 * @param   wk      SHOP_BUY_APP_WORK
 * @param   strId   メッセージID
 * @param   itemno  どうぐ名（技名も内部で取得）
 * @param   price   値段（円orBP）
 */
//----------------------------------------------------------------------------------
static void ShopDecideWazaMsg( SHOP_BUY_APP_WORK *wk, int strId, u16 itemno, u32 price )
{
  // BMPWINクリア
  _clear_msg_bmpwin( wk->win[SHOP_BUY_BMPWIN_TALKMSG] );
  
  // 表示文字列登録
  WORDSET_RegisterNumber( wk->wordSet, 0, itemno-ITEM_WAZAMASIN01+1, 2, 
                          STR_NUM_DISP_ZERO, STR_NUM_CODE_ZENKAKU );
  WORDSET_RegisterWazaName( wk->wordSet, 1, ITEM_GetWazaNo(itemno) );

  WORDSET_RegisterNumber( wk->wordSet, 2, price,
                          5, STR_NUM_DISP_LEFT, STR_NUM_CODE_ZENKAKU );  

  // ウインドウ描画
  _expand_and_print_msg( wk, strId );
}


#define APP_NUMSEL_NONE ( 0 )
#define APP_NUMSEL_UP   ( 1 )
#define APP_NUMSEL_DOWN ( 2 )
//==============================================================================
/**
 * @brief   数値に算出とmax,minに対して回り込み計算を行う
 *
 * @param   num   数値ワークへのポインタ
 * @param   max   数値最大
 * @param   calc  +1,-1,+10,-10のみ受け付ける
 *
 * @retval  u8    計算結果
 */
//==============================================================================
static u8 NumArroundCheck( s16 *num, u16 max, int calc )
{
  s16 tmp;

  tmp = *num;

  switch(calc){
  case -1:
    *num -= 1;
    if( *num <= 0 ){ *num = max; }
    if( *num == tmp ){ return APP_NUMSEL_NONE; }
    return APP_NUMSEL_DOWN;
    break;
  case -10:
    *num -= 10;
    if( *num <= 0 ){ *num = 1; }
    if( *num == tmp ){ return APP_NUMSEL_NONE; }
    return APP_NUMSEL_DOWN;
    break;
  case 1:
    *num += 1;
    if( *num > max ){ *num = 1; }
    if( *num == tmp ){ return APP_NUMSEL_NONE; }
    return APP_NUMSEL_UP;
    break;
  case 10:
    *num += 10;
    if( *num > max ){ *num = max; }
    if( *num == tmp ){ return APP_NUMSEL_NONE; }
    return APP_NUMSEL_UP;
    break;
  }
  return APP_NUMSEL_NONE;
}



//----------------------------------------------------------------------------------
/**
 * @brief 個数選択時のキー制御
 *
 * @param   wk    
 *
 * @retval  int   
 */
//----------------------------------------------------------------------------------
static int price_key_control( SHOP_BUY_APP_WORK *wk )
{
  if(GFL_UI_KEY_GetRepeat() & PAD_KEY_UP){
    if(NumArroundCheck( &wk->item_multi, wk->buy_max, 1 )!=APP_NUMSEL_NONE){
      PMSND_PlaySE( SEQ_SE_SELECT1 );
      print_multiitem_price( wk, wk->item_multi, wk->price );
    }
  }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN){
    if(NumArroundCheck( &wk->item_multi, wk->buy_max, -1 )!=APP_NUMSEL_NONE){
      PMSND_PlaySE( SEQ_SE_SELECT1 );
      print_multiitem_price( wk, wk->item_multi, wk->price );
    }
  }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT){
    if(NumArroundCheck( &wk->item_multi, wk->buy_max, -10 )!=APP_NUMSEL_NONE){
      PMSND_PlaySE( SEQ_SE_SELECT1 );
      print_multiitem_price( wk, wk->item_multi, wk->price );
    }
  }else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT){
    if(NumArroundCheck( &wk->item_multi, wk->buy_max,  10 )!=APP_NUMSEL_NONE){
      PMSND_PlaySE( SEQ_SE_SELECT1 );
      print_multiitem_price( wk, wk->item_multi, wk->price );
    }
  }
  if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
      PMSND_PlaySE( SEQ_SE_DECIDE1 );
//      ShopDecideMsg(wk,  mes_shop_02_03, wk->selectitem, wk->price, wk->item_multi );
      ShopTypeDecideMsg( wk, wk->type );
      wk->next = SHOPBUY_SEQ_YESNO;
      return SHOPBUY_SEQ_MSG_WAIT;
  }else if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_CANCEL){
      PMSND_PlaySE( SEQ_SE_CANCEL1 );
    return SHOPBUY_SEQ_BACK;
  }

  return SHOPBUY_SEQ_DECIDE_NUM;
}


//----------------------------------------------------------------------------------
/**
 * @brief サブメニューのスクリーンクリア
 *
 * @param   type    
 */
//----------------------------------------------------------------------------------
static void submenu_screen_clear( int type )
{
  static const clear_rect[][4]={
    { 0,12,32,12 },   // SCREEN_SUBMENU_AND_INFOWIN
    { 0,12,32, 6 },   // SCREEN_SUBMENU_ONLY
  };

  // 指定の範囲のスクリーンクリア
  GFL_BG_FillScreen( GFL_BG_FRAME1_M, 0, 
    clear_rect[type][0], clear_rect[type][1],
    clear_rect[type][2], clear_rect[type][3], 0  );

}


