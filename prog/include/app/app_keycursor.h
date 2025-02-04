//============================================================================
/**
 *
 *	@file		app_keycursor.h
 *	@brief  メッセージ、キーカーソル（メッセージ送りアイコン）
 *	@author	hosaka genya(kagayaさんのソースを参考に共通化)
 *	@data		2010.01.05
 *
 */
//============================================================================
#pragma once

#include "print/printsys.h"

// 不完全型
typedef struct _APP_KEYCURSOR_WORK APP_KEYCURSOR_WORK;

//=============================================================================
/**
 *								定数定義
 */
//=============================================================================

//=============================================================================
/**
 *								構造体定義
 */
//=============================================================================

//=============================================================================
/**
 *								EXTERN宣言
 */
//=============================================================================

//--------------------------------------------------------------
/**
 * キー送りカーソル 初期化
 *  @param n_col      [IN] 透明色指定 0-15,GF_BMPPRT_NOTNUKI	
 *  @param is_decide_key  [IN] TRUE:決定ボタン、キャンセルボタンでカーソルを消す。
 *  @param is_decide_tp   [IN] TRUE:タッチでカーソルを消す。
 *  @param heapID     [IN] HEAPID ヒープID
 *  @retval nothing
 */
//--------------------------------------------------------------
extern APP_KEYCURSOR_WORK* APP_KEYCURSOR_Create( u16 n_col, BOOL is_decide_key, BOOL is_decide_tp, HEAPID heap_id );

//--------------------------------------------------------------
/**
 * キー送りカーソル 初期化
 *  @param n_col      [IN] 透明色指定 0-15,GF_BMPPRT_NOTNUKI	
 *  @param is_decide_key  [IN] TRUE:決定ボタン、キャンセルボタンでカーソルを消す。
 *  @param is_decide_tp   [IN] TRUE:タッチでカーソルを消す。
 *  @param heapID     [IN] HEAPID ヒープID
 *  @param skip_cursor_character    [IN] キャラエリアへのポインタ(APP_KEYCURSOR_Deleteするまで呼び出し元で保持すること)
 *                                       (app_keycursor.c内にあるsc_skip_cursor_characterを参考に用意して下さい)
 *  @retval nothing
 */
//--------------------------------------------------------------
extern APP_KEYCURSOR_WORK* APP_KEYCURSOR_CreateEx( u16 n_col, BOOL is_decide_key, BOOL is_decide_tp, HEAPID heap_id,
                                                   u8* skip_cursor_character );

//--------------------------------------------------------------
/**
 *  @brief  キー送りカーソル 削除
 *  @param  work メインワーク
 *  @retval nothing
 */
//--------------------------------------------------------------
extern void APP_KEYCURSOR_Delete( APP_KEYCURSOR_WORK *work );

//-----------------------------------------------------------------------------
/**
 *	@brief  キーカーソル 主処理
 *
 *	@param	work メインワーク
 *	@param	stream プリントストリーム
 *	@note  メッセージ表示中に毎SYNC呼ぶ。streamがNULLの時は呼ばないようにする。
 *
 *	@retval nothing
 */
//-----------------------------------------------------------------------------
extern void APP_KEYCURSOR_Main( APP_KEYCURSOR_WORK* work, PRINT_STREAM* stream, GFL_BMPWIN* msgwin );


//====================================================================================
// 以下、Mainに内包されているので、基本的には外部から使用しない関数。念のため公開
//====================================================================================


//--------------------------------------------------------------
/**
 * キー送りカーソル クリア
 * @param work  メインワーク
 * @param bmp   表示先GFL_BMP_DATA
 * @param n_col 透明色指定 0-15,GF_BMPPRT_NOTNUKI	
 * @retval nothing
 */
//--------------------------------------------------------------
extern void APP_KEYCURSOR_Clear( APP_KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col );

//--------------------------------------------------------------
/**
 * キー送りカーソル 表示
 * @param work APP_KEYCURSOR_WORK
 * @param bmp 表示先GFL_BMP_DATA
 * @param n_col 透明色指定 0-15,GF_BMPPRT_NOTNUKI	
 * @retval nothing
 */
//--------------------------------------------------------------
extern void APP_KEYCURSOR_Write( APP_KEYCURSOR_WORK *work, GFL_BMP_DATA *bmp, u16 n_col );

