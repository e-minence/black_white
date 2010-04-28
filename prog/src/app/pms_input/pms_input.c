//============================================================================================
/**
	* @file	pms_input.c
	* @bfief	�ȈՉ�b���͉��
	* @author	taya (DP)
  * @author	iwasawa����  (GS)
  * @author ariizumi���� (WB�ڐA)
  * @author	genya hosaka (WB)
	* @date	06.01.20
	*/
//============================================================================================
#include <gflib.h>

#include "system\main.h"
#include "savedata/save_control.h"

#include "system\pms_data.h"
#include "print\printsys.h"
#include "pmsi_sound_def.h"
#include "pms_input_prv.h"
#include "pmsi_initial_data.h"
#include "pms_input_view.h"

#include "pmsi_search.h"

#include "test/ariizumi/ari_debug.h"

FS_EXTERN_OVERLAY(ui_common);

//------------------------------------------------------
/**
	* �萔��`
	*/
//------------------------------------------------------
enum {
	HEAPSIZE_SYS = 0x18000,
	HEAPSIZE_VIEW = 0x32000,
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
	WORDWIN_CURSOR_Y_MAX = 5,
	WORDWIN_DISP_MAX = 12,

	WORDWIN_PAGE_LINES = WORDWIN_CURSOR_Y_MAX+1,
};

enum TOUCH_BUTTON {
	TOUCH_BUTTON_CHANGE, ///< �J�e�S�����[�h�ؑ�

	TOUCH_BUTTON_NULL,

  TOUCH_BUTTON_MAX,
};

enum{
  // �G�f�B�b�g��� �����Ă��E��߂�
	TPED_DCBTN_PX = 23*8,
	TPED_DCBTN_PY0 = 18*8,  // �����Ă� Y
	TPED_DCBTN_PY1 = 21*8,  // ��߂� Y
	TPED_DCBTN_SX = 9*8,
	TPED_DCBTN_SY = 3*8,

  // �G�f�B�b�g��� �P�ꃂ�[�h�̒P��
	TPED_WORD1_PX = 10*8,
	TPED_WORD_PY = 2*8,
	TPED_WORD_SX = 12*8,
	TPED_WORD_SY = 2*8,
	TPED_WORD2_PX0 = 3*8,
	TPED_WORD2_PX1 = 17*8,

  // �G�f�B�b�g��� ���E�{�^��
	TPED_SBTN_SX = 3*8,
	TPED_SBTN_SY = 3*8,
	TPED_SBTN_PX0 = 0,
	TPED_SBTN_PX1 = TPED_SBTN_PX0 + TPED_SBTN_SX * 6,
	TPED_SBTN_PY = 21*8,

  // �G�f�B�b�g��ʂ̃G�f�B�b�g�G���A�̍��E���
	TPED_ARROW_SX   =  20,
	TPED_ARROW_SY   =  32,
	TPED_ARROW_PX_L =   0,
	TPED_ARROW_PX_R = 256 - TPED_ARROW_SX,
	TPED_ARROW_PY   =   8,

	TPCA_RET_PX = 29*8,
	TPCA_RET_PY = 21*8,
	TPCA_RET_SX = 3*8,
	TPCA_RET_SY = 3*8,

	TPCA_MODE_PX = 0*8,  ///< �J�e�S���ؑփ{�^��
	TPCA_MODE_PY = 8*21,
	TPCA_MODE_SX = 8*5,
	TPCA_MODE_SY = 8*3,

	TPCA_GMA_SX = 9*8,
	TPCA_GMA_SY = 2*8,
	TPCA_GMA_PX = 1*8,
	TPCA_GMA_PY = 9*8,
	TPCA_GMA_OX = TPCA_GMA_SX+1*8,
	TPCA_GMA_OY = TPCA_GMA_SY+1*8,

	TPCA_IMA_SX = 16,   // �h�b�g�P��
	TPCA_IMA_SY = 16,   // �h�b�g�P��
	TPCA_IMA_PX = 1*8, //2*8-GROUPMODE_BG_XOFS,
	TPCA_IMA_PY = 8*8 - CATEGORY_BG_ENABLE_YOFS, //10*8-CATEGORY_BG_ENABLE_YOFS,

	TPWD_SCR_PX = 28*8+4,
	TPWD_SCR_SX = 24,
	TPWD_SCR_SY = 7*8,
	TPWD_SCR_PY0 = 6*8,
	TPWD_SCR_PY1 = TPWD_SCR_PY0+TPWD_SCR_SY,

	TPWD_WORD_PX = 4*8,
	TPWD_WORD_PY = 2*8,
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
	SEQ_CA_NEXTPROC2,
	SEQ_CA_WAIT_MODE_CHANGE,
};

enum {
	SEQ_WORD_KEYWAIT,
	SEQ_WORD_SCROLL_WAIT,
	SEQ_WORD_SCROLL_WAIT_AND_CURSOR_MOVE,
	SEQ_WORD_SCROLL_BAR_WAIT,
	SEQ_WORD_CHANGE_NEXTPROC,
};

// �^�X�N��
#define PMSI_TASK_MAX (5)


//------------------------------------------------------
/**
	* �֐��^��`
	*/
//------------------------------------------------------
typedef  void (*SubProc)(PMS_INPUT_WORK*, int*);
typedef  GFL_PROC_RESULT (*MainProc)(PMS_INPUT_WORK*, int* );
typedef	void (*KTSTChangeProc)(PMS_INPUT_WORK*,int* seq);

//------------------------------------------------------
/**
	* ���j���[���샏�[�N
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
	* ���[�N�̈��`
	*/
//------------------------------------------------------
struct _PMS_INPUT_WORK{

	PMSI_PARAM* input_param;
	u32         input_mode;

	PMS_DATA    edit_pms;
	PMS_WORD    edit_word[ PMS_INPUT_WORD_MAX ];

	PMS_INPUT_VIEW*   vwk;
	PMS_INPUT_DATA*   dwk;
  PMS_INPUT_SEARCH* swk;

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

	u16        cmd_button_pos;
	u16        edit_pos;
	u16        category_pos;
	u16        category_pos_prv;

	WORDWIN_WORK   word_win;

	SENTENCE_WORK  sentence_wk;

	u8         sentence_edit_pos_max;
	u8         category_mode;

	u8	scroll_bar_flg;		// �X�N���[���o�[�^�b�`�ς݃t���O

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
static void sentence_change_type( SENTENCE_WORK* s_wk, PMS_DATA* pms, enum PMS_TYPE type );
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
static int check_wordwin_scroll_up_pos( PMS_INPUT_VIEW* vwk, WORDWIN_WORK* wordwin );
static int check_wordwin_scroll_down_pos( PMS_INPUT_VIEW* vwk, WORDWIN_WORK* wordwin );
static int check_wordwin_scroll_pos( PMS_INPUT_VIEW* vwk, WORDWIN_WORK* wordwin );
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
	* GFL_PROC������
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
	
    //�I�[�o�[���C�ǂݍ���
	  GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));

		GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PMS_INPUT_SYSTEM, HEAPSIZE_SYS );
		GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_PMS_INPUT_VIEW, HEAPSIZE_VIEW );
		wk = ConstructWork( proc , pwk );
		PMSIView_SetCommand( wk->vwk, VCMD_INIT );

    // �����ŒʐM�A�C�R����ǂ�ł��\������Ȃ������B
    
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

      // �ŏ���EditArea����n�܂�̂ŁA���̃L�[/�^�b�`�ؑւ��s���Ă���
	    PMSIView_SetCommand( wk->vwk, VCMD_KTCHANGE_EDITAREA);
      (*seq)++;
    }
    break;
  case 2:
    wk = mywk;
		if( PMSIView_WaitCommandAll( wk->vwk ) )
    {
	    ChangeMainProc(wk, MainProc_EditArea);  // �����Ń`�F���W����
		
      // �ʐM�A�C�R��
      GFL_NET_WirelessIconEasy_HoldLCD( TRUE, HEAPID_PMS_INPUT_VIEW );
      GFL_NET_ReloadIcon();
      
      return GFL_PROC_RES_FINISH;
		}
		break;
	}

	GFL_TCB_Main(wk->tcbSys);	//�������Ŏg���Ă邩��R�R�ł܂킷
	return GFL_PROC_RES_CONTINUE;
}



//------------------------------------------------------------------
/**
	* GFL_PROC ���C��
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
	* �{�^���Ǘ��}�l�[�W���R�[���o�b�N
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
		break;

	case GFL_BMN_EVENT_RELEASE:
	case GFL_BMN_EVENT_SLIDEOUT:
		wk->touch_button = TOUCH_BUTTON_NULL;
		break;

	case GFL_BMN_EVENT_HOLD:
		break;

	default:
		wk->touch_button = TOUCH_BUTTON_NULL;
		break;
	}
}

//------------------------------------------------------------------
/**
	* GFL_PROC �I��
	*
	* @param   proc		
	* @param   seq		
	*
	* @retval  GFL_PROC_RESULT		
	*/
//------------------------------------------------------------------
GFL_PROC_RESULT PMSInput_Quit( GFL_PROC * proc, int * seq , void *pwk, void *mywk )
{
	DestructWork( mywk, proc );

  // �ʐM�A�C�R��
  GFL_NET_WirelessIconEasy_DefaultLCD();

	GFL_HEAP_DeleteHeap( HEAPID_PMS_INPUT_SYSTEM );
	GFL_HEAP_DeleteHeap( HEAPID_PMS_INPUT_VIEW );

	//�I�[�o�[���C�j��
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

	return GFL_PROC_RES_FINISH;
}
//------------------------------------------------------------------
/**
	* ���C�����[�N�\�z
	*
	* @param   wk		
	* @param   proc		
	*
	*/
