//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		mictest.c
 *	@brief		マイクテスト画面
 *	@author		Toru=Nagihashi
 *	@data		2008.07.03
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]


//#define MIC_FUNC_DEF 
// ↑マイク関連関数の移植方式が定まっていないので一時的関連機能をコメントアウトしています
// 2009.08.31 by hosaka genya
//#define MICTEST_USE_SND
//#define MICTEST_USE_OBJ

//	システム
#include <gflib.h>
#include "calctool.h"
#include "system/gfl_use.h"
#include "system/main.h"
#include "system/wipe.h"
#include "system/brightness.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "print/global_font.h"
#include "font/font.naix"

#include "arc_def.h"

//	外部公開
#include "app/mictest.h"

//	リソース
#include "mictest.naix"			//	アーカイブ
#include "message.naix"//	GMM
#include "msg/msg_mictest.h"//	GMM

// @@@ TODO 未実装関数
static void Snd_BgmFadeOut( int a, int b ){}
static void Snd_BgmFadeIn( int a, int b, int c){}
#define BGM_VOL_MIN (0)
#define BGM_VOL_MAX (127)
#define BGM_FADEIN_START_VOL_NOW (0)

//	@@@ TODO 
//	@@@ OBJ
//	@@@ 読み込み
//	@@@ オートアニメ

/*
 *	マイクテスト任天堂規約
 *
 *	・マイクテスト時、マイクの入力レベルを5 段階で確認できるようにしてください。
 *	・ゲインをPM_SetAmpGainを使用して、PM_AMPGAIN_80（80倍）に設定
 *		→snd_systemの初期化で行っている。
 *	・unsigned char型の8bitサンプリングバッファで0から255の範囲を有効値として、128未満のデータを256から減算し（波形のレベル反転）、最大値を探します。
 *		→※１
 *		これによって得られたデータを以下のような区切りで測定してください。
 *	・LEVEL4	188	〜 255
 *	・LEVEL3	167	〜 187
 *	・LEVEL2	153	〜 166
 *	・LEVEL1	141	〜 152
 *	・LEVEL0	0	〜 140
 *	・サンプリング周波数は任意
 *
 *	・NTSC-ONLINEのDSマニュアル大全集、全文検索あたりで、「マイク」と検索するとガイドラインがでてきます。
 */

//=============================================================================
/**
 *					マクロスイッチ
*/
//=============================================================================
//#define MIC_FORMAT_PCM16	//	定義を外すと8bit

//=============================================================================
/**
 *					定数宣言
*/
//=============================================================================
//-------------------------------------
///		マクロ関数
//=====================================
//	0〜引数-1を返すRand
#define MICTEST_RAND_MAX( m )	( GFUser_GetPublicRand(m) )
//#define MICTEST_RAND_MAX( m )	(gf_rand() * (m+1) / (GF_RAND_MAX + 1))

enum
{ 
  // ゲージ
  MICTEST_RES_M_GAGE_PLTT,
  MICTEST_RES_M_GAGE_CHAR,
  MICTEST_RES_M_GAGE_ANIM,
  // ポケモン
  MICTEST_RES_M_POKE_PLTT,
  MICTEST_RES_M_POKE_CHAR,
  MICTEST_RES_M_POKE_ANIM,
  // 音符
  MICTEST_RES_M_NOTE_PLTT,
  MICTEST_RES_M_NOTE_CHAR,
  MICTEST_RES_M_NOTE_ANIM,
  // ボタン
  MICTEST_RES_S_BTN_PLTT,
  MICTEST_RES_S_BTN_CHAR,
  MICTEST_RES_S_BTN_ANIM,
};

//-------------------------------------
///		OBJの個別個数
//=====================================
#define OBJ_NOTE_MAX	(3)

//-------------------------------------
///		OBJ名称
//=====================================
enum {
	MICTEST_OBJ_GAGE_L,	//ゲージ左
	MICTEST_OBJ_GAGE_R,	//ゲージ右
	MICTEST_OBJ_POKEMON,//ポケモン
	MICTEST_OBJ_BUTTON,	//ボタン
	MICTEST_OBJ_NOTE,	//音符
	MICTEST_OBJ_NOTE_END	=	MICTEST_OBJ_NOTE + OBJ_NOTE_MAX - 1,
	MICTEST_OBJ_MAX,
};

//-------------------------------------
///		BWPWIN
//=====================================
enum {
	MICTEST_BMPWIN_TITLE,
	MICTEST_BMPWIN_INFO,
	MICTEST_BMPWIN_RETBTN,
	MICTEST_BMPWIN_MAX,
};

//-------------------------------------
///		OBJの初期位置
//=====================================
#define OBJ_GAGE_L_POS_X		(128-64)
#define OBJ_GAGE_L_POS_Y		(96)
#define OBJ_GAGE_R_POS_X		(128+64)
#define OBJ_GAGE_R_POS_Y		(96)
#define OBJ_POKEMON_POS_X		(128)
#define OBJ_POKEMON_POS_Y		(96)
#define OBJ_NOTE_POS_X			(OBJ_POKEMON_POS_X)
#define OBJ_NOTE_POS_Y			(OBJ_POKEMON_POS_Y-32)
#define OBJ_BUTTON_POS_X		(27*8)
#define OBJ_BUTTON_POS_Y		(22*8)

//-------------------------------------
///		マイクのサンプリングレート等の処理
//=====================================
#define MIC_SAMPLING_RATE		(8000)
#define MIC_SAMPLING_TIME		(32) // msec
#define MIC_SAMPLING_LENGTH		(MIC_SAMPLING_RATE * MIC_SAMPLING_TIME / 1000)

#ifdef MIC_FORMAT_PCM16
#define MIC_SAMPLING_TYPE		MIC_SAMPLING_TYPE_12BIT
typedef u16 MIC_BUF_SIZE;
#else
#define	MIC_SAMPLING_TYPE		MIC_SAMPLING_TYPE_8BIT
typedef u8 MIC_BUF_SIZE;
#endif	// MIC_FORMAT_PCM16

//-------------------------------------
///		マイク規約によるレベル上限
//=====================================
#define IS_MICINPUT_LEVEL0( n )	(  0<=n && n<=140)
#define IS_MICINPUT_LEVEL1( n )	(141<=n && n<=152)
#define IS_MICINPUT_LEVEL2( n )	(153<=n && n<=166)
#define IS_MICINPUT_LEVEL3( n )	(167<=n && n<=187)
#define IS_MICINPUT_LEVEL4( n )	(188<=n && n<=255)

//-------------------------------------
///		音符が揺れ動く処理の定数
//=====================================
//	ランダム幅
#define NOTE_SPEED_MIN			FX32_CONST(1.2f)
#define NOTE_SPEED_MAX			FX32_CONST(2.2f)
#define NOTE_ANGLE_MIN			(60)
#define NOTE_ANGLE_MAX			(120)
#define NOTE_WIDTH_MIN			(6)
#define NOTE_WIDTH_MAX			(20)
/*#define NOTE_WAIT_MIN			(1)
#define NOTE_WAIT_MAX			(5)
#define NOTE_WAIT_SUM			(6)
*/
#define NOTE_SPEED_RANGE		(NOTE_SPEED_MAX - NOTE_SPEED_MIN + 1)
#define NOTE_ANGLE_RANGE		(NOTE_ANGLE_MAX - NOTE_ANGLE_MIN + 1)
#define NOTE_WIDTH_RANGE		(NOTE_WIDTH_MAX - NOTE_WIDTH_MIN + 1)
//#define NOTE_WAIT_RANGE			(NOTE_WAIT_MAX - NOTE_WAIT_MIN + 1)

//-------------------------------------
///		BMPWINの位置
//=====================================
#define BMPWIN_TITLE_FRAME		(GFL_BG_FRAME0_M)
/*↑[GS_CONVERT_TAG]*/
#define BMPWIN_TITLE_X			(1)
#define BMPWIN_TITLE_Y			(1)
#define BMPWIN_TITLE_W			(9)
#define BMPWIN_TITLE_H			(3)
#define BMPWIN_TITLE_PLT		(0)
#define BMPWIN_TITLE_CHR_OFS	(1)
#define BMPWIN_INFO_FRAME		(GFL_BG_FRAME0_S)
/*↑[GS_CONVERT_TAG]*/
#define BMPWIN_INFO_X			(5)
#define BMPWIN_INFO_Y			(8)
#define BMPWIN_INFO_W			(22)
#define BMPWIN_INFO_H			(5)
#define BMPWIN_INFO_PLT			(0)
#define BMPWIN_INFO_CHR_OFS		(1)
#define BMPWIN_RETBTN_FRAME		(GFL_BG_FRAME0_S)
/*↑[GS_CONVERT_TAG]*/
#define BMPWIN_RETBTN_X			(24)
#define BMPWIN_RETBTN_Y			(21)
#define BMPWIN_RETBTN_W			(6)
#define BMPWIN_RETBTN_H			(2)
#define BMPWIN_RETBTN_PLT		(0)
#define BMPWIN_RETBTN_CHR_OFS	(1+BMPWIN_INFO_W*BMPWIN_INFO_H)

//-------------------------------------
///		カウント
//=====================================
#define GAGE_DECREASE			(2)
#define NOTE_ENTER_CNT_MAX		(10)
#define BGM_FADE_CNT_MAX		(10)

//=============================================================================
/**
 *					構造体宣言
*/
//=============================================================================
//-------------------------------------
///		シーケンス管理
//=====================================
typedef struct _MICTEST_SEQ_WORK MICTEST_SEQ_WORK;
typedef void (*SEQFUNC)( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq );
struct _MICTEST_SEQ_WORK{
	void				*p_wk_adrs;
	SEQFUNC				SeqFunction;
	u32					seq;
	BOOL				is_end;
};

