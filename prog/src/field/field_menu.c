//======================================================================
/**
 * @file  field_menu.c
 * @brief フィールドメニュー
 * @author  ariizumi
 * @date  09/05/12
 */
//======================================================================
#include <gflib.h>
#include <calctool.h>

#include "system/main.h"
#include "system/gfl_use.h"

#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "field_menu.naix"
#include "msg/msg_fldmapmenu.h"

#include "savedata/save_control.h"
#include "savedata/mystatus.h"
#include "sound/pm_sndsys.h"
#include "system/wipe.h"
#include "../../../resource/fldmapdata/flagwork/flag_define.h"
#include "app/app_menu_common.h"
#include "infowin/infowin.h"

#include "field/field_msgbg.h"
#include "field/fieldmap.h"
#include "field_subscreen.h"
#include "field_menu.h"
#include "field/zonedata.h" //ZONEDATA_IsUnionRoom
#include "field/eventwork.h"

// LとRで項目を減らすテストができる
//#define TEST_FIELD_MENU


//======================================================================
//  define
//======================================================================
#pragma mark [> define

//InfoBarでS0
#define FIELD_MENU_BG_BUTTON    (GFL_BG_FRAME1_S)
#define FIELD_MENU_BG_NAME      (GFL_BG_FRAME2_S)
#define FIELD_MENU_BG_BACK      (GFL_BG_FRAME3_S)
#define FIELD_MENU_INFOBAR      (GFL_BG_FRAME0_S)
#define FIELD_MENU_BG_PLATE_M   (GFL_BG_FRAME1_M) // 上画面「下を見て！」と促すBG

//スクロール速度
#define FIELD_MENU_SCROLL_SPD (64) 

//レンダーのサーフェイス番号
#define FIELD_MENU_RENDER_SURFACE (0)

//戻る込みのアイテム個数
#define FIELD_MENU_ITEM_NUM (7) 

//セルの回り込み防止チェックに使う。縦の最大サイズを
#define FIELD_MENU_ICON_SIZE_Y (32) 
#define FIELD_MENU_CURSOR_SIZE_Y (44)
//アイコン確定時のウェイト
#define FIELD_MENU_ICON_DECIDE_ANIME_CNT (3) 

//カーソル不正位置
#define FIELD_MENU_INVALU_CURSOR (0xFF)

//パレット
#define FIELD_MENU_PLT_FONT (0x0d)
//InfoBarで0x0f



//======================================================================
//  enum
//======================================================================
#pragma mark [> enum
enum
{
  FMO_COM_PLT,
  FMO_COM_CHR,
  FMO_COM_CEL,
  
  TOUCHBAR_PLT,
  TOUCHBAR_CHR,
  TOUCHBAR_CEL,
  
  FIELD_MENU_OBJ_RES_NUM,
}FIELD_MENU_OBJ_RES;

//カーソルアニメ

//アイコンアニメ
enum
{
  FIA_NORMAL=0, ///< 停止状態
  FIA_ACTIVE,   ///< カーソルが乗ってアクティブ状態
  FIA_DECIDE,   ///< 決定時(使用されてない？）
  
}FIELD_MENU_ICON_ANIME;

// カーソルアニメ定義
enum
{
 FCA_NORMAL=0,
 FCA_DECIDE_CURSOR,
}FIELD_MENU_CURSOR_ANIME;


typedef enum
{
  FMS_WAIT_MOVEIN,      ///< 背景スクロールイン
  FMS_WAIT_INIT,        ///< 起動待ち
  FMS_WAIT_WIPE,        ///< ワイプ終了待ち
  FMS_LOOP,             ///< 操作待ちループ
  FMS_EXIT_ANIME_WAIT,  ///< 「×」OBJ点滅
  FMS_EXIT_INIT,        ///< メニュー終了処理
  FMS_WAIT_ICON_ANIME , ///< アイコン決定時アニメ
  FMS_EXIT_ENDMENU,     ///< 終了時の処理
  FMS_EXIT_DECIDEITEM,  ///< 項目決定時の処理
  FMS_WAIT_MOVEOUT,     ///< 背景スクロールアウト
  FMS_FINISH,           ///< 終了
  
  FIELD_MENU_STATE_MAX,
}FIELD_MENU_STATE;

//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
//アイコン作成用ワーク
typedef struct
{
  FIELD_MENU_ITEM_TYPE type;
  u8  cursorPosX;
  u8  cursorPosY;
  u8  strBmpPosX; //これだけキャラ単位
  u8  strBmpPosY;
  u8  iconPosX;
  u8  iconPosY;
  STRBUF *str;
  
  ARCHANDLE *arcHandle; //NULLならアイコンなし
  u32 ncgArcIdx;
  u32 nceArcIdx;
  u32 anmArcIdx;
  
}FIELD_MENU_ICON_INIT;

typedef struct
{
  BOOL isInit;
  GFL_BMPWIN *strBmp;
  FIELD_MENU_ITEM_TYPE type;
  u32   objResChr;
  u32   objResAnm;
  u8    posX;
  u8    posY;
  GFL_CLWK  *cellIcon;
}FIELD_MENU_ICON;

struct _FIELD_MENU_WORK
{
  HEAPID heapId;
  HEAPID tempHeapId;  //一時確保用ヒープ
  FIELDMAP_WORK *fieldWork;
  FIELD_SUBSCREEN_WORK *subScrWork;
  FIELD_MENU_STATE state;
  FLDMSGBG *fld_msg;
  EVENTWORK *ev;
  MYSTATUS  *my;
  u16 zoneId;

  u8  cursorPosX; //0か1
  u8  cursorPosY; //0〜3(3は戻る)
  u8  waitCnt;
  u8  menuType;
  BOOL isUpdateCursor;
  BOOL isCancel;
  BOOL isDispCursor;  // カーソルの表示・非表示(TRUE:表示　FALSE非表示）
  int  funcType;      // 決定した機能
  FIELD_MENU_ICON icon[FIELD_MENU_ITEM_NUM];
  FIELD_MENU_ICON *activeIcon; //参照中のアイコン
  
  s16 scrollOffset;
  BOOL isUpdateScroll;
  
  u32 objRes[FIELD_MENU_OBJ_RES_NUM];
  GFL_CLUNIT *cellUnit;
  GFL_CLSYS_REND *cellRender;
  GFL_CLWK  *cellCursor;
  GFL_CLWK  *cellEndButton;
  PRINT_QUE *printQue;
  
  GFL_TCB *vBlankTcb;
  int     palanm_count;
};

//======================================================================
//  proto
//======================================================================
#pragma mark [> proto
static void FIELD_MENU_InitGraphic(  FIELD_MENU_WORK* work , ARCHANDLE *arcHandle, int menuType, FLDMSGBG *fld_msg );
static void FIELD_MENU_InitIcon( FIELD_MENU_WORK* work , ARCHANDLE *arcHandle, int menuType );

static void FIELD_MENU_InitScrollIn( FIELD_MENU_WORK* work );
static void FIELD_MENU_VBlankTCB( GFL_TCB *tcb , void *userWork );

static void  FIELD_MENU_UpdateKey( FIELD_MENU_WORK* work );
static void  FIELD_MENU_UpdateTP( FIELD_MENU_WORK* work );
static void  FIELD_MENU_UpdateCursor( FIELD_MENU_WORK* work );

static void FIELD_MENU_Icon_CreateIcon( FIELD_MENU_WORK* work , FIELD_MENU_ICON *icon , const FIELD_MENU_ICON_INIT *initWork );
static void FIELD_MENU_Icon_DeleteIcon( FIELD_MENU_WORK* work , FIELD_MENU_ICON *icon ); 
static void FIELD_MENU_Icon_TransBmp( FIELD_MENU_WORK* work , FIELD_MENU_ICON *icon );

static void _trans_touchbar_screen( HEAPID heapId, int bgfrm );
static void _cancel_func_set( FIELD_MENU_WORK *work, BOOL isCancel );
static void _set_cursor_pos( FIELD_MENU_WORK *work, int x, int y );

