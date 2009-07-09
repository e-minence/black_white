//======================================================================
/**
 * @file  dressup_system.h
 * @brief ステージ 演技メイン
 * @author  ariizumi
 * @data  09/03/02
 */
//======================================================================
#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/wipe.h"
#include "font/font.naix"

#include "arc_def.h"
#include "musical_item.naix"
#include "musical_script.naix"
#include "stage_gra.naix"
#include "message.naix"
#include "print/printsys.h"

#include "sound/snd_strm.h"
#include "snd_strm.naix"

#include "infowin/infowin.h"
#include "test/ariizumi/ari_debug.h"
#include "musical/mus_poke_draw.h"
#include "musical/mus_item_draw.h"
#include "musical/musical_camera_def.h"

#include "musical/musical_stage_sys.h"
#include "sta_local_def.h"
#include "script/sta_act_script_def.h"
#include "sta_act_bg.h"
#include "sta_act_audience.h"

#include "eff_def/mus_eff.h"

#include "debug/debugwin_sys.h"

//======================================================================
//  define
//======================================================================
#pragma mark [> define

#define ACT_FRAME_MAIN_3D   GFL_BG_FRAME0_M
#define ACT_FRAME_MAIN_FONT   GFL_BG_FRAME1_M
#define ACT_FRAME_MAIN_CURTAIN  GFL_BG_FRAME2_M
#define ACT_FRAME_MAIN_BG   GFL_BG_FRAME3_M

#define ACT_FRAME_SUB_BG    GFL_BG_FRAME1_S
#define ACT_FRAME_SUB_INFO    GFL_BG_FRAME3_S

#define ACT_PAL_INFO    (0xE)
#define ACT_PAL_FONT    (0xF)

#define ACT_OBJECT_IVALID (-1)

#define ACT_BG_SCROLL_MAX (256)

//Msg系
#define ACT_MSG_POS_X ( 4 )
#define ACT_MSG_POS_Y ( 20 )
#define ACT_MSG_POS_WIDTH  ( 24 )
#define ACT_MSG_POS_HEIGHT ( 4 )

//スクリプト用
enum
{
  STA_SCRIPT_MAIN = 0,
  //１は未使用
  STA_SCRIPT_POKE_1 = 2,
  STA_SCRIPT_POKE_2 = 3,
  STA_SCRIPT_POKE_3 = 4,
  STA_SCRIPT_POKE_4 = 5,
};


//======================================================================
//  enum
//======================================================================
#pragma mark [> enum

//======================================================================
//  typedef struct
//======================================================================
#pragma mark [> struct
//スポットライト

struct _ACTING_WORK
{
  HEAPID heapId;
  
  BOOL  updateScroll;
  u16   scrollOffset;
  u16   makuOffset;
  
  GFL_TCB   *vblankFuncTcb;

  STAGE_INIT_WORK *initWork;

  MUS_POKE_DRAW_SYSTEM  *drawSys;
  MUS_ITEM_DRAW_SYSTEM  *itemDrawSys;
  
  STA_BG_SYS      *bgSys;

  STA_POKE_SYS    *pokeSys;
  STA_POKE_WORK   *pokeWork[MUSICAL_POKE_MAX];
  
  STA_OBJ_SYS     *objSys;
  STA_OBJ_WORK    *objWork[ACT_OBJECT_MAX];

  STA_EFF_SYS     *effSys;
  STA_EFF_WORK    *effWork[ACT_EFFECT_MAX];
  
  STA_LIGHT_SYS   *lightSys;
  STA_LIGHT_WORK    *lightWork[ACT_LIGHT_MAX];

  GFL_G3D_CAMERA    *camera;
  GFL_BBD_SYS     *bbdSys;
  
  STA_SCRIPT_SYS    *scriptSys;
  void        *scriptData;
  
  STA_AUDI_SYS  *audiSys;
  
  //メッセージ用
  GFL_TCBLSYS     *tcblSys;
  GFL_BMPWIN      *msgWin;
  GFL_FONT      *fontHandle;
  PRINT_STREAM    *printHandle;
  GFL_MSGDATA     *msgHandle;
};

//======================================================================
//  proto
//======================================================================
#pragma mark [> proto
ACTING_WORK*  STA_ACT_InitActing( STAGE_INIT_WORK *initWork , HEAPID heapId);
void  STA_ACT_TermActing( ACTING_WORK *work );
ACTING_RETURN STA_ACT_LoopActing( ACTING_WORK *work );
static void STA_ACT_VBlankFunc(GFL_TCB *tcb,void *work);

static void STA_ACT_SetupGraphic( ACTING_WORK *work );
static void STA_ACT_SetupBg( ACTING_WORK *work );
static void STA_ACT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane  , u8 mode );
static void STA_ACT_SetupPokemon( ACTING_WORK *work );
static void STA_ACT_SetupItem( ACTING_WORK *work );
static void STA_ACT_SetupEffect( ACTING_WORK *work );
static void STA_ACT_SetupMessage( ACTING_WORK *work );

