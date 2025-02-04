//============================================================================================
/**
 * @file  worldtrade_deposit.c
 * @brief 世界交換ポケモン預ける画面処理
 * @author  Akito Mori
 * @date  06.04.16
 */
//============================================================================================
#include <gflib.h>
#include <dwc.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "arc_def.h"
#include <dpw_tr.h>
#include "print/wordset.h"
#include "message.naix"
#include "system/wipe.h"
#include "system/bmp_menu.h"
#include "system/bmp_winframe.h"
#include "sound/pm_sndsys.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/monsno_def.h"

#include "savedata/wifilist.h"
#include "net_app/worldtrade.h"
#include "worldtrade_local.h"
#include "msg/msg_wifi_lobby.h"
#include "msg/msg_wifi_gts.h"
#include "print/printsys.h"

#include "worldtrade.naix"      // グラフィックアーカイブ定義
#include "zukan_data.naix"
#include "../../../resource/zukan_data/zkn_sort_aiueo_idx.h"


#include "msg/msg_wifi_place_msg_world.h"


//============================================================================================
//  プロトタイプ宣言
//============================================================================================
/*** 関数プロトタイプ ***/
static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat );
static void BgInit( void );
static void BgExit( void );
static void BgGraphicSet( WORLDTRADE_WORK * wk );
static void BmpWinInit( WORLDTRADE_WORK *wk );
static void BmpWinDelete( WORLDTRADE_WORK *wk );
static void InitWork( WORLDTRADE_WORK *wk );
static void FreeWork( WORLDTRADE_WORK *wk );
static int SubSeq_Start( WORLDTRADE_WORK *wk);
static int SubSeq_Main( WORLDTRADE_WORK *wk);
static int RoundWork( int num, int max, int move );
static int SubSeq_HeadWordSelectList( WORLDTRADE_WORK *wk );
static int SubSeq_HeadWordSelectWait( WORLDTRADE_WORK *wk );
static int SubSeq_PokeNameSelectList( WORLDTRADE_WORK *wk );
static int SubSeq_PokeNameSelectWait( WORLDTRADE_WORK *wk );
static int SubSeq_End( WORLDTRADE_WORK *wk);
static int SubSeq_YesNo( WORLDTRADE_WORK *wk);
static int SubSeq_MessageWait( WORLDTRADE_WORK *wk );
#if 0
static void PokeWantPrint( GFL_MSGDATA *MsgManager, GFL_MSGDATA *MonsNameManager,WORDSET *WordSet, GFL_BMPWIN win[], int monsno, int sex, int level );
#endif
static void PokeDepositPrint( 
  GFL_MSGDATA *MsgManager, 
  WORDSET *WordSet, 
  GFL_BMPWIN *win[], 
  POKEMON_PASO_PARAM *ppp,
  Dpw_Tr_PokemonDataSimple *post,
  WT_PRINT *print );
static int  SubSeq_SexSelctWait( WORLDTRADE_WORK *wk );
static int  SubSeq_SexSelctList( WORLDTRADE_WORK *wk );
static int  SubSeq_SexSelctMessage( WORLDTRADE_WORK *wk );
static int  SubSeq_LevelSelectMessage( WORLDTRADE_WORK *wk );
static int  SubSeq_LevelSelectWait( WORLDTRADE_WORK *wk );
static int  SubSeq_LevelSelectList( WORLDTRADE_WORK *wk );
static int  SubSeq_DepositOkMessage( WORLDTRADE_WORK *wk );
static int  SubSeq_DepositOkYesNo( WORLDTRADE_WORK *wk );
static int SubSeq_DepositOkYesNoWait( WORLDTRADE_WORK *wk );
static void DepositPokemonDataMake( Dpw_Tr_Data *dtd, WORLDTRADE_WORK *wk );


static int PokeNameSortListMake( BMP_MENULIST_DATA **menulist, GFL_MSGDATA *monsnameman, 
                  GFL_MSGDATA *msgman,u16 *table, u8 *sinou,
                  int num, int select,ZUKAN_SAVEDATA *zukan );
static u16* ZukanSortDataGet( int heap, int idx, int* p_arry_num );
static PRINTSYS_LSB GetSexColor( int sex, PRINTSYS_LSB color );
static void SetCellActor(WORLDTRADE_WORK *wk);
static void DelCellActor(WORLDTRADE_WORK *wk);



enum{
  SUBSEQ_START=0,
  SUBSEQ_MAIN,
  SUBSEQ_END,

  SUBSEQ_HEADWORD_SELECT_LIST,
    SUBSEQ_HEADWORD_SELECT_WAIT,
    SUBSEQ_POKENAME_SELECT_LIST,
    SUBSEQ_POKENAME_SELECT_WAIT,
  SUBSEQ_SEX_SELECT_MES,
  SUBSEQ_SEX_SELECT_LIST,
  SUBSEQ_SEX_SELECT_WAIT,
  SUBSEQ_LEVEL_SELECT_MES,
  SUBSEQ_LEVEL_SELECT_LIST,
  SUBSEQ_LEVEL_SELECT_WAIT,
  SUBSEQ_DEPOSITOK_MESSAGE,
  SUBSEQ_DEPOSIT_YESNO,
  SUBSEQ_DEPOSIT_YESNO_WAIT,
  
  SUBSEQ_MES_WAIT,
};
static int (*Functable[])( WORLDTRADE_WORK *wk ) = {
  SubSeq_Start,       // SUBSEQ_START=0,
  SubSeq_Main,              // SUBSEQ_MAIN,
  SubSeq_End,               // SUBSEQ_END,
  SubSeq_HeadWordSelectList,  // SUBSEQ_HEADWORD_SELECT_LIST
  SubSeq_HeadWordSelectWait,  // SUBSEQ_HEADWORD_SELECT_WAIT
  SubSeq_PokeNameSelectList,  // SUBSEQ_POKENAME_SELECT_LIST
  SubSeq_PokeNameSelectWait,  // SUBSEQ_POKENAME_SELECT_WAIT
  SubSeq_SexSelctMessage,   // SUBSEQ_SEX_SELECT_MES,
  SubSeq_SexSelctList,    // SUBSEQ_SEX_SELECT_LIST,
  SubSeq_SexSelctWait,    // SUBSEQ_SEX_SELECT_WAIT,
  SubSeq_LevelSelectMessage,  // SUBSEQ_LEVEL_SELECT_MES,
    SubSeq_LevelSelectList,     // SUBSEQ_LEVEL_SELECT_LIST,
    SubSeq_LevelSelectWait,     // SUBSEQ_LEVEL_SELECT_WAIT,
  SubSeq_DepositOkMessage,
    SubSeq_DepositOkYesNo,
  SubSeq_DepositOkYesNoWait,

  SubSeq_MessageWait,     // SUBSEQ_MES_WAIT
};

#define BOX_CUROSOR_END_POS     ( 30 )
#define BOX_CUROSOR_TRAYNAME_POS  ( 31 )


// ほしいポケモン・あずけるポケモン情報
#define INFORMATION_STR_X (  0 )
#define INFORMATION_STR_Y (  3 )
#define INFORMATION2_STR_X  (  1 )
#define INFORMATION2_STR_Y  (  5 )
#define INFORMATION3_STR_X  (  2 )
#define INFORMATION3_STR_Y  (  7 )
#define INFORMATION_STR_SX  ( 12 )
#define INFORMATION_STR_SY  (  2 )


#define TITLE_MESSAGE_OFFSET   ( WORLDTRADE_MENUFRAME_CHR + MENU_WIN_CGX_SIZ )
#define LINE_MESSAGE_OFFSET    ( TITLE_MESSAGE_OFFSET     + TITLE_TEXT_SX*TITLE_TEXT_SY )
#define INFOMATION_STR_OFFSET  ( LINE_MESSAGE_OFFSET      + LINE_TEXT_SX*LINE_TEXT_SY )
#define YESNO_OFFSET       ( INFOMATION_STR_OFFSET    + INFORMATION_STR_SX*INFORMATION_STR_SY*6 )

static const u16 infomation_bmp_table[6][2]={
  { INFORMATION_STR_X,  INFORMATION_STR_Y,     }, // 「ほしいポケモン」
  { INFORMATION2_STR_X, INFORMATION2_STR_Y,    }, // ポケモン名
  { INFORMATION3_STR_X, INFORMATION3_STR_Y,    }, // レベル指定
  { INFORMATION_STR_X,  INFORMATION_STR_Y  +8, }, // 「あずけるポケモン」
  { INFORMATION2_STR_X, INFORMATION2_STR_Y +8, }, // ポケモン名
  { INFORMATION3_STR_X, INFORMATION3_STR_Y +8, }, // レベル

};

// 検索レベル指定用構造体
typedef struct{
  int msg;
  s16 min;
  s16 max;
}WT_LEVEL_TERM;

// 検索レベル数(最後はキャンセルだけど）
#define LEVEL_SELECT_NUM  (12)

// 検索レベル指定テーブル：預ける時のマッチング条件指定
static const WT_LEVEL_TERM level_minmax_table[]={
  { msg_gtc_12_001, 0,   0, },  // きにしない
  { msg_gtc_12_002, 0,   9, },  // レベル１０みまん
  { msg_gtc_12_003,10,   0, },  // レベル１０以上
  { msg_gtc_12_004,20,   0, },  // レベル２０以上
  { msg_gtc_12_005,30,   0, },  // レベル３０以上
  { msg_gtc_12_006,40,   0, },  // レベル４０以上
  { msg_gtc_12_007,50,   0, },  // レベル５０以上
  { msg_gtc_12_008,60,   0, },  // レベル６０以上
  { msg_gtc_12_009,70,   0, },  // レベル７０以上
  { msg_gtc_12_010,80,   0, },  // レベル８０以上
  { msg_gtc_12_011,90,   0, },  // レベル９０以上
  { msg_gtc_12_012,100,100, },  // レベル１００
};


// 検索レベル指定テーブル：検索する時のマッチング条件指定
static const WT_LEVEL_TERM search_level_minmax_table[]={
  { msg_gtc_search_001, 0,   0, },  // きにしない
  { msg_gtc_search_002, 1,   10, }, // レベル1から10
  { msg_gtc_search_003,11,   20, }, // レベル11から20
  { msg_gtc_search_004,21,   30, }, // レベル21から30
  { msg_gtc_search_005,31,   40, }, // レベル31から40
  { msg_gtc_search_006,41,   50, }, // レベル41から50
  { msg_gtc_search_007,51,   60, }, // レベル51から60
  { msg_gtc_search_008,61,   70, }, // レベル61から70
  { msg_gtc_search_009,71,   80, }, // レベル71から80
  { msg_gtc_search_010,81,   90, }, // レベル81から90
  { msg_gtc_search_011,91,   100, },  // レベル91から100
};

// 検索マッチング：検索レベル数(最後はキャンセルだけど）
#define SEARCH_LEVEL_SELECT_NUM   (11)

// 国選択用ソートリスト・頭文字リスト
#include "gts_country_list.cdat"


//============================================================================================
//  プロセス関数
//============================================================================================

