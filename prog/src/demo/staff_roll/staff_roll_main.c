//============================================================================================
/**
 * @file		staff_roll_main.c
 * @brief		エンディング・スタッフロール画面
 * @author	Hiroyuki Nakamura
 * @date		10.04.19
 *
 *	モジュール名：SRMAIN
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/main.h"
#include "system/wipe.h"
#include "system/gfl_use.h"

#include "font/font.naix"
#include "message.naix"
#include "msg/msg_staff_list_jp.h"
#include "msg/msg_staff_list_eng.h"

#include "staff_roll_main.h"
#include "staff_roll.naix"


//============================================================================================
//============================================================================================
enum {
	MAINSEQ_INIT = 0,
	MAINSEQ_WIPE,
	MAINSEQ_RELEASE,

	MAINSEQ_MAIN,

	MAINSEQ_END,
};

enum {
	SUBSEQ_INIT = 0,
	SUBSEQ_WAIT,

	// ここから一括表示
	SUBSEQ_PUT,

	// ここから一括表示クリア
	SUBSEQ_CLEAR,

	SUBSEQ_END,
};


#if	PM_VERSION == VERSION_BLACK
/*
#define	BG_COLOR		( 0 )														// バックグラウンドカラー
#define	FCOL_WP00V	( PRINTSYS_MACRO_LSB(15,2,0) )	// フォントカラー：デフォルト
*/
#define	BG_COLOR		( 0x7fff )											// バックグラウンドカラー
#define	FCOL_WP00V	( PRINTSYS_MACRO_LSB(1,2,0) )		// フォントカラー：デフォルト
#else
#define	BG_COLOR		( 0x7fff )											// バックグラウンドカラー
#define	FCOL_WP00V	( PRINTSYS_MACRO_LSB(1,2,0) )		// フォントカラー：デフォルト
#endif
#define	FCOL_WP00R	( PRINTSYS_MACRO_LSB(3,4,0) )		// フォントカラー：赤
#define	FCOL_WP00B	( PRINTSYS_MACRO_LSB(5,6,0) )		// フォントカラー：青
#define	FCOL_WP00Y	( PRINTSYS_MACRO_LSB(7,8,0) )		// フォントカラー：黄
#define	FCOL_WP00G	( PRINTSYS_MACRO_LSB(9,10,0) )	// フォントカラー：緑
#define	FCOL_WP00O	( PRINTSYS_MACRO_LSB(11,12,0) )	// フォントカラー：オレンジ
#define	FCOL_WP00P	( PRINTSYS_MACRO_LSB(13,14,0) )	// フォントカラー：ピンク


#define	MBG_PAL_FONT	( 15 )
#define	SBG_PAL_FONT	( 15 )


#define	ITEMLIST_MSG_NONE		( 0xffff )		// メッセージなし

// ラベル
enum {
	ITEMLIST_LABEL_NONE = 0,
	ITEMLIST_LABEL_STR_PUT,
	ITEMLIST_LABEL_STR_CLEAR,
	ITEMLIST_LABEL_SCROLL_START,
	ITEMLIST_LABEL_SCROLL_STOP,
	ITEMLIST_LABEL_END,

	ITEMLIST_LABEL_MAX,
};

// 描画位置
enum {
	STR_PUT_MODE_LEFT = 0,		// 左詰め
	STR_PUT_MODE_RIGHT,				// 右詰め
	STR_PUT_MODE_CENTER,			// 中央
};


typedef int (*pCOMM_FUNC)(SRMAIN_WORK*,ITEMLIST_DATA*);


//============================================================================================
//============================================================================================
static int MainSeq_Init( SRMAIN_WORK * wk );
static int MainSeq_Wipe( SRMAIN_WORK * wk );
static int MainSeq_Release( SRMAIN_WORK * wk );
static int MainSeq_Main( SRMAIN_WORK * wk );

