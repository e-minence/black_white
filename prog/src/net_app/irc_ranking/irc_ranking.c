//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_ranking.c
 *	@brief	赤外線ミニゲームランキング画面
 *	@author	Toru=Nagihashi
 *	@data		2009.07.10
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>

//	constant
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"

//	archive
#include "arc_def.h"
#include "message.naix"
#include "msg/msg_irc_ranking.h"
#include "ircranking_gra.naix"
#include "font/font.naix"

//	print
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"

//	module
#include "infowin/infowin.h"
#include "sound/pm_sndsys.h"
#include "net_app/irc_appbar.h"
#include "pokeicon/pokeicon.h"
#include "system/bmp_oam.h"

//	common
#include "app/app_menu_common.h"

//sound
#include "../irc_compatible/irc_compatible_snd.h"

//	save
#include "savedata/irc_compatible_savedata.h"
#include "savedata/save_tbl.h"

//	mine
#include "net_app/irc_ranking.h"
//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	マクロ
//=====================================
#ifdef PM_DEBUG
//#define DEBUG_RANKING_TICK
#endif

//タイム
#ifdef DEBUG_RANKING_TICK

static OSTick s_DEBUG_TICK_DRAW_start;
#define DEBUG_TICK_DRAW_Print		NAGI_Printf("line[%d] time=%dmicro\n",__LINE__,OS_TicksToMicroSeconds(OS_GetTick() - s_DEBUG_TICK_DRAW_start) )
#define DEBUG_TICK_DRAW_Start		s_DEBUG_TICK_DRAW_start = OS_GetTick()

#else
		
#define DEBUG_TICK_DRAW_Print
#define DEBUG_TICK_DRAW_Start

#endif

//-------------------------------------
///	パレット
//=====================================
enum
{	
	// メイン画面BG
	RANKING_BG_PAL_M_00 = 0,// 
	RANKING_BG_PAL_M_01,		// 
	RANKING_BG_PAL_M_02,		//
	RANKING_BG_PAL_M_03,		// 
	RANKING_BG_PAL_M_04,		// 
	RANKING_BG_PAL_M_05,		// 
	RANKING_BG_PAL_M_06,		// 
	RANKING_BG_PAL_M_07,		// 
	RANKING_BG_PAL_M_08,		// 使用してない
	RANKING_BG_PAL_M_09,		// 使用してない
	RANKING_BG_PAL_M_10,		// 使用してない
	RANKING_BG_PAL_M_11,		// 使用してない
	RANKING_BG_PAL_M_12,		// 使用してない
	RANKING_BG_PAL_M_13,		// 使用してない
	RANKING_BG_PAL_M_14,		// フォント
	RANKING_BG_PAL_M_15,		//


	// サブ画面BG
	RANKING_BG_PAL_S_00 = 0,	// 
	RANKING_BG_PAL_S_01,		//  
	RANKING_BG_PAL_S_02,		//  
	RANKING_BG_PAL_S_03,		//  
	RANKING_BG_PAL_S_04,		//  
	RANKING_BG_PAL_S_05,		//  
	RANKING_BG_PAL_S_06,		//  
	RANKING_BG_PAL_S_07,		// 使用してない
	RANKING_BG_PAL_S_08,		// 使用してない
	RANKING_BG_PAL_S_09,		// 使用してない
	RANKING_BG_PAL_S_10,		// 使用してない
	RANKING_BG_PAL_S_11,		// 使用してない
	RANKING_BG_PAL_S_12,		// 使用してない
	RANKING_BG_PAL_S_13,		// アプリバー
	RANKING_BG_PAL_S_14,		// フォント
	RANKING_BG_PAL_S_15,		// 使用してない

	// メイン画面OBJ
	RANKING_OBJ_PAL_POKEICON_M = 1,//

	RANKING_OBJ_PAL_FONT_M = 14,// 

	// サブ画面BG
	RANKING_OBJ_PAL_POKEICON_S = 1,// 
};

//-------------------------------------
///	SCROLL
//=====================================

//BMPWINインデックス
enum
{	
	SCROLL_BMPWIN_FONT_M,
	SCROLL_BMPWIN_FONT_S,
	SCROLL_BMPWIN_MAX
};


#define SCROLL_BAR_X					(2)
#define SCROLL_BAR_HEIGHT			(3)

#define SCROLL_BAR_ALL_WIDTH		(32)	//(28)
#define SCROLL_BAR_ALL_HEIGHT		(SCROLL_BAR_HEIGHT)

#define SCROLL_BAR_FONT_RANK_X		(3)
#define SCROLL_BAR_FONT_PLAYER_X	(6)
#define SCROLL_BAR_FONT_SCORE_X		(15)
#define SCROLL_BAR_FONT_COUNT_X		(22)

//余白
#define SCROLL_MARGIN_SIZE_Y_M	(-5*GFL_BG_1CHRDOTSIZ)
#define SCROLL_MARGIN_SIZE_Y_S	(3*GFL_BG_1CHRDOTSIZ)

#define SCROLL_REWITE_DISTANCE	(SCROLL_BAR_HEIGHT*GFL_BG_1CHRDOTSIZ)	//どのくらいの距離すすんだら張り替えて、戻すか
#define	SCROLL_WRITE_BAR_START_M	(0)	//開始インデックス
#define SCROLL_WRITE_BAR_END_M		(10)
#define SCROLL_WRITE_BAR_NUM_M		(SCROLL_WRITE_BAR_END_M-SCROLL_WRITE_BAR_START_M)
#define	SCROLL_WRITE_BAR_START_S	(7)	//開始インデックス
#define SCROLL_WRITE_BAR_END_S		(17)
#define SCROLL_WRITE_BAR_NUM_S		(SCROLL_WRITE_BAR_END_S-SCROLL_WRITE_BAR_START_S)
#define	SCROLL_WRITE_BAR_START_EX_M	(-1)	//EXが開始インデックスより前ならば、前にはる
#define	SCROLL_WRITE_BAR_START_EX_S	(4)	//EXが開始インデックスより前ならば、前にはる

#define SCROLL_WRITE_POS_START_M	(4)	//どの位置から張り始めるか
#define SCROLL_WRITE_POS_START_S	(0)	//どの位置から張り始めるか

#define SCROLL_FONT_Y_OFS	(1+4)	//文字Y位置

#define SCROLL_NEWRANK_NULL	(0xFFFF)


#define SCROLL_BAR_ICON_X		(29*8)
#define SCROLL_BAR_ICON_Y   (0)

#define SCROLL_BAR_ICON_INIT_Y_M  (74+8)
#define SCROLL_BAR_ICON_INIT_Y_S  (-(SCROLL_WRITE_BAR_START_EX_S*3)*8-16)  //*3はOBJが３キャラだから


//-------------------------------------
///		BMPWIN
//=====================================
enum
{	
	BMPWIN_ID_TITLE,
	BMPWIN_ID_MAX,
} ;

#define BMPWIN_TITLE_X				(3)
#define BMPWIN_TITLE_Y				(1)
#define BMPWIN_TITLE_W				(26)
#define BMPWIN_TITLE_H				(2)

#define BMPWIN_RANK_X					(1)
#define BMPWIN_RANK_Y					(5)
#define BMPWIN_RANK_W					(6)
#define BMPWIN_RANK_H					(2)

#define BMPWIN_PLAYER_X				(8)
#define BMPWIN_PLAYER_Y				(5)
#define BMPWIN_PLAYER_W				(6)
#define BMPWIN_PLAYER_H				(2)

#define BMPWIN_SCORE_X				(15)
#define BMPWIN_SCORE_Y				(5)
#define BMPWIN_SCORE_W				(6)
#define BMPWIN_SCORE_H				(2)

#define BMPWIN_COUNT_X				(24)
#define BMPWIN_COUNT_Y				(5)
#define BMPWIN_COUNT_W				(6)
#define BMPWIN_COUNT_H				(2)

#define BMPWIN_BAR_X					(0)
#define BMPWIN_BAR_Y					(21)
#define BMPWIN_BAR_W					(32)
#define BMPWIN_BAR_H					(3)

#define BMPWIN_FONT_X					(0)
#define	BMPWIN_FONT_Y					(0)
#define BMPWIN_FONT_W					(32)
#define	BMPWIN_FONT_H					(32)


//-------------------------------------
///	UI
//=====================================
#define UI_FLIK_RELEASE_SYNC	(1)		//はじくとき、離してから何シンク有効か

//-------------------------------------
///	ACLR
//=====================================
#define	ACLR_SCROLL_MOVE_MIN					(-FX32_CONST(8))
#define	ACLR_SCROLL_MOVE_MAX					(FX32_CONST(8))

#define ACLR_SCROLL_DISTANCE_MIN			(FX32_CONST(1))	//MIN以下だと反応しない
#define ACLR_SCROLL_DISTANCE_MAX			(FX32_CONST(60))	//以上は切捨て
#define ACLR_SCROLL_DISTANCE_DIF			(ACLR_SCROLL_DISTANCE_MAX-ACLR_SCROLL_DISTANCE_MIN)

#define ACLR_SCROLL_SYNC_MIN					(FX32_CONST(0))		//以下は切り捨て
#define ACLR_SCROLL_SYNC_MAX					(FX32_CONST(13))	//MAX以上だと反応しない
#define ACLR_SCROLL_SYNC_DIF					(ACLR_SCROLL_SYNC_MAX-ACLR_SCROLL_SYNC_MIN)

#define ACLR_SCROLL_ACLR_MIN					(FX32_CONST(0))
#define ACLR_SCROLL_ACLR_MAX					(FX32_CONST(40))
#define ACLR_SCROLL_ACLR_DIF					(ACLR_SCROLL_ACLR_MAX-ACLR_SCROLL_ACLR_MIN)

#define ACLR_SCROLL_DEC_RATE					(FX32_CONST(0.05))	//減衰率

#define ACLR_SCROLL_KEY_MIN						(FX32_CONST(20))	
#define ACLR_SCROLL_KEY_MAX						(FX32_CONST(60))
#define ACLR_SCROLL_KEY_DIF						(ACLR_SCROLL_KEY_MAX-ACLR_SCROLL_KEY_MIN)

//キーでの移動パワー
#define ACLR_SCROLL_KEY_MOVE_INIT_DISTANCE	(ACLR_SCROLL_DISTANCE_MIN+FX32_ONE)
#define ACLR_SCROLL_KEY_MOVE_INIT_SYNC			((ACLR_SCROLL_SYNC_MAX-FX32_ONE)>>FX32_SHIFT)
#define ACLR_SCROLL_KEY_MOVE_ACLR_SYNC			((ACLR_SCROLL_SYNC_MAX-FX32_ONE)>>FX32_SHIFT)

#define	ACLR_SCROLL_NEWRANK_OFS				(192)

//-------------------------------------
///	DIV
//=====================================
#define DRAG_MOVE_DIV_ADD_RATE				(FX32_CONST(0.04f))
#define DRAG_MOVE_DIV_MAX							(20)
#define DRAG_MOVE_INIT_DISTANCE				(1)


//-------------------------------------
///	OBJ登録ID
//=====================================
enum {
	OBJREGID_TOUCH_PLT,
	OBJREGID_TOUCH_CHR,
	OBJREGID_TOUCH_CEL,

	OBJREGID_MAX
};
//-------------------------------------
///	CLWK取得
//=====================================
typedef enum{	
	CLWKID_TOUCH,
	
	CLWKID_MAX
}CLWKID;


enum
{
  DISPLAY_TOP,
  DISPLAY_BOTTOM,
  DISPLAY_NUM,
} ;


enum
{ 
  MSGOAM_RANK,
  MSGOAM_NAME,
  MSGOAM_SCORE,
  MSGOAM_COUNT,
  MSGOAM_MAX,
} ;

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	ランキングデータ
//=====================================
typedef struct 
{
	STRBUF	*p_name;
	u32			score			:7;
	u32			play_cnt	:10;
	u32			plt				:8;
	u32			rank			:7;
  u16     mons_no;
  u8      form_no;
  u8      sex;
  u8      egg;
  u8      new;
} RANKING_ONE_DATA;
typedef struct
{ 
  u32               rank_num;
  RANKING_ONE_DATA  data[0];
} RANKING_DATA;

//-------------------------------------
///	メッセージOAM
//=====================================
typedef struct _RANKING_MSGOAM_WORK RANKING_MSGOAM_WORK;

//-------------------------------------
///	BGワーク
//=====================================
typedef struct 
{
	ARCHANDLE		*p_handle;
	GFL_ARCUTIL_TRANSINFO	bar_char_m;
	GFL_ARCUTIL_TRANSINFO	bar_char_s;
	GFL_BMPWIN	*p_bmpwin[BMPWIN_ID_MAX];
} GRAPHIC_BG_WORK;
//-------------------------------------
///	OBJワーク
//=====================================
typedef struct 
{
	GFL_CLUNIT *p_clunit;
	u32					reg_id[OBJREGID_MAX];
	GFL_CLWK	 *p_clwk[CLWKID_MAX];
} GRAPHIC_OBJ_WORK;
//-------------------------------------
///	SCROLLワーク
//====================================
typedef struct 
{
  u16   rank_screen_main[SCROLL_BAR_ALL_HEIGHT*SCROLL_BAR_ALL_WIDTH];
  u16   rank_screen_sub[SCROLL_BAR_ALL_HEIGHT*SCROLL_BAR_ALL_WIDTH];
} BAR_BUFF;

//ランク１つのワーク
typedef struct 
{
  const RANKING_ONE_DATA *cp_data;
  GFL_BMP_DATA  *p_bmp;     //フォントデータ  文字バッファ
  GFL_CLWK      *p_icon[ CLSYS_DEFREND_NUM ];    //ポケモンアイコン
  s16           icon_init_pos[ CLSYS_DEFREND_NUM ];
  u32           chr_idx;    //ポケアイコンリソース
  u32           cel_idx;    //ポケアイコンリソース
  BAR_BUFF      *p_buff;
} RANKBAR_WORK;

