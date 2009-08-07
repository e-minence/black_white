//============================================================================================
/**
 * @file	worldtrade_title.c
 * @brief	���E�����^�C�g����ʏ���
 * @author	Akito Mori
 * @date	06.04.16
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
//#include "savedata/zukanwork.h"
//TODO

#include "net_app/worldtrade.h"
#include "worldtrade_local.h"

#include "msg/msg_wifi_lobby.h"
#include "msg/msg_wifi_gts.h"


#include "worldtrade.naix"			// �O���t�B�b�N�A�[�J�C�u��`

#define SEVERNO_MESSAGE_WAIT	( 45 )

//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
/*** �֐��v���g�^�C�v ***/
static void BgInit( void );
static void BgExit( void );
static void BgGraphicSet( WORLDTRADE_WORK * wk );
static void SetCellActor(WORLDTRADE_WORK *wk);
static void DelCellActor( WORLDTRADE_WORK *wk );
static void BmpWinInit( WORLDTRADE_WORK *wk );
static void BmpWinDelete( WORLDTRADE_WORK *wk );
static void InitWork( WORLDTRADE_WORK *wk );
static void FreeWork( WORLDTRADE_WORK *wk );
static int SubSeq_Opening( WORLDTRADE_WORK *wk);
static int SubSeq_OpeningMessage( WORLDTRADE_WORK *wk );
static int SubSeq_OpeningFade( WORLDTRADE_WORK *wk );
static int SubSeq_OpeningFadeWait( WORLDTRADE_WORK *wk );
static int SubSeq_OpeningWait( WORLDTRADE_WORK *wk);
static int SubSeq_Start( WORLDTRADE_WORK *wk);
static int SubSeq_Main( WORLDTRADE_WORK *wk);
static int SubSeq_End( WORLDTRADE_WORK *wk);
static int SubSeq_YesNo( WORLDTRADE_WORK *wk);
static int SubSeq_YesNoSelect( WORLDTRADE_WORK *wk);
static int SubSeq_MessageWait( WORLDTRADE_WORK *wk );
static int SubSeq_Message1MinWait( WORLDTRADE_WORK *wk );
static int SubSeq_EndDemo( WORLDTRADE_WORK *wk );
static int SubSeq_EndDemoWait( WORLDTRADE_WORK *wk );

static void TitleMenuPrint( WORLDTRADE_WORK *wk );
static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat );
static void SubSeq_TalkPrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat );
static void DemoBgSet( WORLDTRADE_WORK *wk );
static  u32 TouchPanelFunc( WORLDTRADE_WORK *wk );
static void DecideFunc( WORLDTRADE_WORK *wk, int decide );


enum{
	SUBSEQ_OPENING=0,
	SUBSEQ_OPENING_MESSAGE,
	SUBSEQ_OPENING_FADE,
	SUBSEQ_OPENING_FADE_WAIT,
	SUBSEQ_OPENING_WAIT,
	SUBSEQ_START,
	SUBSEQ_MAIN,
	SUBSEQ_END_DEMO,
	SUBSEQ_END_DEMO_WAIT,
	SUBSEQ_END,
	SUBSEQ_MES_WAIT,
	SUBSEQ_MES_1MIN_WAIT,
	SUBSEQ_YESNO,
	SUBSEQ_YESNO_SELECT,
};

static int (*Functable[])( WORLDTRADE_WORK *wk ) = {
	SubSeq_Opening,				// SUBSEQ_OPENING=0,
	SubSeq_OpeningMessage,      // SUBSEQ_OPENING_MESSAGE,
	SubSeq_OpeningFade,         // SUBSEQ_OPENING_FADE,
	SubSeq_OpeningFadeWait,     // SUBSEQ_OPENING_FADE_WAIT,
	SubSeq_OpeningWait,         // SUBSEQ_OPENING_WAIT,
	SubSeq_Start,			    // SUBSEQ_START,
	SubSeq_Main,                // SUBSEQ_MAIN,
	SubSeq_EndDemo,             // SUBSEQ_END_DEMO,
	SubSeq_EndDemoWait,         // SUBSEQ_END_DEMO_WAIT,
	SubSeq_End,                 // SUBSEQ_END,
	SubSeq_MessageWait,         // SUBSEQ_MES_WAIT,
	SubSeq_Message1MinWait,     // SUBSEQ_MES_1MIN_WAIT,
	SubSeq_YesNo,			    // SUBSEQ_YESNO,
	SubSeq_YesNoSelect,		    // SUBSEQ_YESNO_SELECT,
	

};

//============================================================================================
//	�v���Z�X�֐�
//============================================================================================

