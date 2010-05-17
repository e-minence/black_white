//============================================================================================
/**
 * @file	worldtrade_box.c
 * @brief	���E���������{�b�N�X��ʏ���
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
#include "poke_tool/poke_tool.h"
//#include "poke_tool/pokeicon.h"
#include "poke_tool/pokeparty.h"
#include "item/item.h"

#include "savedata/wifilist.h"

#include "net_app/worldtrade.h"
#include "worldtrade_local.h"

#include "msg/msg_wifi_lobby.h"
#include "msg/msg_wifi_gts.h"
#include "print/printsys.h"

#include "net_app/net_bugfix.h"
#include "poke_tool/monsno_def.h"

#include "worldtrade.naix"			// �O���t�B�b�N�A�[�J�C�u��`

#include "net_app/gts_tool.h"


//============================================================================================
//	�^�錾
//============================================================================================
#define POKEICON_TRANS_CHARA	(4*4)
#define POKEICON_TRANS_SIZE		(POKEICON_TRANS_CHARA*0x20)

typedef struct {
  int vadrs;				// VRAM�A�h���X
  int palno;				// �p���b�g�ԍ�
  GFL_CLWK* icon;			// CLACT
  u8 chbuf[POKEICON_TRANS_SIZE];	// �L�����N�^�o�b�t�@
} WORLDTRADE_POKEBUF;



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
static int SubSeq_Start( WORLDTRADE_WORK *wk);
static int SubSeq_Main( WORLDTRADE_WORK *wk);
static int SubSeq_End( WORLDTRADE_WORK *wk);
static int SubSeq_YesNo( WORLDTRADE_WORK *wk);
static int SubSeq_YesNoSelect( WORLDTRADE_WORK *wk);
static int SubSeq_MessageWait( WORLDTRADE_WORK *wk );
static int SubSeq_MessageClearWait( WORLDTRADE_WORK *wk );
static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat, int winflag );
static void TransPokeIconCharaPal( int pokeno, int form, int gender, int tamago, int no, GFL_CLWK* icon, ARCHANDLE* handle, WORLDTRADE_POKEBUF *pbuf );
static void NowBoxPageInfoGet( WORLDTRADE_WORK *wk, int now);
static int BoxTrayRoundWork( int num, int max, int move );
static void CursorControl( WORLDTRADE_WORK *wk );
static int SubSeq_SelectList( WORLDTRADE_WORK *wk );
static int SubSeq_SelectWait( WORLDTRADE_WORK *wk );
static int SubSeq_ExchangeSelectList( WORLDTRADE_WORK *wk );
static int SubSeq_ExchangeSelectWait( WORLDTRADE_WORK *wk );
static int SubSeq_CBallYesNoSelect( WORLDTRADE_WORK *wk );
static int SubSeq_CBallYesNo( WORLDTRADE_WORK *wk );
static int SubSeq_CBallYesNoMessage( WORLDTRADE_WORK *wk );
static int SubSeq_CBallDepositYesNoMessage( WORLDTRADE_WORK *wk );
static int SubSeq_CBallDepositYesNo( WORLDTRADE_WORK *wk );
static int SubSeq_CBallDepositYesNoSelect( WORLDTRADE_WORK *wk );

static int SubSeq_CancelWait( WORLDTRADE_WORK *wk );


static  int WantPokeCheck(POKEMON_PASO_PARAM *ppp, Dpw_Tr_PokemonSearchData *dtsd);
static void MakeExchangePokemonData( Dpw_Tr_Data *dtd, WORLDTRADE_WORK *wk );
static void PokemonIconSet( POKEMON_PASO_PARAM *paso, GFL_CLWK* icon, GFL_CLWK* itemact, u16 *no, int pos, ARCHANDLE* handle, Dpw_Tr_PokemonDataSimple *dat, WORLDTRADE_POKEBUF *pbuf );
static void* CharDataGetbyHandle( ARCHANDLE *handle, u32 dataIdx, NNSG2dCharacterData** charData, u32 heapID );
static  int CompareSearchData( Dpw_Tr_PokemonDataSimple *poke,  Dpw_Tr_PokemonSearchData *search );
static void PokeIconPalSet( Dpw_Tr_PokemonDataSimple *box, GFL_CLWK* *icon, Dpw_Tr_PokemonSearchData *want, WORLDTRADE_POKEBUF *pbuf);
static  int CheckPocket( POKEPARTY *party, BOX_MANAGER *box,  int  tray, int pos );
static  int PokemonCheck( POKEPARTY *party, BOX_MANAGER *box,  int  tray, int pos  );
static  int ExchangeCheck( WORLDTRADE_WORK *wk );
static  int PokeRibbonCheck( POKEMON_PASO_PARAM *ppp );
static  int PokeNewFormCheck( POKEMON_PASO_PARAM *ppp );
static  int PokeNewItemCheck( POKEMON_PASO_PARAM *ppp );
static void CursorPosPrioritySet( GFL_CLWK* cursor, int pos );

static void CancelFunc( WORLDTRADE_WORK *wk, int mode );

#define WANT_POKE_NO		( 0 )
#define WANT_POKE_OK		( 1 )
#define WANT_POKE_TAMAGO	( 2 )

enum{
	SUBSEQ_START=0,
	SUBSEQ_MAIN,
	SUBSEQ_END,
	SUBSEQ_MES_WAIT,
	SUBSEQ_MES_CLEAR_WAIT,
	SUBSEQ_YESNO,
	SUBSEQ_YESNO_SELECT,
	SUBSEQ_SELECT_LIST,
	SUBSEQ_SELECT_WAIT,
	SUBSEQ_EXCHANGE_SELECT_LIST,
	SUBSEQ_EXCHANGE_SELECT_WAIT,

	SUBSEQ_CBALL_YESNO_MES,
	SUBSEQ_CBALL_YESNO,
	SUBSEQ_CBALL_YESNO_SELECT,

	SUBSEQ_CBALL_DEPOSIT_YESNO_MES,
	SUBSEQ_CBALL_DEPOSIT_YESNO,
	SUBSEQ_CBALL_DEPOSIT_YESNO_SELECT,

	SUBSEQ_CANCEL_WAIT,
};

static int (*Functable[])( WORLDTRADE_WORK *wk ) = {
	SubSeq_Start,			// SUBSEQ_START=0,
	SubSeq_Main,            // SUBSEQ_MAIN,
	SubSeq_End,             // SUBSEQ_END,
	SubSeq_MessageWait,     // SUBSEQ_MES_WAIT
	SubSeq_MessageClearWait,// SUBSEQ_MES_CLEAR_WAIT
	SubSeq_YesNo,			// SUBSEQ_YESNO
	SubSeq_YesNoSelect,		// SUBSEQ_YESNO_SELECT
	SubSeq_SelectList,		// SUBSEQ_SELECT_LIST
	SubSeq_SelectWait,		// SUBSEQ_SELECT_WAIT
	SubSeq_ExchangeSelectList,	// SUBSEQ_EXCHANGE_SELECT_LIST
	SubSeq_ExchangeSelectWait,	// SUBSEQ_EXCHANGE_SELECT_WAIT

	SubSeq_CBallYesNoMessage,	// SUBSEQ_CBALL_YESNO_MES
	SubSeq_CBallYesNo,			// SUBSEQ_CBALL_YESNO
	SubSeq_CBallYesNoSelect,	// SUBSEQ_CBALL_YESNO_SELECT

	SubSeq_CBallDepositYesNoMessage,	// SUBSEQ_CBALL_YESNO_MES
	SubSeq_CBallDepositYesNo,			// SUBSEQ_CBALL_YESNO
	SubSeq_CBallDepositYesNoSelect,	// SUBSEQ_CBALL_YESNO_SELECT


	SubSeq_CancelWait,	// SUBSEQ_CANCEL_WAIT
};

#define BOX_CUROSOR_END_POS			( 30 )
#define BOX_CUROSOR_TRAYNAME_POS	( 31 )


//============================================================================================
//	�v���Z�X�֐�
//============================================================================================

//==============================================================================
/**
 * @brief   ���E�����������ʏ�����
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   seq		�V�[�P���X�i���g�p�j
 *
 * @retval  int		���̃V�[�P���X
 */
//==============================================================================
int WorldTrade_Box_Init(WORLDTRADE_WORK *wk, int seq)
{
	// ���[�N������
	InitWork( wk );

	// ���C����ʂ����ɏo�͂����悤�ɂ���
	GX_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
	
	// BG�ݒ�
	BgInit();
  // �T�u��ʏ�����
  WorldTrade_SubLcdBgInit(wk,0, 0 );

	// BG�O���t�B�b�N�]��
	BgGraphicSet( wk );

	// BMPWIN�m��
	BmpWinInit( wk );

	SetCellActor(wk);

    WorldTrade_SetPartnerExchangePos( wk );

    if(GXS_GetMasterBrightness() == 0){
    	WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
    }
    else{
    	WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
    }

	NowBoxPageInfoGet(wk, wk->BoxTrayNo);
	

	// �ʐM��Ԃ��m�F���ăA�C�R���̕\����ς���
	WorldTrade_WifiIconAdd( wk );

	// �u�c�r�̉���ʂ݂Ăˁv�A�C�R�����B��
//	GFL_CLACT_WK_SetDrawEnable( wk->PromptDsActWork, 0 );
   
	wk->subprocess_seq = SUBSEQ_START;
  wk->sub_display_continue  = FALSE;
	
	return SEQ_FADEIN;
}
//==============================================================================
/**
 * @brief   ���E�����������ʃ��C��
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   seq		�V�[�P���X�i���g�p�j
 *
 * @retval  int		���̃V�[�P���X
 */
//==============================================================================
int WorldTrade_Box_Main(WORLDTRADE_WORK *wk, int seq)
{
	int ret;

	
	ret = (*Functable[wk->subprocess_seq])( wk );

	return ret;
}


//==============================================================================
/**
 * @brief   ���E�����������ʏI��
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   seq		�V�[�P���X�i���g�p�j
 *
 * @retval  int		���̃V�[�P���X
 */
//==============================================================================
int WorldTrade_Box_End(WORLDTRADE_WORK *wk, int seq)
{
	//WirelessIconEasyEnd();

    if(GXS_GetMasterBrightness() != 0){
        WorldTrade_SetPartnerExchangePosIsReturns( wk );
    }

	DelCellActor(wk);

	FreeWork( wk );
	
	BmpWinDelete( wk );
	
	BgExit();
	// �T�u��ʂa�f�����
	WorldTrade_SubLcdBgExit( wk );

	// �uDS�̉���ʂ��݂Ă˃A�C�R���v��\��
	GFL_CLACT_WK_SetDrawEnable( wk->PromptDsActWork, 0 );
	
	WorldTrade_SubProcessUpdate( wk );
	
	return SEQ_INIT;
}


