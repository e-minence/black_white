//=============================================================================================
/**
 * @file	printsys.c
 * @brief	Bitmap利用文字描画　システム制御
 * @author	taya
 *
 * @date	2007.02.06	作成
 * @date	2008.09.11	通信対応キュー構造を作成
 * @date	2008.09.26	キュー構造を利用しない文字列描画処理も復活
 */
//=============================================================================================
#ifndef __PRINTSYS_H__
#define __PRINTSYS_H__

#include "print\gf_font.h"


//============================================================================================================
// Typedefs

//--------------------------------------------------------------
/**
 *	Print Queue
 *	通信時に描画処理を小刻みに実行できるよう、処理内容を蓄積するための機構。
 */
//--------------------------------------------------------------
typedef struct _PRINT_QUE	PRINT_QUE;

//--------------------------------------------------------------------------
/**
 *	Print Stream
 *	会話ウィンドウ等、１定間隔で１文字ずつ描画，表示を行うための機構
 */
//--------------------------------------------------------------------------
typedef struct _PRINT_STREAM	PRINT_STREAM;

//--------------------------------------------------------------------------
/**
 *	Print Stream Callback
 *	Print Stream で１文字描画ごとに呼び出されるコールバック関数の型
 */
//--------------------------------------------------------------------------
typedef void (*pPrintCallBack)(u32);


//============================================================================================================
// Consts

//--------------------------------------------------------------------------
/**
 *	Print Stream の状態を識別
 */
//--------------------------------------------------------------------------
typedef enum {
	PRINTSTREAM_STATE_RUNNING = 0,	///< 処理実行中（文字列が流れている）
	PRINTSTREAM_STATE_PAUSE,		///< 一時停止中（ページ切り替え待ち等）
	PRINTSTREAM_STATE_DONE,			///< 文字列終端まで表示完了
}PRINTSTREAM_STATE;

//--------------------------------------------------------------------------
/**
 *	Print Stream の一時停止状態を識別
 */
//--------------------------------------------------------------------------
typedef enum {
	PRINTSTREAM_PAUSE_LINEFEED = 0,	///< ページ切り替え待ち（改行）
	PRINTSTREAM_PAUSE_CLEAR,		/// <ページ切り替え待ち（表示クリア）
}PRINTSTREAM_PAUSE_TYPE;



//============================================================================================================
// Prototypes

//==============================================================================================
/**
 * システム初期化（プログラム起動時に１度だけ呼び出す）
 *
 * @param   heapID		初期化用ヒープID
 *
 */
//==============================================================================================
extern void PRINTSYS_Init( HEAPID heapID );

//==============================================================================================
/**
 * プリントキューをデフォルトのバッファサイズで生成する。
 *
 * @param   heapID			生成用ヒープID
 *
 * @retval  PRINT_QUE*		生成されたプリントキュー
 */
//==============================================================================================
extern PRINT_QUE* PRINTSYS_QUE_Create( HEAPID heapID );

//==============================================================================================
/**
 * プリントキューをサイズ指定して生成する
 *
 * @param   size			バッファサイズ
 * @param   heapID			生成用ヒープID
 *
 * @retval  PRINT_QUE*		生成されたプリントキュー
 */
//==============================================================================================
extern PRINT_QUE* PRINTSYS_QUE_CreateEx( u16 buf_size, HEAPID heapID );

//==============================================================================================
/**
 * プリントキューメイン処理（メインループ中で１回呼び出す）
 *
 * @param   que		プリントキュー
 *
 * @retval  BOOL	キュー処理が残っていない場合はTRUE／にまだ処理が残っている場合はFALSE
 */
//==============================================================================================
extern BOOL PRINTSYS_QUE_Main( PRINT_QUE* que );

//==============================================================================================
/**
 * プリントキューに処理が残っているか判定
 *
 * @param   que		プリントキュー
 *
 * @retval  BOOL	キュー処理が残っていない場合はTRUE／にまだ処理が残っている場合はFALSE
 */
//==============================================================================================
extern BOOL PRINTSYS_QUE_IsFinished( const PRINT_QUE* que );