static void ScrollObj( FIELD_MENU_WORK* work );



static const u16 FIELD_MENU_ITEM_MSG_ARR[FMIT_MAX] =
{
  FLDMAPMENU_STR_ITEM07,    // 「」空白
  FLDMAPMENU_STR_ITEM02,    // 「ポケモン」
  FLDMAPMENU_STR_ITEM01,    // 「ずかん」
  FLDMAPMENU_STR_ITEM03,    // 「バッグ」
  FLDMAPMENU_STR_ITEM04,    // 「じぶん」
  FLDMAPMENU_STR_ITEM05,    // 「レポート」
  FLDMAPMENU_STR_ITEM06,    // 「せってい」
  FLDMAPMENU_STR_ITEM09,    // 「」空白
};


static const u32 FIELD_MENU_ITEM_ICON_RES_ARR[FMIT_MAX+2][3] =
{
  //ダミー
  { NARC_field_menu_menu_obj_common_NCGR, 
    NARC_field_menu_menu_obj_common_NCER,
    NARC_field_menu_menu_obj_common_NANR},
  // モンスターボール
  { NARC_field_menu_menu_icon_poke_NCGR,
    NARC_field_menu_menu_icon_poke_NCER,
    NARC_field_menu_menu_icon_poke_NANR},
  // ずかん
  { NARC_field_menu_menu_icon_zukan_NCGR,
    NARC_field_menu_menu_icon_zukan_NCER,
    NARC_field_menu_menu_icon_zukan_NANR},
  // バッグ(男の子）
  { NARC_field_menu_menu_icon_itembag_NCGR,
    NARC_field_menu_menu_icon_itembag_NCER,
    NARC_field_menu_menu_icon_itembag_NANR},
  // トレーナーカード
  { NARC_field_menu_menu_icon_tcard_NCGR,
    NARC_field_menu_menu_icon_tcard_NCER,
    NARC_field_menu_menu_icon_tcard_NANR},
  // レポートアイコン
  { NARC_field_menu_menu_icon_report_NCGR,
    NARC_field_menu_menu_icon_report_NCER,
    NARC_field_menu_menu_icon_report_NANR},
  // 設定画面アイコン
  { NARC_field_menu_menu_icon_confing_NCGR,
    NARC_field_menu_menu_icon_confing_NCER,
    NARC_field_menu_menu_icon_confing_NANR},
  //ダミー
  { NARC_field_menu_menu_icon_poke_NCGR, 
    NARC_field_menu_menu_icon_poke_NCER,
    NARC_field_menu_menu_icon_poke_NANR},
  // バッグ(女の子）
  { NARC_field_menu_menu_icon_itembag2_NCGR,
    NARC_field_menu_menu_icon_itembag2_NCER,
    NARC_field_menu_menu_icon_itembag2_NANR},
  // 図鑑(女の子）
  { NARC_field_menu_menu_icon_zukan2_NCGR,
    NARC_field_menu_menu_icon_zukan2_NCER,
    NARC_field_menu_menu_icon_zukan2_NANR},
};

#define GIRLS_BAG_RES     ( 8 )
#define GIRLS_ZUKAN_RES   ( 9 )

//--------------------------------------------------------------
//  初期化
//--------------------------------------------------------------
FIELD_MENU_WORK* FIELD_MENU_InitMenu( const HEAPID heapId , const HEAPID tempHeapId , FIELD_SUBSCREEN_WORK* subScreenWork , FIELDMAP_WORK *fieldWork , const BOOL isScrollIn )
{
  int menuType;
  FIELD_MENU_WORK *work = GFL_HEAP_AllocMemory( heapId , sizeof( FIELD_MENU_WORK ) );
  ARCHANDLE *arcHandle;
  GAMESYS_WORK *gameSys = FIELDMAP_GetGameSysWork( fieldWork );
  GAMEDATA *gameData = GAMESYSTEM_GetGameData( gameSys );
  work->heapId = heapId;
  work->tempHeapId = tempHeapId;
  work->fieldWork = fieldWork;
  work->subScrWork = subScreenWork;
  work->state = FMS_WAIT_INIT;
  work->zoneId = PLAYERWORK_getZoneID( GAMESYSTEM_GetMyPlayerWork(gameSys) );
  work->ev = GAMEDATA_GetEventWork( gameData );
  work->my = GAMEDATA_GetMyStatus( gameData );
  work->fld_msg = FIELDMAP_GetFldMsgBG( fieldWork );
  
  work->cursorPosX = 0;
  work->cursorPosY = 0;
  work->isDispCursor   = GFL_UI_CheckTouchOrKey()^1;
  work->isUpdateCursor = work->isDispCursor;
  work->activeIcon = NULL;
  work->vBlankTcb = GFUser_VIntr_CreateTCB( FIELD_MENU_VBlankTCB , (void*)work , 0 );
  
  work->printQue = PRINTSYS_QUE_Create( work->heapId );
  work->isUpdateScroll = FALSE;
  work->scrollOffset = 0;
  
  arcHandle = GFL_ARC_OpenDataHandle( ARCID_FIELD_MENU , work->tempHeapId );
  //メニューの種類取得
  menuType = FIELDMENU_GetMenuType( gameData, work->ev, work->zoneId );
  work->menuType = menuType;

  FIELD_MENU_InitGraphic( work , arcHandle, menuType, work->fld_msg);
  FIELD_MENU_InitIcon( work , arcHandle, menuType);
  GFL_ARC_CloseDataHandle(arcHandle);

  OS_Printf("KEY TOUCH MODE = %d\n", GFL_UI_CheckTouchOrKey());

  GFL_BG_SetVisible( FIELD_MENU_BG_PLATE_M, VISIBLE_ON );
  
  OS_Printf("TrainerView = %d\n", MyStatus_GetTrainerView( work->my ));

  
  // スクロールインか？
  if( isScrollIn == TRUE )
  {
    work->cursorPosX = 0;
    work->cursorPosY = 0;
    work->isUpdateCursor = TRUE;
    FIELD_MENU_UpdateCursor( work );
    FIELD_MENU_InitScrollIn( work );
  }
  else
  {
    // 引き継いだカーソル初期位置を取得
    FIELD_MENU_SetMenuItemNo( work , GAMEDATA_GetSubScreenType( gameData ) );
    work->isUpdateCursor = TRUE;
    if(work->isDispCursor){
      GFL_CLACT_WK_SetDrawEnable( work->cellCursor, TRUE );
    }else{
      GFL_CLACT_WK_SetDrawEnable( work->cellCursor, FALSE );
    }
  }
  
  GFL_NET_ReloadIconTopOrBottom( FALSE , work->tempHeapId );

  return work;
}


//----------------------------------------------------------------------------------
/**
 * @brief 上画面の輝度OFFを判定した上で戻したり戻さなかったりする
 *
 * @param   work    
 */
//----------------------------------------------------------------------------------
static void _return_brightness( FIELD_MENU_WORK *work )
{
  // フィールドメニューをキャンセルしたか、レポート画面決定で終了するのでなければ
  if( work->isCancel==TRUE ){
    // 上画面に掛けていた輝度オフを戻す
    FIELD_SUBSCREEN_MainDispBrightnessOff( work->subScrWork, work->heapId );
    OS_Printf("上画面解放した isCancel=%d, funcType=%d\n", work->isCancel, work->funcType);
  }
}

