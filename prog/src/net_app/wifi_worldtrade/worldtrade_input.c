//============================================================================================
/**
 * @file	worldtrade_input.c
 * @brief	���E�����������̓E�C���h�E����
 * @author	Akito Mori
 * @date	08.11.01
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
//#include "system/fontproc.h"
//#include "system/fontoam.h"
//#include "system/window.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"

#include "net_app/worldtrade.h"
#include "worldtrade_local.h"

#include "msg/msg_wifi_lobby.h"
#include "msg/msg_wifi_gts.h"
#include "msg/msg_wifi_system.h"

#include "worldtrade.naix"			// �O���t�B�b�N�A�[�J�C�u��`
#include "wifip2pmatch.naix"			// �O���t�B�b�N�A�[�J�C�u��`


static int	wi_seq_none( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_winin( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_winwait( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_winout( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_winoutwait( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_winwait_return( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_head1_init( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_head1_main( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_head1_exit( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_head1_return( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_head2_init( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_head2_main( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_head2_exit( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_head2_return( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_pokename_init( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_pokename_main( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_pokename_cancel_exit( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_pokename_exit( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_nation_head1_init( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_nation_head1_main( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_nation_head1_exit( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_nation_head1_return( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_nation_head2_init( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_nation_head2_main( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_nation_head2_exit( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_nation_head2_return( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_nation_init( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_nation_main( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_nation_cancel_exit( WORLDTRADE_INPUT_WORK *wk );
static int  wi_seq_nation_exit( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_sex_init( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_sex_main( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_sex_exit( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_level_init( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_level_main( WORLDTRADE_INPUT_WORK *wk );
static int	wi_seq_level_exit( WORLDTRADE_INPUT_WORK *wk );

static void select_bmpwin_add( WORLDTRADE_INPUT_WORK *wk, int mode );
static void select_bmpwin_del( WORLDTRADE_INPUT_WORK *wk, int mode );
static void SelectFrameBoxWrite( u16 *scr, int x, int y, int type );
static void SexSelect_Init( WORLDTRADE_INPUT_WORK *wk );
static void SexSelect_Main( WORLDTRADE_INPUT_WORK *wk );
static void LevelSelect_Init( WORLDTRADE_INPUT_WORK *wk );

static void HeadWord1_Init( WORLDTRADE_INPUT_WORK *wk, int type, int x );
static  u32 WordHead_SelectMain( WORLDTRADE_INPUT_WORK *wk, u8 *see_check8 );
static  u32 WordHead2_SelectMain( WORLDTRADE_INPUT_WORK *wk, u8 *see_check );
static  u32 LevelSelect_SelectMain( WORLDTRADE_INPUT_WORK *wk );
static  u32 Nation_SelectMain( WORLDTRADE_INPUT_WORK *wk );
static void system_print( BGWINFRM_WORK *wfwk, GFL_MSGDATA *mm, GFL_BMPWIN *win, int mesno, PRINTSYS_LSB color, WT_PRINT *print );
static void touch_print( BGWINFRM_WORK *wfwk, GFL_MSGDATA *mm, GFL_BMPWIN *win, int mesno, WT_PRINT *print );
static  int PokeName_MakeSortList( BMPLIST_DATA **list, GFL_MSGDATA *monsnameman, 
									GFL_MSGDATA *msgman,u8 *sinou, int select, ZUKAN_SAVEDATA *zukan );
static int PokeName_GetSortNum( u8 *sinou, ZUKAN_SAVEDATA *zukan, int pokenum, u16 *sortlist, int start, int end );
static void PokeName_ListPrint( WORLDTRADE_INPUT_WORK *wk, int page, int max );
static  int _list_page_num( int num, int in_page );
static  int Nation_GetSortListNum( int start, int *number );
static  int Nation_MakeSortList( BMP_MENULIST_DATA **menulist, GFL_MSGDATA *CountryNameManager, int start );
static void Nation_ListPrint( WORLDTRADE_INPUT_WORK *wk, int page, int max );
static void PagePrint( BGWINFRM_WORK *wfwk, NUMFONT *numfont, GFL_BMPWIN *win, int page, int max );
static  u32 TouchPanelFunc( WORLDTRADE_INPUT_WORK *wk, int mode );
static  u32 Head2DecideFunc( WORLDTRADE_INPUT_WORK *wk, int decide );
static  u32 NameDecideFunc( WORLDTRADE_INPUT_WORK *wk, int decide );
static  u32 NationHead1DecideFunc( WORLDTRADE_INPUT_WORK *wk, int decide );
static  u32 NationDecideFunc( WORLDTRADE_INPUT_WORK *wk, int decide );
static void LevelPageRefresh( WORLDTRADE_INPUT_WORK *wk, int move );
static  u32 LevelDecideFunc( WORLDTRADE_INPUT_WORK *wk, u32 decide );
static void NationPageRefresh( WORLDTRADE_INPUT_WORK *wk, int move);
static void NamePageRefresh( WORLDTRADE_INPUT_WORK *wk, int move );

static int PokeSeeCount_Siin(WORLDTRADE_INPUT_WORK *wk, int siin);
static BOOL PokeSeeCount_Category(WORLDTRADE_INPUT_WORK *wk, int select);
static int NationCount_Siin(WORLDTRADE_INPUT_WORK *wk, int siin);
static BOOL NationCount_Category(WORLDTRADE_INPUT_WORK *wk, int select);

///���������F:�m�[�}��
#define SEARCH_MOJI_COLOR_NORMAL       (PRINTSYS_LSB_Make(15,14,2))
///���������F:�D�F
#define SEARCH_MOJI_COLOR_GRAY       (PRINTSYS_LSB_Make(8,9,2))


// �������̓V�[�P���X
enum{
	WI_SEQ_NONE=0,
	WI_SEQ_WININ,
	WI_SEQ_WINWAIT,
	WI_SEQ_WINOUT,
	WI_SEQ_WINOUTWAIT,
	WI_SEQ_WINWAIT_RETURN,

	WI_SEQ_HEAD1_INIT,
	WI_SEQ_HEAD1_MAIN,
	WI_SEQ_HEAD1_EXIT,
	WI_SEQ_HEAD1_RETURN,

	WI_SEQ_HEAD2_INIT,
	WI_SEQ_HEAD2_MAIN,
	WI_SEQ_HEAD2_EXIT,
	WI_SEQ_HEAD2_RETURN,

	WI_SEQ_POKENAME_INIT,
	WI_SEQ_POKENAME_MAIN,
	WI_SEQ_POKENAME_CANCEL_EXIT,
	WI_SEQ_POKENAME_EXIT,
	
	WI_SEQ_NATION_HEAD1_INIT,
	WI_SEQ_NATION_HEAD1_MAIN,
	WI_SEQ_NATION_HEAD1_EXIT,
	WI_SEQ_NATION_HEAD1_RETURN,

	WI_SEQ_NATION_HEAD2_INIT,
	WI_SEQ_NATION_HEAD2_MAIN,
	WI_SEQ_NATION_HEAD2_EXIT,
	WI_SEQ_NATION_HEAD2_RETURN,

	WI_SEQ_NATION_INIT,
	WI_SEQ_NATION_MAIN,
	WI_SEQ_NATION_CANCEL_EXIT,
	WI_SEQ_NATION_EXIT,
	
	WI_SEQ_SEX_INIT,
	WI_SEQ_SEX_MAIN,
	WI_SEQ_SEX_EXIT,

	WI_SEQ_LEVEL_INIT,
	WI_SEQ_LEVEL_MAIN,
	WI_SEQ_LEVEL_EXIT,

	WI_SEQ_MAX,
};


static int (*FuncTable[])( WORLDTRADE_INPUT_WORK *wk ) = {
	wi_seq_none,
	wi_seq_winin,
	wi_seq_winwait,
	wi_seq_winout,
	wi_seq_winoutwait,
  wi_seq_winwait_return,

	wi_seq_head1_init,
	wi_seq_head1_main,
	wi_seq_head1_exit,
	wi_seq_head1_return,

	wi_seq_head2_init,
	wi_seq_head2_main,
	wi_seq_head2_exit,
	wi_seq_head2_return,

	wi_seq_pokename_init,
	wi_seq_pokename_main,
	wi_seq_pokename_cancel_exit,
	wi_seq_pokename_exit,
	
	wi_seq_nation_head1_init,
	wi_seq_nation_head1_main,
	wi_seq_nation_head1_exit,
	wi_seq_nation_head1_return,

	wi_seq_nation_head2_init,
	wi_seq_nation_head2_main,
	wi_seq_nation_head2_exit,
	wi_seq_nation_head2_return,

	wi_seq_nation_init,
	wi_seq_nation_main,
	wi_seq_nation_cancel_exit,
	wi_seq_nation_exit,
	
	wi_seq_sex_init,
	wi_seq_sex_main,
	wi_seq_sex_exit,

	wi_seq_level_init,
	wi_seq_level_main,
	wi_seq_level_exit,

};

// �T�O�����̓������P�����ڒ�`
enum{
	INPUT_HEAD_A  =0,
	INPUT_HEAD_KA,
	INPUT_HEAD_SA,
	INPUT_HEAD_TA,
	INPUT_HEAD_NA,
	INPUT_HEAD_HA,
	INPUT_HEAD_MA,
	INPUT_HEAD_YA,
	INPUT_HEAD_RA,
	INPUT_HEAD_WA,
	INPUT_HEAD_CANCEL,	// ���g�p�F�u��߂�v�̈ʒu
	INPUT_HEAD_NONE,	// �u���ɂ��Ȃ��v
};


// �������I�����s���ۂ�
// �������������W
static const u8 word_table[][2]={
	{  1,  5 },	// �P���
	{  4,  5 },	
	{  7,  5 },
	{ 10,  5 },
	{ 13,  5 },

	{  1,  8 },	// �Q���
	{  4,  8 },
	{  7,  8 },
	{ 10,  8 },
	{ 13,  8 },
	
	{  9,  17 },// ���ǂ�
	{  1,   2 },// ���������͂Q�y�[�W�ڂ́i�A�C�E�G�́u�A�v�j�̈ʒu
	{  1,   2 },// ���I���̎��́u���ɂ��Ȃ��v
};

// �������I�����s���ۂ�
// �J�[�\���ʒu���W
static const u8 word_cur_table[][2]={
	{  1,  5 },	// �P���
	{  4,  5 },	
	{  7,  5 },
	{ 10,  5 },
	{ 13,  5 },

	{  1,  8 },	// �Q���
	{  4,  8 },
	{  7,  8 },
	{ 10,  8 },
	{ 13,  8 },
	
	{  8,  17 },// ���ǂ�
	{  1,   2 },// ���������͂Q�y�[�W�ڂ́i�A�C�E�G�́u�A�v�j�̈ʒu
	{  1,   2 },// ���I���̎��́u���ɂ��Ȃ��v
};



static const u8 n_word_table[][2]={
	{  1,  5 },	// �P���
	{  4,  5 },	
	{  7,  5 },
	{ 10,  5 },
	{ 13,  5 },

	{  1,  8 },	// �Q���
	{  4,  8 },
	{  7,  8 },
	{ 10,  8 },
	{ 13,  8 },
	
	{  8,  17 },// ���ǂ�
	{  8,   3 },// ���I���̎��́u���ɂ��Ȃ��v
};

// �|�P�������\��BMPWIN�ʒu
static const u8 name_table[][3]={
	{ 4,    2 },
	{ 4,    5 },
	{ 4,    8 },
	{ 4,   11 },

};


// ���ʑI��BMPWIN�\���ʒu
static const u8 sexselect_table[][2]={
	{ 3,  3 },
	{ 3,  7 },
	{ 3, 11 },
};

// ���x���I��BMPWIN�\���ʒu
static const u8 levelselect_table[][2]={
	{ 3,  2 },
	{ 3,  5 },
	{ 3,  8 },
	{ 3, 11 },

};


static const u8 nation_table[][2]={
	{  3,  1, },
	{  3,  4, },
	{  3,  7, },
	{  3, 10, },
	{  3, 13, },
	{ 24, 17  },	// ���ǂ�
};

#define INPUT_LETTER_W		(  2 )	// 1�����\��W
#define INPUT_LETTER_H		(  2 )	// 1�����\��H
#define INPUT_BACK_W		(  6 )	// ���ǂ�W
#define INPUT_BACK_H		(  2 )	// ���ǂ�H
#define INPUT_POKENAME_W	(  8 )	// �|�P������W
#define INPUT_POKENAME_H	(  2 )	// �|�P������H
#define INPUT_NAME_PAGE_W	(  5 )	// ���O�\����ԃy�[�W��W
#define INPUT_NAME_PAGE_H	(  1 )	// ���O�\����ԃy�[�W��H
#define INPUT_SEXSELECT_W	(  8 )	// ���ʑI��W
#define INPUT_SEXSELECT_H	(  2 )	// ���ʑI��H
#define INPUT_LEVELSELECT_W	(  11 )	// ���x�������I��W
#define INPUT_LEVELSELECT_H	(  2 )	// ���x�������I��H
#define INPUT_NONE_W		(  9 )	// �u���ɂ��Ȃ��vW
#define INPUT_NONE_H		(  2 )	// �u���ɂ��Ȃ��vH
#define INPUT_NATION_W		( 23 )	// ����W
#define INPUT_NATION_H		(  2 )	// ����H

#define INPUT_NATION_NONE_SELECT_X ( 1 )	// �����u���ɂ��Ȃ��v�ʒuX
#define INPUT_NATION_NONE_SELECT_Y ( 2 )	// �����u���ɂ��Ȃ��v�ʒuY

#define INPUT_PAGE_X		(  5 )	// ���x���ƃ|�P�������̃y�[�W�\�����W
#define INPUT_PAGE_Y		( 14 )

#define INPUT_NATION_PAGE_X			( 14 )	// �����y�[�W�ʒuX
#define INPUT_NATION_PAGE_Y			( 16 )	// �����y�[�W�ʒuY

#define HEAD1_MOVE_INIT_X 		( 32 )
#define HEAD1_REWRITE_INIT_X 	( 32-4*4 )

#define PAGE_ARROW1_X			( 154 )
#define PAGE_ARROW2_X			( 228 )
#define PAGE_ARROW_Y			( 120 )
#define NATION_PAGE_ARROW1_X	(  88 )
#define NATION_PAGE_ARROW2_X	( 176 )
#define NATION_PAGE_ARROW_Y		( 136 )
#define POKE_ARROW1_X			( 154 )
#define POKE_ARROW2_X			( 228 )
#define POKE_ARROW_Y			( 120 )


// �����\��BMPWIN���N���A����ۂ̔w�i�F�J���[�w��
#define INPUTPLATE_BASE_COLOR_DATA	( 0xA )
#define INPUTPLATE_PAGE_COLOR_DATA	( 0x8 )

#define LV_PAGE_MAX	(2)

//------------------------------------------------------------------
/**
 * @brief   �K�v��BMPWIN���[�N���m�ۂ���
 *
 * @param   wk		
 * @param   mode		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void select_bmpwin_add( WORLDTRADE_INPUT_WORK *wk, int mode )
{
	int i=0;
  GFL_BG_FillCharacter( wk->BgFrame, 0, 1, 0 );

	switch(mode){

	// ���O���͓������P
	case MODE_HEADWORD_1:
		// ���ǂ�
		wk->MenuBmp[BMPWIN_SELECT_END_WIN]	= GFL_BMPWIN_Create( wk->BgFrame,
									word_table[10][0],		word_table[10][1],	
									INPUT_BACK_W, 			INPUT_BACK_H, 
									WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[BMPWIN_SELECT_END_WIN]), INPUTPLATE_BASE_COLOR_DATA );
		//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[BMPWIN_SELECT_END_WIN] );
		// �q��BMPWIN�m��
		for(i=0;i<MODE_HEADWORD1_WIN_NUM;i++){
			wk->MenuBmp[i]	= GFL_BMPWIN_Create( wk->BgFrame,
									   word_table[i][0],		word_table[i][1],	
									   INPUT_LETTER_W, 			INPUT_LETTER_H, 
									   WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
		//	GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[i] );
		}

		break;

	// ���O���͓������Q
	case MODE_HEADWORD_2:
		// ���ǂ�
		wk->MenuBmp[BMPWIN_SELECT_END_WIN]	= GFL_BMPWIN_Create( wk->BgFrame,
									word_table[10][0],		word_table[10][1],	
									INPUT_BACK_W, 			INPUT_BACK_H, 
									WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[BMPWIN_SELECT_END_WIN]), INPUTPLATE_BASE_COLOR_DATA );
		//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[BMPWIN_SELECT_END_WIN] );

		// �ꉹBMPWIN�m��
		wk->MenuBmp[0]	= GFL_BMPWIN_Create( wk->BgFrame,
								   word_table[11][0],		word_table[11][1],	
								   INPUT_LETTER_W, 			INPUT_LETTER_H, 
								   WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[0]), 7 );
		//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[0] );

		for(i=1;i<MODE_HEADWORD2_WIN_NUM;i++){
			wk->MenuBmp[i]	= GFL_BMPWIN_Create( wk->BgFrame,
								word_table[i-1][0],		word_table[i-1][1],	
								INPUT_LETTER_W, 		INPUT_LETTER_H, 
								WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
			//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[i] );
		}
		break;

	// ���O����
	case MODE_POKEMON_NAME:
		// ���ǂ�
		wk->MenuBmp[BMPWIN_SELECT_END_WIN]	= GFL_BMPWIN_Create( wk->BgFrame,
									word_table[10][0],		word_table[10][1],	
									INPUT_BACK_W, 			INPUT_BACK_H, 
									WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[BMPWIN_SELECT_END_WIN]), INPUTPLATE_BASE_COLOR_DATA );
		//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[BMPWIN_SELECT_END_WIN] );

		for(i=0;i<MODE_POKENAME_WIN_NUM;i++){
			wk->MenuBmp[i]	= GFL_BMPWIN_Create( wk->BgFrame,
									   name_table[i][0],		name_table[i][1],	
									   INPUT_POKENAME_W, 		INPUT_POKENAME_H, 
									   WORLDTRADE_INPUT_PAL,  	
									   GFL_BMP_CHRAREA_GET_B );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
			//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[i] );
		}

		// �y�[�W���\���pBMP
		wk->MenuBmp[MODE_POKENAME_WIN_NUM]	= GFL_BMPWIN_Create( wk->BgFrame,
									   INPUT_PAGE_X,			INPUT_PAGE_Y,	
									   INPUT_NAME_PAGE_W, 		INPUT_NAME_PAGE_H, 
									   WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[MODE_POKENAME_WIN_NUM]), INPUTPLATE_PAGE_COLOR_DATA  );
		//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[MODE_POKENAME_WIN_NUM] );
		break;
	case MODE_NATION_HEAD1:
		// ���ǂ�
		wk->MenuBmp[BMPWIN_SELECT_END_WIN]	= GFL_BMPWIN_Create( wk->BgFrame,
									word_table[10][0],		word_table[10][1],	
									INPUT_BACK_W, 			INPUT_BACK_H, 
									WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[BMPWIN_SELECT_END_WIN]), INPUTPLATE_BASE_COLOR_DATA );
		//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[BMPWIN_SELECT_END_WIN] );


		// �q��BMPWIN�m��
		for(i=0;i<MODE_NATION_HEADWORD1_WIN_NUM;i++){
			wk->MenuBmp[i]	= GFL_BMPWIN_Create( wk->BgFrame,
									   word_table[i][0],		word_table[i][1],	
									   INPUT_LETTER_W, 			INPUT_LETTER_H, 
									   WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
			//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[i] );
		}


		// �u���ɂ��Ȃ��v
		wk->MenuBmp[BMPWIN_NONE_SELECT_WIN]	= GFL_BMPWIN_Create( wk->BgFrame,
								   INPUT_NATION_NONE_SELECT_X,	INPUT_NATION_NONE_SELECT_Y,	
								   INPUT_NONE_W, 				INPUT_NONE_H, 
								   WORLDTRADE_INPUT_PAL,  		GFL_BMP_CHRAREA_GET_B );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[BMPWIN_NONE_SELECT_WIN]), INPUTPLATE_BASE_COLOR_DATA );
		//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[BMPWIN_NONE_SELECT_WIN] );


		break;
	case MODE_NATION:
		// ���ǂ�
		wk->MenuBmp[BMPWIN_SELECT_END_WIN]	= GFL_BMPWIN_Create( wk->BgFrame,
									nation_table[5][0],		nation_table[5][1],	
									INPUT_BACK_W, 			INPUT_BACK_H, 
									WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[BMPWIN_SELECT_END_WIN]), INPUTPLATE_BASE_COLOR_DATA );
		//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[BMPWIN_SELECT_END_WIN] );

		// ����BMPWIN�m��
		for(i=0;i<MODE_NATION_WIN_NUM;i++){
			wk->MenuBmp[i]	= GFL_BMPWIN_Create( wk->BgFrame,
									   nation_table[i][0],		nation_table[i][1],	
									   INPUT_NATION_W, 			INPUT_NATION_H, 
									   WORLDTRADE_INPUT_PAL,  	
									   GFL_BMP_CHRAREA_GET_B );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
			//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[i] );
		}
		// �y�[�W���\���pBMP
		wk->MenuBmp[MODE_NATION_WIN_NUM]	= GFL_BMPWIN_Create( wk->BgFrame,
									   INPUT_NATION_PAGE_X,		INPUT_NATION_PAGE_Y,	
									   INPUT_NAME_PAGE_W, 		INPUT_NAME_PAGE_H, 
									   WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[MODE_NATION_WIN_NUM]), INPUTPLATE_BASE_COLOR_DATA );
		//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[MODE_NATION_WIN_NUM] );

		break;
	case MODE_SEX:
		// ���ǂ�
		wk->MenuBmp[BMPWIN_SELECT_END_WIN]	= GFL_BMPWIN_Create( wk->BgFrame,
									word_table[10][0],		word_table[10][1],	
									INPUT_BACK_W, 			INPUT_BACK_H, 
									WORLDTRADE_INPUT_PAL,  GFL_BMP_CHRAREA_GET_B );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[BMPWIN_SELECT_END_WIN]), INPUTPLATE_BASE_COLOR_DATA );
		//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[BMPWIN_SELECT_END_WIN] );

		// ���ʑI��
		for(i=0;i<MODE_SEX_WIN_NUM;i++){
			wk->MenuBmp[i]	= GFL_BMPWIN_Create( wk->BgFrame,
									   sexselect_table[i][0],	sexselect_table[i][1],	
									   INPUT_SEXSELECT_W, 		INPUT_SEXSELECT_H, 
									   WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
			//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[i] );
		}

		
		break;
	case MODE_LEVEL:
		// ���ǂ�
		wk->MenuBmp[BMPWIN_SELECT_END_WIN]	= GFL_BMPWIN_Create( wk->BgFrame,
									word_table[10][0],		word_table[10][1],	
									INPUT_BACK_W, 			INPUT_BACK_H, 
									WORLDTRADE_INPUT_PAL,  GFL_BMP_CHRAREA_GET_B );
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[BMPWIN_SELECT_END_WIN]), INPUTPLATE_BASE_COLOR_DATA );
		//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[BMPWIN_SELECT_END_WIN] );

		// ���x���I��
		for(i=0;i<MODE_LEVEL_WIN_NUM;i++){
			wk->MenuBmp[i]	= GFL_BMPWIN_Create( wk->BgFrame,
									   levelselect_table[i][0],	levelselect_table[i][1],	
									   INPUT_LEVELSELECT_W, 	INPUT_LEVELSELECT_H, 
									   WORLDTRADE_INPUT_PAL,  GFL_BMP_CHRAREA_GET_B);
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
			//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[i] );
		}

		// �y�[�W���\���p
		wk->MenuBmp[MODE_LEVEL_WIN_NUM]	= GFL_BMPWIN_Create( wk->BgFrame,
									   INPUT_PAGE_X,			INPUT_PAGE_Y,	
									   INPUT_NAME_PAGE_W, 		INPUT_NAME_PAGE_H, 
									   WORLDTRADE_INPUT_PAL,  	GFL_BMP_CHRAREA_GET_B);
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[MODE_LEVEL_WIN_NUM]), INPUTPLATE_PAGE_COLOR_DATA );
		//GFL_BMPWIN_MakeTransWindow( wk->MenuBmp[MODE_LEVEL_WIN_NUM] );

		break;

	}


}

//------------------------------------------------------------------
/**
 * @brief   ���[�h���Ɋm�ۂ���BMPWIN���[�N���������
 *
 * @param   wk		
 * @param   mode		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void select_bmpwin_del( WORLDTRADE_INPUT_WORK *wk, int mode )
{
	int i;
	s8 px, py;
	
	BGWINFRM_PosGet( wk->BgWinFrm, 0, &px, &py );

	switch(mode){
	case MODE_HEADWORD_1:
		for(i=0;i<MODE_HEADWORD1_WIN_NUM;i++){
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
			GFL_BMPWIN_TransVramCharacter( wk->MenuBmp[i] );
			GFL_BMPWIN_Delete( wk->MenuBmp[i] );
		}
		if(px == HEAD1_REWRITE_INIT_X){
      GFL_BG_FillScreen(
          GFL_BG_FRAME2_M, 5, 0x11-1, 1, 1, 0x10-1, GFL_BG_SCRWRT_PALNL);
    		GFL_BG_FillScreen(
    		     GFL_BG_FRAME2_M, 6, 0x11, 1, 0x10-1, 0x10-1, GFL_BG_SCRWRT_PALNL);
    		GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M);
    	}
		GFL_BMPWIN_Delete( wk->MenuBmp[BMPWIN_SELECT_END_WIN] );
		break;
	case MODE_HEADWORD_2:
		for(i=0;i<MODE_HEADWORD2_WIN_NUM;i++){
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
			GFL_BMPWIN_TransVramCharacter( wk->MenuBmp[i] );
			GFL_BMPWIN_Delete( wk->MenuBmp[i] );
		}
		if(px == HEAD1_REWRITE_INIT_X){
      GFL_BG_FillScreen(
          GFL_BG_FRAME2_M, 5, 0x11-1, 1, 1, 0x10-1, GFL_BG_SCRWRT_PALNL);
    		GFL_BG_FillScreen(
    		     GFL_BG_FRAME2_M, 6, 0x11, 1, 0x10-1, 0x10-1, GFL_BG_SCRWRT_PALNL);
    		GFL_BG_LoadScreenReq(GFL_BG_FRAME2_M);
    	}
		GFL_BMPWIN_Delete( wk->MenuBmp[BMPWIN_SELECT_END_WIN] );
		break;
	case MODE_POKEMON_NAME:
		for(i=0;i<MODE_PAGE_WIN_NUM+MODE_POKENAME_WIN_NUM;i++){
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
			GFL_BMPWIN_TransVramCharacter( wk->MenuBmp[i] );
			GFL_BMPWIN_Delete( wk->MenuBmp[i] );
		}
		if(px == HEAD1_REWRITE_INIT_X){
      GFL_BG_FillScreen(
          GFL_BG_FRAME2_M, 5, 0x11-1, 1, 1, 0x10-1, GFL_BG_SCRWRT_PALNL);
    		GFL_BG_FillScreen(GFL_BG_FRAME2_M, 6, 0x11, 1, 0x10-1, 0x10-1, GFL_BG_SCRWRT_PALNL);
    		GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M);
    	}
		GFL_BMPWIN_Delete( wk->MenuBmp[BMPWIN_SELECT_END_WIN]);
		break;
	case MODE_NATION_HEAD1:
		for(i=0;i<MODE_NATION_HEADWORD1_WIN_NUM;i++){
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
			GFL_BMPWIN_TransVramCharacter( wk->MenuBmp[i] );
			GFL_BMPWIN_Delete( wk->MenuBmp[i] );
		}
		if(px == HEAD1_REWRITE_INIT_X){
      GFL_BG_FillScreen(
          GFL_BG_FRAME2_M, 5, 0x11-1, 1, 1, 0x10-1, GFL_BG_SCRWRT_PALNL);
    		GFL_BG_FillScreen(GFL_BG_FRAME2_M, 6, 0x11, 1, 0x10-1, 0x10-1, GFL_BG_SCRWRT_PALNL);
    		GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M);
    	}
		GFL_BMPWIN_Delete( wk->MenuBmp[BMPWIN_SELECT_END_WIN]);
		GFL_BMPWIN_Delete( wk->MenuBmp[BMPWIN_NONE_SELECT_WIN]);
		break;
	case MODE_NATION:
		for(i=0;i<MODE_NATION_WIN_NUM+1;i++){
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
			GFL_BMPWIN_TransVramCharacter( wk->MenuBmp[i] );
			GFL_BMPWIN_Delete( wk->MenuBmp[i] );
		}
    /*
		if(px == HEAD1_REWRITE_INIT_X || px == 17){ //17=wipeout���Ă��鎞�A���̒l(BTS4836)
      GFL_BG_FillScreen(
    		    GFL_BG_FRAME2_M, 5, 0x11, 1, 1, 0x10-1, GFL_BG_SCRWRT_PALNL);
    		GFL_BG_FillScreen(
    		    GFL_BG_FRAME2_M, 6, 0x11+1, 1, 0x10-2, 0x10-1, GFL_BG_SCRWRT_PALNL);
    		GFL_BG_LoadScreenReq( GFL_BG_FRAME2_M);
    	}*/
		GFL_BMPWIN_Delete( wk->MenuBmp[BMPWIN_SELECT_END_WIN]);
		break;
	case MODE_SEX:
		for(i=0;i<MODE_SEX_WIN_NUM;i++){
			GFL_BMPWIN_Delete( wk->MenuBmp[i] );
		}
		// ���ǂ�
		GFL_BMPWIN_Delete(wk->MenuBmp[BMPWIN_SELECT_END_WIN]);
		break;
	case MODE_LEVEL:
		for(i=0;i<MODE_LEVEL_WIN_NUM+MODE_PAGE_WIN_NUM;i++){
			GFL_BMPWIN_Delete( wk->MenuBmp[i] );
		}
		// ���ǂ�
		GFL_BMPWIN_Delete(wk->MenuBmp[BMPWIN_SELECT_END_WIN]);
		break;
	}

  GFL_BG_FillCharacterRelease( wk->BgFrame, 1, 0 );
}


