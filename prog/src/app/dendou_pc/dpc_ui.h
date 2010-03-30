//============================================================================================
/**
 * @file		dpc_ui.h
 * @brief		殿堂入り確認画面 ＵＩ関連
 * @author	Hiroyuki Nakamura
 * @date		10.03.29
 *
 *	モジュール名：DPCUI
 */
//============================================================================================
#pragma	once


//============================================================================================
//	定数定義
//============================================================================================

enum {
	DPCUI_ID_POKE1 = 0,
	DPCUI_ID_POKE2,
	DPCUI_ID_POKE3,
	DPCUI_ID_POKE4,
	DPCUI_ID_POKE5,
	DPCUI_ID_POKE6,

	DPCUI_ID_PAGE_LEFT,
	DPCUI_ID_PAGE_RIGHT,
	DPCUI_ID_EXIT,
	DPCUI_ID_RETURN,

	DPCUI_ID_NONE,
};

//============================================================================================
//	プロトタイプ宣言
//============================================================================================

extern void DPCUI_Init( DPCMAIN_WORK * wk );
extern void DPCUI_Exit( DPCMAIN_WORK * wk );
extern u32 DPCUI_Main( DPCMAIN_WORK * wk );
