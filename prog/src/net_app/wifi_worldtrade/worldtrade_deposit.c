//============================================================================================
/**
 * @file	worldtrade_deposit.c
 * @brief	���E�����|�P�����a�����ʏ���
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
#include "system/bmp_winframe.h"
#include "sound/pm_sndsys.h"
#include "poke_tool/poke_tool.h"
#include "poke_tool/pokeparty.h"
#include "poke_tool/monsno_def.h"

#include "savedata/wifilist.h"
#include "net_app/worldtrade.h"
#include "worldtrade_local.h"
#include "msg/msg_wifi_lobby.h"
#include "msg/msg_wifi_gts.h"
#include "print/printsys.h"

#include "worldtrade.naix"			// �O���t�B�b�N�A�[�J�C�u��`
#include "zukan_data.naix"
#include "../../../resource/zukan_data/zkn_sort_aiueo_idx.h"


#include "msg/msg_wifi_place_msg_world.h"


//============================================================================================
//	�v���g�^�C�v�錾
//============================================================================================
/*** �֐��v���g�^�C�v ***/
static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat );
static void BgInit( void );
static void BgExit( void );
static void BgGraphicSet( WORLDTRADE_WORK * wk );
static void BmpWinInit( WORLDTRADE_WORK *wk );
static void BmpWinDelete( WORLDTRADE_WORK *wk );
static void InitWork( WORLDTRADE_WORK *wk );
static void FreeWork( WORLDTRADE_WORK *wk );
static int SubSeq_Start( WORLDTRADE_WORK *wk);
static int SubSeq_Main( WORLDTRADE_WORK *wk);
static int RoundWork( int num, int max, int move );
static int SubSeq_HeadWordSelectList( WORLDTRADE_WORK *wk );
static int SubSeq_HeadWordSelectWait( WORLDTRADE_WORK *wk );
static int SubSeq_PokeNameSelectList( WORLDTRADE_WORK *wk );
static int SubSeq_PokeNameSelectWait( WORLDTRADE_WORK *wk );
static int SubSeq_End( WORLDTRADE_WORK *wk);
static int SubSeq_YesNo( WORLDTRADE_WORK *wk);
static int SubSeq_MessageWait( WORLDTRADE_WORK *wk );
#if 0
static void PokeWantPrint( GFL_MSGDATA *MsgManager, GFL_MSGDATA *MonsNameManager,WORDSET *WordSet, GFL_BMPWIN win[], int monsno, int sex, int level );
#endif
static void PokeDepositPrint( 
	GFL_MSGDATA *MsgManager, 
	WORDSET *WordSet, 
	GFL_BMPWIN *win[], 
	POKEMON_PASO_PARAM *ppp,
	Dpw_Tr_PokemonDataSimple *post,
	WT_PRINT *print );
static int 	SubSeq_SexSelctWait( WORLDTRADE_WORK *wk );
static int 	SubSeq_SexSelctList( WORLDTRADE_WORK *wk );
static int 	SubSeq_SexSelctMessage( WORLDTRADE_WORK *wk );
static int  SubSeq_LevelSelectMessage( WORLDTRADE_WORK *wk );
static int  SubSeq_LevelSelectWait( WORLDTRADE_WORK *wk );
static int  SubSeq_LevelSelectList( WORLDTRADE_WORK *wk );
static int  SubSeq_DepositOkMessage( WORLDTRADE_WORK *wk );
static int  SubSeq_DepositOkYesNo( WORLDTRADE_WORK *wk );
static int SubSeq_DepositOkYesNoWait( WORLDTRADE_WORK *wk );
static void DepositPokemonDataMake( Dpw_Tr_Data *dtd, WORLDTRADE_WORK *wk );


static int PokeNameSortListMake( BMP_MENULIST_DATA **menulist, GFL_MSGDATA *monsnameman, 
									GFL_MSGDATA *msgman,u16 *table, u8 *sinou,
									int num, int select,ZUKAN_SAVEDATA *zukan );
static u16* ZukanSortDataGet( int heap, int idx, int* p_arry_num );
static PRINTSYS_LSB GetSexColor( int sex, PRINTSYS_LSB color );
static void SetCellActor(WORLDTRADE_WORK *wk);
static void DelCellActor(WORLDTRADE_WORK *wk);



enum{
	SUBSEQ_START=0,
	SUBSEQ_MAIN,
	SUBSEQ_END,

	SUBSEQ_HEADWORD_SELECT_LIST,
    SUBSEQ_HEADWORD_SELECT_WAIT,
    SUBSEQ_POKENAME_SELECT_LIST,
    SUBSEQ_POKENAME_SELECT_WAIT,
	SUBSEQ_SEX_SELECT_MES,
	SUBSEQ_SEX_SELECT_LIST,
	SUBSEQ_SEX_SELECT_WAIT,
	SUBSEQ_LEVEL_SELECT_MES,
	SUBSEQ_LEVEL_SELECT_LIST,
	SUBSEQ_LEVEL_SELECT_WAIT,
	SUBSEQ_DEPOSITOK_MESSAGE,
	SUBSEQ_DEPOSIT_YESNO,
	SUBSEQ_DEPOSIT_YESNO_WAIT,
	
	SUBSEQ_MES_WAIT,
};
static int (*Functable[])( WORLDTRADE_WORK *wk ) = {
	SubSeq_Start,				// SUBSEQ_START=0,
	SubSeq_Main,    	        // SUBSEQ_MAIN,
	SubSeq_End,         	    // SUBSEQ_END,
	SubSeq_HeadWordSelectList,	// SUBSEQ_HEADWORD_SELECT_LIST
	SubSeq_HeadWordSelectWait,	// SUBSEQ_HEADWORD_SELECT_WAIT
	SubSeq_PokeNameSelectList,	// SUBSEQ_POKENAME_SELECT_LIST
	SubSeq_PokeNameSelectWait,	// SUBSEQ_POKENAME_SELECT_WAIT
	SubSeq_SexSelctMessage,		// SUBSEQ_SEX_SELECT_MES,
	SubSeq_SexSelctList,		// SUBSEQ_SEX_SELECT_LIST,
	SubSeq_SexSelctWait,		// SUBSEQ_SEX_SELECT_WAIT,
	SubSeq_LevelSelectMessage,	// SUBSEQ_LEVEL_SELECT_MES,
    SubSeq_LevelSelectList,     // SUBSEQ_LEVEL_SELECT_LIST,
    SubSeq_LevelSelectWait,     // SUBSEQ_LEVEL_SELECT_WAIT,
	SubSeq_DepositOkMessage,
    SubSeq_DepositOkYesNo,
	SubSeq_DepositOkYesNoWait,

	SubSeq_MessageWait,			// SUBSEQ_MES_WAIT
};

#define BOX_CUROSOR_END_POS			( 30 )
#define BOX_CUROSOR_TRAYNAME_POS	( 31 )


// �ق����|�P�����E��������|�P�������
#define INFORMATION_STR_X	(  0 )
#define INFORMATION_STR_Y	(  3 )
#define INFORMATION2_STR_X	(  1 )
#define INFORMATION2_STR_Y	(  5 )
#define INFORMATION3_STR_X	(  2 )
#define INFORMATION3_STR_Y	(  7 )
#define INFORMATION_STR_SX	( 12 )
#define INFORMATION_STR_SY	(  2 )


#define TITLE_MESSAGE_OFFSET   ( WORLDTRADE_MENUFRAME_CHR + MENU_WIN_CGX_SIZ )
#define LINE_MESSAGE_OFFSET    ( TITLE_MESSAGE_OFFSET     + TITLE_TEXT_SX*TITLE_TEXT_SY )
#define INFOMATION_STR_OFFSET  ( LINE_MESSAGE_OFFSET      + LINE_TEXT_SX*LINE_TEXT_SY )
#define YESNO_OFFSET 		   ( INFOMATION_STR_OFFSET    + INFORMATION_STR_SX*INFORMATION_STR_SY*6 )

static const u16 infomation_bmp_table[6][2]={
	{ INFORMATION_STR_X,  INFORMATION_STR_Y,     },	// �u�ق����|�P�����v
	{ INFORMATION2_STR_X, INFORMATION2_STR_Y,    },	// �|�P������
	{ INFORMATION3_STR_X, INFORMATION3_STR_Y,    },	// ���x���w��
	{ INFORMATION_STR_X,  INFORMATION_STR_Y  +8, },	// �u��������|�P�����v
	{ INFORMATION2_STR_X, INFORMATION2_STR_Y +8, },	// �|�P������
	{ INFORMATION3_STR_X, INFORMATION3_STR_Y +8, },	// ���x��

};

// �������x���w��p�\����
typedef struct{
	int msg;
	s16 min;
	s16 max;
}WT_LEVEL_TERM;

// �������x����(�Ō�̓L�����Z�������ǁj
#define LEVEL_SELECT_NUM	(12)

// �������x���w��e�[�u���F�a���鎞�̃}�b�`���O�����w��
static const WT_LEVEL_TERM level_minmax_table[]={
	{ msg_gtc_12_001, 0,   0, },	// ���ɂ��Ȃ�
	{ msg_gtc_12_002, 0,   9, },	// ���x���P�O�݂܂�
	{ msg_gtc_12_003,10,   0, },	// ���x���P�O�ȏ�
	{ msg_gtc_12_004,20,   0, },	// ���x���Q�O�ȏ�
	{ msg_gtc_12_005,30,   0, },	// ���x���R�O�ȏ�
	{ msg_gtc_12_006,40,   0, },	// ���x���S�O�ȏ�
	{ msg_gtc_12_007,50,   0, },	// ���x���T�O�ȏ�
	{ msg_gtc_12_008,60,   0, },	// ���x���U�O�ȏ�
	{ msg_gtc_12_009,70,   0, },	// ���x���V�O�ȏ�
	{ msg_gtc_12_010,80,   0, },	// ���x���W�O�ȏ�
	{ msg_gtc_12_011,90,   0, },	// ���x���X�O�ȏ�
	{ msg_gtc_12_012,100,100, },	// ���x���P�O�O
};


// �������x���w��e�[�u���F�������鎞�̃}�b�`���O�����w��
static const WT_LEVEL_TERM search_level_minmax_table[]={
	{ msg_gtc_search_001, 0,   0, },	// ���ɂ��Ȃ�
	{ msg_gtc_search_002, 1,   10, },	// ���x��1����10
	{ msg_gtc_search_003,11,   20, },	// ���x��11����20
	{ msg_gtc_search_004,21,   30, },	// ���x��21����30
	{ msg_gtc_search_005,31,   40, },	// ���x��31����40
	{ msg_gtc_search_006,41,   50, },	// ���x��41����50
	{ msg_gtc_search_007,51,   60, },	// ���x��51����60
	{ msg_gtc_search_008,61,   70, },	// ���x��61����70
	{ msg_gtc_search_009,71,   80, },	// ���x��71����80
	{ msg_gtc_search_010,81,   90, },	// ���x��81����90
	{ msg_gtc_search_011,91,   100, },	// ���x��91����100
};

// �����}�b�`���O�F�������x����(�Ō�̓L�����Z�������ǁj
#define SEARCH_LEVEL_SELECT_NUM		(11)

//==============================================================================
//	
//==============================================================================
///�������F�܏\�������X�g
ALIGN4 static const u16 CountryListTbl[] = {
	country093,
	country098,
	country001,
	country220,
	country221,
	country218,
	country003,
	country009,
	country002,
	country006,
	country008,
	country219,
	country029,
	country100,
	country101,
	country097,
	country094,
	country095,
	country216,
	country222,
	country060,
	country061,
	country062,
	country012,
	country013,
	country157,
	country146,
	country078,
	country088,
	country036,
	country074,
	country035,
	country110,
	country034,
	country086,
	country054,
	country080,
	country085,
	country083,
	country111,
	country049,
	country081,
	country082,
	country052,
	country107,
	country050,
	country045,
	country048,
	country179,
	country186,
	country079,
	country102,
	country187,
	country200,
	country199,
	country193,
	country196,
	country194,
	country188,
	country189,
	country198,
	country183,
	country205,
	country202,
	country204,
	country055,
	country040,
	country043,
	country211,
	country042,
	country056,
	country077,
	country207,
	country058,
	country059,
	country212,
	country152,
	country150,
	country151,
	country103,
	country148,
	country149,
	country156,
	country016,
	country089,
	country158,
	country160,
	country224,
	country015,
	country161,
	country023,
	country018,
	country092,
	country017,
	country069,
	country164,
	country070,
	country028,
	country071,
	country072,
	country031,
	country033,
	country227,
	country022,
	country226,
	country021,
	country163,
	country020,
	country166,
	country027,
	country025,
	country167,
	country091,
	country090,
	country122,
	country129,
	country131,
	country126,
	country192,
	country142,
	country135,
	country133,
	country140,
	country104,
	country118,
	country117,
	country171,
	country121,
	country115,
	country172,
};

