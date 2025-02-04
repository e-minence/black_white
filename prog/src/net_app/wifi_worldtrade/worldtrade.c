//============================================================================================
/**
 * @file  worldtrade.c
 * @brief 世界交換メイン処理
 * @author  Akito Mori -> toru_nagihashi WBへ移植
 * @date  06.04.16  -> 09.08.04
 */
//============================================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "arc_def.h"
#include <dwc.h>
#include <dpw_tr.h>
#include "print/wordset.h"
#include "message.naix"
#include "msg/msg_wifi_lobby.h"
#include "system/wipe.h"
#include "gamesystem/msgspeed.h"
#include "msg/msg_wifi_gts.h"
#include "print/printsys.h"
#include "msg/msg_wifi_system.h"


#include "savedata/wifilist.h"

#include "poke_tool/monsno_def.h"
#include "system/bmp_menu.h"
#include "net_app/worldtrade.h"
#include "sound/pm_sndsys.h"
#include "savedata/save_control.h"

#include "net/dwc_raputil.h"

#include "worldtrade_local.h"

#include "net_app/connect_anm.h"
#include "net/network_define.h"

//#include "msgdata/msg_ev_win.h"

#include "debug/debug_nagihashi.h"

#define WORLDTRADE_WORDSET_BUFLEN ( 64 )  ///< WORDSet文字列の展開用バッファ長

#include "worldtrade.naix"          ///< グラフィックアーカイブ定義
#include "arc/wifileadingchar.naix"
#include "poke_icon.naix"

#include "debug/debug_nagihashi.h"
#include "sound/pm_sndsys.h"


#define BRIGHT_VAL  (-7)          ///< パッシブ状態のための半透明率

FS_EXTERN_OVERLAY( dpw_common );
FS_EXTERN_OVERLAY( ui_common );
//FS_EXTERN_OVERLAY( app_mail );

//============================================================================================
//  定数定義
//============================================================================================
// ポインタ参照だけできる世界交換ワーク構造体
//typedef struct _WORLDTRADE_WORK WORLDTRADE_WORK;


//============================================================================================
//  プロトタイプ宣言
//============================================================================================
// プロセス定義データ
static GFL_PROC_RESULT WorldTradeProc_Init( GFL_PROC * proc, int * seq, void * param, void * work );
static GFL_PROC_RESULT WorldTradeProc_Main( GFL_PROC * proc, int * seq, void * param, void * work );
static GFL_PROC_RESULT WorldTradeProc_End( GFL_PROC * proc, int * seq, void * param, void * work );


/*** 関数プロトタイプ ***/
static void VBlankFunc( GFL_TCB *, void *work );
static void VramBankSet( const GFL_DISP_VRAM * vram );
static void InitWork( WORLDTRADE_WORK *wk, WORLDTRADE_PARAM *param );
static void FreeWork( WORLDTRADE_WORK *wk );
static void InitCellActor(WORLDTRADE_WORK *wk, const GFL_DISP_VRAM * vram );
static void SetCellActor(WORLDTRADE_WORK *wk);
static void InitCLACT( WORLDTRADE_WORK *wk );
static void FreeCLACT( WORLDTRADE_WORK *wk );
static void ServerWaitTimeFunc( WORLDTRADE_WORK *wk );
static void BoxPokeNumGet( WORLDTRADE_WORK *wk );
static void WorldTrade_WndSetting(void);




//============================================================================================
//  関数テーブル定義
//============================================================================================

// 世界交換サブプロセス用関数配列定義
static int (*SubProcessTable[][3])(WORLDTRADE_WORK *wk, int seq)={
  { WorldTrade_Enter_Init,   WorldTrade_Enter_Main,    WorldTrade_Enter_End   },
  { WorldTrade_Title_Init,   WorldTrade_Title_Main,    WorldTrade_Title_End   },
  { WorldTrade_MyPoke_Init,  WorldTrade_MyPoke_Main,   WorldTrade_MyPoke_End  },
  { WorldTrade_Partner_Init, WorldTrade_Partner_Main,  WorldTrade_Partner_End },
  { WorldTrade_Search_Init,  WorldTrade_Search_Main,   WorldTrade_Search_End  },
  { WorldTrade_Box_Init,     WorldTrade_Box_Main,      WorldTrade_Box_End     },
  { WorldTrade_Deposit_Init, WorldTrade_Deposit_Main,  WorldTrade_Deposit_End },
  { WorldTrade_Upload_Init,  WorldTrade_Upload_Main,   WorldTrade_Upload_End  },
  { WorldTrade_Status_Init,  WorldTrade_Status_Main,   WorldTrade_Status_End  },
  { WorldTrade_Demo_Init,    WorldTrade_Demo_Main,     WorldTrade_Demo_End  },
};


WORLDTRADE_WORK *debug_worldtrade;

//============================================================================================
//  プロセス関数
//============================================================================================
const GFL_PROC_DATA WorldTrade_ProcData =
{
  WorldTradeProc_Init,
  WorldTradeProc_Main,
  WorldTradeProc_End,
};


