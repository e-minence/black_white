//======================================================================
/**
 * @file	field_msgbg.c
 * @brief	フィールド　メッセージBG関連
 * @authaor	kagaya
 * @data	2008.12.11
 */
//======================================================================
#include <gflib.h>
#include "system/gfl_use.h"

#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"
#include "system/bmp_winframe.h"

#include "arc_def.h"
#include "font/font.naix"

#include "field/field_msgbg.h"

//======================================================================
//	define
//======================================================================
#define FLDMSGBG_BGFRAME (GFL_BG_FRAME1_M)	///<使用BGフレーム
#define FLDMSGBG_PANO_MENU (13) 			///<メニューパレットNo
#define FLDMSGBG_PANO_FONT (14)				///<フォントパレットNo
#define FLDMSGBG_PRINT_MAX (4)				///<PRINT関連要素数最大
#define FLDMSGBG_STRLEN (48)				///<文字列長さ標準

//======================================================================
//	typedef struct
//======================================================================
//--------------------------------------------------------------
///	FLDMSGPRINT
//--------------------------------------------------------------
struct _TAG_FLDMSGPRINT
{
	FLDMSGBG *fmb;
	
	BOOL printTransFlag;
	GFL_FONT *fontHandle;	//FLDMSGBGから
	PRINT_QUE *printQue;	//FLDMSGBGから
	GFL_MSGDATA *msgData;	//ユーザーから
	GFL_BMPWIN *bmpwin;		//ユーザーから
	
	STRBUF *strBuf;
	PRINT_UTIL printUtil;
};

//--------------------------------------------------------------
///	FLDMSGBG
//--------------------------------------------------------------
struct _TAG_FLDMSGBG
{
	HEAPID heapID;
	u32 bgFrame;
	
	GFL_FONT *fontHandle;
	PRINT_QUE *printQue;
	FLDMSGPRINT msgPrintTbl[FLDMSGBG_PRINT_MAX];
};

//--------------------------------------------------------------
///	FLDMSGWIN
//--------------------------------------------------------------
struct _TAG_FLDMSGWIN
{
	GFL_BMPWIN *bmpwin;		//ユーザーから
	FLDMSGPRINT *msgPrint;
	FLDMSGBG *fmb;
};

//--------------------------------------------------------------
///	FLDMENUFUNC
//--------------------------------------------------------------
struct _TAG_FLDMENUFUNC
{
	FLDMSGBG *fmb;
	FLDMSGPRINT *msgPrint;
	GFL_BMPWIN *bmpwin;
	
	BMP_MENULIST_DATA *pMenuListData;
	BMPMENULIST_WORK *pMenuListWork;
};

//======================================================================
//	proto
//======================================================================
static void FldMenuFuncH_BmpMenuListH(
	BMPMENULIST_HEADER *menuH, const FLDMENUFUNC_HEADER *fmenuH );

static GFL_BMPWIN * FldBmpWinFrame_Init( u32 bgFrame, HEAPID heapID,
	u16 pos_x, u16 pos_y, u16 size_x, u16 size_y );
static void FldBmpWinFrame_Delete( GFL_BMPWIN *bmpwin );

//======================================================================
//	FLDMSGBG	フィールドメッセージBG関連
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMSGBG セットアップ
 * @param	heapID	HEAPID
 * @retval	FLDMAPBG	FLDMAPBG*
 */
