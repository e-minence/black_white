//============================================================================================
/**
 * @file		b_plist_anm.c
 * @brief		戦闘用ポケモンリスト画面 ボタン制御
 * @author	Hiroyuki Nakamura
 * @date		05.03.16
 */
//============================================================================================
#include <gflib.h>

#include "b_plist_main.h"
#include "b_plist_anm.h"


//============================================================================================
//	定数定義
//============================================================================================
// ボタンアニメパターン
enum {
	BPL_BANM_PAT0 = 0,	// 通常
	BPL_BANM_PAT1,		// アニメ１
	BPL_BANM_PAT2,		// アニメ２
	BPL_BANM_NONE,		// 押せない
};

// ボタンアニメスクリーンデータ
// 戦闘中ポケモンのプレート
#define	DAT_PX_MAINPOKE_1	( 0 )
#define	DAT_PY_MAINPOKE_1	( 0 )
#define	DAT_PX_MAINPOKE_2	( 0 )
#define	DAT_PY_MAINPOKE_2	( DAT_PY_MAINPOKE_1+BPL_COMM_BSY_PLATE )
#define	DAT_PX_MAINPOKE_3	( 0 )
#define	DAT_PY_MAINPOKE_3	( DAT_PY_MAINPOKE_2+BPL_COMM_BSY_PLATE )
#define	DAT_PX_MAINPOKE_4	( 0 )
#define	DAT_PY_MAINPOKE_4	( DAT_PY_MAINPOKE_3+BPL_COMM_BSY_PLATE )
// サブポケモンのプレート
#define	DAT_PX_SUBPOKE_1	( 16 )
#define	DAT_PY_SUBPOKE_1	( 0 )
#define	DAT_PX_SUBPOKE_2	( 16 )
#define	DAT_PY_SUBPOKE_2	( DAT_PY_SUBPOKE_1+BPL_COMM_BSY_PLATE )
#define	DAT_PX_SUBPOKE_3	( 16 )
#define	DAT_PY_SUBPOKE_3	( DAT_PY_SUBPOKE_2+BPL_COMM_BSY_PLATE )
#define	DAT_PX_SUBPOKE_4	( 16 )
#define	DAT_PY_SUBPOKE_4	( DAT_PY_SUBPOKE_3+BPL_COMM_BSY_PLATE )
// 命令ボタン
#define	DAT_PX_COMMAND_1	( 0 )
#define	DAT_PY_COMMAND_1	( 39 )
#define	DAT_PX_COMMAND_2	( DAT_PX_COMMAND_1 )
#define	DAT_PY_COMMAND_2	( DAT_PY_COMMAND_1+BPL_BSY_COMMAND )
#define	DAT_PX_COMMAND_3	( DAT_PX_COMMAND_1+BPL_BSX_COMMAND )
#define	DAT_PY_COMMAND_3	( DAT_PY_COMMAND_1 )
#define	DAT_PX_COMMAND_4	( DAT_PX_COMMAND_1+BPL_BSX_COMMAND )
#define	DAT_PY_COMMAND_4	( DAT_PY_COMMAND_1+BPL_BSY_COMMAND )
// 技切り替え（上）
#define	DAT_PX_UP_1		( 0 )
#define	DAT_PY_UP_1		( 49 )
#define	DAT_PX_UP_2		( DAT_PX_UP_1+BPL_BSX_UPDOWN )
#define	DAT_PY_UP_2		( 49 )
#define	DAT_PX_UP_3		( DAT_PX_UP_2+BPL_BSX_UPDOWN )
#define	DAT_PY_UP_3		( 49 )
#define	DAT_PX_UP_4		( DAT_PX_UP_3+BPL_BSX_UPDOWN )
#define	DAT_PY_UP_4		( 49 )
// 技切り替え（下）
#define	DAT_PX_DOWN_1	( 0 )
#define	DAT_PY_DOWN_1	( 54 )
#define	DAT_PX_DOWN_2	( DAT_PX_DOWN_1+BPL_BSX_UPDOWN )
#define	DAT_PY_DOWN_2	( 54 )
#define	DAT_PX_DOWN_3	( DAT_PX_DOWN_2+BPL_BSX_UPDOWN )
#define	DAT_PY_DOWN_3	( 54 )
#define	DAT_PX_DOWN_4	( DAT_PX_DOWN_3+BPL_BSX_UPDOWN )
#define	DAT_PY_DOWN_4	( 54 )
// 戻るボタンサイズ
#define	DAT_PX_RET_1	( 26 )
#define	DAT_PY_RET_1	( 24 )
#define	DAT_PX_RET_2	( 26 )
#define	DAT_PY_RET_2	( DAT_PY_RET_1+BPL_BSY_RET )
#define	DAT_PX_RET_3	( 26 )
#define	DAT_PY_RET_3	( DAT_PY_RET_2+BPL_BSY_RET )
#define	DAT_PX_RET_4	( 26 )
#define	DAT_PY_RET_4	( DAT_PY_RET_3+BPL_BSY_RET )
// 入れ替えボタン
#define	DAT_PX_CHG_1	( 0 )
#define	DAT_PY_CHG_1	( 0 )
#define	DAT_PX_CHG_2	( 0 )
#define	DAT_PY_CHG_2	( DAT_PY_CHG_1+BPL_BSY_CHG )
#define	DAT_PX_CHG_3	( 0 )
#define	DAT_PY_CHG_3	( DAT_PY_CHG_2+BPL_BSY_CHG )
// 技ボタンサイズ
#define	DAT_PX_WAZA_1	( 0 )
#define	DAT_PY_WAZA_1	( 51 )
#define	DAT_PX_WAZA_2	( DAT_PX_WAZA_1+BPL_BSX_WAZA )
#define	DAT_PY_WAZA_2	( DAT_PY_WAZA_1 )
#define	DAT_PX_WAZA_3	( DAT_PX_WAZA_1 )
#define	DAT_PY_WAZA_3	( DAT_PY_WAZA_1+BPL_BSY_WAZA )
#define	DAT_PX_WAZA_4	( DAT_PX_WAZA_2 )
#define	DAT_PY_WAZA_4	( DAT_PY_WAZA_3 )
// 忘れるボタンサイズ
#define	DAT_PX_DEL_1	( 0 )
#define	DAT_PY_DEL_1	( 24 )
#define	DAT_PX_DEL_2	( 0 )
#define	DAT_PY_DEL_2	( DAT_PY_DEL_1+BPL_BSY_DEL )
#define	DAT_PX_DEL_3	( 0 )
#define	DAT_PY_DEL_3	( DAT_PY_DEL_2+BPL_BSY_DEL )
// コンテスト切り替えボタンサイズ
#define	DAT_PX_CONTEST_1	( 20 )
#define	DAT_PY_CONTEST_1	( 49 )
#define	DAT_PX_CONTEST_2	( 20 )
#define	DAT_PY_CONTEST_2	( DAT_PY_CONTEST_1+BPL_BSY_CONTEST )
#define	DAT_PX_CONTEST_3	( 20 )
#define	DAT_PY_CONTEST_3	( DAT_PY_CONTEST_2+BPL_BSY_CONTEST )

