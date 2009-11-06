//============================================================================================
/**
	* @file	pms_input.c
	* @bfief	簡易会話入力画面
	* @author	taya
	* @date	06.01.20
	*/
//============================================================================================
#include <gflib.h>

#include "system\main.h"
#include "savedata/save_control.h"

#include "system\pms_data.h"
#include "pmsi_sound_def.h"
#include "pms_input_prv.h"
#include "pmsi_initial_data.h"

#include "test/ariizumi/ari_debug.h"

FS_EXTERN_OVERLAY(ui_common);

//------------------------------------------------------
/**
	* 定数定義
	*/
//------------------------------------------------------
enum {
	HEAPSIZE_SYS = 0x10000,
	HEAPSIZE_VIEW = 0x38000,
};

enum {
	MAINSEQ_FADEIN = 0,
	MAINSEQ_SINGLE_MODE,
	MAINSEQ_DOUBLE_MODE,
	MAINSEQ_SENTENCE_MODE,
};


typedef enum {
	MENU_RESULT_NONE = -1,
	MENU_RESULT_CANCEL = -2,
	MENU_RESULT_MOVE = -3,
	MENU_RESULT_DISABLE_KEY = -4,

	MENU_RESULT_POS_YES = 0,
	MENU_RESULT_POS_NO,
	MENU_RESULT_POS_MAX = MENU_RESULT_POS_NO,

}MENU_RESULT;

enum {
	WORDWIN_RESULT_NONE,
	WORDWIN_RESULT_CURSOR_MOVE,
	WORDWIN_RESULT_SCROLL,
	WORDWIN_RESULT_SCROLL_AND_CURSOR_MOVE,
	WORDWIN_RESULT_INVALID,
	WORDWIN_RESULT_SELECT,
};

enum {
	WORDWIN_CURSOR_X_MAX = 1,
	WORDWIN_CURSOR_Y_MAX = 4,
	WORDWIN_DISP_MAX = 10,

	WORDWIN_PAGE_LINES = WORDWIN_CURSOR_Y_MAX+1,
};

enum TOUCH_BUTTON {
	TOUCH_BUTTON_GROUP,
	TOUCH_BUTTON_INITIAL,
	TOUCH_BUTTON_UP,
	TOUCH_BUTTON_DOWN,

	TOUCH_BUTTON_NULL,
};

enum{
	TPED_DCBTN_PX = 24*8,
	TPED_DCBTN_PY0 = 14*8,
	TPED_DCBTN_PY1 = 17*8,
	TPED_DCBTN_SX = 8*8,
	TPED_DCBTN_SY = 2*8,

	TPED_WORD1_PX = 10*8,
	TPED_WORD_PY = 2*8,
	TPED_WORD_SX = 12*8,
	TPED_WORD_SY = 2*8,
	TPED_WORD2_PX0 = 3*8,
	TPED_WORD2_PX1 = 17*8,

	TPED_SBTN_PX0 = 8,
	TPED_SBTN_PX1 = 27*8,
	TPED_SBTN_PY = 6*8+4,
	TPED_SBTN_SX = 4*8,
	TPED_SBTN_SY = 3*8,

	TPCA_RET_PX = 24*8+4,
	TPCA_RET_PY = 20*8+4,
	TPCA_RET_SX = 7*8,
	TPCA_RET_SY = 22,

	TPCA_GMODE_PX = 2*8,
	TPCA_IMODE_PX = 22*8,
	TPCA_MODE_PY = 0,
	TPCA_MODE_SX = 8*8,
	TPCA_MODE_SY = 5*8+2,

	TPCA_GMA_SX = 9*8,
	TPCA_GMA_SY = 2*8,
	TPCA_GMA_PX = 1*8,
	TPCA_GMA_PY = 9*8,
	TPCA_GMA_OX = TPCA_GMA_SX+1*8,
	TPCA_GMA_OY = TPCA_GMA_SY+1*8,

	TPCA_IMA_SX = 13,
	TPCA_IMA_SY = 16,
	TPCA_IMA_PX = 2*8-GROUPMODE_BG_XOFS,
	TPCA_IMA_PY = 10*8-CATEGORY_BG_ENABLE_YOFS,

	TPWD_SCR_PX = 28*8+4,
	TPWD_SCR_SX = 24,
	TPWD_SCR_SY = 7*8,
	TPWD_SCR_PY0 = 6*8,
	TPWD_SCR_PY1 = TPWD_SCR_PY0+TPWD_SCR_SY,

	TPWD_WORD_PX = 4*8,
	TPWD_WORD_PY = 6*8,
	TPWD_WORD_SX = 9*8,
	TPWD_WORD_SY = 2*8,
	TPWD_WORD_OX = TPWD_WORD_SX+32,
	TPWD_WORD_OY = TPWD_WORD_SY+8,
};

enum {
	SEQ_EDW_KEYWAIT,
	SEQ_EDW_BUTTON_KEYWAIT,
	SEQ_EDW_TO_SUBPROC_OK,
	SEQ_EDW_TO_SUBPROC_CANCEL,
	SEQ_EDW_RETURN_SUBPROC,
	SEQ_EDW_TO_SELECT_CATEGORY,
	SEQ_EDW_TO_COMMAND_BUTTON,
};

enum {
	SEQ_EDS_INIT,
	SEQ_EDS_KEYWAIT,
	SEQ_EDS_BUTTON_KEYWAIT,
	SEQ_EDS_WAIT_EDITAREA_UPDATE,
	SEQ_EDS_TO_SUBPROC_OK,
	SEQ_EDS_TO_SUBPROC_CANCEL,
	SEQ_EDS_RETURN_SUBPROC,
	SEQ_EDS_TO_COMMAND_BUTTON,
	SEQ_EDS_TO_SELECT_CATEGORY,
};

enum {
	SEQ_CA_KEYWAIT,
	SEQ_CA_NEXTPROC,
	SEQ_CA_WAIT_MODE_CHANGE,
};

enum {
	SEQ_WORD_KEYWAIT,
	SEQ_WORD_SCROLL_WAIT,
	SEQ_WORD_SCROLL_WAIT_AND_CURSOR_MOVE,
	SEQ_WORD_CHANGE_NEXTPROC,
};

//------------------------------------------------------
/**
	* 関数型定義
	*/
//------------------------------------------------------
typedef  void (*SubProc)(PMS_INPUT_WORK*, int*);
typedef  GFL_PROC_RESULT (*MainProc)(PMS_INPUT_WORK*, int* );
typedef	void (*KTSTChangeProc)(PMS_INPUT_WORK*,int* seq);

//------------------------------------------------------
/**
	* メニュー操作ワーク
	*/
//------------------------------------------------------
typedef struct {
	u8   pos;
	u8   max;
}MENU_WORK;

typedef struct {
	u16   line;
	u16   line_max;
	u16   word_max;
	u8    cursor_x;
	u8    cursor_y;
	int   scroll_lines;
	u16	  back_f;
	u16	  touch_pos;
}WORDWIN_WORK;

typedef struct {
	s16  sentence_type;
	s8   sentence_id;
	s8   sentence_id_max;
}SENTENCE_WORK;


//------------------------------------------------------
/**
	* ワーク領域定義
	*/
//------------------------------------------------------
struct _PMS_INPUT_WORK{

	PMSI_PARAM* input_param;
	u32         input_mode;

	PMS_DATA    edit_pms;
	PMS_WORD    edit_word[ PMS_INPUT_WORD_MAX ];

	PMS_INPUT_VIEW*   vwk;
	PMS_INPUT_DATA*   dwk;

	int        main_seq;
	MainProc   main_proc;
	MainProc   next_proc;

	int        sub_seq;
	SubProc    sub_proc;

	u16        key_trg;
	u16        key_cont;
	u16        key_repeat;
	int        touch_button;
	int        hold_button;
	GFL_BUTTON_MAN*   bmn;
	BOOL       button_up_hold_flag;
	BOOL       button_down_hold_flag;

	u16        cmd_button_pos;
	u16        edit_pos;
	u16        category_pos;
	u16        category_pos_prv;

	WORDWIN_WORK   word_win;

	SENTENCE_WORK  sentence_wk;

	u8         sentence_edit_pos_max;
	u8         category_mode;


	MENU_WORK  menu;

	int			key_mode;
	KTSTChangeProc	cb_ktchg_func;
	
	void		*tcbWork;
	GFL_TCBSYS	*tcbSys;
};


//==============================================================
// Prototype
//==============================================================
static void BmnCallBack( u32 buttonID, u32 event, void* wk_ptr );
static PMS_INPUT_WORK* ConstructWork( GFL_PROC* proc , void* pwk );
static void setup_sentence_work( SENTENCE_WORK* s_wk, PMS_DATA* pms );
static void sentence_increment( SENTENCE_WORK* s_wk, PMS_DATA* pms );
static void sentence_decrement( SENTENCE_WORK* s_wk, PMS_DATA* pms );
static void DestructWork( PMS_INPUT_WORK* wk, GFL_PROC* proc );
static void ChangeMainProc( PMS_INPUT_WORK* wk, MainProc main_proc );
static void ChangeMainProc_ToCommandButtonArea( PMS_INPUT_WORK* wk );
static GFL_PROC_RESULT MainProc_EditArea( PMS_INPUT_WORK* wk, int* seq );
static GFL_PROC_RESULT mp_input_single( PMS_INPUT_WORK* wk, int* seq );
static GFL_PROC_RESULT mp_input_double( PMS_INPUT_WORK* wk, int* seq );
static GFL_PROC_RESULT mp_input_sentence( PMS_INPUT_WORK* wk, int* seq );
static GFL_PROC_RESULT MainProc_CommandButton( PMS_INPUT_WORK* wk, int* seq );
static GFL_PROC_RESULT MainProc_Category( PMS_INPUT_WORK* wk, int* seq );
static void category_input_key(PMS_INPUT_WORK* wk,int* seq);
static void category_input_touch(PMS_INPUT_WORK* wk,int* seq);
static void category_input(PMS_INPUT_WORK* wk,int* seq);
static void CB_EditArea_KTChange(PMS_INPUT_WORK* wk,int* seq);
static void CB_Category_KTChange(PMS_INPUT_WORK* wk,int* seq);
static void CB_WordWin_KTChange(PMS_INPUT_WORK* wk,int* seq);
static BOOL check_category_enable( PMS_INPUT_WORK* wk );
static BOOL check_category_cursor_move( PMS_INPUT_WORK* wk );
static BOOL keycheck_category_group_mode( PMS_INPUT_WORK* wk );
static BOOL keycheck_category_initial_mode( PMS_INPUT_WORK* wk );
static void setup_wordwin_params( WORDWIN_WORK* word_win, PMS_INPUT_WORK* wk );
static u32 get_wordwin_cursorpos( const WORDWIN_WORK* wordwin );
static u32 get_wordwin_pos( const WORDWIN_WORK* wordwin );
static int get_wordwin_scroll_vector( const WORDWIN_WORK* wordwin );
static int get_wordwin_linepos( const WORDWIN_WORK* wordwin );
static int get_wordwin_linemax( const WORDWIN_WORK* wordwin );
static GFL_PROC_RESULT MainProc_WordWin( PMS_INPUT_WORK* wk, int* seq );
static void word_input_key(PMS_INPUT_WORK* wk,int* seq);
static void word_input_touch(PMS_INPUT_WORK* wk,int* seq);
static void word_input(PMS_INPUT_WORK* wk,int* seq);
static int check_wordwin_key( WORDWIN_WORK* wordwin, u16 key );
static int check_wordwin_scroll_up( WORDWIN_WORK* wordwin );
static int check_wordwin_scroll_down( WORDWIN_WORK* wordwin );
static BOOL set_select_word( PMS_INPUT_WORK* wk );
static GFL_PROC_RESULT MainProc_Quit( PMS_INPUT_WORK* wk, int* seq );
static void SetSubProc( PMS_INPUT_WORK* wk, SubProc sub_proc );
static void QuitSubProc( PMS_INPUT_WORK* wk );
static void SubProc_FadeIn( PMS_INPUT_WORK* wk, int* seq );
static void SubProc_CommandOK( PMS_INPUT_WORK* wk, int* seq );
static void SubProc_CommandCancel( PMS_INPUT_WORK* wk, int* seq );
static BOOL CheckModified( PMS_INPUT_WORK* wk );
static BOOL check_input_complete( PMS_INPUT_WORK* wk );
static void InitMenuState( MENU_WORK* menu, int max, int pos );
static MENU_RESULT  CtrlMenuState( MENU_WORK* menu , u16 key );
static u32 get_menu_cursor_pos( const MENU_WORK* menu );
static void SubProc_ChangeCategoryMode( PMS_INPUT_WORK* wk, int* seq );

//@todo 仮用の関数	あとで消してください
static PMS_WORD PMSI_WordToDeco( PMS_WORD word );


//==============================================================
// GFL_PROC-DATA
//==============================================================
const GFL_PROC_DATA ProcData_PMSInput = {
	PMSInput_Init,
	PMSInput_Main,
	PMSInput_Quit,
};


//------------------------------------------------------------------
/**
	* GFL_PROC初期化
	*
	* @param   proc		
	* @param   seq		
	*
	* @retval  GFL_PROC_RESULT		
	*/
//------------------------------------------------------------------
GFL_PROC_RESULT PMSInput_Init( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
	PMS_INPUT_WORK* wk;

	switch( *seq ){
	case 0:
	
    //オーバーレイ読み込み
	  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));

#if PMS_USE_SND
		Snd_DataSetByScene( SND_SCENE_SUB_PMS, 0, 0 );	// サウンドデータロード(PMS)(BGM引継ぎ)
#endif
		GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PMS_INPUT_SYSTEM, HEAPSIZE_SYS );
		GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PMS_INPUT_VIEW, HEAPSIZE_VIEW );
		wk = ConstructWork( proc , pwk );
		PMSIView_SetCommand( wk->vwk, VCMD_INIT );
		(*seq)++;
		break;

	case 1:
		wk = mywk;
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			if( wk->input_mode == PMSI_MODE_SENTENCE )
			{
				wk->sentence_edit_pos_max = PMSIView_GetSentenceEditPosMax( wk->vwk );
			}
			else
			{
				wk->sentence_edit_pos_max = 0;
			}
			return GFL_PROC_RES_FINISH;
		}
		break;
	}

	GFL_TCB_Main(wk->tcbSys);	//初期化で使ってるからココでまわす
	return GFL_PROC_RES_CONTINUE;
}



