//======================================================================
/**
 * @file	plist_sys.c
 * @brief	ポケモンステータス メイン処理
 * @author	ariizumi
 * @data	09/06/10
 *
 * モジュール名：PSTATUS
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "app/app_menu_common.h"
#include "poke_tool/poke_tool.h"
#include "sound/pm_voice.h"
#include "sound/pm_wb_voice.h"

#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "pokelist_gra.naix"
#include "app_menu_common.naix"

#include "p_sta_sys.h"
#include "p_sta_sub.h"
#include "p_sta_info.h"
#include "p_sta_skill.h"
#include "p_sta_ribbon.h"
#include "p_sta_snd_def.h"
#include "ribbon.h"

#include "test/ariizumi/ari_debug.h"

//======================================================================
//	define
//======================================================================
#pragma mark [> define

#define PSTATUS_SCROLL_SPD (4) //何フレームに1回スクロールするか？

//======================================================================
//	enum
//======================================================================
#pragma mark [> enum

//======================================================================
//	typedef struct
//======================================================================
#pragma mark [> struct

//======================================================================
//	proto
//======================================================================
#pragma mark [> proto

static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_A,       // メイン2DエンジンのBG
  GX_VRAM_BGEXTPLTT_NONE,     // メイン2DエンジンのBG拡張パレット
  GX_VRAM_SUB_BG_128_C,     // サブ2DエンジンのBG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // サブ2DエンジンのBG拡張パレット
  GX_VRAM_OBJ_128_B,       // メイン2DエンジンのOBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // メイン2DエンジンのOBJ拡張パレット
  GX_VRAM_SUB_OBJ_16_I,     // サブ2DエンジンのOBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // サブ2DエンジンのOBJ拡張パレット
  GX_VRAM_TEX_0_D,        // テクスチャイメージスロット
  GX_VRAM_TEXPLTT_0_F,     // テクスチャパレットスロット
  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_32K
};


static void PSTATUS_VBlankFunc(GFL_TCB *tcb, void *wk );

static void PSTATUS_InitGraphic( PSTATUS_WORK *work );
static void PSTATUS_TermGraphic( PSTATUS_WORK *work );
static void PSTATUS_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode );

static void PSTATUS_LoadResource( PSTATUS_WORK *work );
static void PSTATUS_ReleaseResource( PSTATUS_WORK *work );
static void PSTATUS_InitCell( PSTATUS_WORK *work );
static void PSTATUS_TermCell( PSTATUS_WORK *work );
static void PSTATUS_InitMessage( PSTATUS_WORK *work );
static void PSTATUS_TermMessage( PSTATUS_WORK *work );

static void PSTATUS_UpdateBarButton( PSTATUS_WORK *work );
static void PSTATUS_UpdateUI( PSTATUS_WORK *work );
static const BOOL PSTATUS_UpdateKey( PSTATUS_WORK *work );
static const BOOL PSTATUS_UpdateKey_Page( PSTATUS_WORK *work );
static void PSTATUS_UpdateTP( PSTATUS_WORK *work );

static const BOOL PSTATUS_ChangeData( PSTATUS_WORK *work , const BOOL isUpOder );
static const BOOL PSTATUS_CanChangeData( PSTATUS_WORK *work , const BOOL isUpOder , u16 *newPos );
static void PSTATUS_RefreshData( PSTATUS_WORK *work );
static void PSTATUS_RefreshDisp( PSTATUS_WORK *work );
static void PSTATUS_WaitDisp( PSTATUS_WORK *work );

static void PSTATUS_PALANIM_UpdatePalletAnime( PSTATUS_WORK *work );
static const BOOL PSTATUS_UTIL_CanUseExitButton( PSTATUS_WORK *work );
static const BOOL PSTATUS_UTIL_CheckHaveAnyRibbon( PSTATUS_WORK *work , const POKEMON_PASO_PARAM *ppp );

#if PM_DEBUG
void PSTATUS_UTIL_DebugCreatePP( PSTATUS_WORK *work );
#endif

#if USE_STATUS_DEBUG
static void PSTATUS_InitDebug( PSTATUS_WORK *work );
static void PSTATUS_TermDebug( PSTATUS_WORK *work );
#endif //USE_STATUS_DEBUG

//--------------------------------------------------------------
//	初期化
//--------------------------------------------------------------
const BOOL PSTATUS_InitPokeStatus( PSTATUS_WORK *work )
{
  u8 i;
  work->dataPos = work->psData->pos;
  work->befDataPos = 0xFF;
  work->scrollCnt = 0;
  work->befVCount = OS_GetVBlankCount();
  work->befPage = PPT_MAX;
  work->isActiveBarButton = TRUE;
  work->isAnimeBarCursor[0] = FALSE;
  work->isAnimeBarCursor[1] = FALSE;
  work->retVal = SRT_CONTINUE;
  work->isWaitDisp = TRUE;
  work->mosaicEffSeq = SMES_NONE;
  work->mosaicCnt = 0;
  work->mainSeq = SMS_FADEIN;
  work->clwkExitButton = NULL;
  work->reqPlayVoice = FALSE;
  work->canSelectHiden = FALSE;
  work->ktst = GFL_UI_CheckTouchOrKey();
  
  //モード例外分岐
  if( work->psData->mode == PST_MODE_WAZAADD_HIDEN )
  {
    work->psData->mode = PST_MODE_WAZAADD;
    work->canSelectHiden = TRUE;
  }

  if( work->psData->ppt == PST_PP_TYPE_POKEPASO )
  {
    const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP(work);
    work->calcPP = PP_CreateByPPP( ppp , work->heapId );
  }
  else
  {
    work->calcPP = NULL;
  }

  PSTATUS_InitGraphic( work );

  work->subWork = PSTATUS_SUB_Init( work );
  work->infoWork = PSTATUS_INFO_Init( work );
  work->skillWork = PSTATUS_SKILL_Init( work );
  work->ribbonWork = PSTATUS_RIBBON_Init( work );

  PSTATUS_LoadResource( work );
  PSTATUS_InitMessage( work );

  PSTATUS_InitCell( work );

  work->vBlankTcb = GFUser_VIntr_CreateTCB( PSTATUS_VBlankFunc , work , 8 );

  //パレットアニメ用
  work->anmCnt = 0;
  //アニメ用に、パレットを退避
  {
    u32 palAdr = (HW_OBJ_PLTT+PSTATUS_PALANIM_PAL_NO*32+PSTATUS_PALANIM_PAL_POS*2);
    GFL_STD_MemCopy16( (void*)(palAdr) , work->anmPalBase , PSTATUS_PALANIM_PAL_POS*2 );
  }
  {
    u32 palAdr = (HW_OBJ_PLTT+PSTATUS_OBJPLT_SKILL_PLATE*32);
    GFL_STD_MemCopy16( (void*)(palAdr) , work->anmSkillPalBase , 32*2 );
  }
  {
    u32 palAdr = (HW_OBJ_PLTT+PSTATUS_OBJPLT_RIBBON_BAR*32);
    GFL_STD_MemCopy16( (void*)(palAdr) , work->anmRibbonPal , 32 );
    GFL_STD_MemCopy16( (void*)(palAdr) , work->anmRibbonPalBase , 32 );
  }
  
  //最初の表示処理
  PSTATUS_RefreshData( work );
  if( work->psData->mode == PST_MODE_WAZAADD )
  {
    work->page = PPT_SKILL_ADD;
    PSTATUS_SUB_DispPage( work , work->subWork );
    PSTATUS_SKILL_DispPage_WazaAdd( work , work->skillWork );
    PSTATUS_RIBBON_CreateRibbonBar( work , work->ribbonWork );
  }
  else
  {
    work->page = work->psData->page;

    if( work->page < PPT_SKILL_ADD )
    {
      for( i=0;i<PPT_MAX-1;i++ )
      {
        work->shortCutCheck[i] = GAMEDATA_GetShortCut( work->psData->game_data, SHORTCUT_ID_PSTATUS_STATUS+i );
        OS_TPrintf("[[%d]]",work->shortCutCheck[i]);
      }
    }

    PSTATUS_SUB_DispPage( work , work->subWork );
    PSTATUS_RIBBON_CreateRibbonBar( work , work->ribbonWork );
    switch( work->page )
    {
    case PPT_INFO:
      PSTATUS_INFO_DispPage( work , work->infoWork );
      break;
    case PPT_SKILL:
      PSTATUS_SKILL_DispPage( work , work->skillWork );
      break;
    case PPT_RIBBON:
      PSTATUS_RIBBON_DispPage( work , work->ribbonWork );
      break;
    default:
      //091026 初期表示ページ追加に伴い
      GF_ASSERT_MSG(0,"Status first page is invalid!![%d]\n",work->psData->page);
      work->page = PPT_INFO;
      PSTATUS_INFO_DispPage( work , work->infoWork );
      break;

    }
  }
  
  GFL_NET_ReloadIconTopOrBottom(TRUE , work->heapId );

#if USE_STATUS_DEBUG
  PSTATUS_InitDebug( work );
#endif

  return TRUE;
}

//--------------------------------------------------------------
//	開放
//--------------------------------------------------------------
const BOOL PSTATUS_TermPokeStatus( PSTATUS_WORK *work )
{
  u8 i;
  GFL_TCB_DeleteTask( work->vBlankTcb );

  work->psData->pos = work->dataPos;

#if USE_STATUS_DEBUG
  PSTATUS_TermDebug( work );
#endif

  PSTATUS_TermCell( work );

  PSTATUS_TermMessage( work );
  PSTATUS_ReleaseResource( work );

  PSTATUS_RIBBON_DeleteRibbonBar( work , work->ribbonWork );
  PSTATUS_RIBBON_Term( work , work->ribbonWork );
  PSTATUS_SKILL_Term( work , work->skillWork );
  PSTATUS_INFO_Term( work , work->infoWork );
  PSTATUS_SUB_Term( work , work->subWork );

  PSTATUS_TermGraphic( work );

  if( work->page < PPT_SKILL_ADD )
  {
    for( i=0;i<PPT_MAX-1;i++ )
    {
      GAMEDATA_SetShortCut( work->psData->game_data, SHORTCUT_ID_PSTATUS_STATUS+i , work->shortCutCheck[i] );
    }
  }


  //モード例外分岐
  if( work->canSelectHiden == TRUE )
  {
    work->psData->mode = PST_MODE_WAZAADD_HIDEN;
    work->canSelectHiden = FALSE;
  }

  GFL_UI_SetTouchOrKey( work->ktst );

  if( work->calcPP != NULL )
  {
    GFL_HEAP_FreeMemory( work->calcPP );
  }
  return TRUE;
}

//--------------------------------------------------------------
//	更新
//--------------------------------------------------------------
const PSTATUS_RETURN_TYPE PSTATUS_UpdatePokeStatus( PSTATUS_WORK *work )
{
  work->befTpx = work->tpx;
  work->befTpy = work->tpy;
  GFL_UI_TP_GetPointCont( &work->tpx , &work->tpy );

  switch( work->mainSeq )
  {
  case SMS_FADEIN:
    if( work->isWaitDisp == TRUE )
    {
      PSTATUS_WaitDisp( work );
    }
    else
    {
      WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEIN , WIPE_TYPE_FADEIN , 
                      WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
      work->mainSeq = SMS_FADEIN_WAIT;
    }
    break;
    
  case SMS_FADEIN_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      work->mainSeq = SMS_UPDATE;
    }
    break;
    
  case SMS_FADEOUT:
    if( work->clwkExitButton == NULL ||
        GFL_CLACT_WK_CheckAnmActive( work->clwkExitButton ) == FALSE ||
        work->psData->isExitRequest == TRUE )
    {
      WIPE_SYS_Start( WIPE_PATTERN_WMS , WIPE_TYPE_FADEOUT , WIPE_TYPE_FADEOUT , 
                      WIPE_FADE_BLACK , WIPE_DEF_DIV , WIPE_DEF_SYNC , work->heapId );
      work->mainSeq = SMS_FADEOUT_WAIT;
    }
    break;
    
  case SMS_FADEOUT_WAIT:
    if( WIPE_SYS_EndCheck() == TRUE )
    {
      return work->retVal;
    }
    break;
    
  case SMS_UPDATE:
    PSTATUS_UpdateBarButton( work );
    
    PSTATUS_SUB_Main( work , work->subWork );

    if( work->isWaitDisp == TRUE )
    {
      PSTATUS_WaitDisp( work );
    }
    else
    {
      if( work->psData->isExitRequest == TRUE )
      {
        work->retVal = SRT_RETURN;
        work->mainSeq = SMS_FADEOUT;
      }
      
      PSTATUS_UpdateUI( work );

      if( work->isWaitDisp == FALSE )
      {
        switch( work->befPage )
        {
        case PPT_INFO:
          PSTATUS_INFO_Main( work , work->infoWork );
          break;
        case PPT_SKILL:
        case PPT_SKILL_ADD:
          PSTATUS_SKILL_Main( work , work->skillWork );
          break;
        case PPT_RIBBON:
          PSTATUS_RIBBON_Main( work , work->ribbonWork );
          break;
        }
      }
    }
    break;
  }


  //メッセージ
  PRINTSYS_QUE_Main( work->printQue );
  
  //OBJの更新
  GFL_CLACT_SYS_Main();

  //MCSS更新
  MCSS_Main( work->mcssSys );

  //3D描画  
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    MCSS_Draw( work->mcssSys );
    //PSTATUS_SUB_Draw( work , work->subWork );
  }
  GFL_G3D_DRAW_End();

  //背景スクロール
  {
    const u32 vCount = OS_GetVBlankCount();
    work->scrollCnt += vCount - work->befVCount;
    work->befVCount = vCount;
    
    while( work->scrollCnt >= PSTATUS_SCROLL_SPD )
    {
      GFL_BG_SetScrollReq( PSTATUS_BG_MAIN_BG , GFL_BG_SCROLL_X_INC , 1 );
      GFL_BG_SetScrollReq( PSTATUS_BG_MAIN_BG , GFL_BG_SCROLL_Y_INC , 1 );
      GFL_BG_SetScrollReq( PSTATUS_BG_SUB_BG , GFL_BG_SCROLL_X_INC , 1 );
      GFL_BG_SetScrollReq( PSTATUS_BG_SUB_BG , GFL_BG_SCROLL_Y_INC , 1 );
      work->scrollCnt -= PSTATUS_SCROLL_SPD;
    }
  }
  
  //上下ボタンのアニメ待ち後の暗転チェック
  if( work->isAnimeBarCursor[0] == TRUE &&
      GFL_CLACT_WK_CheckAnmActive( work->clwkBarIcon[SBT_CURSOR_UP] ) == FALSE )
  {
    const BOOL isChange = PSTATUS_CanChangeData( work , FALSE , NULL );
    if( isChange == TRUE )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_UP] , APP_COMMON_BARICON_CURSOR_UP );
    }
    else
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_UP] , APP_COMMON_BARICON_CURSOR_UP_OFF );
    }
    work->isAnimeBarCursor[0] = FALSE;
  }
  if( work->isAnimeBarCursor[1] == TRUE &&
      GFL_CLACT_WK_CheckAnmActive( work->clwkBarIcon[SBT_CURSOR_DOWN] ) == FALSE )
  {
    const BOOL isChange = PSTATUS_CanChangeData( work , TRUE , NULL );
    if( isChange == TRUE )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_DOWN] , APP_COMMON_BARICON_CURSOR_DOWN );
    }
    else
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_DOWN] , APP_COMMON_BARICON_CURSOR_DOWN_OFF );
    }
    work->isAnimeBarCursor[1] = FALSE;
  }
  
  //パレット転送
  PSTATUS_PALANIM_UpdatePalletAnime( work );
  
  return SRT_CONTINUE;
}

//--------------------------------------------------------------
//	VBlank
//--------------------------------------------------------------
static void PSTATUS_VBlankFunc(GFL_TCB *tcb, void *wk )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)wk;
  
  //OBJの更新
  GFL_CLACT_SYS_VBlankFunc();

}


#pragma mark [>graphic
//--------------------------------------------------------------
//グラフィック系初期化
//--------------------------------------------------------------
static void PSTATUS_InitGraphic( PSTATUS_WORK *work )
{
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
  WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
  WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
  WIPE_ResetWndMask(WIPE_DISP_MAIN);
  WIPE_ResetWndMask(WIPE_DISP_SUB);
  G2_BlendNone();
  G2S_BlendNone();

  
  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);
  GFL_DISP_SetBank( &vramBank );

  //BG系の初期化
  GFL_BG_Init( work->heapId );
  GFL_BMPWIN_Init( work->heapId );

  //Vram割り当ての設定
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
    };
    // BG1 MAIN (パラメータ
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x08000,0x8000,
      GX_BG_EXTPLTT_01, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 MAIN (土台
    static const GFL_BG_BGCNT_HEADER header_main2 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x10000,0x8000,
      GX_BG_EXTPLTT_01, 2, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 MAIN (背景
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x10000,0x00000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG0 SUB (文字
    static const GFL_BG_BGCNT_HEADER header_sub0 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x00000,0x06000,
      GX_BG_EXTPLTT_23, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG1 SUB (Info
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x10000,0x08000,
      GX_BG_EXTPLTT_23, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 SUB (土台
    static const GFL_BG_BGCNT_HEADER header_sub2 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x08000,0x00000,
      GX_BG_EXTPLTT_23, 2, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 SUB (背景
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x08000,0x00000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    GFL_BG_SetBGMode( &sys_data );

    PSTATUS_SetupBgFunc( &header_main1 , PSTATUS_BG_PARAM , GFL_BG_MODE_TEXT );
    PSTATUS_SetupBgFunc( &header_main2 , PSTATUS_BG_PLATE , GFL_BG_MODE_TEXT );
    PSTATUS_SetupBgFunc( &header_main3 , PSTATUS_BG_MAIN_BG , GFL_BG_MODE_TEXT );
    
    GFL_BG_SetVisible( PSTATUS_BG_3D , TRUE );
    
    PSTATUS_SetupBgFunc( &header_sub0 , PSTATUS_BG_SUB_STR  , GFL_BG_MODE_TEXT );
    PSTATUS_SetupBgFunc( &header_sub1 , PSTATUS_BG_SUB_TITLE , GFL_BG_MODE_TEXT );
    PSTATUS_SetupBgFunc( &header_sub2 , PSTATUS_BG_SUB_PLATE , GFL_BG_MODE_TEXT );
    PSTATUS_SetupBgFunc( &header_sub3 , PSTATUS_BG_SUB_BG , GFL_BG_MODE_TEXT );
  }
  
  //WindowMask設定
  {
/*
    G2_SetWnd0Position( 0 , 0 , PSTATUS_MAIN_PAGE_WIDTH*8 , 192 );
    G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ , TRUE );
    G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ , FALSE );
    GX_SetVisibleWnd( GX_WNDMASK_W0 );
*/
    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2 , GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG2|GX_BLEND_PLANEMASK_BG3|GX_BLEND_PLANEMASK_OBJ , 13 , 16 );
    G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2 , GX_BLEND_PLANEMASK_BG3 , 13 , 16 );
  }
  
  //OBJ系の初期化
  {
    GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
    cellSysInitData.CGR_RegisterMax = 110;
    GFL_CLACT_SYS_Create( &cellSysInitData , &vramBank ,work->heapId );

    //個数は適当
    work->cellUnit  = GFL_CLACT_UNIT_Create( 96 , 0, work->heapId );
    GFL_CLACT_UNIT_SetDefaultRend( work->cellUnit );
    
    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
  }
  
  //3D系の初期化
  { //3D系の設定
    static const VecFx32 cam_pos = {FX32_CONST(-41.0f),
                                    FX32_CONST(  0.0f),
                                    FX32_CONST(101.0f)};
    static const VecFx32 cam_target = {FX32_CONST(0.0f),
                                       FX32_CONST(0.0f),
                                       FX32_CONST(-1.0f)};
    static const VecFx32 cam_up = {0,FX32_ONE,0};
    //エッジマーキングカラー
    static  const GXRgb edge_color_table[8]=
      { GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 
        GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 
        GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 
        GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ) };
    GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT16K,
            0, work->heapId, NULL );
 
    //正射影カメラ
    work->camera =  GFL_G3D_CAMERA_Create( GFL_G3D_PRJORTH, 
                       FX32_ONE*192.0f,
                       0,
                       0,
                       FX32_ONE*256.0f,
                       (FX32_ONE*128),
                       (FX32_ONE*512),
                       NULL,
                       &cam_pos,
                       &cam_up,
                       &cam_target,
                       work->heapId );
    
    GFL_G3D_CAMERA_Switching( work->camera );
    //エッジマーキングカラーセット
    G3X_SetEdgeColorTable( edge_color_table );
    G3X_EdgeMarking( FALSE );
    
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO , GX_BUFFERMODE_Z );
    GFL_BG_SetBGControl3D( 0 );
  }
  //MCSS
  {
    work->mcssSys = MCSS_Init( 2 , work->heapId );
    MCSS_SetTextureTransAdrs( work->mcssSys , 0 );
    //MCSS_ResetOrthoMode( work->mcssSys );
    MCSS_SetMultiCellAnimeRate( work->mcssSys , FX32_ONE*16 );
  }
  //Vram転送アニメ