//==============================================================================
/**
 * @brief   ���E�����������ʏ�����
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
int WorldTrade_Title_Init(WORLDTRADE_WORK *wk, int seq)
{
	// ���[�N������
	InitWork( wk );
	
	// ���C����ʂ����ɏo�͂����悤�ɂ���
	GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
	
	// BG�ݒ�
	BgInit();

	// BG�O���t�B�b�N�]��
	BgGraphicSet( wk );

	// BMPWIN�m��
	BmpWinInit( wk );

	SetCellActor(wk);

	// �ʐM��Ԃ��m�F���ăA�C�R���̕\����ς���
    WorldTrade_WifiIconAdd( wk );

	// ���j���[�̍��ڂ�`��
	TitleMenuPrint( wk );

	// �T�E���h�f�[�^���[�h(�t�B�[���h)
	//Snd_DataSetByScene( SND_SCENE_FIELD, SEQ_BLD_BLD_GTC, 1 );



	//���͖��֌W
	//Snd_DataSetByScene( SND_SCENE_WIFI_WORLD_TRADE, SEQ_GS_BLD_GTC, 1 );
	//TODO


	// �v���`�i�ŁA�}�b�v�̋ȂƁA��ʂ̋Ȃ��ύX�ɂȂ����̂ŁA
	// ��ʂ���A�t�B�[���h�ɖ߂鎞�̂��߂ɃV�[����ύX

	if(wk->OpeningFlag==0){
		// ��l���o��f���p�̏��ʐݒ�
		DemoBgSet( wk );


		// �ŏ��̂P���
		// ���C�v�t�F�[�h�J�n�i�T�u��ʂ����j
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
		OS_Printf( "******************** worldtrade_title.c [159] MS ********************\n" );
#endif

		wk->subprocess_seq = SUBSEQ_OPENING;
		wk->OpeningFlag     = 1;

		// ����ʂɎ�l����o�ꂳ����
		WorldTrade_HeroDemo( wk, MyStatus_GetMySex(wk->param->mystatus) );
	}else{
		// �Q��ڈȍ~
		// ���C�v�t�F�[�h�J�n�i����ʁj
		if( wk->sub_out_flg == 1 && GXS_GetMasterBrightness() != 0 ){
            WorldTrade_SetPartnerExchangePos(wk);
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
			OS_Printf( "******************** worldtrade_title.c [171] MS ********************\n" );
#endif
		}else{
			WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
			OS_Printf( "******************** worldtrade_title.c [175] M ********************\n" );
#endif
		}
		wk->subprocess_seq = SUBSEQ_START;
	}
	wk->sub_out_flg = 0;
	OS_TPrintf("opening flag = %d\n", wk->OpeningFlag);
	return SEQ_FADEIN;
}
//==============================================================================
/**
 * @brief   ���E�����������ʃ��C��
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
int WorldTrade_Title_Main(WORLDTRADE_WORK *wk, int seq)
{
	int ret;
	
	// �ʐM��Ԃ��m�F���ăA�C�R���̕\����ς���
    WirelessIconEasy_SetLevel(WorldTrade_WifiLinkLevel());

	ret = (*Functable[wk->subprocess_seq])( wk );

	return ret;
}


//==============================================================================
/**
 * @brief   ���E�����������ʏI��
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
int WorldTrade_Title_End(WORLDTRADE_WORK *wk, int seq)
{
	WirelessIconEasyEnd();

	DelCellActor(wk);

	FreeWork( wk );
	
	BmpWinDelete( wk );
	
	BgExit();

	// �uDS�̉���ʂ��݂Ă˃A�C�R���v��\��
	GFL_CLACT_WK_SetDrawEnable( wk->PromptDsActWork, 0 );
	
	WorldTrade_SubProcessUpdate( wk );
	
	return SEQ_INIT;
}


//--------------------------------------------------------------------------------------------
/**
 * BG�ݒ�
 *
 * @param	ini		BGL�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgInit( void )
{

	// BG SYSTEM
	{	
		GFL_BG_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
		GFL_BG_SetBGMode( &BGsys_data );
	}

	// ���C����ʃe�L�X�g��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl(  GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(  GFL_BG_FRAME0_M );
	}

	// ���C����ʃ��j���[��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl(  GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
	}

	// ���C����ʔw�i��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl(  GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
	}

	// �T�u��ʏ�����
	WorldTrade_SubLcdBgInit(  0, 0 );



	GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 32, 0, HEAPID_WORLDTRADE );

	GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );	//���C�����OBJ�ʂn�m
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );	//�T�u���OBJ�ʂn�m

}

//==============================================================================
/**
 * @brief   �T�u��ʏ�����(�ł�����苤�ʏ����Ƃ��ăT�u��ʂ�BG���������s���j
 *
 * @param   ini		GF_BGL_INI
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_SubLcdBgInit( int sub_bg1_y_offset, BOOL sub_bg2_no_clear )
{
	// �T�u��ʕ�����0
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl(  GFL_BG_FRAME0_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(  GFL_BG_FRAME0_S );

	}

	// �T�u��ʃv���[�g��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x14000, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl(  GFL_BG_FRAME2_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
		if(sub_bg2_no_clear == FALSE){
    		GFL_BG_ClearScreen(  GFL_BG_FRAME2_S );
    	}

	}


	// �T�u��ʔw�iBG1( ���̖ʂ�256�F )
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_256,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
		};
		TextBgCntDat.scrollY = sub_bg1_y_offset;
		GFL_BG_SetBGControl(  GFL_BG_FRAME1_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
	}
	GFL_BG_SetClearCharacter( GFL_BG_FRAME0_S, 32, 0, HEAPID_WORLDTRADE );

	// �T�u��ʕ����ł���2
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xd800, GX_BG_CHARBASE_0x10000, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl(  GFL_BG_FRAME3_S, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(  GFL_BG_FRAME3_S );

	}
}

//==============================================================================
/**
 * @brief   �T�u���BG�����
 *
 * @param   ini		
 *
 * @retval  none		
 */
//==============================================================================
void WorldTrade_SubLcdBgExit( void )
{
	// WorldTrade_SubLcdBgInit �ŏ����������T�uBG�ʂ��������

	GFL_BG_FreeBGControl(  GFL_BG_FRAME2_S );
	GFL_BG_FreeBGControl(  GFL_BG_FRAME1_S );
	GFL_BG_FreeBGControl(  GFL_BG_FRAME0_S );
	GFL_BG_FreeBGControl(  GFL_BG_FRAME3_S );	
}


