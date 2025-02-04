//============================================================================================
/**
 * @file		b_bag_bmp.c
 * @brief		戦闘用バッグ画面 BMP関連
 * @author	Hiroyuki Nakamura
 * @date		09.08.25
 */
//============================================================================================
#include <gflib.h>

#include "gamesystem/msgspeed.h"
#include "system/bmp_winframe.h"

#include "msg/msg_b_bag.h"

#include "b_app_tool.h"

#include "b_bag.h"
#include "b_bag_main.h"
#include "b_bag_bmp.h"
#include "b_bag_bmp_def.h"


//============================================================================================
//	定数定義
//============================================================================================

#define	FCOL_S12W		( PRINTSYS_LSB_Make(15,2,0) )	// フォントカラー：白

// ポケット選択ページ：ポケット名表示座標
#define	P1_ZYOUTAI_PY		( 0 )
#define	P1_KAIFUKU2_PY	( 16 )
#define	P1_HP_PY				( 0 )
#define	P1_KAIFUKU3_PY	( 16 )
#define	P1_BATTLE_PY		( 0 )
#define	P1_BALL_PY			( 0 )
#define	P1_LASTITEM_PY	( 0 )		// 最後に使った道具

#define	P2_ITEMNAME_PY	( 8-1 )		// アイテム選択ページのアイテム名表示Y座標
#define	P2_ITEMNUM_PY		( 4 )			// アイテム選択ページの個数表示Y座標
#define	P2_PAGE_NUM_PY	( 4 )			// アイテム選択ページのページ数表示Y座標

// アイテム選択ページのポケット名表示座標
#define	P2_NEXT_PY				( 8 )
#define	P2_POCKET_HP1_PY	( 4 )
#define	P2_POCKET_HP2_PY	( P2_POCKET_HP1_PY+16 )
#define	P2_POCKET_ST1_PY	( 4 )
#define	P2_POCKET_ST2_PY	( P2_POCKET_ST1_PY+16 )
#define	P2_POCKET_BALL_PY	( 12 )
#define	P2_POCKET_BATL_PY	( 12 )

#define	P3_ITEMNAME_PY	( 0 )		// アイテム使用ページのアイテム名表示Y座標
#define	P3_ITEMINFO_PX	( 4 )		// アイテム使用ページのアイテム情報表示X座標
#define	P3_ITEMINFO_PY	( 0 )		// アイテム使用ページのアイテム情報表示Y座標
#define	P3_ITEMNUM_PY		( 0 )		// アイテム使用ページのアイテム数表示Y座標

#define	P3_USE_PY				( 0 )		// 「つかう」表示Y座標


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void BBAG_Page1BmpWrite( BBAG_WORK * wk );
static void BBAG_Page2BmpWrite( BBAG_WORK * wk );
static void BBAG_Page3BmpWrite( BBAG_WORK * wk );


//============================================================================================
//	グローバル
//============================================================================================

// 会話ウィンドウデータ
static const u8 CommBmpData[] = {
	WIN_TALK_FRM, WIN_TALK_PX, WIN_TALK_PY, WIN_TALK_SX, WIN_TALK_SY, WIN_TALK_PAL
};