//  NNS_GfdClearVramTransferManagerTask();
}


//--------------------------------------------------------------
//グラフィック系開放
//--------------------------------------------------------------
static void PSTATUS_TermGraphic( PSTATUS_WORK *work )
{
  NNS_GfdClearVramTransferManagerTask();
  
  MCSS_Exit( work->mcssSys );

  GFL_G3D_CAMERA_Delete( work->camera );
  GFL_G3D_Exit();

  GFL_CLACT_UNIT_Delete( work->cellUnit );
  GFL_CLACT_SYS_Delete();

  GFL_BG_FreeBGControl( PSTATUS_BG_SUB_TITLE );
  GFL_BG_FreeBGControl( PSTATUS_BG_SUB_PLATE );
  GFL_BG_FreeBGControl( PSTATUS_BG_SUB_BG );
  GFL_BG_FreeBGControl( PSTATUS_BG_MAIN_BG );
  GFL_BG_FreeBGControl( PSTATUS_BG_PLATE );
  GFL_BG_FreeBGControl( PSTATUS_BG_PARAM );
  GFL_BG_FreeBGControl( PSTATUS_BG_3D );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();

}

//--------------------------------------------------------------------------
//  Bg初期化 機能部
//--------------------------------------------------------------------------
static void PSTATUS_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

