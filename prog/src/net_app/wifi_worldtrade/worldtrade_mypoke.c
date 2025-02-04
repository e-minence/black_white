//============================================================================================
/**
 * @file	worldtrade_mypoke.c
 * @brief	世界交換自分ポケモン閲覧画面
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

#include "worldtrade.naix"			// グラフィックアーカイブ定義

#include "debug/debug_str_conv.h"
#include "system/poke2dgra.h"


//============================================================================================
//	プロトタイプ宣言
//============================================================================================
/*** 関数プロトタイプ ***/
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

// 情報表示用のBMPWINの数
#define INFOW_BMPWIN_MAX	( 14 )

//============================================================================================
//	プロセス関数
//============================================================================================

//==============================================================================
/**
 * @brief   世界交換入り口画面初期化
 *
 * @param   wk		
 * @param   seq		
 *
 * @retval  int		
 */
//==============================================================================
int WorldTrade_MyPoke_Init(WORLDTRADE_WORK *wk, int seq)
{

  POKEMON_PARAM *pp = WorldTradeData_GetPokemonDataPtr( wk->param->worldtrade_data );
	
	// ワーク初期化
	InitWork( wk );
	
	// BG設定
	BgInit(  );
	// サブ画面初期化
	WorldTrade_SubLcdBgInit( wk, 0, 0 );

	// BGグラフィック転送
	BgGraphicSet( wk );

	// BMPWIN確保
	BmpWinInit( wk );

	SetCellActor(wk);


	// 自分のポケモンの情報
	WorldTrade_PokeInfoPrint( 	wk->MsgManager, wk->MonsNameManager, wk->WordSet, &wk->InfoWin[INFOWIN_NICKNAME], 
					PP_GetPPPPointer(pp),
					&wk->UploadPokemonData.postSimple, &wk->print );

  {
    WorldTrade_PokeInfoPrint2( wk->MsgManager, &wk->InfoWin[INFOWIN_AZUKETAHITO], 
        (STRCODE*)MyStatus_GetMyName(wk->param->mystatus),
        pp, &wk->InfoWin[INFOWIN_OYA], &wk->print);
  }

	// ほしいポケモンの条件
	WodrldTrade_MyPokeWantPrint( wk->MsgManager, wk->MonsNameManager, wk->WordSet, 
				&wk->InfoWin[INFOWIN_HOSII_POKEMON], 
				wk->UploadPokemonData.wantSimple.characterNo,
				wk->UploadPokemonData.wantSimple.gender,
				WorldTrade_LevelTermGet(wk->UploadPokemonData.wantSimple.level_min,wk->UploadPokemonData.wantSimple.level_max, LEVEL_PRINT_TBL_DEPOSIT),&wk->print);

	// ポケモン画像転送
	WorldTrade_TransPokeGraphic( pp );
	
	// ２回目以降
	// ワイプフェード開始（両画面）
	WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_mypoke.c [152] M ********************\n" );
#endif
	wk->subprocess_seq = SUBSEQ_START;
  wk->sub_display_continue  = FALSE;

	return SEQ_FADEIN;
}
//==============================================================================
/**
 * @brief   世界交換入り口画面メイン
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

	return ret;
}


//==============================================================================
/**
 * @brief   世界交換入り口画面終了
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

	BgExit(  );

	// サブ画面ＢＧ情報解放
	WorldTrade_SubLcdBgExit( wk );

	// 「DSの下画面をみてねアイコン」非表示
	GFL_CLACT_WK_SetDrawEnable( wk->PromptDsActWork, 0 );
	
	WorldTrade_SubProcessUpdate( wk );
	
	return SEQ_INIT;
}


//--------------------------------------------------------------------------------------------
/**
 * BG設定
 *
 * @param	ini		BGLデータ
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
    int sub_visible = GFL_DISP_GetSubVisible(); //GFL_BG_SetBGModeでVisibleフラグが消されるので
		GFL_BG_SetBGMode( &BGsys_data );
		GFL_DISP_GXS_SetVisibleControlDirect(sub_visible);
		GFL_DISP_GXS_SetVisibleControl(
		    GX_PLANEMASK_BG1 | GX_PLANEMASK_BG2 | GX_PLANEMASK_OBJ, VISIBLE_ON);
	}

	// メイン画面テキスト面
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

	// メイン画面メニュー面
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M, TRUE );
	}


	// 情報表示画面テキスト面
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



}

//--------------------------------------------------------------------------------------------
/**
 * BG解放
 *
 * @param	ini		BGLデータ
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgExit( void )
{


	// メイン画面ＢＧ情報解放
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME3_M );
}


//--------------------------------------------------------------------------------------------
/**
 * グラフィックデータセット
 *
 * @param	wk		ポケモンリスト画面のワーク
 *
 * @return	none
 */
