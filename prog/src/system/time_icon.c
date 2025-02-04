//============================================================================================
/**
 * @file		time_icon.c
 * @brief		セーブ中などに表示されるタイマーアイコン処理
 * @author	Hiroyuki Nakamura
 * @date		10.02.22
 *
 *	モジュール名：TIMEICON
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/gfl_use.h"
#include "system/time_icon.h"
#include "time_icon_gra.naix"


//============================================================================================
//	定数定義
//============================================================================================

// ワーク
struct _TIMEICON_WORK {
	GFL_TCB * tcb;			// TCB
	GFL_TCBL * tcbl;		// TCBL
	GFL_BMPWIN * win;		// BMPWIN
	GFL_BMP_DATA * src;	// アイコンキャラデータ
	HEAPID heapID;			// ヒープＩＤ

	u8	col;						// クリアカラー
	u8	cnt;
	u8	wait;
	u8	anm;

	BOOL	flg;					// 動作許可フラグ
};

#define	TIMEICON_SX				( 2 )								// アイコンＸサイズ（キャラ単位）
#define	TIMEICON_SY				( 2 )								// アイコンＹサイズ（キャラ単位）
#define	TIMEICON_DOT_SX		( TIMEICON_SX*8 )		// アイコンＸサイズ（ドット単位）
#define	TIMEICON_DOT_SY		( TIMEICON_SY*8 )		// アイコンＹサイズ（ドット単位）

#define	TIMEICON_ANM_MAX	( 8 )								// アイコンアニメ数


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void MainTask(  GFL_TCB * tcb, void * work );
static void MainTaskTcbl(  GFL_TCBL * tcbl, void * work );

static void TIMEICON_CreateCore(
							TIMEICON_WORK * wk , GFL_BMPWIN * msg_win, u8 clear_color, u8 wait, HEAPID heapID );


//--------------------------------------------------------------------------------------------
/**
 * @brief		タイマーアイコン処理追加 ( TCB )
 *
 * @param		tcbsys				GFL_TCBSYS
 * @param		msg_win				メッセージを表示しているウィンドウ
 * @param		clear_color		塗りつぶしカラー
 * @param		wait					アニメウェイト
 * @param		heapID				ヒープＩＤ
 *
 * @return	ワーク
 */
