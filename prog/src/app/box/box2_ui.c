//============================================================================================
/**
 * @file		box2_ui.c
 * @brief		ボックス画面 キータッチ関連
 * @author	Hiroyuki Nakamura
 * @date		09.10.05
 *
 *	モジュール名：BOX2_UI
 */
//============================================================================================
#include <gflib.h>

#include "system/cursor_move.h"

#include "system/main.h"

#include "box2_main.h"
#include "box2_bmp.h"
#include "box2_obj.h"
#include "box2_ui.h"
#include "box2_bgwfrm.h"


//============================================================================================
//	定数定義
//============================================================================================

// カーソル移動データ
typedef struct {
//	const GFL_UI_TP_HITTBL * hit;
//	const CURSORMOVE_DATA * pos;
	const CURSORMOVE_DATA * dat;
	const CURSORMOVE_CALLBACK * cb;
}MOVE_DATA_TBL;

#define	TRAYPOKE_SIZE_X		( BOX2OBJ_TRAYPOKE_SX )							// トレイのポケモンのタッチ範囲サイズＸ
#define	TRAYPOKE_SIZE_Y		( 24 )											// トレイのポケモンのタッチ範囲サイズＹ
#define	TRAYPOKE_START_X	( BOX2OBJ_TRAYPOKE_PX - TRAYPOKE_SIZE_X / 2 )	// トレイのポケモンのタッチ開始座標Ｘ
#define	TRAYPOKE_START_Y	( 40 )											// トレイのポケモンのタッチ開始座標Ｙ

#define	TRAYPOKE_PX(a)	( TRAYPOKE_START_X + TRAYPOKE_SIZE_X * a )		// トレイのポケモンのタッチ開始座標Ｘ
#define	TRAYPOKE_PY(a)	( TRAYPOKE_START_Y + TRAYPOKE_SIZE_Y * a )		// トレイのポケモンのタッチ開始座標Ｙ

#define	PICUR_X		( 12 )	// カーソル表示補正Ｘ座標
#define	PICUR_Y		( -4 )	// カーソル表示補正Ｙ座標


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void CursorObjMove( BOX2_SYS_WORK * syswk, int pos );

static void CursorMoveCallBack_On( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Off( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Move( void * work, int now_pos, int old_pos );
static void CursorMoveCallBack_Touch( void * work, int now_pos, int old_pos );

static void PartyOutMainCallBack_On( void * work, int now_pos, int old_pos );

static void PartyOutBoxSelCallBack_On( void * work, int now_pos, int old_pos );
static void PartyOutBoxSelCallBack_Move( void * work, int now_pos, int old_pos );
static void PartyOutBoxSelCallBack_Touch( void * work, int now_pos, int old_pos );

static void PartyInMainCallBack_On( void * work, int now_pos, int old_pos );
static void PartyInMainCallBack_Touch( void * work, int now_pos, int old_pos );

static void BoxArrangeMainCallBack_On( void * work, int now_pos, int old_pos );
static void BoxArrangeMainCallBack_Touch( void * work, int now_pos, int old_pos );
static void BoxArrangeMainCallBack_Move( void * work, int now_pos, int old_pos );

static void BoxArrangePokeMoveCallBack_On( void * work, int now_pos, int old_pos );
static void BoxArrangePokeMoveCallBack_Move( void * work, int now_pos, int old_pos );
static void BoxArrangePokeMoveCallBack_Touch( void * work, int now_pos, int old_pos );

static void BoxArrangePartyMoveCallBack_On( void * work, int now_pos, int old_pos );
static void BoxArrangePartyMoveCallBack_Move( void * work, int now_pos, int old_pos );

static void BoxItemArrangeMainCallBack_On( void * work, int now_pos, int old_pos );
static void BoxItemArrangeMainCallBack_Touch( void * work, int now_pos, int old_pos );
static void BoxItemArrangeMainCallBack_Move( void * work, int now_pos, int old_pos );

static void BoxItemArrangePartyMoveCallBack_On( void * work, int now_pos, int old_pos );
static void BoxItemArrangePartyMoveCallBack_Move( void * work, int now_pos, int old_pos );

static void BoxThemaChgCallBack_On( void * work, int now_pos, int old_pos );
static void BoxThemaChgCallBack_Move( void * work, int now_pos, int old_pos );
static void BoxThemaChgCallBack_Touch( void * work, int now_pos, int old_pos );

static void WallPaperChgCallBack_On( void * work, int now_pos, int old_pos );
static void WallPaperChgCallBack_Move( void * work, int now_pos, int old_pos );
static void WallPaperChgCallBack_Touch( void * work, int now_pos, int old_pos );


//============================================================================================
//	グローバル変数
//============================================================================================

// あずける・メイン
static const CURSORMOVE_DATA PartyOutMainCursorMoveData[] =
{
	{  40,  52, 0, 0,	 6,  2,  0,  1, {  68-12,  68+12-1, 42-12, 42+12-1 } },		// 00: ポケモン１
	{  80,  60, 0, 0,	 6,  3,  0,  2, {  76-12,  76+12-1, 78-12, 78+12-1 } },		// 01: ポケモン２
	{  40,  84, 0, 0,	 0,  4,  1,  3, { 100-12, 100+12-1, 42-12, 42+12-1 } },		// 02: ポケモン３
	{  80,  92, 0, 0,	 1,  5,  2,  4, { 108-12, 108+12-1, 78-12, 78+12-1 } },		// 03: ポケモン４
	{  40, 116, 0, 0,	 2,  6,  3,  5, { 132-12, 132+12-1, 42-12, 42+12-1 } },		// 04: ポケモン５
	{  80, 124, 0, 0,	 3,  6,  4,  6, { 140-12, 140+12-1, 78-12, 78+12-1 } },		// 05: ポケモン６

	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|5, 0, 5, 6, { 168, 191, 192, 255 } },		// 06: もどる

	{ 212,  64, 0, 0,	11,  8,  7,  7, {  64,  87, 168, 255 } },		// 07: あずける
	{ 212,  88, 0, 0,	 7,  9,  8,  8, {  88, 111, 168, 255 } },		// 08: ようすをみる
	{ 212, 112, 0, 0,	 8, 10,  9,  9, { 112, 135, 168, 255 } },		// 09: マーキング
	{ 212, 136, 0, 0,	 9, 11, 10, 10, { 136, 159, 168, 255 } },		// 10: にがす
	{ 224, 168, 0, 0,	10,  7, 11, 11, { 168, 191, 192, 255 } }		// 11: とじる
};
/*
static const GFL_UI_TP_HITTBL PartyOutMainHitTbl[] =
{
	{  68-12,  68+12-1, 42-12, 42+12-1 },	// 00: ポケモン１
	{  76-12,  76+12-1, 78-12, 78+12-1 },	// 01: ポケモン２
	{ 100-12, 100+12-1, 42-12, 42+12-1 },	// 02: ポケモン３
	{ 108-12, 108+12-1, 78-12, 78+12-1 },	// 03: ポケモン４
	{ 132-12, 132+12-1, 42-12, 42+12-1 },	// 04: ポケモン５
	{ 140-12, 140+12-1, 78-12, 78+12-1 },	// 05: ポケモン６

	{ 168, 191, 192, 255 },		// 06: もどる

	{  64,  87, 168, 255 },		// 07: あずける
	{  88, 111, 168, 255 },		// 08: ようすをみる
	{ 112, 135, 168, 255 },		// 09: マーキング
	{ 136, 159, 168, 255 },		// 10: にがす
	{ 168, 191, 192, 255 },		// 11: とじる

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA PartyOutMainKeyTbl[] =
{
	{ 40,  52, 0, 0,	6, 2, 0, 1 },	// 00: ポケモン１
	{ 80,  60, 0, 0,	6, 3, 0, 2 },	// 01: ポケモン２
	{ 40,  84, 0, 0,	0, 4, 1, 3 },	// 02: ポケモン３
	{ 80,  92, 0, 0,	1, 5, 2, 4 },	// 03: ポケモン４
	{ 40, 116, 0, 0,	2, 6, 3, 5 },	// 04: ポケモン５
	{ 80, 124, 0, 0,	3, 6, 4, 6 },	// 05: ポケモン６

	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|5, 0, 5, 6 },	// 06: もどる

	{ 212,  64, 0, 0,	11,  8,  7,  7 },	// 07: あずける
	{ 212,  88, 0, 0,	 7,  9,  8,  8 },	// 08: ようすをみる
	{ 212, 112, 0, 0,	 8, 10,  9,  9 },	// 09: マーキング
	{ 212, 136, 0, 0,	 9, 11, 10, 10 },	// 10: にがす
	{ 224, 168, 0, 0,	10,  7, 11, 11 },	// 11: とじる
};
*/
static const CURSORMOVE_CALLBACK PartyOutMainCallBack = {
	PartyOutMainCallBack_On,
	CursorMoveCallBack_Off,
	CursorMoveCallBack_Move,
	CursorMoveCallBack_Touch
};


// あずける・ボックス選択
static const CURSORMOVE_DATA PartyOutBoxSelCursorMoveData[] =
{
	{  43,  83, 0, 0,	0, 8, 5, 1, {  71,  94,  31,  54 } },		// 00: トレイアイコン
	{  77,  83, 0, 0,	1, 8, 0, 2, {  71,  94,  65,  88 } },		// 01: トレイアイコン
	{ 111,  83, 0, 0,	2, 8, 1, 3, {  71,  94,  99, 122 } },		// 02: トレイアイコン
	{ 145,  83, 0, 0,	3, 8, 2, 4, {  71,  94, 133, 156 } },		// 03: トレイアイコン
	{ 179,  83, 0, 0,	4, 8, 3, 5, {  71,  94, 167, 190 } },		// 04: トレイアイコン
	{ 213,  83, 0, 0,	5, 8, 4, 0, {  71,  94, 201, 224 } },		// 05: トレイアイコン

	{  12,  83, 0, 0,	6, 6, 6, 6, {  71,  94,   1,  22 } },		// 06: トレイ矢印（左）
	{ 244,  83, 0, 0,	7, 7, 7, 7, {  71,  94, 233, 254 } },		// 07: トレイ矢印（右）

	{ 212, 112, 0, 0,	0, 9, 8, 8, { 112, 159, 168, 255 } },		// 08:「ポケモンをあずける」
	{ 224, 168, 0, 0,	8, 9, 9, 9, { 168, 191, 192, 255 } },		// 09:「やめる」
};
/*
static const GFL_UI_TP_HITTBL PartyOutBoxSelHitTbl[] =
{
	{  71,  94,  31,  54 },		// 00: トレイアイコン
	{  71,  94,  65,  88 },		// 01: トレイアイコン
	{  71,  94,  99, 122 },		// 02: トレイアイコン
	{  71,  94, 133, 156 },		// 03: トレイアイコン
	{  71,  94, 167, 190 },		// 04: トレイアイコン
	{  71,  94, 201, 224 },		// 05: トレイアイコン

	{  71,  94,   1,  22 },		// 06: トレイ矢印（左）
	{  71,  94, 233, 254 },		// 07: トレイ矢印（右）

	{ 112, 159, 168, 255 },		// 08:「ポケモンをあずける」
	{ 168, 191, 192, 255 },		// 09:「やめる」

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA PartyOutBoxSelKeyTbl[] =
{
	{  43,  83, 0, 0,	0, 8, 5, 1 },		// 00: トレイアイコン
	{  77,  83, 0, 0,	1, 8, 0, 2 },		// 01: トレイアイコン
	{ 111,  83, 0, 0,	2, 8, 1, 3 },		// 02: トレイアイコン
	{ 145,  83, 0, 0,	3, 8, 2, 4 },		// 03: トレイアイコン
	{ 179,  83, 0, 0,	4, 8, 3, 5 },		// 04: トレイアイコン
	{ 213,  83, 0, 0,	5, 8, 4, 0 },		// 05: トレイアイコン

	{  12,  83, 0, 0,	6, 6, 6, 6 },		// 06: トレイ矢印（左）
	{ 244,  83, 0, 0,	7, 7, 7, 7 },		// 07: トレイ矢印（右）

	{ 212, 112, 0, 0,	0, 9, 8, 8 },		// 08:「ポケモンをあずける」
	{ 224, 168, 0, 0,	8, 9, 9, 9 },		// 09:「やめる」
};
*/
static const CURSORMOVE_CALLBACK PartyOutBoxSelCallBack = {
	PartyOutBoxSelCallBack_On,
	CursorMoveCallBack_Off,
	PartyOutBoxSelCallBack_Move,
	PartyOutBoxSelCallBack_Touch,
};

// マーキング
static const CURSORMOVE_DATA MarkingCursorMoveData[] =
{
	{ 196+8,  52+4, 0, 0,	0, 2, 0, 1, {  60,  75, 188, 203 } },		// 00: ●
	{ 228+8,  52+4, 0, 0,	1, 3, 0, 1, {  60,  75, 220, 235 } },		// 01: ▲
	{ 196+8,  76+4, 0, 0,	0, 4, 2, 3, {  84,  99, 188, 203 } },		// 02: ■
	{ 228+8,  76+4, 0, 0,	1, 5, 2, 3, {  84,  99, 220, 235 } },		// 03: ハート
	{ 196+8, 100+4, 0, 0,	2, 6, 4, 5, { 108, 123, 188, 203 } },		// 04: ★
	{ 228+8, 100+4, 0, 0,	3, 6, 4, 5, { 108, 123, 220, 235 } },		// 05: ◆
	{ 212, 130, 0, 0,	CURSORMOVE_RETBIT|4, 7, 6, 6, { 130, 157, 178, 245 } },		// 06:「けってい」
	{ 212, 162, 0, 0,	6, 7, 7, 7, { 162, 189, 178, 245 } },		// 07:「やめる」
};
/*
static const GFL_UI_TP_HITTBL MarkingHitTbl[] =
{
	{  60,  75, 188, 203 },		// 00: ●
	{  60,  75, 220, 235 },		// 01: ▲
	{  84,  99, 188, 203 },		// 02: ■
	{  84,  99, 220, 235 },		// 03: ハート
	{ 108, 123, 188, 203 },		// 04: ★
	{ 108, 123, 220, 235 },		// 05: ◆
	{ 130, 157, 178, 245 },		// 06:「けってい」
	{ 162, 189, 178, 245 },		// 07:「やめる」
	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA MarkingKeyTbl[] =
{
	{ 196+8,  52+4, 0, 0,	0, 2, 0, 1 },		// 00: ●
	{ 228+8,  52+4, 0, 0,	1, 3, 0, 1 },		// 01: ▲
	{ 196+8,  76+4, 0, 0,	0, 4, 2, 3 },		// 02: ■
	{ 228+8,  76+4, 0, 0,	1, 5, 2, 3 },		// 03: ハート
	{ 196+8, 100+4, 0, 0,	2, 6, 4, 5 },		// 04: ★
	{ 228+8, 100+4, 0, 0,	3, 6, 4, 5 },		// 05: ◆
	{ 212, 130, 0, 0,	CURSORMOVE_RETBIT|4, 7, 6, 6 },		// 06:「けってい」
	{ 212, 162, 0, 0,	6, 7, 7, 7 },		// 07:「やめる」
};
*/
static const CURSORMOVE_CALLBACK MarkingCallBack = {
	CursorMoveCallBack_On,
	CursorMoveCallBack_Off,
	CursorMoveCallBack_Move,
	CursorMoveCallBack_Touch
};

// つれていく・メイン
static const CURSORMOVE_DATA PartyInMainCursorMoveData[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 5, 1, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },		// 00: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7, 0, 2, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8, 1, 3, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9, 2, 4, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10, 3, 5, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11, 4, 0, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 11,  7, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 06: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10,  6, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 17, 13, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 12: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, 12, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 23, 19, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 18: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, 18, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, 33, 29, 25, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 24: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, 33, 24, 26, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, 33, 25, 27, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, 33, 26, 28, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, 33, 27, 29, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, 33, 28, 24, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },


	{  84,  16, 0, 0,	33, CURSORMOVE_RETBIT|0, 30, 30, {  17,  39,  26, 141 } },		// 30: ボックス名
	{   0,   0, 0, 0,	31, 31, 31, 31, {  17,  38,   1,  22 } },						// 31: トレイ切り替え矢印・左
	{   0,   0, 0, 0,	32, 32, 32, 32, {  17,  38, 145, 166 } },						// 32: トレイ切り替え矢印・右
	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 33, { 168, 191, 192, 255 } },		// 33: もどる

	{ 212,  64, 0, 0,	38, 35, 34, 34, {  64,  87, 168, 255 } },		// 34: つれていく
	{ 212,  88, 0, 0,	34, 36, 35, 35, {  88, 111, 168, 255 } },		// 35: ようすをみる
	{ 212, 112, 0, 0,	35, 37, 36, 36, { 112, 135, 168, 255 } },		// 36: マーキング
	{ 212, 136, 0, 0,	36, 38, 37, 37, { 136, 159, 168, 255 } },		// 37: にがす
	{ 224, 168, 0, 0,	37, 34, 38, 38, { 168, 191, 192, 255 } },		// 38: とじる
};
/*
static const GFL_UI_TP_HITTBL PartyInMainHitTbl[] =
{
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },		// 00: ポケモン
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

#if	BTS_5462_FIX
	{  17,  39,  26, 141 },		// 30: ボックス名
#else
	{   9,  31,  26, 141 },		// 30: ボックス名
#endif
	{  17,  38,   1,  22 },		// 31: トレイ切り替え矢印・左
	{  17,  38, 145, 166 },		// 32: トレイ切り替え矢印・右
	{ 168, 191, 192, 255 },		// 33: もどる

	{  64,  87, 168, 255 },		// 34: つれていく
	{  88, 111, 168, 255 },		// 35: ようすをみる
	{ 112, 135, 168, 255 },		// 36: マーキング
	{ 136, 159, 168, 255 },		// 37: にがす
	{ 168, 191, 192, 255 },		// 38: とじる

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA PartyInMainKeyTbl[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 5, 1 },		// 00: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7, 0, 2 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8, 1, 3 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9, 2, 4 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10, 3, 5 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11, 4, 0 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 11,  7 },	// 06: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10,  6 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 17, 13 },	// 12: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, 12 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 23, 19 },	// 18: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, 18 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, 33, 29, 25 },	// 24: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, 33, 24, 26 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, 33, 25, 27 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, 33, 26, 28 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, 33, 27, 29 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, 33, 28, 24 },

	{  84,  16, 0, 0,	33, CURSORMOVE_RETBIT|0, 30, 30 },		// 30: ボックス名
	{   0,   0, 0, 0,	31, 31, 31, 31 },						// 31: トレイ切り替え矢印・左
	{   0,   0, 0, 0,	32, 32, 32, 32 },						// 32: トレイ切り替え矢印・右
	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 33 },		// 33: もどる

	{ 212,  64, 0, 0,	38, 35, 34, 34 },	// 34: つれていく
	{ 212,  88, 0, 0,	34, 36, 35, 35 },	// 35: ようすをみる
	{ 212, 112, 0, 0,	35, 37, 36, 36 },	// 36: マーキング
	{ 212, 136, 0, 0,	36, 38, 37, 37 },	// 37: にがす
	{ 224, 168, 0, 0,	37, 34, 38, 38 },	// 38: とじる
};
*/
static const CURSORMOVE_CALLBACK PartyInMainCallBack = {
	PartyInMainCallBack_On,
	CursorMoveCallBack_Off,
	CursorMoveCallBack_Move,
	PartyInMainCallBack_Touch,
};

