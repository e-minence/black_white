//============================================================================================
/**
 * @file		box2_main.h
 * @brief		ボックス画面 メイン処理
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 *
 *	モジュール名：BOX2MAIN
 */
//============================================================================================
#pragma	once

#include "pm_define.h"
#include "savedata/box_savedata.h"
#include "system/palanm.h"
#include "system/bgwinfrm.h"
#include "system/cursor_move.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "app/app_taskmenu.h"
#include "app/box2.h"

/*
//#include "system/clact_tool.h"
//↑[GS_CONVERT_TAG]
#include "system/msgdata.h"
#include "system/numfont.h"
#include "system/wordset.h"
#include "system/window.h"
#include "system/touch_subwindow.h"
#include "system/fontoam.h"
#include "poketool/pokeparty.h"
#include "poketool/boxdata.h"
#include "itemtool/myitem.h"
*/

//============================================================================================
//	定数定義
//============================================================================================

// BGパレット
//#define	BOX2MAIN_BG_PAL_YNWIN	( 8 )		// はい・いいえウィンド（２本）
#define	BOX2MAIN_BG_PAL_TALKWIN	( 10 )
#define	BOX2MAIN_BG_PAL_TALKFNT	( 11 )
#define	BOX2MAIN_BG_PAL_SELWIN	( 12 )		// 選択ボタン（２本）
#define	BOX2MAIN_BG_PAL_WALL1		( 14 )		// 壁紙１
#define	BOX2MAIN_BG_PAL_WALL2		( 15 )		// 壁紙２

#define	BOX2MAIN_BG_PAL_SYSFNT_S	( 15 )

// 選択ウィンドウ定義
#define	BOX2MAIN_SELWIN_CGX_SIZ	( 3 * 4 )
#define	BOX2MAIN_SELWIN_CGX_POS	( 1024 - BOX2MAIN_SELWIN_CGX_SIZ * 2 )
#define	BOX2MAIN_SELWIN_CGX_OFF	( BOX2MAIN_SELWIN_CGX_POS )
//#define	BOX2MAIN_SELWIN_CGX_ON	( BOX2MAIN_SELWIN_CGX_OFF + BOX2MAIN_SELWIN_CGX_SIZ )
// はい・いいえウィンドウ定義
#define	BOX2MAIN_YNWIN_CGX_SIZ	( 9 )//( TOUCH_SW_USE_CHAR_NUM )
#define	BOX2MAIN_YNWIN_CGX_POS	( BOX2MAIN_SELWIN_CGX_POS - BOX2MAIN_YNWIN_CGX_SIZ )
// システムウィンドウ定義
//#define	BOX2MAIN_SYSWIN_CGX_SIZ	( TALK_WIN_CGX_SIZ )
#define	BOX2MAIN_SYSWIN_CGX_POS	( BOX2MAIN_YNWIN_CGX_POS - TALK_WIN_CGX_SIZ )


/*
// BGフレーム
#define	BOX2MAIN_BGF_STR_M	( GFL_BG_FRAME0_M )
//↑[GS_CONVERT_TAG]
#define	BOX2MAIN_BGF_BTN_M	( GFL_BG_FRAME1_M )
//↑[GS_CONVERT_TAG]
#define	BOX2MAIN_BGF_BG_M	( GFL_BG_FRAME2_M )
//↑[GS_CONVERT_TAG]
#define	BOX2MAIN_BGF_WALL_M	( GFL_BG_FRAME3_M )
//↑[GS_CONVERT_TAG]
#define	BOX2MAIN_BGF_STR_S	( GFL_BG_FRAME0_S )
//↑[GS_CONVERT_TAG]
#define	BOX2MAIN_BGF_BG_S	( GFL_BG_FRAME1_S )
//↑[GS_CONVERT_TAG]
#define	BOX2MAIN_BGF_WIN_S	( GFL_BG_FRAME2_S )
//↑[GS_CONVERT_TAG]
*/

// ポケモンアイコン数
#define	BOX2OBJ_POKEICON_H_MAX		( BOX_MAX_COLUMN )
#define	BOX2OBJ_POKEICON_V_MAX		( BOX_MAX_RAW )
#define	BOX2OBJ_POKEICON_TRAY_MAX	( BOX_MAX_POS )

#define	BOX2OBJ_POKEICON_MINE_MAX	( TEMOTI_POKEMAX )

#define	BOX2OBJ_POKEICON_MAX		( BOX2OBJ_POKEICON_TRAY_MAX + BOX2OBJ_POKEICON_MINE_MAX )

#define	BOX2OBJ_PI_OUTLINE_MAX		( 8 )

#define	BOX2OBJ_TRAYICON_MAX		( 6 )
#define	BOX2OBJ_WPICON_MAX			( 4 )

// ポケモンアイコンのキャラサイズ
#define	BOX2OBJ_POKEICON_CGX_SIZE	( 0x20 * 4 * 4 )

#define	BOX2MAIN_GETPOS_NONE		( 0xff )	// ポケモン未取得

#define	BOX2MAIN_PPP_GET_MINE		( 0xff )	// BOX2MAIN_PPPGet()で手持ちを取得するときのトレイ定義

#define	BOX2MAIN_BOXMOVE_FLG		( 0x80 )	// 他のトレイに投げるときのフラグ