static void InitVram(void);
static void InitBg(void);
static void ExitBg(void);
static void LoadBgGraphic(void);
static void InitVBlank( SRMAIN_WORK * wk );
static void ExitVBlank( SRMAIN_WORK * wk );
static void InitMsg( SRMAIN_WORK * wk );
static void ExitMsg( SRMAIN_WORK * wk );

static void CreateListData( SRMAIN_WORK * wk );
static void DeleteListData( SRMAIN_WORK * wk );

static void InitBmp( SRMAIN_WORK * wk );
static void ExitBmp( SRMAIN_WORK * wk );
static void TransBmpWinChar( SRMAIN_WORK * wk );

static int SetFadeIn( SRMAIN_WORK * wk, int next );
static int SetFadeOut( SRMAIN_WORK * wk, int next );

static void ListScroll( SRMAIN_WORK * wk );

static int CreateItemData( SRMAIN_WORK * wk );

static BOOL PutStr( SRMAIN_WORK * wk );
static BOOL ClearStr( SRMAIN_WORK * wk );


static BOOL Comm_LabelNone( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_LabelStrPut( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_LabelClear( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_LabelScrollStart( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_LabelScrollStop( SRMAIN_WORK * wk, ITEMLIST_DATA * item );
static BOOL Comm_LabelEnd( SRMAIN_WORK * wk, ITEMLIST_DATA * item );


FS_EXTERN_OVERLAY(ui_common);


//============================================================================================
//============================================================================================
static const pSRMAIN_FUNC	MainSeq[] = {
	MainSeq_Init,
	MainSeq_Wipe,
	MainSeq_Release,

	MainSeq_Main,
};

// VRAM割り振り
static const GFL_DISP_VRAM VramTbl = {
	GX_VRAM_BG_128_A,							// メイン2DエンジンのBG
	GX_VRAM_BGEXTPLTT_NONE,				// メイン2DエンジンのBG拡張パレット

	GX_VRAM_SUB_BG_128_C,					// サブ2DエンジンのBG
	GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット

	GX_VRAM_OBJ_128_B,						// メイン2DエンジンのOBJ
	GX_VRAM_OBJEXTPLTT_NONE,			// メイン2DエンジンのOBJ拡張パレット

	GX_VRAM_SUB_OBJ_128_D,				// サブ2DエンジンのOBJ
	GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット

	GX_VRAM_TEX_NONE,							// テクスチャイメージスロット
	GX_VRAM_TEXPLTT_NONE,					// テクスチャパレットスロット

	GX_OBJVRAMMODE_CHAR_1D_128K,	// メインOBJマッピングモード
	GX_OBJVRAMMODE_CHAR_1D_128K		// サブOBJマッピングモード
};

// フォントカラーテーブル
static const PRINTSYS_LSB FontColorTbl[] = {
	FCOL_WP00V,		// フォントカラー：デフォルト
	FCOL_WP00R,		// フォントカラー：赤
	FCOL_WP00B,		// フォントカラー：青
	FCOL_WP00Y,		// フォントカラー：黄
	FCOL_WP00G,		// フォントカラー：緑
	FCOL_WP00O,		// フォントカラー：オレンジ
	FCOL_WP00P,		// フォントカラー：ピンク
};

// コマンドテーブル
static const pCOMM_FUNC CommFunc[] = {
	Comm_LabelNone,					// ラベル：なし
	Comm_LabelStrPut,				// ラベル：文字列一括表示
	Comm_LabelClear,				// ラベル：表示クリア
	Comm_LabelScrollStart,	// ラベル：スクロール開始
	Comm_LabelScrollStop,		// ラベル：スクロール停止
	Comm_LabelEnd,					// ラベル：終了
};




//============================================================================================
//	シーケンス
//============================================================================================

BOOL SRMAIN_Main( SRMAIN_WORK * wk )
{
	wk->mainSeq = MainSeq[wk->mainSeq]( wk );
	if( wk->mainSeq == MAINSEQ_END ){
		return FALSE;
	}

	return TRUE;
}

static int MainSeq_Init( SRMAIN_WORK * wk )
{
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common) );

	// 表示初期化
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );
	// ブレンド初期化
	G2_BlendNone();
	G2S_BlendNone();
	// 上画面をメインに
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_MAIN );
	// 輝度を最低にしておく
	GX_SetMasterBrightness( -16 );
	GXS_SetMasterBrightness( -16 );

	InitVram();
	InitBg();
	LoadBgGraphic();
	InitMsg( wk );
	InitBmp( wk );

	CreateListData( wk );

	InitVBlank( wk );

	return SetFadeIn( wk, MAINSEQ_MAIN );
}

