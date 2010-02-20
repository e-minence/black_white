//============================================================================================
/**
 * @file		zkntop_main.h
 * @brief		図鑑トップ画面　メイン処理
 * @author	Hiroyuki Nakamura
 * @date		10.02.20
 *
 *	モジュール名：ZKNTOPMAIN
 */
//============================================================================================
#pragma	once

#include "zukantop.h"


//============================================================================================
//	定数定義
//============================================================================================

typedef struct {
	ZUKANTOP_DATA * dat;

	GFL_TCB * vtask;		// TCB ( VBLANK )

	int	mainSeq;

}ZKNTOPMAIN_WORK;


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

extern BOOL ZKNTOPMAIN_MainSeq( ZKNTOPMAIN_WORK * wk );
