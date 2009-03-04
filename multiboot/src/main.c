//===================================================================
/**
 * @file	main.c
 * @brief	アプリケーション　サンプルスケルトン
 * @author	GAME FREAK Inc.
 * @date	06.11.08
 *
 * $LastChangedDate$
 * $Id$
 */
//===================================================================
#include <nitro.h>
#include <nnsys.h>
#include "gflib.h"
#include "system/machine_use.h"
#include "system/gfl_use.h"
#include "system/main.h"

#include "dl_child/dlplay_child_main.h"

static	void	SkeltonHBlankFunc(void);
static	void	SkeltonVBlankFunc(void);
static	void	GameInit(void);
static	void	GameMain(void);

extern	void	TestModeSet(void);
//------------------------------------------------------------------
/**
 * @brief	メイン処理
 */
//------------------------------------------------------------------
void NitroMain(void)
{
	// ハード環境システム郡を初期化する関数
	MachineSystem_Init();
	// ＧＦＬ初期化
	GFLUser_Init();

	//HBLANK割り込み許可
	OS_SetIrqFunction(OS_IE_H_BLANK,SkeltonHBlankFunc);
	//VBLANK割り込み許可
	OS_SetIrqFunction(OS_IE_V_BLANK,SkeltonVBlankFunc);

	(void)OS_EnableIrqMask(OS_IE_H_BLANK|OS_IE_V_BLANK);
	(void)OS_EnableIrq();

	(void)GX_HBlankIntr(TRUE);
	(void)GX_VBlankIntr(TRUE);
	(void)OS_EnableInterrupts();

	// 必要なTCBとか登録して…
	GameInit();

	while(TRUE){
		MachineSystem_Main();
		// メイン処理して…
		GFLUser_Main();
		GameMain();

		// 描画に必要な準備して…
		GFLUser_Display();

		// レンダリングエンジンが参照するデータ群をスワップ
		// ※gflibに適切な関数が出来たら置き換えてください
		//G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_Z);

		// VBLANK待ち
		// ※gflibに適切な関数が出来たら置き換えてください
		OS_WaitIrq(TRUE,OS_IE_V_BLANK);
	}
}

//------------------------------------------------------------------
/**
 * brief	HBlank割り込み処理
 */
//------------------------------------------------------------------
static	void	SkeltonHBlankFunc(void)
{
	GFLUser_HIntr();
}

//------------------------------------------------------------------
/**
 * brief	VBlank割り込み処理
 */
//------------------------------------------------------------------
static	void	SkeltonVBlankFunc(void)
{
	OS_SetIrqCheckFlag(OS_IE_V_BLANK);

	MI_WaitDma(GX_DEFAULT_DMAID);

	MachineSystem_VIntr();
	GFLUser_VIntr();
}


#include "src/ohno/dl_fatal_error.h"
//------------------------------------------------------------------
/**
 * @brief		ゲームごとの初期化処理
 */
//------------------------------------------------------------------
static	void	GameInit(void)
{
	/* ユーザーレベルで必要な初期化をここに記述する */

    // 通信ブート処理 VBlank割り込み後に行うためここに記述
    GFL_NET_Boot( GFL_HEAPID_APP, FatalError_Disp, GFL_HEAPID_APP, GFL_HEAPID_APP );
	/* 起動プロセスの設定 */
	DLPlayChild_SetProc();
}


//------------------------------------------------------------------
/**
 * @brief		ゲームごとのメイン処理
 */
//------------------------------------------------------------------
static	void	GameMain(void)
{
	/* ユーザーレベルで必要なメイン処理をここに記述する */

}