//==============================================================================
/**
 * @brief   世界交換入り口画面初期化
 *
 * @param   wk    GTS画面ワーク
 * @param   seq   （未使用）
 *
 * @retval  int   アプリシーケンス
 */
//==============================================================================
int WorldTrade_Deposit_Init(WORLDTRADE_WORK *wk, int seq)
{
  // ワーク初期化
  InitWork( wk );
  
  // BG設定
  BgInit( );
  // サブ画面初期化
  WorldTrade_SubLcdBgInit( wk, 0, 0 );

  // BGグラフィック転送
  BgGraphicSet( wk );

  // BMPWIN確保
  BmpWinInit( wk );

  SetCellActor(wk);


  WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
  OS_Printf( "******************** worldtrade_deposit.c [226] M ********************\n" );
#endif

  // 通信状態を確認してアイコンの表示を変える
    WorldTrade_WifiIconAdd( wk );
   
    // ほしいポケモン・あずけるポケモン描画
  WodrldTrade_PokeWantPrint( wk->MsgManager, wk->MonsNameManager, 
        wk->WordSet, &wk->InfoWin[0], 0,DPW_TR_GENDER_NONE,-1, &wk->print);
  PokeDepositPrint( wk->MsgManager, wk->WordSet, &wk->InfoWin[3], wk->deposit_ppp, &wk->Post, &wk->print );

  // 条件入力システム初期化
  {
    WORLDTRADE_INPUT_HEADER wih;
    wih.MenuBmp   = &wk->MenuWin[0];
    wih.BackBmp   = &wk->BackWin;
    wih.CursorAct = wk->SubCursorActWork;
    wih.ArrowAct[0] = wk->BoxArrowActWork[0];
    wih.ArrowAct[1] = wk->BoxArrowActWork[1];
    wih.SearchCursorAct = NULL;
    wih.MsgManager      = wk->MsgManager;
    wih.MonsNameManager = wk->MonsNameManager;
    wih.CountryNameManager = wk->CountryNameManager;
    wih.Zukan     = wk->param->zukanwork;
    wih.SinouTable      = wk->dw->sinouTable;
    wih.config          = wk->param->config;
    wk->WInputWork = WorldTrade_Input_Init( &wih,  GFL_BG_FRAME2_M, SITUATION_DEPOSIT );
  }
  

  OS_TPrintf("図鑑総数 = %d\n",wk->dw->nameSortNum);


  wk->subprocess_seq = SUBSEQ_START;
  wk->sub_display_continue  = FALSE;
  
  return SEQ_FADEIN;
}

//==============================================================================
/**
 * @brief   世界交換入り口画面メイン
 *
 * @param   wk    GTS画面ワーク
 * @param   seq   （未使用）
 *
 * @retval  int   アプリシーケンス
 */
//==============================================================================
int WorldTrade_Deposit_Main(WORLDTRADE_WORK *wk, int seq)
{
  int ret;

  
  ret = (*Functable[wk->subprocess_seq])( wk );

  return ret;
}


//==============================================================================
/**
 * @brief   世界交換入り口画面終了
 *
 * @param   wk    GTS画面ワーク
 * @param   seq   （未使用）
 *
 * @retval  int   アプリシーケンス
 */
//==============================================================================
int WorldTrade_Deposit_End(WORLDTRADE_WORK *wk, int seq)
{
  //WirelessIconEasyEnd();

  DelCellActor(wk);

  // 条件入力システム終了
  WorldTrade_Input_Exit( wk->WInputWork );

  FreeWork( wk );
  
  BmpWinDelete( wk );
  
  BgExit();

  // サブ画面ＢＧ情報解放
  WorldTrade_SubLcdBgExit( wk );

  // 「DSの下画面をみてねアイコン」非表示
  GFL_CLACT_WK_SetDrawEnable( wk->PromptDsActWork, 0 );
  
  WorldTrade_SubProcessUpdate( wk );
  
  return SEQ_INIT;
}


//------------------------------------------------------------------
/**
 * @brief   会話ウインドウ表示
 *
 * @param   wk    
 *
 * @retval  none    
 */
//------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * @brief 会話ウインドウ表示
 *
 * @param   wk    GTS画面ワーク
 * @param   msgno メッセージNO
 * @param   wait  ウェイト
 * @param   flag  未使用
 * @param   dat   未使用
 */
//----------------------------------------------------------------------------------
static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat )
{
  // 文字列取得
  STRBUF *tempbuf;
  
  // 文字列取得
  tempbuf = GFL_MSG_CreateString(  wk->MsgManager, msgno );

  // WORDSET展開
  WORDSET_ExpandStr( wk->WordSet, wk->TalkString, tempbuf );
  

  // 会話ウインドウ枠描画
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin),  0x0f );
  GFL_BMPWIN_MakeTransWindow(wk->MsgWin);
  BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, WORLDTRADE_MESFRAME_CHR, WORLDTRADE_MESFRAME_PAL );

  // 文字列描画開始
  GF_STR_PrintSimple( wk->MsgWin, FONT_TALK, wk->TalkString, 0, 0, &wk->print);

  GFL_STR_DeleteBuffer(tempbuf);
}



//--------------------------------------------------------------------------------------------
/**
 * BG設定
 *
 * @param ini   BGLデータ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgInit( void )
{

  // メイン画面テキスト面
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( GFL_BG_FRAME0_M );
    GFL_BG_SetVisible( GFL_BG_FRAME0_M, TRUE );
  }

  // メイン画面メニュー面
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( GFL_BG_FRAME1_M, TRUE );
  }

  // メイン画面背景面
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_FillScreen(  GFL_BG_FRAME2_M, 0x0000, 0, 0, 32, 24, 0 );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
    GFL_BG_SetVisible( GFL_BG_FRAME2_M, TRUE );
  }

  // 情報表示画面テキスト面
  { 
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( GFL_BG_FRAME3_M );
    GFL_BG_SetVisible( GFL_BG_FRAME3_M, TRUE );
  }

  GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 32, 0, HEAPID_WORLDTRADE );
  GFL_BG_SetClearCharacter( GFL_BG_FRAME3_M, 32, 0, HEAPID_WORLDTRADE );


}

//--------------------------------------------------------------------------------------------
/**
 * BG解放
 *
 * @param ini   BGLデータ
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( void )
{


  // メイン画面ＢＧ情報解放
  GFL_BG_FreeBGControl(GFL_BG_FRAME2_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
  GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
}


//--------------------------------------------------------------------------------------------
/**
 * グラフィックデータセット
 *
 * @param wk    ポケモンリスト画面のワーク
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
static void BgGraphicSet( WORLDTRADE_WORK * wk )
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_WORLDTRADE_GRA, HEAPID_WORLDTRADE );

  // 上下画面ＢＧパレット転送
  GFL_ARC_UTIL_TransVramPalette( ARCID_WORLDTRADE_GRA, NARC_worldtrade_deposit_nclr, PALTYPE_MAIN_BG, 0, 16*2*2,  HEAPID_WORLDTRADE);
  
  // 会話フォントパレット転送
  TalkFontPaletteLoad( PALTYPE_MAIN_BG, WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

  // 会話ウインドウグラフィック転送
  BmpWinFrame_GraphicSet( GFL_BG_FRAME0_M, WORLDTRADE_MESFRAME_CHR, 
            WORLDTRADE_MESFRAME_PAL,  0, HEAPID_WORLDTRADE );


  BmpWinFrame_GraphicSet( GFL_BG_FRAME0_M, WORLDTRADE_MENUFRAME_CHR,
            WORLDTRADE_MENUFRAME_PAL, 0, HEAPID_WORLDTRADE );


  // メイン画面BG1キャラ転送
  GFL_ARC_UTIL_TransVramBgCharacter( ARCID_WORLDTRADE_GRA, NARC_worldtrade_deposit_lz_ncgr, GFL_BG_FRAME1_M, 0, 0, 1, HEAPID_WORLDTRADE);

  // メイン画面BG1スクリーン転送
  GFL_ARC_UTIL_TransVramScreen(   ARCID_WORLDTRADE_GRA, NARC_worldtrade_deposit_lz_nscr, GFL_BG_FRAME1_M, 0, 32*24*2, 1, HEAPID_WORLDTRADE);

  // メイン画面BG2キャラ転送
  GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_worldtrade_input_lz_ncgr, GFL_BG_FRAME2_M, 0, 16*3*0x20, 1, HEAPID_WORLDTRADE);

  WorldTrade_SubLcdWinGraphicSet( wk );   // トレードルームウインドウ転送

  GFL_ARC_CloseDataHandle( p_handle );

}

//------------------------------------------------------------------
/**
 * BMPWIN処理（文字パネルにフォント描画）
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void BmpWinInit( WORLDTRADE_WORK *wk )
{
  int i;
  // ---------- メイン画面 ------------------

  // BG0面BMPWINタイトルウインドウ確保・描画
#if 0 
  GF_BGL_BmpWinAdd( &wk->TitleWin, GFL_BG_FRAME0_M,
  TITLE_TEXT_X, TITLE_TEXT_Y, TITLE_TEXT_SX, TITLE_TEXT_SY, WORLDTRADE_TALKFONT_PAL,  TITLE_MESSAGE_OFFSET );

  GFL_BMP_Clear( &wk->TitleWin, 0x0000 );
  
  // 「ポケモンをあずける」描画
  WorldTrade_TalkPrint( &wk->TitleWin, wk->TitleString, 0, 1, 0, PRINTSYS_LSB_Make(15,14,0) );
#endif    //金銀で不要

  // 一行ウインドウ
  wk->MsgWin= GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
    LINE_TEXT_X, LINE_TEXT_Y, LINE_TEXT_SX, LINE_TEXT_SY, 
    WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0x0000 );
  GFL_BMPWIN_MakeTransWindow(wk->MsgWin);
//  Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_05_001, MSG_ALLPUT, 0 );


  // あずけるポケモン・ほしいポケモン情報BMPWIN確保
  for(i=0;i<6;i++){
    wk->InfoWin[i] = GFL_BMPWIN_Create( GFL_BG_FRAME3_M,
      infomation_bmp_table[i][0], infomation_bmp_table[i][1], 
      INFORMATION_STR_SX, INFORMATION_STR_SY, WORLDTRADE_TALKFONT_PAL,  
      GFL_BMP_CHRAREA_GET_B  );
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin[i]), 0x0000 );
    GFL_BMPWIN_MakeTransWindow(wk->InfoWin[i]);
  }

  // サブ画面のGTS説明用BMPWINを確保する
  WorldTrade_SubLcdExpainPut( wk, EXPLAIN_AZUKERU );

  OS_Printf("WORLDTRADE heap残り = %d\n",GFL_HEAP_GetHeapFreeSize( HEAPID_WORLDTRADE ));
} 

//------------------------------------------------------------------
/**
 * @brief   確保したBMPWINを解放
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void BmpWinDelete( WORLDTRADE_WORK *wk )
{
  int i;

  //BMPを消す前に残ったストリームを消去
  WT_PRINT_ClearBuffer( &wk->print );
  
  GFL_BMPWIN_Delete( wk->ExplainWin );
  for(i=0;i<6;i++){
    GFL_BMPWIN_Delete( wk->InfoWin[i] );
  }
  GFL_BMPWIN_Delete( wk->MsgWin );
/// GFL_BMPWIN_Delete( wk->TitleWin );


}

//------------------------------------------------------------------
/**
 * セルアクター登録
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void SetCellActor(WORLDTRADE_WORK *wk)
{
  //登録情報格納
  GFL_CLWK_DATA add;
  //WorldTrade_MakeCLACT( &add,  wk, &wk->clActHeader_main, NNS_G2D_VRAM_TYPE_2DMAIN );

  GFL_STD_MemClear(&add,sizeof(GFL_CLWK_DATA));

  // 入力用カーソル登録
  add.pos_x = 160;
  add.pos_y = 32;
  wk->SubCursorActWork = GFL_CLACT_WK_Create(wk->clactSet,
      wk->resObjTbl[RES_CURSOR][CLACT_U_CHAR_RES],
      wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
      wk->resObjTbl[RES_CURSOR][CLACT_U_CELL_RES],
      &add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
  GFL_CLACT_WK_SetAnmSeq( wk->SubCursorActWork, 12 );
  GFL_CLACT_WK_SetAutoAnmFlag( wk->SubCursorActWork, 1 );
  GFL_CLACT_WK_StopAnm( wk->SubCursorActWork );
  GFL_CLACT_WK_SetDrawEnable( wk->SubCursorActWork, 0 );

  // 入力用ページ移動カーソル登録（右向き）
  add.pos_x = 228;
  add.pos_y = 117;
  wk->BoxArrowActWork[0] = GFL_CLACT_WK_Create(wk->clactSet,
      wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
      wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
      wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
      &add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
  GFL_CLACT_WK_SetAnmSeq( wk->BoxArrowActWork[0], CELL_BOXARROW_NO );
  GFL_CLACT_WK_SetDrawEnable( wk->BoxArrowActWork[0], 0 );
  // 入力用ページ移動カーソル登録(左向き）
  add.pos_x = 140;
  wk->BoxArrowActWork[1] = GFL_CLACT_WK_Create(wk->clactSet,
      wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
      wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
      wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
      &add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
  GFL_CLACT_WK_SetAnmSeq( wk->BoxArrowActWork[1], CELL_BOXARROW_NO+1 );
  GFL_CLACT_WK_SetDrawEnable( wk->BoxArrowActWork[1], 0 );
}

//------------------------------------------------------------------
/**
 * @brief   登録したセルアクターの削除
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void DelCellActor( WORLDTRADE_WORK *wk )
{
  GFL_CLACT_WK_Remove(wk->SubCursorActWork);
  GFL_CLACT_WK_Remove(wk->BoxArrowActWork[0]);
  GFL_CLACT_WK_Remove(wk->BoxArrowActWork[1]);


}



//------------------------------------------------------------------
/**
 * 世界交換ワーク初期化
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void InitWork( WORLDTRADE_WORK *wk )
{


  // 文字列バッファ作成
  wk->TalkString  = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WORLDTRADE );

  // ポケモンをあずける
//  wk->TitleString = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_05_001 );金銀で不要

  // ポケモン名テーブル作成用のワーク確保
  wk->dw = GFL_HEAP_AllocMemory(HEAPID_WORLDTRADE, sizeof(DEPOSIT_WORK));
  MI_CpuClearFast( wk->dw, sizeof(DEPOSIT_WORK) );

  // 図鑑ソートデータ(全国図鑑と、シンオウ図鑑の分岐が必要だとおもう）
  wk->dw->nameSortTable = WorldTrade_ZukanSortDataGet( HEAPID_WORLDTRADE, 0, &wk->dw->nameSortNum );

  // シンオウ図鑑テーブル
  wk->dw->sinouTable    = WorldTrade_SinouZukanDataGet( HEAPID_WORLDTRADE );

  // カーソル位置初期化
  WorldTrade_SelectListPosInit( &wk->selectListPos );
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

  GFL_HEAP_FreeMemory( wk->dw->sinouTable );
  GFL_HEAP_FreeMemory(wk->dw->nameSortTable);
  GFL_HEAP_FreeMemory(wk->dw);

  GFL_STR_DeleteBuffer( wk->TalkString ); 
/// GFL_STR_DeleteBuffer( wk->TitleString ); 金銀で不要

}



//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------


//------------------------------------------------------------------
/**
 * @brief   サブプロセスシーケンススタート処理
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_Start( WORLDTRADE_WORK *wk)
{
  // 
  if(WIPE_SYS_EndCheck()){
    SubSeq_MessagePrint( wk, msg_gtc_01_010, 1, 0, 0x0f0f );
    WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );

    // ほしいポケモン・あずけるポケモン描画
    
//     wk->subprocess_seq = SUBSEQ_MAIN;
  }

  return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   サブプロセスシーケンスメイン
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_Main( WORLDTRADE_WORK *wk)
{
  if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
     WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, MODE_DEPOSIT_SELECT );
    wk->subprocess_seq = SUBSEQ_END;
  }

  wk->subprocess_seq = SUBSEQ_HEADWORD_SELECT_LIST;

  return SEQ_MAIN;
}




//------------------------------------------------------------------
/**
 * @brief   numにmoveを足し,0以下ならmax-1に、maxなら0にして返す
 *
 * @param   num   元の値
 * @param   max   最大値
 * @param   move  足す値（＋−あり）
 *
 * @retval  int   結果の値
 */