//--------------------------------------------------------------------------------------------
/**
 * @brief   プロセス関数：初期化
 *
 * @param proc  プロセスデータ
 * @param seq   シーケンス
 *
 * @return  処理状況
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT WorldTradeProc_Init( GFL_PROC * proc, int * seq, void * param, void * work )
{
  WORLDTRADE_WORK * wk;

  GFL_OVERLAY_Load( FS_OVERLAY_ID( dpw_common ) );
  //以下デモで使うので呼ぶ
  GFL_OVERLAY_Load( FS_OVERLAY_ID( ui_common ));
//  GFL_OVERLAY_Load( FS_OVERLAY_ID( app_mail ));
#if PM_DEBUG
  GFL_NET_DebugPrintOn();
#endif

    // ヒープ作成
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WORLDTRADE, 0x30000 );  //old 0x70000

    wk = GFL_PROC_AllocWork( proc, sizeof(WORLDTRADE_WORK), HEAPID_WORLDTRADE );
    GFL_STD_MemClear( wk, sizeof(WORLDTRADE_WORK) );

    debug_worldtrade = wk;

    wk->evilcheck_dummy_poke = PokemonParam_AllocWork( HEAPID_WORLDTRADE );

    //local_proc
    wk->local_proc  = GFL_PROC_LOCAL_boot(HEAPID_WORLDTRADE);

    //task
    wk->task_wk_area  = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, GFL_TCB_CalcSystemWorkSize( 8 ) );
    wk->tcbsys        = GFL_TCB_Init( 8, wk->task_wk_area );

    // タッチフォントロード
//    FontProc_LoadFont( FONT_TOUCH, HEAPID_WORLDTRADE );
    {
      WORLDTRADE_PARAM *wt_param  = param;

      WT_PRINT_Init( &wk->print, wt_param->config );
    }
    OS_Printf("-------------------------------プロック初期化\n");

    // 文字列マネージャー生成
    wk->WordSet        = WORDSET_CreateEx( 11, WORLDTRADE_WORDSET_BUFLEN, HEAPID_WORLDTRADE );
    wk->MsgManager       = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_gts_dat, HEAPID_WORLDTRADE );
    wk->LobbyMsgManager  = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_lobby_dat, HEAPID_WORLDTRADE );
    wk->SystemMsgManager = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_system_dat, HEAPID_WORLDTRADE );
    wk->MonsNameManager  = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_monsname_dat, HEAPID_WORLDTRADE );
    wk->CountryNameManager  = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wifi_place_msg_world_dat, HEAPID_WORLDTRADE );


    // ワーク初期化
    InitWork( wk, param );

    //VRAMクリアー
    GFL_DISP_ClearVRAM( 0 );

    WorldTrade_InitSystem( wk );

    //WirelessIconEasy();

    // 会話ウインドウのタッチON
    MsgPrintTouchPanelFlagSet( MSG_TP_ON );

    //フィールドサウンドをPUSH
    PMSND_PauseBGM(FALSE);
    PMSND_StopBGM();

    return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    プロセス関数：メイン
 *
 * @param proc  プロセスデータ
 * @param seq   シーケンス
 *
 * @return  処理状況
 */
//--------------------------------------------------------------------------------------------

