//============================================================================================
/**
 * @file	worldtrade_partner.c
 * @brief	���E��������|�P�����{�����
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
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"
#include "savedata/wifilist.h"

#include "net_app/worldtrade.h"
#include "worldtrade_local.h"

#include "msg/msg_wifi_gts.h"


#include "worldtrade.naix"			// �O���t�B�b�N�A�[�J�C�u��`



//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
/*** �֐��v���g�^�C�v ***/
static void BgInit( int sub_bg1_offset );
static void BgExit( void );
static void BgGraphicSet( WORLDTRADE_WORK *wk );
static void BmpWinInit( WORLDTRADE_WORK *wk );
static void BmpWinDelete( WORLDTRADE_WORK *wk );
static void InitWork( WORLDTRADE_WORK *wk );
static void FreeWork( WORLDTRADE_WORK *wk );
static void SetCellActor(WORLDTRADE_WORK *wk);
static void DelCellActor(WORLDTRADE_WORK *wk);

static int SubSeq_Start( WORLDTRADE_WORK *wk);
static int SubSeq_Main( WORLDTRADE_WORK *wk);
static int SubSeq_End( WORLDTRADE_WORK *wk);
static int SubSeq_MessageWait( WORLDTRADE_WORK *wk );
static int SubSeq_YesNo( WORLDTRADE_WORK *wk);
static int SubSeq_YesNoSelect( WORLDTRADE_WORK *wk);
static int SubSeq_PageChange( WORLDTRADE_WORK *wk);
static int SubSeq_ReturnScreen1( WORLDTRADE_WORK *wk );
static int SubSeq_ExchangeScreen2( WORLDTRADE_WORK *wk );

static void ChangePage( WORLDTRADE_WORK *wk );
static void PokeLabelPrint( GFL_MSGDATA *MsgManager, GFL_BMPWIN * win[], int msg, WT_PRINT *print );
static void TouchPrint( GFL_MSGDATA *MsgManager, GFL_BMPWIN * win[], int msg, WT_PRINT *print );


static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat );
static void WantPokeInfoPrint( GFL_BMPWIN * win[], GFL_MSGDATA *gtcmsg, GFL_MSGDATA *monsname, Dpw_Tr_PokemonSearchData *dtsd, WT_PRINT *print );
static void TrainerInfoPrint( GFL_BMPWIN *win[], STRBUF *strbuf1, STRBUF *strbuf2, WT_PRINT *print );
static void SlideScreenVFunc( void *p );
static void DecidePartner( WORLDTRADE_WORK *wk , int result );


enum{
	SUBSEQ_START=0,
	SUBSEQ_MAIN,
	SUBSEQ_END,
	SUBSEQ_MES_WAIT,
	SUBSEQ_YESNO,
	SUBSEQ_YESNO_SELECT,
	SUBSEQ_PAGE_CHANGE,
 	SUBSEQ_EXCHANGE_SCREEN2,	
    SUBSEQ_RETURN_SCREEN1,	
};

static int (*Functable[])( WORLDTRADE_WORK *wk ) = {
	SubSeq_Start,			// SUBSEQ_START=0,
	SubSeq_Main,            // SUBSEQ_MAIN,
	SubSeq_End,             // SUBSEQ_END,
	SubSeq_MessageWait,     // SUBSEQ_MES_WAIT
	SubSeq_YesNo,			// SUBSEQ_YESNO
	SubSeq_YesNoSelect,		// SUBSEQ_YESNO_SELECT
	SubSeq_PageChange,		// SUBSEQ_PAGE_CHANGE,
	SubSeq_ExchangeScreen2,	// SUBSEQ_EXCHANGE_SCREEN2
	SubSeq_ReturnScreen1,	// SUBSEQ_RETURN_SCREEN1
};

#define INFO_TEXT_WORD_NUM	( 10*2 )

#define PARTNER_INFOWIN_MAX	(   16 )

// ���\���E�C���h�EInfoWin�̃C���f�b�N�X��`
enum {
	P_INFOWIN_NICKNAME=0,
	P_INFOWIN_TRIBENAME,
	P_INFOWIN_REBERU,
	P_INFOWIN_LEVEL,
	P_INFOWIN_MOTIMONO,
	P_INFOWIN_ITEMNAME,
	P_INFOWIN_NAMELABEL,
	P_INFOWIN_MOTINUSI,
	P_INFOWIN_TRAINERNAME,
	P_INFOWIN_SUNDEIRUBASHO,
	P_INFOWIN_1,
	P_INFOWIN_2,
	P_INFOWIN_OYA,
	P_INFOWIN_PARENTNAME,
	P_INFOWIN_HOSII_POKEMON,
	P_INFOWIN_WANT_POKE,

	
};

// ���j���[�E�C���h�EBmpWin�̃C���f�b�N�X��`
enum{
	MENU_EXCHANGE_WIN=0,
	MENU_BACK_WIN,  
};