//==============================================================================================
/**
 * プリントキューに、特定Bitmapを出力対象とした処理が残っているか判定
 *
 * @param   que				プリントキュー
 * @param   targetBmp		出力対象のBitmap
 *
 * @retval  BOOL			targetBmpに対する処理が残っている場合はTRUE／それ以外はFALSE
 */
//==============================================================================================
extern BOOL PRINTSYS_QUE_IsExistTarget( const PRINT_QUE* que, const GFL_BMP_DATA* targetBmp );

//==============================================================================================
/**
 * プリントキューを介した文字列描画
 *
 * @param   que		[out] 描画処理内容を記録するためのプリントキュー
 * @param   dst		[out] 描画先Bitmap
 * @param   xpos	[in]  描画開始Ｘ座標（ドット）
 * @param   ypos	[in]  描画開始Ｙ座標（ドット）
 * @param   str		[in]  文字列
 * @param   font	[in]  フォント
 *
 */
//==============================================================================================
extern void PRINTSYS_PrintQue( PRINT_QUE* que, GFL_BMP_DATA* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font );

//==============================================================================================
/**
 * Bitmap へ直接の文字列描画
 *
 * @param   dst		[out] 描画先Bitmap
 * @param   xpos	[in]  描画開始Ｘ座標（ドット）
 * @param   ypos	[in]  描画開始Ｙ座標（ドット）
 * @param   str		[in]  文字列
 * @param   font	[in]  フォント
 *
 */
//==============================================================================================
extern void PRINTSYS_Print( GFL_BMP_DATA* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font );

//==============================================================================================
/**
 * プリントストリームを利用した文字列描画（通常版 - コールバックなし）
 *
 * @param   dst			描画先Bitmap
 * @param   xpos		描画開始Ｘ座標（ドット）
 * @param   ypos		描画開始Ｙ座標（ドット）
 * @param   str			文字列
 * @param   font		フォントハンドラ
 * @param   wait		１文字描画ごとのウェイトフレーム数
 * @param   tcbsys		使用するGFL_TCBLシステムポインタ
 * @param   tcbpri		使用するタスクプライオリティ
 * @param   heapID		使用するヒープID
 *
 * @retval	PRINT_STREAM*	ストリームハンドル
 */
//==============================================================================================
extern PRINT_STREAM* PRINTSYS_PrintStream(
		GFL_BMPWIN* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font,
		u16 wait, GFL_TCBLSYS* tcbsys, u32 tcbpri, HEAPID heapID, u16 clearColor );

//==============================================================================================
/**
 * プリントストリームを利用した文字列描画（コールバックあり）
 *
 * @param   dst			描画先Bitmap
 * @param   xpos		描画開始Ｘ座標（ドット）
 * @param   ypos		描画開始Ｙ座標（ドット）
 * @param   str			文字列
 * @param   font		フォントハンドラ
 * @param   wait		１文字描画ごとのウェイトフレーム数
 * @param   tcbsys		使用するGFL_TCBLシステムポインタ
 * @param   tcbpri		使用するタスクプライオリティ
 * @param   heapID		使用するヒープID
 * @param   callback	１文字描画ごとのコールバック関数アドレス
 *
 * @retval	PRINT_STREAM*	ストリームハンドラ
 */
//==============================================================================================
extern PRINT_STREAM* PRINTSYS_PrintStreamCallBack(
		GFL_BMPWIN* dst, u16 xpos, u16 ypos, const STRBUF* str, GFL_FONT* font,
		u16 wait, GFL_TCBLSYS* tcbsys, u32 tcbpri, HEAPID heapID, u16 clearColor, pPrintCallBack callback );

//==============================================================================================
/**
 * プリントストリーム状態取得
 *
 * @param   handle		ストリームハンドラ
 *
 * @retval  PRINTSTREAM_STATE		状態
 */
//==============================================================================================
extern PRINTSTREAM_STATE PRINTSYS_PrintStreamGetState( PRINT_STREAM* handle );

//==============================================================================================
/**
 * プリントストリーム一時停止タイプ取得
 *
 * @param   handle		ストリームハンドラ
 *
 * @retval  PRINTSTREAM_PAUSE_TYPE		停止タイプ
 */