//スクロールワーク
typedef struct 
{
  u32 icon_plt_idx_m;
  u32 icon_plt_idx_s;
  BAR_BUFF      barbuff;        //バーのスクリーンデータ
  RANKBAR_WORK  rankbar[ IRC_COMPATIBLE_SV_RANKING_MAX ];

	u8	bar_frm_m;			//上画面バー表示面
	u8	font_frm_m;			//上画面フォント表示面
	u8	bar_frm_s;			//下画面バー表示面
	u8	font_frm_s;			//下画面バー表示面
	const GRAPHIC_BG_WORK	*cp_bg;		//バー用キャラ受け取りのため
	const RANKING_DATA		*cp_data;	//ランキングのデータ
	u16	data_len;										//ランキングのデータ長
	s16 y;													//スクロール絶対位置
	s16	top_limit_y;								//上方向上限
	s16	bottom_limit_y;							//下方向下限
	s16	top_bar;										//張り替える際の一番上のランキングでーたインデックス
	s16 add_y;											//スクロール移動方向
	s16 pre_dir;										//前の移動方向（+or-）
	s16 is_zero_next;								//0の次は方向を変えてもPREへいかないようにする

	s16	rewrite_scr_pos_m;					//張り替える上画面位置
	s16	rewrite_scr_pos_s;					//張り替える下画面位置

	GFL_BMPWIN		*p_bmpwin[SCROLL_BMPWIN_MAX];	//フォント用BMPWIN

	GFL_FONT			*p_font;
  GFL_MSGDATA		*p_msg;

	STRBUF				*p_rank_buf;
	STRBUF				*p_score_buf;
	STRBUF				*p_count_buf;
	STRBUF				*p_strbuf;

	WORDSET				*p_wordset;
	GFL_TCB				*p_vblank_task;
	BOOL					is_update;					//VBLANK張替えアップデート
	BOOL					is_move_req;				//移動リクエスト（メインへの通知）
	BOOL					is_move_update;			//VBLANK移動リクエスト
} SCROLL_WORK;

//-------------------------------------
///	加速度ワーク
//=====================================
typedef struct 
{
	fx32	distance_rate;
	fx32	sync_rate;
	fx32	aclr;
	BOOL	is_update;	//設定したので計算UPDATE
	s8		dir;				//方向
	u8		dummy;
} ACLR_WORK;

//-------------------------------------
///	グラフィックワーク
//=====================================
typedef struct 
{
	GRAPHIC_BG_WORK		bg;
	GRAPHIC_OBJ_WORK	obj;
	GFL_TCB	*p_vblank_task;
} GRAPHIC_WORK;
//-------------------------------------
///	タッチ
//=====================================
typedef struct 
{
	GFL_POINT start;
	GFL_POINT end;
  GFL_POINT drag_start;
  GFL_POINT drag_pos;
	u32	flik_sync;
	u32	release_sync;
	u32 key_cont_sync;
} UI_WORK;
//-------------------------------------
///	シーケンス管理
//=====================================
typedef struct _SEQ_WORK SEQ_WORK;
typedef void (*SEQ_FUNCTION)( SEQ_WORK *p_wk, int *p_seq, void *p_param );
struct _SEQ_WORK
{
	SEQ_FUNCTION	seq_function;
	BOOL is_end;
	int seq;
	void *p_param;
}; 

//-------------------------------------
///	メインワーク
//=====================================
typedef struct _IRC_RANKING_WORK
{
	//モジュール
	GRAPHIC_WORK	grp;
	SEQ_WORK			seq;
	SCROLL_WORK		scroll;
	UI_WORK				ui;
	ACLR_WORK			aclr;
  APPBAR_WORK		*p_appbar;
  GFL_FONT      *p_font;
  PRINT_QUE     *p_que;
  GFL_MSGDATA		*p_msg;
  BMPOAM_SYS_PTR  p_bmpoam_sys; 

  GAMEDATA      *p_gamedata;

  RANKING_MSGOAM_WORK *p_msgoam[  MSGOAM_MAX ];

	//データ
	RANKING_DATA	*p_rank_data;

	//etc
	s16	move_new_sync;
	u16 dummy;
  u32 font_plt;
}IRC_RANKING_WORK;

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
static GFL_PROC_RESULT IRC_RANKING_PROC_Init(
		GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_RANKING_PROC_Exit(
		GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_RANKING_PROC_Main(
		GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
//-------------------------------------
///	SEQ
//=====================================
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param, SEQ_FUNCTION seq_function );
static void SEQ_Exit( SEQ_WORK *p_wk );
static void SEQ_Main( SEQ_WORK *p_wk );
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk );
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function );
static void SEQ_End( SEQ_WORK *p_wk );
//-------------------------------------
///	SEQFUNC
//=====================================
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_MoveNew( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
//-------------------------------------
///	BG
//=====================================
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, GAMEDATA  *p_gamedata, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
static GFL_ARCUTIL_TRANSINFO GRAPHIC_BG_GetTransInfo( const GRAPHIC_BG_WORK *cp_wk, BOOL is_m );
//-------------------------------------
///	OBJ
//=====================================
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLWK* GRAPHIC_OBJ_GetClwk( const GRAPHIC_OBJ_WORK *cp_wk, CLWKID id );
static GFL_CLUNIT * GRAPHIC_OBJ_GetUnit( const GRAPHIC_OBJ_WORK *cp_wk );
//-------------------------------------
///	GRAPHIC
//=====================================
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, GAMEDATA  *p_gamedata, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk );
static const GRAPHIC_BG_WORK *GRAPHIC_GetBgWorkConst( const GRAPHIC_WORK *cp_wk );
static GFL_CLWK	* GRAPHIC_GetClwk( const GRAPHIC_WORK *cp_wk, CLWKID id );
static GFL_CLUNIT * GRAPHIC_GetUnit( const GRAPHIC_WORK *cp_wk );
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );


//-------------------------------------
///	SCROLL	
//=====================================
static void SCROLL_Init( SCROLL_WORK *p_wk, u8 bar_frm_m, u8 font_frm_m, u8 bar_frm_s, u8 font_frm_s, const GRAPHIC_BG_WORK *cp_bg, GFL_CLUNIT *p_clunit, GFL_MSGDATA	*p_msg, const RANKING_DATA* cp_data, HEAPID heapID );
static void SCROLL_Exit( SCROLL_WORK *p_wk );
static void SCROLL_Main( SCROLL_WORK *p_wk );
static void SCROLL_AddPos( SCROLL_WORK *p_wk, s16 y );
static s16 SCROLL_GetPosY( const SCROLL_WORK *cp_wk );
static s16 SCROLL_GetNewRankPosY( const SCROLL_WORK *cp_wk );
static s16 SCROLL_GetBottomLimitPosY( const SCROLL_WORK *cp_wk );
static void Scroll_Write( SCROLL_WORK *p_wk );
static void Scroll_WriteVBlank( SCROLL_WORK *p_wk );
static void Scroll_VBlankTask( GFL_TCB *p_tcb, void *p_wk_adrs );
//-------------------------------------
///	RANKBAR
//=====================================
static void RANKBAR_Init( RANKBAR_WORK *p_wk, const RANKING_ONE_DATA *cp_data, GFL_CLUNIT *p_clunit, u32 icon_plt_idx_m, u32 icon_plt_idx_s, GFL_MSGDATA	*p_msg, GFL_FONT *p_font, WORDSET *p_wordset, BAR_BUFF *p_bar, s16 icon_pos_m, s16 icon_pos_s, HEAPID heapID );
static void RANKBAR_Exit( RANKBAR_WORK *p_wk );
static void RANKBAR_Trans( RANKBAR_WORK *p_wk, GFL_BMPWIN *p_font_bmpwin, u8 bar_frm, u16 y );
static void RANKBAR_Move( RANKBAR_WORK *p_wk, s16 bg_y );
//-------------------------------------
///	BAR_BUFF
//=====================================
static void BARBUFF_Init( BAR_BUFF *p_wk, const GRAPHIC_BG_WORK *cp_bg, HEAPID heapID );
static void BARBUFF_Exit( BAR_BUFF *p_wk );
static void BARBUFF_Trans( BAR_BUFF *p_wk, u8 frm, u16 y, u8 plt );
static void BARBUFF_Clear( BAR_BUFF *p_wk, u8 frm, u16 y );
//-------------------------------------
///	RANKING_DATA
//=====================================
static RANKING_DATA	*RANKING_DATA_Create( GAMEDATA *p_gamedata, HEAPID heapID );
static void RANKING_DATA_Delete( RANKING_DATA	*p_data );
static u32 RANKING_DATA_GetRankNum( const RANKING_DATA *cp_wk );
//-------------------------------------
///	TOUCH
//=====================================
static void UI_Init( UI_WORK *p_wk, HEAPID heapID );
static void UI_Exit( UI_WORK *p_wk );
static void UI_Main( UI_WORK *p_wk );
static BOOL UI_GetDrag( UI_WORK *p_wk, GFL_POINT *p_start, GFL_POINT *p_end, GFL_POINT *p_drag_pos );
static BOOL UI_GetFlik( UI_WORK *p_wk, GFL_POINT *p_start, GFL_POINT *p_end, VecFx32 *p_vec, u32 *p_sync );
static int UI_IsContKey( const UI_WORK *cp_wk, u32 *p_sync );
//-------------------------------------
///	ACLR
//=====================================
static void ACLR_Init( ACLR_WORK *p_wk );
static void ACLR_Exit( ACLR_WORK *p_wk );
static void ACLR_Main( ACLR_WORK *p_wk );
static void ACLR_SetAclr( ACLR_WORK *p_wk, fx32 distance, u32 sync );
static void ACLR_Stop( ACLR_WORK *p_wk );
static s16 ACLR_GetScrollAdd( ACLR_WORK *p_wk );
static BOOL ACLR_IsExist( const ACLR_WORK *cp_wk );
//-------------------------------------
///	ETC
//=====================================
static void PRINT_PrintCenter( GFL_BMPWIN *p_bmpwin, STRBUF *p_strbuf, GFL_FONT *p_font );
static void PRINT_PrintNameCenter( GFL_BMPWIN *p_bmpwin, STRBUF *p_strbuf, const MYSTATUS *cp_status, GFL_FONT *p_font );
static void BMP_Copy( const GFL_BMP_DATA *cp_src, GFL_BMP_DATA *p_dst, u16 src_x, u16 src_y, u16 dst_x, u16 dst_y, u16 w, u16 h );
static s32 GFL_POINT_Distance( const GFL_POINT *cp_a, const GFL_POINT *cp_b );
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h,  u8 plt, BOOL compressedFlag, HEAPID heapID );

//-------------------------------------
///	座標位置
//=====================================
typedef enum 
{
  RANKING_MSGOAM_POS_ABSOLUTE,    //絶対位置
  RANKING_MSGOAM_POS_WH_CENTER,  //相対座標  幅、高さともに中央
  RANKING_MSGOAM_POS_H_CENTER,  //相対座標  高さのみ中央
} RANKING_MSGOAM_POS;

//-------------------------------------
///	メッセージOAM
//=====================================
//-------------------------------------
///	パブリック
//=====================================
static RANKING_MSGOAM_WORK * RANKING_MSGOAM_Init( const GFL_CLWK_DATA *cp_cldata, u8 w, u8 h, u32 plt_idx, u8 plt_ofs, CLSYS_DRAW_TYPE draw_type, BMPOAM_SYS_PTR p_bmpoam_sys, PRINT_QUE *p_que, HEAPID heapID );
static void RANKING_MSGOAM_Exit( RANKING_MSGOAM_WORK* p_wk );
static void RANKING_MSGOAM_Clear( RANKING_MSGOAM_WORK* p_wk );
static void RANKING_MSGOAM_Print( RANKING_MSGOAM_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font );
static void RANKING_MSGOAM_PrintBuf( RANKING_MSGOAM_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font );
static void RANKING_MSGOAM_SetStrColor( RANKING_MSGOAM_WORK* p_wk, PRINTSYS_LSB color );
static void RANKING_MSGOAM_SetStrPos( RANKING_MSGOAM_WORK* p_wk, s16 x, s16 y, RANKING_MSGOAM_POS type );
static BOOL RANKING_MSGOAM_PrintMain( RANKING_MSGOAM_WORK* p_wk );
static BMPOAM_ACT_PTR RANKING_MSGOAM_GetBmpOamAct( RANKING_MSGOAM_WORK* p_wk );


//=============================================================================
/**
 *						DATA
 */
//=============================================================================
//-------------------------------------
///	データ
//=====================================
enum
{	
	GRAPHIC_BG_FRAME_TOP_M,
	GRAPHIC_BG_FRAME_BAR_M,
	GRAPHIC_BG_FRAME_FONT_M,
	GRAPHIC_BG_FRAME_BACK_M,

	GRAPHIC_BG_FRAME_INFO_S,
	GRAPHIC_BG_FRAME_BAR_S,
	GRAPHIC_BG_FRAME_FONT_S,
	GRAPHIC_BG_FRAME_BACK_S,

	GRAPHIC_BG_FRAME_MAX
};
static const struct 
{
	u32									frame;
	GFL_BG_BGCNT_HEADER	bgcnt_header;
	u32									mode;
}	sc_bgsetup[GRAPHIC_BG_FRAME_MAX]	=
{	
	{	
		GFL_BG_FRAME0_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME1_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME2_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME3_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},


	//BOTTOM
	{	
		GFL_BG_FRAME0_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME1_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME2_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME3_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	}
};
//フレーム取得用マクロ
#define GRAPHIC_BG_GetFrame( x )	(sc_bgsetup[ x ].frame)

//=============================================================================
/**
 *						GLOBAL
 */
//=============================================================================
//-------------------------------------
///	PROCデータ外部公開
//=====================================
const GFL_PROC_DATA	IrcRanking_ProcData	=
{
	IRC_RANKING_PROC_Init,
	IRC_RANKING_PROC_Main,
	IRC_RANKING_PROC_Exit,
} ;