//--------------------------------------------------------------------------------------------
static void BgGraphicSet( WORLDTRADE_WORK * wk )
{
	ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_WORLDTRADE_GRA, HEAPID_WORLDTRADE );

	// 上下画面ＢＧパレット転送
	GFL_ARCHDL_UTIL_TransVramPalette(    p_handle, NARC_worldtrade_poke_view_nclr, PALTYPE_MAIN_BG, 0, 16*3*2,  HEAPID_WORLDTRADE);
	
	// 会話フォントパレット転送
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );
  //	TalkFontPaletteLoad( PALTYPE_SUB_BG,  WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

	// 会話ウインドウグラフィック転送
	BmpWinFrame_GraphicSet(	 GFL_BG_FRAME2_M, WORLDTRADE_MESFRAME_CHR, 
						WORLDTRADE_MESFRAME_PAL,  0, HEAPID_WORLDTRADE );

	BmpWinFrame_GraphicSet(	 GFL_BG_FRAME2_M, WORLDTRADE_MENUFRAME_CHR,
						WORLDTRADE_MENUFRAME_PAL, 0, HEAPID_WORLDTRADE );



	// メイン画面BG1キャラ転送
	GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_worldtrade_poke_view_lz_ncgr,  GFL_BG_FRAME1_M, 0, 0, 1, HEAPID_WORLDTRADE);

	// メイン画面BG1スクリーン転送
	GFL_ARCHDL_UTIL_TransVramScreen(   p_handle, NARC_worldtrade_mypoke_lz_nscr,  GFL_BG_FRAME1_M, 0, 32*24*2, 1, HEAPID_WORLDTRADE);


	// 会話フォントパレット転送
	TalkFontPaletteLoad( PALTYPE_SUB_BG, WORLDTRADE_SUB_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

	WorldTrade_SubLcdWinGraphicSet( wk );   // トレードルームウインドウ転送

	GFL_ARC_CloseDataHandle( p_handle );

}


