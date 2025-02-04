//============================================================================================
/**
 * @file  effect_viewer.c
 * @brief MCSを利用してエフェクトを見るビューワー
 * @author  soga
 * @date  2009.03.02
 */
//============================================================================================

#include <gflib.h>
#include <procsys.h>
#include <tcbl.h>

#include "system/main.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "arc_def.h"

#include "battle/btlv/btlv_effect.h"

#include "poke_tool/poke_tool.h"
#include "poke_tool/monsno_def.h"

#include "tr_tool/trtype_def.h"

#include "sound/pm_sndsys.h"

#include "field/zonedata.h"
#include "item/itemsym.h"

#include "debug/gf_mcs.h"

#include "font/font.naix"
#include "message.naix"
#include "debug_message.naix"
#include "effect_viewer.h"

#include "msg/debug/msg_ev_bgm_list.h"
#include "ev_bgm_list.cdat"

#if PM_DEBUG

#define PAD_BUTTON_EXIT ( PAD_BUTTON_L | PAD_BUTTON_R | PAD_BUTTON_START )

#define CAMERA_SPEED    ( FX32_ONE * 2 )
#define MOVE_SPEED      ( FX32_ONE >> 2 )
#define ROTATE_SPEED    ( 65536 / 128 )
#define PHI_MAX       ( ( 65536 / 360 ) * 89 )
#define DEFAULT_PHI     ( 65536 / 12 )
#define DEFAULT_THETA   ( 65536 / 4 )
#define DEFAULT_SCALE   ( FX32_ONE * 16  )
#define SCALE_SPEED     ( FX32_ONE )
#define SCALE_MAX       ( FX32_ONE * 200 )

#define HEAD_SIZE       ( 4 )

#define MCS_READ_CH     ( 0 )
#define MCS_WRITE_CH    ( 0 )

#define EFFECT_ENABLE_KEY  ( PAD_BUTTON_A | PAD_BUTTON_B | PAD_BUTTON_X | PAD_BUTTON_Y )

#define TURN_COUNT_MAX  ( 9 )

enum{
  BACK_COL = 0,
  SHADOW_COL = 4,
  LETTER_COL_NORMAL = 1,
  LETTER_COL_SELECT,
  LETTER_COL_CURSOR,
};

typedef enum
{ 
  EFFECT_TYPE_MCS = 0,
  EFFECT_TYPE_WAZA,
  EFFECT_TYPE_STATUS,
}EFFECT_TYPE;

#define G2D_BACKGROUND_COL  ( GX_RGB( 31, 31, 31 ) )
#define G2D_FONT_COL        ( GX_RGB(  0,  0,  0 ) )
#define G2D_FONT_SELECT_COL ( GX_RGB(  0, 31,  0 ) )
#define G2D_FONT_CURSOR_COL ( GX_RGB( 31,  0,  0 ) )
#define G2D_SHADOW_COL      ( GX_RGB( 16, 16, 16 ) )

enum{
  SEND_SEQUENCE = 1,
  SEND_SEQUENCE_OK,
  SEND_RESOURCE,
  SEND_RESOURCE_OK,
  SEND_RECEIVE,
  SEND_RECEIVE_OK,
  SEND_DECIDE,
  SEND_CANCEL,
  SEND_IDLE,
};

enum{
  SEQ_IDLE = 0,
  SEQ_LOAD_SEQUENCE_DATA,
  SEQ_LOAD_RESOURCE_DATA,
  SEQ_EFFECT_WAIT,
  SEQ_RECEIVE_ACTION,
  SEQ_EFFECT_VIEW,
  SEQ_STATUS_EFFECT_VIEW,
  SEQ_BGM_SELECT,
};

enum{
  SUB_SEQ_INIT = 0,
  SUB_SEQ_PARTICLE_PLAY_PARAM,
  SUB_SEQ_PARTICLE_PLAY_PARAM_SEND,
};

enum{
  EV_WAZANO_X = 44,
  EV_WAZANO_Y = 90 + 56,
  EV_WAZANAME_X = 80,

  EV_STATUS_EFFECT_X = 68,
  EV_STATUS_EFFECT_Y = 70,

  EV_STATUS_EFFECT_NUM_X = 80,
  EV_STATUS_EFFECT_NUM_Y = 90,

  EV_BGMNO_Y = 90,
};

typedef enum
{
  DRAW_REQ_NONE = 0,
  DRAW_REQ_MENU_LIST,
  DRAW_REQ_WAZA_NO,
  DRAW_REQ_STATUS_EFFECT,
  DRAW_REQ_BGM_NO,
}EV_DRAW_REQ;

typedef struct
{
  HEAPID        heapID;

  int           seq_no;
  int           ret_seq_no;
  int           sub_seq_no;
  int           mcs_enable;
  POKEMON_PARAM *pp;
  int           mons_no;
  int           tr_type;
  int           waza_no;
  int           bgm_no;
  int           se_no;
  GFL_BMPWIN    *bmpwin;
  int           key_repeat_speed;
  int           key_repeat_wait;
  int           trg;

  GFL_MSGDATA   *msg;
  GFL_FONT      *font;
  GFL_FONT      *small_font;

  int           menu_list;

  u8            *param_start;
  u32           *param;
  int           edit_param;
  int           cursor_pos;
  int           cursor_keta;

  int           answer;

  EV_DRAW_REQ   draw_req;
  EV_DRAW_REQ   draw_req_old;

  void          *sequence_data;
  void          *resource_data;

  BOOL          viewer_mode;
  BOOL          gauge_mode;

  int           bgm_flag;
  int           bgm_fade_flag;
  int           pinch_bgm_flag;

  int           turn_count;

  BtlRule       rule;

  GFL_PTC_PTR   ptc;
  ZUKAN_SAVEDATA* zs;

}EFFECT_VIEWER_WORK;

