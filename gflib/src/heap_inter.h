//==============================================================================
/**
 *
 *@file		heap_inter.h
 *@brief	ヒープ領域管理
 *
 */
//==============================================================================
#ifndef __HEAP_INTER_H__
#define __HEAP_INTER_H__

//----------------------------------------------------------------
/**
 *	定数
 */
//----------------------------------------------------------------
#define DEFAULT_ALIGN				(4)			// メモリ確保時のアライメント値
#define MEMHEADER_USERINFO_SIZE		(28-4)		// メモリヘッダ上ユーザー情報領域サイズ
#define HEAPID_MASK					(0x7fff)	// ヒープＩＤ取得マスク
#define HEAPDIR_MASK				(0x8000)	// ヒープ確保方向取得マスク

#if 0
//基本システム依存構造だが、外部公開が必要なので、include/heapsys.h におく
//==============================================================
/**
 *	基本ヒープ作成パラメータ構造体
 */
//==============================================================
typedef struct {
	u32        size;		///< ヒープサイズ
	OSArenaId  arenaID;		///< 作成先アリーナID
}HEAP_INIT_HEADER;

//----------------------------------------------------------------
/**
 *	インライン関数
 */
//----------------------------------------------------------------
static inline  u32 HeapGetLow( u32 heapID )	
{
	return (( heapID & HEAPID_MASK )|( HEAPDIR_MASK ));
}
#endif

//------------------------------------------------------------------------------
/**
 * システム初期化
 *
 * @param   header			初期化情報
 * @param   parentHeapMax	基本ヒープエリア数
 * @param   totalHeapMax	全ヒープエリア数
 * @param   startOffset		与えられたバイト数だけ全体の開始位置をずらす
 *
 * @retval  BOOL			TRUEで成功／FALSEで失敗
 *							errorCode：確保できなかった親ヒープＩＤ
 */
//------------------------------------------------------------------------------
extern BOOL
	GFI_HEAP_sysInit
		(const HEAP_INIT_HEADER* header, u32 parentHeapMax, u32 totalHeapMax, u32 startOffset);

//------------------------------------------------------------------------------
/**
 * システム終了
 *
 * @retval  BOOL			TRUEで成功／FALSEで失敗
 */
//------------------------------------------------------------------------------
extern BOOL
	GFI_HEAP_sysExit
		( void );

