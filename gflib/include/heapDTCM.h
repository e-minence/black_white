//==============================================================================
/**
 *
 *@file		heapDTCM.h
 *@brief	ヒープ領域管理(スタックエリア)
 *
 * Description:  DTCMエリアの使用設定をします。
 * 				領域幅が少ないので（16k=0x4000）、
 * 				メインアリーナを使用するheap.cとは異なり、
 * 				単一ヒープのみの簡易的な作りにしてあります。
 * 				初期化時に使用サイズの設定を行いますが
 * 				基本的にスタックおよび関数内メモリに割り当てられるエリアなので
 * 				大きなサイズは控えるようにしてください。
 * 				最大サイズは0x3000-0x80(ヘッダ)としてあります。
 */
//==============================================================================
#ifndef __HEAP_DTCM_H__
#define __HEAP_DTCM_H__

//----------------------------------------------------------------
/**
 *	定数
 */
//----------------------------------------------------------------
#define DEFAULT_ALIGN				(4)					// メモリ確保時のアライメント値

//------------------------------------------------------------------------------
/**
 * システム初期化
 *
 * @param	size		使用サイズ			
 *
 * @retval  BOOL		TRUEで成功／FALSEで失敗
 */
//------------------------------------------------------------------------------
extern BOOL
	GFI_HEAP_DTCM_sysInit
		( u32 size );

//------------------------------------------------------------------------------
/**
 * システム終了
 *
 * @retval  BOOL		TRUEで成功／FALSEで失敗
 */
//------------------------------------------------------------------------------
extern BOOL
	GFI_HEAP_DTCM_sysExit
		( void );

//------------------------------------------------------------------
/**
 * ヒープからメモリを確保する
 *
 * @param   size		確保サイズ
 *
 * @retval  void*		確保した領域アドレス（失敗ならNULL）
 */
//------------------------------------------------------------------
extern void*
	GFI_HEAP_DTCM_AllocMemory
		( u32 size );

//------------------------------------------------------------------
/**
 * ヒープからメモリを解放する
 *
 * @param   memory		確保したメモリアドレス
 *
 * @retval  BOOL		TRUEで解放成功／FALSEで失敗
 */
//------------------------------------------------------------------
extern BOOL
	GFI_HEAP_DTCM_FreeMemory
		( void* memory );


#endif	// __HEAP_DTCM_H__

