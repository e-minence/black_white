//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_core.c
 *	@brief	WIFIのバトルマッチコア画面  ランダムマッチシーケンス
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
#include "sound/pm_sndsys.h"
#include "system/gf_date.h"
#include "pokeicon/pokeicon.h"
#include "gamesystem/game_data.h"
#include "system/net_err.h"
#include "battle/btl_net.h" //BTL_NET_SERVER_VERSION
#include "net/dwc_rap.h"
#include "net/dwc_rapcommon.h"

//	アーカイブ
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "msg/msg_wifi_match.h"

//	文字表示
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"

//  セーブデータ
#include "savedata/dreamworld_data.h"
#include "savedata/my_pms_data.h"
#include "savedata/etc_save.h"

//WIFIバトルマッチのモジュール
#include "wifibattlematch_graphic.h"
#include "wifibattlematch_view.h"
#include "wifibattlematch_net.h"
#include "wifibattlematch_util.h"
#include "wifibattlematch_snd.h"

//外部公開
#include "wifibattlematch_core.h"
#include "bugfix.h"

#include "debug/debug_flg.h"
#include "debug/debug_nagihashi.h"

//-------------------------------------
///	オーバーレイ
//=====================================
FS_EXTERN_OVERLAY(ui_common);
FS_EXTERN_OVERLAY(dpw_common);

//-------------------------------------
///	デバッグマクロ
//=====================================
#ifdef PM_DEBUG
#define DEBUGWIN_USE

//#if defined(DEBUG_ONLY_FOR_toru_nagihashi)||defined(DEBUG_ONLY_FOR_shimoyamada)
//#define DEBUG_GPF_PASS
//#endif
//#define DEBUG_DIRTYCHECK_PASS

#define DEBUG_SC_HEAP //SCのヒープ開放を監視する

#define DEBUG_FLAG_GPF_PASS_ON    //GPFをスルーするフラグの使用


#if defined(DEBUG_ONLY_FOR_toru_nagihashi)
//#define DEBUG_NONE_MATCHING
#endif 

#endif //PM_DEBUG

#define SCREPORT_TIMEOUT_IS_DISCONNECT

//SCヒープ開放を監視
#ifdef DEBUG_SC_HEAP
static int sc_heap_cnt  = 0;
#define DEBUG_SC_HEAP_Increment {NAGI_Printf( "sc_heap cnt=%d\n", ++sc_heap_cnt );}  
#define DEBUG_SC_HEAP_Decrement {NAGI_Printf( "sc_heap cnt=%d\n", --sc_heap_cnt );}  
#else   //DEBUG_SC_HEAP
#define DEBUG_SC_HEAP_Increment /*  */
#define DEBUG_SC_HEAP_Decrement /*  */
#endif  //DEBUG_SC_HEAP

//デバッグWINインクルード
#ifdef DEBUGWIN_USE
//デバッグ
#include "debug/debugwin_sys.h"
#include "wifibattlematch_debug.h"


#define DEBUGWIN_GROUP_CARD (20)
#define DEBUGWIN_GROUP_SCORE (30)

#endif//DEBUGWIN_USE

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	シンク
//=====================================
#define ENEMYDATA_WAIT_SYNC    (60*5)
#define ENEMYDATA_WAIT_SYNC_RATE    (0)
#define MATCHING_MSG_WAIT_SYNC (120)
#define SELECTPOKE_MSG_WAIT_SYNC (60)

#define MATCHING_TIMEOUT_SYNC   (60*40)

//-------------------------------------
///	ヒープサイズ
//=====================================
#define WBM_RND_RATE_HEAP_SIZE  (0x30000)

//-------------------------------------
///	サブシーケンス戻り値
//=====================================
typedef enum
{
  WBM_RND_SUBSEQ_EVILCHECK_RET_SUCCESS  = 0,  //成功
  WBM_RND_SUBSEQ_EVILCHECK_RET_DARTY,         //１体でも不正ポケモンがいた
  WBM_RND_SUBSEQ_EVILCHECK_RET_NET_ERR,       //ネットエラー
  WBM_RND_SUBSEQ_EVILCHECK_RET_SERVER_ERR,    //サーバーレスポンスエラー

  WBM_RND_SUBSEQ_RET_NONE  = 0xFFFF,          //なし
} WBM_RND_SUBSEQ_RET;


//=============================================================================
/**
 *        定義
*/
//=============================================================================
//-------------------------------------
///	メインワーク前方宣言
//=====================================
typedef struct
{
	//グラフィック設定
	WIFIBATTLEMATCH_GRAPHIC_WORK	*p_graphic;

  //リソース
  WIFIBATTLEMATCH_VIEW_RESOURCE *p_res;

	//共通で使うフォント
	GFL_FONT			                *p_font;

	//共通で使うキュー
	PRINT_QUE				              *p_que;

	//共通で使うメッセージ
	GFL_MSGDATA	          	    	*p_msg;

	//共通で使う単語
	WORDSET				              	*p_word;

	//上画面情報
	PLAYERINFO_WORK             	*p_playerinfo;

	//対戦者情報
	MATCHINFO_WORK              	*p_matchinfo;

  //待機アイコン
  WBM_WAITICON_WORK             *p_wait;

  //テキスト面
  WBM_TEXT_WORK                 *p_text;

  //リスト
  WBM_LIST_WORK                 *p_list;
  u32                           list_type;

  //ネット
  WIFIBATTLEMATCH_NET_WORK      *p_net;

  //メインシーケンス
  WBM_SEQ_WORK                  *p_seq;

  //サブシーケンス
  WBM_SEQ_WORK                  *p_subseq;
  WBM_RND_SUBSEQ_RET            subseq_ret;

  //引数
  WIFIBATTLEMATCH_CORE_PARAM    *p_param;

  //不正チェックのときに使用するバッファ（それ以外は参照しない）
  WIFIBATTLEMATCH_NET_EVILCHECK_DATA  evilecheck_data;

  //ウエイト
  u32 cnt;

  //キャンセルシーケンス
  u32 cancel_seq;
  u32 match_timeout;
  BOOL cancel_req;

  //選手証更新
  BOOL  is_card_update;

  BOOL is_send_report;

#ifdef BUGFIX_BTS7762_20100713
  BOOL  is_sc_before_err;
#endif //BUGFIX_BTS7762_20100713

  WIFIBATTLEMATCH_NET_SC_STATE  sc_state;

#ifdef DEBUGWIN_USE
  u32 playerinfo_mode;
  u32 matchinfo_mode;
  u32 playerinfo_rank;
  u32 matchinfo_rank;
  u32 waiticon_draw;
#endif //DEBUGWIN_USE

} WIFIBATTLEMATCH_RND_WORK;