//--------------------------------------------------------------
//  開放
//--------------------------------------------------------------
void FIELD_MENU_ExitMenu( FIELD_MENU_WORK* work )
{
  u8 i;
  for( i=0;i<FIELD_MENU_ITEM_NUM;i++ )
  {
    FIELD_MENU_Icon_DeleteIcon( work , &work->icon[i] );
  }
//  GFL_BG_FreeBGControl( FIELD_MENU_BG_PLATE_M );

  // 上画面の輝度OFFを戻すかどうか判定して処理
  _return_brightness( work );

  GFL_TCB_DeleteTask( work->vBlankTcb );
  
  GFL_CLACT_WK_Remove( work->cellEndButton );
  GFL_CLACT_WK_Remove( work->cellCursor );
  GFL_CLACT_USERREND_Delete( work->cellRender );

  GFL_CLACT_UNIT_Delete( work->cellUnit );
  GFL_CLGRP_CELLANIM_Release( work->objRes[FMO_COM_CEL] );
  GFL_CLGRP_CGR_Release( work->objRes[FMO_COM_CHR] );
  GFL_CLGRP_PLTT_Release( work->objRes[FMO_COM_PLT] );
  GFL_CLGRP_CELLANIM_Release( work->objRes[TOUCHBAR_CEL] );
  GFL_CLGRP_CGR_Release( work->objRes[TOUCHBAR_CHR] );
  GFL_CLGRP_PLTT_Release( work->objRes[TOUCHBAR_PLT] );

  GFL_BG_SetScroll( FIELD_MENU_BG_NAME , GFL_BG_SCROLL_Y_SET , 0 );
  GFL_BG_FreeBGControl( FIELD_MENU_BG_BACK );
  GFL_BG_FreeBGControl( FIELD_MENU_BG_BUTTON );
  GFL_BG_FreeBGControl( FIELD_MENU_BG_NAME );
  PRINTSYS_QUE_Clear( work->printQue ); 
  PRINTSYS_QUE_Delete( work->printQue ); 
  GFL_HEAP_FreeMemory( work );
}


//----------------------------------------------------------------------------------
/**
 * @brief 全てのBG面を有効にする
 *
 * @param   none    
 */
//----------------------------------------------------------------------------------
static void _all_bg_appear( void )
{
  GFL_BG_LoadScreenReq( FIELD_MENU_BG_NAME );

  GFL_BG_SetVisible( FIELD_MENU_INFOBAR, VISIBLE_ON );
  GFL_BG_SetVisible( FIELD_MENU_BG_BUTTON, VISIBLE_ON );
  GFL_BG_SetVisible( FIELD_MENU_BG_NAME  , VISIBLE_ON );
  GFL_BG_SetVisible( FIELD_MENU_BG_BACK  , VISIBLE_ON );
  
}

//--------------------------------------------------------------
//  更新
//--------------------------------------------------------------
void FIELD_MENU_UpdateMenu( FIELD_MENU_WORK* work )
{
  switch( work->state )
  {
  // 初期化
  case FMS_WAIT_INIT:
    if( PRINTSYS_QUE_IsFinished( work->printQue ) == TRUE )
    {
      u8 i;
      
      // 上画面に「下画面をみてね」プレートを表示
      FIELD_SUBSCREEN_SetMainLCDNavigationScreen( work->subScrWork, work->tempHeapId );

      for( i=0;i<FIELD_MENU_ITEM_NUM;i++ )
      {
        FIELD_MENU_Icon_TransBmp( work , &work->icon[i] );
      }
      if( work->scrollOffset != 0 )
      {
        PMSND_PlaySystemSE( SEQ_SE_OPEN1 );
        work->state = FMS_WAIT_MOVEIN;
      }
      else
      {
        work->state = FMS_WAIT_WIPE;
        _all_bg_appear();     // 全てのBGM面をON
      }
    }
    break;
  // スクロールしてくるとき
  case FMS_WAIT_MOVEIN:
    if( work->scrollOffset < FIELD_MENU_SCROLL_SPD )
    {
      work->scrollOffset = 0;
      work->state = FMS_LOOP;
      _all_bg_appear();     // 全てのBGM面をON
    }
    else
    {
      work->scrollOffset -= FIELD_MENU_SCROLL_SPD;
      ScrollObj( work );
    }
    work->isUpdateScroll = TRUE;
    break;

  case FMS_WAIT_WIPE:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      // 上画面に「下画面をみてね」プレートを表示
      FIELD_SUBSCREEN_SetMainLCDNavigationScreen( work->subScrWork, work->tempHeapId );
      work->state = FMS_LOOP;
    }
    break;

  // 通常時
  case FMS_LOOP:
    FIELD_MENU_UpdateKey( work );
    FIELD_MENU_UpdateTP( work );
    FIELD_MENU_UpdateCursor( work );
    break;
   
  // 終了スクロールアニメ待ちの時
  case FMS_EXIT_ANIME_WAIT:
    if(GFL_CLACT_WK_CheckAnmActive( work->cellEndButton )==FALSE){
      work->state = FMS_EXIT_INIT;
      // インフォバー・タッチバーの面をON
      GFL_BG_SetVisible( FIELD_MENU_BG_BUTTON, VISIBLE_ON );
      GFL_BG_SetVisible( FIELD_MENU_BG_NAME  , VISIBLE_ON );
      GFL_BG_SetVisible( FIELD_MENU_BG_BACK  , VISIBLE_ON );
      GFL_BG_SetVisible( FIELD_MENU_INFOBAR, VISIBLE_ON );
    }
    break;

  // 終了処理
  case FMS_EXIT_INIT:
    if( work->isCancel == TRUE ||
        work->cursorPosY == 3 )
    {
      //work->state = FMS_EXIT_ENDMENU;
      work->state = FMS_WAIT_MOVEOUT;
      PMSND_PlaySystemSE( SEQ_SE_CLOSE1 );
      // インフォバー・タッチバーの面をOFF
      GFL_BG_SetVisible( FIELD_MENU_INFOBAR, VISIBLE_OFF );
      GFL_BG_SetVisible( FIELD_MENU_BG_BUTTON, VISIBLE_ON );
      GFL_BG_SetVisible( FIELD_MENU_BG_NAME  , VISIBLE_ON );
      GFL_BG_SetVisible( FIELD_MENU_BG_BACK  , VISIBLE_ON );
      GFL_CLACT_WK_SetDrawEnable( work->cellEndButton, FALSE );

    }
    else
    {
      if( work->activeIcon->cellIcon != NULL )
      {
        work->waitCnt = 0;
        _set_cursor_pos( work, work->activeIcon->posX, work->activeIcon->posY );
        GFL_CLACT_WK_SetAnmSeq( work->cellCursor, FCA_DECIDE_CURSOR );
        GFL_CLACT_WK_SetDrawEnable( work->cellCursor, TRUE );
        work->state = FMS_WAIT_ICON_ANIME;
        PMSND_PlaySystemSE( SEQ_SE_DECIDE1 );
      }
      else
      {
        work->state = FMS_EXIT_DECIDEITEM;
      }
    }
    break;
    
  case FMS_WAIT_ICON_ANIME:
    work->waitCnt++;
//    if( work->waitCnt > FIELD_MENU_ICON_DECIDE_ANIME_CNT )
    // カーソルアニメの終了待ち
    if(GFL_CLACT_WK_CheckAnmActive( work->cellCursor )==FALSE)
    {
      work->state = FMS_EXIT_DECIDEITEM;
    }
    break;
  case FMS_EXIT_ENDMENU:
    FIELD_SUBSCREEN_SetAction( work->subScrWork , FIELD_SUBSCREEN_ACTION_TOPMENU_EXIT );
    work->state = FMS_FINISH;
    break;
    
  case FMS_EXIT_DECIDEITEM:
    FIELD_SUBSCREEN_SetAction( work->subScrWork , FIELD_SUBSCREEN_ACTION_TOPMENU_DECIDE );
    work->state = FMS_FINISH;
    break;
  case FMS_WAIT_MOVEOUT:
    if( work->scrollOffset + FIELD_MENU_SCROLL_SPD > 192 )
    {
      work->scrollOffset = 192;
      work->state = FMS_EXIT_ENDMENU;
    }
    else
    {
      work->scrollOffset += FIELD_MENU_SCROLL_SPD;
      ScrollObj( work );
    }
    work->isUpdateScroll = TRUE;
  }
  PRINTSYS_QUE_Main( work->printQue );
}