const u32 CountryListTblNum = NELEMS(CountryListTbl);


// �A�C�E�G�I���ō���CountryList�����Ԗڂɏo�����邩��
// �i�[���Ă��郊�X�g
// {  CountryList�̓Y����,   �A�C�E�G�I�̓Y�����i�A=0,�J=5,��=35,��=38)
static const u8 CountrySortList[][2]={
	{ 0,	0,},	// �A
	{ 11,	1,},	// 
	{ 18,	2,},	// 
	{ 20,	3,},
	{ 23,	4,},
	{ 27,	5,},	// �J
	{ 34,	6,},
	{ 37,	7,},
	{ 44,	8,},
	{ 45,	9,},
	{ 48,	10,},	// �T
	{ 49,	11,},
	{ 53,	12,},
	{ 61,	13,},
	{ 62,	15,},	// �^
	{ 65,	16,},
	{ 70,	18,},
	{ 71,	19,},
	{ 76,	20,},	// �i
	{ 77,	21,},
	{ 82,	24,},
	{ 83,	25,},	// �n
	{ 94,	27,},	// �t
	{ 102,	28,},	// �w	���R�R�𒼂���
	{ 108,	29,},	// �z
	{ 114,	30,},	// �}
	{ 118,	31,},
	{ 120,	33,},
	{ 121,	34,},
	{ 123,	37,},	// ��
	{ 124,	39,},	// ��
	{ 126,	40,},	// ��
	{ 128,	41,},	// ��
	{ 129,	42,},	// ��
#if GS_BTS5549_20090710_FIX
	{ 130,  255},	// �ԕ�
#else
	{ 129,  255},	// �ԕ�
#endif
					// ���͖���
};



#define COUNTRY_SORT_HEAD_MAX	( 35 )


//============================================================================================
//	�v���Z�X�֐�
//============================================================================================

//==============================================================================
/**
 * @brief   ���E�����������ʏ�����
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   seq		�i���g�p�j
 *
 * @retval  int		�A�v���V�[�P���X
 */
//==============================================================================
int WorldTrade_Deposit_Init(WORLDTRADE_WORK *wk, int seq)
{
	// ���[�N������
	InitWork( wk );
	
	// BG�ݒ�
	BgInit( );
	// �T�u��ʏ�����
	WorldTrade_SubLcdBgInit( wk, 0, 0 );

	// BG�O���t�B�b�N�]��
	BgGraphicSet( wk );

	// BMPWIN�m��
	BmpWinInit( wk );

	SetCellActor(wk);


	WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_deposit.c [226] M ********************\n" );
#endif

	// �ʐM��Ԃ��m�F���ăA�C�R���̕\����ς���
    WorldTrade_WifiIconAdd( wk );
   
    // �ق����|�P�����E��������|�P�����`��
	WodrldTrade_PokeWantPrint( wk->MsgManager, wk->MonsNameManager, 
				wk->WordSet, &wk->InfoWin[0], 0,DPW_TR_GENDER_NONE,-1, &wk->print);
	PokeDepositPrint( wk->MsgManager, wk->WordSet, &wk->InfoWin[3], wk->deposit_ppp, &wk->Post, &wk->print );

	// �������̓V�X�e��������
	{
		WORLDTRADE_INPUT_HEADER wih;
		wih.MenuBmp   = &wk->MenuWin[0];
		wih.BackBmp   = &wk->BackWin;
		wih.CursorAct = wk->SubCursorActWork;
		wih.ArrowAct[0] = wk->BoxArrowActWork[0];
		wih.ArrowAct[1] = wk->BoxArrowActWork[1];
		wih.SearchCursorAct = NULL;
		wih.MsgManager      = wk->MsgManager;
		wih.MonsNameManager = wk->MonsNameManager;
		wih.CountryNameManager = wk->CountryNameManager;
		wih.Zukan			= wk->param->zukanwork;
		wih.SinouTable      = wk->dw->sinouTable;
		wih.config					= wk->param->config;
		wk->WInputWork = WorldTrade_Input_Init( &wih,  GFL_BG_FRAME2_M, SITUATION_DEPOSIT );
	}
	

	OS_TPrintf("�}�ӑ��� = %d\n",wk->dw->nameSortNum);


	wk->subprocess_seq = SUBSEQ_START;
  wk->sub_display_continue  = FALSE;
	
	return SEQ_FADEIN;
}

//==============================================================================
/**
 * @brief   ���E�����������ʃ��C��
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   seq		�i���g�p�j
 *
 * @retval  int		�A�v���V�[�P���X
 */
//==============================================================================
int WorldTrade_Deposit_Main(WORLDTRADE_WORK *wk, int seq)
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
 * @param   seq		�i���g�p�j
 *
 * @retval  int		�A�v���V�[�P���X
 */
//==============================================================================
int WorldTrade_Deposit_End(WORLDTRADE_WORK *wk, int seq)
{
	//WirelessIconEasyEnd();

	DelCellActor(wk);

	// �������̓V�X�e���I��
	WorldTrade_Input_Exit( wk->WInputWork );

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


//------------------------------------------------------------------
/**
 * @brief   ��b�E�C���h�E�\��
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
//----------------------------------------------------------------------------------
/**
 * @brief	��b�E�C���h�E�\��
 *
 * @param   wk		GTS��ʃ��[�N
 * @param   msgno	���b�Z�[�WNO
 * @param   wait	�E�F�C�g
 * @param   flag	���g�p
 * @param   dat		���g�p
 */
//----------------------------------------------------------------------------------
static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat )
{
	// ������擾
	STRBUF *tempbuf;
	
	// ������擾
	tempbuf = GFL_MSG_CreateString(  wk->MsgManager, msgno );

	// WORDSET�W�J
	WORDSET_ExpandStr( wk->WordSet, wk->TalkString, tempbuf );
	

	// ��b�E�C���h�E�g�`��
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin),  0x0f0f );
	BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, WORLDTRADE_MESFRAME_CHR, WORLDTRADE_MESFRAME_PAL );

	// ������`��J�n
	GF_STR_PrintSimple( wk->MsgWin, FONT_TALK, wk->TalkString, 0, 0, &wk->print);
	GFL_BMPWIN_MakeTransWindow(wk->MsgWin);

	GFL_STR_DeleteBuffer(tempbuf);
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

	// ���C����ʃe�L�X�g��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000, GFL_BG_CHRSIZ_256x256,GX_BG_EXTPLTT_01,
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
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M, TRUE );
	}

	// ���C����ʔw�i��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_FillScreen(	GFL_BG_FRAME2_M, 0x0000, 0, 0, 32, 24, 0 );
		GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M, TRUE );
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

	GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 32, 0, HEAPID_WORLDTRADE );
	GFL_BG_SetClearCharacter( GFL_BG_FRAME3_M, 32, 0, HEAPID_WORLDTRADE );


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


	// ���C����ʂa�f�����
	GFL_BG_FreeBGControl(GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
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
	GFL_ARC_UTIL_TransVramPalette( ARCID_WORLDTRADE_GRA, NARC_worldtrade_deposit_nclr, PALTYPE_MAIN_BG, 0, 16*2*2,  HEAPID_WORLDTRADE);
	
	// ��b�t�H���g�p���b�g�]��
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

	// ��b�E�C���h�E�O���t�B�b�N�]��
	BmpWinFrame_GraphicSet(	GFL_BG_FRAME0_M, WORLDTRADE_MESFRAME_CHR, 
						WORLDTRADE_MESFRAME_PAL,  0, HEAPID_WORLDTRADE );


	BmpWinFrame_GraphicSet(	GFL_BG_FRAME0_M, WORLDTRADE_MENUFRAME_CHR,
						WORLDTRADE_MENUFRAME_PAL, 0, HEAPID_WORLDTRADE );


	// ���C�����BG1�L�����]��
	GFL_ARC_UTIL_TransVramBgCharacter( ARCID_WORLDTRADE_GRA, NARC_worldtrade_deposit_lz_ncgr, GFL_BG_FRAME1_M, 0, 0, 1, HEAPID_WORLDTRADE);

	// ���C�����BG1�X�N���[���]��
	GFL_ARC_UTIL_TransVramScreen(   ARCID_WORLDTRADE_GRA, NARC_worldtrade_deposit_lz_nscr, GFL_BG_FRAME1_M, 0, 32*24*2, 1, HEAPID_WORLDTRADE);

	// ���C�����BG2�L�����]��
	GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_worldtrade_input_lz_ncgr, GFL_BG_FRAME2_M, 0, 16*3*0x20, 1, HEAPID_WORLDTRADE);

	WorldTrade_SubLcdWinGraphicSet( wk );   // �g���[�h���[���E�C���h�E�]��

	GFL_ARC_CloseDataHandle( p_handle );

}

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
	int i;
	// ---------- ���C����� ------------------

	// BG0��BMPWIN�^�C�g���E�C���h�E�m�ہE�`��
#if 0	
	GF_BGL_BmpWinAdd( &wk->TitleWin, GFL_BG_FRAME0_M,
	TITLE_TEXT_X, TITLE_TEXT_Y, TITLE_TEXT_SX, TITLE_TEXT_SY, WORLDTRADE_TALKFONT_PAL,  TITLE_MESSAGE_OFFSET );

	GFL_BMP_Clear( &wk->TitleWin, 0x0000 );
	
	// �u�|�P��������������v�`��
	WorldTrade_TalkPrint( &wk->TitleWin, wk->TitleString, 0, 1, 0, PRINTSYS_LSB_Make(15,14,0) );
#endif		//����ŕs�v

	// ��s�E�C���h�E
	wk->MsgWin= GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
		LINE_TEXT_X, LINE_TEXT_Y, LINE_TEXT_SX, LINE_TEXT_SY, 
		WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0x0000 );
	GFL_BMPWIN_MakeTransWindow(wk->MsgWin);
//	Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_05_001, MSG_ALLPUT, 0 );


	// ��������|�P�����E�ق����|�P�������BMPWIN�m��
	for(i=0;i<6;i++){
		wk->InfoWin[i] = GFL_BMPWIN_Create( GFL_BG_FRAME3_M,
			infomation_bmp_table[i][0], infomation_bmp_table[i][1], 
			INFORMATION_STR_SX, INFORMATION_STR_SY, WORLDTRADE_TALKFONT_PAL,  
			GFL_BMP_CHRAREA_GET_B  );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin[i]), 0x0000 );
		GFL_BMPWIN_MakeTransWindow(wk->InfoWin[i]);
	}

	// �T�u��ʂ�GTS�����pBMPWIN���m�ۂ���
	WorldTrade_SubLcdExpainPut( wk, EXPLAIN_AZUKERU );

	OS_Printf("WORLDTRADE heap�c�� = %d\n",GFL_HEAP_GetHeapFreeSize( HEAPID_WORLDTRADE ));
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
	int i;

  //BMP�������O�Ɏc�����X�g���[��������
  WT_PRINT_ClearBuffer( &wk->print );
	
	GFL_BMPWIN_Delete( wk->ExplainWin );
	for(i=0;i<6;i++){
		GFL_BMPWIN_Delete( wk->InfoWin[i] );
	}
	GFL_BMPWIN_Delete( wk->MsgWin );
///	GFL_BMPWIN_Delete( wk->TitleWin );


}

