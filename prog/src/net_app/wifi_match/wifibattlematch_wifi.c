//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_wifi.c
 *	@brief	WIFIのバトルマッチコア画面  WIFIシーケンス
 *	@author	Toru=Nagihashi
 *	@data		2009.11.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	ライブラリ
#include <gflib.h>

//	システム
#include "system/main.h"
#include "system/gfl_use.h"
#include "system/bmp_winframe.h"
#include "pokeicon/pokeicon.h"
#include "gamesystem/game_data.h"
#include "system/net_err.h"
#include "poke_tool/poke_regulation.h"
#include "sound/pm_sndsys.h"
#include "net/dwc_rapcommon.h"
#include "net/dwc_rap.h"
#include "net/dwc_tool.h"
#include "battle/btl_net.h" //BTL_NET_SERVER_VERSION

//	アーカイブ
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "wifimatch_gra.naix"
#include "msg/msg_wifi_match.h"

//	文字表示
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"

//  セーブデータ
#include "savedata/dreamworld_data.h"
#include "savedata/regulation.h"
#include "savedata/battle_box_save.h"
#include "savedata/my_pms_data.h"
#include "savedata/etc_save.h"

//WIFIバトルマッチのモジュール
#include "wifibattlematch_graphic.h"
#include "wifibattlematch_view.h"
#include "wifibattlematch_net.h"
#include "wifibattlematch_util.h"
#include "wifibattlematch_snd.h"

//デバッグ
#include "wifibattlematch_debug.h"

//外部公開
#include "wifibattlematch_core.h"

#include "bugfix.h"

//-------------------------------------
///	オーバーレイ
//=====================================
FS_EXTERN_OVERLAY(ui_common);
FS_EXTERN_OVERLAY(dpw_common);

//-------------------------------------
///	デバッグ
//=====================================
#ifdef PM_DEBUG


#if defined(DEBUG_ONLY_FOR_toru_nagihashi)||defined(DEBUG_ONLY_FOR_shimoyamada)
#define GPF_FLAG_ON             //GPFフラグを強制ONにする
#endif

//#define MYPOKE_SELFCHECK        //自分のポケモンを送ったとき、サケとチェックし署名も証明させる
//#define DEBUG_REGULATION_DATA   //レギュレーションデータを作成する
//#define REGULATION_CHECK_ON     //パーティのレギュレーションチェックを強制ONにする

//#define DEBUG_REG_CRC_CHECK_OFF

#define DEBUGWIN_USE
#endif //PM_DEBUG

#define SCREPORT_TIMEOUT_IS_DISCONNECT


#define DEBUG_WIFICUP_Printf(...)  OS_TFPrintf( 2, __VA_ARGS__ );

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	サブシーケンスの戻り値
//=====================================
typedef enum
{
  //登録解除処理の戻り値
  WBM_WIFI_SUBSEQ_UNREGISTER_RET_TRUE  = 0,//登録解除した
  WBM_WIFI_SUBSEQ_UNREGISTER_RET_FALSE,   //解除しなかった
  WBM_WIFI_SUBSEQ_UNREGISTER_RET_NONE,   //登録されていない
  WBM_WIFI_SUBSEQ_UNREGISTER_RET_SERVER,   //サーバー状態が悪い

  //大会期間チェックの戻り値
  WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMESAFE  = 0,//期間内
  WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEOVER,     //期間後
  WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEBEFORE,   //期間前
  WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER,   //サーバー状態が悪い

  //ポケモン不正チェックの戻り値
  WBM_WIFI_SUBSEQ_EVILCHECK_RET_SUCCESS  = 0,  //成功
  WBM_WIFI_SUBSEQ_EVILCHECK_RET_DARTY,         //１体でも不正ポケモンがいた
  WBM_WIFI_SUBSEQ_EVILCHECK_RET_NET_ERR,       //ネットエラー
  WBM_WIFI_SUBSEQ_EVILCHECK_RET_SERVER_ERR,       //サーバーレスポンスエラー


  WBM_WIFI_SUBSEQ_RET_NONE  = 0xFFFF,       //なし
} WBM_WIFI_SUBSEQ_RET;

//-------------------------------------
///	シンク
//=====================================
#define ENEMYDATA_WAIT_SYNC (0)
#define MATCHING_MSG_WAIT_SYNC (120)
#define SELECTPOKE_MSG_WAIT_SYNC (60)

#define MATCHING_TIMEOUT_SYNC   (60*20)

//=============================================================================
/**
 *        定義
*/
//=============================================================================
//-------------------------------------
///	メインワーク
//=====================================
typedef struct 
{
  // ------ 以下モジュール ------
	//グラフィック設定
	WIFIBATTLEMATCH_GRAPHIC_WORK	*p_graphic;

  //リソース読み込み
  WIFIBATTLEMATCH_VIEW_RESOURCE *p_res;

	//共通で使うフォント
	GFL_FONT			            *p_font;

  //ポケモンのレベル・性別マーク表示のみで使用しているフォント
	GFL_FONT			            *p_small_font;

	//共通で使うキュー
	PRINT_QUE				          *p_que;

	//共通で使うメッセージ
	GFL_MSGDATA	          		*p_msg;

	//共通で使う単語
	WORDSET				          	*p_word;

	//上画面情報
	PLAYERINFO_WORK         	*p_playerinfo;

	//対戦者情報
	MATCHINFO_WORK          	*p_matchinfo;

  //バトルボックス
  WBM_BTLBOX_WORK           *p_btlbox;

  //待機アイコン
  WBM_WAITICON_WORK         *p_wait;

  //テキスト面
  WBM_TEXT_WORK             *p_text;

  //リスト
  WBM_LIST_WORK             *p_list;

  //ネット
  WIFIBATTLEMATCH_NET_WORK  *p_net;

  //メインシーケンス
  WBM_SEQ_WORK              *p_seq;

  //サブシーケンス
  WBM_SEQ_WORK              *p_subseq;
  WBM_WIFI_SUBSEQ_RET       subseq_ret;


  // ------ 以下データ ------
  //不正チェックのときに使用するバッファ（それ以外は参照しない）
  WIFIBATTLEMATCH_NET_EVILCHECK_DATA  evilecheck_data;

  //カウンタ
  u32 cnt;

  //不正カウンタ
  u32 other_dirty_cnt;
  u32 my_dirty_cnt;

  //キャンセルシーケンス
  u32 cancel_seq;
  u32 match_timeout;

  BOOL is_send_report;

  WIFIBATTLEMATCH_NET_SC_STATE  sc_state;

  //サーバーから落としてきたマッチング相手の登録ポケモン
  POKEPARTY *p_other_party;


#ifdef BUGFIX_BTS7762_20100713
  BOOL  is_sc_before_err;
#endif //BUGFIX_BTS7762_20100713

  //引数
  WIFIBATTLEMATCH_CORE_PARAM  *p_param;

  REGULATION_CARDDATA         *p_reg;
  REGULATION_CARDDATA         recv_card;

} WIFIBATTLEMATCH_WIFI_WORK;