static  void  EffectViewerSequence( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerRead( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerSequenceLoad( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerResourceLoad( EFFECT_VIEWER_WORK *evw );
static  BOOL  EffectViewerRecieveAction( EFFECT_VIEWER_WORK *evw );
static  BOOL  EffectViewerEffectView( EFFECT_VIEWER_WORK *evw );
static  BOOL  EffectViewerStatusEffect( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerDrawWazaNo( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerDrawStatusEffect( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerDrawBgmNo( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerInitMenuList( EFFECT_VIEWER_WORK *evw, int menu_list );
static  void  EffectViewerDrawMenuList( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerDrawCursor( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerDrawMenuLabel( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerDrawMenuData( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerDrawMenuDataNum( EFFECT_VIEWER_WORK *evw, const MENU_SCREEN_DATA *msd_p, int param );
static  void  EffectViewerDrawMenuDataComboBox( EFFECT_VIEWER_WORK *evw, const MENU_SCREEN_DATA *msd_p, int param );
static  void  EffectViewerEditMenuList( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerMoveAction( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerEditAction( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerParticleResourceLoad( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerParticlePlay( EFFECT_VIEWER_WORK *evw );
static  void  EffectViewerParticleEnd( EFFECT_VIEWER_WORK *evw );

static  u32   *EffectViewerMakeSendData( EFFECT_VIEWER_WORK *evw, int param_start, int param_count );

static  void  MoveCamera( EFFECT_VIEWER_WORK *evw );
static  BOOL  check_touch_panel( EFFECT_VIEWER_WORK* evw, EFFECT_TYPE type );

static  void  set_pokemon( EFFECT_VIEWER_WORK *evw );
static  void  del_pokemon( EFFECT_VIEWER_WORK *evw );
static  int   ev_pow( int x, int y );
static  int   ev_param_get( EFFECT_VIEWER_WORK* evw, int param );
static  void  pinch_bgm_check( EFFECT_VIEWER_WORK* evw );
static  void  draw_dir_touch_screen( EFFECT_VIEWER_WORK* evw );

static  const int pokemon_pos_table[][2]={
  { BTLV_MCSS_POS_AA, BTLV_MCSS_POS_BB },
  { BTLV_MCSS_POS_A, BTLV_MCSS_POS_B },
  { BTLV_MCSS_POS_C, BTLV_MCSS_POS_D }
};

FS_EXTERN_OVERLAY(battle_view);
FS_EXTERN_OVERLAY(battle);

//--------------------------------------------------------------------------
/**
 * PROC Init
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT EffectViewerProcInit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EFFECT_VIEWER_WORK *evw;
  static const GFL_DISP_VRAM dispvramBank = {
    GX_VRAM_BG_128_A,       // メイン2DエンジンのBG
    GX_VRAM_BGEXTPLTT_NONE,     // メイン2DエンジンのBG拡張パレット
    GX_VRAM_SUB_BG_32_H,      // サブ2DエンジンのBG
    GX_VRAM_SUB_BGEXTPLTT_NONE,   // サブ2DエンジンのBG拡張パレット
    GX_VRAM_OBJ_64_E,       // メイン2DエンジンのOBJ
    GX_VRAM_OBJEXTPLTT_NONE,    // メイン2DエンジンのOBJ拡張パレット
    GX_VRAM_SUB_OBJ_16_I,     // サブ2DエンジンのOBJ
    GX_VRAM_SUB_OBJEXTPLTT_NONE,  // サブ2DエンジンのOBJ拡張パレット
    GX_VRAM_TEX_01_BC,        // テクスチャイメージスロット
    GX_VRAM_TEXPLTT_01_FG,      // テクスチャパレットスロット
    GX_OBJVRAMMODE_CHAR_1D_64K,   // メインOBJマッピングモード
    GX_OBJVRAMMODE_CHAR_1D_32K,   // サブOBJマッピングモード
  };

  GFL_OVERLAY_Load( FS_OVERLAY_ID( battle_view ) );
  GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );

  GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_SOGABE_DEBUG, 0xc0000 );
  evw = GFL_PROC_AllocWork( proc, sizeof( EFFECT_VIEWER_WORK ), HEAPID_SOGABE_DEBUG );
  MI_CpuClearFast( evw, sizeof( EFFECT_VIEWER_WORK ) );
  evw->heapID = HEAPID_SOGABE_DEBUG;
  evw->zs = (ZUKAN_SAVEDATA*)pwk;

  GFL_DISP_SetBank( &dispvramBank );

  //VRAMクリア
  MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
  MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
  MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
  MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
  MI_CpuFill16((void*)HW_BG_PLTT, 0x0000, HW_BG_PLTT_SIZE);

  G2_BlendNone();
  GFL_BG_Init( evw->heapID );
  GFL_BMPWIN_Init( evw->heapID );

  {
    static const GFL_BG_SYS_HEADER sysHeader = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_3, GX_BG0_AS_3D,
    };
    GFL_BG_SetBGMode( &sysHeader );
  }

  {
    ///< main
    GFL_BG_SetVisible( GFL_BG_FRAME0_M,   VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME1_M,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME2_M,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME3_M,   VISIBLE_OFF );

    ///< sub
    GFL_BG_SetVisible( GFL_BG_FRAME0_S,   VISIBLE_ON );
    GFL_BG_SetVisible( GFL_BG_FRAME1_S,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME2_S,   VISIBLE_OFF );
    GFL_BG_SetVisible( GFL_BG_FRAME3_S,   VISIBLE_OFF );

    ///<obj
    GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
    GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
  }
  GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

  //3D関連初期化
  {
    GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K, GFL_G3D_VMANLNK, GFL_G3D_PLT16K, 0, evw->heapID, NULL );
    GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_MANUAL, GX_BUFFERMODE_Z );
    G3X_AlphaBlend( TRUE );
    G3X_AlphaTest( FALSE, 31 );
    G3X_EdgeMarking( FALSE );
    G3X_AntiAlias( TRUE );
    GFL_BG_SetBGControl3D( 1 );
  }

  G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1,
                    GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 |
                    GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BD,
                    31, 7 );

  // フォント作成
  evw->font = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, TRUE, evw->heapID );
  evw->small_font = GFL_FONT_Create( ARCID_FONT, NARC_font_small_batt_gftr, GFL_FONT_LOADTYPE_FILE, TRUE, evw->heapID );

  //戦闘エフェクト初期化
  {
    BTL_FIELD_SITUATION bfs = { 
      0, 0, 0, 0, 0, 12, 0,
    };
    u16 tr_type[] = { 
      TRTYPE_HERO, TRTYPE_HERO, 0xffff, 0xffff,
    };
    BTLV_EFFECT_SETUP_PARAM* besp = BTLV_EFFECT_MakeSetUpParam( BTL_RULE_SINGLE, BTL_COMPETITOR_WILD, &bfs, FALSE, tr_type, NULL, NULL, evw->heapID );

    GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, &dispvramBank, evw->heapID );
    ZONEDATA_Open( evw->heapID );
    BTLV_EFFECT_Init( besp, evw->small_font, evw->heapID );
    GFL_HEAP_FreeMemory( besp );
  }

  evw->mons_no  = MONSNO_WARUBIRU;
  evw->tr_type  = TRTYPE_HERO;
  evw->rule     = BTL_RULE_SINGLE;
  set_pokemon( evw );

  //2D画面初期化
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
      ///<FRAME1_M
      {
        0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x4800, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
      },
      ///<FRAME2_M
      {
        0, 0, 0x1000, 0, GFL_BG_SCRSIZ_512x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x5000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
      },
      ///<FRAME3_M
      {
        0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x6000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
        GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
      },
      ///<FRAME0_S
      {
        0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
        GX_BG_SCRBASE_0x7800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,
        GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
      },
    };
    GFL_BG_SetBGControl(GFL_BG_FRAME1_M, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(GFL_BG_FRAME1_M );
    GFL_BG_SetBGControl(GFL_BG_FRAME2_M, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(GFL_BG_FRAME2_M );
    GFL_BG_SetBGControl(GFL_BG_FRAME3_M, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(GFL_BG_FRAME3_M );
    GFL_BG_SetBGControl(GFL_BG_FRAME0_S, &TextBgCntDat[3], GFL_BG_MODE_TEXT );
    GFL_BG_ClearScreen(GFL_BG_FRAME0_S );
  }

  {
    //フォントパレット作成＆転送
    {
      static  u16 plt[16] = {
        G2D_BACKGROUND_COL, G2D_FONT_COL, G2D_FONT_SELECT_COL, G2D_FONT_CURSOR_COL,
        G2D_SHADOW_COL, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
      PaletteWorkSet( BTLV_EFFECT_GetPfd(), &plt, FADE_SUB_BG, 0, 16 * 2 );
    }

    evw->bmpwin = GFL_BMPWIN_Create( GFL_BG_FRAME0_S, 0, 0, 32, 24, 0, GFL_BG_CHRAREA_GET_F );

  }

#if 0
  //ウインドマスク設定（画面両端のエッジマーキングのゴミを消す）
  {
    G2_SetWnd0InsidePlane( GX_WND_PLANEMASK_BG0 |
                 GX_WND_PLANEMASK_BG1 |
                 GX_WND_PLANEMASK_BG2 |
                 GX_WND_PLANEMASK_BG3 |
                 GX_WND_PLANEMASK_OBJ,
                 FALSE );
    G2_SetWndOutsidePlane( GX_WND_PLANEMASK_NONE, FALSE );
    G2_SetWnd0Position( 1, 1, 255, 191 );
    GX_SetVisibleWnd( GX_WNDMASK_W0 );
  }
#endif

  GFL_BG_SetBackGroundColor( GFL_BG_FRAME0_M, 0x0000 );

  //フェードイン
  GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 16, 0, 2 );

  GFL_UI_KEY_GetRepeatSpeed( &evw->key_repeat_speed, &evw->key_repeat_wait );
  GFL_UI_KEY_SetRepeatSpeed( evw->key_repeat_speed / 4, evw->key_repeat_wait );

  //メッセージ系初期化
  GFL_FONTSYS_Init();
  evw->msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_d_soga_dat, evw->heapID );

  EffectViewerInitMenuList( evw, MENULIST_TITLE );

  // 仮想フィールドBGMスタック設定
  {
    PMSND_PlayBGM( SEQ_BGM_R_A );
    PMSND_PauseBGM(TRUE);
    PMSND_PushBGM();
  }

  return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------
/**
 * PROC Main
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT EffectViewerProcMain( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  int pad = GFL_UI_KEY_GetCont();
  int trg = GFL_UI_KEY_GetTrg();
  int rep = GFL_UI_KEY_GetRepeat();
  int tp = GFL_UI_TP_GetTrg();
  EFFECT_VIEWER_WORK *evw = mywk;

  pinch_bgm_check( evw );

  if( evw->mcs_enable )
  {
    MCS_Main();
  }

  if( trg & PAD_BUTTON_START )
  {
    if( MCS_CheckEnable() == FALSE )
    {
      if( MCS_Init( evw->heapID ) == FALSE )
      { 
        evw->mcs_enable = 1;
      }
    }
    else
    {
      MCS_Exit();
      evw->mcs_enable = 0;
    }
  }

  EffectViewerSequence( evw );
  BTLV_EFFECT_Main();

  if( evw->seq_no == SEQ_IDLE )
  { 
    if( evw->viewer_mode )
    { 
      if( ( rep & PAD_KEY_UP ) && ( evw->tr_type < TRTYPE_MAX - 1 ) )
      { 
        evw->tr_type++;
        del_pokemon( evw );
        set_pokemon( evw );
      }
      if( ( rep & PAD_KEY_DOWN ) && ( evw->tr_type ) )
      { 
        evw->tr_type--;
        del_pokemon( evw );
        set_pokemon( evw );
      }
      if( ( rep & PAD_KEY_RIGHT ) && ( evw->tr_type + 10 <= TRTYPE_MAX - 1 ) )
      { 
        evw->tr_type += 10;
        del_pokemon( evw );
        set_pokemon( evw );
      }
      if( rep & PAD_KEY_LEFT )
      { 
        evw->tr_type -= 10;
        if( evw->tr_type < 0 )
        { 
          evw->tr_type = 0;
        }
        del_pokemon( evw );
        set_pokemon( evw );
      }
    }
    else
    { 
      if( ( rep & PAD_KEY_UP ) && ( evw->mons_no <= MONSNO_END ) )
      { 
        evw->mons_no++;
        del_pokemon( evw );
        set_pokemon( evw );
        evw->draw_req = DRAW_REQ_MENU_LIST;
      }
      if( ( rep & PAD_KEY_DOWN ) && ( evw->mons_no ) )
      { 
        evw->mons_no--;
        del_pokemon( evw );
        set_pokemon( evw );
        evw->draw_req = DRAW_REQ_MENU_LIST;
      }
      if( ( rep & PAD_KEY_RIGHT ) && ( evw->mons_no + 10 <=  MONSNO_END ) )
      { 
        evw->mons_no += 10;
        del_pokemon( evw );
        set_pokemon( evw );
        evw->draw_req = DRAW_REQ_MENU_LIST;
      }
      if( rep & PAD_KEY_LEFT )
      { 
        evw->mons_no -= 10;
        if( evw->mons_no < 0 )
        { 
          evw->mons_no = 0;
        }
        del_pokemon( evw );
        set_pokemon( evw );
        evw->draw_req = DRAW_REQ_MENU_LIST;
      }
    }
  }

  if( trg & PAD_BUTTON_SELECT )
  {
    VecFx32 cam_pos, cam_target;

    PMSND_StopSE();
    BTLV_EFFVM_Stop( BTLV_EFFECT_GetVMHandle() );
    BTLV_CAMERA_GetDefaultCameraPosition( &cam_pos, &cam_target );
    BTLV_CAMERA_MoveCameraPosition( BTLV_EFFECT_GetCameraWork(), &cam_pos, &cam_target );
    del_pokemon( evw );
    set_pokemon( evw );
  }

  if( trg & PAD_BUTTON_L )
  { 
    evw->viewer_mode ^= 1;
    del_pokemon( evw );
    set_pokemon( evw );
  }
  if( trg & PAD_BUTTON_R )
  { 
    evw->gauge_mode ^= 1;
    del_pokemon( evw );
    set_pokemon( evw );
  }

  if( trg & PAD_BUTTON_DEBUG )
  { 
    evw->rule++;
    if( evw->rule > BTL_RULE_ROTATION )
    { 
      evw->rule = BTL_RULE_SINGLE;
    }
    if( evw->rule == BTL_RULE_TRIPLE )
    { 
      BTLV_MCSS_SetMcss3vs3( BTLV_EFFECT_GetMcssWork(), 1 );
      BTLV_MCSS_SetMcssRotate( BTLV_EFFECT_GetMcssWork(), 0 );
    }
    else if( evw->rule == BTL_RULE_ROTATION )
    { 
      BTLV_MCSS_SetMcss3vs3( BTLV_EFFECT_GetMcssWork(), 0 );
      BTLV_MCSS_SetMcssRotate( BTLV_EFFECT_GetMcssWork(), 1 );
    }
    else
    { 
      BTLV_MCSS_SetMcss3vs3( BTLV_EFFECT_GetMcssWork(), 0 );
      BTLV_MCSS_SetMcssRotate( BTLV_EFFECT_GetMcssWork(), 0 );
    }
    BTLV_EFFECT_SetBtlRule( evw->rule );
    del_pokemon( evw );
    set_pokemon( evw );
    evw->draw_req = evw->draw_req_old;
  }

  if( pad == PAD_BUTTON_EXIT ){
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN, 0, 16, 2 );
    return GFL_PROC_RES_FINISH;
  }
  else{
    return GFL_PROC_RES_CONTINUE;
  }
}
//--------------------------------------------------------------------------
/**
 * PROC Exit
 */
//--------------------------------------------------------------------------
static GFL_PROC_RESULT EffectViewerProcExit( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
  EFFECT_VIEWER_WORK *evw = mywk;

  if( GFL_FADE_CheckFade() == TRUE ){
    return GFL_PROC_RES_CONTINUE;
  }

  GFL_UI_KEY_SetRepeatSpeed( evw->key_repeat_speed, evw->key_repeat_wait );

  BTLV_EFFECT_Exit();
  ZONEDATA_Close();

  GFL_CLACT_SYS_Delete();

  GFL_G3D_Exit();

  GFL_MSG_Delete( evw->msg );
  GFL_FONT_Delete( evw->font );
  GFL_FONT_Delete( evw->small_font );

  MCS_Exit();

  GFL_BMPWIN_Delete( evw->bmpwin );

  GFL_BG_Exit();
  GFL_BMPWIN_Exit();

  if( evw->param ){
    GFL_HEAP_FreeMemory( evw->param );
  }
  if( evw->sequence_data ){
    GFL_HEAP_FreeMemory( evw->sequence_data );
  }
  if( evw->resource_data ){
    GFL_HEAP_FreeMemory( evw->resource_data );
  }

  GFL_PROC_FreeWork( proc );

  GFL_HEAP_DeleteHeap( HEAPID_SOGABE_DEBUG );

  GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle_view ) );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );

  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------
/**
 *
 */
//----------------------------------------------------------
const GFL_PROC_DATA   EffectViewerProcData = {
  EffectViewerProcInit,
  EffectViewerProcMain,
  EffectViewerProcExit,
};

//======================================================================
//  メインシーケンス
//======================================================================
static  void  EffectViewerSequence( EFFECT_VIEWER_WORK *evw )
{
  int cont  = GFL_UI_KEY_GetCont();
  int trg   = GFL_UI_KEY_GetTrg();
  int tp    = GFL_UI_TP_GetTrg();

  EffectViewerDrawMenuList( evw );

  switch( evw->seq_no ){
  default:
  case SEQ_IDLE:
    if( check_touch_panel( evw, EFFECT_TYPE_MCS ) == TRUE )
    {  
      evw->waza_no = WAZANO_HATAKU;
      evw->cursor_keta = 0;
      evw->turn_count = 0;
      evw->draw_req = DRAW_REQ_WAZA_NO;
      evw->seq_no = SEQ_EFFECT_VIEW;
    }
    EffectViewerRead( evw );
    break;
  case SEQ_LOAD_SEQUENCE_DATA:
    EffectViewerSequenceLoad( evw );
    break;
  case SEQ_LOAD_RESOURCE_DATA:
    EffectViewerResourceLoad( evw );
    break;
  case SEQ_EFFECT_WAIT:
    if( BTLV_EFFECT_CheckExecute() == FALSE )
    {
      evw->seq_no = evw->ret_seq_no;
    }
    break;
  case SEQ_RECEIVE_ACTION:
    if( EffectViewerRecieveAction( evw ) == TRUE )
    {
      evw->seq_no = SEQ_IDLE;
    }
    break;
  case SEQ_EFFECT_VIEW:
    if( EffectViewerEffectView( evw ) == TRUE )
    {
      evw->waza_no = BTLEFF_STATUS_EFFECT_START;
      evw->draw_req = DRAW_REQ_STATUS_EFFECT;
      evw->seq_no = SEQ_STATUS_EFFECT_VIEW;
    }
    break;
  case SEQ_STATUS_EFFECT_VIEW:
    if( EffectViewerStatusEffect( evw ) == TRUE )
    {
      evw->draw_req = DRAW_REQ_BGM_NO;
      evw->seq_no = SEQ_BGM_SELECT;
    }
    break;
  case SEQ_BGM_SELECT:
    if( ( trg == PAD_KEY_UP ) && ( evw->bgm_no < NELEMS( ev_bgm_table ) - 1 ) )
    { 
      evw->bgm_no++;
      evw->draw_req = DRAW_REQ_BGM_NO;
    }
    else if( ( trg == PAD_KEY_DOWN ) && ( evw->bgm_no ) )
    { 
      evw->bgm_no--;
      evw->draw_req = DRAW_REQ_BGM_NO;
    }
    if( trg == PAD_BUTTON_A )
    {
      PMSND_PlayBGM( ev_bgm_table[ evw->bgm_no ] );
      evw->bgm_flag = 1;
    }
    else if( trg == PAD_BUTTON_B )
    {
      PMSND_StopBGM();
      evw->bgm_flag = 0;
    }
    else if( ( trg == PAD_BUTTON_X ) && ( evw->bgm_flag ) )
    { 
      PMSND_FadeOutBGM( 8 );
      evw->bgm_fade_flag = 1;
    }
    else if( tp )
    {
      evw->draw_req = DRAW_REQ_MENU_LIST;
      evw->seq_no = SEQ_IDLE;
    }
    break;
  }
}

//======================================================================
//  MCSからの読み込みチェック
//======================================================================
static  void  EffectViewerRead( EFFECT_VIEWER_WORK *evw )
{
  u32 size;
  u32 head;

  if( evw->mcs_enable == 0 )
  { 
    return;
  }

  if( MCS_CheckEnable() == FALSE ){
    return;
  }

  size = MCS_CheckRead();

  if( size ){
    MCS_Read( &head, HEAD_SIZE );
    if( head == SEND_SEQUENCE ){
      evw->seq_no = SEQ_LOAD_SEQUENCE_DATA;
      head = SEND_SEQUENCE_OK;
      MCS_Write( MCS_WRITE_CH, &head, 4 );
      OS_TPrintf("sequence_data head:%d\n",size);
    }
    else if( head == SEND_RESOURCE ){
      evw->seq_no = SEQ_LOAD_RESOURCE_DATA;
      head = SEND_RESOURCE_OK;
      MCS_Write( MCS_WRITE_CH, &head, 4 );
      OS_TPrintf("resource_data head:%d\n",size);
    }
    else if( head == SEND_RECEIVE ){
      evw->seq_no = SEQ_RECEIVE_ACTION;
      evw->sub_seq_no = SUB_SEQ_INIT;
      evw->answer = 0;
      head = SEND_RECEIVE_OK;
      MCS_Write( MCS_WRITE_CH, &head, 4 );
      OS_TPrintf("receive head:%d\n",size);
    }
    BTLV_EFFVM_ClearUnpackInfo( BTLV_EFFECT_GetVMHandle() );
  }
}

//======================================================================
//  MCSからのシーケンス読み込みチェック
//======================================================================
static  void  EffectViewerSequenceLoad( EFFECT_VIEWER_WORK *evw )
{
  u32 size;
  u32 head;

  size = MCS_CheckRead();

  if( size ){
    if( evw->sequence_data ){
      GFL_HEAP_FreeMemory( evw->sequence_data );
    }
    evw->sequence_data = GFL_HEAP_AllocClearMemory( evw->heapID, size );
    MCS_Read( evw->sequence_data, size );
    head = SEND_IDLE;
    MCS_Write( MCS_WRITE_CH, &head, 4 );
    evw->seq_no = SEQ_IDLE;
    OS_TPrintf("sequence_data load:%d\n",size);
  }
}

//======================================================================
//  MCSからのシーケンス読み込みチェック
//======================================================================
static  void  EffectViewerResourceLoad( EFFECT_VIEWER_WORK *evw )
{
  u32 size;
  u32 head;

  size = MCS_CheckRead();

  if( size ){
    if( evw->resource_data ){
      GFL_HEAP_FreeMemory( evw->resource_data );
    }
    evw->resource_data = GFL_HEAP_AllocClearMemory( evw->heapID, size );
    MCS_Read( evw->resource_data, size );
    head = SEND_IDLE;
    MCS_Write( MCS_WRITE_CH, &head, 4 );
    evw->seq_no = SEQ_IDLE;
    OS_TPrintf("resource_data adrs:%08x\n",evw->resource_data);
    OS_TPrintf("resource_data size:%d\n",size);
  }
}

//======================================================================
//  DSからPCへデータを受け渡す
//======================================================================
static  BOOL  EffectViewerRecieveAction( EFFECT_VIEWER_WORK *evw )
{
  BOOL  ret = FALSE;

  switch( evw->sub_seq_no ){
  case SUB_SEQ_INIT:
    {
      int *start_ofs = (int *)evw->sequence_data;
      u8 *start = (u8 *)evw->sequence_data;
      u16 *com_start = (u16 *)&start[ start_ofs[ 1 ] ];
      evw->param_start = (u8 *)&start[ start_ofs[ 1 ] + 2 ];

      switch( com_start[ 0 ] ){
      case EC_PARTICLE_PLAY:
        EffectViewerInitMenuList( evw, MENULIST_PPE );
        BTLV_MCSS_ResetOrthoMode( BTLV_EFFECT_GetMcssWork() );
        EffectViewerParticleResourceLoad( evw );
        evw->sub_seq_no = SUB_SEQ_PARTICLE_PLAY_PARAM;
        evw->param[ MLP_PPE_ATTACK ] = BTLV_MCSS_POS_AA;
        evw->param[ MLP_PPE_DEFENCE ] = BTLV_MCSS_POS_BB;
        evw->param[ MLP_PPE_OFSY ] = ev_param_get( evw, PPEPARAM_OFS_Y );
        evw->param[ MLP_PPE_ANGLE ] = ev_param_get( evw, PPEPARAM_DIR_ANGLE );
        break;
#if 0
      case EC_CAMERA_MOVE:
      case EC_POKEMON_MOVE:
      case EC_POKEMON_SCALE:
      case EC_POKEMON_ROTATE:
      case EC_POKEMON_SET_MEPACHI_FLAG:
      case EC_POKEMON_SET_ANM_FLAG:
#endif
      case EC_SE_PLAY:
        {
          int player = ev_param_get( evw, SPPARAM_PLAYER );
          evw->se_no = ev_param_get( evw, SPPARAM_SE_NO );

          if( player == BTLEFF_SEPLAY_DEFAULT )
          {
            PMSND_PlaySE( evw->se_no );
            player = PMSND_GetSE_DefaultPlayerID( evw->se_no );
          }
          else
          {
            PMSND_PlaySE_byPlayerID( evw->se_no, player );
          }
          NNS_SndPlayerSetVolume( PMSND_GetSE_SndHandle( player ), ev_param_get( evw, SPPARAM_VOLUME ) );
          PMSND_SetStatusSE_byPlayerID( player, PMSND_NOEFFECT, PMSND_NOEFFECT, 0 );
          PMSND_SetStatusSE_byPlayerID( player, PMSND_NOEFFECT, ev_param_get( evw, SPPARAM_PITCH ), PMSND_NOEFFECT );
          NNS_SndPlayerSetTrackModDepth( PMSND_GetSE_SndHandle( player ), 0xffff, ev_param_get( evw, SPPARAM_MODUDEPTH ) );
          NNS_SndPlayerSetTrackModSpeed( PMSND_GetSE_SndHandle( player ), 0xffff, ev_param_get( evw, SPPARAM_MODUSPEED ) );
        }
        break;
      case EC_SE_PAN:
        {
          int player = ev_param_get( evw, SPANPARAM_PLAYER );

          PMSND_PlaySE_byPlayerID( evw->se_no, player );

          BTLV_EFFVM_DebugSeEffect( BTLV_EFFECT_GetVMHandle(),
                                    player,
                                    ev_param_get( evw, SPANPARAM_TYPE ),
                                    BTLEFF_SEEFFECT_PAN,
                                    ev_param_get( evw, SPANPARAM_START ),
                                    ev_param_get( evw, SPANPARAM_END ),
                                    ev_param_get( evw, SPANPARAM_START_WAIT ),
                                    ev_param_get( evw, SPANPARAM_FRAME ),
                                    ev_param_get( evw, SPANPARAM_WAIT ),
                                    ev_param_get( evw, SPANPARAM_COUNT ) );
        }
        break;
      case EC_SE_EFFECT:
        {
          int player = ev_param_get( evw, SEFFPARAM_PLAYER );

          PMSND_PlaySE_byPlayerID( evw->se_no, player );

          BTLV_EFFVM_DebugSeEffect( BTLV_EFFECT_GetVMHandle(),
                                    player,
                                    ev_param_get( evw, SEFFPARAM_TYPE ),
                                    ev_param_get( evw, SEFFPARAM_PARAM ),
                                    ev_param_get( evw, SEFFPARAM_START ),
                                    ev_param_get( evw, SEFFPARAM_END ),
                                    ev_param_get( evw, SEFFPARAM_START_WAIT ),
                                    ev_param_get( evw, SEFFPARAM_FRAME ),
                                    ev_param_get( evw, SEFFPARAM_WAIT ),
                                    ev_param_get( evw, SEFFPARAM_COUNT ) );
        }
        break;
      case EC_PARTICLE_LOAD:
      case EC_EFFECT_END_WAIT:
      case EC_SEQ_END:
      default:
        break;
      }
      if( evw->sub_seq_no == SUB_SEQ_INIT )
      {
        u32 *buf;
        evw->answer = EDIT_CANCEL;
        buf = EffectViewerMakeSendData( evw, 0, 0 );
        MCS_Write( MCS_WRITE_CH, buf, 4 );
        GFL_HEAP_FreeMemory( buf );
        EffectViewerInitMenuList( evw, MENULIST_TITLE );
        ret = TRUE;
      }
    }
    break;
  case SUB_SEQ_PARTICLE_PLAY_PARAM:
    EffectViewerEditMenuList( evw );
    EffectViewerParticlePlay( evw );
    if( ( evw->answer == EDIT_DECIDE ) || ( evw->answer == EDIT_CANCEL ) ){
      evw->sub_seq_no = SUB_SEQ_PARTICLE_PLAY_PARAM_SEND;
    }
    break;
  case SUB_SEQ_PARTICLE_PLAY_PARAM_SEND:
    {
      u32 *buf;
      buf = EffectViewerMakeSendData( evw, MLP_PPE_OFSY, 2 );
      MCS_Write( MCS_WRITE_CH, buf, PPE_SEND_SIZE );
      GFL_HEAP_FreeMemory( buf );
      EffectViewerParticleEnd( evw );
      EffectViewerInitMenuList( evw, MENULIST_TITLE );
      ret = TRUE;
    }
    break;
  }
  return ret;
}

static  BOOL  EffectViewerEffectView( EFFECT_VIEWER_WORK *evw )
{
  int trg = GFL_UI_KEY_GetTrg();

  if( trg == PAD_KEY_UP )
  {
    evw->waza_no += ev_pow( 10, evw->cursor_keta );
    if( evw->waza_no >= WAZANO_MAX )
    {
      evw->waza_no = WAZANO_MAX - 1;
    }
    evw->draw_req = DRAW_REQ_WAZA_NO;
  }
  else if( trg == PAD_KEY_DOWN )
  {
    evw->waza_no -= ev_pow( 10, evw->cursor_keta );
    if( evw->waza_no < WAZANO_HATAKU )
    {
      evw->waza_no = WAZANO_HATAKU;
    }
    evw->draw_req = DRAW_REQ_WAZA_NO;
  }
  else if( ( trg == PAD_KEY_LEFT ) && ( evw->cursor_keta < 2 ) )
  {
    evw->cursor_keta++;
    evw->draw_req = DRAW_REQ_WAZA_NO;
  }
  else if( ( trg == PAD_KEY_RIGHT ) && ( evw->cursor_keta > 0 ) )
  {
    evw->cursor_keta--;
    evw->draw_req = DRAW_REQ_WAZA_NO;
  }
  else if( ( trg == PAD_BUTTON_A ) && ( evw->turn_count < TURN_COUNT_MAX ) )
  {
    evw->turn_count++;
    evw->draw_req = DRAW_REQ_WAZA_NO;
  }
  else if( ( trg == PAD_BUTTON_B ) && ( evw->turn_count > 0 ) )
  {
    evw->turn_count--;
    evw->draw_req = DRAW_REQ_WAZA_NO;
  }

  return check_touch_panel( evw, EFFECT_TYPE_WAZA );
}

static  BOOL  EffectViewerStatusEffect( EFFECT_VIEWER_WORK *evw )
{
  int trg = GFL_UI_KEY_GetTrg();

  if( ( trg == PAD_KEY_UP ) && ( evw->waza_no < BTLEFF_STATUS_EFFECT_END - 1 ) )
  {
    evw->waza_no++;
    evw->draw_req = DRAW_REQ_STATUS_EFFECT;
  }
  else if( ( trg == PAD_KEY_DOWN ) && ( evw->waza_no > BTLEFF_STATUS_EFFECT_START ) )
  {
    evw->waza_no--;
    evw->draw_req = DRAW_REQ_STATUS_EFFECT;
  }
  else if( trg == PAD_KEY_LEFT )
  {
    evw->waza_no -= 10;
    if( evw->waza_no < BTLEFF_STATUS_EFFECT_START )
    { 
      evw->waza_no = BTLEFF_STATUS_EFFECT_START;
    }
    evw->draw_req = DRAW_REQ_STATUS_EFFECT;
  }
  else if( trg == PAD_KEY_RIGHT )
  {
    evw->waza_no += 10;
    if( evw->waza_no >= BTLEFF_STATUS_EFFECT_END )
    { 
      evw->waza_no = BTLEFF_STATUS_EFFECT_END -1;
    }
    evw->draw_req = DRAW_REQ_STATUS_EFFECT;
  }

  return check_touch_panel( evw, EFFECT_TYPE_STATUS );
}

//======================================================================
//  技No表示
//======================================================================
static  void  EffectViewerDrawWazaNo( EFFECT_VIEWER_WORK *evw )
{
  STRBUF  *strbuf;
  int     keta, ofsx = 0;
  u32     num;
  int     div_num = 100;
  int     waza_no = evw->waza_no;

  {
    GFL_MSGDATA *man = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_wazaname_dat, evw->heapID );
    strbuf = GFL_MSG_CreateString( man, evw->waza_no );
    GFL_FONTSYS_SetColor( LETTER_COL_NORMAL, SHADOW_COL, BACK_COL );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), EV_WAZANAME_X, EV_WAZANO_Y, strbuf, evw->font );
    GFL_STR_DeleteBuffer( strbuf );
    GFL_MSG_Delete( man );
  }

  for( keta = 2 ; keta >= 0 ; keta-- )
  {
    if( keta == evw->cursor_keta )
    {
      GFL_FONTSYS_SetColor( LETTER_COL_CURSOR, SHADOW_COL, BACK_COL );
    }
    else
    {
      GFL_FONTSYS_SetColor( LETTER_COL_NORMAL, SHADOW_COL, BACK_COL );
    }
    num = waza_no / div_num;
    strbuf = GFL_MSG_CreateString( evw->msg,  EVMSG_NUM0 + num );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), EV_WAZANO_X + ofsx, EV_WAZANO_Y, strbuf, evw->font );
    GFL_STR_DeleteBuffer( strbuf );
    ofsx += 8;
    waza_no %= div_num;
    div_num /= 10;
  }

  GFL_FONTSYS_SetColor( LETTER_COL_NORMAL, SHADOW_COL, BACK_COL );
  strbuf = GFL_MSG_CreateString( evw->msg,  EVMSG_NUM0 + evw->turn_count );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), EV_WAZANO_X, EV_WAZANO_Y + 16, strbuf, evw->font );
  GFL_STR_DeleteBuffer( strbuf );

  draw_dir_touch_screen( evw );
}

//======================================================================
//  ステータスエフェクト表示
//======================================================================
static  void  EffectViewerDrawStatusEffect( EFFECT_VIEWER_WORK *evw )
{
  STRBUF  *strbuf;
  int     keta, ofsx = 0;
  u32     num;
  int     div_num = 100;
  int     waza_no = evw->waza_no - BTLEFF_STATUS_EFFECT_START + 1;

  strbuf = GFL_MSG_CreateString( evw->msg,  EVMSG_STATUS_EFFECT );
  GFL_FONTSYS_SetColor( LETTER_COL_NORMAL, SHADOW_COL, BACK_COL );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), EV_STATUS_EFFECT_X, EV_STATUS_EFFECT_Y, strbuf, evw->font );
  GFL_STR_DeleteBuffer( strbuf );

  for( keta = 2 ; keta >= 0 ; keta-- )
  {
    num = waza_no / div_num;
    strbuf = GFL_MSG_CreateString( evw->msg,  EVMSG_NUM0 + num );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), EV_STATUS_EFFECT_NUM_X + ofsx, EV_STATUS_EFFECT_NUM_Y, strbuf, evw->font );
    GFL_STR_DeleteBuffer( strbuf );
    ofsx += 8;
    waza_no %= div_num;
    div_num /= 10;
  }

  draw_dir_touch_screen( evw );
}

//======================================================================
//  BGMNo表示
//======================================================================
static  void  EffectViewerDrawBgmNo( EFFECT_VIEWER_WORK *evw )
{
  GFL_MSGDATA*  msg;
  STRBUF  *strbuf;
  int     pos_x;

  msg = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_ev_bgm_list_dat, evw->heapID );
  strbuf = GFL_MSG_CreateString( msg, EV_BGM_000 + evw->bgm_no );
  GFL_FONTSYS_SetColor( LETTER_COL_NORMAL, SHADOW_COL, BACK_COL );
  pos_x = 24;
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), pos_x, EV_BGMNO_Y, strbuf, evw->font );

  GFL_STR_DeleteBuffer( strbuf );
  GFL_MSG_Delete( msg );
}

