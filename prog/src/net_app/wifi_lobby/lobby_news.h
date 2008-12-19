//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *	GAME FREAK inc.
 *
 *	@file		lobby_news.h
 *	@brief		ロビーニュース	描画
 *	@author		tomoya takahashi
 *	@data		2007.10.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#ifndef __LOBBY_NEWS_H__
#define __LOBBY_NEWS_H__

#include "lobby_news_data.h"
#include "wflby_system_def.h"

#include <procsys.h>


//-----------------------------------------------------------------------------
/**
 *					定数宣言
*/
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
/**
 *					構造体宣言
*/
//-----------------------------------------------------------------------------

//-------------------------------------
///	データ表示用	プロックデータ
//=====================================
typedef struct{
	NEWS_DATA* p_data;
	const WFLBY_TIME* cp_nowtime;	// 現在時間
	WFLBY_SYSTEM* p_system;

} NEWS_DRAW_DATA;



//-----------------------------------------------------------------------------
/**
 *					プロトタイプ宣言
*/
//-----------------------------------------------------------------------------
extern GFL_PROC_RESULT NEWS_DRAW_Init(GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk);
extern GFL_PROC_RESULT NEWS_DRAW_Main(GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk);
extern GFL_PROC_RESULT NEWS_DRAW_Exit(GFL_PROC* p_proc, int* p_seq, void * pwk, void * mywk);


#endif		// __LOBBY_NEWS_H__