//--------------------------------------------------------------------------------------------
/**
 * BG�ݒ�
 *
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
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME0_M );
		GFL_BG_SetVisible( GFL_BG_FRAME0_M, TRUE );
	}

	// ���C����ʃ��j���[��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M, TRUE );
	}

	// ���C����ʔw�i��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME2_M );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M, TRUE );
	}

	// �{�b�N�X���\����ʃe�L�X�g��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe000, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME3_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME3_M );
		GFL_BG_SetVisible( GFL_BG_FRAME3_M, TRUE );
	}



	GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 32, 0, HEAPID_WORLDTRADE );
	GFL_BG_SetClearCharacter( GFL_BG_FRAME3_M, 32, 0, HEAPID_WORLDTRADE );

}

//--------------------------------------------------------------------------------------------
/**
 * BG���
 *
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( void )
{


	// ���C����ʂa�f�����
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );

}


//--------------------------------------------------------------------------------------------
/**
 * �O���t�B�b�N�f�[�^�Z�b�g
 *
 * @param	wk		GTS��ʃ��[�N
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgGraphicSet( WORLDTRADE_WORK * wk )
{
	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_WORLDTRADE_GRA, HEAPID_WORLDTRADE );

	// �㉺��ʂa�f�p���b�g�]��
	GFL_ARCHDL_UTIL_TransVramPalette(    p_handle, NARC_worldtrade_mybox_nclr, PALTYPE_MAIN_BG, 0, 16*3*2,  HEAPID_WORLDTRADE);
	
	// ��b�t�H���g�p���b�g�]��
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );
	// TalkFontPaletteLoad( PALTYPE_SUB_BG,  WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

	// ��b�E�C���h�E�O���t�B�b�N�]��
	BmpWinFrame_GraphicSet(	GFL_BG_FRAME0_M, WORLDTRADE_MESFRAME_CHR, 
						WORLDTRADE_MESFRAME_PAL,  0, HEAPID_WORLDTRADE );

	BmpWinFrame_GraphicSet(	GFL_BG_FRAME0_M, WORLDTRADE_MENUFRAME_CHR,
						WORLDTRADE_MENUFRAME_PAL, 0, HEAPID_WORLDTRADE );



	// ���C�����BG1�L�����]��
	GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_worldtrade_mybox_lz_ncgr, GFL_BG_FRAME1_M, 0, 16*5*0x20, 1, HEAPID_WORLDTRADE);

	// ���C�����BG1�X�N���[���]��
	GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_worldtrade_mybox_lz_nscr, GFL_BG_FRAME1_M, 0, 32*24*2, 1, HEAPID_WORLDTRADE);

	// ���C�����BG2�X�N���[���]��
	GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_worldtrade_title_base_lz_nscr, GFL_BG_FRAME2_M, 0, 32*24*2, 1, HEAPID_WORLDTRADE);

	GFL_ARC_CloseDataHandle( p_handle );

	WorldTrade_SubLcdBgGraphicSet( wk );	// �g���[�h���[���]��
	WorldTrade_SubLcdWinGraphicSet( wk );   // �g���[�h���[���E�C���h�E�]��
}
#define BOX_SX		( 25-6 )
#define BOX_SY		( 60-24 )
#define BOX_W		( 26 )
#define BOX_H		( 23 )

static const u16 PokemonIconPosTbl[][2]={
	{BOX_SX+BOX_W*0, BOX_SY+BOX_H*0},{BOX_SX+BOX_W*1, BOX_SY+BOX_H*0},{BOX_SX+BOX_W*2, BOX_SY+BOX_H*0},
	{BOX_SX+BOX_W*3, BOX_SY+BOX_H*0},{BOX_SX+BOX_W*4, BOX_SY+BOX_H*0},{BOX_SX+BOX_W*5, BOX_SY+BOX_H*0},

	{BOX_SX+BOX_W*0, BOX_SY+BOX_H*1},{BOX_SX+BOX_W*1, BOX_SY+BOX_H*1},{BOX_SX+BOX_W*2, BOX_SY+BOX_H*1},
	{BOX_SX+BOX_W*3, BOX_SY+BOX_H*1},{BOX_SX+BOX_W*4, BOX_SY+BOX_H*1},{BOX_SX+BOX_W*5, BOX_SY+BOX_H*1},

	{BOX_SX+BOX_W*0, BOX_SY+BOX_H*2},{BOX_SX+BOX_W*1, BOX_SY+BOX_H*2},{BOX_SX+BOX_W*2, BOX_SY+BOX_H*2},
	{BOX_SX+BOX_W*3, BOX_SY+BOX_H*2},{BOX_SX+BOX_W*4, BOX_SY+BOX_H*2},{BOX_SX+BOX_W*5, BOX_SY+BOX_H*2},

	{BOX_SX+BOX_W*0, BOX_SY+BOX_H*3},{BOX_SX+BOX_W*1, BOX_SY+BOX_H*3},{BOX_SX+BOX_W*2, BOX_SY+BOX_H*3},
	{BOX_SX+BOX_W*3, BOX_SY+BOX_H*3},{BOX_SX+BOX_W*4, BOX_SY+BOX_H*3},{BOX_SX+BOX_W*5, BOX_SY+BOX_H*3},

	{BOX_SX+BOX_W*0, BOX_SY+BOX_H*4},{BOX_SX+BOX_W*1, BOX_SY+BOX_H*4},{BOX_SX+BOX_W*2, BOX_SY+BOX_H*4},
	{BOX_SX+BOX_W*3, BOX_SY+BOX_H*4},{BOX_SX+BOX_W*4, BOX_SY+BOX_H*4},{BOX_SX+BOX_W*5, BOX_SY+BOX_H*4},
	{ 224,175-5*8 },{98-16, 16}
};


static const u16 BoxArrowPos[][2]={
	{162-8, 36-24 },
	{ 22-8, 36-24,}
};

#define BT_OFS_W	( 11 )
#define BT_OFS_H	( 13 )

static const GFL_UI_TP_HITTBL BoxTouchTable[]={
	{ BOX_SY+BOX_H*0-BT_OFS_H, BOX_SY+BOX_H*0+BT_OFS_H, BOX_SX+BOX_W*0-BT_OFS_W, BOX_SX+BOX_W*0+BT_OFS_W, },
	{ BOX_SY+BOX_H*0-BT_OFS_H, BOX_SY+BOX_H*0+BT_OFS_H, BOX_SX+BOX_W*1-BT_OFS_W, BOX_SX+BOX_W*1+BT_OFS_W, },
	{ BOX_SY+BOX_H*0-BT_OFS_H, BOX_SY+BOX_H*0+BT_OFS_H, BOX_SX+BOX_W*2-BT_OFS_W, BOX_SX+BOX_W*2+BT_OFS_W, },
	{ BOX_SY+BOX_H*0-BT_OFS_H, BOX_SY+BOX_H*0+BT_OFS_H, BOX_SX+BOX_W*3-BT_OFS_W, BOX_SX+BOX_W*3+BT_OFS_W, },
	{ BOX_SY+BOX_H*0-BT_OFS_H, BOX_SY+BOX_H*0+BT_OFS_H, BOX_SX+BOX_W*4-BT_OFS_W, BOX_SX+BOX_W*4+BT_OFS_W, },
	{ BOX_SY+BOX_H*0-BT_OFS_H, BOX_SY+BOX_H*0+BT_OFS_H, BOX_SX+BOX_W*5-BT_OFS_W, BOX_SX+BOX_W*5+BT_OFS_W, },

	{ BOX_SY+BOX_H*1-BT_OFS_H, BOX_SY+BOX_H*1+BT_OFS_H, BOX_SX+BOX_W*0-BT_OFS_W, BOX_SX+BOX_W*0+BT_OFS_W, },
	{ BOX_SY+BOX_H*1-BT_OFS_H, BOX_SY+BOX_H*1+BT_OFS_H, BOX_SX+BOX_W*1-BT_OFS_W, BOX_SX+BOX_W*1+BT_OFS_W, },
	{ BOX_SY+BOX_H*1-BT_OFS_H, BOX_SY+BOX_H*1+BT_OFS_H, BOX_SX+BOX_W*2-BT_OFS_W, BOX_SX+BOX_W*2+BT_OFS_W, },
	{ BOX_SY+BOX_H*1-BT_OFS_H, BOX_SY+BOX_H*1+BT_OFS_H, BOX_SX+BOX_W*3-BT_OFS_W, BOX_SX+BOX_W*3+BT_OFS_W, },
	{ BOX_SY+BOX_H*1-BT_OFS_H, BOX_SY+BOX_H*1+BT_OFS_H, BOX_SX+BOX_W*4-BT_OFS_W, BOX_SX+BOX_W*4+BT_OFS_W, },
	{ BOX_SY+BOX_H*1-BT_OFS_H, BOX_SY+BOX_H*1+BT_OFS_H, BOX_SX+BOX_W*5-BT_OFS_W, BOX_SX+BOX_W*5+BT_OFS_W, },
                     
	{ BOX_SY+BOX_H*2-BT_OFS_H, BOX_SY+BOX_H*2+BT_OFS_H, BOX_SX+BOX_W*0-BT_OFS_W, BOX_SX+BOX_W*0+BT_OFS_W, },
	{ BOX_SY+BOX_H*2-BT_OFS_H, BOX_SY+BOX_H*2+BT_OFS_H, BOX_SX+BOX_W*1-BT_OFS_W, BOX_SX+BOX_W*1+BT_OFS_W, },
	{ BOX_SY+BOX_H*2-BT_OFS_H, BOX_SY+BOX_H*2+BT_OFS_H, BOX_SX+BOX_W*2-BT_OFS_W, BOX_SX+BOX_W*2+BT_OFS_W, },
	{ BOX_SY+BOX_H*2-BT_OFS_H, BOX_SY+BOX_H*2+BT_OFS_H, BOX_SX+BOX_W*3-BT_OFS_W, BOX_SX+BOX_W*3+BT_OFS_W, },
	{ BOX_SY+BOX_H*2-BT_OFS_H, BOX_SY+BOX_H*2+BT_OFS_H, BOX_SX+BOX_W*4-BT_OFS_W, BOX_SX+BOX_W*4+BT_OFS_W, },
	{ BOX_SY+BOX_H*2-BT_OFS_H, BOX_SY+BOX_H*2+BT_OFS_H, BOX_SX+BOX_W*5-BT_OFS_W, BOX_SX+BOX_W*5+BT_OFS_W, },
                     
	{ BOX_SY+BOX_H*3-BT_OFS_H, BOX_SY+BOX_H*3+BT_OFS_H, BOX_SX+BOX_W*0-BT_OFS_W, BOX_SX+BOX_W*0+BT_OFS_W, },
	{ BOX_SY+BOX_H*3-BT_OFS_H, BOX_SY+BOX_H*3+BT_OFS_H, BOX_SX+BOX_W*1-BT_OFS_W, BOX_SX+BOX_W*1+BT_OFS_W, },
	{ BOX_SY+BOX_H*3-BT_OFS_H, BOX_SY+BOX_H*3+BT_OFS_H, BOX_SX+BOX_W*2-BT_OFS_W, BOX_SX+BOX_W*2+BT_OFS_W, },
	{ BOX_SY+BOX_H*3-BT_OFS_H, BOX_SY+BOX_H*3+BT_OFS_H, BOX_SX+BOX_W*3-BT_OFS_W, BOX_SX+BOX_W*3+BT_OFS_W, },
	{ BOX_SY+BOX_H*3-BT_OFS_H, BOX_SY+BOX_H*3+BT_OFS_H, BOX_SX+BOX_W*4-BT_OFS_W, BOX_SX+BOX_W*4+BT_OFS_W, },
	{ BOX_SY+BOX_H*3-BT_OFS_H, BOX_SY+BOX_H*3+BT_OFS_H, BOX_SX+BOX_W*5-BT_OFS_W, BOX_SX+BOX_W*5+BT_OFS_W, },
                     
	{ BOX_SY+BOX_H*4-BT_OFS_H, BOX_SY+BOX_H*4+BT_OFS_H, BOX_SX+BOX_W*0-BT_OFS_W, BOX_SX+BOX_W*0+BT_OFS_W, },
	{ BOX_SY+BOX_H*4-BT_OFS_H, BOX_SY+BOX_H*4+BT_OFS_H, BOX_SX+BOX_W*1-BT_OFS_W, BOX_SX+BOX_W*1+BT_OFS_W, },
	{ BOX_SY+BOX_H*4-BT_OFS_H, BOX_SY+BOX_H*4+BT_OFS_H, BOX_SX+BOX_W*2-BT_OFS_W, BOX_SX+BOX_W*2+BT_OFS_W, },
	{ BOX_SY+BOX_H*4-BT_OFS_H, BOX_SY+BOX_H*4+BT_OFS_H, BOX_SX+BOX_W*3-BT_OFS_W, BOX_SX+BOX_W*3+BT_OFS_W, },
	{ BOX_SY+BOX_H*4-BT_OFS_H, BOX_SY+BOX_H*4+BT_OFS_H, BOX_SX+BOX_W*4-BT_OFS_W, BOX_SX+BOX_W*4+BT_OFS_W, },
	{ BOX_SY+BOX_H*4-BT_OFS_H, BOX_SY+BOX_H*4+BT_OFS_H, BOX_SX+BOX_W*5-BT_OFS_W, BOX_SX+BOX_W*5+BT_OFS_W, },
                     
	{  123, 148, 186,  245},			// ���ǂ�
	{  16-12,  16+12, 154-8,  154+8, },	// �E���
	{  16-12,  16+12,  18-8,   18+8, },	// �����
	
	{GFL_UI_TP_HIT_END,0,0,0},		// �I���f�[�^
};



//------------------------------------------------------------------
/**
 * �Z���A�N�^�[�o�^
 *
 * @param   wk			GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void SetCellActor(WORLDTRADE_WORK *wk)
{
	int i;
	//�o�^���i�[
	GFL_CLWK_DATA	add;
	GFL_STD_MemClear( &add, sizeof(GFL_CLWK_DATA) );
	//WorldTrade_MakeCLACT( &add,  wk, &wk->clActHeader_main, NNS_G2D_VRAM_TYPE_2DMAIN );


	// �|�P�����������w
	add.pos_x = PokemonIconPosTbl[wk->BoxCursorPos][0];
	add.pos_y = PokemonIconPosTbl[wk->BoxCursorPos][1];
	wk->CursorActWork = GFL_CLACT_WK_Create( wk->clactSet,
			wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
	GFL_CLACT_WK_SetAutoAnmFlag(wk->CursorActWork,1);
	GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, 4 );	

  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 0 );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
  }
	
	/*
	 *	tomoya takahashi
		�y���o�����Ǝw�J�[�\���̗D�揇�ʂ���u�t�ɂȂ�z

		�u�|�P��������������v�ɓ���ہA�w�J�[�\�����u�Ă����v��u�{�b�N�X
		���v�ɗL��A���o���́u��������|�P�����������ł��������v�Əd�Ȃ�
		�Ă���ƁA��u���o���̕������w�J�[�\�����D�揇�ʂ������\�������
		���܂��܂��B
					�Ώ�
		//	GFL_CLACT_WK_SetBgPri( wk->CursorActWork, 1 );
	 */
	// �J�[�\���v���C�I���e�B����
	if(wk->BoxCursorPos==31 || (wk->BoxCursorPos>=0 && wk->BoxCursorPos<=5)){
		// �{�b�N�X�g���C���̏�ɂ���Ƃ��ƃ{�b�N�X�P��ڂɂ���Ƃ��͍ŏ�ʂ�
		GFL_CLACT_WK_SetBgPri( wk->CursorActWork, 0 );
	}else{
		// ����ȊO�͂ЂƂ���
		GFL_CLACT_WK_SetBgPri( wk->CursorActWork, 1 );
	}

	
	// �|�P�����A�C�R��
	for(i=0;i<BOX_POKE_NUM;i++){
		add.pos_x = PokemonIconPosTbl[i][0];
		add.pos_y = PokemonIconPosTbl[i][1];
		add.softpri   = 20;
		wk->PokeIconActWork[i] = GFL_CLACT_WK_Create(wk->clactSet,
			wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
		GFL_CLACT_WK_SetAnmSeq( wk->PokeIconActWork[i], 6+i );	
		GFL_CLACT_WK_SetBgPri( wk->PokeIconActWork[i], 1 );
	}

	// �A�C�e���A�C�R��
	for(i=0;i<BOX_POKE_NUM;i++){
		add.pos_x = ( PokemonIconPosTbl[i][0]+0);
		add.pos_y = ( PokemonIconPosTbl[i][1]+6);
		add.softpri   = 10;
		wk->ItemIconActWork[i] = GFL_CLACT_WK_Create(wk->clactSet,
			wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
		GFL_CLACT_WK_SetAnmSeq( wk->ItemIconActWork[i], CELL_ITEMICON_NO );	
		GFL_CLACT_WK_SetBgPri( wk->ItemIconActWork[i], 1 );

	}

	// �{�[���J�v�Z���A�C�R��
	for(i=0;i<TEMOTI_POKEMAX;i++){
		add.pos_x = ( PokemonIconPosTbl[i][0]+8);
		add.pos_y = ( PokemonIconPosTbl[i][1]+6);
		add.softpri   = 10;
		wk->CBallActWork[i] = GFL_CLACT_WK_Create(wk->clactSet,
			wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
		GFL_CLACT_WK_SetAnmSeq( wk->CBallActWork[i], CELL_CBALLICON_NO );	
		GFL_CLACT_WK_SetBgPri( wk->CBallActWork[i], 1 );

	}

	// �{�b�N�X�؂�ւ��̖��
	for(i=0;i<2;i++){
		add.pos_x = BoxArrowPos[i][0];
		add.pos_y = BoxArrowPos[i][1];
		wk->BoxArrowActWork[i] = GFL_CLACT_WK_Create(wk->clactSet,
			wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
///		GFL_CLACT_WK_SetAutoAnmFlag( wk->BoxArrowActWork[i], 1 );���������̓A�j�����Ȃ��悤�ɂ��Ƃ�
		GFL_CLACT_WK_SetAnmSeq( wk->BoxArrowActWork[i], CELL_BOXARROW_NO+i );	
		GFL_CLACT_WK_SetBgPri( wk->BoxArrowActWork[i], 1 );
	}

  //�w�̃A�C�R���炵�����A���̉�ʂŎg���Ă��Ȃ��̂ŗ��p�i�{�^���̂Ђ���j
	add.pos_x = 239;
	add.pos_y = 136;
  add.softpri = 30;
  add.bgpri   = 1;
  wk->FingerActWork = GFL_CLACT_WK_Create(wk->clactSet,
			wk->resObjTbl[RES_CURSOR][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[RES_CURSOR][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
	GFL_CLACT_WK_SetAnmSeq( wk->FingerActWork, 12 );
	GFL_CLACT_WK_SetAutoAnmFlag( wk->FingerActWork, 1 );
	GFL_CLACT_WK_StopAnm( wk->FingerActWork );
	GFL_CLACT_WK_SetDrawEnable( wk->FingerActWork, 0 );


	// �uDS�̉���ʂ��݂Ă˃A�C�R���v�̕\��
	GFL_CLACT_WK_SetDrawEnable( wk->PromptDsActWork, 1 );
  WorldTrade_CLACT_PosChangeSub( wk->PromptDsActWork, DS_ICON_X, DS_ICON_Y );
}


//------------------------------------------------------------------
/**
 * @brief   �\���Z���A�N�^�[�̉��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void DelCellActor( WORLDTRADE_WORK *wk )
{
	int i;

  GFL_CLACT_WK_Remove( wk->FingerActWork );

	// �{�b�N�X���̖��Q��
	for(i=0;i<2;i++){
		GFL_CLACT_WK_Remove(wk->BoxArrowActWork[i]);
	}

	// �J�[�\���폜
	GFL_CLACT_WK_Remove(wk->CursorActWork);

	// �|�P�����A�C�R���폜
	for(i=0;i<BOX_POKE_NUM;i++){
		GFL_CLACT_WK_Remove(wk->PokeIconActWork[i]);
		GFL_CLACT_WK_Remove(wk->ItemIconActWork[i]);
	}
	// �J�X�^���{�[���A�C�R���폜
	for(i=0;i<TEMOTI_POKEMAX;i++){
		GFL_CLACT_WK_Remove(wk->CBallActWork[i]);
	}
}

#define SELECT_MENU_X	(  21 )
#define SELECT_MENU_Y 	(  13 )
#define SELECT_MENU_SX	( 5*2 )
#define SELECT_MENU_SY	(   6 )


#define BOX_TRAY_NAME_X		(  4 )
#define BOX_TRAY_NAME_Y		(  0 )
#define BOX_TRAY_NAME_SX	( 13 )
#define BOX_TRAY_NAME_SY	(  3 )

#define END_TEXT_X			( 24 )
#define END_TEXT_Y			( 16 )
#define END_TEXT_SX			(  6 )
#define END_TEXT_SY			(  2 )


#define TITLE_MESSAGE_OFFSET   ( WORLDTRADE_MENUFRAME_CHR + MENU_WIN_CGX_SIZ )
#define BOX_TRAY_NAME_OFFSET   ( TITLE_MESSAGE_OFFSET     + TITLE_TEXT_SX*TITLE_TEXT_SY )
#define LINE_MESSAGE_OFFSET    ( BOX_TRAY_NAME_OFFSET     + BOX_TRAY_NAME_SX*BOX_TRAY_NAME_SX )
#define TALK_MESSAGE_OFFSET	   ( LINE_MESSAGE_OFFSET      + LINE_TEXT_SX*LINE_TEXT_SY )
#define END_MESSAGE_OFFSET	   ( TALK_MESSAGE_OFFSET      + TALK_WIN_X*TALK_WIN_Y )
#define SELECT_MENU_OFFSET 	   ( END_MESSAGE_OFFSET       + END_TEXT_SX*END_TEXT_SY )
#define YESNO_OFFSET 		   ( SELECT_MENU_OFFSET       + SELECT_MENU_SX*SELECT_MENU_SY )

//------------------------------------------------------------------
/**
 * BMPWIN�����i�����p�l���Ƀt�H���g�`��j
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BmpWinInit( WORLDTRADE_WORK *wk )
{
	// ---------- ���C����� ------------------

	// BG0��BMPWIN�^�C�g���E�C���h�E�m�ہE�`��
	
//	wk->TitleWin	= GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
//	TITLE_TEXT_X, TITLE_TEXT_Y, TITLE_TEXT_SX, TITLE_TEXT_SY, WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );

//	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->TitleWin), 0x0000 );
	
//	//  ��������|�P�����������ł�������
//	WorldTrade_SysPrint( wk->TitleWin, wk->TitleString, 0, 1, 0, PRINTSYS_LSB_Make(15,14,0) );


	// �{�b�N�X�g���C��BMPWIN�m��
	
	wk->SubWin	= GFL_BMPWIN_Create( GFL_BG_FRAME3_M,
	BOX_TRAY_NAME_X, BOX_TRAY_NAME_Y, BOX_TRAY_NAME_SX, BOX_TRAY_NAME_SY, WORLDTRADE_TALKFONT_PAL,  
	GFL_BMP_CHRAREA_GET_B );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->SubWin), 0x0000 );
	GFL_BMPWIN_MakeTransWindow(wk->SubWin);
	
	// ��s�E�C���h�E
	wk->MsgWin	= GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
		LINE_TEXT_X, LINE_TEXT_Y, LINE_TEXT_SX, LINE_TEXT_SY, 
		WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0x0000 );
	GFL_BMPWIN_MakeTransWindow(wk->MsgWin);

	// 2�s�E�C���h�E
	wk->TalkWin	= GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
		TALK_WIN_X, TALK_WIN_Y, TALK_WIN_SX, TALK_WIN_SY, 
		WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->TalkWin), 0x0000 );
	GFL_BMPWIN_MakeTransWindow(wk->MsgWin);

	// �u���ǂ�v
	wk->MenuWin[1]	= GFL_BMPWIN_Create( GFL_BG_FRAME1_M,
		END_TEXT_X, END_TEXT_Y, END_TEXT_SX, END_TEXT_SY, 
		0,  GFL_BMP_CHRAREA_GET_B );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuWin[1]), 0x4 );
	GFL_BMPWIN_MakeTransWindow(wk->MenuWin[1]);
	// �u���ǂ�v�`��
	WorldTrade_SysPrint( wk->MenuWin[1], wk->EndString, 0, 1, 1, PRINTSYS_LSB_Make(0xF,0xE,4), &wk->print );

	// �I�����j���[�̈�
//	wk->MenuWin[0]	= GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
//		SELECT_MENU_X, SELECT_MENU_Y, SELECT_MENU_SX, SELECT_MENU_SY, 
//		WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );	

	
	// �T�u��ʂ�GTS�����pBMPWIN���m�ۂ���
	if(wk->sub_process_mode==MODE_DEPOSIT_SELECT){
		WorldTrade_SubLcdExpainPut( wk, EXPLAIN_AZUKERU );
	}else{
		WorldTrade_SubLcdExpainPut( wk, EXPLAIN_KOUKAN );
	}

}	

//------------------------------------------------------------------
/**
 * @brief   �m�ۂ���BMPWIN�����
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BmpWinDelete( WORLDTRADE_WORK *wk )
{
  //BMP�������O�Ɏc�����X�g���[��������
  WT_PRINT_ClearBuffer( &wk->print );
	
	GFL_BMPWIN_Delete( wk->ExplainWin );
	GFL_BMPWIN_Delete( wk->TalkWin );
	GFL_BMPWIN_Delete( wk->MenuWin[1] );
//	GFL_BMPWIN_Delete( wk->MenuWin[0] );
	GFL_BMPWIN_Delete( wk->MsgWin );
	GFL_BMPWIN_Delete( wk->SubWin );
//	GFL_BMPWIN_Delete( wk->TitleWin );


}

//------------------------------------------------------------------
/**
 * ���E�������[�N������
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void InitWork( WORLDTRADE_WORK *wk )
{
  //�{�b�N�X�̍ő吔�`�F�b�N
  WorldTrade_CheckBoxTray( wk );

	// �{�b�N�X�g���C��������o�b�t�@
	wk->BoxTrayNameString = GFL_STR_CreateBuffer( BOX_TRAY_NAME_BUF_NUM, HEAPID_WORLDTRADE );


	// ������o�b�t�@�쐬
	wk->TalkString  = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WORLDTRADE );
#if 0
	// �u��������|�P�����������ł��������v�u�����̃|�P�����������ł��������v
	if(wk->sub_process_mode==MODE_DEPOSIT_SELECT){
		wk->TitleString = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_01_022 );
	}else if(wk->sub_process_mode==MODE_EXCHANGE_SELECT){
		wk->TitleString = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_01_018 );
	}
#endif
	// ���ǂ�
	wk->EndString   = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_05_014 );

	if(wk->BoxCursorPos==BOX_CUROSOR_END_POS){
		wk->BoxCursorPos = 0;
	}
	
	// �{�b�N�X�p���[�N
	wk->boxWork = GFL_HEAP_AllocMemory(HEAPID_WORLDTRADE, sizeof(BOX_RESEARCH));

}


//------------------------------------------------------------------
/**
 * @brief   ���[�N���
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void FreeWork( WORLDTRADE_WORK *wk )
{
	GFL_HEAP_FreeMemory(wk->boxWork);

	GFL_STR_DeleteBuffer( wk->BoxTrayNameString );
	GFL_STR_DeleteBuffer( wk->TalkString ); 
	GFL_STR_DeleteBuffer( wk->EndString ); 
///	GFL_STR_DeleteBuffer( wk->TitleString ); 

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
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_Start( WORLDTRADE_WORK *wk)
{
	// 
	if(WIPE_SYS_EndCheck()){
		int msg;
		// �u��������|�P�����������ł��������v�u�����̃|�P�����������ł��������v
		if(wk->sub_process_mode==MODE_DEPOSIT_SELECT){
			msg = msg_gtc_01_022;
		}else if(wk->sub_process_mode==MODE_EXCHANGE_SELECT){
			msg = msg_gtc_01_018;
		}
		SubSeq_MessagePrint( wk, msg, 1, 0, 0x0f0f, 0 );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );

//		SubSeq_MessagePrint( wk, msg_gtc_01_018, 1, 0, 0x0f0f );
//		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
//		wk->subprocess_seq = SUBSEQ_MAIN;
	}

	return SEQ_MAIN;
}
/* 
       31

 0  1  2  3  4  5
 6  7  8  9 10 11
12 13 14 15 16 17
18 19 20 21 22 23
24 25 26 27 28 29 30

 */

static const u8 box_pos_table[][4]={
	// �㉺���E
	{31, 6, 5, 1,},{31, 7, 0, 2,},{31, 8, 1, 3,},{31, 9, 2, 4,},{31,10, 3, 5,},{31,11, 4, 0,},
	{ 0,12,11, 7,},{ 1,13, 6, 8,},{ 2,14, 7, 9,},{ 3,15, 8,10,},{ 4,16, 9,11,},{ 5,17,10, 6,},
	{ 6,18,17,13,},{ 7,19,12,14,},{ 8,20,13,15,},{ 9,21,14,16,},{10,22,15,17,},{11,23,16,12,},
	{12,24,23,19,},{13,25,18,20,},{14,26,19,21,},{15,27,20,22,},{16,28,21,23,},{17,29,22,18,},
	{18,31,30,25,},{19,31,24,26,},{20,31,25,27,},{21,31,26,28,},{22,31,27,29,},{23,31,28,30,},{30,30,29,24},
	{ 26,2,99,101, },

};

static const s8 movetbl[][2]={
	{0,0},{0,-1},{0,1},{1,0},{-1,0},
};


static void DepositDecideFunc( WORLDTRADE_WORK*wk )
{
	if(wk->BoxCursorPos == BOX_CUROSOR_END_POS){
	// �^�C�g�����j���[�ɂ��ǂ�
    GFL_CLACT_WK_SetDrawEnable( wk->FingerActWork, 1);
    GFL_CLACT_WK_ResetAnm( wk->FingerActWork);
    wk->subprocess_seq =SUBSEQ_CANCEL_WAIT;
		PMSND_PlaySE(SE_CANCEL);
	}else{
		if(wk->BoxCursorPos!=BOX_CUROSOR_TRAYNAME_POS){
			// ���������������܂����H
			switch(PokemonCheck( wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos )){
			// ������
			case WANT_POKE_OK:
        PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
				if(CheckPocket( wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos )){
					WT_WORDSET_RegisterPokeNickNamePPP( wk->WordSet, 0, 
							WorldTrade_GetPokePtr(wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos) );
					SubSeq_MessagePrint( wk, msg_gtc_01_023, 1, 0, 0x0f0f, 0 );
					WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT,  SUBSEQ_SELECT_LIST);
					OS_Printf("deposit_ppp0 = %08x\n",WorldTrade_GetPokePtr(wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos));
				}else{
					// �Ă����̍Ō��1�C�������ꍇ�͂����Ȃ�
					SubSeq_MessagePrint( wk, msg_gtc_01_027, 1, 0, 0x0f0f, 1 );
					WorldTrade_SetNextSeq( wk, SUBSEQ_MES_CLEAR_WAIT,  SUBSEQ_MAIN);
				}
				break;
			// �^�}�S�͂����Ȃ�
			case WANT_POKE_TAMAGO:
        PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
				SubSeq_MessagePrint( wk, msg_gtc_01_028, 1, 0, 0x0f0f, 1 );
				WorldTrade_SetNextSeq( wk, SUBSEQ_MES_CLEAR_WAIT,  SUBSEQ_MAIN);
				break;
			// ���ꃊ�{�������Ă���̂Ń_��
			}
		}
	}

}

static void ExchangeDecideFunc( WORLDTRADE_WORK*wk )
{
	if(wk->BoxCursorPos == BOX_CUROSOR_END_POS){
		// �������ʃ|�P�����̉�ʂɂ��ǂ�
    GFL_CLACT_WK_SetDrawEnable( wk->FingerActWork, 1);
    GFL_CLACT_WK_ResetAnm( wk->FingerActWork);
    wk->subprocess_seq =SUBSEQ_CANCEL_WAIT;

/**		
		// �T�[�o�[�`�F�b�N�̌�^�C�g�����j���[��
		WorldTrade_SubProcessChange( wk, WORLDTRADE_PARTNER, 0 );
		wk->subprocess_seq  = SUBSEQ_END;		
		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
*/		
	}else{
		if(wk->BoxCursorPos!=BOX_CUROSOR_TRAYNAME_POS){
			// �������������ɂ����܂����H
			switch(PokemonCheck( wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos )){
			case WANT_POKE_OK:
				{
					POKEMON_PASO_PARAM *ppp = WorldTrade_GetPokePtr(wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos);

					// ���O���擾����WordSet�ɓo�^
					if(WantPokeCheck( ppp, &wk->DownloadPokemonData[wk->TouchTrainerPos].wantSimple)){
						if(CheckPocket( wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos )){
							WT_WORDSET_RegisterPokeNickNamePPP( wk->WordSet, 0, ppp);
							SubSeq_MessagePrint( wk, msg_gtc_01_019, 1, 0, 0x0f0f, 0 );
							WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT,  SUBSEQ_EXCHANGE_SELECT_LIST);
							OS_Printf("deposit_ppp0 = %08x\n",WorldTrade_GetPokePtr(wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos));
							PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
						}else{
							// �Ă����̍Ō��1�C�������ꍇ�͂����Ȃ�
							SubSeq_MessagePrint( wk, msg_gtc_01_027, 1, 0, 0x0f0f, 1 );
							WorldTrade_SetNextSeq( wk, SUBSEQ_MES_CLEAR_WAIT,  SUBSEQ_MAIN);
						}
					}else{
						PMSND_PlaySE(SE_CANCEL);
					}
				}
				break;
			case WANT_POKE_TAMAGO: case WANT_POKE_NO:
				PMSND_PlaySE(SE_CANCEL);
				break;
			}
		}
	}
	
}

//------------------------------------------------------------------
/**
 * @brief   �^�b�`�p�l�����o����
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  u32		�^�b�`���茋��
 */
//------------------------------------------------------------------
static u32 TouchFunc( WORLDTRADE_WORK *wk )
{
	u32 ret = GFL_UI_TP_HitTrg( BoxTouchTable );
	if(ret!=GFL_UI_TP_HIT_NONE){
    GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 0 );
    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
		OS_Printf("box touch = %d\n", ret);
	}
	return ret;
}

//------------------------------------------------------------------
/**
 * @brief   �L�����Z������
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   mode	BOX��ʃ��[�h
 *
 * @retval  none	
 */
//------------------------------------------------------------------
static void CancelFunc( WORLDTRADE_WORK *wk, int mode )
{
	if(mode==MODE_DEPOSIT_SELECT){
		WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
		wk->subprocess_seq = SUBSEQ_END;
	}else if(mode==MODE_EXCHANGE_SELECT){
		// �������ʃ|�P�����̉�ʂɂ��ǂ�
		WorldTrade_SubProcessChange( wk, WORLDTRADE_PARTNER, MODE_FROM_BOX );
		wk->subprocess_seq  = SUBSEQ_END;
	}
}


#define TOUCH_CANCEL		( 30 ) 
#define TOUCH_RIGHT_ARROW	( 31 )
#define TOUCH_LEFT_ARROW	( 32 )

//------------------------------------------------------------------
/**
 * @brief   �T�u�v���Z�X�V�[�P���X���C��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_Main( WORLDTRADE_WORK *wk)
{
	// �^�b�`�Ō���
	u32 result=TouchFunc( wk );
	if(result!=GFL_UI_TP_HIT_NONE){
		switch(result){
		case TOUCH_RIGHT_ARROW:
			//�A�j��
			GFL_CLACT_WK_SetAutoAnmFlag( wk->BoxArrowActWork[0], 1 );
			GFL_CLACT_WK_SetAnmSeq( wk->BoxArrowActWork[0], CELL_BOXARROW_NO );
			
			wk->BoxTrayNo = BoxTrayRoundWork( wk->BoxTrayNo, wk->BoxTrayMax, 1);
			NowBoxPageInfoGet( wk, wk->BoxTrayNo );
			PMSND_PlaySE(WORLDTRADE_MOVE_SE);
			break;
		case TOUCH_LEFT_ARROW:
			//�A�j��
			GFL_CLACT_WK_SetAutoAnmFlag( wk->BoxArrowActWork[1], 1 );
			GFL_CLACT_WK_SetAnmSeq( wk->BoxArrowActWork[1], CELL_BOXARROW_NO+1 );

			wk->BoxTrayNo = BoxTrayRoundWork( wk->BoxTrayNo, wk->BoxTrayMax, -1);
			NowBoxPageInfoGet( wk, wk->BoxTrayNo );
			PMSND_PlaySE(WORLDTRADE_MOVE_SE);
			break;
		case TOUCH_CANCEL:
			// �^�C�g���ɖ߂�����A������ʂɖ߂�����
      GFL_CLACT_WK_SetDrawEnable( wk->FingerActWork, 1);
      GFL_CLACT_WK_ResetAnm( wk->FingerActWork);
      wk->subprocess_seq =SUBSEQ_CANCEL_WAIT;
			wk->BoxCursorPos = result;
      PMSND_PlaySE(SE_CANCEL);
			CursorPosPrioritySet( wk->CursorActWork, wk->BoxCursorPos );
			break;
		default:
			wk->BoxCursorPos = result;
			CursorPosPrioritySet( wk->CursorActWork, wk->BoxCursorPos );
			if(wk->sub_process_mode==MODE_DEPOSIT_SELECT){
				DepositDecideFunc(wk);
			}else if(wk->sub_process_mode==MODE_EXCHANGE_SELECT){
				ExchangeDecideFunc(wk);
			}
		}
	}else{
	// �L�[����
		CursorControl(wk);
		// �������鎞
		if(wk->sub_process_mode==MODE_DEPOSIT_SELECT){
			if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
        GFL_CLACT_WK_SetDrawEnable( wk->FingerActWork, 1);
        GFL_CLACT_WK_ResetAnm( wk->FingerActWork);
        wk->subprocess_seq =SUBSEQ_CANCEL_WAIT;
        PMSND_PlaySE(SE_CANCEL);
			}else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE){
				DepositDecideFunc(wk);
			}
			
		// ��������Ƃ�
		}else if(wk->sub_process_mode==MODE_EXCHANGE_SELECT){
			if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
				// �������ʃ|�P�����̉�ʂɂ��ǂ�
        GFL_CLACT_WK_SetDrawEnable( wk->FingerActWork, 1);
        GFL_CLACT_WK_ResetAnm( wk->FingerActWork);
        wk->subprocess_seq =SUBSEQ_CANCEL_WAIT;
        PMSND_PlaySE(SE_CANCEL);
			}else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE){
				ExchangeDecideFunc(wk);
			}
		}
	}

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �J�[�\������
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static void CursorControl( WORLDTRADE_WORK *wk )
{
	int move=0, arrow=0, tmp=0;

  if( GFL_UI_KEY_GetTrg() && GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    PMSND_PlaySE(WORLDTRADE_MOVE_SE);
    return;
  }

	// �J�[�\������
	if(GFL_UI_KEY_GetTrg() & PAD_KEY_UP){
		arrow = 1;
	}else if(GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN){
		arrow = 2;
	}else if(GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT){
		arrow = 3;
	}else if(GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT){
		arrow = 4;
	}

	// �㉺���E���������H
	if(arrow!=0){
		tmp = box_pos_table[wk->BoxCursorPos][arrow-1];
		if(tmp!=wk->BoxCursorPos){
			if(tmp==99||tmp==101){	// �{�b�N�X�؂�ւ�
				u8 arw_idx;
				//�A�j��
				if (tmp == 101){
					arw_idx = 0;
				}else{
					arw_idx = 1;
				}
				GFL_CLACT_WK_SetAutoAnmFlag( wk->BoxArrowActWork[arw_idx], 1 );
				GFL_CLACT_WK_SetAnmSeq( wk->BoxArrowActWork[arw_idx], CELL_BOXARROW_NO+arw_idx );

				wk->BoxTrayNo = BoxTrayRoundWork( wk->BoxTrayNo, wk->BoxTrayMax, tmp-100);
				NowBoxPageInfoGet( wk, wk->BoxTrayNo );
				PMSND_PlaySE(WORLDTRADE_MOVE_SE);
			}else{
				move = 1;
				wk->BoxCursorPos = tmp;
			}
		}
	}
	
	// �ړ������H
	if(move){
		PMSND_PlaySE(WORLDTRADE_MOVE_SE);
	}
	
	// �J�[�\���ʒu�w�聕�v���C�I���e�B�w��
	CursorPosPrioritySet( wk->CursorActWork, wk->BoxCursorPos );

}



//------------------------------------------------------------------
/**
 * @brief   �{�b�N�X��ʂ̃J�[�\�����W�w�菈��
 *
 * @param   GFL_CLWK*	�J�[�\���̃Z���A�N�^�[
 * @param   pos				�ʒu�ԍ�
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void  CursorPosPrioritySet( GFL_CLWK* cursor, int pos )
{
	WorldTrade_CLACT_PosChange(cursor, PokemonIconPosTbl[pos][0], PokemonIconPosTbl[pos][1] );
	// �J�[�\���v���C�I���e�B����
	if(pos==31 || (pos>=0 && pos<=5)){
		// �{�b�N�X�g���C���̏�ɂ���Ƃ��ƃ{�b�N�X�P��ڂɂ���Ƃ��͍ŏ�ʂ�
		GFL_CLACT_WK_SetBgPri( cursor, 0 );
	}else{
		// ����ȊO�͂ЂƂ���
		GFL_CLACT_WK_SetBgPri( cursor, 1 );
	}
}


//------------------------------------------------------------------
/**
 * @brief   �{�b�N�X�̃g���C��荞�ݏ���
 *              num��move�𑫂�,0�ȉ��Ȃ�max-1�ɁAmax�Ȃ�0�ɂ��ĕԂ�
 *
 *              wb���max�ϓ��{�莝���C���f�b�N�X��ύX�������߁A
 *              ��荞�݂��������������
 *
 * @param   num		���̒l
 * @param   max		�{�b�N�X�̍ő�l
 * @param   move	�����l�i�{�|����j
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int BoxTrayRoundWork( int num, int max, int move )
{
  if( num == WORLDTRADE_BOX_TEMOTI )
  {
    //�莝���̂Ƃ��͈ړ������ɂ����
    //�{�b�N�X�֋A��
    if( move > 0 )
    {
      num = 0;
    }
    else if( move < 0 )
    {
      num = max-1;
    }
  }
  else
  {
    //�莝���ȊO�͒P���ȉ�荞��
    num += move;
    if(num < 0) {
      return WORLDTRADE_BOX_TEMOTI;
    }
    if(num==max){
      return WORLDTRADE_BOX_TEMOTI;
    }
  }
	return num;
}


//------------------------------------------------------------------
/**
 * @brief   �I�����X�g�쐬
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_SelectList( WORLDTRADE_WORK *wk )
{
/**	
	BMPMENU_HEADER temp;

	temp.font  = FONT_SYSTEM;
	temp.x_max = 1;
	temp.y_max = 3;
	temp.line_spc = 0;
	temp.c_disp_f = 0;
	temp.loop_f   = 0;
*/

	wk->BmpMenuList = BmpMenuWork_ListCreate( 3, HEAPID_WORLDTRADE );
	// �悤�����݂�
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_05_005, 1, HEAPID_WORLDTRADE );
	// ��������
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_05_006, 2, HEAPID_WORLDTRADE );
	// ��߂�
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_05_007, 3, HEAPID_WORLDTRADE );


	// �I���{�b�N�X�Ăяo��
	WorldTrade_SelBoxInit( wk, GFL_BG_FRAME0_M, 3, WORLDTRADE_YESNO_PY1 );


	wk->subprocess_seq = SUBSEQ_SELECT_WAIT;

	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   �u�悤�����݂�v�u��������v�u��߂�v�I��҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_SelectWait( WORLDTRADE_WORK *wk )
{
	POKEMON_PASO_PARAM *ppp;
	u32 ret = WorldTrade_SelBoxMain( wk );
	int error=0;
	
//	switch(BmpMenuMain( wk->BmpMenuWork )){
	switch(ret){
	// �u�悤�����݂�v
	case 1:
		WorldTrade_SelBoxEnd( wk );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
		wk->subprocess_seq  = SUBSEQ_END;
		WorldTrade_SubProcessChange( wk, WORLDTRADE_STATUS, MODE_DEPOSIT_SELECT );
		break;

	// �u��������v
	case 2:
		WorldTrade_SelBoxEnd( wk );
		BmpMenuWork_ListDelete( wk->BmpMenuList );

		ppp = WorldTrade_GetPokePtr( wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos);
		if( PokeRibbonCheck( ppp ) ){
			SubSeq_MessagePrint( wk, msg_gtc_01_038, 1, 0, 0x0f0f, 1 );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_CLEAR_WAIT,  SUBSEQ_MAIN);
		}
#if 0//origin
		else if((error=PokeNewFormCheck(ppp))){
#else 
			//warning�ɂȂ��Ă����̂ŏC��
		else if((PokeNewFormCheck(ppp))){
			error=PokeNewFormCheck(ppp);
#endif
			if(error==1){
				SubSeq_MessagePrint( wk, msg_gtc_new_form, 1, 0, 0x0f0f, 1 );
			}else{
				SubSeq_MessagePrint( wk, msg_gtc_new_form_002, 1, 0, 0x0f0f, 1 );
			}
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_CLEAR_WAIT,  SUBSEQ_MAIN);
		}
		else if(PokeNewItemCheck(ppp)){
			SubSeq_MessagePrint( wk, msg_gtc_new_item, 1, 0, 0x0f0f, 1 );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_CLEAR_WAIT,  SUBSEQ_MAIN);
		}
		else{
			int flag = 0;
			// �Ă����̂Ƃ��̓J�X�^���{�[���`�F�b�N
			if(WorldTrade_GetPPorPPP( wk->BoxTrayNo )){
				POKEMON_PARAM *pp;
				pp = PokeParty_GetMemberPointer(wk->param->myparty, wk->BoxCursorPos);
#if 0 //WB�ɃJ�X�^���{�[���͂Ȃ�
				if(PP_Get( pp, ID_PARA_cb_id, NULL )){
					flag = 1;
					wk->subprocess_seq = SUBSEQ_CBALL_DEPOSIT_YESNO_MES;
				}
#endif 
				OS_Printf("�Ă����w�� �J�X�^��=%d\n", PP_Get( pp, ID_PARA_cb_id, NULL ));
			}

			// ���Ȃ���΃|�P�����a����
			if(flag==0){
				wk->deposit_ppp     = WorldTrade_GetPokePtr( wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos );
				wk->subprocess_seq  = SUBSEQ_END;
				WorldTrade_SubProcessChange( wk, WORLDTRADE_DEPOSIT, 0 );
				OS_Printf("deposit_ppp1 = %08x\n",WorldTrade_GetPokePtr(wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos));
			}
		}
		break;
		
	// �u��߂�v
	case 3:case BMPMENU_CANCEL:
		WorldTrade_SelBoxEnd( wk );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
		wk->subprocess_seq  = SUBSEQ_START;

		break;
	}
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �I�����X�g�쐬
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_ExchangeSelectList( WORLDTRADE_WORK *wk )
{
	BMPMENU_HEADER temp;

	temp.x_max = 1;
	temp.y_max = 3;
	temp.line_spc = 0;
	temp.c_disp_f = 0;
	temp.loop_f   = 0;


	wk->BmpMenuList = BmpMenuWork_ListCreate( 3, HEAPID_WORLDTRADE );
	// �悤�����݂�
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_04_018, 1, HEAPID_WORLDTRADE );
	// �������񂷂�
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_04_019, 2, HEAPID_WORLDTRADE );
	// ��߂�
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_04_020, 3, HEAPID_WORLDTRADE );

