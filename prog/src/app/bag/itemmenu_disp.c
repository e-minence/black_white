//============================================================================================
/**
 * @file    itemmenu_disp.c
 * @brief   アイテムメニューの描画周り
 * @author  k.ohno
 * @author  genya hosaka (引継)
 * @date    2009.07.19
 */
//============================================================================================
#include <gflib.h>
#include "net/network_define.h"
#include "arc_def.h"

#include "item/item.h"

#include "app_menu_common.naix"
#include "app/app_menu_common.h"
#include "app/itemuse.h"

#include "gamesystem/gamesystem.h"
#include "gamesystem/game_init.h"
#include "gamesystem/game_event.h"
#include "gamesystem/game_data.h"
#include "gamesystem/msgspeed.h"  //MSGSPEED_GetWait
#include "savedata/mystatus.h"

#include "system/shortcut_data.h"

#include "system/bmp_winframe.h"
#include "system/wipe.h"
#include "system/main.h"      //GFL_HEAPID_APP参照

#include "msg/msg_bag.h"

#include "print/printsys.h"
#include "print/wordset.h"

#include "field/fieldmap.h"
#include "font/font.naix" //NARC_font_large_gftr

#include "waza_tool/wazadata.h"
#include "waza_tool/wazano_def.h"

#include "itemmenu.h"
#include "itemmenu_local.h"

#include "sound/pm_sndsys.h"

#include "bag.naix"
#include "item_icon.naix"
#include "message.naix"

#include "msg/msg_itempocket.h"
#include "bag_parts_d_NANR_LBLDEFS.h"

#include "item/itemtype_def.h"

#include "system/scroll_bar.h"


#if 0
#ifdef PM_DEBUG
// 速度測定
static OSTick _debug_tick = 0;
const char* _debug_chara;
static void _DebugTickCheck_Start( const char* chara )
{
  GF_ASSERT(_debug_tick == 0);
  _debug_tick = OS_GetTick();
  _debug_chara = chara;
}
static void _DebugTickCheck_End( void )
{
  OSTick t = OS_GetTick() - _debug_tick;
  u64 s = OS_TicksToSeconds(t);
  u64 mis = OS_TicksToMilliSeconds(t) % 1000;
  u64 mcs = OS_TicksToMicroSeconds(t) % 1000000;
  
  OS_TPrintf( _debug_chara );
  OS_TPrintf( " >> %d second : %d milli second : %d micro second  \n" ,
      s,
      mis,
      mcs );

  _debug_tick = 0;
}
#endif // PM_DEBUG
#endif

//------------------------------------------------------------------
//------------------------------------------------------------------

enum
{
  // パレットオフセット
  PALOFS_NUM_FRAME = 3,   ///< 数値入力フレーム
};


#define ITEMWIN_FRAME (GFL_BG_FRAME1_S)
#define ITEMREPORT_FRAME (GFL_BG_FRAME2_S)

#define _UP_ITEMNAME_INITX (7)
#define _UP_ITEMNAME_INITY (5)
#define _UP_ITEMNAME_SIZEX (18)
#define _UP_ITEMNAME_SIZEY (3)
#define _UP_ITEMNAME_DOTOFS_Y (4)

#define _UP_ITEMNUM_INITX (20)
#define _UP_ITEMNUM_INITY (8)
#define _UP_ITEMNUM_SIZEX (6)
#define _UP_ITEMNUM_SIZEY (3)

#define _UP_ITEMNUM_DOTOFS_Y (4)

#define _UP_ITEMREPORT_INITX (2)
#define _UP_ITEMREPORT_INITY (12)
#define _UP_ITEMREPORT_SIZEX (28)
#define _UP_ITEMREPORT_SIZEY (7)

#define _ITEMICON_SCR_X (14)
#define _ITEMICON_SCR_Y (8)

#define ITEM_LIST_NUM (8)

typedef enum{
  _CLACT_PLT,
  _CLACT_CHR,
  _CLACT_ANM,
} _BAG_CLACT_TYPE;


static void _itemiconDelete( FIELD_ITEMMENU_WORK * pWork );
static void _itemiconAnim(FIELD_ITEMMENU_WORK* pWork,int itemid);
static void ITEMDISP_InitTaskBar( FIELD_ITEMMENU_WORK* pWork );

static void PrintStr( FIELD_ITEMMENU_WORK * wk, PRINT_UTIL * util, const STRBUF * str, u16 x, u16 y, PRINTSYS_LSB color );
static void PrintScreenTrans( PRINT_UTIL * util );


//------------------------------------------------------------------------------
/**
 * @brief   絵の初期化
 * @retval  none
 */
//------------------------------------------------------------------------------

void _createSubBg(void)
{
  GX_SetDispSelect(GX_DISP_SELECT_SUB_MAIN);

  // 背景面
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000,
      0x8000,
      GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME0_M );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME0_M );
    GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_OFF );

  }
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000,
      0x8000,
      GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME1_M );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME1_M );
    GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_OFF );

  }
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000,
      0x8000,
      GX_BG_EXTPLTT_01,
      1, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_ClearFrame( GFL_BG_FRAME2_M );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
    GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_OFF );

  }
  {
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x18000,
      0x8000,
      GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_FillCharacter( GFL_BG_FRAME3_M, 0x00, 1, 0 );
    GFL_BG_ClearFrame( GFL_BG_FRAME3_M );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME3_M );
    GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_OFF );

  }


  {
    int frame = GFL_BG_FRAME0_S;
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,GX_BG_EXTPLTT_01,
      3, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( frame, VISIBLE_OFF );
    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GFL_BG_FRAME1_S;  // ITEMWIN_FRAME
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x8000,GX_BG_EXTPLTT_01,
      2, 0, 0, FALSE
      };

    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( frame, VISIBLE_OFF );
//    GFL_BG_FillCharacter( frame, 0x00, 1, 0 );
//    GFL_BG_FillScreen( frame, 0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
//    GFL_BG_LoadScreenReq( frame );
  }
  {
    int frame = GFL_BG_FRAME2_S; // ITEMREPORT_FRAME
    GFL_BG_BGCNT_HEADER TextBgCntDat = {
      0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
      GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000,GX_BG_EXTPLTT_01,
      0, 0, 0, FALSE
      };
    GFL_BG_SetBGControl(
      frame, &TextBgCntDat, GFL_BG_MODE_TEXT );
    GFL_BG_SetVisible( frame, VISIBLE_OFF );
		GFL_BG_FillCharacter( frame, 0, 1, 0 );
		GFL_BG_ClearScreen( frame );
//    GFL_BG_LoadScreenReq( frame );
  }
}

#define FIELD_CLSYS_RESOUCE_MAX   (100)


//--------------------------------------------------------------
/// セルアクター　初期化データ
//--------------------------------------------------------------
static const GFL_CLSYS_INIT fldmapdata_CLSYS_Init =
{
  0, 0,
  0, 512,
  GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL,
  GFL_CLSYS_OAMMAN_INTERVAL, 128-GFL_CLSYS_OAMMAN_INTERVAL, //通信アイコン部分
  0,
  FIELD_CLSYS_RESOUCE_MAX,
  FIELD_CLSYS_RESOUCE_MAX,
  FIELD_CLSYS_RESOUCE_MAX,
  FIELD_CLSYS_RESOUCE_MAX,
  16, 16,
};

static GFL_DISP_VRAM _defVBTbl = {
  GX_VRAM_BG_128_A,							// メイン2DエンジンのBG
  GX_VRAM_BGEXTPLTT_NONE,				// メイン2DエンジンのBG拡張パレット
  GX_VRAM_SUB_BG_128_C,				  // サブ2DエンジンのBG
  GX_VRAM_SUB_BGEXTPLTT_NONE,   // サブ2DエンジンのBG拡張パレット
  GX_VRAM_OBJ_128_B,					  // メイン2DエンジンのOBJ
  GX_VRAM_OBJEXTPLTT_NONE,		  // メイン2DエンジンのOBJ拡張パレット
  GX_VRAM_SUB_OBJ_128_D,		    // サブ2DエンジンのOBJ
  GX_VRAM_SUB_OBJEXTPLTT_NONE,  // サブ2DエンジンのOBJ拡張パレット
  GX_VRAM_TEX_NONE,						  // テクスチャイメージスロット
  GX_VRAM_TEXPLTT_NONE,					// テクスチャパレットスロット
  GX_OBJVRAMMODE_CHAR_1D_128K,
  GX_OBJVRAMMODE_CHAR_1D_128K,
};




void ITEMDISP_SetVisible(void)
{
  GFL_BG_SetVisible( GFL_BG_FRAME0_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME1_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME2_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME3_M, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_ON );
  GFL_BG_SetVisible( ITEMWIN_FRAME, VISIBLE_ON );
  GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_ON );
}


