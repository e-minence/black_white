//============================================================================================
/**
 * @file    d_nakahiro.c
 * @brief   中村用デバッグ処理
 * @author  Hiroyuki Nakamura
 * @date    2009.10.02
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "system/wipe.h"
#include "system/bmp_menulist.h"
#include "gamesystem/game_data.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"
#include "item/item.h"
#include "font/font.naix"

#include "app/box2.h"
#include "app/zukan.h"
#include "app/wifi_note.h"
#include "app/dendou_pc.h"
#include "demo/command_demo.h"
#include "demo/dendou_demo.h"
#include "demo/staff_roll.h"
#include "../../battle/app/vs_multi_list.h"

#include "arc_def.h"  //ARCID_MESSAGE
#include "debug_message.naix"
#include "msg/debug/msg_d_nakahiro.h"

#include "pokeicon_view.h"

#ifdef PM_DEBUG
//============================================================================================
//  定数定義
//============================================================================================
#define TOP_MENU_SIZ  ( 16 )

typedef struct {
  u32 main_seq;
  u32 next_seq;
  HEAPID  heapID;

  GFL_FONT * font;          // 通常フォント
  GFL_MSGDATA * mman;       // メッセージデータマネージャ
  PRINT_QUE * que;          // プリントキュー
  GFL_BMPWIN * win;         // BMPWIN
  PRINT_UTIL  util;

  BMP_MENULIST_DATA * ld;
  BMPMENULIST_WORK * lw;

  GAMEDATA * gamedata;

  BOX2_GFL_PROC_PARAM box_data;
  void * bb_party;
  u16 sleepTable[MONSNO_MAX+8];

  COMMANDDEMO_DATA  cdemo_data;

  ZUKAN_PARAM zkn_data;

  VS_MULTI_LIST_PARAM vsl_data;
  POKEPARTY * ppL;
  POKEPARTY * ppR;

  WIFINOTE_PROC_PARAM wifi_note_data;

  DENDOUPC_PARAM  dpc_data;
  DENDOUDEMO_PARAM  ddemo_data;

  STAFFROLL_DATA  sr_data;

}NAKAHIRO_MAIN_WORK;

enum {
  MAIN_SEQ_INIT = 0,
  MAIN_SEQ_MAIN,
  MAIN_SEQ_FADE_MAIN,

  MAIN_SEQ_BOX_CALL1,
  MAIN_SEQ_BOX_CALL2,
  MAIN_SEQ_BOX_CALL3,
  MAIN_SEQ_BOX_CALL4,
  MAIN_SEQ_BOX_CALL5,
  MAIN_SEQ_BOX_CALL6,

  MAIN_SEQ_MOVIE_CALL,

  MAIN_SEQ_ZUKAN_CALL,
  MAIN_SEQ_ZKNLIST_CALL,

  MAIN_SEQ_VSMLIST_L,
  MAIN_SEQ_VSMLIST_R,

  MAIN_SEQ_WIFI_NOTE,

  MAIN_SEQ_DENDOU_PC,
  MAIN_SEQ_DENDOU_DEMO,

  MAIN_SEQ_STAFF_ROLL,

  MAIN_SEQ_POKEICON,

  MAIN_SEQ_END,
};


//============================================================================================
//  プロトタイプ宣言
//============================================================================================
static GFL_PROC_RESULT MainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT MainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT MainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

static void FadeInSet( NAKAHIRO_MAIN_WORK * wk, u32 next );
static void FadeOutSet( NAKAHIRO_MAIN_WORK * wk, u32 next );

static void SetBoxPoke( NAKAHIRO_MAIN_WORK * wk );
static void SetPartyPoke( NAKAHIRO_MAIN_WORK * wk );

static void SetPokeParty( NAKAHIRO_MAIN_WORK * wk, POKEPARTY * party, const u16 * mons, const u16 * item );

static void BgInit( NAKAHIRO_MAIN_WORK * wk );
static void BgExit(void);

static void TopMenuInit( NAKAHIRO_MAIN_WORK * wk );
static void TopMenuExit( NAKAHIRO_MAIN_WORK * wk );

static void SetZukanDataOne( NAKAHIRO_MAIN_WORK * wk, u16 mons, u32 mode );
static void SetZukanData( NAKAHIRO_MAIN_WORK * wk, u32 mode );
static void SetZukanPacchiiru( NAKAHIRO_MAIN_WORK * wk );


//============================================================================================
//  グローバル
//============================================================================================
const GFL_PROC_DATA DebugNakahiroMainProcData = {
  MainProcInit,
  MainProcMain,
  MainProcEnd,
};

static const BMPMENULIST_HEADER TopMenuListH = {
  NULL, NULL, NULL, NULL,
  TOP_MENU_SIZ,   // リスト項目数
  6,    // 表示最大項目数
  0,    // ラベル表示Ｘ座標
  12,   // 項目表示Ｘ座標
  0,    // カーソル表示Ｘ座標
  0,    // 表示Ｙ座標
  1,    // 表示文字色
  15,   // 表示背景色
  2,    // 表示文字影色
  0,    // 文字間隔Ｘ
  16,   // 文字間隔Ｙ
  BMPMENULIST_LRKEY_SKIP,   // ページスキップタイプ
  0,    // 文字指定(本来はu8だけどそんなに作らないと思うので)
  0,    // ＢＧカーソル(allow)表示フラグ(0:ON,1:OFF)

  NULL,
  
  12,     // 文字サイズX(ドット
  16,     // 文字サイズY(ドット
  NULL,   // 表示に使用するメッセージバッファ
  NULL,   // 表示に使用するプリントユーティリティ
  NULL,   // 表示に使用するプリントキュー
  NULL,   // 表示に使用するフォントハンドル
};

static const u16 VSMListMonsL[] = {
  MONSNO_HUSIGIBANA,
  MONSNO_RIZAADON,
  MONSNO_KAMEKKUSU,
  MONSNO_ZYUKAIN,
//  MONSNO_BASYAAMO,
  MONSNO_SHEIMI,
//  MONSNO_RAGURAAZI,
  MONSNO_HOUOU,
  0,
};
static const u16 VSMListMonsR[] = {
  MONSNO_KIMORI,
  MONSNO_ATYAMO,
  MONSNO_MIZUGOROU,
  MONSNO_HUSIGIDANE,
  MONSNO_HITOKAGE,
  MONSNO_ZENIGAME,
  0,
};
static const u16 VSMListItemL[] = {
  ITEM_KIZUGUSURI,
  ITEM_KIZUGUSURI,
  ITEM_KIZUGUSURI,
  ITEM_KIZUGUSURI,
  ITEM_KIZUGUSURI,
  ITEM_KIZUGUSURI,
};
static const u16 VSMListItemR[] = {
  ITEM_HAZIMETEMEERU,
  ITEM_HAZIMETEMEERU,
  ITEM_HAZIMETEMEERU,
  ITEM_HAZIMETEMEERU,
  ITEM_HAZIMETEMEERU,
  ITEM_HAZIMETEMEERU,
};

static const u16 DDemoMonsList[][7] =
{
  {
    MONSNO_KORATTA,     // ノーマル
    MONSNO_KAIRIKII,    // かくとう
    MONSNO_648,         // ひこう カザカミ MONSNO_KAZAKAMI
    MONSNO_DOGAASU,     // どく
    MONSNO_GURAADON,    // じめん
    MONSNO_IWAAKU,      // いわ
/*
    MONSNO_MIZUGOROU,   // みず
    MONSNO_MIZUGOROU,   // みず
    MONSNO_MIZUGOROU,   // みず
    MONSNO_MIZUGOROU,   // みず
    MONSNO_MIZUGOROU,   // みず
    MONSNO_MIZUGOROU,   // みず
*/
    0,
  },
  {
    MONSNO_BATAHURII,   // むし
    MONSNO_GENGAA,      // ゴースト
    MONSNO_METAGUROSU,  // はがね
    MONSNO_ATYAMO,      // ほのお
    MONSNO_MIZUGOROU,   // みず
    MONSNO_KIMORI,      // くさ
    0,
  },
  {
    MONSNO_PIKATYUU,    // でんき
    MONSNO_KIRURIA,     // エスパー
    MONSNO_RUUZYURA,    // こおり
    MONSNO_REKKUUZA,    // ドラゴン
    MONSNO_BURAKKII,    // あく
    0,
  }
};




