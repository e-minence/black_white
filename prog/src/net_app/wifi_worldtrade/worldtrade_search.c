//============================================================================================
/**
 * @file	worldtrade_search.c
 * @brief	���E�����|�P����������ʏ���
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
#include "net_app/worldtrade.h"
#include "worldtrade_local.h"

#include "msg/msg_wifi_gts.h"
#include "system/gf_date.h"



#include "worldtrade.naix"			// �O���t�B�b�N�A�[�J�C�u��`

#include "msg/msg_wifi_place_msg_world.h"

//���O���t�B�b�N�f�[�^�ˑ��̒�`
#define FRD_VIEW_BTN_PX	(17)
#define FRD_VIEW_BTN_PY	(2)
#define FRD_VIEW_BTN_SX	(15)
#define FRD_VIEW_BTN_SY	(4)
#define FRD_VIEW_OK_PAL	(0)
#define FRD_VIEW_NG_PAL	(2)

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

static  int SubSeq_Start( WORLDTRADE_WORK *wk);
static  int SubSeq_Main( WORLDTRADE_WORK *wk);
static  int SubSeq_SearchCheck( WORLDTRADE_WORK *wk );
static  int SubSeq_ServerQuery(WORLDTRADE_WORK *wk );
static  int SubSeq_ServerResult( WORLDTRADE_WORK *wk );
static  int SubSeq_ServerSeWait( WORLDTRADE_WORK *wk );
static  int SubSeq_SearchResultMessage( WORLDTRADE_WORK *wk );
static  int CursorPosGet( WORLDTRADE_WORK *wk );
static void CursorMove( WORLDTRADE_WORK *wk );
static  int SubSeq_End( WORLDTRADE_WORK *wk);
static  int SubSeq_End_Err( WORLDTRADE_WORK *wk);
static  int SubSeq_InputPokenameMessage( WORLDTRADE_WORK *wk);


static  int SUBSEQ_PokenameSelectList( WORLDTRADE_WORK *wk);
static  int SUBSEQ_PokenameSelectWait( WORLDTRADE_WORK *wk);
static  int SUBSEQ_SexSelectMes( WORLDTRADE_WORK *wk);
static  int SUBSEQ_SexSelectList( WORLDTRADE_WORK *wk);
static  int SUBSEQ_SexSelectWait( WORLDTRADE_WORK *wk);
static  int SUBSEQ_LevelSelectMes( WORLDTRADE_WORK *wk);
static  int SUBSEQ_LevelSelectList( WORLDTRADE_WORK *wk);
static  int SUBSEQ_LevelSelectWait( WORLDTRADE_WORK *wk);
static  int SUBSEQ_CountrySelectMes( WORLDTRADE_WORK *wk);
static  int SUBSEQ_CountrySelectList( WORLDTRADE_WORK *wk);
static  int SUBSEQ_CountrySelectWait( WORLDTRADE_WORK *wk);
static  int SubSeq_YesNo( WORLDTRADE_WORK *wk);
static  int SubSeq_YesNoSelect( WORLDTRADE_WORK *wk);
static  int SubSeq_PageChange( WORLDTRADE_WORK *wk);
static  int SubSeq_SearchErrorMessage(WORLDTRADE_WORK *wk);
static  int SubSeq_MessageWait( WORLDTRADE_WORK *wk );
static  int SubSeq_MessageWaitTrg( WORLDTRADE_WORK *wk );
static  int SubSeq_SearchResultMessageWait( WORLDTRADE_WORK *wk );
static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat );
static  int SubSeq_ServerQueryFailure( WORLDTRADE_WORK *wk );
static  int SubSeq_SearchErrorDisconnectMessage1( WORLDTRADE_WORK *wk );
static  int SubSeq_SearchErrorDisconnectMessage2( WORLDTRADE_WORK *wk );
static  int SubSeq_SearchErrorDisconnectMessage3( WORLDTRADE_WORK *wk );
static  int SubSeq_ExchangeScreen1( WORLDTRADE_WORK *wk );
static  int SubSeq_ReturnScreen2( WORLDTRADE_WORK *wk );
static  int SubSeq_ExchangeMain( WORLDTRADE_WORK *wk );
static  int SubSeq_DecideWait( WORLDTRADE_WORK *wk );
static  int SubSeq_CancelWait( WORLDTRADE_WORK *wk );
static  int SubSeq_AnmWait( WORLDTRADE_WORK *wk );


static void WantLabelPrint( GFL_BMPWIN *win[], GFL_BMPWIN *country_win[], GFL_MSGDATA *MsgManager, WT_PRINT *print );
static  int LastTradeDateCheck( WORLDTRADE_WORK *wk, int trade_type );
static  int DpwSerachCompare( const Dpw_Tr_PokemonSearchData *s1, const Dpw_Tr_PokemonSearchData *s2 , int country_code1, int country_code2);
static  int SubSeq_MessageWait1Min( WORLDTRADE_WORK *wk );
static void SelectFrameBoxWrite( u16 *scr, int x, int y, int type );
static void TouchCursorMove( WORLDTRADE_WORK *wk, int touch );
static  u32 TouchPanelFunc( WORLDTRADE_WORK *wk );
static void DecideFunc( WORLDTRADE_WORK *wk, int decide );
static void FriendViewButtonPrint( GFL_BMPWIN *win, GFL_MSGDATA *MsgManager, int flag, WT_PRINT *print );

static  u32 WordHead_SelectMain( WORLDTRADE_WORK *wk );
static void SlideScreenVFunc( void *p );
static void BgBlendSet( int rate  );

enum{
	SUBSEQ_START,
	SUBSEQ_MAIN,
	SUBSEQ_END,
	SUBSEQ_END_ERR,

	SUBSEQ_INPUT_POKENAME_MES,
    SUBSEQ_POKENAME_SELECT_LIST,
    SUBSEQ_POKENAME_SELECT_WAIT,
	SUBSEQ_SEX_SELECT_MES,
	SUBSEQ_SEX_SELECT_LIST,
	SUBSEQ_SEX_SELECT_WAIT,
	SUBSEQ_LEVEL_SELECT_MES,
	SUBSEQ_LEVEL_SELECT_LIST,
	SUBSEQ_LEVEL_SELECT_WAIT,
	SUBSEQ_COUNTRY_SELECT_MES,
	SUBSEQ_COUNTRY_SELECT_LIST,
	SUBSEQ_COUNTRY_SELECT_WAIT,

	SUBSEQ_SEARCH_CHECK,
	SUBSEQ_SERVER_QUERY,
	SUBSEQ_SERVER_RESULT,
  SUBSEQ_SERVER_SE_WAIT,
	SUBSEQ_SEARCH_RESULT_MESSAGE,
	SUBSEQ_SEARCH_RESULT_MESSAGE_WAIT,

	SUBSEQ_SERVER_QUERY_FAILURE,


	SUBSEQ_MES_WAIT,
	SUBSEQ_MES_WAIT_TRG,
	SUBSEQ_MES_WAIT_1MIN,
	SUBSEQ_YESNO,
	SUBSEQ_YESNO_SELECT,
	SUBSEQ_PAGE_CHANGE,
	SUBSEQ_SEARCH_ERROR_MES,

	SUBSEQ_SEARCH_ERROR_DICONNECT_MES1,
	SUBSEQ_SEARCH_ERROR_DICONNECT_MES2,

	 SUBSEQ_EXCHANGE_SCREEN1,	
     SUBSEQ_RETURN_SCREEN2,	
     SUBSEQ_EXCHANGE_MAIN,	

  SUBSEQ_DECIDE_WAIT,
  SUBSEQ_CANCEL_WAIT,
  SUBSEQ_ANM_WAIT,
};

static int (*Functable[])( WORLDTRADE_WORK *wk ) = {
	SubSeq_Start,				// SUBSEQ_START
	SubSeq_Main,             	// SUBSEQ_MAIN,
	SubSeq_End,              	// SUBSEQ_END,
	SubSeq_End_Err,              	// SUBSEQ_END_ERR

	SubSeq_InputPokenameMessage,	// SUBSEQ_INPUT_POKENAME_MES,
	
    SUBSEQ_PokenameSelectList,      // SUBSEQ_POKENAME_SELECT_LIST,            
    SUBSEQ_PokenameSelectWait,      // SUBSEQ_POKENAME_SELECT_WAIT,            
	SUBSEQ_SexSelectMes,            // SUBSEQ_SEX_SELECT_MES,            
	SUBSEQ_SexSelectList,           // SUBSEQ_SEX_SELECT_LIST,            
	SUBSEQ_SexSelectWait,           // SUBSEQ_SEX_SELECT_WAIT,            
	SUBSEQ_LevelSelectMes,          // SUBSEQ_LEVEL_SELECT_MES,            
	SUBSEQ_LevelSelectList,         // SUBSEQ_LEVEL_SELECT_LIST,            
	SUBSEQ_LevelSelectWait,         // SUBSEQ_LEVEL_SELECT_WAIT,            
	SUBSEQ_CountrySelectMes,        // SUBSEQ_COUNTRY_SELECT_MES,            
	SUBSEQ_CountrySelectList,       // SUBSEQ_COUNTRY_SELECT_LIST,            
	SUBSEQ_CountrySelectWait,       // SUBSEQ_COUNTRY_SELECT_WAIT,            
                                                        
	SubSeq_SearchCheck,				// SUBSEQ_SEARCH_CHECK,     
	SubSeq_ServerQuery,             // SUBSEQ_SERVER_QUERY,                  
	SubSeq_ServerResult,            // SUBSEQ_SERVER_RESULT,
  SubSeq_ServerSeWait,            // SUBSEQ_SERVER_SE_WAIT
	SubSeq_SearchResultMessage,     // SUBSEQ_SEARCH_RESULT_MESSAGE,
	SubSeq_SearchResultMessageWait, // SUBSEQ_SEARCH_RESULT_MESSAGE_WAIT,

	SubSeq_ServerQueryFailure,		// SUBSEQ_SERVER_QUERY_FAILURE,


	SubSeq_MessageWait,     		// SUBSEQ_MES_WAIT
	SubSeq_MessageWaitTrg,     		// SUBSEQ_MES_WAIT_TRG
	SubSeq_MessageWait1Min,			// SUBSEQ_MES_WAIT_1MIN
	SubSeq_YesNo,					// SUBSEQ_YESNO
	SubSeq_YesNoSelect,				// SUBSEQ_YESNO_SELECT
	SubSeq_PageChange,				// SUBSEQ_PAGE_CHANGE,
	SubSeq_SearchErrorMessage,		// SUBSEQ_SEARCH_ERROR_MES,

	SubSeq_SearchErrorDisconnectMessage1,	// 	SUBSEQ_SEARCH_ERROR_DICONNECT_MES1,
	SubSeq_SearchErrorDisconnectMessage2,	// 	SUBSEQ_SEARCH_ERROR_DICONNECT_MES2,

	SubSeq_ExchangeScreen1,			// SUBSEQ_EXCHANGE_SCREEN1,	
	SubSeq_ReturnScreen2,			// SUBSEQ_RETURN_SCREEN2,	
	SubSeq_ExchangeMain,			// SUBSEQ_EXCHANGE_MAIN,	

  SubSeq_DecideWait, // SUBSEQ_DECIDE_WAIT
  SubSeq_CancelWait, // SUBSEQ_CANCEL_WAIT
  SubSeq_AnmWait,// SUBSEQ_ANM_WAIT
};

#define INFO_TEXT_WORD_NUM	(10*2)


// �u�������v��ʂ̃J�[�\���ʒu
static u16 CursorPos[][4]={
	{   0,    0, 2, 11 },	// X, Y, �Z���A�N�^�[�̃A�j���ԍ��B���莞�̃A�j���ԍ�
	{   0,   40, 2, 11 },
	{   0,   80, 2, 11 },
	{   0,  120, 2, 11 },
	{ 192,   32, 3, 12 },
	{ 192,   72, 3, 12 },
	{ 192,  112, 3, 12 },
	
};

// �J�[�\���̈ʒu�w���`
enum{
	CURSOR_POS_POKEMON = 0,
	CURSOR_POS_SEX,
	CURSOR_POS_LEVEL,
	CURSOR_POS_NATION,
	CURSOR_POS_VIEW,
	CURSOR_POS_SEARCH,
	CURSOR_POS_BACK,
};


//���������̕\�����E�񂹂��邩�A�Z���^�����O���邩�A���񂹂ɂ��邩
#define SEARCH_INFO_PRINT_FLAG		(0)	//0:���A1:�Z���^�����O�A2:�E��

///�����^�C���A�E�g�܂ł̎���
#define TIMEOUT_TIME			(60*60*2)	//2��


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
int WorldTrade_Search_Init(WORLDTRADE_WORK *wk, int seq)
{
	// ���[�N������
	InitWork( wk );
	
  //���C����ʂ̏���
	// BG�ݒ�
	BgInit();
	// BG�O���t�B�b�N�]��
	BgGraphicSet( wk );
	// BMPWIN�m��
	BmpWinInit( wk );

	// �Z���A�N�^�[�o�^
	SetCellActor(wk);

  //�T�u��ʂ̏���
	WorldTrade_SubLcdBgInit( wk, 0, 0 );  
  WorldTrade_SubLcdWinGraphicSet( wk );   // �g���[�h���[���E�C���h�E�]��
  // �T�u��ʂ�GTS�����pBMPWIN���m�ۂ���
	WorldTrade_SubLcdExpainPut( wk, EXPLAIN_SAGASU );

  wk->sub_display_continue  = FALSE;

	// �������̓V�X�e��������
	{
		WORLDTRADE_INPUT_HEADER wih;
		wih.MenuBmp   = &wk->MenuWin[0];
		wih.BackBmp   = &wk->BackWin;
		wih.CursorAct = wk->SubCursorActWork;
		wih.ArrowAct[0] = wk->BoxArrowActWork[0];
		wih.ArrowAct[1] = wk->BoxArrowActWork[1];
		wih.SearchCursorAct = wk->CursorActWork;
		wih.MsgManager      = wk->MsgManager;
		wih.MonsNameManager = wk->MonsNameManager;
		wih.CountryNameManager = wk->CountryNameManager;
		wih.Zukan			= wk->param->zukanwork;
		wih.SinouTable      = wk->dw->sinouTable;
		wk->WInputWork = WorldTrade_Input_Init( &wih,  GFL_BG_FRAME2_M, SITUATION_SEARCH );
	}

	// �u�ق����|�P�����v�`��
	WantLabelPrint( &wk->InfoWin[0], &wk->CountryWin[0], wk->MsgManager, &wk->print );

	// �u�����Ă��݂�v�`��
	FriendViewButtonPrint(  wk->InfoWin[8], wk->MsgManager, wk->SubLcdTouchOK, &wk->print );


	// ���O
	WorldTrade_PokeNamePrint( wk->InfoWin[1], wk->MonsNameManager, 
		wk->Search.characterNo, SEARCH_INFO_PRINT_FLAG, 0, PRINTSYS_LSB_Make(15,2,0), &wk->print  );

	// ���ʏ���
	WorldTrade_SexPrint( wk->InfoWin[3], wk->MsgManager, 
		wk->Search.gender, 1, 0, SEARCH_INFO_PRINT_FLAG, PRINTSYS_LSB_Make(15,2,0), &wk->print  );

	// ���x���w��
	WorldTrade_WantLevelPrint( wk->InfoWin[5], wk->MsgManager, 
		WorldTrade_LevelTermGet(wk->Search.level_min,wk->Search.level_max, LEVEL_PRINT_TBL_SEARCH),
		SEARCH_INFO_PRINT_FLAG, 0, PRINTSYS_LSB_Make(15,2,0), LEVEL_PRINT_TBL_SEARCH, &wk->print );

	// ���w��
	WorldTrade_CountryPrint( wk->CountryWin[1], wk->CountryNameManager, wk->MsgManager,
		wk->country_code, SEARCH_INFO_PRINT_FLAG, 0, PRINTSYS_LSB_Make(15,2,0), &wk->print );

	wk->vfunc2 = SlideScreenVFunc;
	
	// �Q��ڈȍ~
	// ���C�v�t�F�[�h�J�n�i����ʁj

	if(wk->sub_process_mode==MODE_NORMAL){
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
						WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
	}
	wk->subprocess_seq = SUBSEQ_START;
#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_search.c [218] M ********************\n" );
#endif

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
int WorldTrade_Search_Main(WORLDTRADE_WORK *wk, int seq)
{
	int i;
	int ret;
	
	ret = (*Functable[wk->subprocess_seq])( wk );

	// �T�u��ʂ�OBJ���W���ړ������鏈��
	for(i=0;i<SUB_OBJ_NUM;i++){
		WorldTrade_CLACT_PosChangeSub( wk->SubActWork[i], wk->SubActY[i][0], wk->SubActY[i][1]+wk->DrawOffset );
	}

	WorldTrade_CLACT_PosChange( wk->CursorActWork,  
								CursorPos[CursorPosGet( wk )][0], 
								CursorPos[CursorPosGet( wk )][1] - wk->DrawOffset );

  WorldTrade_CLACT_PosChangeSub( wk->PromptDsActWork, DS_ICON_X, DS_ICON_Y+wk->DrawOffset );

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
int WorldTrade_Search_End(WORLDTRADE_WORK *wk, int seq)
{
	wk->vfunc2 = NULL;
	
	// �������̓V�X�e���I��
	WorldTrade_Input_Exit( wk->WInputWork );
	
	DelCellActor( wk );
	
	FreeWork( wk );
	
	BmpWinDelete( wk );
	
	BgExit();

  if( wk->sub_nextprocess==WORLDTRADE_TITLE )
  { 
    wk->sub_display_continue  = TRUE;
  }
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
    int sub_visible = GFL_DISP_GetSubVisible(); //GFL_BG_SetBGMode��Visible�t���O���������̂�
		GFL_BG_SetBGMode( &BGsys_data );
		GFL_DISP_GXS_SetVisibleControlDirect(sub_visible);

	}


	// ���C����ʃe�L�X�g��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000,GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl(  GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(  GFL_BG_FRAME0_M );
		GFL_BG_SetVisible( GFL_BG_FRAME0_M, TRUE );
	}

	// ���C����ʔw�i��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000,GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			3, 0, 0, FALSE
		};
		GFL_BG_SetBGControl(  GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(  GFL_BG_FRAME1_M );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M, TRUE );
	}

	// ���C����ʊ��荞�݃E�C���h�E��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000,GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl(  GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(  GFL_BG_FRAME2_M );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M, TRUE );
	}

	// ���C����ʑ�R�e�L�X�g��
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x1c000,GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			2, 0, 0, FALSE
		};
		GFL_BG_SetBGControl(  GFL_BG_FRAME3_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen(  GFL_BG_FRAME3_M );
		GFL_BG_SetVisible( GFL_BG_FRAME3_M, TRUE );
	}

	GFL_BG_SetClearCharacter( GFL_BG_FRAME2_M, 32, 0, HEAPID_WORLDTRADE );
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
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
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
	GFL_ARCHDL_UTIL_TransVramPalette(    p_handle, NARC_worldtrade_search_nclr, PALTYPE_MAIN_BG, 0, 16*3*2,  HEAPID_WORLDTRADE);
	
	// ��b�t�H���g�p���b�g�]��
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );
  // 	TalkFontPaletteLoad( PALTYPE_SUB_BG,  WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

	// ��b�E�C���h�E�O���t�B�b�N�]��
	BmpWinFrame_GraphicSet(	 GFL_BG_FRAME0_M, WORLDTRADE_MESFRAME_CHR, 
						WORLDTRADE_MESFRAME_PAL,  0, HEAPID_WORLDTRADE );

	BmpWinFrame_GraphicSet(	 GFL_BG_FRAME0_M, WORLDTRADE_MENUFRAME_CHR,
						WORLDTRADE_MENUFRAME_PAL, 0, HEAPID_WORLDTRADE );



	// ���C�����BG1�L�����]��
	GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_worldtrade_search_lz_ncgr,  GFL_BG_FRAME1_M, 0, 0, 1, HEAPID_WORLDTRADE);

	// ���C�����BG1�X�N���[���]��
	GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_worldtrade_search_lz_nscr,  GFL_BG_FRAME1_M, 0, 0, 1, HEAPID_WORLDTRADE);

	// ���C�����BG2�L�����]��
	GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_worldtrade_input_lz_ncgr,   GFL_BG_FRAME2_M, 0, 0, 1, HEAPID_WORLDTRADE);


	// �T�u���BG1�L�����]��
	//GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_worldtrade_traderoom_lz_ncgr,  GFL_BG_FRAME1_S, 0, 32*21*0x40, 1, HEAPID_WORLDTRADE);

	// �T�u���BG1�X�N���[���]��
	//GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_worldtrade_traderoom_lz_nscr,  GFL_BG_FRAME1_S, 0, 32*24*2, 1, HEAPID_WORLDTRADE);

	// ��b�t�H���g�p���b�g�]��
	TalkFontPaletteLoad( PALTYPE_SUB_BG, WORLDTRADE_SUB_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

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
	GFL_CLWK_DATA	add;

	GFL_STD_MemClear( &add, sizeof(GFL_CLWK_DATA) );

	// �J�[�\���\��
	add.pos_x = CursorPos[0][0];
	add.pos_y = CursorPos[0][1];
	wk->CursorActWork = GFL_CLACT_WK_Create( wk->clactSet,
			wk->resObjTbl[RES_CURSOR][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[RES_CURSOR][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
	GFL_CLACT_WK_SetAutoAnmFlag( wk->CursorActWork, 1 );
	GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[0][2] );
	GFL_CLACT_WK_SetBgPri( wk->CursorActWork, 1 );	//�����I���E�B���h�E�̉��ɍs������׉�����

  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 0 );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
  }
	
	// �T�u�J�[�\���o�^
	add.pos_x = 160;
	add.pos_y = 32;
	wk->SubCursorActWork = GFL_CLACT_WK_Create( wk->clactSet,
			wk->resObjTbl[RES_CURSOR][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[RES_CURSOR][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
	GFL_CLACT_WK_SetAnmSeq( wk->SubCursorActWork, 4 );
	GFL_CLACT_WK_SetDrawEnable( wk->SubCursorActWork, 0 );

	// �i�E�����j
	add.pos_x = 228;
	add.pos_y = 117;
	wk->BoxArrowActWork[0] = GFL_CLACT_WK_Create( wk->clactSet,
			wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
	GFL_CLACT_WK_SetAnmSeq( wk->BoxArrowActWork[0], CELL_BOXARROW_NO );
	GFL_CLACT_WK_SetDrawEnable( wk->BoxArrowActWork[0], 0 );
	
	// �y�[�W�ړ��J�[�\���o�^(�������j
	add.pos_x = 140;
	wk->BoxArrowActWork[1] = GFL_CLACT_WK_Create( wk->clactSet,
			wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
	GFL_CLACT_WK_SetAnmSeq( wk->BoxArrowActWork[1], CELL_BOXARROW_NO+1 );
	GFL_CLACT_WK_SetDrawEnable( wk->BoxArrowActWork[1], 0 );

	WirelessIconEasy();

}

//------------------------------------------------------------------
/**
 * @brief   �u�������v��ʂœo�^�����Z���A�N�^�[�̍폜
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void DelCellActor( WORLDTRADE_WORK *wk )
{
	GFL_CLACT_WK_Remove(wk->CursorActWork);
	GFL_CLACT_WK_Remove(wk->SubCursorActWork);
	GFL_CLACT_WK_Remove(wk->BoxArrowActWork[0]);
	GFL_CLACT_WK_Remove(wk->BoxArrowActWork[1]);


}


// �u�|�P�����v�E���O�E�u�����ׂv�E���ʁE�u���x���v�E���x��
#define INFORMATION_STR_X	(  1 )
#define INFORMATION_STR_X2	(  2 )
#define INFORMATION1_STR_Y	(  1 )
#define INFORMATION2_STR_Y	(  3 )
#define INFORMATION3_STR_Y	(  6 )
#define INFORMATION4_STR_Y	(  8 )
#define INFORMATION5_STR_Y	( 11 )
#define INFORMATION6_STR_Y	( 13 )
#define INFORMATION7_STR_Y	( 16 )
#define INFORMATION8_STR_Y	( 18 )
#define INFORMATION_STR_SX	( 11 )
#define INFORMATION_STR_SY	(  2 )


#define BUTTON_STR_X		( 22 )
#define BUTTON1_STR_Y		(  8 )
#define BUTTON2_STR_Y		( 13 )
#define BUTTON3_STR_Y		(  3 )
#define BUTTON_STR_SX		( 10 )
#define BUTTON_STR_SY		(  2 )

#define COUNTRY_STR_SX	( 28 )
#define COUNTRY_STR_SY	(  2 )

// BG0��
#define TITLE_MESSAGE_OFFSET   ( WORLDTRADE_MENUFRAME_CHR + MENU_WIN_CGX_SIZ )
#define LINE_MESSAGE_OFFSET    ( TITLE_MESSAGE_OFFSET     + TITLE_TEXT_SX*TITLE_TEXT_SY )
//#define BUTTON_STR_OFFSET	   ( LINE_MESSAGE_OFFSET      + LINE_TEXT_SX*LINE_TEXT_SY )
//#define SELECT_MENU1_OFFSET    ( BUTTON_STR_OFFSET        + BUTTON_STR_SX*BUTTON_STR_SY*2 )
#define SELECT_MENU1_OFFSET    ( TITLE_MESSAGE_OFFSET        + BUTTON_STR_SX*BUTTON_STR_SY*2 )
#define SELECT_MENU2_OFFSET    ( SELECT_MENU1_OFFSET      + SELECT_MENU1_SX*SELECT_MENU1_SY )
#define SELECT_MENU3_OFFSET    ( SELECT_MENU1_OFFSET )
#define SELECT_MENU4_OFFSET    ( SELECT_MENU1_OFFSET )
#define SELECT_MENU5_OFFSET    ( SELECT_MENU1_OFFSET )
#define YESNO_OFFSET 		   ( SELECT_MENU4_OFFSET      + SELECT_MENU4_SX*SELECT_MENU4_SY )


//BG3��
#define INFOMATION_STR_OFFSET  ( 1 )
#define COUNTRY_STR_OFFSET	   ( INFOMATION_STR_OFFSET    + INFORMATION_STR_SX*INFORMATION_STR_SY*6)
static const u16 infomation_bmpwin_table[6][2]={
	{INFORMATION_STR_X,   INFORMATION1_STR_Y,},
	{INFORMATION_STR_X2,  INFORMATION2_STR_Y,},
	{INFORMATION_STR_X,   INFORMATION3_STR_Y,},
	{INFORMATION_STR_X2,  INFORMATION4_STR_Y,},
	{INFORMATION_STR_X,   INFORMATION5_STR_Y,},
	{INFORMATION_STR_X2,  INFORMATION6_STR_Y,},

};

#define BUTTON_STR_OFFSET	   ( COUNTRY_STR_OFFSET   +  (COUNTRY_STR_SX*COUNTRY_STR_SY)*2 + 20*2 )

static const u16 button_bmpwin_table[][2]={
	{BUTTON_STR_X,	BUTTON1_STR_Y},
	{BUTTON_STR_X,	BUTTON2_STR_Y},
	{BUTTON_STR_X,	BUTTON3_STR_Y},
};

static const u16 country_bmpwin_table[][2]={
	{INFORMATION_STR_X,		INFORMATION7_STR_Y},
	{INFORMATION_STR_X2,	INFORMATION8_STR_Y},
};

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

//	GFL_BMPWIN_Create( &wk->TitleWin, GFL_BG_FRAME0_M,
//		TITLE_TEXT_X, TITLE_TEXT_Y, TITLE_TEXT_SX, TITLE_TEXT_SY, 
//		WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );

//	GFL_BMP_Clear( &wk->TitleWin, 0x0000 );
	
	// �u�|�P�������������v�`��
//	WorldTrade_TalkPrint( &wk->TitleWin, wk->TitleString, 0, 1, 0, PRINTSYS_LSB_Make(15,13,0) );

	// ��b�E�C���h�E
	wk->MsgWin	= GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
		LINE_TEXT_X, LINE_TEXT_Y, LINE_TEXT_SX, LINE_TEXT_SY, 
		WORLDTRADE_TALKFONT_PAL, GFL_BMP_CHRAREA_GET_B );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0x0000 );
	GFL_BMPWIN_MakeTransWindow( wk->MsgWin );

  //BG�R�ʂ̓t�H���g�̂�
  GFL_BG_FillCharacter( GFL_BG_FRAME3_M, 0, 1, 0 );

	// BG0��BMPWIN���E�C���h�E�m��
	{
		int i, offset;

		// �ق����|�P�������x�U������
		for(i=0;i<6;i++){
			wk->InfoWin[i]	= GFL_BMPWIN_Create( GFL_BG_FRAME3_M,
					infomation_bmpwin_table[i][0], 
					infomation_bmpwin_table[i][1], 
					INFORMATION_STR_SX, INFORMATION_STR_SY,
					WORLDTRADE_TALKFONT_PAL,  
					GFL_BMP_CHRAREA_GET_F );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin[i]), 0x0000 );
			GFL_BMPWIN_MakeTransWindow( wk->InfoWin[i] );
		}

		// �Z��ł��鏊�E����
		for(i=0;i<2;i++){
			wk->CountryWin[i]	= GFL_BMPWIN_Create( GFL_BG_FRAME3_M,
					country_bmpwin_table[i][0], 
					country_bmpwin_table[i][1], 
					COUNTRY_STR_SX,COUNTRY_STR_SY,
					WORLDTRADE_TALKFONT_PAL,  
					GFL_BMP_CHRAREA_GET_F );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->CountryWin[i]), 0x0000 );
			GFL_BMPWIN_MakeTransWindow( wk->CountryWin[i] );
		}

		// �������E���ǂ�E�����Ă��݂�
		for(i=0;i<3;i++){
			wk->InfoWin[6+i]	= GFL_BMPWIN_Create( GFL_BG_FRAME3_M,
					button_bmpwin_table[i][0], 
					button_bmpwin_table[i][1], 
					BUTTON_STR_SX,BUTTON_STR_SY,
					WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_F);
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin[6+i]), 0x0000 );
			GFL_BMPWIN_MakeTransWindow( wk->InfoWin[6+i] );
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
  //BMP�������O�Ɏc�����X�g���[��������
  WT_PRINT_ClearBuffer( &wk->print );
	
	GFL_BMPWIN_Delete( wk->ExplainWin );
	GFL_BMPWIN_Delete( wk->MsgWin );
//	GFL_BMPWIN_Delete( wk->TitleWin );
	{
		int i;
		for(i=0;i<9;i++){
			GFL_BMPWIN_Delete( wk->InfoWin[i] );
		}
		for(i=0;i<2;i++){
			GFL_BMPWIN_Delete( wk->CountryWin[i] );
		}
	}

  GFL_BG_FillCharacterRelease( GFL_BG_FRAME3_M, 1, 0 );
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

	// �u�|�P�������������v
	wk->TitleString = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_02_003 );


	// �|�P�������e�[�u���쐬�p�̃��[�N�m��
	wk->dw = GFL_HEAP_AllocMemory(HEAPID_WORLDTRADE, sizeof(DEPOSIT_WORK));
	MI_CpuClearFast(wk->dw, sizeof(DEPOSIT_WORK));

	wk->dw->sinouTable    = WorldTrade_SinouZukanDataGet( HEAPID_WORLDTRADE );

	// �J�[�\���ʒu������
	WorldTrade_SelectListPosInit( &wk->selectListPos );
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
	GFL_HEAP_FreeMemory( wk->dw->sinouTable );

	// �����Ԃ񂱂ꂢ��Ȃ�
	GFL_HEAP_FreeMemory( wk->dw );

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
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 0 );
  }
  else 
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
  }


	if( wk->sub_process_mode==MODE_PARTNER_RETURN ){
		//���b�Z�[�W�\�� �u����ԁv�Ł@�����Ă��@���߂Ă�������
		SubSeq_MessagePrint( wk, msg_gtc_01_033, MSG_ALLPUT, 0, 0x0f0f );	//�ꊇ�\��
		if(GX_GetMasterBrightness() == -16){    //�^���ÂȂ�΁u����ԁv��ʂ���̖߂�
    		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
    						WIPE_FADE_BLACK, EXCHANGE_SCREEN_SYNC, 1, HEAPID_WORLDTRADE );
    		wk->subprocess_seq = SUBSEQ_RETURN_SCREEN2;
    	}
    	else{
    		wk->subprocess_seq = SUBSEQ_MAIN;
        }
	}else{
		SubSeq_MessagePrint( wk, msg_gtc_01_009, 1, 0, 0x0f0f );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
	}

	// �u����ʂ��݂Ăˁv�A�C�R����\������
	GFL_CLACT_WK_SetDrawEnable( wk->PromptDsActWork, 1 );

	OS_Printf("monsno = %d, gender = %d, level = %d - %d, country = %d\n", 
			wk->Search.characterNo,wk->Search.gender,wk->Search.level_min,wk->Search.level_max,
			wk->country_code);

	OS_Printf("remain ram = %d\n", GFL_HEAP_GetHeapFreeSize( HEAPID_WORLDTRADE ));


	return SEQ_MAIN;
}

// ������ʃ^�b�`���o�e�[�u��
static const GFL_UI_TP_HITTBL TouchTable[]={
	{   3,  41,   2,  99,},
	{  43,  81,   2,  99,},
	{  83, 121,   2,  99,},
	{ 123, 161,   2,  99,},
	{  19,  45, 145, 255,},
	{  58,  85, 145, 255,},
	{  98, 125, 145, 255,},
	{GFL_UI_TP_HIT_END,0,0,0},		// �I���f�[�^
	
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

	return ret;
}


//------------------------------------------------------------------
/**
 * @brief   ���菈���i�^�b�`���L�[���ʁj
 *
 * @param   wk		
 * @param   decide		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void DecideFunc( WORLDTRADE_WORK *wk, int decide )
{
	switch(decide){
	// �|�P�����w��
	case CURSOR_POS_POKEMON:	
		wk->subprocess_seq = SUBSEQ_INPUT_POKENAME_MES;
		break;
	// ���ʎw��
	case CURSOR_POS_SEX:		
		// ���Ƀ|�P��������͂��Ă��āA���̃|�P�����̐��ʂ����肵�Ă���Ƃ���
		// ���͂ł��Ȃ��悤�ɂ���
		if(wk->Search.characterNo!=0){
			wk->dw->sex_selection = PokePersonalParaGet(wk->Search.characterNo,POKEPER_ID_sex);
			if(WorldTrade_SexSelectionCheck( &wk->Search, wk->dw->sex_selection )){
				OS_Printf("���ʌ���H = %d\n", wk->dw->sex_selection);
				return;
			}
		}
		wk->subprocess_seq = SUBSEQ_SEX_SELECT_MES;		
		break;
	// ���x���w��
	case CURSOR_POS_LEVEL:		
		wk->subprocess_seq = SUBSEQ_LEVEL_SELECT_MES;	
		break;
	// ���w��
	case CURSOR_POS_NATION:		
		wk->subprocess_seq = SUBSEQ_COUNTRY_SELECT_MES;	
		break;
	// �������ʂ�����
	case CURSOR_POS_VIEW:
		if(wk->SearchResult){
			wk->subprocess_seq = SUBSEQ_EXCHANGE_SCREEN1;
			WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
						WIPE_FADE_BLACK, EXCHANGE_SCREEN_SYNC, 1, HEAPID_WORLDTRADE );
		}
		break;
	// �T��
	case CURSOR_POS_SEARCH:		
		wk->subprocess_seq = SUBSEQ_SEARCH_CHECK;		
		
		break;
	// ���ǂ�
	case CURSOR_POS_BACK:
		SubSeq_MessagePrint( wk, msg_gtc_01_016, 1, 0, 0x0f0f );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_YESNO );
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
	//printstream�ɂ����̂ŁA
	//printstream�I���O�ɕύX���Ă͂����Ȃ�
	


	// ------�^�b�`����--------
	u32 ret=TouchPanelFunc( wk );
	if(ret!=GFL_UI_TP_HIT_NONE){
    int cursorPos;

    GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );

		TouchCursorMove( wk, ret );

      cursorPos = CursorPosGet( wk );
      switch( cursorPos )
      { 
      case CURSOR_POS_SEX:	
        //���ʎw��Ȃ�΁A���ʂ��P�����Ȃ��|�P�����͓��͂ł��Ȃ�
        if(wk->Search.characterNo!=0){
          wk->dw->sex_selection = PokePersonalParaGet(wk->Search.characterNo,POKEPER_ID_sex);
          if(WorldTrade_SexSelectionCheck( &wk->Search, wk->dw->sex_selection )){
            OS_Printf("���ʌ���H = %d\n", wk->dw->sex_selection);
            GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
            GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
            PMSND_PlaySE(SE_GTC_NG);
            return SEQ_MAIN;
          }
        }

        GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
        PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
        GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
        wk->subprocess_seq  = SUBSEQ_DECIDE_WAIT;
        break;
      
      case CURSOR_POS_SEARCH:
        //�u�������v�Ȃ�΁A�����邩�`�F�b�N
        if( wk->Search.characterNo==0 || DpwSerachCompare( &wk->Search, &wk->SearchBackup, wk->country_code, wk->SearchBackup_CountryCode) )
        { 
          PMSND_PlaySE(SE_GTC_NG);
        }
        else
        { 
          PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
        }
        GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
        GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
        wk->subprocess_seq  = SUBSEQ_DECIDE_WAIT;
        break;

      case CURSOR_POS_VIEW:
        //�u����ԁv�Ȃ�΂��łɑI��ł��邩�`�F�b�N
        if(wk->SearchResult)
        { 
          GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
          PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
          GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
          wk->subprocess_seq  = SUBSEQ_DECIDE_WAIT;
        }
        else
        { 
          GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
          GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
          PMSND_PlaySE(SE_GTC_NG);
          wk->subprocess_seq  = SUBSEQ_ANM_WAIT;
        }
        break;

        //�u����ԁv�Ɓu�������v�ȊO�Ȃ�΂����ɉ�����
      default:

        GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );

        if( cursorPos == CURSOR_POS_BACK )
        { 
          PMSND_PlaySE(SE_CANCEL);
        }
        else
        { 
          PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
        }
        GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
        wk->subprocess_seq  = SUBSEQ_DECIDE_WAIT;
        break;
      }
	}else{

	// ------�L�[����----------
    if( GFL_UI_KEY_GetTrg() && GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
    { 
      GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
      GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
      PMSND_PlaySE(WORLDTRADE_MOVE_SE);
      return SEQ_MAIN;
    }

		// �J�[�\���ړ�
		CursorMove(wk);

		// ����
		if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE){
      int cursorPos = CursorPosGet( wk );

      //�u����ԁv�Ɓu�v�ȊO�Ȃ�΂����ɉ�����
      switch( cursorPos )
      {       
      case CURSOR_POS_SEX:	
        //���ʎw��Ȃ�΁A���ʂ��P�����Ȃ��|�P�����͓��͂ł��Ȃ�
        if(wk->Search.characterNo!=0){
          wk->dw->sex_selection = PokePersonalParaGet(wk->Search.characterNo,POKEPER_ID_sex);
          if(WorldTrade_SexSelectionCheck( &wk->Search, wk->dw->sex_selection )){
            OS_Printf("���ʌ���H = %d\n", wk->dw->sex_selection);
            GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
            GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
            PMSND_PlaySE(SE_GTC_NG);
            return SEQ_MAIN;
          }
        }

        GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
        PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
        GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
        wk->subprocess_seq  = SUBSEQ_DECIDE_WAIT;
        break;

      case CURSOR_POS_SEARCH:
        //�u�������v�Ȃ�΁A�����邩�`�F�b�N
        if( wk->Search.characterNo==0 || DpwSerachCompare( &wk->Search, &wk->SearchBackup, wk->country_code, wk->SearchBackup_CountryCode) )
        { 
          PMSND_PlaySE(SE_GTC_NG);
        }
        else
        { 
          PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
        }
        GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
        wk->subprocess_seq  = SUBSEQ_DECIDE_WAIT;
        break;

      case CURSOR_POS_VIEW:
        if(wk->SearchResult)
        { 
          PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
          GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
          wk->subprocess_seq  = SUBSEQ_DECIDE_WAIT;
        }
        else
        { 
          GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
          GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
          PMSND_PlaySE(SE_GTC_NG);
          wk->subprocess_seq  = SUBSEQ_ANM_WAIT;
        }
        break;

      default:
        if( cursorPos == CURSOR_POS_BACK )
        { 
          PMSND_PlaySE(SE_CANCEL);
        }
        else
        { 
          PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
        }
        GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
        wk->subprocess_seq  = SUBSEQ_DECIDE_WAIT;
        break;

      }
		}else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
			// �L�����Z��
      // �L�����Z���ʒu�ֈړ�
      wk->dw->cursorSide  = 1;
      wk->dw->rightCursorPos  = 2;

      GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][3] );
      wk->subprocess_seq  = SUBSEQ_CANCEL_WAIT;
			PMSND_PlaySE(SE_CANCEL);
		}else{
#if 0
			// �^�b�`�p�l���`�F�b�N
			int result = WorldTrade_SubLcdObjHitCheck( wk->SearchResult );
			if(wk->SubLcdTouchOK){
				if(result>=0){
					GFL_CLACT_WK_SetAnmSeq( wk->SubActWork[result+1], 16+result*4 );
					wk->subprocess_seq     = SUBSEQ_END; 
					WorldTrade_SubProcessChange( wk, WORLDTRADE_PARTNER, 0 );
					wk->TouchTrainerPos = result;
					PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
				}
			}
#endif	
		}
	}
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �����ł��邩�`�F�b�N
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SearchCheck( WORLDTRADE_WORK *wk )
{

	// �|�P���������߂Ă��Ȃ�
	if(wk->Search.characterNo==0){
		SubSeq_MessagePrint( wk, msg_gtc_01_013, 1, 0, 0x0f0f );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
		//PMSND_PlaySE(SE_GTC_NG);
	}else{
	// �ȑO�̏����ƈႤ�Ȃ�

		if(DpwSerachCompare( &wk->Search, &wk->SearchBackup, wk->country_code, wk->SearchBackup_CountryCode)){
			// �������ς���Ă��Ȃ��̂Ń_���u�����Ȃ��݂����c�v
			SubSeq_MessagePrint( wk, msg_gtc_01_034, 1, 0, 0x0f0f );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
			//PMSND_PlaySE(SE_GTC_NG);
		}else{
			PMSND_PlaySE(SE_GTC_SEARCH);
      wk->search_se_sync  = 118;

			OS_TPrintf("search start\n");
			OS_Printf( "SearchData  No = %d,  gender = %d, level min= %d max = %d, country = %d\n", wk->Search.characterNo, wk->Search.gender, wk->Search.level_min, wk->Search.level_max, wk->country_code);

			// �������܁@���񂳂����イ�c
			SubSeq_MessagePrint( wk, msg_gtc_01_014, 1, 0, 0x0f0f );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_SERVER_QUERY );

			// �\������Ă��錟�����ʂn�a�i������ꍇ�͉B��
      if( wk->SearchResult > 0 )
      {
        WorldTrade_SubLcdMatchObjHide( wk );
      }
			
		}
		
	}
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �ŏI�������������`�F�b�N
 *
 * @param   wk		
 * @param   trade_type		���������^�C�v(TRADE_TYPE_SEARCH or TRADE_TYPE_DEPOSIT)
 *
 * @retval  int		������������������Ȃ�1,�Ⴄ�Ȃ�0
 */