static int MainSeq_Wipe( SRMAIN_WORK * wk )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		return wk->nextSeq;
	}
	return MAINSEQ_WIPE;
}

static int MainSeq_Release( SRMAIN_WORK * wk )
{
	ExitVBlank( wk );

	DeleteListData( wk );

	ExitBmp( wk );
	ExitMsg( wk );
	ExitBg();

	// 輝度を最低にしておく
	GX_SetMasterBrightness( -16 );
	GXS_SetMasterBrightness( -16 );
	// ブレンド初期化
	G2_BlendNone();
	G2S_BlendNone();
	// 表示初期化
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );

	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common) );

	return MAINSEQ_END;
}

static int MainSeq_Main( SRMAIN_WORK * wk )
{
#ifdef	PM_DEBUG
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B ){
		return SetFadeOut( wk, MAINSEQ_RELEASE );
	}
	// デバッグ用一時停止
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A ){
		wk->debugStopFlg ^= 1;
	}
	if( wk->debugStopFlg == TRUE ){
		return MAINSEQ_MAIN;
	}
#endif

	if( wk->subSeq == SUBSEQ_INIT ){
		while( 1 ){
			ITEMLIST_DATA * item = &wk->list[wk->listPos];
			OS_Printf( "[%d] : label = %d, msg = %d\n", wk->listPos, item->label, item->msgIdx );
			if( CommFunc[item->label]( wk, item ) == TRUE ){
				wk->listPos++;
				break;
			}
			wk->listPos++;
		}
	}

	switch( wk->subSeq ){
	case SUBSEQ_WAIT:
		if( wk->listWait == 0 ){
			wk->subSeq = SUBSEQ_INIT;
		}else{
			wk->listWait--;
		}
		break;

	case SUBSEQ_PUT:
		if( PutStr( wk ) == FALSE ){
			wk->subSeq = SUBSEQ_INIT;
		}
		break;

	case SUBSEQ_CLEAR:
		if( ClearStr( wk ) == FALSE ){
			wk->subSeq = SUBSEQ_INIT;
		}
		break;

	case SUBSEQ_END:
		return SetFadeOut( wk, MAINSEQ_RELEASE );
	}

	ListScroll( wk );
//	TransBmpWinChar( wk );

	return MAINSEQ_MAIN;
}


//============================================================================================
//	初期化
//============================================================================================

static void InitVram(void)
{
	GFL_DISP_ClearVRAM( 0 );
	GFL_DISP_SetBank( &VramTbl );
}

static void InitBg(void)
{
	GFL_BG_Init( HEAPID_STAFF_ROLL );

	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

	{	// メイン画面：文字面
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &cnth, GFL_BG_MODE_TEXT );
	}

	{	// サブ画面：文字面
		GFL_BG_BGCNT_HEADER cnth= {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &cnth, GFL_BG_MODE_TEXT );
	}

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_ON );
}

static void ExitBg(void)
{
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_BG1, VISIBLE_OFF);

	GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );

	GFL_BG_Exit();
}