// ���̓��̓V�X�e����worldtrade_search.c��worldtrade_deposit.c�Ŏg�p�����B
// �e�X�̉�ʂŕK��BG�P����ʋ󂯂Ă����A����BG�œ��͂̐i�s���s����B
// 
// 
// 
//==============================================================================
/**
 * @brief   �e������i���O�E���ʁE���x���E���j�E���̓E�C���h�E�V�X�e��
 *
 * @param   wih		
 *
 * @retval  WORLDTRADE_INPUT_WORK *		
 */
//==============================================================================
WORLDTRADE_INPUT_WORK * WorldTrade_Input_Init( WORLDTRADE_INPUT_HEADER *wih, int frame, int situation  )
{
	WORLDTRADE_INPUT_WORK *wk = GFL_HEAP_AllocMemory(HEAPID_WORLDTRADE, sizeof(WORLDTRADE_INPUT_WORK));

	wk->MenuBmp     = wih->MenuBmp;
	wk->BackBmp     = wih->BackBmp;
	wk->CursorAct   = wih->CursorAct;
	wk->ArrowAct[0] = wih->ArrowAct[0];
	wk->ArrowAct[1] = wih->ArrowAct[1];
	wk->SearchCursorAct = wih->SearchCursorAct;
	wk->MsgManager      = wih->MsgManager;			// ���O���̓��b�Z�[�W�f�[�^�}�l�[�W���[
	wk->MonsNameManager = wih->MonsNameManager ;		// �|�P���������b�Z�[�W�f�[�^�}�l�[�W���[
	wk->CountryNameManager = wih->CountryNameManager;
	wk->zukan			= wih->Zukan;
	wk->SinouTable		= wih->SinouTable;

	wk->BgFrame   = frame;
	wk->Situation = situation;
	wk->Head1   = 0;
	wk->Head2   = 0;
	wk->Poke    = 0;
	wk->Nation  = 0;
	wk->Sex     = 0;
	wk->Level   = 0;
	wk->listpos = 0;
	wk->listpos_backup_x = 0;
	wk->seq     = WI_SEQ_NONE;

	// BG�E�C���h�E�t���[�����[�N�m��
	wk->BgWinFrm = BGWINFRM_Create( BGWINFRM_TRANS_VBLANK, 1, HEAPID_WORLDTRADE );

	// ���z�X�N���[���ʒǉ�
	BGWINFRM_Add( wk->BgWinFrm, 0, GFL_BG_FRAME2_M, 32, 20 );

	// �T�u�J�[�\���������W�ݒ�
	WorldTrade_CLACT_PosChange( wk->CursorAct, (word_cur_table[0][0]+16)*8, word_cur_table[0][1]*8 );
	GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 4 );
  GFL_CLACT_WK_SetAutoAnmFlag( wk->CursorAct, 1 );
  GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 ); //�t���[���C�����Ă���ݒ�


	// �����t�H���g�V�X�e��������
	wk->NumFontSys = NUMFONT_Create( 15, 14, 2, HEAPID_WORLDTRADE );

	//	�v�����g������
	WT_PRINT_Init( &wk->print, wih->config );
	
	return wk;
}


//==============================================================================
/**
 * @brief   �������͊J�n
 *			�Ȍ�WorldTrade_Input_Main()�Ăё����œ��͂͊�������͂�
 *
 * @param   wk		WORLDTRADE_INPUT_WORK
 * @param   type	MODE_POKEMON_NAME �` MODE_LEVEL  (worldtarde_input.h)
 *
 * @retval  none		
 */
