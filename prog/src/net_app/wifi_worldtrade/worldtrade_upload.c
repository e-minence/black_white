//============================================================================================
/**
 * @file	worldtrade_upload.c
 * @brief	世界交換ポケモンデータアップロードデモ
 * @author	Akito Mori
 * @date	06.05.04
 */
//============================================================================================


#ifdef PM_DEBUG
// 定義すると必ずアップロードで失敗する
//#define DEBUG_UPLOAD_ERROR
#endif

// サーバー確認からダウンロードに流れる時にﾌｪｰﾄﾞｱｳﾄしないようにする版
#define REPAIR_060804

#include <gflib.h>
#include "system/main.h"
#include "system/gfl_use.h"
#include "arc_def.h"
#include <dwc.h>
#include <dpw_tr.h>
#include "print/wordset.h"
#include "message.naix"
#include "system/wipe.h"
#include "pm_version.h"
#include "system/bmp_menu.h"
#include "sound/pm_sndsys.h"
#include "savedata/wifilist.h"
#include "savedata/worldtrade_data.h"
#include "poke_tool/monsno_def.h"
#include "savedata/perapvoice.h"
#include "poke_tool/pokeparty.h"
#include "savedata/box_savedata.h"
#include "item/itemsym.h"

#include "net_app/worldtrade.h"
#include "worldtrade_local.h"

#include "msg/msg_wifi_lobby.h"
#include "msg/msg_wifi_gts.h"
#include "msg/msg_wifi_system.h"

#include "savedata/undata_update.h"
#include "savedata/etc_save.h"
#include "net/dwc_error.h"

#include "worldtrade.naix"			// グラフィックアーカイブ定義
#include "savedata/perapvoice.h"
#include "savedata/mail.h"
#include "item/item.h"

#include "msg/msg_place_name.h"
#define FIRST_NATUKIDO  (70)		///交換されたポケモンに入れるなつき度



//============================================================================================
//	プロトタイプ宣言
//============================================================================================
/*** 関数プロトタイプ ***/
static void BgInit( void );
static void BgExit( void );
static void BgGraphicSet( WORLDTRADE_WORK * wk );
static void BmpWinInit( WORLDTRADE_WORK *wk );
static void BmpWinDelete( WORLDTRADE_WORK *wk );
static void InitWork( WORLDTRADE_WORK *wk );
static void FreeWork( WORLDTRADE_WORK *wk );
static void AfterTradeCheck_ProcessControl( WORLDTRADE_WORK *wk );
static void WifiHistoryDataSet( WIFI_HISTORY *wifiHistory, Dpw_Tr_Data *trData );
static void ExchangePokemonDataAdd( WORLDTRADE_WORK *wk, POKEMON_PARAM *pp, POKEMON_PARAM *sendpp, Dpw_Tr_Data *tr,int boxno );
static void TradeDateUpDate( WORLDTRADE_DATA *worldtrade_data, int trade_type );
static int MyPokemonPocketFullCheck( WORLDTRADE_WORK *wk, Dpw_Tr_Data *trData);
static void PrintError( WORLDTRADE_WORK *wk );
static void SetSaveNextSequence( WORLDTRADE_WORK *wk, int nextSeq1st, int nextSeq2nd );

static int Subseq_Start( WORLDTRADE_WORK *wk);

static int Subseq_EvilCheckStart( WORLDTRADE_WORK *wk );
static int Subseq_EvilCheckResult( WORLDTRADE_WORK *wk );
static int Subseq_NameCheckStart( WORLDTRADE_WORK *wk );
static int Subseq_NameCheckResult( WORLDTRADE_WORK *wk );

static int Subseq_UploadStart( WORLDTRADE_WORK *wk );
static int Subseq_UploadResult( WORLDTRADE_WORK *wk );
static int Subseq_UploadFinish( WORLDTRADE_WORK *wk );
static int Subseq_UploadFinishResult( WORLDTRADE_WORK *wk );
static int Subseq_Main( WORLDTRADE_WORK *wk);
static int Subseq_UploadSuccessMessage( WORLDTRADE_WORK *wk);
static int Subseq_DownloadSuccessMessage( WORLDTRADE_WORK *wk);
static int Subseq_ErrorMessage( WORLDTRADE_WORK *wk );
static int Subseq_EndDpw( WORLDTRADE_WORK *wk );
static int Subseq_ReturnTitleMessage( WORLDTRADE_WORK *wk );
static int Subseq_End( WORLDTRADE_WORK *wk);
static int Subseq_ServerServiceError( WORLDTRADE_WORK *wk );
static int Subseq_ServerServiceEnd( WORLDTRADE_WORK *wk );
static int Subseq_MessageWait( WORLDTRADE_WORK *wk );
static int Subseq_MessageKeyWait( WORLDTRADE_WORK *wk );
static int Subseq_DownloadStart( WORLDTRADE_WORK *wk );
static int Subseq_DownloadResult( WORLDTRADE_WORK *wk );
static int Subseq_DownloadFinish( WORLDTRADE_WORK *wk );
static int Subseq_DownloadFinishResult( WORLDTRADE_WORK *wk );
static int Subseq_ExchangeStart( WORLDTRADE_WORK *wk );			
static int Subseq_ExchangeResult( WORLDTRADE_WORK *wk );
static int Subseq_ExchangeFinish( WORLDTRADE_WORK *wk );
static int Subseq_ExchangeFinishResult( WORLDTRADE_WORK *wk );
static int Subseq_ExchangeSuccessMessage( WORLDTRADE_WORK *wk );
static int Subseq_ExchangeFailedMessage( WORLDTRADE_WORK *wk);
static void UploadPokemonDataDelete( WORLDTRADE_WORK *wk, int flag );
static void DownloadPokemonDataAdd( WORLDTRADE_WORK *wk, POKEMON_PARAM *pp, int boxno, int flag );
static int SubSeq_Save( WORLDTRADE_WORK *wk );
static int SubSeq_NowSaveMessage( WORLDTRADE_WORK *wk );
static int SubSeq_SaveLast( WORLDTRADE_WORK *wk );
static int Subseq_ServerDownloadResult( WORLDTRADE_WORK *wk );
static int Subseq_ServerDownload( WORLDTRADE_WORK *wk );
static int Subseq_ServerTradeCheckResult( WORLDTRADE_WORK *wk );
static int Subseq_ServerTradeCheck( WORLDTRADE_WORK *wk );
static int Subseq_DownloadExStart( WORLDTRADE_WORK *wk );
static int Subseq_DownloadExFinishResult( WORLDTRADE_WORK *wk );
static int Subseq_DownloadExSuccessMessage( WORLDTRADE_WORK *wk );
static int SubSeq_SaveWait( WORLDTRADE_WORK *wk );
static int SubSeq_SaveRandomWait( WORLDTRADE_WORK *wk );
static int Subseq_ServerTradeCheckEnd( WORLDTRADE_WORK *wk );
static int SubSeq_TimeoutSave( WORLDTRADE_WORK *wk );
static int SubSeq_TimeoutSaveWait( WORLDTRADE_WORK *wk );
static int Subseq_DownloadExFinish( WORLDTRADE_WORK *wk );
static void MakeTradeExchangeInfomation( WORLDTRADE_WORK *wk, POKEMON_PARAM *recv_pp, Dpw_Tr_Data *recv_tr, POKEMON_PARAM *send_pp );

static int Subseq_StartCancelAsync( WORLDTRADE_WORK *wk );
static int Subseq_WaitCancelAsync( WORLDTRADE_WORK *wk );
static int Subseq_EndErr( WORLDTRADE_WORK *wk );


#if GTS_DUPLICATE_BUG_FIX

static int Subseq_ServerPokeDelete( WORLDTRADE_WORK *wk);
static int Subseq_ServerPokeDeleteWait( WORLDTRADE_WORK *wk);
static int DupulicateCheck( WORLDTRADE_WORK *wk );

#endif

static void WorldTrade_SetEvilName( POKEMON_PARAM *pp, HEAPID heapID );


enum{
	SUBSEQ_START=0,
	SUBSEQ_MAIN,
  
  SUBSEQ_EVILCHECK_START,
  SUBSEQ_EVILCHECK_RESULT,
  SUBSEQ_NAMECHECK_START,
  SUBSEQ_NAMECHECK_RESULT,

	SUBSEQ_UPLOAD_START,
	SUBSEQ_UPLOAD_RESULT,
	SUBSEQ_UPLOAD_FINISH,
	SUBSEQ_UPLOAD_FINISH_RESULT,
	SUBSEQ_UPLOAD_SUCCESS_MESSAGE,

	SUBSEQ_DOWNLOAD_START,
	SUBSEQ_DOWNLOAD_RESULT,
	SUBSEQ_DOWNLOAD_FINISH,
	SUBSEQ_DOWNLOAD_FINISH_RESULT,
	SUBSEQ_DOWNLOAD_SUCCESS_MESSAGE,

	SUBSEQ_EXCHANGE_START,
	SUBSEQ_EXCHANGE_RESULT,
	SUBSEQ_EXCHANGE_FINISH,
	SUBSEQ_EXCHANGE_FINISH_RESULT,
	SUBSEQ_EXCHANGE_SUCCESS_MESSAGE,
	SUBSEQ_EXCHANGE_FAILED_MESSAGE,

	SUBSEQ_DOWNLOAD_EX_START,
	SUBSEQ_DOWNLOAD_EX_FINISH,
	SUBSEQ_DOWNLOAD_EX_FINISH_RESULT,
	SUBSEQ_DOWNLOAD_EX_SUCCESS_MESSAGE,
	
#if GTS_DUPLICATE_BUG_FIX

	SUBSEQ_SERVER_POKE_DELETE,			// サーバー側ポケモン自動削除シーケンス
	SUBSEQ_SERVER_POKE_DELETE_WAIT,		// サーバー削除コマンドシーケンス待ち

#endif

	SUBSEQ_SERVER_TRADE_CHECK,	
    SUBSEQ_SERVER_TRADECHECK_RESULT,
    SUBSEQ_SERVER_DOWNLOAD,		
    SUBSEQ_SERVER_DOWNLOAD_RESULT,	
	SUBSEQ_SERVER_TRADE_CHECK_END,


	SUBSEQ_NOW_SAVE_MES,
	SUBSEQ_SAVE,
	SUBSEQ_SAVE_RANDOM_WAIT,
	SUBSEQ_SAVE_WAIT,
	SUBSEQ_SAVE_LAST,

	SUBSEQ_TIMEOUT_SAVE,
    SUBSEQ_TIMEOUT_SAVE_WAIT,

	SUBSEQ_END,
	SUBSEQ_MES_WAIT,
  SUBSEQ_MES_KEYWAIT,
	SUBSEQ_ERROR_MESSAGE,
  SUBSEQ_END_DPW,
	
	SUBSEQ_RETURN_TITLE_MESSAGE,

	SUBSEQ_START_CANCEL_ASYNC,
	SUBSEQ_WAIT_CANCEL_ASYNC,
	SUBSEQ_END_ERR,
};


  
  
  
  