//=============================================================================
/**
 *					プロトタイプ
*/
//=============================================================================
//-------------------------------------
///	プロセス
//=====================================
static GFL_PROC_RESULT WIFIBATTLEMATCH_RND_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_RND_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_RND_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	ランダムマッチシーケンス関数
//=====================================
static void WbmRndSeq_Init( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//レーティング処理
static void WbmRndSeq_Rate_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_StartMatching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_EndRec( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_CupContinue( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Rate_DisConnextSendTime( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//フリー処理
static void WbmRndSeq_Free_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Free_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Free_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Free_EndRec( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Free_CupContinue( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmRndSeq_Free_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//エラー処理
static void WbmRndSeq_Err_ReturnLogin( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
//切断
static void WbmRndSeq_DisConnectEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	サブシーケンス関数
//   Util_SubSeq_Start等を使い、シーケンス上で動くもの
//=====================================
static void WbmRndSubSeq_EvilCheck( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	便利
//=====================================
//自分のカード作成
static void Util_PlayerInfo_Create( WIFIBATTLEMATCH_RND_WORK *p_wk, WIFIBATTLEMATCH_CORE_RETMODE mode );
static void Util_PlayerInfo_Delete( WIFIBATTLEMATCH_RND_WORK *p_wk );
static BOOL Util_PlayerInfo_Move( WIFIBATTLEMATCH_RND_WORK *p_wk );
static BOOL Util_PlayerInfo_MoveOut( WIFIBATTLEMATCH_RND_WORK *p_wk );
static void Util_PlayerInfo_CreateStay( WIFIBATTLEMATCH_RND_WORK *p_wk, WIFIBATTLEMATCH_CORE_RETMODE mode );
static void Util_PlayerInfo_DeleteOut( WIFIBATTLEMATCH_RND_WORK *p_wk );
//対戦相手のカード作成
static void Util_MatchInfo_Create( WIFIBATTLEMATCH_RND_WORK *p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_enemy_data );
static void Util_MatchInfo_Delete( WIFIBATTLEMATCH_RND_WORK *p_wk );
static BOOL Util_MatchInfo_Main( WIFIBATTLEMATCH_RND_WORK *p_wk );
static void Util_Matchinfo_Clear( WIFIBATTLEMATCH_RND_WORK *p_wk );
//スコアセーブ
static BOOL Util_SaveRateScore( RNDMATCH_DATA *p_save, WIFIBATTLEMATCH_BTLRULE btl_rule, const WIFIBATTLEMATCH_RND_WORK *cp_wk );
static void Util_SaveFreeScore( RNDMATCH_DATA *p_rndmatch, WIFIBATTLEMATCH_BTLRULE btl_rule, BtlResult btl_result );
//選択肢作成
typedef enum
{ 
  UTIL_LIST_TYPE_YESNO,
  UTIL_LIST_TYPE_FREERATE,
}UTIL_LIST_TYPE;
static void Util_List_Create( WIFIBATTLEMATCH_RND_WORK *p_wk, UTIL_LIST_TYPE type );
static void Util_List_Delete( WIFIBATTLEMATCH_RND_WORK *p_wk );
static u32 Util_List_Main( WIFIBATTLEMATCH_RND_WORK *p_wk );
//マッチング用データ作成
static void Util_Matchkey_SetData( WIFIBATTLEMATCH_MATCH_KEY_DATA *p_data, const WIFIBATTLEMATCH_RND_WORK *cp_wk );
//自分のデータ作成
static void Util_InitMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, WIFIBATTLEMATCH_RND_WORK *p_wk );
static void Util_RenewalMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, WIFIBATTLEMATCH_RND_WORK *p_wk );
//サブシーケンス
static void Util_SubSeq_Start( WIFIBATTLEMATCH_RND_WORK *p_wk, WBM_SEQ_FUNCTION seq_function );
static WBM_RND_SUBSEQ_RET Util_SubSeq_Main( WIFIBATTLEMATCH_RND_WORK *p_wk );
//ポケモン証明
static BOOL Util_VerifyPokeData( WIFIBATTLEMATCH_ENEMYDATA *p_data, HEAPID heapID );
//キャンセルシーケンス
typedef enum
{
  UTIL_CANCEL_STATE_NONE, //何もしていない
  UTIL_CANCEL_STATE_WAIT, //キャンセル待機中（はい、いいえ待ち等）
  UTIL_CANCEL_STATE_EXIT, //待機処理を抜けた
  UTIL_CANCEL_STATE_DECIDE, //キャンセルを確定
}UTIL_CANCEL_STATE;
static UTIL_CANCEL_STATE Util_Cancel_Seq( WIFIBATTLEMATCH_RND_WORK *p_wk, BOOL is_cancel, BOOL can_disconnect );

#ifdef BUGFIX_BTS7769_20100713
static void Util_Cancel_ResetSeq( WIFIBATTLEMATCH_RND_WORK *p_wk );
#endif //BUGFIX_BTS7769_20100713

//-------------------------------------
///	デバッグ
//=====================================
#ifdef DEBUGWIN_USE
static void DEBUGWIN_CARD_Init( WIFIBATTLEMATCH_RND_WORK *p_wk );
static void DEBUGWIN_CARD_Exit( WIFIBATTLEMATCH_RND_WORK *p_wk );
#endif //DEBUGWIN_USE
//=============================================================================
/**
 *					外部参照
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	WifiBattleMatchRnd_ProcData =
{	
	WIFIBATTLEMATCH_RND_PROC_Init,
	WIFIBATTLEMATCH_RND_PROC_Main,
	WIFIBATTLEMATCH_RND_PROC_Exit,
};

//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFIバトルマッチランダム対戦	コアプロセス初期化
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_RND_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	WIFIBATTLEMATCH_RND_WORK	*p_wk;
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;


	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common));

	//ヒープ作成
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_CORE, 0x35000 );

	//プロセスワーク作成
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFIBATTLEMATCH_RND_WORK), HEAPID_WIFIBATTLEMATCH_CORE );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_RND_WORK) );	
  p_wk->p_param = p_param;

	//グラフィック設定
	p_wk->p_graphic	= WIFIBATTLEMATCH_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_WIFIBATTLEMATCH_CORE );

  //リソース読み込み
  p_wk->p_res     = WIFIBATTLEMATCH_VIEW_LoadResource( WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic ), WIFIBATTLEMATCH_VIEW_RES_MODE_RANDOM, HEAPID_WIFIBATTLEMATCH_CORE );

  //自分のデータを設定
  Util_InitMyData( p_param->p_player_data, p_wk );

	//共通モジュールの作成
	p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
												NARC_message_wifi_match_dat, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_word	= WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_COUNTRY_BUFLEN, HEAPID_WIFIBATTLEMATCH_CORE );

	//モジュールの作成
  p_wk->p_net   = WIFIBATTLEMATCH_NET_Init( p_wk->p_param->p_net_data,
      p_param->p_param->p_game_data, p_param->p_svl_result, HEAPID_WIFIBATTLEMATCH_CORE );
  p_wk->p_text  = WBM_TEXT_Init( BG_FRAME_M_TEXT, PLT_FONT_M, PLT_TEXT_M, CGR_OFS_M_TEXT, p_wk->p_que, p_wk->p_font, HEAPID_WIFIBATTLEMATCH_CORE );
  p_wk->p_seq   = WBM_SEQ_Init( p_wk, WbmRndSeq_Init, HEAPID_WIFIBATTLEMATCH_CORE );
  p_wk->p_subseq   = WBM_SEQ_Init( p_wk, NULL, HEAPID_WIFIBATTLEMATCH_CORE );

  { 
    GFL_CLUNIT  *p_unit;
    p_unit  = WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    p_wk->p_wait  = WBM_WAITICON_Init( p_unit, p_wk->p_res, HEAPID_WIFIBATTLEMATCH_CORE );
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
	}


  //戦闘後や録画後の場合は既にプレイヤー情報を呼んだ状態にする
  if( p_param->mode != WIFIBATTLEMATCH_CORE_MODE_START )
  { 
    if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      Util_PlayerInfo_CreateStay( p_wk, WIFIBATTLEMATCH_CORE_RETMODE_RATE );
    }
    else if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_FREE )
    { 
      if( p_param->mode == WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE )
      { 
        Util_SaveFreeScore( p_param->p_rndmatch, p_param->p_param->btl_rule, p_param->cp_btl_score->result );
      }
      Util_PlayerInfo_CreateStay( p_wk, WIFIBATTLEMATCH_CORE_RETMODE_FREE );
    }
  }

#ifdef DEBUGWIN_USE
  DEBUGWIN_InitProc( GFL_BG_FRAME0_M, p_wk->p_font );
  DEBUGWIN_ChangeLetterColor( 0,31,0 );
  DEBUGWIN_WIFISCORE_Init( HEAPID_WIFIBATTLEMATCH_CORE );
  DEBUGWIN_REPORT_Init( HEAPID_WIFIBATTLEMATCH_CORE );
  DEBUGWIN_SAKERECORD_Init( p_wk->p_param->p_record_data, HEAPID_WIFIBATTLEMATCH_CORE );
  DEBUGWIN_BTLBGM_Init( HEAPID_WIFIBATTLEMATCH_CORE );
  DEBUGWIN_ATLAS_Init( HEAPID_WIFIBATTLEMATCH_CORE );
  DEBUGWIN_ETC_InitEx( HEAPID_WIFIBATTLEMATCH_CORE, (POKEPARTY *)p_param->p_player_data->pokeparty );
#endif

  WIFIBATTLEMATCH_NETICON_SetDraw( p_wk->p_net, TRUE );
	
	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFIバトルマッチランダム対戦	コアプロセス破棄
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_RND_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
	WIFIBATTLEMATCH_RND_WORK	      *p_wk	= p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_param_adrs;

  WIFIBATTLEMATCH_NETICON_SetDraw( p_wk->p_net, FALSE );

#ifdef DEBUGWIN_USE
  DEBUGWIN_ETC_Exit();
  DEBUGWIN_ATLAS_Exit();
  DEBUGWIN_BTLBGM_Exit();
  DEBUGWIN_SAKERECORD_Exit();
  DEBUGWIN_REPORT_Exit( );
  DEBUGWIN_WIFISCORE_Exit();
  DEBUGWIN_ExitProc();
#endif

	//モジュールの破棄
  WBM_WAITICON_Exit( p_wk->p_wait );
  WBM_SEQ_Exit( p_wk->p_subseq );
  WBM_SEQ_Exit( p_wk->p_seq );
  WBM_TEXT_Exit( p_wk->p_text );
  if( p_wk->p_net )
  { 
    WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );
  }
  Util_MatchInfo_Delete( p_wk );
  Util_PlayerInfo_Delete( p_wk );
  Util_List_Delete( p_wk );

	//共通モジュールの破棄
	WORDSET_Delete( p_wk->p_word );
	GFL_MSG_Delete( p_wk->p_msg );
	PRINTSYS_QUE_Delete( p_wk->p_que );
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
static GFL_PROC_RESULT WIFIBATTLEMATCH_RND_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_MAIN,
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
  };


	WIFIBATTLEMATCH_RND_WORK	*p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;

#ifdef PM_DEBUG
#if 0
  { 
    static u32  s_core_buff = 0xFFFFFFFF;
    static u32  s_sys_buff = 0xFFFFFFFF;
    static u32  s_app_buff = 0xFFFFFFFF;
    DEBUG_HEAP_PrintRestUse2( HEAPID_WIFIBATTLEMATCH_CORE, &s_core_buff );
    DEBUG_HEAP_PrintRestUse2( HEAPID_WIFIBATTLEMATCH_SYS, &s_sys_buff );
    DEBUG_HEAP_PrintRestUse2( GFL_HEAPID_APP, &s_app_buff );
  }
#endif
#endif

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


    if( p_wk->p_net )
    { 
      WIFIBATTLEMATCH_NET_Main( p_wk->p_net );
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

  //文字表示
  if( p_wk->p_playerinfo )
  { 
    PLAYERINFO_PrintMain( p_wk->p_playerinfo, p_wk->p_que );
  }
  if( p_wk->p_matchinfo )
  { 
    MATCHINFO_PrintMain( p_wk->p_matchinfo, p_wk->p_que );
  }

  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *  シーケンス
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ランダムマッチ初期化
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Init( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_RND_WORK	      *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  NAGI_Printf( " mode%d ret%d\n", p_param->mode, p_param->retmode );
  switch( p_param->mode )
  { 
  case WIFIBATTLEMATCH_CORE_MODE_START:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
    break;

  case WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR:
    WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
    /* fallthrough */

  case WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE:
    if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_EndBattle );
    }
    else if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_FREE )
    { 
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_EndBattle );
    }
    break;

  case WIFIBATTLEMATCH_CORE_MODE_ENDREC:
    if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_EndRec );
    }
    else if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_FREE )
    { 
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_EndRec );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ランダムマッチ開始
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ワーク
 *	@param	WBM_SEQ_WORK *p_wk          シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG_INIT,

    SEQ_SELECT_MSG,
    SEQ_START_SELECT_MODE,
    SEQ_WAIT_SELECT_MODE,

    SEQ_START_CANCEL_MSG,
    SEQ_START_CANCEL_SELECT,
    SEQ_WAIT_CANCEL_SELECT,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	      *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_MSG_INIT:
    //エラーで戻ってくる場合、アイコンを消す
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
    *p_seq = SEQ_SELECT_MSG;
    break;

    //-------------------------------------
    ///	モード選択
    //=====================================
  case SEQ_SELECT_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_001, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SELECT_MODE );
    break;

  case SEQ_START_SELECT_MODE:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_FREERATE );
    *p_seq     = SEQ_WAIT_SELECT_MODE;
    break;

  case SEQ_WAIT_SELECT_MODE:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0:
          p_param->retmode = WIFIBATTLEMATCH_CORE_RETMODE_FREE;
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_Start );
          break;
        case 1:
          p_param->retmode = WIFIBATTLEMATCH_CORE_RETMODE_RATE;
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_Start );
          break;
        case 2:
          p_param->retmode = WIFIBATTLEMATCH_CORE_RETMODE_NONE;
          *p_seq = SEQ_START_CANCEL_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	やめる
    //=====================================
  case SEQ_START_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_006, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_CANCEL_SELECT );
    break;

  case SEQ_START_CANCEL_SELECT:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq     = SEQ_WAIT_CANCEL_SELECT;
    break;

  case SEQ_WAIT_CANCEL_SELECT:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0:
          p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_DisConnectEnd );
          break;

        case 1:
          *p_seq = SEQ_SELECT_MSG;
          break;
        }
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
  };
}
//----------------------------------------------------------------------------
/**
 *	@brief  ランダムマッチ  レーティング開始
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK_GAMESYNC_REG,

    SEQ_START_RATE_MSG,
    SEQ_START_RECV_MSG,

    //GPF分岐
    SEQ_START_RECV_GPF,
    SEQ_WAIT_RECV_GPF,
    SEQ_CHECK_GPF,

    SEQ_START_NOSERVER_MSG,

    SEQ_WAIT_RECVRATE_SAKE,
    SEQ_WAIT_CARDIN,
    SEQ_START_POKECHECK_SERVER,
    SEQ_WAIT_POKECHECK_SERVER,
    SEQ_START_DIRTY_MSG,
    SEQ_DIRTY_END,
    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	      *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECK_GAMESYNC_REG:
    {
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      DREAMWORLD_SAVEDATA* p_dream  = DREAMWORLD_SV_GetDreamWorldSaveData( p_sv );
      if( DREAMWORLD_SV_GetAccount(p_dream) )
      {
        *p_seq  = SEQ_START_RATE_MSG;
      }
      else
      {
        *p_seq  = SEQ_START_NOSERVER_MSG;
      }
    }
    break;

  case SEQ_START_RATE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_018, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_RECV_MSG );
    break;


  case SEQ_START_RECV_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_RECV_GPF );
    break;


  case SEQ_START_RECV_GPF:
#ifdef DEBUG_GPF_PASS
    { 
      *p_seq  = SEQ_START_POKECHECK_SERVER;
      return;
    }
#endif

    GFL_STD_MemClear( p_wk->p_param->p_gpf_data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA) );
    WIFIBATTLEMATCH_NET_StartRecvGpfData( p_wk->p_net, HEAPID_WIFIBATTLEMATCH_CORE );
    *p_seq = SEQ_WAIT_RECV_GPF;
    break;

  case SEQ_WAIT_RECV_GPF:
    switch( WIFIBATTLEMATCH_NET_WaitRecvGpfData( p_wk->p_net ) )
    { 
    case WIFIBATTLEMATCH_RECV_GPFDATA_RET_UPDATE:
      break;
    case WIFIBATTLEMATCH_RECV_GPFDATA_RET_SUCCESS:
      WIFIBATTLEMATCH_NET_GetRecvGpfData( p_wk->p_net, p_wk->p_param->p_gpf_data );
      *p_seq = SEQ_CHECK_GPF;
      break;
    case WIFIBATTLEMATCH_RECV_GPFDATA_RET_DIRTY:
      //もらえなかったので、クリア
      GFL_STD_MemClear( p_wk->p_param->p_gpf_data, sizeof(DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA) );
       WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      break;
    case WIFIBATTLEMATCH_RECV_GPFDATA_RET_ERROR:
      //エラー処理
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT: //切断しログインからやり直し
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
        break;
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:     //復帰地点まで戻る
        Util_PlayerInfo_DeleteOut( p_wk );
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
        break;
      }
      break;
    }
    break;

  case SEQ_CHECK_GPF:

#ifdef DEBUG_FLAG_GPF_PASS_ON
    if( DEBUG_FLG_GetFlg( DEBUG_FLG_WifiMatchRateFlag ) )
    { 
      *p_seq  = SEQ_START_POKECHECK_SERVER;
      return;
    }
#endif//DEBUG_FLAG_GPF_PASS_ON

    { 
      if( p_wk->p_param->p_gpf_data->signin )
      { 
        *p_seq  = SEQ_START_POKECHECK_SERVER;
      }
      else
      {
        *p_seq  = SEQ_START_NOSERVER_MSG;
      }
    }
    break;

  case SEQ_START_NOSERVER_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_05, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    break;

    //-------------------------------------
    /// ポケモン不正チェック
    //=====================================
  case SEQ_START_POKECHECK_SERVER:
    Util_SubSeq_Start( p_wk, WbmRndSubSeq_EvilCheck );
    *p_seq       = SEQ_WAIT_POKECHECK_SERVER;

    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      Util_PlayerInfo_DeleteOut( p_wk );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      break;
    }

    break;

  case SEQ_WAIT_POKECHECK_SERVER:
    { 
      WBM_RND_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_RND_SUBSEQ_EVILCHECK_RET_SUCCESS )
      { 
        *p_seq       = SEQ_END;
      }
      else if( ret == WBM_RND_SUBSEQ_EVILCHECK_RET_DARTY )
      {   
        *p_seq       = SEQ_START_DIRTY_MSG;
      }
      else if( ret == WBM_RND_SUBSEQ_EVILCHECK_RET_SERVER_ERR )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      }
      
      if( ret != WBM_RND_SUBSEQ_RET_NONE )
      {
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
          Util_PlayerInfo_DeleteOut( p_wk );
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;
    
  case SEQ_START_DIRTY_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_ERR_00, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    break;

  case SEQ_DIRTY_END:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_DisConnectEnd );
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_StartMatching );
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
 *	@brief  ランダムマッチレーティング    マッチング前のSAKEサーバー受信
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_StartMatching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_RECVRATE_MSG,
    SEQ_START_RECVRATE_SAKE,
    SEQ_WAIT_RECVRATE_SAKE,

    SEQ_START_CREATE_SAKE,
    SEQ_WAIT_CREATE_SAKE,

    SEQ_CHECK_CARD,
    SEQ_START_CARDIN_OUT,
    SEQ_WAIT_CARDIN_OUT,

    SEQ_START_CARDIN,
    SEQ_WAIT_CARDIN,
    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	      *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;


  switch( *p_seq )
  { 
    //-------------------------------------
    ///	レーティングデータ受信
    //=====================================
  case SEQ_START_RECVRATE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_RECVRATE_SAKE );
    *p_seq       = SEQ_WAIT_MSG;
    break;

  case SEQ_START_RECVRATE_SAKE:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, WIFIBATTLEMATCH_GDB_MYRECORD, WIFIBATTLEMATCH_GDB_GET_RND_SCORE, p_wk->p_param->p_rnd_sake_data );
      *p_seq       = SEQ_WAIT_RECVRATE_SAKE;
    }
    OS_TPrintf( "サーバー待ち%d\n",  *p_wk->p_param->p_server_time );
    break;
    
  case SEQ_WAIT_RECVRATE_SAKE:
    { 
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_GDB_Process( p_wk->p_net );

      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      { 

        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;

        //※１受信してきたデータをセーブワークに入れる
        p_wk->is_card_update  = Util_SaveRateScore( p_param->p_rndmatch, p_param->p_param->btl_rule, p_wk );
        Util_RenewalMyData( p_param->p_player_data, p_wk );

        //消したくない情報を常駐に保存
        p_wk->p_param->p_recv_data->record_save_idx = p_wk->p_param->p_rnd_sake_data->record_save_idx;

        *p_seq       = SEQ_START_CREATE_SAKE;
      }
      
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      { 
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
          Util_PlayerInfo_DeleteOut( p_wk );
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );

          *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
          *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	レコードIDがない場合のレコード作成処理
    //=====================================
  case SEQ_START_CREATE_SAKE:
    if( WIFIBATTLEMATCH_GDB_GetRecordID( p_wk->p_net ) == 0 )
    {
      WIFIBATTLEMATCH_GDB_StartCreateRecord( p_wk->p_net );
      *p_seq      = SEQ_WAIT_CREATE_SAKE;
    }
    else
    {
      *p_seq       = SEQ_CHECK_CARD;
    }
    break;

  case SEQ_WAIT_CREATE_SAKE:
    {
      WIFIBATTLEMATCH_GDB_RESULT  res = WIFIBATTLEMATCH_GDB_ProcessCreateRecord( p_wk->p_net );
      if( res == WIFIBATTLEMATCH_GDB_RESULT_SUCCESS )
      {
        //もう一度受信しに行く
        *p_seq  = SEQ_START_RECVRATE_SAKE;
      }
      
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      {
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
          Util_PlayerInfo_DeleteOut( p_wk );
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );

          *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
          *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	自分のカード処理
    //=====================================
  case SEQ_CHECK_CARD:
    //作成されていなかったら出てくるだけ
    if( p_wk->p_playerinfo == NULL )
    {
      *p_seq       = SEQ_START_CARDIN;
    }
    else
    {
      //作成されて更新されていたら、出て行き入ってくる
      if( p_wk->is_card_update )
      {
        *p_seq       = SEQ_START_CARDIN_OUT;
      }
      //作成されて更新されていなかったら何もしない
      else
      {
        *p_seq        = SEQ_END;
      }
    }
    p_wk->is_card_update  = FALSE;
    break;
  case SEQ_START_CARDIN_OUT:
    *p_seq       = SEQ_WAIT_CARDIN_OUT;
    break;
  case SEQ_WAIT_CARDIN_OUT:
    if( Util_PlayerInfo_MoveOut( p_wk ) )
    {
      Util_PlayerInfo_Delete( p_wk );
      *p_seq       = SEQ_START_CARDIN;
    }
    break;
  case SEQ_START_CARDIN:
    //自分の情報を表示
    Util_PlayerInfo_Create( p_wk, WIFIBATTLEMATCH_CORE_RETMODE_RATE );
    *p_seq       = SEQ_WAIT_CARDIN;
    break;

  case SEQ_WAIT_CARDIN:
    if( Util_PlayerInfo_Move( p_wk ) )
    { 
      *p_seq       = SEQ_END;
    }
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_Matching );
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
 *	@brief  ランダムマッチ  レーティングのマッチング
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MATCHING_MSG,
    SEQ_START_MATCHING,
    SEQ_WAIT_MATCHING,
    SEQ_START_SENDDATA,
    SEQ_WAIT_SENDDATA,
    SEQ_CHECK_DARTYDATA,
    SEQ_START_BADWORD,
    SEQ_WAIT_BADWORD,

    SEQ_START_CANCEL_TIMING,
    SEQ_WAIT_CANCEL_TIMING,
    SEQ_START_OK_TIMING,
    SEQ_WAIT_OK_TIMING,

    SEQ_OK_MATCHING_MSG,
    SEQ_OK_MATCHING_WAIT,
    SEQ_WAIT_MATCH_CARDIN,
    SEQ_CARDIN_WAIT,

    SEQ_START_SESSION,
    SEQ_WAIT_SESSION,

    SEQ_END_SESSION,
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

    SEQ_START_CANCEL,
    SEQ_END_CANCEL,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	      *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;
  UTIL_CANCEL_STATE cancel_state;
  BOOL is_timeout_enable  = FALSE;

  //-------------------------------------
  ///	キャンセルシーケンス
  //=====================================
  {
    BOOL is_cancel_enable = (SEQ_WAIT_MATCHING <= *p_seq && *p_seq < SEQ_START_CANCEL_TIMING);
    BOOL can_disconnect = !( SEQ_START_BADWORD <= *p_seq && *p_seq <= SEQ_WAIT_BADWORD );
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
    ///	マッチング開始
    //=====================================
  case SEQ_START_MATCHING_MSG:
    p_wk->cancel_seq  = 0;
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, TRUE );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_009, WBM_TEXT_TYPE_QUE );
    *p_seq = SEQ_START_MATCHING;
    break;

  case SEQ_START_MATCHING:
#ifdef BUGFIX_BTS7733_20100712
    //マッチングエラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
      return;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      return ;
    }
#endif //BUGFIX_BTS7733_20100712
    { 
      WIFIBATTLEMATCH_MATCH_KEY_DATA  data;
      Util_Matchkey_SetData( &data, p_wk );
      WIFIBATTLEMATCH_NET_StartMatchMake( p_wk->p_net, WIFIBATTLEMATCH_TYPE_RNDRATE, TRUE, p_param->p_param->btl_rule, &data );
      *p_seq = SEQ_WAIT_MATCHING;
    }
    break;

  case SEQ_WAIT_MATCHING:
    //マッチングエラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );

    #ifdef BUGFIX_BTS7769_20100713
      Util_Cancel_ResetSeq( p_wk );
    #endif //BUGFIX_BTS7769_20100713
      return;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      return ;
    }

    {
      WIFIBATTLEMATCH_NET_MATCHMAKE_STATE state = WIFIBATTLEMATCH_NET_WaitMatchMake( p_wk->p_net );
      //マッチング終了待ち
      if( WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_SUCCESS == state )
      { 
        p_wk->match_timeout = 0;
        *p_seq = SEQ_START_SENDDATA;
      }
      else if( WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_FAILED == state )
      {
        *p_seq = SEQ_START_MATCHING;
      }
    }

    break;
  case SEQ_START_SENDDATA:
    if( cancel_state == UTIL_CANCEL_STATE_NONE )
    { 
      WIFIBATTLEMATCH_ENEMYDATA *p_my_data = p_param->p_player_data;
      if( WIFIBATTLEMATCH_NET_StartEnemyData( p_wk->p_net, p_my_data ) )
      { 
        p_wk->match_timeout = 0;
        *p_seq       = SEQ_WAIT_SENDDATA;
      }
      is_timeout_enable = TRUE;
    }
    break;

  case SEQ_WAIT_SENDDATA:
    {
      WIFIBATTLEMATCH_ENEMYDATA *p_recv;
      if( WIFIBATTLEMATCH_NET_WaitEnemyData( p_wk->p_net, &p_recv ) )
      {   
        u32 dirty;
        WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data = p_param->p_enemy_data;
        GFL_STD_MemCopy( p_recv, p_enemy_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
        dirty = WIFIBATTLEMATCH_DATA_ModifiEnemyData( p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );

        if( dirty == 0 )
        { 
          SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
          WIFI_NEGOTIATION_SAVEDATA* pSV  = WIFI_NEGOTIATION_SV_GetSaveData(p_sv_ctrl);
//          WIFI_NEGOTIATION_SV_SetFriend(pSV, (MYSTATUS*)p_param->p_enemy_data->mystatus );
          //仕様がなくなりました
        }

        p_wk->match_timeout = 0;
        *p_seq  = SEQ_CHECK_DARTYDATA;
      }
      is_timeout_enable = TRUE;
    }
    break;

  case SEQ_CHECK_DARTYDATA:
#ifdef DEBUG_DIRTYCHECK_PASS
    if( 1 )
#else
    if( Util_VerifyPokeData( p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE ) )
#endif
    { 
      //対戦者情報表示
      *p_seq  = SEQ_START_BADWORD;
    }
    else
    { 
      //不正なのでマッチングに戻る
      WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_Matching );
    }
    break;

  case SEQ_START_BADWORD:
    { 
      WIFIBATTLEMATCH_NET_StartBadWord( p_wk->p_net, (MYSTATUS*)p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );
    }
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
#ifdef BUGFIX_BTS7760_20100713
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:       //戻る   
#endif //BUGFIX_BTS7760_20100713
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
#ifdef BUGFIX_BTS7760_20100713
          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
#endif //BUGFIX_BTS7760_20100713
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_CupContinue );

       #ifdef BUGFIX_BTS7769_20100713
          Util_Cancel_ResetSeq( p_wk );
        #endif //BUGFIX_BTS7769_20100713
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
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
#ifdef DEBUG_NONE_MATCHING
    {
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
      *p_seq  = SEQ_START_MATCHING_MSG;
    }
    break;
#endif 
    WIFIBATTLEMATCH_NET_StartTiming( p_wk->p_net, WIFIBATTLEMATCH_NET_TIMINGSYNC_MATHING_OK );
    p_wk->match_timeout = 0;
    *p_seq  = SEQ_WAIT_OK_TIMING;
    break;

  case SEQ_WAIT_OK_TIMING:
    if( WIFIBATTLEMATCH_NET_WaitTiming( p_wk->p_net ) )
    {
      *p_seq  = SEQ_OK_MATCHING_MSG;
    }
    is_timeout_enable = TRUE;
    break;


  case SEQ_OK_MATCHING_MSG:
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
    PMSND_PlaySE( WBM_SND_SE_MATCHING_OK );

    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_010, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_OK_MATCHING_WAIT );
    break;

  case SEQ_OK_MATCHING_WAIT:
    if( p_wk->cnt++ > MATCHING_MSG_WAIT_SYNC )
    { 
      p_wk->cnt      = 0;
      Util_MatchInfo_Create( p_wk, p_param->p_enemy_data );
      GFL_BG_SetVisible( BG_FRAME_M_TEXT, FALSE );
      *p_seq      = SEQ_WAIT_MATCH_CARDIN;
    }
    break;
  case SEQ_WAIT_MATCH_CARDIN:
    if( Util_MatchInfo_Main( p_wk ) )
    { 
      *p_seq  = SEQ_CARDIN_WAIT;
    }
    break;
  case SEQ_CARDIN_WAIT:
    if( p_wk->cnt++ > ENEMYDATA_WAIT_SYNC_RATE )
    { 
      p_wk->cnt      = 0;
      *p_seq      = SEQ_START_SESSION;
    }
    break;

  case SEQ_START_SESSION:
    GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_SC, WBM_SC_HEAP_SIZE );
    DWC_RAPCOMMON_SetSubHeapIDEx( DWC_ALLOCTYPE_GS, WBM_SC_HEAP_SIZE, WBM_SC_BORDER_SIZE, HEAPID_WIFIBATTLEMATCH_SC );
    DWC_RAPCOMMON_SetAutoDeleteSubHeap( HEAPID_WIFIBATTLEMATCH_SC );

    WIFIBATTLEMATCH_SC_StartReport( p_wk->p_net, WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_AFTER, WIFIBATTLEMATCH_TYPE_RNDRATE, 0, NULL, FALSE );
    p_wk->sc_state  = WIFIBATTLEMATCH_NET_SC_STATE_UPDATE;
    p_wk->is_send_report  = FALSE;
    *p_seq  = SEQ_WAIT_SESSION;
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
        *p_seq  = SEQ_END_SESSION;
      }
      
      if( p_wk->sc_state != WIFIBATTLEMATCH_NET_SC_STATE_UPDATE )
      {
        NAGI_Printf( "check!\n" );
        //ここでエラーが起こった場合、レポートを送信していれば切断カウンターがあがってしまうので戦闘後へ、レポートを送信していなければ、録画後へいく
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:     //戻る
          WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
          NAGI_Printf( "check1!\n" );
          /* fallthrough */
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
#ifndef SCREPORT_TIMEOUT_IS_DISCONNECT
          DWC_RAPCOMMON_ResetSubHeapID();

          GFL_BG_SetVisible( BG_FRAME_M_TEXT, TRUE );

          Util_Matchinfo_Clear( p_wk );
          NAGI_Printf( "check2!\n" );
          *p_seq  = SEQ_ERROR_END;
          break;