// ボックステーマ変更
static const CURSORMOVE_DATA BoxThemaChgCursorMoveData[] =
{
	{  43, 23, 0, 0,	0, 8, 5, 1, {  15,  38,  31,  54 } },		// 00: トレイアイコン
	{  77, 23, 0, 0,	1, 8, 0, 2, {  15,  38,  65,  88 } },		// 01: トレイアイコン
	{ 111, 23, 0, 0,	2, 8, 1, 3, {  15,  38,  99, 122 } },		// 02: トレイアイコン
	{ 145, 23, 0, 0,	3, 8, 2, 4, {  15,  38, 133, 156 } },		// 03: トレイアイコン
	{ 179, 23, 0, 0,	4, 8, 3, 5, {  15,  38, 167, 190 } },		// 04: トレイアイコン
	{ 213, 23, 0, 0,	5, 8, 4, 0, {  15,  38, 201, 224 } },		// 05: トレイアイコン

	{ 0, 0, 0, 0,	6, 6, 6, 6, {  15,  38,   1,  22 } },			// 06: トレイ矢印（左）
	{ 0, 0, 0, 0,	7, 7, 7, 7, {  15,  38, 233, 254 } },			// 07: トレイ矢印（右）

	{ 212,  64, 0, 0,	 0,  9,  8,  8, {  64, 111, 168, 255 } },		// 08: ボックスをきりかえる
	{ 212, 112, 0, 0,	 8, 10,  9,  9, { 112, 135, 168, 255 } },		// 09: かべがみ
	{ 212, 136, 0, 0,	 9, 11, 10, 10, { 136, 159, 168, 255 } },		// 10: なまえ
	{ 224, 168, 0, 0,	10, 11, 11, 11, { 168, 191, 192, 255 } },		// 11: やめる
};
/*
static const GFL_UI_TP_HITTBL BoxThemaChgHitTbl[] =
{
	{  15,  38,  31,  54 },		// 00: トレイアイコン
	{  15,  38,  65,  88 },		// 01: トレイアイコン
	{  15,  38,  99, 122 },		// 02: トレイアイコン
	{  15,  38, 133, 156 },		// 03: トレイアイコン
	{  15,  38, 167, 190 },		// 04: トレイアイコン
	{  15,  38, 201, 224 },		// 05: トレイアイコン

	{  15,  38,   1,  22 },		// 06: トレイ矢印（左）
	{  15,  38, 233, 254 },		// 07: トレイ矢印（右）

	{  64, 111, 168, 255 },		// 08: ボックスをきりかえる
	{ 112, 135, 168, 255 },		// 09: かべがみ
	{ 136, 159, 168, 255 },		// 10: なまえ
	{ 168, 191, 192, 255 },		// 11: やめる

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA BoxThemaChgKeyTbl[] =
{
	{  43, 23, 0, 0,	0, 8, 5, 1 },		// 00: トレイアイコン
	{  77, 23, 0, 0,	1, 8, 0, 2 },		// 01: トレイアイコン
	{ 111, 23, 0, 0,	2, 8, 1, 3 },		// 02: トレイアイコン
	{ 145, 23, 0, 0,	3, 8, 2, 4 },		// 03: トレイアイコン
	{ 179, 23, 0, 0,	4, 8, 3, 5 },		// 04: トレイアイコン
	{ 213, 23, 0, 0,	5, 8, 4, 0 },		// 05: トレイアイコン

	{ 0, 0, 0, 0,	6, 6, 6, 6 },			// 06: トレイ矢印（左）
	{ 0, 0, 0, 0,	7, 7, 7, 7 },			// 07: トレイ矢印（右）

	{ 212,  64, 0, 0,	 0,  9,  8,  8 },	// 08: ボックスをきりかえる
	{ 212, 112, 0, 0,	 8, 10,  9,  9 },	// 09: かべがみ
	{ 212, 136, 0, 0,	 9, 11, 10, 10 },	// 10: なまえ
	{ 224, 168, 0, 0,	10, 11, 11, 11 },	// 11: やめる
};
*/
static const CURSORMOVE_CALLBACK BoxThemaChgCallBack = {
	BoxThemaChgCallBack_On,
	CursorMoveCallBack_Off,
	BoxThemaChgCallBack_Move,
	BoxThemaChgCallBack_Touch
};

// 壁紙変更
static const CURSORMOVE_DATA WallPaperChgCursorMoveData[] =
{
	{  59, 31, 0, 0,	0, 6, 3, 1,	{  23,  46,  47,  70 } },	// 00: 壁紙アイコン
	{ 105, 31, 0, 0,	1, 6, 0, 2,	{  23,  46,  93, 116 } },	// 01: 壁紙アイコン
	{ 151, 31, 0, 0,	2, 6, 1, 3,	{  23,  46, 139, 162 } },	// 02: 壁紙アイコン
	{ 197, 31, 0, 0,	3, 6, 2, 0,	{  23,  46, 185, 208 } },	// 03: 壁紙アイコン

	{ 0, 0, 0, 0,	4, 4, 4, 4,	{  23,  46,   2,  23 } },		// 04: トレイ矢印（左）
	{ 0, 0, 0, 0,	5, 5, 5, 5,	{  23,  46, 232, 253 } },		// 05: トレイ矢印（右）

	{ 212, 112, 0, 0,	0, 7, 6, 6,	{ 112, 159, 168, 255 } },	// 06: 変更
	{ 224, 168, 0, 0,	6, 7, 7, 7,	{ 168, 191, 192, 255 } },	// 07: キャンセル
};
/*
static const GFL_UI_TP_HITTBL WallPaperChgHitTbl[] =
{
	{  23,  46,  47,  70 },		// 00: 壁紙アイコン
	{  23,  46,  93, 116 },		// 01: 壁紙アイコン
	{  23,  46, 139, 162 },		// 02: 壁紙アイコン
	{  23,  46, 185, 208 },		// 03: 壁紙アイコン

	{  23,  46,   2,  23 },		// 04: トレイ矢印（左）
	{  23,  46, 232, 253 },		// 05: トレイ矢印（右）

	{ 112, 159, 168, 255 },		// 06: 変更
	{ 168, 191, 192, 255 },		// 07: キャンセル

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA WallPaperChgKeyTbl[] =
{
	{  59, 31, 0, 0,	0, 6, 3, 1 },	// 00: 壁紙アイコン
	{ 105, 31, 0, 0,	1, 6, 0, 2 },	// 01: 壁紙アイコン
	{ 151, 31, 0, 0,	2, 6, 1, 3 },	// 02: 壁紙アイコン
	{ 197, 31, 0, 0,	3, 6, 2, 0 },	// 03: 壁紙アイコン

	{ 0, 0, 0, 0,	4, 4, 4, 4 },		// 04: トレイ矢印（左）
	{ 0, 0, 0, 0,	5, 5, 5, 5 },		// 05: トレイ矢印（右）

	{ 212, 112, 0, 0,	0, 7, 6, 6 },	// 06: 変更
	{ 224, 168, 0, 0,	6, 7, 7, 7 },	// 07: キャンセル
};
*/
static const CURSORMOVE_CALLBACK WallPaperChgCallBack = {
	WallPaperChgCallBack_On,
	CursorMoveCallBack_Off,
	WallPaperChgCallBack_Move,
	WallPaperChgCallBack_Touch
};