//------------------------------------------------------------------
/**
 * セルアクター登録
 *
 * @param   wk			WORLDTRADE_WORK*
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void SetCellActor(WORLDTRADE_WORK *wk)
{
	//登録情報格納
	GFL_CLWK_DATA add;
//WorldTrade_MakeCLACT( &add,  wk, &wk->clActHeader_main, NNS_G2D_VRAM_TYPE_2DMAIN );

	GFL_STD_MemClear( &add, sizeof(GFL_CLWK_DATA) );

	// 自分のポケモン表示
	add.pos_x = 208;
	add.pos_y = 58;
	wk->PokemonActWork = GFL_CLACT_WK_Create( wk->clactSet,
			wk->resObjTbl[MAIN_LCD][CLACT_U_CHAR_RES],
			wk->resObjTbl[MAIN_LCD][CLACT_U_PLTT_RES], 
			wk->resObjTbl[MAIN_LCD][CLACT_U_CELL_RES],
			&add, CLSYS_DRAW_MAIN, HEAPID_WORLDTRADE );
	GFL_CLACT_WK_SetAutoAnmFlag(wk->PokemonActWork,1);
	GFL_CLACT_WK_SetAnmSeq( wk->PokemonActWork, 36 );
	GFL_CLACT_WK_SetBgPri(wk->PokemonActWork, 1 );
	GFL_CLACT_WK_SetDrawEnable(wk->PokemonActWork, 1 );
	WirelessIconEasy();

}

//------------------------------------------------------------------
/**
 * @brief   表示セルアクターの解放
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
	{   2,  1,  12,  2, },	// ポケモンのニックネーム
	{   8,  4,  8,  2, },	// ポケモンの種族名
	{  11,  1,  4,  2, },	// 「レベル」
	{  14,  1,  4,  2, },	// レベル
	{   1, 10,  6,  2, },	// 「もちもの」
	{   8, 10, 12,  2, },	// 所持アイテム名
	{   1,  4,  6,  2, },	// 「なまえ」
	{   1, 13, 10,  2, },	//「あずけたひと」
	{  12, 13,  8,  2, },	// トレーナー名
	{   1, 16, 12,  2, },	// 「ほしいポケモン」
	{   1, 18, 10,  2, },	// ポケモン種族名
	{  13, 18, 12,  2, },	// レベル指定
	{   1,  7,  4,  2, },	// 「おや」
	{   8,  7,  8,  2, },	// 親名
};

// はい・いいえのBMPWIN領域は最後にもってきたいのだが、
// 情報ウインドウの総数がよめないので、200キャラずらしておく >> 216に変更（この値も適当）
#define YESNO_OFFSET 		   ( SELECT_MENU_OFFSET + SELECT_MENU_SX*SELECT_MENU_SY+216 + 12)

//------------------------------------------------------------------
/**
 * BMPWIN処理（文字パネルにフォント描画）
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BmpWinInit( WORLDTRADE_WORK *wk )
{
	// ---------- メイン画面 ------------------

	wk->MsgWin	= GFL_BMPWIN_Create( GFL_BG_FRAME2_M,
		LINE_TEXT_X, LINE_TEXT_Y, LINE_TEXT_SX, LINE_TEXT_SY, 
		WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );

	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0x0000 );
	GFL_BMPWIN_MakeTransWindow( wk->MsgWin );

	// BMPMENU用の領域がここにある
	wk->MenuWin[0]	= GFL_BMPWIN_Create( GFL_BG_FRAME2_M,
		SELECT_MENU_X, SELECT_MENU_Y, SELECT_MENU_SX, SELECT_MENU_SY, 
		WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );	
	GFL_BMPWIN_MakeTransWindow( wk->MenuWin[0] );

	// BG0面BMPWIN情報ウインドウ確保
	{
		int i;

		for(i=0;i<INFOW_BMPWIN_MAX;i++){
			wk->InfoWin[i] = GFL_BMPWIN_Create( GFL_BG_FRAME3_M,
					info_bmpwin_table[i][0], 
					info_bmpwin_table[i][1], 
					info_bmpwin_table[i][2], 
					info_bmpwin_table[i][3], 
					WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->InfoWin[i]), 0x0000 );
			GFL_BMPWIN_MakeTransWindow( wk->InfoWin[i] );
		}
	}
	
	// サブ画面のGTS説明用BMPWINを確保する
	WorldTrade_SubLcdExpainPut( wk, EXPLAIN_YOUSU );
	

}	

//------------------------------------------------------------------
/**
 * @brief   確保したBMPWINを解放
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void BmpWinDelete( WORLDTRADE_WORK *wk )
{
  //BMPを消す前に残ったストリームを消去
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
 * 世界交換ワーク初期化
 *
 * @param   wk		WORLDTRADE_WORK*
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void InitWork( WORLDTRADE_WORK *wk )
{
	int i;

	// 文字列バッファ作成
	wk->TalkString  = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WORLDTRADE );

	// POKeMON GLOBAL TRADING SYSTEM
	wk->TitleString = GFL_MSG_CreateString( wk->MsgManager, msg_gtc_02_001 );

	for(i=0;i<10;i++){
		wk->InfoString[i]  = GFL_STR_CreateBuffer( INFO_TEXT_WORD_NUM, HEAPID_WORLDTRADE );
	}
}


//------------------------------------------------------------------
/**
 * @brief   ワーク解放
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
 * @brief   サブプロセスシーケンススタート処理
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_Start( WORLDTRADE_WORK *wk)
{
	// GTCへようこそ！
//	SubSeq_MessagePrint( wk, msg_wifilobby_028, 1, 0, 0x0f0f );
//	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
	wk->subprocess_seq = SUBSEQ_MAIN;;


	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   サブプロセスシーケンスメイン
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_Main( WORLDTRADE_WORK *wk)
{
	POKEMON_PARAM *pp = WorldTradeData_GetPokemonDataPtr( wk->param->worldtrade_data );

//	if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE){
//		SubSeq_MessagePrint( wk, msg_wifilobby_009, 1, 0, 0x0f0f );
//		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_YESNO );
//	}

	if((GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE) || (GFL_UI_TP_GetTrg())){
		// 「●●●●をどうしますか？」
		SubSeq_MessagePrint( wk, msg_gtc_01_006, 1, 0, 0x0f0f, pp );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_SELECT_LIST );
		PMSND_PlaySE(WORLDTRADE_DECIDE_SE);
	}else if(GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL){
		wk->subprocess_seq  = SUBSEQ_END;
		WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
		PMSND_PlaySE(SE_CANCEL);
	}
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   サブプロセスシーケンス終了処理
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
 * @brief   引き取りますか？はい・いいえ
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_YesNo( WORLDTRADE_WORK *wk)
{
 	WorldTrade_TouchWinYesNoMakeEx( wk, WORLDTRADE_YESNO_PY1, YESNO_OFFSET, 8, GFL_BG_FRAME2_M, 1 );

	//↑の関数は通常BG0以外をパッシブ化するが、
	//このソースのみ、BG2がメッセージ面なので、
	//一端解放して改めてBG2以外をパッシブかする
	WorldTrade_ClearPassive();
	WorldTrade_SetPassiveMyPoke(1);

	wk->subprocess_seq = SUBSEQ_YESNO_SELECT;

	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   引き取りますか？はい・いいえ選択待ち
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
		// はい→引取りへ
    WorldTrade_TouchDelete( wk );
		wk->subprocess_seq  = SUBSEQ_END;
		wk->sub_out_flg = 1;
		WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_DOWNLOAD );

		// はい・いいえで背景が崩れているので再描画
		WantPokePrintReWrite( wk );

	}else if(ret==WORLDTRADE_RET_NO){
		// いいえ
    WorldTrade_TouchDelete( wk );
		wk->subprocess_seq = SUBSEQ_START;
		BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );

		// はい・いいえで背景が崩れているので再描画
		WantPokePrintReWrite( wk );
	}

/*
	int ret = BmpYesNoSelectMain( wk->YesNoMenuWork, HEAPID_WORLDTRADE );

	if(ret!=BMPMENU_NULL){
		if(ret==BMPMENU_CANCEL){
			// いいえ
			wk->subprocess_seq = SUBSEQ_START;
			BmpWinFrame_Clear( wk->MsgWin, WINDOW_TRANS_ON );

		}else{
			// はい→引取りへ
			wk->subprocess_seq  = SUBSEQ_END;
			wk->sub_out_flg = 1;
			WorldTrade_SubProcessChange( wk, WORLDTRADE_UPLOAD, MODE_DOWNLOAD );
		}

		// はい・いいえで背景が崩れているので再描画
		WantPokePrintReWrite( wk );

	}
*/
	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   選択リスト作成
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SelectList( WORLDTRADE_WORK *wk )
{

	wk->BmpMenuList = BmpMenuWork_ListCreate( 2, HEAPID_WORLDTRADE );
	// ひきとる
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_02_016, 1, HEAPID_WORLDTRADE );
	// そのまま
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_02_017, 2, HEAPID_WORLDTRADE );

	// 選択ボックス呼び出し
	WorldTrade_SelBoxInit( wk, GFL_BG_FRAME2_M, 2, WORLDTRADE_YESNO_PY1 );

	//パッシブ
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

	// ひきとる
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_02_016, 1 );
	// そのまま
	BmpMenuWork_ListAddArchiveString( wk->BmpMenuList, wk->MsgManager, msg_gtc_02_017, 2 );

	temp.menu = wk->BmpMenuList;
	temp.win  = wk->MenuWin[0];

	// 枠描画
	BmpWinFrame_Write( wk->MenuWin[0], WINDOW_TRANS_ON, WORLDTRADE_MENUFRAME_CHR, WORLDTRADE_MENUFRAME_PAL );

	// BMPメニュー開始
	wk->BmpMenuWork = BmpMenuAddEx( &temp, 9, 0, 0, HEAPID_WORLDTRADE, PAD_BUTTON_CANCEL );

