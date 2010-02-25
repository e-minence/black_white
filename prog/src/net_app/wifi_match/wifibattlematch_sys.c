//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_sys.c
 *	@brief  各プロセスのつなぎ
 *	@author	Toru=Nagihashi
 *	@data		2009.11.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>

//システム
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID
#include "poke_tool/pokeparty.h"
#include "gamesystem/btl_setup.h"
#include "print/str_tool.h"
#include "net/network_define.h"
#include "poke_tool/poke_regulation.h"
#include "sound/pm_sndsys.h"

//各プロセス
#include "net_app/wifi_login.h"
#include "net_app/wifi_logout.h"
#include "title/title.h"
#include "wifibattlematch_battle.h"
#include "net_app/btl_rec_sel.h"
#include "wifibattlematch_utilproc.h"
#include "wifibattlematch_subproc.h"

//セーブデータ
#include "savedata/battle_box_save.h"
#include "savedata/wifihistory.h"
#include "savedata/my_pms_data.h"

//自分のモジュール
#include "wifibattlematch_core.h"
#include "wifibattlematch_data.h"
#include "wifibattlematch_snd.h"

//外部公開
#include "net_app/wifibattlematch.h"

//-------------------------------------
///	DEBUG
//=====================================
#ifdef PM_DEBUG
#endif //PM_DEBUG

//-------------------------------------
///	PROCエクスターン
//=====================================

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
#define WIFIBATTLEMATCH_MEMBER_NUM  (2)

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	サブプロック移動
//=====================================
typedef void *(*SUBPROC_ALLOC_FUNCTION)( HEAPID heapID, void *p_wk_adrs );
typedef BOOL (*SUBPROC_FREE_FUNCTION)( void *p_param, void *p_wk_adrs );
typedef struct 
{
	FSOverlayID							ov_id;
	const GFL_PROC_DATA			*cp_procdata;
	SUBPROC_ALLOC_FUNCTION	alloc_func;
	SUBPROC_FREE_FUNCTION		free_func;
} SUBPROC_DATA;
typedef struct {
	GFL_PROCSYS			*p_procsys;
	void						*p_proc_param;
	SUBPROC_DATA		*p_data;

	HEAPID					heapID;
	void						*p_wk_adrs;
	const SUBPROC_DATA			*cp_procdata_tbl;

	u8							next_procID;
	u8							now_procID;
	u16							seq;

  GFL_PROC_MAIN_STATUS  status;
} SUBPROC_WORK;
//-------------------------------------
///	システムワーク
//=====================================
typedef struct
{ 
  BOOL                        is_create_gamedata;
  WIFIBATTLEMATCH_PARAM       param;
  SUBPROC_WORK                subproc;

  //コアモード
  WIFIBATTLEMATCH_CORE_MODE     core_mode;
  WIFIBATTLEMATCH_CORE_RETMODE  core_ret;

  //バトルの結果
  BtlResult                   btl_result;
  BtlRule                     btl_rule;

  POKEPARTY                   *p_player_btl_party; //自分で決めたパーティ
  POKEPARTY                   *p_enemy_btl_party; //相手の決めたパーティ

  //以下システム層に置いておくデータ
  WIFIBATTLEMATCH_ENEMYDATA   *p_player_data;
  WIFIBATTLEMATCH_ENEMYDATA   *p_enemy_data;

  DWCSvlResult                svl_result;

#if 0
  DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA *p_gpf_data;
  WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA   *p_sake_data;
#endif

} WIFIBATTLEMATCH_SYS;

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
///	プロセス
//=====================================
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	サブプロセス
//=====================================
static void SUBPROC_Init( SUBPROC_WORK *p_wk, const SUBPROC_DATA *cp_procdata_tbl, void *p_wk_adrs, HEAPID heapID );
static BOOL SUBPROC_Main( SUBPROC_WORK *p_wk );
static GFL_PROC_MAIN_STATUS SUBPROC_GetStatus( const SUBPROC_WORK *cp_wk );
static void SUBPROC_Exit( SUBPROC_WORK *p_wk );
static void SUBPROC_CallProc( SUBPROC_WORK *p_wk, u32 procID );

