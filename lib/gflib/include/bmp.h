//=============================================================================================
/**
 * @file  bmp.h
 * @brief BG描画システムプログラム
 * @author  Hiroyuki Nakamura
 * @date  2006/10/18
 */
//=============================================================================================
#ifndef _BMP_H_
#define _BMP_H_

#ifdef __cplusplus
extern "C" {
#endif

//--------------------------------------------------------------------
//ビットマップ描画データ構造体
typedef struct  _GFL_BMP_DATA GFL_BMP_DATA;


#define GFL_BMP_16_COLOR  (0x20)
#define GFL_BMP_256_COLOR (0x40)
typedef u8  GFL_BMP_ColorFormat;

// VRAM確保方向定義
#define GFL_BMP_CHRAREA_GET_F (GFL_BG_CHRAREA_GET_F)    //VRAM前方確保
#define GFL_BMP_CHRAREA_GET_B (GFL_BG_CHRAREA_GET_B)    //VRAM後方確保

//ビットマップ書き込み用定義
#define GF_BMPPRT_NOTNUKI ( 0xffff )  // 抜き色指定なし

//--------------------------------------------------------------------------------------------
/**
 * Bitmap生成（通常版：ヒープ上に必要な領域を確保する）
 *
 * @param sizex Xサイズ（キャラ単位）
 * @param sizey Yサイズ（キャラ単位）
 * @param col   カラーモード
 * @param heapID  ヒープＩＤ
 *
 * @return  生成されたBitmapハンドラ
 */
//--------------------------------------------------------------------------------------------
extern  GFL_BMP_DATA * GFL_BMP_Create( int sizex, int sizey, int col, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * Bitmap生成（既に確保されているキャラクタデータ領域をBitmapとして利用する）
 *
 * @param adrs  利用するキャラクタデータ領域アドレス
 * @param sizex Xサイズ（キャラ単位）
 * @param sizey Yサイズ（キャラ単位）
 * @param col   カラーコード
 * @param heapID  ヒープID
 *
 * @return  生成されたBitmapハンドラ
 */
//--------------------------------------------------------------------------------------------
extern  GFL_BMP_DATA * GFL_BMP_CreateWithData( u8 *adrs,int sizex, int sizey, int col, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * Bitmap生成（VRAM領域を直接,Bitmapとして利用する）
 *
 * @param adrs  利用するVRAMアドレス
 * @param sizex Xサイズ（キャラ単位）
 * @param sizey Yサイズ（キャラ単位）
 * @param col   カラーコード
 * @param heapID  ヒープID
 *
 * @return  生成されたBitmapハンドラ
 */
//--------------------------------------------------------------------------------------------
extern  GFL_BMP_DATA * GFL_BMP_CreateInVRAM( u8 *adrs,int sizex, int sizey, int col, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * BMP領域開放
 *
 * @param bmp   BMP領域へのポインタ
 */
//--------------------------------------------------------------------------------------------
extern  void  GFL_BMP_Delete( GFL_BMP_DATA *bmp );

//--------------------------------------------------------------------------------------------
/*
 * GFL_BMP_DATA構造体のサイズを取得
 *
 * @retval  GFL_BMP_DATA構造体のサイズ
 */
//--------------------------------------------------------------------------------------------
extern  int   GFL_BMP_GetGFL_BMP_DATASize( void );

//--------------------------------------------------------------------------------------------
/*
 * ビットマップキャラエリアアドレスを取得
 *
 * @param bmp   BMP領域へのポインタ
 */
//--------------------------------------------------------------------------------------------
extern  u8  *GFL_BMP_GetCharacterAdrs( GFL_BMP_DATA *bmp );

//--------------------------------------------------------------------------------------------
/*
 * ビットマップサイズX（ドット単位）を取得
 *
 * @param bmp   BMP領域へのポインタ
 */
//--------------------------------------------------------------------------------------------
extern  u16 GFL_BMP_GetSizeX( GFL_BMP_DATA *bmp );

//--------------------------------------------------------------------------------------------
/*
 * ビットマップサイズY（ドット単位）を取得
 *
 * @param bmp   BMP領域へのポインタ
 */
//--------------------------------------------------------------------------------------------
extern  u16 GFL_BMP_GetSizeY( GFL_BMP_DATA *bmp );

//--------------------------------------------------------------------------------------------
/*
 * ビットマップ実データサイズを取得
 *
 * @param bmp   システムワークエリアへのポインタ
 */
//--------------------------------------------------------------------------------------------
extern u32  GFL_BMP_GetBmpDataSize( const GFL_BMP_DATA *bmp );

//--------------------------------------------------------------------------------------------
/*
 * ビットマップデータのコピー
 *
 * @param src   コピー元
 * @param dst   コピー先
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BMP_Copy( const GFL_BMP_DATA *src, GFL_BMP_DATA *dst );

//--------------------------------------------------------------------------------------------
/**
 * BMPキャラロード（BMPデータの元データ作成）
 *
 * @param arcID   キャラのアーカイブID
 * @param datID   キャラのアーカイブ内のデータインデックス
 * @param compflag  圧縮、非圧縮フラグ
 * @param heapID    ヒープＩＤ
 *
 * @return  取得したメモリのアドレス
 */
//--------------------------------------------------------------------------------------------
extern  GFL_BMP_DATA * GFL_BMP_LoadCharacter( int arcID, int datID, int compflag, HEAPID heapID );

//--------------------------------------------------------------------------------------------
/**
 * BMPのカラーフォーマットを取得
 *
 * @param   bmp   対象Bmp
 *
 * @retval  GFL_BMP_ColorFormat     カラーフォーマット
 */
//--------------------------------------------------------------------------------------------
extern GFL_BMP_ColorFormat  GFL_BMP_GetColorFormat( const GFL_BMP_DATA* bmp );

//--------------------------------------------------------------------------------------------
/**
 * 読み込みと書き込みのアドレスと範囲を指定してキャラを描画(透明色指定あり）
 *
 * @param src     読み込み元キャラデータヘッダー構造体ポインタ
 * @param dest    書き込み先キャラデータヘッダー構造体ポインタ
 * @param pos_sx    読み込み元読み込み開始X座標
 * @param pos_sy    読み込み元読み込み開始Y座標
 * @param pos_dx    書き込み先書き込み開始X座標
 * @param pos_dy    書き込み先書き込み開始Y座標
 * @param size_x    描画範囲Xサイズ
 * @param size_y    描画範囲Yサイズ
 * @param nuki_col  透明色指定（0〜15 GF_BMPPRT_NOTNUKI:透明色指定なし）
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BMP_Print( const GFL_BMP_DATA * src, GFL_BMP_DATA * dest, u16 pos_sx, u16 pos_sy, s16 pos_dx, s16 pos_dy,
      u16 size_x, u16 size_y, u16 nuki_col );

//--------------------------------------------------------------------------------------------
/**
 * 読み込みと書き込みのアドレスと範囲を指定してキャラを描画 (透明色指定あり / 16色→256色専用）
 *
 * @param  src        読み込み元Bmpデータ（16色）
 * @param  dest       書き込み先Bmpデータ（256色）
 * @param  pos_sx     読み込み開始Ｘ座標（ドット）
 * @param  pos_sy     読み込み開始Ｙ座標（ドット）
 * @param  pos_dx     書き込み開始Ｘ座標（ドット）
 * @param  pos_dy     書き込み開始Ｙ座標（ドット）
 * @param  size_x     描画範囲Ｘサイズ（ドット）
 * @param  size_y     描画範囲Ｙサイズ（ドット）
 * @param  nuki_col   透明色指定（0〜15 GF_BMPPRT_NOTNUKI:透明色指定なし）
 * @param  col_offset 読み込みデータの色番号(0〜15）に付加するオフセット値
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BMP_Print16to256( const GFL_BMP_DATA * src, GFL_BMP_DATA * dest, u16 pos_sx, u16 pos_sy, s16 pos_dx, s16 pos_dy,
      u16 size_x, u16 size_y, u16 nuki_col, u16 col_offset );

//--------------------------------------------------------------------------------------------
/**
 * 範囲を指定して指定された色コードで塗りつぶし
 *
 * @param dest    書き込み先キャラデータヘッダー構造体ポインタ
 * @param pos_dx    書き込み先書き込み開始X座標
 * @param pos_dy    書き込み先書き込み開始Y座標
 * @param size_x    描画範囲Xサイズ
 * @param size_y    描画範囲Yサイズ
 * @param col_code  塗りつぶし色コード
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern void GFL_BMP_Fill( GFL_BMP_DATA * dest, s16 pos_dx, s16 pos_dy, u16 size_x, u16 size_y, u8 col_code );

//--------------------------------------------------------------------------------------------
/**
 * 指定された色コードで塗りつぶし
 *
 * @param dest    書き込み先キャラデータヘッダー構造体ポインタ
 * @param col_code  塗りつぶし色コード
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern  void GFL_BMP_Clear( GFL_BMP_DATA * dest, u8 col_code );

//--------------------------------------------------------------------------------------------
/**
 * データコンバート（Chrデータフォーマット→BMPデータフォーマット）
 *
 * @param src     読み込み元キャラデータヘッダー構造体ポインタ
 * @param mode    新規にファイル用メモリをアロケートするかどうか
 *
 * @return  none
 */
//--------------------------------------------------------------------------------------------
extern GFL_BMP_DATA * GFL_BMP_DataConv_to_BMPformat( GFL_BMP_DATA * src, BOOL mode, HEAPID heapID );


//--------------------------------------------------------------------------------------------
/**
 * 指定位置のピクセル値を取得
 *
 * @param   bmp   Bitmapデータ
 * @param   x     Ｘ位置指定（ドット単位）
 * @param   y     Ｙ位置指定（ドット単位）
 *
 * @retval  u8    ピクセル値
 */
//--------------------------------------------------------------------------------------------
extern u8 GFL_BMP_GetPixel( const GFL_BMP_DATA* bmp, u32 x, u32 y );

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif
