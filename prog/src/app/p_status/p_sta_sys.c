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
#include "poke_tool/poke_tool.h"

#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"

#include "p_sta_sys.h"
#include "p_sta_sub.h"
#include "p_sta_info.h"
#include "p_sta_ribbon.h"

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
  GX_OBJVRAMMODE_CHAR_1D_128K
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

static void PSTATUS_RefreshDisp( PSTATUS_WORK *work );
static void PSTATUS_UTIL_SetCurrentPPPFast( PSTATUS_WORK *work , const BOOL isFast );


//--------------------------------------------------------------
//	初期化
//--------------------------------------------------------------
const BOOL PSTATUS_InitPokeStatus( PSTATUS_WORK *work )
{
  work->dataPos = work->psData->pos;
  work->befDataPos = work->dataPos;
  work->scrollCnt = 0;
  work->befVCount = OS_GetVBlankCount();
  work->page = PPT_INFO;
  work->befPage = PPT_INFO;
  work->isActiveBarButton = TRUE;

#if PM_DEBUG
  work->debPp = NULL;
#endif

  PSTATUS_InitGraphic( work );

  work->subWork = PSTATUS_SUB_Init( work );
  work->infoWork = PSTATUS_INFO_Init( work );
  work->ribbonWork = PSTATUS_RIBBON_Init( work );

  PSTATUS_LoadResource( work );
  PSTATUS_InitMessage( work );

  PSTATUS_InitCell( work );

  work->vBlankTcb = GFUser_VIntr_CreateTCB( PSTATUS_VBlankFunc , work , 8 );

  //フェードないので仮処理
  GX_SetMasterBrightness(0);  
  GXS_SetMasterBrightness(0);
  
  PSTATUS_SUB_DispPage( work , work->subWork );
  PSTATUS_INFO_DispPage( work , work->infoWork );
  PSTATUS_RIBBON_CreateRibbonBar( work , work->ribbonWork );
  
  return TRUE;
}

//--------------------------------------------------------------
//	開放
//--------------------------------------------------------------
const BOOL PSTATUS_TermPokeStatus( PSTATUS_WORK *work )
{
  GFL_TCB_DeleteTask( work->vBlankTcb );

  PSTATUS_TermCell( work );

  PSTATUS_TermMessage( work );
  PSTATUS_ReleaseResource( work );

  PSTATUS_RIBBON_DeleteRibbonBar( work , work->ribbonWork );
  PSTATUS_RIBBON_Term( work , work->ribbonWork );
  PSTATUS_INFO_Term( work , work->infoWork );
  PSTATUS_SUB_Term( work , work->subWork );

  PSTATUS_TermGraphic( work );

#if PM_DEBUG
  if( work->debPp != NULL )
  {
    GFL_HEAP_FreeMemory( work->debPp );
  }
#endif
  return TRUE;
}