#pragma mark [>Resource
//--------------------------------------------------------------------------
//  リソース読み込み
//--------------------------------------------------------------------------
static void PSTATUS_LoadResource( PSTATUS_WORK *work )
{
  ARCHANDLE *archandle = GFL_ARC_OpenDataHandle( ARCID_P_STATUS , work->heapId );

  //下画面共通パレット
  GFL_ARCHDL_UTIL_TransVramPalette( archandle , NARC_p_status_gra_p_st_bg_d_NCLR , 
                    PALTYPE_MAIN_BG , PSTATUS_BG_PLT_MAIN*16*2 , 0 , work->heapId );
  //下画面共通キャラ
  GFL_ARCHDL_UTIL_TransVramBgCharacter( archandle , NARC_p_status_gra_p_st_bg_d_NCGR ,
                    PSTATUS_BG_PLATE , 0 , 0, FALSE , work->heapId );
  //下画面背景
  GFL_ARCHDL_UTIL_TransVramScreen( archandle , NARC_p_status_gra_p_st_scroll_d_NSCR , 
                    PSTATUS_BG_MAIN_BG , 0 , 0, FALSE , work->heapId );

  //上画面共通パレット
  GFL_ARCHDL_UTIL_TransVramPalette( archandle , NARC_p_status_gra_p_st_bg_u_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  //上画面共通キャラ
  GFL_ARCHDL_UTIL_TransVramBgCharacter( archandle , NARC_p_status_gra_p_st_bg_u_NCGR ,
                    PSTATUS_BG_SUB_PLATE , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( archandle , NARC_p_status_gra_p_st_bg_title_u_NCGR ,
                    PSTATUS_BG_SUB_TITLE , 0 , 0, FALSE , work->heapId );

  //上画面背景
  GFL_ARCHDL_UTIL_TransVramScreen( archandle , NARC_p_status_gra_p_st_scroll_u_NSCR , 
                    PSTATUS_BG_SUB_BG ,  0 , 0, FALSE , work->heapId );
  //上画面info
  GFL_ARCHDL_UTIL_TransVramScreen( archandle , NARC_p_status_gra_p_st_infotitle_u_NSCR , 
                    PSTATUS_BG_SUB_TITLE ,  0 , 0, FALSE , work->heapId );


  //OBJリソース
  //パレット
    //ボタン
  work->cellRes[SCR_PLT_ICON_COMMON] = GFL_CLGRP_PLTT_RegisterEx( archandle , 
        NARC_p_status_gra_p_st_obj_d_NCLR , CLSYS_DRAW_MAIN , 
        PSTATUS_OBJPLT_ICON*32 , 0 , APP_COMMON_BARICON_PLT_NUM+1 , work->heapId  );
  work->cellRes[SCR_PLT_SKILL] = GFL_CLGRP_PLTT_RegisterEx( archandle , 
        NARC_p_status_gra_p_st_skill_plate_NCLR , CLSYS_DRAW_MAIN , 
        PSTATUS_OBJPLT_SKILL_PLATE*32 , 0 , 2 , work->heapId  );
  work->cellRes[SCR_PLT_RIBBON_ICON] = GFL_CLGRP_PLTT_RegisterEx( archandle , 
        NARC_p_status_gra_ribbon_NCLR , CLSYS_DRAW_SUB , 
        PSTATUS_OBJPLT_SUB_RIBBON*32 , 0 , 5 , work->heapId  );
  work->cellRes[SCR_PLT_RIBBON_BAR] = GFL_CLGRP_PLTT_RegisterEx( archandle , 
        NARC_p_status_gra_p_st_ribbon_bar_NCLR , CLSYS_DRAW_MAIN , 
        PSTATUS_OBJPLT_RIBBON_BAR*32 , 0 , 1 , work->heapId  );
  work->cellRes[SCR_PLT_CURSOR_COMMON] = GFL_CLGRP_PLTT_RegisterEx( archandle , 
        NARC_p_status_gra_p_st_cursor_common_NCLR , CLSYS_DRAW_MAIN , 
        PSTATUS_OBJPLT_CURSOR_COMMON*32 , 0 , 1 , work->heapId  );
        
  //キャラクタ
  work->cellRes[SCR_NCG_ICON] = GFL_CLGRP_CGR_Register( archandle , 
        NARC_p_status_gra_p_st_obj_d_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  work->cellRes[SCR_NCG_SKILL] = GFL_CLGRP_CGR_Register( archandle , 
        NARC_p_status_gra_p_st_skill_plate_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  work->cellRes[SCR_NCG_SKILL_CUR] = GFL_CLGRP_CGR_Register( archandle , 
        NARC_p_status_gra_p_st_skill_cur_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  work->cellRes[SCR_NCG_RIBBON_CUR] = GFL_CLGRP_CGR_Register( archandle , 
        NARC_p_status_gra_p_st_ribbon_cur_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  
  //セル・アニメ
  work->cellRes[SCR_ANM_ICON] = GFL_CLGRP_CELLANIM_Register( archandle , 
        NARC_p_status_gra_p_st_obj_d_NCER , NARC_p_status_gra_p_st_obj_d_NANR, work->heapId  );
  work->cellRes[SCR_ANM_SKILL] = GFL_CLGRP_CELLANIM_Register( archandle , 
        NARC_p_status_gra_p_st_skill_plate_NCER , NARC_p_status_gra_p_st_skill_plate_NANR, work->heapId  );
  work->cellRes[SCR_ANM_SKILL_CUR] = GFL_CLGRP_CELLANIM_Register( archandle , 
        NARC_p_status_gra_p_st_skill_cur_NCER , NARC_p_status_gra_p_st_skill_cur_NANR, work->heapId  );
  work->cellRes[SCR_ANM_RIBBON_ICON] = GFL_CLGRP_CELLANIM_Register( archandle , 
        NARC_p_status_gra_ribbon_contest_00_NCER , NARC_p_status_gra_ribbon_contest_00_NANR, work->heapId  );
  work->cellRes[SCR_ANM_RIBBON_CUR] = GFL_CLGRP_CELLANIM_Register( archandle , 
        NARC_p_status_gra_p_st_ribbon_cur_NCER , NARC_p_status_gra_p_st_ribbon_cur_NANR, work->heapId  );

  {
    u8 i;
    //他のarcからの読み込み
    ARCHANDLE *archandleCommon = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId() , work->heapId );

    //下画面バー
    GFL_ARCHDL_UTIL_TransVramPalette( archandleCommon , APP_COMMON_GetBarPltArcIdx() , 
                      PALTYPE_MAIN_BG , PSTATUS_BG_PLT_BAR*16*2 , 16*2 , work->heapId );
    GFL_ARCHDL_UTIL_TransVramBgCharacter( archandleCommon , APP_COMMON_GetBarCharArcIdx() ,
                      PSTATUS_BG_PARAM , 0 , 0, FALSE , work->heapId );
    GFL_ARCHDL_UTIL_TransVramScreen( archandleCommon , APP_COMMON_GetBarScrnArcIdx() , 
                      PSTATUS_BG_PARAM , 0 , 0, FALSE , work->heapId );
    GFL_BG_ChangeScreenPalette( PSTATUS_BG_PARAM , 0 , 21 , 32 , 3 , PSTATUS_BG_PLT_BAR );


    //hpバー描画色
    GFL_ARCHDL_UTIL_TransVramPalette( archandleCommon , NARC_app_menu_common_hp_bar_NCLR , 
                      PALTYPE_SUB_BG , PSTATUS_BG_SUB_PLT_HPBAR*16*2 , 16*2 , work->heapId );
    
    //HPバー土台
    work->cellRes[SCR_PLT_HPBASE] = GFL_CLGRP_PLTT_RegisterEx( archandleCommon , 
          NARC_app_menu_common_hp_dodai_NCLR , CLSYS_DRAW_SUB , 
          PSTATUS_OBJPLT_SUB_HPBAR*32 , 0 , 1 , work->heapId  );
    work->cellRes[SCR_NCG_HPBASE] = GFL_CLGRP_CGR_Register( archandleCommon , 
          NARC_app_menu_common_hp_dodai_32k_NCGR , FALSE , CLSYS_DRAW_SUB , work->heapId  );
    work->cellRes[SCR_ANM_HPBASE] = GFL_CLGRP_CELLANIM_Register( archandleCommon , 
          NARC_app_menu_common_hp_dodai_32k_NCER , NARC_app_menu_common_hp_dodai_32k_NANR, work->heapId  );

    //タイプアイコン
    work->cellRes[SCR_PLT_POKE_TYPE] = GFL_CLGRP_PLTT_RegisterEx( archandleCommon , 
          APP_COMMON_GetPokeTypePltArcIdx() , CLSYS_DRAW_MAIN , 
          PSTATUS_OBJPLT_POKE_TYPE*32 , 0 , APP_COMMON_POKETYPE_PLT_NUM , work->heapId  );
    work->cellRes[SCR_PLT_SUB_POKE_TYPE] = GFL_CLGRP_PLTT_RegisterEx( archandleCommon , 
          APP_COMMON_GetPokeTypePltArcIdx() , CLSYS_DRAW_SUB , 
          PSTATUS_OBJPLT_SUB_POKE_TYPE*32 , 0 , APP_COMMON_POKETYPE_PLT_NUM , work->heapId  );
    work->cellRes[SCR_ANM_POKE_TYPE] = GFL_CLGRP_CELLANIM_Register( archandleCommon , 
          APP_COMMON_GetPokeTypeCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetPokeTypeAnimeArcIdx(APP_COMMON_MAPPING_128K), work->heapId  );
    work->cellRes[SCR_ANM_SUB_POKE_TYPE] = GFL_CLGRP_CELLANIM_Register( archandleCommon , 
          APP_COMMON_GetPokeTypeCellArcIdx(APP_COMMON_MAPPING_32K) , 
          APP_COMMON_GetPokeTypeAnimeArcIdx(APP_COMMON_MAPPING_32K), work->heapId  );
    //属性
    for( i=0;i<POKETYPE_MAX;i++ )
    {
      work->cellResTypeNcg[i] = GFL_CLGRP_CGR_Register( archandleCommon , 
         APP_COMMON_GetPokeTypeCharArcIdx(i) , FALSE , CLSYS_DRAW_MAIN , work->heapId );
    }
    //技タイプ
    work->cellRes[SCR_NCG_SKILL_TYPE_HENKA] = GFL_CLGRP_CGR_Register( archandleCommon , 
          NARC_app_menu_common_p_st_bunrui_henka_NCGR , FALSE , CLSYS_DRAW_SUB , work->heapId  );
    work->cellRes[SCR_NCG_SKILL_TYPE_BUTURI] = GFL_CLGRP_CGR_Register( archandleCommon , 
          NARC_app_menu_common_p_st_bunrui_buturi_NCGR , FALSE , CLSYS_DRAW_SUB , work->heapId  );
    work->cellRes[SCR_NCG_SKILL_TYPE_TOKUSHU] = GFL_CLGRP_CGR_Register( archandleCommon , 
          NARC_app_menu_common_p_st_bunrui_tokusyu_NCGR , FALSE , CLSYS_DRAW_SUB , work->heapId  );
    work->cellRes[SCR_NCG_ICON_COMMON] = GFL_CLGRP_CGR_Register( archandleCommon , 
          APP_COMMON_GetBarIconCharArcIdx() , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
    work->cellRes[SCR_ANM_ICON_COMMON] = GFL_CLGRP_CELLANIM_Register( archandleCommon , 
          APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K), 
          work->heapId  );
    //ポケルス
    work->cellRes[SCR_PLT_POKERUSU] = GFL_CLGRP_PLTT_RegisterEx( archandleCommon , 
          APP_COMMON_GetPokerusPltArcIdx() , CLSYS_DRAW_MAIN , 
          PSTATUS_OBJPLT_POKERUSU*32 , 0 , 1 , work->heapId  );
    work->cellRes[SCR_NCG_POKERUSU] = GFL_CLGRP_CGR_Register( archandleCommon , 
          APP_COMMON_GetPokerusCharArcIdx(APP_COMMON_MAPPING_128K) , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
    work->cellRes[SCR_ANM_POKERUSU] = GFL_CLGRP_CELLANIM_Register( archandleCommon , 
          APP_COMMON_GetPokerusCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetPokerusAnimeArcIdx(APP_COMMON_MAPPING_128K), work->heapId  );
    //マーク
    work->cellRes[SCR_PLT_MARK] = GFL_CLGRP_PLTT_RegisterEx( archandleCommon , 
          APP_COMMON_GetPokeMarkPltArcIdx() , CLSYS_DRAW_MAIN , 
          PSTATUS_OBJPLT_MARK*32 , 0 , 1 , work->heapId  );
    work->cellRes[SCR_NCG_MARK] = GFL_CLGRP_CGR_Register( archandleCommon , 
          APP_COMMON_GetPokeMarkCharArcIdx(APP_COMMON_MAPPING_128K) , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
    work->cellRes[SCR_ANM_MARK] = GFL_CLGRP_CELLANIM_Register( archandleCommon , 
          APP_COMMON_GetPokeMarkCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetPokeMarkAnimeArcIdx(APP_COMMON_MAPPING_128K), work->heapId  );
    //状態異常
    work->cellRes[SCR_PLT_ST_IJYOU] = GFL_CLGRP_PLTT_RegisterEx( archandleCommon , 
          APP_COMMON_GetStatusIconPltArcIdx() , CLSYS_DRAW_MAIN , 
          PSTATUS_OBJPLT_IJYOU*32 , 0 , 1 , work->heapId  );
    work->cellRes[SCR_NCG_ST_IJYOU] = GFL_CLGRP_CGR_Register( archandleCommon , 
          APP_COMMON_GetStatusIconCharArcIdx() , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
    work->cellRes[SCR_ANM_ST_IJYOU] = GFL_CLGRP_CELLANIM_Register( archandleCommon , 
          APP_COMMON_GetStatusIconCellArcIdx(APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetStatusIconAnimeArcIdx(APP_COMMON_MAPPING_128K), work->heapId  );
    //ボール
    work->cellRes[SCR_PLT_BALL] = GFL_CLGRP_PLTT_RegisterEx( archandleCommon , 
          APP_COMMON_GetBallPltArcIdx(1) , CLSYS_DRAW_MAIN , 
          PSTATUS_OBJPLT_BALL*32 , 0 , 1 , work->heapId  );
    work->cellRes[SCR_NCG_BALL] = GFL_CLGRP_CGR_Register( archandleCommon , 
          APP_COMMON_GetBallCharArcIdx(1) , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
    work->cellRes[SCR_ANM_BALL] = GFL_CLGRP_CELLANIM_Register( archandleCommon , 
          APP_COMMON_GetBallCellArcIdx(1,APP_COMMON_MAPPING_128K) , 
          APP_COMMON_GetBallAnimeArcIdx(1,APP_COMMON_MAPPING_128K), work->heapId  );

    
    GFL_ARC_CloseDataHandle(archandleCommon);
  }

  PSTATUS_SUB_LoadResource( work , work->subWork , archandle );
  PSTATUS_INFO_LoadResource( work , work->infoWork , archandle );
  PSTATUS_SKILL_LoadResource( work , work->skillWork , archandle );
  PSTATUS_RIBBON_LoadResource( work , work->ribbonWork , archandle );


  GFL_ARC_CloseDataHandle(archandle);
}

//--------------------------------------------------------------------------
//  リソース開放
//--------------------------------------------------------------------------
static void PSTATUS_ReleaseResource( PSTATUS_WORK *work )
{
  u8 i;
  
  PSTATUS_RIBBON_ReleaseResource( work , work->ribbonWork );
  PSTATUS_SKILL_ReleaseResource( work , work->skillWork );
  PSTATUS_INFO_ReleaseResource( work , work->infoWork );
  PSTATUS_SUB_ReleaseResource( work , work->subWork );

  //OBJ
  for( i=SCR_PLT_START ; i<=SCR_PLT_END ; i++ )
  {
    GFL_CLGRP_PLTT_Release( work->cellRes[i] );
  }
  for( i=SCR_NCG_START ; i<=SCR_NCG_END ; i++ )
  {
    GFL_CLGRP_CGR_Release( work->cellRes[i] );
  }
  for( i=SCR_ANM_START ; i<=SCR_ANM_END ; i++ )
  {
    GFL_CLGRP_CELLANIM_Release( work->cellRes[i] );
  }
    //属性
  for( i=0;i<POKETYPE_MAX;i++ )
  {
    GFL_CLGRP_CGR_Release( work->cellResTypeNcg[i] );
  }
  
}

//--------------------------------------------------------------------------
//  セル初期化
//--------------------------------------------------------------------------
static void PSTATUS_InitCell( PSTATUS_WORK *work )
{
  //バーのボタン
  {
    const u8 anmIdxArr[SBT_MAX] =
    {
      SBA_PAGE1_NORMAL,
      SBA_PAGE2_NORMAL,
      SBA_PAGE3_NORMAL,
      APP_COMMON_BARICON_CHECK_OFF,
      APP_COMMON_BARICON_CURSOR_UP,
      APP_COMMON_BARICON_CURSOR_DOWN,
      APP_COMMON_BARICON_EXIT,
      APP_COMMON_BARICON_RETURN,
    };
    const u8 posXArr[SBT_MAX] =
    {
      PSTATUS_BAR_CELL_PAGE1_X,
      PSTATUS_BAR_CELL_PAGE2_X,
      PSTATUS_BAR_CELL_PAGE3_X,
      PSTATUS_BAR_CELL_CHECK_X+4,
      PSTATUS_BAR_CELL_CURSOR_UP_X,
      PSTATUS_BAR_CELL_CURSOR_DOWN_X,
      PSTATUS_BAR_CELL_CURSOR_EXIT,
      PSTATUS_BAR_CELL_CURSOR_RETURN,
    };
    u8 i;
    GFL_CLWK_DATA cellInitData;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 0;
    
    for( i=0;i<SBT_MAX;i++ )
    {
      u32 ncgRes;
      u32 anmRes;
      if( i<=SBT_PAGE3 )
      {
        ncgRes = work->cellRes[SCR_NCG_ICON];
        anmRes = work->cellRes[SCR_ANM_ICON];
      }
      else
      {
        ncgRes = work->cellRes[SCR_NCG_ICON_COMMON];
        anmRes = work->cellRes[SCR_ANM_ICON_COMMON];
      }
      cellInitData.pos_x = posXArr[i];
      cellInitData.pos_y = ( i == SBT_CHECK ? PSTATUS_BAR_CELL_Y+4 : PSTATUS_BAR_CELL_Y);
      cellInitData.anmseq = anmIdxArr[i];
      work->clwkBarIcon[i] = GFL_CLACT_WK_Create( work->cellUnit ,
                ncgRes,
                work->cellRes[SCR_PLT_ICON_COMMON],
                anmRes,
                &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
      GFL_CLACT_WK_SetAutoAnmFlag( work->clwkBarIcon[i] , TRUE );
      GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[i] , TRUE );
    }
    
    cellInitData.pos_x = 0;
    cellInitData.pos_y = 0;
    cellInitData.anmseq = 0;
    work->clwkTypeIcon[0] = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellResTypeNcg[0],
              work->cellRes[SCR_PLT_POKE_TYPE],
              work->cellRes[SCR_ANM_POKE_TYPE],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );
    work->clwkTypeIcon[1] = GFL_CLACT_WK_Create( work->cellUnit ,
              work->cellResTypeNcg[0],
              work->cellRes[SCR_PLT_POKE_TYPE],
              work->cellRes[SCR_ANM_POKE_TYPE],
              &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );


    for( i=0;i<2;i++ )
    {
      GFL_CLACT_WK_SetDrawEnable( work->clwkTypeIcon[i] , FALSE );
    }
  }
  
  //技モードではリターン以外消す
  if( work->psData->mode == PST_MODE_WAZAADD )
  {
    u8 i;
    for( i=0;i<SBT_MAX;i++ )
    {
      if( i != SBT_RETURN )
      {
        GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[i] , FALSE );
      }
    }
  }
  //キャンセル不可はショートカットも消す
  if( PSTATUS_UTIL_CanUseExitButton(work) == FALSE )
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_EXIT] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_CHECK] , FALSE );
  }
  
  PSTATUS_SUB_InitCell( work , work->subWork );
  PSTATUS_RIBBON_InitCell( work , work->ribbonWork );
  PSTATUS_SKILL_InitCell( work , work->skillWork );
}

//--------------------------------------------------------------------------
//  セル開放
//--------------------------------------------------------------------------
static void PSTATUS_TermCell( PSTATUS_WORK *work )
{
  u8 i;
  PSTATUS_RIBBON_TermCell( work , work->ribbonWork );
  PSTATUS_SKILL_TermCell( work , work->skillWork );
  PSTATUS_SUB_TermCell( work , work->subWork );

  for( i=0;i<SBT_MAX;i++ )
  {
    GFL_CLACT_WK_Remove( work->clwkBarIcon[i] );
  }
  for( i=0;i<2;i++ )
  {
    GFL_CLACT_WK_Remove( work->clwkTypeIcon[i] );
  }
}

//--------------------------------------------------------------------------
//  メッセージ系初期化
//--------------------------------------------------------------------------
static void PSTATUS_InitMessage( PSTATUS_WORK *work )
{
  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_gftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  
  //メッセージ
  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_pokestatus_dat , work->heapId );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , PSTATUS_BG_PLT_FONT*16*2, 16*2, work->heapId );
  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_SUB_BG , PSTATUS_BG_SUB_PLT_FONT*16*2, 16*2, work->heapId );
  
  {
    u16 *palAdr = (u16*)(HW_DB_BG_PLTT+PSTATUS_BG_SUB_PLT_FONT*32);
    palAdr[ 9] = GX_RGB(25,18,19);
    palAdr[10] = GX_RGB(18,18,26);
  }

  
  work->printQue = PRINTSYS_QUE_CreateEx( 4096 , work->heapId );
}

//--------------------------------------------------------------------------
//  メッセージ系開放
//--------------------------------------------------------------------------
static void PSTATUS_TermMessage( PSTATUS_WORK *work )
{
  PRINTSYS_QUE_Clear( work->printQue );
  PRINTSYS_QUE_Delete( work->printQue );
  GFL_MSG_Delete( work->msgHandle );
  GFL_FONT_Delete( work->fontHandle );
}


#pragma mark [>UI
//--------------------------------------------------------------------------
//  バーのボタンの一括チェック
//--------------------------------------------------------------------------
static void PSTATUS_UpdateBarButton( PSTATUS_WORK *work )
{
  //当たり判定作成
  GFL_UI_TP_HITTBL hitTbl[SBT_MAX+1] =
  {
    { PSTATUS_BAR_CELL_Y , PSTATUS_BAR_CELL_Y+24 , PSTATUS_BAR_CELL_PAGE1_X , PSTATUS_BAR_CELL_PAGE1_X+40 },
    { PSTATUS_BAR_CELL_Y , PSTATUS_BAR_CELL_Y+24 , PSTATUS_BAR_CELL_PAGE2_X , PSTATUS_BAR_CELL_PAGE2_X+40 },
    { PSTATUS_BAR_CELL_Y , PSTATUS_BAR_CELL_Y+24 , PSTATUS_BAR_CELL_PAGE3_X , PSTATUS_BAR_CELL_PAGE3_X+40 },
    { PSTATUS_BAR_CELL_Y , PSTATUS_BAR_CELL_Y+24 , PSTATUS_BAR_CELL_CHECK_X , PSTATUS_BAR_CELL_CHECK_X+24 },
    { PSTATUS_BAR_CELL_Y , PSTATUS_BAR_CELL_Y+24 , PSTATUS_BAR_CELL_CURSOR_UP_X , PSTATUS_BAR_CELL_CURSOR_UP_X+24 },
    { PSTATUS_BAR_CELL_Y , PSTATUS_BAR_CELL_Y+24 , PSTATUS_BAR_CELL_CURSOR_DOWN_X , PSTATUS_BAR_CELL_CURSOR_DOWN_X+24 },
    { PSTATUS_BAR_CELL_Y , PSTATUS_BAR_CELL_Y+24 , PSTATUS_BAR_CELL_CURSOR_EXIT , PSTATUS_BAR_CELL_CURSOR_EXIT+24 },
    { PSTATUS_BAR_CELL_Y , PSTATUS_BAR_CELL_Y+24 , PSTATUS_BAR_CELL_CURSOR_RETURN , PSTATUS_BAR_CELL_CURSOR_RETURN+24 },
    { GFL_UI_TP_HIT_END ,0,0,0 },
  };
  
  work->barButtonHit = GFL_UI_TP_HitTrg( hitTbl );
}

//--------------------------------------------------------------------------
//  操作系更新
//--------------------------------------------------------------------------
static void PSTATUS_UpdateUI( PSTATUS_WORK *work )
{
  BOOL isChangePage;
#if USE_STATUS_DEBUG
  if( DEBUGWIN_IsActive() == TRUE ) return;
#endif
  
  if( work->isActiveBarButton == TRUE &&
      PRINTSYS_QUE_IsFinished( work->printQue ) == TRUE )
  {
    if( PSTATUS_UpdateKey(work) == FALSE )
    {
      PSTATUS_UpdateTP( work );
    }
  }
}

//--------------------------------------------------------------------------
//  キー操作更新(ページ切り替え以外
//--------------------------------------------------------------------------
static const BOOL PSTATUS_UpdateKey( PSTATUS_WORK *work )
{
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN )
  {
    const BOOL isChange = PSTATUS_ChangeData( work , TRUE );
    if( isChange == TRUE )
    {
      PSTATUS_RefreshDisp( work );
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_UP] , APP_COMMON_BARICON_CURSOR_UP );
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_DOWN] , APP_COMMON_BARICON_CURSOR_DOWN_ON );
      work->isAnimeBarCursor[0] = FALSE;
      work->isAnimeBarCursor[1] = TRUE;
      PMSND_PlaySystemSE(PSTATUS_SND_PAGE_UD);
      work->ktst = GFL_APP_END_KEY;
      return TRUE;
    }
  }
  else
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP )
  {
    const BOOL isChange = PSTATUS_ChangeData( work , FALSE );
    if( isChange == TRUE )
    {
      PSTATUS_RefreshDisp( work );
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_UP] , APP_COMMON_BARICON_CURSOR_UP_ON );
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_DOWN] , APP_COMMON_BARICON_CURSOR_DOWN );
      work->isAnimeBarCursor[0] = TRUE;
      work->isAnimeBarCursor[1] = FALSE;
      PMSND_PlaySystemSE(PSTATUS_SND_PAGE_UD);
      work->ktst = GFL_APP_END_KEY;
      return TRUE;
    }
  }
  else
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    u8 maxPage = PPT_RIBBON;
    if( work->isHaveRibbon == FALSE )
    {
      maxPage = PPT_SKILL;
    }
    if( work->page < maxPage &&
        work->isEgg == FALSE )
    {
      work->page++;
      PSTATUS_RefreshDisp( work );
      PMSND_PlaySystemSE(PSTATUS_SND_PAGE_LR);
      work->ktst = GFL_APP_END_KEY;
      return TRUE;
    }
  }
  else
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    if( work->page > PPT_INFO &&
        work->isEgg == FALSE )
    {
      work->page--;
      PSTATUS_RefreshDisp( work );
      PMSND_PlaySystemSE(PSTATUS_SND_PAGE_LR);
      work->ktst = GFL_APP_END_KEY;
      return TRUE;
    }
  }
  else
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
  {
    work->retVal = SRT_RETURN;
    work->psData->ret_mode = PST_RET_CANCEL;
    work->mainSeq = SMS_FADEOUT;
    GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_RETURN] , APP_COMMON_BARICON_RETURN_ON );
    work->clwkExitButton = work->clwkBarIcon[SBT_RETURN];
    PMSND_PlaySystemSE(PSTATUS_SND_CANCEL);
    work->ktst = GFL_APP_END_KEY;
    return TRUE;
  }
  else
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X &&
      PSTATUS_UTIL_CanUseExitButton(work) == TRUE )
  {
    work->retVal = SRT_EXIT;
    work->psData->ret_mode = PST_RET_EXIT;
    work->mainSeq = SMS_FADEOUT;
    GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_EXIT] , APP_COMMON_BARICON_EXIT_ON );
    work->clwkExitButton = work->clwkBarIcon[SBT_EXIT];
    PMSND_PlaySystemSE(PSTATUS_SND_EXIT);
    work->ktst = GFL_APP_END_KEY;
    return TRUE;
  }
  else
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
  {
    if(  PSTATUS_UTIL_CanUseExitButton(work) == TRUE )
    {
      if( work->page < PPT_SKILL_ADD )
      {
        if( work->shortCutCheck[work->page] == TRUE )
        {
          work->shortCutCheck[work->page] = FALSE;
          GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CHECK] , APP_COMMON_BARICON_CHECK_OFF );
        }
        else
        {
          work->shortCutCheck[work->page] = TRUE;
          GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CHECK] , APP_COMMON_BARICON_CHECK_ON );
        }
        PMSND_PlaySystemSE(PSTATUS_SND_SHORTCUT);
        work->ktst = GFL_APP_END_KEY;
      }
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------
//  キー操作更新(ページ切り替えのみ
//--------------------------------------------------------------------------
static const BOOL PSTATUS_UpdateKey_Page( PSTATUS_WORK *work )
{
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    u8 maxPage = PPT_RIBBON;
    if( work->isHaveRibbon == FALSE )
    {
      maxPage = PPT_SKILL;
    }
    if( work->page < maxPage &&
        work->isEgg == FALSE )
    {
      work->page++;
      PSTATUS_RefreshDisp( work );
      PMSND_PlaySystemSE(PSTATUS_SND_PAGE_LR);
      work->ktst = GFL_APP_END_TOUCH;
      return TRUE;
    }
  }
  else
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    if( work->page > PPT_INFO &&
        work->isEgg == FALSE )
    {
      work->page--;
      PSTATUS_RefreshDisp( work );
      PMSND_PlaySystemSE(PSTATUS_SND_PAGE_LR);
      work->ktst = GFL_APP_END_TOUCH;
      return TRUE;
    }
  }
  return FALSE;
}