//-------------------------------------
///		音符が動くためのワークエリア
//=====================================
typedef struct {
	GFL_CLWK*		p_act;
	u32					wait;
	VecFx32				pos;
	VecFx32				speed;
	fx32				shake_pt;
	fx32				shake_speed;
	fx32				shake_width;
}NOTE_MOVE_WORK;

//-------------------------------------
//	マイクテストBG用ワーク
//=====================================
typedef struct {
	GFL_BMPWIN	  *bmpwin[MICTEST_BMPWIN_MAX];
	GFL_MSGDATA		*p_msg_mng;	//< MSG描画はBMPWINなのでこちらに
  GFL_FONT      *fontHandle;
}MICTEST_BG_WORK;

//-------------------------------------
///		タスク管理構造体
//=====================================
typedef struct {
	NOTE_MOVE_WORK		note_wk;
	GFL_TCB     *tcb_ptr;
} NOTE_TASK_CTRL;

//-------------------------------------
//	マイクテストOBJ用ワーク
//=====================================
typedef struct {
//	CATS_SYS_PTR		p_catsys;
//	CATS_RES_PTR		p_catres;
	GFL_CLWK*		p_act[MICTEST_OBJ_MAX];
  GFL_CLUNIT* p_cell_unit;

	//	音符用
	NOTE_TASK_CTRL		note_task_ctrl[OBJ_NOTE_MAX];
	u32					note_cnt;
}MICTEST_OBJ_WORK;

//-------------------------------------
//	マイクテストマイク用ワーク
//=====================================
typedef struct {
	MICAutoParam		param;
	void				*p_buf_adrs;
	s32					prev_volume;	//< マイク音量減衰用
	BOOL				anm_set_flg;
	BOOL				is_start;
	BOOL				pre_start_sampling;
	BOOL				mic_use_flag;
	BOOL				pre_mic_use;
}MICTEST_MIC_WORK;

//-------------------------------------
//	マイクテストメインワーク
//=====================================
typedef struct {
	MICTEST_OBJ_WORK	obj;
	MICTEST_BG_WORK		bg;
	MICTEST_MIC_WORK	mic;
	MICTEST_SEQ_WORK	seq;
  void* tcbWork;
  GFL_TCBSYS* tcbSys;
  GFL_TCB  *tcbVBlank;

	int	heap_id;
}MICTEST_MAIN_WORK;

//オーバーレイプロセス定義データ
const GFL_PROC_DATA TitleMicTestProcData = {
	MicTestProc_Init,
	MicTestProc_Main,
	MicTestProc_Exit,
//	FS_OVERLAY_ID(mictest),
//	NO_OVERLAY_ID,
};


//=============================================================================
/**
 *					プロトタイプ宣言
*/
//=============================================================================
//-------------------------------------
///		アプリケーション
//=====================================
static void MicTest_InitApplication( u32 heap_id );
static void MicTest_ExitApplication( void );
static void MicTest_SetVramBank( void );
static void MicTest_VBlankFunction( GFL_TCB * tcb, void * p_wk_adrs );

//-------------------------------------
///		OBJ管理
//=====================================
static void MicTest_OBJ_Init( MICTEST_OBJ_WORK *p_obj, u32 heap_id );
static void MicTest_OBJ_Exit( MICTEST_OBJ_WORK *p_obj );
static void MicTest_OBJ_Main( MICTEST_OBJ_WORK *p_obj );
static void MicTest_OBJ_SetClact( MICTEST_OBJ_WORK *p_obj );
static void MicTest_OBJ_DeleteClact( MICTEST_OBJ_WORK *p_obj );
static void MicTest_OBJ_VBlancFunction( MICTEST_OBJ_WORK *p_obj );

//-------------------------------------
///		BG管理
//=====================================
static void MicTest_BG_Init( MICTEST_BG_WORK *p_bg, u32 heap_id );
static void MicTest_BG_Exit( MICTEST_BG_WORK *p_bg );
static void MicTest_BG_Main( MICTEST_BG_WORK *p_bg );
static void MicTestBG_LoadBg( MICTEST_BG_WORK *p_bg, u32 heap_id );
static void MicTestBG_UnLoadBg( MICTEST_BG_WORK *p_bg );
static void MicTest_BG_VBlancFunction( MICTEST_BG_WORK *p_bg );
static void MicTest_BG_CreateMsg( MICTEST_BG_WORK *p_bg, u32 heap_id );
static void MicTest_BG_DeleteMsg( MICTEST_BG_WORK *p_bg );

//-------------------------------------
///		マイク管理
//=====================================
static void MicTest_MIC_Init( MICTEST_MIC_WORK *p_mic, u32 heap_id, MICCallback MicCallback, void*p_mic_arg );
static void MicTest_MIC_Exit( MICTEST_MIC_WORK *p_mic );
static void MicTest_MIC_Main( MICTEST_MIC_WORK *p_mic );
static void MicTest_MIC_StartSampling( MICTEST_MIC_WORK *p_mic );
static void MicTest_MIC_StopSampling( MICTEST_MIC_WORK *p_mic );

static void ChangeObjAnmMicCallback( MICResult result, void *p_arg );
static int CalcAverageVolumeMicBuffer( MICAutoParam * p_mic_param );
static u8 GetMicLevelToAnmSeqNum( u8 volume );

//-------------------------------------
///		TASK関係
//=====================================
//	Note
static BOOL MoveStartNoteCtrl( MICTEST_OBJ_WORK *p_wk, u32 heap_id, s16 x, s16 y, GFL_TCBSYS* tcbSys );
static void MoveStartNoteTask( GFL_TCBSYS* tcbSys, GFL_CLWK*	p_act, NOTE_TASK_CTRL *p_task_wk, /*u32 heap_id,*/ s16 x, s16 y, fx32 speed, u32 angle, u32 width, u32 wait );
static void MoveNoteTask( GFL_TCB* p_tcb, void *p_wk_adrs );
/*↑[GS_CONVERT_TAG]*/
static void CleanNoteTask( MICTEST_OBJ_WORK *p_obj );
static BOOL IsEmptyNoteTask( const MICTEST_OBJ_WORK *cp_obj );

//-------------------------------------
///		SEQ
//=====================================
static void MicTest_SEQ_Init( MICTEST_SEQ_WORK *p_seq, void * p_wk_adrs, SEQFUNC SeqFunction );
static void MicTest_SEQ_Main( MICTEST_SEQ_WORK *p_seq );
static void MicTest_SEQ_ChangeSeq( MICTEST_SEQ_WORK *p_seq, SEQFUNC SeqFunction );
static void* MicTest_SEQ_GetWk( const MICTEST_SEQ_WORK *cp_seq );
static BOOL MicTest_SEQ_IsEnd( const MICTEST_SEQ_WORK *cp_seq );
static void MicTest_SEQ_End( MICTEST_SEQ_WORK *p_seq );

//-------------------------------------
///		SEQ関数
//=====================================
static void SEQFUNC_FadeIn( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq );
static void SEQFUNC_Main( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq );
static void SEQFUNC_RetBtn( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq );
static void SEQFUNC_FadeOut( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq );
static void SEQFUNC_End( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq );

//-------------------------------------
///		その他
//=====================================
static int GetHitTrg( void );	//	Trg検知（座標テーブル内包）
static BOOL IsProcEndTrg( void );


//=============================================================================
/**
 *					データ領域
*/
//=============================================================================
//-------------------------------------
///		BG面設定データ	（本来MicTest_BG_Init内部におく方がよいが
//							MicTest_BG_Exitで解放に使うため外部におく）
//=====================================
static const struct {
	GFL_BG_BGCNT_HEADER	header;
	u8 frame;
   	u8 mode;
} sc_bg_cnt_data[] = {
	{
		{	0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
/*↑[GS_CONVERT_TAG]*/
			GX_BG_SCRBASE_0xf800,GX_BG_CHARBASE_0x00000,GX_BG_EXTPLTT_01,
			0,0,0,FALSE },
		GFL_BG_FRAME0_M,
/*↑[GS_CONVERT_TAG]*/
		GFL_BG_MODE_TEXT,
/*↑[GS_CONVERT_TAG]*/
	},
	{
		{	0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
/*↑[GS_CONVERT_TAG]*/
			GX_BG_SCRBASE_0xf000,GX_BG_CHARBASE_0x04000,GX_BG_EXTPLTT_01,
			1,0,0,FALSE },
		GFL_BG_FRAME1_M,
/*↑[GS_CONVERT_TAG]*/
		GFL_BG_MODE_TEXT,
/*↑[GS_CONVERT_TAG]*/
	},
	{
		{	0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
/*↑[GS_CONVERT_TAG]*/
			GX_BG_SCRBASE_0xe800,GX_BG_CHARBASE_0x08000,GX_BG_EXTPLTT_01,
			2,0,0,FALSE },
		GFL_BG_FRAME2_M,
/*↑[GS_CONVERT_TAG]*/
		GFL_BG_MODE_TEXT,
/*↑[GS_CONVERT_TAG]*/
	},
	{
		{ 0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
/*↑[GS_CONVERT_TAG]*/
			GX_BG_SCRBASE_0xf800,GX_BG_CHARBASE_0x00000,GX_BG_EXTPLTT_01,
			0,0,0,FALSE },
		GFL_BG_FRAME0_S,
/*↑[GS_CONVERT_TAG]*/
		GFL_BG_MODE_TEXT,
/*↑[GS_CONVERT_TAG]*/
	},
	{
		{ 0,0,0x800,0,GFL_BG_SCRSIZ_256x256,GX_BG_COLORMODE_16,
/*↑[GS_CONVERT_TAG]*/
			GX_BG_SCRBASE_0xf000,GX_BG_CHARBASE_0x04000,GX_BG_EXTPLTT_01,
			1,0,0,FALSE },
		GFL_BG_FRAME1_S,
/*↑[GS_CONVERT_TAG]*/
		GFL_BG_MODE_TEXT,
/*↑[GS_CONVERT_TAG]*/
	},
};