// 技位置ボタン
#define	DAT_PX_WP1	( 0 )
#define	DAT_PY_WP1	( 59 )
#define	DAT_PX_WP2	( 5 )
#define	DAT_PY_WP2	( 59 )
#define	DAT_PX_WP3	( 10 )
#define	DAT_PY_WP3	( 59 )

// 配置位置
#define	SCR_RETURN_PX		( 27 )
#define	SCR_RETURN_PY		( 19 )
#define	SCR_CHANGE_PX		( 0 )
#define	SCR_CHANGE_PY		( 1 )
#define	SCR_STATUS_L_PX		( 0 )
#define	SCR_STATUS_L_PY		( 19 )
#define	SCR_STATUS_R_PX		( 12 )
#define	SCR_STATUS_R_PY		( 19 )
#define	SCR_WAZASEL1_PX		( 13 )
#define	SCR_WAZASEL1_PY		( 19 )
#define	SCR_WAZASEL2_PX		( 12 )
#define	SCR_WAZASEL2_PY		( 19 )
#define	SCR_UP_PX			( 0 )
#define	SCR_UP_PY			( 19 )
#define	SCR_DOWN_PX			( 5 )
#define	SCR_DOWN_PY			( 19 )
#define	SCR_WAZA1_PX		( 0 )
#define	SCR_WAZA1_PY		( 6 )
#define	SCR_WAZA2_PX		( 16 )
#define	SCR_WAZA2_PY		( 6 )
#define	SCR_WAZA3_PX		( 0 )
#define	SCR_WAZA3_PY		( 12 )
#define	SCR_WAZA4_PX		( 16 )
#define	SCR_WAZA4_PY		( 12 )
#define	SCR_WAZA5_PX		( 8 )
#define	SCR_WAZA5_PY		( 18 )
#define	SCR_DEL_PX			( 0 )
#define	SCR_DEL_PY			( 19 )
#define	SCR_CONTEST_PX		( 23 )
#define	SCR_CONTEST_PY		( 0 )
#define	SCR_WP1_PX			( 11 )
#define	SCR_WP1_PY			( 19 )
#define	SCR_WP2_PX			( 16 )
#define	SCR_WP2_PY			( 19 )
#define	SCR_WP3_PX			( 11 )
#define	SCR_WP3_PY			( 21 )
#define	SCR_WP4_PX			( 16 )
#define	SCR_WP4_PY			( 21 )

// ボタンスクリーンデータ
typedef struct {
	u8	px;
	u8	py;
	u8	sx;
	u8	sy;
}BUTTON_ANM;

