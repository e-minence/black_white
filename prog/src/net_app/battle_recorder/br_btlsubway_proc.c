//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_btlsubway_proc.c
 *	@brief	バトルサブウェイプロセス
 *	@author	Toru=Nagihashi
 *	@data		2009.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>

//システム
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID

//アーカイブ
#include "msg/msg_battle_rec.h"
#include "arc_def.h"
#include "battle_recorder_gra.naix"
#include "..\resource\fldmapdata\flagwork\flag_define.h"

//自分のモジュール
#include "br_btn.h"
#include "br_util.h"
#include "br_snd.h"

//外部参照
#include "br_btlsubway_proc.h"

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	ボタンインデックス
//=====================================
typedef enum
{
	BR_BTLSUBWAY_BTNID_RETURN,
	BR_BTLSUBWAY_BTNID_MAX
} BR_BTLSUBWAY_BTNID;

//-------------------------------------
///	MSGWIN
//=====================================
typedef enum
{
  BR_BTLSUBWAY_MSGWINID_S_SINGLE,  //シングルバトル
  BR_BTLSUBWAY_MSGWINID_S_DOUBLE,  //ダブルバトル
  BR_BTLSUBWAY_MSGWINID_S_MULTI,  //マルチバトル
  BR_BTLSUBWAY_MSGWINID_S_WIFI,  //WIFIバトルルーム
  BR_BTLSUBWAY_MSGWINID_S_MAX,
} BR_BTLSUBWAY_MSGWINID_S;
typedef enum
{
  BR_BTLSUBWAY_MSGWINID_M_NONE_CAPTION = 0,  //バトルサブウェイ
  BR_BTLSUBWAY_MSGWINID_M_NONE_MAX,

  BR_BTLSUBWAY_MSGWINID_M_SINGLE_CAPTION = 0,      //バトルサブウェイ
  BR_BTLSUBWAY_MSGWINID_M_SINGLE_RULE,          //シングルバトル
  BR_BTLSUBWAY_MSGWINID_M_SINGLE_PRE_CAPTION,   //ぜんかい
  BR_BTLSUBWAY_MSGWINID_M_SINGLE_PRE_NUM,       //９９９９
  BR_BTLSUBWAY_MSGWINID_M_SINGLE_PRE_SUFFIX,    //連勝
  BR_BTLSUBWAY_MSGWINID_M_SINGLE_MOST_CAPTION,   //さいこう
  BR_BTLSUBWAY_MSGWINID_M_SINGLE_MOST_NUM,       //９９９９
  BR_BTLSUBWAY_MSGWINID_M_SINGLE_MOST_SUFFIX,   //連勝
  BR_BTLSUBWAY_MSGWINID_M_SINGLE_MAX,

  BR_BTLSUBWAY_MSGWINID_M_DOUBLE_CAPTION = 0,      //バトルサブウェイ
  BR_BTLSUBWAY_MSGWINID_M_DOUBLE_RULE,      //ダブルバトル
  BR_BTLSUBWAY_MSGWINID_M_DOUBLE_PRE_CAPTION,   //ぜんかい
  BR_BTLSUBWAY_MSGWINID_M_DOUBLE_PRE_NUM,       //９９９９
  BR_BTLSUBWAY_MSGWINID_M_DOUBLE_PRE_SUFFIX,    //連勝
  BR_BTLSUBWAY_MSGWINID_M_DOUBLE_MOST_CAPTION,   //さいこう
  BR_BTLSUBWAY_MSGWINID_M_DOUBLE_MOST_NUM,       //９９９９
  BR_BTLSUBWAY_MSGWINID_M_DOUBLE_MOST_SUFFIX,   //連勝
  BR_BTLSUBWAY_MSGWINID_M_DOUBLE_MAX,

  BR_BTLSUBWAY_MSGWINID_M_MULTI_CAPTION = 0,      //バトルサブウェイ
  BR_BTLSUBWAY_MSGWINID_M_MULTI_RULE,      //マルチバトル
  BR_BTLSUBWAY_MSGWINID_M_MULTI_TR_CAPTION,      //トレーナーとマルチ
  BR_BTLSUBWAY_MSGWINID_M_MULTI_TR_PRE_CAPTION,   //ぜんかい
  BR_BTLSUBWAY_MSGWINID_M_MULTI_TR_PRE_NUM,       //９９９９
  BR_BTLSUBWAY_MSGWINID_M_MULTI_TR_PRE_SUFFIX,    //連勝
  BR_BTLSUBWAY_MSGWINID_M_MULTI_TR_MOST_CAPTION,   //さいこう
  BR_BTLSUBWAY_MSGWINID_M_MULTI_TR_MOST_NUM,       //９９９９
  BR_BTLSUBWAY_MSGWINID_M_MULTI_TR_MOST_SUFFIX,   //連勝

  BR_BTLSUBWAY_MSGWINID_M_MULTI_FR_CAPTION,      //友達とマルチ
  BR_BTLSUBWAY_MSGWINID_M_MULTI_FR_PRE_CAPTION,   //ぜんかい
  BR_BTLSUBWAY_MSGWINID_M_MULTI_FR_PRE_NUM,       //９９９９
  BR_BTLSUBWAY_MSGWINID_M_MULTI_FR_PRE_SUFFIX,    //連勝
  BR_BTLSUBWAY_MSGWINID_M_MULTI_FR_MOST_CAPTION,   //さいこう
  BR_BTLSUBWAY_MSGWINID_M_MULTI_FR_MOST_NUM,       //９９９９
  BR_BTLSUBWAY_MSGWINID_M_MULTI_FR_MOST_SUFFIX,   //連勝
  BR_BTLSUBWAY_MSGWINID_M_MULTI_MAX,

  BR_BTLSUBWAY_MSGWINID_M_WIFI_CAPTION = 0,      //バトルサブウェイ
  BR_BTLSUBWAY_MSGWINID_M_WIFI_RULE,      //wifiバトルルーム成績
  BR_BTLSUBWAY_MSGWINID_M_WIFI_NOW_CAPTION,   //げんざいの記録
  BR_BTLSUBWAY_MSGWINID_M_WIFI_NOW_PREFFIX,    //ランク
  BR_BTLSUBWAY_MSGWINID_M_WIFI_NOW_NUM,       //９９９９
  BR_BTLSUBWAY_MSGWINID_M_WIFI_MAX,


  BR_BTLSUBWAY_MSGWINID_M_MAX  = BR_BTLSUBWAY_MSGWINID_M_MULTI_MAX,
} BR_BTLSUBWAY_MSGWINID_M;