//--------------------------------------------------------------------------------------------
/**
 * BG���
 *
 * @param	ini		BGL�f�[�^
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( void )
{

	// �T�u��ʂa�f�����
	WorldTrade_SubLcdBgExit();

	// ���C����ʂa�f�����
	GFL_BG_FreeBGControl(  GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl(  GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl(  GFL_BG_FRAME0_M );
}


//------------------------------------------------------------------
/**
 * @brief   ��l���o��f�����͏��ʂ�OBJ�ȊOOFF�ɂ���
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void DemoBgSet( WORLDTRADE_WORK *wk )
{
	GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_BG0, VISIBLE_OFF );	//���C�����OBJ�ʂn�m
	GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_BG1, VISIBLE_OFF );	//���C�����OBJ�ʂn�m
	GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );	//���C�����OBJ�ʂn�m
	
	GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 0 );
}


//--------------------------------------------------------------------------------------------
/**
 * �O���t�B�b�N�f�[�^�Z�b�g
 *
 * @param	wk		�|�P�������X�g��ʂ̃��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgGraphicSet( WORLDTRADE_WORK * wk )
{
	ARCHANDLE* p_handle;

	p_handle = GFL_ARC_OpenDataHandle( ARCID_WORLDTRADE_GRA, HEAPID_WORLDTRADE );

	// ���ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette(    p_handle, NARC_worldtrade_title_nclr, PALTYPE_MAIN_BG, 0, 16*3*2,  HEAPID_WORLDTRADE);


	
	// ��b�t�H���g�p���b�g�]��
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );
   //	TalkFontPaletteLoad( PALTYPE_SUB_BG,  WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );


	// ��b�E�C���h�E�O���t�B�b�N�]��
	
	BmpWinFrame_GraphicSet(	 GFL_BG_FRAME0_M, WORLDTRADE_MESFRAME_CHR, 
						WORLDTRADE_MESFRAME_PAL,  CONFIG_GetWindowType(wk->param->config), HEAPID_WORLDTRADE );


	BmpWinFrame_GraphicSet(	 GFL_BG_FRAME0_M, WORLDTRADE_MENUFRAME_CHR,
						WORLDTRADE_MENUFRAME_PAL, 0, HEAPID_WORLDTRADE );



	// ���C�����BG1�L�����]��
	GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_worldtrade_title_lz_ncgr,  GFL_BG_FRAME1_M, 0, 16*6*0x20, 1, HEAPID_WORLDTRADE);


	// ���C�����BG1�X�N���[���]��
	GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_worldtrade_title_menu_lz_nscr,  GFL_BG_FRAME1_M, 0, 32*24*2, 1, HEAPID_WORLDTRADE);



	// ���C�����BG2�X�N���[���]��
	GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_worldtrade_title_base_lz_nscr,  GFL_BG_FRAME2_M, 0, 32*24*2, 1, HEAPID_WORLDTRADE);


	// �����256�F�w�i�]��
	WorldTrade_SubLcdBgGraphicSet( wk );

	if(wk->OpeningFlag){
		WorldTrade_SubLcdWinGraphicSet( wk );   // �g���[�h���[���E�C���h�E�]��
		//�T�uBG0��\��
		GFL_DISP_GXS_SetVisibleControl(  GX_PLANEMASK_BG0, VISIBLE_ON );	//�T�u���OBJ��ON
	}else{
		//�͂��߂̓T�uBG0���\��
		GFL_DISP_GXS_SetVisibleControl(  GX_PLANEMASK_BG0, VISIBLE_OFF );	//�T�u���OBJ��OFF
	}


	GFL_ARC_CloseDataHandle( p_handle );
}


static const u16 CursorPosTbl[][2]={
	{128,96-5*8},
	{128,96},
	{128,96+5*8},
};

#define CLACT_TITLE_CURSOR_ACTNO ( 43 ) // 1

//------------------------------------------------------------------
/**
 * �Z���A�N�^�[�o�^
 *
 * @param   wk			WORLDTRADE_WORK*
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void SetCellActor(WORLDTRADE_WORK *wk)
{
#if 0
	//�o�^���i�[
	CLACT_ADD add;
	{
		CLACT_HEADER header;
		// �Z���A�N�^�[�w�b�_�쐬
		GFL_CLACT_WK_SetCellResData or GFL_CLACT_WK_SetTrCellResData or GFL_CLACT_WK_SetMCellResData(&header, 0, 0, 0, 0, CLACT_U_HEADER_DATA_NONE, CLACT_U_HEADER_DATA_NONE,
				0, 1,
				wk->resMan[CLACT_U_CHAR_RES],
				wk->resMan[CLACT_U_PLTT_RES],
				wk->resMan[CLACT_U_CELL_RES],
				wk->resMan[CLACT_U_CELLANM_RES],
				NULL,NULL);
		WorldTrade_MakeCLACT( &add,  wk, &header, NNS_G2D_VRAM_TYPE_2DMAIN );
	}

	add.mat.x = FX32_ONE *   CursorPosTbl[wk->TitleCursorPos][0];
	add.mat.y = FX32_ONE *   CursorPosTbl[wk->TitleCursorPos][1];
	wk->CursorActWork = GFL_CLACT_WK_Create(&add);
	GFL_CLACT_WK_SetAutoAnmFlag(wk->CursorActWork,1);
	GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CLACT_TITLE_CURSOR_ACTNO );
#endif 
	//TODO
}

//------------------------------------------------------------------
/**
 * @brief   �\���Z���A�N�^�[�̉��
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void DelCellActor( WORLDTRADE_WORK *wk )
{
	GFL_CLACT_WK_Remove(wk->CursorActWork);
}

#define MENU_TEXT_X		(  9 )
#define MENU_TEXT_Y		(  6 )
#define MENU_TEXT_SX	( 15 )
#define MENU_TEXT_SY	(  2 )

#define WT_MENU_MAX_NUM	(  3 )

#define MENU_MESSAGE_OFFSET    ( 16*6+1 )	// ���ꂾ��BG1�ʂȂ̂�

#define TITLE_MESSAGE_OFFSET   ( WORLDTRADE_MENUFRAME_CHR + MENU_WIN_CGX_SIZ )
#define LINE_MESSAGE_OFFSET    ( TITLE_MESSAGE_OFFSET     + TITLE_TEXT_SX*TITLE_TEXT_SY )
#define TALK_MESSAGE_OFFSET    ( LINE_MESSAGE_OFFSET      + LINE_TEXT_SX*LINE_TEXT_SY )
#define YESNO_OFFSET 		   ( TALK_MESSAGE_OFFSET      + TALK_WIN_SX*TALK_WIN_SY )



//------------------------------------------------------------------
/**
 * BMPWIN�����i�����p�l���Ƀt�H���g�`��j
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BmpWinInit( WORLDTRADE_WORK *wk )
{
	// ---------- ���C����� ------------------

	// BG0��BMPWIN�^�C�g���E�C���h�E�m�ہE�`��
	
	wk->TitleWin	= GFL_BMPWIN_Create( GFL_BG_FRAME1_M,
	TITLE_TEXT_X, TITLE_TEXT_Y, TITLE_TEXT_SX, TITLE_TEXT_SY, WORLDTRADE_TALKFONT_PAL,  TITLE_MESSAGE_OFFSET );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->TitleWin), 0x0000 );
	
	// POKMON GLOBAL TRADING SYSTEM
	GF_STR_PrintColor(	wk->TitleWin, FONT_TALK, wk->TitleString, 
				0, 2, MSG_ALLPUT, 
				PRINTSYS_LSB_Make(15,6,0),NULL);

	// BG0��BMPWIN���j���[�e�L�X�g�E�C���h�E�m�ہE�`��
	{
		int i;
		for(i=0;i<WT_MENU_MAX_NUM;i++){
			wk->MenuWin[i]	= GFL_BMPWIN_Create( GFL_BG_FRAME1_M,
			MENU_TEXT_X, MENU_TEXT_Y+i*5, MENU_TEXT_SX, MENU_TEXT_SY, 1,  
			MENU_MESSAGE_OFFSET + (MENU_TEXT_SX*MENU_TEXT_SY)*i );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuWin[i]), 0x0000 );
		}
	}
	
	// 1�s��b�E�C���h�E
	wk->MsgWin	= GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
		LINE_TEXT_X, LINE_TEXT_Y, LINE_TEXT_SX, LINE_TEXT_SY, 
		WORLDTRADE_TALKFONT_PAL,  LINE_MESSAGE_OFFSET );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0x0f0f );


	// �ʏ��b�E�C���h�E
	wk->TalkWin	= GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
		TALK_WIN_X, TALK_WIN_Y, TALK_WIN_SX, TALK_WIN_SY, 
		WORLDTRADE_TALKFONT_PAL,  TALK_MESSAGE_OFFSET );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->TalkWin), 0x0f0f );

	// �T�u��ʂ�GTS�����pBMPWIN���m�ۂ���
	WorldTrade_SubLcdExpainPut( wk, EXPLAIN_GTS );


}	

//------------------------------------------------------------------
/**
 * @brief   �m�ۂ���BMPWIN�����
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BmpWinDelete( WORLDTRADE_WORK *wk )
{
	int i;

	GFL_BMPWIN_Delete( wk->ExplainWin );
	OS_Printf("�����E�C���h�E���\n");

	GFL_BMPWIN_Delete( wk->TalkWin );
	GFL_BMPWIN_Delete( wk->MsgWin  );

	for(i=0;i<WT_MENU_MAX_NUM;i++){
		GFL_BMPWIN_Delete( wk->MenuWin[i] );
	}

	GFL_BMPWIN_Delete( wk->TitleWin );


}

//------------------------------------------------------------------
/**
 * ���E�������[�N������
 *
 * @param   wk		WORLDTRADE_WORK*
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void InitWork( WORLDTRADE_WORK *wk )
{


	// ������o�b�t�@�쐬
	wk->TalkString  = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WORLDTRADE );

	// POKeMON GLOBAL TRADING SYSTEM
	wk->TitleString = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_02_001 );

}


//------------------------------------------------------------------
/**
 * @brief   ���[�N���
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void FreeWork( WORLDTRADE_WORK *wk )
{

	GFL_STR_DeleteBuffer( wk->TalkString ); 
	GFL_STR_DeleteBuffer( wk->TitleString ); 

}



//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//--------------------------------------------------------------------

//------------------------------------------------------------------
/**
 * @brief   �ŏ��ɐ��E�������j���[��ʂɗ����Ƃ�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_Opening( WORLDTRADE_WORK *wk)
{
	// ��l���~��Ă���f�����I���̂�҂�
	if(wk->demo_end){

		GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_BG0, VISIBLE_OFF );	//���C�����OBJ�ʂn�m
/*��[GS_CONVERT_TAG]*/
		GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_BG1, VISIBLE_OFF );	//���C�����OBJ�ʂn�m