//-------------------------------------
///	サブプロセス用引数の解放、破棄関数
//=====================================
//マッチングメインプロセス
static void *WBM_CORE_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL WBM_CORE_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//リスト＋ステータス
static void *POKELIST_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL POKELIST_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//バトル＋デモ
static void *BATTLE_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL BATTLE_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//WIFIログイン
static void *LOGIN_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL LOGIN_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//リスト＋ステータス〜バトル＋デモへのつなぎプロセス
static void *WBM_LISTAFTER_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL WBM_LISTAFTER_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//録画
static void *WBM_BTLREC_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL WBM_BTLREC_FreeParam( void *p_param_adrs, void *p_wk_adrs );
//WIFIログアウト
static void *LOGOUT_AllocParam( HEAPID heapID, void *p_wk_adrs );
static BOOL LOGOUT_FreeParam( void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	データバッファ作成
//=====================================
static void DATA_CreateBuffer( WIFIBATTLEMATCH_SYS *p_wk, HEAPID heapID );
static void DATA_DeleteBuffer( WIFIBATTLEMATCH_SYS *p_wk );

//=============================================================================
/**
 *					外部参照
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	WifiBattleMatch_ProcData =
{	
	WIFIBATTLEMATCH_PROC_Init,
	WIFIBATTLEMATCH_PROC_Main,
	WIFIBATTLEMATCH_PROC_Exit,
};

//=============================================================================
/**
 *					データ
*/
//=============================================================================
//-------------------------------------
///	サブプロセス移動データ
//=====================================
typedef enum
{	
	SUBPROCID_CORE,
  SUBPROCID_POKELIST,
  SUBPROCID_BATTLE,
  SUBPROCID_LOGIN,
  SUBPROCID_LISTAFTER,
  SUBPROCID_BTLREC,
  SUBPROCID_LOGOUT,

	SUBPROCID_MAX
} SUBPROC_ID;
static const SUBPROC_DATA sc_subproc_data[SUBPROCID_MAX]	=
{	
	//SUBPROCID_CORE
	{	
		FS_OVERLAY_ID( wifibattlematch_core ),
		&WifiBattleMatchCore_ProcData,
		WBM_CORE_AllocParam,
		WBM_CORE_FreeParam,
	},
  //SUBPROCID_POKELIST,
  { 
	  NO_OVERLAY_ID,
    &WifiBattleMatch_Sub_ProcData,
    POKELIST_AllocParam,
    POKELIST_FreeParam,
  },
  //SUBPROCID_BATTLE,
  { 
	  NO_OVERLAY_ID,
    &WifiBattleMatch_BattleLink_ProcData,
    BATTLE_AllocParam,
    BATTLE_FreeParam,
  },
  //SUBPROCID_LOGIN
  { 
    FS_OVERLAY_ID( wifi_login ),
    &WiFiLogin_ProcData,
    LOGIN_AllocParam,
    LOGIN_FreeParam,
  },
  //SUBPROCID_LISTAFTER,
  { 
		FS_OVERLAY_ID( wifibattlematch_core ),
		&WifiBattleMatch_ListAfter_ProcData,
		WBM_LISTAFTER_AllocParam,
		WBM_LISTAFTER_FreeParam,
  },
  //SUBPROCID_BTLREC
  { 
    FS_OVERLAY_ID( btl_rec_sel ),
    &BTL_REC_SEL_ProcData,
    WBM_BTLREC_AllocParam,
    WBM_BTLREC_FreeParam,
  },
  //SUBPROCID_LOGOUT
  { 
    FS_OVERLAY_ID( wifi_login ),
    &WiFiLogout_ProcData,
    LOGOUT_AllocParam,
    LOGOUT_FreeParam,
  },
};

//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFIバトルマッチ画面	メインプロセス初期化
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  WIFIBATTLEMATCH_SYS   *p_wk;
  HEAPID  parentID;

  { 
    WIFIBATTLEMATCH_PARAM *p_param  = p_param_adrs;
    if( p_param->mode == WIFIBATTLEMATCH_MODE_RANDOM )
    { 
      //ランダムマッチはポケセンWIFIカウンターから入り、
      //ゲームシステム等でメモリを食っているので、HEAPID_PROCにシステムをおく
      parentID  = HEAPID_PROC;
    }
    else
    { 
      //それ以外は、タイトル画面からくるためHEAPID_APPが潤沢にあるので、
      //HEAPID_APPからもらう
      parentID  = GFL_HEAPID_APP;
    }
  }
  
  NAGI_Printf( "work %d + %d *2\n", sizeof(WIFIBATTLEMATCH_SYS), sizeof(WIFIBATTLEMATCH_ENEMYDATA) );

	//ヒープ作成
	GFL_HEAP_CreateHeap( parentID, HEAPID_WIFIBATTLEMATCH_SYS, 0x7000 );

	//プロセスワーク作成
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFIBATTLEMATCH_SYS), HEAPID_WIFIBATTLEMATCH_SYS );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_SYS) );
  GFL_STD_MemCopy( p_param_adrs, &p_wk->param, sizeof(WIFIBATTLEMATCH_PARAM) );
  if( p_wk->param.p_game_data == NULL )
  { 
    p_wk->param.p_game_data = GAMEDATA_Create( GFL_HEAPID_APP );
    p_wk->is_create_gamedata  = TRUE;
  }
  if( p_wk->p_player_btl_party == NULL )
  { 
    p_wk->p_player_btl_party = PokeParty_AllocPartyWork( HEAPID_WIFIBATTLEMATCH_SYS );
  }
  if( p_wk->p_enemy_btl_party == NULL )
  { 
    p_wk->p_enemy_btl_party = PokeParty_AllocPartyWork( HEAPID_WIFIBATTLEMATCH_SYS );
  }

  OS_Printf( "ランダムマッチ引数 使用ポケ%d ルール%d\n", p_wk->param.poke, p_wk->param.btl_rule );

  p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_START;

  //データバッファ作成
  DATA_CreateBuffer( p_wk, HEAPID_WIFIBATTLEMATCH_SYS );

  //モジュール作成
	SUBPROC_Init( &p_wk->subproc, sc_subproc_data, p_wk, HEAPID_WIFIBATTLEMATCH_SYS );
  SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_LOGIN );

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFIバトルマッチ画面	メインプロセス破棄
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  WIFIBATTLEMATCH_SYS   *p_wk     = p_wk_adrs;

	//モジュール破棄
	SUBPROC_Exit( &p_wk->subproc );

  //データバッファ破棄
  DATA_DeleteBuffer( p_wk );

  if( p_wk->p_enemy_btl_party )
  { 
    GFL_HEAP_FreeMemory( p_wk->p_enemy_btl_party );
    p_wk->p_enemy_btl_party = NULL;
  }
  if( p_wk->p_player_btl_party )
  { 
    GFL_HEAP_FreeMemory( p_wk->p_player_btl_party );
    p_wk->p_player_btl_party = NULL;
  }

  if( p_wk->is_create_gamedata )
  { 
    GAMEDATA_Delete( p_wk->param.p_game_data );
  }

  //ランダム対戦以外はタイトルから呼ばれるので、戻りはタイトル
  if( p_wk->param.mode != WIFIBATTLEMATCH_MODE_RANDOM )
  { 
    GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);
  }

  if( p_wk->param.is_auto_release )
  { 
    GFL_HEAP_FreeMemory( p_param_adrs );
  }

  //プロセスワーク破棄
	GFL_PROC_FreeWork( p_proc );

	//ヒープ破棄
	GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_SYS );

	return GFL_PROC_RES_FINISH;
}

