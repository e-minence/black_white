////////////////////////////////////////////////////////////////////////////
/**
 * @file   place_name_letter.c
 * @brief  地名表示の文字オブジェクト
 * @author obata_toshihiro
 * @date   2010.03.26
 */
////////////////////////////////////////////////////////////////////////////
#include <gflib.h>
#include "place_name_letter.h"

#include "print/printsys.h"  // for PRINTSYSS_xxxx


//=========================================================================
// ■ 定数・マクロ
//==========================================================================
#define CHAR_SIZE (8) // 1キャラ = 8ドット

#define BMP_XSIZE_CHR (2) // ビットマップの x サイズ ( キャラ単位 )
#define BMP_YSIZE_CHR (2) // ビットマップの y サイズ ( キャラ単位 )

#define	COLOR_NO_LETTER (1)  // 文字本体のカラー番号
#define	COLOR_NO_SHADOW (2)  // 影部分のカラー番号
#define	COLOR_NO_BACK   (0)  // 背景部のカラー番号



//=========================================================================
// ■ 文字オブジェクトの移動パラメータ
//=========================================================================
typedef struct {

	float x;		// 座標
	float y;		// 
	float sx;		// 速度
	float sy;		// 
	float ax;		// 加速度
	float ay;		// 

	float dx;		// 目標位置
	float dy;		// 
	float dsx;	// 目標位置での速度
	float dsy;	// 

	int tx;			// 残り移動回数( x方向 )
	int ty;			// 残り移動回数( y方向 ) 

} PN_LETTER_PARAM;


//=========================================================================
// ■文字オブジェクト
//=========================================================================
struct _PN_LETTER {

  HEAPID          heapID;       // ヒープID
  GFL_FONT*       font;         // フォント
	GFL_BMP_DATA*   bmp;		      // ビットマップ
	BMPOAM_ACT_PTR  bmpOamActor;	// BMPOAMアクター
	PN_LETTER_PARAM moveParam;    // 移動パラメータ
	u8              width;		    // 文字幅
	u8              height;		    // 文字高さ
	BOOL            moveFlag;		  // 移動中かどうか
  STRCODE         strcode;      // 文字コード

};


//=========================================================================
// ■index
//=========================================================================