/*��[GS_CONVERT_TAG]*/
		GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_BG2, VISIBLE_OFF );	//���C�����OBJ�ʂn�m
/*��[GS_CONVERT_TAG]*/
//		GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_OFF );	//���C�����OBJ�ʂn�m
/*��[GS_CONVERT_TAG]*/
		// �I�������T�[�o�[�`�F�b�N��
		WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_SERVER_CHECK );
		wk->sub_returnprocess = WORLDTRADE_TITLE;
		wk->subprocess_seq  = SUBSEQ_END;
	}

	return SEQ_MAIN;

}

//------------------------------------------------------------------
/**
 * @brief   ???
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_OpeningMessage( WORLDTRADE_WORK *wk )
{
//	SubSeq_MessagePrint( wk, msg_gtc_01_001, 1, 0, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_OPENING_FADE );
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ���ʃt�F�[�h�C��
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_OpeningFade( WORLDTRADE_WORK *wk )
{
	WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_title.c [654] M ********************\n" );
#endif
	wk->subprocess_seq = SUBSEQ_OPENING_FADE_WAIT;
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ���ʃt�F�[�h�҂�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_OpeningFadeWait( WORLDTRADE_WORK *wk )
{
	if(WIPE_SYS_EndCheck()){
		wk->subprocess_seq = SUBSEQ_START;
	}
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �t�F�[�h�҂�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_OpeningWait( WORLDTRADE_WORK *wk)
{
	if(WIPE_SYS_EndCheck()){
		wk->subprocess_seq = SUBSEQ_START;
	}
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �T�u�v���Z�X�V�[�P���X�X�^�[�g����
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_Start( WORLDTRADE_WORK *wk)
{
	// ���������ł����H���������ł����H
	SubSeq_MessagePrint( wk, msg_gtc_01_005, 1, 0, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
	GFL_CLACT_WK_SetAutoAnmFlag(wk->CursorActWork,1);
/*��[GS_CONVERT_TAG]*/
	WorldTrade_BoxPokeNumGetStart( wk );

	return SEQ_MAIN;
}