//------------------------------------------------------------------
static int RoundWork( int num, int max, int move )
{
  num += move;
  if(num < 0) {
    return max-1;
  }
  if(num==max){
    return 0;
  }
  return num;
}


//------------------------------------------------------------------
/**
 * @brief   頭文字選択リスト作成
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_HeadWordSelectList( WORLDTRADE_WORK *wk )
{
  // 頭文字選択メニュー作成
//  wk->BmpListWork = WorldTrade_WordheadBmpListMake( wk, &wk->BmpMenuList, &wk->MenuWin[0], wk->MsgManager );
//  wk->listpos = 0xffff;
//  wk->subprocess_seq = SUBSEQ_HEADWORD_SELECT_WAIT;

  // 条件入力システム開始
  WorldTrade_Input_Start( wk->WInputWork, MODE_POKEMON_NAME );
  
  wk->subprocess_seq = SUBSEQ_POKENAME_SELECT_WAIT;

  return SEQ_MAIN;
  
}


//------------------------------------------------------------------
/**
 * @brief   頭文字選択待ち
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_HeadWordSelectWait( WORLDTRADE_WORK *wk )
{
  switch(WorldTrade_BmpMenuList_Main( wk->BmpListWork, &wk->listpos )){
  case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10:
    BmpMenuList_Exit( wk->BmpListWork, &wk->dw->headwordListPos, &wk->dw->headwordPos );
    BmpMenuWork_ListDelete( wk->BmpMenuList );
    wk->subprocess_seq  = SUBSEQ_POKENAME_SELECT_LIST;
    PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

    break;

  // キャンセル
  case BMPMENU_CANCEL:
    BmpMenuList_Exit( wk->BmpListWork, &wk->dw->headwordListPos, &wk->dw->headwordPos );
    BmpMenuWork_ListDelete( wk->BmpMenuList );
    BmpMenuWinClear( wk->MenuWin[0], WINDOW_TRANS_ON );
    BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
    GFL_BMPWIN_Delete( wk->MenuWin[0] );
    GFL_BMPWIN_Delete( wk->MenuWin[1] );

    WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, MODE_DEPOSIT_SELECT );
    wk->subprocess_seq = SUBSEQ_END;
    PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
    break;
  }
  
  return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   ポケモン名前選択リスト作成
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_PokeNameSelectList( WORLDTRADE_WORK *wk )
{
  // ポケモン名リスト作成
  wk->BmpListWork = WorldTrade_PokeNameListMake( wk,
            &wk->BmpMenuList, wk->MenuWin[1], 
            wk->MsgManager, wk->MonsNameManager,wk->dw,
            wk->param->zukanwork );
  wk->listpos = 0xffff;

  wk->subprocess_seq = SUBSEQ_POKENAME_SELECT_WAIT;

  return SEQ_MAIN;
  
}

//------------------------------------------------------------------
/**
 * @brief   ポケモンの説別分布から性別は決定するか？入力が必要か？
 *
 * @param   dtp       検索条件
 * @param   sex_selection 性別指定（オス・メス・性別無し）
 *
 * @retval  int   0:性別の入力が必要  1:決定しているので必要ない
 */
//------------------------------------------------------------------
int WorldTrade_SexSelectionCheck( Dpw_Tr_PokemonSearchData *dtsd, int sex_selection ) 
{
  switch(sex_selection){
  case POKEPER_SEX_MALE:
    dtsd->gender = PARA_MALE+1;
    return 1;
    break;
  case POKEPER_SEX_FEMALE:
    dtsd->gender = PARA_FEMALE+1;
    return 1;
    break;
  case POKEPER_SEX_UNKNOWN:
    dtsd->gender = PARA_UNK+1;
    return 1;
    break;
  }
  
  return 0;
}