*/

	wk->subprocess_seq = SUBSEQ_SELECT_WAIT;
	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   「ひきとる」「そのまま」選択待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SelectWait( WORLDTRADE_WORK *wk )
{
	u32 ret = WorldTrade_SelBoxMain( wk );
	
	if(ret==1){
		// 選択ボックス解放
		WorldTrade_SelBoxEnd( wk );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		{
      POKEMON_PARAM *pp = WorldTradeData_GetPokemonDataPtr( wk->param->worldtrade_data );
			// あずけたポケモンはメールを持っているか？
			if(WorldTrade_PokemonMailCheck( pp )){
				// てもちがいっぱいだと受け取れない
				if(PokeParty_GetPokeCount(wk->param->myparty)==6){
					SubSeq_MessagePrint( wk, msg_gtc_01_037, 1, 0, 0x0f0f, pp );
					WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
					return SEQ_MAIN;
				}
			}
			
			// ●●●をひきとります
			SubSeq_MessagePrint( wk, msg_gtc_01_007, 1, 0, 0x0f0f, pp );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_YESNO );
		}
		WantPokePrintReWrite(wk);
	
	}else if( (ret==2) || (ret==BMPMENULIST_CANCEL) ){
		// 選択ボックス解放
		WorldTrade_SelBoxEnd( wk );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		wk->subprocess_seq  = SUBSEQ_END;
		WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
		WantPokePrintReWrite(wk);
		
	}