//-----------------------------------------------------------------------------
/**
 *  @brief  下画面BGリソース読み込み（男女でリソースきりわけ）
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *  @param  p_handle
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _load_basebg_d( FIELD_ITEMMENU_WORK* pWork, ARCHANDLE* p_handle )
{
  u32 nclr;
  u32 ncgr;
  u32 nscr;
  u32 sex = MyStatus_GetMySex( pWork->mystatus );

  if( sex == PTL_SEX_MALE )
  {
    // 男
    nclr = NARC_bag_bag_basebg_man_d_NCLR;
    ncgr = NARC_bag_bag_basebg_man_d_NCGR;
    nscr = NARC_bag_bag_basebg_man_d_NSCR;
  }
  else if( sex == PTL_SEX_FEMALE )
  {
    // 女
    nclr = NARC_bag_bag_basebg_d_NCLR;
    ncgr = NARC_bag_bag_basebg_d_NCGR;
    nscr = NARC_bag_bag_basebg_d_NSCR;
  }
  else
  {
    GF_ASSERT(0);
  }

  GFL_ARCHDL_UTIL_TransVramPalette( p_handle, nclr, PALTYPE_MAIN_BG, 0, 0x20,  pWork->heapID );

  pWork->mainbg = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, ncgr, GFL_BG_FRAME0_M, 0, 0, pWork->heapID );

  GFL_ARCHDL_UTIL_TransVramScreenCharOfs( p_handle, nscr, GFL_BG_FRAME0_M, 0, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->mainbg), 0, 0, pWork->heapID );
}


//-----------------------------------------------------------------------------
/**
 *  @brief  下画面アイコンリソース読み込み（男女でリソースきりわけ）
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *  @param  p_handle
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _load_parts( FIELD_ITEMMENU_WORK* pWork, ARCHANDLE* p_handle )
{
  u32 parts_clr;
  u32 parts_cgx;
  u32 parts_cer;
  u32 parts_anm;
  u32 sex = MyStatus_GetMySex( pWork->mystatus );

  if( sex == PTL_SEX_MALE )
  {
    // 男
    parts_clr = NARC_bag_bag_parts_man_d_NCLR;
    parts_cgx = NARC_bag_bag_parts_man_d_NCGR;
    parts_cer = NARC_bag_bag_parts_man_d_NCER;
    parts_anm = NARC_bag_bag_parts_man_d_NANR;
  }
  else if( sex == PTL_SEX_FEMALE )
  {
    // 女
    parts_clr = NARC_bag_bag_parts_d_NCLR;
    parts_cgx = NARC_bag_bag_parts_d_NCGR;
    parts_cer = NARC_bag_bag_parts_d_NCER;
    parts_anm = NARC_bag_bag_parts_d_NANR;
  }
  else
  {
    GF_ASSERT(0);
  }

  pWork->cellRes[_PLT_BAGPOCKET] = GFL_CLGRP_PLTT_RegisterEx( p_handle , parts_clr , CLSYS_DRAW_MAIN , _PAL_BAG_PARTS_CELL*0x20 , 0 , _PAL_BAG_PARTS_CELL_NUM , pWork->heapID );
  pWork->cellRes[_NCG_BAGPOCKET] = GFL_CLGRP_CGR_Register( p_handle , parts_cgx , FALSE , CLSYS_DRAW_MAIN , pWork->heapID );
  pWork->cellRes[_ANM_BAGPOCKET] = GFL_CLGRP_CELLANIM_Register( p_handle, parts_cer, parts_anm , pWork->heapID );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  グラフィック初期化
 *
 *  @param  pWork
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
void ITEMDISP_graphicInit(FIELD_ITEMMENU_WORK* pWork)
{
  G2_BlendNone();

  GFL_BG_Init(pWork->heapID);
  GFL_BMPWIN_Init(pWork->heapID);
  GFL_FONTSYS_Init();
  GFL_DISP_SetBank( &_defVBTbl );
  GFL_CLACT_SYS_Create( &fldmapdata_CLSYS_Init, &_defVBTbl, pWork->heapID );

  {
    GFL_BG_SYS_HEADER BGsys_data = {
      GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
    };
    GFL_BG_SetBGMode( &BGsys_data );
  }

  GFL_DISP_GX_SetVisibleControlDirect(0);   //全BG&OBJの表示OFF
  GFL_DISP_GXS_SetVisibleControlDirect(0);

  _createSubBg();

  GFL_FONTSYS_SetDefaultColor();

  pWork->bgchar = BmpWinFrame_GraphicSetAreaMan(GFL_BG_FRAME3_M,
                                                _BUTTON_WIN_PAL, MENU_TYPE_SYSTEM, pWork->heapID);

  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_BAG, pWork->heapID );

    // 上画面パレット一括転送
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_bag_bag_bg_u_NCLR, PALTYPE_SUB_BG, 0, 0, pWork->heapID );

    // 上画面パレット 男の場合一本目を上書き
    {
      u32 sex = MyStatus_GetMySex( pWork->mystatus );

      if( sex == PTL_SEX_MALE )
      {
        GFL_ARCHDL_UTIL_TransVramPaletteEx( p_handle, NARC_bag_bag_bg_u_NCLR, PALTYPE_SUB_BG, 0x20, 0, 0x20, pWork->heapID );
      }
    }

    pWork->subbg = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_bag_bag_basebg_u_NCGR,
                                                                GFL_BG_FRAME0_S, 0, 0, pWork->heapID);
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs( p_handle, NARC_bag_bag_basebg_u_NSCR,
                                            GFL_BG_FRAME0_S, 0,
                                            GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subbg), 0, 0, pWork->heapID);

    // 下画面BG（男女で切替)
    _load_basebg_d( pWork, p_handle );

    pWork->subbg2 = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, NARC_bag_bag_win01_02_u_NCGR,
                                                                 ITEMWIN_FRAME, 0, 0, pWork->heapID);

    GFL_ARCHDL_UTIL_TransVramScreenCharOfs( p_handle, NARC_bag_bag_win01_u_NSCR,
                                            ITEMWIN_FRAME, 0,
                                            GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subbg2), 0, 0, pWork->heapID);

    //下画面アイコン
    _load_parts( pWork, p_handle );

    //数字のフレーム
    pWork->numFrameBg =
      GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
        p_handle, NARC_bag_bag_win05_d_NCGR, GFL_BG_FRAME3_M, 0, 0, pWork->heapID);

    //数字フレーム用パレット
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_bag_bag_win05_d_NCLR,
                                      PALTYPE_MAIN_BG, PALOFS_NUM_FRAME*0x20, 1*0x20,  pWork->heapID);

    GFL_ARC_CloseDataHandle(p_handle);
  }

  GFL_ARC_UTIL_TransVramPalette(ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
                                0x20*_BUTTON_MSG_PAL, 0x20, pWork->heapID);

  pWork->cellUnit = GFL_CLACT_UNIT_Create( _CELLUNIT_NUM , 0 , pWork->heapID );

  GFL_DISP_GXS_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);
  GFL_DISP_GX_SetVisibleControl(GX_PLANEMASK_OBJ,VISIBLE_ON);

  {
    ARCHANDLE *archandle = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON , pWork->heapID );

    //下画面バー
    GFL_ARCHDL_UTIL_TransVramPalette( archandle , NARC_app_menu_common_menu_bar_NCLR ,
                                      PALTYPE_MAIN_BG , 8*32 , 32 , pWork->heapID );
    pWork->barbg = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( archandle , NARC_app_menu_common_menu_bar_NCGR ,
                                                                GFL_BG_FRAME1_M , 0 , 0, pWork->heapID );
    GFL_ARCHDL_UTIL_TransVramScreenCharOfs( archandle , NARC_app_menu_common_menu_bar_NSCR ,
                                            GFL_BG_FRAME1_M , 0 ,
                                            0x8000+GFL_ARCUTIL_TRANSINFO_GetPos(pWork->barbg),0, 0, pWork->heapID );
    GFL_BG_LoadScreenReq( GFL_BG_FRAME1_M );

    //下画面アイコン
    pWork->cellRes[_PLT_COMMON] = GFL_CLGRP_PLTT_RegisterEx(
      archandle , NARC_app_menu_common_bar_button_NCLR , CLSYS_DRAW_MAIN , _PAL_COMMON_CELL*32 , 0 , _PAL_COMMON_CELL_NUM , pWork->heapID  );

    pWork->cellRes[_NCG_COMMON] = GFL_CLGRP_CGR_Register(
      archandle , NARC_app_menu_common_bar_button_128k_NCGR , FALSE , CLSYS_DRAW_MAIN , pWork->heapID  );

    pWork->cellRes[_ANM_COMMON] = GFL_CLGRP_CELLANIM_Register(
      archandle, NARC_app_menu_common_bar_button_128k_NCER, NARC_app_menu_common_bar_button_128k_NANR , pWork->heapID);

    GFL_ARC_CloseDataHandle(archandle);
  }
  //下画面ボタン
  ITEMDISP_InitTaskBar(pWork);

  {
    u32 i;
    ARCHANDLE *archandleCommon = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId() , pWork->heapID );

    //タイプアイコン
    pWork->commonCell[SCR_PLT_SUB_POKE_TYPE] = GFL_CLGRP_PLTT_RegisterEx( archandleCommon ,
                                                                          APP_COMMON_GetPokeTypePltArcIdx() , CLSYS_DRAW_SUB ,
                                                                          _OBJPLT_SUB_POKE_TYPE*32 , 0 , APP_COMMON_POKETYPE_PLT_NUM , pWork->heapID  );
    pWork->commonCell[SCR_ANM_SUB_POKE_TYPE] = GFL_CLGRP_CELLANIM_Register( archandleCommon ,
                                                                            APP_COMMON_GetPokeTypeCellArcIdx(APP_COMMON_MAPPING_128K) ,
                                                                            APP_COMMON_GetPokeTypeAnimeArcIdx(APP_COMMON_MAPPING_128K), pWork->heapID  );
    //技タイプ
		for( i=0; i<WAZA_KIND_MAX; i++ ){
			pWork->commonCell[SCR_NCG_SKILL_TYPE_HENKA+i] = GFL_CLGRP_CGR_Register(
																												archandleCommon,
																												APP_COMMON_GetWazaKindCharArcIdx(i),
																												FALSE, CLSYS_DRAW_SUB, pWork->heapID );
		}
    //属性
    for( i=0;i<WAZA_TYPE_MAX; i++ ){
      pWork->commonCellTypeNcg[i] = GFL_CLGRP_CGR_Register(
																			archandleCommon,
																			APP_COMMON_GetPokeTypeCharArcIdx(i),
																			FALSE, CLSYS_DRAW_SUB, pWork->heapID );
    }

    GFL_ARC_CloseDataHandle(archandleCommon);
  }
  //技のアイコン
  {
    GFL_CLWK_DATA cellInitData1,cellInitData2;
		u32	i;

    cellInitData1.pos_x = 10*8;
    cellInitData1.pos_y = 22*8+8;
    cellInitData1.softpri = 1;
    cellInitData1.bgpri = 1;
    cellInitData1.anmseq = 0;

		// 分類
		for( i=0; i<WAZA_KIND_MAX; i++ ){
	    pWork->clwkWazaKind[i] = GFL_CLACT_WK_Create(
																pWork->cellUnit,
																pWork->commonCell[SCR_NCG_SKILL_TYPE_HENKA+i],
																pWork->commonCell[SCR_PLT_SUB_POKE_TYPE],
																pWork->commonCell[SCR_ANM_SUB_POKE_TYPE],
																&cellInitData1 ,CLSYS_DEFREND_SUB , pWork->heapID );
			GFL_CLACT_WK_SetPlttOffs(
				pWork->clwkWazaKind[i], APP_COMMON_GetWazaKindPltOffset(i), CLWK_PLTTOFFS_MODE_PLTT_TOP );
	    GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaKind[i], FALSE );
		}

    cellInitData2.pos_x = 10*8;
    cellInitData2.pos_y = 19*8+12;
    cellInitData2.softpri = 1;
    cellInitData2.bgpri = 1;
    cellInitData2.anmseq = 0;

		for( i=0;i<WAZA_TYPE_MAX;i++ ){
	    pWork->clwkWazaType[i] = GFL_CLACT_WK_Create(
																pWork->cellUnit,
																pWork->commonCellTypeNcg[i],
																pWork->commonCell[SCR_PLT_SUB_POKE_TYPE],
																pWork->commonCell[SCR_ANM_SUB_POKE_TYPE],
																&cellInitData2, CLSYS_DEFREND_SUB, pWork->heapID );
			GFL_CLACT_WK_SetPlttOffs(
				pWork->clwkWazaType[i], APP_COMMON_GetPokeTypePltOffset(i), CLWK_PLTTOFFS_MODE_PLTT_TOP );
			GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaType[i], FALSE );
		}
  }

  // ソートボタン リソースロード
  {
    ARCHANDLE *archandle = GFL_ARC_OpenDataHandle( ARCID_BAG , pWork->heapID );

    pWork->cellRes[_PLT_SORT] = GFL_CLGRP_PLTT_RegisterEx(
      archandle , NARC_bag_bag_win06_d_NCLR , CLSYS_DRAW_MAIN , _PAL_SORT_CELL*0x20 , 0 , _PAL_SORT_CELL_NUM , pWork->heapID  );
    pWork->cellRes[_NCG_SORT] = GFL_CLGRP_CGR_Register(
      archandle , NARC_bag_bag_win06_d_NCGR , FALSE , CLSYS_DRAW_MAIN , pWork->heapID);
    pWork->cellRes[_ANM_SORT] = GFL_CLGRP_CELLANIM_Register(
      archandle, NARC_bag_bag_win06_d_NCER, NARC_bag_bag_win06_d_NANR , pWork->heapID);

    GFL_ARC_CloseDataHandle(archandle);
  }

  // ソートボタン
  {
    GFL_CLWK_DATA cellInitData;

    cellInitData.softpri = 10;
    cellInitData.bgpri = 1;
    cellInitData.pos_x = 18 * 8 + 2;
    cellInitData.pos_y = 22 * 8;
    cellInitData.anmseq = 0;

    pWork->clwkSort = GFL_CLACT_WK_Create(
      pWork->cellUnit,
      pWork->cellRes[_NCG_SORT],
      pWork->cellRes[_PLT_SORT],
      pWork->cellRes[_ANM_SORT],
      &cellInitData ,CLSYS_DEFREND_MAIN , pWork->heapID );

    GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkSort, TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->clwkSort, TRUE );

		ITEMDISP_ChangeSortButton( pWork );
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  グラフィック消去
 *
 *  @param  pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void ITEMDISP_graphicDelete(FIELD_ITEMMENU_WORK* pWork)
{

  GFL_BG_FreeCharacterArea(GFL_BG_FRAME0_S,
                           GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subbg),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->subbg));
  GFL_BG_FreeCharacterArea(ITEMWIN_FRAME,
                           GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subbg2),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->subbg2));
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME3_M,
                           GFL_ARCUTIL_TRANSINFO_GetPos(pWork->numFrameBg),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->numFrameBg));
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME0_M,
                           GFL_ARCUTIL_TRANSINFO_GetPos(pWork->mainbg),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->mainbg));
  GFL_BG_FreeCharacterArea(GFL_BG_FRAME1_M,
                           GFL_ARCUTIL_TRANSINFO_GetPos(pWork->barbg),
                           GFL_ARCUTIL_TRANSINFO_GetSize(pWork->barbg));
}

//-----------------------------------------------------------------------------
/**
 *  @brief  上画面：表示／非表示 
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *  @param  on_off 
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void ITEMDISP_upMessageSetDispVBlank( FIELD_ITEMMENU_WORK* pWork, BOOL on_off )
{
  // BG アイテム枠／テキスト
  GFL_BG_SetVisible( ITEMWIN_FRAME, on_off );
  GFL_BG_SetVisible( ITEMREPORT_FRAME, on_off );

/*
  if( pWork->cellicon != NULL )
  {
    // アイテムOAMアイコン
    GFL_CLACT_WK_SetDrawEnable( pWork->cellicon, on_off );
  }
*/
}
void ITEMDISP_ItemInfoVanishSet( FIELD_ITEMMENU_WORK* pWork, BOOL on_off )
{
	if( on_off == TRUE ){
    pWork->bDispUpReq = 1;
	}else{
    pWork->bDispUpReq = 2;
	}

	if( pWork->cellicon != NULL ){
		GFL_CLACT_WK_SetDrawEnable( pWork->cellicon, on_off );
	}

	{
		ITEM_ST * item;
		u16	waza;
		item = ITEMMENU_GetItem( pWork, ITEMMENU_GetItemIndex(pWork) );
		waza = ITEM_GetWazaNo( item->id );
		if( waza != WAZANO_NULL ){
			GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaKind[WAZADATA_GetDamageType(waza)], on_off );
			GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaType[WAZADATA_GetType(waza)], on_off );
		}
	}
}