//==============================================================================
void WorldTrade_Input_Start( WORLDTRADE_INPUT_WORK *wk, int type )
{
	wk->type = type;

	switch(type){
	case MODE_POKEMON_NAME:
		//�J�[�\���ʒu��������
		wk->listpos = 0;
    	wk->listpos_backup_x = 0;
		wk->Head1 = 0;
		wk->Head2 = 0;
		wk->seq = WI_SEQ_HEAD1_INIT;
		break;
	case MODE_NATION:
		//�J�[�\���ʒu��������
		wk->listpos = 0;
    	wk->listpos_backup_x = 0;
		wk->Head1 = 0;
		wk->Head2 = 0;
		wk->seq = WI_SEQ_NATION_HEAD1_INIT;
		break;
	case MODE_SEX:
		//�J�[�\���ʒu��������
		wk->listpos = 0;
    	wk->listpos_backup_x = 0;
		wk->seq = WI_SEQ_SEX_INIT;
		break;
	case MODE_LEVEL:
		//�J�[�\���ʒu��������
		wk->listpos = 0;
    	wk->listpos_backup_x = 0;
		wk->seq = WI_SEQ_LEVEL_INIT;
		break;
	}
}

//==============================================================================
/**
 * @brief   �e������i���O�E���ʁE���x���E���j�E���̓E�C���h�E�V�X�e�����
 *
 * @param   wk		
 *
 * @retval  none		
 */
//==============================================================================
void WorldTrade_Input_Exit( WORLDTRADE_INPUT_WORK *wk )
{
	WT_PRINT_Exit( &wk->print );

	NUMFONT_Delete( wk->NumFontSys );
	BGWINFRM_Exit( wk->BgWinFrm );
	GFL_HEAP_FreeMemory( wk );
}

//==============================================================================
/**
 * @brief   ���͏������C��
 *
 * @param   wk		WORLDTRADE_INPUT_WORK
 *
 * @retval  u32		
 */
//==============================================================================
u32 WorldTrade_Input_Main( WORLDTRADE_INPUT_WORK *wk )
{
	u32 ret;
	ret=(*FuncTable[wk->seq])(wk);

	BGWINFRM_MoveMain( wk->BgWinFrm );
	WT_PRINT_Main( &wk->print );
  NUMFONT_Main( wk->NumFontSys );

#ifdef PM_DEBUG
	{
		int d = ret;
		if (d !=-1){
			OS_Printf("ret=%d\n",ret);
		}
	}
#endif	
	return ret;
}



// �������E�C���h�E�J�[�\���ړ��e�[�u��
// 0 1 2 3 4
// 5 6 7 8 9 
//        10
// �㉺���E�̏��Ԃœ��͂����L�[�ɑΉ������ꏊ�Ɉړ�
static const u8 head1table[][4]={   //�L�[�����������̎��̔�ѐ�@UP, DOWN, LEFT, RIGHT
	{ 10, 5, 4, 1, },        //0�@��
	{ 10, 6, 0, 2, },        //1�@��
	{ 10, 7, 1, 3, },        //2�@��
	{ 10, 8, 2, 4, },        //3�@��
	{ 10, 9, 3, 0, },        //4�@��
	
	{ 0, 10, 9, 6, },        //5�@��
	{ 1, 10, 5, 7, },        //6�@��
	{ 2, 10, 6, 8, },        //7�@��
	{ 3, 10, 7, 9, },        //8�@��
	{ 4, 10, 8, 5, },        //9�@��
	
	{ 9,  4, 10, 10, },      //10�@���ǂ�
};

// ���w��̂Ƃ��ɓ�������I������ۂ̃e�[�u��
// 11
// 0 1 2 3 4
// 5 6 7 8 9
//         10
static const u8 n_head1table[][4]={     //�L�[�����������̎��̔�ѐ�@UP, DOWN, LEFT, RIGHT
	{ 11,  5,  4,  1, },        //0�@��
	{ 11,  6,  0, 2, },         //1�@��
	{ 11,  7, 1, 3, },          //2�@��
	{ 11,  8, 2, 4, },          //3�@��
	{ 11, 9, 3,    0, },        //4�@��
	
	{ 0, 10,  9,  6, },         //5�@��
	{  1, 10 , 5, 7, },         //6�@��
	{  2, 10, 6, 8, },          //7�@��
	{  3, 10, 7, 9, },          //8�@��
	{  4, 10, 8,   5, },        //9�@��
	
	{  9, 11, 10, 10, },        //10�@���ǂ�
	{ 10, 0, 11, 11, },	        //11�@���ɂ��Ȃ�
		
};

// �������E�C���h�E�J�[�\���ړ��e�[�u��2
// 0 1 2 3 4
//         5

static const u8 head2table[][4]={   //�L�[�����������̎��̔�ѐ�@UP, DOWN, LEFT, RIGHT
	{ 5, 5, 4, 1, },        //0�@��
	{ 5, 5, 0, 2, },        //1�@��
	{ 5, 5, 1, 3, },        //2�@��
	{ 5, 5, 2, 4, },       //3�@��
	{ 5, 5, 3, 0, },        //4�@��
	
	{ 4,  4, 5, 5, },       //5�@���ǂ�
};



static const u8 namepostable[][2]={
	{ 192,  24 },
	{ 192,  48 },
	{ 192,  72 },
	{ 192,  96 },
	{ 192, 136 },
};

static const u8 sexpostable[][2]={
	{ 192,  32  },
	{ 192,  64  },
	{ 192,  96  },
	{ 192, 136  },
};

static const u8 levelpostable[][2]={
	{ 192,  24  },
	{ 192,  48  },
	{ 192,  72  },
	{ 192,  96  },
	{ 192, 136  },
};

static const u8 nationpostable[][2]={
	{ 136,  16  },
	{ 136,  40  },
	{ 136,  64  },
	{ 136,  88  },
	{ 136, 112  },
	{ 192, 136  },
};


// �������P�p�^�b�`�[�e�[�u��
static const GFL_UI_TP_HITTBL Head1TouchTable[]={
	{  40,  60, 136, 151,},	// ��
	{  40,  60, 160, 175,},	// ��
	{  40,  60, 184, 199,},	// ��
	{  40,  60, 208, 223,},	// ��
	{  40,  60, 232, 247,},	// ��
	{  64,  84, 136, 151,},	// ��
	{  64,  84, 160, 175,},	// ��
	{  64,  84, 184, 199,},	// ��
	{  64,  84, 208, 223,},	// ��
	{  64,  84, 232, 247,},	// ��
	{ 133, 154, 197, 250,},	// ���ǂ�
	{GFL_UI_TP_HIT_END,0,0,0},		// �I���f�[�^
	
};


// �������P�p�^�b�`�[�e�[�u��
static const GFL_UI_TP_HITTBL NationHead1TouchTable[]={
	{  40,  60, 136, 151,},	// ��
	{  40,  60, 160, 175,},	// ��
	{  40,  60, 184, 199,},	// ��
	{  40,  60, 208, 223,},	// ��
	{  40,  60, 232, 247,},	// ��
	{  64,  84, 136, 151,},	// ��
	{  64,  84, 160, 175,},	// ��
	{  64,  84, 184, 199,},	// ��
	{  64,  84, 208, 223,},	// ��
	{  64,  84, 232, 247,},	// ��
	{ 133, 154, 197, 250,},	// ���ǂ�
	{  16,  35, 136, 248,},	// ���ɂ��Ȃ�
	{GFL_UI_TP_HIT_END,0,0,0},		// �I���f�[�^
	
};

// �������Q�p�^�b�`�[�e�[�u��
static const GFL_UI_TP_HITTBL Head2TouchTable[]={
	{  40,  60, 136, 151,},	// ��
	{  40,  60, 160, 175,},	// ��
	{  40,  60, 184, 199,},	// ��
	{  40,  60, 208, 223,},	// ��
	{  40,  60, 232, 247,},	// ��
	{ 133, 154, 197, 250,},	// ���ǂ�
	{GFL_UI_TP_HIT_END,0,0,0},		// �I���f�[�^
	
};

// ���x�����|�P�������p�^�b�`�[�e�[�u��
static const GFL_UI_TP_HITTBL LevelTouchTable[]={
	{  16,  35, 136, 248,},	// ���ɂ��Ȃ�
	{  40,  60, 136, 248,},	// LV10�`
	{  64,  84, 136, 248,},	// LV20�`
	{  88, 108, 136, 248,},	// LV30�`
	{ 133, 154, 198, 248,},	// ���ǂ�
	{ 108, 129, 143, 160,}, // ���������
	{ 108, 126, 223, 240,}, // �E�������
	{GFL_UI_TP_HIT_END,0,0,0},		// �I���f�[�^
};

// ���ʗp�^�b�`�[�e�[�u��
static const GFL_UI_TP_HITTBL SexTouchTable[]={
	{  24,  44, 136, 248,},	// ���ɂ��Ȃ�
	{  56,  76, 136, 248,},	// �I�X
	{  88, 108, 136, 248,},	// ���X
	{ 133, 154, 197, 250,},	// ���ǂ�
	{GFL_UI_TP_HIT_END,0,0,0},		// �I���f�[�^
};

// �����p�^�b�`�[�e�[�u��
static const GFL_UI_TP_HITTBL NationTouchTable[]={
	{   8,  28,   8, 247,},	// �����P
	{  32,  52,   8, 247,},	// �����Q
	{  56,  76,   8, 247,},	// �����R
	{  80, 100,   8, 247,},	// �����S
	{ 104, 124,   8, 247,},	// �����T
	{ 133, 154, 195, 248,},	// ���ǂ�
	{ 124, 145,  77,  96,}, // ���������
	{ 124, 145, 173, 190,}, // �E�������
	{GFL_UI_TP_HIT_END,0,0,0},		// �I���f�[�^
};



static const u8 word2_table[][3]={
	{  2,  2, SELECT_BOX_R_LETTER},
	{  2,  5, SELECT_BOX_N_LETTER},
	{  5,  5, SELECT_BOX_N_LETTER},
	{  8,  5, SELECT_BOX_N_LETTER},
	{ 11,  5, SELECT_BOX_N_LETTER},
	{ 14,  5, SELECT_BOX_N_LETTER},
	
};

static const int head2_word_start[][2]={
	{  0,	5,},	// ��
	{  5,	5,},	// ��
	{ 10,	5,},	// ��
	{ 15,	5,},	// ��
	{ 20,	5,},	// ��
	{ 25,	5,},	// ��
	{ 30,	5,},	// ��
	{ 35,	3,},	// ��
	{ 40,	5,},	// ��
	{ 45,	0,},	// ��
};


static const int head_letter_table[]={
	msg_gts_word_001,	msg_gts_word_002, msg_gts_word_003, msg_gts_word_004, msg_gts_word_005,	// ��
	msg_gts_word_006,	msg_gts_word_007, msg_gts_word_008, msg_gts_word_009, msg_gts_word_010, // ��
	msg_gts_word_011,	msg_gts_word_012, msg_gts_word_013, msg_gts_word_014, msg_gts_word_015, // ��
	msg_gts_word_016,	msg_gts_word_017, msg_gts_word_018, msg_gts_word_019, msg_gts_word_020, // ��
	msg_gts_word_021,	msg_gts_word_022, msg_gts_word_023, msg_gts_word_024, msg_gts_word_025, // ��
	msg_gts_word_026,	msg_gts_word_027, msg_gts_word_028, msg_gts_word_029, msg_gts_word_030, // ��
	msg_gts_word_031,	msg_gts_word_032, msg_gts_word_033, msg_gts_word_034, msg_gts_word_035, // ��
	msg_gts_word_036,	msg_gts_word_037, msg_gts_word_038, msg_gts_word_045, msg_gts_word_045, // ��
	msg_gts_word_039,	msg_gts_word_040, msg_gts_word_041, msg_gts_word_042, msg_gts_word_043, // ��
	msg_gts_word_044,                                                                           // ��
};

