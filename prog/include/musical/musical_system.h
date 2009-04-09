//======================================================================
/**
 * @file	musical_system.h
 * @brief	ミュージカル用 汎用定義
 * @author	ariizumi
 * @data	09/02/10
 */
//======================================================================
#ifndef MUSICAL_SYSTEM_H__
#define MUSICAL_SYSTEM_H__
#include "musical_define.h"


//ミュージカルの参加資格があるか調べる(仮)
BOOL	MUSICAL_SYSTEM_CheckEntryMusical( POKEMON_PARAM *pokePara );
//ミュージカル用パラメータの初期化
MUSICAL_POKE_PARAM* MUSICAL_SYSTEM_InitMusPoke( POKEMON_PARAM *pokePara , HEAPID heapId );

#endif MUSICAL_SYSTEM_H__