//------------------------------------------------------------------------------
/**
 * @brief   上画面のメッセージを再描画
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_upMessageRewrite(FIELD_ITEMMENU_WORK* pWork)
{
	ITEM_ST * item;
	int wazano;
	u32	i;

	// 技マシンのアイコンを全て非表示に
	for( i=0; i<WAZA_KIND_MAX; i++ ){
		GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaKind[i], FALSE );
	}
	for( i=0; i<WAZA_TYPE_MAX; i++ ){
		GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaType[i], FALSE );
	}

	item = ITEMMENU_GetItem( pWork, ITEMMENU_GetItemIndex(pWork) );

	if( item == NULL || item->id == ITEM_DUMMY_DATA ){
		ITEMDISP_ItemInfoVanishSet( pWork, FALSE );
		return;
	}

	ITEMDISP_ItemInfoVanishSet( pWork, TRUE );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(pWork->winItemName.win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(pWork->winItemNum.win), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(pWork->winItemReport.win), 0 );

	// わざマシンNOを取得
	wazano = ITEM_GetWazaNo( item->id );

	// 技マシン以外
	if( wazano == WAZANO_NULL ){
    // アイテム名
    GFL_MSG_GetString(  pWork->MsgManager, MSG_ITEM_STR005, pWork->pStrBuf );
    ITEMMENU_WordsetItemName( pWork, 0, item->id);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winItemName), 0, _UP_ITEMNAME_DOTOFS_Y, pWork->pExpStrBuf, pWork->fontHandle);
		PrintStr( pWork, &pWork->winItemName, pWork->pExpStrBuf, 0, _UP_ITEMNAME_DOTOFS_Y, PRINTSYS_LSB_Make(15,14,0) );

		if( pWork->pocketno != BAG_POKE_EVENT ){
	    //「×」
	    GFL_MSG_GetString(  pWork->MsgManager, MSG_ITEM_STR002, pWork->pStrBuf );
	    WORDSET_RegisterNumber(pWork->WordSet, 0, item->no,
	                           3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
	    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//	    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winItemNum), 0, _UP_ITEMNUM_DOTOFS_Y, pWork->pExpStrBuf, pWork->fontHandle);
			PrintStr( pWork, &pWork->winItemNum, pWork->pExpStrBuf, 0, _UP_ITEMNUM_DOTOFS_Y, PRINTSYS_LSB_Make(15,14,0) );
		}else{
			GFL_BMPWIN_TransVramCharacter( pWork->winItemNum.win );
		}

		GFL_BMPWIN_ClearScreen( pWork->winWaza.win );			// 技マシン用の表示データクリア

	// 技マシン
	}else{
    // わざマシン名
		u8	hiden = ITEM_GetHidenMashineNo( item->id );
		if( hiden == 0xff ){
	    GFL_MSG_GetString( pWork->MsgManager, msg_bag_086, pWork->pStrBuf );
	    WORDSET_RegisterNumber(
				pWork->WordSet, 0, ITEM_GetWazaMashineNo(item->id)+1, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
		}else{
	    GFL_MSG_GetString( pWork->MsgManager, msg_bag_088, pWork->pStrBuf );
	    WORDSET_RegisterNumber( pWork->WordSet, 0, hiden+1, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
		}
    WORDSET_RegisterWazaName(pWork->WordSet, 1, wazano);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winItemName), 0, _UP_ITEMNAME_DOTOFS_Y, pWork->pExpStrBuf, pWork->fontHandle);
		PrintStr( pWork, &pWork->winItemName, pWork->pExpStrBuf, 0, _UP_ITEMNAME_DOTOFS_Y, PRINTSYS_LSB_Make(15,14,0) );

	  ITEMDISP_WazaInfoWindowChange( pWork, wazano );

		GFL_BMPWIN_TransVramCharacter( pWork->winItemNum.win );
	}

  // アイテムの説明文
//  GFL_MSG_GetString(  pWork->MsgManagerItemInfo, item->id, pWork->pStrBuf );
	ITEM_GetInfo( pWork->pStrBuf, item->id, pWork->heapID );
//  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winItemReport), 0, 4, pWork->pStrBuf, pWork->fontHandle);
	PrintStr( pWork, &pWork->winItemReport, pWork->pStrBuf, 0, 4, PRINTSYS_LSB_Make(15,14,0) );

	PrintScreenTrans( &pWork->winItemName );
	PrintScreenTrans( &pWork->winItemNum );
	PrintScreenTrans( &pWork->winItemReport );

  // キャラ転送
//  GFL_BMPWIN_TransVramCharacter(pWork->winItemName);
//  GFL_BMPWIN_TransVramCharacter(pWork->winItemNum);
//  GFL_BMPWIN_TransVramCharacter(pWork->winItemReport);

  // アイテムアイコン アニメーション
  _itemiconAnim(pWork, item->id);

//  GFL_BG_LoadScreenV_Req( ITEMREPORT_FRAME );







/*
  int length;
  int itemIndex;
  int wazano;
  ITEM_ST * item;

  GF_ASSERT( pWork );
  
  length    = ITEMMENU_GetItemPocketNumber( pWork );
  itemIndex = ITEMMENU_GetItemIndex( pWork );  
  item      = ITEMMENU_GetItem( pWork, itemIndex );

  GF_ASSERT( item );

#ifdef PM_DEBUG
  if( item )
  {
    HOSAKA_Printf("上画面更新 length=%d item->id=%d \n",length, item->id );
  }
#endif

  // ワザマシンの説明追加
  ITEMDISP_WazaInfoWindowChange(pWork);
  
  // アイテムがない／ダミー
  if( (item==NULL) || (item->id==ITEM_DUMMY_DATA) )
  {
    // 上画面表示OFF
		ITEMDISP_ItemInfoVanishSet( pWork, FALSE );
    // 抜ける
    return;
  }
  
  // 上画面表示ON
	ITEMDISP_ItemInfoVanishSet( pWork, TRUE );

  // メッセージのCGXを消去
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->winItemName.win), 0 );
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->winItemNum.win), 0 );
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->winItemReport.win), 0 );

  // 文字色設定
//  GFL_FONTSYS_SetColor( 0xf, 0xe, 0 );

  // わざマシンNOを取得
  wazano = ITEM_GetWazaNo( item->id );

  if(wazano == WAZANO_NULL)
  {
    // 一般
    // アイテム名
    GFL_MSG_GetString(  pWork->MsgManager, MSG_ITEM_STR005, pWork->pStrBuf );
    ITEMMENU_WordsetItemName( pWork, 0, item->id);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winItemName), 0, _UP_ITEMNAME_DOTOFS_Y, pWork->pExpStrBuf, pWork->fontHandle);
		PrintStr( pWork, &pWork->winItemName, pWork->pExpStrBuf, 0, _UP_ITEMNAME_DOTOFS_Y, PRINTSYS_LSB_Make(15,14,0) );

    //「×」
    GFL_MSG_GetString(  pWork->MsgManager, MSG_ITEM_STR002, pWork->pStrBuf );

    WORDSET_RegisterNumber(pWork->WordSet, 0, item->no,
                           3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winItemNum), 0, _UP_ITEMNUM_DOTOFS_Y, pWork->pExpStrBuf, pWork->fontHandle);
		PrintStr( pWork, &pWork->winItemNum, pWork->pExpStrBuf, 0, _UP_ITEMNUM_DOTOFS_Y, PRINTSYS_LSB_Make(15,14,0) );
  }
  else
  {
    // わざマシン名
		u8	hiden = ITEM_GetHidenMashineNo( item->id );
		if( hiden == 0xff ){
	    GFL_MSG_GetString( pWork->MsgManager, msg_bag_086, pWork->pStrBuf );
	    WORDSET_RegisterNumber(
				pWork->WordSet, 0, ITEM_GetWazaMashineNo(item->id)+1, 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT);
		}else{
	    GFL_MSG_GetString( pWork->MsgManager, msg_bag_088, pWork->pStrBuf );
	    WORDSET_RegisterNumber( pWork->WordSet, 0, hiden+1, 1, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
		}
    WORDSET_RegisterWazaName(pWork->WordSet, 1, wazano);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winItemName), 0, _UP_ITEMNAME_DOTOFS_Y, pWork->pExpStrBuf, pWork->fontHandle);
		PrintStr( pWork, &pWork->winItemName, pWork->pExpStrBuf, 0, _UP_ITEMNAME_DOTOFS_Y, PRINTSYS_LSB_Make(15,14,0) );
  }

  // アイテムの説明文
//  GFL_MSG_GetString(  pWork->MsgManagerItemInfo, item->id, pWork->pStrBuf );
	ITEM_GetInfo( pWork->pStrBuf, item->id, pWork->heapID );
//  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winItemReport), 0, 4, pWork->pStrBuf, pWork->fontHandle);
	PrintStr( pWork, &pWork->winItemReport, pWork->pStrBuf, 0, 4, PRINTSYS_LSB_Make(15,14,0) );

  // キャラ転送
//  GFL_BMPWIN_TransVramCharacter(pWork->winItemName);
//  GFL_BMPWIN_TransVramCharacter(pWork->winItemNum);
//  GFL_BMPWIN_TransVramCharacter(pWork->winItemReport);

  // アイテムアイコン アニメーション
  _itemiconAnim(pWork, item->id);

  GFL_BG_LoadScreenV_Req( ITEMREPORT_FRAME );
*/
}

//-----------------------------------------------------------------------------
/**
 *  @brief  上画面：非表示
 *  @retval none
 */
//-----------------------------------------------------------------------------
void ITEMDISP_upMessageClean(FIELD_ITEMMENU_WORK* pWork)
{
	u32	i;

  // 上画面表示OFF
	ITEMDISP_ItemInfoVanishSet( pWork, FALSE );
    
  // わざマシン用の説明文を非表示
  GFL_BMPWIN_ClearScreen(pWork->winWaza.win);

  // ワザマシン用アイコンも非表示
	for( i=0; i<WAZA_KIND_MAX; i++ ){
		GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaKind[i], FALSE );
	}
	for( i=0; i<WAZA_TYPE_MAX; i++ ){
		GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaType[i], FALSE );
	}
}


//------------------------------------------------------------------------------
/**
 * @brief   上画面のメッセージの開放
 * @retval  none
 */
//------------------------------------------------------------------------------

void ITEMDISP_upMessageDelete(FIELD_ITEMMENU_WORK* pWork)
{
  int i;

//  GFL_BMPWIN_ClearScreen(pWork->winItemName.win);
//  GFL_BMPWIN_ClearScreen(pWork->winItemNum.win);
//  GFL_BMPWIN_ClearScreen(pWork->winItemReport.win);

//  GFL_BG_LoadScreenV_Req(ITEMREPORT_FRAME);

  GFL_BMPWIN_Delete(pWork->winItemName.win);
  GFL_BMPWIN_Delete(pWork->winItemReport.win);
  GFL_BMPWIN_Delete(pWork->winItemNum.win);
  GFL_BMPWIN_Delete(pWork->winWaza.win);

  // 下画面
  GFL_BMPWIN_Delete(pWork->winNumFrame.win);
  GFL_BMPWIN_Delete(pWork->winSellGold.win);
  GFL_BMPWIN_Delete(pWork->winPocketNone.win);

  GFL_CLACT_WK_Remove( pWork->clwkScroll );

 if(pWork->cellicon!=NULL){
  GFL_CLACT_WK_Remove( pWork->cellicon );
  GFL_CLGRP_CGR_Release( pWork->objRes[_CLACT_CHR] );
  GFL_CLGRP_PLTT_Release( pWork->objRes[_CLACT_PLT] );
 }

  GFL_CLGRP_CELLANIM_Release( pWork->objRes[_CLACT_ANM] );

  for( i=0; i < ITEM_LIST_NUM; i++ ){
    GFL_BMP_Delete(pWork->listBmp[i]);
  }

  GFL_CLACT_UNIT_Delete(pWork->cellUnit);

  GFL_CLACT_SYS_Delete();

}

//------------------------------------------------------------------------------
/**
 * @brief   上画面のメッセージの初期化
 * @retval  none
 */
//------------------------------------------------------------------------------

static void _bmpwinPocketNoneMake(FIELD_ITEMMENU_WORK* pWork )
{
  GF_ASSERT( pWork->pStrBuf );

  GFL_MSG_GetString(  pWork->MsgManager, msg_bag_item_none, pWork->pStrBuf );
/*
  GFL_FONTSYS_SetColor( 0xf, 0xe, 0 ); // カラー指定
  PRINTSYS_Print( GFL_BMPWIN_GetBmp( pWork->winPocketNone),
      _WIN_POCKETNONE_POSX, _WIN_POCKETNONE_POSY,
      pWork->pStrBuf, pWork->fontHandle );
*/
	PrintStr( pWork, &pWork->winPocketNone, pWork->pStrBuf, _WIN_POCKETNONE_POSX, _WIN_POCKETNONE_POSY, PRINTSYS_LSB_Make(15,14,0) );

//  GFL_BMPWIN_TransVramCharacter( pWork->winPocketNone );
}

void ITEMDISP_upMessageCreate(FIELD_ITEMMENU_WORK* pWork)
{
  // 下画面
  pWork->winPocketNone.win = GFL_BMPWIN_Create(
      GFL_BG_FRAME3_M,
      _WIN_POCKETNONE_INITX, _WIN_POCKETNONE_INITY,
      _WIN_POCKETNONE_SIZEX, _WIN_POCKETNONE_SIZEY,
      _WIN_POCKETNONE_PAL, GFL_BMP_CHRAREA_GET_B );

  // キャラクタを作っておく
  _bmpwinPocketNoneMake( pWork );
  GFL_BMPWIN_ClearScreen( pWork->winPocketNone.win );

  pWork->winNumFrame.win = GFL_BMPWIN_Create(
      GFL_BG_FRAME3_M,
      _WINNUM_INITX, _WINNUM_INITY,
      _WINNUM_SIZEX, _WINNUM_SIZEY,
      _WINNUM_PAL, GFL_BMP_CHRAREA_GET_B );

  pWork->winSellGold.win = GFL_BMPWIN_Create(
    GFL_BG_FRAME3_M,
    _SELL_GOLD_DISP_INITX, _SELL_GOLD_DISP_INITY,
    _SELL_GOLD_DISP_SIZEX, _SELL_GOLD_DISP_SIZEY,
    _WINNUM_PAL, GFL_BMP_CHRAREA_GET_B );

  // 上画面
  pWork->winWaza.win = GFL_BMPWIN_Create(
    ITEMREPORT_FRAME,
    0, 19,
    32, 5,
    _SUBBUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );

  pWork->winItemName.win = GFL_BMPWIN_Create(
    ITEMREPORT_FRAME,
    _UP_ITEMNAME_INITX, _UP_ITEMNAME_INITY,
    _UP_ITEMNAME_SIZEX, _UP_ITEMNAME_SIZEY,
    _SUBBUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );

  pWork->winItemNum.win = GFL_BMPWIN_Create(
    ITEMREPORT_FRAME,
    _UP_ITEMNUM_INITX, _UP_ITEMNUM_INITY,
    _UP_ITEMNUM_SIZEX, _UP_ITEMNUM_SIZEY,
    _SUBBUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );

  pWork->winItemReport.win = GFL_BMPWIN_Create(
    ITEMREPORT_FRAME,
    _UP_ITEMREPORT_INITX, _UP_ITEMREPORT_INITY,
    _UP_ITEMREPORT_SIZEX, _UP_ITEMREPORT_SIZEY,
    _SUBBUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );

  GFL_BMPWIN_MakeScreen( pWork->winItemName.win );
  GFL_BMPWIN_MakeScreen( pWork->winItemNum.win );
  GFL_BMPWIN_MakeScreen( pWork->winItemReport.win );
  GFL_BMPWIN_MakeScreen( pWork->winWaza.win );

  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_ITEMICON, pWork->heapID );

    pWork->objRes[_CLACT_ANM] = GFL_CLGRP_CELLANIM_Register( p_handle ,
                                                             NARC_item_icon_itemicon_NCER ,
                                                             NARC_item_icon_itemicon_NANR ,
                                                             pWork->heapID );
    GFL_ARC_CloseDataHandle( p_handle );
  }

	// 技マシンのデフォルト文字列描画
  // タイプ
  GFL_MSG_GetString( pWork->MsgManager, mes_bag_107, pWork->pStrBuf );
//  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winWaza), 8, 4, pWork->pStrBuf, pWork->fontHandle );
	PrintStr( pWork, &pWork->winWaza, pWork->pStrBuf, 8, 4, PRINTSYS_LSB_Make(15,14,0) );
  // ぶんるい
  GFL_MSG_GetString( pWork->MsgManager, mes_bag_098, pWork->pStrBuf );
//  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winWaza), 8, 24, pWork->pStrBuf, pWork->fontHandle );
	PrintStr( pWork, &pWork->winWaza, pWork->pStrBuf, 8, 24, PRINTSYS_LSB_Make(15,14,0) );
  // 威力
  GFL_MSG_GetString( pWork->MsgManager, mes_bag_096, pWork->pStrBuf );
//  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winWaza), 8*14, 4, pWork->pStrBuf, pWork->fontHandle );
	PrintStr( pWork, &pWork->winWaza, pWork->pStrBuf, 8*14, 4, PRINTSYS_LSB_Make(15,14,0) );
  // 命中
  GFL_MSG_GetString( pWork->MsgManager, mes_bag_097, pWork->pStrBuf );
//  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winWaza), 8*14, 24, pWork->pStrBuf, pWork->fontHandle );
	PrintStr( pWork, &pWork->winWaza, pWork->pStrBuf, 8*14, 24, PRINTSYS_LSB_Make(15,14,0) );
  // PP
  GFL_MSG_GetString( pWork->MsgManager, mes_bag_095, pWork->pStrBuf );