//=============================================================================
/**
 *					プロトタイプ
*/
//=============================================================================
//-------------------------------------
///	プロセス
//=====================================
static GFL_PROC_RESULT WIFIBATTLEMATCH_WIFI_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_WIFI_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_WIFI_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	WIFI大会シーケンス関数
//=====================================
static void WbmWifiSeq_Init( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//static void WbmWifiSeq_Join( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );  
////以前はメニューはこのソース内でしていましたが、
//battle_championshipの中でおこなうことになりました念のため消してはいませんがWbmWifiSeq_Joinはつながっていません。
static void WbmWifiSeq_RecvDigCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_CheckDigCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Register( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_EndRec( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_CupContinue( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_DisConnextSendTime( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_DisConnextEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_DisConnextCheckDate( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Err_ReturnLogin( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	サブシーケンス関数（シーケンス上で動くもの）
//=====================================
static void WbmWifiSubSeq_CheckDate( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSubSeq_UnRegister( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSubSeq_EvilCheck( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	モジュールを使いやすくまとめたもの
//=====================================
//プレイヤー情報
static void Util_PlayerInfo_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static void Util_PlayerInfo_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static BOOL Util_PlayerInfo_MoveIn( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static BOOL Util_PlayerInfo_MoveOut( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static void Util_PlayerInfo_CreateStay( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static void Util_PlayerInfo_RenewalData( WIFIBATTLEMATCH_WIFI_WORK *p_wk, PLAYERINFO_WIFI_UPDATE_TYPE type );
//対戦相手情報
static void Util_MatchInfo_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_enemy_data );
static void Util_MatchInfo_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static BOOL Util_MatchInfo_Main( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static void Util_Matchinfo_Clear( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
//バトルボックス
static void Util_BtlBox_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static void Util_BtlBox_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static BOOL Util_BtlBox_MoveIn( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static BOOL Util_BtlBox_MoveOut( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
//選択肢
typedef enum
{ 
  UTIL_LIST_TYPE_YESNO,
  UTIL_LIST_TYPE_JOIN,
  UTIL_LIST_TYPE_DECIDE,
  UTIL_LIST_TYPE_CUPMENU,
  UTIL_LIST_TYPE_CUPMENU_CONT,
  UTIL_LIST_TYPE_YESNO_DEFAULT_NO,
}UTIL_LIST_TYPE;
static void Util_List_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk, UTIL_LIST_TYPE type );
static void Util_List_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static u32 Util_List_Main( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
//サブシーケンス
static void Util_SubSeq_Start( WIFIBATTLEMATCH_WIFI_WORK *p_wk, WBM_SEQ_FUNCTION seq_function );
static WBM_WIFI_SUBSEQ_RET Util_SubSeq_Main( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
//自分のデータ作成
static void Util_InitMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, WIFIBATTLEMATCH_WIFI_WORK *p_wk );
static void Util_SetMyDataInfo( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, const WIFIBATTLEMATCH_WIFI_WORK *cp_wk );
static void Util_SetMyDataSign( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, const WIFIBATTLEMATCH_NET_EVILCHECK_DATA  *cp_evilecheck_data );
//ポケモン証明
static BOOL Util_VerifyPokeData( WIFIBATTLEMATCH_ENEMYDATA *p_data, POKEPARTY *p_party, HEAPID heapID );
//ポケパーティを比較
static BOOL Util_ComarepPokeParty( const POKEPARTY *cp_pokeparty_1, const POKEPARTY *cp_pokeparty_2 );
//キャンセルシーケンス
typedef enum
{
  UTIL_CANCEL_STATE_NONE, //何もしていない
  UTIL_CANCEL_STATE_WAIT, //キャンセル待機中（はい、いいえ待ち等）
  UTIL_CANCEL_STATE_EXIT, //待機処理を抜けた
  UTIL_CANCEL_STATE_DECIDE, //キャンセルを確定
}UTIL_CANCEL_STATE;
static UTIL_CANCEL_STATE Util_Cancel_Seq( WIFIBATTLEMATCH_WIFI_WORK *p_wk, BOOL is_cancel_enable, BOOL can_disconnect );

#ifdef BUGFIX_BTS7769_20100713
static void Util_Cancel_ResetSeq( WIFIBATTLEMATCH_WIFI_WORK *p_wk );
#endif //BUGFIX_BTS7769_20100713

//スコアセーブ
static BOOL Util_SaveScore( WIFIBATTLEMATCH_WIFI_WORK *p_wk, const WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA *cp_data );

//-------------------------------------
///	デバッグ
//=====================================
#ifdef DEBUGWIN_USE
#include "debug/debugwin_sys.h"
#define DEBUGWIN_GROUP_WIFIMATCH (50)

static void DEBUGWIN_Init( WIFIBATTLEMATCH_WIFI_WORK *p_wk, HEAPID heapID );
static void DEBUGWIN_Exit( WIFIBATTLEMATCH_WIFI_WORK *p_wk );

#else

#define DEBUGWIN_Init( ... )  /*  */
#define DEBUGWIN_Exit( ... )  /*  */
#endif //DEBUGWIN_USE

//=============================================================================
/**
 *					外部参照
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	WifiBattleMatchWifi_ProcData =
{	
	WIFIBATTLEMATCH_WIFI_PROC_Init,
	WIFIBATTLEMATCH_WIFI_PROC_Main,
	WIFIBATTLEMATCH_WIFI_PROC_Exit,
};

//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFIバトルマッチ画面	コアプロセス初期化
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_WIFI_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	WIFIBATTLEMATCH_WIFI_WORK	*p_wk;
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;


	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common));

	//ヒープ作成
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_CORE, 0x30000 );

	//プロセスワーク作成
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFIBATTLEMATCH_WIFI_WORK), HEAPID_WIFIBATTLEMATCH_CORE );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_WIFI_WORK) );
  p_wk->p_param = p_param;

  { 
    SAVE_CONTROL_WORK *p_sv       = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
    REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
    p_wk->p_reg    = RegulationSaveData_GetRegulationCard( p_reg_sv, REGULATION_CARD_TYPE_WIFI );
  }

	//グラフィック設定
	p_wk->p_graphic	= WIFIBATTLEMATCH_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_WIFIBATTLEMATCH_CORE );

  //リソース読み込み
  p_wk->p_res     = WIFIBATTLEMATCH_VIEW_LoadResource( WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic ), WIFIBATTLEMATCH_VIEW_RES_MODE_WIFI, HEAPID_WIFIBATTLEMATCH_CORE );

	//共通モジュールの作成
	p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_small_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_num_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
												NARC_message_wifi_match_dat, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_word	= WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_COUNTRY_BUFLEN, HEAPID_WIFIBATTLEMATCH_CORE );

	//モジュールの作成
  p_wk->p_net   = WIFIBATTLEMATCH_NET_Init( 
      p_wk->p_param->p_net_data,
      p_param->p_param->p_game_data, p_param->p_svl_result, HEAPID_WIFIBATTLEMATCH_CORE );
  p_wk->p_text  = WBM_TEXT_Init( BG_FRAME_M_TEXT, PLT_FONT_M, PLT_TEXT_M, CGR_OFS_M_TEXT, p_wk->p_que, p_wk->p_font, HEAPID_WIFIBATTLEMATCH_CORE );
  p_wk->p_seq   = WBM_SEQ_Init( p_wk, WbmWifiSeq_Init, HEAPID_WIFIBATTLEMATCH_CORE );
  p_wk->p_subseq   = WBM_SEQ_Init( p_wk, NULL, HEAPID_WIFIBATTLEMATCH_CORE );

  { 
    GFL_CLUNIT  *p_unit;
    p_unit  = WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    p_wk->p_wait  = WBM_WAITICON_Init( p_unit, p_wk->p_res, HEAPID_WIFIBATTLEMATCH_CORE );
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
  }

  p_wk->p_other_party     = PokeParty_AllocPartyWork( HEAPID_WIFIBATTLEMATCH_CORE );

  WIFIBATTLEMATCH_NETICON_SetDraw( p_wk->p_net, TRUE );

  //デバッグウィンドウ
  DEBUGWIN_Init( p_wk, HEAPID_WIFIBATTLEMATCH_CORE );

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFIバトルマッチ画面	コアプロセス破棄
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_WIFI_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{

	WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	= p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_param_adrs;

  //デバッグウィンドウ
  DEBUGWIN_Exit( p_wk );

  GFL_HEAP_FreeMemory( p_wk->p_other_party );

  WIFIBATTLEMATCH_NETICON_SetDraw( p_wk->p_net, FALSE );

	//モジュールの破棄
  WBM_WAITICON_Exit( p_wk->p_wait );
  WBM_SEQ_Exit( p_wk->p_subseq );
  WBM_SEQ_Exit( p_wk->p_seq );
  WBM_TEXT_Exit( p_wk->p_text );
  if( p_wk->p_net )
  { 
    WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );
  }
  Util_BtlBox_Delete( p_wk );
  Util_MatchInfo_Delete( p_wk );
  Util_PlayerInfo_Delete( p_wk );
  Util_List_Delete( p_wk );

	//共通モジュールの破棄
	WORDSET_Delete( p_wk->p_word );
	GFL_MSG_Delete( p_wk->p_msg );
	PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_small_font );
	GFL_FONT_Delete( p_wk->p_font );

  //リソース破棄
  WIFIBATTLEMATCH_VIEW_UnLoadResource( p_wk->p_res );

	//グラフィック破棄
	WIFIBATTLEMATCH_GRAPHIC_Exit( p_wk->p_graphic );

	//プロセスワーク破棄
	GFL_PROC_FreeWork( p_proc );

	//ヒープ破棄
	GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_CORE );
	
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(dpw_common));
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFIバトルマッチ画面	メインプロセス処理
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_WIFI_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_MAIN,
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
  };


	WIFIBATTLEMATCH_WIFI_WORK	*p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;

  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
    *p_seq  = SEQ_FADEIN_WAIT;
    break;

  case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{
      *p_seq  = SEQ_MAIN;
    }
    break;

  case SEQ_MAIN:
    //メインシーケンス
    WBM_SEQ_Main( p_wk->p_seq );

    if( WBM_SEQ_IsEnd( p_wk->p_seq ) )
    { 
      *p_seq  = SEQ_FADEOUT_START;
    }
    break;

  case SEQ_FADEOUT_START:
    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    *p_seq  = SEQ_FADEOUT_WAIT;
    break;

  case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{
      return GFL_PROC_RES_FINISH;
    }
  }

	//描画
	WIFIBATTLEMATCH_GRAPHIC_2D_Draw( p_wk->p_graphic );

	//プリント
	PRINTSYS_QUE_Main( p_wk->p_que );

  //テキスト
  WBM_TEXT_Main( p_wk->p_text );

  //待機アイコン
  WBM_WAITICON_Main( p_wk->p_wait );

  //BG
  WIFIBATTLEMATCH_VIEW_Main( p_wk->p_res );

  //NET
  WIFIBATTLEMATCH_NET_Main( p_wk->p_net );

  //文字表示
  if( p_wk->p_playerinfo )
  { 
    PLAYERINFO_PrintMain( p_wk->p_playerinfo, p_wk->p_que );
  }
  if( p_wk->p_matchinfo )
  { 
    MATCHINFO_PrintMain( p_wk->p_matchinfo, p_wk->p_que );
  }
  if( p_wk->p_btlbox )
  { 
    WBM_BTLBOX_PrintMain( p_wk->p_btlbox, p_wk->p_que );
  }

  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *  WIFIシーケンス
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  初期化処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Init( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  DEBUG_WIFICUP_Printf( " mode%d ret%d\n", p_param->mode, p_param->retmode );
  switch( p_param->mode )
  { 
  case WIFIBATTLEMATCH_CORE_MODE_START:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_RecvDigCard );
    break;

  case WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR:
    WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
    /* fallthrough */

  case WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE:
    Util_PlayerInfo_CreateStay( p_wk );
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_EndBattle );
    break;

  case WIFIBATTLEMATCH_CORE_MODE_ENDREC:
    Util_PlayerInfo_CreateStay( p_wk );
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_EndRec );
    break;
  }
}
#if 0
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  参加選択処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Join( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG,    //WIFIバトル大会に参加しますか？
    SEQ_START_JOIN_LIST,  //参加しますか？
    SEQ_WAIT_JOIN_LIST,   //待ち
    SEQ_START_INFO_MSG,    //説明文
    SEQ_START_CANCEL_MSG,   //大会へ参加するのをやめる？
    SEQ_START_CANCEL_LIST,
    SEQ_WAIT_CANCEL_LIST,
    SEQ_JOIN,

    SEQ_WAIT_MSG, //メッセージ待ち
  };
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_MSG:    //WIFIバトル大会に参加しますか？
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_01, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_JOIN_LIST );
    break;
  case SEQ_START_JOIN_LIST:  //参加しますか？
    Util_List_Create( p_wk, UTIL_LIST_TYPE_JOIN );
    *p_seq     = SEQ_WAIT_JOIN_LIST;
    break;
  case SEQ_WAIT_JOIN_LIST:   //待ち
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //さんかする
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_RecvDigCard );
          break;
        case 1: //せつめいをきく
          *p_seq = SEQ_START_INFO_MSG;
          break;
        case 2: //やめる
          *p_seq = SEQ_START_CANCEL_MSG;
          break;
        }
      }
    }
    break;
  case SEQ_START_INFO_MSG:    //説明文
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_02, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_MSG );
    break;
  case SEQ_START_CANCEL_MSG:   //大会へ参加するのをやめる？
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_03, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_CANCEL_LIST );
    break;
  case SEQ_START_CANCEL_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq     = SEQ_WAIT_CANCEL_LIST;
    break;
  case SEQ_WAIT_CANCEL_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //はい
          p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
          break;
        case 1: //いいえ
          *p_seq = SEQ_START_MSG;
          break;
        }
      }
    }
    break;

  case SEQ_WAIT_MSG: //メッセージ待ち
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
#endif
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  サーバ上のデジタル選手証データをダウンロード処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_RecvDigCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK_GAMESYNC_REG,

    SEQ_START_SEARCH_MSG,

    SEQ_START_DOWNLOAD_GPF_DATA,
    SEQ_WAIT_DOWNLOAD_GPF_DATA,
  
    SEQ_CHECK_SIGNIN,
    SEQ_START_NONE_MSG,
    SEQ_WAIT_NONE_MSG,

    SEQ_INIT_SAKE,
    SEQ_WAIT_SAKE,

    SEQ_END
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECK_GAMESYNC_REG:
    {
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      DREAMWORLD_SAVEDATA* p_dream  = DREAMWORLD_SV_GetDreamWorldSaveData( p_sv );
      if( DREAMWORLD_SV_GetAccount(p_dream) )
      {
        *p_seq  = SEQ_START_SEARCH_MSG;
      }
      else
      {
        *p_seq  = SEQ_START_NONE_MSG;
      }
    }
    break;

  case SEQ_START_SEARCH_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_04, WBM_TEXT_TYPE_WAIT );
    *p_seq  = SEQ_START_DOWNLOAD_GPF_DATA;
    break;

    //-------------------------------------
    ///	GPFデータ取得
    //=====================================
  case SEQ_START_DOWNLOAD_GPF_DATA:
    GFL_STD_MemClear( p_wk->p_param->p_gpf_data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA) );
    WIFIBATTLEMATCH_NET_StartRecvGpfData( p_wk->p_net, HEAPID_WIFIBATTLEMATCH_CORE );
    *p_seq = SEQ_WAIT_DOWNLOAD_GPF_DATA;
    break;

  case SEQ_WAIT_DOWNLOAD_GPF_DATA:
    { 
      WIFIBATTLEMATCH_RECV_GPFDATA_RET  ret;

      ret = WIFIBATTLEMATCH_NET_WaitRecvGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_RECV_GPFDATA_RET_SUCCESS )
      { 
        WIFIBATTLEMATCH_NET_GetRecvGpfData( p_wk->p_net, p_wk->p_param->p_gpf_data );
        *p_seq = SEQ_CHECK_SIGNIN;
      }
      else if( ret == WIFIBATTLEMATCH_RECV_GPFDATA_RET_DIRTY )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }
      else if( ret != WIFIBATTLEMATCH_RECV_GPFDATA_RET_UPDATE )
      { 
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し

          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	サインインチェック
    //=====================================
  case SEQ_CHECK_SIGNIN:
    if( p_wk->p_param->p_gpf_data->signin )
    { 
      *p_seq = SEQ_INIT_SAKE;
    }
    else
    { 
      *p_seq  = SEQ_START_NONE_MSG;
    }
    break;

  case SEQ_START_NONE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_05, WBM_TEXT_TYPE_STREAM );
    *p_seq  = SEQ_WAIT_NONE_MSG;
    break;

  case SEQ_WAIT_NONE_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    {
      //GPFに登録していないときは日時を送らないためそのまま終了
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextEnd );
    }
    break;

    //-------------------------------------
    ///	SAKEレコードID取得＆なかった場合のレコード作成処理
    //=====================================
  case SEQ_INIT_SAKE:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_NET_StartInitialize( p_wk->p_net );
      *p_seq  = SEQ_WAIT_SAKE;
    }
    break;

  case SEQ_WAIT_SAKE:
    {
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_NET_WaitInitialize( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_END;
      }
      
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し

          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CheckDigCard );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  デジタル選手証データチェック処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_CheckDigCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    //WEB書き込みフラグチェック
    SEQ_CHECK_WRITE,
    SEQ_SEND_GPF_STATUS,
    SEQ_WAIT_GPF_STATUS,

    //大会チェック
    SEQ_CHECK_DATA,
    SEQ_CHECK_STATUS,
    SEQ_START_END_MSG,
    SEQ_START_GIVEUP_MSG,
    SEQ_START_WRITE_GPF,
    SEQ_WAIT_WRITE_GPF,
    SEQ_START_WRITE_SAKE_DELETE_POKE,
    SEQ_WAIT_WRITE_SAKE_DELETE_POKE,

    //大会発見
    SEQ_START_MSG_CUPDATA,
    SEQ_START_DOWNLOAD_REG,
    SEQ_WAIT_DOWNLOAD_REG,
    SEQ_WAIT_DOWNLOAD_REG_SUCCESS,
    SEQ_WAIT_DOWNLOAD_REG_FAILURE,
    SEQ_START_SUBSEQ_CUPDATE,
    SEQ_WAIT_SUBSEQ_CUPDATE,
    SEQ_START_NOTCUP_MSG,

    //バトルボックスロック確認
    SEQ_CHECK_EXITS_REG,

    SEQ_START_MOVEIN_CARD,
    SEQ_WAIT_MOVEIN_CARD,

    SEQ_CHECK_LOCK,

    SEQ_START_BOX_REWRITE_MSG,
    SEQ_START_BOX_REWRITE_YESNO,
    SEQ_SELECT_BOX_REWRITE_YESNO,      // 上書き確認
    SEQ_START_BOX_REWRITE_CANCEL_MSG,
    SEQ_START_BOX_REWRITE_CANCEL_YESNO,
    SEQ_SELECT_BOX_REWRITE_CANCEL_YESNO,

    SEQ_START_BOX_LOCK_MSG,           //  通常ロック確認
    SEQ_START_BOX_LOCK_YESNO,
    SEQ_SELECT_BOX_LOCK_YESNO,
    SEQ_START_BOX_LOCK_CANCEL_MSG,
    SEQ_START_BOX_LOCK_CANCEL_YESNO,
    SEQ_SELECT_BOX_LOCK_CANCEL_YESNO,

    SEQ_START_MOVEOUT_CARD,
    SEQ_WAIT_MOVEOUT_CARD,

    SEQ_START_UPDATE_MSG,
    SEQ_SEND_GPF_POKEMON,
    SEQ_WAIT_GPF_POKEMON,
    SEQ_SEND_GPF_CUPSTATUS,
    SEQ_WAIT_GPF_CUPSTATUS,
    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE_UPDATE,
    SEQ_WAIT_SAVE_UPDATE,
    SEQ_START_SAVE_NG_MSG,

    //大会未発見
    SEQ_START_RECV_SAKE_MSG,
    SEQ_RECV_SAKE_DATA,
    SEQ_WAIT_SAKE_DATA,
    SEQ_CHECK_RATING,
    SEQ_START_RENEWAL_MSG,
    SEQ_START_SAVE_RENEWAL,
    SEQ_WAIT_SAVE_RENEWAL,
    
    SEQ_REGISTER_EXIT,  //選手証チェック＝１にして終了
    SEQ_ALREADY_EXIT,   //選手証チェック＝２にして終了
    SEQ_NG_EXIT,        //選手証チェック＝０にして終了

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  const REGULATION_CARDDATA *cp_reg_card  = p_wk->p_reg;


  switch( *p_seq )
  { 

    //-------------------------------------
    ///	WEB書き込みフラグチェック
    //=====================================
  case SEQ_CHECK_WRITE:
    if( p_wk->p_param->p_gpf_data->GPFEntryFlg == DREAM_WORLD_ENTRYFLAG_GPF_WRITE )
    { 
      //WEBが書き込んだということは、必ず新規大会なので、
      //書き込みフラグと状態をリセット
      *p_seq  = SEQ_SEND_GPF_STATUS;
    }
    else
    { 
      *p_seq  = SEQ_CHECK_DATA;
    }
    break;

  case SEQ_SEND_GPF_STATUS:
    { 
      DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA data;
      GFL_STD_MemClear( &data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      data.WifiMatchUpState = DREAM_WORLD_MATCHUP_NONE;
      data.GPFEntryFlg      = p_wk->p_param->p_gpf_data->GPFEntryFlg;
    #ifdef BUGFIX_GFBTS1976_20100714
      data.profileID  = WIFIBATTLEMATCH_NET_GetGsProfileID( p_wk->p_net );
    #endif //BUGFIX_GFBTS1976_20100714
      p_wk->p_param->p_gpf_data->WifiMatchUpState = DREAM_WORLD_MATCHUP_NONE;
      WIFIBATTLEMATCH_NET_StartSendGpfData( p_wk->p_net, &data, HEAPID_WIFIBATTLEMATCH_CORE );
    }
    *p_seq  = SEQ_WAIT_GPF_STATUS;
    break;

  case SEQ_WAIT_GPF_STATUS:
    { 
      WIFIBATTLEMATCH_SEND_GPFDATA_RET ret;
      
      ret = WIFIBATTLEMATCH_NET_WaitSendGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS )
      { 
        *p_seq  = SEQ_START_MSG_CUPDATA;
      }
      else if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      if( ret != WIFIBATTLEMATCH_SEND_GPFDATA_RET_UPDATE )
      {
        ///	選手証チェックでエラーが起こったら必ずログインからやりなおし
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

    //-------------------------------------
    ///	大会と自分の状態をチェック
    //=====================================
  case SEQ_CHECK_DATA:
    //開催ワークチェック
    if( p_wk->p_param->p_gpf_data->WifiMatchUpState == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS ) )
    { 
      //一致している
      DEBUG_WIFICUP_Printf( "大会ワーク一致\n" );
      *p_seq  = SEQ_CHECK_STATUS;
    }
    else
    { 
      //一致していない

      //シャチが４or５ならば、書き込みに行く
      //それ以外ならば上書き
      if( DREAM_WORLD_MATCHUP_RETIRE == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS )
          || DREAM_WORLD_MATCHUP_CHANGE_DS == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS ) )
      { 
        DEBUG_WIFICUP_Printf( "大会ワーク一致してなくやめていた %d\n",  Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS ));
        *p_seq  = SEQ_START_WRITE_GPF;
      }
      else
      { 
        DEBUG_WIFICUP_Printf( "大会ワーク一致してない\n" );
        *p_seq  = SEQ_CHECK_STATUS;
      }
    }
    break;

  case SEQ_CHECK_STATUS:
    if( p_wk->p_param->p_gpf_data->WifiMatchUpState == DREAM_WORLD_MATCHUP_END )
    { 
      //大会が終了しているので
      //相応のメッセージをだして、大会見つからなかった、へ
      *p_seq  = SEQ_START_END_MSG;
    }
    else if( p_wk->p_param->p_gpf_data->WifiMatchUpState == DREAM_WORLD_MATCHUP_RETIRE
        || p_wk->p_param->p_gpf_data->WifiMatchUpState == DREAM_WORLD_MATCHUP_CHANGE_DS )
    { 
      //この大会にギブアップor本体替えしているので、
      //相応のメッセージをだして、大会見つからなかった、へ
      *p_seq  = SEQ_START_GIVEUP_MSG;
    }
    else
    { 
      //大会発見
      *p_seq  = SEQ_START_MSG_CUPDATA;
    }
    break;

  case SEQ_START_END_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_07, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_RECV_SAKE_MSG );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_GIVEUP_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_07, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_RECV_SAKE_MSG );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_WRITE_GPF:
    { 
      DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA data;
      GFL_STD_MemClear( &data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      data.WifiMatchUpState = Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS );
    #ifdef BUGFIX_GFBTS1976_20100714
      data.profileID  = WIFIBATTLEMATCH_NET_GetGsProfileID( p_wk->p_net );
    #endif //BUGFIX_GFBTS1976_20100714
      p_wk->p_param->p_gpf_data->WifiMatchUpState = Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS );
      WIFIBATTLEMATCH_NET_StartSendGpfData( p_wk->p_net, &data, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_seq  = SEQ_WAIT_WRITE_GPF;
    }
    break;

  case SEQ_WAIT_WRITE_GPF:
    { 
      WIFIBATTLEMATCH_SEND_GPFDATA_RET ret;
      
      ret = WIFIBATTLEMATCH_NET_WaitSendGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS )
      { 
        *p_seq  = SEQ_START_WRITE_SAKE_DELETE_POKE;
      }
      else if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      if( ret != WIFIBATTLEMATCH_SEND_GPFDATA_RET_UPDATE )
      {
        ///	選手証チェックでエラーが起こったら必ずログインからやりなおし
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

  case SEQ_START_WRITE_SAKE_DELETE_POKE:
#ifdef BUGFIX_GFBTS1987_20100716
    *p_seq  = SEQ_START_GIVEUP_MSG;
#else //BUGFIX_GFBTS1987_20100716
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      GFL_STD_MemClear( p_wk->p_param->p_wifi_sake_data->pokeparty, WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE );
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_POKEPARTY, p_wk->p_param->p_wifi_sake_data->pokeparty );
      *p_seq  = SEQ_WAIT_WRITE_SAKE_DELETE_POKE;
    }