// OBJスクロール
static void ScrollObj( FIELD_MENU_WORK* work )
{
  u8 i;
  GFL_CLACTPOS sufacePos;

  sufacePos.x = 0;
  sufacePos.y = -work->scrollOffset;
  GFL_CLACT_USERREND_SetSurfacePos( work->cellRender , FIELD_MENU_RENDER_SURFACE , &sufacePos );
  //アイコンの処理
  for( i=0;i<FIELD_MENU_ITEM_NUM;i++ )
  {
    if( work->icon[i].cellIcon != NULL && work->icon[i].type!=FMIT_NONE)
    {
      GFL_CLACTPOS cellPos;
      GFL_CLACT_WK_GetPos( work->icon[i].cellIcon , &cellPos , FIELD_MENU_RENDER_SURFACE );
      if( cellPos.y < 192 + FIELD_MENU_ICON_SIZE_Y/2 )
      {
        GFL_CLACT_WK_SetDrawEnable( work->icon[i].cellIcon, TRUE );
      }
      else
      {
        GFL_CLACT_WK_SetDrawEnable( work->icon[i].cellIcon, FALSE );
      }
    }
  }
  //カーソルの処理
  {
    GFL_CLACTPOS cellPos;
    GFL_CLACT_WK_GetPos( work->cellCursor , &cellPos , FIELD_MENU_RENDER_SURFACE );
    if( work->isDispCursor &&
        cellPos.y < 192 + FIELD_MENU_CURSOR_SIZE_Y/2 )
    {
      GFL_CLACT_WK_SetDrawEnable( work->cellCursor, TRUE );
    }
    else
    {
      GFL_CLACT_WK_SetDrawEnable( work->cellCursor, FALSE );
    }
  }
}


//--------------------------------------------------------------
//  更新
//--------------------------------------------------------------
void FIELD_MENU_DrawMenu( FIELD_MENU_WORK* work )
{
  if( work->isUpdateScroll == TRUE )
  {
    // BGスクロール
    // OBJはupdateで。
    GFL_BG_SetScrollReq( FIELD_MENU_BG_BUTTON , GFL_BG_SCROLL_Y_SET , -work->scrollOffset );
    GFL_BG_SetScrollReq( FIELD_MENU_BG_NAME , GFL_BG_SCROLL_Y_SET , -work->scrollOffset -4 );
    work->isUpdateScroll = FALSE;
  }
}


// メニューの項目数に対応してスクリーンを書き換える
static const u32 menu_screen_table[] = {
 NARC_field_menu_menu_bg_NSCR,      // 通常
 NARC_field_menu_menu_bg_5_NSCR,    // ユニオン
 NARC_field_menu_menu_bg_5_NSCR,    // 図鑑無し
 NARC_field_menu_menu_bg_4_NSCR,    // ポケモン・図鑑両方無し
 NARC_field_menu_menu_bg_5_NSCR,    // 遊覧船
 NARC_field_menu_menu_bg_5_NSCR,    // パレス
};


//--------------------------------------------------------------
//  グラフィック系初期化
//--------------------------------------------------------------
static void FIELD_MENU_InitGraphic(  FIELD_MENU_WORK* work , ARCHANDLE *arcHandle, int menuType, FLDMSGBG *fld_msg )
{
  static const GFL_BG_BGCNT_HEADER bgCont_Button = {
  0, 0, 0x1000, 0,
  GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x00000, 0x5800,
  GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
  };
  static const GFL_BG_BGCNT_HEADER bgCont_BackStr = {
  0, 0, 0x1000, 0,
  GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000, 0x5800,
  GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
  };
  static const GFL_BG_BGCNT_HEADER bgCont_BackGround = {
  0, 0, 0x800, 0,
  GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x5000, GX_BG_CHARBASE_0x00000, 0x5800,
  GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
  };
  static const GFL_BG_BGCNT_HEADER bgCont_Plate_M = {
  0, 0, 0x800, 0,
  GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
  GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
  GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
  };

  GXS_SetMasterBrightness(-16);
  // BG初期化(
  GFL_BG_SetBGControl( FIELD_MENU_BG_BUTTON, &bgCont_Button, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( FIELD_MENU_BG_BUTTON, VISIBLE_ON );
  GFL_BG_ClearScreen( FIELD_MENU_BG_BUTTON );
  
  GFL_BG_SetBGControl( FIELD_MENU_BG_NAME, &bgCont_BackStr, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( FIELD_MENU_BG_NAME, VISIBLE_ON );
  GFL_BG_ClearScreen( FIELD_MENU_BG_NAME );

  GFL_BG_SetBGControl( FIELD_MENU_BG_BACK, &bgCont_BackGround, GFL_BG_MODE_TEXT );
  GFL_BG_SetVisible( FIELD_MENU_BG_BACK, VISIBLE_ON );
  GFL_BG_ClearScreen( FIELD_MENU_BG_BACK );

  //フォントをずらした位置に出したいので
  GFL_BG_SetScroll( FIELD_MENU_BG_NAME , GFL_BG_SCROLL_Y_SET , -4 );
  
  // BGグラフィック転送
  // 下画面背景
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_field_menu_menu_bg_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->tempHeapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_field_menu_menu_bg_NCGR ,
                    FIELD_MENU_BG_BUTTON , 0 , 0, FALSE , work->tempHeapId );

  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , menu_screen_table[menuType] , 
                      FIELD_MENU_BG_BUTTON ,  0 , 0, FALSE , work->tempHeapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_field_menu_menu_back_NSCR, 
                      FIELD_MENU_BG_BACK ,  0 , 0, FALSE , work->tempHeapId );

  // セルアクターリソース転送(カーソル）
  work->objRes[FMO_COM_PLT] = GFL_CLGRP_PLTT_RegisterEx( arcHandle, NARC_field_menu_menu_obj_common_NCLR, 
                                                         CLSYS_DRAW_SUB, 0, 0, 11, work->heapId );
  
  work->objRes[FMO_COM_CHR] = GFL_CLGRP_CGR_Register( arcHandle , 
                                  NARC_field_menu_menu_obj_common_NCGR ,
                                  FALSE , CLSYS_DRAW_SUB , work->heapId );
  
  work->objRes[FMO_COM_CEL] = GFL_CLGRP_CELLANIM_Register( arcHandle , 
                                  NARC_field_menu_menu_obj_common_NCER ,
                                  NARC_field_menu_menu_obj_common_NANR ,
                                  work->heapId );
  
  // セルアクターリソース転送(タッチバー『×』）
  {
    ARCHANDLE *commonHandle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), work->tempHeapId );
    work->objRes[TOUCHBAR_PLT] = GFL_CLGRP_PLTT_RegisterEx( commonHandle, APP_COMMON_GetBarIconPltArcIdx(), 
                                                            CLSYS_DRAW_SUB, 11*32, 0, 2, work->heapId );
  
    work->objRes[TOUCHBAR_CHR] = GFL_CLGRP_CGR_Register( commonHandle, 
                                    APP_COMMON_GetBarIconCharArcIdx(),
                                    FALSE, CLSYS_DRAW_SUB, work->heapId );
    
    work->objRes[TOUCHBAR_CEL] = GFL_CLGRP_CELLANIM_Register( commonHandle, 
                                    APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_32K) ,
                                    APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_32K),
                                    work->heapId );
    GFL_ARC_CloseDataHandle( commonHandle );
  }
  
  GFL_BG_LoadScreenReq( FIELD_MENU_BG_BUTTON );
  
  //セル系システムの作成
  work->cellUnit = GFL_CLACT_UNIT_Create( 8 , 0 , work->heapId );
  
  //レンダラー作成
  {
    const GFL_REND_SURFACE_INIT renderInitData = { 0,0,256,191,CLSYS_DRAW_SUB, CLSYS_REND_CULLING_TYPE_NORMAL};
    
    work->cellRender = GFL_CLACT_USERREND_Create( &renderInitData , 1 , work->heapId );
    GFL_CLACT_UNIT_SetUserRend( work->cellUnit , work->cellRender );
  }
  
  // セルアクター登録
  {
    GFL_CLWK_DATA cellInitData;
    //セルの生成
    cellInitData.pos_x =  64;
    cellInitData.pos_y =  44;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    // カーソル
    work->cellCursor = GFL_CLACT_WK_Create( work->cellUnit ,
                          work->objRes[FMO_COM_CHR],
                          work->objRes[FMO_COM_PLT],
                          work->objRes[FMO_COM_CEL],
                          &cellInitData , 
                          FIELD_MENU_RENDER_SURFACE , 
                          work->heapId );
    GFL_CLACT_WK_SetAutoAnmSpeed( work->cellCursor, FX32_ONE );
    GFL_CLACT_WK_SetAutoAnmFlag( work->cellCursor, TRUE );
    GFL_CLACT_WK_SetBgPri( work->cellCursor, 2 );

    //タッチバーの「×」
    cellInitData.pos_x = 224;
    cellInitData.pos_y = 168;
    work->cellEndButton = GFL_CLACT_WK_Create( work->cellUnit ,
                          work->objRes[TOUCHBAR_CHR],
                          work->objRes[TOUCHBAR_PLT],
                          work->objRes[TOUCHBAR_CEL],
                          &cellInitData , 
                          FIELD_MENU_RENDER_SURFACE , 
                          work->heapId );
    GFL_CLACT_WK_SetAutoAnmSpeed( work->cellEndButton, FX32_ONE );
    GFL_CLACT_WK_SetAutoAnmFlag( work->cellEndButton, TRUE );
    GFL_CLACT_WK_SetDrawEnable( work->cellEndButton, TRUE );
    GFL_CLACT_WK_SetAutoAnmSpeed( work->cellEndButton, FX32_ONE*2 );
    
  }
  
}

