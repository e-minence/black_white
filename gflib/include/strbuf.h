//=============================================================================================
/**
 * @file	strbuf.h
 * @brief	汎用文字列バッファ型オブジェクト
 */
//=============================================================================================
#ifndef __STRBUF_H__
#define __STRBUF_H__

typedef	u16 STRCODE;
typedef struct _STRBUF	STRBUF;

enum {
	EOM_ = 0,
	CR_,

	STR_DEFAULT_CODE_MAX,
};
#define EOM_CODESIZE			(1)

//------------------------------------------------------------------
/**
 * 指定されたポインタがSTRBUFとして有効なものであるかチェック
 *
 * @param   ptr		ポインタ
 *
 * @retval  BOOL	TRUEで有効である
 */
//------------------------------------------------------------------
extern BOOL
	STRBUF_CheckValid
		(const void* ptr);

//------------------------------------------------------------------
/**
 * 文字列バッファオブジェクト作成
 *
 * @param   size		格納できる文字数（EOMを含む）
 * @param   heapID		作成先ヒープID
 *
 * @retval  STRBUF*		オブジェクトへのポインタ
 */
//------------------------------------------------------------------
extern STRBUF*
	GFL_STR_BufferCreate
		( u32 size, u32 heapID );

//------------------------------------------------------------------
/**
 * 文字列バッファオブジェクト削除
 *
 * @param   strbuf		オブジェクトへのポインタ
 *
 */
//------------------------------------------------------------------
extern void
	GFL_STR_BufferDelete
		( STRBUF* strbuf );

//------------------------------------------------------------------
/**
 * 内容をクリアして空文字列にする
 * （Create 直後はクリアされているので呼ぶ必要は無い）
 *
 * @param   strbuf		オブジェクトへのポインタ
 *
 */
//------------------------------------------------------------------
extern void
	GFL_STR_BufferClear
		( STRBUF* strbuf );

//------------------------------------------------------------------
/**
 * バッファ間の文字列コピー
 *
 * @param   dst		コピー先バッファオブジェクト
 * @param   src		コピー元バッファオブジェクト
 *
 */
//------------------------------------------------------------------
extern void
	GFL_STR_BufferCopy
		( STRBUF* dst, const STRBUF* src );

//------------------------------------------------------------------
/**
 * 同じ内容のバッファを複製する
 *
 * @param   strbuf		複製元のバッファオブジェクト
 *
 * @retval  STRBUF*		複製されたバッファオブジェクト
 */
//------------------------------------------------------------------
extern STRBUF*
	GFL_STR_CreateBufferCopy
		( const STRBUF* origin, u32 heapID );

//------------------------------------------------------------------
/**
 * 文字列を比較する
 *
 * @param   str1		文字列１
 * @param   str2		文字列２
 *
 * @retval  BOOL		TRUEで一致
 */
//------------------------------------------------------------------
extern BOOL
	GFL_STR_BufferCompare
		( const STRBUF* str1, const STRBUF* str2 );

//------------------------------------------------------------------
/**
 * 格納されている文字数を返す
 *
 * @param   strbuf		文字列バッファオブジェクト
 *
 * @retval  u32		文字数
 */
//------------------------------------------------------------------
extern u32
	GFL_STR_GetBufferLength
		( const STRBUF* str );


//==============================================================================================
// 生の文字配列を取り扱う関数群
// これらを利用できるモジュールは出来る限り限定する
//==============================================================================================
//------------------------------------------------------------------
/**
 * 生の文字列（EOM終端）をバッファオブジェクトにセットする
 *
 * @param   strbuf		[out] バッファオブジェクトへのポインタ
 * @param   sz			[in]  EOM で終わる文字配列
 *
 */
//------------------------------------------------------------------
extern void
	GFL_STR_SetStringCode
		( STRBUF* strbuf, const STRCODE* sz );

//------------------------------------------------------------------
/**
 * 生の文字列（長さ指定）をバッファオブジェクトにセットする
 *
 * @param   strbuf		[out] バッファオブジェクトへのポインタ
 * @param   str			[in]  文字配列の先頭ポインタ
 * @param   len			[in]  セットする文字数（EOMを含む）
 *
 */
//------------------------------------------------------------------
extern void
	GFL_STR_SetStringCodeOrderLength
		( STRBUF* strbuf, const STRCODE* str, u32 len );

//------------------------------------------------------------------
/**
 * バッファから生の文字配列をコピーする
 *
 * @param   strbuf		[in]  バッファオブジェクトへのポインタ
 * @param   ary			[out] コピー先配列
 * @param   arysize		[in]  コピー先配列の要素数
 *
 */
//------------------------------------------------------------------
extern void
	GFL_STR_GetStringCode
		( const STRBUF* strbuf, STRCODE* ary, u32 arysize );

//------------------------------------------------------------------
/**
 * バッファオブジェクトが内包する文字配列のアドレスを返す
 * ※この関数を呼び出す箇所は超限定。たぶん文字出力系のみ。
 *
 * @param   strbuf				[in] バッファオブジェクトへのポインタ
 *
 * @retval  const STRCODE*		文字配列のアドレス
 */
//------------------------------------------------------------------
extern const STRCODE* 
	GFL_STR_GetStringCodePointer
		( const STRBUF* strbuf );


//==============================================================================================
// 文字列の連結操作を行う関数。【【 原則使用禁止 】】
// 使って良いのはローカライズを考慮した文字列処理モジュールのみ。
//==============================================================================================
//------------------------------------------------------------------
/**
 * 文字列連結
 *
 * @param   dst		連結されるバッファオブジェクト
 * @param   src		連結する文字列を持つバッファオブジェクト
 *
 */
//------------------------------------------------------------------
extern void
	GFL_STR_AddStr
		( STRBUF* dst, const STRBUF* src );

//------------------------------------------------------------------
/**
 * １文字連結
 *
 * @param   dst			連結されるバッファオブジェクト
 * @param   code		連結する文字コード
 *
 */
//------------------------------------------------------------------
extern void
	GFL_STR_AddChar
		( STRBUF* dst, STRCODE code );

#endif