//-------------------------------------
///	選択
//=====================================
typedef enum 
{
  BR_BTLSUBWAY_SELECT_NONE,
  BR_BTLSUBWAY_SELECT_SINGLE,
  BR_BTLSUBWAY_SELECT_DOUBLE,
  BR_BTLSUBWAY_SELECT_MULTI,
  BR_BTLSUBWAY_SELECT_WIFI,

  BR_BTLSUBWAY_SELECT_MAX
} BR_BTLSUBWAY_SELECT;

//-------------------------------------
///	数
//=====================================
#define OBJNUMBER_MAX				(4)

#define BSUBWAY_SCORE_DRAW_MAX  (9999)
//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================

//-------------------------------------
///	OBJ数字
//=====================================
typedef struct
{
	GFL_CLWK *p_clwk;
	GFL_CLWK *p_num[OBJNUMBER_MAX];
} OBJNUMBER_WORK;

//-------------------------------------
///	バトルサブウェイメインワーク
//=====================================
typedef struct 
{
  BMPOAM_SYS_PTR	  	p_bmpoam;	//BMPOAMシステム
  PRINT_QUE           *p_que;   //プリントキュー
  OBJNUMBER_WORK      objnum;   //OBJ数字
  BR_TEXT_WORK        *p_text;  //テキスト

	BR_BTN_WORK	        *p_btn[ BR_BTLSUBWAY_BTNID_MAX ];
  BR_MSGWIN_WORK      *p_msgwin_s[ BR_BTLSUBWAY_MSGWINID_S_MAX ];
  BR_MSGWIN_WORK      *p_msgwin_m[ BR_BTLSUBWAY_MSGWINID_M_MAX ];
  BR_BALLEFF_WORK     *p_balleff;

  BR_BTLSUBWAY_SELECT select;
	HEAPID              heapID;
} BR_BTLSUBWAY_WORK;


//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
///	バトルサブウェイプロセス
//=====================================
static GFL_PROC_RESULT BR_BTLSUBWAY_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_BTLSUBWAY_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_BTLSUBWAY_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	各画面構築
//=====================================
static void Br_BtlSubway_CreateMainDisplayNone( BR_BTLSUBWAY_WORK	*p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param );
static void Br_BtlSubway_CreateMainDisplaySingle( BR_BTLSUBWAY_WORK	*p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param );
static void Br_BtlSubway_CreateMainDisplayDouble( BR_BTLSUBWAY_WORK	*p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param );
static void Br_BtlSubway_CreateMainDisplayMulti( BR_BTLSUBWAY_WORK	*p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param );
static void Br_BtlSubway_CreateMainDisplayWifi( BR_BTLSUBWAY_WORK	*p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param );
static void Br_BtlSubway_DeleteMainDisplay( BR_BTLSUBWAY_WORK	*p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param );
static void Br_BtlSubway_CreateSubDisplay( BR_BTLSUBWAY_WORK *p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param );
static void Br_BtlSubway_DeleteSubDisplay( BR_BTLSUBWAY_WORK *p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param );

//-------------------------------------
///	private
//=====================================:
static BR_BTLSUBWAY_SELECT Br_BtlSubway_GetSelect( BR_BTLSUBWAY_WORK *p_wk, u32 x, u32 y );

static BOOL Br_BtlSubway_IsAppearSingle( BSUBWAY_SCOREDATA *p_sv, GAMEDATA *p_gamedata );
static BOOL Br_BtlSubway_IsAppearDouble( BSUBWAY_SCOREDATA *p_sv, GAMEDATA *p_gamedata );
static BOOL Br_BtlSubway_IsAppearMulti( BSUBWAY_SCOREDATA *p_sv, GAMEDATA *p_gamedata );

//-------------------------------------
///	OBJNUMBER
//=====================================
static void OBJNUMBER_Init( OBJNUMBER_WORK *p_wk, GFL_CLUNIT *p_clunit, const BR_RES_OBJ_DATA *cp_obj_res, int number, s16 x, s16 y, HEAPID heapID );
static void OBJNUMBER_Exit( OBJNUMBER_WORK *p_wk );
static void ObjNumber_SetNumber( OBJNUMBER_WORK *p_wk, int number );

//=============================================================================
/**
 *					外部参照
*/
//=============================================================================
//-------------------------------------
///	バトルサブウェイプロセスプロセス
//=====================================
const GFL_PROC_DATA BR_BTLSUBWAY_ProcData =
{	
	BR_BTLSUBWAY_PROC_Init,
	BR_BTLSUBWAY_PROC_Main,
	BR_BTLSUBWAY_PROC_Exit,
};