// ボックス整理・メイン
static const CURSORMOVE_DATA BoxArrangeMainCursorMoveData[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 5, 1, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },		// 00: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7, 0, 2, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8, 1, 3, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9, 2, 4, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10, 3, 5, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11, 4, 0, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 11,  7, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 06: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10,  6, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 17, 13, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 12: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, 12, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 23, 19, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 18: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, 18, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, CURSORMOVE_RETBIT|33, 29, 25, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 24: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, CURSORMOVE_RETBIT|33, 24, 26, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, CURSORMOVE_RETBIT|33, 25, 27, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, CURSORMOVE_RETBIT|33, 26, 28, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, CURSORMOVE_RETBIT|33, 27, 29, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, CURSORMOVE_RETBIT|33, 28, 24, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{  84,  16, 0, 0,	33, CURSORMOVE_RETBIT|0, 30, 30, {  17,  39,  26, 141 } },		// 30: ボックス名
	{   0,   0, 0, 0,	31, 31, 31, 31, {  17,  38,   1,  22 } },						// 31: トレイ切り替え矢印・左
	{   0,   0, 0, 0,	32, 32, 32, 32, {  17,  38, 145, 166 } },						// 32: トレイ切り替え矢印・右

	{  44, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 34, { 168, 191,   0,  87 } },		// 33: 手持ちポケモン
	{ 140, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 35, { 168, 191,  96, 183 } },		// 34: ポケモン移動
	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 34, 35, { 168, 191, 192, 255 } },		// 35: もどる

	{ 212,  40, 0, 0,	41, 37, 36, 36, {  40,  63, 168, 255 } },	// 36: ポケモンいどう（メニュー）
	{ 212,  64, 0, 0,	36, 38, 37, 37, {  64,  87, 168, 255 } },	// 37: ようすをみる
	{ 212,  88, 0, 0,	37, 39, 38, 38, {  88, 111, 168, 255 } },	// 38: もちもの
	{ 212, 112, 0, 0,	38, 40, 39, 39, { 112, 135, 168, 255 } },	// 39: マーキング
	{ 212, 136, 0, 0,	39, 41, 40, 40, { 136, 159, 168, 255 } },	// 40: にがす
	{ 224, 168, 0, 0,	40, 36, 41, 41, { 168, 191, 192, 255 } },	// 41: とじる
};
/*
static const GFL_UI_TP_HITTBL BoxArrangeMainHitTbl[] =
{
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },		// 00: ポケモン
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{  17,  39,  26, 141 },		// 30: ボックス名
	{  17,  38,   1,  22 },		// 31: トレイ切り替え矢印・左
	{  17,  38, 145, 166 },		// 32: トレイ切り替え矢印・右
	{ 168, 191,   0,  87 },		// 33: 手持ちポケモン
	{ 168, 191,  96, 183 },		// 34: ポケモン移動
	{ 168, 191, 192, 255 },		// 35: もどる

	{  40,  63, 168, 255 },		// 36: ポケモンいどう（メニュー）
	{  64,  87, 168, 255 },		// 37: ようすをみる
	{  88, 111, 168, 255 },		// 38: もちもの
	{ 112, 135, 168, 255 },		// 39: マーキング
	{ 136, 159, 168, 255 },		// 40: にがす
	{ 168, 191, 192, 255 },		// 41: とじる

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA BoxArrangeMainKeyTbl[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 5, 1 },		// 00: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7, 0, 2 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8, 1, 3 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9, 2, 4 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10, 3, 5 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11, 4, 0 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 11,  7 },	// 06: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10,  6 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 17, 13 },	// 12: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, 12 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 23, 19 },	// 18: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, 18 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, CURSORMOVE_RETBIT|33, 29, 25 },	// 24: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, CURSORMOVE_RETBIT|33, 24, 26 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, CURSORMOVE_RETBIT|33, 25, 27 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, CURSORMOVE_RETBIT|33, 26, 28 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, CURSORMOVE_RETBIT|33, 27, 29 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, CURSORMOVE_RETBIT|33, 28, 24 },

	{  84,  16, 0, 0,	33, CURSORMOVE_RETBIT|0, 30, 30 },		// 30: ボックス名
	{   0,   0, 0, 0,	31, 31, 31, 31 },						// 31: トレイ切り替え矢印・左
	{   0,   0, 0, 0,	32, 32, 32, 32 },						// 32: トレイ切り替え矢印・右

	{  44, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 34 },		// 33: 手持ちポケモン
	{ 140, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 35 },		// 34: ポケモン移動
	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 34, 35 },		// 35: もどる

	{ 212,  40, 0, 0,	41, 37, 36, 36 },	// 36: ポケモンいどう（メニュー）
	{ 212,  64, 0, 0,	36, 38, 37, 37 },	// 37: ようすをみる
	{ 212,  88, 0, 0,	37, 39, 38, 38 },	// 38: もちもの
	{ 212, 112, 0, 0,	38, 40, 39, 39 },	// 39: マーキング
	{ 212, 136, 0, 0,	39, 41, 40, 40 },	// 40: にがす
	{ 224, 168, 0, 0,	40, 36, 41, 41 },	// 41: とじる
};
*/
static const CURSORMOVE_CALLBACK BoxArrangeMainCallBack = {
	BoxArrangeMainCallBack_On,
	CursorMoveCallBack_Off,
	BoxArrangeMainCallBack_Move,
	BoxArrangeMainCallBack_Touch
};

// ボックス整理・ポケモン移動
static const CURSORMOVE_DATA BoxArrangePokeMoveCursorMoveData[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	37,  6, 31, 1, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 00: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	37,  7,  0, 2, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	37,  8,  1, 3, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	37,  9,  2, 4, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	37, 10,  3, 5, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	37, 11,  4, CURSORMOVE_RETBIT|30, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 31,  7, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 06: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10, CURSORMOVE_RETBIT|30, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 33, 13, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 12: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, CURSORMOVE_RETBIT|32, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 33, 19, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 18: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, CURSORMOVE_RETBIT|34, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, 37, 35, 25, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 24: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, 37, 24, 26, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, 37, 25, 27, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, 37, 26, 28, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, 37, 27, 29, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, 37, 28, CURSORMOVE_RETBIT|34, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ 192,  52, 0, 0,	37, 32, CURSORMOVE_RETBIT|5, 31, {  68-12,  68+12-1, 194-12, 194+12-1 } },		// 30: 手持ちポケモン
	{ 232,  60, 0, 0,	37, 33, 30, CURSORMOVE_RETBIT|0, {  76-12,  76+12-1, 230-12, 230+12-1 } },
	{ 192,  84, 0, 0,	30, 34, CURSORMOVE_RETBIT|17, 33, { 100-12, 100+12-1, 194-12, 194+12-1 } },
	{ 232,  92, 0, 0,	31, 35, 32, CURSORMOVE_RETBIT|12, { 108-12, 108+12-1, 230-12, 230+12-1 } },
	{ 192, 116, 0, 0,	32, 36, CURSORMOVE_RETBIT|23, 35, { 132-12, 132+12-1, 194-12, 194+12-1 } },
	{ 232, 124, 0, 0,	33, 36, 34, CURSORMOVE_RETBIT|24, { 140-12, 140+12-1, 230-12, 230+12-1 } },

	{ 212, 162, 0, 0,	CURSORMOVE_RETBIT|35, 37, 36, 36, { 162, 189, 178, 245 } },		// 36: やめる

	{  43, 23, 0, 0,	24, CURSORMOVE_RETBIT|0, 42, 38, {  15,  38,  31,  54 } },		// 37: トレイアイコン
	{  77, 23, 0, 0,	24, CURSORMOVE_RETBIT|0, 37, 39, {  15,  38,  65,  88 } },		// 38: トレイアイコン
	{ 111, 23, 0, 0,	24, CURSORMOVE_RETBIT|0, 38, 40, {  15,  38,  99, 122 } },		// 39: トレイアイコン
	{ 145, 23, 0, 0,	24, CURSORMOVE_RETBIT|0, 39, 41, {  15,  38, 133, 156 } },		// 40: トレイアイコン
	{ 179, 23, 0, 0,	24, CURSORMOVE_RETBIT|0, 40, 42, {  15,  38, 167, 190 } },		// 41: トレイアイコン
	{ 213, 23, 0, 0,	24, CURSORMOVE_RETBIT|0, 41, 37, {  15,  38, 201, 224 } },		// 42: トレイアイコン

	{ 0, 0, 0, 0,	43, 43, 43, 43, {  15,  38,   1,  22 } },		// 43: トレイ矢印（左）
	{ 0, 0, 0, 0,	44, 44, 44, 44, {  15,  38, 233, 254 } },		// 44: トレイ矢印（右）

	{ 68, 168, 0, 0,	45, 45, 45, 45, { 168, 191,   0, 135 } },	// 45: ボックスをきりかえる
};
/*
static const GFL_UI_TP_HITTBL BoxArrangePokeMoveHitTbl[] =
{
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },		// 00: ポケモン
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{  68-12,  68+12-1, 194-12, 194+12-1 },		// 30: 手持ちポケモン
	{  76-12,  76+12-1, 230-12, 230+12-1 },
	{ 100-12, 100+12-1, 194-12, 194+12-1 },
	{ 108-12, 108+12-1, 230-12, 230+12-1 },
	{ 132-12, 132+12-1, 194-12, 194+12-1 },
	{ 140-12, 140+12-1, 230-12, 230+12-1 },

	{ 162, 189, 178, 245 },		// 36: やめる

	{  15,  38,  31,  54 },		// 37: トレイアイコン
	{  15,  38,  65,  88 },		// 38: トレイアイコン
	{  15,  38,  99, 122 },		// 39: トレイアイコン
	{  15,  38, 133, 156 },		// 40: トレイアイコン
	{  15,  38, 167, 190 },		// 41: トレイアイコン
	{  15,  38, 201, 224 },		// 42: トレイアイコン

	{  15,  38,   1,  22 },		// 43: トレイ矢印（左）
	{  15,  38, 233, 254 },		// 44: トレイ矢印（右）

	{ 168, 191,   0, 135 },		// 45: ボックスをきりかえる

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA BoxArrangePokeMoveKeyTbl[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	37,  6, 31, 1 },	// 00: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	37,  7,  0, 2 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	37,  8,  1, 3 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	37,  9,  2, 4 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	37, 10,  3, 5 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	37, 11,  4, CURSORMOVE_RETBIT|30 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 31,  7 },	// 06: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10, CURSORMOVE_RETBIT|30 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 33, 13 },	// 12: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, CURSORMOVE_RETBIT|32 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 33, 19 },	// 18: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, CURSORMOVE_RETBIT|34 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, 37, 35, 25 },	// 24: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, 37, 24, 26 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, 37, 25, 27 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, 37, 26, 28 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, 37, 27, 29 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, 37, 28, CURSORMOVE_RETBIT|34 },

	{ 192,  52, 0, 0,	37, 32, CURSORMOVE_RETBIT|5, 31 },		// 30: 手持ちポケモン
	{ 232,  60, 0, 0,	37, 33, 30, CURSORMOVE_RETBIT|0 },
	{ 192,  84, 0, 0,	30, 34, CURSORMOVE_RETBIT|17, 33 },
	{ 232,  92, 0, 0,	31, 35, 32, CURSORMOVE_RETBIT|12 },
	{ 192, 116, 0, 0,	32, 36, CURSORMOVE_RETBIT|23, 35 },
	{ 232, 124, 0, 0,	33, 36, 34, CURSORMOVE_RETBIT|24 },

	{ 212, 162, 0, 0,	CURSORMOVE_RETBIT|35, 37, 36, 36 },		// 36: やめる

	{  43, 23, 0, 0,	24, CURSORMOVE_RETBIT|0, 42, 38 },		// 37: トレイアイコン
	{  77, 23, 0, 0,	24, CURSORMOVE_RETBIT|0, 37, 39 },		// 38: トレイアイコン
	{ 111, 23, 0, 0,	24, CURSORMOVE_RETBIT|0, 38, 40 },		// 39: トレイアイコン
	{ 145, 23, 0, 0,	24, CURSORMOVE_RETBIT|0, 39, 41 },		// 40: トレイアイコン
	{ 179, 23, 0, 0,	24, CURSORMOVE_RETBIT|0, 40, 42 },		// 41: トレイアイコン
	{ 213, 23, 0, 0,	24, CURSORMOVE_RETBIT|0, 41, 37 },		// 42: トレイアイコン

	{ 0, 0, 0, 0,	43, 43, 43, 43 },		// 43: トレイ矢印（左）
	{ 0, 0, 0, 0,	44, 44, 44, 44 },		// 44: トレイ矢印（右）

	{ 68, 168, 0, 0,	45, 45, 45, 45 },	// 45: ボックスをきりかえる
};
*/
static const CURSORMOVE_CALLBACK BoxArrangePokeMoveCallBack = {
	BoxArrangePokeMoveCallBack_On,
	CursorMoveCallBack_Off,
	BoxArrangePokeMoveCallBack_Move,
	BoxArrangePokeMoveCallBack_Touch
};

