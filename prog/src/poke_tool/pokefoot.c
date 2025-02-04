//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		pokefoot.c
 *	@brief		ポケモン足跡グラフィックデータ取得関数
 *	@author		tomoya takahashi
 *	@data		2006.01.31
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#include <gflib.h>
#include "poke_tool/monsno_def.h"

#define	__POKEFOOT_H_GLOBAL
#include "poke_tool/pokefoot.h"

#include "arc_def.h"

//-----------------------------------------------------------------------------
/**
 *					コーディング規約
 *		●関数名
 *				１文字目は大文字それ以降は小文字にする
 *		●変数名
 *				・変数共通
 *						constには c_ を付ける
 *						staticには s_ を付ける
 *						ポインタには p_ を付ける
 *						全て合わさると csp_ となる
 *				・グローバル変数
 *						１文字目は大文字
 *				・関数内変数
 *						小文字と”＿”と数字を使用する 関数の引数もこれと同じ
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
//----------------------------------------------------------------------------
/**
 *	@brief	モンスターナンバーから足跡キャラクタデータ取得
 *
 *	@param	mons_no		モンスターナンバー
 *	@param	heap		ヒープ
 *
 *	@return	ファイルデータ
 */
//-----------------------------------------------------------------------------
void* PokeFootCharPtrGet( int mons_no, HEAPID heap )
{	
	void* p_buff;
	
	GF_ASSERT( mons_no <= MONSNO_END );

	p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CHAR_DMMY + mons_no, TRUE, heap );

	// 取得失敗
	GF_ASSERT( p_buff );

	return p_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief	足跡パレットデータ取得
 *
 *	@param	heap	ヒープ
 *
 *	@return	ファイルデータ
 */
//-----------------------------------------------------------------------------
void* PokeFootPlttPtrGet( HEAPID heap )
{
	void* p_buff;
	
	p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_PLTT, FALSE, heap );

	// 取得失敗
	GF_ASSERT( p_buff );

	return p_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief	足跡セルデータ取得
 *
 *	@param	heap	ヒープ
 *
 *	@return	ファイルデータ
 */
//-----------------------------------------------------------------------------
//void* PokeFootCellPtrGet( HEAPID heap )
void* PokeFootCellPtrGet( int mons_no, HEAPID heap )
{
	void* p_buff;

	GF_ASSERT( mons_no <= MONSNO_END );

	//p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CELL, TRUE, heap );
  if( mons_no <= POKEFOOT_MONS_NO_OLD_MAX )
  {
    p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CELL, TRUE, heap );
  }
  else
  {
    p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CELL_NEW, TRUE, heap );
  }

	// 取得失敗
	GF_ASSERT( p_buff );

	return p_buff;
}

//----------------------------------------------------------------------------
/**
 *	@brief	足跡セルアニメデータ取得
 *
 *	@param	heap	ヒープ
 *
 *	@return	ファイルデータ
 */
//-----------------------------------------------------------------------------
//void* PokeFootCellAnmPtrGet( HEAPID heap )
void* PokeFootCellAnmPtrGet( int mons_no, HEAPID heap )
{
	void* p_buff;
	
	//p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CELLANM, TRUE, heap );
  if( mons_no <= POKEFOOT_MONS_NO_OLD_MAX )
  {
	  p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CELLANM, TRUE, heap );
  }
  else
  {
	  p_buff = GFL_ARC_UTIL_Load( ARCID_POKEFOOT_GRA, POKEFOOT_ARC_CELLANM_NEW, TRUE, heap );
  }

	// 取得失敗
	GF_ASSERT( p_buff );

	return p_buff;
}



// アーカイブファイルIDX　データIDX
//----------------------------------------------------------------------------
/**
 *	@brief	アーカイブファイルIDX取得
 *
 *	@param	none	
 *
 *	@return	アーカイブファイルIDX
 */
//-----------------------------------------------------------------------------
int PokeFootArcFileGet( void )
{
	return ARCID_POKEFOOT_GRA;
}

//----------------------------------------------------------------------------
/**
 *	@brief	キャラクタデータIDX取得
 *
 *	@param	mons_no		モンスターナンバー
 *
 *	@return	キャラクタデータIDX
 */
//-----------------------------------------------------------------------------
int PokeFootCharDataIdxGet( int mons_no )
{
	return POKEFOOT_ARC_CHAR_DMMY + mons_no;
}
//----------------------------------------------------------------------------
/**
 *	@brief	パレットデータIDX取得
 *
 *	@param	none
 *
 *	@return	パレットデータIDX
 */
//-----------------------------------------------------------------------------
int PokeFootPlttDataIdxGet( void )
{
	return POKEFOOT_ARC_PLTT;
}
//----------------------------------------------------------------------------
/**
 *	@brief	セルデータIDX取得
 *
 *	@param	none
 *
 *	@return	セルデータIDX
 */
//-----------------------------------------------------------------------------
//int PokeFootCellDataIdxGet( void )
int PokeFootCellDataIdxGet( int mons_no )
{
  if( mons_no <= POKEFOOT_MONS_NO_OLD_MAX )
  {
	  return POKEFOOT_ARC_CELL;
  }
  else
  {
	  return POKEFOOT_ARC_CELL_NEW;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	セルアニメデータIDX取得
 *
 *	@param	none
 *
 *	@return	セルアニメデータIDX
 */
//-----------------------------------------------------------------------------
//int PokeFootCellAnmDataIdxGet( void )
int PokeFootCellAnmDataIdxGet( int mons_no )
{
  if( mons_no <= POKEFOOT_MONS_NO_OLD_MAX )
  {
	  return POKEFOOT_ARC_CELLANM;
  }
  else
  {
	  return POKEFOOT_ARC_CELLANM_NEW;
  }
}