//--------------------------------------------------------------------------
//  TP操作更新
//--------------------------------------------------------------------------
static void PSTATUS_UpdateTP( PSTATUS_WORK *work )
{
  //ボタンの判定は別でやっているのでここではチェックのみ
  switch( work->barButtonHit )
  {
  case SBT_PAGE1:
    if( work->page != PPT_INFO )
    {
      work->page = PPT_INFO;
      PSTATUS_RefreshDisp( work );
      PMSND_PlaySystemSE(PSTATUS_SND_PAGE_LR);
      work->ktst = GFL_APP_END_TOUCH;
    }
    break;
  case SBT_PAGE2:
    if( work->page != PPT_SKILL &&
        work->isEgg == FALSE )
    {
      work->page = PPT_SKILL;
      PSTATUS_RefreshDisp( work );
      PMSND_PlaySystemSE(PSTATUS_SND_PAGE_LR);
      work->ktst = GFL_APP_END_TOUCH;
    }
    break;
  case SBT_PAGE3:
    if( work->page != PPT_RIBBON &&
        work->isEgg == FALSE &&
        work->isHaveRibbon == TRUE )
    {
      work->page = PPT_RIBBON;
      PSTATUS_RefreshDisp( work );
      PMSND_PlaySystemSE(PSTATUS_SND_PAGE_LR);
      work->ktst = GFL_APP_END_TOUCH;
    }
    break;
  case SBT_CHECK:
    if( work->page < PPT_SKILL_ADD && 
        PSTATUS_UTIL_CanUseExitButton(work) == TRUE )
    {
      if( work->shortCutCheck[work->page] == TRUE )
      {
        work->shortCutCheck[work->page] = FALSE;
        GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CHECK] , APP_COMMON_BARICON_CHECK_OFF );
      }
      else
      {
        work->shortCutCheck[work->page] = TRUE;
        GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CHECK] , APP_COMMON_BARICON_CHECK_ON );
      }
      PMSND_PlaySystemSE(PSTATUS_SND_SHORTCUT);
      work->ktst = GFL_APP_END_TOUCH;
    }    
    break;
  case SBT_CURSOR_UP:
    {
      const BOOL isChange = PSTATUS_ChangeData( work , FALSE );
      if( isChange == TRUE )
      {
        PSTATUS_RefreshDisp( work );
        GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_UP] , APP_COMMON_BARICON_CURSOR_UP_ON );
        GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_DOWN] , APP_COMMON_BARICON_CURSOR_DOWN );
        work->isAnimeBarCursor[0] = TRUE;
        work->isAnimeBarCursor[1] = FALSE;
        PMSND_PlaySystemSE(PSTATUS_SND_PAGE_UD);
        work->ktst = GFL_APP_END_TOUCH;
      }
    }
    break;
  case SBT_CURSOR_DOWN:
    {
      const BOOL isChange = PSTATUS_ChangeData( work , TRUE );
      if( isChange == TRUE )
      {
        PSTATUS_RefreshDisp( work );
        GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_UP] , APP_COMMON_BARICON_CURSOR_UP );
        GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_DOWN] , APP_COMMON_BARICON_CURSOR_DOWN_ON );
        work->isAnimeBarCursor[0] = FALSE;
        work->isAnimeBarCursor[1] = TRUE;
        PMSND_PlaySystemSE(PSTATUS_SND_PAGE_UD);
        work->ktst = GFL_APP_END_TOUCH;
      }
    }
    break;
  case SBT_EXIT:
    if( PSTATUS_UTIL_CanUseExitButton(work) == TRUE )
    {
      work->retVal = SRT_EXIT;
      work->psData->ret_mode = PST_RET_EXIT;
      work->mainSeq = SMS_FADEOUT;
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_EXIT] , APP_COMMON_BARICON_EXIT_ON );
      work->clwkExitButton = work->clwkBarIcon[SBT_EXIT];
      PMSND_PlaySystemSE(PSTATUS_SND_EXIT);
      work->ktst = GFL_APP_END_TOUCH;
    }
    break;
  case SBT_RETURN:
    work->retVal = SRT_RETURN;
    work->psData->ret_mode = PST_RET_CANCEL;
    work->mainSeq = SMS_FADEOUT;
    GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_RETURN] , APP_COMMON_BARICON_RETURN_ON );
    work->clwkExitButton = work->clwkBarIcon[SBT_RETURN];
    PMSND_PlaySystemSE(PSTATUS_SND_CANCEL);
    work->ktst = GFL_APP_END_TOUCH;
    break;
    
  }
}

