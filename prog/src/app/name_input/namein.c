//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		namein.c
 *	@brief	名前入力画面
 *	@author	Toru=Nagihashi
 *	@data		2009.10.07
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	lib
#include <gflib.h>
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"

//	SYSTEM
#include "gamesystem/gamesystem.h"
#include "print/str_tool.h"

//	module
#include "app/app_menu_common.h"
#include "system/bmp_winframe.h"

//	archive
#include "arc_def.h"
#include "namein_gra.naix"

//	print
#include "font/font.naix"
#include "message.naix"
#include "print/gf_font.h"
#include "print/printsys.h"

//	graphic
#include "namein_graphic.h"

//	mine
#include "namein_data.h"
#include "app/name_input.h"

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///	デバッグ関係
//=====================================
#ifdef PM_DEBUG
//#define DEBUG_PRINT_ON
#endif //PM_DEBUG

#ifdef DEBUG_PRINT_ON
//ユニコードデバッグプリント
static inline void DEBUG_NAMEIN_Print( STRCODE *x , int len )
{	
	int i;	
	char str[3]	= {0};
	for( i = 0; i < len; i++ )
	{
		STD_ConvertCharUnicodeToSjis( str, x[i]);
		NAGI_Printf( "%s ", str );
	}
	NAGI_Printf("\n");
}
#else
#define DEBUG_NAMEIN_Print(x,len)	/*  */
#endif //DEBUG_PRINT_ON

//-------------------------------------
///	マクロ
//=====================================
//回り込み
#define MATH_ROUND(x,min,max)		(x < min? max: x > max ? min: x)

//-------------------------------------
///	BGフレーム
//=====================================
enum
{
	//メイン
	BG_FRAME_STR_M	= GFL_BG_FRAME0_M,		//入力中の文字列欄
	BG_FRAME_KEY_M	= GFL_BG_FRAME1_M,		//キー配列
	BG_FRAME_BTN_M	= GFL_BG_FRAME2_M,		//共通ボタン
	BG_FRAME_FONT_M	= GFL_BG_FRAME3_M,		//フォント

	//サブ
	BG_FRAME_TEXT_S	= GFL_BG_FRAME0_S,		//テキスト欄
	BG_FRAME_BACK_S	= GFL_BG_FRAME1_S,		//背景
} ;

//-------------------------------------
///	パレット
//=====================================
enum
{
	//メインBG
	PLT_BG_STR_M				= 0,	//入力文字列
	PLT_BG_KEY_NORMAL_M	= 1,	//背景基本色パレット
	PLT_BG_KEY_MOVE_M		= 2,	//背景移動時パレット
	PLT_BG_KEY_DECIDE_M	= 3,	//背景決定用パレット

	PLT_BG_FONT_M				= 15,	//フォント色

	//メインOBJ
	PLT_OBJ_CURSOR_M		= 0,	//文字列の下のバーOBJ
	PLT_OBJ_PC_M				=	1,	//PC

	//サブBG
	PLT_BG_S						= 0,
	//サブOBJ
	PLT_OBJ_S						= 0,
} ;

//-------------------------------------
///	リソースインデックス
//=====================================
typedef enum
{
	RESID_OBJ_COMMON_PLT,
	RESID_OBJ_COMMON_CHR,
	RESID_OBJ_COMMON_CEL,
	RESID_OBJ_MAX,
} RESID;

//-------------------------------------
///	OBJ
//=====================================
//バーの数
#define OBJ_BAR_NUM	(16)

//-------------------------------------
///	CLWK
//=====================================
typedef enum
{
	CLWKID_BAR_TOP,
	CLWKID_BAR_END	= CLWKID_BAR_TOP + OBJ_BAR_NUM,
	CLWKID_PC,
	CLWKID_MAX,
} CLWKID;

//-------------------------------------
///	STRINPUT
//=====================================
//文字列長
#define STRINPUT_STR_LEN			(NAMEIN_POKEMON_LENGTH)	//入力文字列長
#define STRINPUT_CHANGE_LEN		(3)											//変換文字列長
//BMPWINサイズ
#define STRINPUT_BMPWIN_X			(8)
#define STRINPUT_BMPWIN_Y			(2)
#define STRINPUT_BMPWIN_W			(14)
#define STRINPUT_BMPWIN_H			(2)
//文字描画位置
#define STRINPUT_STR_START_X	(0)
#define STRINPUT_STR_OFFSET_X	(14)
#define STRINPUT_STR_Y				(0)
//BARの位置
#define STRINPUT_BAR_START_X	(64)
#define STRINPUT_BAR_OFFSET_X	(STRINPUT_STR_OFFSET_X)
#define STRINPUT_BAR_Y				(36)
//デフォルト文字列
typedef enum
{
	STRINPUT_DEFAULT_NONE,	//デフォルトなし
	STRINPUT_DEFAULT_BOX,		//デフォルト
} STRINPUT_DEFAULT;
//特殊文字変換
typedef enum
{	
	STRINPUT_SP_CHANGE_DAKUTEN,			//濁点
	STRINPUT_SP_CHANGE_HANDAKUTEN,	//半濁点
} STRINPUT_SP_CHANGE;

//-------------------------------------
///	KEYMAP
//=====================================
//幅、高さ
#define KEYMAP_KANA_WIDTH			(13)
#define KEYMAP_KANA_HEIGHT		(5)
#define KEYMAP_QWERTY_WIDTH		(11)
#define KEYMAP_QWERTY_HEIGHT	(4)

//モード
typedef enum
{	
	NAMEIN_INPUTTYPE_KANA	= NAMEIN_KEYMAPTYPE_KANA,		//かな
	NAMEIN_INPUTTYPE_KATA	= NAMEIN_KEYMAPTYPE_KATA,		//カナ
	NAMEIN_INPUTTYPE_ABC	= NAMEIN_KEYMAPTYPE_ABC,			//ABC
	NAMEIN_INPUTTYPE_KIGOU= NAMEIN_KEYMAPTYPE_KIGOU,		//1/♪
	NAMEIN_INPUTTYPE_QWERTY	= NAMEIN_KEYMAPTYPE_QWERTY,	//ﾛｰﾏじ

	NAMEIN_INPUTTYPE_MAX	=NAMEIN_KEYMAPTYPE_MAX,
}NAMEIN_INPUTTYPE;

//キー情報
typedef enum
{	
	KEYMAP_KEYTYPE_NONE,		//なにもなし
	KEYMAP_KEYTYPE_STR,			//文字入力
	KEYMAP_KEYTYPE_KANA,		//かなモード
	KEYMAP_KEYTYPE_KATA,		//カナモード
	KEYMAP_KEYTYPE_ABC,			//アルファベットモード
	KEYMAP_KEYTYPE_KIGOU,		//記号モード
	KEYMAP_KEYTYPE_QWERTY,	//ローマ字モード
	KEYMAP_KEYTYPE_DELETE,	//けす
	KEYMAP_KEYTYPE_DECIDE,	//やめる
	KEYMAP_KEYTYPE_SHIFT,		//シフト
	KEYMAP_KEYTYPE_SPACE,		//スペース
	KEYMAP_KEYTYPE_DAKUTEN,		//濁点
	KEYMAP_KEYTYPE_HANDAKUTEN,		//半濁点
}KEYMAP_KEYTYPE;

//キー移動保存バッファ
enum
{
	//kana
	KEYMAP_KEYMOVE_BUFF_DELETE,	//消すから出るとき（上下共通）
	KEYMAP_KEYMOVE_BUFF_DICEDE,	//おわりから出るとき（上下共通）

	//qwerty
	KEYMAP_KEYMOVE_BUFF_Q_U,			//Qから上にでるとき
	KEYMAP_KEYMOVE_BUFF_A_U,			//Aから上にでるとき
	KEYMAP_KEYMOVE_BUFF_A_D,			//Aから下にでるとき
	KEYMAP_KEYMOVE_BUFF_LSHIFT_D,	//左シフトから下にでるとき
	KEYMAP_KEYMOVE_BUFF_BOU_U,		//ーから上にでるとき
	KEYMAP_KEYMOVE_BUFF_SPACE_U,	//スペースから上にでるとき
	KEYMAP_KEYMOVE_BUFF_SPACE_D,	//スペースから下にでるとき
	KEYMAP_KEYMOVE_BUFF_MAX,
};
//キー移動デフォルト値
static const u8 sc_keymove_default[KEYMAP_KEYMOVE_BUFF_MAX]	=
{	
	5,9,  0,0,1,0,7,4,4
};

//-------------------------------------
///	KEYBOARD
//=====================================
//キーボードBMPWINサイズ
#define KEYBOARD_BMPWIN_X		(3)
#define KEYBOARD_BMPWIN_Y		(5)
#define KEYBOARD_BMPWIN_W		(26)
#define KEYBOARD_BMPWIN_H		(15)
//モード切替移動
#define KYEBOARD_CHANGEMOVE_START_Y			(0)
#define KYEBOARD_CHANGEMOVE_END_Y				(-88)
#define KYEBOARD_CHANGEMOVE_SYNC				(30)
#define KYEBOARD_CHANGEMOVE_START_ALPHA	(0)
#define KYEBOARD_CHANGEMOVE_END_ALPHA		(16)
//状態
typedef enum
{	
	KEYBOARD_STATE_WAIT,	//処理待ち中
	KEYBOARD_STATE_INPUT,	//入力
	KEYBOARD_STATE_MOVE,	//移動中
}KEYBOARD_STATE;
//入力情報
typedef enum
{	
	KEYBOARD_INPUT_NONE,				//入力していない
	KEYBOARD_INPUT_STR,					//文字入力
	KEYBOARD_INPUT_CHANGESTR,		//変換文字入力
	KEYBOARD_INPUT_DAKUTEN,			//濁点
	KEYBOARD_INPUT_HANDAKUTEN,	//半濁点
	KEYBOARD_INPUT_BACKSPACE,		//一つ前に戻る
	KEYBOARD_INPUT_CHAGETYPE,		//入力タイプ変更
	KEYBOARD_INPUT_EXIT,				//終了	
	KEYBOARD_INPUT_SHIFT,				//シフト
}KEYBOARD_INPUT;

//-------------------------------------
///	KEYANM
//=====================================
//カウント
#define KEYANM_DECIDE_CNT	(10)
//何のアニメか
typedef enum
{
	KEYANM_TYPE_NONE,
	KEYANM_TYPE_MOVE,
	KEYANM_TYPE_DECIDE
} KEYANM_TYPE;


//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///	シーケンス管理
//=====================================
typedef struct _SEQ_WORK SEQ_WORK;	//関数型作るため前方宣言
typedef void (*SEQ_FUNCTION)( SEQ_WORK *p_wk, int *p_seq, void *p_param );
struct _SEQ_WORK
{
	SEQ_FUNCTION	seq_function;		//実行中のシーケンス関数
	BOOL is_end;									//シーケンスシステム終了フラグ
	int seq;											//実行中のシーケンス関数の中のシーケンス
	void *p_param;								//実行中のシーケンス関数に渡すワーク
};
//-------------------------------------
///	BGリソース管理
//=====================================
typedef struct 
{
	int dummy;
} BG_WORK;
//-------------------------------------
///	OBJリソース管理
//=====================================
typedef struct 
{
	u32					res[RESID_OBJ_MAX];		//リソース
	GFL_CLWK		*p_clwk[CLWKID_MAX];	//CLWK
} OBJ_WORK;
//-------------------------------------
///	KEYANM
//=====================================
typedef struct 
{
	u16		plt_normal[0x10];	//通常時の元のパレット
	u16		plt_flash[0x10];	//決定時の元のパレット
	u16		color[0x10];			//色バッファ
	u16		cnt[0x10];				//カウント
	u16		decide_cnt;				//決定アニメ状態カウント
	

	GFL_RECT		range;			//ボタンの矩形
	KEYANM_TYPE type;				//アニメの種類
	BOOL				is_start;		//開始フラグ
} KEYANM_WORK;

