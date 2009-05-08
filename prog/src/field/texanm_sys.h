//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		texanm_sys.h
 *	@brief		ITPアニメデータの必要な所のみを抜き出しテクスチャをアニメーションデータを取得するシステム
 *	@author		tomoya takahashi	
 *	@data		2005.10.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#pragma once

#include <nnsys.h>

#ifdef PM_DEBUG
//#define DEBUG_TEXANM_SYS_PRINT_LOAD_DATA	// 読み込んできた情報を出力します。
#endif

//-----------------------------------------------------------------------------
/**
 *	ビルボードアクターのアニメーションテーブル動作システムです。
 *	またitp_convでコンバートされたデータをアンパックして
 *	使用できるようになっています。
 *
 *	TEXANM_DATATBLの中に以下ようなデータを設定することにより
 *	アニメーションデータテーブルを作成することが出来ます。
 *		■フレームデータテーブル
 *　		const static u16 FrameTbl[ ANMTBL_NUM ] = {
 *				0, 4, 8, 12, 16, 20, 24, 28, 32, 36,
 *				40, 44, 48, 52, 56, 60
 *			};
 *		■テクスチャインデックスデータテーブル
 *　		const static u8 TexidxTbl[ ANMTBL_NUM ] = {
 *				0, 8, 9, 10, 11, 12, 13, 14, 15, 1,
 *				2, 3, 4, 5, 6, 7
 *			};
 *		■パレットインデックスデータテーブル
 *　		const static u8 PlttIdxTbl[ ANMTBL_NUM ] = {
 *				0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
 *				0, 0, 0, 0, 0, 0
 *			};
 *
 *		■テクスチャアニメーションデータテーブル
 *			const static TEXANM_DATATBL TexAnmTbl = {
 *				FrameTbl,		// フレームデータテーブル
 *				TexidxTbl,		// テクスチャインデックステーブル
 *				PlttidxTbl,		// パレットインデックステーブル
 *				ANMTBL_NUM		// テーブル要素数		３テーブルの要素数は一緒である必要があります。
 *			};
 *
 *		＊各データテーブルのデータは要素数でリンクします。
 *		＊フレームデータテーブルの値は、その要素数のデータを使用する期間の開始フレーム数です。
 *		　終了フレームは(次の要素数-1)です。
 *		＊上の例ですと、フレームが0〜3の時は	テクスチャインデックス0 パレットインデックス0
 *						フレームが8〜11の時は	テクスチャインデックス9 パレットインデックス0
 *		　といった感じです。
 *
 *		今は手動でアニメーションデータテーブルを作成しましたが、
 *		itp_convでコンバートしたデータを使用するとitpファイルから作成することが出来ます。
 *			・前処理itp_convでitpファイルをコンバート->コンバート後のファイルが出来ます。
 *			
 *			１：コンバート後のファイルをRAMに読み込んできます。
 *			２：TEXANM_UnPackLoadFile関数でアンパック(TEXANM_DATATBL形式のファイルにする)
 *		これでitpファイル内のアニメーションデータを使用する事が出来ます。
 *			
 *			
 */
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------
//-------------------------------------
//	
//	テクスチャアニメーションデータテーブル
//
//	アニメーションデータの決まり
//		各テーブルの要素数はtbl_numの数で統一
//		してある必要があります。
//	
//=====================================
typedef struct {
	const u16*	cp_frame;		// フレーム　テーブル
	const u8*	cp_tex_idx;		// テクスチャインデックス　テーブル
	const u8*	cp_pltt_idx;	// パレットインデックス　テーブル
	const u8* cp_data_head;	// 各データの先頭データインデックス
	u32			tbl_num;		// テーブル数
	u32			anime_num;		// テーブル数
	u32			frame_max;		// アニメーションの最終フレーム数
} TEXANM_DATATBL;

//-------------------------------------
//	
//	ある時点のテクスチャインデックスとパレットインデックス
//	
//=====================================
typedef struct {
	u8	tex_idx;		// テクスチャインデックス	
	u8	pltt_idx;		// パレットインデックス
} TEXANM_DATA;



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//	frame数に対応するテクスチャインデックス　パレットインデックスの取得
//----------------------------------------------------------------------------
extern TEXANM_DATA TEXANM_GetFrameData( const TEXANM_DATATBL* cp_texanm, u32 anime_index, u32 frame );

//----------------------------------------------------------------------------
//	アニメーションの数を取得
//----------------------------------------------------------------------------
extern u32 TEXANM_GetAnimeNum( const TEXANM_DATATBL* cp_texanm );

//----------------------------------------------------------------------------
//	アニメーションのフレーム最大数を取得
//----------------------------------------------------------------------------
extern u32 TEXANM_GetFrameMax( const TEXANM_DATATBL* cp_texanm );

//----------------------------------------------------------------------------
//	アニメーションの最終KEY設定フレームを取得
//----------------------------------------------------------------------------
extern u32 TEXANM_GetLastKeyFrame( const TEXANM_DATATBL* cp_texanm, u32 anime_index );

//----------------------------------------------------------------------------
//	ROMから読み込んできたデータをテクスチャアニメーションデータテーブル形式にアンパックする
//----------------------------------------------------------------------------
extern void TEXANM_UnPackLoadFile( const void* cp_buff, TEXANM_DATATBL* op_texanm );

#ifdef DEBUG_TEXANM_SYS_PRINT_LOAD_DATA
extern void TEXANM_DEBUG_PrintData( const TEXANM_DATATBL* cp_texanm );
#endif
