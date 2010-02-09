//============================================================================================
/**
 * @file	worldtrade_mypoke.c
 * @brief	���E���������|�P�����{�����
 * @author	Akito Mori
 * @date	06.04.16
 */
//============================================================================================
#include <gflib.h>
#include <dwc.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "arc_def.h"
#include <dpw_tr.h>
#include "print/wordset.h"
#include "message.naix"
#include "system/wipe.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"
#include "savedata/wifilist.h"
#include "item/item.h"
#include "poke_tool/pokeparty.h"

#include "net_app/worldtrade.h"
#include "worldtrade_local.h"

#include "msg/msg_wifi_lobby.h"
#include "msg/msg_wifi_gts.h"

#include "worldtrade.naix"			// �O���t�B�b�N�A�[�J�C�u��`

#include "debug/debug_str_conv.h"


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
/*** �֐��v���g�^�C�v ***/
static void BgInit( void );
static void BgExit( void );
static void BgGraphicSet( WORLDTRADE_WORK *wk );
static void BmpWinInit( WORLDTRADE_WORK *wk );
static void BmpWinDelete( WORLDTRADE_WORK *wk );
static void InitWork( WORLDTRADE_WORK *wk );
static void FreeWork( WORLDTRADE_WORK *wk );
static void SetCellActor(WORLDTRADE_WORK *wk);
static void DelCellActor(WORLDTRADE_WORK *wk);
static void WantPokePrintReWrite( WORLDTRADE_WORK *wk );

static int SubSeq_Start( WORLDTRADE_WORK *wk);
static int SubSeq_Main( WORLDTRADE_WORK *wk);
static int SubSeq_End( WORLDTRADE_WORK *wk);
static int SubSeq_MessageWait( WORLDTRADE_WORK *wk );
static int SubSeq_Message1MinWait( WORLDTRADE_WORK *wk );
static int SubSeq_YesNo( WORLDTRADE_WORK *wk);
static int SubSeq_YesNoSelect( WORLDTRADE_WORK *wk);
static int SubSeq_SelectList( WORLDTRADE_WORK *wk);
static int SubSeq_SelectWait( WORLDTRADE_WORK *wk);


static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat, POKEMON_PARAM *pp );


enum{
	SUBSEQ_START=0,
	SUBSEQ_MAIN,
	SUBSEQ_END,
	SUBSEQ_MES_WAIT,
	SUBSEQ_MES_1MIN_WAIT,
	SUBSEQ_YESNO,
	SUBSEQ_YESNO_SELECT,
	SUBSEQ_SELECT_LIST,
	SUBSEQ_SELECT_WAIT,
};

static int (*Functable[])( WORLDTRADE_WORK *wk ) = {
	SubSeq_Start,			// SUBSEQ_START=0,
	SubSeq_Main,            // SUBSEQ_MAIN,
	SubSeq_End,             // SUBSEQ_END,
	SubSeq_MessageWait,     // SUBSEQ_MES_WAIT
	SubSeq_Message1MinWait,	// SUBSEQ_MES_1MIN_WAIT,
	SubSeq_YesNo,			// SUBSEQ_YESNO
	SubSeq_YesNoSelect,		// SUBSEQ_YESNO_SELECT
	SubSeq_SelectList,		// SUBSEQ_SELECT_LIST,
	SubSeq_SelectWait,		// SUBSEQ_SELECT_WAIT,
};

#define INFO_TEXT_WORD_NUM	(10*2)
#define MESSAGE_WAIT_1MIN	( 45 )



enum{
	INFOWIN_NICKNAME=0,
	INFOWIN_TRIBENAME,
	INFOWIN_REBERU,
	INFOWIN_LEVEL,
	INFOWIN_MOTIMONO,
	INFOWIN_ITEMNAME,
	INFOWIN_NAMELABEL,
	
	INFOWIN_AZUKETAHITO,
	INFOWIN_TRAINERNAME,
	INFOWIN_HOSII_POKEMON,
	INFOWIN_WANTTRIBE,
	INFOWIN_WANTLEVEL,
	INFOWIN_OYA,
	INFOWIN_PARENTNAME,
};

