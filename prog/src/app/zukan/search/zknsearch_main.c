//============================================================================================
/**
 * @file		zknsearch_main.c
 * @brief		図鑑検索画面 メイン処理
 * @author	Hiroyuki Nakamura
 * @date		10.02.09
 *
 *	モジュール名：ZKNSEARCHMAIN
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/gfl_use.h"
#include "system/main.h"
#include "print/global_msg.h"
#include "font/font.naix"
#include "app/app_menu_common.h"
#include "msg/msg_zukan_search.h"
#include "message.naix"


#include "zknsearch_main.h"
#include "zknsearch_bmp.h"
#include "zknsearch_obj.h"
#include "zukan_gra.naix"


//============================================================================================
//	定数定義
//============================================================================================
#define	EXP_BUF_SIZE	( 128 )		// テンポラリメッセージバッファサイズ

#define	BLEND_EV1		( 6 )				// ブレンド係数
#define	BLEND_EV2		( 10 )			// ブレンド係数

#define	MENU_BLEND_EV1	( 16 )	// ブレンド係数
#define	MENU_BLEND_EV2	( 6 )		// ブレンド係数

// タッチバー座標
#define	TOUCH_BAR_PX		( 0 )
#define	TOUCH_BAR_PY		( 21 )
#define	TOUCH_BAR_SX		( 32 )
#define	TOUCH_BAR_SY		( 3 )

// 検索中ウィンドウ座標
#define	LOAD_WIN_PX		( 0 )
#define	LOAD_WIN_PY		( 17 )
#define	LOAD_WIN_SX		( 32 )
#define	LOAD_WIN_SY		( 6 )

#define	FRAME_SCROLL_COUNT	( 48 )		// タブフレームのスクロールカウント


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void VBlankTask( GFL_TCB * tcb, void * work );
static void HBlankTask( GFL_TCB * tcb, void * work );


//============================================================================================
//	グローバル
//============================================================================================

// VRAM割り振り
static const GFL_DISP_VRAM VramTbl = {
	GX_VRAM_BG_128_A,							// メイン2DエンジンのBG
	GX_VRAM_BGEXTPLTT_NONE,				// メイン2DエンジンのBG拡張パレット

	GX_VRAM_SUB_BG_128_C,					// サブ2DエンジンのBG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット

	GX_VRAM_OBJ_128_B,						// メイン2DエンジンのOBJ
	GX_VRAM_OBJEXTPLTT_NONE,			// メイン2DエンジンのOBJ拡張パレット

	GX_VRAM_SUB_OBJ_128_D,					// サブ2DエンジンのOBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,		// サブ2DエンジンのOBJ拡張パレット

	GX_VRAM_TEX_NONE,							// テクスチャイメージスロット
	GX_VRAM_TEXPLTT_NONE,					// テクスチャパレットスロット

	GX_OBJVRAMMODE_CHAR_1D_128K,	// メインOBJマッピングモード
	GX_OBJVRAMMODE_CHAR_1D_128K		// サブOBJマッピングモード
};

// タイプリストテーブル
static const u32 SortTypeTbl[] = {
	POKETYPE_AKU,			// あく
	POKETYPE_IWA,			// いわ
	POKETYPE_ESPER,		// エスパー
	POKETYPE_KAKUTOU,	// かくとう
	POKETYPE_KUSA,		// くさ
	POKETYPE_KOORI,		// こおり
	POKETYPE_GHOST,		// ゴースト
	POKETYPE_JIMEN,		// じめん
	POKETYPE_DENKI,		// でんき
	POKETYPE_DOKU,		// どく
	POKETYPE_DRAGON,	// ドラゴン
	POKETYPE_NORMAL,	// ノーマル
	POKETYPE_HAGANE,	// はがね
	POKETYPE_HIKOU,		// ひこう
	POKETYPE_HONOO,		// ほのお
	POKETYPE_MIZU,		// みず
	POKETYPE_MUSHI,		// むし
};

// フォルムリストの検索番号テーブル
static const u8 FormListTable[] = {
	0,		// "丸型"=>0,
	13,		// "腕型"=>13,
	1,		// "脚型"=>1,
	7,		// "多足型"=>7,

	5,		// "四枚羽型"=>5,
	12,		// "腹這脚無型"=>12,
	8,		// "直立胴型"=>8,
	4,		// "四足型"=>4,

	6,		// "集合型"=>6,
	3,		// "昆虫型"=>3,
	2,		// "魚型"=>2,
	9,		// "二足尻尾型"=>9,

	11,		// "二枚羽型"=>11,
	10,		// "二足人型"=>10,
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK関数設定
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_InitVBlank( ZKNSEARCHMAIN_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask, wk, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK関数削除
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ExitVBlank( ZKNSEARCHMAIN_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK処理
 *
 * @param		tcb			GFL_TCB
 * @param		wk			図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankTask( GFL_TCB * tcb, void * work )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	GFL_BG_VBlankFunc();
	GFL_CLACT_SYS_VBlankFunc();

	PaletteFadeTrans( wk->pfd );

	if( wk->bgVanish == 1 ){
		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG1 | GX_PLANEMASK_BG3, VISIBLE_ON );
		GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
		wk->bgVanish = 0;
	}else if( wk->bgVanish == 2 ){
		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG1 | GX_PLANEMASK_BG3, VISIBLE_OFF );
		GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
		wk->bgVanish = 0;
	}

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		HBLANK関数設定
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_InitHBlank( ZKNSEARCHMAIN_WORK * wk )
{
	wk->htask = GFUser_HIntr_CreateTCB( HBlankTask, wk, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		HBLANK関数削除
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ExitHBlank( ZKNSEARCHMAIN_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->htask );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		HBLANK処理
 *
 * @param		tcb			GFL_TCB
 * @param		wk			図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void HBlankTask( GFL_TCB * tcb, void * work )
{
	ZKNSEARCHMAIN_WORK * wk;
	s32	vcount;

	wk = work;

	vcount = GX_GetVCount();

	// 上画面半透明
	if( wk->page == ZKNSEARCHMAIN_PAGE_FORM ){
		if( vcount >= 152 ){
			G2S_ChangeBlendAlpha( 11, 5 );
		}else if( vcount >= 112 ){
			G2S_ChangeBlendAlpha( 10, 6 );
		}else if( vcount >= 72 ){
			G2S_ChangeBlendAlpha( 9, 7 );
		}else if( vcount >= 32 ){
			G2S_ChangeBlendAlpha( 8, 8 );
		}else{
			G2S_ChangeBlendAlpha( 6, 10 );
		}
	}else{
		if( vcount >= 168 ){
			G2S_ChangeBlendAlpha( 11, 5 );
		}else if( vcount >= 144 ){
			G2S_ChangeBlendAlpha( 10, 6 );
		}else if( vcount >= 120 ){
			G2S_ChangeBlendAlpha( 9, 7 );
		}else if( vcount >= 96 ){
			G2S_ChangeBlendAlpha( 8, 8 );
		}else if( vcount >= 72 ){
			G2S_ChangeBlendAlpha( 6, 10 );
		}else{
			G2S_ChangeBlendAlpha( 4, 12 );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  VRAM初期化
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_InitVram(void)
{
	GFL_DISP_ClearVRAM( 0 );
	GFL_DISP_SetBank( &VramTbl );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  VRAM設定データ取得
 *
 * @param		none
 *
 * @return	VRAM設定
 */