// 初期化・生成・破棄
static PN_LETTER* CreateLetter( HEAPID heapID ); // 文字オブジェクトを生成する
static void DeleteLetter( PN_LETTER* letter ); // 文字オブジェクトを破棄する
static void InitLetter( PN_LETTER* letter ); // 文字オブジェクトを初期化する
static void CreateBitmap( PN_LETTER* letter ); // ビットマップを生成する
static void DeleteBitmap( PN_LETTER* letter ); // ビットマップを破棄する
static void CreateBmpOamActor( PN_LETTER* letter, BMPOAM_SYS_PTR bmpOamSys, u32 plttRegIdx ); // BMPOAMアクターを生成する
static void DeleteBmpOamActor( PN_LETTER* letter ); // BMPOAMアクターを破棄する
// アクセッサ
static HEAPID GetHeapID( const PN_LETTER* letter ); // ヒープIDを取得する
static void SetHeapID( PN_LETTER* letter, HEAPID heapID ); // ヒープIDを設定する
static GFL_FONT* GetLetterFont( const PN_LETTER* letter ); // フォントを取得する
static void SetLetterFont( PN_LETTER* letter, GFL_FONT* font ); // フォントを設定する
static STRCODE GetLetterCode( const PN_LETTER* letter ); // 文字コードを取得する
static void SetLetterCode( PN_LETTER* letter, STRCODE code ); // 文字コードを設定する
static int GetLetterWidth( const PN_LETTER* letter ); // 文字の幅を取得する
static void SetLetterWidth( PN_LETTER* letter, u8 width ); // 文字の幅を設定する
static int GetLetterHeight( const PN_LETTER* letter ); //文字の高さを取得する
static void SetLetterHeight( PN_LETTER* letter, u8 height ); //文字の高さを設定する
static const PN_LETTER_PARAM* GetLetterMoveParam( const PN_LETTER* letter ); // 移動パラメータを取得する
static void SetLetterMoveParam( PN_LETTER* letter, const PN_LETTER_PARAM* param ); // 移動パラメータを設定する
// 判定
static BOOL CheckLetterMoving( const PN_LETTER* letter ); // 移動中かどうかをチェックする
static BOOL CheckMoveEnd( const PN_LETTER* letter ); // 移動が終了したかどうかをチェックする
static BOOL CheckXMoveEnd( const PN_LETTER* letter ); // x 方向の移動が終了したかどうかをチェックする
static BOOL CheckYMoveEnd( const PN_LETTER* letter ); // y 方向の移動が終了したかどうかをチェックする
// 動作
static void LetterMain( PN_LETTER* letter ); // メイン動作
static void LetterMoveX( PN_LETTER* letter ); // x 方向に動かす
static void LetterMoveY( PN_LETTER* letter ); // y 方向に動かす
// 制御
static void StartMove( PN_LETTER* letter ); // 移動を開始する
static void EndMove( PN_LETTER* letter ); // 移動を終了する
static void SetupMoveParam( PN_LETTER* letterl ); // 移動パラメータをセットアップする
// ビットマップ操作
static void ClearBitmap( PN_LETTER* letter ); // ビットマップをクリアする
static void PrintLetterToBitmap( PN_LETTER* letter ); // 文字をビットマップに書き込む
static void UpdateBmpOamActorPos( PN_LETTER* letter ); // BMPOAMアクターの座標を更新する
// ユーティリティ
static void GetPrintSize( GFL_FONT* font, STRCODE code, HEAPID heapID, int* destX, int* destY ); // 文字サイズを取得する




//=========================================================================
// ■public method
//=========================================================================


//---------------------------------------------------------------------------
/**
 * @brief 文字オブジェクトを生成する
 *
 * @param heapID     使用するヒープID
 * @param bmpOamSys  アクターを追加するシステム
 * @param plttRegIdx アクターに適用するパレット登録インデックス
 *
 * @return 生成した文字オブジェクト
 */
//---------------------------------------------------------------------------
PN_LETTER* PN_LETTER_Create( 
    HEAPID heapID, BMPOAM_SYS_PTR bmpOamSys, u32 plttRegIdx )
{
  PN_LETTER* letter;

  letter = CreateLetter( heapID ); // 本体を生成
  InitLetter( letter ); // 初期化
  SetHeapID( letter, heapID ); // ヒープIDをセット
  CreateBitmap( letter ); // ビットマップを生成
  CreateBmpOamActor( letter, bmpOamSys, plttRegIdx ); // BMPOAMアクターを生成

  return letter;
}

//---------------------------------------------------------------------------
/**
 * @brief 文字オブジェクトを破棄する
 *
 * @param letter
 */
//---------------------------------------------------------------------------
void PN_LETTER_Delete( PN_LETTER* letter )
{
  GF_ASSERT( letter );

  DeleteBmpOamActor( letter ); // BMPOAMアクターを破棄
  DeleteBitmap( letter ); // ビットマップを破棄
  DeleteLetter( letter ); // 本体を破棄
}

//---------------------------------------------------------------------------
/**
 * @brief 文字オブジェクトをセットアップする
 *
 * @param letter
 * @param param セットアップ・パラメータ
 */
//---------------------------------------------------------------------------
void PN_LETTER_Setup( PN_LETTER* letter, const PN_LETTER_SETUP_PARAM* param )
{
  GF_ASSERT( letter );

  SetLetterFont( letter, param->font );
  SetLetterCode( letter, param->code );
}

//---------------------------------------------------------------------------
/**
 * @brief 文字オブジェクトのメイン動作
 *
 * @param letter
 */