//======================================================================
//  メニューリスト初期化
//======================================================================
static  void  EffectViewerInitMenuList( EFFECT_VIEWER_WORK *evw, int menu_list )
{
  const MENU_SCREEN_PARAM *msp_p = msp[ menu_list ];
  evw->menu_list = menu_list;
  evw->edit_param = NO_EDIT;
  evw->cursor_pos = NO_EDIT;
  if( msp_p->count ){
    if( evw->param ){
      GFL_HEAP_FreeMemory( evw->param );
    }
    evw->param = GFL_HEAP_AllocClearMemory( evw->heapID, sizeof( u32 ) * msp_p->count );
  }
  if( msp_p->mlp ){
    evw->cursor_pos = 0;
  }
  evw->draw_req = DRAW_REQ_MENU_LIST;
}

//======================================================================
//  テキスト表示
//======================================================================
static  void  EffectViewerDrawMenuList( EFFECT_VIEWER_WORK *evw )
{
  if( evw->draw_req ){
    GFL_BMP_Clear( GFL_BMPWIN_GetBmp( evw->bmpwin ), 0 );

    switch( evw->draw_req ){
    case DRAW_REQ_MENU_LIST:
      EffectViewerDrawCursor( evw );
      EffectViewerDrawMenuLabel( evw );
      EffectViewerDrawMenuData( evw );
      break;
    case DRAW_REQ_WAZA_NO:
      EffectViewerDrawWazaNo( evw );
      break;
    case DRAW_REQ_STATUS_EFFECT:
      EffectViewerDrawStatusEffect( evw );
      break;
    case DRAW_REQ_BGM_NO:
      EffectViewerDrawBgmNo( evw );
      break;
    }

    GFL_BMPWIN_TransVramCharacter( evw->bmpwin );
    GFL_BMPWIN_MakeScreen( evw->bmpwin );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME0_S );

    evw->draw_req_old = evw->draw_req;
    evw->draw_req = DRAW_REQ_NONE;
  }
}