static GFL_PROC_RESULT MainProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  NAKAHIRO_MAIN_WORK * wk;

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NAKAHIRO_DEBUG, 0x20000 );

  wk = GFL_PROC_AllocWork( proc, sizeof(NAKAHIRO_MAIN_WORK), HEAPID_NAKAHIRO_DEBUG );

  wk->main_seq  = MAIN_SEQ_INIT;
  wk->heapID    = HEAPID_NAKAHIRO_DEBUG;
  wk->gamedata  = GAMEDATA_Create( wk->heapID );
  wk->bb_party  = NULL;

  wk->ppL = NULL;
  wk->ppR = NULL;

  return GFL_PROC_RES_FINISH;
}

static GFL_PROC_RESULT MainProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  NAKAHIRO_MAIN_WORK * wk = mywk;

  switch( wk->main_seq ){
  case MAIN_SEQ_INIT:
    BgInit( wk );
    TopMenuInit( wk );
    wk->main_seq = MAIN_SEQ_MAIN;
    break;

  case MAIN_SEQ_MAIN:
    {
      u32 ret = BmpMenuList_Main( wk->lw );

      switch( ret ){
      case BMPMENULIST_NULL:
      case BMPMENULIST_LABEL:
        break;

      case BMPMENULIST_CANCEL:    // キャンセル
        TopMenuExit( wk );
        BgExit();
        wk->main_seq = MAIN_SEQ_END;
        break;

      default:
        TopMenuExit( wk );
        BgExit();
        FadeOutSet( wk, ret );
        wk->main_seq = MAIN_SEQ_FADE_MAIN;
      }
    }
    break;

  case MAIN_SEQ_FADE_MAIN:
    if( WIPE_SYS_EndCheck() == TRUE ){
      wk->main_seq = wk->next_seq;
    }
    break;

  case MAIN_SEQ_END:
    if( wk->bb_party != NULL ){
      BATTLE_BOX_SAVE * sv;
      sv = BATTLE_BOX_SAVE_GetBattleBoxSave( GAMEDATA_GetSaveControlWork(wk->gamedata) );
      BATTLE_BOX_SAVE_SetPokeParty( sv, wk->bb_party );
      GFL_HEAP_FreeMemory( wk->bb_party );
    }
    if( wk->ppL != NULL ){
      GFL_HEAP_FreeMemory( wk->ppL );
    }
    if( wk->ppR != NULL ){
      GFL_HEAP_FreeMemory( wk->ppR );
    }
//    OS_Printf( "nakahiroデバッグ処理終了しました\n" );
    return GFL_PROC_RES_FINISH;




  case MAIN_SEQ_BOX_CALL1:
    wk->box_data.gamedata  = wk->gamedata;
    wk->box_data.sv_box    = GAMEDATA_GetBoxManager( wk->gamedata );
    wk->box_data.pokeparty = GAMEDATA_GetMyPokemon( wk->gamedata );
    wk->box_data.myitem    = GAMEDATA_GetMyItem( wk->gamedata );
    wk->box_data.mystatus  = GAMEDATA_GetMyStatus( wk->gamedata );
    wk->box_data.callMode  = BOX_MODE_SEIRI;
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ){
      BOXDAT_SetExWallPaperFlag( wk->box_data.sv_box, BOX_EX_WALLPAPER_SET_FLAG_1 );
    }
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){
      BOXDAT_SetExWallPaperFlag( wk->box_data.sv_box, BOX_EX_WALLPAPER_SET_FLAG_2 );
    }
    SetBoxPoke( wk );
    SetPartyPoke( wk );
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &wk->box_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case MAIN_SEQ_BOX_CALL2:
    wk->box_data.gamedata  = wk->gamedata;
    wk->box_data.sv_box    = GAMEDATA_GetBoxManager( wk->gamedata );
    wk->box_data.pokeparty = GAMEDATA_GetMyPokemon( wk->gamedata );
    wk->box_data.myitem    = GAMEDATA_GetMyItem( wk->gamedata );
    wk->box_data.mystatus  = GAMEDATA_GetMyStatus( wk->gamedata );
    wk->box_data.callMode  = BOX_MODE_TURETEIKU;
    SetBoxPoke( wk );
    SetPartyPoke( wk );
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &wk->box_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case MAIN_SEQ_BOX_CALL3:
    wk->box_data.gamedata  = wk->gamedata;
    wk->box_data.sv_box    = GAMEDATA_GetBoxManager( wk->gamedata );
    wk->box_data.pokeparty = GAMEDATA_GetMyPokemon( wk->gamedata );
    wk->box_data.myitem    = GAMEDATA_GetMyItem( wk->gamedata );
    wk->box_data.mystatus  = GAMEDATA_GetMyStatus( wk->gamedata );
    wk->box_data.callMode  = BOX_MODE_AZUKERU;
    SetBoxPoke( wk );
    SetPartyPoke( wk );
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &wk->box_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case MAIN_SEQ_BOX_CALL4:
    wk->box_data.gamedata  = wk->gamedata;
    wk->box_data.sv_box    = GAMEDATA_GetBoxManager( wk->gamedata );
    wk->box_data.pokeparty = GAMEDATA_GetMyPokemon( wk->gamedata );
    wk->box_data.myitem    = GAMEDATA_GetMyItem( wk->gamedata );
    wk->box_data.mystatus  = GAMEDATA_GetMyStatus( wk->gamedata );
    wk->box_data.callMode  = BOX_MODE_ITEM;
    SetBoxPoke( wk );
    SetPartyPoke( wk );
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &wk->box_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case MAIN_SEQ_BOX_CALL5:
    {
      BATTLE_BOX_SAVE * sv;
      sv = BATTLE_BOX_SAVE_GetBattleBoxSave( GAMEDATA_GetSaveControlWork(wk->gamedata) );
      wk->bb_party = BATTLE_BOX_SAVE_MakePokeParty( sv, wk->heapID );
    }
    wk->box_data.gamedata  = wk->gamedata;
    wk->box_data.sv_box    = GAMEDATA_GetBoxManager( wk->gamedata );
    wk->box_data.pokeparty = wk->bb_party;
    wk->box_data.myitem    = GAMEDATA_GetMyItem( wk->gamedata );
    wk->box_data.mystatus  = GAMEDATA_GetMyStatus( wk->gamedata );
    wk->box_data.callMode  = BOX_MODE_BATTLE;
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ){
      wk->box_data.bbRockFlg = TRUE;
    }else{
      wk->box_data.bbRockFlg = FALSE;
    }
    SetBoxPoke( wk );