static const GFL_UI_TP_HITTBL TouchTable[]={
/*��[GS_CONVERT_TAG]*/
	{  40,  71,  24, 231,},
	{  80, 111,  24, 231,},
	{ 120, 151,  24, 231,},
	{GFL_UI_TP_HIT_END,0,0,0},		// �I���f�[�^
/*��[GS_CONVERT_TAG]*/
	
};

//------------------------------------------------------------------
/**
 * @brief   �^�b�`�p�l�����o
 *
 * @param   wk		
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
static u32 TouchPanelFunc( WORLDTRADE_WORK *wk )
{
	u32 ret;
	ret=GFL_UI_TP_HitTrg(TouchTable);
/*��[GS_CONVERT_TAG]*/

	return ret;
}


//------------------------------------------------------------------
/**
 * @brief   ���菈��(�^�b�`�E�L�[���ʁj
 *
 * @param   wk		
 *
 * @retval  none	
 */
//------------------------------------------------------------------
static void DecideFunc( WORLDTRADE_WORK *wk, int decide )
{
		switch(decide){
		// �������邩�H�悤�����݂邩�H
		case 0:		
			if(wk->DepositFlag==0){
				// ��������
				WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, MODE_DEPOSIT_SELECT );
				wk->subprocess_seq  = SUBSEQ_END;
				PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
			}else{
				// �悤�����݂�
				if(wk->serverWaitTime==0){
					WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_SERVER_CHECK );
					wk->sub_returnprocess = WORLDTRADE_MYPOKE;
					wk->subprocess_seq    = SUBSEQ_END;
					wk->serverWaitTime    = SEVER_RETRY_WAIT;
					PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
				}else{
					// �҂����Ԃ��߂��Ȃ��ƍēx�悤�������ɍs�����Ƃ��ł��Ȃ�
#ifdef PM_DEBUG
					if(GFL_UI_KEY_GetCont()&PAD_BUTTON_L){
/*��[GS_CONVERT_TAG]*/
						WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_SERVER_CHECK );
						wk->sub_returnprocess = WORLDTRADE_MYPOKE;
						wk->subprocess_seq    = SUBSEQ_END;
						wk->serverWaitTime    = SEVER_RETRY_WAIT;
						PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
					}else{
						GFL_CLACT_WK_SetAutoAnmFlag(wk->CursorActWork,0);
/*��[GS_CONVERT_TAG]*/
						SubSeq_MessagePrint( wk, msg_gtc_01_035, 1, 0, 0x0f0f );
						WorldTrade_SetNextSeq( wk, SUBSEQ_MES_1MIN_WAIT, SUBSEQ_START );
						PMSND_PlaySE(SE_GTC_NG);
						wk->wait = 0;
					}
#else
					GFL_CLACT_WK_SetAutoAnmFlag(wk->CursorActWork,0);
/*��[GS_CONVERT_TAG]*/
					SubSeq_MessagePrint( wk, msg_gtc_01_035, 1, 0, 0x0f0f );
					WorldTrade_SetNextSeq( wk, SUBSEQ_MES_1MIN_WAIT, SUBSEQ_START );
					PMSND_PlaySE(SE_GTC_NG);
					wk->wait = 0;
#endif
				}
			}
			break;

		// �|�P������T��
		case 1:
			//�u�����Ă��݂�v�͉����Ȃ���Ԃŉ�ʍ\�z����
			wk->SubLcdTouchOK = 0;
			WorldTrade_SubProcessChange( wk, WORLDTRADE_SEARCH, MODE_NORMAL );
			wk->subprocess_seq  = SUBSEQ_END;
			PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
			break;

		// ���イ��傤����
		case 2:
			SubSeq_TalkPrint( wk, msg_gtc_01_008, WorldTrade_GetTalkSpeed(wk), 0, 0x0f0f );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_YESNO );
			GFL_CLACT_WK_SetAutoAnmFlag(wk->CursorActWork,0);