//------------------------------------------------------------------
static PMS_INPUT_WORK* ConstructWork( GFL_PROC* proc , void* pwk )
{
	// enum TOUCH_BUTTON �ƈ�v������
	static const GFL_UI_TP_HITTBL hit_tbl[TOUCH_BUTTON_MAX] = {
		{ BUTTON_CHANGE_TOUCH_TOP, BUTTON_CHANGE_TOUCH_BOTTOM, BUTTON_CHANGE_TOUCH_LEFT, BUTTON_CHANGE_TOUCH_RIGHT },
		{ GFL_UI_TP_HIT_END, 0, 0, 0 },
	};

	PMS_INPUT_WORK* wk;

	wk = GFL_PROC_AllocWork( proc, sizeof(PMS_INPUT_WORK), HEAPID_PMS_INPUT_SYSTEM );
	wk->input_param = pwk;
	wk->input_mode = PMSI_PARAM_GetInputMode( wk->input_param );

	//�L�[or�^�b�`�X�e�[�^�X�擾
	wk->key_mode = PMSI_PARAM_GetKTStatus(wk->input_param);

	PMSI_PARAM_GetInitializeData( wk->input_param, wk->edit_word, &wk->edit_pms );

	if( wk->input_mode == PMSI_MODE_SENTENCE )
	{
		setup_sentence_work( &wk->sentence_wk, &wk->edit_pms );
	}

	wk->tcbWork = GFL_HEAP_AllocMemory( HEAPID_PMS_INPUT_SYSTEM , GFL_TCB_CalcSystemWorkSize( PMSI_TASK_MAX ) );
	wk->tcbSys = GFL_TCB_Init( PMSI_TASK_MAX , wk->tcbWork );

	wk->dwk = PMSI_DATA_Create( HEAPID_PMS_INPUT_SYSTEM, wk->input_param );
	wk->vwk = PMSIView_Create(wk, wk->dwk);
	wk->bmn = GFL_BMN_Create( hit_tbl, BmnCallBack, wk, HEAPID_PMS_INPUT_SYSTEM );

	wk->category_mode = CATEGORY_MODE_GROUP;
	wk->sub_proc = NULL;
	wk->sub_seq = 0;
	wk->edit_pos = 0;
	wk->cmd_button_pos = 0;

	wk->touch_button = TOUCH_BUTTON_NULL;
  
  wk->swk = PMSI_SEARCH_Create( wk, wk->dwk, HEAPID_PMS_INPUT_SYSTEM );

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
		if( s_wk->sentence_type >= PMS_TYPE_USER_MAX )
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
			s_wk->sentence_type = (PMS_TYPE_USER_MAX - 1);
		}
		s_wk->sentence_id_max = PMSDAT_GetSentenceIdMax( s_wk->sentence_type );
		s_wk->sentence_id = (s_wk->sentence_id_max - 1);
	}

	PMSDAT_SetSentence( pms, s_wk->sentence_type, s_wk->sentence_id );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �^�b�`�ɂ��TYPE�`�F���W
 *
 *	@param	SENTENCE_WORK* s_wk
 *	@param	pms
 *	@param	type 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void sentence_change_type( SENTENCE_WORK* s_wk, PMS_DATA* pms, enum PMS_TYPE type )
{
  GF_ASSERT( type < PMS_TYPE_USER_MAX );

	s_wk->sentence_id = 0;
  s_wk->sentence_type = type;
	s_wk->sentence_id_max = PMSDAT_GetSentenceIdMax( s_wk->sentence_type );

	PMSDAT_SetSentence( pms, s_wk->sentence_type, s_wk->sentence_id );
}

//------------------------------------------------------------------
/**
	* ���C�����[�N�j��
	*
	* @param   wk		
	*
	*/
//------------------------------------------------------------------
static void DestructWork( PMS_INPUT_WORK* wk, GFL_PROC* proc )
{
	GFL_TCB_Exit( wk->tcbSys );
	GFL_HEAP_FreeMemory( wk->tcbWork );
	
	//�L�[or�^�b�`�X�e�[�^�X���f
	PMSI_PARAM_SetKTStatus(wk->input_param,wk->key_mode);
	
	GFL_BMN_Delete( wk->bmn );
	PMSIView_Delete( wk->vwk );
	PMSI_DATA_Delete( wk->dwk );
  PMSI_SEARCH_Delete( wk->swk );
	GFL_PROC_FreeWork( proc );
}