/*
	temp.menu = wk->BmpMenuList;
	temp.win  = &wk->MenuWin[0];
	
	GFL_BMP_Clear( temp.win, 0x0f0f );

	// �g�`��
	BmpWinFrame_Write( &wk->MenuWin[0], WINDOW_TRANS_OFF, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

	// BMP���j���[�J�n
	wk->BmpMenuWork = BmpMenuAddEx( &temp, 9, 0, 0, HEAPID_WORLDTRADE, PAD_BUTTON_CANCEL );
*/
	// �I���{�b�N�X�Ăяo��
	WorldTrade_SelBoxInit( wk, GFL_BG_FRAME0_M, 3, WORLDTRADE_YESNO_PY1 );

	wk->subprocess_seq = SUBSEQ_EXCHANGE_SELECT_WAIT;

	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   �u�悤�����݂�v�u�������񂷂�v�u��߂�v�I��҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_ExchangeSelectWait( WORLDTRADE_WORK *wk )
{
	POKEMON_PASO_PARAM *ppp;
	int error=0;
//	switch(BmpMenuMain( wk->BmpMenuWork )){
	u32 ret = WorldTrade_SelBoxMain( wk );
	
	switch(ret){
	// �u�悤�����݂�v
	case 1:
		WorldTrade_SelBoxEnd( wk );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		wk->subprocess_seq  = SUBSEQ_END;
		 WorldTrade_SubProcessChange( wk, WORLDTRADE_STATUS, MODE_EXCHANGE_SELECT );
		break;

	// �u�������񂷂�v
	case 2:
		WorldTrade_SelBoxEnd( wk );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );

		ppp = WorldTrade_GetPokePtr( wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos);
		if( PokeRibbonCheck( ppp )){
			SubSeq_MessagePrint( wk, msg_gtc_01_038, 1, 0, 0x0f0f, 1 );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_CLEAR_WAIT,  SUBSEQ_MAIN);
		}