#include "system/net_err.h"
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
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
	enum
	{	
		WBM_SYS_SEQ_INIT,
		WBM_SYS_SEQ_FADEIN,
		WBM_SYS_SEQ_FADEIN_WAIT,
		WBM_SYS_SEQ_MAIN,
		WBM_SYS_SEQ_FADEOUT,
		WBM_SYS_SEQ_FADEOUT_WAIT,
		WBM_SYS_SEQ_EXIT,
	};

  WIFIBATTLEMATCH_SYS   *p_wk     = p_wk_adrs;

  //シーケンス
	switch( *p_seq )
	{	
	case WBM_SYS_SEQ_INIT:
		*p_seq	= WBM_SYS_SEQ_MAIN;
		break;

	case WBM_SYS_SEQ_FADEIN:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= WBM_SYS_SEQ_FADEIN_WAIT;
		break;

	case WBM_SYS_SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= WBM_SYS_SEQ_MAIN;
		}
		break;

	case WBM_SYS_SEQ_MAIN:
		{
			BOOL is_end;
			is_end	= SUBPROC_Main( &p_wk->subproc );

			if( is_end )
			{	
				*p_seq	= WBM_SYS_SEQ_EXIT;
			}

      //このローカルPROC内でPROC遷移するため、エラーシステムを自分で動かす
      //if( p_wk->param.mode == WIFIBATTLEMATCH_MODE_WIFI )
      { 
        const GFL_PROC_MAIN_STATUS  status  = SUBPROC_GetStatus( &p_wk->subproc );
        if( status == GFL_PROC_MAIN_CHANGE || status == GFL_PROC_MAIN_NULL )
        { 
          NetErr_DispCall(FALSE);
        }
      }
		}
		break;

	case WBM_SYS_SEQ_FADEOUT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= WBM_SYS_SEQ_FADEOUT_WAIT;
		break;

	case WBM_SYS_SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= WBM_SYS_SEQ_EXIT;
		}
		break;

	case WBM_SYS_SEQ_EXIT:
		return GFL_PROC_RES_FINISH;
	}
	return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
 *			SUBPROCシステム
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SUBPROCシステム	初期化
 *
 *	@param	SUBPROC_WORK *p_wk	ワーク
 *	@param	cp_procdata_tbl			プロセス接続テーブル
 *	@param	void *p_wk_adrs			アロック関数と解放関数に渡すワーク
 *	@param	heapID							システム構築用ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_Init( SUBPROC_WORK *p_wk, const SUBPROC_DATA *cp_procdata_tbl, void *p_wk_adrs, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(SUBPROC_WORK) );
	p_wk->p_procsys				= GFL_PROC_LOCAL_boot( heapID );
	p_wk->p_wk_adrs				= p_wk_adrs;
	p_wk->cp_procdata_tbl	= cp_procdata_tbl;
	p_wk->heapID					= heapID;
}