// ページ１のウィンドウデータ
static const u8 Page1_BmpData[][6] =
{
	// ページ１のウィンドウデータ
	{	// 「HP/PPかいふく」
		WIN_P1_HP_FRM, WIN_P1_HP_PX, WIN_P1_HP_PY, WIN_P1_HP_SX, WIN_P1_HP_SY, WIN_P1_HP_PAL
	},
	{	// 「じょうたいかいふく」
		WIN_P1_ZYOUTAI_FRM, WIN_P1_ZYOUTAI_PX, WIN_P1_ZYOUTAI_PY, WIN_P1_ZYOUTAI_SX, WIN_P1_ZYOUTAI_SY, WIN_P1_ZYOUTAI_PAL
	},
	{	// 「ボール」
		WIN_P1_BALL_FRM, WIN_P1_BALL_PX, WIN_P1_BALL_PY, WIN_P1_BALL_SX, WIN_P1_BALL_SY, WIN_P1_BALL_PAL
	},
	{	// 「せんとうよう」
		WIN_P1_BATTLE_FRM, WIN_P1_BATTLE_PX, WIN_P1_BATTLE_PY, WIN_P1_BATTLE_SX, WIN_P1_BATTLE_SY, WIN_P1_BATTLE_PAL
	},
	{	// 「さいごにつかったどうぐ」
		WIN_P1_LASTITEM_FRM, WIN_P1_LASTITEM_PX, WIN_P1_LASTITEM_PY, WIN_P1_LASTITEM_SX, WIN_P1_LASTITEM_SY, WIN_P1_LASTITEM_PAL
	},

	// ページ２のウィンドウデータ
	{	// 道具名１
		WIN_P2_NAME1_FRM, WIN_P2_NAME1_PX, WIN_P2_NAME1_PY, WIN_P2_NAME1_SX, WIN_P2_NAME1_SY, WIN_P2_NAME1_PAL
	},
	{	// 道具数１
		WIN_P2_NUM1_FRM, WIN_P2_NUM1_PX, WIN_P2_NUM1_PY, WIN_P2_NUM1_SX, WIN_P2_NUM1_SY, WIN_P2_NUM1_PAL
	},
	{	// 道具名２
		WIN_P2_NAME2_FRM, WIN_P2_NAME2_PX, WIN_P2_NAME2_PY, WIN_P2_NAME2_SX, WIN_P2_NAME2_SY, WIN_P2_NAME2_PAL
	},
	{	// 道具数２
		WIN_P2_NUM2_FRM, WIN_P2_NUM2_PX, WIN_P2_NUM2_PY, WIN_P2_NUM2_SX, WIN_P2_NUM2_SY, WIN_P2_NUM2_PAL
	},
	{	// 道具名３
		WIN_P2_NAME3_FRM, WIN_P2_NAME3_PX, WIN_P2_NAME3_PY, WIN_P2_NAME3_SX, WIN_P2_NAME3_SY, WIN_P2_NAME3_PAL
	},
	{	// 道具数３
		WIN_P2_NUM3_FRM, WIN_P2_NUM3_PX, WIN_P2_NUM3_PY, WIN_P2_NUM3_SX, WIN_P2_NUM3_SY, WIN_P2_NUM3_PAL
	},
	{	// 道具名４
		WIN_P2_NAME4_FRM, WIN_P2_NAME4_PX, WIN_P2_NAME4_PY, WIN_P2_NAME4_SX, WIN_P2_NAME4_SY, WIN_P2_NAME4_PAL
	},
	{	// 道具数４
		WIN_P2_NUM4_FRM, WIN_P2_NUM4_PX, WIN_P2_NUM4_PY, WIN_P2_NUM4_SX, WIN_P2_NUM4_SY, WIN_P2_NUM4_PAL
	},
	{	// 道具名５
		WIN_P2_NAME5_FRM, WIN_P2_NAME5_PX, WIN_P2_NAME5_PY, WIN_P2_NAME5_SX, WIN_P2_NAME5_SY, WIN_P2_NAME5_PAL
	},
	{	// 道具数５
		WIN_P2_NUM5_FRM, WIN_P2_NUM5_PX, WIN_P2_NUM5_PY, WIN_P2_NUM5_SX, WIN_P2_NUM5_SY, WIN_P2_NUM5_PAL
	},
	{	// 道具名６
		WIN_P2_NAME6_FRM, WIN_P2_NAME6_PX, WIN_P2_NAME6_PY, WIN_P2_NAME6_SX, WIN_P2_NAME6_SY, WIN_P2_NAME6_PAL
	},
	{	// 道具数６
		WIN_P2_NUM6_FRM, WIN_P2_NUM6_PX, WIN_P2_NUM6_PY, WIN_P2_NUM6_SX, WIN_P2_NUM6_SY, WIN_P2_NUM6_PAL
	},
	{	// 道具名１（スワップ用）
		WIN_P2_NAME1_FRM, WIN_P2_NAME1_PX, WIN_P2_NAME1_PY, WIN_P2_NAME1_SX, WIN_P2_NAME1_SY, WIN_P2_NAME1_PAL
	},
	{	// 道具数１（スワップ用）
		WIN_P2_NUM1_FRM, WIN_P2_NUM1_PX, WIN_P2_NUM1_PY, WIN_P2_NUM1_SX, WIN_P2_NUM1_SY, WIN_P2_NUM1_PAL
	},
	{	// 道具名２（スワップ用）
		WIN_P2_NAME2_FRM, WIN_P2_NAME2_PX, WIN_P2_NAME2_PY, WIN_P2_NAME2_SX, WIN_P2_NAME2_SY, WIN_P2_NAME2_PAL
	},
	{	// 道具数２（スワップ用）
		WIN_P2_NUM2_FRM, WIN_P2_NUM2_PX, WIN_P2_NUM2_PY, WIN_P2_NUM2_SX, WIN_P2_NUM2_SY, WIN_P2_NUM2_PAL
	},
	{	// 道具名３（スワップ用）
		WIN_P2_NAME3_FRM, WIN_P2_NAME3_PX, WIN_P2_NAME3_PY, WIN_P2_NAME3_SX, WIN_P2_NAME3_SY, WIN_P2_NAME3_PAL
	},
	{	// 道具数３（スワップ用）
		WIN_P2_NUM3_FRM, WIN_P2_NUM3_PX, WIN_P2_NUM3_PY, WIN_P2_NUM3_SX, WIN_P2_NUM3_SY, WIN_P2_NUM3_PAL
	},
	{	// 道具名４（スワップ用）
		WIN_P2_NAME4_FRM, WIN_P2_NAME4_PX, WIN_P2_NAME4_PY, WIN_P2_NAME4_SX, WIN_P2_NAME4_SY, WIN_P2_NAME4_PAL
	},
	{	// 道具数４（スワップ用）
		WIN_P2_NUM4_FRM, WIN_P2_NUM4_PX, WIN_P2_NUM4_PY, WIN_P2_NUM4_SX, WIN_P2_NUM4_SY, WIN_P2_NUM4_PAL
	},
	{	// 道具名５（スワップ用）
		WIN_P2_NAME5_FRM, WIN_P2_NAME5_PX, WIN_P2_NAME5_PY, WIN_P2_NAME5_SX, WIN_P2_NAME5_SY, WIN_P2_NAME5_PAL
	},
	{	// 道具数５（スワップ用）
		WIN_P2_NUM5_FRM, WIN_P2_NUM5_PX, WIN_P2_NUM5_PY, WIN_P2_NUM5_SX, WIN_P2_NUM5_SY, WIN_P2_NUM5_PAL
	},
	{	// 道具名６（スワップ用）
		WIN_P2_NAME6_FRM, WIN_P2_NAME6_PX, WIN_P2_NAME6_PY, WIN_P2_NAME6_SX, WIN_P2_NAME6_SY, WIN_P2_NAME6_PAL
	},
	{	// 道具数６（スワップ用）
		WIN_P2_NUM6_FRM, WIN_P2_NUM6_PX, WIN_P2_NUM6_PY, WIN_P2_NUM6_SX, WIN_P2_NUM6_SY, WIN_P2_NUM6_PAL
	},
	{	// ポケット名
		WIN_P2_POCKET_FRM, WIN_P2_POCKET_PX, WIN_P2_POCKET_PY, WIN_P2_POCKET_SX, WIN_P2_POCKET_SY, WIN_P2_POCKET_PAL
	},
	{	// ページ数（アイテム数）
		WIN_P2_PAGENUM_FRM, WIN_P2_PAGENUM_PX, WIN_P2_PAGENUM_PY, WIN_P2_PAGENUM_SX, WIN_P2_PAGENUM_SY, WIN_P2_PAGENUM_PAL
	},

	// ページ３のウィンドウデータ
	{	// 道具名
		WIN_P3_NAME_FRM, WIN_P3_NAME_PX, WIN_P3_NAME_PY, WIN_P3_NAME_SX, WIN_P3_NAME_SY, WIN_P3_NAME_PAL
	},
	{	// 個数
		WIN_P3_NUM_FRM, WIN_P3_NUM_PX, WIN_P3_NUM_PY, WIN_P3_NUM_SX, WIN_P3_NUM_SY, WIN_P3_NUM_PAL
	},
	{	// 説明
		WIN_P3_INFO_FRM, WIN_P3_INFO_PX, WIN_P3_INFO_PY, WIN_P3_INFO_SX, WIN_P3_INFO_SY, WIN_P3_INFO_PAL
	},
	{	// 「つかう」
		WIN_P3_USE_FRM, WIN_P3_USE_PX, WIN_P3_USE_PY, WIN_P3_USE_SX, WIN_P3_USE_SY, WIN_P3_USE_PAL
	}
};