//--------------------------------------------------------------
FLDMSGBG * FLDMSGBG_Setup( HEAPID heapID )
{
	FLDMSGBG *fmb;
	
	fmb = GFL_HEAP_AllocClearMemory( heapID, sizeof(FLDMSGBG) );
	fmb->heapID = heapID;
	fmb->bgFrame = FLDMSGBG_BGFRAME;
	
	{	//BG初期化
		GFL_BG_BGCNT_HEADER bgcntText = {
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		
		GFL_BG_SetBGControl( fmb->bgFrame, &bgcntText, GFL_BG_MODE_TEXT );
		GFL_BG_SetVisible( fmb->bgFrame, VISIBLE_ON );
		
		GFL_BG_SetPriority( fmb->bgFrame, 0 );
		GFL_BG_SetPriority( GFL_BG_FRAME0_M, 3 );
		
		GFL_BG_FillCharacter( fmb->bgFrame, 0x00, 1, 0 );
		GFL_BG_FillScreen( fmb->bgFrame,
			0x0000, 0, 0, 32, 32, GFL_BG_SCRWRT_PALIN );
		
		GFL_BG_LoadScreenReq( fmb->bgFrame );
	}
	
	{	//フォントパレット
		GFL_ARC_UTIL_TransVramPalette(
			ARCID_FONT, NARC_font_default_nclr,
			PALTYPE_MAIN_BG, FLDMSGBG_PANO_FONT*32, 32, fmb->heapID );
	}
	
	{	//window frame
		BmpWinFrame_GraphicSet( fmb->bgFrame, 1,
			FLDMSGBG_PANO_MENU, MENU_TYPE_SYSTEM, heapID );
	}
	
	{	//font
		fmb->fontHandle = GFL_FONT_Create(
			ARCID_FONT, NARC_font_large_nftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, fmb->heapID );
	}
	
	{	//print
		fmb->printQue = PRINTSYS_QUE_Create( fmb->heapID );
	}
	
	return( fmb );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG 削除
 * @param	fmb	FLDMSGBG
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_Delete( FLDMSGBG *fmb )
{
	int i = 0;
	FLDMSGPRINT *msgPrint = fmb->msgPrintTbl;
	
//	GFL_BG_FreeCharacterArea( fmb->bgFrame, 0x00, 0x20 );
	GFL_BG_FillCharacterRelease( fmb->bgFrame, 0x00, 0x20 );

	GFL_BG_FreeBGControl( fmb->bgFrame );
	
	do{
		#if 0
		if( msgPrint->msgData != NULL ){
			GFL_MSG_Delete( msgPrint->msgData );
		}
		#endif
		if( msgPrint->strBuf != NULL ){
			GFL_STR_DeleteBuffer( msgPrint->strBuf );
		}
		msgPrint++;
		i++;
	}while( i < FLDMSGBG_PRINT_MAX );
	
	PRINTSYS_QUE_Delete( fmb->printQue );
	GFL_FONT_Delete( fmb->fontHandle );
	GFL_HEAP_FreeMemory( fmb );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG プリント処理
 * @param	fmb	FLDMSGBG
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_PrintMain( FLDMSGBG *fmb )
{
	int i;
	FLDMSGPRINT *msgPrint = fmb->msgPrintTbl;
	
	PRINTSYS_QUE_Main( fmb->printQue );
	
	for( i = 0; i < FLDMSGBG_PRINT_MAX; i++, msgPrint++ ){
		if( msgPrint->printQue != NULL ){
			msgPrint->printTransFlag = PRINT_UTIL_Trans(
				&msgPrint->printUtil, msgPrint->printQue );
		}
	}
}

//--------------------------------------------------------------
/**
 * FLDMSGBG プリントキューに貯まっている処理を全てクリア
 * @param	fmb	FLDMSGBG
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_ClearPrintQue( FLDMSGBG *fmb )
{
	PRINTSYS_QUE_Clear( fmb->printQue );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG プリントキューに貯まっている処理を全て実行
 * @param	fmb	FLDMSGBG
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGBG_AllPrint( FLDMSGBG *fmb )
{
	while( FLDMSGBG_CheckFinishPrint(fmb) != TRUE ){
		FLDMSGBG_PrintMain( fmb );
	}
}

//--------------------------------------------------------------
/**
 * FLDMSGBG プリントキューの処理が全て完了しているかチェック
 * @param	fmb	FLDMSGBG
 * @retval	BOOL TRUE=完了
 */
//--------------------------------------------------------------
BOOL FLDMSGBG_CheckFinishPrint( FLDMSGBG *fmb )
{
	return( PRINTSYS_QUE_IsFinished(fmb->printQue) );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG 使用しているPRINT_QUEを返す
 * @param	fmb	FLDMSGBG
 * @retval	PRINT_QUE*
 */
//--------------------------------------------------------------
PRINT_QUE * FLDMSGBG_GetPrintQue( FLDMSGBG *fmb )
{
	return( fmb->printQue );
}

//--------------------------------------------------------------
/**
 * FLDMSGBG FLDMSGBGで使用しているHEAPIDでGFL_MSGDATA初期化。
 * 戻り値GFL_MSGDATAの削除は各自で行う事。
 * @param	fmb	FLDMSGBG
 * @param	arcDatIDMsg	メッセージが含まれるアーカイブデータID
 * @retval	GFL_MSGDATA*
 */
//--------------------------------------------------------------
GFL_MSGDATA * FLDMSGBG_CreateMSGDATA( FLDMSGBG *fmb, u32 arcDatIDMsg )
{
	GFL_MSGDATA *msgData;
	msgData = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, arcDatIDMsg, fmb->heapID );
	return( msgData );
}

//======================================================================
//	FLDMSGPRINT	フィールド文字表示関連
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMSGPRINT プリント設定
 * @param	fmb	FLDMSGBG
 * @param	msgData	表示する初期化済みのGFL_MSGDATA NULL=使用しない
 * @param	bmpwin	表示する初期化済みのGFL_BMPWIN
 * @retval	FLDMSGPRINT*
 */
//--------------------------------------------------------------
FLDMSGPRINT * FLDMSGPRINT_SetupPrint(
	FLDMSGBG *fmb, GFL_MSGDATA *msgData, GFL_BMPWIN *bmpwin )
{
	int i = 0;
	FLDMSGPRINT *msgPrint = fmb->msgPrintTbl;
	
	do{
		if( msgPrint->printQue == NULL ){
			msgPrint->fmb = fmb;
			msgPrint->bmpwin = bmpwin;
			msgPrint->printTransFlag = TRUE;
			msgPrint->fontHandle = fmb->fontHandle;
			msgPrint->printQue = fmb->printQue;
			msgPrint->strBuf = GFL_STR_CreateBuffer(
					FLDMSGBG_STRLEN, fmb->heapID );
			#if 0
			msgPrint->msgData = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL,
					ARCID_MESSAGE, arcDatIDMsg, fmb->heapID );
			#else
			msgPrint->msgData = msgData;
			#endif
			PRINT_UTIL_Setup( &msgPrint->printUtil, bmpwin );
			return( msgPrint );
		}
		msgPrint++;
		i++;
	}while( i < FLDMSGBG_PRINT_MAX );
	
	GF_ASSERT( 0 );
	return( NULL );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT プリント削除
 * @param	msgPrint	FLDMSGPRINT FLDMSGPRINT_SetupPrint()の戻り値
 * @retval	nothing 
 * FLDMSGPRINT_SetupPrint()で指定したmsgData,bmpwinの削除は各自で行う事。
 */
//--------------------------------------------------------------
void FLDMSGPRINT_Delete( FLDMSGPRINT *msgPrint )
{
	msgPrint->fontHandle = NULL;
	msgPrint->printQue = NULL;
	GFL_STR_DeleteBuffer( msgPrint->strBuf );
	msgPrint->strBuf = NULL;
	#if 0
	GFL_MSG_Delete( msgPrint->msgData );
	#endif
	msgPrint->msgData = NULL;
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT プリント
 * @param	msgPrint	FLDMSGPRINT
 * @param	x		表示X座標
 * @param	y		表示Y座標
 * @param	strID	メッセージデータ 文字列ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_Print( FLDMSGPRINT *msgPrint, u32 x, u32 y, u32 strID )
{
	GF_ASSERT( msgPrint->msgData );
	GFL_MSG_GetString( msgPrint->msgData, strID, msgPrint->strBuf );
	PRINT_UTIL_Print( &msgPrint->printUtil, msgPrint->printQue,
		x, y, msgPrint->strBuf, msgPrint->fontHandle );		
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT プリント　STRBUF指定
 * @param	msgPrint	FLDMSGPRINT
 * @param	x		表示X座標
 * @param	y		表示Y座標
 * @param	strBuf	表示するSTRBUF
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_PrintStrBuf(
		FLDMSGPRINT *msgPrint, u32 x, u32 y, STRBUF *strBuf )
{
	PRINT_UTIL_Print( &msgPrint->printUtil, msgPrint->printQue,
		x, y, strBuf, msgPrint->fontHandle );		
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT　プリント転送チェック
 * @param	msgPrint	FLDMSGPRINT
 * @retval	BOOL	TRUE=転送終了
 */
//--------------------------------------------------------------
BOOL FLDMSGPRINT_CheckPrintTrans( FLDMSGPRINT *msgPrint )
{
	return( msgPrint->printTransFlag );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT 表示するビットマップウィンドウを変更する
 * @param	msgPrint	FLDMSGPRINT
 * @param	bmpwin	表示する初期化済みのGFL_BMPWIN
 * @retval	nothing
 * 以前に指定したbmpwinの削除は各自で行う事。
 */
//--------------------------------------------------------------
void FLDMSGPRINT_ChangeBmpWin( FLDMSGPRINT *msgPrint, GFL_BMPWIN *bmpwin )
{
	msgPrint->bmpwin = bmpwin;
	PRINT_UTIL_Setup( &msgPrint->printUtil, bmpwin );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT ビットマップクリア
 * @param	msgPrint
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGPRINT_ClearBmp( FLDMSGPRINT *msgPrint )
{
	GFL_BMP_DATA *bmp = GFL_BMPWIN_GetBmp( msgPrint->bmpwin );
	GFL_BMP_Clear( bmp, 0xff );
	GFL_BG_LoadScreenReq( msgPrint->fmb->bgFrame );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT　PRINT_QUE取得
 * @param	msgPrint	FLDMSGPRINT
 * @retval	PRINT_QUE*
 */
//--------------------------------------------------------------
PRINT_QUE * FLDMSGPRINT_GetPrintQue( FLDMSGPRINT *msgPrint )
{
	return( msgPrint->printQue );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT　GFL_FONT取得
 * @param	msgPrint	FLDMSGPRINT
 * @retval	GFL_FONT*
 */
//--------------------------------------------------------------
GFL_FONT * FLDMSGPRINT_GetFontHandle( FLDMSGPRINT *msgPrint )
{
	return( msgPrint->fontHandle );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT　PRINT_UTIL取得
 * @param	msgPrint	FLDMSGPRINT
 * @retval	PRINT_UTIL*
 */
//--------------------------------------------------------------
PRINT_UTIL * FLDMSGPRINT_GetPrintUtil( FLDMSGPRINT *msgPrint )
{
	return( &msgPrint->printUtil );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT　STRBUF取得
 * @param	msgPrint	FLDMSGPRINT
 * @retval	STRBUF*
 */
//--------------------------------------------------------------
STRBUF * FLDMSGPRINT_GetStrBuf( FLDMSGPRINT *msgPrint )
{
	return( msgPrint->strBuf );
}

//--------------------------------------------------------------
/**
 * FLDMSGPRINT MSGDATA取得
 * @param	msgPrint	FLDMSGPRINT
 * @retval	MSGDATA*
 */
//--------------------------------------------------------------
GFL_MSGDATA * FLDMSGPRINT_GetMsgData( FLDMSGPRINT *msgPrint )
{
	return( msgPrint->msgData );
}

//======================================================================
//	FLDMSGWIN	メッセージウィンドウ関連
//======================================================================
//--------------------------------------------------------------
/**
 * メッセージウィンドウ　追加
 * @param	fmb	FLDMSGBG*
 * @param	msgData	表示する初期化済みのGFL_MSGDATA NULL=使用しない
 * @param	bmppos_x		//表示座標X キャラ単位
 * @param	bmppos_y		//表示座標Y キャラ単位
 * @param	bmpsize_x		//表示サイズX キャラ単位
 * @param	bmpsize_y		//表示サイズY キャラ単位
 * @retval	FLDMSGWIN*
 */
//--------------------------------------------------------------
FLDMSGWIN * FLDMSGWIN_Add( FLDMSGBG *fmb, GFL_MSGDATA *msgData,
	u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y )
{
	FLDMSGWIN *msgWin;
	
	msgWin = GFL_HEAP_AllocClearMemory( fmb->heapID, sizeof(FLDMSGWIN) );
	msgWin->fmb = fmb;
	msgWin->bmpwin = FldBmpWinFrame_Init(
		fmb->bgFrame, fmb->heapID, bmppos_x, bmppos_y, bmpsize_x, bmpsize_y );
	msgWin->msgPrint = FLDMSGPRINT_SetupPrint( fmb, msgData, msgWin->bmpwin );
	
	return( msgWin );
}

//--------------------------------------------------------------
/**
 * メッセージウィンドウ 削除。
 * FLDMSGWIN_Add()引数msgDataは各自で行う事。
 * @param	msgWin	FLDMSGWIN*
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_Delete( FLDMSGWIN *msgWin )
{
	FldBmpWinFrame_Delete( msgWin->bmpwin );
	FLDMSGPRINT_Delete( msgWin->msgPrint );
	GFL_HEAP_FreeMemory( msgWin );
}

//--------------------------------------------------------------
/**
 * メッセージウィンドウ　メッセージ表示
 * @param	msgWin	FLDMSGWIN
 * @param	x		X表示座標
 * @param	y		Y表示座標
 * @param	strID	メッセージデータ 文字列ID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_Print( FLDMSGWIN *msgWin, u16 x, u16 y, u32 strID )
{
	FLDMSGPRINT_Print( msgWin->msgPrint, x, y, strID );
	GFL_BG_LoadScreenReq( FLDMSGBG_BGFRAME );
}

//--------------------------------------------------------------
/**
 * メッセージウィンドウ　メッセージ表示 STRBUF指定
 * @param	msgWin	FLDMSGWIN
 * @param	x		X表示座標
 * @param	y		Y表示座標
 * @param	strBuf	STRBUF
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_PrintStrBuf( FLDMSGWIN *msgWin, u16 x, u16 y, STRBUF *strBuf )
{
	FLDMSGPRINT_PrintStrBuf( msgWin->msgPrint, x, y, strBuf );
}

//--------------------------------------------------------------
/**
 * メッセージウィンドウ　転送終了チェック
 * @param	msgWin	FLDMSGWIN
 * @retval	BOOL	TRUE=転送終了
 */
//--------------------------------------------------------------
BOOL FLDMSGWIN_CheckPrintTrans( FLDMSGWIN *msgWin )
{
	return( FLDMSGPRINT_CheckPrintTrans(msgWin->msgPrint) );
}

//--------------------------------------------------------------
/**
 * メッセージウィンドウ　メッセージ表示クリア
 * @param	msgWin	FLDMSGWIN
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMSGWIN_ClearWindow( FLDMSGWIN *msgWin )
{
	FLDMSGPRINT_ClearBmp( msgWin->msgPrint );
}

//--------------------------------------------------------------
/**
 * メッセージウィンドウ　追加　会話ウィンドウタイプ
 * @param	fmb	FLDMSGBG*
 * @param	msgData	表示する初期化済みのGFL_MSGDATA NULL=使用しない
 * @retval	FLDMSGWIN*
 */
//--------------------------------------------------------------
FLDMSGWIN * FLDMSGWIN_AddTalkWin( FLDMSGBG *fmb, GFL_MSGDATA *msgData )
{
	return( FLDMSGWIN_Add(fmb,msgData,1,19,30,4) );
}

//======================================================================
//	FLDMENUFUNC　フィールドメニュー関連
//======================================================================
//--------------------------------------------------------------
/**
 * FLDMENUFUNC メニュー追加
 * @param	fmb	FLDMSGBG
 * @param	pMenuHead FLDMENUFUNC_HEADER
 * @param	pMenuListData  FLDMENUFUNC_LISTDATA* Delete時に自動開放される
 * @retval	FLDMENUFUNC*
 */
//--------------------------------------------------------------
FLDMENUFUNC * FLDMENUFUNC_AddMenu( FLDMSGBG *fmb,
	const FLDMENUFUNC_HEADER *pMenuHead,
	FLDMENUFUNC_LISTDATA *pMenuListData )
{
	FLDMENUFUNC *menuFunc;
	BMPMENULIST_HEADER menuH;
	
	menuFunc = GFL_HEAP_AllocClearMemory( fmb->heapID, sizeof(FLDMENUFUNC) );
	FldMenuFuncH_BmpMenuListH( &menuH, pMenuHead );
	
	menuFunc->fmb = fmb;
	menuFunc->pMenuListData = (BMP_MENULIST_DATA *)pMenuListData;
	
	menuFunc->bmpwin = FldBmpWinFrame_Init(
		fmb->bgFrame, fmb->heapID,
		pMenuHead->bmppos_x, pMenuHead->bmppos_y,
		pMenuHead->bmpsize_x, pMenuHead->bmpsize_y );
	
	menuFunc->msgPrint = FLDMSGPRINT_SetupPrint(
			fmb, NULL, menuFunc->bmpwin );
	
	menuH.msgdata = NULL;
	menuH.print_util = FLDMSGPRINT_GetPrintUtil( menuFunc->msgPrint );
	menuH.print_que = FLDMSGPRINT_GetPrintQue( menuFunc->msgPrint );
	menuH.font_handle = fmb->fontHandle;
	menuH.win = menuFunc->bmpwin;
	menuH.list = (const BMP_MENULIST_DATA *)pMenuListData;
	
	menuFunc->pMenuListWork =
		BmpMenuList_Set( &menuH, 0, 0, fmb->heapID );
//	BmpMenuList_SetCursorString( menuFunc->pMenuListWork, 0 );
	BmpMenuList_SetCursorBmp( menuFunc->pMenuListWork, fmb->heapID );
	
	return( menuFunc );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC	削除
 * @param	menuFunc	FLDMENUFUNC*
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMENUFUNC_DeleteMenu( FLDMENUFUNC *menuFunc )
{
	BmpMenuList_Exit( menuFunc->pMenuListWork, NULL, NULL );
	BmpMenuWork_ListDelete( menuFunc->pMenuListData );
	FldBmpWinFrame_Delete( menuFunc->bmpwin );
	FLDMSGPRINT_Delete( menuFunc->msgPrint );
	GFL_HEAP_FreeMemory( menuFunc );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC メニュー処理
 * @param	menuFunc	FLDMENUFUNC*
 * @retval	u32		リスト選択位置のパラメータ
 * FLDMENUFUNC_NULL = 選択中。FLDMENUFUNC_CANCEL = キャンセル
 */
//--------------------------------------------------------------
u32 FLDMENUFUNC_ProcMenu( FLDMENUFUNC *menuFunc )
{
	u32 ret = BmpMenuList_Main( menuFunc->pMenuListWork );
	
	if( ret == BMPMENULIST_NULL ){
		return( FLDMENUFUNC_NULL );
	}
	
	if( ret == BMPMENULIST_CANCEL ){
		return( FLDMENUFUNC_CANCEL );
	}
	
	return( ret );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATAを作成
 * @param	max	リスト数
 * @param	heapID	作成用ヒープID
 * @retval	FLDMENUFUNC_LISTDATA*
 */
//--------------------------------------------------------------
FLDMENUFUNC_LISTDATA * FLDMENUFUNC_CreateListData( int max, HEAPID heapID )
{
	BMP_MENULIST_DATA *listData;
	listData = BmpMenuWork_ListCreate( max, heapID );
	return( (FLDMENUFUNC_LISTDATA*)listData );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATAを削除
 * @param	listData	FLDMENUFUNC_LISTDATA
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMENUFUNC_DeleteListData( FLDMENUFUNC_LISTDATA *listData )
{
	BmpMenuWork_ListDelete( (BMP_MENULIST_DATA*)listData );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATAを作成し
 * FLDMENUFUNC_LISTからメニュー表示用文字列を追加する
 * @param	menuList	FLDMENUFUNC_LIST
 * @param	max			menuList要素数
 * @param	msgData		文字列を
 * @param	heapID		FLDMENUFUNC_LISTDATA作成用ヒープID
 * @retval	FLDMENUFUNC_LISTDATA
 */
//--------------------------------------------------------------
FLDMENUFUNC_LISTDATA * FLDMENUFUNC_CreateMakeListData(
	const FLDMENUFUNC_LIST *menuList,
	int max, GFL_MSGDATA *msgData, HEAPID heapID )
{
	int i;
	FLDMENUFUNC_LISTDATA *listData;
	
	listData = FLDMENUFUNC_CreateListData( max, heapID );
	
	for( i = 0; i < max; i++ ){
		BmpMenuWork_ListAddArchiveString(
			(BMP_MENULIST_DATA*)listData, msgData,
			menuList[i].str_id, (u32)menuList[i].selectParam, heapID );
	}
	
	return( listData );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATAに文字列&パラメータをセット
 * @param	listData	FLDMENUFUNC_LISTDATA
 * @param	strBuf		STRBUF
 * @param	param		パラメタ
 * @param	heapID		ヒープID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMENUFUNC_AddStringListData( FLDMENUFUNC_LISTDATA *listData,
		const STRBUF *strBuf, u32 param, HEAPID heapID  )
{	
	BmpMenuWork_ListAddString(
		(BMP_MENULIST_DATA*)listData, strBuf, param, heapID );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_LISTDATAに文字列&パラメータをセット
 * @param	listData	FLDMENUFUNC_LISTDATA
 * @param	strBuf		STRBUF
 * @param	param		パラメタ
 * @param	heapID		ヒープID
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMENUFUNC_AddArcStringListData( FLDMENUFUNC_LISTDATA *listData,
		GFL_MSGDATA *msgData, u32 strID, u32 param, HEAPID heapID )
{	
	BmpMenuWork_ListAddArchiveString(
		(BMP_MENULIST_DATA*)listData, msgData, strID, param, heapID );
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_HEADERにリスト数、座標系情報を追加
 * @param	head	FLDMENUFUNC_HEADER
 * @param	list_count	リスト数
 * @param	bmppos_x		//表示座標X キャラ単位
 * @param	bmppos_y		//表示座標Y キャラ単位
 * @param	bmpsize_x		//表示サイズX キャラ単位
 * @param	bmpsize_y		//表示サイズY キャラ単位
 * @retval	nothing
 */
//--------------------------------------------------------------
void FLDMENUFUNC_InputHeaderListSize(
	FLDMENUFUNC_HEADER *head, u16 count,
	u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y )
{
	head->count = count;
	head->bmppos_x = bmppos_x;
	head->bmppos_y = bmppos_y;
	head->bmpsize_x = bmpsize_x;
	head->bmpsize_y = bmpsize_y;
}

//--------------------------------------------------------------
/**
 * FLDMENUFUNC_HEADER -> BMPMENULIST_HEADER
 * @param	menuH	BMPMENULIST_HEADER
 * @param	fmenuH	FLDMENUFUNC_HEADER
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldMenuFuncH_BmpMenuListH(
	BMPMENULIST_HEADER *menuH, const FLDMENUFUNC_HEADER *fmenuH )
{
	GFL_STD_MemClear( menuH, sizeof(BMPMENULIST_HEADER) );
	menuH->count = fmenuH->count;			//リスト項目数
	menuH->line = fmenuH->line;				//表示最大項目数
	menuH->rabel_x = fmenuH->rabel_x;		//ラベル表示Ｘ座標
	menuH->data_x = fmenuH->data_x;			//項目表示Ｘ座標
	menuH->cursor_x = fmenuH->cursor_x;		//カーソル表示Ｘ座標
	menuH->line_y = fmenuH->line_y;			//表示Ｙ座標
	menuH->f_col = fmenuH->f_col;			//表示文字色
	menuH->b_col = fmenuH->b_col;			//表示背景色
	menuH->s_col = fmenuH->s_col;			//表示文字影色
	menuH->msg_spc = fmenuH->msg_spc;		//文字間隔Ｘ
	menuH->line_spc = fmenuH->line_spc;		//文字間隔Ｙ
	menuH->page_skip = fmenuH->page_skip;	//ページスキップタイプ
	menuH->font_size_x = fmenuH->font_size_x;	//文字サイズX(ドット
	menuH->font_size_y = fmenuH->font_size_y;	//文字サイズY(ドット
}

//======================================================================
//	パーツ
//======================================================================
//--------------------------------------------------------------
/**
 * ビットマップウィンドウフレーム　初期化
 * @param	bgframe	BGフレーム
 * @param	heapID	HEAPID
 * @param	pos_x	ビットマップ左上X キャラ単位
 * @param	pos_y	ビットマップ左上Y キャラ単位
 * @param	size_x　ビットマップサイズX キャラ単位
 * @param	size_y	ビットマップサイズY キャラ単位
 * @retval	GFL_BMPWIN	作成されたGFL_BMPWIN
 */
//--------------------------------------------------------------
static GFL_BMPWIN * FldBmpWinFrame_Init( u32 bgFrame, HEAPID heapID,
	u16 pos_x, u16 pos_y, u16 size_x, u16 size_y )
{
	GFL_BMP_DATA *bmp;
	GFL_BMPWIN *bmpwin;

	bmpwin = GFL_BMPWIN_Create( bgFrame,
		pos_x, pos_y, size_x, size_y,
		FLDMSGBG_PANO_FONT, GFL_BMP_CHRAREA_GET_B );

	bmp = GFL_BMPWIN_GetBmp( bmpwin );
		
	GFL_BMP_Clear( bmp, 0xff );
	GFL_BMPWIN_MakeScreen( bmpwin );
	GFL_BMPWIN_TransVramCharacter( bmpwin );
	GFL_BG_LoadScreenReq( bgFrame );
	
	BmpWinFrame_Write( bmpwin, WINDOW_TRANS_ON, 1, FLDMSGBG_PANO_MENU );
	
	return( bmpwin );
}

//--------------------------------------------------------------
/**
 * ビットマップウィンドウフレーム　削除
 * @param	bmpwin	FldBmpWinFrame_Init()の戻り値
 * @retval	nothing
 */
//--------------------------------------------------------------
static void FldBmpWinFrame_Delete( GFL_BMPWIN *bmpwin )
{
	BmpWinFrame_Clear( bmpwin, 0 );
	GFL_BMPWIN_Delete( bmpwin );
}