//----------------------------------------------------------------------------
/**
 *	@brief	SUBPROCシステム	メイン処理
 *
 *	@param	SUBPROC_WORK *p_wk	ワーク
 *
 *	@retval	TRUEならば終了	FALSEならばPROCが存在する
 */
//-----------------------------------------------------------------------------
static BOOL SUBPROC_Main( SUBPROC_WORK *p_wk )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_ALLOC_PARAM,
		SEQ_MAIN,
		SEQ_FREE_PARAM,
    SEQ_NEXT,
		SEQ_END,
	};

	switch( p_wk->seq )
	{	
	case SEQ_INIT:
		p_wk->now_procID	= p_wk->next_procID;
		p_wk->seq	= SEQ_ALLOC_PARAM;
		break;

	case SEQ_ALLOC_PARAM:
		//プロセス引数作成関数があれば呼び出し
		if( p_wk->cp_procdata_tbl[ p_wk->now_procID ].alloc_func )
		{	
			p_wk->p_proc_param	= p_wk->cp_procdata_tbl[ p_wk->now_procID ].alloc_func(
					p_wk->heapID, p_wk->p_wk_adrs );
		}
		else
		{	
			p_wk->p_proc_param	= NULL;
		}

		//プロック呼び出し
		GFL_PROC_LOCAL_CallProc( p_wk->p_procsys, p_wk->cp_procdata_tbl[	p_wk->now_procID ].ov_id,
					p_wk->cp_procdata_tbl[	p_wk->now_procID ].cp_procdata, p_wk->p_proc_param );

		p_wk->seq	= SEQ_MAIN;
		break;

	case SEQ_MAIN:
		{	
			p_wk->status	= GFL_PROC_LOCAL_Main( p_wk->p_procsys );
			if( GFL_PROC_MAIN_NULL == p_wk->status )
			{	
				p_wk->seq	= SEQ_FREE_PARAM;
			}
		}
		break;

	case SEQ_FREE_PARAM:
		//プロセス引数破棄関数呼び出し
		if( p_wk->cp_procdata_tbl[	p_wk->now_procID ].free_func )
		{	
			if( p_wk->cp_procdata_tbl[	p_wk->now_procID ].free_func( p_wk->p_proc_param, p_wk->p_wk_adrs ) )
      { 
        p_wk->p_proc_param	= NULL;
				p_wk->seq	= SEQ_NEXT;
      }
		}
    else
    { 
				p_wk->seq	= SEQ_NEXT;
    }
    break;

  case SEQ_NEXT:
		//もし次のプロセスがあれば呼び出し
		//なければ終了
		if( p_wk->now_procID != p_wk->next_procID )
		{	
			p_wk->seq	= SEQ_INIT;
		}
		else
		{	
			p_wk->seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  SUBPROCシステム PROC状態を取得
 *
 *	@param	const SUBPROC_WORK *cp_wk   ワーク
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static GFL_PROC_MAIN_STATUS SUBPROC_GetStatus( const SUBPROC_WORK *cp_wk )
{ 
  return cp_wk->status;
}

//----------------------------------------------------------------------------
/**
 *	@brief	SUBPROCシステム	破棄
 *
 *	@param	SUBPROC_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_Exit( SUBPROC_WORK *p_wk )
{	
	GF_ASSERT( p_wk->p_proc_param == NULL );

	GFL_PROC_LOCAL_Exit( p_wk->p_procsys );
	GFL_STD_MemClear( p_wk, sizeof(SUBPROC_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	SUBPROCシステム	プロセスリクエスト
 *
 *	@param	SUBPROC_WORK *p_wk	ワーク
 *	@param	proc_id							呼ぶプロセスID
 *
 */
//-----------------------------------------------------------------------------
static void SUBPROC_CallProc( SUBPROC_WORK *p_wk, u32 procID )
{	
	p_wk->next_procID	= procID;
}

//=============================================================================
/**
 *		サブプロセス用作成破棄
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFIマッチングコアの引数	作成
 *
 *	@param	HEAPID heapID			ヒープID
 *	@param	*p_wk_adrs				ワーク
 *
 *	@return	引数
 */
//-----------------------------------------------------------------------------
static void *WBM_CORE_AllocParam( HEAPID heapID, void *p_wk_adrs )
{	

  WIFIBATTLEMATCH_CORE_PARAM  *p_param;
  WIFIBATTLEMATCH_SYS         *p_wk     = p_wk_adrs;

  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_CORE_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_CORE_PARAM) );
	p_param->p_param	      = &p_wk->param;
  p_param->mode           = p_wk->core_mode;
  p_param->retmode        = p_wk->core_ret;
  p_param->btl_result     = p_wk->btl_result;
  p_param->p_player_data  = p_wk->p_player_data;
  p_param->p_enemy_data   = p_wk->p_enemy_data;
  p_param->p_svl_result   = &p_wk->svl_result;
  p_param->p_rndmatch     = SaveData_GetRndMatch( GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data ) );

  PMSND_PlayBGM( WBM_SND_SEQ_MAIN );
		
	return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	WIFIマッチングコアの引数	破棄
 *
 *	@param	void *p_param_adrs				引数
 *	@param	*p_wk_adrs								ワーク
 */