//=============================================================================
/**
 *					バトルサブウェイプロセス
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	バトルサブウェイプロセス	初期化
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BTLSUBWAY_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BTLSUBWAY_WORK				*p_wk;
	BR_BTLSUBWAY_PROC_PARAM	*p_param	= p_param_adrs;

	//プロセスワーク作成
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_BTLSUBWAY_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_BTLSUBWAY_WORK) );	
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );

  //モジュール作成
  p_wk->p_bmpoam		= BmpOam_Init( p_wk->heapID, p_param->p_unit);
  p_wk->p_que       = PRINTSYS_QUE_Create( p_wk->heapID );
  p_wk->p_balleff   = BR_BALLEFF_Init( p_param->p_unit, p_param->p_res, CLSYS_DRAW_SUB, p_wk->heapID );

  //グラフィック
  BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_BPFONT_M, p_wk->heapID );

	//グラフィック初期化
  Br_BtlSubway_CreateSubDisplay( p_wk, p_param );
  Br_BtlSubway_CreateMainDisplayNone( p_wk, p_param );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	バトルサブウェイプロセス	破棄
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BTLSUBWAY_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BTLSUBWAY_WORK				*p_wk	= p_wk_adrs;
	BR_BTLSUBWAY_PROC_PARAM	*p_param	= p_param_adrs;

  //グラフィック破棄
  Br_BtlSubway_DeleteSubDisplay( p_wk, p_param );
  Br_BtlSubway_DeleteMainDisplay( p_wk, p_param );
  GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );
  GFL_BG_LoadScreenReq( BG_FRAME_M_FONT );

  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_BPFONT_M );

	//モジュール破棄
  BR_BALLEFF_Exit( p_wk->p_balleff );
  PRINTSYS_QUE_Delete( p_wk->p_que );
  BmpOam_Exit( p_wk->p_bmpoam );

	//プロセスワーク破棄
	GFL_PROC_FreeWork( p_proc );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	バトルサブウェイプロセス	メイン処理
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BTLSUBWAY_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_MAIN,

    SEQ_CHANGEOUT_START,
    SEQ_CHANGEOUT_WAIT,
    SEQ_CHANGEIN_START,
    SEQ_CHANGEIN_WAIT,

    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_EXIT,
  };

	BR_BTLSUBWAY_WORK	*p_wk	= p_wk_adrs;
	BR_BTLSUBWAY_PROC_PARAM	*p_param	= p_param_adrs;

  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    *p_seq  = SEQ_FADEIN_WAIT;
    break;
  case SEQ_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_param->p_fade ) )
    { 
      *p_seq  = SEQ_MAIN;
    }
    break;
  case SEQ_MAIN:
    {
      u32 x, y;
      if( GFL_UI_TP_GetPointTrg( &x, &y ) )
      {
        //プロフィール切り替えチェック
        p_wk->select  = Br_BtlSubway_GetSelect( p_wk, x, y );
        if( p_wk->select != BR_BTLSUBWAY_SELECT_NONE )
        { 
          *p_seq  = SEQ_CHANGEOUT_START;
        }

        //終了チェック
        if( BR_BTN_GetTrg( p_wk->p_btn[BR_BTLSUBWAY_BTNID_RETURN], x, y ) )
        {	
          GFL_POINT pos;
          pos.x = x;
          pos.y = y;
          BR_BALLEFF_StartMove( p_wk->p_balleff, BR_BALLEFF_MOVE_EMIT, &pos );
          *p_seq  = SEQ_FADEOUT_START;
        }	
      }
    }
    break;

  case SEQ_CHANGEOUT_START:
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_MAIN, BR_FADE_DIR_OUT );
    (*p_seq)++;
    break;
  case SEQ_CHANGEOUT_WAIT:
    if( BR_FADE_IsEnd( p_param->p_fade ) )
    { 
      (*p_seq)++;
    }
    break;
  case SEQ_CHANGEIN_START:
    { 
      //読み込み変え
      Br_BtlSubway_DeleteMainDisplay( p_wk, p_param );
      switch( p_wk->select )
      {
      case BR_BTLSUBWAY_SELECT_SINGLE:
        Br_BtlSubway_CreateMainDisplaySingle( p_wk, p_param );
        break;
      case BR_BTLSUBWAY_SELECT_DOUBLE:
        Br_BtlSubway_CreateMainDisplayDouble( p_wk, p_param );
        break;
      case BR_BTLSUBWAY_SELECT_MULTI:
        Br_BtlSubway_CreateMainDisplayMulti( p_wk, p_param );
        break;
      case BR_BTLSUBWAY_SELECT_WIFI:
        Br_BtlSubway_CreateMainDisplayWifi( p_wk, p_param );
        break;
      }
      BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_MAIN, BR_FADE_DIR_IN );
    }
    (*p_seq)++;
    break;
  case SEQ_CHANGEIN_WAIT:
    if( BR_FADE_IsEnd( p_param->p_fade ) )
    { 
      *p_seq  = SEQ_MAIN;
    }
    break;

  case SEQ_FADEOUT_START:
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_FADEOUT_WAIT;
    break;
  case SEQ_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_param->p_fade ) )
    { 
      *p_seq  = SEQ_EXIT;
    }
    break;
  case SEQ_EXIT:
    NAGI_Printf( "BTLSUBWAY: Exit!\n" );
    BR_PROC_SYS_Pop( p_param->p_procsys );
    return GFL_PROC_RES_FINISH;
  }

   //文字表示
  { 
    int i;
    for( i = 0; i < BR_BTLSUBWAY_MSGWINID_S_MAX; i++ )
    { 
      if( p_wk->p_msgwin_s[i] )
      { 
        BR_MSGWIN_PrintMain( p_wk->p_msgwin_s[i] );
      }
    }
    for( i = 0; i < BR_BTLSUBWAY_MSGWINID_M_MAX; i++ )
    { 
      if( p_wk->p_msgwin_m[i] )
      { 
        BR_MSGWIN_PrintMain( p_wk->p_msgwin_m[i] );
      }
    }
  }

  PRINTSYS_QUE_Main( p_wk->p_que );
 
  if( p_wk->p_text )
  { 
    BR_TEXT_PrintMain( p_wk->p_text );
  }

  BR_BALLEFF_Main( p_wk->p_balleff );

	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *      画面構築
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  何も選択していないときの上画面
 *
 *	@param	BR_BTLSUBWAY_WORK	*p_wk ワーク
 *	@param	*p_param                引数
 */
//-----------------------------------------------------------------------------
static void Br_BtlSubway_CreateMainDisplayNone( BR_BTLSUBWAY_WORK	*p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param )
{ 
  static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_BTLSUBWAY_MSGWINID_M_NONE_MAX]  =
  { 
    {
      4,
      6,
      24,
      2,
      msg_804
    },
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );


  //リソース読み込み
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_SUBWAY_M_NONE, p_wk->heapID );

  //メッセージWIN作成
  {
    int i;
    for( i = 0; i < BR_BTLSUBWAY_MSGWINID_M_NONE_MAX; i++ )
    { 
      p_wk->p_msgwin_m[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );
      BR_MSGWIN_PrintColor( p_wk->p_msgwin_m[i], p_msg, sc_msgwin_data[i].msgID, p_font, BR_PRINT_COL_NORMAL );
    }
  }

  p_wk->p_text  = BR_TEXT_Init( p_param->p_res, p_wk->p_que, p_wk->heapID );
  BR_TEXT_Print( p_wk->p_text, p_param->p_res, msg_805 );

  { 
    BR_RES_OBJ_DATA obj_res;
    const u16 bp  = BSUBWAY_SCOREDATA_GetBattlePoint( p_param->p_subway );
    BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_BPFONT_M, &obj_res );
    OBJNUMBER_Init( &p_wk->objnum, p_param->p_unit, &obj_res, bp, 88+16, 88, p_wk->heapID );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  シングル戦の上画面
 *
 *	@param	BR_BTLSUBWAY_WORK	*p_wk ワーク
 *	@param	*p_param                引数
 */