//  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winWaza), 26*8, 4, pWork->pStrBuf, pWork->fontHandle );
	PrintStr( pWork, &pWork->winWaza, pWork->pStrBuf, 26*8, 4, PRINTSYS_LSB_Make(15,14,0) );

  // キー操作時のみ、上画面を更新
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY )
  {
    ITEMDISP_upMessageRewrite(pWork);
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  アイテムアイコン 削除
 *
 *  @param  FIELD_ITEMMENU_WORK * pWork 
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
static void _itemiconDelete( FIELD_ITEMMENU_WORK * pWork )
{
  if(pWork->cellicon!=NULL){
    GFL_CLACT_WK_Remove( pWork->cellicon );
    GFL_CLGRP_CGR_Release( pWork->objRes[_CLACT_CHR] );
    GFL_CLGRP_PLTT_Release( pWork->objRes[_CLACT_PLT] );
    pWork->cellicon = NULL;
  }
}

//-----------------------------------------------------------------------------
/**
 *  @brief  アイテムアイコン アニメ
 *
 *  @param  pWork
 *  @param  itemid
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
static void _itemiconAnim(FIELD_ITEMMENU_WORK* pWork,int itemid)
{
  if(pWork->cellicon!=NULL)
  {
    _itemiconDelete( pWork );
  }

  {
    ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_ITEMICON, pWork->heapID );

    pWork->objRes[_CLACT_PLT] = GFL_CLGRP_PLTT_RegisterEx( p_handle ,
                                                           ITEM_GetIndex(itemid,ITEM_GET_ICON_PAL) ,
                                                           CLSYS_DRAW_SUB , 0, 0, 1 , pWork->heapID );
    pWork->objRes[_CLACT_CHR] = GFL_CLGRP_CGR_Register( p_handle ,
                                                        ITEM_GetIndex(itemid,ITEM_GET_ICON_CGX) ,
                                                        FALSE , CLSYS_DRAW_SUB , pWork->heapID );


    GFL_ARC_CloseDataHandle( p_handle );
  }

  {
    GFL_CLWK_DATA cellInitData;


    cellInitData.pos_x = _ITEMICON_SCR_X * 8 + 8 * 2 + 4;
    cellInitData.pos_y = _ITEMICON_SCR_Y * 8 + 8 * 2;
    cellInitData.anmseq = 0;
    cellInitData.softpri = 0;
    cellInitData.bgpri = 0;
    pWork->cellicon = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                           pWork->objRes[_CLACT_CHR],
                                           pWork->objRes[_CLACT_PLT],
                                           pWork->objRes[_CLACT_ANM],
                                           &cellInitData ,
                                           CLSYS_DEFREND_SUB ,
                                           pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->cellicon, TRUE );
    GFL_CLACT_WK_SetDrawEnable( pWork->cellicon, TRUE );
  }
}


//-----------------------------------------------------------------------------
/**
 *  @brief  描画 主処理
 *
 *  @param  pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void _dispMain(FIELD_ITEMMENU_WORK* pWork)
{
  GFL_CLACT_SYS_Main(); // CLSYSメイン
  if(pWork->pAppTask){
    APP_TASKMENU_UpdateMenu(pWork->pAppTask);
  }
}




//-----------------------------------------------------------------------------
/**
 *  @brief  セルリソース 生成
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void ITEMDISP_CellResourceCreate( FIELD_ITEMMENU_WORK* pWork )
{
  int i;
  ARCHANDLE *archandle = GFL_ARC_OpenDataHandle( ARCID_BAG , pWork->heapID );

  //リスト用チェックマーク
  pWork->cellRes[_PTL_CHECK] = GFL_CLGRP_PLTT_RegisterEx(
    archandle , NARC_bag_bag_win04_d_NCLR , CLSYS_DRAW_MAIN , _PAL_MENU_CHECKBOX_CELL*0x20 , 0 , _PAL_MENU_CHECKBOX_CELL_NUM , pWork->heapID  );
  pWork->cellRes[_NCG_CHECK] = GFL_CLGRP_CGR_Register(
    archandle , NARC_bag_bag_win04_d_NCGR , FALSE , CLSYS_DRAW_MAIN , pWork->heapID  );
  pWork->cellRes[_ANM_CHECK] = GFL_CLGRP_CELLANIM_Register(
    archandle, NARC_bag_bag_win04_d_NCER, NARC_bag_bag_win04_d_NANR , pWork->heapID);

  // スライドバーつまみ・リストカーソル
  pWork->cellRes[_PLT_CUR] = GFL_CLGRP_PLTT_RegisterEx(
															archandle, NARC_bag_bag_win03_d_NCLR, CLSYS_DRAW_MAIN,
															_PAL_CUR_CELL*0x20, 0, _PAL_CUR_CELL_NUM, pWork->heapID );

  pWork->cellRes[_NCG_CUR] = GFL_CLGRP_CGR_Register(
    archandle , NARC_bag_bag_win03_d_NCGR ,
    FALSE , CLSYS_DRAW_MAIN , pWork->heapID  );

  pWork->cellRes[_ANM_CUR] = GFL_CLGRP_CELLANIM_Register(
    archandle , NARC_bag_bag_win03_d_NCER,NARC_bag_bag_win03_d_NANR ,
    pWork->heapID  );


  for( i=0 ; i < ITEM_LIST_NUM ; i++ )
  {
    // OBJVRAM 64k地点からBMPとして使う
//    u8 *vramAdr = (u8*)( (u32)G2_GetOBJCharPtr() + 0x10000 - (12*2*32)*i );
//    pWork->listBmp[i] = GFL_BMP_CreateInVRAM( vramAdr, 12, 2, GFL_BMP_16_COLOR, pWork->heapID );

    pWork->listBmp[i] = GFL_BMP_Create( 12, 2, GFL_BMP_16_COLOR, pWork->heapID );
    pWork->listRes[i] = GFL_CLGRP_CGR_Register(  archandle , NARC_bag_bag_win01_d_NCGR , FALSE , CLSYS_DRAW_MAIN , pWork->heapID );
  }

  // 男女でウィンドウカーソル用パレットを切替
  // OAMリストの項目に使用。
  {
		u32 sex = MyStatus_GetMySex( pWork->mystatus );
    u32 res_nclr;
/*
     if( sex == PTL_SEX_MALE )
     {
       res_nclr = NARC_bag_bag_win01_d_man_NCLR;
     }
     else
     {
       res_nclr = NARC_bag_bag_win01_d_NCLR;
     }
*/
		res_nclr = NARC_bag_bag_win01_d_NCLR;

    pWork->cellRes[_PLT_LIST] = GFL_CLGRP_PLTT_RegisterEx(
																	archandle, res_nclr, CLSYS_DRAW_MAIN,
																	_PAL_WIN01_CELL*0x20, 0, _PAL_WIN01_CELL_NUM, pWork->heapID );
  }


  pWork->cellRes[_ANM_LIST] = GFL_CLGRP_CELLANIM_Register(
    archandle , NARC_bag_bag_win01_d_NCER, NARC_bag_bag_win01_d_NANR ,
    pWork->heapID  );

  GFL_ARC_CloseDataHandle(archandle);
}


//-----------------------------------------------------------------------------
/**
 *  @brief  セル生成
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void ITEMDISP_CellCreate( FIELD_ITEMMENU_WORK* pWork )
{

  //スクロールカーソル
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 31*8;
    cellInitData.pos_y = _SCROLL_TOP_Y;
    cellInitData.softpri = 1;
    cellInitData.bgpri = 2;
    cellInitData.anmseq = 0;

    pWork->clwkScroll = GFL_CLACT_WK_Create(
      pWork->cellUnit ,
      pWork->cellRes[_NCG_CUR], pWork->cellRes[_PLT_CUR],  pWork->cellRes[_ANM_CUR],
      &cellInitData ,CLSYS_DEFREND_MAIN , pWork->heapID );

    GFL_CLACT_WK_SetDrawEnable( pWork->clwkScroll, TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkScroll, TRUE );

		ITEMDISP_ScrollCursorChangePos( pWork );		// 初期位置設定
  }

  //選択カーソル
  {
    GFL_CLWK_DATA cellInitData;
    cellInitData.pos_x = 17*8-4+8;
    cellInitData.pos_y = 24+24;
    cellInitData.softpri = 1;
    cellInitData.bgpri = 2;
    cellInitData.anmseq = 1;

    pWork->clwkCur = GFL_CLACT_WK_Create(
      pWork->cellUnit,
      pWork->cellRes[_NCG_CUR], pWork->cellRes[_PLT_CUR],  pWork->cellRes[_ANM_CUR],
      &cellInitData ,CLSYS_DEFREND_MAIN, pWork->heapID );

    GFL_CLACT_WK_SetDrawEnable( pWork->clwkCur, TRUE );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkCur, TRUE );
  }

  //セルの作成
  {
    int i;
    for( i=0 ; i < ITEM_LIST_NUM ; i++ )
    {
      GFL_CLWK_DATA cellInitData;
      cellInitData.pos_x = 17 * 8 - 4 + 8;
      cellInitData.pos_y = 3 * 8 * i ;
      cellInitData.softpri = 10;
      cellInitData.bgpri = 3;
      cellInitData.anmseq = 0;

      pWork->listCell[i] = GFL_CLACT_WK_Create(
        pWork->cellUnit ,
        pWork->listRes[ i ],pWork->cellRes[_PLT_LIST],  pWork->cellRes[_ANM_LIST],
        &cellInitData ,CLSYS_DEFREND_MAIN , pWork->heapID );

      cellInitData.pos_x -= 3 * 8;
      cellInitData.anmseq = 1;
      pWork->listMarkCell[i] = GFL_CLACT_WK_Create(
        pWork->cellUnit ,
        pWork->cellRes[_NCG_CHECK],pWork->cellRes[_PTL_CHECK],  pWork->cellRes[_ANM_CHECK],
        &cellInitData ,CLSYS_DEFREND_MAIN , pWork->heapID );


      GFL_CLACT_WK_SetDrawEnable( pWork->listCell[i] , FALSE );
      GFL_CLACT_WK_SetDrawEnable( pWork->listMarkCell[i] , FALSE );

    }
  }

}

//-----------------------------------------------------------------------------
/**
 *  @brief  アイテムナンバーからパレットカラーを指定
 *
 *  @param  u16 itemtype
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
/*
static void _cellmessage_printcolor( u16 itemtype )
{
  switch( itemtype )
  {
  case ITEMTYPE_BALL:
  case ITEMTYPE_MAIL:
    GFL_FONTSYS_SetColor( 0x9, 0x8, 0 );
    break;

  case ITEMTYPE_EQUIP:
    GFL_FONTSYS_SetColor( 0x0b, 0xa, 0 );
    break;

  case ITEMTYPE_BATTLE:
  case ITEMTYPE_NORMAL:
  case ITEMTYPE_NONE:
    GFL_FONTSYS_SetColor( 0xf, 0xe, 0 );
    break;

  default : GF_ASSERT(0);
  }
}
*/

static BOOL CheckFieldWazaMachine( u16 item )
{
	if( ITEM_CheckWazaMachine( item ) == TRUE ){
		switch( ITEM_GetWazaNo(item) ){
//		case WAZANO_ANAWOHORU:		// あなをほる
//		case WAZANO_HURASSYU:			// フラッシュ
		case WAZANO_IAIGIRI:			// いあいぎり
		case WAZANO_KAIRIKI:			// かいりき
		case WAZANO_NAMINORI:			// なみのり
		case WAZANO_SORAWOTOBU:		// そらをとぶ
		case WAZANO_DAIBINGU:			// ダイビング
		case WAZANO_TAKINOBORI:		// たきのぼり
//		case WAZANO_IWAKUDAKI:		// いわくだき
			return TRUE;
		}
	}
	return FALSE;
}

//-----------------------------------------------------------------------------
/**
 *  @brief  セル メッセージ表示
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 * 
 */
//-----------------------------------------------------------------------------
void ITEMDISP_CellMessagePrint( FIELD_ITEMMENU_WORK* pWork )
{
  int i;
  int length;
	ARCHANDLE * ah;

  // 文字色指定
  static const u8 color_tbl[ ITEM_LIST_NUM ] = { 1, 0, 0, 0, 0, 0, 0, 1 };

  // ポケット内のアイテムが0個の時
  length = ITEMMENU_GetItemPocketNumber( pWork );

/*
  // スクロールバー表示切替
  if( length < ITEMMENU_SCROLLBAR_ENABLE_NUM )
  {
    // 消す
  }
  else
  {
    // だす
    // GFL_BG_WriteScreenFree( GFL_BG_FRAME2_S );
    
    {
//      void* adrs = GFL_BG_GetScreenBufferAdrs( GFL_BG_FRAME0_M );
    }
  }
*/

  if( length == 0 )
  {
    // 「なにもありません」表示
//    GFL_BMPWIN_MakeTransWindow( pWork->winPocketNone );
		PrintScreenTrans( &pWork->winPocketNone );
//		return;
  }
  else
  {
    // 「なにもありません」非表示
    GFL_BMPWIN_ClearTransWindow( pWork->winPocketNone.win );
//		PrintScreenTrans( &pWork->winPocketNone );
  }

	if( pWork->oamlistpos_old == pWork->oamlistpos ){ return; }

	// リスト表示
	ah = ITEM_OpenItemDataArcHandle( pWork->heapID );
	for( i=0; i<ITEM_LIST_NUM; i++ ){
    ITEM_ST * item;

    pWork->nListEnable[i] = 0;

		if( pWork->oamlistpos+i < 0 ){ continue; }

		item = ITEMMENU_GetItem( pWork, pWork->oamlistpos+i );

    if( (item==NULL) || (item->id==ITEM_DUMMY_DATA) ){ continue; }

    // リスト枠
    {
      void * itemdata;
			s32	type;
			PRINTSYS_LSB color;

      // ※ページ切替時にロードするようにすれば、負荷が軽減される
//      itemdata = ITEM_GetItemArcData( item->id, ITEM_GET_DATA, pWork->heapID );
			itemdata = ITEM_GetItemDataArcHandle( ah, item->id, pWork->heapID );
			type     = ITEM_GetBufParam( itemdata, ITEM_PRM_ITEM_TYPE );

			// 文字色指定。枠の外はグラデ
			if( color_tbl[i] == 0 ){
				if( CheckFieldWazaMachine( item->id ) == TRUE ){
//					GFL_FONTSYS_SetColor( 6, 5, 13 );
					color = PRINTSYS_LSB_Make(6,5,13);
				}else{
//					GFL_FONTSYS_SetColor( 2, 1, 13 );
					color = PRINTSYS_LSB_Make(2,1,13);
				}
			// 画面端はカラーフェード
			}else{
				if( CheckFieldWazaMachine( item->id ) == TRUE ){
//					GFL_FONTSYS_SetColor( 8, 7, 13 );
					color = PRINTSYS_LSB_Make(8,7,13);
				}else{
//					GFL_FONTSYS_SetColor( 4, 3, 13 );
					color = PRINTSYS_LSB_Make(4,3,13);
				}
			}

      GFL_BMP_Clear(pWork->listBmp[i], 13 );
      GFL_MSG_GetString( pWork->MsgManager, MSG_ITEM_STR001, pWork->pStrBuf );
			{
				u16	wazano = ITEM_GetWazaNo( item->id );
				if( wazano == WAZANO_NULL ){
		      ITEMMENU_WordsetItemName( pWork, 0, item->id );
				}else{
					WORDSET_RegisterWazaName( pWork->WordSet, 0, wazano );
				}
			}
      WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
      PRINTSYS_PrintColor( pWork->listBmp[i], 0, 0, pWork->pExpStrBuf, pWork->fontHandle, color );
//			PrintStr( pWork, &pWork->listBmp[i], pWork->pExpStrBuf, 0, 0, color );

			// ボール
			if( type == ITEMTYPE_BALL	){
				pWork->nListEnable[i] = 4;
			// メール
			}else if( type == ITEMTYPE_MAIL ){
				pWork->nListEnable[i] = 5;
			// 装備
			}else if( type == ITEMTYPE_EQUIP ){
				pWork->nListEnable[i] = 6;
			// 秘伝マシン
			}else if( ITEM_GetHidenMashineNo( item->id ) != 0xff ){
				pWork->nListEnable[i] = 8;
			// 技マシン
			}else if( ITEM_GetWazaMashineNo( item->id ) != 0xff ){
				pWork->nListEnable[i] = 7;
			// 登録できない
			}else if( ITEM_GetBufParam( itemdata, ITEM_PRM_CNV ) == 0 ){
				pWork->nListEnable[i] = 3;
			// 登録されていない
			}else if( ITEMMENU_CheckCnvButtonItem(pWork,item->id) == FALSE ){
				pWork->nListEnable[i] = 2;
			// その他
			}else{
				pWork->nListEnable[i] = 1;
			}
      GFL_HEAP_FreeMemory( itemdata );
    }
  }
	GFL_ARC_CloseDataHandle( ah );

	pWork->bCellChange = 1;
}

