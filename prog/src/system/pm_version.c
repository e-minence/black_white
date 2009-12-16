//============================================================================================
/**
 * @file	pm_version.c
 * @brief	バージョン定義ソース
 * @date	2005.12.01
 * @author	GAME FREAK Inc.
 *
 *
 * 2009.12.16   HGSSから移植
 */
//============================================================================================

/*
 * 今のところpm_version.hがプリコンパイルヘッダに含まれており、
 * プリコンパイルヘッダは全てのソースのコンパイルで対象に含まれるため
 * このソースはpm_version.hヘッダをインクルードしていない
 */

#include <gflib.h>
#include "debug/debug_flg.h"

//============================================================================================
//
//	バージョンチェック
//
//============================================================================================

#ifndef	PM_VERSION
/* PM_VERSION未定義、問題あり!*/
#error	PM_VERSION UNDEFINED!!
#endif

#if (PM_VERSION == VERSION_WHITE || PM_VERSION == VERSION_BLACK )
/*WHITE or BLACK、問題なし */
#else
/* 未定義バージョン、問題あり！ */
#error!!
#endif

#ifndef	PM_LANG
/* PM_LANG未定義、問題あり!*/
#error	PM_LANG	UNDEFINED!!
#endif

#if (PM_LANG == LANG_JAPAN )
/* 日本語、問題なし */
#else
/* 未定義の言語、問題あり!　*/
#error!!
#endif

//============================================================================================
//
//	バージョン参照用データ
//
//============================================================================================

//---------------------------------------------------------------------------
/**
 * @brief	ポケモンのカートリッジバージョンを格納しているconst変数
 */
//---------------------------------------------------------------------------
const u8 CasetteVersion = PM_VERSION;

//---------------------------------------------------------------------------
/**
 * @brief	使用言語の種類を保持するロムデータ
 */
//---------------------------------------------------------------------------
const u8 CasetteLanguage = PM_LANG;

//============================================================================================
//============================================================================================
//---------------------------------------------------------------------------
/**
 * @brief 
 */
//---------------------------------------------------------------------------
const u8 GetVersion( void )
{
#ifdef  PM_DEBUG
  if (DEBUG_FLG_GetFlg( DEBUG_FLG_ReverseVersion ) == TRUE )
  {
    if (CasetteVersion == VERSION_BLACK)
    {
      return VERSION_WHITE;
    }
    else
    {
      return VERSION_BLACK;
    }
  }
#endif  //PM_DEBUG
  return CasetteVersion;
}



//============================================================================================
//============================================================================================
//#include "d_date.h"		/* 日付文字列：ROM作成 */