static GFL_PROC_RESULT WorldTradeProc_Main( GFL_PROC * proc, int * seq, void * param, void * work )
{
  WORLDTRADE_WORK * wk  = work;
  if( GFL_NET_IsInit() )
  {
    // 受信強度リンクを反映させる
    DWC_UpdateConnection();
    // 通信状態を確認してアイコンの表示を変える
    WirelessIconEasy_SetLevel(WorldTrade_WifiLinkLevel());
  }
  Dpw_Tr_Main();

  wk->local_proc_status = GFL_PROC_LOCAL_Main( wk->local_proc );
  GFL_TCB_Main( wk->tcbsys );
  WT_PRINT_Main( &wk->print );

  switch( *seq ){
  // サブ処理初期化
  case SEQ_INIT:
    *seq = (*SubProcessTable[wk->sub_process][0])(wk, *seq);
    WorldTrade_WndSetting();
    if(wk->subprocflag){
      //FreeCLACT( wk );
      OS_Printf("OAMシステム臨時解放");
    }
    break;

  // サブ処理フェードイン待ち
  case SEQ_FADEIN:
    if( WIPE_SYS_EndCheck() ){
      *seq = SEQ_MAIN;
    }
    break;

  // サブ処理メイン
  case SEQ_MAIN:
    *seq = (*SubProcessTable[wk->sub_process][1])(wk, *seq);

    break;

  // サブ処理フェードアウト待ち
  case SEQ_FADEOUT:
    if( WIPE_SYS_EndCheck() ){
      // サブ処理解放(SEQ_INITに行くか、SEQ_OUTにいくかはおまかせ）
      *seq = (*SubProcessTable[wk->sub_process][2])(wk, *seq);
      if(wk->subprocflag){
        WorldTrade_SubLcdActorAdd( wk );
        WorldTrade_SubLcdMatchObjAppear( wk, wk->SearchResult, 0 );
        wk->subprocflag = 0;
        OS_Printf("OAMシステム臨時復帰");
      }
    }
    break;

  // 世界交換終了処理
  case SEQ_OUT:
    *seq  = SEQ_EXIT;
    break;

  case SEQ_EXIT:
    return GFL_PROC_RES_FINISH;
  }
  ServerWaitTimeFunc( wk );
  //BoxPokeNumGet( wk );

  if(wk->clactSet!=NULL){
    GFL_CLACT_SYS_Main();
  }

  if( wk->search_se_sync > 0 )
  { 
    wk->search_se_sync--;
  }

  GF_ASSERT( GFL_HEAP_CheckHeapSafe(HEAPID_WORLDTRADE) );

  return GFL_PROC_RES_CONTINUE;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief    プロセス関数：終了
 *
 * @param proc  プロセスデータ
 * @param seq   シーケンス
 *
 * @return  処理状況
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT WorldTradeProc_End( GFL_PROC * proc, int * seq, void * param, void * work )
{
  WORLDTRADE_WORK  *wk    = work;
  int i;
  GF_ASSERT( GFL_HEAP_CheckHeapSafe(HEAPID_WORLDTRADE) );

  //フィールドサウンド復帰
  PMSND_StopBGM();

  WirelessIconEasyEnd();
  WorldTrade_ExitSystem( wk );
  FreeWork( wk );

  // メッセージマネージャー・ワードセットマネージャー解放
  GFL_MSG_Delete( wk->MonsNameManager );
  GFL_MSG_Delete( wk->SystemMsgManager );
  GFL_MSG_Delete( wk->LobbyMsgManager );
  GFL_MSG_Delete( wk->MsgManager );
  GFL_MSG_Delete( wk->CountryNameManager );
  WORDSET_Delete( wk->WordSet );

  // メッセージウインドウのタッチOFF
  MsgPrintTouchPanelFlagSet( MSG_TP_OFF );
  GFL_TCB_Exit( wk->tcbsys );
  GFL_HEAP_FreeMemory( wk->task_wk_area );
  GFL_PROC_LOCAL_Exit( wk->local_proc );

  GFL_HEAP_FreeMemory(wk->evilcheck_dummy_poke);
  // タッチフォントアンロード
  WT_PRINT_Exit( &wk->print );
  GFL_PROC_FreeWork( proc );        // GFL_PROCワーク開放

  GFL_HEAP_DeleteHeap( HEAPID_WORLDTRADE );

//  GFL_OVERLAY_Unload( FS_OVERLAY_ID( app_mail ) );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( ui_common ) );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( dpw_common ) );

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief    VBlank関数
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VBlankFunc( GFL_TCB *, void *work )
{
  WORLDTRADE_WORK *wk = work;

  GFL_BG_VBlankFunc();
  GFL_CLACT_SYS_VBlankFunc();

  // VBlank中転送ファンクション実行
  if(wk->vfunc){
    wk->vfunc(work);
    wk->vfunc = NULL;
  }
  if(wk->vfunc2){
    wk->vfunc2(work);
  }


}


//--------------------------------------------------------------------------------------------
/**
 * @brief    VRAM設定
 *
 * @param none
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void VramBankSet( const GFL_DISP_VRAM * vram )
{

  GFL_DISP_SetBank( vram );

}




//------------------------------------------------------------------
/**
 * @brief    世界交換ワーク初期化
 *
 * @param   wk    WORLDTRADE_WORK*
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void InitWork( WORLDTRADE_WORK *wk, WORLDTRADE_PARAM *param )
{
  GF_ASSERT_HEAVY( param->savedata != NULL );
  GF_ASSERT_HEAVY( param->worldtrade_data != NULL );
  GF_ASSERT_HEAVY( param->systemdata != NULL );
  GF_ASSERT_HEAVY( param->myparty != NULL );
  GF_ASSERT_HEAVY( param->mybox != NULL );
//  GF_ASSERT( param->zukanwork != NULL );
  GF_ASSERT_HEAVY( param->wifilist != NULL );
  GF_ASSERT_HEAVY( param->wifihistory != NULL );
  GF_ASSERT_HEAVY( param->mystatus != NULL );
  GF_ASSERT_HEAVY( param->config != NULL );
  GF_ASSERT_HEAVY( param->record != NULL );
//  GF_ASSERT( param->myitem != NULL );



  // 呼び出し時のパラメータを取得
  wk->param = param;

  wk->sub_process   = WORLDTRADE_ENTER;
  WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, MODE_WIFILOGIN );

  wk->TitleCursorPos  = 0;

  wk->Search.characterNo     = 0;
  wk->Search.gender          = PARA_UNK+1;
  wk->Search.level_min       = 0;
  wk->Search.level_max       = 0;
  wk->SearchBackup.characterNo = 0;
  wk->demo_end               = 0;
  wk->BoxTrayNo          = WORLDTRADE_BOX_TEMOTI;
  wk->boxPokeNum         = 0;
  wk->boxSearchFlag      = 0;
  wk->SubLcdTouchOK      = 0;
  wk->timeWaitWork             = NULL;

  wk->country_code       = 0;
}

//------------------------------------------------------------------
/**
 * @brief   ワーク解放
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void FreeWork( WORLDTRADE_WORK *wk )
{


}

//--------------------------------------------------------------
/**
 * @brief    ウィンドウ設定
 *
 * @param   none
 */
//--------------------------------------------------------------
static void WorldTrade_WndSetting(void)
{
  GXS_SetVisibleWnd(GX_WNDMASK_NONE);
  //受信強度アイコンにフェードがかからないようにウィンドウで囲む
  GX_SetVisibleWnd(GX_WNDMASK_W0);
  G2_SetWnd0InsidePlane(
    GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2
    | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, FALSE);
  G2_SetWndOutsidePlane(
    GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2
    | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE);
  G2_SetWnd0Position(256-16, 0, 255, 16);
}


#define TRANS_POKEICON_COLOR_NUM  ( 3*16 )
//------------------------------------------------------------------
/**
 * @brief    レーダー画面用セルアクター初期化
 *
 * @param   wk    レーダー構造体のポインタ
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void InitCellActor(WORLDTRADE_WORK *wk, const GFL_DISP_VRAM * vram )
{
  int i;
  ARCHANDLE* p_handle;

  p_handle = GFL_ARC_OpenDataHandle( ARCID_WORLDTRADE_GRA, HEAPID_WORLDTRADE );

  // 共有OAMマネージャ作成
  // レンダラ用OAMマネージャ作成
  // ここで作成したOAMマネージャをみんなで共有する
  {
    static const GFL_CLSYS_INIT clsys_init =
    {
      0, 0,
      0, NAMEIN_SUB_ACTOR_DISTANCE,
      4, 124,
      4, 124,
      0,
      32,32,32,32,
      16, 16,
    };

    GFL_CLACT_SYS_Create( &clsys_init, vram, HEAPID_WORLDTRADE );
  }

  // セルアクター初期化
  wk->clactSet = GFL_CLACT_UNIT_Create( 72+24, 0, HEAPID_WORLDTRADE );



  //---------上画面用-------------------
  //chara読み込み
  wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES] = GFL_CLGRP_CGR_Register( p_handle, NARC_worldtrade_worldtrade_obj_lz_ncgr, 1, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE);

  //pal読み込み
  wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES] = GFL_CLGRP_PLTT_Register(p_handle, NARC_worldtrade_worldtrade_obj_nclr,CLSYS_DRAW_MAIN, /*3*0x20*/0, HEAPID_WORLDTRADE);

  //cell読み込み
  wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES] = GFL_CLGRP_CELLANIM_Register( p_handle, NARC_worldtrade_worldtrade_obj_NCER, NARC_worldtrade_worldtrade_obj_NANR, HEAPID_WORLDTRADE);

  //---------上画面カーソル用-------------------
  wk->resObjTbl[RES_CURSOR][CLACT_U_CHAR_RES] = GFL_CLGRP_CGR_Register( p_handle, NARC_worldtrade_worldtrade_obj_c_lz_ncgr, 1, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE);

  //cell読み込み
  wk->resObjTbl[RES_CURSOR][CLACT_U_CELL_RES] = GFL_CLGRP_CELLANIM_Register( p_handle, NARC_worldtrade_worldtrade_obj_c_NCER, NARC_worldtrade_worldtrade_obj_c_NANR, HEAPID_WORLDTRADE);

  //---------下画面用-------------------
  //chara読み込み
  wk->resObjTbl[SUB_LCD][CLACT_U_CHAR_RES] = GFL_CLGRP_CGR_Register( p_handle, NARC_worldtrade_worldtrade_obj_s_lz_ncgr, 1, CLSYS_DRAW_SUB, HEAPID_WORLDTRADE);

  //pal読み込み
  wk->resObjTbl[SUB_LCD][CLACT_U_PLTT_RES] = GFL_CLGRP_PLTT_Register(p_handle, NARC_worldtrade_worldtrade_obj_s_nclr, CLSYS_DRAW_SUB, /*10*0x20*/0, HEAPID_WORLDTRADE);

  //cell読み込み
  wk->resObjTbl[SUB_LCD][CLACT_U_CELL_RES] = GFL_CLGRP_CELLANIM_Register(p_handle, NARC_worldtrade_worldtrade_obj_s_NCER, NARC_worldtrade_worldtrade_obj_s_NANR, HEAPID_WORLDTRADE);

  GFL_ARC_CloseDataHandle( p_handle );

  // ポケモンアイコン用パレットを一気に読み込んでVRAM転送する
  // ポケモンアイコンのパレットを暗くしたデータも作成して転送する
  {
    void              *buf;
    NNSG2dPaletteData *palData;
    int i,r,g,b;
    u16 *pal;

    buf = GFL_ARC_UTIL_LoadPalette( ARCID_POKEICON, NARC_poke_icon_poke_icon_NCLR, &palData, HEAPID_WORLDTRADE );
    DC_FlushRange( palData->pRawData, TRANS_POKEICON_COLOR_NUM*2 );
    GX_LoadOBJPltt( palData->pRawData, POKEICON_PAL_OFFSET*0x20, TRANS_POKEICON_COLOR_NUM*2 );

    pal = (u16*)palData->pRawData;
    for(i=0;i<TRANS_POKEICON_COLOR_NUM;i++){
      r = pal[i]>>10&0x1f;
      g = (pal[i]>>5)&0x1f;
      b = pal[i]&0x1f;

      r /=2;  g /= 2; b /=2;
      pal[i] = (r<<10)|(g<<5)|b;
    }

    DC_FlushRange( palData->pRawData, TRANS_POKEICON_COLOR_NUM*2 );
    GX_LoadOBJPltt( palData->pRawData, (POKEICON_PAL_OFFSET+3)*0x20, TRANS_POKEICON_COLOR_NUM*2 );

    GFL_HEAP_FreeMemory(buf);
  }



  //主人公キャラを読み込み
  { 
    u8 sex  = MyStatus_GetMySex(wk->param->mystatus);
    u32 chr,anm,cel,plt;

    if( sex == PTL_SEX_FEMALE )
    { 
      chr = NARC_wifileadingchar_heroine_gts_NCGR;
      anm = NARC_wifileadingchar_heroine_gts_NANR;
      cel = NARC_wifileadingchar_heroine_gts_NCER;
      plt = NARC_wifileadingchar_heroine_NCLR;
    }
    else
    { 
      chr = NARC_wifileadingchar_hero_gts_NCGR;
      anm = NARC_wifileadingchar_hero_gts_NANR;
      cel = NARC_wifileadingchar_hero_gts_NCER;
      plt = NARC_wifileadingchar_hero_NCLR;
    }

    p_handle = GFL_ARC_OpenDataHandle( ARCID_WIFILEADING, HEAPID_WORLDTRADE );

    //chara読み込み
    wk->resObjTbl[RES_HERO][CLACT_U_CHAR_RES] = GFL_CLGRP_CGR_Register( p_handle, chr, 0, CLSYS_DRAW_SUB, HEAPID_WORLDTRADE);

    //pal読み込み
    wk->resObjTbl[RES_HERO][CLACT_U_PLTT_RES] = GFL_CLGRP_PLTT_RegisterEx(p_handle, plt, CLSYS_DRAW_SUB, 0, 0, 1, HEAPID_WORLDTRADE);

    //cell読み込み
    wk->resObjTbl[RES_HERO][CLACT_U_CELL_RES] = GFL_CLGRP_CELLANIM_Register(p_handle, cel, anm, HEAPID_WORLDTRADE);

    GFL_ARC_CloseDataHandle( p_handle );
  }
}

