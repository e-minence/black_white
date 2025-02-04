//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		touchbar.c
 *	@brief	アプリ用下画面タッチバー
 *	@author	Toru=Nagihashi
 *	@data		2009.09.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>

//sound
#include "sound/pm_sndsys.h"

//mine
#include "ui/touchbar.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	リソース
//=====================================
//リソース
typedef enum
{	
	TOUCHBAR_RES_COMMON_PLT,
	TOUCHBAR_RES_COMMON_CHR,
	TOUCHBAR_RES_COMMON_CEL,

	TOUCHBAR_RES_MAX
}RES_TYPE;

//-------------------------------------
///	シーケンス
//=====================================
enum
{
	TOUCHBAR_SEQ_MAIN,	//タッチ待ち
	TOUCHBAR_SEQ_ANM_TRG,		//ボタンアニメ
	TOUCHBAR_SEQ_ANM,		//ボタンアニメ
	TOUCHBAR_SEQ_TRG,		//トリガ
} ;

//-------------------------------------
///	動作関数タイプ
//=====================================
typedef enum
{
	TOUCHBAR_ICON_MOVETYPE_PUSH,	//押されたら、押されるアニメになるボタン
	TOUCHBAR_ICON_MOVETYPE_FLIP,	//押されたら、ONとOFFが切り替るボタン
} TOUCHBAR_ICON_MOVETYPE;


//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	アイコンワーク
//=====================================
typedef struct _ICON_WORK ICON_WORK;
typedef void (*PUSH_FUNC)( ICON_WORK *p_wk );
typedef void (*ACTIVE_FUNC)( ICON_WORK *p_wk, BOOL is_active );
struct _ICON_WORK
{
	GFL_CLWK						*p_clwk;		
	BOOL								is_active;		//アクティブか
	u32									now_anmseq;		//現在のアニメ
	PUSH_FUNC						push_func;		//押された時の動作
  ACTIVE_FUNC         active_func;  //アクティブ、パッシブ切替時動作
	TOUCHBAR_ICON_MOVETYPE	movetype;	//動作タイプ
	TOUCHBAR_ITEM_ICON			data;			//ボタン情報
} ;
//-------------------------------------
///	共通リソース
//=====================================
typedef struct 
{
	u32				res[TOUCHBAR_RES_MAX];
	u32				bg_frm;
	GFL_ARCUTIL_TRANSINFO				chr_pos;
} RES_WORK;

//-------------------------------------
///	下画面バーメインワーク
//=====================================
struct _TOUCHBAR_WORK
{
	s32				trg;			//押したアイコンのインデックス
	u32				tbl_len;	//最大アイコンのテーブル長
	u32				seq;			//シーケンス

	RES_WORK	res;			//共通リソース
	ICON_WORK	icon[0];	//アイコン
};

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
///	アプリケーション
//=====================================
static ICON_WORK * Touchbar_Search( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON type );
static const ICON_WORK * Touchbar_SearchConst( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON type );

//-------------------------------------
///	リソース
//=====================================
static void RES_Init( RES_WORK *p_wk, u8 frm, CLSYS_DRAW_TYPE	clsys_draw_type, PALTYPE bgplttype, APP_COMMON_MAPPING mapping, u8 bg_plt, u8 obj_plt, BOOL is_notload_bg, HEAPID heapID );
static void RES_Exit( RES_WORK *p_wk );
static u32 RES_GetResource( const RES_WORK *cp_wk, RES_TYPE type );
static void RES_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h,  u8 plt, BOOL compressedFlag, HEAPID heapID );
//-------------------------------------
///	ICON
//=====================================
static void ICON_Init( ICON_WORK *p_wk, GFL_CLUNIT* p_unit, const RES_WORK *cp_res, const TOUCHBAR_ITEM_ICON *cp_setup,CLSYS_DEFREND_TYPE	clsys_def_type, HEAPID heapID );
static void ICON_Exit( ICON_WORK *p_wk );
static BOOL ICON_Main( ICON_WORK *p_wk );
static BOOL ICON_IsMoveEnd( const ICON_WORK *cp_wk );
static TOUCHBAR_ICON ICON_GetType( const ICON_WORK *cp_wk );
static void ICON_SetVisible( ICON_WORK *p_wk, BOOL is_visible );
static BOOL ICON_GetVisible( const ICON_WORK *cp_wk );
static void ICON_SetActive( ICON_WORK *p_wk, BOOL is_active );
static BOOL ICON_GetActive( const ICON_WORK *cp_wk );
static void ICON_SetSE( ICON_WORK *p_wk, u32 se );
static u32 ICON_GetSE( const ICON_WORK *cp_wk );
static void ICON_SetKey( ICON_WORK *p_wk, u32 key );
static u32 ICON_GetKey( const ICON_WORK *cp_wk );
static void ICON_SetFlip( ICON_WORK *p_wk, BOOL is_flip );
static BOOL ICON_GetFlip( const ICON_WORK *cp_wk );
static void ICON_SetSoftPriority( ICON_WORK *p_wk, u8 pri );
static u8 ICON_GetSoftPriority( const ICON_WORK *p_wk );
static void ICON_SetBGPriority( ICON_WORK *p_wk, u8 bg_prio );
static u8 ICON_GetBGPriority( const ICON_WORK *p_wk );
//以下モード別関数
static void Icon_PushFuncNormal( ICON_WORK *p_wk );
static void Icon_PushFuncFlip( ICON_WORK *p_wk );
static void Icon_ActiveFuncNormal( ICON_WORK *p_wk, BOOL is_active );
static void Icon_ActiveFuncFlip( ICON_WORK *p_wk, BOOL is_active );
//=============================================================================
/**
 *					データ
 */