// キャラリソースID
enum {
	BOX2MAIN_CHRRES_POKEICON = 0,	// ポケモンアイコン
//	BOX2MAIN_CHRRES_POKEGRA = BOX2MAIN_CHRRES_POKEICON + BOX2OBJ_POKEICON_MAX,
//	BOX2MAIN_CHRRES_POKEGRA2,
	BOX2MAIN_CHRRES_ITEMICON = BOX2MAIN_CHRRES_POKEICON + BOX2OBJ_POKEICON_MAX,
	BOX2MAIN_CHRRES_ITEMICON_SUB,
	BOX2MAIN_CHRRES_TYPEICON1,
	BOX2MAIN_CHRRES_TYPEICON2,
	BOX2MAIN_CHRRES_TRAYICON,
	BOX2MAIN_CHRRES_WPICON = BOX2MAIN_CHRRES_TRAYICON + BOX2OBJ_TRAYICON_MAX,
	BOX2MAIN_CHRRES_BOXOBJ = BOX2MAIN_CHRRES_WPICON + BOX2OBJ_WPICON_MAX,
	BOX2MAIN_CHRRES_MAX
};
// パレットリソースID
enum {
	BOX2MAIN_PALRES_POKEICON = 0,
//	BOX2MAIN_PALRES_POKEGRA,
//	BOX2MAIN_PALRES_POKEGRA2,
	BOX2MAIN_PALRES_ITEMICON,
	BOX2MAIN_PALRES_ITEMICON_SUB,
	BOX2MAIN_PALRES_TYPEICON,
	BOX2MAIN_PALRES_TRAYICON,
	BOX2MAIN_PALRES_WPICON,
	BOX2MAIN_PALRES_BOXOBJ,
	BOX2MAIN_PALRES_MAX
};
// セルリソースID
enum {
	BOX2MAIN_CELRES_POKEICON = 0,
//	BOX2MAIN_CELRES_POKEGRA,
	BOX2MAIN_CELRES_ITEMICON,
	BOX2MAIN_CELRES_TYPEICON,
	BOX2MAIN_CELRES_TRAYICON,
	BOX2MAIN_CELRES_WPICON,
	BOX2MAIN_CELRES_BOXOBJ,
	BOX2MAIN_CELRES_MAX
};

// OBJ ID
enum {
	BOX2OBJ_ID_L_ARROW = 0,
	BOX2OBJ_ID_R_ARROW,

//	BOX2OBJ_ID_POKEGRA,
//	BOX2OBJ_ID_POKEGRA2,

	BOX2OBJ_ID_BOXMV_LA,	// ボックス移動矢印（左）
	BOX2OBJ_ID_BOXMV_RA,	// ボックス移動矢印（右）
	BOX2OBJ_ID_TRAY_CUR,	// トレイカーソル
	BOX2OBJ_ID_TRAY_NAME,	// トレイ名背景
	BOX2OBJ_ID_TRAY_ARROW,	// トレイ矢印

	BOX2OBJ_ID_HAND_CURSOR,	// 手カーソル
	BOX2OBJ_ID_HAND_SHADOW,	// 手カーソル影

	BOX2OBJ_ID_ITEMICON,
	BOX2OBJ_ID_ITEMICON_SUB,
	BOX2OBJ_ID_TYPEICON1,
	BOX2OBJ_ID_TYPEICON2,

	BOX2OBJ_ID_TRAYICON,

	BOX2OBJ_ID_WPICON = BOX2OBJ_ID_TRAYICON + BOX2OBJ_TRAYICON_MAX,

	BOX2OBJ_ID_POKEICON = BOX2OBJ_ID_WPICON + BOX2OBJ_WPICON_MAX,

	BOX2OBJ_ID_OUTLINE = BOX2OBJ_ID_POKEICON + BOX2OBJ_POKEICON_MAX,

	BOX2OBJ_ID_MAX = BOX2OBJ_ID_OUTLINE + BOX2OBJ_PI_OUTLINE_MAX
};

// BMPWIN ID
enum {
	BOX2BMPWIN_ID_NAME = 0,		// ポケモン名
	BOX2BMPWIN_ID_NICKNAME,		// ニックネーム
	BOX2BMPWIN_ID_LV,			// レベル
	BOX2BMPWIN_ID_TITLE,		// タイトル
	BOX2BMPWIN_ID_SEX,			// 性別
	BOX2BMPWIN_ID_ZKNNUM,		// 図鑑番号
	BOX2BMPWIN_ID_SEIKAKU,		// 性格
	BOX2BMPWIN_ID_TOKUSEI,		// 特性
	BOX2BMPWIN_ID_ITEM,			// 持ち物
	BOX2BMPWIN_ID_WAZA1,		// 技１
	BOX2BMPWIN_ID_WAZA2,		// 技２
	BOX2BMPWIN_ID_WAZA3,		// 技３
	BOX2BMPWIN_ID_WAZA4,		// 技４
	BOX2BMPWIN_ID_WAZA1_2,		// 技１（スワップ用）
	BOX2BMPWIN_ID_WAZA2_2,		// 技２（スワップ用）
	BOX2BMPWIN_ID_WAZA3_2,		// 技３（スワップ用）
	BOX2BMPWIN_ID_WAZA4_2,		// 技４（スワップ用）

	BOX2BMPWIN_ID_SEIKAKU_STR,	//「せいかく」
	BOX2BMPWIN_ID_TOKUSEI_STR,	//「とくせい」
	BOX2BMPWIN_ID_ITEM_STR,		//「もちもの」

	BOX2BMPWIN_ID_ITEMLABEL,	// アイテム名（説明ウィンドウのタブ部分）
	BOX2BMPWIN_ID_ITEMINFO,		// アイテム説明
	BOX2BMPWIN_ID_ITEMLABEL2,	// アイテム名（説明ウィンドウのタブ部分）（スワップ用）
	BOX2BMPWIN_ID_ITEMINFO2,	// アイテム説明（スワップ用）

