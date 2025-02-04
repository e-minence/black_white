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
#include "system/poke2dgra.h"
#include "poke_tool/monsno_def.h"
#include "pokeicon/pokeicon.h"
#include "item/item.h"
#include "ui/touchbar.h"

#include "app/app_menu_common.h"
#include "app_menu_common.naix"

#include "box2_main.h"
#include "box2_obj.h"
#include "box2_bmp.h"
#include "box_gra.naix"


//============================================================================================
//	定数定義
//============================================================================================

// メインパレット
#define	PALNUM_TRAYICON		( 0 )																		// パレット番号：トレイアイコン
#define	PALSIZ_TRAYICON		( 3 )																		// パレット数：トレイアイコン
#define	PALNUM_POKEICON		( PALNUM_TRAYICON + PALSIZ_TRAYICON )		// パレット番号：ポケモンアイコン
#define	PALSIZ_POKEICON		( POKEICON_PAL_MAX )										// パレット数：ポケモンアイコン
#define	PALNUM_BOXOBJ			( PALNUM_POKEICON + PALSIZ_POKEICON )		// パレット番号：その他のＯＢＪ
#define	PALSIZ_BOXOBJ			( 2 )																		// パレット数：その他のＯＢＪ
#define	PALNUM_TOUCH_BAR	( PALNUM_BOXOBJ + PALSIZ_BOXOBJ )				// パレット番号：タッチバー
#define	PALSIZ_TOUCH_BAR	( 4 )																		// パレット数：タッチバー
#define	PALNUM_ITEMICON		( PALNUM_TOUCH_BAR + PALSIZ_TOUCH_BAR )	// パレット番号：アイテムアイコン
#define	PALSIZ_ITEMICON		( 1 )																		// パレット数：アイテムアイコン
#define	PALNUM_MARK				( PALNUM_ITEMICON + PALSIZ_ITEMICON )		// パレット番号：アイテムアイコン
#define	PALSIZ_MARK				( 1 )																		// パレット数：アイテムアイコン

#define	PALNUM_OUTLINE		( 1 )											// パレット番号：アウトラインカーソル（リソースのオフセット）
#define	PALNUM_TB_STATUS	( 3 )											// パレット番号：タッチバーステータス（リソースのオフセット）

// サブパレット
#define	PALNUM_TYPEICON_S	( 0 )																			// パレット番号：タイプアイコン
#define	PALSIZ_TYPEICON_S	( 3 )																			// パレット数：タイプアイコン
#define	PALNUM_POKEGRA1_S	( PALNUM_TYPEICON_S + PALSIZ_TYPEICON_S )	// パレット番号：ポケグラ１
#define	PALSIZ_POKEGRA1_S	( 1 )																			// パレット数：ポケグラ１
#define	PALNUM_POKEGRA2_S	( PALNUM_POKEGRA1_S + PALSIZ_POKEGRA1_S )	// パレット番号：ポケグラ２
#define	PALSIZ_POKEGRA2_S	( 1 )																			// パレット数：ポケグラ２
#define	PALNUM_MARK_S			( PALNUM_POKEGRA2_S + PALSIZ_POKEGRA2_S )	// パレット番号：マーク
#define	PALSIZ_MARK_S			( 1 )																			// パレット数：マーク
#define	PALNUM_POKERUS_S	( PALNUM_MARK_S + PALSIZ_MARK_S )					// パレット番号：ポケルスアイコン
#define	PALSIZ_POKERUS_S	( 1 )																			// パレット数：ポケルスアイコン

// フォントOAMで使用するOBJ管理数
// トレイ名と収納数で使用するBMPWINのキャラサイズ分が最大なので、それだけあれば足りる。
#define	FNTOAM_CHR_MAX		( BOX2OBJ_FNTOAM_BOXNAME_SX*BOX2OBJ_FNTOAM_BOXNAME_SY*3 + BOX2OBJ_FNTOAM_BOXNUM_SX*BOX2OBJ_FNTOAM_BOXNUM_SY*6 )

// ポケモンアイコンのプライオリティ（80は適当）
#define	POKEICON_OBJ_PRI(a)			( 80 + BOX2OBJ_POKEICON_MAX - a*2 )	// 通常のOBJプライオリティ
#define	POKEICON_OBJ_PRI_PUT(a)	( 80 + BOX2OBJ_POKEICON_MAX - a*2 )	// 配置時のOBJプライオリティ
#define	POKEICON_OBJ_PRI_GET		( 20 )															// 取得時のOBJプライオリティ
#define	POKEICON_TBG_PRI_PUT		( 3 )																// トレイ配置時のBGプライオリティ
#define	POKEICON_PBG_PRI_PUT		( 1 )																// 手持ち配置時のBGプライオリティ
#define	POKEICON_BG_PRI_GET			( 0 )																// 取得時のBGプライオリティ

#define	WP_IMAGE_COLOR_START	( 16 )		// 壁紙のイメージカラー開始番号

#define	BOX2OBJ_TRAYICON_SY		( 34 )		// トレイアイコンのＹ座標配置間隔
#define	BOX2OBJ_TRAYICON_PX		( 300 )		// トレイアイコンのデフォルトＸ座標
#define	BOX2OBJ_TRAYICON_PY		( 0 )			// トレイアイコンのデフォルトＹ座標

#define	BOX2OBJ_TRAYNUM_PX		( 316 )		// トレイの格納数のデフォルトＸ座標
#define	BOX2OBJ_TRAYNUM_PY		( 2 )			// トレイの格納数のデフォルトＹ座標
#define	BOX2OBJ_TRAYNUM_SY		( 34 )		// トレイの格納数のＹ座標配置間隔

#define	BOX2OBJ_BOXNAME_DPX		( 36 )	// ボックス名のデフォルトＸ座標
#define	BOX2OBJ_BOXNAME_DPY		( 20 )	// ボックス名のデフォルトＹ座標
#define	BOX2OBJ_BOXNAME_RPX		( BOX2OBJ_BOXNAME_DPX+BOX2MAIN_TRAY_SCROLL_CNT*BOX2MAIN_TRAY_SCROLL_SPD )	// ボックス名の右配置Ｘ座標
#define	BOX2OBJ_BOXNAME_LPX		( BOX2OBJ_BOXNAME_DPX-BOX2MAIN_TRAY_SCROLL_CNT*BOX2MAIN_TRAY_SCROLL_SPD )	// ボックス名の左配置Ｘ座標

#define	BOXNAME_OBJ_PX	( 212-6 )		// トレイ名のOAMフォントのデフォルト表示Ｘ座標
#define	BOXNAME_OBJ_PY	( 18 )			// トレイ名のOAMフォントのデフォルト表示Ｙ座標

#define	PARTYPOKE_FRM_PX	( 16 )		// 手持ちポケモンのアイコンデフォルト表示Ｘ座標
#define	PARTYPOKE_FRM_PY	( 192 )		// 手持ちポケモンのアイコンデフォルト表示Ｙ座標

// トレイのポケモンアイコンスクロール関連
#define	TRAY_L_SCROLL_ICON_SX	( -8 )			// アイコン表示開始座標（左）
#define	TRAY_L_SCROLL_ICON_EX	( 144+32 )	// アイコン表示終了座標（左）
#define	TRAY_R_SCROLL_ICON_SX	( 144+32 )	// アイコン表示開始座標（右）
#define	TRAY_R_SCROLL_ICON_EX	( -8 )			// アイコン表示終了座標（右）

// ポケモンを逃がす関連
#define	POKEICON_FREE_CNT_MAX			( 40 )															// カウンタ
#define	POKEICON_FREE_SCALE_DEF		( FX32_ONE )												// デフォルト拡縮値
#define	POKEICON_FREE_SCALE_PRM		( FX32_ONE/POKEICON_FREE_CNT_MAX )	// 拡縮増減値
#define	POKEICON_FREE_CY					( 8 )																// 拡縮中心Ｙオフセット

#define	TRAY_ICON_CGX_SIZ		( 32 * 32 )		// トレイアイコンキャラサイズ
#define	TRAY_ICON_CHG_COL		( 8 )					// トレイアイコン壁紙デフォルトカラー

#define	TRAY_POKEDOT_PX		( 10 )		// トレイに書き込みポケモンドット開始Ｘ座標
#define	TRAY_POKEDOT_PY		( 11 )		// トレイに書き込みポケモンドット開始Ｙ座標
#define	TRAY_POKEDOT_SX		( 2 )			// トレイに書き込みポケモンドットＸサイズ
#define	TRAY_POKEDOT_SY		( 2 )			// トレイに書き込みポケモンドットＹサイズ

#define	TYPEICON1_PX		( 88 )		// タイプ１の表示Ｘ座標
#define	TYPEICON2_PX		( 122 )		// タイプ２の表示Ｘ座標
#define	TYPEICON_PY			( 56 )		// タイプアイコンの表示Ｙ座標

#define	MARK_SWITCH_PX	( 23 )		// マーキングのマーク表示Ｘ座標
#define	MARK_SWITCH_PY	( 8 )			// マーキングのマーク表示Ｙ座標
#define	MARK_SWITCH_SX	( 32 )		// マーキングのマークＸサイズ
#define	MARK_SWITCH_SY	( 24 )		// マーキングのマークＹサイズ

#define	RES_LOAD_NONE		( 0xffffffff )		// リソースを読み込んでない場合

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
static void TouchBarGraResLoad( BOX2_APP_WORK * appwk );

static void PokeIconResLoad( BOX2_APP_WORK * appwk );
static void PokeIconObjAdd( BOX2_APP_WORK * appwk );

static void PokeGraDummyResLoad( BOX2_APP_WORK * appwk );

static void TypeIconObjAdd( BOX2_APP_WORK * appwk );
static void SubDispResLoad( BOX2_APP_WORK * appwk );

static void ItemIconDummyResLoad( BOX2_APP_WORK * appwk );

static void BoxObjAdd( BOX2_SYS_WORK * syswk );

static void TrayObjAdd( BOX2_APP_WORK * appwk );

static void TrayPokeDotPut( BOX2_SYS_WORK * syswk, u32 tray, u8 * buf );