// ���\���p��BMPWIN�̐�
#define INFOW_BMPWIN_MAX	( 14 )

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
int WorldTrade_MyPoke_Init(WORLDTRADE_WORK *wk, int seq)
{
	POKEMON_PARAM *pp;
	
	// ���[�N������
	InitWork( wk );
	
	// BG�ݒ�
	BgInit(  );

	// 3D�ݒ�
	WorldTrade_MyPoke_G3D_Init( wk );
	{	
		VecFx32	pos;
		pos.x	= 13<<FX32_SHIFT;
		pos.y	= 7<<FX32_SHIFT;
		pos.z	= 0;
		wk->pokeMcss	= WorldTrade_MyPoke_MCSS_Create( wk, (POKEMON_PASO_PARAM*)PP_GetPPPPointerConst((POKEMON_PARAM*)wk->UploadPokemonData.postData.data), &pos );
	}

	// BG�O���t�B�b�N�]��
	BgGraphicSet( wk );

	// BMPWIN�m��
	BmpWinInit( wk );

	SetCellActor(wk);

	// �����̃|�P�����̏��
	WorldTrade_PokeInfoPrint( 	wk->MsgManager, wk->MonsNameManager, wk->WordSet, &wk->InfoWin[INFOWIN_NICKNAME], 
					(POKEMON_PASO_PARAM*)PP_GetPPPPointerConst((POKEMON_PARAM*)wk->UploadPokemonData.postData.data),
					&wk->UploadPokemonData.postSimple, &wk->print );

	// ��������̕\��
	pp = (POKEMON_PARAM *)wk->UploadPokemonData.postData.data;
	WorldTrade_PokeInfoPrint2( wk->MsgManager, &wk->InfoWin[INFOWIN_AZUKETAHITO], 
								wk->UploadPokemonData.name, pp, &wk->InfoWin[INFOWIN_OYA], &wk->print);

#ifdef PM_DEBUG
	{	
		static char string[256]	= {0};
		/*
		STD_ConvertStringUnicodeToSjis( string, NULL,
                                         wk->UploadPokemonData.name, NULL,
                                         NULL );
    */
    DEB_STR_CONV_StrcodeToSjis( wk->UploadPokemonData.name , string , 128 );
		OS_Printf( "�e�� %s\n", string );
	}
#endif //PM_DEBUG

	// �ق����|�P�����̏���
	WodrldTrade_MyPokeWantPrint( wk->MsgManager, wk->MonsNameManager, wk->WordSet, 
				&wk->InfoWin[INFOWIN_HOSII_POKEMON], 
				wk->UploadPokemonData.wantSimple.characterNo,
				wk->UploadPokemonData.wantSimple.gender,
				WorldTrade_LevelTermGet(wk->UploadPokemonData.wantSimple.level_min,wk->UploadPokemonData.wantSimple.level_max, LEVEL_PRINT_TBL_DEPOSIT),&wk->print);

	// �|�P�����摜�]��
	WorldTrade_TransPokeGraphic( (POKEMON_PARAM*)wk->UploadPokemonData.postData.data );
	
	// �Q��ڈȍ~
	// ���C�v�t�F�[�h�J�n�i����ʁj
	WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_mypoke.c [152] M ********************\n" );
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
int WorldTrade_MyPoke_Main(WORLDTRADE_WORK *wk, int seq)
{
	int ret;
	
	ret = (*Functable[wk->subprocess_seq])( wk );

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
int WorldTrade_MyPoke_End(WORLDTRADE_WORK *wk, int seq)
{
	DelCellActor(wk);

	FreeWork( wk );
	
	BmpWinDelete( wk );
	
	WorldTrade_MyPoke_MCSS_Delete( wk, wk->pokeMcss );
	WorldTrade_MyPoke_G3D_Exit( wk );

	BgExit(  );

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
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_3D,
		};
		GFL_BG_SetBGMode( &BGsys_data );
		GFL_DISP_GXS_SetVisibleControl(
		    GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_OBJ, VISIBLE_ON);
	}

	// ���C����ʃe�L�X�g��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,GX_BG_EXTPLTT_01,
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
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M, TRUE );
	}


	// ���\����ʃe�L�X�g��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME3_M );
		GFL_BG_SetVisible( GFL_BG_FRAME3_M, TRUE );
	}

	GFL_BG_SetClearCharacter( GFL_BG_FRAME2_M, 32, 0, HEAPID_WORLDTRADE );
	GFL_BG_SetClearCharacter( GFL_BG_FRAME3_M, 32, 0, HEAPID_WORLDTRADE );


	// �T�u��ʏ�����
	WorldTrade_SubLcdBgInit( 0, 0 );

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
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
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
	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_WORLDTRADE_GRA, HEAPID_WORLDTRADE );

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette(    p_handle, NARC_worldtrade_poke_view_nclr, PALTYPE_MAIN_BG, 0, 16*3*2,  HEAPID_WORLDTRADE);
	GFL_ARCHDL_UTIL_TransVramPalette(    p_handle, NARC_worldtrade_traderoom_nclr, PALTYPE_SUB_BG,  0, 16*8*2,  HEAPID_WORLDTRADE);
	
	// ��b�t�H���g�p���b�g�]��
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );
  //	TalkFontPaletteLoad( PALTYPE_SUB_BG,  WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

	// ��b�E�C���h�E�O���t�B�b�N�]��
	BmpWinFrame_GraphicSet(	 GFL_BG_FRAME2_M, WORLDTRADE_MESFRAME_CHR, 
						WORLDTRADE_MESFRAME_PAL,  CONFIG_GetWindowType(wk->param->config), HEAPID_WORLDTRADE );

	BmpWinFrame_GraphicSet(	 GFL_BG_FRAME2_M, WORLDTRADE_MENUFRAME_CHR,
						WORLDTRADE_MENUFRAME_PAL, 0, HEAPID_WORLDTRADE );



	// ���C�����BG1�L�����]��
	GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_worldtrade_poke_view_lz_ncgr,  GFL_BG_FRAME1_M, 0, 16*5*0x20, 1, HEAPID_WORLDTRADE);

	// ���C�����BG1�X�N���[���]��
	GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_worldtrade_mypoke_lz_nscr,  GFL_BG_FRAME1_M, 0, 32*24*2, 1, HEAPID_WORLDTRADE);


	// �T�u���BG1�L�����]��
	//GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_worldtrade_traderoom_lz_ncgr,  GFL_BG_FRAME1_S, 0, 32*21*0x40, 1, HEAPID_WORLDTRADE);

	// �T�u���BG1�X�N���[���]��
	//GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_worldtrade_traderoom_lz_nscr,  GFL_BG_FRAME1_S, 0, 32*24*2, 1, HEAPID_WORLDTRADE);

	// ��b�t�H���g�p���b�g�]��
	TalkFontPaletteLoad( PALTYPE_SUB_BG, WORLDTRADE_SUB_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

	WorldTrade_SubLcdWinGraphicSet( wk );   // �g���[�h���[���E�C���h�E�]��

	GFL_ARC_CloseDataHandle( p_handle );

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
//WorldTrade_MakeCLACT( &add,  wk, &wk->clActHeader_main, NNS_G2D_VRAM_TYPE_2DMAIN );

	GFL_STD_MemClear( &add, sizeof(GFL_CLWK_DATA) );

	// �����̃|�P�����\��
	add.pos_x = 208;
	add.pos_y = 58;
	wk->PokemonActWork = GFL_CLACT_WK_Create( wk->clactSet,
			wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
	GFL_CLACT_WK_SetAutoAnmFlag(wk->PokemonActWork,1);
	GFL_CLACT_WK_SetAnmSeq( wk->PokemonActWork, 37 );
	GFL_CLACT_WK_SetBgPri(wk->PokemonActWork, 1 );
	GFL_CLACT_WK_SetDrawEnable(wk->PokemonActWork, 0 );
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

#define LINE_MESSAGE_OFFSET   ( WORLDTRADE_MENUFRAME_CHR + MENU_WIN_CGX_SIZ )
#define SELECT_MENU_OFFSET    ( LINE_MESSAGE_OFFSET + LINE_TEXT_SX*LINE_TEXT_SY )
#define INFO_TEXT_OFFSET	  ( SELECT_MENU_OFFSET + SELECT_MENU_SX*SELECT_MENU_SY )


static const info_bmpwin_table[][4]={
	{   1,  1,  9,  2, },	// �|�P�����̃j�b�N�l�[��
	{   8,  4,  8,  2, },	// �|�P�����̎푰��
	{  11,  1,  3,  2, },	// �u���x���v
	{  14,  1,  3,  2, },	// ���x��
	{   1, 10,  6,  2, },	// �u�������́v
	{   8, 10, 11,  2, },	// �����A�C�e����
	{   1,  4,  6,  2, },	// �u�Ȃ܂��v
	{   1, 13,  9,  2, },	//�u���������ЂƁv
	{  11, 13,  8,  2, },	// �g���[�i�[��
	{   1, 16, 12,  2, },	// �u�ق����|�P�����v
	{   1, 18, 10,  2, },	// �|�P�����푰��
	{  11, 18, 12,  2, },	// ���x���w��
	{   1,  7,  4,  2, },	// �u����v
	{   8,  7,  8,  2, },	// �e��
};

// �͂��E��������BMPWIN�̈�͍Ō�ɂ����Ă������̂����A
// ���E�C���h�E�̑�������߂Ȃ��̂ŁA200�L�������炵�Ă��� >> 216�ɕύX�i���̒l���K���j
#define YESNO_OFFSET 		   ( SELECT_MENU_OFFSET + SELECT_MENU_SX*SELECT_MENU_SY+216 + 12)

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

	wk->MsgWin	= GFL_BMPWIN_CreateFixPos( GFL_BG_FRAME2_M,
		LINE_TEXT_X, LINE_TEXT_Y, LINE_TEXT_SX, LINE_TEXT_SY, 
		WORLDTRADE_TALKFONT_PAL,  LINE_MESSAGE_OFFSET );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0x0000 );
	GFL_BMPWIN_MakeTransWindow( wk->MsgWin );

	// BMPMENU�p�̗̈悪�����ɂ���
	wk->MenuWin[0]	= GFL_BMPWIN_CreateFixPos( GFL_BG_FRAME2_M,
		SELECT_MENU_X, SELECT_MENU_Y, SELECT_MENU_SX, SELECT_MENU_SY, 
		WORLDTRADE_TALKFONT_PAL,  SELECT_MENU_OFFSET );	
	GFL_BMPWIN_MakeTransWindow( wk->MenuWin[0] );

	// BG0��BMPWIN���E�C���h�E�m��
	{
		int i, offset;

		offset = INFO_TEXT_OFFSET;
		for(i=0;i<INFOW_BMPWIN_MAX;i++){
			wk->InfoWin[i] = GFL_BMPWIN_CreateFixPos( GFL_BG_FRAME3_M,
					info_bmpwin_table[i][0], 
					info_bmpwin_table[i][1], 
					info_bmpwin_table[i][2], 
					info_bmpwin_table[i][3], 
					WORLDTRADE_TALKFONT_PAL,  offset );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin[i]), 0x0000 );
			offset += info_bmpwin_table[i][2]*info_bmpwin_table[i][3];
			GFL_BMPWIN_MakeTransWindow( wk->InfoWin[i] );
		}
	}
	
	// �T�u��ʂ�GTS�����pBMPWIN���m�ۂ���
	WorldTrade_SubLcdExpainPut( wk, EXPLAIN_YOUSU );
	

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
  //BMP�������O�Ɏc�����X�g���[��������
  WT_PRINT_ClearBuffer( &wk->print );
	
	GFL_BMPWIN_Delete( wk->ExplainWin );
	GFL_BMPWIN_Delete( wk->MsgWin );
	GFL_BMPWIN_Delete( wk->MenuWin[0] );
	{
		int i;
		for(i=0;i<INFOW_BMPWIN_MAX;i++){
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

	// ������o�b�t�@�쐬
	wk->TalkString  = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WORLDTRADE );

	// POKeMON GLOBAL TRADING SYSTEM
	wk->TitleString = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_02_001 );

	for(i=0;i<10;i++){
		wk->InfoString[i]  = GFL_STR_CreateBuffer( INFO_TEXT_WORD_NUM, HEAPID_WORLDTRADE );
	}
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
	for(i=0;i<10;i++){
		GFL_STR_DeleteBuffer( wk->InfoString[i] );
	}
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
 * @brief   �T�u�v���Z�X�V�[�P���X�X�^�[�g����
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_Start( WORLDTRADE_WORK *wk)
{
	// GTC�ւ悤�����I
//	SubSeq_MessagePrint( wk, msg_wifilobby_028, 1, 0, 0x0f0f );
//	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
	wk->subprocess_seq = SUBSEQ_MAIN;;


	return SEQ_MAIN;
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
//	if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE){
//		SubSeq_MessagePrint( wk, msg_wifilobby_009, 1, 0, 0x0f0f );
//		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_YESNO );
//	}

	if((GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE) || (GFL_UI_TP_GetTrg())){
		// �u�����������ǂ����܂����H�v
		SubSeq_MessagePrint( wk, msg_gtc_01_006, 1, 0, 0x0f0f,(POKEMON_PARAM*)wk->UploadPokemonData.postData.data );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_SELECT_LIST );
		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
	}else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
		wk->subprocess_seq  = SUBSEQ_END;
		WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
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
	WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_mypoke.c [605] M ********************\n" );
#endif
	wk->subprocess_seq = 0;
	
	return SEQ_FADEOUT;
}

//------------------------------------------------------------------
/**
 * @brief   �������܂����H�͂��E������
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_YesNo( WORLDTRADE_WORK *wk)
{
//	wk->YesNoMenuWork = WorldTrade_BmpWinYesNoMake( WORLDTRADE_YESNO_PY1, YESNO_OFFSET );
 	wk->tss	= WorldTrade_TouchWinYesNoMakeEx( WORLDTRADE_YESNO_PY1, YESNO_OFFSET, 8, GFL_BG_FRAME2_M, 1 );

	//���̊֐��͒ʏ�BG0�ȊO���p�b�V�u�����邪�A
	//���̃\�[�X�̂݁ABG2�����b�Z�[�W�ʂȂ̂ŁA
	//��[������ĉ��߂�BG2�ȊO���p�b�V�u������
	WorldTrade_ClearPassive();
	WorldTrade_SetPassiveMyPoke(1);

	wk->subprocess_seq = SUBSEQ_YESNO_SELECT;

	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   �������܂����H�͂��E�������I��҂�
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
		// �͂���������
		TOUCH_SW_FreeWork( wk->tss );
		wk->subprocess_seq  = SUBSEQ_END;
		wk->sub_out_flg = 1;
		WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_DOWNLOAD );

		// �͂��E�������Ŕw�i������Ă���̂ōĕ`��
		WantPokePrintReWrite( wk );

	}else if(ret==TOUCH_SW_RET_NO){
		// ������
		TOUCH_SW_FreeWork( wk->tss );
		wk->subprocess_seq = SUBSEQ_START;
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );

		// �͂��E�������Ŕw�i������Ă���̂ōĕ`��
		WantPokePrintReWrite( wk );
	}

/*
	int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_WORLDTRADE );

	if(ret!=BMPMENU_NULL){
		if(ret==BMPMENU_CANCEL){
			// ������
			wk->subprocess_seq = SUBSEQ_START;
			BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );

		}else{
			// �͂���������
			wk->subprocess_seq  = SUBSEQ_END;
			wk->sub_out_flg = 1;
			WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_DOWNLOAD );
		}

		// �͂��E�������Ŕw�i������Ă���̂ōĕ`��
		WantPokePrintReWrite( wk );

	}
*/
	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   �I�����X�g�쐬
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SelectList( WORLDTRADE_WORK *wk )
{

	wk->BmpMenuList = BmpMenuWork_ListCreate( 2, HEAPID_WORLDTRADE );
	// �Ђ��Ƃ�
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_02_016, 1, HEAPID_WORLDTRADE );
	// ���̂܂�
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_02_017, 2, HEAPID_WORLDTRADE );

	// �I���{�b�N�X�Ăяo��
	wk->SelBoxWork = WorldTrade_SelBoxInit( wk, GFL_BG_FRAME2_M, 2, 13 );

	//�p�b�V�u
	WorldTrade_SetPassiveMyPoke(1);