//=============================================================================
/**
 *					gloval function
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	プロック開始処理
 *
 *	@param	GFL_PROC *proc	プロックワーク
 *	@param	*seq		シーケンス番号
 *	@param	*pwk
 *	@param	*mywk 
 *
 *	@return	プロック状態
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT MicTestProc_Init( GFL_PROC *proc,int *seq, void *pwk, void *mywk )
{
	MICTEST_MAIN_WORK *p_wk;

	//ヒープ作成
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_MICTEST, 0x30000 );
	
	p_wk = GFL_PROC_AllocWork( proc,sizeof(MICTEST_MAIN_WORK),HEAPID_MICTEST );
	MI_CpuClear8( p_wk, sizeof(MICTEST_MAIN_WORK) );

	p_wk->heap_id = HEAPID_MICTEST;

	//	appli
	MicTest_InitApplication( p_wk->heap_id );
	MicTest_SetVramBank();

  // TCB
	p_wk->tcbWork = GFL_HEAP_AllocMemory( p_wk->heap_id , GFL_TCB_CalcSystemWorkSize( 5 ) );
	p_wk->tcbSys = GFL_TCB_Init( 5 , p_wk->tcbWork );
  
  p_wk->tcbVBlank = GFUser_VIntr_CreateTCB( MicTest_VBlankFunction, p_wk, 1 );

	//	BG
	MicTest_BG_Init( &p_wk->bg, p_wk->heap_id );
	MicTestBG_LoadBg( &p_wk->bg, p_wk->heap_id );
	MicTest_BG_CreateMsg( &p_wk->bg, p_wk->heap_id );

	//	OBJ
#ifdef MICTEST_USE_OBJ
	MicTest_OBJ_Init( &p_wk->obj, p_wk->heap_id );
	MicTest_OBJ_SetClact( &p_wk->obj );
#endif

	//	MIC
	MicTest_MIC_Init( &p_wk->mic, p_wk->heap_id, ChangeObjAnmMicCallback, p_wk );

	//	SEQ
	MicTest_SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_FadeIn );

	//	曲はフェードアウト
	Snd_BgmFadeOut( BGM_VOL_MIN, BGM_FADE_CNT_MAX );

	return GFL_PROC_RES_FINISH;
}


// タイトルへ飛ぶ
//extern void		Main_SetNextProc(FSOverlayID ov_id, const GFL_PROC_DATA * proc_data);
extern const GFL_PROC_DATA TitleProcData;
FS_EXTERN_OVERLAY( title );

//----------------------------------------------------------------------------
/**
 *	@brief	プロック終了処理
 *
 *	@param	GFL_PROC *proc	プロックワーク
 *	@param	*seq		シーケンス番号
 *	@param	*pwk
 *	@param	*mywk 
 *
 *	@return	プロック状態
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT MicTestProc_Exit( GFL_PROC *proc,int *seq, void *pwk, void *mywk )
{
	MICTEST_MAIN_WORK* p_wk = mywk;

  GFL_TCB_DeleteTask( p_wk->tcbVBlank );

  GFL_TCB_Exit( p_wk->tcbSys );
  GFL_HEAP_FreeMemory( p_wk->tcbWork );

	//	MIC
	MicTest_MIC_Exit( &p_wk->mic );

	//	OBJ
#ifdef MICTEST_USE_OBJ
	MicTest_OBJ_DeleteClact( &p_wk->obj );
	MicTest_OBJ_Exit( &p_wk->obj );
#endif

	//	BG
	MicTest_BG_DeleteMsg( &p_wk->bg );
	MicTest_BG_Exit( &p_wk->bg );

	//	appli
	MicTest_ExitApplication();

	//ワークエリア解放
	GFL_PROC_FreeWork( proc );

	GFL_HEAP_DeleteHeap( p_wk->heap_id );

	//	曲はフェードイン
	Snd_BgmFadeIn( BGM_VOL_MAX, BGM_FADE_CNT_MAX, BGM_FADEIN_START_VOL_NOW );

  GFL_PROC_SysSetNextProc( FS_OVERLAY_ID(title), &TitleProcData, NULL );
//	Main_SetNextProc( FS_OVERLAY_ID(title), &TitleProcData );

	return GFL_PROC_RES_FINISH;
}

//-----------------------------------------------------------------------------
/**
 *	@brief	プロックメイン処理
 *
 *	@param	GFL_PROC *proc	プロックワーク
 *	@param	*seq		シーケンス番号
 *	@param	*pwk
 *	@param	*mywk 
 *
 *	@return	プロック状態
 */
//-----------------------------------------------------------------------------
GFL_PROC_RESULT MicTestProc_Main( GFL_PROC *proc,int *seq, void *pwk, void *mywk )
{
	MICTEST_MAIN_WORK* p_wk = mywk;

#ifdef PM_DEBUG
  // @@@ TODO とりあえずデバッグボタンで抜ける
  if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_DEBUG ) {
    return GFL_PROC_RES_FINISH;
  }
#endif

	if( MicTest_SEQ_IsEnd( &p_wk->seq ) ) {
		return GFL_PROC_RES_FINISH;
	}
	MicTest_SEQ_Main( &p_wk->seq );

#ifdef MICTEST_USE_OBJ
	MicTest_OBJ_Main( &p_wk->obj );
#endif
	MicTest_BG_Main( &p_wk->bg );
	MicTest_MIC_Main( &p_wk->mic );

	GFL_TCB_Main( p_wk->tcbSys );

	return GFL_PROC_RES_CONTINUE;
/*↑[GS_CONVERT_TAG]*/
}
//=============================================================================
/**
 *				シーケンス	seq
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	シーケンスシステム初期化
 *
 *	@param	MICTEST_SEQ_WORK *p_seq	シーケンス管理ワーク
 *	@param	* p_wk_adrs				シーケンス関数に渡すワーク
 *	@param	SeqFunction				最初に実行するシーケンス関数
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void MicTest_SEQ_Init( MICTEST_SEQ_WORK *p_seq, void * p_wk_adrs, SEQFUNC SeqFunction )
{
	p_seq->p_wk_adrs	= p_wk_adrs;
	p_seq->is_end		= FALSE;

	MicTest_SEQ_ChangeSeq( p_seq, SeqFunction );
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーケンスメイン処理
 *
 *	@param	MICTEST_SEQ_WORK *p_seq		シーケンス管理ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_SEQ_Main( MICTEST_SEQ_WORK *p_seq )
{
	if( MicTest_SEQ_IsEnd( p_seq ) == FALSE ) {
		p_seq->SeqFunction( p_seq, &p_seq->seq );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーケンス切り替え
 *
 *	@param	MICTEST_SEQ_WORK *p_seq	シーケンス管理ワーク
 *	@param	SeqFunction				切り替えるシーケンス関数
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_SEQ_ChangeSeq( MICTEST_SEQ_WORK *p_seq, SEQFUNC SeqFunction )
{
	p_seq->SeqFunction	= SeqFunction;
	p_seq->seq			= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーケンス関数に渡すワーク取得
 *
 *	@param	const MICTEST_SEQ_WORK *cp_seq	シーケンス管理ワーク 
 *
 *	@return	シーケンス関数に渡すワークのアドレス
 */
