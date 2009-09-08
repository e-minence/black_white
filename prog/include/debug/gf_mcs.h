
//============================================================================================
/**
 * @file	gf_mcs.h
 * @brief	MCS関連関数
 * @author	soga
 * @date	2009.03.06
 */
//============================================================================================

#ifndef __GF_MCS_H_
#define __GF_MCS_H_

//============================================================================================
/**
 *	定数宣言
 */
//============================================================================================
// Windowsアプリケーションとの識別で使用するチャンネル値です
enum{
	MCS_CHANNEL0 = 0,
	MCS_CHANNEL1,
	MCS_CHANNEL2,
	MCS_CHANNEL3,
	MCS_CHANNEL4,
	MCS_CHANNEL5,
	MCS_CHANNEL6,
	MCS_CHANNEL7,
	MCS_CHANNEL8,
	MCS_CHANNEL9,
	MCS_CHANNEL10,
	MCS_CHANNEL11,
	MCS_CHANNEL12,
	MCS_CHANNEL13,
	MCS_CHANNEL14,
	MCS_CHANNEL15,

	MCS_CHANNEL_END,
};

extern	BOOL	MCS_Init( HEAPID heapID );
extern	void	MCS_Close( void );
extern	void	MCS_Exit( void );
extern	void	MCS_Main( void );
extern	u32		MCS_Read( void *buf, int size );
extern	BOOL	MCS_Write( int ch, const void *buf, int size );
extern	u32		MCS_CheckRead( void );
extern	BOOL	MCS_CheckEnable( void );

typedef	u16 GFL_MCS_LINKIDX;
#define GFL_MCS_LINKIDX_INVALID (0xffff)

//============================================================================================
/**
 *	システム関数
 *	 system/gfl_use.c内から呼ばれているシステム関数
 */
//============================================================================================
extern void	GFL_MCS_Init( void );
extern void GFL_MCS_Main( void );
extern void	GFL_MCS_VIntrFunc( void );
extern void	GFL_MCS_Exit( void );

//============================================================================================
/**
 *	デバイスオープン
 *		MCSデバイスをオープンし通信開始準備をする
 */
//============================================================================================
extern BOOL	GFL_MCS_Open( void );
//============================================================================================
/**
 *	デバイスクローズ
 *		MCSデバイスをクローズし終了処理をする
 */
//============================================================================================
extern void	GFL_MCS_Close( void );
//============================================================================================
/**
 *	データ読み込みチェック（受信データのサイズを取得する）
 *  [IN]  categoryID	: 任意の識別用カテゴリＩＤ（PCアプリと同一のもの）
 *
 *		主に受信データ量に合わせてメモリアロケートする場合を想定している
 *		※呼び出す際はGFL_MCS_Readの前に実行すること
 *		※この関数を呼ばなくてもGFL_MCS_Readは実行可能（受信用バッファサイズが固定の場合など）
 */
//============================================================================================
extern int	GFL_MCS_CheckReadable( u32 categoryID );
//============================================================================================
/**
 *	データ読み込み
 *  [IN]  categoryID	: 任意の識別用カテゴリＩＤ（PCアプリと同一のもの）
 *				pWriteBuf		: 読み込みデータの格納ポインタ
 *				readBufSize	: 上記読み込みバッファサイズ
 *
 *		※事前にGFL_MCS_CheckReadableが呼ばれていない場合はこの関数内で呼び出している
 */
//============================================================================================
extern BOOL	GFL_MCS_Read( u32 categoryID, void* pReadBuf, u32 readBufSize );
//============================================================================================
/**
 *	データ書き込み
 *  [IN]  categoryID	: 任意の識別用カテゴリＩＤ（PCアプリと同一のもの）
 *				pWriteBuf		: 書き込みデータの格納ポインタ
 *				writeSize		: 書き込みデータサイズ
 */
//============================================================================================
extern BOOL	GFL_MCS_Write( u32 categoryID, const void* pWriteBuf, u32 writeSize );


#endif __GF_MCS_H_