/*
	BMPMENU_HEADER temp;

	temp.font  = FONT_SYSTEM;
	temp.x_max = 1;
	temp.y_max = 2;
	temp.line_spc = 0;
	temp.c_disp_f = 0;
	temp.loop_f   = 0;


	wk->BmpMenuList = BmpMenuWork_ListCreate( 2, HEAPID_WORLDTRADE );

	// �Ђ��Ƃ�
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_02_016, 1 );
	// ���̂܂�
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_02_017, 2 );

	temp.menu = wk->BmpMenuList;
	temp.win  = wk->MenuWin[0];

	// �g�`��
	BmpWinFrame_Write( wk->MenuWin[0], WINDOW_TRANS_ON, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

	// BMP���j���[�J�n
	wk->BmpMenuWork = BmpMenuAddEx( &temp, 9, 0, 0, HEAPID_WORLDTRADE, PAD_BUTTON_CANCEL );

*/

	wk->subprocess_seq = SUBSEQ_SELECT_WAIT;
	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   �u�Ђ��Ƃ�v�u���̂܂܁v�I��҂�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SelectWait( WORLDTRADE_WORK *wk )
{
	u32 ret = SelectBoxMain( wk->SelBoxWork );
	
	if(ret==1){
		// �I���{�b�N�X���
		WorldTrade_SelBoxEnd( wk );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		{
			POKEMON_PARAM *pp = (POKEMON_PARAM *)wk->UploadPokemonData.postData.data;
			// ���������|�P�����̓��[���������Ă��邩�H
			if(WorldTrade_PokemonMailCheck( pp )){
				// �Ă����������ς����Ǝ󂯎��Ȃ�
				if(PokeParty_GetPokeCount(wk->param->myparty)==6){
					SubSeq_MessagePrint( wk, msg_gtc_01_037, 1, 0, 0x0f0f, pp );
					WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
					return SEQ_MAIN;
				}
			}
			
			// ���������Ђ��Ƃ�܂�
			SubSeq_MessagePrint( wk, msg_gtc_01_007, 1, 0, 0x0f0f, pp );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_YESNO );
		}
		WantPokePrintReWrite(wk);
	
	}else if( (ret==2) || (ret==SBOX_SELECT_CANCEL) ){
		// �I���{�b�N�X���
		WorldTrade_SelBoxEnd( wk );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		wk->subprocess_seq  = SUBSEQ_END;
		WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
		WantPokePrintReWrite(wk);
		
	}

