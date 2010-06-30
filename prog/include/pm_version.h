//=========================================================================
/**
 * @file	pm_version.h
 * @brief	ポケモンROMや国コードの定義
 * @date	2005.11.25
 * @author	GAME FREAK inc.
 *
 * AGBポケモンの定義ファイルをそのまま移行した
 * POKEMON_PLの定義ファイルをそのまま移行した
 */
//=========================================================================

#ifndef __PM_VERSION_H__
#define __PM_VERSION_H__

//=========================================================================
//	
//
//	ソースのバージョン指定関連ファイル
//
//
//=========================================================================


//=========================================================================
//	定義
//=========================================================================

//　●色違いバージョンによって処理を分岐する場合は
//	　PM_VERSION　によって記述をかえる
//
//	例）
//	#if ( PM_VERSION == VERSION_RUBY )
//		ルビーバージョンの場合の処理を記述
//
//	#elif ( PM_VERSION == VERSION_SAPPHIRE )
//		サファイアバージョンの場合の処理を記述
//
//	#endif
//

//------------------------------------------------------------------
//	色違いバージョン指定
//------------------------------------------------------------------
#define		VERSION_SAPPHIRE	1	///<	バージョン：AGBサファイア
#define		VERSION_RUBY		2	///<	バージョン：AGBルビー
#define		VERSION_EMERALD		3	///<	バージョン：AGBエメラルド
#define		VERSION_RED			4	///<	バージョン：AGBファイアーレッド
#define		VERSION_GREEN		5	///<	バージョン：AGBリーフグリーン

#define		VERSION_GOLD		7	///<	バージョン：ゴールド用予約定義
#define		VERSION_SILVER		8	///<	バージョン：シルバー用予約定義

#define		VERSION_DIAMOND		10	///<	バージョン：DSダイヤモンド
#define		VERSION_PEARL		11	///<	バージョン：DSパール
#define		VERSION_PLATINUM	12	///<	バージョン：DSプラチナ

#define		VERSION_COLOSSEUM	15	///<	バージョン：GCコロシアム

#define		VERSION_WHITE		20	///<	バージョン：DSホワイト
#define		VERSION_BLACK		21	///<	バージョン：DSブラック


//------------------------------------------------------------------
//	言語コード指定
//------------------------------------------------------------------
#define		LANG_JAPAN		1		///<	言語コード：日本
#define		LANG_ENGLISH	2		///<	言語コード：英語
#define		LANG_FRANCE		3		///<	言語コード：フランス語
#define		LANG_ITALY		4		///<	言語コード：イタリア語
#define		LANG_GERMANY	5		///<	言語コード：ドイツ語
#define		LANG_SPAIN		7		///<	言語コード：スペイン語
#define		LANG_KOREA		8		///<	言語コード：韓国語


//------------------------------------------------------------------
//	性別コード指定
//------------------------------------------------------------------
#define		PM_MALE			0		///< 性別コード：男性
#define		PM_FEMALE		1		///< 性別コード：女性
#define		PM_NEUTRAL		2		///< 性別コード：中性

//=========================================================================
//	変数
//=========================================================================

#ifndef	ASM_CPP

#include "playable_version.h"

#ifdef  PLAYABLE_VERSION
extern u8 CasetteVersion;
#else
extern const u8 CasetteVersion;
#endif

extern const u8 CasetteLanguage;

extern const u8 GetVersion( void );

#ifdef  PLAYABLE_VERSION
extern void SetVersion( u8 version );
#endif
#endif

#ifdef  PM_DEBUG
#define GET_VERSION()  GetVersion()
#else
  #ifdef  PLAYABLE_VERSION
  #define GET_VERSION()  GetVersion()
  #else
  #define GET_VERSION()  PM_VERSION
  #endif
#endif

//=========================================================================
//	関数プロトタイプ
//=========================================================================


#endif	/*__PM_VERSION_H__*/