#endif //BUGFIX_GFBTS1987_20100716
    break;

  case SEQ_WAIT_WRITE_SAKE_DELETE_POKE:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_START_GIVEUP_MSG;
      }

      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        ///	選手証チェックでエラーが起こったら必ずログインからやりなおし
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

    //-------------------------------------
    ///	大会が見つかったので、ダウンロード開始
    //=====================================
  case SEQ_START_MSG_CUPDATA:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_36, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_DOWNLOAD_REG );
    *p_seq  = SEQ_WAIT_MSG;
    break;
  case SEQ_START_DOWNLOAD_REG:
    WIFIBATTLEMATCH_NET_StartDownloadDigCard( p_wk->p_net, p_wk->p_param->p_gpf_data->WifiMatchUpID );
    *p_seq  = SEQ_WAIT_DOWNLOAD_REG;
    break;

  case SEQ_WAIT_DOWNLOAD_REG:
    { 
      WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET  ret;
      ret = WIFIBATTLEMATCH_NET_WaitDownloadDigCard( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_SUCCESS )
      { 
        //取得成功
        WIFIBATTLEMATCH_NET_GetDownloadDigCard( p_wk->p_net, &p_wk->recv_card );

        if( Regulation_CheckCrc(&p_wk->recv_card ) )
        {
          //CRC一致
          *p_seq  =  SEQ_WAIT_DOWNLOAD_REG_SUCCESS;
        }
        else
        {
          //CRC不一致
          DEBUG_WIFICUP_Printf( "CRCが不一致だった！\n" );
          *p_seq  =  SEQ_WAIT_DOWNLOAD_REG_FAILURE;
        }
      }
      else if( ret == WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_EMPTY )
      { 
        //取得失敗
        DEBUG_WIFICUP_Printf( "レギュレーションがなかったよ！\n" );
        *p_seq  =  SEQ_WAIT_DOWNLOAD_REG_FAILURE;
      }

      if( ret != WIFIBATTLEMATCH_NET_DOWNLOAD_DIGCARD_RET_DOWNLOADING )
      {
        ///	選手証チェックでエラーが起こったら必ずログインからやりなおし
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

  case SEQ_WAIT_DOWNLOAD_REG_FAILURE:
    //まったく大会がなかったら期間チェックを行わない
    if(0 == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_CUPNO) )
    { 
      *p_seq = SEQ_START_NOTCUP_MSG;
    }
    else
    { 
      *p_seq  = SEQ_START_SUBSEQ_CUPDATE;
    }
    break;

  case SEQ_START_SUBSEQ_CUPDATE:
    {
      ///	選手証チェックでエラーが起こったら必ずログインからやりなおし
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        return;

      case WIFIBATTLEMATCH_NET_ERROR_NONE:
        Util_SubSeq_Start( p_wk, WbmWifiSubSeq_CheckDate );
        *p_seq  = SEQ_WAIT_SUBSEQ_CUPDATE;
        break;
      }
    }
    break;

  case SEQ_WAIT_SUBSEQ_CUPDATE:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret != WBM_WIFI_SUBSEQ_RET_NONE )
      { 
        *p_seq  = SEQ_START_RECV_SAKE_MSG;

        {
          ///	選手証チェックでエラーが起こったら必ずログインからやりなおし
          switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
          { 
          case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
          case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
            WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
            return;
          }
        }
      }
      if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }
    }
    break;

  case SEQ_START_NOTCUP_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_09, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_NG_EXIT );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_WAIT_DOWNLOAD_REG_SUCCESS:
    if( DREAM_WORLD_MATCHUP_NONE == p_wk->p_param->p_gpf_data->WifiMatchUpState )
    { 
      *p_seq  = SEQ_CHECK_EXITS_REG;
    }
    else if( DREAM_WORLD_MATCHUP_SIGNUP == p_wk->p_param->p_gpf_data->WifiMatchUpState )
    { 
      *p_seq  = SEQ_REGISTER_EXIT;
    }
    else if( DREAM_WORLD_MATCHUP_ENTRY == p_wk->p_param->p_gpf_data->WifiMatchUpState )
    { 
      *p_seq  = SEQ_ALREADY_EXIT;
    }
    else
    { 
      *p_seq  = SEQ_NG_EXIT;
    }
    break;

    //-------------------------------------
    /// バトルボックスロック確認
    //=====================================
  case SEQ_CHECK_EXITS_REG:
    if( 0 == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_CUPNO ) )
    { 
      *p_seq  = SEQ_START_UPDATE_MSG;
    }
    else
    { 
      //上書き確認のために以前の選手証を出す
      *p_seq  = SEQ_START_MOVEIN_CARD;
    }
    break;
    
  case SEQ_START_MOVEIN_CARD:
    Util_PlayerInfo_Create( p_wk );
    {
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      if( BATTLE_BOX_SAVE_GetLockType( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI ) )
      {
        //ロックされていれば鍵表示
        Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_LOCK );
      }
      else
      {
        //ロックされていなければ鍵非表示
        Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK );
      }
    }
    *p_seq  = SEQ_WAIT_MOVEIN_CARD;
    break;

  case SEQ_WAIT_MOVEIN_CARD:
    if( Util_PlayerInfo_MoveIn( p_wk ) )
    { 
      *p_seq  = SEQ_CHECK_LOCK;
    }
    break;

  case SEQ_CHECK_LOCK:
    {
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      if( BATTLE_BOX_SAVE_GetLockType( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI ) )
      { 
        *p_seq  = SEQ_START_BOX_REWRITE_MSG;
      }
      else
      { 
        *p_seq  = SEQ_START_BOX_LOCK_MSG;
      }
    }
    break;

  //上書き確認
  case SEQ_START_BOX_REWRITE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_11, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_BOX_REWRITE_YESNO );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_BOX_REWRITE_YESNO:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_BOX_REWRITE_YESNO;
    break;

  case SEQ_SELECT_BOX_REWRITE_YESNO:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //はい
          *p_seq  = SEQ_START_MOVEOUT_CARD;
          break;
        case 1: //いいえ
          *p_seq  = SEQ_START_BOX_REWRITE_CANCEL_MSG;
          break;
        }
      }
    }
    break;

  case SEQ_START_BOX_REWRITE_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_12, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_BOX_REWRITE_CANCEL_YESNO );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_BOX_REWRITE_CANCEL_YESNO:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_BOX_REWRITE_CANCEL_YESNO;
    break;

  case SEQ_SELECT_BOX_REWRITE_CANCEL_YESNO:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //はい
          *p_seq  = SEQ_NG_EXIT;
          break;
        case 1: //いいえ
          *p_seq  = SEQ_START_BOX_REWRITE_MSG;
          break;
        }
      }
    }
    break;

  //ロック確認
  case SEQ_START_BOX_LOCK_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_10, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_BOX_LOCK_YESNO );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_BOX_LOCK_YESNO:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_BOX_LOCK_YESNO;
    break;

  case SEQ_SELECT_BOX_LOCK_YESNO:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //はい
          *p_seq  = SEQ_START_MOVEOUT_CARD;
          break;
        case 1: //いいえ
          *p_seq  = SEQ_START_BOX_LOCK_CANCEL_MSG;
          break;
        }
      }
    }
    break;

  case SEQ_START_BOX_LOCK_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_12, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_BOX_LOCK_CANCEL_YESNO );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_BOX_LOCK_CANCEL_YESNO:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_BOX_LOCK_CANCEL_YESNO;
    break;

  case SEQ_SELECT_BOX_LOCK_CANCEL_YESNO:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //はい
          *p_seq  = SEQ_NG_EXIT;
          break;
        case 1: //いいえ
          *p_seq  = SEQ_START_BOX_LOCK_MSG;
          break;
        }
      }
    }
    break;

  case SEQ_START_MOVEOUT_CARD:
    if( Util_PlayerInfo_MoveOut(p_wk) )
    { 
      *p_seq  = SEQ_WAIT_MOVEOUT_CARD;
    }
    break;

  case SEQ_WAIT_MOVEOUT_CARD:
    Util_PlayerInfo_Delete( p_wk );
    *p_seq  = SEQ_START_UPDATE_MSG;
    break;

  //更新処理
  case SEQ_START_UPDATE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_18, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_SEND_GPF_POKEMON );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_SEND_GPF_POKEMON:
    //GPFサーバへポケモンを転送し、バトルポケモンを消す
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      GFL_STD_MemClear( p_wk->p_param->p_wifi_sake_data->pokeparty, WIFIBATTLEMATCH_GDB_WIFI_POKEPARTY_SIZE );
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_POKEPARTY, p_wk->p_param->p_wifi_sake_data->pokeparty );
      *p_seq  = SEQ_WAIT_GPF_POKEMON;
    }
    break;
    
  case SEQ_WAIT_GPF_POKEMON:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_SEND_GPF_CUPSTATUS;
      }

      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        ///	選手証チェックでエラーが起こったら必ずログインからやりなおし
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

  case SEQ_SEND_GPF_CUPSTATUS:
    //GPFサーバへ大会開催状況を転送する
    { 
      DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA data;
      GFL_STD_MemClear( &data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      data.GPFEntryFlg      = DREAM_WORLD_ENTRYFLAG_DS_WRITE;
      data.WifiMatchUpState = DREAM_WORLD_MATCHUP_SIGNUP;
    #ifdef BUGFIX_GFBTS1976_20100714
      data.profileID  = WIFIBATTLEMATCH_NET_GetGsProfileID( p_wk->p_net );
    #endif //BUGFIX_GFBTS1976_20100714
      p_wk->p_param->p_gpf_data->WifiMatchUpState = DREAM_WORLD_MATCHUP_SIGNUP;
      WIFIBATTLEMATCH_NET_StartSendGpfData( p_wk->p_net, &data, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_seq  = SEQ_WAIT_GPF_CUPSTATUS;
    }
    break;

  case SEQ_WAIT_GPF_CUPSTATUS:
    { 
      WIFIBATTLEMATCH_SEND_GPFDATA_RET ret;
      
      ret = WIFIBATTLEMATCH_NET_WaitSendGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS )
      { 
        *p_seq  = SEQ_START_SAVE_MSG;
      }
      else if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      if( ret != WIFIBATTLEMATCH_SEND_GPFDATA_RET_UPDATE )
      {
        ///	選手証チェックでエラーが起こったら必ずログインからやりなおし
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_08, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE_UPDATE );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_SAVE_UPDATE:
    { 
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
      REGULATION_VIEWDATA *p_view_sv  = RegulationSaveData_GetRegulationView( p_reg_sv, REGULATION_CARD_TYPE_WIFI );

      //デジタル選手証上書き
      GFL_STD_MemCopy( &p_wk->recv_card, p_wk->p_reg, sizeof(REGULATION_CARDDATA) );

      //ロック解除
      BATTLE_BOX_SAVE_OffLockFlg( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI );

      //サインアップ状態へ
      Regulation_SetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_SIGNUP );

      //成績をクリア
      RNDMATCH_SetParam( p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_RATE, 0 );
      RNDMATCH_SetParam( p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_WIN, 0 );
      RNDMATCH_SetParam( p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_LOSE, 0 );

      //新規大会なので登録ポケもクリアする
      RegulationView_Init( p_view_sv );

      GAMEDATA_SaveAsyncStart( p_param->p_param->p_game_data );
      *p_seq  = SEQ_WAIT_SAVE_UPDATE;
    }
    break;

  case SEQ_WAIT_SAVE_UPDATE:
    {
      SAVE_RESULT ret;
      ret = GAMEDATA_SaveAsyncMain( p_param->p_param->p_game_data );
      if( ret == SAVE_RESULT_OK )
      {
        *p_seq  = SEQ_REGISTER_EXIT;
      }
      else if( ret == SAVE_RESULT_NG )
      { 
        *p_seq  = SEQ_START_SAVE_NG_MSG;
      }
    }
    break;

    
  case SEQ_START_SAVE_NG_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_14, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_NG_EXIT );
    *p_seq  = SEQ_WAIT_MSG;
    break;

    //-------------------------------------
    /// 大会が見つからない
    //=====================================
   case SEQ_START_RECV_SAKE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_35, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_RECV_SAKE_DATA );
    *p_seq  = SEQ_WAIT_MSG;
    break; 
  
  case SEQ_RECV_SAKE_DATA:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, WIFIBATTLEMATCH_GDB_MYRECORD, WIFIBATTLEMATCH_GDB_GET_WIFI_SCORE, p_wk->p_param->p_wifi_sake_data );
      *p_seq  = SEQ_WAIT_SAKE_DATA;
    }
    break;
 
  case SEQ_WAIT_SAKE_DATA:
    {
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_GDB_Process( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_CHECK_RATING;
      }

      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        ///	選手証チェックでエラーが起こったら必ずログインからやりなおし
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          return;
        }
      }
    }
    break;

  case SEQ_CHECK_RATING:
    { 
      u32 my_rate ;

      my_rate = RNDMATCH_GetParam( p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_RATE );
      if( p_wk->p_param->p_wifi_sake_data->rate != my_rate )
      { 
        *p_seq  = SEQ_START_RENEWAL_MSG;
      }
      else
      { 
        *p_seq  = SEQ_NG_EXIT;
      }
    }
    break;

  case SEQ_START_RENEWAL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_08, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE_RENEWAL );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_SAVE_RENEWAL:
    { 
      //Regulation_SetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_END );

      //最後の成績を反映
      Util_SaveScore( p_wk, p_wk->p_param->p_wifi_sake_data );

      GAMEDATA_SaveAsyncStart( p_param->p_param->p_game_data );
      *p_seq  = SEQ_WAIT_SAVE_RENEWAL;
    }
    break;

  case SEQ_WAIT_SAVE_RENEWAL:
    {
      SAVE_RESULT ret = GAMEDATA_SaveAsyncMain( p_param->p_param->p_game_data );
      if( ret == SAVE_RESULT_OK )
      {
        *p_seq  = SEQ_NG_EXIT;
      }
    }
    break;

  case SEQ_REGISTER_EXIT:  //選手証チェック＝１にして終了(と仕様書に書いてあるところ)
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Register );
    break;

  case SEQ_ALREADY_EXIT:   //選手証チェック＝２にして終了(と仕様書に書いてあるところ)
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
    break;

  case SEQ_NG_EXIT:        //選手証チェック＝０にして終了(と仕様書に書いてあるところ)
    p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextEnd );
    break;


  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  登録処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Register( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_DRAW_PLAYERINFO,
    SEQ_WAIT_DRAW_PLAYERINFO,
    SEQ_START_REGISTER_POKE_MSG,
    SEQ_START_REGISTER_POKE_LIST,
    SEQ_SELECT_REGISTER_POKE_LIST,
    SEQ_START_REGISTER_POKE_CANCEL_MSG,
    SEQ_START_REGISTER_POKE_CANCEL_LIST,
    SEQ_SELECT_REGISTER_POKE_CANCEL_LIST,
    SEQ_START_REGISTER_CUP_CANCEL_MSG,
    SEQ_START_REGISTER_CUP_CANCEL_LIST,
    SEQ_SELECT_REGISTER_CUP_CANCEL_LIST,
    SEQ_WAIT_MOVEOUT_BTLBOX_END,
    
    SEQ_CHECK_REGULATION,
    SEQ_START_NG_REG_MSG,
    SEQ_START_EMPTY_BOX_MSG,
    SEQ_WAIT_MOVEOUT_BTLBOX,
    SEQ_START_SEND_DATA_MSG,

    SEQ_START_SEND_CHECK_DIRTY_POKE,
    SEQ_WAIT_SEND_CHECK_DIRTY_POKE,
    SEQ_START_DIRTY_POKE_MSG,
    SEQ_START_SEND_SAKE_POKE,
    SEQ_WAIT_SEND_SAKE_POKE,
    SEQ_LOCK_POKE,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_SEND_GPF_CUPSTATUS,
    SEQ_WAIT_GPF_CUPSTATUS,
    SEQ_START_OK_REGISTER_MSG,

    SEQ_NEXT,
    SEQ_DISCONNECT_END,
    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_PLAYERINFO:
    Util_PlayerInfo_Create( p_wk );
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNREGISTER );
    Util_BtlBox_Create( p_wk );
    *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    {
      BOOL ret = TRUE;
      ret &= Util_PlayerInfo_MoveIn( p_wk );
      ret &= Util_BtlBox_MoveIn( p_wk );
      if( ret )
      { 
        *p_seq  = SEQ_START_REGISTER_POKE_MSG;
      }
    }
    break;

  case SEQ_START_REGISTER_POKE_MSG: //バトルボックスのポケモンをとうろくしますか
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_15, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_REGISTER_POKE_LIST );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_REGISTER_POKE_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_DECIDE ); 
    *p_seq  = SEQ_SELECT_REGISTER_POKE_LIST;
    break;

  case SEQ_SELECT_REGISTER_POKE_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //けってい
          *p_seq  = SEQ_CHECK_REGULATION;
          break;

        case 1: //やめる
          *p_seq  = SEQ_START_REGISTER_POKE_CANCEL_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	キャンセル
    //=====================================
  case SEQ_START_REGISTER_POKE_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_16, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_REGISTER_POKE_CANCEL_LIST );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_REGISTER_POKE_CANCEL_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO ); 
    *p_seq  = SEQ_SELECT_REGISTER_POKE_CANCEL_LIST; 
    break;

  case SEQ_SELECT_REGISTER_POKE_CANCEL_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //はい
          *p_seq  = SEQ_START_REGISTER_CUP_CANCEL_MSG;
          break;

        case 1: //いいえ
          *p_seq  = SEQ_START_REGISTER_POKE_MSG;
          break;
        }
      }
    }
    break;

  case SEQ_START_REGISTER_CUP_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_17, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_REGISTER_CUP_CANCEL_LIST );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_REGISTER_CUP_CANCEL_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO ); 
    *p_seq  = SEQ_SELECT_REGISTER_CUP_CANCEL_LIST; 
    break;

  case SEQ_SELECT_REGISTER_CUP_CANCEL_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //はい
          *p_seq  = SEQ_WAIT_MOVEOUT_BTLBOX_END;
          break;

        case 1: //いいえ
          *p_seq  = SEQ_START_REGISTER_POKE_MSG;
          break;
        }
      }
    }
    break;

  case SEQ_WAIT_MOVEOUT_BTLBOX_END:
    if( Util_BtlBox_MoveOut( p_wk ) )
    { 
      Util_BtlBox_Delete( p_wk );
      *p_seq  = SEQ_DISCONNECT_END;
    }
    break;
    
    //-------------------------------------
    ///	登録
    //=====================================
  case SEQ_CHECK_REGULATION:
#ifdef REGULATION_CHECK_ON
    *p_seq  = SEQ_WAIT_MOVEOUT_BTLBOX;
#else
    {
      WIFIBATTLEMATCH_ENEMYDATA *p_my_data = p_param->p_player_data;
      Util_InitMyData( p_my_data, p_wk );

      if( PokeParty_GetPokeCountBattleEnable((POKEPARTY*)p_my_data->pokeparty) == 0 )
      { 
        DEBUG_WIFICUP_Printf( "バトルボックスに入ってなかった! \n" );
        *p_seq  = SEQ_START_EMPTY_BOX_MSG;
      }
      else
      {
        u32 failed_bit  = 0;
        REGULATION        *p_reg  = RegulationData_GetRegulation( p_wk->p_reg );

        if( POKE_REG_OK == PokeRegulationMatchLookAtPokeParty(p_reg, (POKEPARTY*)p_my_data->pokeparty, &failed_bit) )
        { 
          DEBUG_WIFICUP_Printf( "バトルボックスのポケモンレギュレーションOK\n" );
          *p_seq  = SEQ_WAIT_MOVEOUT_BTLBOX;
        }
        else
        { 
          DEBUG_WIFICUP_Printf( "バトルボックスのポケモンレギュレーションNG!! 0x%x \n", failed_bit );
          *p_seq  = SEQ_START_NG_REG_MSG;
        }
      }
    }
