//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_rhythm_main.c
 *	@brief	赤外線ミニゲーム	リズムチェック
 *	@author	Toru=Nagihashi
 *	@data		2009.05.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	lib
#include <gflib.h>

//	constant
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"

//	module
#include "infowin/infowin.h"

#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"

//	archive
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_irc_rhythm.h"
#include "ircrhythm_gra.naix"

//	rhythm
#include "net_app/irc_rhythm.h"
// proc
#include "net_app/irc_result.h"

#ifdef PM_DEBUG
//debug用
#include "system/net_err.h"	//VRAM退避用アドレスを貰うため
#include <wchar.h>					//wcslen
#endif

FS_EXTERN_OVERLAY(irc_result);

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
#ifdef PM_DEBUG
//#define DEBUG_RHYTHM_MSG	//デバッグメッセージを出す
//#define DEBUG_ONLY_PLAY	//デバッグ用一台でプレイするモードを出す
#endif //PM_DEBUG


//-------------------------------------
///	点数
//=====================================
//間隔の時間による得点
#define VAL_RHYTHMSCORE_PROC_00	(100)
#define VAL_RHYTHMSCORE_PROC_01	(90)
#define VAL_RHYTHMSCORE_PROC_02	(80)
#define VAL_RHYTHMSCORE_PROC_03	(70)
#define VAL_RHYTHMSCORE_PROC_04	(60)
#define VAL_RHYTHMSCORE_PROC_05	(50)
#define VAL_RHYTHMSCORE_PROC_06	(40)
#define VAL_RHYTHMSCORE_PROC_07	(30)
#define VAL_RHYTHMSCORE_PROC_08	(20)
#define VAL_RHYTHMSCORE_PROC_09	(10)
#define VAL_RHYTHMSCORE_PROC_10	(0)

#define RANGE_RHYTHMSCORE_PROC_00	(0)
#define RANGE_RHYTHMSCORE_PROC_01	(500)
#define RANGE_RHYTHMSCORE_PROC_02	(1000)
#define RANGE_RHYTHMSCORE_PROC_03	(3000)
#define RANGE_RHYTHMSCORE_PROC_04	(5000)
#define RANGE_RHYTHMSCORE_PROC_05	(10000)
#define RANGE_RHYTHMSCORE_PROC_06	(15000)
#define RANGE_RHYTHMSCORE_PROC_07	(20000)
#define RANGE_RHYTHMSCORE_PROC_08	(25000)
#define RANGE_RHYTHMSCORE_PROC_09	(30000)

//間隔の時間による得点
#define VAL_RHYTHMSCORE_DIFF_00	(100)
#define VAL_RHYTHMSCORE_DIFF_01	(90)
#define VAL_RHYTHMSCORE_DIFF_02	(80)
#define VAL_RHYTHMSCORE_DIFF_03	(70)
#define VAL_RHYTHMSCORE_DIFF_04	(60)
#define VAL_RHYTHMSCORE_DIFF_05	(50)
#define VAL_RHYTHMSCORE_DIFF_06	(40)
#define VAL_RHYTHMSCORE_DIFF_07	(30)
#define VAL_RHYTHMSCORE_DIFF_08	(20)
#define VAL_RHYTHMSCORE_DIFF_09	(10)
#define VAL_RHYTHMSCORE_DIFF_10	(0)

#define RANGE_RHYTHMSCORE_DIFF_00	(0)
#define RANGE_RHYTHMSCORE_DIFF_01	(50)
#define RANGE_RHYTHMSCORE_DIFF_02	(100)
#define RANGE_RHYTHMSCORE_DIFF_03	(200)
#define RANGE_RHYTHMSCORE_DIFF_04	(300)
#define RANGE_RHYTHMSCORE_DIFF_05	(400)
#define RANGE_RHYTHMSCORE_DIFF_06	(500)
#define RANGE_RHYTHMSCORE_DIFF_07	(600)
#define RANGE_RHYTHMSCORE_DIFF_08	(1000)
#define RANGE_RHYTHMSCORE_DIFF_09	(15000)


//-------------------------------------
///	パレット
//=====================================
enum{	
	// メイン画面BG
	RHYTHM_BG_PAL_M_00 = 0,//背景用BG
	RHYTHM_BG_PAL_M_01,		// 使用してない
	RHYTHM_BG_PAL_M_02,		// 使用してない
	RHYTHM_BG_PAL_M_03,		// 使用してない
	RHYTHM_BG_PAL_M_04,		// 使用してない
	RHYTHM_BG_PAL_M_05,		// 使用してない
	RHYTHM_BG_PAL_M_06,		// 使用してない
	RHYTHM_BG_PAL_M_07,		// 使用してない
	RHYTHM_BG_PAL_M_08,		// 使用してない
	RHYTHM_BG_PAL_M_09,		// 使用してない
	RHYTHM_BG_PAL_M_10,		// 使用してない
	RHYTHM_BG_PAL_M_11,		// 使用してない
	RHYTHM_BG_PAL_M_12,		// 使用してない
	RHYTHM_BG_PAL_M_13,		// 使用してない
	RHYTHM_BG_PAL_M_14,		// フォント
	RHYTHM_BG_PAL_M_15,		// INFOWIN


	// サブ画面BG
	RHYTHM_BG_PAL_S_00 = 0,	//使用していない
	RHYTHM_BG_PAL_S_01,		// 使用してない
	RHYTHM_BG_PAL_S_02,		// 使用してない
	RHYTHM_BG_PAL_S_03,		// 使用してない
	RHYTHM_BG_PAL_S_04,		// 使用してない
	RHYTHM_BG_PAL_S_05,		// 使用してない
	RHYTHM_BG_PAL_S_06,		// 使用してない
	RHYTHM_BG_PAL_S_07,		// 使用してない
	RHYTHM_BG_PAL_S_08,		// 使用してない
	RHYTHM_BG_PAL_S_09,		// 使用してない
	RHYTHM_BG_PAL_S_10,		// 使用してない
	RHYTHM_BG_PAL_S_11,		// 使用してない
	RHYTHM_BG_PAL_S_12,		// 使用してない
	RHYTHM_BG_PAL_S_13,		// 使用してない
	RHYTHM_BG_PAL_S_14,		//	フォント
	RHYTHM_BG_PAL_S_15,		// 使用してない

	// メイン画面OBJ
	RHYTHM_OBJ_PAL_M_00 = 0,// タッチ反応OBJ
	RHYTHM_OBJ_PAL_M_01,		// 使用してない
	RHYTHM_OBJ_PAL_M_02,		// 使用してない
	RHYTHM_OBJ_PAL_M_03,		// 使用してない
	RHYTHM_OBJ_PAL_M_04,		// 使用してない
	RHYTHM_OBJ_PAL_M_05,		// 使用してない
	RHYTHM_OBJ_PAL_M_06,		// 使用してない
	RHYTHM_OBJ_PAL_M_07,		// 使用してない
	RHYTHM_OBJ_PAL_M_08,		// 使用してない
	RHYTHM_OBJ_PAL_M_09,		// 使用してない
	RHYTHM_OBJ_PAL_M_10,		// 使用してない
	RHYTHM_OBJ_PAL_M_11,		// 使用してない
	RHYTHM_OBJ_PAL_M_12,		// 使用してない
	RHYTHM_OBJ_PAL_M_13,		// 使用してない
	RHYTHM_OBJ_PAL_M_14,		// 使用してない
	RHYTHM_OBJ_PAL_M_15,		// 使用してない


	// サブ画面OBJ
	RHYTHM_OBJ_PAL_S_00 = 0,	//使用してない
	RHYTHM_OBJ_PAL_S_01,		// 使用してない
	RHYTHM_OBJ_PAL_S_02,		// 使用してない
	RHYTHM_OBJ_PAL_S_03,		// 使用してない
	RHYTHM_OBJ_PAL_S_04,		// 使用してない
	RHYTHM_OBJ_PAL_S_05,		// 使用してない
	RHYTHM_OBJ_PAL_S_06,		// 使用してない
	RHYTHM_OBJ_PAL_S_07,		// 使用してない
	RHYTHM_OBJ_PAL_S_08,		// 使用してない
	RHYTHM_OBJ_PAL_S_09,		// 使用してない
	RHYTHM_OBJ_PAL_S_10,		// 使用してない
	RHYTHM_OBJ_PAL_S_11,		// 使用してない
	RHYTHM_OBJ_PAL_S_12,		// 使用してない
	RHYTHM_OBJ_PAL_S_13,		// 使用してない
	RHYTHM_OBJ_PAL_S_14,		// 使用してない
	RHYTHM_OBJ_PAL_S_15,		// 使用してない
};

//-------------------------------------
///	情報バー
//=====================================
#define INFOWIN_PLT_NO		(RHYTHM_BG_PAL_M_15)
#define INFOWIN_BG_FRAME	(GFL_BG_FRAME0_M)

//-------------------------------------
///	文字
//=====================================
#define TEXTSTR_PLT_NO				(RHYTHM_BG_PAL_S_14)
#define TEXTSTR_BUFFER_LENGTH	(255)

//-------------------------------------
///	位置
//=====================================
#define	MSGWND_TEXT_X	(1)
#define	MSGWND_TEXT_Y	(18)
#define	MSGWND_TEXT_W	(30)
#define	MSGWND_TEXT_H	(5)

#define	MSGWND_RETURN_X	(1)
#define	MSGWND_RETURN_Y	(22)
#define	MSGWND_RETURN_W	(30)
#define	MSGWND_RETURN_H	(2)

//-------------------------------------
///	カウント
//=====================================
#define TOUCHMARKER_VISIBLE_CNT	(10)

//-------------------------------------
///	計測情報
//=====================================
#define RHYTHMSEARCH_DATA_MAX	(15)	//計測回数最大
#define TOUCH_DIAMOND_W	(48)				//タッチ幅
#define TOUCH_DIAMOND_H	(48)

//-------------------------------------
///		MSG_FONT
//=====================================
typedef enum {	
	MSG_FONT_TYPE_LARGE,
	MSG_FONT_TYPE_SMALL,
}MSG_FONT_TYPE;

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

//-------------------------------------
///	MSGWNDID
//=====================================
typedef enum {
	MSGWNDID_TEXT,
	MSGWNDID_RETURN,

	MSGWNDID_MAX
} MSGWNDID;

//-------------------------------------
///	個人結果画面の定数
//=====================================
enum
{	
	ROR_MSGWNDID_TITLE,
	ROR_MSGWNDID_MSG,
	ROR_MSGWNDID_DEBUG,

	ROR_MSGWNDID_MAX,
};