/*
	switch(BmpMenuMain( wk->BmpMenuWork )){
	// �u�Ђ��Ƃ�v
	case 1:
		BmpMenuExit( wk->BmpMenuWork, NULL );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		BmpMenuWinClear( wk->MenuWin[0], WINDOW_TRANS_ON );
		{
			POKEMON_PARAM *pp = (POKEMON_PARAM *)wk->UploadPokemonData.postData.data;
			// ���������|�P�����̓��[���������Ă��邩�H
			if(WorldTrade_PokemonMailCheck( pp )){
				// �Ă����������ς����Ǝ󂯎��Ȃ�
				if(PokeParty_GetPokeCount(wk->param->myparty)==6){
					SubSeq_MessagePrint( wk, msg_gtc_01_037, 1, 0, 0x0f0f, pp );
					WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
					return SEQ_MAIN;
				}
			}
			
			// ���������Ђ��Ƃ�܂�
			SubSeq_MessagePrint( wk, msg_gtc_01_007, 1, 0, 0x0f0f, pp );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_YESNO );
		}
		WantPokePrintReWrite(wk);
		break;

	// �u���̂܂܁v
	case 2:case BMPMENU_CANCEL:
		BmpMenuExit( wk->BmpMenuWork, NULL );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		BmpMenuWinClear( wk->MenuWin[0], WINDOW_TRANS_ON );
		wk->subprocess_seq  = SUBSEQ_END;
		 WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
		WantPokePrintReWrite(wk);
		break;
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
	if( GF_MSG_PrintEndCheck( &wk->print )==0){
		wk->subprocess_seq = wk->subprocess_nextseq;
	}
	return SEQ_MAIN;

}

//------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W���o���ĂP�D�T�b�҂�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_Message1MinWait( WORLDTRADE_WORK *wk )
{
	if( GF_MSG_PrintEndCheck( &wk->print )==0){
		wk->wait++;
		if(wk->wait > MESSAGE_WAIT_1MIN){
			wk->wait = 0;
			wk->subprocess_seq = wk->subprocess_nextseq;
		}
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
static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat, POKEMON_PARAM *pp )
{
	// ������擾
	STRBUF *tempbuf;
	
	WORDSET_RegisterPokeMonsName( wk->WordSet, 0, pp );
	tempbuf = GFL_MSG_CreateString(  wk->MsgManager, msgno );
	WORDSET_ExpandStr( wk->WordSet, wk->TalkString, tempbuf );

	// ��b�E�C���h�E�g�`��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin),  0x0f0f );
	BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, WORLDTRADE_MESFRAME_CHR, WORLDTRADE_MESFRAME_PAL );

	// ������`��J�n
	GF_STR_PrintSimple( wk->MsgWin, FONT_TALK, wk->TalkString, 0, 0,&wk->print);
	
	GFL_STR_DeleteBuffer(tempbuf);

}


// ���ʃA�C�R����\�����邽�߂̃J���[�w��
static PRINTSYS_LSB sex_mark_col( int idx )
{
	switch(idx)
	{
	case 0: return 0;
	case 1: return PRINTSYS_LSB_Make(5,6,0);
	case 2: return PRINTSYS_LSB_Make(3,4,0);
	}

	return 0;
}

//------------------------------------------------------------------
/**
 * @brief   ������a���悤�Ƃ���|�P�����̏���`�悷��
 *
 * @param   MsgManager		
 * @param   WordSet		
 * @param   win[]		
 * @param   ppp		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
void WorldTrade_PokeInfoPrint( 	GFL_MSGDATA *MsgManager,
							GFL_MSGDATA *MonsNameManager, 	
							WORDSET *WordSet, 
							GFL_BMPWIN *win[], 	
							POKEMON_PASO_PARAM *ppp,
							Dpw_Tr_PokemonDataSimple *post,
							WT_PRINT *print )
{
	STRBUF *strbuf,	*sexbuf, *levellabel, *levelbuf, *itemlabel, *namelabel;
	STRBUF *namebuf = GFL_STR_CreateBuffer( (BUFLEN_POKEMON_NAME+EOM_SIZE)*2, HEAPID_WORLDTRADE );
	STRBUF *itembuf = GFL_STR_CreateBuffer( (BUFLEN_ITEM_NAME+EOM_SIZE)*2, HEAPID_WORLDTRADE );
	int sex, level,itemno,i,monsno;
	
	// �j�b�N�l�[���E���ʁE���x���E�J���m�n�E�A�C�e���擾
	PPP_Get(ppp, ID_PARA_nickname, namebuf );
	monsno = post->characterNo;
	sex    = post->gender;
	level  = post->level;
	itemno = PPP_Get( ppp, ID_PARA_item, NULL );
	
	OS_Printf("sex=%d, level=%d\n",sex, level);
	
	// �u�������́v�E���ʕ�����E�k�u�D�H�H�H�E�푰���E�A�C�e�����A�����񐶐�
	itemlabel  = GFL_MSG_CreateString( MsgManager, msg_gtc_04_004  );
	sexbuf     = GFL_MSG_CreateString( MsgManager, WorldTrade_SexStringTable[sex]  );
	levellabel = GFL_MSG_CreateString( MsgManager, msg_gtc_05_013  );
	WORDSET_RegisterNumber( WordSet, 3, level, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	levelbuf   = MSGDAT_UTIL_AllocExpandString( WordSet, MsgManager, msg_gtc_05_013_01, HEAPID_WORLDTRADE );
	strbuf     = GFL_MSG_CreateString( MonsNameManager, monsno );
	ITEM_GetItemName( itembuf, itemno, HEAPID_WORLDTRADE );
	namelabel  = GFL_MSG_CreateString( MsgManager, msg_gtc_02_020  );

	// �`��̑O�ɃN���A
	for(i=0;i<6;i++){
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[i]), 0x0000 );
	}

	// �`��
	WorldTrade_SysPrint( win[0], namebuf,   0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	if(sex!=DPW_TR_GENDER_NONE){
		WorldTrade_SysPrint( win[0], sexbuf,   64, 0, 0, sex_mark_col(sex),print );
	}
	WorldTrade_SysPrint( win[1], strbuf,     0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	WorldTrade_SysPrint( win[2], levellabel, 0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	WorldTrade_SysPrint( win[3], levelbuf,   0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	WorldTrade_SysPrint( win[4], itemlabel,  0, 0, 0, PRINTSYS_LSB_Make(15,2,0),print );
	WorldTrade_SysPrint( win[5], itembuf,    0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	WorldTrade_SysPrint( win[6], namelabel,  0, 0, 0, PRINTSYS_LSB_Make(15,2,0),print );

	GFL_STR_DeleteBuffer( itemlabel  );
	GFL_STR_DeleteBuffer( itembuf    );
	GFL_STR_DeleteBuffer( levellabel );
	GFL_STR_DeleteBuffer( levelbuf   );
	GFL_STR_DeleteBuffer( sexbuf     );
	GFL_STR_DeleteBuffer( namebuf    );
	GFL_STR_DeleteBuffer( strbuf     );
	GFL_STR_DeleteBuffer( namelabel  );
}


//------------------------------------------------------------------
/**
 * @brief   ��������̕`��
 *
 * @param   MsgManager		
 * @param   win[]		
 * @param   name		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
void WorldTrade_PokeInfoPrint2( GFL_MSGDATA *MsgManager, GFL_BMPWIN *win[], STRCODE *name, POKEMON_PARAM *pp, GFL_BMPWIN* oya_win[], WT_PRINT *print )
{
	STRBUF *ornerbuf, *ornerlabel;
	STRBUF *oyalabel, *oyabuf;
	
	ornerbuf = GFL_STR_CreateBuffer( (BUFLEN_PERSON_NAME+EOM_SIZE)*2, HEAPID_WORLDTRADE );
	oyabuf = GFL_STR_CreateBuffer((BUFLEN_PERSON_NAME+EOM_SIZE)*2, HEAPID_WORLDTRADE);
	
	ornerlabel = GFL_MSG_CreateString( MsgManager, msg_gtc_02_010  );
	GFL_STR_SetStringCode( ornerbuf, name );
	
	oyalabel = GFL_MSG_CreateString(MsgManager, msg_gtc_pl_oya);
	PP_Get(pp, ID_PARA_oyaname, oyabuf);

	WorldTrade_SysPrint( win[0], ornerlabel, 0, 0, 0, PRINTSYS_LSB_Make(15,2,0), print );
	WorldTrade_SysPrint( win[1], ornerbuf,   0, 0, 0, PRINTSYS_LSB_Make(1,2,0), print  );
	WorldTrade_SysPrint( oya_win[0], oyalabel,   0, 0, 0, PRINTSYS_LSB_Make(15,2,0), print );
	WorldTrade_SysPrint( oya_win[1], oyabuf,   0, 0, 0, PRINTSYS_LSB_Make(1,2,0), print );
	
	GFL_STR_DeleteBuffer( ornerlabel );
	GFL_STR_DeleteBuffer( ornerbuf   );
	GFL_STR_DeleteBuffer( oyalabel );
	GFL_STR_DeleteBuffer( oyabuf   );
}

#define POKEGRA_VRAM_OFFSET	(   (18*32+16)*32 )
#define POKEGRA_VRAM_SIZE	( 0x20*10*10 )

//------------------------------------------------------------------
/**
 * �|�P�����摜��]������
 *
 * @param   pokeno		POKEMON_PARAM
 *
 * @retval  none
 */