/*��[GS_CONVERT_TAG]*/
			PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
			break;
		}

}

//------------------------------------------------------------------
/**
 * @brief   �T�u�v���Z�X�V�[�P���X���C��
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_Main( WORLDTRADE_WORK *wk)
{
	u32 ret;
	
	// �^�b�`����
	ret=TouchPanelFunc(wk);
	if(ret!=GFL_UI_TP_HIT_NONE){
/*��[GS_CONVERT_TAG]*/
		wk->TitleCursorPos = ret;
		PMSND_PlaySE(WORLDTRADE_MOVE_SE);
		WorldTrade_ActPos(wk->CursorActWork, 
			CursorPosTbl[wk->TitleCursorPos][0],
			CursorPosTbl[wk->TitleCursorPos][1]);
		DecideFunc(wk, ret);
		return SEQ_MAIN;
	}
	// �L�[����
	if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
/*��[GS_CONVERT_TAG]*/
			SubSeq_TalkPrint( wk, msg_gtc_01_008, WorldTrade_GetTalkSpeed(wk), 0, 0x0f0f );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_YESNO );
			GFL_CLACT_WK_SetAutoAnmFlag(wk->CursorActWork,0);
/*��[GS_CONVERT_TAG]*/
	}else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE){
/*��[GS_CONVERT_TAG]*/
		DecideFunc( wk, wk->TitleCursorPos );
	}else if(GFL_UI_KEY_GetTrg() & PAD_KEY_UP){
/*��[GS_CONVERT_TAG]*/
		if(wk->TitleCursorPos!=0){
			wk->TitleCursorPos--;
			PMSND_PlaySE(WORLDTRADE_MOVE_SE);
			WorldTrade_ActPos(wk->CursorActWork, 
				CursorPosTbl[wk->TitleCursorPos][0],
				CursorPosTbl[wk->TitleCursorPos][1]);
		}
	}else if(GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN){
/*��[GS_CONVERT_TAG]*/
		if(wk->TitleCursorPos<2){
			wk->TitleCursorPos++;
			PMSND_PlaySE(WORLDTRADE_MOVE_SE);
			WorldTrade_ActPos(wk->CursorActWork, 
				CursorPosTbl[wk->TitleCursorPos][0],
				CursorPosTbl[wk->TitleCursorPos][1]);
		}
	}

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ��l������Ȃ�f���J�n
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_EndDemo( WORLDTRADE_WORK *wk )
{
	
	WorldTrade_ReturnHeroDemo( wk, MyStatus_GetMySex(wk->param->mystatus) );
	wk->subprocess_seq = SUBSEQ_END_DEMO_WAIT;
	wk->demo_end       = 0;

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   ��l������Ȃ�f���҂�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_EndDemoWait( WORLDTRADE_WORK *wk )
{
	if(wk->demo_end){
		wk->subprocess_seq = SUBSEQ_END;
	}
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �T�u�v���Z�X�V�[�P���X�I������
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_End( WORLDTRADE_WORK *wk)
{
	// �ڑ���ʂ�������A�㉺��ʂŃt�F�[�h
	if(wk->sub_nextprocess==WORLDTRADE_ENTER){
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
		OS_Printf( "******************** worldtrade_title.c [846] MS ********************\n" );
#endif
	}else{
		// �����̉�ʂɂ����ꍇ�͏��ʂ����t�F�[�h
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
		OS_Printf( "******************** worldtrade_title.c [850] M ********************\n" );
#endif
	}
	wk->subprocess_seq = 0;
	
	return SEQ_FADEOUT;
}

//------------------------------------------------------------------
/**
 * @brief   �͂��E������
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_YesNo( WORLDTRADE_WORK *wk)
{
//	wk->YesNoMenuWork = WorldTrade_BmpWinYesNoMake(, WORLDTRADE_YESNO_PY2, YESNO_OFFSET );
	wk->tss			   = WorldTrade_TouchWinYesNoMake(WORLDTRADE_YESNO_PY2, YESNO_OFFSET, 3, 1 );
	wk->subprocess_seq = SUBSEQ_YESNO_SELECT;

	// �J�[�\���B��
	GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 0 );
/*��[GS_CONVERT_TAG]*/

	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   �͂��E�������I��
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_YesNoSelect( WORLDTRADE_WORK *wk)
{

	u32 ret = WorldTrade_TouchSwMain(wk);

	if(ret==TOUCH_SW_RET_YES){
		// ��l���A��f��
		TOUCH_SW_FreeWork( wk->tss );
		BmpWinFrame_Clear( wk->TalkWin, WINDOW_TRANS_OFF );

		GFL_BMPWIN_ClearTransWindow( wk->TalkWin );
		GFL_BMPWIN_ClearTransWindow( wk->ExplainWin );
		GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_OFF );
		GFL_CLACT_WK_SetDrawEnable( wk->PromptDsActWork, 0 );
		WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
		wk->subprocess_seq  = SUBSEQ_END_DEMO;

	}else if(ret==TOUCH_SW_RET_NO){
		// �������������g���C
		TOUCH_SW_FreeWork( wk->tss );
		BmpWinFrame_Clear( wk->TalkWin, WINDOW_TRANS_OFF );
		GFL_BMPWIN_ClearTransWindow( wk->TalkWin );
		GFL_CLACT_WK_SetAutoAnmFlag(wk->CursorActWork,1);
		wk->subprocess_seq = SUBSEQ_START;

		// �J�[�\���\������
		GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );

	}

/*
	int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_WORLDTRADE );

	if(ret!=BMPMENU_NULL){
		if(ret==BMPMENU_CANCEL){
			// �������������g���C
			BmpWinFrame_Clear( &wk->TalkWin, WINDOW_TRANS_OFF );
			GFL_BMPWIN_ClearTransWindow( &wk->TalkWin );
			GFL_CLACT_WK_SetAutoAnmFlag(wk->CursorActWork,1);
			wk->subprocess_seq = SUBSEQ_START;
		}else{
			// ��l���A��f��
			BmpWinFrame_Clear( &wk->TalkWin, WINDOW_TRANS_OFF );
			GFL_BMPWIN_ClearTransWindow( &wk->TalkWin );
			WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
			wk->subprocess_seq  = SUBSEQ_END_DEMO;
			
		}
	}
*/
	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   ��b�I����҂��Ď��̃V�[�P���X��
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_MessageWait( WORLDTRADE_WORK *wk )
{
	if( GF_MSG_PrintEndCheck( wk->MsgIndex )==0){
		wk->subprocess_seq = wk->subprocess_nextseq;
	}
	return SEQ_MAIN;

}

//------------------------------------------------------------------
/**
 * @brief   �P�b�҂��Ď��̃V�[�P���X��
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_Message1MinWait( WORLDTRADE_WORK *wk )
{
	if( GF_MSG_PrintEndCheck( wk->MsgIndex )==0){
		wk->wait++;
		if(wk->wait > SEVERNO_MESSAGE_WAIT){
			wk->wait = 0;
			wk->subprocess_seq = wk->subprocess_nextseq;
		}
	}
	return SEQ_MAIN;
	
}






















// �f�s�r�^�C�g����ʂ̃��j���[����
static const int menu_str_table[][3]={
	{
		msg_gtc_02_002_02,		// �ۂ��������������
		msg_gtc_02_003,			// �|�P������������
		msg_gtc_02_004,			// ���イ��傤����
	},
	{
		msg_gtc_02_002_01,		// �悤�����݂�
		msg_gtc_02_003,			// �|�P������������
		msg_gtc_02_004,			// ���イ��傤����
	},
};

//------------------------------------------------------------------
/**
 * @brief   �R�̃��j���[���ڂ�`�悷��
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void TitleMenuPrint( WORLDTRADE_WORK *wk )
{
	int i;
//	int menu = WorldTradeData_GetFlag( wk->param->worldtrade_data );
	int menu = wk->DepositFlag;

	// �|�P������a���Ă��邩�ǂ����ŁA���j���[�̍��ڂ��ς��
	for(i=0;i<3;i++){
		GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->MenuWin[i]), 15, 0, 0, MENU_TEXT_SX*8, 8 );
		GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->MenuWin[i]), 14, 0, 8, MENU_TEXT_SX*8, 2 );
		GFL_BMP_Fill( GFL_BMPWIN_GetBmp(wk->MenuWin[i]), 13, 0, 10, MENU_TEXT_SX*8,  6 );
		WorldTrade_BmpWinPrint( wk->MenuWin[i], wk->MsgManager, FONT_TOUCH, menu_str_table[menu][i], 0x0000 );
		GFL_BMPWIN_MakeTransWindow( wk->MenuWin[i] );
	}
}





//------------------------------------------------------------------
/**
 * @brief   ��b�E�C���h�E�\��
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat )
{
	// ������擾
	STRBUF *tempbuf;
	
	GFL_MSG_GetString(  wk->MsgManager, msgno, wk->TalkString );

	// ��b�E�C���h�E�g�`��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin),  0x0f0f );
/*��[GS_CONVERT_TAG]*/
	BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, WORLDTRADE_MESFRAME_CHR, WORLDTRADE_MESFRAME_PAL );
/*��[GS_CONVERT_TAG]*/

	// ������`��J�n
	wk->MsgIndex = GF_STR_PrintSimple( wk->MsgWin, FONT_TALK, wk->TalkString, 0, 0, wait, NULL);


}

//------------------------------------------------------------------
/**
 * @brief   ��b�E�C���h�E�\��
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void SubSeq_TalkPrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat )
{
	// ������擾
	STRBUF *tempbuf;
	
	GFL_MSG_GetString(  wk->MsgManager, msgno, wk->TalkString );

	// ��b�E�C���h�E�g�`��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->TalkWin),  0x0f0f );
/*��[GS_CONVERT_TAG]*/
	BmpWinFrame_Write( wk->TalkWin, WINDOW_TRANS_ON, WORLDTRADE_MESFRAME_CHR, WORLDTRADE_MESFRAME_PAL );
/*��[GS_CONVERT_TAG]*/

	// ������`��J�n
	wk->MsgIndex = GF_STR_PrintSimple( wk->TalkWin, FONT_TALK, wk->TalkString, 0, 0, wait, NULL);


}