//=============================================================================
/**
 *						PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	PROC初期化
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					引数
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RANKING_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_RANKING_WORK	*p_wk;
	IRC_RANKING_PARAM	*p_rank_param	= p_param;


	//ヒープ作成
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCRANKING, 0x60000 );

	//ワーク作成
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(IRC_RANKING_WORK), HEAPID_IRCRANKING );
	GFL_STD_MemClear( p_wk, sizeof(IRC_RANKING_WORK) );


  if( p_rank_param->p_gamesys )
  { 
    p_wk->p_gamedata  = GAMESYSTEM_GetGameData(p_rank_param->p_gamesys);
  }
  else
  { 
    p_wk->p_gamedata  = GAMEDATA_Create( GFL_HEAPID_APP );

  }

	//共通モジュール作成
  p_wk->p_que   = PRINTSYS_QUE_Create( HEAPID_IRCRANKING );
  p_wk->p_font  = GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
				GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_IRCRANKING ); 
  p_wk->p_msg				= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
				NARC_message_irc_ranking_dat, HEAPID_IRCRANKING );

	//データ作成
	p_wk->p_rank_data	= RANKING_DATA_Create( p_wk->p_gamedata, HEAPID_IRCRANKING );

	//モジュール初期化
	GRAPHIC_Init( &p_wk->grp, p_wk->p_gamedata, HEAPID_IRCRANKING );
  p_wk->p_bmpoam_sys  = BmpOam_Init( HEAPID_IRCRANKING, GRAPHIC_GetUnit(&p_wk->grp) );


	SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_FadeOut );
	UI_Init( &p_wk->ui, HEAPID_IRCRANKING );
	ACLR_Init( &p_wk->aclr );
	SCROLL_Init( &p_wk->scroll,
			GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M),
			GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_FONT_M),
			GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S),
			GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_FONT_S),
			GRAPHIC_GetBgWorkConst(&p_wk->grp),
      GRAPHIC_GetUnit(&p_wk->grp),
      p_wk->p_msg,
			p_wk->p_rank_data,
			HEAPID_IRCRANKING
			);

  p_wk->p_appbar	= APPBAR_Init( APPBAR_OPTION_MASK_RETURN, GRAPHIC_GetUnit( &p_wk->grp ), GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_INFO_S), RANKING_BG_PAL_S_13, 0, APP_COMMON_MAPPING_128K, p_wk->p_font, p_wk->p_que, HEAPID_IRCRANKING );

  //BMPOAMの作成
  { 
    ARCHANDLE * p_handle;

    p_handle  = GFL_ARC_OpenDataHandle( ARCID_FONT, HEAPID_IRCRANKING );
    p_wk->font_plt  = GFL_CLGRP_PLTT_RegisterComp( p_handle, 
          NARC_font_default_nclr, CLSYS_DRAW_MAIN, RANKING_OBJ_PAL_FONT_M*0x20, HEAPID_IRCRANKING );
    GFL_ARC_CloseDataHandle( p_handle );	
  }
  { 
    static const struct
    { 
      s16 x;  //dot
      s16 y;  //dot
      u16 w;  //chara
      u16 h;  //chara
    }sc_bmpoam_pos[MSGOAM_MAX] =
    { 
      { 
        8,39,6,2
      },
      { 
        64,39,5,2
      },
      { 
        112,39,6,2
      },
      { 
        168,39,6,2
      },
    };

    int i;
    GFL_CLWK_DATA cldata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.pos_x  = 8;
    cldata.pos_y  = 41;
    cldata.softpri  = 0;
    cldata.bgpri    = 0;

    for( i = 0; i < MSGOAM_MAX; i++ )
    { 

      cldata.pos_x  = sc_bmpoam_pos[i].x;
      cldata.pos_y  = sc_bmpoam_pos[i].y;

      p_wk->p_msgoam[i]  = RANKING_MSGOAM_Init( &cldata, sc_bmpoam_pos[i].w, sc_bmpoam_pos[i].h,
          p_wk->font_plt, 3, CLSYS_DRAW_MAIN, p_wk->p_bmpoam_sys, p_wk->p_que, HEAPID_IRCRANKING );
      RANKING_MSGOAM_SetStrColor( p_wk->p_msgoam[i], PRINTSYS_LSB_Make( 1, 2, 0) );
      RANKING_MSGOAM_SetStrPos( p_wk->p_msgoam[i], 0, 0, RANKING_MSGOAM_POS_WH_CENTER );
      RANKING_MSGOAM_Print( p_wk->p_msgoam[i], p_wk->p_msg, RANKING_CAPTION_000+i, p_wk->p_font );
    }
  }

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PROC破棄
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					引数
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RANKING_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_RANKING_WORK	*p_wk	= p_work;
  IRC_RANKING_PARAM	*p_rank_param	=p_param;

  //MSGOAMの破棄
  { 
    int i;
    for( i = 0; i < MSGOAM_MAX; i++ )
    { 
      RANKING_MSGOAM_Exit( p_wk->p_msgoam[i] );
    }
  }
  { 
    GFL_CLGRP_PLTT_Release( p_wk->font_plt );
  }

	//モジュール破棄
	APPBAR_Exit( p_wk->p_appbar );
	SCROLL_Exit( &p_wk->scroll );
	ACLR_Exit( &p_wk->aclr );
	UI_Exit( &p_wk->ui );
	SEQ_Exit( &p_wk->seq );

  BmpOam_Exit( p_wk->p_bmpoam_sys );
	GRAPHIC_Exit( &p_wk->grp );

	//データ破棄
	RANKING_DATA_Delete( p_wk->p_rank_data );

  GFL_MSG_Delete( p_wk->p_msg );
  GFL_FONT_Delete( p_wk->p_font );
  PRINTSYS_QUE_Delete( p_wk->p_que );

  if( p_rank_param->p_gamesys == NULL )
  { 
    GAMEDATA_Delete( p_wk->p_gamedata );
  }

	//ワーク破棄
	GFL_PROC_FreeWork( p_proc );

	//ヒープ破棄
	GFL_HEAP_DeleteHeap(HEAPID_IRCRANKING );


	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PROCメイン処理
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					引数
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RANKING_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
  int i;
	IRC_RANKING_WORK	*p_wk	= p_work;

	//シーケンス
	SEQ_Main( &p_wk->seq );

	//描画
	GRAPHIC_Draw( &p_wk->grp );

  PRINTSYS_QUE_Main( p_wk->p_que );

  for( i = 0; i < MSGOAM_MAX; i++ )
  {
    RANKING_MSGOAM_PrintMain( p_wk->p_msgoam[i] );
  }

	//終了
	if( SEQ_IsEnd( &p_wk->seq ) )
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
 *						SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	初期化
 *
 *	@param	SEQ_WORK *p_wk	ワーク
 *	@param	*p_param				パラメータ
 *	@param	seq_function		シーケンス
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param, SEQ_FUNCTION seq_function )
{	
	//クリア
	GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );

	//初期化
	p_wk->p_param	= p_param;

	//セット
	SEQ_SetNext( p_wk, seq_function );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	破棄
 *
 *	@param	SEQ_WORK *p_wk	ワーク
 */
