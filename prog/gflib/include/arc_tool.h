
//============================================================================================
/**
 * @file	arc_tool.h
 * @bfief	アーカイバ用ツールプログラム
 * @author	HisashiSogabe
 * @date	05.05.30
 */
//============================================================================================

#ifndef	__ARC_TOOL_H_
#define	__ARC_TOOL_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _GFL_ARC_PARAM GFL_ARC_PARAM;

//============================================================================================
/**
 *	アーカイブシステム初期化
 *
 * @param[in]	tbl		アーカイブデータテーブルのアドレス
 * @param[in]	tbl_max	アーカイブデータテーブルの要素数のMAX
 */
//============================================================================================
extern	void	GFL_ARC_Init(const char **tbl,int tbl_max);

//============================================================================================
/**
 *	アーカイブシステム初期化（マルチブート対応）
 *
 * @param[in]	tbl		アーカイブデータテーブルのアドレス
 * @param[in]	tbl_max	アーカイブデータテーブルの要素数のMAX
 */
//============================================================================================
extern	void	GFL_ARC_InitMultiBoot(const char **tbl,int tbl_max);

//============================================================================================
/**
 *	アーカイブシステム終了
 */
//============================================================================================
extern	void	GFL_ARC_Exit(void);

//============================================================================================
/**
 *	アーカイブデータテーブルアドレス取得
 */
//============================================================================================
extern	void	GFL_ARC_GetArchiveTableAddress(GFL_ARC_PARAM *gap_dest);

//============================================================================================
/**
 *	アーカイブデータテーブルアドレス取得
 */
//============================================================================================
extern	void	GFL_ARC_SetArchiveTableAddress(GFL_ARC_PARAM *gap_src);

//============================================================================================
/**
 *	アーカイブデータテーブル構造体サイズを得る
 */
//============================================================================================
extern int	GFL_ARC_GetArchiveTableSize(void);

//============================================================================================
/**
 * アーカイブファイルデータロード
 *
 * @param[in]	data		読み込んだデータを格納するワークのポインタ
 * @param[in]	arcID		読み込むアーカイブファイルの種類インデックスナンバー
 * @param[in]	datID		読み込むデータのアーカイブファイル上のインデックスナンバー
 */
//============================================================================================
extern	void	GFL_ARC_LoadData(void *data, int arcID, int datID);

//============================================================================================
/**
 * アーカイブファイルデータロード（読み込んだデータを格納するワークを関数内で確保するバージョン）
 *
 * ※関数内でワークをAllocするので、自前で開放処理をする必要があります
 *
 * @param[in]	arcID		読み込むアーカイブファイルの種類インデックスナンバー
 * @param[in]	datID		読み込むデータのアーカイブファイル上のインデックスナンバー
 * @param[in]	heapID		メモリを確保するヒープ領域のID
 *
 * @retval	関数内で確保したデータ格納ワークのアドレス
 */
//============================================================================================
extern	void*	GFL_ARC_LoadDataAlloc(int arcID, int datID, HEAPID heapID);

//============================================================================================
/**
 * アーカイブファイルデータロード（オフセット指定あり）
 *
 * @param[in]	data		読み込んだデータを格納するワークのポインタ
 * @param[in]	arcID		読み込むアーカイブファイルの種類インデックスナンバー
 * @param[in]	datID		読み込むデータのアーカイブファイル上のインデックスナンバー
 * @param[in]	ofs			読み込むデータの先頭からのオフセット
 * @param[in]	size		読み込むデータサイズ
 */
//============================================================================================
extern	void	GFL_ARC_LoadDataOfs(void *data, int arcID, int datID, int ofs, int size);