//======================================================================
//  テキスト表示
//======================================================================
static  void  EffectViewerDrawCursor( EFFECT_VIEWER_WORK *evw )
{
  STRBUF  *strbuf;
  const MENU_SCREEN_PARAM *msp_p = msp[ evw->menu_list ];

  GFL_FONTSYS_SetColor( LETTER_COL_NORMAL, SHADOW_COL, BACK_COL );

  if( evw->cursor_pos != NO_EDIT ){
    strbuf = GFL_MSG_CreateString( evw->msg,  EVMSG_CURSOR );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ),
            msp_p->mlp[ evw->cursor_pos ].cursor_pos_x,
            msp_p->mlp[ evw->cursor_pos ].cursor_pos_y,
            strbuf, evw->font );
    GFL_STR_DeleteBuffer( strbuf );
  }
}

//======================================================================
//  テキスト表示
//======================================================================
static  void  EffectViewerDrawMenuLabel( EFFECT_VIEWER_WORK *evw )
{
  int i;
  STRBUF  *strbuf;
  const MENU_SCREEN_PARAM *msp_p = msp[ evw->menu_list ];
  const MENU_SCREEN_DATA *msd_p;
  int ofs_y = 56;

  GFL_FONTSYS_SetColor( LETTER_COL_NORMAL, SHADOW_COL, BACK_COL );

  for( i = 0 ; i < msp_p->count ; i++ ){
    msd_p = msp_p->msd[ i ];
    strbuf = GFL_MSG_CreateString( evw->msg,  msd_p->strID );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), msd_p->label_x, msd_p->label_y + ofs_y, strbuf, evw->font );
    GFL_STR_DeleteBuffer( strbuf );
  }
  if( evw->menu_list == MENULIST_TITLE )
  { 
    STRBUF  *str_src;
    STRBUF  *str_dst = GFL_STR_CreateBuffer( 100, evw->heapID );
    WORDSET*  mons_info = WORDSET_Create( evw->heapID );

    WORDSET_RegisterNumber( mons_info, 0, evw->mons_no, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_HANKAKU );
    if( evw->mons_no <= MONSNO_END )
    {
      WORDSET_RegisterPokeMonsNameNo( mons_info, 1, evw->mons_no );
    }
    str_src = GFL_MSG_CreateString( evw->msg,  PVMSG_MONSNAME );
    WORDSET_ExpandStr( mons_info, str_dst, str_src );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), TITLE_LABEL_X, TITLE_LABEL_Y + 24 + ofs_y, str_dst, evw->font );

    GFL_HEAP_FreeMemory( str_src );
    GFL_HEAP_FreeMemory( str_dst );
    WORDSET_Delete( mons_info );
  }
  draw_dir_touch_screen( evw );
}