//------------------------------------------------------------------
/**
 * @brief   ポケモン名前選択
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_PokeNameSelectWait( WORLDTRADE_WORK *wk )
{
  u32 result;
  int sex;
  
//  switch((result=WorldTrade_BmpMenuList_Main( wk->BmpListWork, &wk->listpos ))){
  switch((result=WorldTrade_Input_Main( wk->WInputWork ))){
  case BMPMENULIST_NULL:
    break;

  // キャンセル
  case BMPMENU_CANCEL:
    BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
    WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, MODE_DEPOSIT_SELECT );
    wk->subprocess_seq = SUBSEQ_END;
//    PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
    
    WorldTrade_SelectNameListBackup( &wk->selectListPos, wk->dw->headwordListPos+wk->dw->headwordPos, wk->dw->nameListPos, wk->dw->namePos );

    break;
  // 選択
  default:
    wk->Want.characterNo = result;
//    PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

    // 性別分布情報取得 
    wk->dw->sex_selection = PokePersonalParaGet(result,POKEPER_ID_sex);
    // 性別は固定か？（固定の場合は性別がWantに格納される）
    if(WorldTrade_SexSelectionCheck( &wk->Want, wk->dw->sex_selection )){
      // 性別は決定しているので、入力する必要が無い
      wk->subprocess_seq  = SUBSEQ_LEVEL_SELECT_MES;
      sex                 = wk->Want.gender;
    }else{
      // 性別は決定していないので入力が必要
      wk->subprocess_seq  = SUBSEQ_SEX_SELECT_MES;
      sex = DPW_TR_GENDER_NONE;
    }

    // 名前決定(性別がきまることもある）
    WodrldTrade_PokeWantPrint( wk->MsgManager, wk->MonsNameManager, wk->WordSet, 
              wk->InfoWin, wk->Want.characterNo,sex,-1, &wk->print);

    WorldTrade_SelectNameListBackup( &wk->selectListPos, wk->dw->headwordListPos+wk->dw->headwordPos, wk->dw->nameListPos, wk->dw->namePos );

    break;

  }
  
  return SEQ_MAIN;
}



//------------------------------------------------------------------
/**
 * @brief   性別を選んでくださいメッセージ
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int  SubSeq_SexSelctMessage( WORLDTRADE_WORK *wk )
{
  // ポケモンの性別を選んでください
  SubSeq_MessagePrint( wk, msg_gtc_01_011, 1, 0, 0x0f0f );
  WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_SEX_SELECT_LIST );

  // 性別選択ウインドウ確保
//  GF_BGL_BmpWinAdd(&wk->MenuWin[0], GFL_BG_FRAME0_M,
//    SELECT_MENU3_X,   SELECT_MENU3_Y,   SELECT_MENU3_SX,    SELECT_MENU3_SY, 
//    WORLDTRADE_TALKFONT_PAL,  SELECT_MENU3_OFFSET );
//  GFL_BMP_Clear( &wk->MenuWin[0], 0x0000 );
  
  return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   性別選択リスト作成・表示
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int  SubSeq_SexSelctList( WORLDTRADE_WORK *wk )
{
//  wk->BmpListWork = WorldTrade_SexSelectListMake( &wk->BmpMenuList, &wk->MenuWin[0], wk->MsgManager );
  wk->listpos = 0xffff;

  // 条件入力システム開始
  WorldTrade_Input_Start( wk->WInputWork, MODE_SEX );

  wk->subprocess_seq = SUBSEQ_SEX_SELECT_WAIT;

  
  return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   性別選択リスト選択待ち
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int  SubSeq_SexSelctWait( WORLDTRADE_WORK *wk )
{
  u32 result;
  switch((result=WorldTrade_Input_Main( wk->WInputWork ))){
  // キャンセル
  case BMPMENU_CANCEL:
    BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
//    PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

    wk->subprocess_seq = SUBSEQ_START;
    break;
  // 選択
  case PARA_MALE: case PARA_FEMALE: case PARA_UNK: 
//    PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

    wk->Want.gender     = result+1;
    wk->subprocess_seq  = SUBSEQ_LEVEL_SELECT_MES;

    // 性別決定
    WodrldTrade_PokeWantPrint( wk->MsgManager, wk->MonsNameManager, wk->WordSet, 
              wk->InfoWin, wk->Want.characterNo,wk->Want.gender,-1, &wk->print);

    break;
  }

  return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   レベルを選んでくださいメッセージ表示
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_LevelSelectMessage( WORLDTRADE_WORK *wk )
{
  // ポケモンのレベルをきめてください
  SubSeq_MessagePrint( wk, msg_gtc_01_012, 1, 0, 0x0f0f );
  WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_LEVEL_SELECT_LIST );

  // 性別選択ウインドウ確保
//  GF_BGL_BmpWinAdd(&wk->MenuWin[0], GFL_BG_FRAME0_M,
//    SELECT_MENU4_X,   SELECT_MENU4_Y,   SELECT_MENU4_SX,    SELECT_MENU4_SY, 
//    WORLDTRADE_TALKFONT_PAL,  SELECT_MENU4_OFFSET );
//  GFL_BMP_Clear( &wk->MenuWin[0], 0x0000 );

  return SEQ_MAIN;
}



//------------------------------------------------------------------
/**
 * @brief   レベル選択リスト作成・表示
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_LevelSelectList( WORLDTRADE_WORK *wk )
{
  // レベル選択リスト作成
//  wk->BmpListWork = WorldTrade_LevelListMake(&wk->BmpMenuList, &wk->MenuWin[0], wk->MsgManager,
//    LEVEL_PRINT_TBL_DEPOSIT );

  // 条件入力システム開始
  WorldTrade_Input_Start( wk->WInputWork, MODE_LEVEL );

  wk->listpos = 0xffff;

  wk->subprocess_seq = SUBSEQ_LEVEL_SELECT_WAIT;

  return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   レベル選択待ち
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_LevelSelectWait( WORLDTRADE_WORK *wk )
{
  u32 result;
//  switch((result=WorldTrade_BmpMenuList_Main( wk->BmpListWork, &wk->listpos ))){
  switch((result=WorldTrade_Input_Main( wk->WInputWork ))){
  case BMPMENULIST_NULL:
    break;
  // キャンセル
  case BMPMENU_CANCEL:
  case 12:
    BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
//    PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
    
    // ポケモンの性別が固定かどうかをチェックして戻る
    if(WorldTrade_SexSelectionCheck( &wk->Want, wk->dw->sex_selection )){
      // 性別は固定無いので、ポケモン選択まで戻る
      wk->subprocess_seq = SUBSEQ_START;
    }else{
      // 性別入力が必要
      wk->subprocess_seq = SUBSEQ_SEX_SELECT_MES;
    }
    break;
  // レベル指定決定
  default: 
//    PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
    WorldTrade_LevelMinMaxSet(&wk->Want, result, LEVEL_PRINT_TBL_DEPOSIT);
    wk->subprocess_seq  = SUBSEQ_DEPOSITOK_MESSAGE;

    // レベル指定決定
    WodrldTrade_PokeWantPrint( wk->MsgManager, wk->MonsNameManager, wk->WordSet, 
            wk->InfoWin, wk->Want.characterNo,wk->Want.gender,
            WorldTrade_LevelTermGet(wk->Want.level_min,wk->Want.level_max,
              LEVEL_PRINT_TBL_DEPOSIT), &wk->print);

    break;
  }

  return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   この条件でよい？メッセージ表示
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_DepositOkMessage( WORLDTRADE_WORK *wk )
{
  // このじょうけんでよろしいですか？
  SubSeq_MessagePrint( wk, msg_gtc_01_024, 1, 0, 0x0f0f );
  WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_DEPOSIT_YESNO );
  
  return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   これで条件であずけます？はい・いいえ
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_DepositOkYesNo( WORLDTRADE_WORK *wk )
{

  WorldTrade_TouchWinYesNoMake(wk,WORLDTRADE_YESNO_PY1, YESNO_OFFSET, 3, 0 );
  wk->subprocess_seq = SUBSEQ_DEPOSIT_YESNO_WAIT;

  return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   はい・いいえ待ち
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_DepositOkYesNoWait( WORLDTRADE_WORK *wk )
{

  u32 ret = WorldTrade_TouchSwMain(wk);

  if(ret==WORLDTRADE_RET_YES){
    // はい→ポケモンをあずける
    WorldTrade_TouchDelete( wk );
    WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_UPLOAD );
    wk->subprocess_seq  = SUBSEQ_END;
    wk->sub_out_flg = 1;

    // サーバーアップロード用のデータを作成
    DepositPokemonDataMake( &wk->UploadPokemonData, wk );
  }else if(ret==WORLDTRADE_RET_NO){
    // いいえ→ポケモン選択からやり直し
    WorldTrade_TouchDelete( wk );
    WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, MODE_DEPOSIT_SELECT );
    wk->subprocess_seq  = SUBSEQ_END;
  }

  return SEQ_MAIN;
  
}














//------------------------------------------------------------------
/**
 * @brief   サブプロセスシーケンス終了処理
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_End( WORLDTRADE_WORK *wk )
{
  // 接続画面だったら、上下画面でフェード
  if(wk->sub_nextprocess==WORLDTRADE_ENTER ){
    WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
    OS_Printf( "******************** worldtrade_deposit.c [1165] MS ********************\n" );
#endif
    wk->sub_out_flg = 1;
  }else{
    // 続きの画面にいく場合は上画面だけフェード
    WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
    OS_Printf( "******************** worldtrade_deposit.c [1171] M ********************\n" );
#endif
  }
  wk->subprocess_seq = 0;
  
  return SEQ_FADEOUT;
}





//------------------------------------------------------------------
/**
 * @brief   会話終了を待って次のシーケンスへ
 *
 * @param   wk    GTS画面ワーク
 *
 * @retval  int   次のサブシーケンス
 */
//------------------------------------------------------------------
static int SubSeq_MessageWait( WORLDTRADE_WORK *wk )
{
  if( GF_MSG_PrintEndCheck( &wk->print )==0){
    wk->subprocess_seq = wk->subprocess_nextseq;
  }
  return SEQ_MAIN;

}













// 性別アイコンを表示するためのカラー指定
static u16 sex_mark_col( int idx )
{
  switch( idx )
  { 
  case 0: return PRINTSYS_LSB_Make(5,6,0);
  case 1: return PRINTSYS_LSB_Make(3,4,0);
  }

  return 0;
}


//==============================================================================
/**
 * @brief   ポケモン名描画(MSG_ALLPUT)
 *
 * @param   win   書き込むBMPWIN
 * @param   nameman ポケモン名メッセージマネージャー
 * @param   monsno  ポケモンNO
 * @param   flag  センタリング
 * @param   color カラー指定
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_PokeNamePrint( GFL_BMPWIN *win, GFL_MSGDATA *nameman, int monsno, int flag, int y, PRINTSYS_LSB color, WT_PRINT *print )
{
  STRBUF *namebuf;

  // 名前
  if(monsno!=0){
    namebuf = GFL_MSG_CreateString( nameman, monsno );
    WorldTrade_SysPrint( win, namebuf, 0, y, flag, color, print );
    GFL_STR_DeleteBuffer(namebuf);
  }

}


//==============================================================================
/**
 * @brief   ポケモン名描画（MSG_NO_PUT版）
 *
 * @param   win   書き込むBMPWIN
 * @param   nameman ポケモン名メッセージマネージャー
 * @param   monsno  ポケモンNO
 * @param   y   Y位置ずらす用のオフセット
 * @param   color カラー指定
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_PokeNamePrintNoPut( GFL_BMPWIN *win, GFL_MSGDATA *nameman, int monsno, int y, PRINTSYS_LSB color, WT_PRINT *print )
{
  STRBUF *namebuf;

  // 名前
  if(monsno!=0){
    namebuf = GFL_MSG_CreateString( nameman, monsno );
    GF_STR_PrintColor( win, FONT_SYSTEM, namebuf, 0, y, MSG_NO_PUT, color, print);
    GFL_STR_DeleteBuffer(namebuf);
  }

}


//--------------------------------------------------------------
/**
 * @brief   国名表示
 *
 * @param   win       書き込むBMPWIN
 * @param   nameman     国名メッセージマネージャー
 * @param   country_code  国コード
 * @param   flag      1だとセンタリング、２だと右よせ
 * @param   y       Y座標ずらすオフセット
 * @param   color     カラー指定
 *
 * @retval  none
 *
 *
 */
//--------------------------------------------------------------
void WorldTrade_CountryPrint( GFL_BMPWIN *win, GFL_MSGDATA *nameman, GFL_MSGDATA *msgman, int country_code, int flag, int y, PRINTSYS_LSB color, WT_PRINT *print )
{
  STRBUF *namebuf;

  // 国名
  if(country_code!=0){
    namebuf = GFL_MSG_CreateString( nameman, country_code );
    WorldTrade_SysPrint( win, namebuf, 0, y, flag, color, print );
    GFL_STR_DeleteBuffer(namebuf);
  }
  else{ //「きにしない」を表示
    namebuf = GFL_MSG_CreateString( msgman, msg_gtc_search_015 );
    WorldTrade_SysPrint( win, namebuf, 0, y, flag, color, print );
    GFL_STR_DeleteBuffer(namebuf);
  }

}