#endif
    break;

  case SEQ_START_NG_REG_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_21, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DISCONNECT_END );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_EMPTY_BOX_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_39, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DISCONNECT_END );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_WAIT_MOVEOUT_BTLBOX:
    if( Util_BtlBox_MoveOut( p_wk ) )
    { 
      Util_BtlBox_Delete( p_wk );
      *p_seq  = SEQ_START_SEND_DATA_MSG;
    }
    break;

  case SEQ_START_SEND_DATA_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_18, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SEND_CHECK_DIRTY_POKE );
    *p_seq  = SEQ_WAIT_MSG;
    break;

    //-------------------------------------
    ///	不正チェック
    //=====================================
  case SEQ_START_SEND_CHECK_DIRTY_POKE:
    { 
      POKEPARTY *p_my_party = (POKEPARTY*)p_param->p_player_data->pokeparty;

      WIFIBATTLEMATCH_NET_StartEvilCheck( p_wk->p_net, p_my_party, WIFIBATTLEMATCH_NET_EVILCHECK_TYPE_PARTY );
      GFL_STD_MemClear( &p_wk->evilecheck_data, sizeof(WIFIBATTLEMATCH_NET_EVILCHECK_DATA));
      *p_seq = SEQ_WAIT_SEND_CHECK_DIRTY_POKE;
    }
    break;

  case SEQ_WAIT_SEND_CHECK_DIRTY_POKE:
    { 
      WIFIBATTLEMATCH_NET_EVILCHECK_RET ret;
      ret = WIFIBATTLEMATCH_NET_WaitEvilCheck( p_wk->p_net, &p_wk->evilecheck_data );
      if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_SUCCESS )
      { 
        POKEPARTY *p_my_party = (POKEPARTY*)p_param->p_player_data->pokeparty;
        const int poke_max  = PokeParty_GetPokeCount( p_my_party );
        int i;
        int ret = 0;
        for( i = 0; i < poke_max; i++ )
        { 
          if( p_wk->evilecheck_data.poke_result[i] == NHTTP_RAP_EVILCHECK_RESULT_OK )
          { 
            ret++;
          }
        }

        if( p_wk->evilecheck_data.status_code == 0 && ret == poke_max )
        { 
          DEBUG_WIFICUP_Printf( "不正チェック通過\n" );
          *p_seq = SEQ_START_SEND_SAKE_POKE;
        }
        else
        { 
          DEBUG_WIFICUP_Printf( "不正チェックNG\n" );
          *p_seq = SEQ_START_DIRTY_POKE_MSG;
        }
      }
      else if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_SERVERERR )
      {
        DEBUG_WIFICUP_Printf( "サーバーレスポンスエラー\n" );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        *p_seq  = SEQ_START_DRAW_PLAYERINFO;
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_START_DIRTY_POKE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_19, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DISCONNECT_END );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_SEND_SAKE_POKE:
    if( *p_wk->p_param->p_server_time == 0 )
    {
      POKEPARTY *p_my_party = (POKEPARTY*)p_param->p_player_data->pokeparty;
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_POKEPARTY, p_my_party );
      *p_seq  = SEQ_WAIT_SEND_SAKE_POKE;
    }
    break;
    
  case SEQ_WAIT_SEND_SAKE_POKE:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_LOCK_POKE;
      }
      
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
          *p_seq  = SEQ_START_DRAW_PLAYERINFO;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

  case SEQ_LOCK_POKE:
    //バトルボックスのロックと
    //レギュレーションへの見た目登録
    //開催状態を参加中に
    { 
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      REGULATION_SAVEDATA *p_reg_sv = SaveData_GetRegulationSaveData( p_sv );
      REGULATION_VIEWDATA *p_reg_view = RegulationSaveData_GetRegulationView( p_reg_sv, REGULATION_CARD_TYPE_WIFI );
      POKEPARTY *p_party  = BATTLE_BOX_SAVE_MakePokeParty( p_bbox_save, GFL_HEAP_LOWID(HEAPID_WIFIBATTLEMATCH_SYS) );

      BATTLE_BOX_SAVE_OnLockFlg( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI );

      Regulation_SetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_ENTRY );

      RegulationView_SetEazy( p_reg_view, p_party );

      GFL_HEAP_FreeMemory( p_party );

      *p_seq  = SEQ_START_SAVE;
    }
    break;

  case SEQ_START_SAVE:
    { 
      GAMEDATA_SaveAsyncStart( p_param->p_param->p_game_data );
      *p_seq  = SEQ_WAIT_SAVE;
    }
    break;

  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT ret = GAMEDATA_SaveAsyncMain( p_param->p_param->p_game_data );
      if( ret == SAVE_RESULT_OK )
      { 
        *p_seq  = SEQ_SEND_GPF_CUPSTATUS;
      }
      else if( ret == SAVE_RESULT_NG )
      { 
        *p_seq  = SEQ_DISCONNECT_END;
      }
    }
    break;

  case SEQ_SEND_GPF_CUPSTATUS:
    { 
      DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA data;
      GFL_STD_MemClear( &data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      data.WifiMatchUpState = DREAM_WORLD_MATCHUP_ENTRY;
    #ifdef BUGFIX_GFBTS1976_20100714
      data.profileID  = WIFIBATTLEMATCH_NET_GetGsProfileID( p_wk->p_net );
    #endif //BUGFIX_GFBTS1976_20100714
      p_wk->p_param->p_gpf_data->WifiMatchUpState = DREAM_WORLD_MATCHUP_ENTRY;
      WIFIBATTLEMATCH_NET_StartSendGpfData( p_wk->p_net, &data, HEAPID_WIFIBATTLEMATCH_CORE );
      DEBUG_WIFICUP_Printf("GPFへ送信 %d\n",DREAM_WORLD_MATCHUP_ENTRY);
    }
    *p_seq  = SEQ_WAIT_GPF_CUPSTATUS;
    break;

  case SEQ_WAIT_GPF_CUPSTATUS:
    { 
      WIFIBATTLEMATCH_SEND_GPFDATA_RET ret;
      
      ret = WIFIBATTLEMATCH_NET_WaitSendGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS )
      { 
        *p_seq  = SEQ_START_OK_REGISTER_MSG;
      }
      else if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        *p_seq  = SEQ_START_DRAW_PLAYERINFO;
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_START_OK_REGISTER_MSG:
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_LOCK );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_20, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_NEXT );
    *p_seq  = SEQ_WAIT_MSG; 
    break;

  case SEQ_NEXT:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
    break;

  case SEQ_DISCONNECT_END:
    p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
    break;

  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk ); //予約したシーケンス変数に飛ぶ
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  大会開始処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK_ERROR,
    SEQ_START_DRAW_PLAYERINFO,
    SEQ_WAIT_DRAW_PLAYERINFO,

    SEQ_START_CUP_MSG,
    SEQ_START_CUP_LIST,
    SEQ_SELECT_CUP_LIST,

    SEQ_NEXT_CUP_END,

    SEQ_START_SUBSEQ_UNREGISTER,
    SEQ_WAIT_SUBSEQ_UNREGISTER,
    SEQ_NEXT_DISCONNECT,

    SEQ_START_EVILCHECK_MSG,
    SEQ_START_SUBSEQ_EVILCHECK,
    SEQ_WAIT_SUBSEQ_EVILCHECK,
    SEQ_NEXT_MATCHING,
    
    SEQ_START_DIRTY_MSG,
    SEQ_DIRTY_END,

    SEQ_WAIT_MSG,
  };
  
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECK_ERROR:
    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_NONE:
      *p_seq  = SEQ_START_DRAW_PLAYERINFO;
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      /* fallthrough */
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;
    }
    break;

  case SEQ_START_DRAW_PLAYERINFO:
    if( p_wk->p_playerinfo == NULL )
    { 
      Util_PlayerInfo_Create( p_wk );
      Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_LOCK );
      *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    }
    else
    { 
      *p_seq  = SEQ_START_CUP_MSG;
    }
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    if( Util_PlayerInfo_MoveIn( p_wk ) )
    { 
      *p_seq  = SEQ_START_CUP_MSG;
    }
    break;

  case SEQ_START_CUP_MSG:
    GFL_BG_SetVisible( BG_FRAME_M_TEXT, TRUE );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_22, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_CUP_LIST );
    *p_seq  = SEQ_WAIT_MSG; 
    break;

  case SEQ_START_CUP_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_CUPMENU );
    *p_seq  = SEQ_SELECT_CUP_LIST;
    break;

  case SEQ_SELECT_CUP_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //たいせんする　　不正チェック後マッチングへ
          *p_seq = SEQ_START_EVILCHECK_MSG;
          break;
        case 1: //さんかかいじょ
          *p_seq = SEQ_START_SUBSEQ_UNREGISTER;
          break;
        case 2: //やめる
          *p_seq = SEQ_NEXT_CUP_END;
          break;
        }
      }
    }
    break;

  case SEQ_NEXT_CUP_END:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupEnd );
    break;
    
  case SEQ_START_SUBSEQ_UNREGISTER:
    Util_SubSeq_Start( p_wk, WbmWifiSubSeq_UnRegister );
    *p_seq  = SEQ_WAIT_SUBSEQ_UNREGISTER;
    break;
  case SEQ_WAIT_SUBSEQ_UNREGISTER:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_FALSE||
          ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_NONE )
      { 
        *p_seq  = SEQ_START_DRAW_PLAYERINFO;
      }
      else if( ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_TRUE )
      { 
        *p_seq  = SEQ_NEXT_DISCONNECT;
      }
      else if( ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_SERVER )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;
  case SEQ_NEXT_DISCONNECT:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
    break;

    //-------------------------------------
    ///	不正チェック
    //=====================================
  case SEQ_START_EVILCHECK_MSG:
    Util_InitMyData( p_param->p_player_data, p_wk );

    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SUBSEQ_EVILCHECK );
    *p_seq  = SEQ_WAIT_MSG; 
    break;
  case SEQ_START_SUBSEQ_EVILCHECK:
    Util_SubSeq_Start( p_wk, WbmWifiSubSeq_EvilCheck );
    *p_seq       = SEQ_WAIT_SUBSEQ_EVILCHECK;
    break;

  case SEQ_WAIT_SUBSEQ_EVILCHECK:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_WIFI_SUBSEQ_EVILCHECK_RET_SUCCESS )
      { 
        *p_seq       = SEQ_NEXT_MATCHING;
      }
      else if( ret == WBM_WIFI_SUBSEQ_EVILCHECK_RET_DARTY )
      {  
        //不正でもマッチングへ行く
        //  →署名チェックで弾かれ回らない
        *p_seq       = SEQ_NEXT_MATCHING;
      }
      else if( ret == WBM_WIFI_SUBSEQ_EVILCHECK_RET_SERVER_ERR )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_NEXT_MATCHING:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Matching );
    break;

  case SEQ_START_DIRTY_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_ERR_00, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    break;

  case SEQ_DIRTY_END:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
    break;

  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk ); //予約したシーケンス変数に飛ぶ
    }
    break;
  }
}