static void ChangeMainProc( PMS_INPUT_WORK* wk, MainProc main_proc )
{
	wk->main_proc = main_proc;
	wk->main_seq = 0;

	if(main_proc == MainProc_EditArea){
    PMSIView_ChangeKTEditArea( wk->vwk, wk, wk->dwk );
		wk->cb_ktchg_func = CB_EditArea_KTChange;
	}else if(main_proc == MainProc_Category){
    PMSIView_ChangeKTCategory( wk->vwk, wk, wk->dwk );
		wk->cb_ktchg_func = CB_Category_KTChange;
	}else if(main_proc == MainProc_WordWin){
    PMSIView_ChangeKTWordWin( wk->vwk, wk, wk->dwk );
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
 *	@brief	�L�[/�^�b�`�̐ؑ�
 */
static int KeyStatusChange(PMS_INPUT_WORK* wk,int* seq)
{
	if(wk->key_mode == GFL_APP_KTST_TOUCH){
		if(GFL_UI_TP_GetCont() != 0){	//
			return 0;
		}
		if(GFL_UI_KEY_GetCont() != 0){
			wk->key_mode = GFL_APP_KTST_KEY;
			if(wk->cb_ktchg_func != NULL){
				(wk->cb_ktchg_func)(wk,seq);
			}
			return 1;
		}
	}else{
		if(GFL_UI_KEY_GetCont() != 0){
			return 0;
		}
		if(GFL_UI_TP_GetCont() != 0){
			wk->key_mode = GFL_APP_KTST_TOUCH;
			if(wk->cb_ktchg_func != NULL){
				(wk->cb_ktchg_func)(wk,seq);
			}
			return 1;
		}
	}
	return 0;

}


//-----------------------------------------------------------------------------
/**
 *	@brief  �P�ꃊ�X�g��
 *
 *	@param	PMS_INPUT_WORK* wk
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static void set_nextproc_category_to_wordwin( PMS_INPUT_WORK* wk )
{
  PMSND_PlaySE(SOUND_DECIDE);
  
  setup_wordwin_params( &wk->word_win, wk );

  wk->next_proc = MainProc_WordWin;
  PMSIView_SetCommand( wk->vwk, VCMD_CATEGORY_TO_WORDWIN );
}


//----------------------------------------------------------------------------------------------
/**
	* ���C���v���Z�X�F���͗��ł̓���
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
	*	@brief	EditArea �L�[/�^�b�`�ؑ�
	*/
static void CB_EditArea_KTChange(PMS_INPUT_WORK* wk,int* seq)
{
  PMSIView_ChangeKTEditArea( wk->vwk, wk, wk->dwk );
}

/**
	*	@brief	Category �L�[/�^�b�`�ؑ�
	*/
static void CB_Category_KTChange(PMS_INPUT_WORK* wk,int* seq)
{	
  PMSIView_ChangeKTCategory( wk->vwk, wk, wk->dwk );
}

/**
	*	@brief	WordWin �L�[/�^�b�`�ؑ�
	*/
static void CB_WordWin_KTChange(PMS_INPUT_WORK* wk,int* seq)
{	
  PMSIView_ChangeKTWordWin( wk->vwk, wk, wk->dwk );
}

//------------------------------------------------------------------
/**
	* �P��P���̓��[�h
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
			PMSND_PlaySE(SOUND_MOVE_CURSOR);

			wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_BUTTON );
			(*seq) = SEQ_EDW_BUTTON_KEYWAIT;
			break;
		}
		if( wk->key_trg & PAD_BUTTON_B )
		{
			PMSND_PlaySE(SOUND_CANCEL);

			SetSubProc( wk, SubProc_CommandCancel );
			break;
		}
		if( wk->key_trg & PAD_BUTTON_A )
		{
			PMSND_PlaySE(SOUND_DECIDE);

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
			PMSND_PlaySE(SOUND_MOVE_CURSOR);

			if(wk->cmd_button_pos){
        // �u��߂�v����u�����Ă��v��
				wk->cmd_button_pos ^= 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			}else{
        // �u�����Ă��v����G�f�B�b�g�G���A��
				PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
				(*seq) = SEQ_EDW_KEYWAIT;
			}
			break;
		}else if( wk->key_trg & (PAD_KEY_DOWN) ){
			PMSND_PlaySE(SOUND_MOVE_CURSOR);
			
      if(wk->cmd_button_pos){
        // �u��߂�v����G�f�B�b�g�G���A��
				PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
				(*seq) = SEQ_EDW_KEYWAIT;
			}else{
        // �u�����Ă��v����u��߂�v��
				wk->cmd_button_pos ^= 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			}
			break;
		}

		if( wk->key_trg & PAD_BUTTON_START ){
			PMSND_PlaySE(SOUND_MOVE_CURSOR);
			
      wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			break;
		}
		if( wk->key_trg & PAD_BUTTON_A ){
			PMSND_PlaySE(SOUND_DECIDE);

			if( wk->cmd_button_pos == BUTTON_POS_DECIDE ){
				(*seq) = SEQ_EDW_TO_SUBPROC_OK;
				break;
			}else{
				(*seq) = SEQ_EDW_TO_SUBPROC_CANCEL;
					break;
				}
			}

			if( wk->key_trg & PAD_BUTTON_B ){
				PMSND_PlaySE(SOUND_DECIDE);
				
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


//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	wk
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static int edit_single_touch(PMS_INPUT_WORK* wk)
{
	int ret;
	
	static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
		{TPED_DCBTN_PY0,TPED_DCBTN_PY0+TPED_DCBTN_SY,TPED_DCBTN_PX,TPED_DCBTN_PX+TPED_DCBTN_SX},
		{TPED_DCBTN_PY1,TPED_DCBTN_PY1+TPED_DCBTN_SY,TPED_DCBTN_PX,TPED_DCBTN_PX+TPED_DCBTN_SX},
		{TPED_WORD_PY,TPED_WORD_PY+TPED_WORD_SY,TPED_WORD1_PX,TPED_WORD1_PX+TPED_WORD_SX},
		{GFL_UI_TP_HIT_END,0,0,0}
	};
	ret = GFL_UI_TP_HitTrg(Btn_TpRect);
	return ret;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	wk
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static int edit_double_touch(PMS_INPUT_WORK* wk)
{
	int ret;

	static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
		{TPED_DCBTN_PY0,TPED_DCBTN_PY0+TPED_DCBTN_SY,TPED_DCBTN_PX,TPED_DCBTN_PX+TPED_DCBTN_SX},
		{TPED_DCBTN_PY1,TPED_DCBTN_PY1+TPED_DCBTN_SY,TPED_DCBTN_PX,TPED_DCBTN_PX+TPED_DCBTN_SX},
		{TPED_WORD_PY,TPED_WORD_PY+TPED_WORD_SY,TPED_WORD2_PX0,TPED_WORD2_PX0+TPED_WORD_SX},
		{TPED_WORD_PY,TPED_WORD_PY+TPED_WORD_SY,TPED_WORD2_PX1,TPED_WORD2_PX1+TPED_WORD_SX},
		{GFL_UI_TP_HIT_END,0,0,0}
	};
	ret = GFL_UI_TP_HitTrg(Btn_TpRect);
	return ret;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	wk
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
#define  edit_sentence_touch_Btn_TpRect_num  (6)
static int edit_sentence_touch(PMS_INPUT_WORK* wk)
{
	int ret,num,i;
	u32 tpx,tpy;
	GFL_UI_TP_HITTBL tbl[2] = { {GFL_UI_TP_HIT_END,0,0,0}, {GFL_UI_TP_HIT_END,0,0,0} };

	static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
		{TPED_DCBTN_PY0,TPED_DCBTN_PY0+TPED_DCBTN_SY,TPED_DCBTN_PX,TPED_DCBTN_PX+TPED_DCBTN_SX}, // �����Ă�
		{TPED_DCBTN_PY1,TPED_DCBTN_PY1+TPED_DCBTN_SY,TPED_DCBTN_PX,TPED_DCBTN_PX+TPED_DCBTN_SX}, // ��߂�
		{TPED_SBTN_PY,TPED_SBTN_PY+TPED_SBTN_SY,TPED_SBTN_PX0,TPED_SBTN_PX0+TPED_SBTN_SX},  // ��
		{TPED_SBTN_PY,TPED_SBTN_PY+TPED_SBTN_SY,TPED_SBTN_PX1,TPED_SBTN_PX1+TPED_SBTN_SX},  // �E

    {TPED_ARROW_PY,TPED_ARROW_PY+TPED_ARROW_SY-1,TPED_ARROW_PX_L,TPED_ARROW_PX_L+TPED_ARROW_SX-1},  // �G�f�B�b�g��ʂ̃G�f�B�b�g�G���A�̍����
		{TPED_ARROW_PY,TPED_ARROW_PY+TPED_ARROW_SY-1,TPED_ARROW_PX_R,TPED_ARROW_PX_R+TPED_ARROW_SX-1},  // �G�f�B�b�g��ʂ̃G�f�B�b�g�G���A�̉E���
   
    // �����܂ł�edit_sentence_touch_Btn_TpRect_num��

    {GFL_UI_TP_HIT_END,0,0,0}
	};

  if(GFL_UI_TP_GetTrg() == 0)
  {
		return GFL_UI_TP_HIT_NONE;
	}

	ret = GFL_UI_TP_HitTrg(Btn_TpRect);

	if(ret != GFL_UI_TP_HIT_NONE)
  {
     // ���͌Œ胂�[�h�͍��E����
    if( PMSI_PARAM_GetLockFlag( wk->input_param ) )
    {
      if( ret == 2 || ret == 3 || ret == 4 || ret == 5 )
      {
        HOSAKA_Printf("lock mode scroll failed! \n");
        return GFL_UI_TP_HIT_NONE;
      }
      else
      {
        return ret;
      }
    }
    else
    {
		  return ret;
    }
	}
    
  // �G�f�B�b�g�G���A�P��̂����蔻��
	num = PMSIView_GetSentenceEditPosMax( wk->vwk );
	
  GFL_UI_TP_GetPointTrg( &tpx,&tpy );
	for(i = 0;i < num;i++){
		PMSIView_GetSentenceWordArea( wk->vwk ,&tbl[0],i);
		if(GFL_UI_TP_HitSelf( tbl, tpx , tpy ) != GFL_UI_TP_HIT_NONE ){
			return i+edit_sentence_touch_Btn_TpRect_num;
		}

	}
	return GFL_UI_TP_HIT_NONE;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �Z���e���X�ύX�{�^��
 *
 *	@param	PMS_INPUT_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static int edit_sentence_check_flipbtn( PMS_INPUT_WORK* wk )
{
	u32 tpx,tpy;

  // ���͌Œ胂�[�h�̓Z���e���X(�󗓂ɒP��𓖂Ă͂߂邱�Ƃ��ł��錊�J������)��ύX�ł��Ȃ�
  if( PMSI_PARAM_GetLockFlag( wk->input_param ) ) return -1;

	if( GFL_UI_TP_GetPointTrg( &tpx, &tpy ) == FALSE )
  {
    return -1; 
  }

  if( tpy > 21 * 8 )
  {
    int idx;

    idx = tpx / TPED_SBTN_SX;
    idx -= 1;
      
    if( idx >= 0 && idx < PMS_TYPE_USER_MAX )
    {
      HOSAKA_Printf("tp flipbtn=%d \n", idx);
      return idx;
    }
  }

  return -1;
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	wk
 *	@param	*seq 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
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
		case 0:	//����
		case 1:	//��߂�
			PMSND_PlaySE(SOUND_DECIDE);
			*seq = SEQ_EDW_TO_SUBPROC_OK+ret;
			break;
		case 2:	//�P��I��
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
	* �P��Q���̓��[�h
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
			PMSND_PlaySE(SOUND_MOVE_CURSOR);

			wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_BUTTON );
			(*seq) = SEQ_EDW_BUTTON_KEYWAIT;
			break;
		}
		if( wk->key_trg & PAD_KEY_LEFT )
		{
			if( wk->edit_pos != 0 )
			{
				PMSND_PlaySE(SOUND_MOVE_CURSOR);
				
        wk->edit_pos = 0;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_EDITAREA_CURSOR );
			}
			break;
		}
		if( wk->key_trg & PAD_KEY_RIGHT )
		{
			if( wk->edit_pos == 0 )
			{
				PMSND_PlaySE(SOUND_MOVE_CURSOR);
				
        wk->edit_pos = 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_EDITAREA_CURSOR );
			}
			break;
		}
		if( wk->key_trg & PAD_BUTTON_B )
		{
			PMSND_PlaySE(SOUND_CANCEL);

			SetSubProc( wk, SubProc_CommandCancel );
			break;
		}
		if( wk->key_trg & PAD_BUTTON_A )
		{
			PMSND_PlaySE(SOUND_DECIDE);

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
			PMSND_PlaySE(SOUND_MOVE_CURSOR);
			
      if(wk->cmd_button_pos){
        // �u��߂�v����u�����Ă��v��
				wk->cmd_button_pos ^= 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			}else{
        // �u�����Ă��v����G�f�B�b�g�G���A��
				PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
				(*seq) = SEQ_EDW_KEYWAIT;
			}
			break;
		}else if( wk->key_trg & (PAD_KEY_DOWN) ){
			PMSND_PlaySE(SOUND_MOVE_CURSOR);
			
      if(wk->cmd_button_pos){
        // �u��߂�v����G�f�B�b�g�G���A��
				PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
				(*seq) = SEQ_EDW_KEYWAIT;
			}else{
        // �u�����Ă��v����u��߂�v��
				wk->cmd_button_pos ^= 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			}
			break;
		}

		if( wk->key_trg & PAD_BUTTON_START ){
			PMSND_PlaySE(SOUND_MOVE_CURSOR);
			
      wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			break;
		}

		if( wk->key_trg & PAD_BUTTON_A ){
			PMSND_PlaySE(SOUND_DECIDE);

			if( wk->cmd_button_pos == BUTTON_POS_DECIDE ){
				(*seq) = SEQ_EDW_TO_SUBPROC_OK;
				break;
			}else{
				(*seq) = SEQ_EDW_TO_SUBPROC_CANCEL;
					break;
				}
			}

			if( wk->key_trg & PAD_BUTTON_B ){
				PMSND_PlaySE(SOUND_DECIDE);
				
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
	* ���͓��̓��[�h
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
    // ���͌Œ胂�[�h�͍��E�𖳌���
    if( PMSI_PARAM_GetLockFlag( wk->input_param ) )
    {
      if( (wk->key_repeat & PAD_KEY_LEFT) || ( wk->key_repeat & PAD_KEY_RIGHT ) )
      {
        break;
      }
    }

		if( wk->key_trg & PAD_KEY_UP )
		{
			PMSND_PlaySE(SOUND_MOVE_CURSOR);

      if( wk->edit_pos == 0 )
      {
        // ��荞��
				wk->cmd_button_pos = BUTTON_POS_CANCEL;
				PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_BUTTON );
				(*seq) = SEQ_EDS_BUTTON_KEYWAIT;
      }
			else if( wk->sentence_edit_pos_max > 1 && wk->edit_pos > 0 )
			{
				wk->edit_pos--;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_EDITAREA_CURSOR );
			}
			break;
		}
		if( wk->key_trg & PAD_KEY_DOWN )
		{
			PMSND_PlaySE(SOUND_MOVE_CURSOR);

			if( wk->sentence_edit_pos_max > 1 && wk->edit_pos < wk->sentence_edit_pos_max -1 )
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
			PMSND_PlaySE(SOUND_MOVE_CURSOR);

			wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_BUTTON );
			(*seq) = SEQ_EDS_BUTTON_KEYWAIT;
			break;
		}
		if( wk->key_repeat & PAD_KEY_LEFT )
		{
			PMSND_PlaySE(SOUND_CHANGE_SENTENCE);

			wk->edit_pos = 0;
			sentence_decrement( &wk->sentence_wk, &wk->edit_pms );
			PMSIView_SetCommand( wk->vwk, VCMD_UPDATE_EDITAREA );
      PMSIView_TouchEditButton( wk->vwk, EDIT_BUTTON_ID_LEFT );
			(*seq) = SEQ_EDS_WAIT_EDITAREA_UPDATE;
			break;
		}
		if( wk->key_repeat & PAD_KEY_RIGHT )
		{
			PMSND_PlaySE(SOUND_CHANGE_SENTENCE);

			wk->edit_pos = 0;
			sentence_increment( &wk->sentence_wk, &wk->edit_pms );
			PMSIView_SetCommand( wk->vwk, VCMD_UPDATE_EDITAREA );
      PMSIView_TouchEditButton( wk->vwk, EDIT_BUTTON_ID_RIGHT );
			(*seq) = SEQ_EDS_WAIT_EDITAREA_UPDATE;
			break;
		}

		if( wk->key_trg & PAD_BUTTON_B )
		{
			PMSND_PlaySE(SOUND_CANCEL);
			
      SetSubProc( wk, SubProc_CommandCancel );
			break;
		}
		if( wk->key_trg & PAD_BUTTON_A )
		{
			PMSND_PlaySE(SOUND_DECIDE);

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
			PMSND_PlaySE(SOUND_MOVE_CURSOR);

      if(wk->cmd_button_pos){
        // �u��߂�v����u�����Ă��v��
				wk->cmd_button_pos ^= 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			}else{
        // �u�����Ă��v����G�f�B�b�g�G���A��
        wk->edit_pos = (wk->sentence_edit_pos_max>0)?(wk->sentence_edit_pos_max-1):(0);  // �G�f�B�b�g�G���A�̈�ԉ��̘g��
				PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
				(*seq) = SEQ_EDW_KEYWAIT;
			}
			break;
		}else if( wk->key_trg & (PAD_KEY_DOWN) ){
			PMSND_PlaySE(SOUND_MOVE_CURSOR);
			
      if(wk->cmd_button_pos){
        // �u��߂�v����G�f�B�b�g�G���A��
        wk->edit_pos = (wk->sentence_edit_pos_max>0)?(0):(0);  // �G�f�B�b�g�G���A�̈�ԏ�̘g��
				PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
				(*seq) = SEQ_EDW_KEYWAIT;
			}else{
        // �u�����Ă��v����u��߂�v��
				wk->cmd_button_pos ^= 1;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			}
			break;
		}

		if( wk->key_trg & PAD_BUTTON_START ){
			PMSND_PlaySE(SOUND_MOVE_CURSOR);
			
      wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
			break;
		}
		if( wk->key_trg & PAD_BUTTON_A ){
			if( wk->cmd_button_pos == BUTTON_POS_DECIDE ){
        if( check_input_complete( wk ) )
        {
		  	  PMSND_PlaySE(SOUND_DECIDE);
        }
        else
        {
		  	  PMSND_PlaySE(SOUND_DISABLE_BUTTON);
        }
				(*seq) = SEQ_EDS_TO_SUBPROC_OK;
				break;
			}else{
				PMSND_PlaySE(SOUND_CANCEL);
				(*seq) = SEQ_EDS_TO_SUBPROC_CANCEL;
					break;
				}
			}

			if( wk->key_trg & PAD_BUTTON_B ){
				PMSND_PlaySE(SOUND_CANCEL);
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
	* ���͓��̓��[�h
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

	switch( *seq ){
	case SEQ_EDS_INIT:
		wk->sentence_edit_pos_max = PMSIView_GetSentenceEditPosMax( wk->vwk );
		(*seq) = SEQ_EDS_KEYWAIT;
		/* fallthru */

	case SEQ_EDS_KEYWAIT:
	case SEQ_EDS_BUTTON_KEYWAIT:
    
    {
      int idx;

      // �t���b�v�{�^���`�F�b�N
      idx = edit_sentence_check_flipbtn(wk);

      if( idx > -1 )
      {
			  wk->edit_pos = 0;
        sentence_change_type( &wk->sentence_wk, &wk->edit_pms, idx );
        PMSIView_SetCommand( wk->vwk, VCMD_UPDATE_EDITAREA );
        PMSND_PlaySE( SOUND_TOUCH_FLIPBUTTON );
        (*seq) = SEQ_EDS_WAIT_EDITAREA_UPDATE;
        break;
      }
    }

		ret = edit_sentence_touch(wk);

		switch(ret){
		case 0:	//����
      if( check_input_complete( wk ) )
      {
		   PMSND_PlaySE(SOUND_DECIDE);
      }
      else
      {
		   PMSND_PlaySE(SOUND_DISABLE_BUTTON);
      }
			wk->cmd_button_pos = BUTTON_POS_DECIDE;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_BUTTON_TOUCH );  // �ҏW�G���A����Ȃ��u�����Ă�/��߂�{�^���v�̂Ƃ���ɂ��邩������Ȃ����A������ꏊ��ҏW�G���A����{�^���̈�ֈړ����������̂ł��̃R�}���h���Ă�ł����B
			*seq = SEQ_EDS_TO_SUBPROC_OK+ret;
			break;
    case 1:	//��߂�
			PMSND_PlaySE(SOUND_CANCEL);
			wk->cmd_button_pos = BUTTON_POS_CANCEL;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_BUTTON_TOUCH );  // �ҏW�G���A����Ȃ��u�����Ă�/��߂�{�^���v�̂Ƃ���ɂ��邩������Ȃ����A������ꏊ��ҏW�G���A����{�^���̈�ֈړ����������̂ł��̃R�}���h���Ă�ł����B
			*seq = SEQ_EDS_TO_SUBPROC_OK+ret;
			break;
		case 2:	// ��
    case 4:
			PMSND_PlaySE(SOUND_CHANGE_SENTENCE);

			wk->edit_pos = 0;
			sentence_decrement( &wk->sentence_wk, &wk->edit_pms );
			PMSIView_SetCommand( wk->vwk, VCMD_UPDATE_EDITAREA );
      PMSIView_TouchEditButton( wk->vwk, EDIT_BUTTON_ID_LEFT );
			(*seq) = SEQ_EDS_WAIT_EDITAREA_UPDATE;
			break;
		case 3: // �E
    case 5:
			PMSND_PlaySE(SOUND_CHANGE_SENTENCE);

			wk->edit_pos = 0;
			sentence_increment( &wk->sentence_wk, &wk->edit_pms );
			PMSIView_SetCommand( wk->vwk, VCMD_UPDATE_EDITAREA );
      PMSIView_TouchEditButton( wk->vwk, EDIT_BUTTON_ID_RIGHT );
			(*seq) = SEQ_EDS_WAIT_EDITAREA_UPDATE;
			break;
		case edit_sentence_touch_Btn_TpRect_num:	//�P��I��
    case edit_sentence_touch_Btn_TpRect_num +1:
			PMSND_PlaySE(SOUND_DECIDE);
			wk->category_pos = 0;
			wk->edit_pos = ret-edit_sentence_touch_Btn_TpRect_num;
			PMSIView_SetCommand( wk->vwk, VCMD_EDITAREA_TO_CATEGORY );
			*seq = SEQ_EDS_TO_SELECT_CATEGORY;
			break;
		}
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
	    if(wk->key_mode == GFL_APP_KTST_KEY)  // �^�b�`���L�[�ɕύX�����Ƃ�����(KeyStatusChange�֐�����wk->cb_ktchg_func���ĂԈȊO��)���������ɖ߂�
      {
			  return GFL_PROC_RES_CONTINUE;
      }
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
	* ���C���v���Z�X�F�R�}���h�{�^���i�����Ă��E��߂�j�ł̓���
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
				PMSND_PlaySE(SOUND_MOVE_CURSOR);

        if(wk->cmd_button_pos){
          // �u��߂�v����u�����Ă��v��
          wk->cmd_button_pos ^= 1;
          PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
        }else{
          // �u�����Ă��v����G�f�B�b�g�G���A��
          PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
          (*seq) = SEQ_EDW_KEYWAIT;
        }
        break;
      }else if( wk->key_trg & (PAD_KEY_DOWN) ){
        PMSND_PlaySE(SOUND_MOVE_CURSOR);
        
        if(wk->cmd_button_pos){
          // �u��߂�v����G�f�B�b�g�G���A��
          PMSIView_SetCommand( wk->vwk, VCMD_BUTTON_TO_EDITAREA );
          (*seq) = SEQ_EDW_KEYWAIT;
        }else{
          // �u�����Ă��v����u��߂�v��
          wk->cmd_button_pos ^= 1;
          PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
        }
        break;
      }

			if( wk->key_trg & PAD_BUTTON_START )
			{
				PMSND_PlaySE(SOUND_MOVE_CURSOR);
				wk->cmd_button_pos = BUTTON_POS_DECIDE;
				PMSIView_SetCommand( wk->vwk, VCMD_MOVE_BUTTON_CURSOR );
				break;
			}


			if( wk->key_trg & PAD_BUTTON_A )
			{
				PMSND_PlaySE(SOUND_DECIDE);

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
				PMSND_PlaySE(SOUND_DECIDE);
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
	* ���C���v���Z�X�F�J�e�S���I��
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
		if (
            PMSIView_WaitCommand( wk->vwk, VCMD_MOVE_CATEGORY_CURSOR) == FALSE 
		     || PMSIView_WaitCommand( wk->vwk, VCMD_INPUT_BLINK_IN_CATEGORY ) == FALSE
		     || PMSIView_WaitCommand( wk->vwk, VCMD_ERASE_BLINK_IN_CATEGORY_INITIAL ) == FALSE
    )
		{
			break;
		}
		category_input(wk,seq);

		break;

	case SEQ_CA_NEXTPROC:
		if( PMSIView_WaitCommandAll( wk->vwk ) )
		{
      if( wk->next_proc == MainProc_EditArea )
      { 
        // ����EditArea���n�܂�̂ŁA���̃L�[/�^�b�`�ؑւ��s���Ă���
	      PMSIView_SetCommand( wk->vwk, VCMD_KTCHANGE_EDITAREA);
      }
			(*seq) = SEQ_CA_NEXTPROC2;
		}
		break;
	case SEQ_CA_NEXTPROC2:
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
  if( wk->key_trg & PAD_BUTTON_START )
  {
    if( wk->category_mode == CATEGORY_MODE_INITIAL )
    {
		  wk->category_pos = CATEGORY_POS_SELECT;

      // �����J�n
      if( PMSI_SEARCH_Start( wk->swk ) )
      {
        // �P�ꃊ�X�g��
        set_nextproc_category_to_wordwin( wk );
        *seq = SEQ_CA_NEXTPROC;
      }
      else
      {
        PMSND_PlaySE( SOUND_SEARCH_DISABLE );

		    PMSIView_SetCommand( wk->vwk, VCMD_MOVE_CATEGORY_CURSOR );  // �J�[�\�����u����ԁv�̈ʒu�Ɉړ�������
      }
      
      return;
    }
  }
  
  //�ؑփ{�^��
	if(	(wk->touch_button == TOUCH_BUTTON_CHANGE) || (wk->key_trg & PAD_BUTTON_SELECT)
	){
		PMSND_PlaySE(SOUND_CHANGE_CATEGORY);
		wk->category_mode ^= 1;
		wk->category_pos = 0;
		PMSIView_SetCommand( wk->vwk, VCMD_CHANGE_CATEGORY_MODE_ENABLE );
		(*seq) = SEQ_CA_WAIT_MODE_CHANGE;
		return;
	}

  // �L�����Z��
	if( wk->key_trg & PAD_BUTTON_B )
	{
    // INITIAL�Ȃ當������������ȏ�������̂��Ȃ���Ή�ʂ��甲����
    if( wk->category_mode == CATEGORY_MODE_INITIAL && PMSI_SEARCH_DelWord( wk->swk ) )
    {
		  wk->category_pos = CATEGORY_POS_ERASE;

		  PMSND_PlaySE(SOUND_WORD_DELETE);
		  PMSIView_SetCommand( wk->vwk, VCMD_ERASE_BLINK_IN_CATEGORY_INITIAL );

      PMSI_SEARCH_Start( wk->swk );
      PMSIView_SetCommand( wk->vwk, VCMD_INPUTWORD_UPDATE );
    }
    // GROUP�͖������ŉ�ʂ��甲����
    else
    {
      if( wk->category_mode == CATEGORY_MODE_INITIAL )
		    wk->category_pos = CATEGORY_POS_BACK;
      
      PMSND_PlaySE(SOUND_CANCEL); 
      PMSIView_SetCommand( wk->vwk, VCMD_CATEGORY_TO_EDITAREA );
      wk->next_proc = MainProc_EditArea;
		  *seq = SEQ_CA_NEXTPROC;
    }
		return;
	}

  // ����
	if( wk->key_trg & PAD_BUTTON_A )
  {
		if( wk->category_pos == CATEGORY_POS_SELECT )
    {
      if( PMSI_SEARCH_Start( wk->swk ) )
      {
        // �P�ꃊ�X�g��
        set_nextproc_category_to_wordwin( wk );
        *seq = SEQ_CA_NEXTPROC;
      }
      else
      {
        PMSND_PlaySE( SOUND_SEARCH_DISABLE );
      }
    }
    else if( wk->category_pos == CATEGORY_POS_ERASE )
    {
      // INITIAL�Ȃ當������������ȏ�������̂��Ȃ���΋֎~��
      if( wk->category_mode == CATEGORY_MODE_INITIAL && PMSI_SEARCH_DelWord( wk->swk ) )
      {
		    PMSND_PlaySE(SOUND_WORD_DELETE);  // A�{�^�����g���Ă��������ɂȂ���(�V)
		    //PMSND_PlaySE(SOUND_DECIDE);  // A�{�^�����g�����̂Ō��艹(��)
		    PMSIView_SetCommand( wk->vwk, VCMD_ERASE_BLINK_IN_CATEGORY_INITIAL );

        PMSI_SEARCH_Start( wk->swk );
        PMSIView_SetCommand( wk->vwk, VCMD_INPUTWORD_UPDATE );
      }
      else
      {
        PMSND_PlaySE( SOUND_DISABLE_BUTTON );
      }
    }
    else if( wk->category_pos == CATEGORY_POS_BACK )
    {
			PMSND_PlaySE(SOUND_CANCEL);

			PMSIView_SetCommand( wk->vwk, VCMD_CATEGORY_TO_EDITAREA );
			wk->next_proc = MainProc_EditArea;
			*seq = SEQ_CA_NEXTPROC;
		}
    // �J�e�S���I��
    else if( wk->category_mode == CATEGORY_MODE_GROUP )
    {
      PMSIView_SetCommand( wk->vwk, VCMD_INPUT_BLINK_IN_CATEGORY );
      
      if(check_category_enable( wk ) )
      {
        // �P�ꃊ�X�g��
        set_nextproc_category_to_wordwin( wk );
        *seq = SEQ_CA_NEXTPROC;
      }
      else
      {
        PMSND_PlaySE( SOUND_DISABLE_CATEGORY );
      }
	  }
    // �C�j�V�������[�h�͕�������
    else
    {
      PMSI_SEARCH_AddWord( wk->swk, wk->category_pos );
      //PMSND_PlaySE( SOUND_WORD_INPUT );
		  PMSND_PlaySE(SOUND_DECIDE);  // A�{�^�����g�����̂Ō��艹
      
      PMSI_SEARCH_Start( wk->swk );
      PMSIView_SetCommand( wk->vwk, VCMD_INPUTWORD_UPDATE );
      
      PMSIView_SetCommand( wk->vwk, VCMD_INPUT_BLINK_IN_CATEGORY );
    }
    return;
  }

  // �J�[�\���ړ��`�F�b�N
	if( check_category_cursor_move( wk ) )
  {
		PMSND_PlaySE(SOUND_MOVE_CURSOR);
		PMSIView_SetCommand( wk->vwk, VCMD_MOVE_CATEGORY_CURSOR );
		return;
	}
}