static const int head2pokename[]={
	  0,	// ��
	  5,	// ��
	 10,	// ��
	 15,	// ��
	 20,	// ��
	 25,	// ��
	 30,	// ��
	 35,	// ��
	 38,	// ��
	 43,	// ��
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
static u32 TouchPanelFunc( WORLDTRADE_INPUT_WORK *wk, int mode )
{
	u32 ret;
	switch(mode){
	case MODE_HEADWORD_1:	// �������q���w��
		ret=GFL_UI_TP_HitTrg(Head1TouchTable);
		break;
	case MODE_NATION_HEAD1:	// �����������w��
		ret=GFL_UI_TP_HitTrg(NationHead1TouchTable);
		break;
	case MODE_POKEMON_NAME:	// �|�P�������O�w��
	case MODE_LEVEL:		// ���x�������w��
		ret=GFL_UI_TP_HitTrg(LevelTouchTable);
		break;
	case MODE_SEX:			// ���ʏ����w��
		ret=GFL_UI_TP_HitTrg(SexTouchTable);
		break;
	case MODE_NATION:		// ����
		ret=GFL_UI_TP_HitTrg(NationTouchTable);
		break;
	case MODE_HEADWORD_2:	// �������ꉹ�w��
	case MODE_NATION_HEAD2:	// �����������w��
		ret=GFL_UI_TP_HitTrg(Head2TouchTable);
		break;
	}

	return ret;
}


//------------------------------------------------------------------
/**
 * @brief   �������I�����C��
 *
 * @param   wk		
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
static u32 WordHead_SelectMain( WORLDTRADE_INPUT_WORK *wk, u8 *see_check )
{
	int oldpos = wk->listpos;
	
	if(oldpos < 10){    //���`��
	    wk->listpos_backup_x = oldpos;  //X�ʒu������Ă���
	}

  if( GFL_UI_KEY_GetTrg() && GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    PMSND_PlaySE(WORLDTRADE_MOVE_SE);
    return BMPMENU_NULL;
  }
	
	if(GFL_UI_KEY_GetRepeat() & PAD_KEY_UP){
		wk->listpos = head1table[wk->listpos][0];
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN){
		wk->listpos = head1table[wk->listpos][1];
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT){
		wk->listpos = head1table[wk->listpos][2];
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT){
		wk->listpos = head1table[wk->listpos][3];
	}

	//�u���ǂ�v�̈ʒu����u���`��v���C���ɕ��A�����ꍇ��X�ʒu��߂��␳
	if(oldpos >= 10 && wk->listpos < 10){
        if(wk->listpos > 4){    //���i�ɂ��ǂ��Ă���
            if(wk->listpos_backup_x > 4){
                wk->listpos = wk->listpos_backup_x;
            }
            else{
                wk->listpos = wk->listpos_backup_x + 5;
            }
            wk->listpos_backup_x = wk->listpos;
        }
        else{   //��i�ɖ߂��Ă���
            if(wk->listpos_backup_x > 4){
                wk->listpos = wk->listpos_backup_x - 5;
            }
            else{
                wk->listpos = wk->listpos_backup_x;
            }
            wk->listpos_backup_x = wk->listpos;
        }
    }


	if(oldpos!=wk->listpos){
		PMSND_PlaySE(WORLDTRADE_MOVE_SE);
		WorldTrade_CLACT_PosChange( wk->CursorAct, 
						(word_cur_table[wk->listpos][0]+16)*8,
						word_cur_table[wk->listpos][1]*8 );
		if(wk->listpos==10){
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
		}else{
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 4 );
		}

	}
	
	{
		u32 ret;
		// �^�b�`����
		ret = TouchPanelFunc( wk, MODE_HEADWORD_1 );
		if(ret!=GFL_UI_TP_HIT_NONE){
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );

      WorldTrade_CLACT_PosChange( wk->CursorAct, 
          (word_cur_table[ret][0]+16)*8,
          word_cur_table[ret][1]*8 );
      if(ret==10){
        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
      }else{
        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 4 );
      }

      if(ret == 10){
        PMSND_PlaySE(SE_CANCEL);
        return BMPMENU_CANCEL;
      }
      GF_ASSERT(ret < 10);
      if(see_check == NULL || see_check[ret]){
        PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
        return ret;
      }
    }else{
      // �L�[����
			if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){


				if(wk->listpos == 10){				
        PMSND_PlaySE(SE_CANCEL);
					return BMPMENU_CANCEL;
				}
				GF_ASSERT(wk->listpos < 10);
				if(see_check == NULL || see_check[wk->listpos]){
          PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
					return wk->listpos;
				}
		
			}else if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_CANCEL){
				PMSND_PlaySE(SE_CANCEL);

        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
        WorldTrade_CLACT_PosChange( wk->CursorAct, 
          (word_cur_table[10][0]+16)*8,
          word_cur_table[10][1]*8 );

				return BMPMENU_CANCEL;
			}
		}
	}
	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   �������Q�̌��菈��
 *
 * @param   wk		
 * @param   decide		
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
static u32 Head2DecideFunc( WORLDTRADE_INPUT_WORK *wk, int decide )
{
	if(decide!=5){
		if(wk->Head1==INPUT_HEAD_YA){
            if(decide<3){
    			return decide;
    		}
    		else{
                return BMPMENU_NULL;
            }
		}else if(wk->Head1!=INPUT_HEAD_YA){
			return decide;
		}
	}

	return BMPMENU_CANCEL;

}

//------------------------------------------------------------------
/**
 * @brief   �������Q�����ڑI�����C��
 *
 * @param   wk		
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
static u32 WordHead2_SelectMain( WORLDTRADE_INPUT_WORK *wk, u8 *see_check )
{
	int oldpos = wk->listpos;

	if(oldpos < 5){    //���`��
	    wk->listpos_backup_x = oldpos;  //X�ʒu������Ă���
	}

  if( GFL_UI_KEY_GetTrg() && GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    PMSND_PlaySE(WORLDTRADE_MOVE_SE);
    return BMPMENU_NULL;
  }

	// �J�[�\���ړ�
	if(GFL_UI_KEY_GetRepeat() & PAD_KEY_UP){
		wk->listpos = head2table[wk->listpos][0];
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN){
		wk->listpos = head2table[wk->listpos][1];
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT){
		wk->listpos = head2table[wk->listpos][2];
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT){
		wk->listpos = head2table[wk->listpos][3];
	}

	//�u���ǂ�v�̈ʒu����u���`���v���C���ɕ��A�����ꍇ��X�ʒu��߂��␳
	if(oldpos == 5 && wk->listpos < 5){
        wk->listpos = wk->listpos_backup_x;
    }

	// �ړ����̓A�N�^�[�𔽉f������
	if(oldpos!=wk->listpos){
    PMSND_PlaySE(WORLDTRADE_MOVE_SE);
		if(wk->listpos==5){
			WorldTrade_CLACT_PosChange( wk->CursorAct, 
						(word_cur_table[10][0]+16)*8,
						 word_cur_table[10][1]*8 );
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
		}else{
			WorldTrade_CLACT_PosChange( wk->CursorAct, 
						(word_cur_table[wk->listpos][0]+16)*8,
						 word_cur_table[wk->listpos][1]*8 );
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 4 );
		}

	}

	// ����E�L�����Z��
	{
		u32 ret;
		u32 decide;
		// �^�b�`����
		ret = TouchPanelFunc( wk, MODE_HEADWORD_2 );
		if(ret!=GFL_UI_TP_HIT_NONE){
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );

      if(ret==5){
        WorldTrade_CLACT_PosChange( wk->CursorAct, 
            (word_cur_table[10][0]+16)*8,
            word_cur_table[10][1]*8 );
        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
      }else{
        WorldTrade_CLACT_PosChange( wk->CursorAct, 
            (word_cur_table[ret][0]+16)*8,
            word_cur_table[ret][1]*8 );
        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 4 );
      }

			decide = Head2DecideFunc( wk, ret );
      if(decide == BMPMENU_NULL){ //���s�̒[2�������̋󔒂��^�b�`�������݂̂̏���
        return decide;
      }
      if(decide == BMPMENU_CANCEL)
      { 
        PMSND_PlaySE(SE_CANCEL);
        return decide;
      }
      else if( see_check == NULL || (see_check[head2pokename[wk->Head1] + decide])){
        PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
        return decide;
      }
		}else{
		// �L�[����
			if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
				decide = Head2DecideFunc( wk, wk->listpos );
    			if(decide == BMPMENU_NULL){ //���s�̒[2�������̋󔒂��^�b�`�������݂̂̏���
            return decide;
          }
          if(decide == BMPMENU_CANCEL )
          { 
            PMSND_PlaySE(SE_CANCEL);
            return decide;
          }
          else if( see_check == NULL || (see_check[head2pokename[wk->Head1] + decide])){
            PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
            return decide;
          }
			}else if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_CANCEL){
        PMSND_PlaySE(SE_CANCEL);

        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
        WorldTrade_CLACT_PosChange( wk->CursorAct, 
          (word_cur_table[10][0]+16)*8,
          word_cur_table[10][1]*8 );

				return BMPMENU_CANCEL;
			}
		}
	}
	return BMPMENU_NULL;
}



#define POKENAME_1PAGE_MAX	( 4 )
#define LEVEL_1PAGE_MAX	( 4 )
#define NATION_1PAGE_MAX	( 5 )

//------------------------------------------------------------------
/**
 * @brief   ���̑����łS���ڂ����Ɖ��y�[�W�ɂȂ�H
 *
 * @param   num			���ڑ���
 * @param   in_page		�P�y�[�W�����ڕ\���\���H
 *
 * @retval  int			�y�[�W��	1�`	
 */
//------------------------------------------------------------------
static int _list_page_num( int num, int in_page)
{
	int page;
	if (num == 0){
		return 1;
	}
	page = (num+(in_page-1))/in_page;

	return page;
}

//------------------------------------------------------------------
/**
 * @brief   ���O���͌��菈��
 *
 * @param   wk		
 * @param   decide		
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
static u32 NameDecideFunc( WORLDTRADE_INPUT_WORK *wk, int decide )
{
	switch(decide){
	case 4:
		return BMPMENU_CANCEL;
		break;
	case 5:
		NamePageRefresh( wk, -1 );
		return BMPMENU_NULL;
		break;
	case 6:
		NamePageRefresh( wk, +1 );
		return BMPMENU_NULL;
		break;
	}

	// ���ڌ���
	{
		int namepos = decide+wk->page*POKENAME_1PAGE_MAX;
		// ���O�̂���ꏊ�Ȃ���͌���
		if(namepos<wk->listMax){
			PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
			return wk->NameList[namepos].param;
		}
	}

	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   �|�P�������I���y�[�W�؂�ւ�����
 *
 * @param   wk		
 * @param   move	-1 or +1
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void NamePageRefresh( WORLDTRADE_INPUT_WORK *wk, int move )
{
	int page_no = _list_page_num( wk->listMax, MODE_POKENAME_WIN_NUM ) - 1;

	if (page_no == 0){
		return;
	}

	if(move<0){
		PMSND_PlaySE(WORLDTRADE_PAGE_SE);
		//���A���[�A�j��
		GFL_CLACT_WK_SetAutoAnmFlag( wk->ArrowAct[1], 1 );
		GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[1], CELL_BOXARROW_NO+1 );
		if(wk->page!=0){
			wk->page--;	
		}else{
			wk->page = page_no;
		}
	}else{
		PMSND_PlaySE(WORLDTRADE_PAGE_SE);
		//�E�A���[�A�j��
		GFL_CLACT_WK_SetAutoAnmFlag( wk->ArrowAct[0], 1 );
		GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[0], CELL_BOXARROW_NO );
		if(wk->page < page_no){
			wk->page++;
		}else{
			wk->page = 0;
		}
	}

	PokeName_ListPrint( wk, wk->page, wk->listMax );
	PagePrint(  wk->BgWinFrm, wk->NumFontSys, wk->MenuBmp[MODE_POKENAME_WIN_NUM], 
				wk->page,
				_list_page_num( wk->listMax, MODE_POKENAME_WIN_NUM) );

}

//------------------------------------------------------------------
/**
 * @brief   ���O�I�����C��
 *
 * @param   wk		
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
static u32 PokeName_SelectMain( WORLDTRADE_INPUT_WORK *wk )
{
	int oldpos = wk->listpos;

  if( GFL_UI_KEY_GetTrg() && GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    PMSND_PlaySE(WORLDTRADE_MOVE_SE);
    return BMPMENU_NULL;
  }


	if(GFL_UI_KEY_GetRepeat() & PAD_KEY_UP){
		if(wk->listpos!=0){
			wk->listpos--;
		}else{
			wk->listpos=4;
		}
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN){
		if(wk->listpos!=4){
			wk->listpos++;
		}else{
			wk->listpos=0;
		}
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT){
		NamePageRefresh( wk, -1 );
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT){
		NamePageRefresh( wk, +1 );
	}

	if(oldpos!=wk->listpos){
    PMSND_PlaySE(WORLDTRADE_MOVE_SE);
		WorldTrade_CLACT_PosChange( wk->CursorAct, 
					namepostable[wk->listpos][0],namepostable[wk->listpos][1] );
		if(wk->listpos==4){
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
		}else{
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 6 );
		}
	}

	{
		u32 ret;
		// �^�b�`����
		ret = TouchPanelFunc( wk, MODE_POKEMON_NAME );
		if(ret!=GFL_UI_TP_HIT_NONE){
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );

      if( ret < 5 )
      { 
        WorldTrade_CLACT_PosChange( wk->CursorAct, 
            namepostable[ret][0],namepostable[ret][1] );
        if(ret==4){
          GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
        }else{
          GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 6 );
        }
      }

      return NameDecideFunc( wk, ret );
		}else{
		// �L�[����
			if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
				return NameDecideFunc( wk, wk->listpos );
			}else if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_CANCEL){

        WorldTrade_CLACT_PosChange( wk->CursorAct, 
            namepostable[4][0],namepostable[4][1] );
        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );

        PMSND_PlaySE(SE_CANCEL);
				return BMPMENU_CANCEL;
			}
		}
	}
	return BMPMENU_NULL;
}


static const u32 s_ret_table[]={
	PARA_UNK,
    PARA_MALE,
    PARA_FEMALE,
    BMPMENU_CANCEL,
};
//------------------------------------------------------------------
/**
 * @brief   ���ʏ������̓R���g���[��
 *
 * @param   wk		
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
static u32 SexSelect_SelectMain( WORLDTRADE_INPUT_WORK *wk )
{
	int oldpos = wk->listpos;

  if( GFL_UI_KEY_GetTrg() && GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    PMSND_PlaySE(WORLDTRADE_MOVE_SE);
    return BMPMENU_NULL;
  }


	if(GFL_UI_KEY_GetRepeat() & PAD_KEY_UP){
		if(wk->listpos!=0){
			wk->listpos--;
		}else{
			wk->listpos=3;
		}
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN){
		if(wk->listpos!=3){
			wk->listpos++;
		}else{
			wk->listpos=0;
		}
	}

	if(oldpos!=wk->listpos){
    PMSND_PlaySE(WORLDTRADE_MOVE_SE);
		WorldTrade_CLACT_PosChange( wk->CursorAct, 
					sexpostable[wk->listpos][0],sexpostable[wk->listpos][1] );
		if(wk->listpos==3){
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
		}else{
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 6 );
		}
	}


	{
		u32 ret;
		// �^�b�`����
		ret = TouchPanelFunc( wk, MODE_SEX );
		if(ret!=GFL_UI_TP_HIT_NONE){
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );
      WorldTrade_CLACT_PosChange( wk->CursorAct, 
          sexpostable[ret][0],sexpostable[ret][1] );
      if(ret==3){
        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
      }else{
        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 6 );
      }

      if( s_ret_table[ret] == BMPMENU_CANCEL )
      {
        PMSND_PlaySE(SE_CANCEL);
      }
      else
      { 
        PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
      }
			return s_ret_table[ret];
		}else{
		// �L�[����
			if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
        if( s_ret_table[wk->listpos] == BMPMENU_CANCEL )
        { 
          PMSND_PlaySE(SE_CANCEL);
        }
        else
        { 
          PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
        }
				return s_ret_table[wk->listpos];

			}else if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_CANCEL){
        PMSND_PlaySE(SE_CANCEL);

        WorldTrade_CLACT_PosChange( wk->CursorAct, 
            sexpostable[3][0],sexpostable[3][1] );
        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );

				return BMPMENU_CANCEL;
			}
		}
	}

	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   ���x���I���̃y�[�W�؂�ւ�
 *
 * @param   wk		
 * @param   move	-1 or +1
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void LevelPageRefresh( WORLDTRADE_INPUT_WORK *wk, int move )
{
	if(move<0){
		PMSND_PlaySE(WORLDTRADE_PAGE_SE);
		//���A���[�A�j��
		GFL_CLACT_WK_SetAutoAnmFlag( wk->ArrowAct[1], 1 );
		GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[1], CELL_BOXARROW_NO+1 );
		if(wk->page!=0){
			wk->page--;
		}else{
			wk->page = LV_PAGE_MAX;
		}
	}else{
		PMSND_PlaySE(WORLDTRADE_PAGE_SE);
		//�E�A���[�A�j��
		GFL_CLACT_WK_SetAutoAnmFlag( wk->ArrowAct[0], 1 );
		GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[0], CELL_BOXARROW_NO );
		if(wk->page < LV_PAGE_MAX){
			wk->page++;
		}else{
			wk->page = 0;
		}
	}
	PokeName_ListPrint( wk, wk->page, wk->listMax );
	PagePrint(  wk->BgWinFrm, wk->NumFontSys, wk->MenuBmp[MODE_LEVEL_WIN_NUM], 
				wk->page,
				_list_page_num( wk->listMax, MODE_LEVEL_WIN_NUM ) );

}

//------------------------------------------------------------------
/**
 * @brief   ���x���I�����菈��
 *
 * @param   wk		
 * @param   decide		
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
static u32 LevelDecideFunc( WORLDTRADE_INPUT_WORK *wk, u32 decide )
{
	switch(decide){
	case 4:
    PMSND_PlaySE(SE_CANCEL);
		return BMPMENU_CANCEL;
		break;
	case 5:
		LevelPageRefresh( wk, -1 );
		return BMPMENU_NULL;
		break;
	case 6:
		LevelPageRefresh( wk, +1 );
		return BMPMENU_NULL;
		break;
	}
	// ���ڌ���
	{
		int pos = decide+wk->page*LEVEL_1PAGE_MAX;
		// ���O�̂���ꏊ�Ȃ���͌���
		if(pos<wk->listMax){
			PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
			return wk->NameList[decide].param;
		}
	}
	
	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   ���x���������̓R���g���[��
 *
 * @param   wk		
 *
 * @retval  u32 	
 */
//------------------------------------------------------------------
static u32 LevelSelect_SelectMain( WORLDTRADE_INPUT_WORK *wk )
{
	int oldpos = wk->listpos;

  if( GFL_UI_KEY_GetTrg() && GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    PMSND_PlaySE(WORLDTRADE_MOVE_SE);
    return BMPMENU_NULL;
  }

	if(GFL_UI_KEY_GetRepeat() & PAD_KEY_UP){
		if(wk->listpos!=0){
			wk->listpos--;
		}else{
			wk->listpos=4;
		}
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN){
		if(wk->listpos!=4){
			wk->listpos++;
		}else{
			wk->listpos=0;
		}
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT){
		LevelPageRefresh( wk, -1 );
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT){
		LevelPageRefresh( wk, +1 );
	}

	if(oldpos!=wk->listpos){
    PMSND_PlaySE(WORLDTRADE_MOVE_SE);
		WorldTrade_CLACT_PosChange( wk->CursorAct, 
					levelpostable[wk->listpos][0],levelpostable[wk->listpos][1] );
		if(wk->listpos==4){
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
		}else{
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 6 );
		}
	}

	{
		u32 ret;
		// �^�b�`����
		ret = TouchPanelFunc( wk, MODE_LEVEL );
		if(ret!=GFL_UI_TP_HIT_NONE){
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );

      if( ret < 5 )
      { 
        WorldTrade_CLACT_PosChange( wk->CursorAct, 
            levelpostable[ret][0],levelpostable[ret][1] );
        if(ret==4){
          GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
        }else{
          GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 6 );
        }
      }

      return LevelDecideFunc( wk, ret );
		}else{
		// �L�[����
			if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
        return LevelDecideFunc( wk, wk->listpos );
			}else if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_CANCEL){

        WorldTrade_CLACT_PosChange( wk->CursorAct, 
					levelpostable[4][0],levelpostable[4][1] );
        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );

        PMSND_PlaySE(SE_CANCEL);
				return BMPMENU_CANCEL;
			}
		}
	}

	return BMPMENU_NULL;
	
}


