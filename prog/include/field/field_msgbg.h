//======================================================================
/**
 * @file	field_msgbg.h
 * @brief	フィールド　メッセージBG関連
 * @authaor	kagaya
 * @data	2008.12.11
 */
//======================================================================
#ifndef __FLDMSGBG_H__
#define	__FLDMSGBG_H__
#include <gflib.h>
#include "system/gfl_use.h"
#include "print/printsys.h"
#include "print/gf_font.h"
#include "system/talkmsgwin.h"

//======================================================================
//	define
//======================================================================
//--------------------------------------------------------------
///	メニュー戻り値
//--------------------------------------------------------------
#define FLDMENUFUNC_NULL (0xffffffff)		///<特に無し
#define FLDMENUFUNC_CANCEL (0xfffffffe)		///<キャンセル

//--------------------------------------------------------------
///	メニュースキップ操作
//--------------------------------------------------------------
typedef enum
{
	FLDMENUFUNC_SKIP_NON =		(0),			///<スキップ無し
	FLDMENUFUNC_SKIP_LRKEY =	(1),			///<LRキースキップ
	FLDMENUFUNC_SKIP_LRBTN = 	(2),			///<LRボタンスキップ
}FLDMENUFUNC_SKIP;

//--------------------------------------------------------------
/// はい、いいえ
//--------------------------------------------------------------
typedef enum
{
  FLDMENUFUNC_YESNO_YES = 0, ///<はい
  FLDMENUFUNC_YESNO_NO, ///<いいえorキャンセル
  
  FLDMENUFUNC_YESNO_NULL, ///<選択中
}FLDMENUFUNC_YESNO;

//--------------------------------------------------------------
/// FLDTALKMSGWIN_IDX 吹き出しウィンドウインデックス
//--------------------------------------------------------------
typedef enum
{
  FLDTALKMSGWIN_IDX_UPPER = 0, ///<画面上部に
  FLDTALKMSGWIN_IDX_LOWER, ///<画面下部に
  FLDTALKMSGWIN_IDX_AUTO, ///<吹き出し元の座標を考慮し自動で配置
}FLDTALKMSGWIN_IDX;

//======================================================================
//	struct
//======================================================================
typedef struct _TAG_FLDMSGBG FLDMSGBG;			///<FLDMSGBG
typedef struct _TAG_FLDMSGPRINT FLDMSGPRINT;	///<FLDMSGPRINT
typedef struct _TAG_FLDMSGWIN FLDMSGWIN;		///<FLDMSGWIN
typedef struct _TAG_FLDMSGPRINT_STREAM FLDMSGPRINT_STREAM; ///<FLDMSGPRINT_STREAM
typedef struct _TAG_FLDMSGWIN_STREAM FLDMSGWIN_STREAM; ///<FLDMSGWIN_STREAM
typedef struct _TAG_FLDMENUFUNC FLDMENUFUNC;	///<FLDMENUFUNC
typedef struct _TAG_FLDTALKMSGWIN FLDTALKMSGWIN; ///FLDTALKMSGWIN

//--------------------------------------------------------------
///	FLDMENUFUNC_LISTDATA
//--------------------------------------------------------------
typedef struct
{
	const void *str;		///<表示する文字列
	u32 param;				///<選択された際に返すパラメタ
}FLDMENUFUNC_LISTDATA;

//--------------------------------------------------------------
///	FLDMENUFUNC_LIST
//--------------------------------------------------------------
typedef struct
{
	u32 str_id;				///<表示する文字列ID
	void *selectParam;		///<選択された際に返すパラメタ
}FLDMENUFUNC_LIST;

//--------------------------------------------------------------
///	FLDMENUFUNC_HEADER
//--------------------------------------------------------------
typedef struct
{
	u16		count;				//リスト項目数
	u16		line;				//表示最大項目数
	u8		rabel_x;			//ラベル表示Ｘ座標
	u8		data_x;				//項目表示Ｘ座標
	u8		cursor_x;			//カーソル表示Ｘ座標
	u8		line_y:4;			//表示Ｙ座標
	u8		f_col:4;			//表示文字色
	u8		b_col:4;			//表示背景色
	u8		s_col:4;			//表示文字影色
	u16		msg_spc:3;			//文字間隔Ｘ
	u16		line_spc:4;			//文字間隔Ｙ
	FLDMENUFUNC_SKIP page_skip:2; //ページスキップタイプ
	u16		font_size_x;		//文字サイズX(ドット
	u16		font_size_y;		//文字サイズY(ドット
	
	u16		bmppos_x;			//表示座標X キャラ単位
	u16		bmppos_y;			//表示座標Y キャラ単位
	u16		bmpsize_x;			//表示サイズX キャラ単位
	u16		bmpsize_y;			//表示サイズY キャラ単位
}FLDMENUFUNC_HEADER;