//-----------------------------------------------------------------------------
static void SEQ_Exit( SEQ_WORK *p_wk )
{	
	//クリア
	GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	メイン処理
 *
 *	@param	SEQ_WORK *p_wk ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Main( SEQ_WORK *p_wk )
{	
	if( !p_wk->is_end )
	{	
		p_wk->seq_function( p_wk, &p_wk->seq, p_wk->p_param );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	終了取得
 *
 *	@param	const SEQ_WORK *cp_wk		ワーク
 *
 *	@return	TRUEならば終了	FALSEならば処理中
 */	
//-----------------------------------------------------------------------------
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk )
{	
	return cp_wk->is_end;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	次のシーケンスを設定
 *
 *	@param	SEQ_WORK *p_wk	ワーク
 *	@param	seq_function		シーケンス
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	終了
 *
 *	@param	SEQ_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( SEQ_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}
//=============================================================================
/**
 *					SEQFUNC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	フェードOUT
 *
 *	@param	SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_param				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{	
	enum
	{	
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_END,
	};

	IRC_RANKING_WORK	*p_wk	= p_param;

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
		//新規があったら、移動シーケンスへ
		if( SCROLL_GetNewRankPosY( &p_wk->scroll ) != SCROLL_NEWRANK_NULL )
		{	
			SEQ_SetNext( &p_wk->seq, SEQFUNC_MoveNew );
		}
		else
		{	
			SEQ_SetNext( p_seqwk, SEQFUNC_Main );
		}
		break;

	default:
		GF_ASSERT(0);
	}
	
}
//----------------------------------------------------------------------------
/**
 *	@brief	フェードIN
 *
 *	@param	SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_param				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{
	enum
	{
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};	

	IRC_RANKING_WORK	*p_wk	= p_param;

	switch( *p_seq )
	{	
	case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_EXIT;
		}
		break;

	case SEQ_EXIT:
		SEQ_End( p_seqwk );
		break;

	default:
		GF_ASSERT(0);
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム開始
 *
 *	@param	SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_param				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{	
	IRC_RANKING_WORK	*p_wk	= p_param;
	u32 sync;

  if( !APPBAR_IsStartAnime( p_wk->p_appbar ) )
  {

    //キー入力による移動
    if( UI_IsContKey( &p_wk->ui, &sync ) & PAD_KEY_DOWN )
    {	
      fx32 rate;
      if( ACLR_SCROLL_KEY_MIN < FX32_CONST(sync) )
      {	
        rate	= FX_Div( (FX32_CONST(sync) - ACLR_SCROLL_KEY_MIN), ACLR_SCROLL_KEY_DIF );
        ACLR_SetAclr( &p_wk->aclr, FX_Mul( rate	, ACLR_SCROLL_DISTANCE_MAX ), ACLR_SCROLL_KEY_MOVE_ACLR_SYNC );
      }
      else
      {	
        ACLR_SetAclr( &p_wk->aclr, ACLR_SCROLL_KEY_MOVE_INIT_DISTANCE, ACLR_SCROLL_KEY_MOVE_INIT_SYNC );
      }
    }
    else if( UI_IsContKey( &p_wk->ui, &sync ) & PAD_KEY_UP )
    {	
      fx32 rate;
      if( ACLR_SCROLL_KEY_MIN < FX32_CONST(sync) )
      {	
        rate	= FX_Div( (FX32_CONST(sync) - ACLR_SCROLL_KEY_MIN), ACLR_SCROLL_KEY_DIF );
        ACLR_SetAclr( &p_wk->aclr, -FX_Mul( rate, ACLR_SCROLL_DISTANCE_MAX ), ACLR_SCROLL_KEY_MOVE_ACLR_SYNC );
      }
      else
      {	
        ACLR_SetAclr( &p_wk->aclr, -ACLR_SCROLL_KEY_MOVE_INIT_DISTANCE, ACLR_SCROLL_KEY_MOVE_INIT_SYNC );
      }
    }
    else
    //タッチによる移動
    {	
      GFL_POINT drag_now;
      VecFx32 dist;
      fx32 distance;
      u32		sync;

      static const VecFx32 sc_up_norm	= 
      {
        0, FX32_ONE, 0
      };

      //ドラッグによる移動
      if( UI_GetDrag( &p_wk->ui, NULL, NULL, &drag_now ) )
      {	
        SCROLL_AddPos( &p_wk->scroll, -drag_now.y );
        ACLR_Stop( &p_wk->aclr );
      }
      //はじきにより移動		
      else if( UI_GetFlik( &p_wk->ui, NULL, NULL, &dist, &sync ) )
      {	
        distance	= VEC_DotProduct( &sc_up_norm, &dist );
        ACLR_SetAclr( &p_wk->aclr, distance, sync );
      }
    }
  }

	//加速可能ならば移動
	if( ACLR_IsExist(&p_wk->aclr) )
	{	
		SCROLL_AddPos( &p_wk->scroll, ACLR_GetScrollAdd( &p_wk->aclr ) );
	}



	//戻る
	if( APPBAR_GetTrg( p_wk->p_appbar ) == APPBAR_ICON_RETURN )
	{	
		SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
	}

	//モジュールメイン
	SCROLL_Main( &p_wk->scroll );
	UI_Main( &p_wk->ui );
	ACLR_Main( &p_wk->aclr );
	APPBAR_Main( p_wk->p_appbar );
}
//----------------------------------------------------------------------------
/**
 *	@brief	新規ランクまで移動する処理
 *
 *	@param	SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					パラメータ
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_MoveNew( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{	
	IRC_RANKING_WORK	*p_wk	= p_param;
	s16 new_pos;
	s16 now_pos;
	s16 limit_pos;

	new_pos		= SCROLL_GetNewRankPosY( &p_wk->scroll );
	GF_ASSERT( new_pos != SCROLL_NEWRANK_NULL );
	new_pos		-= ACLR_SCROLL_NEWRANK_OFS;
	new_pos		= MATH_CLAMP( new_pos, 0, new_pos );
	now_pos		= SCROLL_GetPosY( &p_wk->scroll );
	limit_pos	= SCROLL_GetBottomLimitPosY( &p_wk->scroll );

  {
    //新規が一定の場所にくるか、最後までうごいたら
    if( new_pos  > now_pos && now_pos != limit_pos )
    {	
      fx32 rate;
      p_wk->move_new_sync++;
      if( ACLR_SCROLL_KEY_MIN < FX32_CONST(p_wk->move_new_sync) )
      {	
        rate	= FX_Div( (FX32_CONST(p_wk->move_new_sync) - ACLR_SCROLL_KEY_MIN), ACLR_SCROLL_KEY_DIF );
        ACLR_SetAclr( &p_wk->aclr, FX_Mul( rate, ACLR_SCROLL_DISTANCE_MAX ), ACLR_SCROLL_KEY_MOVE_ACLR_SYNC );
      }
      else
      {	
        ACLR_SetAclr( &p_wk->aclr, ACLR_SCROLL_KEY_MOVE_INIT_DISTANCE, ACLR_SCROLL_KEY_MOVE_INIT_SYNC );
      }
    }
    else
    {	
      ACLR_Stop( &p_wk->aclr );
      SEQ_SetNext( &p_wk->seq, SEQFUNC_Main );
    }
  }

	//加速可能ならば移動
	if( ACLR_IsExist(&p_wk->aclr) )
	{	
		SCROLL_AddPos( &p_wk->scroll, ACLR_GetScrollAdd( &p_wk->aclr ) );
	}


	//戻る
	if( APPBAR_GetTrg( p_wk->p_appbar ) == APPBAR_ICON_RETURN )
	{	
		SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
	}
	//モジュールメイン
	SCROLL_Main( &p_wk->scroll );
	ACLR_Main( &p_wk->aclr );
	APPBAR_Main( p_wk->p_appbar );
}
//=============================================================================
/**
 *						BG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG	初期化
 *
 *	@param	GRAPHIC_BG_WORK *p_wk	ワーク
 *	@param	heapID								ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, GAMEDATA  *p_gamedata, HEAPID heapID )
{	
	//クリア
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_BG_WORK) );

	//初期化
	GFL_BG_Init( heapID );
	GFL_BMPWIN_Init( heapID );

	//グラフィックモード設定
	{	
		static const GFL_BG_SYS_HEADER sc_bgsys_header	=
		{	
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
		};
		GFL_BG_SetBGMode( &sc_bgsys_header );
	}

	//BG面設定
	{	
		int i;
		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{	
			GFL_BG_SetBGControl( sc_bgsetup[i].frame, &sc_bgsetup[i].bgcnt_header, sc_bgsetup[i].mode );
			GFL_BG_ClearFrame( sc_bgsetup[i].frame );
			GFL_BG_SetVisible( sc_bgsetup[i].frame, VISIBLE_ON );
		}
	}

	//読み込み
	{	
		p_wk->p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCRANKING_GRAPHIC, heapID );


		GFL_ARCHDL_UTIL_TransVramPalette( p_wk->p_handle, NARC_ircranking_gra_bg_NCLR,
				PALTYPE_MAIN_BG, RANKING_BG_PAL_M_00*0x20, 0, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_wk->p_handle, NARC_ircranking_gra_back_NCGR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_M), 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_wk->p_handle, NARC_ircranking_gra_top_NCGR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M), 0, 0, FALSE, heapID );


		GFL_ARCHDL_UTIL_TransVramScreen( p_wk->p_handle, NARC_ircranking_gra_back_NSCR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_M), 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_wk->p_handle, NARC_ircranking_gra_top_NSCR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M), 0, 0, FALSE, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_wk->p_handle, NARC_ircranking_gra_bg_NCLR,
				PALTYPE_SUB_BG, RANKING_BG_PAL_S_00*0x20, 0, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_wk->p_handle, NARC_ircranking_gra_top_NCGR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_S), 0, 0, FALSE, heapID );


		GFL_ARCHDL_UTIL_TransVramScreen( p_wk->p_handle, NARC_ircranking_gra_top2_NSCR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_S), 0, 0, FALSE, heapID );




		GFL_BG_FillCharacter( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 0, 1,  0 );
		GFL_BG_FillCharacter( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S), 0, 1,  0 );

		p_wk->bar_char_m	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_wk->p_handle, 
				NARC_ircranking_gra_frame_NCGR, GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 0, FALSE, heapID );
		p_wk->bar_char_s	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_wk->p_handle, 
				NARC_ircranking_gra_frame_NCGR, GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S), 0, FALSE, heapID );

		//フォント用パレット
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, RANKING_BG_PAL_M_14*0x20, 0x20, heapID );
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, RANKING_BG_PAL_S_14*0x20, 0x20, heapID );

		GFL_ARC_CloseDataHandle( p_wk->p_handle );

	}
	//BMPWIN
	{
		GFL_MSGDATA	*p_msg;
		GFL_FONT		*p_font;
		STRBUF			*p_strbuf;
		int i;
		u16 x, y;

		//作成
		p_msg				= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
				NARC_message_irc_ranking_dat, heapID );
		p_font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
				GFL_FONT_LOADTYPE_FILE, FALSE, heapID );


		p_wk->p_bmpwin[BMPWIN_ID_TITLE]	= GFL_BMPWIN_Create( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M),
				BMPWIN_TITLE_X,BMPWIN_TITLE_Y,BMPWIN_TITLE_W,BMPWIN_TITLE_H,RANKING_BG_PAL_M_14,
			GFL_BMP_CHRAREA_GET_B );

		for( i = 0; i < BMPWIN_ID_MAX; i++ )
		{	
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0xF );	
		}


		//文字描画
		p_strbuf	= GFL_MSG_CreateString( p_msg, RANKING_TITLE_000 );
    { 
      MYSTATUS  *p_mystatus = NULL;
      { 
        p_mystatus =GAMEDATA_GetMyStatus(p_gamedata);
      }
      PRINT_PrintNameCenter( p_wk->p_bmpwin[BMPWIN_ID_TITLE], p_strbuf, p_mystatus, p_font );
    }
		GFL_STR_DeleteBuffer( p_strbuf );


		//破棄
		GFL_FONT_Delete( p_font );
		GFL_MSG_Delete( p_msg );

		//転送
		for( i = 0; i < BMPWIN_ID_MAX; i++ )
		{	
			GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin[i] );
			GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin[i] );
		}
		GFL_BG_LoadScreenReq(GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M) );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	BG	破棄
 *
 *	@param	GRAPHIC_BG_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk )
{	
	
	//BMPWIN破棄
	{	
		int i;
		for( i = 0; i < BMPWIN_ID_MAX; i++ )
		{	
			GFL_BMPWIN_Delete( p_wk->p_bmpwin[i] );
		}
	}

	//破棄
	{	
		GFL_BG_FreeCharacterArea(GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M),
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->bar_char_m),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->bar_char_m));

		GFL_BG_FreeCharacterArea(GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S),
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->bar_char_s),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->bar_char_s));

		GFL_BG_FillCharacterRelease( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S), 1, 0 );
		GFL_BG_FillCharacterRelease( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 1, 0 );
	}

	//BG面破棄
	{	
		int i;
		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{	
			GFL_BG_FreeBGControl( sc_bgsetup[i].frame );
		}
	}

	//終了
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();

	//クリア
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_BG_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	BG	VBlank関数
 *
 *	@param	GRAPHIC_BG_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk )
{	
	GFL_BG_VBlankFunc();
}
//----------------------------------------------------------------------------
/**
 *	@brief	トランスインフォ取得
 *
 *	@param	const GRAPHIC_BG_WORK *cp_wk	ワーク
 *	@param	is_m													TRUEならば上画面用
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static GFL_ARCUTIL_TRANSINFO GRAPHIC_BG_GetTransInfo( const GRAPHIC_BG_WORK *cp_wk, BOOL is_m )
{	
	if( is_m )
	{	
		return cp_wk->bar_char_m;
	}
	else
	{	
		return cp_wk->bar_char_s;
	}
}
//=============================================================================
/**
 *				OBJ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ描画	初期化
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk			ワーク
 *	@param	GFL_DISP_VRAM* cp_vram_bank	バンクテーブル
 *	@param	heapID											ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID )
{	

  static const GFL_CLSYS_INIT sc_clsys_init = 
  {
    0, 0,
    0, 512,
    4, 124,
    4, 124,
		0,
    48,48,48,48,
    16, 16,
  };

	//クリア
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );

	//システム作成
	GFL_CLACT_SYS_Create( &sc_clsys_init, cp_vram_bank, heapID );
	p_wk->p_clunit	= GFL_CLACT_UNIT_Create( 128, 0, heapID );
	GFL_CLACT_UNIT_SetDefaultRend( p_wk->p_clunit );

	//表示
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

#if 0
	//リソース読み込み
	{	
		ARCHANDLE *p_handle;

		p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCAURA_GRAPHIC, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_PLT]	= GFL_CLGRP_PLTT_Register( p_handle, 
				NARC_ircaura_gra_aura_obj_touch_NCLR, CLSYS_DRAW_MAIN, AURA_OBJ_PAL_M_00*0x20, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_ircaura_gra_aura_obj_touch_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_ircaura_gra_aura_obj_touch_NCER, NARC_ircaura_gra_aura_obj_touch_NANR, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}

	//CLWK作成
	{	
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		p_wk->p_clwk[CLWKID_TOUCH]	= GFL_CLACT_WK_Create( p_wk->p_clunit, 
				p_wk->reg_id[OBJREGID_TOUCH_CHR],
				p_wk->reg_id[OBJREGID_TOUCH_PLT],
				p_wk->reg_id[OBJREGID_TOUCH_CEL],
				&cldata,
				CLSYS_DEFREND_MAIN,
				heapID
				);
	}
#endif


}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ描画	破棄
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk )
{	
#if 0
	//CLWK破棄
	{	
		int i;
		for( i = 0; i < CLWKID_MAX; i++ )
		{	
			GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
		}
	}

	//リソース破棄
	{	
		GFL_CLGRP_PLTT_Release( p_wk->reg_id[OBJREGID_TOUCH_PLT] );
		GFL_CLGRP_CGR_Release( p_wk->reg_id[OBJREGID_TOUCH_CHR] );
		GFL_CLGRP_CELLANIM_Release( p_wk->reg_id[OBJREGID_TOUCH_CEL] );
	}
#endif 

	//システム破棄
	GFL_CLACT_UNIT_Delete( p_wk->p_clunit );
	GFL_CLACT_SYS_Delete();
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ描画	メイン処理
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk )
{	
	GFL_CLACT_SYS_Main();

}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ描画	Vブランク処理
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk )
{	
	GFL_CLACT_SYS_VBlankFunc();
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJ描画	CLWK取得
 *
 *	@param	const GRAPHIC_OBJ_WORK *cp_wk	ワーク
 *	@param	id														CLWKのID
 *
 *	@return	CLWK
 */
//-----------------------------------------------------------------------------
static GFL_CLWK* GRAPHIC_OBJ_GetClwk( const GRAPHIC_OBJ_WORK *cp_wk, CLWKID id )
{	
	GF_ASSERT( id < CLWKID_MAX );
	return cp_wk->p_clwk[id];
}
//----------------------------------------------------------------------------
/**
 *	@brief	CLユニットの取得
 *
 *	@param	const GRAPHIC__OBJ_WORK *cp_wk	ワーク
 *
 *	@return	GFL_CLUNIT
 */
//-----------------------------------------------------------------------------
static GFL_CLUNIT * GRAPHIC_OBJ_GetUnit( const GRAPHIC_OBJ_WORK *cp_wk )
{	
	return cp_wk->p_clunit;
}
//=============================================================================
/**
 *						GRAPHIC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	グラフィック設定
 *
 *	@param	GRAPHIC_WORK *p_wk	ワーク
 *	@param	heapID							ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, GAMEDATA  *p_gamedata, HEAPID heapID )
{	
	static const GFL_DISP_VRAM sc_vramSetTable =
	{
		GX_VRAM_BG_128_A,					// メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,     // メイン2DエンジンのBG拡張パレット
		GX_VRAM_SUB_BG_128_C,				// サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE, // サブ2DエンジンのBG拡張パレット
		GX_VRAM_OBJ_128_B,					// メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
		GX_VRAM_SUB_OBJ_128_D,       // サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,// サブ2DエンジンのOBJ拡張パレット
		GX_VRAM_TEX_NONE,						// テクスチャイメージスロット
		GX_VRAM_TEXPLTT_NONE,				// テクスチャパレットスロット
		GX_OBJVRAMMODE_CHAR_1D_128K,		
		GX_OBJVRAMMODE_CHAR_1D_128K,		
	};

	//クリア
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );

	//レジスタ初期化
	G2_BlendNone();
	G2S_BlendNone();	
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

	//VRAMクリアー
	GFL_DISP_ClearVRAM( 0 );

	//VRAMバンク設定
	GFL_DISP_SetBank( &sc_vramSetTable );

	// ディスプレイON
	GFL_DISP_SetDispSelect( GX_DISP_SELECT_MAIN_SUB );
	GFL_DISP_SetDispOn();

	//	表示
	GFL_DISP_GX_InitVisibleControl();

	//	フォント初期化
	GFL_FONTSYS_Init();

	//	モジュール初期化
	GRAPHIC_BG_Init( &p_wk->bg, p_gamedata, heapID );
	GRAPHIC_OBJ_Init( &p_wk->obj, &sc_vramSetTable, heapID );

	//VBlankTask登録
	p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(Graphic_VBlankTask, p_wk, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief	グラフィック破棄
 *
 *	@param	GRAPHIC_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk )
{	
	//VBLANKTask消去
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

	//モジュール破棄
	GRAPHIC_OBJ_Exit( &p_wk->obj );
	GRAPHIC_BG_Exit( &p_wk->bg );

	//クリア
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	グラフィック描画
 *
 *	@param	GRAPHIC_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk )
{	
	GRAPHIC_OBJ_Main( &p_wk->obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGワーク取得
 *
 *	@param	const GRAPHIC_WORK *cp_wk		ワーク
 *
 *	@return	BGワーク
 */
//-----------------------------------------------------------------------------
static const GRAPHIC_BG_WORK *GRAPHIC_GetBgWorkConst( const GRAPHIC_WORK *cp_wk )
{	
	return &cp_wk->bg;
}
//----------------------------------------------------------------------------
/**
 *	@brief	CLWK取得
 *
 *	@param	const GRAPHIC_WORK *cp_wk	ワーク
 *	@param	id												CLWKID
 *
 *	@return	CLWK
 */
//-----------------------------------------------------------------------------
static GFL_CLWK	* GRAPHIC_GetClwk( const GRAPHIC_WORK *cp_wk, CLWKID id )
{	
	return GRAPHIC_OBJ_GetClwk( &cp_wk->obj, id );
}
//----------------------------------------------------------------------------
/**
 *	@brief	CLUNIT取得
 *
 *	@param	const GRAPHIC_WORK *cp_wk		ワーク
 *
 *	@return	CLUNIT
 */
//-----------------------------------------------------------------------------
static GFL_CLUNIT * GRAPHIC_GetUnit( const GRAPHIC_WORK *cp_wk )
{	
	return GRAPHIC_OBJ_GetUnit( &cp_wk->obj );
}
//----------------------------------------------------------------------------
/**
 *	@brief	グラフィックVBLANK関数
 *
 *	@param	GRAPHIC_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work )
{	
	GRAPHIC_WORK *p_wk	= p_work;
	GRAPHIC_BG_VBlankFunction( &p_wk->bg );
	GRAPHIC_OBJ_VBlankFunction( &p_wk->obj );
}

//=============================================================================
/**
 *	SCROLL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SCROLL	初期化
 *
 *	@param	SCROLL_WORK *p_wk	ワーク
 *	@param	bar_frm						枠のBG面
 *	@param	font_frm					文字のBG面
 *	@param	bar_frm						枠のBG面
 *	@param	font_frm					文字のBG面
 *	@param	*p_bg							BGシステム
 *	@param	cp_data						表示するデータ
 *	@param	heapID						ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void SCROLL_Init( SCROLL_WORK *p_wk, u8 bar_frm_m, u8 font_frm_m, u8 bar_frm_s, u8 font_frm_s, const GRAPHIC_BG_WORK *cp_bg, GFL_CLUNIT *p_clunit, GFL_MSGDATA	*p_msg, const RANKING_DATA* cp_data, HEAPID heapID )
{	
	//クリアー
	GFL_STD_MemClear( p_wk, sizeof(SCROLL_WORK) );
	p_wk->cp_bg				= cp_bg;
	p_wk->bar_frm_m		= bar_frm_m;
	p_wk->font_frm_m	= font_frm_m;
	p_wk->bar_frm_s		= bar_frm_s;
	p_wk->font_frm_s	= font_frm_s;
	p_wk->cp_data			=	cp_data;
  p_wk->data_len		= RANKING_DATA_GetRankNum( cp_data );


	//ランキング用フォント作成
	p_wk->p_font			= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, TRUE, heapID );

	//ランキング用メッセージマネージャ
	p_wk->p_msg				= p_msg;

	//単語セット作成
	p_wk->p_wordset	= WORDSET_Create( heapID );

  //バーの共通素材作成
  { 
    BARBUFF_Init( &p_wk->barbuff, cp_bg, heapID );

    {
      ARCHANDLE* p_handle = GFL_ARC_OpenDataHandle( ARCID_POKEICON, heapID );
      p_wk->icon_plt_idx_m  = GFL_CLGRP_PLTT_RegisterComp( p_handle, POKEICON_GetPalArcIndex(), CLSYS_DRAW_MAIN, RANKING_OBJ_PAL_POKEICON_M*0x20, heapID );
      p_wk->icon_plt_idx_s  = GFL_CLGRP_PLTT_RegisterComp( p_handle, POKEICON_GetPalArcIndex(), CLSYS_DRAW_SUB, RANKING_OBJ_PAL_POKEICON_S*0x20, heapID );
      GFL_ARC_CloseDataHandle( p_handle );
    }
  }

  //バーデータ作成
  { 
    int i;

    for( i = 0; i < RANKING_DATA_GetRankNum( cp_data ); i++ )
    { 
      RANKBAR_Init( &p_wk->rankbar[i], &cp_data->data[i], p_clunit, p_wk->icon_plt_idx_m, p_wk->icon_plt_idx_s,
          p_wk->p_msg, p_wk->p_font, p_wk->p_wordset, &p_wk->barbuff,
          SCROLL_BAR_ICON_INIT_Y_M + i * 24,
          SCROLL_BAR_ICON_INIT_Y_S + i * 24,
          heapID );
    }
  }

  //BMPWIN作成
	p_wk->p_bmpwin[SCROLL_BMPWIN_FONT_M]	= GFL_BMPWIN_Create( 
			font_frm_m,BMPWIN_FONT_X,BMPWIN_FONT_Y,BMPWIN_FONT_W,BMPWIN_FONT_H,
			RANKING_BG_PAL_M_14,GFL_BMP_CHRAREA_GET_B );
	p_wk->p_bmpwin[SCROLL_BMPWIN_FONT_S]	= GFL_BMPWIN_Create( 
			font_frm_s,BMPWIN_FONT_X,BMPWIN_FONT_Y,BMPWIN_FONT_W,BMPWIN_FONT_H,
			RANKING_BG_PAL_S_14,GFL_BMP_CHRAREA_GET_B );

	//張替えのため、スクリーン位置をずらす
	p_wk->rewrite_scr_pos_m	= SCROLL_MARGIN_SIZE_Y_M;
	p_wk->rewrite_scr_pos_s	= SCROLL_MARGIN_SIZE_Y_S;	
	GFL_BG_SetScroll( p_wk->bar_frm_m, GFL_BG_SCROLL_Y_SET, p_wk->rewrite_scr_pos_m );
	GFL_BG_SetScroll( p_wk->font_frm_m, GFL_BG_SCROLL_Y_SET, p_wk->rewrite_scr_pos_m );
	GFL_BG_SetScroll( p_wk->bar_frm_s, GFL_BG_SCROLL_Y_SET, p_wk->rewrite_scr_pos_s );
	GFL_BG_SetScroll( p_wk->font_frm_s, GFL_BG_SCROLL_Y_SET, p_wk->rewrite_scr_pos_s );

	//移動制限
	p_wk->y								= 0;
	p_wk->top_limit_y			= 0;
	p_wk->bottom_limit_y	= (p_wk->data_len * SCROLL_BAR_ALL_HEIGHT)*GFL_BG_1CHRDOTSIZ-192+SCROLL_MARGIN_SIZE_Y_M-SCROLL_WRITE_POS_START_M*GFL_BG_1CHRDOTSIZ-40+16;
	p_wk->bottom_limit_y	= MATH_IMax( 0, p_wk->bottom_limit_y );


  //描画のために一度呼ぶ
	Scroll_Write( p_wk );
	Scroll_WriteVBlank( p_wk );


  //VBlankTask登録
	p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(Scroll_VBlankTask, p_wk, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SCROLL	破棄
 *
 *	@param	SCROLL_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SCROLL_Exit( SCROLL_WORK *p_wk )
{
  int i;

	//VBLANKTask消去
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

  //BMPWIN破棄
	for( i = 0; i < SCROLL_BMPWIN_MAX; i++ )
	{	
		GFL_BMPWIN_Delete( p_wk->p_bmpwin[i] );
	}

  //バーデータ破棄
  { 
    int i;

    for( i = 0; i < RANKING_DATA_GetRankNum( p_wk->cp_data ); i++ )
    { 
      RANKBAR_Exit( &p_wk->rankbar[i] );
    }
  }

  //バー共通素材破棄
  { 
    GFL_CLGRP_PLTT_Release( p_wk->icon_plt_idx_m );
    GFL_CLGRP_PLTT_Release( p_wk->icon_plt_idx_s );

    BARBUFF_Exit( &p_wk->barbuff );
  }

  //単語セット破棄
	WORDSET_Delete( p_wk->p_wordset );

	//フォント破棄
	GFL_FONT_Delete( p_wk->p_font );

	//クリアー
	GFL_STD_MemClear( p_wk, sizeof(SCROLL_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	SCROLL	メイン処理
 *
 *	@param	SCROLL_WORK *p_wk ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SCROLL_Main( SCROLL_WORK *p_wk )
{	
	int i;

	//移動時のみ更新
	if( p_wk->is_move_req )
	{	
		p_wk->is_move_req	= FALSE;
		//張替えるチェック
		if( p_wk->y	% SCROLL_REWITE_DISTANCE == 0 )
		{				

			if( p_wk->top_bar != p_wk->y / SCROLL_REWITE_DISTANCE )
			{	
				PMSND_PlaySE( IRCRANK_SE_SLIDE );
			}

			//描画先頭バーを計算
			p_wk->top_bar						= p_wk->y / SCROLL_REWITE_DISTANCE;
			p_wk->rewrite_scr_pos_m	= SCROLL_MARGIN_SIZE_Y_M;
			p_wk->rewrite_scr_pos_s	= SCROLL_MARGIN_SIZE_Y_S;
						
	DEBUG_TICK_DRAW_Start;
			Scroll_Write( p_wk );
	DEBUG_TICK_DRAW_Print;

			p_wk->is_update	= TRUE;	//VBlankの描画をUPDATE
		}
		else
		{	
			p_wk->is_move_update	= TRUE;
		}

	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	SCROLL	位置
 *
 *	@param	SCROLL_WORK *p_wk	ワーク
 *	@param	y									Y
 *
 */
//-----------------------------------------------------------------------------
static void SCROLL_AddPos( SCROLL_WORK *p_wk, s16 y )
{	
  s16 icon_add  = 0;

	//移動制限
	if( p_wk->top_limit_y > p_wk->y + y )
	{	
    icon_add  = p_wk->top_limit_y - (p_wk->y);
		p_wk->y	= p_wk->top_limit_y;
		p_wk->add_y		=	 0;
	}
	else if( p_wk->bottom_limit_y < p_wk->y + y )
	{	
    icon_add  = (p_wk->y) - p_wk->bottom_limit_y;
		p_wk->y	= p_wk->bottom_limit_y;
		p_wk->add_y		=	 0;
	}
	else
	{	
		//更新タイミングを越えていたら修正
		//NAGI_Printf( "left %d right %d y%d\n", (p_wk->y / SCROLL_REWITE_DISTANCE),((p_wk->y+y)	/ SCROLL_REWITE_DISTANCE), y  );
		if( (p_wk->y / SCROLL_REWITE_DISTANCE) != ((p_wk->y+y)	/ SCROLL_REWITE_DISTANCE) &&
				p_wk->y%SCROLL_REWITE_DISTANCE!=0)
		{	
			if( 0 <= y )
			{	
				y	= y - ((p_wk->y+y) % SCROLL_REWITE_DISTANCE);
				//NAGI_Printf( "- Y POS %d add%d ofs%d\n", p_wk->y, y, p_wk->y%SCROLL_REWITE_DISTANCE );
			}
			else
			{	
				//NAGI_Printf( "te %d  %d\n", y, SCROLL_REWITE_DISTANCE-((p_wk->y+y) % SCROLL_REWITE_DISTANCE) );
				y	= y + (SCROLL_REWITE_DISTANCE-((p_wk->y+y) % SCROLL_REWITE_DISTANCE));
				//NAGI_Printf( "- Y POS %d add%d ofs%d\n", p_wk->y, y, p_wk->y%SCROLL_REWITE_DISTANCE );
			}
		}
	
		//加算
		p_wk->y				+= y;
		p_wk->add_y		=	 y;
    icon_add      = y;
		//上に移動
	//	NAGI_Printf( "Ypos %d top%d \n", p_wk->y, p_wk->y / SCROLL_REWITE_DISTANCE );
	}

  {
    int i;
    for( i = 0; i < p_wk->data_len; i++ )
    {	
      RANKBAR_Move( &p_wk->rankbar[ i ], p_wk->y );
    }
  }
		
	p_wk->is_move_req	= TRUE;

}

//----------------------------------------------------------------------------
/**
 *	@brief	スクロール位置を取得
 *
 *	@param	const SCROLL_WORK *cp_wk	ワーク
 *
 *	@return	スクロール位置
 */
//-----------------------------------------------------------------------------
static s16 SCROLL_GetPosY( const SCROLL_WORK *cp_wk )
{	
	return cp_wk->y;
}
//----------------------------------------------------------------------------
/**
 *	@brief	新規ランクインの座標を取得
 *
 *	@param	const SCROLL_WORK *cp_wk	ワーク 
 *
 *	@return	新規ランクインの座標
 */
//-----------------------------------------------------------------------------
static s16 SCROLL_GetNewRankPosY( const SCROLL_WORK *cp_wk )
{	
	int i;
	BOOL is_exist	= FALSE;

	for( i = 0; i < cp_wk->data_len; i++ )
	{	
		if( cp_wk->rankbar[i].cp_data->new )
		{	
			is_exist	= TRUE;
			break;
		}
	}
	if( !is_exist )
	{	
		return SCROLL_NEWRANK_NULL;
	}

	return i * SCROLL_BAR_HEIGHT * GFL_BG_1CHRDOTSIZ;

  return 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	移動下限値を取得
 *
 *	@param	const SCROLL_WORK *cp_wk ワーク
 *
 *	@return	移動下限値
 */
//-----------------------------------------------------------------------------
static s16 SCROLL_GetBottomLimitPosY( const SCROLL_WORK *cp_wk )
{	
	return cp_wk->bottom_limit_y;
}
//----------------------------------------------------------------------------
/**
 *	@brief	書き込みし転送する処理
 *
 *	@param	SCROLL_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void Scroll_Write( SCROLL_WORK *p_wk )
{
	int i;
	int print_y;

	//一端クリア
	GFL_BG_FillScreen( p_wk->bar_frm_m, 0, 0, 0, 32, 24, 0 );	
	GFL_BG_FillScreen( p_wk->bar_frm_s, 0, 0, 0, 32, 24, 0 );	
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[SCROLL_BMPWIN_FONT_M]), 0 );
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[SCROLL_BMPWIN_FONT_S]), 0 );

	//上画面書き込み
	for( i = SCROLL_WRITE_BAR_START_EX_M+p_wk->top_bar; i < SCROLL_WRITE_BAR_END_M+p_wk->top_bar; i++ )
	{	
		if( 0 <= i && i < p_wk->data_len )
		{
			//SCROLL_WRITE_BAR_START_EX_Mが-だと、基準値の前にプリントする
			print_y	= (i-(p_wk->top_bar+SCROLL_WRITE_BAR_START_M))*
				SCROLL_BAR_HEIGHT+SCROLL_WRITE_POS_START_M;

      if( 0 <= print_y && print_y <= 24 )
      { 
        RANKBAR_Trans( &p_wk->rankbar[ i ], p_wk->p_bmpwin[ SCROLL_BMPWIN_FONT_M ],
          p_wk->bar_frm_m, print_y );
      }
		}
	}

	//下画面書き込み
	for( i = SCROLL_WRITE_BAR_START_EX_S+p_wk->top_bar; i < SCROLL_WRITE_BAR_END_S+p_wk->top_bar; i++ )
	{	
		if( 0 <= i &&i < p_wk->data_len )
		{
			print_y	= (i-(p_wk->top_bar+SCROLL_WRITE_BAR_START_EX_S)+SCROLL_WRITE_POS_START_S)*
				SCROLL_BAR_HEIGHT+SCROLL_WRITE_POS_START_S;

      if( 0 <= print_y && print_y <= 24 )
      { 
        RANKBAR_Trans( &p_wk->rankbar[ i ], p_wk->p_bmpwin[ SCROLL_BMPWIN_FONT_S ],
            p_wk->bar_frm_s, print_y );
      }
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	書き込みVBlank版
 *
 *	@param	SCROLL_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void Scroll_WriteVBlank( SCROLL_WORK *p_wk )
{	
	int i;

	//転送処理
	for( i = 0; i < SCROLL_BMPWIN_MAX; i++ )
	{	
		GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin[i] );
		GFL_BMPWIN_MakeScreen( p_wk->p_bmpwin[i] );
	}

	GFL_BG_LoadScreenReq( p_wk->bar_frm_m );
	GFL_BG_LoadScreenReq( p_wk->bar_frm_s );
	GFL_BG_LoadScreenReq( p_wk->font_frm_m );
	GFL_BG_LoadScreenReq( p_wk->font_frm_s );
}

//----------------------------------------------------------------------------
/**
 *	@brief  VBLANKタスク
 *
 *	@param	GFL_TCB *p_tcb  タスク
 *	@param	*p_work         ワーク
 */
//-----------------------------------------------------------------------------
static void Scroll_VBlankTask( GFL_TCB *p_tcb, void *p_wk_adrs )
{ 
	SCROLL_WORK *p_wk	= p_wk_adrs;
		
	//転送
	if( p_wk->is_update )
	{		
	DEBUG_TICK_DRAW_Start;
		Scroll_WriteVBlank( p_wk );
	DEBUG_TICK_DRAW_Print;

		p_wk->is_update	= FALSE;

		//転送後の張替え処理のため移動する
		p_wk->is_move_update	= TRUE;
	}

	//移動
	if( p_wk->is_move_update )
	{	
		s16 move_ofs;
		s16 pos_m;
		s16 pos_s;

		//移動オフセット
		move_ofs	= p_wk->y % SCROLL_REWITE_DISTANCE;

		//もし前回と同じ方向だったらが0ならば通常移動
		//0のみ例外移動
		if( move_ofs == 0 )
		{	
				pos_m	= p_wk->rewrite_scr_pos_m + move_ofs;
				pos_s	= p_wk->rewrite_scr_pos_s + move_ofs;
				//NAGI_Printf(  "ゼロ移動量 %d\n", move_ofs );
				p_wk->pre_dir	= p_wk->add_y;
				p_wk->is_zero_next	= TRUE;	
		}
		else  if( (0 <= p_wk->add_y && 0 <= p_wk->pre_dir)
			|| (p_wk->add_y < 0 && p_wk->pre_dir < 0) || p_wk->is_zero_next )
		{	
			p_wk->is_zero_next	= FALSE;

			//移動処理
			if( 0<= p_wk->add_y )
			{	
				pos_m	= p_wk->rewrite_scr_pos_m + move_ofs;
				pos_s	= p_wk->rewrite_scr_pos_s + move_ofs;
				p_wk->pre_dir	= 1;
				//NAGI_Printf(  "移動量 %d\n", move_ofs );
			}
			else
			{	
				pos_m	= p_wk->rewrite_scr_pos_m - ( SCROLL_REWITE_DISTANCE - move_ofs - 1 );
				pos_s	= p_wk->rewrite_scr_pos_s - ( SCROLL_REWITE_DISTANCE - move_ofs - 1 );
				p_wk->pre_dir	= -1;
				//NAGI_Printf(  "移動量 %d 補正%d\n", move_ofs, ( SCROLL_REWITE_DISTANCE - move_ofs - 1 ));
			}
		}
		else
		{	
			//違う場合は、張替え位置まで、前回と同じ移動方法
			if( 0<= p_wk->pre_dir )
			{	
				pos_m	= p_wk->rewrite_scr_pos_m + move_ofs;
				pos_s	= p_wk->rewrite_scr_pos_s + move_ofs;
				//NAGI_Printf(  "pre移動量 %d\n", move_ofs );
				p_wk->pre_dir	= 1;
			}
			else
			{	
				pos_m	= p_wk->rewrite_scr_pos_m - ( SCROLL_REWITE_DISTANCE - move_ofs - 1 );
				pos_s	= p_wk->rewrite_scr_pos_s - ( SCROLL_REWITE_DISTANCE - move_ofs - 1 );
				p_wk->pre_dir	= -1;
				//NAGI_Printf(  "pre移動量 %d 補正%d\n", move_ofs, ( SCROLL_REWITE_DISTANCE - move_ofs - 1 ));
			}

		}

		GFL_BG_SetScroll( p_wk->bar_frm_m, GFL_BG_SCROLL_Y_SET, pos_m );
		GFL_BG_SetScroll( p_wk->font_frm_m, GFL_BG_SCROLL_Y_SET, pos_m );
		GFL_BG_SetScroll( p_wk->bar_frm_s, GFL_BG_SCROLL_Y_SET, pos_s );
		GFL_BG_SetScroll( p_wk->font_frm_s, GFL_BG_SCROLL_Y_SET, pos_s );

		p_wk->is_move_update	= FALSE;
	}
}
//=============================================================================
/**
 *	RANKBAR_WORK
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  バー１つ分作成
 *
 *	@param	RANKBAR_WORK *p_wk        ワーク
 *	@param	RANKING_ONE_DATA *cp_data データ
 *	@param  GFL_CLUNIT                ユニット
 *	@param  plt_idx                   ポケアイコンパレット登録インデックス
 *	@param	heapID                    ヒープID
 */
//-----------------------------------------------------------------------------
static void RANKBAR_Init( RANKBAR_WORK *p_wk, const RANKING_ONE_DATA *cp_data, GFL_CLUNIT *p_clunit, u32 icon_plt_idx_m, u32 icon_plt_idx_s, GFL_MSGDATA	*p_msg, GFL_FONT *p_font, WORDSET *p_wordset, BAR_BUFF *p_bar, s16 icon_pos_m, s16 icon_pos_s, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(RANKBAR_WORK) );
  p_wk->cp_data = cp_data;
  p_wk->p_buff  = p_bar;

  //BMP作成
  p_wk->p_bmp = GFL_BMP_Create( SCROLL_BAR_ALL_WIDTH, SCROLL_BAR_ALL_HEIGHT, GFL_BMP_16_COLOR, heapID );

  
  //書き込み
  { 
    STRBUF* p_rank_buf	= GFL_STR_CreateBuffer( 32, heapID );
    STRBUF* p_score_buf	= GFL_STR_CreateBuffer( 32, heapID );
    STRBUF* p_count_buf	= GFL_STR_CreateBuffer( 32, heapID );
    STRBUF* p_strbuf		= GFL_STR_CreateBuffer( 32, heapID );

    GFL_MSG_GetString( p_msg, RANKING_RANK_000, p_rank_buf );
    GFL_MSG_GetString( p_msg, RANKING_SCORE_000, p_score_buf );
    GFL_MSG_GetString( p_msg, RANKING_COUNT_000, p_count_buf );
    WORDSET_RegisterNumber( p_wordset, 0, cp_data->rank, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
    WORDSET_RegisterNumber( p_wordset, 2, cp_data->score, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );
    WORDSET_RegisterNumber( p_wordset, 3, cp_data->play_cnt, 3, STR_NUM_DISP_LEFT, STR_NUM_CODE_HANKAKU );

    //文字描画
    //ランク
    WORDSET_ExpandStr( p_wordset, p_strbuf, p_rank_buf );
    {
      //わる２をしているのは、半角の文字サイズを認識できていないので
      const int w = PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 )/2;
      NAGI_Printf( "rank w=%d\n", w );
      PRINTSYS_Print( p_wk->p_bmp, (SCROLL_BAR_FONT_RANK_X+2)*GFL_BG_1CHRDOTSIZ-2- w, SCROLL_FONT_Y_OFS, p_strbuf, p_font );
    }
    //名前
    PRINTSYS_Print( p_wk->p_bmp, SCROLL_BAR_FONT_PLAYER_X*GFL_BG_1CHRDOTSIZ+4, SCROLL_FONT_Y_OFS, cp_data->p_name, p_font );
    //点数
    WORDSET_ExpandStr( p_wordset, p_strbuf, p_score_buf );
    {
      const int w = PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 )/2;
      NAGI_Printf( "score w=%d\n", w );
      PRINTSYS_Print( p_wk->p_bmp, (SCROLL_BAR_FONT_SCORE_X+3)*GFL_BG_1CHRDOTSIZ-2- w, SCROLL_FONT_Y_OFS, p_strbuf, p_font );
    }
    //回数
    WORDSET_ExpandStr( p_wordset, p_strbuf, p_count_buf );
    {
      const int w = PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 )/2;
      NAGI_Printf( "count w=%d\n", w );
      PRINTSYS_Print( p_wk->p_bmp, (SCROLL_BAR_FONT_COUNT_X+3)*GFL_BG_1CHRDOTSIZ-6-w, SCROLL_FONT_Y_OFS, p_strbuf, p_font );
    }

    GFL_STR_DeleteBuffer( p_strbuf );
    GFL_STR_DeleteBuffer( p_count_buf );
    GFL_STR_DeleteBuffer( p_score_buf );
    GFL_STR_DeleteBuffer( p_rank_buf );
  }

  //ポケアイコンリソース読み込み
  { 
    ARCDATID  cgx_dat = POKEICON_GetCgxArcIndexByMonsNumber( cp_data->mons_no, cp_data->form_no, cp_data->sex, cp_data->egg );
    ARCDATID  cel_dat = POKEICON_GetCellArcIndex();
    ARCDATID  anm_dat = POKEICON_GetAnmArcIndex();

    ARCHANDLE* p_handle  = GFL_ARC_OpenDataHandle( ARCID_POKEICON, heapID );
    p_wk->chr_idx = GFL_CLGRP_CGR_Register( p_handle, cgx_dat, FALSE, CLSYS_DRAW_MAX, heapID );
    p_wk->cel_idx = GFL_CLGRP_CELLANIM_Register( p_handle, cel_dat, anm_dat, heapID );

    GFL_ARC_CloseDataHandle( p_handle );
  }

  //OBJ作成
  { 
    u32 plt_idx;
    int i;
    GFL_CLWK_DATA cldata;
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
    cldata.anmseq = POKEICON_ANM_HPMAX;
    cldata.bgpri  = 1;

    for( i = 0; i < CLSYS_DEFREND_NUM; i++  )
    {
      cldata.pos_x  = SCROLL_BAR_ICON_X;
      if( i == CLSYS_DRAW_MAIN )
      { 
        cldata.pos_y  = icon_pos_m;
        plt_idx = icon_plt_idx_m;
      }
      else
      { 
        cldata.pos_y  = icon_pos_s;
        plt_idx = icon_plt_idx_s;
      }
      p_wk->icon_init_pos[i]  = cldata.pos_y;
      p_wk->p_icon[i]  = GFL_CLACT_WK_Create( p_clunit,
          p_wk->chr_idx,
          plt_idx,
          p_wk->cel_idx,
          &cldata,
          i,
          heapID
          );

      GFL_CLACT_WK_SetPlttOffs( p_wk->p_icon[i], 
          POKEICON_GetPalNum( cp_data->mons_no, cp_data->form_no, cp_data->sex, cp_data->egg )
          ,CLWK_PLTTOFFS_MODE_OAM_COLOR );
      if( -16 < cldata.pos_y && cldata.pos_y < 192 + 16 )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[i], TRUE );
      }
      else
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[i], FALSE );
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  バー１つ分破棄
 *
 *	@param	RANKBAR_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void RANKBAR_Exit( RANKBAR_WORK *p_wk )
{ 
  //OBJ破棄
  { 
    int i;
    for( i = 0; i < CLSYS_DEFREND_NUM; i++  )
    {
      GFL_CLACT_WK_Remove( p_wk->p_icon[i] );
    }
  }
  
  //リソース破棄
  { 
    GFL_CLGRP_CGR_Release( p_wk->chr_idx );
    GFL_CLGRP_CELLANIM_Release( p_wk->cel_idx );
  }

  GFL_BMP_Delete( p_wk->p_bmp );


  GFL_STD_MemClear( p_wk, sizeof(RANKBAR_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  転送  
 *
 *	@param	RANKBAR_WORK *p_wk  ワーク
 *	@param	y                   Y位置
 */
//-----------------------------------------------------------------------------
static void RANKBAR_Trans( RANKBAR_WORK *p_wk, GFL_BMPWIN *p_font_bmpwin, u8 bar_frm, u16 y )
{ 
  GF_ASSERT( p_font_bmpwin );

  BMP_Copy( p_wk->p_bmp, GFL_BMPWIN_GetBmp( p_font_bmpwin ),
      0, 0, 0, y, SCROLL_BAR_ALL_WIDTH, SCROLL_BAR_ALL_HEIGHT );

  BARBUFF_Trans( p_wk->p_buff, bar_frm, y, p_wk->cp_data->plt );
}

//----------------------------------------------------------------------------
/**
 *	@brief  移動
 *
 *	@param	RANKBAR_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void RANKBAR_Move( RANKBAR_WORK *p_wk, s16 bg_y )
{ 
  int i;
  GFL_CLACTPOS pos;
  int bg_pos[2];

  bg_pos[0] = -bg_y;//- (bg_y + SCROLL_MARGIN_SIZE_Y_M);
  bg_pos[1] = -bg_y;//- (bg_y + SCROLL_MARGIN_SIZE_Y_S);
  for( i = 0; i < CLSYS_DRAW_MAX; i++ )
  { 
    GFL_CLACT_WK_GetPos( p_wk->p_icon[i], &pos, i );
    pos.y = bg_pos[i] + p_wk->icon_init_pos[ i ];
    GFL_CLACT_WK_SetPos( p_wk->p_icon[i], &pos, i );
    if( -16 < pos.y && pos.y < 192 + 16 )
    { 
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[i], TRUE );
    }
    else
    { 
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[i], FALSE );
    }
  }
}
//=============================================================================
/**
 *  BARBUFF
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  バー書き込み用ワーク  初期化
 *
 *	@param	BAR_BUFF *p_wk  ワーク
 *	@param	heapID          ヒープID
 */
//-----------------------------------------------------------------------------
static void BARBUFF_Init( BAR_BUFF *p_wk, const GRAPHIC_BG_WORK *cp_bg, HEAPID heapID )
{ 
  GFL_STD_MemClear( p_wk, sizeof(BAR_BUFF) );

  //bar用スクリーンを呼ぶ
  { 
    int i, j;
    void *p_src_arc;
    NNSG2dScreenData* p_scr_data;
    u16 *p_scr;

    u16 start_char_num_m	= GRAPHIC_BG_GetTransInfo( cp_bg, TRUE );
		u16 start_char_num_s	= GRAPHIC_BG_GetTransInfo( cp_bg, FALSE );

    //読み込み
    p_src_arc = GFL_ARC_UTIL_LoadScreen(ARCID_IRCRANKING_GRAPHIC, NARC_ircranking_gra_frame_NSCR, FALSE, &p_scr_data, heapID );

    p_scr = (u16*)p_scr_data->rawData;

    for( i = 0; i < SCROLL_BAR_ALL_HEIGHT; i++ )
    {
      for( j = 0; j < SCROLL_BAR_ALL_WIDTH; j++ )
      { 
        p_wk->rank_screen_main[i*SCROLL_BAR_ALL_WIDTH+j] = 
          p_scr[ i * SCROLL_BAR_ALL_WIDTH + j ] + start_char_num_m;
        p_wk->rank_screen_sub[i*SCROLL_BAR_ALL_WIDTH+j] = 
          p_scr[ i * SCROLL_BAR_ALL_WIDTH + j ] + start_char_num_s;
      }
    }

    GFL_HEAP_FreeMemory( p_src_arc );
  }


}
//----------------------------------------------------------------------------
/**
 *	@brief  バー書き込み用ワーク  破棄
 *
 *	@param	BAR_BUFF *p_wk  ワーク
 */
//-----------------------------------------------------------------------------
static void BARBUFF_Exit( BAR_BUFF *p_wk )
{ 

  GFL_STD_MemClear( p_wk, sizeof(BAR_BUFF) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  バー書き込み用ワーク  転送
 *
 *	@param	BAR_BUFF *p_wk  ワーク
 *	@param  frm             BG面
 *	@param	y               転送位置
 */
//-----------------------------------------------------------------------------
static void BARBUFF_Trans( BAR_BUFF *p_wk, u8 frm, u16 y, u8 plt )
{ 
  u16 *p_buff;

  if( frm < GFL_BG_FRAME0_S )
  { 
    p_buff  = p_wk->rank_screen_main;
  }
  else
  { 
    p_buff  = p_wk->rank_screen_sub;
  }

  GFL_BG_WriteScreen( frm, p_buff, 0, y, SCROLL_BAR_ALL_WIDTH, SCROLL_BAR_ALL_HEIGHT );
  GFL_BG_ChangeScreenPalette( frm, 0, y, SCROLL_BAR_ALL_WIDTH, SCROLL_BAR_ALL_HEIGHT, plt );
}
//----------------------------------------------------------------------------
/**
 *	@brief  バー書き込み用ワーク  クリアー
 *
 *	@param	BAR_BUFF *p_wk  ワーク
 *	@arapm  frm             BG面
 *	@param	y               クリア位置
 */
//-----------------------------------------------------------------------------
static void BARBUFF_Clear( BAR_BUFF *p_wk, u8 frm, u16 y )
{ 
  GFL_BG_FillScreen( frm, 0, 0, y, SCROLL_BAR_ALL_WIDTH, SCROLL_BAR_ALL_HEIGHT, GFL_BG_SCRWRT_PALNL );
}
//=============================================================================
/**
 *	RANKING_DATA
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ランキングデータ作成
 *
 *	@param	u16 data_len	データ長
 *	@param	heapID				ヒープID
 *
 *	@return	データ
 */
//-----------------------------------------------------------------------------
static RANKING_DATA	*RANKING_DATA_Create( GAMEDATA *p_gamedata, HEAPID heapID )
{	
	RANKING_DATA	          *p_wk;
  SAVE_CONTROL_WORK       *p_sv_ctrl;
  IRC_COMPATIBLE_SAVEDATA * p_sv;
  u16 rank_max;
  u32 size;

#ifdef PM_DEBUG
  if( p_gamedata == NULL )
  { 
    p_sv_ctrl = SaveControl_GetPointer();
  }
  else
#endif
  { 
    p_sv_ctrl = GAMEDATA_GetSaveControlWork( p_gamedata );
  }

  p_sv	= IRC_COMPATIBLE_SV_GetSavedata( p_sv_ctrl );
  rank_max	= IRC_COMPATIBLE_SV_GetRankNum( p_sv );


  size  = sizeof( RANKING_DATA ) + sizeof(RANKING_ONE_DATA) * rank_max;

	//確保
	p_wk	= GFL_HEAP_AllocMemory( heapID, size );
	GFL_STD_MemClear( p_wk, size );
  p_wk->rank_num  = rank_max;

	//データ読み込み
  if( p_wk->rank_num > 0 )
	{	

		u8	new_rank;
		int i;
		u8	pre_score	= 128;	//100以上のあたい
		u8	pre_rank	= 0;
    u8  sex;
    RANKING_ONE_DATA  *p_data;

		new_rank	= IRC_COMPATIBLE_SV_GetNewRank( p_sv );
		for( i = 0; i < p_wk->rank_num; i++ )
		{	
      p_data  = &p_wk->data[i];

			//名前
			p_data->p_name	= GFL_STR_CreateBuffer( IRC_COMPATIBLE_SV_DATA_NAME_LEN, heapID );
			GFL_STR_SetStringCode( p_data->p_name, IRC_COMPATIBLE_SV_GetPlayerName( p_sv, i ) );	
			//スコア
			p_data->score	= IRC_COMPATIBLE_SV_GetScore( p_sv, i );
				
			//回数
			p_data->play_cnt	= IRC_COMPATIBLE_SV_GetPlayCount( p_sv, i );
			//ランク
			//点数が下ならそのときのランク
			//同じならば同ランク
			if( p_data->score < pre_score )
			{	
				p_data->rank	= i+1;
			}
			else if( p_data->score == pre_score )
			{	
				p_data->rank	= pre_rank;
			}
			else
			{	
				GF_ASSERT(0);
			}
			//パレット
      sex = IRC_COMPATIBLE_SV_GetSex( p_sv, i );
			if( new_rank == i )
			{	
				p_data->plt		= 5 + !sex * 5;
        p_data->new  = TRUE;
			}else if( p_data->rank == 1 )
			{	
				p_data->plt		= 2 + !sex * 5;
			}
			else if( p_data->rank == 2 )
			{	
				p_data->plt		= 3 + !sex * 5;
			}
			else if( p_data->rank == 3 )
			{	
				p_data->plt		= 4 + !sex * 5;
			}
			else
			{	
				p_data->plt		= 1 + !sex * 5;
			}

      //ポケアイコン
      IRC_COMPATIBLE_SV_GetPokeData( p_sv, i, 
          &p_data->mons_no, &p_data->form_no, &p_data->sex, &p_data->egg );

			//前の人のスコア保存
			pre_score	= p_data->score;
			pre_rank	= p_data->rank;
		}
	}

	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ランキングデータ破棄
 *
 *	@param	RANKING_DATA	*p_data		データ
 *
 */
//-----------------------------------------------------------------------------
static void RANKING_DATA_Delete( RANKING_DATA	*p_wk )
{
  u32 rank_max;
  int i;

  RANKING_ONE_DATA  *p_data;

  if( p_wk->rank_num > 0 )
  { 
    for( i = 0; i < p_wk->rank_num; i++ )
    {	
      p_data  = &p_wk->data[i];
      if( p_data->p_name )
      {	
        GFL_STR_DeleteBuffer( p_data->p_name );
      }
    }
  }

  GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ランクの数取得
 *
 *	@param	const RANKING_DATA	*cp_wk  ワーク
 *
 *	@return ランクの数を取得
 */
//-----------------------------------------------------------------------------
static u32 RANKING_DATA_GetRankNum( const RANKING_DATA	*cp_wk )
{ 
  return cp_wk->rank_num;
}
//=============================================================================
/**
 *	UI
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	UI初期化
 *
 *	@param	UI_WORK *p_wk	ワーク
 *	@param	heapID				ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void UI_Init( UI_WORK *p_wk, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(UI_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	UI破棄
 *
 *	@param	UI_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void UI_Exit( UI_WORK *p_wk )
{	
	GFL_STD_MemClear( p_wk, sizeof(UI_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	UIメイン処理
 *
 *	@param	UI_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void UI_Main( UI_WORK *p_wk )
{	
	u32 x, y;
	//タッチの処理
	if( GFL_UI_TP_GetPointTrg( &x, &y ) )
	{	
		p_wk->start.x		= x;
		p_wk->start.y		= y;
		p_wk->end.x		= x;
		p_wk->end.y		= y;
    p_wk->drag_start.x  = x;
    p_wk->drag_start.y  = y;
    p_wk->drag_pos.x  = 0;
    p_wk->drag_pos.y  = 0;

		p_wk->flik_sync	= 0;
	}
	else if(GFL_UI_TP_GetPointCont( &x, &y ) )
	{	
    if( MATH_IAbs( y - p_wk->start.y ) >= DRAG_MOVE_INIT_DISTANCE )
    {
      p_wk->drag_pos.x  = x - p_wk->drag_start.x;
      p_wk->drag_pos.y  = y - p_wk->drag_start.y;
      p_wk->drag_start.x  = x;
      p_wk->drag_start.y  = y;
    }

		p_wk->end.x		= x;
		p_wk->end.y		= y;

		p_wk->flik_sync++;
		p_wk->release_sync	= 0;
	}
	else
	{	
		p_wk->release_sync++;
	}

	//キーの処理
	if( GFL_UI_KEY_GetTrg() & (PAD_KEY_UP|PAD_KEY_DOWN) )
	{
		p_wk->key_cont_sync	= 0;
	}
	else if( GFL_UI_KEY_GetCont() & (PAD_KEY_UP|PAD_KEY_DOWN) )
	{	
		p_wk->key_cont_sync++;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	UI	ドラッグ情報取得
 *
 *	@param	const UI_WORK *p_wk	ワーク
 *	@param	*p_start						開始点
 *	@param	*p_end							終了点
 *	@param	*p_vec							ベクトル
 *
 */
//-----------------------------------------------------------------------------
static BOOL UI_GetDrag( UI_WORK *p_wk, GFL_POINT *p_start, GFL_POINT *p_end, GFL_POINT *p_drag_pos )
{	
	if( GFL_UI_TP_GetCont() )
	{	
		if( p_start )
		{	
			*p_start	= p_wk->start;
		}
		if( p_end )
		{	
			*p_end	  = p_wk->end;
		}
		if( p_drag_pos )
		{	
      *p_drag_pos = p_wk->drag_pos;
		}
		return TRUE;
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	UI	はじき情報取得
 *
 *	@param	const UI_WORK *p_wk	ワーク
 *	@param	*p_start	開始点
 *	@param	*p_end		終了点
 *	@param	*p_vec		ベクトル
 *	@param	*p_sync		はじきにかかったシンク
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static BOOL UI_GetFlik( UI_WORK *p_wk, GFL_POINT *p_start, GFL_POINT *p_end, VecFx32 *p_vec, u32 *p_sync )
{	
	if( !GFL_UI_KEY_GetCont() && p_wk->release_sync < UI_FLIK_RELEASE_SYNC )
	{	
		//1回しか取得できないように
		p_wk->release_sync	= UI_FLIK_RELEASE_SYNC;
		if( p_start )
		{	
			*p_start	= p_wk->start;
		}
		if( p_end )
		{	
			*p_end	= p_wk->end;
		}
		if( p_vec )
		{	
			VecFx32	s, e;
			VEC_Set( &s, FX32_CONST( p_wk->start.x ), FX32_CONST( p_wk->start.y ), 0 );
			VEC_Set( &e, FX32_CONST( p_wk->end.x ), FX32_CONST( p_wk->end.y ), 0 );
			VEC_Subtract( &s, &e, p_vec );
		}
		if( p_sync )
		{	
			*p_sync	= p_wk->flik_sync;
		}
		return TRUE;
	}
	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	キー押しつづけを取得
 *
 *	@param	const UI_WORK *cp_wk	ワーク
 *	@param	*p_sync								押し続けている時間
 *
 *	@return	TRUEなら押している
 */
//-----------------------------------------------------------------------------
static int UI_IsContKey( const UI_WORK *cp_wk, u32 *p_sync )
{	
	if( p_sync )
	{	
		*p_sync	= cp_wk->key_cont_sync;
	}
	return GFL_UI_KEY_GetCont();
}
//=============================================================================
/**
 *	ACLR
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	初期化
 *
 *	@param	ACLR_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void ACLR_Init( ACLR_WORK *p_wk )
{	
	GFL_STD_MemClear(p_wk, sizeof(ACLR_WORK));
}
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	破棄
 *
 *	@param	ACLR_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void ACLR_Exit( ACLR_WORK *p_wk )
{	
	GFL_STD_MemClear(p_wk, sizeof(ACLR_WORK));
}
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	メイン処理
 *
 *	@param	ACLR_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void ACLR_Main( ACLR_WORK *p_wk )
{	
	if( p_wk->is_update )
	{
		p_wk->is_update	= FALSE;
		p_wk->aclr	= p_wk->distance_rate	+ p_wk->sync_rate / 2;
		p_wk->aclr	= FX_Mul( p_wk->aclr, ACLR_SCROLL_ACLR_DIF ) + ACLR_SCROLL_ACLR_MIN;
	}
	//NAGI_Printf( "now aclr %d\n", p_wk->aclr );
}
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	加速を決める値をセット
 *
 *	@param	ACLR_WORK *p_wk	ワーク
 *	@param	distance				距離
 *	@param	sync						シンク
 *
 */
//-----------------------------------------------------------------------------
static void ACLR_SetAclr( ACLR_WORK *p_wk, fx32 distance, u32 sync )
{	
	//上限・下限以外ならばはじく
	if( ACLR_SCROLL_DISTANCE_MIN < MATH_IAbs(distance) )
	{	
		p_wk->dir	= distance/ MATH_IAbs(distance);
		distance	= MATH_IAbs( distance );
		distance	= MATH_CLAMP( distance, ACLR_SCROLL_DISTANCE_MIN, ACLR_SCROLL_DISTANCE_MAX );
	}
	else
	{
		return ;
	}
	if( FX32_CONST(sync) < ACLR_SCROLL_SYNC_MAX ) 
	{	
		sync			= MATH_CLAMP( FX32_CONST(sync), ACLR_SCROLL_SYNC_MIN, ACLR_SCROLL_SYNC_MAX );
	}
	else
	{
		return ;
	}

	//それぞれのレートを計算
	p_wk->distance_rate	=	FX_Div((distance - ACLR_SCROLL_DISTANCE_MIN), ACLR_SCROLL_DISTANCE_DIF);
	//syncはMINの方が良い値（=RateではMAXに近くしたい）ので反転する
	p_wk->sync_rate	=	FX_Div((sync - ACLR_SCROLL_SYNC_MIN), ACLR_SCROLL_SYNC_DIF);
	p_wk->sync_rate	= FX32_ONE - p_wk->sync_rate;

	//OS_Printf( "距離%f\n シンク%f\n", FX_FX32_TO_F32(distance), FX_FX32_TO_F32(sync) );
	//OS_Printf( "rate dis%f sync%f\n", FX_FX32_TO_F32(p_wk->distance_rate), FX_FX32_TO_F32(p_wk->sync_rate) );

	p_wk->is_update	= TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	停止処理
 *
 *	@param	p_wk ワーク
 *
 */
//-----------------------------------------------------------------------------
static void ACLR_Stop( ACLR_WORK *p_wk )
{	
	p_wk->aclr	= 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	スクロール加速度を取得
 *
 *	@param	const ACLR_WORK *cp_wk	ワーク
 *
 *	@return	スクロール加速度
 */
//-----------------------------------------------------------------------------
static s16 ACLR_GetScrollAdd( ACLR_WORK *p_wk )
{	
	
	fx32 scroll_add;
	fx32 aclr_dec;

	//スクロール速度
	scroll_add	= MATH_CLAMP( p_wk->aclr*p_wk->dir, ACLR_SCROLL_MOVE_MIN, ACLR_SCROLL_MOVE_MAX );

	//減衰
	aclr_dec		= FX_Mul(p_wk->aclr, ACLR_SCROLL_DEC_RATE);
	p_wk->aclr	-= aclr_dec;
	if( -FX32_ONE <p_wk->aclr && p_wk->aclr < FX32_ONE)
	{	
		p_wk->aclr	= 0;
	}
	//OS_Printf("スクロールAdd %d\n",  scroll_add>>FX32_SHIFT );

	return (scroll_add >> FX32_SHIFT);
}
//----------------------------------------------------------------------------
/**
 *	@brief	ACLR	加速が存在するか
 *
 *	@param	const ACLR_WORK *cp_wk	ワーク
 *
 *	@return	TRUEならば加速が存在
 */
//-----------------------------------------------------------------------------
static BOOL ACLR_IsExist( const ACLR_WORK *cp_wk )
{	
	return cp_wk->aclr != 0;
}
//=============================================================================
/**
 *	ETC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	センター位置へプリント
 *
 *	@param	GFL_BMPWIN *p_bmpwin	BMPWIN
 *	@param	*p_strbuf							文字
 *	@param	*p_font								フォント
 *
 */
//-----------------------------------------------------------------------------
static void PRINT_PrintCenter( GFL_BMPWIN *p_bmpwin, STRBUF *p_strbuf, GFL_FONT *p_font )
{	
	u16 x, y;

	x	= GFL_BMPWIN_GetSizeX( p_bmpwin )*4;
	y	= GFL_BMPWIN_GetSizeY( p_bmpwin )*4;
	x	-= PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 )/2;
	y	-= PRINTSYS_GetStrHeight( p_strbuf, p_font )/2;

	PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_bmpwin), x, y, p_strbuf, p_font );
}
//----------------------------------------------------------------------------
/**
 *	@brief  センター位置へのプリント
 *
 *	@param	GFL_BMPWIN *p_bmpwin	BMPWIN
 *	@param	*p_strbuf							文字
 *	@param	MYSTATUS *cp_status   ステータス
 *	@param	*p_font								フォント
 */
//-----------------------------------------------------------------------------
static void PRINT_PrintNameCenter( GFL_BMPWIN *p_bmpwin, STRBUF *p_strbuf, const MYSTATUS *cp_status, GFL_FONT *p_font )
{ 
  if( cp_status == NULL )
  { 
	u16 x, y;

	x	= GFL_BMPWIN_GetSizeX( p_bmpwin )*4;
	y	= GFL_BMPWIN_GetSizeY( p_bmpwin )*4;
	x	-= PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 )/2;
	y	-= PRINTSYS_GetStrHeight( p_strbuf, p_font )/2;

	PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_bmpwin), x, y, p_strbuf, p_font );
  }
  else
  { 

	u16 x, y;
  STRBUF *p_dst;
  WORDSET *p_word = WORDSET_Create( GFL_HEAP_LOWID(HEAPID_IRCRANKING) );

  p_dst = GFL_STR_CreateBuffer( 128, GFL_HEAP_LOWID(HEAPID_IRCRANKING) );
  WORDSET_RegisterPlayerName( p_word, 0, cp_status );
  WORDSET_ExpandStr( p_word, p_dst, p_strbuf );

	x	= GFL_BMPWIN_GetSizeX( p_bmpwin )*4;
	y	= GFL_BMPWIN_GetSizeY( p_bmpwin )*4;
	x	-= PRINTSYS_GetStrWidth( p_dst, p_font, 0 )/2;
	y	-= PRINTSYS_GetStrHeight( p_dst, p_font )/2;


	PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_bmpwin), x, y, p_dst, p_font );

  GFL_STR_DeleteBuffer( p_dst );
  WORDSET_Delete( p_word );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief	BMPのキャラ単位コピー処理（PRINTはドット単位でコピーするのでより早い）
 *
 *	@param	const GFL_BMP_DATA *cp_src	転送元
 *	@param	*p_dst	転送先
 *	@param	src_x		転送元X座標（以下すべてキャラ単位）
 *	@param	src_y		転送元Y座標
 *	@param	dst_x		転送先X座標	
 *	@param	dst_y		転送先Y座標
 *	@param	w				転送幅
 *	@param	h				転送高さ
 *
 */
//-----------------------------------------------------------------------------
static void BMP_Copy( const GFL_BMP_DATA *cp_src, GFL_BMP_DATA *p_dst, u16 src_x, u16 src_y, u16 dst_x, u16 dst_y, u16 w, u16 h )
{	
	const u8	*cp_src_chr		= (const u8 *)GFL_BMP_GetCharacterAdrs( (GFL_BMP_DATA *)(cp_src) );
	u8	*p_dst_chr					= (u8 *)GFL_BMP_GetCharacterAdrs( p_dst );
	const u8 *cp_src_chr_ofs;
	u8 *p_dst_chr_ofs;


	cp_src_chr_ofs	= cp_src_chr + ((w * src_y) + src_x)*GFL_BG_1CHRDATASIZ;
	p_dst_chr_ofs		= p_dst_chr	+ ((w * dst_y) + dst_x)*GFL_BG_1CHRDATASIZ;

	GFL_STD_MemCopy32( cp_src_chr_ofs, p_dst_chr_ofs, w * h * GFL_BG_1CHRDATASIZ );
}

//----------------------------------------------------------------------------
/**
 *	@brief	POINT版距離を測る
 *
 *	@param	const GFL_POINT *cp_a
 *	@param	GFL_POINT *cp_b 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static s32 GFL_POINT_Distance( const GFL_POINT *cp_a, const GFL_POINT *cp_b )
{	
	VecFx32	a;
	VecFx32	b;

	VEC_Set( &a, FX32_CONST(cp_a->x), FX32_CONST(cp_a->y), 0);
	VEC_Set( &b, FX32_CONST(cp_b->x), FX32_CONST(cp_b->y), 0);

	return VEC_Distance( &a, &b ) >> FX32_SHIFT;
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
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h, u8 plt, BOOL compressedFlag, HEAPID heapID )
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

struct _RANKING_MSGOAM_WORK
{ 
  GFL_BMP_DATA        *p_bmp;     //文字用BMP
  PRINTSYS_LSB        color;      //文字色
  BOOL                trans_req;  //BMP転送リクエストフラグ
  GFL_POINT           ofs;        //文字描画位置オフセット
  RANKING_MSGOAM_POS  pos_type;   //描画位置タイプ
  STRBUF              *p_strbuf;  //文字バッファ
  BMPOAM_ACT_PTR		  p_bmpoam_wk;  //BMPOAM
  PRINT_QUE           *p_que;       //描画キュー
};
//-------------------------------------
///	文字描画位置計算
//=====================================
static void Ranking_Msgoam_CalcPos( const RANKING_MSGOAM_WORK* cp_wk, GFL_FONT *p_font, GFL_POINT *p_pos );

//-------------------------------------
///	パブリック
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM  作成
 *
 *	@param	const GFL_CLWK_DATA *cp_data  OAM情報
 *	@param	w                             幅  キャラ単位
 *	@param	h                             高さ  キャラ単位
 *	@param	plt_idx                       GFL_CLGRPに登録したパレットインデックス
 *	@param  plt_ofs                       パレットオフセット
 *	@param	p_bmpoam_sys                  BMPOAMシステム
 *	@param	*p_que  メッセージ描画キュー
 *	@param	heapID  ヒープID
 *
 *	@return MSGOAMワーク
 */
//-----------------------------------------------------------------------------
static RANKING_MSGOAM_WORK * RANKING_MSGOAM_Init( const GFL_CLWK_DATA *cp_cldata, u8 w, u8 h, u32 plt_idx, u8 plt_ofs, CLSYS_DRAW_TYPE draw_type, BMPOAM_SYS_PTR p_bmpoam_sys, PRINT_QUE *p_que, HEAPID heapID )
{ 
  RANKING_MSGOAM_WORK *p_wk;
  p_wk  = GFL_HEAP_AllocMemory( heapID, sizeof(RANKING_MSGOAM_WORK) );
  GFL_STD_MemClear( p_wk, sizeof(RANKING_MSGOAM_WORK) );
  p_wk->p_que = p_que;

  //バッファ作成
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( 128, heapID );

  //BMP作成
  p_wk->p_bmp = GFL_BMP_Create( w, h, GFL_BMP_16_COLOR, heapID );

	//BMPOAM作成
	{	
		BMPOAM_ACT_DATA	actdata;
		
		GFL_STD_MemClear( &actdata, sizeof(BMPOAM_ACT_DATA) );
		actdata.bmp	= p_wk->p_bmp;
		actdata.x		= cp_cldata->pos_x;
		actdata.y		= cp_cldata->pos_y;
		actdata.pltt_index	= plt_idx;
		actdata.soft_pri		= cp_cldata->softpri;
		actdata.setSerface	= draw_type;
		actdata.draw_type		= draw_type;
		actdata.bg_pri			= cp_cldata->bgpri;
    actdata.pal_offset  = plt_ofs;
		p_wk->p_bmpoam_wk	  = BmpOam_ActorAdd( p_bmpoam_sys, &actdata );
	}

  return p_wk;
}

//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM　破棄
 *
 *	@param	RANKING_MSGOAM_WORK* p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void RANKING_MSGOAM_Exit( RANKING_MSGOAM_WORK* p_wk )
{ 
  BmpOam_ActorDel( p_wk->p_bmpoam_wk );
  GFL_BMP_Delete( p_wk->p_bmp );
  GFL_STR_DeleteBuffer( p_wk->p_strbuf );
  GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM  背景色でクリア
 *
 *	@param	RANKING_MSGOAM_WORK* p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void RANKING_MSGOAM_Clear( RANKING_MSGOAM_WORK* p_wk )
{
  GFL_BMP_Clear( p_wk->p_bmp, PRINTSYS_LSB_GetB(p_wk->color) );
  BmpOam_ActorBmpTrans( p_wk->p_bmpoam_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM  文字描画
 *
 *	@param	RANKING_MSGOAM_WORK* p_wk ワーク
 *	@param	*p_msg                    メッセージ
 *	@param	strID                     文字番号
 *	@param	*p_font                   フォント
 */
//-----------------------------------------------------------------------------
static void RANKING_MSGOAM_Print( RANKING_MSGOAM_WORK* p_wk, GFL_MSGDATA *p_msg, u32 strID, GFL_FONT *p_font )
{ 
  GFL_POINT pos;

	//一端消去
	GFL_BMP_Clear( p_wk->p_bmp, PRINTSYS_LSB_GetB( p_wk->color ) );	

	//文字列作成
	GFL_MSG_GetString( p_msg, strID, p_wk->p_strbuf );

  //位置計算
  Ranking_Msgoam_CalcPos( p_wk, p_font, &pos );

	//文字列描画
  PRINTSYS_PrintQueColor( p_wk->p_que, p_wk->p_bmp, pos.x, pos.y, p_wk->p_strbuf, p_font, p_wk->color );
  p_wk->trans_req = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  MSGOAM  文字描画STRBUF版
 *
 *	@param	RANKING_MSGOAM_WORK* p_wk     ワーク
 *	@param	STRBUF *cp_strbuf             文字列
 *	@param	*p_font                       フォント
 */
//-----------------------------------------------------------------------------
static void RANKING_MSGOAM_PrintBuf( RANKING_MSGOAM_WORK* p_wk, const STRBUF *cp_strbuf, GFL_FONT *p_font )
{ 
  GFL_POINT pos;

	//一端消去
	GFL_BMP_Clear( p_wk->p_bmp, PRINTSYS_LSB_GetB( p_wk->color ) );	

	//文字列作成
  GFL_STR_CopyBuffer( p_wk->p_strbuf, cp_strbuf );

  //位置計算
  Ranking_Msgoam_CalcPos( p_wk, p_font, &pos );

	//文字列描画
  PRINTSYS_PrintQueColor( p_wk->p_que, p_wk->p_bmp, pos.x, pos.y, p_wk->p_strbuf, p_font, p_wk->color );
  p_wk->trans_req = TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  文字色を設定
 *
 *	@param	RANKING_MSGOAM_WORK* p_wk   ワーク
 *	@param	color                       文字色
 */
//-----------------------------------------------------------------------------
static void RANKING_MSGOAM_SetStrColor( RANKING_MSGOAM_WORK* p_wk, PRINTSYS_LSB color )
{ 
  p_wk->color = color;
}

//----------------------------------------------------------------------------
/**
 *	@brief  文字描画位置設定
 *
 *	@param	RANKING_MSGOAM_WORK* p_wk ワーク
 *	@param	x 座標
 *	@param	y 座標
 *	@param	type  座標タイプ
 */
//-----------------------------------------------------------------------------
static void RANKING_MSGOAM_SetStrPos( RANKING_MSGOAM_WORK* p_wk, s16 x, s16 y, RANKING_MSGOAM_POS type )
{ 
  p_wk->ofs.x = x;
  p_wk->ofs.y = y;
  p_wk->pos_type  = type;
}
//----------------------------------------------------------------------------
/**
 *	@brief  表示処理
 *
 *	@param	RANKING_MSGOAM_WORK* p_wk ワーク
 *
 *	@return TRUEで描画完了  FALSEで描画中
 */
//-----------------------------------------------------------------------------
static BOOL RANKING_MSGOAM_PrintMain( RANKING_MSGOAM_WORK* p_wk )
{ 
  if( p_wk->trans_req )
  { 
    if( !PRINTSYS_QUE_IsExistTarget( p_wk->p_que, p_wk->p_bmp ) )
    { 
      BmpOam_ActorBmpTrans( p_wk->p_bmpoam_wk );
      p_wk->trans_req = FALSE;
    }
  }

  return !p_wk->trans_req;
}

//----------------------------------------------------------------------------
/**
 *	@brief  BMPOAM取得
 *
 *	@param	RANKING_MSGOAM_WORK* p_wk   ワーク
 *
 *	@return BMPOAM
 */
//-----------------------------------------------------------------------------
static BMPOAM_ACT_PTR RANKING_MSGOAM_GetBmpOamAct( RANKING_MSGOAM_WORK* p_wk )
{ 
  return p_wk->p_bmpoam_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief  描画座標計算
 *
 *	@param	const RANKING_MSGOAM_WORK* cp_wk  ワーク
 *	@param  フォント
 *	@param	*p_pos                            描画位置受け取り
 *
 */
//-----------------------------------------------------------------------------
static void Ranking_Msgoam_CalcPos( const RANKING_MSGOAM_WORK* cp_wk, GFL_FONT *p_font, GFL_POINT *p_pos )
{ 
  switch( cp_wk->pos_type )
  { 
  case RANKING_MSGOAM_POS_ABSOLUTE:     //絶対位置
    *p_pos  = cp_wk->ofs;
    break;
  case RANKING_MSGOAM_POS_WH_CENTER:  //相対座標  幅、高さともに中央
    { 
      u32 x, y;
      x = GFL_BMP_GetSizeX( cp_wk->p_bmp ) / 2;
      y = GFL_BMP_GetSizeY( cp_wk->p_bmp ) / 2;
      x -= PRINTSYS_GetStrWidth( cp_wk->p_strbuf, p_font, 0 ) / 2;
      y -= PRINTSYS_GetStrHeight( cp_wk->p_strbuf, p_font ) / 2;
      p_pos->x  = x + cp_wk->ofs.x;
      p_pos->y  = y + cp_wk->ofs.y;
    }
    break;
  case RANKING_MSGOAM_POS_H_CENTER:    //相対座標  高さのみ中央
    { 
      u32 y;
      y = GFL_BMP_GetSizeY( cp_wk->p_bmp ) / 2;
      y -= PRINTSYS_GetStrHeight( cp_wk->p_strbuf, p_font ) / 2;
      p_pos->x  = cp_wk->ofs.x;
      p_pos->y  = y + cp_wk->ofs.y;
    }
    break;
  }
}