//------------------------------------------------------------------
/**
	* GFL_PROC メイン
	*
	* @param   proc		
	* @param   seq		
	*
	* @retval  GFL_PROC_RESULT		
	*/
//------------------------------------------------------------------
GFL_PROC_RESULT PMSInput_Main( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
	GFL_PROC_RESULT ret = GFL_PROC_RES_CONTINUE;
	PMS_INPUT_WORK* wk = mywk;

	wk->key_trg = GFL_UI_KEY_GetTrg();
	wk->key_cont = GFL_UI_KEY_GetCont();
	wk->key_repeat = GFL_UI_KEY_GetRepeat();

	GFL_BMN_Main( wk->bmn );

	if( wk->sub_proc != NULL )
	{
		wk->sub_proc( wk, &(wk->sub_seq) );
		ret = GFL_PROC_RES_CONTINUE;
	}
	else
	{
		ret = wk->main_proc( wk, &(wk->main_seq) );
	}

	GFL_TCB_Main(wk->tcbSys);
	return ret;
}

//------------------------------------------------------------------
/**
	* ボタン管理マネージャコールバック
	*
	* @param   buttonID	
	* @param   event		
	* @param   wk_ptr		
	*
	*/
//------------------------------------------------------------------
static void BmnCallBack( u32 buttonID, u32 event, void* wk_ptr )
{
	PMS_INPUT_WORK* wk = wk_ptr;

	switch( event ){
	case GFL_BMN_EVENT_TOUCH:
		wk->touch_button = buttonID;
		switch(buttonID){
		case TOUCH_BUTTON_UP:
			PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_UP_HOLD );
			break;
		case TOUCH_BUTTON_DOWN:
			PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_DOWN_HOLD );
			break;
		}
		break;

	case GFL_BMN_EVENT_RELEASE:
	case GFL_BMN_EVENT_SLIDEOUT:
		switch(buttonID){
		case TOUCH_BUTTON_UP:
			PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_UP_RELEASE );
			break;
		case TOUCH_BUTTON_DOWN:
			PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_DOWN_RELEASE );
			break;
		}
		wk->touch_button = TOUCH_BUTTON_NULL;
		break;

	case GFL_BMN_EVENT_HOLD:
		if(	(buttonID == TOUCH_BUTTON_UP)
		||	(buttonID == TOUCH_BUTTON_DOWN)
		){
			wk->touch_button = buttonID;
		}
		break;

	default:
		wk->touch_button = TOUCH_BUTTON_NULL;
		break;
	}
}

//------------------------------------------------------------------
/**
	* GFL_PROC 終了
	*
	* @param   proc		
	* @param   seq		
	*
	* @retval  GFL_PROC_RESULT		
	*/
//------------------------------------------------------------------
GFL_PROC_RESULT PMSInput_Quit( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
	{	
		int i;

		PMS_INPUT_WORK* wk	= mywk;

		//↓@todoGMM文字列をデコメに変換する処理
		for( i = 0; i < PMS_INPUT_WORD_MAX; i++ )
		{	
			wk->edit_word[i]	= PMSI_WordToDeco( wk->edit_word[i] );
		}
		for( i = 0; i < PMS_WORD_MAX; i++ )
		{	
			wk->edit_pms.word[i]	= PMSI_WordToDeco( wk->edit_pms.word[i] );
		}
		PMSI_PARAM_WriteBackData( wk->input_param, wk->edit_word, &wk->edit_pms );
		//↑ここまで
	}

	

	DestructWork( mywk, proc );

	GFL_HEAP_DeleteHeap( HEAPID_PMS_INPUT_SYSTEM );
	GFL_HEAP_DeleteHeap( HEAPID_PMS_INPUT_VIEW );

	//オーバーレイ破棄
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

	return GFL_PROC_RES_FINISH;
}
//------------------------------------------------------------------
/**
	* メインワーク構築
	*
	* @param   wk		
	* @param   proc		
	*
	*/
//------------------------------------------------------------------
static PMS_INPUT_WORK* ConstructWork( GFL_PROC* proc , void* pwk )
{
	// enum TOUCH_BUTTON と一致させる
	static const GFL_UI_TP_HITTBL hit_tbl[] = {
		{ BUTTON_GROUP_TOUCH_TOP, BUTTON_GROUP_TOUCH_BOTTOM, BUTTON_GROUP_TOUCH_LEFT, BUTTON_GROUP_TOUCH_RIGHT },
		{ BUTTON_INITIAL_TOUCH_TOP, BUTTON_INITIAL_TOUCH_BOTTOM, BUTTON_INITIAL_TOUCH_LEFT, BUTTON_INITIAL_TOUCH_RIGHT },
		{ BUTTON_UP_TOUCH_TOP, BUTTON_UP_TOUCH_BOTTOM, BUTTON_UP_TOUCH_LEFT, BUTTON_UP_TOUCH_RIGHT },
		{ BUTTON_DOWN_TOUCH_TOP, BUTTON_DOWN_TOUCH_BOTTOM, BUTTON_DOWN_TOUCH_LEFT, BUTTON_DOWN_TOUCH_RIGHT },
		{ GFL_UI_TP_HIT_END, 0, 0, 0 },
	};

	PMS_INPUT_WORK* wk;

	wk = GFL_PROC_AllocWork( proc, sizeof(PMS_INPUT_WORK), HEAPID_PMS_INPUT_SYSTEM );
	wk->input_param = pwk;
	wk->input_mode = PMSI_PARAM_GetInputMode( wk->input_param );

	//キーorタッチステータス取得
	wk->key_mode = PMSI_PARAM_GetKTStatus(wk->input_param);

	PMSI_PARAM_GetInitializeData( wk->input_param, wk->edit_word, &wk->edit_pms );

	if( wk->input_mode == PMSI_MODE_SENTENCE )
	{
		setup_sentence_work( &wk->sentence_wk, &wk->edit_pms );
	}


	wk->tcbWork = GFL_HEAP_AllocMemory( HEAPID_PMS_INPUT_SYSTEM , GFL_TCB_CalcSystemWorkSize( 5 ) );
	wk->tcbSys = GFL_TCB_Init( 5 , wk->tcbWork );

	wk->dwk = PMSI_DATA_Create( HEAPID_PMS_INPUT_SYSTEM, wk->input_param );
	wk->vwk = PMSIView_Create(wk, wk->dwk);
	wk->bmn = GFL_BMN_Create( hit_tbl, BmnCallBack, wk, HEAPID_PMS_INPUT_SYSTEM );
	wk->button_up_hold_flag = FALSE;
	wk->button_down_hold_flag = FALSE;

	wk->category_mode = CATEGORY_MODE_GROUP;
	wk->sub_proc = NULL;
	wk->sub_seq = 0;
	wk->edit_pos = 0;

	wk->touch_button = TOUCH_BUTTON_NULL;

	ChangeMainProc(wk, MainProc_EditArea);
	SetSubProc( wk, SubProc_FadeIn );

	return wk;
}

static void setup_sentence_work( SENTENCE_WORK* s_wk, PMS_DATA* pms )
{
	s_wk->sentence_type = PMSDAT_GetSentenceType(pms);
	s_wk->sentence_id_max = PMSDAT_GetSentenceIdMax(s_wk->sentence_type);
	s_wk->sentence_id = PMSDAT_GetSentenceID(pms);
}

static void sentence_increment( SENTENCE_WORK* s_wk, PMS_DATA* pms )
{
	s_wk->sentence_id++;
	if( s_wk->sentence_id >= s_wk->sentence_id_max )
	{
		s_wk->sentence_id = 0;
		s_wk->sentence_type++;
		if( s_wk->sentence_type >= PMS_TYPE_MAX )
		{
			s_wk->sentence_type = 0;
		}
		s_wk->sentence_id_max = PMSDAT_GetSentenceIdMax( s_wk->sentence_type );
	}

	PMSDAT_SetSentence( pms, s_wk->sentence_type, s_wk->sentence_id );
}

static void sentence_decrement( SENTENCE_WORK* s_wk, PMS_DATA* pms )
{
	s_wk->sentence_id--;
	if( s_wk->sentence_id < 0 )
	{
		s_wk->sentence_type--;
		if( s_wk->sentence_type < 0 )
		{
			s_wk->sentence_type = (PMS_TYPE_MAX - 1);
		}
		s_wk->sentence_id_max = PMSDAT_GetSentenceIdMax( s_wk->sentence_type );
		s_wk->sentence_id = (s_wk->sentence_id_max - 1);
	}

	PMSDAT_SetSentence( pms, s_wk->sentence_type, s_wk->sentence_id );
}



//------------------------------------------------------------------
/**
	* メインワーク破棄
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
static void DestructWork( PMS_INPUT_WORK* wk, GFL_PROC* proc )
{
	GFL_TCB_Exit( wk->tcbSys );
	GFL_HEAP_FreeMemory( wk->tcbWork );
	
	//キーorタッチステータス反映
	PMSI_PARAM_SetKTStatus(wk->input_param,wk->key_mode);
	
	GFL_BMN_Delete( wk->bmn );
	PMSIView_Delete( wk->vwk );
	PMSI_DATA_Delete( wk->dwk );
	GFL_PROC_FreeWork( proc );
}



static void ChangeMainProc( PMS_INPUT_WORK* wk, MainProc main_proc )
{
	wk->main_proc = main_proc;
	wk->main_seq = 0;

	if(main_proc == MainProc_EditArea){
		wk->cb_ktchg_func = CB_EditArea_KTChange;
	}else if(main_proc == MainProc_Category){
		wk->cb_ktchg_func = CB_Category_KTChange;
	}else if(main_proc == MainProc_WordWin){
		wk->cb_ktchg_func = CB_WordWin_KTChange;
	}else{
		wk->cb_ktchg_func = NULL;
	}
}
static void ChangeMainProc_ToCommandButtonArea( PMS_INPUT_WORK* wk )
{
	ChangeMainProc( wk, MainProc_CommandButton );
}

/*
	*	@brief	キー/タッチの切替
	*/
static int KeyStatusChange(PMS_INPUT_WORK* wk,int* seq)
{
	if(wk->key_mode == GFL_APP_KTST_TOUCH){
		if(GFL_UI_TP_GetCont() != 0){	//
			return 0;
		}
		if(GFL_UI_KEY_GetCont() != 0){
			if(wk->cb_ktchg_func != NULL){
				(wk->cb_ktchg_func)(wk,seq);
			}
			wk->key_mode = GFL_APP_KTST_KEY;
			return 1;
		}
	}else{
		if(GFL_UI_KEY_GetCont() != 0){
			return 0;
		}
		if(GFL_UI_TP_GetCont() != 0){
			if(wk->cb_ktchg_func != NULL){
				(wk->cb_ktchg_func)(wk,seq);
			}
			wk->key_mode = GFL_APP_KTST_TOUCH;
			return 0;
		}
	}
	return 0;

}



//----------------------------------------------------------------------------------------------
/**
	* メインプロセス：入力欄での動作
	*
	* @param   wk		
	* @param   seq		
	*
	* @retval  GFL_PROC_RESULT		
	*/
//----------------------------------------------------------------------------------------------
static GFL_PROC_RESULT MainProc_EditArea( PMS_INPUT_WORK* wk, int* seq )
{
	static const MainProc proc_tbl[] = {
		mp_input_single,
		mp_input_double,
		mp_input_sentence,
	};

	return proc_tbl[ wk->input_mode ]( wk, seq );
}

/**
	*	@brief	EditArea キー/タッチ切替
	*/
static void CB_EditArea_KTChange(PMS_INPUT_WORK* wk,int* seq)
{
	PMSIView_SetCommand( wk->vwk, VCMD_KTCHANGE_EDITAREA);
}

/**
	*	@brief	Category キー/タッチ切替
	*/
static void CB_Category_KTChange(PMS_INPUT_WORK* wk,int* seq)
{	
	PMSIView_SetCommand( wk->vwk, VCMD_KTCHANGE_CATEGORY);
}

/**
	*	@brief	WordWin キー/タッチ切替
	*/
static void CB_WordWin_KTChange(PMS_INPUT_WORK* wk,int* seq)
{	
	PMSIView_SetCommand( wk->vwk, VCMD_KTCHANGE_WORDWIN);
}

//------------------------------------------------------------------
/**
	* 単語１つ入力モード
	*
	* @param   wk		
	* @param   seq		
	*
	* @retval  GFL_PROC_RESULT		
	*/
//------------------------------------------------------------------
static GFL_PROC_RESULT mp_input_single_key(PMS_INPUT_WORK* wk,int *seq )
{
	switch( *seq ){
	case SEQ_EDW_KEYWAIT:
		if(! PMSIView_WaitCommandAll( wk->vwk ) ){
			break;
		}
		if( wk->key_trg & (PAD_KEY_DOWN | PAD_BUTTON_START))
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND

			wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_BUTTON );
//			(*seq) = SEQ_EDW_TO_COMMAND_BUTTON;
			(*seq) = SEQ_EDW_BUTTON_KEYWAIT;
			break;
		}
		if( wk->key_trg & PAD_BUTTON_B )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_CANCEL);