// ボックス整理・手持ちポケモン
static const CURSORMOVE_DATA BoxArrangePartyPokeCursorMoveData[] =
{
	{ 40,  52, 0, 0,	7, 2, 0, 1, {  68-12,  68+12-1, 42-12, 42+12-1 } },	// 00: ポケモン１
	{ 80,  60, 0, 0,	7, 3, 0, 2, {  76-12,  76+12-1, 78-12, 78+12-1 } },	// 01: ポケモン２
	{ 40,  84, 0, 0,	0, 4, 1, 3, { 100-12, 100+12-1, 42-12, 42+12-1 } },	// 02: ポケモン３
	{ 80,  92, 0, 0,	1, 5, 2, 4, { 108-12, 108+12-1, 78-12, 78+12-1 } },	// 03: ポケモン４
	{ 40, 116, 0, 0,	2, 6, 3, 5, { 132-12, 132+12-1, 42-12, 42+12-1 } },	// 04: ポケモン５
	{ 80, 124, 0, 0,	3, 6, 4, 6, { 140-12, 140+12-1, 78-12, 78+12-1 } },	// 05: ポケモン６

	{  60, 162, 0, 0,	CURSORMOVE_RETBIT|5, 7, 5, 7, { 162, 189,  26,  93 } },	// 06: いれかえ

	{ 224, 168, 0, 0,	 6,  0,  6,  7, { 168, 191, 192, 255 } },	// 07: もどる

	{ 212,  40, 0, 0,	13,  9,  8,  8, {  40,  63, 168, 255 } },	// 08: ポケモンいどう（メニュー）
	{ 212,  64, 0, 0,	 8, 10,  9,  9, {  64,  87, 168, 255 } },	// 09: ようすをみる
	{ 212,  88, 0, 0,	 9, 11, 10, 10, {  88, 111, 168, 255 } },	// 10: もちもの
	{ 212, 112, 0, 0,	10, 12, 11, 11, { 112, 135, 168, 255 } },	// 11: マーキング
	{ 212, 136, 0, 0,	11, 13, 12, 12, { 136, 159, 168, 255 } },	// 12: にがす
	{ 224, 168, 0, 0,	12,  8, 13, 13, { 168, 191, 192, 255 } },	// 13: とじる
};
/*
static const GFL_UI_TP_HITTBL BoxArrangePartyPokeHitTbl[] =
{
	{  68-12,  68+12-1, 42-12, 42+12-1 },	// 00: ポケモン１
	{  76-12,  76+12-1, 78-12, 78+12-1 },	// 01: ポケモン２
	{ 100-12, 100+12-1, 42-12, 42+12-1 },	// 02: ポケモン３
	{ 108-12, 108+12-1, 78-12, 78+12-1 },	// 03: ポケモン４
	{ 132-12, 132+12-1, 42-12, 42+12-1 },	// 04: ポケモン５
	{ 140-12, 140+12-1, 78-12, 78+12-1 },	// 05: ポケモン６

	{ 162, 189,  26,  93 },		// 06: いれかえ

	{ 168, 191, 192, 255 },		// 07: もどる

	{  40,  63, 168, 255 },		// 08: ポケモンいどう（メニュー）
	{  64,  87, 168, 255 },		// 09: ようすをみる
	{  88, 111, 168, 255 },		// 10: もちもの
	{ 112, 135, 168, 255 },		// 11: マーキング
	{ 136, 159, 168, 255 },		// 12: にがす
	{ 168, 191, 192, 255 },		// 13: とじる

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA BoxArrangePartyPokeKeyTbl[] =
{
	{ 40,  52, 0, 0,	7, 2, 0, 1 },	// 00: ポケモン１
	{ 80,  60, 0, 0,	7, 3, 0, 2 },	// 01: ポケモン２
	{ 40,  84, 0, 0,	0, 4, 1, 3 },	// 02: ポケモン３
	{ 80,  92, 0, 0,	1, 5, 2, 4 },	// 03: ポケモン４
	{ 40, 116, 0, 0,	2, 6, 3, 5 },	// 04: ポケモン５
	{ 80, 124, 0, 0,	3, 6, 4, 6 },	// 05: ポケモン６

	{  60, 162, 0, 0,	CURSORMOVE_RETBIT|5, 7, 5, 7 },	// 06: いれかえ

	{ 224, 168, 0, 0,	 6,  0,  6,  7 },	// 07: もどる

	{ 212,  40, 0, 0,	13,  9,  8,  8 },	// 08: ポケモンいどう（メニュー）
	{ 212,  64, 0, 0,	 8, 10,  9,  9 },	// 09: ようすをみる
	{ 212,  88, 0, 0,	 9, 11, 10, 10 },	// 10: もちもの
	{ 212, 112, 0, 0,	10, 12, 11, 11 },	// 11: マーキング
	{ 212, 136, 0, 0,	11, 13, 12, 12 },	// 12: にがす
	{ 224, 168, 0, 0,	12,  8, 13, 13 },	// 13: とじる
};
*/
static const CURSORMOVE_CALLBACK BoxArrangePartyPokeCallBack = {
	BoxArrangePartyMoveCallBack_On,
	CursorMoveCallBack_Off,
	BoxArrangePartyMoveCallBack_Move,
	CursorMoveCallBack_Touch
};

// 持ち物整理・メイン
static const CURSORMOVE_DATA BoxItemArrangeMainCursorMoveData[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 5, 1, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },		// 00: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7, 0, 2, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8, 1, 3, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9, 2, 4, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10, 3, 5, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11, 4, 0, { TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 11,  7, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 06: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10,  6, { TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 17, 13, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 12: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, 12, { TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 23, 19, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 18: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, 18, { TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, CURSORMOVE_RETBIT|33, 29, 25, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 } },	// 24: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, CURSORMOVE_RETBIT|33, 24, 26, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 } },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, CURSORMOVE_RETBIT|33, 25, 27, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 } },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, CURSORMOVE_RETBIT|33, 26, 28, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 } },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, CURSORMOVE_RETBIT|33, 27, 29, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 } },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, CURSORMOVE_RETBIT|33, 28, 24, { TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 } },

	{  84,  16, 0, 0,	33, CURSORMOVE_RETBIT|0, 30, 30, {  17,  39,  26, 141 } },		// 30: ボックス名
	{   0,   0, 0, 0,	31, 31, 31, 31, {  17,  38,   1,  22 } },						// 31: トレイ切り替え矢印・左
	{   0,   0, 0, 0,	32, 32, 32, 32, {  17,  38, 145, 166 } },						// 32: トレイ切り替え矢印・右
	{  44, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 34, { 168, 191,   0,  87 } },		// 33: 手持ちポケモン
	{ 140, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 35, { 168, 191,  96, 183 } },		// 34: 持ち物整理
	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 34, 35, { 168, 191, 192, 255 } },		// 35: もどる

	{ 212, 136, 0, 0,	37, 37, 36, 36, { 136, 159, 168, 255 } },	// 36: バッグへ or もたせる
	{ 224, 168, 0, 0,	36, 36, 37, 37, { 168, 191, 192, 255 } },	// 37: とじる
};
/*
static const GFL_UI_TP_HITTBL BoxItemArrangeMainHitTbl[] =
{
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },		// 00: ポケモン
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(0), TRAYPOKE_PY(1)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(1), TRAYPOKE_PY(2)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(2), TRAYPOKE_PY(3)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(3), TRAYPOKE_PY(4)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(0),	TRAYPOKE_PX(1)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(1),	TRAYPOKE_PX(2)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(2),	TRAYPOKE_PX(3)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(3),	TRAYPOKE_PX(4)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(4),	TRAYPOKE_PX(5)-1 },
	{ TRAYPOKE_PY(4), TRAYPOKE_PY(5)-1, TRAYPOKE_PX(5),	TRAYPOKE_PX(6)-1 },

	{  17,  39,  26, 141 },		// 30: ボックス名
	{  17,  38,   1,  22 },		// 31: トレイ切り替え矢印・左
	{  17,  38, 145, 166 },		// 32: トレイ切り替え矢印・右
	{ 168, 191,   0,  87 },		// 33: 手持ちポケモン
	{ 168, 191,  96, 183 },		// 34: 持ち物整理
	{ 168, 191, 192, 255 },		// 35: もどる

	{ 136, 159, 168, 255 },		// 36: バッグへ or もたせる
	{ 168, 191, 192, 255 },		// 37: とじる

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA BoxItemArrangeMainKeyTbl[] =
{
	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  6, 5, 1 },		// 00: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  7, 0, 2 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  8, 1, 3 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30,  9, 2, 4 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 10, 3, 5 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(0)+PICUR_Y, 0, 0,	30, 11, 4, 0 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	0, 12, 11,  7 },	// 06: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	1, 13,  6,  8 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	2, 14,  7,  9 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	3, 15,  8, 10 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	4, 16,  9, 11 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(1)+PICUR_Y, 0, 0,	5, 17, 10,  6 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 6, 18, 17, 13 },	// 12: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 7, 19, 12, 14 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 8, 20, 13, 15 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	 9, 21, 14, 16 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	10, 22, 15, 17 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(2)+PICUR_Y, 0, 0,	11, 23, 16, 12 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	12, 24, 23, 19 },	// 18: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	13, 25, 18, 20 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	14, 26, 19, 21 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	15, 27, 20, 22 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	16, 28, 21, 23 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(3)+PICUR_Y, 0, 0,	17, 29, 22, 18 },

	{ TRAYPOKE_PX(0)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	18, CURSORMOVE_RETBIT|33, 29, 25 },	// 24: ポケモン
	{ TRAYPOKE_PX(1)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	19, CURSORMOVE_RETBIT|33, 24, 26 },
	{ TRAYPOKE_PX(2)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	20, CURSORMOVE_RETBIT|33, 25, 27 },
	{ TRAYPOKE_PX(3)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	21, CURSORMOVE_RETBIT|33, 26, 28 },
	{ TRAYPOKE_PX(4)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	22, CURSORMOVE_RETBIT|33, 27, 29 },
	{ TRAYPOKE_PX(5)+PICUR_X,TRAYPOKE_PY(4)+PICUR_Y, 0, 0,	23, CURSORMOVE_RETBIT|33, 28, 24 },

	{  84,  16, 0, 0,	33, CURSORMOVE_RETBIT|0, 30, 30 },		// 30: ボックス名
	{   0,   0, 0, 0,	31, 31, 31, 31 },						// 31: トレイ切り替え矢印・左
	{   0,   0, 0, 0,	32, 32, 32, 32 },						// 32: トレイ切り替え矢印・右
	{  44, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 34 },		// 33: 手持ちポケモン
	{ 140, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 33, 35 },		// 34: 持ち物整理
	{ 224, 168, 0, 0,	CURSORMOVE_RETBIT|24, 30, 34, 35 },		// 35: もどる

	{ 212, 136, 0, 0,	37, 37, 36, 36 },	// 36: バッグへ or もたせる
	{ 224, 168, 0, 0,	36, 36, 37, 37 },	// 37: とじる
};
*/
static const CURSORMOVE_CALLBACK BoxItemArrangeMainCallBack = {
	BoxItemArrangeMainCallBack_On,
	CursorMoveCallBack_Off,
	BoxItemArrangeMainCallBack_Move,
	BoxItemArrangeMainCallBack_Touch
};

