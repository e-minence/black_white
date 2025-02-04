//============================================================================================
/**
 * @file		zukan.c
 * @brief		図鑑画面
 * @author	Hiroyuki Nakamura
 * @date		09.12.14
 *
 *	モジュール名：ZUKAN
 */
//============================================================================================
#include <gflib.h>

#include "system/main.h"
#include "app/zukan.h"

#include "zukan_common.h"
#include "top/zukantop.h"
#include "list/zukanlist.h"
#include "search/zukansearch.h"
#include "detail/zukan_detail.h"


//============================================================================================
//	定数定義
//============================================================================================

// メインシーケンス
enum {
	SEQ_LOCAL_PROC_WAIT,	// 各処理終了待ち

	SEQ_TOP_CALL,					// トップ呼び出し
	SEQ_TOP_END,					// トップ終了後

	SEQ_LIST_CALL,				// リスト呼び出し
	SEQ_LIST_END,					// リスト終了後

	SEQ_INFO_CALL,				// 情報呼び出し(INFO,MAP,VOICE,FORMのどれかを呼び出す)
	SEQ_INFO_END,					// 情報終了後

	SEQ_SEARCH_CALL,			// 検索呼び出し
	SEQ_SEARCH_END,				// 検索終了後

	SEQ_PROC_FINISH,			// 図鑑終了
};

// 図鑑メインワーク
typedef struct {
	ZUKAN_PARAM * prm;			// 外部データ

	GFL_PROCSYS * localProc;						// ローカルＰＲＯＣ
	GFL_PROC_MAIN_STATUS	procStatus;		// ローカルＰＲＯＣの戻り値

	u16 * list;			// リストデータ
	u16	listMax;		// リストデータ数
	u16	listMode;		// リストモード

	ZKNCOMM_LIST_SORT	sort;		// ソートデータ

	int	mainSeq;			// メインシーケンス
	int nextSeq;			// 次のシーケンス

  ZUKAN_DETAIL_TYPE  detail_type;  // SEQ_INFO_CALLのときのみ使用

	void * work;			// 各処理のワーク
}ZUKAN_MAIN_WORK;

typedef int (*pZUKAN_FUNC)(ZUKAN_MAIN_WORK*);


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
static GFL_PROC_RESULT ZukanProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT ZukanProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk );
static GFL_PROC_RESULT ZukanProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk );

static void FreeListData( ZUKAN_MAIN_WORK * wk );

static int MainSeq_LocalProcWait( ZUKAN_MAIN_WORK * wk );

static int MainSeq_CallTop( ZUKAN_MAIN_WORK * wk );
static int MainSeq_EndTop( ZUKAN_MAIN_WORK * wk );

static int MainSeq_CallList( ZUKAN_MAIN_WORK * wk );
static int MainSeq_EndList( ZUKAN_MAIN_WORK * wk );

static int MainSeq_CallSearch( ZUKAN_MAIN_WORK * wk );
static int MainSeq_EndSearch( ZUKAN_MAIN_WORK * wk );

static int MainSeq_CallDetail( ZUKAN_MAIN_WORK * wk );
static int MainSeq_EndDetail( ZUKAN_MAIN_WORK * wk );


//============================================================================================
//	グローバル
//============================================================================================

// PROCデータ
const GFL_PROC_DATA ZUKAN_ProcData = {
	ZukanProc_Init,
	ZukanProc_Main,
	ZukanProc_End
};

// メインシーケンス関数テーブル
static const pZUKAN_FUNC MainSeq[] = {
	MainSeq_LocalProcWait,	// 各処理の終了待ち

	MainSeq_CallTop,				// トップ呼び出し
	MainSeq_EndTop,					// トップ終了後

	MainSeq_CallList,				// リスト呼び出し
	MainSeq_EndList,				// リスト終了後

	MainSeq_CallDetail,			// 情報呼び出し
	MainSeq_EndDetail,			// 情報終了後

	MainSeq_CallSearch,			// 検索呼び出し
	MainSeq_EndSearch,			// 検索終了後
};


//--------------------------------------------------------------------------------------------
/**
 * @brief		プロセス関数：初期化
 *
 * @param		proc	プロセスデータ
 * @param		seq		シーケンス
 * @param		pwk		親ワーク
 * @param		mywk	自作ワーク
 *
 * @return	処理状況
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT ZukanProc_Init( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	ZUKAN_MAIN_WORK * wk;

//	OS_Printf( "↓↓↓↓↓　図鑑処理開始　↓↓↓↓↓\n" );

	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_ZUKAN_SYS, 0x8000 );

	wk = GFL_PROC_AllocWork( proc, sizeof(ZUKAN_MAIN_WORK), HEAPID_ZUKAN_SYS );
	GFL_STD_MemClear( wk, sizeof(ZUKAN_MAIN_WORK) );

	wk->prm = pwk;

	wk->localProc = GFL_PROC_LOCAL_boot( HEAPID_ZUKAN_SYS );

	wk->listMax  = ZKNCOMM_MakeDefaultList( wk->prm->savedata, &wk->list, HEAPID_ZUKAN_SYS );
	wk->listMode = ZKNLIST_CALL_NORMAL;
	ZKNCOMM_InitDefaultMons( wk->prm->savedata, wk->list, wk->listMax );
	ZKNCOMM_ResetSortData( wk->prm->savedata, &wk->sort );

	if( wk->prm->callMode == ZUKAN_MODE_TOP ){
		wk->mainSeq = SEQ_TOP_CALL;
	}else if( wk->prm->callMode == ZUKAN_MODE_LIST ){
		wk->mainSeq = SEQ_LIST_CALL;
	}else if( wk->prm->callMode == ZUKAN_MODE_INFO ){
		wk->mainSeq = SEQ_INFO_CALL;
    wk->detail_type = ZUKAN_DETAIL_TYPE_INFO;
	}else if( wk->prm->callMode == ZUKAN_MODE_MAP ){
		wk->mainSeq = SEQ_INFO_CALL;
    wk->detail_type = ZUKAN_DETAIL_TYPE_MAP;
	}else if( wk->prm->callMode == ZUKAN_MODE_VOICE ){
		wk->mainSeq = SEQ_INFO_CALL;
    wk->detail_type = ZUKAN_DETAIL_TYPE_VOICE;
	}else if( wk->prm->callMode == ZUKAN_MODE_FORM ){
		wk->mainSeq = SEQ_INFO_CALL;
    wk->detail_type = ZUKAN_DETAIL_TYPE_FORM;
	}else{
		wk->mainSeq = SEQ_SEARCH_CALL;
	}

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		プロセス関数：メイン
 *
 * @param		proc	プロセスデータ
 * @param		seq		シーケンス
 * @param		pwk		親ワーク
 * @param		mywk	自作ワーク
 *
 * @return	処理状況
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT ZukanProc_Main( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	ZUKAN_MAIN_WORK * wk = mywk;

	if( wk->mainSeq == SEQ_PROC_FINISH ){
		return GFL_PROC_RES_FINISH;
	}

	wk->procStatus = GFL_PROC_LOCAL_Main( wk->localProc );

	wk->mainSeq = MainSeq[wk->mainSeq]( wk );

	return GFL_PROC_RES_CONTINUE;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		プロセス関数：終了
 *
 * @param		proc	プロセスデータ
 * @param		seq		シーケンス
 * @param		pwk		親ワーク
 * @param		mywk	自作ワーク
 *
 * @return	処理状況
 */