#pragma mark [>pallet anime
static void PSTATUS_PALANIM_UpdatePalletAnime( PSTATUS_WORK *work )
{
  u8 i;
  s8 colOfs;
  if( work->anmCnt + PSTATUS_PALANIM_SPEED >= 0x10000 )
  {
    work->anmCnt = PSTATUS_PALANIM_SPEED + work->anmCnt - 0x10000;
  }
  else
  {
    work->anmCnt += PSTATUS_PALANIM_SPEED;
  }
  
  /*
  //色の補正値計算
  colOfs = (s8)(FX_FX32_TO_F32( FX_SinIdx( work->anmCnt )*PSTATUS_PALANIM_COL_LENGTH ));
  OS_TPrintf("[%d]\n",colOfs);
  for( i=0;i<PSTATUS_PALANIM_NUM;i++ )
  {
    s8 r = (work->anmPalBase[i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
    s8 g = (work->anmPalBase[i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
    s8 b = (work->anmPalBase[i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
//      r = (r<anmRate*2?0:r-(anmRate*2));
//      g = (g<anmRate*2?0:g-(anmRate*2));
//      b = (b<anmRate*2?0:b-(anmRate*2));
    r+=colOfs;
    g+=colOfs;
    b+=colOfs;
    if( r < 0 ) r = 0;
    if( g < 0 ) g = 0;
    if( b < 0 ) b = 0;
    if( r > 31 ) r = 31;
    if( g > 31 ) g = 31;
    if( b > 31 ) b = 31;
    work->anmPal[i] = GX_RGB(r,g,b);

  }
  */
  
  //BARのカテゴリアイコン
  {
    //青
//    const u16 startCol[PSTATUS_PALANIM_NUM] = { 0x4e2c,0x39a9,0x2506,0x1083 };
//    const u16 endCol[PSTATUS_PALANIM_NUM] = { 0x7f96,0x7332,0x62af,0x522b };
    //白
    const u16 startCol[PSTATUS_PALANIM_NUM] = { 0x5a94,0x4a10,0x398c,0x39ad };
    const u16 endCol[PSTATUS_PALANIM_NUM] = { 0x7fff,0x7fff,0x5e52,0x5231 };
    //白青
//    const u16 startCol[PSTATUS_PALANIM_NUM] = { 0x7fff,0x7fff,0x5e52,0x5231 };
//    const u16 endCol[PSTATUS_PALANIM_NUM] = { 0x7f96,0x7332,0x62af,0x522b };
    const float rate = FX_FX32_TO_F32((FX_SinIdx(work->anmCnt)+FX16_ONE)/2);
    for( i=0;i<PSTATUS_PALANIM_NUM;i++ )
    {
      s8 r,b,g;
      s8 sr = (startCol[i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
      s8 sg = (startCol[i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
      s8 sb = (startCol[i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
      s8 er = (endCol[i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
      s8 eg = (endCol[i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
      s8 eb = (endCol[i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;

      r = sr + ((er-sr)*rate);
      g = sg + ((eg-sg)*rate);
      b = sb + ((eb-sb)*rate);

      if( r < 0 ) r = 0;
      if( g < 0 ) g = 0;
      if( b < 0 ) b = 0;
      if( r > 31 ) r = 31;
      if( g > 31 ) g = 31;
      if( b > 31 ) b = 31;
      work->anmPal[i] = GX_RGB(r,g,b);
      
  //    OS_TPrintf("[%4x]",GX_RGB(r,g,b));
    }
  //  OS_TPrintf("\n");
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_OBJ_PLTT_MAIN ,
                                        PSTATUS_PALANIM_PAL_NO * 32 + PSTATUS_PALANIM_PAL_POS * 2 ,
                                        work->anmPal , 2*PSTATUS_PALANIM_NUM );

    //スキルカーソル
    for( i=0;i<16;i++ )
    {
      s8 r,b,g;
      s8 sr = (work->anmSkillPalBase[0][i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
      s8 sg = (work->anmSkillPalBase[0][i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
      s8 sb = (work->anmSkillPalBase[0][i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
      s8 er = (work->anmSkillPalBase[1][i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
      s8 eg = (work->anmSkillPalBase[1][i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
      s8 eb = (work->anmSkillPalBase[1][i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;

      r = sr + ((er-sr)*rate);
      g = sg + ((eg-sg)*rate);
      b = sb + ((eb-sb)*rate);

      if( r < 0 ) r = 0;
      if( g < 0 ) g = 0;
      if( b < 0 ) b = 0;
      if( r > 31 ) r = 31;
      if( g > 31 ) g = 31;
      if( b > 31 ) b = 31;
      work->anmSkillPal[i] = GX_RGB(r,g,b);
      
  //    OS_TPrintf("[%4x]",GX_RGB(r,g,b));
    }
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_OBJ_PLTT_MAIN ,
                                        (PSTATUS_OBJPLT_SKILL_PLATE+1) * 32 ,
                                        work->anmSkillPal , 32 );
    //リボンカーソル
    //CDEだけ！
    for( i=0xC;i<=0xE;i++ )
    {
      s8 r,b,g;
      s8 sr = (work->anmRibbonPalBase[i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT;
      s8 sg = (work->anmRibbonPalBase[i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT;
      s8 sb = (work->anmRibbonPalBase[i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT;
      s8 er = ((work->anmRibbonPalBase[i]&GX_RGB_R_MASK)>>GX_RGB_R_SHIFT) + 10;
      s8 eg = ((work->anmRibbonPalBase[i]&GX_RGB_G_MASK)>>GX_RGB_G_SHIFT) + 10;
      s8 eb = ((work->anmRibbonPalBase[i]&GX_RGB_B_MASK)>>GX_RGB_B_SHIFT) + 10;
      if( er > 31 )er = 31;
      if( eg > 31 )eg = 31;
      if( eb > 31 )eb = 31;
      r = sr + ((er-sr)*rate);
      g = sg + ((eg-sg)*rate);
      b = sb + ((eb-sb)*rate);

      if( r < 0 ) r = 0;
      if( g < 0 ) g = 0;
      if( b < 0 ) b = 0;
      if( r > 31 ) r = 31;
      if( g > 31 ) g = 31;
      if( b > 31 ) b = 31;
      work->anmRibbonPal[i] = GX_RGB(r,g,b);
      
  //    OS_TPrintf("[%4x]",GX_RGB(r,g,b));
    }
    //  OS_TPrintf("\n");
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_OBJ_PLTT_MAIN ,
                                        PSTATUS_OBJPLT_RIBBON_BAR * 32 ,
                                        work->anmRibbonPal , 32 );
  }
}

//外部からの操作関数
#pragma mark [>Outer
//--------------------------------------------------------------------------
//  バー操作(キーのページ切り替えも)の有効設定
//--------------------------------------------------------------------------
void PSTATUS_SetActiveBarButton( PSTATUS_WORK *work , const BOOL isActive )
{
  work->isActiveBarButton = isActive;

  //バーアイコン処理
  if( isActive == FALSE )
  {
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_PAGE1] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_PAGE2] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_PAGE3] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_CHECK] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_CURSOR_UP] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_CURSOR_DOWN] , FALSE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_EXIT] , FALSE );
    
/*
    if( work->page != PPT_INFO )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_PAGE1] , SBA_PAGE1_OFF );
    }
    if( work->page != PPT_SKILL )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_PAGE2] , SBA_PAGE2_OFF );
    }
    if( work->page != PPT_RIBBON )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_PAGE3] , SBA_PAGE3_OFF );
    }
    if( work->shortCutCheck[ work->page ] == TRUE )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CHECK] , APP_COMMON_BARICON_CHECK_ON_PASSIVE );
    }
    else
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CHECK] , APP_COMMON_BARICON_CHECK_OFF_PASSIVE );
    }
    GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_UP] , APP_COMMON_BARICON_CURSOR_UP_OFF );
    GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_DOWN] , APP_COMMON_BARICON_CURSOR_DOWN_OFF );
    if( PSTATUS_UTIL_CanUseExitButton(work) == TRUE )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_EXIT] , APP_COMMON_BARICON_EXIT_OFF );
    }
*/
  }
  else
  {
/*
    if( work->page != PPT_INFO )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_PAGE1] , SBA_PAGE1_NORMAL );
    }
    if( work->page != PPT_SKILL )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_PAGE2] , SBA_PAGE2_NORMAL );
    }
    if( work->page != PPT_RIBBON )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_PAGE3] , SBA_PAGE3_NORMAL );
    }
    if( work->shortCutCheck[ work->page ] == TRUE )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CHECK] , APP_COMMON_BARICON_CHECK_ON );
    }
    else
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CHECK] , APP_COMMON_BARICON_CHECK_OFF );
    }

    GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_UP] , APP_COMMON_BARICON_CURSOR_UP );
    GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_DOWN] , APP_COMMON_BARICON_CURSOR_DOWN );
    if( PSTATUS_UTIL_CanUseExitButton(work) == TRUE )
    {
      GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_EXIT] , APP_COMMON_BARICON_EXIT );
    }
    GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_RETURN] , APP_COMMON_BARICON_RETURN );
*/
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_PAGE1] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_PAGE2] , TRUE );
    if( work->isHaveRibbon == TRUE )
    {
      GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_PAGE3] , TRUE );
    }
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_CURSOR_UP] , TRUE );
    GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_CURSOR_DOWN] , TRUE );
    if( PSTATUS_UTIL_CanUseExitButton(work) == TRUE )
    {
      GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_CHECK] , TRUE );
      GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_EXIT] , TRUE );
    }
  }
}

#pragma mark [>util

//--------------------------------------------------------------
//キャラが変わったので表示更新
// @value isUpOder 昇順
//--------------------------------------------------------------
static const BOOL PSTATUS_ChangeData( PSTATUS_WORK *work , const BOOL isUpOder )
{
  u16 newPos;
  BOOL isChange = PSTATUS_CanChangeData( work , isUpOder , &newPos );

  if( isChange == TRUE )
  {
    work->dataPos = newPos;
    if( work->psData->ppt == PST_PP_TYPE_POKEPASO )
    {
      if( work->calcPP != NULL )
      {
        PP_Clear( work->calcPP );
        GFL_HEAP_FreeMemory( work->calcPP );
      }
      {
        const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP(work);
        work->calcPP = PP_CreateByPPP( ppp , work->heapId );
      }
    }
    PSTATUS_RefreshData( work );
  }
  return isChange;
}

static const BOOL PSTATUS_CanChangeData( PSTATUS_WORK *work , const BOOL isUpOder , u16 *newPos )
{
  BOOL isFinish = FALSE;
  BOOL isChange = FALSE;
  u16 befDataPos = work->dataPos;
  while( isFinish == FALSE )
  {
    if( isUpOder == TRUE &&
        work->dataPos >= work->psData->max-1 )
    {
      isFinish = TRUE;
    }
    else
    if( isUpOder == FALSE &&
        work->dataPos <= 0 )
    {
      isFinish = TRUE;
    }
    else
    {
      if( isUpOder == TRUE )
      {
        work->dataPos++;
      }
      else
      {
        work->dataPos--;
      }
      
      {
        const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP(work);
        
        if( PPP_Get( ppp , ID_PARA_poke_exist , NULL ) == 0 )
        {
          //データが無い(ボックス時
        }
        else
        if( PPP_Get( ppp , ID_PARA_tamago_flag , NULL ) == 1 &&
            work->page != PPT_INFO )
        {
          //タマゴの時はInfo以外見れない
        }
        else
        if( work->page == PPT_RIBBON &&
            PSTATUS_UTIL_CheckHaveAnyRibbon( work , ppp ) == FALSE )
        {
          //リボンは持ってるやつだけ
        }
        else
        {
          isFinish = TRUE;
          isChange = TRUE;
        }
      }
    }
  }
  if( newPos != NULL )
  {
    *newPos = work->dataPos;
  }
  work->dataPos = befDataPos;
  return isChange;
}

//--------------------------------------------------------------
//キャラが変わったのでデータ更新
//--------------------------------------------------------------
static void PSTATUS_RefreshData( PSTATUS_WORK *work )
{
  POKEMON_PARAM *pp = PSTATUS_UTIL_GetCurrentPP(work);
  const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP(work);
  //PPP暗号解除
  PSTATUS_UTIL_SetCurrentPPPFast( work , TRUE );
  
  work->isEgg = PP_Get( pp , ID_PARA_tamago_flag , NULL );
  work->friend = PP_Get( pp , ID_PARA_friend , NULL );
  work->isHaveRibbon = PSTATUS_UTIL_CheckHaveAnyRibbon( work , ppp );
  
  //PPP暗号化
  PSTATUS_UTIL_SetCurrentPPPFast( work , FALSE );

  work->reqPlayVoice = TRUE;

/*
  PMVOICE_Stop( 0 );
  if( work->isEgg == FALSE )
  {
    const u32 monsNo = PP_Get( pp , ID_PARA_monsno , NULL );
    const u32 formNo = PP_Get( pp , ID_PARA_form_no , NULL );
    PMVOICE_Play( monsNo , formNo , 64 , FALSE , 0 , 0 , FALSE , 0 );
  }
*/
}

//--------------------------------------------------------------
//キャラが変わったので表示更新
//--------------------------------------------------------------
static void PSTATUS_RefreshDisp( PSTATUS_WORK *work )
{
  //PPP暗号解除
  PSTATUS_UTIL_SetCurrentPPPFast( work , TRUE );
  
  if( work->befDataPos != work->dataPos )
  {
    if( work->befDataPos != 0xFF )
    {
      PSTATUS_SUB_ClearPage( work , work->subWork );
      PSTATUS_RIBBON_DeleteRibbonBar( work , work->ribbonWork );
    }
    PSTATUS_SUB_DispPage( work , work->subWork );
    PSTATUS_RIBBON_CreateRibbonBar( work , work->ribbonWork );
  }

  switch( work->befPage )
  {
  case PPT_INFO:
    PSTATUS_INFO_ClearPage( work , work->infoWork );
    break;
  case PPT_SKILL:
    PSTATUS_SKILL_ClearPage( work , work->skillWork );
    break;
  case PPT_RIBBON:
    PSTATUS_RIBBON_ClearPage( work , work->ribbonWork );
    break;
  case PPT_SKILL_ADD:
    PSTATUS_SKILL_ClearPage_WazaAdd( work , work->skillWork );
    break;
  }

  switch( work->page )
  {
  case PPT_INFO:
    PSTATUS_INFO_DispPage( work , work->infoWork );
    break;
  case PPT_SKILL:
    PSTATUS_SKILL_DispPage( work , work->skillWork );
    break;
  case PPT_RIBBON:
    PSTATUS_RIBBON_DispPage( work , work->ribbonWork );
    break;
  case PPT_SKILL_ADD:
    PSTATUS_SKILL_DispPage_WazaAdd( work , work->skillWork );
    break;
  }
  
  if( work->befPage != work->page )
  {
    work->mosaicEffSeq = SMES_FADEOUT;
    work->mosaicCnt = 0;
//    G2_BG1Mosaic( TRUE );
    G2_BG2Mosaic( TRUE );
//    G2S_BG0Mosaic( TRUE );
    G2S_BG1Mosaic( TRUE );
    G2S_BG2Mosaic( TRUE );
  }
  
  //PPP暗号化
  PSTATUS_UTIL_SetCurrentPPPFast( work , FALSE );
  work->isWaitDisp = TRUE;

//  work->befPage = work->page;
}


//--------------------------------------------------------------
//表示更新待ち
//--------------------------------------------------------------
static void PSTATUS_WaitDisp( PSTATUS_WORK *work )
{
  //モザイクエフェクトの処理
  if( work->mosaicEffSeq == SMES_FADEOUT )
  {
    work->mosaicCnt += 1;
    G2_SetBGMosaicSize( work->mosaicCnt,work->mosaicCnt*2 );
    G2S_SetBGMosaicSize( work->mosaicCnt,work->mosaicCnt*2 );
    if( work->mosaicCnt >= 3 )
    {
      work->mosaicEffSeq = SMES_WAIT;
    }
  }
  if( work->mosaicEffSeq == SMES_FADEIN )
  {
    work->mosaicCnt -= 1;
    G2_SetBGMosaicSize( work->mosaicCnt,work->mosaicCnt*2 );
    G2S_SetBGMosaicSize( work->mosaicCnt,work->mosaicCnt*2 );
    if( work->mosaicCnt <= 0 )
    {
      work->isWaitDisp = FALSE;
      work->mosaicEffSeq = SMES_NONE;
      return;
    }
  }
  
  //表示切替タイミング
  if( PRINTSYS_QUE_IsFinished( work->printQue ) == TRUE &&
      ( work->mosaicEffSeq == SMES_WAIT || work->mosaicEffSeq == SMES_NONE ) )
  {
    if( work->befDataPos != work->dataPos )
    {
      if( work->befDataPos != 0xFF )
      {
        PSTATUS_SUB_ClearPage_Trans( work , work->subWork );
      }
      PSTATUS_SUB_DispPage_Trans( work , work->subWork );
      
      work->befDataPos = work->dataPos;
    }

    switch( work->befPage )
    {
    case PPT_INFO:
      PSTATUS_INFO_ClearPage_Trans( work , work->infoWork );
      break;
    case PPT_SKILL:
      PSTATUS_SKILL_ClearPage_Trans( work , work->skillWork );
      break;
    case PPT_RIBBON:
      PSTATUS_RIBBON_ClearPage_Trans( work , work->ribbonWork );
      break;
    case PPT_SKILL_ADD:
      PSTATUS_SKILL_ClearPage_Trans_WazaAdd( work , work->skillWork );
      break;
    }

    switch( work->page )
    {
    case PPT_INFO:
      PSTATUS_INFO_DispPage_Trans( work , work->infoWork );
      break;
    case PPT_SKILL:
      PSTATUS_SKILL_DispPage_Trans( work , work->skillWork );
      break;
    case PPT_RIBBON:
      PSTATUS_RIBBON_DispPage_Trans( work , work->ribbonWork );
      break;
    case PPT_SKILL_ADD:
      PSTATUS_SKILL_DispPage_Trans_WazaAdd( work , work->skillWork );
      break;
    }
    
    if( work->isEgg == FALSE &&
        work->psData->mode != PST_MODE_WAZAADD )
    {
      GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_PAGE2] , TRUE );
      if( work->isHaveRibbon == FALSE )
      {
        //リボン持ってなければ出さない
        GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_PAGE3] , FALSE );
      }
      else
      {
        GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_PAGE3] , TRUE );
      }
    }
    else
    {
      //技モードとタマゴの時は出さない
      GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_PAGE2] , FALSE );
      GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[SBT_PAGE3] , FALSE );
    }

    if( work->isAnimeBarCursor[0] == FALSE )
    {
      const BOOL isChange = PSTATUS_CanChangeData( work , FALSE , NULL );
      if( isChange == TRUE )
      {
        GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_UP] , APP_COMMON_BARICON_CURSOR_UP );
      }
      else
      {
        GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_UP] , APP_COMMON_BARICON_CURSOR_UP_OFF );
      }
    }
    if( work->isAnimeBarCursor[1] == FALSE )
    {
      const BOOL isChange = PSTATUS_CanChangeData( work , TRUE , NULL );
      if( isChange == TRUE )
      {
        GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_DOWN] , APP_COMMON_BARICON_CURSOR_DOWN );
      }
      else
      {
        GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CURSOR_DOWN] , APP_COMMON_BARICON_CURSOR_DOWN_OFF );
      }
    }
    
    if( work->page < PPT_SKILL_ADD &&
        PSTATUS_UTIL_CanUseExitButton(work) == TRUE )
    {
      if( work->shortCutCheck[work->page] == TRUE )
      {
        GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CHECK] , APP_COMMON_BARICON_CHECK_ON );
      }
      else
      {
        GFL_CLACT_WK_SetAnmSeq( work->clwkBarIcon[SBT_CHECK] , APP_COMMON_BARICON_CHECK_OFF );
      }
    }
    
    if( work->reqPlayVoice == TRUE )
    {
      work->reqPlayVoice = FALSE;
      PMVOICE_Stop( 0 );
      if( work->isEgg == FALSE )
      {
        POKEMON_PARAM *pp = PSTATUS_UTIL_GetCurrentPP(work);
        PMV_REF pmvRef;
        const u32 monsNo = PP_Get( pp , ID_PARA_monsno , NULL );
        const u32 formNo = PP_Get( pp , ID_PARA_form_no , NULL );
        PMV_MakeRefDataMine( &pmvRef );
        PMVOICE_Play( monsNo , formNo , 64 , FALSE , 0 , 0 , FALSE , (u32)&pmvRef );
      }
    }
    
    
    if( work->mosaicEffSeq == SMES_WAIT )
    {
      work->mosaicEffSeq = SMES_FADEIN;
      work->befPage = work->page;
    }
    else
    {
      work->isWaitDisp = FALSE;
      work->befPage = work->page;
    }
  }
}