// ページ１
static const u8 P1_PutWin[] = {
	WIN_P1_HP,				// 「HP/PPかいふく」
	WIN_P1_ZYOUTAI,		// 「じょうたいかいふく」
	WIN_P1_BALL,			// 「ボール」
	WIN_P1_BATTLE,		// 「せんとうよう」
	WIN_P1_LASTITEM,	// 「さいごにつかったどうぐ」
	BAPPTOOL_SET_STR_SCRN_END
};

// ページ２
static const u8 P2_PutWin[] = {
	WIN_P2_NAME1,			// 道具名１
	WIN_P2_NUM1,			// 道具数１
	WIN_P2_NAME2,			// 道具名２
	WIN_P2_NUM2,			// 道具数２
	WIN_P2_NAME3,			// 道具名３
	WIN_P2_NUM3,			// 道具数３
	WIN_P2_NAME4,			// 道具名４
	WIN_P2_NUM4,			// 道具数４
	WIN_P2_NAME5,			// 道具名５
	WIN_P2_NUM5,			// 道具数５
	WIN_P2_NAME6,			// 道具名６
	WIN_P2_NUM6,			// 道具数６
	WIN_P2_POCKET,		// ポケット名
	WIN_P2_PAGENUM,		// ページ数（アイテム数）
	BAPPTOOL_SET_STR_SCRN_END
};