/*
void ITEMDISP_CellMessageScroll( FIELD_ITEMMENU_WORK * wk, int mv )
{
	GFL_BMP_DATA * bmp;
	ITEM_ST * item;
	u16	ap, wp;
	u32	i;
      void * itemdata;
			s32	type;

	if( mv < 0 ){
		ap = 0;
		wp = ITEM_LIST_NUM-1;

		bmp = wk->listBmp[wp];

		for( i=ITEM_LIST_NUM-1; i>0; i-- ){
			wk->listBmp[i] = wk->listBmp[i-1];
			wk->nListEnable[i] = wk->nListEnable[i-1];
		}

		wk->listBmp[ap] = bmp;
		wk->nListEnable[ap] = 0;

	}else{
		ap = ITEM_LIST_NUM-1;
		wp = 0;

		bmp = wk->listBmp[wp];

		for( i=0; i<ITEM_LIST_NUM-1; i++ ){
			wk->listBmp[i] = wk->listBmp[i+1];
			wk->nListEnable[i] = wk->nListEnable[i+1];
		}

		wk->listBmp[ap] = bmp;
		wk->nListEnable[ap] = 0;
	}


	item = ITEMMENU_GetItem( wk, wk->oamlistpos+ap );
  itemdata = ITEM_GetItemArcData( item->id, ITEM_GET_DATA, wk->heapID );
	type = ITEM_GetBufParam( itemdata, ITEM_PRM_ITEM_TYPE );

	if( !( item == NULL || item->id == ITEM_DUMMY_DATA ) ){
//		wk->listBmp[ap] = 
//					color = PRINTSYS_LSB_Make(2,1,13);
		GFL_BMP_Clear(wk->listBmp[ap], 13 );
		GFL_MSG_GetString( wk->MsgManager, MSG_ITEM_STR001, wk->pStrBuf );
    ITEMMENU_WordsetItemName( wk, 0, item->id );
		WORDSET_ExpandStr( wk->WordSet, wk->pExpStrBuf, wk->pStrBuf  );
		PRINTSYS_PrintColor( wk->listBmp[ap], 0, 0, wk->pExpStrBuf, wk->fontHandle, PRINTSYS_LSB_Make(2,1,13) );


			// ボール
			if( type == ITEMTYPE_BALL	){
				wk->nListEnable[ap] = 4;
			// メール
			}else if( type == ITEMTYPE_MAIL ){
				wk->nListEnable[ap] = 5;
			// 装備
			}else if( type == ITEMTYPE_EQUIP ){
				wk->nListEnable[ap] = 6;
			// 秘伝マシン
			}else if( ITEM_GetHidenMashineNo( item->id ) != 0xff ){
				wk->nListEnable[ap] = 8;
			// 技マシン
			}else if( ITEM_GetWazaMashineNo( item->id ) != 0xff ){
				wk->nListEnable[ap] = 7;
			// 登録できない
			}else if( ITEM_GetBufParam( itemdata, ITEM_PRM_CNV ) == 0 ){
				wk->nListEnable[ap] = 3;
			// 登録されていない
			}else if( ITEMMENU_CheckCnvButtonItem(wk,item->id) == FALSE ){
				wk->nListEnable[ap] = 2;
			// その他
			}else{
				wk->nListEnable[ap] = 1;
			}
      GFL_HEAP_FreeMemory( itemdata );


	}
}
*/




//-----------------------------------------------------------------------------
/**
 *  @brief  セル VRAM転送
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void ITEMDISP_CangeCursorPos( FIELD_ITEMMENU_WORK* pWork )
{

//  u32 dest = GFL_CLGRP_CGR_GetAddr( pWork->listRes[0], CLSYS_DRAW_MAIN);

  // リストカーソルの表示計算
  {
    enum {
      CUR_OFFSET_Y = 24,
      CUR_START_Y = 24,
    };

    GFL_CLACTPOS pos;
    int length;
//    BOOL is_cur_draw;
    BOOL is_scr_draw;

		ITEMDISP_ChangeCursorPosPalette( pWork, 0 );

    GFL_CLACT_WK_GetPos( pWork->clwkCur , &pos, CLWK_SETSF_NONE );
    pos.y = CUR_OFFSET_Y * pWork->curpos + CUR_START_Y;
    GFL_CLACT_WK_SetPos( pWork->clwkCur ,  &pos, CLWK_SETSF_NONE );

		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY ){
			ITEMDISP_ChangeCursorPosPalette( pWork, 1 );
		}
		BLINKPALANM_InitAnimeCount( pWork->blwk );

    // 現在のポケットのアイテム数取得
    length = ITEMMENU_GetItemPocketNumber( pWork );
    
    // カーソル条件：ポケット内のアイテムが0個かつキー状態のとき
//    is_cur_draw = (length!=0) && GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY;
    // スクロールバー条件：アイテムが7個以上
    is_scr_draw = (length >= ITEMMENU_SCROLLBAR_ENABLE_NUM);

    HOSAKA_Printf("length=%d \n", length);

    // カーソル／スクロールバー非表示
//    GFL_CLACT_WK_SetDrawEnable( pWork->clwkCur, is_cur_draw );
    GFL_CLACT_WK_SetDrawEnable( pWork->clwkScroll, is_scr_draw );

    // アニメーションフレームを更新
    GFL_CLACT_WK_SetAnmIndex( pWork->clwkCur, 0 );
    GFL_CLACT_WK_SetAnmIndex( pWork->clwkScroll, 0 );
  }
}

void ITEMDISP_CellVramTrans( FIELD_ITEMMENU_WORK* pWork )
{
  int i;

  // リストOAM生成＆描画
  for(i = 0; i< ITEM_LIST_NUM ; i++)
  {
    u32 dest_adrs = GFL_CLGRP_CGR_GetAddr( pWork->listRes[i], CLSYS_DRAW_MAIN);
    u8* charbuff = GFL_BMP_GetCharacterAdrs(pWork->listBmp[i]);
    u32 size = GFL_BMP_GetBmpDataSize(pWork->listBmp[i]);

    DC_FlushRange(charbuff, size);

    // 転送回数を減らせれば高速化できそうだが……
#if 1
    dest_adrs += (4*32);
    GX_LoadOBJ( &charbuff[8*32], dest_adrs, (32*4) );
    dest_adrs += (4*32);
    GX_LoadOBJ( &charbuff[20*32], dest_adrs, (32*4) );
    dest_adrs += (16*32);
    GX_LoadOBJ( &charbuff[0], dest_adrs, (32*8) );
    dest_adrs += (8*32);
    GX_LoadOBJ( &charbuff[12*32], dest_adrs, (32*8) );
#endif

    if( pWork->nListEnable[i] != 0 ){
      //「たいせつなもの」ポケット
      if( pWork->pocketno == BAG_POKE_EVENT ){
				if( ITEMMENU_CheckShortCutSetMode( pWork ) == TRUE && pWork->nListEnable[i] != 3 ){
					GFL_CLACT_WK_SetAnmSeq( pWork->listMarkCell[i], pWork->nListEnable[i]-1 );
	        GFL_CLACT_WK_SetDrawEnable( pWork->listMarkCell[i], TRUE );
				}else{
	        GFL_CLACT_WK_SetDrawEnable( pWork->listMarkCell[i], FALSE );
				}
			//「どうぐ」ポケット
			}else if( pWork->pocketno == BAG_POKE_NORMAL && pWork->nListEnable[i] >= 4 ){
        GFL_CLACT_WK_SetAnmSeq( pWork->listMarkCell[i], pWork->nListEnable[i]-1 );
        GFL_CLACT_WK_SetDrawEnable( pWork->listMarkCell[i], TRUE );
			//「わざマシン」ポケット
			}else if( pWork->pocketno == BAG_POKE_WAZA && pWork->nListEnable[i] >= 7 ){
        GFL_CLACT_WK_SetAnmSeq( pWork->listMarkCell[i], pWork->nListEnable[i]-1 );
        GFL_CLACT_WK_SetDrawEnable( pWork->listMarkCell[i], TRUE );
			}else{
        GFL_CLACT_WK_SetDrawEnable( pWork->listMarkCell[i], FALSE );
      }
      GFL_CLACT_WK_SetDrawEnable( pWork->listCell[i], TRUE );
		}else{
      GFL_CLACT_WK_SetDrawEnable( pWork->listMarkCell[i] , FALSE );
      GFL_CLACT_WK_SetDrawEnable( pWork->listCell[i] , FALSE );
    }
  }
}

//      GX_LoadOBJ(&charbuff[ 0*32], dest_adrs + (11)*32, (32*5));
//     GX_LoadOBJ(&charbuff[12*32], dest_adrs + (19)*32, (32*5));
//      GX_LoadOBJ(&charbuff[ 5*32], dest_adrs + (40)*32, (32*7));
//      GX_LoadOBJ(&charbuff[17*32], dest_adrs + (48)*32, (32*7));


//------------------------------------------------------------------------------
/**
 * @brief   スクロールバー上のスクロールカーソル表示
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_ScrollCursorMove(FIELD_ITEMMENU_WORK* pWork)
{
  u32 x,y;

  if(GFL_UI_TP_GetPointCont(&x, &y) == TRUE){
    GFL_CLACTPOS pos;
    if( y < _SCROLL_TOP_Y ){
			y = _SCROLL_TOP_Y;
    }else if( y > _SCROLL_BOTTOM_Y ){
			y = _SCROLL_BOTTOM_Y;
    }
    GFL_CLACT_WK_GetPos( pWork->clwkScroll, &pos, CLWK_SETSF_NONE );
    pos.y = y;
    GFL_CLACT_WK_SetPos( pWork->clwkScroll, &pos, CLWK_SETSF_NONE );
  }
}

//------------------------------------------------------------------------------
/**
 * @brief   指しているアイテム番号からスクロールカーソル位置を設定
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_ScrollCursorChangePos( FIELD_ITEMMENU_WORK * pWork )
{
	GFL_CLACTPOS pos;
	int length;
	
	length = ITEMMENU_GetItemPocketNumber( pWork );
	if( length < ITEMMENU_SCROLLBAR_ENABLE_NUM ){ return; }

	GFL_CLACT_WK_GetPos( pWork->clwkScroll, &pos, CLWK_SETSF_NONE );
	pos.y = SCROLLBAR_GetPosY( length-6, pWork->oamlistpos+1, _SCROLL_TOP_Y, _SCROLL_BOTTOM_Y, 0 );
	GFL_CLACT_WK_SetPos( pWork->clwkScroll, &pos, CLWK_SETSF_NONE );

//	PMSND_PlaySE( SE_BAG_SLIDE );
}

//------------------------------------------------------------------------------
/**
 * @brief   タスクバーのCELLの初期化
 * @retval  none
 */
//------------------------------------------------------------------------------
static void ITEMDISP_InitTaskBar( FIELD_ITEMMENU_WORK* pWork )
{
  u8 i;
  GFL_CLWK_DATA cellInitData;

   cellInitData.softpri = 10;
   cellInitData.bgpri = 1;

   //バーのボタン
  {
    const u8 anmIdxArr[] = { 4, 5, 6, 0, 1 };
    const u8 posXArr[] =
    {
      0,
      8*15,
      8*22-3,
      _BAR_CELL_CURSOR_EXIT,
      _BAR_CELL_CURSOR_RETURN,
    };

    u8 i;

    for( i = 0;i < elementof(anmIdxArr) ; i++ )
    {
      cellInitData.pos_x = posXArr[i];
      cellInitData.pos_y = 21*8;
      cellInitData.anmseq = anmIdxArr[i];

      if( i == BAR_ICON_CHECK_BOX ){ cellInitData.pos_y += 4; }

      pWork->clwkBarIcon[i] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                   pWork->cellRes[_NCG_COMMON],
                                                   pWork->cellRes[_PLT_COMMON],
                                                   pWork->cellRes[_ANM_COMMON],
                                                   &cellInitData ,CLSYS_DEFREND_MAIN , pWork->heapID );

      GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[i] , TRUE );
    }
		GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkBarIcon[BAR_ICON_LEFT], TRUE );
		GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkBarIcon[BAR_ICON_RIGHT], TRUE );

    
		//「売る」「持たせる」画面のときはチェックボックスを非表示