//--------------------------------------------------------------------------------------------
static GFL_PROC_RESULT ZukanProc_End( GFL_PROC * proc, int * seq, void * pwk, void * mywk )
{
	ZUKAN_MAIN_WORK * wk = mywk;

	FreeListData( wk );

	GFL_PROC_LOCAL_Exit( wk->localProc );

	GFL_PROC_FreeWork( proc );
	GFL_HEAP_DeleteHeap( HEAPID_ZUKAN_SYS );

//	OS_Printf( "↑↑↑↑↑　図鑑処理終了　↑↑↑↑↑\n" );

	return GFL_PROC_RES_FINISH;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		リスト解放
 *
 * @param		wk		図鑑メインワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void FreeListData( ZUKAN_MAIN_WORK * wk )
{
	if( wk->list != NULL ){
		GFL_HEAP_FreeMemory( wk->list );
		wk->list = NULL;
	}
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		各処理の終了待ちへ
 *
 * @param		wk		図鑑メインワーク
 * @param		next	終了後のシーケンス
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int NextProcCall( ZUKAN_MAIN_WORK * wk, int next )
{
	wk->nextSeq = next;
	return SEQ_LOCAL_PROC_WAIT;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：各処理の終了待ち
 *
 * @param		wk		図鑑メインワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_LocalProcWait( ZUKAN_MAIN_WORK * wk )
{
	if( wk->procStatus != GFL_PROC_MAIN_VALID ){
		return wk->nextSeq;
	}
	return SEQ_LOCAL_PROC_WAIT;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：トップ画面呼び出し
 *
 * @param		wk		図鑑メインワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_CallTop( ZUKAN_MAIN_WORK * wk )
{
	ZUKANTOP_DATA * top;

	wk->work = GFL_HEAP_AllocMemory( HEAPID_ZUKAN_SYS, sizeof(ZUKANTOP_DATA) );
	top = wk->work;

	top->gamedata = wk->prm->gamedata;
	top->mystatus = wk->prm->mystatus;

	GFL_PROC_LOCAL_CallProc( wk->localProc, FS_OVERLAY_ID(zukan_top), &ZUKANTOP_ProcData, wk->work );

	return NextProcCall( wk, SEQ_TOP_END );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：トップ画面終了後
 *
 * @param		wk		図鑑メインワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_EndTop( ZUKAN_MAIN_WORK * wk )
{
	ZUKANTOP_DATA * top;
	int	ret;

	top = wk->work;

	switch( top->retMode ){
	case ZKNTOP_RET_EXIT:			// 図鑑終了
		wk->prm->retMode = ZUKAN_RET_NORMAL;
		ret = SEQ_PROC_FINISH;
		break;

	case ZKNTOP_RET_EXIT_X:		// 図鑑を終了してメニューを閉じる
		wk->prm->retMode = ZUKAN_RET_MENU_CLOSE;
		ret = SEQ_PROC_FINISH;
		break;

	case ZKNTOP_RET_LIST:			// リスト画面へ
		ret = SEQ_LIST_CALL;
		break;
	}

	GFL_HEAP_FreeMemory( wk->work );

	return ret;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：リスト画面呼び出し
 *
 * @param		wk		図鑑メインワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_CallList( ZUKAN_MAIN_WORK * wk )
{
	ZUKANLIST_DATA * list;

	wk->work = GFL_HEAP_AllocMemory( HEAPID_ZUKAN_SYS, sizeof(ZUKANLIST_DATA) );
	list = wk->work;

	list->gamedata = wk->prm->gamedata;
	list->savedata = wk->prm->savedata;

	list->callMode = wk->listMode;
	if( wk->listMode == ZKNLIST_CALL_NORMAL && ZKNCOMM_CheckLocalListNumberZero(wk->prm->savedata) == FALSE ){
		list->list    = &wk->list[1];
		list->listMax = wk->listMax-1;
	}else{
		list->list    = wk->list;
		list->listMax = wk->listMax;
	}
	GFL_PROC_LOCAL_CallProc( wk->localProc, FS_OVERLAY_ID(zukan_list), &ZUKANLIST_ProcData, wk->work );

	return NextProcCall( wk, SEQ_LIST_END );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：リスト画面終了後
 *
 * @param		wk		図鑑メインワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_EndList( ZUKAN_MAIN_WORK * wk )
{
	ZUKANLIST_DATA * list;
	int	ret;

	list = wk->work;

	switch( list->retMode ){
	case ZKNLIST_RET_EXIT:		// 図鑑終了
		if( wk->listMode == ZKNLIST_CALL_NORMAL ){
			wk->prm->retMode = ZUKAN_RET_NORMAL;
			ret = SEQ_PROC_FINISH;
		}else{
			FreeListData( wk );
			wk->listMax  = ZKNCOMM_MakeDefaultList( wk->prm->savedata, &wk->list, HEAPID_ZUKAN_SYS );
			wk->listMode = ZKNLIST_CALL_NORMAL;
			ret = SEQ_LIST_CALL;
		}
		break;

	case ZKNLIST_RET_EXIT_X:	// 図鑑を終了してメニューを閉じる
		wk->prm->retMode = ZUKAN_RET_MENU_CLOSE;
		ret = SEQ_PROC_FINISH;
		break;

	case ZKNLIST_RET_MODE_CHANGE:	// 図鑑モードを切り替える
		FreeListData( wk );
		wk->listMax = ZKNCOMM_MakeDefaultList( wk->prm->savedata, &wk->list, HEAPID_ZUKAN_SYS );
		ret = SEQ_LIST_CALL;
		break;

	case ZKNLIST_RET_INFO:		// 詳細画面へ
		ret = SEQ_INFO_CALL;
    wk->detail_type = ZUKAN_DETAIL_TYPE_INFO;
		break;

	case ZKNLIST_RET_SEARCH:	// 検索画面へ
		if( wk->listMode == ZKNLIST_CALL_SEARCH ){
			FreeListData( wk );
			wk->listMax = ZKNCOMM_MakeDefaultList( wk->prm->savedata, &wk->list, HEAPID_ZUKAN_SYS );
		}
		ret = SEQ_SEARCH_CALL;
		break;

	default:
		wk->prm->retMode = ZUKAN_RET_NORMAL;
		ret = SEQ_PROC_FINISH;
	}

	GFL_HEAP_FreeMemory( wk->work );

	return ret;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：検索画面呼び出し
 *
 * @param		wk		図鑑メインワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_CallSearch( ZUKAN_MAIN_WORK * wk )
{
	ZUKANSEARCH_DATA * search;

	wk->work = GFL_HEAP_AllocMemory( HEAPID_ZUKAN_SYS, sizeof(ZUKANSEARCH_DATA) );
	search = wk->work;

	search->gamedata = wk->prm->gamedata;
	search->savedata = wk->prm->savedata;
	search->sort     = &wk->sort;
	search->list     = NULL;
	search->listMax  = 0;
	GFL_PROC_LOCAL_CallProc( wk->localProc, FS_OVERLAY_ID(zukan_search), &ZUKANSEARCH_ProcData, wk->work );

	return NextProcCall( wk, SEQ_SEARCH_END );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：検索画面終了後
 *
 * @param		wk		図鑑メインワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_EndSearch( ZUKAN_MAIN_WORK * wk )
{
	ZUKANSEARCH_DATA * search;
	int	ret;

	search = wk->work;

	switch( search->retMode ){
	case ZKNSEARCH_RET_START:		// 検索実行
		FreeListData( wk );
		wk->list     = search->list;
		wk->listMax  = search->listMax;
		wk->listMode = ZKNLIST_CALL_SEARCH;
		ret = SEQ_LIST_CALL;
		break;
	
	case ZKNSEARCH_RET_CANCEL:	// キャンセル
#ifdef	BUGFIX_BTS7745_20100712
		wk->listMode = ZKNLIST_CALL_NORMAL;
#endif	// BUGFIX_BTS7745_20100712
		ret = SEQ_LIST_CALL;
		break;

	case ZKNSEARCH_RET_EXIT_X:	// 図鑑を終了してメニューを閉じる
	default:
		wk->prm->retMode = ZUKAN_RET_MENU_CLOSE;
		ret = SEQ_PROC_FINISH;
		break;
	}

	GFL_HEAP_FreeMemory( wk->work );

	return ret;
}


//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：図鑑詳細画面呼び出し
 *
 * @param		wk		図鑑メインワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_CallDetail( ZUKAN_MAIN_WORK * wk )
{
  ZUKAN_DETAIL_PARAM*   detail;

  GAMEDATA*             gamedata       = wk->prm->gamedata;
  ZUKAN_SAVEDATA*       zukan_savedata = GAMEDATA_GetZukanSave( gamedata );
  u16                   monsno         = ZUKANSAVE_GetDefaultMons( zukan_savedata );

	detail = GFL_HEAP_AllocMemory( HEAPID_ZUKAN_SYS, sizeof(ZUKAN_DETAIL_PARAM) );
  
  detail->gamedata = wk->prm->gamedata;
  detail->type     = wk->detail_type;
  detail->list     = wk->list;
  detail->num      = wk->listMax;
  detail->no       = 0;
  {
    u16 i;
    for( i=0; i<detail->num; i++ )
    {
      if( detail->list[i] == monsno )
      {
        detail->no = i;
        break;
      }
    }
  }

	GFL_PROC_LOCAL_CallProc( wk->localProc, FS_OVERLAY_ID(zukan_detail), &ZUKAN_DETAIL_ProcData, detail );

  wk->work = detail;	
  return NextProcCall( wk, SEQ_INFO_END );
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		メインシーケンス：図鑑詳細画面終了後
 *
 * @param		wk		図鑑メインワーク
 *
 * @return	次のシーケンス
 */
//--------------------------------------------------------------------------------------------
static int MainSeq_EndDetail( ZUKAN_MAIN_WORK * wk )
{
	ZUKAN_DETAIL_PARAM*   detail;

  GAMEDATA*             gamedata       = wk->prm->gamedata;
  ZUKAN_SAVEDATA*       zukan_savedata = GAMEDATA_GetZukanSave( gamedata );
  u16                   monsno;

	int	                  ret;

	detail = wk->work;

  {
    monsno = detail->list[detail->no];
    ZUKANSAVE_SetDefaultMons( zukan_savedata, monsno );
  }

  switch( detail->ret )
  {
  case ZUKAN_DETAIL_RET_CLOSE:
    {
		  wk->prm->retMode = ZUKAN_RET_MENU_CLOSE;
      ret = SEQ_PROC_FINISH;
    }
    break;
  case ZUKAN_DETAIL_RET_RETURN:
    {
      wk->prm->retMode = ZUKAN_RET_NORMAL;
      ret = SEQ_LIST_CALL;
    }
    break;
  }
		
	GFL_HEAP_FreeMemory( wk->work );

	return ret;
}
