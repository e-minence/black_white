//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		namein_data.c
 *	@brief	名前入力用データ
 *	@author	Toru=Nagihashi
 *	@data		2009.10.08
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>

//システム
#include "print/str_tool.h"

//アーカイブ
#include "arc_def.h"
#include "namein_data.naix"

//外部
#include "namein_data.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	変換用データ
//=====================================
//存在しない場合入っている値（namein_change_conv.pl内でスペースをいれている）
#define NAMEIN_STRCHANGE_EMPTY	(L'　')
#define NAMEIN_STRCHANGE_EX_STR_MAX (3)

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	名前入力データ
//=====================================
struct _NAMEIN_KEYMAP
{
	u16			w;
	u16			h;
	STRCODE	str[0];
} ;

//-------------------------------------
///	変換用データ
//=====================================
typedef struct
{	
	STRCODE	input[NAMEIN_STRCHANGE_CODE_LEN];
	STRCODE	change[NAMEIN_STRCHANGE_CODE_LEN];
	STRCODE	shift[NAMEIN_STRCHANGE_CODE_LEN];
	STRCODE	rest[NAMEIN_STRCHANGE_CODE_LEN];
}CHANGECODE;
struct _NAMEIN_STRCHANGE
{
	u16					num;
	CHANGECODE	word[0];
} ;

//-------------------------------------
///	変換用データ拡張版
//=====================================
typedef struct
{	
	STRCODE	change[NAMEIN_STRCHANGE_EX_STR_MAX];
}CHANGECODE_EX;
struct _NAMEIN_STRCHANGE_EX
{ 
  u16           num;
  CHANGECODE_EX word[0];
};

//-------------------------------------
///	ハンドル
//=====================================
struct _NAMEIN_KEYMAP_HANDLE
{ 
  NAMEIN_KEYMAP *p_data[ NAMEIN_KEYMAPTYPE_MAX ];
};


//=============================================================================
/**
 *	プロトタイプ宣言
 */
//=============================================================================
static BOOL STRCODE_Search( const STRCODE* data, const STRCODE* code );

//=============================================================================
/**
 *				キー配列
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	データ読みこみ
 *
 *	@param	NAMEIN_KEYMAP type	タイプ
 *	@param	heapID						ヒープID
 *
 *	@return	データハンドル
 */