/*
	switch(BmpMenuMain( wk->BmpMenuWork )){
	// 「ひきとる」
	case 1:
		BmpMenuExit( wk->BmpMenuWork, NULL );
		BmpMenuWork_ListDelete( wk->BmpMenuList );
		BmpMenuWinClear( wk->MenuWin[0], WINDOW_TRANS_ON );
		{
			POKEMON_PARAM *pp = (POKEMON_PARAM *)wk->UploadPokemonData.postData.data;
			// あずけたポケモンはメールを持っているか？
			if(WorldTrade_PokemonMailCheck( pp )){
				// てもちがいっぱいだと受け取れない
				if(PokeParty_GetPokeCount(wk->param->myparty)==6){
					SubSeq_MessagePrint( wk, msg_gtc_01_037, 1, 0, 0x0f0f, pp );
					WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_MAIN );
					return SEQ_MAIN;
				}
			}
			
			// ●●●をひきとります
			SubSeq_MessagePrint( wk, msg_gtc_01_007, 1, 0, 0x0f0f, pp );
			WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_YESNO );
		}
		WantPokePrintReWrite(wk);
		break;

	// 「そのまま」
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
 * @brief   会話終了を待って次のシーケンスへ
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
 * @brief   メッセージを出して１．５秒待つ
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
 * @brief   会話ウインドウ表示
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void SubSeq_MessagePrint( WORLDTRADE_WORK *wk, int msgno, int wait, int flag, u16 dat, POKEMON_PARAM *pp )
{
	// 文字列取得
	STRBUF *tempbuf;
	
	WORDSET_RegisterPokeMonsName( wk->WordSet, 0, pp );
	tempbuf = GFL_MSG_CreateString(  wk->MsgManager, msgno );
	WORDSET_ExpandStr( wk->WordSet, wk->TalkString, tempbuf );

	// 会話ウインドウ枠描画
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin),  0x0f0f );
	BmpWinFrame_Write( wk->MsgWin, WINDOW_TRANS_ON, WORLDTRADE_MESFRAME_CHR, WORLDTRADE_MESFRAME_PAL );
	GFL_BMPWIN_MakeTransWindow( wk->MsgWin );

	// 文字列描画開始
	GF_STR_PrintSimple( wk->MsgWin, FONT_TALK, wk->TalkString, 0, 0,&wk->print);
	
	GFL_STR_DeleteBuffer(tempbuf);

}


// 性別アイコンを表示するためのカラー指定
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
 * @brief   今から預けようとするポケモンの情報を描画する
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
  BOOL is_sex_visible;
	
	// ニックネーム・性別・レベル・開発ＮＯ・アイテム取得
	PPP_Get(ppp, ID_PARA_nickname, namebuf );
	monsno = post->characterNo;
	sex    = post->gender;
	level  = post->level;
	itemno = PPP_Get( ppp, ID_PARA_item, NULL );
  is_sex_visible  = PPP_Get( ppp, ID_PARA_nidoran_nickname, NULL );
	
	OS_Printf("sex=%d, level=%d nidoran=%d\n",sex, level, is_sex_visible);
	
	// 「もちもの」・性別文字列・ＬＶ．？？？・種族名・アイテム名、文字列生成
	itemlabel  = GFL_MSG_CreateString( MsgManager, msg_gtc_04_004  );
	sexbuf     = GFL_MSG_CreateString( MsgManager, WorldTrade_SexStringTable[sex]  );
	levellabel = GFL_MSG_CreateString( MsgManager, msg_gtc_05_013  );
	WORDSET_RegisterNumber( WordSet, 3, level, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
	levelbuf   = MSGDAT_UTIL_AllocExpandString( WordSet, MsgManager, msg_gtc_05_013_01, HEAPID_WORLDTRADE );
	strbuf     = GFL_MSG_CreateString( MonsNameManager, monsno );
	ITEM_GetItemName( itembuf, itemno, HEAPID_WORLDTRADE );
	namelabel  = GFL_MSG_CreateString( MsgManager, msg_gtc_02_020  );

	// 描画の前にクリア
	for(i=0;i<6;i++){
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(win[i]), 0x0000 );
	}

	// 描画
	WorldTrade_SysPrint( win[0], namebuf,   0, 0, 0, PRINTSYS_LSB_Make(1,2,0),print );
	if(sex!=DPW_TR_GENDER_NONE && is_sex_visible){
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
 * @brief   持ち主情報の描画
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

//通信アイコン分を忘れないように
#define POKEGRA_VRAM_OFFSET	( (16*30+12 + 16)*0x20 )

//------------------------------------------------------------------
/**
 * ポケモン画像を転送する
 *
 * @param   pokeno		POKEMON_PARAM
 *
 * @retval  none
 */