/**
	*	@brief	���[�h�ؑփ{�^���擾
	*/
static int input_category_word_btn(PMS_INPUT_WORK* wk)
{
	int ret;

	static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
		{TPCA_RET_PY,TPCA_RET_PY+TPCA_RET_SY,TPCA_RET_PX,TPCA_RET_PX+TPCA_RET_SX},  ///< ���ǂ�{�^��
		{TPCA_MODE_PY,TPCA_MODE_PY+TPCA_MODE_SY,TPCA_MODE_PX,TPCA_MODE_PX+TPCA_MODE_SX},  ///< �`�F���W�{�^��
		{GFL_UI_TP_HIT_END,0,0,0}
	};
	ret = GFL_UI_TP_HitTrg(Btn_TpRect);
	if(	ret == GFL_UI_TP_HIT_NONE){
		return 0;
	}
	switch(ret){
	case 0:	//�߂�
		return 1;
	case 1:	//�O���[�v
		if(wk->touch_button == TOUCH_BUTTON_CHANGE){
			return 2;
		}
		break;
	}
	return 0;
}

// �߂�{�^���`�F�b�N
static int input_word_btn(PMS_INPUT_WORK* wk)
{
	static const GFL_UI_TP_HITTBL Btn_TpRect[] = {
		{TPCA_RET_PY,TPCA_RET_PY+TPCA_RET_SY-1,TPCA_RET_PX,TPCA_RET_PX+TPCA_RET_SX-1},
		{GFL_UI_TP_HIT_END,0,0,0}
	};

	return GFL_UI_TP_HitTrg(Btn_TpRect);
}