#endif //PMS_USE_SND

			SetSubProc( wk, SubProc_CommandCancel );
			break;
		}
		if( wk->key_trg & PAD_BUTTON_A )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND

			wk->category_pos = 0;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_CATEGORY );
			(*seq) = SEQ_EDW_TO_SELECT_CATEGORY;
			break;
		}
		break;
	case SEQ_EDW_BUTTON_KEYWAIT:
		if(! PMSIView_WaitCommandAll( wk->vwk ) ){
			break;
		}

		if( wk->key_trg & PAD_KEY_UP )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND

			if(wk->cmd_button_pos){
        // 「やめる」から「けってい」へ
				wk->cmd_button_pos ^= 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			}else{
        // 「けってい」からエディットエリアへ
				PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
				(*seq) = SEQ_EDW_KEYWAIT;
			}
			break;
		}else if( wk->key_trg & (PAD_KEY_DOWN) ){
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
			
      if(wk->cmd_button_pos){
        // 「やめる」からエディットエリアへ
				PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
				(*seq) = SEQ_EDW_KEYWAIT;
			}else{
        // 「けってい」から「やめる」へ
				wk->cmd_button_pos ^= 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			}
			break;
		}

		if( wk->key_trg & PAD_BUTTON_START ){
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
			wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			break;
		}
		if( wk->key_trg & PAD_BUTTON_A ){
#if PMS_USE_SND
			Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND

			if( wk->cmd_button_pos == BUTTON_POS_DECIDE ){
				(*seq) = SEQ_EDW_TO_SUBPROC_OK;
				break;
			}else{
				(*seq) = SEQ_EDW_TO_SUBPROC_CANCEL;
					break;
				}
			}

			if( wk->key_trg & PAD_BUTTON_B ){
#if PMS_USE_SND
				Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND
				(*seq) = SEQ_EDW_TO_SUBPROC_CANCEL;
				break;
			}
		break;
	case SEQ_EDW_TO_SUBPROC_OK:
		(*seq) = SEQ_EDW_RETURN_SUBPROC;
		SetSubProc( wk, SubProc_CommandOK );
		break;

	case SEQ_EDW_TO_SUBPROC_CANCEL:
		(*seq) = SEQ_EDW_RETURN_SUBPROC;
		SetSubProc( wk, SubProc_CommandCancel );
		break;

	case SEQ_EDW_RETURN_SUBPROC:
		(*seq) = SEQ_EDW_BUTTON_KEYWAIT;
		break;
	case SEQ_EDW_TO_COMMAND_BUTTON:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			ChangeMainProc( wk, MainProc_CommandButton );
		}
		break;

	case SEQ_EDW_TO_SELECT_CATEGORY:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			ChangeMainProc( wk, MainProc_Category );
		}
		break;
	}

	return GFL_PROC_RES_CONTINUE;

}


static int edit_single_touch(PMS_INPUT_WORK* wk)
{
	int ret;
	
	static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
//		{0,191,0,255}, ty,by,lx,rx
		{TPED_DCBTN_PY0,TPED_DCBTN_PY0+TPED_DCBTN_SY,TPED_DCBTN_PX,TPED_DCBTN_PX+TPED_DCBTN_SX},
		{TPED_DCBTN_PY1,TPED_DCBTN_PY1+TPED_DCBTN_SY,TPED_DCBTN_PX,TPED_DCBTN_PX+TPED_DCBTN_SX},
		{TPED_WORD_PY,TPED_WORD_PY+TPED_WORD_SY,TPED_WORD1_PX,TPED_WORD1_PX+TPED_WORD_SX},
		{GFL_UI_TP_HIT_END,0,0,0}
	};
//	pat = 0x007C;	//0000 0000 0111 1100	無効パレットbit(onになっている色Noは反応させない)
//	if(GF_BGL_DotCheck(wk->bgl,GF_BGL_FRAME2_M,sys.tp_x,sys.tp_y,&pat) == FALSE){
//		return FALSE;
//	}
	ret = GFL_UI_TP_HitTrg(Btn_TpRect);
	return ret;
}


static int edit_double_touch(PMS_INPUT_WORK* wk)
{
	int ret;

	static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
//		{0,191,0,255}, ty,by,lx,rx
		{TPED_DCBTN_PY0,TPED_DCBTN_PY0+TPED_DCBTN_SY,TPED_DCBTN_PX,TPED_DCBTN_PX+TPED_DCBTN_SX},
		{TPED_DCBTN_PY1,TPED_DCBTN_PY1+TPED_DCBTN_SY,TPED_DCBTN_PX,TPED_DCBTN_PX+TPED_DCBTN_SX},
		{TPED_WORD_PY,TPED_WORD_PY+TPED_WORD_SY,TPED_WORD2_PX0,TPED_WORD2_PX0+TPED_WORD_SX},
		{TPED_WORD_PY,TPED_WORD_PY+TPED_WORD_SY,TPED_WORD2_PX1,TPED_WORD2_PX1+TPED_WORD_SX},
		{GFL_UI_TP_HIT_END,0,0,0}
	};
	ret = GFL_UI_TP_HitTrg(Btn_TpRect);
	return ret;
}
static int edit_sentence_touch(PMS_INPUT_WORK* wk)
{
	int ret,num,i;
	u32 tpx,tpy;
	GFL_UI_TP_HITTBL tbl[2] = { {GFL_UI_TP_HIT_END,0,0,0}, {GFL_UI_TP_HIT_END,0,0,0} };

	static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
//		{0,191,0,255}, ty,by,lx,rx
		{TPED_DCBTN_PY0,TPED_DCBTN_PY0+TPED_DCBTN_SY,TPED_DCBTN_PX,TPED_DCBTN_PX+TPED_DCBTN_SX},
		{TPED_DCBTN_PY1,TPED_DCBTN_PY1+TPED_DCBTN_SY,TPED_DCBTN_PX,TPED_DCBTN_PX+TPED_DCBTN_SX},
//		{TPED_WORD_PY,TPED_WORD_PY+TPED_WORD_SY,TPED_WORD2_PX0,TPED_WORD2_PX0+TPED_WORD_SX},
//		{TPED_WORD_PY,TPED_WORD_PY+TPED_WORD_SY,TPED_WORD2_PX1,TPED_WORD2_PX1+TPED_WORD_SX},
		{TPED_SBTN_PY,TPED_SBTN_PY+TPED_SBTN_SY,TPED_SBTN_PX0,TPED_SBTN_PX0+TPED_SBTN_SX},
		{TPED_SBTN_PY,TPED_SBTN_PY+TPED_SBTN_SY,TPED_SBTN_PX1,TPED_SBTN_PX1+TPED_SBTN_SX},
		{GFL_UI_TP_HIT_END,0,0,0}
	};
	if(GFL_UI_TP_GetTrg() == 0){
		return GFL_UI_TP_HIT_NONE;
	}
	ret = GFL_UI_TP_HitTrg(Btn_TpRect);
	if(ret != GFL_UI_TP_HIT_NONE){
		return ret;
	}
	num = PMSIView_GetSentenceEditPosMax( wk->vwk );
	//TODO Cont? Trg?
	GFL_UI_TP_GetPointTrg( &tpx,&tpy );
	for(i = 0;i < num;i++){
		PMSIView_GetSentenceWordArea( wk->vwk ,&tbl[0],i);
		if(GFL_UI_TP_HitSelf( tbl, tpx , tpy ) != GFL_UI_TP_HIT_NONE ){
			return i+4;
		}

	}
	return GFL_UI_TP_HIT_NONE;
}


static GFL_PROC_RESULT mp_input_touch(PMS_INPUT_WORK* wk,int *seq )
{
	int ret;
	u16	pat;

	switch( *seq ){
	case SEQ_EDW_KEYWAIT:
	case SEQ_EDW_BUTTON_KEYWAIT:
		if(wk->input_mode == 0){
			ret = edit_single_touch(wk);
		}else{
			ret = edit_double_touch(wk);
		}
		switch(ret){
		case 0:	//決定
		case 1:	//やめる
#if PMS_USE_SND
			Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND
			*seq = SEQ_EDW_TO_SUBPROC_OK+ret;
			break;
		case 2:	//単語選択
		case 3:
			wk->category_pos = 0;
			wk->edit_pos = ret-2;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_CATEGORY );
			*seq = SEQ_EDW_TO_SELECT_CATEGORY;
			break;
		}
		break;
	case SEQ_EDW_TO_SUBPROC_OK:
		(*seq) = SEQ_EDW_RETURN_SUBPROC;
		SetSubProc( wk, SubProc_CommandOK );
		break;
	case SEQ_EDW_TO_SUBPROC_CANCEL:
		(*seq) = SEQ_EDW_RETURN_SUBPROC;
		SetSubProc( wk, SubProc_CommandCancel );
		break;
	case SEQ_EDW_RETURN_SUBPROC:
		(*seq) = SEQ_EDW_KEYWAIT;
		break;
	case SEQ_EDW_TO_SELECT_CATEGORY:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			ChangeMainProc( wk, MainProc_Category );
		}
		break;
	}
	return GFL_PROC_RES_CONTINUE;
}

static GFL_PROC_RESULT mp_input_single( PMS_INPUT_WORK* wk, int* seq )
{
	if(	*seq == SEQ_EDW_KEYWAIT ||
		*seq == SEQ_EDW_BUTTON_KEYWAIT){
		if(KeyStatusChange(wk,seq)){
			return GFL_PROC_RES_CONTINUE;
		}
	}
	if(wk->key_mode == GFL_APP_KTST_KEY){
		return mp_input_single_key(wk,seq);
	}else{
		return mp_input_touch(wk,seq);
	}
}

//------------------------------------------------------------------
/**
	* 単語２つ入力モード
	*
	* @param   wk		
	* @param   seq		
	*
	* @retval  GFL_PROC_RESULT		
	*/
//------------------------------------------------------------------
static GFL_PROC_RESULT mp_input_double_key( PMS_INPUT_WORK* wk, int* seq )
{
	switch( *seq ){
	case SEQ_EDW_KEYWAIT:
		if(! PMSIView_WaitCommandAll( wk->vwk ) ){
			break;
		}
		if(KeyStatusChange(wk,seq)){
			break;
		}
		if( wk->key_trg & (PAD_KEY_DOWN|PAD_BUTTON_START) )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND

			wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_BUTTON );
			(*seq) = SEQ_EDW_BUTTON_KEYWAIT;
			break;
		}
		if( wk->key_trg & PAD_KEY_LEFT )
		{
			if( wk->edit_pos != 0 )
			{
#if PMS_USE_SND
				Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
				wk->edit_pos = 0;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_EDITAREA_CURSOR );
			}
			break;
		}
		if( wk->key_trg & PAD_KEY_RIGHT )
		{
			if( wk->edit_pos == 0 )
			{
#if PMS_USE_SND
				Snd_SePlay(SOUND_MOVE_CURSOR);
#endif /PMS_USE_SND
				wk->edit_pos = 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_EDITAREA_CURSOR );
			}
			break;
		}
		if( wk->key_trg & PAD_BUTTON_B )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_CANCEL);
#endif //PMS_USE_SND

			SetSubProc( wk, SubProc_CommandCancel );
			break;
		}
		if( wk->key_trg & PAD_BUTTON_A )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND

			wk->category_pos = 0;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_CATEGORY );
			(*seq) = SEQ_EDW_TO_SELECT_CATEGORY;
			break;
		}
		break;
	case SEQ_EDW_BUTTON_KEYWAIT:
		if(! PMSIView_WaitCommandAll( wk->vwk ) ){
			break;
		}
		if(KeyStatusChange(wk,seq)){
			break;
		}
		if( wk->key_trg & PAD_KEY_UP )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
			
      if(wk->cmd_button_pos){
        // 「やめる」から「けってい」へ
				wk->cmd_button_pos ^= 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			}else{
        // 「けってい」からエディットエリアへ
				PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
				(*seq) = SEQ_EDW_KEYWAIT;
			}
			break;
		}else if( wk->key_trg & (PAD_KEY_DOWN) ){
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
			
      if(wk->cmd_button_pos){
        // 「やめる」からエディットエリアへ
				PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
				(*seq) = SEQ_EDW_KEYWAIT;
			}else{
        // 「けってい」から「やめる」へ
				wk->cmd_button_pos ^= 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			}
			break;
		}

		if( wk->key_trg & PAD_BUTTON_START ){
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
			wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			break;
		}

		if( wk->key_trg & PAD_BUTTON_A ){
#if PMS_USE_SND
			Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND

			if( wk->cmd_button_pos == BUTTON_POS_DECIDE ){
				(*seq) = SEQ_EDW_TO_SUBPROC_OK;
				break;
			}else{
				(*seq) = SEQ_EDW_TO_SUBPROC_CANCEL;
					break;
				}
			}

			if( wk->key_trg & PAD_BUTTON_B ){
#if PMS_USE_SND
				Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND
				(*seq) = SEQ_EDW_TO_SUBPROC_CANCEL;
				break;
			}
		break;
	case SEQ_EDW_TO_SUBPROC_OK:
		(*seq) = SEQ_EDW_RETURN_SUBPROC;
		SetSubProc( wk, SubProc_CommandOK );
		break;

	case SEQ_EDW_TO_SUBPROC_CANCEL:
		(*seq) = SEQ_EDW_RETURN_SUBPROC;
		SetSubProc( wk, SubProc_CommandCancel );
		break;

	case SEQ_EDW_RETURN_SUBPROC:
		(*seq) = SEQ_EDW_BUTTON_KEYWAIT;
		break;

	case SEQ_EDW_TO_COMMAND_BUTTON:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			ChangeMainProc( wk, MainProc_CommandButton );
		}
		break;

	case SEQ_EDW_TO_SELECT_CATEGORY:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			ChangeMainProc( wk, MainProc_Category );
		}
		break;
	}

	return GFL_PROC_RES_CONTINUE;
}
static GFL_PROC_RESULT mp_input_double( PMS_INPUT_WORK* wk, int* seq )
{
	if(	*seq == SEQ_EDW_KEYWAIT ||
		*seq == SEQ_EDW_BUTTON_KEYWAIT){
		if(KeyStatusChange(wk,seq)){
			return GFL_PROC_RES_CONTINUE;
		}
	}
	if(wk->key_mode == GFL_APP_KTST_KEY){
		return mp_input_double_key(wk,seq);
	}else{
		return mp_input_touch(wk,seq);
	}
}

//------------------------------------------------------------------
/**
	* 文章入力モード
	*
	* @param   wk		
	* @param   seq		
	*
	* @retval  GFL_PROC_RESULT		
	*/