#define TRAINER_NAME_POS_X    ( 24 )
#define TRAINER_NAME_POS_Y    ( 32 )
#define TRAINER_NAME_POS_SPAN ( 32 )

#define TRAINER_NAME_WIN_X    (  3 )
#define TRAINER1_NAME_WIN_Y   (  6 )
#define TRAINER2_NAME_WIN_Y   (  7 )

static const u16 obj_pos_tbl[][2]={
  {212,64},
  {187,77},
  {236,77},
  {194,120},
  {224,120},
};

//------------------------------------------------------------------
/**
 * @brief   セルアクター登録
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void SetCellActor(WORLDTRADE_WORK *wk)
{
  GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON ); //メイン画面OBJ面ＯＮ
  GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON ); //サブ画面OBJ面ＯＮ
}



// はい・いいえ
#define BMP_YESNO_PX  ( 24 )

#define BMP_YESNO_PY  ( 13 )
#define BMP_YESNO_SX  ( 7 )
#define BMP_YESNO_SY  ( 4 )
#define BMP_YESNO_PAL ( 13 )

//==============================================================================
/**
 * @brief   タッチ対応YESNOウインドウ（メインのみ）
 *
 * @param   bgl       GF_BGL_INI
 * @param   y       Y座標
 * @param   yesno_bmp_cgx VRAMオフセット位置
 * @param   pltt      パレット指定
 * @param inPassive   パッシブするか
 *
 * @retval  TOUCH_SW_WORK *   はい・いいえ画面ワーク
 */