//------------------------------------------------------------------
static int LastTradeDateCheck( WORLDTRADE_WORK *wk, int trade_type )
{
	GFDATE gfDate;
	RTCDate sdkDate, todayDate;
	RTCTime time;
	int diff;
	
	// �Z�[�u����Ă�����t��SDK�̓��t���ɕϊ�
	if(trade_type == TRADE_TYPE_SEARCH){
		gfDate = WorldTradeData_GetLastDate_Search( wk->param->worldtrade_data );
		OS_TPrintf("���������� ");
	}
	else{
		gfDate = WorldTradeData_GetLastDate_Deposit( wk->param->worldtrade_data );
		OS_TPrintf("�a�������� ");
	}
	GFDATE_GFDate2RTCDate( gfDate, &sdkDate );
	
	// ���݃T�[�o�[�������擾
	DWC_GetDateTime( &todayDate, &time);

	diff = RTC_ConvertDateToDay( &todayDate )-RTC_ConvertDateToDay( &sdkDate );
	if(diff>=0 && diff<3){
		OS_Printf("����-�����ŏI�� = %d�@�Ȃ̂Ł{�Q\n",diff);
		return 1;
	}

	return 0;
}

//------------------------------------------------------------------
/**
 * @brief   �|�P���������J�n
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_ServerQuery(WORLDTRADE_WORK *wk )
{
	Dpw_Tr_PokemonSearchData search;
	int  search_max = 3;				// �����������͂R

	// ���݃T�[�o�[�Ƀ|�P������a���Ă���Ȃ�΁A�ő匟�������Q�C������
#if 0	//�����ύX
	if(wk->DepositFlag){
		search_max += 2;
		OS_Printf( "�a���Ă���̂�+2\n");
	}
#endif

#ifdef WB_SEARCH_NUM_FLAG
  search_max  = 7;
#else

	if(LastTradeDateCheck(wk, TRADE_TYPE_SEARCH)){
		search_max += 2;
	}
	if(LastTradeDateCheck(wk, TRADE_TYPE_DEPOSIT)){
		search_max += 2;
	}
#endif

#ifdef PM_DEBUG
	if(GFL_UI_KEY_GetCont() & PAD_BUTTON_L){
		search_max = 7;
	}
#endif

	// �����J�n	
	if(wk->country_code == 0){
		Dpw_Tr_DownloadMatchDataAsync (&wk->Search, search_max, wk->DownloadPokemonData );
	}
	else{
		Dpw_Tr_PokemonSearchDataEx exs;

		MI_CpuClear8(&exs, sizeof(Dpw_Tr_PokemonSearchDataEx));
		exs.characterNo = wk->Search.characterNo;
		exs.gender = wk->Search.gender;
		exs.level_min = wk->Search.level_min;
		exs.level_max = wk->Search.level_max;
		exs.padding = wk->Search.padding;
		exs.maxNum = search_max;
		exs.countryCode = wk->country_code;

		Dpw_Tr_DownloadMatchDataExAsync(&exs, wk->DownloadPokemonData);
	}
	wk->SearchBackup = wk->Search;
	wk->SearchBackup_CountryCode = wk->country_code;
	
	OS_Printf("�����ő吔 = %d\n", search_max);
	
	wk->timeout_count = 0;
	wk->subprocess_seq = SUBSEQ_SERVER_RESULT;
	
	// ����ʂ��^�b�`���Ă��������Ȃ�
	wk->SubLcdTouchOK = 0;
	
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �T�[�o�[���������҂�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_ServerResult( WORLDTRADE_WORK *wk )
{
	int result;
	result=Dpw_Tr_IsAsyncEnd();
	if (result){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch (result){
		case 0:		case 1:		case 2:		case 3:		case 4:		
		case 5:		case 6:		case 7:
			OS_TPrintf(" query is success!\n");

			// �����̌��ʂ̐���ۑ�
			wk->SearchResult   = result;

			wk->subprocess_seq = SUBSEQ_SERVER_SE_WAIT;
			break;
			


	// -----------------------------------------
	// ���ʃG���[����
	// -----------------------------------------

		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
      PMSND_StopSE();

			// �T�[�o�[�ƒʐM�ł��܂��񁨏I��
			wk->subprocess_seq = SUBSEQ_SEARCH_ERROR_DICONNECT_MES1;

      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();
			break;
		case DPW_TR_ERROR_FATAL:			//!< �ʐM�v���I�G���[�B�d���̍ē������K�v�ł�
      PMSND_StopSE();
			// ���ӂ��Ƃ΂�
      WorldTrade_DispCallFatal(wk);
			break;
	// -----------------------------------------

		// �������ʎ擾���s�i�u���Ȃ������v�ł͂Ȃ��j���g���C����
		case DPW_TR_ERROR_FAILURE:
		case DPW_TR_ERROR_CANCEL :
      PMSND_StopSE();
			wk->subprocess_seq = SUBSEQ_SERVER_QUERY_FAILURE;
			break;
		default:
			OS_TPrintf(" dpw result = %d.\n",result);
			break;
		}
		
	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      PMSND_StopSE();
      WorldTrade_DispCallFatal(wk);
		}
	}
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �T�[�o�[����SE���҂�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_ServerSeWait( WORLDTRADE_WORK *wk )
{
  if( wk->search_se_sync == 0 )
  { 
    PMSND_StopSE();

    // ����ʂɂn�a�i�𔽉f������
    WorldTrade_SubLcdMatchObjAppear( wk, wk->SearchResult, 1 );

    if(wk->SearchResult==0){
      FriendViewButtonPrint(  wk->InfoWin[8], wk->MsgManager, FALSE, &wk->print );
    }else{
      FriendViewButtonPrint(  wk->InfoWin[8], wk->MsgManager, TRUE, &wk->print );
    }

    wk->subprocess_seq = SUBSEQ_SEARCH_RESULT_MESSAGE;
  }

  return SEQ_MAIN;
}
//------------------------------------------------------------------
/**
 * @brief   �������ʕ\�����K�v�Ȏ��͕\��
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SearchResultMessage( WORLDTRADE_WORK *wk )
{
	if(wk->SearchResult==0){
		// �q�b�g����������
		SubSeq_MessagePrint( wk, msg_gtc_01_015, 1, 0, 0x0f0f );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
		PMSND_PlaySE(SE_GTC_NG);
		
	}else{
		// �d�l�ł͌������ʂ���̎��̓��b�Z�[�W��������������
		// �v�]�������̂Łu�݂���܂����I�v�ƕ\��
		SubSeq_MessagePrint( wk, msg_gtc_01_031, 1, 0, 0x0f0f );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_SEARCH_RESULT_MESSAGE_WAIT );
		wk->wait = 0;
	}

	return SEQ_MAIN;

}

//------------------------------------------------------------------
/**
 * @brief   �u�݂���܂����I�v��1�b�҂�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SearchResultMessageWait( WORLDTRADE_WORK *wk )
{
	wk->wait++;
	if(wk->wait>45){
		SubSeq_MessagePrint( wk, msg_gtc_01_033, 1, 0, 0x0f0f );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
		wk->SubLcdTouchOK = 1;
	}
	
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �������̂Ɏ��s���Ă���i�T�[�o�[����̕Ԏ����^�C���A�E�g�����j
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_ServerQueryFailure( WORLDTRADE_WORK *wk )
{
	// �������s�Ȃ̂ŁA�����Ă�����x�����������ł���悤�ɂ���
	wk->SearchBackup.characterNo = 0;

	// �u�f�s�r�̂����ɂ�ɂ����ς����܂����v
	SubSeq_MessagePrint( wk, msg_gtc_01_039, 1, 0, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
	PMSND_PlaySE(SE_GTC_NG);

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �����G���[�i�f�s�r�I���j���̂P
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SearchErrorDisconnectMessage1( WORLDTRADE_WORK *wk )
{
	// �u�f�s�r�Ƃ̂�����������܂����B�������ɂ��ǂ�܂��v
	SubSeq_MessagePrint( wk, msg_gtc_error_006_01, 4, 0, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT_TRG, SUBSEQ_SEARCH_ERROR_DICONNECT_MES2 );
	wk->wait =0;
	PMSND_PlaySE(SE_GTC_NG);

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �����G���[�i�f�s�r�I���j���̂Q
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SearchErrorDisconnectMessage2( WORLDTRADE_WORK *wk )
{
	// �������ɂ��ǂ�܂�
	SubSeq_MessagePrint( wk, msg_gtc_error_006_02, 4, 0, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT_TRG, SUBSEQ_END_ERR );
	WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, MODE_WIFILOGIN_ERR );
	PMSND_PlaySE(SE_GTC_NG);

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �J�[�\���̈ʒu�ԍ����擾
 *
 * @param   wk		
 *
 * @retval  static		
 */