//=============================================================================================
/**
 * @brief INFOBARのセットアップ後にBG0面のスクリーンを操作する
 *
 * @param   heap    
 */
//=============================================================================================
void FIELDMENU_RewriteInfoScreen( HEAPID heapId, BOOL isScroll )
{
  // タッチバーのBGを背景面に転送
  _trans_touchbar_screen( heapId, GFL_BG_FRAME0_S );

  // インフォバー・タッチバーの面をOFF
  if(isScroll){
    GFL_BG_SetVisible( FIELD_MENU_INFOBAR, VISIBLE_OFF );
  }

}


//----------------------------------------------------------------------------------
/**
 * @brief 現在のメニュータイプを取得する
 *
 * @param   gamedata  GAMEDATA
 * @param   ev        EVENTWORK
 * @param   zoneId    ゾーンID
 *
 * @retval  int     FIELD_MENU_NORMAL〜FIELD_MENU_PLEASURE_BOAT( field_menu.h )
 */
//----------------------------------------------------------------------------------
int FIELDMENU_GetMenuType( GAMEDATA * gamedata, EVENTWORK *ev, int zoneId )
{
  int type=FIELD_MENU_NORMAL;   // 通常

  // ユニオンルーム・コロシアム
  if (ZONEDATA_IsUnionRoom(zoneId) 
  ||  ZONEDATA_IsColosseum(zoneId)){
    type = FIELD_MENU_UNION;

  // 遊覧船か？
  }else if(ZONEDATA_IsPlBoat(zoneId)){
    type = FIELD_MENU_PLEASURE_BOAT; 
  
  }else if(GAMEDATA_GetIntrudeReverseArea(gamedata)){
    type = FIELD_MENU_PALACE;

  // ゲーム開始時チェック
  }else if( EVENTWORK_CheckEventFlag(ev,SYS_FLAG_FIRST_POKE_GET) == FALSE ){
    OS_Printf("図鑑なし・ポケモン無し\n");
    type = FIELD_MENU_NO_POKEMON_NO_ZUKAN; // 図鑑無し・ポケモン無し

  }else if(EVENTWORK_CheckEventFlag(ev,SYS_FLAG_ZUKAN_GET) == FALSE ){
    OS_Printf("図鑑なし\n");
    type  = FIELD_MENU_NO_ZUKAN;            // 図鑑無し
  }





#ifdef TEST_FIELD_MENU
  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_L){
    type = FIELD_MENU_NO_ZUKAN;
  }

  if(GFL_UI_KEY_GetCont()&PAD_BUTTON_DEBUG){
    type = FIELD_MENU_NO_POKEMON_NO_ZUKAN;
  }

#endif
  return type;
}



// menuTypeに対応したアイコンの数
//static const u8 menu_type_table[]={
//  6,  // FIELD_MENU_NORMAL=0,            通常メニュー
//  5,  // FIELD_MENU_UNION,               ユニオンルーム
//  5,  // FIELD_MENU_NO_ZUKAN,            図鑑無し
//  4,  // FIELD_MENU_NO_POKEMON_NO_ZUKAN, 図鑑無し・ポケモン無し
//  5,  // FIELD_MENU_PLEASURE_BOAT,       遊覧船内
//};

// --------------------------------------------------
// menuType毎の項目列
// --------------------------------------------------
static const FIELD_MENU_ITEM_TYPE typeArr[FIELD_MENU_TYPE_MAX][7] =
{
  { //通常
    FMIT_POKEMON,
    FMIT_ZUKAN,
    FMIT_ITEMMENU,
    FMIT_TRAINERCARD,
    FMIT_REPORT,
    FMIT_CONFING,
    FMIT_EXIT,
  },
  { //ユニオン
    FMIT_POKEMON,
    FMIT_ZUKAN,
    FMIT_ITEMMENU,
    FMIT_TRAINERCARD,
    FMIT_CONFING,
    FMIT_NONE,
    FMIT_EXIT,
  },
  { //図鑑無し
    FMIT_POKEMON,
    FMIT_ITEMMENU,
    FMIT_TRAINERCARD,
    FMIT_REPORT,
    FMIT_CONFING,
    FMIT_NONE,
    FMIT_EXIT,
  },
  { //ポケモン無し・図鑑無し
    FMIT_ITEMMENU,
    FMIT_TRAINERCARD,
    FMIT_REPORT,
    FMIT_CONFING,
    FMIT_NONE,
    FMIT_NONE,
    FMIT_EXIT,
  },
  { //遊覧船
    FMIT_POKEMON,
    FMIT_ZUKAN,
    FMIT_ITEMMENU,
    FMIT_TRAINERCARD,
    FMIT_CONFING,
    FMIT_NONE,
    FMIT_EXIT,
  },
  { //パレス
    FMIT_POKEMON,
    FMIT_ZUKAN,
    FMIT_ITEMMENU,
    FMIT_TRAINERCARD,
    FMIT_CONFING,
    FMIT_NONE,
    FMIT_EXIT,
  },
};

static int _get_menu_num(int menutype)
{
  int n=0,count=0;


  while( typeArr[menutype][n]!=FMIT_EXIT ){
    if(typeArr[menutype][n]!=FMIT_NONE){
      count++;
    }
    n++;
  }
  return count;
}


//----------------------------------------------------------------------------------
/**
 * @brief メニューアイコン配置初期化
 *
 * @param   work    
 * @param   arcHandle   
 * @param   menuType    
 */
