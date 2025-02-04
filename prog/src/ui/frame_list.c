//============================================================================================
/**
 * @file		frame_list.c
 * @brief		ＢＧフレーム全体を使用したリスト処理
 * @author	Hiroyuki Nakamura
 * @date		10.01.23
 *
 *	モジュール名：FRAMELIST
 *
 *	・１画面につき、ＢＧ１面を占有します（スクロールするため）
 *	・内部でBMPWINを確保しているため、BMPWINの初期化後にリストを作成してください
 */
//============================================================================================
#include <gflib.h>

#include "system/bgwinfrm.h"
#include "system/blink_palanm.h"
#include "system/scroll_bar.h"
#include "sound/pm_sndsys.h"

#include "ui/frame_list.h"


//============================================================================================
//	定数定義
//============================================================================================

// 項目データ
typedef struct {
	u32	type;					// 背景番号
	u32	param;				// 戻り値
}FL_ITEM;

// 背景データ
typedef struct {
	u16 * scrn;
}FL_GRA;

// PRINT_UTIL
typedef struct {
	PRINT_UTIL	util;
}FL_PRINT_UTIL;

// ＢＧフレームリストワーク
struct _FRAMELIST_WORK {
	FRAMELIST_HEADER	hed;				// ヘッダーデータ

	FL_ITEM * item;								// 項目データ

	GFL_UI_TP_HITTBL * touch;			// タッチデータ

	BLINKPALANM_WORK * blink;			// 点滅パレットアニメ

	PRINT_QUE * que;							// プリントキュー
	FL_PRINT_UTIL * printUtil;		// PRINT_UTIL
	BGWINFRM_WORK * wfrm;					// ウィンドウフレーム
	FL_GRA * wfrmGra;							// 背景保存場所
	u8	printMax;									// フレーム数
	u8	printSubPos;							// 上画面に表示するフレーム番号開始位置
	s8	putFrameMain;							// 配置基準フレーム（メイン）
	s8	putFrameSub;							// 配置基準フレーム（サブ）
	u8	putFrameMaxMain;					// 実際に配置したフレーム数（メイン）
	u8	putFrameMaxSub;						// 実際に配置したフレーム数（サブ）

	s8	subFramePy;				// サブ画面に配置するフレームのＹ補正値

	u16	listMax;					// リスト登録数

	u16	mainSeq;					// メインシーケンス
	u16	nextSeq;					// 復帰シーケンス

	u32	nowTpy;						// 現在のタッチＹ座標
	u32	oldTpy;						// 前回のタッチＹ座標

	s16	listPos;					// カーソル表示位置
	u16	listPosMax;				// カーソル移動最大値
	s16	listOldPos;				// 前回のカーソル位置

	s16	listScroll;				// リストスクロール値
	u16	listScrollMax;		// リストスクロール最大値

	s8	listBgScoll;				// ＢＧスクロール値
	s8	listBgScrollSpeed;	// ＢＧスクロール速度
	u8	listBgScrollMax;		// ＢＧスクロール回数
	u8	listBgScrollCount;	// ＢＧスクロールカウンタ

	BOOL scrollSE;					// スクロール中のＳＥ再生フラグ

	BOOL autoScroll;				// 自動スクロールフラグ

	GFL_UI_TP_HITTBL	railHit[2];		// レール移動用タッチテーブル
	u8	railHitPos;									// レールタッチテーブル位置
	u8	railTop;										// レール最上部のＹ座標
	u8	railBottom;									// レール最下部のＹ座標
	u8	railMode;									// レール動作中か
	u32	railInitY;									// レール動作開始時にタッチされたＹ座標（処理が１回は走るように）

	u8	slideSeq;
	s8	slideVec;
	u8	slideReqCount:5;
	u8	slideTouchFlag:1;
	u8	slideAutoStop:1;
	u8	slideAutoRepeat:1;
	u8	slidePos;						// スライド基準項目位置
	u8	slidePosTmp;				// スライド基準項目位置
	u8	slideCnt;						// スライド処理テーブル位置

	u32	slideStartTpy;			// 現在のタッチＹ座標
	u32	slideOldTpy;				// 前回のタッチＹ座標

	u8	keyRepeat;					// キーリピートカウンタ
	u8	keyRepPos;					// キーリピート処理テーブル位置
	u8	listWait;						// キーリピートウェイト

	HEAPID	heapID;								// ヒープＩＤ
};

// メインシーケンス
enum {
	MAINSEQ_MAIN = 0,
	MAINSEQ_SCROLL,
	MAINSEQ_RAIL,
	MAINSEQ_SLIDE,
	MAINSEQ_WAIT,
};

// コマンド
enum {
	COMMAND_CURSOR_ON = 0,					// カーソル表示
	COMMAND_CURSOR_MOVE,						// カーソル移動

	COMMAND_SCROLL_UP,							// スクロール：上
	COMMAND_SCROLL_DOWN,						// スクロール：下

	COMMAND_SCROLL_UP_1P,						// スクロール：１ページ上
	COMMAND_SCROLL_DOWN_1P,					// スクロール：１ページ下

	COMMAND_SCROLL_UP_1P_NONE,			// ページ数が足りなくて１ページ上にスクロールできなかった
	COMMAND_SCROLL_DOWN_1P_NONE,		// ページ数が足りなくて１ページ下にスクロールできなかった

	COMMAND_LIST_TOP,								// リスト一番上まで移動
	COMMAND_LIST_BOTTOM,						// リスト一番下まで移動

	COMMAND_RAIL,										// レール移動

	COMMAND_SLIDE,									// スライド処理へ

	COMMAND_SELECT,									// 選択

	COMMAND_ERROR,									// コマンド無効
	COMMAND_NONE,										// 動作なし
};


// ＢＧスクリーンキャラサイズ（画面表示範囲）
#define	BGSCRN_CHR_SIZE_X		( 32 )
#define	BGSCRN_CHR_SIZE_Y		( 24 )

#define	PALCHG_NONE		( 0xff )			// パレットチェンジ無効

#define	KEYWAIT_LIST_MOVE					( 8 )			// キー入力後に次の入力を許可するウェイト（カーソル移動後）
#define	KEYWAIT_LIST_SCROLL				( 0 )			// キー入力後に次の入力を許可するウェイト（スクロール後）
#define	KEYWAIT_LIST_PAGE_MOVE		( 4 )			// キー入力後に次の入力を許可するウェイト（ページ移動後）

#define	RAIL_AREA_X				( 0 )				// レール処理のタッチ範囲Ｘ補正値
#define	RAIL_AREA_Y				( 32 )			// レール処理のタッチ範囲Ｙ補正値
#define	RAIL_AREA_X_MAX		( 255 )			// レール処理のタッチ範囲Ｘ最大値
#define	RAIL_AREA_Y_MAX		( 191 )			// レール処理のタッチ範囲Ｙ最大値

// キー・タッチ切り替えに対応したボタン
#define	KEY_TOUCH_CHG_TRG		( PAD_KEY_UP|PAD_KEY_DOWN|PAD_KEY_LEFT|PAD_KEY_RIGHT|PAD_BUTTON_DECIDE|PAD_BUTTON_L|PAD_BUTTON_R )

#define	RAIL_INIT_COMP		( 0xffffffff )		// レール処理１回目実行済み


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static void InitListDraw( FRAMELIST_WORK * wk, BOOL flg );
static void InitListData( FRAMELIST_WORK * wk );

static u32 MoveListMain( FRAMELIST_WORK * wk );
static void SetListScrollSpeed( FRAMELIST_WORK * wk );

static void InitListScroll( FRAMELIST_WORK * wk, s8 vec, u8 max, s16 next, BOOL se );
static BOOL MainListScroll( FRAMELIST_WORK * wk );

static void InitRailMove( FRAMELIST_WORK * wk, int pos );
static BOOL MainRailMove( FRAMELIST_WORK * wk );

static void InitSlideMove( FRAMELIST_WORK * wk, int pos );
static BOOL MainSlideMove( FRAMELIST_WORK * wk );
static BOOL SetSlideScroll( FRAMELIST_WORK * wk, int pos );
static BOOL InitSledeAutoScroll( FRAMELIST_WORK * wk );
static BOOL CheckSledeAutoScrollRepeat( FRAMELIST_WORK * wk );

static void WriteItemFrame( FRAMELIST_WORK * wk, u32 itemNum, u32 frmNum );
static void PutItemFrame( FRAMELIST_WORK * wk, u32 idx, s8 py );
static s8 GetItemFramePosY( FRAMELIST_WORK * wk, s8 vec );
static s8 GetNextFrameNum( s8 now, s8 max, s8 vec );
static void DrawListItemMain( FRAMELIST_WORK * wk, s32 pos, s8 py );
static void DrawListItemSub( FRAMELIST_WORK * wk, s32 pos, s8 py );
static void ChangeCursorPosPalette( FRAMELIST_WORK * wk, u16 now, u16 old );
static void PrintTransMain( FRAMELIST_WORK * wk );


//============================================================================================
//	グローバル
//============================================================================================