static void STA_ACT_UpdateScroll( ACTING_WORK *work );
static void STA_ACT_UpdateMessage( ACTING_WORK *work );

static void STA_ACT_StartScript( ACTING_WORK *work );



static const GFL_DISP_VRAM vramBank = {
  GX_VRAM_BG_128_C,       // メイン2DエンジンのBG
  GX_VRAM_BGEXTPLTT_23_G,     // メイン2DエンジンのBG拡張パレット
  GX_VRAM_SUB_BG_48_HI,     // サブ2DエンジンのBG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // サブ2DエンジンのBG拡張パレット
  GX_VRAM_OBJ_16_F,       // メイン2DエンジンのOBJ
  GX_VRAM_OBJEXTPLTT_NONE,    // メイン2DエンジンのOBJ拡張パレット
  GX_VRAM_SUB_OBJ_128_D,     // サブ2DエンジンのOBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // サブ2DエンジンのOBJ拡張パレット
  GX_VRAM_TEX_01_AB,        // テクスチャイメージスロット
  GX_VRAM_TEXPLTT_0123_E,     // テクスチャパレットスロット
  GX_OBJVRAMMODE_CHAR_1D_32K,
  GX_OBJVRAMMODE_CHAR_1D_128K
};
//  A テクスチャ
//  B テクスチャ
//  C MainBg
//  D SubObj
//  E テクスチャパレット
//  F MainObj
//  G MainBgExPlt
//  H SubBG
//  I SubBG

//--------------------------------------------------------------
//  
//--------------------------------------------------------------
ACTING_WORK*  STA_ACT_InitActing( STAGE_INIT_WORK *initWork , HEAPID heapId )
{
  ACTING_WORK *work = GFL_HEAP_AllocMemory( heapId , sizeof( ACTING_WORK ));
  
  work->heapId = heapId;
  work->initWork = initWork;
  work->updateScroll = FALSE;
  work->scrollOffset = 0;
  work->makuOffset = 0;
  work->scriptData = NULL;

  work->vblankFuncTcb = GFUser_VIntr_CreateTCB( STA_ACT_VBlankFunc , (void*)work , 64 );

  STA_ACT_SetupGraphic( work );
  STA_ACT_SetupBg( work );
  STA_ACT_SetupMessage( work);
  STA_ACT_SetupPokemon( work );
  STA_ACT_SetupItem( work );
  STA_ACT_SetupEffect( work );
  
  work->lightSys = STA_LIGHT_InitSystem(work->heapId , work );
  work->audiSys = STA_AUDI_InitSystem( work->heapId , work );

  work->scriptSys = STA_SCRIPT_InitSystem( work->heapId , work );
  
  INFOWIN_Init( ACT_FRAME_SUB_INFO,ACT_PAL_INFO,NULL,work->heapId);

  //フェードないので仮処理
  GX_SetMasterBrightness(0);  
  GXS_SetMasterBrightness(0);
  
#if USE_DEBUGWIN_SYSTEM
  DEBUGWIN_InitProc( ACT_FRAME_MAIN_CURTAIN , work->fontHandle );
  DEBUGWIN_ChangeLetterColor( 31,31,31 );
#endif  //USE_DEBUGWIN_SYSTEM
//  OS_TPrintf("FreeHeap:[%x]\n", GFL_HEAP_GetHeapFreeSize( work->heapId ) );
  return work;
}


void  STA_ACT_TermActing( ACTING_WORK *work )
{
#if USE_DEBUGWIN_SYSTEM
  DEBUGWIN_ExitProc();
#endif  //USE_DEBUGWIN_SYSTEM
  //フェードないので仮処理
  GX_SetMasterBrightness(-16);  
  GXS_SetMasterBrightness(-16);
  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_NONE , GX_BLEND_PLANEMASK_NONE , 31 , 31 );
  
  INFOWIN_Exit();
  
  STA_SCRIPT_ExitSystem( work->scriptSys );
  if( work->scriptData != NULL )
  {
    GFL_HEAP_FreeMemory( work->scriptData );
  }

  GFL_MSG_Delete( work->msgHandle );
  GFL_BMPWIN_Delete( work->msgWin );
  GFL_FONT_Delete( work->fontHandle );
  GFL_TCBL_Exit( work->tcblSys );
  
  STA_AUDI_ExitSystem( work->audiSys );
  STA_BG_ExitSystem( work->bgSys );
  STA_LIGHT_ExitSystem( work->lightSys );
  STA_EFF_ExitSystem( work->effSys );
  STA_OBJ_ExitSystem( work->objSys );
  STA_POKE_ExitSystem( work->pokeSys );

  MUS_POKE_DRAW_TermSystem( work->drawSys );
  MUS_ITEM_DRAW_TermSystem( work->itemDrawSys );

  GFL_CLACT_SYS_Delete();

  GFL_BBD_DeleteSys( work->bbdSys );
  GFL_G3D_CAMERA_Delete( work->camera );
  GFL_G3D_Exit();

  GFL_BG_FreeBGControl( ACT_FRAME_MAIN_3D );
  GFL_BG_FreeBGControl( ACT_FRAME_MAIN_FONT );
  GFL_BG_FreeBGControl( ACT_FRAME_MAIN_CURTAIN );
  GFL_BG_FreeBGControl( ACT_FRAME_MAIN_BG );
  GFL_BG_FreeBGControl( ACT_FRAME_SUB_BG );
  GFL_BG_FreeBGControl( ACT_FRAME_SUB_INFO );
  GFL_BMPWIN_Exit();
  GFL_BG_Exit();

  GFL_TCB_DeleteTask( work->vblankFuncTcb );

  GFL_HEAP_FreeMemory( work );
  
}

