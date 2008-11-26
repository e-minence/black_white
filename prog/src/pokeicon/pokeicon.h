//==============================================================================
/**
 * @file	pokeicon.h
 * @brief	ポケモンアイコンのヘッダ
 * @author	matsuda
 * @date	2008.11.26(水)
 */
//==============================================================================
#ifndef __POKEICON_H__
#define __POKEICON_H__

#include "poke_tool/poke_tool.h"


//==============================================================================
//	定数定義
//==============================================================================
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


//==============================================================================
//	外部関数宣言
//==============================================================================
extern u32 POKEICON_GetCgxArcIndex( const POKEMON_PASO_PARAM* ppp );
extern u32 POKEICON_GetCgxArcIndexByMonsNumber( u32 mons, u32 form_no, BOOL egg );
extern u16 POKEICON_GetCgxForm( const POKEMON_PASO_PARAM* ppp );
extern const u8 POKEICON_GetPalNum( u32 mons, u32 form, BOOL egg );
extern const u8 POKEICON_GetPalNumByPPP( const POKEMON_PASO_PARAM * ppp );
extern u8 POKEICON_GetPalNumGetByPPP( const POKEMON_PASO_PARAM * ppp );
extern u32 POKEICON_GetPalArcIndex(void);
extern u32 POKEICON_GetCellArcIndex(void);
extern u32 POKEICON_GetAnmCellArcIndex(void);
extern u32 POKEICON_Get64kCellArcIndex(void);
extern u32 POKEICON_GetCellAnmArcIndex(void);
extern u32 POKEICON_GetAnmCellAnmArcIndex(void);
extern u32 POKEICON_Get64kCellAnmArcIndex(void);

#endif	//__POKEICON_H__