	BOX2BMPWIN_ID_TEMOCHI,		//「てもちポケモン」
	BOX2BMPWIN_ID_IDOU,			//「ポケモンいどう」
	BOX2BMPWIN_ID_MODORU,		//「もどる」
	BOX2BMPWIN_ID_YAMERU,		//「やめる」
	BOX2BMPWIN_ID_TOZIRU,		//「とじる」

	BOX2BMPWIN_ID_MENU1,		// メニュー１
	BOX2BMPWIN_ID_MENU2,		// メニュー２
	BOX2BMPWIN_ID_MENU3,		// メニュー３
	BOX2BMPWIN_ID_MENU4,		// メニュー４
	BOX2BMPWIN_ID_MENU5,		// メニュー５

	BMPWIN_MARK_ENTER,			// マーキング決定
	BMPWIN_MARK_CANCEL,			// マーキングキャンセル

	BMPWIN_PARTY_CNG,			// 手持ちポケモン入れ替えorやめる

	BOX2BMPWIN_ID_MSG1,			// メッセージ１
	BOX2BMPWIN_ID_MSG2,			// メッセージ２
	BOX2BMPWIN_ID_MSG3,			// メッセージ３
	BOX2BMPWIN_ID_MSG4,			// メッセージ４

	BOX2BMPWIN_ID_BOXMV_MENU,	// ボックス移動のメニュー

	BOX2BMPWIN_ID_BOXMV_BTN,	//「ボックスをきりかえる」
	BOX2BMPWIN_ID_Y_STATUS,		// Ｙボタンステータス

	BOX2BMPWIN_ID_TRAY_NAME,	// トレイ名

	BOX2BMPWIN_ID_MAX
};

// BGWINFRAME
enum {
	BOX2MAIN_WINFRM_MOVE = 0,	// ボックス移動フレーム
	BOX2MAIN_WINFRM_PARTY,		// 手持ちポケモンフレーム
	BOX2MAIN_WINFRM_MARK,		// マーキングフレーム
	BOX2MAIN_WINFRM_MENU1,		// メニューフレーム
	BOX2MAIN_WINFRM_MENU2,		// メニューフレーム
	BOX2MAIN_WINFRM_MENU3,		// メニューフレーム
	BOX2MAIN_WINFRM_MENU4,		// メニューフレーム
	BOX2MAIN_WINFRM_MENU5,		// メニューフレーム
	BOX2MAIN_WINFRM_POKE_BTN,	//「てもちポケモン」ボタン
	BOX2MAIN_WINFRM_MV_BTN,		//「ポケモンいどう」ボタン
	BOX2MAIN_WINFRM_RET_BTN,	//「もどる」ボタン
	BOX2MAIN_WINFRM_CLOSE_BTN,	//「とじる」ボタン
	BOX2MAIN_WINFRM_BOXMV_MENU,	// ボックス移動時のメニュー
	BOX2MAIN_WINFRM_WPCHG,		// 壁紙変更フレーム
	BOX2MAIN_WINFRM_BOXMV_BTN,	// ボックス移動時のボタン
	BOX2MAIN_WINFRM_Y_ST_BTN,	// Ｙボタンステータス

	BOX2MAIN_WINFRM_SUBDISP,	// 上画面用（技・持ち物）

	BOX2MAIN_WINFRM_MAX,
};

/*
// ＯＡＭフォント
enum {
	BOX2MAIN_FNTOAM_TRAY_NAME = 0,
	BOX2MAIN_FNTOAM_TRAY_NUM,
	BOX2MAIN_FNTOAM_MAX
};
*/

// VBLANK関数ワーク
typedef struct {
	void * func;
	void * freq;
	void * work;
	u16	seq;
	u16	cnt;
//	BOOL flg;		// 稼動フラグ
}BOX2_IRQWK;

/*
// ＯＡＭフォントワーク
typedef struct {
	FONTOAM_OBJ_PTR oam;
	CHAR_MANAGER_ALLOCDATA	cma;
}BOX2_FONTOAM;
*/

// ボタンアニメモード
enum {
	BOX2MAIN_BTN_ANM_MODE_OBJ = 0,
	BOX2MAIN_BTN_ANM_MODE_BG
};

// ボタンアニメワーク
typedef struct {
	u8	btn_mode:1;
	u8	btn_id:7;			// OBJ = ID, BG = FRM
	u8	btn_pal1:4;
	u8	btn_pal2:4;
	u8	btn_seq;
	u8	btn_cnt;

	u8	btn_px;			// BGのみ
	u8	btn_py;
	u8	btn_sx;
	u8	btn_sy;
}BUTTON_ANM_WORK;