//-----------------------------------------------------------------------------
static void Br_BtlSubway_CreateMainDisplaySingle( BR_BTLSUBWAY_WORK	*p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param )
{ 
  static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_BTLSUBWAY_MSGWINID_M_SINGLE_MAX]  =
  { 
    {
      3,
      3,
      12,
      2,
      msg_729,
    },
    { 
      4,
      6,
      24,
      2,
      msg_800,
    },
    { 
      4,
      10,
      7,
      2,
      msg_807,
    },
    { 
      14,
      10,
      7,
      2,
      msg_809,
    },
    { 
      21,
      10,
      7,
      2,
      msg_811,
    },
    { 
      4,
      14,
      7,
      2,
      msg_808,
    },
    { 
      14,
      14,
      7,
      2,
      msg_809,
    },
    { 
      21,
      14,
      7,
      2,
      msg_811,
    },
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 
  WORDSET *p_word;
  STRBUF  *p_strbuf;
  STRBUF  *p_src;

  BSWAY_PLAYMODE  mode;

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );
  p_word  = BR_RES_GetWordSet( p_param->p_res );


  //リソース読み込み
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_SUBWAY_M_SINGLE, p_wk->heapID );

  //スーパーかどうか
  if( Br_BtlSubway_IsAppearSingle( p_param->p_subway, p_param->p_gamedata ) )
  {
    mode  = BSWAY_PLAYMODE_S_SINGLE;
  }
  else
  {
    mode  = BSWAY_PLAYMODE_SINGLE;
  }

  //メッセージWIN作成
  {
    int i;
    for( i = 0; i < BR_BTLSUBWAY_MSGWINID_M_SINGLE_MAX; i++ )
    { 
      p_wk->p_msgwin_m[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );

      switch( i )
      { 
      case BR_BTLSUBWAY_MSGWINID_M_SINGLE_RULE:
        {
          u16 msgID;
          if( Br_BtlSubway_IsAppearSingle( p_param->p_subway, p_param->p_gamedata ) )
          {
            msgID = msg_800_01;
          }
          else
          {
            msgID = msg_800;
          }
          p_strbuf  = GFL_MSG_CreateString( p_msg, msgID );
        }
        break;

      case BR_BTLSUBWAY_MSGWINID_M_SINGLE_PRE_CAPTION: //ぜんかい
        { 
          u16 msgID;
          if( BSUBWAY_SCOREDATA_CheckExistStageNo( p_param->p_subway, mode ) )
          { 
            msgID = msg_817;
          }
          else
          { 
            msgID = msg_807;
          }
          p_strbuf  = GFL_MSG_CreateString( p_msg, msgID );
        }
        break;

      case BR_BTLSUBWAY_MSGWINID_M_SINGLE_PRE_NUM:  //ぜんかいの数値
        { 
          int number  = BSUBWAY_SCOREDATA_GetRenshouCount( p_param->p_subway, mode ); 
          number  = MATH_IMin( number, BSUBWAY_SCORE_DRAW_MAX );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterNumber( p_word, 0, number, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_src );
        }
        break;

      case BR_BTLSUBWAY_MSGWINID_M_SINGLE_MOST_NUM: //さいこうの数値
        { 
          int number  = BSUBWAY_SCOREDATA_GetMaxRenshouCount( p_param->p_subway, mode); 
          number  = MATH_IMin( number, BSUBWAY_SCORE_DRAW_MAX );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterNumber( p_word, 0, number, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_src );
        }
        break;

      default:
        p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
      }

      BR_MSGWIN_PrintBufColor( p_wk->p_msgwin_m[i], p_strbuf, p_font, BR_PRINT_COL_NORMAL );

      GFL_STR_DeleteBuffer( p_strbuf ); 
    }
  }


  { 
    BR_RES_OBJ_DATA obj_res;
    const u16 bp  = BSUBWAY_SCOREDATA_GetBattlePoint( p_param->p_subway );
    BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_BPFONT_M, &obj_res );
    OBJNUMBER_Init( &p_wk->objnum, p_param->p_unit,&obj_res,  bp, 144+16, 32, p_wk->heapID );
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  ダブル戦の上画面
 *
 *	@param	BR_BTLSUBWAY_WORK	*p_wk ワーク
 *	@param	*p_param                引数
 */