//-------------------------------------
///	STRINPUT 入力した文字欄
//=====================================
typedef struct 
{
	STRCODE						input_str[STRINPUT_STR_LEN+1];		//確定文字列
	STRCODE						change_str[STRINPUT_CHANGE_LEN+1];//変換文字列
	u16								input_idx;		//確定文字列文字数（EOMのぞく）
	u16								change_idx;		//変換文字列文字数（EOMのぞく）
	u16								strlen;				//EOMを除く最大文字列長
	u16								dummy;
	BOOL							is_update;		//文字描画アップデート
	PRINT_UTIL				util;					//文字描画UTIL
	GFL_FONT					*p_font;			//フォント
	PRINT_QUE					*p_que;				//文字キュー
	STRBUF						*p_strbuf;		//文字バッファ
	GFL_BMPWIN				*p_bmpwin;		//BMPWIN
	GFL_CLWK					*p_clwk[OBJ_BAR_NUM];		//バーのOBJ

	NAMEIN_STRCHANGE	*p_changedata[NAMEIN_STRCHANGETYPE_MAX];	//変換データ
} STRINPUT_WORK;
//-------------------------------------
///	キー配列
//=====================================
typedef struct 
{
	u8 data[KEYMAP_KEYMOVE_BUFF_MAX];
} KEYMOVE_BUFF;
typedef BOOL (*GET_KEYRECT_FUNC)( const GFL_POINT *cp_cursor, GFL_RECT *p_rect );
typedef void (*GET_KEYMOVE_FUNC)( GFL_POINT *p_now, const GFL_POINT *cp_add, KEYMOVE_BUFF *p_buff );
typedef KEYMAP_KEYTYPE (*GET_KEYTYPE_FUNC)( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos );
typedef struct 
{
	NAMEIN_KEYMAP			*p_key;	//モードによってことなるキー配列情報

	KEYMOVE_BUFF			buff;		//キー移動バッファ

	//以下関数型は、モードによって、
	//キー配列が異なる情報（キーの位置、カーソルの移動方法、キーの種類判別）を
	//関数切り替えによって実現させるための型
	GET_KEYRECT_FUNC	get_keyrect;		//キーの位置（矩形）
	GET_KEYMOVE_FUNC	get_movecursor;	//カーソルの移動
	GET_KEYTYPE_FUNC	get_keytype;		//キーの種類

	u16								w;	//配列最大WIDTH
	u16								h;	//配列最大HEIGHT
} KEYMAP_WORK;
//-------------------------------------
///	キーボード
//=====================================
typedef struct 
{
	KEYMAP_WORK			keymap;			//キー情報
	KEYANM_WORK			keyanm;			//キーアニメ
	GFL_POINT				cursor;			//カーソル位置
	GFL_BMPWIN			*p_bmpwin;	//BMPWIN
	KEYBOARD_STATE	state;			//状態
	KEYBOARD_INPUT	input;			//入力種類
	PRINT_UTIL			util;				//文字描画
	GFL_FONT				*p_font;		//フォント
	PRINT_QUE				*p_que;			//文字キュー
	STRCODE					code;				//入力したコード
	HEAPID					heapID;			//ヒープID(Main内で読みこみがあるため)
	NAMEIN_INPUTTYPE mode;			//入力モード（かな・ABCとか）
	BOOL						is_shift;		//シフトを押しているか
	//以下、モード切替演出用
	u16							change_move_cnt;	//モード切替のときのアニメカウント
	u16							change_move_seq;	//モード切替のときのシーケンス
	BOOL						is_change_anm;		//モード切替のアニメリクエスト
	NAMEIN_INPUTTYPE change_mode;			//モード切替する入力モード（かな・ABCとか）
	BOOL						is_btn_move;			//ボタンも動作するか
} KEYBOARD_WORK;
//-------------------------------------
///	メインワーク
//=====================================
typedef struct 
{
	//グラフィック設定
	NAMEIN_GRAPHIC_WORK *p_graphic;

	//OBJリソース
	OBJ_WORK			obj;

	//BGリソース
	BG_WORK				bg;

	//入力文字欄
	STRINPUT_WORK	strinput;

	//キーボード
	KEYBOARD_WORK	keyboard;

	//シーケンス
	SEQ_WORK			seq;

	//共通で使うフォント
	GFL_FONT			*p_font;

	//共通で使うキュー
	PRINT_QUE			*p_que;

	//共通で使うタスクシステム
	GFL_TCBLSYS		*p_tcbl;

	//引数
	NAMEIN_PARAM	*p_param;
} NAMEIN_WORK;
//=============================================================================
/**
 *					プロトタイプ
*/
//=============================================================================
//-------------------------------------
///	プロセス
//=====================================
static GFL_PROC_RESULT NAMEIN_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT NAMEIN_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT NAMEIN_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
///	BGリソースなど
//=====================================
static void BG_Init( BG_WORK *p_wk, HEAPID heapID );
static void BG_Exit( BG_WORK *p_wk );
//-------------------------------------
///	OBJリソースなど
//=====================================
static void OBJ_Init( OBJ_WORK *p_wk, GFL_CLUNIT *p_clunit, HEAPID heapID );
static void OBJ_Exit( OBJ_WORK *p_wk );
static GFL_CLWK *OBJ_GetClwk( const OBJ_WORK *cp_wk, CLWKID clwkID );
//-------------------------------------
///	パレットアニメ
//=====================================
static void KEYANM_Init( KEYANM_WORK *p_wk, HEAPID heapID );
static void KEYANM_Exit( KEYANM_WORK *p_wk );
static void KEYANM_Start( KEYANM_WORK *p_wk, KEYANM_TYPE type, const GFL_RECT *cp_rect );
static void KEYANM_Main( KEYANM_WORK *p_wk );
static void KEYANM_Reset( KEYANM_WORK *p_wk );
static BOOL KeyAnm_PltFade( u16 *p_buff, u16 *p_cnt, u16 add, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end );
//-------------------------------------
///	STRINPUT
//=====================================
static void STRINPUT_Init( STRINPUT_WORK *p_wk, STRINPUT_DEFAULT default_str, u8 strlen, GFL_FONT *p_font, PRINT_QUE *p_que, const OBJ_WORK *cp_obj, HEAPID heapID );
static void STRINPUT_Exit( STRINPUT_WORK *p_wk );
static void STRINPUT_Main( STRINPUT_WORK *p_wk );
static void STRINPUT_BackSpace( STRINPUT_WORK *p_wk );
static BOOL STRINPUT_SetStr( STRINPUT_WORK *p_wk, STRCODE code );
static BOOL STRINPUT_SetChangeStr( STRINPUT_WORK *p_wk, STRCODE code, BOOL is_shift );
static BOOL STRINPUT_SetChangeSP( STRINPUT_WORK *p_wk, STRINPUT_SP_CHANGE type );
static BOOL STRINPUT_PrintMain( STRINPUT_WORK *p_wk );
static void STRINPUT_CopyStr( const STRINPUT_WORK *cp_wk, STRBUF *p_strbuf );
static BOOL StrInput_ChangeStrToStr( STRINPUT_WORK *p_wk, BOOL is_shift );
//-------------------------------------
///	KEYMAP
//=====================================
static void KEYMAP_Create( KEYMAP_WORK *p_wk, NAMEIN_INPUTTYPE mode, HEAPID heapID );
static void KEYMAP_Delete( KEYMAP_WORK *p_wk );
static KEYMAP_KEYTYPE KEYMAP_GetKeyInfo( const KEYMAP_WORK *cp_wk, const GFL_POINT *cp_cursor, STRCODE *p_code );
static BOOL KEYMAP_GetTouchCursor( const KEYMAP_WORK *cp_wk, const GFL_POINT *cp_trg, GFL_POINT *p_cursor );
static void KEYMAP_MoveCursor( KEYMAP_WORK *p_wk, GFL_POINT *p_now, const GFL_POINT *cp_add );
static void KEYMAP_Print( const KEYMAP_WORK *cp_wk, PRINT_UTIL *p_util, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID );
static BOOL KEYMAP_GetRange( const KEYMAP_WORK *cp_wk, const GFL_POINT *cp_cursor, GFL_RECT *p_rect );