//------------------------------------------------------------------
/**
 * @brief   BMPWIN�ɕ�����`��
 *
 * @param   win		
 * @param   msgman		
 * @param   msgno		
 * @param   dat		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void WorldTrade_BmpWinPrint( GFL_BMPWIN *win, GFL_MSGDATA *msgman, int font, int msgno, u16 dat )
/*��[GS_CONVERT_TAG]*/
{
	// ������擾
	STRBUF *tempbuf;
	
	tempbuf = GFL_MSG_CreateString(  msgman, msgno );

	// ��b�E�C���h�E�g�`��
//	GFL_BMP_Clear( win,  dat );
/*��[GS_CONVERT_TAG]*/

	// ������`��J�n
	GF_STR_PrintColor( win, font, tempbuf, 0, 0, MSG_ALLPUT, PRINTSYS_LSB_Make( 10, 9, 0), NULL);
/*��[GS_CONVERT_TAG]*/

	GFL_STR_DeleteBuffer(tempbuf);
}


//==============================================================================
/**
 * @brief   ����ʗp�u���E�������[���v256�F�O���t�B�b�N�]��
 *
 * @param   wk		
 * @param   winflag	 1:�g���[�h��ʂ̃E�C���h�E��]������ 0:���Ȃ�
 *
 * @retval  none		
 */