//-----------------------------------------------------------------------------
static void* MicTest_SEQ_GetWk( const MICTEST_SEQ_WORK *cp_seq )
{
	return cp_seq->p_wk_adrs;
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーケンスが終了したかどうか
 *
 *	@param	const MICTEST_SEQ_WORK *cp_seq	シーケンス管理ワーク
 *
 *	@return	シーケンスが終了したか
 */
//-----------------------------------------------------------------------------
static BOOL MicTest_SEQ_IsEnd( const MICTEST_SEQ_WORK *cp_seq )
{
	return cp_seq->is_end;
}

//----------------------------------------------------------------------------
/**
 *	@brief	シーケンスシステム終了
 *
 *	@param	MICTEST_SEQ_WORK *p_seq 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void MicTest_SEQ_End( MICTEST_SEQ_WORK *p_seq )
{
	p_seq->is_end = TRUE;
}

//=============================================================================
/**
 *				シーケンス関数
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	フェードイン
 *
 *	@param	MICTEST_SEQ_WORK *p_seq_wk	シーケンス管理ワーク
 *	@param	*p_seq					シーケンス番号
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeIn( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq )
{
	enum {
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_END,
	};
	MICTEST_MAIN_WORK *p_wk	= MicTest_SEQ_GetWk( p_seq_wk );

	switch( *p_seq ) {
	case SEQ_INIT :
		WIPE_SYS_Start(WIPE_PATTERN_WMS, WIPE_TYPE_FADEIN, WIPE_TYPE_FADEIN,
        0x0000, COMM_BRIGHTNESS_SYNC, 1, p_wk->heap_id );
		*p_seq = SEQ_MAIN;
		break;
	
	case SEQ_MAIN:
		if( WIPE_SYS_EndCheck() ){
			*p_seq = SEQ_END;
		}
		break;

	case SEQ_END:
		MicTest_SEQ_ChangeSeq( p_seq_wk, SEQFUNC_Main );
		break;

	default :
		GF_ASSERT( 0 );
	};
}

//----------------------------------------------------------------------------
/**
 *	@brief	マイクテストメイン処理
 *
 *	@param	MICTEST_SEQ_WORK *p_seq_wk	シーケンス管理ワーク
 *	@param	*p_seq					シーケンス番号
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Main( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq )
{
	enum {
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_EXIT,
		SEQ_END,
	};
	MICTEST_MAIN_WORK *p_wk	= MicTest_SEQ_GetWk( p_seq_wk );

	switch( *p_seq ) {
	case SEQ_INIT :
		MicTest_MIC_StartSampling( &p_wk->mic );
		*p_seq = SEQ_MAIN;
		break;

	case SEQ_MAIN:
		if( IsProcEndTrg() ) {
#if MICTEST_USE_SND
			GFL_SOUND_PlaySE( SEQ_SE_DP_SELECT );
#endif
			*p_seq = SEQ_EXIT;
		}
		break;

	case SEQ_EXIT:
		MicTest_MIC_StopSampling( &p_wk->mic );
		*p_seq = SEQ_END;
		break;

	case SEQ_END:
		MicTest_SEQ_ChangeSeq( p_seq_wk, SEQFUNC_RetBtn );
		break;

	default :
		GF_ASSERT( 0 );
	};
}

//----------------------------------------------------------------------------
/**
 *	@brief	戻るボタンを押したときの処理
 *
 *	@param	MICTEST_SEQ_WORK *p_seq_wkq	シーケンス管理ワーク
 *	@param	*p_seq					シーケンス番号
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_RetBtn( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq )
{
	enum {
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_END,
	};
	MICTEST_MAIN_WORK *p_wk	= MicTest_SEQ_GetWk( p_seq_wk );

#ifdef MICTEST_USE_OBJ
	switch( *p_seq ) {
	case SEQ_INIT :
		GFL_CLACT_WK_SetAnmSeq( p_wk->obj.p_act[MICTEST_OBJ_BUTTON], 1 );
/*↑[GS_CONVERT_TAG]*/
		*p_seq = SEQ_MAIN;
		break;

	case SEQ_MAIN :
		if( GFL_CLACT_WK_CheckAnmActiveCap( p_wk->obj.p_act[MICTEST_OBJ_BUTTON] ) == FALSE ) {
/*↑[GS_CONVERT_TAG]*/
			*p_seq = SEQ_END;
		}
		break;

	case SEQ_END:
		MicTest_SEQ_ChangeSeq( p_seq_wk, SEQFUNC_FadeOut );
		break;

	default :
		GF_ASSERT( 0 );
	};
#else
  // @@@
  *p_seq = SEQ_END;
#endif


}

//----------------------------------------------------------------------------
/**
 *	@brief	フェードアウト
 *
 *	@param	MICTEST_SEQ_WORK *p_seq_wk	シーケンス管理ワーク
 *	@param	*p_seq					シーケンス番号
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeOut( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq )
{
	enum {
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_END,
	};
	MICTEST_MAIN_WORK *p_wk	= MicTest_SEQ_GetWk( p_seq_wk );

	switch( *p_seq ) {
	case SEQ_INIT :
		WIPE_SYS_Start(WIPE_PATTERN_WMS,
					WIPE_TYPE_FADEOUT,
					WIPE_TYPE_FADEOUT, 0x0000, COMM_BRIGHTNESS_SYNC, 1, p_wk->heap_id
					);
		*p_seq = SEQ_MAIN;
		break;

	case SEQ_MAIN:
		if( WIPE_SYS_EndCheck() ){
			*p_seq = SEQ_END;
		}
		break;

	case SEQ_END:
		MicTest_SEQ_ChangeSeq( p_seq_wk, SEQFUNC_End );
		break;

	default :
		GF_ASSERT( 0 );
	};
}

//----------------------------------------------------------------------------
/**
 *	@brief	終了処理
 *
 *	@param	MICTEST_SEQ_WORK *p_seq_wk	シーケンス管理ワーク
 *	@param	*p_seq					シーケンス番号
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( MICTEST_SEQ_WORK *p_seq_wk, u32 *p_seq )
{
	MICTEST_MAIN_WORK *p_wk	= MicTest_SEQ_GetWk( p_seq_wk );
	CleanNoteTask( &p_wk->obj );
	MicTest_SEQ_End( p_seq_wk );
}

//=============================================================================
/**
 *					system
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	アプリケーション初期化
 *
 *	@param	u32 heap_id		ヒープID
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_InitApplication( u32 heap_id )
{
	//	割り込み終了
//  sys_VBlankFuncChange(NULL,NULL);	
//	sys_HBlankIntrStop();

	//	表示設定解除
	GFL_DISP_GX_InitVisibleControl();
/*↑[GS_CONVERT_TAG]*/
	GFL_DISP_GXS_InitVisibleControl();
/*↑[GS_CONVERT_TAG]*/
	GX_SetVisiblePlane( 0 );
	GXS_SetVisiblePlane( 0 );

	//	Wnd設定解除
	WIPE_ResetWndMask( WIPE_DISP_MAIN );
	WIPE_ResetWndMask( WIPE_DISP_SUB );	

	//	VRAMマネージャ初期化
//	initVramTransferManagerHeap( 32, heap_id );
}

//----------------------------------------------------------------------------
/**
 *	@brief	アプリケーション終了
 *
 *	@param	void 
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_ExitApplication( void )
{
	//	VRAMマネージャ終了
//	DellVramTransferManager();

	//	割り込み終了
//	sys_VBlankFuncChange(NULL,NULL);	
//	sys_HBlankIntrStop();

	//	非表示
	GFL_DISP_GX_InitVisibleControl();
/*↑[GS_CONVERT_TAG]*/
	GFL_DISP_GXS_InitVisibleControl();	
/*↑[GS_CONVERT_TAG]*/
	GX_SetVisiblePlane( 0 );
	GXS_SetVisiblePlane( 0 );	
}


//----------------------------------------------------------------------------
/**
 *	@brief	バンクテーブルセット
 *
 *	@param	void 
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_SetVramBank( void )
{
	static const GFL_DISP_VRAM vram_set_table = {
/*↑[GS_CONVERT_TAG]*/
		GX_VRAM_BG_128_A,				// メイン2DエンジンのBG
		GX_VRAM_BGEXTPLTT_NONE,			// メイン2DエンジンのBG拡張パレット
		GX_VRAM_SUB_BG_128_C,			// サブ2DエンジンのBG
		GX_VRAM_SUB_BGEXTPLTT_NONE,		// サブ2DエンジンのBG拡張パレット
		GX_VRAM_OBJ_128_B,				// メイン2DエンジンのOBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// メイン2DエンジンのOBJ拡張パレット
		GX_VRAM_SUB_OBJ_16_I,			// サブ2DエンジンのOBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,	// サブ2DエンジンのOBJ拡張パレット
		GX_VRAM_TEX_NONE,				// テクスチャイメージスロット
		GX_VRAM_TEXPLTT_NONE			// テクスチャパレットスロット
	};

	GFL_DISP_SetBank( &vram_set_table );
/*↑[GS_CONVERT_TAG]*/
}

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank関数
 *
 *	@param	GFL_TCB * tcb
 *	@param	void * work 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void MicTest_VBlankFunction( GFL_TCB * tcb, void * p_wk_adrs )
{
	MICTEST_MAIN_WORK	*p_wk	= p_wk_adrs;

	NNS_GfdDoVramTransfer();	//VRam転送マネージャ実行
#ifdef MICTEST_USE_OBJ
	MicTest_OBJ_VBlancFunction( &p_wk->obj );
#endif
	MicTest_BG_VBlancFunction( &p_wk->bg );

	OS_SetIrqCheckFlag( OS_IE_V_BLANK);
}


//=============================================================================
/**
 *		OBJ管理
 */
//=============================================================================