//    SetPartyPoke( wk );
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &wk->box_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case MAIN_SEQ_BOX_CALL6:
    GFL_STD_MemClear( wk->sleepTable, sizeof(u16)*(MONSNO_END+8) );
    wk->sleepTable[1] = 1;
    wk->box_data.gamedata  = wk->gamedata;
    wk->box_data.sv_box    = GAMEDATA_GetBoxManager( wk->gamedata );
    wk->box_data.pokeparty = GAMEDATA_GetMyPokemon( wk->gamedata );
    wk->box_data.myitem    = GAMEDATA_GetMyItem( wk->gamedata );
    wk->box_data.mystatus  = GAMEDATA_GetMyStatus( wk->gamedata );
    wk->box_data.callMode  = BOX_MODE_SLEEP;
    wk->box_data.sleepTable = wk->sleepTable;
    SetBoxPoke( wk );
    SetPartyPoke( wk );
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(box), &BOX2_ProcData, &wk->box_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case MAIN_SEQ_MOVIE_CALL:
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(command_demo), &COMMANDDEMO_ProcData, &wk->cdemo_data );
    wk->main_seq = MAIN_SEQ_END;
    break;


  case MAIN_SEQ_ZUKAN_CALL:
    SetZukanData( wk, 0 );
    wk->zkn_data.gamedata = wk->gamedata;
    wk->zkn_data.savedata = GAMEDATA_GetZukanSave( wk->gamedata );
    wk->zkn_data.mystatus = GAMEDATA_GetMyStatus( wk->gamedata );
    wk->zkn_data.callMode = ZUKAN_MODE_TOP;
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(zukan), &ZUKAN_ProcData, &wk->zkn_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case MAIN_SEQ_ZKNLIST_CALL:
    SetZukanData( wk, 1 );
    SetZukanPacchiiru( wk );
    wk->zkn_data.gamedata = wk->gamedata;
    wk->zkn_data.savedata = GAMEDATA_GetZukanSave( wk->gamedata );
    wk->zkn_data.mystatus = GAMEDATA_GetMyStatus( wk->gamedata );
    wk->zkn_data.callMode = ZUKAN_MODE_LIST;
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(zukan), &ZUKAN_ProcData, &wk->zkn_data );
    wk->main_seq = MAIN_SEQ_END;
    break;


  case MAIN_SEQ_VSMLIST_L:
    wk->vsl_data.pos = VS_MULTI_LIST_POS_LEFT;
    wk->ppL = PokeParty_AllocPartyWork( wk->heapID );
    wk->ppR = PokeParty_AllocPartyWork( wk->heapID );
    SetPokeParty( wk, wk->ppL, VSMListMonsL, VSMListItemL );
    SetPokeParty( wk, wk->ppR, VSMListMonsR, VSMListItemR );
    wk->vsl_data.myPP = wk->ppL;
    wk->vsl_data.ptPP = wk->ppR;
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(vs_multi_list), &VS_MULTI_LIST_ProcData, &wk->vsl_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case MAIN_SEQ_VSMLIST_R:
    wk->vsl_data.pos = VS_MULTI_LIST_POS_RIGHT;
    wk->ppL = PokeParty_AllocPartyWork( wk->heapID );
    wk->ppR = PokeParty_AllocPartyWork( wk->heapID );
    SetPokeParty( wk, wk->ppL, VSMListMonsL, VSMListItemL );
    SetPokeParty( wk, wk->ppR, VSMListMonsR, VSMListItemR );
    wk->vsl_data.myPP = wk->ppL;
    wk->vsl_data.ptPP = wk->ppR;
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(vs_multi_list), &VS_MULTI_LIST_ProcData, &wk->vsl_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case MAIN_SEQ_WIFI_NOTE:
    wk->wifi_note_data.pGameData = wk->gamedata;
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(wifinote), &WifiNoteProcData, &wk->wifi_note_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case  MAIN_SEQ_DENDOU_PC:
    wk->dpc_data.gamedata = wk->gamedata;
    wk->dpc_data.callMode = 0;
    {
      SAVE_CONTROL_WORK * sv;
      DENDOU_SAVEDATA * ex_rec;
      RTCDate date;
      RTC_GetDate( &date );
      wk->ppL = PokeParty_AllocPartyWork( wk->heapID );
      SetPokeParty( wk, wk->ppL, VSMListMonsL, VSMListItemL );
      DendouRecord_Add( GAMEDATA_GetDendouRecord(wk->gamedata), wk->ppL, &date, wk->heapID );
    }
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(dendou_pc), &DENDOUPC_ProcData, &wk->dpc_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case  MAIN_SEQ_DENDOU_DEMO:
    wk->ppL = PokeParty_AllocPartyWork( wk->heapID );
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){
      SetPokeParty( wk, wk->ppL, &DDemoMonsList[2][0], VSMListItemL );
    }else if( GFL_UI_KEY_GetCont() & PAD_BUTTON_Y ){
      SetPokeParty( wk, wk->ppL, &DDemoMonsList[1][0], VSMListItemL );
    }else{
      SetPokeParty( wk, wk->ppL, &DDemoMonsList[0][0], VSMListItemL );
    }
    wk->ddemo_data.party    = wk->ppL;
    wk->ddemo_data.mystatus = GAMEDATA_GetMyStatus( wk->gamedata );
    wk->ddemo_data.ptime    = SaveData_GetPlayTime( GAMEDATA_GetSaveControlWork(wk->gamedata) );
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(dendou_demo), &DENDOUDEMO_ProcData, &wk->ddemo_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case MAIN_SEQ_STAFF_ROLL:
    wk->sr_data.fastMode = STAFFROLL_MODE_DEBUG;
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_L ){
      wk->sr_data.mojiMode = MOJIMODE_KANJI;
    }else{
      wk->sr_data.mojiMode = MOJIMODE_HIRAGANA;
    }
    GFL_PROC_SysCallProc( FS_OVERLAY_ID(staff_roll), &STAFFROLL_ProcData, &wk->sr_data );
    wk->main_seq = MAIN_SEQ_END;
    break;

  case MAIN_SEQ_POKEICON:
    GFL_PROC_SysCallProc( NO_OVERLAY_ID, &POKEICONVIEW_ProcData, NULL );
    wk->main_seq = MAIN_SEQ_END;
    break;
  }

  return GFL_PROC_RES_CONTINUE;
}