//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  マッチング処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  enum
  { 
    //大会期間チェック
    SEQ_START_MSG,
    SEQ_START_SUBSEQ_CUPDATE,
    SEQ_WAIT_SUBSEQ_CUPDATE,
    SEQ_NEXT_START,
    SEQ_RECV_SAKE_DATA,
    SEQ_WAIT_SAKE_DATA,

    //サケサーバーから更新があったときの
    //カード更新処理
    SEQ_START_CARD_MOVEOUT,
    SEQ_WAIT_CARD_MOVEOUT,
    SEQ_START_CARD_MOVEIN,
    SEQ_WAIT_CARD_MOVEIN,

    //マッチング開始
    SEQ_START_MATCH_MSG,
    SEQ_START_MATCH,
    SEQ_WAIT_MATCHING,

    SEQ_START_SENDDATA,
    SEQ_WAIT_SENDDATA,
    SEQ_CHECK_YOU_CUPNO,
    SEQ_CHECK_YOU_REGULATION,

    SEQ_START_DOWNLOAD_OTHER_SAKE_POKE,
    SEQ_WAIT_DOWNLOAD_OTHER_SAKE_POKE,

    SEQ_START_SAKE_TIMING,
    SEQ_WAIT_SAKE_TIMING,

    SEQ_CHECK_DIRTY_POKE,
    SEQ_SEND_DIRTY_CHECK_DATA,
    SEQ_RECV_DIRTY_CHECK_DATA,

    SEQ_CHECK_DIRTY,
    SEQ_START_BADWORD,
    SEQ_WAIT_BADWORD,

    SEQ_START_CANCEL_TIMING,
    SEQ_WAIT_CANCEL_TIMING,
    SEQ_START_OK_TIMING,
    SEQ_WAIT_OK_TIMING,

    SEQ_START_OK_MATCHING_MSG,
    SEQ_START_DRAW_MATCHINFO,
    SEQ_WAIT_MOVEIN_MATCHINFO,
    SEQ_WAIT_CNT,
    SEQ_START_SESSION,
    SEQ_WAIT_SESSION,
    SEQ_END_MATCHING_MSG,
    SEQ_END_MATCHING,
    SEQ_ERROR_END,

    //キャンセル処理
    //今日の対戦をおわりにします
    SEQ_START_SELECT_END_MSG,
    SEQ_START_END_LIST,
    SEQ_SELECT_END_LIST,

    //たいせんをつづけますか？
    SEQ_START_SELECT_CONTINUE_MSG,
    SEQ_START_CONTINUE_LIST,
    SEQ_SELECT_CONTINUE_LIST,

    //キャンセルから復帰時の日時チェック
    SEQ_START_CUPDATE_MSG_RET,
    SEQ_START_SUBSEQ_CUPDATE_RET,
    SEQ_WAIT_SUBSEQ_CUPDATE_RET,

    SEQ_START_CANCEL,
    SEQ_END_CANCEL,

    SEQ_START_DISCONNECT,
    SEQ_WAIT_DISCONNECT,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;
  UTIL_CANCEL_STATE cancel_state;
  BOOL is_timeout_enable  = FALSE;

  //-------------------------------------
  ///	キャンセルシーケンス
  //=====================================
  {
    BOOL is_cancel_enable = (SEQ_WAIT_MATCHING <= *p_seq && *p_seq < SEQ_START_CANCEL_TIMING);
    BOOL can_disconnect = !( (SEQ_START_BADWORD <= *p_seq && *p_seq <= SEQ_WAIT_BADWORD ) || ( SEQ_START_DOWNLOAD_OTHER_SAKE_POKE <= *p_seq && *p_seq <= SEQ_WAIT_DOWNLOAD_OTHER_SAKE_POKE ) );
    cancel_state  = Util_Cancel_Seq( p_wk, is_cancel_enable, can_disconnect );

    //自分が切断
    if( cancel_state == UTIL_CANCEL_STATE_DECIDE )
    {
      *p_seq  = SEQ_START_SELECT_END_MSG;
      return ;
    }
  }

  //-------------------------------------
  ///	マッチングシーケンス
  //=====================================
  switch( *p_seq )
  { 
    //-------------------------------------
    ///	大会期間チェック
    //=====================================
  case SEQ_START_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SUBSEQ_CUPDATE );
    *p_seq  = SEQ_WAIT_MSG; 
    break;

  case SEQ_START_SUBSEQ_CUPDATE:
    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );

    #ifdef BUGFIX_BTS7769_20100713
      Util_Cancel_ResetSeq( p_wk );
    #endif //BUGFIX_BTS7769_20100713
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_NONE:  //正常
      Util_SubSeq_Start( p_wk, WbmWifiSubSeq_CheckDate );
      *p_seq  = SEQ_WAIT_SUBSEQ_CUPDATE;
      break;
    }
    break;
  case SEQ_WAIT_SUBSEQ_CUPDATE:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMESAFE )
      { 
        *p_seq  = SEQ_RECV_SAKE_DATA;
      }
      else if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEOVER
          || ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEBEFORE )
      { 
        *p_seq  = SEQ_NEXT_START;
      }
      else if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );

    #ifdef BUGFIX_BTS7769_20100713
      Util_Cancel_ResetSeq( p_wk );
    #endif //BUGFIX_BTS7769_20100713
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_NEXT_START:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
    break;

  case SEQ_RECV_SAKE_DATA:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, WIFIBATTLEMATCH_GDB_MYRECORD, WIFIBATTLEMATCH_GDB_GET_WIFI_SCORE, p_wk->p_param->p_wifi_sake_data );
      *p_seq  = SEQ_WAIT_SAKE_DATA;
    }
    break;
    
  case SEQ_WAIT_SAKE_DATA:
    { 
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_GDB_Process( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        WIFIBATTLEMATCH_ENEMYDATA *p_my_data = p_param->p_player_data;


        //自分のデータ作成
        *p_wk->p_param->p_server_time  = 0;
        Util_SetMyDataInfo( p_my_data, p_wk );


        //消したくない情報を常駐に保存
        p_wk->p_param->p_recv_data->record_save_idx = p_wk->p_param->p_wifi_sake_data->record_save_idx;

#ifdef MYPOKE_SELFCHECK
        {
          POKEPARTY *p_my_party = (POKEPARTY*)p_param->p_player_data->pokeparty;
          {
            int i;
            u8 *p_src = (u8*)p_my_party;
            OS_TFPrintf( 2, "\n\n" );
            for( i = 0; i < PokeParty_GetWorkSize(); i++ )
            { 
              OS_TFPrintf( 2, "%x ", p_src[i] );
            }
          }
          {
            int i;
            u8 *p_src = (u8*)p_my_data->pokeparty;
            OS_TFPrintf( 2, "\n\n" );
            for( i = 0; i < PokeParty_GetWorkSize(); i++ )
            { 
              OS_TFPrintf( 2, "%x ", p_src[i] );
            }
            OS_TFPrintf( 2, "\n\n" );
          }


          if( Util_ComarepPokeParty( (POKEPARTY*)p_wk->p_param->p_wifi_sake_data->pokeparty, p_my_party ) )
          { 
            OS_TFPrintf( 3, "自分のパーティとサケは一緒\n" );
          }
          else
          { 
            OS_TFPrintf( 3, "自分のパーティとサケが違う！！\n" );
            GF_ASSERT(0);
          }
        }

        //自分で自分の署名は通るのかチェック
        if( Util_VerifyPokeData( p_my_data, (POKEPARTY*)p_wk->p_param->p_player_data->pokeparty, HEAPID_WIFIBATTLEMATCH_CORE ) )
#endif  //MYPOKE_SELFCHECK
        { 
          OS_TFPrintf( 3, "自分の署名は通ったぜ\n" );

          //更新があった
          if( Util_SaveScore( p_wk, p_wk->p_param->p_wifi_sake_data ) )
          {
            *p_seq  = SEQ_START_CARD_MOVEOUT;
          }
          //更新はなかった
          else
          {
            *p_seq  = SEQ_START_MATCH_MSG;
          }
        }
      }
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
        #ifdef BUGFIX_BTS7769_20100713
          Util_Cancel_ResetSeq( p_wk );
        #endif //BUGFIX_BTS7769_20100713
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	カード更新処理
    //=====================================
  case SEQ_START_CARD_MOVEOUT:
    *p_seq  = SEQ_WAIT_CARD_MOVEOUT;
    break;
  case SEQ_WAIT_CARD_MOVEOUT:
    if( Util_PlayerInfo_MoveOut( p_wk ) )
    {
      Util_PlayerInfo_Delete( p_wk );
      *p_seq  = SEQ_START_CARD_MOVEIN;
    }
    break;
  case SEQ_START_CARD_MOVEIN:
    Util_PlayerInfo_Create( p_wk );
    *p_seq  = SEQ_WAIT_CARD_MOVEIN;
    break;
  case SEQ_WAIT_CARD_MOVEIN:
    if( Util_PlayerInfo_MoveIn(p_wk ) )
    {
      *p_seq  = SEQ_START_MATCH_MSG;
    }
    break;

    //-------------------------------------
    ///	マッチング開始
    //=====================================
  case SEQ_START_MATCH_MSG:
    p_wk->cancel_seq  = 0;
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, TRUE );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_30, WBM_TEXT_TYPE_QUE );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_MATCH );
    break;
  case SEQ_START_MATCH:
    {
      const REGULATION_CARDDATA *cp_reg_card  = p_wk->p_reg;
      WIFIBATTLEMATCH_MATCH_KEY_DATA  data;
      GFL_STD_MemClear( &data, sizeof(WIFIBATTLEMATCH_MATCH_KEY_DATA) );
      data.rate = p_wk->p_param->p_wifi_sake_data->rate;
      data.disconnect = p_wk->p_param->p_wifi_sake_data->disconnect;
      data.cup_no = Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_CUPNO );
      data.btlcnt= p_wk->p_param->p_wifi_sake_data->win + p_wk->p_param->p_wifi_sake_data->lose;
      WIFIBATTLEMATCH_NET_StartMatchMake( p_wk->p_net, WIFIBATTLEMATCH_TYPE_WIFICUP, FALSE, p_param->p_param->btl_rule, &data ); 
      p_wk->other_dirty_cnt = 0;
      p_wk->my_dirty_cnt = 0;
      *p_seq  = SEQ_WAIT_MATCHING;
    }
    break;
  case SEQ_WAIT_MATCHING:
    //マッチングエラー
    {
      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );

        #ifdef BUGFIX_BTS7769_20100713
          Util_Cancel_ResetSeq( p_wk );
        #endif //BUGFIX_BTS7769_20100713
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    {
      //マッチング待ち
      WIFIBATTLEMATCH_NET_MATCHMAKE_STATE state = WIFIBATTLEMATCH_NET_WaitMatchMake( p_wk->p_net );
      //マッチング終了待ち
      if( WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_SUCCESS == state ) 
      { 
        *p_seq = SEQ_START_SENDDATA;
      }
      else if( WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_FAILED == state )
      {
        *p_seq = SEQ_START_MATCH;
      }
    }
    break;

  case SEQ_START_SENDDATA:
    if( cancel_state == UTIL_CANCEL_STATE_NONE )
    { 
      if( WIFIBATTLEMATCH_NET_StartEnemyData( p_wk->p_net, p_param->p_player_data ) )
      {  
        *p_seq  = SEQ_WAIT_SENDDATA;
        p_wk->match_timeout = 0;
      }
    }
    else
    {
      //待機中でエラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        p_wk->cancel_seq  = 0;
        Util_List_Delete( p_wk );
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );

        #ifdef BUGFIX_BTS7769_20100713
          Util_Cancel_ResetSeq( p_wk );
        #endif //BUGFIX_BTS7769_20100713
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        Util_List_Delete( p_wk );
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      } 
    }
    break;
  case SEQ_WAIT_SENDDATA:
    {
      WIFIBATTLEMATCH_ENEMYDATA *p_recv;
      if( WIFIBATTLEMATCH_NET_WaitEnemyData( p_wk->p_net, &p_recv ) )
      { 
        u32 dirty ;
        GFL_STD_MemCopy( p_recv, p_param->p_enemy_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
        
        dirty = WIFIBATTLEMATCH_DATA_ModifiEnemyData( p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );  //不正データ対策

        if( dirty == 0 )
        { 
          SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
          WIFI_NEGOTIATION_SAVEDATA* pSV  = WIFI_NEGOTIATION_SV_GetSaveData(p_sv_ctrl);
//          WIFI_NEGOTIATION_SV_SetFriend(pSV, (MYSTATUS*)p_param->p_enemy_data->mystatus );
          //仕様がなくなりました
        }

        *p_seq  = SEQ_CHECK_YOU_CUPNO;
      }

      is_timeout_enable = TRUE;
    }
    break;
  case SEQ_CHECK_YOU_CUPNO:
    WIFIBATTLEMATCH_DATA_DebugPrint( p_param->p_player_data );
    WIFIBATTLEMATCH_DATA_DebugPrint( p_param->p_enemy_data );
#ifdef DEBUG_REG_CRC_CHECK_OFF 
    if( p_param->p_player_data->wificup_no == p_param->p_enemy_data->wificup_no )
#else
    if( p_param->p_player_data->wificup_no == p_param->p_enemy_data->wificup_no
        && p_param->p_player_data->reg_crc == p_param->p_enemy_data->reg_crc )
#endif
    { 
      DEBUG_WIFICUP_Printf( "大会が同じ！！\n" );
      *p_seq  = SEQ_CHECK_YOU_REGULATION;
    }
    else
    {
      //大会番号がことなっていたらマッチングに戻る
      DEBUG_WIFICUP_Printf( "大会番号が違うのでマッチングに戻る no %d=%d crc %d=%d \n",
          p_param->p_player_data->wificup_no,
          p_param->p_enemy_data->wificup_no,
          p_param->p_player_data->reg_crc,
          p_param->p_enemy_data->reg_crc
          );
      *p_seq  = SEQ_START_DISCONNECT;
    }
    break;
  case SEQ_CHECK_YOU_REGULATION:
    {
      u32 failed_bit  = 0;
      REGULATION        *p_reg  = RegulationData_GetRegulation( p_wk->p_reg );
      POKEPARTY         *p_party=  (POKEPARTY*)p_param->p_enemy_data->pokeparty;
      //相手のポケモンをレギュレーションチェックにかける
      if( POKE_REG_OK == PokeRegulationMatchLookAtPokeParty(p_reg, p_party, &failed_bit)  )
      {
        DEBUG_WIFICUP_Printf( "相手のポケモンはレギュレーションOK\n" );
        *p_seq = SEQ_START_DOWNLOAD_OTHER_SAKE_POKE;
      }
      else
      { 
        p_wk->other_dirty_cnt++;
        DEBUG_WIFICUP_Printf( "!!相手のポケモンはレギュレーションNG!! 0x%x\n", failed_bit );
        *p_seq = SEQ_SEND_DIRTY_CHECK_DATA;
      }
    }
    break;

  case SEQ_START_DOWNLOAD_OTHER_SAKE_POKE:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, p_wk->p_param->p_enemy_data->sake_recordID, WIFIBATTLEMATCH_GDB_GET_PARTY, p_wk->p_other_party );
      *p_seq  = SEQ_WAIT_DOWNLOAD_OTHER_SAKE_POKE;
    }
    break;
  case SEQ_WAIT_DOWNLOAD_OTHER_SAKE_POKE:
    { 
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_GDB_Process( p_wk->p_net );

      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC2;
        *p_seq  = SEQ_START_SAKE_TIMING;
      }
      
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:       //戻る
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );

        #ifdef BUGFIX_BTS7769_20100713
          Util_Cancel_ResetSeq( p_wk );
        #endif //BUGFIX_BTS7769_20100713
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

  case SEQ_START_SAKE_TIMING:
    //サケのサーバー待ちが存在するため、このあとのBキャンセルのタイムアウトに影響がでてしまう
    //そのため、ここで一旦同期をとり、このあとのタイムアウトを行えるようにする
    WIFIBATTLEMATCH_NET_StartTiming( p_wk->p_net, WIFIBATTLEMATCH_NET_TIMINGSYNC_MATHING_SAKE );
    *p_seq  = SEQ_WAIT_SAKE_TIMING;
    break;

  case SEQ_WAIT_SAKE_TIMING:
    if( WIFIBATTLEMATCH_NET_WaitTiming( p_wk->p_net ) )
    {
      *p_seq  = SEQ_CHECK_DIRTY_POKE;
    }

    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
    {
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:       //戻る
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );

    #ifdef BUGFIX_BTS7769_20100713
      Util_Cancel_ResetSeq( p_wk );
    #endif //BUGFIX_BTS7769_20100713
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;
    }
    break;

  case SEQ_CHECK_DIRTY_POKE:
    //相手のサーバーポケモンと送信されてきたポケモンは同一か。
    if( !Util_ComarepPokeParty( p_wk->p_other_party, (POKEPARTY*)p_wk->p_param->p_enemy_data->pokeparty ) )
    { 
      DEBUG_WIFICUP_Printf( "!!相手のポケモンとサーバーポケモン違うNG!! \n" );
      p_wk->other_dirty_cnt++;
    }
    else
    { 
      DEBUG_WIFICUP_Printf( "!!相手のポケモンとサーバーポケモンが一致!! \n" );
    }

    //相手のサーバーポケモンと署名から、正常データであることを証明
    if( !Util_VerifyPokeData( p_wk->p_param->p_enemy_data, (POKEPARTY*)p_wk->p_param->p_enemy_data->pokeparty, HEAPID_WIFIBATTLEMATCH_CORE ) )
    { 
      DEBUG_WIFICUP_Printf( "!!相手のポケモン署名NG!! \n" );
      p_wk->other_dirty_cnt++;
    }
    else
    { 
      DEBUG_WIFICUP_Printf( "!!相手のポケモン署名完全一致!! \n" );
    }

    *p_seq  = SEQ_SEND_DIRTY_CHECK_DATA;
    break;
  case SEQ_SEND_DIRTY_CHECK_DATA:
    if( cancel_state == UTIL_CANCEL_STATE_NONE )
    {
      if( WIFIBATTLEMATCH_NET_SendDirtyCnt( p_wk->p_net, &p_wk->other_dirty_cnt ) )
      { 
        *p_seq  = SEQ_RECV_DIRTY_CHECK_DATA;
        p_wk->match_timeout = 0;
      }
    }
    else
    {
      //待機中でエラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        p_wk->cancel_seq  = 0;
        Util_List_Delete( p_wk );
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );

      #ifdef BUGFIX_BTS7769_20100713
         Util_Cancel_ResetSeq( p_wk );
      #endif //BUGFIX_BTS7769_20100713
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        Util_List_Delete( p_wk );
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      } 
    }
    break;
  case SEQ_RECV_DIRTY_CHECK_DATA:
    if( WIFIBATTLEMATCH_NET_RecvDirtyCnt( p_wk->p_net, &p_wk->my_dirty_cnt ) )
    { 
      DEBUG_WIFICUP_Printf( "不正カウンタ　自分%d相手%d\n", p_wk->my_dirty_cnt, p_wk->other_dirty_cnt );
      *p_seq  = SEQ_CHECK_DIRTY;
    }
    else
    {
      is_timeout_enable = TRUE;
    }
    break;

  case SEQ_CHECK_DIRTY:
    if( p_wk->my_dirty_cnt == 0 && p_wk->other_dirty_cnt == 0 )  //不正カウンタチェック
    { 
      *p_seq  = SEQ_START_BADWORD;
    }
    else
    { 
      //不正だったらマッチングに戻る
      *p_seq  = SEQ_START_DISCONNECT;
    }
    break;

  case SEQ_START_BADWORD:
    WIFIBATTLEMATCH_NET_StartBadWord( p_wk->p_net, (MYSTATUS*)p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );
    *p_seq  = SEQ_WAIT_BADWORD;
    break;
  case SEQ_WAIT_BADWORD:
    { 
      BOOL ret;
      BOOL is_badword;
      
      ret = WIFIBATTLEMATCH_NET_WaitBadWord( p_wk->p_net, &is_badword );
      if( ret )
      { 
        *p_wk->p_param->p_is_dirty_name  = is_badword;
        *p_seq  = SEQ_START_CANCEL_TIMING;
      }

      { 
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );

        #ifdef BUGFIX_BTS7769_20100713
          Util_Cancel_ResetSeq( p_wk );
        #endif //BUGFIX_BTS7769_20100713
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

  case SEQ_START_CANCEL_TIMING:
    //キャンセル状態のときはタイミングとらない
    if( cancel_state == UTIL_CANCEL_STATE_NONE )
    {
      WIFIBATTLEMATCH_NET_StartTiming( p_wk->p_net, WIFIBATTLEMATCH_NET_TIMINGSYNC_MATHING_CANCEL );
      p_wk->match_timeout = 0;
      *p_seq  = SEQ_WAIT_CANCEL_TIMING;
    }
    break;

  case SEQ_WAIT_CANCEL_TIMING:
    if( WIFIBATTLEMATCH_NET_RecvMatchCancel( p_wk->p_net ) == FALSE )
    {
      if( WIFIBATTLEMATCH_NET_WaitTiming( p_wk->p_net ) )
      {
        *p_seq  = SEQ_START_OK_TIMING;
      }
    }
    is_timeout_enable = TRUE;
    break;

  case SEQ_START_OK_TIMING:
    WIFIBATTLEMATCH_NET_StartTiming( p_wk->p_net, WIFIBATTLEMATCH_NET_TIMINGSYNC_MATHING_OK );
    *p_seq  = SEQ_WAIT_OK_TIMING;
#ifdef BUGFIX_GFBTS1989_20100716
    p_wk->match_timeout = 0;
#endif //BUGFIX_GFBTS1989_20100716
    break;

  case SEQ_WAIT_OK_TIMING:
    if( WIFIBATTLEMATCH_NET_WaitTiming( p_wk->p_net ) )
    {
      *p_seq  = SEQ_START_OK_MATCHING_MSG;
    }
#ifdef BUGFIX_GFBTS1989_20100716
    is_timeout_enable = TRUE;
#endif //BUGFIX_GFBTS1989_20100716
    break;

  case SEQ_START_OK_MATCHING_MSG:

    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );

    PMSND_PlaySE( WBM_SND_SE_MATCHING_OK );

    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_32, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_DRAW_MATCHINFO ); 
    p_wk->cnt = 0;
    break;
  case SEQ_START_DRAW_MATCHINFO:
    if( p_wk->cnt++ > MATCHING_MSG_WAIT_SYNC )
    { 
      p_wk->cnt      = 0;
      GFL_BG_SetVisible( BG_FRAME_M_TEXT, FALSE );
      //対戦者情報表示
      Util_MatchInfo_Create( p_wk, p_param->p_enemy_data );
      *p_seq  = SEQ_WAIT_MOVEIN_MATCHINFO;
    }
    break;
  case SEQ_WAIT_MOVEIN_MATCHINFO:
    if( Util_MatchInfo_Main( p_wk ) )
    {
      *p_seq  = SEQ_WAIT_CNT;
    }
    break;
  case SEQ_WAIT_CNT:
    if( p_wk->cnt++ > ENEMYDATA_WAIT_SYNC )
    { 
      p_wk->cnt = 0;
      *p_seq  = SEQ_START_SESSION;
    }
    break;

  case SEQ_START_SESSION:
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_SC, WBM_SC_HEAP_SIZE );
    DWC_RAPCOMMON_SetSubHeapIDEx( DWC_ALLOCTYPE_GS, WBM_SC_HEAP_SIZE, WBM_SC_BORDER_SIZE, HEAPID_WIFIBATTLEMATCH_SC );
    DWC_RAPCOMMON_SetAutoDeleteSubHeap( HEAPID_WIFIBATTLEMATCH_SC );

    WIFIBATTLEMATCH_SC_StartReport( p_wk->p_net, WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_AFTER, WIFIBATTLEMATCH_TYPE_WIFICUP, 0, NULL, FALSE );
    p_wk->is_send_report  = FALSE;
    *p_seq  = SEQ_WAIT_SESSION;
    p_wk->sc_state  = WIFIBATTLEMATCH_NET_SC_STATE_UPDATE;
    break;

  case SEQ_WAIT_SESSION:
    {
      //内部でエラー検知してもDWCのエラーがでないと、
      //もう一度内部で処理してしまう場合があるので、
      //エラーが起こったら起動しないようにしています
      if( p_wk->sc_state == WIFIBATTLEMATCH_NET_SC_STATE_UPDATE )
      {
        p_wk->sc_state = WIFIBATTLEMATCH_SC_ProcessReport(p_wk->p_net, &p_wk->is_send_report );
      }
      if( p_wk->sc_state == WIFIBATTLEMATCH_NET_SC_STATE_SUCCESS )
      { 
        *p_seq  = SEQ_END_MATCHING_MSG;
      }
      
      if( p_wk->sc_state != WIFIBATTLEMATCH_NET_SC_STATE_UPDATE )
      {
        //ここでエラーが起こった場合、レポートを送信していれば切断カウンターがあがってしまうので戦闘後へ、レポートを送信していなければ、録画後へいく
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:     //戻る
          WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
          /* fallthrough */
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:

#ifndef SCREPORT_TIMEOUT_IS_DISCONNECT
          DWC_RAPCOMMON_ResetSubHeapID();

          Util_Matchinfo_Clear( p_wk );

          *p_seq  = SEQ_ERROR_END;
          break;
#endif
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          DWC_RAPCOMMON_ResetSubHeapID();

          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

  case SEQ_END_MATCHING_MSG:

    DWC_RAPCOMMON_ResetSubHeapID();

    GFL_BG_SetVisible( BG_FRAME_M_TEXT, TRUE );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_012, WBM_TEXT_TYPE_WAIT );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_END_MATCHING );
    break;


  case SEQ_END_MATCHING:
    if( p_wk->cnt++ > SELECTPOKE_MSG_WAIT_SYNC )
    { 
      p_wk->cnt = 0;
      p_param->result = WIFIBATTLEMATCH_CORE_RESULT_NEXT_BATTLE;
      WBM_SEQ_End( p_seqwk );
    }
    break;

  case SEQ_ERROR_END:
    WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
    p_param->mode = WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;
#ifdef BUGFIX_BTS7762_20100713
    p_wk->is_sc_before_err  = TRUE;
#endif //BUGFIX_BTS7762_20100713

    if( p_wk->is_send_report )
    {
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_EndBattle );
    }
    else
    {
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
    }
    break;

    //-------------------------------------
    ///	今日の対戦をおわりにします
    //=====================================
  case SEQ_START_SELECT_END_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_23, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_END_LIST ); 
    break;
  case SEQ_START_END_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_END_LIST;
    break;
  case SEQ_SELECT_END_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //はい
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextCheckDate );
          break;
        case 1: //いいえ
          *p_seq  = SEQ_START_SELECT_CONTINUE_MSG;
          break;
        }
      }
    }
    break;

    //たいせんをつづけますか？
  case SEQ_START_SELECT_CONTINUE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_015, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_CONTINUE_LIST );
    break;
  case SEQ_START_CONTINUE_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_CONTINUE_LIST;
    break;
  case SEQ_SELECT_CONTINUE_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //はい
          *p_seq  = SEQ_START_CUPDATE_MSG_RET;
          break;
        case 1: //いいえ
          *p_seq  = SEQ_START_SELECT_END_MSG;
          break;
        }
      }
    }
    break;

    //キャンセルから復帰時の日時チェック
  case SEQ_START_CUPDATE_MSG_RET:
//    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
//    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SUBSEQ_CUPDATE_RET );
//    *p_seq  = SEQ_WAIT_MSG; 
    *p_seq = SEQ_START_SUBSEQ_CUPDATE_RET;
    break;
  case SEQ_START_SUBSEQ_CUPDATE_RET:
    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;
    case WIFIBATTLEMATCH_NET_ERROR_NONE:
      Util_SubSeq_Start( p_wk, WbmWifiSubSeq_CheckDate );
      *p_seq  = SEQ_WAIT_SUBSEQ_CUPDATE_RET;
      break;
    }
    break;
  case SEQ_WAIT_SUBSEQ_CUPDATE_RET:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMESAFE )
      { 
        *p_seq  = SEQ_START_MATCH_MSG;
      }
      else if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEOVER
          || ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEBEFORE )
      { 
        *p_seq  = SEQ_NEXT_START;
      }
      else if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Start );

      #ifdef BUGFIX_BTS7769_20100713
        Util_Cancel_ResetSeq( p_wk );
      #endif //BUGFIX_BTS7769_20100713
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_START_CANCEL:
    if( WIFIBATTLEMATCH_NET_SendMatchCancel( p_wk->p_net ) )
    {
      *p_seq  = SEQ_END_CANCEL;
    }
    break;

  case SEQ_END_CANCEL:
    WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
    *p_seq  = SEQ_START_MATCH_MSG;
    break;

  case SEQ_START_DISCONNECT:
    WIFIBATTLEMATCH_NET_StartDisConnect( p_wk->p_net );
    *p_seq  = SEQ_WAIT_DISCONNECT;
    p_wk->match_timeout = 0;
    break;

  case SEQ_WAIT_DISCONNECT:
    is_timeout_enable = TRUE;
    if( WIFIBATTLEMATCH_NET_WaitDisConnect( p_wk->p_net ) )
    {
      *p_seq = SEQ_START_MATCH;
    }
    break;

    //-------------------------------------
    ///	共通
    //=====================================
  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }

  //-------------------------------------
  ///	タイムアウトチェック
  //=====================================
  {
    if( is_timeout_enable )
    {
      BOOL is_disconnect  = FALSE;
      //相手が切断
      if( p_wk->match_timeout++ > MATCHING_TIMEOUT_SYNC )
      {
        OS_TPrintf( "タイムアウト！ seq=%d\n", *p_seq );
        p_wk->match_timeout = 0;
        is_disconnect  = TRUE;
      }

      //相手が切断
      if( WIFIBATTLEMATCH_NET_RecvMatchCancel( p_wk->p_net ) )
      {
        OS_TPrintf( "相手の切断を検知！ seq=%d\n", *p_seq );
        is_disconnect  = TRUE;
      }

      if( is_disconnect )
      {
        WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );

#ifdef BUGFIX_BTS7867_20100716
        if( NET_ERR_CHECK_NONE == NetErr_App_CheckError()
            && GFL_NET_IsInit() )
#endif //BUGFIX_BTS7867_20100716
        {
          DWC_CloseAllConnectionsHard();
        }

#ifdef BUGFIX_GFBTS1958_20100712
        *p_seq  = SEQ_START_MATCH_MSG;
#endif  //BUGFIX_GFBTS1958_20100712
      }

      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );

      #ifdef BUGFIX_BTS7769_20100713
        Util_Cancel_ResetSeq( p_wk );
      #endif //BUGFIX_BTS7769_20100713
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  戦闘後処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_SC_HEAP_INIT,
    SEQ_START_NET_MSG,
    SEQ_START_REPORT_ATLAS,
    SEQ_WAIT_REPORT_ATLAS,

    SEQ_SC_HEAP_EXIT,

    SEQ_INIT_DISCONNECT,
    SEQ_WAIT_DISCONNECT,
    SEQ_START_SAKE_RECORD,
    SEQ_WAIT_SAKE_RECORD,

    SEQ_START_RESULT_MSG,
    SEQ_END,

    //大会期間チェック
    SEQ_START_SUBSEQ_CUPDATE,
    SEQ_WAIT_SUBSEQ_CUPDATE,

    //共通
    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_SC_HEAP_INIT:
#ifdef BUGFIX_BTS7762_20100713
    //以前はWIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERRだけを見て分岐していたが、
    //以下の処理を満たすためフラグを追加しました
    //１）戦闘中に切断されていたらレポートを送信しない
    //２）戦闘デモ前に切断されていたらレポートを送信する
    if( p_param->mode == WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR
        && p_wk->is_sc_before_err == FALSE )
    {
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_EndRec );
    }
    else
