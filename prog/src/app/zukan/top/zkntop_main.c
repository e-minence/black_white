//============================================================================================
/**
 * @file		zkntop_main.c
 * @brief		�}�Ӄg�b�v��ʁ@���C������
 * @author	Hiroyuki Nakamura
 * @date		10.02.20
 *
 *	���W���[�����FZKNTOPMAIN
 */
//============================================================================================
#include <gflib.h>

#include "arc_def.h"
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/wipe.h"
#include "sound/pm_sndsys.h"
#include "savedata/zukan_wp_savedata.h"

#include "../zukan_common.h"
#include "zkntop_main.h"
#include "zukan_gra.naix"


//============================================================================================
//	�萔��`
//============================================================================================

// ���C���V�[�P���X
enum {
	MAINSEQ_INIT = 0,
	MAINSEQ_RELEASE,
	MAINSEQ_WIPE_IN,
	MAINSEQ_WIPE_OUT,
	MAINSEQ_MAIN,
	MAINSEQ_DEMO,
	MAINSEQ_END
};

typedef int (*pZKNTOP_FUNC)(ZKNTOPMAIN_WORK*);

#define	HEAPID_ZUKAN_TOP_L	( GFL_HEAP_LOWID(HEAPID_ZUKAN_TOP) )

//#define	BG_PALNUM_FRAME		( 1 )
//#define	BG_PALNUM_GRAPHIC	( 2 )

#define	AUTO_START_TIME		( 60*5 )


static int MainSeq_Init( ZKNTOPMAIN_WORK * wk );
static int MainSeq_Release( ZKNTOPMAIN_WORK * wk );
static int MainSeq_WipeIn( ZKNTOPMAIN_WORK * wk );
static int MainSeq_WipeOut( ZKNTOPMAIN_WORK * wk );
static int MainSeq_Main( ZKNTOPMAIN_WORK * wk );
static int MainSeq_Demo( ZKNTOPMAIN_WORK * wk );

static void ScaleAnimeFrameBG(void);


static const pZKNTOP_FUNC	MainSeq[] = {
	MainSeq_Init,
	MainSeq_Release,
	MainSeq_WipeIn,
	MainSeq_WipeOut,
	MainSeq_Main,
	MainSeq_Demo,
};



BOOL ZKNTOPMAIN_MainSeq( ZKNTOPMAIN_WORK * wk )
{
	wk->mainSeq = MainSeq[wk->mainSeq]( wk );
	if( wk->mainSeq == MAINSEQ_END ){
		return FALSE;
	}

/*
	ZKNLISTOBJ_AnmMain( wk );
	ZKNLISTBMP_PrintUtilTrans( wk );
//	BGWINFRM_MoveMain( wk->wfrm );
	ZKNCOMM_ScrollBaseBG( GFL_BG_FRAME3_M, GFL_BG_FRAME3_S, &wk->BaseScroll );
*/

	return TRUE;
}


static void InitVram(void)
{
	const GFL_DISP_VRAM tbl = {
		GX_VRAM_BG_128_A,							// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_0123_E,			// ���C��2D�G���W����BG�g���p���b�g

		GX_VRAM_SUB_BG_128_C,					// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_0123_H,	// �T�u2D�G���W����BG�g���p���b�g

		GX_VRAM_OBJ_128_B,						// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,			// ���C��2D�G���W����OBJ�g���p���b�g

		GX_VRAM_SUB_OBJ_128_D,					// �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,		// �T�u2D�G���W����OBJ�g���p���b�g

		GX_VRAM_TEX_NONE,							// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_NONE,					// �e�N�X�`���p���b�g�X���b�g

		GX_OBJVRAMMODE_CHAR_1D_128K,	// ���C��OBJ�}�b�s���O���[�h
		GX_OBJVRAMMODE_CHAR_1D_128K		// �T�uOBJ�}�b�s���O���[�h
	};

	GFL_DISP_ClearVRAM( NULL );
	GFL_DISP_SetBank( &tbl );
}