//------------------------------------------------------------------
static int CursorPosGet( WORLDTRADE_WORK *wk )
{
	int pos;
	// ���݂̃J�[�\���̈ʒu���擾
	if(wk->dw->cursorSide==0){
		pos = wk->dw->leftCursorPos;
	}else{
		pos = wk->dw->rightCursorPos+4;
	}

  GF_ASSERT_HEAVY(  pos < 7 );
	return pos;
}

//------------------------------------------------------------------
/**
 * @brief   �J�[�\���ړ�
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void CursorMove( WORLDTRADE_WORK *wk )
{
	int pos ;
	if(GFL_UI_KEY_GetTrg() & PAD_KEY_UP){
		if(wk->dw->cursorSide==0){
			if(wk->dw->leftCursorPos>0){
				wk->dw->leftCursorPos--;
				PMSND_PlaySE(WORLDTRADE_MOVE_SE);

        GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][2] );
			}
		}else{
			if(wk->dw->rightCursorPos>0){
				PMSND_PlaySE(WORLDTRADE_MOVE_SE);
				wk->dw->rightCursorPos--;

        GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][2] );
			}
		}
		
	}else if(GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN){
    if(wk->dw->cursorSide==0){
      if(wk->dw->leftCursorPos<3){
        wk->dw->leftCursorPos++;
				PMSND_PlaySE(WORLDTRADE_MOVE_SE);

        GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][2] );
			}
		}else{
			if(wk->dw->rightCursorPos<2){
				PMSND_PlaySE(WORLDTRADE_MOVE_SE);
				wk->dw->rightCursorPos++;

        GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][2] );
			}
		}
		
	}else if(GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT){
		if(wk->dw->cursorSide != 1){
      wk->dw->cursorSide = 1;
			PMSND_PlaySE(WORLDTRADE_MOVE_SE);
      GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][2] );
		}
    else
    { 
      wk->dw->cursorSide = 1;
    }
	}else if(GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT){
		if(wk->dw->cursorSide != 0){
      wk->dw->cursorSide = 0;
			PMSND_PlaySE(WORLDTRADE_MOVE_SE);
      GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][2] );
		}
    else
    { 
      wk->dw->cursorSide = 0;
    }
	}
	
///	OS_Printf("cursor side = %d, left=%d, right=%d\n", wk->dw->cursorSide, wk->dw->leftCursorPos, wk->dw->rightCursorPos);


	// �J�[�\���ړ�
	WorldTrade_CLACT_PosChange( wk->CursorActWork,  
								CursorPos[CursorPosGet( wk )][0], 
								CursorPos[CursorPosGet( wk )][1] );


}

static u8 Touch2CursorPos[][2]={
	{ 0, 0, },
	{ 0, 1, },
	{ 0, 2, },
	{ 0, 3, },
	{ 1, 0, },
	{ 1, 1, },
	{ 1, 2, },
};

//------------------------------------------------------------------
/**
 * @brief   �^�b�`�I���̍ۂɃJ�[�\���̈ʒu��ύX����
 *
 * @param   wk		
 * @param   touch		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void TouchCursorMove( WORLDTRADE_WORK *wk, int touch )
{
	if(Touch2CursorPos[touch][0]==0){
		wk->dw->cursorSide    = 0;
		wk->dw->leftCursorPos = Touch2CursorPos[touch][1];
	}else{
		wk->dw->cursorSide    = 1;
		wk->dw->rightCursorPos = Touch2CursorPos[touch][1];
	
	}

	// �J�[�\���ړ�
	WorldTrade_CLACT_PosChange( wk->CursorActWork,  
								CursorPos[CursorPosGet( wk )][0], 
								CursorPos[CursorPosGet( wk )][1] );
//	GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][2] );
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
	if(wk->DrawOffset==0){
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
	}

#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_search.c [1076] M ********************\n" );
#endif
	wk->subprocess_seq = 0;
	
	return SEQ_FADEOUT;
}

//------------------------------------------------------------------
/**
 * @brief   �T�u�v���Z�X�V�[�P���X�I������  �G���[�p
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static  int SubSeq_End_Err( WORLDTRADE_WORK *wk)
{
  OS_TPrintf( "DPW_TR���I�����܂���\n" );
  Dpw_Tr_End();

	// �K�����ԃA�C�R��������(�Q�d����΍�͂��Ă����j
	WorldTrade_TimeIconDel(wk);

  WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
    wk->subprocess_seq = 0;
	
	return SEQ_FADEOUT;
}


//------------------------------------------------------------------
/**
 * @brief   �|�P�����̖��O����͂��Ă�������
 *
 * @param   wk		
 *
 * @retval  int 			
 */