//======================================================================
//  テキスト表示
//======================================================================
static  void  EffectViewerDrawMenuData( EFFECT_VIEWER_WORK *evw )
{
  int param;
  const MENU_SCREEN_PARAM *msp_p = msp[ evw->menu_list ];
  const MENU_SCREEN_DATA *msd_p;

  for( param = 0 ; param < msp_p->count ; param++ ){
    msd_p = msp_p->msd[ param ];
    switch( msd_p->edit_type ){
    default:
    case EDIT_NONE:
      break;
    case EDIT_INT:
    case EDIT_FX32:
      EffectViewerDrawMenuDataNum( evw, msd_p, param );
      break;
    case EDIT_COMBOBOX:
      EffectViewerDrawMenuDataComboBox( evw, msd_p, param );
      break;
    }
  }
}

//======================================================================
//  テキスト表示
//======================================================================
static  void  EffectViewerDrawMenuDataNum( EFFECT_VIEWER_WORK *evw, const MENU_SCREEN_DATA *msd_p, int param )
{
  STRBUF  *strbuf;
  int   keta, ofsx = 0;
  u32   num;

  for( keta = 7 ; keta >= 0 ; keta-- ){
    if( param == evw->edit_param ){
      if( keta == evw->cursor_keta ){
        GFL_FONTSYS_SetColor( LETTER_COL_CURSOR, SHADOW_COL, BACK_COL );
      }
      else{
        GFL_FONTSYS_SetColor( LETTER_COL_SELECT, SHADOW_COL, BACK_COL );
      }
    }
    else{
      GFL_FONTSYS_SetColor( LETTER_COL_NORMAL, SHADOW_COL, BACK_COL );
    }
    if( ( keta == 2 ) && ( msd_p->edit_type == EDIT_FX32 ) ){
      strbuf = GFL_MSG_CreateString( evw->msg,  EVMSG_NUMDOT );
      PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), msd_p->data_x + ofsx, msd_p->data_y, strbuf, evw->font );
      GFL_STR_DeleteBuffer( strbuf );
      ofsx += 8;
    }
    num = ( evw->param[ param ] & ( 0x0000000f << ( keta * 4 ) ) ) >> ( keta * 4 );
    strbuf = GFL_MSG_CreateString( evw->msg,  EVMSG_NUM0 + num );
    PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), msd_p->data_x + ofsx, msd_p->data_y, strbuf, evw->font );
    GFL_STR_DeleteBuffer( strbuf );
    ofsx += 8;
  }
}