#ifdef MICTEST_USE_OBJ
//----------------------------------------------------------------------------
/**
 *	@brief	OBJシステム初期化
 *
 *	@param	MicTest_OBJ_WORK *p_obj	OBJ用ワーク
 *	@param	heap_id					ヒープID
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_OBJ_Init( MICTEST_OBJ_WORK *p_obj, u32 heap_id )
{
	//	システム作成
	p_obj->p_catsys	= CATS_AllocMemory( heap_id );

	//	システム設定
	{
		static const TCATS_OAM_INIT	oam_init = {
			0, 128,	//	メインOAM管理領域
			0, 32,	//	メインアフィン管理領域
			0, 128,
			0, 32,
		};
		static const TCATS_CHAR_MANAGER_MAKE chr_man = {
			MICTEST_OBJ_MAX,
			1024*128,	//	メイン画面サイズ（byte）
			1024*16,
			GX_OBJVRAMMODE_CHAR_1D_32K,
			GX_OBJVRAMMODE_CHAR_1D_32K
		};
		CATS_SystemInit( p_obj->p_catsys, &oam_init, &chr_man, 32 );
	}
	REND_OAM_UtilOamRamClear_Main( heap_id );
	REND_OAM_UtilOamRamClear_Sub( heap_id );


	p_obj->p_catres	= CATS_ResourceCreate( p_obj->p_catsys );
	CATS_ClactSetInit( p_obj->p_catsys, p_obj->p_catres, MICTEST_OBJ_MAX );

	//	リソース読み込み
	{
		static const TCATS_RESOURCE_NUM_LIST num_list = {
			32,		//	キャラ数
			32,		//	パレット数
			32,		//	セル数
			32,		//	セルアニメ数
			0,		//	マルチセル数
			0,		//	マルチセルアニメ数
		};
		
		CATS_ResourceManagerInit( p_obj->p_catsys, p_obj->p_catres, &num_list );

	}

	//	表示
	GFL_DISP_GX_SetVisibleControl(  GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJシステム破棄
 *
 *	@param	MicTest_OBJ_WORK *p_obj	OBJ用ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_OBJ_Exit( MICTEST_OBJ_WORK *p_obj )
{
	CATS_FreeMemory( p_obj->p_catsys );
	p_obj->p_catsys	= NULL;	
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJシステムメイン処理
 *
 *	@param	MICTEST_OBJ_WORK *p_obj 
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_OBJ_Main( MICTEST_OBJ_WORK *p_obj )
{
	u16	i;
	for( i = 0; i < MICTEST_OBJ_MAX; i++ ){
		GFL_CLACT_WK_AddAnmFrameCap( p_obj->p_act[i] );
	}

	GFL_CLACT_SYS_Main();
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJCLACT生成
 *
 *	@param	MICTEST_OBJ_WORK *p_obj 
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_OBJ_SetClact( MICTEST_OBJ_WORK *p_obj )
{
	u16	i, param_no;
	//	セルアクターのリソース番号
	enum {
		//	ゲージ
		MICTEST_CLRES_CHR_ID_GAGE	= 0xA01,
		MICTEST_CLRES_CEL_ID_GAGE	= 0xA02,
		MICTEST_CLRES_PLT_ID_GAGE	= 0xA03,
		MICTEST_CLRES_ANM_ID_GAGE	= 0xA04,
		//	ポケモン
		MICTEST_CLRES_CHR_ID_POKEMON	= 0xB01,
		MICTEST_CLRES_CEL_ID_POKEMON	= 0xB02,
		MICTEST_CLRES_PLT_ID_POKEMON	= 0xB03,
		MICTEST_CLRES_ANM_ID_POKEMON	= 0xB04,
		//	ボタン
		MICTEST_CLRES_CHR_ID_BUTTON	= 0xD01,
		MICTEST_CLRES_CEL_ID_BUTTON	= 0xD02,
		MICTEST_CLRES_PLT_ID_BUTTON	= 0xD03,
		MICTEST_CLRES_ANM_ID_BUTTON	= 0xD04,
		//	音符
		MICTEST_CLRES_CHR_ID_NOTE	= 0xC01,
		MICTEST_CLRES_CEL_ID_NOTE	= 0xC02,
		MICTEST_CLRES_PLT_ID_NOTE	= 0xC03,
		MICTEST_CLRES_ANM_ID_NOTE	= 0xC04,
	};

	static const TCATS_OBJECT_ADD_PARAM_S sc_obj_add_param[] =
	{
		//	s16 x;				///< [ X ] 座標
		//	s16 y;				///< [ Y ] 座標
		//	s16 z;				///< [ Z ] 座標
		//	
		//	u16	anm;			///< アニメ番号
		//	int pri;			///< 優先順位
		//	int	pal;			///< パレット番号
		//	int d_area;			///< 描画エリア
		//
		//	id[]				///< 仕様リソースIDテーブル
		//	bg_pri				///< BG面への優先度
		//	vram_trans			///< VRAM転送フラグ

		//	gage_l
		{
			OBJ_GAGE_L_POS_X, OBJ_GAGE_L_POS_Y, 0, 0, 0, 0, CATS_D_AREA_MAIN,
			{	MICTEST_CLRES_CHR_ID_GAGE, MICTEST_CLRES_PLT_ID_GAGE,
				MICTEST_CLRES_CEL_ID_GAGE, MICTEST_CLRES_ANM_ID_GAGE, 0, 0, }
		},
		//	gage_r
		{
			OBJ_GAGE_R_POS_X, OBJ_GAGE_R_POS_Y, 0, 0, 0, 0, CATS_D_AREA_MAIN,
			{	MICTEST_CLRES_CHR_ID_GAGE, MICTEST_CLRES_PLT_ID_GAGE,
				MICTEST_CLRES_CEL_ID_GAGE, MICTEST_CLRES_ANM_ID_GAGE, 0, 0, }
		},
		//	pokemon
		{
			OBJ_POKEMON_POS_X, OBJ_POKEMON_POS_Y, 0, 0, 0, 0, CATS_D_AREA_MAIN,
			{	MICTEST_CLRES_CHR_ID_POKEMON, MICTEST_CLRES_PLT_ID_POKEMON,
				MICTEST_CLRES_CEL_ID_POKEMON, MICTEST_CLRES_ANM_ID_POKEMON, 0, 0, }
		},
		//	button
		{
			OBJ_BUTTON_POS_X, OBJ_BUTTON_POS_Y, 0, 0, 0, 0, CATS_D_AREA_SUB,
			{	MICTEST_CLRES_CHR_ID_BUTTON, MICTEST_CLRES_PLT_ID_BUTTON,
				MICTEST_CLRES_CEL_ID_BUTTON, MICTEST_CLRES_ANM_ID_BUTTON, 1, 0, }
		},
		//	note
		{
			0, 0, 0, 0, 0, 0, CATS_D_AREA_MAIN,
			{	MICTEST_CLRES_CHR_ID_NOTE, MICTEST_CLRES_PLT_ID_NOTE,
				MICTEST_CLRES_CEL_ID_NOTE, MICTEST_CLRES_ANM_ID_NOTE, 0, 0, }
		},

	};
	static const u32 sc_act_res_tbl[MICTEST_OBJ_MAX]	=
		{ 0, 1, 2, 3, 4, 4, 4 };

	//	ゲージ
	CATS_LoadResourcePlttArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_gage_NCLR, 0, 1, NNS_G2D_VRAM_TYPE_2DMAIN, MICTEST_CLRES_PLT_ID_GAGE );
	CATS_LoadResourceCellArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_gage_NCER, 0, MICTEST_CLRES_CEL_ID_GAGE );
	CATS_LoadResourceCellAnmArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_gage_NANR, 0, MICTEST_CLRES_ANM_ID_GAGE );
	CATS_LoadResourceCharArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_gage_NCGR, 0, NNS_G2D_VRAM_TYPE_2DMAIN,  MICTEST_CLRES_CHR_ID_GAGE );
	//	ポケモン
	CATS_LoadResourcePlttArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_pokemon_NCLR, 0, 1, NNS_G2D_VRAM_TYPE_2DMAIN, MICTEST_CLRES_PLT_ID_POKEMON );
	CATS_LoadResourceCellArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_pokemon_NCER, 0, MICTEST_CLRES_CEL_ID_POKEMON );
	CATS_LoadResourceCellAnmArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_pokemon_NANR, 0, MICTEST_CLRES_ANM_ID_POKEMON );
	CATS_LoadResourceCharArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_pokemon_NCGR, 0, NNS_G2D_VRAM_TYPE_2DMAIN,  MICTEST_CLRES_CHR_ID_POKEMON );
	//	ボタン
	CATS_LoadResourcePlttArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_button_NCLR, 0, 1, NNS_G2D_VRAM_TYPE_2DSUB, MICTEST_CLRES_PLT_ID_BUTTON );
	CATS_LoadResourceCellArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_button_NCER, 0, MICTEST_CLRES_CEL_ID_BUTTON );
	CATS_LoadResourceCellAnmArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_button_NANR, 0, MICTEST_CLRES_ANM_ID_BUTTON );
	CATS_LoadResourceCharArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_button_NCGR, 0, NNS_G2D_VRAM_TYPE_2DSUB,  MICTEST_CLRES_CHR_ID_BUTTON );	
	//	音符
	CATS_LoadResourcePlttArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_note_NCLR, 0, 1, NNS_G2D_VRAM_TYPE_2DMAIN, MICTEST_CLRES_PLT_ID_NOTE );
	CATS_LoadResourceCellArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_note_NCER, 0, MICTEST_CLRES_CEL_ID_NOTE );
	CATS_LoadResourceCellAnmArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_note_NANR, 0, MICTEST_CLRES_ANM_ID_NOTE );
	CATS_LoadResourceCharArc( p_obj->p_catsys, p_obj->p_catres, ARCID_MICTEST_GRA, NARC_mictest_obj_note_NCGR, 0, NNS_G2D_VRAM_TYPE_2DMAIN,  MICTEST_CLRES_CHR_ID_NOTE );

	//	アクター生成
	for( i = 0; i < MICTEST_OBJ_MAX; i++ ){
		p_obj->p_act[i] = CATS_ObjectAdd_S( p_obj->p_catsys, p_obj->p_catres, &sc_obj_add_param[ sc_act_res_tbl[i] ] );
		// オートアニメフラグセット
		GFL_CLACT_WK_SetAutoAnmFlagCap( p_obj->p_act[i], CATS_ANM_AUTO_ON );
/*↑[GS_CONVERT_TAG]*/
	}

	//	音符は非表示
	for( i = 0; i < OBJ_NOTE_MAX; i++ ) {
		GFL_CLACT_UNIT_SetDrawEnableCap( p_obj->p_act[ MICTEST_OBJ_NOTE + i ], CATS_ENABLE_FALSE );
/*↑[GS_CONVERT_TAG]*/
	}

	//	個別設定
	GFL_CLACT_WK_SetFlipCap(p_obj->p_act[MICTEST_OBJ_GAGE_L], CLACT_FLIP_H);
/*↑[GS_CONVERT_TAG]*/
	GFL_CLACT_WK_SetBgPriCap( p_obj->p_act[MICTEST_OBJ_BUTTON], 1 );
/*↑[GS_CONVERT_TAG]*/

	p_obj->note_cnt	= NOTE_ENTER_CNT_MAX;
}