//------------------------------------------------------------------
static GFL_PROC_RESULT mp_input_sentence_key( PMS_INPUT_WORK* wk, int* seq )
{

	switch( *seq ){
	case SEQ_EDS_INIT:
		wk->sentence_edit_pos_max = PMSIView_GetSentenceEditPosMax( wk->vwk );
		(*seq) = SEQ_EDS_KEYWAIT;

		/* fallthru */


	case SEQ_EDS_KEYWAIT:
		if( wk->key_trg & PAD_KEY_UP )
		{
			if( (wk->sentence_edit_pos_max) && (wk->edit_pos != 0) )
			{
#if PMS_USE_SND
				Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
				wk->edit_pos--;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_EDITAREA_CURSOR );
			}
			break;
		}
		if( wk->key_trg & PAD_KEY_DOWN )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND

			if( (wk->sentence_edit_pos_max) && (wk->edit_pos < (wk->sentence_edit_pos_max-1)) )
			{
				wk->edit_pos++;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_EDITAREA_CURSOR );
			}
			else
			{
				wk->cmd_button_pos = BUTTON_POS_DECIDE;
				PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_BUTTON );
				(*seq) = SEQ_EDS_BUTTON_KEYWAIT;
				break;
			}
		}
		if( (wk->key_trg & PAD_BUTTON_START)
		||	( (wk->key_trg & PAD_BUTTON_A) && (wk->sentence_edit_pos_max==0) )
		){
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND

			wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_BUTTON );
			(*seq) = SEQ_EDS_BUTTON_KEYWAIT;
			break;
		}
		if( wk->key_repeat & PAD_KEY_LEFT )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_CHANGE_SENTENCE);
#endif //PMS_USE_SND

			wk->edit_pos = 0;
			sentence_decrement( &wk->sentence_wk, &wk->edit_pms );
			PMSIView_SetCommand( wk->vwk, VCMD_UPDATE_EDITAREA );
			(*seq) = SEQ_EDS_WAIT_EDITAREA_UPDATE;
			break;
		}
		if( wk->key_repeat & PAD_KEY_RIGHT )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_CHANGE_SENTENCE);
#endif //PMS_USE_SND

			wk->edit_pos = 0;
			sentence_increment( &wk->sentence_wk, &wk->edit_pms );
			PMSIView_SetCommand( wk->vwk, VCMD_UPDATE_EDITAREA );
			(*seq) = SEQ_EDS_WAIT_EDITAREA_UPDATE;
			break;
		}

		if( wk->key_trg & PAD_BUTTON_B )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_CANCEL);
#endif //PMS_USE_SND
			SetSubProc( wk, SubProc_CommandCancel );
			break;
		}
		if( wk->key_trg & PAD_BUTTON_A )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND

			wk->category_pos = 0;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_CATEGORY );
			(*seq) = SEQ_EDS_TO_SELECT_CATEGORY;
			break;
		}
		break;

	case SEQ_EDS_WAIT_EDITAREA_UPDATE:
		if( PMSIView_WaitCommand( wk->vwk, VCMD_UPDATE_EDITAREA ) )
		{
			(*seq) = SEQ_EDS_INIT;
		}
		break;
	
	case SEQ_EDS_BUTTON_KEYWAIT:
		if(! PMSIView_WaitCommandAll( wk->vwk ) ){
			break;
		}
		if( wk->key_trg & PAD_KEY_UP )
		{
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND

      if(wk->cmd_button_pos){
        // 「やめる」から「けってい」へ
				wk->cmd_button_pos ^= 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			}else{
        // 「けってい」からエディットエリアへ
				PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
				(*seq) = SEQ_EDW_KEYWAIT;
			}
			break;
		}else if( wk->key_trg & (PAD_KEY_DOWN) ){
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
			
      if(wk->cmd_button_pos){
        // 「やめる」からエディットエリアへ
				PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
				(*seq) = SEQ_EDW_KEYWAIT;
			}else{
        // 「けってい」から「やめる」へ
				wk->cmd_button_pos ^= 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			}
			break;
		}

		if( wk->key_trg & PAD_BUTTON_START ){
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
			wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			break;
		}
		if( wk->key_trg & PAD_BUTTON_A ){
#if PMS_USE_SND
			Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND

			if( wk->cmd_button_pos == BUTTON_POS_DECIDE ){
				(*seq) = SEQ_EDS_TO_SUBPROC_OK;
				break;
			}else{
				(*seq) = SEQ_EDS_TO_SUBPROC_CANCEL;
					break;
				}
			}

			if( wk->key_trg & PAD_BUTTON_B ){
#if PMS_USE_SND
				Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND
				(*seq) = SEQ_EDS_TO_SUBPROC_CANCEL;
				break;
			}
		break;
	case SEQ_EDS_TO_SUBPROC_OK:
		(*seq) = SEQ_EDS_RETURN_SUBPROC;
		SetSubProc( wk, SubProc_CommandOK );
		break;

	case SEQ_EDS_TO_SUBPROC_CANCEL:
		(*seq) = SEQ_EDS_RETURN_SUBPROC;
		SetSubProc( wk, SubProc_CommandCancel );
		break;

	case SEQ_EDS_RETURN_SUBPROC:
		(*seq) = SEQ_EDS_BUTTON_KEYWAIT;
		break;

	case SEQ_EDS_TO_COMMAND_BUTTON:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			ChangeMainProc( wk, MainProc_CommandButton );
		}
		break;

	case SEQ_EDS_TO_SELECT_CATEGORY:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			ChangeMainProc( wk, MainProc_Category );
		}
		break;
	}

	return GFL_PROC_RES_CONTINUE;
}

//------------------------------------------------------------------
/**
	* 文章入力モード
	*
	* @param   wk		
	* @param   seq		
	*
	* @retval  GFL_PROC_RESULT		
	*/
//------------------------------------------------------------------
static GFL_PROC_RESULT mp_input_sentence_touch( PMS_INPUT_WORK* wk, int* seq )
{
	int ret;
	u16 pat;

	switch( *seq ){
	case SEQ_EDS_INIT:
		wk->sentence_edit_pos_max = PMSIView_GetSentenceEditPosMax( wk->vwk );
		(*seq) = SEQ_EDS_KEYWAIT;

		/* fallthru */


	case SEQ_EDS_KEYWAIT:
	case SEQ_EDS_BUTTON_KEYWAIT:
		ret = edit_sentence_touch(wk);
		if( ret != GFL_UI_TP_HIT_NONE )
		{
			ARI_TPrintf("[%d]\n",ret);
		}
		
		switch(ret){
		case 0:	//決定
		case 1:	//やめる
#if PMS_USE_SND
			Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND
			*seq = SEQ_EDS_TO_SUBPROC_OK+ret;
			break;
		case 2:	//スクロールボタン
#if PMS_USE_SND
			Snd_SePlay(SOUND_CHANGE_SENTENCE);
#endif //PMS_USE_SND

			wk->edit_pos = 0;
			sentence_decrement( &wk->sentence_wk, &wk->edit_pms );
			PMSIView_SetCommand( wk->vwk, VCMD_UPDATE_EDITAREA );
			(*seq) = SEQ_EDS_WAIT_EDITAREA_UPDATE;
			break;
		case 3:
#if PMS_USE_SND
			Snd_SePlay(SOUND_CHANGE_SENTENCE);
#endif //PMS_USE_SND

			wk->edit_pos = 0;
			sentence_increment( &wk->sentence_wk, &wk->edit_pms );
			PMSIView_SetCommand( wk->vwk, VCMD_UPDATE_EDITAREA );
			(*seq) = SEQ_EDS_WAIT_EDITAREA_UPDATE;
			break;
		case 4:	//単語選択
		case 5:
			wk->category_pos = 0;
			wk->edit_pos = ret-4;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_CATEGORY );
			*seq = SEQ_EDS_TO_SELECT_CATEGORY;
			break;
		}
		//ページスクロール
#if 0	//移植していません
		if( wk->key_trg & PAD_KEY_UP )
		{
			if( (wk->sentence_edit_pos_max) && (wk->edit_pos != 0) )
			{
				Snd_SePlay(SOUND_MOVE_CURSOR);
				wk->edit_pos--;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_EDITAREA_CURSOR );
			}
			break;
		}
		if( wk->key_trg & PAD_KEY_DOWN )
		{
			Snd_SePlay(SOUND_MOVE_CURSOR);

			if( (wk->sentence_edit_pos_max) && (wk->edit_pos < (wk->sentence_edit_pos_max-1)) )
			{
				wk->edit_pos++;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_EDITAREA_CURSOR );
			}
			else
			{
				wk->cmd_button_pos = BUTTON_POS_DECIDE;
				PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_BUTTON );
				(*seq) = SEQ_EDS_BUTTON_KEYWAIT;
				break;
			}
		}
		if( (wk->key_trg & PAD_BUTTON_START)
		||	( (wk->key_trg & PAD_BUTTON_A) && (wk->sentence_edit_pos_max==0) )
		){
			Snd_SePlay(SOUND_MOVE_CURSOR);

			wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_BUTTON );
			(*seq) = SEQ_EDS_BUTTON_KEYWAIT;
			break;
		}
		if( wk->key_repeat & PAD_KEY_LEFT )
		{
			Snd_SePlay(SOUND_CHANGE_SENTENCE);

			wk->edit_pos = 0;
			sentence_decrement( &wk->sentence_wk, &wk->edit_pms );
			PMSIView_SetCommand( wk->vwk, VCMD_UPDATE_EDITAREA );
			(*seq) = SEQ_EDS_WAIT_EDITAREA_UPDATE;
			break;
		}
		if( wk->key_repeat & PAD_KEY_RIGHT )
		{
			Snd_SePlay(SOUND_CHANGE_SENTENCE);

			wk->edit_pos = 0;
			sentence_increment( &wk->sentence_wk, &wk->edit_pms );
			PMSIView_SetCommand( wk->vwk, VCMD_UPDATE_EDITAREA );
			(*seq) = SEQ_EDS_WAIT_EDITAREA_UPDATE;
			break;
		}

		if( wk->key_trg & PAD_BUTTON_B )
		{
			Snd_SePlay(SOUND_CANCEL);
			SetSubProc( wk, SubProc_CommandCancel );
			break;
		}
		if( wk->key_trg & PAD_BUTTON_A )
		{
			Snd_SePlay(SOUND_DECIDE);

			wk->category_pos = 0;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_CATEGORY );
			(*seq) = SEQ_EDS_TO_SELECT_CATEGORY;
			break;
		}
#endif
		break;

	case SEQ_EDS_WAIT_EDITAREA_UPDATE:
		if( PMSIView_WaitCommand( wk->vwk, VCMD_UPDATE_EDITAREA ) )
		{
			(*seq) = SEQ_EDS_INIT;
		}
		break;
	
	case SEQ_EDS_TO_SUBPROC_OK:
		(*seq) = SEQ_EDS_RETURN_SUBPROC;
		SetSubProc( wk, SubProc_CommandOK );
		break;

	case SEQ_EDS_TO_SUBPROC_CANCEL:
		(*seq) = SEQ_EDS_RETURN_SUBPROC;
		SetSubProc( wk, SubProc_CommandCancel );
		break;

	case SEQ_EDS_RETURN_SUBPROC:
		(*seq) = SEQ_EDS_BUTTON_KEYWAIT;
		break;

	case SEQ_EDS_TO_COMMAND_BUTTON:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			ChangeMainProc( wk, MainProc_CommandButton );
		}
		break;

	case SEQ_EDS_TO_SELECT_CATEGORY:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			ChangeMainProc( wk, MainProc_Category );
		}
		break;
	}

	return GFL_PROC_RES_CONTINUE;
}

static GFL_PROC_RESULT mp_input_sentence( PMS_INPUT_WORK* wk, int* seq )
{
	if(	*seq == SEQ_EDS_INIT || 
		*seq == SEQ_EDS_KEYWAIT ||
		*seq == SEQ_EDS_BUTTON_KEYWAIT){
		if(KeyStatusChange(wk,seq)){
			return GFL_PROC_RES_CONTINUE;
		}
	}
	if(wk->key_mode == GFL_APP_KTST_KEY){
		return mp_input_sentence_key(wk,seq);
	}else{
		return mp_input_sentence_touch(wk,seq);
	}
}

//----------------------------------------------------------------------------------------------
/**
	* メインプロセス：コマンドボタン（けってい・やめる）での動作
	*
	* @param   wk		
	* @param   seq		
	*
	*/
//----------------------------------------------------------------------------------------------
static GFL_PROC_RESULT MainProc_CommandButton( PMS_INPUT_WORK* wk, int* seq )
{
	enum {
		SEQ_KEYWAIT,
		SEQ_TO_SUBPROC_OK,
		SEQ_TO_SUBPROC_CANCEL,
		SEQ_RETURN_SUBPROC_OK,
		SEQ_RETURN_SUBPROC_CANCEL,
		SEQ_TO_INPUT_AREA,
	};

	switch( *seq ){
	case SEQ_KEYWAIT:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			if( wk->key_trg & PAD_KEY_UP )
			{
#if PMS_USE_SND
				Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND

        if(wk->cmd_button_pos){
          // 「やめる」から「けってい」へ
          wk->cmd_button_pos ^= 1;
          PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
        }else{
          // 「けってい」からエディットエリアへ
          PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
          (*seq) = SEQ_EDW_KEYWAIT;
        }
        break;
      }else if( wk->key_trg & (PAD_KEY_DOWN) ){
#if PMS_USE_SND
        Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
        
        if(wk->cmd_button_pos){
          // 「やめる」からエディットエリアへ
          PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
          (*seq) = SEQ_EDW_KEYWAIT;
        }else{
          // 「けってい」から「やめる」へ
          wk->cmd_button_pos ^= 1;
          PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
        }
        break;
      }

			if( wk->key_trg & PAD_BUTTON_START )
			{
#if PMS_USE_SND
				Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
				wk->cmd_button_pos = BUTTON_POS_DECIDE;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
				break;
			}


			if( wk->key_trg & PAD_BUTTON_A )
			{
#if PMS_USE_SND
				Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND

				if( wk->cmd_button_pos == BUTTON_POS_DECIDE )
				{
					(*seq) = SEQ_TO_SUBPROC_OK;
					break;
				}
				else
				{
					(*seq) = SEQ_TO_SUBPROC_CANCEL;
					break;
				}
			}

			if( wk->key_trg & PAD_BUTTON_B )
			{
#if PMS_USE_SND
				Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND
				(*seq) = SEQ_TO_SUBPROC_CANCEL;
				break;
			}
		}
		break;

	case SEQ_TO_SUBPROC_OK:
		(*seq) = SEQ_RETURN_SUBPROC_OK;
		SetSubProc( wk, SubProc_CommandOK );
		break;

	case SEQ_TO_SUBPROC_CANCEL:
		(*seq) = SEQ_RETURN_SUBPROC_CANCEL;
		SetSubProc( wk, SubProc_CommandCancel );
		break;

	case SEQ_RETURN_SUBPROC_OK:
	case SEQ_RETURN_SUBPROC_CANCEL:
		(*seq) = SEQ_KEYWAIT;
		break;

	case SEQ_TO_INPUT_AREA:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			ChangeMainProc( wk, MainProc_EditArea );
		}
		break;
	}


	return GFL_PROC_RES_CONTINUE;
}