static void VBlankTask( GFL_TCB * tcb, void * work )
{
//	ZKNTOPMAIN_WORK * wk = work;

	GFL_BG_VBlankFunc();
//	GFL_CLACT_SYS_VBlankFunc();

//	PaletteFadeTrans( wk->pfd );

	OS_SetIrqCheckFlag( OS_IE_V_BLANK );
}

static void InitVBlank( ZKNTOPMAIN_WORK * wk )
{
	wk->vtask = GFUser_VIntr_CreateTCB( VBlankTask, wk, 0 );
}

static void ExitVBlank( ZKNTOPMAIN_WORK * wk )
{
	GFL_TCB_DeleteTask( wk->vtask );
}


static void InitBg(void)
{
	GFL_BG_Init( HEAPID_ZUKAN_TOP );

	{	// BG SYSTEM
		GFL_BG_SYS_HEADER sysh = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_3, GX_BGMODE_3, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &sysh );
	}

	{	// ���C����ʁF�t���[��
		GFL_BG_BGCNT_HEADER cnth = {
			0, 29*8, 0x2000, 0, GFL_BG_SCRSIZ_512x512, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x10000, 0x10000,
			GX_BG_EXTPLTT_23, 2, 1, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &cnth, GFL_BG_MODE_256X16 );
		GFL_BG_ClearScreen( GFL_BG_FRAME3_M );
	}
	{	// ���C����ʁF�J�X�^���O���t�B�b�N
		GFL_BG_BGCNT_HEADER cnth = {
			0, 0, 0x1000, 0, GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd000, GX_BG_CHARBASE_0x00000, 0x8000,
			GX_BG_EXTPLTT_01, 1, 1, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME1_M );
	}
	{	// ���C����ʁF
		GFL_BG_BGCNT_HEADER cnth = {
			0, 0, 0x1000, 0, GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xc000, GX_BG_CHARBASE_0x10000, 0x10000,
			GX_BG_EXTPLTT_23, 0, 1, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME2_M );
	}

	{	// �T�u��ʁF�w�i
		GFL_BG_BGCNT_HEADER cnth = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x10000, 0x10000,
			GX_BG_EXTPLTT_01, 1, 1, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_S, &cnth, GFL_BG_MODE_256X16 );
		GFL_BG_ClearScreen( GFL_BG_FRAME3_S );
	}
	{	// �T�u��ʁF�t���[��
		GFL_BG_BGCNT_HEADER cnth = {
			0, 3*8, 0x1000, 0, GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_23, 0, 1, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_S, &cnth, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME1_S );
	}

	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG1 | GX_PLANEMASK_BG3, VISIBLE_ON );
}

static void ExitBg(void)
{
	GFL_DISP_GX_SetVisibleControl(
		GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_BG3, VISIBLE_OFF );
	GFL_DISP_GXS_SetVisibleControl(
		GX_PLANEMASK_BG1 | GX_PLANEMASK_BG3, VISIBLE_OFF );

	GFL_BG_FreeBGControl( GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_S );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );

	GFL_BG_Exit();
}