//--------------------------------------------------------------
//現在のPPPを取得
//--------------------------------------------------------------
POKEMON_PASO_PARAM* PSTATUS_UTIL_GetCurrentPPP( PSTATUS_WORK *work )
{
  switch( work->psData->ppt )
  {
  case PST_PP_TYPE_POKEPARAM:
    {
      POKEMON_PARAM *pp = work->psData->ppd;
      return PP_GetPPPPointer( pp );
    }

    break;

  case PST_PP_TYPE_POKEPARTY:
    {
      POKEMON_PARAM *pp = PokeParty_GetMemberPointer( (POKEPARTY*)work->psData->ppd , work->dataPos );
      return PP_GetPPPPointer( pp );
    }
    break;

  case PST_PP_TYPE_POKEPASO:
    {
      u32 pppAdr = (u32)work->psData->ppd + POKETOOL_GetPPPWorkSize()*work->dataPos;
      return (POKEMON_PASO_PARAM*)pppAdr;
    }
    break;

#if PM_DEBUG
  case PST_PP_TYPE_DEBUG:
    PSTATUS_UTIL_DebugCreatePP( work );
    return PP_GetPPPPointer( work->calcPP );
    break;
#endif
  }
  return NULL;
}

//--------------------------------------------------------------
//現在のPPを取得
//--------------------------------------------------------------
POKEMON_PARAM* PSTATUS_UTIL_GetCurrentPP( PSTATUS_WORK *work )
{
  switch( work->psData->ppt )
  {
  case PST_PP_TYPE_POKEPARAM:
    return work->psData->ppd;
    break;

  case PST_PP_TYPE_POKEPARTY:
    {
      return PokeParty_GetMemberPointer( (POKEPARTY*)work->psData->ppd , work->dataPos );
    }
    break;

  case PST_PP_TYPE_POKEPASO:
    return work->calcPP;
    break;

#if PM_DEBUG
  case PST_PP_TYPE_DEBUG:
    PSTATUS_UTIL_DebugCreatePP( work );
    return work->calcPP;
    break;
#endif
  }
  return NULL;
}