#if 0//origin
		else if((error=PokeNewFormCheck(ppp))){
#else
			//warning�ɂȂ��Ă����̂ŏC��
		else if((PokeNewFormCheck(ppp))){
			error=PokeNewFormCheck(ppp);
#endif
			if(error==1){
				SubSeq_MessagePrint( wk, msg_gtc_new_form, 1, 0, 0x0f0f, 1 );
			}else{
				SubSeq_MessagePrint( wk, msg_gtc_new_form_002, 1, 0, 0x0f0f, 1 );
			}
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_CLEAR_WAIT,  SUBSEQ_MAIN);
		}
		else if(PokeNewItemCheck(ppp)){
			SubSeq_MessagePrint( wk, msg_gtc_new_item, 1, 0, 0x0f0f, 1 );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_CLEAR_WAIT,  SUBSEQ_MAIN);
		}
		else{
			int flag = 0;
			// �Ă����̂Ƃ��̓J�X�^���{�[���`�F�b�N
			if(WorldTrade_GetPPorPPP( wk->BoxTrayNo )){
				POKEMON_PARAM *pp;
				pp = PokeParty_GetMemberPointer(wk->param->myparty, wk->BoxCursorPos);
#if 0 //WB�ɃJ�X�^���{�[���͂Ȃ�
				if(PP_Get( pp, ID_PARA_cb_id, NULL )){
					flag = 1;
					wk->subprocess_seq = SUBSEQ_CBALL_YESNO_MES;
				}
#endif
				OS_Printf("�Ă����w�� �J�X�^��=%d\n", PP_Get( pp, ID_PARA_cb_id, NULL ));
			}
			
			// ���Ȃ���Ό�����
			if(flag==0){
				ExchangeCheck(wk);
			}
		}
		break;
		
	// �u��߂�v
	case 3:case BMPMENU_CANCEL:
		WorldTrade_SelBoxEnd( wk );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
		wk->subprocess_seq  = SUBSEQ_START;

		break;
	}
	
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �T�u�v���Z�X�V�[�P���X�I������
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_End( WORLDTRADE_WORK *wk )
{
	// �ڑ���ʂ�������A�㉺��ʂŃt�F�[�h
	if( wk->sub_nextprocess==WORLDTRADE_ENTER || wk->sub_nextprocess==WORLDTRADE_STATUS 
	 || wk->sub_nextprocess==WORLDTRADE_PARTNER ){
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
		OS_Printf( "******************** worldtrade_box.c [1164] MS ********************\n" );
#endif
		wk->sub_out_flg = 1;
	}else{
		// �����̉�ʂɂ����ꍇ�͏��ʂ����t�F�[�h
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
		OS_Printf( "******************** worldtrade_box.c [1169] M ********************\n" );
#endif
	}
	wk->subprocess_seq = 0;
	
	return SEQ_FADEOUT;
}

//------------------------------------------------------------------
/**
 * @brief   �͂��E������
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_YesNo( WORLDTRADE_WORK *wk)
{
	WorldTrade_TouchWinYesNoMake( wk, WORLDTRADE_YESNO_PY1, YESNO_OFFSET, 8, 1 );
	wk->subprocess_seq = SUBSEQ_YESNO_SELECT;
	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   �͂��E�������I��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_YesNoSelect( WORLDTRADE_WORK *wk)
{

  //@�����炭�����Ă��Ȃ�

	u32 ret = WorldTrade_TouchSwMain(wk);

  GF_ASSERT( 0 );

	if(ret==WORLDTRADE_RET_YES){
		// �^�C�g�����j���[���I��
    WorldTrade_TouchDelete( wk );
		wk->subprocess_seq  = SUBSEQ_END;
		 WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, 0 );
	}else if(ret==WORLDTRADE_RET_NO){
		// �������������g���C
    WorldTrade_TouchDelete( wk );
		wk->subprocess_seq = SUBSEQ_START;
	}

	return SEQ_MAIN;
	
}



//------------------------------------------------------------------
/**
 * @brief   �u�{�[���J�v�Z�����O��܂����A��낵���ł����H�v
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_CBallYesNoMessage( WORLDTRADE_WORK *wk )
{
	SubSeq_MessagePrint( wk, msg_gtc_01_026, 1, 0, 0x0f0f, 1 );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT,  SUBSEQ_CBALL_YESNO );

	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   �{�[���J�v�Z�����O��܂����E�E�E�͂��E������
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_CBallYesNo( WORLDTRADE_WORK *wk )
{
	 WorldTrade_TouchWinYesNoMake( wk, WORLDTRADE_YESNO_PY2, YESNO_OFFSET, 8, 1 );
	wk->subprocess_seq = SUBSEQ_CBALL_YESNO_SELECT;

	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   �{�[���J�v�Z�����O��܂����E�E�E�E�E�͂��E�������I��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_CBallYesNoSelect( WORLDTRADE_WORK *wk )
{
	u32 ret = WorldTrade_TouchSwMain(wk);

	if(ret==WORLDTRADE_RET_YES){
		// ������
    WorldTrade_TouchDelete( wk );
		ExchangeCheck(wk);
	}else if(ret==WORLDTRADE_RET_NO){
		// ������������
    WorldTrade_TouchDelete( wk );
		BmpWinFrame_Clear( wk->TalkWin, WINDOW_TRANS_ON );
		wk->subprocess_seq  = SUBSEQ_MAIN;
	}


/*
	int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_WORLDTRADE );

	if(ret!=BMPMENU_NULL){
		if(ret==BMPMENU_CANCEL){
			// ������������
			BmpWinFrame_Clear( wk->TalkWin, WINDOW_TRANS_ON );
			wk->subprocess_seq  = SUBSEQ_MAIN;
		}else{
			// ������
			ExchangeCheck(wk);
		}
	}
*/
	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   �������邽�߂̃`�F�b�N
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int ExchangeCheck( WORLDTRADE_WORK *wk )
{
	POKEMON_PARAM *pp;
	pp = (POKEMON_PARAM*)wk->DownloadPokemonData[wk->TouchTrainerPos].postData.data;

	// �����Ẵ|�P�����Ƀ��[�������Ă��āA�����̂Ă������U�Ђ��������Ƃ��͌����ł��Ȃ��B
	if(WorldTrade_PokemonMailCheck(pp) && wk->BoxTrayNo!=WORLDTRADE_BOX_TEMOTI){
		if(PokeParty_GetPokeCount(wk->param->myparty)==6){
			SubSeq_MessagePrint( wk, msg_gtc_01_029, 1, 0, 0x0f0f, 1 );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_CLEAR_WAIT,  SUBSEQ_MAIN );
			return 0;
		}
	}

	wk->deposit_ppp     = WorldTrade_GetPokePtr( wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos );
	wk->subprocess_seq  = SUBSEQ_END;
	wk->sub_out_flg = 1;
	WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_EXCHANGE );
	OS_Printf("deposit_ppp1 = %08x\n",WorldTrade_GetPokePtr(wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos));

	MakeExchangePokemonData( &wk->UploadPokemonData, wk );

	// �����q�b�g�����N���A���Ă����Ȃ��ƁA�f���I����ɂn�a�i���\������Ă��܂�
	wk->SearchResult = 0;

	return 1;
}

//------------------------------------------------------------------
/**
 * @brief   �u�{�[���J�v�Z�����O��܂����A��낵���ł����H�v
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_CBallDepositYesNoMessage( WORLDTRADE_WORK *wk )
{
	SubSeq_MessagePrint( wk, msg_gtc_01_026, 1, 0, 0x0f0f, 1 );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT,  SUBSEQ_CBALL_DEPOSIT_YESNO );

	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   �{�[���J�v�Z�����O��܂����E�E�E�͂��E������
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_CBallDepositYesNo( WORLDTRADE_WORK *wk )
{
	 WorldTrade_TouchWinYesNoMake( wk, WORLDTRADE_YESNO_PY2, YESNO_OFFSET, 8 ,1 );
	wk->subprocess_seq = SUBSEQ_CBALL_DEPOSIT_YESNO_SELECT;

	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   �{�[���J�v�Z�����O��܂����E�E�E�E�E�͂��E�������I��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_CBallDepositYesNoSelect( WORLDTRADE_WORK *wk )
{

	u32 ret = WorldTrade_TouchSwMain(wk);

	if(ret==WORLDTRADE_RET_YES){
		// ������
    WorldTrade_TouchDelete( wk );
		wk->deposit_ppp     = WorldTrade_GetPokePtr( wk->param->myparty, wk->param->mybox, 
														wk->BoxTrayNo, wk->BoxCursorPos );
		wk->subprocess_seq  = SUBSEQ_END;
		 WorldTrade_SubProcessChange( wk, WORLDTRADE_DEPOSIT, 0 );
		OS_Printf("deposit_ppp1 = %08x\n",WorldTrade_GetPokePtr(wk->param->myparty, wk->param->mybox, 
																wk->BoxTrayNo, wk->BoxCursorPos));
	}else if(ret==WORLDTRADE_RET_NO){
		// ������������
    WorldTrade_TouchDelete( wk );
		BmpWinFrame_Clear( wk->TalkWin, WINDOW_TRANS_ON );
		wk->subprocess_seq  = SUBSEQ_MAIN;
	}

/*
	int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_WORLDTRADE );

	if(ret!=BMPMENU_NULL){
		if(ret==BMPMENU_CANCEL){
			// ������������
			BmpWinFrame_Clear( wk->TalkWin, WINDOW_TRANS_ON );
			wk->subprocess_seq  = SUBSEQ_MAIN;
		}else{
			// ������
			wk->deposit_ppp     = WorldTrade_GetPokePtr( wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos );
			wk->subprocess_seq  = SUBSEQ_END;
			 WorldTrade_SubProcessChange( wk, WORLDTRADE_DEPOSIT, 0 );
			OS_Printf("deposit_ppp1 = %08x\n",WorldTrade_GetPokePtr(wk->param->myparty, wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos));
		}
	}
*/
	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   �L�����Z���E�F�C�g�҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_CancelWait( WORLDTRADE_WORK *wk )
{ 
  if( !GFL_CLACT_WK_CheckAnmActive( wk->FingerActWork) )
  { 
    CancelFunc( wk, wk->sub_process_mode );
  }
  return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   ��b�I����҂��Ď��̃V�[�P���X��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
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
 * @brief   ��b�I����҂��ăE�C���h�E���������A���̃V�[�P���X��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none
 */
//------------------------------------------------------------------
static int SubSeq_MessageClearWait( WORLDTRADE_WORK *wk )
{
	if( GF_MSG_PrintEndCheck( &wk->print )==0){
		BmpWinFrame_Clear( wk->TalkWin, WINDOW_TRANS_ON );
		wk->subprocess_seq = wk->subprocess_nextseq;
	}
	return SEQ_MAIN;

}


























//------------------------------------------------------------------
/**
 * @brief   ��b�E�C���h�E�\��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		�T�u�V�[�P���X
 */
//------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * @brief	��b�E�C���h�E�\��
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   msgno	���b�Z�[�WNO
 * @param   wait	�E�F�C�g�iBmpPrintColor�p�j
 * @param   flag	0:�o�͐悪�ꌾ�E�C���h�E  1:�o�͐悪��b�E�C���h�E
 * @param   dat		���g�p
 * @param   winflag	�E�C���h�E��`�悷�邩1:����	0:���Ȃ�
 */
//----------------------------------------------------------------------------------
static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat, int winflag )
{
	GFL_BMPWIN *win;
	
	// ������擾
	STRBUF *tempbuf;
	
	// ������擾
	tempbuf = GFL_MSG_CreateString(  wk->MsgManager, msgno );

	// WORDSET�W�J
	WORDSET_ExpandStr( wk->WordSet, wk->TalkString, tempbuf );
	

	if(winflag==0){
		win = wk->MsgWin;
	}else{
		win = wk->TalkWin;
	}
	// ��b�E�C���h�E�g�`��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win),  0x0f0f );
	BmpWinFrame_Write( win, WINDOW_TRANS_ON, WORLDTRADE_MESFRAME_CHR, WORLDTRADE_MESFRAME_PAL );

	// ������`��J�n
	GF_STR_PrintSimple( win, FONT_TALK, wk->TalkString, 0, 0, &wk->print );
	GFL_BMPWIN_MakeTransWindow(win);


	GFL_STR_DeleteBuffer(tempbuf);
}