static void LoadBgGraphic(void)
{
	// バックグラウンドカラー
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME1_M, BG_COLOR );
	GFL_BG_SetBackGroundColor( GFL_BG_FRAME1_S, BG_COLOR );

	// フォントパレット
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr,
		PALTYPE_MAIN_BG, MBG_PAL_FONT*0x20, 0x20, HEAPID_STAFF_ROLL );
	GFL_ARC_UTIL_TransVramPalette(
		ARCID_FONT, NARC_font_default_nclr,
		PALTYPE_SUB_BG, SBG_PAL_FONT*0x20, 0x20, HEAPID_STAFF_ROLL );
}

static void VBlankTask( GFL_TCB * tcb, void * work )
{
	SRMAIN_WORK * wk = work;

	TransBmpWinChar( wk );
  GFL_BG_VBlankFunc();
	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

static void InitVBlank( SRMAIN_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask, wk, 0 );
}

static void ExitVBlank( SRMAIN_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );
}

static void InitMsg( SRMAIN_WORK * wk )
{
	if( wk->dat->mojiMode == MOJIMODE_HIRAGANA ){
		wk->mman = GFL_MSG_Create(
								GFL_MSG_LOAD_NORMAL,
								ARCID_MESSAGE, NARC_message_staff_list_jp_dat, HEAPID_STAFF_ROLL );
	}else{
		wk->mman = GFL_MSG_Create(
								GFL_MSG_LOAD_NORMAL,
								ARCID_MESSAGE, NARC_message_staff_list_eng_dat, HEAPID_STAFF_ROLL );
	}
  
	wk->wset = WORDSET_Create( HEAPID_STAFF_ROLL );
//	wk->que  = PRINTSYS_QUE_Create( HEAPID_STAFF_ROLL );
	wk->exp  = GFL_STR_CreateBuffer( 64, HEAPID_STAFF_ROLL );

	wk->font[SRMAIN_FONT_NORMAL] = GFL_FONT_Create(
																	ARCID_FONT, NARC_font_large_gftr,
//																GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_STAFF_ROLL );
																	GFL_FONT_LOADTYPE_MEMORY, FALSE, HEAPID_STAFF_ROLL );
	wk->font[SRMAIN_FONT_SMALL] = GFL_FONT_Create(
																	ARCID_FONT, NARC_font_small_gftr,
//																GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_STAFF_ROLL );
																	GFL_FONT_LOADTYPE_MEMORY, FALSE, HEAPID_STAFF_ROLL );
}

static void ExitMsg( SRMAIN_WORK * wk )
{
	GFL_FONT_Delete( wk->font[SRMAIN_FONT_SMALL] );
	GFL_FONT_Delete( wk->font[SRMAIN_FONT_NORMAL] );

	GFL_STR_DeleteBuffer( wk->exp );
//	PRINTSYS_QUE_Delete( wk->que );
	WORDSET_Delete( wk->wset );
	GFL_MSG_Delete( wk->mman );
}

static void CreateListData( SRMAIN_WORK * wk )
{
	if( wk->dat->mojiMode == MOJIMODE_HIRAGANA ){
		wk->list = GFL_ARC_LoadDataAlloc(
								ARCID_STAFF_ROLL, NARC_staff_roll_staff_list_jp_dat, HEAPID_STAFF_ROLL );
	}else{
		wk->list = GFL_ARC_LoadDataAlloc(
								ARCID_STAFF_ROLL, NARC_staff_roll_staff_list_eng_dat, HEAPID_STAFF_ROLL );
	}
/*
	{
		u32	i;

		for( i=0; i<16; i++ ){
			OS_Printf(
				"[%d] : label = %d, ltype = %d, msg = %d, wait = %d, fnt = %d, col = %d, mode = %d, px = %d, ox = %d\n",
				i, wk->list[i].label, wk->list[i].labelType, wk->list[i].msgIdx, wk->list[i].wait,
				wk->list[i].font, wk->list[i].color, wk->list[i].putMode, wk->list[i].px, wk->list[i].offs_x );
		}
	}
*/
}