// 持ち物整理・手持ちポケモン
static const CURSORMOVE_DATA BoxItemArrangePartyPokeCursorMoveData[] =
{
	{ 40,  52, 0, 0,	4, 2, 0, 1, {  68-12,  68+12-1, 42-12, 42+12-1 } },	// 00: ポケモン１
	{ 80,  60, 0, 0,	5, 3, 0, 2, {  76-12,  76+12-1, 78-12, 78+12-1 } },	// 01: ポケモン２
	{ 40,  84, 0, 0,	0, 4, 1, 3, { 100-12, 100+12-1, 42-12, 42+12-1 } },	// 02: ポケモン３
	{ 80,  92, 0, 0,	1, 5, 2, 4, { 108-12, 108+12-1, 78-12, 78+12-1 } },	// 03: ポケモン４
	{ 40, 116, 0, 0,	2, 6, 3, 5, { 132-12, 132+12-1, 42-12, 42+12-1 } },	// 04: ポケモン５
	{ 80, 124, 0, 0,	3, 6, 4, 6, { 140-12, 140+12-1, 78-12, 78+12-1 } },	// 05: ポケモン６

	{  60, 162, 0, 0,	CURSORMOVE_RETBIT|5, 7, 5, 7, { 162, 189,  26,  93 } },	// 06: いれかえ

	{ 224, 168, 0, 0,	6, 7, 6, 7, { 168, 191, 192, 255 } },	// 07: もどる

	{ 212, 136, 0, 0,	9, 9, 8, 8, { 136, 159, 168, 255 } },	// 08: バッグへ or  もたせる
	{ 224, 168, 0, 0,	8, 8, 9, 9, { 168, 191, 192, 255 } },	// 09: とじる
};
/*
static const GFL_UI_TP_HITTBL BoxItemArrangePartyPokeHitTbl[] =
{
	{  68-12,  68+12-1, 42-12, 42+12-1 },	// 00: ポケモン１
	{  76-12,  76+12-1, 78-12, 78+12-1 },	// 01: ポケモン２
	{ 100-12, 100+12-1, 42-12, 42+12-1 },	// 02: ポケモン３
	{ 108-12, 108+12-1, 78-12, 78+12-1 },	// 03: ポケモン４
	{ 132-12, 132+12-1, 42-12, 42+12-1 },	// 04: ポケモン５
	{ 140-12, 140+12-1, 78-12, 78+12-1 },	// 05: ポケモン６

	{ 162, 189,  26,  93 },		// 06: いれかえ

	{ 168, 191, 192, 255 },		// 07: もどる

	{ 136, 159, 168, 255 },		// 08: バッグへ or  もたせる
	{ 168, 191, 192, 255 },		// 09: とじる

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};
static const CURSORMOVE_DATA BoxItemArrangePartyPokeKeyTbl[] =
{
	{ 40,  52, 0, 0,	4, 2, 0, 1 },	// 00: ポケモン１
	{ 80,  60, 0, 0,	5, 3, 0, 2 },	// 01: ポケモン２
	{ 40,  84, 0, 0,	0, 4, 1, 3 },	// 02: ポケモン３
	{ 80,  92, 0, 0,	1, 5, 2, 4 },	// 03: ポケモン４
	{ 40, 116, 0, 0,	2, 6, 3, 5 },	// 04: ポケモン５
	{ 80, 124, 0, 0,	3, 6, 4, 6 },	// 05: ポケモン６

	{  60, 162, 0, 0,	CURSORMOVE_RETBIT|5, 7, 5, 7 },	// 06: いれかえ

	{ 224, 168, 0, 0,	6, 7, 6, 7 },	// 07: もどる

	{ 212, 136, 0, 0,	9, 9, 8, 8 },	// 08: バッグへ or  もたせる
	{ 224, 168, 0, 0,	8, 8, 9, 9 },	// 09: とじる
};
*/
static const CURSORMOVE_CALLBACK BoxItemArrangePartyPokeCallBack = {
	BoxItemArrangePartyMoveCallBack_On,
	CursorMoveCallBack_Off,
	BoxItemArrangePartyMoveCallBack_Move,
	CursorMoveCallBack_Touch
};

// Ｙステータスボタンチェック
static const GFL_UI_TP_HITTBL YStatusHitTbl[] =
{
	{ 168, 191, 0, 135 },
	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
};

// タッチチェックの終了データ
static const GFL_UI_TP_HITTBL TpCheckEnd = { GFL_UI_TP_HIT_END, 0, 0, 0 };

// カーソル移動データテーブル
static const MOVE_DATA_TBL MoveDataTable[] =
{
	{	// あずける・メイン
		PartyOutMainCursorMoveData,
		&PartyOutMainCallBack
	},
	{	// あずける・ボックス選択
		PartyOutBoxSelCursorMoveData,
		&PartyOutBoxSelCallBack
	},

	{	// つれていく・メイン
		PartyInMainCursorMoveData,
		&PartyInMainCallBack
	},

	{	// ボックス整理・メイン
		BoxArrangeMainCursorMoveData,
		&BoxArrangeMainCallBack
	},
	{	// ボックス整理・ポケモン移動
		BoxArrangePokeMoveCursorMoveData,
		&BoxArrangePokeMoveCallBack
	},
	{	// ボックス整理・手持ちポケモン
		BoxArrangePartyPokeCursorMoveData,
		&BoxArrangePartyPokeCallBack
	},

	{	// 持ち物整理・メイン
		BoxItemArrangeMainCursorMoveData,
		&BoxItemArrangeMainCallBack
	},
	{	// 持ち物整理・手持ちポケモン
		BoxItemArrangePartyPokeCursorMoveData,
		&BoxItemArrangePartyPokeCallBack
	},

	{	// マーキング
		MarkingCursorMoveData,
		&MarkingCallBack
	},
	{	// ボックステーマ変更
		BoxThemaChgCursorMoveData,
		&BoxThemaChgCallBack
	},
	{	// 壁紙変更
		WallPaperChgCursorMoveData,
		&WallPaperChgCallBack
	},
};