#define ROR_MSGWND_TITLE_X	(2)
#define ROR_MSGWND_TITLE_Y	(3)
#define ROR_MSGWND_TITLE_W	(20)
#define ROR_MSGWND_TITLE_H	(2)
#define ROR_MSGWND_MSG_X		(6)
#define ROR_MSGWND_MSG_Y		(9)
#define ROR_MSGWND_MSG_W		(20)
#define ROR_MSGWND_MSG_H		(6)
#define ROR_MSGWND_DEBUG_X	(2)
#define ROR_MSGWND_DEBUG_Y	(16)
#define ROR_MSGWND_DEBUG_W	(28)
#define ROR_MSGWND_DEBUG_H	(4)

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	BG関係
//=====================================
typedef struct
{
	int dummy;
} GRAPHIC_BG_WORK;

//-------------------------------------
///	OBJ関係
//=====================================
typedef struct {
	GFL_CLUNIT *p_clunit;
	u32				reg_id[OBJREGID_MAX];
	GFL_CLWK	 *p_clwk[CLWKID_MAX];
} GRAPHIC_OBJ_WORK;

//-------------------------------------
///	描画関係
//=====================================
typedef struct 
{
	GRAPHIC_BG_WORK		bg;
	GRAPHIC_OBJ_WORK	obj;
	GFL_TCB						*p_vblank_task;
} GRAPHIC_WORK;
//-------------------------------------
///	文字管理
//=====================================
typedef struct 
{
	GFL_FONT*				  p_font;
  GFL_MSGDATA*			p_msg;
  PRINT_QUE*        p_print_que;
	WORDSET*					p_wordset;
} MSG_WORK;

//-------------------------------------
///	メッセージ表示ウィンドウ
//=====================================
typedef struct
{
	GFL_BMPWIN*				p_bmpwin;
	PRINT_UTIL        print_util;
	STRBUF*						p_strbuf;
} MSGWND_WORK;

//-------------------------------------
///	サーチ部
//=====================================
typedef struct
{
	//どこを押したか
	GFL_POINT		pos;
	//押した時間
	u32					prog_ms;	//経過時間
	u32					diff_ms;	//差の時間
} RHYTHMSEARCH_DATA;
typedef struct
{	
	RHYTHMSEARCH_DATA	data[RHYTHMSEARCH_DATA_MAX];
	u32			data_idx;
	OSTick	start_time;
} RHYTHMSEARCH_WORK;

//-------------------------------------
///	RHYTHM用ネット
//=====================================
//ネットメイン
typedef struct {
	COMPATIBLE_IRC_SYS	*p_irc;
	u32 seq;
	RHYTHMSEARCH_WORK	result_recv;
	RHYTHMSEARCH_WORK	result_recv_my;
	RHYTHMSEARCH_WORK	result_send;
	BOOL is_recv;
} RHYTHMNET_WORK;

//-------------------------------------
///	個人成績画面
//=====================================
typedef struct 
{
	u32	seq;
	const MSG_WORK *cp_msg;
	const RHYTHMSEARCH_WORK *cp_search;
	MSGWND_WORK			msgwnd[ROR_MSGWNDID_MAX];
} RHYTHM_ONLYRESULT_WORK;

#ifdef DEBUG_RHYTHM_MSG
//-------------------------------------
///	デバッグプリント用画面
//=====================================
typedef struct
{
	GFL_BMP_DATA *p_bmp;
	GFL_FONT*			p_font;

	BOOL	is_now_save;
	u8	frm;

  u8  *p_char_temp_area;      ///<キャラクタVRAM退避先
  u16 *p_scrn_temp_area;      ///<スクリーンVRAM退避先
  u16 *p_pltt_temp_area;      ///<パレットVRAM退避先

 	u8  font_col_bkup[3];
  u8  prioryty_bkup;
  u8  scroll_x_bkup;
  u8  scroll_y_bkup;

	HEAPID heapID;
} DEBUG_PRINT_WORK;

#define DEBUGPRINT_CHAR_TEMP_AREA (0x4000)
#define DEBUGPRINT_SCRN_TEMP_AREA (0x800)
#define DEBUGPRINT_PLTT_TEMP_AREA (0x20)
#define DEBUGPRINT_WIDTH  (32)
#define DEBUGPRINT_HEIGHT (18)

static DEBUG_PRINT_WORK *sp_dp_wk;
#endif //DEBUG_RHYTHM_MSG

//-------------------------------------
///	リズムチェックメインワーク
//=====================================
typedef struct _RHYTHM_MAIN_WORK RHYTHM_MAIN_WORK;
typedef void (*SEQ_FUNCTION)( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
struct _RHYTHM_MAIN_WORK
{
	//グラフィックモジュール
	GRAPHIC_WORK		grp;
	MSG_WORK				msg;
	MSGWND_WORK			msgwnd[MSGWNDID_MAX];

	//ネット
	RHYTHMNET_WORK	net;

	//結果表示
	RHYTHM_ONLYRESULT_WORK	onlyresult;

	//シーケンス管理
	SEQ_FUNCTION		seq_function;
	u16		seq;
	BOOL is_end;
	BOOL is_next_proc;

	//計測
	RHYTHMSEARCH_WORK	search;

	//その他
	u32								marker_cnt;	//マーカー表示時間

	//引数
	IRC_RHYTHM_PARAM	*p_param;

#ifdef DEBUG_ONLY_PLAY
	//デバッグ用
	RHYTHMSEARCH_WORK	search2;
	u16							debug_player;			//自分か相手か
	u16							debug_game_cnt;	//何ゲーム目か
#endif //DEBUG_ONLY_PLAY
};

//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//proc
static GFL_PROC_RESULT IRC_RHYTHM_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_RHYTHM_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_RHYTHM_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
//grp
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk );
static GFL_CLWK* GRAPHIC_GetClwk( const GRAPHIC_WORK *cp_wk, CLWKID id );
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//BG
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
//obj
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLWK* GRAPHIC_OBJ_GetClwk( const GRAPHIC_OBJ_WORK *cp_wk, CLWKID id );
//MSG_WORK
static void MSG_Init( MSG_WORK *p_wk, MSG_FONT_TYPE font, HEAPID heapID );
static void MSG_Exit( MSG_WORK *p_wk );
static BOOL MSG_Main( MSG_WORK *p_wk );
static GFL_FONT*	MSG_GetFont( const MSG_WORK *cp_wk );
static PRINT_QUE* MSG_GetPrintQue( const MSG_WORK *cp_wk );
static const GFL_MSGDATA * MSG_GetMsgDataConst( const MSG_WORK *cp_wk );
static WORDSET * MSG_GetWordSet( const MSG_WORK *cp_wk );
//MSG_WINDOW
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe,
		u8 x, u8 y, u8 w, u8 h, HEAPID heapID );
