//============================================================================================
/**
 * @file	pokeicon.h
 * @bfief	ポケモンアイコン関連処理
 * @author	Hiroyuki Nakamura
 * @date	05.09.16
 */
//============================================================================================
#ifndef POKEICON_H
#define POKEICON_H

#include "poketool\poke_tool.h"

#undef GLOBAL
#ifdef POKEICON_H_GLOBAL
#define GLOBAL	/*	*/
#else
#define GLOBAL	extern
#endif


//============================================================================================
//	定数定義
//============================================================================================
// デオキシス用定義
enum {
	PICON_DEOKISISU_N = 0,		// ノーマルフォルム
	PICON_DEOKISISU_A,			// アタックフォルム
	PICON_DEOKISISU_D,			// ディフェンスフォルム
	PICON_DEOKISISU_S			// スピードフォルム
};

// アンノーン用定義
enum {
	PICON_ANNOON_A = 0,			// A
	PICON_ANNOON_B,				// B
	PICON_ANNOON_C,				// C
	PICON_ANNOON_D,				// D
	PICON_ANNOON_E,				// E
	PICON_ANNOON_F,				// F
	PICON_ANNOON_G,				// G
	PICON_ANNOON_H,				// H
	PICON_ANNOON_I,				// I
	PICON_ANNOON_J,				// J
	PICON_ANNOON_K,				// K
	PICON_ANNOON_L,				// L
	PICON_ANNOON_M,				// M
	PICON_ANNOON_N,				// N
	PICON_ANNOON_O,				// O
	PICON_ANNOON_P,				// P
	PICON_ANNOON_Q,				// Q
	PICON_ANNOON_R,				// R
	PICON_ANNOON_S,				// S
	PICON_ANNOON_T,				// T
	PICON_ANNOON_U,				// U
	PICON_ANNOON_V,				// V
	PICON_ANNOON_W,				// W
	PICON_ANNOON_X,				// X
	PICON_ANNOON_Y,				// Y
	PICON_ANNOON_Z,				// Z
	PICON_ANNOON_EXQ,			// !
	PICON_ANNOON_QUE,			// ?
};

// アニメウェイト取得パラメータ
enum {
	POKEICON_HP_MAX = 0,
	POKEICON_HP_GREEN,
	POKEICON_HP_YERROW,
	POKEICON_HP_RED,
	POKEICON_HP_NULL
};

// アニメ種類
enum {
	POKEICON_ANM_DEATH = 0,		// 瀕死
	POKEICON_ANM_HPMAX,			// HP MAX
	POKEICON_ANM_HPGREEN,		// HP 緑
	POKEICON_ANM_HPYERROW,		// HP 黄
	POKEICON_ANM_HPRED,			// HP 赤
	POKEICON_ANM_STCHG,			// 状態異常
};

#define	POKEICON_PAL_MAX	( 3 )	// アイコンで使用するパレット数

#define POKEICON_SIZE_X		(32)	// アイコンの横サイズ
#define POKEICON_SIZE_Y		(32)	// アイコンの縦サイズ

//============================================================================================
//	プロトタイプ宣言
//============================================================================================

//--------------------------------------------------------------------------------------------
/**
 * キャラのアーカイブインデックス取得 ( POKEMON_PASO_PARAM 版 )
 *
 * @param	ppp			POKEMON_PASO_PARAM
 *
 * @return	アーカイブインデックス
 *
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 PokeIconCgxArcIndexGet( const POKEMON_PASO_PARAM* ppp );

//--------------------------------------------------------------------------------------------
/**
 * キャラのアーカイブインデックス取得 ( POKEMON_PARAM 版 )
 *
 * @param	pp			POKEMON_PARAM
 *
 * @return	アーカイブインデックス
 *
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 PokeIconCgxArcIndexGetByPP( POKEMON_PARAM * pp );

//--------------------------------------------------------------------------------------------
/**
 * キャラのアーカイブインデックス取得
 *
 * @param	mons		ポケモン番号
 * @param	egg			タマゴフラグ
 * @param	pat			拡張パターン
 *
 * @return	アーカイブインデックス
 *
 *	patはデオキシスやアンノーンに使用
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 PokeIconCgxArcIndexGetByMonsNumber( u32 mons, u32 egg, u32 pat );

//--------------------------------------------------------------------------------------------
/**
 * キャラの拡張パターンナンバーを取得（取得したナンバーはPokeIconCgxArcIndexGetByMonsNoの引数として使える）
 *
 * @param   ppp		
 *
 * @retval  u16		拡張パターンナンバー
 */
//--------------------------------------------------------------------------------------------
GLOBAL u16 PokeIconCgxPatternGet( const POKEMON_PASO_PARAM* ppp );

//--------------------------------------------------------------------------------------------
/**
 * パレット番号取得
 *
 * @param	mons		ポケモン番号
 * @param	form		フォルム番号
 * @param	egg			タマゴフラグ
 *
 * @return	パレット番号
 */
//--------------------------------------------------------------------------------------------
GLOBAL const u8 PokeIconPalNumGet( u32 mons, u32 form, u32 egg );

//--------------------------------------------------------------------------------------------
/**
 * パレット番号取得（POKEMON_PASO_PARAM版）
 *
 * @param	ppp		POKEMON_PASO_PARAM
 *
 * @return	パレット番号
 */
//--------------------------------------------------------------------------------------------
GLOBAL const u8 PokeIconPalNumGetByPPP( const POKEMON_PASO_PARAM * ppp );

//--------------------------------------------------------------------------------------------
/**
 * パレット番号取得（POKEMON_PARAM版）
 *
 * @param	pp		POKEMON_PARAM
 *
 * @return	パレット番号
 */
//--------------------------------------------------------------------------------------------
GLOBAL const u8 PokeIconPalNumGetByPP( POKEMON_PARAM * pp );

//--------------------------------------------------------------------------------------------
/**
 * パレットのアーカイブインデックス取得
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 PokeIconPalArcIndexGet(void);

//--------------------------------------------------------------------------------------------
/**
 * セルのアーカイブインデックス取得
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 PokeIconCellArcIndexGet(void);

//--------------------------------------------------------------------------------------------
/**
 * セルのアーカイブインデックス取得（アニメ入り）
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 PokeIconAnmCellArcIndexGet(void);

//--------------------------------------------------------------------------------------------
/**
 * セルのアーカイブインデックス取得（64k,アニメ入り）
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 PokeIcon64kCellArcIndexGet(void);

//--------------------------------------------------------------------------------------------
/**
 * セルアニメのアーカイブインデックス取得
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 PokeIconCellAnmArcIndexGet(void);

//--------------------------------------------------------------------------------------------
/**
 * セルアニメのアーカイブインデックス取得（アニメ入り）
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 PokeIconAnmCellAnmArcIndexGet(void);

//--------------------------------------------------------------------------------------------
/**
 * セルアニメのアーカイブインデックス取得（64k,アニメ入り）
 *
 * @param	none
 *
 * @return	アーカイブインデックス
 */
//--------------------------------------------------------------------------------------------
GLOBAL u32 PokeIcon64kCellAnmArcIndexGet(void);


#undef GLOBAL
#endif	// POKEICON_H