//----------------------------------------------------------------------------
/**
 *	@brief	CLACT終了
 *
 *	@param	MICTEST_OBJ_WORK *p_obj 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static void MicTest_OBJ_DeleteClact( MICTEST_OBJ_WORK *p_obj )
{
	int i;
	
  // セルアクターセット破棄
	GFL_CLACT_UNIT_Delete( p_obj->p_cell_unit );

	//OAMレンダラー破棄
  GFL_CLACT_SYS_Delete();

  // @@@ アクター、リソース開放
	for( i = 0;i < MICTEST_OBJ_MAX;i++ ){
		if( p_obj->p_act[i] != NULL ){
			CATS_ActorPointerDelete_S( p_obj->p_act[i] );
		}
	}

	CATS_ResourceDestructor_S( p_obj->p_catsys, p_obj->p_catres );
}

//----------------------------------------------------------------------------
/**
 *	@brief	OBJVBlanc内処理
 *
 *	@param	MICTEST_OBJ_WORK *p_obj	OBJ管理ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_OBJ_VBlancFunction( MICTEST_OBJ_WORK *p_obj )
{
	if( p_obj->p_catsys != NULL ) {
		CATS_RenderOamTrans();
	}
}

#endif

//=============================================================================
/**
 *		BG管理
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BGシステム初期化
 *
 *	@param	MICTEST_BG_WORK *p_bg	BG管理ワーク
 *	@param	heap_id					管理用ワークアロック用ヒープ
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_Init( MICTEST_BG_WORK *p_bg, u32 heap_id )
{
	//	システム
  GFL_BG_Init( heap_id );
  
  // BMPWIN
  GFL_BMPWIN_Init( heap_id );

	//	システム初期化
	{
		static const GFL_BG_SYS_HEADER bg_sys_header = {
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
		};
		GFL_BG_SetBGMode( &bg_sys_header );
	}
	
	// タッチフォントロード
  p_bg->fontHandle = GFL_FONT_Create( ARCID_FONT, NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heap_id );
//	FontProc_LoadFont( FONT_TOUCH, HEAPID_MICTEST );

	//	BG面設定
	{
		/* BG面設定データを外部へ移動	*/
		int i;
		for( i = 0; i < NELEMS(sc_bg_cnt_data); i++ ){
			
			GFL_BG_SetBGControl( sc_bg_cnt_data[i].frame, &sc_bg_cnt_data[i].header, sc_bg_cnt_data[i].mode );
			GFL_BG_ClearScreen( sc_bg_cnt_data[i].frame );
			GFL_BG_SetClearCharacter( sc_bg_cnt_data[i].frame, 32, 0, heap_id );
      GFL_BG_SetVisible( sc_bg_cnt_data[i].frame, VISIBLE_ON );
		}
	}

}