// オートスクロールカウンタ
static const u8 AutoScrollCount[] = {
//	128, 64, 32, 16, 8, 4
	32, 24, 16, 10, 6, 4
};

// オートスクロールに必要な移動ドット幅
static const u8 AutoScrollSpeed[6][6] =
{
	{  16,   8,  6,  4,  4, 4 },		// 0frm
	{  32,  16,  8,  6,  4, 1 },		// 1frm
	{  64,  40, 32, 16,  8, 1 },		// 2frm
	{  96,  80, 64, 48, 32, 1 },		// 3frm
	{ 112,  96, 80, 64, 48, 1 },		// 4frm
	{ 128, 112, 96, 80, 64, 1 },		// 5frm
};



//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧフレームリスト作成
 *
 * @param		hed			ヘッダーデータ
 * @param		heapID	ヒープＩＤ
 *
 * @return	ワーク
 */
//--------------------------------------------------------------------------------------------
FRAMELIST_WORK * FRAMELIST_Create( FRAMELIST_HEADER * hed, HEAPID heapID )
{
	FRAMELIST_WORK * wk;
	u32	i;
	
	wk = GFL_HEAP_AllocClearMemory( heapID, sizeof(FRAMELIST_WORK) );

	wk->hed    = *hed;
	wk->heapID = heapID;

	wk->nowTpy = 0xffffffff;
	wk->oldTpy = 0xffffffff;

	// プリントキュー作成
//	wk->que  = PRINTSYS_QUE_Create( heapID );
	wk->que  = PRINTSYS_QUE_CreateEx( 2048, heapID );
	// 点滅アニメ作成
	wk->blink = BLINKPALANM_Create( wk->hed.selPal*16, 16, wk->hed.mainBG, wk->heapID );

	// 項目データ領域確保
	wk->item = GFL_HEAP_AllocClearMemory( wk->heapID, sizeof(FL_ITEM)*wk->hed.itemMax );

	// 背景グラフィック領域確保
	if( wk->hed.graMax != 0 ){
		u32	i;
		wk->wfrmGra = GFL_HEAP_AllocMemory( wk->heapID, sizeof(FL_GRA)*wk->hed.graMax );
		for( i=0; i<wk->hed.graMax; i++ ){
			wk->wfrmGra[i].scrn = GFL_HEAP_AllocMemory( wk->heapID, wk->hed.itemSizX*wk->hed.itemSizY*2 );
		}
	}

	// BGWINFRM作成 & BMPWIN作成
	wk->printMax = BGSCRN_CHR_SIZE_Y / wk->hed.itemSizY + 2;		// 最大表示フレーム数＋補正分＋書き換え分
	wk->printSubPos = wk->printMax;
	if( wk->hed.subBG != FRAMELIST_SUB_BG_NONE ){
		wk->printMax *= 2;
	}
	wk->wfrm = BGWINFRM_Create( BGWINFRM_TRANS_VBLANK, wk->printMax, wk->heapID );

	wk->printUtil = GFL_HEAP_AllocMemory( wk->heapID, sizeof(FL_PRINT_UTIL)*wk->printMax );

	for( i=0; i<wk->printMax; i++ ){
		if( wk->hed.subBG != FRAMELIST_SUB_BG_NONE && wk->printSubPos <= i ){
			BGWINFRM_Add( wk->wfrm, i, wk->hed.subBG, wk->hed.itemSizX, wk->hed.itemSizY );
			wk->printUtil[i].util.win = GFL_BMPWIN_Create(
																		wk->hed.subBG,
																		wk->hed.bmpPosX, wk->hed.bmpPosY,
																		wk->hed.bmpSizX, wk->hed.bmpSizY,
																		wk->hed.bmpPal, GFL_BMP_CHRAREA_GET_B );
		}else{
			BGWINFRM_Add( wk->wfrm, i, wk->hed.mainBG, wk->hed.itemSizX, wk->hed.itemSizY );
			wk->printUtil[i].util.win = GFL_BMPWIN_Create(
																		wk->hed.mainBG,
																		wk->hed.bmpPosX, wk->hed.bmpPosY,
																		wk->hed.bmpSizX, wk->hed.bmpSizY,
																		wk->hed.bmpPal, GFL_BMP_CHRAREA_GET_B );
		}
	}

	wk->subFramePy = BGSCRN_CHR_SIZE_Y;
	while( 1 ){
		wk->subFramePy -= wk->hed.itemSizY;
		if( wk->subFramePy <= 0 ){
			break;
		}
	}

	// タッチデータ作成
	if( wk->hed.touch != NULL ){
		u32	max = 0;
		i = 0;
		while( 1 ){
			if( wk->hed.touch[max].tbl.rect.top == GFL_UI_TP_HIT_END ){
				max++;
				break;
			}
			max++;
		}
		wk->touch = GFL_HEAP_AllocMemory( wk->heapID, sizeof(GFL_UI_TP_HITTBL)*max );
		for( i=0; i<max; i++ ){
			GFL_STD_MemCopy32( &wk->hed.touch[i].tbl, &wk->touch[i], sizeof(GFL_UI_TP_HITTBL) );
		}
	}

	return wk;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧフレームリスト削除
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_Exit( FRAMELIST_WORK * wk )
{
	u32	i;

  GF_ASSERT( wk );

	// タッチデータ削除
	GFL_HEAP_FreeMemory( wk->touch );

	// BMPWIN削除
	for( i=0; i<wk->printMax; i++ ){
		GFL_BMPWIN_Delete( wk->printUtil[i].util.win );
	}
	GFL_HEAP_FreeMemory( wk->printUtil );

	// BGWINFRM削除
	BGWINFRM_Exit( wk->wfrm );

	// 背景グラフィック削除
	if( wk->hed.graMax != 0 ){
		for( i=0; i<wk->hed.graMax; i++ ){
			GFL_HEAP_FreeMemory( wk->wfrmGra[i].scrn );
		}
		GFL_HEAP_FreeMemory( wk->wfrmGra );
	}

	// 項目データ削除
	GFL_HEAP_FreeMemory( wk->item );

	// 点滅アニメ削除
	BLINKPALANM_Exit( wk->blink );
	// プリントキュー削除
	PRINTSYS_QUE_Delete( wk->que );

	// ＢＧフレームリストワーク削除
	GFL_HEAP_FreeMemory( wk );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目追加
 *
 * @param		wk			ワーク
 * @param		type		背景番号
 * @param		param		パラメータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_AddItem( FRAMELIST_WORK * wk, u32 type, u32 param )
{
  GF_ASSERT( wk );
  GF_ASSERT( wk->hed.itemMax > wk->listMax );
  GF_ASSERT( wk->hed.graMax > type );

	wk->item[wk->listMax].type  = type;
	wk->item[wk->listMax].param = param;
	wk->listMax++;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		背景グラフィック読み込み
 *
 * @param		wk				ワーク
 * @param		ah				アーカイブハンドル
 * @param		dataIdx		データインデックス
 * @param		comp			圧縮フラグ TRUE = 圧縮
 * @param		frameNum	背景番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_LoadFrameGraphicAH( FRAMELIST_WORK * wk, ARCHANDLE * ah, u32 dataIdx, BOOL comp, u32 frameNum )
{
	NNSG2dScreenData * scrn;
	void * buf;

  GF_ASSERT( wk );
  GF_ASSERT( wk->hed.graMax > frameNum );

	buf = GFL_ARCHDL_UTIL_LoadScreen( ah, dataIdx, comp, &scrn, wk->heapID );

	GFL_STD_MemCopy16(
		(const void *)scrn->rawData,
		wk->wfrmGra[frameNum].scrn,
		wk->hed.itemSizX*wk->hed.itemSizY*2 );

	GFL_HEAP_FreeMemory( buf );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソルパレットアニメ用データ読み込み
 *
 * @param		wk				ワーク
 * @param		ah				アーカイブハンドル
 * @param		dataIdx		データインデックス
 * @param		startPal	パレット１ 0〜15
 * @param		endPal		パレット２ 0〜15
 *
 * @return	ワーク
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_LoadBlinkPalette( FRAMELIST_WORK * wk, ARCHANDLE * ah, u32 dataIdx, u32 startPal, u32 endPal )
{
  GF_ASSERT( wk );

	BLINKPALANM_SetPalBufferArcHDL( wk->blink, ah, dataIdx, startPal*16, endPal*16 );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧフレームリスト初期描画
 *
 * @param		wk		ワーク
 *
 * @retval	"TRUE = 初期化中"
 * @retval	"FALSE = 初期化終了"
 */
//--------------------------------------------------------------------------------------------
BOOL FRAMELIST_Init( FRAMELIST_WORK * wk )
{
  GF_ASSERT( wk );

	switch( wk->mainSeq ){
	case 0:
		InitListData( wk );
		InitListDraw( wk, FALSE );
		wk->mainSeq++;
		break;

	case 1:
	  if( PRINTSYS_QUE_IsFinished( wk->que ) == TRUE ){
			wk->mainSeq = 0;
			return FALSE;
		}
		break;

  default:
		GF_ASSERT(0);
	}

	PrintTransMain( wk );

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＧフレームリストメイン処理
 *
 * @param		wk		ワーク
 *
 * @return	動作結果
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_Main( FRAMELIST_WORK * wk )
{
	u32	ret;
	u32	x;
  
  GF_ASSERT( wk );
	
	ret = FRAMELIST_RET_NONE;

	if( GFL_UI_TP_GetPointCont( &x, &wk->nowTpy ) == FALSE ){
		wk->nowTpy = 0xffffffff;
		wk->slideTouchFlag = 0;
	}

	switch( wk->mainSeq ){
	case MAINSEQ_MAIN:
		ret = MoveListMain( wk );
		break;

	case MAINSEQ_SCROLL:
		if( MainListScroll( wk ) == TRUE ){
			ret = FRAMELIST_RET_SCROLL;
		}
		break;

	case MAINSEQ_RAIL:
		if( MainRailMove( wk ) == TRUE ){
			ret = FRAMELIST_RET_RAIL;
		}
		break;

	case MAINSEQ_SLIDE:
		if( MainSlideMove( wk ) == TRUE ){
			ret = FRAMELIST_RET_SLIDE;
		}
		break;

	case MAINSEQ_WAIT:
		// Ａボタンが押されたら選択を返す
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY ){
			if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
				ret = wk->listPos;
			}
		}
		if( wk->listWait == 0 ){
			wk->mainSeq = MAINSEQ_MAIN;
		}else{
			wk->listWait--;
		}
		break;

  default:
		GF_ASSERT(0);
	}

	wk->oldTpy = wk->nowTpy;

//	BGWINFRM_MoveMain( wk->wfrm );
	BLINKPALANM_Main( wk->blink );
	PrintTransMain( wk );

	return ret;
}


//============================================================================================
//	初期描画
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストデータ初期化
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitListData( FRAMELIST_WORK * wk )
{
	// カーソル移動最大値設定
	if( wk->hed.posMax > wk->listMax ){
		wk->listPosMax = wk->listMax;
	}else{
		wk->listPosMax = wk->hed.posMax;
	}

	// スクロール最大値設定
	if( wk->listMax < wk->hed.posMax ){
		wk->listScrollMax = 0;
	}else{
		wk->listScrollMax = wk->listMax - wk->hed.posMax;
	}

	// カーソル位置補正
	if( wk->hed.initPos > wk->listPosMax ||
			wk->hed.initScroll > wk->listScrollMax ){
		wk->listPos    = 0;
		wk->listScroll = 0;
	}else{
		wk->listPos    = wk->hed.initPos;
		wk->listScroll = wk->hed.initScroll;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リスト初期描画
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitListDraw( FRAMELIST_WORK * wk, BOOL flg )
{
	s16	pos;
	s8	py;

	GFL_BG_SetScrollReq( wk->hed.mainBG, GFL_BG_SCROLL_Y_SET, 0 );
	wk->listBgScoll = 0;

	pos = wk->listScroll;
	py  = wk->hed.itemPosY;
	while( 1 ){
		if( pos == 0 || py <= 0 ){
			break;
		}
		pos--;
		py -= wk->hed.itemSizY;
	}

	wk->putFrameMain = 0;

	while( 1 ){
//		if( py >= BGSCRN_CHR_SIZE_Y || pos >= wk->listMax ){
		if( py >= BGSCRN_CHR_SIZE_Y ){
			break;
		}

		DrawListItemMain( wk, pos, py );
		wk->putFrameMain++;

		pos++;
		py += wk->hed.itemSizY;
	}

	wk->putFrameMaxMain = wk->putFrameMain;

	// サブ画面
	if( wk->hed.subBG == FRAMELIST_SUB_BG_NONE ){ return; }

	GFL_BG_SetScrollReq( wk->hed.subBG, GFL_BG_SCROLL_Y_SET, 0 );

	py  = BGSCRN_CHR_SIZE_Y + wk->hed.itemPosY - wk->hed.itemSizY;
	pos = wk->listScroll - 1;
	while( 1 ){
		if( py <= 0 ){
			break;
		}
		pos--;
		py -= wk->hed.itemSizY;
	}

	wk->putFrameSub = 0;

	while( 1 ){
		if( py >= BGSCRN_CHR_SIZE_Y ){
			break;
		}

		DrawListItemSub( wk, pos, py );
		wk->putFrameSub++;

		pos++;
		py += wk->hed.itemSizY;
	}

	wk->putFrameMaxSub = wk->putFrameSub;

	ChangeCursorPosPalette( wk, wk->listPos, PALCHG_NONE );
	wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, flg );
}


//============================================================================================
//	メインコントロール
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチチェック
 *
 * @param		wk		ワーク
 *
 * @return	タッチに対応したコマンド
 */
//--------------------------------------------------------------------------------------------
static int MoveListTouch( FRAMELIST_WORK * wk )
{
	int	ret;
	int	mode;

	if( wk->touch == NULL ){
		return COMMAND_NONE;
	}

	ret = GFL_UI_TP_HitTrg( wk->touch );

	if( ret == GFL_UI_TP_HIT_NONE ){
		return COMMAND_NONE;
	}

	mode = GFL_UI_CheckTouchOrKey();
	GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );

	switch( wk->hed.touch[ret].prm ){
	case FRAMELIST_TOUCH_PARAM_ITEM:					// 項目
		if( ret >= wk->listPosMax ){
			return COMMAND_ERROR;					// コマンド無効
		}
		wk->listOldPos = wk->listPos;
		wk->listPos = ret;
//		return COMMAND_CURSOR_MOVE;		// カーソル移動
		return COMMAND_SELECT;					// 選択

	case FRAMELIST_TOUCH_PARAM_SLIDE:
		if( ret >= wk->listPosMax ){
			return COMMAND_ERROR;					// コマンド無効
		}
		wk->listOldPos = wk->listPos;
		wk->listPos = ret;
		return COMMAND_SLIDE;

	case FRAMELIST_TOUCH_PARAM_RAIL:					// レール
		if( wk->listMax > wk->listPosMax ){
			wk->railHitPos = ret;
			wk->listOldPos = wk->listPos;
//			wk->listPos = 0;
			return COMMAND_RAIL;
		}
		GFL_UI_SetTouchOrKey( mode );
		break;

	case FRAMELIST_TOUCH_PARAM_UP:						// 上移動
		if( wk->listScroll != 0 ){
			wk->keyRepPos = 0;
			return COMMAND_SCROLL_UP;		// スクロール：上
		}
		break;

	case FRAMELIST_TOUCH_PARAM_DOWN:					// 下移動
		if( wk->listScroll < wk->listScrollMax ){
			wk->keyRepPos = 0;
			return COMMAND_SCROLL_DOWN;	// スクロール：下
		}
		break;

	case FRAMELIST_TOUCH_PARAM_PAGE_UP:				// ページ上移動
		if( wk->listScroll != 0 ){
			return COMMAND_SCROLL_UP_1P;	// スクロール：１ページ上
		}else if( wk->listPos != 0 ){
			wk->listOldPos = wk->listPos;
			wk->listPos = 0;
//			return COMMAND_CURSOR_MOVE;		// カーソル移動
			return COMMAND_SCROLL_UP_1P_NONE;			// ページ数が足りなくて１ページ上にスクロールできなかった
		}
		break;

	case FRAMELIST_TOUCH_PARAM_PAGE_DOWN:			// ページ下移動
		if( wk->listScroll < wk->listScrollMax ){
			return COMMAND_SCROLL_DOWN_1P;	// スクロール：１ページ下
		}else if( wk->listPos < (wk->listPosMax-1) ){
			wk->listOldPos = wk->listPos;
			wk->listPos = wk->listPosMax - 1;
//			return COMMAND_CURSOR_MOVE;		// カーソル移動
			return COMMAND_SCROLL_DOWN_1P_NONE;		// ページ数が足りなくて１ページ下にスクロールできなかった
		}
		break;

	case FRAMELIST_TOUCH_PARAM_LIST_TOP:			// リスト最上位へ
		if( wk->listPos != 0 || wk->listScroll != 0 ){
			return COMMAND_LIST_TOP;		// リスト一番上まで移動
		}
		break;

	case FRAMELIST_TOUCH_PARAM_LIST_BOTTOM:		// リスト最下位へ
		if( wk->listPos != (wk->listPosMax-1) || wk->listScroll != wk->listScrollMax ){
			return COMMAND_LIST_BOTTOM;		// リスト一番下まで移動
		}
		break;

  default:
		GF_ASSERT(0);
	}

	return COMMAND_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		キーチェック
 *
 * @param		wk		ワーク
 *
 * @return	キーに対応したコマンド
 */
//--------------------------------------------------------------------------------------------
static int MoveListKey( FRAMELIST_WORK * wk )
{
	if( GFL_UI_KEY_GetTrg() & KEY_TOUCH_CHG_TRG ){
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
			GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
			wk->listOldPos = PALCHG_NONE;
			wk->listWait = KEYWAIT_LIST_MOVE;
			return COMMAND_CURSOR_ON;	// カーソル表示
		}
	}

	if( !(GFL_UI_KEY_GetCont()) ){
		wk->keyRepeat = 0;
		wk->keyRepPos = 6;
	}

	if( GFL_UI_KEY_GetCont() & PAD_KEY_UP  ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		if( wk->listPos != 0 ){
			wk->listOldPos = wk->listPos;
			wk->listPos--;
			wk->listWait = KEYWAIT_LIST_MOVE;
			return COMMAND_CURSOR_MOVE;	// カーソル移動
		}else if( wk->listScroll != 0 ){
			wk->listWait = KEYWAIT_LIST_SCROLL;
			SetListScrollSpeed( wk );
			return COMMAND_SCROLL_UP;		// スクロール：上
		}
		return COMMAND_NONE;					// 動作なし
	}

	if( GFL_UI_KEY_GetCont() & PAD_KEY_DOWN ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		if( wk->listPos < (wk->listPosMax-1) ){
			wk->listOldPos = wk->listPos;
			wk->listPos++;
			wk->listWait = KEYWAIT_LIST_MOVE;
			return COMMAND_CURSOR_MOVE;	// カーソル移動
		}else if( wk->listScroll < wk->listScrollMax ){
			wk->listWait = KEYWAIT_LIST_SCROLL;
			SetListScrollSpeed( wk );
			return COMMAND_SCROLL_DOWN;	// スクロール：下
		}
		return COMMAND_NONE;					// 動作なし
	}

	if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		if( wk->listScroll != 0 ){
			wk->listWait = KEYWAIT_LIST_PAGE_MOVE;
			return COMMAND_SCROLL_UP_1P;	// スクロール：１ページ上
		}else if( wk->listPos != 0 ){
			wk->listOldPos = wk->listPos;
			wk->listPos = 0;
			wk->listWait = KEYWAIT_LIST_PAGE_MOVE;
//			return COMMAND_CURSOR_MOVE;		// カーソル移動
			return COMMAND_SCROLL_UP_1P_NONE;			// ページ数が足りなくて１ページ上にスクロールできなかった
		}
		return COMMAND_NONE;						// 動作なし
	}

	if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		if( wk->listScroll < wk->listScrollMax ){
			wk->listWait = KEYWAIT_LIST_PAGE_MOVE;
			return COMMAND_SCROLL_DOWN_1P;	// スクロール：１ページ下
		}else if( wk->listPos < (wk->listPosMax-1) ){
			wk->listOldPos = wk->listPos;
			wk->listPos = wk->listPosMax - 1;
			wk->listWait = KEYWAIT_LIST_PAGE_MOVE;
//			return COMMAND_CURSOR_MOVE;		// カーソル移動
			return COMMAND_SCROLL_DOWN_1P_NONE;		// ページ数が足りなくて１ページ下にスクロールできなかった
		}
		return COMMAND_NONE;						// 動作なし
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
		return COMMAND_SELECT;					// 選択
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		if( wk->listPos != 0 || wk->listScroll != 0 ){
			return COMMAND_LIST_TOP;		// リスト一番上まで移動
		}
		return COMMAND_NONE;					// 動作なし
	}

	// リストの一番下へ移動
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		if( wk->listPos != (wk->listPosMax-1) || wk->listScroll != wk->listScrollMax ){
			return COMMAND_LIST_BOTTOM;		// リスト一番下まで移動
		}
		return COMMAND_NONE;						// 動作なし
	}

	return COMMAND_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインコントロール
 *
 * @param		wk		ワーク
 *
 * @return	外部へ戻す値
 */
//--------------------------------------------------------------------------------------------
static u32 MoveListMain( FRAMELIST_WORK * wk )
{
	int	ret;

	if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
		return FRAMELIST_RET_NONE;
	}

	ret = MoveListTouch( wk );
	if( ret == COMMAND_NONE ){
		ret = MoveListKey( wk );
	}

	switch( ret ){
	case COMMAND_CURSOR_ON:				// カーソル表示
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		ChangeCursorPosPalette( wk, wk->listPos, wk->listOldPos );
		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
		wk->mainSeq = MAINSEQ_WAIT;
		return FRAMELIST_RET_CURSOR_ON;

	case COMMAND_CURSOR_MOVE:			// カーソル移動
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		ChangeCursorPosPalette( wk, wk->listPos, wk->listOldPos );
		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
		wk->mainSeq = MAINSEQ_WAIT;
		return FRAMELIST_RET_MOVE;

	case COMMAND_SCROLL_UP:				// スクロール：上
//		PMSND_PlaySE( SEQ_SE_SELECT1 );
		InitListScroll( wk, -wk->hed.scrollSpeed[wk->keyRepPos], 1, MAINSEQ_WAIT, TRUE );
		return FRAMELIST_RET_SCROLL;

	case COMMAND_SCROLL_DOWN:			// スクロール：下
//		PMSND_PlaySE( SEQ_SE_SELECT1 );
		InitListScroll( wk, wk->hed.scrollSpeed[wk->keyRepPos], 1, MAINSEQ_WAIT, TRUE );
		return FRAMELIST_RET_SCROLL;

	case COMMAND_SCROLL_UP_1P:		// スクロール：１ページ上
		{
			s16	cnt = wk->listPosMax;
			wk->listOldPos = wk->listPos;
			if( ( wk->listScroll - cnt ) < 0 ){
				wk->listPos = 0;
				cnt = wk->listScroll;
			}
			ChangeCursorPosPalette( wk, PALCHG_NONE, wk->listOldPos );
			InitListScroll( wk, -wk->hed.scrollSpeed[0], cnt, MAINSEQ_WAIT, FALSE );
		}
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		return FRAMELIST_RET_PAGE_UP;

	case COMMAND_SCROLL_DOWN_1P:	// スクロール：１ページ下
		{
			s16	cnt = wk->listPosMax;
			wk->listOldPos = wk->listPos;
			if( ( wk->listScroll + cnt ) > wk->listScrollMax ){
				wk->listPos = wk->listPosMax - 1;
				cnt = wk->listScrollMax - wk->listScroll;
			}
			ChangeCursorPosPalette( wk, PALCHG_NONE, wk->listOldPos );
			InitListScroll( wk, wk->hed.scrollSpeed[0], cnt, MAINSEQ_WAIT, FALSE );
		}
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		return FRAMELIST_RET_PAGE_DOWN;

	case COMMAND_SCROLL_UP_1P_NONE:			// ページ数が足りなくて１ページ上にスクロールできなかった
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		ChangeCursorPosPalette( wk, wk->listPos, wk->listOldPos );
		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
		wk->mainSeq = MAINSEQ_WAIT;
		return FRAMELIST_RET_PAGE_UP_NONE;

	case COMMAND_SCROLL_DOWN_1P_NONE:		// ページ数が足りなくて１ページ下にスクロールできなかった
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		ChangeCursorPosPalette( wk, wk->listPos, wk->listOldPos );
		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
		wk->mainSeq = MAINSEQ_WAIT;
		return FRAMELIST_RET_PAGE_DOWN_NONE;

	case COMMAND_LIST_TOP:				// リスト一番上まで移動
		wk->listPos = 0;
		wk->listScroll = 0;
		InitListDraw( wk, TRUE );
//		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll );
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		return FRAMELIST_RET_JUMP_TOP;

	case COMMAND_LIST_BOTTOM:			// リスト一番下まで移動
		wk->listPos = wk->listPosMax-1;
		wk->listScroll = wk->listScrollMax;
		InitListDraw( wk, TRUE );
//		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll );
		PMSND_PlaySE( SEQ_SE_SELECT1 );
		return FRAMELIST_RET_JUMP_BOTTOM;

	case COMMAND_RAIL:						// レール移動
		ChangeCursorPosPalette( wk, PALCHG_NONE, wk->listOldPos );
//		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll );
		InitRailMove( wk, wk->railHitPos );
		PMSND_PlaySE( SEQ_SE_SYS_06 );
		return FRAMELIST_RET_RAIL;

	case COMMAND_SLIDE:
		ChangeCursorPosPalette( wk, PALCHG_NONE, wk->listOldPos );
		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
		InitSlideMove( wk, wk->listPos );
		PMSND_PlaySE( SEQ_SE_SYS_06 );
		return FRAMELIST_RET_SLIDE;

	case COMMAND_SELECT:					// 選択
		if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH ){
//			PMSND_PlaySE( SEQ_SE_DECIDE1 );
			ChangeCursorPosPalette( wk, wk->listPos, wk->listOldPos );
			wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
		}
		return wk->listPos;

	case COMMAND_ERROR:						// コマンド無効
	case COMMAND_NONE:						// 動作なし
		break;
  
  default:
		GF_ASSERT(0);
	}

	return FRAMELIST_RET_NONE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロール速度セット
 *
 * @param		wk		ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void SetListScrollSpeed( FRAMELIST_WORK * wk )
{
	if( wk->keyRepeat < 20 ){
		wk->keyRepeat++;
	}
	if( wk->keyRepeat == 20 ){
		wk->keyRepPos = 0;
	}else if( wk->keyRepeat >= 16 ){
		wk->keyRepPos = 1;
	}else if( wk->keyRepeat >= 12 ){
		wk->keyRepPos = 2;
	}else if( wk->keyRepeat >= 8 ){
		wk->keyRepPos = 3;
	}else if( wk->keyRepeat >= 4 ){
		wk->keyRepPos = 4;
	}else{
		wk->keyRepPos = 5;
	}
}


//============================================================================================
//	スクロール関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロールフレーム数作成
 *
 * @param		wk			ワーク
 * @param		speed		スクロール速度
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static u8 MakeScrollCount( FRAMELIST_WORK * wk, s8 speed )
{
	return ( wk->hed.itemSizY * 8 / GFL_STD_Abs(speed) );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		現在の位置からの残りの最大スクロール値を取得
 *
 * @param		wk			ワーク
 * @param		speed		スクロール速度
 *
 * @return	残りの最大スクロール値
 */
//--------------------------------------------------------------------------------------------
static u32 GetListScrollCount( FRAMELIST_WORK * wk, s8 speed )
{
	if( speed < 0 ){
		return wk->listScroll;
	}
	return ( wk->listScrollMax - wk->listScroll );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロール初期設定
 *
 * @param		wk			ワーク
 * @param		speed		スクロール速度
 * @param		max			スクロール数
 * @param		next		スクロール後のシーケンス
 * @param		se			ＳＥ再生フラグ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitListScroll( FRAMELIST_WORK * wk, s8 speed, u8 max, s16 next, BOOL se )
{
	wk->listBgScrollSpeed = speed;
	wk->listBgScrollMax   = max;
	wk->listBgScrollCount = 0;

	wk->nextSeq = next;
	wk->mainSeq = MAINSEQ_SCROLL;

	wk->scrollSE = se;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロールメイン処理
 *
 * @param		wk			ワーク
 *
 * @retval	"TRUE = スクロール中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
static BOOL MainListScroll( FRAMELIST_WORK * wk )
{
	if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
		return TRUE;
	}

	// スライドからの処理
	if( wk->nextSeq == MAINSEQ_SLIDE ){
		// オートスクロール中
		if( wk->autoScroll == TRUE ){
			int	ret = GFL_UI_TP_HitCont( wk->touch );
			// タッチ停止処理
			if( wk->slideAutoStop == 1 ){
				if( wk->slideAutoRepeat == 0 ){
					if( ret == GFL_UI_TP_HIT_NONE ){
						wk->slideVec = wk->slideOldTpy - wk->slideStartTpy;
						if( GFL_STD_Abs(wk->slideVec) >= 16 ){
							wk->slideReqCount = 0;
							wk->slideAutoStop = 0;
							wk->slideAutoRepeat = 1;
						}
					}else{
						if( wk->slideReqCount != 3 ){
							wk->slideStartTpy = wk->nowTpy;
							wk->slideReqCount++;
						}
					}
				}
			}else{
				if( ret != GFL_UI_TP_HIT_NONE ){
					if( wk->hed.touch[ret].prm == FRAMELIST_TOUCH_PARAM_SLIDE ){
						wk->listPos = ret;
						wk->slideAutoStop = 1;
						wk->slideReqCount = 0;
						wk->slideStartTpy = wk->nowTpy;
						wk->slideOldTpy   = wk->nowTpy;
					}
				}
			}
		}
	}

	// 一項目スクロール終了
	if( wk->listBgScrollCount == 0 ){
		// 指定回数スクロール終了
		if( wk->listBgScrollMax == 0 ){
			int	ret = GFL_UI_TP_HitCont( wk->touch );
			if( ret != GFL_UI_TP_HIT_NONE && wk->hed.touch[ret].prm == FRAMELIST_TOUCH_PARAM_SLIDE &&
					wk->slideTouchFlag == 1 &&
					wk->hed.touch[ret].prm == FRAMELIST_TOUCH_PARAM_SLIDE &&
					( ( wk->slideVec < 0 && wk->slidePos < ret ) || ( wk->slideVec > 0 && wk->slidePos > ret ) ) ){
				wk->slideTouchFlag = 0;
				{
					u32	max = GetListScrollCount( wk, wk->listBgScrollSpeed );
					if( max != 0 ){
						wk->listBgScrollMax = GFL_STD_Abs( wk->slidePos - ret );
						if( wk->listBgScrollMax > max ){
							wk->listBgScrollMax = max;
						}
						wk->slidePos = ret;
					}else{
						wk->slideStartTpy = wk->nowTpy;
						wk->slideOldTpy   = wk->nowTpy;
						wk->slideAutoStop = 0;
						ChangeCursorPosPalette( wk, wk->listPos, PALCHG_NONE );
						wk->mainSeq = wk->nextSeq;
						return FALSE;
					}
				}
			}else{
				wk->slideStartTpy = wk->nowTpy;
				wk->slideOldTpy   = wk->nowTpy;
				wk->slideTouchFlag = 0;
				wk->slideAutoStop = 0;
				ChangeCursorPosPalette( wk, wk->listPos, PALCHG_NONE );
				wk->mainSeq = wk->nextSeq;
				return FALSE;
			}
		}

		ChangeCursorPosPalette( wk, PALCHG_NONE, wk->listPos );

		// スライドからの処理
		if( wk->nextSeq == MAINSEQ_SLIDE ){
			// オートスクロール以外
			if( wk->autoScroll == FALSE ){
				if( wk->listBgScrollSpeed < 0 ){
					wk->listPos++;
				}else{
					wk->listPos--;
				}
			}else{
				// リピート
				if( wk->slideAutoRepeat == 1 ){
					if( InitSledeAutoScroll( wk ) == TRUE ){
						wk->slideReqCount = 0;
					}
					wk->slideAutoRepeat = 0;
				}
				// タッチ停止
				if( wk->slideAutoStop == 1 && wk->slideReqCount == 3 ){
					int	ret = GFL_UI_TP_HitCont( wk->touch );
					if( ret != GFL_UI_TP_HIT_NONE ){
						if( wk->hed.touch[ret].prm == FRAMELIST_TOUCH_PARAM_SLIDE ){
							wk->listPos = ret;
//							wk->slideStartTpy = wk->nowTpy;
//							wk->slideOldTpy   = wk->nowTpy;
						}
					}
					wk->autoScroll = FALSE;
					wk->slidePos = wk->listPos;
					wk->slideAutoStop = 0;
					wk->slideReqCount = 0;
					wk->slideStartTpy = wk->nowTpy;
					wk->slideOldTpy   = wk->nowTpy;
					wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
					wk->mainSeq = wk->nextSeq;
					return FALSE;
				}
				// 減速処理
				if( wk->listBgScrollMax == 1 ){
					u32	max = GetListScrollCount( wk, wk->listBgScrollSpeed );
					wk->slideCnt++;
					if( wk->slideCnt != FRAMELIST_SPEED_MAX ){
						if( wk->listBgScrollSpeed < 0 ){
							wk->listBgScrollSpeed = -wk->hed.scrollSpeed[wk->slideCnt];
						}else{
							wk->listBgScrollSpeed = wk->hed.scrollSpeed[wk->slideCnt];
						}
						wk->listBgScrollMax = AutoScrollCount[wk->slideCnt];
					}
					if( wk->listBgScrollMax > max ){
						wk->listBgScrollMax = max;
					}
				}
			}
		}

		wk->listBgScrollCount = MakeScrollCount( wk, wk->listBgScrollSpeed );
		wk->listBgScrollMax--;

		if( wk->listBgScrollSpeed < 0 ){
			wk->listScroll--;
			DrawListItemMain( wk, wk->listScroll, GetItemFramePosY(wk,-1) );
			wk->putFrameMain = GetNextFrameNum( wk->putFrameMain, wk->putFrameMaxMain, -1 );
			DrawListItemSub( wk, wk->listScroll-wk->putFrameMaxSub, GetItemFramePosY(wk,-1)+wk->subFramePy );
			wk->putFrameSub = GetNextFrameNum( wk->putFrameSub, wk->putFrameMaxSub, -1 );
		}else{
			wk->listScroll++;
			DrawListItemMain( wk, wk->listScroll+(wk->putFrameMaxMain-1), GetItemFramePosY(wk,1) );
			wk->putFrameMain = GetNextFrameNum( wk->putFrameMain, wk->putFrameMaxMain, 1 );
			DrawListItemSub( wk, wk->listScroll-1, GetItemFramePosY(wk,1)+wk->subFramePy );
			wk->putFrameSub = GetNextFrameNum( wk->putFrameSub, wk->putFrameMaxSub, 1 );
		}
		wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );

		if( wk->scrollSE == TRUE ){
			if( wk->nextSeq == MAINSEQ_SLIDE || wk->nextSeq == MAINSEQ_RAIL ){
				PMSND_PlaySE( SEQ_SE_SYS_06 );
			}else{
				PMSND_PlaySE( SEQ_SE_SELECT1 );
			}
		}
	}

	wk->listBgScoll += wk->listBgScrollSpeed;
	GFL_BG_SetScrollReq( wk->hed.mainBG, GFL_BG_SCROLL_Y_SET, wk->listBgScoll );
	if( wk->hed.subBG != FRAMELIST_SUB_BG_NONE ){
		GFL_BG_SetScrollReq( wk->hed.subBG, GFL_BG_SCROLL_Y_SET, wk->listBgScoll );
	}
	wk->hed.cbFunc->scroll( wk->hed.cbWork, wk->listBgScrollSpeed );

	wk->listBgScrollCount--;

	return TRUE;
}


//============================================================================================
//	レール関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		レール移動初期化
 *
 * @param		wk			ワーク
 * @param		pos			タッチデータ位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitRailMove( FRAMELIST_WORK * wk, int pos )
{
	wk->railTop    = wk->touch[pos].rect.top;
	wk->railBottom = wk->touch[pos].rect.bottom;

	wk->railHit[0] = wk->touch[pos];
	if( wk->railHit[0].rect.top >= RAIL_AREA_Y ){
		wk->railHit[0].rect.top = wk->railHit[0].rect.top - RAIL_AREA_Y;
	}else{
		wk->railHit[0].rect.top = 0;
	}
	if( ( wk->railHit[0].rect.bottom + RAIL_AREA_Y ) <= RAIL_AREA_Y_MAX  ){
		wk->railHit[0].rect.bottom = wk->railHit[0].rect.bottom + RAIL_AREA_Y;
	}else{
		wk->railHit[0].rect.bottom = RAIL_AREA_Y_MAX;
	}
	if( wk->railHit[0].rect.left >= RAIL_AREA_X ){
		wk->railHit[0].rect.left = wk->railHit[0].rect.left - RAIL_AREA_X;
	}else{
		wk->railHit[0].rect.left = 0;
	}
	if( ( wk->railHit[0].rect.right + RAIL_AREA_X ) <= RAIL_AREA_X_MAX  ){
		wk->railHit[0].rect.right = wk->railHit[0].rect.right + RAIL_AREA_X;
	}else{
		wk->railHit[0].rect.right = RAIL_AREA_X_MAX;
	}

	wk->railHit[1].rect.top = GFL_UI_TP_HIT_END;

	{	// 初期Ｙ座標取得
		u32	x;
		if( GFL_UI_TP_GetPointCont( &x, &wk->railInitY ) == FALSE ){
			wk->railInitY = RAIL_INIT_COMP;
		}
	}
	wk->railMode = TRUE;

	wk->mainSeq = MAINSEQ_RAIL;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		タッチ位置からスクロール値を取得
 *
 * @param		wk			ワーク
 *
 * @return	スクロール値
 */
//--------------------------------------------------------------------------------------------
static u32 GetRailScroll( FRAMELIST_WORK * wk )
{
	u32	x, y;

	if( GFL_UI_TP_GetPointCont( &x, &y ) == FALSE ){
		y = wk->railInitY;
	}
	wk->railInitY	= RAIL_INIT_COMP;

	return SCROLLBAR_GetCount( wk->listScrollMax, y, wk->railTop, wk->railBottom, wk->hed.barSize );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		レールのタッチデータの上下のY座標を取得
 *
 * @param		wk			ワーク
 * @param		ty			上部Y座標
 * @param		by			下部Y座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void GetScrollBarTouchY( FRAMELIST_WORK * wk, u8 * ty, u8 * by )
{
	u8	i = 0;

	while( 1 ){
		if( wk->hed.touch[i].tbl.rect.top == GFL_UI_TP_HIT_END ){
			break;
		}
		if( wk->hed.touch[i].prm == FRAMELIST_TOUCH_PARAM_RAIL ){
			*ty = wk->hed.touch[i].tbl.rect.top;
			*by = wk->hed.touch[i].tbl.rect.bottom;
			break;
		}
		i++;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロールバーの表示Ｙ座標を取得
 *
 * @param		wk			ワーク
 *
 * @return	Ｙ座標
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_GetScrollBarPY( FRAMELIST_WORK * wk )
{
	u8	ty, by;

	GetScrollBarTouchY( wk, &ty, &by );
	return SCROLLBAR_GetPosY( wk->listScrollMax, wk->listScroll, ty, by, wk->hed.barSize );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロールバーの表示Ｙ座標を取得（拡張版）
 *
 * @param		wk			ワーク
 * @param		now_y		現在のＹ座標
 *
 * @return	Ｙ座標
 *
 * @li	レール処理中はタッチＹ座標を返す
 * @li	現在のＹ座標がスクロール幅に収まっていれば、現在のＹ座標を返す
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_GetScrollBarPY2( FRAMELIST_WORK * wk, u32 now_y )
{
	u32	y1, y2;
	u8	ty, by;

	if( wk->railMode == TRUE ){
		if( GFL_UI_TP_HitCont( wk->railHit ) != GFL_UI_TP_HIT_NONE ){
			u32	px, py;
			GFL_UI_TP_GetPointCont( &px, &py );
			return py;
		}else{
			return now_y;
		}
	}

	GetScrollBarTouchY( wk, &ty, &by );
	y1 = SCROLLBAR_GetPosY( wk->listScrollMax, wk->listScroll, ty, by, wk->hed.barSize );
	if( wk->listScrollMax == wk->listScroll ){
		y2 = by;
	}else{
		y2 = SCROLLBAR_GetPosY( wk->listScrollMax, wk->listScroll+1, ty, by, wk->hed.barSize );
	}
	if( now_y >= y1 && now_y < y2 ){
		return now_y;
	}
	return y1;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		レール処理メイン
 *
 * @param		wk			ワーク
 *
 * @retval	"TRUE = 動作中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
static BOOL MainRailMove( FRAMELIST_WORK * wk )
{
	u32	scroll;
	u32	max;

	if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
		return TRUE;
	}

	if( GFL_UI_TP_HitCont( wk->railHit ) == GFL_UI_TP_HIT_NONE && wk->railInitY == RAIL_INIT_COMP ){
		wk->railMode = FALSE;
		wk->mainSeq = MAINSEQ_MAIN;
		return FALSE;
	}

	scroll = GetRailScroll( wk );
	max = GFL_STD_Abs( wk->listScroll-scroll );
	if( max > wk->putFrameMaxMain ){
		max = wk->putFrameMaxMain;
	}

	if( wk->listScroll > scroll ){
		wk->listScroll = scroll+max;
		InitListScroll( wk, -wk->hed.scrollSpeed[0], max, MAINSEQ_RAIL, TRUE );
	}else if( wk->listScroll < scroll ){
		wk->listScroll = scroll-max;
		InitListScroll( wk, wk->hed.scrollSpeed[0], max, MAINSEQ_RAIL, TRUE );
	}

	return TRUE;
}



//============================================================================================
//	スライド処理
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		スライド処理初期化
 *
 * @param		wk			ワーク
 * @param		pos			タッチした項目位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void InitSlideMove( FRAMELIST_WORK * wk, int pos )
{
	wk->slideSeq      = 0;
	wk->slidePos      = pos;
	wk->slideReqCount = 0;
	wk->slideStartTpy = wk->nowTpy;
	wk->slideOldTpy   = wk->nowTpy;

	wk->autoScroll = FALSE;

	wk->mainSeq  = MAINSEQ_SLIDE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スライド処理メイン
 *
 * @param		wk			ワーク
 *
 * @retval	"TRUE = 動作中"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
static BOOL MainSlideMove( FRAMELIST_WORK * wk )
{
	int	ret;
	u32	x, y;

	if( PRINTSYS_QUE_IsFinished( wk->que ) == FALSE ){
		return TRUE;
	}

	ret = GFL_UI_TP_HitCont( wk->touch );

	switch( wk->slideSeq ){
	case 0:
		// タッチされていない
		if( ret == GFL_UI_TP_HIT_NONE ){
			if( wk->slideStartTpy != 0xffffffff && wk->slideOldTpy != 0xffffffff ){
				wk->slideVec = wk->slideOldTpy - wk->slideStartTpy;
				wk->slideReqCount = 0;
				if( InitSledeAutoScroll( wk ) == TRUE ){
					break;
				}
			}
			wk->autoScroll = FALSE;
			wk->mainSeq = MAINSEQ_MAIN;
			return FALSE;
		}
		// スライドエリア範囲外
		if( wk->hed.touch[ret].prm != FRAMELIST_TOUCH_PARAM_SLIDE ){
			if( wk->slideStartTpy != 0xffffffff && wk->slideOldTpy != 0xffffffff ){
				wk->slideVec = wk->slideOldTpy - wk->slideStartTpy;
				wk->slideReqCount = 0;
				if( InitSledeAutoScroll( wk ) == TRUE ){
					break;
				}
			}
			wk->autoScroll = FALSE;
			wk->mainSeq = MAINSEQ_MAIN;
			return FALSE;
		}
		// オートスクロール中なら
/*
		if( wk->autoScroll == TRUE ){
			wk->autoScroll = FALSE;
			wk->slidePos = ret;
		}
*/
		if( wk->slidePos != ret ){
			wk->slidePosTmp   = ret;
			wk->slideVec      = wk->slidePos - ret;
			wk->slideReqCount = 0;
//			wk->slideStartTpy = wk->nowTpy;
//			wk->slideOldTpy   = wk->nowTpy;
			wk->slideSeq      = 1;
		}
		wk->slideOldTpy   = wk->slideStartTpy;
		wk->slideStartTpy = wk->nowTpy;
		break;

	case 1:
		// タッチされていない
		if( ret == GFL_UI_TP_HIT_NONE ){
			wk->slideSeq = 0;
			return InitSledeAutoScroll( wk );
		}
		// スライドエリア範囲外
		if( wk->hed.touch[ret].prm != FRAMELIST_TOUCH_PARAM_SLIDE ){
			wk->slideSeq = 0;
			return InitSledeAutoScroll( wk );
		}
		// タイムオーバー
		if( wk->slideReqCount == 5 ){
			wk->slideReqCount = 0;
			wk->slideSeq = 0;
			if( SetSlideScroll( wk, ret ) == TRUE ){
				wk->slideTouchFlag = 1;
			}
		}else{
			wk->slidePosTmp   = ret;
//			wk->slideOldTpy   = wk->nowTpy;
			wk->slideStartTpy = wk->nowTpy;
			wk->slideReqCount++;
		}
		break;
	}

	return TRUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スライドスクロール設定
 *
 * @param		wk		ワーク
 * @param		pos		移動先
 *
 * @retval	"TRUE = スクロール可"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
static BOOL SetSlideScroll( FRAMELIST_WORK * wk, int pos )
{
	u32	cntMax;
	s32	abs;
	s8	speed;

	abs = wk->slidePos - pos;
	// 向きが違う
	if( ( abs < 0 && wk->slideVec > 0 ) || ( abs > 0 && wk->slideVec < 0 ) ){
		return FALSE;
	}

	// スクロール不可
	if( ( wk->slideVec < 0 && wk->listScroll == 0 ) ||
			( wk->slideVec > 0 && wk->listScroll == wk->listScrollMax ) ){
		return FALSE;
	}

	abs = GFL_STD_Abs( abs );

	if( abs == 0 ){
		return FALSE;
	}else if( abs == 1 ){
		speed = wk->hed.scrollSpeed[3];
	}else if( abs == 2 ){
		speed = wk->hed.scrollSpeed[2];
	}else if( abs == 3 ){
		speed = wk->hed.scrollSpeed[1];
	}else{
		speed = wk->hed.scrollSpeed[0];
	}

	if( pos > wk->slidePos ){
		speed *= -1;
	}

	cntMax = GetListScrollCount( wk, speed );
	if( cntMax != 0 ){
		if( cntMax < abs ){
			abs = cntMax;
		}
		InitListScroll( wk, speed, abs, MAINSEQ_SLIDE, TRUE );
		wk->slidePos = pos;
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		オートスクロール開始チェック
 *
 * @param		wk		ワーク
 *
 * @retval	"TRUE = スクロール開始"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
static BOOL InitSledeAutoScroll( FRAMELIST_WORK * wk )
{
	u32	max;
	u32	abs;
	s8	speed;

	if( ( wk->slideVec < 0 && wk->slideOldTpy > wk->slideStartTpy ) || 
			( wk->slideVec > 0 && wk->slideOldTpy < wk->slideStartTpy ) ){
		return FALSE;
	}

	abs = GFL_STD_Abs( wk->slideOldTpy - wk->slideStartTpy );

	if( abs == 0 ){
		return FALSE;
	}

	for( wk->slideCnt=0; wk->slideCnt<FRAMELIST_SPEED_MAX; wk->slideCnt++ ){
		if( abs >= AutoScrollSpeed[wk->slideReqCount][wk->slideCnt] ){
			speed = wk->hed.scrollSpeed[wk->slideCnt];
			abs   = AutoScrollCount[wk->slideCnt];
			break;
		}
	}
	if( wk->slideCnt == FRAMELIST_SPEED_MAX ){
		return FALSE;
	}
	if( wk->slideStartTpy > wk->slideOldTpy ){
		speed *= -1;
	}

	max = GetListScrollCount( wk, speed );
	if( max != 0 ){
		if( max < abs ){
			abs = max;
		}
		InitListScroll( wk, speed, abs, MAINSEQ_SLIDE, TRUE );
//		OS_Printf( "speed = %d, max = %d\n", speed, abs );
		wk->autoScroll = TRUE;
//		wk->slidePos = pos;
		return TRUE;
	}

	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		オートスクロール再設定チェック
 *
 * @param		wk		ワーク
 *
 * @retval	"TRUE = スクロール開始"
 * @retval	"FALSE = それ以外"
 */
//--------------------------------------------------------------------------------------------
static BOOL CheckSledeAutoScrollRepeat( FRAMELIST_WORK * wk )
{
	u32	max;
	u32	abs;
	u32	i;
	s8	speed;

	if( ( wk->slideVec < 0 && wk->slideOldTpy > wk->slideStartTpy ) || 
			( wk->slideVec > 0 && wk->slideOldTpy < wk->slideStartTpy ) ){
		return FALSE;
	}

	abs = GFL_STD_Abs( wk->slideOldTpy - wk->slideStartTpy );

	if( abs == 0 ){
		return FALSE;
	}

	for( i=0; i<FRAMELIST_SPEED_MAX; i++ ){
		if( abs >= AutoScrollSpeed[wk->slideReqCount][i] ){
			speed = wk->hed.scrollSpeed[i];
			abs   = AutoScrollCount[i];
			break;
		}
	}
	if( i == FRAMELIST_SPEED_MAX ){
		return FALSE;
	}
	if( wk->slideStartTpy > wk->slideOldTpy ){
		speed *= -1;
	}

	max = GetListScrollCount( wk, speed );
	if( max != 0 ){
		if( max < abs ){
			abs = max;
		}
		return TRUE;
	}

	return FALSE;
}


//============================================================================================
//	描画関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＶＲＡＭ取得用のＢＧフレームパラメータを取得
 *
 * @param		bgFrame		ＢＧフレーム番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static GFL_DISPUT_BGID GetDispBGID( u8 bgFrame )
{
	if( bgFrame == GFL_BG_FRAME0_M ){
		return GFL_DISPUT_BGID_M0;
	}else if( bgFrame == GFL_BG_FRAME1_M ){
		return GFL_DISPUT_BGID_M1;
	}else if( bgFrame == GFL_BG_FRAME2_M ){
		return GFL_DISPUT_BGID_M2;
	}else if( bgFrame == GFL_BG_FRAME3_M ){
		return GFL_DISPUT_BGID_M3;
	}else if( bgFrame == GFL_BG_FRAME0_S ){
		return GFL_DISPUT_BGID_S0;
	}else if( bgFrame == GFL_BG_FRAME1_S ){
		return GFL_DISPUT_BGID_S1;
	}else if( bgFrame == GFL_BG_FRAME2_S ){
		return GFL_DISPUT_BGID_S2;
	}else if( bgFrame == GFL_BG_FRAME3_S ){
		return GFL_DISPUT_BGID_S3;
	}
	return GFL_DISPUT_BGID_M0;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目背景書き込み
 *
 * @param		itemNum		項目番号
 * @param		frmNum		プリントフレーム番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void WriteItemFrame( FRAMELIST_WORK * wk, u32 itemNum, u32 frmNum )
{
	GFL_BMPWIN * win;
	u16 * scrn;
	u8 * bmp;
	u8 * cgx;
	u8	bmp_px, bmp_py, bmp_sx, bmp_sy;
	u8	frm_sx, frm_sy;
	u8	i, j;

	win  = wk->printUtil[frmNum].util.win;
	scrn = wk->wfrmGra[ wk->item[itemNum].type ].scrn;
	cgx  = GFL_DISPUT_GetCgxPtr( GetDispBGID(BGWINFRM_BGFrameGet(wk->wfrm,frmNum)) );

	// フレームに背景をセット
	BGWINFRM_FrameSet( wk->wfrm, frmNum, scrn );

	// フレームにBMPWINをセット
	BGWINFRM_BmpWinOn( wk->wfrm, frmNum, win );

	// BMPWINに背景を描画
	frm_sx = wk->hed.itemSizX;
	frm_sy = wk->hed.itemSizY;
	bmp_sx = GFL_BMPWIN_GetScreenSizeX( win );
	bmp_sy = GFL_BMPWIN_GetScreenSizeY( win );
	bmp_px = GFL_BMPWIN_GetPosX( win );
	bmp_py = GFL_BMPWIN_GetPosY( win );
	bmp = GFL_BMP_GetCharacterAdrs( GFL_BMPWIN_GetBmp(win) );
	for( i=0; i<bmp_sy; i++ ){
		for( j=0; j<bmp_sx; j++ ){
			GFL_STD_MemCopy32(
				&cgx[(scrn[(i+bmp_py)*frm_sx+j+bmp_px]&0x3ff)*0x20],
				&bmp[(i*bmp_sx+j)*0x20],
				0x20 );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目配置
 *
 * @param		idx			項目インデックス
 * @param		py			配置Ｙ座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PutItemFrame( FRAMELIST_WORK * wk, u32 idx, s8 py )
{
	u16 * buf;
	u16	frm;
	u16	i;

	buf = BGWINFRM_FrameBufGet( wk->wfrm, idx );
	frm = BGWINFRM_BGFrameGet( wk->wfrm, idx );

	for( i=0; i<wk->hed.itemSizY; i++ ){
		if( py < 0 ){
			py += 32;
		}else if( py >= 32 ){
			py -= 32;
		}
		GFL_BG_WriteScreenExpand(
			frm,
			wk->hed.itemPosX, py,
			wk->hed.itemSizX, 1,
			buf,
			0, i,
			wk->hed.itemSizX, wk->hed.itemSizY );
		py++;
	}
	GFL_BG_LoadScreenV_Req( frm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		一項目クリア
 *
 * @param		wk			ワーク
 * @param		frm			ＢＧフレーム
 * @param		py			配置Ｙ座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ClearItemFrame( FRAMELIST_WORK * wk, u8 frm, s8 py )
{
	u32	i;

	for( i=0; i<wk->hed.itemSizY; i++ ){
		if( py < 0 ){
			py += 32;
		}else if( py >= 32 ){
			py -= 32;
		}
		GFL_BG_FillScreen( frm, 0, wk->hed.itemPosX, py, wk->hed.itemSizX, 1, 0 );
		py++;
	}
	GFL_BG_LoadScreenV_Req( frm );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目配置Ｙ座標を取得
 *
 * @param		wk		ワーク
 * @param		vec		配置方向
 *
 * @return	配置Ｙ座標
 */
//--------------------------------------------------------------------------------------------
static s8 GetItemFramePosY( FRAMELIST_WORK * wk, s8 vec )
{
	s8	py;
	s8	prm;
	
	py  = wk->hed.itemPosY;
	prm = wk->hed.itemSizY * vec;
	
	while( 1 ){
		py += prm;
		if( py < 0 || py >= BGSCRN_CHR_SIZE_Y ){
			break;
		}
	}
	py = py + ( wk->listBgScoll / 8 );
	return py;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		次の項目フレーム番号を取得
 *
 * @param		now		現在の項目フレーム番号
 * @param		max		フレーム最大数
 * @param		vec		配置方向
 *
 * @return	フレーム番号
 */
//--------------------------------------------------------------------------------------------
static s8 GetNextFrameNum( s8 now, s8 max, s8 vec )
{
	now += vec;
	if( now < 0 ){
		now = max;
	}else if( now > max ){
		now = 0;
	}

	return now;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メイン画面の項目描画
 *
 * @param		wk			ワーク
 * @param		pos			項目番号
 * @param		py			描画Ｙ座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void DrawListItemMain( FRAMELIST_WORK * wk, s32 pos, s8 py )
{
	if( pos >= 0 && pos < wk->listMax ){
		WriteItemFrame( wk, pos, wk->putFrameMain );
		PutItemFrame( wk, wk->putFrameMain, py );
		{
			s32	drawPy = py;
			wk->hed.cbFunc->draw(
				wk->hed.cbWork,
				pos,
				&wk->printUtil[wk->putFrameMain].util,
				drawPy * 8 - wk->listBgScoll,
				TRUE );
		}
	}else{
		ClearItemFrame( wk, wk->hed.mainBG, py );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		サブ画面の項目描画
 *
 * @param		wk			ワーク
 * @param		pos			項目番号
 * @param		py			描画Ｙ座標
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void DrawListItemSub( FRAMELIST_WORK * wk, s32 pos, s8 py )
{
	if( wk->hed.subBG != FRAMELIST_SUB_BG_NONE ){
//		py += wk->subFramePy;
		if( pos >= 0 ){
			u32	subFrame = wk->putFrameSub + wk->printSubPos;
			WriteItemFrame( wk, pos, subFrame );
			PutItemFrame( wk, subFrame, py );
			{
				s32	drawPy = py;
				wk->hed.cbFunc->draw(
					wk->hed.cbWork,
					pos,
					&wk->printUtil[subFrame].util,
					drawPy * 8 - wk->listBgScoll,
					FALSE );
			}
		}else{
			ClearItemFrame( wk, wk->hed.subBG, py );
		}
	}
}

//--------------------------------------------------------------------------------------------
/**
 * 指定位置の項目のパレットを変更	
 *
 * @param		wk		ワーク
 * @param		pos		カーソル位置
 * @param		pal		パレット番号
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_ChangePosPalette( FRAMELIST_WORK * wk, u16 pos, u16 pal )
{
	s16	py;
	u16	i;

	py = wk->hed.itemPosY + pos * wk->hed.itemSizY + wk->listBgScoll / 8;

	for( i=0; i<wk->hed.itemSizY; i++ ){
		if( py < 0 ){
			py += 32;
		}else if( py >= 32 ){
			py -= 32;
		}
		GFL_BG_ChangeScreenPalette(
			wk->hed.mainBG,
			wk->hed.itemPosX, py,
			wk->hed.itemSizX, 1,
			pal );
		py++;
	}
	GFL_BG_LoadScreenV_Req( wk->hed.mainBG );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソル位置のパレット変更
 *
 * @param		wk			ワーク
 * @param		now			現在のカーソル位置
 * @param		old			前回のカーソル位置
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void ChangeCursorPosPalette( FRAMELIST_WORK * wk, u16 now, u16 old )
{
	if( old < wk->listPosMax ){
		u16 * scrn = wk->wfrmGra[wk->item[wk->listScroll+old].type].scrn;
		FRAMELIST_ChangePosPalette( wk, old, scrn[0] >> 12 );
	}
	if( now < wk->listPosMax && GFL_UI_CheckTouchOrKey() == GFL_APP_END_KEY ){
		BLINKPALANM_InitAnimeCount( wk->blink );
		FRAMELIST_ChangePosPalette( wk, now, wk->hed.selPal );
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		ＢＭＰ転送
 *
 * @param		wk			ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void PrintTransMain( FRAMELIST_WORK * wk )
{
	u32	i;

	PRINTSYS_QUE_Main( wk->que );
	for( i=0; i<wk->printMax; i++ ){
		PRINT_UTIL_Trans( &wk->printUtil[i].util, wk->que );
	}
}


//============================================================================================
//	ワーク操作関連
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * @brief		プリントキュー取得
 *
 * @param		wk				ワーク
 *
 * @return	プリントキュー
 */
//--------------------------------------------------------------------------------------------
PRINT_QUE * FRAMELIST_GetPrintQue( FRAMELIST_WORK * wk )
{
	return wk->que;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		項目パラメータ取得
 *
 * @param		wk				ワーク
 * @param		itemIdx		項目インデックス
 *
 * @return	項目パラメータ
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_GetItemParam( FRAMELIST_WORK * wk, u32 itemIdx )
{
	return wk->item[itemIdx].param;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リスト位置取得
 *
 * @param		wk				ワーク
 *
 * @return	リスト位置
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_GetListPos( FRAMELIST_WORK * wk )
{
	return ( wk->listPos + wk->listScroll );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソル位置取得
 *
 * @param		wk				ワーク
 *
 * @return	項目パラメータ
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_GetCursorPos( FRAMELIST_WORK * wk )
{
	return wk->listPos;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストスクロールカウンタを取得
 *
 * @param		wk		ワーク
 *
 * @return	スクロールカウンタ
 */
//--------------------------------------------------------------------------------------------
u32 FRAMELIST_GetScrollCount( FRAMELIST_WORK * wk )
{
	return wk->listScroll;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リストを下にスクロールできるか
 *
 * @param		wk		ワーク
 *
 * @retval	"TRUE = 可"
 * @retval	"FALSE = 不可"
 */
//--------------------------------------------------------------------------------------------
BOOL FRAMELIST_CheckScrollMax( FRAMELIST_WORK * wk )
{
	if( wk->listScroll < wk->listScrollMax ){
		return TRUE;
	}
	return FALSE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソル位置を設定
 *
 * @param		wk					ワーク
 * @param		pos					カーソル位置
 *
 * @return	none
 *
 *	コールバック[move]が呼ばれます
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_SetCursorPos( FRAMELIST_WORK * wk, u32 pos )
{
	wk->listOldPos = wk->listPos;
	wk->listPos = pos;
	ChangeCursorPosPalette( wk, wk->listPos, wk->listOldPos );
	wk->hed.cbFunc->move( wk->hed.cbWork, wk->listPos+wk->listScroll, TRUE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソル位置のパレットを光らせる
 *
 * @param		wk					ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_SetCursorPosPalette( FRAMELIST_WORK * wk )
{
	ChangeCursorPosPalette( wk, wk->listPos, PALCHG_NONE );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		カーソル位置のパレットを戻す
 *
 * @param		wk					ワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
void FRAMELIST_SetCursorPosPaletteOff( FRAMELIST_WORK * wk )
{
	ChangeCursorPosPalette( wk, PALCHG_NONE, wk->listPos );
}