//======================================================================
//	extern
//======================================================================
//メッセージBG
extern FLDMSGBG * FLDMSGBG_Setup( HEAPID heapID, GFL_G3D_CAMERA *g3Dcamera );
extern void FLDMSGBG_Delete( FLDMSGBG *fmb );
extern void FLDMSGBG_PrintMain( FLDMSGBG *fmb );
extern void FLDMSGBG_PrintG3D( FLDMSGBG *fmb );
extern void FLDMSGBG_ClearPrintQue( FLDMSGBG *fmb );
extern void FLDMSGBG_AllPrint( FLDMSGBG *fmb );
extern BOOL FLDMSGBG_CheckFinishPrint( FLDMSGBG *fmb );
extern PRINT_QUE * FLDMSGBG_GetPrintQue( FLDMSGBG *fmb );
extern GFL_FONT * FLDMSGBG_GetFontHandle( FLDMSGBG *fmb );
extern GFL_MSGDATA * FLDMSGBG_CreateMSGDATA( FLDMSGBG *fmb, u32 arcDatIDMsg );
extern void FLDMSGBG_DeleteMSGDATA( GFL_MSGDATA *msgData );

//メッセージ表示
extern FLDMSGPRINT * FLDMSGPRINT_SetupPrint(
	FLDMSGBG *fmb, GFL_MSGDATA *msgData, GFL_BMPWIN *bmpwin );
extern void FLDMSGPRINT_Delete( FLDMSGPRINT *msgPrint );
extern void FLDMSGPRINT_Print(
	FLDMSGPRINT *msgPrint, u32 x, u32 y, u32 strID );
extern void FLDMSGPRINT_PrintColor(
    FLDMSGPRINT *msgPrint, u32 x, u32 y, u32 strID, PRINTSYS_LSB color );
extern void FLDMSGPRINT_PrintStrBuf(
	FLDMSGPRINT *msgPrint, u32 x, u32 y, STRBUF *strBuf );
extern BOOL FLDMSGPRINT_CheckPrintTrans( FLDMSGPRINT *msgPrint );
extern void FLDMSGPRINT_ChangeBmpWin(
		FLDMSGPRINT *msgPrint, GFL_BMPWIN *bmpwin );
extern void FLDMSGPRINT_ClearBmp( FLDMSGPRINT *msgPrint );
extern void FLDMSGPRINT_FillClearBmp(
    FLDMSGPRINT *msgPrint, u32 x, u32 y, u32 size_x, u32 size_y );
extern PRINT_QUE * FLDMSGPRINT_GetPrintQue( FLDMSGPRINT *msgPrint );
extern GFL_FONT * FLDMSGPRINT_GetFontHandle( FLDMSGPRINT *msgPrint );
extern PRINT_UTIL * FLDMSGPRINT_GetPrintUtil( FLDMSGPRINT *msgPrint );
extern STRBUF * FLDMSGPRINT_GetStrBuf( FLDMSGPRINT *msgPrint );
extern GFL_MSGDATA * FLDMSGPRINT_GetMsgData( FLDMSGPRINT *msgPrint );

//メッセージウィンドウ
extern FLDMSGWIN * FLDMSGWIN_Add( FLDMSGBG *fmb, GFL_MSGDATA *msgData,
	u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y );
extern void FLDMSGWIN_Delete( FLDMSGWIN *msgWin );
extern void FLDMSGWIN_Print( FLDMSGWIN *msgWin, u16 x, u16 y, u32 strID );
extern void FLDMSGWIN_PrintStrBuf( FLDMSGWIN *msgWin, u16 x, u16 y, STRBUF *strBuf );
extern BOOL FLDMSGWIN_CheckPrintTrans( FLDMSGWIN *msgWin );
extern void FLDMSGWIN_ClearWindow( FLDMSGWIN *msgWin );
extern void FLDMSGWIN_FillClearWindow(
    FLDMSGWIN *msgWin, u32 x, u32 y, u32 size_x, u32 size_y );
extern FLDMSGWIN * FLDMSGWIN_AddTalkWin( FLDMSGBG *fmb, GFL_MSGDATA *msgData );

//メニュー
extern FLDMENUFUNC * FLDMENUFUNC_AddMenuList( FLDMSGBG *fmb,
	const FLDMENUFUNC_HEADER *pMenuHead,
	FLDMENUFUNC_LISTDATA *pMenuListData,
  u16 list_pos, u16 cursor_pos );
extern FLDMENUFUNC * FLDMENUFUNC_AddMenu( FLDMSGBG *fmb,
	const FLDMENUFUNC_HEADER *pMenuHead,
	FLDMENUFUNC_LISTDATA *pMenuListData );