static void MSGWND_Exit( MSGWND_WORK* p_wk );
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, const MSG_WORK *cp_msg );
static void MSGWND_Print( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y );
static void MSGWND_PrintCenter( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID );
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 number, u16 buff_id, u16 x, u16 y );
static void MSGWND_Clear( MSGWND_WORK* p_wk );
//ONLYRESULT
static void RHYTHM_ONLYRESULT_Init( RHYTHM_ONLYRESULT_WORK* p_wk, u8 frm, const MSG_WORK *cp_msg, const RHYTHMSEARCH_WORK *cp_search,  HEAPID heapID );
static void RHYTHM_ONLYRESULT_Exit( RHYTHM_ONLYRESULT_WORK* p_wk );
static BOOL RHYTHM_ONLYRESULT_Main( RHYTHM_ONLYRESULT_WORK* p_wk );
//SEQ
static void SEQ_Change( RHYTHM_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function );
static void SEQ_End( RHYTHM_MAIN_WORK *p_wk );
//SEQ_FUNC
static void SEQFUNC_StartGame( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_MainGame( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_Result( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq );
//RHYTHMSEARCH
static void RHYTHMSEARCH_Init( RHYTHMSEARCH_WORK *p_wk );
static void RHYTHMSEARCH_Exit( RHYTHMSEARCH_WORK *p_wk );
static void RHYTHMSEARCH_Start( RHYTHMSEARCH_WORK *p_wk );
static BOOL RHYTHMSEARCH_IsEnd( const RHYTHMSEARCH_WORK *cp_wk );
static void RHYTHMSEARCH_SetData( RHYTHMSEARCH_WORK *p_wk, const GFL_POINT *cp_pos );
//net
static void RHYTHMNET_Init( RHYTHMNET_WORK *p_wk, COMPATIBLE_IRC_SYS *p_irc );
static void RHYTHMNET_Exit( RHYTHMNET_WORK *p_wk );
static BOOL RHYTHMNET_SendResultData( RHYTHMNET_WORK *p_wk, const RHYTHMSEARCH_WORK *cp_data );
static void RHYTHMNET_GetResultData( const RHYTHMNET_WORK *cp_wk, RHYTHMSEARCH_WORK *p_data );
//net_recv
static void NETRECV_Result( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle );
static u8* NETRECV_GetBufferAddr(int netID, void* pWork, int size);
//汎用
static BOOL TP_GetDiamondTrg( const GFL_POINT *cp_diamond, GFL_POINT *p_trg );
static void TouchMarker_SetPos( RHYTHM_MAIN_WORK *p_wk, const GFL_POINT *cp_pos );
static void TouchMarker_Main( RHYTHM_MAIN_WORK *p_wk );
static u8		CalcScore( const RHYTHM_MAIN_WORK *cp_wk );
static BOOL TouchReturnBtn( void );

#ifdef DEBUG_RHYTHM_MSG
static void DEBUGRHYTHM_PRINT_UpDate( RHYTHM_MAIN_WORK *p_wk );
#else
#define DEBUGRHYTHM_PRINT_UpDate(...)	((void)0)
#endif

//DEBUG_PRINT
#ifdef DEBUG_RHYTHM_MSG
static void DEBUGPRINT_Init( u8 frm, BOOL is_now_save, HEAPID heapID );
static void DEBUGPRINT_Exit( void );
static void DEBUGPRINT_Open( void );
static void DEBUGPRINT_Close( void );
static void DEBUGPRINT_Print( const u16 *cp_str, u16 x, u16 y );
static void DEBUGPRINT_PrintNumber( const u16 *cp_str, int number, u16 x, u16 y );
static void DEBUGPRINT_Clear( void );
#else
#define DEBUGPRINT_Init(...)				((void)0)
#define DEBUGPRINT_Exit(...)				((void)0)
#define DEBUGPRINT_Open(...)				((void)0)
#define DEBUGPRINT_Close(...)				((void)0)
#define DEBUGPRINT_Print(...)				((void)0)
#define DEBUGPRINT_PrintNumber(...) ((void)0)
#define DEBUGPRINT_Clear(...)				((void)0)
#endif //DEBUG_RHYTHM_MSG
static STRBUF * DEBUGPRINT_CreateWideChar( const u16 *cp_str, HEAPID heapID );
static STRBUF * DEBUGPRINT_CreateWideCharNumber( const u16 *cp_str, int number, HEAPID heapID );

//=============================================================================
/**
 *					データ
 */
//=============================================================================
//-------------------------------------
///	リズムチェック用プロックデータ
//=====================================
const GFL_PROC_DATA IrcRhythm_ProcData	= 
{	
	IRC_RHYTHM_PROC_Init,
	IRC_RHYTHM_PROC_Main,
	IRC_RHYTHM_PROC_Exit,
};
//-------------------------------------
///	BGシステム
//=====================================
typedef enum 
{
	GRAPHIC_BG_FRAME_M_INFOWIN,
	GRAPHIC_BG_FRAME_M_TEXT	= GRAPHIC_BG_FRAME_M_INFOWIN,
	GRAPHIC_BG_FRAME_M_BACK,
	GRAPHIC_BG_FRAME_S_TEXT,
	GRAPHIC_BG_FRAME_S_BACK,
	GRAPHIC_BG_FRAME_MAX
} GRAPHIC_BG_FRAME;
static const u32 sc_bgcnt_frame[ GRAPHIC_BG_FRAME_MAX ] = 
{
	INFOWIN_BG_FRAME, GFL_BG_FRAME1_M, GFL_BG_FRAME0_S, GFL_BG_FRAME1_S,
};
static const GFL_BG_BGCNT_HEADER sc_bgcnt_data[ GRAPHIC_BG_FRAME_MAX ] = 
{
	// GRAPHIC_BG_FRAME_M_INFOWIN	
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_M_BACK
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_TEXT
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_BACK
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},

};

//-------------------------------------
///	タッチ範囲
//=====================================
static const GFL_POINT	sc_diamond_pos[]	=
{	
	{	
		31, 47
	},
	{	
		79, 47
	},
	{	
		127, 47
	},
	{	
		175, 47
	},
	{	
		223, 47
	},
	{	
		55, 71
	},
	{	
		103, 71
	},
	{	
		151, 71
	},
	{	
		199, 71
	},
	{	
		31, 95
	},
	{	
		79, 95
	},
	{	
		127, 95
	},
	{	
		175, 95
	},
	{	
		223, 95
	},
	{	
		55, 119
	},
	{	
		103, 119
	},
	{	
		151, 119
	},
	{	
		199, 119
	},
	{	
		31, 143
	},
	{	
		79, 143
	},
	{	
		127, 143
	},
	{	
		175, 143
	},
	{	
		223, 143
	},

};

//-------------------------------------
///	NET
//=====================================
enum
{	
	NETRECV_RESULT	= GFL_NET_CMD_IRCRHYTHM,
};
static const NetRecvFuncTable	sc_recv_tbl[]	=
{
	{	
		NETRECV_Result, NETRECV_GetBufferAddr
	}
};

//=============================================================================
/**
 *			PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	リズムチェック	メインプロセス初期化
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RHYTHM_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	RHYTHM_MAIN_WORK	*p_wk;

	//ヒープ作成
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCRHYTHM, 0x20000 );
	//プロセスワーク作成
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(RHYTHM_MAIN_WORK), HEAPID_IRCRHYTHM );
	GFL_STD_MemClear( p_wk, sizeof(RHYTHM_MAIN_WORK) );
	p_wk->p_param	= p_param;

	//モジュール初期化
	GRAPHIC_Init( &p_wk->grp, HEAPID_IRCRHYTHM );
	MSG_Init( &p_wk->msg, MSG_FONT_TYPE_LARGE, HEAPID_IRCRHYTHM );
	INFOWIN_Init( INFOWIN_BG_FRAME, INFOWIN_PLT_NO, HEAPID_IRCRHYTHM );
	MSGWND_Init( &p_wk->msgwnd[MSGWNDID_TEXT], sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TEXT],
			MSGWND_TEXT_X, MSGWND_TEXT_Y, MSGWND_TEXT_W, MSGWND_TEXT_H, HEAPID_IRCRHYTHM );
	MSGWND_Init( &p_wk->msgwnd[MSGWNDID_RETURN], sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT],
			MSGWND_RETURN_X, MSGWND_RETURN_Y, MSGWND_RETURN_W, MSGWND_RETURN_H, HEAPID_IRCRHYTHM );
	MSGWND_PrintCenter( &p_wk->msgwnd[MSGWNDID_RETURN], &p_wk->msg, RHYTHM_BTN_000 );

	RHYTHMNET_Init( &p_wk->net, p_wk->p_param->p_irc );

	RHYTHMSEARCH_Init( &p_wk->search );

	//デバッグ
	DEBUGPRINT_Init( sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_BACK], FALSE, HEAPID_IRCRHYTHM );
	DEBUGPRINT_Open();


	SEQ_Change( p_wk, SEQFUNC_StartGame );
	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	リズムチェック	メインプロセス破棄処理
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RHYTHM_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	RHYTHM_MAIN_WORK	*p_wk;

	p_wk	= p_work;

#if 0
	//次のプロック予約
	if( p_wk->is_next_proc )
	{	
		if( p_wk->p_param->p_gamesys == NULL )
		{	
			GFL_PROC_SysSetNextProc( FS_OVERLAY_ID(irc_result), &IrcResult_ProcData,p_wk->p_param );
		}
		else
		{	
			GAMESYSTEM_SetNextProc( p_wk->p_param->p_gamesys, FS_OVERLAY_ID(irc_result), &IrcResult_ProcData,p_wk->p_param );
		}
	}
#endif

	//デバッグ破棄
	DEBUGPRINT_Close();
	DEBUGPRINT_Exit();

	RHYTHMNET_Exit( &p_wk->net );

	//モジュール破棄
	{
		int i;
		for( i = 0; i < MSGWNDID_MAX; i++ )
		{	
			MSGWND_Exit( &p_wk->msgwnd[i] );
		}
	}
	INFOWIN_Exit();
	GRAPHIC_Exit( &p_wk->grp );
	MSG_Exit( &p_wk->msg );

	//プロセスワーク破棄
	GFL_PROC_FreeWork( p_proc );
	//ヒープ破棄
	GFL_HEAP_DeleteHeap( HEAPID_IRCRHYTHM );
	
	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	リズムチェック	メインプロセスメイン処理
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RHYTHM_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_MAIN,
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};

	RHYTHM_MAIN_WORK	*p_wk;
	p_wk	= p_work;

	switch( *p_seq )
	{	
	case SEQ_INIT:
		*p_seq	= SEQ_FADEOUT_START;
		break;

	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_MAIN;
		}
		break;

	case SEQ_MAIN:

		p_wk->seq_function( p_wk, &p_wk->seq );
		if( p_wk->is_end )
		{	
			*p_seq	= SEQ_FADEIN_START;
		}

#if 0
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_START )
		{	
			SEQ_Change( p_wk, SEQFUNC_StartGame );
		}
#endif

		break;

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
		return GFL_PROC_RES_FINISH;

	default:
		GF_ASSERT_MSG( 0, "IRC_RHYTHM_PROC_MainのSEQエラー %d", *p_seq );
	}

	INFOWIN_Update();
	if( MSG_Main( &p_wk->msg ) )
	{
		int i;
		for( i = 0; i < MSGWNDID_MAX; i++ )
		{	
			MSGWND_Main( &p_wk->msgwnd[i], &p_wk->msg );
		}
	}

	GRAPHIC_Draw( &p_wk->grp );

	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *				GRAPHIC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	描画関係初期化
 *
 *	@param	GRAPHIC_WORK* p_wk	ワーク
 *	@param	heapID					ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Init( GRAPHIC_WORK* p_wk, HEAPID heapID )
{
	static const GFL_DISP_VRAM sc_vramSetTable =
	{
		GX_VRAM_BG_128_A,						// メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,     // メイン2DエンジンのBG拡張パレット
		GX_VRAM_SUB_BG_128_C,				// サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE, // サブ2DエンジンのBG拡張パレット
		GX_VRAM_OBJ_128_B,						// メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
		GX_VRAM_SUB_OBJ_16_I,       // サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,// サブ2DエンジンのOBJ拡張パレット
		GX_VRAM_TEX_NONE,						// テクスチャイメージスロット
		GX_VRAM_TEXPLTT_NONE,				// テクスチャパレットスロット
		GX_OBJVRAMMODE_CHAR_1D_128K,		
		GX_OBJVRAMMODE_CHAR_1D_128K,		
	};

	//ワーククリア
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );

	//VRAMクリアー
	GFL_DISP_ClearVRAM( 0 );

	// VRAMバンク設定
	GFL_DISP_SetBank( &sc_vramSetTable );

	// ディスプレイON
	GFL_DISP_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
	GFL_DISP_SetDispOn();

	//表示
	GFL_DISP_GX_InitVisibleControl();

	//描画モジュール
	GRAPHIC_OBJ_Init( &p_wk->obj, &sc_vramSetTable, heapID );
	GRAPHIC_BG_Init( &p_wk->bg, heapID );

	//VBlankTask登録
	p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(Graphic_VBlankTask, p_wk, 0 );
}


//----------------------------------------------------------------------------
/**
 *	@brief	描画関係破棄
 *
 *	@param	GRAPHIC_WORK* p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Exit( GRAPHIC_WORK* p_wk )
{
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

	GRAPHIC_BG_Exit( &p_wk->bg );
	GRAPHIC_OBJ_Exit( &p_wk->obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	描画関係描画処理
 *
 *	@param	GRAPHIC_WORK* p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Draw( GRAPHIC_WORK* p_wk )
{
	GRAPHIC_OBJ_Main( &p_wk->obj );
}

//----------------------------------------------------------------------------
/**
 *	@brief	CLWK取得
 *
 *	@param	const GRAPHIC_WORK *cp_wk	ワーク
 *	@param	id												CLWKのID
 *
 *	@return	CLWK
 */
//-----------------------------------------------------------------------------
static GFL_CLWK* GRAPHIC_GetClwk( const GRAPHIC_WORK *cp_wk, CLWKID id )
{	
	return GRAPHIC_OBJ_GetClwk( &cp_wk->obj, id );
}

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank用タスク
 *
 *	@param	GFL_TCB *p_tcb	タスクControlブロック
 *	@param	*p_work					ワークアドレス
 *
 */
//-----------------------------------------------------------------------------
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work )
{	
	GRAPHIC_WORK* p_wk	= p_work;
	GRAPHIC_BG_VBlankFunction( &p_wk->bg );
	GRAPHIC_OBJ_VBlankFunction( &p_wk->obj );
}