//------------------------------------------------------------------
static int 	SubSeq_InputPokenameMessage( WORLDTRADE_WORK *wk)
{
	int i;
	SubSeq_MessagePrint( wk, msg_gtc_01_010, 1, 0, 0x0f0f );
//	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_HEADWORD_SELECT_LIST );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_POKENAME_SELECT_LIST );

	return SEQ_MAIN;
}










//------------------------------------------------------------------
/**
 * @brief   �|�P�������I���J�n
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SUBSEQ_PokenameSelectList( WORLDTRADE_WORK *wk)
{
	// �|�P���������X�g�쐬
#if 0
	wk->BmpListWork = WorldTrade_PokeNameListMake( wk,
							&wk->BmpMenuList, &wk->MenuWin[1], 
							wk->MsgManager, wk->MonsNameManager,wk->dw,
							wk->param->zukanwork );
	wk->listpos = 0xffff;
#endif

	// �������̓V�X�e���J�n
	WorldTrade_Input_Start( wk->WInputWork, MODE_POKEMON_NAME );
	BgBlendSet( PASSIVE_SCREEN_RATE );

	wk->subprocess_seq = SUBSEQ_POKENAME_SELECT_WAIT;

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ���O����
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SUBSEQ_PokenameSelectWait( WORLDTRADE_WORK *wk)
{
	u32 result;
//	switch((result=WorldTrade_BmpMenuList_Main( wk->BmpListWork, &wk->listpos ))){
	switch((result=WorldTrade_Input_Main( wk->WInputWork ))){

	case BMPMENULIST_NULL:
		break;

	// �L�����Z��
	case BMPMENU_CANCEL:
//		BmpListExit( wk->BmpListWork, &wk->dw->nameListPos, &wk->dw->namePos );
//		BmpMenuWork_ListDelete( wk->BmpMenuList );
//		BmpMenuWinClear( &wk->MenuWin[1], WINDOW_TRANS_ON );
		BgBlendSet( 0 );
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
		wk->subprocess_seq  = SUBSEQ_START;
		WorldTrade_SelectNameListBackup( &wk->selectListPos, wk->dw->headwordListPos+wk->dw->headwordPos, 
										wk->dw->nameListPos, wk->dw->namePos );

		break;
	// �I��
	default:
//		BmpListExit( wk->BmpListWork, &wk->dw->nameListPos, &wk->dw->namePos );
//		BmpMenuWork_ListDelete( wk->BmpMenuList );
//		BmpMenuWinClear( &wk->MenuWin[0], WINDOW_TRANS_ON );
//		BmpMenuWinClear( &wk->MenuWin[1], WINDOW_TRANS_ON );
//		GFL_BMPWIN_Delete( &wk->MenuWin[0] );
//		GFL_BMPWIN_Delete( &wk->MenuWin[1] );
		BgBlendSet( 0 );
		wk->Search.characterNo = result;
		wk->subprocess_seq     = SUBSEQ_START;
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

		// ���O����
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin[1]), 0x0000 );
		WorldTrade_PokeNamePrint( wk->InfoWin[1], wk->MonsNameManager, 
			result, SEARCH_INFO_PRINT_FLAG, 0, PRINTSYS_LSB_Make(15,2,0), &wk->print  );
		
		// ���肵���|�P�����̐��ʕ��z���擾
		wk->dw->sex_selection = PokePersonalParaGet(result,POKEPER_ID_sex);
		OS_Printf("");

		// �J�[�\���ʒu�ۑ�
		WorldTrade_SelectNameListBackup( &wk->selectListPos, wk->dw->headwordListPos+wk->dw->headwordPos, 
										wk->dw->nameListPos, wk->dw->namePos );

		if(WorldTrade_SexSelectionCheck( &wk->Search, wk->dw->sex_selection )){
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin[3]), 0x0000 );
			WorldTrade_SexPrint( wk->InfoWin[3], wk->MsgManager, wk->Search.gender, 1, 0, SEARCH_INFO_PRINT_FLAG,  PRINTSYS_LSB_Make(15,2,0), &wk->print  );
			
		}

		break;

	}

	return SEQ_MAIN;
}
//------------------------------------------------------------------
/**
 * @brief   ���ʂ�I�����Ă�������
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SUBSEQ_SexSelectMes( WORLDTRADE_WORK *wk)
{
	// �|�P�����̐��ʂ�I��ł�������
	SubSeq_MessagePrint( wk, msg_gtc_01_011, 1, 0, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_SEX_SELECT_LIST );

	// ���ʑI���E�C���h�E�m��
//	GFL_BMPWIN_Create( &wk->MenuWin[0], GFL_BG_FRAME0_M,
//		SELECT_MENU3_X,		SELECT_MENU3_Y,		SELECT_MENU3_SX, 		SELECT_MENU3_SY, 
//		WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );
//	GFL_BMP_Clear( &wk->MenuWin[0], 0x0000 );

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ���ʑI���J�n
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SUBSEQ_SexSelectList( WORLDTRADE_WORK *wk)
{
//	wk->BmpListWork = WorldTrade_SexSelectListMake( &wk->BmpMenuList, &wk->MenuWin[0], wk->MsgManager );
//	wk->listpos = 0xffff;

	// �������̓V�X�e���J�n
	WorldTrade_Input_Start( wk->WInputWork, MODE_SEX );
	BgBlendSet( PASSIVE_SCREEN_RATE );

	wk->subprocess_seq = SUBSEQ_SEX_SELECT_WAIT;
	
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ���ʑI��҂�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SUBSEQ_SexSelectWait( WORLDTRADE_WORK *wk)
{
	u32 result;
//	switch((result=WorldTrade_BmpMenuList_Main( wk->BmpListWork, &wk->listpos ))){
	switch((result=WorldTrade_Input_Main ( wk->WInputWork ))){
	// �L�����Z��
	case BMPMENU_CANCEL:
		BgBlendSet( 0 );
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
		GFL_BMPWIN_MakeTransWindow( wk->MsgWin );
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

		wk->subprocess_seq = SUBSEQ_START;
		break;
	// �I��
	case PARA_MALE: case PARA_FEMALE: case PARA_UNK: 
		BgBlendSet( 0 );
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

		wk->Search.gender	= result+1;
		wk->subprocess_seq  = SUBSEQ_START;

		// ���ʌ���
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin[3]), 0x0000 );
		WorldTrade_SexPrint( wk->InfoWin[3], wk->MsgManager, wk->Search.gender, 
									1, 0, SEARCH_INFO_PRINT_FLAG, PRINTSYS_LSB_Make(15,2,0), &wk->print  );
		break;
	}


	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �ق������x����I�����Ă�������
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SUBSEQ_LevelSelectMes( WORLDTRADE_WORK *wk)
{
	// �|�P�����̃��x�������߂Ă�������
	SubSeq_MessagePrint( wk, msg_gtc_01_012, 1, 0, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_LEVEL_SELECT_LIST );

	// ���ʑI���E�C���h�E�m��
//	GFL_BMPWIN_Create( &wk->MenuWin[0], GFL_BG_FRAME0_M,
//		SELECT_MENU4_X,		SELECT_MENU4_Y,		SELECT_MENU4_SX, 		SELECT_MENU4_SY, 
//		WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );
//	GFL_BMP_Clear( &wk->MenuWin[0], 0x0000 );

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ���x���I���J�n
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SUBSEQ_LevelSelectList( WORLDTRADE_WORK *wk)
{
	// ���x���I�����X�g�쐬
//	wk->BmpListWork = WorldTrade_LevelListMake(&wk->BmpMenuList, &wk->MenuWin[0], wk->MsgManager,
//		LEVEL_PRINT_TBL_SEARCH );
	wk->listpos = 0xffff;

	// �������̓V�X�e���J�n
	WorldTrade_Input_Start( wk->WInputWork, MODE_LEVEL );
	BgBlendSet( PASSIVE_SCREEN_RATE );
	

	wk->subprocess_seq = SUBSEQ_LEVEL_SELECT_WAIT;

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   ���x���I��҂�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SUBSEQ_LevelSelectWait( WORLDTRADE_WORK *wk)
{
	u32 result;
//	switch((result=WorldTrade_BmpMenuList_Main( wk->BmpListWork, &wk->listpos ))){
	switch((result=WorldTrade_Input_Main( wk->WInputWork ))){
	case BMPMENULIST_NULL:
		break;
	// �L�����Z��
	case BMPMENU_CANCEL:
	case 11:
		BgBlendSet( 0 );
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

		wk->subprocess_seq = SUBSEQ_START;
		break;
	// ���x���w�茈��
	default: 
		BgBlendSet( 0 );
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

		WorldTrade_LevelMinMaxSet(&wk->Search, result, LEVEL_PRINT_TBL_SEARCH);
		wk->subprocess_seq  = SUBSEQ_START;

		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin[5]), 0x0000 );
		// ���x���w�茈��
		WorldTrade_WantLevelPrint( wk->InfoWin[5], wk->MsgManager, result, SEARCH_INFO_PRINT_FLAG, 0, PRINTSYS_LSB_Make(15,2,0), LEVEL_PRINT_TBL_SEARCH, &wk->print );

		break;
	}


	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   �ق�������I�����Ă�������
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SUBSEQ_CountrySelectMes( WORLDTRADE_WORK *wk)
{
	// �������߂Ă�������
	SubSeq_MessagePrint( wk, msg_gtc_country_000, 1, 0, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_COUNTRY_SELECT_LIST );

	// ���I���E�C���h�E�m��
//	GFL_BMPWIN_Create( &wk->MenuWin[0], GFL_BG_FRAME0_M,
//		SELECT_MENU5_X,		SELECT_MENU5_Y,		SELECT_MENU5_SX, 		SELECT_MENU5_SY, 
//		WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B);
//	GFL_BMP_Clear( &wk->MenuWin[0], 0x0000 );

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ���I���J�n
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SUBSEQ_CountrySelectList( WORLDTRADE_WORK *wk)
{
	// ���I�����X�g�쐬
//	wk->BmpListWork = WorldTrade_CountryListMake(&wk->BmpMenuList, &wk->MenuWin[0], 
//		wk->CountryNameManager, wk->MsgManager);
	wk->listpos = 0xffff;

	// �������̓V�X�e���J�n
	WorldTrade_Input_Start( wk->WInputWork, MODE_NATION );
	BgBlendSet( PASSIVE_SCREEN_RATE );

	wk->subprocess_seq = SUBSEQ_COUNTRY_SELECT_WAIT;

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   ���I��҂�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
///���I���E�B���h�E�Łu���ǂ�v�̃��X�g�ʒu
#define COUNTRY_SELECT_WIN_CANCEL	(CountryListTblNum + 1)	//(country233 + 1)

static int SUBSEQ_CountrySelectWait( WORLDTRADE_WORK *wk)
{
	u32 result;
	
//	result=WorldTrade_BmpMenuList_Main( wk->BmpListWork, &wk->listpos );
	result=WorldTrade_Input_Main( wk->WInputWork );
	if(result == BMPMENULIST_NULL){
		;
	}
	else if(result == BMPMENU_CANCEL || result == COUNTRY_SELECT_WIN_CANCEL){
		// �L�����Z��
		BgBlendSet( 0 );
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );
		GFL_BMPWIN_MakeTransWindow( wk->MsgWin );
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

		wk->subprocess_seq = SUBSEQ_START;
	}
	else{
		// ���w�茈��
		BgBlendSet( 0 );
//		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);

		OS_Printf("input -> country = %d\n", result);
		WorldTrade_CountryCodeSet(wk, result);
		wk->subprocess_seq  = SUBSEQ_START;

		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->CountryWin[1]), 0x0000 );
		// ���w�茈��
		WorldTrade_CountryPrint( wk->CountryWin[1], wk->CountryNameManager, wk->MsgManager,
			wk->country_code, SEARCH_INFO_PRINT_FLAG, 0, PRINTSYS_LSB_Make(15,2,0), &wk->print );
	}

	return SEQ_MAIN;
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
	WorldTrade_TouchWinYesNoMake(  wk, WORLDTRADE_YESNO_PY1, YESNO_OFFSET, 3, 1 );
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

	if(ret==WORLDTRADE_RET_YES){
		// �^�C�g�����j���[��
    WorldTrade_TouchDelete( wk );
		wk->subprocess_seq  = SUBSEQ_END;
		WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );

		// ����ʂ�OBJ���B��
    if( wk->SearchResult > 0 )
    { 
      WorldTrade_SubLcdMatchObjHide( wk );
    }

		// �������ʐl���̓N���A����
		wk->SearchResult = 0;
	}else if(ret==WORLDTRADE_RET_NO){
		// �������������g���C
    WorldTrade_TouchDelete( wk );
		wk->subprocess_seq = SUBSEQ_START;
	}

	return SEQ_MAIN;
	
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
//	ChangePage( wk );
	wk->subprocess_seq = SUBSEQ_MAIN;

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �����G���[���b�Z�[�W�\��
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SearchErrorMessage(WORLDTRADE_WORK *wk)
{
	SubSeq_MessagePrint( wk, msg_gtc_error_003, 1, 0, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
	
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
//----------------------------------------------------------------------------
/**
 *	@brief  ��b�I�����^�b�`�҂������̃V�[�P���X��
 *
 *	@param	WORLDTRADE_WORK *wk 
 *
 *	@return int 
 */
