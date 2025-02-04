//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mystery.c
 *	@brief  ふしぎなおくりものプロセス
 *	@author	Toru=Nagihashi
 *	@data		2009.12.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//ライブラリ
#include <gflib.h>

//システム
#include "system/main.h"
#include "gamesystem/game_data.h"
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"
#include "print/wordset.h"
#include "system/bmp_winframe.h"
#include "system/ds_system.h"

//アーカイブ
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "arc\mystery.naix"
#include "msg/msg_mystery.h"
#include "item/item.h"
#include "item_icon.naix"
#include "system/poke2dgra.h"
#include "script_message.naix"
#include "msg/script/msg_common_scr.h"
#include "mystery_spa.h"

//セーブデータ
#include "savedata/mystery_data.h"

//外部プロセス
#include "net_app/wifi_login.h"
#include "title/title.h"

//ふしぎなおくりもの内モジュール
#include "mystery_graphic.h"
#include "mystery_util.h"
#include "mystery_album.h"
#include "mystery_net.h"
#include "mystery_snd.h"
#include "mystery_inline.h"
#include "net_app/mystery_data_util.h"

//デバッグ
#include "mystery_debug.h"
#include "debug/debug_str_conv.h"
#include "debug/debugwin_sys.h"
#include "debug/debug_ui.h"

//外部公開
#include "net_app/mystery.h"

//-------------------------------------
///	オーバーレイ
//=====================================
FS_EXTERN_OVERLAY(dpw_common);

//-------------------------------------
///	デバッグ
//=====================================
#ifdef PM_DEBUG

//#define MYSTERY_SAVEDATA_CLEAR
#define MYSTERY_MOVIE_DEMO  //Rボタン押せば強制映画デモ
#define MYSTERY_PRINT_ON    //デバッグ出力

//#define MYSTERY_AUTO_DEBUG  //オートデバッグ

#endif //PM_DEBUG

#ifdef MYSTERY_PRINT_ON
#ifdef DEBUG_ONLY_FOR_toru_nagihashi
#define MYSTERY_Printf(...)  OS_Printf( __VA_ARGS__ )
#elif DEBUG_ONLY_FOR_shimoyamada
#define MYSTERY_Printf(...)  OS_Printf( __VA_ARGS__ )
#endif //DEBUG_ONLY_FOR_
#endif  //MYSTERY_PRINT_ON

#ifndef MYSTERY_Printf
#define MYSTERY_Printf(...)  /*  */
#endif


#ifdef MYSTERY_AUTO_DEBUG  //オートデバッグ
static int s_debug_flag = 0;
static int s_debug_cnt = 0;
#endif // MYSTERY_AUTO_DEBUG

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	BGフレーム
//=====================================
enum
{
	//メイン画面
	BG_FRAME_M_LIST 	=	GFL_BG_FRAME0_M, 
	BG_FRAME_M_TEXT	  =	GFL_BG_FRAME1_M, 
	BG_FRAME_M_BACK1	=	GFL_BG_FRAME2_M, 
	BG_FRAME_M_BACK2	=	GFL_BG_FRAME3_M, 

	//サブ画面
	BG_FRAME_S_TEXT	  =	GFL_BG_FRAME0_S, 
	BG_FRAME_S_BACK1	=	GFL_BG_FRAME2_S, 
	BG_FRAME_S_BACK2	=	GFL_BG_FRAME3_S, 
} ;

//-------------------------------------
///	パレット
//=====================================
enum
{
	//メイン画面BG
	PLT_BG_BACK_M	=	0,
  PLT_BG_RECV_M = 4,
  PLT_BG_CARD_M = 8,
  PLT_BG_LIST_M = 13,
  PLT_BG_TEXT_M = 14,
	PLT_BG_FONT_M = 15,

	//サブ画面BG
	PLT_BG_BACK_S	= 0,
  PLT_BG_CARD_S  = 1,
	PLT_BG_FONT_S	= 15,

	//メイン画面OBJ
	PLT_OBJ_CURSOR_M  = 0,
  PLT_OBJ_GIFT_M    = 14,
  PLT_OBJ_CARD_ICON_M       = 10,
  PLT_OBJ_CARD_SILHOUETTE_M = 13,

	//サブ画面OBJ
} ;

//-------------------------------------
///	OBJ_WORK
//=====================================
enum
{ 
  OBJ_RES_PLT_CURSOR  = 0, 
  OBJ_RES_PLT_MAX,

  OBJ_RES_CGX_CURSOR  = 0,
  OBJ_RES_CGX_MAX,

  OBJ_RES_CEL_CURSOR  = 0,
  OBJ_RES_CEL_MAX,
};

#define EFFECT_BRIGHT_MAX  (32)
enum
{
	OBJ_CLWKID_CURSOR,
  OBJ_CLWKID_BRIGHT_TOP,
  OBJ_CLWKID_BRIGHT_END = OBJ_CLWKID_BRIGHT_TOP + EFFECT_BRIGHT_MAX,

  OBJ_CLWKID_BTN,
	OBJ_CLWKID_MAX,
} ;

//-------------------------------------
///	キャラ
//=====================================
enum
{ 
  //メイン画面  BG_FRAME_M_TEXT
  BG_CGX_OFS_M_CLEAR = 0,
  BG_CGX_OFS_M_LIST  = 1,
  BG_CGX_OFS_M_TEXT  = 10,
};

//-------------------------------------
///	BG読み込み
//=====================================
typedef enum
{
  BG_LOAD_TYPE_INIT,  //初期グラ
  BG_LOAD_TYPE_GUIDE_S, //下画面ガイドBG
  BG_LOAD_TYPE_STAGE_M, //デモ用ステージ
} BG_LOAD_TYPE;


//-------------------------------------
///	通信方法
//=====================================
typedef enum
{
  MYSTERY_NET_MODE_WIRELESS,
  MYSTERY_NET_MODE_WIFI,
  MYSTERY_NET_MODE_IRC,
} MYSTERY_NET_MODE;

//-------------------------------------
///	デモ
//=====================================
#define MYSTERY_DEMO_GIFT_X             (128)
#define MYSTERY_DEMO_MOVE_GIFT_START_Y  (-96)
#define MYSTERY_DEMO_MOVE_GIFT_END_Y    (118)
#define MYSTERY_DEMO_MOVE_GIFT_SYNC     (60)
#define MYSTERY_DEMO_INIT_WAIT_SYNC     (180)
#define MYSTERY_DEMO_END_WAIT_SYNC      (60)
#define MYSTERY_DEMO_STAGE_FADE_SYNC    (120)
#define MYSTERY_DEMO_POKE_FADE_SYNC1     (10)
#define MYSTERY_DEMO_POKE_WAIT_SYNC     (10)
#define MYSTERY_DEMO_POKE_FADE_SYNC2     (120)
#define MYSTERY_DEMO_POKE_END_SYNC     (60)
#define MYSTERY_DEMO_FINISH_FADE_SYNC    (60)
#define MYSTERY_DEMO_PTC_FALL_SYNC      (120)

//-------------------------------------
///	演出
//=====================================
#define MYSTERY_EFFECT_DEFAULT_SCROLL_SPEED     (-FX32_CONST(0.2))
#define MYSTERY_EFFECT_DEMO_SCROLL_SPEED        (-FX32_CONST(4.0))
#define MYSTERY_EFFECT_DEMO_SCROLL_CHANGE_SYNC  (60)

//演出タイプ
typedef enum
{ 
  MYSTERY_EFFECT_TYPE_NORMAL,
  MYSTERY_EFFECT_TYPE_DEMO,
} MYSTERY_EFFECT_TYPE;


#define MYSTERY_MENU_ALPHA_EV1  (15)
#define MYSTERY_MENU_ALPHA_EV2  (3)


#define MYSTERY_RECV_TIMEOUT    (120*60)

#define MYSTERY_RECV_CARD_END_BTN (4*60)

//-------------------------------------
///	デモ
//=====================================
typedef enum
{ 
  MYSTERY_DEMO_TYPE_RECV, //受信時
  MYSTERY_DEMO_TYPE_FINISH, //受信終了時
} MYSTERY_DEMO_TYPE;

//-------------------------------------
///	メニューの数
//=====================================
typedef enum
{ 
  UTIL_MENU_TYPE_TOP,
  UTIL_MENU_TYPE_NETMODE,
  UTIL_MENU_TYPE_GIFT,
  UTIL_MENU_TYPE_INFO,

  UTIL_MENU_TYPE_MAX,
}UTIL_MENU_TYPE;

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	BGリソース管理
//=====================================
typedef struct 
{
  HEAPID  heapID;
} BG_WORK;
//-------------------------------------
///	OBJリソース管理
//=====================================
typedef struct 
{
  u32       res_plt[OBJ_RES_PLT_MAX]; //リソースパレット
  u32       res_cgx[OBJ_RES_CGX_MAX]; //リソースキャラクター
  u32       res_cel[OBJ_RES_CEL_MAX]; //リソースセル
  GFL_CLWK  *p_clwk[OBJ_CLWKID_MAX];  //CLWK

  u16 plt_cnt;
  u16 plt[0x10];
  u16 plt_src[0x10];
  u16 plt_dst[0x10];
} OBJ_WORK;

//-------------------------------------
///	ボタン
//=====================================
typedef struct 
{
  BMPOAM_SYS_PTR      p_bmpoam_sys;
  MYSTERY_MSGOAM_WORK *p_msgoam;
  const OBJ_WORK      *cp_obj;
} MYSTERY_BTN_WORK;

//-------------------------------------
///	パーティクル
//=====================================
typedef struct 
{
  GFL_PTC_PTR   p_ptc;
  GFL_EMIT_PTR  p_emitter;
  void          *p_buffer;
  BOOL          is_use;
} MYSTERY_PTC_WORK;

//-------------------------------------
///	エフェクトワーク(光の玉と背景のスクロール)
//=====================================
typedef struct _MYSTERY_EFFECT_WORK MYSTERY_EFFECT_WORK;
typedef void (*MYSTERY_EFFECT_MOVE_FUNCTION)( MYSTERY_EFFECT_WORK *p_wk );
struct _MYSTERY_EFFECT_WORK
{
  BOOL is_start;
  BOOL is_update;
  u16   cnt;
  fx32  scroll_now;
  fx32  scroll_add;
  fx32  init_add;
  fx32  bright_pos[EFFECT_BRIGHT_MAX];
  fx32  bright_add[EFFECT_BRIGHT_MAX];
  const OBJ_WORK *cp_obj;
  MYSTERY_EFFECT_MOVE_FUNCTION  move_function;
  u32 seq;
  HEAPID heapID;
} ;

//-------------------------------------
///	受信デモワーク
//=====================================
typedef struct _MYSTERY_DEMO_WORK MYSTERY_DEMO_WORK;
typedef void (*MYSTERY_DEMO_MOVE_FUNCTION)( MYSTERY_DEMO_WORK *p_wk );
struct _MYSTERY_DEMO_WORK
{
  HEAPID  heapID;
  GFL_CLWK  *p_clwk;
  BOOL      is_end;
  u32       res_plt;
  u32       res_cgx;
  u32       res_cel;
  u32       sync;
  u32       fade_sync;
  u32       seq;

  BG_WORK *p_bg;
  MYSTERY_EFFECT_WORK *p_effect;
  MYSTERY_DEMO_MOVE_FUNCTION  move_function;

  //BGパレット転送用
  u16 plt_cnt;
  u16 plt[0x10];
  u16 plt_sub[0x10];
  u16 plt_src[0x10];
  u16 plt_dst[0x10];

  //ポケモンOBJパレット転送用
  u16 plt_poke[0x10];
  u16 plt_poke_src[0x10]; //白
  u16 plt_poke_dst[0x10]; //ポケモンの色　

  const DOWNLOAD_GIFT_DATA *cp_data;
  MYSTERY_PTC_WORK  ptc;

  s16               leg_pos;
  s16               x_ofs;

} ;