static void LoadBgGraphic( ZKNTOPMAIN_WORK * wk )
{
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_TOP_L );

	if( MyStatus_GetMySex( wk->dat->mystatus ) == PTL_SEX_MALE ){
		GFL_ARCHDL_UTIL_TransVramPalette(
			ah, NARC_zukan_gra_top_zkn_top_bg_NCLR, PALTYPE_MAIN_BG_EX, 0x4000, 0, HEAPID_ZUKAN_TOP );
		GFL_ARCHDL_UTIL_TransVramPalette(
			ah, NARC_zukan_gra_top_zkn_top_bg_NCLR, PALTYPE_MAIN_BG_EX, 0x6000, 0, HEAPID_ZUKAN_TOP );
		GFL_ARCHDL_UTIL_TransVramPalette(
			ah, NARC_zukan_gra_top_zkn_top_bg_NCLR, PALTYPE_SUB_BG_EX, 0x4000, 0, HEAPID_ZUKAN_TOP );
		GFL_ARCHDL_UTIL_TransVramPalette(
			ah, NARC_zukan_gra_top_zkn_top_bg_NCLR, PALTYPE_SUB_BG_EX, 0x6000, 0, HEAPID_ZUKAN_TOP );
	}else{
		GFL_ARCHDL_UTIL_TransVramPalette(
			ah, NARC_zukan_gra_top_zkn_top_bg2_NCLR, PALTYPE_MAIN_BG_EX, 0x4000, 0, HEAPID_ZUKAN_TOP );
		GFL_ARCHDL_UTIL_TransVramPalette(
			ah, NARC_zukan_gra_top_zkn_top_bg2_NCLR, PALTYPE_MAIN_BG_EX, 0x6000, 0, HEAPID_ZUKAN_TOP );
		GFL_ARCHDL_UTIL_TransVramPalette(
			ah, NARC_zukan_gra_top_zkn_top_bg2_NCLR, PALTYPE_SUB_BG_EX, 0x4000, 0, HEAPID_ZUKAN_TOP );
		GFL_ARCHDL_UTIL_TransVramPalette(
			ah, NARC_zukan_gra_top_zkn_top_bg2_NCLR, PALTYPE_SUB_BG_EX, 0x6000, 0, HEAPID_ZUKAN_TOP );
	}

	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_top_zkn_top_cover_NCGR, GFL_BG_FRAME2_M, 0, 0, FALSE, HEAPID_ZUKAN_TOP );
	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_top_zkn_top_cover_NCGR, GFL_BG_FRAME1_S, 0, 0, FALSE, HEAPID_ZUKAN_TOP );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_top_zkn_top_cover_NSCR,
		GFL_BG_FRAME3_M, 0, 0, FALSE, HEAPID_ZUKAN_TOP );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_top_zkn_top_pict_NSCR,
		GFL_BG_FRAME1_M, 0, 0, FALSE, HEAPID_ZUKAN_TOP );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_top_zkn_top_cover_up_NSCR,
		GFL_BG_FRAME2_M, 0, 0, FALSE, HEAPID_ZUKAN_TOP );

	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_top_zkn_top_sub_bg_NSCR,
		GFL_BG_FRAME3_S, 0, 0, FALSE, HEAPID_ZUKAN_TOP );
	GFL_ARCHDL_UTIL_TransVramScreen(
		ah, NARC_zukan_gra_top_zkn_top_cover_u_NSCR,
		GFL_BG_FRAME1_S, 0, 0, FALSE, HEAPID_ZUKAN_TOP );

	GFL_ARC_CloseDataHandle( ah );
}