//-----------------------------------------------------------------------------
static void Br_BtlSubway_CreateMainDisplayDouble( BR_BTLSUBWAY_WORK	*p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param )
{ 
 static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_BTLSUBWAY_MSGWINID_M_DOUBLE_MAX]  =
  { 
    {
      3,
      3,
      12,
      2,
      msg_729,
    },
    { 
      4,
      6,
      24,
      2,
      msg_801,
    },
    { 
      4,
      10,
      7,
      2,
      msg_807,
    },
    { 
      14,
      10,
      7,
      2,
      msg_809,
    },
    { 
      21,
      10,
      7,
      2,
      msg_811,
    },
    { 
      4,
      14,
      7,
      2,
      msg_808,
    },
    { 
      14,
      14,
      7,
      2,
      msg_809,
    },
    { 
      21,
      14,
      7,
      2,
      msg_811,
    },
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 
  WORDSET *p_word;
  STRBUF  *p_strbuf;
  STRBUF  *p_src;

  BSWAY_PLAYMODE  mode;

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );
  p_word  = BR_RES_GetWordSet( p_param->p_res );


  //リソース読み込み
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_SUBWAY_M_DOUBLE, p_wk->heapID );

  //スーパーかどうか
  if( Br_BtlSubway_IsAppearDouble( p_param->p_subway, p_param->p_gamedata ) )
  {
    mode  = BSWAY_PLAYMODE_S_DOUBLE;
  }
  else
  {
    mode  = BSWAY_PLAYMODE_DOUBLE;
  }

  //メッセージWIN作成
  {
    int i;
    for( i = 0; i < BR_BTLSUBWAY_MSGWINID_M_DOUBLE_MAX; i++ )
    { 
      p_wk->p_msgwin_m[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );

      switch( i )
      { 
      case BR_BTLSUBWAY_MSGWINID_M_DOUBLE_RULE:
        {
          u16 msgID;
          if( Br_BtlSubway_IsAppearDouble( p_param->p_subway, p_param->p_gamedata ) )
          {
            msgID = msg_801_01;
          }
          else
          {
            msgID = msg_801;
          }
          p_strbuf  = GFL_MSG_CreateString( p_msg, msgID );
        }
        break;

      case BR_BTLSUBWAY_MSGWINID_M_DOUBLE_PRE_CAPTION: //ぜんかい
        { 
          u16 msgID;
          if( BSUBWAY_SCOREDATA_CheckExistStageNo( p_param->p_subway, mode ) )
          { 
            msgID = msg_817;
          }
          else
          { 
            msgID = msg_807;
          }
          p_strbuf  = GFL_MSG_CreateString( p_msg, msgID );
        }
        break;

      case BR_BTLSUBWAY_MSGWINID_M_DOUBLE_PRE_NUM:  //ぜんかいの数値
        { 
          int number  = BSUBWAY_SCOREDATA_GetRenshouCount( p_param->p_subway, mode ); 
          number  = MATH_IMin( number, BSUBWAY_SCORE_DRAW_MAX );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterNumber( p_word, 0, number, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_src );
        }
        break;

      case BR_BTLSUBWAY_MSGWINID_M_DOUBLE_MOST_NUM: //さいこうの数値
        { 
          int number  = BSUBWAY_SCOREDATA_GetMaxRenshouCount( p_param->p_subway, mode );
          number  = MATH_IMin( number, BSUBWAY_SCORE_DRAW_MAX );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterNumber( p_word, 0, number, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_src );
        }
        break;

      default:
        p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
      }

      BR_MSGWIN_PrintBufColor( p_wk->p_msgwin_m[i], p_strbuf, p_font, BR_PRINT_COL_NORMAL );

      GFL_STR_DeleteBuffer( p_strbuf ); 
    }
  }

  { 
    BR_RES_OBJ_DATA obj_res;
    const u16 bp  = BSUBWAY_SCOREDATA_GetBattlePoint( p_param->p_subway );
    BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_BPFONT_M, &obj_res );
    OBJNUMBER_Init( &p_wk->objnum, p_param->p_unit, &obj_res, bp, 144+16, 32, p_wk->heapID );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  マルチ戦用上画面
 *
 *	@param	BR_BTLSUBWAY_WORK	*p_wk ワーク
 *	@param	*p_param                引数
 */
//-----------------------------------------------------------------------------
static void Br_BtlSubway_CreateMainDisplayMulti( BR_BTLSUBWAY_WORK	*p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param )
{ 
 static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_BTLSUBWAY_MSGWINID_M_MULTI_MAX]  =
  { 
    {
      3,
      3,
      12,
      2,
      msg_729,
    },
    { 
      4,
      6,
      24,
      2,
      msg_802,
    },
    { 
      4,
      9,
      15,
      2,
      msg_812,
    },
    { 
      4,
      12,
      7,
      2,
      msg_807,
    },
    { 
      14,
      12,
      7,
      2,
      msg_809,
    },
    { 
      21,
      12,
      7,
      2,
      msg_811,
    },
    { 
      4,
      14,
      7,
      2,
      msg_808,
    },
    { 
      14,
      14,
      7,
      2,
      msg_809,
    },
    { 
      21,
      14,
      7,
      2,
      msg_811,
    },
    { 
      4,
      17,
      15,
      2,
      msg_813,
    },
    { 
      4,
      20,
      7,
      2,
      msg_807,
    },
    { 
      14,
      20,
      7,
      2,
      msg_809,
    },
    { 
      21,
      20,
      7,
      2,
      msg_811,
    },
    { 
      4,
      22,
      7,
      2,
      msg_808,
    },
    { 
      14,
      22,
      7,
      2,
      msg_809,
    },
    { 
      21,
      22,
      7,
      2,
      msg_811,
    },
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 
  WORDSET *p_word;
  STRBUF  *p_strbuf;
  STRBUF  *p_src;

  BSWAY_PLAYMODE  multi_mode;
  BSWAY_PLAYMODE  comm_multi_mode;


  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );
  p_word  = BR_RES_GetWordSet( p_param->p_res );


  //リソース読み込み
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_SUBWAY_M_MULTI, p_wk->heapID );


  //マルチモード
  if( Br_BtlSubway_IsAppearMulti( p_param->p_subway, p_param->p_gamedata ) )
  {
    multi_mode      = BSWAY_PLAYMODE_S_MULTI;
    comm_multi_mode = BSWAY_PLAYMODE_S_COMM_MULTI;
  }
  else
  {
    multi_mode      = BSWAY_PLAYMODE_MULTI;
    comm_multi_mode = BSWAY_PLAYMODE_COMM_MULTI;
  }


  //メッセージWIN作成
  {
    int i;
    for( i = 0; i < BR_BTLSUBWAY_MSGWINID_M_MULTI_MAX; i++ )
    { 
      p_wk->p_msgwin_m[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );

      switch( i )
      { 
      case BR_BTLSUBWAY_MSGWINID_M_MULTI_RULE:
        {
          u16 msgID;
          if( Br_BtlSubway_IsAppearMulti( p_param->p_subway, p_param->p_gamedata ) )
          {
            msgID = msg_802_01;
          }
          else
          {
            msgID = msg_802;
          }
          p_strbuf  = GFL_MSG_CreateString( p_msg, msgID );
        }
        break;

      case BR_BTLSUBWAY_MSGWINID_M_MULTI_TR_PRE_CAPTION:
        { 
          u16 msgID;
          if( BSUBWAY_SCOREDATA_CheckExistStageNo( p_param->p_subway, multi_mode ) )
          { 
            msgID = msg_817;
          }
          else
          { 
            msgID = msg_807;
          }
          p_strbuf  = GFL_MSG_CreateString( p_msg, msgID );
        }
        break; 

      case BR_BTLSUBWAY_MSGWINID_M_MULTI_TR_PRE_NUM:   //ぜんかい
        { 
          int number  = BSUBWAY_SCOREDATA_GetRenshouCount( p_param->p_subway, multi_mode ); 
          number  = MATH_IMin( number, BSUBWAY_SCORE_DRAW_MAX );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterNumber( p_word, 0, number, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_src );
        }
        break;
      case BR_BTLSUBWAY_MSGWINID_M_MULTI_TR_MOST_NUM:       //９９９９
        { 
          int number  = BSUBWAY_SCOREDATA_GetMaxRenshouCount( p_param->p_subway, multi_mode ); 
          number  = MATH_IMin( number, BSUBWAY_SCORE_DRAW_MAX );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterNumber( p_word, 0, number, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_src );
        }
        break;
      case BR_BTLSUBWAY_MSGWINID_M_MULTI_FR_PRE_CAPTION:
        { 
          u16 msgID;
          if( BSUBWAY_SCOREDATA_CheckExistStageNo( p_param->p_subway, comm_multi_mode ) )
          { 
            msgID = msg_817;
          }
          else
          { 
            msgID = msg_807;
          }
          p_strbuf  = GFL_MSG_CreateString( p_msg, msgID );
        }
        break;

      case BR_BTLSUBWAY_MSGWINID_M_MULTI_FR_PRE_NUM:  //ぜんかいの数値
        { 
          int number  = BSUBWAY_SCOREDATA_GetRenshouCount( p_param->p_subway, comm_multi_mode ); 
          number  = MATH_IMin( number, BSUBWAY_SCORE_DRAW_MAX );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterNumber( p_word, 0, number, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_src );
        }
        break;

      case BR_BTLSUBWAY_MSGWINID_M_MULTI_FR_MOST_NUM: //さいこうの数値
        { 
          int number  = BSUBWAY_SCOREDATA_GetMaxRenshouCount( p_param->p_subway, comm_multi_mode ); 
          number  = MATH_IMin( number, BSUBWAY_SCORE_DRAW_MAX );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterNumber( p_word, 0, number, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_src );
        }
        break;

      default:
        p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
      }

      BR_MSGWIN_PrintBufColor( p_wk->p_msgwin_m[i], p_strbuf, p_font, BR_PRINT_COL_NORMAL );

      GFL_STR_DeleteBuffer( p_strbuf ); 

    }
  }

  { 
    BR_RES_OBJ_DATA obj_res;
    const u16 bp  = BSUBWAY_SCOREDATA_GetBattlePoint( p_param->p_subway );
    BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_BPFONT_M, &obj_res );
    OBJNUMBER_Init( &p_wk->objnum, p_param->p_unit, &obj_res, bp, 144+16, 32, p_wk->heapID );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI用上画面
 *
 *	@param	BR_BTLSUBWAY_WORK	*p_wk ワーク
 *	@param	*p_param                引数
 */