// ボックス画面アプリワーク
typedef struct {
/*
	// ＯＡＭフォント
	FONTOAM_SYS_PTR	fntoam;
	BOX2_FONTOAM	fobj[BOX2MAIN_FNTOAM_MAX];

	// はい・いいえ関連
	TOUCH_SW_SYS * tsw;		// タッチウィンドウ
	u16	ynID;				// はい・いいえＩＤ

	u16	pokegra_swap;		// 上画面ポケモングラフィック切替フラグ
	u16	obj_trans_stop;		// VBLANKでのOBJ描画を停止するフラグ ( 1 = stop )


*/
	GFL_TCB * vtask;					// TCB ( VBLANK )

	BOX2_IRQWK	vfunk;			// VBLANK関数ワーク

	PALETTE_FADE_PTR	pfd;		// パレットフェードデータ

	CURSORMOVE_WORK * cmwk;		// カーソル移動ワーク
	BGWINFRM_WORK * wfrm;			// ウィンドウフレーム

	GFL_FONT * font;					// 通常フォント
	GFL_FONT * nfnt;					// 8x8フォント
	GFL_MSGDATA * mman;				// メッセージデータマネージャ
	WORDSET * wset;						// 単語セット
	STRBUF * exp;							// メッセージ展開領域
	PRINT_QUE * que;					// プリントキュー
	PRINT_STREAM * stream;		// プリントストリーム

	PRINT_UTIL	win[BOX2BMPWIN_ID_MAX];		// BMPWIN

	// はい・いいえ関連
//	TOUCH_SW_SYS * tsw;		// タッチウィンドウ
	APP_TASKMENU_ITEMWORK	ynList[2];
	APP_TASKMENU_RES * ynRes;
	APP_TASKMENU_WORK * ynWork;
	u16	ynID;				// はい・いいえＩＤ

	BUTTON_ANM_WORK	bawk;				// ボタンアニメワーク

//	ARCHANDLE * ppd_ah;				// パーソナルデータのアークハンドル

	// ポケモンアイコン
	ARCHANDLE * pokeicon_ah;

	u8	pokeicon_cgx[BOX2OBJ_POKEICON_TRAY_MAX][BOX2OBJ_POKEICON_CGX_SIZE];
	u8	pokeicon_pal[BOX2OBJ_POKEICON_TRAY_MAX];
	u8	pokeicon_id[BOX2OBJ_POKEICON_MAX];
	BOOL	pokeicon_exist[BOX2OBJ_POKEICON_TRAY_MAX];

	// トレイアイコンのキャラデータ
	u8	trayicon_cgx[BOX_MAX_TRAY][0x40*4*4];

	// OBJ
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[BOX2OBJ_ID_MAX];
	u32	chrRes[BOX2MAIN_CHRRES_MAX];
	u32	palRes[BOX2MAIN_PALRES_MAX];
	u32	celRes[BOX2MAIN_CELRES_MAX];

	BOOL cancel_flg;		// ポケモン取得キャンセルフラグ

	u32	old_cur_pos;		// 前回のカーソル位置（外部で必要なため）

	u16	get_item;						// 取得アイテム
	u16	get_item_init_pos;	// アイテム取得位置

	u8	subdisp_win_swap:4;		// サブ画面BMPWINのスワップフラグ
	u8	subdisp_waza_put:3;		// サブ画面技BMPWINのスワップフラグ
	u8	poke_free_err:1;			// 逃がすが実行できないとき１

	u8	party_frm;			// 手持ちポケモンフレーム制御

	BOOL poke_mv_cur_flg;	// ポケモンアイコン移動後にアウトラインを表示するか

	u32	pokeMark;			// マーキング変更ワーク

	// ボックス切り替え関連
	s8	wall_px;
	u8	wall_area;

	u8	wallpaper_pos;				// 壁紙選択位置

	u8	poke_get_key;		// キーでの取得位置
	u8	poke_put_key;		// キーでの配置位置

	u32	tpx;				// タッチＸ座標
	u32	tpy;				// タッチＹ座標

	int	wipe_seq;							// ワイプ後のシーケンス
	int	wait;									// ウェイト

	void * seqwk;			// シーケンスワーク

}BOX2_APP_WORK;

// ボックス画面システムワーク
typedef struct {

/*
	BOX_DATA * box;			// ボックスデータ
	POKEPARTY * party;		// 手持ちポケモン
	MYITEM * item;			// アイテムデータ
	CONFIG * config;		// コンフィグデータ


	u8	pos;				// 現在のカーソル位置
*/


	BOX2_GFL_PROC_PARAM * dat;	// 外部データ

//	GFL_PROC * subProcFunc;		// サブプロセス
	void * subProcWork;		// サブプロセスで使用するワーク
	u16	subRet;				// サブプロセスの戻り値などを保存する場所
	u8	subProcType;		// サブプロセスの種類


	u8	tray;										// 現在のトレイ

	u8	get_pos;								// 取得位置

	u8	box_mv_pos;							// ボックス移動選択位置

	u8	move_mode;			// ポケモン移動モード

	u8	quick_mode;			//「ポケモンいどう」のモード ( 0 = 通常, 1 = トレイポケモン, 2 = 手持ちポケモン )
	u8	quick_get;			//「ポケモンいどう」メニューでの取得位置

	u8	y_status_flg;		// Ｙボタンでステータスを開いたかどうか
	u8	y_status_hand;		// Ｙステータス時に掴んでいるか
	u8	y_status_pos;		// Ｙステータス時に掴んでいたときの戻り位置

	u8	pokechg_mode;		// 入れ替えモード

	u8	party_sel_flg;		// 手持ち選択フラグ
	u8	box_mv_flg;			// ボックス移動フラグ
//	u8	box_mv_pos;			// ボックス移動選択位置

	u32	cur_rcv_pos;		// カーソル表示復帰位置

	int	next_seq;								// 次のシーケンス

	BOX2_APP_WORK * app;				// アプリワーク

}BOX2_SYS_WORK;

typedef int (*pBOX2_FUNC)(BOX2_SYS_WORK*);


/*
// キー操作データ取得定義
enum {
	BOX2MAIN_KEY_JUMP_UP = 0,
	BOX2MAIN_KEY_JUMP_DOWN,
	BOX2MAIN_KEY_JUMP_LEFT,
	BOX2MAIN_KEY_JUMP_RIGHT,
};

// 場所タイプ取得定義
enum {
	BOX2MAIN_POS_TYPE_BOXNAME = 0,
	BOX2MAIN_POS_TYPE_SCROLL_L,
	BOX2MAIN_POS_TYPE_SCROLL_R,
	BOX2MAIN_POS_TYPE_BOXPOKE,
	BOX2MAIN_POS_TYPE_PARTYPOKE,
};
*/
// ボックス切り替え方向
enum {
	BOX2MAIN_TRAY_SCROLL_L = 0,
	BOX2MAIN_TRAY_SCROLL_R,
	BOX2MAIN_TRAY_SCROLL_NONE,
};