//==============================================================================
void WorldTrade_TouchWinYesNoMake( WORLDTRADE_WORK *wk, int y, int yesno_bmp_cgx, int pltt, u8 inPassive )
{
  WorldTrade_TouchWinYesNoMakeEx( wk, y, yesno_bmp_cgx, pltt, GFL_BG_FRAME0_M, inPassive );
}

//==============================================================================
/**
 * @brief   タッチ対応YESNOウインドウ(フレーム指定版)
 *
 * @param   bgl       GF_BGL_INI
 * @param   y           Y座標
 * @param   yesno_bmp_cgx VRAMオフセット位置
 * @param   pltt        パレット指定
 * @param   frame     表示フレーム指定
 * @param inPassive   パッシブするか
 *
 * @retval  TOUCH_SW_SYS *    はい・いいえ画面ワーク
 */
//==============================================================================
void WorldTrade_TouchWinYesNoMakeEx( WORLDTRADE_WORK *wk, int y, int yesno_bmp_cgx,
                        int pltt, int frame, u8 inPassive )
{
  GF_ASSERT_HEAVY( wk->task_res == NULL );
  GF_ASSERT_HEAVY( wk->task_work == NULL );

  wk->task_res  = APP_TASKMENU_RES_Create( frame, pltt, wk->print.font, wk->print.que, HEAPID_WORLDTRADE );

  { 
    APP_TASKMENU_ITEMWORK itemWork[]  =
    { 
      { 
        NULL,
        APP_TASKMENU_ITEM_MSGCOLOR,
        APP_TASKMENU_WIN_TYPE_NORMAL,
      },
      { 
        NULL,
        APP_TASKMENU_ITEM_MSGCOLOR,
        APP_TASKMENU_WIN_TYPE_NORMAL,
      },
    };

    APP_TASKMENU_INITWORK initWork;
    GFL_STD_MemClear( &initWork, sizeof(APP_TASKMENU_INITWORK) );


    itemWork[0].str = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_02_018 );
    itemWork[1].str = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_02_019 );

    initWork.heapId = HEAPID_WORLDTRADE;
    initWork.itemNum  = 2;
    initWork.itemWork = itemWork;
    initWork.posType  = ATPT_RIGHT_DOWN;
    initWork.charPosX = 32;
    initWork.charPosY = y;
    initWork.w = APP_TASKMENU_PLATE_WIDTH_YN_WIN;
    initWork.h = APP_TASKMENU_PLATE_HEIGHT_YN_WIN;

    wk->task_work = APP_TASKMENU_OpenMenu( &initWork, wk->task_res );

    GFL_STR_DeleteBuffer( itemWork[0].str );
    GFL_STR_DeleteBuffer( itemWork[1].str );
  }

  if (inPassive){
    u8 target = 0;
    if (frame <= GFL_BG_FRAME3_M){
      target = 1;   //対象はメイン画面
    }
    WorldTrade_SetPassive(target);
  }

}

//==============================================================================
/**
 * @brief   タッチ対応YESNOウインドウ（メインのみ）
 *
 * @param   bgl       GF_BGL_INI
 * @param   y       Y座標
 * @param   yesno_bmp_cgx VRAMオフセット位置
 * @param   pltt      パレット指定
 * @param inPassive   パッシブするか
 *
 * @retval  TOUCH_SW_WORK *   はい・いいえ画面ワーク
 */
//==============================================================================
void WorldTrade_TouchDelete( WORLDTRADE_WORK *wk )
{
  if( wk->task_work )
  { 
    APP_TASKMENU_CloseMenu( wk->task_work );
    wk->task_work   = NULL;
  }
  if( wk->task_res )
  { 
    APP_TASKMENU_RES_Delete( wk->task_res );
    wk->task_res  = NULL;
  }
}

//------------------------------------------------------------------
/**
 * @brief    YesNoウィンドウ監視
 *
 * @param   wk      WORLDTRADE_WORK*
 *
 * @retval  none
 */