//---------------------------------------------------------------------------
void PN_LETTER_Main( PN_LETTER* letter )
{
  GF_ASSERT( letter );

  LetterMain( letter );
}

//---------------------------------------------------------------------------
/**
 * @brief 文字オブジェクトの移動を開始する
 *
 * @param letter
 */
//---------------------------------------------------------------------------
void PN_LETTER_MoveStart( PN_LETTER* letter )
{
  GF_ASSERT( letter );
}

//---------------------------------------------------------------------------
/**
 * @brief 文字オブジェクトのビットマップを取得する
 *
 * @param letter
 */
//---------------------------------------------------------------------------
const GFL_BMP_DATA* PN_LETTER_GetBitmap( const PN_LETTER* letter )
{
  GF_ASSERT( letter );
  GF_ASSERT( letter->bmp );

  return letter->bmp;
}



//=========================================================================
// ■private method
//=========================================================================


//---------------------------------------------------------------------------
/**
 * @brief 文字オブジェクトを生成する
 *
 * @param letter
 *
 * @return 生成した文字オブジェクト
 */
//---------------------------------------------------------------------------
static PN_LETTER* CreateLetter( HEAPID heapID )
{
  PN_LETTER* letter;

  letter = GFL_HEAP_AllocMemory( heapID, sizeof(PN_LETTER) );

  return letter;
}

//---------------------------------------------------------------------------
/**
 * @brief 文字オブジェクトを破棄する
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void DeleteLetter( PN_LETTER* letter )
{
  GFL_HEAP_FreeMemory( letter );
}

//---------------------------------------------------------------------------
/**
 * @brief 文字オブジェクトを初期化する
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void InitLetter( PN_LETTER* letter )
{
  // ゼロクリア
  GFL_STD_MemClear( letter, sizeof(PN_LETTER) );

  // 初期化
  letter->moveFlag = FALSE;
  letter->strcode  = GFL_STR_GetEOMCode();
}

//---------------------------------------------------------------------------
/**
 * @brief ビットマップを生成する
 *
 * @param letter
 * @param heapID
 */
//---------------------------------------------------------------------------
static void CreateBitmap( PN_LETTER* letter )
{
  GF_ASSERT( letter->bmp == NULL );

	letter->bmp = GFL_BMP_Create( 
      BMP_XSIZE_CHR, BMP_YSIZE_CHR, GFL_BMP_16_COLOR, letter->heapID );
}

//---------------------------------------------------------------------------
/**
 * @brief ビットマップを破棄する
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void DeleteBitmap( PN_LETTER* letter )
{
  if( letter->bmp ) {
    GFL_BMP_Delete( letter->bmp );
    letter->bmp = NULL;
  } 
}

//---------------------------------------------------------------------------
/**
 * @brief BMPOAMアクターを生成する
 *
 * @param letter
 * @param bmpOamSys  アクターを追加するシステム
 * @param plttRegIdx 適用するパレットの登録インデックス
 */
//---------------------------------------------------------------------------
static void CreateBmpOamActor( 
    PN_LETTER* letter, BMPOAM_SYS_PTR bmpOamSys, u32 plttRegIdx )
{
	BMPOAM_ACT_DATA actorData;

  GF_ASSERT( letter->bmp );
  GF_ASSERT( letter->bmpOamActor == NULL );

	actorData.bmp        = letter->bmp;
	actorData.x          = 0;
	actorData.y          = 0;
	actorData.pltt_index = plttRegIdx;
	actorData.pal_offset = 0;
	actorData.soft_pri   = 0;
	actorData.bg_pri     = 0;
	actorData.setSerface = CLSYS_DEFREND_MAIN;
	actorData.draw_type  = CLSYS_DRAW_MAIN; 

  // アクターを追加
	letter->bmpOamActor = BmpOam_ActorAdd( bmpOamSys, &actorData ); 

  // 非表示にする
	BmpOam_ActorSetDrawEnable( letter->bmpOamActor, FALSE );  
}