//--------------------------------------------------------------------------------------------
/**
 * カーソル移動ワーク初期化
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2UI_CursorMoveInit( BOX2_SYS_WORK * syswk )
{
	u32		id;

	switch( syswk->dat->callMode ){
	case BOX_MODE_AZUKERU:
		id = BOX2UI_INIT_ID_PTOUT_MAIN;
		break;
	case BOX_MODE_TURETEIKU:
		id = BOX2UI_INIT_ID_PTIN_MAIN;
		break;
	case BOX_MODE_SEIRI:
		id = BOX2UI_INIT_ID_ARRANGE_MAIN;
		break;
	case BOX_MODE_ITEM:
		id = BOX2UI_INIT_ID_ITEM_A_MAIN;
		break;
	}

	BOX2UI_CursorMoveExit( syswk );

	syswk->app->cmwk = CURSORMOVE_Create(
							MoveDataTable[id].dat,
							MoveDataTable[id].cb,
							syswk,
							TRUE,
							syswk->cur_rcv_pos,			// pos
							HEAPID_BOX_APP );

	CursorObjMove( syswk, syswk->cur_rcv_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * カーソル移動ワーク切り替え
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	id		カーソル移動データ番号
 * @param	pos		カーソル初期位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2UI_CursorMoveChange( BOX2_SYS_WORK * syswk, u32 id, u32 pos )
{
	BOX2UI_CursorMoveExit( syswk );

	syswk->app->cmwk = CURSORMOVE_Create(
							MoveDataTable[id].dat,
							MoveDataTable[id].cb,
							syswk,
							TRUE,
							pos,					// pos
							HEAPID_BOX_APP );

	CursorObjMove( syswk, pos );
}

//--------------------------------------------------------------------------------------------
/**
 * カーソル移動ワーク削除
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2UI_CursorMoveExit( BOX2_SYS_WORK * syswk )
{
	if( syswk->app->cmwk != NULL ){
		CURSORMOVE_Exit( syswk->app->cmwk );
		syswk->app->cmwk = NULL;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * カーソルＯＢＪ移動
 *
 * @param	syswk	ボックス画面システムワーク
 * @param	pos		位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CursorObjMove( BOX2_SYS_WORK * syswk, int pos )
{
	const CURSORMOVE_DATA * pw;
	u8	x, y;

	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, pos );

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );
}

//--------------------------------------------------------------------------------------------
/**
 * カーソル移動汎用コールバック：カーソル表示時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * カーソル移動汎用コールバック：カーソル非表示時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_Off( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, FALSE );
}

//--------------------------------------------------------------------------------------------
/**
 * カーソル移動汎用コールバック：カーソル移動時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * カーソル移動汎用コールバック：タッチ時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void CursorMoveCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンをあずける」メイン操作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	動作結果
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_PartyOutMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( ret < NELEMS(PartyOutMainCursorMoveData) ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( ret >= BOX2UI_PTOUT_MAIN_AZUKERU && ret <= BOX2UI_PTOUT_MAIN_CLOSE ){
				CURSORMOVE_PosSet( syswk->app->cmwk, now );
				CursorObjMove( syswk, now );
				return CURSORMOVE_NONE;
			}
		}else{
			if( ret == BOX2UI_PTOUT_MAIN_RETURN ){
				return BOX2UI_PTOUT_MAIN_CLOSE;
			}
		}
		return ret;
	}

	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
		if( ret == BOX2UI_PTOUT_MAIN_RETURN || ret == CURSORMOVE_CANCEL ){
			return BOX2UI_PTOUT_MAIN_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンをあずける」メイン操作コールバック関数：カーソル表示時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyOutMainCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( syswk->get_pos != BOX2MAIN_GETPOS_NONE &&
				syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
				now_pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
			}else{
				now_pos = BOX2UI_PTOUT_MAIN_POKE1;
			}
		}else{
			if( now_pos != BOX2UI_PTOUT_MAIN_STATUS &&
				now_pos != BOX2UI_PTOUT_MAIN_MARKING &&
				now_pos != BOX2UI_PTOUT_MAIN_FREE ){
				now_pos = BOX2UI_PTOUT_MAIN_AZUKERU;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンをあずける」預けるトレイ選択コールバック関数：カーソル表示時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyOutBoxSelCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	u8	x, y;

	syswk = work;

	if( now_pos == BOX2UI_PTOUT_BOXSEL_LEFT || now_pos == BOX2UI_PTOUT_BOXSEL_RIGHT ){
		now_pos = syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;
		syswk->app->old_cur_pos = now_pos;
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, now_pos );

	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );
	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	if( now_pos >= BOX2UI_PTOUT_BOXSEL_TRAY1 && now_pos <= BOX2UI_PTOUT_BOXSEL_TRAY6 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンをあずける」預けるトレイ選択コールバック関数：カーソル移動時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyOutBoxSelCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	if( now_pos >= BOX2UI_PTOUT_BOXSEL_TRAY1 && now_pos <= BOX2UI_PTOUT_BOXSEL_TRAY6 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}

	//「あずける」からトレイへの移動
	if( old_pos == BOX2UI_PTOUT_BOXSEL_ENTER && now_pos == BOX2UI_PTOUT_BOXSEL_TRAY1 ){
		now_pos = syswk->app->old_cur_pos;
		CURSORMOVE_PosSetEx( syswk->app->cmwk, now_pos, BOX2UI_PTOUT_BOXSEL_ENTER, BOX2UI_PTOUT_BOXSEL_ENTER );
	}

	// 前回位置更新
	if( now_pos >= BOX2UI_PTOUT_BOXSEL_TRAY1 && now_pos <= BOX2UI_PTOUT_BOXSEL_TRAY6 ){
		syswk->app->old_cur_pos = now_pos;
	}

	// トレイ左から右へのループ
	if( now_pos == BOX2UI_PTOUT_BOXSEL_TRAY1 && old_pos == BOX2UI_PTOUT_BOXSEL_TRAY6 ){
		if( syswk->box_mv_pos + 1 >= syswk->trayMax ){
			syswk->box_mv_pos = 0;
		}else{
			syswk->box_mv_pos++;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, BOX2OBJ_ANM_R_ARROW_ON );
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	// トレイ右から左へのループ
	if( now_pos == BOX2UI_PTOUT_BOXSEL_TRAY6 && old_pos == BOX2UI_PTOUT_BOXSEL_TRAY1 ){
		if( syswk->box_mv_pos - 1 < 0 ){
			syswk->box_mv_pos = syswk->trayMax - 1;
		}else{
			syswk->box_mv_pos--;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, BOX2OBJ_ANM_L_ARROW_ON );
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	if( now_pos >= BOX2UI_PTOUT_BOXSEL_TRAY1 && now_pos <= BOX2UI_PTOUT_BOXSEL_TRAY6 ){
		if( old_pos != BOX2UI_PTOUT_BOXSEL_ENTER ){
			syswk->box_mv_pos = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + now_pos;
			BOX2OBJ_BoxMoveNameSet( syswk );
			BOX2BMP_BoxMoveNameNumWrite( syswk );
		}
	}

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンをあずける」預けるトレイ選択コールバック関数：タッチ時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyOutBoxSelCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	if( now_pos != BOX2UI_PTOUT_BOXSEL_LEFT && now_pos != BOX2UI_PTOUT_BOXSEL_RIGHT ){
		CursorObjMove( syswk, now_pos );
	}
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンをつれていく」メイン操作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	動作結果
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_PartyInMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( ret < NELEMS(PartyInMainCursorMoveData) ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( ret >= BOX2UI_PTIN_MAIN_TSURETEIKU && ret <= BOX2UI_PTIN_MAIN_CLOSE ){
				CURSORMOVE_PosSet( syswk->app->cmwk, now );
				CursorObjMove( syswk, now );
				return CURSORMOVE_NONE;
			}
		}else{
			if( ret == BOX2UI_PTIN_MAIN_RETURN ){
				return BOX2UI_PTIN_MAIN_CLOSE;
			}
		}
		return ret;
	}

	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
		if( ret == BOX2UI_PTIN_MAIN_RETURN || ret == CURSORMOVE_CANCEL ){
			return BOX2UI_PTIN_MAIN_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンをつれていく」メイン操作コールバック関数：カーソル表示時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyInMainCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( now_pos == BOX2UI_PTIN_MAIN_LEFT || now_pos == BOX2UI_PTIN_MAIN_RIGHT ){
				now_pos = BOX2UI_PTIN_MAIN_NAME;
			}else if( now_pos != BOX2UI_PTIN_MAIN_NAME && now_pos != BOX2UI_PTIN_MAIN_RETURN ){
				if( syswk->get_pos != BOX2MAIN_GETPOS_NONE &&
					syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
					now_pos = syswk->get_pos;
				}else{
					now_pos = BOX2UI_PTIN_MAIN_POKE1;
				}
			}
		}else{
			if( now_pos != BOX2UI_PTIN_MAIN_STATUS &&
				now_pos != BOX2UI_PTIN_MAIN_MARKING &&
				now_pos != BOX2UI_PTIN_MAIN_FREE ){
				now_pos = BOX2UI_PTIN_MAIN_TSURETEIKU;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * 「ポケモンをつれていく」メイン操作コールバック関数：タッチ時
 *
 * @param	syswk		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PartyInMainCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	if( now_pos != BOX2UI_PTIN_MAIN_LEFT && now_pos != BOX2UI_PTIN_MAIN_RIGHT ){
		CursorObjMove( syswk, now_pos );
	}
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをせいりする」メイン操作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	動作結果
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxArrangeMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
		if( ret >= BOX2UI_ARRANGE_MAIN_MOVE && ret <= BOX2UI_ARRANGE_MAIN_FREE ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
	}else{
		if( ret == BOX2UI_ARRANGE_MAIN_PARTY || ret == BOX2UI_ARRANGE_MAIN_CHANGE ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
		if( ret == BOX2UI_ARRANGE_MAIN_RETURN || ret == CURSORMOVE_CANCEL ){
			return BOX2UI_ARRANGE_MAIN_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをせいりする」メイン操作コールバック関数：カーソル表示時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangeMainCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			switch( now_pos ){
			case BOX2UI_ARRANGE_MAIN_LEFT:
			case BOX2UI_ARRANGE_MAIN_RIGHT:
				now_pos = BOX2UI_ARRANGE_MAIN_NAME;
				break;

			case BOX2UI_ARRANGE_MAIN_NAME:
			case BOX2UI_ARRANGE_MAIN_PARTY:
			case BOX2UI_ARRANGE_MAIN_CHANGE:
			case BOX2UI_ARRANGE_MAIN_RETURN:
				break;

			default:
				if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
					now_pos = syswk->get_pos;
				}else{
					now_pos = BOX2UI_ARRANGE_MOVE_POKE1;
				}
			}
		}else{
			if( now_pos != BOX2UI_ARRANGE_MAIN_MOVE ||
				now_pos != BOX2UI_ARRANGE_MAIN_STATUS ||
				now_pos != BOX2UI_ARRANGE_MAIN_ITEM ||
				now_pos != BOX2UI_ARRANGE_MAIN_MARKING ||
				now_pos != BOX2UI_ARRANGE_MAIN_FREE ){

				now_pos = BOX2UI_ARRANGE_MAIN_MOVE;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをせいりする」メイン操作コールバック関数：カーソル移動時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangeMainCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをせいりする」メイン操作コールバック関数：タッチ時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangeMainCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをせいりする」ポケモン移動操作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	動作結果
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxArrangePokeMove( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	
	if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == FALSE ){
		CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_CHANGE );
	}

	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	CURSORMOVE_MoveTableInit( syswk->app->cmwk );

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをせいりする」ポケモン移動操作コールバック関数：カーソル表示時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePokeMoveCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	u8	x, y;

	syswk = work;

	if( now_pos == BOX2UI_ARRANGE_MOVE_LEFT || now_pos == BOX2UI_ARRANGE_MOVE_RIGHT ){
		now_pos = BOX2UI_ARRANGE_MOVE_TRAY1 + syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;
		syswk->app->old_cur_pos = now_pos;
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, now_pos );

	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

	BOX2OBJ_Vanish( syswk->app, BOX2OBJ_ID_HAND_CURSOR, TRUE );

	if( syswk->app->poke_get_key == 0 ){
		if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
		}else{
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
		}
	}
}

// カーソルループのための位置
#define	BAPM_TRAY_POKE_LINE1_S	( BOX2UI_ARRANGE_MOVE_POKE1 )
#define	BAPM_TRAY_POKE_LINE1_E	( BOX2UI_ARRANGE_MOVE_POKE1 + BOX2OBJ_POKEICON_H_MAX - 1 )
#define	BAPM_TRAY_POKE_LINE5_S	( BOX2UI_ARRANGE_MOVE_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX - BOX2OBJ_POKEICON_H_MAX )
#define	BAPM_TRAY_POKE_LINE5_E	( BOX2UI_ARRANGE_MOVE_POKE1 + BOX2OBJ_POKEICON_TRAY_MAX - 1 )
#define	BAPM_PARTY_POKE_LINE1_S	( BOX2UI_ARRANGE_MOVE_PARTY1 )
#define	BAPM_PARTY_POKE_LINE1_E	( BOX2UI_ARRANGE_MOVE_PARTY1 + 1 )

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをせいりする」ポケモン移動操作コールバック関数：カーソル移動時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePokeMoveCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	if( syswk->app->poke_get_key == 0 ){
		if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
		}else{
			BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
		}
	}

	// ポケモンアイコンからトレイへの移動
	if( now_pos == BOX2UI_ARRANGE_MOVE_TRAY1 &&
		( ( old_pos >= BAPM_TRAY_POKE_LINE1_S && old_pos <= BAPM_TRAY_POKE_LINE1_E ) ||
		  ( old_pos >= BAPM_TRAY_POKE_LINE5_S && old_pos <= BAPM_TRAY_POKE_LINE5_E ) ||
		  ( old_pos >= BAPM_PARTY_POKE_LINE1_S && old_pos <= BAPM_PARTY_POKE_LINE1_E ) ||
		  old_pos == BOX2UI_ARRANGE_MOVE_RETURN ) ){

		now_pos = syswk->app->old_cur_pos;
		CURSORMOVE_PosSetEx( syswk->app->cmwk, now_pos, old_pos, CURSORMOVE_SavePosGet(syswk->app->cmwk) );
	}

	// 前回位置更新
	if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
		syswk->app->old_cur_pos = now_pos;
	}

	// トレイ左から右へのループ
	if( now_pos == BOX2UI_ARRANGE_MOVE_TRAY1 && old_pos == BOX2UI_ARRANGE_MOVE_TRAY6 ){
		if( syswk->box_mv_pos + 1 >= syswk->trayMax ){
			syswk->box_mv_pos = 0;
		}else{
			syswk->box_mv_pos++;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, BOX2OBJ_ANM_R_ARROW_ON );
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	// トレイ右から左へのループ
	if( now_pos == BOX2UI_ARRANGE_MOVE_TRAY6 && old_pos == BOX2UI_ARRANGE_MOVE_TRAY1 ){
		if( syswk->box_mv_pos - 1 < 0 ){
			syswk->box_mv_pos = syswk->trayMax - 1;
		}else{
			syswk->box_mv_pos--;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, BOX2OBJ_ANM_L_ARROW_ON );
		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
		if( old_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && old_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
			syswk->box_mv_pos = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + now_pos - BOX2UI_ARRANGE_MOVE_TRAY1;
			BOX2OBJ_BoxMoveNameSet( syswk );
			BOX2BMP_BoxMoveNameNumWrite( syswk );
		}
	}

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをせいりする」ポケモン移動操作コールバック関数：タッチ時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePokeMoveCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	// 前回位置更新
	if( now_pos >= BOX2UI_ARRANGE_MOVE_TRAY1 && now_pos <= BOX2UI_ARRANGE_MOVE_TRAY6 ){
		syswk->app->old_cur_pos = now_pos;
	}

	if( now_pos != BOX2UI_ARRANGE_MOVE_LEFT && now_pos != BOX2UI_ARRANGE_MOVE_RIGHT ){
		CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );
		CursorObjMove( syswk, now_pos );
	}
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをせいりする」ポケモン移動時にアイコンを掴んでいるときの操作
 *
 * @param	syswk		ボックス画面システムワーク
 *
 * @return	動作結果
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxArrangePokeMoveHand( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	
	if( BOX2BGWFRM_BoxMoveButtonCheck( syswk->app->wfrm ) == FALSE ){
		CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_CHANGE );
	}

	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_LEFT );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_RIGHT );

	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	CURSORMOVE_MoveTableInit( syswk->app->cmwk );

	return ret;
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをせいりする」手持ちポケモン操作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	動作結果
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxArrangePartyMoveMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == FALSE ){
		if( ret >= BOX2UI_ARRANGE_PARTY_MOVE && ret <= BOX2UI_ARRANGE_PARTY_FREE ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
	}else{
		if( ret == BOX2UI_ARRANGE_PARTY_RETURN || ret == CURSORMOVE_CANCEL ){
			return BOX2UI_ARRANGE_PARTY_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをせいりする」手持ちポケモン操作コールバック関数：カーソル表示時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePartyMoveCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( now_pos >= BOX2UI_ARRANGE_PARTY_MOVE ){
				if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
					now_pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
				}else{
					now_pos = BOX2UI_ARRANGE_PARTY_POKE1;
				}
			}
		}else{
			if( now_pos != BOX2UI_ARRANGE_PARTY_MOVE ||
				now_pos != BOX2UI_ARRANGE_PARTY_STATUS ||
				now_pos != BOX2UI_ARRANGE_PARTY_ITEM ||
				now_pos != BOX2UI_ARRANGE_PARTY_MARKING ||
				now_pos != BOX2UI_ARRANGE_PARTY_FREE ){

				now_pos = BOX2UI_ARRANGE_PARTY_MOVE;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * 「ボックスをせいりする」手持ちポケモン操作コールバック関数：カーソル移動時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxArrangePartyMoveCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	// メニューが表示されてるとき
	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == TRUE ){
		if( now_pos <= BOX2UI_ARRANGE_PARTY_CHANGE ){
			if( old_pos >= BOX2UI_ARRANGE_PARTY_RETURN ){
				BOX2MAIN_PokeSelectOff( syswk );
				BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
				BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncMenuCloseKeyArrange );
				return;
			}
		}
	}else{
		if( now_pos == BOX2UI_ARRANGE_PARTY_FREE && old_pos == BOX2UI_ARRANGE_PARTY_RETURN ){
			CURSORMOVE_PosSet( syswk->app->cmwk, BOX2UI_ARRANGE_PARTY_RETURN );
			now_pos = BOX2UI_ARRANGE_PARTY_RETURN;
		}
	}

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 「もちものをせいりする」メイン操作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	動作結果
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxItemArrangeMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
		if( ret == BOX2UI_ITEM_A_MAIN_MENU1 ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
	}else{
		if( ret == BOX2UI_ITEM_A_MAIN_PARTY || ret == BOX2UI_ITEM_A_MAIN_CHANGE ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
		if( ret == BOX2UI_ITEM_A_MAIN_RETURN || ret == CURSORMOVE_CANCEL ){
			return BOX2UI_ITEM_A_MAIN_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * 「もちものをせいりする」メイン操作コールバック関数：カーソル表示時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxItemArrangeMainCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			switch( now_pos ){
			case BOX2UI_ITEM_A_MAIN_LEFT:
			case BOX2UI_ITEM_A_MAIN_RIGHT:
				now_pos = BOX2UI_ITEM_A_MAIN_NAME;
				break;

			case BOX2UI_ITEM_A_MAIN_NAME:
			case BOX2UI_ITEM_A_MAIN_PARTY:
			case BOX2UI_ITEM_A_MAIN_CHANGE:
			case BOX2UI_ITEM_A_MAIN_RETURN:
				break;

			default:
				if( syswk->get_pos < BOX2OBJ_POKEICON_TRAY_MAX ){
					now_pos = syswk->get_pos;
				}else{
					now_pos = BOX2UI_ITEM_A_MAIN_POKE1;
				}
			}
		}else{
			if( now_pos != BOX2UI_ITEM_A_MAIN_MENU1 ){
				now_pos = BOX2UI_ITEM_A_MAIN_MENU1;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * 「もちものをせいりする」メイン操作コールバック関数：カーソル移動時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxItemArrangeMainCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * 「もちものをせいりする」メイン操作コールバック関数：タッチ時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxItemArrangeMainCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 「もちものをせいりする」手持ちポケモン操作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	動作結果
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxItemArrangePartyMoveMain( BOX2_SYS_WORK * syswk )
{
	u32	ret;
	u8	now;

	now = CURSORMOVE_PosGet( syswk->app->cmwk );
	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	if( BOX2BGWFRM_PokeMenuPutCheck( syswk->app->wfrm ) == FALSE ){
		if( ret == BOX2UI_ITEM_A_PARTY_MENU1 ){
			CURSORMOVE_PosSet( syswk->app->cmwk, now );
			CursorObjMove( syswk, now );
			return CURSORMOVE_NONE;
		}
	}else{
		if( ret == BOX2UI_ITEM_A_PARTY_RETURN || ret == CURSORMOVE_CANCEL ){
			return BOX2UI_ITEM_A_PARTY_CLOSE;
		}
	}

	return ret;
}

//--------------------------------------------------------------------------------------------
/**
 * 「もちものをせいりする」手持ちポケモン操作コールバック関数：カーソル表示時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxItemArrangePartyMoveCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	
	syswk = work;

	if( old_pos != CURSORMOVE_ONOFF_DIRECT ){
		if( BOX2BGWFRM_PokeMenuPutCheck(syswk->app->wfrm) == FALSE ){
			if( now_pos >= BOX2UI_ITEM_A_PARTY_MENU1 ){
				if( syswk->get_pos >= BOX2OBJ_POKEICON_TRAY_MAX ){
					now_pos = syswk->get_pos - BOX2OBJ_POKEICON_TRAY_MAX;
				}else{
					now_pos = BOX2UI_ITEM_A_PARTY_POKE1;
				}
			}
		}else{
			if( now_pos != BOX2UI_ITEM_A_PARTY_MENU1 ){
				now_pos = BOX2UI_ITEM_A_PARTY_MENU1;
			}
		}
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	CursorObjMove( syswk, now_pos );
}

//--------------------------------------------------------------------------------------------
/**
 * 「もちものをせいりする」手持ちポケモン操作コールバック関数：カーソル移動時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxItemArrangePartyMoveCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );

	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * ボックステーマ変更操作コールバック関数：カーソル表示時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxThemaChgCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	u8	x, y;

	syswk = work;

	if( now_pos == BOX2UI_BOXTHEMA_CHG_LEFT || now_pos == BOX2UI_BOXTHEMA_CHG_RIGHT ){
		now_pos = syswk->box_mv_pos % BOX2OBJ_TRAYICON_MAX;
		syswk->app->old_cur_pos = now_pos;
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, now_pos );

	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

	if( now_pos >= BOX2UI_BOXTHEMA_CHG_TRAY1 && now_pos <= BOX2UI_BOXTHEMA_CHG_TRAY6 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * ボックステーマ変更操作コールバック関数：カーソル移動時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxThemaChgCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	if( now_pos >= BOX2UI_BOXTHEMA_CHG_TRAY1 && now_pos <= BOX2UI_BOXTHEMA_CHG_TRAY6 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}

	//「ボックスをきりかえる」からトレイへの移動
	if( old_pos == BOX2UI_BOXTHEMA_CHG_MOVE && now_pos == BOX2UI_BOXTHEMA_CHG_TRAY1 ){
		now_pos = syswk->app->old_cur_pos;
		CURSORMOVE_PosSetEx( syswk->app->cmwk, now_pos, BOX2UI_BOXTHEMA_CHG_MOVE, BOX2UI_BOXTHEMA_CHG_MOVE );
	}

	// 前回位置更新
	if( now_pos >= BOX2UI_BOXTHEMA_CHG_TRAY1 && now_pos <= BOX2UI_BOXTHEMA_CHG_TRAY6 ){
		syswk->app->old_cur_pos = now_pos;
	}

	// トレイ左から右へのループ
	if( now_pos == BOX2UI_BOXTHEMA_CHG_TRAY1 && old_pos == BOX2UI_BOXTHEMA_CHG_TRAY6 ){
		if( syswk->box_mv_pos + 1 >= syswk->trayMax ){
			syswk->box_mv_pos = 0;
		}else{
			syswk->box_mv_pos++;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, BOX2OBJ_ANM_R_ARROW_ON );

		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	// トレイ右から左へのループ
	if( now_pos == BOX2UI_BOXTHEMA_CHG_TRAY6 && old_pos == BOX2UI_BOXTHEMA_CHG_TRAY1 ){
		if( syswk->box_mv_pos - 1 < 0 ){
			syswk->box_mv_pos = syswk->trayMax - 1;
		}else{
			syswk->box_mv_pos--;
		}
		BOX2OBJ_TrayIconChange( syswk );
		BOX2OBJ_BoxMoveNameSet( syswk );
		BOX2OBJ_BoxMoveCursorSet( syswk );
		BOX2BMP_BoxMoveNameNumWrite( syswk );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, BOX2OBJ_ANM_L_ARROW_ON );

		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	if( now_pos >= BOX2UI_BOXTHEMA_CHG_TRAY1 && now_pos <= BOX2UI_BOXTHEMA_CHG_TRAY6 ){
		if( old_pos != BOX2UI_BOXTHEMA_CHG_MOVE ){
			syswk->box_mv_pos = BOX2MAIN_MV_TRAY_DEF(syswk->box_mv_pos) + now_pos;
			BOX2OBJ_BoxMoveNameSet( syswk );
			BOX2BMP_BoxMoveNameNumWrite( syswk );
		}
	}

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * ボックステーマ変更操作コールバック関数：カーソル移動時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BoxThemaChgCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	if( now_pos != BOX2UI_BOXTHEMA_CHG_LEFT && now_pos != BOX2UI_BOXTHEMA_CHG_RIGHT ){
		CursorObjMove( syswk, now_pos );
	}
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更操作
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @return	動作結果
 */
