//============================================================================================
/**
 * @file		zknlist_main.h
 * @brief		図鑑リスト画面 メイン処理
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	モジュール名：ZKNLISTMAIN
 */
//============================================================================================
#pragma once

#include "system/main.h"
#include "system/bgwinfrm.h"
#include "system/cursor_move.h"
#include "print/printsys.h"
#include "print/wordset.h"

#include "../zukan_common.h"
#include "zukanlist.h"
#include "zknlist_bmp_def.h"
#include "zknlist_obj_def.h"
#include "zknlist_bgwfrm_def.h"


//============================================================================================
//	定数定義
//============================================================================================

// 後方確保用ヒープＩＤ
#define	HEAPID_ZUKAN_LIST_L		( GFL_HEAP_LOWID(HEAPID_ZUKAN_LIST) )

#define	ZKNLISTMAIN_MBG_PAL_TOUCHBAR	( 14 )
#define	ZKNLISTMAIN_MBG_PAL_FONT			( 15 )

#define	ZKNLISTMAIN_SBG_PAL_FONT			( 15 )

// タッチバー定義
#define	ZKNLISTMAIN_TOUCH_BAR_PX		( 0 )
#define	ZKNLISTMAIN_TOUCH_BAR_PY		( 21 )
#define	ZKNLISTMAIN_TOUCH_BAR_SX		( 32 )
#define	ZKNLISTMAIN_TOUCH_BAR_SY		( 3 )

// リスト項目サイズ
#define	ZKNLISTMAIN_LIST_PX					( 16 )
#define	ZKNLISTMAIN_LIST_SX					( 16 )
#define	ZKNLISTMAIN_LIST_SY					( 3 )


typedef struct _ZUKAN_LIST_WORK	ZUKAN_LIST_WORK;
typedef void (*pZUKAN_LIST_CALLBACK)(void*,s16,s16,s16,s16,u32);


// リスト画面ワーク
typedef struct {
	ZUKANLIST_DATA * dat;

	GFL_TCB * vtask;		// TCB ( VBLANK )

//	int	key_repeat_speed;
//	int	key_repeat_wait;

	int	mainSeq;		// メインシーケンス
	int	nextSeq;		// 次のシーケンス
	int	wipeSeq;		// ワイプ後のシーケンス

	BGWINFRM_WORK * wfrm;			// ウィンドウフレーム
	u16 * nameBG[2];

	PRINT_UTIL	win[ZKNLISTBMP_WINIDX_MAX];		// BMPWIN
	u8 * nameBmp;

	GFL_FONT * font;					// 通常フォント
	GFL_MSGDATA * mman;				// メッセージデータマネージャ
	WORDSET * wset;						// 単語セット
	STRBUF * exp;							// メッセージ展開領域
	PRINT_QUE * que;					// プリントキュー

	// OBJ
	GFL_CLUNIT * clunit;
	GFL_CLWK * clwk[ZKNLISTOBJ_IDX_MAX];
	u32	chrRes[ZKNLISTOBJ_CHRRES_MAX];
	u32	palRes[ZKNLISTOBJ_PALRES_MAX];
	u32	celRes[ZKNLISTOBJ_CELRES_MAX];

	u32	buttonID:31;				// ボタンアニメ用ＩＤ
	u32	pokeGraFlag:1;			// ポケモン正面絵表示制御

	ZUKAN_LIST_WORK * list;
	s32	listPutIndex;
	u8	listScroll;
	u8	listSpeed;
	u8	listRepeat;
	u8	listConut;

	u32	bgScroll;


/*
	PALETTE_FADE_PTR	pfd;		// パレットフェードデータ
	BUTTON_ANM_WORK	bawk;				// ボタンアニメワーク
*/



}ZKNLISTMAIN_WORK;

typedef int (*pZKNLIST_FUNC)(ZKNLISTMAIN_WORK*);


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK関数設定
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_InitVBlank( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief		VBLANK関数削除
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_ExitVBlank( ZKNLISTMAIN_WORK * wk );

extern void ZKNLISTMAIN_InitVram(void);
extern const GFL_DISP_VRAM * ZKNLISTMAIN_GetVramBankData(void);
extern void ZKNLISTMAIN_InitBg(void);
extern void ZKNLISTMAIN_ExitBg(void);
extern void ZKNLISTMAIN_LoadBgGraphic(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief		アルファブレンド設定
 *
 * @param		none
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_SetBlendAlpha(void);

//--------------------------------------------------------------------------------------------
/**
 * @brief	  メッセージ関連初期化
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_InitMsg( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  メッセージ関連解放
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_ExitMsg( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リスト作成
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_MakeList( ZKNLISTMAIN_WORK * wk );

//--------------------------------------------------------------------------------------------
/**
 * @brief	  リスト削除
 *
 * @param		wk		図鑑リストワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
extern void ZKNLISTMAIN_FreeList( ZKNLISTMAIN_WORK * wk );

extern u32 ZKNLISTMAIN_GetListMons( ZUKAN_LIST_WORK * wk, u32 pos );

extern u32 ZKNLISTMAIN_GetListInfo( ZUKAN_LIST_WORK * wk, u32 pos );

extern void ZKNLISTMAIN_PutListCursor( ZKNLISTMAIN_WORK * wk, u8 pal, s16 pos );

extern void ZKNLISTMAIN_InitListPut( ZKNLISTMAIN_WORK * wk );






#define	ZKNLISTMAIN_LIST_MOVE_UP					( 0 )
#define	ZKNLISTMAIN_LIST_MOVE_DOWN				( 1 )
#define	ZKNLISTMAIN_LIST_MOVE_LEFT				( 2 )
#define	ZKNLISTMAIN_LIST_MOVE_RIGHT				( 3 )
#define	ZKNLISTMAIN_LIST_MOVE_SCROLL_UP		( 4 )
#define	ZKNLISTMAIN_LIST_MOVE_SCROLL_DOWN	( 5 )
#define	ZKNLISTMAIN_LIST_MOVE_NONE		( 0xffffffff )

extern ZUKAN_LIST_WORK * ZKNLISTMAIN_CreateList( u32 siz, HEAPID heapID );
extern void ZKNLISTMAIN_ExitList( ZUKAN_LIST_WORK * wk );
extern void ZKNLISTMAIN_AddListData( ZUKAN_LIST_WORK * wk, STRBUF * str, u32 prm );
extern void ZKNLISTMAIN_InitList( ZUKAN_LIST_WORK * wk, s16 pos, s16 posMax, s16 scroll, void * work, pZUKAN_LIST_CALLBACK func );
extern s16 ZKNLISTMAIN_GetListPos( ZUKAN_LIST_WORK * wk );
extern s16 ZKNLISTMAIN_GetListScroll( ZUKAN_LIST_WORK * wk );
extern s16 ZKNLISTMAIN_GetListCursorPos( ZUKAN_LIST_WORK * wk );
extern STRBUF * ZKNLISTMAIN_GetListStr( ZUKAN_LIST_WORK * wk, u32 pos );
extern u32 ZKNLISTMAIN_GetListParam( ZUKAN_LIST_WORK * wk, u32 pos );
extern u32 ZKNLISTMAIN_Main( ZUKAN_LIST_WORK * wk );
extern u32 ZKNLISTMAIN_MoveLeft( ZUKAN_LIST_WORK * wk );
extern u32 ZKNLISTMAIN_MoveRight( ZUKAN_LIST_WORK * wk );