//------------------------------------------------------------------
/**
 * @brief   �����I�𓪕����P�̌��菈��
 *
 * @param   wk		
 * @param   decide		
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
static u32 NationHead1DecideFunc( WORLDTRADE_INPUT_WORK *wk, int decide )
{
	switch(decide){
	case 10:	
		return BMPMENU_CANCEL;	
		break;
	case 11:	
		return 11;				
		break;
	}

	return decide;
}

//------------------------------------------------------------------
/**
 * @brief   �������I�����C��
 *
 * @param   wk		
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
static u32 NationHead1_SelectMain( WORLDTRADE_INPUT_WORK *wk, u8 *see_check )
{
	int oldpos = wk->listpos;
	
	if(oldpos < 10){    //���`��
	    wk->listpos_backup_x = oldpos;  //X�ʒu������Ă���
	}

  if( GFL_UI_KEY_GetTrg() && GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    PMSND_PlaySE(WORLDTRADE_MOVE_SE);
    return BMPMENU_NULL;
  }
        
	if(GFL_UI_KEY_GetRepeat() & PAD_KEY_UP){
		wk->listpos = n_head1table[wk->listpos][0];
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN){
		wk->listpos = n_head1table[wk->listpos][1];
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT){
		wk->listpos = n_head1table[wk->listpos][2];
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT){
		wk->listpos = n_head1table[wk->listpos][3];
	}
	
	//�u���ǂ�or���ɂ��Ȃ��v�̈ʒu����u���`��v���C���ɕ��A�����ꍇ��X�ʒu��߂��␳
	if(oldpos >= 10 && wk->listpos < 10){
        switch(oldpos){
        case 10:    //���ǂ�
            if(wk->listpos_backup_x > 4){
                wk->listpos = wk->listpos_backup_x;
            }
            else{
                wk->listpos = wk->listpos_backup_x + 5;
            }
            wk->listpos_backup_x = wk->listpos;
            break;
        case 11:    //���ɂ��Ȃ�
            if(wk->listpos_backup_x > 4){
                wk->listpos = wk->listpos_backup_x - 5;
            }
            else{
                wk->listpos = wk->listpos_backup_x;
            }
            wk->listpos_backup_x = wk->listpos;
            break;
        }
    }

	if(oldpos!=wk->listpos){
		PMSND_PlaySE(WORLDTRADE_MOVE_SE);
		WorldTrade_CLACT_PosChange( wk->CursorAct, 
						(n_word_table[wk->listpos][0]+16)*8,
						 n_word_table[wk->listpos][1]*8 );
		switch(wk->listpos){
		case 10:	GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );	break;
		case 11:	GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 6 );	break;
		default:	GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 4 );	
		}

	}


	{
		u32 ret, decide;
		// �^�b�`����
		ret = TouchPanelFunc( wk, MODE_NATION_HEAD1 );
		if(ret!=GFL_UI_TP_HIT_NONE){
      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );

      WorldTrade_CLACT_PosChange( wk->CursorAct, 
          (n_word_table[ret][0]+16)*8,
          n_word_table[ret][1]*8 );
      switch(ret){
      case 10:	GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );	break;
      case 11:	GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 6 );	break;
      default:	GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 4 );	
      }

      decide = NationHead1DecideFunc( wk, ret );
      if(decide == BMPMENU_CANCEL)
      { 
        PMSND_PlaySE(SE_CANCEL);
        return decide;
      }
      if( decide == 11 || see_check == NULL || see_check[decide]){
        PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
        return decide;
      }
		}else{
		// �L�[����
			if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
				decide = NationHead1DecideFunc( wk, wk->listpos );
        if(decide == BMPMENU_CANCEL)
        { 
          PMSND_PlaySE(SE_CANCEL);
          return decide;
        }
        else if( decide == 11 || see_check == NULL || see_check[decide]){
          PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
          return decide;
        }
			}else if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_CANCEL){

        WorldTrade_CLACT_PosChange( wk->CursorAct, 
            (n_word_table[10][0]+16)*8,
            n_word_table[10][1]*8 );
        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );

        PMSND_PlaySE(SE_CANCEL);
				return BMPMENU_CANCEL;
			}
		}
	}

	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   �����I�����菈��
 *
 * @param   wk		
 * @param   decide	NationTouchTable�̋�`����擾�����ԍ�
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
static u32 NationDecideFunc( WORLDTRADE_INPUT_WORK *wk, int decide )
{
	switch(decide){
	case 5:		// �L�����Z��
    PMSND_PlaySE(SE_CANCEL);
		return BMPMENU_CANCEL;
		break;
	case 6:		// ���y�[�W��
		NationPageRefresh(wk, -1);
		return BMPMENU_NULL;
		break;
	case 7:		// �E�y�[�W��
		NationPageRefresh(wk, +1);
		return BMPMENU_NULL;
		break;
	}

	// ���E�L�����Z���ȊO
	{
		int namepos = decide+wk->page*NATION_1PAGE_MAX;
		// ���O�̂���ꏊ�Ȃ���͌���
		if(namepos<wk->listMax){
			PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
			return wk->NameList[namepos].param;
		}
	}

	return BMPMENU_NULL;

}


//------------------------------------------------------------------
/**
 * @brief   ���w��y�[�W�ύX����
 *
 * @param   wk		
 * @param   move	-1 or +1
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void NationPageRefresh( WORLDTRADE_INPUT_WORK *wk, int move)
{
	int page_no = _list_page_num( wk->listMax, MODE_NATION_WIN_NUM ) -1;

	OS_Printf("page_no = %d\n",page_no);

	if (page_no == 0){
		return;
	}

	if(move<0){
		PMSND_PlaySE(WORLDTRADE_PAGE_SE);
		//���A���[�A�j��
		GFL_CLACT_WK_SetAutoAnmFlag( wk->ArrowAct[1], 1 );
		GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[1], CELL_BOXARROW_NO+1 );
		if(wk->page!=0){
			wk->page--;
		}else{
			wk->page = page_no;
		}
	}else{
		PMSND_PlaySE(WORLDTRADE_PAGE_SE);
		//�E�A���[�A�j��
		GFL_CLACT_WK_SetAutoAnmFlag( wk->ArrowAct[0], 1 );
		GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[0], CELL_BOXARROW_NO );
		if(wk->page < page_no){
			wk->page++;	
		}else{
			wk->page = 0;
		}
	}

	Nation_ListPrint( wk, wk->page, wk->listMax );
	PagePrint(  wk->BgWinFrm, wk->NumFontSys, wk->MenuBmp[MODE_NATION_WIN_NUM], 
				wk->page,
				_list_page_num( wk->listMax, MODE_NATION_WIN_NUM ) );
	
}

//------------------------------------------------------------------
/**
 * @brief   ���I�𑀍상�C��
 *
 * @param   wk		
 *
 * @retval  u32		
 */
//------------------------------------------------------------------
static u32 Nation_SelectMain( WORLDTRADE_INPUT_WORK *wk )
{
	int oldpos = wk->listpos;

  if( GFL_UI_KEY_GetTrg() && GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_UI_SetTouchOrKey( GFL_APP_END_KEY );
    PMSND_PlaySE(WORLDTRADE_MOVE_SE);
    return BMPMENU_NULL;
  }

	if(GFL_UI_KEY_GetRepeat() & PAD_KEY_UP){
		if(wk->listpos!=0){
			wk->listpos--;
		}else{
			wk->listpos=5;
		}
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN){
		if(wk->listpos!=5){
			wk->listpos++;
		}else{
			wk->listpos=0;
		}
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_LEFT){
		NationPageRefresh( wk, -1);
	}else if(GFL_UI_KEY_GetRepeat() & PAD_KEY_RIGHT){
		NationPageRefresh( wk, +1);
	}

	if(oldpos!=wk->listpos){
		PMSND_PlaySE(WORLDTRADE_MOVE_SE);
		WorldTrade_CLACT_PosChange( wk->CursorAct, 
					nationpostable[wk->listpos][0],nationpostable[wk->listpos][1] );
		if(wk->listpos==5){
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
		}else{
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 7 );
		}
	}

	{
		u32 ret;
		// �^�b�`����
		ret = TouchPanelFunc( wk, MODE_NATION );
		if(ret!=GFL_UI_TP_HIT_NONE){

      GFL_UI_SetTouchOrKey( GFL_APP_END_TOUCH );

      if( ret < 6 )
      { 
        WorldTrade_CLACT_PosChange( wk->CursorAct, 
            nationpostable[ret][0],nationpostable[ret][1] );
        if(ret==5){
          GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
        }else{
          GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 7 );
        }
      }
      return NationDecideFunc( wk, ret );
		}else{
		// �L�[����
			if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_DECIDE){
				return NationDecideFunc( wk, wk->listpos );
			}else if(GFL_UI_KEY_GetTrg()&PAD_BUTTON_CANCEL){
        PMSND_PlaySE(SE_CANCEL);
        WorldTrade_CLACT_PosChange( wk->CursorAct, 
            nationpostable[5][0],nationpostable[5][1] );
        GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 5 );
				return BMPMENU_CANCEL;
			}
		}
	}

	return BMPMENU_NULL;
	
}

//------------------------------------------------------------------
/**
 * @brief   �q�����������͉�ʏ������̃T�u����
 *
 * @param   wk		
 * @param   type	MODE_HEAD1 or MODE_NATION_HEAD1
 * @param   x		�X�N���[�������o���ʒu�̏����ʒux
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void HeadWord1_Init( WORLDTRADE_INPUT_WORK *wk, int type, int x )
{
	int scr_dat;
	u16 *scr;

	// �f�t�H���g�X�N���[�������z�ʂɓǂݍ���
	
	if(type==MODE_NATION_HEAD1){	// ���I���̎�
		scr_dat = NARC_worldtrade_search_name2_lz_nscr;
	}else{							//�|�P�����I���̎�
		scr_dat = NARC_worldtrade_search_name3_lz_nscr;
	}

	BGWINFRM_FrameSetArc( wk->BgWinFrm, 0, ARCID_WORLDTRADE_GRA, scr_dat, 1 );
	scr = BGWINFRM_FrameBufGet( wk->BgWinFrm, 0 );

	// BMPWIN�m��
	select_bmpwin_add( wk, type );
//	BGWINFRM_FrameSetArc( wk->BgWinFrm, 0, ARCID_WORLDTRADE_GRA, NARC_worldtrade_search_sub_lz_nscr, 1 );

	// �A�J�T�^�i�n�}������BMPWIN�m��
	{
		int i;
		for(i=0;i<MODE_HEADWORD1_WIN_NUM;i++){
//			SelectFrameBoxWrite( scr, word_table[i][0], word_table[i][1], SELECT_BOX_N_LETTER );
		}
	}
//	SelectFrameBoxWrite( scr, word_table[10][0]-1, word_table[10][1]-1, SELECT_BOX_END );

// ���ǂ�
	touch_print( wk->BgWinFrm, wk->MsgManager, wk->MenuBmp[BMPWIN_SELECT_END_WIN], msg_gtc_03_009, &wk->print );

	// ������`��
	{
		int i;
		STRBUF *strbuf;
		PRINTSYS_LSB color;
		
        MI_CpuFill8(wk->see_check, TRUE, SEE_CHECK_MAX);
		for(i=0;i<MODE_HEADWORD1_WIN_NUM;i++){
			strbuf = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_10_001+i );
			if(type == MODE_HEADWORD_1){    //�|�P��������
    			if(PokeSeeCount_Category(wk, i) == TRUE){
                    color = SEARCH_MOJI_COLOR_NORMAL;
                    wk->see_check[i] = TRUE;
                }
                else{
                    color =  SEARCH_MOJI_COLOR_GRAY;
                    wk->see_check[i] = FALSE;
                }
    		}
    		else{   //������
    		    if(NationCount_Category(wk, i) == TRUE){
        		    color = SEARCH_MOJI_COLOR_NORMAL;
        		    wk->see_check[i] = TRUE;
        		}
        		else{
                    color = SEARCH_MOJI_COLOR_GRAY;
        		    wk->see_check[i] = FALSE;
                }
            }
            
			WorldTrade_Input_SysPrint( wk->BgWinFrm, wk->MenuBmp[i], strbuf,  2, color, &wk->print );
			GFL_STR_DeleteBuffer(strbuf);
		}

	}



	// ���I���̎��́u���ɂ��Ȃ��v��ǉ�����
	if(type==MODE_NATION_HEAD1){
		STRBUF *strbuf = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_search_015_01 );
//		SelectFrameBoxWrite( scr, 	word_table[BMPWIN_NONE_SELECT_WIN][0]-1, 
//									word_table[BMPWIN_NONE_SELECT_WIN][1]-1, SELECT_BOX_N_TEXT );
		WorldTrade_Input_SysPrint( wk->BgWinFrm, wk->MenuBmp[BMPWIN_NONE_SELECT_WIN], 
									strbuf,  2, PRINTSYS_LSB_Make(15,14,2), &wk->print );
		GFL_STR_DeleteBuffer(strbuf);
	}

	BGWINFRM_FramePut( wk->BgWinFrm, 0, x, 0 );
	
	BGWINFRM_FrameOn( wk->BgWinFrm, 0 );	

}

//------------------------------------------------------------------
/**
 * @brief   �q�����������͉�ʏ������̃T�u����
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void SexSelect_Init( WORLDTRADE_INPUT_WORK *wk )
{
	u16 *scr;

	// �f�t�H���g�X�N���[�������z�ʂɓǂݍ���
	BGWINFRM_FrameSetArc( wk->BgWinFrm, 0, ARCID_WORLDTRADE_GRA, NARC_worldtrade_search_seibetsu_lz_nscr, 1 );
	scr = BGWINFRM_FrameBufGet( wk->BgWinFrm, 0 );

	// BMPWIN�m��
	select_bmpwin_add( wk, MODE_SEX );

//	SelectFrameBoxWrite( scr, word_table[10][0]-1, word_table[10][1]-1, SELECT_BOX_END );

	// ������`��
	{
		int i;
		STRBUF *strbuf;
		for(i=0;i<MODE_SEX_WIN_NUM;i++){
			strbuf = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_11_001+i );
			WorldTrade_Input_SysPrint( wk->BgWinFrm, wk->MenuBmp[i], strbuf,  2, PRINTSYS_LSB_Make(15,14,2), &wk->print );
			GFL_STR_DeleteBuffer(strbuf);
		}
	}

	// ���ǂ�
	touch_print( wk->BgWinFrm, wk->MsgManager, wk->MenuBmp[BMPWIN_SELECT_END_WIN], msg_gtc_03_009, &wk->print );

	BGWINFRM_FramePut( wk->BgWinFrm, 0, 32, 0 );
	BGWINFRM_FrameOn( wk->BgWinFrm, 0 );	

	BGWINFRM_MoveInit( wk->BgWinFrm, 0, -4, 0, 4 );

}

//------------------------------------------------------------------
/**
 * @brief   ���x�������I��BMPWIN�m��
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void LevelSelect_Init( WORLDTRADE_INPUT_WORK *wk )
{
	u16 *scr;
	BGWINFRM_FrameSetArc( wk->BgWinFrm, 0, ARCID_WORLDTRADE_GRA, NARC_worldtrade_search_level_lz_nscr, 1 );
	scr = BGWINFRM_FrameBufGet( wk->BgWinFrm, 0 );

	// �q�����������͗pBMPWIN�̊m��
	select_bmpwin_add( wk, MODE_LEVEL );

//	SelectFrameBoxWrite( scr, 9, 15, SELECT_BOX_END );
	
	if(wk->Situation==SITUATION_SEARCH){
		wk->listMax  = WorldTrade_LevelListAdd( &wk->NameList, wk->MsgManager, LEVEL_PRINT_TBL_SEARCH);
	}else if(wk->Situation==SITUATION_DEPOSIT){
		wk->listMax  = WorldTrade_LevelListAdd( &wk->NameList, wk->MsgManager, LEVEL_PRINT_TBL_DEPOSIT);
	}

	PokeName_ListPrint( wk, 0, wk->listMax );
	wk->page    = 0;
	
	PagePrint(  wk->BgWinFrm, wk->NumFontSys, wk->MenuBmp[MODE_LEVEL_WIN_NUM], 
				wk->page, _list_page_num( wk->listMax, MODE_LEVEL_WIN_NUM ) );

	// ���ǂ�
	touch_print( wk->BgWinFrm, wk->MsgManager, wk->MenuBmp[BMPWIN_SELECT_END_WIN], msg_gtc_03_009, &wk->print );


	BGWINFRM_FramePut( wk->BgWinFrm, 0, 32, 0 );
	GFL_CLACT_WK_SetAnmSeq(wk->CursorAct, 6);
	
	wk->listpos = 0;
	wk->listpos_backup_x = 0;
	
	WorldTrade_CLACT_PosChange( wk->CursorAct, 
				namepostable[wk->listpos][0], namepostable[wk->listpos][1] );

	BGWINFRM_FrameOn( wk->BgWinFrm, 0 );	
	BGWINFRM_MoveInit( wk->BgWinFrm, 0, -4, 0, 4 );

	wk->seq  = WI_SEQ_LEVEL_MAIN;


}


//==============================================================================
/**
 * @brief   BMPWIN���̕\���ʒu���w�肵��FONT_SYSTEM�Ńv�����g(CGX�]���̂݁j
 *
 * @param   win		
 * @param   strbuf		
 * @param   x		
 * @param   color		
 *
 * @retval  none		
 */
//==============================================================================
void WorldTrade_Input_SysPrint( BGWINFRM_WORK *wfwk, GFL_BMPWIN *win, STRBUF *strbuf, int x, PRINTSYS_LSB color, WT_PRINT *print )
{
	GF_STR_PrintColor( win, FONT_SYSTEM, strbuf, x, 0, MSG_NO_PUT, color,print);
	GFL_BMPWIN_TransVramCharacter( win );
	BGWINFRM_BmpWinOn( wfwk, 0, win );
}


static const u8 box_table[][9]={
	{ 16*0+7, 16*0+9, 16*0+10,  16*0+12, 16*0+8, 16*0+11,},
	{ 16*1+0, 16*1+2, 16*1+3,   16*1+5,  16*1+1, 16*1+4, },
	{ 16*1+6, 16*1+8, 16*1+12,  16*1+14, 16*1+7, 16*1+13, 16*1+9, 16*1+11, 16*1+10},
};

#define SUB_WIN_W			( 32 )
#define SELECT_TEXT_W		( 14 )
#define SELECT_NATION_W		( 28 )
#define SELECT_END_W		(  9 )

//------------------------------------------------------------------
/**
 * @brief   �X�N���[���f�[�^��32x20�Ō��߂����Ƃ��ĕ������͗p���H
 *
 * @param   scr		�X�N���[���o�b�t�@
 * @param   x		�������݊J�nX���W
 * @param   y		�������݊J�nY���W
 * @param   type	�������݃{�b�N�X�^�C�v
 *
 * @retval  none	
 */
