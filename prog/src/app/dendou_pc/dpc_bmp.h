//============================================================================================
/**
 * @file		dpc_bmp.h
 * @brief		殿堂入り確認画面 ＢＭＰ関連
 * @author	Hiroyuki Nakamura
 * @date		10.03.29
 *
 *	モジュール名：DPCBMP
 */
//============================================================================================
#pragma	once


//============================================================================================
//	定数定義
//============================================================================================


//============================================================================================
//	プロトタイプ宣言
//============================================================================================

extern void DPCBMP_Init( DPCMAIN_WORK * wk );
extern void DPCBMP_Exit( DPCMAIN_WORK * wk );
extern void DPCBMP_PrintUtilTrans( DPCMAIN_WORK * wk );

extern void DPCBMP_PutTitle( DPCMAIN_WORK * wk );
extern void DPCBMP_PutPage( DPCMAIN_WORK * wk );
extern void DPCBMP_PutInfo( DPCMAIN_WORK * wk );
extern void DPCBMP_ClearInfo( DPCMAIN_WORK * wk );