static void LoadSaveGraphic( ZKNTOPMAIN_WORK * wk )
{
/*
	ARCHANDLE * ah = GFL_ARC_OpenDataHandle( ARCID_ZUKAN_GRA, HEAPID_ZUKAN_TOP_L );

	GFL_ARCHDL_UTIL_TransVramBgCharacter(
		ah, NARC_zukan_gra_top_zkn_top_dummy_NCGR, GFL_BG_FRAME1_M, 0, 0, FALSE, HEAPID_ZUKAN_TOP );

	GFL_ARC_CloseDataHandle( ah );
*/
	SAVE_CONTROL_WORK * sv = GAMEDATA_GetSaveControlWork( wk->dat->gamedata );

	// �O���Z�[�u�f�[�^�ǂݍ���
	if( SaveControl_Extra_Load( sv, SAVE_EXTRA_ID_ZUKAN_WALLPAPER, HEAPID_ZUKAN_TOP_L ) == LOAD_RESULT_OK ){
		ZUKANWP_SAVEDATA * exsv = SaveControl_Extra_DataPtrGet( sv, SAVE_EXTRA_ID_ZUKAN_WALLPAPER, 0 );

		if( exsv != NULL ){
			if( ZUKANWP_SAVEDATA_GetDataCheckFlag( exsv ) == TRUE ){
				u8 * chr;
				u16 * pal;

				chr = ZUKANWP_SAVEDATA_GetCustomGraphicCharacter( exsv );
				pal = ZUKANWP_SAVEDATA_GetCustomGraphicPalette( exsv );
				GFL_BG_LoadCharacter( GFL_BG_FRAME1_M, chr, ZUKANWP_SAVEDATA_CHAR_SIZE, 0 );
				GFL_BG_LoadPalette(
					GFL_BG_FRAME1_M, pal, ZUKANWP_SAVEDATA_PAL_SIZE*2, 0 );
				GFL_BG_LoadPalette(
					GFL_BG_FRAME1_S, pal, ZUKANWP_SAVEDATA_PAL_SIZE*2, 0 );

				// �g���p���b�g�ɂ��ǂݍ���
	      DC_FlushRange( (void*)pal, ZUKANWP_SAVEDATA_PAL_SIZE*2 );
        GX_BeginLoadBGExtPltt();
        GX_LoadBGExtPltt( (const void *)pal, 0x4000, ZUKANWP_SAVEDATA_PAL_SIZE*2 );
        GX_LoadBGExtPltt( (const void *)pal, 0x6000, ZUKANWP_SAVEDATA_PAL_SIZE*2 );
        GX_EndLoadBGExtPltt();
        GXS_BeginLoadBGExtPltt();
        GXS_LoadBGExtPltt( (const void *)pal, 0x4000, ZUKANWP_SAVEDATA_PAL_SIZE*2 );
        GXS_LoadBGExtPltt( (const void *)pal, 0x6000, ZUKANWP_SAVEDATA_PAL_SIZE*2 );
        GXS_EndLoadBGExtPltt();
			}
		}
	}

	SaveControl_Extra_Unload( sv, SAVE_EXTRA_ID_ZUKAN_WALLPAPER );
}










static int MainSeq_Init( ZKNTOPMAIN_WORK * wk )
{
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );
	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �T�u��ʂ����C����
	GFL_DISP_SetDispSelect( GFL_DISP_3D_TO_SUB );

	InitVram();
	InitBg();

	LoadBgGraphic( wk );
	LoadSaveGraphic( wk );

	GFL_NET_WirelessIconEasy_HoldLCD( TRUE, HEAPID_ZUKAN_TOP );
	GFL_NET_ReloadIcon();
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );		// SUB DISP OBJ ON

	InitVBlank( wk );

	ZKNCOMM_SetFadeIn( HEAPID_ZUKAN_TOP );
	return MAINSEQ_WIPE_IN;
}

static int MainSeq_Release( ZKNTOPMAIN_WORK * wk )
{
	ExitVBlank( wk );

	ExitBg();

	// �u�����h������
	G2_BlendNone();
	G2S_BlendNone();
	// �\��������
	GFL_DISP_GX_SetVisibleControlDirect( 0 );
	GFL_DISP_GXS_SetVisibleControlDirect( 0 );

	return MAINSEQ_END;
}

static int MainSeq_WipeIn( ZKNTOPMAIN_WORK * wk )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		return MAINSEQ_MAIN;
	}
	return MAINSEQ_WIPE_IN;
}

static int MainSeq_WipeOut( ZKNTOPMAIN_WORK * wk )
{
	if( WIPE_SYS_EndCheck() == TRUE ){
		return MAINSEQ_RELEASE;
	}
	return MAINSEQ_WIPE_OUT;
}