ACTING_RETURN STA_ACT_LoopActing( ACTING_WORK *work )
{
#if DEB_ARI|1
  if( STA_SCRIPT_GetRunningScriptNum( work->scriptSys ) == 0 )
  {
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    {
      
      //演劇風
      //STA_SCRIPT_SetScript( work->scriptSys , (void*)musicalScriptTestData );
      if( work->scriptData != NULL )
      {
        GFL_HEAP_FreeMemory( work->scriptData );
      }
      work->scriptData = GFL_ARC_UTIL_Load( ARCID_MUSICAL_SCRIPT , NARC_musical_script_mus_002_001_bin , FALSE , work->heapId );
      STA_ACT_StartScript( work );
    }
  }
/*
  if( work->initWork->commWork != NULL )
  {
    static u32 isStartCnt = 0;
    static BOOL isWait = FALSE;
    MUS_COMM_WORK *commWork = work->initWork->commWork;
    //if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    if( STA_SCRIPT_GetRunningScriptNum( work->scriptSys ) == 0 &&
        isWait == FALSE )
    {
      MUS_COMM_SendTimingCommand( commWork , MUS_COMM_TIMMING_START_SCRIPT+isStartCnt );
      isWait = TRUE;
    }
    
    if( STA_SCRIPT_GetRunningScriptNum( work->scriptSys ) == 0 )
    {
      static u32 vcount = 0;
      if( vcount != 0 )
      {
        u32 nowVCount = OS_GetVBlankCount();
        OS_TPrintf("Script count[%d]\n",nowVCount-vcount);
        vcount = 0;
      }
      if( MUS_COMM_CheckTimingCommand( commWork , MUS_COMM_TIMMING_START_SCRIPT+isStartCnt ) == TRUE )
      {
        if( work->scriptData != NULL )
        {
          GFL_HEAP_FreeMemory( work->scriptData );
        }
        work->scriptData = GFL_ARC_UTIL_Load( ARCID_MUSICAL_SCRIPT , NARC_musical_script_mus_002_001_bin , FALSE , work->heapId );
        STA_ACT_StartScript( work );
        vcount = OS_GetVBlankCount(); 
        isStartCnt++;
        isWait = FALSE;
        OS_TPrintf("-------------------------------\n");
        OS_TPrintf("Start script[%d]\n",isStartCnt);
        OS_TPrintf("-------------------------------\n");
      }
    }
  }
*/

#endif

  INFOWIN_Update();
  STA_ACT_UpdateScroll(work);

  STA_SCRIPT_UpdateSystem( work->scriptSys );

  STA_BG_UpdateSystem( work->bgSys );
  STA_POKE_UpdateSystem( work->pokeSys );
  STA_OBJ_UpdateSystem( work->objSys );
  STA_EFF_UpdateSystem( work->effSys );
  STA_ACT_UpdateMessage( work );
  
  MUS_POKE_DRAW_UpdateSystem( work->drawSys ); 

  //ポケとかに追従する可能性もあるので最後
  STA_LIGHT_UpdateSystem( work->lightSys );

  STA_AUDI_UpdateSystem( work->audiSys );

  //3D描画  
  GFL_G3D_DRAW_Start();
  GFL_G3D_DRAW_SetLookAt();
  {
    STA_LIGHT_DrawSystem( work->lightSys );
    STA_POKE_DrawSystem( work->pokeSys );
    MUS_POKE_DRAW_DrawSystem( work->drawSys ); 
//    STA_OBJ_DrawSystem( work->objSys );
    STA_POKE_UpdateSystem_Item( work->pokeSys );  //ポケの描画後に入れること
    STA_BG_DrawSystem( work->bgSys );
    GFL_BBD_Draw( work->bbdSys , work->camera , NULL );
    STA_EFF_DrawSystem( work->effSys );

  }
  GFL_G3D_DRAW_End();
  //OBJの更新
  GFL_CLACT_SYS_Main();

#if DEB_ARI|1
  if( GFL_UI_KEY_GetCont() & PAD_BUTTON_SELECT &&
    GFL_UI_KEY_GetCont() & PAD_BUTTON_START )
  {
    return ACT_RET_GO_END;
  }
#endif //DEB_ARI

  return ACT_RET_CONTINUE;
}