//----------------------------------------------------------------------------------
static void FIELD_MENU_InitIcon(  FIELD_MENU_WORK* work , ARCHANDLE *arcHandle, int menuType )
{
  u8 i;
  //BmpWinの位置(下に半キャラずらして使う
  static const u8 bmpState[7][2] =
  {
    {  6, 4 },
    { 22, 4 },
    {  6,10 },
    { 22,10 },
    {  6,16 },
    { 22,16 },
    { 13,21 },
  };

  GFL_MSGDATA *msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , 
                                           NARC_message_fldmapmenu_dat , work->tempHeapId );
  //文字列の作成
  for( i=0;i<FIELD_MENU_ITEM_NUM;i++ )
  {
    work->icon[i].isInit = FALSE;
  }
  for( i=0;i<FIELD_MENU_ITEM_NUM;i++ )
  {
    const FIELD_MENU_ITEM_TYPE itemType = typeArr[menuType][i];
    FIELD_MENU_ICON_INIT initWork;

    //表示名
    if( itemType == FMIT_TRAINERCARD )
    {
      //トレーナーカード
      initWork.str = GFL_STR_CreateBuffer( 16 , work->tempHeapId );
      if( MyStatus_CheckNameClear( work->my ) == FALSE )
      {
        const STRCODE *myname = MyStatus_GetMyName( work->my );
        GFL_STR_SetStringCode( initWork.str , myname );
      }
      else
      {
        //念のため名前が入ってないときに落ちないようにしておく
        u16 tempName[7] = { L'N',L'o',L'N',L'a',L'm',L'e',0xFFFF };
        GFL_STR_SetStringCode( initWork.str , tempName );
      }
    }
    else
    {
      initWork.str = GFL_MSG_CreateString( msgHandle , FIELD_MENU_ITEM_MSG_ARR[itemType] );
    }

    initWork.type = itemType;
    if( i != 6 )
    {
      //通常
      initWork.cursorPosX = 64 + (i%2)*128;
      initWork.cursorPosY = 44 + (i/2)*48;
      initWork.iconPosX   = initWork.cursorPosX - 32;
      initWork.iconPosY   = initWork.cursorPosY;
      initWork.arcHandle  = arcHandle;
      initWork.ncgArcIdx  = FIELD_MENU_ITEM_ICON_RES_ARR[itemType][0];
      initWork.nceArcIdx  = FIELD_MENU_ITEM_ICON_RES_ARR[itemType][1];
      initWork.anmArcIdx  = FIELD_MENU_ITEM_ICON_RES_ARR[itemType][2];
      
      // バッグで女の子の時はリソースを差し替えて登録
      if(itemType==FMIT_ITEMMENU && MyStatus_GetMySex( work->my )==1){
        initWork.ncgArcIdx  = FIELD_MENU_ITEM_ICON_RES_ARR[GIRLS_BAG_RES][0];
        initWork.nceArcIdx  = FIELD_MENU_ITEM_ICON_RES_ARR[GIRLS_BAG_RES][1];
        initWork.anmArcIdx  = FIELD_MENU_ITEM_ICON_RES_ARR[GIRLS_BAG_RES][2];
      }
      // バッグで女の子の時はリソースを差し替えて登録
      if(itemType==FMIT_ZUKAN && MyStatus_GetMySex( work->my )==1){
        initWork.ncgArcIdx  = FIELD_MENU_ITEM_ICON_RES_ARR[GIRLS_ZUKAN_RES][0];
        initWork.nceArcIdx  = FIELD_MENU_ITEM_ICON_RES_ARR[GIRLS_ZUKAN_RES][1];
        initWork.anmArcIdx  = FIELD_MENU_ITEM_ICON_RES_ARR[GIRLS_ZUKAN_RES][2];
      }
    }
    else
    {
      //閉じる
      initWork.cursorPosX = 128;
      initWork.cursorPosY = 180;
      initWork.iconPosX = 0;
      initWork.iconPosY = 0;
      initWork.arcHandle = NULL;
      initWork.ncgArcIdx = 0;
      initWork.nceArcIdx = 0;
      initWork.anmArcIdx = 0;
    }
    initWork.strBmpPosX = bmpState[i][0];
    initWork.strBmpPosY = bmpState[i][1];

    
    FIELD_MENU_Icon_CreateIcon( work , &work->icon[i] , &initWork );
    GFL_STR_DeleteBuffer( initWork.str );

  }
  
  GFL_MSG_Delete( msgHandle );

}

// カーソルをパレットフェードさせるためのテーブル
static const int paltbl[2][2][3]={
  {
    {9,31,18},  { 17,31,23 },
  },
  {
    {0,25,18},  { 31,31,31 },
  }
};

#define PALETTE_BLINK_SPEED  ( 6 )
#define PALPOS_10_1          ( 10*32+1*2 )   
#define PALPOS_10_15         ( 10*32+15*2 )   

//---------------------------------------------------------------------------------
/**
 * @brief Vblankタスク（カーソルパレットを転送）
 *
 * @param   tcb   
 * @param   userWork    
 */
//----------------------------------------------------------------------------------
static void FIELD_MENU_VBlankTCB( GFL_TCB *tcb , void *userWork )
{
  FIELD_MENU_WORK *work = userWork;
  int n,r,g,b;
  u16 pal1, pal15;

  // カーソルのパレットフェードを転送
  n = GFL_CALC_Sin360R( work->palanm_count ) + 4096;
  r = paltbl[0][0][0] + (paltbl[0][1][0] - paltbl[0][0][0])*n/8192;
  g = paltbl[0][0][1] + (paltbl[0][1][1] - paltbl[0][0][1])*n/8192;
  b = paltbl[0][0][2] + (paltbl[0][1][2] - paltbl[0][0][2])*n/8192;
  pal1 = (g<<10 | b<<5 | r);
  GXS_LoadOBJPltt( &pal1, PALPOS_10_1, 2);    // 10列目1番目

  r = paltbl[1][0][0] + (paltbl[1][1][0] - paltbl[1][0][0])*n/8192;
  g = paltbl[1][0][1] + (paltbl[1][1][1] - paltbl[1][0][1])*n/8192;
  b = paltbl[1][0][2] + (paltbl[1][1][2] - paltbl[1][0][2])*n/8192;
  pal15 = (g<<10 | b<<5 | r);
  GXS_LoadOBJPltt( &pal15, PALPOS_10_15, 2);  // 10列目15番目

  work->palanm_count+=PALETTE_BLINK_SPEED;

//  OS_Printf(" count=%d, n=%d , r=%d, g=%d, b=%d\n ", work->palanm_count, n,r,g,b);

}

static void FIELD_MENU_InitScrollIn( FIELD_MENU_WORK* work )
{
  GFL_CLACTPOS sufacePos;
  work->scrollOffset = 192;
  GFL_BG_SetScroll( FIELD_MENU_BG_BUTTON , GFL_BG_SCROLL_Y_SET , -192 );
  GFL_BG_SetScroll( FIELD_MENU_BG_NAME , GFL_BG_SCROLL_Y_SET , -192 -4);
  
  sufacePos.x = 0;
  sufacePos.y = 192;
  
  GFL_CLACT_USERREND_SetSurfacePos( work->cellRender , FIELD_MENU_RENDER_SURFACE , &sufacePos );
}


//--------------------------------------------------------------
//  決定項目を渡す
//--------------------------------------------------------------
const FIELD_MENU_ITEM_TYPE FIELD_MENU_GetMenuItemNo( FIELD_MENU_WORK* work )
{
  if( work->activeIcon == NULL )
  {
    GF_ASSERT_MSG(0,"FieldMenu ActiveItem is NULL!!\n");
    return FMIT_EXIT;
  }
  else
  {
    return work->activeIcon->type;
  }
}

//--------------------------------------------------------------
//  項目初期位置を貰う
//--------------------------------------------------------------
void FIELD_MENU_SetMenuItemNo( FIELD_MENU_WORK* work , const FIELD_MENU_ITEM_TYPE itemType )
{
  u8 i;
  for( i=0;i<FIELD_MENU_ITEM_NUM;i++ )
  {
    if( work->icon[i].isInit == TRUE )
    {
      if( work->icon[i].type == itemType )
      {
        FIELD_MENU_ICON *tmpIcon;

        // カーソルの初期位置を設定
        work->cursorPosX = i%2;
        work->cursorPosY = i/2;
        work->isUpdateCursor = TRUE;
        
        // カーソルの実座標も設定する
        tmpIcon = &work->icon[work->cursorPosX + work->cursorPosY*2];
        _set_cursor_pos(work, tmpIcon->posX, tmpIcon->posY);
      }
    }
  }
}