static BOOL KeyMap_KANA_GetRect( const GFL_POINT *cp_cursor, GFL_RECT *p_rect );
static BOOL KeyMap_QWERTY_GetRect( const GFL_POINT *cp_cursor, GFL_RECT *p_rect );
static void KeyMap_KANA_GetMoveCursor( GFL_POINT *p_now, const GFL_POINT *cp_add, KEYMOVE_BUFF *p_buff );
static void KeyMap_QWERTY_GetMoveCursor( GFL_POINT *p_now, const GFL_POINT *cp_add, KEYMOVE_BUFF *p_buff );
static KEYMAP_KEYTYPE KeyMap_KANA_GetKeyType( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos );
static KEYMAP_KEYTYPE KeyMap_KIGOU_GetKeyType( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos );
static KEYMAP_KEYTYPE KeyMap_QWERTY_GetKeyType( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos );
//-------------------------------------
///	KEYBOARD_WORK
//=====================================
static void KEYBOARD_Init( KEYBOARD_WORK *p_wk, NAMEIN_INPUTTYPE mode, GFL_FONT *p_font, PRINT_QUE *p_que, HEAPID heapID );
static void KEYBOARD_Exit( KEYBOARD_WORK *p_wk );
static void KEYBOARD_Main( KEYBOARD_WORK *p_wk );
static KEYBOARD_STATE KEYBOARD_GetState( const KEYBOARD_WORK *cp_wk );
static KEYBOARD_INPUT KEYBOARD_GetInput( const KEYBOARD_WORK *cp_wk, STRCODE *p_str );
static BOOL KEYBOARD_IsShift( const KEYBOARD_WORK *cp_wk );
static BOOL KEYBOARD_PrintMain( KEYBOARD_WORK *p_wk );
static BOOL Keyboard_StartMove( KEYBOARD_WORK *p_wk, NAMEIN_INPUTTYPE mode );
static BOOL Keyboard_MainMove( KEYBOARD_WORK *p_wk );
static void Keyboard_ChangeMode( KEYBOARD_WORK *p_wk, NAMEIN_INPUTTYPE mode );
//-------------------------------------
///	その他
//=====================================
static BOOL COLLISION_IsRectXPos( const GFL_RECT *cp_rect, const GFL_POINT *cp_pos );
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
static void SEQFUNC_WaitPrint( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_End( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
//=============================================================================
/**
 *					外部参照
*/
//=============================================================================
//-------------------------------------
///	プロセステーブル
//=====================================
const GFL_PROC_DATA NameInputProcData =
{	
	NAMEIN_PROC_Init,
	NAMEIN_PROC_Main,
	NAMEIN_PROC_Exit,
};

//@todo
NAMEIN_PARAM *NameIn_ParamAllocMake(int heapId, int mode, int info, int wordmax, CONFIG *config )
{	
	NAMEIN_PARAM *p_param;
	p_param	= GFL_HEAP_AllocMemory( heapId, sizeof(NAMEIN_PARAM) );
	GFL_STD_MemClear( p_param, sizeof(NAMEIN_PARAM) );
	p_param->strbuf	= GFL_STR_CreateBuffer( NAMEIN_POKEMON_LENGTH, heapId);

	return p_param;
}
void NameIn_ParamDelete(NAMEIN_PARAM *param)
{	
	GFL_STR_DeleteBuffer( param->strbuf );
	GFL_HEAP_FreeMemory( param );
}

//=============================================================================
/**
 *					proc
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	名前入力	メインプロセス初期化
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT NAMEIN_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	NAMEIN_WORK		*p_wk;
	NAMEIN_PARAM	*p_param;

	//引数受け取り
	p_param	= p_param_adrs;

	//ヒープ作成
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NAME_INPUT, 0x30000 );

	//プロセスワーク作成
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(NAMEIN_WORK), HEAPID_NAME_INPUT );
	GFL_STD_MemClear( p_wk, sizeof(NAMEIN_WORK) );

	//初期化
	p_wk->p_param	= p_param;

	//共通モジュールの作成
	p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_NAME_INPUT );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_NAME_INPUT );
	p_wk->p_tcbl		= GFL_TCBL_Init( HEAPID_NAME_INPUT, HEAPID_NAME_INPUT, 32, 32 );

	//グラフィック設定
	p_wk->p_graphic	= NAMEIN_GRAPHIC_Init( 0, HEAPID_NAME_INPUT );

	//読みこみ
	BG_Init( &p_wk->bg, HEAPID_NAME_INPUT );
	{	
		GFL_CLUNIT	*p_clunit	= NAMEIN_GRAPHIC_GetClunit( p_wk->p_graphic );
		OBJ_Init( &p_wk->obj, p_clunit, HEAPID_NAME_INPUT );
	}

	//モジュール作成
	SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_WaitPrint );
	STRINPUT_Init( &p_wk->strinput, STRINPUT_DEFAULT_NONE, NAMEIN_POKEMON_LENGTH, p_wk->p_font, p_wk->p_que, &p_wk->obj, HEAPID_NAME_INPUT );
	KEYBOARD_Init( &p_wk->keyboard, NAMEIN_INPUTTYPE_KANA, p_wk->p_font, p_wk->p_que, HEAPID_NAME_INPUT );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	名前入力	メインプロセス破棄
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT NAMEIN_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	NAMEIN_WORK	*p_wk	= p_wk_adrs;


	//モジュール破棄
	KEYBOARD_Exit( &p_wk->keyboard );
	STRINPUT_Exit( &p_wk->strinput );
	SEQ_Exit( &p_wk->seq );

	//グラフィック破棄
	OBJ_Exit( &p_wk->obj );
	BG_Exit( &p_wk->bg );
	NAMEIN_GRAPHIC_Exit( p_wk->p_graphic );

	//共通モジュールの破棄
	GFL_TCBL_Exit( p_wk->p_tcbl );
	PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_font );

	//プロセスワーク破棄
	GFL_PROC_FreeWork( p_proc );
	//ヒープ破棄
	GFL_HEAP_DeleteHeap( HEAPID_NAME_INPUT );

	
	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	名前入力	メインプロセスメイン処理
 *
 *	@param	GFL_PROC *p_proc	プロセス
 *	@param	*p_seq						シーケンス
 *	@param	*p_param					親ワーク
 *	@param	*p_work						ワーク
 *
 *	@return	終了コード
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT NAMEIN_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	NAMEIN_WORK	*p_wk	= p_wk_adrs;

	//シーケンス
	SEQ_Main( &p_wk->seq );

	//描画
	NAMEIN_GRAPHIC_2D_Draw( p_wk->p_graphic );

	//プリント
	PRINTSYS_QUE_Main( p_wk->p_que );

	//タスク
	GFL_TCBL_Main( p_wk->p_tcbl );	

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
	ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_NAMEIN_GRA, heapID );

	//PLT
	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_namein_gra_name_bg_NCLR,
			PALTYPE_MAIN_BG, PLT_BG_STR_M*0x20, 0, heapID );
	GFL_ARCHDL_UTIL_TransVramPaletteEx( p_handle, NARC_namein_gra_name_bg_NCLR,
			PALTYPE_MAIN_BG, PLT_BG_KEY_MOVE_M*0x20, PLT_BG_KEY_DECIDE_M*0x20, 0x20, heapID );

	//CHR
	GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_namein_gra_name_bg_NCGR, 
			BG_FRAME_STR_M, 0, 0, FALSE, heapID );	

	//SCR
	GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_namein_gra_name_base_NSCR,
			BG_FRAME_STR_M, 0, 0, FALSE, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_namein_gra_name_kana_NSCR,
			BG_FRAME_KEY_M, 0, 0, FALSE, heapID );
	GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_namein_gra_name_btn_NSCR,
			BG_FRAME_BTN_M, 0, 0, FALSE, heapID );
	

	GFL_ARC_CloseDataHandle( p_handle );

	//フォントパレット
	p_handle	= GFL_ARC_OpenDataHandle( ARCID_FLDMAP_WINFRAME, heapID );
	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, BmpWinFrame_WinPalArcGet(),
			PALTYPE_MAIN_BG, PLT_BG_FONT_M*0x20, 0, heapID );
	GFL_ARC_CloseDataHandle( p_handle );
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

}
//----------------------------------------------------------------------------
/**
 *	@brief	BGパレットフェード
 *
 *	@param	u16 *p_buff	色保存バッファ
 *	@param	*p_cnt			カウンタバッファ
 *	@param	add					カウンタ加算値
 *	@param	plt_num			パレット縦番号
 *	@param	plt_col			パレット横番号
 *	@param	start				開始色
 *	@param	end					終了色
 */
//-----------------------------------------------------------------------------
static void BG_MainPltAnm( u16 *p_buff, u16 *p_cnt, u16 add, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end )
{	
	//パレットアニメ
  if( *p_cnt + add >= 0x10000 )
  {
    *p_cnt = *p_cnt+add-0x10000;
  }
  else
  {
    *p_cnt += add;
  }
  {
    //1〜0に変換
    const fx16 cos = (FX_CosIdx(*p_cnt)+FX16_ONE)/2;
		const u8 start_r	= (start & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
		const u8 start_g	= (start & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
		const u8 start_b	= (start & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
		const u8 end_r	= (end & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
		const u8 end_g	= (end & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
		const u8 end_b	= (end & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

    const u8 r = start_r + (((end_r-start_r)*cos)>>FX16_SHIFT);
    const u8 g = start_g + (((end_g-start_g)*cos)>>FX16_SHIFT);
    const u8 b = start_b + (((end_b-start_b)*cos)>>FX16_SHIFT);
    
		*p_buff = GX_RGB(r, g, b);
    
    NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN,
                                        plt_num * 32 + plt_col * 2,
                                        p_buff, 2 );
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
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_NAMEIN_GRA, heapID );

		p_wk->res[ RESID_OBJ_COMMON_PLT ]	= GFL_CLGRP_PLTT_Register( p_handle, 
				NARC_namein_gra_name_obj_NCLR, CLSYS_DRAW_MAIN, PLT_OBJ_CURSOR_M*0x20, heapID );
		p_wk->res[ RESID_OBJ_COMMON_CEL ]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_namein_gra_name_obj_NCER, NARC_namein_gra_name_obj_NANR, heapID );
		p_wk->res[ RESID_OBJ_COMMON_CHR ]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_namein_gra_name_obj_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}
	//CLWK登録
	{
		int i;
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.anmseq	= 0;
		for( i = CLWKID_BAR_TOP; i < CLWKID_BAR_END; i++ )
		{	
			p_wk->p_clwk[i]	=		GFL_CLACT_WK_Create( p_clunit,
					p_wk->res[RESID_OBJ_COMMON_CHR],
					p_wk->res[RESID_OBJ_COMMON_PLT],
					p_wk->res[RESID_OBJ_COMMON_CEL],
					&cldata,
					CLSYS_DEFREND_MAIN,
					heapID );
		}

		cldata.anmseq	= 1;
		p_wk->p_clwk[CLWKID_PC]	=		GFL_CLACT_WK_Create( p_clunit,
				p_wk->res[RESID_OBJ_COMMON_CHR],
				p_wk->res[RESID_OBJ_COMMON_PLT],
				p_wk->res[RESID_OBJ_COMMON_CEL],
				&cldata,
				CLSYS_DEFREND_MAIN,
				heapID );
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[CLWKID_PC], FALSE );
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
		for( i = 0; i < CLWKID_MAX; i++ )
		{	
			if( p_wk->p_clwk[i] )
			{	
				GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
			}
		}
	}

	//リソース破棄
	{	
		GFL_CLGRP_PLTT_Release( p_wk->res[RESID_OBJ_COMMON_PLT] );
		GFL_CLGRP_CGR_Release( p_wk->res[RESID_OBJ_COMMON_CHR] );
		GFL_CLGRP_CELLANIM_Release( p_wk->res[RESID_OBJ_COMMON_CEL] );
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
static GFL_CLWK *OBJ_GetClwk( const OBJ_WORK *cp_wk, CLWKID clwkID )
{	
	return cp_wk->p_clwk[ clwkID ];
}
//=============================================================================
/**
 *					KEYANM
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	キーアニメ	初期化
 *
 *	@param	KEYANM_WORK *p_wk	ワーク
 *	@param	heapID						ヒープID
 */
//-----------------------------------------------------------------------------
static void KEYANM_Init( KEYANM_WORK *p_wk, HEAPID heapID )
{	
	void *p_buff;
	NNSG2dPaletteData	*p_plt;

	//クリア
	GFL_STD_MemClear( p_wk, sizeof(KEYANM_WORK) );

	//もとのパレットから色情報を保存
	p_buff	= GFL_ARC_UTIL_LoadPalette( ARCID_NAMEIN_GRA, NARC_namein_gra_name_bg_NCLR, &p_plt, heapID );

	{	
		int i;
		const u16 *cp_plt_adrs;

		cp_plt_adrs	= p_plt->pRawData;
		//１ライン目
		for( i = 0; i < 0x10; i++ )
		{	
			p_wk->plt_normal[i]	= cp_plt_adrs[i+0x10];
		}
		//２ライン目
		for( i = 0; i < 0x10; i++ )
		{	
			p_wk->plt_flash[i]	= cp_plt_adrs[i+0x20];
		}
	}

	//パレット破棄
	GFL_HEAP_FreeMemory( p_buff );
}
//----------------------------------------------------------------------------
/**
 *	@brief	キーアニメ	破棄
 *
 *	@param	KEYANM_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void KEYANM_Exit( KEYANM_WORK *p_wk )
{	
	//クリア
	GFL_STD_MemClear( p_wk, sizeof(KEYANM_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	キーアニメ	開始
 *
 *	@param	KEYANM_WORK *p_wk		ワーク
 *	@param	type								移動タイプ
 *	@param	GFL_RECT *cp_rect		矩形
 */
//-----------------------------------------------------------------------------
static void KEYANM_Start( KEYANM_WORK *p_wk, KEYANM_TYPE type, const GFL_RECT *cp_rect )
{	
	p_wk->range	= *cp_rect;
	p_wk->type	= type;

	//他のキーを全て元に戻す
	GFL_BG_ChangeScreenPalette( BG_FRAME_BTN_M, 0, 0, 32, 24, PLT_BG_KEY_NORMAL_M );
	GFL_BG_ChangeScreenPalette( BG_FRAME_KEY_M, 0, 0, 32, 24, PLT_BG_KEY_NORMAL_M );

	//キーの色を張替え
	GFL_BG_ChangeScreenPalette( BG_FRAME_BTN_M, cp_rect->left, cp_rect->top,
			cp_rect->right - cp_rect->left, cp_rect->bottom - cp_rect->top,
			PLT_BG_KEY_NORMAL_M + type );
	GFL_BG_ChangeScreenPalette( BG_FRAME_KEY_M, cp_rect->left, cp_rect->top,
			cp_rect->right - cp_rect->left, cp_rect->bottom - cp_rect->top,
			PLT_BG_KEY_NORMAL_M + type );

	//スクリーンリクエスト
	GFL_BG_LoadScreenReq( BG_FRAME_BTN_M );
	GFL_BG_LoadScreenReq( BG_FRAME_KEY_M );

	//カウントバッファクリア
	GFL_STD_MemClear( p_wk->cnt, sizeof(u16)*0x10 );
	p_wk->decide_cnt	= 0;

	p_wk->is_start	= TRUE;

}
//----------------------------------------------------------------------------
/**
 *	@brief	キーアニメ	メイン処理
 *
 *	@param	KEYANM_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void KEYANM_Main( KEYANM_WORK *p_wk )
{	
	u8 plt_num;
	u16 add;
	int i;
	BOOL is_end;

	if( p_wk->is_start )
	{	
		//モード別処理
		switch( p_wk->type )
		{	
		case KEYANM_TYPE_MOVE:	
			plt_num	= PLT_BG_KEY_MOVE_M;
			add			= 0x400;
			break;

		case KEYANM_TYPE_DECIDE:
			plt_num	= PLT_BG_KEY_DECIDE_M;
			add			= 0xFFFF/10;
			//終了したら自分でモード切替
			if( p_wk->decide_cnt++ > KEYANM_DECIDE_CNT )
			{	
				KEYANM_Start( p_wk, KEYANM_TYPE_MOVE, &p_wk->range );
			}
			break;
		}

		//それぞれのパレットをアニメ
		for( i = 0; i < 0x10; i++ )
		{	
			KeyAnm_PltFade( &p_wk->color[i], &p_wk->cnt[i], add, plt_num, i, p_wk->plt_normal[i], p_wk->plt_flash[i] );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	キーアニメ	リセット
 *
 *	@param	KEYANM_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void KEYANM_Reset( KEYANM_WORK *p_wk )
{	

}
//----------------------------------------------------------------------------
/**
 *	@brief	キーアニメ	フェード
 *
 *	@param	u16 *p_buff	色保存バッファ（VBlank転送のため）
 *	@param	*p_cnt			カウンタバッファ
 *	@param	add					カウンタ加算値
 *	@param	plt_num			パレット縦番号
 *	@param	plt_col			パレット横番号
 *	@param	start				開始色
 *	@param	end					終了色
 *
 *	@retval	TRUEでENDになった。　FALSEでまだ	（ENDで止まらず、ループします）
 */
//-----------------------------------------------------------------------------
static BOOL KeyAnm_PltFade( u16 *p_buff, u16 *p_cnt, u16 add, u8 plt_num, u8 plt_col, GXRgb start, GXRgb end )
{	
	//パレットアニメ
  if( *p_cnt + add >= 0x10000 )
  {
    *p_cnt = *p_cnt+add-0x10000;
  }
  else
  {
    *p_cnt += add;
  }
  {
		BOOL ret;
    //1〜0に変換
    const fx16 cos = (FX_CosIdx(*p_cnt)+FX16_ONE)/2;
		const u8 start_r	= (start & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
		const u8 start_g	= (start & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
		const u8 start_b	= (start & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;
		const u8 end_r	= (end & GX_RGB_R_MASK ) >> GX_RGB_R_SHIFT;
		const u8 end_g	= (end & GX_RGB_G_MASK ) >> GX_RGB_G_SHIFT;
		const u8 end_b	= (end & GX_RGB_B_MASK ) >> GX_RGB_B_SHIFT;

    const u8 r = start_r + (((end_r-start_r)*cos)>>FX16_SHIFT);
    const u8 g = start_g + (((end_g-start_g)*cos)>>FX16_SHIFT);
    const u8 b = start_b + (((end_b-start_b)*cos)>>FX16_SHIFT);
    
		*p_buff = GX_RGB(r, g, b);
    
    ret = NNS_GfdRegisterNewVramTransferTask( NNS_GFD_DST_2D_BG_PLTT_MAIN ,
                                        plt_num * 32 + plt_col *2 ,
                                        p_buff, 2 );

		GF_ASSERT( ret );
  }

	return *p_buff == end;
}
//=============================================================================
/**
 *					STRINPUT
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	入力文字欄	初期化
 *
 *	@param	STRINPUT_WORK *p_wk		ワーク
 *	@param	default_str						デフォルト文字列
 *	@param	strlen								文字列長
 *	@param	p_font								フォント
 *	@param	p_que									プリントキュー
 *	@param	cp_obj								clwk取得用OBJワーク
 *	@param	heapID								ヒープID
 */
//-----------------------------------------------------------------------------
static void STRINPUT_Init( STRINPUT_WORK *p_wk, STRINPUT_DEFAULT default_str, u8 strlen, GFL_FONT *p_font, PRINT_QUE *p_que, const OBJ_WORK *cp_obj, HEAPID heapID )
{	
	GF_ASSERT( strlen <= STRINPUT_STR_LEN );

	//クリア
	GFL_STD_MemClear( p_wk, sizeof(STRINPUT_WORK) );
	p_wk->p_font	= p_font;
	p_wk->strlen	= strlen;
	p_wk->p_que		= p_que;

	//CLWK取得
	{	 
		int i;
		for( i = 0; i < OBJ_BAR_NUM; i++ )
		{	
			p_wk->p_clwk[ i ]	= OBJ_GetClwk( cp_obj, CLWKID_BAR_TOP + i );
		}
	}

	//@todo　デフォルトネーム処理
	p_wk->input_str[0]	= GFL_STR_GetEOMCode();

	//BMPWIN作成
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( BG_FRAME_STR_M, STRINPUT_BMPWIN_X, STRINPUT_BMPWIN_Y, STRINPUT_BMPWIN_W, STRINPUT_BMPWIN_H, PLT_BG_FONT_M, GFL_BMP_CHRAREA_GET_B );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

	//プリントキュー設定
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

	//文字バッファ
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( STRINPUT_STR_LEN + STRINPUT_CHANGE_LEN + 1, heapID );

	//検索用データ読みこみ
	{	
		int i;
		for( i = 0; i < NAMEIN_STRCHANGETYPE_MAX; i++ )
		{	
			p_wk->p_changedata[i]	= NAMEIN_STRCHANGE_Alloc( i, heapID );
		}
	}

	//CLWK設定
	{	 
		int i;
		u16 x;
		GFL_CLACTPOS	clpos;
		clpos.y	= STRINPUT_BAR_Y;
		x	= STRINPUT_BMPWIN_W * 8 / 2 - (p_wk->strlen * STRINPUT_STR_OFFSET_X) /2;	//開始
		for( i = 0; i < OBJ_BAR_NUM; i++ )
		{	
			clpos.x	= STRINPUT_BAR_START_X + x + i * STRINPUT_BAR_OFFSET_X;

			GFL_CLACT_WK_SetAnmSeq( p_wk->p_clwk[i], 0 );
			GFL_CLACT_WK_SetAutoAnmFlag( p_wk->p_clwk[i], TRUE );
			GFL_CLACT_WK_StopAnm( p_wk->p_clwk[i] );
			GFL_CLACT_WK_SetPos( p_wk->p_clwk[i], &clpos, CLSYS_DEFREND_MAIN );
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
		}
		for( i = p_wk->strlen; i < OBJ_BAR_NUM; i++ )
		{	
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
		}

		GFL_CLACT_WK_StartAnm( p_wk->p_clwk[ p_wk->input_idx ] );


	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	入力文字欄	破棄
 *
 *	@param	STRINPUT_WORK *p_wk ワーク
 *
 */
//-----------------------------------------------------------------------------
static void STRINPUT_Exit( STRINPUT_WORK *p_wk )
{	
	//検索用データ破棄
	{	
		int i;
		for( i = 0; i < NAMEIN_STRCHANGETYPE_MAX; i++ )
		{	
			NAMEIN_STRCHANGE_Free( p_wk->p_changedata[i] );
		}
	}

	//文字バッファ破棄
	GFL_STR_DeleteBuffer( p_wk->p_strbuf );
	
	//BMPWIN破棄
	GFL_BMPWIN_Delete( p_wk->p_bmpwin );

	//クリア
	GFL_STD_MemClear( p_wk, sizeof(STRINPUT_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	入力文字欄	メイン処理
 *
 *	@param	STRINPUT_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void STRINPUT_Main( STRINPUT_WORK *p_wk )
{	
	int j;
	if( p_wk->is_update )
	{	
		STRCODE	buff[	STRINPUT_STR_LEN + STRINPUT_CHANGE_LEN + 1 ];

		//確定文字列があれば連結
		if( p_wk->input_idx > 0 )
		{	
			STRTOOL_Copy( p_wk->input_str, buff, p_wk->input_idx + 1 );
		}
		//変換文字列があれば、後尾に連結
		if( p_wk->change_idx > 0 )
		{	
			STRTOOL_Copy( p_wk->change_str, &buff[p_wk->input_idx], p_wk->change_idx + 1 );
		}

		//連結した文字数があれば、文字描画
		//なければクリア
		if( p_wk->input_idx + p_wk->change_idx > 0 )
		{	
			int i;
			STRCODE code[2];
			u16 x;
	
			//一端クリア
			GFL_BMP_Clear(GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );

			//等幅にするために1文字ずつプリント
			for( i = 0; i < p_wk->input_idx + p_wk->change_idx; i++ )
			{	
				code[0]	= buff[i];
				code[1]	= GFL_STR_GetEOMCode();

				GFL_STR_SetStringCodeOrderLength( p_wk->p_strbuf, code, 2 );
				x	= STRINPUT_BMPWIN_W * 8 / 2 - (p_wk->strlen * STRINPUT_STR_OFFSET_X) /2;	//開始
				x	+= i * STRINPUT_STR_OFFSET_X + STRINPUT_BAR_OFFSET_X/2;								//幅
				x -= PRINTSYS_GetStrWidth( p_wk->p_strbuf, p_wk->p_font, 0 )/2;

				PRINT_UTIL_Print( &p_wk->util, p_wk->p_que, x, STRINPUT_STR_Y, p_wk->p_strbuf, p_wk->p_font );
			}
		}
		else
		{	
			GFL_BMP_Clear(GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );
			GFL_BMPWIN_TransVramCharacter(p_wk->p_bmpwin);
		}

		//バーのアニメを変更
		{	
			int i;
			for( i = 0; i < p_wk->strlen; i++ )
			{	
				GFL_CLACT_WK_SetAnmFrame( p_wk->p_clwk[i], 0 );
				GFL_CLACT_WK_StopAnm( p_wk->p_clwk[i] );
				GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
			}
			for( ; i < OBJ_BAR_NUM; i++ )
			{	
				GFL_CLACT_WK_SetAnmFrame( p_wk->p_clwk[i], 0 );
				GFL_CLACT_WK_StopAnm( p_wk->p_clwk[i] );
				GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], FALSE );
			}
			for(i = 0; i <= p_wk->input_idx && i < p_wk->strlen; i++ )
			{	
				GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk[i], TRUE );
			}
			GFL_CLACT_WK_StartAnm( p_wk->p_clwk[ p_wk->input_idx ] );
		}

		p_wk->is_update	= FALSE;
	}
	
	STRINPUT_PrintMain( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	入力文字欄	1文字消去
 *
 *	@param	STRINPUT_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void STRINPUT_BackSpace( STRINPUT_WORK *p_wk )
{	
	//変換中ならば、変換中の文字を
	//そうでないならば、普通に消去
	if( p_wk->change_idx > 0 )
	{	
		p_wk->change_idx--;
		p_wk->change_str[ p_wk->change_idx ] = GFL_STR_GetEOMCode();
		p_wk->is_update	= TRUE;
	}
	else
	{	
		if( p_wk->input_idx > 0 )
		{	
			p_wk->input_idx--;
			p_wk->input_str[ p_wk->input_idx ] = GFL_STR_GetEOMCode();
			p_wk->is_update	= TRUE;
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	入力文字欄	文字を設定
 *
 *	@param	STRINPUT_WORK *p_wk ワーク
 *	@param	STRCODE	code
 */
//-----------------------------------------------------------------------------
static BOOL STRINPUT_SetStr( STRINPUT_WORK *p_wk, STRCODE code )
{	
	int j;

	//入力できるか
	if( p_wk->input_idx < p_wk->strlen )
	{	
		p_wk->input_str[ p_wk->input_idx ] = code;
		p_wk->input_idx++;
		p_wk->input_str[ p_wk->input_idx ] = GFL_STR_GetEOMCode();

		OS_Printf( "\nSetStr\n " );
		DEBUG_NAMEIN_Print(p_wk->input_str,p_wk->input_idx);

		p_wk->is_update	= TRUE;

		return TRUE;
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	入力文字欄	変換文字を設定
 *
 *	@param	STRINPUT_WORK *p_wk ワーク
 *	@param	code								変換文字列にいれる文字
 *	@param	is_shift						シフトが入力されているかどうか
 *
 *	@retval TRUE　文字を変換した　FALSEしていない
 */
//-----------------------------------------------------------------------------
static BOOL STRINPUT_SetChangeStr( STRINPUT_WORK *p_wk, STRCODE code, BOOL is_shift )
{	
	//入力できるか
	if( p_wk->change_idx < STRINPUT_CHANGE_LEN )
	{	
		p_wk->change_str[ p_wk->change_idx ] = code;
		p_wk->change_idx++;
		p_wk->change_str[ p_wk->change_idx ] = GFL_STR_GetEOMCode();

		//変換バッファから確定バッファへの変換処理
		StrInput_ChangeStrToStr( p_wk, is_shift );
		
		p_wk->is_update	= TRUE;
		return TRUE;
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	入力文字欄	濁点、半濁点を変換する
 *
 *	@param	STRINPUT_WORK *p_wk ワーク
 *
 *	@retval TRUE　文字を変換した　FALSEしていない
 */
//-----------------------------------------------------------------------------
static BOOL STRINPUT_SetChangeSP( STRINPUT_WORK *p_wk, STRINPUT_SP_CHANGE type )
{	
	NAMEIN_STRCHANGE	*p_data;

	switch( type )
	{	
	case STRINPUT_SP_CHANGE_DAKUTEN:			//濁点
		//検索文字サーチ
		p_data	=  p_wk->p_changedata[NAMEIN_STRCHANGETYPE_DAKUTEN];
		break;
	case STRINPUT_SP_CHANGE_HANDAKUTEN:	//半濁点
		//検索文字サーチ
		p_data	=  p_wk->p_changedata[NAMEIN_STRCHANGETYPE_HANDAKUTEN];
		break;
	default:
		GF_ASSERT(0);
		return FALSE;
	}

	//確定文字列の一番後ろを見て、変換可能だったら、変える
	if( 0 < p_wk->input_idx )
	{	
		u16 idx;
		STRCODE	code[NAMEIN_STRCHANGE_CODE_LEN+1];
		u8 len;
		STRCODE	delete[NAMEIN_STRCHANGE_CODE_LEN+1];
		u8 delete_len;
		int i;

		idx	= NAMEIN_STRCHANGE_GetIndexByInputStr( p_data, &p_wk->input_str[ p_wk->input_idx-1 ] );	
		if( idx != NAMEIN_STRCHANGE_NONE )
		{	
			if( NAMEIN_STRCHANGE_GetChangeStr( p_data, idx, code, NAMEIN_STRCHANGE_CODE_LEN+1, &len ) )
			{	
				//確定バッファから削除
				NAMEIN_STRCHANGE_GetInputStr( p_data, idx, delete, NAMEIN_STRCHANGE_CODE_LEN+1, &delete_len );
				//確定バッファに追加
				for( i = 0; i < delete_len; i++ )
				{	
					STRINPUT_BackSpace( p_wk );
				}

				//確定バッファに追加
				for( i = 0; i < len; i++ )
				{	
					STRINPUT_SetStr( p_wk, code[i] );
				}
				return TRUE;
			}
		}
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	入力文字欄	描画メイン
 *
 *	@param	STRINPUT_WORK *p_wk		ワーク
 *
 * @retval  BOOL  転送が終わっている場合はTRUE／終わっていない場合はFALSE
 */
//-----------------------------------------------------------------------------
static BOOL STRINPUT_PrintMain( STRINPUT_WORK *p_wk )
{	
	return PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
}
//----------------------------------------------------------------------------
/**
 *	@brief	入力文字欄	入力した文字列をコピー
 *
 *	@param	const STRINPUT_WORK *cp_wk	ワーク
 *	@param	*p_strbuf										STRBUF
 */
//-----------------------------------------------------------------------------
static void STRINPUT_CopyStr( const STRINPUT_WORK *cp_wk, STRBUF *p_strbuf )
{	
	GFL_STR_SetStringCodeOrderLength( p_strbuf, cp_wk->input_str, cp_wk->input_idx + 1 );
}
//----------------------------------------------------------------------------
/**
 *	@brief	変換文字列から確定文字列への変換
 *
 *	@param	STRINPUT_WORK *p_wk		ワーク
 *	@param	is_shift							シフトが入力されているかどうか
 *
 *	@return	TRUEで変換した	FALSEで変換しなかった
 */
//-----------------------------------------------------------------------------
static BOOL StrInput_ChangeStrToStr( STRINPUT_WORK *p_wk, BOOL is_shift )
{	
		//変換処理
	int i,j;
	u16	idx;
	u8  len;
	STRCODE	code[NAMEIN_STRCHANGE_CODE_LEN+1];

	BOOL (*GetChangeStr)( const NAMEIN_STRCHANGE *cp_wk, u16 idx, STRCODE *p_code, u8 code_len, u8 *p_len );
	
	//シフト入力によって関数が違う
	if( is_shift )
	{	
		GetChangeStr	= NAMEIN_STRCHANGE_GetChangeShiftStr;
	}
	else
	{	
		GetChangeStr	= NAMEIN_STRCHANGE_GetChangeStr;
	}


	for( i = 0; i < p_wk->change_idx; i++ )
	{	
		//検索文字サーチ
		idx	= NAMEIN_STRCHANGE_GetIndexByInputStr( p_wk->p_changedata[NAMEIN_STRCHANGETYPE_QWERTY], &p_wk->change_str[ i ] );	
		if( idx != NAMEIN_STRCHANGE_NONE )
		{	
			//変換してた文字列の前の文字も確定文字列にする
			NAGI_Printf( "変換せず確定する文字列 長さ%d\n", i );
			DEBUG_NAMEIN_Print(p_wk->change_str,i);
			for( j = 0; j < i; j++ )
			{	
				STRINPUT_SetStr( p_wk, p_wk->change_str[ j ] );
			}	
			for( j = 0; j < i; j++ )
			{	
				STRINPUT_BackSpace( p_wk );
			}

			//置換し確定バッファに送る
			if( GetChangeStr( p_wk->p_changedata[NAMEIN_STRCHANGETYPE_QWERTY], idx, code, NAMEIN_STRCHANGE_CODE_LEN+1, &len ) )
			{	
				NAGI_Printf( "変換後文字列 長さ%d\n", len );
				DEBUG_NAMEIN_Print(code,len);
				for( j = 0; j < len; j++ )
				{	
					STRINPUT_SetStr( p_wk, code[j] );
				}
				//置換したので、検索バッファから削除
				NAMEIN_STRCHANGE_GetInputStr( p_wk->p_changedata[NAMEIN_STRCHANGETYPE_QWERTY], idx, code, NAMEIN_STRCHANGE_CODE_LEN+1, &len );

				NAGI_Printf( "消す文字列 長さ%d\n", len );
				DEBUG_NAMEIN_Print(code,len);
				for( j = 0; j < len; j++ )
				{	
					STRINPUT_BackSpace( p_wk );
				}

				//残る文字を追加
				NAMEIN_STRCHANGE_GetRestStr( p_wk->p_changedata[NAMEIN_STRCHANGETYPE_QWERTY], idx, code, NAMEIN_STRCHANGE_CODE_LEN+1, &len );	
				NAGI_Printf( "残る文字列 長さ%d\n", len );
				DEBUG_NAMEIN_Print(code,len);
				for( j = 0; j < len; j++ )
				{	
					p_wk->change_str[ p_wk->change_idx ] = code[j];
					p_wk->change_idx++;	
					p_wk->change_str[ p_wk->change_idx ] = GFL_STR_GetEOMCode();
				}

				return TRUE;
			}
		}
	}
	return FALSE;
}
//=============================================================================
/**
 *					KEYMAP
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	キー配列	作成
 *
 *	@param	KEYMAP_WORK *p_wk	ワーク
 *	@param	mode							配列モード
 *	@param	heapID						ヒープID
 */
//-----------------------------------------------------------------------------
static void KEYMAP_Create( KEYMAP_WORK *p_wk, NAMEIN_INPUTTYPE mode, HEAPID heapID )
{
	GF_ASSERT( mode < NAMEIN_INPUTTYPE_MAX );

	//クリア
	GFL_STD_MemClear( p_wk, sizeof(KEYMAP_WORK) );
	p_wk->p_key	= NAMEIN_KEYMAP_Alloc( mode, heapID );

	switch( mode )
	{
	case NAMEIN_INPUTTYPE_QWERTY:
		p_wk->w			= 11;
		p_wk->h			= 4;
		p_wk->get_keyrect			= KeyMap_QWERTY_GetRect;
		p_wk->get_movecursor	= KeyMap_QWERTY_GetMoveCursor;
		p_wk->get_keytype			= KeyMap_QWERTY_GetKeyType;
		break;
	case NAMEIN_INPUTTYPE_KANA:
		/* fallthrough */
	case NAMEIN_INPUTTYPE_KATA:
		p_wk->w			= 13;
		p_wk->h			= 6;
		p_wk->get_keyrect			= KeyMap_KANA_GetRect;
		p_wk->get_movecursor	= KeyMap_KANA_GetMoveCursor;
		p_wk->get_keytype			= KeyMap_KANA_GetKeyType;
		break;
	case NAMEIN_INPUTTYPE_ABC:
		/* fallthrough */
	case NAMEIN_INPUTTYPE_KIGOU:
		p_wk->w			= 13;
		p_wk->h			= 6;
		p_wk->get_keyrect			= KeyMap_KANA_GetRect;
		p_wk->get_movecursor	= KeyMap_KANA_GetMoveCursor;
		p_wk->get_keytype			= KeyMap_KIGOU_GetKeyType;
		break;
	}

	//デフォルト移動値
	{	
		int i;
		for( i = 0; i < KEYMAP_KEYMOVE_BUFF_MAX; i++ )
		{	
			p_wk->buff.data[i]	= sc_keymove_default[i];
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	KEYMAP_WORK *p_wk 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void KEYMAP_Delete( KEYMAP_WORK *p_wk )
{	
	NAMEIN_KEYMAP_Free( p_wk->p_key );
	//クリア
	GFL_STD_MemClear( p_wk, sizeof(KEYMAP_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	キー配列	カーソルの位置の文字を取得する
 *
 *	@param	const KEYMAP_WORK *cp_wk	ワーク
 *	@param	GFL_POINT *cp_cursor			カーソルの位置
 *	@param	文字列
 *
 *	@return	キーの種類
 */
//-----------------------------------------------------------------------------
static KEYMAP_KEYTYPE KEYMAP_GetKeyInfo( const KEYMAP_WORK *cp_wk, const GFL_POINT *cp_cursor, STRCODE *p_code )
{	
	KEYMAP_KEYTYPE	type;
	GFL_POINT	strpos;

	//キータイプ取得
	type	= cp_wk->get_keytype( cp_cursor, &strpos );

	if( type == KEYMAP_KEYTYPE_STR ||
			type == KEYMAP_KEYTYPE_DAKUTEN ||
			type == KEYMAP_KEYTYPE_HANDAKUTEN )
	{	
		if( p_code )
		{	
			*p_code	= NAMEIN_KEYMAP_GetStr( cp_wk->p_key, strpos.x, strpos.y );
		}
	}
	else if( type == KEYMAP_KEYTYPE_SPACE )
	{	
		if( p_code )
		{
			*p_code	= L'　';
		}
	}


	return type;
}
//----------------------------------------------------------------------------
/**
 *	@brief	キー配列	タッチ座標をカーソル座標に変換する
 *
 *	@param	const KEYMAP_WORK *cp_wk	ワーク
 *	@param	GFL_POINT *cp_trg					タッチ座標
 *	@param	*cp_cursor								カーソル座標
 *
 *	@retval	TRUE変換できた FALSE範囲外だった
 */
//-----------------------------------------------------------------------------
static BOOL KEYMAP_GetTouchCursor( const KEYMAP_WORK *cp_wk, const GFL_POINT *cp_trg, GFL_POINT *p_cursor )
{	
	GFL_POINT pos;
	GFL_RECT rect;

	for( pos.y = 0; pos.y < cp_wk->h; pos.y++ )
	{	
		for( pos.x = 0; pos.x < cp_wk->w; pos.x++ )
		{	
			if( KEYMAP_GetRange( cp_wk, &pos, &rect ) )
			{
				//キャラ単位をドット単位に
				rect.left		*= 8;
				rect.top		*= 8;
				rect.right	*= 8;
				rect.bottom	*= 8;

				//当たり判定
				if( COLLISION_IsRectXPos( &rect, cp_trg ) )
				{	
					*p_cursor	= pos;
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	キー配列	キーの繋がりを考慮して、加算値分動く
 *
 *	@param	KEYMAP_WORK *p_wk		ワーク
 *	@param	*p_now							カーソルの現在座標
 *	@param	GFL_POINT *cp_add		加算値
 */
//-----------------------------------------------------------------------------
static void KEYMAP_MoveCursor( KEYMAP_WORK *p_wk, GFL_POINT *p_now, const GFL_POINT *cp_add )
{	
	p_wk->get_movecursor( p_now, cp_add, &p_wk->buff );
}
//----------------------------------------------------------------------------
/**
 *	@brief	キー配列	キー配列をBMPWINに書き込む
 *
 *	@param	const KEYMAP_WORK *cp_wk	ワーク
 *	@param	*p_util		プリント便利構造体
 *	@param	*p_que		キュー
 *	@param	*p_font		フォント
 *	@param	heapID		ヒープID
 */
//-----------------------------------------------------------------------------
static void KEYMAP_Print( const KEYMAP_WORK *cp_wk, PRINT_UTIL *p_util, PRINT_QUE *p_que, GFL_FONT *p_font, HEAPID heapID )
{	
	STRBUF	*p_strbuf;
	GFL_POINT	pos;
	GFL_RECT	rect;
	u16 x, y;
	KEYMAP_KEYTYPE keytype;
	STRCODE	get;

	//バッファ作成
	p_strbuf	= GFL_STR_CreateBuffer( 2, heapID );

	//全文字をプリント
	for( pos.y = 0; pos.y < NAMEIN_KEYMAP_GetHeight( cp_wk->p_key ); pos.y++ )
	{	
		for( pos.x = 0; pos.x < NAMEIN_KEYMAP_GetWidth( cp_wk->p_key ); pos.x++ )
		{	
			if( KEYMAP_GetRange( cp_wk, &pos, &rect ) )
			{	
				keytype	= KEYMAP_GetKeyInfo( cp_wk, &pos, &get );
				if( keytype == KEYMAP_KEYTYPE_STR ||
						keytype == KEYMAP_KEYTYPE_DAKUTEN ||
						keytype == KEYMAP_KEYTYPE_HANDAKUTEN)
				{	
					//文字取得し、バッファに納める
					STRCODE	code[2];
					code[0]	= get;
					code[1]	= GFL_STR_GetEOMCode();
					GFL_STR_SetStringCodeOrderLength( p_strbuf, code, 2 );

					//ドット単位へ
					rect.left		*=	8;
					rect.top		*=	8;
					rect.right	*=	8;
					rect.bottom	*=	8;

					//矩形から、文字を中心に描画するために、位置算出
					x	= rect.left + (rect.right - rect.left)/2 - KEYBOARD_BMPWIN_X*8;
					y	= rect.top + (rect.bottom - rect.top)/2 - KEYBOARD_BMPWIN_Y*8;
					x -= PRINTSYS_GetStrWidth( p_strbuf, p_font, 0 )/2;
					y -= PRINTSYS_GetStrHeight( p_strbuf, p_font )/2;

					//描画
					//	絵のデータを見ると、わくが左から右へ、15ドットまでしかないため
					//	絵とあわせるために-1
					//	(うけとってくる矩形座標はキャラ単位なので16)
					PRINT_UTIL_Print( p_util, p_que, x-1, y, p_strbuf, p_font );
				}
			}
		}
	}

	//バッファクリア
	GFL_STR_DeleteBuffer( p_strbuf );
}
//----------------------------------------------------------------------------
/**
 *	@brief	キー配列	カーソルからキーのキャラ座標を受け取る
 *
 *	@param	const KEYMAP_WORK *cp_wk	ワーク
 *	@param	GFL_POINT *cp_cursor			カーソル
 *	@param	*p_rect										キャラ矩形情報
 *	@retval	TRUEなら受け取れた	FALSEならば受け取れない
 */
//-----------------------------------------------------------------------------
static BOOL KEYMAP_GetRange( const KEYMAP_WORK *cp_wk, const GFL_POINT *cp_cursor, GFL_RECT *p_rect )
{	
	return cp_wk->get_keyrect( cp_cursor, p_rect );
}
//----------------------------------------------------------------------------
/**
 *	@brief	かなカナなどの碁盤の目上の矩形受け取り
 *
 *	@param	const GFL_POINT *cp_cursor	カーソル
 *	@param	*p_rect											矩形受け取り(キャラ単位)
 *	@retval	TRUEならば矩形が存在する	FALSEならばしない
 */
//-----------------------------------------------------------------------------
static BOOL KeyMap_KANA_GetRect( const GFL_POINT *cp_cursor, GFL_RECT *p_rect )
{	
	enum
	{	
		KANA_START_X	= 3,
		KANA_START_Y	= 5,
		KANA_KEY_W		= 2,
		KANA_KEY_H		= 3,

		KANA_LONGKEY_W		= 8,
	};

	//キーの種類取得
	KEYMAP_KEYTYPE	key;
	GFL_POINT				strpos;

	key	= KeyMap_KANA_GetKeyType( cp_cursor, &strpos );

	switch( key )
	{	
	case KEYMAP_KEYTYPE_STR:		//文字入力
		/*	fallthrough	*/
	case KEYMAP_KEYTYPE_DAKUTEN:	//濁点
		/*	fallthrough	*/
	case KEYMAP_KEYTYPE_HANDAKUTEN:	//半濁点
		p_rect->left		= KANA_START_X + strpos.x * KANA_KEY_W;
		p_rect->top			= KANA_START_Y + strpos.y * KANA_KEY_H;
		p_rect->right		= p_rect->left + KANA_KEY_W;
		p_rect->bottom	= p_rect->top  + KANA_KEY_H;
		break;
	case KEYMAP_KEYTYPE_KANA:		//かなモード
		/*	fallthrough	*/
	case KEYMAP_KEYTYPE_KATA:		//カナモード
		/*	fallthrough	*/
	case KEYMAP_KEYTYPE_ABC:		//アルファベットモード
		/*	fallthrough	*/
	case KEYMAP_KEYTYPE_KIGOU:	//記号モード
		/*	fallthrough	*/
	case KEYMAP_KEYTYPE_QWERTY:	//ローマ字モード
		p_rect->left		= KANA_START_X + cp_cursor->x * KANA_KEY_W;
		p_rect->top			= KANA_START_Y + cp_cursor->y * KANA_KEY_H;
		p_rect->right		= p_rect->left + KANA_KEY_W;
		p_rect->bottom	= p_rect->top  + KANA_KEY_H;
		break;
	case KEYMAP_KEYTYPE_DELETE:	//けす
		p_rect->left		= KANA_START_X + cp_cursor->x * KANA_KEY_W;
		p_rect->top			= KANA_START_Y + cp_cursor->y * KANA_KEY_H;
		p_rect->right		= p_rect->left + KANA_LONGKEY_W;
		p_rect->bottom	= p_rect->top  + KANA_KEY_H;
		break;	
		break;
	case KEYMAP_KEYTYPE_DECIDE:	//やめる
		p_rect->left		= KANA_START_X + (cp_cursor->x-1) * KANA_KEY_W + KANA_LONGKEY_W;
		p_rect->top			= KANA_START_Y + (cp_cursor->y) * KANA_KEY_H;
		p_rect->right		= p_rect->left + KANA_LONGKEY_W;
		p_rect->bottom	= p_rect->top  + KANA_KEY_H;
		break;
	case KEYMAP_KEYTYPE_NONE:
		return FALSE;
	}

	switch( cp_cursor->y )
	{	
	case 5:
		if( cp_cursor->x > 6 )
		{	
			return FALSE;
		}
		break;
	}


	return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PCのキーボード状の矩形受け取り
 *
 *	@param	const GFL_POINT *cp_cursor	カーソル
 *	@param	*p_rect											矩形受け取り
 *	@retval	TRUEならば矩形が存在する	FALSEならばしない
 */
//-----------------------------------------------------------------------------
static BOOL KeyMap_QWERTY_GetRect( const GFL_POINT *cp_cursor, GFL_RECT *p_rect )
{	
	enum
	{	
		KANA_KEY_W				= 2,
		KANA_KEY_H				= 3,

		KANA_LONGKEY_W		= 4,	
		KANA_ENTER_W			= 5,	
		KANA_SPACE_W			= 14,	
	};

	//文字キーの行ごとの開始位置
	static const int sc_start_x[]	=
	{	
		5,6,7
	};
	static const int sc_start_y[]	=
	{	
		8,12,16
	};


	//キーの種類取得
	KEYMAP_KEYTYPE	key;
	GFL_POINT				strpos;
	
	key	= KeyMap_QWERTY_GetKeyType( cp_cursor, &strpos );

	switch( key )
	{	
	case KEYMAP_KEYTYPE_STR:		//文字入力
		p_rect->left		= sc_start_x[ strpos.y ] + strpos.x * KANA_KEY_W;
		p_rect->top			= sc_start_y[ strpos.y ];
		p_rect->right		= p_rect->left + KANA_KEY_W;
		p_rect->bottom	= p_rect->top  + KANA_KEY_H;
		break;
	case KEYMAP_KEYTYPE_KANA:		//かなモード
		/*	fallthrough	*/
	case KEYMAP_KEYTYPE_KATA:		//カナモード
		/*	fallthrough	*/
	case KEYMAP_KEYTYPE_ABC:		//アルファベットモード
		/*	fallthrough	*/
	case KEYMAP_KEYTYPE_KIGOU:	//記号モード
		/*	fallthrough	*/
	case KEYMAP_KEYTYPE_QWERTY:	//ローマ字モード
		p_rect->left		= 3 + cp_cursor->x * KANA_KEY_W;
		p_rect->top			= 20;
		p_rect->right		= p_rect->left + KANA_KEY_W;
		p_rect->bottom	= p_rect->top  + KANA_KEY_H;
		break;
	case KEYMAP_KEYTYPE_DELETE:	//けす
		p_rect->left		= 25;
		p_rect->top			= 8;
		p_rect->right		= p_rect->left + KANA_LONGKEY_W;
		p_rect->bottom	= p_rect->top  + KANA_KEY_H;
		break;	
		break;
	case KEYMAP_KEYTYPE_DECIDE:	//やめる
		p_rect->left		= 24;
		p_rect->top			= 12;
		p_rect->right		= p_rect->left + KANA_ENTER_W;
		p_rect->bottom	= p_rect->top  + KANA_KEY_H;
		break;
	case KEYMAP_KEYTYPE_SHIFT :	//シフト
		if( cp_cursor->x == 0 )
		{	
			p_rect->left		= 3;
			p_rect->top			= 16;
			p_rect->right		= p_rect->left + KANA_LONGKEY_W;
			p_rect->bottom	= p_rect->top  + KANA_KEY_H;
		}
		else
		{	
			p_rect->left		= 23;
			p_rect->top			= 16;
			p_rect->right		= p_rect->left + KANA_LONGKEY_W;
			p_rect->bottom	= p_rect->top  + KANA_KEY_H;
		}
		break;
	case KEYMAP_KEYTYPE_SPACE :	//スペース
		p_rect->left		= 13;
		p_rect->top			= 20;
		p_rect->right		= p_rect->left + KANA_SPACE_W;
		p_rect->bottom	= p_rect->top  + KANA_KEY_H;
		break;
	case KEYMAP_KEYTYPE_NONE:
		return FALSE;
	}
	
	switch( cp_cursor->y )
	{	
	case 1:
		if( cp_cursor->x > 9 ) 
		{	
			return FALSE;
		}
		break;
	case 2:
		if( cp_cursor->x > 9 ) 
		{	
			return FALSE;
		}
		break;
	case 3:
		if( cp_cursor->x > 5 ) 
		{	
			return FALSE;
		}
		break;
	}

	return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	かなカナなどの碁盤の目上のカーソル移動
 *
 *	@param	GFL_POINT *p_now		カーソル
 *	@param	GFL_POINT *cp_add		移動値
 *	@param	p_buff							カーソル移動記憶バッファ
 */
//-----------------------------------------------------------------------------
static void KeyMap_KANA_GetMoveCursor( GFL_POINT *p_now, const GFL_POINT *cp_add, KEYMOVE_BUFF *p_buff )
{
	//位置
	enum
	{	
		ROW_START		= 0,
		ROW_KEY_END	= 4,
		ROW_BTN			= 5,
		ROW_END			= ROW_BTN,

		COL_START		= 0,
		COL_KEY_END	= 12,
		COL_BTN_END	= 6,
	};


	BOOL is_btn_in	= FALSE;
	BOOL is_btn_out	= FALSE;

	switch( p_now->y )
	{	
	case ROW_START:
		//キー最上段
		p_now->x	+= cp_add->x;
		p_now->x	= MATH_ROUND( p_now->x, COL_START, COL_KEY_END );
		p_now->y	+= cp_add->y;
		p_now->y	= MATH_ROUND( p_now->y, ROW_START, ROW_END );

		if( p_now->y == ROW_BTN )
		{	
			is_btn_in	= TRUE;
		}
		break;
	case ROW_KEY_END:
		//キー最下段

		p_now->x	+= cp_add->x;
		p_now->x	= MATH_ROUND( p_now->x, COL_START, COL_KEY_END );
		p_now->y	+= cp_add->y;
		p_now->y	= MATH_ROUND( p_now->y, ROW_START, ROW_END );

		if( p_now->y == ROW_BTN )
		{	
			is_btn_in	= TRUE;
		}
		break;
	case ROW_BTN:
		//ボタン

		p_now->x	+= cp_add->x;
		p_now->x	= MATH_ROUND( p_now->x, COL_START, COL_BTN_END );
		p_now->y	+= cp_add->y;
		p_now->y	= MATH_ROUND( p_now->y, ROW_START, ROW_END );

		if( p_now->y != ROW_BTN )
		{	
			is_btn_out	= TRUE;
		}
		break;
	default:
		//キー中間
		p_now->x	+= cp_add->x;
		p_now->x	= MATH_ROUND( p_now->x, COL_START, COL_KEY_END );
		p_now->y	+= cp_add->y;
		p_now->y	= MATH_ROUND( p_now->y, ROW_START, ROW_END );
	}

	//ボタンに入ったときの修正
	if( is_btn_in )
	{
		if( 5 <= p_now->x  && p_now->x <= 8 )
		{	
			//けすボタン
			p_buff->data[KEYMAP_KEYMOVE_BUFF_DELETE]	= p_now->x;
			p_now->x	= 5;
		}
		else if( 9 <= p_now->x  && p_now->x <= 12 )
		{	
			//やめるボタン
			p_buff->data[KEYMAP_KEYMOVE_BUFF_DICEDE]	= p_now->x;
			p_now->x	= 6;
		}
	}

	//ボタンから出たときの修正
	if( is_btn_out )
	{	
		if( p_now->x == 5 )
		{	
			//けすボタン
			p_now->x	= p_buff->data[KEYMAP_KEYMOVE_BUFF_DELETE];
		}
		else if( p_now->x == 6 )
		{	
			p_now->x	= p_buff->data[KEYMAP_KEYMOVE_BUFF_DICEDE];
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	PCのキーボード状のカーソル移動
 *
 *	@param	GFL_POINT *p_now		カーソル
 *	@param	GFL_POINT *cp_add		移動値
 *	@param	p_buff							カーソル移動記憶バッファ
 */
//-----------------------------------------------------------------------------
static void KeyMap_QWERTY_GetMoveCursor( GFL_POINT *p_now, const GFL_POINT *cp_add, KEYMOVE_BUFF *p_buff )
{	
	//位置
	enum
	{	
		ROW_START		= 0,
		ROW_LINE1		= ROW_START,
		ROW_LINE2,
		ROW_LINE3,
		ROW_LINE4,
		ROW_END			= ROW_LINE4,

		COL_START			= 0,
		COL_LINE1_END	= 10,
		COL_LINE2_END	= 9,
		COL_LINE3_END	= 9,
		COL_LINE4_END	= 5,
	};

	switch( p_now->y )
	{	
	case ROW_LINE1:
		p_now->x	+= cp_add->x;
		p_now->x	= MATH_ROUND( p_now->x, COL_START, COL_LINE1_END );
		p_now->y	+= cp_add->y;
		p_now->y	= MATH_ROUND( p_now->y, ROW_START, ROW_END );
		if( p_now->y == ROW_LINE4 )
		{	
			//ROW_LINE4へ移動
			switch( p_now->x )
			{	
			case 0:	//Q
				p_now->x	= p_buff->data[KEYMAP_KEYMOVE_BUFF_Q_U];
				break;
			case 1:
				/* fallthrough */
			case 2:
				/* fallthrough */
			case 3:
				p_now->x	+= 1;
				break;
			default:
				p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_D]	= p_now->x;
				p_now->x	= 5;
				break;
			}
		}
		else if( p_now->y == ROW_LINE2 )
		{	
			//ROW_LINE2へ移動
			switch( p_now->x )
			{	
			case 0:	//Q
				/* fallthrough */
			case 1:	//W
				p_buff->data[KEYMAP_KEYMOVE_BUFF_A_U]	= p_now->x;
				p_now->x	= 0;
				break;
			default:
				p_now->x	-= 1;
				break;
			}
		}
		break;
	case ROW_LINE2:
		p_now->x	+= cp_add->x;
		p_now->x	= MATH_ROUND( p_now->x, COL_START, COL_LINE2_END );
		p_now->y	+= cp_add->y;
		p_now->y	= MATH_ROUND( p_now->y, ROW_START, ROW_END );
		if( p_now->y == ROW_LINE1 )
		{	
			//ROW_LINE1へ移動
			switch( p_now->x )
			{	
			case 0:	//A
				p_now->x	= p_buff->data[KEYMAP_KEYMOVE_BUFF_A_U];
				break;
			default:
				p_now->x	+= 1;
				break;
			}
		}
		else if( p_now->y == ROW_LINE3 )
		{	
			//ROW_LINE3へ移動
			switch( p_now->x )
			{	
			case 0:	//a
				p_now->x	= p_buff->data[KEYMAP_KEYMOVE_BUFF_A_D];
				break;
			case 7:
				/* fallthrough */
			case 8:
				p_buff->data[KEYMAP_KEYMOVE_BUFF_BOU_U]	= p_now->x;
				p_now->x	= 8;
				break;
			case 9:
				p_now->x	= 9;
				break;
			default:
				p_now->x	+= 1;
				break;
			}
		}
		break;
	case ROW_LINE3:
		p_now->x	+= cp_add->x;
		p_now->x	= MATH_ROUND( p_now->x, COL_START, COL_LINE3_END );
		p_now->y	+= cp_add->y;
		p_now->y	= MATH_ROUND( p_now->y, ROW_START, ROW_END );
		if( p_now->y == ROW_LINE2 )
		{	
			//ROW_LINE2へ移動
			switch( p_now->x )
			{	
			case 0:	//SHIFT	L
				/* fallthrough */
			case 1:	//Z
				p_buff->data[KEYMAP_KEYMOVE_BUFF_A_D]	= p_now->x;
				p_now->x	= 0;
				break;
			case 8:	//-
				p_now->x	= p_buff->data[KEYMAP_KEYMOVE_BUFF_BOU_U];
				break;
			case 9:	//SHIFT R
				p_now->x	= 9;
				break;
			default:
				p_now->x	-= 1;
			}
		}
		else if( p_now->y == ROW_LINE4 )
		{	
			//ROW_LINE4へ移動
			switch( p_now->x )
			{	
			case 0:	//SHIFT L
				p_now->x	= p_buff->data[KEYMAP_KEYMOVE_BUFF_LSHIFT_D];
				break;
			case 1: //Z
				/* fallthrough */
			case 2:	//X
				/* fallthrough */
			case 3:	//C
				p_now->x	+= 1;
				break;
			default:
				p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_U]	= p_now->x;
				p_now->x	= 5;
			}
		}
		break;
	case ROW_LINE4:
		p_now->x	+= cp_add->x;
		p_now->x	= MATH_ROUND( p_now->x, COL_START, COL_LINE4_END );
		p_now->y	+= cp_add->y;
		p_now->y	= MATH_ROUND( p_now->y, ROW_START, ROW_END );
		if( p_now->y == ROW_LINE3 )
		{	
			//ROW_LINE3へ移動
			switch( p_now->x )
			{	
			case 0:	//かな
				/* fallthrough */
			case 1:	//カナ
				p_buff->data[KEYMAP_KEYMOVE_BUFF_LSHIFT_D]	= p_now->x;
				p_now->x	= 0;
				break;
			case 2:	//ABC
				/* fallthrough */
			case 3:	//記号
				/* fallthrough */
			case 4:	//ローマ字
				p_now->x	-= 1;
				break;
			case 5:	//SPCASE
				p_now->x	= p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_U];
				break;
			}
		}
		else if( p_now->y == ROW_LINE1 )
		{	
			//ROW_LINE1へ移動
			switch( p_now->x )
			{	
			case 0:	//かな
				/* fallthrough */
			case 1:	//カナ
				p_buff->data[KEYMAP_KEYMOVE_BUFF_Q_U]	= p_now->x;
				p_now->x	= 0;
				break;
			case 2:	//ABC
				/* fallthrough */
			case 3:	//記号
				/* fallthrough */
			case 4:	//ローマ字
				p_now->x	-= 1;
				break;
			case 5:	//SPCASE
				p_now->x	= p_buff->data[KEYMAP_KEYMOVE_BUFF_SPACE_D];
				break;
			}
		}
		break;
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	かなカナなどの碁盤の目上のキー種類取得
 *
 *	@param	const GFL_POINT *cp_cursor	カーソル
 *	@param	p_strpos										文字受け取り座標
 *
 *	@return	キー情報
 */
//-----------------------------------------------------------------------------
static KEYMAP_KEYTYPE KeyMap_KANA_GetKeyType( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos )
{	
	if( cp_cursor->y < KEYMAP_KANA_HEIGHT )
	{	
		if( cp_cursor->y == 0 && cp_cursor->x == 12 )
		{	
			if( p_strpos )
			{	
				p_strpos->x	= cp_cursor->x;
				p_strpos->y	= cp_cursor->y;
			}
			//濁点
			return KEYMAP_KEYTYPE_DAKUTEN;
		}
		else if( cp_cursor->y == 1 && cp_cursor->x == 12 )
		{	
			if( p_strpos )
			{	
				p_strpos->x	= cp_cursor->x;
				p_strpos->y	= cp_cursor->y;
			}
			//半濁点
			return KEYMAP_KEYTYPE_HANDAKUTEN;
		}
		else
		{	
			//その他文字
			//碁盤の目上はカーソル位置と同じ
			if( p_strpos )
			{	
				p_strpos->x	= cp_cursor->x;
				p_strpos->y	= cp_cursor->y;
			}
			return KEYMAP_KEYTYPE_STR;
		}
	}
	else if( cp_cursor->x < 7 )
	{	
		return cp_cursor->x + KEYMAP_KEYTYPE_KANA;
	}

	return KEYMAP_KEYTYPE_NONE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	かなカナなどの碁盤の目上のキー種類取得
 *
 *	@param	const GFL_POINT *cp_cursor	カーソル
 *	@param	p_strpos										文字受け取り座標
 *
 *	@return	キー情報
 */
//-----------------------------------------------------------------------------
static KEYMAP_KEYTYPE KeyMap_KIGOU_GetKeyType( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos )
{	
	if( cp_cursor->y < KEYMAP_KANA_HEIGHT )
	{	
		//その他文字
		//碁盤の目上はカーソル位置と同じ
		if( p_strpos )
		{	
			p_strpos->x	= cp_cursor->x;
			p_strpos->y	= cp_cursor->y;
		}

		return KEYMAP_KEYTYPE_STR;
	}
	else if( cp_cursor->x < 7 )
	{	
		return cp_cursor->x + KEYMAP_KEYTYPE_KANA;
	}

	return KEYMAP_KEYTYPE_NONE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PCのキーボード状のキー種類取得
 *
 *	@param	const GFL_POINT *cp_cursor	カーソル
 *	@param	GFL_POINT	*p_strpos					文字受け取り座標
 *
 *	@return	キー情報
 */
//-----------------------------------------------------------------------------
static KEYMAP_KEYTYPE KeyMap_QWERTY_GetKeyType( const GFL_POINT *cp_cursor, GFL_POINT *p_strpos )
{	
	GFL_POINT	strpos;
	KEYMAP_KEYTYPE	ret	= KEYMAP_KEYTYPE_NONE;

	//それぞれの行からキーの種類を受け取る
	switch( cp_cursor->y )
	{
	case 0:
		if( cp_cursor->x == 10 )
		{	
			ret =  KEYMAP_KEYTYPE_DELETE;
		}
		else
		{	
			strpos	= *cp_cursor;
			ret =  KEYMAP_KEYTYPE_STR;
		}
		break;
	case 1:
		if( cp_cursor->x == 9 )
		{	
			ret =  KEYMAP_KEYTYPE_DECIDE;
		}
		else
		{	
			strpos	= *cp_cursor;
			ret =  KEYMAP_KEYTYPE_STR;
		}
		break;
	case 2:
		if( cp_cursor->x == 0 || cp_cursor->x == 9 )
		{	
			ret =  KEYMAP_KEYTYPE_SHIFT;
		}
		else
		{	
			//左シフトの分減らす
			strpos.x	= cp_cursor->x - 1;
			strpos.y	= cp_cursor->y;
			ret =  KEYMAP_KEYTYPE_STR;
		}
		break;
	case 3:
		if( cp_cursor->x == 5 )
		{	
			ret =  KEYMAP_KEYTYPE_SPACE;
		}
		else
		{	
			ret =  KEYMAP_KEYTYPE_KANA + cp_cursor->x;
		}
		break;
	}

	//文字位置を受け取り
	if( ret == KEYMAP_KEYTYPE_STR )
	{	
		if( p_strpos )
		{	
			*p_strpos	= strpos;
		}
	}


	return ret;
}
//=============================================================================
/**
 *					KEYBOARD_WORK
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	キーボード	初期化
 *
 *	@param	KEYBOARD_WORK *p_wk	ワーク
 *	@param	mode		入力モード
 *	@param	p_font	フォント
 *	@param	p_que		キュー
 *	@param	heapID	ヒープID
 */
//-----------------------------------------------------------------------------
static void KEYBOARD_Init( KEYBOARD_WORK *p_wk, NAMEIN_INPUTTYPE mode, GFL_FONT *p_font, PRINT_QUE *p_que, HEAPID heapID )
{	
	//クリア
	GFL_STD_MemClear( p_wk, sizeof(KEYBOARD_WORK) );
	p_wk->cursor.x	= 0;
	p_wk->cursor.y	= 0;
	p_wk->p_font		= p_font;
	p_wk->p_que			= p_que;
	p_wk->heapID		= heapID;
	p_wk->mode			= mode;

	//キーアニメ作成
	KEYANM_Init( &p_wk->keyanm, heapID );

	//キーマップ作成
	KEYMAP_Create( &p_wk->keymap, mode, heapID );

	//キー用BMPWIN作成
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( BG_FRAME_FONT_M, KEYBOARD_BMPWIN_X, KEYBOARD_BMPWIN_Y, KEYBOARD_BMPWIN_W, KEYBOARD_BMPWIN_H, PLT_BG_FONT_M, GFL_BMP_CHRAREA_GET_B );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

	//プリントキュー設定
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

	//描画
	KEYMAP_Print( &p_wk->keymap, &p_wk->util, p_que, p_font, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	キーボード	破棄
 *
 *	@param	KEYBOARD_WORK *p_wk ワーク
 */
//-----------------------------------------------------------------------------
static void KEYBOARD_Exit( KEYBOARD_WORK *p_wk )
{	
	//BMPWIN破棄
	GFL_BMPWIN_Delete( p_wk->p_bmpwin );

	//キーマップ破棄
	KEYMAP_Delete( &p_wk->keymap );

	//キーアニメ破棄
	KEYANM_Exit( &p_wk->keyanm );

	//クリア
	GFL_STD_MemClear( p_wk, sizeof(KEYBOARD_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	キーボード	メイン処理
 *
 *	@param	KEYBOARD_WORK *p_wk		ワーク
 *
 */
//-----------------------------------------------------------------------------
static void KEYBOARD_Main( KEYBOARD_WORK *p_wk )
{	
	switch( p_wk->state )
	{	
	case KEYBOARD_STATE_WAIT:		//処理待ち中
		p_wk->input	= KEYBOARD_INPUT_NONE;
		{	
			BOOL is_decide = FALSE;
			//タッチ
			if( is_decide == FALSE )
			{	
				GFL_POINT	pos;
				u32 x, y;
				if( GFL_UI_TP_GetPointTrg( &x, &y ) )
				{	
					pos.x	= x;
					pos.y	= y;
					if( KEYMAP_GetTouchCursor( &p_wk->keymap, &pos, &p_wk->cursor ) )
					{	
						is_decide	= TRUE;
					}
				}
			}
			//十字キー移動
			if( is_decide == FALSE )
			{	
				GFL_POINT	pos;
				BOOL is_move	= FALSE;

				if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
				{	
					pos.x	= 0;
					pos.y	= -1;
					is_move	= TRUE;
				}
				else if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
				{	
					pos.x	= 0;
					pos.y	= 1;
					is_move	= TRUE;
				}
				else if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT )
				{	
					pos.x	= -1;
					pos.y	= 0;
					is_move	= TRUE;
				}
				else if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT )
				{	
					pos.x	= 1;
					pos.y	= 0;
					is_move	= TRUE;
				}
				if( is_move )
				{	
					GFL_RECT rect;
					KEYMAP_MoveCursor( &p_wk->keymap, &p_wk->cursor, &pos );
					//移動アニメ
					if( KEYMAP_GetRange( &p_wk->keymap, &p_wk->cursor, &rect ) )
					{	
						KEYANM_Start( &p_wk->keyanm, KEYANM_TYPE_MOVE, &rect );
					}
				}
				if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DECIDE )
				{	
					is_decide	= TRUE;
				}
				if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL )
				{	
					p_wk->input = KEYBOARD_INPUT_BACKSPACE;
				}
			}
			//決定
			if( is_decide )
			{	
				KEYMAP_KEYTYPE	type;
				GFL_RECT rect;

				type	= KEYMAP_GetKeyInfo( &p_wk->keymap, &p_wk->cursor, &p_wk->code );
				switch( type )
				{	
				case KEYMAP_KEYTYPE_STR:			//文字入力
					/* fallthrough */
				case KEYMAP_KEYTYPE_SPACE:		//スペース
					//QWERTYのときは変換入力
					//他のときは通常入力
					p_wk->input	= p_wk->mode == NAMEIN_INPUTTYPE_QWERTY ?
						KEYBOARD_INPUT_CHANGESTR: KEYBOARD_INPUT_STR;
					break;

				case KEYMAP_KEYTYPE_KANA:		//かなモード
					/* fallthrough */
				case KEYMAP_KEYTYPE_KATA:		//カナモード
					/* fallthrough */
				case KEYMAP_KEYTYPE_ABC:			//アルファベットモード
					/* fallthrough */
				case KEYMAP_KEYTYPE_KIGOU:		//記号モード
					/* fallthrough */
				case KEYMAP_KEYTYPE_QWERTY:	//ローマ字モード
					p_wk->input = KEYBOARD_INPUT_CHAGETYPE;
					if( Keyboard_StartMove( p_wk, type - KEYMAP_KEYTYPE_KANA ) )
					{	
						p_wk->state	= KEYBOARD_STATE_MOVE;
					}
					break;

				case KEYMAP_KEYTYPE_DELETE:	//けす
					p_wk->input = KEYBOARD_INPUT_BACKSPACE;
					break;

				case KEYMAP_KEYTYPE_DECIDE:	//やめる
					p_wk->input = KEYBOARD_INPUT_EXIT;
					break;

				case KEYMAP_KEYTYPE_SHIFT:		//シフト
					p_wk->input = KEYBOARD_INPUT_SHIFT;
					p_wk->is_shift	^= 1;
					break;

				case KEYMAP_KEYTYPE_DAKUTEN:		//濁点
					p_wk->input = KEYBOARD_INPUT_DAKUTEN;
					break;

				case KEYMAP_KEYTYPE_HANDAKUTEN:		//半濁点
					p_wk->input = KEYBOARD_INPUT_HANDAKUTEN;
					break;
				}

				//決定アニメ
				if( KEYMAP_GetRange( &p_wk->keymap, &p_wk->cursor, &rect ) )
				{	
					KEYANM_Start( &p_wk->keyanm, KEYANM_TYPE_DECIDE, &rect );
				}
			}
		}
		break;

	case KEYBOARD_STATE_INPUT:	//入力
		break;

	case KEYBOARD_STATE_MOVE:		//移動中
		if( Keyboard_MainMove( p_wk ) )
		{	
			p_wk->state	= KEYBOARD_STATE_WAIT;
		}
		break;

	default:
		GF_ASSERT(0);
	}

	KEYANM_Main( &p_wk->keyanm );

	KEYBOARD_PrintMain( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	キーボード	現在の情報を取得
 *
 *	@param	const KEYBOARD_WORK *cp_wk ワーク
 *
 *	@return	状態
 */
//-----------------------------------------------------------------------------
static KEYBOARD_STATE KEYBOARD_GetState( const KEYBOARD_WORK *cp_wk )
{	
	return cp_wk->state;
}
//----------------------------------------------------------------------------
/**
 *	@brief	キーボード	現在の入力を取得
 *
 *	@param	const KEYBOARD_WORK *cp_wk	ワーク
 *	@param	*p_str											入力した文字列
 *
 *	@return	入力状況
 */
//-----------------------------------------------------------------------------
static KEYBOARD_INPUT KEYBOARD_GetInput( const KEYBOARD_WORK *cp_wk, STRCODE *p_str )
{	

	//入力したときは文字列を返す
	if( cp_wk->input == KEYBOARD_INPUT_STR ||
			cp_wk->input == KEYBOARD_INPUT_CHANGESTR 
			)
	{	
		if( p_str )
		{	
			*p_str	= cp_wk->code;
		}
	}

	return cp_wk->input;
}
//----------------------------------------------------------------------------
/**
 *	@brief	キーボード	シフト押しているかどうか
 *
 *	@param	const KEYBOARD_WORK *cp_wk	ワーク
 *
 *	@return	TRUE シフト押している FALSEシフト押してない
 */
//-----------------------------------------------------------------------------
static BOOL KEYBOARD_IsShift( const KEYBOARD_WORK *cp_wk )
{	
	return cp_wk->is_shift;
}
//----------------------------------------------------------------------------
/**
 *	@brief	キーボード	描画メイン
 *
 *	@param	KEYBOARD_WORK *p_wk	ワーク
 *
 *	@retval  BOOL  転送が終わっている場合はTRUE／終わっていない場合はFALSE
 */
//-----------------------------------------------------------------------------
static BOOL KEYBOARD_PrintMain( KEYBOARD_WORK *p_wk )
{	
	return PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
}
//----------------------------------------------------------------------------
/**
 *	@brief	キーボード	入力モード変更時の動作アニメ開始
 *
 *	@param	KEYBOARD_WORK *p_wk	ワーク
 *	@param	mode								モード
 *	@retval TRUE 開始可能	FALSE不可能
 */
//-----------------------------------------------------------------------------
static BOOL Keyboard_StartMove( KEYBOARD_WORK *p_wk, NAMEIN_INPUTTYPE mode )
{	
	if( p_wk->change_mode != mode )
	{	
		p_wk->change_mode			= mode;
		p_wk->change_move_cnt	= 0;
		p_wk->change_move_seq	= 0;
		p_wk->is_change_anm		= TRUE;
/*
		G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG3,
			GX_BLEND_PLANEMASK_BG0 |GX_BLEND_PLANEMASK_BG2 | GX_BLEND_PLANEMASK_BD,
			KYEBOARD_CHANGEMOVE_START_ALPHA, 16 );
*/
	
		p_wk->is_btn_move	 = p_wk->change_mode == NAMEIN_INPUTTYPE_QWERTY
												|| p_wk->mode == NAMEIN_INPUTTYPE_QWERTY;

	
		if( p_wk->is_btn_move )
		{	
			//フォントとフレームとボタンが動く
			G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG2 |
					GX_BLEND_PLANEMASK_BG3, KYEBOARD_CHANGEMOVE_START_ALPHA );
		}
		else
		{
			//フォントとフレームだけ動く
			G2_SetBlendBrightness( GX_BLEND_PLANEMASK_BG1 | GX_BLEND_PLANEMASK_BG3,
					KYEBOARD_CHANGEMOVE_START_ALPHA );
		}
		return TRUE;
	}
	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	キーボード	入力モード変更時の動作アニメ動作
 *
 *	@param	KEYBOARD_WORK *p_wk	ワーク
 *
 *	@retval	TRUE終了	FALSE処理中or処理していない
 */
//-----------------------------------------------------------------------------
static BOOL Keyboard_MainMove( KEYBOARD_WORK *p_wk )
{
	enum
	{	
		SEQ_DOWN_MAIN,
		SEQ_CHANGE_MODE,
		SEQ_UP_INIT,
		SEQ_UP_MAIN,
		SEQ_END,
	};
	s16 scroll_y;
	s16	alpha;

	if( p_wk->is_change_anm )
	{	
		switch( p_wk->change_move_seq )
		{	
		case SEQ_DOWN_MAIN:
			scroll_y	= KYEBOARD_CHANGEMOVE_START_Y
				+ (KYEBOARD_CHANGEMOVE_END_Y-KYEBOARD_CHANGEMOVE_START_Y)
				* p_wk->change_move_cnt / KYEBOARD_CHANGEMOVE_SYNC;

			alpha			= KYEBOARD_CHANGEMOVE_START_ALPHA
				- (KYEBOARD_CHANGEMOVE_END_ALPHA-KYEBOARD_CHANGEMOVE_START_ALPHA)
				* p_wk->change_move_cnt / KYEBOARD_CHANGEMOVE_SYNC;

			//G2_ChangeBlendAlpha( alpha, 16 );
			G2_ChangeBlendBrightness( alpha );
			GFL_BG_SetScroll( BG_FRAME_KEY_M, GFL_BG_SCROLL_Y_SET, scroll_y );
			GFL_BG_SetScroll( BG_FRAME_FONT_M, GFL_BG_SCROLL_Y_SET, scroll_y );
			if( p_wk->is_btn_move )
			{	
				GFL_BG_SetScroll( BG_FRAME_BTN_M, GFL_BG_SCROLL_Y_SET, scroll_y );
			}
			if( p_wk->change_move_cnt++ > KYEBOARD_CHANGEMOVE_SYNC )
			{	
				p_wk->change_move_cnt	= 0;
				p_wk->change_move_seq			= SEQ_CHANGE_MODE;
			}
			break;

		case SEQ_CHANGE_MODE:
			Keyboard_ChangeMode( p_wk, p_wk->change_mode );
			p_wk->change_move_seq	= SEQ_UP_INIT;
			break;

		case SEQ_UP_INIT:
			p_wk->change_move_cnt	= KYEBOARD_CHANGEMOVE_SYNC;
			p_wk->change_move_seq	= SEQ_UP_MAIN;
			break;

		case SEQ_UP_MAIN:
			scroll_y	= KYEBOARD_CHANGEMOVE_START_Y
				+ (KYEBOARD_CHANGEMOVE_END_Y-KYEBOARD_CHANGEMOVE_START_Y)
				* p_wk->change_move_cnt / KYEBOARD_CHANGEMOVE_SYNC;

			alpha			= KYEBOARD_CHANGEMOVE_START_ALPHA
				- (KYEBOARD_CHANGEMOVE_END_ALPHA-KYEBOARD_CHANGEMOVE_START_ALPHA)
				* p_wk->change_move_cnt / KYEBOARD_CHANGEMOVE_SYNC;

			//G2_ChangeBlendAlpha( alpha, 16 );
			G2_ChangeBlendBrightness( alpha );
			GFL_BG_SetScroll( BG_FRAME_KEY_M, GFL_BG_SCROLL_Y_SET, scroll_y );
			GFL_BG_SetScroll( BG_FRAME_FONT_M, GFL_BG_SCROLL_Y_SET, scroll_y );
			if( p_wk->is_btn_move )		
			{	
				GFL_BG_SetScroll( BG_FRAME_BTN_M, GFL_BG_SCROLL_Y_SET, scroll_y );
			}
			if( p_wk->change_move_cnt-- == 0 )
			{	
				p_wk->change_move_seq	= SEQ_END;
			}
			break;

		case SEQ_END:
			G2_BlendNone();
			p_wk->is_change_anm	= FALSE;
			return TRUE;
		}
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	キーボード	モード切替
 *
 *	@param	KEYBOARD_WORK *p_wk	ワーク
 *	@param	mode								モード
 */
//-----------------------------------------------------------------------------
static void Keyboard_ChangeMode( KEYBOARD_WORK *p_wk, NAMEIN_INPUTTYPE mode )
{	
	if( p_wk->mode != mode )
	{	
		//キー配列作成しなおし
		KEYMAP_Delete( &p_wk->keymap );
		KEYMAP_Create( &p_wk->keymap, mode, p_wk->heapID );

		//キー配列描画
		GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );
		KEYMAP_Print( &p_wk->keymap, &p_wk->util, p_wk->p_que, p_wk->p_font, p_wk->heapID );

		//モードにより、読み込むスクリーンを切り替え
		if( mode == NAMEIN_INPUTTYPE_QWERTY )
		{	
			GFL_ARC_UTIL_TransVramScreen( ARCID_NAMEIN_GRA, NARC_namein_gra_name_romaji_NSCR,
					BG_FRAME_KEY_M, 0, 0, FALSE, p_wk->heapID );
			GFL_BG_SetVisible( BG_FRAME_BTN_M, FALSE );

			//@todoカーソル位置変更 qwertになったとき、もどるとき
			

			//@todo変換バッファを消す処理
		}
		else
		{	
			GFL_ARC_UTIL_TransVramScreen( ARCID_NAMEIN_GRA, NARC_namein_gra_name_kana_NSCR,
					BG_FRAME_KEY_M, 0, 0, FALSE, p_wk->heapID );
			GFL_BG_SetVisible( BG_FRAME_BTN_M, TRUE );
		}

		//変更したのでモード代入
		p_wk->mode	= mode;
	}
}
//=============================================================================
/**
 *		その他
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	矩形と点の衝突検知
 *
 *	@param	const GFL_RECT *cp_rect		矩形
 *	@param	GFL_POINT *cp_pos					点
 *
 *	@return	TRUEでヒット	FALSEでヒットしていない
 */
//-----------------------------------------------------------------------------
static BOOL COLLISION_IsRectXPos( const GFL_RECT *cp_rect, const GFL_POINT *cp_pos )
{	
	return ( ((u32)( cp_pos->x - cp_rect->left) <= (u32)(cp_rect->right - cp_rect->left))
						&	((u32)( cp_pos->y - cp_rect->top) <= (u32)(cp_rect->bottom - cp_rect->top)));
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
 *	@brief	描画完了待ち
 *
 *	@param	SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_param				ワーク
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_WaitPrint( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{
	NAMEIN_WORK	*p_wk	= p_param;

	BOOL ret;

	ret	= KEYBOARD_PrintMain( &p_wk->keyboard );
	ret	|= STRINPUT_PrintMain( &p_wk->strinput );

	if( ret )
	{	
		SEQ_SetNext( p_seqwk, SEQFUNC_FadeOut );
	}
}
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
		SEQ_SetNext( p_seqwk, SEQFUNC_Main );
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
		SEQ_SetNext( p_seqwk, SEQFUNC_End );
		break;

	default:
		GF_ASSERT(0);
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	設定画面メイン処理
 *
 *	@param	SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_param				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{	
	NAMEIN_WORK	*p_wk	= p_param;

	//入力処理
	{	
		KEYBOARD_INPUT	input;
		BOOL						is_shift;
		STRCODE					code;
		input			= KEYBOARD_GetInput( &p_wk->keyboard, &code );
		is_shift	= KEYBOARD_IsShift( &p_wk->keyboard );

		switch( input )
		{	
		case KEYBOARD_INPUT_STR:				//文字入力
			STRINPUT_SetStr( &p_wk->strinput, code );
			break;
		case KEYBOARD_INPUT_CHANGESTR:	//変換文字入力
			STRINPUT_SetChangeStr( &p_wk->strinput, code, is_shift );
			break;
		case KEYBOARD_INPUT_DAKUTEN:		//濁点
			STRINPUT_SetChangeSP( &p_wk->strinput, STRINPUT_SP_CHANGE_DAKUTEN );
			break;
		case KEYBOARD_INPUT_HANDAKUTEN:	//半濁点
			STRINPUT_SetChangeSP( &p_wk->strinput, STRINPUT_SP_CHANGE_HANDAKUTEN );
			break;
		case KEYBOARD_INPUT_BACKSPACE:	//一つ前に戻る
			STRINPUT_BackSpace( &p_wk->strinput );
			break;
		case KEYBOARD_INPUT_CHAGETYPE:	//入力タイプ変更
			//@todo
			break;
		case KEYBOARD_INPUT_EXIT:				//終了	
			SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
			break;
		case KEYBOARD_INPUT_NONE:				//何もしていない
			/*  何もしない */
			break;
		case KEYBOARD_INPUT_SHIFT:			//シフト
			/*  何もしない */
			break;
		default:
			GF_ASSERT(0);
		}
	}

	//メイン処理
	KEYBOARD_Main( &p_wk->keyboard );
	STRINPUT_Main( &p_wk->strinput );
}

//----------------------------------------------------------------------------
/**
 *	@brief	設定画面終了処理
 *
 *	@param	SEQ_WORK *p_seqwk	シーケンスワーク
 *	@param	*p_seq					シーケンス
 *	@param	*p_param				ワーク
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{	
	NAMEIN_WORK	*p_wk = p_param;

	STRINPUT_CopyStr( &p_wk->strinput, p_wk->p_param->strbuf );
	SEQ_End( p_seqwk );
}