//============================================================================================
/**
 * アーカイブファイルデータロード（読み込んだデータを格納するワークを関数内で確保するバージョンとオフセット指定あり）
 *
 * ※関数内でワークをAllocするので、自前で開放処理をする必要があります
 *
 * @param[in]	arcID		読み込むアーカイブファイルの種類インデックスナンバー
 * @param[in]	datID		読み込むデータのアーカイブファイル上のインデックスナンバー
 * @param[in]	heapID		メモリを確保するヒープ領域のID
 * @param[in]	ofs			読み込むデータの先頭からのオフセット
 * @param[in]	size		読み込むデータサイズ
 *
 * @retval	関数内で確保したデータ格納ワークのアドレス
 */
//============================================================================================
extern	void*	GFL_ARC_LoadDataAllocOfs(int arcID, int datID, HEAPID heapID, int ofs, int size);

//============================================================================================
/**
 *	nnsarcで作成したアーカイブファイルに対して直接ファイル名を指定して任意のデータを取り出す
 *	読み込んだデータを格納するワークもこの関数内で確保して、ポインタを返す
 *
 * @param[in]	name		読み込むアーカイブファイル名
 * @param[in]	datID		読み込むデータのアーカイブ上のインデックスナンバー
 * @param[in]	heapID		メモリを確保するヒープ領域のID
 *
 * @retval	関数内で確保したデータ格納ワークのアドレス
 */
//============================================================================================
extern	void	GFL_ARC_LoadDataPath(void *data, const char* name, int datID);
extern	void*	GFL_ARC_LoadDataFilePathAlloc(const char *name,int datID,HEAPID heapID);

//============================================================================================
/**
 * アーカイブファイルデータのファイル数を取得
 *
 * @param[in]	arcID	読み込むアーカイブファイルの種類インデックスナンバー
 * @param[in]	datID		読み込むデータのアーカイブファイル上のインデックスナンバー
 *
 * @retval	アーカイブファイルデータのファイル数
 */
//============================================================================================
extern	u16	GFL_ARC_GetDataFileCnt(int arcID, int datID);

//============================================================================================
/**
 * アーカイブファイルデータのサイズを取得
 *
 *	ArchiveDataLoadを使用する時、確保するメモリサイズを取得するのに使用します
 *
 * @param[in]	arcID		読み込むアーカイブファイルの種類インデックスナンバー（arc_tool.hに記述）
 * @param[in]	datID		読み込むデータのアーカイブファイル上のインデックスナンバー
 *
 * @retval	アーカイブファイルデータのサイズ
 */
//============================================================================================
extern	u32	GFL_ARC_GetDataSize(int arcID,int datID);

/*====================================================================================*/
/*
  アーカイブデータハンドル処理

    １つのアーカイブファイルから何度もデータ取得を行いたい時、ファイルの
  オープン・クローズ回数を減らすことで処理負荷を軽減させるための仕組み。
*/
/*====================================================================================*/

//--------------------------------------------------
/**
 * アーカイブデータハンドル構造体の宣言
 */
//--------------------------------------------------
typedef struct _ARCHANDLE  ARCHANDLE;

//------------------------------------------------------------------
/**
 * アーカイブデータハンドルオープン
 * （内部でファイルをオープンします）
 *
 * @param   arcID		アーカイブデータインデックス
 * @param   heapID		管理用ヒープＩＤ
 *
 * @retval  ARCHANDLE	オープンされたハンドルのポインタ（失敗ならNULL）
 */
//------------------------------------------------------------------
extern ARCHANDLE* GFL_ARC_OpenDataHandle( u32 arcID, HEAPID heapID );

//------------------------------------------------------------------
/**
 * アーカイブデータハンドルクローズ
 * （内部でファイルをクローズします）
 *
 * @param   handle			ハンドルポインタ
 */
//------------------------------------------------------------------
extern void GFL_ARC_CloseDataHandle( ARCHANDLE* handle );

//------------------------------------------------------------------
/**
 * アーカイブデータハンドルを使ってデータ取得
 *
 * @param   handle		ハンドルポインタ
 * @param   datID		アーカイブ内のデータインデックス
 * @param   buffer		データ読み込み先バッファ
 */