//=============================================================================
//-------------------------------------
///	アイコン情報
//	TOUCHBAR_ICONと対応したアクティブテーブル
//=====================================
static const struct
{	
  union
  { 
    struct
    { 
      u16 active_anmseq;
      u16 noactive_anmseq;
      u16 push_anmseq;
      u16 dummy_anmseq;
    };
    struct
    { 
      u16 off_active_anmseq;
      u16 off_noactive_anmseq;
      u16 on_active_anmseq;
      u16 on_noactive_anmseq;
    };
  };
	u16 se;
	u32	key;
	TOUCHBAR_ICON_MOVETYPE	movetype;
} sc_common_icon_data[TOUCHBAR_ICON_MAX]	=
{	
	//TOUCHBAR_ICON_CLOSE,	//×ボタン
	{	
    { 
      APP_COMMON_BARICON_EXIT,
      APP_COMMON_BARICON_EXIT_OFF,
      APP_COMMON_BARICON_EXIT_ON,
      0,
    },
    TOUCHBAR_SE_CLOSE,
		PAD_BUTTON_X,
		TOUCHBAR_ICON_MOVETYPE_PUSH,
	},
	//TOUCHBAR_ICON_RETURN,	//←┘ボタン
	{	
    { 
      APP_COMMON_BARICON_RETURN,
      APP_COMMON_BARICON_RETURN_OFF,
      APP_COMMON_BARICON_RETURN_ON,
      0,
    },
		TOUCHBAR_SE_CANCEL,
		PAD_BUTTON_B,
		TOUCHBAR_ICON_MOVETYPE_PUSH,
	},
	//TOUCHBAR_ICON_CUR_D,	//↓ボタン
	{	
    { 
      APP_COMMON_BARICON_CURSOR_DOWN,
      APP_COMMON_BARICON_CURSOR_DOWN_OFF,
      APP_COMMON_BARICON_CURSOR_DOWN_ON,
      0,
    },
		TOUCHBAR_SE_DECIDE,
		PAD_KEY_DOWN,
		TOUCHBAR_ICON_MOVETYPE_PUSH,
	},
	//TOUCHBAR_ICON_CUR_U,	//↑ボタン
	{	
    { 
      APP_COMMON_BARICON_CURSOR_UP,
      APP_COMMON_BARICON_CURSOR_UP_OFF,
      APP_COMMON_BARICON_CURSOR_UP_ON,
      0,
    },
		TOUCHBAR_SE_DECIDE,
		PAD_KEY_UP,
		TOUCHBAR_ICON_MOVETYPE_PUSH,
	},
	//TOUCHBAR_ICON_CUR_L,	//←ボタン
	{	
    { 
      APP_COMMON_BARICON_CURSOR_LEFT,
      APP_COMMON_BARICON_CURSOR_LEFT_OFF,
      APP_COMMON_BARICON_CURSOR_LEFT_ON,
      0,
    },
		TOUCHBAR_SE_DECIDE,
		PAD_KEY_LEFT,
		TOUCHBAR_ICON_MOVETYPE_PUSH,
	},
	//TOUCHBAR_ICON_CUR_R,	//→ボタン
	{	
    { 
      APP_COMMON_BARICON_CURSOR_RIGHT,
      APP_COMMON_BARICON_CURSOR_RIGHT_OFF,
      APP_COMMON_BARICON_CURSOR_RIGHT_ON,
      0,
    },
		TOUCHBAR_SE_DECIDE,
		PAD_KEY_RIGHT,
		TOUCHBAR_ICON_MOVETYPE_PUSH,
	},
	//TOUCHBAR_ICON_CHECK,	//ちぇっくボタン
	{	
    { 
      APP_COMMON_BARICON_CHECK_OFF,
      APP_COMMON_BARICON_CHECK_OFF_PASSIVE,
      APP_COMMON_BARICON_CHECK_ON,
      APP_COMMON_BARICON_CHECK_ON_PASSIVE,
    },
		TOUCHBAR_SE_Y_REG,
		PAD_BUTTON_Y,
		TOUCHBAR_ICON_MOVETYPE_FLIP,
	},
};


//=============================================================================
/**
 *					GLOBAL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	初期化拡張版（アイコン位置指定版）
 *
 *	@param	const TOUCHBAR_SETUP *cp_setup	設定情報
 *	@param	heapID	ヒープID
 *
 *	@return	ワーク
 */