//-----------------------------------------------------------------------------
NAMEIN_KEYMAP *NAMEIN_KEYMAP_Alloc( NAMEIN_KEYMAPTYPE type, HEAPID heapID )
{	
	NAMEIN_KEYMAP	*p_wk;

	GF_ASSERT( type < NAMEIN_KEYMAPTYPE_MAX );

	//narcのインデックスがタイプ順になっているので、
	//そのまま読み込む
	p_wk	= GFL_ARC_UTIL_Load( ARCID_NAMEIN_DATA, NARC_namein_data_keymap_kana_dat + type, FALSE, heapID );

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	データ破棄
 *
 *	@param	NAMEIN_KEYMAP *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void NAMEIN_KEYMAP_Free( NAMEIN_KEYMAP *p_wk )
{	
	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  メモリ上に全データをおくためのハンドル作成
 *
 *	@param	HEAPID heapID   ヒープID
 *
 *	@return KEYMAPをすべてメモリ上におくためのワーク
 */
//-----------------------------------------------------------------------------
NAMEIN_KEYMAP_HANDLE * NAMEIN_KEYMAP_HANDLE_Alloc( HEAPID heapID )
{ 
  NAMEIN_KEYMAP_HANDLE *p_wk;
  p_wk= GFL_HEAP_AllocMemory( heapID, sizeof(NAMEIN_KEYMAP_HANDLE) );
  GFL_STD_MemClear( p_wk, sizeof(NAMEIN_KEYMAP_HANDLE) );

  {
    int i;
    for( i = 0; i < NAMEIN_KEYMAPTYPE_MAX; i++ )
    { 
      p_wk->p_data[i] = NAMEIN_KEYMAP_Alloc( i, heapID );

#ifdef PM_DEBUG
      //太陽のマークチェック
      if( i == NAMEIN_KEYMAPTYPE_KIGOU )
      { 
        NAGI_Printf( "太陽の文字コード 0x%x\n", NAMEIN_KEYMAP_GetStr( p_wk->p_data[i], 0, 4 ) );
        NAGI_Printf( "曇の文字コード 0x%x\n", NAMEIN_KEYMAP_GetStr( p_wk->p_data[i], 1, 4 ) );
        NAGI_Printf( "雨の文字コード 0x%x\n", NAMEIN_KEYMAP_GetStr( p_wk->p_data[i], 2, 4 ) );
        NAGI_Printf( "雪の文字コード 0x%x\n", NAMEIN_KEYMAP_GetStr( p_wk->p_data[i], 3, 4 ) );
      }
#endif //PM_DEBUG

    }
  }

  return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ハンドル破棄
 *
 *	@param	NAMEIN_KEYMAP_HANDLE *p_handleハンドル
 */
//-----------------------------------------------------------------------------
void NAMEIN_KEYMAP_HANDLE_Free( NAMEIN_KEYMAP_HANDLE *p_handle )
{ 
  {
    int i;
    for( i = 0; i < NAMEIN_KEYMAPTYPE_MAX; i++ )
    { 
      NAMEIN_KEYMAP_Free( p_handle->p_data[i] );
    }
  }

  GFL_HEAP_FreeMemory( p_handle );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ハンドルからデータを取得
 *
 *	@param	NAMEIN_KEYMAP_HANDLE *p_handle ハンドル
 */
//-----------------------------------------------------------------------------
NAMEIN_KEYMAP *NAMEIN_KEYMAP_HANDLE_GetData( NAMEIN_KEYMAP_HANDLE *p_handle, NAMEIN_KEYMAPTYPE type )
{ 
  return p_handle->p_data[ type ];
}


//----------------------------------------------------------------------------
/**
 *	@brief	データ幅を取得
 *
 *	@param	const NAMEIN_KEYMAP *cp_wk	ワーク
 *
 *	@return	幅
 */
//-----------------------------------------------------------------------------
u16 NAMEIN_KEYMAP_GetWidth( const NAMEIN_KEYMAP *cp_wk )
{	
	return cp_wk->w;
}
//----------------------------------------------------------------------------
/**
 *	@brief	データ高さを取得
 *
 *	@param	const NAMEIN_KEYMAP *cp_wk	ワーク
 *
 *	@return	高さ
 */
//-----------------------------------------------------------------------------
u16 NAMEIN_KEYMAP_GetHeight( const NAMEIN_KEYMAP *cp_wk )
{	
	return cp_wk->h;
}
//----------------------------------------------------------------------------
/**
 *	@brief	指定位置の文字を取得
 *
 *	@param	const NAMEIN_KEYMAP *cp_wk	ワーク
 *	@param	x	X位置
 *	@param	y	Y位置
 *
 *	@return	文字を取得
 */
//-----------------------------------------------------------------------------
STRCODE NAMEIN_KEYMAP_GetStr( const NAMEIN_KEYMAP *cp_wk, u16 x, u16 y )
{	
	GF_ASSERT( x < NAMEIN_KEYMAP_GetWidth(cp_wk) );
	GF_ASSERT( y < NAMEIN_KEYMAP_GetHeight(cp_wk) );

	return cp_wk->str[  y * NAMEIN_KEYMAP_GetWidth( cp_wk ) + x ];
}

//=============================================================================
/**
 *				変換
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	データ読みこみ
 *
 *	@param	NAMEIN_STRCHANGE type	タイプ
 *	@param	heapID						ヒープID
 *
 *	@return	データハンドル
 */
//-----------------------------------------------------------------------------
NAMEIN_STRCHANGE *NAMEIN_STRCHANGE_Alloc( NAMEIN_STRCHANGETYPE type, HEAPID heapID )
{	
	NAMEIN_STRCHANGE	*p_wk;

	GF_ASSERT( type < NAMEIN_STRCHANGETYPE_MAX );

	//narcのインデックスがタイプ順になっているので、
	//そのまま読み込む
	p_wk	= GFL_ARC_UTIL_Load( ARCID_NAMEIN_DATA, NARC_namein_data_change_dakuten_dat + type, FALSE, heapID );

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	データ破棄
 *
 *	@param	NAMEIN_STRCHANGE *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void NAMEIN_STRCHANGE_Free( NAMEIN_STRCHANGE *p_wk )
{	
	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	データ幅を取得
 *
 *	@param	const NAMEIN_STRCHANGE *cp_wk	ワーク
 *
 *	@return	幅
 */
//-----------------------------------------------------------------------------
u16 NAMEIN_STRCHANGE_GetNum( const NAMEIN_STRCHANGE *cp_wk )
{	
	return cp_wk->num;
}
//----------------------------------------------------------------------------
/**
 *	@brief	変換前文字列取得
 *
 *	@param	const NAMEIN_STRCHANGE *cp_wk	ワーク
 *	@param	idx				インデックス
 *	@param	*p_code		受け取り配列
 *	@param	code_len	受け取り配列要素数
 *	@param	*p_len		何文字あるか
 */
//-----------------------------------------------------------------------------
void NAMEIN_STRCHANGE_GetInputStr( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_len, u8 *p_len )
{	
	GF_ASSERT( idx < NAMEIN_STRCHANGE_GetNum(cp_wk) );
	STRTOOL_Copy( cp_wk->word[ idx ].input, p_code, code_len );	
	if( p_len )
	{	
		for( *p_len = 0; *p_len < NAMEIN_STRCHANGE_CODE_LEN && p_code[*p_len] != NAMEIN_STRCHANGE_EMPTY ; (*p_len)++ )
		{	};
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	変換後文字列取得
 *
 *	@param	const NAMEIN_STRCHANGE *cp_wk	ワーク
 *	@param	idx				インデックス
 *	@param	*p_code		受け取り配列
 *	@param	code_len	受け取り配列要素数
 *	@param	*p_len		何文字あるか
 *	@return	TRUEならば存在、FALSEならば存在しない
 */
//-----------------------------------------------------------------------------
BOOL NAMEIN_STRCHANGE_GetChangeStr( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_len, u8 *p_len )
{	
	u8	len;

	GF_ASSERT( idx < NAMEIN_STRCHANGE_GetNum(cp_wk) );
	STRTOOL_Copy( cp_wk->word[ idx ].change, p_code, code_len );	
	for( len = 0; len < NAMEIN_STRCHANGE_CODE_LEN && p_code[len] != NAMEIN_STRCHANGE_EMPTY ; (len)++ )
	{	};
	if( p_len )
	{	
		*p_len = len;
	}
	return len != 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SHIFT変換後文字列取得
 *
 *	@param	const NAMEIN_STRCHANGE *cp_wk	ワーク
 *	@param	idx				インデックス
 *	@param	*p_code		受け取り配列
 *	@param	code_len	受け取り配列要素数
 *	@param	*p_len		何文字あるか
 *	@return	TRUEならば存在、FALSEならば存在しない
 */
//-----------------------------------------------------------------------------
BOOL NAMEIN_STRCHANGE_GetChangeShiftStr( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_len, u8 *p_len )
{	
	u8	len;

	GF_ASSERT( idx < NAMEIN_STRCHANGE_GetNum(cp_wk) );
	STRTOOL_Copy( cp_wk->word[ idx ].shift, p_code, code_len );	
	for( len = 0; len < NAMEIN_STRCHANGE_CODE_LEN && p_code[len] != NAMEIN_STRCHANGE_EMPTY ; (len)++ )
	{	};
	if( p_len )
	{	
		*p_len = len;
	}
	return len != 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	変換後に残る文字列取得
 *
 *	@param	const NAMEIN_STRCHANGE *cp_wk	ワーク
 *	@param	idx				インデックス
 *	@param	*p_code		受け取り配列
 *	@param	code_len	受け取り配列要素数
 *	@param	*p_len		何文字あるか
 *	@return	TRUEならば存在、FALSEならば存在しない
 */
//-----------------------------------------------------------------------------
BOOL NAMEIN_STRCHANGE_GetRestStr( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_len, u8 *p_len )
{	
	u8	len;

	GF_ASSERT( idx < NAMEIN_STRCHANGE_GetNum(cp_wk) );
	STRTOOL_Copy( cp_wk->word[ idx ].rest, p_code, code_len );
	for( len = 0; len < NAMEIN_STRCHANGE_CODE_LEN && p_code[len] != NAMEIN_STRCHANGE_EMPTY ; (len)++ )
	{	};
	if( p_len )
	{	
		*p_len = len;
	}
	return len != 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	変換前文字列から、インデックスを得る
 *
 *	@param	const NAMEIN_STRCHANGE *cp_wk	ワーク
 *	@param	*p_code												変換前文字列
 *
 *	@return	インデックス	なかった場合はNAMEIN_STRCHANGE_NONE
 */
//-----------------------------------------------------------------------------
u16 NAMEIN_STRCHANGE_GetIndexByInputStr( const NAMEIN_STRCHANGE *cp_wk, const STRCODE *cp_code )
{	
	int i;
	u16 max;

	max	= NAMEIN_STRCHANGE_GetNum(cp_wk);
	for( i = 0; i < max; i++ )
	{	
		if( STRCODE_Search( cp_wk->word[ i ].input, cp_code) )
		{	
			return i;
		}
	}

	return NAMEIN_STRCHANGE_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	検索文字列をサーチ
 *
 *	@param	const STRCODE* data	データ
 *	@param	STRCODE* code				検索文字列
 *
 *	@return	TRUEで成功
 */
//-----------------------------------------------------------------------------
static BOOL STRCODE_Search( const STRCODE* data, const STRCODE* code )
{
	STRCODE EOMCODE = GFL_STR_GetEOMCode();
	int i;

	for( i = 0; i < NAMEIN_STRCHANGE_CODE_LEN; i++ )
	{
		if( *data == NAMEIN_STRCHANGE_EMPTY
				&& *code == EOMCODE )
		{	
			return TRUE;
		}
		if( *data != *code )
		{ 
			return FALSE; 
		}
		if( *code == EOMCODE )
		{	
			return FALSE;
		}
		data++;
		code++;
	}

	//ここにくる条件は
	//3文字あった

	return TRUE;
}


//=============================================================================
/**
 *					変換の外部公開
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  変換拡張  読み込み
 *
 *	@param	HEAPID heapID   ヒープID
 *
 *	@return ワーク
 */
//-----------------------------------------------------------------------------
NAMEIN_STRCHANGE_EX *NAMEIN_STRCHANGE_EX_Alloc( HEAPID heapID )
{ 
	return GFL_ARC_UTIL_Load( ARCID_NAMEIN_DATA, NARC_namein_data_changeex_all_dat, FALSE, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief  変換拡張  破棄
 *
 *	@param	NAMEIN_STRCHANGE_EX *p_wk ワーク
 *
 */
//-----------------------------------------------------------------------------
void NAMEIN_STRCHANGE_EX_Free( NAMEIN_STRCHANGE_EX *p_wk )
{ 
	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  変換拡張  変換文字の数取得
 *
 *	@param	const NAMEIN_STRCHANGE_EX *cp_wk  ワーク
 *
 *	@return 総数
 */
//-----------------------------------------------------------------------------
u16 NAMEIN_STRCHANGE_EX_GetNum( const NAMEIN_STRCHANGE_EX *cp_wk )
{
  return cp_wk->num;
}

//----------------------------------------------------------------------------
/**
 *	@brief  変換文字を取得
 *
 *	@param	const NAMEIN_STRCHANGE_EX *cp_wk  ワーク
 *	@param	index   インデックス（0〜NAMEIN_STRCHANGE_EX_GetNumまで）
 *	@param	num     0〜3まで
 *
 *	@return もじ
 */
//-----------------------------------------------------------------------------
static STRCODE NAMEIN_STRCHANGE_EX_GetWord( const NAMEIN_STRCHANGE_EX *cp_wk, u16 index, u16 num )
{ 
  return cp_wk->word[ index ].change[ num ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  変換変換  変換出来る場合変換先文字列を返す
 *
 *	@param	const NAMEIN_STRCHANGE_EX *cp_wk  ワーク
 *	@param	STRCODE *cp_code  変換元文字列
 *	@param	*p_code           変換先文字列
 *
 *	@return TRUEならば変換可能  FALSEならば不可能
 */
//-----------------------------------------------------------------------------
BOOL NAMEIN_STRCHANGE_EX_GetChangeStr( const NAMEIN_STRCHANGE_EX *cp_wk, const STRCODE *cp_code, STRCODE *p_code )
{ 
  int i,j;
  
  if( NAMEIN_STRCHANGE_EMPTY == *cp_code )
  { 
    return FALSE;
  }

  for( i = 0; i < NAMEIN_STRCHANGE_EX_GetNum(cp_wk); i++ )
  { 
    for( j = 0; j < NAMEIN_STRCHANGE_EX_STR_MAX; j++ )
    { 
      if( NAMEIN_STRCHANGE_EX_GetWord( cp_wk, i, j ) == *cp_code )
      { 
        //一致したので、変換先を返す
        //ただし空文字の場合次の変換
        do
        { 
          j++;
          j %= NAMEIN_STRCHANGE_EX_STR_MAX;
          *p_code = cp_wk->word[ i ].change[ j ];
        }while( *p_code == NAMEIN_STRCHANGE_EMPTY );

        return TRUE;
      }
    }
  }

  return FALSE;
}

//=============================================================================
/**
 *					ここの複数のモジュールを使用する版
 *					  内部で上記モジュールのロード、アンロードを行います
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  入力できる文字かどうかチェックする
 *	        （海外文字対応として、初期表示文字に入力不可能文字があった場合クリアするため）
 *
 *	@param	STRCODE *cp_code            検知する文字列(EOMを必ず含んでいること！)
 *	@param	heapID                      ヒープID
 *
 *
 *	@return TRUEならば入力可能文字のみ  FALSEならば入力不可能な文字が入っている
 */
//-----------------------------------------------------------------------------
BOOL NAMEIN_DATA_CheckInputStr( const STRCODE *cp_code, HEAPID heapID )
{ 
  BOOL ret;
  NAMEIN_KEYMAP   *p_keymap[NAMEIN_KEYMAPTYPE_MAX-1]; //NAMEIN_KEYMAPTYPE_QWERTYはチェックしなくても良い
  NAMEIN_STRCHANGE_EX *p_changeex;  //本来Rボタンで巡回するだけの表だが＝変換が全てはいっているので


  //読み込み
  {
    int i;
    for( i = 0; i < NAMEIN_KEYMAPTYPE_MAX-1; i++ )
    { 
      p_keymap[i]  = NAMEIN_KEYMAP_Alloc( i, GFL_HEAP_LOWID(heapID) );
    }

    p_changeex  = NAMEIN_STRCHANGE_EX_Alloc( GFL_HEAP_LOWID(heapID) );
  }


  //チェック
  { 
    int i,j,k,l;
    BOOL exist;

    ret = TRUE;
    for( k = 0; cp_code[k] != GFL_STR_GetEOMCode(); k++ )
    { 
      exist = FALSE;

      //キー配列をチェック
      for( l = 0; l < NELEMS(p_keymap); l++ )
      { 
        for( i = 0; i < NAMEIN_KEYMAP_GetHeight(p_keymap[l]) && exist == FALSE; i++ )
        { 
          for( j = 0; j < NAMEIN_KEYMAP_GetWidth(p_keymap[l]) && exist == FALSE; j++ )
          {
            if( cp_code[k] == NAMEIN_KEYMAP_GetStr( p_keymap[l], j, i ) )
            {
              exist = TRUE;
            }
          }
        }
      }
      //変換文字をチェック
      for( l = 0; l < NAMEIN_STRCHANGE_EX_GetNum( p_changeex ); l++ )
      { 
        for( j = 0; j < NAMEIN_STRCHANGE_EX_STR_MAX; j++ )
        { 
          if( NAMEIN_STRCHANGE_EX_GetWord( p_changeex, l, j ) == cp_code[k] )
          { 
            exist = TRUE;
          }
        }
      }

      if( exist == FALSE )
      { 
        ret = FALSE;
        break;
      }
    }
  }


  //破棄
  {
    int i;

    NAMEIN_STRCHANGE_EX_Free( p_changeex );

    for( i = 0; i < NAMEIN_KEYMAPTYPE_MAX-1; i++ )
    { 
      NAMEIN_KEYMAP_Free( p_keymap[i] );
    }
  }


  return ret;
}