extern void FLDMENUFUNC_DeleteMenu( FLDMENUFUNC *menuFunc );
extern u32 FLDMENUFUNC_ProcMenu( FLDMENUFUNC *menuFunc );
extern FLDMENUFUNC_LISTDATA * FLDMENUFUNC_CreateListData( int max, HEAPID heapID );
extern void FLDMENUFUNC_DeleteListData( FLDMENUFUNC_LISTDATA *listData );
extern FLDMENUFUNC_LISTDATA * FLDMENUFUNC_CreateMakeListData(
	const FLDMENUFUNC_LIST *menuList,
	int max, GFL_MSGDATA *msgData, HEAPID heapID );
extern void FLDMENUFUNC_AddStringListData( FLDMENUFUNC_LISTDATA *listData,
		const STRBUF *strBuf, u32 param, HEAPID heapID  );
extern void FLDMENUFUNC_AddArcStringListData(
		FLDMENUFUNC_LISTDATA *listData,
		GFL_MSGDATA *msgData, u32 strID, u32 param, HEAPID heapID );
extern u32 FLDMENUFUNC_GetListLengthMax( const FLDMENUFUNC_LISTDATA *listData );
extern u32 FLDMENUFUNC_GetListMax( const FLDMENUFUNC_LISTDATA *listData );
extern u32 FLDMENUFUNC_GetListMenuWidth(
    const FLDMENUFUNC_LISTDATA *listData, u32 font_size );
extern u32 FLDMENUFUNC_GetListMenuHeight(
    const FLDMENUFUNC_LISTDATA *listData, u32 font_size );
extern void FLDMENUFUNC_InputHeaderListSize(
	FLDMENUFUNC_HEADER *head, u16 count,
	u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y );

//はい、いいえ選択メニュー
extern FLDMENUFUNC * FLDMENUFUNC_AddYesNoMenu(
    FLDMSGBG *fmb, FLDMENUFUNC_YESNO pos );
extern FLDMENUFUNC_YESNO FLDMENUFUNC_ProcYesNoMenu( FLDMENUFUNC *menuFunc );

//プリントストリーム
extern FLDMSGPRINT_STREAM * FLDMSGPRINT_STREAM_SetupPrint(
	FLDMSGBG *fmb, const STRBUF *strbuf,
  GFL_BMPWIN *bmpwin, u16 x, u16 y, int wait );
extern void FLDMSGPRINT_STREAM_Delete( FLDMSGPRINT_STREAM *stm );
extern BOOL FLDMSGPRINT_STREAM_ProcPrint( FLDMSGPRINT_STREAM *stm );

//メッセージウィンドウ　プリントストリーム
extern FLDMSGWIN_STREAM * FLDMSGWIN_STREAM_Add(
    FLDMSGBG *fmb, const GFL_MSGDATA *msgData,
    u16 bmppos_x, u16 bmppos_y, u16 bmpsize_x, u16 bmpsize_y );
extern void FLDMSGWIN_STREAM_Delete( FLDMSGWIN_STREAM *msgWin );
extern void FLDMSGWIN_STREAM_PrintStart(
    FLDMSGWIN_STREAM *msgWin, u16 x, u16 y, u32 strID );
extern void FLDMSGWIN_STREAM_PrintStrBufStart(
    FLDMSGWIN_STREAM *msgWin, u16 x, u16 y, const STRBUF *strBuf );
extern BOOL FLDMSGWIN_STREAM_Print( FLDMSGWIN_STREAM *msgWin );
extern FLDMSGWIN_STREAM * FLDMSGWIN_STREAM_AddTalkWin(
    FLDMSGBG *fmb, GFL_MSGDATA *msgData );
extern void FLDMSGWIN_STREAM_ClearMessage( FLDMSGWIN_STREAM *msgWin );
extern void FLDMSGWIN_STREAM_ClearWindow( FLDMSGWIN_STREAM *msgWin );
extern void FLDMSGWIN_STREAM_WriteWindow( FLDMSGWIN_STREAM *msgWin );

//吹き出しメッセージウィンドウ
extern FLDTALKMSGWIN * FLDTALKMSGWIN_Add( FLDMSGBG *fmb,
    FLDTALKMSGWIN_IDX idx, const VecFx32 *pos,
    const GFL_MSGDATA *msgData, u32 msgID );
extern FLDTALKMSGWIN * FLDTALKMSGWIN_AddStrBuf( FLDMSGBG *fmb,
    FLDTALKMSGWIN_IDX idx, const VecFx32 *pos, STRBUF *strBuf );
extern void FLDTALKMSGWIN_Delete( FLDTALKMSGWIN *tmsg );
extern BOOL FLDTALKMSGWIN_Print( FLDTALKMSGWIN *tmsg );

//その他
extern void FLDMSGBG_SetBlendAlpha( void );

#endif //__FLDMSGBG_H__