//----------------------------------------------------------------------------
/**
 *	@brief	BGシステム解放
 *
 *	@param	MICTEST_BG_WORK *p_bg	BG管理ワーク
 *
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_Exit( MICTEST_BG_WORK *p_bg )
{
	int i;
	for( i = 0; i < NELEMS(sc_bg_cnt_data); i++ ){
		GFL_BG_FreeBGControl( sc_bg_cnt_data[i].frame );
	}

  GFL_BMPWIN_Exit();
  GFL_BG_Exit();
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGシステムメイン処理
 *
 *	@param	MICTEST_BG_WORK *p_bg	BG管理ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_Main( MICTEST_BG_WORK *p_bg )
{

}

//----------------------------------------------------------------------------
/**
 *	@brief	BGリソース読み込み
 *
 *	@param	MICTEST_BG_WORK *p_bg	BG管理ワーク
 *	@param	heap_id					リソース読み込み用ヒープ
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTestBG_LoadBg( MICTEST_BG_WORK *p_bg, u32 heap_id )
{
	//	----- 下画面 -----
	GFL_ARC_UTIL_TransVramPalette( ARCID_MICTEST_GRA, NARC_mictest_back_bg_down_NCLR, PALTYPE_MAIN_BG, 0, 
						0x20, heap_id );
	GFL_ARC_UTIL_TransVramBgCharacter(	ARCID_MICTEST_GRA, NARC_mictest_back_bg_down_NCGR,
						GFL_BG_FRAME1_S, 0, 0, 0, heap_id );
	GFL_ARC_UTIL_TransVramScreen(	ARCID_MICTEST_GRA, NARC_mictest_back_bg_down_NSCR,
						GFL_BG_FRAME1_S, 0, 0, 0, heap_id );	

	//	----- 上画面 -----
	GFL_ARC_UTIL_TransVramPalette( ARCID_MICTEST_GRA, NARC_mictest_back_bg_up_NCLR, PALTYPE_SUB_BG, 0, 
						0x20, heap_id );
	GFL_ARC_UTIL_TransVramBgCharacter(	ARCID_MICTEST_GRA, NARC_mictest_back_bg_up_NCGR,
						GFL_BG_FRAME1_M, 0, 0, 0, heap_id );
	GFL_ARC_UTIL_TransVramScreen(	ARCID_MICTEST_GRA, NARC_mictest_back_bg_up_NSCR,
						GFL_BG_FRAME1_M, 0, 0, 0, heap_id );		
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGリソース解放
 *
 *	@param	MICTEST_BG_WORK *p_bg	BG管理ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTestBG_UnLoadBg( MICTEST_BG_WORK *p_bg )
{
}

//----------------------------------------------------------------------------
/**
 *	@brief	BGシステムVBlank内処理
 *
 *	@param	MICTEST_BG_WORK *p_bg BG管理ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_VBlancFunction( MICTEST_BG_WORK *p_bg )
{
	GFL_BG_VBlankFunc();
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ作成
 *
 *	@param	MICTEST_BG_WORK *p_bg	BG管理ワーク
 *	@param	heap_id					BMPWIN用ヒープID
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_CreateMsg( MICTEST_BG_WORK *p_bg, u32 heap_id )
{
	static const struct {
		u8 frame;
		u8 x;
		u8 y;
		u8 w;
		u8 h;
		u8 plt_num;
		u16 chr_ofs;
//		PRINTSYS_LSB font_col;
	} sc_bmp_setup_data[] = {
		//	TITLE
		{
			BMPWIN_TITLE_FRAME, BMPWIN_TITLE_X, BMPWIN_TITLE_Y, BMPWIN_TITLE_W, BMPWIN_TITLE_H,
			BMPWIN_TITLE_PLT, BMPWIN_TITLE_CHR_OFS,
//			PRINTSYS_LSB_Make(FBMP_COL_WHITE,FBMP_COL_BLK_SDW,FBMP_COL_NULL)
		},
		//	INFO
		{
			BMPWIN_INFO_FRAME, BMPWIN_INFO_X, BMPWIN_INFO_Y, BMPWIN_INFO_W, BMPWIN_INFO_H,
			BMPWIN_INFO_PLT, BMPWIN_INFO_CHR_OFS,
//			PRINTSYS_LSB_Make(FBMP_COL_WHITE,FBMP_COL_BLK_SDW,FBMP_COL_NULL)
		},
		//	RETBTN
		{
			BMPWIN_RETBTN_FRAME, BMPWIN_RETBTN_X, BMPWIN_RETBTN_Y, BMPWIN_RETBTN_W, BMPWIN_RETBTN_H,
			BMPWIN_RETBTN_PLT, BMPWIN_RETBTN_CHR_OFS,
//			PRINTSYS_LSB_Make(14,15,FBMP_COL_NULL)
		},
	};
	int i;
	STRBUF	*p_buf;

  p_bg->p_msg_mng = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_mictest_dat, heap_id );

	for( i = 0; i < MICTEST_BMPWIN_MAX; i++ ) {
    GFL_BMP_DATA* bmp;

    p_bg->bmpwin[i] = GFL_BMPWIN_CreateFixPos( sc_bmp_setup_data[i].frame,
				sc_bmp_setup_data[i].x, sc_bmp_setup_data[i].y, sc_bmp_setup_data[i].w, 
				sc_bmp_setup_data[i].h, sc_bmp_setup_data[i].plt_num, sc_bmp_setup_data[i].chr_ofs );

    bmp = GFL_BMPWIN_GetBmp( p_bg->bmpwin[i] );

		GFL_BMP_Clear( bmp, 0 );
		GFL_BMPWIN_ClearScreen( p_bg->bmpwin[i] );

		p_buf	= GFL_MSG_CreateString( p_bg->p_msg_mng, i );
    PRINTSYS_Print( bmp, 0, 0, p_buf, p_bg->fontHandle );
//	GF_STR_PrintColor(	&p_bg->bmpwin[i], sc_bmp_setup_data[i].font, p_buf, 0, 0, 
//					MSG_NO_PUT, sc_bmp_setup_data[i].font_col, NULL );
		GFL_STR_DeleteBuffer( p_buf );
		GFL_BMPWIN_MakeTransWindow( p_bg->bmpwin[i] );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	メッセージ破棄
 *
 *	@param	MICTEST_BG_WORK *p_bg	BG管理ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_BG_DeleteMsg( MICTEST_BG_WORK *p_bg )
{
	int i;
	for( i = 0; i < MICTEST_BMPWIN_MAX; i++ )  {
    GFL_BMP_DATA* bmp;

    bmp = GFL_BMPWIN_GetBmp( p_bg->bmpwin[i] );

//	GF_BGL_BmpWinOff( p_bg->bmpwin[i] );
		GFL_BMP_Clear( bmp, 0 );
		GFL_BMPWIN_ClearScreen( p_bg->bmpwin[i] ) ;
		GFL_BMPWIN_Delete( p_bg->bmpwin[i] );	
	}

	GFL_MSG_Delete( p_bg->p_msg_mng );

	// タッチフォント解放
  GFL_FONT_Delete( p_bg->fontHandle );
}

//=============================================================================
/**
 *	マイク管理
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	マイク処理、初期化
 *
 *	@param	MICTEST_MIC_WORK *p_mic	マイク管理ワーク
 *	@param  heap_id					バッファ用ヒープ
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_MIC_Init( MICTEST_MIC_WORK *p_mic, u32 heap_id, MICCallback MicCallback, void*p_mic_arg )
{
#ifdef MIC_FUNC_DEF
	//マイク時スリープを禁止に
	sys_SleepNG( SLEEPTYPE_MIC );

	//	バッファ作成
	{	
		u32 size		= sizeof(MIC_BUF_SIZE)*MIC_SAMPLING_LENGTH + 32;	//	後にバッファアドレスを32バイトアラインするため+32
		MIC_BUF_SIZE	*p_buf	= GFL_HEAP_AllocMemory( heap_id, size );
/*↑[GS_CONVERT_TAG]*/
		MI_CpuClear8( p_buf, size );
		p_mic->p_buf_adrs		= p_buf;

		//	マイクパラム初期化
		p_mic->param.type	= MIC_SAMPLING_TYPE;
		p_mic->param.buffer	= (void*)MATH_ROUNDUP32( (int)p_buf );
		p_mic->param.size	= sizeof(MIC_BUF_SIZE)*MIC_SAMPLING_LENGTH;
		p_mic->param.rate	= NNS_SND_STRM_TIMER_CLOCK / MIC_SAMPLING_RATE * 64;
		p_mic->param.loop_enable	= TRUE;
		p_mic->param.full_callback	= MicCallback;
		p_mic->param.full_arg		= p_mic_arg;
	}
	p_mic->pre_mic_use	= Snd_MicIsAmpOnWaitFlag();
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	マイク処理　解放
 *
 *	@param	MICTEST_MIC_WORK *p_mic マイク管理ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_MIC_Exit( MICTEST_MIC_WORK *p_mic )
{
#ifdef MIC_FUNC_DEF
	GFL_HEAP_FreeMemory( p_mic->p_buf_adrs );
/*↑[GS_CONVERT_TAG]*/

	//マイク時スリープを許可
	sys_SleepOK( SLEEPTYPE_MIC );
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	マイク処理	メイン処理
 *
 *	@param	MICTEST_MIC_WORK *p_mic		マイク管理ワーク
 *
 */
//-----------------------------------------------------------------------------
static void MicTest_MIC_Main( MICTEST_MIC_WORK *p_mic )
{	
#ifdef MIC_FUNC_DEF
	BOOL now_mic_use	= Snd_MicIsAmpOnWaitFlag();

	//もしマイク使用不可状態だったら、
	//現在のバッファをクリアする
	if( !now_mic_use || !p_mic->pre_mic_use )
	{	
		if( !p_mic->mic_use_flag )
		{	
			if( p_mic->is_start )
			{	
				//startしてたらSTOP
				MicTest_MIC_StopSampling( p_mic );
				p_mic->pre_start_sampling	= TRUE;
			}
			//さらにバッファをクリア
			{	
				//int size	= p_mic->param.size / sizeof(MIC_BUF_SIZE) - 1;
				//MIC_BUF_SIZE	*p_mic_buffer	= p_mic->param.buffer;
				//MI_CpuClear8( p_mic_buffer, size );
				OS_Printf( "buff使用不可状態　初期\n" );
			}
			p_mic->mic_use_flag	= TRUE;
		}
		//使用開始になった
		if( now_mic_use && !p_mic->pre_mic_use )
		{	
			p_mic->mic_use_flag	= FALSE;
			if( p_mic->pre_start_sampling )
			{	
				//STOPしてたらStart
				MicTest_MIC_StartSampling( p_mic );
				OS_Printf( "AutoStart\n" );
				p_mic->pre_start_sampling	= FALSE;
			}
			OS_Printf( "buff使用可能状態　初期\n" );
		}
	}

	p_mic->pre_mic_use	= now_mic_use;
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	マイク処理	サンプリングスタート
 *
 *	@param	MICTEST_MIC_WORK *p_mic マイク管理ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_MIC_StartSampling( MICTEST_MIC_WORK *p_mic )
{
#ifdef MIC_FUNC_DEF
	if( p_mic->mic_use_flag == FALSE )
	{	
		Snd_MicStartAutoSampling( &p_mic->param );
		p_mic->is_start	= TRUE;
	}
	else
	{	
		p_mic->pre_start_sampling	= TRUE;
	}
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	マイク処理	サンプリング停止
 *
 *	@param	MICTEST_MIC_WORK *p_mic マイク管理ワーク
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MicTest_MIC_StopSampling( MICTEST_MIC_WORK *p_mic )
{
#ifdef MIC_FUNC_DEF
	Snd_MicStopAutoSampling();
	p_mic->is_start	= FALSE;
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	マイクバッファを見て、平均音量を出す処理
 *
 *	@param	MICAutoParam * p_mic_param	マイクの情報
 *
 *	@return	平均音量値
 */
//-----------------------------------------------------------------------------
static int CalcAverageVolumeMicBuffer( MICAutoParam * p_mic_param )
{
	int i;
	int volume	= 0;
	int size	= p_mic_param->size / sizeof(MIC_BUF_SIZE) - 1;
	MIC_BUF_SIZE	*p_mic_buffer	= p_mic_param->buffer;
	for( i = 0; i < size; i++ ) {
		if( p_mic_buffer[i]	< 128 ) {
			volume	+= (256 - p_mic_buffer[i]);		//	※１
		}else{
			volume	+= p_mic_buffer[i];
		}
	}

	return volume / size;
}


//----------------------------------------------------------------------------
/**
 *	@brief	マイクバッファが一杯になったときに呼ばれるコールバック
 *			内部では下記を行っている
 *			１．音量ゲージの増減処理
 *			２．ポケモンのアニメ開始、停止
 *			３．音量が一定のとき音符タスクON
 *
 *	@param	MICResult result	マイク録音終了用状態
 *	@param	*p_arg				渡されたアドレス(MICTEST_MAIN_WORKを渡している)
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void ChangeObjAnmMicCallback( MICResult result, void *p_arg )
{		
#ifdef MICTEST_USE_OBJ
	if( result == MIC_RESULT_SUCCESS ) {
		MICTEST_MAIN_WORK	*p_wk	= p_arg;
		MICTEST_MIC_WORK	*p_mic	= &p_wk->mic;
		MICTEST_OBJ_WORK	*p_obj	= &p_wk->obj;
		int	now_volume;	
		int anm_seq;
		static u32 cnt = 0;

		now_volume	= CalcAverageVolumeMicBuffer( &p_mic->param );
	
		//	ゲージ減衰処理
		//	減衰値より現在の入力が多いなら減衰値に現在の値をセット
		if( p_mic->prev_volume < now_volume)  {
			anm_seq = GetMicLevelToAnmSeqNum( now_volume );
		//	OS_Printf("VOLUME=%d GAGE=%d\n", now_volume, anm_seq );
			p_mic->prev_volume	= now_volume;
			p_mic->anm_set_flg	= TRUE;
		} else {
			anm_seq = GetMicLevelToAnmSeqNum( p_mic->prev_volume );
			p_mic->prev_volume	-= GAGE_DECREASE;
		}

		//	アニメシーケンス更新
		if( anm_seq != 0 ) {
			//	ポケモンアニメ開始
			if( p_mic->anm_set_flg ) {
				GFL_CLACT_WK_SetAnmSeq( p_obj->p_act[MICTEST_OBJ_POKEMON], 1 );
/*↑[GS_CONVERT_TAG]*/
				p_mic->anm_set_flg	= FALSE;
			}

			if( IsEmptyNoteTask( p_obj ) )
			{
				MoveStartNoteCtrl( p_obj, p_wk->heap_id, OBJ_NOTE_POS_X, OBJ_NOTE_POS_Y, p_wk->tcbSys );
			}else{
				//	ハート昇るタスク開始
				if( --p_obj->note_cnt == 0 ) {
					if( MoveStartNoteCtrl( p_obj, p_wk->heap_id, OBJ_NOTE_POS_X, OBJ_NOTE_POS_Y, p_wk->tcbSys ) ) {
						OS_Printf( "NOTE_START:%d\n",cnt );
					}
					p_obj->note_cnt	= NOTE_ENTER_CNT_MAX;
				}
			}

			GFL_CLACT_WK_SetAnmSeq( p_obj->p_act[MICTEST_OBJ_GAGE_L], anm_seq );
/*↑[GS_CONVERT_TAG]*/
			GFL_CLACT_WK_SetAnmSeq( p_obj->p_act[MICTEST_OBJ_GAGE_R], anm_seq );
/*↑[GS_CONVERT_TAG]*/
		}else if( anm_seq == 0 ) {
			//	アニメ停止
			GFL_CLACT_WK_SetAnmSeq( p_obj->p_act[MICTEST_OBJ_POKEMON], 0 );
/*↑[GS_CONVERT_TAG]*/
			GFL_CLACT_WK_SetAnmSeq( p_obj->p_act[MICTEST_OBJ_GAGE_L], 0 );
/*↑[GS_CONVERT_TAG]*/
			GFL_CLACT_WK_SetAnmSeq( p_obj->p_act[MICTEST_OBJ_GAGE_R], 0 );
/*↑[GS_CONVERT_TAG]*/
		}

		cnt++;
	}
#endif
}



//----------------------------------------------------------------------------
/**
 *	@brief	マイク入力レベルから、ゲージアニメシーケンスを取得する
 *
 *	@param	u8 volume	音量
 *
 *	@return	アニメシーケンス
 */