static GFL_PROC_RESULT MainProcEnd( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  NAKAHIRO_MAIN_WORK * wk = mywk;

  GAMEDATA_Delete( wk->gamedata );

  GFL_PROC_FreeWork( proc );
  GFL_HEAP_DeleteHeap( HEAPID_NAKAHIRO_DEBUG );

  return GFL_PROC_RES_FINISH;
}

static void BgInit( NAKAHIRO_MAIN_WORK * wk )
{
  GFL_BG_Init( wk->heapID );
  { // BG SYSTEM
    GFL_BG_SYS_HEADER sysh = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &sysh );
  }
  { // メイン画面：文字
    GFL_BG_BGCNT_HEADER cnth= {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
    };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen( GFL_BG_FRAME0_M );
    GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0, wk->heapID );
  }
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );

  // フォントパレット
  GFL_ARC_UTIL_TransVramPalette(
    ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, 0, 0x20, wk->heapID );
}

static void BgExit(void)
{
  GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_OFF );
  GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
  GFL_BG_Exit();
}

// フェードイン
static void FadeInSet( NAKAHIRO_MAIN_WORK * wk, u32 next )
{
  WIPE_SYS_Start(
    WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
    WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapID );

  wk->next_seq = next;
}

// フェードアウト
static void FadeOutSet( NAKAHIRO_MAIN_WORK * wk, u32 next )
{
  WIPE_SYS_Start(
    WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
    WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, wk->heapID );

  wk->next_seq = next;
}