//=============================================================================
/**
 *					GRAPHIC_BG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG描画	初期化
 *
 *	@param	GRAPHIC_BG_WORK* p_wk	ワーク
 *	@param	heapID						ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK* p_wk, HEAPID heapID )
{
	int i;

	//ＢＧシステム起動
	GFL_BG_Init( heapID );
	GFL_BMPWIN_Init( heapID );

	//ＢＧモード設定
	{
		static const GFL_BG_SYS_HEADER sc_bg_sys_header = 
		{
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
		};	
		GFL_BG_SetBGMode( &sc_bg_sys_header );
	}

	//ＢＧコントロール設定
	{
		int i;

		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{
			GFL_BG_SetBGControl( sc_bgcnt_frame[i], &sc_bgcnt_data[i], GFL_BG_MODE_TEXT );
			GFL_BG_ClearFrame( sc_bgcnt_frame[i] );
			GFL_BG_SetVisible( sc_bgcnt_frame[i], VISIBLE_ON );
		}
	}

	//読み込み設定
	{	
		ARCHANDLE *p_handle;

		p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCRHYTHM_GRAPHIC, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_ircrhythm_gra_rhythm_bg_back_NCLR,
				PALTYPE_MAIN_BG, PALTYPE_MAIN_BG*0x20, 0x20, heapID );
	
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_ircrhythm_gra_rhythm_bg_back_NCGR,
				sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_BACK], 0, 0, FALSE, heapID );

		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_ircrhythm_gra_rhythm_bg_back_NSCR,
				sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_BACK], 0, 0, FALSE, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG描画	破棄
 *
 *	@param	GRAPHIC_BG_WORK* p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK* p_wk )
{	
	int i;

	// BGコントロール破棄
	for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
	{
		GFL_BG_FreeBGControl( sc_bgcnt_frame[i] );
	}

	// BGシステム破棄
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG描画	VBlack関数
 *
 *	@param	GRAPHIC_BG_WORK *p_wk ワーク
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk )
{	
	GFL_BG_VBlankFunc();
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
	//クリア
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );

	//システム作成
	GFL_CLACT_SYS_Create( &GFL_CLSYSINIT_DEF_DIVSCREEN, cp_vram_bank, heapID );
	p_wk->p_clunit	= GFL_CLACT_UNIT_Create( 128, 0, heapID );
	GFL_CLACT_UNIT_SetDefaultRend( p_wk->p_clunit );

	//表示
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

	//リソース読み込み
	{	
		ARCHANDLE *p_handle;

		p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCRHYTHM_GRAPHIC, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_PLT]	= GFL_CLGRP_PLTT_Register( p_handle, 
				NARC_ircrhythm_gra_rhythm_obj_touch_NCLR, CLSYS_DRAW_MAIN, RHYTHM_OBJ_PAL_M_00*0x20, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_ircrhythm_gra_rhythm_obj_touch_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

		p_wk->reg_id[OBJREGID_TOUCH_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_ircrhythm_gra_rhythm_obj_touch_NCER, NARC_ircrhythm_gra_rhythm_obj_touch_NANR, heapID );

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


		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CLWKID_TOUCH], FALSE );
	}
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

//=============================================================================
/**
 *					MSG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	MSG関係を設定
 *
 *	@param	MSG_WORK *p_wk	ワーク
 *	@param	font						フォントタイプ
 *	@param	heapID					ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void MSG_Init( MSG_WORK *p_wk, MSG_FONT_TYPE font, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MSG_WORK) );

	GFL_FONTSYS_Init();

	switch( font )
	{	
	case MSG_FONT_TYPE_LARGE:
		p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
				NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
		break;
	case MSG_FONT_TYPE_SMALL:
		p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
				NARC_font_small_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
		break;
	default:
		GF_ASSERT_MSG( 0, "MSGFONT_ERRO %d", font );
	};

	p_wk->p_print_que = PRINTSYS_QUE_Create( heapID );

	p_wk->p_msg = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_irc_rhythm_dat, heapID );

	p_wk->p_wordset	= WORDSET_Create( heapID );

	{	
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
		GFL_BG_SetBackGroundColor( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], GX_RGB(31,31,31) );
		GFL_BG_SetBackGroundColor( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_TEXT], GX_RGB(31,31,31) );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	MSG関係を破棄
 *
 *	@param	MSG_WORK *p_wk	ワーク
 *	
 */