//------------------------------------------------------------------
//  VBLank Function
//------------------------------------------------------------------
static void STA_ACT_VBlankFunc(GFL_TCB *tcb,void *work)
{
  GFL_CLACT_SYS_VBlankFunc();
}

//--------------------------------------------------------------
//  描画系初期化
//--------------------------------------------------------------
static void STA_ACT_SetupGraphic( ACTING_WORK *work )
{
  GFL_DISP_GX_InitVisibleControl();
  GFL_DISP_GXS_InitVisibleControl();
  GX_SetVisiblePlane( 0 );
  GXS_SetVisiblePlane( 0 );
  WIPE_SetBrightness(WIPE_DISP_MAIN,WIPE_FADE_BLACK);
  WIPE_SetBrightness(WIPE_DISP_SUB,WIPE_FADE_BLACK);
  WIPE_ResetWndMask(WIPE_DISP_MAIN);
  WIPE_ResetWndMask(WIPE_DISP_SUB);
  
  GX_SetDispSelect(GX_DISP_SELECT_MAIN_SUB);
  GFL_DISP_SetBank( &vramBank );
  
  GFL_BG_Init( work->heapId );
  GFL_BMPWIN_Init( work->heapId );
  
  //Vram割り当ての設定
  {
    static const GFL_BG_SYS_HEADER sys_data = {
        GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BGMODE_0, GX_BG0_AS_3D,
    };
    
    // BG1 MAIN (文字
    static const GFL_BG_BGCNT_HEADER header_main1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x4800, GX_BG_CHARBASE_0x08000,0x8000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG2 MAIN (幕
    static const GFL_BG_BGCNT_HEADER header_main2 = {
      0, 0, 0x1000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x5000, GX_BG_CHARBASE_0x00000,0x4000,
      GX_BG_EXTPLTT_01, 1, 1, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 MAIN (背景 256*16色
    static const GFL_BG_BGCNT_HEADER header_main3 = {
      0, 0, 0x2000, 0,  // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_256,
      GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x10000,0x10000,
      GX_BG_EXTPLTT_23, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    // BG1 SUB (背景
    static const GFL_BG_BGCNT_HEADER header_sub1 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000,0x7000,
      GX_BG_EXTPLTT_01, 3, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };
    // BG3 SUB (Info
    static const GFL_BG_BGCNT_HEADER header_sub3 = {
      0, 0, 0x800, 0, // scrX, scrY, scrbufSize, scrbufofs,
      GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0x7000, GX_BG_CHARBASE_0x08000,0x4000,
      GX_BG_EXTPLTT_01, 0, 0, 0, FALSE  // pal, pri, areaover, dmy, mosaic
    };

    GFL_BG_SetBGMode( &sys_data );
    STA_ACT_SetupBgFunc( &header_main1, ACT_FRAME_MAIN_FONT , GFL_BG_MODE_TEXT);
    STA_ACT_SetupBgFunc( &header_main2, ACT_FRAME_MAIN_CURTAIN , GFL_BG_MODE_TEXT);
    STA_ACT_SetupBgFunc( &header_main3, ACT_FRAME_MAIN_BG , GFL_BG_MODE_256X16);
    STA_ACT_SetupBgFunc( &header_sub1 , ACT_FRAME_SUB_BG , GFL_BG_MODE_TEXT);
    STA_ACT_SetupBgFunc( &header_sub3 , ACT_FRAME_SUB_INFO , GFL_BG_MODE_TEXT);
    
    GFL_BG_SetVisible( ACT_FRAME_MAIN_3D , TRUE );
  }
  
  { //3D系の設定
    static const VecFx32 cam_pos = MUSICAL_CAMERA_POS;
    static const VecFx32 cam_target = MUSICAL_CAMERA_TRG;
    static const VecFx32 cam_up = MUSICAL_CAMERA_UP;
    //エッジマーキングカラー
    static  const GXRgb stage_edge_color_table[8]=
      { GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 0, 0, 0, 0, 0, 0 };
    GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT80K,
            0, work->heapId, NULL );
    GFL_BG_SetBGControl3D( 2 ); //NNS_g3dInitの中で表示優先順位変わる・・・
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_W );
#if 0 //透視射影カメラ
    work->camera =  GFL_G3D_CAMERA_Create( GFL_G3D_PRJPERS, 
                       FX32_SIN13,
                       FX32_COS13,
                       FX_F32_TO_FX32( 1.33f ),
                       NULL,
                       FX32_ONE,
                       FX32_ONE * 300,
                       NULL,
                       &cam_pos,
                       &cam_up,
                       &cam_target,
                       work->heapId );
#else
    //正射影カメラ
    work->camera =  GFL_G3D_CAMERA_Create( GFL_G3D_PRJORTH, 
                       FX32_ONE*12.0f,
                       0,
                       0,
                       FX32_ONE*16.0f,
                       MUSICAL_CAMERA_NEAR,
                       MUSICAL_CAMERA_FAR,
                       NULL,
                       &cam_pos,
                       &cam_up,
                       &cam_target,
                       work->heapId );
#endif
    
    GFL_G3D_CAMERA_Switching( work->camera );
    //エッジマーキングカラーセット
    G3X_SetEdgeColorTable( &stage_edge_color_table[0] );
    G3X_EdgeMarking( TRUE );
    G3X_AntiAlias( FALSE );
    G3X_AlphaBlend( TRUE );
    
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO , GX_BUFFERMODE_Z );
  }
  {
    GFL_BBD_SETUP bbdSetup = {
      128,128,
      {FX32_ONE,FX32_ONE,FX32_ONE},
      GX_RGB(0,0,0),
      GX_RGB(0,0,0),
      GX_RGB(0,0,0),
      GX_RGB(0,0,0),
      ACT_POLYID_SHADOW, //影用の
			GFL_BBD_ORIGIN_CENTER,
    };
    VecFx32 scale ={FX32_ONE*4,FX32_ONE*4,FX32_ONE};
    //ビルボードシステム構築
    work->bbdSys = GFL_BBD_CreateSys( &bbdSetup , work->heapId );
    //背景のために全体を４倍する
    GFL_BBD_SetScale( work->bbdSys , &scale );
  }
  
  //CELL初期化(ライト・観客用)
  {
    GFL_CLSYS_INIT cellSysInitData = GFL_CLSYSINIT_DEF_DIVSCREEN;
    cellSysInitData.oamst_main = 0x10;  //デバッグメータの分
    cellSysInitData.oamnum_main = 128-0x10;
    cellSysInitData.CGR_RegisterMax = 48;
    cellSysInitData.CELL_RegisterMax = 48;
    cellSysInitData.tr_cell = 48;
    GFL_CLACT_SYS_Create( &cellSysInitData , &vramBank ,work->heapId );
    
    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ , TRUE );
  }
  //ライト用のアルファ設定
  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_OBJ , GX_BLEND_PLANEMASK_BG0|GX_BLEND_PLANEMASK_BG3 , 4 , 31 );
}