static void TopMenuInit( NAKAHIRO_MAIN_WORK * wk )
{
  BMPMENULIST_HEADER  lh;
  u32 i;

  GFL_BMPWIN_Init( wk->heapID );

  wk->mman = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_d_nakahiro_dat, wk->heapID );
  wk->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, wk->heapID );
  wk->que  = PRINTSYS_QUE_Create( wk->heapID );
  wk->win  = GFL_BMPWIN_Create( GFL_BG_FRAME0_M, 1, 1, 16, 12, 0, GFL_BMP_CHRAREA_GET_B );

  wk->ld = BmpMenuWork_ListCreate( TOP_MENU_SIZ, wk->heapID );
  for( i=0; i<TOP_MENU_SIZ; i++ ){
    STRBUF * str = GFL_MSG_CreateString( wk->mman, top_menu00+i );
    BmpMenuWork_ListAddString( &wk->ld[i], str, MAIN_SEQ_BOX_CALL1+i, wk->heapID );
    GFL_STR_DeleteBuffer( str );
  }

  lh = TopMenuListH;
  lh.list = wk->ld;
  lh.win  = wk->win;
  lh.msgdata = wk->mman;      //表示に使用するメッセージバッファ
  lh.print_util = &wk->util;  //表示に使用するプリントユーティリティ
  lh.print_que  = wk->que;    //表示に使用するプリントキュー
  lh.font_handle = wk->font;  //表示に使用するフォントハンドル

  wk->lw = BmpMenuList_Set( &lh, 0, 0, wk->heapID );
  BmpMenuList_SetCursorBmp( wk->lw, wk->heapID );

  GFL_BMPWIN_TransVramCharacter( wk->win );
  GFL_BMPWIN_MakeScreen( wk->win );
  GFL_BG_LoadScreenReq( GFL_BMPWIN_GetFrame(wk->win) );
}