//------------------------------------------------------------------
u32 WorldTrade_TouchSwMain(WORLDTRADE_WORK* wk)
{
  u32 ret = WORLDTRADE_RET_NORMAL;
  if( wk->task_work )
  { 
    APP_TASKMENU_UpdateMenu( wk->task_work );
    if ( APP_TASKMENU_IsFinish( wk->task_work ) ){

      ret = APP_TASKMENU_GetCursorPos( wk->task_work );
      if( ret == 0 )
      { 
        ret = WORLDTRADE_RET_YES;
      }
      else if( ret == 1 )
      { 
        ret = WORLDTRADE_RET_NO;
      }
      //パッシブ解除
      WorldTrade_ClearPassive();
    }
  }

  return ret;
}


// 選択ボックスの幅
#define WORLDTRADESELBOX_X  ( 32 - (WORLDTRADE_SELBOX_W+2)) // 幅0の時は20
#define WORLDTRADESELBOX_CGX_SIZE ( WORLDTRADE_SELBOX_W*2 )
#define WORLDTRADESEL_MAX (3)
#define WORLDTRADESELBOX_PLT (10)

//==============================================================================
/**
 * @brief   選択ボックスシステム呼び出し
 *
 * @param   wk    GTS画面ワーク
 * @param   count リストの個数
 * @param   y   表示位置Y座標
 *
 * @retval  SELBOX_WORK*    選択ボックスワークのポインタ
 */
//==============================================================================
void WorldTrade_SelBoxInit( WORLDTRADE_WORK *wk, u8 frm, int count, int y )
{
  GF_ASSERT_HEAVY( wk->task_res == NULL );
  GF_ASSERT_HEAVY( wk->task_work == NULL );

  wk->task_res  = APP_TASKMENU_RES_Create( frm, WORLDTRADESELBOX_PLT, wk->print.font, wk->print.que, HEAPID_WORLDTRADE );

  { 
    APP_TASKMENU_ITEMWORK itemWork[]  =
    { 
      { 
        NULL,
        APP_TASKMENU_ITEM_MSGCOLOR,
        APP_TASKMENU_WIN_TYPE_NORMAL,
      },
      { 
        NULL,
        APP_TASKMENU_ITEM_MSGCOLOR,
        APP_TASKMENU_WIN_TYPE_NORMAL,
      },
      { 
        NULL,
        APP_TASKMENU_ITEM_MSGCOLOR,
        APP_TASKMENU_WIN_TYPE_NORMAL,
      },
    };

    int i;
    APP_TASKMENU_INITWORK initWork;
    GFL_STD_MemClear( &initWork, sizeof(APP_TASKMENU_INITWORK) );

    GF_ASSERT_HEAVY( count <= NELEMS( itemWork ) );

    for( i = 0; i < count; i++ )
    { 
      itemWork[i].str = (STRBUF*)wk->BmpMenuList[i].str;
    }

    initWork.heapId = HEAPID_WORLDTRADE;
    initWork.itemNum  = count;
    initWork.itemWork = itemWork;
    initWork.posType  = ATPT_RIGHT_DOWN;
    initWork.charPosX = 32;
    initWork.charPosY = y;
    initWork.w = APP_TASKMENU_PLATE_WIDTH;
    initWork.h = APP_TASKMENU_PLATE_HEIGHT;

    wk->task_work = APP_TASKMENU_OpenMenu( &initWork, wk->task_res );
  }

  //パッシブ
  WorldTrade_SetPassive(1);

}

//----------------------------------------------------------------------------
/**
 *	@brief  選択ボックス処理中
 *
 *	@param	WORLDTRADE_WORK *wk ワーク
 *
 *	@return 決定したもののインデックス
 */
//-----------------------------------------------------------------------------
u32 WorldTrade_SelBoxMain( WORLDTRADE_WORK *wk )
{ 
  u32 ret = BMPMENULIST_NULL;
  if( wk->task_work )
  { 
    APP_TASKMENU_UpdateMenu( wk->task_work );
    if ( APP_TASKMENU_IsFinish( wk->task_work ) ){

      ret = APP_TASKMENU_GetCursorPos( wk->task_work ) + 1;

      //パッシブ解除
      WorldTrade_ClearPassive();
    }
  }

  return ret;
}

//==============================================================================
/**
 * @brief   選択ボックスシステム終了
 *
 * @param   wk    GTS画面ワーク
 * @param   count リストの個数
 * @param   y   表示位置Y座標
 *
 * @retval  SELBOX_WORK*    選択ボックスワークのポインタ
 */
//==============================================================================
void WorldTrade_SelBoxEnd( WORLDTRADE_WORK *wk )
{
  if( wk->task_work )
  { 
    APP_TASKMENU_CloseMenu( wk->task_work );
    wk->task_work   = NULL;
  }
  if( wk->task_res )
  { 
    APP_TASKMENU_RES_Delete( wk->task_res );
    wk->task_res  = NULL;
  }

  //パッシブ解除
  WorldTrade_ClearPassive();
}


//------------------------------------------------------------------
/**
 * @brief   次の次のシーケンスを予約しておく
 *
 * @param   wk      GTS画面ワーク
 * @param   to_seq    次のシーケンス
 * @param   next_seq  次の次のシーケンス
 *
 * @retval  none
 */
//------------------------------------------------------------------
void WorldTrade_SetNextSeq( WORLDTRADE_WORK *wk, int to_seq, int next_seq )
{
  wk->subprocess_seq      = to_seq;
  wk->subprocess_nextseq  = next_seq;
}

//=============================================================================================
/**
 * @brief サブプロセス終了時の次の行き先を入力する
 *
 * @param   wk        GTS画面ワーク
 * @param   next_process  次のシーケンス
 */