//--------------------------------------------------------------------------------------------
static void WallPaperChgCallBack_On( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk;
	const CURSORMOVE_DATA * pw;
	u8	x, y;

	syswk = work;

	if( now_pos == BOX2UI_WALLPAPER_CHG_LEFT || now_pos == BOX2UI_WALLPAPER_CHG_RIGHT ){
		now_pos = syswk->app->wallpaper_pos % BOX2OBJ_WPICON_MAX;
		syswk->app->old_cur_pos = now_pos;
	}

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	pw = CURSORMOVE_GetMoveData( syswk->app->cmwk, now_pos );

	x = pw->px;
	y = pw->py;
	BOX2OBJ_SetPos( syswk->app, BOX2OBJ_ID_HAND_CURSOR, x, y, CLSYS_DEFREND_MAIN );

	if( now_pos >= BOX2UI_WALLPAPER_CHG_ICON1 && now_pos <= BOX2UI_WALLPAPER_CHG_ICON4 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}
}

static void WallPaperChgCallBack_Move( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	if( now_pos >= BOX2UI_WALLPAPER_CHG_ICON1 && now_pos <= BOX2UI_WALLPAPER_CHG_ICON4 ){
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_TRAY );
	}else{
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_HAND_CURSOR, BOX2OBJ_ANM_HAND_NORMAL );
	}

	//「はりかえる」からトレイへの移動
	if( old_pos == BOX2UI_WALLPAPER_CHG_ENTER && now_pos == BOX2UI_WALLPAPER_CHG_ICON1 ){
		now_pos = syswk->app->old_cur_pos;
		CURSORMOVE_PosSetEx( syswk->app->cmwk, now_pos, BOX2UI_WALLPAPER_CHG_ENTER, BOX2UI_WALLPAPER_CHG_ENTER );
	}

	// 前回位置更新
	if( now_pos >= BOX2UI_WALLPAPER_CHG_ICON1 && now_pos <= BOX2UI_WALLPAPER_CHG_ICON4 ){
		syswk->app->old_cur_pos = now_pos;
	}

	// トレイ左から右へのループ
	if( now_pos == BOX2UI_WALLPAPER_CHG_ICON1 && old_pos == BOX2UI_WALLPAPER_CHG_ICON4 ){
		if( syswk->app->wallpaper_pos + 1 >= BOX_TOTAL_WALLPAPER_MAX ){
			syswk->app->wallpaper_pos = 0;
		}else{
			syswk->app->wallpaper_pos++;
		}
		BOX2OBJ_WallPaperObjChange( syswk );
		BOX2OBJ_WallPaperCursorSet( syswk );
		BOX2OBJ_WallPaperNameSet( syswk );
		BOX2BMP_WallPaperNameWrite( syswk );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_RA, BOX2OBJ_ANM_R_ARROW_ON );

		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	// トレイ右から左へのループ
	if( now_pos == BOX2UI_WALLPAPER_CHG_ICON4 && old_pos == BOX2UI_WALLPAPER_CHG_ICON1 ){
		if( syswk->app->wallpaper_pos - 1 < 0 ){
			syswk->app->wallpaper_pos = BOX_TOTAL_WALLPAPER_MAX - 1;
		}else{
			syswk->app->wallpaper_pos--;
		}
		BOX2OBJ_WallPaperObjChange( syswk );
		BOX2OBJ_WallPaperCursorSet( syswk );
		BOX2OBJ_WallPaperNameSet( syswk );
		BOX2BMP_WallPaperNameWrite( syswk );
		BOX2OBJ_AnmSet( syswk->app, BOX2OBJ_ID_BOXMV_LA, BOX2OBJ_ANM_L_ARROW_ON );

		BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
		BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
		return;
	}

	if( now_pos >= BOX2UI_WALLPAPER_CHG_ICON1 && now_pos <= BOX2UI_WALLPAPER_CHG_ICON4 ){
		if( old_pos != BOX2UI_WALLPAPER_CHG_ENTER ){
			syswk->app->wallpaper_pos = syswk->app->wallpaper_pos / BOX2OBJ_WPICON_MAX * BOX2OBJ_WPICON_MAX + now_pos;
			BOX2OBJ_WallPaperNameSet( syswk );
			BOX2BMP_WallPaperNameWrite( syswk );
		}
	}

	BOX2UI_CursorMoveVFuncWorkSet( syswk->app, now_pos, old_pos );
	BOX2MAIN_VFuncReq( syswk->app, BOX2MAIN_VFuncCursorMove );
}

//--------------------------------------------------------------------------------------------
/**
 * 壁紙変更操作コールバック関数：タッチ時
 *
 * @param	work		ボックス画面システムワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WallPaperChgCallBack_Touch( void * work, int now_pos, int old_pos )
{
	BOX2_SYS_WORK * syswk = work;

	CURSORMOVE_PosSet( syswk->app->cmwk, now_pos );

	if( now_pos != BOX2UI_WALLPAPER_CHG_LEFT && now_pos != BOX2UI_WALLPAPER_CHG_RIGHT ){
		CursorObjMove( syswk, now_pos );
	}
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * 「もちものをせいりする」持ち物整理時にアイコンを掴んでいるときの操作
 *
 * @param	syswk		ボックス画面システムワーク
 *
 * @return	動作結果
 *
 *	※ボックス整理・ポケモン移動のワークを使用
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_BoxItemArrangeMoveHand( BOX2_SYS_WORK * syswk )
{
	u32	ret;

	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_LEFT );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_RIGHT );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_CHANGE );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY1 );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY2 );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY3 );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY4 );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY5 );
	CURSORMOVE_MoveTableBitOff( syswk->app->cmwk, BOX2UI_ARRANGE_MOVE_TRAY6 );

	ret = CURSORMOVE_MainCont( syswk->app->cmwk );

	CURSORMOVE_MoveTableInit( syswk->app->cmwk );

	return ret;
}


//============================================================================================
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * カーソル移動をVBLANKで行うためのワーク設定
 *
 * @param	appwk		ボックス画面アプリワーク
 * @param	now_pos		現在の位置
 * @param	old_pos		前回の位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void BOX2UI_CursorMoveVFuncWorkSet( BOX2_APP_WORK * appwk, int now_pos, int old_pos )
{
	const CURSORMOVE_DATA * pw;
	BOX2MAIN_CURSORMOVE_WORK * cwk;
	u8	nx, ny, ox, oy;

	pw = CURSORMOVE_GetMoveData( appwk->cmwk, now_pos );
	nx = pw->px;
	ny = pw->py;
	pw = CURSORMOVE_GetMoveData( appwk->cmwk, old_pos );
	ox = pw->px;
	oy = pw->py;

	cwk = GFL_HEAP_AllocMemory( HEAPID_BOX_APP, sizeof(BOX2MAIN_CURSORMOVE_WORK) );
//↑[GS_CONVERT_TAG]
	cwk->cnt = 4;
	cwk->px  = nx;
	cwk->py  = ny;

	if( nx >= ox ){
		cwk->vx = nx - ox;
		cwk->mx = 0;
	}else{
		cwk->vx = ox - nx;
		cwk->mx = 1;
	}
	if( ny >= oy ){
		cwk->vy = ny - oy;
		cwk->my = 0;
	}else{
		cwk->vy = oy - ny;
		cwk->my = 1;
	}
	cwk->vx = ( ( cwk->vx << 8 ) / cwk->cnt ) >> 8;
	cwk->vy = ( ( cwk->vy << 8 ) / cwk->cnt ) >> 8;

	appwk->vfunk.work = cwk;
}

//--------------------------------------------------------------------------------------------
/**
 * Ｙステータスボタンチェック
 *
 * @param	syswk	ボックス画面システムワーク
 *
 * @retval	"TRUE = 起動可"
 * @retval	"FALSE = 起動不可"
 */