//--------------------------------------------------------------
//PP・PPPの暗号・複合切り替え
//--------------------------------------------------------------
void PSTATUS_UTIL_SetCurrentPPPFast( PSTATUS_WORK *work , const BOOL isFast )
{
  switch( work->psData->ppt )
  {
  case PST_PP_TYPE_POKEPARAM:
    {
      POKEMON_PARAM *pp = work->psData->ppd;
      if( isFast == TRUE )
      {
        PP_FastModeOn( pp );
      }
      else
      {
        PP_FastModeOff( pp , TRUE );
      }
    }
    break;

  case PST_PP_TYPE_POKEPARTY:
    {
      POKEMON_PARAM *pp = PokeParty_GetMemberPointer( (POKEPARTY*)work->psData->ppd , work->dataPos );
      if( isFast == TRUE )
      {
        PP_FastModeOn( pp );
      }
      else
      {
        PP_FastModeOff( pp , TRUE );
      }
    }
    break;

  case PST_PP_TYPE_POKEPASO:
    {
      POKEMON_PASO_PARAM *ppp = (POKEMON_PASO_PARAM*)PSTATUS_UTIL_GetCurrentPPP( work );
      if( isFast == TRUE )
      {
        PP_FastModeOn( work->calcPP );
        PPP_FastModeOn( ppp );
      }
      else
      {
        PP_FastModeOff( work->calcPP , TRUE );
        PPP_FastModeOff( ppp , TRUE );
      }
    }

    break;

#if PM_DEBUG
  case PST_PP_TYPE_DEBUG:
    PSTATUS_UTIL_DebugCreatePP( work );

    if( isFast == TRUE )
    {
      PP_FastModeOn( work->calcPP );
    }
    else
    {
      PP_FastModeOff( work->calcPP , TRUE );
    }
    break;
#endif
  }
}


//--------------------------------------------------------------
//	文字の描画(bmp
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawStrFuncBmp( PSTATUS_WORK *work , GFL_BMP_DATA *bmpData , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  PRINTSYS_PrintQueColor( work->printQue , bmpData , 
          posX , posY , srcStr , work->fontHandle , col );
  GFL_STR_DeleteBuffer( srcStr );
}

//--------------------------------------------------------------
//	文字の描画
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawStrFunc( PSTATUS_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  PSTATUS_UTIL_DrawStrFuncBmp( work , GFL_BMPWIN_GetBmp( bmpWin ) , strId , posX , posY , col );
}

//--------------------------------------------------------------
//	文字の描画(右寄せ
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawStrFuncRight( PSTATUS_WORK *work , GFL_BMPWIN *bmpWin , 
                                      const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  u32 width;
  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  width = PRINTSYS_GetStrWidth( srcStr , work->fontHandle , 0 );
  PRINTSYS_PrintQueColor( work->printQue , GFL_BMPWIN_GetBmp( bmpWin ) , 
          posX-width , posY , srcStr , work->fontHandle , col );
  GFL_STR_DeleteBuffer( srcStr );
}

//--------------------------------------------------------------
//	文字の描画(値用 bmp
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawValueStrFuncBmp( PSTATUS_WORK *work , GFL_BMP_DATA *bmpData , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );

  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  WORDSET_ExpandStr( wordSet , dstStr , srcStr );
  PRINTSYS_PrintQueColor( work->printQue , bmpData , 
          posX , posY , dstStr , work->fontHandle , col );

  GFL_STR_DeleteBuffer( srcStr );
  GFL_STR_DeleteBuffer( dstStr );
}
//--------------------------------------------------------------
//	文字の描画(値用
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawValueStrFunc( PSTATUS_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  PSTATUS_UTIL_DrawValueStrFuncBmp( work , GFL_BMPWIN_GetBmp( bmpWin ) , wordSet ,
                                    strId , posX , posY , col );
}

//--------------------------------------------------------------
//	文字の描画(値用 bmp
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawValueStrFuncRightBmp( PSTATUS_WORK *work , GFL_BMP_DATA *bmpData ,
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  STRBUF *srcStr;
  STRBUF *dstStr = GFL_STR_CreateBuffer( 16, work->heapId );
  u32 width;

  srcStr = GFL_MSG_CreateString( work->msgHandle , strId ); 
  WORDSET_ExpandStr( wordSet , dstStr , srcStr );
  width = PRINTSYS_GetStrWidth( dstStr , work->fontHandle , 0 );
  PRINTSYS_PrintQueColor( work->printQue , bmpData , 
          posX-width , posY , dstStr , work->fontHandle , col );

  GFL_STR_DeleteBuffer( srcStr );
  GFL_STR_DeleteBuffer( dstStr );
}

//--------------------------------------------------------------
//	文字の描画(値用
//--------------------------------------------------------------
void PSTATUS_UTIL_DrawValueStrFuncRight( PSTATUS_WORK *work , GFL_BMPWIN *bmpWin , 
                                      WORDSET *wordSet, const u16 strId , const u16 posX , const u16 posY , const u16 col )
{
  PSTATUS_UTIL_DrawValueStrFuncRightBmp( work , GFL_BMPWIN_GetBmp( bmpWin ) , wordSet ,
                                         strId , posX , posY , col );
}

//--------------------------------------------------------------
//	X戻るボタンが使えるか？
//--------------------------------------------------------------
static const BOOL PSTATUS_UTIL_CanUseExitButton( PSTATUS_WORK *work )
{
  if( work->psData->canExitButton == TRUE )
  {
    return TRUE;
  }
  return FALSE;
}

//--------------------------------------------------------------
//	何かリボンを持っているか？
//--------------------------------------------------------------
static const BOOL PSTATUS_UTIL_CheckHaveAnyRibbon( PSTATUS_WORK *work , const POKEMON_PASO_PARAM *ppp )
{
  u8 i;
#if USE_STATUS_DEBUG
  if( work->isDevRibbon == TRUE )
  {
    return TRUE;
  }
#endif
  for( i=0;i<RIBBON_MAX;i++ )
  {
    const u32 checkId = RIBBON_DataGet( i , RIBBON_PARA_POKEPARA );
    const u32 isHave = PPP_Get( ppp , checkId , NULL );
    if( isHave == TRUE )
    {
      return TRUE;
    }
  }
  return FALSE;
}

#if PM_DEBUG
void PSTATUS_UTIL_DebugCreatePP( PSTATUS_WORK *work )
{
  u16 pos = work->dataPos+1;
  if( work->debMonsNo != 0 )
  {
    pos = work->debMonsNo;
  }
  
  
  if( work->calcPP != NULL &&
      PP_Get( work->calcPP , ID_PARA_monsno , NULL ) != pos )
  {
    GFL_HEAP_FreeMemory( work->calcPP );
    work->calcPP = NULL;
  }
  if( work->calcPP == NULL )
  {
    u16 oyaName[5] = {L'ブ',L'ラ',L'ッ',L'ク',0xFFFF};
    work->calcPP = PP_Create( pos , 50 , PTL_SETUP_ID_AUTO , HEAPID_POKE_STATUS );
    PP_Put( work->calcPP , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
    PP_Put( work->calcPP , ID_PARA_oyasex , PTL_SEX_MALE );
    PP_Put( work->calcPP , ID_PARA_get_ball , work->dataPos%24+1 );

    {
      const u32 lv = PP_Get( work->calcPP, ID_PARA_level, NULL );
      const u32 exp = PP_Get( work->calcPP , ID_PARA_exp , NULL );
      const u32 nextLvExp = POKETOOL_GetMinExp(PP_Get( work->calcPP, ID_PARA_monsno, NULL ),
                                      PP_Get( work->calcPP, ID_PARA_form_no, NULL ),
                                      lv+1) ;
      const u32 nowLvExp = POKETOOL_GetMinExp(PP_Get( work->calcPP, ID_PARA_monsno, NULL ),
                                      PP_Get( work->calcPP, ID_PARA_form_no, NULL ),
                                      lv) ;
      const u32 modLvExp = (nextLvExp-nowLvExp);
      
      const u32 calcExp = modLvExp/2;
      PP_Put( work->calcPP,ID_PARA_exp , nowLvExp+calcExp );
      
    }
    
    if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R )
    {
      PP_Put( work->calcPP , ID_PARA_tamago_flag , 1 );
    }
  }  
}
#endif



#pragma mark [>debug menu

#if USE_STATUS_DEBUG
#define PSTATUS_DEBUG_GROUP_NUMBER (60)
#define MEMO_DEBUG_GROUP_NUMBER (61)
#define MEMOINFO_DEBUG_GROUP_NUMBER (62)
#define SETTING_DEBUG_GROUP_NUMBER (63)

static fx32 PSTATUS_DEB_Update_Value( fx32 befValue );
static const BOOL PSTD_UpdateU32( u32 *num );
static const BOOL PSTD_UpdateU32_Max( u32 *num , const u32 max );