#define PARTNER_POKEMON_X	( 208 )
#define PARTNER_POKEMON_Y	(  58 )


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
int WorldTrade_Partner_Init(WORLDTRADE_WORK *wk, int seq)
{
	POKEMON_PARAM *pp;
	
	// ���[�N������
	InitWork( wk );
	
	// BG�ݒ�
   	BgInit(  -wk->DrawOffset-32 );

	// 3D�ݒ�
	WorldTrade_MyPoke_G3D_Init( wk );
	{	
		VecFx32	pos;
		pos.x	= 13<<FX32_SHIFT;
		pos.y	= 7<<FX32_SHIFT;
		pos.z	= 0;
		wk->pokeMcss	= WorldTrade_MyPoke_MCSS_Create( wk, (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst((POKEMON_PARAM*)wk->DownloadPokemonData[wk->TouchTrainerPos].postData.data), &pos );
	}


	// BG�O���t�B�b�N�]��
	BgGraphicSet( wk );

	// BMPWIN�m��
	BmpWinInit( wk );

	SetCellActor(wk);


	// ����̃|�P�����̏��
	WorldTrade_PokeInfoPrint( wk->MsgManager, 
							  wk->MonsNameManager, 
							  wk->WordSet, 
							  &wk->InfoWin[P_INFOWIN_NICKNAME], 
							  (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst((POKEMON_PARAM*)wk->DownloadPokemonData[wk->TouchTrainerPos].postData.data),
							  &wk->DownloadPokemonData[wk->TouchTrainerPos].postSimple, &wk->print );

	// ��������̕\��
	pp = (POKEMON_PARAM*)wk->DownloadPokemonData[wk->TouchTrainerPos].postData.data;
	WorldTrade_PokeInfoPrint2( wk->MsgManager, &wk->InfoWin[P_INFOWIN_MOTINUSI], 
								wk->DownloadPokemonData[wk->TouchTrainerPos].name, 
								pp, &wk->InfoWin[P_INFOWIN_OYA], &wk->print );

	// �|�P�����摜�]��
	WorldTrade_TransPokeGraphic( (POKEMON_PARAM*)wk->DownloadPokemonData[wk->TouchTrainerPos].postData.data );

	PokeLabelPrint( wk->MsgManager, &wk->InfoWin[P_INFOWIN_HOSII_POKEMON], msg_gtc_04_008, &wk->print );	// �~�����|�P����
	PokeLabelPrint( wk->MsgManager, &wk->InfoWin[P_INFOWIN_SUNDEIRUBASHO], msg_gtc_04_012, &wk->print );	// �Z��ł���ꏊ

	TouchPrint( wk->MsgManager, &wk->MenuWin[MENU_EXCHANGE_WIN], msg_gtc_04_019, &wk->print );	// �u�������񂷂�v
	TouchPrint( wk->MsgManager, &wk->MenuWin[MENU_BACK_WIN],     msg_gtc_05_014 , &wk->print);	// �u���ǂ�v

	// �u�ق����|�P�����v���u����ł���΂���v�̕`��
	ChangePage( wk );
	WorldTrade_SetPartnerCursorPos( wk, wk->TouchTrainerPos, -wk->DrawOffset );

	wk->vfunc2 = SlideScreenVFunc;

	// �T�u��ʂ���E���C����ʂ���
	GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );

	// �Q��ڈȍ~
	// ���C�v�t�F�[�h�J�n
	if(wk->sub_process_mode==MODE_FROM_BOX){
	    if(GX_GetMasterBrightness() == -16 && GXS_GetMasterBrightness() != -16){
    		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
	    				WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
        }
	    else if(GX_GetMasterBrightness() != -16 && GXS_GetMasterBrightness() == -16){
    		WIPE_SYS_Start( WIPE_PATTERN_S, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
	    				WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
        }
        else{
    		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
	    				WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
        }
	}
#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_partner.c [148] M ********************\n" );
#endif
	wk->subprocess_seq = SUBSEQ_START;

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
int WorldTrade_Partner_Main(WORLDTRADE_WORK *wk, int seq)
{
	int ret,i;
	
	ret = (*Functable[wk->subprocess_seq])( wk );

	// �T�u��ʂ�OBJ���W���ړ������鏈��
	for(i=0;i<SUB_OBJ_NUM;i++){
		WorldTrade_CLACT_PosChangeSub( wk->SubActWork[i], wk->SubActY[i][0], 
									wk->SubActY[i][1]+wk->DrawOffset+32 );
	}
	// ���C����ʂ̃|�P�����摜���ړ������鏈��
	//WorldTrade_CLACT_PosChange( wk->PokemonActWork, PARTNER_POKEMON_X, 
		//											PARTNER_POKEMON_Y-wk->DrawOffset );
	{
		VecFx32	pos;
		pos.x	= 13<< FX32_SHIFT;
		pos.y	= 7<< FX32_SHIFT - wk->DrawOffset;
		pos.z	= 0;
		MCSS_SetPosition( wk->pokeMcss, &pos );
	}
	WorldTrade_SetPartnerCursorPos( wk, wk->TouchTrainerPos, wk->DrawOffset );
	WorldTrade_MyPoke_G3D_Draw( wk );

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
int WorldTrade_Partner_End(WORLDTRADE_WORK *wk, int seq)
{

	wk->vfunc2 = NULL;

	DelCellActor(wk);

	FreeWork( wk );
	
	BmpWinDelete( wk );
	
	WorldTrade_MyPoke_MCSS_Delete( wk, wk->pokeMcss );
	WorldTrade_MyPoke_G3D_Exit( wk );

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
static void BgInit( int sub_bg1_offset )
{
	// BG SYSTEM
	{	
		GFL_BG_SYS_HEADER BGsys_data = {
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		GFL_BG_SetBGMode( &BGsys_data );

	}

	// ���C����ʃe�L�X�g��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000,GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME2_M );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M, TRUE );
	}

	// ���C����ʃ��j���[��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000,GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M, TRUE );
	}

	GFL_BG_SetClearCharacter( GFL_BG_FRAME2_M, 32, 0, HEAPID_WORLDTRADE );

	// �T�u��ʏ�����
	if(GXS_GetMasterBrightness() == 0){
    	WorldTrade_SubLcdBgInit( sub_bg1_offset, TRUE );
    }
    else{
    	WorldTrade_SubLcdBgInit( sub_bg1_offset, FALSE );
    }

	//���g�pBG�I�t
	GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_BG3, VISIBLE_OFF );	//���C�����BG3��OFF
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
	WorldTrade_SubLcdBgExit( );

	// ���C����ʂa�f�����
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );

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

	// -------------���C�����---------------------
	// ��b�t�H���g�p���b�g�]��
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

	// ��b�E�C���h�E�O���t�B�b�N�]��
	BmpWinFrame_GraphicSet(	 GFL_BG_FRAME0_S, WORLDTRADE_MESFRAME_CHR, 
						WORLDTRADE_MESFRAME_SUB_PAL,  CONFIG_GetWindowType(wk->param->config), HEAPID_WORLDTRADE );




	// ���C�����BG1�L�����]��
	GFL_ARC_UTIL_TransVramBgCharacter( ARCID_WORLDTRADE_GRA, NARC_worldtrade_poke_view_lz_ncgr,  
					   GFL_BG_FRAME1_M, 0, 32*3*0x20, 1, HEAPID_WORLDTRADE);

	// ���ʂa�f�p���b�g�]��
	GFL_ARC_UTIL_TransVramPalette(    ARCID_WORLDTRADE_GRA, NARC_worldtrade_poke_view_nclr, 
					   PALTYPE_MAIN_BG, 0, 16*3*2,  HEAPID_WORLDTRADE);

	// �X�N���[����_Init�̉��̕��ōs��


	// -------------�T�u���---------------------
	// ���C�����BGs�L�����]��
	GFL_ARC_UTIL_TransVramBgCharacter(  ARCID_WORLDTRADE_GRA, NARC_worldtrade_traderoom_win_lz_ncgr,  
						GFL_BG_FRAME2_S,    0, 0, 1, HEAPID_WORLDTRADE);

	GFL_ARC_UTIL_TransVramScreen( ARCID_WORLDTRADE_GRA, NARC_worldtrade_traderoom_win2_lz_nscr,  
					  GFL_BG_FRAME2_S,    0, 0, 1, HEAPID_WORLDTRADE);

	// ��b�t�H���g�p���b�g�]��
	TalkFontPaletteLoad( PALTYPE_SUB_BG, WORLDTRADE_SUB_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

}


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
	//�o�^���i�[
	GFL_CLWK_DATA add;

	GFL_STD_MemClear( &add, sizeof(GFL_CLWK_DATA) );

	//WorldTrade_MakeCLACT( &add,  wk, &wk->clActHeader_main, NNS_G2D_VRAM_TYPE_2DMAIN );

	// ����̃|�P�����\��
	add.pos_x = PARTNER_POKEMON_X;
	add.pos_y = PARTNER_POKEMON_Y;
	wk->PokemonActWork = GFL_CLACT_WK_Create( wk->clactSet,
			wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
	GFL_CLACT_WK_SetAutoAnmFlag(wk->PokemonActWork,1);
	GFL_CLACT_WK_SetAnmSeq( wk->PokemonActWork, 37 );
	GFL_CLACT_WK_SetDrawEnable( wk->PokemonActWork, 0 );


	WirelessIconEasy();
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
	GFL_CLACT_WK_Remove(wk->PokemonActWork);
}

#define NAME_TEXT_X		(  8 )
#define NAME_TEXT_Y		(  5 )
#define NAME_TEXT_SX	( 10 )
#define NAME_TEXT_SY	(  2 )

#define MENU_MAX_NUM	(  3 )

#define SELECT_MENU_X	(  21 )
#define SELECT_MENU_Y 	(  15 )
#define SELECT_MENU_SX	( 5*2 )
#define SELECT_MENU_SY	(   4 )


#define MENU_EXCHANGE1_X	(  1 )
#define MENU_EXCHANGE1_Y	( 21 )
#define MENU_EXCHANGE1_SX	( 13 )
#define MENU_EXCHANGE1_SY 	(  2 )

#define MENU_EXCHANGE2_X	( 17 )
#define MENU_EXCHANGE2_Y	( 21 )
#define MENU_EXCHANGE2_SX	( 13 )
#define MENU_EXCHANGE2_SY 	(  2 )

#define BMPWIN_MAIN_START_OFFSET	( 1 )

#define BMPWIN_SUB_START_OFFSET		( WORLDTRADE_MENUFRAME_CHR )
#define BMPWIN_MESWINDOW_OFFSET   	( BMPWIN_SUB_START_OFFSET + MENU_WIN_CGX_SIZ )
#define BMPWIN_MENU1_OFFSET			( BMPWIN_MESWINDOW_OFFSET + TALK_WIN_SX*TALK_WIN_SY )
#define BMPWIN_MENU2_OFFSET			( BMPWIN_MENU1_OFFSET + MENU_EXCHANGE1_SX*MENU_EXCHANGE1_SY )
#define INFO_TEXT_SUB_OFFSET		( BMPWIN_MENU2_OFFSET + MENU_EXCHANGE2_SX*MENU_EXCHANGE2_SY  )


// ����̃|�P��������ʂ�BMPWIN���
static const info_bmpwin_table[][5]={
	{   1,  1,  9,  2, GFL_BG_FRAME2_M, },	// �|�P�����̃j�b�N�l�[��
	{   8,  4,  8,  2, GFL_BG_FRAME2_M, },	// �|�P�����̎푰��
	{  11,  1,  7,  2, GFL_BG_FRAME2_M, },	// �u���x���v
	{  14,  1,  7,  2, GFL_BG_FRAME2_M, },	// ���x��
	{   1, 10,  5,  2, GFL_BG_FRAME2_M, },	// �u�������́v
	{   8, 10, 11,  2, GFL_BG_FRAME2_M, },	// �����A�C�e����
	{   1,  4,  6,  2, GFL_BG_FRAME2_M, },	// �u�Ȃ܂��v
	{   1, 13,  9,  2, GFL_BG_FRAME2_M, },	// �u���������ЂƁv
	{  11, 13,  8,  2, GFL_BG_FRAME2_M, },	// �g���[�i�[��
	{   1, 16, 13,  2, GFL_BG_FRAME2_M, },	// �u����ł���΂���v//154
	{   2, 18, 27,  2, GFL_BG_FRAME2_M, },	// �Z��ł��鍑
	{   3, 20, 27,  2, GFL_BG_FRAME2_M, },	// �Z��ł���ꏊ�i���E�B�j
	{   1,  7,  4,  2, GFL_BG_FRAME2_M, },	// �u����v
	{   8,  7,  8,  2, GFL_BG_FRAME2_M, },	// �e��
	{   1,  1, 24,  2, GFL_BG_FRAME3_S, },	// �u�ق����|�P�����v
	{   2,  3, 27,  2, GFL_BG_FRAME3_S, },	// �ق����|�P�������1
};

// �͂��E��������BMPWIN�̈�͍Ō�ɂ����Ă������̂����A
// ���E�C���h�E�̑�������߂Ȃ��̂ŁA204�L�������炵�Ă���
// �i�͂��Ă��܂����̂�204�L�����ɕς��� by Mori (06.05.09)
#define YESNO_OFFSET 		   ( INFO_TEXT_SUB_OFFSET + 24*2+27*2 )



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

	wk->MsgWin	= GFL_BMPWIN_CreateFixPos( GFL_BG_FRAME0_S,
		TALK_WIN_X, TALK_WIN_Y, TALK_WIN_SX, TALK_WIN_SY, 
		WORLDTRADE_TALKFONT_SUB_PAL,  BMPWIN_MESWINDOW_OFFSET );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0x0000 );

	// �u�������񂷂�v
	wk->MenuWin[0]	= GFL_BMPWIN_CreateFixPos( GFL_BG_FRAME0_S,
		MENU_EXCHANGE1_X, MENU_EXCHANGE1_Y, MENU_EXCHANGE1_SX, MENU_EXCHANGE1_SY, 
		WORLDTRADE_TALKFONT_SUB_PAL,  BMPWIN_MENU1_OFFSET );	
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuWin[0]), 0x0000 );

	// �u���ǂ�v
	wk->MenuWin[1]	= GFL_BMPWIN_CreateFixPos( GFL_BG_FRAME0_S,
		MENU_EXCHANGE2_X, MENU_EXCHANGE2_Y, MENU_EXCHANGE2_SX, MENU_EXCHANGE2_SY, 
		WORLDTRADE_TALKFONT_SUB_PAL,  BMPWIN_MENU2_OFFSET );	
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuWin[1]), 0x0000 );

	// BG0��BMPWIN���E�C���h�E�m��
	{
		int i, m_offset,s_offset;

		// ���ʂƉ���ʂŕʁX�ɃL�����N�^�[�̈�����Z���Ȃ���m�ۂ��Ă���
		m_offset = BMPWIN_MAIN_START_OFFSET;		
		s_offset = INFO_TEXT_SUB_OFFSET;
		
		// BMPWIN�m��
		for(i=0;i<PARTNER_INFOWIN_MAX;i++){
			if(info_bmpwin_table[i][4]==GFL_BG_FRAME2_M){
				wk->InfoWin[i] = GFL_BMPWIN_CreateFixPos(
						info_bmpwin_table[i][4],	// �\���t���[��(GFL_BG_FRAME2_M)
						info_bmpwin_table[i][0], 	// X
						info_bmpwin_table[i][1], 	// Y
						info_bmpwin_table[i][2], 	// W
						info_bmpwin_table[i][3], 	// H
						WORLDTRADE_TALKFONT_PAL,  m_offset );
				m_offset += info_bmpwin_table[i][2]*info_bmpwin_table[i][3];

			}else{
				wk->InfoWin[i]	= GFL_BMPWIN_CreateFixPos(
						info_bmpwin_table[i][4],	// �\���t���[��(GFL_BG_FRAME0_S)
						info_bmpwin_table[i][0], 	// X
						info_bmpwin_table[i][1], 	// Y
						info_bmpwin_table[i][2], 	// W
						info_bmpwin_table[i][3], 	// H
						WORLDTRADE_TALKFONT_SUB_PAL,  s_offset );
				s_offset += info_bmpwin_table[i][2]*info_bmpwin_table[i][3];
				
			}
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin[i]), 0x0000 );
			GFL_BMPWIN_MakeTransWindow( wk->InfoWin[i] );
		}
	}


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
	
	GFL_BMPWIN_Delete( wk->MsgWin );

	GFL_BMPWIN_ClearTransWindow( wk->MenuWin[1] );
	GFL_BMPWIN_ClearTransWindow( wk->MenuWin[0] );
                           
	GFL_BMPWIN_Delete( wk->MenuWin[1] );
	GFL_BMPWIN_Delete( wk->MenuWin[0] );
	{
		int i;
		for(i=0;i<PARTNER_INFOWIN_MAX;i++){
			GFL_BMPWIN_ClearTransWindow( wk->InfoWin[i] );
			GFL_BMPWIN_Delete( wk->InfoWin[i] );
		}
	}


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
	int i;
	GFL_MSGDATA *man;
	Dpw_Tr_Data     *dtp = &wk->DownloadPokemonData[wk->TouchTrainerPos];

	// ������o�b�t�@�쐬
	wk->TalkString  = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WORLDTRADE );

	WORDSET_ClearAllBuffer( wk->WordSet );
	// ���ԍ������݂��Ă���ꍇ�͕�������Z�b�g
	if(dtp->countryCode!=0){
		WORDSET_RegisterCountryName( wk->WordSet, 8, dtp->countryCode );
	}
	// �n��ԍ������݂��Ă���ꍇ�͕�������Z�b�g
	if(dtp->localCode!=0){
		WORDSET_RegisterLocalPlaceName( wk->WordSet, 9, dtp->countryCode, dtp->localCode );
	}

	// ���E�n�敶����擾
	wk->InfoString[0] = MSGDAT_UTIL_AllocExpandString( wk->WordSet, wk->MsgManager, msg_gtc_04_013, HEAPID_WORLDTRADE );
	wk->InfoString[1] = MSGDAT_UTIL_AllocExpandString( wk->WordSet, wk->MsgManager, msg_gtc_04_014, HEAPID_WORLDTRADE );
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
	int i;

	GFL_STR_DeleteBuffer(wk->InfoString[0]);
	GFL_STR_DeleteBuffer(wk->InfoString[1]);

	GFL_STR_DeleteBuffer( wk->TalkString ); 

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
 * @brief   �T�u�v���Z�X�V�[�P���X�X�^�[�g����
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_Start( WORLDTRADE_WORK *wk)
{
	// 
//	SubSeq_MessagePrint( wk, msg_wifilobby_028, 1, 0, 0x0f0f );
//	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );

//	ChangePage( wk );

	// �u����ʂ��݂Ă˃A�C�R���v�B��
	GFL_CLACT_WK_SetDrawEnable( wk->PromptDsActWork, 0 );

	// ������ʂ��痈�����͉�ʃY�����t�F�[�h
	if(wk->sub_process_mode==MODE_FROM_SEARCH){	
		// �㉺��ʃt�F�[�h�i��ʂ��炵������j
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
						WIPE_FADE_BLACK, EXCHANGE_SCREEN_SYNC, 1, HEAPID_WORLDTRADE );
		wk->subprocess_seq = SUBSEQ_EXCHANGE_SCREEN2;
	}else{
		// �{�b�N�X���痈�����͕��ʂɃt�F�[�h
		wk->subprocess_seq = SUBSEQ_MAIN;
		GFL_CLACT_WK_SetDrawEnable( wk->PartnerCursorActWork, 1 );
	}

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   ��������؂�ւ�����
 *
 * @param   wk		
 * @param   result	
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void DecidePartner( WORLDTRADE_WORK *wk, int result )
{
	if(result != wk->TouchTrainerPos && result>=0){
		GFL_CLACT_WK_SetAnmSeq( wk->SubActWork[result+1], 16+result*4 );
		wk->subprocess_seq     = SUBSEQ_END; 
		WorldTrade_SubProcessChange( wk, WORLDTRADE_PARTNER, MODE_FROM_BOX );
		wk->TouchTrainerPos = result;
		WorldTrade_SetPartnerCursorPos( wk, result, 0 );
		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
		OS_Printf("�l���^�b�`%d\N", result);
	}

}

static const GFL_UI_TP_HITTBL PartnerTouchTbl[]={
	{ 162, 191,   3, 125, },
	{ 162, 191, 130, 253, },
	{ GFL_UI_TP_HIT_END, 0, 0, 0 },
};

//   1 0 2
// 3       4
// 5       6 

// ���A�E�̏�
static const u8 partner_key_tbl[][2]={
	{ 1, 2, },
	{ 3, 0, },
	{ 0, 4, },
	{ 5, 1, },
	{ 2, 6, },
	{ 5, 3, },
	{ 4, 6, },
};

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
	if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE){
		SubSeq_MessagePrint( wk, msg_gtc_01_017, 1, 0, 0x0f0f );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_YESNO );
		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
	}else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
		// ��ʂ��炵���t�F�[�h�J�n(��������ʂɖ߂�j
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
						WIPE_FADE_BLACK, EXCHANGE_SCREEN_SYNC, 1, HEAPID_WORLDTRADE );
		wk->subprocess_seq  = SUBSEQ_RETURN_SCREEN1;
		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
	}else if(GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT){
		if( (wk->TouchTrainerPos!=partner_key_tbl[wk->TouchTrainerPos][0]) &&
				(wk->SearchResult>=partner_key_tbl[wk->TouchTrainerPos][0]+1) ){
			DecidePartner( wk, partner_key_tbl[wk->TouchTrainerPos][0] );
		}
	}else if(GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT){
		if( (wk->TouchTrainerPos!=partner_key_tbl[wk->TouchTrainerPos][1]) &&
				(wk->SearchResult>=partner_key_tbl[wk->TouchTrainerPos][1]+1) ){
			DecidePartner( wk, partner_key_tbl[wk->TouchTrainerPos][1] );
		}
	}
	else{

		// �^�b�`�p�l���`�F�b�N(�l���j
		u32 menuret;
		int result = WorldTrade_SubLcdObjHitCheck( wk->SearchResult );
		if(result!=GFL_UI_TP_HIT_NONE){
			DecidePartner( wk, result );
		}

		// �^�b�`�p�l���i�u�������񂷂�v�u���ǂ�v�{�^���j
		menuret = GFL_UI_TP_HitTrg( PartnerTouchTbl );
		if(menuret==0){
			SubSeq_MessagePrint( wk, msg_gtc_01_017, 1, 0, 0x0f0f );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_YESNO );
			PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
			OS_Printf("�����^�b�`%d\N", result);
		}else if(menuret==1){
			// ��ʂ��炵���t�F�[�h�J�n(��������ʂɖ߂�j
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
						WIPE_FADE_BLACK, EXCHANGE_SCREEN_SYNC, 1, HEAPID_WORLDTRADE );
			wk->subprocess_seq  = SUBSEQ_RETURN_SCREEN1;
			PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
			OS_Printf("�L�����Z���^�b�`%d\N", result);
		}

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
	// �͂��E�������\���ŉ�ʂ�����Ă���̂Łu�ق����|�P�v�u�Z��ł�ꏊ�v�����ĕ`��
//	PokeLabelPrint( wk->MsgManager, wk->InfoWin[7] );
//	ChangePage( wk );


	GFL_CLACT_WK_SetDrawEnable( wk->PartnerCursorActWork, 0 );

    OS_TPrintf("aaa mode = %d\n", wk->sub_process_mode);
	if( wk->sub_process_mode==MODE_PARTNER_RETURN ){
//		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
//					WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
    }
    else if(wk->sub_process_mode==MODE_EXCHANGE_SELECT){
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
					WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
	}else{
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
					WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
	}
#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_partner.c [607] M ********************\n" );
#endif
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
//	wk->YesNoMenuWork = WorldTrade_BmpWinYesNoMake( WORLDTRADE_YESNO_PY1, YESNO_OFFSET );
//	wk->tss = WorldTrade_TouchWinYesNoMake(  WORLDTRADE_YESNO_PY2, YESNO_OFFSET, 8 );
	wk->tss = WorldTrade_TouchWinYesNoMakeEx(  WORLDTRADE_YESNO_PY2, 
											  YESNO_OFFSET, 3, GFL_BG_FRAME0_S, 1 );
	wk->subprocess_seq = SUBSEQ_YESNO_SELECT;

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
		// �������o���|�P�����̑I����
		TOUCH_SW_FreeWork( wk->tss );
		wk->subprocess_seq  = SUBSEQ_END;
		WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, MODE_EXCHANGE_SELECT );
		PokeLabelPrint( wk->MsgManager, &wk->InfoWin[P_INFOWIN_HOSII_POKEMON], msg_gtc_04_008, &wk->print );	// �~�����|�P����
		PokeLabelPrint( wk->MsgManager, &wk->InfoWin[P_INFOWIN_SUNDEIRUBASHO], msg_gtc_04_012, &wk->print );	// �Z��ł���ꏊ
		ChangePage( wk );
	}else if(ret==TOUCH_SW_RET_NO){
		// ��������I���ɖ߂�
		TOUCH_SW_FreeWork( wk->tss );
		//���b�Z�[�W�E�B���h�E�N���A
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );

		wk->subprocess_seq  = SUBSEQ_MAIN;
		TouchPrint( wk->MsgManager, &wk->MenuWin[MENU_EXCHANGE_WIN], msg_gtc_04_019, &wk->print );	// �u�������񂷂�v
		TouchPrint( wk->MsgManager, &wk->MenuWin[MENU_BACK_WIN],     msg_gtc_05_014, &wk->print );	// �u���ǂ�v
	}
	
	