//------------------------------------------------------------------
/**
 * �Z���A�N�^�[�o�^
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void SetCellActor(WORLDTRADE_WORK *wk)
{
	//�o�^���i�[
	GFL_CLWK_DATA	add;
	//WorldTrade_MakeCLACT( &add,  wk, &wk->clActHeader_main, NNS_G2D_VRAM_TYPE_2DMAIN );

	GFL_STD_MemClear(&add,sizeof(GFL_CLWK_DATA));

	// ���͗p�J�[�\���o�^
	add.pos_x = 160;
	add.pos_y = 32;
	wk->SubCursorActWork = GFL_CLACT_WK_Create(wk->clactSet,
			wk->resObjTbl[RES_CURSOR][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[RES_CURSOR][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
	GFL_CLACT_WK_SetAnmSeq( wk->SubCursorActWork, 12 );
	GFL_CLACT_WK_SetAutoAnmFlag( wk->SubCursorActWork, 1 );
	GFL_CLACT_WK_StopAnm( wk->SubCursorActWork );
	GFL_CLACT_WK_SetDrawEnable( wk->SubCursorActWork, 0 );

	// ���͗p�y�[�W�ړ��J�[�\���o�^�i�E�����j
	add.pos_x = 228;
	add.pos_y = 117;
	wk->BoxArrowActWork[0] = GFL_CLACT_WK_Create(wk->clactSet,
			wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
	GFL_CLACT_WK_SetAnmSeq( wk->BoxArrowActWork[0], CELL_BOXARROW_NO );
	GFL_CLACT_WK_SetDrawEnable( wk->BoxArrowActWork[0], 0 );
	// ���͗p�y�[�W�ړ��J�[�\���o�^(�������j
	add.pos_x = 140;
	wk->BoxArrowActWork[1] = GFL_CLACT_WK_Create(wk->clactSet,
			wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
	GFL_CLACT_WK_SetAnmSeq( wk->BoxArrowActWork[1], CELL_BOXARROW_NO+1 );
	GFL_CLACT_WK_SetDrawEnable( wk->BoxArrowActWork[1], 0 );
}

//------------------------------------------------------------------
/**
 * @brief   �o�^�����Z���A�N�^�[�̍폜
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void DelCellActor( WORLDTRADE_WORK *wk )
{
	GFL_CLACT_WK_Remove(wk->SubCursorActWork);
	GFL_CLACT_WK_Remove(wk->BoxArrowActWork[0]);
	GFL_CLACT_WK_Remove(wk->BoxArrowActWork[1]);


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


	// ������o�b�t�@�쐬
	wk->TalkString  = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WORLDTRADE );

	// �|�P��������������
//	wk->TitleString = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_05_001 );����ŕs�v

	// �|�P�������e�[�u���쐬�p�̃��[�N�m��
	wk->dw = GFL_HEAP_AllocMemory(HEAPID_WORLDTRADE, sizeof(DEPOSIT_WORK));
	MI_CpuClearFast( wk->dw, sizeof(DEPOSIT_WORK) );

	// �}�Ӄ\�[�g�f�[�^(�S���}�ӂƁA�V���I�E�}�ӂ̕��򂪕K�v���Ƃ������j
	wk->dw->nameSortTable = WorldTrade_ZukanSortDataGet( HEAPID_WORLDTRADE, 0, &wk->dw->nameSortNum );

	// �V���I�E�}�Ӄe�[�u��
	wk->dw->sinouTable    = WorldTrade_SinouZukanDataGet( HEAPID_WORLDTRADE );

	// �J�[�\���ʒu������
	WorldTrade_SelectListPosInit( &wk->selectListPos );
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

	GFL_HEAP_FreeMemory( wk->dw->sinouTable );
	GFL_HEAP_FreeMemory(wk->dw->nameSortTable);
	GFL_HEAP_FreeMemory(wk->dw);

	GFL_STR_DeleteBuffer( wk->TalkString ); 
///	GFL_STR_DeleteBuffer( wk->TitleString ); ����ŕs�v

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
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_Start( WORLDTRADE_WORK *wk)
{
	// 
	if(WIPE_SYS_EndCheck()){
		SubSeq_MessagePrint( wk, msg_gtc_01_010, 1, 0, 0x0f0f );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );

		// �ق����|�P�����E��������|�P�����`��
		
//		 wk->subprocess_seq = SUBSEQ_MAIN;
	}

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �T�u�v���Z�X�V�[�P���X���C��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_Main( WORLDTRADE_WORK *wk)
{
	if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
		 WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, MODE_DEPOSIT_SELECT );
		wk->subprocess_seq = SUBSEQ_END;
	}

	wk->subprocess_seq = SUBSEQ_HEADWORD_SELECT_LIST;

	return SEQ_MAIN;
}




//------------------------------------------------------------------
/**
 * @brief   num��move�𑫂�,0�ȉ��Ȃ�max-1�ɁAmax�Ȃ�0�ɂ��ĕԂ�
 *
 * @param   num		���̒l
 * @param   max		�ő�l
 * @param   move	�����l�i�{�|����j
 *
 * @retval  int		���ʂ̒l
 */
//------------------------------------------------------------------
static int RoundWork( int num, int max, int move )
{
	num += move;
	if(num < 0) {
		return max-1;
	}
	if(num==max){
		return 0;
	}
	return num;
}


//------------------------------------------------------------------
/**
 * @brief   �������I�����X�g�쐬
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_HeadWordSelectList( WORLDTRADE_WORK *wk )
{
	// �������I�����j���[�쐬
//	wk->BmpListWork = WorldTrade_WordheadBmpListMake( wk, &wk->BmpMenuList, &wk->MenuWin[0], wk->MsgManager );
//	wk->listpos = 0xffff;
//	wk->subprocess_seq = SUBSEQ_HEADWORD_SELECT_WAIT;

	// �������̓V�X�e���J�n
	WorldTrade_Input_Start( wk->WInputWork, MODE_POKEMON_NAME );
	
	wk->subprocess_seq = SUBSEQ_POKENAME_SELECT_WAIT;

	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   �������I��҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_HeadWordSelectWait( WORLDTRADE_WORK *wk )
{
	switch(WorldTrade_BmpMenuList_Main( wk->BmpListWork, &wk->listpos )){
	case 1: case 2: case 3: case 4: case 5:	case 6: case 7: case 8: case 9: case 10:
		BmpMenuList_Exit( wk->BmpListWork, &wk->dw->headwordListPos, &wk->dw->headwordPos );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		wk->subprocess_seq  = SUBSEQ_POKENAME_SELECT_LIST;
		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

		break;

	// �L�����Z��
	case BMPMENU_CANCEL:
		BmpMenuList_Exit( wk->BmpListWork, &wk->dw->headwordListPos, &wk->dw->headwordPos );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		BmpMenuWinClear( wk->MenuWin[0], WINDOW_TRANS_ON );
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
		GFL_BMPWIN_Delete( wk->MenuWin[0] );
		GFL_BMPWIN_Delete( wk->MenuWin[1] );

		WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, MODE_DEPOSIT_SELECT );
		wk->subprocess_seq = SUBSEQ_END;
		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
		break;
	}
	
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �|�P�������O�I�����X�g�쐬
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_PokeNameSelectList( WORLDTRADE_WORK *wk )
{
	// �|�P���������X�g�쐬
	wk->BmpListWork = WorldTrade_PokeNameListMake( wk,
						&wk->BmpMenuList, wk->MenuWin[1], 
						wk->MsgManager, wk->MonsNameManager,wk->dw,
						wk->param->zukanwork );
	wk->listpos = 0xffff;

	wk->subprocess_seq = SUBSEQ_POKENAME_SELECT_WAIT;

	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   �|�P�����̐��ʕ��z���琫�ʂ͌��肷�邩�H���͂��K�v���H
 *
 * @param   dtp				��������
 * @param   sex_selection	���ʎw��i�I�X�E���X�E���ʖ����j
 *
 * @retval  int		0:���ʂ̓��͂��K�v	1:���肵�Ă���̂ŕK�v�Ȃ�
 */
//------------------------------------------------------------------
int WorldTrade_SexSelectionCheck( Dpw_Tr_PokemonSearchData *dtsd, int sex_selection ) 
{
	switch(sex_selection){
	case POKEPER_SEX_MALE:
		dtsd->gender = PARA_MALE+1;
		return 1;
		break;
	case POKEPER_SEX_FEMALE:
		dtsd->gender = PARA_FEMALE+1;
		return 1;
		break;
	case POKEPER_SEX_UNKNOWN:
		dtsd->gender = PARA_UNK+1;
		return 1;
		break;
	}
	
	return 0;
}