// ポケモンを逃がす処理のワーク
typedef struct {
	GFL_CLWK * clwk;		// アイコンセルアクター
	u16	check_cnt;			// 技チェックカウント
	u8	check_flg;			// 技チェックフラグ ( 0xff = def, 0 = none, 1 = hit )
	u8	scale_cnt;			// 拡縮カウント
	f32	scale;				// 拡縮値
}BOX2MAIN_POKEFREE_WORK;

// 手持ちフレーム動作定義
#define	BOX2MAIN_PARTYPOKE_FRM_SPD	( 8 )		// 移動量
#define	BOX2MAIN_PARTYPOKE_FRM_CNT	( 18 )		// カウント

#define	BOX2MAIN_PARTYPOKE_FRM_H_SPD	( 8 )	// 手持ちポケモンフレーム横スクロール速度
#define	BOX2MAIN_PARTYPOKE_FRM_H_CNT	( 19 )	// 手持ちポケモンフレーム横スクロールカウント

// ポケモン移動モード
enum {
	BOX2MAIN_POKEMOVE_MODE_TRAY = 0,
	BOX2MAIN_POKEMOVE_MODE_PARTY,
	BOX2MAIN_POKEMOVE_MODE_ALL,
};

// 取得ポケモン移動
typedef struct {
//	POKEMON_PARAM * pp;
	u32	df_pos;
	u32	mv_pos;
	u32	flg;
	u32	mx;
	u32	my;
	s16	vx;
	s16	vy;
	s16	dx;
	s16	dy;
}BOX2MAIN_POKEMOVE_DATA;

typedef struct {
	BOX2MAIN_POKEMOVE_DATA	dat[BOX2OBJ_POKEICON_MINE_MAX+1];	// 手持ち＋トレイ選択分
	u32	cnt;
	u32	get_pos;
	u32	put_pos;
	u32	set_pos;
}BOX2MAIN_POKEMOVE_WORK;

// 取得アイテム移動
typedef struct {
	u16	put_pos;
	u16	set_pos;
	u32	mv_mode;

	u32	now_x;
	u32	now_y;
	u32	mx;
	u32	my;
	u32	mv_x:1;
	u32	mv_y:1;
	u32	cnt:30;
}BOX2MAIN_ITEMMOVE_WORK;

// 表示されているトレイアイコンのトレイ番号の基準値を取得
#define	BOX2MAIN_MV_TRAY_DEF(a)	( a / BOX2OBJ_TRAYICON_MAX * BOX2OBJ_TRAYICON_MAX )

// カーソル移動
typedef struct {
	u8	px;			// 移動先Ｘ座標
	u8	py;			// 移動先Ｙ座標
	u8	vx;			// Ｘ移動量
	u8	vy;			// Ｙ移動量
	u32	mx:1;		// Ｘ移動方向
	u32	my:1;		// Ｙ移動方向
	u32	cnt:30;		// カウンタ
}BOX2MAIN_CURSORMOVE_WORK;