static int (*Functable[])( WORLDTRADE_WORK *wk ) = {
	Subseq_Start,				// SUBSEQ_START=0,
	Subseq_Main,             	// SUBSEQ_MAIN,

  Subseq_EvilCheckStart,    //SUBSEQ_EVILCHECK_START,
  Subseq_EvilCheckResult,   //SUBSEQ_EVILCHECK_RESULT,
  Subseq_NameCheckStart,    //SUBSEQ_NAMECHECK_START,
  Subseq_NameCheckResult,   //SUBSEQ_NAMECHECK_RESULT,

	Subseq_UploadStart,			// SUBSEQ_UPLOAD_START,
	Subseq_UploadResult,		// SUBSEQ_UPLOAD_RESULT,
	Subseq_UploadFinish,		// SUBSEQ_UPLOAD_FINISH,
	Subseq_UploadFinishResult,	// SUBSEQ_UPLOAD_FINISH_RESULT,
	Subseq_UploadSuccessMessage,// SUBSEQ_UPLOAD_SUCCESS_MESSAGE

	Subseq_DownloadStart,			// SUBSEQ_DOWNLOAD_START,
	Subseq_DownloadResult,			// SUBSEQ_DOWNLOAD_RESULT,
	Subseq_DownloadFinish,			// SUBSEQ_DOWNLOAD_FINISH,
	Subseq_DownloadFinishResult,	// SUBSEQ_DOWNLOAD_FINISH_RESULT,
	Subseq_DownloadSuccessMessage,	// SUBSEQ_DOWNLOAD_SUCCESS_MESSAGE

	Subseq_ExchangeStart,			// SUBSEQ_EXCHANGE_START,
	Subseq_ExchangeResult,			// SUBSEQ_EXCHANGE_RESULT,
	Subseq_ExchangeFinish,			// SUBSEQ_EXCHANGE_FINISH,
	Subseq_ExchangeFinishResult,	// SUBSEQ_EXCHANGE_FINISH_RESULT,
	Subseq_ExchangeSuccessMessage,	// SUBSEQ_EXCHANGE_SUCCESS_MESSAGE
	Subseq_ExchangeFailedMessage,	// SUBSEQ_EXCHANGE_FAILED_MESSAGE

	Subseq_DownloadExStart,			// SUBSEQ_DOWNLOAD_EX_START
	Subseq_DownloadExFinish,		// SUBSEQ_DOWNLOAD_EX_FINISH
	Subseq_DownloadExFinishResult,	// SUBSEQ_DOWNLOAD_EX_FINISH_RESULT
	Subseq_DownloadExSuccessMessage,// 

#if GTS_DUPLICATE_BUG_FIX

	Subseq_ServerPokeDelete,			//	SUBSEQ_SERVER_POKE_DELETE,
	Subseq_ServerPokeDeleteWait,        //	SUBSEQ_SERVER_POKE_DELETE_WAIT,
#endif


	Subseq_ServerTradeCheck,		// SUBSEQ_SERVER_TRADE_CHECK,	
	Subseq_ServerTradeCheckResult,	// SUBSEQ_SERVER_TRADECHECK_RESULT
	Subseq_ServerDownload,			//SUBSEQ_SERVER_DOWNLOAD 
	Subseq_ServerDownloadResult,		// SUBSEQ_SERVER_DOWNLOAD_RESULT
	Subseq_ServerTradeCheckEnd,		// SUBSEQ_SERVER_TRADE_CHECK_END,	

	SubSeq_NowSaveMessage,			// SUBSEQ_NOW_SAVE_MES,
	SubSeq_Save,					// SUBSEQ_SAVE,
	SubSeq_SaveRandomWait,			// SUBSEQ_SAVE_RANDOM_WAIT
	SubSeq_SaveWait,				// SUBSEQ_SAVE_WAIT,
	SubSeq_SaveLast,				// SUBSEQ_SAVE_LAST

	SubSeq_TimeoutSave,				// SUBSEQ_TIMEOUT_SAVE,
	SubSeq_TimeoutSaveWait,			// SUBSEQ_TIMEOUT_SAVE_WAIT


	Subseq_End,             	// SUBSEQ_END,
	Subseq_MessageWait,     	// SUBSEQ_MES_WAIT
	Subseq_MessageKeyWait,     	// SUBSEQ_MES_KEYWAIT
	Subseq_ErrorMessage,		// SUBSEQ_ERROR_MESSAGE,
	Subseq_EndDpw,		// SUBSEQ_END_DPW,
	Subseq_ReturnTitleMessage,	// SUBSEQ_RETURN_TITLE_MESSAGE,

	Subseq_StartCancelAsync, //SUBSEQ_START_CANCEL_ASYNC,
	Subseq_WaitCancelAsync, //SUBSEQ_WAIT_CANCEL_ASYNC,
	Subseq_EndErr,//SUBSEQ_END_ERR,
};


#define POKEMON_RECV_OK						( 0 )
#define	POKEMON_ALL_FULL					( 1 )
#define POKEMON_NOT_FULL_BUT_MAIL_NORECV	( 2 )

///強制タイムアウトまでの時間
#define TIMEOUT_TIME			(30*60*2)	//2分

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
int WorldTrade_Upload_Init(WORLDTRADE_WORK *wk, int seq)
{
	// ワーク初期化
	InitWork( wk );
	
	// BG設定
	BgInit(  );
  WorldTrade_SubLcdBgInit(  wk, 0, 0 );

  //ようすを見るときは、サブ画面消去
  if( wk->sub_process_mode == MODE_SERVER_CHECK )
  { 
    GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );	//サブ画面OBJ面OFF
    GFL_BG_SetVisible( GFL_BG_FRAME2_S, VISIBLE_OFF );	//サブ画面OBJ面OFF
  }

	// BGグラフィック転送
	BgGraphicSet( wk );

	// BMPWIN確保
	BmpWinInit( wk );

  WorldTrade_SetPartnerExchangePos( wk );

	// ワイプフェード開始
	WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
	OS_Printf( "******************** worldtrade_upload.c [250] M ********************\n" );
#endif

	// Wifi通信アイコン
	WorldTrade_WifiIconAdd( wk );

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
int WorldTrade_Upload_Main(WORLDTRADE_WORK *wk, int seq)
{
	int ret;
  int temp_seq  = wk->subprocess_seq;

	// シーケンス遷移で実行
	ret = (*Functable[wk->subprocess_seq])( wk );

  if(temp_seq != wk->subprocess_seq )
  {
    OS_TPrintf( "SEQ:pre %d now %d\n", temp_seq, wk->subprocess_seq );
  }

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
int WorldTrade_Upload_End(WORLDTRADE_WORK *wk, int seq)
{
    if(GXS_GetMasterBrightness() != 0){
        WorldTrade_SetPartnerExchangePosIsReturns( wk );
    }

	FreeWork( wk );
	
	BmpWinDelete( wk );
	
	BgExit(  );
  WorldTrade_SubLcdBgExit( wk );

	// 次のサブプロセスを設定する
	WorldTrade_SubProcessUpdate( wk );
	
	// 次が設定されていたら初期化へ
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
			GX_DISPMODE_GRAPHICS, GX_BGMODE_0, GX_BGMODE_0, GX_BG0_AS_2D,
		};
    int sub_visible = GFL_DISP_GetSubVisible(); //GFL_BG_SetBGModeでVisibleフラグが消されるので
		GFL_BG_SetBGMode( &BGsys_data );
		GFL_DISP_GXS_SetVisibleControlDirect(sub_visible);
	}

	// メイン画面テキスト面
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf800, GX_BG_CHARBASE_0x00000,GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			0, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME0_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME0_M );
		GFL_BG_SetVisible( GFL_BG_FRAME0_M, TRUE );
	}

	// メイン画面背景面
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xf000, GX_BG_CHARBASE_0x08000,GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME1_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME1_M );
		GFL_BG_SetVisible( GFL_BG_FRAME1_M, TRUE );
	}

	// メイン画面背景面2
	{	
		GFL_BG_BGCNT_HEADER TextBgCntDat = {
			0, 0, 0x800, 0, GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0xe800, GX_BG_CHARBASE_0x08000,GFL_BG_CHRSIZ_256x256, GX_BG_EXTPLTT_01,
			1, 0, 0, FALSE
		};
		GFL_BG_SetBGControl( GFL_BG_FRAME2_M, &TextBgCntDat, GFL_BG_MODE_TEXT );
		GFL_BG_ClearScreen( GFL_BG_FRAME2_M );
		GFL_BG_SetVisible( GFL_BG_FRAME2_M, TRUE );
	}


	GFL_BG_SetClearCharacter( GFL_BG_FRAME0_M, 32, 0, HEAPID_WORLDTRADE );
	GFL_BG_SetClearCharacter( GFL_BG_FRAME1_M, 32, 0, HEAPID_WORLDTRADE );

	GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );	//メイン画面OBJ面ＯＮ

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

	GFL_BG_FreeBGControl( GFL_BG_FRAME2_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME1_M );
	GFL_BG_FreeBGControl( GFL_BG_FRAME0_M );

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
	// 上画面ＢＧパレット転送
	//GFL_ARC_UTIL_TransVramPalette(    ARCID_WORLDTRADE_GRA, NARC_worldtrade_conect_nclr, PALTYPE_MAIN_BG, 0, 16*3*2,  HEAPID_WORLDTRADE);
	
	// 会話フォントパレット転送
	TalkFontPaletteLoad( PALTYPE_MAIN_BG, WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );
//	TalkFontPaletteLoad( PALTYPE_SUB_BG,  WORLDTRADE_TALKFONT_PAL*0x20, HEAPID_WORLDTRADE );

	// 会話ウインドウグラフィック転送
	BmpWinFrame_GraphicSet(	GFL_BG_FRAME0_M, WORLDTRADE_MESFRAME_CHR, 
						WORLDTRADE_MESFRAME_PAL,  0, HEAPID_WORLDTRADE );

	BmpWinFrame_GraphicSet(	GFL_BG_FRAME0_M, WORLDTRADE_MENUFRAME_CHR,
						WORLDTRADE_MENUFRAME_PAL, 0, HEAPID_WORLDTRADE );

	// 主人公が登場してないときはサブ画面を表示しない
	if(wk->demo_end==0){
		GFL_BG_SetVisible( GFL_BG_FRAME0_S, VISIBLE_OFF );
		GFL_BG_SetVisible( GFL_BG_FRAME1_S, VISIBLE_OFF );
		GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_OFF );	//サブ画面OBJ面ＯＮ
	}

	// デモから遷移してきた時は背景が壊れているので再転送
	if(wk->old_sub_process==WORLDTRADE_DEMO){
		WorldTrade_SubLcdBgGraphicSet( wk );
		WorldTrade_SubLcdWinGraphicSet( wk );   // トレードルームウインドウ転送
	}

}

#define SUB_TEXT_X		(  2 )
#define SUB_TEXT_Y		(  1 )
#define SUB_TEXT_SX		( 28 )
#define SUB_TEXT_SY		( 16 )

#define SUB_NUMBER_X	( 20 )
#define SUB_NUMBER_Y	( 21 )
#define SUB_NUMBER_SX	( 10 )
#define SUB_NUMBER_SY	(  2 )


#define TALK_MESSAGE_OFFSET	 ( WORLDTRADE_MENUFRAME_CHR + MENU_WIN_CGX_SIZ )
#define YESNO_OFFSET 		 ( TALK_MESSAGE_OFFSET + SUB_TEXT_SX*SUB_TEXT_SY )
//#define TITLE_MESSAGE_OFFSET ( TALK_MESSAGE_OFFSET + SUB_TEXT_SX*SUB_TEXT_SY )
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
	// BG0面BMP（会話ウインドウ）確保
	wk->MsgWin	= GFL_BMPWIN_Create( GFL_BG_FRAME0_M,
		TALK_WIN_X, TALK_WIN_Y, TALK_WIN_SX, TALK_WIN_SY, WORLDTRADE_TALKFONT_PAL,  GFL_BMP_CHRAREA_GET_B );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(wk->MsgWin), 0x0000 );
	GFL_BMPWIN_MakeTransWindow( wk->MsgWin );

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
	GFL_BMPWIN_Delete( wk->MsgWin );
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

	// 文字列バッファ作成
	wk->TalkString   = GFL_STR_CreateBuffer( TALK_MESSAGE_BUF_NUM, HEAPID_WORLDTRADE );

	// レコードこうかんぼしゅう中！文字列取得
//	GFL_MSG_GetString(  wk->MsgManager, msg_wifilobby_002, wk->TitleString );

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
 * @brief   サブプロセスシーケンススタート処理
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_Start( WORLDTRADE_WORK *wk)
{
	// 預けるのか？引き取るのか？もしくは交換か？

	switch(wk->sub_process_mode){
	case MODE_UPLOAD:
		// おくっています
		Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_01_025, 1, 0x0f0f );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_EVILCHECK_START );
    wk->evilcheck_loop  = 0;  //SUBSEQ_EVILCHECK_STARTをするときは必ず０にしてから移動
    //不正チェックしてからアップロードへ
    wk->evilcheck_mode  = SUBSEQ_UPLOAD_START;
		break;
	case MODE_DOWNLOAD:
		// うけとっています。
		Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_01_025, 1, 0x0f0f );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_DOWNLOAD_START );
		break;
	case MODE_EXCHANGE:
		// 交換します
 		Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_01_025, 1, 0x0f0f );
 		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_EVILCHECK_START );
    wk->evilcheck_loop  = 0;  //SUBSEQ_EVILCHECK_STARTをするときは必ず０にしてから移動
    //不正チェックしてから交換へ
    wk->evilcheck_mode  = SUBSEQ_EXCHANGE_START;
		break;

		// 交換が終わったポケモンを受け取ります
	case MODE_DOWNLOAD_EX:
 		Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_01_025, 1, 0x0f0f );
 		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_DOWNLOAD_EX_START );
		wk->sub_out_flg = 1;
		break;
		
	case MODE_SERVER_CHECK:	
		Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_01_025, 1, 0x0f0f );
		// 「ようすをみる」に行く前にサーバーチェックを行う
    WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_SERVER_TRADE_CHECK );
		break;
    
	case MODE_POKEMON_EVO_SAVE:
		// 「ポケモンレポートをかいています。でんげんをきらないでください」
		Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_15_004, 1, 0x0f0f );
		wk->sub_nextprocess = WORLDTRADE_TITLE;

		// 「セーブしています」と表示してからセーブ
		wk->subprocess_seq  = SUBSEQ_NOW_SAVE_MES;
    GFL_NET_ReloadIconTopOrBottom( TRUE, HEAPID_WORLDTRADE );

		break;
	default:
		OS_TPrintf("モード指定がない\n");
		GF_ASSERT_HEAVY(0);
	}
	// 必ず時間アイコンを追加(upload.cの中では１回しかよばないはず）
	WorldTrade_TimeIconAdd(wk);

	return SEQ_MAIN;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ポケモン不正検査を行う  開始
 *
 *	@param	WORLDTRADE_WORK *wk ワーク
 */