//=============================================================================================
void WorldTrade_SetNextProcess( WORLDTRADE_WORK *wk, int next_process )
{
  wk->sub_nextprocess = next_process;
}

//==============================================================================
/**
 * @brief   CLACTの座標変更
 *
 * @param   act   アクターのポインタ
 * @param   x   X座標
 * @param   y   Y座標
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_CLACT_PosChangeSub( GFL_CLWK* act, int x, int y )
{
  GFL_CLACTPOS pos;

  pos.x = x;
  pos.y = y;
  GFL_CLACT_WK_SetPos( act, &pos, CLSYS_DRAW_SUB );

}


//==============================================================================
/**
 * @brief   WIFI接続状況取得
 *
 * @param   none
 *
 * @retval  int   接続強度取得
 */
//==============================================================================
int WorldTrade_WifiLinkLevel( void )
{
    return WM_LINK_LEVEL_3 - DWC_GetLinkLevel();
}




//==============================================================================
/**
 * @brief   サブプロセスモードの切り替え指定
 *
 * @param   wk      GTS画面ワーク
 * @param   subprccess  指定の画面モード（ENTER,TITLE,SEACHなど）
 * @param   mode    画面モード内の開始条件指定(MODE_...)
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_SubProcessChange( WORLDTRADE_WORK *wk, int subprccess, int mode )
{
  wk->sub_nextprocess  = subprccess;
  wk->sub_process_mode = mode;
}


//==============================================================================
/**
 * @brief   次のサブプロセスへ遷移する（一つ前を取っておく）
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_SubProcessUpdate( WORLDTRADE_WORK *wk )
{
  wk->old_sub_process = wk->sub_process;
  wk->sub_process     = wk->sub_nextprocess;

}
//==============================================================================
/**
 * @brief   会話スピードをコンフィグデータから取得する
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   メッセージスピード
 */
//==============================================================================
int WorldTrade_GetTalkSpeed( WORLDTRADE_WORK *wk )
{
  return MSGSPEED_GetWait();
}

//==============================================================================
/**
 * @brief   会話ウインドウの形状をコンフィグデータから取得する
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  WINTYPE 会話ウインドウのスキン番号
 */
//==============================================================================
int WorldTrade_GetMesWinType( WORLDTRADE_WORK *wk )
{
  return 0;
}

//------------------------------------------------------------------
/**
 * @brief   セルアクター初期化
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void InitCLACT( WORLDTRADE_WORK *wk )
{
  static const GFL_DISP_VRAM tbl =
  {
    GX_VRAM_BG_128_D,       // メイン2DエンジンのBG
    GX_VRAM_BGEXTPLTT_NONE,     // メイン2DエンジンのBG拡張パレット

    GX_VRAM_SUB_BG_128_C,     // サブ2DエンジンのBG
    GX_VRAM_SUB_BGEXTPLTT_NONE,   // サブ2DエンジンのBG拡張パレット

    GX_VRAM_OBJ_256_AB,       // メイン2DエンジンのOBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // メイン2DエンジンのOBJ拡張パレット

    GX_VRAM_SUB_OBJ_16_I,     // サブ2DエンジンのOBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,  // サブ2DエンジンのOBJ拡張パレット

    GX_VRAM_TEX_NONE,        // テクスチャイメージスロット
    GX_VRAM_TEXPLTT_NONE,      // テクスチャパレットスロット
    GX_OBJVRAMMODE_CHAR_1D_128K,
    GX_OBJVRAMMODE_CHAR_1D_128K,
  };

  // VRAM バンク設定
  VramBankSet( &tbl );

  // CellActorシステム初期化
  InitCellActor(wk, &tbl);

  // CellActro表示登録
  SetCellActor(wk);

}

//------------------------------------------------------------------
/**
 * @brief   セルアクター解放
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void FreeCLACT( WORLDTRADE_WORK *wk )
{
  int i;

  // ユニオンＯＢＪグラフィックデータ解放
  FreeFieldObjData( wk );

  // キャラ・パレット・セル・セルアニメのリソースマネージャー破棄
  GFL_CLGRP_PLTT_Release( wk->resObjTbl[RES_HERO][CLACT_U_PLTT_RES] );
  GFL_CLGRP_CGR_Release( wk->resObjTbl[RES_HERO][CLACT_U_CHAR_RES] );
  GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[RES_HERO][CLACT_U_CELL_RES] );

  GFL_CLGRP_PLTT_Release( wk->resObjTbl[SUB_LCD][CLACT_U_PLTT_RES] );
  GFL_CLGRP_CGR_Release( wk->resObjTbl[SUB_LCD][CLACT_U_CHAR_RES] );
  GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[SUB_LCD][CLACT_U_CELL_RES] );

  GFL_CLGRP_PLTT_Release( wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES] );
  GFL_CLGRP_CGR_Release( wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES] );
  GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES] );

  GFL_CLGRP_CGR_Release( wk->resObjTbl[RES_CURSOR][CLACT_U_CHAR_RES] );
  GFL_CLGRP_CELLANIM_Release( wk->resObjTbl[RES_CURSOR][CLACT_U_CELL_RES] );

  // セルアクターセット破棄
  GFL_CLACT_UNIT_Delete(wk->clactSet);
  wk->clactSet = NULL;

  //OAMレンダラー破棄
  GFL_CLACT_SYS_Delete();


}


//------------------------------------------------------------------
/**
 * @brief   0になるまで１減らす
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void ServerWaitTimeFunc( WORLDTRADE_WORK *wk )
{

  if(wk->serverWaitTime){
    wk->serverWaitTime--;
  }
}

//==============================================================================
/**
 * @brief   １syncに１トレイずつBOXの数を確認していくスタート
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_BoxPokeNumGetStart( WORLDTRADE_WORK *wk )
{
  wk->boxSearchFlag = 1;
  wk->boxPokeNum    = 0;
}


//------------------------------------------------------------------
/**
 * @brief   １syncに１トレイずつBOXの数を確認していく(常駐処理）
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none
 */