//------------------------------------------------------------------
/**
 * ヒープ作成
 *
 * @param   parentHeapID	メモリ領域確保用ヒープＩＤ（既に有効である必要がある）
 * @param   childHeapID		新規に作成するヒープＩＤ（最上位ビットは取得方向フラグ）
 * @param   size			ヒープサイズ
 *
 * @retval  BOOL			TRUEで作成成功／FALSEで失敗
 *							errorCode：失敗原因
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_CREATE_DOUBLE = 1,	//二重登録
	HEAP_CANNOT_CREATE_NOPARENT,	//親ヒープ無効
	HEAP_CANNOT_CREATE_HEAP,		//メモリ不足
	HEAP_CANNOT_CREATE_HEAPTABLE,	//ヒープ作成量オーバー
	HEAP_CANNOT_CREATE_HEAPHANDLE,	//ヒープハンドル作成失敗
};

extern BOOL 
	GFI_HEAP_CreateHeap
		( u32 parentHeapID, u32 childHeapID, u32 size );

//------------------------------------------------------------------
/**
 * ヒープ破棄
 *
 * @param   childHeapID		破棄するヒープＩＤ（最上位ビットは取得方向フラグ）
 *
 * @retval  BOOL			TRUEで破棄成功／FALSEで失敗
 *							errorCode：失敗原因
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_DELETE_DOUBLE = 1,	//多重破棄
	HEAP_CANNOT_DELETE_NOPARENT,	//親ヒープ無効
};

extern BOOL
	GFI_HEAP_DeleteHeap
		( u32 childHeapID );

//------------------------------------------------------------------
/**
 * ヒープからメモリを確保する
 *
 * @param   heapID		ヒープＩＤ（最上位ビットは取得方向フラグ）
 * @param   size		確保サイズ
 *
 * @retval  void*		確保した領域アドレス（失敗ならNULL）
 *						errorCode：失敗原因
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_ALLOC_NOID = 1,	//指定ＩＤが無効
	HEAP_CANNOT_ALLOC_NOHEAP,	//指定ＩＤヒープが登録されていない
	HEAP_CANNOT_ALLOC_MEM,		//メモリ不足
};

extern void*
	GFI_HEAP_AllocMemory
		( u32 heapID, u32 size );

//------------------------------------------------------------------
/**
 * ヒープからメモリを解放する
 *
 * @param   memory		確保したメモリポインタ
 *
 * @retval  BOOL		TRUEで解放成功／FALSEで失敗
 *						errorCode：失敗原因
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_FREE_NOBLOCK = 1,	//指定ポインタに間違いがある
	HEAP_CANNOT_FREE_NOID,			//保存ＩＤヒープが異常
	HEAP_CANNOT_FREE_NOHEAP,		//保存ＩＤヒープが取得したハンドルが未登録状態
	HEAP_CANNOT_FREE_NOMEM,			//メモリ確保カウンタに異常（存在していない）
};

extern BOOL
	GFI_HEAP_FreeMemory
		( void* memory );

//------------------------------------------------------------------
/**
 * NitroSystem ライブラリ系関数が要求するアロケータを作成する
 *
 * @param   pAllocator		NNSFndAllocator構造体のアドレス
 * @param   heapID			ヒープＩＤ（最上位ビットは取得方向フラグ）
 * @param   align			確保するメモリブロックに適用するアライメント（負の値は正の値に変換）
 *
 * @retval  BOOL			TRUEで成功／FALSEで失敗
 *							errorCode：失敗原因
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_INITALLOCATER_NOID = 1,	//指定ＩＤが無効
};

extern BOOL
	GFI_HEAP_InitAllocator
		( NNSFndAllocator* pAllocator, u32 heapID, s32 alignment );

//------------------------------------------------------------------
/**
 * 確保したメモリブロックのサイズを変更する。
 *
 * @param   memory		メモリブロックポインタ
 * @param   newSize		変更後のサイズ（バイト単位）
 *
 * @retval  BOOL		TRUEで成功／FALSEで失敗
 *						errorCode：失敗原因
 *
 * 状態や変更後のサイズによっては失敗、もしくは指定サイズより大きくなることもある
 *
 * 例えば【ヘッダ＋実体】のような形式のグラフィックバイナリをＲＡＭに読み込み、
 * 実体部をVRAMに転送した後、ヘッダのみを残したいというケースなどで使用することを
 * 想定しているが。使用は慎重に。
 *
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_MEMRESIZE_NOBLOCK = 1,	//指定ポインタに間違いがある
	HEAP_CANNOT_MEMRESIZE_NOID,			//保存ＩＤヒープが異常
	HEAP_CANNOT_MEMRESIZE_NOHEAP,		//保存ＩＤヒープが取得したハンドルが未登録状態
	HEAP_CANNOT_MEMRESIZE_LARGE,		//メモリ不足で拡大出来ない
	HEAP_CANNOT_MEMRESIZE_SMALL,		//差分が少なすぎて縮小出来ない
};

extern BOOL
	GFI_HEAP_MemoryResize
		( void* memory, u32 newSize );

//------------------------------------------------------------------
/**
 * ヒープの空き領域サイズを返す
 *
 * @param   heapID	ヒープＩＤ（最上位ビットは取得方向フラグ）
 *
 * @retval  u32		空き領域サイズ（バイト単位）エラー時は0
 *					errorCode：失敗原因
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETSIZE_NOID = 1,	//指定ＩＤが無効
};

extern u32
	GFI_HEAP_GetHeapFreeSize
		( u32 heapID );

//------------------------------------------------------------------
/**
 * ハンドル情報の取得
 *
 * @param   heapID				ヒープＩＤ（最上位ビットは取得方向フラグ）
 *
 * @retval  NNSFndHeapHandle*	該当するハンドル（失敗ならNULL）
 *								errorCode：失敗原因
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETHANDLE_NOID = 1,	//指定ＩＤが無効
};

extern NNSFndHeapHandle
	GFI_HEAP_GetHandle
		( u32 heapID );

//------------------------------------------------------------------
/**
 * 親ハンドル情報の取得
 *
 * @param   heapID				ヒープＩＤ（最上位ビットは取得方向フラグ）
 *
 * @retval  NNSFndHeapHandle*	該当するヒープを所持している親ハンドル（失敗ならNULL）
 *								errorCode：失敗原因
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETPARENTHANDLE_NOID = 1,	//指定ＩＤが無効
};

extern NNSFndHeapHandle
	GFI_HEAP_GetParentHandle
		( u32 heapID );

//------------------------------------------------------------------
/**
 * メモリブロック情報の取得
 *
 * @param   heapID	ヒープＩＤ（最上位ビットは取得方向フラグ）
 *
 * @retval  void*	該当するヒープのポインタ（失敗ならNULL）
 *					errorCode：失敗原因
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETHEAPMEMBLOCK_NOID = 1,	//指定ＩＤが無効
};

extern void*
	GFI_HEAP_GetHeapMemBlock
		( u32 heapID );

//------------------------------------------------------------------
/**
 * アロケート情報の取得
 *
 * @param   heapID	ヒープＩＤ（最上位ビットは取得方向フラグ）
 *
 * @retval  u16		該当するヒープでアロケートされている数（失敗なら0xff）
 *					errorCode：失敗原因
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETHEAPCOUNT_NOID = 1,	//指定ＩＤが無効
};

extern u16
	GFI_HEAP_GetHeapCount
		( u32 heapID );

//------------------------------------------------------------------
/**
 * メモリヘッダ上ヒープＩＤの取得
 *
 * @param   memory	確保したメモリアドレス
 *
 * @retval  u32		ヒープＩＤ、取得失敗時は0xff
 *					errorCode：失敗原因
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETMEMHEADERHEAPID_NOBLOCK = 1,	//保存ＩＤヒープが異常
};

extern u32
	GFI_HEAP_GetMemheaderHeapID
		( void* memory );

//------------------------------------------------------------------
/**
 * メモリヘッダ上ユーザー情報ポインタの取得
 *
 * @param   memory	確保したメモリアドレス
 *
 * @retval  u32		ユーザー情報ポインタ、取得失敗時はNULL
 *					errorCode：失敗原因
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_GETMEMHEADERUSERINFO_NOBLOCK = 1,	//保存ＩＤヒープが異常
};

extern void*
	GFI_HEAP_GetMemheaderUserinfo
		( void* memory );

//------------------------------------------------------------------
/**
 * ヒープ領域が破壊されていないかチェック
 *
 * @param   heapID	ヒープID（最上位ビットは取得方向フラグ）
 *
 * @retval  BOOL	破壊されていなければTRUE
 */
//------------------------------------------------------------------
enum{
	HEAP_CANNOT_CHECKHEAPSAFE_NOID = 1,	//指定ＩＤが無効
	HEAP_CANNOT_CHECKHEAPSAFE_NOHEAP,	//取得したハンドルが未登録状態
	HEAP_CANNOT_CHECKHEAPSAFE_DESTROY,	//ヒープ領域が破壊されている
};

extern BOOL
	GFI_HEAP_CheckHeapSafe
		( u32 heapID );

//------------------------------------------------------------------
/**
 * エラーコードの取得
 *
 * @retval  int		エラーコード
 */
//------------------------------------------------------------------
extern u32
	GFI_HEAP_ErrorCodeGet
		( void );


#endif	// __HEAP_INTER_H__