//-----------------------------------------------------------------------------
static BOOL WBM_CORE_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{	
  WIFIBATTLEMATCH_SYS         *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_param_adrs;

  p_wk->core_ret = p_param->retmode;
  switch( p_param->result )
  { 
  case WIFIBATTLEMATCH_CORE_RESULT_NEXT_BATTLE:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_POKELIST );
    break;
    
  case WIFIBATTLEMATCH_CORE_RESULT_NEXT_REC:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_BTLREC );
    break;
  
  case WIFIBATTLEMATCH_CORE_RESULT_ERR_NEXT_LOGIN:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_LOGIN );
    break;

  case WIFIBATTLEMATCH_CORE_RESULT_FINISH:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_LOGOUT );
    break;
  }

	GFL_HEAP_FreeMemory( p_param );

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	リストの引数	作成
 *
 *	@param	HEAPID heapID			ヒープID
 *	@param	*p_wk_adrs				ワーク
 *
 *	@return	引数
 */
//-----------------------------------------------------------------------------
static void *POKELIST_AllocParam( HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SUBPROC_PARAM    *p_param;
  WIFIBATTLEMATCH_SYS               *p_wk   = p_wk_adrs;
  int reg_no;

  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_SUBPROC_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_SUBPROC_PARAM) );

  if( p_wk->param.mode == WIFIBATTLEMATCH_MODE_RANDOM )
  { 

    switch( p_wk->param.btl_rule  )
    { 
    case WIFIBATTLEMATCH_BTLRULE_SINGLE:
      reg_no  = REG_RND_SINGLE;
      break;
    case WIFIBATTLEMATCH_BTLRULE_DOUBLE:
      reg_no  = REG_RND_DOUBLE;
      break;
    case WIFIBATTLEMATCH_BTLRULE_TRIPLE:
      reg_no  = REG_RND_TRIPLE;
      break;
    case WIFIBATTLEMATCH_BTLRULE_ROTATE:
      reg_no  = REG_RND_ROTATION;
      break;
    case WIFIBATTLEMATCH_BTLRULE_SHOOTER:
      reg_no  = REG_RND_TRIPLE_SHOOTER;
      break;
    }

    p_param->regulation = (REGULATION*)PokeRegulation_LoadDataAlloc( reg_no, heapID );
  }
  else if( p_wk->param.mode == WIFIBATTLEMATCH_MODE_WIFI )
  { 
    SAVE_CONTROL_WORK *p_sv   = GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data );
    p_param->regulation       = SaveData_GetRegulation( p_sv,0 );
  }
  else
  { 
    GF_ASSERT(0);
  }

  p_param->p_party    = p_wk->p_player_btl_party;
  p_param->gameData   = p_wk->param.p_game_data;

  //自分のデータ
  { 
    int i;
    WIFIBATTLEMATCH_ENEMYDATA *p_player;
    POKEPARTY *p_party;
    POKEMON_PARAM *pp;

    p_player = p_wk->p_player_data;
    p_party  = (POKEPARTY*)p_player->pokeparty;
    for( i = 0; i < PokeParty_GetPokeCount( p_party ); i++ )
    {
      pp = PokeParty_GetMemberPointer( p_party, i );
      p_param->ppp[i] = PP_GetPPPPointer( pp );
    }
  }

  //敵データ
  { 
    WIFIBATTLEMATCH_ENEMYDATA *p_enemy;
    POKEPARTY *p_party;

    p_enemy = p_wk->p_enemy_data;
    p_param->enemyName      = MyStatus_GetMyName( (MYSTATUS*)p_enemy->mystatus );
    p_param->enemyPokeParty = (POKEPARTY*)p_enemy->pokeparty;
    p_param->enemySex       = MyStatus_GetMySex( (MYSTATUS*)p_enemy->mystatus );
  }

	return p_param;

}
//----------------------------------------------------------------------------
/**
 *	@brief  リストの引数	破棄
 *
 *	@param	void *p_param_adrs				引数
 *	@param	*p_wk_adrs								ワーク
 */