//------------------------------------------------------------------
/**
 * @brief   性別カラー取得
 *
 * @param   sex   性別
 * @param   color カラー
 *
 * @retval  PRINTSYS_LSB  カラー指定
 */
//------------------------------------------------------------------
static PRINTSYS_LSB GetSexColor( int sex, PRINTSYS_LSB color )
{

  if(sex==DPW_TR_GENDER_MALE){
    return sex_mark_col(0);
  }else if(sex==DPW_TR_GENDER_FEMALE){
    return sex_mark_col(1);
  }
  return color;
}

// 性別文字列のテーブル「きにしない・♂・♀・きにしない」
const int WorldTrade_SexStringTable[]={
  msg_gtc_11_001,
  msg_gtc_11_002,
  msg_gtc_11_003,
  msg_gtc_11_001,
  
};

//==============================================================================
/**
 * @brief   性別を表示する
 *
 * @param   win   書き込むBMPWIN
 * @param   msgman  メッセージマネージャー
 * @param   sex   性別
 * @param   flag  「きにしない」を表示するかどうか
 * @param   color カラー指定
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_SexPrint( GFL_BMPWIN *win, GFL_MSGDATA *msgman, int sex, int flag, int y, int printflag, PRINTSYS_LSB color, WT_PRINT *print )
{
  STRBUF *sexbuf;

  OS_Printf("SEX Print flag = %d, sex = %d\n", flag, sex);

  if(flag==0 && sex==DPW_TR_GENDER_NONE){
    // 性別無しを表示しなくてもいい時は表示しない
    return;
  }
  
  sexbuf = GFL_MSG_CreateString( msgman, WorldTrade_SexStringTable[sex]  );
  // flagが3以内の場合はセンタリング指定・以上の場合はＸ座標指定
  if(printflag > 3){
    WorldTrade_SysPrint( win, sexbuf,   printflag, y, 0, GetSexColor( sex, color ), print );
  }else{
    WorldTrade_SysPrint( win, sexbuf,   0, y, printflag, GetSexColor( sex, color ), print );
  }
  GFL_STR_DeleteBuffer(sexbuf);
}


//==============================================================================
/**
 * @brief   性別を表示する(MSG_NO_PUT版）
 *
 * @param   win   書き込みBMPWIN
 * @param   msgman  メッセージマネジャー
 * @param   sex   性別
 * @param   flag  未使用
 * @param   x   X位置
 * @param   y   Y位置
 * @param   color カラー指定
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_SexPrintNoPut( GFL_BMPWIN *win, GFL_MSGDATA *msgman, int sex, int flag, int x, int y, PRINTSYS_LSB color, WT_PRINT *print )
{
  STRBUF *sexbuf;

  OS_Printf("SEX Print flag = %d, sex = %d\n", flag, sex);

  if(flag==0 && sex==DPW_TR_GENDER_NONE){
    // 性別無しを表示しなくてもいい時は表示しない
    return;
  }
  
  sexbuf = GFL_MSG_CreateString( msgman, WorldTrade_SexStringTable[sex]  );
  // flagが3以内の場合はセンタリング指定・以上の場合はＸ座標指定
  GF_STR_PrintColor( win, FONT_SYSTEM, sexbuf, x, y, MSG_NO_PUT, GetSexColor( sex, color ),print);
  GFL_STR_DeleteBuffer(sexbuf);
}

//==============================================================================
/**
 * @brief   レベル条件の文字列描画を行う(互換性を保つためラップ版）
 *
 * @param   win   BMPWIN
 * @param   msgman  メッセージマネージャー  
 * @param   sex   レベル条件
 * @param   flag  転送フラグ  
 * @param   y       Y座標
 * @param   color 色指定  
 * @param   tbl_select  LEVEL_PRINT_TBL_DEPOSIT or LEVEL_PRINT_TBL_SEARCH
 *
 * @retval  none    
 */
//==============================================================================
void WorldTrade_WantLevelPrint( GFL_BMPWIN *win, GFL_MSGDATA *msgman, int level, int flag, int y, PRINTSYS_LSB color, int tbl_select, WT_PRINT *print )
{
  WorldTrade_WantLevelPrint_XY( win, msgman, level, flag, 0, y, color, tbl_select, print );

}


//==============================================================================
/**
 * @brief   XY指定ができるようになったレベル条件文字列描画
 *
 * @param   win     BMPWIN
 * @param   msgman    メッセージマネージャー
 * @param   level   レベル条件
 * @param   flag    転送フラグ
 * @param   x     X座標
 * @param   y     Y座標
 * @param   color   色指定
 * @param   tbl_select  LEVEL_PRINT_TBL_DEPOSIT or LEVEL_PRINT_TBL_SEARCH
 *
 * @retval  none    
 */
//==============================================================================
void WorldTrade_WantLevelPrint_XY(GFL_BMPWIN *win, GFL_MSGDATA *msgman, int level, int flag, int x, int y, PRINTSYS_LSB color, int tbl_select, WT_PRINT *print )
{
  STRBUF *levelbuf;
  const WT_LEVEL_TERM * level_tbl;
  
  if(tbl_select == LEVEL_PRINT_TBL_DEPOSIT){
    level_tbl = level_minmax_table;
  }
  else{ //LEVEL_PRINT_TBL_SEARCH
    level_tbl = search_level_minmax_table;
  }
  
  if(level!=-1){
    levelbuf = GFL_MSG_CreateString( msgman, level_tbl[level].msg );
    WorldTrade_SysPrint( win, levelbuf,  x, y, flag, color, print );
    GFL_STR_DeleteBuffer(levelbuf);
  }

}

//------------------------------------------------------------------
/**
 * @brief   交換して欲しいポケモンの条件を描画する
 *
 * @param   MsgManager    GTSメッセージマネージャー
 * @param   MonsNameManager ポケモンメッセージマネージャー
 * @param   WordSet     登録用WORDSET
 * @param   win[]     BMPWIN
 * @param   monsno      ポケモンNO
 * @param   sex       性別
 * @param   level     レベル
 *
 * @retval  none    
 */
//------------------------------------------------------------------
void WodrldTrade_PokeWantPrint( GFL_MSGDATA *MsgManager, GFL_MSGDATA *MonsNameManager,
  WORDSET *WordSet, GFL_BMPWIN *win[], int monsno, int sex, int level, WT_PRINT *print )
{
  STRBUF *strbuf;
  int i;

  BOOL is_sex_visible = TRUE;

  //「ほしいポケモン」描画
  strbuf = GFL_MSG_CreateString( MsgManager, msg_gtc_05_008 );
  WorldTrade_SysPrint( win[0], strbuf,    0, 0, 0, PRINTSYS_LSB_Make(1,2,0), print );

  // 名前・性別・レベルの欄はクリアする
  for(i=1;i<3;i++){
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[i]), 0x0000 );
  }

  // 名前
  WorldTrade_PokeNamePrint(win[1], MonsNameManager, monsno, 0, 0,PRINTSYS_LSB_Make(1,2,0), print );

  //ニドランチェック
  if( monsno == MONSNO_NIDORAN_F && sex == DPW_TR_GENDER_FEMALE )
  {
    is_sex_visible  = FALSE;
  }
  else if( monsno == MONSNO_NIDORAN_M && sex == DPW_TR_GENDER_MALE  )
  {
    is_sex_visible  = FALSE;
  }

  // 性別
  if( is_sex_visible && (sex==DPW_TR_GENDER_MALE || sex==DPW_TR_GENDER_FEMALE) ){
    OS_Printf("性別描画した %d\n", sex);
    WorldTrade_SexPrint( win[1], MsgManager, sex, 0, 0, 70, PRINTSYS_LSB_Make(1,2,0), print );
  }else{
    OS_Printf("性別描画してない %d\n");
  
  }

  // レベル指定
  WorldTrade_WantLevelPrint( win[2], MsgManager, level, 2, 0, PRINTSYS_LSB_Make(1,2,0),
    LEVEL_PRINT_TBL_DEPOSIT, print );
  
  

  GFL_STR_DeleteBuffer(strbuf);
}

//------------------------------------------------------------------
/**
 * @brief   交換して欲しいポケモンの条件を描画する
 *
 * @param   MsgManager    GTSメッセージマネージャー
 * @param   MonsNameManager ポケモン名メッセージマネージャー
 * @param   WordSet     登録WORDSET
 * @param   win[]     BMPWIN
 * @param   monsno      ポケモンNO
 * @param   sex       性別
 * @param   level     レベル
 *
 * @retval  none
 */
//------------------------------------------------------------------
void WodrldTrade_MyPokeWantPrint( GFL_MSGDATA *MsgManager, GFL_MSGDATA *MonsNameManager,
  WORDSET *WordSet, GFL_BMPWIN *win[], int monsno, int sex, int level, WT_PRINT *print )
{
  STRBUF *strbuf;
  int i;
  BOOL is_sex_visible = TRUE;


  //「ほしいポケモン」描画
  strbuf = GFL_MSG_CreateString( MsgManager, msg_gtc_05_008 );
  WorldTrade_SysPrint( win[0], strbuf,    0, 0, 0, PRINTSYS_LSB_Make(1,2,0), print );

  // 名前・性別・レベルの欄はクリアする
  for(i=1;i<3;i++){
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[i]), 0x0000 );
  }

  // 名前
  WorldTrade_PokeNamePrint(win[1], MonsNameManager, monsno, 0, 0,PRINTSYS_LSB_Make(1,2,0), print );

  //ニドランチェック
  if( monsno == MONSNO_NIDORAN_F && sex == DPW_TR_GENDER_FEMALE )
  {
    is_sex_visible  = FALSE;
  }
  else if( monsno == MONSNO_NIDORAN_M && sex == DPW_TR_GENDER_MALE  )
  {
    is_sex_visible  = FALSE;
  }

  // 性別
  if( is_sex_visible && (sex==DPW_TR_GENDER_MALE || sex==DPW_TR_GENDER_FEMALE ) ){
    OS_Printf("性別描画した %d\n", sex);
    WorldTrade_SexPrint( win[1], MsgManager, sex, 0, 0, 70, PRINTSYS_LSB_Make(1,2,0),print );
  }else{
    OS_Printf("性別描画してない\n");
  }

  // レベル指定
  WorldTrade_WantLevelPrint( win[2], MsgManager, level, 0, 0, PRINTSYS_LSB_Make(1,2,0),
    LEVEL_PRINT_TBL_DEPOSIT,print );
  
  

  GFL_STR_DeleteBuffer(strbuf);
}

//------------------------------------------------------------------
/**
 * @brief   今から預けようとするポケモンの情報を描画する
 *
 * @param   MsgManager  GTSメッセージマネージャー
 * @param   WordSet   WORDSET
 * @param   win[]   BMWPIN
 * @param   ppp     POKEMON_PASO_PARAM
 *
 * @retval  none    
 */