static int  _get_nowcursor_pos( int x, int y );
static BOOL _move_cursor( FIELD_MENU_WORK* work, int move );

enum{
  MOVE_UP=0,
  MOVE_DOWN,
  MOVE_LEFT,
  MOVE_RIGHT,
};

//----------------------------------------------------------------------------------
/**
 * @brief XYからカーソル位置を算出
 *
 * @param   x   cursorPosX
 * @param   y   cursorPosY
 *
 * @retval  int   メニュー番号(下のコメント参照）
 */
//----------------------------------------------------------------------------------
static int _get_nowcursor_pos( int x, int y )
{
  return y*2+x;
}

// 0     1
// 2     3
// 4     5
//   6(7)


// カーソル位置をXYに振り分けるテーブル
static const u8 pos2xy_table[][2]={
  {0,0},{1,0},
  {0,1},{1,1},
  {0,2},{1,2},
  {0,3},
};

// 上下左右の順
static const u8 move_table[][8][4]={
 {{0,2,0,1,},{1,3,0,1,},{0,2,2,3,},{1,3,2,3,},{6,6,6,6,},{6,6,6,6},{0xfe,6,6,6,},{0xfe,6,6,6,},},  // 4個
 {{0,2,0,1,},{1,3,0,1,},{0,4,2,3,},{1,3,2,3,},{2,4,4,4,},{6,6,6,6},{   4,6,6,6,},{   4,6,6,6,},},  // 5個
 {{0,2,0,1,},{1,3,0,1,},{0,4,2,3,},{1,5,2,3,},{2,4,4,5,},{3,5,4,5},{0xff,6,6,6,},{0xff,6,6,6,},},  // 6個
};

//----------------------------------------------------------------------------------
/**
 * @brief 移動方向を渡すと実際に移動できるメニューの場所にカーソル位置を変更する
 *
 * @param   work  FIELD_MENU_WORK
 * @param   mx    移動方向X
 * @param   my    移動方向Y
 *
 * @retval  BOOL  移動した:TRUE  してない:FALSE
 */
//----------------------------------------------------------------------------------
static BOOL _move_cursor( FIELD_MENU_WORK* work, int move )
{
  int now = _get_nowcursor_pos(work->cursorPosX, work->cursorPosY);
  int ret,menunum;
  int i;

  if(now<0 || now>7){
    GF_ASSERT_MSG(0,"カーソルが範囲外 (%d,%d)", work->cursorPosX, work->cursorPosY);
  }

  // タイプからメニューの個数を取得
  menunum = _get_menu_num( work->menuType )-4;
  ret = move_table[menunum][now][move];

  
  if(now!=ret){     // 特殊移動1(0xffの場合はY座標的に1段上に移動する）
    if(ret==0xff){
      work->cursorPosY = 2;
      return TRUE;
    }
    else 
    if(ret==0xfe){  // 特殊移動2(0xfeの場合はY座標的に2段上に移動する）
      work->cursorPosY = 1;
      return TRUE;
    }
    // 「閉じる」はX座標を指定しない（戻り位置を保存するため）
    if(ret!=6){
      work->cursorPosX = pos2xy_table[ret][0];
    }
    work->cursorPosY = pos2xy_table[ret][1];
    return TRUE;
  }

  // 移動してない
  return FALSE;
}


#pragma mark [> UI Func
static void  FIELD_MENU_UpdateKey( FIELD_MENU_WORK* work )
{
  const int trg    = GFL_UI_KEY_GetTrg();
  const int repeat = GFL_UI_KEY_GetRepeat();

  // カーソル非表示モードの場合はキーを入れると表示ONになる
  if( work->isDispCursor == FALSE &&
      (trg|repeat) != 0 )
  {
    // メニュー終了
    if( trg & PAD_BUTTON_B || 
        trg & PAD_BUTTON_X )
    {
      _cancel_func_set( work, TRUE );
    }
    else
    { // 隠れていたカーソル表示
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
      work->isDispCursor   = TRUE;
      work->isUpdateCursor = TRUE;
      work->activeIcon = NULL;
      GFL_CLACT_WK_SetDrawEnable( work->cellCursor, TRUE );
    }

    return;
  }
  
  // 上下左右移動
  if( repeat & PAD_KEY_UP )
  {
    if(_move_cursor(work, MOVE_UP))
    {
      work->isUpdateCursor = TRUE;
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
    }
  }
  else
  if( repeat & PAD_KEY_DOWN )
  {
    if(_move_cursor(work, MOVE_DOWN))
    {
      work->isUpdateCursor = TRUE;
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
    }
  }
  else
  if( repeat & PAD_KEY_LEFT )
  {
    if(_move_cursor(work, MOVE_LEFT))
    {
      work->isUpdateCursor = TRUE;
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
    }
  }
  else
  if( repeat & PAD_KEY_RIGHT )
  {
    if(_move_cursor(work, MOVE_RIGHT))
    {
      work->isUpdateCursor = TRUE;
      PMSND_PlaySystemSE( SEQ_SE_SELECT1 );
    }
  }
  else
  if( trg & PAD_BUTTON_B || 
      trg & PAD_BUTTON_X )
  {
    _cancel_func_set( work, TRUE );
  }
  else
  if( trg & PAD_BUTTON_A )
  {
    // 項目無しで無いなら実行
    if(work->icon[_get_nowcursor_pos(work->cursorPosX,work->cursorPosY)].type!=FMIT_NONE){
      _cancel_func_set( work, FALSE );
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    }
  }
}


#define MENU_BUTTON_END   ( 6 )
static void  FIELD_MENU_UpdateTP( FIELD_MENU_WORK* work )
{
  // 1  2
  // 3  4
  // 5  6
  //    7
  const GFL_UI_TP_HITTBL hitTbl[FIELD_MENU_ITEM_NUM+1] =
  {
    { 24, 64 ,  8,120},   // 
    { 24, 64 ,136,248},   // 
    { 72,112 ,  8,120},   // 
    { 72,112 ,136,248},   // 
    {120,160 ,  8,120},   // 
    {120,160 ,136,248},   // 
    {168,191, 224,248},   // 「×」ボタン(MENU_BUTTON_END)   
    {GFL_UI_TP_HIT_END,0,0,0},
  };
  
  const int ret = GFL_UI_TP_HitTrg( hitTbl );
  
  if( ret != GFL_UI_TP_HIT_NONE && work->icon[ret].type!=FMIT_NONE)
  {
    work->cursorPosX = ret%2;
    work->cursorPosY = ret/2;
//    work->isUpdateCursor = TRUE;
    work->isDispCursor = FALSE;
    GFL_CLACT_WK_SetDrawEnable( work->cellCursor, FALSE );

    // 「×」の時はアニメさせる
    if(ret==MENU_BUTTON_END){
      _cancel_func_set( work, TRUE );
    }else{
      work->isUpdateCursor = TRUE;
      _cancel_func_set( work, FALSE );
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
    }
  }
}

//----------------------------------------------------------------------------------
/**
 * @brief カーソル座標セット
 *
 * @param   work    
 */
//----------------------------------------------------------------------------------
static void _set_cursor_pos( FIELD_MENU_WORK *work, int x ,int y )
{
  GFL_CLACTPOS pos;
  pos.x = x;
  pos.y = y;
  GFL_CLACT_WK_SetPos( work->cellCursor , &pos , FIELD_MENU_RENDER_SURFACE );
  work->palanm_count = 0;
}

//----------------------------------------------------------------------------------
/**
 * @brief キャンセル操作の挙動を設定
 *
 * @param   work    
 * @param   isCancel    
 */