//---------------------------------------------------------------------------
/**
 * @brief BMPOAMアクターを破棄する
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void DeleteBmpOamActor( PN_LETTER* letter )
{
  if( letter->bmpOamActor ) {
    BmpOam_ActorDel( letter->bmpOamActor );
    letter->bmpOamActor = NULL;
  } 
}

//---------------------------------------------------------------------------
/**
 * @brief ヒープIDを取得する
 *
 * @param letter
 *
 * @return ヒープID
 */
//---------------------------------------------------------------------------
static HEAPID GetHeapID( const PN_LETTER* letter )
{
  return letter->heapID;
}

//---------------------------------------------------------------------------
/**
 * @brief ヒープIDを設定する
 *
 * @param letter
 * @param heapID
 */
//---------------------------------------------------------------------------
static void SetHeapID( PN_LETTER* letter, HEAPID heapID )
{
  letter->heapID = heapID;
}

//---------------------------------------------------------------------------
/**
 * @brief フォントを取得する
 *
 * @param letter
 *
 * @return フォント
 */
//---------------------------------------------------------------------------
static GFL_FONT* GetLetterFont( const PN_LETTER* letter )
{
  GF_ASSERT( letter->font );

  return letter->font;
}

//---------------------------------------------------------------------------
/**
 * @brief フォントを設定する
 *
 * @param letter
 * @param font   フォント
 */
//---------------------------------------------------------------------------
static void SetLetterFont( PN_LETTER* letter, GFL_FONT* font )
{
  letter->font = font;
}

//---------------------------------------------------------------------------
/**
 * @brief 文字コードを取得する
 *
 * @param letter
 *
 * @return 文字コード
 */
//---------------------------------------------------------------------------
static STRCODE GetLetterCode( const PN_LETTER* letter )
{
  return letter->strcode;
}

//---------------------------------------------------------------------------
/**
 * @brief 文字コードを設定する
 *
 * @param letter 
 * @param code   文字コード
 */
//---------------------------------------------------------------------------
static void SetLetterCode( PN_LETTER* letter, STRCODE code )
{
  letter->strcode = code;
}

//---------------------------------------------------------------------------
/**
 * @brief 移動中かどうかをチェックする
 *
 * @param letter
 *
 * @return 移動中なら TRUE, そうでないなら FALSE.
 */
//---------------------------------------------------------------------------
static BOOL CheckLetterMoving( const PN_LETTER* letter )
{
  return letter->moveFlag;
}

//---------------------------------------------------------------------------
/**
 * @brief 文字の幅を取得する
 *
 * @param letter
 *
 * @return 文字の幅
 */
//---------------------------------------------------------------------------
static int GetLetterWidth( const PN_LETTER* letter )
{
  return letter->width;
}

//---------------------------------------------------------------------------
/**
 * @brief 文字の幅を設定する
 *
 * @param letter
 * @param width  文字の幅
 */
//---------------------------------------------------------------------------
static void SetLetterWidth( PN_LETTER* letter, u8 width )
{
  letter->width = width;
}

//---------------------------------------------------------------------------
/**
 * @brief 文字の高さを取得する
 *
 * @param letter
 *
 * @return 文字の高さ
 */
//---------------------------------------------------------------------------
static int GetLetterHeight( const PN_LETTER* letter )
{
  return letter->height;
}

//---------------------------------------------------------------------------
/**
 * @brief 文字の高さを設定する
 *
 * @param letter
 * @param height  文字の高さ
 */
//---------------------------------------------------------------------------
static void SetLetterHeight( PN_LETTER* letter, u8 height )
{
  letter->height = height;
}

//---------------------------------------------------------------------------
/**
 * @brief 移動パラメータを取得する
 *
 * @param letter
 *
 * @return 移動パラメータ
 */
//---------------------------------------------------------------------------
static const PN_LETTER_PARAM* GetLetterMoveParam( const PN_LETTER* letter )
{
  return &(letter->moveParam);
}