static void TopMenuExit( NAKAHIRO_MAIN_WORK * wk )
{
  BmpMenuList_Exit( wk->lw, NULL, NULL );
  BmpMenuWork_ListDelete( wk->ld );

  GFL_BMPWIN_Delete( wk->win );
  PRINTSYS_QUE_Delete( wk->que );
  GFL_FONT_Delete( wk->font );
  GFL_MSG_Delete( wk->mman );

  GFL_BMPWIN_Exit();
}


static void SetBoxPoke( NAKAHIRO_MAIN_WORK * wk )
{
  const POKEMON_PASO_PARAM * ppp;
  POKEMON_PARAM * pp;
  GFL_MSGDATA * man;
  STRBUF * str;
  u32 i;

  man = GFL_MSG_Create(
            GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_d_nakahiro_dat, wk->heapID );
  str = GFL_MSG_CreateString( man, pokename );

//  for( i=MONSNO_HUSIGIDANE; i<=MONSNO_END; i++ ){
  for( i=MONSNO_HUSIGIDANE; i<=40; i++ ){
//  for( i=MONSNO_HUSIGIDANE; i<=8; i++ ){
    pp  = PP_Create( i, 50, 0, wk->heapID );
    PP_Put( pp, ID_PARA_oyaname, (u32)str );
    PP_Put( pp, ID_PARA_oyasex, PTL_SEX_MALE );
    if( i == 40 ){
      PP_Put( pp, ID_PARA_tamago_flag, 1 );
      PP_Put( pp, ID_PARA_form_no, 0 );
    }
    if( ( i & 3 ) == 3 ){
      PP_Put( pp, ID_PARA_item, 1+(i&1) );
    }
    ppp = PP_GetPPPPointerConst( pp );
    if( BOXDAT_PutPokemon( wk->box_data.sv_box, ppp ) == FALSE ){
      GFL_HEAP_FreeMemory( pp );
      break;
    }
    GFL_HEAP_FreeMemory( pp );
  }

  GFL_STR_DeleteBuffer( str );
  GFL_MSG_Delete( man );

/*
  ID_PARA_oyaname,              //親の名前（STRBUF使用）
  ID_PARA_oyaname_raw,            //親の名前（STRCODE配列使用 ※許可制）
  ID_PARA_get_year,             //捕まえた年
  ID_PARA_get_month,              //捕まえた月
  ID_PARA_get_day,              //捕まえた日
  ID_PARA_birth_year,             //生まれた年
  ID_PARA_birth_month,            //生まれた月
  ID_PARA_birth_day,              //生まれた日
  ID_PARA_get_place,              //捕まえた場所
  ID_PARA_birth_place,            //生まれた場所
  ID_PARA_pokerus,              //ポケルス
  ID_PARA_get_ball,             //捕まえたボール
  ID_PARA_get_level,              //捕まえたレベル
  ID_PARA_oyasex,               //親の性別
*/

}

static void SetPartyPoke( NAKAHIRO_MAIN_WORK * wk )
{
  static const u32 mons[] = {
    MONSNO_HUSIGIDANE,
    MONSNO_HITOKAGE,
    MONSNO_ZENIGAME,
  };
  POKEMON_PARAM * pp;
  GFL_MSGDATA * man;
  STRBUF * str;
  u32 i;

  man = GFL_MSG_Create(
            GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_d_nakahiro_dat, wk->heapID );
  str = GFL_MSG_CreateString( man, pokename );

  for( i=0; i<3; i++ ){
    pp = PP_Create( mons[i], 50, 0, wk->heapID );
    PP_Put( pp, ID_PARA_oyaname, (u32)str );
    PP_Put( pp, ID_PARA_oyasex, PTL_SEX_MALE );
    if( i & 1 ){
//      PP_Put( pp, ID_PARA_item, 1 );
//#define ITEM_HAZIMETEMEERU    ( 137 )   // はじめてメール
      PP_Put( pp, ID_PARA_item, 137 );
    }
    PokeParty_Add( wk->box_data.pokeparty, pp );
    GFL_HEAP_FreeMemory( pp );
  }

  GFL_STR_DeleteBuffer( str );
  GFL_MSG_Delete( man );
}

static void SetPokeParty( NAKAHIRO_MAIN_WORK * wk, POKEPARTY * party, const u16 * mons, const u16 * item )
{
  POKEMON_PARAM * pp;
  GFL_MSGDATA * man;
  STRBUF * str;
  u32 i;

  man = GFL_MSG_Create(
            GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_d_nakahiro_dat, wk->heapID );
  str = GFL_MSG_CreateString( man, pokename );

  PokeParty_InitWork( party );

  i = 0;

  while( 1 ){
    if( mons[i] == 0 ){ break; }

    pp = PP_Create( mons[i], 50, 0, wk->heapID );
    PP_Put( pp, ID_PARA_oyaname, (u32)str );
    PP_Put( pp, ID_PARA_oyasex, PTL_SEX_MALE );
    PP_Put( pp, ID_PARA_item, item[i] );
    PokeParty_Add( party, pp );
    GFL_HEAP_FreeMemory( pp );

    i++;
  }

  GFL_STR_DeleteBuffer( str );
  GFL_MSG_Delete( man );
}