//----------------------------------------------------------------------------------------------
/**
	* メインプロセス：カテゴリ選択
	*
	* @param   wk
	* @param   seq		
	*
	*/
//----------------------------------------------------------------------------------------------
static GFL_PROC_RESULT MainProc_Category( PMS_INPUT_WORK* wk, int* seq )
{
	switch( *seq ){
	case SEQ_CA_KEYWAIT:
		if (PMSIView_WaitCommand( wk->vwk, VCMD_MOVE_WORDWIN_CURSOR) == FALSE )
		{
			break;
		}
		category_input(wk,seq);

		break;

	case SEQ_CA_NEXTPROC:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			ChangeMainProc( wk, wk->next_proc );
		}
		break;

	case SEQ_CA_WAIT_MODE_CHANGE:
		if( PMSIView_WaitCommand( wk->vwk, VCMD_CHANGE_CATEGORY_MODE_ENABLE ) )
		{
			(*seq) = SEQ_CA_KEYWAIT;
		}
		break;
	}

	return GFL_PROC_RES_CONTINUE;
}

/**
	*	@brief
	*/
static void category_input_key(PMS_INPUT_WORK* wk,int* seq)
{
	if(	(wk->touch_button == TOUCH_BUTTON_GROUP) && (wk->category_mode != CATEGORY_MODE_GROUP)
	||	(wk->touch_button == TOUCH_BUTTON_INITIAL) && (wk->category_mode !=CATEGORY_MODE_INITIAL)
	||	(wk->key_trg & PAD_BUTTON_SELECT)
	){
#if PMS_USE_SND
		Snd_SePlay(SOUND_CHANGE_CATEGORY);
#endif //PMS_USE_SND
		wk->category_mode ^= 1;
		wk->category_pos = 0;
		PMSIView_SetCommand( wk->vwk, VCMD_CHANGE_CATEGORY_MODE_ENABLE );
		(*seq) = SEQ_CA_WAIT_MODE_CHANGE;
		return;
	}

	if( wk->key_trg & PAD_BUTTON_B )
	{
#if PMS_USE_SND
		Snd_SePlay(SOUND_CANCEL);
#endif //PMS_USE_SND

		PMSIView_SetCommand( wk->vwk, VCMD_CATEGORY_TO_EDITAREA );
		wk->next_proc = MainProc_EditArea;
		*seq = SEQ_CA_NEXTPROC;
		return;
	}
	if( wk->key_trg & PAD_BUTTON_A ){
		if( wk->category_pos == CATEGORY_POS_BACK ){
#if PMS_USE_SND
			Snd_SePlay(SOUND_CANCEL);
#endif //PMS_USE_SND

			PMSIView_SetCommand( wk->vwk, VCMD_CATEGORY_TO_EDITAREA );
			wk->next_proc = MainProc_EditArea;
			*seq = SEQ_CA_NEXTPROC;
		}else if( check_category_enable( wk ) ){
#if PMS_USE_SND
			Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND

			setup_wordwin_params( &wk->word_win, wk );
			wk->next_proc = MainProc_WordWin;
			PMSIView_SetCommand( wk->vwk, VCMD_CATEGORY_TO_WORDWIN );
			*seq = SEQ_CA_NEXTPROC;
		}else{
#if PMS_USE_SND
			Snd_SePlay( SOUND_DISABLE_CATEGORY );
#endif //PMS_USE_SND
		}
		return;
	}
	if( check_category_cursor_move( wk ) ){
#if PMS_USE_SND
		Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
		PMSIView_SetCommand( wk->vwk, VCMD_MOVE_CATEGORY_CURSOR );
		return;
	}
}

/**
	*	@brief	モード切替ボタン取得
	*/
static int input_category_word_btn(PMS_INPUT_WORK* wk)
{
	int ret;

	static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
//		{0,191,0,255}, ty,by,lx,rx
		{TPCA_RET_PY,TPCA_RET_PY+TPCA_RET_SY,TPCA_RET_PX,TPCA_RET_PX+TPCA_RET_SX},
		{TPCA_MODE_PY,TPCA_MODE_PY+TPCA_MODE_SY,TPCA_GMODE_PX,TPCA_GMODE_PX+TPCA_MODE_SX},
		{TPCA_MODE_PY,TPCA_MODE_PY+TPCA_MODE_SY,TPCA_IMODE_PX,TPCA_IMODE_PX+TPCA_MODE_SX},
		{GFL_UI_TP_HIT_END,0,0,0}
	};
	ret = GFL_UI_TP_HitTrg(Btn_TpRect);
	if(	ret == GFL_UI_TP_HIT_NONE){
		return 0;
	}
	switch(ret){
	case 0:	//戻る
		return 1;
	case 1:	//グループ
		if(wk->touch_button == TOUCH_BUTTON_GROUP){
			return 0;
		}
		break;
	case 2:	//やめる
		if(wk->touch_button == TOUCH_BUTTON_INITIAL){
			return 0;
		}
	}
	return 2;
}

static int category_touch_group(PMS_INPUT_WORK* wk)
{
	int i,j;
	u32 tpx,tpy;
	GFL_UI_TP_HITTBL tbl[2] = { {GFL_UI_TP_HIT_END,0,0,0}, {GFL_UI_TP_HIT_END,0,0,0} };

	if(GFL_UI_TP_GetTrg() == 0){
		return -1;
	}
	//TODO Cont? Trg?
	GFL_UI_TP_GetPointTrg( &tpx,&tpy );
	for(i = 0;i < 4;i++){
		tbl[0].rect.top = TPCA_GMA_PY+TPCA_GMA_OY*i - CATEGORY_BG_ENABLE_YOFS;
		tbl[0].rect.bottom = tbl[0].rect.top + TPCA_GMA_SY;
		for(j = 0;j < 3;j++){
			tbl[0].rect.left = TPCA_GMA_PX+TPCA_GMA_OX*j - GROUPMODE_BG_XOFS;
			tbl[0].rect.right = tbl[0].rect.left + TPCA_GMA_SX;
			
			if(GFL_UI_TP_HitSelf( tbl, tpx,tpy) != GFL_UI_TP_HIT_NONE ){
				return i*3+j;
			}
		}
	}
	return -1;
}

static int category_touch_initial(PMS_INPUT_WORK* wk)
{
	int ret;
	u32 tpx,tpy;
	s16 x,y;
	static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
//		{0,191,0,255}, ty,by,lx,rx
		{TPCA_IMA_PY,TPCA_IMA_PY+TPCA_IMA_SY*4,TPCA_IMA_PX,TPCA_IMA_PX+TPCA_IMA_SX*15},
		{GFL_UI_TP_HIT_END,0,0,0}
	};
	
	ret = GFL_UI_TP_HitTrg(Btn_TpRect);
	if(	ret == GFL_UI_TP_HIT_NONE){
		return -1;
	}
	GFL_UI_TP_GetPointTrg( &tpx,&tpy );
	x = (tpx - TPCA_IMA_PX) / TPCA_IMA_SX;
	y = (tpy - TPCA_IMA_PY) / TPCA_IMA_SY;
	if(x == 5 || x == 11){
		return -1;
	}
	if(x < 5){
		ARI_TPrintf(" Initial Hit0 = %d\n",y*5+x);
		return y*5+x;
	}
	if(x < 11){
		ARI_TPrintf(" Initial Hit1 = %d\n",y*5+(x-6)+20);
		return y*5+(x-6)+20;
	}
	if(y == 0){
		ARI_TPrintf(" Initial Hit2 = %d\n",(x-12)+40);
		return (x-12)+40;
	}
	if(y == 1 && x < 14){
		ARI_TPrintf(" Initial Hit3 = %d\n",(x-12)+43);
		return (x-12)+43;
	}
	return -1;
}

/**
	*	@brief
	*/
static void category_input_touch(PMS_INPUT_WORK* wk,int* seq)
{
	int ret;
	static const u8	ini_idx[] = {
	 INI_A,INI_I,INI_U,INI_E,INI_O,
	 INI_KA,INI_KI,INI_KU,INI_KE,INI_KO,
	 INI_SA,INI_SI,INI_SU,INI_SE,INI_SO,
	 INI_TA,INI_TI,INI_TU,INI_TE,INI_TO,
	 INI_NA,INI_NI,INI_NU,INI_NE,INI_NO,
	 INI_HA,INI_HI,INI_HU,INI_HE,INI_HO,
	 INI_MA,INI_MI,INI_MU,INI_ME,INI_MO,
	 INI_RA,INI_RI,INI_RU,INI_RE,INI_RO,
	 INI_YA,INI_YU,INI_YO,
	 INI_WA,INI_OTHER,
	};	
	ret = input_category_word_btn(wk);
	switch(ret){
	case 1:
#if PMS_USE_SND
		Snd_SePlay(SOUND_CANCEL);
#endif //PMS_USE_SND

		PMSIView_SetCommand( wk->vwk, VCMD_CATEGORY_TO_EDITAREA );
		wk->next_proc = MainProc_EditArea;
		*seq = SEQ_CA_NEXTPROC;
		return;
	case 2:
#if PMS_USE_SND
		Snd_SePlay(SOUND_CHANGE_CATEGORY);
#endif //PMS_USE_SND
		wk->category_mode ^= 1;
		wk->category_pos = 0;
		PMSIView_SetCommand( wk->vwk, VCMD_CHANGE_CATEGORY_MODE_ENABLE );
		(*seq) = SEQ_CA_WAIT_MODE_CHANGE;
		return;
	}
	if( wk->category_mode == CATEGORY_MODE_GROUP ){
    // グループ
		ret = category_touch_group(wk);
		if(ret < 0){
			return;
		}
		wk->category_pos = ret;
		if(PMSI_DATA_GetGroupEnableWordCount( wk->dwk, wk->category_pos ) == 0){
#if PMS_USE_SND
			Snd_SePlay( SOUND_DISABLE_CATEGORY );
#endif //PMS_USE_SND
			return;
		}
	}else{
    // イニシャル
		ret = category_touch_initial(wk);
		if(ret < 0){
			return;
		}
		wk->category_pos = ini_idx[ret];
		if(PMSI_DATA_GetInitialEnableWordCount( wk->dwk, wk->category_pos ) == 0 ){
#if PMS_USE_SND
			Snd_SePlay( SOUND_DISABLE_CATEGORY );
#endif //PMS_USE_SND
			return;
		}
	}
#if PMS_USE_SND
	Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND

	setup_wordwin_params( &wk->word_win, wk );
	wk->next_proc = MainProc_WordWin;
	PMSIView_SetCommand( wk->vwk, VCMD_CATEGORY_TO_WORDWIN );
	*seq = SEQ_CA_NEXTPROC;
}

/**
	*	@brief	
	*/
static void category_input(PMS_INPUT_WORK* wk,int* seq)
{
	if(KeyStatusChange(wk,seq)){
		return;
	}
	if(wk->key_mode == GFL_APP_KTST_KEY){
		category_input_key(wk,seq);
	}else{
		category_input_touch(wk,seq);
	}
}


//------------------------------------------------------------------
/**
	* 選択しているカテゴリに使用可能な単語が含まれているかチェック
	*
	* @param   wk		
	*
	* @retval  BOOL	TRUEで含まれている
	*/
//------------------------------------------------------------------
static BOOL check_category_enable( PMS_INPUT_WORK* wk )
{
	if( wk->category_mode == CATEGORY_MODE_GROUP )
	{
		return (PMSI_DATA_GetGroupEnableWordCount( wk->dwk, wk->category_pos ) != 0);
	}
	else
	{
		return (PMSI_DATA_GetInitialEnableWordCount( wk->dwk, wk->category_pos ) != 0 );
	}
	
}

//------------------------------------------------------------------
/**
	* カテゴリ選択時のカーソル移動チェック
	*
	* @param   wk			ワークポインタ
	*
	* @retval  BOOL		TRUEでカーソル移動した
	*/
