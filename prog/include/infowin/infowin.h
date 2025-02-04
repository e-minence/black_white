//======================================================================
/**
 * @file	情報バー
 * @brief	画面上に常駐する情報表示画面
 * @author	ariizumi
 * @data	09/02/23
 */
//======================================================================

#pragma once

#include "gamesystem/game_data.h"

//======================================================================
//	define
//======================================================================

//NCGを読み込むVRAMサイズ
#define INFOWIN_CHARAREA_SIZE (0x1000)

//======================================================================
//	enum
//======================================================================

//======================================================================
//	typedef struct
//======================================================================

//======================================================================
//	proto
//======================================================================

//初期化
//	@param bgplane	BG面
//	@param pltNo	パレット番号
//	@param commSys	通信システム
//	@param heapId	ヒープID
//	VramのCharエリアに INFOWIN_CHARAREA_SIZE の空きが必要です
void	INFOWIN_Init( const u8 bgplane , const u8 pltNo, GAMEDATA *gameData ,const HEAPID heapId );
void	INFOWIN_Update( void );
void	INFOWIN_Exit( void );