//--------------------------------------------------------------
//  BGの初期化
//--------------------------------------------------------------
static void STA_ACT_SetupBg( ACTING_WORK *work )
{
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_STAGE_GRA , work->heapId );
/*
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_stage_gra_stage_bg01_NCLR , 
                    PALTYPE_MAIN_BG_EX , 0x6000 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_stage_gra_stage_bg01_NCGR ,
                    ACT_FRAME_MAIN_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_stage_gra_stage_bg01_NSCR , 
                    ACT_FRAME_MAIN_BG ,  0 , 0, FALSE , work->heapId );
*/
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_stage_gra_maku_NCLR , 
                    PALTYPE_MAIN_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_stage_gra_maku_NCGR ,
                    ACT_FRAME_MAIN_CURTAIN , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_stage_gra_maku_NSCR , 
                    ACT_FRAME_MAIN_CURTAIN ,  0 , 0, FALSE , work->heapId );

  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_stage_gra_audience_NCLR , 
                    PALTYPE_SUB_BG , 0 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_stage_gra_audience_NCGR ,
                    ACT_FRAME_SUB_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_stage_gra_audience_NSCR , 
                    ACT_FRAME_SUB_BG ,  0 , 0, FALSE , work->heapId );

  GFL_ARC_CloseDataHandle(arcHandle);

  GFL_BG_LoadScreenReq(ACT_FRAME_MAIN_BG);
  GFL_BG_LoadScreenReq(ACT_FRAME_SUB_BG);
  
  work->bgSys = STA_BG_InitSystem( work->heapId , work );

}

void  STA_ACT_LoadBg( ACTING_WORK *work , const u8 bgNo )
{
/*
  ARCHANDLE *arcHandle = GFL_ARC_OpenDataHandle( ARCID_STAGE_GRA , work->heapId );
  GFL_ARCHDL_UTIL_TransVramPalette( arcHandle , NARC_stage_gra_stage_bg00_NCLR + bgNo, 
                    PALTYPE_MAIN_BG_EX , 0x6000 , 0 , work->heapId );
  GFL_ARCHDL_UTIL_TransVramBgCharacter( arcHandle , NARC_stage_gra_stage_bg00_NCGR  + bgNo,
                    ACT_FRAME_MAIN_BG , 0 , 0, FALSE , work->heapId );
  GFL_ARCHDL_UTIL_TransVramScreen( arcHandle , NARC_stage_gra_stage_bg00_NSCR  + bgNo, 
                    ACT_FRAME_MAIN_BG ,  0 , 0, FALSE , work->heapId );
  GFL_ARC_CloseDataHandle(arcHandle);
  GFL_BG_LoadScreenReq(ACT_FRAME_MAIN_BG);
*/

  STA_BG_CreateBg( work->bgSys , ARCID_STAGE_GRA , NARC_stage_gra_back_00_nsbtx + bgNo );

}