//-----------------------------------------------------------------------------
static BOOL POKELIST_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_SUBPROC_PARAM   *p_param  = p_param_adrs;

  if( p_param->result == WIFIBATTLEMATCH_SUBPROC_RESULT_SUCCESS )
  { 
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_LISTAFTER );
  }
  else if( p_param->result == WIFIBATTLEMATCH_SUBPROC_RESULT_ERROR_NEXT_LOGIN )
  { 
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_LOGIN );
  }
  GFL_HEAP_FreeMemory( p_param->regulation );
  GFL_HEAP_FreeMemory( p_param );

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	バトルの引数	作成
 *
 *	@param	HEAPID heapID			ヒープID
 *	@param	*p_wk_adrs				ワーク
 *
 *	@return	引数
 */
//-----------------------------------------------------------------------------
static void *BATTLE_AllocParam( HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_BATTLELINK_PARAM  *p_param;
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  
  //デモバトル接続ワーク
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_BATTLELINK_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_BATTLELINK_PARAM) );
  p_param->p_gamedata  = p_wk->param.p_game_data;

  //デモパラメータ
  p_param->p_demo_param = GFL_HEAP_AllocMemory( heapID, sizeof(COMM_BTL_DEMO_PARAM) );
	GFL_STD_MemClear( p_param->p_demo_param, sizeof(COMM_BTL_DEMO_PARAM) );

  //デモパラメータへの設定
  //自分
  {
    COMM_BTL_DEMO_TRAINER_DATA *p_tr;
    p_tr  = &p_param->p_demo_param->trainer_data[ COMM_BTL_DEMO_TRDATA_A ];
    p_tr->mystatus  = (MYSTATUS*)p_wk->p_player_data->mystatus;
    p_tr->party     = p_wk->p_player_btl_party;
    p_tr->server_version  = p_wk->p_player_data->btl_server_version;
  }
  //相手
  { 
    COMM_BTL_DEMO_TRAINER_DATA *p_tr;
    p_tr  = &p_param->p_demo_param->trainer_data[ COMM_BTL_DEMO_TRDATA_B ];
    p_tr->mystatus  = (MYSTATUS*)p_wk->p_enemy_data->mystatus;
    p_tr->party     = p_wk->p_enemy_btl_party;
    p_tr->server_version  = p_wk->p_enemy_data->btl_server_version;
  }

  //バトル設定パラメータ
  p_param->p_btl_setup_param	= GFL_HEAP_AllocMemory( heapID, sizeof(BATTLE_SETUP_PARAM) );
	GFL_STD_MemClear( p_param->p_btl_setup_param, sizeof(BATTLE_SETUP_PARAM) );

  GFL_OVERLAY_Load( FS_OVERLAY_ID( battle ) );

	//ランダムバトルのバトル設定
  if( p_wk->param.mode == WIFIBATTLEMATCH_MODE_RANDOM )
  { 
    switch( p_wk->param.btl_rule )
    {
    case WIFIBATTLEMATCH_BTLRULE_SINGLE:    ///< シングル
      BTL_SETUP_Single_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case WIFIBATTLEMATCH_BTLRULE_DOUBLE:    ///< ダブル
      BTL_SETUP_Double_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case WIFIBATTLEMATCH_BTLRULE_TRIPLE:    ///< トリプル
      BTL_SETUP_Triple_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case WIFIBATTLEMATCH_BTLRULE_ROTATE:  ///< ローテーション
      BTL_SETUP_Rotation_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;
    case WIFIBATTLEMATCH_BTLRULE_SHOOTER:  ///< シューター
      BTL_SETUP_Triple_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      //@todo 
      GF_ASSERT( 0 );

      break;
    }
  }
  //WiFI大会のバトル設定
  else if( p_wk->param.mode == WIFIBATTLEMATCH_MODE_WIFI )
  { 
    SAVE_CONTROL_WORK *p_sv   = GAMEDATA_GetSaveControlWork( p_wk->param.p_game_data );
    REGULATION* p_reg         = SaveData_GetRegulation( p_sv,0 );

    switch( Regulation_GetParam( p_reg, REGULATION_BATTLETYPE ) )
    {
    case REGULATION_BATTLE_SINGLE:    ///< シングル
      BTL_SETUP_Single_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case REGULATION_BATTLE_DOUBLE:    ///< ダブル
      BTL_SETUP_Double_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case REGULATION_BATTLE_TRIPLE:    ///< トリプル
      BTL_SETUP_Triple_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

    case REGULATION_BATTLE_ROTATION:  ///< ローテーション
      BTL_SETUP_Rotation_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;

//    case REGULATION_BATTLE_SH:    ///< シューター
      default:
      //@todo
      BTL_SETUP_Triple_Comm( p_param->p_btl_setup_param, p_wk->param.p_game_data, 
          GFL_NET_HANDLE_GetCurrentHandle() , BTL_COMM_WIFI, heapID );
      break;
    }
  }
  else
  { 
    GF_ASSERT(0);
  }


  BATTLE_PARAM_SetPokeParty( p_param->p_btl_setup_param, p_wk->p_player_btl_party, BTL_CLIENT_PLAYER ); 

  //録画準備
  BTL_SETUP_AllocRecBuffer( p_param->p_btl_setup_param, heapID );

  GFL_OVERLAY_Unload( FS_OVERLAY_ID( battle ) );

  //録画情報にバトル情報を設定
  BattleRec_Init( HEAPID_WIFIBATTLEMATCH_SYS );
  BattleRec_LoadToolModule();
  BattleRec_StoreSetupParam( p_param->p_btl_setup_param );
  BattleRec_UnloadToolModule();

  //デモパラメータ
	return p_param;

}
//----------------------------------------------------------------------------
/**
 *	@brief	バトルの引数	破棄
 *
 *	@param	void *p_param_adrs				引数
 *	@param	*p_wk_adrs								ワーク
 */