//------------------------------------------------------------------
/**
 * @brief   �|�P�������O�I��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_PokeNameSelectWait( WORLDTRADE_WORK *wk )
{
	u32 result;
	int sex;
	
//	switch((result=WorldTrade_BmpMenuList_Main( wk->BmpListWork, &wk->listpos ))){
	switch((result=WorldTrade_Input_Main( wk->WInputWork ))){
	case BMPMENULIST_NULL:
		break;

	// �L�����Z��
	case BMPMENU_CANCEL:
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
		WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, MODE_DEPOSIT_SELECT );
		wk->subprocess_seq = SUBSEQ_END;
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
		
		WorldTrade_SelectNameListBackup( &wk->selectListPos, wk->dw->headwordListPos+wk->dw->headwordPos, wk->dw->nameListPos, wk->dw->namePos );

		break;
	// �I��
	default:
		wk->Want.characterNo = result;
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

		// ���ʕ��z���擾 
		wk->dw->sex_selection = PokePersonalParaGet(result,POKEPER_ID_sex);
		// ���ʂ͌Œ肩�H�i�Œ�̏ꍇ�͐��ʂ�Want�Ɋi�[�����j
		if(WorldTrade_SexSelectionCheck( &wk->Want, wk->dw->sex_selection )){
			// ���ʂ͌��肵�Ă���̂ŁA���͂���K�v������
			wk->subprocess_seq  = SUBSEQ_LEVEL_SELECT_MES;
			sex                 = wk->Want.gender;
		}else{
			// ���ʂ͌��肵�Ă��Ȃ��̂œ��͂��K�v
			wk->subprocess_seq  = SUBSEQ_SEX_SELECT_MES;
			sex = DPW_TR_GENDER_NONE;
		}

		// ���O����(���ʂ����܂邱�Ƃ�����j
		WodrldTrade_PokeWantPrint( wk->MsgManager, wk->MonsNameManager, wk->WordSet, 
							wk->InfoWin, wk->Want.characterNo,sex,-1, &wk->print);

		WorldTrade_SelectNameListBackup( &wk->selectListPos, wk->dw->headwordListPos+wk->dw->headwordPos, wk->dw->nameListPos, wk->dw->namePos );

		break;

	}
	
	return SEQ_MAIN;
}



//------------------------------------------------------------------
/**
 * @brief   ���ʂ�I��ł����������b�Z�[�W
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int 	SubSeq_SexSelctMessage( WORLDTRADE_WORK *wk )
{
	// �|�P�����̐��ʂ�I��ł�������
	SubSeq_MessagePrint( wk, msg_gtc_01_011, 1, 0, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_SEX_SELECT_LIST );

	// ���ʑI���E�C���h�E�m��
//	GF_BGL_BmpWinAdd(&wk->MenuWin[0], GFL_BG_FRAME0_M,
//		SELECT_MENU3_X,		SELECT_MENU3_Y,		SELECT_MENU3_SX, 		SELECT_MENU3_SY, 
//		WORLDTRADE_TALKFONT_PAL,  SELECT_MENU3_OFFSET );
//	GFL_BMP_Clear( &wk->MenuWin[0], 0x0000 );
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ���ʑI�����X�g�쐬�E�\��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int 	SubSeq_SexSelctList( WORLDTRADE_WORK *wk )
{
//	wk->BmpListWork = WorldTrade_SexSelectListMake( &wk->BmpMenuList, &wk->MenuWin[0], wk->MsgManager );
	wk->listpos = 0xffff;

	// �������̓V�X�e���J�n
	WorldTrade_Input_Start( wk->WInputWork, MODE_SEX );

	wk->subprocess_seq = SUBSEQ_SEX_SELECT_WAIT;

	
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   ���ʑI�����X�g�I��҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int 	SubSeq_SexSelctWait( WORLDTRADE_WORK *wk )
{
	u32 result;
	switch((result=WorldTrade_Input_Main( wk->WInputWork ))){
	// �L�����Z��
	case BMPMENU_CANCEL:
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

		wk->subprocess_seq = SUBSEQ_START;
		break;
	// �I��
	case PARA_MALE: case PARA_FEMALE: case PARA_UNK: 
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

		wk->Want.gender			= result+1;
		wk->subprocess_seq  = SUBSEQ_LEVEL_SELECT_MES;

		// ���ʌ���
		WodrldTrade_PokeWantPrint( wk->MsgManager, wk->MonsNameManager, wk->WordSet, 
							wk->InfoWin, wk->Want.characterNo,wk->Want.gender,-1, &wk->print);

		break;
	}

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   ���x����I��ł����������b�Z�[�W�\��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_LevelSelectMessage( WORLDTRADE_WORK *wk )
{
	// �|�P�����̃��x�������߂Ă�������
	SubSeq_MessagePrint( wk, msg_gtc_01_012, 1, 0, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_LEVEL_SELECT_LIST );

	// ���ʑI���E�C���h�E�m��
//	GF_BGL_BmpWinAdd(&wk->MenuWin[0], GFL_BG_FRAME0_M,
//		SELECT_MENU4_X,		SELECT_MENU4_Y,		SELECT_MENU4_SX, 		SELECT_MENU4_SY, 
//		WORLDTRADE_TALKFONT_PAL,  SELECT_MENU4_OFFSET );
//	GFL_BMP_Clear( &wk->MenuWin[0], 0x0000 );

	return SEQ_MAIN;
}



//------------------------------------------------------------------
/**
 * @brief   ���x���I�����X�g�쐬�E�\��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_LevelSelectList( WORLDTRADE_WORK *wk )
{
	// ���x���I�����X�g�쐬
//	wk->BmpListWork = WorldTrade_LevelListMake(&wk->BmpMenuList, &wk->MenuWin[0], wk->MsgManager,
//		LEVEL_PRINT_TBL_DEPOSIT );

	// �������̓V�X�e���J�n
	WorldTrade_Input_Start( wk->WInputWork, MODE_LEVEL );

	wk->listpos = 0xffff;

	wk->subprocess_seq = SUBSEQ_LEVEL_SELECT_WAIT;

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   ���x���I��҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_LevelSelectWait( WORLDTRADE_WORK *wk )
{
	u32 result;
//	switch((result=WorldTrade_BmpMenuList_Main( wk->BmpListWork, &wk->listpos ))){
	switch((result=WorldTrade_Input_Main( wk->WInputWork ))){
	case BMPMENULIST_NULL:
		break;
	// �L�����Z��
	case BMPMENU_CANCEL:
	case 12:
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
		
		// �|�P�����̐��ʂ��Œ肩�ǂ������`�F�b�N���Ė߂�
		if(WorldTrade_SexSelectionCheck( &wk->Want, wk->dw->sex_selection )){
			// ���ʂ͌Œ薳���̂ŁA�|�P�����I���܂Ŗ߂�
			wk->subprocess_seq = SUBSEQ_START;
		}else{
			// ���ʓ��͂��K�v
			wk->subprocess_seq = SUBSEQ_SEX_SELECT_MES;
		}
		break;
	// ���x���w�茈��
	default: 
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
		WorldTrade_LevelMinMaxSet(&wk->Want, result, LEVEL_PRINT_TBL_DEPOSIT);
		wk->subprocess_seq  = SUBSEQ_DEPOSITOK_MESSAGE;

		// ���x���w�茈��
		WodrldTrade_PokeWantPrint( wk->MsgManager, wk->MonsNameManager, wk->WordSet, 
						wk->InfoWin, wk->Want.characterNo,wk->Want.gender,
						WorldTrade_LevelTermGet(wk->Want.level_min,wk->Want.level_max,
							LEVEL_PRINT_TBL_DEPOSIT), &wk->print);

		break;
	}

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   ���̏����ł悢�H���b�Z�[�W�\��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_DepositOkMessage( WORLDTRADE_WORK *wk )
{
	// ���̂��傤����ł�낵���ł����H
	SubSeq_MessagePrint( wk, msg_gtc_01_024, 1, 0, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_DEPOSIT_YESNO );
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ����ŏ����ł������܂��H�͂��E������
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_DepositOkYesNo( WORLDTRADE_WORK *wk )
{

	WorldTrade_TouchWinYesNoMake(wk,WORLDTRADE_YESNO_PY1, YESNO_OFFSET, 3, 0 );
	wk->subprocess_seq = SUBSEQ_DEPOSIT_YESNO_WAIT;

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �͂��E�������҂�
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_DepositOkYesNoWait( WORLDTRADE_WORK *wk )
{

	u32 ret = WorldTrade_TouchSwMain(wk);

	if(ret==TOUCH_SW_RET_YES){
		// �͂����|�P��������������
    WorldTrade_TouchDelete( wk );
	 	WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_UPLOAD );
		wk->subprocess_seq  = SUBSEQ_END;
		wk->sub_out_flg = 1;

		// �T�[�o�[�A�b�v���[�h�p�̃f�[�^���쐬
		DepositPokemonDataMake( &wk->UploadPokemonData, wk );
	}else if(ret==TOUCH_SW_RET_NO){
		// ���������|�P�����I�������蒼��
    WorldTrade_TouchDelete( wk );
		WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, MODE_DEPOSIT_SELECT );
		wk->subprocess_seq  = SUBSEQ_END;
	}

/*
	int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_WORLDTRADE );

	if(ret!=BMPMENU_NULL){
		if(ret==BMPMENU_CANCEL){
			// ���������|�P�����I�������蒼��
			WorldTrade_SubProcessChange( wk, WORLDTRADE_MYBOX, MODE_DEPOSIT_SELECT );
			wk->subprocess_seq  = SUBSEQ_END;
		}else{
			// �͂����|�P��������������
		 	WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_UPLOAD );
			wk->subprocess_seq  = SUBSEQ_END;
			wk->sub_out_flg = 1;

			// �T�[�o�[�A�b�v���[�h�p�̃f�[�^���쐬
			DepositPokemonDataMake( &wk->UploadPokemonData, wk );
		}
	}
*/
	return SEQ_MAIN;
	
}














//------------------------------------------------------------------
/**
 * @brief   �T�u�v���Z�X�V�[�P���X�I������
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_End( WORLDTRADE_WORK *wk )
{
	// �ڑ���ʂ�������A�㉺��ʂŃt�F�[�h
	if(wk->sub_nextprocess==WORLDTRADE_ENTER ){
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
		OS_Printf( "******************** worldtrade_deposit.c [1165] MS ********************\n" );
#endif
		wk->sub_out_flg = 1;
	}else{
		// �����̉�ʂɂ����ꍇ�͏��ʂ����t�F�[�h
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
		OS_Printf( "******************** worldtrade_deposit.c [1171] M ********************\n" );
#endif
	}
	wk->subprocess_seq = 0;
	
	return SEQ_FADEOUT;
}





//------------------------------------------------------------------
/**
 * @brief   ��b�I����҂��Ď��̃V�[�P���X��
 *
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  int		���̃T�u�V�[�P���X
 */
//------------------------------------------------------------------
static int SubSeq_MessageWait( WORLDTRADE_WORK *wk )
{
	if( GF_MSG_PrintEndCheck( &wk->print )==0){
		wk->subprocess_seq = wk->subprocess_nextseq;
	}
	return SEQ_MAIN;

}













// ���ʃA�C�R����\�����邽�߂̃J���[�w��
static u16 sex_mark_col( int idx )
{
	switch( idx )
	{	
	case 0: return PRINTSYS_LSB_Make(5,6,0);
	case 1: return PRINTSYS_LSB_Make(3,4,0);
	}

	return 0;
}


//==============================================================================
/**
 * @brief   �|�P�������`��(MSG_ALLPUT)
 *
 * @param   win		��������BMPWIN
 * @param   nameman	�|�P���������b�Z�[�W�}�l�[�W���[
 * @param   monsno	�|�P����NO
 * @param   flag	�Z���^�����O
 * @param   color	�J���[�w��
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_PokeNamePrint( GFL_BMPWIN *win, GFL_MSGDATA *nameman, int monsno, int flag, int y, PRINTSYS_LSB color, WT_PRINT *print )
{
	STRBUF *namebuf;

	// ���O
	if(monsno!=0){
		namebuf = GFL_MSG_CreateString( nameman, monsno );
		WorldTrade_SysPrint( win, namebuf, 0, y, flag, color, print );
		GFL_STR_DeleteBuffer(namebuf);
	}

}


//==============================================================================
/**
 * @brief   �|�P�������`��iMSG_NO_PUT�Łj
 *
 * @param   win		��������BMPWIN
 * @param   nameman	�|�P���������b�Z�[�W�}�l�[�W���[
 * @param   monsno	�|�P����NO
 * @param   y		Y�ʒu���炷�p�̃I�t�Z�b�g
 * @param   color	�J���[�w��
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_PokeNamePrintNoPut( GFL_BMPWIN *win, GFL_MSGDATA *nameman, int monsno, int y, PRINTSYS_LSB color, WT_PRINT *print )
{
	STRBUF *namebuf;

	// ���O
	if(monsno!=0){
		namebuf = GFL_MSG_CreateString( nameman, monsno );
		GF_STR_PrintColor( win, FONT_SYSTEM, namebuf, 0, y, MSG_NO_PUT, color, print);
		GFL_STR_DeleteBuffer(namebuf);
	}

}


//--------------------------------------------------------------
/**
 * @brief   �����\��
 *
 * @param   win				��������BMPWIN
 * @param   nameman			�������b�Z�[�W�}�l�[�W���[
 * @param   country_code	���R�[�h
 * @param   flag			1���ƃZ���^�����O�A�Q���ƉE�悹
 * @param   y				Y���W���炷�I�t�Z�b�g
 * @param   color			�J���[�w��
 *
 * @retval  none
 *
 *
 */
//--------------------------------------------------------------
void WorldTrade_CountryPrint( GFL_BMPWIN *win, GFL_MSGDATA *nameman, GFL_MSGDATA *msgman, int country_code, int flag, int y, PRINTSYS_LSB color, WT_PRINT *print )
{
	STRBUF *namebuf;

	// ����
	if(country_code!=0){
		namebuf = GFL_MSG_CreateString( nameman, country_code );
		WorldTrade_SysPrint( win, namebuf, 0, y, flag, color, print );
		GFL_STR_DeleteBuffer(namebuf);
	}
	else{	//�u���ɂ��Ȃ��v��\��
		namebuf = GFL_MSG_CreateString( msgman, msg_gtc_search_015 );
		WorldTrade_SysPrint( win, namebuf, 0, y, flag, color, print );
		GFL_STR_DeleteBuffer(namebuf);
	}

}




//------------------------------------------------------------------
/**
 * @brief   ���ʃJ���[�擾
 *
 * @param   sex		����
 * @param   color	�J���[
 *
 * @retval  PRINTSYS_LSB	�J���[�w��
 */
//------------------------------------------------------------------
static PRINTSYS_LSB GetSexColor( int sex, PRINTSYS_LSB color )
{

	if(sex==DPW_TR_GENDER_MALE){
		return sex_mark_col(0);
	}else if(sex==DPW_TR_GENDER_FEMALE){
		return sex_mark_col(1);
	}
	return color;
}

// ���ʕ�����̃e�[�u���u���ɂ��Ȃ��E���E���E���ɂ��Ȃ��v
const int WorldTrade_SexStringTable[]={
	msg_gtc_11_001,
	msg_gtc_11_002,
	msg_gtc_11_003,
	msg_gtc_11_001,
	
};

