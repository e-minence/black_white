//============================================================================================
/**
 * @file		box2_obj.c
 * @brief		ボックス画面 OBJ関連
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 *
 *	モジュール名：BOX2OBJ
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/main.h"
#include "pokeicon/pokeicon.h"
#include "item/item.h"

#include "app_menu_common.naix"

#include "box2_main.h"
#include "box2_obj.h"
#include "box_gra.naix"


//============================================================================================
//	定数定義
//============================================================================================

// メインパレット
#define	PALNUM_TRAYICON		( 0 )																	// パレット番号：トレイアイコン
#define	PALSIZ_TRAYICON		( 1 )																	// パレット数：トレイアイコン
#define	PALNUM_WPICON			( PALNUM_TRAYICON + PALSIZ_TRAYICON )	// パレット番号：壁紙アイコン
#define	PALSIZ_WPICON			( 2 )																	// パレット数：壁紙アイコン
#define	PALNUM_POKEICON		( PALNUM_WPICON + PALSIZ_WPICON )			// パレット番号：ポケモンアイコン
#define	PALSIZ_POKEICON		( POKEICON_PAL_MAX )									// パレット数：ポケモンアイコン
#define	PALNUM_BOXOBJ			( PALNUM_POKEICON + PALSIZ_POKEICON )	// パレット番号：その他のＯＢＪ
#define	PALSIZ_BOXOBJ			( 3 )																	// パレット数：その他のＯＢＪ
#define	PALNUM_ITEMICON		( PALNUM_BOXOBJ + PALSIZ_BOXOBJ )			// パレット番号：アイテムアイコン
#define	PALSIZ_ITEMICON		( 1 )																	// パレット数：アイテムアイコン

#define	PALNUM_OUTLINE		( PALNUM_BOXOBJ + 2 )		// パレット番号：アウトラインカーソル
#define	PLANUM_TRAYPOKE		( PALNUM_WPICON + 1 )		// パレット番号：トレイアイコンに表示するドット

// サブパレット
#define	PALNUM_TYPEICON_S	( 0 )																			// パレット番号：タイプアイコン
#define	PALSIZ_TYPEICON_S	( 3 )																			// パレット数：タイプアイコン
#define	PALNUM_ITEMICON_S	( PALNUM_TYPEICON_S + PALSIZ_TYPEICON_S )	// パレット番号：アイテムアイコン
#define	PALSIZ_ITEMICON_S	( 1 )																			// パレット数：アイテムアイコン

/*
// アクター表示フラグ
enum {
	ACT_OFF = 0,	// アクター非表示
	ACT_ON			// アクター表示
};

#define	BOX_SUB_ACTOR_DISTANCE	( 512*FX32_ONE )	// サブ画面のOBJ表示Y座標オフセット
#define	BOX_SUB_ACTOR_DEF_PY	( 320 )				// サブ画面のOBJ表示Y座標オフセット

// ポケモン一枚絵データ
#define	POKEGRA_TEX_SIZE	( 0x20 * 100 )	// テクスチャサイズ
#define	POKEGRA_PAL_SIZE	( 0x20 )		// パレットサイズ

// フォントOAMで使用するOBJ管理数
// トレイ名と収納数で使用するBMPWINのキャラサイズ分が最大なので、それだけあれば足りる。
#define	FNTOAM_CHR_MAX		( BOX2OBJ_FNTOAM_BOXNAME_SX * BOX2OBJ_FNTOAM_BOXNAME_SY + BOX2OBJ_FNTOAM_BOXNUM_SX * BOX2OBJ_FNTOAM_BOXNUM_SY )
*/
// ポケモンアイコンのプライオリティ（80は適当）
#define	POKEICON_OBJ_PRI(a)			( 80 + BOX2OBJ_POKEICON_MAX - a*2 )	// 通常のOBJプライオリティ
#define	POKEICON_OBJ_PRI_PUT(a)	( 80 + BOX2OBJ_POKEICON_MAX - a*2 )	// 配置時のOBJプライオリティ
#define	POKEICON_OBJ_PRI_GET		( 20 )															// 取得時のOBJプライオリティ
#define	POKEICON_TBG_PRI_PUT		( 3 )																// トレイ配置時のBGプライオリティ
#define	POKEICON_PBG_PRI_PUT		( 1 )																// 手持ち配置時のBGプライオリティ
#define	POKEICON_BG_PRI_GET			( 0 )																// 取得時のBGプライオリティ

/*
#define	WP_IMAGE_COLOR_START	( 16 )		// 壁紙のイメージカラー開始番号
#define	WP_ICON_CHG_COL			( 0x1e )	// グラフィックデータの壁紙のデフォルトカラー
*/

#define	BOX2OBJ_TRAYICON_SX		( 34 )	// トレイアイコンのＸ座標配置間隔
#define	BOX2OBJ_TRAYICON_PX		( 43 )	// トレイアイコンのデフォルトＸ座標

#define	BOX2OBJ_WPICON_SX		( 46 )	// 壁紙アイコンのＸ座標配置間隔
#define	BOX2OBJ_WPICON_PX		( 59 )	// 壁紙アイコンのデフォルトＸ座標

#define	BOXNAME_OBJ_PX	( 128 )			// トレイ名のOAMフォントのデフォルト表示Ｘ座標
#define	BOXNAME_OBJ_PY	( 20-48 )		// トレイ名のOAMフォントのデフォルト表示Ｙ座標

/*
#define	PARTYPOKE_FRM_PX	( 16 )		// 手持ちポケモンのアイコンデフォルト表示Ｘ座標
#define	PARTYPOKE_FRM_PY	( 192 )		// 手持ちポケモンのアイコンデフォルト表示Ｙ座標

// トレイのポケモンアイコンスクロール関連
#define	TRAY_L_SCROLL_ICON_SX	( -8 )			// アイコン表示開始座標（左）
#define	TRAY_L_SCROLL_ICON_EX	( 144+32 )		// アイコン表示終了座標（左）
#define	TRAY_R_SCROLL_ICON_SX	( 144+32 )		// アイコン表示開始座標（右）
#define	TRAY_R_SCROLL_ICON_EX	( -8 )			// アイコン表示終了座標（右）

// ポケモンを逃がす関連
#define	POKEICON_FREE_SCALE_DEF		( 1.0f )	// デフォルト拡縮値
#define	POKEICON_FREE_SCALE_PRM		( 0.025f )	// 拡縮増減値
#define	POKEICON_FREE_CNT_MAX		( 40 )		// カウンタ
#define	POKEICON_FREE_CY			( 8 )		// 拡縮中心Ｙオフセット

// ボックス移動関連
#define	BOXMV_LA_PX				( 12 )						// 左矢印表示Ｘ座標
#define	BOXMV_LA_PY				( 27-48 )					// 左矢印表示Ｙ座標
#define	BOXMV_RA_PX				( 244 )						// 右矢印表示Ｘ座標
#define	BOXMV_RA_PY				( 27-48 )					// 右矢印表示Ｘ座標
#define	BOXMV_TRAY_CUR_PX		( BOX2OBJ_TRAYICON_PX )		// カーソル表示Ｘ座標
#define	BOXMV_TRAY_CUR_PY		( 27-48 )					// カーソル表示Ｙ座標
#define	BOXMV_TRAY_NAME_PX		( 128 )						// トレイ名背景表示Ｘ座標
#define	BOXMV_TRAY_NAME_PY		( 8-48 )					// トレイ名背景表示Ｘ座標
#define	BOXMV_TRAY_ARROW_PX		( 128 )						// カーソル矢印表示Ｘ座標
#define	BOXMV_TRAY_ARROW_PY		( 20-48 )					// カーソル矢印表示Ｙ座標

#define	TRAY_ICON_CGX_SIZ		( 32 * 32 )		// トレイアイコンキャラサイズ
#define	TRAY_ICON_CHG_COL		( 8 )			// トレイアイコン壁紙デフォルトカラー

#define	TRAY_POKEDOT_PX		( 10 )		// トレイに書き込みポケモンドット開始Ｘ座標
#define	TRAY_POKEDOT_PY		( 11 )		// トレイに書き込みポケモンドット開始Ｙ座標
#define	TRAY_POKEDOT_SX		( 2 )		// トレイに書き込みポケモンドットＸサイズ
#define	TRAY_POKEDOT_SY		( 2 )		// トレイに書き込みポケモンドットＹサイズ

// 壁紙変更関連
#define	WPCHG_LA_PX				( 13 )					// 左矢印表示Ｘ座標
#define	WPCHG_LA_PY				( 27-48 )				// 左矢印表示Ｙ座標
#define	WPCHG_RA_PX				( 243 )					// 右矢印表示Ｘ座標
#define	WPCHG_RA_PY				( 27-48 )				// 右矢印表示Ｙ座標
#define	WPCHG_TRAY_CUR_PX		( BOX2OBJ_WPICON_PX )	// カーソル表示Ｘ座標
#define	WPCHG_TRAY_CUR_PY		( 27-48 )				// カーソル表示Ｙ座標
#define	WPCHG_TRAY_NAME_PX		( 128 )					// 名前背景表示Ｘ座標
#define	WPCHG_TRAY_NAME_PY		( 8-48 )				// 名前背景表示Ｙ座標
#define WPCHG_TRAY_ARROW_PX		( 128 )					// カーソル矢印表示Ｘ座標
#define	WPCHG_TRAY_ARROW_PY		( 20-48 )				// カーソル矢印表示Ｙ座標

#define	WP_ICON_CGX_SIZ		( 24 * 24 )		// 壁紙アイコンキャラサイズ

// ポケモンを預けるボックス選択関連
#define	PTOUT_TRAY_ICON_PY		( 84 )						// トレイアイコン表示Ｙ座標
#define	PTOUT_LA_PX				( 12 )						// トレイ切り替え左矢印表示Ｘ座標
#define	PTOUT_LA_PY				( 84 )						// トレイ切り替え左矢印表示Ｙ座標
#define	PTOUT_RA_PX				( 244 )						// トレイ切り替え右矢印表示Ｘ座標
#define	PTOUT_RA_PY				( 84 )						// トレイ切り替え右矢印表示Ｙ座標
#define	PTOUT_TRAY_CUR_PX		( BOX2OBJ_TRAYICON_PX )		// トレイカーソル表示Ｘ座標
#define	PTOUT_TRAY_CUR_PY		( 84 )						// トレイカーソル表示Ｙ座標
#define	PTOUT_TRAY_NAME_PX		( 128 )						// トレイ名表示Ｘ座標
#define	PTOUT_TRAY_NAME_PY		( 65 )						// トレイ名表示Ｙ座標
#define	PTOUT_TRAY_ARROW_PX		( 128 )						// カーソル矢印Ｘ座標
#define	PTOUT_TRAY_ARROW_PY		( 77 )						// カーソル矢印Ｙ座標

// 上画面アイテムアイコン表示座標
#define	SUBDISP_ITEMICON_PX	( 18 )
#define	SUBDISP_ITEMICON_PY	( 40 )
*/





// セルアクターデータ
typedef struct {
	GFL_CLWK_DATA	dat;

	u32	chrRes;
	u32	palRes;
	u32	celRes;

	u16	pal;
	u16	disp;

}BOX_CLWK_DATA;


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void ClactInit( BOX2_APP_WORK * appwk );
static void ClactResLoad( BOX2_APP_WORK * appwk );
static void ClactResFree( BOX2_APP_WORK * appwk );
static void ClactAdd( BOX2_SYS_WORK * syswk );
static void ClactDelAll( BOX2_APP_WORK * appwk );

static void BoxGraResLoad( BOX2_APP_WORK * appwk );

static void PokeIconResLoad( BOX2_APP_WORK * appwk );
static void PokeIconObjAdd( BOX2_APP_WORK * appwk );

static void PokeGraDummyResLoad( BOX2_APP_WORK * appwk );

static void TypeIconResLoad( BOX2_APP_WORK * appwk );

//static void WallPaperIconResLoad( BOX2_APP_WORK * appwk );
static void WallPaperObjAdd( BOX2_APP_WORK * appwk );

static void ItemIconDummyResLoad( BOX2_APP_WORK * appwk );
//static void ItemIconObjAdd( BOX2_APP_WORK * appwk );

//static void BoxObjResLoad( BOX2_APP_WORK * appwk );
static void BoxObjAdd( BOX2_APP_WORK * appwk );

//static void TrayIconResLoad( BOX2_APP_WORK * appwk );
static void TrayObjAdd( BOX2_APP_WORK * appwk );


//============================================================================================
//	グローバル
//============================================================================================