//==============================================================================
/**
 * @brief   �|�P�����A�C�R���̓ǂݍ��݂𑬂����邽�߂Ƀn���h�����J�����n���h������ǂݍ���
 *
 * @param   handle		�A�[�J�C�u�n���h��
 * @param   dataIdx		�f�[�^�C���f�b�N�X
 * @param   charData	�J�����f�[�^�̃L�����f�[�^�|�C���^���������܂��
 * @param   heapID		�q�[�vID
 *
 * @retval  void*		�J�����t�@�C���̃o�b�t�@�|�C���^�i�������ŉ������j
 */
//==============================================================================
static void* CharDataGetbyHandle( ARCHANDLE *handle, u32 dataIdx, NNSG2dCharacterData** charData, u32 heapID )
{
	void* arcData = GFL_ARC_LoadDataAllocByHandle( handle, dataIdx, heapID );

	if( arcData != NULL )
	{
		if( NNS_G2dGetUnpackedBGCharacterData( arcData, charData ) == FALSE)
		{
			// ���s������NULL
			GFL_HEAP_FreeMemory( arcData );
			return NULL;
		}
	}
	return arcData;
}

//------------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R����]�����ăA�N�^�[�̃p���b�g�����킹��
 *
 * @param   chara	�|�P����NO
 * @param   pokeno	
 * @param   no		
 * @param   icon	
 *
 * @retval  none	
 */