//==============================================================================================
extern PRINTSTREAM_PAUSE_TYPE PRINTSYS_PrintStreamGetPauseType( PRINT_STREAM* handle );

//==============================================================================================
/**
 * プリントストリーム一時停止を解除
 *
 * @param   handle		ストリームハンドラ
 *
 */
//==============================================================================================
extern void PRINTSYS_PrintStreamReleasePause( PRINT_STREAM* handle );

//==============================================================================================
/**
 * プリントストリーム削除
 *
 * @param   handle		ストリームハンドラ
 *
 */
//==============================================================================================
extern void PRINTSYS_PrintStreamDelete( PRINT_STREAM* handle );

//==============================================================================================
/**
 * プリントストリームウェイト短縮（キー押し中などに毎フレーム呼び出すことで強制的にウェイトを短縮）
 *
 * @param   handle		ストリームハンドラ
 * @param   wait		設定するウェイトフレーム数
 *
 */
//==============================================================================================
extern void PRINTSYS_PrintStreamShortWait( PRINT_STREAM* handle, u16 wait );



//==============================================================================================
/**
 * 文字列をBitmap表示する際の幅（ドット数）を計算
 * ※文字列が複数行ある場合、その最長行のドット数
 *
 * @param   str			文字列
 * @param   font		フォントハンドラ
 * @param   margin		字間スペース（ドット）
 *
 * @retval  u32			文字列幅（ドット）
 */
//==============================================================================================
extern u32 PRINTSYS_GetStrWidth( const STRBUF* str, GFL_FONT* font, u16 margin );







//--------------------------------------------------------------
/**
 * プリントキュー用ユーティリティ
 *
 * プリントキューを簡単に使うためのユーティリティ類を用意する予定…
 */
//--------------------------------------------------------------


//--------------------------------------------------------------
/**
 *	ユーティリティ管理構造体
 *
 *	中身を公開してるのでAlloc不要。
 *	各自ワーク領域に必要なだけ作ってください
 */
//--------------------------------------------------------------
typedef struct {
	GFL_BMPWIN*		win;
	u8				transReq;
}PRINT_UTIL;


//--------------------------------------------------------------------------------------
/**
 * ユーティリティ初期化（BitmapWindowと１対１で関連付けを行う。１回だけ呼び出し）
 *
 * @param   wk		
 * @param   win		
 */
//--------------------------------------------------------------------------------------
inline void PRINT_UTIL_Setup( PRINT_UTIL* wk, GFL_BMPWIN* win )
{
	wk->win = win;
	wk->transReq = FALSE;
}

//--------------------------------------------------------------------------------------
/**
 * ユーティリティを介してBitmap文字列描画
 *
 * @param   wk			
 * @param   que			
 * @param   xpos		
 * @param   ypos		
 * @param   buf			
 * @param   font		
 *
 */
//--------------------------------------------------------------------------------------
inline void PRINT_UTIL_Print( PRINT_UTIL* wk, PRINT_QUE* que, u16 xpos, u16 ypos, const STRBUF* buf, GFL_FONT* font )
{
	PRINTSYS_PrintQue( que, GFL_BMPWIN_GetBmp(wk->win), xpos, ypos, buf, font );
	wk->transReq = TRUE;
}


//--------------------------------------------------------------------------------------
/**
 * 必要なタイミングを判断して、CGXデータのVRAM転送を行います
 *
 * @param   wk		
 * @param   que		
 *
 * @retval  BOOL	まだ転送が終わっていない場合はTRUE／それ以外FALSE
 */
//--------------------------------------------------------------------------------------
inline BOOL PRINT_UTIL_Trans( PRINT_UTIL* wk, PRINT_QUE* que )
{
	if( wk->transReq )
	{
		if( !PRINTSYS_QUE_IsExistTarget(que, GFL_BMPWIN_GetBmp(wk->win)) )
		{
			GFL_BMPWIN_TransVramCharacter( wk->win );
			wk->transReq = FALSE;
		}
	}
	return wk->transReq;
}






#endif