static void SetZukanDataOne( NAKAHIRO_MAIN_WORK * wk, u16 mons, u32 mode )
{
  ZUKAN_SAVEDATA * sv;
  POKEMON_PARAM * pp;

  sv = GAMEDATA_GetZukanSave( wk->gamedata );
  pp = PP_Create( mons, 50, 0, wk->heapID );

  if( mode == 0 ){
    ZUKANSAVE_SetPokeGet( sv, pp );
  }else{
    ZUKANSAVE_SetPokeSee( sv, pp );
  }

  GFL_HEAP_FreeMemory( pp );
}

static void SetZukanData( NAKAHIRO_MAIN_WORK * wk, u32 mode )
{
  u32 i;

//  for( i=1; i<=MONSNO_END; i++ ){
  for( i=600; i<=608; i++ ){
    SetZukanDataOne( wk, i, 0 );
  }
  SetZukanDataOne( wk, MONSNO_KIMORI, 0 );
  SetZukanDataOne( wk, MONSNO_PATTIIRU, 0 );

  ZUKANSAVE_SetZenkokuZukanFlag( GAMEDATA_GetZukanSave( wk->gamedata ) );
}

static void SetZukanPacchiiru( NAKAHIRO_MAIN_WORK * wk )
{
  ZUKAN_SAVEDATA * sv;
  POKEMON_PARAM * pp;
  u32 sex;

  sv = GAMEDATA_GetZukanSave( wk->gamedata );
  pp = PP_Create( MONSNO_PATTIIRU, 50, 0, wk->heapID );
  ZUKANSAVE_SetPokeGet( sv, pp );
  sex = PP_GetSex( pp );
  GFL_HEAP_FreeMemory( pp );

  if( sex == PTL_SEX_MALE ){
    sex = PTL_SEX_FEMALE;
    OS_Printf( "パッチール♂\n" );
  }else if( sex == PTL_SEX_FEMALE ){
    sex = PTL_SEX_MALE;
    OS_Printf( "パッチール♀\n" );
  }else{
    return;
  }

  while( 1 ){
    pp = PP_Create( MONSNO_PATTIIRU, 50, 0, wk->heapID );
    if( PP_GetSex( pp ) == sex ){
      ZUKANSAVE_SetPokeGet( sv, pp );
      GFL_HEAP_FreeMemory( pp );
      break;
    }
    GFL_HEAP_FreeMemory( pp );
  }

  ZUKANSAVE_SetZenkokuZukanFlag( sv );
  ZUKANSAVE_SetGraphicVersionUpFlag( sv );
}
#endif