#ifdef	BUGFIX_BTS7820_20100714
		if( ITEMMENU_CheckShortCutSetMode( pWork ) == FALSE ){
#else		// BUGFIX_BTS7820_20100714
		if( pWork->mode == BAG_MODE_SELL || pWork->mode == BAG_MODE_POKELIST ){
#endif	// BUGFIX_BTS7820_20100714
			GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[BAR_ICON_CHECK_BOX] , FALSE );
		}
		//「持たせる」画面のときはＸボタン非表示
		if( pWork->mode == BAG_MODE_POKELIST ){
			GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[BAR_ICON_EXIT] , FALSE );
		}
	}

  // 数値入力ボタン初期化
  {
    const u8 anmIdxArr[] = { 3, 2 };
    const u8 posYArr[] = { 8 * 12 + 1, 8 * 15 - 1 };

    for( i=0; i<elementof(anmIdxArr); i++ )
    {
      u8 clwk_id = BAR_ICON_INPUT_U + i;

      cellInitData.pos_x = 8 * 29;
      cellInitData.pos_y = posYArr[i];
      cellInitData.anmseq = anmIdxArr[i];
      cellInitData.bgpri = 0;

      pWork->clwkBarIcon[ clwk_id ] = GFL_CLACT_WK_Create( pWork->cellUnit ,
                                                 pWork->cellRes[_NCG_COMMON],
                                                 pWork->cellRes[_PLT_COMMON],
                                                 pWork->cellRes[_ANM_COMMON],
                                                 &cellInitData ,CLSYS_DEFREND_MAIN , pWork->heapID );

     GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkBarIcon[ clwk_id ], TRUE );
     GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[ clwk_id ] , FALSE );
    }
  }

}


//------------------------------------------------------------------------------
/**
 * @brief   ポケットのCELLを初期化
 * @retval  none
 */
//------------------------------------------------------------------------------
static GFL_CLACTPOS pokectCellPos[]={
#include "bag_anmpos.h"
};
static const u8 BagSoftPriority[] = {
	14,		// どうぐ
	12,		// かいふく
	10,		// わざマシン
	12,		// きのみ
	16		// たいせつなもの
};
static const u8 BagAnime[][2] =
{	// 開, 閉
	{ NANR_bag_parts_d_dougu1,    NANR_bag_parts_d_dougu },			// どうぐ
	{ NANR_bag_parts_d_kaifuku1,  NANR_bag_parts_d_kaifuku },		// かいふく
	{ NANR_bag_parts_d_waza1,     NANR_bag_parts_d_waza },			// わざマシン
	{ NANR_bag_parts_d_kinomi1,   NANR_bag_parts_d_kinomi },		// きのみ
	{ NANR_bag_parts_d_taisetsu1, NANR_bag_parts_d_taisetsu },	// たいせつなもの
};

#define	BAG_OBJ_MOVE_SIZ	( 16 )
#define	BAG_OBJ_MOVE_CNT	( 4 )

void ITEMDISP_InitPocketCell( FIELD_ITEMMENU_WORK* pWork )
{
	GFL_CLWK_DATA dat;
	u32	i;

	for( i=0; i<BAG_POKE_MAX; i++ ){
		dat.softpri = BagSoftPriority[i];
		dat.bgpri   = 1;
		dat.pos_x   = pokectCellPos[i].x-BAG_OBJ_MOVE_SIZ;		// 初期移動分を動かしておく
		dat.pos_y   = pokectCellPos[i].y;
		dat.anmseq  = BagAnime[i][1];
		pWork->clwkBag[i] = GFL_CLACT_WK_Create(
													pWork->cellUnit,
													pWork->cellRes[_NCG_BAGPOCKET],
													pWork->cellRes[_PLT_BAGPOCKET],
													pWork->cellRes[_ANM_BAGPOCKET],
													&dat, CLSYS_DEFREND_MAIN, pWork->heapID );
    GFL_CLACT_WK_SetAutoAnmFlag( pWork->clwkBag[i], TRUE );
	}
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケットのCELLを初期化
 * @retval  none
 */
//------------------------------------------------------------------------------



//-----------------------------------------------------------------------------
/**
 *  @brief
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *  @param  pocketno
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void ITEMDISP_ChangePocketCell( FIELD_ITEMMENU_WORK* pWork,int pocketno )
{
	u32	i;

	for( i=0; i<BAG_POKE_MAX; i++ ){
		if( i == pocketno ){
			GFL_CLACT_WK_SetAnmSeq( pWork->clwkBag[i], BagAnime[i][0] );
		}else{
			GFL_CLACT_WK_SetAnmSeq( pWork->clwkBag[i], BagAnime[i][1] );
		}
	}
}

//------------------------------------------------------------------------------
/**
 * @brief   リストプレートのクリア
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_ListPlateClear( FIELD_ITEMMENU_WORK* pWork )
{
  //プレートの土台の絵

  //  for(i = 0 ; i < elementof(pWork->menuWin) ; i++){
  //  GFL_BMPWIN_ClearScreen(pWork->menuWin[i]);
  //  }

  //メッセージウインドのクリアも
//  GFL_BMPWIN_ClearScreen(pWork->itemInfoDispWin.win);
  BmpWinFrame_Clear(pWork->itemInfoDispWin.win,WINDOW_TRANS_ON_V);

//  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_M);
}





//------------------------------------------------------------------------------
/**
 * @brief   リストプレートの絵を使ってリストを作成
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_MenuWinDisp(  FIELD_ITEMMENU_WORK *pWork , int *menustr,int num )
{
  int i;
  APP_TASKMENU_INITWORK appinit;

  appinit.heapId = pWork->heapID;
  appinit.itemNum =  num;
  appinit.itemWork =  &pWork->appitem[0];

  appinit.posType = ATPT_RIGHT_DOWN;
  appinit.charPosX = 32;
  appinit.charPosY = 24;
  appinit.w        = APP_TASKMENU_PLATE_WIDTH;
  appinit.h        = APP_TASKMENU_PLATE_HEIGHT;

  for(i=0;i<num;i++){
    pWork->appitem[i].str = GFL_STR_CreateBuffer(100, pWork->heapID);
    GFL_MSG_GetString(pWork->MsgManager, menustr[i], pWork->appitem[i].str);
    pWork->appitem[i].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
    pWork->appitem[i].type = APP_TASKMENU_WIN_TYPE_NORMAL;
  }
  pWork->appitem[i-1].type = APP_TASKMENU_WIN_TYPE_RETURN;
  pWork->pAppTask = APP_TASKMENU_OpenMenu(&appinit,pWork->pAppTaskRes);
  for(i=0;i<num;i++){
    GFL_STR_DeleteBuffer(pWork->appitem[i].str);
  }

	ITEMDISP_ChangeActive( pWork, FALSE );
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテム説明の画面変更
 * @retval  none
 */
//------------------------------------------------------------------------------

void ITEMDISP_ItemInfoWindowChange(FIELD_ITEMMENU_WORK *pWork,int pocketno )
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_BAG, pWork->heapID );
  if(pocketno!=BAG_POKE_WAZA){
    GFL_ARCHDL_UTIL_TransVramScreenCharOfsVBlank(
      p_handle, NARC_bag_bag_win01_u_NSCR, ITEMWIN_FRAME, 0,
      GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subbg2), 0, 0, pWork->heapID);
  }
  else{
    GFL_ARCHDL_UTIL_TransVramScreenCharOfsVBlank(
      p_handle, NARC_bag_bag_win02_u_NSCR, ITEMWIN_FRAME, 0,
      GFL_ARCUTIL_TRANSINFO_GetPos(pWork->subbg2), 0, 0, pWork->heapID);
  }
  GFL_ARC_CloseDataHandle(p_handle);
}

//------------------------------------------------------------------------------
/**
 * @brief   技種類のアイコンの表示
 * @retval  none
 */
//------------------------------------------------------------------------------
/*
static void _wazaKindDisp( FIELD_ITEMMENU_WORK *pWork ,int wazaNo)
{
  {
    NNSG2dImageProxy imageProxy;
    const int type = WAZADATA_GetDamageType( wazaNo );
    GFL_CLGRP_CGR_GetProxy( pWork->commonCell[SCR_NCG_SKILL_TYPE_HENKA+type] , &imageProxy );
    GFL_CLACT_WK_SetImgProxy( pWork->clwkWazaKind , &imageProxy );
    GFL_CLACT_WK_SetPlttOffs( pWork->clwkWazaKind ,
                              APP_COMMON_GetWazaKindPltOffset(type) ,
                              CLWK_PLTTOFFS_MODE_PLTT_TOP );
    GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaKind, TRUE );
  }
}
*/

//------------------------------------------------------------------------------
/**
 * @brief   技タイプのアイコン表示
 * @retval  none
 */
//------------------------------------------------------------------------------
/*
static void _wazaTypeDisp( FIELD_ITEMMENU_WORK *pWork ,int wazaNo)
{
  {
    NNSG2dImageProxy imageProxy;
    int type1 = WAZADATA_GetType(wazaNo);
    GFL_CLGRP_CGR_GetProxy( pWork->commonCellTypeNcg[type1] , &imageProxy );
    GFL_CLACT_WK_SetImgProxy( pWork->clwkWazaType , &imageProxy );
    GFL_CLACT_WK_SetPlttOffs( pWork->clwkWazaType ,
                              APP_COMMON_GetPokeTypePltOffset(type1) ,
                              CLWK_PLTTOFFS_MODE_PLTT_TOP );
    GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaType , TRUE );
  }
}
*/

//------------------------------------------------------------------------------
/**
 * @brief   技マシンの説明追加
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_WazaInfoWindowChange( FIELD_ITEMMENU_WORK *pWork, int wazano )
{
  PRINT_UTIL * pwin;
  int ppnum;
  int pow;
  int hit;

	pwin = &pWork->winWaza;

	GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaKind[WAZADATA_GetDamageType(wazano)], TRUE );
	GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaType[WAZADATA_GetType(wazano)], TRUE );

  ppnum = WAZADATA_GetMaxPP(wazano, 0 );
  pow = WAZADATA_GetParam( wazano, WAZAPARAM_POWER );
  hit = WAZADATA_GetParam( wazano, WAZAPARAM_HITPER );

	// パラメータ部分だけクリア
	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(pwin->win), 22*8, 0, 4*8, 5*8, 0 );
	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(pwin->win), 29*8, 0, 3*8, 3*8, 0 );

	// 威力
  if(pow<=1){
    GFL_MSG_GetString(  pWork->MsgManager, msg_bag_023, pWork->pStrBuf );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 22*8, 4, pWork->pStrBuf, pWork->fontHandle);
		PrintStr( pWork, pwin, pWork->pStrBuf, 22*8, 4, PRINTSYS_LSB_Make(15,14,0) );
  }
  else{
    //いりょくの桁数
    GFL_MSG_GetString(  pWork->MsgManager, mes_bag_100, pWork->pStrBuf );
    WORDSET_RegisterNumber(pWork->WordSet, 0, pow, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 22*8, 4, pWork->pExpStrBuf, pWork->fontHandle);
		PrintStr( pWork, pwin, pWork->pExpStrBuf, 22*8, 4, PRINTSYS_LSB_Make(15,14,0) );
  }

	// 命中
  if(hit==0||WAZADATA_IsAlwaysHit(wazano)==TRUE){
    GFL_MSG_GetString(  pWork->MsgManager, msg_bag_023, pWork->pStrBuf );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 22*8, 24, pWork->pStrBuf, pWork->fontHandle);
		PrintStr( pWork, pwin, pWork->pStrBuf, 22*8, 24, PRINTSYS_LSB_Make(15,14,0) );
  }
  else{
    //めいちゅうの桁数
    GFL_MSG_GetString(  pWork->MsgManager, mes_bag_100, pWork->pStrBuf );
    WORDSET_RegisterNumber(pWork->WordSet, 0, hit, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 22*8, 24, pWork->pExpStrBuf, pWork->fontHandle);
		PrintStr( pWork, pwin, pWork->pExpStrBuf, 22*8, 24, PRINTSYS_LSB_Make(15,14,0) );
  }

  //PPの桁数
  GFL_MSG_GetString(  pWork->MsgManager, mes_bag_099, pWork->pStrBuf );
  WORDSET_RegisterNumber(pWork->WordSet, 0, ppnum, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 29*8, 4, pWork->pExpStrBuf, pWork->fontHandle);
	PrintStr( pWork, pwin, pWork->pExpStrBuf, 29*8, 4, PRINTSYS_LSB_Make(15,14,0) );

	PrintScreenTrans( pwin );

/*
  PRINT_UTIL * pwin = &pWork->winWaza;
  ITEM_ST * item;
  int wazano;
  int ppnum;
  int pow;
  int hit;
	u32	i;

  GFL_BG_LoadScreenV_Req( ITEMWIN_FRAME );

	// アイコンを全て非表示に
	for( i=0; i<WAZA_KIND_MAX; i++ ){
		GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaKind[i], FALSE );
	}
	for( i=0; i<WAZA_TYPE_MAX; i++ ){
		GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaType[i], FALSE );
	}
  
  item = ITEMMENU_GetItem( pWork,ITEMMENU_GetItemIndex(pWork) );
  wazano = ITEM_GetWazaNo( item->id );

  // アイテムがない／ダミー／非わざマシン
  if((item==NULL) || (item->id==ITEM_DUMMY_DATA) || wazano==WAZANO_NULL )
  {
    // わざマシン用の表示を非表示にして抜ける
		GFL_BMPWIN_ClearScreen( pwin->win );
		return;
  }
  
//  GFL_BMPWIN_MakeScreen(pwin);

  ppnum = WAZADATA_GetMaxPP(wazano, 0);
  pow = WAZADATA_GetParam( wazano, WAZAPARAM_POWER );
  hit = WAZADATA_GetParam( wazano, WAZAPARAM_HITPER );

//  _wazaKindDisp(pWork,wazano);
//  _wazaTypeDisp(pWork,wazano);
	GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaKind[WAZADATA_GetDamageType(wazano)], TRUE );
	GFL_CLACT_WK_SetDrawEnable( pWork->clwkWazaType[WAZADATA_GetType(wazano)], TRUE );

  // カラー指定
//  GFL_FONTSYS_SetColor( 0xf, 0xe, 0 );

	// パラメータ部分だけクリア
	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(pwin->win), 22*8, 0, 4*8, 5*8, 0 );
	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(pwin->win), 29*8, 0, 3*8, 3*8, 0 );

	// 威力
  if(pow==0){
    GFL_MSG_GetString(  pWork->MsgManager, msg_bag_023, pWork->pStrBuf );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 22*8, 4, pWork->pStrBuf, pWork->fontHandle);
		PrintStr( pWork, pwin, pWork->pStrBuf, 22*8, 4, PRINTSYS_LSB_Make(15,14,0) );
  }
  else{
    //いりょくの桁数
    GFL_MSG_GetString(  pWork->MsgManager, mes_bag_100, pWork->pStrBuf );
    WORDSET_RegisterNumber(pWork->WordSet, 0, pow, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 22*8, 4, pWork->pExpStrBuf, pWork->fontHandle);
		PrintStr( pWork, pwin, pWork->pExpStrBuf, 22*8, 4, PRINTSYS_LSB_Make(15,14,0) );
  }

	// 命中
  if(hit==0){
    GFL_MSG_GetString(  pWork->MsgManager, msg_bag_023, pWork->pStrBuf );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 22*8, 24, pWork->pStrBuf, pWork->fontHandle);
		PrintStr( pWork, pwin, pWork->pStrBuf, 22*8, 24, PRINTSYS_LSB_Make(15,14,0) );
  }
  else{
    //めいちゅうの桁数
    GFL_MSG_GetString(  pWork->MsgManager, mes_bag_100, pWork->pStrBuf );
    WORDSET_RegisterNumber(pWork->WordSet, 0, hit, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 22*8, 24, pWork->pExpStrBuf, pWork->fontHandle);
		PrintStr( pWork, pwin, pWork->pExpStrBuf, 22*8, 24, PRINTSYS_LSB_Make(15,14,0) );
  }

  //PPの桁数
  GFL_MSG_GetString(  pWork->MsgManager, mes_bag_099, pWork->pStrBuf );
  WORDSET_RegisterNumber(pWork->WordSet, 0, ppnum, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 29*8, 4, pWork->pExpStrBuf, pWork->fontHandle);
	PrintStr( pWork, pwin, pWork->pExpStrBuf, 29*8, 4, PRINTSYS_LSB_Make(15,14,0) );

	PrintScreenTrans( pwin );

  // キャラクタ転送
//  GFL_BMPWIN_TransVramCharacter(pwin);
*/
}