//======================================================================
//  テキスト表示
//======================================================================
static  void  EffectViewerDrawMenuDataComboBox( EFFECT_VIEWER_WORK *evw, const MENU_SCREEN_DATA *msd_p, int param )
{
  STRBUF  *strbuf;

  if( param == evw->edit_param ){
    GFL_FONTSYS_SetColor( LETTER_COL_SELECT, SHADOW_COL, BACK_COL );
  }
  else{
    GFL_FONTSYS_SetColor( LETTER_COL_NORMAL, SHADOW_COL, BACK_COL );
  }

  strbuf = GFL_MSG_CreateString( evw->msg,  msd_p->edit_min + evw->param[ param ] );
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), msd_p->data_x, msd_p->data_y, strbuf, evw->font );
  GFL_STR_DeleteBuffer( strbuf );
}

//======================================================================
//  データエディット
//======================================================================
static  void  EffectViewerEditMenuList( EFFECT_VIEWER_WORK *evw )
{
  int tp = GFL_UI_TP_GetCont();

  if( evw->edit_param == NO_EDIT ){
    if( tp ){
      MoveCamera( evw );
    }
    else{
      EffectViewerMoveAction( evw );
    }
  }
  else{
    EffectViewerEditAction( evw );
  }
}

//======================================================================
//  データエディット
//======================================================================
static  void  EffectViewerMoveAction( EFFECT_VIEWER_WORK *evw )
{
  int trg = GFL_UI_KEY_GetTrg();
  const MENU_SCREEN_PARAM *msp_p = msp[ evw->menu_list ];
  const MENU_SCREEN_DATA *msd_p = msp_p->msd[ evw->cursor_pos ];

  if( ( trg == PAD_KEY_UP ) && ( msp_p->mlp[ evw->cursor_pos ].move_up != NO_MOVE ) ){
    evw->cursor_pos = msp_p->mlp[ evw->cursor_pos ].move_up;
    evw->draw_req = DRAW_REQ_MENU_LIST;
  }
  else if( ( trg == PAD_KEY_DOWN ) && ( msp_p->mlp[ evw->cursor_pos ].move_down != NO_MOVE ) ){
    evw->cursor_pos = msp_p->mlp[ evw->cursor_pos ].move_down;
    evw->draw_req = DRAW_REQ_MENU_LIST;
  }
  else if( ( trg == PAD_KEY_LEFT ) && ( msp_p->mlp[ evw->cursor_pos ].move_left != NO_MOVE ) ){
    evw->cursor_pos = msp_p->mlp[ evw->cursor_pos ].move_left;
    evw->draw_req = DRAW_REQ_MENU_LIST;
  }
  else if( ( trg == PAD_KEY_RIGHT ) && ( msp_p->mlp[ evw->cursor_pos ].move_right != NO_MOVE ) ){
    evw->cursor_pos = msp_p->mlp[ evw->cursor_pos ].move_right;
    evw->draw_req = DRAW_REQ_MENU_LIST;
  }
  else if ( trg == PAD_BUTTON_A ){
    evw->edit_param = evw->cursor_pos;
    evw->draw_req = DRAW_REQ_MENU_LIST;
    switch( msd_p->edit_type ){
    default:
    case EDIT_NONE:
    case EDIT_COMBOBOX:
      break;
    case EDIT_DECIDE:
    case EDIT_CANCEL:
      evw->answer = msd_p->edit_type;
      break;
    case EDIT_INT:
      evw->cursor_keta = 0;
      break;
    case EDIT_FX32:
      evw->cursor_keta = 3;
      break;
    }
  }
}