//--------------------------------------------------------------------------
//  Bg初期化 機能部
//--------------------------------------------------------------------------
static void STA_ACT_SetupBgFunc( const GFL_BG_BGCNT_HEADER *bgCont , u8 bgPlane , u8 mode )
{
  GFL_BG_SetBGControl( bgPlane, bgCont, mode );
  GFL_BG_SetVisible( bgPlane, VISIBLE_ON );
  GFL_BG_ClearFrame( bgPlane );
  GFL_BG_LoadScreenReq( bgPlane );
}

//--------------------------------------------------------------
//  表示ポケモンの初期化
//--------------------------------------------------------------
static void STA_ACT_SetupPokemon( ACTING_WORK *work )
{
  u8 i;
  VecFx32 pos = {0,FX32_CONST(160.0f),FX32_CONST(170.0f)};
  const fx32 XBase = FX32_CONST(512.0f/(MUSICAL_POKE_MAX+1));
  
  work->drawSys = MUS_POKE_DRAW_InitSystem( work->heapId );
  work->itemDrawSys = MUS_ITEM_DRAW_InitSystem( work->bbdSys , MUSICAL_POKE_MAX*MUS_POKE_EQUIP_MAX, work->heapId );
  
  work->pokeSys = STA_POKE_InitSystem( work->heapId , work->drawSys , work->itemDrawSys , work->bbdSys );

  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    work->pokeWork[i] = NULL;
  }

  
  for( i=0;i<MUSICAL_POKE_MAX;i++ )
  {
    pos.x = XBase*(i+1);
    work->pokeWork[i] = STA_POKE_CreatePoke( work->pokeSys , &work->initWork->musPoke[i] );
    STA_POKE_SetPosition( work->pokeSys , work->pokeWork[i] , &pos );

    pos.z -= FX32_CONST(30.0f); //ひとキャラの厚みは30と見る
  }
}

//--------------------------------------------------------------
//アイテムの初期化
//--------------------------------------------------------------
static void STA_ACT_SetupItem( ACTING_WORK *work )
{
  int i;
  VecFx32 pos = {0,0,0};
  
  for( i=0;i<ACT_OBJECT_MAX;i++ )
  {
    work->objWork[i] = NULL;
  }

  //背景Obj
  {
    work->objSys = STA_OBJ_InitSystem( work->heapId , work->bbdSys );
/*
    pos.x = FX32_CONST( 64.0f );
    pos.y = FX32_CONST( 96.0f );
    pos.z = FX32_CONST( 50.0f);
    work->objWork[0] = STA_OBJ_CreateObject( work->objSys , 0 );
    STA_OBJ_SetPosition( work->objSys , work->objWork[0] , &pos );

    pos.x = FX32_CONST( 448.0f );
    pos.y = FX32_CONST( 100.0f );
    pos.z = FX32_CONST( 50.0f );
    work->objWork[1] = STA_OBJ_CreateObject( work->objSys , 1 );
    STA_OBJ_SetPosition( work->objSys , work->objWork[1] , &pos );

    pos.x = FX32_CONST( 192.0f );
    pos.y = FX32_CONST( 160.0f );
    pos.z = FX32_CONST( 180.0f );
    work->objWork[2] = STA_OBJ_CreateObject( work->objSys , 1 );
    STA_OBJ_SetPosition( work->objSys , work->objWork[2] , &pos );
*/
  }


}

static void STA_ACT_SetupEffect( ACTING_WORK *work )
{
  int i;
  work->effSys = STA_EFF_InitSystem( work->heapId);

  for( i=0;i<ACT_EFFECT_MAX;i++ )
  {
    work->effWork[i] = NULL;
  }

//  work->effWork[0] = STA_EFF_CreateEffect( work->effSys , NARC_stage_gra_mus_eff_00_spa );
//  work->effWork[1] = STA_EFF_CreateEffect( work->effSys , NARC_stage_gra_mus_eff_01_spa );
}