static void DeleteListData( SRMAIN_WORK * wk )
{
	GFL_HEAP_FreeMemory( wk->list );
}


//============================================================================================
//	ＢＭＰ
//============================================================================================

static void InitBmp( SRMAIN_WORK * wk )
{
	GFL_BMPWIN_Init( HEAPID_STAFF_ROLL );
	wk->util[0].win = GFL_BMPWIN_Create( GFL_BG_FRAME1_M, 0, 0, 32, 26, MBG_PAL_FONT, GFL_BMP_CHRAREA_GET_B );
	wk->util[1].win = GFL_BMPWIN_Create( GFL_BG_FRAME1_S, 0, 0, 32, 26, SBG_PAL_FONT, GFL_BMP_CHRAREA_GET_B );

//	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->util[0].win), 0, 16, 256, 32, 1 );
//	GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->util[1].win), 0, 32, 256, 16, 2 );

/*
	GFL_BMPWIN_MakeScreen( wk->util[0].win );
	GFL_BMPWIN_MakeScreen( wk->util[1].win );

	GFL_BMPWIN_TransVramCharacter( wk->util[0].win );
	GFL_BMPWIN_TransVramCharacter( wk->util[1].win );
*/
	GFL_BMPWIN_MakeTransWindow( wk->util[0].win );
	GFL_BMPWIN_MakeTransWindow( wk->util[1].win );
}

static void ExitBmp( SRMAIN_WORK * wk )
{
	GFL_BMPWIN_Delete( wk->util[0].win );
	GFL_BMPWIN_Delete( wk->util[1].win );
	GFL_BMPWIN_Exit();
}

#define	BMPWIN_TRANS_MAIN_FLAG		( 1 )
#define	BMPWIN_TRANS_SUB_FLAG			( 2 )

static void SetBmpWinTransFlag( SRMAIN_WORK * wk, u32 flg )
{
	wk->bmpTransFlag |= flg;
}

static void TransBmpWinChar( SRMAIN_WORK * wk )
{
	if( wk->bmpTransFlag & BMPWIN_TRANS_MAIN_FLAG ){
		GFL_BMPWIN_TransVramCharacter( wk->util[0].win );
	}
	if( wk->bmpTransFlag & BMPWIN_TRANS_SUB_FLAG ){
		GFL_BMPWIN_TransVramCharacter( wk->util[1].win );
	}
	wk->bmpTransFlag = 0;
}

//============================================================================================
//	その他
//============================================================================================

static int SetFadeIn( SRMAIN_WORK * wk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_STAFF_ROLL );
	wk->nextSeq = next;
	return MAINSEQ_WIPE;
}

static int SetFadeOut( SRMAIN_WORK * wk, int next )
{
	WIPE_SYS_Start(
		WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
		WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC, HEAPID_STAFF_ROLL );
	wk->nextSeq = next;
	return MAINSEQ_WIPE;
}

#define	ITEMLIST_SCROLL_SPEED		( FX32_CONST(1) )

/*
static void ShiftChar( u8 * mchr, u8 * schr, u32 shift )
{
	u32	byte;
	u32	i;

	byte = shift << 2;		// ４バイトごとなので４倍

	GFL_STD_MemCopy( mchr, &schr[32-byte], byte );

	for( i=shift; i<8; i++ ){
		mchr += byte;
		GFL_STD_MemCopy( &mchr[byte], mchr, byte );
	}
}
*/
static void ShiftChar( u32 * mchr, u32 * schr, BOOL loop )
{
	u32	i;

	if( loop == FALSE ){
		schr[7] = 0;
	}else{
		schr[7] = mchr[0];
	}

	for( i=0; i<7; i++ ){
		mchr[i] = mchr[i+1];
	}
}