//------------------------------------------------------------------
static void BoxPokeNumGet( WORLDTRADE_WORK *wk )
{
  if(wk->boxSearchFlag){
//    wk->boxPokeNum += BOXDAT_GetPokeExistCount( wk->param->mybox, wk->boxSearchFlag-1 );
    wk->boxPokeNum  = 0;
    wk->boxSearchFlag++;
    if(wk->boxSearchFlag==19){
      wk->boxSearchFlag = 0;
      OS_Printf("BOX総数は%d\n",wk->boxPokeNum);
    }

  }

}



//------------------------------------------------------------------
/**
 * @brief   時間アイコン追加
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none
 */
//------------------------------------------------------------------
void WorldTrade_TimeIconAdd( WORLDTRADE_WORK *wk )
{
  wk->timeWaitWork = TimeWaitIconAdd( wk->MsgWin, WORLDTRADE_MESFRAME_CHR );
}

//------------------------------------------------------------------
/**
 * @brief   時間アイコン消去（NULLチェックする）
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none
 */
//------------------------------------------------------------------
void WorldTrade_TimeIconDel( WORLDTRADE_WORK *wk )
{
  if(wk->timeWaitWork!=NULL){
    TimeWaitIconDel(wk->timeWaitWork);
    wk->timeWaitWork = NULL;
  }
}

//==============================================================================
/**
 * @brief   セルアクターの座標変更（簡易処理版）
 *
 * @param   ptr   clact_work_ptr
 * @param   x   X
 * @param   y   Y
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_CLACT_PosChange( GFL_CLWK* ptr, int x, int y )
{
  GFL_CLACTPOS pos;

  pos.x = x;
  pos.y = y;
  GFL_CLACT_WK_SetPos( ptr, &pos, CLSYS_DRAW_MAIN );

}

//------------------------------------------------------------------
/**
 * @brief パッシブ状態セット
 * @param inIsMain    対象がメイン画面か？  YES:1 NO:0
 * @return  none
 */
//------------------------------------------------------------------
void WorldTrade_SetPassive(u8 inIsMain)
{
  if (inIsMain){
    G2_SetBlendBrightness(
      GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
      BRIGHT_VAL
    );
  }else{
    G2S_SetBlendBrightness(
      GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
      BRIGHT_VAL
    );
  }
}

//------------------------------------------------------------------
/**
 * @brief パッシブ状態セット  mypoke用
 * @param inIsMain    対象がメイン画面か？  YES:1 NO:0
 * @return  none
 */
//------------------------------------------------------------------
void WorldTrade_SetPassiveMyPoke(u8 inIsMain)
{
  if (inIsMain){
    G2_SetBlendBrightness(
      GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
      BRIGHT_VAL
    );
  }else{
    G2S_SetBlendBrightness(
      GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
      BRIGHT_VAL
    );
  }
}

//------------------------------------------------------------------
/**
 * @brief パッシブ状態解除
 * @param none
 * @return  none
 */
//------------------------------------------------------------------
void WorldTrade_ClearPassive(void)
{
  G2_BlendNone();
  G2S_BlendNone();
}

//----------------------------------------------------------------------------
/**
 *  @brief  システム系を初期化
 *
 *  @param  WORLDTRADE_WORK *wk ワーク
 *
 */
//-----------------------------------------------------------------------------
void WorldTrade_InitSystem( WORLDTRADE_WORK *wk )
{
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GFL_DISP_GX_SetVisibleControlDirect( 0 );
  GFL_DISP_GXS_SetVisibleControlDirect( 0 );

  InitCLACT( wk );

  GFL_BG_Init( HEAPID_WORLDTRADE );
  GFL_BMPWIN_Init( HEAPID_WORLDTRADE );


  // BG SYSTEM
  {
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &BGsys_data );
  }

  WorldTrade_WndSetting();
  G2_BlendNone();
  G2S_BlendNone();

  // VBlank関数セット
  if( wk->vblank_task == NULL )
  { 
    wk->vblank_task = GFUser_VIntr_CreateTCB(VBlankFunc, wk, 0);
  }
}

//----------------------------------------------------------------------------
/**
 *  @brief  システム系を破棄
 *
 *  @param  WORLDTRADE_WORK *wk ワーク
 */
//-----------------------------------------------------------------------------
void WorldTrade_ExitSystem( WORLDTRADE_WORK *wk )
{
  // VBlank関数セット
  if( wk->vblank_task )
  { 
    GFL_TCB_DeleteTask( wk->vblank_task );
    wk->vblank_task = NULL;
  }

  G2_BlendNone();
  G2S_BlendNone();

  FreeCLACT( wk );

  GFL_BMPWIN_Exit();
  GFL_BG_Exit();

  GX_SetVisibleWnd(GX_WNDMASK_NONE);
  GXS_SetVisibleWnd(GX_WNDMASK_NONE);


}


//----------------------------------------------------------------------------
/**
 *	@brief  GTS用フェイタルエラー表示
 *
 */
//-----------------------------------------------------------------------------
void WorldTrade_DispCallFatal( WORLDTRADE_WORK *wk )
{
  GAMEDATA_SaveAsyncCancel(GAMESYSTEM_GetGameData(wk->param->gamesys));
  NetErr_App_FatalDispCallWifiMessage(dwc_message_0023);
}
