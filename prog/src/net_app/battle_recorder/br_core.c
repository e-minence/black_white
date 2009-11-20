//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_core.c
 *	@brief	バトルレコーダーコア部分（バトルレコードプレイヤー以外の部分）
 *	@author	Toru=Nagihashi
 *	@data		2009.11.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>

//システム
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID

//自分のモジュール
#include "br_graphic.h"
#include "br_res.h"
#include "br_proc_sys.h"

//各プロセス
#include "br_start_proc.h"
#include "br_menu_proc.h"
#include "br_record_proc.h"
#include "br_btlsubway_proc.h"
#include "br_rndmatch_proc.h"
#include "br_bvrank_proc.h"
#include "br_bvsearch_proc.h"
#include "br_codein_proc.h"
#include "br_bvsend_proc.h"
#include "br_musicallook_proc.h"
#include "br_musicalsend_proc.h"

//外部参照
#include "br_core.h"

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
#define BR_PROC_STACK_MAX	(3)

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	メインワーク
//=====================================
typedef struct 
{
	//グラフィック
	BR_GRAPHIC_WORK	*p_graphic;

	//リソース管理
	BR_RES_WORK			*p_res;

	//プロセス管理
	BR_PROC_SYS			*p_procsys;

	//引数
	BR_CORE_PARAM		*p_param;
} BR_CORE_WORK;

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
///	BRコアプロセス
//=====================================
static GFL_PROC_RESULT BR_CORE_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_CORE_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_CORE_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
///	各プロセスのための処理関数
//=====================================
//起動
static void BR_START_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_START_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//メニュー
static void BR_MENU_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_MENU_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//レコード
static void BR_RECORD_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_RECORD_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//バトルサブウェイ
static void BR_BTLSUBWAY_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_BTLSUBWAY_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//ランダムマッチ
static void BR_RNDMATCH_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_RNDMATCH_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//バトルビデオ30件
static void BR_BVRANK_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_BVRANK_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//詳しく探す
static void BR_BVSEARCH_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_BVSEARCH_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//ビデオナンバー
static void BR_CODEIN_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_CODEIN_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//ビデオ送信
static void BR_BVSEND_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_BVSEND_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//ミュージカルショット写真をみる
static void BR_MUSICALLOOK_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_MUSICALLOOK_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//ミュージカルショット写真を送る
static void BR_MUSICALSEND_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID );
static void BR_MUSICALSEND_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs );
//=============================================================================
/**
 *					外部参照
*/
//=============================================================================
//-------------------------------------
///	バトルレコーダーコアプロセス
//=====================================
const GFL_PROC_DATA BR_CORE_ProcData =
{	
	BR_CORE_PROC_Init,
	BR_CORE_PROC_Main,
	BR_CORE_PROC_Exit,
};

//=============================================================================
/**
 *					データ
 */