#endif //BUGFIX_BTS7762_20100713
    {
#ifdef BUGFIX_BTS7762_20100713
      p_wk->is_sc_before_err = FALSE;
#endif //BUGFIX_BTS7762_20100713

      GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_SC, WBM_SC_HEAP_SIZE );
      DWC_RAPCOMMON_SetSubHeapIDEx( DWC_ALLOCTYPE_GS, WBM_SC_HEAP_SIZE, WBM_SC_BORDER_SIZE, HEAPID_WIFIBATTLEMATCH_SC );
      DWC_RAPCOMMON_SetAutoDeleteSubHeap( HEAPID_WIFIBATTLEMATCH_SC );

      *p_seq  = SEQ_START_NET_MSG;
    }
    break;
  case SEQ_START_NET_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_18, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_REPORT_ATLAS );
    break;

  case SEQ_START_REPORT_ATLAS:
    {
      BOOL is_error = p_param->mode == WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;
      WIFIBATTLEMATCH_SC_StartReport( p_wk->p_net, WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_SCORE, WIFIBATTLEMATCH_TYPE_WIFICUP, p_param->p_param->btl_rule, p_param->cp_btl_score, is_error );
      p_wk->sc_state  = WIFIBATTLEMATCH_NET_SC_STATE_UPDATE;
    }
    p_wk->is_send_report  = FALSE;
    *p_seq = SEQ_WAIT_REPORT_ATLAS;
    break;
  case SEQ_WAIT_REPORT_ATLAS:
    { 

      //内部でエラー検知してもDWCのエラーがでないと、
      //もう一度内部で処理してしまう場合があるので、
      //エラーが起こったら起動しないようにしています
      if( p_wk->sc_state == WIFIBATTLEMATCH_NET_SC_STATE_UPDATE )
      {
        p_wk->sc_state = WIFIBATTLEMATCH_SC_ProcessReport(p_wk->p_net, &p_wk->is_send_report );

        //レポート送信してればカウント
        if( p_wk->sc_state != WIFIBATTLEMATCH_NET_SC_STATE_UPDATE  )
        {
          if( p_wk->is_send_report )
          {
            WBM_RECORD_Count( p_param->p_param->p_game_data, p_param->cp_btl_score->result );
          }
        }
      }
      if( p_wk->sc_state == WIFIBATTLEMATCH_NET_SC_STATE_SUCCESS )
      { 
        //相手の不正フラグを受け取る
        ((BATTLEMATCH_BATTLE_SCORE *)(p_param->cp_btl_score))->is_other_dirty = WIFIBATTLEMATCH_NET_SC_GetDirtyFlag( p_wk->p_net );
        *p_seq = SEQ_SC_HEAP_EXIT;
      }
      
      if( p_wk->sc_state != WIFIBATTLEMATCH_NET_SC_STATE_UPDATE  )
      {
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:     //戻る
          WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
          /* fallthrough */
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
#ifndef SCREPORT_TIMEOUT_IS_DISCONNECT
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
          DWC_RAPCOMMON_ResetSubHeapID();
          break;
#endif
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          DWC_RAPCOMMON_ResetSubHeapID();
          break;
        }
      }
    }
    break;

  case SEQ_SC_HEAP_EXIT:
    DWC_RAPCOMMON_ResetSubHeapID();

    //相手に切断されていたら、録画を飛ばす
    if( p_param->mode == WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR )
    { 
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_EndRec );
    }
    else
    { 
      *p_seq = SEQ_INIT_DISCONNECT;
    }
    break;

    //-------------------------------------
    ///	切断処理
    //=====================================
  case SEQ_INIT_DISCONNECT:
    //ここまで切断等がない場合は相手を知り合いに登録
    {           
      SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      ETC_SAVE_WORK *p_etc  = SaveData_GetEtc( p_sv_ctrl );
      WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data = p_param->p_enemy_data;
      EtcSave_SetAcquaintance( p_etc, MyStatus_GetID( (MYSTATUS*)p_enemy_data->mystatus ) );
    }
    WIFIBATTLEMATCH_NET_StartDisConnect( p_wk->p_net );
    *p_seq = SEQ_WAIT_DISCONNECT;
    break;

  case SEQ_WAIT_DISCONNECT:
    if( WIFIBATTLEMATCH_NET_WaitDisConnect( p_wk->p_net ) )
    { 
      *p_seq = SEQ_START_SAKE_RECORD;
    }

    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      *p_seq  = SEQ_START_SAKE_RECORD;
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;
    }
    break;
    
  case SEQ_START_SAKE_RECORD:
    //履歴を入れる
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_RND_RECORD_DATA data;
      GFL_STD_MemClear( &data, sizeof(WIFIBATTLEMATCH_GDB_RND_RECORD_DATA) );
      data.record_data  = *p_wk->p_param->p_record_data;
      data.save_idx     = p_wk->p_param->p_recv_data->record_save_idx;
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_RECORD, &data );
      *p_seq  = SEQ_WAIT_SAKE_RECORD;
    }
    break;

  case SEQ_WAIT_SAKE_RECORD:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq = SEQ_START_RESULT_MSG;
      }

      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      { 
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
          break;
        }
      }
    } 
    break;

  case SEQ_START_RESULT_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_33, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_END );
    break;
    
  case SEQ_END:
    p_param->result = WIFIBATTLEMATCH_CORE_RESULT_NEXT_REC;
    WBM_SEQ_End( p_seqwk ); 
    break;

    //-------------------------------------
    ///	共通
    //=====================================
  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  録画終了後
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_EndRec( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_NET_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,

    //大会期間チェック
    SEQ_START_SUBSEQ_CUPDATE,
    SEQ_WAIT_SUBSEQ_CUPDATE,

    //共通
    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_NET_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    break;

  case SEQ_START_SAVE:
    { 
      Util_SaveScore( p_wk, p_wk->p_param->p_wifi_sake_data );
    }
    GAMEDATA_SaveAsyncStart(p_param->p_param->p_game_data);
    *p_seq  = SEQ_WAIT_SAVE;
    break;

  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT result;
      result = GAMEDATA_SaveAsyncMain(p_param->p_param->p_game_data);
      if( result == SAVE_RESULT_OK )
      { 
        *p_seq  = SEQ_START_SUBSEQ_CUPDATE;
      }
      else if(result == SAVE_RESULT_NG)
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
      }
    }
    break;

  case SEQ_START_SUBSEQ_CUPDATE:
    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_NONE:
      Util_SubSeq_Start( p_wk, WbmWifiSubSeq_CheckDate );
      *p_seq  = SEQ_WAIT_SUBSEQ_CUPDATE;
      break;
    }
    break;
  case SEQ_WAIT_SUBSEQ_CUPDATE:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMESAFE )
      { 
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
      }
      else if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEOVER ||
          ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEBEFORE )
      { 
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
      }
      else if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

    //-------------------------------------
    ///	共通
    //=====================================
  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  大会続行確認処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_CupContinue( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK_ERR,

    SEQ_START_SELECT_CONTINUE_MSG,
    SEQ_START_SELECT_CONTINUE,
    SEQ_WAIT_SELECT_CONTINUE,

    SEQ_START_SUBSEQ_UNREGISTER,
    SEQ_WAIT_SUBSEQ_UNREGISTER,
    SEQ_NEXT_DISCONNECT,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECK_ERR:
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:     //戻る
    case WIFIBATTLEMATCH_NET_ERROR_NONE:
      *p_seq  = SEQ_START_SELECT_CONTINUE_MSG;
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;
    }
    break;

    //-------------------------------------
    ///	続行確認
    //=====================================
  case SEQ_START_SELECT_CONTINUE_MSG:
    GFL_BG_SetVisible( BG_FRAME_M_TEXT, TRUE );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_37, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SELECT_CONTINUE );
    break;
  case SEQ_START_SELECT_CONTINUE:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_CUPMENU_CONT );
    *p_seq     = SEQ_WAIT_SELECT_CONTINUE;
    break;
  case SEQ_WAIT_SELECT_CONTINUE:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //対戦する
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Matching );
          break;
        case 1://参加を解除する
          *p_seq  = SEQ_START_SUBSEQ_UNREGISTER;
          break;
        case 2://やめる
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupEnd );
          break;
        }
      }
    }
    break;

  case SEQ_START_SUBSEQ_UNREGISTER:
    Util_SubSeq_Start( p_wk, WbmWifiSubSeq_UnRegister );
    *p_seq  = SEQ_WAIT_SUBSEQ_UNREGISTER;
    break;
  case SEQ_WAIT_SUBSEQ_UNREGISTER:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_FALSE||
          ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_NONE )
      { 
        *p_seq  = SEQ_START_SELECT_CONTINUE_MSG;
      }
      else if( ret == WBM_WIFI_SUBSEQ_UNREGISTER_RET_TRUE )
      { 
        *p_seq  = SEQ_NEXT_DISCONNECT;
      }

      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_NEXT_DISCONNECT:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
    break;

  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  大会終了処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_END_MSG,
    SEQ_START_END_LIST,
    SEQ_SELECT_END_LIST,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_END_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_23, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_END_LIST );
    break;
    
  case SEQ_START_END_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_SELECT_END_LIST;
    break;

  case SEQ_SELECT_END_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //はい
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextCheckDate );
          break;
        case 1: //いいえ
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CupContinue );
          break;
        }
      }
    }
    break;

  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  切断処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_DisConnextSendTime( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  enum
  { 
    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_START_LOGINTIME_MSG,
    SEQ_SEND_SAKE_LOGINTIME,
    SEQ_WAIT_SAKE_LOGINTIME,
    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    *p_seq  = SEQ_WAIT_MSG;
    break;
  case SEQ_START_SAVE:
    GAMEDATA_SaveAsyncStart( p_param->p_param->p_game_data );
    *p_seq  = SEQ_WAIT_SAVE;
    break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT ret;
      ret = GAMEDATA_SaveAsyncMain( p_param->p_param->p_game_data );
      if( ret == SAVE_RESULT_OK )
      {
        *p_seq  = SEQ_START_LOGINTIME_MSG;
      }
    }
    break;
  case SEQ_START_LOGINTIME_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_013, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_SEND_SAKE_LOGINTIME );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_SEND_SAKE_LOGINTIME:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_MYINFO, NULL );
      *p_seq  = SEQ_WAIT_SAKE_LOGINTIME;
    }
    break;

  case SEQ_WAIT_SAKE_LOGINTIME:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_END;

        //エラー
        WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE );
      }
    }
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextEnd );
    break;


  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  日時チェックをして終了
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_DisConnextCheckDate( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_SUBSEQ_CUPDATE,
    SEQ_WAIT_SUBSEQ_CUPDATE,
    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_SUBSEQ_CUPDATE:
    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      break;
    case WIFIBATTLEMATCH_NET_ERROR_NONE:
      Util_SubSeq_Start( p_wk, WbmWifiSubSeq_CheckDate );
      *p_seq  = SEQ_WAIT_SUBSEQ_CUPDATE;
      break;
    }
    break;

  case SEQ_WAIT_SUBSEQ_CUPDATE:
    { 
      WBM_WIFI_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret != WBM_WIFI_SUBSEQ_RET_NONE )
      { 
        *p_seq  = SEQ_END;
      }
      
      if( ret == WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
      }

      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Err_ReturnLogin );
        break;
      }
    }
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_DisConnextSendTime );
    break;

  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  終了処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_DisConnextEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  //タイトルへ一気に戻る場合→なくなりました
 //   p_param->result = WIFIBATTLEMATCH_CORE_RESULT_END_WIFICUP;

  WBM_SEQ_End( p_seqwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  エラー処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Err_ReturnLogin( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  p_param->result = WIFIBATTLEMATCH_CORE_RESULT_ERR_NEXT_LOGIN;
  
  if( p_wk->p_net )
  { 
    WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );
    p_wk->p_net = NULL;
  }

  NetErr_ExitNetSystem();

  WBM_SEQ_End( p_seqwk );
}
//=============================================================================
/**
///	サブシーケンス関数（メインシーケンス上で動くシーケンス処理）
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  大会期間をチェック
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSubSeq_CheckDate( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK_CUP_STATUS,

    SEQ_CHECK_DATE,

    SEQ_START_DATEBEFORE_MSG,
    SEQ_START_DATE_MSG,
    SEQ_CHECK_STATUS,
    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_CHECK_LOCK,
    SEQ_START_UNLOCK_MSG,
    SEQ_START_SEND_MSG,
    SEQ_SEND_GPF_CUPSTATUS,
    SEQ_WAIT_GPF_CUPSTATUS,
    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECK_CUP_STATUS:
    *p_seq  = SEQ_CHECK_DATE;
    break;

  case SEQ_CHECK_DATE:
    { 
      RTCDate start_date;
      RTCDate end_date;
      RTCDate now_date;
      RTCTime time;
      BOOL    ret;
      s32 start;
      s32 now;
      s32 end;

      REGULATION_CARDDATA *p_reg = p_wk->p_reg;

#ifdef BUGFIX_BTS7867_20100716
      if( !GFL_NET_IsInit() )
      {
        NetErr_DispCallFatal();
      }
#endif //BUGFIX_BTS7867_20100716

      ret = DWC_TOOL_GetLocalDateTime( &now_date, &time );

#ifdef PM_DEBUG
      if( ((*DEBUGWIN_SERVERTIME_GetFlag()) && (*DEBUGWIN_BATTLE_GetFlag()))
        || (*DEBUGWIN_SERVERTIME_USER_GetFlag()) )
      {
        now_date.day  += 1;
      }
#endif //PM_DEBUG

      start_date.year  = Regulation_GetCardParam( p_reg, REGULATION_CARD_START_YEAR );
      start_date.month = Regulation_GetCardParam( p_reg, REGULATION_CARD_START_MONTH );
      start_date.day   = Regulation_GetCardParam( p_reg, REGULATION_CARD_START_DAY );
      start_date.week  = RTC_WEEK_SUNDAY; //RTC_ConvertDateToDay関数のなかで範囲チェックしてるのでだますために適当な曜日をいれる

      end_date.year    = Regulation_GetCardParam( p_reg, REGULATION_CARD_END_YEAR );
      end_date.month   = Regulation_GetCardParam( p_reg, REGULATION_CARD_END_MONTH );
      end_date.day     = Regulation_GetCardParam( p_reg, REGULATION_CARD_END_DAY );
      end_date.week    = RTC_WEEK_SUNDAY; //RTC_ConvertDateToDay関数のなかで範囲チェックしてるのでだますために適当な曜日をいれる

      start = RTC_ConvertDateToDay( &start_date );
      end   = RTC_ConvertDateToDay( &end_date );
      now   = RTC_ConvertDateToDay( &now_date );

      OS_TFPrintf( 3, "開催期間チェック 開始%d.%d.%d[%d] 現在%d.%d.%d[%d] 終了%d.%d.%d[%d]\n",
                        start_date.year, start_date.month, start_date.day, start,
                        now_date.year, now_date.month, now_date.day, now,
                        end_date.year, end_date.month, end_date.day, end );

      if( ret && ( now < start ) )
      { 
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEBEFORE;
        *p_seq  = SEQ_START_DATEBEFORE_MSG;
      }
      else if( ret && ( start <= now && now <= end ) )
      {
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMESAFE;
        *p_seq  = SEQ_END;
      }
      else
      { 
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_CUPDATE_RET_TIMEOVER;
        *p_seq  = SEQ_START_DATE_MSG;
      }
    }
    break;

  case SEQ_START_DATEBEFORE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_38, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_END );
    break;

  case SEQ_START_DATE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_25, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_CHECK_STATUS );
    break;


  case SEQ_CHECK_STATUS:
    {
      if( Regulation_GetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS ) == DREAM_WORLD_MATCHUP_SIGNUP
        || Regulation_GetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS ) == DREAM_WORLD_MATCHUP_ENTRY )
      {
        *p_seq  = SEQ_START_SAVE_MSG;
      }
      else
      { 
        *p_seq  = SEQ_END;
      }
    }
    break;

  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    break;

  case SEQ_START_SAVE:
    { 
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      BATTLE_BOX_SAVE_OffLockFlg( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI );

      //大会終了状態にしてセーブ
      Regulation_SetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_END );

      GAMEDATA_SaveAsyncStart( p_param->p_param->p_game_data );
      *p_seq  = SEQ_WAIT_SAVE;
    }
    break;

  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT ret = GAMEDATA_SaveAsyncMain( p_param->p_param->p_game_data );
      if( ret == SAVE_RESULT_OK )
      { 
        *p_seq  = SEQ_CHECK_LOCK;
      }
    }
    break;


  case SEQ_CHECK_LOCK:
    {
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      if( BATTLE_BOX_SAVE_GetLockType( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI ) )
      {
        *p_seq  = SEQ_START_UNLOCK_MSG;
      }
      else
      { 
        *p_seq  = SEQ_START_SEND_MSG;
      }
    }
    break;

  case SEQ_START_UNLOCK_MSG:
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_26, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SEND_MSG );
    break;
    
  case SEQ_START_SEND_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_18, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_SEND_GPF_CUPSTATUS );
    break;

  case SEQ_SEND_GPF_CUPSTATUS:
    { 
      DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA data;
      GFL_STD_MemClear( &data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      data.WifiMatchUpState = DREAM_WORLD_MATCHUP_END;
    #ifdef BUGFIX_GFBTS1976_20100714
      data.profileID  = WIFIBATTLEMATCH_NET_GetGsProfileID( p_wk->p_net );
    #endif //BUGFIX_GFBTS1976_20100714
      p_wk->p_param->p_gpf_data->WifiMatchUpState = DREAM_WORLD_MATCHUP_END;
      WIFIBATTLEMATCH_NET_StartSendGpfData( p_wk->p_net, &data, HEAPID_WIFIBATTLEMATCH_CORE ); 
      *p_seq  = SEQ_WAIT_GPF_CUPSTATUS;
    }
    break;
    
 case SEQ_WAIT_GPF_CUPSTATUS:
    { 
      WIFIBATTLEMATCH_SEND_GPFDATA_RET ret;
      
      ret = WIFIBATTLEMATCH_NET_WaitSendGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS )
      { 
        *p_seq  = SEQ_END;
      }
      else if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY )
      {
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_CUPDATE_RET_SERVER;
        *p_seq  = SEQ_END;
      }

      //ここはサブシーケンスなのでエラー処理はこの上で行う
    }
    break;

  case SEQ_END:
    WBM_SEQ_End( p_seqwk );
    break;

  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI大会  登録解除処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_seq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSubSeq_UnRegister( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  {
    SEQ_CHECK_CUP_STATUS,
    SEQ_START_NOT_CUP_MSG,
    SEQ_START_UNREGISTER_MSG,
    SEQ_START_UNREGISTER_LIST,
    SEQ_SELECT_UNREGISTER_LIST,
    SEQ_START_CONFIRM_MSG,
    SEQ_START_CONFIRM_LIST,
    SEQ_SELECT_CONFIRM_LIST,

    //セーブ
    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,

    SEQ_START_UNLOCK_MSG,
    //GPF送信
    SEQ_START_SEND_MSG, 
    SEQ_SEND_GPF_CUPSTATUS,
    SEQ_WAIT_GPF_CUPSTATUS,

    SEQ_END,
    SEQ_WAIT_MSG,

  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECK_CUP_STATUS:
    if( p_wk->p_param->p_gpf_data->WifiMatchUpState == DREAM_WORLD_MATCHUP_ENTRY ||
        p_wk->p_param->p_gpf_data->WifiMatchUpState == DREAM_WORLD_MATCHUP_SIGNUP )
    { 
      p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_UNREGISTER_RET_FALSE;
      *p_seq  = SEQ_START_UNREGISTER_MSG;
    }
    else
    { 
      p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_UNREGISTER_RET_NONE;
      *p_seq  = SEQ_START_NOT_CUP_MSG;
    }
    break;
  case SEQ_START_NOT_CUP_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_27, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_END );
    break;
  case SEQ_START_UNREGISTER_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_28, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_UNREGISTER_LIST );
    break;
  case SEQ_START_UNREGISTER_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO_DEFAULT_NO );
    *p_seq     = SEQ_SELECT_UNREGISTER_LIST;
    break;
  case SEQ_SELECT_UNREGISTER_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //はい
          *p_seq  = SEQ_START_CONFIRM_MSG;
          break;
        case 1: //いいえ
          *p_seq  = SEQ_END;
          break;
        }
      }
    }
    break;
  case SEQ_START_CONFIRM_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_29, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_CONFIRM_LIST );
    break;
  case SEQ_START_CONFIRM_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO_DEFAULT_NO );
    *p_seq     = SEQ_SELECT_CONFIRM_LIST;
    break;
  case SEQ_SELECT_CONFIRM_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0: //はい
          *p_seq  = SEQ_START_SAVE_MSG;
          break;
        case 1: //いいえ
          *p_seq  = SEQ_END;
          break;
        }
      }
    }
    break;


  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    break;

  case SEQ_START_SAVE:
    {
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      BATTLE_BOX_SAVE_OffLockFlg( p_bbox_save, BATTLE_BOX_LOCK_BIT_WIFI );

      //リタイヤ状態にしてセーブ
      Regulation_SetCardParam( p_wk->p_reg, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_RETIRE );

      GAMEDATA_SaveAsyncStart( p_param->p_param->p_game_data );
      *p_seq  = SEQ_WAIT_SAVE;
    }
    break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT ret = GAMEDATA_SaveAsyncMain( p_param->p_param->p_game_data );
      if( ret == SAVE_RESULT_OK )
      { 
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_UNREGISTER_RET_TRUE;
        *p_seq  = SEQ_START_UNLOCK_MSG;
      }
      else if( ret == SAVE_RESULT_NG )
      { 
        *p_seq  = SEQ_END;
      }
    }
    break;

  case SEQ_START_UNLOCK_MSG:
    Util_PlayerInfo_RenewalData( p_wk, PLAYERINFO_WIFI_UPDATE_TYPE_UNLOCK );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_26, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SEND_MSG );
    break;


  case SEQ_START_SEND_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_18, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_SEND_GPF_CUPSTATUS );
    break;

  case SEQ_SEND_GPF_CUPSTATUS:
    { 
      DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA data;
      GFL_STD_MemClear( &data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_SET_DATA) );
      data.WifiMatchUpState = DREAM_WORLD_MATCHUP_RETIRE;
    #ifdef BUGFIX_GFBTS1976_20100714
      data.profileID  = WIFIBATTLEMATCH_NET_GetGsProfileID( p_wk->p_net );
    #endif //BUGFIX_GFBTS1976_20100714
      p_wk->p_param->p_gpf_data->WifiMatchUpState = DREAM_WORLD_MATCHUP_RETIRE;
      WIFIBATTLEMATCH_NET_StartSendGpfData( p_wk->p_net, &data, HEAPID_WIFIBATTLEMATCH_CORE ); 
      *p_seq  = SEQ_WAIT_GPF_CUPSTATUS;
    }
    break;

  case SEQ_WAIT_GPF_CUPSTATUS:
    { 
      WIFIBATTLEMATCH_SEND_GPFDATA_RET ret;
      
      ret = WIFIBATTLEMATCH_NET_WaitSendGpfData( p_wk->p_net );
      if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_SUCCESS )
      { 
        *p_seq  = SEQ_END;
      }
      else if( ret == WIFIBATTLEMATCH_SEND_GPFDATA_RET_DIRTY )
      {
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_UNREGISTER_RET_SERVER;
        *p_seq  = SEQ_END;
      }
      //ここはサブシーケンスなのでエラー処理はこの上で行う
    } 
    break;


  case SEQ_END:
    WBM_SEQ_End( p_seqwk );
    break;

  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  不正チェックサーバーへ1匹ずつ送り署名を貰う
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmWifiSubSeq_EvilCheck( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START,
    SEQ_START_EVIL_CHECK,
    SEQ_WAIT_EVIL_CHECK,
    SEQ_END_CHECK,
    SEQ_END,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	      = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param    = p_wk->p_param;
  WIFIBATTLEMATCH_ENEMYDATA   *p_my_data  = p_param->p_player_data;

  switch( *p_seq )
  { 
  case SEQ_START:
    { 
      OS_TFPrintf( 3, "自分のポケモン署名取得開始\n" );
    }
    (*p_seq)  = SEQ_START_EVIL_CHECK;
    break;

  case SEQ_START_EVIL_CHECK:
    { 
      POKEPARTY *p_my_party = (POKEPARTY*)p_param->p_player_data->pokeparty;
      GFL_STD_MemClear( &p_wk->evilecheck_data, sizeof(WIFIBATTLEMATCH_NET_EVILCHECK_DATA));
      WIFIBATTLEMATCH_NET_StartEvilCheck( p_wk->p_net, p_my_party, WIFIBATTLEMATCH_NET_EVILCHECK_TYPE_PARTY );
      (*p_seq)  = SEQ_WAIT_EVIL_CHECK;
      DEBUG_WIFICUP_Printf( "EvilCheck Start\n" );
    }
    break;

  case SEQ_WAIT_EVIL_CHECK:
    {
      WIFIBATTLEMATCH_NET_EVILCHECK_RET ret;
      ret = WIFIBATTLEMATCH_NET_WaitEvilCheck( p_wk->p_net, &p_wk->evilecheck_data );
      if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_SUCCESS )
      { 
        DEBUG_WIFICUP_Printf( "EvilCheck Success\n" );
        (*p_seq)  = SEQ_END_CHECK;
      }
      else if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_ERROR )
      { 
        DEBUG_WIFICUP_Printf( "EvilCheck Error\n" );
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_EVILCHECK_RET_NET_ERR;
        *p_seq  = SEQ_END;
      }
      else if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_SERVERERR )
      {
        p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_EVILCHECK_RET_SERVER_ERR;
        *p_seq  = SEQ_END;
      }
    }
    break;

  case SEQ_END_CHECK:
    //データを格納
    OS_TFPrintf( 3, "ポケモン結果[不正番号%d]status=%d\n", p_wk->evilecheck_data.poke_result[0], p_wk->evilecheck_data.status_code );
    if( p_wk->evilecheck_data.status_code == 0 )
    { 
      OS_TFPrintf( 3, "-=-=-=ポケモン証明-=-=-=\n" );
      {
        int len = 0;
        for(len = 0; len < 128; len++ )
        {
          OS_TFPrintf( 3,"%x ", (u8) p_wk->evilecheck_data.sign[len] );
          if( (len + 1) % 16 == 0 ) OS_TFPrintf( 3,"\n");
        }
        OS_TFPrintf( 3, "\n" );
      }
      OS_TFPrintf( 3, "-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");

      //署名を保存
      Util_SetMyDataSign( p_my_data, &p_wk->evilecheck_data );

      DEBUG_WIFICUP_Printf( "EvilCheck 成功！！\n" );
      p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_EVILCHECK_RET_SUCCESS;
      *p_seq  = SEQ_END;
    }
    else
    { 
      DEBUG_WIFICUP_Printf( "EvilCheck 不正！！\n");
      p_wk->subseq_ret  = WBM_WIFI_SUBSEQ_EVILCHECK_RET_DARTY;
      *p_seq  = SEQ_END;
    }
    break;

  case SEQ_END:
    WBM_SEQ_End( p_seqwk );
    break;
  }


}
//=============================================================================
/**
 *  便利
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  自分の情報を表示
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  if( p_wk->p_playerinfo == NULL )
  {
    MYSTATUS    *p_my;
    GFL_CLUNIT	*p_unit;
    SAVE_CONTROL_WORK *p_sv;
    REGULATION_SAVEDATA *p_reg_sv;
    REGULATION_VIEWDATA *p_reg_view;

    PLAYERINFO_WIFICUP_DATA info_setup;

    const REGULATION_CARDDATA *cp_reg_card  = p_wk->p_reg;

    p_my  = GAMEDATA_GetMyStatus( p_wk->p_param->p_param->p_game_data); 
    p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    p_sv = GAMEDATA_GetSaveControlWork( p_wk->p_param->p_param->p_game_data );
    p_reg_sv  = SaveData_GetRegulationSaveData( p_sv );
    p_reg_view  = RegulationSaveData_GetRegulationView( p_reg_sv, REGULATION_CARD_TYPE_WIFI );

    //自分のデータを表示
    GFL_STD_MemClear( &info_setup, sizeof(PLAYERINFO_WIFICUP_DATA) );
    GFL_STD_MemCopy( Regulation_GetCardCupNamePointer( cp_reg_card ), info_setup.cup_name, 2*(WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE) );

    info_setup.start_date = GFDATE_Set( 
        Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_START_YEAR ),
        Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_START_MONTH ), 
        Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_START_DAY ),
          0);

    info_setup.end_date = GFDATE_Set( 
        Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_END_YEAR ),
        Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_END_MONTH ), 
        Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_END_DAY ),
          0);

    info_setup.trainerID  = MyStatus_GetTrainerView( p_my );
    info_setup.rate = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_RATE );
    info_setup.btl_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_WIN ) + RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_LOSE );
    info_setup.bgm_no = Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_BGM );


    if( info_setup.btl_cnt == 0 &&  info_setup.rate == 0 )
    { 
      info_setup.rate = WIFIBATTLEMATCH_GDB_DEFAULT_RATEING;
    }

    p_wk->p_playerinfo	= PLAYERINFO_WIFI_Init( &info_setup, FALSE, p_my, p_unit, p_wk->p_res, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, p_reg_view, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  自分の情報を破棄
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  if( p_wk->p_playerinfo )
  { 
    PLAYERINFO_WIFI_Exit( p_wk->p_playerinfo );
    p_wk->p_playerinfo  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  自分のカードをスライドイン
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk  ワーク
 *
 *	@return TRUEで完了  FALSEで処理中
 */
//-----------------------------------------------------------------------------
static BOOL Util_PlayerInfo_MoveIn( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  return PLAYERINFO_MoveMain( p_wk->p_playerinfo );
}

//----------------------------------------------------------------------------
/**
 *	@brief  自分のカードをスライドアウト
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk   ワーク
 *
 *	@return TRUEで完了  FALSEで処理中
 */
//-----------------------------------------------------------------------------
static BOOL Util_PlayerInfo_MoveOut( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  return PLAYERINFO_MoveOutMain( p_wk->p_playerinfo );
}
//----------------------------------------------------------------------------
/**
 *	@brief  自分のカード作成  すでにスライドインしている版
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_CreateStay( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  Util_PlayerInfo_Create( p_wk );

  while( !Util_PlayerInfo_MoveIn( p_wk ) )
  { 

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  自分のカードのデータを更新
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk
 *	@param	type 
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_RenewalData( WIFIBATTLEMATCH_WIFI_WORK *p_wk, PLAYERINFO_WIFI_UPDATE_TYPE type )
{ 
  if( p_wk->p_playerinfo )
  {
    PLAYERINFO_WIFI_RenewalData( p_wk->p_playerinfo, type, p_wk->p_msg, p_wk->p_que, p_wk->p_font, HEAPID_WIFIBATTLEMATCH_CORE );
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  対戦者の情報を表示
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_MatchInfo_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_enemy_data )
{ 
  if( p_wk->p_matchinfo == NULL )
  { 
    GFL_CLUNIT  *p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    REGULATION_CARD_BGM_TYPE type = Regulation_GetCardParam( p_wk->p_reg, REGULATION_CARD_BGM );
    p_wk->p_matchinfo		= MATCHINFO_Init( cp_enemy_data, p_unit, p_wk->p_res, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, WIFIBATTLEMATCH_TYPE_WIFICUP, TRUE, type, HEAPID_WIFIBATTLEMATCH_CORE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  対戦者の情報を破棄
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_MatchInfo_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  if( p_wk->p_matchinfo )
  { 
    MATCHINFO_Exit( p_wk->p_matchinfo );
    p_wk->p_matchinfo = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  対戦者のカードをスライドイン
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk  ワーク
 *
 *	@return TRUEで完了  FALSEで処理中
 */