// ページ２（スワップ用）
static const u8 P2S_PutWin[] = {
	WIN_P2_NAME1_S,		// 道具名１（スワップ用）
	WIN_P2_NUM1_S,		// 道具数１（スワップ用）
	WIN_P2_NAME2_S,		// 道具名２（スワップ用）
	WIN_P2_NUM2_S,		// 道具数２（スワップ用）
	WIN_P2_NAME3_S,		// 道具名３（スワップ用）
	WIN_P2_NUM3_S,		// 道具数３（スワップ用）
	WIN_P2_NAME4_S,		// 道具名４（スワップ用）
	WIN_P2_NUM4_S,		// 道具数４（スワップ用）
	WIN_P2_NAME5_S,		// 道具名５（スワップ用）
	WIN_P2_NUM5_S,		// 道具数５（スワップ用）
	WIN_P2_NAME6_S,		// 道具名６（スワップ用）
	WIN_P2_NUM6_S,		// 道具数６（スワップ用）
	WIN_P2_POCKET,		// ポケット名
	WIN_P2_PAGENUM,		// ページ数（アイテム数）
	BAPPTOOL_SET_STR_SCRN_END
};

// ページ３
static const u8 P3_PutWin[] = {
	WIN_P3_NAME,		// 道具名
	WIN_P3_NUM,			// 個数
	WIN_P3_INFO,		// 説明
	WIN_P3_USE,			// 「つかう」
	BAPPTOOL_SET_STR_SCRN_END
};

