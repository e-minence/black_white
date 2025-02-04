//============================================================================================
/**
 * @file		zknlist_main.c
 * @brief		図鑑リスト画面 メイン処理
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	モジュール名：ZKNLISTMAIN
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"
#include "pokeicon/pokeicon.h"
#include "print/global_msg.h"
#include "font/font.naix"
#include "app/app_menu_common.h"
#include "msg/msg_zukan_list.h"
#include "message.naix"

#include "zknlist_main.h"
#include "zknlist_bmp.h"
#include "zknlist_obj.h"
#include "zknlist_snd_def.h"
#include "zukan_gra.naix"
#include "zukan_data.naix"


//============================================================================================
//	定数定義
//============================================================================================
#define	EXP_BUF_SIZE	( 128 )		// テンポラリメッセージバッファサイズ

#define	BLEND_EV1		( 6 )				// ブレンド係数
#define	BLEND_EV2		( 10 )			// ブレンド係数

#define	FRAME_SCROLL_COUNT	( 48 )		// 背景スクロールカウント


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void VBlankTask( GFL_TCB * tcb, void * work );
static void HBlankTask( GFL_TCB * tcb, void * work );

static void ListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void ListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void ListCallBack_Scroll( void * work, s8 mv );


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

// リストコールバック
static const FRAMELIST_CALLBACK	FRMListCallBack = {
	ListCallBack_Draw,
	ListCallBack_Move,
	ListCallBack_Scroll,
};

// リストタッチデータ
static const FRAMELIST_TOUCH_DATA TouchHitTbl[] =
{
	{ {   0,  23, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 00: ポケモンアイコン
	{ {  24,  47, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 01: ポケモンアイコン
	{ {  48,  71, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 02: ポケモンアイコン
	{ {  72,  95, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 03: ポケモンアイコン
	{ {  96, 119, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 04: ポケモンアイコン
	{ { 120, 143, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 05: ポケモンアイコン
	{ { 144, 167, 128, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },		// 06: ポケモンアイコン

	{ {   8, 160, 232, 255 }, FRAMELIST_TOUCH_PARAM_RAIL },					// 07: レール

	{ { 168, 191, 136, 159 }, FRAMELIST_TOUCH_PARAM_PAGE_UP },			// 08: 左
	{ { 168, 191, 160, 183 }, FRAMELIST_TOUCH_PARAM_PAGE_DOWN },		// 09: 右

	{ { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};

// リストヘッダーデータ
static const FRAMELIST_HEADER	ListHeader = {
	GFL_BG_FRAME2_M,							// 下画面ＢＧ
	GFL_BG_FRAME2_S,							// 上画面ＢＧ

	ZKNLISTMAIN_LIST_PX,					// 項目フレーム表示開始Ｘ座標
	ZKNLISTMAIN_LIST_PY,					// 項目フレーム表示開始Ｙ座標
	ZKNLISTMAIN_LIST_SX,					// 項目フレーム表示Ｘサイズ
	ZKNLISTMAIN_LIST_SY,					// 項目フレーム表示Ｙサイズ

	7,							// フレーム内に表示するBMPWINの表示Ｘ座標
	0,							// フレーム内に表示するBMPWINの表示Ｙ座標
	11,							// フレーム内に表示するBMPWINの表示Ｘサイズ
	3,							// フレーム内に表示するBMPWINの表示Ｙサイズ
	1,							// フレーム内に表示するBMPWINのパレット

	{ 24, 12, 8, 6, 4, 3 },		// スクロール速度 [0] = 最速

	3,							// 選択項目のパレット

	8,							// スクロールバーのＹサイズ

	0,							// 項目登録数
	3,							// 背景登録数

	0,												// 初期位置
	ZKNLISTMAIN_LIST_POS_MAX,	// カーソル移動範囲
	0,												// 初期スクロール値

	TouchHitTbl,			// タッチデータ

	&FRMListCallBack,	// コールバック関数
	NULL,
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK関数設定
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitVBlank( ZKNLISTMAIN_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask, wk, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK関数削除
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_ExitVBlank( ZKNLISTMAIN_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK処理
 *
 * @param		tcb			GFL_TCB
 * @param		work		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void VBlankTask( GFL_TCB * tcb, void * work )
{
	ZKNLISTMAIN_WORK * wk = work;

	GFL_BG_VBlankFunc();
	GFL_CLACT_SYS_VBlankFunc();

	PaletteFadeTrans( wk->pfd );

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		HBLANK関数設定
 *
 * @param		wk			図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitHBlank( ZKNLISTMAIN_WORK * wk )
{
	wk->htask = GFUser_HIntr_CreateTCB( HBlankTask, wk, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		HBLANK関数削除
 *
 * @param		wk			図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_ExitHBlank( ZKNLISTMAIN_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->htask );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		HBLANK処理
 *
 * @param		tcb			GFL_TCB
 * @param		work		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void HBlankTask( GFL_TCB * tcb, void * work )
{
	s32	vcount = GX_GetVCount();

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

//--------------------------------------------------------------------------------------------
/**
 * @brief		VRAM初期化
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitVram(void)
{
	GFL_DISP_SetBank( &VramTbl );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		VRAM設定取得
 *
 * @param		none
 *
 * @return	VRAM設定データ
 */