//-----------------------------------------------------------------------------
/**
 * @brief   アイテム説明の文章をExpバッファに入れる
 * @param   is_stream TRUE:ストリーム再生, FALSE:ベタ描画
 * @retval  none
 */
//-----------------------------------------------------------------------------
void ITEMDISP_ItemInfoWindowDispEx( FIELD_ITEMMENU_WORK* pWork, BOOL is_stream )
{
  PRINT_UTIL * pwin;

  if(pWork->itemInfoDispWin.win==NULL){
    pWork->itemInfoDispWin.win = GFL_BMPWIN_Create(
      GFL_BG_FRAME3_M ,
      1 , 1, 30 ,4 ,
      _BUTTON_MSG_PAL , GFL_BMP_CHRAREA_GET_B );
  }
  pwin = &pWork->itemInfoDispWin;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pwin->win), 15);
	GFL_BMPWIN_TransVramCharacter( pwin->win );
//  GFL_FONTSYS_SetColor(1, 2, 15);

  if( is_stream == TRUE )
  {
    pWork->pStream = PRINTSYS_PrintStream(pwin->win ,0,0, pWork->pExpStrBuf, pWork->fontHandle,
                      MSGSPEED_GetWait(), pWork->pMsgTcblSys, 2, pWork->heapID, 15);
  }
  else
  {
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pwin), 0, 0, pWork->pExpStrBuf, pWork->fontHandle);
		PrintStr( pWork, pwin, pWork->pExpStrBuf, 0, 0, PRINTSYS_LSB_Make(1,2,15) );
  }

  BmpWinFrame_Write( pwin->win, WINDOW_TRANS_ON_V, GFL_ARCUTIL_TRANSINFO_GetPos(pWork->bgchar), _BUTTON_WIN_PAL );

//  GFL_BMPWIN_TransVramCharacter(pwin);
//  GFL_BMPWIN_MakeScreen(pwin);
//  GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_M);
	PrintScreenTrans( pwin );

}

//------------------------------------------------------------------------------
/**
 * @brief   説明ウインドウ表示
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_ItemInfoWindowDisp( FIELD_ITEMMENU_WORK *pWork )
{
  ITEMDISP_ItemInfoWindowDispEx( pWork, FALSE );
}

//------------------------------------------------------------------------------
/**
 * @brief   アイテム説明の文章をExpバッファに入れる
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_ItemInfoMessageMake( FIELD_ITEMMENU_WORK *pWork,int id )
{
  GFL_MSG_GetString( pWork->MsgManager, msg_bag_042, pWork->pStrBuf );
  ITEMMENU_WordsetItemName( pWork, 0, id);
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
}

//------------------------------------------------------------------------------
/**
 * @brief   メッセージの終了待ち
 * @retval  none
 */
//------------------------------------------------------------------------------

BOOL ITEMDISP_MessageEndCheck(FIELD_ITEMMENU_WORK* pWork)
{
  if(pWork->pStream)
  {
    int state = PRINTSYS_PrintStreamGetState( pWork->pStream );
    APP_KEYCURSOR_Main( pWork->MsgCursorWork, pWork->pStream, pWork->itemInfoDispWin.win );
    switch(state)
    {
    case PRINTSTREAM_STATE_DONE: // 終了
      PRINTSYS_PrintStreamDelete( pWork->pStream );
      pWork->pStream = NULL;
			pWork->stream_clear_flg = FALSE;
      break;

    case PRINTSTREAM_STATE_PAUSE: // 一時停止中
			if( pWork->stream_clear_flg == FALSE ){
				if( ( GFL_UI_KEY_GetTrg() & MSG_SKIP_BTN ) || GFL_UI_TP_GetTrg() ){
	        PMSND_PlaySystemSE( SE_BAG_MESSAGE );
	        PRINTSYS_PrintStreamReleasePause( pWork->pStream );
					pWork->stream_clear_flg = TRUE;
				}
      }
      break;
    case PRINTSTREAM_STATE_RUNNING :  // 実行中
      // メッセージスキップ
      if( ( GFL_UI_KEY_GetCont() & MSG_SKIP_BTN ) || GFL_UI_TP_GetCont() )
      {
        PRINTSYS_PrintStreamShortWait( pWork->pStream, 0 );
      }
			pWork->stream_clear_flg = FALSE;
      break;
    default:
      GF_ASSERT(0);
    }
    return FALSE;  //まだ終わってない
  }
  return TRUE;// 終わっている
}


//-----------------------------------------------------------------------------
/**
 *  @brief  BARメッセージ生成
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void ITEMDISP_BarMessageCreate( FIELD_ITEMMENU_WORK* pWork )
{
  pWork->pocketNameWin.win = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M,
    _POCKETNAME_DISP_INITX, _POCKETNAME_DISP_INITY,
    _POCKETNAME_DISP_SIZEX, _POCKETNAME_DISP_SIZEY,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );

  pWork->winGoldCap.win = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M,
    _GOLD_CAP_DISP_INITX, _GOLD_CAP_DISP_INITY,
    _GOLD_CAP_DISP_SIZEX, _GOLD_CAP_DISP_SIZEY,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );

  pWork->winGold.win = GFL_BMPWIN_Create(
    GFL_BG_FRAME2_M,
    _GOLD_DISP_INITX, _GOLD_DISP_INITY,
    _GOLD_DISP_SIZEX, _GOLD_DISP_SIZEY,
    _BUTTON_MSG_PAL, GFL_BMP_CHRAREA_GET_B );

  ITEMDISP_PocketMessage(pWork, pWork->pocketno);
  ITEMDISP_ChangePocketCell( pWork, pWork->pocketno );
}


//-----------------------------------------------------------------------------
/**
 *  @brief  BARメッセージ削除
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void ITEMDISP_BarMessageDelete( FIELD_ITEMMENU_WORK* pWork )
{
  GFL_BMPWIN_Delete(pWork->pocketNameWin.win);
  GFL_BMPWIN_Delete(pWork->winGold.win);
  GFL_BMPWIN_Delete(pWork->winGoldCap.win);
}

//------------------------------------------------------------------------------
/**
 * @brief   ポケット名の表示
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_PocketMessage(FIELD_ITEMMENU_WORK* pWork,int newpocket)
{
  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->pocketNameWin.win), 0 );
//  GFL_FONTSYS_SetColor( _POCKETNAME_FONT_PAL_L, _POCKETNAME_FONT_PAL_S, _POCKETNAME_FONT_PAL_B );
//  GFL_MSG_GetString(pWork->MsgManagerPocket, msg_pocket_001+newpocket, pWork->pStrBuf );
	GFL_MSG_GetString(  pWork->MsgManager, mes_bag_114, pWork->pStrBuf );
	WORDSET_RegisterItemPocketName( pWork->WordSet, 0, newpocket );
	WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf );

  {  //センタリング
    u32 dot =PRINTSYS_GetStrWidth(pWork->pExpStrBuf, pWork->fontHandle, 0);
    HOSAKA_Printf("dot=%d \n", dot);
    dot = (_POCKETNAME_DISP_SIZEX*8 - dot )/2;
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->pocketNameWin), dot, 4, pWork->pExpStrBuf, pWork->fontHandle);
		PrintStr(
			pWork, &pWork->pocketNameWin, pWork->pExpStrBuf, dot, 4,
			PRINTSYS_LSB_Make(_POCKETNAME_FONT_PAL_L,_POCKETNAME_FONT_PAL_S,_POCKETNAME_FONT_PAL_B) );
  }

//  GFL_BMPWIN_MakeTransWindow_VBlank( pWork->pocketNameWin );
	PrintScreenTrans( &pWork->pocketNameWin );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  おこづかい表示
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
void ITEMDISP_GoldDispWrite( FIELD_ITEMMENU_WORK* pWork )
{
/*
  GFL_BMP_DATA* bmpGold;
  GFL_BMP_DATA* bmpGoldCap;

  bmpGold = GFL_BMPWIN_GetBmp( pWork->winGold );
  bmpGoldCap = GFL_BMPWIN_GetBmp( pWork->winGoldCap );
*/

//  GFL_FONTSYS_SetColor( _POCKETNAME_FONT_PAL_L, _POCKETNAME_FONT_PAL_S, _POCKETNAME_FONT_PAL_B );

  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(pWork->winGold.win), 0 );
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( pWork->winGoldCap.win), 0 );

  //「おこづかい」
  GFL_MSG_GetString( pWork->MsgManager, mes_shop_097, pWork->pStrBuf );
//  PRINTSYS_Print( bmpGoldCap, 0, 4, pWork->pStrBuf, pWork->fontHandle );
	PrintStr(
		pWork, &pWork->winGoldCap, pWork->pStrBuf, 0, 4,
		PRINTSYS_LSB_Make(_POCKETNAME_FONT_PAL_L,_POCKETNAME_FONT_PAL_S,_POCKETNAME_FONT_PAL_B) );
//  GFL_BMPWIN_MakeTransWindow_VBlank( pWork->winGoldCap );
	PrintScreenTrans( &pWork->winGoldCap );

  //「円」
  GFL_MSG_GetString( pWork->MsgManager, mes_shop_098, pWork->pStrBuf );
  WORDSET_RegisterNumber(pWork->WordSet, 0, MISC_GetGold( GAMEDATA_GetMiscWork(pWork->gamedata) ),
                          7, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT);
  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//  PRINTSYS_Print( bmpGold, 0, 4, pWork->pExpStrBuf, pWork->fontHandle );
	PrintStr(
		pWork, &pWork->winGold, pWork->pExpStrBuf, 0, 4,
		PRINTSYS_LSB_Make(_POCKETNAME_FONT_PAL_L,_POCKETNAME_FONT_PAL_S,_POCKETNAME_FONT_PAL_B) );
//  GFL_BMPWIN_MakeTransWindow_VBlank( pWork->winGold );
	PrintScreenTrans( &pWork->winGold );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  おこづかい表示開始
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void ITEMDISP_GoldDispIn( FIELD_ITEMMENU_WORK* pWork )
{
  // 左右カーソル非表示
  GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[BAR_ICON_LEFT] , FALSE );
  GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[BAR_ICON_RIGHT] , FALSE );

  // ポケット名非表示
//  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( pWork->pocketNameWin ), 0 );
//  GFL_BMPWIN_TransVramCharacter(pWork->pocketNameWin);
	GFL_BMPWIN_ClearScreen( pWork->pocketNameWin.win );

  // おこづかい表示
  ITEMDISP_GoldDispWrite( pWork );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  おこづかい表示終了
 *
 *  @param  FIELD_ITEMMENU_WORK* pWork
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void ITEMDISP_GoldDispOut( FIELD_ITEMMENU_WORK* pWork )
{
  // 左右カーソル表示
  GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[BAR_ICON_LEFT] , TRUE );
  GFL_CLACT_WK_SetDrawEnable( pWork->clwkBarIcon[BAR_ICON_RIGHT] , TRUE );
  // おこづかい非表示
//  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( pWork->winGold ), 0 );
//  GFL_BMP_Clear( GFL_BMPWIN_GetBmp( pWork->winGoldCap ), 0 );
//  GFL_BMPWIN_TransVramCharacter( pWork->winGold );
//  GFL_BMPWIN_TransVramCharacter( pWork->winGoldCap );
	GFL_BMPWIN_ClearScreen( pWork->winGold.win );
	GFL_BMPWIN_ClearScreen( pWork->winGoldCap.win );

  // ポケット名表示
  ITEMDISP_PocketMessage( pWork, pWork->pocketno );
}

//-----------------------------------------------------------------------------
/**
 *  @brief   はいいいえウインドウ 開始
 *
 *  @param  pWork
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
void ITEMDISP_YesNoStart(FIELD_ITEMMENU_WORK* pWork)
{
  int i;
  APP_TASKMENU_INITWORK appinit;

  appinit.heapId = pWork->heapID;
  appinit.itemNum =  2;
  appinit.itemWork =  &pWork->appitem[0];

  appinit.posType = ATPT_RIGHT_DOWN;
  appinit.charPosX = 32;
  appinit.charPosY = 12;
  appinit.w        = APP_TASKMENU_PLATE_WIDTH_YN_WIN;
  appinit.h        = APP_TASKMENU_PLATE_HEIGHT_YN_WIN;

  pWork->appitem[0].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->MsgManager, MSG_ITEM_STR003, pWork->appitem[0].str);
  pWork->appitem[0].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  pWork->appitem[0].type = APP_TASKMENU_WIN_TYPE_NORMAL;

  pWork->appitem[1].str = GFL_STR_CreateBuffer(100, pWork->heapID);
  GFL_MSG_GetString(pWork->MsgManager, MSG_ITEM_STR004, pWork->appitem[1].str);
  pWork->appitem[1].msgColor = PRINTSYS_LSB_Make( 0xe,0xf,0);
  pWork->appitem[1].type = APP_TASKMENU_WIN_TYPE_NORMAL;

  pWork->pAppTask = APP_TASKMENU_OpenMenu(&appinit,pWork->pAppTaskRes);
  GFL_STR_DeleteBuffer(pWork->appitem[0].str);
  GFL_STR_DeleteBuffer(pWork->appitem[1].str);

	ITEMDISP_ChangeRetButtonActive( pWork, FALSE );
	ITEMDISP_ChangeActive( pWork, FALSE );
}

//-----------------------------------------------------------------------------
/**
 *  @brief  はいいいえウィンドウ 終了
 *
 *  @param  pWork
 *
 *  @retval none
 */