//---------------------------------------------------------------------------
/**
 * @brief 移動パラメータを設定する
 *
 * @param letter
 * @param param 移動パラメータ
 */
//---------------------------------------------------------------------------
static void SetLetterMoveParam( PN_LETTER* letter, const PN_LETTER_PARAM* param )
{
  letter->moveParam = *param;
}

//---------------------------------------------------------------------------
/**
 * @brief 移動が終了したかどうかをチェックする
 *
 * @param letter
 *
 * @return 移動が終了した場合 TRUE
 *         そうでなければ FALSE
 */
//---------------------------------------------------------------------------
static BOOL CheckMoveEnd( const PN_LETTER* letter )
{ 
  // 移動していない
  if( CheckLetterMoving(letter) == FALSE ) { return TRUE; }

  // x 方向, y 方向 ともに移動が終了
	if( CheckXMoveEnd(letter) && CheckYMoveEnd(letter) ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//---------------------------------------------------------------------------
/**
 * @brief x 方向の移動が終了したかどうかをチェックする
 *
 * @param letter
 *
 * @return 移動が終了した場合 TRUE
 *         そうでなければ FALSE
 */
//---------------------------------------------------------------------------
static BOOL CheckXMoveEnd( const PN_LETTER* letter )
{
  const PN_LETTER_PARAM* param;

  param = GetLetterMoveParam( letter );

  // 移動していない
  if( CheckLetterMoving(letter) == FALSE ) { return TRUE; }

  // x 方向の移動が終了
	if( param->tx <= 0 ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//---------------------------------------------------------------------------
/**
 * @brief y 方向の移動が終了したかどうかをチェックする
 *
 * @param letter
 *
 * @return 移動が終了した場合 TRUE
 *         そうでなければ FALSE
 */
//---------------------------------------------------------------------------
static BOOL CheckYMoveEnd( const PN_LETTER* letter )
{
  const PN_LETTER_PARAM* param;

  param = GetLetterMoveParam( letter );

  // 移動していない
  if( CheckLetterMoving(letter) == FALSE ) { return TRUE; }

  // y 方向の移動が終了
	if( param->ty <= 0 ) {
    return TRUE;
  }
  else {
    return FALSE;
  }
}

//---------------------------------------------------------------------------
/**
 * @brief メイン動作
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void LetterMain( PN_LETTER* letter )
{
	// 動いていない
	if( CheckLetterMoving(letter) == FALSE ) { return; }

  // 移動
  LetterMoveX( letter );
  LetterMoveY( letter );

	// BMPOAMアクターの表示位置を更新
  UpdateBmpOamActorPos( letter );

	// 移動が完了
	if( CheckMoveEnd(letter) == TRUE ) {
    EndMove( letter );
	} 
}

//---------------------------------------------------------------------------
/**
 * @brief x 方向に動かす
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void LetterMoveX( PN_LETTER* letter )
{
  PN_LETTER_PARAM* param;

  GF_ASSERT( CheckLetterMoving(letter) == TRUE );

  // 移動完了済み
  if( CheckXMoveEnd(letter) == TRUE ) { return; }

  param = &(letter->moveParam);

  param->x  += param->sx; // 移動
  param->sx += param->ax; // 加速
  param->tx--;            // 残り移動回数をデクリメント
}

//---------------------------------------------------------------------------
/**
 * @brief y 方向に動かす
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void LetterMoveY( PN_LETTER* letter )
{
  PN_LETTER_PARAM* param;

  GF_ASSERT( CheckLetterMoving(letter) == TRUE );

  // 移動完了済み
  if( CheckYMoveEnd(letter) == TRUE ) { return; }

  param = &(letter->moveParam);
  
  param->y  += param->sy; // 移動
  param->sy += param->ay; // 加速
  param->ty--;            // 残り移動回数をデクリメント
} 

//---------------------------------------------------------------------------
/**
 * @brief 移動を開始する
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void StartMove( PN_LETTER* letter )
{
  letter->moveFlag = TRUE;
}

//---------------------------------------------------------------------------
/**
 * @brief 移動を終了する
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void EndMove( PN_LETTER* letter )
{
  letter->moveFlag = FALSE;
}

//--------------------------------------------------------------------------
/**
 * @brief 移動パラメータをセットアップする
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void SetupMoveParam( PN_LETTER* letter )
{
	float xDist;
	float yDist;
  PN_LETTER_PARAM* param;

  param = &(letter->moveParam);

  // 移動距離を計算
	xDist = param->dx - param->x;
	yDist = param->dy - param->y;

	// 加速度を計算
	if( xDist != 0 ) {
    param->ax = 
      ( ( param->dsx * param->dsx ) - ( param->sx * param->sx ) ) / ( 2 * xDist );
  }
	if( yDist != 0 ) {
    param->ay = 
      ( ( param->dsy * param->dsy ) - ( param->sy * param->sy ) ) / ( 2 * yDist );
  }

	// 移動回数を計算
	if( param->ax != 0 ) {
    param->tx = (int)( ( param->dsx - param->sx ) / param->ax );
  }
	if( param->ay != 0 ) {
    param->ty = (int)( ( param->dsy - param->sy ) / param->ay );
  }
}

//---------------------------------------------------------------------------
/**
 * @brief ビットマップをクリアする
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void ClearBitmap( PN_LETTER* letter )
{
	GFL_BMP_Clear( letter->bmp, 0 );
}

//---------------------------------------------------------------------------
/**
 * @brief 文字をビットマップに書き込む
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void PrintLetterToBitmap( PN_LETTER* letter )
{
	STRCODE string[2];
	STRBUF* strbuf;
  PRINTSYS_LSB color;

	string[0] = GetLetterCode( letter );
	string[1] = GFL_STR_GetEOMCode();
	strbuf    = GFL_STR_CreateBuffer( 2, letter->heapID );
	GFL_STR_SetStringCodeOrderLength( strbuf, string, 2 );

  color = PRINTSYS_LSB_Make( COLOR_NO_LETTER, COLOR_NO_SHADOW, COLOR_NO_BACK );

	PRINTSYS_PrintColor( letter->bmp, 0, 0, strbuf, letter->font, color );
	BmpOam_ActorBmpTrans( letter->bmpOamActor );

	GFL_STR_DeleteBuffer( strbuf );
}

//---------------------------------------------------------------------------
/**
 * @brief BMPOAMアクターの座標を更新する
 *
 * @param letter
 */
//---------------------------------------------------------------------------
static void UpdateBmpOamActorPos( PN_LETTER* letter )
{
  const PN_LETTER_PARAM* param;

  param = GetLetterMoveParam( letter );

	BmpOam_ActorSetPos( letter->bmpOamActor, param->x, param->y );
}

//---------------------------------------------------------------------------
/**
 * @brief 文字サイズを取得する
 *
 * @param font   フォント
 * @param code   サイズを取得する文字
 * @param heapID 一時使用するヒープID
 * @param destX  文字幅の格納先
 * @param destY  文字高の格納先
 */
//---------------------------------------------------------------------------
static void GetPrintSize( 
    GFL_FONT* font, STRCODE code, HEAPID heapID, int* destX, int* destY )
{
	STRCODE strcode[2];
	STRBUF* strbuf;

	strcode[0] = code;
	strcode[1] = GFL_STR_GetEOMCode();
	strbuf     = GFL_STR_CreateBuffer( 2, heapID );
	GFL_STR_SetStringCodeOrderLength( strbuf, strcode, 2 );

	// サイズ取得
	*destX = PRINTSYS_GetStrWidth( strbuf, font, 0 );
	*destY = PRINTSYS_GetStrHeight( strbuf, font );

	GFL_STR_DeleteBuffer( strbuf );
}
