static void ListScroll( SRMAIN_WORK * wk )
{
	if( wk->listScrollFlg == FALSE ){ return; }

	wk->listScrollCnt += ITEMLIST_SCROLL_SPEED;
	if( wk->listScrollCnt >= FX32_ONE ){
		wk->listScrollCnt -= FX32_ONE;
	}else{
		return;
	}

	// ここでシフト処理
	{
		u8 * mBmp;
		u8 * sBmp;
		u32	p1, p2;
		u16	i, j;

		mBmp = GFL_BMP_GetCharacterAdrs( GFL_BMPWIN_GetBmp(wk->util[0].win) );
		sBmp = GFL_BMP_GetCharacterAdrs( GFL_BMPWIN_GetBmp(wk->util[1].win) );

		// 一列目はループさせるので別処理
		p1 = 0;
		p2 = ( 25 << 5 ) << 5;
		for( j=0; j<32; j++ ){
			ShiftChar( (u32*)&mBmp[p1], (u32*)&sBmp[p2], FALSE );
			ShiftChar( (u32*)&sBmp[p1], (u32*)&mBmp[p2], TRUE );
			p1 += 0x20;
			p2 += 0x20;
		}
		// 二列目以降
		for( i=1; i<26; i++ ){
			p1 = ( i << 5 ) << 5;
			p2 = ( (i-1) << 5 ) << 5;
			for( j=0; j<32; j++ ){
				ShiftChar( (u32*)&mBmp[p1], (u32*)&mBmp[p2], TRUE );
				ShiftChar( (u32*)&sBmp[p1], (u32*)&sBmp[p2], TRUE );
				p1 += 0x20;
				p2 += 0x20;
			}
		}

		SetBmpWinTransFlag( wk, BMPWIN_TRANS_MAIN_FLAG|BMPWIN_TRANS_SUB_FLAG );
	}
}

// 文字描画
static void PrintStr( SRMAIN_WORK * wk, ITEMLIST_DATA * item, GFL_BMP_DATA * bmp, u32 py )
{
	u32	px;

	GFL_MSG_GetString( wk->mman, item->msgIdx, wk->exp );
	// 左詰め
	if( item->putMode == STR_PUT_MODE_LEFT ){
		px = item->px + item->offs_x;
	// 右詰め
	}else if( item->putMode == STR_PUT_MODE_RIGHT ){
		px = item->px + item->offs_x - PRINTSYS_GetStrWidth( wk->exp, wk->font[item->font], 0 );
	// 中央
	}else if( item->putMode == STR_PUT_MODE_CENTER ){
		px = item->px + item->offs_x - ( PRINTSYS_GetStrWidth( wk->exp, wk->font[item->font], 0 ) >> 1 );
	}else{
		// アサート
	}
	PRINTSYS_PrintColor( bmp, px, py, wk->exp, wk->font[item->font], FontColorTbl[item->color] );
}

// 一行文字列作成
static void MakeScrollStr( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	if( item->font == SRMAIN_FONT_NORMAL ){
		PrintStr( wk, item, GFL_BMPWIN_GetBmp(wk->util[1].win), 192 );
	}else{
		PrintStr( wk, item, GFL_BMPWIN_GetBmp(wk->util[1].win), 192+6 );
	}
}

// 一括描画文字列作成
static void MakePutStr( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	GFL_BMP_DATA * bmp;
	u32	py;

	if( item->wait < 256 ){
		bmp = GFL_BMPWIN_GetBmp( wk->util[0].win );
		py  = item->wait;
		wk->putFrame |= BMPWIN_TRANS_MAIN_FLAG;
	}else{
		bmp = GFL_BMPWIN_GetBmp( wk->util[1].win );
		py  = item->wait - 256;
		wk->putFrame |= BMPWIN_TRANS_SUB_FLAG;
	}

	PrintStr( wk, item, bmp, py );
}


static BOOL PutStr( SRMAIN_WORK * wk )
{
	switch( wk->labelSeq ){
	case 0:
		// 輝度下げ
		if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
			G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BD, 0, 16 );
		}
		if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
			G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BD, 0, 16 );
		}
		wk->bmpTransFlag = wk->putFrame;
		wk->labelSeq++;
		break;

	case 1:
		// 輝度上げ
		wk->britness++;
		if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
			G2_ChangeBlendAlpha( wk->britness, 16-wk->britness );
		}
		if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
			G2S_ChangeBlendAlpha( wk->britness, 16-wk->britness );
		}
		if( wk->britness == 16 ){
			if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
				G2_BlendNone();
			}
			if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
				G2S_BlendNone();
			}
			wk->britness = 0;
			wk->labelSeq = 0;
			return FALSE;
		}
		break;
	}
	return TRUE;
}

static BOOL ClearStr( SRMAIN_WORK * wk )
{
	switch( wk->labelSeq ){
	case 0:
		if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
			G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BD, 16, 0 );
		}
		if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
			G2S_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BD, 16, 0 );
		}
		wk->labelSeq++;

	case 1:
		wk->britness++;
		if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
			G2_ChangeBlendAlpha( 16-wk->britness, wk->britness );
		}
		if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
			G2S_ChangeBlendAlpha( 16-wk->britness, wk->britness );
		}
		if( wk->britness == 16 ){
			if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
				GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->util[0].win), 0 );
			}
			if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
				GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->util[1].win), 0 );
			}
			wk->bmpTransFlag = wk->putFrame;
			wk->britness = 0;
			wk->labelSeq++;
		}
		break;

	case 2:
		if( wk->putFrame & BMPWIN_TRANS_MAIN_FLAG ){
			G2_BlendNone();
		}
		if( wk->putFrame & BMPWIN_TRANS_SUB_FLAG ){
			G2S_BlendNone();
		}
		wk->putFrame = 0;
		wk->labelSeq = 0;
		return FALSE;
	}

	return TRUE;
}



//============================================================================================
//	コマンド
//============================================================================================

// ラベル：なし
static BOOL Comm_LabelNone( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	// ウェイト
	if( item->msgIdx == ITEMLIST_MSG_NONE ){
		wk->listWait = item->wait;
		wk->subSeq = SUBSEQ_WAIT;
		return TRUE;
	}

	// 一行文字列
	if( wk->listWait == 0 ){
//		GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->util[1].win), 0, 192, 256, 64, 0 );
		wk->listWait = item->wait;
	}
	MakeScrollStr( wk, item );
	if( item[1].label != ITEMLIST_LABEL_NONE || item[1].wait != 0 ){
		wk->subSeq = SUBSEQ_WAIT;
		return TRUE;
	}

	return FALSE;
}

// 文字列一括表示
static BOOL Comm_LabelStrPut( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	// 表示タイプ設定
	if( wk->subSeq == SUBSEQ_INIT ){
		wk->labelType = item->labelType;
		wk->subSeq = SUBSEQ_PUT;
	}

	// 文字描画
	MakePutStr( wk, item );

	// 次が一括表示じゃないとき
	if( item[1].label != ITEMLIST_LABEL_STR_PUT ){
		return TRUE;
	}

	return FALSE;
}

// 表示クリア
static BOOL Comm_LabelClear( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	wk->labelType = item->labelType;
	wk->subSeq = SUBSEQ_CLEAR;
	return TRUE;
}

// スクロール開始
static BOOL Comm_LabelScrollStart( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
//	OS_Printf( "■スクロール開始\n" );
	wk->listScrollFlg = TRUE;
	return FALSE;
}

// スクロール停止
static BOOL Comm_LabelScrollStop( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
//	OS_Printf( "■スクロール停止\n" );
	wk->listScrollFlg = FALSE;
	return FALSE;
}

// 終了
static BOOL Comm_LabelEnd( SRMAIN_WORK * wk, ITEMLIST_DATA * item )
{
	wk->subSeq = SUBSEQ_END;
	return TRUE;
}