//==============================================================================
void WorldTrade_SubLcdBgGraphicSet( WORLDTRADE_WORK *wk  )
{

	// ����ʂa�f�p���b�g�]��
	GFL_ARC_UTIL_TransVramPalette(    ARCID_WORLDTRADE_GRA, NARC_worldtrade_traderoom_nclr, PALTYPE_SUB_BG,  0, 16*16*2,  HEAPID_WORLDTRADE);
/*��[GS_CONVERT_TAG]*/

	// �T�u���BG1�L�����]��
	GFL_ARC_UTIL_TransVramBgCharacter( ARCID_WORLDTRADE_GRA, NARC_worldtrade_traderoom_lz_ncgr,  GFL_BG_FRAME1_S, 0, 32*21*0x40, 1, HEAPID_WORLDTRADE);
/*��[GS_CONVERT_TAG]*/
/*��[GS_CONVERT_TAG]*/

	// �T�u���BG1�X�N���[���]��
	GFL_ARC_UTIL_TransVramScreen( ARCID_WORLDTRADE_GRA, NARC_worldtrade_traderoom_lz_nscr, 
/*��[GS_CONVERT_TAG]*/
	     GFL_BG_FRAME1_S, 0, 32*24*2, 1, HEAPID_WORLDTRADE);
/*��[GS_CONVERT_TAG]*/
	//����256x256�S�Ă𖄂߂Ă��Ȃ��X�N���[���ׁ̈A�����ɃS�~���c��̂ŃN���A
	GFL_BG_FillScreen(  GFL_BG_FRAME1_S, 0, 0, 24, 32, 32-24, GFL_BG_SCRWRT_PALIN );
/*��[GS_CONVERT_TAG]*/
/*��[GS_CONVERT_TAG]*/
/*��[GS_CONVERT_TAG]*/
	GFL_BG_LoadScreenReq(  GFL_BG_FRAME1_S );
/*��[GS_CONVERT_TAG]*/
/*��[GS_CONVERT_TAG]*/

	// ��b�t�H���g�p���b�g�]��
	TalkFontPaletteLoad( PALTYPE_SUB_BG, WORLDTRADE_SUB_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

}	

//==============================================================================
/**
 * @brief   �g���[�h���[����Ԑ����X�N���[���]��
 *
 * @param   wk		
 *
 * @retval  none		
 */
//==============================================================================
void WorldTrade_SubLcdWinGraphicSet( WORLDTRADE_WORK *wk )
{
	// -------------�T�u���---------------------
	// ���C�����BG2�L�����]��
	GFL_ARC_UTIL_TransVramBgCharacter(  ARCID_WORLDTRADE_GRA, NARC_worldtrade_traderoom_win_lz_ncgr,
/*��[GS_CONVERT_TAG]*/
						GFL_BG_FRAME2_S,    0, 0, 1, HEAPID_WORLDTRADE);
/*��[GS_CONVERT_TAG]*/

	// ��Ԑ����X�N���[��
	GFL_ARC_UTIL_TransVramScreen( ARCID_WORLDTRADE_GRA, NARC_worldtrade_traderoom_win_lz_nscr,
/*��[GS_CONVERT_TAG]*/
					  GFL_BG_FRAME2_S,    0, 0, 1, HEAPID_WORLDTRADE);
/*��[GS_CONVERT_TAG]*/

	// �uDS�̉���ʂ��݂Ă˃A�C�R���v�\��
	GFL_CLACT_WK_SetDrawEnable( wk->PromptDsActWork, 1 );
/*��[GS_CONVERT_TAG]*/

}


//==============================================================================
/**
 * @brief   �T�u��ʂ�GTS�����pBMPWIN���m�ۂ���
 *
 * @param   wk		
 *
 * @retval  none		
 */
//==============================================================================
void WorldTrade_SubLcdExpainPut( WORLDTRADE_WORK *wk, int explain )
{
	// �uGTS�v
	wk->ExplainWin	= GFL_BMPWIN_Create( GFL_BG_FRAME0_S,
		EXPLAIN_WIN_X, EXPLAIN_WIN_Y, EXPLAIN_WIN_SX, EXPLAIN_WIN_SY, 
		WORLDTRADE_TALKFONT_SUB_PAL, GTS_EXPLAIN_OFFSET  );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->ExplainWin), 0x0000 );

	// ���ʃ��b�Z�[�W
	WorldTrade_ExplainPrint( wk->ExplainWin,  wk->MsgManager, explain );

	OS_Printf("�����E�C���h�E�m��\n");
}