//-----------------------------------------------------------------------------
static void MSG_Exit( MSG_WORK *p_wk )
{	
	WORDSET_Delete( p_wk->p_wordset );

	GFL_MSG_Delete( p_wk->p_msg );

	PRINTSYS_QUE_Delete( p_wk->p_print_que );

	GFL_FONT_Delete( p_wk->p_font );
	GFL_STD_MemClear( p_wk, sizeof(MSG_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	MSG関係	メイン処理
 *
 *	@param	MSG_WORK *p_wk	ワーク
 *
 * @retval  BOOL	処理が終了していればTRUE／それ以外はFALSE
 *
 */
//-----------------------------------------------------------------------------
static BOOL MSG_Main( MSG_WORK *p_wk )
{	
	return PRINTSYS_QUE_Main( p_wk->p_print_que );
}

//----------------------------------------------------------------------------
/**
 *	@brief	FONTを取得
 *
 *	@param	const MSG_WORK *cp_wk		ワーク
 *
 *	@return	FONT
 */
//-----------------------------------------------------------------------------
static GFL_FONT*	MSG_GetFont( const MSG_WORK *cp_wk )
{	
	return cp_wk->p_font;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PRINTQUEを取得
 *
 *	@param	const MSG_WORK *cp_wk		ワーク
 *
 *	@return	PRINTQUE
 */
//-----------------------------------------------------------------------------
static PRINT_QUE* MSG_GetPrintQue( const MSG_WORK *cp_wk )
{	
	return cp_wk->p_print_que;
}
//----------------------------------------------------------------------------
/**
 *	@brief	MSGDATAを取得
 *
 *	@param	const MSG_WORK *cp_wk		ワーク
 *
 *	@return	MSGDATA
 */
//-----------------------------------------------------------------------------
static const GFL_MSGDATA * MSG_GetMsgDataConst( const MSG_WORK *cp_wk )
{	
	return cp_wk->p_msg;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WORDSETを取得
 *
 *	@param	const MSG_WORK *cp_wk ワーク
 *
 *	@return	WORDSET
 */
//-----------------------------------------------------------------------------
static WORDSET * MSG_GetWordSet( const MSG_WORK *cp_wk )
{	
	return cp_wk->p_wordset;
}

//=============================================================================
/**
 *			MSGWND
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示面	初期化
 *
 *	@param	MSGWND_WORK* p_wk	ワーク
 *	@param	bgframe						表示するBG面
 *	@param	x									開始X位置（キャラ単位）
 *	@param	y									開始Y位置（キャラ単位）
 *	@param	w									幅（キャラ単位）
 *	@param	h									高さ（キャラ単位）
 *	@param	heapID						ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe, u8 x, u8 y, u8 w, u8 h, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( bgframe, x, y, w, h, TEXTSTR_PLT_NO, GFL_BMP_CHRAREA_GET_B );
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( TEXTSTR_BUFFER_LENGTH, heapID );
	PRINT_UTIL_Setup( &p_wk->print_util, p_wk->p_bmpwin );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );
}
//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示面	破棄
 *
 *	@param	MSGWND_WORK* p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_Exit( MSGWND_WORK* p_wk )
{	
	GFL_STR_DeleteBuffer( p_wk->p_strbuf );
	GFL_BMPWIN_Delete( p_wk->p_bmpwin );
	GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示面	メイン処理
 *
 *	@param	MSGWND_WORK *p_wk		ワーク
 *	@param	MSG_WORK	*p_msg		メッセージ
 *
 *	@retval  BOOL	転送が終わっている場合はTRUE／終わっていない場合はFALSE
 *
 */
//-----------------------------------------------------------------------------
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, const MSG_WORK *cp_msg )
{	
	return PRINT_UTIL_Trans( &p_wk->print_util, cp_msg->p_print_que );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示面に文字を表示
 *
 *	@param	MSGWND_WORK* p_wk	ワーク
 *	@param	MSG_WORK *cp_msg	文字管理
 *	@param	strID							文字ID
 *	@param	x									開始位置X
 *	@param	y									開始位置Y
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_Print( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y )
{	
	const GFL_MSGDATA* cp_msgdata;
	PRINT_QUE*	p_que;
	GFL_FONT*		p_font;

	cp_msgdata	= MSG_GetMsgDataConst( cp_msg );
	p_que		= MSG_GetPrintQue( cp_msg );
	p_font	= MSG_GetFont( cp_msg );

	//一端消去
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	

	//文字列作成
	GFL_MSG_GetString( cp_msgdata, strID, p_wk->p_strbuf );

	//表示
	PRINT_UTIL_Print( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示面の中央に文字を表示
 *
 *	@param	MSGWND_WORK* p_wk	ワーク
 *	@param	MSG_WORK *cp_msg	文字管理
 *	@param	strID							文字ID
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_PrintCenter( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID )
{	
	const GFL_MSGDATA* cp_msgdata;
	PRINT_QUE*	p_que;
	GFL_FONT*		p_font;
	u16 x, y;

	cp_msgdata	= MSG_GetMsgDataConst( cp_msg );
	p_que		= MSG_GetPrintQue( cp_msg );
	p_font	= MSG_GetFont( cp_msg );

	//一端消去
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	

	//文字列作成
	GFL_MSG_GetString( cp_msgdata, strID, p_wk->p_strbuf );

	//センター位置計算
	x	= GFL_BMPWIN_GetSizeX( p_wk->p_bmpwin )*4;
	y	= GFL_BMPWIN_GetSizeY( p_wk->p_bmpwin )*4;
	x	-= PRINTSYS_GetStrWidth( p_wk->p_strbuf, p_font, 0 )/2;
	y	-= PRINTSYS_GetStrHeight( p_wk->p_strbuf, p_font )/2;

	//表示
	PRINT_UTIL_Print( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font );
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ表示面に数値つき文字を表示
 *
 *	@param	MSGWND_WORK* p_wk	ワーク
 *	@param	MSG_WORK *cp_msg	文字管理
 *	@param	strID							文字ID
 *	@param	number						数値
 *	@param	buff_id						ワードセット登録バッファ
 *	@param	x									開始位置X
 *	@param	y									開始位置Y
 */
//-----------------------------------------------------------------------------
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 number, u16 buff_id, u16 x, u16 y )
{
	const GFL_MSGDATA* cp_msgdata;
	WORDSET *p_wordset;
	
	//一端消去
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	

	//モジュール取得
	p_wordset		= MSG_GetWordSet( cp_msg );
	cp_msgdata	= MSG_GetMsgDataConst( cp_msg );

	//数値をワードセットに登録
	WORDSET_RegisterNumber(	p_wordset, buff_id, number, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );

	//元の文字列に数値を適用
	{	
		STRBUF	*p_strbuf;
		p_strbuf	= GFL_MSG_CreateString( cp_msgdata, strID );
		WORDSET_ExpandStr( p_wordset, p_wk->p_strbuf, p_strbuf );
		GFL_STR_DeleteBuffer( p_strbuf );
	}

	//表示
	{	
		PRINT_QUE*	p_que;
		GFL_FONT*		p_font;	
		p_que		= MSG_GetPrintQue( cp_msg );
		p_font	= MSG_GetFont( cp_msg );
		PRINT_UTIL_Print( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	画面クリア
 *
 *	@param	MSGWND_WORK* p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_Clear( MSGWND_WORK* p_wk )
{	
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	
	GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin );
}
//=============================================================================
/**
 *				個人結果画面
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	個人結果画面	初期化
 *
 *	@param	RHYTHM_ONLYRESULT_WORK* p_wk	ワーク
 *	@param	frm									使用フレーム
 *	@param	MSG_WORK *cp_wk			メッセージ
 *	@param	heapID							ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHM_ONLYRESULT_Init( RHYTHM_ONLYRESULT_WORK* p_wk, u8 frm, const MSG_WORK *cp_msg, const RHYTHMSEARCH_WORK *cp_search,  HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(RHYTHM_ONLYRESULT_WORK) );
	p_wk->cp_msg		= cp_msg;
	p_wk->cp_search	= cp_search;

	MSGWND_Init( &p_wk->msgwnd[ROR_MSGWNDID_TITLE], frm, ROR_MSGWND_TITLE_X, ROR_MSGWND_TITLE_Y,
				ROR_MSGWND_TITLE_W, ROR_MSGWND_TITLE_H, heapID );
	MSGWND_Init( &p_wk->msgwnd[ROR_MSGWNDID_MSG], frm, ROR_MSGWND_MSG_X, ROR_MSGWND_MSG_Y,
				ROR_MSGWND_MSG_W, ROR_MSGWND_MSG_H, heapID );

	MSGWND_Init( &p_wk->msgwnd[ROR_MSGWNDID_DEBUG], frm, ROR_MSGWND_DEBUG_X, ROR_MSGWND_DEBUG_Y,
				ROR_MSGWND_DEBUG_W, ROR_MSGWND_DEBUG_H, heapID );

	MSGWND_Print( &p_wk->msgwnd[ROR_MSGWNDID_TITLE], cp_msg, RHYTHM_RES_000, 0, 0 );


	//文字列作成
	{
		u32 msg_idx;
		u32 ms_sum;
		int i;

		ms_sum	= 0;
		for( i = 1; i <10; i++ )
		{	
			ms_sum	+= cp_search->data[i].diff_ms;
		}
		ms_sum	/=	9;

		if( 40 <= ms_sum )
		{	
			msg_idx	= 0;
		}
		else if( 20 < ms_sum && ms_sum < 40 )
		{	
			msg_idx	= 1;
		}
		else if( ms_sum <= 20 )
		{	
			msg_idx	= 2;
		}
		else
		{	
			GF_ASSERT( 0 );
		}
		OS_Printf( "差の平均値 %d\n", ms_sum );

		MSGWND_Print( &p_wk->msgwnd[ROR_MSGWNDID_MSG], cp_msg, RHYTHM_RESMSG_000 + msg_idx, 0, 0 );
	}

	//デバッグ文字列作成
	{	
		MSGWND_WORK* p_msgwnd;
		PRINT_QUE*	p_que;
		GFL_FONT*		p_font;
		int i;
		u32 ms;
		u32 ms_sum;
		STRBUF *p_strbuf;

		p_msgwnd	= &p_wk->msgwnd[ROR_MSGWNDID_DEBUG];
		p_font	= GFL_FONT_Create( ARCID_FONT,
					NARC_font_small_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );		
		ms_sum	= 0;
		for( i = 1; i < 10; i++ )
		{	
			ms	= cp_search->data[i].diff_ms;
			ms_sum	+= ms;

			p_strbuf	= DEBUGPRINT_CreateWideCharNumber( L"%d回目",i, heapID ); 
			PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_msgwnd->p_bmpwin ), i*20, 0, p_strbuf, p_font );
			GFL_STR_DeleteBuffer( p_strbuf );

			p_strbuf	= DEBUGPRINT_CreateWideCharNumber( L" %d", ms, heapID ); 
			PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_msgwnd->p_bmpwin ), i*20, 10, p_strbuf, p_font );
			GFL_STR_DeleteBuffer( p_strbuf );
		}

		p_strbuf	= DEBUGPRINT_CreateWideChar( L"平均", heapID ); 
		PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_msgwnd->p_bmpwin ), i*20, 0, p_strbuf, p_font );
		GFL_STR_DeleteBuffer( p_strbuf );


		p_strbuf	= DEBUGPRINT_CreateWideCharNumber( L" %d", ms_sum/9, heapID ); 
		PRINTSYS_Print( GFL_BMPWIN_GetBmp( p_msgwnd->p_bmpwin ), i*20, 10, p_strbuf, p_font );
		GFL_STR_DeleteBuffer( p_strbuf );

		GFL_BMPWIN_MakeTransWindow( p_msgwnd->p_bmpwin );
		GFL_FONT_Delete( p_font );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	個人結果画面	破棄
 *
 *	@param	RHYTHM_ONLYRESULT_WORK* p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHM_ONLYRESULT_Exit( RHYTHM_ONLYRESULT_WORK* p_wk )
{	
	int i;
	for( i = 0; i < ROR_MSGWNDID_MAX; i++ )
	{	
		MSGWND_Exit( &p_wk->msgwnd[i] );
	}
	GFL_STD_MemClear( p_wk, sizeof(RHYTHM_ONLYRESULT_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	個人結果画面	メイン処理
 *
 *	@param	RHYTHM_ONLYRESULT_WORK* p_wk ワーク
 *
 *	@retval	TRUEならば終了
 *	@retval	FALSEならば処理中
 */
//-----------------------------------------------------------------------------
static BOOL RHYTHM_ONLYRESULT_Main( RHYTHM_ONLYRESULT_WORK* p_wk )
{	

	{	
		int i;
		for( i = 0; i < MSGWNDID_MAX; i++ )
		{	
			MSGWND_Main( &p_wk->msgwnd[i], p_wk->cp_msg );
		}
	}

	if( GFL_UI_TP_GetTrg() )
	{	
		return TRUE;
	}

	return FALSE;
}

//=============================================================================
/**
 *				SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	シーケンス変更
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	ワーク
 *	@param	seq_function					シーケンス関数
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Change( RHYTHM_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PROC終了
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( RHYTHM_MAIN_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}

//=============================================================================
/**
 *			SEQ_FUNCTION
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ゲーム開始直後
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	メインワーク
 *	@param	*p_seq								シーケンス
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_StartGame( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq )
{	
	RHYTHMSEARCH_WORK	*p_search;
	p_search	= &p_wk->search;

	MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_STR_000, 0, 0 );


#ifdef DEBUG_ONLY_PLAY
	DEBUGRHYTHM_PRINT_UpDate( p_wk );
	if( p_wk->debug_player == 0 )
	{
		p_search	= &p_wk->search;
	}
	else
	{	
		p_search	= &p_wk->search2;
		MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_DEBUG_001, 0, 0 );
	}
#endif // DEBUG_ONLY_PLAY


	RHYTHMSEARCH_Start( p_search );
	SEQ_Change( p_wk, SEQFUNC_MainGame );

}
//----------------------------------------------------------------------------
/**
 *	@brief	メイン処理
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	メインワーク
 *	@param	*p_seq								シーケンス
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_MainGame( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq )
{	
	int i;
	RHYTHMSEARCH_WORK	*p_search;
	p_search	= &p_wk->search;

#ifdef DEBUG_ONLY_PLAY
	if( p_wk->debug_player == 0 )
	{	
		p_search	= &p_wk->search;
	}
	else
	{	
		p_search	= &p_wk->search2;
	}
#endif //DEBUG_ONLY_PLAY


	TouchMarker_Main( p_wk );
	for( i = 0; i< NELEMS(sc_diamond_pos); i++ )
	{
		if( TP_GetDiamondTrg( &sc_diamond_pos[i], NULL ) )
		{	
			TouchMarker_SetPos( p_wk, &sc_diamond_pos[i] );
			RHYTHMSEARCH_SetData( p_search, &sc_diamond_pos[i] );
			break;
		}
	}

	//計測終了チェック
	if( RHYTHMSEARCH_IsEnd( p_search ) )
	{	
		MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_STR_001, 0, 0 );
		SEQ_Change( p_wk, SEQFUNC_Result );

#ifdef DEBUG_ONLY_PLAY
		DEBUGRHYTHM_PRINT_UpDate( p_wk );
		if( p_wk->debug_player == 0 )
		{	
			p_wk->debug_player	= 1;
			MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_DEBUG_001, 0, 0 );
			SEQ_Change( p_wk, SEQFUNC_StartGame );
		}
		else
		{	
			MSGWND_Print( &p_wk->msgwnd[MSGWNDID_TEXT], &p_wk->msg, RHYTHM_DEBUG_002, 0, 0 );
		SEQ_Change( p_wk, SEQFUNC_Result );
		}
#endif //DEBUG_ONLY_PLAY
	}


	if( TouchReturnBtn() )
	{
		SEQ_End( p_wk );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	結果
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	メインワーク
 *	@param	*p_seq								シーケンス
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Result( RHYTHM_MAIN_WORK *p_wk, u16 *p_seq )
{	
#ifdef DEBUG_ONLY_PLAY
	if(	GFL_UI_KEY_GetTrg() & PAD_BUTTON_A	)
	{	

		RHYTHMSEARCH_Init( &p_wk->search );
		SEQ_Change( p_wk, SEQFUNC_StartGame );

		RHYTHMSEARCH_Init( &p_wk->search2 );
		p_wk->debug_player		= 0;

	}
#else
	enum
	{	
		SEQ_SENDRESULT,
		SEQ_CALC,
		SEQ_END,
#if 0
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_ONLYRESULT_INIT,
		SEQ_ONLYRESULT_MAIN,
		SEQ_ONLYRESULT_EXIT,
		SEQ_NEXTPROC,
#endif
	};

	switch( *p_seq )
	{	
	case SEQ_SENDRESULT:
		if( RHYTHMNET_SendResultData( &p_wk->net, &p_wk->search ) )
		{	
			*p_seq	= SEQ_CALC;
		}
		break;

	case SEQ_CALC:
		p_wk->p_param->score	= CalcScore( p_wk );
	//	*p_seq	= SEQ_FADEIN_START;
		*p_seq	= SEQ_END;
		break;

	case SEQ_END:
		SEQ_End( p_wk );
		break;
#if 0
	case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_ONLYRESULT_INIT;
		}
		break;

	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_ONLYRESULT_MAIN;
		}
		break;

	case SEQ_ONLYRESULT_INIT:
		//画面上のものを消去
		GFL_BG_SetVisible( sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BACK], FALSE );
		MSGWND_Clear( &p_wk->msgwnd[MSGWNDID_TEXT] );
		MSGWND_Clear( &p_wk->msgwnd[MSGWNDID_RETURN] );

		//新たに結果画面作成
		RHYTHM_ONLYRESULT_Init( &p_wk->onlyresult, sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT], &p_wk->msg, &p_wk->search, HEAPID_IRCRHYTHM );

		*p_seq	= SEQ_FADEOUT_START;
		break;

	case SEQ_ONLYRESULT_MAIN:
		if( RHYTHM_ONLYRESULT_Main( &p_wk->onlyresult ) )
		{	
			*p_seq	= SEQ_ONLYRESULT_EXIT;
		}
		break;

	case SEQ_ONLYRESULT_EXIT:
		RHYTHM_ONLYRESULT_Exit( &p_wk->onlyresult );
		*p_seq	= SEQ_NEXTPROC;
		break;

	case SEQ_NEXTPROC:
		p_wk->is_next_proc	= TRUE;
		SEQ_End( p_wk );
		break;
#endif
	}
#endif //DEBUG_ONLY_PLAY
	TouchMarker_Main( p_wk );

}
//=============================================================================
/**
 *			RHYTHMSEARCH
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	リズムサーチ用計測	初期化
 *
 *	@param	RHYTHMSEARCH_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMSEARCH_Init( RHYTHMSEARCH_WORK *p_wk )
{	
	//クリア
	GFL_STD_MemClear( p_wk, sizeof(RHYTHMSEARCH_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	リズムサーチ用計測	破棄
 *
 *	@param	RHYTHMSEARCH_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMSEARCH_Exit( RHYTHMSEARCH_WORK *p_wk )
{	
	//破棄
	GFL_STD_MemClear( p_wk, sizeof(RHYTHMSEARCH_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	リズムサーチ用計測	計測開始
 *
 *	@param	RHYTHMSEARCH_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMSEARCH_Start( RHYTHMSEARCH_WORK *p_wk )
{	
	if( OS_IsTickAvailable() )
	{	
		OS_InitTick();
	}
	//時間計測を開始
	p_wk->start_time	=		OS_TicksToMilliSeconds(OS_GetTick());
}
//----------------------------------------------------------------------------
/**
 *	@brief	リズムサーチ用計測	計測終了判定
 *
 *	@param	RHYTHMSEARCH_WORK *p_wk		ワーク
 *
 *	@retval	TRUE計測終了
 *	@retval	FALSE計測終了
 */
//-----------------------------------------------------------------------------
static BOOL RHYTHMSEARCH_IsEnd( const RHYTHMSEARCH_WORK *cp_wk )
{	

	//終了条件１
	//タッチ回数が10回以上で、無タッチ状態が１秒以上
	if( cp_wk->data_idx >= 10 )
	{	
		if( OS_TicksToMilliSeconds(OS_GetTick()) - cp_wk->start_time 
			  - cp_wk->data[cp_wk->data_idx-1].prog_ms >= 1000 ) 
		{	
			return TRUE;
		}
	}

	//終了条件２
	//タッチ回数が15回
	if( cp_wk->data_idx == RHYTHMSEARCH_DATA_MAX )
	{	
		return TRUE;
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	リズムサーチ用計測	データ設定
 *
 *	@param	RHYTHMSEARCH_WORK *p_wk	ワーク
 *	@param	GFL_POINT *cp_pos				座標
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMSEARCH_SetData( RHYTHMSEARCH_WORK *p_wk, const GFL_POINT *cp_pos )
{	
	GF_ASSERT( p_wk->data_idx < RHYTHMSEARCH_DATA_MAX );

	p_wk->data[ p_wk->data_idx ].pos						= *cp_pos;
	p_wk->data[ p_wk->data_idx ].prog_ms				= OS_TicksToMilliSeconds(OS_GetTick())
																							  - p_wk->start_time;

	//最初は、差が0
	if( p_wk->data_idx == 0 )
	{	
		p_wk->data[ p_wk->data_idx ].diff_ms	= 0;
	}
	else	//お互いの差
	{	
		p_wk->data[ p_wk->data_idx ].diff_ms	= p_wk->data[ p_wk->data_idx ].prog_ms
																								- p_wk->data[ p_wk->data_idx-1 ].prog_ms;
	}

	//セットしたので次へ進める
	p_wk->data_idx++;
}

//=============================================================================
/**
 *							NET
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	NET初期化
 *
 *	@param	RHYTHMNET_WORK *p_wk	ワーク
 *	@param	*p_irc								赤外線システム
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMNET_Init( RHYTHMNET_WORK *p_wk, COMPATIBLE_IRC_SYS *p_irc )
{	
	GFL_STD_MemClear( p_wk, sizeof(RHYTHMNET_WORK) );
	p_wk->p_irc	= p_irc;

	COMPATIBLE_IRC_AddCommandTable( p_irc, GFL_NET_CMD_IRCRHYTHM, sc_recv_tbl, NELEMS(sc_recv_tbl), p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	NET破棄
 *
 *	@param	RHYTHMNET_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMNET_Exit( RHYTHMNET_WORK *p_wk )
{	
	COMPATIBLE_IRC_DelCommandTable( p_wk->p_irc, GFL_NET_CMD_IRCRHYTHM );
	GFL_STD_MemClear( p_wk, sizeof(RHYTHMNET_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	結果情報を相手へ送受信
 *
 *	@param	NET_WORK *p_wk	ワーク
 *	@param	NET_RESULT_DATA *cp_data	データ
 *
 *	@retval	TRUEならば、相手から受信
 *	@retval	FALSEならば受信待ち
 */
//-----------------------------------------------------------------------------
static BOOL RHYTHMNET_SendResultData( RHYTHMNET_WORK *p_wk, const RHYTHMSEARCH_WORK *cp_data )
{	
	switch( p_wk->seq )
	{	
	case 0:
		p_wk->result_send	= *cp_data;
		p_wk->seq = 1;
		NAGI_Printf( "結果データ送信開始\n" );
		break;

	case 1:
		{	
			NetID	netID;
			netID	= GFL_NET_GetNetID( GFL_NET_HANDLE_GetCurrentHandle() );
			if( netID == 0 )
			{	
				netID	= 1;
			}else{	
				netID	= 0;
			}
			if( COMPATIBLE_IRC_SendDataEx( p_wk->p_irc, NETRECV_RESULT,
					sizeof(RHYTHMSEARCH_WORK), &p_wk->result_send, netID, FALSE, FALSE, TRUE ) )
			{	
				NAGI_Printf( "結果データ送信完了、相手待ち\n" );
				p_wk->seq	= 2;
			}
		}
		break;

	case 2:
		if( p_wk->is_recv )
		{
			p_wk->is_recv	= FALSE;
			p_wk->seq	= 0;
			return TRUE;
		}
		break;
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	結果データ受け取り
 *
 *	@param	NET_WORK *p_wk	ワーク
 *	@param	*p_data							データ受け取り
 *
 */
//-----------------------------------------------------------------------------
static void RHYTHMNET_GetResultData( const RHYTHMNET_WORK *cp_wk, RHYTHMSEARCH_WORK *p_data )
{	
	*p_data	= cp_wk->result_recv;
}
//=============================================================================
/**
 *					NETRECV
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	結果データ受け取り受信コールバック
 *
 *	@param	const int netID	ネットID
 *	@param	int size				サイズ
 *	@param	void* p_data		データ
 *	@param	p_work					ワーク
 *	@param	p_net_handle		ハンドル
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void NETRECV_Result( const int netID, const int size, const void* cp_data, void* p_work, GFL_NETHANDLE* p_net_handle )
{	
	RHYTHMNET_WORK *p_wk	= p_work;

	if( p_net_handle != GFL_NET_HANDLE_GetCurrentHandle() )
	{
		return;	//自分のハンドルと一致しない場合、親としてのデータ受信なので無視する
	}
	if( netID == GFL_NET_SystemGetCurrentID() )
	{
		return;	//自分のデータは無視
	}

//	GFL_STD_MemCopy( cp_data, &p_wk->result_recv, sizeof(RHYTHMNET_RESULT_DATA) );
	NAGI_Printf("結果データ受け取り完了\n" );
	p_wk->is_recv		= TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	結果データ受け取りバッファポインタ
 *
 *	@param	netID
 *	@param	pWork
 *	@param	size
 *
 *	@return	バッファポインタ
 */
//-----------------------------------------------------------------------------
static u8* NETRECV_GetBufferAddr(int netID, void* p_work, int size)
{	
	RHYTHMNET_WORK *p_wk	= p_work;

	if( netID == GFL_NET_SystemGetCurrentID() )
	{
		return (u8*)&p_wk->result_recv_my;
	}
	else
	{	
		return (u8*)&p_wk->result_recv;
	}
}
//=============================================================================
/**
 *				汎用
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	ひし形内にTrgしたかどうか	(W＝Hの場合のみ)
 *
 *	@param	const GFL_POINT *cp_point	座標
 *	@param	w													幅
 *	@param	h													高さ
 *	@param	*p_trg										座標受け取り
 *
 *	@retval	TRUEタッチした
 *	@retval	FALSEタッチしていない
 */
//-----------------------------------------------------------------------------
static BOOL TP_GetDiamondTrg( const GFL_POINT *cp_diamond, GFL_POINT *p_trg )
{	
	u32 x, y;
	BOOL ret;

	//Cont中で、菱形内のとき
	//	当たり判定の方法は、ベクトルの右側に点があるとき〜を辺の数だけ行う
	//
	//
	if( GFL_UI_TP_GetPointTrg( &x, &y ) )
	{	
		//４頂点を作成
		GFL_POINT	top, left, right, bottom;
		int i;
		int dx, dy;
		u8 ret;
		GFL_POINT	v[5];	//左、上、右、下、左の順に頂点

		v[0].x	= cp_diamond->x - TOUCH_DIAMOND_W/2;
		v[0].y	= cp_diamond->y;
		v[1].x	= cp_diamond->x;
		v[1].y	= cp_diamond->y - TOUCH_DIAMOND_H/2;
		v[2].x	= cp_diamond->x + TOUCH_DIAMOND_W/2;
		v[2].y	= cp_diamond->y;
		v[3].x	= cp_diamond->x;
		v[3].y	= cp_diamond->y + TOUCH_DIAMOND_H/2;
		v[4]		= v[0];

		dx	= x;
		dy	= y;

		ret = 0;
		for( i = 0; i < 4; i++ )
		{	
			if( (v[i+1].x - v[i].x)*(dy-v[i].y) - (dx-v[i].x)*(v[i+1].y-v[i].y) > 0 )
			{	
				ret++;
			}
		}

		if( ret == 4)
		{
			//受け取りが存在したら代入して返す
			if( p_trg )
			{	
				p_trg->x	= x;
				p_trg->y	= y;
			}
			return TRUE;
		}

	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	タッチマーカーの表示をOFF
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	ワーク
 *
 */
//-----------------------------------------------------------------------------
static void TouchMarker_Main( RHYTHM_MAIN_WORK *p_wk )
{	
	GFL_CLWK	*p_marker;
	p_marker	= GRAPHIC_GetClwk( &p_wk->grp, CLWKID_TOUCH );

	if( GFL_CLACT_WK_GetDrawEnable( p_marker ) )
	{	
		if( p_wk->marker_cnt++ > TOUCHMARKER_VISIBLE_CNT )
		{	
			GFL_CLACT_WK_SetDrawEnable( p_marker, FALSE );
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	スコア計算
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	ワーク
 *
 *	@return	スコア
 */
//-----------------------------------------------------------------------------
static u8	CalcScore( const RHYTHM_MAIN_WORK *cp_wk )
{	
	RHYTHMSEARCH_WORK	my;
	RHYTHMSEARCH_WORK	you;

	u32	prog_score;
	u32	diff_score;

	my	= cp_wk->search;
	RHYTHMNET_GetResultData( &cp_wk->net, &you );

	OS_Printf( "■結果の表示\n" );
	//経過時間の判定
	{	
		s32 player1;
		s32 player2;
		u32	prog;
		player1	= my.data[my.data_idx-1].prog_ms - my.data[0].prog_ms;
		player2	= you.data[you.data_idx-1].prog_ms - you.data[0].prog_ms;
		prog	= MATH_IAbs(player1 - player2);
		if( prog  == RANGE_RHYTHMSCORE_PROC_00 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_00;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_01 )
		{	
			prog_score	= VAL_RHYTHMSCORE_PROC_01;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_02 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_02;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_03 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_03;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_04 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_04;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_05 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_05;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_06 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_06;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_07 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_07;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_08 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_08;
		}
		else if( prog  < RANGE_RHYTHMSCORE_PROC_09 )
		{
			prog_score	= VAL_RHYTHMSCORE_PROC_09;
		}
		else
		{
#if VAL_RHYTHMSCORE_PROC_10
			prog_score	= VAL_RHYTHMSCORE_PROC_10;
#endif 
		}
		OS_Printf( "経過時間 %d点", prog_score );
	}

	//時間の差の判定
	{	
		int i;
		s32 player1;
		s32 player2;
		u32	prog;
		diff_score	= 0;
		for( i = 1; i < 10; i++ )
		{	
			player1	= my.data[i].diff_ms;
			player2	= you.data[i].diff_ms;
			prog	= MATH_IAbs(player1 - player2);

			if( prog  == RANGE_RHYTHMSCORE_DIFF_00 )
			{	
				diff_score	+= VAL_RHYTHMSCORE_DIFF_00;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_01 )
			{	
				diff_score	+= VAL_RHYTHMSCORE_DIFF_01;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_02 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_02;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_03 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_03;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_04 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_04;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_05 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_05;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_06 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_06;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_07 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_07;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_08 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_08;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_09 )
			{
				diff_score	+= VAL_RHYTHMSCORE_DIFF_09;
			}
			else
			{
#if VAL_RHYTHMSCORE_DIFF_10
				diff_score	+= VAL_RHYTHMSCORE_DIFF_10;
#endif
			}
		}
		OS_Printf( "間隔 %d点\n", diff_score/9 );
	}
	return (prog_score + (diff_score/9) )/2;
}
//----------------------------------------------------------------------------
/**
 *	@brief	戻るボタンを押したかどうか
 *
 *	@param	void 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static BOOL TouchReturnBtn( void )
{	

	u32 x;
	u32 y;
	if( GFL_UI_TP_GetPointTrg( &x, &y) )
	{	
		if( ((u32)( x - MSGWND_RETURN_X*8) < (u32)(MSGWND_RETURN_W*8))
				&	((u32)( y - MSGWND_RETURN_Y*8) < (u32)(MSGWND_RETURN_H*8))
			){
			return TRUE;
		}

	}
	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	タッチマーカーの座標を設定し表示
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	ワーク
 *	@param	GFL_POINT *cp_pos				座標
 *
 */
//-----------------------------------------------------------------------------
static void TouchMarker_SetPos( RHYTHM_MAIN_WORK *p_wk, const GFL_POINT *cp_pos )
{	
	GFL_CLACTPOS clpos;
	GFL_CLWK	*p_marker;
	p_marker	= GRAPHIC_GetClwk( &p_wk->grp, CLWKID_TOUCH );
	clpos.x	= cp_pos->x+1;
	clpos.y	= cp_pos->y+1;
	GFL_CLACT_WK_SetPos( p_marker, &clpos, 0 );
	GFL_CLACT_WK_SetDrawEnable( p_marker, TRUE );
	p_wk->marker_cnt	= 0;
}

#ifdef DEBUG_RHYTHM_MSG
//----------------------------------------------------------------------------
/**
 *	@brief	デバッグプリントを更新
 *
 *	@param	RHYTHM_MAIN_WORK *p_wk	ワーク
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void DEBUGRHYTHM_PRINT_UpDate( RHYTHM_MAIN_WORK *p_wk )
{
	enum{	
		DEBUGMSG_TAB	= 90,
		DEBUGMSG_TAB2	= 170,
	};

	int i,j;

	RHYTHMSEARCH_WORK	*p_search;
	p_search	= &p_wk->search;

	DEBUGPRINT_Clear();

	OS_Printf( "○リズムチェック表示スタート↓↓↓↓↓\n" );
	for( j = 0; j < 2; j++ )
	{	
		if( j == 0 )
		{	
			p_search	= &p_wk->search;
		}
		else
		{	
			p_search	= &p_wk->search2;
		}

		OS_Printf( "▼%d人目の表示\n", j );
		{	
			u32 ret;

			DEBUGPRINT_Print( L"タッチ開始から終了まで", 0,  0 );
			ret	= p_search->data[p_search->data_idx-1].prog_ms - p_search->data[0].prog_ms;
			DEBUGPRINT_PrintNumber( L"ミリ秒 %d", ret, DEBUGMSG_TAB*j,  10 );
			DEBUGPRINT_PrintNumber( L"回数 %d", p_search->data_idx, 45+DEBUGMSG_TAB*j, 10 );
			OS_Printf( "継続時間 %d, タッチ回数 %d\n", ret, p_search->data_idx );
		}

		{	
			DEBUGPRINT_Print( L"タッチ間隔", 0,  30 );
			for( i = 0; i < p_search->data_idx; i++ )
			{	
				if( i == 10 )
				{	
					break;
				}
				DEBUGPRINT_PrintNumber( L"間隔[%d]", i, DEBUGMSG_TAB*j,  40+i*10 );
				DEBUGPRINT_PrintNumber( L"%d", p_search->data[i].diff_ms, 30+DEBUGMSG_TAB*j,  40+i*10 );
				OS_Printf( "間隔[%d] %d\n", i, p_search->data[i].diff_ms );
			}
		}
	}

	OS_Printf( "◇差の表示\n" );
	//差
	{	
		s32 player1;
		s32 player2;
		player1	= p_wk->search.data[p_wk->search.data_idx-1].prog_ms - p_wk->search.data[0].prog_ms;
		player2	= p_wk->search2.data[p_wk->search2.data_idx-1].prog_ms - p_wk->search2.data[0].prog_ms;
		DEBUGPRINT_PrintNumber( L"経過の差 %d", MATH_IAbs(player1 - player2), DEBUGMSG_TAB2,  10 );
		OS_Printf( "経過時間の差 %d\n", MATH_IAbs(player1 - player2) );


		for( i = 0; i < p_search->data_idx; i++ )
		{	
			if( i == 10 )
			{	
				break;
			}
			DEBUGPRINT_PrintNumber( L"差[%d]", i, DEBUGMSG_TAB2,  40+i*10 );
			player1	= p_wk->search.data[i].diff_ms;
			player2	= p_wk->search2.data[i].diff_ms;
			DEBUGPRINT_PrintNumber( L"%d", MATH_IAbs(player1-player2), 30+DEBUGMSG_TAB2,  40+i*10 );
			OS_Printf( "間隔の差[%d] %d\n", i, MATH_IAbs(player1 - player2) );
		}
	}

	OS_Printf( "■結果の表示\n" );
	//結果
	{	
		s32 player1;
		s32 player2;
		u32	prog;
		u32	score;
		player1	= p_wk->search.data[p_wk->search.data_idx-1].prog_ms - p_wk->search.data[0].prog_ms;
		player2	= p_wk->search2.data[p_wk->search2.data_idx-1].prog_ms - p_wk->search2.data[0].prog_ms;
		prog	= MATH_IAbs(player1 - player2);
		if( prog  == 0 )
		{	
			score	= 100;
		}
		else if( prog  < 500 )
		{	
			score	= 90;
		}
		else if( prog  < 1000 )
		{
			score	= 80;
		}
		else if( prog  < 3000 )
		{
			score	= 70;
		}
		else if( prog  < 5000 )
		{
			score	= 60;
		}
		else if( prog  < 10000 )
		{
			score	= 50;
		}
		else if( prog  < 15000 )
		{
			score	= 40;
		}
		else if( prog  < 20000 )
		{
			score	= 30;
		}
		else if( prog  < 25000 )
		{
			score	= 20;
		}
		else if( prog  < 30000 )
		{
			score	= 10;
		}
		else
		{
			score	= 0;
		}

		OS_Printf( "経過時間 点%d\n", score );
		score	= 0;
		for( i = 1; i < 10; i++ )
		{	
			player1	= p_wk->search.data[i].diff_ms;
			player2	= p_wk->search2.data[i].diff_ms;
			prog	= MATH_IAbs(player1 - player2);

			if( prog  == RANGE_RHYTHMSCORE_DIFF_00 )
			{	
				score	+= VAL_RHYTHMSCORE_DIFF_00;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_01 )
			{	
				score	+= VAL_RHYTHMSCORE_DIFF_01;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_02 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_02;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_03 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_03;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_04 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_04;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_05 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_05;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_06 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_06;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_07 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_07;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_08 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_08;
			}
			else if( prog  < RANGE_RHYTHMSCORE_DIFF_09 )
			{
				score	+= VAL_RHYTHMSCORE_DIFF_09;
			}
			else
			{
#if VAL_RHYTHMSCORE_DIFF_10
				score	+= VAL_RHYTHMSCORE_DIFF_10;
#endif
			}
		}
		OS_Printf( "間隔 点%d\n", score/9 );
		
	}
	
	OS_Printf( "↑リズムチェック表示終了↑↑↑↑↑\n" );

}
#endif //DEBUG_RHYTHM_MSG
 
#ifdef DEBUG_RHYTHM_MSG
//=============================================================================
/**
 *				デバッグプリント用画面
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	デバッグ用プリント領域	初期化
 *
 *	@param	frm											フレーム
 *	@param	heapID									ヒープID
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Init( u8 frm, BOOL is_now_save, HEAPID heapID )
{	
	DEBUG_PRINT_WORK *p_wk;

	sp_dp_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(DEBUG_PRINT_WORK) );
	p_wk = sp_dp_wk;
	GFL_STD_MemClear( p_wk, sizeof(DEBUG_PRINT_WORK) );
	p_wk->heapID						= heapID;
	p_wk->is_now_save				= is_now_save;
	p_wk->frm								= frm;

	//デバッグプリント用フォント
	p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
				NARC_font_small_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );	

	//退避エリアをNetEffから取得
	NetErr_GetTempArea( &p_wk->p_char_temp_area, &p_wk->p_scrn_temp_area, &p_wk->p_pltt_temp_area );
}
//----------------------------------------------------------------------------
/**
 *	@brief	デバッグ用プリント領域	破棄
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Exit( void )
{	
	DEBUG_PRINT_WORK *p_wk	= sp_dp_wk;

	GFL_FONT_Delete( p_wk->p_font );
	GFL_STD_MemClear( p_wk, sizeof(DEBUG_PRINT_WORK) );

	GFL_HEAP_FreeMemory( p_wk );
}

//----------------------------------------------------------------------------
/**
 *	@brief	デバッグ用プリント領域オープン
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Open( void )
{	
	DEBUG_PRINT_WORK *p_wk	= sp_dp_wk;

	GF_ASSERT( p_wk );

	if( p_wk->is_now_save )
	{	
		//VRAMのデータを退避
		GFL_STD_MemCopy16(GFL_DISPUT_GetCgxPtr(p_wk->frm), p_wk->p_char_temp_area, DEBUGPRINT_CHAR_TEMP_AREA);
		GFL_STD_MemCopy16(GFL_DISPUT_GetScrPtr(p_wk->frm), p_wk->p_scrn_temp_area, DEBUGPRINT_SCRN_TEMP_AREA);
		GFL_STD_MemCopy16(GFL_DISPUT_GetPltPtr(p_wk->frm), p_wk->p_pltt_temp_area, DEBUGPRINT_PLTT_TEMP_AREA);	
		//Fontカラーの退避
		GFL_FONTSYS_GetColor( &p_wk->font_col_bkup[0] ,
				&p_wk->font_col_bkup[1] ,
				&p_wk->font_col_bkup[2] );

		//もろもろ退避
		p_wk->prioryty_bkup = GFL_BG_GetPriority(p_wk->frm);
		p_wk->scroll_x_bkup = GFL_BG_GetScrollX(p_wk->frm);
		p_wk->scroll_y_bkup = GFL_BG_GetScrollY(p_wk->frm);
	}

	//上で退避させたものの設定
	GFL_BG_SetPriority( p_wk->frm , 0 );
	GFL_BG_SetScroll( p_wk->frm , GFL_BG_SCROLL_X_SET , 0 );
	GFL_BG_SetScroll( p_wk->frm , GFL_BG_SCROLL_Y_SET , 0 );

	//デバッグプリント用設定
	//スクリーンの作成
  {
    u8 x,y;
		u16 buf;
    for( y = 0;y<DEBUGPRINT_HEIGHT;y++ )
    {
      for( x=0;x<DEBUGPRINT_WIDTH;x++ )
      {
        buf = x+y*DEBUGPRINT_WIDTH;
        GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
      }
      for( x=DEBUGPRINT_WIDTH;x<32;x++ )
      {
        buf = DEBUGPRINT_HEIGHT*DEBUGPRINT_WIDTH;
        GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
      }
    }
		for( y = DEBUGPRINT_HEIGHT;y<24;y++ )
    {
	 		buf = DEBUGPRINT_HEIGHT*DEBUGPRINT_WIDTH;
 			GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
			for( x=0;x<32;x++ )
      {
        buf = DEBUGPRINT_HEIGHT*DEBUGPRINT_WIDTH;
        GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
      }
		}
    GFL_BG_LoadScreenReq( p_wk->frm );
  }
  
  //パレットの作成
  {
    u16 col[4]={ 0xFFFF , 0x0000 , 0x7fff , 0x001f };
    GFL_BG_LoadPalette( p_wk->frm, col, sizeof(u16)*4, 0 );
  }

	//書き込むためのBMP作成
	p_wk->p_bmp	= GFL_BMP_CreateInVRAM( GFL_DISPUT_GetCgxPtr(p_wk->frm), DEBUGPRINT_WIDTH, DEBUGPRINT_HEIGHT, GFL_BMP_16_COLOR, p_wk->heapID );
	GFL_STD_MemClear16( GFL_DISPUT_GetCgxPtr(p_wk->frm) , DEBUGPRINT_CHAR_TEMP_AREA );
}

//----------------------------------------------------------------------------
/**
 *	@brief	デバッグ用プリント領域	終了
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Close( void )
{	
	DEBUG_PRINT_WORK *p_wk	= sp_dp_wk;

	GF_ASSERT( p_wk );

	GFL_BMP_Delete( p_wk->p_bmp );
	if( p_wk->is_now_save )
	{	
		//もろもろ復帰
		GFL_BG_SetScroll( p_wk->frm , GFL_BG_SCROLL_X_SET , p_wk->scroll_x_bkup );
		GFL_BG_SetScroll( p_wk->frm , GFL_BG_SCROLL_Y_SET , p_wk->scroll_y_bkup );
		GFL_BG_SetPriority( p_wk->frm , p_wk->prioryty_bkup );
		//Fontカラーの復帰
		GFL_FONTSYS_SetColor( p_wk->font_col_bkup[0] ,
				p_wk->font_col_bkup[1] ,
				p_wk->font_col_bkup[2] );
		GFL_STD_MemCopy16(p_wk->p_char_temp_area, GFL_DISPUT_GetCgxPtr(p_wk->frm), DEBUGPRINT_CHAR_TEMP_AREA);
		GFL_STD_MemCopy16(p_wk->p_scrn_temp_area, GFL_DISPUT_GetScrPtr(p_wk->frm), DEBUGPRINT_SCRN_TEMP_AREA);
		GFL_STD_MemCopy16(p_wk->p_pltt_temp_area, GFL_DISPUT_GetPltPtr(p_wk->frm), DEBUGPRINT_PLTT_TEMP_AREA);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	デバッグ用プリント領域に書き込み
 *
 *	@param	u16 *cp_str							ワイド文字列
 *	@param	x												座標X
 *	@param	y												座標Y
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Print( const u16 *cp_str, u16 x, u16 y )
{	
	STRBUF	*p_strbuf;
	STRCODE	str[128];
	u16	strlen;
	DEBUG_PRINT_WORK *p_wk	= sp_dp_wk;

	GF_ASSERT(p_wk);

	//STRBUF用に変換
	strlen	= wcslen(cp_str);
	GFL_STD_MemCopy(cp_str, str, strlen*2);
	str[strlen]	= GFL_STR_GetEOMCode();

	//STRBUFに転送
	p_strbuf	= GFL_STR_CreateBuffer( strlen*2, p_wk->heapID );
	GFL_STR_SetStringCode( p_strbuf, str);

	//書き込み
	PRINTSYS_Print( p_wk->p_bmp, x, y, p_strbuf, p_wk->p_font );

	//破棄
	GFL_STR_DeleteBuffer( p_strbuf );
}

//----------------------------------------------------------------------------
/**
 *	@brief	デバッグ用プリント領域に数値つき文字書き込み
 *
 *	@param	u16 *cp_str							ワイド文字列（%dや%fを使ってください）
 *	@param	number									数字
 *	@param	x												座標X
 *	@param	y												座標Y
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_PrintNumber( const u16 *cp_str, int number, u16 x, u16 y )
{	
	u16	str[128];
	swprintf( str, 128, cp_str, number );
	DEBUGPRINT_Print( str, x, y );
}
//----------------------------------------------------------------------------
/**
 *	@brief	デバッグ用プリント領域をクリアー
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Clear( void )
{	
	DEBUG_PRINT_WORK *p_wk	= sp_dp_wk;
	GF_ASSERT(p_wk);
	GFL_BMP_Clear( p_wk->p_bmp, 0 );
}
#endif //DEBUG_RHYTHM_MSG

//----------------------------------------------------------------------------
/**
 *	@brief	Wide文字列をSTRBUFに格納
 *
 *	@param	const u16 *cp_char	ワイド文字列
 *	@param	heapID							ヒープID
 *
 *	@return	STRBUF
 */
//-----------------------------------------------------------------------------
static STRBUF * DEBUGPRINT_CreateWideChar( const u16 *cp_str, HEAPID heapID )
{	
	STRBUF	*p_strbuf;
	STRCODE	str[128];
	u16	strlen;

	//STRBUF用に変換
	strlen	= wcslen(cp_str);
	GFL_STD_MemCopy(cp_str, str, strlen*2);
	str[strlen]	= GFL_STR_GetEOMCode();

	//STRBUFに転送
	p_strbuf	= GFL_STR_CreateBuffer( strlen*2, heapID );
	GFL_STR_SetStringCode( p_strbuf, str);

	return p_strbuf;
}

//----------------------------------------------------------------------------
/**
 *	@brief	数値つきWide文字列をSTRBUFに格納
 *
 *	@param	const u16 *cp_char	ワイド文字列
 *	@param	number							数値
 *	@param	heapID							ヒープID
 *
 *	@return	STRBUF
 */
//-----------------------------------------------------------------------------
static STRBUF * DEBUGPRINT_CreateWideCharNumber( const u16 *cp_str, int number, HEAPID heapID )
{	
	u16	str[128];
	swprintf( str, 128, cp_str, number ); 
	return DEBUGPRINT_CreateWideChar( str, heapID );
}
//#endif //DEBUG_RHYTHM_MSG 