//-------------------------------------
///	メインワーク
//=====================================
typedef struct 
{ 
  //カード画面
  MYSTERY_CARD_WORK         *p_card;
  MYSTERY_CARD_RES          *p_card_res;

  //アルバム
  MYSTERY_ALBUM_WORK        *p_album;

  //デモ
  MYSTERY_DEMO_WORK         demo;

  //演出
  MYSTERY_EFFECT_WORK       effect;
    
  //選択肢
  MYSTERY_LIST_WORK         *p_list;

  MYSTERY_BTN_WORK          btn;

  //メニュー
  MYSTERY_MENU_WORK         *p_menu;
  MYSTERY_MENU_DATA         menu_data[ UTIL_MENU_TYPE_MAX ];

  //テキスト
  MYSTERY_TEXT_WORK         *p_text;

  //メイン画面用BMPWINセット
  MYSTERY_MSGWINSET_WORK    *p_winset_m;

  //サブ画面用BMPWINセット
  MYSTERY_MSGWINSET_WORK    *p_winset_s;

  //シーケンス管理
  MYSTERY_SEQ_WORK          *p_seq;

	//OBJリソース
	OBJ_WORK		            	obj;

	//BGリソース
	BG_WORK				            bg;

  //グラフィック設定
  MYSTERY_GRAPHIC_WORK      *p_graphic;

  //共通で使うフォント
	GFL_FONT			            *p_font;

	//共通で使うキュー
	PRINT_QUE				          *p_que;

	//共通で使うエラーメッセージ
	GFL_MSGDATA	          		*p_err_msg;

	//共通で使うメッセージ
	GFL_MSGDATA	          		*p_msg;

	//共通で使う単語
	WORDSET				          	*p_word;

  //通信方法
  MYSTERY_NET_MODE          mode;

  //通信
  MYSTERY_NET_WORK          *p_net;

  //引数
  MYSTERY_PARAM             *p_param;

  //ゲームデータ
  GAMEDATA                  *p_gamedata;

  //セーブデータ
  MYSTERY_DATA              *p_sv;

  //ログインプロセスへの引数
  WIFILOGIN_PARAM           *p_wifilogin_param;

  //汎用カウンタ
  u32                       cnt;
  BOOL                      is_delete;  //カード削除した直後

  //取得データ
  DOWNLOAD_GIFT_DATA        data;
} MYSTERY_WORK;

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
///	プロセス
//=====================================
static GFL_PROC_RESULT MYSTERY_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT MYSTERY_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT MYSTERY_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
///	BGリソース
//=====================================
static void BG_Init( BG_WORK *p_wk, HEAPID heapID );
static void BG_Exit( BG_WORK *p_wk );
static void BG_Load( BG_WORK *p_wk, BG_LOAD_TYPE type );
static void BG_UnLoad( BG_WORK *p_wk, BG_LOAD_TYPE type );
//-------------------------------------
///	OBJリソース
//=====================================
static void OBJ_Init( OBJ_WORK *p_wk, GFL_CLUNIT *p_clunit, HEAPID heapID );
static void OBJ_Exit( OBJ_WORK *p_wk );
static GFL_CLWK *OBJ_GetClwk( const OBJ_WORK *cp_wk, u32 clwkID );
static void OBJ_ReLoad( OBJ_WORK *p_wk, HEAPID heapID );
static void OBJ_PltFade_Main( OBJ_WORK *p_wk );
static void OBJ_PltFade_Reset( OBJ_WORK *p_wk );
//-------------------------------------
///	パーティクル
//=====================================
static void MYSTERY_PTC_Init( MYSTERY_PTC_WORK *p_wk, HEAPID heapID );
static void MYSTERY_PTC_Exit( MYSTERY_PTC_WORK *p_wk );
static void MYSTERY_PTC_LoadResource( MYSTERY_PTC_WORK *p_wk, ARCID arcID, ARCDATID datID, HEAPID heapID );
static void MYSTERY_PTC_Start( MYSTERY_PTC_WORK *p_wk, u32 id );
static BOOL MYSTERY_PTC_IsUse( const MYSTERY_PTC_WORK *cp_wk );
//-------------------------------------
///	SEQFUNC
//=====================================
static void SEQFUNC_Start( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_FadeIn( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_FadeOut( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_StartSelect( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );  //開始選択
static void SEQFUNC_Infomation( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );   //説明
static void SEQFUNC_RecvGift( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );     //贈り物受け取り
static void SEQFUNC_Demo( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );         //受け取りデモ
static void SEQFUNC_CardAlbum( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );    //アルバム
static void SEQFUNC_DeleteCard( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );   //カード１枚削除
static void SEQFUNC_WifiLogin( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_End( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

static void SEQFUNC_EnableWireless( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	UTIL  Mystery_utilのモジュールをパックしたもの
//=====================================
//はいいいえなどのリスト
typedef enum
{ 
  UTIL_LIST_TYPE_YESNO,
}UTIL_LIST_TYPE;
static void UTIL_CreateList( MYSTERY_WORK *p_wk, UTIL_LIST_TYPE type, HEAPID heapID );
static void UTIL_DeleteList( MYSTERY_WORK *p_wk );
static void UTIL_CreateMenu( MYSTERY_WORK *p_wk, UTIL_MENU_TYPE type, HEAPID heapID );
static void UTIL_DeleteMenu( MYSTERY_WORK *p_wk );
static u32 UTIL_MainMenu( MYSTERY_WORK *p_wk );
static void Util_MenuCallback( void *p_wk_adrs );
//ガイドテキスト
static void UTIL_CreateGuideText( MYSTERY_WORK *p_wk, HEAPID heapID );
static void UTIL_DeleteGuideText( MYSTERY_WORK *p_wk );
//-------------------------------------
///	デモ
//=====================================
static void MYSTERY_DEMO_Init( MYSTERY_DEMO_WORK *p_wk, GFL_CLUNIT *p_unit, const DOWNLOAD_GIFT_DATA *cp_data, GAMEDATA *p_gamedata, const OBJ_WORK *cp_obj, BG_WORK *p_bg, MYSTERY_EFFECT_WORK *p_effect, HEAPID heapID );
static void MYSTERY_DEMO_Start( MYSTERY_DEMO_WORK *p_wk, MYSTERY_DEMO_TYPE type );
static void MYSTERY_DEMO_Exit( MYSTERY_DEMO_WORK *p_wk );
static void MYSTERY_DEMO_Main( MYSTERY_DEMO_WORK *p_wk );
static BOOL MYSTERY_DEMO_IsEnd( const MYSTERY_DEMO_WORK *cp_wk );
static void Mystery_Demo_NormalMain( MYSTERY_DEMO_WORK *p_wk );
static void Mystery_Demo_MovieMain( MYSTERY_DEMO_WORK *p_wk );
static void Mystery_Demo_FinishMain( MYSTERY_DEMO_WORK *p_wk );

//-------------------------------------
///	演出
//=====================================
static void MYSTERY_EFFECT_Init( MYSTERY_EFFECT_WORK *p_wk, const OBJ_WORK *cp_obj, HEAPID heapID );
static void MYSTERY_EFFECT_Exit( MYSTERY_EFFECT_WORK *p_wk );
static void MYSTERY_EFFECT_Main( MYSTERY_EFFECT_WORK *p_wk );
static void MYSTERY_EFFECT_Start( MYSTERY_EFFECT_WORK *p_wk, MYSTERY_EFFECT_TYPE type );
static void MYSTERY_EFFECT_SetUpdateFlag( MYSTERY_EFFECT_WORK *p_wk, BOOL is_update );
static void Mystery_Effect_NormalMain( MYSTERY_EFFECT_WORK *p_wk );
static void Mystery_Effect_DemoMain( MYSTERY_EFFECT_WORK *p_wk );
//-------------------------------------
///	ボタン
//=====================================
static void MYSTERY_BTN_Init( MYSTERY_BTN_WORK *p_wk, const OBJ_WORK *cp_obj, GFL_CLUNIT *p_clunit, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font, HEAPID heapID );
static void MYSTERY_BTN_Exit( MYSTERY_BTN_WORK *p_wk );
static void MYSTERY_BTN_PrintMain( MYSTERY_BTN_WORK *p_wk );
//-------------------------------------
///	その他
//=====================================
static u32 Mystery_GetLegPos( const POKEMON_PASO_PARAM* cp_ppp, HEAPID heapID );
static BOOL Mystery_IsValid( const DOWNLOAD_GIFT_DATA *cp_data, u32 dirty );

//=============================================================================
/**
 *					外部データ
*/
//=============================================================================
//-------------------------------------
///	プロセスデータ
//=====================================
const GFL_PROC_DATA MysteryGiftProcData  =
{ 
  MYSTERY_PROC_Init,
  MYSTERY_PROC_Main,
  MYSTERY_PROC_Exit,
};
//=============================================================================
/**
 *          プロセス
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ふしぎなおくりもの	メインプロセス初期化
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT MYSTERY_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  MYSTERY_WORK  *p_wk;
  MYSTERY_PARAM *p_param  = p_param_adrs;

  GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common));

  //ヒープ作成
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MYSTERYGIFT, 0xA0000 );

	//プロセスワーク作成
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(MYSTERY_WORK), HEAPID_MYSTERYGIFT );
	GFL_STD_MemClear( p_wk, sizeof(MYSTERY_WORK) );	
  p_wk->p_param = p_param;

  p_wk->p_gamedata  = GAMEDATA_Create( HEAPID_MYSTERYGIFT );

  p_wk->p_sv    = MYSTERY_DATA_Load( GAMEDATA_GetSaveControlWork(p_wk->p_gamedata), MYSTERYDATA_LOADTYPE_AUTO, HEAPID_MYSTERYGIFT );

  //セーブデータをクリア
#ifdef MYSTERY_SAVEDATA_CLEAR
  { 
    int i;
    for( i = 0; i < MYSTERY_DATA_GetCardMax( p_wk->p_sv ); i++ )
    { 
      if( MYSTERYDATA_IsExistsCard( p_wk->p_sv, i ) )
      { 
        MYSTERYDATA_RemoveCardData( p_wk->p_sv, i );
        i--;  //内部で空いた分詰めるので
      }
    }
  }
#endif //MYSTERY_SAVEDATA_CLEAR


  //グラフィック設定
	p_wk->p_graphic	= MYSTERY_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_MYSTERYGIFT );

	//リソース読み込み
	BG_Init( &p_wk->bg, HEAPID_MYSTERYGIFT );
	{	
		GFL_CLUNIT	*p_clunit	= MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
		OBJ_Init( &p_wk->obj, p_clunit, HEAPID_MYSTERYGIFT );
	}

  //共通モジュールの初期化
  p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_MYSTERYGIFT );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_MYSTERYGIFT );
	p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_mystery_dat, HEAPID_MYSTERYGIFT );
	p_wk->p_err_msg	= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_SCRIPT_MESSAGE, NARC_script_message_common_scr_dat, HEAPID_MYSTERYGIFT );
	p_wk->p_word	= WORDSET_Create( HEAPID_MYSTERYGIFT );

  //モジュール作成
  p_wk->p_seq = MYSTERY_SEQ_Init( p_wk, SEQFUNC_Start, HEAPID_MYSTERYGIFT );
  p_wk->p_text  = MYSTERY_TEXT_Init( BG_FRAME_M_TEXT, PLT_BG_FONT_M, p_wk->p_que, p_wk->p_font, HEAPID_MYSTERYGIFT );
  MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, BG_CGX_OFS_M_TEXT, PLT_BG_TEXT_M );

  p_wk->p_net   = MYSTERY_NET_Init( GAMEDATA_GetSaveControlWork(p_wk->p_gamedata), HEAPID_MYSTERYGIFT );

  MYSTERY_EFFECT_Init( &p_wk->effect, &p_wk->obj, HEAPID_MYSTERYGIFT );
  MYSTERY_EFFECT_Start( &p_wk->effect, MYSTERY_EFFECT_TYPE_NORMAL );
  MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, TRUE );

  PMSND_PlayBGM( SEQ_BGM_WIFI_PRESENT );

#ifdef PM_DEBUG
  DEBUGWIN_InitProc( GFL_BG_FRAME0_M, p_wk->p_font );
#endif //PM_DEBUG
  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ふしぎなおくりもの	メインプロセス破棄
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT MYSTERY_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  MYSTERY_WORK  *p_wk     = p_wk_adrs;
  MYSTERY_PARAM *p_param  = p_param_adrs;

#ifdef PM_DEBUG
  DEBUGWIN_ExitProc();
#endif

  //モジュール破棄
  MYSTERY_EFFECT_Exit( &p_wk->effect );
  MYSTERY_NET_Exit( p_wk->p_net );
  if( p_wk->p_text )
  { 
    MYSTERY_TEXT_Exit( p_wk->p_text );
  }
   MYSTERY_SEQ_Exit( p_wk->p_seq );

	//共通モジュールの破棄
	WORDSET_Delete( p_wk->p_word );
	GFL_MSG_Delete( p_wk->p_err_msg );
	GFL_MSG_Delete( p_wk->p_msg );
	PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_font );

  //リソース破棄
	OBJ_Exit( &p_wk->obj );
	BG_Exit( &p_wk->bg );

  //グラフィック破棄
	MYSTERY_GRAPHIC_Exit( p_wk->p_graphic );

  MYSTERY_DATA_UnLoad( p_wk->p_sv );

  //ゲームデータ破棄
  GAMEDATA_Delete( p_wk->p_gamedata );

	//プロセスワーク破棄
	GFL_PROC_FreeWork( p_proc );

	//ヒープ破棄
	GFL_HEAP_DeleteHeap( HEAPID_MYSTERYGIFT );

  //タイトルに戻る
  //GFL_PROC_SysSetNextProc(FS_OVERLAY_ID(title), &TitleProcData, NULL);
  OS_ResetSystem(0);

  GFL_OVERLAY_Unload( FS_OVERLAY_ID(dpw_common));

  PMSND_StopBGM();

  return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ふしぎなおくりもの	メインプロセス処理
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT MYSTERY_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{ 
  MYSTERY_WORK  *p_wk     = p_wk_adrs;
  MYSTERY_PARAM *p_param  = p_param_adrs;

  //通信処理
  MYSTERY_NET_Main( p_wk->p_net );

  //シーケンス
  MYSTERY_SEQ_Main( p_wk->p_seq );
  
  if( p_wk->p_graphic )
  {
    //2D描画
    MYSTERY_GRAPHIC_2D_Draw( p_wk->p_graphic );

    //3D描画
    MYSTERY_GRAPHIC_3D_StartDraw( p_wk->p_graphic );
    MYSTERY_GRAPHIC_3D_EndDraw( p_wk->p_graphic );
  }
  
  MYSTERY_EFFECT_Main( &p_wk->effect );

  //プリント
	PRINTSYS_QUE_Main( p_wk->p_que );
  if( p_wk->p_text )
  { 
    MYSTERY_TEXT_Main( p_wk->p_text );
  }
  if( p_wk->p_winset_s )
  { 
    MYSTERY_MSGWINSET_PrintMain( p_wk->p_winset_s );
  }
  if( p_wk->p_winset_m )
  { 
    MYSTERY_MSGWINSET_PrintMain( p_wk->p_winset_m );
  }
  if( p_wk->p_menu )
  { 
    MYSTERY_MENU_PrintMain( p_wk->p_menu );
  }

	//終了
	if( MYSTERY_SEQ_IsEnd( p_wk->p_seq ) )
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
 *		BGリソース読みこみ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BGリソース読みこみ
 *
 *	@param	BG_WORK *p_wk	ワーク
 *	@param	heapID				ヒープID
 */
//-----------------------------------------------------------------------------
static void BG_Init( BG_WORK *p_wk, HEAPID heapID )
{	
  GFL_STD_MemClear( p_wk, sizeof(BG_WORK) );
  p_wk->heapID  = heapID;

  BG_Load( p_wk, BG_LOAD_TYPE_INIT );


  //初期は非表示のもの
  GFL_BG_SetVisible( BG_FRAME_S_BACK1, TRUE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGリソース破棄
 *
 *	@param	BG_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void BG_Exit( BG_WORK *p_wk )
{	
  //GFL_BG_FillCharacterRelease( BG_FRAME_M_LIST, 1, BG_CGX_OFS_M_CLEAR );
  //GFL_BG_FillCharacterRelease( BG_FRAME_M_TEXT, 1, BG_CGX_OFS_M_CLEAR );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG読み込み
 *
 *	@param	BG_WORK *p_wk ワーク
 *	@param  type          読み込みタイプ
 */
//-----------------------------------------------------------------------------
static void BG_Load( BG_WORK *p_wk, BG_LOAD_TYPE type )
{ 
  HEAPID  heapID  = GFL_HEAP_LOWID( p_wk->heapID );

  switch( type )
  { 
  case BG_LOAD_TYPE_INIT:
    {	
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );
      //PLT
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_mystery_fusigi_bg_00_NCLR,
          PALTYPE_MAIN_BG, PLT_BG_BACK_M*0x20, 0, heapID );
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_mystery_fusigi_bg_00_NCLR,
          PALTYPE_SUB_BG, PLT_BG_BACK_S*0x20, 0x20*4, heapID );
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_mystery_fusigi_card_NCLR,
          PALTYPE_SUB_BG, PLT_BG_CARD_S*0x20, 0x20, heapID );

      //CHR
      GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fusigi_bg_00_NCGR, 
          BG_FRAME_M_BACK2, 0, 0, FALSE, heapID );
      GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fusigi_bg_00_NCGR, 
          BG_FRAME_S_BACK2, 0, 0, FALSE, heapID );

      //SCR
      GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_mystery_fusigi_bg_00_NSCR,
          BG_FRAME_M_BACK2, 0, 0, FALSE, heapID );
      GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_mystery_fusigi_bg_00_NSCR,
          BG_FRAME_S_BACK2, 0, 0, FALSE, heapID );

      GFL_ARC_CloseDataHandle( p_handle );
    }
    //フォント
    {	
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_FONT, heapID );

      //上下画面フォントパレット
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_font_default_nclr,
          PALTYPE_MAIN_BG, PLT_BG_FONT_M*0x20, 0x20, heapID );
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_font_default_nclr,
          PALTYPE_SUB_BG, PLT_BG_FONT_S*0x20, 0x20, heapID );

      GFL_ARC_CloseDataHandle( p_handle );
    }

    { 
      BmpWinFrame_GraphicSet( BG_FRAME_M_TEXT, BG_CGX_OFS_M_TEXT, PLT_BG_TEXT_M, MENU_TYPE_SYSTEM, heapID );
      BmpWinFrame_GraphicSet( BG_FRAME_M_LIST, BG_CGX_OFS_M_LIST, PLT_BG_LIST_M, MENU_TYPE_SYSTEM, heapID );
    }

    //キャラを単位で読み込み
    GFL_BG_SetClearCharacter( BG_FRAME_M_TEXT, 0x20, BG_CGX_OFS_M_CLEAR, heapID );
    GFL_BG_SetClearCharacter( BG_FRAME_M_LIST, 0x20, BG_CGX_OFS_M_CLEAR, heapID );

    break;

  case BG_LOAD_TYPE_GUIDE_S:
    {	
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );
      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_mystery_fusigi_bg_00_NCLR,
          PALTYPE_SUB_BG, PLT_BG_BACK_S*0x20, 0x20*6, heapID );
      GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fusigi_bg_00_NCGR, 
          BG_FRAME_S_BACK1, 0, 0, FALSE, heapID );
      GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_mystery_fushigi_card03_NSCR,
          BG_FRAME_S_BACK1, 0, 0, FALSE, heapID );
      GFL_ARC_CloseDataHandle( p_handle );
    }
    break;

  case BG_LOAD_TYPE_STAGE_M:
    {	
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );
      GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fushigi_back_NCGR, 
          BG_FRAME_M_BACK1, 0, 0, FALSE, heapID );
      GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_mystery_fushigi_stage_NSCR,
          BG_FRAME_M_BACK1, 0, 0, FALSE, heapID );
      GFL_ARC_CloseDataHandle( p_handle );
    }
    break;
  }
}
//=============================================================================
/**
 *		OBJリソース読みこみ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	OBJリソース読みこみ
 *
 *	@param	OBJ_WORK *p_wk	ワーク
 *	@param	clunit					ユニット
 *	@param	HEAPID heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void OBJ_Init( OBJ_WORK *p_wk, GFL_CLUNIT *p_clunit, HEAPID heapID )
{	
	//リソース読みこみ
	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );

		p_wk->res_plt[ OBJ_RES_PLT_CURSOR ]	= GFL_CLGRP_PLTT_Register( p_handle, 
				NARC_mystery_title_cursol_NCLR, CLSYS_DRAW_MAX, PLT_OBJ_CURSOR_M*0x20, heapID );
		p_wk->res_cel[ OBJ_RES_CEL_CURSOR ]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_mystery_title_cursol_NCER, NARC_mystery_title_cursol_NANR, heapID );
		p_wk->res_cgx[ OBJ_RES_CGX_CURSOR ]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_mystery_title_cursol_NCGR, FALSE, CLSYS_DRAW_MAX, heapID );


    //カーソルのパレットフェード用にパレットをメモリ読み込み
    { 
      void *p_buff;
      NNSG2dPaletteData *p_plt;
      const u16 *cp_plt_adrs;

      //もとのパレットから色情報を保存
      p_buff  = GFL_ARCHDL_UTIL_LoadPalette( p_handle, NARC_mystery_title_cursol_NCLR, &p_plt, heapID );
      cp_plt_adrs = p_plt->pRawData;
      GFL_STD_MemCopy( cp_plt_adrs, p_wk->plt_dst, sizeof(u16) * 0x10 );
      GFL_STD_MemCopy( (u8*)cp_plt_adrs + 1 * 0x20, p_wk->plt_src, sizeof(u16) * 0x10 );

      //パレット破棄
      GFL_HEAP_FreeMemory( p_buff );
    }


		GFL_ARC_CloseDataHandle( p_handle );
	}

	//CLWK登録
	{
		int i;
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

		p_wk->p_clwk[OBJ_CLWKID_CURSOR]	=		GFL_CLACT_WK_Create( p_clunit,
				p_wk->res_cgx[OBJ_RES_CGX_CURSOR],
				p_wk->res_plt[OBJ_RES_PLT_CURSOR],
				p_wk->res_cel[OBJ_RES_CEL_CURSOR],
				&cldata,
				CLSYS_DEFREND_MAIN,
				heapID );
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[OBJ_CLWKID_CURSOR], FALSE );

    for( i = 0; i < EFFECT_BRIGHT_MAX; i++ )
    { 
      cldata.pos_x   = GFUser_GetPublicRand0( 256 );
      cldata.pos_y   = GFUser_GetPublicRand0( 256 ) - (256 + 16);
      cldata.bgpri   = BG_FRAME_M_BACK2;
      cldata.anmseq  = 3 + GFUser_GetPublicRand0( 6 );
      p_wk->p_clwk[OBJ_CLWKID_BRIGHT_TOP + i]	=		GFL_CLACT_WK_Create( p_clunit,
          p_wk->res_cgx[OBJ_RES_CGX_CURSOR],
          p_wk->res_plt[OBJ_RES_PLT_CURSOR],
          p_wk->res_cel[OBJ_RES_CEL_CURSOR],
          &cldata,
          CLSYS_DEFREND_MAIN,
          heapID );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[OBJ_CLWKID_BRIGHT_TOP + i], FALSE );
    }

    cldata.pos_x   = 128;
    cldata.pos_y   = 177;
    cldata.bgpri   = 0;
    cldata.softpri = 1;
    cldata.anmseq  = 9;
    p_wk->p_clwk[OBJ_CLWKID_BTN]	=		GFL_CLACT_WK_Create( p_clunit,
				p_wk->res_cgx[OBJ_RES_CGX_CURSOR],
				p_wk->res_plt[OBJ_RES_PLT_CURSOR],
				p_wk->res_cel[OBJ_RES_CEL_CURSOR],
				&cldata,
				CLSYS_DEFREND_MAIN,
				heapID );
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[OBJ_CLWKID_BTN], FALSE );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJリソース破棄
 *
 *	@param	BG_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void OBJ_Exit( OBJ_WORK *p_wk )
{	
	//CLWK破棄
	{	
		int i;
		for( i = 0; i < OBJ_CLWKID_MAX; i++ )
		{	
			if( p_wk->p_clwk[i] )
			{	
				GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
			}
		}
	}

	//リソース破棄
	{
    int i;
    for( i = 0; i < OBJ_RES_PLT_MAX; i++ )
    { 
      GFL_CLGRP_PLTT_Release( p_wk->res_plt[i] );
    }
    for( i = 0; i < OBJ_RES_CGX_MAX; i++ )
    { 
      GFL_CLGRP_CGR_Release( p_wk->res_cgx[i] );
    }
    for( i = 0; i < OBJ_RES_CEL_MAX; i++ )
    { 
      GFL_CLGRP_CELLANIM_Release( p_wk->res_cel[i] );
    }
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	CLWK取得
 *
 *	@param	const OBJ_WORK *p_wk		ワーク
 *	@param	clwkID									CLWK番号
 *
 *	@return	CLWK
 */