//-----------------------------------------------------------------------------
static BOOL Util_MatchInfo_Main( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{
  return MATCHINFO_MoveMain( p_wk->p_matchinfo );
}

//----------------------------------------------------------------------------
/**
 *	@brief  対戦者カードを消去
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_Matchinfo_Clear( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{
  if( p_wk->p_matchinfo )
  {
    Util_MatchInfo_Delete( p_wk );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  バトルボックスを表示
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_BtlBox_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  if( p_wk->p_btlbox == NULL )
  { 
    GFL_CLUNIT  *p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->p_param->p_param->p_game_data );
    BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
    p_wk->p_btlbox  = WBM_BTLBOX_Init( p_bbox_save, p_unit, p_wk->p_res, p_wk->p_small_font, p_wk->p_que, p_wk->p_msg, HEAPID_WIFIBATTLEMATCH_CORE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  バトルボックスを表示
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_BtlBox_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  if( p_wk->p_btlbox )
  { 
    WBM_BTLBOX_Exit( p_wk->p_btlbox );
    p_wk->p_btlbox  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  バトルボックスを移動
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 *	@retval TRUE移動完了 FALSE移動中
 */
//-----------------------------------------------------------------------------
static BOOL Util_BtlBox_MoveIn( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  return WBM_BTLBOX_MoveInMain( p_wk->p_btlbox );
}
//----------------------------------------------------------------------------
/**
 *	@brief  バトルボックスを移動
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 *	@retval TRUE移動完了 FALSE移動中
 */
//-----------------------------------------------------------------------------
static BOOL Util_BtlBox_MoveOut( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  return WBM_BTLBOX_MoveOutMain( p_wk->p_btlbox );
}
//----------------------------------------------------------------------------
/**
 *	@brief  リスト初期化
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk  ワーク
 *	@param	type                            リストの種類
 */
//-----------------------------------------------------------------------------
static void Util_List_Create( WIFIBATTLEMATCH_WIFI_WORK *p_wk, UTIL_LIST_TYPE type )
{ 
  if( p_wk->p_list == NULL )
  { 
    WBM_LIST_SETUP  setup;
    GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
    setup.p_msg   = p_wk->p_msg;
    setup.p_font  = p_wk->p_font;
    setup.p_que   = p_wk->p_que;
    setup.frm     = BG_FRAME_M_TEXT;
    setup.font_plt= PLT_FONT_M;
    setup.frm_plt = PLT_LIST_M;
    setup.frm_chr = CGR_OFS_M_LIST;

    switch( type )
    {
    case UTIL_LIST_TYPE_YESNO_DEFAULT_NO:
      setup.default_idx = 1;
      /* fallthrough */
    case UTIL_LIST_TYPE_YESNO:
      setup.strID[0]= WIFIMATCH_WIFI_SELECT_04;
      setup.strID[1]= WIFIMATCH_WIFI_SELECT_05;
      setup.list_max= 2;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 1;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      break;

    case UTIL_LIST_TYPE_JOIN:
      setup.strID[0]= WIFIMATCH_WIFI_SELECT_01;
      setup.strID[1]= WIFIMATCH_WIFI_SELECT_02;
      setup.strID[2]= WIFIMATCH_WIFI_SELECT_03;
      setup.list_max= 3;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 2;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      break;

    case UTIL_LIST_TYPE_DECIDE:
      setup.strID[0]= WIFIMATCH_WIFI_SELECT_06;
      setup.strID[1]= WIFIMATCH_WIFI_SELECT_07;
      setup.list_max= 2;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 1;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      break;

    case UTIL_LIST_TYPE_CUPMENU:
      setup.strID[0]= WIFIMATCH_WIFI_SELECT_08;
      setup.strID[1]= WIFIMATCH_WIFI_SELECT_09;
      setup.strID[2]= WIFIMATCH_WIFI_SELECT_10;
      setup.list_max= 3;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 2;
      p_wk->p_list  = WBM_LIST_InitEx( &setup, 32/2 - 26/2, (24-6)/2 - 3*2/2, 26, 3*2, HEAPID_WIFIBATTLEMATCH_CORE );
      break;

    case UTIL_LIST_TYPE_CUPMENU_CONT:
      setup.strID[0]= WIFIMATCH_WIFI_SELECT_11;
      setup.strID[1]= WIFIMATCH_WIFI_SELECT_12;
      setup.strID[2]= WIFIMATCH_WIFI_SELECT_13;
      setup.list_max= 3;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 2;
      p_wk->p_list  = WBM_LIST_InitEx( &setup, 32/2 - 26/2, (24-6)/2 - 3*2/2, 26, 3*2, HEAPID_WIFIBATTLEMATCH_CORE );
      break;
    }

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  リスト破棄
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_List_Delete( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{
  if( p_wk->p_list )
  { 
    WBM_LIST_Exit( p_wk->p_list );
    p_wk->p_list  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  リストメイン
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 *
 *	@return 選択したもの
 */
//-----------------------------------------------------------------------------
static u32 Util_List_Main( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  if( p_wk->p_list )
  { 
    return WBM_LIST_Main( p_wk->p_list );
  }
  else
  { 
    DEBUG_WIFICUP_Printf( "List not exist !!!\n" );
    return WBM_LIST_SELECT_NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  サブシーケンススタート
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk   ワーク
 *	@param	seq_function                      サブシーケンス
 */
//-----------------------------------------------------------------------------
static void Util_SubSeq_Start( WIFIBATTLEMATCH_WIFI_WORK *p_wk, WBM_SEQ_FUNCTION seq_function )
{ 
  WBM_SEQ_SetNext( p_wk->p_subseq, seq_function );
  p_wk->subseq_ret = WBM_WIFI_SUBSEQ_RET_NONE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  サブシーケンスメイン
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 *
 *	@return WBM_WIFI_SUBSEQ_RET_NONE動作中  それ以外のときはサブシーケンスが終了し戻り値を返している
 */
//-----------------------------------------------------------------------------
static WBM_WIFI_SUBSEQ_RET Util_SubSeq_Main( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  WBM_SEQ_Main( p_wk->p_subseq );
  if( WBM_SEQ_IsEnd( p_wk->p_subseq ) )
  {
    return p_wk->subseq_ret;
  }
  else
  { 
    return WBM_WIFI_SUBSEQ_RET_NONE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  自分のデータを初期化
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data ワーク
 */
//-----------------------------------------------------------------------------
static void Util_InitMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{ 
  GFL_STD_MemClear( p_my_data, sizeof(WIFIBATTLEMATCH_ENEMYDATA) );

  { 
    SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->p_param->p_param->p_game_data );
    BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
    POKEPARTY         *p_party  =  BATTLE_BOX_SAVE_MakePokeParty( p_bbox_save, HEAPID_WIFIBATTLEMATCH_CORE );

    PokeParty_InitWork( (POKEPARTY*)p_my_data->pokeparty );

    GFL_STD_MemCopy( p_party, (POKEPARTY*)p_my_data->pokeparty, PokeParty_GetWorkSize() );

    GFL_HEAP_FreeMemory( p_party );

#ifdef PM_DEBUG
    {
      //事前に不正にされていたら、不正にする
      DEBUGWIN_EVILCHECK *p_data  = DEBUGWIN_EVILCHECK_GetInstance();
      if( p_data->is_evil )
      {
        int i;
        POKEMON_PARAM *p_pp = PokeParty_GetMemberPointer( (POKEPARTY*)p_my_data->pokeparty, 0 );
        //不正にする
        for( i = 0; i < PTL_ABILITY_MAX; i++ )
        {
          p_data->st_exp[i]  = PP_Get( p_pp, ID_PARA_hp_exp+i, NULL );
          PP_Put( p_pp, ID_PARA_hp_deb_exp+i, 255 );
          PP_Renew( p_pp );
        }
      }
    }
#endif //PM_DEBUG

  }

  p_my_data->btl_server_version  = BTL_NET_SERVER_VERSION;
}
//----------------------------------------------------------------------------
/**
 *	@brief  自分のデータを格納
 *	        SAKE GPFからデータをもらってきていること
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data  データ格納先
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *cp_wk      ワーク
 */
//-----------------------------------------------------------------------------
static void Util_SetMyDataInfo( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, const WIFIBATTLEMATCH_WIFI_WORK *cp_wk )
{ 
  SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork(cp_wk->p_param->p_param->p_game_data);
  {
    MYSTATUS  *p_my;
    p_my  = GAMEDATA_GetMyStatus(cp_wk->p_param->p_param->p_game_data);
    MyStatus_Copy( p_my, (MYSTATUS*)p_my_data->mystatus );
  }
  { 
    const MYPMS_DATA *cp_mypms;
    cp_mypms        = SaveData_GetMyPmsDataConst( p_sv );
    MYPMS_GetPms( cp_mypms, MYPMS_PMS_TYPE_INTRODUCTION, &p_my_data->pms );
  }
  p_my_data->win_cnt    = cp_wk->p_param->p_wifi_sake_data->win;
  p_my_data->lose_cnt   = cp_wk->p_param->p_wifi_sake_data->lose;
  p_my_data->rate       = cp_wk->p_param->p_wifi_sake_data->rate;

  p_my_data->sake_recordID = WIFIBATTLEMATCH_GDB_GetRecordID( cp_wk->p_net );

  { 
    //PID
    WIFI_LIST *p_wlist = GAMEDATA_GetWiFiList( cp_wk->p_param->p_param->p_game_data );
    p_my_data->profileID  = WifiList_GetMyGSID( p_wlist );
  }

  { 
    const REGULATION_CARDDATA *cp_reg_card  = cp_wk->p_reg;
    p_my_data->wificup_no      = Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_CUPNO );
    p_my_data->reg_crc  = Regulation_GetCrc( cp_reg_card );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  署名を追加
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data      ワーク
 *	@param	WIFIBATTLEMATCH_NET_EVILCHECK_DATA  *cp_evilecheck_data 不正チェック
 *	@param	index                                                   何番目のポケモンか
 *
 */
//-----------------------------------------------------------------------------
static void Util_SetMyDataSign( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, const WIFIBATTLEMATCH_NET_EVILCHECK_DATA  *cp_evilecheck_data )
{ 
  OS_TFPrintf( 3, "署名を追加\n" );
  GFL_STD_MemCopy( cp_evilecheck_data->sign, p_my_data->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ポケモン証明
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data  証明したい相手のデータ
 *	@param	heapID  ヒープID
 *
 *	@return TRUEで証明  FALSEで不正
 */
//-----------------------------------------------------------------------------
static BOOL Util_VerifyPokeData( WIFIBATTLEMATCH_ENEMYDATA *p_data, POKEPARTY *p_party, HEAPID heapID )
{ 
  int i;
  void *p_buff;
  POKEMON_PARAM *p_pp;
  BOOL ret  = TRUE;

  DEBUG_WIFICUP_Printf( "ポケモン署名開始　全部で[%d]匹\n", PokeParty_GetPokeCount( p_party ) );
  OS_TFPrintf( 3, "相手のポケモン署名チェック開始\n" );

  p_buff  = NHTTP_RAP_EVILCHECK_CreateVerifyPokeBuffer( POKETOOL_GetWorkSize(), PokeParty_GetPokeCount(p_party), GFL_HEAP_LOWID(heapID) );

  for( i = 0; i < PokeParty_GetPokeCount( p_party ); i++ )
  { 
    p_pp  = PokeParty_GetMemberPointer( p_party, i);

    NHTTP_RAP_EVILCHECK_AddPokeVerifyPokeBuffer( p_buff, p_pp, POKETOOL_GetWorkSize(), i );
  }

  ret = NHTTP_RAP_EVILCHECK_VerifySign( p_buff, POKETOOL_GetWorkSize(), PokeParty_GetPokeCount(p_party), p_data->sign, GFL_HEAP_LOWID(heapID) );

  NHTTP_RAP_EVILCHECK_DeleteVerifyPokeBuffer( p_buff );

  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ポケモンパーティの一致
 *
 *	@param	const POKEPARTY *cp_pokeparty_1 比較A
 *	@param	POKEPARTY *cp_pokeparty_2       比較B
 *
 *	@return TRUE一致  FALSE不一致
 */
//-----------------------------------------------------------------------------
static BOOL Util_ComarepPokeParty( const POKEPARTY *cp_pokeparty_1, const POKEPARTY *cp_pokeparty_2 )
{ 
  const u32 max1  = PokeParty_GetPokeCount(cp_pokeparty_1);
  const u32 max2  = PokeParty_GetPokeCount(cp_pokeparty_2 );
  if( max1 == max2 )
  { 
    int i;
    POKEMON_PARAM *p_pp1;
    POKEMON_PARAM *p_pp2;
    POKEMON_PASO_PARAM  *p_ppp1;
    POKEMON_PASO_PARAM  *p_ppp2;
    for( i = 0; i < max1; i++ )
    { 
      p_pp1 = PokeParty_GetMemberPointer( cp_pokeparty_1, i );
      p_pp2 = PokeParty_GetMemberPointer( cp_pokeparty_2, i );

      p_ppp1  = PP_GetPPPPointer( p_pp1 );
      p_ppp2  = PP_GetPPPPointer( p_pp2 );

      if( GFL_STD_MemComp( p_ppp1, p_ppp2, POKETOOL_GetPPPWorkSize() ) != 0 )
      {
        DEBUG_WIFICUP_Printf( "Cnt%d Failed !!!\n", i );
        return FALSE;
      }
    }
    return TRUE;
  }

  DEBUG_WIFICUP_Printf( "%d != %d !!!\n", max1, max2 );
  
  return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  キャンセル処理
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 *	@param	is_cancel_enable TRUEならばキャンセルをおこなってよい FALSEならばダメ
 *	@param  can_disconnect  切断してよいのならばTRUE
 *
 *	@return キャンセル状態
 */
//-----------------------------------------------------------------------------
static UTIL_CANCEL_STATE Util_Cancel_Seq( WIFIBATTLEMATCH_WIFI_WORK *p_wk, BOOL is_cancel_enable, BOOL can_disconnect )
{
  enum
  {
    SEQ_CHECK_CANCEL,
    SEQ_START_CANCEL_MSG,
    SEQ_WAIT_CANCEL_MSG,
    SEQ_START_LIST,
    SEQ_WAIT_LIST,

    SEQ_START_DISCONNECT_MSG,
    SEQ_SEND_CANCEL,
    SEQ_END,
  };

  switch( p_wk->cancel_seq )
  {
  case SEQ_CHECK_CANCEL:
    if( is_cancel_enable && (GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL) )
    {
      p_wk->cancel_seq++;
    }
    break;
  case SEQ_START_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_011, WBM_TEXT_TYPE_STREAM );
    p_wk->cancel_seq++;
    break;
  case SEQ_WAIT_CANCEL_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    {
      p_wk->cancel_seq++;
    }
    break;
  case SEQ_START_LIST:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    p_wk->cancel_seq++;
    break;
  case SEQ_WAIT_LIST:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0:
          p_wk->cancel_seq  = SEQ_START_DISCONNECT_MSG;
          break;
        case 1:
          WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_009, WBM_TEXT_TYPE_QUE );
          p_wk->cancel_seq  = 0;
          return UTIL_CANCEL_STATE_EXIT;
        }
      }
    }
    break;
  case SEQ_START_DISCONNECT_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_020, WBM_TEXT_TYPE_WAIT );
    p_wk->cancel_seq++;
    break;
  case SEQ_SEND_CANCEL:
    if( can_disconnect )
    {
      WIFIBATTLEMATCH_NET_SendMatchCancel( p_wk->p_net );

      {
        p_wk->cancel_seq++;
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );

#ifdef BUGFIX_BTS7867_20100716
        if( NET_ERR_CHECK_NONE == NetErr_App_CheckError()
            && GFL_NET_IsInit() )
#endif //BUGFIX_BTS7867_20100716
        {
          DWC_CloseAllConnectionsHard();
        }
      }
    }
    break;
  case SEQ_END:
    p_wk->cancel_seq  = 0;
    return UTIL_CANCEL_STATE_DECIDE;
  }

  if( p_wk->cancel_seq == 0 )
  {
    return UTIL_CANCEL_STATE_NONE;
  }
  else
  {
    return UTIL_CANCEL_STATE_WAIT;
  }
}

#ifdef BUGFIX_BTS7769_20100713
//----------------------------------------------------------------------------
/**
 *	@brief  キャンセルシーケンスをリセット
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_Cancel_ResetSeq( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{
  p_wk->cancel_seq  = 0;
  Util_List_Delete( p_wk );
}
#endif //BUGFIX_BTS7769_20100713

//----------------------------------------------------------------------------
/**
 *	@brief  スコアセーブ
 *
 *	@param	WIFIBATTLEMATCH_WIFI_WORK *p_wk ワーク 
 *
 *	@return TRUEで変更あり  FALSEで変更なし
 */
//-----------------------------------------------------------------------------
static BOOL Util_SaveScore( WIFIBATTLEMATCH_WIFI_WORK *p_wk, const WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA *cp_data )
{
  BOOL ret;

  if( RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_RATE ) == cp_data->rate
  && RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_WIN ) == cp_data->win
  && RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_LOSE ) == cp_data->lose )
  {
    ret = FALSE;
  }
  else
  {
    ret = TRUE;
  }


  RNDMATCH_SetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_RATE, cp_data->rate );
  RNDMATCH_SetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_WIN, cp_data->win );
  RNDMATCH_SetParam( p_wk->p_param->p_rndmatch, RNDMATCH_TYPE_WIFI_CUP, RNDMATCH_PARAM_IDX_LOSE, cp_data->lose );


  return ret;
}

//=============================================================================
/**
 *  デバッグ
 */
//=============================================================================

#ifdef DEBUGWIN_USE

static void DEBUGWIN_Init( WIFIBATTLEMATCH_WIFI_WORK *p_wk, HEAPID heapID )
{ 
  DEBUGWIN_InitProc( GFL_BG_FRAME0_M, p_wk->p_font );
  DEBUGWIN_ChangeLetterColor( 0,31,0 );

  DEBUGWIN_REG_Init( p_wk->p_reg, heapID );
  DEBUGWIN_WIFISCORE_Init( heapID );
  DEBUGWIN_SAKERECORD_Init( p_wk->p_param->p_record_data, heapID );
  DEBUGWIN_REPORT_Init( heapID );
  DEBUGWIN_BTLBOX_Init( heapID );
  DEBUGWIN_BTLBGM_Init( heapID );
  DEBUGWIN_ATLAS_Init( heapID );
  DEBUGWIN_ETC_InitEx( heapID, (POKEPARTY*)p_wk->p_param->p_player_data->pokeparty );
}
static void DEBUGWIN_Exit( WIFIBATTLEMATCH_WIFI_WORK *p_wk )
{
  DEBUGWIN_ETC_Exit();
  DEBUGWIN_ATLAS_Exit();
  DEBUGWIN_BTLBGM_Exit();
  DEBUGWIN_BTLBOX_Exit();
  DEBUGWIN_REPORT_Exit();
  DEBUGWIN_SAKERECORD_Exit();
  DEBUGWIN_WIFISCORE_Exit();
  DEBUGWIN_REG_Exit();

  DEBUGWIN_ExitProc();
}
#endif