//------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * @brief	�|�P�����A�C�R����]�����ăA�N�^�[�̃p���b�g�����킹��
 *
 * @param   pokeno	�|�P����NO
 * @param   form	�t�H����
 * @param   gender	����
 * @param   tamago	�^�}�S�t���O�i1:�^�}�S
 * @param   no		�]����I�t�Z�b�g�i�{�b�N�X������̂�0-29�j
 * @param   icon	�|�P�����A�C�R���̃Z���A�N�^�[�̃|�C���^
 * @param   handle	�A�[�J�C�u�n���h��
 * @param   pbuf	�������ݐ��VBLANK�]���p�o�b�t�@
 */
//----------------------------------------------------------------------------------
static void TransPokeIconCharaPal( int pokeno, int form, int gender, int tamago, int no, GFL_CLWK* icon, ARCHANDLE* handle, WORLDTRADE_POKEBUF *pbuf )
{
	u8 *pokepal;
	u8 *buf;
	NNSG2dCharacterData *chara;

	// �|�P�����A�C�R���̃L�����f�[�^���o�b�t�@�̓ǂݍ���

	buf = CharDataGetbyHandle( handle, POKEICON_GetCgxArcIndexByMonsNumber( pokeno, form, gender, tamago ), 
								&chara, HEAPID_WORLDTRADE );
	// VBLANK���ɃL�����N�^��]�����邽�߂̏���
	MI_CpuCopyFast(chara->pRawData, pbuf->chbuf, POKEICON_TRANS_SIZE);
	pbuf->vadrs = (POKEICON_VRAM_OFFSET+no*POKEICON_TRANS_CHARA)*0x20;
	pbuf->icon = icon;
	pbuf->palno = POKEICON_GetPalNum( pokeno, form, gender, tamago )+POKEICON_PAL_OFFSET;
	GFL_HEAP_FreeMemory(buf);
}


//------------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R���̓]���ƃZ���A�N�^�[�̕\���E��\���𐧌䂷��
 *
 * @param   paso	POKEMON_PASO_PARAM
 * @param   dat		GTS���M�f�[�^
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void PokemonLevelSet(POKEMON_PASO_PARAM *paso, Dpw_Tr_PokemonDataSimple *dat )
{
	dat->level = PokePasoLevelCalc( paso );
}

//------------------------------------------------------------------
/**
 * @brief   �|�P�����A�C�R���̓]���iVBLANK���Ɏ��s�j
 *
 * @param   work	GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void PokemonIconDraw(void *work)
{
  WORLDTRADE_WORK *wk = work;
  
  int i;
  WORLDTRADE_POKEBUF *pbuf = wk->boxicon;

  for(i = 0; i < BOX_POKE_NUM; i++, pbuf++){
    if(pbuf->icon){
      // �L�����N�^�[�]��
      DC_FlushRange(pbuf->chbuf, POKEICON_TRANS_SIZE);
      GX_LoadOBJ(pbuf->chbuf, pbuf->vadrs, POKEICON_TRANS_SIZE);
      // �p���b�g�ݒ�
			GFL_CLACT_WK_SetPlttOffs( pbuf->icon, pbuf->palno, CLWK_PLTTOFFS_MODE_PLTT_TOP );
    }
  }
  GFL_HEAP_FreeMemory( wk->boxicon );
}

//----------------------------------------------------------------------------------
/**
 * @brief	�|�P�����A�C�R���̓]���ƃZ���A�N�^�[�̕\���E��\���𐧌䂷��
 *
 * @param   paso		POKEMON_PASO_PARAM
 * @param   icon		�|�P�����A�C�R���̃Z���A�N�^�[
 * @param   itemact		�ǂ����A�C�R���̃Z���A�N�^�[
 * @param   no			�|�P����NO���擾���ď�������
 * @param   pos			�{�b�N�X���̉��Ԗڂ�
 * @param   handle		�A�[�J�C�u�n���h��
 * @param   dat			�w��̃|�P�����̊�{������������
 * @param   pbuf		VBLANK���ɓ]������f�[�^���������ރo�b�t�@
 */
//----------------------------------------------------------------------------------
static void PokemonIconSet( POKEMON_PASO_PARAM *paso, GFL_CLWK* icon, 
							GFL_CLWK* itemact, u16 *no, int pos, ARCHANDLE* handle, 
							Dpw_Tr_PokemonDataSimple *dat, WORLDTRADE_POKEBUF *pbuf )
{
	int flag,itemno, tamago,form;
	
	PPP_FastModeOn(paso);

	flag   = PPP_Get(paso, ID_PARA_poke_exist, NULL);
	*no    = PPP_Get(paso, ID_PARA_monsno,     NULL);
	form   = PPP_Get(paso, ID_PARA_form_no,    NULL);
	tamago = PPP_Get(paso, ID_PARA_tamago_flag,NULL);
	itemno = PPP_Get(paso, ID_PARA_item,       NULL);
	dat->characterNo = *no;
	dat->gender      = PPP_Get( paso, ID_PARA_sex, NULL )+1;

	// �����p�Ƀ^�}�S�̃��x�����O�Ƃ��Ă��܂��i�{���͂O����Ȃ����ǁj
	if(tamago){
		dat->level = 0;
	}

	PPP_FastModeOff(paso,TRUE);
	
	if(flag){
		TransPokeIconCharaPal( *no, form, dat->gender-1, tamago, pos, icon, handle, pbuf );
		GFL_CLACT_WK_SetDrawEnable( icon, 1 );

		// �A�C�e���������Ă��邩
		if(itemno!=0){	
			GFL_CLACT_WK_SetDrawEnable( itemact, 1 );

			// �A�C�e���̓��[�����H
			if(ITEM_CheckMail( itemno )){	
				GFL_CLACT_WK_SetAnmSeq( itemact, CELL_MAILICON_NO );	// ���[��
			}else{
				GFL_CLACT_WK_SetAnmSeq( itemact, CELL_ITEMICON_NO );	// �A�C�e��
			}
		}else{
			GFL_CLACT_WK_SetDrawEnable( itemact, 0 );
		}
	}else{
		GFL_CLACT_WK_SetDrawEnable( icon, 0 );
		GFL_CLACT_WK_SetDrawEnable( itemact, 0 );
		pbuf->icon = NULL;
	}
}

//------------------------------------------------------------------
/**
 * @brief   �{�b�N�X�E�Ă����̃|�P�����f�[�^��ǂݍ���
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   now		0-BoxTrayMax:�{�b�N�XNO�AWORLDTRADE_BOX_TEMOTI:�莝��
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void NowBoxPageInfoGet( WORLDTRADE_WORK *wk, int now)
{
	u16 monsno[30],i,flag;
	POKEMON_PARAM      *pp;
	POKEMON_PASO_PARAM *paso;
	BOX_MANAGER *boxdata = wk->param->mybox;
	ARCHANDLE* handle;
	WORLDTRADE_POKEBUF *pokebuf;
	
	// �|�P�����A�C�R����]������
	wk->boxicon = pokebuf =   GFL_HEAP_AllocMemoryLo(GFL_HEAPID_APP, sizeof(WORLDTRADE_POKEBUF) * BOX_POKE_NUM);

	handle = GFL_ARC_OpenDataHandle( ARCID_POKEICON, HEAPID_WORLDTRADE );

	// �{�b�N�X
	if(now>=0 && now < wk->BoxTrayMax ){
		for(i=0;i<BOX_POKE_NUM;i++){
		  PokemonLevelSet(BOXDAT_GetPokeDataAddress( boxdata, now, i ), &wk->boxWork->info[i] );
		}
		for(i=0;i<BOX_POKE_NUM;i++){
			wk->boxWork->info[i].characterNo = 0;
			PokemonIconSet(BOXDAT_GetPokeDataAddress( boxdata, now, i ), 
							wk->PokeIconActWork[i], wk->ItemIconActWork[i],
							&monsno[i], i, handle, &wk->boxWork->info[i], &pokebuf[i] );
			// �{�[���J�v�Z���̃Z���A�N�^�[�͂��ׂĉB��
			if(i<TEMOTI_POKEMAX){
				GFL_CLACT_WK_SetDrawEnable( wk->CBallActWork[i], 0 );
			}
		}

		// �{�b�N�X�̃g���C���擾
		BOXDAT_GetBoxName( boxdata, now, wk->BoxTrayNameString );
	
	}else if( now == WORLDTRADE_BOX_TEMOTI ){
	// �Ă���
		int num = PokeParty_GetPokeCount( wk->param->myparty );

		for(i=0;i<num;i++){
			pp   = PokeParty_GetMemberPointer(wk->param->myparty, i);
			paso = (POKEMON_PASO_PARAM *)PP_GetPPPPointerConst(pp);
			PokemonLevelSet(paso, &wk->boxWork->info[i] );
			PokemonIconSet( paso, wk->PokeIconActWork[i], wk->ItemIconActWork[i],
							&monsno[i], i, handle, &wk->boxWork->info[i], &pokebuf[i] );
      GFL_CLACT_WK_SetDrawEnable( wk->CBallActWork[i], 0 );
#if 0 //WB�ɃJ�X�^���{�[���͂Ȃ�
			// �{�[���J�v�Z�������Ă��邩�H
			if(PP_Get(pp, ID_PARA_cb_id, NULL)){
				GFL_CLACT_WK_SetDrawEnable( wk->CBallActWork[i], 1 );
			}else{
				GFL_CLACT_WK_SetDrawEnable( wk->CBallActWork[i], 0 );
			}
#endif 
		}

		// �Ă����̃|�P������\�����I��������͑S�Ĕ�\����
		for(;i<BOX_POKE_NUM;i++){
			wk->boxWork->info[i].characterNo = 0;
			GFL_CLACT_WK_SetDrawEnable( wk->PokeIconActWork[i], 0 );
			GFL_CLACT_WK_SetDrawEnable( wk->ItemIconActWork[i], 0 );
			pokebuf[i].icon = NULL;
			if(i<TEMOTI_POKEMAX){
				GFL_CLACT_WK_SetDrawEnable( wk->CBallActWork[i], 0 );
			}
		}
		GFL_MSG_GetString( wk->MsgManager, msg_gtc_04_023, wk->BoxTrayNameString );
	}
  else
  {
    GF_ASSERT_MSG(0,"BOX�͈͊O%d\n",now);
  }
	GFL_ARC_CloseDataHandle( handle );
	
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->SubWin), 0x0000 );
	WorldTrade_SysPrint( wk->SubWin, wk->BoxTrayNameString, 0, 5, 1, PRINTSYS_LSB_Make(1,2,0), &wk->print );
	
	// �����|�P�����I�����[�h�̎��͏����ɍ���Ȃ��|�P�������Â�����
	if(wk->sub_process_mode==MODE_EXCHANGE_SELECT){
		PokeIconPalSet( wk->boxWork->info, wk->PokeIconActWork, &wk->DownloadPokemonData[wk->TouchTrainerPos].wantSimple, pokebuf);
	}
	
	// pokebuf�̊J����PokemonIconDraw���ōs����
	wk->vfunc = PokemonIconDraw;
}


//==============================================================================
/**
 * @brief   �w�肵���|�P�����f�[�^��POKEMON_PARAM���APOKEMON_PASO_PARAM���H
 *
 * @param   tray	0-BoxTrayMax:�{�b�N�XNO�AWORLDTRADE_BOX_TEMOTI:�莝��
 *
 * @retval  int		0:POKEMON_PASO_PARAM 1:POKEMON_PARAM
 */
//==============================================================================
int WorldTrade_GetPPorPPP( int tray )
{
	if( tray == WORLDTRADE_BOX_TEMOTI ){
		return 1;
	}else{
		return 0;
	}
}

//==============================================================================
/**
 * @brief   �ԍ�����Ă������{�b�N�X�̒���POKEMON_PASO_PARAM�ւ̍\���̂�Ԃ�
 *
 * @param   party	POKEPARTY�|�C���^
 * @param   box		�{�b�N�X�̃Z�[�u�f�[�^
 * @param   tray	0-BoxTrayMax:�{�b�N�XNO�AWORLDTRADE_BOX_TEMOTI:�莝��
 * @param   pos		�{�b�N�X�ʒu�i�O�|�Q�X�j�莝���̏ꍇ��0-5
 *
 * @retval  POKEMON_PASO_PARAM *		�擾�����|�P�����f�[�^�̃|�C���^
 */
//==============================================================================
POKEMON_PASO_PARAM *WorldTrade_GetPokePtr( POKEPARTY *party, BOX_MANAGER *box,  int  tray, int pos )
{
	// �Ă���
	if(WorldTrade_GetPPorPPP( tray )){
		if(pos>(PokeParty_GetPokeCount(party)-1)){
			return NULL;
		}
		return  (POKEMON_PASO_PARAM *)PP_GetPPPPointerConst(PokeParty_GetMemberPointer( party, pos ));
	}

	// �{�b�N�X
	return BOXDAT_GetPokeDataAddress( box, tray, pos );
}

//==============================================================================
/**
 * @brief   �ԍ�����Ă������{�b�N�X�̒���POKEMON_PASO_PARAM�ւ̍\���̂�Ԃ�
 *
 * @param   party	POKEPARTY�|�C���^
 * @param   box		�{�b�N�X�̃Z�[�u�f�[�^
 * @param   tray	0-BoxTrayMax:�{�b�N�XNO�AWORLDTRADE_BOX_TEMOTI:�莝��
 *
 * @retval  int ����
 */
//==============================================================================
int WorldTrade_GetPokeMax( POKEPARTY *party, BOX_MANAGER *box,  int  tray )
{
	// �Ă���
	if(WorldTrade_GetPPorPPP( tray )){
    return PokeParty_GetPokeCount(party);
  }else{
    return BOXDAT_GetPokeExistCount( box, tray );
  }
}


//==============================================================================
/**
 * @brief   �Ă����|�P�����������Ƃ��Ɏc����`�F�b�N����
 *
 * @param   party	POKEPARTY�|�C���^
 * @param   box		�{�b�N�X�̃Z�[�u�f�[�^
 * @param   tray	0-BoxTrayMax:�{�b�N�XNO�AWORLDTRADE_BOX_TEMOTI:�莝��
 * @param   pos		�{�b�N�X�ʒu�i�O�|�Q�X�j�莝���̏ꍇ��0-5
 *
 * @retval  int		
 */
//==============================================================================
static int CheckPocket( POKEPARTY *party, BOX_MANAGER *box,  int  tray, int pos )
{
	// �Ă������������͎c��𐔂���
	if(WorldTrade_GetPPorPPP( tray )){
		if( PokeParty_GetPokeCount(party) < 2 ){
			return 0;
		}
	}
	return 1;
}



//==============================================================================
// �g�����{���p�̒�`
//==============================================================================
//------------------------------------------------------------------
/**
 * @brief   ���ꃊ�{���������Ă��邩�`�F�b�N
 *
 * @param   ppp		POKEMON_PASO_PARAM
 *
 * @retval  int		�����Ă��郊�{�����̐�
 */
//------------------------------------------------------------------
static int PokeRibbonCheck( POKEMON_PASO_PARAM *ppp )
{
	return GTS_TOOL_IsForbidRibbonPPP( ppp );
}

//------------------------------------------------------------------
/**
 * @brief   DP�ɂ͂Ȃ��t�H�����ɂȂ��Ă��Ȃ����`�F�b�N
 *
 * @param   ppp		POKEMON_PASO_PARAM
 *
 * @retval  int		0:��薳�� 1:����t�H�����ɂȂ��Ă��� 2:�M�U���s�`���[�Ȃ̂Ŗ��
 */
//------------------------------------------------------------------
static int PokeNewFormCheck( POKEMON_PASO_PARAM *ppp )
{
	int flag;
	int monsno, form_no;
	
	flag = PPP_FastModeOn(ppp);
	{
		monsno = PPP_Get(ppp, ID_PARA_monsno, NULL);
		form_no = PPP_Get(ppp, ID_PARA_form_no, NULL);
	}
	PPP_FastModeOff(ppp, flag);

	if(form_no > 0){
		switch(monsno){
		case MONSNO_GIRATHINA:
		case MONSNO_SHEIMI:
		case MONSNO_ROTOMU:
			return 1;
			break;
		case MONSNO_PITYUU:		// �M�U���s�`���[�Ή� 
			return 2;
			break;
		}
	}
	return 0;
}

//------------------------------------------------------------------
/**
 * @brief   DP�ɂ͂Ȃ��A�C�e���������Ă��Ȃ����`�F�b�N
 *
 * @param   ppp		POKEMON_PASO_PARAM
 *
 * @retval  int		TRUE:�V�K�A�C�e���������Ă���
 * @retval  int		FALSE:�V�K�A�C�e���͎����Ă��Ȃ�
 */
//------------------------------------------------------------------
static int PokeNewItemCheck( POKEMON_PASO_PARAM *ppp )
{
	int flag;
	int item;
	
	flag = PPP_FastModeOn(ppp);
	{
		item = PPP_Get(ppp, ID_PARA_item, NULL);
	}
	PPP_FastModeOff(ppp, flag);

	switch(item){
	case ITEM_HAKKINDAMA:
		return TRUE;
	}
	return FALSE;
}

//==============================================================================
/**
 * @brief   �w��̃{�b�N�XNO�EPOS�ɂ̓|�P���������邩�H
 *
 * @param   party	POKEPARTY�|�C���^
 * @param   box		BOX�Z�[�u�f�[�^
 * @param   tray	0-BoxTrayMax:�{�b�N�XNO�AWORLDTRADE_BOX_TEMOTI:�莝��
 * @param   pos		�{�b�N�X�ʒu�i�O�|�Q�X�j�莝���̏ꍇ��0-5
 *
 * @retval  int		WANT_POKE_NO, WANT_POME_YES, WANT_POKE_TAMAGO
 */
//==============================================================================
static int PokemonCheck( POKEPARTY *party, BOX_MANAGER *box,  int  tray, int pos  )
{
	POKEMON_PASO_PARAM *ppp = WorldTrade_GetPokePtr(party, box, tray, pos );
	
#ifndef CHANGE_POKE_RULE_IGNORE
	
	// NULL�`�F�b�N
	if(ppp==NULL){
		return WANT_POKE_NO;
	}

	// �|�P�����͂��邩�H
	if(!PPP_Get(ppp, ID_PARA_poke_exist, NULL)){
		return WANT_POKE_NO;
	}

	// �^�}�S����Ȃ����H
	if(PPP_Get(ppp, ID_PARA_tamago_exist, NULL)){
		return WANT_POKE_TAMAGO;
	}

#endif

	return WANT_POKE_OK;
}


//------------------------------------------------------------------
/**
 * @brief   ���������ɍ��v���Ă��邩��r����
 *
 * @param   poke		��������
 * @param   search		��������
 *
 * @retval  int		0:���v���Ă��Ȃ�	1:���v���Ă���
 */
//------------------------------------------------------------------
static int CompareSearchData( Dpw_Tr_PokemonDataSimple *poke,  Dpw_Tr_PokemonSearchData *search )
{
#ifndef CHANGE_POKE_RULE_IGNORE
	// �|�P�������Ⴄ
	if( poke->characterNo != search->characterNo){
		return 0;
	}
	
	// ���ʎw��ƈႤ
	if( search->gender!=DPW_TR_GENDER_NONE){
		if(search->gender != poke->gender){
			return 0;
		}
	}
	
	// �|�P�������^�}�S
	if(poke->level==0){
		return 0;
	}
	
	// �w��̍Œ჌�x�������Ⴂ
	if( search->level_min!=0){
		if( search->level_min > poke->level){
			return 0;
		}
	}
	// �w��̍ō����x����������
	if( search->level_max!=0){
		if( search->level_max < poke->level){
			return 0;
		}
	}
#endif
	
	// ���v���Ă���
	return 1;

}

//------------------------------------------------------------------
/**
 * @brief   ���肪�v������|�P�����ɂ����Ă��邩�H
 *
 * @param   ppp		POKEMON_PASO_PARAM
 * @param   dtp		��������
 *
 * @retval  int		0:���v���ĂȂ�	1:���Ă�
 */
//------------------------------------------------------------------
static int WantPokeCheck(POKEMON_PASO_PARAM *ppp, Dpw_Tr_PokemonSearchData *dtsd)
{
	Dpw_Tr_PokemonDataSimple temp;
	temp.characterNo = PPP_Get(ppp, ID_PARA_monsno, NULL);
	temp.gender      = PPP_Get( ppp, ID_PARA_sex,   NULL )+1;
	temp.level       = PokePasoLevelCalc(ppp);

	OS_Printf("want pokeno = %d, mypoke    = %d\n", dtsd->characterNo, temp.characterNo);
	OS_Printf("want sex    = %d, minesex   = %d\n", dtsd->gender     , temp.gender   );
	OS_Printf("want level_min  = %d, max  = %d, level = %d\n", dtsd->level_min,dtsd->level_max, temp.level );

	// ��r����
	return CompareSearchData( &temp, dtsd );
}

//------------------------------------------------------------------
/**
 * @brief   ���M�f�[�^�̏������s��
 *
 * @param   dtd		GTS���M�f�[�^�o�b�t�@
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void MakeExchangePokemonData( Dpw_Tr_Data *dtd, WORLDTRADE_WORK *wk )
{
	Dpw_Tr_PokemonDataSimple post;
	Dpw_Tr_PokemonSearchData want;
	POKEMON_PASO_PARAM *ppp;

	// ���O�E���ʁE���x���擾
	post.characterNo = PPP_Get( wk->deposit_ppp, ID_PARA_monsno, NULL );
	post.gender      = PPP_Get( wk->deposit_ppp, ID_PARA_sex,   NULL )+1;
	post.level       = PokePasoLevelCalc( wk->deposit_ppp );

	dtd->postSimple = post;

	WorldTrade_PostPokemonBaseDataMake( dtd, wk );

	ppp = (POKEMON_PASO_PARAM *)PP_GetPPPPointerConst( (POKEMON_PARAM*)wk->DownloadPokemonData[wk->TouchTrainerPos].postData.data );

	want.characterNo = PPP_Get( ppp, ID_PARA_monsno, NULL );
	want.gender      = PPP_Get( ppp, ID_PARA_sex,   NULL )+1;
	want.level_min   = 0;
	want.level_max   = 0;
	dtd->wantSimple  = want;

	OS_Printf( "postData  No = %d,  gender = %d, level = %d\n", dtd->postSimple.characterNo, dtd->postSimple.gender, dtd->postSimple.level);

}

//------------------------------------------------------------------
/**
 * @brief   ���������ɍ���Ȃ��|�P�����A�C�R���̃J���[���Â�����
 *
 * @param   box		GTS�|�P������{���
 * @param   icon	�|�P�����A�C�R���Z���A�N�^�[
 * @param   want	
 *
 * @retval  none		
 */
//------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * @brief	���������ɍ���Ȃ��|�P�����A�C�R���̃J���[���Â�����
 *
 * @param   box		GTS�|�P������{���
 * @param   icon	�|�P�����A�C�R���Z���A�N�^�[	
 * @param   want	��������
 * @param   pbuf	VBLANK�]���p�摜�o�b�t�@
 */
//----------------------------------------------------------------------------------
static void PokeIconPalSet( Dpw_Tr_PokemonDataSimple *box, GFL_CLWK* *icon, Dpw_Tr_PokemonSearchData *want, WORLDTRADE_POKEBUF *pbuf)
{
	int pal,i;
	for(i=0;i<BOX_POKE_NUM;i++){
		if(box[i].characterNo!=0){
			if(CompareSearchData( &box[i], want )==0){
				pbuf[i].palno += 3;
			}
		}
	}
}


//------------------------------------------------------------------
/**
 * @brief   ���[���������Ă��邩�H
 *
 * @param   pp		POKEMON_PARAM
 *
 * @retval  BOOL	TRUE:���[�������Ă�	FALSE:�����ĂȂ�
 */
//------------------------------------------------------------------
BOOL WorldTrade_PokemonMailCheck( POKEMON_PARAM *pp )
{
	int itemno = PP_Get( pp, ID_PARA_item,       NULL);
	// �A�C�e���̓��[�����H
	if(ITEM_CheckMail( itemno )){
		return TRUE;
	}
	
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �{�b�N�X�̃g���C�ő吔���`�F�b�N����
 *
 *	@param	WORLDTRADE_WORK *wk ���[�N
 */
//-----------------------------------------------------------------------------
static void WorldTrade_CheckBoxTray( WORLDTRADE_WORK *wk )
{
  wk->BoxTrayMax  = BOXDAT_GetTrayMax( wk->param->mybox );

  //�g���C���ő�ɂȂ��Ă��Ȃ�������g���C�����`�F�b�N���s�Ȃ�
  if( wk->BoxTrayMax < BOX_MAX_TRAY )
  {
    int i;
    //���݂̃g���C�S�ĂɃ|�P�������P�C�ł������瑝������
    for( i = 0; i < wk->BoxTrayMax; i++ )
    {
      if( BOXDAT_GetPokeExistCount( wk->param->mybox, i ) == 0 )
      {
        break;
      }
    }

    if( i == wk->BoxTrayMax )
    {
      wk->BoxTrayMax = BOXDAT_AddTrayMax( wk->param->mybox );
    }
  }
}