//--------------------------------------------------------------------------------------------
const GFL_DISP_VRAM * ZKNLISTMAIN_GetVramBankData(void)
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
void ZKNLISTMAIN_InitBg(void)
{
	GFL_BG_Init( HEAPID_ZUKAN_LIST );

	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

	{	// メイン画面：カバー？
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME0_M );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 0x20, 0, HEAPID_ZUKAN_LIST );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_M );
	}
	{	// メイン画面：タッチバー
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, 0x4000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// メイン画面：リスト
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME2_M );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_M );
	}
	{	// メイン画面：背景
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x18000, 0x8000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &cnth, GFL_BG_MODE_TEXT );
	}

	{	// サブ画面：カバー？
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_S, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME0_S );
		GFL_BG_SetClearCharacter( GFL_BG_FRAME0_S, 0x20, 0, HEAPID_ZUKAN_LIST );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME0_S );
	}
	{	// サブ画面：フレーム
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x18000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &cnth, GFL_BG_MODE_TEXT );
	}
	{	// サブ画面：リスト
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_S, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME2_S );
		GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );
	}

	{	// サブ画面：背景
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x18000, 0x8000,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &cnth, GFL_BG_MODE_TEXT );
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
void ZKNLISTMAIN_ExitBg(void)
{
	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG0 | GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_OFF );

	GFL_BG_FreeBGControl( GFL_BG_FRAME3_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

	GFL_BG_Exit();
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
void ZKNLISTMAIN_LoadBgGraphic(void)
{
	ARCHANDLE * ah;

	ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_zukan_gra_list_list_bgd_NCLR, PALTYPE_MAIN_BG, 0, 0x20*4, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, NARC_zukan_gra_list_list_bgu_NCLR, PALTYPE_SUB_BG, 0, 0x20*5, HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_list_listframe_bgd_NCGR, GFL_BG_FRAME2_M, 0, 0, FALSE, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_list_listbase_bgd_NCGR, GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_list_listframe_bgu_NCGR, GFL_BG_FRAME2_S, 0, 0, FALSE, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_list_listbase_bgu_NCGR, GFL_BG_FRAME3_S, 0, 0, FALSE, HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_list_listbase_bgd_NSCR,
		GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_list_listtitle_bgu_NSCR,
		GFL_BG_FRAME1_S, 0, 0, FALSE, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_list_listbase_bgu_NSCR,
		GFL_BG_FRAME3_S, 0, 0, FALSE, HEAPID_ZUKAN_LIST );

	GFL_ARC_CloseDataHandle( ah );

	// タッチバー
	ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_ZUKAN_LIST );

	GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan(
		ah, APP_COMMON_GetBarCharArcIdx(), GFL_BG_FRAME1_M, 0, FALSE, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramPalette(
		ah, APP_COMMON_GetBarPltArcIdx(), PALTYPE_MAIN_BG,
		ZKNLISTMAIN_MBG_PAL_TOUCHBAR*0x20, 0x20, HEAPID_ZUKAN_LIST );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, APP_COMMON_GetBarScrnArcIdx(),
		GFL_BG_FRAME1_M, 0, 0, FALSE, HEAPID_ZUKAN_LIST );
	GFL_BG_ChangeScreenPalette(
		GFL_BG_FRAME1_M, ZKNLISTMAIN_TOUCH_BAR_PX, ZKNLISTMAIN_TOUCH_BAR_PY,
		ZKNLISTMAIN_TOUCH_BAR_SX, ZKNLISTMAIN_TOUCH_BAR_SY, ZKNLISTMAIN_MBG_PAL_TOUCHBAR );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME1_M );

	GFL_ARC_CloseDataHandle( ah );

	// フォントパレット
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG,
		ZKNLISTMAIN_MBG_PAL_FONT*0x20, 0x20, HEAPID_ZUKAN_LIST );
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG,
		ZKNLISTMAIN_SBG_PAL_FONT*0x20, 0x20, HEAPID_ZUKAN_LIST );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		パレットフェード初期化
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitPaletteFade( ZKNLISTMAIN_WORK * wk )
{
	wk->pfd = PaletteFadeInit( HEAPID_ZUKAN_LIST );

	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_BG, FADE_PAL_ALL_SIZE, HEAPID_ZUKAN_LIST );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_SUB_BG, FADE_PAL_ALL_SIZE, HEAPID_ZUKAN_LIST );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_MAIN_OBJ, FADE_PAL_ALL_SIZE, HEAPID_ZUKAN_LIST );
	PaletteFadeWorkAllocSet( wk->pfd, FADE_SUB_OBJ, FADE_PAL_ALL_SIZE, HEAPID_ZUKAN_LIST );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		パレットフェード解放
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_ExitPaletteFade( ZKNLISTMAIN_WORK * wk )
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
 * @param		wk			図鑑リストワーク
 * @param		start		開始濃度
 * @param		end			終了濃度
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_SetPalFadeSeq( ZKNLISTMAIN_WORK * wk, u8 start, u8 end )
{
	PaletteFadeReq( wk->pfd, PF_BIT_MAIN_BG,  0xbfff, ZKNCOMM_FADE_WAIT, start, end, 0, GFUser_VIntr_GetTCBSYS() );
	PaletteFadeReq( wk->pfd, PF_BIT_SUB_BG,   0xffef, ZKNCOMM_FADE_WAIT, start, end, 0, GFUser_VIntr_GetTCBSYS() );
	PaletteFadeReq( wk->pfd, PF_BIT_MAIN_OBJ, 0xf878, ZKNCOMM_FADE_WAIT, start, end, 0, GFUser_VIntr_GetTCBSYS() );
	PaletteFadeReq( wk->pfd, PF_BIT_SUB_OBJ,  0xbfff, ZKNCOMM_FADE_WAIT, start, end, 0, GFUser_VIntr_GetTCBSYS() );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		アルファブレンド設定
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_SetBlendAlpha(void)
{
	G2S_SetBlendAlpha(
		GX_BLEND_PLANEMASK_BG2,
		GX_BLEND_PLANEMASK_BG3,
		BLEND_EV1, BLEND_EV2 );
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
void ZKNLISTMAIN_SetWindow(void)
{
	G2S_SetWndOBJInsidePlane(
		GX_WND_PLANEMASK_BG0 | GX_WND_PLANEMASK_BG1 |
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
void ZKNLISTMAIN_ResetWindow(void)
{
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  メッセージ関連初期化
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitMsg( ZKNLISTMAIN_WORK * wk )
{
	wk->mman = GFL_MSG_Create(
							GFL_MSG_LOAD_NORMAL,
							ARCID_MESSAGE, NARC_message_zukan_list_dat, HEAPID_ZUKAN_LIST );
	wk->font = GFL_FONT_Create(
												ARCID_FONT, NARC_font_large_gftr,
												GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_ZUKAN_LIST );
	wk->wset = WORDSET_Create( HEAPID_ZUKAN_LIST );
	wk->que  = PRINTSYS_QUE_Create( HEAPID_ZUKAN_LIST );
	wk->exp  = GFL_STR_CreateBuffer( EXP_BUF_SIZE, HEAPID_ZUKAN_LIST );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  メッセージ関連解放
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_ExitMsg( ZKNLISTMAIN_WORK * wk )
{
	GFL_STR_DeleteBuffer( wk->exp );
	PRINTSYS_QUE_Delete( wk->que );
	WORDSET_Delete( wk->wset );
	GFL_FONT_Delete( wk->font );
	GFL_MSG_Delete( wk->mman );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  地方図鑑番号リスト作成
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_LoadLocalNoList( ZKNLISTMAIN_WORK * wk )
{
	wk->localNo = POKE_PERSONAL_GetZenkokuToChihouArray( HEAPID_ZUKAN_LIST, NULL );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  地方図鑑番号リスト解放
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_FreeLocalNoList( ZKNLISTMAIN_WORK * wk )
{
	GFL_HEAP_FreeMemory( wk->localNo );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  タブフレーム初期位置設定
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_InitFrameScroll( ZKNLISTMAIN_WORK * wk )
{
	s16	x, y;
	u32	i;

	GFL_BG_SetScrollReq( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, FRAME_SCROLL_COUNT );
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_SET, -FRAME_SCROLL_COUNT );

	for( i=ZKNLISTOBJ_IDX_TB_RETURN; i<=ZKNLISTOBJ_IDX_TB_START; i++ ){
		ZKNLISTOBJ_GetPos( wk, i, &x, &y, CLSYS_DRAW_MAIN );
		y += FRAME_SCROLL_COUNT;
		ZKNLISTOBJ_SetPos( wk, i, x, y, CLSYS_DRAW_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  タブフレームスクロール設定
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_SetFrameScrollParam( ZKNLISTMAIN_WORK * wk, s16 val )
{
	wk->frameScrollCnt = FRAME_SCROLL_COUNT;
	wk->frameScrollVal = val;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  タブフレームスクロール
 *
 * @param		wk		図鑑リストワーク
 *
 * @retval	"TRUE = スクロール中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
BOOL ZKNLISTMAIN_MainFrameScroll( ZKNLISTMAIN_WORK * wk )
{
	s16	x, y;
	u32	i;

	if( wk->frameScrollCnt == 0 ){
		return FALSE;
	}

	GFL_BG_SetScrollReq( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_INC, wk->frameScrollVal );
	GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_DEC, wk->frameScrollVal );

	for( i=ZKNLISTOBJ_IDX_TB_RETURN; i<=ZKNLISTOBJ_IDX_TB_START; i++ ){
		ZKNLISTOBJ_GetPos( wk, i, &x, &y, CLSYS_DRAW_MAIN );
		y += wk->frameScrollVal;
		ZKNLISTOBJ_SetPos( wk, i, x, y, CLSYS_DRAW_MAIN );
	}

	wk->frameScrollCnt -= GFL_STD_Abs(wk->frameScrollVal);

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リスト作成
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_MakeList( ZKNLISTMAIN_WORK * wk )
{
	ARCHANDLE * ah;
	u32	i;

	ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_LIST_L );

	// リストワーク作成
	{
		FRAMELIST_HEADER	hed;
		u16	def_mons;

		def_mons = ZUKANSAVE_GetDefaultMons( wk->dat->savedata );

		hed = ListHeader;
		hed.itemMax = MONSNO_END;
		hed.cbWork  = wk;

		for( i=0; i<wk->dat->listMax; i++ ){
			if( wk->dat->list[i] == def_mons ){
				if( i > (wk->dat->listMax-ZKNLISTMAIN_LIST_POS_MAX) ){
					hed.initPos    = i - ( wk->dat->listMax - ZKNLISTMAIN_LIST_POS_MAX );
					hed.initScroll = i - hed.initPos;
				}else if( i < ZKNLISTMAIN_LIST_POS_MAX ){
					hed.initPos    = i;
				}else{
					hed.initScroll = i;
				}
				break;
			}
		}

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_LIST );
	}

	// 表示項目数
	if( wk->dat->listMax < ZKNLISTMAIN_LIST_POS_MAX ){
		wk->listPosMax = wk->dat->listMax;
	}else{
		wk->listPosMax = ZKNLISTMAIN_LIST_POS_MAX;
	}

	// 項目背景設定
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_list_listframe_NSCR, FALSE, 0 );
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_list_listframe2_NSCR, FALSE, 1 );
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_list_listframe3_NSCR, FALSE, 2 );

	// 点滅アニメパレット設定
	FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_zukan_gra_list_list_bgd_NCLR, 2, 3 );

	GFL_ARC_CloseDataHandle( ah );

	// リストパラメータ設定・項目文字列作成
	for( i=0; i<wk->dat->listMax; i++ ){
		if( ZUKANSAVE_GetPokeGetFlag( wk->dat->savedata, wk->dat->list[i] ) == TRUE ){
			STRBUF * srcStr = GFL_MSG_CreateString( GlobalMsg_PokeName, wk->dat->list[i] );
			wk->name[i] = GFL_STR_CreateCopyBuffer( srcStr, HEAPID_ZUKAN_LIST );
			GFL_STR_DeleteBuffer( srcStr );
			FRAMELIST_AddItem( wk->lwk, 0, SET_LIST_PARAM(ZUKAN_LIST_MONS_GET,wk->dat->list[i]) );
		}else if( ZUKANSAVE_GetPokeSeeFlag( wk->dat->savedata, wk->dat->list[i] ) == TRUE ){
			STRBUF * srcStr = GFL_MSG_CreateString( GlobalMsg_PokeName, wk->dat->list[i] );
			wk->name[i] = GFL_STR_CreateCopyBuffer( srcStr, HEAPID_ZUKAN_LIST );
			GFL_STR_DeleteBuffer( srcStr );
			FRAMELIST_AddItem( wk->lwk, 1, SET_LIST_PARAM(ZUKAN_LIST_MONS_SEE,wk->dat->list[i]) );
		}else{
			wk->name[i] = GFL_MSG_CreateString( wk->mman, str_name_01 );
			FRAMELIST_AddItem( wk->lwk, 2, SET_LIST_PARAM(ZUKAN_LIST_MONS_NONE,wk->dat->list[i]) );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リスト削除
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNLISTMAIN_FreeList( ZKNLISTMAIN_WORK * wk )
{
	u32	i;

	FRAMELIST_Exit( wk->lwk );

	// 名前削除
	for( i=0; i<MONSNO_END; i++ ){
		if( wk->name[i] != NULL ){
			GFL_STR_DeleteBuffer( wk->name[i] );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：項目表示
 *
 * @param		work			図鑑リストワーク
 * @param		itemNum		項目番号
 * @param		util			項目で使用しているPRINT_UTIL
 * @param		py				表示Ｙ座標
 * @param		disp			描画面
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNLISTMAIN_WORK * wk;
	u32	prm;
	u32	obj;

	wk = work;

	prm = FRAMELIST_GetItemParam( wk->lwk, itemNum );

	ZKNLISTBMP_PutPokeList( wk, util, wk->name[itemNum], GET_LIST_MONS(prm) );

	if( GET_LIST_INFO(prm) != 0 ){
		ZKNLISTOBJ_PutPokeList( wk, GET_LIST_MONS(prm), py, disp );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：カーソル移動
 *
 * @param		work			図鑑リストワーク
 * @param		listPos		リスト位置
 * @param		flg				FALSE = 初期化時
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
	ZKNLISTMAIN_WORK * wk;
	u32	prm;

	wk = work;

	prm = FRAMELIST_GetItemParam( wk->lwk, listPos );

	if( GET_LIST_INFO(prm) != 0 ){
		ZKNLISTOBJ_SetPokeGra( wk, GET_LIST_MONS(prm) );
	}else{
		ZKNLISTOBJ_SetPokeGra( wk, 0 );
	}
	ZKNLISTOBJ_ChangePokeIconAnime( wk, listPos-FRAMELIST_GetScrollCount(wk->lwk) );
//	ZKNLISTOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY2(wk->lwk) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：リストスクロール
 *
 * @param		work			図鑑リストワーク
 * @param		mv				スクロール値
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ListCallBack_Scroll( void * work, s8 mv )
{
	ZKNLISTMAIN_WORK * wk;

	wk = work;

	ZKNLISTOBJ_PutScrollList( wk, mv*-1 );
	ZKNLISTOBJ_ChangePokeIconAnime( wk, FRAMELIST_GetCursorPos(wk->lwk) );

//	ZKNLISTOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY2(wk->lwk) );
	ZKNLISTOBJ_SetListPageArrowAnime( wk, TRUE );
}