//==============================================================================
/**
 * @brief   ���ʂ�\������
 *
 * @param   win		��������BMPWIN
 * @param   msgman	���b�Z�[�W�}�l�[�W���[
 * @param   sex		����
 * @param   flag	�u���ɂ��Ȃ��v��\�����邩�ǂ���
 * @param   color	�J���[�w��
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_SexPrint( GFL_BMPWIN *win, GFL_MSGDATA *msgman, int sex, int flag, int y, int printflag, PRINTSYS_LSB color, WT_PRINT *print )
{
	STRBUF *sexbuf;

	OS_Printf("SEX Print flag = %d, sex = %d\n", flag, sex);

	if(flag==0 && sex==DPW_TR_GENDER_NONE){
		// ���ʖ�����\�����Ȃ��Ă��������͕\�����Ȃ�
		return;
	}
	
	sexbuf = GFL_MSG_CreateString( msgman, WorldTrade_SexStringTable[sex]  );
	// flag��3�ȓ��̏ꍇ�̓Z���^�����O�w��E�ȏ�̏ꍇ�͂w���W�w��
	if(printflag > 3){
		WorldTrade_SysPrint( win, sexbuf,   printflag, y, 0, GetSexColor( sex, color ), print );
	}else{
		WorldTrade_SysPrint( win, sexbuf,   0, y, printflag, GetSexColor( sex, color ), print );
	}
	GFL_STR_DeleteBuffer(sexbuf);
}


//==============================================================================
/**
 * @brief   ���ʂ�\������(MSG_NO_PUT�Łj
 *
 * @param   win		��������BMPWIN
 * @param   msgman	���b�Z�[�W�}�l�W���[
 * @param   sex		����
 * @param   flag	���g�p
 * @param   x		X�ʒu
 * @param   y		Y�ʒu
 * @param   color	�J���[�w��
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_SexPrintNoPut( GFL_BMPWIN *win, GFL_MSGDATA *msgman, int sex, int flag, int x, int y, PRINTSYS_LSB color, WT_PRINT *print )
{
	STRBUF *sexbuf;

	OS_Printf("SEX Print flag = %d, sex = %d\n", flag, sex);

	if(flag==0 && sex==DPW_TR_GENDER_NONE){
		// ���ʖ�����\�����Ȃ��Ă��������͕\�����Ȃ�
		return;
	}
	
	sexbuf = GFL_MSG_CreateString( msgman, WorldTrade_SexStringTable[sex]  );
	// flag��3�ȓ��̏ꍇ�̓Z���^�����O�w��E�ȏ�̏ꍇ�͂w���W�w��
	GF_STR_PrintColor( win, FONT_SYSTEM, sexbuf, x, y, MSG_NO_PUT, GetSexColor( sex, color ),print);
	GFL_STR_DeleteBuffer(sexbuf);
}

//==============================================================================
/**
 * @brief   ���x�������̕�����`����s��(�݊�����ۂ��߃��b�v�Łj
 *
 * @param   win		BMPWIN
 * @param   msgman	���b�Z�[�W�}�l�[�W���[	
 * @param   sex		���x������
 * @param   flag	�]���t���O	
 * @param   y       Y���W
 * @param   color	�F�w��	
 * @param   tbl_select	LEVEL_PRINT_TBL_DEPOSIT or LEVEL_PRINT_TBL_SEARCH
 *
 * @retval  none		
 */
//==============================================================================
void WorldTrade_WantLevelPrint( GFL_BMPWIN *win, GFL_MSGDATA *msgman, int level, int flag, int y, PRINTSYS_LSB color, int tbl_select, WT_PRINT *print )
{
	WorldTrade_WantLevelPrint_XY( win, msgman, level, flag, 0, y, color, tbl_select, print );

}


//==============================================================================
/**
 * @brief   XY�w�肪�ł���悤�ɂȂ������x������������`��
 *
 * @param   win			BMPWIN
 * @param   msgman		���b�Z�[�W�}�l�[�W���[
 * @param   level		���x������
 * @param   flag		�]���t���O
 * @param   x			X���W
 * @param   y			Y���W
 * @param   color		�F�w��
 * @param   tbl_select	LEVEL_PRINT_TBL_DEPOSIT or LEVEL_PRINT_TBL_SEARCH
 *
 * @retval  none		
 */
//==============================================================================
void WorldTrade_WantLevelPrint_XY(GFL_BMPWIN *win, GFL_MSGDATA *msgman, int level, int flag, int x, int y, PRINTSYS_LSB color, int tbl_select, WT_PRINT *print )
{
	STRBUF *levelbuf;
	const WT_LEVEL_TERM * level_tbl;
	
	if(tbl_select == LEVEL_PRINT_TBL_DEPOSIT){
		level_tbl = level_minmax_table;
	}
	else{	//LEVEL_PRINT_TBL_SEARCH
		level_tbl = search_level_minmax_table;
	}
	
	if(level!=-1){
		levelbuf = GFL_MSG_CreateString( msgman, level_tbl[level].msg );
		WorldTrade_SysPrint( win, levelbuf,  x, y, flag, color, print );
		GFL_STR_DeleteBuffer(levelbuf);
	}

}

//------------------------------------------------------------------
/**
 * @brief   �������ė~�����|�P�����̏�����`�悷��
 *
 * @param   MsgManager		GTS���b�Z�[�W�}�l�[�W���[
 * @param   MonsNameManager	�|�P�������b�Z�[�W�}�l�[�W���[
 * @param   WordSet			�o�^�pWORDSET
 * @param   win[]			BMPWIN
 * @param   monsno			�|�P����NO
 * @param   sex				����
 * @param   level			���x��
 *
 * @retval  none		
 */
//------------------------------------------------------------------
void WodrldTrade_PokeWantPrint( GFL_MSGDATA *MsgManager, GFL_MSGDATA *MonsNameManager,
	WORDSET *WordSet, GFL_BMPWIN *win[], int monsno, int sex, int level, WT_PRINT *print )
{
	STRBUF *strbuf;
	int i;

	//�u�ق����|�P�����v�`��
	strbuf = GFL_MSG_CreateString( MsgManager, msg_gtc_05_008 );
	WorldTrade_SysPrint( win[0], strbuf,    0, 0, 0, PRINTSYS_LSB_Make(1,2,0), print );

	// ���O�E���ʁE���x���̗��̓N���A����
	for(i=1;i<3;i++){
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[i]), 0x0000 );
	}

	// ���O
	WorldTrade_PokeNamePrint(win[1], MonsNameManager, monsno, 0, 0,PRINTSYS_LSB_Make(1,2,0), print );

	// ����
	if( sex==DPW_TR_GENDER_MALE || sex==DPW_TR_GENDER_FEMALE ){
		OS_Printf("���ʕ`�悵�� %d\n", sex);
		WorldTrade_SexPrint( win[1], MsgManager, sex, 0, 0, 70, PRINTSYS_LSB_Make(1,2,0), print );
	}else{
		OS_Printf("���ʕ`�悵�ĂȂ� %d\n");
	
	}

	// ���x���w��
	WorldTrade_WantLevelPrint( win[2], MsgManager, level, 2, 0, PRINTSYS_LSB_Make(1,2,0),
		LEVEL_PRINT_TBL_DEPOSIT, print );
	
	

	GFL_STR_DeleteBuffer(strbuf);
}

//------------------------------------------------------------------
/**
 * @brief   �������ė~�����|�P�����̏�����`�悷��
 *
 * @param   MsgManager		GTS���b�Z�[�W�}�l�[�W���[
 * @param   MonsNameManager	�|�P���������b�Z�[�W�}�l�[�W���[
 * @param   WordSet			�o�^WORDSET
 * @param   win[]			BMPWIN
 * @param   monsno			�|�P����NO
 * @param   sex				����
 * @param   level			���x��
 *
 * @retval  none
 */
//------------------------------------------------------------------
void WodrldTrade_MyPokeWantPrint( GFL_MSGDATA *MsgManager, GFL_MSGDATA *MonsNameManager,
	WORDSET *WordSet, GFL_BMPWIN *win[], int monsno, int sex, int level, WT_PRINT *print )
{
	STRBUF *strbuf;
	int i;

	//�u�ق����|�P�����v�`��
	strbuf = GFL_MSG_CreateString( MsgManager, msg_gtc_05_008 );
	WorldTrade_SysPrint( win[0], strbuf,    0, 0, 0, PRINTSYS_LSB_Make(1,2,0), print );

	// ���O�E���ʁE���x���̗��̓N���A����
	for(i=1;i<3;i++){
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[i]), 0x0000 );
	}

	// ���O
	WorldTrade_PokeNamePrint(win[1], MonsNameManager, monsno, 0, 0,PRINTSYS_LSB_Make(1,2,0), print );

	// ����
	if( sex==DPW_TR_GENDER_MALE || sex==DPW_TR_GENDER_FEMALE ){
		OS_Printf("���ʕ`�悵�� %d\n", sex);
		WorldTrade_SexPrint( win[1], MsgManager, sex, 0, 0, 70, PRINTSYS_LSB_Make(1,2,0),print );
	}else{
		OS_Printf("���ʕ`�悵�ĂȂ�\n");
	}

	// ���x���w��
	WorldTrade_WantLevelPrint( win[2], MsgManager, level, 0, 0, PRINTSYS_LSB_Make(1,2,0),
		LEVEL_PRINT_TBL_DEPOSIT,print );
	
	

	GFL_STR_DeleteBuffer(strbuf);
}

//------------------------------------------------------------------
/**
 * @brief   ������a���悤�Ƃ���|�P�����̏���`�悷��
 *
 * @param   MsgManager	GTS���b�Z�[�W�}�l�[�W���[
 * @param   WordSet		WORDSET
 * @param   win[]		BMWPIN
 * @param   ppp			POKEMON_PASO_PARAM
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void PokeDepositPrint( 
	GFL_MSGDATA *MsgManager, 
	WORDSET *WordSet, 
	GFL_BMPWIN *win[], 
	POKEMON_PASO_PARAM *ppp,
	Dpw_Tr_PokemonDataSimple *post,
	WT_PRINT *print )
{
	STRBUF *strbuf,	*levelbuf;
	STRBUF *namebuf = GFL_STR_CreateBuffer( MONS_NAME_SIZE+EOM_SIZE, HEAPID_WORLDTRADE );
	STRBUF *sexbuf = GFL_STR_CreateBuffer( MONS_NAME_SIZE+EOM_SIZE, HEAPID_WORLDTRADE );
	int sex, level,i;
	
	OS_TPrintf("deposit_ppp3 = %08x", ppp);
	
	// ��������|�P����

	// ���O�E���ʁE���x���擾
	PPP_Get(ppp, ID_PARA_nickname, namebuf );
	sex   = PPP_Get( ppp, ID_PARA_sex,   NULL )+1;
	level = PokePasoLevelCalc(ppp);
	
	OS_Printf("sex=%d, level=%d\n",sex, level);
	
	// �u��������|�P�����v�E���ʃA�C�R��������擾
	strbuf     = GFL_MSG_CreateString( MsgManager, msg_gtc_05_011  );
	WORDSET_RegisterNumber( WordSet, 3, level, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	levelbuf = MSGDAT_UTIL_AllocExpandString( WordSet, MsgManager, msg_gtc_05_013_02, HEAPID_WORLDTRADE );
	if(sex!=DPW_TR_GENDER_NONE){
		GFL_MSG_GetString( MsgManager, WorldTrade_SexStringTable[sex], sexbuf );
	}

	// �`��̑O�ɃN���A
	for(i=0;i<3;i++){
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[i]), 0x0000 );
	}

	// �`��
	WorldTrade_SysPrint( win[0], strbuf,    0, 0, 0, PRINTSYS_LSB_Make(15,2,0), print );
	WorldTrade_SysPrint( win[1], namebuf,   0, 0, 0, PRINTSYS_LSB_Make(15,2,0), print );
	//WorldTrade_SysPrint( win[2], levelbuf,  0, 0, 0, PRINTSYS_LSB_Make(15,2,0) );
	WorldTrade_SysPrint( win[2], levelbuf,  0, 0, 2, PRINTSYS_LSB_Make(15,2,0), print );
	if(sex!=DPW_TR_GENDER_NONE){
		WorldTrade_SysPrint( win[1], sexbuf,   70, 0, 0, sex_mark_col(sex-1), print );
	}

	post->characterNo = PPP_Get(ppp, ID_PARA_monsno, NULL);
	post->gender      = sex;
	post->level       = level;

	GFL_STR_DeleteBuffer( levelbuf );
	GFL_STR_DeleteBuffer( sexbuf   );
	GFL_STR_DeleteBuffer( namebuf  );
	GFL_STR_DeleteBuffer( strbuf   );
}

#define ZKN_SORTDATA_ONESIZE	( sizeof(u16) )

//------------------------------------------------------------------
/**
 * @brief   ������\�[�g�f�[�^�擾�i�������������j�B����͂��Ԃ�ŁB
 *			
 * @param   heap		�q�[�vID
 * @param   idx			�C���f�b�N�X
 * @param   p_arry_num	�ʏ�}�ӂƑS���}�ӂ̑���
 *
 * @retval  u16*		�ǂݍ��񂾃\�[�g�f�[�^
 */