//-----------------------------------------------------------------------------
static int Subseq_EvilCheckStart( WORLDTRADE_WORK *wk )
{ 
  BOOL ret;
  
  POKEMON_PARAM  *pp = (POKEMON_PARAM*)wk->UploadPokemonData.postData.data;

  //不正チェックで生中身が書き換わる前にデモ用にデータを受け取る
  POKETOOL_CopyPPtoPP( pp, wk->evilcheck_dummy_poke );

  //もし国と地域が不正ならば書き換える
  //もし言語と国が異なっていたら書き換える
  wk->UploadPokemonData.countryCode = 
  WIFI_COUNTRY_GetNGTestCountryCode(wk->UploadPokemonData.countryCode, 
      wk->UploadPokemonData.localCode, wk->UploadPokemonData.langCode);

  wk->UploadPokemonData.langCode = 
  WIFI_COUNTRY_GetNGTestLocalCode(wk->UploadPokemonData.countryCode, 
      wk->UploadPokemonData.localCode, wk->UploadPokemonData.langCode);


  wk->nhttp = NHTTP_RAP_Init( HEAPID_WORLDTRADE, MyStatus_GetProfileID( wk->param->mystatus ), &wk->svl );
  NHTTP_RAP_PokemonEvilCheckCreate( wk->nhttp, HEAPID_WORLDTRADE, POKETOOL_GetWorkSize(), NHTTP_POKECHK_GTS);

  OS_TPrintf( "PPサイズ %d\n", POKETOOL_GetWorkSize() );
  NHTTP_RAP_PokemonEvilCheckAdd( wk->nhttp, pp, POKETOOL_GetWorkSize() );

  ret = NHTTP_RAP_PokemonEvilCheckConectionCreate( wk->nhttp );
  GF_ASSERT_HEAVY( ret );

  ret = NHTTP_RAP_StartConnect( wk->nhttp ) == NHTTP_ERROR_NONE; 
  GF_ASSERT_HEAVY( ret );

  OS_TPrintf( "不正検査開始\n" );
  wk->subprocess_seq = SUBSEQ_EVILCHECK_RESULT;

	return SEQ_MAIN;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ポケモン不正検査を行う  終了
 *
 *	@param	WORLDTRADE_WORK *wk ワーク
 */
//-----------------------------------------------------------------------------
static int Subseq_EvilCheckResult( WORLDTRADE_WORK *wk )
{ 
  NHTTPError error;
  int responce;

  responce  = NHTTP_RAP_GetGetResultCode(wk->nhttp);
  error     = NHTTP_RAP_Process( wk->nhttp );
  if( NHTTP_ERROR_NONE == error )
  { 
    void *p_data;
    p_data  = NHTTP_RAP_GetRecvBuffer(wk->nhttp);

    switch( responce )
    {
    case 200:
      /*  err none  */
      break;

    case 400:
      NetErr_App_ReqErrorDispForce( NHTTP_RESPONSE_400 );
      NetErr_DispCall(FALSE);

      NHTTP_RAP_ErrorClean(wk->nhttp);
      NHTTP_RAP_PokemonEvilCheckDelete(wk->nhttp);
      NHTTP_RAP_End(wk->nhttp);

      wk->subprocess_seq  = SUBSEQ_END_DPW;
      return SEQ_MAIN;

    case 401:
      NetErr_App_ReqErrorDispForce( NHTTP_RESPONSE_401 );
      NetErr_DispCall(FALSE);

      NHTTP_RAP_ErrorClean(wk->nhttp);
      NHTTP_RAP_PokemonEvilCheckDelete(wk->nhttp);
      NHTTP_RAP_End(wk->nhttp);

      wk->subprocess_seq  = SUBSEQ_END_DPW;
      return SEQ_MAIN;

    case 408:
      NetErr_App_ReqErrorDispForce( NHTTP_RESPONSE_TIMEOUT );
      NetErr_DispCall(FALSE);

      NHTTP_RAP_ErrorClean(wk->nhttp);
      NHTTP_RAP_PokemonEvilCheckDelete(wk->nhttp);
      NHTTP_RAP_End(wk->nhttp);

      wk->subprocess_seq  = SUBSEQ_END_DPW;
      return SEQ_MAIN;

    default:
      NetErr_App_ReqErrorDispForce( NHTTP_RESPONSE_ETC );
      NetErr_DispCall(FALSE);

      NHTTP_RAP_ErrorClean(wk->nhttp);
      NHTTP_RAP_PokemonEvilCheckDelete(wk->nhttp);
      NHTTP_RAP_End(wk->nhttp);

      wk->subprocess_seq  = SUBSEQ_END_DPW;
      return SEQ_MAIN;
    }

    wk->evilcheck_data.status_code  = NHTTP_RAP_EVILCHECK_GetStatusCode( p_data );
    wk->evilcheck_data.poke_result  = NHTTP_RAP_EVILCHECK_GetPokeResult( p_data, 0 );
    { 
      const s8 *cp_sign  = NHTTP_RAP_EVILCHECK_GetSign( p_data, 1 );
      GFL_STD_MemCopy( cp_sign, wk->evilcheck_data.sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );
    }

    NHTTP_RAP_PokemonEvilCheckDelete(wk->nhttp);
    NHTTP_RAP_End(wk->nhttp);

    //送られてきた状態は正常か
    OS_TPrintf( "不正検査=[%d]\n", wk->evilcheck_data.poke_result );
    if( wk->evilcheck_data.status_code == 0 )
    { 
      if( wk->evilcheck_data.poke_result == NHTTP_RAP_EVILCHECK_RESULT_OK )
      { 
        wk->subprocess_seq = SUBSEQ_NAMECHECK_START;
      }
      else
      { 
        // 「このポケモンはあずける事ができません」→タイトルへ
        if( wk->evilcheck_loop  == 0 )
        {
          wk->evilcheck_loop++;
          //初回ならば、ニックネーム、親、メールを変えてもう一度チェック
          wk->subprocess_seq = SUBSEQ_EVILCHECK_START;

          WorldTrade_SetEvilName( (POKEMON_PARAM*)wk->UploadPokemonData.postData.data, HEAPID_WORLDTRADE );
        }
        else
        {
          OS_TPrintf( "不正検査NG=[%d]\n", wk->evilcheck_data.poke_result );
          wk->ConnectErrorNo = DPW_TR_ERROR_CHEAT_DATA;
          wk->subprocess_seq = SUBSEQ_RETURN_TITLE_MESSAGE;
        }
      }
    }
    else
    { 
      if( wk->evilcheck_loop  == 0 )
      {
        wk->evilcheck_loop++;
        //初回ならば、ニックネーム、親、メールを変えてもう一度チェック
        wk->subprocess_seq = SUBSEQ_EVILCHECK_START;

        WorldTrade_SetEvilName( (POKEMON_PARAM*)wk->UploadPokemonData.postData.data, HEAPID_WORLDTRADE );
      }
      else
      {
        // 「このポケモンはあずける事ができません」→タイトルへ
        OS_TPrintf( "不正検査NG=[%d]\n", wk->evilcheck_data.poke_result );
        wk->ConnectErrorNo = DPW_TR_ERROR_CHEAT_DATA;
        wk->subprocess_seq = SUBSEQ_RETURN_TITLE_MESSAGE;
      }
    }
  }
  else if( NHTTP_ERROR_BUSY != error )
  {
			// サーバーと通信できません→終了
			OS_TPrintf(" evilcheck error. %d \n", error);
			wk->ConnectErrorNo = DPW_TR_ERROR_FAILURE;
			wk->subprocess_seq = SUBSEQ_ERROR_MESSAGE;

      NHTTP_RAP_PokemonEvilCheckDelete(wk->nhttp);
      NHTTP_RAP_End(wk->nhttp);

      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();
  }

	return SEQ_MAIN;
}

//----------------------------------------------------------------------------
/**
 *	@brief  トレーナー名の不正検査を行う  開始
 *
 *	@param	WORLDTRADE_WORK *wk ワーク
 */
//-----------------------------------------------------------------------------
static int Subseq_NameCheckStart( WORLDTRADE_WORK *wk )
{
  STRCODE *p_code   = wk->UploadPokemonData.name;
  STRBUF  *p_buff   = GFL_STR_CreateBuffer( DPW_TR_NAME_SIZE, HEAPID_WORLDTRADE );
  
  GFL_STR_SetStringCodeOrderLength( p_buff, p_code, DPW_TR_NAME_SIZE );

  DWC_TOOL_BADWORD_Start( &wk->badword_wk, p_buff, HEAPID_WORLDTRADE );

  GFL_STR_DeleteBuffer( p_buff );

  wk->subprocess_seq = SUBSEQ_NAMECHECK_RESULT;
  wk->timeout_count = 0;

	return SEQ_MAIN;
}
//----------------------------------------------------------------------------
/**
 *	@brief  トレーナー名の不正検査を行う  終了
 *
 *	@param	WORLDTRADE_WORK *wk ワーク
 */
//-----------------------------------------------------------------------------
static int Subseq_NameCheckResult( WORLDTRADE_WORK *wk )
{ 
  BOOL is_bad_word;

  if( DWC_TOOL_BADWORD_Wait( &wk->badword_wk, &is_bad_word ) )
  {
    if( is_bad_word )
    {
      STRCODE *p_code   = wk->UploadPokemonData.name;
      DWC_TOOL_SetBadNickName( p_code, DPW_TR_NAME_SIZE, HEAPID_WORLDTRADE );
    }

    OS_TPrintf( "不正検査終了=[%d]次=%d\n", wk->evilcheck_data.poke_result );
    wk->subprocess_seq = wk->evilcheck_mode;
  }
  else
  {
    if(wk->timeout_count++ == TIMEOUT_TIME){
      // サーバーと通信できません→終了
      OS_TPrintf("name error. \n" );
      wk->ConnectErrorNo = DPW_TR_ERROR_ILLIGAL_REQUEST;
      wk->subprocess_seq = SUBSEQ_ERROR_MESSAGE;

      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();
		}
  }

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   世界交換ライブラリ接続状況取得開始
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_UploadStart( WORLDTRADE_WORK *wk )
{
	
	// ポケモンデータアップロード開始  
	Dpw_Tr_UploadAsync( &wk->UploadPokemonData, wk->evilcheck_data.sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );

	OS_TPrintf("Dpw Trade データアップロード開始\n");

	// サーバー状態確認待ちへ
	wk->subprocess_seq = SUBSEQ_UPLOAD_RESULT;
	wk->timeout_count = 0;
	
	SetSaveNextSequence( wk, SUBSEQ_UPLOAD_FINISH, SUBSEQ_UPLOAD_SUCCESS_MESSAGE);

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   サーバー状態確認待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_UploadResult( WORLDTRADE_WORK *wk )
{
	int result;

  result =Dpw_Tr_IsAsyncEnd();
	if (result){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch (result){
		case 0:		// 正常に動作している
			OS_TPrintf(" upload is right!\n");

			// データ退避
			UploadPokemonDataDelete( wk, 1 );
			// レコード用処理
			RECORD_Inc( wk->param->record, RECID_GTS_PUT );

			wk->subprocess_seq = SUBSEQ_SAVE;
			break;
		case DPW_TR_ERROR_SERVER_FULL:
			// サーバーが満杯なのでアクセスできません→終了
			OS_TPrintf(" server full.\n");
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_ERROR_MESSAGE;

      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();
			break;
		case DPW_TR_ERROR_ILLEGAL_DATA:
		case DPW_TR_ERROR_CHEAT_DATA:
		case DPW_TR_ERROR_NG_POKEMON_NAME :
		case DPW_TR_ERROR_NG_PARENT_NAME :
		case DPW_TR_ERROR_NG_MAIL_NAME :
		case DPW_TR_ERROR_NG_OWNER_NAME:
		case DPW_TR_ERROR_ILLIGAL_REQUEST :
			// 「このポケモンはあずける事ができません」→タイトルへ
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_RETURN_TITLE_MESSAGE;
			break;		
		case DPW_TR_ERROR_CANCEL :
		case DPW_TR_ERROR_DATA_TIMEOUT :
			// 入り口で預けるか確認してからここ３０日前ポケモンはありなのか？
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_RETURN_TITLE_MESSAGE;
			break;

		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
			// 「GTSのかくにんにしっぱいしました」
		case DPW_TR_ERROR_FAILURE :
			// サーバーと通信できません→終了
			OS_TPrintf(" upload error. %d \n", result);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_ERROR_MESSAGE;

      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();
			break;
		case DPW_TR_ERROR_FATAL:			//!< 通信致命的エラー。電源の再投入が必要です
			// 即ふっとばし
      WorldTrade_DispCallFatal(wk);
			break;
		}
		
	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      WorldTrade_DispCallFatal(wk);
		}
	}
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   サーバーにアップロードしたデータを有効にする
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_UploadFinish( WORLDTRADE_WORK *wk )
{
	Dpw_Tr_UploadFinishAsync();
	
	wk->subprocess_seq = SUBSEQ_UPLOAD_FINISH_RESULT;
	wk->timeout_count = 0;
	
	// ここに来ている時点でセーブには成功しているので、DepsitFlagを立てておかないと
	// 通信エラーになった時にもう一度「預ける」が表示されてしまう
	wk->DepositFlag = 1;

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   データ有効処理待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_UploadFinishResult( WORLDTRADE_WORK *wk )
{
	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();
#ifdef DEBUG_UPLOAD_ERROR
		result = DPW_TR_ERROR_FAILURE;
#endif
		wk->timeout_count = 0;
		switch(result){
		case 0:		// 正常に動作している
			// アップロード成功。セーブする。
      
      wk->serverWaitTime  = 0;
			OS_TPrintf(" upload success.\n");
			wk->subprocess_seq = SUBSEQ_SAVE_LAST;
			break;

		case DPW_TR_ERROR_CANCEL :
		case DPW_TR_ERROR_DATA_TIMEOUT :
			// 入り口で預けるか確認してからここ３０日前ポケモンはありなのか？
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_RETURN_TITLE_MESSAGE;
			break;

		case DPW_TR_ERROR_SERVER_FULL:
			// サーバーが満杯なのでアクセスできません→終了
			// （ここくるの？）
			OS_TPrintf(" server full.\n");
		case DPW_TR_ERROR_NO_DATA:
		case DPW_TR_ERROR_ILLIGAL_REQUEST :
			// 「このポケモンはあずける事ができません」→タイトルへ
		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
		case DPW_TR_ERROR_FAILURE :
			// 「GTSのかくにんにしっぱいしました」
			// サーバーと通信できません→終了
			OS_TPrintf(" upload error. %d \n", result);
			// ここはうけつけに戻ってはいけない。無理矢理通信エラー
      WorldTrade_DispCallFatal(wk);
			break;

		case DPW_TR_ERROR_FATAL:			//!< 通信致命的エラー。電源の再投入が必要です
			WorldTrade_DispCallFatal(wk);
			break;
		}

	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      WorldTrade_DispCallFatal(wk);
		}
	}
	return SEQ_MAIN;
}




//------------------------------------------------------------------
/**
 * @brief   世界交換ライブラリ接続状況取得開始
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_DownloadStart( WORLDTRADE_WORK *wk )
{
	
	// ポケモンデータダウンロード開始

	Dpw_Tr_DownloadAsync( &wk->UploadPokemonData );
	OS_TPrintf("Dpw Trade データダウンロード開始\n");

	// サーバー状態確認待ちへ
	wk->subprocess_seq = SUBSEQ_DOWNLOAD_RESULT;
	wk->timeout_count = 0;

	// セーブ込みシーケンス予約
	SetSaveNextSequence( wk, SUBSEQ_DOWNLOAD_FINISH, SUBSEQ_DOWNLOAD_SUCCESS_MESSAGE);


	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   サーバー状態確認待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_DownloadResult( WORLDTRADE_WORK *wk )
{
	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch (result){
		case 0:		// 正常に動作している

			// 引き取ったポケモンが交換成立していたらひきとったポケモンデータは間違っているので放棄
			// もう一度サーバーチェックに戻る
			if(wk->UploadPokemonData.isTrade){
				OS_TPrintf(" download is right! but traded\n");

				wk->subprocess_seq = SUBSEQ_SERVER_TRADE_CHECK;
			}else{

				OS_TPrintf(" download is right!\n");
				// データ復元
#if 1
        // データ復元の際は、不正名の置き換えによりすでに別のデータになっているので
        // セーブデータから復元するように変更
        DownloadPokemonDataAdd( wk, WorldTradeData_GetPokemonDataPtr( wk->param->worldtrade_data ),
            WorldTradeData_GetBoxNo( wk->param->worldtrade_data ), 
            wk->UploadPokemonData.isTrade );
#else
				DownloadPokemonDataAdd( wk, (POKEMON_PARAM*)wk->UploadPokemonData.postData.data,
										WorldTradeData_GetBoxNo( wk->param->worldtrade_data ), 
										wk->UploadPokemonData.isTrade );
#endif
				wk->subprocess_seq = SUBSEQ_SAVE;

			}


			break;

		// データが無い（かなりおかしい状況、さっきは落とせたのに）
		case DPW_TR_ERROR_NO_DATA :	
			OS_TPrintf(" download server stop service.\n");
			wk->subprocess_seq = SUBSEQ_ERROR_MESSAGE;

      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();

			break;

		// 1ヶ月過ぎてしまった
		case DPW_TR_ERROR_DATA_TIMEOUT :
			OS_TPrintf(" server full.\n");
			wk->subprocess_seq = SUBSEQ_ERROR_MESSAGE;

      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();
			break;

	// -----------------------------------------
	// 共通エラー処理
	// -----------------------------------------
		case DPW_TR_ERROR_CANCEL :
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_RETURN_TITLE_MESSAGE;
			break;

		case DPW_TR_ERROR_FAILURE :
			// 「GTSのかくにんにしっぱいしました」
		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
			// サーバーと通信できません→終了
			OS_TPrintf(" upload error. %d \n", result);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_ERROR_MESSAGE;


      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();
			break;
		case DPW_TR_ERROR_FATAL:			//!< 通信致命的エラー。電源の再投入が必要です
			// 即ふっとばし
      WorldTrade_DispCallFatal(wk);
			break;
	// -----------------------------------------
		}
		
	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      WorldTrade_DispCallFatal(wk);
		}
	}
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   サーバーからデータを削除する
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_DownloadFinish( WORLDTRADE_WORK *wk )
{
	Dpw_Tr_ReturnAsync();
	OS_TPrintf("-------------------------------------Dpw_Tr_ReturnAsyncよびだし\n");
	
	wk->subprocess_seq = SUBSEQ_DOWNLOAD_FINISH_RESULT;
	wk->timeout_count = 0;
	
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   データ有効処理待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_DownloadFinishResult( WORLDTRADE_WORK *wk )
{
	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch(result){
		case 0:
			// サーバーからダウンロード成功。セーブする。
			OS_TPrintf(" download success.\n");
			wk->subprocess_seq = SUBSEQ_SAVE_LAST;

			break;
		// データが無い（かなりおかしい状況、さっきは落とせたのに）
		case DPW_TR_ERROR_NO_DATA :	
			OS_TPrintf(" download server stop service.\n");

		// 1ヶ月過ぎてしまった(来ないような気がする）
		case DPW_TR_ERROR_DATA_TIMEOUT :
			OS_TPrintf(" server full.\n");

		// 最後のつめを行おうとしたら交換が成立してしまった。
		// エラーが起きた事にして外にだしてしまう→もどってくれば交換が成立してます。
		case DPW_TR_ERROR_ILLIGAL_REQUEST:
			WorldTrade_DispCallFatal(wk);
			break;
	// -----------------------------------------
	// 共通エラー処理
	// -----------------------------------------
		case DPW_TR_ERROR_CANCEL :

		case DPW_TR_ERROR_FAILURE :
			// 「GTSのかくにんにしっぱいしました」
		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
			// サーバーと通信できません→終了
			OS_TPrintf(" upload error. %d \n", result);

			// セーブがほとんど書き込まれている状況ではデータを元に戻せない
			// 無理矢理通信エラーへ
      WorldTrade_DispCallFatal(wk);
			break;
		case DPW_TR_ERROR_FATAL:			//!< 通信致命的エラー。電源の再投入が必要です
			// 即ふっとばし
      WorldTrade_DispCallFatal(wk);
			break;
	// -----------------------------------------

		}

	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      WorldTrade_DispCallFatal(wk);
		}
	}
	return SEQ_MAIN;
}







//------------------------------------------------------------------
/**
 * @brief   世界交換ライブラリ接続状況取得開始
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ExchangeStart( WORLDTRADE_WORK *wk )
{
	// ポケモンデータ交換開始 
	Dpw_Tr_TradeAsync ( wk->DownloadPokemonData[wk->TouchTrainerPos].id,
						&wk->UploadPokemonData,
						&wk->ExchangePokemonData,
            wk->evilcheck_data.sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );
  
	OS_TPrintf("Dpw Trade データ交換開始 id = %08x\n", wk->DownloadPokemonData[wk->TouchTrainerPos].id);


	// セーブ込みシーケンス予約
	SetSaveNextSequence( wk, SUBSEQ_EXCHANGE_FINISH, SUBSEQ_EXCHANGE_SUCCESS_MESSAGE);

	// サーバー状態確認待ちへ
	wk->subprocess_seq = SUBSEQ_EXCHANGE_RESULT;
	wk->timeout_count = 0;

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   サーバー状態確認待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ExchangeResult( WORLDTRADE_WORK *wk )
{
	int result;
  result=Dpw_Tr_IsAsyncEnd();
	if (result){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch (result){
		case 0:		// 正常に動作している
			OS_TPrintf(" exchange is right!\n");
			wk->subprocess_seq = SUBSEQ_SAVE;

			UploadPokemonDataDelete( wk, 0 );
			ExchangePokemonDataAdd(  wk, 
                    (POKEMON_PARAM*)wk->ExchangePokemonData.postData.data,
                    (POKEMON_PARAM*)wk->UploadPokemonData.postData.data,
                    &wk->ExchangePokemonData,
										wk->BoxTrayNo);
			OS_Printf("exchange poke adr = %08x\n", (u32)wk->ExchangePokemonData.postData.data);

			// 地球儀情報登録
			WifiHistoryDataSet( wk->param->wifihistory, &wk->ExchangePokemonData );

			MakeTradeExchangeInfomation( wk, (POKEMON_PARAM*)wk->ExchangePokemonData.postData.data, &wk->ExchangePokemonData, (POKEMON_PARAM*)wk->UploadPokemonData.postData.data );

			break;

		// トレード発行失敗
		case DPW_TR_ERROR_ILLIGAL_REQUEST :	
			OS_TPrintf(" exchange is failed.\n");
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_EXCHANGE_FAILED_MESSAGE;
			break;

		// 不正データだった
		case DPW_TR_ERROR_ILLEGAL_DATA:
		case DPW_TR_ERROR_CHEAT_DATA:
		case DPW_TR_ERROR_NG_POKEMON_NAME :
		case DPW_TR_ERROR_NG_PARENT_NAME :
		case DPW_TR_ERROR_NG_MAIL_NAME :
		case DPW_TR_ERROR_NG_OWNER_NAME:
			// 「このポケモンはあずける事ができません」→タイトルへ
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_RETURN_TITLE_MESSAGE;
			break;		
	// -----------------------------------------
	// 共通エラー処理
	// -----------------------------------------
		case DPW_TR_ERROR_CANCEL :
			// 「GTSのかくにんにしっぱいしました」→タイトルへ
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_RETURN_TITLE_MESSAGE;
			break;

		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
		case DPW_TR_ERROR_FAILURE :
			// サーバーと通信できません→終了
			OS_TPrintf(" upload error. %d \n", result);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_ERROR_MESSAGE;


      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();
			break;
		case DPW_TR_ERROR_FATAL:			//!< 通信致命的エラー。電源の再投入が必要です
			// 即ふっとばし
      WorldTrade_DispCallFatal(wk);
			break;
	// -----------------------------------------
		}
		
	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      WorldTrade_DispCallFatal(wk);
		}
	}
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   サーバーからデータを削除する
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ExchangeFinish( WORLDTRADE_WORK *wk )
{
	Dpw_Tr_TradeFinishAsync();
	OS_TPrintf("交換終了処理開始\n");
	
	wk->subprocess_seq = SUBSEQ_EXCHANGE_FINISH_RESULT;
	wk->timeout_count = 0;
	
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   データ有効処理待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ExchangeFinishResult( WORLDTRADE_WORK *wk )
{
	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch(result){
		case 0:
			OS_TPrintf(" exchange success.\n");
			wk->subprocess_seq = SUBSEQ_SAVE_LAST;

			break;

		// 引き取られてしまった
		case DPW_TR_ERROR_ILLIGAL_REQUEST:
      WorldTrade_DispCallFatal(wk);
			break;
	// -----------------------------------------
	// 共通エラー処理
	// -----------------------------------------
		case DPW_TR_ERROR_CANCEL :
			// 「GTSのかくにんにしっぱいしました」→タイトルへ

		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
		case DPW_TR_ERROR_FAILURE :
			// サーバーと通信できません→終了
			OS_TPrintf(" upload error. %d \n", result);
	
			// ９９％までセーブが書き込まれた状況では元に戻せないので
			// 無理矢理通信エラーへ
			WorldTrade_DispCallFatal(wk);
			break;

			break;
		case DPW_TR_ERROR_FATAL:			//!< 通信致命的エラー。電源の再投入が必要です
			// 即ふっとばし
      WorldTrade_DispCallFatal(wk);
			break;
	// -----------------------------------------
		}

	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      WorldTrade_DispCallFatal(wk);
		}
	}
	return SEQ_MAIN;
}




//------------------------------------------------------------------
/**
 * @brief   交換が成立しているか確認する
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ServerTradeCheck( WORLDTRADE_WORK *wk )
{
	Dpw_Tr_GetUploadResultAsync( &wk->UploadPokemonData );

	OS_Printf("サーバー状態確認開始\n");

	wk->subprocess_seq  = SUBSEQ_SERVER_TRADECHECK_RESULT;
	wk->timeout_count = 0;

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   交換ポケモンサーバー確認処理待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ServerTradeCheckResult( WORLDTRADE_WORK *wk )
{
	// サーバー問い合わせ終了待ち
	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch (result){
		// 交換は成立していなかったので、自分のデータ取得へ
		case 0:
//			OS_TPrintf(" no exchange.\n");
			wk->subprocess_seq = SUBSEQ_SERVER_DOWNLOAD;

#ifdef PM_DEBUG
			{
				Dpw_Tr_Data *dtd = &wk->UploadPokemonData;

				OS_Printf( "TrainerId = %05d,  nation = %d, area = %d\n", dtd->trainerID, dtd->countryCode, dtd->localCode);
				OS_Printf( "Version   = %d,  language = %d, \n", dtd->versionCode, dtd->langCode);
				OS_Printf( "postData  No = %d,  gender = %d, level = %d\n", dtd->postSimple.characterNo, dtd->postSimple.gender, dtd->postSimple.level);
				OS_Printf( "WantData  No = %d,  gender = %d, level min = %d max = %d\n", 
						dtd->wantSimple.characterNo, dtd->wantSimple.gender, dtd->wantSimple.level_min,dtd->wantSimple.level_max);
			}
#endif
			// 預けているか？
			break;

		// 交換は成立していた
		case 1:
			OS_TPrintf(" download exchange pokemon on sever.\n");
			// サーバーにポケモンを預けているのは確か。
			wk->DepositFlag = 1;

#ifdef PM_DEBUG
			{
				Dpw_Tr_Data *dtd = &wk->UploadPokemonData;
	
				OS_Printf( "TrainerId = %05d,  nation = %d, area = %d\n", dtd->trainerID, dtd->countryCode, dtd->localCode);
				OS_Printf( "Version   = %d,  language = %d, \n", dtd->versionCode, dtd->langCode);
				OS_Printf( "postData  No = %d,  gender = %d, level = %d\n", dtd->postSimple.characterNo, dtd->postSimple.gender, dtd->postSimple.level);
				OS_Printf( "WantData  No = %d,  gender = %d, level min = %d max = %d\n", 
						dtd->wantSimple.characterNo, dtd->wantSimple.gender, dtd->wantSimple.level_min,dtd->wantSimple.level_max);
			}
#endif
			switch(MyPokemonPocketFullCheck(wk, &wk->UploadPokemonData)){
			// 空きが無い
			case POKEMON_ALL_FULL:
				WorldTrade_TimeIconDel(wk);
		 		Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_01_030, 1, 0x0f0f );
 				WorldTrade_SetNextSeq( wk, SUBSEQ_MES_KEYWAIT, SUBSEQ_SERVER_TRADE_CHECK_END );
				break;
			// メールが受け取れない
			case POKEMON_NOT_FULL_BUT_MAIL_NORECV:
				WorldTrade_TimeIconDel(wk);
		 		Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_01_036, 1, 0x0f0f );
 				WorldTrade_SetNextSeq( wk, SUBSEQ_MES_KEYWAIT, SUBSEQ_SERVER_TRADE_CHECK_END );
				break;
			// 受け取れる
			case POKEMON_RECV_OK:
				wk->subprocess_seq   = SUBSEQ_DOWNLOAD_EX_START;
				wk->sub_out_flg = 1;

        wk->is_trade_download = TRUE;
				break;
			}
			break;
		// ポケモンをあずけている場合はタイムアウトが発生してしまっている（てもちに戻す）
		case DPW_TR_ERROR_NO_DATA :	
			OS_TPrintf(" no data on sever.\n");
			wk->DepositFlag = 0;
			if(WorldTradeData_GetFlag(wk->param->worldtrade_data)){
				POKEMON_PARAM *pp = PokemonParam_AllocWork(HEAPID_WORLDTRADE);
				WorldTradeData_GetPokemonData( wk->param->worldtrade_data, pp );
				WORDSET_RegisterPokeNickName( wk->WordSet, 0, pp );

				wk->error_mes_no   = msg_gtc_01_003;
				wk->subprocess_seq = SUBSEQ_TIMEOUT_SAVE;
				
				// 保存してあったポケモンを戻す
				DownloadPokemonDataAdd( wk, pp, WorldTradeData_GetBoxNo( wk->param->worldtrade_data ), 0 );
				// 預けてあるフラグを解除
				WorldTradeData_SetFlag(wk->param->worldtrade_data, 0);
				GFL_HEAP_FreeMemory(pp);
			}else{
				// サーバー確認処理終了後の行き先を設定
				AfterTradeCheck_ProcessControl( wk );
			}
			break;

		// 預けたポケモンが交換成立後にタイムアウトした。そのまま削除
		case DPW_TR_ERROR_DATA_TIMEOUT :
			OS_TPrintf(" your data on server is timeout.\n");

			// サーバー確認処理終了後の行き先を設定
//			AfterTradeCheck_ProcessControl( wk );

			wk->DepositFlag = 0;
			// 絶対預けてあるはずだけど念のため、フラグ確認
			if(WorldTradeData_GetFlag(wk->param->worldtrade_data)){
				POKEMON_PARAM *pp = PokemonParam_AllocWork(HEAPID_WORLDTRADE);
				WorldTradeData_GetPokemonData( wk->param->worldtrade_data, pp );
				WORDSET_RegisterPokeNickName( wk->WordSet, 0, pp );

				wk->error_mes_no   = msg_gtc_01_004;
				wk->subprocess_seq = SUBSEQ_TIMEOUT_SAVE;
				
				// 預けてあるフラグを解除
				WorldTradeData_SetFlag(wk->param->worldtrade_data, 0);

				GFL_HEAP_FreeMemory(pp);
			}
			break;
	// -----------------------------------------
	// 共通エラー処理
	// -----------------------------------------
		case DPW_TR_ERROR_CANCEL :
		case DPW_TR_ERROR_FAILURE :
			// 他の場合はタイトルに戻すが、ここで失敗すると
			// その後の操作も失敗する可能性が高いので。
		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
			// サーバーと通信できません→終了
			OS_TPrintf(" upload error. %d \n", result);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_ERROR_MESSAGE;

      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();
			break;
		case DPW_TR_ERROR_FATAL:			//!< 通信致命的エラー。電源の再投入が必要です
			// 即ふっとばし
			WorldTrade_DispCallFatal(wk);
			break;
	// -----------------------------------------
		}

	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      WorldTrade_DispCallFatal(wk);
		}
	}
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   受け取ることができなかったのでタイトルにもどる
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ServerTradeCheckEnd( WORLDTRADE_WORK *wk )
{
	// ポケモンを受け取ることができないので、タイトルにもどる
	WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
	wk->subprocess_seq = SUBSEQ_END;

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   既にサーバーに預けているか確認
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ServerDownload( WORLDTRADE_WORK *wk )
{
	Dpw_Tr_DownloadAsync( &wk->UploadPokemonData );

	wk->subprocess_seq  = SUBSEQ_SERVER_DOWNLOAD_RESULT;
	wk->timeout_count = 0;

	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   交換ポケモンサーバー確認処理待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ServerDownloadResult( WORLDTRADE_WORK *wk )
{
	// サーバー問い合わせ終了待ち
	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch (result){
		// 預けているポケモンのデータを取得した
		case 0:
			OS_TPrintf(" download data on sever.\n");

#if GTS_DUPLICATE_BUG_FIX
			
			// 複製チェックの結果が陰性だった場合はサーバーポケモン消去へ
			if(DupulicateCheck( wk )){
				wk->subprocess_seq = SUBSEQ_SERVER_POKE_DELETE;
				wk->DepositFlag    = 0;
				return SEQ_MAIN;
			}else{
				// 正常
				wk->DepositFlag = 1;
			}
#else
			// 預けてあるフラグＯＮ
			wk->DepositFlag = 1;

#endif


			{
				Dpw_Tr_Data *dtd = &wk->UploadPokemonData;

				OS_Printf( "TrainerId = %05d,  nation = %d, area = %d\n", dtd->trainerID, dtd->countryCode, dtd->localCode);
				OS_Printf( "Version   = %d,  language = %d, \n", dtd->versionCode, dtd->langCode);
				OS_Printf( "postData  No = %d,  gender = %d, level = %d\n", dtd->postSimple.characterNo, dtd->postSimple.gender, dtd->postSimple.level);
				OS_Printf( "WantData  No = %d,  gender = %d, level min = %d max = %d\n", 
						dtd->wantSimple.characterNo, dtd->wantSimple.gender, dtd->wantSimple.level_min,dtd->wantSimple.level_max);

			}
			break;
		// ポケモンは預けられていない
		case DPW_TR_ERROR_NO_DATA :	
			OS_TPrintf(" no data on sever.\n");
			wk->DepositFlag = 0;
			break;
		// 預けたポケモンがタイムアウトしてしまったので、隠しているポケモンを復活させる
		case DPW_TR_ERROR_DATA_TIMEOUT :
			OS_TPrintf(" your data on server is timeout.\n");
			break;
	// -----------------------------------------
	// 共通エラー処理
	// -----------------------------------------
		case DPW_TR_ERROR_CANCEL :
		case DPW_TR_ERROR_FAILURE :
			// 「GTSのかくにんにしっぱいしました」
		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
			// サーバーと通信できません→終了
			OS_TPrintf(" upload error. %d \n", result);
			wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_ERROR_MESSAGE;

      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();
			return SEQ_MAIN;
		case DPW_TR_ERROR_FATAL:			//!< 通信致命的エラー。電源の再投入が必要です
			// 即ふっとばし
      WorldTrade_DispCallFatal(wk);
			return SEQ_MAIN;
	// -----------------------------------------

		}

		// サーバー確認処理終了後の行き先を設定
		AfterTradeCheck_ProcessControl( wk );

	}
  else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      WorldTrade_DispCallFatal(wk);
		}
  }
	return SEQ_MAIN;
}



//------------------------------------------------------------------
/**
 * @brief   サーバー確認処理終了後の戻り先を設定
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void AfterTradeCheck_ProcessControl( WORLDTRADE_WORK *wk )
{
	switch( wk->sub_returnprocess ){
	case WORLDTRADE_TITLE:
		// GTC入り口画面へ
		WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
		wk->subprocess_seq  = SUBSEQ_END;
		break;

	// 自分のポケモン確認画面へ	
	case WORLDTRADE_MYPOKE:
		WorldTrade_SubProcessChange( wk, WORLDTRADE_MYPOKE, MODE_VIEW );
		wk->subprocess_seq  = SUBSEQ_END;
		break;
	}

}


//------------------------------------------------------------------
/**
 * @brief   交換成立時の追加情報作成処理
 *
 * @param   wk		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void MakeTradeExchangeInfomation( WORLDTRADE_WORK *wk, POKEMON_PARAM *recv_pp, Dpw_Tr_Data *recv_tr, POKEMON_PARAM *send_pp )
{
	// スコア加算->なくなりました
	//RECORD_Score_Add( wk->param->record, SCORE_ID_WORLD_TRADE );

	OS_Printf("exchange poke adr = %08x\n", (u32)recv_pp);

	// レコード用処理
	RECORD_Inc( wk->param->record, RECID_WIFI_TRADE );

  //  知り合いに登録
  { 
    ETC_SAVE_WORK *p_etc  = SaveData_GetEtc( wk->param->savedata );
    EtcSave_SetAcquaintance( p_etc, recv_tr->trainerID );
  }

  //GTS用友達
  { 
    MYSTATUS  *p_status = MakePartnerStatusData( recv_tr );
    {
      WIFI_NEGOTIATION_SAVEDATA *p_nego   = WIFI_NEGOTIATION_SV_GetSaveData(wk->param->savedata);
      WIFI_NEGOTIATION_SV_SetFriend( p_nego, p_status );
    }
    GFL_HEAP_FreeMemory( p_status );
  }

  //国連データ
  { 
    MYSTATUS  *p_status = MakePartnerStatusData( recv_tr );

    UNITEDNATIONS_SAVE  un_data;
    u16 send_poke;
    GFL_STD_MemClear( &un_data, sizeof(UNITEDNATIONS_SAVE) );

    MyStatus_Copy( p_status, &un_data.aMyStatus );

    if( send_pp  )
    {
      send_poke = PP_Get( send_pp, ID_PARA_monsno, NULL );
    }
    else
    {
      send_poke = recv_tr->postSimple.characterNo;
    }

#ifdef BUGFIX_BTS7927_100723
    un_data.recvPokemon = send_poke;  //NPCがもらったポケモン（プレーヤーがあげたポケモン）
    un_data.sendPokemon = PP_Get( recv_pp, ID_PARA_monsno, NULL );  //NPCがあげたポケモン（プレーヤーがもらったポケモン）
#else
    un_data.recvPokemon = PP_Get( recv_pp, ID_PARA_monsno, NULL );
    un_data.sendPokemon = send_poke;
#endif
    un_data.favorite    = recv_tr->favorite;
    un_data.nature      = recv_tr->nature;
    un_data.countryCount= recv_tr->countryCount;
    UNDATAUP_Update( wk->param->wifihistory, &un_data );
    GFL_HEAP_FreeMemory( p_status );

    NAGI_Printf( "recv=%d send=%d\n", un_data.recvPokemon, un_data.sendPokemon );
  }

#if 0	//TV_OFF
	// TVデータ
	{
		TV_WORK* tvwk;
		tvwk = SaveData_GetTvWork( wk->param->savedata );
		TVTOPIC_GTSTemp_Set( tvwk );
		//TVTOPIC_Entry_Record_GTS( wk->param->savedata );
	}
#endif

#ifdef PHC_EVENT_CHECK
	if( PP_Get(recv_pp,ID_PARA_country_code, NULL) != CasetteLanguage ){
		PHC_SVDATA *phc_svdata = SaveData_GetPhcSaveData( wk->param->savedata );
		PhcSvData_SetCourseOpenFlag( phc_svdata, PHC_WIFI_OPEN_COURSE_NO );
	}else{
		OS_Printf("うみのむこうフラグはたたない\n");
	}
#endif //PHC_EVENT_CHECK
}
//------------------------------------------------------------------
/**
 * @brief   非同期処理のキャンセルを開始
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_StartCancelAsync( WORLDTRADE_WORK *wk )
{
  OS_TPrintf( "キャンセル処理開始\n" );
  wk->timeout_count = 0;
  Dpw_Tr_CancelAsync();
  wk->subprocess_seq = SUBSEQ_WAIT_CANCEL_ASYNC;
	return SEQ_MAIN;
}
//------------------------------------------------------------------
/**
 * @brief   非同期処理のキャンセル待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_WaitCancelAsync( WORLDTRADE_WORK *wk )
{
  if( Dpw_Tr_IsAsyncEnd() )
  {
    s32 result = Dpw_Tr_GetAsyncResult();
    switch( result )
    {
    case DPW_TR_ERROR_CANCEL:
      OS_TPrintf( "キャンセル成功\n" );
      wk->subprocess_seq = SUBSEQ_END_ERR;//wk->subprocess_nextseq;
      break;

    case DPW_TR_ERROR_DISCONNECTED:
    case DPW_TR_ERROR_FAILURE:
    case DPW_TR_ERROR_SERVER_TIMEOUT:
      OS_TPrintf( "キャンセル失敗=%d\n",result );
      wk->subprocess_seq = SUBSEQ_END_ERR;//wk->subprocess_nextseq;
      break;

    case DPW_TR_ERROR_FATAL:
      WorldTrade_DispCallFatal(wk);
      break;
    }

  }
  else
  {
    OS_TPrintf( "キャンセル処理を待っています\n" );

    wk->timeout_count++;
    if(wk->timeout_count == 60*60){
      wk->subprocess_seq = SUBSEQ_END_ERR;//wk->subprocess_nextseq;
		}
  }
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   エラーでタイトルへ戻る
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_EndErr( WORLDTRADE_WORK *wk )
{
  OS_TPrintf( "DPW_TRを終了しました\n" );
  Dpw_Tr_End();

	// 必ず時間アイコンを消去(２重解放対策はしておく）
	WorldTrade_TimeIconDel(wk);

  WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
  wk->subprocess_seq = 0;
	
	return SEQ_FADEOUT;

}

//------------------------------------------------------------------
/**
 * @brief   交換済みポケモンをサーバーから削除する
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_DownloadExStart( WORLDTRADE_WORK *wk)
{

	// データ復元
	DownloadPokemonDataAdd( wk, (POKEMON_PARAM*)wk->UploadPokemonData.postData.data,
							WorldTradeData_GetBoxNo( wk->param->worldtrade_data ), 
							wk->UploadPokemonData.isTrade );


	// 地球儀情報登録
	WifiHistoryDataSet( wk->param->wifihistory, &wk->UploadPokemonData );

	// 交換成立時の追加情報作成処理
	MakeTradeExchangeInfomation( wk, (POKEMON_PARAM*)wk->UploadPokemonData.postData.data, &wk->UploadPokemonData, NULL );

	// 引き取った事にする
	WorldTradeData_SetFlag( wk->param->worldtrade_data, 0 );

	// セーブへ
	wk->subprocess_seq = SUBSEQ_SAVE;

	// セーブ込みシーケンス予約
	SetSaveNextSequence( wk, SUBSEQ_DOWNLOAD_EX_FINISH, SUBSEQ_DOWNLOAD_SUCCESS_MESSAGE);
	
	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   交換済みデータ安定処理開始
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_DownloadExFinish( WORLDTRADE_WORK *wk )
{

	// 交換済みポケモンサーバーから削除処理
	Dpw_Tr_DeleteAsync();
	OS_TPrintf("-------------------------------------Dpw_Tr_DeleteAsyncよびだし\n");
	OS_TPrintf("ダウンロード終了処理開始\n");

	// セーブへ
	wk->subprocess_seq = SUBSEQ_DOWNLOAD_EX_FINISH_RESULT;
	wk->timeout_count = 0;
	
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   結果待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_DownloadExFinishResult( WORLDTRADE_WORK *wk)
{
	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();
		wk->timeout_count = 0;
		switch (result){
		case 0:		// 正常に動作している

			// 引き取ったポケモンが交換成立していたらひきとったポケモンデータは間違っているので放棄
			// もう一度サーバーチェックに戻る
			OS_TPrintf(" downloadEx is right!\n");

			wk->subprocess_seq = SUBSEQ_SAVE_LAST;
			break;

		// データが無い（かなりおかしい状況、さっきは落とせたのに）
		case DPW_TR_ERROR_NO_DATA :	
			OS_TPrintf(" download server stop service.\n");
      wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_ERROR_MESSAGE;

      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();
			break;

		// 1ヶ月過ぎてしまった
		case DPW_TR_ERROR_DATA_TIMEOUT :
			OS_TPrintf(" server full.\n");


	// -----------------------------------------
	// 共通エラー処理
	// -----------------------------------------
		case DPW_TR_ERROR_CANCEL :
			wk->ConnectErrorNo = result;

		case DPW_TR_ERROR_FAILURE :
			// 「GTSのかくにんにしっぱいしました」
		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
			// サーバーと通信できません→終了
			OS_TPrintf(" upload error. %d \n", result);

			// ９９％までセーブが書き込まれた状況では元に戻せないので
			// 無理矢理通信エラーへ
			WorldTrade_DispCallFatal(wk);

			break;
		case DPW_TR_ERROR_FATAL:			//!< 通信致命的エラー。電源の再投入が必要です
			// 即ふっとばし
      WorldTrade_DispCallFatal(wk);
			break;
	// -----------------------------------------
		}
		
	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      WorldTrade_DispCallFatal(wk);
		}
	}
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
static int Subseq_Main( WORLDTRADE_WORK *wk)
{


	return SEQ_MAIN;
}



//------------------------------------------------------------------
/**
 * @brief   あずけるのに成功しましたメッセージ
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_UploadSuccessMessage( WORLDTRADE_WORK *wk )
{
	
	// あずけたフラグを立てる
	wk->DepositFlag     = 1;

	// デモへ行く予約をしておく
	WorldTrade_SubProcessChange( wk, WORLDTRADE_DEMO, MODE_UPLOAD );
	
	wk->subprocess_seq = SUBSEQ_END;
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   ひきとるのに成功しましたメッセージ
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_DownloadSuccessMessage( WORLDTRADE_WORK *wk)
{
	
	// あずけたフラグを落とす
	 wk->DepositFlag     = 0;


	// デモへ行く予約をしておく
	WorldTrade_SubProcessChange( wk, WORLDTRADE_DEMO, MODE_DOWNLOAD );
	
	wk->subprocess_seq = SUBSEQ_END;

	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   交換に成功しましたメッセージ
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ExchangeSuccessMessage( WORLDTRADE_WORK *wk)
{
	
	// デモへ行く予約をしておく
	WorldTrade_SubProcessChange( wk, WORLDTRADE_DEMO, MODE_EXCHANGE );

	wk->subprocess_seq = SUBSEQ_END;

	
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   あたらしいポケモンがやってきました
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_DownloadExSuccessMessage( WORLDTRADE_WORK *wk)
{
	
	// あずけたフラグを落とす
	 wk->DepositFlag     = 0;

	// デモへ行く予約をしておく
	WorldTrade_SubProcessChange( wk, WORLDTRADE_DEMO, MODE_DOWNLOAD_EX );
	
	wk->subprocess_seq = SUBSEQ_SAVE;

	return SEQ_MAIN;
	
}



#if GTS_DUPLICATE_BUG_FIX


//------------------------------------------------------------------
/**
 * @brief   ポケモン複製が確認されたので、サーバーのポケモンを消去する
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ServerPokeDelete( WORLDTRADE_WORK *wk)
{
	Dpw_Tr_ReturnAsync();
	OS_TPrintf("----------------------------------強制ポケモン削除命令-Dpw_Tr_ReturnAsyncよびだし\n");
	
	wk->subprocess_seq = SUBSEQ_SERVER_POKE_DELETE_WAIT;
	wk->timeout_count = 0;

	
	return SEQ_MAIN;


}

//------------------------------------------------------------------
/**
 * @brief   サーバー内ポケモン強制消去の終了待ち
 *
 * @param   wk		
 *
 * @retval  int		（ちなみにこの後、ＧＴＳメインメニューに戻る）
 */
//------------------------------------------------------------------
static int Subseq_ServerPokeDeleteWait( WORLDTRADE_WORK *wk)
{
	if (Dpw_Tr_IsAsyncEnd()){
		s32 result = Dpw_Tr_GetAsyncResult();

		wk->timeout_count = 0;
		switch(result){
		case 0:
			// サーバーからの削除成功
			OS_TPrintf(" duplicate pokemon delete success.\n");
			AfterTradeCheck_ProcessControl( wk );
			break;
		// データが無い（かなりおかしい状況、さっきは落とせたのに）
		case DPW_TR_ERROR_NO_DATA :	
			OS_TPrintf(" download server stop service.\n");
			AfterTradeCheck_ProcessControl( wk );

		// 1ヶ月過ぎてしまった(来ないような気がする）
		case DPW_TR_ERROR_DATA_TIMEOUT :
			OS_TPrintf(" timeout.\n");
			AfterTradeCheck_ProcessControl( wk );

		// 最後のつめを行おうとしたら交換が成立してしまった。
		// エラーが起きた事にして外にだしてしまう→もどってくれば交換が成立してます。
		case DPW_TR_ERROR_ILLIGAL_REQUEST:
      wk->ConnectErrorNo = result;
			wk->subprocess_seq = SUBSEQ_ERROR_MESSAGE;

      NetErr_ExitNetSystem();
      GFL_NET_StateClearWifiError();
      NetErr_ErrWorkInit();
      GFL_NET_StateResetError();
			break;
	// -----------------------------------------
	// 共通エラー処理
	// -----------------------------------------
		case DPW_TR_ERROR_CANCEL :

		case DPW_TR_ERROR_FAILURE :
			// 「GTSのかくにんにしっぱいしました」
		case DPW_TR_ERROR_SERVER_TIMEOUT :
		case DPW_TR_ERROR_DISCONNECTED:	
			// サーバーと通信できません→終了
			OS_TPrintf(" upload error. %d \n", result);

			// セーブがほとんど書き込まれている状況ではデータを元に戻せない
			// 無理矢理通信エラーへ
      WorldTrade_DispCallFatal(wk);
			break;
		case DPW_TR_ERROR_FATAL:			//!< 通信致命的エラー。電源の再投入が必要です
			// 即ふっとばし
      WorldTrade_DispCallFatal(wk);
			break;
	// -----------------------------------------

		}

	}
	else{
		wk->timeout_count++;
		if(wk->timeout_count == TIMEOUT_TIME){
      WorldTrade_DispCallFatal(wk);
		}
	}
	return SEQ_MAIN;

}
#endif


//------------------------------------------------------------------
/**
 * @brief   交換しようとしたポケモンが既に交換されてしまった
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ExchangeFailedMessage( WORLDTRADE_WORK *wk)
{
	Enter_MessagePrint( wk, wk->MsgManager, msg_gtc_15_003, 1, 0x0f0f );
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_KEYWAIT, SUBSEQ_END );
	
	WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );

	// 時間アイコン消去(２重解放になるのでNULLチェックはする）
	WorldTrade_TimeIconDel( wk );

	// 交換失敗なので下画面のＯＢＪを隠す
	WorldTrade_SubLcdMatchObjHide( wk );
	
	return SEQ_MAIN;
}


//------------------------------------------------------------------
/**
 * @brief   エラーに対応したメッセージでプリント
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static void PrintError( WORLDTRADE_WORK *wk )
{
	int msgno = msg_gtc_error_004_01;

	// エラーが発生しました。
	switch(wk->ConnectErrorNo){
	case DPW_TR_ERROR_SERVER_FULL:
		// サーバーがいっぱいです。しばらくしてからきてください
		msgno = msg_gtc_error_002;
		break;
	case DPW_TR_ERROR_SERVER_TIMEOUT:
		// ＧＴＳとのせつぞくがきれました。うけつけにもどります
		msgno = msg_gtc_error_006_1;
		break;
	case DPW_TR_ERROR_NO_DATA:
		//　つうしんエラーが発生しました。
		msgno = msg_gtc_error_004_03;
		break;
	case DPW_TR_ERROR_DATA_TIMEOUT:
		//　つうしんエラーが発生しました。
		msgno = msg_gtc_error_004_05;
		break;
	case DPW_TR_ERROR_ILLIGAL_REQUEST :
		//　つうしんエラーが発生しました。
		msgno = msg_gtc_error_004_04;
		break;
	case DPW_TR_ERROR_ILLEGAL_DATA:
    msgno = msg_gtc_01_027_1;
    break;
	case DPW_TR_ERROR_CHEAT_DATA:		//!< アップロードされたデータが不正
    msgno = msg_gtc_01_027_2;
    break;
	case DPW_TR_ERROR_NG_POKEMON_NAME:	//!< アップロードされたポケモンの名前がNGワードを含んでいる
    msgno = msg_gtc_01_027_3;
    break;
	case DPW_TR_ERROR_NG_PARENT_NAME:	//!< アップロードされたポケモンの親の名前がNGワードを含んでいる
    msgno = msg_gtc_01_027_4;
    break;
	case DPW_TR_ERROR_NG_MAIL_NAME:	//!< アップロードされたメールの名前がNGワードを含んでいる
    msgno = msg_gtc_01_027_5;
    break;
	case DPW_TR_ERROR_NG_OWNER_NAME:	//!< アップロードされた主人公名がNGワードを含んでいる
    msgno = msg_gtc_01_027_6;
    break;
  case DPW_TR_ERROR_CANCEL:
		//　つうしんエラーが発生しました。
		msgno = msg_gtc_error_004_01;
		break;
  case DPW_TR_ERROR_FATAL:
    msgno = msg_gtc_error_004_06;
		break;
	case DPW_TR_ERROR_DISCONNECTED:
		// ＧＴＳとのせつぞくがきれました。うけつけにもどります
		msgno = msg_gtc_error_006_2;
		break;
	case DPW_TR_ERROR_FAILURE :
		//　つうしんエラーが発生しました。
		msgno = msg_gtc_error_004_02;
		break;
  default:
		//　つうしんエラーが発生しました。
		msgno = msg_gtc_error_004;
    break;
	}

	OS_TPrintf("error %d\n", wk->ConnectErrorNo);
	// エラーに対応したプリント
	Enter_MessagePrint( wk, wk->MsgManager, msgno, 1, 0x0f0f );

	
}


//------------------------------------------------------------------
/**
 * @brief   GTS終了エラーのメッセージ表示（この後強制終了）
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ErrorMessage( WORLDTRADE_WORK *wk )
{

	// エラーに対応したプリント
	PrintError(wk);

  //非同期処理が終わっていないければキャンセル
  if( Dpw_Tr_IsAsyncEnd() )
  {
    WorldTrade_SetNextSeq( wk, SUBSEQ_MES_KEYWAIT, SUBSEQ_END_ERR );
  }
  else
  {
    WorldTrade_SetNextSeq( wk, SUBSEQ_MES_KEYWAIT, SUBSEQ_START_CANCEL_ASYNC );
  }
	WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, MODE_WIFILOGIN_ERR );
	
	// 時間アイコン消去(２重解放になるのを気をつける）
	WorldTrade_TimeIconDel(wk);

	return SEQ_MAIN;
}

//----------------------------------------------------------------------------
/**
 *	@brief  DPWの終了処理
 *
 *	@param	WORLDTRADE_WORK *wk 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static int Subseq_EndDpw( WORLDTRADE_WORK *wk )
{
  //非同期処理が終わっていないければキャンセル
  if( Dpw_Tr_IsAsyncEnd() )
  {
    wk->subprocess_seq      = SUBSEQ_END_ERR;
  }
  else
  {
    wk->subprocess_seq      = SUBSEQ_START_CANCEL_ASYNC;
  }
	WorldTrade_SubProcessChange( wk, WORLDTRADE_ENTER, MODE_WIFILOGIN_ERR );

  return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   アクセスに失敗して、エラーを表示してタイトルへ
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int Subseq_ReturnTitleMessage( WORLDTRADE_WORK *wk )
{	
	// エラーに対応したプリント
	PrintError(wk);
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_KEYWAIT, SUBSEQ_END );
	WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );
	
	// 時間アイコン消去(SUBSEQ_ENDと２重解放になるのでNULLチェックは必要
	WorldTrade_TimeIconDel(wk);

	// goto
  if( wk->SearchResult > 0 )
  {
    WorldTrade_SubLcdMatchObjHide( wk );
  }

	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   進化後に「セーブ中」表示して終了する流れの最初
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_NowSaveMessage( WORLDTRADE_WORK *wk )
{
	WorldTrade_SetNextSeq( wk, SUBSEQ_MES_WAIT, SUBSEQ_SAVE );

	// 進化後セーブはサーバーアクセスはないので、素直にセーブして終わる
	SetSaveNextSequence( wk, SUBSEQ_SAVE_LAST, SUBSEQ_END);
	
	return SEQ_MAIN;
}



//------------------------------------------------------------------
/**
 * @brief   セーブ処理呼び出し
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_Save( WORLDTRADE_WORK *wk )
{

	// セーブ初期化
  { 
    GAMEDATA  *gamedata = GAMESYSTEM_GetGameData( wk->param->gamesys );
    GAMEDATA_SaveAsyncStart(gamedata);
  }


	wk->subprocess_seq = SUBSEQ_SAVE_RANDOM_WAIT;
	wk->wait           = GFUser_GetPublicRand(60)+2;

	OS_TPrintf("セーブ開始 wait=%d\n", wk->wait);

	return SEQ_MAIN;

}



//------------------------------------------------------------------
/**
 * @brief   セーブ開始までのランダム終了待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SaveRandomWait( WORLDTRADE_WORK *wk )
{
	wk->wait--;
	if(wk->wait==0){
		wk->subprocess_seq = SUBSEQ_SAVE_WAIT;
	}
	
	return SEQ_MAIN;
}

//------------------------------------------------------------------
/**
 * @brief   セーブ処理終了待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SaveWait( WORLDTRADE_WORK *wk )
{
  GAMEDATA  *gamedata = GAMESYSTEM_GetGameData( wk->param->gamesys );
#ifdef DEBUG_SAVE_NONE
	if(1){	
#else
	if(GAMEDATA_SaveAsyncMain(gamedata)==SAVE_RESULT_LAST){
#endif

		// セーブシーケンスにくるまでに次の設定は終えているので、SUBSEQ_ENDだけでよい
		wk->subprocess_seq = wk->saveNextSeq1st;
		OS_TPrintf("セーブ９９％終了\n");

	}

	return SEQ_MAIN;
	
}


//------------------------------------------------------------------
/**
 * @brief   セーブ最後の１セクタ書き込み呼び出し処理
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_SaveLast( WORLDTRADE_WORK *wk )
{
  GAMEDATA  *gamedata = GAMESYSTEM_GetGameData( wk->param->gamesys );
#ifdef DEBUG_SAVE_NONE
	if(1){	
#else
	if(GAMEDATA_SaveAsyncMain(gamedata)==SAVE_RESULT_OK){
#endif
		wk->subprocess_seq = wk->saveNextSeq2nd;

		OS_TPrintf("セーブ100％終了\n");
		// 時間アイコン消去
		WorldTrade_TimeIconDel(wk);
	}
	
	
	return SEQ_MAIN;
	
}

//------------------------------------------------------------------
/**
 * @brief   セーブ処理呼び出し
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_TimeoutSave( WORLDTRADE_WORK *wk )
{
	// セーブ初期化
  { 
    GAMEDATA  *gamedata = GAMESYSTEM_GetGameData( wk->param->gamesys );
    GAMEDATA_SaveAsyncStart(gamedata);
  }
	wk->subprocess_seq = SUBSEQ_TIMEOUT_SAVE_WAIT;

	return SEQ_MAIN;

}



//------------------------------------------------------------------
/**
 * @brief   セーブ処理終了待ち
 *
 * @param   wk		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int SubSeq_TimeoutSaveWait( WORLDTRADE_WORK *wk )
{
  GAMEDATA  *gamedata = GAMESYSTEM_GetGameData( wk->param->gamesys );
#ifdef DEBUG_SAVE_NONE
	if(1){	
#else
	if(GAMEDATA_SaveAsyncMain(gamedata)==SAVE_RESULT_OK){
#endif

		// タイトル画面に戻る設定
		WorldTrade_SubProcessChange( wk, WORLDTRADE_TITLE, 0 );

		// 時間アイコン消去(２重解放になるのを気をつける）
		WorldTrade_TimeIconDel(wk);

		// ●●●はこうかんされませんでした…
		Enter_MessagePrint( wk, wk->MsgManager, wk->error_mes_no, 1, 0x0f0f );
		WorldTrade_SetNextSeq( wk, SUBSEQ_MES_KEYWAIT, SUBSEQ_SERVER_TRADE_CHECK_END );

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
static int Subseq_End( WORLDTRADE_WORK *wk)
{
	// 必ず時間アイコンを消去(２重解放対策はしておく）
	WorldTrade_TimeIconDel(wk);
	
	//WirelessIconEasyEnd();

	if( wk->sub_out_flg == 1 ){
    //既にサブの輝度が落ちているならばメインだけにする  20100621 mod Saito
    if ( GXS_GetMasterBrightness() == -16 )
    {
      NOZOMU_Printf("サブは輝度が落ちているからメインだけ\n");
      WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
    }
    else
    {
      WIPE_SYS_Start( WIPE_PATTERN_WMS, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
    }
#ifdef GTS_FADE_OSP
		OS_Printf( "******************** worldtrade_upload.c [1737] MS ********************\n" );
#endif
	}else{
		WIPE_SYS_Start( WIPE_PATTERN_M, WIPE_TYPE_FADEOUT, WIPE_TYPE_FADEOUT, WIPE_FADE_BLACK, WORLDTRADE_WIPE_SPPED, 1, HEAPID_WORLDTRADE );
#ifdef GTS_FADE_OSP
		OS_Printf( "******************** worldtrade_upload.c [1741] M ********************\n" );
#endif
	}
	wk->subprocess_seq = 0;
	
	return SEQ_FADEOUT;
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
static int Subseq_MessageWait( WORLDTRADE_WORK *wk )
{
	if( GF_MSG_PrintEndCheck( &wk->print )==0 ) {
		wk->subprocess_seq = wk->subprocess_nextseq;
	}
	return SEQ_MAIN;

}

//----------------------------------------------------------------------------
/**
 *	@brief  会話終了をキーかタッチで待ってから次のシーケンスへ
 *
 *	@param	WORLDTRADE_WORK *wk 
 *
 *	@return int 
 */
//-----------------------------------------------------------------------------
static int Subseq_MessageKeyWait( WORLDTRADE_WORK *wk )
{
	if( GF_MSG_PrintEndCheck( &wk->print )==0
      && (( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE ))
          || ( GFL_UI_TP_GetTrg() ) ) {
    PMSND_PlaySE( SEQ_SE_MESSAGE );
		wk->subprocess_seq = wk->subprocess_nextseq;
	}
	return SEQ_MAIN;
}



//------------------------------------------------------------------
/**
 * @brief   預けたポケモンデータをBOXかてもちから消去する
 *
 * @param   pp		
 * @param   flag	格納フラグを立てるか?(1:立てる 0:立てない）
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void UploadPokemonDataDelete( WORLDTRADE_WORK *wk, int flag )
{
//	BOXDAT_PutPokemonBox( BOX_MANAGER* box, u32 boxNum, POKEMON_PASO_PARAM* poke );

	// 手持ちからでなければBOXのポケモンを消去する
	if(wk->BoxTrayNo!=WORLDTRADE_BOX_TEMOTI){
		POKEMON_PARAM *pp = PokemonParam_AllocWork(HEAPID_WORLDTRADE);
		PokeReplace(
			BOXDAT_GetPokeDataAddress( wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos ),
			pp
		);

    if( flag )
    {
      WorldTradeData_SetPokemonData( wk->param->worldtrade_data, pp, wk->BoxTrayNo );
    }

		// ボックスから消去
		BOXDAT_ClearPokemon( wk->param->mybox, wk->BoxTrayNo, wk->BoxCursorPos );
		OS_Printf("box %d, %d のポケモンを削った\n", wk->BoxTrayNo, wk->BoxCursorPos);

		GFL_HEAP_FreeMemory(pp);
	}else{
	// てもち

		POKEMON_PARAM *pp = PokeParty_GetMemberPointer(wk->param->myparty, wk->BoxCursorPos);
		OS_Printf("てもちから消去 pos = %d\n", wk->BoxCursorPos);


		// セーブ領域に保存
    if( flag )
    {
      WorldTradeData_SetPokemonData( wk->param->worldtrade_data, pp, wk->BoxTrayNo );
    }

		// 手持ちから消去
		PokeParty_Delete( wk->param->myparty, wk->BoxCursorPos );

		// てもちからペラップがいなくなったら声データを消去する
    { 
      GAMEDATA  *gamedata = GAMESYSTEM_GetGameData( wk->param->gamesys );
      PERAPVOICE *perap   = GAMEDATA_GetPerapVoice( gamedata );
      POKEPARTY *temoti   = GAMEDATA_GetMyPokemon( gamedata );
      PERAPVOICE_CheckPerapInPokeParty( perap, temoti );
    }
  }
	
	if(flag){
		// 預けたフラグ
		WorldTradeData_SetFlag( wk->param->worldtrade_data, 1 );
	}
}

//------------------------------------------------------------------
/**
 * @brief   引き取る処理
 *
 * @param   wk		
 * @param   pp		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void DownloadPokemonDataAdd( WORLDTRADE_WORK *wk, POKEMON_PARAM *pp, int boxno, int flag )
{
	int itemno = PP_Get(pp, ID_PARA_item, NULL);

	// 図鑑等の登録処理
	ZUKANSAVE_SetPokeGet( wk->param->zukanwork, pp );

	// なにはともあれてもちに入れようとする
	// ボックスに入れようとしている時にポケモンにメールがついている場合は
	// てもちに入れるようにする
	boxno = WORLDTRADE_BOX_TEMOTI;

	if(PokeParty_GetPokeCount(wk->param->myparty)==6){
		// てもちがいっぱいだったらボックスに
		boxno = 0;
	}

	// 交換が成立していた
	if(flag){
		u8 friend = FIRST_NATUKIDO;

#ifdef ARUCEUSU_EVENT_CHECK
		//受け取るポケモンがアルセウスイベント起動条件を満たしているならフラグセット
		if(PP_Get(pp, ID_PARA_monsno, NULL) == MONSNO_ARUSEUSU){
			if(PP_Get(pp, ID_PARA_event_get_flag, NULL) || 
					(PP_Get(pp, ID_PARA_birth_place, NULL) == MAPNAME_D5ROOM 
					&& PP_Get(pp, ID_PARA_event_get_flag, NULL) == 0)){
				EVENTWORK* ev = SaveData_GetEventWork(wk->param->savedata);
				if( SysWork_AruseusuEventGet(ev) == 0 ){
					SysWork_AruseusuEventSet( ev, 1 );
				}
			}
		}
#endif //ARUCEUSU_EVENT_CHECK

		// なつき度を初期値７０にする
		PP_Put( pp, ID_PARA_friend, friend );

		// 個性乱数から取得される性別とPOKEPARAに格納されている性別が違った場合に修正する
		// 配布ブーバーン・エレキブル・ペラップ・エレブー・ブーバー対策
		// 性別再計算
		PP_Put( pp, ID_PARA_sex, POKETOOL_GetSex( PP_Get( pp, ID_PARA_monsno, NULL ), PP_Get( pp, ID_PARA_form_no, NULL ), PP_Get( pp, ID_PARA_personal_rnd, NULL )  ) );

		// ポケモンの交換成立最終日をセーブする
		TradeDateUpDate( wk->param->worldtrade_data, TRADE_TYPE_DEPOSIT );


	}

	// てもち(てもちが一杯はこの時点だとどうしようもないのでこないようにしないと）
	if(boxno==WORLDTRADE_BOX_TEMOTI){
		int num;

		// メールがついているので、手持ちにしか受け取れない
		PokeParty_Add(wk->param->myparty, pp);
		num = PokeParty_GetPokeCount( wk->param->myparty );

		wk->EvoPokeInfo.boxno = WORLDTRADE_BOX_TEMOTI;
		wk->EvoPokeInfo.pos   = num-1;

	}else{
	// ボックス
		
		int boxpos=0;
		// メールが無ければBOXに入れる

		// ボックスの空いているところを探す
		BOXDAT_GetEmptyTrayNumberAndPos( wk->param->mybox, &boxno, &boxpos );

		// 受け取ったポケモンを格納する
		BOXDAT_PutPokemonBox( wk->param->mybox, boxno, PPPPointerGet(pp) );
		
		wk->EvoPokeInfo.boxno = boxno;
		wk->EvoPokeInfo.pos   = boxpos;
	}

	// 世界交換ワークから預けてるフラグを落とす
	WorldTradeData_SetFlag( wk->param->worldtrade_data, 0 );



}


//------------------------------------------------------------------
/**
 * @brief   検索してみつけたポケモンと交換する時の処理
 *
 * @param   wk		
 * @param   pp		  受け取ったポケモン
 * @param   sendpp  あげたポケモン
 * @param   tr      交換した人のデータ
 * @param   boxno		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void ExchangePokemonDataAdd( WORLDTRADE_WORK *wk, POKEMON_PARAM *pp, POKEMON_PARAM *sendpp, Dpw_Tr_Data *tr,int boxno )
{

	// 図鑑登録処理
  ZUKANSAVE_SetPokeGet( wk->param->zukanwork, pp );

	boxno = WORLDTRADE_BOX_TEMOTI;
	if(PokeParty_GetPokeCount(wk->param->myparty)==6){
		// てもちがいっぱいだったら格納先をボックスに
		OS_Printf("格納先はBOX\n");
		boxno = 0;
	}

#ifdef ARUCEUSU_EVENT_CHECK
	//受け取るポケモンがアルセウスイベント起動条件を満たしているならフラグセット
	if(PP_Get(pp, ID_PARA_monsno, NULL) == MONSNO_ARUSEUSU){
		if(PP_Get(pp, ID_PARA_event_get_flag, NULL) || 
				(PP_Get(pp, ID_PARA_birth_place, NULL) == MAPNAME_D5ROOM 
				&& PP_Get(pp, ID_PARA_event_get_flag, NULL) == 0)){
			EVENTWORK* ev = SaveData_GetEventWork(wk->param->savedata);
			if( SysWork_AruseusuEventGet(ev) == 0 ){
				SysWork_AruseusuEventSet( ev, 1 );
			}
		}
	}
#endif //ARUCEUSU_EVENT_CHECK

	{
		// 交換されたポケモンに入れるなつき度
		u8 friend = FIRST_NATUKIDO;
		PP_Put(pp, ID_PARA_friend, friend);
	}
	
	// 個性乱数から取得される性別とPOKEPARAに格納されている性別が違った場合に修正する
	// 配布ブーバーン・エレキブル・ペラップ・エレブー・ブーバー対策
	// 性別再計算
  PP_Put( pp, ID_PARA_sex, POKETOOL_GetSex( PP_Get( pp, ID_PARA_monsno, NULL ), PP_Get( pp, ID_PARA_form_no, NULL ), PP_Get( pp, ID_PARA_personal_rnd, NULL )  ) );

	// てもち(てもちが一杯はこの時点だとどうしようもないので）
	if(boxno==WORLDTRADE_BOX_TEMOTI){
		int num;

		// メールがついているので、手持ちにしか受け取れない
		PokeParty_Add(wk->param->myparty, pp);
		num = PokeParty_GetPokeCount( wk->param->myparty );

		wk->EvoPokeInfo.boxno = WORLDTRADE_BOX_TEMOTI;
		wk->EvoPokeInfo.pos   = num-1;
		OS_Printf("てもちに追加した\n");

	}else{
	// ボックス
		
		int boxpos=0;
		// メールが無ければBOXに入れる

		// ボックスの空いているところを探す
		BOXDAT_GetEmptyTrayNumberAndPos( wk->param->mybox, &boxno, &boxpos );

		// 受け取ったポケモンを格納する
		BOXDAT_PutPokemonBox( wk->param->mybox, boxno, PPPPointerGet(pp) );

		wk->EvoPokeInfo.boxno = boxno;
		wk->EvoPokeInfo.pos   = boxpos;

		OS_Printf("BOXの %d トレイに追加した\n", boxno);

	}


/* GTSで自分でポケモンを預けている時に、自分から検索してポケモンを交換した際
   セーブデータの「ポケモンをGTSに預けているフラグ」を落としてしまっているバグに対処 */

  //この処理はいらない
	//WorldTradeData_SetFlag( wk->param->worldtrade_data, 0 );


	// ポケモンの交換成立最終日をセーブする
	TradeDateUpDate( wk->param->worldtrade_data, TRADE_TYPE_SEARCH );
	
}

//------------------------------------------------------------------
/**
 * @brief   最終交換日付を更新
 *
 * @param   worldtrade_data		
 * @param   trade_type			(TRADE_TYPE_SEARCH or TRADE_TYPE_DEPOSIT)
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void TradeDateUpDate( WORLDTRADE_DATA *worldtrade_data, int trade_type )
{
	RTCDate date;
	RTCTime time;
	GFDATE gfDate;

	// サーバー基準の現在時刻を取得
	DWC_GetDateTime( &date, &time);

	// 最終交換日付として保存
	gfDate = GFDATE_RTCDate2GFDate( &date );
	if(trade_type == TRADE_TYPE_SEARCH){
		WorldTradeData_SetLastDate_Search( worldtrade_data, gfDate );
		OS_TPrintf("検索して成立 ");
	}
	else{
		WorldTradeData_SetLastDate_Deposit( worldtrade_data, gfDate );
		OS_TPrintf("預けて成立 ");
	}
	OS_Printf(" %d年 %d月 %d日に交換成立\n",
				GFDATE_GetYear( gfDate ),GFDATE_GetMonth( gfDate ),GFDATE_GetDay( gfDate ));

}

//------------------------------------------------------------------
/**
 * @brief   相手の交換情報を参照して地球儀情報を登録する
 *
 * @param   wifiHistory		
 * @param   trData		
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void WifiHistoryDataSet( WIFI_HISTORY *wifiHistory, Dpw_Tr_Data *trData )
{

  OS_Printf("Nation=%d area=%d langCode=%d\n", trData->countryCode, trData->localCode, trData->langCode );
	Comm_WifiHistoryDataSet( wifiHistory, trData->countryCode, trData->localCode, trData->langCode );
	
}

#define BOX_MAX_NUM	(BOX_MAX_RAW*BOX_MAX_COLUMN*BOX_MAX_TRAY)

//------------------------------------------------------------------
/**
 * @brief   自分はポケモンを受け取れるか？メールがついていても受け取れる？
 *
 * @param   wk		
 * @param   trData		
 *
 * @retval  int		
 */
//------------------------------------------------------------------
static int MyPokemonPocketFullCheck( WORLDTRADE_WORK *wk, Dpw_Tr_Data *trData)
{
	
	// うけとるポケモンがメールをもっている時はてもちに空き無いといけない
	if( WorldTrade_PokemonMailCheck( (POKEMON_PARAM*)trData->postData.data ) 
		&&  PokeParty_GetPokeCount(wk->param->myparty) == 6){
		return POKEMON_NOT_FULL_BUT_MAIL_NORECV;
	}

	OS_Printf("boxnum = %d, temochi = %d\n", wk->boxPokeNum, PokeParty_GetPokeCount(wk->param->myparty));
	// とにかくてもちもボックスも空きがない
	if(wk->boxPokeNum == BOX_MAX_NUM && PokeParty_GetPokeCount(wk->param->myparty) == 6){
		return POKEMON_ALL_FULL;
	}

	// OK
	return POKEMON_RECV_OK;
}


//------------------------------------------------------------------
/**
 * @brief   セーブシーケンス後に移動する先を予約する
 *
 * @param   nextSeq1st		セーブ前半終了後に遷移するシーケンス
 * @param   nextSeq2nd		セーブ後半終了後に遷移するシーケンス
 *
 * @retval  none		
 */
//------------------------------------------------------------------
static void SetSaveNextSequence( WORLDTRADE_WORK *wk, int nextSeq1st, int nextSeq2nd )
{
	wk->saveNextSeq1st = nextSeq1st;
	wk->saveNextSeq2nd = nextSeq2nd;
	
}

#if GTS_DUPLICATE_BUG_FIX

//------------------------------------------------------------------
/**
 * @brief   複製操作チェック
 *
 * @param   wk		
 *
 * @retval  int		複製なら１、正常なら０
 */
//------------------------------------------------------------------
static int DupulicateCheck( WORLDTRADE_WORK *wk )
{
	POKEMON_PARAM *pp = (POKEMON_PARAM *)wk->UploadPokemonData.postData.data;

	// 複製チェックの内容は
	// 「ＧＴＳのセーブ領域に退避ポケモンが入っていないのに、
	// 　サーバーには預けてある状態になっていたら」を確認する事
	if(WorldTradeData_GetFlag( wk->param->worldtrade_data )==0){
		if(wk->DepositFlag){
			OS_Printf("セーブデータでは預けていないのに、サーバーは預けていると判断している\n");
			return 1;
		}
	}

	return 0;
}

#endif


//----------------------------------------------------------------------------
/**
 *	@brief  不正名を設定
 *
 *	@param	POKEMON_PARAM *pp ポケパラ
 */
//-----------------------------------------------------------------------------
static void WorldTrade_SetEvilName( POKEMON_PARAM *pp, HEAPID heapID )
{
  STRBUF * p_name = DWC_TOOL_CreateBadNickName( GFL_HEAP_LOWID(heapID) );

  //ニックネーム
  PP_SetDefaultNickName( pp );

  //親名
  PP_Put( pp, ID_PARA_oyaname_raw, (u32)GFL_STR_GetStringCodePointer(p_name) );

  //メールを持っていれば
  //書いた人も変える
  {
    int item = PP_Get( pp , ID_PARA_item ,NULL);

    if(ITEM_CheckMail(item))
    {
      MAIL_DATA*  p_mail  = MailData_CreateWork(GFL_HEAP_LOWID(heapID));
      PP_Get( pp, ID_PARA_mail_data, p_mail );
      MailData_SetWriterName( p_mail, (STRCODE*)GFL_STR_GetStringCodePointer(p_name));
      PP_Put( pp, ID_PARA_mail_data, (u32)p_mail );
      GFL_HEAP_FreeMemory( p_mail );
    }
  }
  GFL_STR_DeleteBuffer( p_name );
}