// 上画面表示用データ
typedef struct {
	POKEMON_PASO_PARAM * ppp;

	u16	mons;					// ポケモン番号
	u16	item;					// アイテム番号

	u32	rand;					// 個性乱数

	u8	type1;				// タイプ１
	u8	type2;				// タイプ２
	u8	tokusei;			// 特性
	u8	seikaku;			// 性格

	u16	mark;					// ボックスマーク
	u8	lv:7;					// レベル
	u8	tamago:1;			// タマゴフラグ
	u8	sex:7;				// 性別
	u8	sex_put:1;		// 性別表示（ニドラン用）1=ON, 0=OFF

	u16	waza[4];			// 技

}BOX2_POKEINFO_DATA;


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK関数設定
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_InitVBlank( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK関数削除
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_ExitVBlank( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		VRAM設定
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_VramBankSet(void);

extern const GFL_DISP_VRAM * BOX2MAIN_GetVramBankData(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧ初期化
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧ解放
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧグラフィック読み込み
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgGraphicLoad( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		パレットフェード初期化
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PaletteFadeInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		パレットフェード解放
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PaletteFadeExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	 アルファブレンド設定
 *
 * @param		flg		TRUE = 有効, FALSE = 無効
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_SetBlendAlpha( BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  メッセージ関連初期化
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_MsgInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  メッセージ関連解放
 *
 * @param		syswk		ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_MsgExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンデータ取得
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		pos			位置
 * @param		prm			取得パラメータ
 * @param		buf			取得バッファ
 *
 * @return	取得データ
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2MAIN_PokeParaGet( BOX2_SYS_WORK * syswk, u32 pos, int prm, void * buf );

//--------------------------------------------------------------------------------------------
/**
 * @brief		ポケモンデータ設定
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		pos			位置
 * @param		prm			設定パラメータ
 * @param		buf			設定データ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeParaPut( BOX2_SYS_WORK * syswk, u32 pos, int prm, u32 buf );

//--------------------------------------------------------------------------------------------
/**
 * @brief		POKEMON_PASO_PARAM取得
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		tray		トレイ番号
 * @param		pos			位置
 *
 * @return	POKEMON_PASO_PARAM
 */
//--------------------------------------------------------------------------------------------
extern POKEMON_PASO_PARAM * BOX2MAIN_PPPGet( BOX2_SYS_WORK * syswk, u32 tray, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * @brief		壁紙セット
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		id			壁紙ＩＤ
 * @param		mv			スクロール方向
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_WallPaperSet( BOX2_SYS_WORK * syswk, u32 id, u32 mv );

//--------------------------------------------------------------------------------------------
/**
 * @brief		壁紙番号取得
 *
 * @param		syswk		ボックス画面システムワーク
 * @param		num			トレイ番号
 *
 * @return	壁紙番号
 *
 *	デフォルト壁紙からの通し番号にする
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2MAIN_GetWallPaperNumber( BOX2_SYS_WORK * syswk, u32 num );














#if 1
//============================================================================================
//	システム関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * VBLANK処理
 *
 * @param	tcb
 * @param	work
 *
 * @return	none
 *
 *	1/60のための処理
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_VBlank( GFL_TCB* tcb, void * work );
//↑[GS_CONVERT_TAG]

//--------------------------------------------------------------------------------------------
/**
 * VBLANK内で呼ばれる関数を設定
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	func	関数ポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_VFuncSet( BOX2_APP_WORK * appwk, void * func );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK内で呼ばれる関数を予約
 *
 * @param	appwk	ボックス画面アプリワーク
 * @param	func	関数ポインタ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_VFuncReq( BOX2_APP_WORK * appwk, void * func );

//--------------------------------------------------------------------------------------------
/**
 * 予約したVBLANK関数をセット
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_VFuncReqSet( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * VRAM設定
 *
 * @param	none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_VramBankSet(void);

//--------------------------------------------------------------------------------------------
/**
 * ＢＧ初期化
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ＢＧ解放
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ＢＧグラフィック読み込み
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgGraphicLoad( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * メッセージ関連初期化
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_MsgInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * メッセージ関連解放
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_MsgExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ウィンドウ読み込み
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_WindowLoad( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * パレットフェード初期化
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PaletteFadeInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * パレットフェード解放
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PaletteFadeExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * アルファブレンド設定
 *
 * @param	flg		TRUE = 有効, FALSE = 無効
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_SetBlendAlpha( BOOL flg );

//--------------------------------------------------------------------------------------------
/**
 * はい・いいえ処理初期化
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_YesNoWinInit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * はい・いいえ処理解放
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_YesNoWinExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * はい・いいえセット
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	pos		カーソル位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_YesNoWinSet( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * ボタンアニメ処理
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"TRUE = アニメ中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2MAIN_ButtonAnmMain( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * キータッチステータスセット
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none 
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_KeyTouchStatusSet( BOX2_SYS_WORK * syswk );


//============================================================================================
//	ポケモンデータ
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ポケモンデータ取得
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	pos		位置
 * @param	prm		取得パラメータ
 * @param	buf		取得バッファ
 *
 * @return	取得データ
 *
 * @li	ポケモンが存在したいときは"NULL"を返す
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2MAIN_PokeParaGet( BOX2_SYS_WORK * syswk, u32 pos, int prm, void * buf );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンデータクリア
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	tray	トレイ番号
 * @param	pos		位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeDataClear( BOX2_SYS_WORK * syswk, u32 tray, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンデータ移動
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeDataMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 指定位置以外に戦えるポケモンがいるか
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	pos		対象外のポケモン位置
 *
 * @retval	"TRUE = いる"
 * @retval	"FALSE = いない"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2MAIN_BattlePokeCheck( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * 持ち物変更によるフォルムチェンジ
 *
 * @param	ppp		POKEMON_PASO_PARAM
 *
 * @retval	"TRUE = フォルムチェンジ"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2MAIN_PokeItemFormChange( POKEMON_PASO_PARAM * ppp );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちからボックスに移動させたときのシェイミのフォルムチェンジ
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	b_pos	移動前の位置
 * @param	a_pos	移動後の位置
 *
 * @return	none
 *
 *	データは入れ替えたあとに呼ぶ
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_FormChangeSheimi( BOX2_SYS_WORK * syswk, u32 b_pos, u32 a_pos );

//--------------------------------------------------------------------------------------------
/**
 * 取得アイテムを指定場所のポケモンにセットできるかチェック
 *
 * @param	syswk		ボックス画面システムワーク
 * @param	get_pos		取得場所
 * @param	put_pos		配置位置
 *
 * @retval	"TRUE = 可"
 * @retval	"FALSE = 不可"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2MAIN_PokeItemMoveCheck( BOX2_SYS_WORK * syswk, u32 get_pos, u32 put_pos );


//============================================================================================
//	ポケモンを逃がす
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンを逃がす」ワーク作成
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeFreeCreate( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンを逃がす」ワーク解放
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeFreeExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 逃がせるかどうか技チェック
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	"TRUE = チェック中"
 * @return	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2MAIN_PokeFreeWazaCheck( BOX2_SYS_WORK * syswk );


//============================================================================================
//	データ表示関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 選択中のポケモンの情報表示
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	pos		位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern BOOL BOX2MAIN_PokeInfoPut( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * 上画面の一部を再描画
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	pos		位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeInfoRewrite( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * 上画面表示オフ
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeInfoOff( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 非選択に切り替え
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_PokeSelectOff( BOX2_SYS_WORK * syswk );


//============================================================================================
//	壁紙
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 壁紙セット
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	id		壁紙ＩＤ
 * @param	mv		スクロール方向
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_WallPaperSet( BOX2_SYS_WORK * syswk, u32 id, u32 mv );

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	id		壁紙番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_WallPaperChange( BOX2_SYS_WORK * syswk, u32 id );

//--------------------------------------------------------------------------------------------
/**
 * 壁紙番号取得
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	num		トレイ番号
 *
 * @return	壁紙番号
 *
 *	デフォルト壁紙からの通し番号にする
 */
//--------------------------------------------------------------------------------------------
extern u32 BOX2MAIN_GetWallPaperNumber( BOX2_SYS_WORK * syswk, u32 num );


//============================================================================================
//	ＢＧフレーム
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ＢＧフレームワーク作成（全体）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgFrameWorkMake( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ＢＧフレームワーク解放
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_BgFrameWorkExit( BOX2_APP_WORK * appwk );


//============================================================================================
//	マーキングフレーム
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * マークキングフレーム表示
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_MarkingFramePut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * マーク表示切り替え
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	pos		マーク位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_MarkingSwitch( BOX2_SYS_WORK * syswk, u32 pos );

//--------------------------------------------------------------------------------------------
/**
 * マーキングフレームインセット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_MarkingFrameInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * マーキングフレームアウトセット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_MarkingFrameOutSet( BGWINFRM_WORK * wk );


//============================================================================================
//	手持ちポケモンフレーム
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレーム表示
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFramePut( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレーム表示（右）
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFramePutRight( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレーム非表示
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameOff( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレーム初期位置設定（左）
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameInitPutLeft( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレーム初期位置設定（右）
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameInitPutRight( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレームインセット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレームアウトセット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレーム右移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameRightMoveSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレーム右移動セット（「ポケモンをあずける」用）
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_AzukeruPartyPokeFrameRightMoveSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレーム左移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameLeftMoveSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 手持ちポケモンフレーム左移動セット（「ポケモンをあずける」用）
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_AzukeruPartyPokeFrameLeftMoveSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ＢＧウィンドウフレームに手持ちポケモンフレームグラフィックをセット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 * @param	index	ＢＧウィンドウフレームインデックス
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PartyPokeFrameLoadArrange2( BGWINFRM_WORK * wk, u32 index );

//--------------------------------------------------------------------------------------------
/**
 * 「てもちポケモンボタン」ボタン配置
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_ButtonPutTemochi( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「てもちポケモンボタン」ボタンを画面外に配置
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_ButtonOutPutTemochi( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「てもちポケモンボタン」ボタン画面外への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_TemochiButtonOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 「てもちポケモンボタン」ボタン画面内への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_TemochiButtonInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンいどう」ボタン配置
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_ButtonPutIdou( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンいどう」ボタンを画面外に配置
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_ButtonOutPutIdou( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンいどう」ボタン画面外への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_IdouButtonOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンいどう」ボタン画面内への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_IdouButtonInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 「もどる」ボタン配置
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_ButtonPutModoru( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「やめる」ボタン配置
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_ButtonPutYameru( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * キャンセルボタンを画面外に配置
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_RetButtonOutPut( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * キャンセルボタン画面外への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_RetButtonOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * キャンセルボタン画面内への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_RetButtonInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 「とじる」ボタン配置チェック
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @retval	"TRUE = 配置済み"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
//extern BOOL BOX2MAIN_CloseButtonPutCheck( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 「てもちポケモン」「ポケモンいどう」ボタン非表示
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxPartyButtonVanish( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンメニューボタンを画面内に配置
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PokeMenuOpenPosSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンメニューボタン非表示
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PokeMenuOff( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンメニューボタン画面内への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PokeMenuInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンメニューボタン画面外への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_PokeMenuOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ポケモンメニュー配置チェック
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @retval	"TRUE = 配置済み"
 * @retval	"FLASE = それ以外"
 */
//--------------------------------------------------------------------------------------------
//extern BOOL BOX2MAIN_PokeMenuPutCheck( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ボックステーマ変更フレーム画面内への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxThemaMenuInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ボックステーマ変更フレーム画面外への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxThemaMenuOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをきりかえる」ボタン画面内への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveButtonInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをきりかえる」ボタン画面外への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveButtonOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをきりかえる」ボタン配置チェック
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @retval	"TRUE = 配置済み"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
//extern BOOL BOX2MAIN_BoxMoveButtonCheck( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * Ｙステータスボタン画面内への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_YStatusButtonInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * Ｙステータスボタンを画面内へ配置
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_YStatusButtonPut( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * Ｙステータスボタン画面外への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_YStatusButtonOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * Ｙステータスボタン配置チェック
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @retval	"TRUE = 配置済み"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
//extern BOOL BOX2MAIN_YStatusButtonCheck( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * トレイ選択フレームを画面内へ配置
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 *
 *	他画面から復帰時にボックス移動フレームを表示
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveFrmPut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * トレイ選択フレーム画面内への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveFrmInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * トレイ選択フレーム画面外への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveFrmOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更フレームインセット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_WallPaperFrameInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更フレームアウトセット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_WallPaperFrameOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ボックス切り替えメニューボタン画面内への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 *
 * @li	壁紙変更、預けるボックス選択で使用してます
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveMenuInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ボックス切り替えメニューボタン画面外への移動セット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 *
 * @li	壁紙変更、預けるボックス選択で使用してます
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxMoveMenuOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * ボックステーマ変更フレームを画面内へ配置
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 *
 *	他画面から復帰時にボックステーマ変更関連フレームを表示
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_BoxThemaFrmPut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 「てもちポケモン」「ポケモンいどう」を非表示にする
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_ArrangeUnderButtonDel( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * サブ画面技フレームインセット
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispWazaFrmInSet( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * サブ画面技フレームアウトセット
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispWazaFrmOutSet( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * サブ画面技フレーム画面内に配置
 *
 * @param	appwk	ボックス画面アプリワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispWazaFrmInPosSet( BOX2_APP_WORK * appwk );

//--------------------------------------------------------------------------------------------
/**
 * サブ画面技フレーム画面外に配置
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_SubDispWazaFrmOutPosSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * サブ画面持ち物フレームインセット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispItemFrmInSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * サブ画面持ち物フレームアウトセット
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispItemFrmOutSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * サブ画面持ち物フレームを画面内に配置
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispItemFrmInPosSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * サブ画面持ち物フレームを画面外に配置
 *
 * @param	wk		ＢＧウィンドウフレームワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
//extern void BOX2MAIN_SubDispItemFrmOutPosSet( BGWINFRM_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * 上画面のマーキングを変更
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	maek	マークデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_SubDispMarkingChange( BOX2_SYS_WORK * syswk, u32 mark );


//============================================================================================
//	サブプロセス
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ステータス画面呼び出し
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_PokeStatusCall( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * ステータス画面終了
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_PokeStatusExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * バッグ画面呼び出し
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_BagCall( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * バッグ画面終了
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_BagExit( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 名前入力画面呼び出し
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_NameInCall( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * 名前入力画面終了
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	0
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_NameInExit( BOX2_SYS_WORK * syswk );


//============================================================================================
//	VBLANK FUNC
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 取得ポケモン移動（トレイ）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncGetPokeMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 取得ポケモン移動（パーティ）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncGetPokeMoveParty( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 取得ポケモン移動（パーティ＆トレイ）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncGetPokeMoveBoxParty( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 手持ちを逃がした後のOBJ動作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPartyPokeFreeSort( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ポケモンを手持ちに加える動作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPartyInPokeMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ポケモンを預ける動作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPartyOutPokeMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : トレイをスクロール（左ボタンが押されたとき）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncTrayScrollLeft( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : トレイをスクロール（右ボタンが押されたとき）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncTrayScrollRight( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ポケモンメニュー動作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPokeMenuMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : メニュー・「てもちポケモン」・「ポケモンいどう」・「もどる」動作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPokeFreeMenuMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : マーキングフレーム動作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncMarkingFrameMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 「てもちポケモン」「ポケモンいどう」「とじる」「もどる」などのボタン動作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncUnderButtonMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 手持ちポケモンフレーム動作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPartyFrameMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 手持ちポケモンフレームを右へ
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPartyFrmRight( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 手持ちポケモンフレームを左へ
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPartyFrmLeft( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ボックス選択ウィンドウイン
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncBoxMoveFrmIn( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ボックス選択ウィンドウアウト
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncBoxMoveFrmOut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : やめるフレームイン
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncExitButtonMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 「ボックスをきりかえる」フレームイン
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncBoxMoveMenuIn( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 「ボックスをきりかえる」フレームアウト
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncBoxMoveMenuOut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ボックステーマ変更メニューイン
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncBoxThemaMenuMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 壁紙変更フレームイン
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncWallPaperFrmIn( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 壁紙変更フレームアウト
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncWallPaperFrmOut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : あずけるボックス選択フレームイン
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPokeOutBoxFrmIn( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : あずけるボックス選択フレームアウト
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPokeOutBoxFrmOut( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 「ボックスをきりかえる」ボタンフレームイン
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncBoxMoveButtonMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 「ボックスをきりかえる」ボタンＹステータスボタン動作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncBoxMoveButtonYStatusButtonMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : Ｙステータスボタンフレームイン
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncYStatusButtonMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 「あずける」「やめる」フレーム動作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPartyOutButtonMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 取得ポケモン移動（ポケモンをあずける）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_GetPartyPokeMoveDrop( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 取得ポケモン移動（ポケモンをつれていく）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_GetTrayPokeMoveDrop( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 手カーソル移動
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncCursorMove( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 戻るを消してメニューを出す
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncMenuOpenKey( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 戻るを消してメニューを出す（ボックス整理）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncMenuOpenKeyArrange( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : メニューを消して戻るを出す
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncMenuCloseKey( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : メニューを消して戻るを出す（ボックス整理）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncMenuCloseKeyArrange( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 手カーソルでポケモン取得
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPokeMoveGetKey( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ポケモンを放す
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncPokeMovePutKey( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : アイテム取得（持ち物整理・キー操作）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemArrangeGetKey( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : アイテム取得（持ち物整理・タッチ操作）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemArrangeGetTouch( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : メニューを消して戻るを出す（持ち物整理）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemArrangeMenuClose( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : アイテムアイコン入れ替え動作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemIconChange( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 持ち物整理・トレイ/手持ち取得（タッチ）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemArrangeBoxPartyIconSetTouch( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 持ち物整理・トレイ/手持ち入れ替え（タッチ）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemArrangeBoxPartyIconChgTouch( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 手カーソルでアイテム取得
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemArrangeBoxPartyIconGetKey( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : アイテムを放す
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemArrangeBoxPartyIconPutKey( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : アイテムを放す・キャンセル
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemArrangeBoxPartyIconPutKeyCancel( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : 手持ちポケモンのアイテム取得（持ち物整理・タッチ操作）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemArrangePartyGetTouch( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : アイテムアイコン入れ替え動作（手持ちポケモン）
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern int BOX2MAIN_VFuncItemIconPartyChange( BOX2_SYS_WORK * syswk );

//--------------------------------------------------------------------------------------------
/**
 * VBLANK FUNC : ポケモンアイコンを手で取得した状態にする
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"1 = 処理中"
 * @retval	"0 = それ以外"
 */
//--------------------------------------------------------------------------------------------
extern void BOX2MAIN_HandGetPokeSet( BOX2_SYS_WORK * syswk );

#endif