// アイテム名のメッセージID
static const u32 ItemStrGmm[][2] =
{	// 名前、x???
	{ mes_b_bag_02_000, mes_b_bag_02_001 },
	{ mes_b_bag_02_100, mes_b_bag_02_101 },
	{ mes_b_bag_02_200, mes_b_bag_02_201 },
	{ mes_b_bag_02_300, mes_b_bag_02_301 },
	{ mes_b_bag_02_400, mes_b_bag_02_401 },
	{ mes_b_bag_02_500, mes_b_bag_02_501 }
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		BMPウィンドウ初期化
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_BmpInit( BBAG_WORK * wk )
{
	wk->talk_win = GFL_BMPWIN_Create(
									CommBmpData[0],
									CommBmpData[1], CommBmpData[2],
									CommBmpData[3], CommBmpData[4],
									CommBmpData[5],
									GFL_BMP_CHRAREA_GET_B );
	BattleBag_BmpAdd( wk, wk->page );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ページごとのBMPウィンドウ追加
 *
 * @param		wk		ワーク
 * @param		page	ページ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_BmpAdd( BBAG_WORK * wk, u32 page )
{
	const u8 * dat;
	u32	i;

	// ポケット選択ページ
	dat = Page1_BmpData[0];
	for( i=0; i<WIN_MAX; i++ ){
		wk->add_win[i].win = GFL_BMPWIN_Create(
													dat[0],
													dat[1], dat[2],
													dat[3], dat[4],
													dat[5],
													GFL_BMP_CHRAREA_GET_B );
		dat += 6;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		追加BMPウィンドウ削除
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_BmpFree( BBAG_WORK * wk )
{
	u32	i;

	for( i=0; i<WIN_MAX; i++ ){
		GFL_BMPWIN_Delete( wk->add_win[i].win );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BMPウィンドウ全削除
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_BmpFreeAll( BBAG_WORK * wk )
{
	u32	i;

	BattleBag_BmpFree( wk );
	GFL_BMPWIN_Delete( wk->talk_win );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BMP書き込み
 *
 * @param		wk		ワーク
 * @param		page	ページ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_BmpWrite( BBAG_WORK * wk, u32 page )
{
	switch( page ){
	case BBAG_PAGE_POCKET:	// ポケット選択ページ
		BBAG_Page1BmpWrite( wk );
		break;
	case BBAG_PAGE_MAIN:	// アイテム選択ページ
		BBAG_Page2BmpWrite( wk );
		break;
	case BBAG_PAGE_ITEM:	// アイテム使用ページ
		BBAG_Page3BmpWrite( wk );
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		文字列表示
 *
 * @param		wk		戦闘バッグのワーク
 * @param		widx	ウィンドウインデックス
 * @param		midx	メッセージインデックス
 * @param		py		表示Y座標
 * @param		col		カラー
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_StrPut( BBAG_WORK * wk, u32 widx, u32 midx, u32 py, u16 col )
{
	GFL_BMPWIN * win;
	STRBUF * str;
	u32	siz;
	u32	px;

	win = wk->add_win[widx].win;
	str = GFL_MSG_CreateString( wk->mman, midx );
	siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
	px = ( GFL_BMPWIN_GetSizeX(win) * 8 - siz ) / 2;
	PRINT_UTIL_PrintColor( &wk->add_win[widx], wk->que, px, py, str, wk->dat->font, col );

	GFL_STR_DeleteBuffer( str );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケット選択ページ表示
 *
 * @param		wk		戦闘バッグのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_Page1BmpWrite( BBAG_WORK * wk )
{
	GFL_BMP_DATA * bmp;
	u32	i;

	for( i=WIN_P1_HP; i<=WIN_P1_LASTITEM; i++ ){
		bmp = GFL_BMPWIN_GetBmp( wk->add_win[i].win );
		GFL_BMP_Clear( bmp, 0 );
	}

	BBAG_StrPut( wk, WIN_P1_HP, mes_b_bag_01_000, P1_HP_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_HP, mes_b_bag_01_001, P1_KAIFUKU3_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_ZYOUTAI, mes_b_bag_01_100, P1_ZYOUTAI_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_ZYOUTAI, mes_b_bag_01_101, P1_KAIFUKU2_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_BALL, mes_b_bag_01_500, P1_BALL_PY, FCOL_S12W );
	BBAG_StrPut( wk, WIN_P1_BATTLE, mes_b_bag_01_400, P1_BATTLE_PY, FCOL_S12W );
	if( wk->used_item != ITEM_DUMMY_DATA ){
		STRBUF * str = GFL_MSG_CreateString( wk->mman, mes_b_bag_01_600 );
		PRINT_UTIL_PrintColor(
			&wk->add_win[WIN_P1_LASTITEM], wk->que, 0, P1_LASTITEM_PY, str, wk->dat->font, FCOL_S12W );
		GFL_STR_DeleteBuffer( str );
	}else{
		GFL_BMPWIN_TransVramCharacter( wk->add_win[WIN_P1_LASTITEM].win );
	}

	wk->putWin = P1_PutWin;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテム名表示
 *
 * @param		wk			戦闘バッグのワーク
 * @param		dat_pos		データ位置（ポケット内のアイテム位置）
 * @param		put_pos		表示位置
 * @param		widx		ウィンドウインデックス
 * @param		fidx		フォントインデックス
 * @param		col			カラー
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_ItemNamePut(
				BBAG_WORK * wk, u32 dat_pos, u32 put_pos, u32 widx, u32 fidx, u32 col )
{
	GFL_BMPWIN * win;
	GFL_BMP_DATA * bmp;
	STRBUF * str;
	u32	siz;
	u32	px;

	win = wk->add_win[widx].win;
	bmp = GFL_BMPWIN_GetBmp( win );
	GFL_BMP_Clear( bmp, 0 );

	if( wk->pocket[wk->poke_id][dat_pos].id != 0 ){
		str = GFL_MSG_CreateString( wk->mman, ItemStrGmm[put_pos][0] );
		WORDSET_RegisterItemName( wk->wset, 0, wk->pocket[wk->poke_id][dat_pos].id );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
		siz = PRINTSYS_GetStrWidth( wk->msg_buf, wk->dat->font, 0 );
		px  = ( GFL_BMPWIN_GetSizeX(win) * 8 - siz ) / 2;
		PRINT_UTIL_PrintColor(
			&wk->add_win[widx], wk->que, px, P2_ITEMNAME_PY, wk->msg_buf, wk->dat->font, FCOL_S12W );
		GFL_STR_DeleteBuffer( str );
	}else{
		GFL_BMPWIN_TransVramCharacter( win );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		個数表示
 *
 * @param		wk				戦闘バッグのワーク
 * @param		dat_pos		データ位置（ポケット内のアイテム位置）
 * @param		put_pos		表示位置
 * @param		widx			ウィンドウインデックス
 * @param		fidx			フォントインデックス
 * @param		py				表示Y座標
 * @param		col				カラー
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_ItemNumPut(
				BBAG_WORK * wk, u32 dat_pos, u32 put_pos, u32 widx, u32 fidx, u32 py, u32 col )
{
	STRBUF * str;
	GFL_BMPWIN * win;
	GFL_BMP_DATA * bmp;

	win = wk->add_win[widx].win;
	bmp = GFL_BMPWIN_GetBmp( win );
	GFL_BMP_Clear( bmp, 0 );

	if( wk->pocket[wk->poke_id][dat_pos].no != 0 && wk->dat->mode != BBAG_MODE_SHOOTER ){
		str = GFL_MSG_CreateString( wk->mman, ItemStrGmm[put_pos][1] );

		WORDSET_RegisterNumber(
			wk->wset, 0, wk->pocket[wk->poke_id][dat_pos].no,
			3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
		WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );

		PRINT_UTIL_PrintColor(
			&wk->add_win[widx], wk->que, 0, py, wk->msg_buf, wk->dat->font, FCOL_S12W );
		GFL_STR_DeleteBuffer( str );
	}else{
		GFL_BMPWIN_TransVramCharacter( win );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテム名と個数表示（個別）
 *
 * @param		wk		戦闘バッグのワーク
 * @param		pos		表示位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_P2_ItemPut( BBAG_WORK * wk, u32 pos )
{
	u32	win_pos;
	u32	dat_pos;

	dat_pos = wk->dat->item_scr[wk->poke_id]*6+pos;

	if( wk->p2_swap == 0 ){
		win_pos = WIN_P2_NAME1;
	}else{
		win_pos = WIN_P2_NAME1_S;
	}

	BBAG_ItemNamePut( wk, dat_pos, pos, win_pos+pos*2, 0, 0 );
	BBAG_ItemNumPut( wk, dat_pos, pos, win_pos+1+pos*2, 0, P2_ITEMNUM_PY, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテム名と個数表示（全体）
 *
 * @param		wk		戦闘バッグのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_Page2_StrItemPut( BBAG_WORK * wk )
{
	u16	i;

	GFL_BG_FillScreen( GFL_BG_FRAME1_S, 0, 0, 0, 32, 19, GFL_BG_SCRWRT_PALIN );

	for( i=0; i<6; i++ ){
		BBAG_P2_ItemPut( wk, i );
	}

	if( wk->p2_swap == 0 ){
		wk->putWin = P2_PutWin;
	}else{
		wk->putWin = P2S_PutWin;
	}

	wk->p2_swap ^= 1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ページ数表示
 *
 * @param		wk		戦闘バッグのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_Page2_StrPageNumPut( BBAG_WORK * wk )
{
	GFL_BMPWIN * win;
	GFL_BMP_DATA * bmp;
	STRBUF * str;
	u32	siz;
	u32	px;

	win = wk->add_win[WIN_P2_PAGENUM].win;
	bmp = GFL_BMPWIN_GetBmp( win );
	GFL_BMP_Clear( bmp, 0 );

	str = GFL_MSG_CreateString( wk->mman, mes_b_bag_02_800 );
	siz = PRINTSYS_GetStrWidth( str, wk->dat->font, 0 );
	px  = ( GFL_BMPWIN_GetSizeX(win) * 8 - siz ) / 2;
	PRINTSYS_PrintQueColor(
			wk->que, bmp, px, P2_PAGE_NUM_PY, str, wk->dat->font, FCOL_S12W );
	GFL_STR_DeleteBuffer( str );

	str = GFL_MSG_CreateString( wk->mman, mes_b_bag_02_801 );
	WORDSET_RegisterNumber(
		wk->wset, 0, wk->scr_max[wk->poke_id]+1,
		2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	PRINTSYS_PrintQueColor(
			wk->que, bmp, px+siz, P2_PAGE_NUM_PY, wk->msg_buf, wk->dat->font, FCOL_S12W );
	GFL_STR_DeleteBuffer( str );

	str = GFL_MSG_CreateString( wk->mman, mes_b_bag_02_802 );
	WORDSET_RegisterNumber(
		wk->wset, 0, wk->dat->item_scr[wk->poke_id]+1,
		2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );

	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );
	siz = PRINTSYS_GetStrWidth( wk->msg_buf, wk->dat->font, 0 );
	PRINT_UTIL_PrintColor(
		&wk->add_win[WIN_P2_PAGENUM], wk->que,
		px-siz, P2_PAGE_NUM_PY, wk->msg_buf, wk->dat->font, FCOL_S12W );
	GFL_STR_DeleteBuffer( str );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケット名表示
 *
 * @param		wk		戦闘バッグのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_Page2PocketNamePut( BBAG_WORK * wk )
{
	GFL_BMP_DATA * bmp;

	bmp = GFL_BMPWIN_GetBmp( wk->add_win[WIN_P2_POCKET].win );
	GFL_BMP_Clear( bmp, 0 );

	if( wk->dat->mode != BBAG_MODE_SHOOTER ){
		switch( wk->poke_id ){
		case BBAG_POKE_HPRCV:	// HP回復ポケット
			BBAG_StrPut(
				wk, WIN_P2_POCKET, mes_b_bag_02_700, P2_POCKET_HP1_PY, FCOL_S12W );
			BBAG_StrPut(
				wk, WIN_P2_POCKET, mes_b_bag_02_701, P2_POCKET_HP2_PY, FCOL_S12W );
			break;
		case BBAG_POKE_STRCV:	// 状態回復ポケット
			BBAG_StrPut(
				wk, WIN_P2_POCKET, mes_b_bag_02_702, P2_POCKET_ST1_PY, FCOL_S12W );
			BBAG_StrPut(
				wk, WIN_P2_POCKET, mes_b_bag_02_703, P2_POCKET_ST2_PY, FCOL_S12W );
			break;
		case BBAG_POKE_BALL:	// ボールポケット
			BBAG_StrPut(
				wk, WIN_P2_POCKET, mes_b_bag_02_704, P2_POCKET_BALL_PY, FCOL_S12W );
			break;
		case BBAG_POKE_BATTLE:	// 戦闘用ポケット
			BBAG_StrPut(
				wk, WIN_P2_POCKET, mes_b_bag_02_705, P2_POCKET_BATL_PY, FCOL_S12W );
			break;
		}
	}else{
		GFL_BMPWIN_TransVramCharacter( wk->add_win[WIN_P2_POCKET].win );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテム選択ページ
 *
 * @param		wk		戦闘バッグのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_Page2BmpWrite( BBAG_WORK * wk )
{
	BattleBag_Page2_StrItemPut( wk );
	BBAG_Page2PocketNamePut( wk );
	BattleBag_Page2_StrPageNumPut( wk );
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテム名表示
 *
 * @param		wk				戦闘バッグのワーク
 * @param		dat_pos		データ位置（ポケット内のアイテム位置）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_P3_ItemNamePut( BBAG_WORK * wk, u32 dat_pos )
{
	GFL_BMPWIN * win;
	GFL_BMP_DATA * bmp;
	STRBUF * str;

	win = wk->add_win[WIN_P3_NAME].win;
	str = GFL_MSG_CreateString( wk->mman, ItemStrGmm[0][0] );
	WORDSET_RegisterItemName( wk->wset, 0, wk->pocket[wk->poke_id][dat_pos].id );
	WORDSET_ExpandStr( wk->wset, wk->msg_buf, str );

	bmp = GFL_BMPWIN_GetBmp( win );
	GFL_BMP_Clear( bmp, 0 );
	PRINT_UTIL_PrintColor(
		&wk->add_win[WIN_P3_NAME], wk->que,
		0, P3_ITEMNAME_PY, wk->msg_buf, wk->dat->font, FCOL_S12W );
	GFL_STR_DeleteBuffer( str );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテム情報表示
 *
 * @param		wk				戦闘バッグのワーク
 * @param		dat_pos		データ位置（ポケット内のアイテム位置）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_P3_ItemInfoPut( BBAG_WORK * wk, u32 dat_pos )
{
	GFL_BMPWIN * win;
	STRBUF * buf;
	
	win = wk->add_win[WIN_P3_INFO].win;
	buf = GFL_STR_CreateBuffer( BUFLEN_ITEM_INFO, wk->dat->heap );
	ITEM_GetInfo( buf, wk->pocket[wk->poke_id][dat_pos].id, wk->dat->heap );
	PRINT_UTIL_PrintColor(
		&wk->add_win[WIN_P3_INFO], wk->que,
		P3_ITEMINFO_PX, P3_ITEMINFO_PY, buf, wk->dat->font, FCOL_S12W );
	GFL_STR_DeleteBuffer( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテム使用ページ
 *
 * @param		wk		戦闘バッグのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BBAG_Page3BmpWrite( BBAG_WORK * wk )
{
	GFL_BMP_DATA * bmp;
	u32	i;
	u32	dat_pos;

	for( i=WIN_P3_NAME; i<=WIN_P3_USE; i++ ){
		bmp = GFL_BMPWIN_GetBmp( wk->add_win[i].win );
		GFL_BMP_Clear( bmp, 0 );
	}

	dat_pos = wk->dat->item_scr[wk->poke_id]*6+wk->dat->item_pos[wk->poke_id];

	BBAG_P3_ItemNamePut( wk, dat_pos );
	BBAG_ItemNumPut( wk, dat_pos, 0, WIN_P3_NUM, 0, P3_ITEMNUM_PY, 0 );
	BBAG_P3_ItemInfoPut( wk, dat_pos );
	BBAG_StrPut( wk, WIN_P3_USE, mes_b_bag_03_000, P3_USE_PY, FCOL_S12W );

	wk->putWin = P3_PutWin;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メッセージ表示
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_TalkMsgSet( BBAG_WORK * wk )
{
	GFL_BMP_DATA * bmp;

	BmpWinFrame_Write( wk->talk_win, WINDOW_TRANS_OFF ,1, BBAG_PAL_TALK_WIN );

	bmp = GFL_BMPWIN_GetBmp( wk->talk_win );
	GFL_BMP_Clear( bmp, 15 );
	BattleBag_TalkMsgStart( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メッセージ表示開始
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattleBag_TalkMsgStart( BBAG_WORK * wk )
{
	GFL_FONTSYS_SetColor( 1, 2, 0 );

	wk->stream = PRINTSYS_PrintStream(
									wk->talk_win,
									0, 0, wk->msg_buf, wk->dat->font,
									MSGSPEED_GetWait(),
									wk->tcbl,
									10,		// tcbl pri
									wk->dat->heap,
									15 );	// clear color

	GFL_BMPWIN_MakeTransWindow_VBlank( wk->talk_win );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		文字列転送
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BBAGBMP_PrintMain( BBAG_WORK * wk )
{
	BAPPTOOL_PrintUtilTrans( wk->add_win, wk->que, WIN_MAX );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		BMPWINスクリーン配置
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BBAGBMP_SetStrScrn( BBAG_WORK * wk )
{
	BAPPTOOL_SetStrScrn( wk->add_win, wk->putWin );
}
