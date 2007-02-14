
//============================================================================================
/**
 * @file	title.c
 * @brief	DS版ヨッシーのたまご　タイトルプログラム
 * @author	sogabe
 * @date	2007.02.06
 */
//============================================================================================


#include "gflib.h"
#include "procsys.h"
#include "ui.h"

#include "main.h"
#include "yt_common.h"

#include "sample_graphic/yossyegg.naix"

#define __TITLE_H_GLOBAL__
#include "title.h"

//----------------------------------------------------------------------------
/**
 *	@brief	タイトル初期化
 *	
 *	@param	gp		ゲームパラメータポインタ
 */
//-----------------------------------------------------------------------------
void	YT_InitTitle(GAME_PARAM *gp)
{
	//BGシステム初期化
	GFL_BG_sysInit(gp->heapID);

	//VRAM設定
	{
		GFL_BG_DISPVRAM vramSetTable = {
			GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
			GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
			GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
			GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
			GX_VRAM_OBJ_64_E,				// メイン2DエンジンのOBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
			GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
			GX_VRAM_TEX_0_B,				// テクスチャイメージスロット
			GX_VRAM_TEXPLTT_0_F				// テクスチャパレットスロット
		};
		GFL_DISP_SetBank( &vramSetTable );

		//VRAMクリア
		MI_CpuClear32((void*)HW_BG_VRAM, HW_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_BG_VRAM, HW_DB_BG_VRAM_SIZE);
		MI_CpuClear32((void*)HW_OBJ_VRAM, HW_OBJ_VRAM_SIZE);
		MI_CpuClear32((void*)HW_DB_OBJ_VRAM, HW_DB_OBJ_VRAM_SIZE);
	}

	// BG SYSTEM
	{
		GFL_BG_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_InitBG( &BGsys_data );
	}

	//メイン画面フレーム設定
	{
		GFL_BG_BGCNT_HEADER TextBgCntDat[] = {
			///<FRAME2_M
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME3_M
			{
				0, 0, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME2_S
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
			///<FRAME3_S
			{
				0, 0, 0x0800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
				GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
				GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
			},
		};
		GFL_BG_BGControlSet(GFL_BG_FRAME2_M, &TextBgCntDat[0], GFL_BG_MODE_TEXT );
		GFL_BG_ScrClear(GFL_BG_FRAME2_M );
		GFL_BG_BGControlSet(GFL_BG_FRAME3_M, &TextBgCntDat[1], GFL_BG_MODE_TEXT );
		GFL_BG_ScrClear(GFL_BG_FRAME3_M );
		GFL_BG_BGControlSet(GFL_BG_FRAME2_S, &TextBgCntDat[2], GFL_BG_MODE_TEXT );
		GFL_BG_ScrClear(GFL_BG_FRAME2_S );
		GFL_BG_BGControlSet(GFL_BG_FRAME3_S, &TextBgCntDat[3], GFL_BG_MODE_TEXT );
		GFL_BG_ScrClear(GFL_BG_FRAME3_S );

		GFL_DISP_GX_VisibleControl(GX_PLANEMASK_BG0, VISIBLE_ON );

		// OBJマッピングモード
		GX_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );
		GXS_SetOBJVRamModeChar( GX_OBJVRAMMODE_CHAR_1D_32K );

		GFL_DISP_GX_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
		GFL_DISP_GXS_VisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	}

	//画面生成
	GFL_ARC_UtilBgCharSet(0,NARC_yossyegg_TITLE_V2_NCGR,GFL_BG_FRAME2_M,0,0,0,gp->heapID);
	GFL_ARC_UtilScrnSet(0,NARC_yossyegg_TITLE_V2_NSCR,GFL_BG_FRAME2_M,0,0,0,gp->heapID);
	GFL_ARC_UtilBgCharSet(0,NARC_yossyegg_STAGESEL_NCGR,GFL_BG_FRAME3_M,0,0,0,gp->heapID);
	GFL_ARC_UtilScrnSet(0,NARC_yossyegg_STAGESEL_NSCR,GFL_BG_FRAME3_M,0,0,0,gp->heapID);
	GFL_ARC_UtilPalSet(0,NARC_yossyegg_TITLE_BG_NCLR,PALTYPE_MAIN_BG,0,0x100,gp->heapID);

	GFL_DISP_DispOn();
	GFL_DISP_DispSelect( GFL_DISP_3D_TO_SUB );

	GFL_FADE_MasterBrightReq(GFL_FADE_MASTER_BRIGHT_WHITEOUT_MAIN|GFL_FADE_MASTER_BRIGHT_WHITEOUT_SUB,16,0,2);

	YT_JobNoSet(gp,YT_MainTitleNo);
}

//----------------------------------------------------------------------------
/**
 *	@brief	タイトルメイン処理
 *	
 *	@param	gp		ゲームパラメータポインタ
 */
//-----------------------------------------------------------------------------
enum{
	YT_SEQ_TITLE_KEY_WAIT=0,
	YT_SEQ_TITLE_END,
};
void	YT_MainTitle(GAME_PARAM *gp)
{
	switch(gp->seq_no){
	case YT_SEQ_TITLE_KEY_WAIT:
		if( GFL_UI_KeyGetTrg() & PAD_BUTTON_START){
			GFL_FADE_MasterBrightReq(GFL_FADE_MASTER_BRIGHT_BLACKOUT_MAIN|GFL_FADE_MASTER_BRIGHT_BLACKOUT_SUB,0,16,2);
			gp->seq_no++;
		}
		break;
	case YT_SEQ_TITLE_END:
		if(GFL_FADE_FadeCheck()==FALSE){
			GFL_BG_sysExit();
			YT_JobNoSet(gp,YT_InitGameNo);
		}
	}
}

