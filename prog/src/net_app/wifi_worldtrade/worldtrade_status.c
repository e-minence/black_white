//============================================================================================
/**
 * @file	worldtrade_status.c
 * @brief	世界交換ポケモンステータス画面呼び出し・復帰
 * @author	Akito Mori
 * @date	06.05.10
 */
//============================================================================================
#include <gflib.h>
#include <dwc.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "arc_def.h"
#include "libdpw/dpw_tr.h"
#include "print/wordset.h"
#include "message.naix"
#include "system/wipe.h"
//#include "system/fontproc.h"
//#include "system/fontoam.h"
//#include "system/window.h"
//TODO
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"
#include "savedata/wifilist.h"
#include "savedata/worldtrade_data.h"
//#include "savedata/zukanwork.h"
//TODO
#include "savedata/sp_ribbon_save.h"
#include "poke_tool/pokeparty.h"
#include "savedata/box_savedata.h"
#include "item/itemsym.h"

#include "net_app/worldtrade.h"
#include "worldtrade_local.h"

#include "libdpw/dpw_tr.h"




//============================================================================================
//	プロトタイプ宣言
//============================================================================================
/*** 関数プロトタイプ ***/
//static void InitWork( WORLDTRADE_WORK *wk );
//static void FreeWork( WORLDTRADE_WORK *wk );



// ステータス画面で表示するフラグの列（ビットテーブルにすればいいのに…）
static const u8 StatusPageTable[]={
	PST_PAGE_INFO_MEMO,		// 「ポケモンじょうほう」「トレーナーメモ」
	PST_PAGE_PARAM_B_SKILL,	// 「ポケモンのうりょく」「わざ説明」
	PST_PAGE_RIBBON,		// 「きねんリボン」
	PST_PAGE_RET,			// 「もどる」
	PST_PAGE_MAX

};

//============================================================================================
//	プロセス関数
//============================================================================================

//==============================================================================
/**
 * @brief   世界交換入り口画面初期化
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
int WorldTrade_Status_Init(WORLDTRADE_WORK *wk, int seq)
{
	// ワーク初期化
//	InitWork( wk );
	
	MORI_PRINT( "heap remain RAM = %d\n", GFL_HEAP_GetHeapFreeSize( GFL_HEAPID_APP ));
	
	wk->statusParam.ppd  = WorldTrade_GetPokePtr(wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos);
	wk->statusParam.ppt  = PST_PP_TYPE_POKEPASO;
	wk->statusParam.max  = 1;
	wk->statusParam.pos  = 0;
	wk->statusParam.mode = PST_MODE_NO_WAZACHG;	// 技入れ替え禁止にする
	wk->statusParam.waza = 0;
//	wk->statusParam.ev_contest = PokeStatus_ContestFlagGet(wk->param->savedata);
	wk->statusParam.ev_contest = FALSE;
	//TODO
	wk->statusParam.zukan_mode = wk->param->zukanmode;
	wk->statusParam.cfg        = wk->param->config;
	wk->statusParam.ribbon     = SP_RIBBON_SAVE_GetSaveData(wk->param->savedata);
//	wk->statusParam.pokethlon  = PokeStatus_PerformanceFlagGet(wk->param->savedata);
	wk->statusParam.pokethlon  = FALSE;
	//TODO
//	PokeStatus_PageSet( &wk->statusParam, StatusPageTable );
//	PokeStatus_PlayerSet( &wk->statusParam, wk->param->mystatus );
//	TODO
	
//	wk->proc = GFL_PROC_Create( &PokeStatusProcData, &wk->statusParam, HEAPID_WORLDTRADE );
//	TODO

	wk->subprocflag = 1;

	return SEQ_FADEIN;
}
//==============================================================================
/**
 * @brief   世界交換入り口画面メイン
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
int WorldTrade_Status_Main(WORLDTRADE_WORK *wk, int seq)
{
	int ret=SEQ_MAIN;

//		if( ProcMain( wk->proc ) ){
		if(1){	
			//GFL_PROC_Delete( wk->proc );
			//TODO

			WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, wk->sub_process_mode );
			
			ret = SEQ_FADEOUT;
		}

	return ret;
}


//==============================================================================
/**
 * @brief   世界交換入り口画面終了
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
int WorldTrade_Status_End(WORLDTRADE_WORK *wk, int seq)
{
//	FreeWork( wk );

	// ボックス画面に戻る
	WorldTrade_SubProcessUpdate( wk );

	return SEQ_INIT;
}