// セルアクターデータ
static const BOX_CLWK_DATA ClactParamTbl[] =
{
	{	// ページ矢印（左）
		{ 12, 28, BOX2OBJ_ANM_L_ARROW_OFF, 0, 3 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN
	},
	{	// ページ矢印（右）
		{ 156, 28, BOX2OBJ_ANM_R_ARROW_OFF, 0, 3 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
/*
	{	// ポケモン
		{ 140, 100, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEGRA, BOX2MAIN_PALRES_POKEGRA, BOX2MAIN_CELRES_POKEGRA,
		0, CLSYS_DRAW_SUB,
	},
	{	// ポケモン
		{ 140, 100, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEGRA, BOX2MAIN_PALRES_POKEGRA, BOX2MAIN_CELRES_POKEGRA,
		0, CLSYS_DRAW_SUB,
	},
*/
	{	// ボックス移動矢印（左）
		{ 12, -21, BOX2OBJ_ANM_L_ARROW_OFF, 5, 1 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
	{	// ボックス移動矢印（右）
		{ 244, -21, BOX2OBJ_ANM_R_ARROW_OFF, 5, 1 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
	{	// トレイカーソル
		{ BOX2OBJ_TRAYICON_PX, -21, BOX2OBJ_ANM_TRAY_CURSOR, 5, 1 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
	{	// トレイ名背景
		{ 128, -40, BOX2OBJ_ANM_TRAY_NAME, 5, 1 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
	{	// トレイ矢印
		{ BOXNAME_OBJ_PX, BOXNAME_OBJ_PY, BOX2OBJ_ANM_TRAY_ARROW, 5, 1 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},

	{	// 手カーソル
		{ 128, 128, BOX2OBJ_ANM_HAND_NORMAL, 0, 0 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
	{	// 手カーソル影
		{ 128, 160, BOX2OBJ_ANM_HAND_SHADOW, 0, 0 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},

	{	// アイテムアイコン
		{ 0, 0, 0, 0, 0 },
		BOX2MAIN_CHRRES_ITEMICON, BOX2MAIN_PALRES_ITEMICON, BOX2MAIN_CELRES_ITEMICON,
		0, CLSYS_DRAW_MAIN,
	},
	{	// アイテムアイコン（上画面）
		{ 0, 0, 0, 0, 0 },
		BOX2MAIN_CHRRES_ITEMICON_SUB, BOX2MAIN_PALRES_ITEMICON, BOX2MAIN_CELRES_ITEMICON,
		0, CLSYS_DRAW_SUB,
	},

	{	// タイプアイコン１（上画面）
		{ 160, 48, 0, 0, 1 },
		BOX2MAIN_CHRRES_TYPEICON1, BOX2MAIN_PALRES_TYPEICON, BOX2MAIN_CELRES_TYPEICON,
		0, CLSYS_DRAW_SUB,
	},
	{	// タイプアイコン２（上画面）
		{ 194, 48, 0, 0, 1 },
		BOX2MAIN_CHRRES_TYPEICON2, BOX2MAIN_PALRES_TYPEICON, BOX2MAIN_CELRES_TYPEICON,
		0, CLSYS_DRAW_SUB,
	}
};

// ポケモンアイコン
static const BOX_CLWK_DATA ClaPokeIconParam = {
	{ BOX2OBJ_TRAYPOKE_PX, BOX2OBJ_TRAYPOKE_PY, 0, 10, POKEICON_TBG_PRI_PUT },
	BOX2MAIN_CHRRES_POKEICON, BOX2MAIN_PALRES_POKEICON, BOX2MAIN_CELRES_POKEICON,
	0, CLSYS_DRAW_MAIN,
};

// トレイアイコン
static const BOX_CLWK_DATA ClaTrayIconParam = {
	{ BOX2OBJ_TRAYICON_PX, -21, 0, 10, 1 },
	BOX2MAIN_CHRRES_TRAYICON, BOX2MAIN_PALRES_TRAYICON, BOX2MAIN_CELRES_TRAYICON,
	0, CLSYS_DRAW_MAIN,
};

// 壁紙アイコン
static const BOX_CLWK_DATA ClaWallPaperIconParam = {
	{ BOX2OBJ_WPICON_PX, -21, 0, 10, 1 },
	BOX2MAIN_CHRRES_WPICON, BOX2MAIN_PALRES_WPICON, BOX2MAIN_CELRES_WPICON,
	0, CLSYS_DRAW_MAIN,
};





//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター初期化
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_Init( BOX2_SYS_WORK * syswk )
{
	ClactInit( syswk->app );
	ClactResLoad( syswk->app );
	ClactAdd( syswk );

/*
	BOX2OBJ_FontOamInit( syswk->app );
*/

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// MAIN DISP OBJ ON
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// SUB DISP OBJ ON
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター解放
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_Exit( BOX2_APP_WORK * appwk )
{
/*
	BOX2OBJ_FontOamExit( appwk );

	ClactResManExit( appwk );
*/
	ClactDelAll( appwk );
	ClactResFree( appwk );
	GFL_CLACT_SYS_Delete();
}

static void ClactInit( BOX2_APP_WORK * appwk )
{
	{
		const GFL_CLSYS_INIT init = {
			0, 0,									// メイン　サーフェースの左上座標
			0, 512,								// サブ　サーフェースの左上座標
			4,										// メイン画面OAM管理開始位置	4の倍数
			124,									// メイン画面OAM管理数				4の倍数
			4,										// サブ画面OAM管理開始位置		4の倍数
			124,									// サブ画面OAM管理数					4の倍数
			0,										// セルVram転送管理数

			BOX2MAIN_CHRRES_MAX,	// 登録できるキャラデータ数
			BOX2MAIN_PALRES_MAX,	// 登録できるパレットデータ数
			BOX2MAIN_CELRES_MAX,	// 登録できるセルアニメパターン数
			0,										// 登録できるマルチセルアニメパターン数（※現状未対応）

		  16,										// メイン CGR　VRAM管理領域　開始オフセット（キャラクタ単位）
		  16										//< サブ CGR　VRAM管理領域　開始オフセット（キャラクタ単位）
		};
		GFL_CLACT_SYS_Create( &init, BOX2MAIN_GetVramBankData(), HEAPID_BOX_APP );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リソース読み込み
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClactResLoad( BOX2_APP_WORK * appwk )
{
	BoxGraResLoad( appwk );
	PokeIconResLoad( appwk );
	ItemIconDummyResLoad( appwk );

//	PokeGraDummyResLoad( appwk );
	TypeIconResLoad( appwk );

/*
	TrayIconResLoad( appwk );
	WallPaperIconResLoad( appwk );
	PokeIconResLoad( appwk );
	BoxObjResLoad( appwk );
	ItemIconDummyResLoad( appwk );

//	PokeGraDummyResLoad( appwk );
	TypeIconResLoad( appwk );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リソース削除
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClactResFree( BOX2_APP_WORK * appwk )
{
	u32	i;

	for( i=0; i<BOX2MAIN_CHRRES_MAX; i++ ){
    GFL_CLGRP_CGR_Release( appwk->chrRes[i] );
	}
	for( i=0; i<BOX2MAIN_PALRES_MAX; i++ ){
    GFL_CLGRP_PLTT_Release( appwk->palRes[i] );
	}
	for( i=0; i<BOX2MAIN_CELRES_MAX; i++ ){
    GFL_CLGRP_CELLANIM_Release( appwk->celRes[i] );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター追加
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClactAdd( BOX2_SYS_WORK * syswk )
{
	syswk->app->clunit = GFL_CLACT_UNIT_Create( BOX2OBJ_ID_MAX, 0, HEAPID_BOX_APP );

	TrayObjAdd( syswk->app );
	WallPaperObjAdd( syswk->app );
	PokeIconObjAdd( syswk->app );
	BoxObjAdd( syswk->app );

/*
	ItemIconObjAdd( syswk->app );

	PokeGraObjAdd( syswk );
	TypeIconObjAdd( syswk->app );
*/
}

static GFL_CLWK * ClactWorkCreate( BOX2_APP_WORK * appwk, const BOX_CLWK_DATA * prm )
{
	GFL_CLWK * clwk;

	clwk = GFL_CLACT_WK_Create(
					appwk->clunit,
					appwk->chrRes[prm->chrRes],
					appwk->palRes[prm->palRes],
					appwk->celRes[prm->celRes],
					&prm->dat, prm->disp, HEAPID_BOX_APP );

	GFL_CLACT_WK_SetPlttOffs( clwk, prm->pal, CLWK_PLTTOFFS_MODE_PLTT_TOP );

	return clwk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター削除
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClactDel( BOX2_APP_WORK * appwk, u32 id )
{
	if( appwk->clwk[id] != NULL ){
		GFL_CLACT_WK_Remove( appwk->clwk[id] );
		appwk->clwk[id] = NULL;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター全削除
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClactDelAll( BOX2_APP_WORK * appwk )
{
	u32	i;

	for( i=0; i<BOX2OBJ_ID_MAX; i++ ){
		ClactDel( appwk, i );
	}
	GFL_CLACT_UNIT_Delete( appwk->clunit );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターアニメメイン
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_AnmMain( BOX2_APP_WORK * appwk )
{
	u32	i;

	for( i=0; i<BOX2OBJ_ID_TRAYICON; i++ ){
		if( appwk->clwk[i] != NULL ){
			GFL_CLACT_WK_AddAnmFrame( appwk->clwk[i], FX32_ONE );
		}
	}
	GFL_CLACT_SYS_Main();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターアニメ変更
 *
 * @param		appwk	ボックス画面アプリワーク
 * @param		id		OBJ ID
 * @param		anm		アニメ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_AnmSet( BOX2_APP_WORK * appwk, u32 id, u32 anm )
{
	GFL_CLACT_WK_SetAnmFrame( appwk->clwk[id], 0 );
	GFL_CLACT_WK_SetAnmSeq( appwk->clwk[id], anm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターアニメ取得
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 *
 * @return	アニメ番号
 */
//--------------------------------------------------------------------------------------------
u32 BOX2OBJ_AnmGet( BOX2_APP_WORK * appwk, u32 id )
{
	return GFL_CLACT_WK_GetAnmSeq( appwk->clwk[id] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターアニメ状態取得
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 *
 * @retval	"TRUE = アニメ中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2OBJ_AnmCheck( BOX2_APP_WORK * appwk, u32 id )
{
	return GFL_CLACT_WK_CheckAnmActive( appwk->clwk[id] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター表示切り替え
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		flg			表示フラグ
 *
 * @return	none
 *
 * @li	flg = TRUE : 表示
 * @li	flg = FALSE : 非表示
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_Vanish( BOX2_APP_WORK * appwk, u32 id, BOOL flg )
{
	GFL_CLACT_WK_SetDrawEnable( appwk->clwk[id], flg );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター表示チェック
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 *
 * @retval	"TRUE = 表示"
 * @retval	"FALSE = 非表示"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2OBJ_VanishCheck( BOX2_APP_WORK * appwk, u32 id )
{
	return GFL_CLACT_WK_GetDrawEnable( appwk->clwk[id] );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターBGプライオリティ変更
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		pri			プライオリティ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BgPriChange( BOX2_APP_WORK * appwk, u32 id, int pri )
{
	GFL_CLACT_WK_SetBgPri( appwk->clwk[id], pri );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターのOBJ同士のプライオリティ変更
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		pri			プライオリティ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ObjPriChange( BOX2_APP_WORK * appwk, u32 id, int pri )
{
//	appwk->obj_trans_stop = 1;
	GFL_CLACT_WK_SetSoftPri( appwk->clwk[id], pri );
//	appwk->obj_trans_stop = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター半透明設定
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		flg			ON/OFFフラグ
 *
 * @return	none
 *
 * @li	flg : TRUE = ON, FALSE = OFF
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BlendModeSet( BOX2_APP_WORK * appwk, u32 id, BOOL flg )
{
	if( flg == TRUE ){
		GFL_CLACT_WK_SetObjMode( appwk->clwk[id], GX_OAM_MODE_XLU );
	}else{
		GFL_CLACT_WK_SetObjMode( appwk->clwk[id], GX_OAM_MODE_NORMAL );
	}
}






static void BoxGraResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
	u32	i;

	ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	// トレイアイコン
	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_TRAYICON+i] = GFL_CLGRP_CGR_Register(
																									ah, NARC_box_gra_box_tray_lz_NCGR,
																									TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	}
  appwk->palRes[BOX2MAIN_PALRES_TRAYICON] = GFL_CLGRP_PLTT_Register(
																							ah, NARC_box_gra_box_tray_NCLR,
																							CLSYS_DRAW_MAIN, PALNUM_TRAYICON*0x20, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_TRAYICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_box_tray_NCER,
																							NARC_box_gra_box_tray_NANR,
																							HEAPID_BOX_APP );

	// 壁紙アイコン
	for( i=0; i<BOX2OBJ_WPICON_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_WPICON+i] = GFL_CLGRP_CGR_Register(
																								ah, NARC_box_gra_box_wp_image_NCGR,
																								FALSE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	}
	appwk->celRes[BOX2MAIN_CELRES_WPICON] = GFL_CLGRP_CELLANIM_Register(
																						ah,
																						NARC_box_gra_box_wp_image_NCER,
																						NARC_box_gra_box_wp_image_NANR,
																						HEAPID_BOX_APP );
  appwk->palRes[BOX2MAIN_PALRES_WPICON] = GFL_CLGRP_PLTT_Register(
																						ah, NARC_box_gra_box_wp_image_NCLR,
																						CLSYS_DRAW_MAIN, PALNUM_WPICON*0x20, HEAPID_BOX_APP );

	// ポケモンアイコン
	// キャラ
	for( i=0; i<BOX2OBJ_POKEICON_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_POKEICON+i] = GFL_CLGRP_CGR_Register(
																									ah, NARC_box_gra_box_pokeicon_lz_NCGR,
																									TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	}

	// その他のＯＢＪ
	appwk->chrRes[BOX2MAIN_CHRRES_BOXOBJ] = GFL_CLGRP_CGR_Register(
																						ah, NARC_box_gra_box_m_obj_lz_NCGR,
																						TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
  appwk->palRes[BOX2MAIN_PALRES_BOXOBJ] = GFL_CLGRP_PLTT_Register(
																						ah, NARC_box_gra_box_m_obj2_NCLR,
																						CLSYS_DRAW_MAIN, PALNUM_BOXOBJ*0x20, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_BOXOBJ] = GFL_CLGRP_CELLANIM_Register(
																						ah,
																						NARC_box_gra_box_m_obj_NCER,
																						NARC_box_gra_box_m_obj_NANR,
																						HEAPID_BOX_APP );

	// アイテムアイコン
	appwk->chrRes[BOX2MAIN_CHRRES_ITEMICON] = GFL_CLGRP_CGR_Register(
																							ah, NARC_box_gra_box_itemicon_lz_NCGR,
																							TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_ITEMICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_box_itemicon_NCER,
																							NARC_box_gra_box_itemicon_NANR,
																							HEAPID_BOX_APP );
	appwk->chrRes[BOX2MAIN_CHRRES_ITEMICON_SUB] = GFL_CLGRP_CGR_Register(
																									ah, NARC_box_gra_box_itemicon_lz_NCGR,
																									TRUE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );

	// タイプアイコン
	appwk->chrRes[BOX2MAIN_CHRRES_TYPEICON1] = GFL_CLGRP_CGR_Register(
																							ah, NARC_box_gra_box_typeicon_lz_NCGR,
																							TRUE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	appwk->chrRes[BOX2MAIN_CHRRES_TYPEICON2] = GFL_CLGRP_CGR_Register(
																							ah, NARC_box_gra_box_typeicon_lz_NCGR,
																							TRUE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_TYPEICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_box_typeicon_NCER,
																							NARC_box_gra_box_typeicon_NANR,
																							HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}


//============================================================================================
//	ポケモンアイコン
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンリソース読み込み
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 *
 *	キャラはダミー
 */
//--------------------------------------------------------------------------------------------
static void PokeIconResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
/*
	u32	i;

	ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	// キャラ
	for( i=0; i<BOX2OBJ_POKEICON_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_POKEICON+i] = GFL_CLGRP_CGR_Register(
																									ah, NARC_box_gra_box_pokeicon_lz_NCGR,
																									TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	}

  GFL_ARC_CloseDataHandle( ah );
*/

	ah = GFL_ARC_OpenDataHandle( ARCID_POKEICON, HEAPID_BOX_APP );

	// パレット
  appwk->palRes[BOX2MAIN_PALRES_POKEICON] = GFL_CLGRP_PLTT_RegisterComp(
																							ah, POKEICON_GetPalArcIndex(),
																							CLSYS_DRAW_MAIN, PALNUM_POKEICON*0x20, HEAPID_BOX_APP );

	// セル・アニメ
  appwk->celRes[BOX2MAIN_CELRES_POKEICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							POKEICON_GetCellArcIndex(),
																							POKEICON_GetAnmArcIndex(),
																							HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンOBJ追加
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
*/
//--------------------------------------------------------------------------------------------
static void PokeIconObjAdd( BOX2_APP_WORK * appwk )
{
	BOX_CLWK_DATA	prm;
	u32	i;

	for( i=0; i<BOX2OBJ_POKEICON_MAX; i++ ){
		prm = ClaPokeIconParam;
		prm.dat.pos_x += ( i % BOX2OBJ_POKEICON_H_MAX * BOX2OBJ_TRAYPOKE_SX );
		prm.dat.pos_y += ( i / BOX2OBJ_POKEICON_H_MAX * BOX2OBJ_TRAYPOKE_SY );
		prm.dat.softpri = POKEICON_OBJ_PRI(i);
		prm.chrRes = BOX2MAIN_CHRRES_POKEICON + i;
		appwk->clwk[BOX2OBJ_ID_POKEICON+i] = ClactWorkCreate( appwk, &prm );

		appwk->pokeicon_id[i] = BOX2OBJ_ID_POKEICON+i;
	}

/*
	TCATS_OBJECT_ADD_PARAM_S	prm;
	u32	i;

	for( i=0; i<BOX2OBJ_POKEICON_MAX; i++ ){
		prm = ClaPokeIconParam;
		prm.x += ( i % BOX2OBJ_POKEICON_H_MAX * BOX2OBJ_TRAYPOKE_SX );
		prm.y += ( i / BOX2OBJ_POKEICON_H_MAX * BOX2OBJ_TRAYPOKE_SY );
		prm.pri = POKEICON_OBJ_PRI(i);
		prm.id[0] = CHR_ID_POKEICON + i;

		appwk->cap[BOX2OBJ_ID_POKEICON+i] = CATS_ObjectAdd_S( appwk->csp, appwk->crp, &prm );

		appwk->pokeicon_id[i] = BOX2OBJ_ID_POKEICON+i;
	}
*/
}

//--------------------------------------------------------------------------------------------
/**
 * ＯＢＪに割り当てられているキャラデータを書き換える
 *
 * @param	appwk		ボックス画面アプリワーク
 * @param	id			OBJ ID
 * @param	buf			キャラデータ
 * @param	siz			キャラサイズ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void CgxChangeTrans( BOX2_APP_WORK * appwk, u32 id, u8 * buf, u32 siz )
{
	NNSG2dImageProxy * ipc;
	u32	cgx;

	ipc = CLACT_ImageProxyGet( appwk->cap[id]->act );
	cgx = NNS_G2dGetImageLocation( ipc, NNS_G2D_VRAM_TYPE_2DMAIN );

	DC_FlushRange( buf, siz );
	GX_LoadOBJ( buf, cgx, siz );
}
*/


//--------------------------------------------------------------------------------------------
/**
 * ＯＢＪに割り当てられているキャラデータを書き換える（サブ画面用）
 *
 * @param	appwk		ボックス画面アプリワーク
 * @param	id			OBJ ID
 * @param	buf			キャラデータ
 * @param	siz			キャラサイズ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void CgxChangeTransSub( BOX2_APP_WORK * appwk, u32 id, u8 * buf, u32 siz )
{
	NNSG2dImageProxy * ipc;
	u32	cgx;

	ipc = CLACT_ImageProxyGet( appwk->cap[id]->act );
	cgx = NNS_G2dGetImageLocation( ipc, NNS_G2D_VRAM_TYPE_2DSUB );

	DC_FlushRange( buf, siz );
	GXS_LoadOBJ( buf, cgx, siz );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンキャラデータ読み込み
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	ppp		POKEMON_PASO_PARAM
 * @param	chr		NNSG2dCharacterData
 *
 * @return	buf
 */
//--------------------------------------------------------------------------------------------
static void * PokeIconCgxLoad(
				BOX2_APP_WORK * appwk, const POKEMON_PASO_PARAM * ppp, NNSG2dCharacterData ** chr )
{
	void * buf;
	u32	arc;

	arc = POKEICON_GetCgxArcIndex( ppp );
	buf = GFL_ARCHDL_UTIL_LoadOBJCharacter( appwk->pokeicon_ah, arc, FALSE, chr, HEAPID_BOX_APP );

	return buf;
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンのキャラデータをワークに展開
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	tray	トレイ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_PokeIconBufLoad( BOX2_SYS_WORK * syswk, u32 tray )
{
	POKEMON_PASO_PARAM * ppp;
	NNSG2dCharacterData * chr;
	void * buf;
	u32	i;

	for( i=0; i<BOX2OBJ_POKEICON_TRAY_MAX; i++ ){
		if( BOXDAT_PokeParaGet( syswk->box, tray, i, ID_PARA_poke_exist, NULL ) == 0 ){
			syswk->app->pokeicon_exist[i] = ACT_OFF;
			continue;
		}
		ppp = BOX2MAIN_PPPGet( syswk, tray, i );
		buf = PokeIconCgxLoad( syswk->app, ppp, &chr );
		MI_CpuCopy32( chr->pRawData, syswk->app->pokeicon_cgx[i], BOX2OBJ_POKEICON_CGX_SIZE );
		GFL_HEAP_FreeMemory( buf );
//↑[GS_CONVERT_TAG]

		syswk->app->pokeicon_pal[i] = PokeIconPalNumGetByPPP( ppp );

		syswk->app->pokeicon_exist[i] = ACT_ON;
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンのキャラデータを変更
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	buf		キャラデータバッファ
 * @param	id		OBJ ID
 * @param	pal		パレット番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconChangeCore( BOX2_APP_WORK * appwk, void * buf, u32 id, u32 pal )
{
//	CgxChangeTrans( appwk, id, buf, BOX2OBJ_POKEICON_CGX_SIZE );
	GFL_CLGRP_CGR_ReplaceEx(
		GFL_CLACT_WK_GetCgrIndex( appwk->clwk[id] ),
		buf, BOX2OBJ_POKEICON_CGX_SIZE, 0, CLSYS_DRAW_MAIN );

	GFL_CLACT_WK_SetPlttOffs( appwk->clwk[id], pal, CLWK_PLTTOFFS_MODE_PLTT_TOP );
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンのキャラデータを変更
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	ppp		POKEMON_PASO_PARAM
 * @param	id		OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconChange( BOX2_SYS_WORK * syswk, const POKEMON_PASO_PARAM * ppp, u32 id )
{
	NNSG2dCharacterData * chr;
	void * buf;

	buf = PokeIconCgxLoad( syswk->app, ppp, &chr );

	PokeIconChangeCore( syswk->app, chr->pRawData, id, POKEICON_GetPalNumGetByPPP(ppp) );

	GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコン変更
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		tray		トレイ番号
 * @param		pos			ポケモン位置
 * @param		id			OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconChange( BOX2_SYS_WORK * syswk, u32 tray, u32 pos, u32 id )
{
	BOX2_APP_WORK * appwk = syswk->app;

	BOX2OBJ_Vanish( appwk, id, FALSE );

	if( BOX2MAIN_PokeParaGet( syswk, pos, ID_PARA_poke_exist, NULL ) == 0 ){
		return;
	}

	PokeIconChange( syswk, BOX2MAIN_PPPGet( syswk, tray, pos ), id );
	BOX2OBJ_Vanish( appwk, id, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイのポケモンアイコンを変更
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayPokeIconChange( BOX2_SYS_WORK * syswk )
{
	u32	i;

	for( i=0; i<BOX2OBJ_POKEICON_TRAY_MAX; i++ ){
		BOX2OBJ_PokeIconChange( syswk, syswk->tray, i, BOX2OBJ_ID_POKEICON+i );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちのポケモンアイコンを変更
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconChange( BOX2_SYS_WORK * syswk )
{
	const POKEMON_PASO_PARAM * ppp;
	u32	ppcnt;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

	for( i=0; i<ppcnt; i++ ){
		ppp = PP_GetPPPPointerConst( PokeParty_GetMemberPointer(syswk->dat->pokeparty,i) );
		PokeIconChange( syswk, ppp, syswk->app->pokeicon_id[BOX2OBJ_POKEICON_TRAY_MAX+i] );
		BOX2OBJ_Vanish( syswk->app, syswk->app->pokeicon_id[BOX2OBJ_POKEICON_TRAY_MAX+i], FALSE );
	}
}


//============================================================================================
//	ポケモングラフィック
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ポケモングラフィックリソース読み込み
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeGraDummyResLoad( BOX2_APP_WORK * appwk )
{
/*
	CATS_LoadResourceCharArc(
		appwk->csp, appwk->crp,
		ARC_BATT_OBJ, POKE_OAM_NCGR, 0, NNS_G2D_VRAM_TYPE_2DSUB, CHR_ID_POKEGRA );
	CATS_LoadResourceCharArc(
		appwk->csp, appwk->crp,
		ARC_BATT_OBJ, POKE_OAM_NCGR, 0, NNS_G2D_VRAM_TYPE_2DSUB, CHR_ID_POKEGRA2 );

	CATS_LoadResourcePlttArc(
		appwk->csp, appwk->crp,
		ARC_BATT_OBJ, POKE_OAM_NCLR, 0, 1, NNS_G2D_VRAM_TYPE_2DSUB, PAL_ID_POKEGRA );
	CATS_LoadResourcePlttArc(
		appwk->csp, appwk->crp,
		ARC_BATT_OBJ, POKE_OAM_NCLR, 0, 1, NNS_G2D_VRAM_TYPE_2DSUB, PAL_ID_POKEGRA2 );

	CATS_LoadResourceCellArc(
		appwk->csp, appwk->crp, ARC_BATT_OBJ, POKE_OAM_NCER, 0, CEL_ID_POKEGRA );
	CATS_LoadResourceCellAnmArc(
		appwk->csp, appwk->crp, ARC_BATT_OBJ, POKE_OAM_NANR, 0, ANM_ID_POKEGRA );
*/
}


//============================================================================================
//	タイプアイコン
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * タイプアイコンリソース読み込み
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TypeIconResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
/*
	ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	// キャラ
	appwk->chrRes[BOX2MAIN_CHRRES_TYPEICON1] = GFL_CLGRP_CGR_Register(
																							ah, NARC_box_gra_box_typeicon_lz_NCGR,
																							TRUE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	appwk->chrRes[BOX2MAIN_CHRRES_TYPEICON2] = GFL_CLGRP_CGR_Register(
																							ah, NARC_box_gra_box_typeicon_lz_NCGR,
																							TRUE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	// セル・アニメ
	appwk->celRes[BOX2MAIN_CELRES_TYPEICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_box_typeicon_NCER,
																							NARC_box_gra_box_typeicon_NANR,
																							HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
*/
	ah = GFL_ARC_OpenDataHandle( ARCID_APP_MENU_COMMON, HEAPID_BOX_APP );
	
	// パレット
  appwk->palRes[BOX2MAIN_PALRES_TYPEICON] = GFL_CLGRP_PLTT_Register(
																							ah, NARC_app_menu_common_p_st_type_NCLR,
																							CLSYS_DRAW_SUB, PALNUM_TYPEICON_S*0x20, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * タイプアイコン追加
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void TypeIconObjAdd( BOX2_APP_WORK * appwk )
{
	appwk->cap[BOX2OBJ_ID_TYPEICON1] = CATS_ObjectAdd_S_SubDistance(
											appwk->csp, appwk->crp,
											&ClactParamTbl[BOX2OBJ_ID_TYPEICON1],
											BOX_SUB_ACTOR_DISTANCE );
	appwk->cap[BOX2OBJ_ID_TYPEICON2] = CATS_ObjectAdd_S_SubDistance(
											appwk->csp, appwk->crp,
											&ClactParamTbl[BOX2OBJ_ID_TYPEICON2],
											BOX_SUB_ACTOR_DISTANCE );

	GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_TYPEICON1], ACT_OFF );
	GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_TYPEICON2], ACT_OFF );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * タイプアイコン切り替え
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	type	タイプ
 * @param	objID	OBJ ID
 * @param	resID	リソースＩＤ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TypeIconChange( BOX2_APP_WORK * appwk, u32 type, u32 objID, u32 resID )
{
/*
	NNSG2dCharacterData * chr;
	void * buf;

	buf = GFL_ARC_UTIL_LoadBGCharacter or GFL_ARC_UTIL_LoadOBJCharacter(
			WazaTypeIcon_ArcIDGet(),
			WazaTypeIcon_CgrIDGet(type),
			WAZATYPEICON_COMP_CHAR, &chr, HEAPID_BOX_APP );
	CgxChangeTransSub( appwk, objID, chr->pRawData, 0x20 * 4 * 2 );
	GFL_HEAP_FreeMemory( buf );

	CATS_ObjectPaletteSetCap(
		appwk->cap[objID],
		GFL_CLACT_WK_GetPlttOffs(appwk->cap[objID])+WazaTypeIcon_PlttOffsetGet(type) );
*/
}

//--------------------------------------------------------------------------------------------
/**
 * タイプアイコン切り替え
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	info	表示データ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_TypeIconChange( BOX2_APP_WORK * appwk, BOX2_POKEINFO_DATA * info )
{
	if( info->tamago != 0 ){
		GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_TYPEICON1], ACT_OFF );
		GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_TYPEICON2], ACT_OFF );
	}else{
		TypeIconChange( appwk, info->type1, BOX2OBJ_ID_TYPEICON1, CHR_ID_TYPEICON1 );
		GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_TYPEICON1], ACT_ON );

		if( info->type2 != 0 && info->type1 != info->type2 ){
			TypeIconChange( appwk, info->type2, BOX2OBJ_ID_TYPEICON2, CHR_ID_TYPEICON2 );
			GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_TYPEICON2], ACT_ON );
		}else{
			GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_TYPEICON2], ACT_OFF );
		}
	}
}
*/

//============================================================================================
//	壁紙アイコン
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 壁紙アイコンリソース読み込み
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void WallPaperIconResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
	u32	i;
	
	ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	// キャラ
	for( i=0; i<BOX2OBJ_WPICON_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_WPICON+i] = GFL_CLGRP_CGR_Register(
																								ah, NARC_box_gra_box_wp_image_NCGR,
																								FALSE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	}
	// セル・アニメ
	appwk->celRes[BOX2MAIN_CELRES_WPICON] = GFL_CLGRP_CELLANIM_Register(
																						ah,
																						NARC_box_gra_box_wp_image_NCER,
																						NARC_box_gra_box_wp_image_NANR,
																						HEAPID_BOX_APP );
	// パレット
  appwk->palRes[BOX2MAIN_PALRES_WPICON] = GFL_CLGRP_PLTT_Register(
																						ah, NARC_box_gra_box_wp_image_NCLR,
																						CLSYS_DRAW_MAIN, PALNUM_WPICON*0x20, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * 壁紙アイコンOBJ追加
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WallPaperObjAdd( BOX2_APP_WORK * appwk )
{
	BOX_CLWK_DATA	prm;
	u32	i;

	for( i=0; i<BOX2OBJ_WPICON_MAX; i++ ){
		prm = ClaWallPaperIconParam;
		prm.dat.pos_x += BOX2OBJ_WPICON_SX * i;
		prm.chrRes = BOX2MAIN_CHRRES_WPICON + i;
		appwk->clwk[BOX2OBJ_ID_WPICON+i] = ClactWorkCreate( appwk, &prm );
	}
/*
	TCATS_OBJECT_ADD_PARAM_S	prm;
	u32	i;

	for( i=0; i<BOX2OBJ_WPICON_MAX; i++ ){
		prm = ClaWallPaperIconParam;
		prm.x += BOX2OBJ_WPICON_SX * i;
		prm.id[0] = CHR_ID_WPICON + i;
		appwk->cap[BOX2OBJ_ID_WPICON+i] = CATS_ObjectAdd_S( appwk->csp, appwk->crp, &prm );
	}
*/
}


//============================================================================================
//	アイテムアイコン
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコンリソース読み込み
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemIconDummyResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
/*
	ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_ITEMICON] = GFL_CLGRP_CGR_Register(
																							ah, NARC_box_gra_box_itemicon_lz_NCGR,
																							TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_ITEMICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_box_itemicon_NCER,
																							NARC_box_gra_box_itemicon_NANR,
																							HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_ITEMICON_SUB] = GFL_CLGRP_CGR_Register(
																									ah, NARC_box_gra_box_itemicon_lz_NCGR,
																									TRUE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
*/
  ah = GFL_ARC_OpenDataHandle( ARCID_ITEMICON, HEAPID_BOX_APP );

  appwk->palRes[BOX2MAIN_PALRES_ITEMICON] = GFL_CLGRP_PLTT_Register(
																							ah, ITEM_GetIndex(0,ITEM_GET_ICON_PAL),
																							CLSYS_DRAW_MAIN, PALNUM_ITEMICON*0x20, HEAPID_BOX_APP );
  appwk->palRes[BOX2MAIN_PALRES_ITEMICON_SUB] = GFL_CLGRP_PLTT_Register(
																									ah, ITEM_GetIndex(0,ITEM_GET_ICON_PAL),
																									CLSYS_DRAW_SUB, PALNUM_ITEMICON_S*0x20, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}



//============================================================================================
//	その他
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ボックス画面リソース読み込み
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void BoxObjResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_BOXOBJ] = GFL_CLGRP_CGR_Register(
																						ah, NARC_box_gra_box_m_obj_lz_NCGR,
																						TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
  appwk->palRes[BOX2MAIN_PALRES_BOXOBJ] = GFL_CLGRP_PLTT_Register(
																						ah, NARC_box_gra_box_m_obj2_NCLR,
																						CLSYS_DRAW_MAIN, PALNUM_BOXOBJ*0x20, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_BOXOBJ] = GFL_CLGRP_CELLANIM_Register(
																						ah,
																						NARC_box_gra_box_m_obj_NCER,
																						NARC_box_gra_box_m_obj_NANR,
																						HEAPID_BOX_APP );
  GFL_ARC_CloseDataHandle( ah );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * ボックス画面OBJ追加
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxObjAdd( BOX2_APP_WORK * appwk )
{
	appwk->clwk[BOX2OBJ_ID_L_ARROW] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_L_ARROW] );
	appwk->clwk[BOX2OBJ_ID_R_ARROW] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_R_ARROW] );
	appwk->clwk[BOX2OBJ_ID_BOXMV_LA] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_BOXMV_LA] );
	appwk->clwk[BOX2OBJ_ID_BOXMV_RA] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_BOXMV_RA] );
	appwk->clwk[BOX2OBJ_ID_TRAY_CUR] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TRAY_CUR] );
	appwk->clwk[BOX2OBJ_ID_TRAY_NAME] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TRAY_NAME] );
	appwk->clwk[BOX2OBJ_ID_TRAY_ARROW] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TRAY_ARROW] );
	appwk->clwk[BOX2OBJ_ID_HAND_CURSOR] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_HAND_CURSOR] );
	appwk->clwk[BOX2OBJ_ID_HAND_SHADOW] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_HAND_SHADOW] );
	appwk->clwk[BOX2OBJ_ID_ITEMICON] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_ITEMICON] );
	appwk->clwk[BOX2OBJ_ID_ITEMICON_SUB] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_ITEMICON_SUB] );
	appwk->clwk[BOX2OBJ_ID_TYPEICON1] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TYPEICON1] );
	appwk->clwk[BOX2OBJ_ID_TYPEICON2] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TYPEICON2] );

	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_HAND_SHADOW, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_ITEMICON, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_ITEMICON_SUB, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_TYPEICON1, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_TYPEICON2, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * 手カーソル表示切替
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	flg		ON = 表示、OFF = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_HandCursorVanish( BOX2_APP_WORK * appwk, BOOL flg )
{
//	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_HAND_CURSOR, flg );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * 手カーソル移動
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	px		Ｘ座標
 * @param	py		Ｙ座標
 * @param	shadow	影表示フラグ ON = 表示、OFF = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_HandCursorSet( BOX2_APP_WORK * appwk, s16 px, s16 py, BOOL shadow )
{
	GFL_CLACT_WK_SetWldPosCap( appwk->cap[BOX2OBJ_ID_HAND_CURSOR], px, py );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( appwk->cap[BOX2OBJ_ID_HAND_SHADOW], px, py+24 );
//↑[GS_CONVERT_TAG]
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_HAND_SHADOW, shadow );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * アウトラインカーソル追加
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_PokeCursorAdd( BOX2_SYS_WORK * syswk )
{
	BOX2_APP_WORK * appwk;
	s16	x, y;
	u16	posID;
	u16	i;

	if( syswk->get_pos == BOX2MAIN_GETPOS_NONE ){
		return;
	}

	appwk = syswk->app;
	posID = appwk->pokeicon_id[ syswk->get_pos ];
	GFL_CLACT_WK_GetPosCap( appwk->cap[posID], &x, &y );
//↑[GS_CONVERT_TAG]

	// 新規追加
	if( appwk->cap[BOX2OBJ_ID_OUTLINE] == NULL ){
		TCATS_OBJECT_ADD_PARAM_S	prm = ClaPokeIconParam;
		prm.pri = GFL_CLACT_WK_GetSoftPriCap( appwk->cap[posID] ) + 1;
//↑[GS_CONVERT_TAG]
		prm.bg_pri = CATS_ObjectBGPriGetCap( appwk->cap[posID] );
//↑[GS_CONVERT_TAG]
		prm.id[0] = CHR_ID_POKEICON + posID - BOX2OBJ_ID_POKEICON;

		appwk->obj_trans_stop = 1;
		for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
			prm.x = x + PokeCursorXTbl[i];
			prm.y = y + PokeCursorYTbl[i];

			appwk->cap[BOX2OBJ_ID_OUTLINE+i] = CATS_ObjectAdd_S( appwk->csp, appwk->crp, &prm );
			CATS_ObjectPaletteSetCap( appwk->cap[BOX2OBJ_ID_OUTLINE+i], PALNUM_OUTLINE );
		}
		appwk->obj_trans_stop = 0;
	// キャラを書き換え
	}else{
		NNSG2dImageProxy * ipc;
		int	obj_pri, bg_pri;

		ipc = CLACT_ImageProxyGet( appwk->cap[posID]->act );

		obj_pri = GFL_CLACT_WK_GetSoftPriCap( appwk->cap[posID] ) + 1;
//↑[GS_CONVERT_TAG]
		bg_pri  = CATS_ObjectBGPriGetCap( appwk->cap[posID] );
//↑[GS_CONVERT_TAG]

		for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
			CLACT_ImageProxySet( appwk->cap[BOX2OBJ_ID_OUTLINE+i]->act, ipc );
			GFL_CLACT_WK_SetWldPosCap(
//↑[GS_CONVERT_TAG]
				appwk->cap[BOX2OBJ_ID_OUTLINE+i],
				x + PokeCursorXTbl[i], y + PokeCursorYTbl[i] );
			BOX2OBJ_ObjPriChange( syswk->app, BOX2OBJ_ID_OUTLINE+i, obj_pri );
			GFL_CLACT_WK_SetBgPriCap( syswk->app->cap[BOX2OBJ_ID_OUTLINE+i], bg_pri );
//↑[GS_CONVERT_TAG]
			GFL_CLACT_UNIT_SetDrawEnableCap( syswk->app->cap[BOX2OBJ_ID_OUTLINE+i], TRUE );
//↑[GS_CONVERT_TAG]
		}
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * アウトラインカーソル削除
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_PokeCursorDel( BOX2_SYS_WORK * syswk )
{
//	BOX2OBJ_PokeCursorVanish( syswk, FALSE );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * アウトラインカーソル表示切り替え
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	flg		表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_PokeCursorVanish( BOX2_SYS_WORK * syswk, BOOL flg )
{
	u32	i;

	if( flg == TRUE ){
		flg = ACT_ON;
	}else{
		flg = ACT_OFF;
	}

	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		if( syswk->app->cap[BOX2OBJ_ID_OUTLINE+i] != NULL ){
			GFL_CLACT_UNIT_SetDrawEnableCap( syswk->app->cap[BOX2OBJ_ID_OUTLINE+i], flg );
			if( flg == TRUE && syswk->get_pos != BOX2MAIN_GETPOS_NONE ){
				int	pri;
				u32	posID;
				posID = syswk->app->pokeicon_id[ syswk->get_pos ];

				pri = GFL_CLACT_WK_GetSoftPriCap( syswk->app->cap[posID] );
				BOX2OBJ_ObjPriChange( syswk->app, BOX2OBJ_ID_OUTLINE+i, pri+1 );

				pri = CATS_ObjectBGPriGetCap( syswk->app->cap[posID] );
				GFL_CLACT_WK_SetBgPriCap( syswk->app->cap[BOX2OBJ_ID_OUTLINE+i], pri );
			}
		}
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * アウトラインカーソル移動
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_PokeCursorMove( BOX2_SYS_WORK * syswk )
{
	BOX2_APP_WORK * appwk;
	s16	x, y;
	u16	posID;
	u16	i;

	if( syswk->get_pos == BOX2MAIN_GETPOS_NONE ){
		return;
	}

	appwk = syswk->app;
	posID = appwk->pokeicon_id[ syswk->get_pos ];
	GFL_CLACT_WK_GetPosCap( appwk->cap[posID], &x, &y );

	// アウトライン
	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		GFL_CLACT_WK_SetWldPosCap(
			appwk->cap[BOX2OBJ_ID_OUTLINE+i], x + PokeCursorXTbl[i], y + PokeCursorYTbl[i] );
	}
}
*/

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンリソース読み込み
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
static void TrayIconResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
	u32	i;

	ah = GFL_ARC_OpenDataHandle( ARCID_BOX2_GRA, HEAPID_BOX_APP );

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_TRAYICON+i] = GFL_CLGRP_CGR_Register(
																									ah, NARC_box_gra_box_tray_lz_NCGR,
																									TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	}
  appwk->palRes[BOX2MAIN_PALRES_TRAYICON] = GFL_CLGRP_PLTT_Register(
																							ah, NARC_box_gra_box_tray_NCLR,
																							CLSYS_DRAW_MAIN, PALNUM_TRAYICON*0x20, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_TRAYICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_box_tray_NCER,
																							NARC_box_gra_box_tray_NANR,
																							HEAPID_BOX_APP );
  GFL_ARC_CloseDataHandle( ah );
}
*/

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンOBJ追加
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TrayObjAdd( BOX2_APP_WORK * appwk )
{
	BOX_CLWK_DATA	prm;
	u32	i;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		prm = ClaTrayIconParam;
		prm.dat.pos_x += BOX2OBJ_TRAYICON_SX * i;
		prm.chrRes = BOX2MAIN_CHRRES_TRAYICON + i;
		appwk->clwk[BOX2OBJ_ID_TRAYICON+i] = ClactWorkCreate( appwk, &prm );
	}
/*
	TCATS_OBJECT_ADD_PARAM_S	prm;
	u32	i;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		prm = ClaTrayIconParam;
		prm.x += BOX2OBJ_TRAYICON_SX * i;
		prm.id[0] = CHR_ID_TRAYICON + i;
		appwk->cap[BOX2OBJ_ID_TRAYICON+i] = CATS_ObjectAdd_S( appwk->csp, appwk->crp, &prm );
	}
*/
}

//--------------------------------------------------------------------------------------------
/**
 * トレイ切り替え矢印表示切替
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	flg		ON/OFFフラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
/*
void BOX2OBJ_TrayMoveArrowVanish( BOX2_APP_WORK * appwk, BOOL flg )
{
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_L_ARROW, flg );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_R_ARROW, flg );
}
*/







































#if 0
//↑[GS_CONVERT_TAG]

#include "system/softsprite.h"
#include "system/gra_tool.h"
#include "poketool/poke_tool.h"
#include "poketool/monsno.h"
#include "poketool/pokeicon.h"
#include "itemtool/item.h"
#include "battle/wazatype_icon.h"

#include "../../battle/graphic/batt_obj_gs_def.h"

#include "box2_main.h"
#include "box2_obj.h"
#include "box_gra.naix"

#include "application/app_tool.h"





//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void ClactResManInit( BOX2_APP_WORK * appwk );
static void ClactResManExit( BOX2_APP_WORK * appwk );
static void ClactResLoad( BOX2_APP_WORK * appwk );
static void ClactAdd( BOX2_SYS_WORK * syswk );
static void ClactDel( BOX2_APP_WORK * appwk, u32 id );
static void ClactDelAll( BOX2_APP_WORK * appwk );

static void PokeGraDummyResLoad( BOX2_APP_WORK * appwk );
static void PokeGraObjAdd( BOX2_SYS_WORK * syswk );

static void PokeGraSet( BOX2_APP_WORK * appwk, POKEMON_PASO_PARAM * ppp, u32 id );

static void PokeIconResLoad( BOX2_APP_WORK * appwk );
static void PokeIconObjAdd( BOX2_APP_WORK * appwk );

static void BoxObjResLoad( BOX2_APP_WORK * appwk );
static void BoxObjAdd( BOX2_APP_WORK * appwk );

static void TrayIconResLoad( BOX2_APP_WORK * appwk );
static void TrayObjAdd( BOX2_APP_WORK * appwk );

static void WallPaperIconResLoad( BOX2_APP_WORK * appwk );
static void WallPaperObjAdd( BOX2_APP_WORK * appwk );

static void WallPaperBufferFill( BOX2_SYS_WORK * syswk, u8 * buf, u32 wp, u32 chg_col, u32 siz );
static void CgxChangeTrans( BOX2_APP_WORK * appwk, u32 id, u8 * buf, u32 siz );
static void TrayPokeDotPut( BOX2_SYS_WORK * syswk, u32 tray, u8 * buf );

static void ItemIconDummyResLoad( BOX2_APP_WORK * appwk );
static void ItemIconObjAdd( BOX2_APP_WORK * appwk );

static void TypeIconResLoad( BOX2_APP_WORK * appwk );
static void TypeIconObjAdd( BOX2_APP_WORK * appwk );


//============================================================================================
//	グローバル変数
//============================================================================================


// 手持ちポケモンのアイコン座標テーブル
static const s16 PartyPokeInitPos[6][2] =
{
	{ PARTYPOKE_FRM_PX+24, PARTYPOKE_FRM_PY+16 }, { PARTYPOKE_FRM_PX+64, PARTYPOKE_FRM_PY+24 },
	{ PARTYPOKE_FRM_PX+24, PARTYPOKE_FRM_PY+48 }, { PARTYPOKE_FRM_PX+64, PARTYPOKE_FRM_PY+56 },
	{ PARTYPOKE_FRM_PX+24, PARTYPOKE_FRM_PY+80 }, { PARTYPOKE_FRM_PX+64, PARTYPOKE_FRM_PY+88 },
};

static const u8 BoxNamePosTbl[] = { 80, 96, 112, 144, 160, 176 };	// ボックス移動時の名前表示Ｘ座標テーブル

static const s8	PokeCursorXTbl[] = { 1, -1, 0, 0, 1, 1, -1, -1 };	// アウトラインＸ座標オフセットテーブル
static const s8	PokeCursorYTbl[] = { 0, 0, 1, -1, 1, -1, 1, -1 };	// アウトラインＹ座標オフセットテーブル

// トレイアイコンに描画するポケモンドットのカラー
// 追加壁紙と共用です
static const u8 TrayPokeDotColorPos[] = {
//  赤    青    黄    緑    黒    茶    紫    灰    白    桃
//	0x0e, 0x0f, 0x05, 0x01, 0x0d, 0x0c, 0x06, 0x0b, 0x0a, 0x09,
	0x0e, 0x0f, 0x05, 0x02, 0x0d, 0x0c, 0x06, 0x0b, 0x0a, 0x09,
};

// 手持ちポケモン表示座標
static const u8 PartyPokeFramePos[6][2] =
{
	{ 24, 16 }, { 64, 24 },
	{ 24, 48 }, { 64, 56 },
	{ 24, 80 }, { 64, 88 }
};


//============================================================================================
//	システム関連
//============================================================================================



//--------------------------------------------------------------------------------------------
/**
 * リソースマネージャー初期化
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClactResManInit( BOX2_APP_WORK * appwk )
{
	TCATS_RESOURCE_NUM_LIST	crnl = { CHR_ID_MAX, PAL_ID_MAX, CEL_ID_MAX, ANM_ID_MAX, 0, 0 };

	appwk->csp = CATS_AllocMemory( HEAPID_BOX_APP );
	appwk->crp = CATS_ResourceCreate( appwk->csp );
	{
		TCATS_OAM_INIT	coi = {
			0, 128,
			0, 32,
			0, 128,
			0, 32,
		};
		TCATS_CHAR_MANAGER_MAKE ccmm = {
			BOX2OBJ_ID_MAX,
			1024*64,
			1024*16,
			GX_OBJVRAMMODE_CHAR_1D_64K,
			GX_OBJVRAMMODE_CHAR_1D_64K
		};
		CATS_SystemInit( appwk->csp, &coi, &ccmm, 32 );
	}
	CATS_ClactSetInit( appwk->csp, appwk->crp, BOX2OBJ_ID_MAX+FNTOAM_CHR_MAX );
	CATS_ResourceManagerInit( appwk->csp, appwk->crp, &crnl );

	{
		CLACT_U_EASYRENDER_DATA * renddata = CATS_EasyRenderGet( appwk->csp );
		CLACT_U_SetSubSurfaceMatrix( renddata, 0, BOX_SUB_ACTOR_DISTANCE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * リソースマネージャー解放
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClactResManExit( BOX2_APP_WORK * appwk )
{
	CATS_ResourceDestructor_S( appwk->csp, appwk->crp );
	CATS_FreeMemory( appwk->csp );
}




//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンのデフォルト座標取得
 *
 * @param	pos		位置
 * @param	x		Ｘ座標格納場所
 * @param	y		Ｙ座標格納場所
 * @param	mode	ポケモンアイコン移動モード
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconDefaultPosGet( u32 pos, s16 * x, s16 * y, u32 mode )
{
	if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
		*x = BOX2OBJ_TRAYPOKE_PX + ( pos % BOX2OBJ_POKEICON_H_MAX * BOX2OBJ_TRAYPOKE_SX );
		*y = BOX2OBJ_TRAYPOKE_PY + ( pos / BOX2OBJ_POKEICON_H_MAX * BOX2OBJ_TRAYPOKE_SY );
	}else{
		// 手持ち
		pos -= BOX2OBJ_POKEICON_TRAY_MAX;
		*x = PartyPokeInitPos[pos][0];
		*y = PartyPokeInitPos[pos][1] - BOX2MAIN_PARTYPOKE_FRM_SPD * BOX2MAIN_PARTYPOKE_FRM_CNT;
		if( mode == BOX2MAIN_POKEMOVE_MODE_ALL ){
			*x = *x + 152;
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンにデフォルト座標を設定
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	pos		位置
 * @param	id		OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconDefaultPosSet( BOX2_SYS_WORK * syswk, u32 pos, u32 objID )
{
	s16	x, y;

	BOX2OBJ_PokeIconDefaultPosGet( pos, &x, &y, syswk->move_mode );
	GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[objID], x, y );
//↑[GS_CONVERT_TAG]
	BOX2OBJ_ObjPriChange( syswk->app, objID, POKEICON_OBJ_PRI(pos) );
}

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンアイコンをフレームイン前の座標に移動（左）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconFrmInSet( BOX2_SYS_WORK * syswk )
{
	u32	ppcnt;
	u32	id;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->party );

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_TRAY_MAX+i ];
		GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[id], PartyPokeInitPos[i][0], PartyPokeInitPos[i][1] );
//↑[GS_CONVERT_TAG]
		BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_TRAY_MAX+i, BOX2OBJ_POKEICON_PRI_CHG_PUT );
		if( i < ppcnt ){
			BOX2OBJ_Vanish( syswk->app, id, TRUE );
		}else{
			BOX2OBJ_Vanish( syswk->app, id, FALSE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンアイコンをフレームイン前の座標に移動（右）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconFrmInSetRight( BOX2_SYS_WORK * syswk )
{
	u32	ppcnt;
	u32	id;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->party );

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_TRAY_MAX+i ];
		GFL_CLACT_WK_SetWldPosCap(
//↑[GS_CONVERT_TAG]
			syswk->app->cap[id],
			PartyPokeInitPos[i][0]+BOX2MAIN_PARTYPOKE_FRM_H_SPD*BOX2MAIN_PARTYPOKE_FRM_H_CNT,
			PartyPokeInitPos[i][1] );
		BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_TRAY_MAX+i, BOX2OBJ_POKEICON_PRI_CHG_PUT );
		if( i < ppcnt ){
			BOX2OBJ_Vanish( syswk->app, id, TRUE );
		}else{
			BOX2OBJ_Vanish( syswk->app, id, FALSE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンアイコンをフレームイン後の座標に移動
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconFrmSet( BOX2_SYS_WORK * syswk )
{
	u32	ppcnt;
	u32	id;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->party );

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_TRAY_MAX+i ];
		GFL_CLACT_WK_SetWldPosCap(
//↑[GS_CONVERT_TAG]
			syswk->app->cap[id],
			PartyPokeInitPos[i][0],
			PartyPokeInitPos[i][1] - BOX2MAIN_PARTYPOKE_FRM_SPD * BOX2MAIN_PARTYPOKE_FRM_CNT );
		BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_TRAY_MAX+i, BOX2OBJ_POKEICON_PRI_CHG_PUT );
		if( i < ppcnt ){
			BOX2OBJ_Vanish( syswk->app, id, TRUE );
		}else{
			BOX2OBJ_Vanish( syswk->app, id, FALSE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンアイコンをフレームイン後の座標に移動（右）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconFrmSetRight( BOX2_SYS_WORK * syswk )
{
	u32	ppcnt;
	u32	id;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->party );

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_TRAY_MAX+i ];
		GFL_CLACT_WK_SetWldPosCap(
//↑[GS_CONVERT_TAG]
			syswk->app->cap[id],
			PartyPokeInitPos[i][0] + BOX2MAIN_PARTYPOKE_FRM_H_SPD * BOX2MAIN_PARTYPOKE_FRM_H_CNT,
			PartyPokeInitPos[i][1] - BOX2MAIN_PARTYPOKE_FRM_SPD * BOX2MAIN_PARTYPOKE_FRM_CNT );
		BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_TRAY_MAX+i, BOX2OBJ_POKEICON_PRI_CHG_PUT );
		if( i < ppcnt ){
			BOX2OBJ_Vanish( syswk->app, id, TRUE );
		}else{
			BOX2OBJ_Vanish( syswk->app, id, FALSE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンのプライオリティを変更
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	pos		位置
 * @param	flg		取得か配置か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconPriChg( BOX2_APP_WORK * appwk, u32 pos, u32 flg )
{
	u32	id = appwk->pokeicon_id[pos];

	if( flg == BOX2OBJ_POKEICON_PRI_CHG_GET ){
		BOX2OBJ_BgPriChange( appwk, id, POKEICON_BG_PRI_GET );
		BOX2OBJ_ObjPriChange( appwk, id, POKEICON_OBJ_PRI_GET );
	}else{
		if( pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2OBJ_BgPriChange( appwk, id, POKEICON_TBG_PRI_PUT );
		}else{
			BOX2OBJ_BgPriChange( appwk, id, POKEICON_PBG_PRI_PUT );
		}
		BOX2OBJ_ObjPriChange( appwk, id, POKEICON_OBJ_PRI_PUT(pos) );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンのプライオリティを変更（掴んだ手持ちをトレイに落とすとき）
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	dp		掴んだ位置
 * @param	mp		配置する位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconPriChgDrop( BOX2_APP_WORK * appwk, u32 dp, u32 mp )
{
	u32	id = appwk->pokeicon_id[dp];

	if( mp < BOX2OBJ_POKEICON_TRAY_MAX ){
		BOX2OBJ_BgPriChange( appwk, id, POKEICON_TBG_PRI_PUT );
	}else{
		BOX2OBJ_BgPriChange( appwk, id, POKEICON_PBG_PRI_PUT );
	}
	BOX2OBJ_ObjPriChange( appwk, id, POKEICON_OBJ_PRI_GET );
}


//--------------------------------------------------------------------------------------------
/**
 * トレイのポケモンアイコンスクロール処理
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	mv		移動量
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayPokeIconScroll( BOX2_SYS_WORK * syswk, s8 mv )
{
	s16	x, y;
	s16	sp, ep;
	u16	id;
	u16	i;

	if( mv >= 0 ){
		ep = TRAY_L_SCROLL_ICON_EX;
		sp = TRAY_L_SCROLL_ICON_SX;
	}else{
		ep = TRAY_R_SCROLL_ICON_EX;
		sp = TRAY_R_SCROLL_ICON_SX;
	}

	for( i=0; i<BOX2OBJ_POKEICON_TRAY_MAX; i++ ){
		id = syswk->app->pokeicon_id[i];
		GFL_CLACT_WK_GetPosCap( syswk->app->cap[id], &x, &y );
//↑[GS_CONVERT_TAG]
		x += mv;
		if( x == ep ){
			x = sp;
			PokeIconChangeCore(
				syswk->app, syswk->app->pokeicon_cgx[i], id, syswk->app->pokeicon_pal[i] );
			GFL_CLACT_UNIT_SetDrawEnableCap( syswk->app->cap[id], syswk->app->pokeicon_exist[i] );
//↑[GS_CONVERT_TAG]
			if( syswk->dat->mode == BOX_MODE_ITEM ){
				BOX2OBJ_PokeIconBlendSetItem( syswk, i );
			}
		}
		GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[id], x, y );
//↑[GS_CONVERT_TAG]
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 手持ちのポケモンアイコンスクロール処理
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	mx		移動量Ｘ
 * @param	my		移動量Ｙ
 *
 * @return	none
 *
 *	※使用しているところ無し
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconScroll( BOX2_SYS_WORK * syswk, s8 mx, s8 my )
{
	s16	x, y;
	u16	id;
	u16	i;

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[BOX2OBJ_POKEICON_TRAY_MAX+i];
		GFL_CLACT_WK_GetPosCap( syswk->app->cap[id], &x, &y );
//↑[GS_CONVERT_TAG]
		x += mx;
		y += my;
		GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[id], x, y );
//↑[GS_CONVERT_TAG]
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 手持ちのポケモンアイコンスクロール処理
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconScroll2( BOX2_SYS_WORK * syswk )
{
	u16	id;
	u16	i;
	s16	px, py;
	s8	wfrm_x, wfrm_y;

	BGWINFRM_PosGet( syswk->app->wfrmwk, BOX2MAIN_WINFRM_PARTY, &wfrm_x, &wfrm_y );
	px = wfrm_x * 8;
	py = wfrm_y * 8;

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[BOX2OBJ_POKEICON_TRAY_MAX+i];
		GFL_CLACT_WK_SetWldPosCap(
//↑[GS_CONVERT_TAG]
			syswk->app->cap[id],
			px + PartyPokeFramePos[i][0],
			py + PartyPokeFramePos[i][1] );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンを逃がす：ポケモンアイコン動作初期化
 *
 * @param	wk		逃がすワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconFreeStart( BOX2MAIN_POKEFREE_WORK * wk )
{
	wk->scale     = POKEICON_FREE_SCALE_DEF;
	wk->scale_cnt = 0;

	GFL_CLACT_WK_SetAffineParamCap( wk->cap, CLACT_AFFINE_NORMAL );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetScaleCap( wk->cap, wk->scale, wk->scale );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetAffinePos( wk->cap, 0, POKEICON_FREE_CY );
//↑[GS_CONVERT_TAG]
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンを逃がす：ポケモンアイコン縮小処理
 *
 * @param	wk		逃がすワーク
 *
 * @retval	"TRUE = 処理中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2OBJ_PokeIconFreeMove( BOX2MAIN_POKEFREE_WORK * wk )
{
	wk->scale_cnt++;
	wk->scale -= POKEICON_FREE_SCALE_PRM;

	if( wk->scale_cnt == POKEICON_FREE_CNT_MAX ){
		return FALSE;
	}

	GFL_CLACT_WK_SetScaleCap( wk->cap, wk->scale, wk->scale );
//↑[GS_CONVERT_TAG]
	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンを逃がす：ポケモンアイコン拡大処理
 *
 * @param	wk		逃がすワーク
 *
 * @retval	"TRUE = 処理中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2OBJ_PokeIconFreeErrorMove( BOX2MAIN_POKEFREE_WORK * wk )
{
	wk->scale_cnt -= 2;
	wk->scale += POKEICON_FREE_SCALE_PRM;
	wk->scale += POKEICON_FREE_SCALE_PRM;

	if( wk->scale_cnt == 0 ){
		return FALSE;
	}

	GFL_CLACT_WK_SetScaleCap( wk->cap, wk->scale, wk->scale );
//↑[GS_CONVERT_TAG]
	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンを逃がす：ポケモンアイコン終了設定
 *
 * @param	wk		逃がすワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconFreeEnd( BOX2MAIN_POKEFREE_WORK * wk )
{
	GFL_CLACT_UNIT_SetDrawEnableCap( wk->cap, ACT_OFF );
//↑[GS_CONVERT_TAG]
	BOX2OBJ_PokeIconFreeErrorEnd( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンを逃がす：ポケモンアイコンエラー終了設定
 *
 * @param	wk		逃がすワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconFreeErrorEnd( BOX2MAIN_POKEFREE_WORK * wk )
{
	GFL_CLACT_WK_SetAffinePos( wk->cap, 0, 0 );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetScaleCap( wk->cap, POKEICON_FREE_SCALE_DEF, POKEICON_FREE_SCALE_DEF );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetAffineParamCap( wk->cap, CLACT_AFFINE_NONE );
//↑[GS_CONVERT_TAG]
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンに半透明設定
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	flg		ON/OFFフラグ
 * @param	start	開始アイコン位置
 * @param	end		終了アイコン位置
 * @param	item	持ち物に影響されるか
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconBlendSetCore( BOX2_SYS_WORK * syswk, BOOL flg, u32 start, u32 end, BOOL item )
{
	u32	i;

	if( item == TRUE ){
		for( i=start; i<end; i++ ){
			if( BOX2MAIN_PokeParaGet( syswk, i, ID_PARA_item, NULL ) == 0 ){
				BOX2OBJ_BlendModeSet( syswk->app, syswk->app->pokeicon_id[i], TRUE );
			}else{
				BOX2OBJ_BlendModeSet( syswk->app, syswk->app->pokeicon_id[i], FALSE );
			}
		}
	}else{
		for( i=start; i<end; i++ ){
			BOX2OBJ_BlendModeSet( syswk->app, syswk->app->pokeicon_id[i], flg );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンに半透明設定（全体）
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	type	アイコン位置
 * @param	flg		ON/OFFフラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconBlendSetAll( BOX2_SYS_WORK * syswk, u32 type, BOOL flg )
{
	BOOL item;

	if( type & BOX2OBJ_BLENDTYPE_ITEM ){
		item = TRUE;
	}else{
		item = FALSE;
	}

	if( ( type & BOX2OBJ_BLENDTYPE_TRAYPOKE	) != 0 ){
		PokeIconBlendSetCore( syswk, flg, 0, BOX2OBJ_POKEICON_TRAY_MAX, item );
	}
	if( ( type & BOX2OBJ_BLENDTYPE_PARTYPOKE ) != 0 ){
		PokeIconBlendSetCore( syswk, flg, BOX2OBJ_POKEICON_TRAY_MAX, BOX2OBJ_POKEICON_MAX, item );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンに半透明設定
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	pos		アイコン位置
 * @param	flg		ON/OFFフラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconBlendSet( BOX2_APP_WORK * appwk, u32 pos, BOOL flg )
{
	BOX2OBJ_BlendModeSet( appwk, appwk->pokeicon_id[pos], flg );
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンアイコンに半透明設定（持ち物整理のトレイスクロール用）
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	pos		アイコン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconBlendSetItem( BOX2_SYS_WORK * syswk, u32 pos )
{
	PokeIconBlendSetCore( syswk, TRUE, pos, pos+1, TRUE );
}



//--------------------------------------------------------------------------------------------
/**
 * ポケモングラフィックOBJ追加
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeGraObjAdd( BOX2_SYS_WORK * syswk )
{
	BOX2_APP_WORK * appwk = syswk->app;

	appwk->cap[BOX2OBJ_ID_POKEGRA]  = CATS_ObjectAdd_S_SubDistance(
										appwk->csp, appwk->crp,
										&ClactParamTbl[BOX2OBJ_ID_POKEGRA],
										BOX_SUB_ACTOR_DISTANCE );
	appwk->cap[BOX2OBJ_ID_POKEGRA2] = CATS_ObjectAdd_S_SubDistance(
										appwk->csp, appwk->crp,
										&ClactParamTbl[BOX2OBJ_ID_POKEGRA2],
										BOX_SUB_ACTOR_DISTANCE );

	GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_POKEGRA], ACT_OFF );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_POKEGRA2], ACT_OFF );
//↑[GS_CONVERT_TAG]
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモングラフィック切り替え
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	info	表示データ
 * @param	id		OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeGraChange( BOX2_APP_WORK * appwk, BOX2_POKEINFO_DATA * info, u32 id )
{
	SOFT_SPRITE_ARC  ssa;
	void * buf;
	NNSG2dImageProxy * ipc;
	NNSG2dImagePaletteProxy * ipp;
	u32	cgx;
	u32	pal;
	CHANGES_INTO_DATA_RECT rect = { POKE_TEX_X, POKE_TEX_Y, POKE_TEX_W, POKE_TEX_H, };

	buf = GFL_HEAP_AllocMemoryLo( HEAPID_BOX_APP, POKEGRA_TEX_SIZE );
//↑[GS_CONVERT_TAG]

	PokeGraArcDataGetPPP( &ssa, info->ppp, PARA_FRONT, FALSE );

	Ex_ChangesInto_OAM_from_PokeTex_RC(
		ssa.arc_no,
		ssa.index_chr,
		HEAPID_BOX_APP,
		&rect,
		buf,
		info->rand,
		FALSE,
		PARA_FRONT,
		info->mons );

	ipc = CLACT_ImageProxyGet( appwk->cap[id]->act );
	cgx = NNS_G2dGetImageLocation( ipc, NNS_G2D_VRAM_TYPE_2DSUB );

	DC_FlushRange( buf, POKEGRA_TEX_SIZE );
	GXS_LoadOBJ( buf, cgx, POKEGRA_TEX_SIZE );

	ipp = GFL_CLACT_WK_GetPlttProxy( appwk->cap[id]->act );
//↑[GS_CONVERT_TAG]
	pal = NNS_G2dGetImagePaletteLocation( ipp, NNS_G2D_VRAM_TYPE_2DSUB );

	GFL_ARC_UTIL_TransVramPalette(
//↑[GS_CONVERT_TAG]
		ssa.arc_no, ssa.index_pal, PALTYPE_SUB_OBJ, pal, POKEGRA_PAL_SIZE, HEAPID_BOX_APP );

	GFL_HEAP_FreeMemory( buf );
//↑[GS_CONVERT_TAG]
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモングラフィック切り替え
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	info	表示データ
 * @param	id		OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeGraChange( BOX2_SYS_WORK * syswk, BOX2_POKEINFO_DATA * info, u32 id )
{
	id += syswk->app->pokegra_swap;

	PokeGraChange( syswk->app, info, id );
	GFL_CLACT_UNIT_SetDrawEnableCap( syswk->app->cap[id], ACT_ON );
//↑[GS_CONVERT_TAG]

	syswk->app->pokegra_swap ^= 1;
}




//--------------------------------------------------------------------------------------------
/**
 * アイテムグラフィック切り替え
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	item	アイテム番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconChange( BOX2_APP_WORK * appwk, u16 item )
{
	NNSG2dCharacterData * chr;
	NNSG2dImagePaletteProxy * ipp;
	void * buf;
	u32	pal;

	buf = GFL_ARC_UTIL_LoadBGCharacter or GFL_ARC_UTIL_LoadOBJCharacter( ARC_ITEMICON, GetItemIndex(item,ITEM_GET_ICON_CGX), 0, &chr, HEAPID_BOX_APP );
//↑[GS_CONVERT_TAG]
	CgxChangeTrans( appwk, BOX2OBJ_ID_ITEMICON, chr->pRawData, ITEM_ICON_CGX_SIZE );
	GFL_HEAP_FreeMemory( buf );
//↑[GS_CONVERT_TAG]

	ipp = GFL_CLACT_WK_GetPlttProxy( appwk->cap[BOX2OBJ_ID_ITEMICON]->act );
//↑[GS_CONVERT_TAG]
	pal = NNS_G2dGetImagePaletteLocation( ipp, NNS_G2D_VRAM_TYPE_2DMAIN );

	GFL_ARC_UTIL_TransVramPalette(
//↑[GS_CONVERT_TAG]
		ARC_ITEMICON,
		GetItemIndex(item,ITEM_GET_ICON_PAL),
		PALTYPE_MAIN_OBJ, pal, 0x20, HEAPID_BOX_APP );
}

//--------------------------------------------------------------------------------------------
/**
 * アイテムグラフィック切り替え（サブ画面用）
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	item	アイテム番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconChangeSub( BOX2_APP_WORK * appwk, u16 item )
{
	NNSG2dCharacterData * chr;
	NNSG2dImagePaletteProxy * ipp;
	void * buf;
	u32	pal;

	buf = GFL_ARC_UTIL_LoadBGCharacter or GFL_ARC_UTIL_LoadOBJCharacter( ARC_ITEMICON, GetItemIndex(item,ITEM_GET_ICON_CGX), 0, &chr, HEAPID_BOX_APP );
//↑[GS_CONVERT_TAG]
	CgxChangeTransSub( appwk, BOX2OBJ_ID_ITEMICON_SUB, chr->pRawData, ITEM_ICON_CGX_SIZE );
	GFL_HEAP_FreeMemory( buf );
//↑[GS_CONVERT_TAG]

	ipp = GFL_CLACT_WK_GetPlttProxy( appwk->cap[BOX2OBJ_ID_ITEMICON_SUB]->act );
//↑[GS_CONVERT_TAG]
	pal = NNS_G2dGetImagePaletteLocation( ipp, NNS_G2D_VRAM_TYPE_2DSUB );

	GFL_ARC_UTIL_TransVramPalette(
//↑[GS_CONVERT_TAG]
		ARC_ITEMICON,
		GetItemIndex(item,ITEM_GET_ICON_PAL),
		PALTYPE_SUB_OBJ, pal, 0x20, HEAPID_BOX_APP );
}

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコン拡縮設定
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	flg		TRUE = 拡縮, FALSE = 通常
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconAffineSet( BOX2_APP_WORK * appwk, BOOL flg )
{
	if( flg == TRUE ){
		GFL_CLACT_WK_SetAffineParamCap( appwk->cap[BOX2OBJ_ID_ITEMICON], CLACT_AFFINE_NORMAL );
//↑[GS_CONVERT_TAG]
		GFL_CLACT_WK_SetAffinePos( appwk->cap[BOX2OBJ_ID_ITEMICON], 12, 12 );
//↑[GS_CONVERT_TAG]
	}else{
		GFL_CLACT_WK_SetAffinePos( appwk->cap[BOX2OBJ_ID_ITEMICON], 0, 0 );
//↑[GS_CONVERT_TAG]
		GFL_CLACT_WK_SetAffineParamCap( appwk->cap[BOX2OBJ_ID_ITEMICON], CLACT_AFFINE_NONE );
//↑[GS_CONVERT_TAG]
	}
}

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコン座標設定
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	x		Ｘ座標
 * @param	y		Ｙ座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconPosSet( BOX2_APP_WORK * appwk, s16 x, s16 y )
{
	GFL_CLACT_WK_SetWldPosCap( appwk->cap[BOX2OBJ_ID_ITEMICON], x, y );
//↑[GS_CONVERT_TAG]
}

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコン移動
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	mx		Ｘ座標移動量
 * @param	my		Ｙ座標移動量
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconMove( BOX2_APP_WORK * appwk, s16 mx, s16 my )
{
	s16	px, py;

	GFL_CLACT_WK_GetPosCap( appwk->cap[BOX2OBJ_ID_ITEMICON], &px, &py );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( appwk->cap[BOX2OBJ_ID_ITEMICON], px+mx, py+my );
//↑[GS_CONVERT_TAG]
}

//--------------------------------------------------------------------------------------------
/**
 * 指定ポケモンアイコンの位置にアイテムアイコンをセット
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	poke	ポケモンアイコン位置
 * @param	mode	ポケモンアイコン表示モード
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconPokePut( BOX2_APP_WORK * appwk, u32 poke, u32 mode )
{
	s16	x, y;

	BOX2OBJ_PokeIconDefaultPosGet( poke, &x, &y, mode );
	BOX2OBJ_ItemIconPosSet( appwk, x+8, y+8 );
}

//--------------------------------------------------------------------------------------------
/**
 * 指定ポケモンアイコンの位置にアイテムアイコンの掴んだ位置をセット
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	poke	ポケモンアイコン位置
 * @param	mode	ポケモンアイコン表示モード
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconPokePutHand( BOX2_APP_WORK * appwk, u32 poke, u32 mode )
{
	s16	x, y;

	BOX2OBJ_PokeIconDefaultPosGet( poke, &x, &y, mode );
	BOX2OBJ_ItemIconPosSet( appwk, x, y+4 );
}

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコンのアウトラインカーソルを追加
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconCursorAdd( BOX2_APP_WORK * appwk )
{
	s16	x, y;
	u32	i;

	if( appwk->get_item == ITEM_DUMMY_DATA ){
		return;
	}

	GFL_CLACT_WK_GetPosCap( appwk->cap[BOX2OBJ_ID_ITEMICON], &x, &y );
//↑[GS_CONVERT_TAG]

	// 新規追加
	if( appwk->cap[BOX2OBJ_ID_OUTLINE] == NULL ){
		TCATS_OBJECT_ADD_PARAM_S	prm = ClactParamTbl[BOX2OBJ_ID_ITEMICON];
		prm.pri = GFL_CLACT_WK_GetSoftPriCap( appwk->cap[BOX2OBJ_ID_ITEMICON] ) + 1;
//↑[GS_CONVERT_TAG]
		prm.bg_pri = CATS_ObjectBGPriGetCap( appwk->cap[BOX2OBJ_ID_ITEMICON] );
//↑[GS_CONVERT_TAG]
		prm.id[0] = CHR_ID_ITEMICON;

		// アウトライン
		appwk->obj_trans_stop = 1;
		for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
			prm.x = x + PokeCursorXTbl[i];
			prm.y = y + PokeCursorYTbl[i];

			appwk->cap[BOX2OBJ_ID_OUTLINE+i] = CATS_ObjectAdd_S( appwk->csp, appwk->crp, &prm );
			CATS_ObjectPaletteSetCap( appwk->cap[BOX2OBJ_ID_OUTLINE+i], PALNUM_OUTLINE );
			GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_OUTLINE+i], ACT_OFF );
//↑[GS_CONVERT_TAG]
		}
		appwk->obj_trans_stop = 0;
	}else{
		int	obj_pri, bg_pri;
		
		obj_pri = GFL_CLACT_WK_GetSoftPriCap( appwk->cap[BOX2OBJ_ID_ITEMICON] ) + 1;
//↑[GS_CONVERT_TAG]
		bg_pri  = CATS_ObjectBGPriGetCap( appwk->cap[BOX2OBJ_ID_ITEMICON] );
//↑[GS_CONVERT_TAG]

		// アウトライン
		for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
			GFL_CLACT_WK_SetWldPosCap(
//↑[GS_CONVERT_TAG]
				appwk->cap[BOX2OBJ_ID_OUTLINE+i],
				x + PokeCursorXTbl[i], y + PokeCursorYTbl[i] );
			BOX2OBJ_ObjPriChange( appwk, BOX2OBJ_ID_OUTLINE+i, obj_pri );
			GFL_CLACT_WK_SetBgPriCap( appwk->cap[BOX2OBJ_ID_OUTLINE+i], bg_pri );
//↑[GS_CONVERT_TAG]
			GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_OUTLINE+i], ACT_OFF );
//↑[GS_CONVERT_TAG]
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコンのアウトラインカーソルを表示
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconCursorOn( BOX2_APP_WORK * appwk )
{
	u32	i;

	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_OUTLINE+i], ACT_ON );
//↑[GS_CONVERT_TAG]
	}
}

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコンのアウトラインカーソル移動
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconCursorMove( BOX2_APP_WORK * appwk )
{
	u32	i;
	s16	x, y;

	GFL_CLACT_WK_GetPosCap( appwk->cap[BOX2OBJ_ID_ITEMICON], &x, &y );
//↑[GS_CONVERT_TAG]

	if( appwk->cap[BOX2OBJ_ID_OUTLINE] == NULL ){ return; }

	// アウトライン
	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		GFL_CLACT_WK_SetWldPosCap(
//↑[GS_CONVERT_TAG]
			appwk->cap[BOX2OBJ_ID_OUTLINE+i],
			x + PokeCursorXTbl[i],
			y + PokeCursorYTbl[i] );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * アイテムアイコン配置（サブ画面用）
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconPutSub( BOX2_APP_WORK * appwk )
{
	s16	objy;
	s8	wfmx, wfmy;

	BGWINFRM_PosGet( appwk->wfrmwk, BOX2MAIN_WINFRM_SUBDISP, &wfmx, &wfmy );
	objy = (s16)wfmy * 8 + BOX_SUB_ACTOR_DEF_PY + SUBDISP_ITEMICON_PY;
	GFL_CLACT_WK_SetWldPosCap( appwk->cap[BOX2OBJ_ID_ITEMICON_SUB], SUBDISP_ITEMICON_PX, objy );
//↑[GS_CONVERT_TAG]

	GFL_CLACT_UNIT_SetDrawEnableCap( appwk->cap[BOX2OBJ_ID_ITEMICON_SUB], ACT_ON );
//↑[GS_CONVERT_TAG]
}







//============================================================================================
//	壁紙変更関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更：壁紙名のＯＡＭフォントを移動
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WallPaperChgFntOamPos( BOX2_APP_WORK * appwk )
{
	FONTOAM_SetMat( appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, -48, -8 );
}

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更：ＯＢＪ初期化
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_WallPaperChgObjInit( BOX2_SYS_WORK * syswk )
{
	BOX2_APP_WORK * appwk = syswk->app;

	GFL_CLACT_WK_SetWldPosCap( appwk->cap[BOX2OBJ_ID_BOXMV_LA], BOXMV_LA_PX, BOXMV_LA_PY );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( appwk->cap[BOX2OBJ_ID_BOXMV_RA], BOXMV_RA_PX, BOXMV_RA_PY );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( appwk->cap[BOX2OBJ_ID_TRAY_CUR], BOXMV_TRAY_CUR_PX, BOXMV_TRAY_CUR_PY );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( appwk->cap[BOX2OBJ_ID_TRAY_NAME], BOXMV_TRAY_NAME_PX, BOXMV_TRAY_NAME_PY );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( appwk->cap[BOX2OBJ_ID_TRAY_ARROW], BOXMV_TRAY_ARROW_PX, BOXMV_TRAY_ARROW_PY );
//↑[GS_CONVERT_TAG]

	BOX2OBJ_WallPaperObjChange( syswk );

	BOX2OBJ_AnmSet( appwk, BOX2OBJ_ID_TRAY_NAME, BOX2OBJ_ANM_WP_NAME );

	WallPaperChgFntOamPos( appwk );
	FONTOAM_SetDrawFlag( appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, TRUE );
	FONTOAM_SetDrawFlag( appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NUM].oam, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更：ＯＢＪスクロール
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	mv		移動量
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_WallPaperChgFrmScroll( BOX2_APP_WORK * appwk, s16 mv )
{
	u32	i;
	s16	x, y;

	for( i=BOX2OBJ_ID_BOXMV_LA; i<=BOX2OBJ_ID_TRAY_ARROW; i++ ){
		GFL_CLACT_WK_GetPosCap( appwk->cap[i], &x, &y );
//↑[GS_CONVERT_TAG]
		GFL_CLACT_WK_SetWldPosCap( appwk->cap[i], x, y+mv );
//↑[GS_CONVERT_TAG]
	}
	WallPaperChgFntOamPos( appwk );

	for( i=0; i<BOX2OBJ_WPICON_MAX; i++ ){
		GFL_CLACT_WK_GetPosCap( appwk->cap[BOX2OBJ_ID_WPICON+i], &x, &y );
//↑[GS_CONVERT_TAG]
		GFL_CLACT_WK_SetWldPosCap( appwk->cap[BOX2OBJ_ID_WPICON+i], x, y+mv );
//↑[GS_CONVERT_TAG]
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更：カーソルセット
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_WallPaperCursorSet( BOX2_SYS_WORK * syswk )
{
	u32	wp;
	u32	pos;
	s16	x, y;

	wp = BOX2MAIN_GetWallPaperNumber( syswk, syswk->box_mv_pos );

	if( (syswk->app->wallpaper_pos/BOX2OBJ_WPICON_MAX) == (wp/BOX2OBJ_WPICON_MAX) ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, TRUE );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, FALSE );
	}

	pos = wp % BOX2OBJ_WPICON_MAX;

	GFL_CLACT_WK_GetPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_CUR], &x, &y );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap(
//↑[GS_CONVERT_TAG]
		syswk->app->cap[BOX2OBJ_ID_TRAY_CUR],
		BOX2OBJ_WPICON_PX+BOX2OBJ_WPICON_SX*pos, y );
}

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更：名前セット
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_WallPaperNameSet( BOX2_SYS_WORK * syswk )
{
	s16	x, y, px;
	u16	pos;

	pos = syswk->app->wallpaper_pos % BOX2OBJ_WPICON_MAX;

	GFL_CLACT_WK_GetPosCap( syswk->app->cap[BOX2OBJ_ID_WPICON+pos], &px, &y );
//↑[GS_CONVERT_TAG]

	GFL_CLACT_WK_GetPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_NAME], &x, &y );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_NAME], px, y );
//↑[GS_CONVERT_TAG]

	GFL_CLACT_WK_GetPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_ARROW], &x, &y );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_ARROW], px, y );
//↑[GS_CONVERT_TAG]

	WallPaperChgFntOamPos( syswk->app );
}

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更：壁紙ＯＢＪ切り替え
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_WallPaperObjChange( BOX2_SYS_WORK * syswk )
{
	NNSG2dCharacterData * chr;
	void * buf;
	u8 * cpy;
	u32	id;
	u32	wp;
	u32	i;

	wp = syswk->app->wallpaper_pos / BOX2OBJ_WPICON_MAX * BOX2OBJ_WPICON_MAX;
	id = BOX2OBJ_ID_WPICON;

	cpy = GFL_HEAP_AllocMemoryLo( HEAPID_BOX_APP, WP_ICON_CGX_SIZ );
//↑[GS_CONVERT_TAG]
	buf = ArchiveDataLoadMallocLo( ARC_BOX2_GRA, NARC_box_gra_box_wp_image_NCGR, HEAPID_BOX_APP );
	NNS_G2dGetUnpackedCharacterData( buf, &chr ); 

	for( i=0; i<BOX2OBJ_WPICON_MAX; i++ ){
		MI_CpuCopy8( chr->pRawData, cpy, WP_ICON_CGX_SIZ );
		WallPaperBufferFill( syswk, cpy, wp, WP_ICON_CHG_COL, WP_ICON_CGX_SIZ );
		CgxChangeTrans( syswk->app, id, cpy, WP_ICON_CGX_SIZ );
		id++;
		wp++;
	}

	GFL_HEAP_FreeMemory( buf );
//↑[GS_CONVERT_TAG]
	GFL_HEAP_FreeMemory( cpy );
//↑[GS_CONVERT_TAG]
}

//--------------------------------------------------------------------------------------------
/**
 * 壁紙カラー塗りつぶし
 *
 * @param	buf			キャラデータ
 * @param	wp			壁紙番号
 * @param	chg_col		変更元カラー
 * @param	siz			キャラサイズ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WallPaperBufferFill( BOX2_SYS_WORK * syswk, u8 * buf, u32 wp, u32 chg_col, u32 siz )
{
	u32	i;
	u8	col;

	if( wp >= BOX_NORMAL_WALLPAPER_MAX ){
		if( BOXDAT_GetDaisukiKabegamiFlag( syswk->box, wp-BOX_NORMAL_WALLPAPER_MAX ) == FALSE ){
			col = WP_IMAGE_COLOR_START + BOX_TOTAL_WALLPAPER_MAX;
		}else{
			col = WP_IMAGE_COLOR_START + wp;
		}
	}else{
		col = WP_IMAGE_COLOR_START + wp;
	}

	for( i=0; i<siz; i++ ){
		if( buf[i] == chg_col ){
			buf[i] = col;
		}
	}
}


//============================================================================================
//	ボックス移動フレーム関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ボックス移動：トレイ名と収納数のＯＡＭフォントを移動
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxMoveFntOamPos( BOX2_APP_WORK * appwk )
{
	FONTOAM_SetMat( appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, -72, -8 );
	FONTOAM_SetMat( appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NUM].oam, 32, -8 );
}

//--------------------------------------------------------------------------------------------
/**
 * ボックス移動：各ＯＢＪを初期化
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BoxMoveObjInit( BOX2_SYS_WORK * syswk )
{
	GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[BOX2OBJ_ID_BOXMV_LA], BOXMV_LA_PX, BOXMV_LA_PY );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[BOX2OBJ_ID_BOXMV_RA], BOXMV_RA_PX, BOXMV_RA_PY );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_CUR], BOXMV_TRAY_CUR_PX, BOXMV_TRAY_CUR_PY );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_NAME], BOXMV_TRAY_NAME_PX, BOXMV_TRAY_NAME_PY );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_ARROW], BOXMV_TRAY_ARROW_PX, BOXMV_TRAY_ARROW_PY );
//↑[GS_CONVERT_TAG]

	BOX2OBJ_TrayIconChange( syswk );

	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_TRAY_NAME, BOX2OBJ_ANM_TRAY_NAME );

	BoxMoveFntOamPos( syswk->app );
	FONTOAM_SetDrawFlag( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, TRUE );
	FONTOAM_SetDrawFlag( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM].oam, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * ボックス移動：各ＯＢＪをスクロール
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	mv		移動量
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BoxMoveFrmScroll( BOX2_SYS_WORK * syswk, s16 mv )
{
	u32	i;
	s16	x, y;

	for( i=BOX2OBJ_ID_BOXMV_LA; i<=BOX2OBJ_ID_TRAY_ARROW; i++ ){
		GFL_CLACT_WK_GetPosCap( syswk->app->cap[i], &x, &y );
//↑[GS_CONVERT_TAG]
		GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[i], x, y+mv );
//↑[GS_CONVERT_TAG]
	}
	BoxMoveFntOamPos( syswk->app );

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		GFL_CLACT_WK_GetPosCap( syswk->app->cap[BOX2OBJ_ID_TRAYICON+i], &x, &y );
//↑[GS_CONVERT_TAG]
		GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[BOX2OBJ_ID_TRAYICON+i], x, y+mv );
//↑[GS_CONVERT_TAG]
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ボックス移動：カーソルセット
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BoxMoveCursorSet( BOX2_SYS_WORK * syswk )
{
	u32	pos;
	s16	x, y;

	if( (syswk->box_mv_pos/BOX2OBJ_TRAYICON_MAX) == (syswk->tray/BOX2OBJ_TRAYICON_MAX) ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, TRUE );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, FALSE );
	}

	pos = syswk->tray % BOX2OBJ_TRAYICON_MAX;

	GFL_CLACT_WK_GetPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_CUR], &x, &y );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap(
//↑[GS_CONVERT_TAG]
		syswk->app->cap[BOX2OBJ_ID_TRAY_CUR],
		BOX2OBJ_TRAYICON_PX+BOX2OBJ_TRAYICON_SX*pos, y );
}

//--------------------------------------------------------------------------------------------
/**
 * ボックス移動：トレイ名関連ＯＢＪセット
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BoxMoveNameSet( BOX2_SYS_WORK * syswk )
{
	s16	x, y, px;
	u16	pos;

	pos = syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;

	GFL_CLACT_WK_GetPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_NAME], &x, &y );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_NAME], BoxNamePosTbl[pos], y );
//↑[GS_CONVERT_TAG]

	GFL_CLACT_WK_GetPosCap( syswk->app->cap[BOX2OBJ_ID_TRAYICON+pos], &px, &y );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_GetPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_ARROW], &x, &y );
//↑[GS_CONVERT_TAG]
	GFL_CLACT_WK_SetWldPosCap( syswk->app->cap[BOX2OBJ_ID_TRAY_ARROW], px, y );
//↑[GS_CONVERT_TAG]

	BoxMoveFntOamPos( syswk->app );
}

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンの座標を取得
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	pos		トレイ表示位置
 * @param	x		Ｘ座標格納場所
 * @param	y		Ｙ座標格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconPosGet( BOX2_APP_WORK * appwk, u32 pos, s16 * x, s16 * y )
{
	GFL_CLACT_WK_GetPosCap( appwk->cap[BOX2OBJ_ID_TRAYICON+pos], x, y );
//↑[GS_CONVERT_TAG]
}

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンのキャラデータ作成（個別）
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	tray	トレイ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconCgxMake( BOX2_SYS_WORK * syswk, u32 tray )
{
	NNSG2dCharacterData * chr;
	void * buf;

	buf = ArchiveDataLoadMallocLo( ARC_BOX2_GRA, NARC_box_gra_box_tray_NCGR, HEAPID_BOX_APP );
	NNS_G2dGetUnpackedCharacterData( buf, &chr );
	MI_CpuCopy8( chr->pRawData, syswk->app->trayicon_cgx[tray], TRAY_ICON_CGX_SIZ );
	GFL_HEAP_FreeMemory( buf );
//↑[GS_CONVERT_TAG]

	WallPaperBufferFill(
		syswk,
		syswk->app->trayicon_cgx[tray],
		BOX2MAIN_GetWallPaperNumber( syswk, tray ),
		TRAY_ICON_CHG_COL,
		TRAY_ICON_CGX_SIZ );

	TrayPokeDotPut( syswk, tray, syswk->app->trayicon_cgx[tray] );
}

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンのキャラデータ作成（全部）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconCgxMakeAll( BOX2_SYS_WORK * syswk )
{
	u32	i;

	for( i=0; i<BOX_MAX_TRAY; i++ ){
		BOX2OBJ_TrayIconCgxMake( syswk, i );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンのキャラデータ作成（全部）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconChange( BOX2_SYS_WORK * syswk )
{
	u32	id;
	u32	pos;
	u32	i;

	pos = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos);
	id  = BOX2OBJ_ID_TRAYICON;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		CgxChangeTrans( syswk->app, id, syswk->app->trayicon_cgx[pos], TRAY_ICON_CGX_SIZ );
		id++;
		pos++;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンのキャラを転送
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	tray	トレイ番号
 *
 * @return	none
 *
 * @li	指定トレイとアイコンが一致しない場合は転送しない
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconCgxTrans( BOX2_SYS_WORK * syswk, u32 tray )
{
	if( (syswk->box_mv_pos/BOX2OBJ_TRAYICON_MAX) != (tray/BOX2OBJ_TRAYICON_MAX) ){
		return;
	}
	CgxChangeTrans(
		syswk->app,
		BOX2OBJ_ID_TRAYICON + tray % BOX2OBJ_TRAYICON_MAX,
		syswk->app->trayicon_cgx[tray],
		TRAY_ICON_CGX_SIZ );
}

//--------------------------------------------------------------------------------------------
/**
 * ポケモンドットを描画
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	tray	トレイ番号
 * @param	buf		描画先
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TrayPokeDotPut( BOX2_SYS_WORK * syswk, u32 tray, u8 * buf )
{
	POKEMON_PASO_PARAM * ppp;
	BOOL fast;
	u32	mons;
	u16	color;
	u8	i, j, y;
	u8	px, py;

	py = TRAY_POKEDOT_PY;

	for( i=0; i<BOX2OBJ_POKEICON_V_MAX; i++ ){
		px = TRAY_POKEDOT_PX;
		for( j=0; j<BOX2OBJ_POKEICON_H_MAX; j++ ){
			ppp  = BOXDAT_GetPokeDataAddress( syswk->box, tray, i*BOX2OBJ_POKEICON_H_MAX+j );
			fast = PokePasoParaFastModeOn( ppp );
			mons = PokePasoParaGet( ppp, ID_PARA_monsno, NULL );
			if( PokePasoParaGet( ppp, ID_PARA_poke_exist, NULL ) != 0 ){
				if( PokePasoParaGet( ppp, ID_PARA_tamago_flag, NULL ) == 0 ){
					u16 frm = PokePasoParaGet( ppp, ID_PARA_form_no, NULL );
					color = PokePersonalParaGetHandle( syswk->app->ppd_ah, mons, frm, ID_PER_color );
				}else{
					if( mons == MONSNO_MANAFI ){
						color = COLOR_BLUE;
					}else{
						color = COLOR_WHITE;
					}
				}
				color = ( PLANUM_TRAYPOKE << 4 ) + TrayPokeDotColorPos[color];
				color = ( color << 8 ) | color;
				for( y=py; y<py+TRAY_POKEDOT_SY; y++ ){
					MI_CpuFill16( &buf[ ((((y>>3)<<2)+(px>>3))<<6) + ((y&7)<<3)+(px&7) ], color, 2 );
				}
			}
			PokePasoParaFastModeOff( ppp, fast );
			px += TRAY_POKEDOT_SX;
		}
		py += TRAY_POKEDOT_SY;
	}
}


//============================================================================================
//	ポケモンを預けるボックス選択関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 預ける：ＯＢＪ初期化
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	id		OBJ ID
 * @param	x		表示Ｘ座標
 * @param	y		表示Ｙ座標
 * @param	flg		表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeOutBoxObjSet( BOX2_APP_WORK * appwk, u32 id, s16 x, s16 y, BOOL flg )
{
	GFL_CLACT_WK_SetWldPosCap( appwk->cap[id], x, y );
//↑[GS_CONVERT_TAG]
	BOX2OBJ_Vanish( appwk, id, flg );
	BOX2OBJ_BgPriChange( appwk, id, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * 預ける：ＯＢＪ初期化（全体）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeOutBoxObjInit( BOX2_SYS_WORK * syswk )
{
	u32	i;
	s16	x, y;

	PokeOutBoxObjSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, PTOUT_LA_PX, PTOUT_LA_PY, TRUE );
	PokeOutBoxObjSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, PTOUT_RA_PX, PTOUT_RA_PY, TRUE );
	PokeOutBoxObjSet( syswk->app, BOX2OBJ_ID_TRAY_CUR, PTOUT_TRAY_CUR_PX, PTOUT_TRAY_CUR_PY, TRUE );
	PokeOutBoxObjSet( syswk->app, BOX2OBJ_ID_TRAY_NAME, PTOUT_TRAY_NAME_PX, PTOUT_TRAY_NAME_PY, TRUE );
	PokeOutBoxObjSet( syswk->app, BOX2OBJ_ID_TRAY_ARROW, PTOUT_TRAY_ARROW_PX, PTOUT_TRAY_ARROW_PY, TRUE );

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		GFL_CLACT_WK_GetPosCap( syswk->app->cap[BOX2OBJ_ID_TRAYICON+i], &x, &y );
//↑[GS_CONVERT_TAG]
		PokeOutBoxObjSet( syswk->app, BOX2OBJ_ID_TRAYICON+i, x, PTOUT_TRAY_ICON_PY, TRUE );
	}

	BOX2OBJ_TrayIconChange( syswk );

	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_TRAY_NAME, BOX2OBJ_ANM_TRAY_NAME );

	BoxMoveFntOamPos( syswk->app );
	FONTOAM_SetDrawFlag( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, TRUE );
	FONTOAM_SetDrawFlag( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM].oam, TRUE );
	FONTOAM_SetBGPriority( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, 0 );
	FONTOAM_SetBGPriority( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM].oam, 0 );
}

//--------------------------------------------------------------------------------------------
/**
 * 預ける：関連ＯＢＪを非表示へ
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeOutBoxObjVanish( BOX2_SYS_WORK * syswk )
{
	u32	i;

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_BOXMV_LA, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_BOXMV_RA, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_NAME, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_ARROW, FALSE );

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAYICON+i, FALSE );
	}

	FONTOAM_SetDrawFlag( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, FALSE );
	FONTOAM_SetDrawFlag( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM].oam, FALSE );
}


//============================================================================================
//	ボックス名と数 ( OAM font )
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ＯＡＭフォント初期化
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_FontOamInit( BOX2_APP_WORK * appwk )
{
	BOX2_FONTOAM * fobj;
	GF_BGL_BMPWIN	win;
	FONTOAM_INIT	finit;
	u32	siz;

	appwk->fntoam = FONTOAM_SysInit( BOX2MAIN_FNTOAM_MAX, HEAPID_BOX_APP );

	// 名前
	fobj = &appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NAME];

	GF_BGL_BmpWinInit( &win );
	GF_BGL_BmpWinObjAdd( appwk->bgl, &win, BOX2OBJ_FNTOAM_BOXNAME_SX, BOX2OBJ_FNTOAM_BOXNAME_SY, 0, 2 );

	siz = FONTOAM_NeedCharSize( &win, NNS_G2D_VRAM_TYPE_2DMAIN,  HEAPID_BOX_APP );
	CharVramAreaAlloc( siz, CHARM_CONT_AREACONT, NNS_G2D_VRAM_TYPE_2DMAIN, &fobj->cma );

	finit.fontoam_sys = appwk->fntoam;
	finit.bmp = &win;
	finit.clact_set = CATS_GetClactSetPtr( appwk->crp );
	finit.pltt = CATS_PlttProxy( appwk->crp, PAL_ID_BOXOBJ );
	finit.parent = appwk->cap[BOX2OBJ_ID_TRAY_NAME]->act;
	finit.char_ofs = fobj->cma.alloc_ofs;
	finit.x = BOXNAME_OBJ_PX;
	finit.y = BOXNAME_OBJ_PY;
	finit.bg_pri = 1;
	finit.soft_pri = 4;
	finit.draw_area = NNS_G2D_VRAM_TYPE_2DMAIN;
	finit.heap = HEAPID_BOX_APP;

	fobj->oam = FONTOAM_Init( &finit );
	FONTOAM_SetPaletteOffset( fobj->oam, 1 );
	FONTOAM_SetPaletteOffsetAddTransPlttNo( fobj->oam, 1 );

	GFL_BMPWIN_Delete( &win );
//↑[GS_CONVERT_TAG]

	// 数字
	fobj = &appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NUM];

	GF_BGL_BmpWinInit( &win );
	GF_BGL_BmpWinObjAdd( appwk->bgl, &win, BOX2OBJ_FNTOAM_BOXNUM_SX, BOX2OBJ_FNTOAM_BOXNUM_SY, 0, 2 );

	siz = FONTOAM_NeedCharSize( &win, NNS_G2D_VRAM_TYPE_2DMAIN,  HEAPID_BOX_APP );
	CharVramAreaAlloc( siz, CHARM_CONT_AREACONT, NNS_G2D_VRAM_TYPE_2DMAIN, &fobj->cma );

	finit.fontoam_sys = appwk->fntoam;
	finit.bmp = &win;
	finit.clact_set = CATS_GetClactSetPtr( appwk->crp );
	finit.pltt = CATS_PlttProxy( appwk->crp, PAL_ID_BOXOBJ );
	finit.parent = appwk->cap[BOX2OBJ_ID_TRAY_NAME]->act;
	finit.char_ofs = fobj->cma.alloc_ofs;
	finit.x = BOXNAME_OBJ_PX;
	finit.y = BOXNAME_OBJ_PY;
	finit.bg_pri = 1;
	finit.soft_pri = 4;
	finit.draw_area = NNS_G2D_VRAM_TYPE_2DMAIN;
	finit.heap = HEAPID_BOX_APP;

	fobj->oam = FONTOAM_Init( &finit );
	FONTOAM_SetPaletteOffset( fobj->oam, 1 );
	FONTOAM_SetPaletteOffsetAddTransPlttNo( fobj->oam, 1 );

	GFL_BMPWIN_Delete( &win );
//↑[GS_CONVERT_TAG]
}

//--------------------------------------------------------------------------------------------
/**
 * ＯＡＭフォント削除
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_FontOamExit( BOX2_APP_WORK * appwk )
{
	u32	i;

	for( i=0; i<BOX2MAIN_FNTOAM_MAX; i++ ){
		CharVramAreaFree( &appwk->fobj[i].cma );
		FONTOAM_Delete( appwk->fobj[i].oam );
	}

	FONTOAM_SysDelete( appwk->fntoam );
}

//--------------------------------------------------------------------------------------------
/**
 * ＯＡＭフォントにＢＭＰを再設定
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	win		ＢＭＰウィンドウ
 * @param	id		ＯＡＭフォントＩＤ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_FontOamResetBmp( BOX2_APP_WORK * appwk, GF_BGL_BMPWIN * win, u32 id )
{
	FONTOAM_OAM_DATA_PTR dat = FONTOAM_OAMDATA_Make( win, HEAPID_BOX_APP );

	FONTOAM_OAMDATA_ResetBmp( appwk->fobj[id].oam, dat, win, HEAPID_BOX_APP );

	FONTOAM_OAMDATA_Free( dat );
}
#endif