//------------------------------------------------------------------
static BOOL check_category_cursor_move( PMS_INPUT_WORK* wk )
{
	static BOOL (* const func_tbl[])(PMS_INPUT_WORK*) = {
		keycheck_category_group_mode,
		keycheck_category_initial_mode,
	};

	return func_tbl[wk->category_mode]( wk );
}
//----------------------------------------------
// グループモード
//----------------------------------------------
static BOOL keycheck_category_group_mode( PMS_INPUT_WORK* wk )
{
	static const struct {
		u8   up_pos;
		u8   down_pos;
		u8   left_pos;
		u8   right_pos;
	}next_pos_tbl[] = {
		{ CATEGORY_GROUP_PICTURE,		CATEGORY_GROUP_TRAINER,	CATEGORY_GROUP_SKILL,	  CATEGORY_GROUP_STATUS },	// ポケモン
		{ CATEGORY_GROUP_PICTURE,	  CATEGORY_GROUP_UNION,	  CATEGORY_GROUP_POKEMON,	CATEGORY_GROUP_SKILL },	  // ステータス
		{ CATEGORY_GROUP_PICTURE,		CATEGORY_GROUP_GREET,	  CATEGORY_GROUP_STATUS,  CATEGORY_GROUP_POKEMON },	// わざ

		{ CATEGORY_GROUP_POKEMON,	  CATEGORY_GROUP_PERSON,  	CATEGORY_GROUP_GREET,	  CATEGORY_GROUP_UNION },	  // トレーナー
		{ CATEGORY_GROUP_STATUS,  	CATEGORY_GROUP_LIFE,	  	CATEGORY_GROUP_TRAINER,	CATEGORY_GROUP_GREET },		// ユニオン
		{ CATEGORY_GROUP_SKILL,	    CATEGORY_GROUP_MIND,	    CATEGORY_GROUP_UNION,	  CATEGORY_GROUP_TRAINER },	// あいさつ

		{ CATEGORY_GROUP_TRAINER,	  CATEGORY_GROUP_PICTURE,	  CATEGORY_GROUP_MIND,	  CATEGORY_GROUP_LIFE },		// ひと
		{ CATEGORY_GROUP_UNION,	    CATEGORY_GROUP_PICTURE,	  CATEGORY_GROUP_PERSON,	CATEGORY_GROUP_MIND },	  // せいかつ
		{ CATEGORY_GROUP_GREET,	    CATEGORY_GROUP_PICTURE,	  CATEGORY_GROUP_LIFE,	  CATEGORY_GROUP_PERSON },	// きもち

	  { CATEGORY_GROUP_PERSON,	  CATEGORY_GROUP_POKEMON,		CATEGORY_GROUP_PICTURE,	CATEGORY_GROUP_PICTURE },	// ピクチャ
	};

	u32  pos = wk->category_pos;
	if( pos == CATEGORY_POS_BACK )
	{
		// テーブルを引くように
		pos = CATEGORY_GROUP_MAX;
	}

	if( wk->category_pos != CATEGORY_POS_BACK )
	{
		wk->category_pos_prv = wk->category_pos;

		if( wk->key_repeat & PAD_KEY_UP )
		{
			wk->category_pos = next_pos_tbl[ pos ].up_pos;
			return TRUE;
		}
		if( wk->key_repeat & PAD_KEY_DOWN )
		{
			wk->category_pos = next_pos_tbl[ pos ].down_pos;
			return TRUE;
		}
		if( wk->key_repeat & PAD_KEY_LEFT )
		{
			wk->category_pos = next_pos_tbl[ pos ].left_pos;
			return TRUE;
		}
		if( wk->key_repeat & PAD_KEY_RIGHT )
		{
			wk->category_pos = next_pos_tbl[ pos ].right_pos;
			return TRUE;
		}
	}
	else
	{
		if( wk->key_repeat & (PAD_KEY_UP) )
		{
			wk->category_pos = next_pos_tbl[CATEGORY_GROUP_MAX].up_pos + (wk->category_pos_prv % 3);
			return TRUE;
		}

		if( wk->key_repeat & (PAD_KEY_DOWN) )
		{
			wk->category_pos = next_pos_tbl[CATEGORY_GROUP_MAX].down_pos + (wk->category_pos_prv % 3);
			return TRUE;
		}

	}
	return FALSE;
}

//----------------------------------------------
// イニシャルモード
//----------------------------------------------
static BOOL keycheck_category_initial_mode( PMS_INPUT_WORK* wk )
{
	if( wk->category_pos != CATEGORY_POS_BACK )
	{
		int  next_pos = CATEGORY_POS_DISABLE;

    // @TODO これは何のための while?
		do
		{
			if( wk->key_repeat & PAD_KEY_UP )
			{
				next_pos = PMSI_INITIAL_DAT_GetUpCode( wk->category_pos );
				break;
			}
			if( wk->key_repeat & PAD_KEY_DOWN )
			{
				next_pos = PMSI_INITIAL_DAT_GetDownCode( wk->category_pos );
				break;
			}
			if( wk->key_repeat & PAD_KEY_RIGHT )
			{
				next_pos = PMSI_INITIAL_DAT_GetRightCode( wk->category_pos );
				break;
			}
			if( wk->key_repeat & PAD_KEY_LEFT )
			{
				next_pos = PMSI_INITIAL_DAT_GetLeftCode( wk->category_pos );
				break;
			}

		}while(0);

		if( next_pos != CATEGORY_POS_DISABLE )
		{
			wk->category_pos_prv = wk->category_pos;
			wk->category_pos = next_pos;
			return TRUE;
		}
	}
	else
	{
		if( wk->key_repeat & PAD_KEY_UP )
		{
			wk->category_pos = PMSI_INITIAL_DAT_GetColBottomCode( wk->category_pos_prv );
			return TRUE;
		}
		if( wk->key_repeat & PAD_KEY_DOWN )
		{
			wk->category_pos = PMSI_INITIAL_DAT_GetColTopCode( wk->category_pos_prv );
			return TRUE;
		}
	}
	return FALSE;
}


//------------------------------------------------------------------
/**
	* 単語ウィンドウパラメータ初期化
	*
	* @param   word_win		
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
static void setup_wordwin_params( WORDWIN_WORK* word_win, PMS_INPUT_WORK* wk )
{
	word_win->line = 0;
	word_win->cursor_x = 0;
	word_win->cursor_y = 0;
	word_win->word_max = PMSI_GetCategoryWordMax(wk);
	word_win->scroll_lines = 0;
	word_win->back_f = 0;
	word_win->touch_pos = 0xFFFF;

	if( word_win->word_max > WORDWIN_DISP_MAX )
	{
		word_win->line_max = ((word_win->word_max - WORDWIN_DISP_MAX) / 2) + (word_win->word_max & 1);
	}
	else
	{
		word_win->line_max = 0;
	}

}

static u32 get_wordwin_cursorpos( const WORDWIN_WORK* wordwin )
{
	return (wordwin->cursor_y * 2) + wordwin->cursor_x;
}
static u32 get_wordwin_pos( const WORDWIN_WORK* wordwin )
{
	return (wordwin->line * 2) + get_wordwin_cursorpos( wordwin );
}
static int get_wordwin_scroll_vector( const WORDWIN_WORK* wordwin )
{
	return wordwin->scroll_lines;
}
static int get_wordwin_linepos( const WORDWIN_WORK* wordwin )
{
	return wordwin->line;
}
static int get_wordwin_linemax( const WORDWIN_WORK* wordwin )
{
	return wordwin->line_max;
}


//----------------------------------------------------------------------------------------------
/**
	* メインプロセス：単語選択
	*
	* @param   wk
	* @param   seq		
	*
	*/
//----------------------------------------------------------------------------------------------
static GFL_PROC_RESULT MainProc_WordWin( PMS_INPUT_WORK* wk, int* seq )
{

	switch(*seq ) {
	case SEQ_WORD_KEYWAIT:
		word_input(wk,seq);
		break;
	case SEQ_WORD_SCROLL_WAIT:
		if( PMSIView_WaitCommand( wk->vwk, VCMD_SCROLL_WORDWIN ) )
		{
			(*seq) = SEQ_WORD_KEYWAIT;
		}
		break;

	case SEQ_WORD_SCROLL_WAIT_AND_CURSOR_MOVE:
		if( PMSIView_WaitCommand( wk->vwk, VCMD_SCROLL_WORDWIN ) )
		{
			PMSIView_SetCommand( wk->vwk, VCMD_MOVE_WORDWIN_CURSOR );
			(*seq) = SEQ_WORD_KEYWAIT;
		}
		break;

	case SEQ_WORD_CHANGE_NEXTPROC:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			ChangeMainProc(wk, wk->next_proc );
			(*seq) = SEQ_WORD_KEYWAIT;
		}
		break;
	}


	return GFL_PROC_RES_CONTINUE;
}

static void word_input_key(PMS_INPUT_WORK* wk,int* seq)
{
	if(	(wk->touch_button == TOUCH_BUTTON_GROUP) && (wk->category_mode != CATEGORY_MODE_GROUP)
	||	(wk->touch_button == TOUCH_BUTTON_INITIAL) && (wk->category_mode !=CATEGORY_MODE_INITIAL)
	||	(wk->key_trg & PAD_BUTTON_SELECT)
	){
#if PMS_USE_SND
		Snd_SePlay(SOUND_CHANGE_CATEGORY);
#endif //PMS_USE_SND
		wk->category_pos = 0;
		wk->category_mode ^= 1;
		PMSIView_SetCommand( wk->vwk, VCMD_WORDWIN_TO_CATEGORY );
		wk->next_proc = MainProc_Category;
		*seq = SEQ_WORD_CHANGE_NEXTPROC;
		return;
	}

	{
		int result;

		if( wk->touch_button == TOUCH_BUTTON_UP ){
			result = check_wordwin_scroll_up( &(wk->word_win) );
		}else if( wk->touch_button == TOUCH_BUTTON_DOWN ){
			result = check_wordwin_scroll_down( &(wk->word_win) );
		}else{
			result = check_wordwin_key( &(wk->word_win), wk->key_repeat );
		}

		switch( result ){
		case WORDWIN_RESULT_CURSOR_MOVE:
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
			PMSIView_SetCommand( wk->vwk, VCMD_MOVE_WORDWIN_CURSOR );
			return;

		case WORDWIN_RESULT_SCROLL:
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
			PMSIView_SetCommand( wk->vwk, VCMD_SCROLL_WORDWIN );
			(*seq) = SEQ_WORD_SCROLL_WAIT;
			return;

		case WORDWIN_RESULT_SCROLL_AND_CURSOR_MOVE:
#if PMS_USE_SND
			Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
			PMSIView_SetCommand( wk->vwk, VCMD_SCROLL_WORDWIN );
			(*seq) = SEQ_WORD_SCROLL_WAIT_AND_CURSOR_MOVE;
			return;
		}
	}

	if( wk->key_trg & PAD_BUTTON_B )
	{
#if PMS_USE_SND
		Snd_SePlay(SOUND_CANCEL);
#endif //PMS_USE_SND

		PMSIView_SetCommand( wk->vwk, VCMD_WORDWIN_TO_CATEGORY );
		wk->next_proc = MainProc_Category;
		*seq = SEQ_WORD_CHANGE_NEXTPROC;
		return;
	}
	if( wk->key_trg & PAD_BUTTON_A )
	{
		if(wk->word_win.back_f){
#if PMS_USE_SND
			Snd_SePlay(SOUND_CANCEL);
#endif //PMS_USE_SND

			PMSIView_SetCommand( wk->vwk, VCMD_WORDWIN_TO_CATEGORY );
			wk->next_proc = MainProc_Category;
			*seq = SEQ_WORD_CHANGE_NEXTPROC;
			return;
		}
#if PMS_USE_SND
		Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND

		wk->word_win.touch_pos = 0xFFFF;
		set_select_word( wk );
		PMSIView_SetCommand( wk->vwk, VCMD_WORDTIN_TO_EDITAREA );
		wk->next_proc = MainProc_EditArea;
		(*seq) = SEQ_WORD_CHANGE_NEXTPROC;
		return;
	}
}

static int word_input_scr_btn(PMS_INPUT_WORK* wk)
{
	int ret,i,j;
	u32 tpx,tpy;
	u16	pos,end_f;
	GFL_UI_TP_HITTBL tbl[2] = { {GFL_UI_TP_HIT_END,0,0,0}, {GFL_UI_TP_HIT_END,0,0,0} };

	static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
//		{0,191,0,255}, ty,by,lx,rx
		{TPWD_SCR_PY0,TPWD_SCR_PY0+TPWD_SCR_SY,TPWD_SCR_PX,TPWD_SCR_PX+TPWD_SCR_SX},
		{TPWD_SCR_PY1,TPWD_SCR_PY1+TPWD_SCR_SY,TPWD_SCR_PX,TPWD_SCR_PX+TPWD_SCR_SX},
		{GFL_UI_TP_HIT_END,0,0,0}
	};
	if(GFL_UI_TP_GetTrg() == 0){
		return WORDWIN_RESULT_NONE;
	}
	ret = GFL_UI_TP_HitTrg(Btn_TpRect);
	switch(ret){
	case 0:
		return check_wordwin_scroll_up( &(wk->word_win) );
	case 1:
		return check_wordwin_scroll_down( &(wk->word_win) );
	}
	GFL_UI_TP_GetPointTrg( &tpx,&tpy );
	pos = wk->word_win.line*2;
	end_f = 0;
	for(i = 0;i < 5;i++){
		tbl[0].rect.top = i*TPWD_WORD_OY+TPWD_WORD_PY;
		tbl[0].rect.bottom = tbl[0].rect.top+TPWD_WORD_SY;
		for(j = 0;j < 2;j++){
			tbl[0].rect.left = j*TPWD_WORD_OX+TPWD_WORD_PX;
			tbl[0].rect.right = tbl[0].rect.left+TPWD_WORD_SX;
		
			if(GFL_UI_TP_HitSelf( tbl, tpx,tpy) != GFL_UI_TP_HIT_NONE ){
				wk->word_win.touch_pos = pos;

				ARI_TPrintf(" Select Pos = %d -> %d\n",i*2+j , pos);
				return WORDWIN_RESULT_SELECT;
			}
			if(++pos >= wk->word_win.word_max){
				end_f = 1;
				break;
			}
		}
		if(end_f){
			break;
		}
	}
	return WORDWIN_RESULT_NONE;
}