//-----------------------------------------------------------------------------
static u8 GetMicLevelToAnmSeqNum( u8 volume )
{
	u8 anm_seq;
	if( IS_MICINPUT_LEVEL0( volume ) )  {
		anm_seq	= 0;
	}else if( IS_MICINPUT_LEVEL1( volume ) ) {
		anm_seq	= 1;
	}else if( IS_MICINPUT_LEVEL2( volume ) ) {
		anm_seq = 2;
	}else if( IS_MICINPUT_LEVEL3( volume ) ) {
		anm_seq = 3;
	}else if( IS_MICINPUT_LEVEL4( volume ) ){
		anm_seq = 4;
	}else {
		GF_ASSERT( 0 );
	}

	return anm_seq;
}


//----------------------------------------------------------------------------
/**
 *	@brief	音符タスクが空かどうか
 *
 *	@param	const MICTEST_OBJ_WORK *cp_obj OBJ管理ワーク
 *
 *	@return	TRUEなら空
 */
//-----------------------------------------------------------------------------
static BOOL IsEmptyNoteTask( const MICTEST_OBJ_WORK *cp_obj )
{
	int i;
	for( i = 0; i < OBJ_NOTE_MAX; i++ ) {
		if( cp_obj->note_task_ctrl[i].tcb_ptr != NULL ) {
			return FALSE;
		}
	}
	return TRUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	使っていないアクターを音符が上にゆらゆら動くタスクに渡す処理
 *
 *	@param	MICTEST_OBJ_WORK *p_wk	OBJ処理ワーク
 *	@param	heap_id					タスクワーク用ヒープ
 *	@param	x						初期座標
 *	@param	y						初期座標
 *	@param	is_start				開始
 *
 *	@return	TRUE なら処理開始、FALSEなら実行していない
 */
//-----------------------------------------------------------------------------
static BOOL MoveStartNoteCtrl( MICTEST_OBJ_WORK *p_wk, u32 heap_id, s16 x, s16 y, GFL_TCBSYS* tcbSys )
{
	int i;
	GFL_CLWK*	p_note			= NULL;
	NOTE_TASK_CTRL	*p_task_ctrl	= NULL;

	//	空いている箇所を検索
	for( i = 0; i < OBJ_NOTE_MAX; i++ ) {
		if( p_wk->note_task_ctrl[i].tcb_ptr == NULL ) {
			p_task_ctrl	= &p_wk->note_task_ctrl[i];
			p_note		= p_wk->p_act[ MICTEST_OBJ_NOTE+i ];
			break;
		}
	}
	
	//	見つかったら実行
	if( p_task_ctrl != NULL ) {
		fx32 speed	= NOTE_SPEED_MIN + MICTEST_RAND_MAX( NOTE_SPEED_RANGE );
		u32 angle	= NOTE_ANGLE_MIN + MICTEST_RAND_MAX( NOTE_ANGLE_RANGE );
		u32 width	= NOTE_WIDTH_MIN + MICTEST_RAND_MAX( NOTE_WIDTH_RANGE );
	//	u32 wait	= NOTE_WAIT_MIN  + MICTEST_RAND_MAX( NOTE_WAIT_RANGE  );
	//	wait	*=	NOTE_WAIT_SUM;	// できる限りバラけさせるため、ランダムの値をかけて差をだしている
		MoveStartNoteTask( tcbSys, p_note, /*heap_id*/p_task_ctrl, x, y, speed, angle, width, 1 );
		return TRUE;
	}

	return FALSE;
}


//----------------------------------------------------------------------------
/**
 *	@brief	音符が上にゆらゆら動くタスク、開始
 *
 *	@param	GFL_CLWK*	p_act	音符のアクター
 *	@param	NOTE_MOVE_WORK *p_task_wk	タスクワーク
 *	//			@param	heap_id			タスクワーク用ヒープ
 *	@param	x				初期座標
 *	@param	y				初期座標
 *	@param	speed			上昇スピード
 *	@param	angle			上昇アングル
 *	@param	width			ゆれ幅
 *	@param	wait			開始待機シンク
 *	@param	p_is_end		終了フラグ（NULLだと無視）
 *
 *	@return	tcbのポインタ
 */
//-----------------------------------------------------------------------------
static void MoveStartNoteTask( GFL_TCBSYS* tcbSys, GFL_CLWK*	p_act, NOTE_TASK_CTRL *p_task_wk, /*u32 heap_id,*/ s16 x, s16 y, fx32 speed, u32 angle, u32 width, u32 wait )
{
	NOTE_MOVE_WORK	*p_wk;
  GFL_CLACTPOS pos;

	/* 割り込み中にアロケートするとアサートのためアロケートできないので、実体をもらってくる */
//	p_tcb	= PMDS_taskAdd( MoveNoteTask, sizeof(NOTE_MOVE_WORK), 0, heap_id );	//	(バスを占有するから　マイクのバッファ終了コールバックは割り込みらしい)
	p_task_wk->tcb_ptr	= GFL_TCB_AddTask( tcbSys, MoveNoteTask, p_task_wk, 0 );

	p_wk	= GFL_TCB_GetWork( p_task_wk->tcb_ptr );
	p_wk->p_act	= p_act;
	p_wk->wait	= wait;
	p_wk->pos.x	= FX32_CONST(x);
	p_wk->pos.y	= FX32_CONST(y);
	p_wk->pos.z = 0;
	p_wk->speed.x	=  FX_Mul( speed, GFL_CALC_Sin360(90+angle));
	p_wk->speed.y	= -FX_Mul( speed, GFL_CALC_Sin360(angle) );
	p_wk->speed.z	= 0;
	p_wk->shake_pt		= 0;
	p_wk->shake_speed	= FX32_CONST(6.0f);
	p_wk->shake_width	= width;

  pos.x = x;
  pos.y = y;
	GFL_CLACT_WK_SetWldPos( p_wk->p_act, &pos );
}

//----------------------------------------------------------------------------
/**
 *	@brief	音符が上にゆらゆら動くタスク
 *
 *	@param	GFL_TCB* p_tcb	タスクポインタ
 *	@param	*p_wk_adrs		渡されたアドレス
 *
 *	@return	none
 */
//-----------------------------------------------------------------------------
static void MoveNoteTask( GFL_TCB* p_tcb, void *p_wk_adrs )
{
#ifdef MICTEST_USE_OBJ
	NOTE_TASK_CTRL  *p_ctrl	= p_wk_adrs;
	NOTE_MOVE_WORK	*p_wk	= &p_ctrl->note_wk;

	if( p_wk->wait ) {
		p_wk->wait--;
		if( p_wk->wait == 0 ) {
			GFL_CLACT_UNIT_SetDrawEnableCap( p_wk->p_act, CATS_ENABLE_TRUE );
		}
	}else{	
		int shake;
		s16 x, y;

		VEC_Add( &p_wk->pos, &p_wk->speed, &p_wk->pos );
		x	= p_wk->pos.x >> FX32_SHIFT;
		y	= p_wk->pos.y >> FX32_SHIFT;
		p_wk->shake_pt	+= p_wk->shake_speed;
		while( p_wk->shake_pt > FX32_CONST(360) ) {
			p_wk->shake_pt -= FX32_CONST(360);
		}
		shake	= p_wk->shake_pt >> FX32_SHIFT;
		x		+= (p_wk->shake_width * GFL_CALC_Sin360( shake )) >> FX32_SHIFT;

		GFL_CLACT_WK_SetWldPosCap( p_wk->p_act, x, y );

		if( y < - 16 ) {
			GFL_CLACT_UNIT_SetDrawEnableCap( p_wk->p_act, CATS_ENABLE_FALSE );
			GFL_TCB_DeleteTask( p_tcb );
			p_ctrl->tcb_ptr = NULL;
		}
	}
#endif
}

//----------------------------------------------------------------------------
/**
 *	@brief	音符タスク、全て削除
 *
 *	@param	MICTEST_OBJ_WORK *p_obj OBJ管理ワーク
 *
 *	@return	note
 */	
//-----------------------------------------------------------------------------
static void CleanNoteTask( MICTEST_OBJ_WORK *p_obj )
{
	int i;
	for( i = 0; i < OBJ_NOTE_MAX; i++ ) {
		if( p_obj->note_task_ctrl[i].tcb_ptr != NULL ) {
      GFL_TCB_DeleteTask( p_obj->note_task_ctrl[i].tcb_ptr );
			p_obj->note_task_ctrl[i].tcb_ptr	= NULL;
		}
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	キーヒット状況確認
 *
 *	@param	none
 *
 *	@return	ヒットしたテーブルの番号　ヒットがないと GFL_UI_TP_HIT_NONE
 */
//-----------------------------------------------------------------------------
static int GetHitTrg()
{
	static const GFL_UI_TP_HITTBL sc_hit_tbl[]	= {
/*↑[GS_CONVERT_TAG]*/
		{ OBJ_BUTTON_POS_Y - 16, OBJ_BUTTON_POS_Y + 16,
		  OBJ_BUTTON_POS_X - 32, OBJ_BUTTON_POS_X + 32},
		{ GFL_UI_TP_HIT_END,0,0,0 },		// 終了データ	
/*↑[GS_CONVERT_TAG]*/
	};

	return GFL_UI_TP_HitTrg( sc_hit_tbl );
/*↑[GS_CONVERT_TAG]*/
}

//----------------------------------------------------------------------------
/**
 *	@brief	プロセス終了キー
 *
 *	@param	none
 *
 *	@return TRUEでプロセス終了　FALSEでまだ
 */
//-----------------------------------------------------------------------------
static BOOL IsProcEndTrg()
{
	return  GetHitTrg() == 0 || GFL_UI_KEY_GetTrg() & PAD_BUTTON_CANCEL;
/*↑[GS_CONVERT_TAG]*/
}