//-----------------------------------------------------------------------------
static void Br_BtlSubway_CreateMainDisplayWifi( BR_BTLSUBWAY_WORK	*p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param )
{ 
 static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_BTLSUBWAY_MSGWINID_M_WIFI_MAX]  =
  { 
    {
      3,
      3,
      12,
      2,
      msg_729,
    },
    { 
      4,
      6,
      24,
      2,
      msg_816,
    },
    { 
      4,
      10,
      13,
      2,
      msg_815,
    },
    { 
      21,
      10,
      5,
      2,
      msg_814,
    },
    { 
      27,
      10,
      2,
      2,
      msg_809,
    },
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 
  WORDSET *p_word;
  STRBUF  *p_strbuf;
  STRBUF  *p_src;

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );
  p_word  = BR_RES_GetWordSet( p_param->p_res );

  //リソース読み込み
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_SUBWAY_M_WIFI, p_wk->heapID );

  //メッセージWIN作成
  {
    int i;
    for( i = 0; i < BR_BTLSUBWAY_MSGWINID_M_WIFI_MAX; i++ )
    { 
      p_wk->p_msgwin_m[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );

      switch( i )
      { 
      case BR_BTLSUBWAY_MSGWINID_M_WIFI_NOW_NUM:  //ランク
        { 
         const int number  = BSUBWAY_SCOREDATA_GetWifiRank( p_param->p_subway );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterNumber( p_word, 0, number, 2, STR_NUM_DISP_SPACE, STR_NUM_CODE_HANKAKU );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_src );
        }
        break;

      default:
        p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
      }

      BR_MSGWIN_PrintBufColor( p_wk->p_msgwin_m[i], p_strbuf, p_font, BR_PRINT_COL_NORMAL );

      GFL_STR_DeleteBuffer( p_strbuf ); 
    }
  }
  { 
    BR_RES_OBJ_DATA obj_res;
    const u16 bp  = BSUBWAY_SCOREDATA_GetBattlePoint( p_param->p_subway );
    BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_BPFONT_M, &obj_res );
    OBJNUMBER_Init( &p_wk->objnum, p_param->p_unit, &obj_res, bp, 144+16, 32, p_wk->heapID );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  上画面を破棄
 *
 *	@param	BR_BTLSUBWAY_WORK	*p_wk ワーク
 *	@param	*p_param                引数
 */