//------------------------------------------------------------------
void WorldTrade_TransPokeGraphic( POKEMON_PARAM *pp )
{
  { 
    void *p_char_buff;
    NNSG2dCharacterData *p_chara;

    p_char_buff	= POKE2DGRA_LoadCharacterPPP( &p_chara, PP_GetPPPPointer(pp), POKEGRA_DIR_FRONT, HEAPID_WORLDTRADE );

    DC_FlushRange( p_chara->pRawData, POKEGRA_POKEMON_CHARA_SIZE );	// 転送前にメモリ安定

    // OAM用VRAMに転送
    GX_LoadOBJ( p_chara->pRawData, POKEGRA_VRAM_OFFSET, POKEGRA_POKEMON_CHARA_SIZE );

    // ワーク解放
    GFL_HEAP_FreeMemory(p_char_buff);
  }

	// パレット転送
  { 
    ARCDATID  pal = POKEGRA_GetPalArcIndex( 
          POKEGRA_GetArcID(), 
          PP_Get( pp, ID_PARA_monsno,	NULL ), 
          PP_Get( pp, ID_PARA_form_no,NULL ),
          PP_Get( pp, ID_PARA_sex,	NULL ),
          PP_CheckRare( pp ),
          POKEGRA_DIR_FRONT,
          FALSE );

    GFL_ARC_UTIL_TransVramPalette( POKEGRA_GetArcID(), pal, PALTYPE_MAIN_OBJ, 0x20*13, POKEGRA_POKEMON_PLT_SIZE, HEAPID_WORLDTRADE );
  }
}


//------------------------------------------------------------------
/**
 * @brief   ほしいポケモンの部分を再描画
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void WantPokePrintReWrite( WORLDTRADE_WORK *wk )
{

	// ほしいポケモンの条件
	WodrldTrade_MyPokeWantPrint( wk->MsgManager, wk->MonsNameManager, wk->WordSet, 
				&wk->InfoWin[INFOWIN_HOSII_POKEMON], 
				wk->UploadPokemonData.wantSimple.characterNo,
				wk->UploadPokemonData.wantSimple.gender,
				WorldTrade_LevelTermGet(wk->UploadPokemonData.wantSimple.level_min,wk->UploadPokemonData.wantSimple.level_max, LEVEL_PRINT_TBL_DEPOSIT),&wk->print);


}