//------------------------------------------------------------------
static void PokeDepositPrint( 
  GFL_MSGDATA *MsgManager, 
  WORDSET *WordSet, 
  GFL_BMPWIN *win[], 
  POKEMON_PASO_PARAM *ppp,
  Dpw_Tr_PokemonDataSimple *post,
  WT_PRINT *print )
{
  STRBUF *strbuf, *levelbuf;
  STRBUF *namebuf = GFL_STR_CreateBuffer( MONS_NAME_SIZE+EOM_SIZE, HEAPID_WORLDTRADE );
  STRBUF *sexbuf = GFL_STR_CreateBuffer( MONS_NAME_SIZE+EOM_SIZE, HEAPID_WORLDTRADE );
  int sex, level,i;

  BOOL is_sex_visible;
  
  OS_TPrintf("deposit_ppp3 = %08x", ppp);
  
  // あずけるポケモン

  // 名前・性別・レベル取得
  PPP_Get(ppp, ID_PARA_nickname, namebuf );
  sex   = PPP_Get( ppp, ID_PARA_sex,   NULL )+1;
  level = PokePasoLevelCalc(ppp);

  is_sex_visible  = PPP_Get( ppp, ID_PARA_nidoran_nickname, NULL );
  
  OS_Printf("sex=%d, level=%d\n nidoran%d\n",sex, level, is_sex_visible);
  
  // 「あずけるポケモン」・性別アイコン文字列取得
  strbuf     = GFL_MSG_CreateString( MsgManager, msg_gtc_05_011  );
  WORDSET_RegisterNumber( WordSet, 3, level, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
  levelbuf = MSGDAT_UTIL_AllocExpandString( WordSet, MsgManager, msg_gtc_05_013_02, HEAPID_WORLDTRADE );
  if(sex!=DPW_TR_GENDER_NONE){
    GFL_MSG_GetString( MsgManager, WorldTrade_SexStringTable[sex], sexbuf );
  }

  // 描画の前にクリア
  for(i=0;i<3;i++){
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[i]), 0x0000 );
  }

  // 描画
  WorldTrade_SysPrint( win[0], strbuf,    0, 0, 0, PRINTSYS_LSB_Make(15,2,0), print );
  WorldTrade_SysPrint( win[1], namebuf,   0, 0, 0, PRINTSYS_LSB_Make(15,2,0), print );
  //WorldTrade_SysPrint( win[2], levelbuf,  0, 0, 0, PRINTSYS_LSB_Make(15,2,0) );
  WorldTrade_SysPrint( win[2], levelbuf,  0, 0, 2, PRINTSYS_LSB_Make(15,2,0), print );
  if(sex!=DPW_TR_GENDER_NONE && is_sex_visible){
    WorldTrade_SysPrint( win[1], sexbuf,   70, 0, 0, sex_mark_col(sex-1), print );
  }

  post->characterNo = PPP_Get(ppp, ID_PARA_monsno, NULL);
  post->gender      = sex;
  post->level       = level;

  GFL_STR_DeleteBuffer( levelbuf );
  GFL_STR_DeleteBuffer( sexbuf   );
  GFL_STR_DeleteBuffer( namebuf  );
  GFL_STR_DeleteBuffer( strbuf   );
}

#define ZKN_SORTDATA_ONESIZE  ( sizeof(u16) )

//------------------------------------------------------------------
/**
 * @brief   ずかんソートデータ取得（あういえお順）。解放はじぶんで。
 *      
 * @param   heap    ヒープID
 * @param   idx     インデックス
 * @param   p_arry_num  通常図鑑と全国図鑑の総数
 *
 * @retval  u16*    読み込んだソートデータ
 */
//------------------------------------------------------------------
u16* WorldTrade_ZukanSortDataGet( int heap, int idx, int* p_arry_num )
{
  u32 size;
  u16* p_buf;
  // 読み込み
  p_buf = GFL_ARC_UTIL_LoadEx( ARCID_ZUKAN_DATA, NARC_zukan_data_zkn_sort_aiueo_dat, FALSE, heap, &size );

  *p_arry_num = size / ZKN_SORTDATA_ONESIZE;

  return p_buf;
  // ポケモンの名前を取得する
  //STRBUF* MSGDAT_UTIL_GetMonsName( u32 monsno, u32 heapID )
}

//----------------------------------------------------------------------------
/**
 *  @brief  上記図鑑データで、頭文字を指定したときの開始から終了インデックスを取得する
 *
 *  @param  int select  頭文字インデックス
 *  @param  *start      開始受け取り
 *  @param  *end        終了受け取り
 */
//-----------------------------------------------------------------------------
void WorldTrade_ZukanSortDataGetOnlyIndex( int select, int *start, int *end )
{ 
  static const u32 ZukanSortHiraTable[]={
    ZKN_SORT_AIUEO_IDX_START_A,   // あ
    ZKN_SORT_AIUEO_IDX_START_I,
    ZKN_SORT_AIUEO_IDX_START_U,
    ZKN_SORT_AIUEO_IDX_START_E,
    ZKN_SORT_AIUEO_IDX_START_O,
    ZKN_SORT_AIUEO_IDX_START_KA , // か
    ZKN_SORT_AIUEO_IDX_START_KI ,
    ZKN_SORT_AIUEO_IDX_START_KU ,
    ZKN_SORT_AIUEO_IDX_START_KE ,
    ZKN_SORT_AIUEO_IDX_START_KO ,
    ZKN_SORT_AIUEO_IDX_START_SA , // さ
    ZKN_SORT_AIUEO_IDX_START_SI ,
    ZKN_SORT_AIUEO_IDX_START_SU ,
    ZKN_SORT_AIUEO_IDX_START_SE ,
    ZKN_SORT_AIUEO_IDX_START_SO ,
    ZKN_SORT_AIUEO_IDX_START_TA , // た
    ZKN_SORT_AIUEO_IDX_START_TI ,
    ZKN_SORT_AIUEO_IDX_START_TU ,
    ZKN_SORT_AIUEO_IDX_START_TE ,
    ZKN_SORT_AIUEO_IDX_START_TO ,
    ZKN_SORT_AIUEO_IDX_START_NA , // な
    ZKN_SORT_AIUEO_IDX_START_NI ,
    ZKN_SORT_AIUEO_IDX_START_NU ,
    ZKN_SORT_AIUEO_IDX_START_NE ,
    ZKN_SORT_AIUEO_IDX_START_NO ,
    ZKN_SORT_AIUEO_IDX_START_HA , // は
    ZKN_SORT_AIUEO_IDX_START_HI ,
    ZKN_SORT_AIUEO_IDX_START_HU ,
    ZKN_SORT_AIUEO_IDX_START_HE ,
    ZKN_SORT_AIUEO_IDX_START_HO ,
    ZKN_SORT_AIUEO_IDX_START_MA , // ま
    ZKN_SORT_AIUEO_IDX_START_MI ,
    ZKN_SORT_AIUEO_IDX_START_MU ,
    ZKN_SORT_AIUEO_IDX_START_ME ,
    ZKN_SORT_AIUEO_IDX_START_MO ,
    ZKN_SORT_AIUEO_IDX_START_YA , // や
    ZKN_SORT_AIUEO_IDX_START_YU ,
    ZKN_SORT_AIUEO_IDX_START_YO ,
    ZKN_SORT_AIUEO_IDX_START_RA , // ら
    ZKN_SORT_AIUEO_IDX_START_RI ,
    ZKN_SORT_AIUEO_IDX_START_RU ,
    ZKN_SORT_AIUEO_IDX_START_RE ,
    ZKN_SORT_AIUEO_IDX_START_RO ,
    ZKN_SORT_AIUEO_IDX_START_WA , // わ
    ZKN_SORT_AIUEO_IDX_ALL_END,   //終端
  };
  GF_ASSERT_HEAVY( select < NELEMS(ZukanSortHiraTable) );

  *start  = ZukanSortHiraTable[ select ];
  *end    = ZukanSortHiraTable[ select + 1 ];
}

//==============================================================================
/**
 * @brief   シンオウ図鑑データを読み込む      解放はじぶんで
 *      シンオウ図鑑の順に開発NOが入っているので、
 *      新たにテーブルを確保して「この開発NOはシンオウか？」の作成もする
 *
 * @param   heap    ヒープID
 * @param   p_arry_num  MONS_END+1
 *
 * @retval  u16 *   494個分のシンオウ図鑑かどうかのフラグが入ったテーブル
 */
//==============================================================================
u8 *WorldTrade_SinouZukanDataGet( int heap  )
{
  u32 size,num,i;
  u16* p_buf;

  // テーブル確保・初期化
  u8 *sinouData = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, MONSNO_END+1 );
  MI_CpuClearFast( sinouData, MONSNO_END+1 );

  // シンオウ図鑑テーブル
  p_buf = GFL_ARC_UTIL_LoadEx( ARCID_ZUKAN_DATA, NARC_zukan_data_zkn_sort_chihou_dat, FALSE, heap, &size );
  
  num = size / ZKN_SORTDATA_ONESIZE;

  // フラグテーブルに作り直す
  for(i=0;i<num;i++){
    if(p_buf[i]<MONSNO_END+1){
      sinouData[p_buf[i]] = 1;
    }
  }

  GFL_HEAP_FreeMemory(p_buf);

  return sinouData;
}


//==============================================================================
/**
 * @brief   サーバーに送信する基本情報を格納する
 *
 * @param   dtd   GTS用ポケモン基本情報
 * @param   wk    GTS画面ワーク
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_PostPokemonBaseDataMake( Dpw_Tr_Data *dtd, WORLDTRADE_WORK *wk )
{
  // ポケモン構造体を代入
  
  if(WorldTrade_GetPPorPPP( wk->BoxTrayNo )){
    //特定のポケモンフォルムNoをノーマルにする
    switch( PPP_Get(wk->deposit_ppp, ID_PARA_monsno, NULL) )
    {
    case MONSNO_653:  //ライ
    case MONSNO_SHEIMI://シェイミ
      PP_ChangeFormNo((POKEMON_PARAM*)(wk->deposit_ppp), 0 );
      break;
    }
    // POKEMON_PARAMとしてコピー
    MI_CpuCopyFast( wk->deposit_ppp, dtd->postData.data,  POKETOOL_GetWorkSize() );
  }else{
    //特定のポケモンはフォルムNoをノーマルにする
    switch( PPP_Get(wk->deposit_ppp, ID_PARA_monsno, NULL) )
    {
    case MONSNO_653:  //ライ
    case MONSNO_SHEIMI://シェイミ
      PPP_ChangeFormNo(wk->deposit_ppp, FORMNO_SHEIMI_LAND);
    }
    // POKEMON_PASO_PARAMなのでReplace関数でPOKEMON_PARAMに肉付けする
    PokeReplace(wk->deposit_ppp,(POKEMON_PARAM *)dtd->postData.data);
  }

  // トレーナー名コピー
  STRTOOL_Copy( MyStatus_GetMyName(wk->param->mystatus), dtd->name, DPW_TR_NAME_SIZE );

  // ID
  dtd->trainerID   = MyStatus_GetID( wk->param->mystatus );

  // 国・地域
  dtd->countryCode = MyStatus_GetMyNation( wk->param->mystatus );
  dtd->localCode   = MyStatus_GetMyArea( wk->param->mystatus );

  // 見た目
  dtd->trainerType = MyStatus_GetTrainerView( wk->param->mystatus );

  // 性別
  dtd->gender      = MyStatus_GetMySex( wk->param->mystatus );

  // バージョン・国コード
  dtd->versionCode = GET_VERSION();
  dtd->langCode    = PM_LANG;

  //国連用データ
  dtd->favorite     = WIFIHISTORY_GetMyFavorite(wk->param->wifihistory);   //趣味
  dtd->countryCount = WIFIHISTORY_GetMyCountryCount(wk->param->wifihistory) ;  //交換した国の回数
  dtd->nature       = WIFIHISTORY_GetMyNature(wk->param->wifihistory);   //性格
}


//------------------------------------------------------------------
/**
 * @brief   ポケモンを預けるのに必要なデータを作成する
 *
 * @param   dtd   GTS用ポケモン基本情報
 * @param   wk    GTS画面ワーク
 *
 * @retval  none  
 */