static void STA_ACT_UpdateScroll( ACTING_WORK *work )
{
  const u8 spd = 2;
#if DEB_ARI|1
  if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
  {
    if( work->scrollOffset + spd < ACT_BG_SCROLL_MAX )
    {
      work->scrollOffset += spd;
    }
    else
    {
      work->scrollOffset = ACT_BG_SCROLL_MAX;
    }
    work->updateScroll = TRUE;
  }
  if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
  {
    if( work->scrollOffset - spd > 0 )
    {
      work->scrollOffset -= spd;
    }
    else
    {
      work->scrollOffset = 0;
    }
    
    work->updateScroll = TRUE;
  }
#endif
  
  if( work->updateScroll == TRUE )
  {
    //GFL_BG_SetScroll( ACT_FRAME_MAIN_BG , GFL_BG_SCROLL_X_SET , work->scrollOffset );
    GFL_BG_SetScroll( ACT_FRAME_MAIN_CURTAIN , GFL_BG_SCROLL_X_SET , work->scrollOffset );
    STA_BG_SetScrollOffset( work->bgSys , work->scrollOffset );
    STA_POKE_System_SetScrollOffset( work->pokeSys , work->scrollOffset ); 
    STA_OBJ_System_SetScrollOffset( work->objSys , work->scrollOffset ); 
    work->updateScroll = FALSE;
  }
  
#if DEB_ARI
  if( GFL_UI_KEY_GetCont() & PAD_KEY_UP )
  {
    if( work->makuOffset + spd < ACT_CURTAIN_SCROLL_MAX )
    {
      work->makuOffset += spd;
    }
    else
    {
      work->makuOffset = ACT_CURTAIN_SCROLL_MAX;
    }
  }
  if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN )
  {
    if( work->makuOffset - spd > 0 )
    {
      work->makuOffset -= spd;
    }
    else
    {
      work->makuOffset = 0;
    }
    
  }
#endif
  GFL_BG_SetScroll( ACT_FRAME_MAIN_CURTAIN , GFL_BG_SCROLL_Y_SET , work->makuOffset );
}

static void STA_ACT_StartScript( ACTING_WORK *work )
{
  u8 i;
  const u32 *headData = (u32*)work->scriptData;
  const u8 pokeScriptArr[4] = 
  {
    STA_SCRIPT_POKE_1,
    STA_SCRIPT_POKE_2,
    STA_SCRIPT_POKE_3,
    STA_SCRIPT_POKE_4
  };

  STA_SCRIPT_SetScript( work->scriptSys , (u8*)work->scriptData+headData[STA_SCRIPT_MAIN] , TRUE );
  for( i=0; i<4 ; i++ )
  {
    if( headData[pokeScriptArr[i]] != headData[STA_SCRIPT_MAIN] )
    {
      STA_SCRIPT_SetScript( work->scriptSys , (u8*)work->scriptData+headData[pokeScriptArr[i]] , TRUE );
    }
  }
}

#pragma mark [> message func
//--------------------------------------------------------------
//  メッセージ関係
//--------------------------------------------------------------
static void STA_ACT_SetupMessage( ACTING_WORK *work )
{
  //メッセージ用処理
  work->msgWin = GFL_BMPWIN_Create( ACT_FRAME_MAIN_FONT , ACT_MSG_POS_X , ACT_MSG_POS_Y ,
                  ACT_MSG_POS_WIDTH , ACT_MSG_POS_HEIGHT , ACT_PAL_FONT ,
                  GFL_BMP_CHRAREA_GET_B );
  GFL_BMPWIN_TransVramCharacter( work->msgWin );
  GFL_BMPWIN_MakeScreen( work->msgWin );
  GFL_BG_LoadScreenReq(ACT_FRAME_MAIN_FONT);
  
  
  //フォント読み込み
  work->fontHandle = GFL_FONT_Create( ARCID_FONT , NARC_font_large_nftr , GFL_FONT_LOADTYPE_FILE , FALSE , work->heapId );
  
  //メッセージ
  work->msgHandle = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL , ARCID_MESSAGE , NARC_message_musical_00_dat , work->heapId );

  GFL_ARC_UTIL_TransVramPalette( ARCID_FONT , NARC_font_default_nclr , PALTYPE_MAIN_BG , ACT_PAL_FONT*0x20, 16*2, work->heapId );
  
  work->tcblSys = GFL_TCBL_Init( work->heapId , work->heapId , 3 , 0x100 );
  work->printHandle = NULL;
}

static void STA_ACT_UpdateMessage( ACTING_WORK *work )
{
  GFL_TCBL_Main( work->tcblSys );

  if( work->printHandle != NULL  )
  {
    if( PRINTSYS_PrintStreamGetState( work->printHandle ) == PRINTSTREAM_STATE_DONE )
    {
      PRINTSYS_PrintStreamDelete( work->printHandle );
      work->printHandle = NULL;
    }
  }
}

void STA_ACT_ShowMessage( ACTING_WORK *work , const u16 msgNo , const u8 msgSpd )
{
  if( work->printHandle != NULL )
  {
    //デバッグ時にした画面の文字切り替えにで文字がバグる事があったため
//    GF_ASSERT_MSG( NULL , "Message is not finish!!\n" )
    OS_TPrintf( NULL , "Message is not finish!!\n" );
    PRINTSYS_PrintStreamDelete( work->printHandle );
    work->printHandle = NULL;
  }

  {
    STRBUF  *str = GFL_STR_CreateBuffer( 128 , work->heapId );
    
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->msgWin ) , 0 );
    
    GFL_MSG_GetString( work->msgHandle , msgNo , str );
    work->printHandle = PRINTSYS_PrintStream( work->msgWin , 0,0, str ,work->fontHandle ,
                        msgSpd , work->tcblSys , 2 , work->heapId , 0 );
    GFL_STR_DeleteBuffer( str );
  }
}