static void WallPaperBufferFill( BOX2_SYS_WORK * syswk, u8 * buf, u32 wp, u32 chg_col, u32 siz );


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
	{	// トレイカーソル
		{ 0, -21, BOX2OBJ_ANM_TRAY_CURSOR, 6, 1 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},
	{	// トレイ名背景
		{ 128, -40, BOX2OBJ_ANM_TRAY_NAME, 5, 1 },
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

	{	// タッチバー戻るボタン（ボックスメニューへ）
		{ TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y, APP_COMMON_BARICON_RETURN, 64, 0 },
		BOX2MAIN_CHRRES_TOUCH_BAR, BOX2MAIN_PALRES_TOUCH_BAR, BOX2MAIN_CELRES_TOUCH_BAR,
		0, CLSYS_DRAW_MAIN,
	},
	{	// タッチバー終了ボタン（C-gearへ）
		{ TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_Y, APP_COMMON_BARICON_EXIT, 64, 0 },
		BOX2MAIN_CHRRES_TOUCH_BAR, BOX2MAIN_PALRES_TOUCH_BAR, BOX2MAIN_CELRES_TOUCH_BAR,
		0, CLSYS_DRAW_MAIN,
	},
	{	// タッチバーステータスボタン
		{ TOUCHBAR_ICON_X_03, TOUCHBAR_ICON_Y, 0, 64, 0 },
		BOX2MAIN_CHRRES_BOX_BAR, BOX2MAIN_PALRES_TOUCH_BAR, BOX2MAIN_CELRES_BOX_BAR,
		PALNUM_TB_STATUS, CLSYS_DRAW_MAIN,
	},

	{	// 便利モードボタン
		{ 128, 6, BOX2OBJ_ANM_CONV_RED, 100, 0 },
		BOX2MAIN_CHRRES_BOXOBJ, BOX2MAIN_PALRES_BOXOBJ, BOX2MAIN_CELRES_BOXOBJ,
		0, CLSYS_DRAW_MAIN,
	},

	{	// アイテムアイコン
		{ 0, 0, 0, 0, 0 },
		BOX2MAIN_CHRRES_ITEMICON, BOX2MAIN_PALRES_ITEMICON, BOX2MAIN_CELRES_ITEMICON,
		0, CLSYS_DRAW_MAIN,
	},

	{	// ポケモン
		{ 200, 56, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEGRA, BOX2MAIN_PALRES_POKEGRA, BOX2MAIN_CELRES_POKEGRA,
		0, CLSYS_DRAW_SUB,
	},
	{	// ポケモン
		{ 200, 56, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEGRA2, BOX2MAIN_PALRES_POKEGRA2, BOX2MAIN_CELRES_POKEGRA2,
		0, CLSYS_DRAW_SUB,
	},

	{	// ●（メイン）
		{ 192, 200, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK, BOX2MAIN_PALRES_POKEMARK, BOX2MAIN_CELRES_POKEMARK,
		0, CLSYS_DRAW_MAIN,
	},
	{	// ★（メイン）
		{ 224, 200, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK, BOX2MAIN_PALRES_POKEMARK, BOX2MAIN_CELRES_POKEMARK,
		0, CLSYS_DRAW_MAIN,
	},
	{	// ◆（メイン）
		{ 192, 224, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK, BOX2MAIN_PALRES_POKEMARK, BOX2MAIN_CELRES_POKEMARK,
		0, CLSYS_DRAW_MAIN,
	},
	{	// ■（メイン）
		{ 224, 224, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK, BOX2MAIN_PALRES_POKEMARK, BOX2MAIN_CELRES_POKEMARK,
		0, CLSYS_DRAW_MAIN,
	},
	{	// ▲（メイン）
		{ 192, 248, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK, BOX2MAIN_PALRES_POKEMARK, BOX2MAIN_CELRES_POKEMARK,
		0, CLSYS_DRAW_MAIN,
	},
	{	// ハート（メイン）
		{ 224, 248, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK, BOX2MAIN_PALRES_POKEMARK, BOX2MAIN_CELRES_POKEMARK,
		0, CLSYS_DRAW_MAIN,
	},

	{	// ●（サブ）
		{ 200, 103, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// ★（サブ）
		{ 208, 103, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// ◆（サブ）
		{ 216, 103, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// ■（サブ）
		{ 224, 103, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// ▲（サブ）
		{ 232, 103, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// ハート（サブ）
		{ 240, 103, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},

	{	// レア
		{ 160, 103, APP_COMMON_POKE_MARK_STAR_RED, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// ポケルス
		{ 168, 103, APP_COMMON_POKE_MARK_POKERUSU, 10, 1 },
		BOX2MAIN_CHRRES_POKEMARK_SUB, BOX2MAIN_PALRES_POKEMARK_SUB, BOX2MAIN_CELRES_POKEMARK_SUB,
		0, CLSYS_DRAW_SUB,
	},
	{	// ポケルスアイコン
		{ 116, 104, 0, 10, 1 },
		BOX2MAIN_CHRRES_POKERUSICON, BOX2MAIN_PALRES_POKERUSICON, BOX2MAIN_CELRES_POKERUSICON,
		0, CLSYS_DRAW_SUB,
	},
};

// ポケモンアイコン
static const BOX_CLWK_DATA ClaPokeIconParam = {
	{ BOX2OBJ_TRAYPOKE_PX, BOX2OBJ_TRAYPOKE_PY, 0, 10, POKEICON_TBG_PRI_PUT },
	BOX2MAIN_CHRRES_POKEICON, BOX2MAIN_PALRES_POKEICON, BOX2MAIN_CELRES_POKEICON,
	0, CLSYS_DRAW_MAIN,
};

// トレイアイコン
static const BOX_CLWK_DATA ClaTrayIconParam = {
	{ BOX2OBJ_TRAYICON_PX, BOX2OBJ_TRAYICON_PY, 0, 80, 1 },
	BOX2MAIN_CHRRES_TRAYICON, BOX2MAIN_PALRES_TRAYICON, BOX2MAIN_CELRES_TRAYICON,
	0, CLSYS_DRAW_MAIN,
};

// タイプアイコン（上画面）
static const BOX_CLWK_DATA ClaTypeIconParam = {
	{ 160, 48, 0, 0, 1 },
	BOX2MAIN_CHRRES_TYPEICON, BOX2MAIN_PALRES_TYPEICON, BOX2MAIN_CELRES_TYPEICON,
	0, CLSYS_DRAW_SUB,
};

// トレイアイコンに描画する壁紙のカラー
static const u8 TrayWallPaperColor[] = {
	0x1b, 0x17, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21,
	0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x10, 0x28,
	// 追加壁紙	
	0x18, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f,
};

static const s8	PokeCursorXTbl[] = { 1, -1, 0, 0, 1, 1, -1, -1 };	// アウトラインＸ座標オフセットテーブル
static const s8	PokeCursorYTbl[] = { 0, 0, 1, -1, 1, -1, 1, -1 };	// アウトラインＹ座標オフセットテーブル

// 手持ちポケモンのアイコン座標テーブル
static const s16 PartyPokeInitPos[6][2] =
{
	{ PARTYPOKE_FRM_PX+24, PARTYPOKE_FRM_PY+16 }, { PARTYPOKE_FRM_PX+64, PARTYPOKE_FRM_PY+24 },
	{ PARTYPOKE_FRM_PX+24, PARTYPOKE_FRM_PY+48 }, { PARTYPOKE_FRM_PX+64, PARTYPOKE_FRM_PY+56 },
	{ PARTYPOKE_FRM_PX+24, PARTYPOKE_FRM_PY+80 }, { PARTYPOKE_FRM_PX+64, PARTYPOKE_FRM_PY+88 },
};

// 手持ちポケモン表示座標
static const u8 PartyPokeFramePos[6][2] =
{
	{ 24, 16 }, { 64, 24 },
	{ 24, 48 }, { 64, 56 },
	{ 24, 80 }, { 64, 88 }
};

// トレイ表示位置に対応した名前表示Ｙ座標
static const s16 TrayNamePosYTbl[] = {
	BOXNAME_OBJ_PY + BOX2OBJ_TRAYICON_SY,
	BOXNAME_OBJ_PY + BOX2OBJ_TRAYICON_SY*2,
	BOXNAME_OBJ_PY + BOX2OBJ_TRAYICON_SY*3,
	BOXNAME_OBJ_PY + BOX2OBJ_TRAYICON_SY*4,
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		ＯＢＪ初期化
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

	BOX2OBJ_SetTouchBarButton(
		syswk, syswk->tb_ret_btn, syswk->tb_exit_btn, syswk->tb_status_btn );

	BOX2OBJ_FontOamInit( syswk->app );

	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// MAIN DISP OBJ ON
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// SUB DISP OBJ ON
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＯＢＪ解放
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_Exit( BOX2_APP_WORK * appwk )
{
	BOX2OBJ_FontOamExit( appwk );

	ClactDelAll( appwk );
	ClactResFree( appwk );
	GFL_CLACT_SYS_Delete();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター初期化
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
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

			BOX2MAIN_CHRRES_MAX+FNTOAM_CHR_MAX,	// 登録できるキャラデータ数
			BOX2MAIN_PALRES_MAX,								// 登録できるパレットデータ数
			BOX2MAIN_CELRES_MAX+FNTOAM_CHR_MAX,	// 登録できるセルアニメパターン数
			0,																	// 登録できるマルチセルアニメパターン数（※現状未対応）

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
	TouchBarGraResLoad( appwk );
	PokeIconResLoad( appwk );
	ItemIconDummyResLoad( appwk );

	PokeGraDummyResLoad( appwk );
	SubDispResLoad( appwk );
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
		if( appwk->chrRes[i] != RES_LOAD_NONE ){
			GFL_CLGRP_CGR_Release( appwk->chrRes[i] );
		}
	}
	for( i=0; i<BOX2MAIN_PALRES_MAX; i++ ){
		if( appwk->palRes[i] != RES_LOAD_NONE ){
	    GFL_CLGRP_PLTT_Release( appwk->palRes[i] );
		}
	}
	for( i=0; i<BOX2MAIN_CELRES_MAX; i++ ){
		if( appwk->celRes[i] != RES_LOAD_NONE ){
	    GFL_CLGRP_CELLANIM_Release( appwk->celRes[i] );
		}
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
	syswk->app->clunit = GFL_CLACT_UNIT_Create( BOX2OBJ_ID_MAX+FNTOAM_CHR_MAX, 0, HEAPID_BOX_APP );

	TrayObjAdd( syswk->app );
	PokeIconObjAdd( syswk->app );
	BoxObjAdd( syswk );
	TypeIconObjAdd( syswk->app );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター作成
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		prm			パラメータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static GFL_CLWK * ClactWorkCreate( BOX2_APP_WORK * appwk, const BOX_CLWK_DATA * prm )
{
	GFL_CLWK * clwk;

	clwk = GFL_CLACT_WK_Create(
					appwk->clunit,
					appwk->chrRes[prm->chrRes],
					appwk->palRes[prm->palRes],
					appwk->celRes[prm->celRes],
					&prm->dat, prm->disp, HEAPID_BOX_APP );

	GFL_CLACT_WK_SetSRTAnimeFlag( clwk, FALSE );

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
		if( appwk->clwk[i] == NULL ){ continue; }
		if( GFL_CLACT_WK_GetAutoAnmFlag( appwk->clwk[i] ) == TRUE ){ continue; }
		GFL_CLACT_WK_AddAnmFrame( appwk->clwk[i], FX32_ONE );
	}
	GFL_CLACT_SYS_Main();
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターアニメ設定
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
 * @brief		セルアクターオートアニメ設定
 *
 * @param		appwk	ボックス画面アプリワーク
 * @param		id		OBJ ID
 * @param		anm		アニメ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_AutoAnmSet( BOX2_APP_WORK * appwk, u32 id, u32 anm )
{
	BOX2OBJ_AnmSet( appwk, id, anm );
	GFL_CLACT_WK_SetAutoAnmFlag( appwk->clwk[id], TRUE );
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
	if( appwk->clwk[id] != NULL ){
		GFL_CLACT_WK_SetDrawEnable( appwk->clwk[id], flg );
	}
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
	GFL_CLACT_WK_SetSoftPri( appwk->clwk[id], pri );
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

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター座標設定
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		x				Ｘ座標
 * @param		y				Ｙ座標
 * @param		setsf		表示画面
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_SetPos( BOX2_APP_WORK * appwk, u32 id, s16 x, s16 y, u16 setsf )
{
	GFL_CLACTPOS	pos;

	pos.x = x;
	pos.y = y;
	GFL_CLACT_WK_SetPos( appwk->clwk[id], &pos, setsf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクター座標取得
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		x				Ｘ座標
 * @param		y				Ｙ座標
 * @param		setsf		表示画面
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_GetPos( BOX2_APP_WORK * appwk, u32 id, s16 * x, s16 * y, u16 setsf )
{
	GFL_CLACTPOS	pos;

	GFL_CLACT_WK_GetPos( appwk->clwk[id], &pos, setsf );
	*x = pos.x;
	*y = pos.y;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		セルアクターパレット変更
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		id			OBJ ID
 * @param		pal			パレット番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ChgPltt( BOX2_APP_WORK * appwk, u32 id, u32 pal )
{
	GFL_CLACT_WK_SetPlttOffs( appwk->clwk[id], pal, CLWK_PLTTOFFS_MODE_PLTT_TOP );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボックス用のリソース読み込み
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
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

	// ステータスボタン
	appwk->chrRes[BOX2MAIN_CHRRES_BOX_BAR] = GFL_CLGRP_CGR_Register(
																							ah, NARC_box_gra_bar_button_lz_NCGR,
																							TRUE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_BOX_BAR] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							NARC_box_gra_bar_button_NCER,
																							NARC_box_gra_bar_button_NANR,
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

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチバーのリソース読み込み
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TouchBarGraResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;

	ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_TOUCH_BAR] = GFL_CLGRP_CGR_Register(
																							ah, APP_COMMON_GetBarIconCharArcIdx(),
																							FALSE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );
  appwk->palRes[BOX2MAIN_PALRES_TOUCH_BAR] = GFL_CLGRP_PLTT_Register(
																							ah, APP_COMMON_GetBarIconPltArcIdx(),
																							CLSYS_DRAW_MAIN, PALNUM_TOUCH_BAR*0x20, HEAPID_BOX_APP );
	appwk->celRes[BOX2MAIN_CELRES_TOUCH_BAR] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K),
																							APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K),
																							HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}


//============================================================================================
//	ポケモンアイコン
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコンリソース読み込み
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_POKEICON, HEAPID_BOX_APP );

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
 * @brief		ポケモンアイコンOBJ追加
 *
 * @param		appwk		ボックス画面アプリワーク
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
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコンキャラデータ読み込み
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		ppp			POKEMON_PASO_PARAM
 * @param		chr			NNSG2dCharacterData
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
 * @brief		ポケモンアイコンのキャラデータをワークに展開
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		tray		トレイ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconBufLoad( BOX2_SYS_WORK * syswk, u32 tray )
{
	POKEMON_PASO_PARAM * ppp;
	NNSG2dCharacterData * chr;
	void * buf;
	u32	i;

	for( i=0; i<BOX2OBJ_POKEICON_TRAY_MAX; i++ ){
		// ポケモンが存在しない
		if( BOXDAT_PokeParaGet( syswk->dat->sv_box, tray, i, ID_PARA_poke_exist, NULL ) == 0 ){
			syswk->app->pokeicon_exist[i] = FALSE;
			continue;
		}
		// 取得位置
		if( syswk->dat->callMode != BOX_MODE_ITEM && syswk->tray == syswk->get_tray && syswk->get_pos == i ){
			syswk->app->pokeicon_exist[i] = FALSE;
			continue;
		}
		ppp = BOX2MAIN_PPPGet( syswk, tray, i );
		buf = PokeIconCgxLoad( syswk->app, ppp, &chr );
		GFL_STD_MemCopy32( chr->pRawData, syswk->app->pokeicon_cgx[i], BOX2OBJ_POKEICON_CGX_SIZE );
		GFL_HEAP_FreeMemory( buf );

		syswk->app->pokeicon_pal[i] = POKEICON_GetPalNumGetByPPP( ppp );

		syswk->app->pokeicon_exist[i] = TRUE;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコンのキャラデータを変更
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		buf			キャラデータバッファ
 * @param		id			OBJ ID
 * @param		pal			パレット番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconChangeCore( BOX2_APP_WORK * appwk, void * buf, u32 id, u32 pal )
{
	GFL_CLGRP_CGR_ReplaceEx(
		GFL_CLACT_WK_GetCgrIndex( appwk->clwk[id] ),
		buf, BOX2OBJ_POKEICON_CGX_SIZE, 0, CLSYS_DRAW_MAIN );

	GFL_CLACT_WK_SetPlttOffs( appwk->clwk[id], pal, CLWK_PLTTOFFS_MODE_PLTT_TOP );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコンのキャラデータを変更
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		ppp			POKEMON_PASO_PARAM
 * @param		id			OBJ ID
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

	// BTS3823対処で変更
//	if( BOX2MAIN_PokeParaGet( syswk, pos, syswk->tray, ID_PARA_poke_exist, NULL ) == 0 ){
	if( BOX2MAIN_PokeParaGet( syswk, pos, tray, ID_PARA_poke_exist, NULL ) == 0 ){
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

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコンのデフォルト座標取得
 *
 * @param		pos		位置
 * @param		x			Ｘ座標格納場所
 * @param		y			Ｙ座標格納場所
 * @param		mode	ポケモンアイコン移動モード
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
 * @brief		ポケモンアイコンにデフォルト座標を設定
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		pos			位置
 * @param		id			OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconDefaultPosSet( BOX2_SYS_WORK * syswk, u32 pos, u32 objID )
{
	s16	x, y;

	BOX2OBJ_PokeIconDefaultPosGet( pos, &x, &y, syswk->move_mode );
	BOX2OBJ_SetPos( syswk->app, objID, x, y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_ObjPriChange( syswk->app, objID, POKEICON_OBJ_PRI(pos) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンアイコンをフレームイン前の座標に移動（左）
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		get			1 = 取得中
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconFrmInSet( BOX2_SYS_WORK * syswk, u32 get )
{
	u32	ppcnt;
	u32	id;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_TRAY_MAX+i ];
		BOX2OBJ_SetPos(
			syswk->app, id, PartyPokeInitPos[i][0], PartyPokeInitPos[i][1], CLSYS_DEFREND_MAIN );
		BOX2OBJ_PokeIconPriChg( syswk->app, BOX2OBJ_POKEICON_TRAY_MAX+i, BOX2OBJ_POKEICON_PRI_CHG_PUT );
		if( i < ppcnt && ( get == 0 || syswk->get_pos != (i+BOX2OBJ_POKEICON_TRAY_MAX) ) ){
			BOX2OBJ_Vanish( syswk->app, id, TRUE );
		}else{
			BOX2OBJ_Vanish( syswk->app, id, FALSE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちポケモンアイコンをフレームイン前の座標に移動（右）
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconFrmInSetRight( BOX2_SYS_WORK * syswk )
{
	u32	ppcnt;
	u32	id;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_TRAY_MAX+i ];
		BOX2OBJ_SetPos(
			syswk->app, id,
			PartyPokeInitPos[i][0]+BOX2MAIN_PARTYPOKE_FRM_H_SPD*BOX2MAIN_PARTYPOKE_FRM_H_CNT,
			PartyPokeInitPos[i][1],
			CLSYS_DEFREND_MAIN );
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
 * @brief		手持ちポケモンアイコンをフレームイン後の座標に移動
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconFrmSet( BOX2_SYS_WORK * syswk )
{
	u32	ppcnt;
	u32	id;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_TRAY_MAX+i ];
		BOX2OBJ_SetPos(
			syswk->app, id,
			PartyPokeInitPos[i][0],
			PartyPokeInitPos[i][1] - BOX2MAIN_PARTYPOKE_FRM_SPD * BOX2MAIN_PARTYPOKE_FRM_CNT,
			CLSYS_DEFREND_MAIN );
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
 * @brief		手持ちポケモンアイコンをフレームイン後の座標に移動（右）
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconFrmSetRight( BOX2_SYS_WORK * syswk )
{
	u32	ppcnt;
	u32	id;
	u32	i;

	ppcnt = PokeParty_GetPokeCount( syswk->dat->pokeparty );

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[ BOX2OBJ_POKEICON_TRAY_MAX+i ];
		BOX2OBJ_SetPos(
			syswk->app, id,
			PartyPokeInitPos[i][0] + BOX2MAIN_PARTYPOKE_FRM_H_SPD * BOX2MAIN_PARTYPOKE_FRM_H_CNT,
			PartyPokeInitPos[i][1] - BOX2MAIN_PARTYPOKE_FRM_SPD * BOX2MAIN_PARTYPOKE_FRM_CNT,
			CLSYS_DEFREND_MAIN );
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
 * @brief		ポケモンアイコンのプライオリティを変更
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		pos			位置
 * @param		flg			取得か配置か
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
		if( pos < BOX2OBJ_POKEICON_TRAY_MAX || pos == BOX2OBJ_POKEICON_GET_POS ){
			BOX2OBJ_BgPriChange( appwk, id, POKEICON_TBG_PRI_PUT );
		}else{
			BOX2OBJ_BgPriChange( appwk, id, POKEICON_PBG_PRI_PUT );
		}
		BOX2OBJ_ObjPriChange( appwk, id, POKEICON_OBJ_PRI_PUT(pos) );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコンのプライオリティを変更（位置拡張版）
 *
 * @param		appwk			ボックス画面アプリワーク
 * @param		icon_pos	アイコンバッファの位置
 * @param		put_pos		配置位置
 * @param		flg				取得か配置か
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconPriChg2( BOX2_APP_WORK * appwk, u32 icon_pos, u32 put_pos, u32 flg )
{
	u32	id = appwk->pokeicon_id[icon_pos];

	if( flg == BOX2OBJ_POKEICON_PRI_CHG_GET ){
		BOX2OBJ_BgPriChange( appwk, id, POKEICON_BG_PRI_GET );
		BOX2OBJ_ObjPriChange( appwk, id, POKEICON_OBJ_PRI_GET );
	}else{
		if( put_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
			BOX2OBJ_BgPriChange( appwk, id, POKEICON_TBG_PRI_PUT );
		}else{
			BOX2OBJ_BgPriChange( appwk, id, POKEICON_PBG_PRI_PUT );
		}
		BOX2OBJ_ObjPriChange( appwk, id, POKEICON_OBJ_PRI_PUT(put_pos) );
	}
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコンのプライオリティを変更（掴んだ手持ちをトレイに落とすとき）
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		dp			掴んだ位置
 * @param		mp			配置する位置
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
 * @brief		トレイのポケモンアイコンスクロール処理
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		mv			移動量
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
		BOX2OBJ_GetPos( syswk->app, id, &x, &y, CLSYS_DEFREND_MAIN );
		x += mv;
		if( x == ep ){
			x = sp;
			PokeIconChangeCore(
				syswk->app, syswk->app->pokeicon_cgx[i], id, syswk->app->pokeicon_pal[i] );
			BOX2OBJ_Vanish( syswk->app, id, syswk->app->pokeicon_exist[i] );
			if( syswk->app->jump_tray == 0 &&
					( syswk->dat->callMode == BOX_MODE_ITEM || syswk->dat->callMode == BOX_MODE_SLEEP ) ){
				BOX2OBJ_PokeIconBlendSetItem( syswk, i );
			}
		}
		BOX2OBJ_SetPos( syswk->app, id, x, y, CLSYS_DEFREND_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		手持ちのポケモンアイコンスクロール処理
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PartyPokeIconScroll( BOX2_SYS_WORK * syswk )
{
	u16	id;
	u16	i;
	s16	px, py;
	s8	wfrm_x, wfrm_y;

	BGWINFRM_PosGet( syswk->app->wfrm, BOX2MAIN_WINFRM_PARTY, &wfrm_x, &wfrm_y );
	px = wfrm_x * 8;
	py = wfrm_y * 8;

	for( i=0; i<BOX2OBJ_POKEICON_MINE_MAX; i++ ){
		id = syswk->app->pokeicon_id[BOX2OBJ_POKEICON_TRAY_MAX+i];
		BOX2OBJ_SetPos(
			syswk->app, id,
			px + PartyPokeFramePos[i][0],
			py + PartyPokeFramePos[i][1],
			CLSYS_DEFREND_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンを逃がす：ポケモンアイコン動作初期化
 *
 * @param		wk		逃がすワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconFreeStart( BOX2MAIN_POKEFREE_WORK * wk )
{
	GFL_CLSCALE	scale;
	GFL_CLACTPOS	pos;

	wk->scale     = POKEICON_FREE_SCALE_DEF;
	wk->scale_cnt = 0;

	GFL_CLACT_WK_SetAffineParam( wk->clwk, CLSYS_AFFINETYPE_NORMAL );

	scale.x = wk->scale;
	scale.y = wk->scale;
	GFL_CLACT_WK_SetScale( wk->clwk, &scale );

	pos.x = 0;
	pos.y = POKEICON_FREE_CY;
	GFL_CLACT_WK_SetAffinePos( wk->clwk, &pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンを逃がす：ポケモンアイコン縮小処理
 *
 * @param		wk		逃がすワーク
 *
 * @retval	"TRUE = 処理中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2OBJ_PokeIconFreeMove( BOX2MAIN_POKEFREE_WORK * wk )
{
	GFL_CLSCALE	scale;

	wk->scale_cnt++;
	wk->scale -= POKEICON_FREE_SCALE_PRM;

	if( wk->scale_cnt == POKEICON_FREE_CNT_MAX ){
		return FALSE;
	}

	scale.x = wk->scale;
	scale.y = wk->scale;
	GFL_CLACT_WK_SetScale( wk->clwk, &scale );

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンを逃がす：ポケモンアイコン拡大処理
 *
 * @param		wk		逃がすワーク
 *
 * @retval	"TRUE = 処理中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2OBJ_PokeIconFreeErrorMove( BOX2MAIN_POKEFREE_WORK * wk )
{
	GFL_CLSCALE	scale;

	wk->scale_cnt -= 2;
	wk->scale += POKEICON_FREE_SCALE_PRM;
	wk->scale += POKEICON_FREE_SCALE_PRM;

	if( wk->scale_cnt == 0 ){
		return FALSE;
	}

	scale.x = wk->scale;
	scale.y = wk->scale;
	GFL_CLACT_WK_SetScale( wk->clwk, &scale );

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンを逃がす：ポケモンアイコン終了設定
 *
 * @param		wk		逃がすワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconFreeEnd( BOX2MAIN_POKEFREE_WORK * wk )
{
	GFL_CLACT_WK_SetDrawEnable( wk->clwk, FALSE );
	BOX2OBJ_PokeIconFreeErrorEnd( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンを逃がす：ポケモンアイコンエラー終了設定
 *
 * @param		wk		逃がすワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeIconFreeErrorEnd( BOX2MAIN_POKEFREE_WORK * wk )
{
	GFL_CLSCALE	scale;
	GFL_CLACTPOS	pos;

	pos.x = 0;
	pos.y = 0;
	GFL_CLACT_WK_SetAffinePos( wk->clwk, &pos );

	scale.x = POKEICON_FREE_SCALE_DEF;
	scale.y = POKEICON_FREE_SCALE_DEF;
	GFL_CLACT_WK_SetScale( wk->clwk, &scale );

	GFL_CLACT_WK_SetAffineParam( wk->clwk, CLSYS_AFFINETYPE_NONE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコンに半透明設定
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		flg			ON/OFFフラグ
 * @param		start		開始アイコン位置
 * @param		end			終了アイコン位置
 * @param		item		持ち物に影響されるか
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeIconBlendSetCore( BOX2_SYS_WORK * syswk, BOOL flg, u32 start, u32 end, BOOL item )
{
	u32	i;

	if( item == TRUE ){
		if( syswk->dat->callMode == BOX_MODE_SLEEP ){
			for( i=start; i<end; i++ ){
				if( BOX2MAIN_CheckSleep( syswk, i ) == TRUE &&
						BOX2MAIN_PokeParaGet( syswk, i, syswk->tray, ID_PARA_tamago_flag, NULL ) == 0 ){
					BOX2OBJ_BlendModeSet( syswk->app, syswk->app->pokeicon_id[i], FALSE );
				}else{
					BOX2OBJ_BlendModeSet( syswk->app, syswk->app->pokeicon_id[i], TRUE );
				}
			}
		}else{
			for( i=start; i<end; i++ ){
				if( BOX2MAIN_PokeParaGet( syswk, i, syswk->tray, ID_PARA_item, NULL ) == 0 ){
					BOX2OBJ_BlendModeSet( syswk->app, syswk->app->pokeicon_id[i], TRUE );
				}else{
					BOX2OBJ_BlendModeSet( syswk->app, syswk->app->pokeicon_id[i], FALSE );
				}
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
 * @brief		ポケモンアイコンに半透明設定（全体）
 *
 * @param		syswk	ボックス画面システムワーク
 * @param		type	アイコン位置
 * @param		flg		ON/OFFフラグ
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
 * @brief		ポケモンアイコンに半透明設定
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		pos			アイコン位置
 * @param		flg			ON/OFFフラグ
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
 * @brief		ポケモンアイコンに半透明設定（持ち物整理のトレイスクロール用）
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		pos			アイコン位置
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
 * @brief		ポケモンアイコンを取得したときのＯＢＪいれかえ
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		pos			アイコン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_GetPokeIcon( BOX2_APP_WORK * appwk, u32 pos )
{
	s16	x, y;
	u8	tmp;

	BOX2OBJ_GetPos( appwk, appwk->pokeicon_id[pos], &x, &y, CLSYS_DEFREND_MAIN );

	tmp = appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS];
	appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS] = appwk->pokeicon_id[pos];
	appwk->pokeicon_id[pos] = tmp;

	BOX2OBJ_SetPos( appwk, appwk->pokeicon_id[pos], x, y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_PokeIconPriChg( appwk, pos, BOX2OBJ_POKEICON_PRI_CHG_PUT );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコンを配置したときのＯＢＪいれかえ
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		pos			アイコン位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PutPokeIcon( BOX2_APP_WORK * appwk, u32 pos )
{
	s16	x, y;
	u8	tmp;

	BOX2OBJ_GetPos( appwk, appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS], &x, &y, CLSYS_DEFREND_MAIN );

	tmp = appwk->pokeicon_id[pos];
	appwk->pokeicon_id[pos] = appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS];
	appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS] = tmp;

	BOX2OBJ_SetPos( appwk, appwk->pokeicon_id[BOX2OBJ_POKEICON_GET_POS], x, y, CLSYS_DEFREND_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンアイコン取得状態の移動
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_MovePokeIconHand( BOX2_SYS_WORK * syswk )
{
	u32	id;
	s16	x, y;

	if( syswk->poke_get_key == 0 ){
		return;
	}

	BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, &x, &y, CLSYS_DEFREND_MAIN );

	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		id = BOX2OBJ_ID_ITEMICON;
		y += 8;
	}else{
		id = syswk->app->pokeicon_id[BOX2OBJ_POKEICON_GET_POS];
		y += 4;
	}

	BOX2OBJ_SetPos( syswk->app, id, x, y, CLSYS_DEFREND_MAIN );
	if( syswk->dat->callMode == BOX_MODE_ITEM ){
		BOX2OBJ_ItemIconCursorMove( syswk->app );
	}else{
		if( syswk->app->clwk[BOX2OBJ_ID_OUTLINE] == NULL ){
			BOX2OBJ_PokeCursorAdd( syswk );
		}
		BOX2OBJ_PokeCursorMove( syswk->app, BOX2OBJ_POKEICON_GET_POS );
	}
}


//============================================================================================
//	ポケモングラフィック
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモングラフィックリソース読み込み
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PokeGraDummyResLoad( BOX2_APP_WORK * appwk )
{
	appwk->chrRes[BOX2MAIN_CHRRES_POKEGRA] = RES_LOAD_NONE;
	appwk->palRes[BOX2MAIN_PALRES_POKEGRA] = RES_LOAD_NONE;
	appwk->celRes[BOX2MAIN_CELRES_POKEGRA] = RES_LOAD_NONE;

	appwk->chrRes[BOX2MAIN_CHRRES_POKEGRA2] = RES_LOAD_NONE;
	appwk->palRes[BOX2MAIN_PALRES_POKEGRA2] = RES_LOAD_NONE;
	appwk->celRes[BOX2MAIN_CELRES_POKEGRA2] = RES_LOAD_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモングラフィック切り替え
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		info		表示データ
 * @param		id			OBJ ID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeGraChange( BOX2_SYS_WORK * syswk, BOX2_POKEINFO_DATA * info, u32 id )
{
	ARCHANDLE * ah;
	u32 * chrRes;
	u32 * palRes;
	u32 * celRes;
	u32	oldID;
	u16	pal;
	BOOL	fast;

	if( syswk->app->pokegra_swap == 0 ){
		chrRes = &syswk->app->chrRes[BOX2MAIN_CHRRES_POKEGRA];
		palRes = &syswk->app->palRes[BOX2MAIN_PALRES_POKEGRA];
		celRes = &syswk->app->celRes[BOX2MAIN_CELRES_POKEGRA];
		pal = PALNUM_POKEGRA1_S * 0x20;
		oldID = id + 1;
	}else{
		chrRes = &syswk->app->chrRes[BOX2MAIN_CHRRES_POKEGRA2];
		palRes = &syswk->app->palRes[BOX2MAIN_PALRES_POKEGRA2];
		celRes = &syswk->app->celRes[BOX2MAIN_CELRES_POKEGRA2];
		pal = PALNUM_POKEGRA2_S * 0x20;
		oldID = id;
		id += 1;
	}

	if( syswk->app->clwk[id] != NULL ){
		GFL_CLACT_WK_Remove( syswk->app->clwk[id] );
		GFL_CLGRP_CGR_Release( *chrRes );
    GFL_CLGRP_PLTT_Release( *palRes );
    GFL_CLGRP_CELLANIM_Release( *celRes );
	}

	ah = POKE2DGRA_OpenHandle( HEAPID_BOX_APP );

	fast = PPP_FastModeOn( info->ppp );
	*chrRes = POKE2DGRA_OBJ_CGR_RegisterPPP( ah, info->ppp, POKEGRA_DIR_FRONT, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	*palRes = POKE2DGRA_OBJ_PLTT_RegisterPPP( ah, info->ppp, POKEGRA_DIR_FRONT, CLSYS_DRAW_SUB, pal, HEAPID_BOX_APP );
	*celRes = POKE2DGRA_OBJ_CELLANM_RegisterPPP( info->ppp, POKEGRA_DIR_FRONT, APP_COMMON_MAPPING_128K, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	PPP_FastModeOff( info->ppp, fast );

  GFL_ARC_CloseDataHandle( ah );

	syswk->app->clwk[id] = ClactWorkCreate( syswk->app, &ClactParamTbl[id] );
	BOX2OBJ_Vanish( syswk->app, id, TRUE );

	if( syswk->app->clwk[oldID] != NULL ){
		BOX2OBJ_Vanish( syswk->app, oldID, FALSE );
	}

	syswk->app->pokegra_swap ^= 1;
}


//============================================================================================
//	タイプアイコン
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		タイプアイコンリソース読み込み
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SubDispResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah;
	u32	i;

	ah = GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), HEAPID_BOX_APP_L );

	for( i=0; i<POKETYPE_MAX; i++ ){
		appwk->chrRes[BOX2MAIN_CHRRES_TYPEICON+i] = GFL_CLGRP_CGR_Register(
																									ah,
																									APP_COMMON_GetPokeTypeCharArcIdx(i),
																									FALSE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );
	}

	appwk->celRes[BOX2MAIN_CELRES_TYPEICON] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							APP_COMMON_GetPokeTypeCellArcIdx(APP_COMMON_MAPPING_128K),
																							APP_COMMON_GetPokeTypeAnimeArcIdx(APP_COMMON_MAPPING_128K),
																							HEAPID_BOX_APP );

  appwk->palRes[BOX2MAIN_PALRES_TYPEICON] = GFL_CLGRP_PLTT_Register(
																							ah, APP_COMMON_GetPokeTypePltArcIdx(),
																							CLSYS_DRAW_SUB, PALNUM_TYPEICON_S*0x20, HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_POKEMARK_SUB] = GFL_CLGRP_CGR_Register(
																									ah,
																									APP_COMMON_GetPokeMarkCharArcIdx(APP_COMMON_MAPPING_128K),
																									FALSE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );

	appwk->celRes[BOX2MAIN_CELRES_POKEMARK_SUB] = GFL_CLGRP_CELLANIM_Register(
																									ah,
																									APP_COMMON_GetPokeMarkCellArcIdx(APP_COMMON_MAPPING_128K),
																									APP_COMMON_GetPokeMarkAnimeArcIdx(APP_COMMON_MAPPING_128K),
																									HEAPID_BOX_APP );

  appwk->palRes[BOX2MAIN_PALRES_POKEMARK_SUB] = GFL_CLGRP_PLTT_Register(
																									ah,APP_COMMON_GetPokeMarkPltArcIdx(),
																									CLSYS_DRAW_SUB, PALNUM_MARK_S*0x20, HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_POKERUSICON] = GFL_CLGRP_CGR_Register(
																								ah,
																								APP_COMMON_GetPokerusCharArcIdx(APP_COMMON_MAPPING_128K),
																								FALSE, CLSYS_DRAW_SUB, HEAPID_BOX_APP );

	appwk->celRes[BOX2MAIN_CELRES_POKERUSICON] = GFL_CLGRP_CELLANIM_Register(
																								ah,
																								APP_COMMON_GetPokerusCellArcIdx(APP_COMMON_MAPPING_128K),
																								APP_COMMON_GetPokerusAnimeArcIdx(APP_COMMON_MAPPING_128K),
																								HEAPID_BOX_APP );

  appwk->palRes[BOX2MAIN_PALRES_POKERUSICON] = GFL_CLGRP_PLTT_Register(
																								ah, APP_COMMON_GetPokerusPltArcIdx(),
																								CLSYS_DRAW_SUB, PALNUM_POKERUS_S*0x20, HEAPID_BOX_APP );

	appwk->chrRes[BOX2MAIN_CHRRES_POKEMARK] = GFL_CLGRP_CGR_Register(
																							ah,
																							APP_COMMON_GetPokeMarkCharArcIdx(APP_COMMON_MAPPING_128K),
																							FALSE, CLSYS_DRAW_MAIN, HEAPID_BOX_APP );

	appwk->celRes[BOX2MAIN_CELRES_POKEMARK] = GFL_CLGRP_CELLANIM_Register(
																							ah,
																							APP_COMMON_GetPokeMarkCellArcIdx(APP_COMMON_MAPPING_128K),
																							APP_COMMON_GetPokeMarkAnimeArcIdx(APP_COMMON_MAPPING_128K),
																							HEAPID_BOX_APP );

  appwk->palRes[BOX2MAIN_PALRES_POKEMARK] = GFL_CLGRP_PLTT_Register(
																							ah,APP_COMMON_GetPokeMarkPltArcIdx(),
																							CLSYS_DRAW_MAIN, PALNUM_MARK*0x20, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タイプアイコン追加
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TypeIconObjAdd( BOX2_APP_WORK * appwk )
{
	BOX_CLWK_DATA	dat;
	u32	i;

	dat = ClaTypeIconParam;

	for( i=0; i<POKETYPE_MAX; i++ ){
		dat.chrRes = BOX2MAIN_CHRRES_TYPEICON + i;
		appwk->clwk[BOX2OBJ_ID_TYPEICON+i] = ClactWorkCreate( appwk, &dat );
		GFL_CLACT_WK_SetPlttOffs(
			appwk->clwk[BOX2OBJ_ID_TYPEICON+i],
			APP_COMMON_GetPokeTypePltOffset(i), CLWK_PLTTOFFS_MODE_PLTT_TOP );
		BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_TYPEICON+i, FALSE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タイプアイコン配置
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		type1		タイプ１
 * @param		type2		タイプ２
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TypeIconPut( BOX2_APP_WORK * appwk, u32 type1, u32 type2 )
{
	u32	objID;

	objID = BOX2OBJ_ID_TYPEICON + type1;
	BOX2OBJ_Vanish( appwk, objID, TRUE );
	BOX2OBJ_SetPos( appwk, objID, TYPEICON1_PX, TYPEICON_PY, CLSYS_DEFREND_SUB );

	if( type2 != 0 && type1 != type2 ){
		objID = BOX2OBJ_ID_TYPEICON + type2;
		BOX2OBJ_Vanish( appwk, objID, TRUE );
		BOX2OBJ_SetPos( appwk, objID, TYPEICON2_PX, TYPEICON_PY, CLSYS_DEFREND_SUB );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タイプアイコン切り替え
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		info		表示データ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TypeIconChange( BOX2_APP_WORK * appwk, BOX2_POKEINFO_DATA * info )
{
	u32	i;

	// 全タイプアイコンオフ
	for( i=BOX2OBJ_ID_TYPEICON; i<BOX2OBJ_ID_TYPEICON+POKETYPE_MAX; i++ ){
		BOX2OBJ_Vanish( appwk, i, FALSE );
	}

	// タマゴ
	if( info->tamago != 0 ){ return; }

	TypeIconPut( appwk, info->type1, info->type2 );
}


//============================================================================================
//	アイテムアイコン
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムアイコンリソース読み込み
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ItemIconDummyResLoad( BOX2_APP_WORK * appwk )
{
  ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ITEMICON, HEAPID_BOX_APP );

  appwk->palRes[BOX2MAIN_PALRES_ITEMICON] = GFL_CLGRP_PLTT_Register(
																							ah, ITEM_GetIndex(0,ITEM_GET_ICON_PAL),
																							CLSYS_DRAW_MAIN, PALNUM_ITEMICON*0x20, HEAPID_BOX_APP );

  GFL_ARC_CloseDataHandle( ah );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムグラフィック切り替え
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		item		アイテム番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconChange( BOX2_APP_WORK * appwk, u16 item )
{
	NNSG2dCharacterData * chr;
	NNSG2dPaletteData * pal;
	void * buf;

	buf = GFL_ARC_UTIL_LoadOBJCharacter(
					ITEM_GetIconArcID(), ITEM_GetIndex(item,ITEM_GET_ICON_CGX), 0, &chr, HEAPID_BOX_APP );
	GFL_CLGRP_CGR_Replace( GFL_CLACT_WK_GetCgrIndex(appwk->clwk[BOX2OBJ_ID_ITEMICON]), chr );
	GFL_HEAP_FreeMemory( buf );

	buf = GFL_ARC_UTIL_LoadPalette(
					ITEM_GetIconArcID(), ITEM_GetIndex(item,ITEM_GET_ICON_PAL), &pal, HEAPID_BOX_APP );
	GFL_CLGRP_PLTT_Replace( GFL_CLACT_WK_GetPlttIndex(appwk->clwk[BOX2OBJ_ID_ITEMICON]), pal, 1 );
	GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムアイコン拡縮設定
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		flg			TRUE = 拡縮, FALSE = 通常
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconAffineSet( BOX2_APP_WORK * appwk, BOOL flg )
{
	GFL_CLACTPOS	pos;

	if( flg == TRUE ){
		GFL_CLACT_WK_SetAffineParam( appwk->clwk[BOX2OBJ_ID_ITEMICON], CLSYS_AFFINETYPE_NORMAL );
		pos.x = 12;
		pos.y = 12;
		GFL_CLACT_WK_SetAffinePos( appwk->clwk[BOX2OBJ_ID_ITEMICON], &pos );
	}else{
		pos.x = 0;
		pos.y = 0;
		GFL_CLACT_WK_SetAffinePos( appwk->clwk[BOX2OBJ_ID_ITEMICON], &pos );
		GFL_CLACT_WK_SetAffineParam( appwk->clwk[BOX2OBJ_ID_ITEMICON], CLSYS_AFFINETYPE_NONE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムアイコン座標設定
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		x				Ｘ座標
 * @param		y				Ｙ座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconPosSet( BOX2_APP_WORK * appwk, s16 x, s16 y )
{
	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_ITEMICON, x, y, CLSYS_DEFREND_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムアイコン移動
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		mx			Ｘ座標移動量
 * @param		my			Ｙ座標移動量
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconMove( BOX2_APP_WORK * appwk, s16 mx, s16 my )
{
	s16	px, py;

	BOX2OBJ_GetPos( appwk, BOX2OBJ_ID_ITEMICON, &px, &py, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_ITEMICON, px+mx, py+my, CLSYS_DEFREND_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		指定ポケモンアイコンの位置にアイテムアイコンをセット
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		poke		ポケモンアイコン位置
 * @param		mode		ポケモンアイコン表示モード
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
 * @brief		指定ポケモンアイコンの位置にアイテムアイコンの掴んだ位置をセット
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		poke		ポケモンアイコン位置
 * @param		mode		ポケモンアイコン表示モード
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
 * @brief		アイテムアイコンのアウトラインカーソルを追加
 *
 * @param		appwk		ボックス画面アプリワーク
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

	BOX2OBJ_GetPos( appwk, BOX2OBJ_ID_ITEMICON, &x, &y, CLSYS_DEFREND_MAIN );

	// 新規追加
	if( appwk->clwk[BOX2OBJ_ID_OUTLINE] == NULL ){
		BOX_CLWK_DATA	prm = ClactParamTbl[BOX2OBJ_ID_ITEMICON];
		prm.dat.softpri = GFL_CLACT_WK_GetSoftPri( appwk->clwk[BOX2OBJ_ID_ITEMICON] ) + 1;
		prm.dat.bgpri   = GFL_CLACT_WK_GetBgPri( appwk->clwk[BOX2OBJ_ID_ITEMICON] );
		prm.chrRes      = BOX2MAIN_CHRRES_ITEMICON;
		prm.palRes      = BOX2MAIN_PALRES_BOXOBJ;
		prm.pal         = PALNUM_OUTLINE;

		for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
			prm.dat.pos_x = x + PokeCursorXTbl[i];
			prm.dat.pos_y = y + PokeCursorYTbl[i];

			appwk->clwk[BOX2OBJ_ID_OUTLINE+i] = ClactWorkCreate( appwk, &prm );
			GFL_CLACT_WK_SetPlttOffs( appwk->clwk[BOX2OBJ_ID_OUTLINE+i], prm.pal, CLWK_PLTTOFFS_MODE_PLTT_TOP );
			BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_OUTLINE+i, FALSE );
		}
	}else{
		int	obj_pri, bg_pri;

		obj_pri = GFL_CLACT_WK_GetSoftPri( appwk->clwk[BOX2OBJ_ID_ITEMICON] ) + 1;
		bg_pri  = GFL_CLACT_WK_GetBgPri( appwk->clwk[BOX2OBJ_ID_ITEMICON] );

		for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
			BOX2OBJ_SetPos(
				appwk, BOX2OBJ_ID_OUTLINE+i,
				x+PokeCursorXTbl[i], y+PokeCursorYTbl[i], CLSYS_DEFREND_MAIN );
			BOX2OBJ_ObjPriChange( appwk, BOX2OBJ_ID_OUTLINE+i, obj_pri );
			BOX2OBJ_BgPriChange( appwk, BOX2OBJ_ID_OUTLINE+i, bg_pri );
			BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_OUTLINE+i, FALSE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムアイコンのアウトラインカーソルを表示
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconCursorOn( BOX2_APP_WORK * appwk )
{
	u32	i;

	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_OUTLINE+i, TRUE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アイテムアイコンのアウトラインカーソル移動
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ItemIconCursorMove( BOX2_APP_WORK * appwk )
{
	u32	i;
	s16	x, y;

	if( appwk->clwk[BOX2OBJ_ID_OUTLINE] == NULL ){ return; }

	BOX2OBJ_GetPos( appwk, BOX2OBJ_ID_ITEMICON, &x, &y, CLSYS_DEFREND_MAIN );

	// アウトライン
	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		BOX2OBJ_SetPos(
			appwk, BOX2OBJ_ID_OUTLINE+i,
			x + PokeCursorXTbl[i],
			y + PokeCursorYTbl[i],
			CLSYS_DEFREND_MAIN );
	}
}


//============================================================================================
//	その他
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボックス画面OBJ追加
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxObjAdd( BOX2_SYS_WORK * syswk )
{
	BOX2_APP_WORK * appwk;
	u32	i;

	appwk = syswk->app;

	appwk->clwk[BOX2OBJ_ID_L_ARROW] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_L_ARROW] );
	appwk->clwk[BOX2OBJ_ID_R_ARROW] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_R_ARROW] );
	appwk->clwk[BOX2OBJ_ID_TRAY_CUR] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TRAY_CUR] );
	appwk->clwk[BOX2OBJ_ID_TRAY_NAME] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TRAY_NAME] );
	appwk->clwk[BOX2OBJ_ID_HAND_CURSOR] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_HAND_CURSOR] );
	appwk->clwk[BOX2OBJ_ID_HAND_SHADOW] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_HAND_SHADOW] );
	appwk->clwk[BOX2OBJ_ID_ITEMICON] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_ITEMICON] );
	appwk->clwk[BOX2OBJ_ID_TB_CANCEL] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TB_CANCEL] );
	appwk->clwk[BOX2OBJ_ID_TB_END] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TB_END] );
	appwk->clwk[BOX2OBJ_ID_TB_STATUS] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_TB_STATUS] );
	appwk->clwk[BOX2OBJ_ID_CONV_BUTTON] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_CONV_BUTTON] );

	for( i=0; i<6; i++ ){
		appwk->clwk[BOX2OBJ_ID_MARK1+i]   = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_MARK1+i] );
		appwk->clwk[BOX2OBJ_ID_MARK1_S+i] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_MARK1_S+i] );
	}
	appwk->clwk[BOX2OBJ_ID_RARE] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_RARE] );
	appwk->clwk[BOX2OBJ_ID_POKERUS] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_POKERUS] );
	appwk->clwk[BOX2OBJ_ID_POKERUS_ICON] = ClactWorkCreate( appwk, &ClactParamTbl[BOX2OBJ_ID_POKERUS_ICON] );

	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_HAND_SHADOW, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_ITEMICON, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_RARE, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_POKERUS, FALSE );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_POKERUS_ICON, FALSE );

	BOX2OBJ_PutConvButton( syswk, BOX2OBJ_ANM_CONV_RED, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		便利モードボタン表示
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		anm			アニメ番号
 * @param		flg			TRUE = 表示, FALSE = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PutConvButton( BOX2_SYS_WORK * syswk, u32 anm, BOOL flg )
{
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_CONV_BUTTON, anm );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_CONV_BUTTON, flg );

	if( syswk->dat->callMode != BOX_MODE_SEIRI &&
			syswk->dat->callMode != BOX_MODE_BATTLE &&
			syswk->dat->callMode != BOX_MODE_ITEM ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_CONV_BUTTON, FALSE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		手カーソルアニメセット
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		anm			アニメ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_SetHandCursorAnm( BOX2_SYS_WORK * syswk, u32 anm )
{
	if( syswk->mv_cnv_mode != 0 ){
		anm = anm - BOX2OBJ_ANM_HAND_NORMAL + BOX2OBJ_ANM_HAND2_NORMAL;
	}
	BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, anm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		手カーソル表示切り替え
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		flg			表示フラグ TRUE = 表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_SetHandCursorOnOff( BOX2_SYS_WORK * syswk, BOOL flg )
{
	if( flg == TRUE ){
		if( CURSORMOVE_CursorOnOffGet( syswk->app->cmwk ) == TRUE ){
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );
		}
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アウトラインカーソル追加
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeCursorAdd( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_PokeCursorAdd2( syswk, syswk->get_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アウトラインカーソル追加（位置指定）
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		pos			表示位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeCursorAdd2( BOX2_SYS_WORK * syswk, u32 pos )
{
	BOX2_APP_WORK * appwk;
	s16	x, y;
	u16	posID;
	u16	i;

	if( pos == BOX2MAIN_GETPOS_NONE ){
		return;
	}

	appwk = syswk->app;
	posID = appwk->pokeicon_id[ pos ];
	BOX2OBJ_GetPos( appwk, posID, &x, &y, CLSYS_DEFREND_MAIN );

	// 新規追加
	if( appwk->clwk[BOX2OBJ_ID_OUTLINE] == NULL ){
		BOX_CLWK_DATA	prm = ClaPokeIconParam;
		prm.dat.softpri = GFL_CLACT_WK_GetSoftPri( appwk->clwk[posID] ) + 1;
		prm.dat.bgpri   = GFL_CLACT_WK_GetBgPri( appwk->clwk[posID] );
		prm.chrRes      = BOX2MAIN_CHRRES_POKEICON + posID - BOX2OBJ_ID_POKEICON;
		prm.palRes      = BOX2MAIN_PALRES_BOXOBJ;
		prm.pal         = PALNUM_OUTLINE;

		for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
			prm.dat.pos_x = x + PokeCursorXTbl[i];
			prm.dat.pos_y = y + PokeCursorYTbl[i];

			appwk->clwk[BOX2OBJ_ID_OUTLINE+i] = ClactWorkCreate( appwk, &prm );
			GFL_CLACT_WK_SetPlttOffs( appwk->clwk[BOX2OBJ_ID_OUTLINE+i], prm.pal, CLWK_PLTTOFFS_MODE_PLTT_TOP );
		}
	// キャラを書き換え
	}else{
		NNSG2dImageProxy	ipc;
		int	obj_pri, bg_pri;

		GFL_CLACT_WK_GetImgProxy( appwk->clwk[posID], &ipc );

		obj_pri = GFL_CLACT_WK_GetSoftPri( appwk->clwk[posID] ) + 1;
		bg_pri  = GFL_CLACT_WK_GetBgPri( appwk->clwk[posID] );

		for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
			GFL_CLACT_WK_SetImgProxy( appwk->clwk[BOX2OBJ_ID_OUTLINE+i], &ipc );
			BOX2OBJ_SetPos( appwk, BOX2OBJ_ID_OUTLINE+i, x+PokeCursorXTbl[i], y+PokeCursorYTbl[i], CLSYS_DEFREND_MAIN );
			BOX2OBJ_ObjPriChange( appwk, BOX2OBJ_ID_OUTLINE+i, obj_pri );
			BOX2OBJ_BgPriChange( appwk, BOX2OBJ_ID_OUTLINE+i, bg_pri );
			BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_OUTLINE+i, TRUE );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アウトラインカーソル表示切り替え
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		flg			表示フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeCursorVanish( BOX2_SYS_WORK * syswk, BOOL flg )
{
	u32	i;

	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		if( syswk->app->clwk[BOX2OBJ_ID_OUTLINE+i] != NULL ){
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_OUTLINE+i, flg );
			if( flg == TRUE && syswk->get_pos != BOX2MAIN_GETPOS_NONE ){
				int	pri;
				u32	posID;
				posID = syswk->app->pokeicon_id[ syswk->get_pos ];

				pri = GFL_CLACT_WK_GetSoftPri( syswk->app->clwk[posID] );
				BOX2OBJ_ObjPriChange( syswk->app, BOX2OBJ_ID_OUTLINE+i, pri+1 );

				pri = GFL_CLACT_WK_GetBgPri( syswk->app->clwk[posID] );
				BOX2OBJ_BgPriChange( syswk->app, BOX2OBJ_ID_OUTLINE+i, pri );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アウトラインカーソル表示プライオリティ切り替え
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		pos			ポケモンアイコンの位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ChgPokeCursorPriority( BOX2_SYS_WORK * syswk, u32 pos )
{
	int	obj_pri, bg_pri;
	u32	posID;
	u32	i;

	posID = syswk->app->pokeicon_id[ pos ];
	obj_pri = GFL_CLACT_WK_GetSoftPri( syswk->app->clwk[posID] );
	bg_pri  = GFL_CLACT_WK_GetBgPri( syswk->app->clwk[posID] );

	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){
		if( syswk->app->clwk[BOX2OBJ_ID_OUTLINE+i] != NULL ){
			if( syswk->get_pos != BOX2MAIN_GETPOS_NONE ){
				BOX2OBJ_ObjPriChange( syswk->app, BOX2OBJ_ID_OUTLINE+i, obj_pri+1 );
				BOX2OBJ_BgPriChange( syswk->app, BOX2OBJ_ID_OUTLINE+i, bg_pri );
			}
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		アウトラインカーソル移動
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		pos			ポケモンアイコンの位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_PokeCursorMove( BOX2_APP_WORK * appwk, u32 pos )
{
	u32	i;
	s16	x, y;

	if( pos == BOX2MAIN_GETPOS_NONE ){
		return;
	}

	BOX2OBJ_GetPos( appwk, appwk->pokeicon_id[pos], &x, &y, CLSYS_DEFREND_MAIN );

	for( i=0; i<BOX2OBJ_PI_OUTLINE_MAX; i++ ){


		BOX2OBJ_SetPos(
			appwk, BOX2OBJ_ID_OUTLINE+i,
			x+PokeCursorXTbl[i], y+PokeCursorYTbl[i], CLSYS_DEFREND_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイアイコンOBJ追加
 *
 * @param		appwk		ボックス画面アプリワーク
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
		prm.dat.pos_y += BOX2OBJ_TRAYICON_SY * i;
		prm.chrRes = BOX2MAIN_CHRRES_TRAYICON + i;
		appwk->clwk[BOX2OBJ_ID_TRAYICON+i] = ClactWorkCreate( appwk, &prm );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイ切り替え矢印表示切替
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		flg			ON/OFFフラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayMoveArrowVanish( BOX2_APP_WORK * appwk, BOOL flg )
{
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_L_ARROW, flg );
	BOX2OBJ_Vanish( appwk, BOX2OBJ_ID_R_ARROW, flg );
}


//============================================================================================
//	マーキング
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		マーキングＯＢＪ移動
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_MarkingScroll( BOX2_SYS_WORK * syswk )
{
	u32	i;
	s16	px, py;
	s8	wx, wy;

	BGWINFRM_PosGet( syswk->app->wfrm, BOX2MAIN_WINFRM_MARK, &wx, &wy );
	px = wx * 8 + MARK_SWITCH_PX;
	py = wy * 8 + MARK_SWITCH_PY;

	for( i=0; i<6; i++ ){
		BOX2OBJ_SetPos(
			syswk->app, BOX2OBJ_ID_MARK1+i,
			px + MARK_SWITCH_SX * ( i & 1 ),
			py + MARK_SWITCH_SY * ( i / 2 ),
			CLSYS_DEFREND_MAIN );
	}
}


//============================================================================================
//	ボックス移動フレーム関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボックス移動：各ＯＢＪを初期化
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BoxMoveObjInit( BOX2_SYS_WORK * syswk )
{
	BOX2_APP_WORK * appwk = syswk->app;

	BOX2OBJ_TrayIconCgxMake( syswk, syswk->tray );
	BOX2OBJ_InitTrayIconScroll( syswk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボックス移動：各ＯＢＪをスクロール
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		mv			移動量
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BoxMoveFrmScroll( BOX2_SYS_WORK * syswk, s16 mv )
{
	u32	i;
	s16	x, y;

	BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, &x, &y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, x-mv, y, CLSYS_DEFREND_MAIN );

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, x-mv, y, CLSYS_DEFREND_MAIN );

		BmpOam_ActorGetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, &x, &y );
		BmpOam_ActorSetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, x-mv, y );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソルを初期位置へ
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_InitBoxMoveCursorPos( BOX2_APP_WORK * appwk )
{
	BOX2OBJ_SetPos(
		appwk, BOX2OBJ_ID_TRAY_CUR, BOX2OBJ_TRAYICON_PX, BOX2OBJ_TRAYICON_SY, CLSYS_DEFREND_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイアイコンを初期座標へ移動
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_InitTrayIconScroll( BOX2_SYS_WORK * syswk )
{
	BOX_CLWK_DATA	prm;
	s16	i;
	u16	tray;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		tray = BOX2MAIN_GetBoxMoveTrayNum( syswk, i-1 );

		prm = ClaTrayIconParam;
		prm.dat.pos_y += BOX2OBJ_TRAYICON_SY * i;
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, prm.dat.pos_x, prm.dat.pos_y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_TrayIconCgxTransIdx( syswk, tray, BOX2OBJ_ID_TRAYICON+i );

		BmpOam_ActorSetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, BOX2OBJ_TRAYNUM_PX, BOX2OBJ_TRAYNUM_PY+BOX2OBJ_TRAYNUM_SY*i );
		BOX2BMP_WriteTrayNum( syswk, tray, BOX2MAIN_FNTOAM_TRAY_NUM1+i );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイアイコンの座標を取得
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		pos			トレイ表示位置
 * @param		x				Ｘ座標格納場所
 * @param		y				Ｙ座標格納場所
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconPosGet( BOX2_APP_WORK * appwk, u32 pos, s16 * x, s16 * y )
{
	u32	i;

	pos = ( pos + 1 ) * BOX2OBJ_TRAYICON_SY;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		BOX2OBJ_GetPos( appwk, BOX2OBJ_ID_TRAYICON+i, x, y, CLSYS_DEFREND_MAIN );
		if( pos == *y ){
			return;
		}
	}

	*x = 0;
	*y = 0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイアイコンのキャラデータ作成（個別）
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		tray		トレイ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconCgxMake( BOX2_SYS_WORK * syswk, u32 tray )
{
	NNSG2dCharacterData * chr;
	void * buf;

	buf = GFL_ARC_UTIL_LoadOBJCharacter(
					ARCID_BOX2_GRA, NARC_box_gra_box_tray_lz_NCGR, TRUE, &chr, HEAPID_BOX_APP );
	GFL_STD_MemCopy( chr->pRawData, syswk->app->trayicon_cgx[tray], TRAY_ICON_CGX_SIZ );
	GFL_HEAP_FreeMemory( buf );

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
 * @brief		トレイアイコンのキャラデータ作成（全部）
 *
 * @param		syswk		ボックス画面システムワーク
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
 * @brief		トレイアイコンのキャラデータ切り替え
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconChange( BOX2_SYS_WORK * syswk )
{
	u32	id;
	s32	pos;
	u32	i;

	pos = syswk->box_mv_pos - 1;
	if( pos < 0 ){
		pos = syswk->trayMax - 1;
	}
	id  = BOX2OBJ_ID_TRAYICON;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		GFL_CLGRP_CGR_ReplaceEx(
			GFL_CLACT_WK_GetCgrIndex( syswk->app->clwk[id] ),
			syswk->app->trayicon_cgx[pos], TRAY_ICON_CGX_SIZ, 0, CLSYS_DRAW_MAIN );
		id++;
		pos++;
		if( pos >= syswk->trayMax ){
			pos = 0;
		}
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
	s16	pos;
	s16	i;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		pos = BOX2MAIN_GetBoxMoveTrayNum( syswk, i-1 );
		if( pos == tray ){
			BOX2OBJ_TrayIconCgxTransPos( syswk, tray, i );
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンのキャラを転送（個別・ＯＢＪ指定）
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		tray		トレイ番号
 * @param		idx			OBJ index
 *
 * @return	none
 *
 * @li	指定トレイとアイコンが一致しない場合は転送しない
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconCgxTransIdx( BOX2_SYS_WORK * syswk, u32 tray, u32 idx )
{
	GFL_CLGRP_CGR_ReplaceEx(
		GFL_CLACT_WK_GetCgrIndex( syswk->app->clwk[idx] ),
		syswk->app->trayicon_cgx[tray], TRAY_ICON_CGX_SIZ, 0, CLSYS_DRAW_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * トレイアイコンのキャラを転送（個別・表示位置指定）
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		tray		トレイ番号
 * @param		pos			表示位置
 *
 * @return	none
 *
 * @li	指定トレイとアイコンが一致しない場合は転送しない
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconCgxTransPos( BOX2_SYS_WORK * syswk, u32 tray, u32 pos )
{
	u32	i;
	s16	x, y;

	pos *= BOX2OBJ_TRAYICON_SY;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, &x, &y, CLSYS_DEFREND_MAIN );
		if( pos == y ){
			BOX2OBJ_TrayIconCgxTransIdx( syswk, tray, BOX2OBJ_ID_TRAYICON+i );
			BOX2BMP_WriteTrayNum( syswk, tray, BOX2MAIN_FNTOAM_TRAY_NUM1+i );
			break;
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイアイコンを壁紙カラー塗りつぶし
 *
 * @param		buf				キャラデータ
 * @param		wp				壁紙番号
 * @param		chg_col		変更元カラー
 * @param		siz				キャラサイズ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WallPaperBufferFill( BOX2_SYS_WORK * syswk, u8 * buf, u32 wp, u32 chg_col, u32 siz )
{
	u32	i;

	for( i=0; i<siz; i++ ){
		if( buf[i] == chg_col ){
			buf[i] = TrayWallPaperColor[wp];
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイアイコンにポケモンドットを描画
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		tray		トレイ番号
 * @param		buf			描画先
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void TrayPokeDotPut( BOX2_SYS_WORK * syswk, u32 tray, u8 * buf )
{
	POKEMON_PASO_PARAM * ppp;
	POKEMON_PERSONAL_DATA * ppd;
	BOOL fast;
	u32	mons;
	u16	color;
	u8	i, j, y;
	u8	px, py;

	py = TRAY_POKEDOT_PY;

	for( i=0; i<BOX2OBJ_POKEICON_V_MAX; i++ ){
		px = TRAY_POKEDOT_PX;
		for( j=0; j<BOX2OBJ_POKEICON_H_MAX; j++ ){
			ppp  = BOXDAT_GetPokeDataAddress( syswk->dat->sv_box, tray, i*BOX2OBJ_POKEICON_H_MAX+j );
			fast = PPP_FastModeOn( ppp );
			mons = PPP_Get( ppp, ID_PARA_monsno, NULL );
			if( PPP_Get( ppp, ID_PARA_poke_exist, NULL ) != 0 ){
				if( PPP_Get( ppp, ID_PARA_tamago_flag, NULL ) == 0 ){
					u16 frm = PPP_Get( ppp, ID_PARA_form_no, NULL );
					ppd = POKE_PERSONAL_OpenHandle( mons, frm, HEAPID_BOX_APP );
					color = POKE_PERSONAL_GetParam( ppd, POKEPER_ID_color );
					POKE_PERSONAL_CloseHandle( ppd );
				}else{
					if( mons == MONSNO_MANAFI ){
						color = POKEPER_COLOR_BLUE;
					}else{
						color = POKEPER_COLOR_WHITE;
					}
				}
				color = color + WP_IMAGE_COLOR_START;
				color = ( color << 8 ) | color;
				for( y=py; y<py+TRAY_POKEDOT_SY; y++ ){
					GFL_STD_MemFill16( &buf[ ((((y>>3)<<2)+(px>>3))<<6) + ((y&7)<<3)+(px&7) ], color, 2 );
				}
			}
			PPP_FastModeOff( ppp, fast );
			px += TRAY_POKEDOT_SX;
		}
		py += TRAY_POKEDOT_SY;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイアイコン移動
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		mv			移動量
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_TrayIconScroll( BOX2_SYS_WORK * syswk, s16 mv )
{
	u32	i;
	s16	x, y, mvy;
	s16	nx, ny, nmvy;
	s16	prm;
	u16	tray;

	if( GFL_STD_Abs(mv) == BOX2MAIN_TRAYICON_SCROLL_CNT ){
		prm = 2;
	}else{
		prm = 8;
	}
	if( mv < 0 ){
		prm *= -1;
	}

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		// トレイアイコン
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, &x, &y, CLSYS_DEFREND_MAIN );
		mvy = y + prm;
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, x, mvy, CLSYS_DEFREND_MAIN );
		// 格納数
		BmpOam_ActorGetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, &nx, &ny );
		nmvy = ny + prm;
		BmpOam_ActorSetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, nx, nmvy );

		if( mvy < -16 ){
			tray = BOX2MAIN_GetBoxMoveTrayNum( syswk, 4 );
			BOX2OBJ_TrayIconCgxTransIdx( syswk, tray, BOX2OBJ_ID_TRAYICON+i );
			BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, x, mvy+BOX2OBJ_TRAYICON_SY*BOX2OBJ_TRAYICON_MAX, CLSYS_DEFREND_MAIN );
			BOX2BMP_WriteTrayNum( syswk, tray, BOX2MAIN_FNTOAM_TRAY_NUM1+i );
			BmpOam_ActorSetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, nx, nmvy+BOX2OBJ_TRAYICON_SY*BOX2OBJ_TRAYICON_MAX );
		}else if( mvy >= (BOX2OBJ_TRAYICON_PY+BOX2OBJ_TRAYICON_SY*(BOX2OBJ_TRAYICON_MAX-1)+16) ){
			tray = BOX2MAIN_GetBoxMoveTrayNum( syswk, -1 );
			BOX2OBJ_TrayIconCgxTransIdx( syswk, tray, BOX2OBJ_ID_TRAYICON+i );
			BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAYICON+i, x, mvy-BOX2OBJ_TRAYICON_SY*BOX2OBJ_TRAYICON_MAX, CLSYS_DEFREND_MAIN );
			BOX2BMP_WriteTrayNum( syswk, tray, BOX2MAIN_FNTOAM_TRAY_NUM1+i );
			BmpOam_ActorSetPos( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i].oam, nx, nmvy-BOX2OBJ_TRAYICON_SY*BOX2OBJ_TRAYICON_MAX );
		}
	}

	if( BOX2OBJ_VanishCheck( syswk->app, BOX2OBJ_ID_TRAY_CUR ) == TRUE ){
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, x, y+prm, CLSYS_DEFREND_MAIN );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイカーソルをスクロール初期位置へ
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		mv			移動方向
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_InitTrayCursorScroll( BOX2_SYS_WORK * syswk, s32 mv )
{
	s16	x, y;

	if( mv < 0 && BOX2MAIN_GetBoxMoveTrayNum( syswk, -1 ) == syswk->tray ){
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, x, -BOX2OBJ_TRAYICON_SY, CLSYS_DEFREND_MAIN );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, TRUE );
		return;
	}
	if( mv > 0 && BOX2MAIN_GetBoxMoveTrayNum( syswk, 4 ) == syswk->tray ){
		BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, &x, &y, CLSYS_DEFREND_MAIN );
		BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, x, BOX2OBJ_TRAYICON_SY*BOX2OBJ_TRAYICON_MAX, CLSYS_DEFREND_MAIN );
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, TRUE );
		return;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイカーソルスクロール終了後の処理
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_EndTrayCursorScroll( BOX2_SYS_WORK * syswk )
{
	u32	i;
	s16	x, y;

	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		if( BOX2MAIN_GetBoxMoveTrayNum( syswk, i-1 ) == syswk->tray ){
			BOX2OBJ_GetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, &x, &y, CLSYS_DEFREND_MAIN );
			BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_TRAY_CUR, x, i*BOX2OBJ_TRAYICON_SY, CLSYS_DEFREND_MAIN );
			BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, TRUE );
			return;
		}
	}
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_CUR, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイ名配置
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		pos			位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_SetTrayNamePos( BOX2_SYS_WORK * syswk, u32 pos )
{
	BOX2OBJ_SetPos(
		syswk->app, BOX2OBJ_ID_TRAY_NAME, BOXNAME_OBJ_PX, TrayNamePosYTbl[pos], CLSYS_DEFREND_MAIN );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_NAME, TRUE );

	BmpOam_ActorSetPos(
		syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam,
		BOXNAME_OBJ_PX - BOX2OBJ_FNTOAM_BOXNAME_SX*8/2,
		TrayNamePosYTbl[pos] - 8 );
	BmpOam_ActorSetDrawEnable( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		トレイ表示切り替え
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		pos			位置
 * @param		flg			TRUE = 表示, FALSE = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_ChangeTrayName( BOX2_SYS_WORK * syswk, u32 pos, BOOL flg )
{
	if( flg == TRUE ){
		BOX2BMP_BoxMoveNameWrite( syswk, pos );
		BOX2OBJ_SetTrayNamePos( syswk, pos );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TRAY_NAME, FALSE );
		BmpOam_ActorSetDrawEnable( syswk->app->fobj[BOX2MAIN_FNTOAM_TRAY_NAME].oam, FALSE );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチバー上のボタン表示切り替え
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		ret			戻るボタン
 * @param		exit		終了ボタン
 * @param		status	ステータスボタン
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_SetTouchBarButton( BOX2_SYS_WORK * syswk, u8 ret, u8 exit, u8 status )
{
	syswk->tb_ret_btn    = ret;
	syswk->tb_exit_btn   = exit;
	syswk->tb_status_btn = status;

	// 戻る
	if( ret == BOX2OBJ_TB_ICON_ON ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_CANCEL, TRUE );
		BOX2OBJ_AutoAnmSet( syswk->app, BOX2OBJ_ID_TB_CANCEL, APP_COMMON_BARICON_RETURN );
	}else if( ret == BOX2OBJ_TB_ICON_OFF ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_CANCEL, FALSE );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_CANCEL, TRUE );
		BOX2OBJ_AutoAnmSet( syswk->app, BOX2OBJ_ID_TB_CANCEL, APP_COMMON_BARICON_RETURN_OFF );
	}
	// 閉じる
	if( exit == BOX2OBJ_TB_ICON_ON ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_END, TRUE );
		BOX2OBJ_AutoAnmSet( syswk->app, BOX2OBJ_ID_TB_END, APP_COMMON_BARICON_EXIT );
	}else if( exit == BOX2OBJ_TB_ICON_OFF ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_END, FALSE );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_END, TRUE );
		BOX2OBJ_AutoAnmSet( syswk->app, BOX2OBJ_ID_TB_END, APP_COMMON_BARICON_EXIT_OFF );
	}
	// ステータス
	if( status == BOX2OBJ_TB_ICON_ON ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_STATUS, TRUE );
		BOX2OBJ_AutoAnmSet( syswk->app, BOX2OBJ_ID_TB_STATUS, 0 );
	}else if( status == BOX2OBJ_TB_ICON_OFF ){
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_STATUS, FALSE );
	}else{
		BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_STATUS, TRUE );
		BOX2OBJ_AutoAnmSet( syswk->app, BOX2OBJ_ID_TB_STATUS, 2 );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチバー上のボタンを非表示
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_VanishTouchBarButton( BOX2_SYS_WORK * syswk )
{
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_CANCEL, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_END, FALSE );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_TB_STATUS, FALSE );
}


//============================================================================================
//	ボックス名と数 ( OAM font )
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＯＡＭフォント初期化
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_FontOamInit( BOX2_APP_WORK * appwk )
{
	BMPOAM_ACT_DATA	finit;
	BOX2_FONTOAM * fobj;
	u32	i;

	appwk->fntoam = BmpOam_Init( HEAPID_BOX_APP, appwk->clunit );

	// ボックス名（ボックス移動用）
	fobj = &appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NAME];

	fobj->bmp = GFL_BMP_Create( BOX2OBJ_FNTOAM_BOXNAME_SX, BOX2OBJ_FNTOAM_BOXNAME_SY, GFL_BMP_16_COLOR, HEAPID_BOX_APP );

	finit.bmp = fobj->bmp;
	finit.x = 0;
	finit.y = 0;
	finit.pltt_index = appwk->palRes[BOX2MAIN_PALRES_BOXOBJ];
	finit.pal_offset = 0;		// pltt_indexのパレット内でのオフセット
	finit.soft_pri = 4;			// ソフトプライオリティ
	finit.bg_pri = 1;				// BGプライオリティ
	finit.setSerface = CLWK_SETSF_NONE;
	finit.draw_type  = CLSYS_DRAW_MAIN;

	fobj->oam = BmpOam_ActorAdd( appwk->fntoam, &finit );
	BmpOam_ActorSetDrawEnable( fobj->oam, FALSE );

	// ボックス名（常時表示）
	for( i=BOX2MAIN_FNTOAM_BOX_NAME1; i<=BOX2MAIN_FNTOAM_BOX_NAME2; i++ ){
		fobj = &appwk->fobj[i];
		fobj->bmp = GFL_BMP_Create( BOX2OBJ_FNTOAM_BOXNAME_SX, BOX2OBJ_FNTOAM_BOXNAME_SY, GFL_BMP_16_COLOR, HEAPID_BOX_APP );
		finit.bmp = fobj->bmp;
		finit.x = BOX2OBJ_BOXNAME_DPX;
		finit.y = BOX2OBJ_BOXNAME_DPY;
		finit.bg_pri = 3;				// BGプライオリティ
		fobj->oam = BmpOam_ActorAdd( appwk->fntoam, &finit );
	}

	// 格納数
	for( i=0; i<BOX2OBJ_TRAYICON_MAX; i++ ){
		fobj = &appwk->fobj[BOX2MAIN_FNTOAM_TRAY_NUM1+i];
		fobj->bmp = GFL_BMP_Create( BOX2OBJ_FNTOAM_BOXNUM_SX, BOX2OBJ_FNTOAM_BOXNUM_SY, GFL_BMP_16_COLOR, HEAPID_BOX_APP );
		finit.bmp = fobj->bmp;
		finit.x = BOX2OBJ_TRAYNUM_PX;
		finit.y = BOX2OBJ_TRAYNUM_PY+BOX2OBJ_TRAYNUM_SY*i;
		finit.bg_pri = 1;				// BGプライオリティ
		fobj->oam = BmpOam_ActorAdd( appwk->fntoam, &finit );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＯＡＭフォント削除
 *
 * @param		appwk		ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_FontOamExit( BOX2_APP_WORK * appwk )
{
	u32	i;

	for( i=0; i<BOX2MAIN_FNTOAM_MAX; i++ ){
		BmpOam_ActorDel( appwk->fobj[i].oam );
		GFL_BMP_Delete( appwk->fobj[i].bmp );
	}
	BmpOam_Exit( appwk->fntoam );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＯＡＭフォント表示切り替え
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		idx			ＯＡＭフォントインデックス
 * @param		flg			TRUE = 表示, FALSE = 非表示
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_FontOamVanish( BOX2_APP_WORK * appwk, u32 idx, BOOL flg )
{
	BmpOam_ActorSetDrawEnable( appwk->fobj[idx].oam, flg );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＯＡＭフォント表示チェック
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		idx			ＯＡＭフォントインデックス
 *
 * @retval	"TRUE = 表示中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2OBJ_CheckFontOamVanish( BOX2_APP_WORK * appwk, u32 idx )
{
	return BmpOam_ActorGetDrawEnable( appwk->fobj[idx].oam );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボックス名をスクロール位置開始へ配置
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		idx			ＯＡＭフォントインデックス
 * @param		mv			スクロール方向
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_SetBoxNamePos( BOX2_APP_WORK * appwk, u32 idx, u32 mv )
{
	if( mv == BOX2MAIN_TRAY_SCROLL_L ){
		BmpOam_ActorSetPos( appwk->fobj[idx].oam, BOX2OBJ_BOXNAME_LPX, BOX2OBJ_BOXNAME_DPY );
	}else{
		BmpOam_ActorSetPos( appwk->fobj[idx].oam, BOX2OBJ_BOXNAME_RPX, BOX2OBJ_BOXNAME_DPY );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボックス名をスクロール
 *
 * @param		appwk		ボックス画面アプリワーク
 * @param		mv			移動量
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2OBJ_BoxNameScroll( BOX2_APP_WORK * appwk, s8 mv )
{
	u32	i;
	s16	x, y;

	for( i=BOX2MAIN_FNTOAM_BOX_NAME1; i<=BOX2MAIN_FNTOAM_BOX_NAME2; i++ ){
		BmpOam_ActorGetPos( appwk->fobj[i].oam, &x, &y );
		BmpOam_ActorSetPos( appwk->fobj[i].oam, x+mv, y );
		if( x+mv == BOX2OBJ_BOXNAME_LPX || x+mv == BOX2OBJ_BOXNAME_RPX ){
			BOX2OBJ_FontOamVanish( appwk, i, FALSE );
		}
	}
}