//--------------------------------------------------------------
//	更新
//--------------------------------------------------------------
const BOOL PSTATUS_UpdatePokeStatus( PSTATUS_WORK *work )
{
  PSTATUS_UpdateBarButton( work );
  
  PSTATUS_SUB_Main( work , work->subWork );
  //FIXME 仮キー処理
  if( work->isActiveBarButton == TRUE )
  {
    if( GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN &&
        !(GFL_UI_KEY_GetCont() & PAD_BUTTON_R))
    {
      if( work->dataPos < work->psData->max-1 )
      {
        work->dataPos++;
        PSTATUS_RefreshDisp( work );
      }
    }
    else
    if( GFL_UI_KEY_GetRepeat() & PAD_KEY_UP &&
        !(GFL_UI_KEY_GetCont() & PAD_BUTTON_R))
    {
      if( work->dataPos != 0 )
      {
        work->dataPos--;
        PSTATUS_RefreshDisp( work );
      }
    }
    else
    if( (GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT) ||
        (GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT) )
    {
      if( work->page == PPT_INFO )
      {
        work->page = PPT_RIBBON;
      }
      else
      {
        work->page = PPT_INFO;
      }
      PSTATUS_RefreshDisp( work );
    }
    else
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
    {
      return TRUE;
    }
  }

  switch( work->befPage )
  {
  case PPT_INFO:
    PSTATUS_INFO_Main( work , work->infoWork );
    break;
  case PPT_SKILL:
    break;
  case PPT_RIBBON:
    PSTATUS_RIBBON_Main( work , work->ribbonWork );
    break;
  }


  //メッセージ
  PRINTSYS_QUE_Main( work->printQue );
  
  //OBJの更新
  GFL_CLACT_SYS_Main();

  //3D描画  
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    PSTATUS_SUB_Draw( work , work->subWork );
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

  return FALSE;
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
      GX_BG_EXTPLTT_23, 1, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG1 SUB (Info
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x800, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x6800, GX_BG_CHARBASE_0x08000,0x08000,
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
    
    GFL_BG_SetBGControl3D( 0 );
    GFL_BG_SetVisible( PSTATUS_BG_3D , TRUE );
    
    PSTATUS_SetupBgFunc( &header_sub0 , PSTATUS_BG_SUB_STR  , GFL_BG_MODE_TEXT );
    PSTATUS_SetupBgFunc( &header_sub1 , PSTATUS_BG_SUB_INFO , GFL_BG_MODE_TEXT );
    PSTATUS_SetupBgFunc( &header_sub2 , PSTATUS_BG_SUB_PLATE , GFL_BG_MODE_TEXT );
    PSTATUS_SetupBgFunc( &header_sub3 , PSTATUS_BG_SUB_BG , GFL_BG_MODE_TEXT );
  }
  
  //WindowMask設定
  {
    G2_SetWnd0Position( 0 , 0 , PSTATUS_MAIN_PAGE_WIDTH*8 , 192 );
    G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ , TRUE );
    G2_SetWndOutsidePlane( GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 | GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ , FALSE );
    GX_SetVisibleWnd( GX_WNDMASK_W0 );

    G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2 , GX_BLEND_PLANEMASK_BG3 , 13 , 16 );
    G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG2 , GX_BLEND_PLANEMASK_BG3 , 13 , 16 );
  }
  
  //OBJ系の初期化
  {
    GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
    cellSysInitData.CGR_RegisterMax = 110;
    GFL_CLACT_SYS_Create( &cellSysInitData , &vramBank ,work->heapId );

    //TODO 個数は適当
    work->cellUnit  = GFL_CLACT_UNIT_Create( 110 , 0, work->heapId );
    GFL_CLACT_UNIT_SetDefaultRend( work->cellUnit );
    
    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
  }
  
  //3D系の初期化
  { //3D系の設定
    static const VecFx32 cam_pos = {FX32_CONST(0.0f),FX32_CONST(0.0f),FX32_CONST(101.0f)};
    static const VecFx32 cam_target = {FX32_CONST(0.0f),FX32_CONST(0.0f),FX32_CONST(-100.0f)};
    static const VecFx32 cam_up = {0,FX32_ONE,0};
    //エッジマーキングカラー
    static  const GXRgb edge_color_table[8]=
      { GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 0, 0, 0, 0, 0, 0 };
    GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT16K,
            0, work->heapId, NULL );
 
    //正射影カメラ
    work->camera =  GFL_G3D_CAMERA_Create( GFL_G3D_PRJORTH, 
                       FX32_ONE*12.0f,
                       0,
                       0,
                       FX32_ONE*16.0f,
                       (FX32_ONE),
                       (FX32_ONE*200),
                       NULL,
                       &cam_pos,
                       &cam_up,
                       &cam_target,
                       work->heapId );
    
    GFL_G3D_CAMERA_Switching( work->camera );
    //エッジマーキングカラーセット
    G3X_SetEdgeColorTable( edge_color_table );
    G3X_EdgeMarking( TRUE );
    
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO , GX_BUFFERMODE_Z );
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
  
  GFL_G3D_CAMERA_Delete( work->camera );
  GFL_G3D_Exit();

  GFL_CLACT_UNIT_Delete( work->cellUnit );
  GFL_CLACT_SYS_Delete();

  GFL_BG_FreeBGControl( PSTATUS_BG_SUB_INFO );
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
  //BARパレット
  GFL_ARCHDL_UTIL_TransVramPalette( archandle , NARC_p_status_gra_menu_bar_NCLR , 
                    PALTYPE_MAIN_BG , PSTATUS_BG_PLT_BAR*16*2 , 16*2 , work->heapId );
  
  //下画面共通キャラ
  GFL_ARCHDL_UTIL_TransVramBgCharacter( archandle , NARC_p_status_gra_p_st_bg_d_NCGR ,
                    PSTATUS_BG_PLATE , 0 , 0, FALSE , work->heapId );
  //下画面背景
  GFL_ARCHDL_UTIL_TransVramScreen( archandle , NARC_p_status_gra_p_st_scroll_d_NSCR , 
                    PSTATUS_BG_MAIN_BG , 0 , 0, FALSE , work->heapId );

  //下画面バー
  GFL_ARCHDL_UTIL_TransVramBgCharacter( archandle , NARC_p_status_gra_menu_bar_NCGR ,
                    PSTATUS_BG_PARAM , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( archandle , NARC_p_status_gra_menu_bar_NSCR , 
                    PSTATUS_BG_PARAM , 0 , 0, FALSE , work->heapId );
  GFL_BG_ChangeScreenPalette( PSTATUS_BG_PARAM , 0 , 21 , 32 , 3 , PSTATUS_BG_PLT_BAR );


  //上画面共通パレット
  GFL_ARCHDL_UTIL_TransVramPalette( archandle , NARC_p_status_gra_p_st_bg_u_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  //上画面共通キャラ
  GFL_ARCHDL_UTIL_TransVramBgCharacter( archandle , NARC_p_status_gra_p_st_bg_u_NCGR ,
                    PSTATUS_BG_SUB_PLATE , 0 , 0, FALSE , work->heapId );

  //上画面背景
  GFL_ARCHDL_UTIL_TransVramScreen( archandle , NARC_p_status_gra_p_st_scroll_u_NSCR , 
                    PSTATUS_BG_SUB_BG ,  0 , 0, FALSE , work->heapId );
  //上画面info
  GFL_ARCHDL_UTIL_TransVramScreen( archandle , NARC_p_status_gra_p_st_infotitle_u_NSCR , 
                    PSTATUS_BG_SUB_INFO ,  0 , 0, FALSE , work->heapId );


  //OBJリソース
  //パレット
  work->cellRes[SCR_PLT_ICON] = GFL_CLGRP_PLTT_Register( archandle , 
        NARC_p_status_gra_p_st_obj_d_NCLR , CLSYS_DRAW_MAIN , 
        PSTATUS_OBJPLT_ICON*32 , work->heapId  );
  work->cellRes[SCR_PLT_RIBBON_BAR] = GFL_CLGRP_PLTT_Register( archandle , 
        NARC_p_status_gra_p_status_ribbon_bar_NCLR , CLSYS_DRAW_MAIN , 
        PSTATUS_OBJPLT_RIBBON_BAR*32 , work->heapId  );
  work->cellRes[SCR_PLT_RIBBON_CUR] = GFL_CLGRP_PLTT_Register( archandle , 
        NARC_p_status_gra_p_st_ribbon_cur_NCLR , CLSYS_DRAW_MAIN , 
        PSTATUS_OBJPLT_RIBBON_CUR*32 , work->heapId  );
        
  //キャラクタ
  work->cellRes[SCR_NCG_ICON] = GFL_CLGRP_CGR_Register( archandle , 
        NARC_p_status_gra_p_st_obj_d_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  work->cellRes[SCR_NCG_RIBBON_CUR] = GFL_CLGRP_CGR_Register( archandle , 
        NARC_p_status_gra_p_st_ribbon_cur_NCGR , FALSE , CLSYS_DRAW_MAIN , work->heapId  );
  
  //セル・アニメ
  work->cellRes[SCR_ANM_ICON] = GFL_CLGRP_CELLANIM_Register( archandle , 
        NARC_p_status_gra_p_st_obj_d_NCER , NARC_p_status_gra_p_st_obj_d_NANR, work->heapId  );
  work->cellRes[SCR_ANM_RIBBON_CUR] = GFL_CLGRP_CELLANIM_Register( archandle , 
        NARC_p_status_gra_p_st_ribbon_cur_NCER , NARC_p_status_gra_p_st_ribbon_cur_NANR, work->heapId  );



  PSTATUS_SUB_LoadResource( work , work->subWork , archandle );
  PSTATUS_INFO_LoadResource( work , work->infoWork , archandle );
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
      SBA_PAGE1_SELECT,
      SBA_PAGE2_NORMAL,
      SBA_PAGE3_NORMAL,
      SBA_CHECK,
      SBA_BUTTON_UP,
      SBA_BUTTON_DOWN,
      SBA_BUTTON_EXIT,
      SBA_BUTTON_RETURN,
    };
    const u8 posXArr[SBT_MAX] =
    {
      PSTATUS_BAR_CELL_PAGE1_X,
      PSTATUS_BAR_CELL_PAGE2_X,
      PSTATUS_BAR_CELL_PAGE3_X,
      PSTATUS_BAR_CELL_CHECK_X,
      PSTATUS_BAR_CELL_CURSOR_UP_X,
      PSTATUS_BAR_CELL_CURSOR_DOWN_X,
      PSTATUS_BAR_CELL_CURSOR_EXIT,
      PSTATUS_BAR_CELL_CURSOR_RETURN,
    };
    u8 i;
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_y = PSTATUS_BAR_CELL_Y;
    cellInitData.softpri = 10;
    cellInitData.bgpri = 1;
    
    for( i=0;i<SBT_MAX;i++ )
    {
      cellInitData.pos_x = posXArr[i];
      cellInitData.anmseq = anmIdxArr[i];
      work->clwkBarIcon[i] = GFL_CLACT_WK_Create( work->cellUnit ,
                work->cellRes[SCR_NCG_ICON],
                work->cellRes[SCR_PLT_ICON],
                work->cellRes[SCR_ANM_ICON],
                &cellInitData ,CLSYS_DEFREND_MAIN , work->heapId );

      GFL_CLACT_WK_SetDrawEnable( work->clwkBarIcon[i] , TRUE );
    }
  }
  
  PSTATUS_RIBBON_InitCell( work , work->ribbonWork );
}