//-----------------------------------------------------------------------------
static GFL_CLWK *OBJ_GetClwk( const OBJ_WORK *cp_wk, u32 clwkID )
{	
	return cp_wk->p_clwk[ clwkID ];
}

//----------------------------------------------------------------------------
/**
 *	@brief  再読み込み
 *
 *	@param	OBJ_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void OBJ_ReLoad( OBJ_WORK *p_wk, HEAPID heapID )
{ 
  //リソース読みこみ
	{	
    void *p_plt_buff;
    NNSG2dPaletteData *p_plt_data;
    p_plt_buff  = GFL_ARC_UTIL_LoadPalette( ARCID_MYSTERY, NARC_mystery_title_cursol_NCLR, &p_plt_data, heapID );
    GFL_CLGRP_PLTT_Replace( p_wk->res_plt[ OBJ_RES_PLT_CURSOR ], p_plt_data, 7 );
    GFL_HEAP_FreeMemory( p_plt_buff );
	} 
}
//----------------------------------------------------------------------------
/**
 *	@brief  パレットフェードメイン
 *
 *	@param	OBJ_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void OBJ_PltFade_Main( OBJ_WORK *p_wk )
{ 
  //パレットフェード
  { 
    int i;
    const u16 add = 0x400;

    //パレットアニメ
    if( p_wk->plt_cnt + add >= 0x10000 )
    {
      p_wk->plt_cnt = p_wk->plt_cnt+add-0x10000;
    }
    else
    {
      p_wk->plt_cnt += add;
    }

    for( i = 0; i < 5; i++ )  //5以降は別なOBJにパレットを使っている
    { 
      MYSTERY_UTIL_MainPltAnm( NNS_GFD_DST_2D_OBJ_PLTT_MAIN, &p_wk->plt[i], p_wk->plt_cnt, PLT_OBJ_CURSOR_M, i, p_wk->plt_src[i], p_wk->plt_dst[i] );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  パレットフェードカウンタをリセット
 *
 *	@param	OBJ_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void OBJ_PltFade_Reset( OBJ_WORK *p_wk )
{ 
  p_wk->plt_cnt = 0;
}

//=============================================================================
/**
 *					PARTICLE
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  パーティクル初期化
 *
 *	@param	MYSTERY_PTC_WORK *p_wk  ワーク
 *	@param	heapID                ヒープID
 */