//-----------------------------------------------------------------------------
static BOOL BATTLE_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_BATTLELINK_PARAM  *p_param  = p_param_adrs;
  BATTLE_SETUP_PARAM  *p_btl_param  = p_param->p_btl_setup_param;
  WIFIBATTLEMATCH_BATTLELINK_RESULT result  = p_param->result;

  //受け取り
  p_wk->btl_result  = p_btl_param->result;
  p_wk->btl_rule  = p_btl_param->rule;
  OS_FPrintf( 3, "バトル結果 %d \n", p_wk->btl_result);

  //破棄
  BATTLE_PARAM_Release( p_param->p_btl_setup_param );
	GFL_HEAP_FreeMemory( p_param->p_btl_setup_param );
	GFL_HEAP_FreeMemory( p_param->p_demo_param );
	GFL_HEAP_FreeMemory( p_param );

  //次のPROC
  switch( result )
  {
  case WIFIBATTLEMATCH_BATTLELINK_RESULT_SUCCESS:
    p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE;
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );
    break;

  case WIFIBATTLEMATCH_BATTLELINK_RESULT_DISCONNECT:
    //切断された
    p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );
    break;
    
  case WIFIBATTLEMATCH_BATTLELINK_RESULT_EVIL:
    p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE_ERR;
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );
    break;
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	WIFIログインの引数	作成
 *
 *	@param	HEAPID heapID			ヒープID
 *	@param	*p_wk_adrs				ワーク
 *
 *	@return	引数
 */
//-----------------------------------------------------------------------------
static void *LOGIN_AllocParam( HEAPID heapID, void *p_wk_adrs )
{ 
  WIFILOGIN_PARAM *p_param;
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFILOGIN_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFILOGIN_PARAM) );

  p_param->gamedata = p_wk->param.p_game_data;
  p_param->bg       = WIFILOGIN_BG_NORMAL;
  p_param->display  = WIFILOGIN_DISPLAY_UP;
  p_param->pSvl     = &p_wk->svl_result;
  p_param->nsid     = WB_NET_WIFIMATCH;

  return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ログインの引数	破棄
 *
 *	@param	void *p_param_adrs				引数
 *	@param	*p_wk_adrs								ワーク
 */
//-----------------------------------------------------------------------------
static BOOL LOGIN_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  WIFILOGIN_PARAM  *p_param     = p_param_adrs;
  WIFILOGIN_RESULT  result      = p_param->result;

  GFL_HEAP_FreeMemory( p_param );

  switch( result )
  { 
  case WIFILOGIN_RESULT_LOGIN:
    p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_START;
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );
    break;

  case WIFILOGIN_RESULT_CANCEL:
    //PROCが全て死ぬと終了するので
    //なにもよばないで終わる
    break;
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	リスト後プロセスの引数	作成
 *
 *	@param	HEAPID heapID			ヒープID
 *	@param	*p_wk_adrs				ワーク
 *
 *	@return	引数
 */
//-----------------------------------------------------------------------------
static void *WBM_LISTAFTER_AllocParam( HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_LISTAFTER_PARAM *p_param;
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_LISTAFTER_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFIBATTLEMATCH_LISTAFTER_PARAM) );
  p_param->p_param        = &p_wk->param;
  p_param->p_player_btl_party = p_wk->p_player_btl_party;
  p_param->p_enemy_btl_party  = p_wk->p_enemy_btl_party;

  return p_param;
}

//----------------------------------------------------------------------------
/**
 *	@brief	リスト後プロセスの引数	破棄
 *
 *	@param	void *p_param_adrs				引数
 *	@param	*p_wk_adrs								ワーク
 */