static void PSTD_U_year1( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_year1( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_month1( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_month1( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_day1( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_day1( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_place1( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_place1( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_year2( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_year2( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_month2( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_month2( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_day2( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_day2( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_place2( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_place2( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_level( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_level( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_natsuki( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_natsuki( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_egg( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_egg( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_parent( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_parent( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_event( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_event( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_memo( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_memo( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_getPPP( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_rom( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_rom( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_ribbon( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_ribbon( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_Nickname( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_Nickname( void* userWork , DEBUGWIN_ITEM* item );

static void PSTD_U_pokerusu( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_pokerusu( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_AddRibbon( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_AddRibbon( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_Monsno( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_Monsno( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_ScaleRateX( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_ScaleRateX( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_U_ScaleRateY( void* userWork , DEBUGWIN_ITEM* item );
static void PSTD_D_ScaleRateY( void* userWork , DEBUGWIN_ITEM* item );

static void PSTATUS_InitDebug( PSTATUS_WORK *work )
{
  VEC_Set( &work->shadowScale , PSTATUS_SUB_SHADOW_SCALE_X , PSTATUS_SUB_SHADOW_SCALE_Y , PSTATUS_SUB_SHADOW_SCALE_Z );
  work->shadowRotate = 0xD6C1;
  VEC_Set( &work->shadowOfs , PSTATUS_SUB_SHADOW_OFFSET_X , PSTATUS_SUB_SHADOW_OFFSET_Y , PSTATUS_SUB_SHADOW_OFFSET_Z );
  
  work->year1 = 0;
  work->month1 = 0;
  work->day1 = 0;
  work->place1 = 0;
  work->year2 = 0;
  work->month2 = 0;
  work->day2 = 0;
  work->place2 = 0;
  work->level = 0;
  work->devRom = 0;
  work->natsuki = 0;
  work->isDevEgg = FALSE;
  work->isDevParent = FALSE;
  work->isDevEvent = FALSE;
  work->isDevMemo = FALSE;
  work->isDevRibbon = FALSE;
  work->addRibbon = 0;
  work->debMonsNo = 0;
  work->scaleRateX = 0;
  work->scaleRateY = 0;
  
  DEBUGWIN_InitProc( PSTATUS_BG_SUB_STR , work->fontHandle );
  DEBUGWIN_ChangeLetterColor( 0,31,0 );
  
  DEBUGWIN_AddGroupToTop( PSTATUS_DEBUG_GROUP_NUMBER , "ステータス" , work->heapId );
  DEBUGWIN_AddGroupToGroup( MEMO_DEBUG_GROUP_NUMBER , "トレーナーメモ" , PSTATUS_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddGroupToGroup( SETTING_DEBUG_GROUP_NUMBER , "せってい" , PSTATUS_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddGroupToGroup( MEMOINFO_DEBUG_GROUP_NUMBER , "すうち" , MEMO_DEBUG_GROUP_NUMBER , work->heapId );

  DEBUGWIN_AddItemToGroupEx( PSTD_U_ribbon   ,PSTD_D_ribbon   , (void*)work , PSTATUS_DEBUG_GROUP_NUMBER , work->heapId );
  if( work->psData->ppt == PST_PP_TYPE_DEBUG )
  {
    DEBUGWIN_AddItemToGroupEx( PSTD_U_Monsno   ,PSTD_D_Monsno   , (void*)work , PSTATUS_DEBUG_GROUP_NUMBER , work->heapId );
  }
  DEBUGWIN_AddItemToGroupEx( PSTD_U_ScaleRateX   ,PSTD_D_ScaleRateX   , (void*)work , PSTATUS_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_ScaleRateY   ,PSTD_D_ScaleRateY   , (void*)work , PSTATUS_DEBUG_GROUP_NUMBER , work->heapId );

  DEBUGWIN_AddItemToGroupEx( PSTD_U_memo  ,PSTD_D_memo  , (void*)work , MEMO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_event ,PSTD_D_event , (void*)work , MEMO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_parent,PSTD_D_parent, (void*)work , MEMO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_egg   ,PSTD_D_egg   , (void*)work , MEMO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_rom   ,PSTD_D_rom   , (void*)work , MEMO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_Nickname   ,PSTD_D_Nickname   , (void*)work , MEMO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroup( "しゅとく" , PSTD_U_getPPP ,  (void*)work , MEMO_DEBUG_GROUP_NUMBER , work->heapId );

  DEBUGWIN_AddItemToGroupEx( PSTD_U_year1  ,PSTD_D_year1  , (void*)work , MEMOINFO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_month1 ,PSTD_D_month1 , (void*)work , MEMOINFO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_day1   ,PSTD_D_day1   , (void*)work , MEMOINFO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_place1 ,PSTD_D_place1 , (void*)work , MEMOINFO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_year2  ,PSTD_D_year2  , (void*)work , MEMOINFO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_month2 ,PSTD_D_month2 , (void*)work , MEMOINFO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_day2   ,PSTD_D_day2   , (void*)work , MEMOINFO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_place2 ,PSTD_D_place2 , (void*)work , MEMOINFO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_level  ,PSTD_D_level  , (void*)work , MEMOINFO_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_natsuki ,PSTD_D_natsuki , (void*)work , MEMOINFO_DEBUG_GROUP_NUMBER , work->heapId );

  //設定
  DEBUGWIN_AddItemToGroupEx( PSTD_U_pokerusu ,PSTD_D_pokerusu , (void*)work , SETTING_DEBUG_GROUP_NUMBER , work->heapId );
  DEBUGWIN_AddItemToGroupEx( PSTD_U_AddRibbon ,PSTD_D_AddRibbon , (void*)work , SETTING_DEBUG_GROUP_NUMBER , work->heapId );
}



static void PSTATUS_TermDebug( PSTATUS_WORK *work )
{
  DEBUGWIN_RemoveGroup( PSTATUS_DEBUG_GROUP_NUMBER );
  DEBUGWIN_ExitProc();
}

static fx32 PSTATUS_DEB_Update_Value( fx32 befValue )
{
  fx32 value;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X )
  {
    value = FX32_ONE;
  }
  else
  {
    value = FX32_CONST(0.1f);
  }

  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    return (befValue + value);
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    return (befValue - value);
  }
  return befValue;
  
}

static const BOOL PSTD_UpdateU32( u32 *num )
{
  return PSTD_UpdateU32_Max( num , 0xFFFFFFFF );
}

static const BOOL PSTD_UpdateU32_Max( u32 *num , const u32 max )
{
  u32 val = 1;
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_X ){val = 10;}
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_R ){val = 10000;}
  
  if( GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT )
  {
    if( *num + val > max ){*num = max;}
    else                  {*num += val;}
    return TRUE;
  }
  else if( GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT )
  {
    if( *num < val ){*num = 0;}
    else            {*num -= val;}
    return TRUE;
  }
  return FALSE;
}

static const BOOL PSTD_UpdateBool( BOOL *val )
{
  if( (GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT) ||
      (GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT) ||
      (GFL_UI_KEY_GetTrg() & PAD_BUTTON_A)  )
  {
    *val = (*val == TRUE ? FALSE : TRUE );
    return TRUE;
  }
  return FALSE;
}


static void PSTD_U_year1( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateU32( &work->year1 ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_year1( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "year1[%d]",work->year1 );  
}
static void PSTD_U_month1( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateU32( &work->month1 ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_month1( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "month1[%d]",work->month1 );  
}
static void PSTD_U_day1( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateU32( &work->day1 ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_day1( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "day1[%d]",work->day1 );  
}
static void PSTD_U_place1( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateU32( &work->place1 ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_place1( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "place1[%d]",work->place1 );  
}

static void PSTD_U_year2( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateU32( &work->year2 ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_year2( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "year2[%d]",work->year2 );  
}
static void PSTD_U_month2( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateU32( &work->month2 ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_month2( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "month2[%d]",work->month2 );  
}
static void PSTD_U_day2( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateU32( &work->day2 ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_day2( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "day2[%d]",work->day2 );  
}
static void PSTD_U_place2( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateU32( &work->place2 ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_place2( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "place2[%d]",work->place2 );  
}
static void PSTD_U_level( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateU32( &work->level ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void PSTD_D_level( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "lavel2[%d]",work->level );  
}

static void PSTD_U_natsuki( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateU32( &work->natsuki ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_natsuki( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "natsuki[%d]",work->natsuki );  
}

static void PSTD_U_egg( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateBool( &work->isDevEgg ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_egg( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "Egg[%s]",(work->isDevEgg?"ON":"OFF") );  
}

static void PSTD_U_parent( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateBool( &work->isDevParent ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_parent( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "Parent[%s]",(work->isDevParent?"ON":"OFF") );  
}

static void PSTD_U_event( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateBool( &work->isDevEvent ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_event( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "Event[%s]",(work->isDevEvent?"ON":"OFF") );  
}

static void PSTD_U_memo( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateBool( &work->isDevMemo ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_memo( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "Memo[%s]",(work->isDevMemo?"ON":"OFF") );  
}
static void PSTD_U_rom( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateU32( &work->devRom ) == TRUE )
  {
    if( work->devRom >= 21 )
    {
      work->devRom = 21;
    }
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_rom( void* userWork , DEBUGWIN_ITEM* item )
{
  static const char romName[22][3] = 
  {
    "--","Sa","Ru","Em",
    "FR","LG","--",
    "HG","SS","--",
    "D","P","Pl","--","--",
    "XS","--","--","--","--",
    "W","B"
  };
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "Rom[%s]",romName[work->devRom] );  
}


static void PSTD_U_getPPP( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );
    MYSTATUS *myStatus = GAMEDATA_GetMyStatus( work->psData->game_data );

    work->natsuki = PPP_Get( ppp , ID_PARA_friend , NULL );
    work->year1   = PPP_Get( ppp , ID_PARA_get_year , NULL );
    work->month1  = PPP_Get( ppp , ID_PARA_get_month , NULL );
    work->day1    = PPP_Get( ppp , ID_PARA_get_day , NULL );
    work->place1  = PPP_Get( ppp , ID_PARA_get_place , NULL );
    work->year2   = PPP_Get( ppp , ID_PARA_birth_year , NULL );
    work->month2  = PPP_Get( ppp , ID_PARA_birth_month , NULL );
    work->day2    = PPP_Get( ppp , ID_PARA_birth_day , NULL );
    work->place2  = PPP_Get( ppp , ID_PARA_birth_place , NULL );
    work->level   = PPP_Get( ppp , ID_PARA_get_level , NULL );
    work->devRom = PPP_Get( ppp , ID_PARA_get_cassette , NULL );
    work->isDevEgg   = PPP_Get( ppp , ID_PARA_tamago_flag , NULL );
    work->isDevParent= PPP_IsMatchOya( ppp , myStatus );
    work->isDevEvent = PPP_Get( ppp , ID_PARA_event_get_flag , NULL );
  }
}


static void PSTD_U_ribbon( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  if( PSTD_UpdateBool( &work->isDevRibbon ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_ribbon( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "RibbonAll[%s]",(work->isDevRibbon?"ON":"OFF") );  
}

static void PSTD_U_Nickname( void* userWork , DEBUGWIN_ITEM* item )
{
}
static void PSTD_D_Nickname( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  const POKEMON_PASO_PARAM *ppp = PSTATUS_UTIL_GetCurrentPPP( work );
  const BOOL isNickName = PPP_Get( ppp , ID_PARA_nickname_flag , NULL );
  DEBUGWIN_ITEM_SetNameV( item , "NickName[%s]",(isNickName?"ON":"OFF") );  

}

#pragma mark [>setting

static void PSTD_U_pokerusu( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  POKEMON_PARAM *pp = PSTATUS_UTIL_GetCurrentPP( work );
	u32 pokerus = PP_Get( pp, ID_PARA_pokerus, NULL );
  
  if( PSTD_UpdateU32_Max( &pokerus , 0xFF ) == TRUE )
  {
    PP_Put( pp, ID_PARA_pokerus, pokerus );
    DEBUGWIN_RefreshScreen();
  }
}
static void PSTD_D_pokerusu( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  POKEMON_PARAM *pp = PSTATUS_UTIL_GetCurrentPP( work );
	u32 pokerus = PP_Get( pp, ID_PARA_pokerus, NULL );
  DEBUGWIN_ITEM_SetNameV( item , "Pokerusu[%d]",pokerus );  
}

static void PSTD_U_AddRibbon( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  
  if( PSTD_UpdateU32_Max( &work->addRibbon , 80 ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
  {
    POKEMON_PARAM *pp = PSTATUS_UTIL_GetCurrentPP( work );
    u32 ppId = RIBBON_DataGet( work->addRibbon , RIBBON_PARA_POKEPARA );
    u32 isHave = PP_Get( pp , ppId , NULL );
    PP_Put( pp , ppId , !isHave );
    DEBUGWIN_RefreshScreen();
  }
}

static void PSTD_D_AddRibbon( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  POKEMON_PARAM *pp = PSTATUS_UTIL_GetCurrentPP( work );
  u32 ppId = RIBBON_DataGet( work->addRibbon , RIBBON_PARA_POKEPARA );
  u32 isHave = PP_Get( pp , ppId , NULL );
  
  DEBUGWIN_ITEM_SetNameV( item , "AddRibbon[%d][%s]",work->addRibbon,(isHave?"o":"x") );  
  
}

static void PSTD_U_Monsno( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  
  if( PSTD_UpdateU32_Max( &work->debMonsNo , MONSNO_GENOSEKUTO ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void PSTD_D_Monsno( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "MonsNo[%d]",work->debMonsNo );  
  
}
static void PSTD_U_ScaleRateX( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  
  if( PSTD_UpdateU32_Max( &work->scaleRateX , 200 ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void PSTD_D_ScaleRateX( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "ScaleRateX[%d]",work->scaleRateX );  
  
}
static void PSTD_U_ScaleRateY( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  
  if( PSTD_UpdateU32_Max( &work->scaleRateY , 200 ) == TRUE )
  {
    DEBUGWIN_RefreshScreen();
  }
}

static void PSTD_D_ScaleRateY( void* userWork , DEBUGWIN_ITEM* item )
{
  PSTATUS_WORK *work = (PSTATUS_WORK*)userWork;
  DEBUGWIN_ITEM_SetNameV( item , "ScaleRateY[%d]",work->scaleRateY );  
  
}
#endif //USE_STATUS_DEBUG