//-----------------------------------------------------------------------------
static  int SubSeq_MessageWaitTrg( WORLDTRADE_WORK *wk )
{
	if( GF_MSG_PrintEndCheck( &wk->print )==0){
    if( GFL_UI_KEY_GetTrg() || GFL_UI_TP_GetTrg() )
    {
      wk->subprocess_seq = wk->subprocess_nextseq;
    }
	}
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ���b�Z�[�W�I����P�b�҂�
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_MessageWait1Min( WORLDTRADE_WORK *wk )
{
	if( GF_MSG_PrintEndCheck( &wk->print )==0){
		wk->wait++;
		if(wk->wait>45){
			wk->wait = 0;
			wk->subprocess_seq = wk->subprocess_nextseq;
		}
	}
	return SEQ_MAIN;


}


//------------------------------------------------------------------
/**
 * @brief   ����̃|�P�����������ʂɈڍs����
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_ExchangeScreen1( WORLDTRADE_WORK *wk )
{
	// �t�F�[�h�҂��Ɠ����ɉ�ʃI�t�Z�b�g�ړ�
	wk->DrawOffset++;
	if(WIPE_SYS_EndCheck()){
		GX_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );
		wk->DrawOffset = -16;
//		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, 
//						WIPE_FADE_BLACK, EXCHANGE_SCREEN_SYNC, 1, HEAPID_WORLDTRADE );

		if(wk->SubLcdTouchOK){
			// �K���������w�肷��i���O�j
			int result=0;
				//GFL_CLACT_WK_SetAnmSeq( wk->SubActWork[result+1], 16+result*4 );
				wk->subprocess_seq     = SUBSEQ_END; 
				WorldTrade_SubProcessChange( wk, WORLDTRADE_PARTNER, MODE_FROM_SEARCH );
				wk->TouchTrainerPos = result;
//				PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
		}
		GFL_CLACT_WK_SetDrawEnable( wk->PromptDsActWork, 0 );


//		wk->subprocess_seq = SUBSEQ_EXCHANGE_SCREEN2;


		// ���̐��worldtrade_partner.c�ōs��

	}
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   �|�P���������ʂ���߂��Ă���
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_ReturnScreen2( WORLDTRADE_WORK *wk )
{
	// �t�F�[�h�҂��Ɠ����ɉ�ʃI�t�Z�b�g�ړ�
	wk->DrawOffset--;
	if(WIPE_SYS_EndCheck()){
		wk->DrawOffset = 0;

		wk->subprocess_seq = SUBSEQ_MAIN;
	}
	return SEQ_MAIN;
}


static int SubSeq_ExchangeMain( WORLDTRADE_WORK *wk )
{
	if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
		WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, 
						WIPE_FADE_BLACK, EXCHANGE_SCREEN_SYNC, 1, HEAPID_WORLDTRADE );
//		wk->subprocess_seq = SUBSEQ_RETURN_SCREEN1;
	}else{
		// �^�b�`�p�l���`�F�b�N
		int result = WorldTrade_SubLcdObjHitCheck( wk->SearchResult );
		if(wk->SubLcdTouchOK){
			if(result>=0){
				GFL_CLACT_WK_SetAnmSeq( wk->SubActWork[result+1], 16+result*4 );
				wk->subprocess_seq     = SUBSEQ_END; 
				WorldTrade_SubProcessChange( wk, WORLDTRADE_PARTNER, 0 );
				wk->TouchTrainerPos = result;
				PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
			}
		}
	
	}
	return SEQ_MAIN;
}


//----------------------------------------------------------------------------
/**
 *	@brief  ����҂�
 *
 *	@param	WORLDTRADE_WORK *wk   ���[�N
 *
 *	@return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static  int SubSeq_DecideWait( WORLDTRADE_WORK *wk )
{ 
  if( !GFL_CLACT_WK_CheckAnmActive( wk->CursorActWork) )
  { 
    GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][2] );
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
    { 
      GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 0 );
    }
    else
    { 
      GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
    }

    DecideFunc( wk, CursorPosGet( wk ) );
  }

  return SEQ_MAIN;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �L�����Z���҂�
 *
 *	@param	WORLDTRADE_WORK *wk   ���[�N
 *
 *	@return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static  int SubSeq_CancelWait( WORLDTRADE_WORK *wk )
{ 
  if( !GFL_CLACT_WK_CheckAnmActive( wk->CursorActWork) )
  { 
    GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][2] );
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
    { 
      GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 0 );
    }
    else
    { 
      GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
    }


    SubSeq_MessagePrint( wk, msg_gtc_01_016, 1, 0, 0x0f0f );
    WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_YESNO );
  }

  return SEQ_MAIN;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �A�j���҂�
 *
 *	@param	WORLDTRADE_WORK *wk   ���[�N
 *
 *	@return �I���R�[�h
 */
//-----------------------------------------------------------------------------
static  int SubSeq_AnmWait( WORLDTRADE_WORK *wk )
{ 

  if( !GFL_CLACT_WK_CheckAnmActive( wk->CursorActWork) )
  { 
    GFL_CLACT_WK_SetAnmSeq( wk->CursorActWork, CursorPos[CursorPosGet( wk )][2] );
    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
    { 
      GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 0 );
    }
    else
    { 
      GFL_CLACT_WK_SetDrawEnable( wk->CursorActWork, 1 );
    }

    wk->subprocess_seq  = SUBSEQ_MAIN;
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
	BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, WORLDTRADE_MESFRAME_CHR, WORLDTRADE_MESFRAME_PAL );

	// ������`��J�n
	GF_STR_PrintSimple( wk->MsgWin, FONT_TALK, wk->TalkString, 0, 0, &wk->print );
	GFL_BMPWIN_MakeTransWindow( wk->MsgWin );


}

//------------------------------------------------------------------
/**
 * @brief   �ق����|�P�������́u���x���v�`��
 *
 * @param   win		
 * @param   MsgManager		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void WantLabelPrint( GFL_BMPWIN *win[], GFL_BMPWIN *country_win[], GFL_MSGDATA *MsgManager, WT_PRINT *print )
{
	STRBUF *strbuf, *levelbuf, *sexbuf;

	//�u�ق����|�P�����v�`��
	strbuf = GFL_MSG_CreateString( MsgManager, msg_gtc_03_002 );
	WorldTrade_SysPrint( win[0], strbuf,    0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	GFL_STR_DeleteBuffer(strbuf);

	// �����ׂ�
	sexbuf = GFL_MSG_CreateString( MsgManager, msg_gtc_03_004  );
	WorldTrade_SysPrint( win[2], sexbuf,    0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	GFL_STR_DeleteBuffer(sexbuf);

	// ���x��
	levelbuf = GFL_MSG_CreateString( MsgManager, msg_gtc_03_006 );
	WorldTrade_SysPrint( win[4], levelbuf,  0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	GFL_STR_DeleteBuffer(levelbuf);

	// ��
	levelbuf = GFL_MSG_CreateString( MsgManager, msg_gtc_search_013 );
	WorldTrade_SysPrint( country_win[0], levelbuf,  0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	GFL_STR_DeleteBuffer(levelbuf);

	//�u�������v�`��
	strbuf = GFL_MSG_CreateString( MsgManager, msg_gtc_03_008 );
	WorldTrade_TouchPrint( win[6], strbuf,    0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	GFL_STR_DeleteBuffer(strbuf);

	//�u���ǂ�v�`��
	strbuf = GFL_MSG_CreateString( MsgManager, msg_gtc_03_011 );
	WorldTrade_TouchPrint( win[7], strbuf,    0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	GFL_STR_DeleteBuffer(strbuf);

}

//------------------------------------------------------------------
/**
 * @brief   �u�����Ă��݂�v�{�^���̕�����`��
 *
 * @param  none
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void FriendViewButtonPrint( GFL_BMPWIN *win, GFL_MSGDATA *MsgManager, int flag, WT_PRINT *print )
{
	u8 pal;
	STRBUF *strbuf;
	PRINTSYS_LSB color = PRINTSYS_LSB_Make(15,2,0);
	if(flag){
		color = PRINTSYS_LSB_Make(1,2,0);
		pal = FRD_VIEW_OK_PAL;
	}else{
		pal = FRD_VIEW_NG_PAL;
	}
	//�X�N���[���p���b�g��������
	GFL_BG_ChangeScreenPalette( GFL_BG_FRAME1_M,
			         	 FRD_VIEW_BTN_PX, FRD_VIEW_BTN_PY, FRD_VIEW_BTN_SX, FRD_VIEW_BTN_SY, pal );
	GFL_BG_LoadScreenReq( GFL_BG_FRAME1_M );
	//�u�����Ă��݂�v�`��
	strbuf = GFL_MSG_CreateString( MsgManager, msg_gtc_03_010 );
	WorldTrade_TouchPrint( win, strbuf,    0, 0, 0, color, print );
	GFL_STR_DeleteBuffer(strbuf);

}

//------------------------------------------------------------------
/**
 * @brief   �����f�[�^���܂�������������r
 *
 * @param   s1		
 * @param   s2		
 *
 * @retval  int		�����Ȃ�P
 */
//------------------------------------------------------------------
static int DpwSerachCompare( const Dpw_Tr_PokemonSearchData *s1, const Dpw_Tr_PokemonSearchData *s2 , int country_code1, int country_code2)
{
	if( s1->characterNo==s2->characterNo && s1->gender==s2->gender
	&&	s1->level_min==s2->level_min     && s1->level_max==s2->level_max
	&&  country_code1 == country_code2){
		return 1;
	}
	
	return 0;
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
	GFL_BG_SetScroll(  GFL_BG_FRAME1_S, GFL_BG_SCROLL_Y_SET, -wk->DrawOffset );
	GFL_BG_SetScroll(  GFL_BG_FRAME2_S, GFL_BG_SCROLL_Y_SET, -wk->DrawOffset );
	GFL_BG_SetScroll(  GFL_BG_FRAME3_S, GFL_BG_SCROLL_Y_SET, -wk->DrawOffset );

}


//------------------------------------------------------------------
/**
 * @brief   BG�P�x�ݒ�
 *
 * @param   rate		�P�x( 0 -31 ) 0�ŋP�x����OFF
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BgBlendSet( int rate  )
{
	if(rate!=0){
		G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG1|GX_BLEND_PLANEMASK_BG3, rate );
	}else{
		G2_BlendNone();
	}
}