//--------------------------------------------------------------------------------------------
BOOL BOX2UI_YStatusButtonCheck( BOX2_SYS_WORK * syswk )
{
	if( BOX2BGWFRM_YStatusButtonCheck(syswk->app->wfrm) == FALSE ){
		return FALSE;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y ){
		return TRUE;
	}

	if( GFL_UI_TP_HitTrg( YStatusHitTbl ) != GFL_UI_TP_HIT_NONE ){
		return TRUE;
	}

	return FALSE;
}


static u32 PokeHitCheck( const CURSORMOVE_DATA * dat, u32 max )
{
	GFL_UI_TP_HITTBL	rect[2];
	u32	i;

	rect[1] = TpCheckEnd;

	for( i=0; i<max; i++ ){
		rect[0] = dat[i].rect;
		if( GFL_UI_TP_HitTrg( rect ) != GFL_UI_TP_HIT_NONE ){
			return i;
		}
	}

	return GFL_UI_TP_HIT_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * タッチチェック：トレイのポケモン
 *
 * @param	none
 *
 * @return	GFL_UI_TP_HIT_NONE = 触っていない
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_HitTrayPokeTrg(void)
{
	return PokeHitCheck( BoxArrangeMainCursorMoveData, BOX2OBJ_POKEICON_TRAY_MAX );
}

//--------------------------------------------------------------------------------------------
/**
 * タッチチェック：手持ちポケモン（左）
 *
 * @param	none
 *
 * @return	GFL_UI_TP_HIT_NONE = 触っていない
//↑[GS_CONVERT_TAG]
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_HitCheckTrgPartyPokeLeft(void)
{
	return PokeHitCheck( BoxArrangePartyPokeCursorMoveData, BOX2OBJ_POKEICON_MINE_MAX );
}

//--------------------------------------------------------------------------------------------
/**
 * タッチチェック：手持ちポケモン（右）
 *
 * @param	none
 *
 * @return	GFL_UI_TP_HIT_NONE = 触っていない
//↑[GS_CONVERT_TAG]
 */
//--------------------------------------------------------------------------------------------
u32 BOX2UI_HitCheckTrgPartyPokeRight(void)
{
	return PokeHitCheck(
					&BoxArrangePartyPokeCursorMoveData[BOX2UI_ARRANGE_MOVE_PARTY1], BOX2OBJ_POKEICON_MINE_MAX );
}





#if 0
//============================================================================================
/**
 * @file	box2_tp.c
 * @brief	新ボックス画面 タッチパネル関連
 * @author	Hiroyuki Nakamura
 * @date	08.05.30
 */
//============================================================================================
#include <gflib.h>
//↑[GS_CONVERT_TAG]

#include "gflib/touchpanel.h"

#include "box2_main.h"
#include "box2_obj.h"
#include "box2_tp.h"


//============================================================================================
//	定数定義
//============================================================================================
#define	TRAYPOKE_SX		( BOX2OBJ_TRAYPOKE_SX )						// トレイのポケモンのタッチ範囲サイズＸ
#define	TRAYPOKE_SY		( 24 )										// トレイのポケモンのタッチ範囲サイズＹ
#define	TRAYPOKE_PX		( BOX2OBJ_TRAYPOKE_PX - TRAYPOKE_SX / 2 )	// トレイのポケモンのタッチ開始座標Ｘ
#define	TRAYPOKE_PY		( 40 )										// トレイのポケモンのタッチ開始座標Ｙ


//============================================================================================
//	プロトタイプ宣言
//============================================================================================


//============================================================================================
//	グローバル変数
//============================================================================================

// タッチ座標テーブル：トレイのポケモン
static const GFL_UI_TP_HITTBL TrayPokeHitTbl[] =
//↑[GS_CONVERT_TAG]
{
	{ TRAYPOKE_PY+TRAYPOKE_SX*0, TRAYPOKE_PY+TRAYPOKE_SY*1-1, TRAYPOKE_PX+TRAYPOKE_SX*0, TRAYPOKE_PX+TRAYPOKE_SX*1-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*0, TRAYPOKE_PY+TRAYPOKE_SY*1-1, TRAYPOKE_PX+TRAYPOKE_SX*1, TRAYPOKE_PX+TRAYPOKE_SX*2-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*0, TRAYPOKE_PY+TRAYPOKE_SY*1-1, TRAYPOKE_PX+TRAYPOKE_SX*2, TRAYPOKE_PX+TRAYPOKE_SX*3-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*0, TRAYPOKE_PY+TRAYPOKE_SY*1-1, TRAYPOKE_PX+TRAYPOKE_SX*3, TRAYPOKE_PX+TRAYPOKE_SX*4-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*0, TRAYPOKE_PY+TRAYPOKE_SY*1-1, TRAYPOKE_PX+TRAYPOKE_SX*4, TRAYPOKE_PX+TRAYPOKE_SX*5-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*0, TRAYPOKE_PY+TRAYPOKE_SY*1-1, TRAYPOKE_PX+TRAYPOKE_SX*5, TRAYPOKE_PX+TRAYPOKE_SX*6-1 },

	{ TRAYPOKE_PY+TRAYPOKE_SX*1, TRAYPOKE_PY+TRAYPOKE_SY*2-1, TRAYPOKE_PX+TRAYPOKE_SX*0, TRAYPOKE_PX+TRAYPOKE_SX*1-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*1, TRAYPOKE_PY+TRAYPOKE_SY*2-1, TRAYPOKE_PX+TRAYPOKE_SX*1, TRAYPOKE_PX+TRAYPOKE_SX*2-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*1, TRAYPOKE_PY+TRAYPOKE_SY*2-1, TRAYPOKE_PX+TRAYPOKE_SX*2, TRAYPOKE_PX+TRAYPOKE_SX*3-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*1, TRAYPOKE_PY+TRAYPOKE_SY*2-1, TRAYPOKE_PX+TRAYPOKE_SX*3, TRAYPOKE_PX+TRAYPOKE_SX*4-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*1, TRAYPOKE_PY+TRAYPOKE_SY*2-1, TRAYPOKE_PX+TRAYPOKE_SX*4, TRAYPOKE_PX+TRAYPOKE_SX*5-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*1, TRAYPOKE_PY+TRAYPOKE_SY*2-1, TRAYPOKE_PX+TRAYPOKE_SX*5, TRAYPOKE_PX+TRAYPOKE_SX*6-1 },

	{ TRAYPOKE_PY+TRAYPOKE_SX*2, TRAYPOKE_PY+TRAYPOKE_SY*3-1, TRAYPOKE_PX+TRAYPOKE_SX*0, TRAYPOKE_PX+TRAYPOKE_SX*1-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*2, TRAYPOKE_PY+TRAYPOKE_SY*3-1, TRAYPOKE_PX+TRAYPOKE_SX*1, TRAYPOKE_PX+TRAYPOKE_SX*2-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*2, TRAYPOKE_PY+TRAYPOKE_SY*3-1, TRAYPOKE_PX+TRAYPOKE_SX*2, TRAYPOKE_PX+TRAYPOKE_SX*3-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*2, TRAYPOKE_PY+TRAYPOKE_SY*3-1, TRAYPOKE_PX+TRAYPOKE_SX*3, TRAYPOKE_PX+TRAYPOKE_SX*4-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*2, TRAYPOKE_PY+TRAYPOKE_SY*3-1, TRAYPOKE_PX+TRAYPOKE_SX*4, TRAYPOKE_PX+TRAYPOKE_SX*5-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*2, TRAYPOKE_PY+TRAYPOKE_SY*3-1, TRAYPOKE_PX+TRAYPOKE_SX*5, TRAYPOKE_PX+TRAYPOKE_SX*6-1 },

	{ TRAYPOKE_PY+TRAYPOKE_SX*3, TRAYPOKE_PY+TRAYPOKE_SY*4-1, TRAYPOKE_PX+TRAYPOKE_SX*0, TRAYPOKE_PX+TRAYPOKE_SX*1-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*3, TRAYPOKE_PY+TRAYPOKE_SY*4-1, TRAYPOKE_PX+TRAYPOKE_SX*1, TRAYPOKE_PX+TRAYPOKE_SX*2-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*3, TRAYPOKE_PY+TRAYPOKE_SY*4-1, TRAYPOKE_PX+TRAYPOKE_SX*2, TRAYPOKE_PX+TRAYPOKE_SX*3-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*3, TRAYPOKE_PY+TRAYPOKE_SY*4-1, TRAYPOKE_PX+TRAYPOKE_SX*3, TRAYPOKE_PX+TRAYPOKE_SX*4-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*3, TRAYPOKE_PY+TRAYPOKE_SY*4-1, TRAYPOKE_PX+TRAYPOKE_SX*4, TRAYPOKE_PX+TRAYPOKE_SX*5-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*3, TRAYPOKE_PY+TRAYPOKE_SY*4-1, TRAYPOKE_PX+TRAYPOKE_SX*5, TRAYPOKE_PX+TRAYPOKE_SX*6-1 },

	{ TRAYPOKE_PY+TRAYPOKE_SX*4, TRAYPOKE_PY+TRAYPOKE_SY*5-1, TRAYPOKE_PX+TRAYPOKE_SX*0, TRAYPOKE_PX+TRAYPOKE_SX*1-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*4, TRAYPOKE_PY+TRAYPOKE_SY*5-1, TRAYPOKE_PX+TRAYPOKE_SX*1, TRAYPOKE_PX+TRAYPOKE_SX*2-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*4, TRAYPOKE_PY+TRAYPOKE_SY*5-1, TRAYPOKE_PX+TRAYPOKE_SX*2, TRAYPOKE_PX+TRAYPOKE_SX*3-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*4, TRAYPOKE_PY+TRAYPOKE_SY*5-1, TRAYPOKE_PX+TRAYPOKE_SX*3, TRAYPOKE_PX+TRAYPOKE_SX*4-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*4, TRAYPOKE_PY+TRAYPOKE_SY*5-1, TRAYPOKE_PX+TRAYPOKE_SX*4, TRAYPOKE_PX+TRAYPOKE_SX*5-1 },
	{ TRAYPOKE_PY+TRAYPOKE_SX*4, TRAYPOKE_PY+TRAYPOKE_SY*5-1, TRAYPOKE_PX+TRAYPOKE_SX*5, TRAYPOKE_PX+TRAYPOKE_SX*6-1 },

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
//↑[GS_CONVERT_TAG]
};

// タッチ座標テーブル：手持ちポケモン（左）
static const GFL_UI_TP_HITTBL PartyPokeLeftHitTbl[] =
//↑[GS_CONVERT_TAG]
{
	{  68-12,  68+12-1, 42-12, 42+12-1 }, {  76-12,  76+12-1, 78-12, 78+12-1 },
	{ 100-12, 100+12-1, 42-12, 42+12-1 }, { 108-12, 108+12-1, 78-12, 78+12-1 },
	{ 132-12, 132+12-1, 42-12, 42+12-1 }, { 140-12, 140+12-1, 78-12, 78+12-1 },

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
//↑[GS_CONVERT_TAG]
};

// タッチ座標テーブル：手持ちポケモン（右）
static const GFL_UI_TP_HITTBL PartyPokeRightHitTbl[] =
//↑[GS_CONVERT_TAG]
{
	{  68-12,  68+12-1, 194-12, 194+12-1 }, {  76-12,  76+12-1, 230-12, 230+12-1 },
	{ 100-12, 100+12-1, 194-12, 194+12-1 }, { 108-12, 108+12-1, 230-12, 230+12-1 },
	{ 132-12, 132+12-1, 194-12, 194+12-1 }, { 140-12, 140+12-1, 230-12, 230+12-1 },

	{ GFL_UI_TP_HIT_END, 0, 0, 0 }
//↑[GS_CONVERT_TAG]
};


#endif