static void word_input_touch(PMS_INPUT_WORK* wk,int* seq)
{
	int ret;
	
	ret = input_category_word_btn(wk);
	switch(ret){
	case 1:
#if PMS_USE_SND
		Snd_SePlay(SOUND_CANCEL);
#endif //PMS_USE_SND

		PMSIView_SetCommand( wk->vwk, VCMD_WORDWIN_TO_CATEGORY );
		wk->next_proc = MainProc_Category;
		*seq = SEQ_WORD_CHANGE_NEXTPROC;
		return;
	case 2:
#if PMS_USE_SND
		Snd_SePlay(SOUND_CHANGE_CATEGORY);
#endif //PMS_USE_SND
		wk->category_pos = 0;
		wk->category_mode ^= 1;
		PMSIView_SetCommand( wk->vwk, VCMD_WORDWIN_TO_CATEGORY );
		wk->next_proc = MainProc_Category;
		*seq = SEQ_WORD_CHANGE_NEXTPROC;
		return;
	}
	ret = word_input_scr_btn(wk);

	switch( ret ){
	case WORDWIN_RESULT_SELECT:
#if PMS_USE_SND
		Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND

		set_select_word( wk );
		wk->next_proc = MainProc_EditArea;
		PMSIView_SetCommand( wk->vwk, VCMD_WORDTIN_TO_EDITAREA );
		(*seq) = SEQ_WORD_CHANGE_NEXTPROC;
		return;
	case WORDWIN_RESULT_CURSOR_MOVE:
#if PMS_USE_SND
		Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
		PMSIView_SetCommand( wk->vwk, VCMD_MOVE_WORDWIN_CURSOR );
		return;

	case WORDWIN_RESULT_SCROLL:
#if PMS_USE_SND
		Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
		PMSIView_SetCommand( wk->vwk, VCMD_SCROLL_WORDWIN );
		(*seq) = SEQ_WORD_SCROLL_WAIT;
		return;

	case WORDWIN_RESULT_SCROLL_AND_CURSOR_MOVE:
#if PMS_USE_SND
		Snd_SePlay(SOUND_MOVE_CURSOR);
#endif //PMS_USE_SND
		PMSIView_SetCommand( wk->vwk, VCMD_SCROLL_WORDWIN );
		(*seq) = SEQ_WORD_SCROLL_WAIT_AND_CURSOR_MOVE;
		return;
	}
}

static void word_input(PMS_INPUT_WORK* wk,int* seq)
{
	if(KeyStatusChange(wk,seq)){
		return;
	}
	if(wk->key_mode == GFL_APP_KTST_KEY){
		word_input_key(wk,seq);
	}else{
		word_input_touch(wk,seq);
	}
}


static int check_wordwin_key( WORDWIN_WORK* wordwin, u16 key )
{
#if 0
	if( key & PAD_KEY_DOWN )
	{
		if( wordwin->cursor_y < WORDWIN_CURSOR_Y_MAX )
		{
			u32  pos;

			wordwin->cursor_y++;
			pos = get_wordwin_pos( wordwin );

			if( pos < wordwin->word_max )
			{
				return WORDWIN_RESULT_CURSOR_MOVE;
			}
			if( pos == wordwin->word_max )
			{
				if( pos & 1 )
				{
					wordwin->cursor_x = 0;
					return WORDWIN_RESULT_CURSOR_MOVE;
				}
			}
			wordwin->cursor_y--;
			return WORDWIN_RESULT_INVALID;
		}
		else if( wordwin->line < wordwin->line_max )
		{
			wordwin->scroll_lines = 1;
			wordwin->line++;
			
			if( get_wordwin_pos( wordwin ) < wordwin->word_max )
			{
				return WORDWIN_RESULT_SCROLL;
			}
			else
			{
				wordwin->cursor_x = 0;
				return WORDWIN_RESULT_SCROLL_AND_CURSOR_MOVE;
			}
		}
		return WORDWIN_RESULT_INVALID;
	}

	if( key & PAD_KEY_UP )
	{
		if( wordwin->cursor_y )
		{
			wordwin->cursor_y--;
			return WORDWIN_RESULT_CURSOR_MOVE;
		}
		else if( wordwin->line )
		{
			wordwin->scroll_lines = -1;
			wordwin->line--;
			return WORDWIN_RESULT_SCROLL;
		}
		return WORDWIN_RESULT_INVALID;
	}
#else
	if( key & PAD_KEY_DOWN )
	{
		if( wordwin->back_f){
			return WORDWIN_RESULT_INVALID;
		}
		if( wordwin->cursor_y < WORDWIN_CURSOR_Y_MAX )
		{
			u32  pos;

			wordwin->cursor_y++;
			pos = get_wordwin_pos( wordwin );

			if( pos < wordwin->word_max )
			{
				return WORDWIN_RESULT_CURSOR_MOVE;
			}
			if( pos == wordwin->word_max )
			{
				if( pos & 1 )
				{
					wordwin->cursor_x = 0;
					return WORDWIN_RESULT_CURSOR_MOVE;
				}
			}
			wordwin->cursor_y--;
			wordwin->back_f = TRUE;
			return WORDWIN_RESULT_CURSOR_MOVE;
	//		return WORDWIN_RESULT_INVALID;
		}
		else if( wordwin->line < wordwin->line_max )
		{
			wordwin->scroll_lines = 1;
			wordwin->line++;
			
			if( get_wordwin_pos( wordwin ) < wordwin->word_max )
			{
				return WORDWIN_RESULT_SCROLL;
			}
			else
			{
				wordwin->cursor_x = 0;
				return WORDWIN_RESULT_SCROLL_AND_CURSOR_MOVE;
			}
		}
		wordwin->back_f = TRUE;
		return WORDWIN_RESULT_CURSOR_MOVE;
//		return WORDWIN_RESULT_INVALID;
	}

	if( key & PAD_KEY_UP )
	{
		if( wordwin->back_f){
			wordwin->back_f = FALSE;
			return WORDWIN_RESULT_CURSOR_MOVE;
		}
		if( wordwin->cursor_y )
		{
			wordwin->cursor_y--;
			return WORDWIN_RESULT_CURSOR_MOVE;
		}
		else if( wordwin->line )
		{
			wordwin->scroll_lines = -1;
			wordwin->line--;
			return WORDWIN_RESULT_SCROLL;
		}
		return WORDWIN_RESULT_INVALID;
	}

#endif
	if( key & (PAD_KEY_LEFT | PAD_KEY_RIGHT ) )
	{
		wordwin->cursor_x ^= 1;
		if( get_wordwin_pos( wordwin ) < wordwin->word_max )
		{
			return WORDWIN_RESULT_CURSOR_MOVE;
		}
		wordwin->cursor_x ^= 1;
		return WORDWIN_RESULT_INVALID;
	}

	if( key & PAD_BUTTON_L )
	{
		return check_wordwin_scroll_up( wordwin );
	}

	if( key & PAD_BUTTON_R )
	{
		return check_wordwin_scroll_down( wordwin );
	}


	return WORDWIN_RESULT_NONE;
}
static int check_wordwin_scroll_up( WORDWIN_WORK* wordwin )
{
	
	if( wordwin->line )
	{
		if( wordwin->line >= WORDWIN_PAGE_LINES )
		{
			wordwin->line -= WORDWIN_PAGE_LINES;
			wordwin->scroll_lines = -WORDWIN_PAGE_LINES;
		}
		else
		{
			wordwin->scroll_lines = -(wordwin->line);
			wordwin->line = 0;
		}
		return WORDWIN_RESULT_SCROLL;
	}
	return WORDWIN_RESULT_INVALID;
}

static int check_wordwin_scroll_down( WORDWIN_WORK* wordwin )
{
	if( wordwin->line < wordwin->line_max )
	{
		if( (wordwin->line + WORDWIN_PAGE_LINES) <= wordwin->line_max )
		{
			wordwin->scroll_lines = WORDWIN_PAGE_LINES;
			wordwin->line += WORDWIN_PAGE_LINES;
		}
		else
		{
			wordwin->scroll_lines = wordwin->line_max - wordwin->line;
			wordwin->line = wordwin->line_max;
		}

		if( get_wordwin_pos( wordwin ) < wordwin->word_max )
		{
			return WORDWIN_RESULT_SCROLL;
		}
		else
		{
			wordwin->cursor_x = 0;
			return WORDWIN_RESULT_SCROLL_AND_CURSOR_MOVE;
		}
	}
	return WORDWIN_RESULT_INVALID;
}


//------------------------------------------------------------------
/**
	* 選択された単語をデータに反映する
	*
	* @param   wk		
	*
	* @retval  BOOL	入力できる欄を全て埋めたらTRUEが返る
	*/
//------------------------------------------------------------------
static BOOL set_select_word( PMS_INPUT_WORK* wk )
{
	u32  word_idx;
	PMS_WORD  word;

	if(wk->word_win.touch_pos != 0xFFFF){
		word_idx = wk->word_win.touch_pos;
	}else{
		word_idx = get_wordwin_pos( &wk->word_win );
	}
	if( wk->category_mode == CATEGORY_MODE_GROUP ){
		word = PMSI_DATA_GetGroupEnableWordCode( wk->dwk, wk->category_pos, word_idx );
	}else{
		word = PMSI_DATA_GetInitialEnableWordCode( wk->dwk, wk->category_pos, word_idx );
	}
	OS_Printf(" WordSet pos=%d, idx = %d, word = %d\n",wk->edit_pos,word_idx,word);
	switch( wk->input_mode ){
	case PMSI_MODE_SINGLE:
		wk->edit_word[0] = word;
		break;

	case PMSI_MODE_DOUBLE:
		wk->edit_word[ wk->edit_pos ] = word;
		break;

	case PMSI_MODE_SENTENCE:
		PMSDAT_SetWord( &wk->edit_pms, wk->edit_pos, word );
		break;
	}

	return check_input_complete( wk );
}

//----------------------------------------------------------------------------------------------
/**
	* メインプロセス：フェードアウト〜終了
	*
	* @param   wk
	* @param   seq		
	*
	*/
//----------------------------------------------------------------------------------------------
static GFL_PROC_RESULT MainProc_Quit( PMS_INPUT_WORK* wk, int* seq )
{
	switch( *seq ){
	case 0:
		PMSIView_SetCommand( wk->vwk, VCMD_QUIT );
		(*seq)++;
		break;

	case 1:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			return GFL_PROC_RES_FINISH;
		}
		break;
	}
	return GFL_PROC_RES_CONTINUE;
}






//------------------------------------------------------------------
/**
	* サブプロセススタート（メイン・サブ両方から呼んで良い）
	*
	* @param   wk		
	* @param   sub_proc
	*
	*/
//------------------------------------------------------------------
static void SetSubProc( PMS_INPUT_WORK* wk, SubProc sub_proc )
{
	wk->sub_proc = sub_proc;
	wk->sub_seq = 0;
}

//------------------------------------------------------------------
/**
	* サブプロセス終了（実行中のサブプロセスからのみ呼び出す）
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
static void QuitSubProc( PMS_INPUT_WORK* wk )
{
	wk->sub_proc = NULL;
}




//------------------------------------------------------------------
/**
	* サブプロセス：フェードイン
	*
	* @param   wk		ワークポインタ
	* @param   seq		シーケンス
	*
	*/
//------------------------------------------------------------------
static void SubProc_FadeIn( PMS_INPUT_WORK* wk, int* seq )
{
	switch(*seq){
	case 0:
		PMSIView_SetCommand( wk->vwk, VCMD_FADEIN );
		(*seq)++;
		break;
	case 1:
		if(PMSIView_WaitCommandAll( wk->vwk ))
		{
			QuitSubProc( wk );
		}
		break;
	}
}
//------------------------------------------------------------------
/**
	* サブプロセス：「けってい」コマンド
	*
	* @param   wk		ワークポインタ
	* @param   seq		シーケンス
	*
	*/
//------------------------------------------------------------------
static void SubProc_CommandOK( PMS_INPUT_WORK* wk, int* seq )
{
	int ret;
	enum {
		SEQ_INIT,
		SEQ_MENU_CTRL,
		SEQ_WAIT_ANYKEY,
		SEQ_RETURN,
	};

	switch(*seq){
	case SEQ_INIT:
		if( CheckModified( wk ) || PMSI_PARAM_GetNotEditEgnoreFlag(wk->input_param) )
		{
			if( check_input_complete( wk ) )
			{
				InitMenuState( &wk->menu, MENU_RESULT_POS_MAX, MENU_RESULT_POS_YES );
        wk->cmd_button_pos = 0;
				PMSIView_SetCommand( wk->vwk, VCMD_TASKMENU_DECIDE );
				(*seq) = SEQ_MENU_CTRL;
			}
			else
			{
        /// @TODO このままでは何もメッセージが出ない
				PMSIView_SetCommand( wk->vwk, VCMD_DISP_MESSAGE_WARN );
				(*seq) = SEQ_WAIT_ANYKEY;
			}
		}
		else
		{
			SetSubProc( wk, SubProc_CommandCancel );
		}
		break;

	case SEQ_MENU_CTRL:
#if 0
		if( PMSIView_WaitCommand(wk->vwk, VCMD_MOVE_MENU_CURSOR) )
		{
			switch( CtrlMenuState(&(wk->menu), wk->key_trg) ){
			case MENU_RESULT_MOVE:
				Snd_SePlay(SOUND_MOVE_CURSOR);

				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_MENU_CURSOR );
				break;

			case MENU_RESULT_POS_NO:
			case MENU_RESULT_CANCEL:
				Snd_SePlay(SOUND_CANCEL);

				PMSIView_SetCommand( wk->vwk, VCMD_ERASE_MENU );
				(*seq) = SEQ_RETURN;
				break;

			case MENU_RESULT_POS_YES:
				Snd_SePlay(SOUND_DECIDE);
				if( wk->input_mode == PMSI_MODE_SENTENCE )
				{
					PMSDAT_ClearUnnecessaryWord( &(wk->edit_pms) );
				}
				PMSI_PARAM_WriteBackData( wk->input_param, wk->edit_word, &wk->edit_pms );
				ChangeMainProc( wk, MainProc_Quit );
				(*seq) = SEQ_RETURN;
				break;
			}
		}
#else
		ret = PMSIView_WaitYesNo(wk->vwk);
		switch(ret){
		case 0:
#if PMS_USE_SND
			Snd_SePlay(SOUND_CANCEL);
#endif //PMS_USE_SND

			PMSIView_SetCommand( wk->vwk, VCMD_ERASE_MENU );
			(*seq) = SEQ_RETURN;
			break;
		case 1:
#if PMS_USE_SND
			Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND
			if( wk->input_mode == PMSI_MODE_SENTENCE )
			{
				PMSDAT_ClearUnnecessaryWord( &(wk->edit_pms) , HEAPID_PMS_INPUT_VIEW );
			}
			PMSI_PARAM_WriteBackData( wk->input_param, wk->edit_word, &wk->edit_pms );
			ChangeMainProc( wk, MainProc_Quit );
			(*seq) = SEQ_RETURN;
			break;
		default:
			break;
		}
#endif
		break;

	case SEQ_WAIT_ANYKEY:
		if( wk->key_trg & (PAD_BUTTON_A|PAD_BUTTON_B|PAD_PLUS_KEY_MASK) )
		{
			PMSIView_SetCommand( wk->vwk, VCMD_DISP_MESSAGE_DEFAULT );
			(*seq) = SEQ_RETURN;
		}
		break;

	case SEQ_RETURN:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			QuitSubProc( wk );
		}
		break;
	}
}