//=============================================================================
//-------------------------------------
///	プロセス繋がり情報
//=====================================
static const BR_PROC_SYS_DATA sc_procdata_tbl[BR_PROCID_MAX]	=
{	
	//	BR_PROCID_START,		//起動画面
	{	
		&BR_START_ProcData,
		sizeof( BR_START_PROC_PARAM ),
		BR_START_PROC_BeforeFunc,
		BR_START_PROC_AfterFunc,
	},
	//	BR_PROCID_MENU,			//メニュー画面
	{	
		&BR_MENU_ProcData,
		sizeof( BR_MENU_PROC_PARAM ),
		BR_MENU_PROC_BeforeFunc,
		BR_MENU_PROC_AfterFunc,
	},
	//	BR_PROCID_RECORD,		//録画記録画面
	{	
		&BR_RECORD_ProcData,
		sizeof( BR_RECORD_PROC_PARAM ),
		BR_RECORD_PROC_BeforeFunc,
		BR_RECORD_PROC_AfterFunc,
	},
	//	BR_PROCID_BTLSUBWAY,//バトルサブウェイ成績画面
	{	
		&BR_BTLSUBWAY_ProcData,
		sizeof( BR_BTLSUBWAY_PROC_PARAM ),
		BR_BTLSUBWAY_PROC_BeforeFunc,
		BR_BTLSUBWAY_PROC_AfterFunc,
	},
	//	BR_PROCID_RNDMATCH,	//ランダムマッチ成績画面
	{	
		&BR_RNDMATCH_ProcData,
		sizeof( BR_RNDMATCH_PROC_PARAM ),
		BR_RNDMATCH_PROC_BeforeFunc,
		BR_RNDMATCH_PROC_AfterFunc,
	},
	//	BR_PROCID_BV_RANK,	//バトルビデオ30件画面（最新、通信対戦、サブウェイ）
	{	
		&BR_BVRANK_ProcData,
		sizeof( BR_BVRANK_PROC_PARAM ),
		BR_BVRANK_PROC_BeforeFunc,
		BR_BVRANK_PROC_AfterFunc,
	},
	//	BR_PROCID_BV_SEARCH,//詳しく探す画面
	{	
		&BR_BVSEARCH_ProcData,
		sizeof( BR_BVSEARCH_PROC_PARAM ),
		BR_BVSEARCH_PROC_BeforeFunc,
		BR_BVSEARCH_PROC_AfterFunc,
	},
	//	BR_PROCID_CODEIN		,//バトルビデオナンバー入力画面
	{	
		&BR_CODEIN_ProcData,
		sizeof( BR_CODEIN_PROC_PARAM ),
		BR_CODEIN_PROC_BeforeFunc,
		BR_CODEIN_PROC_AfterFunc,
	},
	//	BR_PROCID_BV_SEND,	//バトルビデオ送信画面
	{	
		&BR_BVSEND_ProcData,
		sizeof( BR_BVSEND_PROC_PARAM ),
		BR_BVSEND_PROC_BeforeFunc,
		BR_BVSEND_PROC_AfterFunc,
	},
	//	BR_PROCID_MUSICAL_LOOK,	//ミュージカルショット	写真を見る画面
	{	
		&BR_MUSICALLOOK_ProcData,
		sizeof( BR_MUSICALLOOK_PROC_PARAM ),
		BR_MUSICALLOOK_PROC_BeforeFunc,
		BR_MUSICALLOOK_PROC_AfterFunc,
	},
	//	BR_PROCID_MUSICAL_SEND,	//ミュージカルショット	写真を送る画面
	{	
		&BR_MUSICALSEND_ProcData,
		sizeof( BR_MUSICALSEND_PROC_PARAM ),
		BR_MUSICALSEND_PROC_BeforeFunc,
		BR_MUSICALSEND_PROC_AfterFunc,
	},
};

//=============================================================================
/**
 *					BRコアプロセス
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	バトルレコーダーコア	初期化
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_CORE_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_CORE_WORK	*p_wk;

	//ヒープ作成
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BATTLE_RECORDER_CORE, 0x50000 );
//	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BATTLE_RECORDER_CORE, 0x30000 );

	//プロセスワーク作成
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_CORE_WORK), HEAPID_BATTLE_RECORDER_CORE );
	GFL_STD_MemClear( p_wk, sizeof(BR_CORE_WORK) );	
	p_wk->p_param		= p_param_adrs;

	//グラフィック初期化
	p_wk->p_graphic	= BR_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_BATTLE_RECORDER_CORE );
	p_wk->p_res			= BR_RES_Init( HEAPID_BATTLE_RECORDER_CORE );
	BR_RES_LoadBG( p_wk->p_res, BR_RES_BG_START_M, HEAPID_BATTLE_RECORDER_CORE );
	BR_RES_LoadBG( p_wk->p_res, BR_RES_BG_START_S, HEAPID_BATTLE_RECORDER_CORE );

	//コアプロセス初期化
	p_wk->p_procsys	= BR_PROC_SYS_Init( BR_PROCID_MENU, sc_procdata_tbl, 
			BR_PROCID_MAX, p_wk, HEAPID_BATTLE_RECORDER_CORE );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	バトルレコーダーコア	初期化
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_CORE_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_CORE_WORK	*p_wk	= p_wk_adrs;

	//コアプロセス破棄
	BR_PROC_SYS_Exit(	p_wk->p_procsys );

	//グラフィック破棄
	BR_RES_UnLoadBG( p_wk->p_res, BR_RES_BG_START_M );
	BR_RES_UnLoadBG( p_wk->p_res, BR_RES_BG_START_S );
	BR_RES_Exit( p_wk->p_res );
	BR_GRAPHIC_Exit( p_wk->p_graphic );

	//プロセスワーク破棄
	GFL_PROC_FreeWork( p_proc );

	//ヒープ破棄
	GFL_HEAP_DeleteHeap( HEAPID_BATTLE_RECORDER_CORE );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	バトルレコーダーコア	初期化
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_CORE_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_CORE_WORK	*p_wk	= p_wk_adrs;

	//プロセス処理
	BR_PROC_SYS_Main( p_wk->p_procsys );

	//描画
	BR_GRAPHIC_2D_Draw( p_wk->p_graphic );


	//プロセス終了条件
	if( BR_PROC_SYS_IsEnd( p_wk->p_procsys ) )
	{	
		return GFL_PROC_RES_FINISH;
	}
	else
	{	
		return GFL_PROC_RES_CONTINUE;
	}
}
//=============================================================================
/**
 *					各プロセス接続用
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理前関数
 *
 *	@param	void *p_param_adrs	アロケートされた引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 *	@param	void *cp_pre_param	このプロセスの前の引数ワーク
 *	@param	preID								このプロセスの前のID
 */