//------------------------------------------------------------------
void WorldTrade_TransPokeGraphic( POKEMON_PARAM *pp )
{

//MCSS�ŕ`�悷��悤�ɂȂ����̂Ŏg�p���Ȃ��Ȃ�܂���
#if 0
	SOFT_SPRITE_ARC ssa;
	u8  *char_work = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, POKEGRA_VRAM_SIZE );

	// �|�P�����̉摜��ǂݍ��ށi�������\�t�g�E�F�A�X�v���C�g�p�̃e�N�X�`����ԁj
	PokeGraArcDataGetPP(&ssa, pp, PARA_FRONT);

	// �e�N�X�`����OAM�p�̕��тɕϊ�����
	{
		int rnd    = PP_Get( pp, ID_PARA_personal_rnd, NULL );
		int monsno = PP_Get( pp, ID_PARA_monsno, NULL );
		Ex_ChangesInto_OAM_from_PokeTex(ssa.arc_no, ssa.index_chr, HEAPID_WORLDTRADE, 0, 0, 10, 10, char_work, 
										rnd, 0, PARA_FRONT, monsno);
	}
//	ChangesInto_OAM_from_PokeTex(ssa.arc_no, ssa.index_chr, HEAPID_WORLDTRADE, 0, 0, 10, 10, char_work);
	DC_FlushRange( char_work,POKEGRA_VRAM_SIZE );	// �]���O�Ƀ���������

	// OAM�pVRAM�ɓ]��
	GX_LoadOBJ( char_work, POKEGRA_VRAM_OFFSET, POKEGRA_VRAM_SIZE );

	// �p���b�g�]��
	GFL_ARC_UTIL_TransVramPalette( ssa.arc_no, ssa.index_pal, PALTYPE_MAIN_OBJ, 0x20*13, 32, HEAPID_WORLDTRADE );
	
	// ���[�N���
	GFL_HEAP_FreeMemory(char_work);