//--------------------------------------------------------------------------------------------
const GFL_DISP_VRAM * ZKNSEARCHMAIN_GetVramBankData(void)
{
	return &VramTbl;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧ初期化
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_InitBg(void)
{
	GFL_BG_Init( HEAPID_ZUKAN_SEARCH );

	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

	{	// メイン画面：タッチバー
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// メイン画面：
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME1_M );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_M );
	}
	{	// メイン画面：背景
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x08000, 0x4000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// メイン画面：
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x18000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME3_M );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_M );
	}

	{	// サブ画面：フレーム
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x18000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// サブ画面：リスト
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME1_S );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_S );
	}
	{	// サブ画面：背景
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// サブ画面：文字
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME3_S );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_S );
	}

	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_ON );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧ解放
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ExitBg(void)
{
	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_OFF );

	GFL_BG_FreeBGControl( GFL_BG_FRAME2_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

	GFL_BG_Exit();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧプライオリティ切り替え（メインページ）
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ChangeBgPriorityMenu(void)
{
	GFL_BG_SetPriority( GFL_BG_FRAME0_M, 1 );
	GFL_BG_SetPriority( GFL_BG_FRAME1_M, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧプライオリティ切り替え（各リストページ）
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ChangeBgPriorityList(void)
{
	GFL_BG_SetPriority( GFL_BG_FRAME0_M, 0 );
	GFL_BG_SetPriority( GFL_BG_FRAME1_M, 1 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧグラフィック読み込み
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_LoadBgGraphic(void)
{
	ARCHANDLE * ah;
	GFL_ARCUTIL_TRANSINFO  info;

	ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_zukan_gra_search_search_bgd_NCLR, PALTYPE_MAIN_BG, 0, 0x20*6, HEAPID_ZUKAN_SEARCH );
	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_zukan_gra_search_search_bgu_NCLR, PALTYPE_SUB_BG, 0, 0x20*5, HEAPID_ZUKAN_SEARCH );

	GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
		ah, NARC_zukan_gra_search_searchframe_bgd_NCGR, GFL_BG_FRAME0_M, 0, FALSE, HEAPID_ZUKAN_SEARCH );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_search_searchframe_bgd_NCGR, GFL_BG_FRAME1_M, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_search_searchframe_bgd_NCGR, GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_search_searchbase_bgd_NCGR, GFL_BG_FRAME2_M, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );

	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_search_searchframe_bgd_NCGR, GFL_BG_FRAME1_S, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_search_searchbase_bgu_NCGR, GFL_BG_FRAME2_S, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_search_searchbase_bgu_NCGR, GFL_BG_FRAME0_S, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_search_searchbase_bgd_NSCR,
		GFL_BG_FRAME2_M, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_search_search_main_bgu_NSCR,
		GFL_BG_FRAME0_S, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_search_searchbase_bgu_NSCR,
		GFL_BG_FRAME2_S, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );

	GFL_ARC_CloseDataHandle( ah );

	// タッチバー
	ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_ZUKAN_SEARCH_L );

	info = GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
					ah, APP_COMMON_GetBarCharArcIdx(), GFL_BG_FRAME0_M, 0, FALSE, HEAPID_ZUKAN_SEARCH );
	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, APP_COMMON_GetBarPltArcIdx(), PALTYPE_MAIN_BG,
		ZKNSEARCHMAIN_MBG_PAL_TOUCHBAR*0x20, 0x20, HEAPID_ZUKAN_SEARCH );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, APP_COMMON_GetBarScrnArcIdx(),
		GFL_BG_FRAME0_M, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );
	GFL_BG_ChangeScreenPalette(
		GFL_BG_FRAME0_M, ZKNSEARCHMAIN_TOUCH_BAR_PX, ZKNSEARCHMAIN_TOUCH_BAR_PY,
		ZKNSEARCHMAIN_TOUCH_BAR_SX, ZKNSEARCHMAIN_TOUCH_BAR_SY, ZKNSEARCHMAIN_MBG_PAL_TOUCHBAR );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_M );

	GFL_ARC_CloseDataHandle( ah );

	{	// タッチバー展開
		u16 * scr0;
		u32	i;

		scr0 = GFL_BG_GetScreenBufferAdrs( GFL_BG_FRAME0_M );
		scr0 = &scr0[ TOUCH_BAR_PY*TOUCH_BAR_SX+TOUCH_BAR_PX ];

		for( i=0; i<TOUCH_BAR_SX*TOUCH_BAR_SY; i++ ){
			scr0[i] += info;
		}
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_M );
	}

	// フォントパレット
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
		ZKNSEARCHMAIN_MBG_PAL_FONT*0x20, 0x20, HEAPID_ZUKAN_SEARCH );
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
		ZKNSEARCHMAIN_SBG_PAL_FONT*0x20, 0x20, HEAPID_ZUKAN_SEARCH );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		パレットフェード初期化
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_InitPaletteFade( ZKNSEARCHMAIN_WORK * wk )
{
	wk->pfd = PaletteFadeInit( HEAPID_ZUKAN_SEARCH );

	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_BG, FADE_PAL_ALL_SIZE, HEAPID_ZUKAN_SEARCH );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_SUB_BG, FADE_PAL_ALL_SIZE, HEAPID_ZUKAN_SEARCH );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_OBJ, FADE_PAL_ALL_SIZE, HEAPID_ZUKAN_SEARCH );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_SUB_OBJ, FADE_PAL_ALL_SIZE, HEAPID_ZUKAN_SEARCH );

	PaletteWorkSet_VramCopy( wk->pfd, FADE_MAIN_BG, 0, FADE_PAL_ALL_SIZE );
	PaletteWorkSet_VramCopy( wk->pfd, FADE_SUB_BG, 0, FADE_PAL_ALL_SIZE );
	PaletteWorkSet_VramCopy( wk->pfd, FADE_MAIN_OBJ, 0, FADE_PAL_ALL_SIZE );
	PaletteWorkSet_VramCopy( wk->pfd, FADE_SUB_OBJ, 0, FADE_PAL_ALL_SIZE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		パレットフェード解放
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ExitPaletteFade( ZKNSEARCHMAIN_WORK * wk )
{
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_BG );
	PaletteFadeWorkAllocFree( wk->pfd, FADE_SUB_BG );
	PaletteFadeWorkAllocFree( wk->pfd, FADE_MAIN_OBJ );
	PaletteFadeWorkAllocFree( wk->pfd, FADE_SUB_OBJ );

	PaletteFadeFree( wk->pfd );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		パレットフェードリクエスト
 *
 * @param		wk			図鑑検索画面ワーク
 * @param		start		開始濃度
 * @param		end			終了濃度
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_SetPalFadeSeq( ZKNSEARCHMAIN_WORK * wk, u8 start, u8 end )
{
	PaletteFadeReq( wk->pfd, PF_BIT_MAIN_BG,  0xbfff, ZKNCOMM_FADE_WAIT, start, end, 0, GFUser_VIntr_GetTCBSYS() );
	PaletteFadeReq( wk->pfd, PF_BIT_SUB_BG,   0x7fef, ZKNCOMM_FADE_WAIT, start, end, 0, GFUser_VIntr_GetTCBSYS() );
	PaletteFadeReq( wk->pfd, PF_BIT_MAIN_OBJ, 0xfe1f, ZKNCOMM_FADE_WAIT, start, end, 0, GFUser_VIntr_GetTCBSYS() );
	PaletteFadeReq( wk->pfd, PF_BIT_SUB_OBJ,  0xbfff, ZKNCOMM_FADE_WAIT, start, end, 0, GFUser_VIntr_GetTCBSYS() );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アルファブレンド設定
 *
 * @param		flg		ON/OFF
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_SetBlendAlpha( BOOL flg )
{
	if( flg == TRUE ){
		G2_SetBlendAlpha(
			GX_BLEND_PLANEMASK_BG3,
			GX_BLEND_PLANEMASK_BG2,
			MENU_BLEND_EV1, MENU_BLEND_EV2 );
		G2S_SetBlendAlpha(
			GX_BLEND_PLANEMASK_BG1,
			GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2,
			BLEND_EV1, BLEND_EV2 );
	}else{
		G2S_BlendNone();
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ウィンドウ設定
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_SetWindow(void)
{
	G2S_SetWndOBJInsidePlane(
		GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG2 |
		GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE );
	G2S_SetWndOutsidePlane(
		GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 |
		GX_WND_PLANEMASK_BG2 | GX_WND_PLANEMASK_BG3 | GX_WND_PLANEMASK_OBJ, TRUE );
	GXS_SetVisibleWnd( GX_WNDMASK_OW );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ウィンドウ無効
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ResetWindow(void)
{
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  メッセージ関連初期化
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_InitMsg( ZKNSEARCHMAIN_WORK * wk )
{
	wk->mman = GFL_MSG_Create(
							GFL_MSG_LOAD_NORMAL,
							ARCID_MESSAGE, NARC_message_zukan_search_dat, HEAPID_ZUKAN_SEARCH );
	wk->font = GFL_FONT_Create(
							ARCID_FONT, NARC_font_large_gftr,
							GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_ZUKAN_SEARCH );
	wk->wset = WORDSET_Create( HEAPID_ZUKAN_SEARCH );
	wk->que  = PRINTSYS_QUE_Create( HEAPID_ZUKAN_SEARCH );
	wk->exp  = GFL_STR_CreateBuffer( EXP_BUF_SIZE, HEAPID_ZUKAN_SEARCH );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  メッセージ関連解放
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ExitMsg( ZKNSEARCHMAIN_WORK * wk )
{
	GFL_STR_DeleteBuffer( wk->exp );
	PRINTSYS_QUE_Delete( wk->que );
	WORDSET_Delete( wk->wset );
	GFL_FONT_Delete( wk->font );
	GFL_MSG_Delete( wk->mman );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソルアニメ初期化
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_InitBlinkAnm( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	wk->blink = BLINKPALANM_Create(
								ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR2*16, 16, GFL_BG_FRAME1_M, HEAPID_ZUKAN_SEARCH );

	BLINKPALANM_SetPalBufferArcHDL(
		wk->blink, ah, NARC_zukan_gra_search_search_bgd_NCLR,
		ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR*16, ZKNSEARCHMAIN_MBG_PAL_BUTTON_CUR2*16 );

	GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソルアニメ解放
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ExitBlinkAnm( ZKNSEARCHMAIN_WORK * wk )
{
	BLINKPALANM_Exit( wk->blink );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BGWINFRM初期化
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_InitBgWinFrame( ZKNSEARCHMAIN_WORK * wk )
{
	u16 * scr = GFL_BG_GetScreenBufferAdrs( GFL_BG_FRAME0_M );

	wk->wfrm = BGWINFRM_Create( BGWINFRM_TRANS_VBLANK, 1, HEAPID_ZUKAN_SEARCH );
	BGWINFRM_Add( wk->wfrm, 0, GFL_BG_FRAME0_M, ZKNSEARCHMAIN_TOUCH_BAR_SX, ZKNSEARCHMAIN_TOUCH_BAR_SY );
	BGWINFRM_FrameSet( wk->wfrm, 0, &scr[ZKNSEARCHMAIN_TOUCH_BAR_PY*ZKNSEARCHMAIN_TOUCH_BAR_SX] );

	GFL_BG_FillScreen(
		GFL_BG_FRAME0_M, 0,
		ZKNSEARCHMAIN_TOUCH_BAR_PX, ZKNSEARCHMAIN_TOUCH_BAR_PY,
		ZKNSEARCHMAIN_TOUCH_BAR_SX, ZKNSEARCHMAIN_TOUCH_BAR_SY, 0 );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_M );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BGWINFRM削除
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ExitBgWinFrame( ZKNSEARCHMAIN_WORK * wk )
{
	BGWINFRM_Exit( wk->wfrm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインページＢＧスクリーン読み込み
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_LoadMenuPageScreen( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_search_search_main_bgd_NSCR,
		GFL_BG_FRAME0_M, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_search_search_main2_bgd_NSCR,
		GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );

	GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		並び選択ページＢＧスクリーン読み込み
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_LoadRowListPageScreen( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_search_list_row_bgd_NSCR,
		GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );

	GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		名前選択ページＢＧスクリーン読み込み
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_LoadNameListPageScreen( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_search_list_name_bgd_NSCR,
		GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );

	GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タイプ選択ページＢＧスクリーン読み込み
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_LoadTypeListPageScreen( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_search_list_type_bgd_NSCR,
		GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );

	GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		色選択ページＢＧスクリーン読み込み
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_LoadColorListPageScreen( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_search_list_color_bgd_NSCR,
		GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );

	GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		フォルム選択ページＢＧスクリーン読み込み
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_LoadFormListPageScreen( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_search_list_form_bgd_NSCR,
		GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_ZUKAN_SEARCH );

	GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		検索中ウィンドウ読み込み
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_LoadLoadingWindow( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah;
	NNSG2dScreenData * scrn;
	void * buf;
	u32	siz;

	siz = sizeof(u16) * LOAD_WIN_SX * LOAD_WIN_SY;

	wk->loadingBuff = GFL_HEAP_AllocMemory( HEAPID_ZUKAN_SEARCH, siz );

	ah  = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );
	buf = GFL_ARCHDL_UTIL_LoadScreen(
					ah, NARC_zukan_gra_search_load_win_NSCR, FALSE, &scrn, HEAPID_ZUKAN_SEARCH_L );
  GFL_STD_MemCopy16( (void *)scrn->rawData, wk->loadingBuff, siz );
	GFL_HEAP_FreeMemory( buf );
	GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		検索中ウィンドウ解放
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_UnloadLoadingWindow( ZKNSEARCHMAIN_WORK * wk )
{
	GFL_HEAP_FreeMemory( wk->loadingBuff );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		検索中ウィンドウ表示
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_LoadingWindowOn( ZKNSEARCHMAIN_WORK * wk )
{
	GFL_BG_WriteScreen( GFL_BG_FRAME1_M, wk->loadingBuff, LOAD_WIN_PX, LOAD_WIN_PY, LOAD_WIN_SX, LOAD_WIN_SY );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_M );

	ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN_OFF );
	ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_EXIT, APP_COMMON_BARICON_EXIT_OFF );
	if( GAMEDATA_GetShortCut( wk->dat->gamedata, SHORTCUT_ID_ZUKAN_SEARCH ) == TRUE ){
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, APP_COMMON_BARICON_CHECK_ON_PASSIVE );
	}else{
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, APP_COMMON_BARICON_CHECK_OFF_PASSIVE );
	}
	ZKNSEARCHOBJ_BgPriChange( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, 1 );
	ZKNSEARCHOBJ_BgPriChange( wk, ZKNSEARCHOBJ_IDX_TB_EXIT, 1 );
	ZKNSEARCHOBJ_BgPriChange( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, 1 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		検索中ウィンドウ非表示
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_LoadingWindowOff( ZKNSEARCHMAIN_WORK * wk )
{
	ZKNSEARCHOBJ_SetVanish( wk, ZKNSEARCHOBJ_IDX_LOADING_BAR, FALSE );
	GFL_BG_FillScreen( GFL_BG_FRAME1_M, 0, LOAD_WIN_PX, LOAD_WIN_PY, LOAD_WIN_SX, LOAD_WIN_SY, 0 );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_M );

	ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, APP_COMMON_BARICON_RETURN );
	ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_EXIT, APP_COMMON_BARICON_EXIT );
	if( GAMEDATA_GetShortCut( wk->dat->gamedata, SHORTCUT_ID_ZUKAN_SEARCH ) == TRUE ){
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, APP_COMMON_BARICON_CHECK_ON );
	}else{
		ZKNSEARCHOBJ_SetAutoAnm( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, APP_COMMON_BARICON_CHECK_OFF );
	}
	ZKNSEARCHOBJ_BgPriChange( wk, ZKNSEARCHOBJ_IDX_TB_RETURN, 0 );
	ZKNSEARCHOBJ_BgPriChange( wk, ZKNSEARCHOBJ_IDX_TB_EXIT, 0 );
	ZKNSEARCHOBJ_BgPriChange( wk, ZKNSEARCHOBJ_IDX_TB_Y_BUTTON, 0 );

	ZKNSEARCHBMP_PutResetStart( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストのＢＧ面表示リクエスト
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ListBGOn( ZKNSEARCHMAIN_WORK * wk )
{
	wk->bgVanish = 1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストのＢＧ面非表示リクエスト
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ListBGOff( ZKNSEARCHMAIN_WORK * wk )
{
	wk->bgVanish = 2;

	GFL_BG_ClearScreen( GFL_BG_FRAME1_M );
	GFL_BG_ClearScreen( GFL_BG_FRAME3_M );
	GFL_BG_ClearScreen( GFL_BG_FRAME1_S );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_M );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME3_M );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_S );

	// タッチバーを残してクリア
	GFL_BG_FillScreen( GFL_BG_FRAME0_M, 0, 0, 0, 32, 21, 0 );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_M );

	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_SET, 0 );
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タブＢＧフレーム初期位置設定
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_InitFrameScroll( ZKNSEARCHMAIN_WORK * wk )
{
	s16	x, y;
	u32	i;

	GFL_BG_SetScrollReq( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, FRAME_SCROLL_COUNT );
	GFL_BG_SetScrollReq( GFL_BG_FRAME3_S, GFL_BG_SCROLL_Y_SET, FRAME_SCROLL_COUNT );

	BGWINFRM_FramePut( wk->wfrm, 0, 0, 27 );
	BGWINFRM_MoveMain( wk->wfrm );

	for( i=ZKNSEARCHOBJ_IDX_TB_RETURN; i<=ZKNSEARCHOBJ_IDX_TB_Y_BUTTON; i++ ){
		ZKNSEARCHOBJ_GetPos( wk, i, &x, &y, CLSYS_DRAW_MAIN );
		y += FRAME_SCROLL_COUNT;
		ZKNSEARCHOBJ_SetPos( wk, i, x, y, CLSYS_DRAW_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タブＢＧフレームスクロール設定
 *
 * @param		wk		図鑑検索画面ワーク
 * @param		val		スクロール速度
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_SetFrameScrollParam( ZKNSEARCHMAIN_WORK * wk, s16 val )
{
	wk->frameScrollCnt = FRAME_SCROLL_COUNT;
	wk->frameScrollVal = val;

	BGWINFRM_MoveInit( wk->wfrm, 0, 0, val/GFL_STD_Abs(val), FRAME_SCROLL_COUNT/GFL_STD_Abs(val) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タブＢＧフレームスクロールメイン
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @retval	"TRUE = 処理中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
BOOL ZKNSEARCHMAIN_MainSrameScroll( ZKNSEARCHMAIN_WORK * wk )
{
	s16	x, y;
	u32	i;

	if( wk->frameScrollCnt == 0 ){
		return FALSE;
	}

	BGWINFRM_MoveMain( wk->wfrm );

	GFL_BG_SetScrollReq( GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_INC, wk->frameScrollVal );
	GFL_BG_SetScrollReq( GFL_BG_FRAME3_S, GFL_BG_SCROLL_Y_INC, wk->frameScrollVal );

	for( i=ZKNSEARCHOBJ_IDX_TB_RETURN; i<=ZKNSEARCHOBJ_IDX_TB_Y_BUTTON; i++ ){
		ZKNSEARCHOBJ_GetPos( wk, i, &x, &y, CLSYS_DRAW_MAIN );
		y += wk->frameScrollVal;
		ZKNSEARCHOBJ_SetPos( wk, i, x, y, CLSYS_DRAW_MAIN );
	}

	wk->frameScrollCnt -= GFL_STD_Abs(wk->frameScrollVal);

	return TRUE;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		指定位置のタイプ取得
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	タイプ
 */
//--------------------------------------------------------------------------------------------
u32 ZKNSEARCHMAIN_GetSortType( ZKNSEARCHMAIN_WORK * wk, u32 pos )
{
	return SortTypeTbl[pos];
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タイプからリスト位置を取得
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	リスト位置
 */
//--------------------------------------------------------------------------------------------
u32 ZKNSEARCHMAIN_GetSortTypeIndex( ZKNSEARCHMAIN_WORK * wk, u32 type )
{
	u32	i;

	for( i=0; i<POKETYPE_MAX; i++ ){
		if( SortTypeTbl[i] == type ){
			return i;
		}
	}
	return ZKNCOMM_LIST_SORT_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		フォルムデータからリストインデックスへ変換
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ConvFormDataToList( ZKNSEARCHMAIN_WORK * wk )
{
	u32	i;

	if( wk->dat->sort->form == ZKNCOMM_LIST_SORT_NONE ){
		return;
	}

	for( i=0; i<NELEMS(FormListTable); i++ ){
		if( wk->dat->sort->form == FormListTable[i] ){
			wk->dat->sort->form = i;
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		フォルムリストインデックスからデータへ変換
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHMAIN_ConvFormListToData( ZKNSEARCHMAIN_WORK * wk )
{
	u32	i;

	if( wk->dat->sort->form == ZKNCOMM_LIST_SORT_NONE ){
		return;
	}

	wk->dat->sort->form = FormListTable[wk->dat->sort->form];
}