//--------------------------------------------------------------------------
//  セル開放
//--------------------------------------------------------------------------
static void PSTATUS_TermCell( PSTATUS_WORK *work )
{
  u8 i;
  PSTATUS_RIBBON_TermCell( work , work->ribbonWork );

  for( i=0;i<SBT_MAX;i++ )
  {
    GFL_CLACT_WK_Remove( work->clwkBarIcon[i] );
  }
}

//--------------------------------------------------------------------------
//  メッセージ系初期化
//--------------------------------------------------------------------------
static void PSTATUS_InitMessage( PSTATUS_WORK *work )
{
  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  
  //メッセージ
  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_pokestatus_dat , work->heapId );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , PSTATUS_BG_PLT_FONT*16*2, 16*2, work->heapId );
  
  work->printQue = PRINTSYS_QUE_CreateEx( 4096 , work->heapId );
}

//--------------------------------------------------------------------------
//  メッセージ系開放
//--------------------------------------------------------------------------
static void PSTATUS_TermMessage( PSTATUS_WORK *work )
{
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

//外部からの操作関数
#pragma mark [>Outer
//--------------------------------------------------------------------------
//  バー操作(キーのページ切り替えも)の有効設定
//--------------------------------------------------------------------------
void PSTATUS_SetActiveBarButton( PSTATUS_WORK *work , const BOOL isActive )
{
  work->isActiveBarButton = isActive;
}

#pragma mark [>util

//--------------------------------------------------------------
//キャラが変わったので表示更新
//--------------------------------------------------------------
static void PSTATUS_RefreshDisp( PSTATUS_WORK *work )
{
  //PPP暗号解除
  PSTATUS_UTIL_SetCurrentPPPFast( work , TRUE );
  
  if( work->befDataPos != work->dataPos )
  {
    PSTATUS_SUB_ClearPage( work , work->subWork );
    PSTATUS_SUB_DispPage( work , work->subWork );
    
    PSTATUS_RIBBON_DeleteRibbonBar( work , work->ribbonWork );
    PSTATUS_RIBBON_CreateRibbonBar( work , work->ribbonWork );

    work->befDataPos = work->dataPos;
  }

  switch( work->befPage )
  {
  case PPT_INFO:
    PSTATUS_INFO_ClearPage( work , work->infoWork );
    break;
  case PPT_SKILL:
    break;
  case PPT_RIBBON:
    PSTATUS_RIBBON_ClearPage( work , work->ribbonWork );
    break;
  }

  switch( work->page )
  {
  case PPT_INFO:
    PSTATUS_INFO_DispPage( work , work->infoWork );
    break;
  case PPT_SKILL:
    break;
  case PPT_RIBBON:
    PSTATUS_RIBBON_DispPage( work , work->ribbonWork );
    break;
  }
  
  //PPP暗号化
  PSTATUS_UTIL_SetCurrentPPPFast( work , FALSE );
  work->befPage = work->page;
}

//--------------------------------------------------------------
//現在のPPPを取得
//--------------------------------------------------------------
const POKEMON_PASO_PARAM* PSTATUS_UTIL_GetCurrentPPP( PSTATUS_WORK *work )
{
  switch( work->psData->ppt )
  {
  case PST_PP_TYPE_POKEPARAM:

    break;

  case PST_PP_TYPE_POKEPARTY:
    {
      const POKEMON_PARAM *pp = PokeParty_GetMemberPointer( (POKEPARTY*)work->psData->ppd , work->dataPos );
      return PP_GetPPPPointerConst( pp );
    }
    break;

  case PST_PP_TYPE_POKEPASO:

    break;

#if PM_DEBUG
  case PST_PP_TYPE_DEBUG:
    if( work->debPp != NULL &&
        PP_Get( work->debPp , ID_PARA_monsno , NULL ) != work->dataPos+1 )
    {
      GFL_HEAP_FreeMemory( work->debPp );
      work->debPp = NULL;
    }
    if( work->debPp == NULL )
    {
      u16 oyaName[5] = {L'ブ',L'ラ',L'ッ',L'ク',0xFFFF};
      work->debPp = PP_Create( work->dataPos+1 , 50 , PTL_SETUP_POW_AUTO , HEAPID_POKE_STATUS );
      PP_Put( work->debPp , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
      PP_Put( work->debPp , ID_PARA_oyasex , PTL_SEX_MALE );
    }
    return PP_GetPPPPointerConst( work->debPp );
    break;
#endif
  }
  return NULL;
}

//--------------------------------------------------------------
//PP・PPPの暗号・複合切り替え
//--------------------------------------------------------------
static void PSTATUS_UTIL_SetCurrentPPPFast( PSTATUS_WORK *work , const BOOL isFast )
{
  switch( work->psData->ppt )
  {
  case PST_PP_TYPE_POKEPARAM:

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

    break;

#if PM_DEBUG
  case PST_PP_TYPE_DEBUG:
    if( work->debPp != NULL &&
        PP_Get( work->debPp , ID_PARA_monsno , NULL ) != work->dataPos+1 )
    {
      GFL_HEAP_FreeMemory( work->debPp );
      work->debPp = NULL;
    }
    if( work->debPp == NULL )
    {
      u16 oyaName[5] = {L'ブ',L'ラ',L'ッ',L'ク',0xFFFF};
      work->debPp = PP_Create( work->dataPos+1 , 50 , PTL_SETUP_POW_AUTO , HEAPID_POKE_STATUS );
      PP_Put( work->debPp , ID_PARA_oyaname_raw , (u32)&oyaName[0] );
      PP_Put( work->debPp , ID_PARA_oyasex , PTL_SEX_MALE );
    }

    if( isFast == TRUE )
    {
      PP_FastModeOn( work->debPp );
    }
    else
    {
      PP_FastModeOff( work->debPp , TRUE );
    }
    break;
#endif
  }
}