//-----------------------------------------------------------------------------
void ITEMDISP_YesNoExit(FIELD_ITEMMENU_WORK* pWork)
{
  APP_TASKMENU_CloseMenu(pWork->pAppTask);
  pWork->pAppTask=NULL;
//	ITEMDISP_ChangeActive( pWork, TRUE );
}

//------------------------------------------------------------------------------
/**
 * @brief   数字フレームの表示
 * @retval  none
 */
//------------------------------------------------------------------------------
void ITEMDISP_NumFrameDisp(FIELD_ITEMMENU_WORK* pWork)
{
  ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_BAG, pWork->heapID );

  {
    void* arcData = GFL_ARCHDL_UTIL_Load( p_handle, NARC_bag_bag_win05_d_NSCR, 0, pWork->heapID );

    NNSG2dScreenData *scrnData;

    if( NNS_G2dGetUnpackedScreenData( arcData, &scrnData ) )
    {
      int i;
      int xs = scrnData->screenWidth/8;
      int ys = scrnData->screenHeight/8;
      u16* buff = (u16*)&scrnData->rawData;

      // パレット指定
      for(i=0;i<(xs*ys);i++){
        buff[i] += GFL_ARCUTIL_TRANSINFO_GetPos(pWork->numFrameBg) | (0x1000 * PALOFS_NUM_FRAME);
      }

      GFL_BG_WriteScreen(
        GFL_BG_FRAME3_M, &scrnData->rawData,_WINNUM_SCR_INITX,_WINNUM_SCR_INITY,
        scrnData->screenWidth/8,scrnData->screenHeight/8);
      GFL_BG_LoadScreenV_Req(GFL_BG_FRAME3_M);
    }
    GFL_HEAP_FreeMemory( arcData );

  }

  GFL_ARC_CloseDataHandle(p_handle);

}

//-----------------------------------------------------------------------------
/**
 *  @brief  数値入力 表示更新
 *
 *  @param  pWork
 *  @param  num
 *
 *  @retval
 */
//-----------------------------------------------------------------------------
void ITEMDISP_InputNumDisp(FIELD_ITEMMENU_WORK* pWork,int num)
{
  u8 backColor = 5;

  GFL_BMP_Clear(GFL_BMPWIN_GetBmp(pWork->winNumFrame.win), backColor );
//  GFL_FONTSYS_SetColor( 0xf, 0xe, backColor );
  GFL_MSG_GetString(  pWork->MsgManager, MSG_ITEM_STR002, pWork->pStrBuf );
  WORDSET_RegisterNumber(pWork->WordSet, 0, num,
                         3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);

  WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//  PRINTSYS_Print( GFL_BMPWIN_GetBmp(pWork->winNumFrame), 0, 0, pWork->pExpStrBuf, pWork->fontHandle);
//  GFL_BMPWIN_MakeTransWindow_VBlank(pWork->winNumFrame.win);
	PrintStr( pWork, &pWork->winNumFrame, pWork->pExpStrBuf, 0, 0, PRINTSYS_LSB_Make(15,14,backColor) );
	PrintScreenTrans( &pWork->winNumFrame );

  // 売却金額表示
  if( pWork->InputMode == BAG_INPUT_MODE_SELL )
  {
    s32 val;
    PRINT_UTIL * win = &pWork->winSellGold;

    // 売値を取得
    val = ITEMMENU_SellPrice( pWork->ret_item, pWork->InputNum, pWork->heapID );

    GFL_BMP_Clear(GFL_BMPWIN_GetBmp(win->win), backColor );
//    GFL_FONTSYS_SetColor( 0xf, 0xe, backColor );
    GFL_MSG_GetString(  pWork->MsgManager, mes_shop_100, pWork->pStrBuf );

    WORDSET_RegisterNumber(pWork->WordSet, 0, val,
                           7, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT);

    WORDSET_ExpandStr( pWork->WordSet, pWork->pExpStrBuf, pWork->pStrBuf  );
//    PRINTSYS_Print( GFL_BMPWIN_GetBmp(win), 0, 0, pWork->pExpStrBuf, pWork->fontHandle);
//    GFL_BMPWIN_MakeTransWindow_VBlank(win->win);
		PrintStr( pWork, win, pWork->pExpStrBuf, 0, 0, PRINTSYS_LSB_Make(15,14,backColor) );
		PrintScreenTrans( win );
  }
}




//--------------------------------------------------------------------------------------------
//	↓ここから中村追加分
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
/**
 * @brief		ショートカットＩＤ取得
 */
//--------------------------------------------------------------------------------------------
SHORTCUT_ID ITEMDISP_GetPocketShortcut( int pocketno )
{
	SHORTCUT_ID id;

	switch( pocketno ){
	case BAG_POKE_NORMAL:
		id = SHORTCUT_ID_BAG_ITEM;
		break;
	case BAG_POKE_NUTS:
		id = SHORTCUT_ID_BAG_NUTS;
		break;
	case BAG_POKE_DRUG:
		id = SHORTCUT_ID_BAG_RECOVERY;
		break;
	case BAG_POKE_WAZA:
		id = SHORTCUT_ID_BAG_WAZAMACHINE;
		break;
	case BAG_POKE_EVENT:
		id = SHORTCUT_ID_BAG_IMPORTANT;
		break;
	default:
		GF_ASSERT(0);
	}

	return id;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソル位置の項目ＯＢＪのパレットを切り替える
 */
//--------------------------------------------------------------------------------------------
void ITEMDISP_ChangeCursorPosPalette( FIELD_ITEMMENU_WORK * wk, u32 pal )
{
	GFL_CLACTPOS	cur_pos;
	GFL_CLACTPOS	lst_pos;
	u32	i;

	GFL_CLACT_WK_GetPos( wk->clwkCur, &cur_pos, CLSYS_DEFREND_MAIN );

	for( i=0; i<ITEM_LIST_NUM; i++ ){
		GFL_CLACT_WK_GetPos( wk->listCell[i], &lst_pos, CLSYS_DEFREND_MAIN );
		if( lst_pos.y == cur_pos.y ){
			break;
		}
	}

	GFL_CLACT_WK_SetPlttOffs( wk->listCell[i], pal, CLWK_PLTTOFFS_MODE_OAM_COLOR );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アクティブ/パッシブ切り替え
 */
//--------------------------------------------------------------------------------------------
void ITEMDISP_ChangeActive( FIELD_ITEMMENU_WORK * wk, BOOL flg )
{
	BOOL	shortcut;

	// 二重処理対策
	if( wk->active == flg ){
		return;
	}
	wk->active = flg;

	shortcut = GAMEDATA_GetShortCut( wk->gamedata, ITEMDISP_GetPocketShortcut(wk->pocketno) );

	// アクティブ
	if( flg == TRUE ){
		PaletteFadeReq( wk->pfd, PF_BIT_MAIN_OBJ, _PAL_FADE_OBJ_BIT, 0, 0, 0, 0, GFUser_VIntr_GetTCBSYS() );
		PaletteFadeReq( wk->pfd, PF_BIT_MAIN_BG, 0x0001, 0, 0, 0, 0, GFUser_VIntr_GetTCBSYS() );
	  G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1, 0 );

    // ソートボタン
		ITEMDISP_ChangeSortButton( wk );
    // バーアイコン
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_LEFT],  BAR_ICON_ANM_LEFT );
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_RIGHT], BAR_ICON_ANM_RIGHT );
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_EXIT],  BAR_ICON_ANM_EXIT );
		if( shortcut == TRUE ){
	    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_CHECK_BOX], APP_COMMON_BARICON_CHECK_ON );
		}else{
	    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_CHECK_BOX], APP_COMMON_BARICON_CHECK_OFF );
		}

		ITEMDISP_ChangeRetButtonActive( wk, TRUE );		// 戻るボタンをアクティブへ

	// パッシブ
	}else{
		PaletteWorkSet_VramCopy( wk->pfd, FADE_MAIN_OBJ, 0, FADE_PAL_ALL_SIZE );
		PaletteFadeReq( wk->pfd, PF_BIT_MAIN_OBJ, _PAL_FADE_OBJ_BIT, 0, 8, 8, 0, GFUser_VIntr_GetTCBSYS() );
		PaletteFadeReq( wk->pfd, PF_BIT_MAIN_BG, 0x0001, 0, 8, 8, 0, GFUser_VIntr_GetTCBSYS() );
		//「売る」モードでページ切り替え矢印が表示されていない時
		if( wk->mode == BAG_MODE_SELL && GFL_CLACT_WK_GetDrawEnable( wk->clwkBarIcon[BAR_ICON_LEFT] ) == FALSE ){
			G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG1, -8 );
		}else{
			G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG1, -8 );
		}

    // ソートボタン
		GFL_CLACT_WK_SetAnmSeq( wk->clwkSort, 4 );
    // バーアイコン各種
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_LEFT], BAR_ICON_ANM_LEFT_OFF );
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_RIGHT], BAR_ICON_ANM_RIGHT_OFF );
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_EXIT], BAR_ICON_ANM_EXIT_OFF );
		if( shortcut == TRUE ){
	    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_CHECK_BOX], APP_COMMON_BARICON_CHECK_ON_PASSIVE );
		}else{
	    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_CHECK_BOX], APP_COMMON_BARICON_CHECK_OFF_PASSIVE );
		}
	}
}

void ITEMDISP_ChangeRetButtonActive( FIELD_ITEMMENU_WORK * wk, BOOL flg )
{
	if( flg == TRUE ){
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_RETURN], APP_COMMON_BARICON_RETURN );
	}else{
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_RETURN], APP_COMMON_BARICON_RETURN_OFF );
	}
}

void ITEMDISP_ChangeSortButton( FIELD_ITEMMENU_WORK * wk )
{
	// 技マシン・木の実はソート不可能
	if( wk->pocketno == BAG_POKE_WAZA || wk->pocketno == BAG_POKE_NUTS ||
			ITEMMENU_GetItemPocketNumber( wk ) <= 1 ){
		GFL_CLACT_WK_SetAnmSeq( wk->clwkSort, 4 );
	}else{
		GFL_CLACT_WK_SetAnmSeq( wk->clwkSort, 0 );
	}
}

void ITEMDISP_ChangeMoveModeButton( FIELD_ITEMMENU_WORK * wk, BOOL flg )
{
	BOOL shortcut = GAMEDATA_GetShortCut( wk->gamedata, ITEMDISP_GetPocketShortcut(wk->pocketno) );

	if( flg == TRUE ){
    // ソートボタン
		ITEMDISP_ChangeSortButton( wk );
    // バーアイコン
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_LEFT],  BAR_ICON_ANM_LEFT );
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_RIGHT], BAR_ICON_ANM_RIGHT );
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_EXIT],  BAR_ICON_ANM_EXIT );
		if( shortcut == TRUE ){
	    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_CHECK_BOX], APP_COMMON_BARICON_CHECK_ON );
		}else{
	    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_CHECK_BOX], APP_COMMON_BARICON_CHECK_OFF );
		}
	}else{
    // ソートボタン
		GFL_CLACT_WK_SetAnmSeq( wk->clwkSort, 4 );
    // バーアイコン各種
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_LEFT], BAR_ICON_ANM_LEFT_OFF );
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_RIGHT], BAR_ICON_ANM_RIGHT_OFF );
    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_EXIT], BAR_ICON_ANM_EXIT_OFF );
		if( shortcut == TRUE ){
	    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_CHECK_BOX], APP_COMMON_BARICON_CHECK_ON_PASSIVE );
		}else{
	    GFL_CLACT_WK_SetAnmSeq( wk->clwkBarIcon[BAR_ICON_CHECK_BOX], APP_COMMON_BARICON_CHECK_OFF_PASSIVE );
		}
	}
}

BOOL ITEMDISP_InitBagMode( FIELD_ITEMMENU_WORK * wk )
{
	GFL_CLACTPOS	pos;
	u32	i;

	for( i=0; i<BAG_POKE_MAX; i++ ){
		GFL_CLACT_WK_GetPos( wk->clwkBag[i], &pos, CLWK_SETSF_NONE );
		if( pos.x == pokectCellPos[i].x ){
			return FALSE;
		}
    pos.x += BAG_OBJ_MOVE_CNT;
		GFL_CLACT_WK_SetPos( wk->clwkBag[i], &pos, CLWK_SETSF_NONE );
	}
	return TRUE;
}


static void PrintStr(
							FIELD_ITEMMENU_WORK * wk, PRINT_UTIL * util,
							const STRBUF * str, u16 x, u16 y, PRINTSYS_LSB color )
{
	PRINT_UTIL_PrintColor( util, wk->SysMsgQue, x, y, str, wk->fontHandle, color );
}

static void PrintScreenTrans( PRINT_UTIL * util )
{
	GFL_BMPWIN_MakeScreen( util->win );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(util->win) );
}

void ITEMDISP_PrintUtilTrans( FIELD_ITEMMENU_WORK * wk )
{
	PRINTSYS_QUE_Main( wk->SysMsgQue );

	PRINT_UTIL_Trans( &wk->winWaza, wk->SysMsgQue );
	PRINT_UTIL_Trans( &wk->winItemName, wk->SysMsgQue );
	PRINT_UTIL_Trans( &wk->winItemNum, wk->SysMsgQue );
	PRINT_UTIL_Trans( &wk->winItemReport, wk->SysMsgQue );
	PRINT_UTIL_Trans( &wk->winNumFrame, wk->SysMsgQue );
	PRINT_UTIL_Trans( &wk->winGoldCap, wk->SysMsgQue );
	PRINT_UTIL_Trans( &wk->winGold, wk->SysMsgQue );
	PRINT_UTIL_Trans( &wk->winSellGold, wk->SysMsgQue );
	PRINT_UTIL_Trans( &wk->winPocketNone, wk->SysMsgQue );
	PRINT_UTIL_Trans( &wk->itemInfoDispWin, wk->SysMsgQue );
	PRINT_UTIL_Trans( &wk->pocketNameWin, wk->SysMsgQue );
}