//------------------------------------------------------------------
static void DepositPokemonDataMake( Dpw_Tr_Data *dtd, WORLDTRADE_WORK *wk )
{

  // 基本情報を格納（ポケモン・トレーナー名・国・地域・見た目など）
  WorldTrade_PostPokemonBaseDataMake( dtd, wk );
  
  // ほしいポケモン情報を格納
  dtd->postSimple = wk->Post;
  dtd->wantSimple = wk->Want;

  OS_Printf( "postData  No = %d,  gender = %d, level min= %d\n", wk->Post.characterNo, wk->Post.gender, wk->Post.level);
  OS_Printf( "WantData  No = %d,  gender = %d, level min= %d\n", wk->Want.characterNo, wk->Want.gender, wk->Want.level_min);


  OS_Printf( "TrainerId = %05d,  nation = %d, area = %d\n", dtd->trainerID, dtd->countryCode, dtd->localCode);
  OS_Printf( "Version   = %d,  language = %d, \n", dtd->versionCode, dtd->langCode);


}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// BMPLIST関係
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define DEPOSIT_HEADWORD_NUM  ( 10 )
#define DEPOSIT_Y_NUM     (  6 )

///BMPLIST用定義
static const BMPMENULIST_HEADER MenuListHeader = {
    NULL,     // 表示文字データポインタ
    NULL,         // カーソル移動ごとのコールバック関数
    NULL,         // 一列表示ごとのコールバック関数
    NULL,         // 
    DEPOSIT_HEADWORD_NUM, // リスト項目数
    DEPOSIT_Y_NUM,      // 表示最大項目数
    4,            // ラベル表示Ｘ座標
    8,            // 項目表示Ｘ座標
    0,            // カーソル表示Ｘ座標
    2,            // 表示Ｙ座標
    FBMP_COL_BLACK,     // 文字色
    FBMP_COL_WHITE,     // 背景色
    FBMP_COL_BLK_SDW,   // 文字影色
    0,            // 文字間隔Ｘ
    16,           // 文字間隔Ｙ
    BMPMENULIST_LRKEY_SKIP,   // ページスキップタイプ
    FONT_SYSTEM,      // 文字指定
    0,            // ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)
    NULL,                   // ワーク
};



//==============================================================================
/**
 * @brief   頭文字選択メニュー作成
 *
 * @param  none
 *
 * @retval  none    
 */
//==============================================================================
BMPMENULIST_WORK *WorldTrade_WordheadBmpListMake( WORLDTRADE_WORK *wk, BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, GFL_MSGDATA *MsgManager )
{
  BMPMENULIST_HEADER list_h;
  int i;

  *menulist = BmpMenuWork_ListCreate( 10, HEAPID_WORLDTRADE );
  for(i=0;i<10;i++){
    BmpMenuWork_ListAddArchiveString( *menulist, MsgManager, msg_gtc_10_001+i, i+1 , HEAPID_WORLDTRADE );
  }

    list_h       = MenuListHeader;
    list_h.list  = *menulist;
    list_h.win   = win;

  // 枠描画
  BmpWinFrame_Write( win, WINDOW_TRANS_ON, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

  // BMPメニュー開始
    return BmpMenuList_Set(&list_h, wk->dw->headwordListPos, wk->dw->headwordPos, HEAPID_WORLDTRADE);

}

// 頭文字テーブル（ソートされたポケモンの何番目に「ア・カ・サ…」が登場するか？
static u16 NameHeadTable[]={
  ZKN_SORT_AIUEO_IDX_START_A,
  ZKN_SORT_AIUEO_IDX_START_KA,
  ZKN_SORT_AIUEO_IDX_START_SA,
  ZKN_SORT_AIUEO_IDX_START_TA,
  ZKN_SORT_AIUEO_IDX_START_NA,
  ZKN_SORT_AIUEO_IDX_START_HA,
  ZKN_SORT_AIUEO_IDX_START_MA,
  ZKN_SORT_AIUEO_IDX_START_YA,
  ZKN_SORT_AIUEO_IDX_START_RA,
  ZKN_SORT_AIUEO_IDX_START_WA,
  ZKN_SORT_AIUEO_IDX_ALL_END,
  0,
};

//------------------------------------------------------------------
/**
 * @brief   ポケモン名、５０音順ソートリスト作成
 *
 * @param   menulist  リストデータ（ここに登録する）
 * @param   msgman    ポケモン名メッセージデータ
 * @param   table   ５０音順モンスターナンバーテーブル
 * @param   num     ポケモンの総数
 * @param   select    アカサタナハマヤラワのどれを選択したか？（ア: 0〜9まで）
 *
 * @retval  int 図鑑総数
 */
//------------------------------------------------------------------
static int PokeNameSortListMake( BMP_MENULIST_DATA **menulist, GFL_MSGDATA *monsnameman, 
                  GFL_MSGDATA *msgman,u16 *table, u8 *sinou,
                  int num, int select,ZUKAN_SAVEDATA *zukan )
{
  int i,index,see_count=0;
  int pokenum = NameHeadTable[select+1]-NameHeadTable[select];
  int flag    = TRUE; //ZUKANSAVE_GetZenkokuZukanFlag(zukan); 全国図鑑になっているかは関係なくなった 2008.06.23(月) matsuda

  OS_TPrintf("select = %d, num = %d\n",select, pokenum);
  
  // 図鑑と比べて何体見ているかを確認
  index = NameHeadTable[select];
  for(i=0;i<pokenum;i++){
    // 全国図鑑か？
    if(flag){
      if(ZUKANSAVE_GetPokeSeeFlag( zukan, table[index+i] )){
        OS_Printf(" SeeCheck i = %d, table[index+i] = %d\n", i, table[index+i]);
        see_count++;
      }
    }else{
      // シンオウ図鑑にいることを確認した上で図鑑チェック
      if(sinou[table[index+i]]){
        if(ZUKANSAVE_GetPokeSeeFlag( zukan, table[index+i] )){
          see_count++;
        }
      }
    
    }
  }

  // 項目リストの作成
  *menulist = BmpMenuWork_ListCreate( see_count+1, HEAPID_WORLDTRADE );


  // ポケモン名の登録
  for(i=0;i<pokenum;i++){
    // 全国図鑑か？
    if(flag){
      if(ZUKANSAVE_GetPokeSeeFlag( zukan, table[index+i] )){
        OS_Printf(" ListAdd i = %d, table[index+i] = %d\n", i, table[index+i]);
        BmpMenuWork_ListAddArchiveString( *menulist, monsnameman, table[index+i], table[index+i], HEAPID_WORLDTRADE  );
      }
    }else{
      // シンオウ図鑑にいることを確認した上で図鑑チェック
      if(sinou[table[index+i]]){
        if(ZUKANSAVE_GetPokeSeeFlag( zukan, table[index+i] )){
          BmpMenuWork_ListAddArchiveString( *menulist, monsnameman, table[index+i], table[index+i], HEAPID_WORLDTRADE  );
        }
      }
    }
  }
  // 「もどる」を登録
  BmpMenuWork_ListAddArchiveString( *menulist, msgman, msg_gtc_11_004, BMPMENU_CANCEL, HEAPID_WORLDTRADE  );

  return see_count+1;
}



//==============================================================================
/**
 * @brief   ポケモン名選択リスト作成
 *
 * @param   menulist    
 * @param   win       
 * @param   MsgManager    
 * @param   MonsNameManager 
 * @param   dw        
 *
 * @retval  BMPMENULIST_WORK *  
 */
//==============================================================================
//=============================================================================================
/**
 * @brief ポケモン名選択リスト作成
 *
 * @param   wk        GTS画面ワーク
 * @param   menulist    メニューリスト書き込みバッファ
 * @param   win       BMPWIN
 * @param   MsgManager    GTSメッセージマネージャー
 * @param   MonsNameManager ポケモン名メッセージマネージャー
 * @param   dw        預ける用ポケモンデータ
 * @param   zukan     図鑑データ
 *
 * @retval  BMPMENULIST_WORK *  BMPリストワーク
 */
//=============================================================================================
BMPMENULIST_WORK *WorldTrade_PokeNameListMake( WORLDTRADE_WORK *wk, BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, 
      GFL_MSGDATA *MsgManager, GFL_MSGDATA *MonsNameManager, DEPOSIT_WORK* dw, ZUKAN_SAVEDATA *zukan)
{
  BMPMENULIST_HEADER list_h;
  int i,listnum,head;

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win), 0x0f0f );

  head = dw->headwordListPos+dw->headwordPos;
  listnum = PokeNameSortListMake( menulist, MonsNameManager, MsgManager, 
                  dw->nameSortTable, dw->sinouTable, 
                  dw->nameSortNum, head, zukan );
  
  
    list_h       = MenuListHeader;
    list_h.count = listnum;
    list_h.list  = *menulist;
    list_h.win   = win;

  // 枠描画
  BmpWinFrame_Write( win, WINDOW_TRANS_ON, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

  // BMPメニュー開始
    return BmpMenuList_Set(&list_h, wk->selectListPos.name_list[head],
                   wk->selectListPos.name_pos[head], HEAPID_WORLDTRADE);

}


// 選択リスト用性別アイコン列
static sex_select_table[][2]={
  {msg_gtc_11_001, PARA_UNK,},
  {msg_gtc_11_002, PARA_MALE,},
  {msg_gtc_11_003, PARA_FEMALE,} ,
  {msg_gtc_11_004, BMPMENU_CANCEL},
};

//==============================================================================
/**
 * @brief   ポケモン性別選択リスト作成
 *
 * @param   menulist  メニューリストが書き込まれる
 * @param   win     BMPWIN
 * @param   MsgManager  GTSメッセージマネージャー
 *
 * @retval  BMPLIST_MENU *    BMPリストワーク
 */