void STA_ACT_HideMessage( ACTING_WORK *work )
{
  if( work->printHandle != NULL )
  {
    GF_ASSERT_MSG( NULL , "Message is not finish!!\n" )
    PRINTSYS_PrintStreamDelete( work->printHandle );
    work->printHandle = NULL;
  }
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( work->msgWin ) , 0 );
  GFL_BMPWIN_TransVramCharacter( work->msgWin );
}

#pragma mark [> BGM func
//--------------------------------------------------------------
//  BGM関係
//--------------------------------------------------------------
void  STA_ACT_StartBgm(  ACTING_WORK *work )
{
  SND_STRM_SetUp( ARCID_SNDSTRM, NARC_snd_strm_poketari_swav, SND_STRM_PCM8, SND_STRM_8KHZ, work->heapId );
  SND_STRM_Play();
}

void  STA_ACT_StopBgm(  ACTING_WORK *work )
{
  SND_STRM_Stop();
  SND_STRM_Release();
}


#pragma mark [> offer func
//--------------------------------------------------------------
//  スクリプト用に外部提供関数
//--------------------------------------------------------------
STA_POKE_SYS* STA_ACT_GetPokeSys( ACTING_WORK *work )
{
  return work->pokeSys;
}
STA_POKE_WORK* STA_ACT_GetPokeWork( ACTING_WORK *work , const u8 idx )
{
  GF_ASSERT( idx < MUSICAL_POKE_MAX );
  return work->pokeWork[idx];
}
void STA_ACT_SetPokeWork( ACTING_WORK *work , STA_POKE_WORK *pokeWork , const u8 idx )
{
  GF_ASSERT( idx < MUSICAL_POKE_MAX );
  work->pokeWork[idx] = pokeWork;
}

STA_OBJ_SYS* STA_ACT_GetObjectSys( ACTING_WORK *work )
{
  return work->objSys;
}
STA_OBJ_WORK* STA_ACT_GetObjectWork( ACTING_WORK *work , const u8 idx )
{
  GF_ASSERT( idx < ACT_OBJECT_MAX );
  return work->objWork[idx];
}
void STA_ACT_SetObjectWork( ACTING_WORK *work , STA_OBJ_WORK *objWork , const u8 idx )
{
  GF_ASSERT( idx < ACT_OBJECT_MAX );
  work->objWork[idx] = objWork;
}

STA_EFF_SYS* STA_ACT_GetEffectSys( ACTING_WORK *work )
{
  return work->effSys;
}
STA_EFF_WORK* STA_ACT_GetEffectWork( ACTING_WORK *work , const u8 idx )
{
  GF_ASSERT( idx < ACT_EFFECT_MAX );
  return work->effWork[idx];
}
void STA_ACT_SetEffectWork( ACTING_WORK *work , STA_EFF_WORK *effWork , const u8 idx )
{
  GF_ASSERT( idx < ACT_EFFECT_MAX );
  work->effWork[idx] = effWork;
}

STA_LIGHT_SYS* STA_ACT_GetLightSys( ACTING_WORK *work )
{
  return work->lightSys;
}
STA_LIGHT_WORK* STA_ACT_GetLightWork( ACTING_WORK *work , const u8 idx )
{
  GF_ASSERT( idx < ACT_LIGHT_MAX );
  return work->lightWork[idx];
}
void STA_ACT_SetLightWork( ACTING_WORK *work , STA_LIGHT_WORK *lightWork , const u8 idx )
{
  GF_ASSERT( idx < ACT_LIGHT_MAX );
  work->lightWork[idx] = lightWork;
}

STA_AUDI_SYS* STA_ACT_GetAudienceSys( ACTING_WORK *work )
{
  return work->audiSys;
}

u16   STA_ACT_GetCurtainHeight( ACTING_WORK *work )
{
  return work->makuOffset;
}
void  STA_ACT_SetCurtainHeight( ACTING_WORK *work , const u16 height )
{
  work->makuOffset = height;
}
u16   STA_ACT_GetStageScroll( ACTING_WORK *work )
{
  return work->scrollOffset;
}
void  STA_ACT_SetStageScroll( ACTING_WORK *work , const u16 scroll )
{
  work->scrollOffset = scroll;
  work->updateScroll = TRUE;
}

#pragma mark [> editor func
//--------------------------------------------------------------
//エディタ用
//--------------------------------------------------------------
void  STA_ACT_EDITOR_SetScript( ACTING_WORK *work , void* data )
{
  if( work->scriptData != NULL )
  {
    GFL_HEAP_FreeMemory( work->scriptData );
  }
  work->scriptData = data;
}

void  STA_ACT_EDITOR_StartScript( ACTING_WORK *work )
{
  if( work->scriptData != NULL )
  {
    STA_ACT_StartScript( work );
  }
}