//-----------------------------------------------------------------------------
extern TOUCHBAR_WORK * TOUCHBAR_Init( TOUCHBAR_SETUP *p_setup, HEAPID heapID )
{	
	TOUCHBAR_WORK	*p_wk;


	//ワーク作成、初期化
	{	
		u32 size;

		size	= sizeof(TOUCHBAR_WORK) + sizeof(ICON_WORK) * p_setup->item_num;
		p_wk	= GFL_HEAP_AllocMemory( heapID, size );
		GFL_STD_MemClear( p_wk, size );

		p_wk->tbl_len	= p_setup->item_num;
		p_wk->seq			= TOUCHBAR_SEQ_MAIN;
	}

	//リソース読みこみ
	{	
		CLSYS_DRAW_TYPE			clsys_draw_type;
		CLSYS_DEFREND_TYPE	clsys_def_type;
		PALTYPE							bgplttype;

		//OBJ読み込む場所をチェック
		if( p_setup->bar_frm >= GFL_BG_FRAME0_S )
		{	
			clsys_draw_type	= CLSYS_DRAW_SUB;
			clsys_def_type	= CLSYS_DEFREND_SUB;
			bgplttype				= PALTYPE_SUB_BG;
		}
		else
		{	
			clsys_draw_type	= CLSYS_DRAW_MAIN;
			clsys_def_type	= CLSYS_DEFREND_MAIN;
			bgplttype				= PALTYPE_MAIN_BG;
		}

		//リソース読み込み
		RES_Init( &p_wk->res, p_setup->bar_frm, clsys_draw_type, bgplttype, 
				p_setup->mapping, p_setup->bg_plt, p_setup->obj_plt, p_setup->is_notload_bg, heapID );
	
		//ICON設定
		{	
			int i;
			for( i = 0; i < p_wk->tbl_len; i++ )
			{	
				ICON_Init( &p_wk->icon[i], p_setup->p_unit, &p_wk->res, &p_setup->p_item[i], clsys_draw_type, heapID );
			}
		}
	}

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	破棄
 *
 *	@param	TOUCHBAR_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_Exit( TOUCHBAR_WORK *p_wk )
{	
	//ICON破棄
	{	
		int i;
		for( i = 0; i < p_wk->tbl_len; i++ )
		{	
			ICON_Exit( &p_wk->icon[i] );
		}
	}

	//リソース破棄
	RES_Exit( &p_wk->res );
	
	//破棄
	GFL_HEAP_FreeMemory( p_wk );

}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	メイン処理
 *
 *	@param	TOUCHBAR_WORK *p_wk			ワーク
 *	@param	CURSOR_WORK	cp_cursor	カーソルでボタンを押すのを実現させるため
 *
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_Main( TOUCHBAR_WORK *p_wk )
{	
	switch( p_wk->seq )
	{	
	case TOUCHBAR_SEQ_MAIN:
		//タッチ検出
		{	
			int i;
			p_wk->trg		= TOUCHBAR_SELECT_NONE;
			for( i = 0; i < p_wk->tbl_len; i++ )
			{
				if( ICON_Main( &p_wk->icon[ i ] ) )
				{	
					p_wk->trg	= i;
					p_wk->seq	= TOUCHBAR_SEQ_ANM_TRG;
					break;
				}
			}
		}
		break;

	case TOUCHBAR_SEQ_ANM_TRG:	//アニメ前の１Fを得るため
			p_wk->seq	 = TOUCHBAR_SEQ_ANM;
		/* fallthrough */
	
	case TOUCHBAR_SEQ_ANM:
		//動作
		if( ICON_IsMoveEnd( &p_wk->icon[ p_wk->trg ] ) )
		{
			p_wk->seq	 = TOUCHBAR_SEQ_TRG;
		}
		break;

	case TOUCHBAR_SEQ_TRG:
		//トリガを1フレームだけ伝える
		p_wk->seq	 = TOUCHBAR_SEQ_MAIN;
		break;
	}
	
	}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	選択されたものを取得	アニメ後の１F
 *
 *	@param	const TOUCHBAR_WORK *cp_wk ワーク
 *
 *	@return	TOUCHBAR_SELECT列挙
 */
//-----------------------------------------------------------------------------
TOUCHBAR_ICON TOUCHBAR_GetTrg( const TOUCHBAR_WORK *cp_wk )
{	

	if( cp_wk->seq	== TOUCHBAR_SEQ_TRG && cp_wk->trg != TOUCHBAR_SELECT_NONE )
	{	
		return ICON_GetType( &cp_wk->icon[ cp_wk->trg ] );
	}

	return TOUCHBAR_SELECT_NONE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	選択されたものを取得	アニメ前の１F
 *
 *	@param	const TOUCHBAR_WORK *cp_wk ワーク
 *
 *	@return	TOUCHBAR_SELECT列挙
 */
//-----------------------------------------------------------------------------
TOUCHBAR_ICON TOUCHBAR_GetTouch( const TOUCHBAR_WORK *cp_wk )
{	
	if( cp_wk->seq	== TOUCHBAR_SEQ_ANM_TRG && cp_wk->trg != TOUCHBAR_SELECT_NONE )
	{	
		return ICON_GetType( &cp_wk->icon[ cp_wk->trg ] );
	}

	return TOUCHBAR_SELECT_NONE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  TOUCHBAR  ボタン決定後かどうか
 *
 *	@param	const TOUCHBAR_WORK *cp_wk ワーク
 *
 *	@retval TRUE  タッチしてからトリガまで
 *	@retval FALSE タッチできる状況のとき
 */
//-----------------------------------------------------------------------------
BOOL TOUCHBAR_IsDecide( const TOUCHBAR_WORK *cp_wk )
{
  return cp_wk->seq != TOUCHBAR_SEQ_MAIN;
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	全てのアイコンを設定
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	is_active						アクティブ
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetActiveAll( TOUCHBAR_WORK *p_wk, BOOL is_active )
{	
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		ICON_SetActive( &p_wk->icon[ i ], is_active );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	全てのアイコンを表示設定
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	is_active						表示
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetVisibleAll( TOUCHBAR_WORK *p_wk, BOOL is_visible )
{	
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		ICON_SetVisible( &p_wk->icon[ i ], is_visible );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	全てのアイコンのBGプライオリティを設定
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	bg_prio						BGプライオリティ
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetBGPriorityAll( TOUCHBAR_WORK* p_wk, u8 bg_prio )
{
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		ICON_SetBGPriority( &p_wk->icon[ i ], bg_prio );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	全てのアイコンのOBJプライオリティを設定
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	pri						OBJプライオリティ
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetSoftPriorityAll( TOUCHBAR_WORK* p_wk, u8 pri )
{
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		ICON_SetSoftPriority( &p_wk->icon[ i ], pri );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	アイコンのアクティブ設定
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	icon								設定するアイコン
 *	@param	is_active						TRUEでアクティブ	FALSEでノンアクティブ
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetActive( TOUCHBAR_WORK *p_wk,  TOUCHBAR_ICON icon, BOOL is_active )
{	
	ICON_WORK * p_icon;
		
	//指定アイコンを検索
	p_icon	= Touchbar_Search( p_wk, icon );

	//設定
	ICON_SetActive( p_icon, is_active );
}
//----------------------------------------------------------------------------
/**	
 *	@brief	TOUCHBAR	アイコンのアクティブを取得
 *
 *	@param	const TOUCHBAR_WORK *cp_wk	ワーク
 *	@param	icon												取得するアイコン
 *
 *	@return	TRUEでアクティブ	FALSEでノンアクティブ
 */
//-----------------------------------------------------------------------------
BOOL TOUCHBAR_GetActive( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{	
	const ICON_WORK * cp_icon;
		
	//指定アイコンを検索
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//取得
	return ICON_GetActive( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	アイコンの表示設定
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	icon								設定するアイコン
 *	@param	is_visible					TRUEで表示	FALSEで非表示
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetVisible( TOUCHBAR_WORK *p_wk,  TOUCHBAR_ICON icon, BOOL is_visible )
{	
	ICON_WORK * p_icon;
		
	//指定アイコンを検索
	p_icon	= Touchbar_Search( p_wk, icon );

	//設定
	ICON_SetVisible( p_icon, is_visible );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	アイコンの表示取得
 *
 *	@param	const TOUCHBAR_WORK *cp_wk	ワーク
 *	@param	icon												取得するアイコン
 *
 *	@return	TRUEで表示	FALSEで非表示
 */
//-----------------------------------------------------------------------------
BOOL TOUCHBAR_GetVisible( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{	
	const ICON_WORK * cp_icon;
		
	//指定アイコンを検索
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//取得
	return ICON_GetVisible( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	アイコンにSEを設定
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	icon								アイコン
 *	@param	se									SE
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetSE( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, u32 se )
{	

	ICON_WORK * p_icon;
		
	//指定アイコンを検索
	p_icon	= Touchbar_Search( p_wk, icon );

	//設定
	ICON_SetSE( p_icon, se );
}

//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	アイコンのSEを取得
 *
 *	@param	const TOUCHBAR_WORK *cp_wk	ワーク
 *	@param	icon	アイコン
 *
 *	@return	SE
 */
//-----------------------------------------------------------------------------
u32 TOUCHBAR_GetSE( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{	

	const ICON_WORK * cp_icon;
		
	//指定アイコンを検索
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//取得
	return ICON_GetSE( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	アイコンにキー入力を設定
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	icon								アイコン
 *	@param	key									キー入力	PAD_KEY_UPやPAD_BUTTON_A等
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetUseKey( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, u32 key )
{	

	ICON_WORK * p_icon;
		
	//指定アイコンを検索
	p_icon	= Touchbar_Search( p_wk, icon );

	//設定
	ICON_SetKey( p_icon, key );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	アイコンからキー入力を取得
 *
 *	@param	const TOUCHBAR_WORK *cp_wk	ワーク
 *	@param	icon												アイコン
 *
 *	@return	キー入力
 */
//-----------------------------------------------------------------------------
u32 TOUCHBAR_GetUseKey( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{	

	const ICON_WORK * cp_icon;
		
	//指定アイコンを検索
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//取得
	return ICON_GetKey( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief  OBJプライオリティ設定
 *
 *	@param	TOUCHBAR_WORK *p_wk ワーク
 *	@param	icon                アイコン
 *	@param	pri                 優先順位
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetSoftPriority( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, u8 pri )
{ 

	ICON_WORK * p_icon;
		
	//指定アイコンを検索
	p_icon	= Touchbar_Search( p_wk, icon );

	//設定
	ICON_SetSoftPriority( p_icon, pri );
}
//----------------------------------------------------------------------------
/**
 *	@brief  OBJプライオリティ設定
 *
 *	@param	TOUCHBAR_WORK *p_wk ワーク
 *	@param	icon                アイコン
 *	@param	pri                 優先順位
 */
//-----------------------------------------------------------------------------
u8 TOUCHBAR_GetSoftPriority( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{ 
	const ICON_WORK * cp_icon;
		
	//指定アイコンを検索
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//設定
	return ICON_GetSoftPriority( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief  BGプライオリティ設定
 *
 *	@param	TOUCHBAR_WORK *p_wk ワーク
 *	@param	icon                アイコン
 *	@param	pri                 優先順位
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetBgPriority( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, u8 pri )
{ 
	ICON_WORK * p_icon;
		
	//指定アイコンを検索
	p_icon	= Touchbar_Search( p_wk, icon );

	//設定
	ICON_SetBGPriority( p_icon, pri );
}
//----------------------------------------------------------------------------
/**
 *	@brief  OBJプライオリティ設定
 *
 *	@param	TOUCHBAR_WORK *p_wk ワーク
 *	@param	icon                アイコン
 *	@param	pri                 優先順位
 */
//-----------------------------------------------------------------------------
u8 TOUCHBAR_GetBgPriority( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{ 
	const ICON_WORK * cp_icon;
		
	//指定アイコンを検索
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//設定
	return ICON_GetBGPriority( cp_icon );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	フリップ式ボタンへの設定
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	icon			アイコン
 *	@param	is_flip		TRUEでフリップON（チェックされている）FALSEでOFF
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_SetFlip( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon, BOOL is_flip )
{	
	ICON_WORK * p_icon;
		
	//指定アイコンを検索
	p_icon	= Touchbar_Search( p_wk, icon );

	//設定
	ICON_SetFlip( p_icon, is_flip );
}
//----------------------------------------------------------------------------
/**
 *	@brief	TOUCHBAR	フリップ式ボタンの取得
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	icon			アイコン
 *	@retval	is_flip		TRUEでフリップON（チェックされている）FALSEでOFF
 */
//-----------------------------------------------------------------------------
BOOL TOUCHBAR_GetFlip( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON icon )
{	
	const ICON_WORK * cp_icon;
		
	//指定アイコンを検索
	cp_icon	= Touchbar_SearchConst( cp_wk, icon );

	//取得
	return ICON_GetFlip( cp_icon );
}



//----------------------------------------------------------------------------
/**
 *	@brief  タッチバーリセット
 *
 *	@param	TOUCHBAR_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_ReStart( TOUCHBAR_WORK *p_wk )
{
  p_wk->seq = TOUCHBAR_SEQ_MAIN;
  p_wk->trg = TOUCHBAR_SELECT_NONE;

  TOUCHBAR_SetActiveAll( p_wk, TRUE );
}
//----------------------------------------------------------------------------
/**
 *	@brief  タッチバーリセット  + 一部ボタンのみアクティブにしたいとき　専用
 *
 *	@param	TOUCHBAR_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
void TOUCHBAR_ReStartEx( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON icon )
{
  p_wk->seq = TOUCHBAR_SEQ_MAIN;
  p_wk->trg = TOUCHBAR_SELECT_NONE;

  TOUCHBAR_SetActive( p_wk, icon, TRUE );
}

//=============================================================================
/**
 *	TOUCHBAR private
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	アイコンの種類からオブジェクトをSearch
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	type								探すアイコンのタイプ
 *
 *	@return	見つかったICON
 */
//-----------------------------------------------------------------------------
static ICON_WORK * Touchbar_Search( TOUCHBAR_WORK *p_wk, TOUCHBAR_ICON type )
{	
	int i;
	for( i = 0; i < p_wk->tbl_len; i++ )
	{	
		if( ICON_GetType( &p_wk->icon[ i ] ) == type )
		{	
			return &p_wk->icon[ i ];
		}
	}

	GF_ASSERT_MSG( 0,"TOUCHBAR:指定したアイコンが見つかりませんでした%d", type );
	return NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief	アイコンの種類からオブジェクトをSearch	CONST版
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	type								探すアイコンのタイプ
 *
 *	@return	見つかったICON
 */
//-----------------------------------------------------------------------------
static const ICON_WORK * Touchbar_SearchConst( const TOUCHBAR_WORK *cp_wk, TOUCHBAR_ICON type )
{	
	return Touchbar_Search( (TOUCHBAR_WORK*)cp_wk, type );
}
//=============================================================================
/**
 *			RES
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	リソース読みこみ
 *
 *	@param	TOUCHBAR_WORK *p_wk	ワーク
 *	@param	frm								フレーム
 *	@param	clsys_draw_type		セル読み込みタイプ
 *	@param	PALTYPE bgplttype	パレット読み込み場所
 *	@param	mapping						セルマッピングモード
 *	@param	bg_plt						BGのパレット番号
 *	@patam	obj_plt						OBJのパレット番号
 *	@param	is_notload_bg			TRUでBG読み込まないフラグ
 *	@param	heapID						読み込みテンポラリ用ヒープID
 */
//-----------------------------------------------------------------------------
static void RES_Init( RES_WORK *p_wk, u8 frm, CLSYS_DRAW_TYPE	clsys_draw_type, PALTYPE bgplttype, APP_COMMON_MAPPING mapping, u8 bg_plt, u8 obj_plt, BOOL is_notload_bg, HEAPID heapID )
{	
	//クリアー
	GFL_STD_MemClear( p_wk, sizeof(RES_WORK) );
	p_wk->bg_frm	= frm;

	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );

		//BG
		if( !is_notload_bg )
		{	
			//領域の確保
			GFL_ARCHDL_UTIL_TransVramPalette( p_handle, APP_COMMON_GetBarPltArcIdx(),
					bgplttype, bg_plt*0x20, TOUCHBAR_BG_PLT_NUM*0x20, heapID );
			//確保した領域に読み込み
			p_wk->chr_pos	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, APP_COMMON_GetBarCharArcIdx(), p_wk->bg_frm, TOUCHBAR_BG_CHARAAREA_SIZE, FALSE, heapID );
			GF_ASSERT_MSG( p_wk->chr_pos != GFL_ARCUTIL_TRANSINFO_FAIL, "TOUCHBAR:BGキャラ領域が足りませんでした\n" );
			//スクリーンはメモリ上に置いて、下部32*3だけ書き込み
			RES_UTIL_TransVramScreenEx( p_handle, APP_COMMON_GetBarScrnArcIdx(), p_wk->bg_frm,
					GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos), TOUCHBAR_MENUBAR_X, TOUCHBAR_MENUBAR_Y, 32, 24, 
					TOUCHBAR_MENUBAR_X, TOUCHBAR_MENUBAR_Y, TOUCHBAR_MENUBAR_W, TOUCHBAR_MENUBAR_H,
					bg_plt, FALSE, heapID );
		}


		//OBJ
		//共通アイコンリソース	
		p_wk->res[TOUCHBAR_RES_COMMON_PLT]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
				APP_COMMON_GetBarIconPltArcIdx(), clsys_draw_type, obj_plt*0x20, 0, TOUCHBAR_OBJ_PLT_NUM, heapID );	
		p_wk->res[TOUCHBAR_RES_COMMON_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
				APP_COMMON_GetBarIconCharArcIdx(), FALSE, clsys_draw_type, heapID );

		p_wk->res[TOUCHBAR_RES_COMMON_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				APP_COMMON_GetBarIconCellArcIdx(mapping),
				APP_COMMON_GetBarIconAnimeArcIdx(mapping), heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	リソース破棄
 *
 *	@param	RES_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void RES_Exit( RES_WORK *p_wk )
{	
	//OBJ
	GFL_CLGRP_CELLANIM_Release( p_wk->res[TOUCHBAR_RES_COMMON_CEL] );
	GFL_CLGRP_CGR_Release( p_wk->res[TOUCHBAR_RES_COMMON_CHR] );
	GFL_CLGRP_PLTT_Release( p_wk->res[TOUCHBAR_RES_COMMON_PLT] );

	//BG
	GFL_BG_FreeCharacterArea(p_wk->bg_frm,
			GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos),
			GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->chr_pos));


	//クリアー
	GFL_STD_MemClear( p_wk, sizeof(RES_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	リソース取得
 *
 *	@param	const RES_WORK *cp_wk	ワーク
 *	@param	type									タイプ
 *
 *	@return	リソース取得
 */
//-----------------------------------------------------------------------------
static u32 RES_GetResource( const RES_WORK *cp_wk, RES_TYPE type )
{	
	return cp_wk->res[ type ];
}
//----------------------------------------------------------------------------
/**
 *	@brief	Screenデータの　VRAM転送拡張版（読み込んだスクリーンの一部範囲をバッファに張りつける）
 *
 *	@param	ARCHANDLE *handle	ハンドル
 *	@param	datID							データID
 *	@param	frm								フレーム
 *	@param	chr_ofs						キャラオフセット
 *	@param	src_x							転送元ｘ座標
 *	@param	src_y							転送元Y座標
 *	@param	src_w							転送元幅			//データの全体の大きさ
 *	@param	src_h							転送元高さ		//データの全体の大きさ
 *	@param	dst_x							転送先X座標
 *	@param	dst_y							転送先Y座標
 *	@param	dst_w							転送先幅
 *	@param	dst_h							転送先高さ
 *	@param	plt								パレット番号
 *	@param	compressedFlag		圧縮フラグ
 *	@param	heapID						一時バッファ用ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void RES_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h, u8 plt, BOOL compressedFlag, HEAPID heapID )
{	
	void *p_src_arc;
	NNSG2dScreenData* p_scr_data;

	//読み込み
	p_src_arc = GFL_ARCHDL_UTIL_Load( handle, datID, compressedFlag, GetHeapLowID(heapID) );

	//アンパック
	if(!NNS_G2dGetUnpackedScreenData( p_src_arc, &p_scr_data ) )
	{	
		GF_ASSERT(0);	//スクリーンデータじゃないよ
	}

	//エラー
	GF_ASSERT( src_w * src_h * 2 <= p_scr_data->szByte );

	//キャラオフセット計算
	if( chr_ofs )
	{	
		int i;
		if( GFL_BG_GetScreenColorMode( frm ) == GX_BG_COLORMODE_16 )
		{
			u16 *p_scr16;
			
			p_scr16	=	(u16 *)&p_scr_data->rawData;
			for( i = 0; i < src_w * src_h ; i++ )
			{
				p_scr16[i]	+= chr_ofs;
			}	
		}
		else
		{	
			GF_ASSERT(0);	//256版は作ってない
		}
	}

	//書き込み
	if( GFL_BG_GetScreenBufferAdrs( frm ) != NULL )
	{	
		GFL_BG_WriteScreenExpand( frm, dst_x, dst_y, dst_w, dst_h,
				&p_scr_data->rawData, src_x, src_y, src_w, src_h );	
		GFL_BG_ChangeScreenPalette( frm, dst_x, dst_y, dst_w, dst_h, plt );
		GFL_BG_LoadScreenReq( frm );
	}
	else
	{	
		GF_ASSERT(0);	//バッファがない
	}

	GFL_HEAP_FreeMemory( p_src_arc );
}	

//=============================================================================
/**
 *			ICON
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ICON初期化
 *
 *	@param	ICON_WORK *p_wk								ワーク
 *	@param	p_unit												CLWK作成用UNIT
 *	@param	RES_WORK											共通リソース
 *	@param	TOUCHBAR_ITEM_ICON *cp_setup	セットアップデータ
 *	@param	HEAPID heapID									ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void ICON_Init( ICON_WORK *p_wk, GFL_CLUNIT* p_unit, const RES_WORK *cp_res, const TOUCHBAR_ITEM_ICON *cp_setup, CLSYS_DEFREND_TYPE	clsys_def_type, HEAPID heapID )
{	
	//クリア
	GFL_STD_MemClear( p_wk ,sizeof(ICON_WORK) );
	p_wk->data	= *cp_setup;
	p_wk->is_active	= TRUE;
	
	//CLWK作成
	{	
		u32 cg, plt, cell;
		GFL_CLWK_DATA				cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.pos_x	= cp_setup->pos.x;
		cldata.pos_y	= cp_setup->pos.y;

		//タイプによってリソースやデータが違う
		if( cp_setup->icon >= TOUCHBAR_ICON_CUTSOM1 )
		{	
			//カスタムアイコン
			cg		= cp_setup->cg_idx;
			plt		= cp_setup->plt_idx;
			cell	= cp_setup->cell_idx;
			p_wk->movetype	= TOUCHBAR_ICON_MOVETYPE_PUSH;
		}
		else
		{	
			//共通アイコン
			cg		= RES_GetResource( cp_res, TOUCHBAR_RES_COMMON_CHR );
			plt		= RES_GetResource( cp_res, TOUCHBAR_RES_COMMON_PLT );
			cell	= RES_GetResource( cp_res, TOUCHBAR_RES_COMMON_CEL );

			p_wk->data.active_anmseq		= sc_common_icon_data[ cp_setup->icon ].active_anmseq;
			p_wk->data.noactive_anmseq	= sc_common_icon_data[ cp_setup->icon ].noactive_anmseq;
			p_wk->data.push_anmseq			= sc_common_icon_data[ cp_setup->icon ].push_anmseq;
			p_wk->data.dummy_anmseq			= sc_common_icon_data[ cp_setup->icon ].dummy_anmseq;
			p_wk->data.key							= sc_common_icon_data[ cp_setup->icon ].key;
			p_wk->data.se								= sc_common_icon_data[ cp_setup->icon ].se;
			p_wk->movetype							= sc_common_icon_data[ cp_setup->icon ].movetype;
		}
		p_wk->now_anmseq	= p_wk->data.active_anmseq;
		cldata.anmseq	= p_wk->now_anmseq;

		//作成
		p_wk->p_clwk	= GFL_CLACT_WK_Create( p_unit, cg, plt, cell, &cldata, clsys_def_type, heapID );
		GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk, TRUE );

	}

		//動作タイプによって実行関数切り替え
	{	
		switch( p_wk->movetype )
		{	
		case TOUCHBAR_ICON_MOVETYPE_PUSH:	//押されたら、押されるアニメになるボタン
			p_wk->push_func	= Icon_PushFuncNormal;
      p_wk->active_func = Icon_ActiveFuncNormal;
			break;
		case TOUCHBAR_ICON_MOVETYPE_FLIP:	//押されたら、ONとOFFが切り替るボタン
			p_wk->push_func	  = Icon_PushFuncFlip;
      p_wk->active_func = Icon_ActiveFuncFlip;
			break;
		default:
			GF_ASSERT_MSG( 0, "TOUCHBAR:動作タイプが不適切です%d", p_wk->movetype );
		}

	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON破棄
 *
 *	@param	ICON_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void ICON_Exit( ICON_WORK *p_wk )
{	
	//CLWK破棄
	GFL_CLACT_WK_Remove( p_wk->p_clwk );
	//クリア
	GFL_STD_MemClear( p_wk ,sizeof(ICON_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	メイン処理
 *
 *	@param	ICON_WORK *p_wk ワーク
 *
 *	@return	TRUE入力ON　FALSE入力OFF
 */
//-----------------------------------------------------------------------------
static BOOL ICON_Main( ICON_WORK *p_wk )
{	
				
	BOOL	is_visible;
	BOOL	is_update;
	u32 x, y;

	is_update		= FALSE;
	is_visible	= GFL_CLACT_WK_GetDrawEnable( p_wk->p_clwk );

	//判定可能なときのみ
	if( p_wk->is_active & is_visible )
	{	
		//タッチ判定
		if( GFL_UI_TP_GetPointTrg( &x, &y ) )
		{	
			if( ((u32)( x - p_wk->data.pos.x) <= (u32)(TOUCHBAR_ICON_WIDTH))
					&	((u32)( y - p_wk->data.pos.y) <= (u32)(TOUCHBAR_ICON_HEIGHT)))
			{
				GFL_UI_SetTouchOrKey(GFL_APP_KTST_TOUCH);
				is_update	= TRUE;
			}
		}

		//キー判定
		if( p_wk->data.key != 0 )
		{	
			if( GFL_UI_KEY_GetTrg() & p_wk->data.key )
			{	
				GFL_UI_SetTouchOrKey(GFL_APP_KTST_KEY);
				is_update	= TRUE;
			}
		}

		//動作関数
		if( is_update )
		{	
			p_wk->push_func( p_wk );
			return TRUE;
		}
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	アニメの動作をチェック
 *
 *	@param	const ICON_WORK *cp_wk	ワーク
 *
 *	@return	TRUEで動作していない	FALSEで動作中
 */
//-----------------------------------------------------------------------------
static BOOL ICON_IsMoveEnd( const ICON_WORK *cp_wk )
{	
	return !GFL_CLACT_WK_CheckAnmActive( cp_wk->p_clwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	アイコンの種類
 *
 *	@param	const ICON_WORK *cp_wk ワーク
 *
 *	@return	アイコンの種類
 */
//-----------------------------------------------------------------------------
static TOUCHBAR_ICON ICON_GetType( const ICON_WORK *cp_wk )
{	
	return	cp_wk->data.icon;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	表示設定
 *
 *	@param	ICON_WORK *p_wk	ワーク
 *	@param	is_visible			TRUEで表示	FALSEで非表示
 */
//-----------------------------------------------------------------------------
static void ICON_SetVisible( ICON_WORK *p_wk, BOOL is_visible )
{	
	GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, is_visible );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	表示取得
 *
 *	@param	const ICON_WORK *cp_wk	ワーク
 *
 *	@return	TRUE表示	FALSE非表示
 */
//-----------------------------------------------------------------------------
static BOOL ICON_GetVisible( const ICON_WORK *cp_wk )
{	
	return GFL_CLACT_WK_GetDrawEnable( cp_wk->p_clwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	アクティブ状態設定
 *
 *	@param	ICON_WORK *p_wk		ワーク
 *	@param	is_active					TRUEでアクティブ状態	FALSEでノンアクティブ状態
 */
//-----------------------------------------------------------------------------
static void ICON_SetActive( ICON_WORK *p_wk, BOOL is_active )
{	
  p_wk->active_func( p_wk, is_active );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	アクティブ判定取得
 *
 *	@param	const ICON_WORK *cp_wk	ワーク
 *
 *	@return	TRUEでアクティブ状態	FALSEでノンアクティブ状態
 */
//-----------------------------------------------------------------------------
static BOOL ICON_GetActive( const ICON_WORK *cp_wk )
{	
	return cp_wk->is_active;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	SEを設定
 *
 *	@param	ICON_WORK *p_wk	ワーク
 *	@param	se							SE
 */
//-----------------------------------------------------------------------------
static void ICON_SetSE( ICON_WORK *p_wk, u32 se )
{	
	p_wk->data.se	= se;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	SEを取得
 *
 *	@param	const ICON_WORK *cp_wk	ワーク
 *
 *	@return	SE
 */
//-----------------------------------------------------------------------------
static u32 ICON_GetSE( const ICON_WORK *cp_wk )
{	
	return cp_wk->data.se;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	キー入力をセット
 *
 *	@param	ICON_WORK *p_wk	ワーク
 *	@param	key							キー入力	PAD_KEY_UP等
 */
//-----------------------------------------------------------------------------
static void ICON_SetKey( ICON_WORK *p_wk, u32 key )
{	
	p_wk->data.key	= key;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	キー入力を取得
 *
 *	@param	const ICON_WORK *cp_wk	ワーク
 *
 *	@return	キー入力
 */
//-----------------------------------------------------------------------------
static u32 ICON_GetKey( const ICON_WORK *cp_wk )
{	
	return cp_wk->data.key;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	フリップ設定
 *
 *	@param	ICON_WORK *p_wk	ワーク
 *	@param	is_flip					TRUEでON	FALSEでOFF
 *
 */
//-----------------------------------------------------------------------------
static void ICON_SetFlip( ICON_WORK *p_wk, BOOL is_flip )
{	
	if( is_flip )
	{	
		p_wk->now_anmseq	= p_wk->data.push_anmseq;
	}
	else
	{	
		p_wk->now_anmseq	= p_wk->data.active_anmseq;
	}	
	GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->now_anmseq );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	フリップ取得
 *
 *	@param	const ICON_WORK *cp_wk ワーク
 *
 *	@return	TRUEでON	FALSEでOFF
 */
//-----------------------------------------------------------------------------
static BOOL ICON_GetFlip( const ICON_WORK *cp_wk )
{	
	return cp_wk->now_anmseq	== cp_wk->data.push_anmseq ||
          cp_wk->now_anmseq == cp_wk->data.dummy_anmseq;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	OBJプライオリティ設定
 *
 *	@param	ICON_WORK *p_wk	ワーク
 *	@param	pri					OBJプライオリティ
 *
 */
//-----------------------------------------------------------------------------
static void ICON_SetSoftPriority( ICON_WORK *p_wk, u8 pri )
{
  GF_ASSERT(p_wk);
  GFL_CLACT_WK_SetSoftPri( p_wk->p_clwk, pri );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	OBJプライオリティ取得
 *
 *	@param	const ICON_WORK *cp_wk ワーク
 *
 *	@return	OBJプライオリティ
 */
//-----------------------------------------------------------------------------
static u8 ICON_GetSoftPriority( const ICON_WORK *p_wk )
{
  GF_ASSERT(p_wk);
  return GFL_CLACT_WK_GetSoftPri( p_wk->p_clwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	BGプライオリティ設定
 *
 *	@param	ICON_WORK *p_wk	ワーク
 *	@param	bg_prio						BGプライオリティ
 *
 */
//-----------------------------------------------------------------------------
static void ICON_SetBGPriority( ICON_WORK *p_wk, u8 bg_prio )
{
  GF_ASSERT(p_wk);
  GFL_CLACT_WK_SetBgPri( p_wk->p_clwk, bg_prio );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ICON	BGプライオリティ取得
 *
 *	@param	const ICON_WORK *cp_wk ワーク
 *
 *	@return	BGプライオリティ
 */
//-----------------------------------------------------------------------------
static u8 ICON_GetBGPriority( const ICON_WORK *p_wk )
{
  GF_ASSERT(p_wk);
  return GFL_CLACT_WK_GetBgPri( p_wk->p_clwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ボタンが押された時の動作関数	通常版
 *
 *	@param	ICON_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Icon_PushFuncNormal( ICON_WORK *p_wk )
{	
	//アニメを変え、音を鳴らす
	p_wk->now_anmseq	= p_wk->data.push_anmseq;

	GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->now_anmseq );

	if( p_wk->data.se != 0 )
	{	
		PMSND_PlaySE( p_wk->data.se );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	ボタンが押された時の動作関数	フリップ版
 *
 *	@param	ICON_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Icon_PushFuncFlip( ICON_WORK *p_wk )
{	
	BOOL now_flip	= ICON_GetFlip( p_wk );

	ICON_SetFlip( p_wk, !now_flip );

	if( p_wk->data.se != 0 )
	{	
		PMSND_PlaySE( p_wk->data.se );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	ボタンが押された時の動作関数	通常版
 *
 *	@param	ICON_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Icon_ActiveFuncNormal( ICON_WORK *p_wk, BOOL is_active )
{	
	p_wk->is_active	= is_active;

	if( is_active )
	{	
		//アクティブになった瞬間にアニメ開始にならないようにする
		if( p_wk->now_anmseq == p_wk->data.push_anmseq )
		{	
			p_wk->now_anmseq	= p_wk->data.active_anmseq;
		}	
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->now_anmseq );
	}
	else
	{	
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->data.noactive_anmseq );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	ボタンが押された時の動作関数	フリップ版
 *
 *	@param	ICON_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Icon_ActiveFuncFlip( ICON_WORK *p_wk, BOOL is_active )
{	
	p_wk->is_active	= is_active;

	if( is_active )
	{	
		if( p_wk->now_anmseq == p_wk->data.push_anmseq )
		{	
			p_wk->now_anmseq	= p_wk->data.dummy_anmseq;
		}	
    else if( p_wk->now_anmseq == p_wk->data.noactive_anmseq )
    { 
			p_wk->now_anmseq	= p_wk->data.active_anmseq;
    }
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->now_anmseq );
	}
	else
	{	
		if( p_wk->now_anmseq == p_wk->data.dummy_anmseq )
		{	
			p_wk->now_anmseq	= p_wk->data.push_anmseq;
		}	
    else if( p_wk->now_anmseq == p_wk->data.active_anmseq )
    { 
			p_wk->now_anmseq	= p_wk->data.noactive_anmseq;
    }
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk, p_wk->data.noactive_anmseq );
	}
}