//======================================================================
//  データエディット
//======================================================================
static  void  EffectViewerEditAction( EFFECT_VIEWER_WORK *evw )
{
  int trg = GFL_UI_KEY_GetTrg();
  const MENU_SCREEN_PARAM *msp_p = msp[ evw->menu_list ];
  const MENU_SCREEN_DATA *msd_p = msp_p->msd[ evw->edit_param ];

  switch( msd_p->edit_type ){
  default:
  case EDIT_NONE:
    break;
  case EDIT_INT:
  case EDIT_FX32:
    if( trg == PAD_KEY_UP ){
      evw->param[ evw->edit_param ] += 1 << ( 4 * evw->cursor_keta );
      evw->draw_req = DRAW_REQ_MENU_LIST;
    }
    if( trg == PAD_KEY_DOWN ){
      evw->param[ evw->edit_param ] -= 1 << ( 4 * evw->cursor_keta );
      evw->draw_req = DRAW_REQ_MENU_LIST;
    }
    if( ( trg == PAD_KEY_LEFT ) && ( evw->cursor_keta < 7 ) ){
      evw->cursor_keta++;
      evw->draw_req = DRAW_REQ_MENU_LIST;
    }
    if( ( trg == PAD_KEY_RIGHT ) && ( evw->cursor_keta > 0 ) ){
      evw->cursor_keta--;
      evw->draw_req = DRAW_REQ_MENU_LIST;
    }
    break;
  case EDIT_COMBOBOX:
    if( ( trg == PAD_KEY_UP ) && ( evw->param[ evw->edit_param ] < ( msd_p->edit_max - msd_p->edit_min ) ) ){
      evw->param[ evw->edit_param ]++;
      evw->draw_req = DRAW_REQ_MENU_LIST;
    }
    if( ( trg == PAD_KEY_DOWN ) && ( evw->param[ evw->edit_param ] > 0 ) ){
      evw->param[ evw->edit_param ]--;
      evw->draw_req = DRAW_REQ_MENU_LIST;
    }
    break;
  }
  if( trg == PAD_BUTTON_A ){
    evw->edit_param = NO_EDIT;
    evw->draw_req = DRAW_REQ_MENU_LIST;
  }
}

//======================================================================
//  パーティクルリソースロード
//======================================================================
static  void  EffectViewerParticleResourceLoad( EFFECT_VIEWER_WORK *evw )
{
  int         ofs;
  void        *heap;
  void        *resource;
  DEBUG_PARTICLE_DATA *dpd = (DEBUG_PARTICLE_DATA *)evw->resource_data;

  heap = GFL_HEAP_AllocMemory( evw->heapID, PARTICLE_LIB_HEAP_SIZE );
  evw->ptc = GFL_PTC_Create( heap, PARTICLE_LIB_HEAP_SIZE, FALSE, evw->heapID );
  ofs = dpd->adrs[ 0 ];
  resource = (void *)&dpd->adrs[ ofs ];
  GFL_PTC_SetResourceEx( evw->ptc, resource, FALSE, GFUser_VIntr_GetTCBSYS() );
}

//======================================================================
//  パーティクル再生
//======================================================================
static  void  EffectViewerParticlePlay( EFFECT_VIEWER_WORK *evw )
{
  if( ( GFL_PTC_GetEmitterNum( evw->ptc ) ) && ( evw->draw_req == DRAW_REQ_MENU_LIST ) ){
    GFL_PTC_DeleteEmitterAll( evw->ptc );
  }
  if( GFL_PTC_GetEmitterNum( evw->ptc ) == 0 ){
    BTLV_EFFVM_DebugParticlePlay( BTLV_EFFECT_GetVMHandle(), evw->ptc, 0,
                    evw->param[ MLP_PPE_ATTACK ],
                    evw->param[ MLP_PPE_DEFENCE ],
                    evw->param[ MLP_PPE_OFSY ],
                    evw->param[ MLP_PPE_ANGLE ] );
  }
}

//======================================================================
//  パーティクル再生後始末
//======================================================================
static  void  EffectViewerParticleEnd( EFFECT_VIEWER_WORK *evw )
{
  if( evw->ptc ){
    void  *heap;

    heap = GFL_PTC_GetHeapPtr( evw->ptc );
    GFL_HEAP_FreeMemory( heap );
    GFL_PTC_Delete( evw->ptc );
    evw->ptc = NULL;
  }
  BTLV_MCSS_SetOrthoMode( BTLV_EFFECT_GetMcssWork() );
}

//======================================================================
//  MCS転送するデータを生成
//======================================================================
static  u32 *EffectViewerMakeSendData( EFFECT_VIEWER_WORK *evw, int param_start, int param_count )
{
  int i;
  u32 *buf = GFL_HEAP_AllocMemory( evw->heapID, sizeof( u32 ) * ( param_count + 1 ) );

  if( evw->answer == EDIT_DECIDE ){
    buf[ 0 ] = SEND_DECIDE;
  }
  else{
    buf[ 0 ] = SEND_CANCEL;
  }
  for( i = 0 ; i < param_count ; i++ ){
    buf[ i + 1 ] = evw->param[ param_start + i ];
  }
  return buf;
}

//======================================================================
//  カメラの制御
//======================================================================
static  void  MoveCamera( EFFECT_VIEWER_WORK *evw )
{
  int pad = GFL_UI_KEY_GetCont();
  VecFx32 pos,ofsx,ofsz,camPos, camTarget;
  fx32  xofs=0,yofs=0,zofs=0;

  if( pad & PAD_BUTTON_L ){
    if( pad & PAD_KEY_LEFT ){
      BTLV_CAMERA_MoveCameraAngle( BTLV_EFFECT_GetCameraWork(), 0, -ROTATE_SPEED );
    }
    if( pad & PAD_KEY_RIGHT ){
      BTLV_CAMERA_MoveCameraAngle( BTLV_EFFECT_GetCameraWork(), 0,  ROTATE_SPEED );
    }
    if( pad & PAD_KEY_UP ){
      BTLV_CAMERA_MoveCameraAngle( BTLV_EFFECT_GetCameraWork(), -ROTATE_SPEED, 0 );
    }
    if( pad & PAD_KEY_DOWN ){
      BTLV_CAMERA_MoveCameraAngle( BTLV_EFFECT_GetCameraWork(),  ROTATE_SPEED, 0 );
    }
  }
  else{
    BTLV_CAMERA_GetCameraPosition( BTLV_EFFECT_GetCameraWork(), &camPos, &camTarget );

    if( pad & PAD_KEY_LEFT ){
      xofs = -MOVE_SPEED;
    }
    if( pad & PAD_KEY_RIGHT ){
      xofs = MOVE_SPEED;
    }

    if( pad & PAD_BUTTON_R ){
      if( pad & PAD_KEY_UP ){
        zofs = MOVE_SPEED;
      }
      if( pad & PAD_KEY_DOWN ){
        zofs = -MOVE_SPEED;
      }
    }
    else{
      if( pad & PAD_KEY_UP ){
        yofs = MOVE_SPEED;
      }
      if( pad & PAD_KEY_DOWN ){
        yofs = -MOVE_SPEED;
      }
    }
    pos.x = camPos.x - camTarget.x;
    pos.y = 0;
    pos.z = camPos.z - camTarget.z;
    VEC_Normalize( &pos, &pos );

    ofsx.x = FX_MUL( pos.z, xofs );
    ofsx.y = 0;
    ofsx.z = -FX_MUL( pos.x, xofs );

    ofsz.x = -FX_MUL( pos.x, zofs );
    ofsz.y = yofs;
    ofsz.z = -FX_MUL( pos.z, zofs );

    camPos.x += ofsx.x + ofsz.x;
    camPos.y += ofsx.y + ofsz.y;
    camPos.z += ofsx.z + ofsz.z;
    camTarget.x += ofsx.x + ofsz.x;
    camTarget.y += ofsx.y + ofsz.y;
    camTarget.z += ofsx.z + ofsz.z;

    BTLV_CAMERA_MoveCameraPosition( BTLV_EFFECT_GetCameraWork(), &camPos, &camTarget );
/*
    OS_TPrintf("pos_x:%08x pos_y:%08x pos_z:%08x tar_x:%08x tar_y:%08x tar_z:%08x\n",
      camPos.x,
      camPos.y,
      camPos.z,
      camTarget.x,
      camTarget.y,
      camTarget.z );
*/
  }
  if( pad & PAD_BUTTON_DEBUG ){
    BTLV_CAMERA_GetDefaultCameraPosition( &camPos, &camTarget );
    BTLV_CAMERA_MoveCameraInterpolation( BTLV_EFFECT_GetCameraWork(), &camPos, &camTarget, 20, 0, 20 );
  }

}