//------------------------------------------------------------------
u16* WorldTrade_ZukanSortDataGet( int heap, int idx, int* p_arry_num )
{
	u32 size;
	u16* p_buf;
	// �ǂݍ���
	p_buf = GFL_ARC_UTIL_LoadEx( ARCID_ZUKAN_DATA, NARC_zukan_data_zkn_sort_aiueo_dat, FALSE, heap, &size );

	*p_arry_num = size / ZKN_SORTDATA_ONESIZE;

	return p_buf;
	// �|�P�����̖��O���擾����
	//STRBUF* MSGDAT_UTIL_GetMonsName( u32 monsno, u32 heapID )
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��L�}�Ӄf�[�^�ŁA���������w�肵���Ƃ��̊J�n����I���C���f�b�N�X���擾����
 *
 *	@param	int select  �������C���f�b�N�X
 *	@param	*start      �J�n�󂯎��
 *	@param	*end        �I���󂯎��
 */
//-----------------------------------------------------------------------------
void WorldTrade_ZukanSortDataGetOnlyIndex( int select, int *start, int *end )
{ 
	static const u32 ZukanSortHiraTable[]={
		ZKN_SORT_AIUEO_IDX_START_A,  	// ��
		ZKN_SORT_AIUEO_IDX_START_I,
    ZKN_SORT_AIUEO_IDX_START_U,
		ZKN_SORT_AIUEO_IDX_START_E,
		ZKN_SORT_AIUEO_IDX_START_O,
		ZKN_SORT_AIUEO_IDX_START_KA ,	// ��
		ZKN_SORT_AIUEO_IDX_START_KI ,
		ZKN_SORT_AIUEO_IDX_START_KU ,
		ZKN_SORT_AIUEO_IDX_START_KE ,
		ZKN_SORT_AIUEO_IDX_START_KO ,
		ZKN_SORT_AIUEO_IDX_START_SA ,	// ��
		ZKN_SORT_AIUEO_IDX_START_SI ,
		ZKN_SORT_AIUEO_IDX_START_SU ,
		ZKN_SORT_AIUEO_IDX_START_SE ,
		ZKN_SORT_AIUEO_IDX_START_SO ,
		ZKN_SORT_AIUEO_IDX_START_TA ,	// ��
		ZKN_SORT_AIUEO_IDX_START_TI ,
		ZKN_SORT_AIUEO_IDX_START_TU ,
		ZKN_SORT_AIUEO_IDX_START_TE ,
		ZKN_SORT_AIUEO_IDX_START_TO ,
		ZKN_SORT_AIUEO_IDX_START_NA ,	// ��
		ZKN_SORT_AIUEO_IDX_START_NI ,
		ZKN_SORT_AIUEO_IDX_START_NU ,
		ZKN_SORT_AIUEO_IDX_START_NE ,
		ZKN_SORT_AIUEO_IDX_START_NO ,
		ZKN_SORT_AIUEO_IDX_START_HA ,	// ��
		ZKN_SORT_AIUEO_IDX_START_HI ,
		ZKN_SORT_AIUEO_IDX_START_HU ,
		ZKN_SORT_AIUEO_IDX_START_HE ,
		ZKN_SORT_AIUEO_IDX_START_HO ,
		ZKN_SORT_AIUEO_IDX_START_MA ,	// ��
		ZKN_SORT_AIUEO_IDX_START_MI ,
		ZKN_SORT_AIUEO_IDX_START_MU ,
		ZKN_SORT_AIUEO_IDX_START_ME ,
		ZKN_SORT_AIUEO_IDX_START_MO ,
		ZKN_SORT_AIUEO_IDX_START_YA ,	// ��
		ZKN_SORT_AIUEO_IDX_START_YU ,
		ZKN_SORT_AIUEO_IDX_START_YO ,
		ZKN_SORT_AIUEO_IDX_START_RA ,	// ��
		ZKN_SORT_AIUEO_IDX_START_RI ,
		ZKN_SORT_AIUEO_IDX_START_RU ,
		ZKN_SORT_AIUEO_IDX_START_RE ,
		ZKN_SORT_AIUEO_IDX_START_RO ,
		ZKN_SORT_AIUEO_IDX_START_WA ,	// ��
    ZKN_SORT_AIUEO_IDX_ALL_END,   //�I�[
  };
  GF_ASSERT( select < NELEMS(ZukanSortHiraTable) );

  *start  = ZukanSortHiraTable[ select ];
  *end    = ZukanSortHiraTable[ select + 1 ];
}

//==============================================================================
/**
 * @brief   �V���I�E�}�Ӄf�[�^��ǂݍ���			����͂��Ԃ��
 *			�V���I�E�}�ӂ̏��ɊJ��NO�������Ă���̂ŁA
 *			�V���Ƀe�[�u�����m�ۂ��āu���̊J��NO�̓V���I�E���H�v�̍쐬������
 *
 * @param   heap		�q�[�vID
 * @param   p_arry_num	MONS_END+1
 *
 * @retval  u16 *		494���̃V���I�E�}�ӂ��ǂ����̃t���O���������e�[�u��
 */
//==============================================================================
u8 *WorldTrade_SinouZukanDataGet( int heap  )
{
	u32 size,num,i;
	u16* p_buf;

	// �e�[�u���m�ہE������
	u8 *sinouData = GFL_HEAP_AllocMemory( HEAPID_WORLDTRADE, MONSNO_END+1 );
	MI_CpuClearFast( sinouData, MONSNO_END+1 );

	// �V���I�E�}�Ӄe�[�u��
	p_buf = GFL_ARC_UTIL_LoadEx( ARCID_ZUKAN_DATA, NARC_zukan_data_zkn_sort_chihou_dat, FALSE, heap, &size );
	
	num = size / ZKN_SORTDATA_ONESIZE;

	// �t���O�e�[�u���ɍ�蒼��
	for(i=0;i<num;i++){
		if(p_buf[i]<MONSNO_END+1){
			sinouData[p_buf[i]] = 1;
		}
	}

	GFL_HEAP_FreeMemory(p_buf);

	return sinouData;
}


//==============================================================================
/**
 * @brief   �T�[�o�[�ɑ��M�����{�����i�[����
 *
 * @param   dtd		GTS�p�|�P������{���
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none
 */
//==============================================================================
void WorldTrade_PostPokemonBaseDataMake( Dpw_Tr_Data *dtd, WORLDTRADE_WORK *wk )
{
#ifndef RESERVE_POKE_GS_BINARY
	// �|�P�����\���̂���
	
	if(WorldTrade_GetPPorPPP( wk->BoxTrayNo )){
		//�V�F�C�~�̏ꍇ�̓t�H����No���m�[�}���ɂ���
		PP_Put((POKEMON_PARAM*)(wk->deposit_ppp), ID_PARA_form_no, FORMNO_SHEIMI_LAND);
		// POKEMON_PARAM�Ƃ��ăR�s�[
		MI_CpuCopyFast( wk->deposit_ppp, dtd->postData.data,  POKETOOL_GetWorkSize() );
	}else{
		//�V�F�C�~�̏ꍇ�̓t�H����No���m�[�}���ɂ���
		PPP_Put(wk->deposit_ppp, ID_PARA_form_no, FORMNO_SHEIMI_LAND);
		// POKEMON_PASO_PARAM�Ȃ̂�Replace�֐���POKEMON_PARAM�ɓ��t������
		PokeReplace(wk->deposit_ppp,(POKEMON_PARAM *)dtd->postData.data);
	}

	// �g���[�i�[���R�s�[
	STRTOOL_Copy( MyStatus_GetMyName(wk->param->mystatus), dtd->name, DPW_TR_NAME_SIZE );

	
	// ID
	dtd->trainerID   = MyStatus_GetID_Low( wk->param->mystatus );

	// ���E�n��
	dtd->countryCode = MyStatus_GetMyNation( wk->param->mystatus );
	dtd->localCode   = MyStatus_GetMyArea( wk->param->mystatus );

	// ������
	dtd->trainerType = MyStatus_GetTrainerView( wk->param->mystatus );

	// ����
	dtd->gender      = MyStatus_GetMySex( wk->param->mystatus );

	// �o�[�W�����E���R�[�h
	dtd->versionCode = PM_VERSION;
	dtd->langCode    = PM_LANG;
#else

	//����̖쐶�|�P��߂܂��ăo�C�i��������POKEMON_PARAM
	//Lv11���R���b�^
	static const u8	sc_debug_gs_pokepara[]	=
	{	
		38,0x85,0x32,0x31,0x0,0x0,0xe8,0x3c,0x7d,0x16,0xfa,0x69,0x2,0x3c,0x26,0x6d,0xa5,0x1a,0x1a,0x87,0x8d,0xd7,0x39,0xc9,0x58,0x12,0xa,0x4e,0xb1,0xf4,0x75,0x38,0x90,0xad,0x33,0x57,0x3,0x66,0xc5,0x9b,0x70,0xa5,0x93,0x38,0xc7,0x6b,0x30,0x13,0x7b,0x67,0xb4,0x4f,0xf5,0xeb,0x52,0x53,0x4f,0x12,0x5f,0xda,0xfe,0x74,0x5,0xb2,0x7e,0xf1,0xa2,0x41,0x2d,0x1a,0xc,0xb,0x5f,0x19,0x77,0x5c,0x65,0x31,0xf3,0x96,0x47,0xc,0x3d,0x6f,0x7,0xb4,0x76,0x80,0xed,0x7,0x7f,0xd,0xc,0x6b,0xe1,0x8f,0x68,0x23,0x27,0x33,0xf3,0x16,0x23,0x2,0xc0,0x25,0xde,0xa,0x81,0x89,0x41,0x2,0xfb,0xa8,0xc,0x49,0xb8,0x34,0x4,0xed,0x5b,0xe7,0x23,0x6c,0x78,0x7f,0x7c,0x27,0xa,0x10,0x1d,0x51,0x3d,0xcc,0x80,0xc3,0x6a,0x98,0xca,0x38,0x5d,0x41,0x6d,0xb5,0xe4,0xac,0x44,0x8c,0x85,0x83,0xd4,0xa,0x4b,0xd2,0x4e,0xaa,0x5a,0x4,0x93,0xf8,0xc6,0xb1,0xf8,0x6f,0xfa,0xfd,0x7e,0xe9,0x70,0xd,0x22,0x75,0x53,0xd0,0x4b,0xbb,0x4b,0x2,0x4,0x49,0xfc,0xbb,0x77,0xb9,0xce,0x3b,0x98,0x1e,0xba,0x40,0x11,0x4d,0x1,0x43,0x6d,0x25,0x97,0xe3,0x15,0x32,0xa3,0xa5,0xd4,0xf2,0x6d,0x1e,0x2f,0xf1,0x8e,0x6e,0xef,0x4b,0xca,0x43,0xe,0x16,0x16,0x47,0x97,0x96,0xf2,0x51,0x9e,0xe8,0xf0,0xb5,0x48,0xd4,0xf5,0xda,0x74,0x65,0xbd,0x21,0x9e,0xd7

	};

	//����p�ʏ�|�P�ƋU������

	MI_CpuCopyFast( sc_debug_gs_pokepara, dtd->postData.data,  POKETOOL_GetWorkSize() );
	PP_Put( (POKEMON_PARAM *)dtd->postData.data, ID_PARA_level, 11 );
	PP_Put( (POKEMON_PARAM *)dtd->postData.data, ID_PARA_tamago_flag,FALSE);
	PP_Put( (POKEMON_PARAM *)dtd->postData.data, ID_PARA_tamago_exist,FALSE);


	// �g���[�i�[���R�s�[
	//STRTOOL_Copy( MyStatus_GetMyName(wk->param->mystatus), dtd->name, DPW_TR_NAME_SIZE );
	dtd->name[0]	= 0x2b;
	dtd->name[1]	= 0x0f;
	dtd->name[2]	= 0xFFFF;
	
	// ID
	dtd->trainerID   = MyStatus_GetID_Low( wk->param->mystatus );

	// ���E�n��
	dtd->countryCode = MyStatus_GetMyNation( wk->param->mystatus );
	dtd->localCode   = MyStatus_GetMyArea( wk->param->mystatus );

	// ������
	dtd->trainerType = MyStatus_GetTrainerView( wk->param->mystatus );

	// ����
	dtd->gender      = MyStatus_GetMySex( wk->param->mystatus );

	// �o�[�W�����E���R�[�h
	dtd->versionCode = VERSION_GOLD;//PM_VERSION;
	dtd->langCode    = PM_LANG;

#endif 
}


//------------------------------------------------------------------
/**
 * @brief   �|�P������a����̂ɕK�v�ȃf�[�^���쐬����
 *
 * @param   dtd		GTS�p�|�P������{���
 * @param   wk		GTS��ʃ��[�N
 *
 * @retval  none	
 */
//------------------------------------------------------------------
static void DepositPokemonDataMake( Dpw_Tr_Data *dtd, WORLDTRADE_WORK *wk )
{

	// ��{�����i�[�i�|�P�����E�g���[�i�[���E���E�n��E�����ڂȂǁj
	WorldTrade_PostPokemonBaseDataMake( dtd, wk );
	
	// �ق����|�P���������i�[
	dtd->postSimple = wk->Post;
	dtd->wantSimple = wk->Want;

	OS_Printf( "postData  No = %d,  gender = %d, level min= %d\n", wk->Post.characterNo, wk->Post.gender, wk->Post.level);
	OS_Printf( "WantData  No = %d,  gender = %d, level min= %d\n", wk->Want.characterNo, wk->Want.gender, wk->Want.level_min);


	OS_Printf( "TrainerId = %05d,  nation = %d, area = %d\n", dtd->trainerID, dtd->countryCode, dtd->localCode);
	OS_Printf( "Version   = %d,  language = %d, \n", dtd->versionCode, dtd->langCode);


}

// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------
// BMPLIST�֌W
// -----------------------------------------------------------------------------
// -----------------------------------------------------------------------------

#define DEPOSIT_HEADWORD_NUM	( 10 )
#define DEPOSIT_Y_NUM			(  6 )

///BMPLIST�p��`
static const BMPMENULIST_HEADER MenuListHeader = {
    NULL,			// �\�������f�[�^�|�C���^
    NULL,					// �J�[�\���ړ����Ƃ̃R�[���o�b�N�֐�
    NULL,					// ���\�����Ƃ̃R�[���o�b�N�֐�
    NULL,					// 
    DEPOSIT_HEADWORD_NUM,	// ���X�g���ڐ�
    DEPOSIT_Y_NUM,			// �\���ő區�ڐ�
    4,						// ���x���\���w���W
    8,						// ���ڕ\���w���W
    0,						// �J�[�\���\���w���W
    2,						// �\���x���W
    FBMP_COL_BLACK,			// �����F
    FBMP_COL_WHITE,			// �w�i�F
    FBMP_COL_BLK_SDW,		// �����e�F
    0,						// �����Ԋu�w
    16,						// �����Ԋu�x
    BMPMENULIST_LRKEY_SKIP,		// �y�[�W�X�L�b�v�^�C�v
    FONT_SYSTEM,			// �����w��
    0,						// �a�f�J�[�\��(allow)�\���t���O(0:ON,1:OFF)
    NULL,                   // ���[�N
};



//==============================================================================
/**
 * @brief   �������I�����j���[�쐬
 *
 * @param  none
 *
 * @retval  none		
 */
//==============================================================================
BMPMENULIST_WORK *WorldTrade_WordheadBmpListMake( WORLDTRADE_WORK *wk, BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, GFL_MSGDATA *MsgManager )
{
	BMPMENULIST_HEADER list_h;
	int i;

	*menulist = BmpMenuWork_ListCreate( 10, HEAPID_WORLDTRADE );
	for(i=0;i<10;i++){
		BmpMenuWork_ListAddArchiveString( *menulist, MsgManager, msg_gtc_10_001+i, i+1 , HEAPID_WORLDTRADE );
	}

    list_h       = MenuListHeader;
    list_h.list  = *menulist;
    list_h.win   = win;

	// �g�`��
	BmpWinFrame_Write( win, WINDOW_TRANS_ON, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

	// BMP���j���[�J�n
    return BmpMenuList_Set(&list_h, wk->dw->headwordListPos, wk->dw->headwordPos, HEAPID_WORLDTRADE);

}

// �������e�[�u���i�\�[�g���ꂽ�|�P�����̉��ԖڂɁu�A�E�J�E�T�c�v���o�ꂷ�邩�H
static u16 NameHeadTable[]={
  ZKN_SORT_AIUEO_IDX_START_A,
  ZKN_SORT_AIUEO_IDX_START_KA,
  ZKN_SORT_AIUEO_IDX_START_SA,
  ZKN_SORT_AIUEO_IDX_START_TA,
  ZKN_SORT_AIUEO_IDX_START_NA,
  ZKN_SORT_AIUEO_IDX_START_HA,
  ZKN_SORT_AIUEO_IDX_START_MA,
  ZKN_SORT_AIUEO_IDX_START_YA,
  ZKN_SORT_AIUEO_IDX_START_RA,
  ZKN_SORT_AIUEO_IDX_START_WA,
  ZKN_SORT_AIUEO_IDX_ALL_END,
	0,
};

//------------------------------------------------------------------
/**
 * @brief   �|�P�������A�T�O�����\�[�g���X�g�쐬
 *
 * @param   menulist	���X�g�f�[�^�i�����ɓo�^����j
 * @param   msgman		�|�P���������b�Z�[�W�f�[�^
 * @param   table		�T�O���������X�^�[�i���o�[�e�[�u��
 * @param   num			�|�P�����̑���
 * @param   select		�A�J�T�^�i�n�}�������̂ǂ��I���������H�i�A: 0�`9�܂Łj
 *
 * @retval  int	�}�ӑ���
 */
//------------------------------------------------------------------
static int PokeNameSortListMake( BMP_MENULIST_DATA **menulist, GFL_MSGDATA *monsnameman, 
									GFL_MSGDATA *msgman,u16 *table, u8 *sinou,
									int num, int select,ZUKAN_SAVEDATA *zukan )
{
	int i,index,see_count=0;
	int pokenum = NameHeadTable[select+1]-NameHeadTable[select];
	int flag    = TRUE;	//ZUKANSAVE_GetZenkokuZukanFlag(zukan); �S���}�ӂɂȂ��Ă��邩�͊֌W�Ȃ��Ȃ��� 2008.06.23(��) matsuda

	OS_TPrintf("select = %d, num = %d\n",select, pokenum);
	
	// �}�ӂƔ�ׂĉ��̌��Ă��邩���m�F
	index = NameHeadTable[select];
	for(i=0;i<pokenum;i++){
		// �S���}�ӂ��H
		if(flag){
			if(ZUKANSAVE_GetPokeSeeFlag( zukan, table[index+i] )){
				OS_Printf(" SeeCheck i = %d, table[index+i] = %d\n", i, table[index+i]);
				see_count++;
			}
		}else{
			// �V���I�E�}�ӂɂ��邱�Ƃ��m�F������Ő}�Ӄ`�F�b�N
			if(sinou[table[index+i]]){
				if(ZUKANSAVE_GetPokeSeeFlag( zukan, table[index+i] )){
					see_count++;
				}
			}
		
		}
	}

	// ���ڃ��X�g�̍쐬
	*menulist = BmpMenuWork_ListCreate( see_count+1, HEAPID_WORLDTRADE );


	// �|�P�������̓o�^
	for(i=0;i<pokenum;i++){
		// �S���}�ӂ��H
		if(flag){
			if(ZUKANSAVE_GetPokeSeeFlag( zukan, table[index+i] )){
				OS_Printf(" ListAdd i = %d, table[index+i] = %d\n", i, table[index+i]);
				BmpMenuWork_ListAddArchiveString( *menulist, monsnameman, table[index+i], table[index+i], HEAPID_WORLDTRADE  );
			}
		}else{
			// �V���I�E�}�ӂɂ��邱�Ƃ��m�F������Ő}�Ӄ`�F�b�N
			if(sinou[table[index+i]]){
				if(ZUKANSAVE_GetPokeSeeFlag( zukan, table[index+i] )){
					BmpMenuWork_ListAddArchiveString( *menulist, monsnameman, table[index+i], table[index+i], HEAPID_WORLDTRADE  );
				}
			}
		}
	}
	// �u���ǂ�v��o�^
	BmpMenuWork_ListAddArchiveString( *menulist, msgman, msg_gtc_11_004, BMPMENU_CANCEL, HEAPID_WORLDTRADE  );

	return see_count+1;
}



//==============================================================================
/**
 * @brief   �|�P�������I�����X�g�쐬
 *
 * @param   menulist		
 * @param   win				
 * @param   MsgManager		
 * @param   MonsNameManager	
 * @param   dw				
 *
 * @retval  BMPMENULIST_WORK *	
 */
//==============================================================================
//=============================================================================================
/**
 * @brief	�|�P�������I�����X�g�쐬
 *
 * @param   wk				GTS��ʃ��[�N
 * @param   menulist		���j���[���X�g�������݃o�b�t�@
 * @param   win				BMPWIN
 * @param   MsgManager		GTS���b�Z�[�W�}�l�[�W���[
 * @param   MonsNameManager	�|�P���������b�Z�[�W�}�l�[�W���[
 * @param   dw				�a����p�|�P�����f�[�^
 * @param   zukan			�}�Ӄf�[�^
 *
 * @retval  BMPMENULIST_WORK *	BMP���X�g���[�N
 */
//=============================================================================================
BMPMENULIST_WORK *WorldTrade_PokeNameListMake( WORLDTRADE_WORK *wk, BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, 
			GFL_MSGDATA *MsgManager, GFL_MSGDATA *MonsNameManager, DEPOSIT_WORK* dw, ZUKAN_SAVEDATA *zukan)
{
	BMPMENULIST_HEADER list_h;
	int i,listnum,head;

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win), 0x0f0f );

	head = dw->headwordListPos+dw->headwordPos;
	listnum = PokeNameSortListMake( menulist, MonsNameManager, MsgManager, 
									dw->nameSortTable, dw->sinouTable, 
									dw->nameSortNum, head, zukan );
	
	
    list_h       = MenuListHeader;
    list_h.count = listnum;
    list_h.list  = *menulist;
    list_h.win   = win;

	// �g�`��
	BmpWinFrame_Write( win, WINDOW_TRANS_ON, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

	// BMP���j���[�J�n
    return BmpMenuList_Set(&list_h, wk->selectListPos.name_list[head],
    						   wk->selectListPos.name_pos[head], HEAPID_WORLDTRADE);

}


// �I�����X�g�p���ʃA�C�R����
static sex_select_table[][2]={
	{msg_gtc_11_001, PARA_UNK,},
	{msg_gtc_11_002, PARA_MALE,},
	{msg_gtc_11_003, PARA_FEMALE,} ,
	{msg_gtc_11_004, BMPMENU_CANCEL},
};

//==============================================================================
/**
 * @brief   �|�P�������ʑI�����X�g�쐬
 *
 * @param   menulist	���j���[���X�g���������܂��
 * @param   win			BMPWIN
 * @param   MsgManager	GTS���b�Z�[�W�}�l�[�W���[
 *
 * @retval  BMPLIST_MENU *		BMP���X�g���[�N
 */
//==============================================================================
BMPMENULIST_WORK *WorldTrade_SexSelectListMake( BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, GFL_MSGDATA *MsgManager  )
{
	BMPMENULIST_HEADER list_h;
	int i;

	*menulist = BmpMenuWork_ListCreate( 4, HEAPID_WORLDTRADE );
	for(i=0;i<4;i++){
		BmpMenuWork_ListAddArchiveString( *menulist, MsgManager, sex_select_table[i][0], sex_select_table[i][1], HEAPID_WORLDTRADE  );
	}

    list_h       = MenuListHeader;
    list_h.count = 4;
    list_h.list  = *menulist;
    list_h.win   = win;

	// �g�`��
	BmpWinFrame_Write( win, WINDOW_TRANS_ON, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

	// BMP���j���[�J�n
    return BmpMenuList_Set(&list_h, 0, 0, HEAPID_WORLDTRADE);
	
}



//------------------------------------------------------------------
/**
 * @brief   ���x�������pBMP_MENULIST_DATA���쐬
 *
 * @param   menulist	BMPMENU���ڂ�ǉ�����|�C���^
 * @param   MsgManager	���b�Z�[�W�}�l�[�W���[
 * @param   tbl_select	�����̎��Ɨa���鎞�Ń��x���I���̕����񂪈Ⴄ(LEVEL_PRINT_TBL_DEPOSIT or LEVEL_PRINT_TBL_SEARCH)
 *
 * @retval  none		
 */
//------------------------------------------------------------------
int WorldTrade_LevelListAdd( BMP_MENULIST_DATA **menulist, GFL_MSGDATA *MsgManager, int tbl_select)
{
	int i;
	const WT_LEVEL_TERM * level_tbl;
	int select_num;
	
	if(tbl_select == LEVEL_PRINT_TBL_DEPOSIT){
		level_tbl = level_minmax_table;
		select_num = LEVEL_SELECT_NUM;
	}
	else{	//LEVEL_PRINT_TBL_SEARCH
		level_tbl = search_level_minmax_table;
		select_num = SEARCH_LEVEL_SELECT_NUM;
	}

	*menulist = BmpMenuWork_ListCreate( select_num, HEAPID_WORLDTRADE );
	for(i=0;i<select_num;i++){
		BmpMenuWork_ListAddArchiveString( *menulist, MsgManager, level_tbl[i].msg, i, HEAPID_WORLDTRADE  );
	}

	return select_num;
}

//==============================================================================
/**
 * @brief   ���x���I�����X�g�쐬
 *
 * @param   menulist	�������ރ��j���[�o�b�t�@
 * @param   win			BMPWIN
 * @param   MsgManager	GTS���b�Z�[�W�}�l�[�W���[
 * @param   tbl_select	LEVEL_PRINT_TBL_DEPOSIT or LEVEL_PRINT_TBL_SEARCH
 *
 * @retval  BMPMENULIST_WORK *		
 */
//==============================================================================
BMPMENULIST_WORK *WorldTrade_LevelListMake(BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, GFL_MSGDATA *MsgManager, int tbl_select  )
{
	BMPMENULIST_HEADER list_h;
	int select_num;

	select_num   = WorldTrade_LevelListAdd( menulist, MsgManager, tbl_select );
	
    list_h       = MenuListHeader;
    list_h.count = select_num;
    list_h.list  = *menulist;
    list_h.win   = win;

	// �g�`��
	BmpWinFrame_Write( win, WINDOW_TRANS_ON, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

	// BMP���j���[�J�n
    return BmpMenuList_Set(&list_h, 0, 0, HEAPID_WORLDTRADE);

}


//==============================================================================
/**
 * @brief   ���x���w�������������
 *
 * @param   dtps		���������\����
 * @param   index		���������̓Y����
 * @param   tbl_select	LEVEL_PRINT_TBL_DEPOSIT or LEVEL_PRINT_TBL_SEARCH
 *
 * @retval  none		
 */
//==============================================================================
void WorldTrade_LevelMinMaxSet( Dpw_Tr_PokemonSearchData *dtps, int index, int tbl_select )
{
	const WT_LEVEL_TERM * level_tbl;

	if(tbl_select == LEVEL_PRINT_TBL_DEPOSIT){
		level_tbl = level_minmax_table;
		GF_ASSERT(index<(LEVEL_SELECT_NUM));// ���x���w��̔z����ゾ����
	}
	else{	//LEVEL_PRINT_TBL_SEARCH
		level_tbl = search_level_minmax_table;
		GF_ASSERT(index<(SEARCH_LEVEL_SELECT_NUM));// ���x���w��̔z����ゾ����
	}
	
	dtps->level_min = level_tbl[index].min;
	dtps->level_max = level_tbl[index].max;
}

//==============================================================================
/**
 * @brief   �w��̃��x�������̃C���f�b�N�X���擾����
 *
 * @param   min		�Œ�
 * @param   max		�ő僌�x��
 * @param   tbl_select	LEVEL_PRINT_TBL_DEPOSIT or LEVEL_PRINT_TBL_SEARCH
 *
 * @retval  int		�擾�����t�@�C���C���f�b�N�X
 */
//==============================================================================
int WorldTrade_LevelTermGet( int min, int max, int tbl_select )
{
	int i;
	int select_num;
	const WT_LEVEL_TERM * level_tbl;
	
	if(tbl_select == LEVEL_PRINT_TBL_DEPOSIT){
		level_tbl = level_minmax_table;
		select_num = LEVEL_SELECT_NUM;
	}
	else{	//LEVEL_PRINT_TBL_SEARCH
		level_tbl = search_level_minmax_table;
		select_num = SEARCH_LEVEL_SELECT_NUM;
	}
	
	for(i=0;i<select_num;i++){
		if(level_tbl[i].min==min && level_tbl[i].max==max){
			return i;
		}
	}

	// ���x������������Ȃ���������Ƃ�����ASSERT�ɂ���ƁA��X���|���̂�
	// �u�Ȃ�ł�����v�ɂ��Ă���
	return 0;
}


//==============================================================================
/**
 * @brief   ���I�����X�g�쐬
 *
 * @param   menulist	�������ރ��j���[���[�N
 * @param   win			BMPWIN
 * @param   MsgManager	GTS���b�Z�[�W�}�l�[�W���[
 *
 * @retval  BMPMENULIST_WORK *	���������a�l�o���X�g���[�N
 */
//==============================================================================
BMPMENULIST_WORK *WorldTrade_CountryListMake(BMP_MENULIST_DATA **menulist, GFL_BMPWIN *win, GFL_MSGDATA *CountryMsgManager, GFL_MSGDATA *NormalMsgManager)
{
	BMPMENULIST_HEADER list_h;
	int i;
	int list_num = NELEMS(CountryListTbl) + 2;	// +2 = ���ɂ��Ȃ��@���ǂ�
	
	*menulist = BmpMenuWork_ListCreate( list_num, HEAPID_WORLDTRADE );

	//�u���ɂ��Ȃ��v
	BmpMenuWork_ListAddArchiveString( *menulist, NormalMsgManager, msg_gtc_search_015, 0, HEAPID_WORLDTRADE  );
	
	//�����o�^
	for(i=1;i<list_num-1;i++){
		BmpMenuWork_ListAddArchiveString( *menulist, CountryMsgManager, CountryListTbl[i-1], i, HEAPID_WORLDTRADE  );
	}

	//�u���ǂ�v
	BmpMenuWork_ListAddArchiveString( *menulist, NormalMsgManager, msg_gtc_search_016, list_num-1, HEAPID_WORLDTRADE  );

    list_h       = MenuListHeader;
    list_h.count = list_num;
    list_h.list  = *menulist;
    list_h.win   = win;

	// �g�`��
	BmpWinFrame_Write( win, WINDOW_TRANS_OFF, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

	// BMP���j���[�J�n
    return BmpMenuList_Set(&list_h, 0, 0, HEAPID_WORLDTRADE);

}

//==============================================================================
/**
 * @brief   ���w�������������
 *
 * @param   dtps			���������\����
 * @param   country_code	���R�[�h
 *
 * @retval  none		
 */
//==============================================================================
void WorldTrade_CountryCodeSet( WORLDTRADE_WORK *wk, int country_code )
{
	if(country_code == 0){
		wk->country_code = 0;
	}
	else if(country_code - 1 < NELEMS(CountryListTbl)){
		wk->country_code = CountryListTbl[country_code - 1];
	}
}

//==============================================================================
/**
 * @brief   �w��̓������̍����͂������邩�H
 *
 * @param   start	�u�A�C�E�c�v�Ȃǂ�\��������
 *
 * @retval  int		�����̌�
 */
//==============================================================================
int WorldTrade_NationSortListNumGet( int start, int *number )
{
	int i,count=0;
	
	for(i=0,count=0;i<COUNTRY_SORT_HEAD_MAX;i++){
		if(CountrySortList[i][1]==start){
			*number = CountrySortList[i][0];
			return CountrySortList[i+1][0]-CountrySortList[i][0];
		}
	}
	return count;
}

//==============================================================================
/**
 * @brief   ��������n���č������X�g���쐬����
 *
 * @param   menulist			�������i�[����BMPMENULIST_DATA
 * @param   CountryNameManager	���b�Z�[�W�f�[�^�}�l�[�W���[
 * @param   start				������(�A=�O,�C���P�A�J���T�A�c�j
 *
 * @retval  int		�Y�����鍑���̐�
 */
//==============================================================================
int WorldTrade_NationSortListMake( BMP_MENULIST_DATA **menulist, GFL_MSGDATA *CountryNameManager, int start )
{
	int i,count, head;
	
	// �w��̍����͂������邩�擾
	count = WorldTrade_NationSortListNumGet( start, &head );

	OS_Printf("country start=%d, num=%d\n", head, count);

	// ���ڃ��X�g�̍쐬
	*menulist = BmpMenuWork_ListCreate( count, HEAPID_WORLDTRADE );
	
	// BMPMENULIST_DATA�쐬
	for(i=0;i<count;i++){
		BmpMenuWork_ListAddArchiveString( *menulist, CountryNameManager, CountryListTbl[head+i], head+i+1, HEAPID_WORLDTRADE  );
	}
	
	return count;
}



//==============================================================================
/**
 * @brief   ���ʉ��ɑΉ�����BMPLISTMAIN�֐�
 *
 * @param   lw			���X�g���[�N
 * @param   posbackup	�J�[�\���ʒu
 *
 * @retval  u32			�I������
 */
//==============================================================================
u32 WorldTrade_BmpMenuList_Main( BMPMENULIST_WORK * lw, u16 *posbackup )
{
	u16 listpos;
	u32 result = BmpMenuList_Main( lw );

	BmpMenuList_DirectPosGet( lw, &listpos );

	if(*posbackup != listpos){
		if(*posbackup!=0xffff){
			PMSND_PlaySE(WORLDTRADE_MOVE_SE);
		}
		*posbackup = listpos;
		
	}

	return result;
}

//==============================================================================
/**
 * @brief   �I���ʒu�J�[�\�����̃N���A
 *
 * @param   slp		
 *
 * @retval  none		
 */
//==============================================================================
void WorldTrade_SelectListPosInit( SELECT_LIST_POS *slp)
{
	int i;
	
	for(i=0;i<NAME_HEAD_MAX;i++){
		slp->name_list[i] = 0;
		slp->name_pos[i]  = 0;
	}
	
	slp->head_list = 0;
	slp->head_pos  = 0;

	
}


//=============================================================================================
/**
 * @brief	�J�[�\���ʒu�ۑ�
 *
 * @param   slp		���X�g�J�[�\���ʒu
 * @param   head	���X�g�J�n�ʒu
 * @param   list	���O���X�g�̔ԍ�
 * @param   pos		�y�[�W��̈ʒu
 */
//=============================================================================================
void WorldTrade_SelectNameListBackup( SELECT_LIST_POS *slp, int head, int list, int pos )
{
	slp->name_list[head] = list;
	slp->name_pos[head]  = pos;
}