//------------------------------------------------------------------
static void SelectFrameBoxWrite( u16 *scr, int x, int y, int type )
{
	int index=0;
	int i;
	switch(type){

	// �P�������̃{�b�N�X
	case SELECT_BOX_N_LETTER:	case SELECT_BOX_R_LETTER:
		// �P�������̃{�b�N�X
		scr[  x+y*SUB_WIN_W]     = box_table[type][0]+0x1000;
		scr[1+x+y*SUB_WIN_W]     = box_table[type][1]+0x1000;
		scr[x+(y+1)*SUB_WIN_W]   = box_table[type][2]+0x1000;
		scr[1+x+(y+1)*SUB_WIN_W] = box_table[type][3]+0x1000;
		break;

	// ������\���̃{�b�N�X
	case SELECT_BOX_N_TEXT:     case SELECT_BOX_R_TEXT:
		index = type-SELECT_BOX_N_TEXT;

		// �l���𖄂߂�
		scr[  x+y*SUB_WIN_W]     			   = box_table[index][0]+0x1000;
		scr[SELECT_TEXT_W+x+y*SUB_WIN_W-1]     = box_table[index][1]+0x1000;
		scr[x+(y+1)*SUB_WIN_W]   			   = box_table[index][2]+0x1000;
		scr[SELECT_TEXT_W+x+(y+1)*SUB_WIN_W-1] = box_table[index][3]+0x1000;
		// �P��̒��������ɖ��߂�
		for(i=1;i<SELECT_TEXT_W-1;i++){
			scr[x+y*SUB_WIN_W+i]     = box_table[index][4]+0x1000;
			scr[x+(y+1)*SUB_WIN_W+i] = box_table[index][5]+0x1000;
			
		}
		break;

	// �����\���̃{�b�N�X
	case SELECT_BOX_N_NATION:	case SELECT_BOX_R_NATION:
		index = type - SELECT_BOX_N_NATION;
		// �l���𖄂߂�
		scr[  x+y*SUB_WIN_W]     			     = box_table[index][0]+0x1000;
		scr[SELECT_NATION_W+x+y*SUB_WIN_W-1]     = box_table[index][1]+0x1000;
		scr[x+(y+1)*SUB_WIN_W]   			     = box_table[index][2]+0x1000;
		scr[SELECT_NATION_W+x+(y+1)*SUB_WIN_W-1] = box_table[index][3]+0x1000;
		// �P��̒��������ɖ��߂�
		for(i=1;i<SELECT_NATION_W-1;i++){
			scr[x+y*SUB_WIN_W+i]     = box_table[index][4]+0x1000;
			scr[x+(y+1)*SUB_WIN_W+i] = box_table[index][5]+0x1000;
			
		}
		break;

	// �߂�{�^��
	case SELECT_BOX_END:
		index = 2;
		// �l��
		scr[  x+y*SUB_WIN_W]     			   = box_table[index][0]+0x1000;
		scr[SELECT_END_W+x+y*SUB_WIN_W-1]      = box_table[index][1]+0x1000;
		scr[x+(y+3)*SUB_WIN_W]   			   = box_table[index][2]+0x1000;
		scr[SELECT_END_W+x+(y+3)*SUB_WIN_W-1]  = box_table[index][3]+0x1000;
		// ���[�ƉE�[�̏c��
		scr[  x+(y+1)*SUB_WIN_W]     		   = box_table[index][6]+0x1000;
		scr[SELECT_END_W+x+(y+1)*SUB_WIN_W-1]  = box_table[index][7]+0x1000;
		scr[x+(y+2)*SUB_WIN_W]   			   = box_table[index][6]+0x1000;
		scr[SELECT_END_W+x+(y+2)*SUB_WIN_W-1]  = box_table[index][7]+0x1000;

		for(i=1;i<SELECT_END_W-1;i++){
			scr[x+y*SUB_WIN_W+i]    		 = box_table[index][4]+0x1000;
			scr[x+(y+3)*SUB_WIN_W+i]		 = box_table[index][5]+0x1000;
			scr[x+(y+1)*SUB_WIN_W+i]		 = box_table[index][8]+0x1000;
			scr[x+(y+2)*SUB_WIN_W+i]		 = box_table[index][8]+0x1000;
		}


		break;

	}
}