//--------------------------------------------------------------------------------------
/**
	* サブプロセス：「やめる」コマンド
	*
	* @param   wk		ワークポインタ
	* @param   seq		シーケンス
	*
	*/
//--------------------------------------------------------------------------------------
static void SubProc_CommandCancel( PMS_INPUT_WORK* wk, int* seq )
{
	int ret;

	enum {
		SEQ_INIT,
		SEQ_MENU_CTRL,
		SEQ_RETURN,
	};

	switch(*seq){
	case SEQ_INIT:
		InitMenuState( &wk->menu, MENU_RESULT_POS_MAX, MENU_RESULT_POS_NO );
    wk->cmd_button_pos = 1;
		PMSIView_SetCommand( wk->vwk, VCMD_TASKMENU_DECIDE );
//	PMSIView_SetCommand( wk->vwk, VCMD_DISP_MESSAGE_CANCEL );
		(*seq) = SEQ_MENU_CTRL;
		break;

	case SEQ_MENU_CTRL:
		ret = PMSIView_WaitYesNo(wk->vwk);
		switch(ret){
		case 0:
#if PMS_USE_SND
			Snd_SePlay(SOUND_CANCEL);
#endif //PMS_USE_SND

			PMSIView_SetCommand( wk->vwk, VCMD_ERASE_MENU );
			(*seq) = SEQ_RETURN;
			break;
		case 1:
#if PMS_USE_SND
			Snd_SePlay(SOUND_DECIDE);
#endif //PMS_USE_SND
			ChangeMainProc( wk, MainProc_Quit );
			(*seq) = SEQ_RETURN;
			break;
		default:
			break;
		}
		break;

	case SEQ_RETURN:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			QuitSubProc( wk );
		}
		break;
	}
}


//------------------------------------------------------------------
/**
	* 入力欄が初期状態と比べて変更されているかチェック
	*
	* @param   wk			
	*
	* @retval  BOOL		更新されていたらTRUEを返す
	*/
//------------------------------------------------------------------
static BOOL CheckModified( PMS_INPUT_WORK* wk )
{
	return (PMSI_PARAM_CheckModifiedByEditDatas( wk->input_param, wk->edit_word, &wk->edit_pms ) == FALSE);
}

static BOOL check_input_complete( PMS_INPUT_WORK* wk )
{
	switch( wk->input_mode ){
	case PMSI_MODE_SINGLE:
		return (wk->edit_word[0] != PMS_WORD_NULL);

	case PMSI_MODE_DOUBLE:
		return ( (wk->edit_word[0] != PMS_WORD_NULL) && (wk->edit_word[1] != PMS_WORD_NULL) );

	case PMSI_MODE_SENTENCE:
		return PMSDAT_IsComplete( &wk->edit_pms , HEAPID_PMS_INPUT_VIEW );
	}

	return FALSE;
}


//------------------------------------------------------------------
/**
	* メニューコントロール初期化
	*
	* @param   menu		
	* @param   max		
	* @param   pos		
	*
	*/
//------------------------------------------------------------------
static void InitMenuState( MENU_WORK* menu, int max, int pos )
{
	menu->pos = pos;
	menu->max = max;
}

//------------------------------------------------------------------
/**
	* メニューコントロール更新
	*
	* @param   menu		
	* @param   key		
	*
	* @retval  MENU_RESULT		
	*/
//------------------------------------------------------------------
static MENU_RESULT  CtrlMenuState( MENU_WORK* menu , u16 key )
{
	if( key & PAD_KEY_UP )
	{
		if( menu->pos == 0 )
		{
			return MENU_RESULT_DISABLE_KEY;
		}
		else
		{
			menu->pos--;
			return MENU_RESULT_MOVE;
		}
	}
	if( key & PAD_KEY_DOWN )
	{
		if( menu->pos == menu->max )
		{
			return MENU_RESULT_DISABLE_KEY;
		}
		else
		{
			menu->pos++;
			return MENU_RESULT_MOVE;
		}
	}
	if( key & PAD_BUTTON_A )
	{
		return menu->pos;
	}
	if( key & PAD_BUTTON_B )
	{
		return MENU_RESULT_CANCEL;
	}
	return MENU_RESULT_NONE;
}

static u32 get_menu_cursor_pos( const MENU_WORK* menu )
{
	return menu->pos;
}

//--------------------------------------------------------------------------------------
/**
	* サブプロセス： カテゴリモード変更（編集・ボタン操作時のみ）
	*
	* @param   wk		ワークポインタ
	* @param   seq		シーケンス
	*
	*/
//--------------------------------------------------------------------------------------
static void SubProc_ChangeCategoryMode( PMS_INPUT_WORK* wk, int* seq )
{
	switch(*seq ){

	case 0:
#if PMS_USE_SND
		Snd_SePlay(SOUND_CHANGE_CATEGORY);
#endif //PMS_USE_SND
		wk->category_mode ^= 1;
		PMSIView_SetCommand( wk->vwk, VCMD_CHANGE_CATEGORY_MODE_DISABLE );
		(*seq) = 1;
		break;

	case 1:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
			QuitSubProc( wk );
		}
		break;

	}
}

//==============================================================================================
//==============================================================================================

/**
	*	@brief	キーモード保持ポインタの取得
	*/
int* PMSI_GetKTModePointer(const PMS_INPUT_WORK* wk)
{
	return (int*)&(wk->key_mode);
}

//------------------------------------------------------------------
/*int**
	* 入力モード取得
	*
	* @param   wk		ワークポインタ
	*
	* @retval  u32		入力モード（enum PMSI_MODE)
	*/
//------------------------------------------------------------------
u32 PMSI_GetInputMode( const PMS_INPUT_WORK* wk )
{
	return wk->input_mode;
}

//------------------------------------------------------------------
/**
	* 説明文字列タイプ取得
	*
	* @param   wk		ワークポインタ
	*
	* @retval  u32		説明文字列タイプ（enum PMSI_GUIDANCE)
	*/
//------------------------------------------------------------------
u32 PMSI_GetGuidanceType( const PMS_INPUT_WORK* wk )
{
	return PMSI_PARAM_GetGuidanceType( wk->input_param );
}

//------------------------------------------------------------------
/**
	* カテゴリモード取得
	*
	* @param   wk		ワークポインタ
	*
	* @retval  u32		カテゴリモード
	*/
//------------------------------------------------------------------
u32 PMSI_GetCategoryMode( const PMS_INPUT_WORK* wk )
{
	return wk->category_mode;
}

//------------------------------------------------------------------
/**
	* 表示中の文章タイプを取得
	*
	* @param   wk		ワークポインタ
	*
	* @retval  u32		文章タイプ（enum PMS_TYPE）
	*/
//------------------------------------------------------------------
u32 PMSI_GetSentenceType( const PMS_INPUT_WORK* wk )
{
	return PMSDAT_GetSentenceType( &wk->edit_pms );
}

//------------------------------------------------------------------
/**
	* 編集中の単語ナンバーを返す
	*
	* @param   wk			ワークポインタ
	* @param   pos			何番目の単語？
	*
	* @retval  PMS_WORD	簡易会話単語ナンバー
	*/
//------------------------------------------------------------------
PMS_WORD  PMSI_GetEditWord( const PMS_INPUT_WORK* wk, int pos )
{
	if( wk->input_mode == PMSI_MODE_SENTENCE )
	{
		return PMSDAT_GetWordNumber( &wk->edit_pms, pos );
	}
	else
	{
		return wk->edit_word[ pos ];
	}
}

//------------------------------------------------------------------
/**
	* 
	*
	* @param   wk			
	* @param   heapID		
	*
	* @retval  STRBUF*		
	*/
//------------------------------------------------------------------
STRBUF* PMSI_GetEditSourceString( const PMS_INPUT_WORK* wk, u32 heapID )
{
	return PMSDAT_GetSourceString( &(wk->edit_pms), heapID );
}

//------------------------------------------------------------------
/**
	* 編集エリアのカーソル位置を返す
	*
	* @param   wk		
	*
	* @retval  u32		
	*/
//------------------------------------------------------------------
u32 PMSI_GetEditAreaCursorPos( const PMS_INPUT_WORK* wk )
{
	return wk->edit_pos;
}

//------------------------------------------------------------------
/**
	* ボタンカーソル位置取得
	*
	* @param   wk		
	*
	* @retval  u32		
	*/
//------------------------------------------------------------------
u32 PMSI_GetButtonCursorPos( const PMS_INPUT_WORK* wk )
{
	return wk->cmd_button_pos;
}

//------------------------------------------------------------------
/**
	* カテゴリカーソル位置取得
	*
	* @param   wk		
	*
	* @retval  u32		
	*/
//------------------------------------------------------------------
u32 PMSI_GetCategoryCursorPos( const PMS_INPUT_WORK* wk )
{
	return wk->category_pos;
}



//------------------------------------------------------------------
/**
	* カテゴリ内の有効単語総数を返す
	*
	* @param   wk		
	*
	* @retval  u32		
	*/
//------------------------------------------------------------------
u32 PMSI_GetCategoryWordMax( const PMS_INPUT_WORK* wk )
{
	if( wk->category_mode == CATEGORY_MODE_GROUP )
	{
		return PMSI_DATA_GetGroupEnableWordCount( wk->dwk, wk->category_pos );
	}
	else
	{
		return PMSI_DATA_GetInitialEnableWordCount( wk->dwk, wk->category_pos );
	}
}

//------------------------------------------------------------------
/**
	* カテゴリ内の有効単語の内、指定インデックスの単語をバッファにコピーする
	*
	* @param   wk		
	* @param   word_num		
	* @param   buf		
	*
	*/
//------------------------------------------------------------------
void PMSI_GetCategoryWord( const PMS_INPUT_WORK* wk, u32 word_num, STRBUF* buf )
{
	if( wk->category_mode == CATEGORY_MODE_GROUP )
	{
		PMSI_DATA_GetGroupEnableWord( wk->dwk, wk->category_pos, word_num, buf );
	}
	else
	{
		PMSI_DATA_GetInitialEnableWord( wk->dwk, wk->category_pos, word_num, buf );
	}
}

u32 PMSI_GetWordWinCursorPos( const PMS_INPUT_WORK* wk )
{
	if(wk->word_win.back_f){
		return 0xFFFFFFFF;
	}
	return get_wordwin_cursorpos( &(wk->word_win) );
}

int PMSI_GetWordWinScrollVector( const PMS_INPUT_WORK* wk )
{
	return get_wordwin_scroll_vector( &(wk->word_win) );
}
BOOL PMSI_GetWordWinUpArrowVisibleFlag( const PMS_INPUT_WORK* wk )
{
	return (get_wordwin_linepos(&(wk->word_win)) != 0);
}
BOOL PMSI_GetWordWinDownArrowVisibleFlag( const PMS_INPUT_WORK* wk )
{
	int  line, line_max;

	line = get_wordwin_linepos(&(wk->word_win));
	line_max = get_wordwin_linemax(&(wk->word_win));

	return ( line < line_max );
}

int PMSI_GetTalkWindowType( const PMS_INPUT_WORK* wk )
{
	return PMSI_PARAM_GetWindowType( wk->input_param );
}

//------------------------------------------------------------------
/**
	* メニューカーソルの位置を返す
	*
	* @param   wk		
	*
	* @retval  u32		
	*/
//------------------------------------------------------------------
u32 PMSI_GetMenuCursorPos( const PMS_INPUT_WORK* wk )
{
	return get_menu_cursor_pos( &wk->menu );
}


GFL_TCBSYS* PMSI_GetTcbSystem( const PMS_INPUT_WORK* wk )
{
	return wk->tcbSys;
}

//----------------------------------------------------------------------------
/**
 *	@brief	仮文字をデコメに変換する関数
 *					@todo 後ほど消してください
 *
 *	@param	PMS_WORD word 
 *
 *	@return
 */
//-----------------------------------------------------------------------------
static PMS_WORD PMSI_WordToDeco( PMS_WORD word )
{	
	// きもち
	static const s16 deco_tbl[ PMS_DECOID_MAX ] = {
		-1,	//1オリジン
		1480,	//はじめまして
		1481,	//こんにちは
		1482,	//だいすき
		1484,	//たのしい
		1483,	//がんばれ！
		1485,	//うれしい
		1486,	//ありがとう
		1487,	//さいこう
		1488,	//ごめんね
		1489,	//バイバイ
	};

	//仮に、選ばれた文字列をデコメ文字に変換する
	
	u32	word_bit;
	u32	deco_bit;
	u32 local_bit;
	

	//ビットを取り出す
	word_bit	= word &  PMS_WORD_NUM_MASK;
	deco_bit	= word &  (PMS_WORD_DECO_MASK << PMS_WORD_DECO_BITSHIFT);
	local_bit	= word &  (PMS_WORD_LOCALIZE_MASK << PMS_WORD_LOCALIZE_BITSHIFT);

	OS_Printf( "check word %d num %d deco %d local %d\n", word, word_bit, deco_bit, local_bit );
	{	
		int i;
		for( i = 0; i < PMS_DECOID_MAX; i++ )
		{	
			//でこ用文字なので変換
			if( deco_tbl[i] == word_bit )
			{	
				word_bit	= i;	//デコの番号
				deco_bit	= 1 << PMS_WORD_DECO_BITSHIFT;

				word	= local_bit | deco_bit | word_bit;
				break;
			}
		}
	}
	
	return word;
}


