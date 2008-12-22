//============================================================================================
/**
 * @file	file_util.h
 * @brief	ファイルアクセスラッパー
 * @author	tamada GAME FREAK inc.
 * @date	2008.12.19
 *
 * ファイルアクセス用関数について、gflib/arc_tool.hとインターフェイスをそろえたもの
 * 最終的にarc_tool.hに統合する可能性が高い
 */
//============================================================================================
#pragma once

#include <gflib.h>
//============================================================================================
//============================================================================================
//--------------------------------------------------------------------------------------
/**
 * @brief	ファイルデータハンドルの不完全宣言
 */
//--------------------------------------------------------------------------------------
typedef struct _FILEHANDLE FILEHANDLE;


//============================================================================================
//
//		ハンドル経由アクセス関数
//
//============================================================================================
//--------------------------------------------------------------------------------------
/**
 * データハンドルオープン 
 * （内部でファイルをオープンします）
 *
 * @param   arcID		アーカイブデータインデックス
 * @param   heapID		管理用ヒープＩＤ
 *
 * @retval  FILEHANDLE	オープンされたハンドルのポインタ（失敗ならNULL）
 */
//--------------------------------------------------------------------------------------
extern FILEHANDLE * GFL_FILE_OpenHandle(ARCID arcID, HEAPID heapID);


//--------------------------------------------------------------------------------------
/**
 * データハンドルクローズ
 * （内部でファイルをクローズします）
 *
 * @param   handle			ハンドルポインタ
 */
//--------------------------------------------------------------------------------------
extern void GFL_FILE_CloseHandle(FILEHANDLE* handle);

//--------------------------------------------------------------------------------------
/**
 * アーカイブデータハンドルを使ってデータサイズ取得
 *
 * @param   handle		ハンドルポインタ
 *
 * @retval	u32			インデックスのデータサイズ
 */
//--------------------------------------------------------------------------------------
extern u32 GFL_FILE_GetSizeByHandle(FILEHANDLE* handle);

//--------------------------------------------------------------------------------------
/**
 * データハンドルを使ってデータ取得
 *
 * @param   handle		ハンドルポインタ
 * @param   buffer		データ読み込み先バッファ
 */
//--------------------------------------------------------------------------------------
extern void GFL_FILE_LoadDataByHandle(FILEHANDLE* handle, void * buffer);

//--------------------------------------------------------------------------------------
/**
 * データハンドルを使ってデータ取得（内部でアロケートしたメモリに）
 *
 * @param   handle			ハンドルポインタ
 * @param   heapID			メモリアロケート用のヒープＩＤ
 *
 * @return	void*			読み込みデータのアドレス
 */
//--------------------------------------------------------------------------------------
extern void* GFL_FILE_LoadDataAllocByHandle(FILEHANDLE* handle, HEAPID heapID);

//--------------------------------------------------------------------------------------
/**
 * データハンドルを使ってデータ取得（読み込み開始オフセット＆サイズ指定）
 *
 * @param   handle		ハンドルポインタ
 * @param   ofs			読み込み開始オフセット
 * @param   size		読み込みサイズ
 * @param   buffer		データ読み込み先バッファ
 */
//--------------------------------------------------------------------------------------
extern void GFL_FILE_LoadDataOfsByHandle(FILEHANDLE* handle, u32 ofs, u32 size, void * buffer);

//--------------------------------------------------------------------------------------
/**
 * データハンドルを使ってデータ取得（読み込み開始オフセット＆サイズ指定、内部でメモリアロケート）
 *
 * @param   handle		ハンドルポインタ
 * @param   heapID		メモリアロケート用のヒープＩＤ
 * @param   ofs			読み込み開始オフセット
 * @param   size		読み込みサイズ
 *
 * @return	void*		読み込みデータのアドレス
 */
//--------------------------------------------------------------------------------------
extern void* GFL_FILE_LoadDataAllocOfsByHandle(FILEHANDLE* handle, HEAPID heapID, u32 ofs, u32 size);

//--------------------------------------------------------------------------------------
/**
 * データハンドルを介してファイルシーク
 *
 * @param   handle		ハンドルポインタ
 * @param   seek_size	シークするバイト数
 */
//--------------------------------------------------------------------------------------
extern void GFL_FILE_SeekByHandle(FILEHANDLE* handle, u32 seek_size);

//============================================================================================
//
//	ID指定直接アクセス関数
//
//============================================================================================
//--------------------------------------------------------------------------------------
/**
 * データサイズ取得
 *
 * @param   arcID		アーカイブデータインデックス
 *
 * @return	u32			データサイズ
 */
//--------------------------------------------------------------------------------------
extern u32 GFL_FILE_GetDataSize(ARCID arcID);

//--------------------------------------------------------------------------------------
/**
 * データロード
 *
 * @param	buffer		データ読み込み用バッファの先頭アドレス
 * @param   arcID		アーカイブデータインデックス
 */
//--------------------------------------------------------------------------------------
extern void GFL_FILE_LoadData(void * buffer, ARCID arcID);

//--------------------------------------------------------------------------------------
/**
 * データロード（内部でメモリアロケート）
 *
 * @param   arcID		アーカイブデータインデックス
 * @param   heapID		メモリアロケート用のヒープＩＤ
 *
 * @return	void*		読み込みデータのアドレス
 */
//--------------------------------------------------------------------------------------
extern void* GFL_FILE_LoadDataAlloc(ARCID arcID, HEAPID heapID);

//--------------------------------------------------------------------------------------
/**
 * データロード（読み込み開始オフセット＆サイズ指定）
 *
 *
 * @param   buffer		データ読み込み先バッファ
 * @param   arcID		アーカイブデータインデックス
 * @param   ofs			読み込み開始オフセット
 * @param   size		読み込みサイズ
 *
 */
//--------------------------------------------------------------------------------------
extern void GFL_FILE_LoadDataOfs(void * buffer, ARCID arcID, u32 ofs, u32 size);

//--------------------------------------------------------------------------------------
/**
 * データロード（読み込み開始オフセット＆サイズ指定、内部でメモリアロケート）
 *
 * @param   arcID		アーカイブデータインデックス
 * @param   heapID		メモリアロケート用のヒープＩＤ
 * @param   ofs			読み込み開始オフセット
 * @param   size		読み込みサイズ
 *
 * @return	void*		読み込みデータのアドレス
 */
//--------------------------------------------------------------------------------------
extern void* GFL_FILE_LoadDataAllocOfs(ARCID arcID, HEAPID heapID, u32 ofs, u32 size);