//------------------------------------------------------------------
extern void GFL_ARC_LoadDataByHandle( ARCHANDLE* handle, u32 datID, void* buffer );

//------------------------------------------------------------------
/**
 * アーカイブデータハンドルを使ってデータサイズ取得
 *
 * @param   handle		ハンドルポインタ
 * @param   datID		アーカイブ内のデータインデックス
 *
 * @retval	u32			インデックスのデータサイズ
 */
//------------------------------------------------------------------
extern u32 GFL_ARC_GetDataSizeByHandle( ARCHANDLE* handle, u32 datID );

//------------------------------------------------------------------
/**
 * アーカイブデータハンドルを使ってデータ取得（読み込み開始オフセット＆サイズ指定）
 *
 * @param   handle		ハンドルポインタ
 * @param   datID		アーカイブ内のデータインデックス
 * @param   ofs			読み込み開始オフセット
 * @param   size		読み込みサイズ
 * @param   buffer		データ読み込み先バッファ
 */
//------------------------------------------------------------------
extern void GFL_ARC_LoadDataOfsByHandle( ARCHANDLE* handle, u32 datID, u32 ofs, u32 size, void* buffer );

//------------------------------------------------------------------
/**
 * アーカイブデータハンドルを使ってデータ取得（内部でアロケートしたメモリに）
 *
 * @param   handle			ハンドルポインタ
 * @param   datID			アーカイブ内のデータインデックス
 * @param   heapID			メモリアロケート用のヒープＩＤ
 *
 * @retval  u32				データサイズ（バイト）
 */
//------------------------------------------------------------------
extern void* GFL_ARC_LoadDataAllocByHandle( ARCHANDLE* handle, u32 datID, HEAPID heapID );

//------------------------------------------------------------------
/**
 * アーカイブデータハンドルを使ってイメージデータオフセット取得
 *
 * @param   handle		ハンドルポインタ
 * @param   datID		アーカイブ内のデータインデックス
 * @param   offset		オフセット読み込み先バッファ
 */
//------------------------------------------------------------------
extern void GFL_ARC_LoadDataImgofsByHandle( ARCHANDLE* handle, u32 datID, u32* offset );

//------------------------------------------------------------------
/**
 * アーカイブデータハンドルを使ってデータ取得（ファイルハンドル内情報の続きから読み込み）
 *
 * @param   handle		ハンドルポインタ
 * @param   size		読み込みサイズ
 * @param   buffer		データ読み込み先バッファ
 */
//------------------------------------------------------------------
extern	void GFL_ARC_LoadDataByHandleContinue( ARCHANDLE* handle, u32 size, void* buffer );

//------------------------------------------------------------------
/**
 * アーカイブデータハンドルを介してファイルシーク
 *
 * @param   handle		ハンドルポインタ
 * @param   size
 */
//------------------------------------------------------------------
extern	void GFL_ARC_SeekDataByHandle( ARCHANDLE* handle, u32 size );

//============================================================================================
/**
 * アーカイブファイルデータのファイル数を取得(ハンドル使用)
 *
 * @param	handle			ハンドルポインタ
 *
 * @retval	u16				ファイル数	
 */
//============================================================================================
extern	u16		GFL_ARC_GetDataFileCntByHandle(ARCHANDLE* handle);

//============================================================================================
/**
 * アーカイブファイルデータをオープンして、FSFile構造体のtopメンバをimg_topで上書きする
 *
 * @param[in]	arcID		読み込むアーカイブファイルの種類インデックスナンバー
 * @param[in]	datID		読み込むデータのアーカイブファイル上のインデックスナンバー
 * @param[out]	p_file		データを書き込むFSFile構造体のポインタ
 */
//============================================================================================
extern	void	GFL_ARC_OpenFileTopPosWrite(int arcID,int datID,FSFile *p_file);

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif	__ARC_TOOL_H_