static int	wi_seq_none( WORLDTRADE_INPUT_WORK *wk )
{

	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   ���̓E�C���h�E�C�����Ď������̃V�[�P���X��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_winin( WORLDTRADE_INPUT_WORK *wk )
{
	// �t���[���C���҂�
	if(BGWINFRM_MoveCheck( wk->BgWinFrm, 0)==0){
		// �J�[�\���\���J�n

    if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
    { 
      GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );
    }
    else
    { 
      GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    }


		if(wk->type==MODE_LEVEL){

      GFL_CLACT_WK_SetAutoAnmFlag( wk->ArrowAct[0], 0 );
      GFL_CLACT_WK_SetAutoAnmFlag( wk->ArrowAct[1], 0 );
      GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[0], CELL_BOXARROW_NO );
      GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[1], CELL_BOXARROW_NO+1 );
			WorldTrade_CLACT_PosChange( wk->ArrowAct[0], PAGE_ARROW2_X, PAGE_ARROW_Y);
			WorldTrade_CLACT_PosChange( wk->ArrowAct[1], PAGE_ARROW1_X, PAGE_ARROW_Y);
			GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[0], 1 );
			GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[1], 1 );
		}
		wk->seq  = wk->next;
		MORI_PRINT("input win in\n");
	}

	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   ���̓E�C���h�E�C�����Ď������̃V�[�P���X��(�e���Ȃ��j
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_winwait( WORLDTRADE_INPUT_WORK *wk )
{
	// �t���[���C���҂�
	if(BGWINFRM_MoveCheck( wk->BgWinFrm, 0)==0){
		wk->seq  = wk->next;
		MORI_PRINT("input win no effectin\n");
	}

	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   ���̓E�C���h�E�A�E�g���Ď����A���̃V�[�P���X��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_winout( WORLDTRADE_INPUT_WORK *wk )
{
  //�L�[�A�j���҂�
  if( GFL_CLACT_WK_CheckAnmActive( wk->CursorAct ) )
  { 
    return BMPMENU_NULL;
  }

	// �J�[�\���\��OFF�ɂ���
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );		
  GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[0], 0 );
  GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[1], 0 );

  //���̓E�B���h�E����
  if( wk->type == MODE_NATION && wk->next == WI_SEQ_NATION_CANCEL_EXIT )
  { 
    //�������C�������肵�Ė߂�Ƃ����������߂�
    BGWINFRM_MoveInit( wk->BgWinFrm, 0, 6, 0, 6 );
  }
  else
  { 
    BGWINFRM_MoveInit( wk->BgWinFrm, 0, 4, 0, 4 );
  }

  wk->seq  = WI_SEQ_WINOUTWAIT;

	return BMPMENU_NULL;

}

//------------------------------------------------------------------
/**
 * @brief   ���̓E�C���h�E�A�E�g���Ď����A���̃V�[�P���X��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_winoutwait( WORLDTRADE_INPUT_WORK *wk )
{ 
	// �t���[���A�E�g�҂�
	if(BGWINFRM_MoveCheck( wk->BgWinFrm, 0)==0){
		BGWINFRM_FrameOff( wk->BgWinFrm, 0 );
		MORI_PRINT("input win out wait\n");
		wk->seq  = wk->next;
	}
	return BMPMENU_NULL;
}
//------------------------------------------------------------------
/**
 * @brief   ��������߂�Ƃ�
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_winwait_return( WORLDTRADE_INPUT_WORK *wk )
{ 
  //�L�[�A�j���҂�
  if( GFL_CLACT_WK_CheckAnmActive( wk->CursorAct ) )
  { 
    return BMPMENU_NULL;
  }

	// �J�[�\���\��OFF�ɂ���
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );		
  GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[0], 0 );
  GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[1], 0 );

  BGWINFRM_MoveInit( wk->BgWinFrm, 0, 4, 0, 4 );

  wk->seq  = WI_SEQ_WINWAIT;

	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   �q�����������͉�ʂ̏�����
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_head1_init( WORLDTRADE_INPUT_WORK *wk )
{
	HeadWord1_Init( wk, MODE_HEADWORD_1, HEAD1_MOVE_INIT_X );
	GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 4 );
	WorldTrade_CLACT_PosChange( wk->CursorAct, 
						(word_cur_table[0][0]+16)*8,
						word_cur_table[0][1]*8 );

	BGWINFRM_MoveInit( wk->BgWinFrm, 0, -4, 0, 4 );
	wk->seq   = WI_SEQ_WININ;
	wk->next  = WI_SEQ_HEAD1_MAIN;
	wk->Head1 = -1;

	MORI_PRINT("input head1 init\n");

	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   �q�����������͉�ʃ��C��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_head1_main( WORLDTRADE_INPUT_WORK *wk )
{
	int i;
	u32 ret;

	switch((ret=WordHead_SelectMain( wk, wk->see_check ))){
	case INPUT_HEAD_A:	case INPUT_HEAD_KA:	case INPUT_HEAD_SA:	case INPUT_HEAD_TA:	case INPUT_HEAD_NA:
	case INPUT_HEAD_HA:	case INPUT_HEAD_MA: case INPUT_HEAD_YA: case INPUT_HEAD_RA: case INPUT_HEAD_WA:

		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );

		wk->seq  = WI_SEQ_HEAD1_EXIT;
		wk->Head1 = ret;
		// ���͂��s��
		break;
	// �L�����Z��
	case BMPMENU_CANCEL:
//		BmpWinFrame_Clear( &wk->MsgWin, WINDOW_TRANS_ON );

		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );

		wk->seq  = WI_SEQ_WINOUT;
		wk->next = WI_SEQ_HEAD1_EXIT;
		wk->Head1 = -1;

		// �L�����Z���Ȃ̂œ��͂�����
		break;

	}
	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   �q�����������͉�ʏI��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_head1_exit( WORLDTRADE_INPUT_WORK *wk )
{
  //�L�[�A�j���҂�
  if( GFL_CLACT_WK_CheckAnmActive( wk->CursorAct ) )
  { 
    return BMPMENU_NULL;
  }

	// �q�����������͗pBMPWIN�̉��
	select_bmpwin_del( wk, MODE_HEADWORD_1 );

	// �J�[�\���\��OFF
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );

	// ���͂��Ă��邩���Ă��Ȃ����ŕ���
	if(wk->Head1 >= 0){
		if(wk->Head1!=INPUT_HEAD_WA){
			wk->seq = WI_SEQ_HEAD2_INIT;
		}else{
			wk->Head2 = 0;
			wk->seq   = WI_SEQ_POKENAME_INIT;
		}
		MORI_PRINT("input head1 exit ---> input next Head2(Head1=%d) \n", wk->Head1);
	}else{
		MORI_PRINT("input head1 exit ---> input end (Head1=%d) \n", wk->Head1);
		return BMPMENU_CANCEL;
	}
	
	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   HEAD2����߂��Ă����Ƃ��̏�����
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_head1_return( WORLDTRADE_INPUT_WORK *wk )
{
	u16 *scr;
	BGWINFRM_FrameSetArc( wk->BgWinFrm, 0, ARCID_WORLDTRADE_GRA, NARC_worldtrade_search_name3_lz_nscr, 1 );
	scr = BGWINFRM_FrameBufGet( wk->BgWinFrm, 0 );
	select_bmpwin_add( wk, MODE_HEADWORD_1 );

	// ���ǂ�
	touch_print( wk->BgWinFrm, wk->MsgManager, wk->MenuBmp[BMPWIN_SELECT_END_WIN], msg_gtc_03_009, &wk->print );

	// �A�J�T�^�i�n�}��������������
	{
    int i;
		STRBUF *strbuf;
		PRINTSYS_LSB color;
		
    MI_CpuFill8(wk->see_check, TRUE, SEE_CHECK_MAX);
    for(i=0;i<MODE_HEADWORD1_WIN_NUM;i++){
      strbuf = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_10_001+i );
      if(PokeSeeCount_Category(wk, i) == TRUE){
        color = SEARCH_MOJI_COLOR_NORMAL;
        wk->see_check[i] = TRUE;
      }
      else{
        color = SEARCH_MOJI_COLOR_GRAY;
        wk->see_check[i] = FALSE;
      }

      WorldTrade_Input_SysPrint( wk->BgWinFrm, wk->MenuBmp[i], strbuf,  2, color, &wk->print );
      GFL_STR_DeleteBuffer(strbuf);
    }
	}

	BGWINFRM_FramePut( wk->BgWinFrm, 0, HEAD1_REWRITE_INIT_X, 0 );
	BGWINFRM_FrameOn( wk->BgWinFrm, 0 );	

	GFL_CLACT_WK_SetAnmSeq(wk->CursorAct, 4);
	WorldTrade_CLACT_PosChange( wk->CursorAct, 
						(word_cur_table[wk->Head1][0]+16)*8,
						word_cur_table[wk->Head1][1]*8 );
	wk->listpos = wk->Head1;
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );

//	GFL_HEAP_FreeMemory( buf );
	wk->seq = WI_SEQ_HEAD1_MAIN;

  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
  }

	OS_Printf("input head1 return -----\n");
	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   �ꉹ���������͏�����
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_head2_init( WORLDTRADE_INPUT_WORK *wk )
{
	u16 *scr;
	BGWINFRM_FrameSetArc( wk->BgWinFrm, 0, ARCID_WORLDTRADE_GRA, NARC_worldtrade_search_name1_lz_nscr, 1 );
	scr = BGWINFRM_FrameBufGet( wk->BgWinFrm, 0 );
	
	select_bmpwin_add( wk, MODE_HEADWORD_2 );
	{
		int i;
		for(i=0;i<MODE_HEADWORD2_WIN_NUM;i++){
//			SelectFrameBoxWrite( scr, word2_table[i][0], word2_table[i][1], word2_table[i][2] );
		}
	}
//	SelectFrameBoxWrite( scr, 9, 15, SELECT_BOX_END );
	
	{
		int i;
    	int see_count;
        PRINTSYS_LSB color;
        
        MI_CpuFill8(wk->see_check, TRUE, SEE_CHECK_MAX);
		system_print( wk->BgWinFrm, wk->MsgManager, wk->MenuBmp[0], 
				  head_letter_table[head2_word_start[wk->Head1][0]], SEARCH_MOJI_COLOR_NORMAL, &wk->print );
		for(i=1;i<MODE_HEADWORD2_WIN_NUM;i++){
        	see_count = PokeSeeCount_Siin(wk, head2pokename[wk->Head1]+i-1);
            if(see_count > 0){
                color = SEARCH_MOJI_COLOR_NORMAL;
                wk->see_check[head2pokename[wk->Head1]+i-1] = TRUE;
            }
            else{
                color = SEARCH_MOJI_COLOR_GRAY;
                wk->see_check[head2pokename[wk->Head1]+i-1] = FALSE;
            }
			system_print( wk->BgWinFrm,wk->MsgManager, wk->MenuBmp[i], 
				  head_letter_table[head2_word_start[wk->Head1][0]+i-1], color, &wk->print);
		}
	}
	touch_print( wk->BgWinFrm,wk->MsgManager, wk->MenuBmp[BMPWIN_SELECT_END_WIN], msg_gtc_03_009, &wk->print);

	BGWINFRM_FramePut( wk->BgWinFrm, 0, HEAD1_REWRITE_INIT_X, 0 );
	GFL_CLACT_WK_SetAnmSeq(wk->CursorAct, 4);
	
	if(wk->Head2<0){
		wk->listpos = 0;
	}else{
		wk->listpos = wk->Head2;
	}
	WorldTrade_CLACT_PosChange( wk->CursorAct, 
				(word_cur_table[wk->listpos][0]+16)*8,
				 word_cur_table[wk->listpos][1]*8 );
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
  }

	wk->seq  = WI_SEQ_HEAD2_MAIN;

	MORI_PRINT("input head2 init\n");

	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   �ꉹ�I�����C��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_head2_main( WORLDTRADE_INPUT_WORK *wk )
{
	u32 ret;
	
	switch((ret=WordHead2_SelectMain( wk, wk->see_check ))){
	case 0:	case 1: case 2: case 3: case 4:
	
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );

		wk->seq  = WI_SEQ_HEAD2_EXIT;
		wk->Head2 = ret;
		break;
	case BMPMENU_CANCEL:
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );

		wk->seq  = WI_SEQ_HEAD2_EXIT;
//		wk->next  = WI_SEQ_HEAD1_RETURN;
		wk->Head2 = -1;
		break;
	}

	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   �ꉹ�I������BMP���
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_head2_exit( WORLDTRADE_INPUT_WORK *wk )
{
  //�L�[�A�j���҂�
  if( GFL_CLACT_WK_CheckAnmActive( wk->CursorAct ) )
  { 
    return BMPMENU_NULL;
  }	// �J�[�\���\��OFF
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );

	// �q�����������͗pBMPWIN�̉��
	select_bmpwin_del( wk, MODE_HEADWORD_2 );

	// ���͂��Ă��邩���Ă��Ȃ����ŕ���
	if(wk->Head2 >= 0){
		wk->seq = WI_SEQ_POKENAME_INIT;
		MORI_PRINT("input head2 exit ---> input return (Head2=%d) \n", wk->Head2);
	}else{
		wk->seq = WI_SEQ_HEAD1_RETURN;
		MORI_PRINT("input head1 return ---> input next Name(Head2=%d) \n", wk->Head2);
	}

	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   �ꉹ�I���������A
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_head2_return( WORLDTRADE_INPUT_WORK *wk )
{
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
  }
	wk->seq  = WI_SEQ_HEAD2_INIT;

	MORI_PRINT("input head2 return\n");
	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   �w��̃|�P�������͉��Α��݂��Ă��邩�H
 *
 * @param   sinou		
 * @param   zukan		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int PokeName_GetSortNum( u8 *sinou, ZUKAN_SAVEDATA *zukan, int pokenum, u16 *sortlist, int start, int end )
{
	int i, count;
	for(i=start,count=0;i<end;i++){
		if(ZUKANSAVE_GetPokeSeeFlag( zukan, sortlist[i] )){
			OS_Printf(" SeeCheck i = %d, table[index+i] = %d\n", i, sortlist[i]);
			count++;
		}
	}
	return count;
}
//------------------------------------------------------------------
/**
 * @brief   
 *
 * @param   list			�|�P�����̖��O��o�^����BMPLIST_DATA�̃|�C���^
 * @param   monsnameman		�|�P�����̖��O��MSGMAN
 * @param   msgman			GTS��MSGMAN
 * @param   sinou			�V���I�E������̃e�[�u��
 * @param   num				�擾���閼�O�̓�����
 * @param   zukan			�����񃏁[�N�ւ̃|�C���^
 *
 * @retval  int		�o�^�������O�̐�
 */
//------------------------------------------------------------------
static int PokeName_MakeSortList( BMPLIST_DATA **list, GFL_MSGDATA *monsnameman, 
									GFL_MSGDATA *msgman, u8 *sinou, int select, ZUKAN_SAVEDATA *zukan )
{
	int i,index,see_count=0;
	int pokenum;
  int start, end;
	u16 *sortlist = WorldTrade_ZukanSortDataGet( HEAPID_WORLDTRADE, 0, &pokenum );
  WorldTrade_ZukanSortDataGetOnlyIndex( select, &start, &end );

  pokenum = end - start;
	OS_TPrintf("select = %d, num = %d\n",select, pokenum);
	
	// �}�ӂƔ�ׂĉ��̌��Ă��邩���m�F
	see_count = PokeName_GetSortNum( sinou, zukan, pokenum, sortlist, start ,end );

	// ���ڃ��X�g�̍쐬
	*list = BmpMenuWork_ListCreate( see_count+1, HEAPID_WORLDTRADE );


	// �|�P�������̓o�^
	for(i=start;i<end;i++){
		if(ZUKANSAVE_GetPokeSeeFlag( zukan, sortlist[i] )){
			OS_Printf(" ListAdd i = %d, table[index+i] = %d\n", i, sortlist[i]);
			BmpMenuWork_ListAddArchiveString( *list, monsnameman, sortlist[i], sortlist[i], HEAPID_WORLDTRADE );
		}
	}

	OS_Printf("----------------------list_print\n");
	for(i=0;i<see_count;i++){
		OS_Printf("%02d: %d\n", i, (*list)[i].param);
	}

	GFL_HEAP_FreeMemory( sortlist );

	return see_count;
}




//------------------------------------------------------------------
/**
 * @brief   �|�P���������y�[�W���ɕ\������
 *
 * @param   wk		WORLDTRADE_INPUT_WORK
 * @param   page	�\���y�[�W�̎w��
 * @param   max		�|�P�������̍ő吔
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void PokeName_ListPrint( WORLDTRADE_INPUT_WORK *wk, int page, int max )
{
	int i;
	
	for(i=0;i<POKENAME_1PAGE_MAX;i++){
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
		if(page*POKENAME_1PAGE_MAX+i<max){
			WorldTrade_Input_SysPrint( wk->BgWinFrm, wk->MenuBmp[i], 
									   (STRBUF*)wk->NameList[page*POKENAME_1PAGE_MAX+i].str,  
									   0, PRINTSYS_LSB_Make(15,14,2),&wk->print );
		}else{
			GFL_BMPWIN_TransVramCharacter( wk->MenuBmp[i] );
			BGWINFRM_BmpWinOn( wk->BgWinFrm, 0, wk->MenuBmp[i] );
		}
	}

}
//------------------------------------------------------------------
/**
 * @brief   �������P�y�[�W���\������
 *
 * @param   wk		
 * @param   page		
 * @param   max		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void Nation_ListPrint( WORLDTRADE_INPUT_WORK *wk, int page, int max )
{
	int i;
	for(i=0;i<NATION_1PAGE_MAX;i++){
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MenuBmp[i]), INPUTPLATE_BASE_COLOR_DATA );
		if(page*NATION_1PAGE_MAX+i<max){
			WorldTrade_Input_SysPrint( wk->BgWinFrm, wk->MenuBmp[i], 
									   (STRBUF*)wk->NameList[page*NATION_1PAGE_MAX+i].str,  
									   0, PRINTSYS_LSB_Make(15,14,2),&wk->print );
		}else{
			GFL_BMPWIN_TransVramCharacter( wk->MenuBmp[i] );
			BGWINFRM_BmpWinOn( wk->BgWinFrm, 0, wk->MenuBmp[i] );
		}
	}
	
}




//------------------------------------------------------------------
/**
 * @brief   �|�P�������I����ʏ�����
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_pokename_init( WORLDTRADE_INPUT_WORK *wk )
{
	u16 *scr;
  u8 page_max;

	BGWINFRM_FrameSetArc( wk->BgWinFrm, 0, ARCID_WORLDTRADE_GRA, NARC_worldtrade_search_level_lz_nscr, 1 );
	scr = BGWINFRM_FrameBufGet( wk->BgWinFrm, 0 );

	// �q�����������͗pBMPWIN�̊m��
	select_bmpwin_add( wk, MODE_POKEMON_NAME );

//	SelectFrameBoxWrite( scr, 9, 15, SELECT_BOX_END );
	
	// �|�P���������X�g�̍쐬
	wk->listMax = PokeName_MakeSortList( &wk->NameList, wk->MonsNameManager, wk->MsgManager, 
							wk->SinouTable, head2pokename[wk->Head1]+wk->Head2, wk->zukan );
	wk->page = 0;

	// ���X�g�\��
	PokeName_ListPrint( wk, 0, wk->listMax );

	// �y�[�W���\��
  page_max = _list_page_num( wk->listMax, MODE_POKENAME_WIN_NUM );
  PagePrint(  wk->BgWinFrm, wk->NumFontSys, wk->MenuBmp[MODE_POKENAME_WIN_NUM], 
      wk->page, page_max );

	// ���ǂ�\��
	touch_print( wk->BgWinFrm, wk->MsgManager, wk->MenuBmp[BMPWIN_SELECT_END_WIN], msg_gtc_03_009, &wk->print );


	// �X�N���[�����f
	BGWINFRM_FramePut( wk->BgWinFrm, 0, HEAD1_REWRITE_INIT_X, 0 );

	// �J�[�\���\��
	GFL_CLACT_WK_SetAnmSeq(wk->CursorAct, 6);
	
	wk->listpos = 0;
	
	// �J�[�\���ʒu�ړ��E���E�ړ��J�[�\���\��
	WorldTrade_CLACT_PosChange( wk->CursorAct, 
				namepostable[wk->listpos][0], namepostable[wk->listpos][1] );
	WorldTrade_CLACT_PosChange( wk->ArrowAct[0], POKE_ARROW2_X, POKE_ARROW_Y);
	WorldTrade_CLACT_PosChange( wk->ArrowAct[1], POKE_ARROW1_X, POKE_ARROW_Y);
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
  }

  if( page_max == 1 )
  {
    GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[0], CELL_BOXARROW_NO_PASSIVE );
    GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[1], CELL_BOXARROW_NO_PASSIVE+1 );
  }
  else
  {
    GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[0], CELL_BOXARROW_NO );
    GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[1], CELL_BOXARROW_NO+1 );
  }

  GFL_CLACT_WK_SetAutoAnmFlag( wk->ArrowAct[0], 0 );
  GFL_CLACT_WK_SetAutoAnmFlag( wk->ArrowAct[1], 0 );
  GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[0], TRUE );
  GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[1], TRUE );

	wk->seq  = WI_SEQ_POKENAME_MAIN;

	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   �|�P�������I��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_pokename_main( WORLDTRADE_INPUT_WORK *wk )
{
	u32 ret;
	switch((ret=PokeName_SelectMain( wk ))){
	case BMPMENU_CANCEL:
    PMSND_PlaySE(SE_CANCEL);

		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );

		wk->seq  = WI_SEQ_POKENAME_CANCEL_EXIT;
		wk->Poke = -1;
		break;
	case BMPMENU_NULL:
		break;
	default:
		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );

		wk->seq  = WI_SEQ_WINOUT;
		wk->next = WI_SEQ_POKENAME_EXIT;
		wk->Poke = ret;
		break;
	}
	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   ���O�I�����L�����Z���Ŕ�����
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_pokename_cancel_exit( WORLDTRADE_INPUT_WORK *wk )
{
	MORI_PRINT("input pokename exit ---> input next Name(Head2=%d) \n", wk->Head2);

  //�L�[�A�j���҂�
  if( GFL_CLACT_WK_CheckAnmActive( wk->CursorAct ) )
  { 
    return BMPMENU_NULL;
  }

	// �J�[�\���\��OFF
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );

	GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[0], 0 );
	GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[1], 0 );
	select_bmpwin_del( wk, MODE_POKEMON_NAME );
	// �|�P���������X�g�̉��
	BmpMenuWork_ListDelete( wk->NameList );

	if(wk->Head1!=INPUT_HEAD_WA){
		wk->seq = WI_SEQ_HEAD2_RETURN;
	}else{
		wk->seq = WI_SEQ_HEAD1_RETURN;
	}
	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   ���O�I�����s��ꂽ�̂ŁA�E�C���h�E���B���ďI��
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int wi_seq_pokename_exit( WORLDTRADE_INPUT_WORK *wk )
{
	int ret=BMPMENU_NULL;

  //�L�[�A�j���҂�
  if( GFL_CLACT_WK_CheckAnmActive( wk->CursorAct ) )
  { 
    return BMPMENU_NULL;
  }

	// �J�[�\���\��OFF
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );

	select_bmpwin_del( wk, MODE_POKEMON_NAME );
	// �|�P���������X�g�̉��
	BmpMenuWork_ListDelete( wk->NameList );

	return wk->Poke;
}

//------------------------------------------------------------------
/**
 * @brief   ���I�𓪕����w��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_nation_head1_init( WORLDTRADE_INPUT_WORK *wk )
{
	HeadWord1_Init( wk, MODE_NATION_HEAD1, HEAD1_MOVE_INIT_X );
	GFL_CLACT_WK_SetAnmSeq(   wk->CursorAct, 4 );


	BGWINFRM_MoveInit( wk->BgWinFrm, 0, -4, 0, 4 );
	wk->seq     = WI_SEQ_WININ;
	wk->next    = WI_SEQ_NATION_HEAD1_MAIN;
	wk->listpos = INPUT_HEAD_NONE;
	wk->Head1   = -1;

	WorldTrade_CLACT_PosChange( wk->CursorAct, 
					(n_word_table[wk->listpos][0]+16)*8,
					 n_word_table[wk->listpos][1]*8 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 6 );
    
	MORI_PRINT("input nation head1 init\n");


	return BMPMENU_NULL;
}
//------------------------------------------------------------------
/**
 * @brief   ���I���P�����ړ��̓��C��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_nation_head1_main( WORLDTRADE_INPUT_WORK *wk )
{
	u32 ret;
	switch((ret=NationHead1_SelectMain(wk, wk->see_check))){
	case BMPMENU_NULL:
		break;
	case BMPMENU_CANCEL:	// �L�����Z��
		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );
		wk->seq   = WI_SEQ_WINOUT;
		wk->next  = WI_SEQ_NATION_HEAD1_EXIT;
		wk->Head1 = -1;
		break;
	case INPUT_HEAD_NONE:	// �u���ɂ��Ȃ��v����͂���
		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );
		wk->seq   = WI_SEQ_WINOUT;
		wk->next  = WI_SEQ_NATION_HEAD1_EXIT;
		wk->Head1 = -2;
		break;
	case INPUT_HEAD_A:	case INPUT_HEAD_KA:	case INPUT_HEAD_SA:	case INPUT_HEAD_TA:	case INPUT_HEAD_NA:
	case INPUT_HEAD_HA:	case INPUT_HEAD_MA: case INPUT_HEAD_YA: case INPUT_HEAD_RA: case INPUT_HEAD_WA:
	default:				// �ʏ�̓���

    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );
		wk->Head1 = ret;
		wk->seq   = WI_SEQ_NATION_HEAD1_EXIT;
	}
	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   �������͓������P�I��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_nation_head1_exit( WORLDTRADE_INPUT_WORK *wk )
{
  //�L�[�A�j���҂�
  if( GFL_CLACT_WK_CheckAnmActive( wk->CursorAct ) )
  { 
    return BMPMENU_NULL;
  }	// �J�[�\���\��OFF
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );

	select_bmpwin_del( wk, MODE_NATION_HEAD1 );
	
	if(wk->Head1<0){
		if(wk->Head1==-1){
			return BMPMENU_CANCEL;	// �L�����Z��
		}else if(wk->Head1==-2){
			return 0;				// �u���ɂ��Ȃ��v��I��
		}
	}else{
		if(wk->Head1!=INPUT_HEAD_WA){
			wk->seq = WI_SEQ_NATION_HEAD2_INIT;
		}else{
			wk->Head2 = 0;
			BGWINFRM_MoveInit( wk->BgWinFrm, 0,  -4, 0, 3 );
			// ������ʂ̃��C���J�[�\�����B��
			if(wk->SearchCursorAct!=NULL){
				GFL_CLACT_WK_SetDrawEnable( wk->SearchCursorAct, 0 );
			}
			GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 7 );
			wk->seq  = WI_SEQ_WINWAIT;
			wk->next = WI_SEQ_NATION_INIT;
		}
	}
	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   �������͓������P�̕��A
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_nation_head1_return( WORLDTRADE_INPUT_WORK *wk )
{
	HeadWord1_Init( wk, MODE_NATION_HEAD1, HEAD1_REWRITE_INIT_X );
	GFL_CLACT_WK_SetAnmSeq(   wk->CursorAct, 4 );
	GFL_CLACT_WK_SetDrawEnable(   wk->CursorAct, 1 );
	wk->listpos = wk->Head1;
	WorldTrade_CLACT_PosChange( wk->CursorAct, 
					(n_word_table[wk->listpos][0]+16)*8,
					 n_word_table[wk->listpos][1]*8 );

	//�C�j�b�g�ɖ߂�Ȃ����炱���Ō�����ʂ̃��C���J�[�\���\��
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
  }

	wk->seq   = WI_SEQ_NATION_HEAD1_MAIN;
	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   ���I���Q�����ڂ̑I����ʏ�����
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_nation_head2_init( WORLDTRADE_INPUT_WORK *wk )
{
	u16 *scr;
	BGWINFRM_FrameSetArc( wk->BgWinFrm, 0, ARCID_WORLDTRADE_GRA, NARC_worldtrade_search_name1_lz_nscr, 1 );
	scr = BGWINFRM_FrameBufGet( wk->BgWinFrm, 0 );
	select_bmpwin_add( wk, MODE_HEADWORD_2 );
	{
		int i;
		for(i=0;i<MODE_HEADWORD2_WIN_NUM;i++){
//			SelectFrameBoxWrite( scr, word2_table[i][0], word2_table[i][1], word2_table[i][2] );
		}
	}
//	SelectFrameBoxWrite( scr, 9, 15, SELECT_BOX_END );
	
	{
		int i;
        MI_CpuFill8(wk->see_check, TRUE, SEE_CHECK_MAX);
		system_print( wk->BgWinFrm, wk->MsgManager, wk->MenuBmp[0], 
				   head_letter_table[head2_word_start[wk->Head1][0]], SEARCH_MOJI_COLOR_NORMAL, &wk->print );
		for(i=1;i<MODE_HEADWORD2_WIN_NUM;i++){
            int nation_num;
            PRINTSYS_LSB color;
            nation_num = NationCount_Siin(wk, head2pokename[wk->Head1]+i-1);
            if(nation_num > 0){
                color = SEARCH_MOJI_COLOR_NORMAL;
                wk->see_check[head2pokename[wk->Head1]+i-1] = TRUE;
            }
            else{
                color = SEARCH_MOJI_COLOR_GRAY;
                wk->see_check[head2pokename[wk->Head1]+i-1] = FALSE;
            }
			system_print( wk->BgWinFrm,wk->MsgManager, wk->MenuBmp[i], 
				  head_letter_table[head2_word_start[wk->Head1][0]+i-1], color, &wk->print);
		}


	}
	touch_print( wk->BgWinFrm,wk->MsgManager, wk->MenuBmp[BMPWIN_SELECT_END_WIN], msg_gtc_03_009, &wk->print);

	BGWINFRM_FramePut( wk->BgWinFrm, 0, HEAD1_REWRITE_INIT_X, 0 );
	GFL_CLACT_WK_SetAnmSeq(wk->CursorAct, 4);
	
	if(wk->Head2<0){
		wk->listpos = 0;
	}else{
		wk->listpos = wk->Head2;
	}
	WorldTrade_CLACT_PosChange( wk->CursorAct, 
				(word_cur_table[wk->listpos][0]+16)*8,
				 word_cur_table[wk->listpos][1]*8 );
  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
  }
	if(wk->SearchCursorAct!=NULL){
		GFL_CLACT_WK_SetDrawEnable( wk->SearchCursorAct, 1 );
	}

	wk->seq    = WI_SEQ_NATION_HEAD2_MAIN;

	MORI_PRINT("input head2 init\n");

	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   �������͓������Q���C��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_nation_head2_main( WORLDTRADE_INPUT_WORK *wk )
{
	u32 ret;
	switch((ret=WordHead2_SelectMain( wk, wk->see_check ))){
	case 0:	case 1: case 2: case 3: case 4:
		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );

		wk->seq  = WI_SEQ_NATION_HEAD2_EXIT;
		wk->Head2 = ret;
		break;
	case BMPMENU_CANCEL:
		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );
		wk->seq  = WI_SEQ_NATION_HEAD2_EXIT;
		wk->Head2 = -1;
		break;
	}

	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   �������Q�ڂ̏I������(�������P�ɖ߂邩�������͂�����j
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_nation_head2_exit( WORLDTRADE_INPUT_WORK *wk )
{
  //�L�[�A�j���҂�
  if( GFL_CLACT_WK_CheckAnmActive( wk->CursorAct ) )
  { 
    return BMPMENU_NULL;
  }	// �J�[�\���\��OFF
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );

	select_bmpwin_del( wk, MODE_HEADWORD_2 );
	
	if(wk->Head2<0){
		wk->seq = WI_SEQ_NATION_HEAD1_RETURN;
	}else{
		BGWINFRM_MoveInit( wk->BgWinFrm, 0,  -4, 0, 3 );
		// ������ʂ̃��C���J�[�\�����B��
		if(wk->SearchCursorAct!=NULL){
			GFL_CLACT_WK_SetDrawEnable( wk->SearchCursorAct, 0 );
		}
		GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 7 );
		wk->seq  = WI_SEQ_WINWAIT;
		wk->next = WI_SEQ_NATION_INIT;
	}
	return BMPMENU_NULL;
	
	
}
//------------------------------------------------------------------
/**
 * @brief   
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_nation_head2_return( WORLDTRADE_INPUT_WORK *wk )
{
	wk->seq     = WI_SEQ_NATION_HEAD2_INIT;
	wk->listpos = wk->Head2;

  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
  }
	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   �������͏�����
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_nation_init( WORLDTRADE_INPUT_WORK *wk )
{
	u16 *scr;
  u8 page_max;

	BGWINFRM_FrameSetArc( wk->BgWinFrm, 0, ARCID_WORLDTRADE_GRA, NARC_worldtrade_search_sub_lz_nscr, 1 );
	scr = BGWINFRM_FrameBufGet( wk->BgWinFrm, 0 );

	// �q�����������͗pBMPWIN�̊m��
	select_bmpwin_add( wk, MODE_NATION );

//	SelectFrameBoxWrite( scr, 22, 15, SELECT_BOX_END );
	
	wk->listMax = WorldTrade_NationSortListMake( &wk->NameList, wk->CountryNameManager, 
												 head2pokename[wk->Head1]+wk->Head2 );
	Nation_ListPrint( wk, 0, wk->listMax );
	wk->page    = 0;

  page_max  = _list_page_num( wk->listMax, MODE_NATION_WIN_NUM );
  PagePrint(  wk->BgWinFrm, wk->NumFontSys, wk->MenuBmp[MODE_NATION_WIN_NUM], 
      wk->page, page_max );

	touch_print( wk->BgWinFrm, wk->MsgManager, wk->MenuBmp[BMPWIN_SELECT_END_WIN], msg_gtc_03_009,&wk->print );

	BGWINFRM_FramePut( wk->BgWinFrm, 0, 1, 0 );
	
	wk->listpos = 0;
	
	WorldTrade_CLACT_PosChange( wk->CursorAct, 
				nationpostable[wk->listpos][0], nationpostable[wk->listpos][1] );

	WorldTrade_CLACT_PosChange( wk->ArrowAct[0], NATION_PAGE_ARROW2_X, NATION_PAGE_ARROW_Y);
	WorldTrade_CLACT_PosChange( wk->ArrowAct[1], NATION_PAGE_ARROW1_X, NATION_PAGE_ARROW_Y);

  if( GFL_UI_CheckTouchOrKey() == GFL_APP_END_TOUCH )
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );
  }
  else
  { 
    GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
  }

  if( page_max == 1 )
  {
    GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[0], CELL_BOXARROW_NO_PASSIVE );
    GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[1], CELL_BOXARROW_NO_PASSIVE+1 );
  }
  else
  {
    GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[0], CELL_BOXARROW_NO );
    GFL_CLACT_WK_SetAnmSeq( wk->ArrowAct[1], CELL_BOXARROW_NO+1 );
  }
  GFL_CLACT_WK_SetAutoAnmFlag( wk->ArrowAct[0], 0 );
  GFL_CLACT_WK_SetAutoAnmFlag( wk->ArrowAct[1], 0 );
  GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[0], TRUE );
  GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[1], TRUE );


	wk->seq  = WI_SEQ_NATION_MAIN;
	

	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   �������̓��C��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_nation_main( WORLDTRADE_INPUT_WORK *wk )
{
	u32 ret;
	switch((ret=Nation_SelectMain(wk))){
	case BMPMENU_CANCEL:
		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );
		wk->seq  = WI_SEQ_WINWAIT_RETURN;
		wk->next = WI_SEQ_NATION_CANCEL_EXIT;
		break;
	case BMPMENU_NULL:
		break;
	default:
		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );

		wk->seq    = WI_SEQ_WINOUT;
		wk->next   = WI_SEQ_NATION_EXIT;
		wk->Nation = ret;
		break;
	}

	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   �������̓L�����Z�����i�������Q�ɖ߂�j
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_nation_cancel_exit( WORLDTRADE_INPUT_WORK *wk )
{
  //�L�[�A�j���҂�
  if( GFL_CLACT_WK_CheckAnmActive( wk->CursorAct ) )
  { 
    return BMPMENU_NULL;
  }	// �J�[�\���\��OFF
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );
  GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[0], 0 );
	GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[1], 0 );

	select_bmpwin_del( wk, MODE_NATION );

	// �������X�g�̉��
	BmpMenuWork_ListDelete( wk->NameList );

	if(wk->Head1!=INPUT_HEAD_WA){
		wk->seq = WI_SEQ_NATION_HEAD2_RETURN;
	}else{
		wk->seq = WI_SEQ_NATION_HEAD1_RETURN;
	}
	return BMPMENU_NULL;
}


//------------------------------------------------------------------
/**
 * @brief   ���͏I��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_nation_exit( WORLDTRADE_INPUT_WORK *wk )
{
  //�L�[�A�j���҂�
  if( GFL_CLACT_WK_CheckAnmActive( wk->CursorAct ) )
  { 
    return BMPMENU_NULL;
  }	// �J�[�\���\��OFF
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );
  GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[0], 0 );
	GFL_CLACT_WK_SetDrawEnable( wk->ArrowAct[1], 0 );

	select_bmpwin_del( wk, MODE_NATION );


	// �������X�g�̉��
	BmpMenuWork_ListDelete( wk->NameList );

	OS_Printf("result nation = %d\n", wk->Nation);	
	return wk->Nation;
	
}

//------------------------------------------------------------------
/**
 * @brief   ���ʏ������͉�ʏ�����
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int	wi_seq_sex_init( WORLDTRADE_INPUT_WORK *wk )
{
	wk->listpos = 0;

	SexSelect_Init( wk );
	GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 6 );
	WorldTrade_CLACT_PosChange( wk->CursorAct, 
								sexpostable[wk->listpos][0],sexpostable[wk->listpos][1] );

	wk->seq     = WI_SEQ_WININ;
	wk->next    = WI_SEQ_SEX_MAIN;

	MORI_PRINT("input sex init\n");
	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   ���ʏ������͉�ʃ��C��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_sex_main( WORLDTRADE_INPUT_WORK *wk )
{
	u32 ret;
	switch((ret=SexSelect_SelectMain( wk ))){
	case BMPMENU_CANCEL:
		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );
		wk->seq  = WI_SEQ_WINOUT;
		wk->next = WI_SEQ_SEX_EXIT;
		wk->Sex  = BMPMENU_CANCEL;
		break;
	case BMPMENU_NULL:
		break;
	default:
		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );

		wk->seq  = WI_SEQ_WINOUT;
		wk->next = WI_SEQ_SEX_EXIT;
		wk->Sex  = ret;
		break;
	}
	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   ���ʏ������͏I��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_sex_exit( WORLDTRADE_INPUT_WORK *wk )
{
  //�L�[�A�j���҂�
  if( GFL_CLACT_WK_CheckAnmActive( wk->CursorAct ) )
  { 
    return BMPMENU_NULL;
  }	// �J�[�\���\��OFF
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );

	select_bmpwin_del( wk, MODE_SEX );
//	BmpMenuWork_ListDelete( wk->NameList );

	return wk->Sex;

}

//------------------------------------------------------------------
/**
 * @brief   ���x���������͏�����
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_level_init( WORLDTRADE_INPUT_WORK *wk )
{
	wk->listpos = 0;

	LevelSelect_Init( wk );
	GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, 6 );
	WorldTrade_CLACT_PosChange( wk->CursorAct, 
								levelpostable[wk->listpos][0],levelpostable[wk->listpos][1] );

	wk->seq     = WI_SEQ_WININ;
	wk->next    = WI_SEQ_LEVEL_MAIN;


	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   ���x���������̓��C��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_level_main( WORLDTRADE_INPUT_WORK *wk )
{

	u32 ret;
	switch((ret=LevelSelect_SelectMain( wk ))){
	case BMPMENU_CANCEL:
		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );
		wk->seq   = WI_SEQ_WINOUT;
		wk->next  = WI_SEQ_LEVEL_EXIT;
		wk->Level = BMPMENU_CANCEL;
		break;
	case BMPMENU_NULL:
		break;
	default:
		GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 1 );
    GFL_CLACT_WK_SetAnmSeq( wk->CursorAct, GFL_CLACT_WK_GetAnmSeq( wk->CursorAct ) + 9 );

		wk->seq   = WI_SEQ_WINOUT;
		wk->next  = WI_SEQ_LEVEL_EXIT;
		wk->Level = wk->NameList[ret+wk->page*MODE_LEVEL_WIN_NUM].param;
		break;
	}
	return BMPMENU_NULL;
}

//------------------------------------------------------------------
/**
 * @brief   ���x���������͏I��
 *
 * @param   wk		
 *
 * @retval  int			
 */
//------------------------------------------------------------------
static int	wi_seq_level_exit( WORLDTRADE_INPUT_WORK *wk )
{
  //�L�[�A�j���҂�
  if( GFL_CLACT_WK_CheckAnmActive( wk->CursorAct ) )
  { 
    return BMPMENU_NULL;
  }	// �J�[�\���\��OFF
	GFL_CLACT_WK_SetDrawEnable( wk->CursorAct, 0 );

	select_bmpwin_del( wk, MODE_LEVEL );
	BmpMenuWork_ListDelete( wk->NameList );

	return wk->Level;
}


//------------------------------------------------------------------
/**
 * @brief   �P���\�����邽�߂����̃v�����g
 *
 * @param   win		
 * @param   mesno		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void system_print( BGWINFRM_WORK *wfwk, GFL_MSGDATA *mm, GFL_BMPWIN *win, int mesno, PRINTSYS_LSB color, WT_PRINT *print )
{
	STRBUF *strbuf;

	strbuf = GFL_MSG_CreateString( mm, mesno );
//	WorldTrade_SysPrint( win, strbuf,  2, 0, 0, PRINTSYS_LSB_Make(15,2,0) );
	WorldTrade_Input_SysPrint( wfwk, win, strbuf,  2, color, print );
	GFL_STR_DeleteBuffer(strbuf);
	
}
//------------------------------------------------------------------
/**
 * @brief   �P���\�����邽�߂����̃v�����g
 *
 * @param   win		
 * @param   mesno		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void touch_print( BGWINFRM_WORK *wfwk, GFL_MSGDATA *mm, GFL_BMPWIN *win, int mesno, WT_PRINT *print )
{
	STRBUF *strbuf;

	strbuf = GFL_MSG_CreateString( mm, mesno );

	GF_STR_PrintColor( win, FONT_TOUCH, strbuf, 0, 0, MSG_NO_PUT, PRINTSYS_LSB_Make(15,14,2),print);
	GFL_BMPWIN_TransVramCharacter( win );
	BGWINFRM_BmpWinOn( wfwk, 0, win );

	GFL_STR_DeleteBuffer(strbuf);
	
}


//------------------------------------------------------------------
/**
 * @brief   �y�[�W����`��
 *
 * @param   wfwk	BGFrameWorkd�\����
 * @param   win		BMPWIN
 * @param   page	���݂̃y�[�W��	0�`
 * @param   max		���ڐ��ő吔	1�`
 *
 * @retval  none	
 */
//------------------------------------------------------------------
static void PagePrint( BGWINFRM_WORK *wfwk, NUMFONT *numfont, GFL_BMPWIN *win, int page, int max )
{
	// �u�y�[�W�� �^ �y�[�WMAX�v�`��
  GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win), INPUTPLATE_PAGE_COLOR_DATA );
  NUMFONT_WriteNumber( numfont, page+1, 2, NUMFONT_MODE_RIGHT, win,  0, 0 );
	NUMFONT_WriteMark(   numfont, NUMFONT_MARK_SLASH, win, 8*2, 0 );
	NUMFONT_WriteNumber( numfont,  max, 2, NUMFONT_MODE_LEFT, win, 8*3, 0 );

	// BGFrame�ɔ��f
	GFL_BMPWIN_TransVramCharacter( win );
	BGWINFRM_BmpWinOn( wfwk, 0, win );


}