/*
	int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_WORLDTRADE );

	if(ret!=BMPMENU_NULL){
		if(ret==BMPMENU_CANCEL){
			// ����������������
			wk->subprocess_seq  = SUBSEQ_END;
			WorldTrade_SubProcessChange( wk, WORLDTRADE_SEARCH, 0 );
			PokeLabelPrint( wk->MsgManager, &wk->InfoWin[P_INFOWIN_HOSII_POKEMON] );
			ChangePage( wk );
		}else{
			// �������o���|�P�����̑I����
			wk->subprocess_seq  = SUBSEQ_END;
			WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, MODE_EXCHANGE_SELECT );
			PokeLabelPrint( wk->MsgManager, &wk->InfoWin[P_INFOWIN_HOSII_POKEMON] );
			ChangePage( wk );
		}
	}
*/
	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   ���E�ŏ��y�[�W��؂�ւ���
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void ChangePage( WORLDTRADE_WORK *wk )
{
	GFL_ARC_UTIL_TransVramScreen(   
		ARCID_WORLDTRADE_GRA, 
		NARC_worldtrade_searchpoke0_lz_nscr, 
		 GFL_BG_FRAME1_M, 0, 32*24*2, 1, HEAPID_WORLDTRADE);

	// ����̓y�[�W�؂�ւ��ł͂Ȃ��S���\������
	// ����ł���΂���
	TrainerInfoPrint( &wk->InfoWin[P_INFOWIN_1], wk->InfoString[0], wk->InfoString[1],&wk->print );

	// �ق����|�P�����̏��
	WantPokeInfoPrint( &wk->InfoWin[P_INFOWIN_WANT_POKE], wk->MsgManager, wk->MonsNameManager, 
						&wk->DownloadPokemonData[wk->TouchTrainerPos].wantSimple,&wk->print);

/*
	if(wk->PartnerPageInfo==0){
		// �ق����|�P�����̏��
		GFL_ARC_UTIL_TransVramScreen(   
			ARCID_WORLDTRADE_GRA, 
			NARC_worldtrade_searchpoke0_lz_nscr, 
			 GFL_BG_FRAME1_M, 0, 32*24*2, 1, HEAPID_WORLDTRADE);

		WantPokeInfoPrint( &wk->InfoWin[P_INFOWIN_1], wk->MsgManager, wk->MonsNameManager, 
						&wk->DownloadPokemonData[wk->TouchTrainerPos].wantSimple);
	}else{
		// ����ł���΂���
		GFL_ARC_UTIL_TransVramScreen(   
			ARCID_WORLDTRADE_GRA, 
			NARC_worldtrade_searchpoke1_lz_nscr, 
			 GFL_BG_FRAME1_M, 0, 32*24*2, 1, HEAPID_WORLDTRADE);

		TrainerInfoPrint( &wk->InfoWin[P_INFOWIN_1], wk->InfoString[0], wk->InfoString[1] );
	}
*/	
	// BMPWIN�̓��e�ύX�������ōs��

}	



//------------------------------------------------------------------
/**
 * @brief   ���y�[�W��؂�ւ���
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_PageChange( WORLDTRADE_WORK *wk)
{
	ChangePage( wk );
	wk->subprocess_seq = SUBSEQ_MAIN;

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   ������ʂ���ړ����Ă������̃A�j���[�V����
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_ExchangeScreen2( WORLDTRADE_WORK *wk )
{
	wk->DrawOffset++;

	OS_Printf("DrawOffset = %d\n", wk->DrawOffset);
	if(WIPE_SYS_EndCheck()){
		wk->DrawOffset = 0;
		wk->subprocess_seq = SUBSEQ_MAIN;
		GFL_CLACT_WK_SetDrawEnable( wk->PartnerCursorActWork, 1 );

	}
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ������ʂɖ߂�Ƃ��̃A�j���[�V����
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_ReturnScreen1( WORLDTRADE_WORK *wk )
{
	wk->DrawOffset--;
	if(WIPE_SYS_EndCheck()){
		// �T�u��ʂ���E���C����ʂ���
		GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );

		wk->subprocess_seq  = SUBSEQ_END;
		WorldTrade_SubProcessChange( wk, WORLDTRADE_SEARCH, MODE_PARTNER_RETURN );

		wk->DrawOffset = 16;

//		wk->subprocess_seq = SUBSEQ_RETURN_SCREEN2;

		// ���̐��worldtrade_search.c�ōs��
	}
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
	if( GF_MSG_PrintEndCheck( &wk->print )==0){
		wk->subprocess_seq = wk->subprocess_nextseq;
	}
	return SEQ_MAIN;

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
	
//	tempbuf = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WORLDTRADE );
	GFL_MSG_GetString(  wk->MsgManager, msgno, wk->TalkString );
//	WORDSET_ExpandStr( wk->WordSet, wk->TalkString, tempbuf );
//	GFL_STR_DeleteBuffer(tempbuf);

	// ��b�E�C���h�E�g�`��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin),  0x0f0f );
	BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, WORLDTRADE_MESFRAME_CHR, WORLDTRADE_MESFRAME_SUB_PAL );

	// ������`��J�n
	GF_STR_PrintSimple( wk->MsgWin, FONT_TALK, wk->TalkString, 0, 0, &wk->print);
	GFL_BMPWIN_MakeTransWindow( wk->MsgWin );


}



//------------------------------------------------------------------
/**
 * @brief   �u�ق����ۂ�����v�E�u����ł���ꏊ�v�`��
 *
 * @param   win[]		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void PokeLabelPrint( GFL_MSGDATA *MsgManager, GFL_BMPWIN * win[], int msg, WT_PRINT *print )
{
	STRBUF *strbuf;
	
	strbuf = GFL_MSG_CreateString( MsgManager, msg  );

	WorldTrade_SysPrint( win[0], strbuf, 0, 2, 0, PRINTSYS_LSB_Make(15,2,0), print );
	
	GFL_STR_DeleteBuffer( strbuf );

}

//------------------------------------------------------------------
/**
 * @brief   �u�ق����ۂ�����v�E�u����ł���ꏊ�v�`��
 *
 * @param   win[]		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void TouchPrint( GFL_MSGDATA *MsgManager, GFL_BMPWIN * win[], int msg, WT_PRINT *print )
{
	STRBUF *strbuf;
	
	strbuf = GFL_MSG_CreateString( MsgManager, msg  );

	WorldTrade_TouchPrint( win[0], strbuf, 0, 0, 1, PRINTSYS_LSB_Make(15,2,0), print );
	GFL_BMPWIN_MakeTransWindow( win[0] );
	
	GFL_STR_DeleteBuffer( strbuf );
	
}

//------------------------------------------------------------------
/**
 * @brief   �ق����|�P�����̕`��
 *
 * @param   win[]		
 * @param   gtcmsg		
 * @param   monsname		
 * @param   dtp		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void WantPokeInfoPrint( GFL_BMPWIN * win[], GFL_MSGDATA *gtcmsg, GFL_MSGDATA *monsname, Dpw_Tr_PokemonSearchData *dtsd, WT_PRINT *print )
{
	// �ق����|�P�����`��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[0]), 0x0000 );
	WorldTrade_PokeNamePrintNoPut( win[0], monsname, dtsd->characterNo,  0, PRINTSYS_LSB_Make(1,2,0),print );

	// ����
	WorldTrade_SexPrintNoPut( win[0], gtcmsg, dtsd->gender, 0, 3+13*5+5, 0, PRINTSYS_LSB_Make(1,2,0),print );


	// ���x���w��
	// �~�������x���\��
	WorldTrade_WantLevelPrint_XY( win[0], gtcmsg, 
		WorldTrade_LevelTermGet(dtsd->level_min, dtsd->level_max, LEVEL_PRINT_TBL_DEPOSIT), 
		0, 13*8, 0, PRINTSYS_LSB_Make(1,2,0), LEVEL_PRINT_TBL_DEPOSIT, print );
	
}

//------------------------------------------------------------------
/**
 * @brief   �g���[�i�[�̍��E�n���`��
 *
 * @param   win[]		
 * @param   strbuf1		
 * @param   strbuf2		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void TrainerInfoPrint( GFL_BMPWIN *win[], STRBUF *strbuf1, STRBUF *strbuf2, WT_PRINT *print )
{
	
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[0]), 0x0000 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[1]), 0x0000 );

	if(strbuf1!=NULL){
		WorldTrade_SysPrint( win[0], strbuf1, 0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	}
	if(strbuf2!=NULL){
		WorldTrade_SysPrint( win[1], strbuf2, 0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	}
	
}


//------------------------------------------------------------------
/**
 * @brief   �㉺��ʑS�̂��Y�����iVBlank���ŌĂяo���j
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void SlideScreenVFunc( void *p )
{
	WORLDTRADE_WORK *wk = (WORLDTRADE_WORK*)p;
	int i;
	
	GFL_BG_SetScroll(  GFL_BG_FRAME0_M, GFL_BG_SCROLL_Y_SET, wk->DrawOffset );
	GFL_BG_SetScroll(  GFL_BG_FRAME1_M, GFL_BG_SCROLL_Y_SET, wk->DrawOffset );
	GFL_BG_SetScroll(  GFL_BG_FRAME2_M, GFL_BG_SCROLL_Y_SET, wk->DrawOffset );
	GFL_BG_SetScroll(  GFL_BG_FRAME3_M, GFL_BG_SCROLL_Y_SET, wk->DrawOffset );

	GFL_BG_SetScroll(  GFL_BG_FRAME0_S, GFL_BG_SCROLL_Y_SET, -wk->DrawOffset );
	GFL_BG_SetScroll(  GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, -wk->DrawOffset-32 );
	GFL_BG_SetScroll(  GFL_BG_FRAME2_S, GFL_BG_SCROLL_Y_SET, -wk->DrawOffset );
	GFL_BG_SetScroll(  GFL_BG_FRAME3_S, GFL_BG_SCROLL_Y_SET, -wk->DrawOffset );

}