static  void  set_pokemon( EFFECT_VIEWER_WORK *evw )
{
  if( evw->viewer_mode == FALSE )
  { 
    //POKEMON_PARAM生成
    POKEMON_PARAM *pp = GFL_HEAP_AllocMemory( evw->heapID, POKETOOL_GetWorkSize() );
    PP_SetupEx( pp, evw->mons_no, 0, 0x10, 0, 255 );
    { 
      BtlvMcssPos pos, start, end;

      switch( evw->rule ){ 
      case BTL_RULE_SINGLE:
        start = BTLV_MCSS_POS_AA;
        end   = BTLV_MCSS_POS_BB;
        break;
      case BTL_RULE_DOUBLE:
        start = BTLV_MCSS_POS_A;
        end   = BTLV_MCSS_POS_D;
        break;
      case BTL_RULE_TRIPLE:
      case BTL_RULE_ROTATION:
        start = BTLV_MCSS_POS_A;
        end   = BTLV_MCSS_POS_F;
        break;
      }
      for( pos = start ; pos <= end ; pos++ )
      { 
        BTLV_EFFECT_SetPokemon( pp, pos );
        if( evw->gauge_mode )
        { 
          BTLV_EFFECT_SetGaugePP( evw->zs, pp, pos );
        }
      }
      GFL_HEAP_FreeMemory( pp );
    }
  }
  else
  { 
    BTLV_EFFECT_SetTrainer( evw->tr_type, BTLV_MCSS_POS_TR_BB, 0, 0, 0 );
  }
}

static  void  del_pokemon( EFFECT_VIEWER_WORK *evw )
{
  BtlvMcssPos pos;
  
  for( pos = BTLV_MCSS_POS_AA ; pos < BTLV_MCSS_POS_MAX ; pos++ )
  { 
    if( BTLV_EFFECT_CheckExist( pos ) == TRUE )
    { 
      BTLV_EFFECT_DelPokemon( pos );
    }
    if( BTLV_EFFECT_CheckExistGauge( pos ) == TRUE )
    { 
      BTLV_EFFECT_DelGauge( pos );
    }
  }
  if( BTLV_EFFECT_CheckExist( BTLV_MCSS_POS_TR_BB ) == TRUE )
  { 
    BTLV_EFFECT_DelTrainer( BTLV_MCSS_POS_TR_BB );
  }
}

static  int ev_pow( int x, int y )
{
  int i;
  int ans = 1;

  for( i = 0 ; i < y ; i++ )
  {
    ans *= x;
  }

  return ans;
}

static  int   ev_param_get( EFFECT_VIEWER_WORK* evw, int param )
{
  int result = 0;
  u8 a,b,c,d;

  a = evw->param_start[ param * 4 + 0 ];
  b = evw->param_start[ param * 4 + 1 ];
  c = evw->param_start[ param * 4 + 2 ];
  d = evw->param_start[ param * 4 + 3 ];

  result += (u32)d;
  result <<= 8;
  result += (u32)c;
  result <<= 8;
  result += (u32)b;
  result <<= 8;
  result += (u32)a;

  return result;
}

static  void  pinch_bgm_check( EFFECT_VIEWER_WORK* evw )
{ 
  if( evw->bgm_fade_flag )
  {
    if( PMSND_CheckFadeOnBGM() == FALSE )
    {
      evw->bgm_fade_flag = 0;
      if( evw->pinch_bgm_flag )
      {
        PMSND_PopBGM();
        PMSND_PauseBGM( FALSE );
        PMSND_FadeInBGM( 24 );
      }
      else
      {
        PMSND_PauseBGM( TRUE );
        PMSND_PushBGM();
        PMSND_PlayBGM( SEQ_BGM_BATTLEPINCH );
      }
      evw->pinch_bgm_flag ^= 1;
    }
  }
}

static  BOOL  check_touch_panel( EFFECT_VIEWER_WORK* evw, EFFECT_TYPE type )
{
  int hit = GFL_UI_TP_HitTrg( DirTouchHitTbl[ evw->rule ] );

  if( ( hit != 0 ) && ( hit != GFL_UI_TP_HIT_NONE ) )
  {
    int hit_pos = hit - 1;

    switch( type ){ 
    case EFFECT_TYPE_MCS:
      if( ( evw->sequence_data != NULL ) && ( evw->resource_data != NULL ) )
      { 
        BTLV_EFFVM_StartDebug( BTLV_EFFECT_GetVMHandle(),
                               DirAttack[ evw->rule ][ hit_pos ], DirDefence[ evw->rule ][ hit_pos ],
                               evw->sequence_data, evw->resource_data, NULL, evw->viewer_mode );
      }
      break;
    case EFFECT_TYPE_WAZA:
      { 
        BTLV_EFFVM_PARAM  param;
        param.waza_range = 0;                 ///<技の効果範囲
        param.turn_count = evw->turn_count;   ///< ターンによって異なるエフェクトを出す場合のターン指定。（ex.そらをとぶ）
        param.continue_count = 0;             ///< 連続して出すとエフェクトが異なる場合の連続カウンタ（ex. ころがる）
        param.yure_cnt = 0;                   ///<ボールゆれるカウント
        param.get_success = FALSE;            ///<捕獲成功かどうか
        param.item_no = 0;                    ///<ボールのアイテムナンバー
        BTLV_EFFVM_Start( BTLV_EFFECT_GetVMHandle(), DirAttack[ evw->rule ][ hit_pos ], DirDefence[ evw->rule ][ hit_pos ],
                          evw->waza_no, &param );
      }
      break;
    case EFFECT_TYPE_STATUS:
      BTLV_EFFVM_StartThrough( BTLV_EFFECT_GetVMHandle(),
                               DirAttack[ evw->rule ][ hit_pos ], DirDefence[ evw->rule ][ hit_pos ], evw->waza_no, NULL );
      break;
    }
    evw->ret_seq_no = evw->seq_no;
    evw->seq_no = SEQ_EFFECT_WAIT;
  }

  return ( hit == 0 );
}

static  void  draw_dir_touch_screen( EFFECT_VIEWER_WORK* evw )
{ 
  int pos_x = 24;
  int pos_y = 8;
  switch( evw->rule ){ 
  case BTL_RULE_SINGLE:
    { 
      int x;
      for( x = 0 ; x < 2 ; x++ )
      { 
        STRBUF* str_src = GFL_MSG_CreateString( evw->msg, EVMSG_AA2BB + x );
        PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), pos_x + 48 * x, pos_y, str_src, evw->font );
        GFL_HEAP_FreeMemory( str_src );
      }
    }
    break;
  case BTL_RULE_DOUBLE:
  case BTL_RULE_TRIPLE:
    { 
      int x, y;
      int x_max = ( evw->rule == BTL_RULE_DOUBLE ) ? 3 : 5;
      int y_max = ( evw->rule == BTL_RULE_DOUBLE ) ? 4 : 6;
      for( y = 0 ; y < y_max ; y++ )
      { 
        for( x = 0 ; x < x_max ; x++ )
        { 
          STRBUF* str_src = GFL_MSG_CreateString( evw->msg, ( EVMSG_A2B + 5 * y ) + x );
          PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), pos_x + 48 * x, pos_y + 24 * y, str_src, evw->font );
          GFL_HEAP_FreeMemory( str_src );
        }
      }
    }
    break;
  case BTL_RULE_ROTATION:
    { 
      int x;
      for( x = 0 ; x < 2 ; x++ )
      { 
        STRBUF* str_src = GFL_MSG_CreateString( evw->msg, EVMSG_A2B + 5 * x );
        PRINTSYS_Print( GFL_BMPWIN_GetBmp( evw->bmpwin ), pos_x + 48 * x, pos_y, str_src, evw->font );
        GFL_HEAP_FreeMemory( str_src );
      }
    }
    break;
  }
}

#endif