//-----------------------------------------------------------------------------
static BOOL WBM_LISTAFTER_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS               *p_wk     = p_wk_adrs;
  WIFIBATTLEMATCH_LISTAFTER_PARAM   *p_param  = p_param_adrs;
  WIFIBATTLEMATCH_LISTAFTER_RESULT  result    = p_param->result;

  GFL_HEAP_FreeMemory( p_param );

  switch( result )
  { 
  case WIFIBATTLEMATCH_LISTAFTER_RESULT_SUCCESS:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_BATTLE );
    break;

  case WIFIBATTLEMATCH_LISTAFTER_RESULT_ERROR_NEXT_LOGIN:
    SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_LOGIN );
    break;
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	録画プロセスの引数	作成
 *
 *	@param	HEAPID heapID			ヒープID
 *	@param	*p_wk_adrs				ワーク
 *
 *	@return	引数
 */
//-----------------------------------------------------------------------------
static void *WBM_BTLREC_AllocParam( HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  BTL_REC_SEL_PARAM               *p_param;
  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(BTL_REC_SEL_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(BTL_REC_SEL_PARAM) );

  p_param->gamedata   = p_wk->param.p_game_data;
  p_param->b_rec        = TRUE;

  if( p_wk->p_enemy_data->btl_server_version != p_wk->p_player_data->btl_server_version )
  { 
    p_param->b_rec        = FALSE;
  }

  return p_param;
}

//----------------------------------------------------------------------------
/**
 *	@brief	録画プロセスの引数	破棄
 *
 *	@param	void *p_param_adrs				引数
 *	@param	*p_wk_adrs								ワーク
 */
//-----------------------------------------------------------------------------
static BOOL WBM_BTLREC_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS             *p_wk     = p_wk_adrs;
  BTL_REC_SEL_PARAM               *p_param  = p_param_adrs;

  GFL_HEAP_FreeMemory( p_param );

  //録画バッファ終了
  BattleRec_Exit();

  //次のPROC
  p_wk->core_mode = WIFIBATTLEMATCH_CORE_MODE_ENDREC;
  SUBPROC_CallProc( &p_wk->subproc, SUBPROCID_CORE );

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFIログアウトプロセスの引数	破棄
 *
 *	@param	void *p_param_adrs				引数
 *	@param	*p_wk_adrs								ワーク
 */
//-----------------------------------------------------------------------------
static void *LOGOUT_AllocParam( HEAPID heapID, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  WIFILOGOUT_PARAM    *p_param;

  p_param	= GFL_HEAP_AllocMemory( heapID, sizeof(WIFILOGOUT_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(WIFILOGOUT_PARAM) );

  p_param->gamedata = p_wk->param.p_game_data;
  p_param->bg       = WIFILOGIN_BG_NORMAL;
  p_param->display  = WIFILOGIN_DISPLAY_UP;

  return p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	WIFIログアウトプロセスの引数	破棄
 *
 *	@param	void *p_param_adrs				引数
 *	@param	*p_wk_adrs								ワーク
 */
//-----------------------------------------------------------------------------
static BOOL LOGOUT_FreeParam( void *p_param_adrs, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_SYS *p_wk     = p_wk_adrs;
  WIFILOGOUT_PARAM    *p_param  = p_param_adrs;

  //次のPROCは無し

  GFL_HEAP_FreeMemory( p_param );

  return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  データバッファ作成
 *
 *	@param	WIFIBATTLEMATCH_SYS *p_wk ワーク
 *	@param  heapID                    heapID
 */
//-----------------------------------------------------------------------------
static void DATA_CreateBuffer( WIFIBATTLEMATCH_SYS *p_wk, HEAPID heapID )
{ 
  p_wk->p_player_data = GFL_HEAP_AllocMemory( heapID, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
  GFL_STD_MemClear( p_wk->p_player_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
  p_wk->p_enemy_data  = GFL_HEAP_AllocMemory( heapID, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );
  GFL_STD_MemClear( p_wk->p_enemy_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );

#if 0
  p_wk->p_gpf_data = GFL_HEAP_AllocMemory( heapID, sizeof( DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA) );
  GFL_STD_MemClear( p_wk->p_gpf_data, sizeof( DREAM_WORLD_SERVER_WORLDBATTLE_STATE_DATA) );

  p_wk->p_sake_data = GFL_HEAP_AllocMemory( heapID, sizeof(WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA) );
  GFL_STD_MemClear( p_wk->p_gpf_data, sizeof(WIFIBATTLEMATCH_GDB_WIFI_SCORE_DATA) );
#endif
}
//----------------------------------------------------------------------------
/**
 *	@brief  データバッファ破棄
 *
 *	@param	WIFIBATTLEMATCH_SYS *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void DATA_DeleteBuffer( WIFIBATTLEMATCH_SYS *p_wk )
{ 
#if 0
  GFL_HEAP_FreeMemory( p_wk->p_sake_data );
  GFL_HEAP_FreeMemory( p_wk->p_gpf_data );
#endif
  GFL_HEAP_FreeMemory( p_wk->p_player_data );
  GFL_HEAP_FreeMemory( p_wk->p_enemy_data );
}