//----------------------------------------------------------------------------------
static void _cancel_func_set( FIELD_MENU_WORK *work, BOOL isCancel )
{
  if(isCancel==TRUE){
    work->isCancel = TRUE;
    work->state = FMS_EXIT_ANIME_WAIT;
    GFL_CLACT_WK_SetAnmSeq( work->cellEndButton, 8 );
  }else{
    work->isCancel = FALSE;
    work->state    = FMS_EXIT_INIT;
    work->funcType = work->icon[_get_nowcursor_pos(work->cursorPosX,work->cursorPosY)].type;
  }
}


//----------------------------------------------------------------------------------
/**
 * @brief カーソル更新処理
 *
 * @param   work    
 */
//----------------------------------------------------------------------------------
static void  FIELD_MENU_UpdateCursor( FIELD_MENU_WORK* work )
{

  // カーソル更新があった
  if( work->isUpdateCursor == TRUE )
  {
    FIELD_MENU_ICON *prevIcon = work->activeIcon;

    work->activeIcon = &work->icon[work->cursorPosX + work->cursorPosY*2];
    GFL_CLACT_WK_SetAnmSeq( work->cellCursor , FCA_NORMAL );
    
    // カーソル位置をセット
    _set_cursor_pos(work, work->activeIcon->posX, work->activeIcon->posY);
    if( work->activeIcon->cellIcon != NULL &&
        work->isDispCursor == TRUE )
    {
      GFL_CLACT_WK_SetAnmSeq( work->activeIcon->cellIcon , FIA_ACTIVE );
    }
    //前回のアイコンのアニメを戻す
    if( prevIcon != NULL &&
        prevIcon->cellIcon != NULL )
    {
      GFL_CLACT_WK_SetAnmSeq( prevIcon->cellIcon , FIA_NORMAL );
    }
    
    work->isUpdateCursor = FALSE;
  }
}

#pragma mark [> IconFunc

static void FIELD_MENU_Icon_CreateIcon( FIELD_MENU_WORK* work , 
                                   FIELD_MENU_ICON *icon , 
                                   const FIELD_MENU_ICON_INIT *initWork )
{
  FLDMSGBG *msgBG = FIELDMAP_GetFldMsgBG( work->fieldWork );
  GFL_FONT *fontHandle = FLDMSGBG_GetFontHandle( msgBG );

  icon->isInit = TRUE;
  icon->type = initWork->type;
  icon->posX = initWork->cursorPosX;
  icon->posY = initWork->cursorPosY;
  icon->strBmp = GFL_BMPWIN_Create( FIELD_MENU_BG_NAME , 
                        initWork->strBmpPosX , initWork->strBmpPosY , 8 , 2, 
                        FIELD_MENU_PLT_FONT , GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_MakeScreen( icon->strBmp );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(icon->strBmp), 0x00 );
  
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( icon->strBmp ) , 2 , 2 ,
                          initWork->str , fontHandle , PRINTSYS_LSB_Make( 1,2,0 ) );
  if( initWork->arcHandle != NULL )
  {
    GFL_CLWK_AFFINEDATA cellInitData;
    icon->objResChr = GFL_CLGRP_CGR_Register( initWork->arcHandle , initWork->ncgArcIdx ,
                           FALSE , CLSYS_DRAW_SUB , work->heapId );
    
    icon->objResAnm = GFL_CLGRP_CELLANIM_Register( initWork->arcHandle , 
                           initWork->nceArcIdx , initWork->anmArcIdx ,work->heapId );
    //セルの生成  //倍角を使っているのでAffineで
    cellInitData.clwkdata.pos_x = initWork->iconPosX;
    cellInitData.clwkdata.pos_y = initWork->iconPosY;
    cellInitData.clwkdata.anmseq = 0;
    cellInitData.clwkdata.softpri = 0;
    cellInitData.clwkdata.bgpri = 0;
    cellInitData.affinepos_x = 0;
    cellInitData.affinepos_y = 0;
    cellInitData.scale_x = FX32_ONE;
    cellInitData.scale_y = FX32_ONE;
    cellInitData.rotation = 0;
    cellInitData.affine_type = CLSYS_AFFINETYPE_DOUBLE;
    icon->cellIcon = GFL_CLACT_WK_CreateAffine( work->cellUnit ,
                          icon->objResChr,
                          work->objRes[FMO_COM_PLT],
                          icon->objResAnm,
                          &cellInitData , 
                          FIELD_MENU_RENDER_SURFACE , 
                          work->heapId );
    GFL_CLACT_WK_SetAutoAnmSpeed( icon->cellIcon, FX32_ONE );
    GFL_CLACT_WK_SetAutoAnmFlag( icon->cellIcon, TRUE );
    GFL_CLACT_WK_SetAnmSeq( icon->cellIcon , 0 );

    if(icon->type==FMIT_NONE){
      GFL_CLACT_WK_SetDrawEnable( icon->cellIcon, FALSE );
    }else{
      GFL_CLACT_WK_SetDrawEnable( icon->cellIcon, TRUE );
    }
  }
  else
  {
    icon->cellIcon = NULL;
  }
}

static void FIELD_MENU_Icon_DeleteIcon( FIELD_MENU_WORK* work , FIELD_MENU_ICON *icon )
{
  if( icon->isInit == TRUE )
  {
    GFL_BMPWIN_Delete( icon->strBmp );
    if( icon->cellIcon != NULL )
    {
      GFL_CLACT_WK_Remove( icon->cellIcon );
      GFL_CLGRP_CELLANIM_Release(icon->objResAnm);
      GFL_CLGRP_CGR_Release(icon->objResChr);
    }
  }
}

static void FIELD_MENU_Icon_TransBmp( FIELD_MENU_WORK* work , FIELD_MENU_ICON *icon )
{
  if( icon->isInit == TRUE )
  {
    GFL_BMPWIN_TransVramCharacter( icon->strBmp );
  }
}


#define FIELD_MENU_BG_OFFSET  ( INFOWIN_CHARAREA_SIZE/32 )   // menu_bg.ncgのキャラ数
#define TOUCHBAR_PAL_OFFSET   ( 0x0003 )
#define SCREEN_PAL_OFFSET     ( TOUCHBAR_PAL_OFFSET<<12 )

//----------------------------------------------------------------------------------
/**
 * @brief タッチバーのBGスクリーンを背景面に転送
 *
 * @param   work    
 * @param   bg    
 */
//----------------------------------------------------------------------------------
static void _trans_touchbar_screen( HEAPID heapId, int bgfrm )
{
  //  FIELD_MENU_BG_BACK
  void *buf;
  NNSG2dScreenData* screen;
  ARCHANDLE *handle = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapId );

  // パレット
  GFL_ARCHDL_UTIL_TransVramPalette( handle,  APP_COMMON_GetBarPltArcIdx(), 
                                    PALTYPE_SUB_BG, TOUCHBAR_PAL_OFFSET*16*2, 
                                    32, heapId );
  // キャラ
  GFL_ARCHDL_UTIL_TransVramBgCharacter( handle, APP_COMMON_GetBarCharArcIdx(), 
                                        bgfrm, FIELD_MENU_BG_OFFSET, 0, 0, heapId );
  // スクリーン読み込み
  buf = GFL_ARCHDL_UTIL_LoadScreen( handle, APP_COMMON_GetBarScrnArcIdx(),
                                    0, &screen, heapId );

  GFL_ARC_CloseDataHandle( handle );


  // スクリーン加工
  {
    int i;
    u16 *scr = (u16*)screen->rawData;
    for(i=0;i<32*24;i++){
      scr[i] = (scr[i]+FIELD_MENU_BG_OFFSET) | SCREEN_PAL_OFFSET;
    }
  }

  // スクリーン書き込み＆転送
  GFL_BG_WriteScreenExpand( bgfrm,           0, 21, 32,  3,
                            screen->rawData, 0, 21, 32, 24 );
  GFL_BG_LoadScreenV_Req( bgfrm );

  GFL_HEAP_FreeMemory( buf );
  
}