#endif
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          DWC_RAPCOMMON_ResetSubHeapID();
          NAGI_Printf( "check3!\n" );

          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;

  case SEQ_END_SESSION:
    DWC_RAPCOMMON_ResetSubHeapID();
    *p_seq  = SEQ_END_MATCHING_MSG;
    break;

  case SEQ_END_MATCHING_MSG:
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

    //エラー
  case SEQ_ERROR_END:
    WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
    p_param->mode = WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;
    if( p_wk->is_send_report )
    {
#ifdef BUGFIX_BTS7762_20100713
      p_wk->is_sc_before_err  = TRUE;
#endif //BUGFIX_BTS7762_20100713

      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_EndBattle );
    }
    else
    {
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_EndRec );
    }
    break;

    //-------------------------------------
    ///	キャンセル処理
    //=====================================


    //ランダムマッチをおわりにします
  case SEQ_START_SELECT_END_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_016, WBM_TEXT_TYPE_STREAM );
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
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_DisConnextSendTime );
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
          *p_seq  = SEQ_START_MATCHING_MSG;
          break;
        case 1: //いいえ
          *p_seq  = SEQ_START_SELECT_END_MSG;
          break;
        }
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
    *p_seq  = SEQ_START_MATCHING_MSG;
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
        *p_seq  = SEQ_START_CANCEL;
      }

      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_CupContinue );

      #ifdef BUGFIX_BTS7769_20100713
        Util_Cancel_ResetSeq( p_wk );
      #endif //BUGFIX_BTS7769_20100713
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
        break;
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ランダムマッチ  レーティングの戦闘終了後
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_SC_HEAP_INIT,
    SEQ_START_MSG,
    SEQ_START_REPORT_ATLAS,
    SEQ_WAIT_REPORT_ATLAS,

    SEQ_SC_HEAP_EXIT,

    SEQ_START_DISCONNECT,
    SEQ_WAIT_DISCONNECT,

    SEQ_START_MSG_WAIT,
    SEQ_START_SAKE_RECORD,
    SEQ_WAIT_SAKE_RECORD,

    SEQ_START_RESULT_MSG,
    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
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
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_EndRec );
    }
    else