// HP関連
#define	POKE_SCRN_HP_PX			( 6 )
#define	POKE_SCRN_HP_PY			( 2 )
#define	POKE_SCRN_HP_SX			( 9 )
#define	POKE_SCRN_HP_SY			( 2 )
#define	POKE_COL_DEATH(a)		( (BPL_COMM_PAL_DN_PLATE+a) << 12 )
#define	POKE_COL_MULTI(a)		( (BPL_COMM_PAL_MN_PLATE+a) << 12 )
#define	WAZA_COL_DEL				( BPL_PAL_B_RED << 12 )


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void BPL_ScrnCopy( u16 * buf, u16 * scrn, u8 px, u8 py, u8 sx, u8 sy );
static u8 BPL_PokeChangeButtonCheck( BPLIST_WORK * wk );


//============================================================================================
//	グローバル
//============================================================================================
// ボタンスクリーンデータ
static const BUTTON_ANM ButtonScreenAnm[] =
{
	{ BPL_COMM_SCR_POKE1_PX, BPL_COMM_SCR_POKE1_PY, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE },			// ポケモン１
	{ BPL_COMM_SCR_POKE2_PX, BPL_COMM_SCR_POKE2_PY, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE },			// ポケモン２
	{ BPL_COMM_SCR_POKE3_PX, BPL_COMM_SCR_POKE3_PY, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE },			// ポケモン３
	{ BPL_COMM_SCR_POKE4_PX, BPL_COMM_SCR_POKE4_PY, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE },			// ポケモン４
	{ BPL_COMM_SCR_POKE5_PX, BPL_COMM_SCR_POKE5_PY, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE },			// ポケモン５
	{ BPL_COMM_SCR_POKE6_PX, BPL_COMM_SCR_POKE6_PY, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE },			// ポケモン６
	{ SCR_RETURN_PX, SCR_RETURN_PY, BPL_BSX_RET, BPL_BSY_RET },							// 戻る
	{ SCR_CHANGE_PX, SCR_CHANGE_PY, BPL_BSX_CHG, BPL_BSY_CHG },							// 入れ替え
	{ SCR_STATUS_L_PX, SCR_STATUS_L_PY, BPL_BSX_COMMAND, BPL_BSY_COMMAND },	// 強さを見る（左配置）
	{ SCR_STATUS_R_PX, SCR_STATUS_R_PY, BPL_BSX_COMMAND, BPL_BSY_COMMAND },	// 強さを見る（右配置）
	{ SCR_WAZASEL1_PX, SCR_WAZASEL1_PY, BPL_BSX_COMMAND, BPL_BSY_COMMAND },	// 技を見る
	{ SCR_WAZASEL2_PX, SCR_WAZASEL2_PY, BPL_BSX_COMMAND, BPL_BSY_COMMAND },	// 技を見る（単独）
	{ SCR_UP_PX, SCR_UP_PY, BPL_BSX_UPDOWN, BPL_BSY_UPDOWN },					// 切り替え（上）
	{ SCR_DOWN_PX, SCR_DOWN_PY, BPL_BSX_UPDOWN, BPL_BSY_UPDOWN },			// 切り替え（下）
	{ SCR_WAZA1_PX, SCR_WAZA1_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },				// 技１
	{ SCR_WAZA2_PX, SCR_WAZA2_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },				// 技２
	{ SCR_WAZA3_PX, SCR_WAZA3_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },				// 技３
	{ SCR_WAZA4_PX, SCR_WAZA4_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },				// 技４

	{ SCR_WAZA1_PX, SCR_WAZA1_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },		// 技回復１
	{ SCR_WAZA2_PX, SCR_WAZA2_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },		// 技回復２
	{ SCR_WAZA3_PX, SCR_WAZA3_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },		// 技回復３
	{ SCR_WAZA4_PX, SCR_WAZA4_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },		// 技回復４

	{ SCR_WAZA1_PX, SCR_WAZA1_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },		// 技忘れ１
	{ SCR_WAZA2_PX, SCR_WAZA2_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },		// 技忘れ２
	{ SCR_WAZA3_PX, SCR_WAZA3_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },		// 技忘れ３
	{ SCR_WAZA4_PX, SCR_WAZA4_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },		// 技忘れ４
	{ SCR_WAZA5_PX, SCR_WAZA5_PY, BPL_BSX_WAZA, BPL_BSY_WAZA },		// 技忘れ５

	{ SCR_DEL_PX, SCR_DEL_PY, BPL_BSX_DEL, BPL_BSY_DEL },			// 忘れさせる（戦闘）

	{ SCR_WP1_PX, SCR_WP1_PY, BPL_BSX_WP, BPL_BSY_WP },		// 技切り替え１
	{ SCR_WP2_PX, SCR_WP2_PY, BPL_BSX_WP, BPL_BSY_WP },		// 技切り替え２
	{ SCR_WP3_PX, SCR_WP3_PY, BPL_BSX_WP, BPL_BSY_WP },		// 技切り替え３
	{ SCR_WP4_PX, SCR_WP4_PY, BPL_BSX_WP, BPL_BSY_WP },		// 技切り替え４
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンスクリーン作成１
 *
 * @param		wk		戦闘ポケリストワーク
 * @param		scrn	グラフィックデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ButtonScreenMake( BPLIST_WORK * wk, u16 * scrn )
{
	// 戦闘中のプレートボタン
	BPL_ScrnCopy(
		wk->btn_plate1[0], scrn,
		DAT_PX_MAINPOKE_1, DAT_PY_MAINPOKE_1, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );
	BPL_ScrnCopy(
		wk->btn_plate1[1], scrn,
		DAT_PX_MAINPOKE_2, DAT_PY_MAINPOKE_2, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );
	BPL_ScrnCopy(
		wk->btn_plate1[2], scrn,
		DAT_PX_MAINPOKE_3, DAT_PY_MAINPOKE_3, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );
	BPL_ScrnCopy(
		wk->btn_plate1[3], scrn,
		DAT_PX_MAINPOKE_4, DAT_PY_MAINPOKE_4, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );

	// 控えのプレートボタン
	BPL_ScrnCopy(
		wk->btn_plate2[0], scrn,
		DAT_PX_SUBPOKE_1, DAT_PY_SUBPOKE_1, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );
	BPL_ScrnCopy(
		wk->btn_plate2[1], scrn,
		DAT_PX_SUBPOKE_2, DAT_PY_SUBPOKE_2, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );
	BPL_ScrnCopy(
		wk->btn_plate2[2], scrn,
		DAT_PX_SUBPOKE_3, DAT_PY_SUBPOKE_3, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );
	BPL_ScrnCopy(
		wk->btn_plate2[3], scrn,
		DAT_PX_SUBPOKE_4, DAT_PY_SUBPOKE_4, BPL_COMM_BSX_PLATE, BPL_COMM_BSY_PLATE );

	// 命令ボタン
	BPL_ScrnCopy(
		wk->btn_command[0], scrn,
		DAT_PX_COMMAND_1, DAT_PY_COMMAND_1, BPL_BSX_COMMAND, BPL_BSY_COMMAND );
	BPL_ScrnCopy(
		wk->btn_command[1], scrn,
		DAT_PX_COMMAND_2, DAT_PY_COMMAND_2, BPL_BSX_COMMAND, BPL_BSY_COMMAND );
	BPL_ScrnCopy(
		wk->btn_command[2], scrn,
		DAT_PX_COMMAND_3, DAT_PY_COMMAND_3, BPL_BSX_COMMAND, BPL_BSY_COMMAND );
	BPL_ScrnCopy(
		wk->btn_command[3], scrn,
		DAT_PX_COMMAND_4, DAT_PY_COMMAND_4, BPL_BSX_COMMAND, BPL_BSY_COMMAND );

	// 切り替えボタン（上）
	BPL_ScrnCopy( wk->btn_up[0], scrn, DAT_PX_UP_1, DAT_PY_UP_1, BPL_BSX_UPDOWN, BPL_BSY_UPDOWN );
	BPL_ScrnCopy( wk->btn_up[1], scrn, DAT_PX_UP_2, DAT_PY_UP_2, BPL_BSX_UPDOWN, BPL_BSY_UPDOWN );
	BPL_ScrnCopy( wk->btn_up[2], scrn, DAT_PX_UP_3, DAT_PY_UP_3, BPL_BSX_UPDOWN, BPL_BSY_UPDOWN );
	BPL_ScrnCopy( wk->btn_up[3], scrn, DAT_PX_UP_4, DAT_PY_UP_4, BPL_BSX_UPDOWN, BPL_BSY_UPDOWN );

	// 切り替えボタン（下）
	BPL_ScrnCopy(
		wk->btn_down[0], scrn, DAT_PX_DOWN_1, DAT_PY_DOWN_1, BPL_BSX_UPDOWN, BPL_BSY_UPDOWN );
	BPL_ScrnCopy(
		wk->btn_down[1], scrn, DAT_PX_DOWN_2, DAT_PY_DOWN_2, BPL_BSX_UPDOWN, BPL_BSY_UPDOWN );
	BPL_ScrnCopy(
		wk->btn_down[2], scrn, DAT_PX_DOWN_3, DAT_PY_DOWN_3, BPL_BSX_UPDOWN, BPL_BSY_UPDOWN );
	BPL_ScrnCopy(
		wk->btn_down[3], scrn, DAT_PX_DOWN_4, DAT_PY_DOWN_4, BPL_BSX_UPDOWN, BPL_BSY_UPDOWN );

	// 戻るボタン
	BPL_ScrnCopy( wk->btn_ret[0], scrn, DAT_PX_RET_1, DAT_PY_RET_1, BPL_BSX_RET, BPL_BSY_RET );
	BPL_ScrnCopy( wk->btn_ret[1], scrn, DAT_PX_RET_2, DAT_PY_RET_2, BPL_BSX_RET, BPL_BSY_RET );
	BPL_ScrnCopy( wk->btn_ret[2], scrn, DAT_PX_RET_3, DAT_PY_RET_3, BPL_BSX_RET, BPL_BSY_RET );
	BPL_ScrnCopy( wk->btn_ret[3], scrn, DAT_PX_RET_4, DAT_PY_RET_4, BPL_BSX_RET, BPL_BSY_RET );

	// 忘れるボタン
	BPL_ScrnCopy( wk->btn_del[0], scrn, DAT_PX_DEL_1, DAT_PY_DEL_1, BPL_BSX_DEL, BPL_BSY_DEL );
	BPL_ScrnCopy( wk->btn_del[1], scrn, DAT_PX_DEL_2, DAT_PY_DEL_2, BPL_BSX_DEL, BPL_BSY_DEL );
	BPL_ScrnCopy( wk->btn_del[2], scrn, DAT_PX_DEL_3, DAT_PY_DEL_3, BPL_BSX_DEL, BPL_BSY_DEL );

	// 技位置ボタン
	BPL_ScrnCopy( wk->btn_wp[0], scrn, DAT_PX_WP1, DAT_PY_WP1, BPL_BSX_WP, BPL_BSY_WP );
	BPL_ScrnCopy( wk->btn_wp[1], scrn, DAT_PX_WP2, DAT_PY_WP2, BPL_BSX_WP, BPL_BSY_WP );
	BPL_ScrnCopy( wk->btn_wp[2], scrn, DAT_PX_WP3, DAT_PY_WP3, BPL_BSX_WP, BPL_BSY_WP );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンスクリーン作成２
 *
 * @param		wk		戦闘ポケリストワーク
 * @param		scrn	グラフィックデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ButtonScreenMake2( BPLIST_WORK * wk, u16 * scrn )
{
	// 入れ替えボタン
	BPL_ScrnCopy( wk->btn_chg[0], scrn, DAT_PX_CHG_1, DAT_PY_CHG_1, BPL_BSX_CHG, BPL_BSY_CHG );
	BPL_ScrnCopy( wk->btn_chg[1], scrn, DAT_PX_CHG_2, DAT_PY_CHG_2, BPL_BSX_CHG, BPL_BSY_CHG );
	BPL_ScrnCopy( wk->btn_chg[2], scrn, DAT_PX_CHG_3, DAT_PY_CHG_3, BPL_BSX_CHG, BPL_BSY_CHG );

	// 技ボタン
	BPL_ScrnCopy(
		wk->btn_waza[0], scrn, DAT_PX_WAZA_1, DAT_PY_WAZA_1, BPL_BSX_WAZA, BPL_BSY_WAZA );
	BPL_ScrnCopy(
		wk->btn_waza[1], scrn, DAT_PX_WAZA_2, DAT_PY_WAZA_2, BPL_BSX_WAZA, BPL_BSY_WAZA );
	BPL_ScrnCopy(
		wk->btn_waza[2], scrn, DAT_PX_WAZA_3, DAT_PY_WAZA_3, BPL_BSX_WAZA, BPL_BSY_WAZA );
	BPL_ScrnCopy(
		wk->btn_waza[3], scrn, DAT_PX_WAZA_4, DAT_PY_WAZA_4, BPL_BSX_WAZA, BPL_BSY_WAZA );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクリーンデータコピー
 *
 * @param		buf		コピー先
 * @param		scrn	コピー元
 * @param		px		コピー元のX座標
 * @param		py		コピー元のY座標
 * @param		sx		コピーXサイズ
 * @param		sy		コピーYサイズ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ScrnCopy( u16 * buf, u16 * scrn, u8 px, u8 py, u8 sx, u8 sy )
{
	u16	i, j;

	for( i=0; i<sy; i++ ){
		for( j=0; j<sx; j++ ){
			buf[i*sx+j] = scrn[ (py+i)*32 + px+j ];
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンスクリーン取得
 *
 * @param		wk		戦闘ポケリストのワーク
 * @param		id		ボタンID
 * @param		num		アニメ番号
 * @param		pat		指定パターン（戦闘中のポケモンかどうかなど）
 *
 * @return	スクリーンデータ
 */
//--------------------------------------------------------------------------------------------
static u16 * BPL_ButtonScreenBufGet( BPLIST_WORK * wk, u8 id, u8 num, u8 pat )
{
	switch( id ){
	case BPL_BUTTON_POKE1:		// ポケモン１
	case BPL_BUTTON_POKE2:		// ポケモン２
	case BPL_BUTTON_POKE3:		// ポケモン３
	case BPL_BUTTON_POKE4:		// ポケモン４
	case BPL_BUTTON_POKE5:		// ポケモン５
	case BPL_BUTTON_POKE6:		// ポケモン６
		if( pat == 0 ){
			return wk->btn_plate1[num];
		}else{
			return wk->btn_plate2[num];
		}

	case BPL_BUTTON_RET:		// 戻る
		return wk->btn_ret[num];

	case BPL_BUTTON_POKE_CHG:	// 入れ替え
		return wk->btn_chg[num];

	case BPL_BUTTON_STATUS1:	// 強さを見る（左配置）
	case BPL_BUTTON_STATUS2:	// 強さを見る（右配置）
	case BPL_BUTTON_WAZASEL1:	// 技を見る
	case BPL_BUTTON_WAZASEL2:	// 技を見る（単独）
		return wk->btn_command[num];

	case BPL_BUTTON_UP:			// 切り替え（上）
		return wk->btn_up[num];

	case BPL_BUTTON_DOWN:		// 切り替え（下）
		return wk->btn_down[num];

	case BPL_BUTTON_WAZA1:		// 技１
	case BPL_BUTTON_WAZA2:		// 技２
	case BPL_BUTTON_WAZA3:		// 技３
	case BPL_BUTTON_WAZA4:		// 技４
	case BPL_BUTTON_WAZARCV1:	// 技回復１
	case BPL_BUTTON_WAZARCV2:	// 技回復２
	case BPL_BUTTON_WAZARCV3:	// 技回復３
	case BPL_BUTTON_WAZARCV4:	// 技回復４
	case BPL_BUTTON_WAZADEL1:	// 技忘れ１
	case BPL_BUTTON_WAZADEL2:	// 技忘れ２
	case BPL_BUTTON_WAZADEL3:	// 技忘れ３
	case BPL_BUTTON_WAZADEL4:	// 技忘れ４
	case BPL_BUTTON_WAZADEL5:	// 技忘れ５
		return wk->btn_waza[num];

	case BPL_BUTTON_WAZADEL_B:	// 忘れさせる
		return wk->btn_del[num];

	case BPL_BUTTON_WAZAPOS1:	// 技切り替え１
	case BPL_BUTTON_WAZAPOS2:	// 技切り替え２
	case BPL_BUTTON_WAZAPOS3:	// 技切り替え３
	case BPL_BUTTON_WAZAPOS4:	// 技切り替え４
		return wk->btn_wp[num];
	}

	return NULL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンスクリーン作成
 *
 * @param		wk		戦闘ポケリストのワーク
 * @param		buf		作成場所
 * @param		id		ボタンID
 * @param		num		アニメ番号
 * @param		pat		指定パターン（戦闘中のポケモンかどうかなど）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ButtonScreenMake( BPLIST_WORK * wk, u16 * buf, u8 id, u8 anm, u8 pat )
{
	u16 * scrn;
	u8	sx, sy;
	u8	i, j;

	scrn = BPL_ButtonScreenBufGet( wk, id, anm, pat );
	sx   = ButtonScreenAnm[id].sx;
	sy   = ButtonScreenAnm[id].sy;

	GFL_STD_MemCopy( scrn, buf, sx*sy*2 );

	switch( id ){
	case BPL_BUTTON_POKE1:		// ポケモン１
	case BPL_BUTTON_POKE2:		// ポケモン２
	case BPL_BUTTON_POKE3:		// ポケモン３
	case BPL_BUTTON_POKE4:		// ポケモン４
	case BPL_BUTTON_POKE5:		// ポケモン５
	case BPL_BUTTON_POKE6:		// ポケモン６
		{
			u8	pos = BPLISTMAIN_GetListRow( wk, id-BPL_BUTTON_POKE1 );
			if( wk->poke[pos].mons == 0 ){ break; }
			if( wk->poke[pos].egg != 0 ){
				u16	fill[2];
				fill[0] = buf[ POKE_SCRN_HP_PY*sx+POKE_SCRN_HP_PX-1 ];
				fill[1] = buf[ (POKE_SCRN_HP_PY+1)*sx+POKE_SCRN_HP_PX-1 ];
				for( i=0; i<POKE_SCRN_HP_SY; i++ ){
					for( j=0; j<POKE_SCRN_HP_SX; j++ ){
						buf[ (POKE_SCRN_HP_PY+i)*sx+POKE_SCRN_HP_PX+j ] = fill[i];
					}
				}
			}else{
				if( wk->poke[pos].hp == 0 ){
					for( i=0; i<sx*sy; i++ ){
						buf[i] = ( buf[i] & 0xfff ) | POKE_COL_DEATH((anm&1));
					}
				}else if( BattlePokeList_MultiPosCheck( wk, pos ) == TRUE ){
					for( i=0; i<sx*sy; i++ ){
						buf[i] = ( buf[i] & 0xfff ) | POKE_COL_MULTI((anm&1));
					}
				}
			}
		}
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタン表示
 *
 * @param		wk		戦闘ポケリストのワーク
 * @param		id		ボタンID
 * @param		anm		アニメ番号
 * @param		pat		指定パターン（戦闘中のポケモンかどうかなど）
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BPL_ButtonScreenWrite( BPLIST_WORK * wk, u8 id, u8 anm, u8 pat )
{
	u16 * buf = GFL_HEAP_AllocMemory( GFL_HEAP_LOWID(wk->dat->heap), ButtonScreenAnm[id].sx*ButtonScreenAnm[id].sy*2 );

	BPL_ButtonScreenMake( wk, buf, id, anm, pat );

	GFL_BG_WriteScreen(
		GFL_BG_FRAME2_S, buf,
		ButtonScreenAnm[id].px, ButtonScreenAnm[id].py,
		ButtonScreenAnm[id].sx, ButtonScreenAnm[id].sy );
	GFL_BG_LoadScreenV_Req( GFL_BG_FRAME2_S );

	GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンアニメ初期化
 *
 * @param		wk		戦闘ポケリストのワーク
 * @param		id		ボタンID
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ButtonAnmInit( BPLIST_WORK * wk, u8 id )
{
	wk->btn_pat = 0;
	if( id >= BPL_BUTTON_POKE1 && id <= BPL_BUTTON_POKE6 ){
		if( BattlePokeList_PokeSetCheck( wk, id - BPL_BUTTON_POKE1 ) == 2 ){
			wk->btn_pat = 1;
		}
	}
	wk->btn_seq  = 0;
	wk->btn_cnt  = 0;
	wk->btn_id   = id;
	wk->btn_flg  = 1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンアニメメイン
 *
 * @param		wk		戦闘ポケリストのワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ButtonAnmMain( BPLIST_WORK * wk )
{
	if( wk->btn_flg == 0 ){ return; }

	switch( wk->btn_seq ){
	case 0:
		BPL_ButtonScreenWrite( wk, wk->btn_id, 1, wk->btn_pat );
		wk->btn_seq++;
		break;

	case 1:
		if( wk->btn_cnt == 4 ){
			BPL_ButtonScreenWrite( wk, wk->btn_id, 0, wk->btn_pat );
			wk->btn_cnt = 0;
			wk->btn_seq++;
		}else{
			wk->btn_cnt++;
		}
		break;

	case 2:
		if( wk->btn_cnt == 1 ){
			wk->btn_cnt = 0;
			wk->btn_seq = 0;
			wk->btn_flg = 0;
		}else{
			wk->btn_cnt++;
		}
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンBG初期化
 *
 * @param		wk		戦闘ポケリストのワーク
 * @param		page	ページ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ButtonPageScreenInit( BPLIST_WORK * wk, u8 page )
{
	u16	i, j;

	switch( page ){
	case BPLIST_PAGE_SELECT:		// ポケモン選択ページ
		for( i=0; i<TEMOTI_POKEMAX; i++ ){
			j = BattlePokeList_PokeSetCheck( wk, i );
			if( j == 0 ){
				BPL_ButtonScreenWrite( wk, BPL_BUTTON_POKE1+i, BPL_BANM_NONE, 1 );
			}else if( j == 1 ){
				BPL_ButtonScreenWrite( wk, BPL_BUTTON_POKE1+i, BPL_BANM_PAT0, 0 );
			}else if( j == 2 ){
				BPL_ButtonScreenWrite( wk, BPL_BUTTON_POKE1+i, BPL_BANM_PAT0, 1 );
			}
		}
		if( wk->dat->mode == BPL_MODE_CHG_DEAD || wk->dat->mode == BPL_MODE_NO_CANCEL ){
			BPLISTANM_RetButtonPut( wk );
		}else{
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_RET, BPL_BANM_PAT0, 0 );
		}
		break;

	case BPLIST_PAGE_DEAD:			// 瀕死入れ替え選択ページ
		for( i=0; i<TEMOTI_POKEMAX; i++ ){
			j = BattlePokeList_PokeSetCheck( wk, i );
			if( j == 0 ){
				BPL_ButtonScreenWrite( wk, BPL_BUTTON_POKE1+i, BPL_BANM_NONE, 1 );
			}else if( j == 1 ){
				BPL_ButtonScreenWrite( wk, BPL_BUTTON_POKE1+i, BPL_BANM_PAT0, 0 );
			}else if( j == 2 ){
				BPL_ButtonScreenWrite( wk, BPL_BUTTON_POKE1+i, BPL_BANM_PAT0, 1 );
			}
		}
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_RET, BPL_BANM_PAT0, 0 );
		break;

	case BPLIST_PAGE_POKE_CHG:		// ポケモン入れ替えページ
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_RET, 0, 0 );
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_POKE_CHG, 0, 0 );
		if( wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].egg != 0 ){
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_STATUS1, BPL_BANM_NONE, 0 );
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZASEL1, BPL_BANM_NONE, 0 );
		}else{
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_STATUS1, BPL_BANM_PAT0, 0 );
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZASEL1, BPL_BANM_PAT0, 0 );
		}
		break;

	case BPLIST_PAGE_MAIN:			// ステータスメインページ
		if( BPL_PokeChangeButtonCheck( wk ) == TRUE ){
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_UP, BPL_BANM_PAT0, 0 );
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_DOWN, BPL_BANM_PAT0, 0 );
		}else{
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_UP, BPL_BANM_NONE, 0 );
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_DOWN, BPL_BANM_NONE, 0 );
		}
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZASEL2, 0, 0 );
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_RET, 0, 0 );
		break;

	case BPLIST_PAGE_WAZA_SEL:		// ステータス技選択ページ
		if( BPL_PokeChangeButtonCheck( wk ) == TRUE ){
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_UP, BPL_BANM_PAT0, 0 );
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_DOWN, BPL_BANM_PAT0, 0 );
		}else{
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_UP, BPL_BANM_NONE, 0 );
			BPL_ButtonScreenWrite( wk, BPL_BUTTON_DOWN, BPL_BANM_NONE, 0 );
		}
		for( i=0; i<4; i++ ){
			if( wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[i].id != 0 ){
				BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZA1+i, BPL_BANM_PAT0, 0 );
			}else{
				BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZA1+i, BPL_BANM_NONE, 0 );
			}
		}
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_STATUS2, 0, 0 );
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_RET, 0, 0 );
		break;

	case BPLIST_PAGE_SKILL:			// ステータス技詳細ページ
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_RET, 0, 0 );
		for( i=0; i<4; i++ ){
			if( wk->dat->sel_wp == i ){
				BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZAPOS1+i, BPL_BANM_PAT2, 0 );
			}else{
				BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZAPOS1+i, BPL_BANM_PAT0, 0 );
			}
		}
		break;

	case BPLIST_PAGE_PP_RCV:		// PP回復技選択ページ
		for( i=0; i<4; i++ ){
			if( wk->poke[ BPLISTMAIN_GetListRow(wk,wk->dat->sel_poke) ].waza[i].id != 0 ){
				BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZARCV1+i, BPL_BANM_PAT0, 0 );
			}else{
				BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZARCV1+i, BPL_BANM_NONE, 0 );
			}
		}
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_RET, 0, 0 );
		break;

	case BPLIST_PAGE_WAZASET_BS:	// ステータス技忘れ１ページ（戦闘技選択）
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZADEL1, 0, 0 );
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZADEL2, 0, 0 );
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZADEL3, 0, 0 );
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZADEL4, 0, 0 );
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZADEL5, 0, 0 );
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_RET, 0, 0 );
		break;

	case BPLIST_PAGE_WAZASET_BI:	// ステータス技忘れ２ページ（戦闘技詳細）
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_WAZADEL_B, 0, 0 );
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_RET, 0, 0 );
		break;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ボタンのパレットをセット
 *
 * @param		wk		戦闘ポケリストのワーク
 * @param		page	ページ番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BattlePokeList_ButtonPalSet( BPLIST_WORK * wk, u8 page )
{
	PaletteWorkSet( wk->pfd, &wk->wb_pal[0], FADE_SUB_BG, BPL_PAL_B_GREEN*16, 0x20 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		切り替えるポケモンがいるか
 *
 * @param		wk		戦闘ポケリストのワーク
 *
 * @retval	"TRUE = いる"
 * @retval	"FALSE = いない"
 */
//--------------------------------------------------------------------------------------------
static u8 BPL_PokeChangeButtonCheck( BPLIST_WORK * wk )
{
	u16	i, j;

	j = 0;
	for( i=0; i<TEMOTI_POKEMAX; i++ ){
		if( wk->poke[i].mons != 0 && wk->poke[i].egg == 0 ){
			j++;
		}
	}
	if( j >= 2 ){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		瀕死回復時に強制的にプレートの色を変える
 *
 * @param		wk		戦闘ポケリストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BPL_HPRcvButtonPut( BPLIST_WORK * wk )
{
	if( BattlePokeList_PokeSetCheck( wk, wk->dat->sel_poke ) == 2 ){
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_POKE1+wk->dat->sel_poke, 0, 1 );
	}else{
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_POKE1+wk->dat->sel_poke, 0, 0 );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		戻るボタン配置
 *
 * @param		wk		戦闘ポケリストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BPLISTANM_RetButtonPut( BPLIST_WORK * wk )
{
	u8	pos1, pos2;

	if( BPLISTMAIN_GetNewLog( wk, &pos1, &pos2, FALSE ) == TRUE ){
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_RET, BPL_BANM_PAT0, 0 );
	}else{
		BPL_ButtonScreenWrite( wk, BPL_BUTTON_RET, BPL_BANM_NONE, 0 );
	}
}