//-----------------------------------------------------------------------------
static void MYSTERY_PTC_Init( MYSTERY_PTC_WORK *p_wk, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_PTC_WORK) );
  p_wk->is_use  = TRUE;

  p_wk->p_buffer  = GFL_HEAP_AllocMemory( heapID, PARTICLE_LIB_HEAP_SIZE );
  p_wk->p_ptc     = GFL_PTC_Create( p_wk->p_buffer, PARTICLE_LIB_HEAP_SIZE, TRUE, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief  パーティクル破棄
 *
 *	@param	MYSTERY_PTC_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void MYSTERY_PTC_Exit( MYSTERY_PTC_WORK *p_wk )
{ 
  //GFL_PTC_Delete( p_wk->p_ptc );  //GFL_PTC_Exitで自動的に解放されるのでいらない
  GFL_HEAP_FreeMemory( p_wk->p_buffer );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_PTC_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  パーティクル  リソース読み込み
 *  
 *	@param	MYSTERY_PTC_WORK *p_wk  ワーク
 *	@param	arcID                   アークID
 *	@param	datID                   データID
 *	@param	heapID                  ヒープID
 */
//-----------------------------------------------------------------------------
static void MYSTERY_PTC_LoadResource( MYSTERY_PTC_WORK *p_wk, ARCID arcID, ARCDATID datID, HEAPID heapID )
{ 
  void *p_resource;
  int i;
  p_resource = GFL_PTC_LoadArcResource( arcID, datID, heapID );
  GFL_PTC_SetResource( p_wk->p_ptc, p_resource, TRUE, NULL );
}
//----------------------------------------------------------------------------
/**
 *	@brief  パーティクル  開始
 *
 *	@param	MYSTERY_PTC_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void MYSTERY_PTC_Start( MYSTERY_PTC_WORK *p_wk, u32 id )
{ 
  GFL_PTC_CreateEmitterCallback( p_wk->p_ptc, id, NULL, p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  パーティクル使用しているか
 *
 *	@param	const MYSTERY_PTC_WORK *cp_wk   ワーク
 *
 *	@return TRUEで使用中  FALSEで使用してない
 */
//-----------------------------------------------------------------------------
static BOOL MYSTERY_PTC_IsUse( const MYSTERY_PTC_WORK *cp_wk )
{ 
  return cp_wk->is_use;
}
//=============================================================================
/**
 *					SEQFUNC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	開始
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_wk_adrs				ワーク
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Start( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  MYSTERY_WORK  *p_wk     = p_wk_adrs;  
  
  MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_001, MYSTERY_TEXT_TYPE_QUE );

  UTIL_CreateMenu( p_wk, UTIL_MENU_TYPE_TOP, HEAPID_MYSTERYGIFT ); 

	MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
}
//----------------------------------------------------------------------------
/**
 *	@brief	フェードイン
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_wk_adrs				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeIn( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{	
	enum
	{	
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_END,
	};

	switch( *p_seq )
	{	
	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
		break;

	default:
		GF_ASSERT(0);
	}
	
}
//----------------------------------------------------------------------------
/**
 *	@brief	フェードアウト
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_wk_adrs				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeOut( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
	enum
	{
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};	

	switch( *p_seq )
	{	
	case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, 0 );
    PMSND_FadeOutBGM( 8 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_EXIT;
		}
		break;

	case SEQ_EXIT:
		MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_End );
		break;

	default:
		GF_ASSERT(0);
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	開始選択
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_wk_adrs				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_StartSelect( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{	
  enum
  { 
    SEQ_INIT,
    SEQ_TOP_SELECT,
    SEQ_CONNECT_MSG,
    SEQ_YESNO_INIT,
    SEQ_YESNO_SELECT,
    SEQ_NET_INIT,
    SEQ_NET_WAIT,
    SEQ_NET_SELECT_MSG,
    SEQ_NET_SELECT_WAIT,
    SEQ_NET_YESNO_MSG,
    SEQ_NET_YESNO_INIT,
    SEQ_NET_YESNO_WAIT,
    SEQ_NET_EXIT,
    SEQ_NET_EXIT_WAIT,
    SEQ_NET_EXIT_RET,
    SEQ_NET_EXIT_RET_WAIT,

    //カードがいっぱいで削除シーケンスへ
    SEQ_FULL_YESNO_MSG,
    SEQ_FULL_YESNO_INIT,
    SEQ_FULL_YESNO_WAIT,

    SEQ_MSG_WAIT,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  //シーケンス
  switch( *p_seq )
  { 
  case SEQ_INIT:
    if( p_wk->is_delete )
    { 
      //カード捨てた直後ならば通信確率へ
      p_wk->is_delete = FALSE;
      *p_seq  = SEQ_CONNECT_MSG;
    }
    else
    { 
      MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_001, MYSTERY_TEXT_TYPE_QUE );
      if( p_wk->p_menu == NULL )
      {
        UTIL_CreateMenu( p_wk, UTIL_MENU_TYPE_TOP, HEAPID_MYSTERYGIFT ); 
      }
      *p_seq  = SEQ_TOP_SELECT;
    }
    break;

  case SEQ_TOP_SELECT:
    //おくりものをうけとるorカードアルバムをみるorやめる
    { 
      u32 ret;
      ret = UTIL_MainMenu( p_wk ); 
      if( ret != MYSTERY_MENU_SELECT_NULL )
      {
        if( ret == 0 )
        { 
          BOOL  is_full;
          BOOL  is_no_recv  = TRUE;

          //おくりものをうけとる
          UTIL_DeleteMenu( p_wk );

          is_full = !MYSTERYDATA_CheckCardDataSpace( p_wk->p_sv );

          //未取得の贈り物データがMAXかチェック
          if( is_full )
          { 
            int i;
            for( i = 0; i < MYSTERY_DATA_GetCardMax( p_wk->p_sv ); i++ )
            { 
              if( MYSTERYDATA_IsHavePresent( p_wk->p_sv, i ) )
              { 
                is_no_recv  = FALSE;
              }
            }
          }

          if( is_full )
          { 
            if( is_no_recv )
            {
              u16 strID;
              //未取得の贈り物データがMAX
              //ゲーム開始前とゲーム開始後でエラーが違う
              if( MYSTERY_DATA_TYPE_OUTSIDE == MYSTERY_DATA_GetDataType( p_wk->p_sv ) )
              { 
                //管理外セーブならば、ゲーム開始前メッセージ
                strID = syachi_mystery_err_008;
              }
              else
              { 
                strID = syachi_mystery_err_001;
              }
              
              MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, strID, MYSTERY_TEXT_TYPE_STREAM );
              MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_INIT );
              *p_seq  = SEQ_MSG_WAIT;
            }
            else
            { 
              //カードの数がMAX
              *p_seq  = SEQ_FULL_YESNO_MSG;
            }
          }
          else
          { 
            if( !DS_SYSTEM_IsAvailableWireless() )
            { 
              //DSの無線設定で通信不可のとき
              MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_EnableWireless );
            }
            else
            { 
              //通常時なので受け取れる
              *p_seq  = SEQ_CONNECT_MSG;
            }
          }
        }
        else if( ret == 1 )
        { 
          //カードアルバムをみる
          UTIL_DeleteMenu( p_wk );
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_CardAlbum );
        }
        else if( ret == 2)
        { 
          //せつめいをきく
          UTIL_DeleteMenu( p_wk );
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_Infomation );
        }
        else if( ret == 3 )
        {
          //やめる
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_FadeOut );
        }
      }
    }
    break;
    
  case SEQ_CONNECT_MSG:
    //通常時  受け取れる
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_002, MYSTERY_TEXT_TYPE_STREAM );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_YESNO_INIT );
    *p_seq  = SEQ_MSG_WAIT;
    break;

  case SEQ_YESNO_INIT:
    UTIL_CreateList( p_wk, UTIL_LIST_TYPE_YESNO, HEAPID_MYSTERYGIFT );
    *p_seq  = SEQ_YESNO_SELECT;
    break;

  case SEQ_YESNO_SELECT:
    //つうしんを開始しますよろしいですか
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        UTIL_DeleteList( p_wk );
        if( ret == 0 )
        { 
          //はい
          *p_seq  = SEQ_NET_INIT;
        }
        else if( ret == 1 )
        { 
          //いいえ
          *p_seq  = SEQ_INIT;
        }
      }
    }
    break;
  
  case SEQ_NET_INIT:
    MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_START_BEACON );
    *p_seq  = SEQ_NET_WAIT;
    break;

  case SEQ_NET_WAIT:
    if( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_MAIN_BEACON )
    {
      *p_seq  = SEQ_NET_SELECT_MSG;
    }
    break;

  case SEQ_NET_SELECT_MSG:
    UTIL_DeleteMenu( p_wk );
    UTIL_CreateMenu( p_wk, UTIL_MENU_TYPE_NETMODE, HEAPID_MYSTERYGIFT );
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_003, MYSTERY_TEXT_TYPE_QUE );
    *p_seq  = SEQ_NET_SELECT_WAIT;
    break;

  case SEQ_NET_SELECT_WAIT:
    { 
      int i;
      u32 ret;
      GAME_COMM_STATUS_BIT  bit;

      bit = MYSTERY_NET_GetCommStatus( p_wk->p_net );
      if( bit & (GAME_COMM_STATUS_BIT_WIRELESS_FU) )
      { 
        MYSTERY_MENU_SetBlink( p_wk->p_menu, 0, TRUE );
      }
      else
      { 
        MYSTERY_MENU_SetBlink( p_wk->p_menu, 0, FALSE );
      }

      //選択
      ret = UTIL_MainMenu( p_wk ); 
      if( ret != MYSTERY_MENU_SELECT_NULL )
      { 
        UTIL_DeleteMenu( p_wk );

        switch( ret )
        { 
        case 0: //ワイヤレス
          p_wk->mode  = MYSTERY_NET_MODE_WIRELESS;
          *p_seq  = SEQ_NET_YESNO_MSG;
          break;
        case 1: //Wi-Fi
          p_wk->mode  = MYSTERY_NET_MODE_WIFI;
          *p_seq  = SEQ_NET_EXIT;
          break;
        case 2: //赤外線
          p_wk->mode  = MYSTERY_NET_MODE_IRC;
          *p_seq  = SEQ_NET_YESNO_MSG;
          break;
        case 3: //もどる
          *p_seq  = SEQ_NET_EXIT_RET;
          break;
        }
      }
    }
    break;

  case SEQ_NET_YESNO_MSG:
    //ワイヤレスorWIFIor赤外線でせつぞくします
    switch( p_wk->mode )
    {
    case MYSTERY_NET_MODE_WIRELESS:
      MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_007, MYSTERY_TEXT_TYPE_STREAM );
      MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_NET_YESNO_INIT );
      *p_seq  = SEQ_MSG_WAIT;
      break;

    case MYSTERY_NET_MODE_WIFI:
      GF_ASSERT(0); //ここへはこない
      break;

    case MYSTERY_NET_MODE_IRC:
      MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_009, MYSTERY_TEXT_TYPE_STREAM );
      MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_NET_YESNO_INIT );
      *p_seq  = SEQ_MSG_WAIT;
      break;
    } 
    break;

  case SEQ_NET_YESNO_INIT:
    UTIL_CreateList( p_wk, UTIL_LIST_TYPE_YESNO, HEAPID_MYSTERYGIFT );
    *p_seq  = SEQ_NET_YESNO_WAIT;
    break;

  case SEQ_NET_YESNO_WAIT:
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        UTIL_DeleteList( p_wk );
        if( ret == 0 )
        { 
          //はい
          *p_seq  =  SEQ_NET_EXIT;
        }
        else if( ret == 1 )
        { 
          //いいえ
          *p_seq  = SEQ_NET_SELECT_MSG;
        }
      }
    }
    break;

  //切断して先へ
  case SEQ_NET_EXIT:
    if( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_MAIN_BEACON )
    { 
      MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_END_BEACON );
      *p_seq  = SEQ_NET_EXIT_WAIT;
    }
    break;

  case SEQ_NET_EXIT_WAIT:
    if( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_WAIT )
    {
      //はい
      if( p_wk->mode == MYSTERY_NET_MODE_WIFI )
      { 
        MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_WifiLogin );
      }
      else
      { 
        MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_RecvGift );
      }
    }
    break;

  //切断して戻る
  case SEQ_NET_EXIT_RET:
    if( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_MAIN_BEACON )
    { 
      MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_END_BEACON );
      *p_seq  = SEQ_NET_EXIT_RET_WAIT;
    }
    else
    { 
      *p_seq  = SEQ_INIT;
    }
    break;

  case SEQ_NET_EXIT_RET_WAIT:
    if( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_WAIT )
    {
      *p_seq  = SEQ_INIT;
    }
    break;

    //-------------------------------------
    ///	カードいっぱいで整理シーケンスへ
    //=====================================
  case SEQ_FULL_YESNO_MSG:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_err_005, MYSTERY_TEXT_TYPE_STREAM );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_FULL_YESNO_INIT );
    *p_seq  = SEQ_MSG_WAIT;
    break;

  case SEQ_FULL_YESNO_INIT:
    UTIL_CreateList( p_wk, UTIL_LIST_TYPE_YESNO, HEAPID_MYSTERYGIFT );
    *p_seq  = SEQ_FULL_YESNO_WAIT;
    break;

  case SEQ_FULL_YESNO_WAIT:
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        UTIL_DeleteList( p_wk );
        if( ret == 0 )
        { 
          //はい
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_DeleteCard );
        }
        else if( ret == 1 )
        { 
          //いいえ
          *p_seq  = SEQ_INIT;
        }
      }
    }
    break;
    
  case SEQ_MSG_WAIT:
    if( MYSTERY_TEXT_IsEndPrint( p_wk->p_text ) )
    { 
      MYSTERY_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	説明
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_wk_adrs				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Infomation( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INFO_SELECT_MSG,
    SEQ_INFO_SELECT_INIT,
    SEQ_INFO_SELECT_WAIT,

    SEQ_MSG_WAIT,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INFO_SELECT_MSG:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_022, MYSTERY_TEXT_TYPE_QUE );
    UTIL_CreateMenu( p_wk, UTIL_MENU_TYPE_INFO, HEAPID_MYSTERYGIFT ); 
    *p_seq  = SEQ_INFO_SELECT_WAIT;
    break;

  case SEQ_INFO_SELECT_WAIT:
    { 
      u32 ret;
      ret = UTIL_MainMenu( p_wk ); 
      if( ret != MYSTERY_MENU_SELECT_NULL )
      {
        UTIL_DeleteMenu( p_wk );
        if( ret == 0 )
        { 
          //ふしぎなおくりものについて
          MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_023, MYSTERY_TEXT_TYPE_STREAM );
          MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_INFO_SELECT_MSG );
          *p_seq  = SEQ_MSG_WAIT;
        }
        else if( ret == 1 )
        { 
          //カードアルバムについて
          MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_024, MYSTERY_TEXT_TYPE_STREAM );
          MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_INFO_SELECT_MSG );
          *p_seq  = SEQ_MSG_WAIT;
        }
        else if( ret == 2 )
        { 
          //おくりものがいっぱいのとき
          MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_025, MYSTERY_TEXT_TYPE_STREAM );
          MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_INFO_SELECT_MSG );
          *p_seq  = SEQ_MSG_WAIT;
        }
        else if( ret == 3 )
        {
          //やめる
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
        }
      }
    }
    break;

  case SEQ_MSG_WAIT:
    if( MYSTERY_TEXT_IsEndPrint( p_wk->p_text ) )
    { 
      MYSTERY_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	受信
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_wk_adrs				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_RecvGift( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_SEARCH,
    SEQ_NO_GIFT_INIT,
    SEQ_NO_GIFT_INIT2,
    SEQ_NO_GIFT_WAIT,
    SEQ_SELECT_GIFT_MSG,
    SEQ_SELECT_GIFT_WAIT,
    SEQ_GIFT_YESNO_INIT,
    SEQ_GIFT_YESNO_WAIT,
    SEQ_CANCEL_GIFT_INIT,
    SEQ_CANCEL_GIFT_WAIT,

    SEQ_RE_RECV_MSG,
    SEQ_DIRTY_MSG,
    SEQ_DIRTY_END,

    SEQ_MSG_WAIT,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

#ifdef MYSTERY_AUTO_DEBUG
  if( *p_seq == SEQ_SEARCH && s_debug_cnt-- == 0 )
  {
    if( (OS_GetConsoleType() & (OS_CONSOLE_ISDEBUGGER|OS_CONSOLE_TWLDEBUGGER)) )
    {
      DEBUG_UI_SetUp( DEBUG_UI_AUTO_MYSTERY, DEBUG_UI_PLAY_ONE );
      s_debug_flag  = 0;
    }
  }
#endif //MYSTERY_AUTO_DEBUG

  switch( *p_seq )
  { 
  case SEQ_INIT:
    switch( p_wk->mode )
    { 
    case MYSTERY_NET_MODE_WIFI:
      MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_WIFI_DOWNLOAD );
      break;
    case MYSTERY_NET_MODE_WIRELESS:
      MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_START_BEACON_DOWNLOAD );
      break;
    case MYSTERY_NET_MODE_IRC:
      MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_START_IRC_DOWNLOAD );
      break;
    }
#ifdef MYSTERY_AUTO_DEBUG
    s_debug_cnt = GFUser_GetPublicRand0( 16 );
#endif //MYSTERY_AUTO_DEBUG

    //さがしています
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_010, MYSTERY_TEXT_TYPE_QUE );
    p_wk->cnt = 0;
    *p_seq  = SEQ_SEARCH;
    break;

  case SEQ_SEARCH:
    { 
      if( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_WAIT )
      {
        MYSTERY_NET_RECV_STATUS ret;
        ret = MYSTERY_NET_GetDownloadData( p_wk->p_net, &p_wk->data, sizeof(DOWNLOAD_GIFT_DATA) );
        if( ret == MYSTERY_NET_RECV_STATUS_SUCCESS )
        { 
          u32 dirty;

          dirty = MYSTERYDATA_ModifyDownloadData( &p_wk->data, p_wk->p_gamedata, GFL_HEAP_LOWID(HEAPID_MYSTERYGIFT) );

#ifdef PM_DEBUG
          { 
          static char title_buff[GIFT_DATA_CARD_TITLE_MAX];
          static char text_buff[GIFT_DATA_CARD_TEXT_MAX];
          DEB_STR_CONV_StrcodeToSjis( p_wk->data.data.event_name, title_buff, GIFT_DATA_CARD_TITLE_MAX );
          DEB_STR_CONV_StrcodeToSjis( p_wk->data.event_text, text_buff, GIFT_DATA_CARD_TEXT_MAX );

          MYSTERY_Printf( "取得しました\n" );
          MYSTERY_Printf( "TITLE: %s\n", title_buff );
          MYSTERY_Printf( "TEXT : %s\n", text_buff );
          MYSTERY_Printf( "EV_ID: %d\n", p_wk->data.data.event_id );
          MYSTERY_Printf( "TYPE : %d\n",   p_wk->data.data.gift_type );
          MYSTERY_Printf( "ONLYONE : %d\n",   p_wk->data.data.only_one_flag );
          MYSTERY_Printf( "不正 : %d\n", dirty );
          }
#endif //PM_DEBUG

          //CRCがあっていて、不正が０かつ、取得できるバージョンならばOK
          //バージョンが０ならば制限なし、それ以外は自分のバージョンマスクで見る
          if( Mystery_IsValid( &p_wk->data, dirty ) )
          { 
            *p_seq  = SEQ_SELECT_GIFT_MSG;
          }
          else
          { 
            *p_seq  = SEQ_DIRTY_MSG;
          }
        }
        else if( ret == MYSTERY_NET_RECV_STATUS_FAILED )
        { 
          MYSTERY_Printf( "取得できなかった\n" );
          *p_seq = SEQ_NO_GIFT_INIT;
        }
      }
    }

    //エラーチェック
    switch( MYSTERY_NET_GetErrorRepairType( p_wk->p_net ) )
    { 
    case MYSTERY_NET_ERROR_REPAIR_RETURN:      //１つ前の選択肢まで戻る
    case MYSTERY_NET_ERROR_REPAIR_DISCONNECT:  //切断する
      MYSTERY_NET_ClearError( p_wk->p_net );

      MYSTERY_Printf( "取得できなかった\n" );
      MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
      break;
    }

    //Bボタンキャンセルかタイムアウト
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL || p_wk->cnt++ > MYSTERY_RECV_TIMEOUT )
    { 
      BOOL is_cancel  = FALSE;
      switch( p_wk->mode )
      { 
      case MYSTERY_NET_MODE_WIFI:
        if( MYSTERY_NET_GetState( p_wk->p_net ) == MYSTERY_NET_STATE_WIFI_DOWNLOAD )
        { 
          MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_CANCEL_WIFI_DOWNLOAD );
          is_cancel  = TRUE;
        }
        break;
      case MYSTERY_NET_MODE_WIRELESS:
        if( MYSTERY_NET_GetState( p_wk->p_net ) == MYSTERY_NET_STATE_MAIN_BEACON_DOWNLOAD )
        { 
          MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_END_BEACON_DOWNLOAD );
          is_cancel  = TRUE;
        }
        break;
      case MYSTERY_NET_MODE_IRC:
        if( MYSTERY_NET_GetState( p_wk->p_net ) == MYSTERY_NET_STATE_MAIN_IRC_DOWNLOAD )
        { 
          MYSTERY_NET_ChangeStateReq( p_wk->p_net, MYSTERY_NET_STATE_END_IRC_DOWNLOAD );
          is_cancel  = TRUE;
        }
        break;
      }

      if( is_cancel )
      { 
        //Bキャンセルorタイムアウト
        *p_seq = SEQ_NO_GIFT_INIT;
        p_wk->cnt   = 0;
      }
    }
    break;

  case SEQ_NO_GIFT_INIT:

    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_026, MYSTERY_TEXT_TYPE_WAIT );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_NO_GIFT_INIT2 );
    *p_seq  = SEQ_MSG_WAIT;
    break;

  case SEQ_NO_GIFT_INIT2:
    p_wk->cnt++;
    if(( MYSTERY_NET_GetState( p_wk->p_net)  == MYSTERY_NET_STATE_WAIT )
        && ( p_wk->cnt > 30 ))
    {
      p_wk->cnt = 0;
      MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_err_003, MYSTERY_TEXT_TYPE_STREAM );
      MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_NO_GIFT_WAIT );
      *p_seq  = SEQ_MSG_WAIT;
    }
    break;

  case SEQ_NO_GIFT_WAIT:
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    break;

  case SEQ_SELECT_GIFT_MSG:
    UTIL_CreateMenu( p_wk, UTIL_MENU_TYPE_GIFT, HEAPID_MYSTERYGIFT );
    UTIL_CreateGuideText( p_wk, HEAPID_MYSTERYGIFT );
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_011, MYSTERY_TEXT_TYPE_QUE );
    *p_seq  = SEQ_SELECT_GIFT_WAIT;
    break;

  case SEQ_SELECT_GIFT_WAIT:
    //うけとるおくりものをえらんでください
    { 
      u32 ret;
      BOOL is_cancel  = FALSE;
      ret = UTIL_MainMenu( p_wk ); 
      if( ret != MYSTERY_MENU_SELECT_NULL )
      { 
        if( ret == 0 )
        { 

          MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_013, MYSTERY_TEXT_TYPE_STREAM );
          MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_GIFT_YESNO_INIT );
          *p_seq  = SEQ_MSG_WAIT;
        }
        else if ( ret == MYSTERY_MENU_SELECT_CENCEL )
        { 
          is_cancel = TRUE;
        }
      }
      else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL )
      { 
        is_cancel = TRUE;
      }

      if( is_cancel )
      { 
        MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_012, MYSTERY_TEXT_TYPE_STREAM );
        MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_CANCEL_GIFT_INIT );
        *p_seq  = SEQ_MSG_WAIT;
      }
    }
    break;

  case SEQ_GIFT_YESNO_INIT:
    UTIL_CreateList( p_wk, UTIL_LIST_TYPE_YESNO, HEAPID_MYSTERYGIFT );
    *p_seq  = SEQ_GIFT_YESNO_WAIT;
    break;

  case SEQ_GIFT_YESNO_WAIT:
    //おくりものをじゅしんしますか
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        UTIL_DeleteList( p_wk );
        if( ret == 0 )
        { 
          //はい
          UTIL_DeleteMenu(p_wk);

          //すでに受け取っていたら、受け取れない
          MYSTERY_Printf( "受信可能？ ONLY_ONE%d RECV%d\n", p_wk->data.data.only_one_flag, MYSTERYDATA_IsEventRecvFlag( p_wk->p_sv, p_wk->data.data.event_id ) );
          if( p_wk->data.data.only_one_flag && MYSTERYDATA_IsEventRecvFlag( p_wk->p_sv, p_wk->data.data.event_id ) )
          { 
            MYSTERY_Printf( "すでに受け取っていました\n" );
            *p_seq = SEQ_RE_RECV_MSG;
          }
          else
          { 
            //受信時間を設定
            { 
              RTCDate date;
              GFL_RTC_GetDate( &date );
              p_wk->data.data.recv_date = MYSTERYDATA_PackDate( &date );
            }

            //受信データをセーブデータに設定
            MYSTERYDATA_SetCardData( p_wk->p_sv, &p_wk->data.data);
            MYSTERYDATA_SetEventRecvFlag(p_wk->p_sv, p_wk->data.data.event_id);

            //デモへ
            MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_Demo );
          }
        }
        else if( ret == 1 )
        { 
          //いいえ
          *p_seq  = SEQ_SELECT_GIFT_MSG;
        }
      }
    }
    break;

  case SEQ_CANCEL_GIFT_INIT:
    UTIL_CreateList( p_wk, UTIL_LIST_TYPE_YESNO, HEAPID_MYSTERYGIFT );
    *p_seq  = SEQ_CANCEL_GIFT_WAIT;
    break;

  case SEQ_CANCEL_GIFT_WAIT:
    //おくりもののうけとりをやめますか
    { 
      u32 ret;
      ret = MYSTERY_LIST_Main( p_wk->p_list );
      if( ret != MYSTERY_LIST_SELECT_NULL )
      { 
        UTIL_DeleteList( p_wk );
        if( ret == 0 )
        { 
          //はい
          UTIL_DeleteMenu(p_wk);
          UTIL_DeleteGuideText( p_wk );
          MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
        }
        else if( ret == 1 )
        { 
          //いいえ
          *p_seq  = SEQ_SELECT_GIFT_MSG;
        }
      }
    }
    break;
  
    //-------------------------------------
    /// 一度受信している
    //=====================================
  case SEQ_RE_RECV_MSG:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_err_002, MYSTERY_TEXT_TYPE_STREAM );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    *p_seq  = SEQ_MSG_WAIT;
    break;

    //-------------------------------------
    ///	不正な場合
    //=====================================
  case SEQ_DIRTY_MSG:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_err_006, MYSTERY_TEXT_TYPE_STREAM );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_DIRTY_END );
    *p_seq  = SEQ_MSG_WAIT;
    break;

  case SEQ_DIRTY_END:
    UTIL_DeleteMenu(p_wk);
    UTIL_DeleteGuideText( p_wk );

    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    break;

  case SEQ_MSG_WAIT:
    if( MYSTERY_TEXT_IsEndPrint( p_wk->p_text ) )
    { 
      MYSTERY_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	デモ
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_wk_adrs				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Demo( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_INIT_ONE_WAIT,
    SEQ_INIT_WAIT,
    SEQ_DEMO_MAIN,
    SEQ_SAVE_INIT,
    SEQ_SAVE_MAIN,
    SEQ_BACK_FADE_INIT,
    SEQ_BACK_FADE_WAIT,
    SEQ_RECV,
    SEQ_MSG_WAIT,
    SEQ_EXIT_3D,
    SEQ_EXITWAIT_3D,
    SEQ_CARD_INIT,
    SEQ_START_CARD_EFFECT,
    SEQ_WAIT_CARD_EFFECT,
    SEQ_WAIT_SYNC,
    SEQ_INIT_BTN,
    SEQ_CARD_WAIT,
    SEQ_END,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    //3D用に設定
    MYSTERY_GRAPHIC_3D_Setup( p_wk->p_graphic );
    GFL_BG_SetVisible( GFL_BG_FRAME1_M, TRUE ); //なぜかセットアップするとB１と３GがOFFになるので
    GFL_BG_SetVisible( GFL_BG_FRAME3_M, TRUE );
    //おくりものを受信中です
    { 
      GFL_CLUNIT	*p_unit	= MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );

#ifdef MYSTERY_MOVIE_DEMO
      if (GFL_UI_KEY_GetCont() & PAD_BUTTON_R)
      { 
        p_wk->data.movie_flag = TRUE;
      }
#endif //MYSTERY_MOVIE_DEMO


      MYSTERY_DEMO_Init( &p_wk->demo, p_unit, &p_wk->data, p_wk->p_gamedata, &p_wk->obj, &p_wk->bg, &p_wk->effect, HEAPID_MYSTERYGIFT );
      MYSTERY_DEMO_Start( &p_wk->demo, MYSTERY_DEMO_TYPE_RECV );

      MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_014, MYSTERY_TEXT_TYPE_WAIT );
      *p_seq  = SEQ_INIT_ONE_WAIT;
    }
    break;

  case SEQ_INIT_ONE_WAIT:
    GFL_BG_SetVisible( GFL_BG_FRAME0_M, TRUE ); //パシり回避のため
    *p_seq  = SEQ_INIT_WAIT;
    break;

  case SEQ_INIT_WAIT:
    if( MYSTERY_TEXT_IsEndPrint(p_wk->p_text) )
    {
      *p_seq  = SEQ_DEMO_MAIN;
    }
    break;

  case SEQ_DEMO_MAIN:
    MYSTERY_DEMO_Main( &p_wk->demo );
    if( MYSTERY_DEMO_IsEnd(&p_wk->demo) )
    {
      *p_seq  = SEQ_SAVE_INIT;
    }
    break;

  case SEQ_SAVE_INIT:
    MYSTERYDATA_SaveAsyncStart( p_wk->p_sv,p_wk->p_gamedata );
    *p_seq  = SEQ_SAVE_MAIN;
    break;

  case SEQ_SAVE_MAIN:
    { 
      SAVE_RESULT result  = MYSTERYDATA_SaveAsyncMain(p_wk->p_sv,p_wk->p_gamedata);
      if( result == SAVE_RESULT_OK )
      { 
        *p_seq  = SEQ_BACK_FADE_INIT;
      }
    }
    break;

  case SEQ_BACK_FADE_INIT:
    MYSTERY_DEMO_Start( &p_wk->demo, MYSTERY_DEMO_TYPE_FINISH );
    *p_seq  = SEQ_BACK_FADE_WAIT;
    break;

  case SEQ_BACK_FADE_WAIT:
    MYSTERY_DEMO_Main( &p_wk->demo );
    if( MYSTERY_DEMO_IsEnd(&p_wk->demo) )
    {
      *p_seq  = SEQ_RECV;
    }
    break;

  case SEQ_RECV:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_msg, syachi_mystery_01_015, MYSTERY_TEXT_TYPE_STREAM );
    *p_seq  = SEQ_MSG_WAIT;
    break;

  case SEQ_MSG_WAIT:
    if( MYSTERY_TEXT_IsEndPrint(p_wk->p_text) )
    { 
      *p_seq  = SEQ_EXIT_3D;
    }
    break;

  case SEQ_EXIT_3D:
    MYSTERY_DEMO_Exit( &p_wk->demo );
    MYSTERY_TEXT_Exit( p_wk->p_text );
    p_wk->p_text  = NULL;
    UTIL_DeleteMenu( p_wk );
    G2_BlendNone();
    MYSTERY_GRAPHIC_3D_CleanUp( p_wk->p_graphic );
    GFL_BG_SetVisible( GFL_BG_FRAME1_M, TRUE ); //なぜかセットアップするとBGがOFFになるので
    GFL_BG_SetVisible( GFL_BG_FRAME3_M, TRUE );
    *p_seq  = SEQ_EXITWAIT_3D;
    break;

  case SEQ_EXITWAIT_3D:

    GFL_BG_SetVisible( GFL_BG_FRAME0_M, TRUE ); //パシリ回避
    *p_seq  = SEQ_CARD_INIT;
    break;

  case SEQ_CARD_INIT:
    { 
      GFL_BG_SetVisible( BG_FRAME_M_TEXT, FALSE );
      { 
        MYSTERY_CARD_SETUP setup;
        GFL_STD_MemClear( &setup, sizeof(MYSTERY_CARD_SETUP) );

        setup.back_frm = BG_FRAME_M_BACK1;
        setup.font_frm = BG_FRAME_M_LIST;
        setup.back_plt_num  = PLT_BG_CARD_M;
        setup.font_plt_num  = PLT_BG_FONT_M;
        setup.icon_obj_plt_num  = PLT_OBJ_CARD_ICON_M; 
        setup.silhouette_obj_plt_num  = PLT_OBJ_CARD_SILHOUETTE_M; 
        setup.p_clunit  = MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
        setup.p_sv      = p_wk->p_sv;
        setup.p_msg     = p_wk->p_msg; 
        setup.p_font    = p_wk->p_font; 
        setup.p_que     = p_wk->p_que; 
        setup.p_word    = p_wk->p_word;

        p_wk->p_card_res  = MYSTERY_CARD_RES_Init( &setup, HEAPID_MYSTERYGIFT );
      }

      { 
        p_wk->p_card  = MYSTERY_CARD_Init( &p_wk->data.data, p_wk->p_card_res, p_wk->p_gamedata, HEAPID_MYSTERYGIFT );
        MYSTERY_CARD_Trans( p_wk->p_card, MYSTERY_CARD_TRANS_TYPE_NORMAL );
      }
    }
    *p_seq  = SEQ_START_CARD_EFFECT;
    break;

  case SEQ_START_CARD_EFFECT:
    MYSTERY_CARD_Main( p_wk->p_card );
    MYSTERY_CARD_StartEffect( p_wk->p_card );
    *p_seq = SEQ_WAIT_CARD_EFFECT;
    break;

  case SEQ_WAIT_CARD_EFFECT:
    MYSTERY_CARD_Main( p_wk->p_card );
    if( MYSTERY_CARD_IsEndEffect(p_wk->p_card) )
    { 
      p_wk->cnt = 0;
      *p_seq  = SEQ_WAIT_SYNC;
    }
    break;
    
  case SEQ_WAIT_SYNC:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
    { 
      *p_seq  = SEQ_END;
    }

    if( p_wk->cnt++ > MYSTERY_RECV_CARD_END_BTN )
    { 
      p_wk->cnt  = 0;
      *p_seq  = SEQ_INIT_BTN;
    }
    break;

  case SEQ_INIT_BTN:
    {	
      GFL_CLUNIT	*p_clunit	= MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
      MYSTERY_BTN_Init( &p_wk->btn, &p_wk->obj, p_clunit, p_wk->p_que, p_wk->p_msg, syachi_mystery_album_009, p_wk->p_font, HEAPID_MYSTERYGIFT );
      MYSTERY_BTN_PrintMain( &p_wk->btn );
      *p_seq  = SEQ_CARD_WAIT;
    }
    break;

  case SEQ_CARD_WAIT:
    MYSTERY_BTN_PrintMain( &p_wk->btn );
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
    { 
      MYSTERY_BTN_Exit( &p_wk->btn );
      *p_seq  = SEQ_END;
    }
    break;

  case SEQ_END:
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_FadeOut );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	カードアルバム
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_wk_adrs				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_CardAlbum( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_FADEOUT_INIT,
    SEQ_INIT,
    SEQ_START_FADEIN_INIT,
    SEQ_MAIN,
    SEQ_START_FADEOUT_EXIT,
    SEQ_EXIT,
    SEQ_START_FADEIN_EXIT,
    SEQ_NEXT_STARTSELECT,

    SEQ_WAIT_FADE,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_FADEOUT_INIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, -1 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_INIT );
    *p_seq  = SEQ_WAIT_FADE;
    break;
  case SEQ_INIT:
    MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, FALSE );
    { 
      UTIL_DeleteMenu( p_wk );
      MYSTERY_TEXT_Exit( p_wk->p_text );
      p_wk->p_text  = NULL;
    }
    {
      MYSTERY_ALBUM_SETUP setup;
      GFL_STD_MemClear( &setup, sizeof(MYSTERY_LIST_SETUP) );
      setup.mode  = MYSTERY_ALBUM_MODE_VIEW;
      setup.p_clunit  = MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
      setup.p_sv      = p_wk->p_sv;
      setup.p_font    = p_wk->p_font;
      setup.p_que     = p_wk->p_que;
      setup.p_word    = p_wk->p_word;
      setup.p_msg     = p_wk->p_msg;
      setup.p_gamedata  = p_wk->p_gamedata;
      p_wk->p_album = MYSTERY_ALBUM_Init( &setup, HEAPID_MYSTERYGIFT );
      *p_seq  = SEQ_START_FADEIN_INIT;
    }
    break;
  case SEQ_START_FADEIN_INIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, -1 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_MAIN );
    *p_seq  = SEQ_WAIT_FADE;
    break;
  case SEQ_MAIN:
    MYSTERY_ALBUM_Main( p_wk->p_album );
    if( MYSTERY_ALBUM_IsEnd( p_wk->p_album ) )
    { 
      *p_seq  = SEQ_START_FADEOUT_EXIT;
    }
    break;
  case SEQ_START_FADEOUT_EXIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_EXIT );
    *p_seq  = SEQ_WAIT_FADE;
    break;
  case SEQ_EXIT:
    MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, TRUE );
    MYSTERY_ALBUM_Exit( p_wk->p_album );
    p_wk->p_album = NULL;
    BG_Load( &p_wk->bg, BG_LOAD_TYPE_INIT );
    OBJ_ReLoad( &p_wk->obj, HEAPID_MYSTERYGIFT );
    p_wk->p_text  = MYSTERY_TEXT_Init( BG_FRAME_M_TEXT, PLT_BG_FONT_M, p_wk->p_que, p_wk->p_font, HEAPID_MYSTERYGIFT );
    MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, BG_CGX_OFS_M_TEXT, PLT_BG_TEXT_M );

    UTIL_CreateMenu( p_wk, UTIL_MENU_TYPE_TOP, HEAPID_MYSTERYGIFT ); 
    { 
      G2_SetBlendAlpha(
          GX_BLEND_PLANEMASK_BG2,
          GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
          MYSTERY_MENU_ALPHA_EV1,
          MYSTERY_MENU_ALPHA_EV2
          );
    }


    *p_seq  = SEQ_START_FADEIN_EXIT;
    break;
  case SEQ_START_FADEIN_EXIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_NEXT_STARTSELECT );
    *p_seq  = SEQ_WAIT_FADE;
    break;

  case SEQ_NEXT_STARTSELECT:
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    break;

  case SEQ_WAIT_FADE:
		if( !GFL_FADE_CheckFade() )
		{	
      MYSTERY_SEQ_NextReservSeq( p_seqwk );
		}
    break;
  }

  if( p_wk->p_album )
  { 
    MYSTERY_ALBUM_PrintMain( p_wk->p_album );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	カードアルバム　カード削除
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_wk_adrs				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_DeleteCard( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_FADEOUT_INIT,
    SEQ_INIT,
    SEQ_START_FADEIN_INIT,
    SEQ_MAIN,
    SEQ_START_FADEOUT_EXIT,
    SEQ_EXIT,
    SEQ_START_FADEIN_EXIT,
    SEQ_NEXT_STARTSELECT,

    SEQ_WAIT_FADE,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_FADEOUT_INIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, -1 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_INIT );
    *p_seq  = SEQ_WAIT_FADE;
    break;
  case SEQ_INIT:
    MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, FALSE );
    { 
      UTIL_DeleteMenu( p_wk );
      MYSTERY_TEXT_Exit( p_wk->p_text );
      p_wk->p_text  = NULL;
    }
    {
      MYSTERY_ALBUM_SETUP setup;
      GFL_STD_MemClear( &setup, sizeof(MYSTERY_LIST_SETUP) );
      setup.mode  = MYSTERY_ALBUM_MODE_DELETE;
      setup.p_clunit  = MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
      setup.p_sv      = p_wk->p_sv;
      setup.p_font    = p_wk->p_font;
      setup.p_que     = p_wk->p_que;
      setup.p_word    = p_wk->p_word;
      setup.p_msg     = p_wk->p_msg;
      setup.p_gamedata  = p_wk->p_gamedata;
      p_wk->p_album = MYSTERY_ALBUM_Init( &setup, HEAPID_MYSTERYGIFT );
      *p_seq  = SEQ_START_FADEIN_INIT;
    }
    break;
  case SEQ_START_FADEIN_INIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, -1 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_MAIN );
    *p_seq  = SEQ_WAIT_FADE;
    break;
  case SEQ_MAIN:
    MYSTERY_ALBUM_Main( p_wk->p_album );
    if( MYSTERY_ALBUM_IsEnd( p_wk->p_album ) )
    { 
      *p_seq  = SEQ_START_FADEOUT_EXIT;
    }
    break;
  case SEQ_START_FADEOUT_EXIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_EXIT );
    *p_seq  = SEQ_WAIT_FADE;
    break;
  case SEQ_EXIT:
    MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, TRUE );
    MYSTERY_ALBUM_Exit( p_wk->p_album );
    p_wk->p_album = NULL;
    BG_Load( &p_wk->bg, BG_LOAD_TYPE_INIT );
    OBJ_ReLoad( &p_wk->obj, HEAPID_MYSTERYGIFT );
    p_wk->p_text  = MYSTERY_TEXT_Init( BG_FRAME_M_TEXT, PLT_BG_FONT_M, p_wk->p_que, p_wk->p_font, HEAPID_MYSTERYGIFT );
    MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, BG_CGX_OFS_M_TEXT, PLT_BG_TEXT_M );

    { 
      G2_SetBlendAlpha(
          GX_BLEND_PLANEMASK_BG2,
          GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
          MYSTERY_MENU_ALPHA_EV1,
          MYSTERY_MENU_ALPHA_EV2
          );
    }

    UTIL_CreateMenu( p_wk, UTIL_MENU_TYPE_TOP, HEAPID_MYSTERYGIFT ); 
    *p_seq  = SEQ_START_FADEIN_EXIT;
    break;
  case SEQ_START_FADEIN_EXIT:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
    MYSTERY_SEQ_SetReservSeq( p_seqwk, SEQ_NEXT_STARTSELECT );
    *p_seq  = SEQ_WAIT_FADE;
    break;

  case SEQ_NEXT_STARTSELECT:
    p_wk->is_delete = MYSTERYDATA_CheckCardDataSpace( p_wk->p_sv );
    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    break;

  case SEQ_WAIT_FADE:
		if( !GFL_FADE_CheckFade() )
		{	
      MYSTERY_SEQ_NextReservSeq( p_seqwk );
		}
    break;
  }

  if( p_wk->p_album )
  { 
    MYSTERY_ALBUM_PrintMain( p_wk->p_album );
  }
}

//----------------------------------------------------------------------------
/** 
 *	@brief  WiFiLogin画面の接続
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_wk_adrs				ワーク
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_WifiLogin( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
    SEQ_DELETE,
    SEQ_PROC_CALL,
    SEQ_PROC_WAIT,
    SEQ_CREATE,
    SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
    SEQ_END,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  //エラーチェック
  switch( MYSTERY_NET_GetErrorRepairType( p_wk->p_net ) )
  { 
  case MYSTERY_NET_ERROR_REPAIR_RETURN:      //１つ前の選択肢まで戻る
  case MYSTERY_NET_ERROR_REPAIR_DISCONNECT:  //切断する
    MYSTERY_NET_ClearError( p_wk->p_net );

    MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    break;
  }

  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
      MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, FALSE );
			*p_seq	= SEQ_DELETE;
		}
		break;

  case SEQ_DELETE:
    if( p_wk->p_text )
    { 
      MYSTERY_TEXT_Exit( p_wk->p_text );
      p_wk->p_text  = NULL;
    }

    //リソース破棄
    OBJ_Exit( &p_wk->obj );
    BG_Exit( &p_wk->bg );

    //グラフィック破棄
    MYSTERY_GRAPHIC_Exit( p_wk->p_graphic );
    p_wk->p_graphic = NULL;

    *p_seq  = SEQ_PROC_CALL;
    break;

  case SEQ_PROC_CALL:
    p_wk->p_wifilogin_param = GFL_HEAP_AllocMemory( HEAPID_MYSTERYGIFT, sizeof(WIFILOGIN_PARAM) );
    GFL_STD_MemClear( p_wk->p_wifilogin_param, sizeof(WIFILOGIN_PARAM) );
    p_wk->p_wifilogin_param->gamedata     = p_wk->p_gamedata;
    p_wk->p_wifilogin_param->bg           = WIFILOGIN_BG_NORMAL;
    p_wk->p_wifilogin_param->display      = WIFILOGIN_DISPLAY_UP;
    p_wk->p_wifilogin_param->pSvl         = NULL;
    p_wk->p_wifilogin_param->bgm          = WIFILOGIN_BGM_NORMAL;
    p_wk->p_wifilogin_param->nsid         = WB_NET_MYSTERY;
    if( MYSTERY_DATA_TYPE_OUTSIDE == MYSTERY_DATA_GetDataType( p_wk->p_sv ) )
    { 
      //管理外セーブならば、WIFILOGINはセーブへ行かない
      p_wk->p_wifilogin_param->mode       = WIFILOGIN_MODE_NOTSAVE;
    }
    else
    { 
      p_wk->p_wifilogin_param->mode       = WIFILOGIN_MODE_NORMAL;
    }

    GFL_PROC_SysCallProc( FS_OVERLAY_ID(wifi_login), &WiFiLogin_ProcData, p_wk->p_wifilogin_param );
    *p_seq  = SEQ_CREATE;
    break;

  case SEQ_CREATE:
    //グラフィック設定
    p_wk->p_graphic	= MYSTERY_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_MYSTERYGIFT );

    //リソース読み込み
    BG_Init( &p_wk->bg, HEAPID_MYSTERYGIFT );
    {	
      GFL_CLUNIT	*p_clunit	= MYSTERY_GRAPHIC_GetClunit( p_wk->p_graphic );
      OBJ_Init( &p_wk->obj, p_clunit, HEAPID_MYSTERYGIFT );
    }

    p_wk->p_text  = MYSTERY_TEXT_Init( BG_FRAME_M_TEXT, PLT_BG_FONT_M, p_wk->p_que, p_wk->p_font, HEAPID_MYSTERYGIFT );
    MYSTERY_TEXT_WriteWindowFrame( p_wk->p_text, BG_CGX_OFS_M_TEXT, PLT_BG_TEXT_M );

    { 
      G2_SetBlendAlpha(
          GX_BLEND_PLANEMASK_BG2,
          GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
          MYSTERY_MENU_ALPHA_EV1,
          MYSTERY_MENU_ALPHA_EV2
          );
    }

    GFL_NET_ReloadIconTopOrBottom( TRUE, HEAPID_MYSTERYGIFT );

    PMSND_PlayBGM( SEQ_BGM_WIFI_PRESENT );
    MYSTERY_EFFECT_SetUpdateFlag( &p_wk->effect, TRUE );
    *p_seq  = SEQ_PROC_WAIT;
    break;

  case SEQ_PROC_WAIT:
    *p_seq  = SEQ_FADEOUT_START;
    break;

  case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_END;
		}
		break;

  case SEQ_END:
    if( p_wk->p_wifilogin_param->result == WIFILOGIN_RESULT_LOGIN )
    { 
      //次へ
      MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_RecvGift );
    }
    else
    { 
      //キャンセル
      MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    }
    GFL_HEAP_FreeMemory( p_wk->p_wifilogin_param );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	終了処理
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_wk_adrs				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  UTIL_DeleteList( p_wk );
  UTIL_DeleteMenu( p_wk );
  UTIL_DeleteGuideText( p_wk );

  if( p_wk->p_card )
  { 
    MYSTERY_CARD_Exit( p_wk->p_card );
    MYSTERY_CARD_RES_Exit( p_wk->p_card_res );
  }

  //終了
  MYSTERY_SEQ_End( p_seqwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	DSの無線通信設定で通信負荷にしていた場合
 *
 *	@param	MYSTERY_SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_wk_adrs				ワーク
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_EnableWireless( MYSTERY_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG,
    SEQ_WAIT_MSG,
  };

  MYSTERY_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_MSG:
    MYSTERY_TEXT_Print( p_wk->p_text, p_wk->p_err_msg, msg_common_wireless_off_keywait, MYSTERY_TEXT_TYPE_STREAM );

    *p_seq  = SEQ_WAIT_MSG;
    break;
  
  case SEQ_WAIT_MSG:
    if( MYSTERY_TEXT_IsEndPrint(p_wk->p_text) )
    {
      MYSTERY_SEQ_SetNext( p_seqwk, SEQFUNC_StartSelect );
    }
    break;
  }
}

//=============================================================================
/**
 *      UTIL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  リスト作成
 *
 *	@param	MYSTERY_WORK *p_wk    ワーク
 *	@param	heapID                ヒープID
 */
//-----------------------------------------------------------------------------
static void UTIL_CreateList( MYSTERY_WORK *p_wk, UTIL_LIST_TYPE type, HEAPID heapID )
{ 
  if( p_wk->p_list == NULL )
  { 
    MYSTERY_LIST_SETUP setup;
    GFL_STD_MemClear( &setup, sizeof(MYSTERY_LIST_SETUP) );
    setup.p_msg   = p_wk->p_msg;
    setup.p_font  = p_wk->p_font;
    setup.p_que   = p_wk->p_que;
    setup.strID[0]= syachi_mystery_01_002_yes;
    setup.strID[1]= syachi_mystery_01_002_no;
    setup.list_max= 2;
    setup.frm     = BG_FRAME_M_LIST;
    setup.font_plt= PLT_BG_FONT_M;
    setup.frm_plt = PLT_BG_TEXT_M;
    setup.frm_chr = BG_CGX_OFS_M_LIST;
    p_wk->p_list  = MYSTERY_LIST_Init( &setup, heapID ); 
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  リスト破棄
 *
 *	@param	MYSTERY_WORK *p_wk    ワーク
 */
//-----------------------------------------------------------------------------
static void UTIL_DeleteList( MYSTERY_WORK *p_wk )
{ 
  if( p_wk->p_list )
  { 
    MYSTERY_LIST_Exit( p_wk->p_list );
    p_wk->p_list  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  メニュー作成
 *
 *	@param	MYSTERY_WORK *p_wk    ワーク
 *	@param	heapID                ヒープID
 */
//-----------------------------------------------------------------------------
static void UTIL_CreateMenu( MYSTERY_WORK *p_wk, UTIL_MENU_TYPE type, HEAPID heapID )
{ 
 //メニュー作成
  if( p_wk->p_menu == NULL )
  { 
    MYSTERY_MENU_SETUP setup;
    GFL_STD_MemClear( &setup, sizeof(MYSTERY_MENU_SETUP) );
    setup.p_font  = p_wk->p_font;
    setup.p_que   = p_wk->p_que;
    setup.p_cursor= OBJ_GetClwk( &p_wk->obj, OBJ_CLWKID_CURSOR );
    setup.font_frm     = BG_FRAME_M_TEXT;
    setup.font_plt= PLT_BG_FONT_M;
    setup.bg_frm = BG_FRAME_M_BACK1;
    setup.bg_ofs = PLT_BG_RECV_M;
    setup.callback  = Util_MenuCallback;
    setup.p_wk_adrs = p_wk;
    setup.chr_y_ofs = 0;
    setup.chr_x_ofs_str = 0;
    setup.anm_seq = 0;
    setup.p_data  = &p_wk->menu_data[ type ];


    switch( type )
    { 
    case UTIL_MENU_TYPE_TOP:
      setup.p_msg   = p_wk->p_msg;
      setup.strID[0]= syachi_mystery_menu_001;
      setup.strID[1]= syachi_mystery_menu_002;
      setup.strID[2]= syachi_mystery_menu_015;
      setup.strID[3]= syachi_mystery_menu_003;
      setup.list_max= 4;
      break;

    case UTIL_MENU_TYPE_NETMODE:
      setup.p_msg   = p_wk->p_msg;
      setup.strID[0]= syachi_mystery_menu_004;
      setup.strID[1]= syachi_mystery_menu_005;
      setup.strID[2]= syachi_mystery_menu_006;
      setup.strID[3]= syachi_mystery_menu_007;
      setup.list_max= 4;
      break;
      
    case UTIL_MENU_TYPE_GIFT:
      setup.p_msg   = NULL;
      setup.p_strbuf[0] = GFL_STR_CreateBuffer( GIFT_DATA_CARD_TITLE_MAX+EOM_SIZE, heapID );
      GFL_STR_SetStringCodeOrderLength( setup.p_strbuf[0], p_wk->data.data.event_name, GIFT_DATA_CARD_TITLE_MAX+EOM_SIZE );
      setup.list_max  = 1;
      setup.chr_y_ofs = -1;
      setup.chr_x_ofs_str = -2;
      setup.anm_seq = 10;
      break;

    case UTIL_MENU_TYPE_INFO:
      setup.p_msg   = p_wk->p_msg;
      setup.strID[0]= syachi_mystery_menu_011;
      setup.strID[1]= syachi_mystery_menu_012;
      setup.strID[2]= syachi_mystery_menu_013;
      setup.strID[3]= syachi_mystery_menu_014;
      setup.list_max= 4;
      break;
    }

    //BG設定
    { 
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );

      GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_mystery_fusigi_bg_00_NCLR,
          PALTYPE_MAIN_BG, PLT_BG_BACK_M*0x20, 5*0x20, heapID );

      GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_mystery_fusigi_bg_00_NCGR, 
          BG_FRAME_M_BACK1, 0, 0, FALSE, heapID );

      if( type == UTIL_MENU_TYPE_GIFT )
      { 
        GFL_ARC_UTIL_TransVramScreen( ARCID_MYSTERY, NARC_mystery_fushigi_top02_NSCR,
            BG_FRAME_M_BACK1, 0, 0, FALSE, heapID );
      }
      else
      { 
        GFL_ARC_UTIL_TransVramScreen( ARCID_MYSTERY, NARC_mystery_fushigi_top_NSCR,
            BG_FRAME_M_BACK1, 0, 0, FALSE, heapID );
      }
      GFL_ARC_CloseDataHandle( p_handle );

      GFL_BG_ChangeScreenPalette( BG_FRAME_M_BACK1, 0, 0, 32, 24, PLT_BG_RECV_M );

      GFL_BG_LoadScreenReq( BG_FRAME_M_BACK1 );
      GFL_BG_SetVisible( BG_FRAME_M_BACK1, TRUE );
    }


    //メニュー作成
    p_wk->p_menu  = MYSTERY_MENU_Init( &setup, heapID );

    //メニューへ文字列バッファを渡していたら開放
    if( setup.p_msg == NULL )
    { 
      int i;
      for( i  = 0; i < MYSTERY_MENU_WINDOW_MAX; i++ )
      {
        if( setup.p_strbuf[i] )
        { 
          GFL_STR_DeleteBuffer( setup.p_strbuf[i] );
        }
      }
    }

    { 
      G2_SetBlendAlpha(
          GX_BLEND_PLANEMASK_BG2,
          GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
          MYSTERY_MENU_ALPHA_EV1,
          MYSTERY_MENU_ALPHA_EV2
          );
    }
    //フェード最初から
    OBJ_PltFade_Reset( &p_wk->obj );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  メニュー破棄
 *
 *	@param	MYSTERY_WORK *p_wk    ワーク
 *	@param	heapID                ヒープID
 */
//-----------------------------------------------------------------------------
static void UTIL_DeleteMenu( MYSTERY_WORK *p_wk )
{
  if(p_wk->p_menu)
  {

    GFL_BG_ClearScreen( BG_FRAME_M_BACK1 );
    GFL_BG_LoadScreenReq( BG_FRAME_M_BACK1 );
    GFL_BG_SetVisible( BG_FRAME_M_BACK1, FALSE );

    MYSTERY_MENU_Exit( p_wk->p_menu );
    p_wk->p_menu  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  メニューメイン処理
 *
 *	@param	MYSTERY_WORK *p_wk  ワーク
 *
 *	@return 選んだ値
 */
//-----------------------------------------------------------------------------
static u32 UTIL_MainMenu( MYSTERY_WORK *p_wk )
{ 
  u32 ret;
  ret = MYSTERY_MENU_Main( p_wk->p_menu ); 
  OBJ_PltFade_Main( &p_wk->obj );

  return ret;
}
//----------------------------------------------------------------------------
/**
 *	@brief  カーソルが動くときに呼ばれるコールバック
 *
 *	@param	MYSTERY_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Util_MenuCallback( void *p_wk_adrs )
{ 
  MYSTERY_WORK *p_wk  = p_wk_adrs;
  OBJ_PltFade_Reset( &p_wk->obj );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ガイド用テキスト文  作成
 *
 *	@param	MYSTERY_WORK *p_wk
 *	@param	heapID 
 */
//-----------------------------------------------------------------------------
static void UTIL_CreateGuideText( MYSTERY_WORK *p_wk, HEAPID heapID )
{ 
  if( p_wk->p_winset_s == NULL )
  { 
    MYSTERY_MSGWINSET_SETUP_TBL tbl[] =
    { 
      //タイトル
      { 
        1,2,30,2, 0, NULL, MYSTERY_MSGWIN_POS_WH_CENTER, 0, 0, MYSTERY_MSGWIN_WHITE_COLOR,
      },
      //本文
      {
        2,6,28,15, 0, NULL, MYSTERY_MSGWIN_POS_ABSOLUTE, 0, 4, MYSTERY_MSGWIN_WHITE_COLOR,
      },
    };
    tbl[0].p_strbuf = GFL_STR_CreateBuffer( GIFT_DATA_CARD_TITLE_MAX+1, heapID );
    GFL_STR_SetStringCodeOrderLength( tbl[0].p_strbuf, p_wk->data.data.event_name, GIFT_DATA_CARD_TITLE_MAX+EOM_SIZE ); 

    tbl[1].p_strbuf = GFL_STR_CreateBuffer( GIFT_DATA_CARD_TEXT_MAX+1, heapID );
    GFL_STR_SetStringCodeOrderLength( tbl[1].p_strbuf, p_wk->data.event_text, GIFT_DATA_CARD_TEXT_MAX+EOM_SIZE ); 

    p_wk->p_winset_s  = MYSTERY_MSGWINSET_Init( MYSTERY_MSGWIN_TRANS_MODE_AUTO, tbl, NELEMS(tbl), BG_FRAME_S_TEXT, PLT_BG_FONT_S, p_wk->p_que, p_wk->p_msg, p_wk->p_font, heapID );

    GFL_STR_DeleteBuffer( tbl[0].p_strbuf );
    GFL_STR_DeleteBuffer( tbl[1].p_strbuf );

    BG_Load( &p_wk->bg, BG_LOAD_TYPE_GUIDE_S ); 
    
 //   GFL_BG_ChangeScreenPalette( BG_FRAME_S_BACK1, 0, 0, 32, 23, PLT_BG_CARD_S );
    GFL_BG_LoadScreenReq( BG_FRAME_S_BACK1 );

    GFL_BG_SetVisible( BG_FRAME_S_BACK1, TRUE );

    //アルファ設定
    { 
      G2S_SetBlendAlpha(
          GX_BLEND_PLANEMASK_BG2,
          GX_BLEND_PLANEMASK_BG3 | GX_BLEND_PLANEMASK_OBJ,
          MYSTERY_MENU_ALPHA_EV1,
          MYSTERY_MENU_ALPHA_EV2
          );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ガイド用テキスト文  破棄
 *
 *	@param	MYSTERY_WORK *p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void UTIL_DeleteGuideText( MYSTERY_WORK *p_wk )
{ 
  if( p_wk->p_winset_s )
  { 
    G2S_BlendNone();

   MYSTERY_MSGWINSET_Clear( p_wk->p_winset_s );
   MYSTERY_MSGWINSET_Exit( p_wk->p_winset_s );
   p_wk->p_winset_s = NULL;

   GFL_BG_LoadScreenReq( BG_FRAME_S_TEXT );
   GFL_BG_SetVisible( BG_FRAME_S_BACK1, FALSE );
  }
}
//=============================================================================
/**
 *      デモ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  受信デモ  初期化
 *
 *	@param	MYSTERY_DEMO_WORK *p_wk ワーク
 *	@param	*p_unit                 CLUNIT
 *	@param  cp_data                 データ
 *	@param	heapID                  ヒープID
 */
//-----------------------------------------------------------------------------
static void MYSTERY_DEMO_Init( MYSTERY_DEMO_WORK *p_wk, GFL_CLUNIT *p_unit, const DOWNLOAD_GIFT_DATA *cp_data, GAMEDATA *p_gamedata, const OBJ_WORK *cp_obj, BG_WORK *p_bg, MYSTERY_EFFECT_WORK *p_effect, HEAPID heapID )
{
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_DEMO_WORK) );
  p_wk->p_effect  = p_effect;
  p_wk->p_bg      = p_bg;
  p_wk->heapID    = heapID;
  p_wk->cp_data   = cp_data;
  p_wk->leg_pos   = 0;
  p_wk->x_ofs     = 0;

  { 
    static const u16 sc_type_to_back_fade[] =
    { 
      0,  //なにもない
      1,  //ポケモン
      2,  //道具
      6,  //Gパワー
      3,   //映画
    };
    u16 plt_ofs;

    //なんのデータかによって、パレットフェードする色が違う読み込み
    GF_ASSERT( cp_data->data.gift_type < NELEMS(sc_type_to_back_fade) );

    if( cp_data->movie_flag )
    { 
      plt_ofs = sc_type_to_back_fade[ 4 ];
    }
    else
    { 
      plt_ofs = sc_type_to_back_fade[ cp_data->data.gift_type ];
    }

    //BGのパレットフェード用にパレットをメモリ読み込み
    { 

      void *p_buff;
      NNSG2dPaletteData *p_plt;
      const u16 *cp_plt_adrs;

      //もとのパレットから色情報を保存
      p_buff  = GFL_ARC_UTIL_LoadPalette( ARCID_MYSTERY, NARC_mystery_fushigi_back_NCLR, &p_plt, heapID );
      cp_plt_adrs = p_plt->pRawData;
      GFL_STD_MemCopy( cp_plt_adrs, p_wk->plt_dst, sizeof(u16) * 0x10 );
      GFL_STD_MemCopy( (u8*)cp_plt_adrs + plt_ofs * 0x20, p_wk->plt_src, sizeof(u16) * 0x10 );

      //パレット破棄
      GFL_HEAP_FreeMemory( p_buff );
    }

    //丸い玉のパレットを変更
    { 
      int i;
      GFL_CLWK      *p_clwk;
      for( i = 0; i < EFFECT_BRIGHT_MAX; i++ )
      { 
        p_clwk  = OBJ_GetClwk( cp_obj, OBJ_CLWKID_BRIGHT_TOP+i );
        GFL_CLACT_WK_SetPlttOffs( p_clwk, plt_ofs, CLWK_PLTTOFFS_MODE_OAM_COLOR );
      }
    }
  }

	//リソース読みこみ
  switch( cp_data->data.gift_type )
  { 
  case MYSTERYGIFT_TYPE_POKEMON:
    {	
      const GIFT_PRESENT_POKEMON  *cp_pokemon = &cp_data->data.data.pokemon;

      POKEMON_PARAM* p_pp = Mystery_CreatePokemon( &cp_data->data, GFL_HEAP_LOWID(heapID), p_gamedata );
      ARCHANDLE		*p_handle	= POKE2DGRA_OpenHandle( heapID );

      p_wk->res_plt	= POKE2DGRA_OBJ_PLTT_RegisterPPP( p_handle, PP_GetPPPPointerConst(p_pp), POKEGRA_DIR_FRONT, CLSYS_DRAW_MAIN, PLT_OBJ_GIFT_M * 0x20, heapID );
 
      p_wk->res_cel	= POKE2DGRA_OBJ_CELLANM_RegisterPPP( PP_GetPPPPointerConst(p_pp), POKEGRA_DIR_FRONT, APP_COMMON_MAPPING_128K, CLSYS_DRAW_MAIN, heapID );
        
      p_wk->res_cgx	= POKE2DGRA_OBJ_CGR_RegisterPPP( p_handle, PP_GetPPPPointerConst(p_pp), POKEGRA_DIR_FRONT, CLSYS_DRAW_MAIN, heapID );

      //足元座標を保存
      //(中心からのオフセット)
      p_wk->leg_pos = Mystery_GetLegPos( PP_GetPPPPointerConst(p_pp), heapID ) - 48;
      p_wk->leg_pos = MATH_CLAMP( p_wk->leg_pos, 0, 48 );
 
      GFL_ARC_CloseDataHandle( p_handle );
      GFL_HEAP_FreeMemory( p_pp );
    }
    break;

  case MYSTERYGIFT_TYPE_ITEM:
    {	
      const GIFT_PRESENT_ITEM  *cp_item = &cp_data->data.data.item;
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_ITEMICON, heapID );

      p_wk->res_plt	= GFL_CLGRP_PLTT_Register( p_handle, 
          ITEM_GetIndex( cp_item->itemNo, ITEM_GET_ICON_PAL ), CLSYS_DRAW_MAIN, PLT_OBJ_GIFT_M*0x20, heapID );

      p_wk->res_cel	= GFL_CLGRP_CELLANIM_Register( p_handle,
          NARC_item_icon_itemicon_NCER, NARC_item_icon_itemicon_NANR, heapID );

      p_wk->res_cgx	= GFL_CLGRP_CGR_Register( p_handle,
          ITEM_GetIndex( cp_item->itemNo, ITEM_GET_ICON_CGX ), FALSE, CLSYS_DRAW_MAIN, heapID );

      GFL_ARC_CloseDataHandle( p_handle );
      p_wk->x_ofs = 4;
      p_wk->leg_pos = 8;
    }
    break;

  case MYSTERYGIFT_TYPE_POWER:
    {	
      ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_MYSTERY, heapID );

      p_wk->res_plt	= GFL_CLGRP_PLTT_Register( p_handle, 
          NARC_mystery_fushigi_box_NCLR, CLSYS_DRAW_MAIN, PLT_OBJ_GIFT_M*0x20, heapID );

      p_wk->res_cel	= GFL_CLGRP_CELLANIM_Register( p_handle,
          NARC_mystery_fushigi_box_NCER, NARC_mystery_fushigi_box_NANR, heapID );

      p_wk->res_cgx	= GFL_CLGRP_CGR_Register( p_handle,
          NARC_mystery_fushigi_box_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

      GFL_ARC_CloseDataHandle( p_handle );
      p_wk->leg_pos = 8;
    }
    break;
  default:
    GF_ASSERT( 0 );
    break;
  }


  //映画配信ではパレットフェードさせるので転送したパレットを保存
  //メインOBJ
  if( cp_data->movie_flag )
  { 
    u32 plt_addr  = HW_OBJ_PLTT + PLT_OBJ_GIFT_M * 0x20;
    GFL_STD_MemCopy( (void*)plt_addr, p_wk->plt_poke_dst, sizeof(u16)*0x10 );

    //転送したら真っ白にする
    GFL_STD_MemFill16( (void*)plt_addr, GX_RGB(31,31,31), sizeof(u16)*0x10 );
    GFL_STD_MemCopy( (void*)plt_addr, p_wk->plt_poke_src, sizeof(u16)*0x10 );
  }

	//CLWK登録
	{
		int i;
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.pos_x  = MYSTERY_DEMO_GIFT_X + p_wk->x_ofs;
    cldata.pos_y  = MYSTERY_DEMO_MOVE_GIFT_START_Y;
    cldata.bgpri  = BG_FRAME_M_BACK1;

		p_wk->p_clwk	=	GFL_CLACT_WK_Create( p_unit,
				p_wk->res_cgx,
				p_wk->res_plt,
				p_wk->res_cel,
				&cldata,
				CLSYS_DEFREND_MAIN,
				heapID );

    GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, FALSE );


    if( cp_data->movie_flag )
    {
      GFL_CLACT_WK_SetObjMode( p_wk->p_clwk, GX_OAM_MODE_XLU );
    }
	}


}
//----------------------------------------------------------------------------
/**
 *	@brief  デモ開始
 *
 *	@param	MYSTERY_DEMO_WORK *p_wk ワーク
 *	@param	type                    開始するデモのタイプ
 */
//-----------------------------------------------------------------------------
void MYSTERY_DEMO_Start( MYSTERY_DEMO_WORK *p_wk, MYSTERY_DEMO_TYPE type )
{ 
  p_wk->is_end  = FALSE;
  p_wk->seq     = 0;
  p_wk->sync    = 0;


  //動作関数
  switch( type )
  { 
  case MYSTERY_DEMO_TYPE_RECV: //受信時
    if( p_wk->cp_data->movie_flag )
    { 
      p_wk->move_function = Mystery_Demo_MovieMain;
    }
    else
    { 
      p_wk->move_function = Mystery_Demo_NormalMain;
    }
    break;

  case MYSTERY_DEMO_TYPE_FINISH:
    p_wk->move_function = Mystery_Demo_FinishMain;
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  受信デモ  破棄
 *
 *	@param	MYSTERY_DEMO_WORK *p_wk ワーク
 *
 */
//-----------------------------------------------------------------------------
static void MYSTERY_DEMO_Exit( MYSTERY_DEMO_WORK *p_wk )
{ 
  if( MYSTERY_PTC_IsUse( &p_wk->ptc ) )
  { 
    MYSTERY_PTC_Exit( &p_wk->ptc );
  }

  GFL_BG_SetVisible( BG_FRAME_M_BACK1, FALSE ); 

	//CLWK破棄
	{	
    GFL_CLACT_WK_Remove( p_wk->p_clwk );
	}

	//リソース破棄
	{
    GFL_CLGRP_PLTT_Release( p_wk->res_plt );
    GFL_CLGRP_CGR_Release( p_wk->res_cgx );
    GFL_CLGRP_CELLANIM_Release( p_wk->res_cel );
	}

  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_DEMO_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief  受信デモ  メイン処理
 *
 *	@param	MYSTERY_DEMO_WORK *p_wk ワーク
 *
 */
//-----------------------------------------------------------------------------
static void MYSTERY_DEMO_Main( MYSTERY_DEMO_WORK *p_wk )
{
  if( p_wk->move_function )
  { 
    p_wk->move_function( p_wk );

    if( p_wk->is_end )
    { 
      p_wk->move_function = NULL;
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  受信デモ  終了チェック
 *
 *	@param	const MYSTERY_DEMO_WORK *cp_wk  ワーク
 *
 *	@return TRUEでデモ終了  FALSEでデモ中
 */
//-----------------------------------------------------------------------------
static BOOL MYSTERY_DEMO_IsEnd( const MYSTERY_DEMO_WORK *cp_wk )
{ 
  return cp_wk->is_end;
}

//----------------------------------------------------------------------------
/**
 *	@brief  通常デモ
 *
 *	@param	MYSTERY_DEMO_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Mystery_Demo_NormalMain( MYSTERY_DEMO_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_START_FADEIN,
    SEQ_WAIT_FADEIN,
    SEQ_INIT_WAIT,
    SEQ_MOVE,
    SEQ_END_WAIT,
    SEQ_END,
  };

  switch( p_wk->seq )
  {
  case SEQ_INIT:
    GFL_BG_SetVisible( BG_FRAME_M_BACK1, FALSE );
    BG_Load( p_wk->p_bg, BG_LOAD_TYPE_STAGE_M );
    p_wk->seq = SEQ_START_FADEIN;
    break;

  case SEQ_START_FADEIN:
    G2_SetBlendAlpha(
          GX_BLEND_PLANEMASK_BG2,
          GX_BLEND_PLANEMASK_BG3,
          0,
          16
          );
    GFL_BG_SetVisible( BG_FRAME_M_BACK1, TRUE );
    p_wk->sync  = 0;
    p_wk->seq = SEQ_WAIT_FADEIN;
    break;

  case SEQ_WAIT_FADEIN:
    //お盆のアルファフェード
    {
      s16 ev1 = 0 + 16 * p_wk->sync / MYSTERY_DEMO_STAGE_FADE_SYNC;
      s16 ev2 = 16 - 16 * p_wk->sync / MYSTERY_DEMO_STAGE_FADE_SYNC;
      G2_ChangeBlendAlpha( ev1, ev2 );
    }
    //BGのパレットフェード
    { 
      int i;
      p_wk->plt_cnt = 0 + 0x7FFF * p_wk->sync / MYSTERY_DEMO_STAGE_FADE_SYNC;
#if 0
      for( i = 0; i < 0x10; i++ )
      { 
        MYSTERY_UTIL_MainPltAnm( NNS_GFD_DST_2D_BG_PLTT_MAIN, &p_wk->plt[i], p_wk->plt_cnt, PLT_BG_BACK_M, i, p_wk->plt_src[i], p_wk->plt_dst[i] );
        MYSTERY_UTIL_MainPltAnm( NNS_GFD_DST_2D_BG_PLTT_SUB, &p_wk->plt_sub[i], p_wk->plt_cnt, PLT_BG_BACK_S, i, p_wk->plt_src[i], p_wk->plt_dst[i] );
      }
#else
      MYSTERY_UTIL_MainPltAnmLine( NNS_GFD_DST_2D_BG_PLTT_MAIN, p_wk->plt, p_wk->plt_cnt, PLT_BG_BACK_M, p_wk->plt_src, p_wk->plt_dst );
      MYSTERY_UTIL_MainPltAnmLine( NNS_GFD_DST_2D_BG_PLTT_SUB, p_wk->plt_sub, p_wk->plt_cnt, PLT_BG_BACK_S, p_wk->plt_src, p_wk->plt_dst );
#endif
    }
    //終了チェック
    if( p_wk->sync++ > MYSTERY_DEMO_STAGE_FADE_SYNC )
    { 
      G2_BlendNone();

      MYSTERY_EFFECT_Start( p_wk->p_effect, MYSTERY_EFFECT_TYPE_DEMO );
      p_wk->sync  = 0;
      p_wk->seq = SEQ_INIT_WAIT;
    }
    break;

  case SEQ_INIT_WAIT:
    if( p_wk->sync++ > MYSTERY_DEMO_INIT_WAIT_SYNC )
    { 
      p_wk->sync  = 0;
      p_wk->seq   = SEQ_MOVE;
    }
    break;

  case SEQ_MOVE:
    { 
      const s32 move_diff_y = MYSTERY_DEMO_MOVE_GIFT_END_Y - p_wk->leg_pos -MYSTERY_DEMO_MOVE_GIFT_START_Y;
      s16 pos;


      pos = MYSTERY_DEMO_MOVE_GIFT_START_Y + move_diff_y * p_wk->sync / MYSTERY_DEMO_MOVE_GIFT_SYNC;
      GFL_CLACT_WK_SetTypePos( p_wk->p_clwk, pos, CLSYS_DEFREND_MAIN, CLSYS_MAT_Y );

      if( 0 - 6*8 <= pos && pos <= 192 )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
      }

      if( p_wk->sync++ >= MYSTERY_DEMO_MOVE_GIFT_SYNC )
      { 
        PMSND_PlaySE( MYSTERY_SE_RECV_PRESENT );

        MYSTERY_EFFECT_Start( p_wk->p_effect, MYSTERY_EFFECT_TYPE_NORMAL );
        p_wk->sync  = 0;
        p_wk->seq   = SEQ_END_WAIT;
      }
    }
    break;

  case SEQ_END_WAIT:
    if( p_wk->sync++ > MYSTERY_DEMO_END_WAIT_SYNC )
    { 
      p_wk->sync  = 0;
      p_wk->seq   = SEQ_END;
    }
    break;

  case SEQ_END:
    p_wk->is_end  = TRUE;
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  映画用デモ
 *
 *	@param	MYSTERY_DEMO_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Mystery_Demo_MovieMain( MYSTERY_DEMO_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_START_FADEIN,
    SEQ_WAIT_FADEIN,
    SEQ_INIT_WAIT,

    SEQ_APPEAR_POKE_INIT,
    SEQ_APPEAR_POKE_WAIT,
    SEQ_PTC_FALL_WAIT,
    SEQ_PTC_EMIT_WAIT,

    SEQ_FADE_POKE_INIT,
    SEQ_FADE_POKE_WAIT,

#if 0
    SEQ_FADEOUT_INIT,
    SEQ_FADEOUT_WAIT,
    SEQ_FADEIN_INIT,
    SEQ_FADEIN_WAIT,
#endif

    SEQ_END_WAIT,
    SEQ_END,
  };

  switch( p_wk->seq )
  {
  case SEQ_INIT:
    MYSTERY_PTC_Init( &p_wk->ptc, p_wk->heapID );
    MYSTERY_PTC_LoadResource( &p_wk->ptc, ARCID_MYSTERY, NARC_mystery_mystery_spa, p_wk->heapID );


    GFL_BG_SetVisible( BG_FRAME_M_BACK1, FALSE );
    BG_Load( p_wk->p_bg, BG_LOAD_TYPE_STAGE_M );
    p_wk->seq = SEQ_START_FADEIN;
    break;

  case SEQ_START_FADEIN:
    G2_SetBlendAlpha(
          GX_BLEND_PLANEMASK_BG2,
          GX_BLEND_PLANEMASK_BG3,
          0,
          16
          );
    GFL_BG_SetVisible( BG_FRAME_M_BACK1, TRUE );
    p_wk->sync  = 0;
    p_wk->seq = SEQ_WAIT_FADEIN;
    break;

  case SEQ_WAIT_FADEIN:
    //お盆のアルファフェード
    {
      s16 ev1 = 0 + 16 * p_wk->sync / MYSTERY_DEMO_STAGE_FADE_SYNC;
      s16 ev2 = 16 - 16 * p_wk->sync / MYSTERY_DEMO_STAGE_FADE_SYNC;
      G2_ChangeBlendAlpha( ev1, ev2 );
    }
    //BGのパレットフェード
    { 
      int i;
      p_wk->plt_cnt = 0 + 0x7FFF * p_wk->sync / MYSTERY_DEMO_STAGE_FADE_SYNC;

#if 0
      for( i = 0; i < 0x10; i++ )
      { 
        MYSTERY_UTIL_MainPltAnm( NNS_GFD_DST_2D_BG_PLTT_MAIN, &p_wk->plt[i], p_wk->plt_cnt, PLT_BG_BACK_M, i, p_wk->plt_src[i], p_wk->plt_dst[i] );
        MYSTERY_UTIL_MainPltAnm( NNS_GFD_DST_2D_BG_PLTT_SUB, &p_wk->plt_sub[i], p_wk->plt_cnt, PLT_BG_BACK_S, i, p_wk->plt_src[i], p_wk->plt_dst[i] );
      }
#else
      MYSTERY_UTIL_MainPltAnmLine( NNS_GFD_DST_2D_BG_PLTT_MAIN, p_wk->plt, p_wk->plt_cnt, PLT_BG_BACK_M, p_wk->plt_src, p_wk->plt_dst );
      MYSTERY_UTIL_MainPltAnmLine( NNS_GFD_DST_2D_BG_PLTT_SUB, p_wk->plt_sub, p_wk->plt_cnt, PLT_BG_BACK_S, p_wk->plt_src, p_wk->plt_dst );
#endif
    }
    //終了チェック
    if( p_wk->sync++ > MYSTERY_DEMO_STAGE_FADE_SYNC )
    { 
      G2_BlendNone();
      p_wk->sync  = 0;
      p_wk->seq = SEQ_INIT_WAIT;
    }
    break;

  case SEQ_INIT_WAIT:
    if( p_wk->sync++ > MYSTERY_DEMO_INIT_WAIT_SYNC )
    { 
      p_wk->sync  = 0;
      p_wk->seq   = SEQ_PTC_FALL_WAIT;
      //PTC+ポケモンOBJをアルファ

      G2_SetBlendAlpha(
          GX_BLEND_PLANEMASK_BG0,
          GX_BLEND_PLANEMASK_OBJ | GX_BLEND_PLANEMASK_BG0 | GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BG3 ,
          0,
          16
          );

      //光のパーティクルが降りてくる
      MYSTERY_PTC_Start( &p_wk->ptc, BIG_RING );
      MYSTERY_PTC_Start( &p_wk->ptc, DOWN_BALL );
      MYSTERY_PTC_Start( &p_wk->ptc, SPLASH_BALL );
      PMSND_PlaySE( MYSTERY_SE_MOVIE_FALL );
    }
    break;

  case SEQ_PTC_FALL_WAIT:
    if( p_wk->sync++ > MYSTERY_DEMO_PTC_FALL_SYNC )
    { 
      //PTC爆発
      PMSND_PlaySE( MYSTERY_SE_MOVIE_EMIT );
      p_wk->sync  = 0;
      p_wk->seq   = SEQ_APPEAR_POKE_INIT;

      MYSTERY_EFFECT_Start( p_wk->p_effect, MYSTERY_EFFECT_TYPE_DEMO );
    }
    break;


  case SEQ_APPEAR_POKE_INIT:
      //ポケモンON
      { 
        GFL_CLACTPOS  pos;
        pos.x = MYSTERY_DEMO_GIFT_X;
        pos.y = MYSTERY_DEMO_MOVE_GIFT_END_Y - p_wk->leg_pos;
        GFL_CLACT_WK_SetPos( p_wk->p_clwk, &pos, CLSYS_DRAW_MAIN );
      }
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );

      p_wk->fade_sync = 0;
      p_wk->seq   = SEQ_APPEAR_POKE_WAIT;
    break;

  case SEQ_APPEAR_POKE_WAIT:
    {
      s16 ev1 = 0 + 16 * p_wk->fade_sync / MYSTERY_DEMO_POKE_FADE_SYNC1;
      s16 ev2 = 16 - 16 * p_wk->fade_sync / MYSTERY_DEMO_POKE_FADE_SYNC1;
      G2_ChangeBlendAlpha( ev1, ev2 );

      p_wk->sync++;
      if( p_wk->fade_sync++ > MYSTERY_DEMO_POKE_FADE_SYNC1 )
      { 
        p_wk->fade_sync  = 0;
        p_wk->seq = SEQ_PTC_EMIT_WAIT;
      }
    }
    break;


  case SEQ_PTC_EMIT_WAIT:
    if( p_wk->sync++ > 90 ) //50 )
    { 
      //爆発終了
      G2_BlendNone();
      p_wk->sync  = 0;
      p_wk->seq   = SEQ_FADE_POKE_INIT;
    }
    break;
  case SEQ_FADE_POKE_INIT:
    if( p_wk->sync++ > MYSTERY_DEMO_POKE_WAIT_SYNC )
    { 
      p_wk->sync  = 0;
      p_wk->seq = SEQ_FADE_POKE_WAIT;
    }
    break;

  case SEQ_FADE_POKE_WAIT:
    {
      int i;
      p_wk->plt_cnt = 0 + 0x7FFF * p_wk->sync / MYSTERY_DEMO_POKE_FADE_SYNC2;

#if 0
      for( i = 0; i < 0x10; i++ )
      { 
        MYSTERY_UTIL_MainPltAnm( NNS_GFD_DST_2D_OBJ_PLTT_MAIN, &p_wk->plt_poke[i], p_wk->plt_cnt, PLT_OBJ_GIFT_M, i, p_wk->plt_poke_dst[i], p_wk->plt_poke_src[i] );
      }
#else
      MYSTERY_UTIL_MainPltAnmLine( NNS_GFD_DST_2D_OBJ_PLTT_MAIN, p_wk->plt_poke, p_wk->plt_cnt, PLT_OBJ_GIFT_M, p_wk->plt_poke_dst, p_wk->plt_poke_src );
#endif

      //終了チェック
      if( p_wk->sync++ > MYSTERY_DEMO_POKE_FADE_SYNC2 )
      { 

        MYSTERY_EFFECT_Start( p_wk->p_effect, MYSTERY_EFFECT_TYPE_NORMAL );
        p_wk->sync  = 0;
        p_wk->seq = SEQ_END_WAIT;
      }
    }
    break;
#if 0
  case SEQ_FADEOUT_INIT:
    if( p_wk->sync++ > 0 )
    { 
      p_wk->sync  = 0;
      GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 0, 16, 0 );
      p_wk->seq = SEQ_FADEOUT_WAIT;
    }
    break;
  case SEQ_FADEOUT_WAIT:
    if( !GFL_FADE_CheckFade() )
    { 
      p_wk->seq = SEQ_FADEIN_INIT;
    }
    break;
  case SEQ_FADEIN_INIT:

    GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_WHITEOUT, 16, 0, 2 );
    p_wk->seq = SEQ_FADEIN_WAIT;
    break;
  case SEQ_FADEIN_WAIT:
    if( !GFL_FADE_CheckFade() )
    { 
      p_wk->seq = SEQ_END_WAIT;
    }
    break;
#endif 
  case SEQ_END_WAIT:
    if( p_wk->sync++ > MYSTERY_DEMO_POKE_END_SYNC )
    { 
      PMSND_PlaySE( MYSTERY_SE_RECV_PRESENT );
      p_wk->sync  = 0;
      p_wk->seq = SEQ_END;
    }
    break;

  case SEQ_END:
    G2_BlendNone();
    p_wk->is_end  = TRUE;
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  戻るワーク
 *
 *	@param	MYSTERY_DEMO_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Mystery_Demo_FinishMain( MYSTERY_DEMO_WORK *p_wk )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_MAIN,
    SEQ_EXIT,
  };

  switch( p_wk->seq )
  { 
  case SEQ_INIT:
    p_wk->seq = SEQ_MAIN;
    break;

  case SEQ_MAIN:
    //BGのパレットフェード
    { 
      int i;
      p_wk->plt_cnt = 0 + 0x7FFF * p_wk->sync / MYSTERY_DEMO_FINISH_FADE_SYNC;
#if 0
      for( i = 0; i < 0x10; i++ )
      { 
        MYSTERY_UTIL_MainPltAnm( NNS_GFD_DST_2D_BG_PLTT_MAIN, &p_wk->plt[i], p_wk->plt_cnt, PLT_BG_BACK_M, i, p_wk->plt_dst[i], p_wk->plt_src[i] );
        MYSTERY_UTIL_MainPltAnm( NNS_GFD_DST_2D_BG_PLTT_SUB, &p_wk->plt_sub[i], p_wk->plt_cnt, PLT_BG_BACK_S, i, p_wk->plt_dst[i], p_wk->plt_src[i] );
      }
#else
      MYSTERY_UTIL_MainPltAnmLine( NNS_GFD_DST_2D_BG_PLTT_MAIN, p_wk->plt, p_wk->plt_cnt, PLT_BG_BACK_M, p_wk->plt_dst, p_wk->plt_src );
      MYSTERY_UTIL_MainPltAnmLine( NNS_GFD_DST_2D_BG_PLTT_SUB, p_wk->plt_sub, p_wk->plt_cnt, PLT_BG_BACK_S, p_wk->plt_dst, p_wk->plt_src );
#endif
    }
    //終了チェック
    if( p_wk->sync++ > MYSTERY_DEMO_FINISH_FADE_SYNC )
    { 
      p_wk->sync  = 0;
      p_wk->seq = SEQ_EXIT;
    }
    break;

  case SEQ_EXIT:
    p_wk->is_end  = TRUE;
    break;
  }
}

//=============================================================================
/**
 *  演出ワーク
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  演出ワーク  初期化
 *
 *	@param	MYSTERY_EFFECT_WORK *p_wk ワーク
 *	@param	heapID                    ヒープID
 */
//-----------------------------------------------------------------------------
static void MYSTERY_EFFECT_Init( MYSTERY_EFFECT_WORK *p_wk, const OBJ_WORK *cp_obj, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_EFFECT_WORK) );
  p_wk->scroll_add  = MYSTERY_EFFECT_DEFAULT_SCROLL_SPEED;
  p_wk->cp_obj  = cp_obj;
  p_wk->init_add    = p_wk->scroll_add;
  p_wk->heapID    = heapID;

  { 
    int i;
    GFL_CLWK      *p_clwk;
    GFL_CLACTPOS  pos;
    for( i = 0; i < EFFECT_BRIGHT_MAX; i++ )
    { 
      p_clwk  = OBJ_GetClwk( p_wk->cp_obj, OBJ_CLWKID_BRIGHT_TOP+i );
      GFL_CLACT_WK_GetWldPos( p_clwk, &pos );
      p_wk->bright_pos[i] = pos.y << FX32_SHIFT;
      p_wk->bright_add[i] = FX32_CONST(1.6) + GFUser_GetPublicRand0( FX32_CONST(1.0) );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  演出ワーク  破棄
 *
 *	@param	MYSTERY_EFFECT_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void MYSTERY_EFFECT_Exit( MYSTERY_EFFECT_WORK *p_wk )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_EFFECT_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  演出ワーク  メイン処理
 *
 *	@param	MYSTERY_EFFECT_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void MYSTERY_EFFECT_Main( MYSTERY_EFFECT_WORK *p_wk )
{ 
  if( p_wk->is_update )
  { 
    if( p_wk->is_start )
    { 
      if( p_wk->move_function )
      { 
        p_wk->move_function( p_wk );
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  演出ワーク  開始
 *
 *	@param	MYSTERY_EFFECT_WORK *p_wk ワーク
 *	@param	type                      開始種類
 */
//-----------------------------------------------------------------------------
static void MYSTERY_EFFECT_Start( MYSTERY_EFFECT_WORK *p_wk, MYSTERY_EFFECT_TYPE type )
{
  p_wk->is_start  = TRUE;
  p_wk->cnt       = 0;
  p_wk->init_add  = p_wk->scroll_add;

  switch( type )
  { 
  case MYSTERY_EFFECT_TYPE_NORMAL:
    p_wk->move_function = Mystery_Effect_NormalMain;
    break;

  case MYSTERY_EFFECT_TYPE_DEMO:
    p_wk->move_function = Mystery_Effect_DemoMain;
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  演出ワーク  アップデート設定
 *
 *	@param	MYSTERY_EFFECT_WORK *p_wk ワーク
 *	@param	is_update TRUEで動作  FALSEで停止
 */
//-----------------------------------------------------------------------------
static void MYSTERY_EFFECT_SetUpdateFlag( MYSTERY_EFFECT_WORK *p_wk, BOOL is_update )
{ 
  p_wk->is_update  = is_update;
}

//----------------------------------------------------------------------------
/**
 *	@brief  通常エフェクト動作関数
 *
 *	@param	MYSTERY_EFFECT_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Mystery_Effect_NormalMain( MYSTERY_EFFECT_WORK *p_wk )
{ 
  if( p_wk->scroll_add != MYSTERY_EFFECT_DEFAULT_SCROLL_SPEED )
  { 
    p_wk->scroll_add  = p_wk->init_add  + (MYSTERY_EFFECT_DEFAULT_SCROLL_SPEED-p_wk->init_add) * p_wk->cnt / MYSTERY_EFFECT_DEMO_SCROLL_CHANGE_SYNC;
    if( p_wk->cnt++ > MYSTERY_EFFECT_DEMO_SCROLL_CHANGE_SYNC )
    { 
      p_wk->scroll_add  = MYSTERY_EFFECT_DEFAULT_SCROLL_SPEED;
    }
  }
  { 
    int i;
    GFL_CLWK      *p_clwk;
    GFL_CLACTPOS  pos;
    for( i = 0; i < EFFECT_BRIGHT_MAX; i++ )
    { 
      p_clwk  = OBJ_GetClwk( p_wk->cp_obj, OBJ_CLWKID_BRIGHT_TOP+i );
      if( GFL_CLACT_WK_GetDrawEnable( p_clwk ) )
      { 
        p_wk->bright_pos[i] += p_wk->bright_add[ i ];

        GFL_CLACT_WK_GetWldPos( p_clwk, &pos );
        pos.y = p_wk->bright_pos[i] >> FX32_SHIFT;
        GFL_CLACT_WK_SetWldPos( p_clwk, &pos );

        if( 0 <= pos.y && pos.y <= 192 + 16 )
        { 
          GFL_CLACT_WK_SetDrawEnable( p_clwk, TRUE );
        }
        else
        { 
          GFL_CLACT_WK_SetDrawEnable( p_clwk, FALSE );
        }
      }
    }
  }

  p_wk->scroll_now  += p_wk->scroll_add;
  GFL_BG_SetScrollReq( BG_FRAME_M_BACK2, GFL_BG_SCROLL_Y_SET, p_wk->scroll_now >> FX32_SHIFT );
  GFL_BG_SetScrollReq( BG_FRAME_S_BACK2, GFL_BG_SCROLL_Y_SET, p_wk->scroll_now >> FX32_SHIFT );
}
//----------------------------------------------------------------------------
/**
 *	@brief  通常受け取り中デモエフェクト動作関数
 *
 *	@param	MYSTERY_EFFECT_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void Mystery_Effect_DemoMain( MYSTERY_EFFECT_WORK *p_wk )
{ 
  if( p_wk->scroll_add != MYSTERY_EFFECT_DEMO_SCROLL_SPEED )
  { 
    p_wk->scroll_add  = p_wk->init_add  + (MYSTERY_EFFECT_DEMO_SCROLL_SPEED-p_wk->init_add) * p_wk->cnt / MYSTERY_EFFECT_DEMO_SCROLL_CHANGE_SYNC;
    if( p_wk->cnt++ > MYSTERY_EFFECT_DEMO_SCROLL_CHANGE_SYNC )
    { 
      p_wk->scroll_add  = MYSTERY_EFFECT_DEMO_SCROLL_SPEED;
    }
  }
  { 
    int i;
    GFL_CLWK      *p_clwk;
    GFL_CLACTPOS  pos;
    for( i = 0; i < EFFECT_BRIGHT_MAX; i++ )
    { 
      p_wk->bright_pos[i] += p_wk->bright_add[ i ];
      if( p_wk->bright_pos[i]>= FX32_CONST(192 +16) )
      { 
        p_wk->bright_pos[i] = FX32_CONST( -16 );
      }

      p_clwk  = OBJ_GetClwk( p_wk->cp_obj, OBJ_CLWKID_BRIGHT_TOP+i );
      GFL_CLACT_WK_GetWldPos( p_clwk, &pos );
      pos.y = p_wk->bright_pos[i] >> FX32_SHIFT;
      GFL_CLACT_WK_SetWldPos( p_clwk, &pos );

      if( 0 <= pos.y && pos.y <= 192 + 16 )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_clwk, TRUE );
      }
      else
      { 
        GFL_CLACT_WK_SetDrawEnable( p_clwk, FALSE );
      }
    }
  }
  p_wk->scroll_now  += p_wk->scroll_add;
  GFL_BG_SetScrollReq( BG_FRAME_M_BACK2, GFL_BG_SCROLL_Y_SET, p_wk->scroll_now >> FX32_SHIFT );
  GFL_BG_SetScrollReq( BG_FRAME_S_BACK2, GFL_BG_SCROLL_Y_SET, p_wk->scroll_now >> FX32_SHIFT );
}
//=============================================================================
/**
 *    ぼたん
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ボタン初期化
 *
 *	@param	MYSTERY_BTN_WORK *p_wk  ワーク
 *	@param	heapID                  ヒープID
 */
//-----------------------------------------------------------------------------
static void MYSTERY_BTN_Init( MYSTERY_BTN_WORK *p_wk, const OBJ_WORK *cp_obj, GFL_CLUNIT *p_clunit, PRINT_QUE *p_que, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_BTN_WORK) );
  p_wk->cp_obj  = cp_obj;

  p_wk->p_bmpoam_sys  = BmpOam_Init( heapID, p_clunit );

  { 
    GFL_CLWK_DATA cldata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.pos_x  = 128 - 32;
    cldata.pos_y  = 177 - 8;
    cldata.softpri  = 0;
    cldata.bgpri    = 0;

    p_wk->p_msgoam  = MYSTERY_MSGOAM_Init( &cldata, 8, 2, cp_obj->res_plt[OBJ_RES_PLT_CURSOR], 3, CLSYS_DRAW_MAIN, p_wk->p_bmpoam_sys, p_que, heapID );
    MYSTERY_MSGOAM_SetStrColor( p_wk->p_msgoam, PRINTSYS_LSB_Make( 1, 3, 0) );
    MYSTERY_MSGOAM_SetStrPos( p_wk->p_msgoam, 0, 0, MYSTERY_MSGOAM_POS_WH_CENTER );
    MYSTERY_MSGOAM_Print( p_wk->p_msgoam, p_msg, strID, p_font );
  }

  //OBJ表示
  { 
    GFL_CLWK  *p_clwk = OBJ_GetClwk( cp_obj, OBJ_CLWKID_BTN );
    GFL_CLACT_WK_SetDrawEnable( p_clwk, TRUE );
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  ボタン破棄
 *
 *	@param	MYSTERY_BTN_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void MYSTERY_BTN_Exit( MYSTERY_BTN_WORK *p_wk )
{ 
  MYSTERY_MSGOAM_Clear( p_wk->p_msgoam );

  //OBJ表示
  { 
    GFL_CLWK  *p_clwk = OBJ_GetClwk( p_wk->cp_obj, OBJ_CLWKID_BTN );
    GFL_CLACT_WK_SetDrawEnable( p_clwk, FALSE );
  }

  MYSTERY_MSGOAM_Exit( p_wk->p_msgoam );
  BmpOam_Exit( p_wk->p_bmpoam_sys );
  GFL_STD_MemClear( p_wk, sizeof(MYSTERY_BTN_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ボタン  文字描画
 *
 *	@param	MYSTERY_BTN_WORK *p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void MYSTERY_BTN_PrintMain( MYSTERY_BTN_WORK *p_wk )
{
  MYSTERY_MSGOAM_PrintMain( p_wk->p_msgoam );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ポケモンの足元座標を取得
 *
 *	@param	const POKEMON_PASO_PARAM* cp_ppp  ポケモン
 *
 *	@return 足元座標(ドット単位）
 */
//-----------------------------------------------------------------------------
static u32 Mystery_GetLegPos( const POKEMON_PASO_PARAM* cp_ppp, HEAPID heapID )
{ 
  u32 temp;
  u32 pos = 48;  //中心にしておく

  void *p_chr_buff;
  NNSG2dCharacterData *p_chr_data;
  

  p_chr_buff  =  POKE2DGRA_LoadCharacterPPP( &p_chr_data, cp_ppp, POKEGRA_DIR_FRONT, GFL_HEAP_LOWID(heapID) );
  POKEGRA_SortBGCharacter( p_chr_data, GFL_HEAP_LOWID(heapID) );

  //足元サーチ
  { 
    int i, j, k;
    u32  chara_adrs;
    u32 *p_chara_y;

    BOOL exits  = FALSE;

    for( i = POKEGRA_POKEMON_CHARA_HEIGHT-1; i >= 0; i-- )
    { 
      for( j = 0; j < POKEGRA_POKEMON_CHARA_WIDTH; j++ )
      { 
        chara_adrs = ((u32)p_chr_data->pRawData + ((i * POKEGRA_POKEMON_CHARA_WIDTH + j ) * GFL_BG_1CHRDATASIZ));

        //Yを知れればよいのでYしか検知しない
        for( k = GFL_BG_1CHRDOTSIZ-1; k >= 0; k-- )
        { 
          p_chara_y = (u32*)(chara_adrs + k * 4);
          if( *p_chara_y != 0 )
          { 
            NAGI_Printf( "足元キャラx=%d y=%d doty=%d\n", j, i, k );
            temp = i*8+k;
            if( pos < temp )
            { 
              pos = temp;
            }
            exits = TRUE;
          }
        }
      }

      //横軸を全てチェックし終わったら終了
      if( exits )
      { 
        break;
      }
    }
  }

  GFL_HEAP_FreeMemory( p_chr_buff );

  return pos;
}
//----------------------------------------------------------------------------
/**
 *	@brief  データの正当性をチェック
 *
 *	@param	const DOWNLOAD_GIFT_DATA *cp_data   データ
 *
 *	@return TRUE正統  FALSE不正
 */
//-----------------------------------------------------------------------------
static BOOL Mystery_IsValid( const DOWNLOAD_GIFT_DATA *cp_data, u32 dirty )
{
  //１、CRCチェックがただしい
  //２、不正カウントが０
  //３、取得バージョンがただしい
  //４、中身のデータが正しい
  if( MYSTERYDATA_CheckCrc( cp_data )
      && dirty == 0 
      && ( cp_data->version == 0 || (cp_data->version & (1<<GET_VERSION())) ) )
  {
    return TRUE;
  }


  return FALSE;
}