#endif 
    {

#ifdef BUGFIX_BTS7762_20100713
      p_wk->is_sc_before_err = FALSE;
#endif //BUGFIX_BTS7762_20100713

      GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_SC, WBM_SC_HEAP_SIZE );
      DWC_RAPCOMMON_SetSubHeapIDEx( DWC_ALLOCTYPE_GS, WBM_SC_HEAP_SIZE, WBM_SC_BORDER_SIZE, HEAPID_WIFIBATTLEMATCH_SC );
      DWC_RAPCOMMON_SetAutoDeleteSubHeap( HEAPID_WIFIBATTLEMATCH_SC );
      *p_seq  = SEQ_START_MSG;
    }
    break;

    //-------------------------------------
    ///	Atlasへの書き込み
    //=====================================
  case SEQ_START_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_013, WBM_TEXT_TYPE_WAIT);
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_REPORT_ATLAS );
    break;

  case SEQ_START_REPORT_ATLAS:
    {
      BOOL is_error = p_param->mode == WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;
      WIFIBATTLEMATCH_SC_StartReport( p_wk->p_net, WIFIBATTLEMATCH_SC_REPORT_TYPE_BTL_SCORE, WIFIBATTLEMATCH_TYPE_RNDRATE, p_param->p_param->btl_rule, p_param->cp_btl_score, is_error );
      p_wk->sc_state  = WIFIBATTLEMATCH_NET_SC_STATE_UPDATE;
    }
    p_wk->is_send_report  = FALSE;
    *p_seq = SEQ_WAIT_REPORT_ATLAS;
    break;
  case SEQ_WAIT_REPORT_ATLAS:
    { 
      if( p_wk->sc_state == WIFIBATTLEMATCH_NET_SC_STATE_UPDATE )
      {
        p_wk->sc_state = WIFIBATTLEMATCH_SC_ProcessReport(p_wk->p_net , &p_wk->is_send_report );

        //レポート送信していればレコード登録
        if( p_wk->sc_state != WIFIBATTLEMATCH_NET_SC_STATE_UPDATE )
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

      if( p_wk->sc_state != WIFIBATTLEMATCH_NET_SC_STATE_UPDATE )
      {
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:     //戻る
          WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
          /* fallthrough */
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
#ifndef SCREPORT_TIMEOUT_IS_DISCONNECT
          DWC_RAPCOMMON_ResetSubHeapID();
          //相手に切断されていたら、録画を飛ばす
          if( p_param->mode == WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR )
          { 
            WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_EndRec );
          }
          else
          { 
            *p_seq = SEQ_START_SAVE_MSG;
          }
          break;
#endif
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          DWC_RAPCOMMON_ResetSubHeapID();
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
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
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_EndRec );
    }
    else
    { 
      *p_seq = SEQ_START_DISCONNECT;
    }
    break;

    //-------------------------------------
    /// 切断処理
    //=====================================
  case SEQ_START_DISCONNECT:
    //ここまで切断等がない場合は相手を知り合いに登録
    {           
      SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      ETC_SAVE_WORK *p_etc  = SaveData_GetEtc( p_sv_ctrl );
      WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data = p_param->p_enemy_data;
      EtcSave_SetAcquaintance( p_etc, MyStatus_GetID( (MYSTATUS*)p_enemy_data->mystatus ) );
    }
    *p_seq = SEQ_WAIT_DISCONNECT;
    WIFIBATTLEMATCH_NET_StartDisConnect( p_wk->p_net );
    break;

  case SEQ_WAIT_DISCONNECT:
    if( WIFIBATTLEMATCH_NET_WaitDisConnect( p_wk->p_net ) )
    {
      if( *p_wk->p_param->p_server_time == 0 )
      { 
        *p_seq  = SEQ_START_SAKE_RECORD;
      }
      else
      { 
        WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_019, WBM_TEXT_TYPE_WAIT );
        *p_seq = SEQ_WAIT_MSG;
        WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_MSG_WAIT );
      }
    }

    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      *p_seq  = SEQ_START_MSG_WAIT;
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      break;
    }

    break;


  case SEQ_START_MSG_WAIT:
    OS_TPrintf( "サーバー待ち%d\n",  *p_wk->p_param->p_server_time );
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      *p_seq  = SEQ_START_SAKE_RECORD;
    }
    break;

  case SEQ_START_SAKE_RECORD:
    //履歴を入れる！
    { 
      WIFIBATTLEMATCH_GDB_RND_RECORD_DATA data;
      GFL_STD_MemClear( &data, sizeof(WIFIBATTLEMATCH_GDB_RND_RECORD_DATA) );
      data.record_data  = *p_wk->p_param->p_record_data;
      data.save_idx     = p_wk->p_param->p_recv_data->record_save_idx;
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_RECORD, &data );
    }
    *p_seq  = SEQ_WAIT_SAKE_RECORD;
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
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE  ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
          *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
          *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
          *p_seq = SEQ_START_RESULT_MSG;
          break;
        }
      }
    } 
    break;

  case SEQ_START_RESULT_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_33, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE_MSG );
    break;

    //-------------------------------------
    /// レーティングセーブ処理
    //=====================================
  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, WBM_TEXT_TYPE_WAIT );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    break;
  case SEQ_START_SAVE:
    //レーティングでセーブデータに保存するものは、自分のスコアのみ
    //マッチング前でセーブワークに代入をしている
    GAMEDATA_SaveAsyncStart(p_param->p_param->p_game_data);
    *p_seq       = SEQ_WAIT_SAVE;
    break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT result;
      result = GAMEDATA_SaveAsyncMain(p_param->p_param->p_game_data);
      switch(result){
      case SAVE_RESULT_CONTINUE:
      case SAVE_RESULT_LAST:
        break;
      case SAVE_RESULT_OK:
      case SAVE_RESULT_NG:
        p_param->result = WIFIBATTLEMATCH_CORE_RESULT_NEXT_REC;
        WBM_SEQ_End( p_seqwk ); 
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
 *	@brief  ランダムマッチ  レーティングモード録画からの戻り
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_EndRec( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECKERR,
    SEQ_START,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECKERR:
    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_NONE:
      *p_seq  = SEQ_START;
      break;
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_CupContinue );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      break;
    }
    break;


  case SEQ_START:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_CupContinue );
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
 *	@brief  ランダムマッチ  たいせんを続けますか？
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_CupContinue( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECKERR,
    SEQ_START_SELECT_CONTINUE_MSG,
    SEQ_START_SELECT_CONTINUE,
    SEQ_WAIT_SELECT_CONTINUE,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECKERR:
    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_NONE:
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:
      *p_seq  = SEQ_START_SELECT_CONTINUE_MSG;
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      break;
    }
    break;

    //-------------------------------------
    /// 対戦続行確認
    //=====================================
  case SEQ_START_SELECT_CONTINUE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_015, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SELECT_CONTINUE );
    break;
  case SEQ_START_SELECT_CONTINUE:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
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
        case 0:
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_StartMatching );
          break;
        case 1:
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_CupEnd );
          break;
        }
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
 *	@brief  ランダムマッチ  たいかいを終了しますか？
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_SELECT_CANCEL_MSG,
    SEQ_START_SELECT_CANCEL,
    SEQ_WAIT_SELECT_CANCEL,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
    //-------------------------------------
    /// ランダムマッチ終了確認
    //=====================================
  case SEQ_START_SELECT_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_016, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SELECT_CANCEL );
    break;

  case SEQ_START_SELECT_CANCEL:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq     = SEQ_WAIT_SELECT_CANCEL;
    break;
  case SEQ_WAIT_SELECT_CANCEL:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0:
          WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );

          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_DisConnextSendTime );
          break;
        case 1:
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Rate_CupContinue );
          break;
        }
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
 *	@brief  ランダムマッチ  レーティングモード  サケに日時を送信して終了
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Rate_DisConnextSendTime( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_START_MSG_SAKE_TIME,
    SEQ_START_SEND_SAKE_TIME,
    SEQ_WAIT_SEND_SAKE_TIME,

    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
    //-------------------------------------
    ///	セーブ
    //=====================================
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
        *p_seq  = SEQ_START_MSG_SAKE_TIME;
      }
    }
    break; 

    //-------------------------------------
    /// サケへの日時書き込み
    //=====================================
  case SEQ_START_MSG_SAKE_TIME:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_013, WBM_TEXT_TYPE_WAIT );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SEND_SAKE_TIME );
    break;

  case SEQ_START_SEND_SAKE_TIME:
    if( *p_wk->p_param->p_server_time == 0 )
    { 
      WIFIBATTLEMATCH_GDB_StartWrite( p_wk->p_net, WIFIBATTLEMATCH_GDB_WRITE_MYINFO, NULL );
      *p_seq  = SEQ_WAIT_SEND_SAKE_TIME;
    }
    else
    { 
      OS_TPrintf( "サーバー待ち%d\n",  *p_wk->p_param->p_server_time );
    }
    break;

  case SEQ_WAIT_SEND_SAKE_TIME:
    {
      WIFIBATTLEMATCH_GDB_RESULT res  = WIFIBATTLEMATCH_GDB_ProcessWrite( p_wk->p_net );
      if( res != WIFIBATTLEMATCH_GDB_RESULT_UPDATE )
      { 
        *p_wk->p_param->p_server_time  = WIFIBATTLEMATCH_NET_SAKE_SERVER_WAIT_SYNC;
        *p_seq  = SEQ_END;

        WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, TRUE, FALSE );
      }
    }
    break;

  case SEQ_END:
    WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
    p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_DisConnectEnd );
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
//=============================================================================
/**
 *      フリーモード
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ランダムマッチ  フリーモード開始
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Free_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_FREE_MSG,
    SEQ_WAIT_CARDIN,
    SEQ_WAIT_MSG,

    SEQ_START_MSG_EVILCHECK,

    SEQ_START_SUBSEQ_EVILCHECK,
    SEQ_WAIT_SUBSEQ_EVILCHECK,
    SEQ_START_DIRTY_MSG,
    SEQ_DIRTY_END,
  };

  WIFIBATTLEMATCH_RND_WORK	      *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
    //-------------------------------------
    ///	フリーモード開始メッセージ
    //=====================================
  case SEQ_START_FREE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_002, WBM_TEXT_TYPE_STREAM );
    { 
      //自分のデータを表示
      Util_PlayerInfo_Create( p_wk, WIFIBATTLEMATCH_CORE_RETMODE_FREE );
    }
    *p_seq       = SEQ_WAIT_CARDIN;
    break;

  case SEQ_WAIT_CARDIN:
    if( Util_PlayerInfo_Move( p_wk ) )
    { 
      *p_seq       = SEQ_WAIT_MSG;
    }
    break;

  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    {
#ifdef DEBUG_DIRTYCHECK_PASS
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_Matching );
#else
      *p_seq  = SEQ_START_MSG_EVILCHECK;
#endif 
    }
    break;

    //-------------------------------------
    /// 不正チェック
    //=====================================

  case SEQ_START_MSG_EVILCHECK:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, WBM_TEXT_TYPE_WAIT );
    *p_seq  = SEQ_START_SUBSEQ_EVILCHECK;
    break;

  case SEQ_START_SUBSEQ_EVILCHECK:
    Util_SubSeq_Start( p_wk, WbmRndSubSeq_EvilCheck );
    *p_seq  = SEQ_WAIT_SUBSEQ_EVILCHECK;

    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      Util_PlayerInfo_DeleteOut( p_wk );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      break;
    }

    break;

  case SEQ_WAIT_SUBSEQ_EVILCHECK:
    { 
      WBM_RND_SUBSEQ_RET ret = Util_SubSeq_Main( p_wk );
      if( ret == WBM_RND_SUBSEQ_EVILCHECK_RET_SUCCESS )
      { 
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_Matching );
      }
      else if( ret == WBM_RND_SUBSEQ_EVILCHECK_RET_DARTY )
      {   
        *p_seq       = SEQ_START_DIRTY_MSG;
      }
      else if( ret == WBM_RND_SUBSEQ_EVILCHECK_RET_SERVER_ERR )
      {
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      }

      if( ret != WBM_RND_SUBSEQ_RET_NONE )
      {
        //エラー
        switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
          Util_PlayerInfo_DeleteOut( p_wk );
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
          break;
        }
      }
    }
    break;
    
  case SEQ_START_DIRTY_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_ERR_00, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_DIRTY_END;
    break;

  case SEQ_DIRTY_END:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    {
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_DisConnectEnd );
    }
    break;

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ランダムマッチ  フリーモードのマッチング処理
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 *
 *  接続OKと表示されるぎりぎりまで、はい、いいえでキャンセルさせるため、以下のことを行っている
 *  ・はい/いいえ選択中は接続OKまでシーケンスが動かないことを考慮して、各待ち処理でタイムアウトを設定
 *  ・はいを押したときや、タイムアウトで切断するときは
 * 　　 相手と繋がっていたら切断キャンセルフラグを送信してから切断
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Free_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_START_MATCHING_MSG,
    SEQ_START_MATCHING,
    SEQ_WAIT_MATCHING,
    SEQ_START_CHECK_CHEAT,
    SEQ_WAIT_CHECK_CHEAT,

    SEQ_START_SEND_TIMING,
    SEQ_WAIT_SEND_TIMING,
    SEQ_START_SENDDATA,
    SEQ_WAIT_SENDDATA,

    SEQ_CHECK_DARTYDATA,
    SEQ_START_BADWORD,
    SEQ_WAIT_BADWORD,

    SEQ_START_CANCEL_TIMING,
    SEQ_WAIT_CANCEL_TIMING,
    SEQ_START_OK_TIMING,
    SEQ_WAIT_OK_TIMING,

    SEQ_OK_MATCHING_MSG,
    SEQ_OK_MATCHING_WAIT,
    SEQ_WAIT_MATCH_CARDIN,
    SEQ_CARDIN_WAIT,
    SEQ_END_MATCHING_MSG,
    SEQ_END_MATCHING,

    SEQ_START_CANCEL,
    SEQ_END_CANCEL,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;
  UTIL_CANCEL_STATE cancel_state;
  BOOL is_timeout_enable  = FALSE;

  //-------------------------------------
  ///	キャンセルシーケンス
  //=====================================
  {
    BOOL is_cancel_enable = (SEQ_WAIT_MATCHING <= *p_seq && *p_seq < SEQ_START_CANCEL_TIMING);
    BOOL can_disconnect = !( SEQ_START_BADWORD <= *p_seq  && *p_seq <= SEQ_WAIT_BADWORD );
    cancel_state  = Util_Cancel_Seq( p_wk, is_cancel_enable, can_disconnect );

    //キャンセルリクエスト
    if( cancel_state == UTIL_CANCEL_STATE_DECIDE )
    {
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_CupEnd );
      return;
    }
  }

  //-------------------------------------
  ///	マッチングシーケンス
  //=====================================
  switch( *p_seq )
  { 
    //-------------------------------------
    ///	マッチング開始
    //=====================================
  case SEQ_INIT:
    { 
      Util_RenewalMyData( p_param->p_player_data, p_wk );
      *p_seq = SEQ_START_MATCHING_MSG;
    }
    break;

  case SEQ_START_MATCHING_MSG:
    p_wk->cancel_seq  = 0;
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, TRUE );
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_009, WBM_TEXT_TYPE_QUE );
    *p_seq = SEQ_START_MATCHING;
    break;

  case SEQ_START_MATCHING:
#ifdef BUGFIX_BTS7733_20100712
    //マッチングエラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );
      return;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      return;
    }
#endif //BUGFIX_BTS7733_20100712
    { 
      WIFIBATTLEMATCH_MATCH_KEY_DATA  data;
      Util_Matchkey_SetData( &data, p_wk );
      WIFIBATTLEMATCH_NET_StartMatchMake( p_wk->p_net, WIFIBATTLEMATCH_TYPE_RNDFREE, FALSE, p_param->p_param->btl_rule, &data ); 
    }
    *p_seq  = SEQ_WAIT_MATCHING;
    break;

  case SEQ_WAIT_MATCHING:
    //マッチングエラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, FALSE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Start );

    #ifdef BUGFIX_BTS7769_20100713
      Util_Cancel_ResetSeq( p_wk );
    #endif //BUGFIX_BTS7769_20100713
      return;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      return;
    }

    {
      WIFIBATTLEMATCH_NET_MATCHMAKE_STATE state = WIFIBATTLEMATCH_NET_WaitMatchMake( p_wk->p_net );
      //マッチング終了待ち
      if( WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_SUCCESS == state )
      { 
        p_wk->match_timeout = 0;
        *p_seq = SEQ_START_SEND_TIMING;
      }
      else if( WIFIBATTLEMATCH_NET_MATCHMAKE_STATE_FAILED == state )
      {
        *p_seq = SEQ_START_MATCHING;
      }
    }
    break;

    //-------------------------------------
    ///	データ送受信
    //=====================================

  case SEQ_START_SEND_TIMING:
    WIFIBATTLEMATCH_NET_StartTiming( p_wk->p_net, WIFIBATTLEMATCH_NET_TIMINGSYNC_MATHING_START );
    *p_seq  = SEQ_WAIT_SEND_TIMING;
#ifdef BUGFIX_BTS7738_20100712
    p_wk->match_timeout = 0;
#endif //BUGFIX_BTS7738_20100712
    break;

  case SEQ_WAIT_SEND_TIMING:
    if( WIFIBATTLEMATCH_NET_WaitTiming( p_wk->p_net ) )
    {
#ifdef BUGFIX_BTS7738_20100712
      p_wk->match_timeout = 0;
#endif //BUGFIX_BTS7738_20100712
      *p_seq  = SEQ_START_SENDDATA;
    }
#ifdef BUGFIX_BTS7738_20100712
    is_timeout_enable = TRUE;
#endif //BUGFIX_BTS7738_20100712
    break;

  case SEQ_START_SENDDATA:
    //キャンセル状態のときは送信しない
    if( cancel_state == UTIL_CANCEL_STATE_NONE )
    {
      WIFIBATTLEMATCH_ENEMYDATA *p_my_data = p_param->p_player_data;
      if( WIFIBATTLEMATCH_NET_StartEnemyData( p_wk->p_net, p_my_data ) )
      { 
        p_wk->match_timeout = 0;
        *p_seq       = SEQ_WAIT_SENDDATA;
      }
      is_timeout_enable = TRUE;
    }
    break;

  case SEQ_WAIT_SENDDATA:
    {
      WIFIBATTLEMATCH_ENEMYDATA *p_recv;
      if( WIFIBATTLEMATCH_NET_WaitEnemyData( p_wk->p_net, &p_recv ) )
      { 
        u32 dirty;
        WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data = p_param->p_enemy_data;
        GFL_STD_MemCopy( p_recv, p_enemy_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
  

        dirty = WIFIBATTLEMATCH_DATA_ModifiEnemyData( p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );

        p_wk->match_timeout = 0;
        *p_seq       = SEQ_CHECK_DARTYDATA;
      }
      is_timeout_enable = TRUE;
    }
    break;

  case SEQ_CHECK_DARTYDATA:
#ifdef DEBUG_DIRTYCHECK_PASS
    if( 1 )
#else
    if( Util_VerifyPokeData( p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE ) )
#endif
    { 
      *p_seq       = SEQ_START_BADWORD;
    }
    else
    { 
      //不正なのでマッチングに戻る
      WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_Matching );
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
#ifdef BUGFIX_BTS7760_20100713
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:       //戻る   
#endif //BUGFIX_BTS7760_20100713
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る   
          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_CupContinue );

        #ifdef BUGFIX_BTS7769_20100713
          Util_Cancel_ResetSeq( p_wk );
        #endif //BUGFIX_BTS7769_20100713
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
          WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
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
    p_wk->match_timeout = 0;
    *p_seq  = SEQ_WAIT_OK_TIMING;
    break;

  case SEQ_WAIT_OK_TIMING:

    is_timeout_enable = TRUE;
    if( WIFIBATTLEMATCH_NET_WaitTiming( p_wk->p_net ) )
    {
      *p_seq  = SEQ_OK_MATCHING_MSG;
    }
    break;

  case SEQ_OK_MATCHING_MSG:
    WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
    PMSND_PlaySE( WBM_SND_SE_MATCHING_OK );

    Util_List_Delete( p_wk ); //はい、いいえ中に何かの影響でマッチングした場合、解放
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_010, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_OK_MATCHING_WAIT );
    break;

  case SEQ_OK_MATCHING_WAIT:
    if( p_wk->cnt++ > MATCHING_MSG_WAIT_SYNC )
    { 
      p_wk->cnt      = 0;
      Util_MatchInfo_Create( p_wk, p_param->p_enemy_data );
      GFL_BG_SetVisible( BG_FRAME_M_TEXT, FALSE );
      *p_seq      = SEQ_WAIT_MATCH_CARDIN;
    }
    break;

  case SEQ_WAIT_MATCH_CARDIN:
    if( Util_MatchInfo_Main( p_wk ) )
    { 
      *p_seq  = SEQ_CARDIN_WAIT;
    }
    break;

  case SEQ_CARDIN_WAIT:
    if( p_wk->cnt++ > ENEMYDATA_WAIT_SYNC )
    { 
      p_wk->cnt      = 0;
      *p_seq      = SEQ_END_MATCHING_MSG;
    }
    break;

  case SEQ_END_MATCHING_MSG:
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

  case SEQ_START_CANCEL:
    if( WIFIBATTLEMATCH_NET_SendMatchCancel( p_wk->p_net ) )
    {
      *p_seq  = SEQ_END_CANCEL;
    }
    break;

  case SEQ_END_CANCEL:
    WIFIBATTLEMATCH_NET_SetDisConnectForce( p_wk->p_net );
    *p_seq  = SEQ_START_MATCHING_MSG;
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
        *p_seq  = SEQ_START_CANCEL;
      }

      //エラー
      switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
      { 
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_CupContinue );

      #ifdef BUGFIX_BTS7769_20100713
        Util_Cancel_ResetSeq( p_wk );
      #endif //BUGFIX_BTS7769_20100713
        break;

      case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
        WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
        WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
        break;
      }
    }
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  ランダムマッチ  フリーモードの戦闘終了後
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Free_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK,

    SEQ_SEND_BTLDIRTYFLGA,
    SEQ_RECV_BTLDIRTYFLGA,

    SEQ_START_DISCONNECT_MSG,
    SEQ_START_DISCONNECT,
    SEQ_WAIT_DISCONNECT,

    SEQ_START_CARD,
    SEQ_WAIT_CARD,

    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_CHECK:
    //相手に切断されていたら、録画を飛ばす
    if( p_param->mode == WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR )
    { 
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_EndRec );
    }
    else
    { 
      *p_seq = SEQ_START_DISCONNECT_MSG;
    }
    break;

  case SEQ_START_DISCONNECT_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_020, WBM_TEXT_TYPE_WAIT);
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_SEND_BTLDIRTYFLGA );
    p_wk->cnt = 0;
    break;

  case SEQ_SEND_BTLDIRTYFLGA:
    //不正フラグをお互い贈りあう
    if( WIFIBATTLEMATCH_NET_SendBtlDirtyFlag( p_wk->p_net, &p_param->cp_btl_score->is_dirty ) )
    {
      *p_seq  = SEQ_RECV_BTLDIRTYFLGA;
    }
    break;

  case SEQ_RECV_BTLDIRTYFLGA:
#ifdef BUGFIX_BTS7849_20100715
    if( WIFIBATTLEMATCH_NET_RecvBtlDirtyFlag( p_wk->p_net, (u32*)&p_param->cp_btl_score->is_other_dirty ) )
#else //BUGFIX_BTS7849_20100715
    if( WIFIBATTLEMATCH_NET_RecvBtlDirtyFlag( p_wk->p_net, (u32*)&p_param->cp_btl_score->is_dirty ) )
#endif //BUGFIX_BTS7849_20100715
    {
      *p_seq = SEQ_START_DISCONNECT;
    }

    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      p_wk->cnt = 0;
      *p_seq  = SEQ_WAIT_CARD;
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      p_wk->cnt = 0;
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      break;
    }

    break;
    //-------------------------------------
    /// 切断処理
    //=====================================
  case SEQ_START_DISCONNECT:
    //ここまで切断がない場合は相手を知り合いに登録
    {           
      SAVE_CONTROL_WORK *p_sv_ctrl  = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      ETC_SAVE_WORK *p_etc  = SaveData_GetEtc( p_sv_ctrl );
      WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data = p_param->p_enemy_data;
      EtcSave_SetAcquaintance( p_etc, MyStatus_GetID( (MYSTATUS*)p_enemy_data->mystatus ) );
    }
    //ここまで切断等がない場合はレコード登録
    {
      WBM_RECORD_Count( p_param->p_param->p_game_data, p_param->cp_btl_score->result );
    }
    WIFIBATTLEMATCH_NET_StartDisConnect( p_wk->p_net );
    *p_seq = SEQ_WAIT_DISCONNECT;
    break;

  case SEQ_WAIT_DISCONNECT:
    p_wk->cnt++;
    if( WIFIBATTLEMATCH_NET_WaitDisConnect( p_wk->p_net ) )
    { 
      *p_seq  = SEQ_START_CARD;
    }

    //エラー
    switch( WIFIBATTLEMATCH_NET_CheckErrorRepairType( p_wk->p_net, FALSE, TRUE ) )
    { 
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_TIMEOUT:
    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETURN:       //戻る
      p_wk->cnt = 0;
      *p_seq  = SEQ_WAIT_CARD;
      break;

    case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //切断しログインからやり直し
      p_wk->cnt = 0;
      WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Err_ReturnLogin );
      break;
    }
    break;

  case SEQ_START_CARD:
    //つうしんたいきちゅうが一瞬で終わるのを回避
    if( p_wk->cnt++ > 30 )
    {
      p_wk->cnt = 0;
      *p_seq  = SEQ_WAIT_CARD;
    }
    break;

  case SEQ_WAIT_CARD:
    { 
      *p_seq  = SEQ_START_SAVE_MSG;
    }
    break;

    //-------------------------------------
    /// セーブ処理
    //=====================================
  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, WBM_TEXT_TYPE_WAIT);
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    break;

  case SEQ_START_SAVE:
    GAMEDATA_SaveAsyncStart(p_param->p_param->p_game_data);
    *p_seq       = SEQ_WAIT_SAVE;
    break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT result;
      result = GAMEDATA_SaveAsyncMain(p_param->p_param->p_game_data);
      switch(result){
      case SAVE_RESULT_CONTINUE:
      case SAVE_RESULT_LAST:
        break;
      case SAVE_RESULT_OK:
      case SAVE_RESULT_NG:
        p_param->result = WIFIBATTLEMATCH_CORE_RESULT_NEXT_REC;
        WBM_SEQ_End( p_seqwk ); 
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
 *	@brief  ランダムマッチ  フリーモードの録画終了後
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Free_EndRec( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_CARD,
    SEQ_WAIT_CARD,

    SEQ_START,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_CARD:
    //Util_PlayerInfo_Create( p_wk, WIFIBATTLEMATCH_CORE_RETMODE_FREE );
    *p_seq  = SEQ_WAIT_CARD;
    break;

  case SEQ_WAIT_CARD:
    //if( Util_PlayerInfo_Move( p_wk ) )
    { 
      *p_seq  = SEQ_START;
    }
    break;

  case SEQ_START:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_CupContinue );
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
 *	@brief  ランダムマッチ  たいせんをつづけますか？
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Free_CupContinue( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_SELECT_CONTINUE_MSG,
    SEQ_START_SELECT_CONTINUE,
    SEQ_WAIT_SELECT_CONTINUE,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
    //-------------------------------------
    /// 対戦続行確認
    //=====================================
  case SEQ_START_SELECT_CONTINUE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_015, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SELECT_CONTINUE );
    break;
  case SEQ_START_SELECT_CONTINUE:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
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
        case 0:
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_Matching );
          break;
        case 1:
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_CupEnd );
          break;
        }
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
 *	@brief  ランダムマッチ  たいかいをしゅうりょうしますか？
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Free_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_SELECT_CANCEL_MSG,
    SEQ_START_SELECT_CANCEL,
    SEQ_WAIT_SELECT_CANCEL,

    SEQ_START_MSG_END,
    SEQ_END,

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
    //-------------------------------------
    /// ランダムマッチ終了確認
    //=====================================
  case SEQ_START_SELECT_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_016, WBM_TEXT_TYPE_STREAM );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SELECT_CANCEL );
    break;
  case SEQ_START_SELECT_CANCEL:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq     = SEQ_WAIT_SELECT_CANCEL;
    break;
  case SEQ_WAIT_SELECT_CANCEL:
    {
      const u32 select  = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        switch( select )
        { 
        case 0:
          p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
          *p_seq  = SEQ_START_MSG_END;
          break;
        case 1:
          WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_Free_CupContinue );
          break;
        }
      }
    }
    break;

  case SEQ_START_MSG_END:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_020, WBM_TEXT_TYPE_WAIT );
    *p_seq = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_END );
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmRndSeq_DisConnectEnd );
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
 *	@brief  エラーのためログインに戻る
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_Err_ReturnLogin( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
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
//----------------------------------------------------------------------------
/**
 *	@brief  切断して終了
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSeq_DisConnectEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_END,
  };
  WIFIBATTLEMATCH_RND_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_END:


    if( p_wk->p_net )
    { 
      WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );
      p_wk->p_net = NULL;
    }
    WBM_SEQ_End( p_seqwk );
    break;
  }
}
//=============================================================================
/**
 *    サブシーケンス
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  切断して終了
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       シーケンスワーク
 *	@param  p_peq                       シーケンス
 *	@param	p_wk_adrs                   ワークアドレス
 */