//-----------------------------------------------------------------------------
static void BR_START_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_START_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK					*p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理後関数
 *
 *	@param	void *p_param_adrs	引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 */
//-----------------------------------------------------------------------------
static void BR_START_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 

}

//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理前関数
 *
 *	@param	void *p_param_adrs	アロケートされた引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 *	@param	void *cp_pre_param	このプロセスの前の引数ワーク
 *	@param	preID								このプロセスの前のID
 */
//-----------------------------------------------------------------------------
static void BR_MENU_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{	
	BR_MENU_PROC_PARAM	*p_param	= p_param_adrs;
	BR_CORE_WORK				*p_wk			= p_wk_adrs;

	if( preID == BR_PROCID_RECORD )
	{	
		const BR_RECORD_PROC_PARAM	*cp_record_param	= cp_pre_param;
		if( cp_record_param->mode == BR_RECODE_PROC_MY )
		{	
			p_param->menuID			= BR_BROWSE_MENUID_BTLVIDEO;
		}
		else
		{
			p_param->menuID			= BR_BROWSE_MENUID_OTHER_RECORD;
		}
	}
	else
	{
    switch( p_wk->p_param->p_param->mode )
    { 
    case BR_MODE_BROWSE:
      p_param->menuID			= BR_BROWSE_MENUID_TOP;
      break;

    case BR_MODE_GLOBAL_BV:
      p_param->menuID			= BR_BTLVIDEO_MENUID_TOP;
      break;

    case BR_MODE_GLOBAL_MUSICAL:
      p_param->menuID			= BR_MUSICAL_MENUID_TOP;
      break;
    }
	}
	p_param->p_res			= p_wk->p_res;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理後関数
 *
 *	@param	void *p_param_adrs	引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 */
//-----------------------------------------------------------------------------
static void BR_MENU_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{	

}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理前関数
 *
 *	@param	void *p_param_adrs	アロケートされた引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 *	@param	void *cp_pre_param	このプロセスの前の引数ワーク
 *	@param	preID								このプロセスの前のID
 */
//-----------------------------------------------------------------------------
static void BR_RECORD_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{	
	BR_RECORD_PROC_PARAM			*p_param	= p_param_adrs;
	BR_CORE_WORK							*p_wk			= p_wk_adrs;
	const BR_MENU_PROC_PARAM	*cp_menu_param	= cp_pre_param;

	//メニュー以外から来ない
	GF_ASSERT_MSG( preID == BR_PROCID_MENU, "メニュー以外からは来ない %d", preID );

	p_param->mode				= cp_menu_param->next_mode;
	p_param->p_res			= p_wk->p_res;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理後関数
 *
 *	@param	void *p_param_adrs	引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 */
//-----------------------------------------------------------------------------
static void BR_RECORD_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{	

}

//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理前関数
 *
 *	@param	void *p_param_adrs	アロケートされた引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 *	@param	void *cp_pre_param	このプロセスの前の引数ワーク
 *	@param	preID								このプロセスの前のID
 */
//-----------------------------------------------------------------------------
static void BR_BTLSUBWAY_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{	
	BR_BTLSUBWAY_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK							*p_wk			= p_wk_adrs;
	const BR_MENU_PROC_PARAM	*cp_menu_param	= cp_pre_param;

	//メニュー以外から来ない
	GF_ASSERT_MSG( preID == BR_PROCID_MENU, "メニュー以外からは来ない %d", preID );
/*
	switch( cp_menu_param->next_data )
	{	
		
	}
*/
	p_param->p_res			= p_wk->p_res;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理後関数
 *
 *	@param	void *p_param_adrs	引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 */
//-----------------------------------------------------------------------------
static void BR_BTLSUBWAY_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{	

}

//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理前関数
 *
 *	@param	void *p_param_adrs	アロケートされた引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 *	@param	void *cp_pre_param	このプロセスの前の引数ワーク
 *	@param	preID								このプロセスの前のID
 */
//-----------------------------------------------------------------------------
static void BR_RNDMATCH_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{	
	BR_RNDMATCH_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK							*p_wk			= p_wk_adrs;
	const BR_MENU_PROC_PARAM	*cp_menu_param	= cp_pre_param;

	//メニュー以外から来ない
	GF_ASSERT_MSG( preID == BR_PROCID_MENU, "メニュー以外からは来ない %d", preID );
/*
	switch( cp_menu_param->next_data )
	{	
		
	}
*/
	p_param->p_res			= p_wk->p_res;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理後関数
 *
 *	@param	void *p_param_adrs	引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 */
//-----------------------------------------------------------------------------
static void BR_RNDMATCH_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{	

}

//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理前関数
 *
 *	@param	void *p_param_adrs	アロケートされた引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 *	@param	void *cp_pre_param	このプロセスの前の引数ワーク
 *	@param	preID								このプロセスの前のID
 */
//-----------------------------------------------------------------------------
static void BR_BVRANK_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_BVRANK_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						*p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理後関数
 *
 *	@param	void *p_param_adrs	引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 */
//-----------------------------------------------------------------------------
static void BR_BVRANK_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 

}

//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理前関数
 *
 *	@param	void *p_param_adrs	アロケートされた引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 *	@param	void *cp_pre_param	このプロセスの前の引数ワーク
 *	@param	preID								このプロセスの前のID
 */
//-----------------------------------------------------------------------------
static void BR_BVSEARCH_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_BVSEND_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						*p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理後関数
 *
 *	@param	void *p_param_adrs	引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 */
//-----------------------------------------------------------------------------
static void BR_BVSEARCH_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 

}

//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理前関数
 *
 *	@param	void *p_param_adrs	アロケートされた引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 *	@param	void *cp_pre_param	このプロセスの前の引数ワーク
 *	@param	preID								このプロセスの前のID
 */
//-----------------------------------------------------------------------------
static void BR_CODEIN_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_CODEIN_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						*p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理後関数
 *
 *	@param	void *p_param_adrs	引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 */
//-----------------------------------------------------------------------------
static void BR_CODEIN_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 

}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理前関数
 *
 *	@param	void *p_param_adrs	アロケートされた引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 *	@param	void *cp_pre_param	このプロセスの前の引数ワーク
 *	@param	preID								このプロセスの前のID
 */
//-----------------------------------------------------------------------------
static void BR_BVSEND_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_BVSEND_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						*p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_unit			= BR_GRAPHIC_GetClunit( p_wk->p_graphic );
}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理後関数
 *
 *	@param	void *p_param_adrs	引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 */