//==============================================================================
BMPMENULIST_WORK *WorldTrade_SexSelectListMake( BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, GFL_MSGDATA *MsgManager  )
{
  BMPMENULIST_HEADER list_h;
  int i;

  *menulist = BmpMenuWork_ListCreate( 4, HEAPID_WORLDTRADE );
  for(i=0;i<4;i++){
    BmpMenuWork_ListAddArchiveString( *menulist, MsgManager, sex_select_table[i][0], sex_select_table[i][1], HEAPID_WORLDTRADE  );
  }

    list_h       = MenuListHeader;
    list_h.count = 4;
    list_h.list  = *menulist;
    list_h.win   = win;

  // 枠描画
  BmpWinFrame_Write( win, WINDOW_TRANS_ON, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

  // BMPメニュー開始
    return BmpMenuList_Set(&list_h, 0, 0, HEAPID_WORLDTRADE);
  
}



//------------------------------------------------------------------
/**
 * @brief   レベル条件用BMP_MENULIST_DATAを作成
 *
 * @param   menulist  BMPMENU項目を追加するポインタ
 * @param   MsgManager  メッセージマネージャー
 * @param   tbl_select  検索の時と預ける時でレベル選択の文字列が違う(LEVEL_PRINT_TBL_DEPOSIT or LEVEL_PRINT_TBL_SEARCH)
 *
 * @retval  none    
 */
//------------------------------------------------------------------
int WorldTrade_LevelListAdd( BMP_MENULIST_DATA **menulist, GFL_MSGDATA *MsgManager, int tbl_select)
{
  int i;
  const WT_LEVEL_TERM * level_tbl;
  int select_num;
  
  if(tbl_select == LEVEL_PRINT_TBL_DEPOSIT){
    level_tbl = level_minmax_table;
    select_num = LEVEL_SELECT_NUM;
  }
  else{ //LEVEL_PRINT_TBL_SEARCH
    level_tbl = search_level_minmax_table;
    select_num = SEARCH_LEVEL_SELECT_NUM;
  }

  *menulist = BmpMenuWork_ListCreate( select_num, HEAPID_WORLDTRADE );
  for(i=0;i<select_num;i++){
    BmpMenuWork_ListAddArchiveString( *menulist, MsgManager, level_tbl[i].msg, i, HEAPID_WORLDTRADE  );
  }

  return select_num;
}

//==============================================================================
/**
 * @brief   レベル選択リスト作成
 *
 * @param   menulist  書き込むメニューバッファ
 * @param   win     BMPWIN
 * @param   MsgManager  GTSメッセージマネージャー
 * @param   tbl_select  LEVEL_PRINT_TBL_DEPOSIT or LEVEL_PRINT_TBL_SEARCH
 *
 * @retval  BMPMENULIST_WORK *    
 */
//==============================================================================
BMPMENULIST_WORK *WorldTrade_LevelListMake(BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, GFL_MSGDATA *MsgManager, int tbl_select  )
{
  BMPMENULIST_HEADER list_h;
  int select_num;

  select_num   = WorldTrade_LevelListAdd( menulist, MsgManager, tbl_select );
  
    list_h       = MenuListHeader;
    list_h.count = select_num;
    list_h.list  = *menulist;
    list_h.win   = win;

  // 枠描画
  BmpWinFrame_Write( win, WINDOW_TRANS_ON, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

  // BMPメニュー開始
    return BmpMenuList_Set(&list_h, 0, 0, HEAPID_WORLDTRADE);

}


//==============================================================================
/**
 * @brief   レベル指定条件を代入する
 *
 * @param   dtps    検索条件構造体
 * @param   index   検索条件の添え字
 * @param   tbl_select  LEVEL_PRINT_TBL_DEPOSIT or LEVEL_PRINT_TBL_SEARCH
 *
 * @retval  none    
 */
//==============================================================================
void WorldTrade_LevelMinMaxSet( Dpw_Tr_PokemonSearchData *dtps, int index, int tbl_select )
{
  const WT_LEVEL_TERM * level_tbl;

  if(tbl_select == LEVEL_PRINT_TBL_DEPOSIT){
    level_tbl = level_minmax_table;
    GF_ASSERT_HEAVY(index<(LEVEL_SELECT_NUM));// レベル指定の配列より上だった
  }
  else{ //LEVEL_PRINT_TBL_SEARCH
    level_tbl = search_level_minmax_table;
    GF_ASSERT_HEAVY(index<(SEARCH_LEVEL_SELECT_NUM));// レベル指定の配列より上だった
  }
  
  dtps->level_min = level_tbl[index].min;
  dtps->level_max = level_tbl[index].max;
}

//==============================================================================
/**
 * @brief   指定のレベル条件のインデックスを取得する
 *
 * @param   min   最低
 * @param   max   最大レベル
 * @param   tbl_select  LEVEL_PRINT_TBL_DEPOSIT or LEVEL_PRINT_TBL_SEARCH
 *
 * @retval  int   取得したファイルインデックス
 */
//==============================================================================
int WorldTrade_LevelTermGet( int min, int max, int tbl_select )
{
  int i;
  int select_num;
  const WT_LEVEL_TERM * level_tbl;
  
  if(tbl_select == LEVEL_PRINT_TBL_DEPOSIT){
    level_tbl = level_minmax_table;
    select_num = LEVEL_SELECT_NUM;
  }
  else{ //LEVEL_PRINT_TBL_SEARCH
    level_tbl = search_level_minmax_table;
    select_num = SEARCH_LEVEL_SELECT_NUM;
  }
  
  for(i=0;i<select_num;i++){
    if(level_tbl[i].min==min && level_tbl[i].max==max){
      return i;
    }
  }

  // レベル条件が合わなかったからといってASSERTにすると、後々が怖いので
  // 「なんでもあり」にしておく
  return 0;
}


//==============================================================================
/**
 * @brief   国選択リスト作成
 *
 * @param   menulist  書き込むメニューワーク
 * @param   win     BMPWIN
 * @param   MsgManager  GTSメッセージマネージャー
 *
 * @retval  BMPMENULIST_WORK *  生成したＢＭＰリストワーク
 */
//==============================================================================
BMPMENULIST_WORK *WorldTrade_CountryListMake(BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, GFL_MSGDATA *CountryMsgManager, GFL_MSGDATA *NormalMsgManager)
{
  BMPMENULIST_HEADER list_h;
  int i;
  int list_num = NELEMS(CountryListTbl) + 2;  // +2 = きにしない　もどる
  
  *menulist = BmpMenuWork_ListCreate( list_num, HEAPID_WORLDTRADE );

  //「きにしない」
  BmpMenuWork_ListAddArchiveString( *menulist, NormalMsgManager, msg_gtc_search_015, 0, HEAPID_WORLDTRADE  );
  
  //国名登録
  for(i=1;i<list_num-1;i++){
    BmpMenuWork_ListAddArchiveString( *menulist, CountryMsgManager, CountryListTbl[i-1], i, HEAPID_WORLDTRADE  );
  }

  //「もどる」
  BmpMenuWork_ListAddArchiveString( *menulist, NormalMsgManager, msg_gtc_search_016, list_num-1, HEAPID_WORLDTRADE  );

    list_h       = MenuListHeader;
    list_h.count = list_num;
    list_h.list  = *menulist;
    list_h.win   = win;

  // 枠描画
  BmpWinFrame_Write( win, WINDOW_TRANS_OFF, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

  // BMPメニュー開始
    return BmpMenuList_Set(&list_h, 0, 0, HEAPID_WORLDTRADE);

}

//==============================================================================
/**
 * @brief   国指定条件を代入する
 *
 * @param   dtps      検索条件構造体
 * @param   country_code  国コード
 *
 * @retval  none    
 */
//==============================================================================
void WorldTrade_CountryCodeSet( WORLDTRADE_WORK *wk, int country_code )
{
  if(country_code == 0){
    wk->country_code = 0;
  }
  else if(country_code - 1 < NELEMS(CountryListTbl)){
    wk->country_code = CountryListTbl[country_code - 1];
  }
}

//==============================================================================
/**
 * @brief   指定の頭文字の国名はいくつあるか？
 *
 * @param   start 「アイウ…」などを表す頭文字
 *
 * @retval  int   国名の個数
 */
//==============================================================================
int WorldTrade_NationSortListNumGet( int start, int *number )
{
  int i,count=0;
  
  for(i=0,count=0;i<COUNTRY_SORT_HEAD_MAX;i++){
    if(CountrySortList[i][1]==start){
      *number = CountrySortList[i][0];
      return CountrySortList[i+1][0]-CountrySortList[i][0];
    }
  }
  return count;
}

//==============================================================================
/**
 * @brief   頭文字を渡して国名リストを作成する
 *
 * @param   menulist      国名を格納するBMPMENULIST_DATA
 * @param   CountryNameManager  メッセージデータマネージャー
 * @param   start       頭文字(ア=０,イ＝１、カ＝５、…）
 *
 * @retval  int   該当する国名の数
 */
//==============================================================================
int WorldTrade_NationSortListMake( BMP_MENULIST_DATA **menulist, GFL_MSGDATA *CountryNameManager, int start )
{
  int i,count, head;
  
  // 指定の国名はいくつあるか取得
  count = WorldTrade_NationSortListNumGet( start, &head );

  OS_Printf("country start=%d, num=%d\n", head, count);

  // 項目リストの作成
  *menulist = BmpMenuWork_ListCreate( count, HEAPID_WORLDTRADE );
  
  // BMPMENULIST_DATA作成
  for(i=0;i<count;i++){
    BmpMenuWork_ListAddArchiveString( *menulist, CountryNameManager, CountryListTbl[head+i], head+i+1, HEAPID_WORLDTRADE  );
  }
  
  return count;
}



//==============================================================================
/**
 * @brief   効果音に対応したBMPLISTMAIN関数
 *
 * @param   lw      リストワーク
 * @param   posbackup カーソル位置
 *
 * @retval  u32     選択結果
 */
//==============================================================================
u32 WorldTrade_BmpMenuList_Main( BMPMENULIST_WORK * lw, u16 *posbackup )
{
  u16 listpos;
  u32 result = BmpMenuList_Main( lw );

  BmpMenuList_DirectPosGet( lw, &listpos );

  if(*posbackup != listpos){
    if(*posbackup!=0xffff){
      PMSND_PlaySE(WORLDTRADE_MOVE_SE);
    }
    *posbackup = listpos;
    
  }

  return result;
}

//==============================================================================
/**
 * @brief   選択位置カーソル情報のクリア
 *
 * @param   slp   
 *
 * @retval  none    
 */
//==============================================================================
void WorldTrade_SelectListPosInit( SELECT_LIST_POS *slp)
{
  int i;
  
  for(i=0;i<NAME_HEAD_MAX;i++){
    slp->name_list[i] = 0;
    slp->name_pos[i]  = 0;
  }
  
  slp->head_list = 0;
  slp->head_pos  = 0;

  
}


//=============================================================================================
/**
 * @brief カーソル位置保存
 *
 * @param   slp   リストカーソル位置
 * @param   head  リスト開始位置
 * @param   list  名前リストの番号
 * @param   pos   ページ上の位置
 */
//=============================================================================================
void WorldTrade_SelectNameListBackup( SELECT_LIST_POS *slp, int head, int list, int pos )
{
  slp->name_list[head] = list;
  slp->name_pos[head]  = pos;
}