//-----------------------------------------------------------------------------
static void WbmRndSubSeq_EvilCheck( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START,
    SEQ_START_EVIL_CHECK,
    SEQ_WAIT_EVIL_CHECK,
    SEQ_END_CHECK,
    SEQ_END,
  };

  WIFIBATTLEMATCH_RND_WORK	  *p_wk	      = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param    = p_wk->p_param;
  WIFIBATTLEMATCH_ENEMYDATA   *p_my_data  = p_param->p_player_data;

  switch( *p_seq )
  { 
  case SEQ_START:
    p_wk->cnt = 0;
    GFL_STD_MemClear( &p_wk->evilecheck_data, sizeof(WIFIBATTLEMATCH_NET_EVILCHECK_DATA ));
    (*p_seq)++;
    break;

  case SEQ_START_EVIL_CHECK:
    { 
      WIFIBATTLEMATCH_NET_StartEvilCheck( p_wk->p_net, p_my_data->pokeparty, WIFIBATTLEMATCH_NET_EVILCHECK_TYPE_PARTY );
      (*p_seq)++;
      NAGI_Printf( "EvilCheck Start\n" );
    }
    break;

  case SEQ_WAIT_EVIL_CHECK:
    {
      WIFIBATTLEMATCH_NET_EVILCHECK_RET ret;
      ret = WIFIBATTLEMATCH_NET_WaitEvilCheck( p_wk->p_net, &p_wk->evilecheck_data );
      if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_SUCCESS )
      { 
        NAGI_Printf( "EvilCheck Success\n" );
        (*p_seq)  = SEQ_END_CHECK;
      }
      else if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_ERROR )
      { 
        NAGI_Printf( "EvilCheck Error\n" );
        p_wk->subseq_ret  = WBM_RND_SUBSEQ_EVILCHECK_RET_NET_ERR;
        *p_seq  = SEQ_END;
      }
      else if( ret == WIFIBATTLEMATCH_NET_EVILCHECK_RET_SERVERERR )
      {
        NAGI_Printf( "EvilCheck Server Error\n" );
        p_wk->subseq_ret  = WBM_RND_SUBSEQ_EVILCHECK_RET_SERVER_ERR;
        *p_seq  = SEQ_END;
      }
    }
    break;

  case SEQ_END_CHECK:
    //データを格納
    OS_TPrintf( "ポケモン結果[不正番号%d]status=%d\n", p_wk->evilecheck_data.poke_result[0], p_wk->evilecheck_data.status_code );
    if( p_wk->evilecheck_data.status_code == 0 )
    { 
      //署名を保存
      GFL_STD_MemCopy( p_wk->evilecheck_data.sign, p_my_data->sign, NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN );

      NAGI_Printf( "EvilCheck 成功！！\n" );
      p_wk->subseq_ret  = WBM_RND_SUBSEQ_EVILCHECK_RET_SUCCESS;
      *p_seq  = SEQ_END;
    }
    else
    { 
      NAGI_Printf( "EvilCheck 不正！！\n" );
      p_wk->subseq_ret  = WBM_RND_SUBSEQ_EVILCHECK_RET_DARTY;
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
 *    便利関数
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  自分の情報を表示
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_Create( WIFIBATTLEMATCH_RND_WORK *p_wk, WIFIBATTLEMATCH_CORE_RETMODE mode )
{ 
  if( p_wk->p_playerinfo == NULL )
  {
    MYSTATUS    *p_my;
    GFL_CLUNIT	*p_unit;
    BOOL is_rate;

    //自分のデータを表示
    PLAYERINFO_RANDOMMATCH_DATA info_setup;

    p_my  = GAMEDATA_GetMyStatus( p_wk->p_param->p_param->p_game_data); 
    p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );

    GFL_STD_MemClear( &info_setup, sizeof(PLAYERINFO_RANDOMMATCH_DATA) );


    if( mode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      info_setup.btl_rule = p_wk->p_param->p_param->btl_rule;
      is_rate = TRUE;

      info_setup.rate     = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_RATE );
      info_setup.win_cnt  = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      info_setup.lose_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );
      info_setup.btl_cnt  = (u32)info_setup.win_cnt + (u32)info_setup.lose_cnt;

      if( info_setup.rate == 0 && info_setup.btl_cnt == 0 )
      { 
        info_setup.rate = WIFIBATTLEMATCH_GDB_DEFAULT_RATEING;
      }
    }
    else if( mode == WIFIBATTLEMATCH_CORE_RETMODE_FREE )
    { 
      is_rate = FALSE;
      info_setup.btl_rule = p_wk->p_param->p_param->btl_rule;
      info_setup.rate     = 0;
      info_setup.win_cnt  = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      info_setup.lose_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );
      info_setup.btl_cnt  = (u32)info_setup.win_cnt + (u32)info_setup.lose_cnt;
    }
    info_setup.trainerID  = MyStatus_GetTrainerView( p_my );
    p_wk->p_playerinfo	= PLAYERINFO_RND_Init( &info_setup, is_rate, p_my, p_unit, p_wk->p_res, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, HEAPID_WIFIBATTLEMATCH_CORE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  自分の情報を破棄
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_Delete( WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  if( p_wk->p_playerinfo )
  { 
    PLAYERINFO_RND_Exit( p_wk->p_playerinfo );
    p_wk->p_playerinfo  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  自分のカードをスライドイン
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ワーク
 *
 *	@return TRUEで完了  FALSEで処理中
 */
//-----------------------------------------------------------------------------
static BOOL Util_PlayerInfo_Move( WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  return PLAYERINFO_MoveMain( p_wk->p_playerinfo );
}
//----------------------------------------------------------------------------
/**
 *	@brief  自分のカードをスライドアウト
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ワーク
 *
 *	@return TRUEで完了  FALSEで処理中
 */
//-----------------------------------------------------------------------------
static BOOL Util_PlayerInfo_MoveOut( WIFIBATTLEMATCH_RND_WORK *p_wk )
{
  return PLAYERINFO_MoveOutMain( p_wk->p_playerinfo );
}
//----------------------------------------------------------------------------
/**
 *	@brief  自分のカードを作成  すでにスライドインしている版
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ワーク
 *	@param	mode                            モード
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_CreateStay( WIFIBATTLEMATCH_RND_WORK *p_wk, WIFIBATTLEMATCH_CORE_RETMODE mode )
{ 
  Util_PlayerInfo_Create( p_wk, mode );

  while( !Util_PlayerInfo_Move( p_wk ) )
  { 

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  自分のカードを破棄
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_DeleteOut( WIFIBATTLEMATCH_RND_WORK *p_wk )
{

  if( p_wk->p_playerinfo )
  { 
    while( !Util_PlayerInfo_MoveOut( p_wk ) )
    { 

    }
    Util_PlayerInfo_Delete( p_wk );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  対戦者の情報を表示
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_MatchInfo_Create( WIFIBATTLEMATCH_RND_WORK *p_wk, const WIFIBATTLEMATCH_ENEMYDATA *cp_enemy_data )
{ 
  if( p_wk->p_matchinfo == NULL )
  { 
    GFL_CLUNIT  *p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    WIFIBATTLEMATCH_TYPE type;
    BOOL is_rate;

    if( p_wk->p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      is_rate = TRUE;
      type  = WIFIBATTLEMATCH_TYPE_RNDRATE;
    }
    else if( p_wk->p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_FREE )
    { 
      is_rate = FALSE;
      type  = WIFIBATTLEMATCH_TYPE_RNDFREE;
    }

    p_wk->p_matchinfo		= MATCHINFO_Init( cp_enemy_data, p_unit, p_wk->p_res, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, type, is_rate, REGULATION_CARD_BGM_TRAINER, HEAPID_WIFIBATTLEMATCH_CORE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  対戦者の情報を破棄
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_MatchInfo_Delete( WIFIBATTLEMATCH_RND_WORK *p_wk )
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
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ワーク
 *
 *	@return TRUEで完了  FALSEで処理中
 */
//-----------------------------------------------------------------------------
static BOOL Util_MatchInfo_Main( WIFIBATTLEMATCH_RND_WORK *p_wk )
{
  return MATCHINFO_MoveMain( p_wk->p_matchinfo );
}

//----------------------------------------------------------------------------
/**
 *	@brief  対戦者カードを破棄
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_Matchinfo_Clear( WIFIBATTLEMATCH_RND_WORK *p_wk )
{
  if( p_wk->p_matchinfo )
  {
    Util_MatchInfo_Delete( p_wk );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  セーブ  レートモード用
 *
 *	@param	RNDMATCH_DATA *p_save セーブ
 *	@param	mode                  ルール
 *	@param	WIFIBATTLEMATCH_RND_WORK *cp_wk ワーク
 *	@retval TRUEセーブと受信してきたデータで差があった  FALSE同じ
 */
//-----------------------------------------------------------------------------
static BOOL Util_SaveRateScore( RNDMATCH_DATA *p_save, WIFIBATTLEMATCH_BTLRULE btl_rule, const WIFIBATTLEMATCH_RND_WORK *cp_wk )
{ 
  u32 rate, win_cnt, lose_cnt, btl_cnt;
  BOOL ret  = FALSE;

  switch( btl_rule )
  { 
  case WIFIBATTLEMATCH_BTLRULE_SINGLE:  
    rate     = cp_wk->p_param->p_rnd_sake_data->single_rate;
    win_cnt  = cp_wk->p_param->p_rnd_sake_data->single_win;
    lose_cnt = cp_wk->p_param->p_rnd_sake_data->single_lose;
    btl_cnt  = cp_wk->p_param->p_rnd_sake_data->single_win + cp_wk->p_param->p_rnd_sake_data->single_lose;
    break;

  case WIFIBATTLEMATCH_BTLRULE_DOUBLE:
    rate     = cp_wk->p_param->p_rnd_sake_data->double_rate;
    win_cnt  = cp_wk->p_param->p_rnd_sake_data->double_win;
    lose_cnt = cp_wk->p_param->p_rnd_sake_data->double_lose;
    btl_cnt  = cp_wk->p_param->p_rnd_sake_data->double_win + cp_wk->p_param->p_rnd_sake_data->double_lose;
    break;

  case WIFIBATTLEMATCH_BTLRULE_TRIPLE:
    rate     = cp_wk->p_param->p_rnd_sake_data->triple_rate;
    win_cnt  = cp_wk->p_param->p_rnd_sake_data->triple_win;
    lose_cnt = cp_wk->p_param->p_rnd_sake_data->triple_lose;
    btl_cnt  = cp_wk->p_param->p_rnd_sake_data->triple_win + cp_wk->p_param->p_rnd_sake_data->triple_lose;
    break;

  case WIFIBATTLEMATCH_BTLRULE_ROTATE:
    rate     = cp_wk->p_param->p_rnd_sake_data->rot_rate;
    win_cnt  = cp_wk->p_param->p_rnd_sake_data->rot_win;
    lose_cnt = cp_wk->p_param->p_rnd_sake_data->rot_lose;
    btl_cnt  = cp_wk->p_param->p_rnd_sake_data->rot_win + cp_wk->p_param->p_rnd_sake_data->rot_lose;
    break;

  case WIFIBATTLEMATCH_BTLRULE_SHOOTER:
    rate     = cp_wk->p_param->p_rnd_sake_data->shooter_rate;
    win_cnt  = cp_wk->p_param->p_rnd_sake_data->shooter_win;
    lose_cnt = cp_wk->p_param->p_rnd_sake_data->shooter_lose;
    btl_cnt  = cp_wk->p_param->p_rnd_sake_data->shooter_win + cp_wk->p_param->p_rnd_sake_data->shooter_lose;
    break;
  } 

  //スコアとセーブデータに差があるかチェック
  if( rate == RNDMATCH_GetParam( p_save, RNDMATCH_TYPE_RATE_SINGLE + btl_rule, RNDMATCH_PARAM_IDX_RATE )
      && win_cnt == RNDMATCH_GetParam( p_save, RNDMATCH_TYPE_RATE_SINGLE + btl_rule, RNDMATCH_PARAM_IDX_WIN )
      && lose_cnt == RNDMATCH_GetParam( p_save, RNDMATCH_TYPE_RATE_SINGLE + btl_rule, RNDMATCH_PARAM_IDX_LOSE ) )
  {
    ret = FALSE;
  }
  else
  {
    ret = TRUE;
  }

  //セーブデータ
  RNDMATCH_SetParam( p_save, RNDMATCH_TYPE_RATE_SINGLE + btl_rule, RNDMATCH_PARAM_IDX_RATE, rate );
  RNDMATCH_SetParam( p_save, RNDMATCH_TYPE_RATE_SINGLE + btl_rule, RNDMATCH_PARAM_IDX_WIN, win_cnt );
  RNDMATCH_SetParam( p_save, RNDMATCH_TYPE_RATE_SINGLE + btl_rule, RNDMATCH_PARAM_IDX_LOSE, lose_cnt );

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  セーブ  フリーモード用
 *
 *	@param	RNDMATCH_DATA *p_save セーブ
 *	@param	mode                  ルール
 *	@param	WIFIBATTLEMATCH_RND_WORK *cp_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_SaveFreeScore( RNDMATCH_DATA *p_rndmatch, WIFIBATTLEMATCH_BTLRULE btl_rule, BtlResult btl_result )
{ 
  OS_TFPrintf( 3, "スコア戦闘結果 %d", btl_result );
  switch( btl_result )
  {
  case BTL_RESULT_WIN:
  case BTL_RESULT_RUN_ENEMY:
    RNDMATCH_AddParam( p_rndmatch, btl_rule, RNDMATCH_PARAM_IDX_WIN );
    break;
  case BTL_RESULT_LOSE:
  case BTL_RESULT_RUN:
    RNDMATCH_AddParam( p_rndmatch, btl_rule, RNDMATCH_PARAM_IDX_LOSE );
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  リスト初期化
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ワーク
 *	@param	type                            リストの種類
 */
//-----------------------------------------------------------------------------
static void Util_List_Create( WIFIBATTLEMATCH_RND_WORK *p_wk, UTIL_LIST_TYPE type )
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
    case UTIL_LIST_TYPE_YESNO:
      setup.strID[0]= WIFIMATCH_TEXT_007;
      setup.strID[1]= WIFIMATCH_TEXT_008;
      setup.list_max= 2;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 1;
      break;

    case UTIL_LIST_TYPE_FREERATE:
      setup.strID[0]= WIFIMATCH_TEXT_003;
      setup.strID[1]= WIFIMATCH_TEXT_004;
      setup.strID[2]= WIFIMATCH_TEXT_005;
      setup.list_max= 3;
      setup.is_cancel   = TRUE;
      setup.cancel_idx  = 2;
#ifdef WIFIMATCH_RATE_AUTO
      setup.default_idx = 1;
#endif
      break;
    }

    p_wk->list_type = type;
    p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  リスト破棄
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_List_Delete( WIFIBATTLEMATCH_RND_WORK *p_wk )
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
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ワーク
 *
 *	@return 選択したもの
 */
//-----------------------------------------------------------------------------
static u32 Util_List_Main( WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  if( p_wk->p_list )
  { 
    return  WBM_LIST_Main( p_wk->p_list );
  }
  else
  { 
    NAGI_Printf( "List not exist !!!\n" );
    return WBM_LIST_SELECT_NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  マッチング用データ設定
 *
 *	@param	WIFIBATTLEMATCH_MATCH_KEY_DATA *p_data  データ
 *	@param	WIFIBATTLEMATCH_RND_WORK *cp_wk         ワーク
 */
//-----------------------------------------------------------------------------
static void Util_Matchkey_SetData( WIFIBATTLEMATCH_MATCH_KEY_DATA *p_data, const WIFIBATTLEMATCH_RND_WORK *cp_wk )
{ 
  GFL_STD_MemClear( p_data, sizeof(WIFIBATTLEMATCH_MATCH_KEY_DATA) );

  p_data->disconnect = cp_wk->p_param->p_rnd_sake_data->disconnect;
  p_data->cup_no = 0;

  //全試合数
  p_data->btlcnt  =  cp_wk->p_param->p_rnd_sake_data->single_win + cp_wk->p_param->p_rnd_sake_data->single_lose //シングル
    + cp_wk->p_param->p_rnd_sake_data->double_win + cp_wk->p_param->p_rnd_sake_data->double_lose   //ダブル
    + cp_wk->p_param->p_rnd_sake_data->triple_win + cp_wk->p_param->p_rnd_sake_data->triple_lose   //トリプル
    + cp_wk->p_param->p_rnd_sake_data->rot_win + cp_wk->p_param->p_rnd_sake_data->rot_lose   //ローテーション
    + cp_wk->p_param->p_rnd_sake_data->shooter_win + cp_wk->p_param->p_rnd_sake_data->shooter_lose;  //シューター

  switch( cp_wk->p_param->p_param->btl_rule )
  { 
  case WIFIBATTLEMATCH_BTLRULE_SINGLE:  
    p_data->rate    = cp_wk->p_param->p_rnd_sake_data->single_rate;
    break;

  case WIFIBATTLEMATCH_BTLRULE_DOUBLE:
    p_data->rate     = cp_wk->p_param->p_rnd_sake_data->double_rate;
    break;

  case WIFIBATTLEMATCH_BTLRULE_TRIPLE:
    p_data->rate     = cp_wk->p_param->p_rnd_sake_data->triple_rate;
    break;

  case WIFIBATTLEMATCH_BTLRULE_ROTATE:
    p_data->rate     = cp_wk->p_param->p_rnd_sake_data->rot_rate;
    break;

  case WIFIBATTLEMATCH_BTLRULE_SHOOTER:
    p_data->rate     = cp_wk->p_param->p_rnd_sake_data->shooter_rate;
    break;
  } 

}

//----------------------------------------------------------------------------
/**
 *	@brief  自分のデータを格納 
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data    データ
 *	@param	WIFIBATTLEMATCH_RND_WORK *cp_wk         ワーク
 */
//-----------------------------------------------------------------------------
static void Util_InitMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  GAMEDATA  *p_game_data  = p_wk->p_param->p_param->p_game_data;
  SAVE_CONTROL_WORK *p_sv            = GAMEDATA_GetSaveControlWork(p_game_data);

  p_my_data->btl_server_version  = BTL_NET_SERVER_VERSION;
  {
    MYSTATUS  *p_my;
    p_my  = GAMEDATA_GetMyStatus(p_game_data);
    GFL_STD_MemCopy( p_my, p_my_data->mystatus, MyStatus_GetWorkSize() );
  }
  { 
    const MYPMS_DATA *cp_mypms;
    cp_mypms        = SaveData_GetMyPmsDataConst( p_sv );
    MYPMS_GetPms( cp_mypms, MYPMS_PMS_TYPE_INTRODUCTION, &p_my_data->pms );
  }
  { 
    if( p_wk->p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      p_my_data->win_cnt  = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      p_my_data->lose_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );
    }
    else
    { 
      p_my_data->win_cnt  = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_FREE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      p_my_data->lose_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_FREE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );
    }
  }
  { 
    //PID
    WIFI_LIST *p_wlist = GAMEDATA_GetWiFiList( p_game_data );
    p_my_data->profileID  = WifiList_GetMyGSID( p_wlist );
  }
#if 0 //sysのprocInitで行っている
  {
    POKEPARTY *p_temoti;
    switch( p_wk->p_param->p_param->poke )
    {
    case WIFIBATTLEMATCH_POKE_TEMOTI:
      p_temoti = GAMEDATA_GetMyPokemon(p_game_data);
      GFL_STD_MemCopy( p_temoti, p_my_data->pokeparty, PokeParty_GetWorkSize() );
      break;

    case WIFIBATTLEMATCH_POKE_BTLBOX:
      { 
        BATTLE_BOX_SAVE *p_btlbox_sv = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
        p_temoti  = BATTLE_BOX_SAVE_MakePokeParty( p_btlbox_sv, GFL_HEAP_LOWID(HEAPID_WIFIBATTLEMATCH_SYS) );
        GFL_STD_MemCopy( p_temoti, p_my_data->pokeparty, PokeParty_GetWorkSize() );
        GFL_HEAP_FreeMemory( p_temoti );
      }
      break;
    }
  }
#endif
}
//----------------------------------------------------------------------------
/**
 *	@brief  自分のデータ更新
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_my_data  ワーク
 *	@param	*p_wk   ワーク
 */
//-----------------------------------------------------------------------------
static void Util_RenewalMyData( WIFIBATTLEMATCH_ENEMYDATA *p_my_data, WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  { 
    if( p_wk->p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      p_my_data->win_cnt  = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      p_my_data->lose_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );

      p_my_data->rate = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_RATE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_RATE );
    }
    else
    { 
      p_my_data->win_cnt  = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_FREE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      p_my_data->lose_cnt = RNDMATCH_GetParam( p_wk->p_param->p_rndmatch, 
          RNDMATCH_TYPE_FREE_SINGLE + p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  サブシーケンス開始
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ワーク
 *	@param	seq_function                      サブシーケンス
 */
//-----------------------------------------------------------------------------
static void Util_SubSeq_Start( WIFIBATTLEMATCH_RND_WORK *p_wk, WBM_SEQ_FUNCTION seq_function )
{ 
  WBM_SEQ_SetNext( p_wk->p_subseq, seq_function );
  p_wk->subseq_ret = WBM_RND_SUBSEQ_RET_NONE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  サブシーケンス中
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk  ワーク
 *	@return WBM_RND_SUBSEQ_RET_NONE動作中  それ以外のときはサブシーケンスが終了し戻り値を返している
 *
 *	@return 戻り値
 */
//-----------------------------------------------------------------------------
static WBM_RND_SUBSEQ_RET Util_SubSeq_Main( WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  WBM_SEQ_Main( p_wk->p_subseq );
  if( WBM_SEQ_IsEnd( p_wk->p_subseq ) )
  {
    return p_wk->subseq_ret;
  }
  else
  { 
    return WBM_RND_SUBSEQ_RET_NONE;
  }
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
static BOOL Util_VerifyPokeData( WIFIBATTLEMATCH_ENEMYDATA *p_data, HEAPID heapID )
{ 
  int i;
  void *p_buff;
  POKEPARTY *p_party  = (POKEPARTY *)p_data->pokeparty;
  const POKEMON_PARAM *cp_pp;
  BOOL ret;

  p_buff  = NHTTP_RAP_EVILCHECK_CreateVerifyPokeBuffer( POKETOOL_GetWorkSize(), PokeParty_GetPokeCount(p_party) , GFL_HEAP_LOWID(heapID) );

  for( i = 0; i < PokeParty_GetPokeCount( p_party ); i++ )
  { 
    cp_pp  = PokeParty_GetMemberPointer( p_party, i);
    NHTTP_RAP_EVILCHECK_AddPokeVerifyPokeBuffer( p_buff, cp_pp, POKETOOL_GetWorkSize(), i );
    OS_TPrintf( "monsno %d\n", PP_Get( cp_pp, ID_PARA_monsno, NULL) );
    OS_TPrintf( "rnd %d\n", PP_Get( cp_pp, ID_PARA_personal_rnd, NULL) );
    OS_TPrintf( "crc %d\n", GFL_STD_CrcCalc( cp_pp, POKETOOL_GetWorkSize() ) );
  }

  ret = NHTTP_RAP_EVILCHECK_VerifySign( p_buff, POKETOOL_GetWorkSize(), PokeParty_GetPokeCount(p_party) , p_data->sign, GFL_HEAP_LOWID(heapID) );

  NHTTP_RAP_EVILCHECK_DeleteVerifyPokeBuffer( p_buff );

  NAGI_Printf( "不正チェック通過？=[%d]\n", ret );
  NAGI_Printf( "パーティ=[%d]\n", PokeParty_GetPokeCount( p_party ) );
  
  { 
    int i;
    for( i = 0; i < NHTTP_RAP_EVILCHECK_RESPONSE_SIGN_LEN; i++ )
    { 
      OS_TPrintf( "%d ", p_data->sign[i] );
      if(i % 0x10 == 0xF )
      { 
        OS_TPrintf( "\n" );
      }
    }
  }


  return ret;

}
//----------------------------------------------------------------------------
/**
 *	@brief  キャンセル処理
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ワーク
 *	@param  is_cancel TRUEならばキャンセルをおこなってよい　FALSEならばダメ
 *	@param  can_disconnect  切断してよいのならばTRUE
 *
 *	@retval キャンセル状態
 */
//-----------------------------------------------------------------------------
static UTIL_CANCEL_STATE Util_Cancel_Seq( WIFIBATTLEMATCH_RND_WORK *p_wk, BOOL is_cancel, BOOL can_disconnect )
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
    if( is_cancel && (GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL) )
    {
      p_wk->cancel_seq++;
    }
    else
    {
      return UTIL_CANCEL_STATE_NONE;
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

  return UTIL_CANCEL_STATE_WAIT;
}


#ifdef BUGFIX_BTS7769_20100713
//----------------------------------------------------------------------------
/**
 *	@brief  キャンセルシーケンスをリセット
 *
 *	@param	WIFIBATTLEMATCH_RND_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_Cancel_ResetSeq( WIFIBATTLEMATCH_RND_WORK *p_wk )
{
  p_wk->cancel_seq  = 0;
  Util_List_Delete( p_wk );
}
#endif //BUGFIX_BTS7769_20100713

//=============================================================================
/**
 *    DEBUG
 */
//=============================================================================
#ifdef DEBUGWIN_USE
static void DEBUGWIN_VisiblePlayerInfo( void* userWork , DEBUGWIN_ITEM* item );
static void DEBUGWIN_VisibleMatchInfo( void* userWork , DEBUGWIN_ITEM* item );
static void DEBUGWIN_ChangePlayerInfo( void* userWork , DEBUGWIN_ITEM* item );
static void DEBUGWIN_ChangeMatchInfo( void* userWork , DEBUGWIN_ITEM* item );
static void DEBUGWIN_ChangeWaitIcon( void* userWork , DEBUGWIN_ITEM* item );


static void DEBUGWIN_CARD_Init( WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  DEBUGWIN_AddGroupToTop( DEBUGWIN_GROUP_CARD, "カード", HEAPID_WIFIBATTLEMATCH_CORE );
  DEBUGWIN_AddItemToGroup( "じぶんカード", 
                              DEBUGWIN_VisiblePlayerInfo,
                              p_wk, 
                              DEBUGWIN_GROUP_CARD, 
                              HEAPID_WIFIBATTLEMATCH_CORE );

  DEBUGWIN_AddItemToGroup( "あいてカード", 
                              DEBUGWIN_VisibleMatchInfo, 
                              p_wk, 
                              DEBUGWIN_GROUP_CARD, 
                              HEAPID_WIFIBATTLEMATCH_CORE );

  DEBUGWIN_AddItemToGroup( "じぶんランク", 
                              DEBUGWIN_ChangePlayerInfo, 
                              p_wk, 
                              DEBUGWIN_GROUP_CARD, 
                              HEAPID_WIFIBATTLEMATCH_CORE );

  DEBUGWIN_AddItemToGroup( "あいてランク", 
                              DEBUGWIN_ChangeMatchInfo, 
                              p_wk, 
                              DEBUGWIN_GROUP_CARD, 
                              HEAPID_WIFIBATTLEMATCH_CORE );

  DEBUGWIN_AddItemToGroup( "ぐるぐる[ON]", 
                              DEBUGWIN_ChangeWaitIcon, 
                              p_wk, 
                              DEBUGWIN_GROUP_CARD, 
                              HEAPID_WIFIBATTLEMATCH_CORE );
}

static void DEBUGWIN_CARD_Exit( WIFIBATTLEMATCH_RND_WORK *p_wk )
{ 
  DEBUGWIN_RemoveGroup( DEBUGWIN_GROUP_CARD );
}

//----------------------------------------------------------------------------
/**
 *	@brief  自分のカード表示
 *
 *	@param	void* userWork  ワーク
 *	@param	item            アイテム
 */
//-----------------------------------------------------------------------------
static void DEBUGWIN_VisiblePlayerInfo( void* userWork , DEBUGWIN_ITEM* item )
{ 
  static const char *scp_name_tbl[] =
  { 
    "レート",
    "フリー",
  };

  WIFIBATTLEMATCH_RND_WORK  *p_wk = userWork;

  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
  { 

    p_wk->playerinfo_mode++;
    p_wk->playerinfo_mode %= NELEMS( scp_name_tbl );

    Util_PlayerInfo_Delete( p_wk );
    Util_PlayerInfo_Create( p_wk, p_wk->playerinfo_mode );

    while( !Util_PlayerInfo_Move( p_wk ) )
    { 
      GFL_BG_VBlankFunc();
      GFL_CLACT_SYS_VBlankFunc();
      PLAYERINFO_PrintMain( p_wk->p_playerinfo, p_wk->p_que );
    }

    DEBUGWIN_ITEM_SetNameV( item , "じぶんカード[%s]", scp_name_tbl[ p_wk->playerinfo_mode ] );
    DEBUGWIN_RefreshScreen();
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  対戦者のカード表示
 *
 *	@param	void* userWork  ワーク
 *	@param	item            アイテム
 */
//-----------------------------------------------------------------------------
static void DEBUGWIN_VisibleMatchInfo( void* userWork , DEBUGWIN_ITEM* item )
{ 

  WIFIBATTLEMATCH_RND_WORK  *p_wk = userWork;

  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
  { 
    WIFIBATTLEMATCH_DATA_ModifiEnemyData( p_wk->p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );

    Util_MatchInfo_Delete( p_wk );
    Util_MatchInfo_Create( p_wk, p_wk->p_param->p_enemy_data );

    while( !Util_MatchInfo_Main( p_wk ) )
    { 
      GFL_BG_VBlankFunc();
      GFL_CLACT_SYS_VBlankFunc();
      MATCHINFO_PrintMain( p_wk->p_matchinfo, p_wk->p_que );
    }

  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  自分のカード切り替え
 *
 *	@param	void* userWork  ワーク
 *	@param	item            アイテム
 */
//-----------------------------------------------------------------------------
static void DEBUGWIN_ChangePlayerInfo( void* userWork , DEBUGWIN_ITEM* item )
{ 
  static const char *scp_name_tbl[] =
  { 
    "ブラック",
    "ゴールド",
    "シルバー",
    "ブロンズ",
    "カッパー",
    "ノーマル",
  };

  WIFIBATTLEMATCH_RND_WORK  *p_wk = userWork;
  u32 win;

  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
  { 

    p_wk->playerinfo_rank++;
    p_wk->playerinfo_rank %= NELEMS( scp_name_tbl );

    switch( p_wk->playerinfo_rank )
    { 
    case 0: //ブラック
      win = WBM_CARD_WIN_BLACK;
      break;
    case 1:  //ゴールド
      win = WBM_CARD_WIN_GOLD;
      break;
    case 2: //シルバー
      win = WBM_CARD_WIN_SILVER;
      break;
    case 3: //ブロンズ
      win = WBM_CARD_WIN_BRONZE;
      break;
    case 4: //カッパー
      win = WBM_CARD_WIN_COPPER;
      break;
    case 5: //ノーマル
      win = WBM_CARD_WIN_NORMAL;
      break;
    }
    p_wk->p_param->p_rnd_sake_data->single_win  = win;
    p_wk->p_param->p_rnd_sake_data->double_win  = win;
    p_wk->p_param->p_rnd_sake_data->triple_win  = win;
    p_wk->p_param->p_rnd_sake_data->rot_win  = win;
    p_wk->p_param->p_rnd_sake_data->shooter_win  = win;
    RNDMATCH_SetParam( p_wk->p_param->p_rndmatch, p_wk->p_param->p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN, win );

    Util_PlayerInfo_Delete( p_wk );
    Util_PlayerInfo_Create( p_wk, p_wk->playerinfo_mode );

    while( !Util_PlayerInfo_Move( p_wk ) )
    { 
      GFL_BG_VBlankFunc();
      GFL_CLACT_SYS_VBlankFunc();
      PLAYERINFO_PrintMain( p_wk->p_playerinfo, p_wk->p_que );
    }

    DEBUGWIN_ITEM_SetNameV( item , "じぶんランク[%s]", scp_name_tbl[ p_wk->playerinfo_rank ] );
    DEBUGWIN_RefreshScreen();
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  対戦者のカード切り替え
 *
 *	@param	void* userWork  ワーク
 *	@param	item            アイテム
 */
//-----------------------------------------------------------------------------
static void DEBUGWIN_ChangeMatchInfo( void* userWork , DEBUGWIN_ITEM* item )
{ 
  static const char *scp_name_tbl[] =
  { 
    "ブラック",
    "ゴールド",
    "シルバー",
    "ブロンズ",
    "カッパー",
    "ノーマル",
  };

  WIFIBATTLEMATCH_RND_WORK  *p_wk = userWork;
  u32 win;

  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
  { 
    p_wk->matchinfo_rank++;
    p_wk->matchinfo_rank %= NELEMS( scp_name_tbl );

    switch( p_wk->matchinfo_rank )
    { 
    case 0: //ブラック
      win = WBM_CARD_WIN_BLACK;
      break;
    case 1:  //ゴールド
      win = WBM_CARD_WIN_GOLD;
      break;
    case 2: //シルバー
      win = WBM_CARD_WIN_SILVER;
      break;
    case 3: //ブロンズ
      win = WBM_CARD_WIN_BRONZE;
      break;
    case 4: //カッパー
      win = WBM_CARD_WIN_COPPER;
      break;
    case 5: //ノーマル
      win = WBM_CARD_WIN_NORMAL;
      break;
    }
    p_wk->p_param->p_enemy_data->win_cnt = win;

    WIFIBATTLEMATCH_DATA_ModifiEnemyData( p_wk->p_param->p_enemy_data, HEAPID_WIFIBATTLEMATCH_CORE );

    Util_MatchInfo_Delete( p_wk );
    Util_MatchInfo_Create( p_wk, p_wk->p_param->p_enemy_data );

    while( !Util_MatchInfo_Main( p_wk ) )
    { 
      GFL_BG_VBlankFunc();
      GFL_CLACT_SYS_VBlankFunc();
      MATCHINFO_PrintMain( p_wk->p_matchinfo, p_wk->p_que );
    }

    DEBUGWIN_ITEM_SetNameV( item , "あいてランク[%s]", scp_name_tbl[ p_wk->matchinfo_rank ] );
    DEBUGWIN_RefreshScreen();
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  アイコン表示
 *
 *	@param	void* userWork  ワーク
 *	@param	item            アイテム
 */
//-----------------------------------------------------------------------------
static void DEBUGWIN_ChangeWaitIcon( void* userWork , DEBUGWIN_ITEM* item )
{
  static const char *scp_name_tbl[] =
  { 
    "OFF",
    "ON",
  };

  if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_A )
  { 
    WIFIBATTLEMATCH_RND_WORK  *p_wk = userWork;

    p_wk->waiticon_draw++;
    p_wk->waiticon_draw %= NELEMS( scp_name_tbl );

    switch( p_wk->waiticon_draw )
    { 
    case 0:
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, FALSE );
      break;
    case 1:
      WBM_WAITICON_SetDrawEnable( p_wk->p_wait, TRUE );
      break;
    }

    DEBUGWIN_ITEM_SetNameV( item , "ぐるぐる[%s]", scp_name_tbl[ p_wk->waiticon_draw ] );
    DEBUGWIN_RefreshScreen();
  }
}




#endif //DEBUGWIN_USE
