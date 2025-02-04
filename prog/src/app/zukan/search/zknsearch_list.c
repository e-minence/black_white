//============================================================================================
/**
 * @file		zknsearch_list.c
 * @brief		図鑑検索画面 条件選択リスト処理
 * @author	Hiroyuki Nakamura
 * @date		10.02.10
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/main.h"
#include "msg/msg_zukan_search.h"

#include "zknsearch_main.h"
#include "zknsearch_bmp.h"
#include "zknsearch_obj.h"
#include "zknsearch_list.h"
#include "zukan_gra.naix"


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void RowListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void RowListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void RowListCallBack_Scroll( void * work, s8 mv );

static void NameListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void NameListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void NameListCallBack_Scroll( void * work, s8 mv );

static void TypeListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void TypeListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void TypeListCallBack_Scroll( void * work, s8 mv );

static void ColorListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void ColorListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void ColorListCallBack_Scroll( void * work, s8 mv );

static void FormListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp );
static void FormListCallBack_Move( void * work, u32 listPos, BOOL flg );
static void FormListCallBack_Scroll( void * work, s8 mv );


//============================================================================================
//	グローバル
//============================================================================================

// 並びページタッチデータ
static const FRAMELIST_TOUCH_DATA RowTouchHitTbl[] =
{
	{ {   0,  23, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_ITEM },
	{ {  24,  47, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_ITEM },
	{ {  48,  71, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_ITEM },
	{ {  72,  95, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_ITEM },
	{ {  96, 119, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_ITEM },
	{ { 120, 143, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_ITEM },

	{ { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};
// 並びページコールバック
static const FRAMELIST_CALLBACK	RowListCallBack = {
	RowListCallBack_Draw,
	RowListCallBack_Move,
	RowListCallBack_Scroll,
};
// 並びページリストデータ
static const FRAMELIST_HEADER	RowListHeader = {
	GFL_BG_FRAME1_M,							// 下画面ＢＧ
	GFL_BG_FRAME1_S,							// 上画面ＢＧ

	16,					// 項目フレーム表示開始Ｘ座標
	0,					// 項目フレーム表示開始Ｙ座標
	16,					// 項目フレーム表示Ｘサイズ
	3,					// 項目フレーム表示Ｙサイズ

	3,							// フレーム内に表示するBMPWINの表示Ｘ座標			
	0,							// フレーム内に表示するBMPWINの表示Ｙ座標
	11,							// フレーム内に表示するBMPWINの表示Ｘサイズ
	3,							// フレーム内に表示するBMPWINの表示Ｙサイズ
	1,							// フレーム内に表示するBMPWINのパレット

	{ 24, 12, 8, 6, 4, 3 },		// スクロール速度 [0] = 最速

	3,							// 選択項目のパレット

	8,							// スクロールバーのＹサイズ

	6,							// 項目登録数
	1,							// 背景登録数

	0,							// 初期位置
	6,							// カーソル移動範囲
	0,							// 初期スクロール値

	RowTouchHitTbl,			// タッチデータ

	&RowListCallBack,	// コールバック関数
	NULL,
};

// 名前ページタッチデータ
static const FRAMELIST_TOUCH_DATA NameTouchHitTbl[] =
{
	{ {   0,  23, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  24,  47, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  48,  71, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  72,  95, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  96, 119, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 120, 143, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 144, 167, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },

	{ {   8, 160, 232, 255 }, FRAMELIST_TOUCH_PARAM_RAIL },					// 07: レール

	{ { 168, 191, 184, 207 }, FRAMELIST_TOUCH_PARAM_PAGE_UP },			// 08: 左
	{ { 168, 191, 208, 231 }, FRAMELIST_TOUCH_PARAM_PAGE_DOWN },		// 09: 右

	{ { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};
// 名前ページコールバック
static const FRAMELIST_CALLBACK	NameListCallBack = {
	NameListCallBack_Draw,
	NameListCallBack_Move,
	NameListCallBack_Scroll,
};
// 名前ページリストデータ
static const FRAMELIST_HEADER	NameListHeader = {
	GFL_BG_FRAME1_M,							// 下画面ＢＧ
	GFL_BG_FRAME1_S,							// 上画面ＢＧ

	16,					// 項目フレーム表示開始Ｘ座標
	0,					// 項目フレーム表示開始Ｙ座標
	16,					// 項目フレーム表示Ｘサイズ
	3,					// 項目フレーム表示Ｙサイズ

	3,							// フレーム内に表示するBMPWINの表示Ｘ座標			
	0,							// フレーム内に表示するBMPWINの表示Ｙ座標
	11,							// フレーム内に表示するBMPWINの表示Ｘサイズ
	3,							// フレーム内に表示するBMPWINの表示Ｙサイズ
	1,							// フレーム内に表示するBMPWINのパレット

	{ 24, 12, 8, 6, 4, 3 },		// スクロール速度 [0] = 最速

	3,							// 選択項目のパレット

	8,							// スクロールバーのＹサイズ

	50,							// 項目登録数
	1,							// 背景登録数

	0,							// 初期位置
	7,							// カーソル移動範囲
	0,							// 初期スクロール値

	NameTouchHitTbl,			// タッチデータ

	&NameListCallBack,	// コールバック関数
	NULL,
};

// タイプページタッチデータ
static const FRAMELIST_TOUCH_DATA TypeTouchHitTbl[] =
{
	{ {   0,  23, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  24,  47, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  48,  71, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  72,  95, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  96, 119, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 120, 143, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 144, 167, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },

	{ {   8, 160, 232, 255 }, FRAMELIST_TOUCH_PARAM_RAIL },					// 07: レール

	{ { 168, 191, 184, 207 }, FRAMELIST_TOUCH_PARAM_PAGE_UP },			// 08: 左
	{ { 168, 191, 208, 231 }, FRAMELIST_TOUCH_PARAM_PAGE_DOWN },		// 09: 右

	{ { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};
// タイプページコールバック
static const FRAMELIST_CALLBACK	TypeListCallBack = {
	TypeListCallBack_Draw,
	TypeListCallBack_Move,
	TypeListCallBack_Scroll,
};
// タイプページリストデータ
static const FRAMELIST_HEADER	TypeListHeader = {
	GFL_BG_FRAME1_M,							// 下画面ＢＧ
	GFL_BG_FRAME1_S,							// 上画面ＢＧ

	16,					// 項目フレーム表示開始Ｘ座標
	0,					// 項目フレーム表示開始Ｙ座標
	16,					// 項目フレーム表示Ｘサイズ
	3,					// 項目フレーム表示Ｙサイズ

	3,							// フレーム内に表示するBMPWINの表示Ｘ座標			
	0,							// フレーム内に表示するBMPWINの表示Ｙ座標
	11,							// フレーム内に表示するBMPWINの表示Ｘサイズ
	3,							// フレーム内に表示するBMPWINの表示Ｙサイズ
	1,							// フレーム内に表示するBMPWINのパレット

	{ 24, 12, 8, 6, 4, 3 },		// スクロール速度 [0] = 最速

	3,							// 選択項目のパレット

	8,							// スクロールバーのＹサイズ

	50,							// 項目登録数
	1,							// 背景登録数

	0,							// 初期位置
	7,							// カーソル移動範囲
	0,							// 初期スクロール値

	TypeTouchHitTbl,			// タッチデータ

	&TypeListCallBack,	// コールバック関数
	NULL,
};

// 色ページタッチデータ
static const FRAMELIST_TOUCH_DATA ColorTouchHitTbl[] =
{
	{ {   0,  23, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  24,  47, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  48,  71, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  72,  95, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  96, 119, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 120, 143, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 144, 167, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },

	{ {   8, 160, 232, 255 }, FRAMELIST_TOUCH_PARAM_RAIL },					// 07: レール

	{ { 168, 191, 184, 207 }, FRAMELIST_TOUCH_PARAM_PAGE_UP },			// 08: 左
	{ { 168, 191, 208, 231 }, FRAMELIST_TOUCH_PARAM_PAGE_DOWN },		// 09: 右

	{ { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};
// 色ページコールバックデータ
static const FRAMELIST_CALLBACK	ColorListCallBack = {
	ColorListCallBack_Draw,
	ColorListCallBack_Move,
	ColorListCallBack_Scroll,
};
// 色ページリストデータ
static const FRAMELIST_HEADER	ColorListHeader = {
	GFL_BG_FRAME1_M,							// 下画面ＢＧ
	GFL_BG_FRAME1_S,							// 上画面ＢＧ

	16,					// 項目フレーム表示開始Ｘ座標
	0,					// 項目フレーム表示開始Ｙ座標
	16,					// 項目フレーム表示Ｘサイズ
	3,					// 項目フレーム表示Ｙサイズ

	3,							// フレーム内に表示するBMPWINの表示Ｘ座標			
	0,							// フレーム内に表示するBMPWINの表示Ｙ座標
	11,							// フレーム内に表示するBMPWINの表示Ｘサイズ
	3,							// フレーム内に表示するBMPWINの表示Ｙサイズ
	1,							// フレーム内に表示するBMPWINのパレット

	{ 24, 12, 8, 6, 4, 3 },		// スクロール速度 [0] = 最速

	3,							// 選択項目のパレット

	8,							// スクロールバーのＹサイズ

	50,							// 項目登録数
	1,							// 背景登録数

	0,							// 初期位置
	7,							// カーソル移動範囲
	0,							// 初期スクロール値

	ColorTouchHitTbl,			// タッチデータ

	&ColorListCallBack,	// コールバック関数
	NULL,
};

// フォルムページタッチデータ
static const FRAMELIST_TOUCH_DATA FormTouchHitTbl[] =
{
	{ {   0,  39, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  40,  79, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ {  80, 119, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },
	{ { 120, 159, 128+24, 231 }, FRAMELIST_TOUCH_PARAM_SLIDE },

	{ {   8, 160, 232, 255 }, FRAMELIST_TOUCH_PARAM_RAIL },					// 04: レール

	{ { 168, 191, 184, 207 }, FRAMELIST_TOUCH_PARAM_PAGE_UP },			// 05: 左
	{ { 168, 191, 208, 231 }, FRAMELIST_TOUCH_PARAM_PAGE_DOWN },		// 06: 右

	{ { GFL_UI_TP_HIT_END, 0, 0, 0 }, 0 },
};
// フォルムページコールバック
static const FRAMELIST_CALLBACK	FormListCallBack = {
	FormListCallBack_Draw,
	FormListCallBack_Move,
	FormListCallBack_Scroll,
};
// フォルムページリストデータ
static const FRAMELIST_HEADER	FormListHeader = {
	GFL_BG_FRAME1_M,							// 下画面ＢＧ
	GFL_BG_FRAME1_S,							// 上画面ＢＧ

	16,					// 項目フレーム表示開始Ｘ座標
	0,					// 項目フレーム表示開始Ｙ座標
	16,					// 項目フレーム表示Ｘサイズ
	5,					// 項目フレーム表示Ｙサイズ

	4,							// フレーム内に表示するBMPWINの表示Ｘ座標			
	1,							// フレーム内に表示するBMPWINの表示Ｙ座標
	1,							// フレーム内に表示するBMPWINの表示Ｘサイズ
	1,							// フレーム内に表示するBMPWINの表示Ｙサイズ
	1,							// フレーム内に表示するBMPWINのパレット

	{ 40, 20, 10, 8, 5, 4 },		// スクロール速度 [0] = 最速

	3,							// 選択項目のパレット

	8,							// スクロールバーのＹサイズ

	ZKNSEARCHOBJ_FORM_MAX,	// 項目登録数
	1,											// 背景登録数

	0,							// 初期位置
	4,							// カーソル移動範囲
	0,							// 初期スクロール値

	FormTouchHitTbl,			// タッチデータ

	&FormListCallBack,	// コールバック関数
	NULL,
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		並び選択ページリスト作成
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHLIST_MakeRowList( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	// リストワーク作成
	{
		FRAMELIST_HEADER	hed;

		hed = RowListHeader;
		hed.initPos    = 0;
		hed.initScroll = 0;
		hed.cbWork     = wk;

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_SEARCH );
		wk->listPosMax = hed.posMax;
	}

	// 項目背景設定
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_search_searchframe_NSCR, FALSE, 0 );

	// 点滅アニメパレット設定
	FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_zukan_gra_search_search_bgd_NCLR, 2, 3 );

	GFL_ARC_CloseDataHandle( ah );

	{	// リストパラメータ設定
		u32	i;

		for( i=0; i<6; i++ ){
			FRAMELIST_AddItem( wk->lwk, 0, i );
			wk->item[i] = GFL_MSG_CreateString( wk->mman, ZKN_SEARCH_SORT_01+i );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		名前選択ページリスト作成
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHLIST_MakeNameList( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	// リストワーク作成
	{
		FRAMELIST_HEADER	hed;

		hed = NameListHeader;
		hed.initPos    = 0;
		hed.initScroll = 0;
		hed.cbWork     = wk;

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_SEARCH );
		wk->listPosMax = hed.posMax;
	}

	// 項目背景設定
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_search_searchframe_NSCR, FALSE, 0 );

	// 点滅アニメパレット設定
	FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_zukan_gra_search_search_bgd_NCLR, 2, 3 );

	GFL_ARC_CloseDataHandle( ah );

	{	// リストパラメータ設定
		u32	i;

		for( i=ZKN_SEARCH_INITIAL_01; i<=ZKN_SEARCH_INITIAL_44; i++ ){
			FRAMELIST_AddItem( wk->lwk, 0, i-ZKN_SEARCH_INITIAL_01 );
			wk->item[i-ZKN_SEARCH_INITIAL_01] = GFL_MSG_CreateString( wk->mman, i );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タイプページリスト作成
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHLIST_MakeTypeList( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	// リストワーク作成
	{
		FRAMELIST_HEADER	hed;

		hed = TypeListHeader;
		hed.initPos    = 0;
		hed.initScroll = 0;
		hed.cbWork     = wk;

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_SEARCH );
		wk->listPosMax = hed.posMax;
	}

	// 項目背景設定
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_search_searchframe_NSCR, FALSE, 0 );

	// 点滅アニメパレット設定
	FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_zukan_gra_search_search_bgd_NCLR, 2, 3 );

	GFL_ARC_CloseDataHandle( ah );

	{	// リストパラメータ設定
		u32	i;

		for( i=ZKN_SEARCH_TYPE_01; i<=ZKN_SEARCH_TYPE_17; i++ ){
			FRAMELIST_AddItem( wk->lwk, 0, i-ZKN_SEARCH_TYPE_01 );
			wk->item[i-ZKN_SEARCH_TYPE_01] = GFL_MSG_CreateString( wk->mman, i );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		色ページリスト作成
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHLIST_MakeColorList( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	// リストワーク作成
	{
		FRAMELIST_HEADER	hed;

		hed = ColorListHeader;
		hed.initPos    = 0;
		hed.initScroll = 0;
		hed.cbWork     = wk;

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_SEARCH );
		wk->listPosMax = hed.posMax;
	}

	// 項目背景設定
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_search_searchframe_NSCR, FALSE, 0 );

	// 点滅アニメパレット設定
	FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_zukan_gra_search_search_bgd_NCLR, 2, 3 );

	GFL_ARC_CloseDataHandle( ah );

	{	// リストパラメータ設定
		u32	i;

		for( i=ZKN_SEARCH_COLOR_00; i<=ZKN_SEARCH_COLOR_09; i++ ){
			FRAMELIST_AddItem( wk->lwk, 0, i-ZKN_SEARCH_COLOR_00 );
			wk->item[i-ZKN_SEARCH_COLOR_00] = GFL_MSG_CreateString( wk->mman, i );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		フォルムページリスト作成
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHLIST_MakeFormList( ZKNSEARCHMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_SEARCH_L );

	// リストワーク作成
	{
		FRAMELIST_HEADER	hed;

		hed = FormListHeader;
		hed.initPos    = 0;
		hed.initScroll = 0;
		hed.cbWork     = wk;

		wk->lwk = FRAMELIST_Create( &hed, HEAPID_ZUKAN_SEARCH );
		wk->listPosMax = hed.posMax;
	}

	// 項目背景設定
	FRAMELIST_LoadFrameGraphicAH( wk->lwk, ah, NARC_zukan_gra_search_searchframe2_NSCR, FALSE, 0 );

	// 点滅アニメパレット設定
	FRAMELIST_LoadBlinkPalette( wk->lwk, ah, NARC_zukan_gra_search_search_bgd_NCLR, 2, 3 );

	GFL_ARC_CloseDataHandle( ah );

	{	// リストパラメータ設定
		u32	i;

		for( i=0; i<ZKNSEARCHOBJ_FORM_MAX; i++ ){
			FRAMELIST_AddItem( wk->lwk, 0, i );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リスト削除
 *
 * @param		wk		図鑑検索画面ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void ZKNSEARCHLIST_FreeList( ZKNSEARCHMAIN_WORK * wk )
{
	u32	i;

	FRAMELIST_Exit( wk->lwk );
	wk->lwk = NULL;

	// 項目削除
	for( i=0; i<ZKNSEARCHMAIN_ITEM_MAX; i++ ){
		if( wk->item[i] != NULL ){
			GFL_STR_DeleteBuffer( wk->item[i] );
			wk->item[i] = NULL;
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストコールバック：並びページ項目表示
 *
 * @param		work			図鑑検索画面ワーク
 * @param		itemNum		項目番号
 * @param		util			項目で使用しているPRINT_UTIL
 * @param		py				表示Ｙ座標
 * @param		disp			描画面
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void RowListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHBMP_PutListItem( wk, util, wk->item[itemNum] );

	if( wk->dat->sort->row == itemNum ){
		ZKNSEARCHOBJ_PutMark( wk, 0, py, disp );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：並びページカーソル移動
 *
 * @param		work			図鑑検索画面ワーク
 * @param		listPos		リスト位置
 * @param		flg				FALSE = 初期化時
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void RowListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：並びページリストスクロール
 *
 * @param		work			図鑑検索画面ワーク
 * @param		mv				スクロール値
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void RowListCallBack_Scroll( void * work, s8 mv )
{
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		リストコールバック：名前ページ項目表示
 *
 * @param		work			図鑑検索画面ワーク
 * @param		itemNum		項目番号
 * @param		util			項目で使用しているPRINT_UTIL
 * @param		py				表示Ｙ座標
 * @param		disp			描画面
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void NameListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHBMP_PutListItem( wk, util, wk->item[itemNum] );

	if( wk->dat->sort->name == itemNum ){
		ZKNSEARCHOBJ_PutMark( wk, 0, py, disp );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：名前ページカーソル移動
 *
 * @param		work			図鑑検索画面ワーク
 * @param		listPos		リスト位置
 * @param		flg				FALSE = 初期化時
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void NameListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
	ZKNSEARCHMAIN_WORK * wk = work;

//	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：名前ページリストスクロール
 *
 * @param		work			図鑑検索画面ワーク
 * @param		mv				スクロール値
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void NameListCallBack_Scroll( void * work, s8 mv )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHOBJ_ScrollList( wk, mv*-1 );

//	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
	ZKNSEARCHOBJ_SetListPageArrowAnime( wk, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストコールバック：タイプページ項目表示
 *
 * @param		work			図鑑検索画面ワーク
 * @param		itemNum		項目番号
 * @param		util			項目で使用しているPRINT_UTIL
 * @param		py				表示Ｙ座標
 * @param		disp			描画面
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TypeListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHBMP_PutListItem( wk, util, wk->item[itemNum] );

	if( ZKNSEARCHMAIN_GetSortTypeIndex(wk,wk->dat->sort->type1) == itemNum ){
		ZKNSEARCHOBJ_PutMark( wk, 0, py, disp );
	}
	if( ZKNSEARCHMAIN_GetSortTypeIndex(wk,wk->dat->sort->type2) == itemNum ){
		ZKNSEARCHOBJ_PutMark( wk, 1, py, disp );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：タイプページカーソル移動
 *
 * @param		work			図鑑検索画面ワーク
 * @param		listPos		リスト位置
 * @param		flg				FALSE = 初期化時
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TypeListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
	ZKNSEARCHMAIN_WORK * wk = work;

//	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：タイプページリストスクロール
 *
 * @param		work			図鑑検索画面ワーク
 * @param		mv				スクロール値
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TypeListCallBack_Scroll( void * work, s8 mv )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHOBJ_ScrollList( wk, mv*-1 );

//	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
	ZKNSEARCHOBJ_SetListPageArrowAnime( wk, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストコールバック：色ページ項目表示
 *
 * @param		work			図鑑検索画面ワーク
 * @param		itemNum		項目番号
 * @param		util			項目で使用しているPRINT_UTIL
 * @param		py				表示Ｙ座標
 * @param		disp			描画面
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ColorListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHBMP_PutListItem( wk, util, wk->item[itemNum] );

	if( wk->dat->sort->color == itemNum ){
		ZKNSEARCHOBJ_PutMark( wk, 0, py, disp );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：色ページカーソル移動
 *
 * @param		work			図鑑検索画面ワーク
 * @param		listPos		リスト位置
 * @param		flg				FALSE = 初期化時
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ColorListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
	ZKNSEARCHMAIN_WORK * wk = work;

//	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：色ページリストスクロール
 *
 * @param		work			図鑑検索画面ワーク
 * @param		mv				スクロール値
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ColorListCallBack_Scroll( void * work, s8 mv )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHOBJ_ScrollList( wk, mv*-1 );

//	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
	ZKNSEARCHOBJ_SetListPageArrowAnime( wk, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストコールバック：フォルムページ項目表示
 *
 * @param		work			図鑑検索画面ワーク
 * @param		itemNum		項目番号
 * @param		util			項目で使用しているPRINT_UTIL
 * @param		py				表示Ｙ座標
 * @param		disp			描画面
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void FormListCallBack_Draw( void * work, u32 itemNum, PRINT_UTIL * util, s16 py, BOOL disp )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHBMP_PutFormListItem( wk, util );

	if( wk->dat->sort->form == itemNum ){
		ZKNSEARCHOBJ_PutFormMark( wk, py, disp );
	}
	ZKNSEARCHOBJ_PutFormList( wk, itemNum, py, disp );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：フォルムページカーソル移動
 *
 * @param		work			図鑑検索画面ワーク
 * @param		listPos		リスト位置
 * @param		flg				FALSE = 初期化時
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void FormListCallBack_Move( void * work, u32 listPos, BOOL flg )
{
	ZKNSEARCHMAIN_WORK * wk = work;

//	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リストコールバック：フォルムリストスクロール
 *
 * @param		work			図鑑検索画面ワーク
 * @param		mv				スクロール値
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void FormListCallBack_Scroll( void * work, s8 mv )
{
	ZKNSEARCHMAIN_WORK * wk = work;

	ZKNSEARCHOBJ_ScrollFormList( wk, mv*-1 );

//	ZKNSEARCHOBJ_SetScrollBar( wk, FRAMELIST_GetScrollBarPY(wk->lwk) );
	ZKNSEARCHOBJ_SetListPageArrowAnime( wk, TRUE );
}