//--------------------------------------------------------------------------------------------
TIMEICON_WORK * TIMEICON_Create(
									GFL_TCBSYS * tcbsys, GFL_BMPWIN * msg_win, u8 clear_color, u8 wait, HEAPID heapID )
{
	TIMEICON_WORK * wk = GFL_HEAP_AllocMemory( heapID, sizeof(TIMEICON_WORK) );

	TIMEICON_CreateCore( wk , msg_win , clear_color , wait , heapID );

	if( tcbsys != NULL ){
		 wk->tcb = GFL_TCB_AddTask( tcbsys, MainTask, wk, 0 );
	}else{
		 wk->tcb = NULL;
	}
  wk->tcbl = NULL;
	wk->flg = TRUE;

	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タイマーアイコン処理追加 ( TCBL )
 *
 * @param		tcblsys				GFL_TCBLSYS
 * @param		msg_win				メッセージを表示しているウィンドウ
 * @param		clear_color		塗りつぶしカラー
 * @param		wait					アニメウェイト
 * @param		heapID				ヒープＩＤ
 *
 * @return	ワーク
 */
//--------------------------------------------------------------------------------------------
TIMEICON_WORK * TIMEICON_CreateTcbl(
									GFL_TCBLSYS * tcblsys, GFL_BMPWIN * msg_win, u8 clear_color, u8 wait , HEAPID heapID )
{
  GFL_TCBL *tcblWk = GFL_TCBL_Create( tcblsys, MainTaskTcbl, sizeof(TIMEICON_WORK), 0 );
  TIMEICON_WORK * wk = GFL_TCBL_GetWork( tcblWk );
	
	TIMEICON_CreateCore( wk , msg_win , clear_color , wait , heapID );

  wk->tcbl = tcblWk;
  wk->tcb = NULL;
	wk->flg = TRUE;

	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		初期化コア
 *
 * @param		wk						ワーク
 * @param		msg_win				メッセージを表示しているウィンドウ
 * @param		clear_color		塗りつぶしカラー
 * @param		wait					アニメウェイト
 * @param		heapID				ヒープＩＤ
 *
 * @return	ワーク
 */
//--------------------------------------------------------------------------------------------
static void TIMEICON_CreateCore(
							TIMEICON_WORK * wk , GFL_BMPWIN * msg_win, u8 clear_color, u8 wait, HEAPID heapID )
{
	wk->heapID = heapID;
	wk->col    = clear_color;
	wk->cnt    = 0;
	wk->wait   = wait;
	wk->anm    = GFUser_GetPublicRand( TIMEICON_ANM_MAX );

	wk->src = GFL_BMP_LoadCharacter(
							ARCID_TIMEICON_GRA, NARC_time_icon_gra_time_icon_lz_NCGR, TRUE, wk->heapID );
	wk->win = GFL_BMPWIN_Create(
							GFL_BMPWIN_GetFrame(msg_win),
							GFL_BMPWIN_GetPosX(msg_win)+GFL_BMPWIN_GetSizeX(msg_win)-TIMEICON_SX,
							GFL_BMPWIN_GetPosY(msg_win)+GFL_BMPWIN_GetSizeY(msg_win)-TIMEICON_SY,
							TIMEICON_SX, TIMEICON_SY,
							GFL_BMPWIN_GetPalette(msg_win),
							GFL_BMP_CHRAREA_GET_B );

	GFL_BMPWIN_MakeScreen( wk->win );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(wk->win) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タイマーアイコン処理追加（簡易版）
 *
 * @param		heapID				ヒープＩＤ
 *
 * @return	ワーク
 *
 * @li	ワークを作成して、キャラの読み込みを行います
 * @li	TCBの使用を想定してます
 */
//--------------------------------------------------------------------------------------------
TIMEICON_WORK * TIMEICON_CreateEz( HEAPID heapID )
{
	TIMEICON_WORK * wk = GFL_HEAP_AllocMemory( heapID, sizeof(TIMEICON_WORK) );

	wk->heapID = heapID;

	wk->src = GFL_BMP_LoadCharacter(
							ARCID_TIMEICON_GRA, NARC_time_icon_gra_time_icon_lz_NCGR, TRUE, wk->heapID );

	wk->win  = NULL;
  wk->tcb  = NULL;
  wk->tcbl = NULL;

	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タイマーアイコン処理開始
 *
 * @param		wk						ワーク
 * @param		tcbsys				GFL_TCBSYS
 * @param		msg_win				メッセージを表示しているウィンドウ
 * @param		clear_color		塗りつぶしカラー
 * @param		wait					アニメウェイト
 *
 * @return	none
 *
 * @li	TIMEICON_CreateEz()で初期化した場合のみ使用
 */
//--------------------------------------------------------------------------------------------
void TIMEICON_StartEz( TIMEICON_WORK * wk, GFL_TCBSYS * tcbsys, GFL_BMPWIN * msg_win, u8 clear_color, u8 wait )
{
	wk->col    = clear_color;
	wk->cnt    = 0;
	wk->wait   = wait;
	wk->anm    = GFUser_GetPublicRand( TIMEICON_ANM_MAX );

	wk->win = GFL_BMPWIN_Create(
							GFL_BMPWIN_GetFrame(msg_win),
							GFL_BMPWIN_GetPosX(msg_win)+GFL_BMPWIN_GetSizeX(msg_win)-TIMEICON_SX,
							GFL_BMPWIN_GetPosY(msg_win)+GFL_BMPWIN_GetSizeY(msg_win)-TIMEICON_SY,
							TIMEICON_SX, TIMEICON_SY,
							GFL_BMPWIN_GetPalette(msg_win),
							GFL_BMP_CHRAREA_GET_B );

	GFL_BMPWIN_MakeScreen( wk->win );
	GFL_BG_LoadScreenV_Req( GFL_BMPWIN_GetFrame(wk->win) );

  wk->tcbl = NULL;
  wk->tcb = GFL_TCB_AddTask( tcbsys, MainTask, wk, 0 );
	wk->flg = TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タイマーアイコン処理削除
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void TIMEICON_Exit( TIMEICON_WORK * wk )
{
	// Ezで初期化しただけの場合
	if( wk->tcb == NULL && wk->tcbl == NULL ){
		GFL_BMP_Delete( wk->src );
  	GFL_HEAP_FreeMemory( wk );
	// 通常
	}else{
		wk->flg = FALSE;
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win), wk->col );
		GFL_BMPWIN_TransVramCharacter( wk->win );
		GFL_BMPWIN_Delete( wk->win );
		GFL_BMP_Delete( wk->src );
		// TCB
		if( wk->tcb != NULL ){
			GFL_TCB_DeleteTask( wk->tcb );
	  	GFL_HEAP_FreeMemory( wk );
		// TCBL
	  }else if( wk->tcbl != NULL ){
			GFL_TCBL_Delete( wk->tcbl );
	  }
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインタスク ( TCB )
 *
 * @param		tcb			GFL_TCB
 * @param		work		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainTask( GFL_TCB * tcb, void * work )
{
	TIMEICON_Main( work );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインタスク ( TCBL )
 *
 * @param		tcb			GFL_TCBL
 * @param		work		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void MainTaskTcbl(  GFL_TCBL * tcbl, void * work )
{
	TIMEICON_Main( work );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メイン処理
 *
 * @param		wk		ワーク
 *
 * @return	none
 *
 * @li	セーブ消去など、TCBが動かない場合のみ、外部から直接呼んでください。
 */
//--------------------------------------------------------------------------------------------
void TIMEICON_Main( TIMEICON_WORK * wk )
{
	if( wk->flg == FALSE ){ return; }

	if( wk->cnt == 0 ){
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->win), wk->col );
		GFL_BMP_Print(
			wk->src,
			GFL_BMPWIN_GetBmp(wk->win),
			0, TIMEICON_DOT_SY * wk->anm,
			0, 0,
      TIMEICON_DOT_SX, TIMEICON_DOT_SY,
			0 );
		GFL_BMPWIN_TransVramCharacter( wk->win );
		wk->cnt = wk->wait;
		wk->anm++;
		if( wk->anm == TIMEICON_ANM_MAX ){
			wk->anm = 0;
		}
	}else{
		wk->cnt--;
	}
}