//-----------------------------------------------------------------------------
static void BR_BVSEND_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 


}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理前関数
 *
 *	@param	void *p_param_adrs	アロケートされた引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 *	@param	void *cp_pre_param	このプロセスの前の引数ワーク
 *	@param	preID								このプロセスの前のID
 */
//-----------------------------------------------------------------------------
static void BR_MUSICALLOOK_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_MUSICALLOOK_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						    *p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_graphic	= p_wk->p_graphic;
}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理後関数
 *
 *	@param	void *p_param_adrs	引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 */
//-----------------------------------------------------------------------------
static void BR_MUSICALLOOK_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 

}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理前関数
 *
 *	@param	void *p_param_adrs	アロケートされた引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 *	@param	void *cp_pre_param	このプロセスの前の引数ワーク
 *	@param	preID								このプロセスの前のID
 */
//-----------------------------------------------------------------------------
static void BR_MUSICALSEND_PROC_BeforeFunc( void *p_param_adrs, void *p_wk_adrs, const void *cp_pre_param, u32 preID )
{ 
	BR_MUSICALSEND_PROC_PARAM		*p_param	= p_param_adrs;
	BR_CORE_WORK						    *p_wk			= p_wk_adrs;

	p_param->p_res			= p_wk->p_res;
	p_param->p_procsys	= p_wk->p_procsys;
	p_param->p_graphic	= p_wk->p_graphic;
}
//----------------------------------------------------------------------------
/**
 *	@brief	プロック処理後関数
 *
 *	@param	void *p_param_adrs	引数ワーク
 *	@param	*p_wk_adrs					メインワーク
 */
//-----------------------------------------------------------------------------
static void BR_MUSICALSEND_PROC_AfterFunc( void *p_param_adrs, void *p_wk_adrs )
{ 

}