static int category_touch_group(PMS_INPUT_WORK* wk)
{
	int i,j;
	u32 tpx,tpy;
	GFL_UI_TP_HITTBL tbl[2] = { {GFL_UI_TP_HIT_END,0,0,0}, {GFL_UI_TP_HIT_END,0,0,0} };

	if(GFL_UI_TP_GetTrg() == 0){
		return -1;
	}

  // ���X�g
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
      
//-----------------------------------------------------------------------------
/**
 *	@brief  �J�e�S�� ������ʂ̃{�^�������蔻��
 *
 *	@param	PMS_INPUT_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static int category_touch_initial_button( PMS_INPUT_WORK* wk )
{
  GFL_UI_TP_HITTBL rect[] = {
    { 8*21,     8*(21+3),   8*(5),      8*(5+9)   },  ///< �����
    { 8*21,     8*(21+3),   8*(5+9),    8*(5+9*2) },  ///< ����
    { 8*21,     8*(21+3),   8*(5+9*2),  8*(5+9*3) },  ///< ��߂�
    {GFL_UI_TP_HIT_END,0,0,0}
  };

  return GFL_UI_TP_HitTrg(rect);
}

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	wk
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
static int category_touch_initial(PMS_INPUT_WORK* wk)
{
  int i;
  int initial_max;
    
  // 50���^�b�`
	
  initial_max = PMSI_INITIAL_DAT_GetInitialMax();
  
  for( i=0; i<initial_max; i++ )
  {
    GFL_UI_TP_HITTBL rect[2] = {
      {0,0,0,0},
      {GFL_UI_TP_HIT_END,0,0,0}
    };

    u32 ini_x, ini_y;
    
    PMSI_INITIAL_DAT_GetPrintPos(i, &ini_x, &ini_y);

    rect[0].rect.top    = ini_y + TPCA_IMA_PY;
    rect[0].rect.bottom = ini_y + TPCA_IMA_PY + TPCA_IMA_SY;
    rect[0].rect.left   = ini_x + TPCA_IMA_PX;
    rect[0].rect.right  = ini_x + TPCA_IMA_PX + TPCA_IMA_SX;
	
    if( GFL_UI_TP_HitTrg(rect) != GFL_UI_TP_HIT_NONE )
    {
      HOSAKA_Printf("touch=%d\n",i);
      return i;
    }
  }

	return -1;
}

/**
	*	@brief
	*/
static void category_input_touch(PMS_INPUT_WORK* wk,int* seq)
{
	int ret;

	ret = input_category_word_btn(wk);

	switch(ret){
	case 1:
		PMSND_PlaySE(SOUND_CANCEL);

		PMSIView_SetCommand( wk->vwk, VCMD_CATEGORY_TO_EDITAREA );
		wk->next_proc = MainProc_EditArea;
		*seq = SEQ_CA_NEXTPROC;
		return;
	case 2:
		PMSND_PlaySE(SOUND_CHANGE_CATEGORY);
		wk->category_mode ^= 1;
		wk->category_pos = 0;
		PMSIView_SetCommand( wk->vwk, VCMD_CHANGE_CATEGORY_MODE_ENABLE );
		(*seq) = SEQ_CA_WAIT_MODE_CHANGE;
		return;
	}

  // �O���[�v
	if( wk->category_mode == CATEGORY_MODE_GROUP )
  {
		ret = category_touch_group(wk);
    // �^�b�`�Ȃ�
		if(ret < 0 ) { return; }
		wk->category_pos = ret;
    PMSIView_SetCommand( wk->vwk, VCMD_INPUT_BLINK_IN_CATEGORY );
    // ��������
		if( ( ret >= CATEGORY_GROUP_MAX ) ||
        ( PMSI_DATA_GetGroupEnableWordCount( wk->dwk, wk->category_pos ) == 0 ) )
    {
			PMSND_PlaySE( SOUND_DISABLE_CATEGORY );
			return;
		}
    // �P�ꃊ�X�g��
    set_nextproc_category_to_wordwin( wk );
    *seq = SEQ_CA_NEXTPROC;
	}
  // �C�j�V����
  else
  {
    // ��������
		ret = category_touch_initial(wk);

		if(ret > -1)
    {
      wk->category_pos = ret;
      PMSI_SEARCH_AddWord( wk->swk, wk->category_pos );

      PMSI_SEARCH_Start( wk->swk );
      PMSIView_SetCommand( wk->vwk, VCMD_INPUTWORD_UPDATE );

      PMSND_PlaySE( SOUND_WORD_INPUT );

      PMSIView_SetCommand( wk->vwk, VCMD_INPUT_BLINK_IN_CATEGORY );
      return;
    }
    else
    {
      int btn_ret;
      // �u����ԁv�u�����v�u��߂�v�̓����蔻��
      btn_ret = category_touch_initial_button( wk );

#ifdef PM_DEBUG
      if(btn_ret!=-1){HOSAKA_Printf("btn_ret=%d\n",btn_ret);}
#endif
      
      switch( btn_ret )
      {
      case -1:
        // �����Ȃ�
        break;
      case 0 :
        wk->category_pos = CATEGORY_POS_SELECT;
        if( PMSI_SEARCH_Start( wk->swk ) )
        {
          // �P�ꃊ�X�g��
          set_nextproc_category_to_wordwin( wk );
          *seq = SEQ_CA_NEXTPROC;
        }
        else
        {
          PMSND_PlaySE( SOUND_SEARCH_DISABLE );
        }
        break;
      case 1 :
        // ����
        wk->category_pos = CATEGORY_POS_ERASE;
        if( PMSI_SEARCH_DelWord( wk->swk ) )
        {
          PMSND_PlaySE(SOUND_WORD_DELETE);
		      PMSIView_SetCommand( wk->vwk, VCMD_ERASE_BLINK_IN_CATEGORY_INITIAL );

          PMSI_SEARCH_Start( wk->swk );
          PMSIView_SetCommand( wk->vwk, VCMD_INPUTWORD_UPDATE );
        }
        else
        {
          PMSND_PlaySE( SOUND_DISABLE_BUTTON );
        }
        break;
      case 2 :
        {
          wk->category_pos = CATEGORY_POS_BACK;
          PMSND_PlaySE(SOUND_CANCEL);

          PMSIView_SetCommand( wk->vwk, VCMD_CATEGORY_TO_EDITAREA );
          wk->next_proc = MainProc_EditArea;
          *seq = SEQ_CA_NEXTPROC;
        }
        break;
      default : GF_ASSERT(0);
      }

    }
	}
}

/**
	*	@brief	
	*/
static void category_input(PMS_INPUT_WORK* wk,int* seq)
{
	if(KeyStatusChange(wk,seq)){
    if( wk->category_mode == CATEGORY_MODE_GROUP )
    {
	    if( wk->key_mode == GFL_APP_KTST_KEY && (!(GFL_UI_KEY_GetTrg() & PAD_BUTTON_B)) )  // �^�b�`���L�[�ɕύX�����Ƃ��ŁA���̂Ƃ��̓��͂�B�{�^���łȂ��Ƃ�����(KeyStatusChange�֐�����wk->cb_ktchg_func���ĂԈȊO��)���������ɖ߂�
      {
        return;
      }
    }
    else
    {
	    if(wk->key_mode == GFL_APP_KTST_KEY)  // �^�b�`���L�[�ɕύX�����Ƃ�����(KeyStatusChange�֐�����wk->cb_ktchg_func���ĂԈȊO��)���������ɖ߂�
      {
			  return;
      }
    }
  }

  // ���o���͎󂯕t���Ȃ�
  // @TODO �ǂ��ɂ��㏟���ɂł��Ȃ����H
  if( PMSIView_WaitCommand( wk->vwk, VCMD_INPUTWORD_UPDATE ) == FALSE )
  {
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
	* �I�����Ă���J�e�S���Ɏg�p�\�ȒP�ꂪ�܂܂�Ă��邩�`�F�b�N
	*
	* @param   wk		
	*
	* @retval  BOOL	TRUE�Ŋ܂܂�Ă���
	*/
//------------------------------------------------------------------
static BOOL check_category_enable( PMS_INPUT_WORK* wk )
{
	return (PMSI_DATA_GetGroupEnableWordCount( wk->dwk, wk->category_pos ) != 0);
}

//------------------------------------------------------------------
/**
	* �J�e�S���I�����̃J�[�\���ړ��`�F�b�N
	*
	* @param   wk			���[�N�|�C���^
	*
	* @retval  BOOL		TRUE�ŃJ�[�\���ړ�����
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
// �O���[�v���[�h
//----------------------------------------------
static BOOL keycheck_category_group_mode( PMS_INPUT_WORK* wk )
{
	static const struct {
		u8   up_pos;
		u8   down_pos;
		u8   left_pos;
		u8   right_pos;
	}next_pos_tbl[CATEGORY_GROUP_MAX] = {
		{ CATEGORY_GROUP_MIND,		  CATEGORY_GROUP_TRAINER,	  CATEGORY_GROUP_SKILL,	    CATEGORY_GROUP_STATUS },	    // �|�P����
		{ CATEGORY_GROUP_TERM,	    CATEGORY_GROUP_UNION,	    CATEGORY_GROUP_POKEMON,	  CATEGORY_GROUP_SKILL },	      // �X�e�[�^�X
		{ CATEGORY_GROUP_PICTURE,		CATEGORY_GROUP_GREET,	    CATEGORY_GROUP_STATUS,    CATEGORY_GROUP_POKEMON },	    // �킴

		{ CATEGORY_GROUP_POKEMON,	  CATEGORY_GROUP_INTERJECT, CATEGORY_GROUP_GREET,	    CATEGORY_GROUP_UNION },	      // �g���[�i�[
		{ CATEGORY_GROUP_STATUS,  	CATEGORY_GROUP_PERSON,	  CATEGORY_GROUP_TRAINER,	  CATEGORY_GROUP_GREET },		    // ���j�I��
		{ CATEGORY_GROUP_SKILL,	    CATEGORY_GROUP_LIFE,	    CATEGORY_GROUP_UNION,	    CATEGORY_GROUP_TRAINER },	    // ��������

	  { CATEGORY_GROUP_TRAINER,	  CATEGORY_GROUP_MIND,		  CATEGORY_GROUP_LIFE,	    CATEGORY_GROUP_PERSON },	    // ����
		{ CATEGORY_GROUP_UNION,	    CATEGORY_GROUP_TERM,	    CATEGORY_GROUP_INTERJECT,	CATEGORY_GROUP_LIFE },		    // �Ђ�
		{ CATEGORY_GROUP_GREET,	    CATEGORY_GROUP_PICTURE,	  CATEGORY_GROUP_PERSON,	  CATEGORY_GROUP_INTERJECT },	  // ��������

		{ CATEGORY_GROUP_INTERJECT,	CATEGORY_GROUP_POKEMON,	  CATEGORY_GROUP_PICTURE,	  CATEGORY_GROUP_TERM },	      // ������
	  { CATEGORY_GROUP_PERSON,	  CATEGORY_GROUP_STATUS,		CATEGORY_GROUP_MIND,	    CATEGORY_GROUP_PICTURE },	    // �悤��
	  { CATEGORY_GROUP_LIFE,	    CATEGORY_GROUP_SKILL,		  CATEGORY_GROUP_TERM,	    CATEGORY_GROUP_MIND },	      // �s�N�`��
	};

	u32  pos = wk->category_pos;
	if( pos == CATEGORY_POS_BACK )
	{
		// �e�[�u���������悤��
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

//-----------------------------------------------------------------------------
/**
 *	@brief  �C�j�V�������[�h �L�[����
 *
 *	@param	PMS_INPUT_WORK* wk  ���[�N
 *
 *	@retval TRUE
 */
//-----------------------------------------------------------------------------
static BOOL keycheck_category_initial_mode( PMS_INPUT_WORK* wk )
{
	if( wk->category_pos != CATEGORY_POS_BACK &&
      wk->category_pos != CATEGORY_POS_SELECT &&
      wk->category_pos != CATEGORY_POS_ERASE )
	{
		int  next_pos = CATEGORY_POS_DISABLE;

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
      int pos_prv = PMSI_INITIAL_DAT_GetColBottomCode( wk->category_pos_prv );
      if( wk->category_pos == CATEGORY_POS_SELECT )
      {
        if( INI_NA <= pos_prv && pos_prv <= INI_NO ) wk->category_pos = pos_prv;
        else                                         wk->category_pos = INI_NU;
      }
      else if( wk->category_pos == CATEGORY_POS_ERASE )
      {
        if( pos_prv == INI_WA || pos_prv == INI_WO ) wk->category_pos = pos_prv;
        else                                         wk->category_pos = INI_WA;
      }
      else  // if( wk->category_pos == CATEGORY_POS_BACK )
      {
        if( pos_prv == INI_NN || pos_prv == INI_BOU || pos_prv == INI_OTHER ) wk->category_pos = pos_prv;
        else                                                                  wk->category_pos = INI_BOU;
      }

			return TRUE;
		}
		else if( wk->key_repeat & PAD_KEY_DOWN )
		{
      int pos_prv = PMSI_INITIAL_DAT_GetColTopCode( wk->category_pos_prv );
      if( wk->category_pos == CATEGORY_POS_SELECT )
      {
        if( INI_A <= pos_prv && pos_prv <= INI_O ) wk->category_pos = pos_prv;
        else                                       wk->category_pos = INI_U;
      }
      else if( wk->category_pos == CATEGORY_POS_ERASE )
      {
        if( pos_prv == INI_HA || pos_prv == INI_HI ) wk->category_pos = pos_prv;
        else                                         wk->category_pos = INI_HA;
      }
      else  // if( wk->category_pos == CATEGORY_POS_BACK )
      {
        if( pos_prv == INI_HU || pos_prv == INI_HE || pos_prv == INI_HO ) wk->category_pos = pos_prv;
        else                                                              wk->category_pos = INI_HE;
      }

			return TRUE;
		}
    else if( wk->key_repeat & PAD_KEY_LEFT )
    {
      switch( wk->category_pos )
      {
      case CATEGORY_POS_BACK    : wk->category_pos = CATEGORY_POS_ERASE; break;
      case CATEGORY_POS_ERASE   : wk->category_pos = CATEGORY_POS_SELECT; break;
      case CATEGORY_POS_SELECT  : wk->category_pos = CATEGORY_POS_BACK; break;
      default : GF_ASSERT(0);
      }

      return TRUE;
    }
    else if( wk->key_repeat & PAD_KEY_RIGHT )
    {
      switch( wk->category_pos )
      {
      case CATEGORY_POS_BACK    : wk->category_pos = CATEGORY_POS_SELECT; break;
      case CATEGORY_POS_ERASE   : wk->category_pos = CATEGORY_POS_BACK; break;
      case CATEGORY_POS_SELECT  : wk->category_pos = CATEGORY_POS_ERASE; break;
      default : GF_ASSERT(0);
      }
      
      return TRUE;
    }
	}

	return FALSE;
}


//------------------------------------------------------------------
/**
	* �P��E�B���h�E�p�����[�^������
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
		word_win->line_max = (((word_win->word_max-PMSI_DUMMY_LABEL_NUM) - WORDWIN_DISP_MAX) / 2) + (word_win->word_max & 1);
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
	* ���C���v���Z�X�F�P��I��
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

	case SEQ_WORD_SCROLL_BAR_WAIT:
		if( PMSIView_WaitCommand( wk->vwk, VCMD_SCROLL_WORDWIN_BAR ) )
		{
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
	{
		int result;

		{
			result = check_wordwin_key( &(wk->word_win), wk->key_repeat );
		}

		switch( result ){
		case WORDWIN_RESULT_CURSOR_MOVE:
			PMSND_PlaySE(SOUND_MOVE_CURSOR);
			PMSIView_SetCommand( wk->vwk, VCMD_MOVE_WORDWIN_CURSOR );
			return;

		case WORDWIN_RESULT_SCROLL:
			PMSND_PlaySE(SOUND_MOVE_CURSOR);
			PMSIView_SetCommand( wk->vwk, VCMD_SCROLL_WORDWIN );
			(*seq) = SEQ_WORD_SCROLL_WAIT;
			return;

		case WORDWIN_RESULT_SCROLL_AND_CURSOR_MOVE:
			PMSND_PlaySE(SOUND_MOVE_CURSOR);
			PMSIView_SetCommand( wk->vwk, VCMD_SCROLL_WORDWIN );
			(*seq) = SEQ_WORD_SCROLL_WAIT_AND_CURSOR_MOVE;
			return;
		}
	}

	if( wk->key_trg & PAD_BUTTON_B )
	{
		PMSND_PlaySE(SOUND_CANCEL);

		PMSIView_SetCommand( wk->vwk, VCMD_WORDWIN_TO_CATEGORY );
		wk->next_proc = MainProc_Category;
		*seq = SEQ_WORD_CHANGE_NEXTPROC;
		return;
	}
	if( wk->key_trg & PAD_BUTTON_A )
	{
		if(wk->word_win.back_f){
			PMSND_PlaySE(SOUND_CANCEL);

			PMSIView_SetCommand( wk->vwk, VCMD_WORDWIN_TO_CATEGORY );
			wk->next_proc = MainProc_Category;
			*seq = SEQ_WORD_CHANGE_NEXTPROC;
			return;
		}
		PMSND_PlaySE(SOUND_DECIDE);

		wk->word_win.touch_pos = 0xFFFF;
		set_select_word( wk );
    PMSIView_SetCommand( wk->vwk, VCMD_INPUT_BLINK_IN_WORDWIN );
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
		{PMSIV_TPWD_RAIL_PY,PMSIV_TPWD_RAIL_PY+PMSIV_TPWD_RAIL_SY-1,PMSIV_TPWD_RAIL_PX,PMSIV_TPWD_RAIL_PX+PMSIV_TPWD_RAIL_SX-1},
		{GFL_UI_TP_HIT_END,0,0,0}
	};

	// �X�N���[���o�[�`�F�b�N
	if( GFL_UI_TP_HitTrg(Btn_TpRect) == 0 ){
		u32	tpypos;
		GFL_UI_TP_GetPointTrg( &tpx,&tpy );
		tpypos = PMSIView_GetScrollBarPos( wk->vwk, tpx, tpy );
		switch( tpypos ){
		case 0:	// �^�b�`���W�ɃX�N���[���o�[������
			wk->scroll_bar_flg = 1;
      PMSND_PlaySE(SOUND_TOUCH_SLIDEBAR);  // ����ȂƂ���ŉ���炵�����͂Ȃ����A���̖߂�l�ł͌Ăяo�����Ŗ炷�͍̂���B
			return WORDWIN_RESULT_NONE;
		case 1:	// �^�b�`���W���X�N���[���o�[�����ɂ���i��ɃX�N���[���j
			wk->scroll_bar_flg = 1;
			PMSIView_SetScrollBarPos( wk->vwk, tpy );
			return check_wordwin_scroll_up_pos( wk->vwk, &(wk->word_win) );
		case 2:	// �^�b�`���W���X�N���[���o�[����ɂ���i���ɃX�N���[���j
			wk->scroll_bar_flg = 1;
			PMSIView_SetScrollBarPos( wk->vwk, tpy );
			return check_wordwin_scroll_down_pos( wk->vwk, &(wk->word_win) );
    case 3:  // 1��ʂŕ\���ł���̂ŃX�N���[���o�[���s�v�ŁA��\���ɂ��Ă���
      // �������Ȃ�
      break;
		}
	}


	GFL_UI_TP_GetPointTrg( &tpx,&tpy );
	pos = wk->word_win.line*2;
	end_f = 0;
	for(i = 0;i < 6;i++){
		tbl[0].rect.top = i*TPWD_WORD_OY+TPWD_WORD_PY;
		tbl[0].rect.bottom = tbl[0].rect.top+TPWD_WORD_SY;
		for(j = 0;j < 2;j++){
			tbl[0].rect.left = j*TPWD_WORD_OX+TPWD_WORD_PX;
			tbl[0].rect.right = tbl[0].rect.left+TPWD_WORD_SX;
		
			if(GFL_UI_TP_HitSelf( tbl, tpx,tpy) != GFL_UI_TP_HIT_NONE ){
				wk->word_win.touch_pos = pos;

        wk->word_win.cursor_x = j;  // �I�������Ƃ��ɖ��ŃA�j�������Ȃ���΂Ȃ�Ȃ��̂ŁA
        wk->word_win.cursor_y = i;  // �J�[�\���̏ꏊ���^�b�`�����ꏊ�ɂ��Ă���

				ARI_TPrintf(" Select Pos = %d -> %d\n",i*2+j , pos);
				return WORDWIN_RESULT_SELECT;
			}
			if(++pos >= (wk->word_win.word_max-PMSI_DUMMY_LABEL_NUM)){
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

// �X�N���[���o�[�^�b�`�p���`�F�b�N
static BOOL word_input_scroll_bar_touch(PMS_INPUT_WORK* wk)
{
	u32	tpx, tpy;

	if( GFL_UI_TP_GetPointCont( &tpx, &tpy ) == TRUE ){
		if( wk->scroll_bar_flg == 1 ){
			PMSIView_SetScrollBarPos( wk->vwk, tpy );
			return TRUE;
		}
	}
	wk->scroll_bar_flg = 0;
	return FALSE;
}

static void word_input_touch(PMS_INPUT_WORK* wk,int* seq)
{
	int ret;

	// �߂�{�^���`�F�b�N
	if( input_word_btn(wk) == 0 ){
		PMSND_PlaySE(SOUND_CANCEL);
		PMSIView_SetCommand( wk->vwk, VCMD_WORDWIN_TO_CATEGORY );
		wk->next_proc = MainProc_Category;
		*seq = SEQ_WORD_CHANGE_NEXTPROC;
		return;
	}

	// �X�N���[���o�[�^�b�`�p���`�F�b�N
	if( word_input_scroll_bar_touch(wk) == TRUE ){
		check_wordwin_scroll_pos( wk->vwk, &(wk->word_win) );
		PMSIView_SetCommand( wk->vwk, VCMD_SCROLL_WORDWIN_BAR );
		(*seq) = SEQ_WORD_SCROLL_BAR_WAIT;
		return;
	}

	ret = word_input_scr_btn(wk);

	switch( ret ){
	case WORDWIN_RESULT_SELECT:
		PMSND_PlaySE(SOUND_DECIDE);

		set_select_word( wk );
		wk->next_proc = MainProc_EditArea;
    PMSIView_SetCommand( wk->vwk, VCMD_INPUT_BLINK_IN_WORDWIN );
		PMSIView_SetCommand( wk->vwk, VCMD_WORDTIN_TO_EDITAREA );
		(*seq) = SEQ_WORD_CHANGE_NEXTPROC;
		return;
	case WORDWIN_RESULT_CURSOR_MOVE:
		PMSND_PlaySE(SOUND_MOVE_CURSOR);
		PMSIView_SetCommand( wk->vwk, VCMD_MOVE_WORDWIN_CURSOR );
		return;

	case WORDWIN_RESULT_SCROLL:
		PMSND_PlaySE(SOUND_TOUCH_SLIDEBAR);
		PMSIView_SetCommand( wk->vwk, VCMD_SCROLL_WORDWIN_BAR );
		(*seq) = SEQ_WORD_SCROLL_BAR_WAIT;
		return;

	case WORDWIN_RESULT_SCROLL_AND_CURSOR_MOVE:
		PMSND_PlaySE(SOUND_MOVE_CURSOR);
		PMSIView_SetCommand( wk->vwk, VCMD_SCROLL_WORDWIN );
		(*seq) = SEQ_WORD_SCROLL_WAIT_AND_CURSOR_MOVE;
		return;
	}
}

static void word_input(PMS_INPUT_WORK* wk,int* seq)
{
	if(KeyStatusChange(wk,seq)){
	  if( wk->key_mode == GFL_APP_KTST_KEY && (!(GFL_UI_KEY_GetTrg() & PAD_BUTTON_B)) )  // �^�b�`���L�[�ɕύX�����Ƃ��ŁA���̂Ƃ��̓��͂�B�{�^���łȂ��Ƃ�����(KeyStatusChange�֐�����wk->cb_ktchg_func���ĂԈȊO��)���������ɖ߂�
    {
      return;
    }
  }

	if(wk->key_mode == GFL_APP_KTST_KEY){
		word_input_key(wk,seq);
	}else{
		word_input_touch(wk,seq);
	}
}


static int check_wordwin_key( WORDWIN_WORK* wordwin, u16 key )
{
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

      if( pos < wordwin->word_max - PMSI_DUMMY_LABEL_NUM )
			{
				return WORDWIN_RESULT_CURSOR_MOVE;
			}
			else if( pos == wordwin->word_max - PMSI_DUMMY_LABEL_NUM )
			{
				if( pos & 1 )
				{
					wordwin->cursor_x = 0;
					return WORDWIN_RESULT_CURSOR_MOVE;
				}
        else
        {
			    wordwin->cursor_y--;
			    return WORDWIN_RESULT_INVALID;
        }
			}
      else
      {
			  wordwin->cursor_y--;
			  return WORDWIN_RESULT_INVALID;
      }
		}
		else if( wordwin->line < wordwin->line_max )
		{
			wordwin->scroll_lines = 1;
			wordwin->line++;
			
			if( get_wordwin_pos( wordwin ) < wordwin->word_max - PMSI_DUMMY_LABEL_NUM )
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

  // wordwin(�t�V�M�o�i �q�g�J�Q ���U�[�h �Ƃ� �A�N�A�e�[�� ���� �Ƃ�������ł�����)�ŁA�E�[�ɂ���Ƃ��ɉE���͂ō��[�֍s���Ȃ�
	if( key & PAD_KEY_LEFT  )
	{
    if( wordwin->cursor_x == 1 )
    {
		  wordwin->cursor_x ^= 1;
		  if( get_wordwin_pos( wordwin ) < wordwin->word_max - PMSI_DUMMY_LABEL_NUM )
      {
			  return WORDWIN_RESULT_CURSOR_MOVE;
      }
      else
      {
		    wordwin->cursor_x ^= 1;
      }
    }
		return WORDWIN_RESULT_INVALID;
	}
  else if( key & PAD_KEY_RIGHT )
	{
    if( wordwin->cursor_x == 0 )
    {
		  wordwin->cursor_x ^= 1;
		  if( get_wordwin_pos( wordwin ) < wordwin->word_max - PMSI_DUMMY_LABEL_NUM )
      {
			  return WORDWIN_RESULT_CURSOR_MOVE;
      }
      else
      {
		    wordwin->cursor_x ^= 1;
      }
    }
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

// �X�N���[���o�[�̏���^�b�`�����Ƃ��̃X�N���[���ݒ�
static int check_wordwin_scroll_up_pos( PMS_INPUT_VIEW* vwk, WORDWIN_WORK* wordwin )
{
	if( wordwin->line ){
		wordwin->line = PMSIView_GetScrollBarPosCount( vwk, wordwin->line_max );
		wordwin->scroll_lines = 0;
		return WORDWIN_RESULT_SCROLL;
	}
	return WORDWIN_RESULT_INVALID;
}

// �X�N���[���o�[�̉����^�b�`�����Ƃ��̃X�N���[���ݒ�
static int check_wordwin_scroll_down_pos( PMS_INPUT_VIEW* vwk, WORDWIN_WORK* wordwin )
{
	if( wordwin->line < wordwin->line_max ){
		wordwin->line = PMSIView_GetScrollBarPosCount( vwk, wordwin->line_max );
		wordwin->scroll_lines = 0;
		return WORDWIN_RESULT_SCROLL;
	}
	return WORDWIN_RESULT_INVALID;
}

// �X�N���[���o�[���^�b�`�����Ƃ��̃X�N���[���ݒ�
static int check_wordwin_scroll_pos( PMS_INPUT_VIEW* vwk, WORDWIN_WORK* wordwin )
{
  u32 line_prev = wordwin->line;

	wordwin->line = PMSIView_GetScrollBarPosCount( vwk, wordwin->line_max );
	wordwin->scroll_lines = 0;

  if( line_prev != wordwin->line )  // �ړ�����
  {
    PMSND_PlaySE( SOUND_TOUCH_SLIDEBAR );
  }

  return WORDWIN_RESULT_SCROLL;
}


//------------------------------------------------------------------
/**
	* �I�����ꂽ�P����f�[�^�ɔ��f����
	*
	* @param   wk		���[�N
	*
	* @retval  BOOL	���͂ł��闓��S�Ė��߂���TRUE���Ԃ�
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
    word = PMSI_SEARCH_GetWordCode( wk->swk, word_idx );
	}

	//OS_TPrintf(" WordSet pos=%d, idx = %d, word = %d \n",wk->edit_pos,word_idx,word);
	
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
	* ���C���v���Z�X�F�t�F�[�h�A�E�g�`�I��
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
	* �T�u�v���Z�X�X�^�[�g�i���C���E�T�u��������Ă�ŗǂ��j
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
	* �T�u�v���Z�X�I���i���s���̃T�u�v���Z�X����̂݌Ăяo���j
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
	* �T�u�v���Z�X�F�t�F�[�h�C��
	*
	* @param   wk		���[�N�|�C���^
	* @param   seq		�V�[�P���X
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
	* �T�u�v���Z�X�F�u�����Ă��v�R�}���h
	*
	* @param   wk		���[�N�|�C���^
	* @param   seq		�V�[�P���X
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
		if( PMSIView_WaitCommandAll( wk->vwk ) )
    {
  		//if( CheckModified( wk ) || PMSI_PARAM_GetNotEditEgnoreFlag(wk->input_param) )
  	  if( 1 )  // �ύX���Ă��Ȃ��Ă��A���肪�I�ׂĂ���	
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
  				(*seq) = SEQ_RETURN;
  			}
  		}
  		else
  		{
        // �����ȃ{�^���^�b�`��
//���̓��C���v���Z�X�Ŗ炷        PMSND_PlaySE( SOUND_DISABLE_BUTTON );

  			PMSIView_SetCommand( wk->vwk, VCMD_ERASE_MENU );
  			(*seq) = SEQ_RETURN;
  //			SetSubProc( wk, SubProc_CommandCancel );
  		}
    }
		break;

	case SEQ_MENU_CTRL:
		ret = PMSIView_WaitYesNo(wk->vwk);
		switch(ret){
		case 0:
	//���̓��C���v���Z�X�Ŗ炷		PMSND_PlaySE(SOUND_CANCEL);

			PMSIView_SetCommand( wk->vwk, VCMD_ERASE_MENU );
			(*seq) = SEQ_RETURN;
			break;
		case 1:
	//���̓��C���v���Z�X�Ŗ炷		PMSND_PlaySE(SOUND_DECIDE);
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
		break;

	case SEQ_WAIT_ANYKEY:
		if( wk->key_trg & (PAD_BUTTON_A|PAD_BUTTON_B|PAD_PLUS_KEY_MASK) )
		{
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
	* �T�u�v���Z�X�F�u��߂�v�R�}���h
	*
	* @param   wk		���[�N�|�C���^
	* @param   seq		�V�[�P���X
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
//			PMSND_PlaySE(SOUND_CANCEL);

			PMSIView_SetCommand( wk->vwk, VCMD_ERASE_MENU );
			(*seq) = SEQ_RETURN;
			break;
		case 1:
//			PMSND_PlaySE(SOUND_DECIDE);
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
	* ���͗���������ԂƔ�ׂĕύX����Ă��邩�`�F�b�N
	*
	* @param   wk			
	*
	* @retval  BOOL		�X�V����Ă�����TRUE��Ԃ�
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
	* ���j���[�R���g���[��������
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
	* ���j���[�R���g���[���X�V
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
	* �T�u�v���Z�X�F �J�e�S�����[�h�ύX�i�ҏW�E�{�^�����쎞�̂݁j
	*
	* @param   wk		���[�N�|�C���^
	* @param   seq		�V�[�P���X
	*
	*/
//--------------------------------------------------------------------------------------
static void SubProc_ChangeCategoryMode( PMS_INPUT_WORK* wk, int* seq )
{
	switch(*seq ){

	case 0:
		PMSND_PlaySE(SOUND_CHANGE_CATEGORY);
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
	*	@brief	�L�[���[�h�ێ��|�C���^�̎擾
	*/
int* PMSI_GetKTModePointer(const PMS_INPUT_WORK* wk)
{
	return (int*)&(wk->key_mode);
}

//------------------------------------------------------------------
/**
	* ���̓��[�h�擾
	*
	* @param   wk		���[�N�|�C���^
	*
	* @retval  u32		���̓��[�h�ienum PMSI_MODE)
	*/
//------------------------------------------------------------------
u32 PMSI_GetInputMode( const PMS_INPUT_WORK* wk )
{
	return wk->input_mode;
}

//------------------------------------------------------------------
/**
	* ����������^�C�v�擾
	*
	* @param   wk		���[�N�|�C���^
	*
	* @retval  u32		����������^�C�v�ienum PMSI_GUIDANCE)
	*/
//------------------------------------------------------------------
u32 PMSI_GetGuidanceType( const PMS_INPUT_WORK* wk )
{
	return PMSI_PARAM_GetGuidanceType( wk->input_param );
}

//------------------------------------------------------------------
/**
	* �J�e�S�����[�h�擾
	*
	* @param   wk		���[�N�|�C���^
	*
	* @retval  u32		�J�e�S�����[�h
	*/
//------------------------------------------------------------------
u32 PMSI_GetCategoryMode( const PMS_INPUT_WORK* wk )
{
	return wk->category_mode;
}

//------------------------------------------------------------------
/**
	* �\�����̕��̓^�C�v���擾
	*
	* @param   wk		���[�N�|�C���^
	*
	* @retval  u32		���̓^�C�v�ienum PMS_TYPE�j
	*/
//------------------------------------------------------------------
u32 PMSI_GetSentenceType( const PMS_INPUT_WORK* wk )
{
	return PMSDAT_GetSentenceType( &wk->edit_pms );
}

//------------------------------------------------------------------
/**
	* �ҏW���̒P��i���o�[��Ԃ�
	*
	* @param   wk			���[�N�|�C���^
	* @param   pos			���Ԗڂ̒P��H
	*
	* @retval  PMS_WORD	�ȈՉ�b�P��i���o�[
	*/
//------------------------------------------------------------------
PMS_WORD  PMSI_GetEditWord( const PMS_INPUT_WORK* wk, int pos )
{
	if( wk->input_mode == PMSI_MODE_SENTENCE )
	{
		return PMSDAT_GetWordNumberDirect( &wk->edit_pms, pos );
	}
	else
	{
		return wk->edit_word[ pos ];
	}
}

//------------------------------------------------------------------
/**
	* �������Ԃ�
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
	* �ҏW�G���A�̃J�[�\���ʒu��Ԃ�(�ҏW�G���A���̒�^���ɒP��𓖂Ă͂߂�̈�̃J�[�\���ʒu)
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
	* �{�^���J�[�\���ʒu�擾(�ҏW�G���A���̃^�X�N���j���[(����/��߂�)�̈�̃J�[�\���ʒu)
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
	* �ҏW�G���A���ŁA�ҏW�̈�(��^���ɒP��𓖂Ă͂߂�)�ɂ��邩�A�{�^���̈�(�^�X�N���j���[(����/��߂�))�ɂ��邩��Ԃ�
	*
	* @param   wk		
	*
	* @retval  �ҏW�̈�ɂ���Ƃ�TRUE��Ԃ�	
	*/
//------------------------------------------------------------------
BOOL PMSI_GetEditAreaOrButton( const PMS_INPUT_WORK* wk )
{
  if(    wk->input_mode == PMSI_MODE_SINGLE     ///< �P��P���[�h
      || wk->input_mode == PMSI_MODE_DOUBLE )   ///< �P��Q���[�h
  {
	  if(    wk->main_seq == SEQ_EDW_BUTTON_KEYWAIT
        || wk->main_seq == SEQ_EDW_TO_SUBPROC_OK
        || wk->main_seq == SEQ_EDW_TO_SUBPROC_CANCEL
        || wk->main_seq == SEQ_EDW_RETURN_SUBPROC )
      return FALSE;
  }
  else  // if( wk->input_mode == PMSI_MODE_SENTENCE )    ///< ���̓��[�h
  {
    if(    wk->main_seq == SEQ_EDS_BUTTON_KEYWAIT
        || wk->main_seq == SEQ_EDS_TO_SUBPROC_OK
        || wk->main_seq == SEQ_EDS_TO_SUBPROC_CANCEL
        || wk->main_seq == SEQ_EDS_RETURN_SUBPROC )
	    return FALSE;
  }
  return TRUE;
}

//------------------------------------------------------------------
/**
	* �J�e�S���J�[�\���ʒu�擾
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
	* �J�e�S�����̗L���P�ꑍ����Ԃ�
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
		// ���X�g�̍ŏ�i�͋󗓂Ƃ��邽�߁A�������{�Q�@2009/11/14 by nakahiro
		return PMSI_DATA_GetGroupEnableWordCount( wk->dwk, wk->category_pos ) + PMSI_DUMMY_LABEL_NUM;
	}
	else
	{
    return PMSI_SEARCH_GetResultCount( wk->swk ) + PMSI_DUMMY_LABEL_NUM;
//		return PMSI_DATA_GetInitialEnableWordCount( wk->dwk, wk->category_pos );
	}
}

//------------------------------------------------------------------
/**
	* �J�e�S�����̗L���P��̓��A�w��C���f�b�N�X�̒P����o�b�t�@�ɃR�s�[����
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
    PMSI_SEARCH_GetResultString( wk->swk, word_num, buf );
	}
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �P�ꃊ�X�g�E�B���h�E�J�[�\�����W
 *
 *	@param	const PMS_INPUT_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
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
int PMSI_GetWordWinLinePos( const PMS_INPUT_WORK* wk )
{
	return get_wordwin_linepos(&(wk->word_win));
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

//-----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	const PMS_INPUT_WORK* wk 
 *
 *	@retval
 */
//-----------------------------------------------------------------------------
int PMSI_GetTalkWindowType( const PMS_INPUT_WORK* wk )
{
	return PMSI_PARAM_GetWindowType( wk->input_param );
}

//------------------------------------------------------------------
/**
	* ���j���[�J�[�\���̈ʒu��Ԃ�
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

//-----------------------------------------------------------------------------
/**
 *	@brief  �Z���e���X�ύX�}�����[�h�t���O���擾
 *
 *	@param	const PMS_INPUT_WORK* wk ���[�N
 *
 *	@retval TRUE : �}�����[�h
 */
//-----------------------------------------------------------------------------
BOOL PMSI_GetLockFlag( const PMS_INPUT_WORK* wk )
{
  return PMSI_PARAM_GetLockFlag( wk->input_param );
}


//-----------------------------------------------------------------------------
/**
 *	@brief  TBC���擾
 *
 *	@param	const PMS_INPUT_WORK* wk 
 *
 *	@retval GFL_TCBSYS* TCB
 */
//-----------------------------------------------------------------------------
GFL_TCBSYS* PMSI_GetTcbSystem( const PMS_INPUT_WORK* wk )
{
	return wk->tcbSys;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ������ʁi�J�e�S�����C�j�V�����j�œ��͂��ꂽ��������󂯎��
 *
 *	@param	const PMS_INPUT_WORK* wk ���[�N
 *	@param	out_buf [OUT] �R�s�[��
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSI_GetInputWord( const PMS_INPUT_WORK* wk, STRBUF* out_buf )
{
  PMSI_SEARCH_GetInputWord( wk->swk, out_buf );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ������ʁi�J�e�S�����C�j�V�����j�œ��͂��ꂽ��������N���A����
 *
 *	@param	PMS_INPUT_WORK* wk ���[�N
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSI_ClearInputWord( PMS_INPUT_WORK* wk )
{
  PMSI_SEARCH_ClearWord( wk->swk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �X�N���[���f�[�^�擾
 *
 *	@param	const PMS_INPUT_WORK * wk ���[�N
 *	@param	* line      ���C��
 *	@param	* line_max  ���C���ő�l
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSI_GetWorkScrollData( const PMS_INPUT_WORK * wk, u16 * line, u16 * line_max )
{
	*line = wk->word_win.line;
	*line_max = wk->word_win.line_max;
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �������ʐ����擾
 *
 *	@param	const PMS_INPUT_WORK* wk ���[�N
 *
 *	@retval ��
 */
//-----------------------------------------------------------------------------
u32 PMSI_GetSearchResultCount( const PMS_INPUT_WORK* wk )
{
  return PMSI_SEARCH_GetResultCount( wk->swk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���͂���Ă��镶�����𓾂�
 *
 *	@param	const PMS_INPUT_WORK* wk ���[�N
 *
 *	@retval ���͂���Ă��镶����
 */
//-----------------------------------------------------------------------------
u8 PMSI_GetSearchCharNum( const PMS_INPUT_WORK* wk )
{
  return PMSI_SEARCH_GetCharNum( wk->swk );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  �������ʂ𕶎���Ŏ擾
 *
 *	@param	const PMS_INPUT_WORK* wk ���[�N
 *	@param	result_idx ��������ID
 *	@param	dst_buf [OUT] �R�s�[�� 
 *
 *	@retval none
 */
//-----------------------------------------------------------------------------
void PMSI_GetSearchResultString( const PMS_INPUT_WORK* wk, u32 result_idx, STRBUF* dst_buf )
{
  PMSI_SEARCH_GetResultString( wk->swk, result_idx, dst_buf );
}

//-----------------------------------------------------------------------------
/**
 *	@brief  ���͂Ƃ��Ċ������Ă��邩�`�F�b�N
 *
 *	@param	const PMS_INPUT_WORK* wk ���[�N
 *
 *	@retval ���͂Ƃ��Ċ������Ă����TRUE
 */
//-----------------------------------------------------------------------------
BOOL PMSI_CheckInputComplete( const PMS_INPUT_WORK* wk )
{
  return check_input_complete( (PMS_INPUT_WORK*)wk );  // const�͂����̓C�������Acheck_input_complete��wk��ύX���ĂȂ�����܂��悵�Ƃ���
}