static int MainSeq_Main( ZKNTOPMAIN_WORK * wk )
{
	if( wk->time == AUTO_START_TIME ){
		return MAINSEQ_DEMO;
	}
	wk->time++;

	if( GFL_UI_TP_GetCont() == TRUE ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
		return MAINSEQ_DEMO;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		return MAINSEQ_DEMO;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		wk->dat->retMode = ZKNTOP_RET_EXIT;
		ZKNCOMM_SetFadeOut( HEAPID_ZUKAN_TOP );
		return MAINSEQ_WIPE_OUT;
	}

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X ){
		GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
		wk->dat->retMode = ZKNTOP_RET_EXIT_X;
		ZKNCOMM_SetFadeOut( HEAPID_ZUKAN_TOP );
		return MAINSEQ_WIPE_OUT;
	}

	return MAINSEQ_MAIN;
}

#define	DEMO_SCROLL_SPEED				( 8 )
#define	DEMO_SCROLL_COUNT				( 208/DEMO_SCROLL_SPEED )
#define	DEMO_SCROLL_END_WAIT		( 32 )


static int MainSeq_Demo( ZKNTOPMAIN_WORK * wk )
{
	switch( wk->demoSeq ){
	case 0:
		if( wk->demoCnt == DEMO_SCROLL_COUNT ){
			PMSND_PlaySE( SEQ_SE_SYS_76 );
			wk->demoCnt = 0;
			wk->demoSeq++;
			break;
		}
		GFL_BG_SetScrollReq( GFL_BG_FRAME3_M, GFL_BG_SCROLL_Y_DEC, DEMO_SCROLL_SPEED );
		GFL_BG_SetScrollReq( GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_DEC, DEMO_SCROLL_SPEED );
		GFL_BG_SetScrollReq( GFL_BG_FRAME2_M, GFL_BG_SCROLL_Y_DEC, DEMO_SCROLL_SPEED );
		GFL_BG_SetScrollReq( GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_DEC, DEMO_SCROLL_SPEED );
		wk->demoCnt++;
		break;

	case 1:
		if( wk->demoCnt == DEMO_SCROLL_END_WAIT ){
			PMSND_PlaySE( SEQ_SE_SYS_77 );
			GFL_BG_SetRotateCenterReq( GFL_BG_FRAME3_M, GFL_BG_CENTER_X_SET, 128 );
			GFL_BG_SetRotateCenterReq( GFL_BG_FRAME3_M, GFL_BG_CENTER_Y_SET, 120 );
			GFL_BG_SetRotateCenterReq( GFL_BG_FRAME3_S, GFL_BG_CENTER_X_SET, 128 );
			GFL_BG_SetRotateCenterReq( GFL_BG_FRAME3_S, GFL_BG_CENTER_Y_SET, 96 );
			wk->demoCnt = 0;
			wk->demoSeq++;
			break;
		}
		wk->demoCnt++;
		break;

	case 2:
		ScaleAnimeFrameBG();
		if( wk->demoCnt == 4 ){
			WIPE_SYS_Start(
				WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT,
				WIPE_FADE_BLACK, WIPE_DEF_DIV, WIPE_DEF_SYNC*4, HEAPID_ZUKAN_TOP );
			wk->demoCnt = 0;
			wk->demoSeq++;
		}else{
			wk->demoCnt++;
		}
		break;

	case 3:
		ScaleAnimeFrameBG();
		if( WIPE_SYS_EndCheck() == TRUE ){
			wk->dat->retMode = ZKNTOP_RET_LIST;
			return MAINSEQ_RELEASE;
		}
		break;
	}

	return MAINSEQ_DEMO;
}

static void ScaleAnimeFrameBG(void)
{
	GFL_BG_SetScaleReq( GFL_BG_FRAME3_M, GFL_BG_SCALE_X_INC, FX32_ONE/32 );
	GFL_BG_SetScaleReq( GFL_BG_FRAME3_M, GFL_BG_SCALE_Y_INC, FX32_ONE/32 );
	GFL_BG_SetScaleReq( GFL_BG_FRAME3_S, GFL_BG_SCALE_X_INC, FX32_ONE/32 );
	GFL_BG_SetScaleReq( GFL_BG_FRAME3_S, GFL_BG_SCALE_Y_INC, FX32_ONE/32 );
}