//-----------------------------------------------------------------------------
static void Br_BtlSubway_DeleteMainDisplay( BR_BTLSUBWAY_WORK	*p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param )
{ 
  OBJNUMBER_Exit( &p_wk->objnum );

  { 
    int i;
    for( i = 0; i < BR_BTLSUBWAY_MSGWINID_M_MAX; i++ )
    { 
      if( p_wk->p_msgwin_m[i] )
      { 
        BR_MSGWIN_Exit( p_wk->p_msgwin_m[i] );
        p_wk->p_msgwin_m[i] = NULL;
      }
    }
  }
  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_SUBWAY_M_SINGLE );

  if( p_wk->p_text )
  { 
    BR_TEXT_Exit( p_wk->p_text, p_param->p_res );
    p_wk->p_text  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  下画面を構築
 *
 *	@param	BR_BTLSUBWAY_WORK *p_wk ワーク
 *	@param	*p_param                引数
 */
//-----------------------------------------------------------------------------
static void Br_BtlSubway_CreateSubDisplay( BR_BTLSUBWAY_WORK *p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param )
{ 
  static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_BTLSUBWAY_MSGWINID_S_MAX]  =
  { 
    {
      4,
      2,
      24,
      2,
      msg_800
    },
    {
      4,
      6,
      24,
      2,
      msg_801
    },
    {
      4,
      10,
      24,
      2,
      msg_802
    },
    {
      4,
      14,
      24,
      2,
      msg_803
    },
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 


  //リソース読み込み
  BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, p_wk->heapID ); 
	BR_RES_LoadBG( p_param->p_res, BR_RES_BG_SUBWAY_S, p_wk->heapID );

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );


  //ボタン作成
  { 
    GFL_CLWK_DATA cldata;
    BR_RES_OBJ_DATA res;

    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    cldata.pos_x    = 80;
    cldata.pos_y    = 168;
    cldata.anmseq   = 0;
    cldata.softpri  = 1;

    BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, &res );

    p_wk->p_btn[BR_BTLSUBWAY_BTNID_RETURN] = BR_BTN_Init( &cldata, msg_05, BR_BTN_DATA_SHORT_WIDTH, CLSYS_DRAW_SUB, p_param->p_unit, p_wk->p_bmpoam, p_font, p_msg, &res, p_wk->heapID );
;
  }

  //メッセージWIN作成
  {
    int i;
    u16 msgID;

    for( i = 0; i < BR_BTLSUBWAY_MSGWINID_S_MAX; i++ )
    { 
      p_wk->p_msgwin_s[i]  = BR_MSGWIN_Init( BG_FRAME_S_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_S_FONT, p_wk->p_que, p_wk->heapID );

      switch( i )
      { 
      case BR_BTLSUBWAY_MSGWINID_S_SINGLE:  //シングルバトル
        if( Br_BtlSubway_IsAppearSingle( p_param->p_subway, p_param->p_gamedata ) )
        {
          //スーパーシングル
          msgID = msg_800_01;
        }
        else
        {
          //シングル
          msgID = msg_800;
        }
        break;
      case BR_BTLSUBWAY_MSGWINID_S_DOUBLE:  //ダブルバトル
        if( Br_BtlSubway_IsAppearDouble( p_param->p_subway, p_param->p_gamedata ) )
        {
          //スーパーダブル
          msgID = msg_801_01;
        }
        else
        {
          //ダブル
          msgID = msg_801;
        }
        break;
      case BR_BTLSUBWAY_MSGWINID_S_MULTI:  //マルチバトル
        if( Br_BtlSubway_IsAppearMulti( p_param->p_subway, p_param->p_gamedata ) )
        {
          //スーパーマルチ
          msgID = msg_802_01;
        }
        else
        {
          //マルチ
          msgID = msg_802;
        }
        break;
      case BR_BTLSUBWAY_MSGWINID_S_WIFI:  //WIFIバトルルーム
        msgID = msg_803;
        break;
      }

      BR_MSGWIN_PrintColor( p_wk->p_msgwin_s[i], p_msg, msgID, p_font, BR_PRINT_COL_NORMAL );
    }
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  下画面を破棄
 *
 *	@param	BR_BTLSUBWAY_WORK *p_wk ワーク
 *	@param	*p_param                引数
 */
//-----------------------------------------------------------------------------
static void Br_BtlSubway_DeleteSubDisplay( BR_BTLSUBWAY_WORK *p_wk, BR_BTLSUBWAY_PROC_PARAM *p_param )
{ 
  //メッセージを破棄
  {
    int i;
    for( i = 0; i < BR_BTLSUBWAY_MSGWINID_S_MAX; i++ )
    {
      if( p_wk->p_msgwin_s[i] )
      { 
        BR_MSGWIN_Exit( p_wk->p_msgwin_s[i] );
        p_wk->p_msgwin_s[i] = NULL;
      }
    }
  }

  //ボタン破棄
  { 
    int i;
    for( i = 0; i < BR_BTLSUBWAY_BTNID_MAX; i++ )
    { 
      if( p_wk->p_btn[i] )
      { 
        BR_BTN_Exit( p_wk->p_btn[i] );
        p_wk->p_btn[i]  = NULL;
      }
    }
  }

  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S ); 
	BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_SUBWAY_S );
}
//----------------------------------------------------------------------------
/**
 *	@brief  選択を取得
 *
 *	@param	BR_BTLSUBWAY_WORK *p_wk ワーク
 *	@param	x タッチ座標X
 *	@param	y タッチ座標Y
 *
 *	@return 選んだもの
 */