//==================================================================
/**
 * �q���w��Ń|�P���������C�����t���O�������Ă��邩�J�E���g
 *
 * @param   wk		
 *
 * @retval  int		�����t���O��
 */
//==================================================================
static int PokeSeeCount_Siin(WORLDTRADE_INPUT_WORK *wk, int siin)
{
	int pokenum, see_count;
  u16 *sortlist;
  int start, end;
    
	sortlist = WorldTrade_ZukanSortDataGet( HEAPID_WORLDTRADE, 0, &pokenum );
  WorldTrade_ZukanSortDataGetOnlyIndex( siin, &start, &end );
  pokenum = end - start;

	see_count = PokeName_GetSortNum( wk->SinouTable, wk->zukan, pokenum, sortlist, start, end );
    GFL_HEAP_FreeMemory(sortlist);
    
    OS_TPrintf("�q�� %d �����t���O��=%d\n", siin, see_count);
    return see_count;
}

//--------------------------------------------------------------
/**
 * �u���������Ȃ͂܂���v�w��Ń|�P�����������t���O�������Ă��邩���ׂ�
 *
 * @param   wk		
 * @param   select
 *
 * @retval  BOOL		TRUE:1�C�ȏア��B�@FALSE:0�C
 */
//--------------------------------------------------------------
static BOOL PokeSeeCount_Category(WORLDTRADE_INPUT_WORK *wk, int select)
{
    int siin_start_index = 0;
    int i;
    
    siin_start_index = head2pokename[select];
    
    if(head2_word_start[select][1] > 0){
        for(i = 0; i < head2_word_start[select][1]; i++){
            if(PokeSeeCount_Siin(wk, siin_start_index + i) > 0){
                return TRUE;
            }
        }
    }
    else{
        if(PokeSeeCount_Siin(wk, siin_start_index) > 0){
            return TRUE;
        }
    }
    return FALSE;
}

//==================================================================
/**
 * �q���w��ō����������݂��邩�J�E���g
 *
 * @param   wk		
 *
 * @retval  int		�����t���O��
 */
//==================================================================
static int NationCount_Siin(WORLDTRADE_INPUT_WORK *wk, int siin)
{
    int nation_num, number, i;
    
    nation_num = WorldTrade_NationSortListNumGet( siin, &number );
    OS_TPrintf("�� �q�� %d �����t���O��=%d\n", siin, nation_num);
    return nation_num;
}

//--------------------------------------------------------------
/**
 * �u���������Ȃ͂܂���v�w��ō����������݂��邩���ׂ�
 *
 * @param   wk		
 * @param   select
 *
 * @retval  BOOL		TRUE:1���ȏ゠��B�@FALSE:0
 */
//--------------------------------------------------------------
static BOOL NationCount_Category(WORLDTRADE_INPUT_WORK *wk, int select)
{
    int siin_start_index = 0;
    int i;
    
    siin_start_index = head2pokename[select];
    
    if(head2_word_start[select][1] > 0){
        for(i = 0; i < head2_word_start[select][1]; i++){
            if(NationCount_Siin(wk, siin_start_index + i) > 0){
                return TRUE;
            }
        }
    }
    else{
        if(NationCount_Siin(wk, siin_start_index) > 0){
            return TRUE;
        }
    }
    return FALSE;
}
