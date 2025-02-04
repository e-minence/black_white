//============================================================================================
/**
 * @file		scroll_bar.c
 * @brief		スクロールバー関連処理
 * @author	Hiroyuki Nakamura
 * @date		10.02.03
 *
 *	モジュール名：SCROLLBAR
 */
//============================================================================================
#include <gflib.h>

#include "system/scroll_bar.h"


//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロールバーの表示Ｙ座標からスクロール値を取得
 *
 * @param		scrollMax			スクロール最大値
 * @param		py						スクロールバーの表示Ｙ座標
 * @param		railTop				レール上部のＹ座標
 * @param		railBottom		レール下部のＹ座標
 * @param		barSize				バーのＹサイズ
 *
 * @return	Ｙ座標
 */
//--------------------------------------------------------------------------------------------
u32 SCROLLBAR_GetCount( u32 scrollMax, u32 py, u32 railTop, u32 railBottom, u32 barSize )
{
	u32	cnt;

	barSize /= 2;

	railTop += barSize;
	railBottom -= barSize;

	if( py <= railTop ){
		return 0;
	}else if( py >= railBottom ){
		return scrollMax;
	}
	py -= railTop;

//	cnt = ( ( (scrollMax+1) << 8 ) / ( railBottom - railTop ) * py ) >> 8;
//	cnt = ( ( (scrollMax) << 8 ) / ( railBottom - railTop ) * py ) >> 8;		// BTS7404修正前までの処理。これだとSCROLLBAR_GetPosY()の結果と異なる。
	cnt = ( py * ( scrollMax << 8 ) / ( railBottom - railTop ) ) >> 8;

	return cnt;
}

//--------------------------------------------------------------------------------------------
/**
 * @brief		スクロールバーの表示Ｙ座標を取得
 *
 * @param		scrollMax			スクロール最大値
 * @param		scrollCount		現在のスクロール値
 * @param		railTop				レール上部のＹ座標
 * @param		railBottom		レール下部のＹ座標
 * @param		barSize				バーのＹサイズ
 *
 * @return	Ｙ座標
 */
//--------------------------------------------------------------------------------------------
u32 SCROLLBAR_GetPosY( u32 scrollMax, u32 scrollCount, u32 railTop, u32 railBottom, u32 barSize )
{
	u32	py;

	py = railBottom - railTop - barSize;
//	py = ( ( py << 8 ) / ( scrollMax + 1 ) * scrollCount ) >> 8;
	py = ( (( py << 8 ) * scrollCount) / ( scrollMax ) ) >> 8;

	return ( railTop + py + barSize/2 );
}