//-----------------------------------------------------------------------------
static BR_BTLSUBWAY_SELECT Br_BtlSubway_GetSelect( BR_BTLSUBWAY_WORK *p_wk, u32 x, u32 y )
{ 
  static const GFL_RECT sc_select_rect[BR_BTLSUBWAY_SELECT_MAX] =
  { 
    { 
      0,0,0,0
    },
    { 
      3*8,1*8,29*8,4*8,
    },
    { 
      3*8,5*8,29*8,8*8,
    },
    { 
      3*8,9*8,29*8,12*8,
    },
    { 
      3*8,13*8,29*8,16*8,
    },
  };

  int i;

  for( i = 0; i < BR_BTLSUBWAY_SELECT_MAX; i++ )
  { 
    if( i != p_wk->select )
    { 
      if( ((u32)( x - sc_select_rect[i].left) <= (u32)(sc_select_rect[i].right - sc_select_rect[i].left))
          & ((u32)( y - sc_select_rect[i].top) <= (u32)(sc_select_rect[i].bottom - sc_select_rect[i].top)))
      {

        GFL_POINT pos;
        pos.x = x;
        pos.y = y;
        BR_BALLEFF_StartMove( p_wk->p_balleff, BR_BALLEFF_MOVE_EMIT, &pos );

        PMSND_PlaySE( BR_SND_SE_OK );
        return i;
      }
    }
  }

  return BR_BTLSUBWAY_SELECT_NONE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  スーパーシングルが出現しているか
 *
 *	@param	BSUBWAY_SCOREDATA *p_wk ワーク
 *
 *	@return TRUEで登場  FALSEでまだ
 */
//-----------------------------------------------------------------------------
static BOOL Br_BtlSubway_IsAppearSingle( BSUBWAY_SCOREDATA *p_sv, GAMEDATA *p_gamedata )
{
  BOOL  is_clear  =  BSUBWAY_SCOREDATA_SetFlag( p_sv,
                          BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_SINGLE, BSWAY_SETMODE_get );
  EVENTWORK *p_ev  = GAMEDATA_GetEventWork( p_gamedata );
  BOOL  is_gameclear  = EVENTWORK_CheckEventFlag( p_ev, SYS_FLAG_GAME_CLEAR );

  return is_clear && is_gameclear;
}
//----------------------------------------------------------------------------
/**
 *	@brief  スーパーダブルが出現しているか
 *
 *	@param	BSUBWAY_SCOREDATA *p_sv ワーク
 *
 *	@return TRUEで登場  FALSEでまだ
 */
//-----------------------------------------------------------------------------
static BOOL Br_BtlSubway_IsAppearDouble( BSUBWAY_SCOREDATA *p_sv, GAMEDATA *p_gamedata )
{
  BOOL  is_clear  =  BSUBWAY_SCOREDATA_SetFlag( p_sv,
                          BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_DOUBLE, BSWAY_SETMODE_get );
  EVENTWORK *p_ev  = GAMEDATA_GetEventWork( p_gamedata );
  BOOL  is_gameclear  = EVENTWORK_CheckEventFlag( p_ev, SYS_FLAG_GAME_CLEAR );

  return is_clear && is_gameclear;
}
//----------------------------------------------------------------------------
/**
 *	@brief  スーパーマルチが出現しているか
 *
 *	@param	BSUBWAY_SCOREDATA *p_sv ワーク
 *
 *	@return TRUEで登場  FALSEでまだ
 */
//-----------------------------------------------------------------------------
static BOOL Br_BtlSubway_IsAppearMulti(  BSUBWAY_SCOREDATA *p_sv, GAMEDATA *p_gamedata )
{
  BOOL  is_clear  =  BSUBWAY_SCOREDATA_SetFlag( p_sv,
                          BSWAY_SCOREDATA_FLAG_BOSS_CLEAR_MULTI, BSWAY_SETMODE_get );
  EVENTWORK *p_ev  = GAMEDATA_GetEventWork( p_gamedata );
  BOOL  is_gameclear  = EVENTWORK_CheckEventFlag( p_ev, SYS_FLAG_GAME_CLEAR );

  return is_clear && is_gameclear;
}
//=============================================================================
/**
 *      OBJ数字
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  OBJ数字 初期化
 *
 *	@param	OBJNUMBER_WORK *p_wk  ワーク
 *	@param	*p_clunit             CLUNIT
 *	@param	number                セットする数字
 *	@param  原点座標
 */
//-----------------------------------------------------------------------------
static void OBJNUMBER_Init( OBJNUMBER_WORK *p_wk, GFL_CLUNIT *p_clunit, const BR_RES_OBJ_DATA *cp_obj_res, int number, s16 x, s16 y, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(OBJNUMBER_WORK) );

  //CLWK作成
  { 
    int i;
    GFL_CLWK_DATA cldata;


    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    //BP
    cldata.pos_x  = x;
    cldata.pos_y  = y;
    cldata.anmseq = 10;
    p_wk->p_clwk = GFL_CLACT_WK_Create( p_clunit,
          cp_obj_res->ncg, cp_obj_res->ncl,cp_obj_res->nce,
            &cldata, CLSYS_DEFREND_MAIN, heapID );
    GFL_CLACT_WK_SetObjMode( p_wk->p_clwk, GX_OAM_MODE_XLU );

    //数字
    cldata.anmseq = 0;
    for( i = 0; i < OBJNUMBER_MAX; i++ )
    {
      cldata.pos_x  = x + 20 + i * 16;
      cldata.pos_y  = y;

      p_wk->p_num[i] = GFL_CLACT_WK_Create( p_clunit,
          cp_obj_res->ncg, cp_obj_res->ncl,cp_obj_res->nce,
            &cldata, CLSYS_DEFREND_MAIN, heapID );

      GFL_CLACT_WK_SetObjMode( p_wk->p_num[i], GX_OAM_MODE_XLU );
    }
  }

  //数字をセット
  ObjNumber_SetNumber( p_wk, number );
}
//----------------------------------------------------------------------------
/**
 *	@brief  OBJ数字 破棄
 *
 *	@param	OBJNUMBER_WORK *p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void OBJNUMBER_Exit( OBJNUMBER_WORK *p_wk )
{
  //CLWK破棄
  { 
    int i;
    for( i = 0; i < OBJNUMBER_MAX; i++ )
    { 
      if( p_wk->p_num[i] )
      { 
        GFL_CLACT_WK_Remove( p_wk->p_num[i] );
        p_wk->p_num[i] = NULL;
      }
    }
    GFL_CLACT_WK_Remove( p_wk->p_clwk );
  }

  GFL_STD_MemClear( p_wk, sizeof(OBJNUMBER_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJに番号をセット
 *
 *	@param	OBJNUMBER_WORK *p_wk	ワーク
 *	@param	number							番号
 *
 */
//-----------------------------------------------------------------------------
static void ObjNumber_SetNumber( OBJNUMBER_WORK *p_wk, int number )
{	
	static const u32 sc_num_table[] = 
	{
		1,
		10,
		100,
		1000,
    10000,
	};
	u8 fig;
	u8 n;
	int i;
  int anmseq;

	//何桁か調べる
	n		= number;
	fig	= 0;
	if( n == 0 )
	{	
		fig	= 1;
	}
	else
	{	
		while( 1 )
		{
			if( n == 0 )
			{	
				break;
			}
			n	/= 10;
			fig++;
		}
	}
	

	//一端消去
	for( i = 0; i < OBJNUMBER_MAX; i++ )
	{
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_num[i], 0 );
	}

	//OBJのアニメを桁の数値に合わせる
	for( i = 0; i < /*fig*/OBJNUMBER_MAX; i++ )
	{
		anmseq	= number % sc_num_table[ i+1 ] / sc_num_table[ i ];
		//+1はセルの順番
		GFL_CLACT_WK_SetAnmSeq( p_wk->p_num[OBJNUMBER_MAX-1-i], anmseq );
	}
}