#endif 
}


//------------------------------------------------------------------
/**
 * @brief   �ق����|�P�����̕������ĕ`��
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void WantPokePrintReWrite( WORLDTRADE_WORK *wk )
{

	// �ق����|�P�����̏���
	WodrldTrade_MyPokeWantPrint( wk->MsgManager, wk->MonsNameManager, wk->WordSet, 
				&wk->InfoWin[INFOWIN_HOSII_POKEMON], 
				wk->UploadPokemonData.wantSimple.characterNo,
				wk->UploadPokemonData.wantSimple.gender,
				WorldTrade_LevelTermGet(wk->UploadPokemonData.wantSimple.level_min,wk->UploadPokemonData.wantSimple.level_max, LEVEL_PRINT_TBL_DEPOSIT),&wk->print);


}

//=============================================================================
/**
 *	WB�Ń|�P������MCSS�ɂȂ�������3D�����ǉ�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	G3D�֌W��MCSS�̏�����
 *
 *	@param	WORLDTRADE_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
void WorldTrade_MyPoke_G3D_Init( WORLDTRADE_WORK *wk )
{	
		const VecFx32 cam_pos = {FX32_CONST(0.0f),FX32_CONST(0.0f),FX32_CONST(101.0f)};
		const VecFx32 cam_target = {FX32_CONST(0.0f),FX32_CONST(0.0f),FX32_CONST(-100.0f)};
		const VecFx32 cam_up = {0,FX32_ONE,0};
		//�G�b�W�}�[�L���O�J���[
		 const GXRgb edge_color_table[8]=
		{ GX_RGB( 0, 0, 0 ), GX_RGB( 0, 0, 0 ), 0, 0, 0, 0, 0, 0 };

		//G3D�̏�����--------
		GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX256K, GFL_G3D_VMANLNK, GFL_G3D_PLT16K,
									0, HEAPID_WORLDTRADE, NULL );

		//���ˉe�J����
		wk->camera =  GFL_G3D_CAMERA_Create( GFL_G3D_PRJORTH,
																						FX32_ONE*12.0f,
																						0,
																						0,
																						FX32_ONE*16.0f,
																						(FX32_ONE),
																						(FX32_ONE*200),
																						NULL,
																						&cam_pos,
																						&cam_up,
																						&cam_target,
																						HEAPID_WORLDTRADE );

		GFL_G3D_CAMERA_Switching( wk->camera );
		//�G�b�W�}�[�L���O�J���[�Z�b�g
		G3X_SetEdgeColorTable( edge_color_table );
		G3X_EdgeMarking( TRUE );

		GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO , GX_BUFFERMODE_Z );	

		GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, TRUE );


		//MCSS�̏�����--------
		wk->mcssSys = MCSS_Init( 2 , HEAPID_WORLDTRADE );
		MCSS_SetTextureTransAdrs( wk->mcssSys , 0 );
		MCSS_SetOrthoMode( wk->mcssSys );
}

//----------------------------------------------------------------------------
/**
 *	@brief	G3D�֌W��MCSS�̔j��
 *
 *	@param	WORLDTRADE_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
void WorldTrade_MyPoke_G3D_Exit( WORLDTRADE_WORK *wk )
{	
	MCSS_Exit(wk->mcssSys);

	GFL_G3D_CAMERA_Delete(wk->camera);
	GFL_G3D_Exit();
}

//----------------------------------------------------------------------------
/**
 *	@brief	G3D�֌W��MCSS�̕`��
 *
 *	@param	WORLDTRADE_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
void WorldTrade_MyPoke_G3D_Draw( WORLDTRADE_WORK *wk )
{	
	GFL_G3D_DRAW_Start();
	GFL_G3D_DRAW_SetLookAt();
	MCSS_Main( wk->mcssSys );
	MCSS_Draw( wk->mcssSys );
	GFL_G3D_DRAW_End();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����P�̕���MCSS�쐬
 *
 *	@param	WORLDTRADE_WORK *wk		���[�N
 *	@param	VecFx32 *pos					�ʒu
 *
 *	@return	�|�P����1�̕���MCSS
 */
//-----------------------------------------------------------------------------
MCSS_WORK * WorldTrade_MyPoke_MCSS_Create( WORLDTRADE_WORK *wk, POKEMON_PASO_PARAM *ppp, const VecFx32 *pos )
{	
	MCSS_WORK *poke;
	MCSS_ADD_WORK addWork;

	VecFx32 scale = {FX32_ONE*16,FX32_ONE*16,FX32_ONE};

	MCSS_TOOL_MakeMAWPPP( ppp , &addWork , MCSS_DIR_FRONT );
	poke = MCSS_Add( wk->mcssSys , pos->x, pos->y , pos->z, &addWork );
	MCSS_SetScale( poke , &scale );
	return poke;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����P�̕���MCSS�j��
 *
 *	@param	WORLDTRADE_WORK *wk		���[�N
 *	@param	*poke									�j������|�P����
 */
//-----------------------------------------------------------------------------
void WorldTrade_MyPoke_MCSS_Delete( WORLDTRADE_WORK *wk, MCSS_WORK *poke )
{
	MCSS_SetVanishFlag( poke );
	MCSS_Del(wk->mcssSys,poke);
}
