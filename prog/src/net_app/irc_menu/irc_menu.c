//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_meu.c
 *	@brief	�ԊO���~�j�Q�[���@���j���[���
 *	@author	Toru=Nagihashi
 *	@data		2009.05.11
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	lib
#include <gflib.h>

//	system
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"

//	module
#include "infowin/infowin.h"

#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"
#include "system/bmp_winframe.h"
#include "net_app/compatible_irc_sys.h"

//	archive
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_irc_compatible.h"

#include "net_app/irc_menu.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	���y
//=====================================
#define MENU_SE_DECIDE	(SEQ_SE_DECIDE1)
#define MENU_SE_CANCEL	(SEQ_SE_CANCEL1)


//-------------------------------------
///	�p���b�g
//=====================================
enum{	
	// ���C�����BG
	IRC_MENU_BG_PAL_M_00 = 0,//�t�H���g
	IRC_MENU_BG_PAL_M_01,		// �t���[���p�p���b�g
	IRC_MENU_BG_PAL_M_02,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_03,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_04,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_05,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_06,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_07,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_08,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_09,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_10,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_11,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_12,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_13,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_14,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_M_15,		// INFOWIN

	// �T�u���BG
	IRC_MENU_BG_PAL_S_00 = 0,	//�t�H���g
	IRC_MENU_BG_PAL_S_01,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_02,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_03,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_04,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_05,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_06,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_07,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_08,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_09,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_10,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_11,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_12,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_13,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_14,		// �g�p���ĂȂ�
	IRC_MENU_BG_PAL_S_15,		// �g�p���ĂȂ�
};

//-------------------------------------
///	���o�[
//=====================================
#define INFOWIN_PLT_NO		(IRC_MENU_BG_PAL_M_15)
#define INFOWIN_BG_FRAME	(GFL_BG_FRAME0_M)

//-------------------------------------
///	����
//=====================================
#define TEXTSTR_PLT_NO				(IRC_MENU_BG_PAL_S_00)
#define TEXTSTR_BUFFER_LENGTH	(255)

//-------------------------------------
///	�ʒu
//=====================================
#define	MSGWND_MSG_X	(4)
#define	MSGWND_MSG_Y	(7)
#define	MSGWND_MSG_W	(24)
#define	MSGWND_MSG_H	(6)

#define	MSGWND_AURA_X	(8)
#define	MSGWND_AURA_Y	(7)
#define	MSGWND_AURA_W	(16)
#define	MSGWND_AURA_H	(3)

#define	MSGWND_RHYTHM_X	(8)
#define	MSGWND_RHYTHM_Y	(12)
#define	MSGWND_RHYTHM_W	(16)
#define	MSGWND_RHYTHM_H	(3)

#define	MSGWND_RETURN_X	(1)
#define	MSGWND_RETURN_Y	(20)
#define	MSGWND_RETURN_W	(30)
#define	MSGWND_RETURN_H	(3)

//-------------------------------------
///	�J�E���g
//=====================================
#define TOCH_COUNTER_MAX	(30*5)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	BG�֌W
//=====================================
typedef struct
{
	GFL_ARCUTIL_TRANSINFO	frame_char;
	GFL_ARCUTIL_TRANSINFO	frame_char2;
	GFL_TCB						*p_vblank_task;
} GRAPHIC_BG_WORK;
//-------------------------------------
///	�`��֌W
//=====================================
typedef struct 
{
	int dummy;
} GRAPHIC_WORK;
//-------------------------------------
///	�����Ǘ�
//=====================================
typedef struct 
{
	GFL_FONT*				  p_font;
  GFL_MSGDATA*			p_msg;
  PRINT_QUE*        p_print_que;
	WORDSET*					p_wordset;
} MSG_WORK;

//-------------------------------------
///	���b�Z�[�W�\���E�B���h�E
//=====================================
typedef struct 
{
	GFL_BMPWIN*				p_bmpwin;
	PRINT_UTIL        print_util;
	STRBUF*						p_strbuf;
} MSGWND_WORK;

//-------------------------------------
///	�{�^��
//=====================================
#define BUTTON_MAX	(4)
typedef struct
{	
	u32				strID;
	u16				x;			//�J�n�_X
	u16				y;			//�J�n�_Y
	u16				w;			//�J�n�_����̕�
	u16				h;
} BUTTON_SETUP;
typedef struct 
{
	GFL_BUTTON_MAN			*p_btn;
	GFL_BMPWIN					*p_bmpwin[BUTTON_MAX];
	GFL_UI_TP_HITTBL		hit_tbl[BUTTON_MAX+1];//+1�͏I���R�[�h��
	const	 BUTTON_SETUP *cp_btn_setup_tbl;
	u16			btn_num;
	u16			frm;
	u16			select_btn_id;
	u16			is_touch;
} BUTTON_WORK;

//-------------------------------------
///	�����f�f���j���[���C�����[�N
//=====================================
typedef struct _IRC_MENU_MAIN_WORK IRC_MENU_MAIN_WORK;
typedef void (*SEQ_FUNCTION)( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
struct _IRC_MENU_MAIN_WORK
{
	//���W���[��
	GRAPHIC_WORK		grp;
	GRAPHIC_BG_WORK	bg;
	MSG_WORK				msg;
	MSGWND_WORK			msgwnd;
	BUTTON_WORK			btn;

	//�V�[�P���X�Ǘ�
	SEQ_FUNCTION		seq_function;
	u16		seq;
	u16		dummy;
	BOOL	is_end;

	//�p�����[�^
	IRC_MENU_PARAM	*p_param;

	//���̑�
	u32		start_ms;
	u32		select;
	u32		now_ms;
	BOOL	is_send;
};

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//proc
static GFL_PROC_RESULT IRC_MENU_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_MENU_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_MENU_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
//grp
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk );
//BG
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void Graphic_BG_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//MSG_WORK
static void MSG_Init( MSG_WORK *p_wk, HEAPID heapID );
static void MSG_Exit( MSG_WORK *p_wk );
static BOOL MSG_Main( MSG_WORK *p_wk );
static GFL_FONT*	MSG_GetFont( const MSG_WORK *cp_wk );
static PRINT_QUE* MSG_GetPrintQue( const MSG_WORK *cp_wk );
static const GFL_MSGDATA * MSG_GetMsgDataConst( const MSG_WORK *cp_wk );
static WORDSET * MSG_GetWordSet( const MSG_WORK *cp_wk );
//MSG_WINDOW
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe,
		u8 x, u8 y, u8 w, u8 h, u8 plt, HEAPID heapID );
static void MSGWND_Exit( MSGWND_WORK* p_wk );
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, MSG_WORK *p_msg );
static void MSGWND_Print( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y );
static void MSGWND_PrintColor( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y, PRINTSYS_LSB color );
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 number, u16 buff_id, u16 x, u16 y );
static GFL_BMPWIN * MSGWND_GetBmpWin( const MSGWND_WORK *cp_wk );
//SEQ
static void SEQ_Change( IRC_MENU_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function );
static void SEQ_End( IRC_MENU_MAIN_WORK *p_wk );
//SEQ_FUNC
static void SEQFUNC_Connect( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_Select( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_NextProc( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_DisConnect( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
static void SEQFUNC_End( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq );
//BTN
static void BUTTON_Init( BUTTON_WORK *p_wk, u8 frm, const	 BUTTON_SETUP *cp_btn_setup_tbl, u8 tbl_max, const MSG_WORK *cp_msg, GFL_ARCUTIL_TRANSINFO frame_char, u8 plt, HEAPID heapID );
static void BUTTON_Exit( BUTTON_WORK *p_wk );
static void BUTTON_Main( BUTTON_WORK *p_wk );
static BOOL BUTTON_IsTouch( const BUTTON_WORK *cp_wk, u32 *p_btnID );
static void Button_TouchCallBack( u32 btnID, u32 event, void *p_param );
//�ėp
static void MainModules( IRC_MENU_MAIN_WORK *p_wk );
static BOOL TP_GetRectTrg( const BUTTON_SETUP *cp_btn );
static BOOL TouchReturnBtn( void );

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�ԊO���~�j�Q�[�����j���[�p�v���b�N�f�[�^
//=====================================
const GFL_PROC_DATA IrcMenu_ProcData	= 
{	
	IRC_MENU_PROC_Init,
	IRC_MENU_PROC_Main,
	IRC_MENU_PROC_Exit,
};
//-------------------------------------
///	BG�V�X�e��
//=====================================
typedef enum 
{
	GRAPHIC_BG_FRAME_M_INFOWIN,
	GRAPHIC_BG_FRAME_M_TEXT,
	GRAPHIC_BG_FRAME_M_BTN,
	GRAPHIC_BG_FRAME_S_TEXT,
	GRAPHIC_BG_FRAME_S_BACK,
	GRAPHIC_BG_FRAME_MAX
} GRAPHIC_BG_FRAME;
static const u32 sc_bgcnt_frame[ GRAPHIC_BG_FRAME_MAX ] = 
{
	INFOWIN_BG_FRAME, GFL_BG_FRAME1_M, GFL_BG_FRAME2_M, GFL_BG_FRAME0_S, GFL_BG_FRAME1_S,
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
	// GRAPHIC_BG_FRAME_M_TEXT
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_M_BTN
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
	},
	// GRAPHIC_BG_FRAME_S_TEXT
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

};

//-------------------------------------
///	�{�^���͈�
//=====================================
enum{	
	BTNID_COMATIBLE,
	BTNID_RETURN,

	BTNID_MAX,
	BTNID_NULL	= BTNID_MAX,
};
static const BUTTON_SETUP	sc_btn_setp_tbl[BTNID_MAX]	=
{	
	//list�I��
	{	
		COMPATI_LIST_000,
		MSGWND_AURA_X,
		MSGWND_AURA_Y,
		MSGWND_AURA_W,
		MSGWND_AURA_H,
	},
	//�߂�
	{	
		COMPATI_BTN_000,
		MSGWND_RETURN_X,
		MSGWND_RETURN_Y,
		MSGWND_RETURN_W,
		MSGWND_RETURN_H,
	},
};

//=============================================================================
/**
 *			PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO���~�j�Q�[�����j���[	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_MENU_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_MENU_MAIN_WORK	*p_wk;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCCOMPATIBLE,  0x20000 );
	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(IRC_MENU_MAIN_WORK), HEAPID_IRCCOMPATIBLE );
	GFL_STD_MemClear( p_wk, sizeof(IRC_MENU_MAIN_WORK) );
	p_wk->p_param	= p_param;

	//���W���[��������
	MSG_Init( &p_wk->msg, HEAPID_IRCCOMPATIBLE );
	GRAPHIC_Init( &p_wk->grp, HEAPID_IRCCOMPATIBLE );
	GRAPHIC_BG_Init( &p_wk->bg, HEAPID_IRCCOMPATIBLE );
	{
		GAME_COMM_SYS_PTR comm	= NULL;
		if( p_wk->p_param->p_gamesys )
		{	
			comm	= GAMESYSTEM_GetGameCommSysPtr(p_wk->p_param->p_gamesys);
		}
		INFOWIN_Init( INFOWIN_BG_FRAME, INFOWIN_PLT_NO, comm, HEAPID_IRCCOMPATIBLE );
	}
	
	BUTTON_Init( &p_wk->btn, sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_BTN],
			sc_btn_setp_tbl, NELEMS(sc_btn_setp_tbl), &p_wk->msg, p_wk->bg.frame_char, IRC_MENU_BG_PAL_M_01, HEAPID_IRCCOMPATIBLE );

	MSGWND_Init( &p_wk->msgwnd, sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT],
			MSGWND_MSG_X, MSGWND_MSG_Y, MSGWND_MSG_W, MSGWND_MSG_H, IRC_MENU_BG_PAL_M_00, HEAPID_IRCCOMPATIBLE );
	BmpWinFrame_Write( p_wk->msgwnd.p_bmpwin, WINDOW_TRANS_ON, 
					GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->bg.frame_char), IRC_MENU_BG_PAL_M_01 );

	GFL_BG_SetVisible( sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT], FALSE );

	switch( p_wk->p_param->mode )
	{	
	case IRCMENU_MODE_INIT:
		SEQ_Change( p_wk, SEQFUNC_Select );
		break;

	case IRCMENU_MODE_RETURN:
		GFL_BG_SetVisible( sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT], FALSE );
		SEQ_Change( p_wk, SEQFUNC_DisConnect );
		break;

	default:
		GF_ASSERT( 0 );
	}

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO���~�j�Q�[�����j���[	���C���v���Z�X�j������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_MENU_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_MENU_MAIN_WORK	*p_wk;

	p_wk	= p_work;

	//���W���[���j��
	BUTTON_Exit( &p_wk->btn );

	MSGWND_Exit( &p_wk->msgwnd );
	INFOWIN_Exit();
	GRAPHIC_BG_Exit( &p_wk->bg );
	GRAPHIC_Exit( &p_wk->grp );
	MSG_Exit( &p_wk->msg );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );
	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_IRCCOMPATIBLE );
	
	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO���~�j�Q�[�����j���[	���C���v���Z�X���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_MENU_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
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

	IRC_MENU_MAIN_WORK	*p_wk;
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
		GF_ASSERT_MSG( 0, "IRC_MENU_PROC_Main��SEQ�G���[ %d", *p_seq );
	}

	MainModules( p_wk );
	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *				GRAPHIC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�`��֌W������
 *
 *	@param	GRAPHIC_WORK* p_wk	���[�N
 *	@param	heapID					�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Init( GRAPHIC_WORK* p_wk, HEAPID heapID )
{
	//���[�N�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );

	// �f�B�X�v���CON
	GFL_DISP_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
	GFL_DISP_SetDispOn();

	// VRAM�o���N�ݒ�
	{
		static const GFL_DISP_VRAM sc_vramSetTable =
		{
			GX_VRAM_BG_128_A,						// ���C��2D�G���W����BG
			GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
			GX_VRAM_SUB_BG_128_C,				// �T�u2D�G���W����BG
			GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
			GX_VRAM_OBJ_NONE,						// ���C��2D�G���W����OBJ
			GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
			GX_VRAM_SUB_OBJ_16_I,       // �T�u2D�G���W����OBJ
			GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
			GX_VRAM_TEX_NONE,						// �e�N�X�`���C���[�W�X���b�g
			GX_VRAM_TEXPLTT_NONE,				// �e�N�X�`���p���b�g�X���b�g
			GX_OBJVRAMMODE_CHAR_1D_128K,		
			GX_OBJVRAMMODE_CHAR_1D_128K,		
		};
		GFL_DISP_SetBank( &sc_vramSetTable );
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief	�`��֌W�j��
 *
 *	@param	GRAPHIC_WORK* p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Exit( GRAPHIC_WORK* p_wk )
{

}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��֌W�`�揈��
 *
 *	@param	GRAPHIC_WORK* p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Draw( GRAPHIC_WORK* p_wk )
{
}

//=============================================================================
/**
 *					GRAPHIC_BG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG�`��	������
 *
 *	@param	GRAPHIC_BG_WORK* p_wk	���[�N
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK* p_wk, HEAPID heapID )
{
	int i;

	//�a�f�V�X�e���N��
	GFL_BG_Init( heapID );
	GFL_BMPWIN_Init( heapID );

	//�a�f���[�h�ݒ�
	{
		static const GFL_BG_SYS_HEADER sc_bg_sys_header = 
		{
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
		};	
		GFL_BG_SetBGMode( &sc_bg_sys_header );
	}

	//�a�f�R���g���[���ݒ�
	{
		int i;

		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{
			GFL_BG_SetBGControl( sc_bgcnt_frame[i], &sc_bgcnt_data[i], GFL_BG_MODE_TEXT );
			GFL_BG_ClearFrame( sc_bgcnt_frame[i] );
			GFL_BG_SetVisible( sc_bgcnt_frame[i], VISIBLE_ON );
		}
	}

	//�ǂݍ��ݐݒ�
	{	
		GFL_BG_SetBackGroundColor( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_BTN], GX_RGB(31,31,31) );
		GFL_BG_SetBackGroundColor( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], GX_RGB(31,31,31) );
		GFL_BG_SetBackGroundColor( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_BACK], GX_RGB(31,31,31) );

		GFL_BG_FillCharacter( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_BTN], 0x00, 1, 0 );
		p_wk->frame_char	= BmpWinFrame_GraphicSetAreaMan(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_BTN], IRC_MENU_BG_PAL_M_01, MENU_TYPE_SYSTEM, heapID);

		GFL_BG_FillCharacter( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_TEXT], 0x00, 1, 0 );
		p_wk->frame_char2	= BmpWinFrame_GraphicSetAreaMan(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_TEXT], IRC_MENU_BG_PAL_M_01, MENU_TYPE_SYSTEM, heapID);
	}
	
	//VBlackTask�o�^
	p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(Graphic_BG_VBlankTask, p_wk, 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG�`��	�j��
 *
 *	@param	GRAPHIC_BG_WORK* p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK* p_wk )
{	
	//�^�X�N�j��
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

	//���\�[�X�j��
	{	
		GFL_BG_FreeCharacterArea(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_BTN],
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_char),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->frame_char));
		GFL_BG_FillCharacterRelease(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_BTN], 1,0);

		GFL_BG_FreeCharacterArea(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_TEXT],
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->frame_char2),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->frame_char2));
		GFL_BG_FillCharacterRelease(sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_TEXT], 1,0);
	}

	//BG�ݒ�j��
	{	
		int i;
		// BG�R���g���[���j��
		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{
			GFL_BG_FreeBGControl( sc_bgcnt_frame[i] );
		}
	}

	// BG�V�X�e���j��
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();
}
//----------------------------------------------------------------------------
/**
 *	@brief	VBlank�p�^�X�N
 *
 *	@param	GFL_TCB *p_tcb	�^�X�NControl�u���b�N
 *	@param	*p_work					���[�N�A�h���X
 *
 */
//-----------------------------------------------------------------------------
static void Graphic_BG_VBlankTask( GFL_TCB *p_tcb, void *p_work )
{	
	GFL_BG_VBlankFunc();

}

//=============================================================================
/**
 *					MSG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	MSG�֌W��ݒ�
 *
 *	@param	MSG_WORK *p_wk	���[�N
 *	@param	heapID					�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void MSG_Init( MSG_WORK *p_wk, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MSG_WORK) );

	GFL_FONTSYS_Init();

	p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
    NARC_font_large_nftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );

	p_wk->p_print_que = PRINTSYS_QUE_Create( heapID );

	p_wk->p_msg = GFL_MSG_Create(
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_irc_compatible_dat, heapID );

	p_wk->p_wordset	= WORDSET_Create( heapID );

	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	MSG�֌W��j��
 *
 *	@param	MSG_WORK *p_wk	���[�N
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
 *	@brief	MSG�֌W	���C������
 *
 *	@param	MSG_WORK *p_wk	���[�N
 *
 * @retval  BOOL	�������I�����Ă����TRUE�^����ȊO��FALSE
 *
 */
//-----------------------------------------------------------------------------
static BOOL MSG_Main( MSG_WORK *p_wk )
{	
	return PRINTSYS_QUE_Main( p_wk->p_print_que );
}

//----------------------------------------------------------------------------
/**
 *	@brief	FONT���擾
 *
 *	@param	const MSG_WORK *cp_wk		���[�N
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
 *	@brief	PRINTQUE���擾
 *
 *	@param	const MSG_WORK *cp_wk		���[�N
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
 *	@brief	MSGDATA���擾
 *
 *	@param	const MSG_WORK *cp_wk		���[�N
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
 *	@brief	WORDSET���擾
 *
 *	@param	const MSG_WORK *cp_wk ���[�N
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
 *	@brief	���b�Z�[�W�\����	������
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	bgframe						�\������BG��
 *	@param	x									�J�nX�ʒu�i�L�����P�ʁj
 *	@param	y									�J�nY�ʒu�i�L�����P�ʁj
 *	@param	w									���i�L�����P�ʁj
 *	@param	h									�����i�L�����P�ʁj
 *	@param	plt								�p���b�g�ԍ�
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe,
		u8 x, u8 y, u8 w, u8 h, u8 plt, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(MSGWND_WORK) );
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( bgframe, x, y, w, h, plt, GFL_BMP_CHRAREA_GET_B );
	p_wk->p_strbuf	= GFL_STR_CreateBuffer( TEXTSTR_BUFFER_LENGTH, heapID );
	PRINT_UTIL_Setup( &p_wk->print_util, p_wk->p_bmpwin );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );
}
//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\����	�j��
 *
 *	@param	MSGWND_WORK* p_wk		���[�N
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
 *	@brief	���b�Z�[�W�\����	���C������
 *
 *	@param	MSGWND_WORK *p_wk		���[�N
 *	@param	MSG_WORK	*p_msg		���b�Z�[�W
 *
 *	@retval  BOOL	�]�����I����Ă���ꍇ��TRUE�^�I����Ă��Ȃ��ꍇ��FALSE
 *
 */
//-----------------------------------------------------------------------------
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, MSG_WORK *p_msg )
{	
	return PRINT_UTIL_Trans( &p_wk->print_util, p_msg->p_print_que );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\���ʂɕ�����\��
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	MSG_WORK *cp_msg	�����Ǘ�
 *	@param	strID							����ID
 *	@param	x									�J�n�ʒuX
 *	@param	y									�J�n�ʒuY
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

	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0xF );	

	//������쐬
	GFL_MSG_GetString( cp_msgdata, strID, p_wk->p_strbuf );

	//�\��
	PRINT_UTIL_Print( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\���ʂɕ�����\��
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	MSG_WORK *cp_msg	�����Ǘ�
 *	@param	strID							����ID
 *	@param	x									�J�n�ʒuX
 *	@param	y									�J�n�ʒuY
 *	@param	color							�F
 *
 */
//-----------------------------------------------------------------------------
static void MSGWND_PrintColor( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y, PRINTSYS_LSB color )
{	
	const GFL_MSGDATA* cp_msgdata;
	PRINT_QUE*	p_que;
	GFL_FONT*		p_font;

	cp_msgdata	= MSG_GetMsgDataConst( cp_msg );
	p_que		= MSG_GetPrintQue( cp_msg );
	p_font	= MSG_GetFont( cp_msg );

	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0xF );	

	//������쐬
	GFL_MSG_GetString( cp_msgdata, strID, p_wk->p_strbuf );

	//�\��
	PRINT_UTIL_PrintColor( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font, color );
}

//----------------------------------------------------------------------------
/**
 *	@brief	���b�Z�[�W�\���ʂɐ��l��������\��
 *
 *	@param	MSGWND_WORK* p_wk	���[�N
 *	@param	MSG_WORK *cp_msg	�����Ǘ�
 *	@param	strID							����ID
 *	@param	number						���l
 *	@param	buff_id						���[�h�Z�b�g�o�^�o�b�t�@
 *	@param	x									�J�n�ʒuX
 *	@param	y									�J�n�ʒuY
 */
//-----------------------------------------------------------------------------
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 number, u16 buff_id, u16 x, u16 y )
{
	const GFL_MSGDATA* cp_msgdata;
	WORDSET *p_wordset;
	
	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0xF );	

	//���W���[���擾
	p_wordset		= MSG_GetWordSet( cp_msg );
	cp_msgdata	= MSG_GetMsgDataConst( cp_msg );

	//���l�����[�h�Z�b�g�ɓo�^
	WORDSET_RegisterNumber(	p_wordset, buff_id, number, 3, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );

	//���̕�����ɐ��l��K�p
	{	
		STRBUF	*p_strbuf;
		p_strbuf	= GFL_MSG_CreateString( cp_msgdata, strID );
		WORDSET_ExpandStr( p_wordset, p_wk->p_strbuf, p_strbuf );
		GFL_STR_DeleteBuffer( p_strbuf );
	}

	//�\��
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
 *	@brief	BMPWIN���擾
 *
 *	@param	const MSGWND_WORK *cp_wk	���[�N
 *
 *	@return	BMPWIN
 */
//-----------------------------------------------------------------------------
static GFL_BMPWIN * MSGWND_GetBmpWin( const MSGWND_WORK *cp_wk )
{	
	return cp_wk->p_bmpwin;
}

//=============================================================================
/**
 *				SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ�`�F���W
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	SEQ_FUNCTION										SEQ�֐�
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Change( IRC_MENU_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PROC�I��
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( IRC_MENU_MAIN_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
}
//=============================================================================
/**
 *				SEQFUNC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�ڑ��V�[�P���X
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Connect( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum{	
		SEQ_MSG_STARTNET,
		SEQ_CONNECT,
		SEQ_SEND_STATUS,
		SEQ_TIMING,
		SEQ_MSG_CONNECT,
		SEQ_CHANGE_SELECT,
		SEQ_SENDMENU,
		SEQ_RECVMENU,
		SEQ_SENDRETURNMENU,
		SEQ_RECVRETURNMENU,
		SEQ_NEXTPROC,
	};

	switch( *p_seq )
	{	
	case SEQ_MSG_STARTNET:
		if(COMPATIBLE_IRC_InitWait( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_CONNECT;
		}
		break;

	case SEQ_CONNECT:
		if( COMPATIBLE_IRC_ConnextWait( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_SEND_STATUS;
		}

		if( TP_GetRectTrg( &sc_btn_setp_tbl[BTNID_RETURN] ) )
		{
			PMSND_PlaySystemSE( MENU_SE_CANCEL );
			COMPATIBLE_IRC_Cancel( p_wk->p_param->p_irc );
			SEQ_Change( p_wk, SEQFUNC_End );
		}
		break;

	case SEQ_SEND_STATUS:
		if(COMPATIBLE_MENU_SendStatusData( p_wk->p_param->p_irc, p_wk->p_param->p_gamesys ) )
		{	
			COMPATIBLE_MENU_GetStatusData( p_wk->p_param->p_irc, p_wk->p_param->p_you_status  );
			*p_seq	= SEQ_TIMING;
		}

		if( TP_GetRectTrg( &sc_btn_setp_tbl[BTNID_RETURN] ) )
		{
			PMSND_PlaySystemSE( MENU_SE_CANCEL );
			COMPATIBLE_IRC_Cancel( p_wk->p_param->p_irc );
			SEQ_Change( p_wk, SEQFUNC_End );
		}
		break;

	case SEQ_TIMING:
		if( COMPATIBLE_IRC_TimingSyncWait( p_wk->p_param->p_irc, COMPATIBLE_TIMING_NO_MENU_START ) )
		{	
			*p_seq	= SEQ_MSG_CONNECT;
		}
		break;

	case SEQ_MSG_CONNECT:
		MSGWND_Print( &p_wk->msgwnd, &p_wk->msg, COMPATI_STR_002, 0, 0  );
		*p_seq	= SEQ_SENDMENU;
		break;
		
	case SEQ_SENDMENU:
		p_wk->is_send	= TRUE;
		if( COMPATIBLE_MENU_SendMenuData( p_wk->p_param->p_irc, p_wk->now_ms, p_wk->select ) )
		{	
			*p_seq	= SEQ_RECVMENU;
		}
		break;

	case SEQ_RECVMENU:
		if( COMPATIBLE_MENU_RecvMenuData( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_SENDRETURNMENU;
		}
		break;

	case SEQ_SENDRETURNMENU:
		if( COMPATIBLE_MENU_SendReturnMenu( p_wk->p_param->p_irc ) )
		{
			*p_seq	= SEQ_RECVRETURNMENU;
		}
		break;

	case SEQ_RECVRETURNMENU:
		if( COMPATIBLE_MENU_RecvReturnMenu( p_wk->p_param->p_irc ) )
		{
			*p_seq	= SEQ_NEXTPROC;
		}
		break;

	case SEQ_NEXTPROC:
		SEQ_Change( p_wk, SEQFUNC_NextProc );
		break;
	};
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I���V�[�P���X
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Select( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum{	
		SEQ_INIT,
		SEQ_SELECT,
		SEQ_MSG,
		SEQ_NEXTPROC,	
	};
	u32 ret;
#if 0
	//�������ȊO��M���āA����PROC�����߂鏈��
	if( *p_seq != SEQ_INIT  )
	{	
		if( COMPATIBLE_MENU_RecvMenuData( p_wk->p_param->p_irc ) )
		{	
			u32 you_ms;
			u32 you_select;
			COMPATIBLE_MENU_GetMenuData( p_wk->p_param->p_irc, &you_ms, &you_select );
			OS_TPrintf( "YOU PROC%d MS%d\n", you_select, you_ms );
			OS_TPrintf( "MY PROC%d MS%d\n", p_wk->select, p_wk->now_ms );
			//�������I��łȂ�������A����ɏ]��
			if( p_wk->select == BTNID_NULL )
			{	
				p_wk->select	= you_select;
			}
			else
			{
				//�������I��ł�����A�b�����������ɂ������������Ȃ�ΐe�@�ɏ]��
				if( p_wk->now_ms < you_ms )
				{	
	
				}
				else if( p_wk->now_ms > you_ms )
				{	
					p_wk->select	= you_select;
				}
				else
				{	
					if( GFL_NET_IsParentMachine() )
					{	
	
					}
					else
					{	
							p_wk->select	= you_select;
					}
				}
			}
			COMPATIBLE_MENU_SendReturnMenu( p_wk->p_param->p_irc );
			if( p_wk->is_send )
			{	
				*p_seq	= SEQ_RECVMENU;
			}
			else
			{	
				*p_seq	= SEQ_NEXTPROC;
			}
		}
	}
#endif
	switch( *p_seq )
	{	
	case SEQ_INIT:
		p_wk->select	= BTNID_NULL;
		p_wk->is_send	= FALSE;
		GFL_BG_SetVisible( sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT], FALSE );
		p_wk->start_ms	= OS_TicksToMilliSeconds32( OS_GetTick() );
		*p_seq	= SEQ_SELECT;
		break;

	case SEQ_SELECT:
		BUTTON_Main( &p_wk->btn );
		if( BUTTON_IsTouch( &p_wk->btn,  &p_wk->select ) )
		{	
			switch( p_wk->select )
			{	
			case BTNID_COMATIBLE:
				PMSND_PlaySystemSE( MENU_SE_DECIDE );
				*p_seq	= SEQ_MSG;
				break;
			case BTNID_RETURN:
				PMSND_PlaySystemSE( MENU_SE_CANCEL );
				SEQ_Change( p_wk, SEQFUNC_End );
				break;
			};
		}
		p_wk->now_ms	= OS_TicksToMilliSeconds32( OS_GetTick() ) - p_wk->start_ms;
		break;

	case SEQ_MSG:
		MSGWND_Print( &p_wk->msgwnd, &p_wk->msg, COMPATI_STR_003, 0, 0  );
		GFL_BG_SetVisible( sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT], TRUE );
		SEQ_Change( p_wk, SEQFUNC_Connect );
		break;
#if 0
	case SEQ_SENDMENU:
		p_wk->is_send	= TRUE;
		if( COMPATIBLE_MENU_SendMenuData( p_wk->p_param->p_irc, p_wk->now_ms, p_wk->select ) )
		{	
			*p_seq	= SEQ_RECVMENU;
		}
		break;

	case SEQ_RECVMENU:
		if( COMPATIBLE_MENU_RecvReturnMenu( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_NEXTPROC;
		}
		break;

	case SEQ_NEXTPROC:
		SEQ_Change( p_wk, SEQFUNC_NextProc );
		break;
#endif
	};

	if( TouchReturnBtn() && *p_seq >= SEQ_MSG )
	{
		COMPATIBLE_IRC_Cancel( p_wk->p_param->p_irc );
		PMSND_PlaySystemSE( MENU_SE_CANCEL );
		SEQ_Change( p_wk, SEQFUNC_End );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	����PROC�ɂƂԂ��߁A�I��
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_NextProc( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_END,
	};

	switch( *p_seq )
	{	
	case SEQ_INIT:
		p_wk->p_param->select	= IRCMENU_SELECT_COMPATIBLE;
		*p_seq	= SEQ_END;
		break;

	case SEQ_END:
		SEQ_End( p_wk );
		break;
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ؒf�V�[�P���X
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_DisConnect( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum{	
		SEQ_NET_DISCONNECT,
		SEQ_NET_EXIT,
		SEQ_CHANGE_MENU,
	};

	switch( *p_seq )
	{	
	case SEQ_NET_DISCONNECT:
		if( COMPATIBLE_IRC_DisConnextWait( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_NET_EXIT;
		}
		break;

	case SEQ_NET_EXIT:
		if( COMPATIBLE_IRC_ExitWait( p_wk->p_param->p_irc) )
		{	
			*p_seq	= SEQ_CHANGE_MENU;
		}
		break;

	case SEQ_CHANGE_MENU:
		SEQ_Change( p_wk, SEQFUNC_Select );
		break;

	};
}

//----------------------------------------------------------------------------
/**
 *	@brief	�I���V�[�P���X
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *	@param	*p_seq													�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( IRC_MENU_MAIN_WORK *p_wk, u16 *p_seq )
{	
	enum{	
		SEQ_JUNCTION,
		SEQ_NET_DISCONNECT,
		SEQ_NET_EXIT,
		SEQ_END,
	};

	switch( *p_seq )
	{
	case SEQ_JUNCTION:
		if( COMPATIBLE_IRC_IsConnext(p_wk->p_param->p_irc) )
		{	
			*p_seq	= SEQ_NET_DISCONNECT;
		}
		else
		{	
			*p_seq	= SEQ_END;
		}
		break;

	case SEQ_NET_DISCONNECT:
		if( COMPATIBLE_IRC_DisConnextWait( p_wk->p_param->p_irc ) )
		{	
			*p_seq	= SEQ_NET_EXIT;
		}
		break;

	case SEQ_NET_EXIT:
		if( COMPATIBLE_IRC_ExitWait( p_wk->p_param->p_irc) )
		{	
			*p_seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		p_wk->p_param->select	= IRCMENU_SELECT_RETURN;
		SEQ_End( p_wk );
		break;
	}
}
//=============================================================================
/**
 *			adapter
 */
//=============================================================================

//----------------------------------------------------------------------------
/**
 *	@brief	PROC�؂�ւ����ɏ�����郂�W���[��	���C������
 *
 *
 *	@param	IRC_MENU_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void MainModules( IRC_MENU_MAIN_WORK *p_wk )
{	
	INFOWIN_Update();
	if( MSG_Main( &p_wk->msg ) )
	{	
		MSGWND_Main( &p_wk->msgwnd, &p_wk->msg );
	}
}

//=============================================================================
/**
 *						BUTTON
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^��������	
 *
 *	@param	BUTTON_WORK *p_wk				���[�N
 *	@param	frm											�{�^����\������t���[��
 *	@param	GFL_RECT *cp_rect_tbl		�{�^���̐ݒ�e�[�u��(���̂������Ă�������)
 *	@param	tbl_max									�e�[�u���̌�
 *	@param	cp_msg									�����Ǘ�
 *	@param	frame_char							�t���[���Ɏg���L����
 *	@param	plt											�p���b�g
 *	@param	HEAPID									�q�[�vID
 */
//-----------------------------------------------------------------------------
static void BUTTON_Init( BUTTON_WORK *p_wk, u8 frm, const	 BUTTON_SETUP *cp_btn_setup_tbl, u8 tbl_max, const MSG_WORK *cp_msg, GFL_ARCUTIL_TRANSINFO frame_char, u8 plt, HEAPID heapID )
{	
	//�G���[
	GF_ASSERT_MSG( tbl_max < BUTTON_MAX, "�{�^�����������ł�BUTTON_MAX�̒�`��ς��Ă�������", tbl_max );

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(BUTTON_WORK) );
	p_wk->cp_btn_setup_tbl	= cp_btn_setup_tbl;
	p_wk->btn_num			= tbl_max;
	p_wk->frm					= frm;

	//HITTBL���쐬�i�L�����P�ʂ��h�b�g�P�ʂɁj
	{	
		int i;
		const	 BUTTON_SETUP *cp_setup;
		for( i = 0; i < p_wk->btn_num; i++ )
		{	
			cp_setup	= &cp_btn_setup_tbl[i];
			p_wk->hit_tbl[i].rect.top			= (cp_setup->y)*8+1;
			p_wk->hit_tbl[i].rect.bottom	= (cp_setup->y + cp_setup->h)*8-1;
			p_wk->hit_tbl[i].rect.left		= (cp_setup->x)*8+1;
			p_wk->hit_tbl[i].rect.right		= (cp_setup->x + cp_setup->w)*8-1;
		}
		p_wk->hit_tbl[i].rect.top	= GFL_UI_TP_HIT_END;
	}

	//�{�^���}�l�[�W���쐬
	p_wk->p_btn	= GFL_BMN_Create( p_wk->hit_tbl, Button_TouchCallBack, p_wk, heapID );

	//BMPWIN�쐬�����e����������
	{	
		int i;
		const	 BUTTON_SETUP *cp_setup;
		STRBUF *p_strbuf;
		for( i = 0; i < p_wk->btn_num; i++ )
		{	
			//BMPWIN�쐬
			cp_setup	= &cp_btn_setup_tbl[i];
			p_wk->p_bmpwin[i]	= GFL_BMPWIN_Create( frm, cp_setup->x,
					cp_setup->y, cp_setup->w, cp_setup->h, plt, GFL_BMP_CHRAREA_GET_B );

			//BMPWIN�]��
			GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 0 );
			GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin[i] );

			//�g�`��
			BmpWinFrame_Write( p_wk->p_bmpwin[i], WINDOW_TRANS_ON, 
					GFL_ARCUTIL_TRANSINFO_GetPos(frame_char), plt );

			//�����쐬�`��
			p_strbuf	= GFL_MSG_CreateString( MSG_GetMsgDataConst(cp_msg), cp_setup->strID );
			GFL_FONTSYS_SetColor( 0xf, 0xe, 0 );

			//�����ɔz�u
			{	
				u32	w;
				u32 h;
				w	= PRINTSYS_GetStrWidth( p_strbuf, MSG_GetFont(cp_msg), 0 );
				h	= PRINTSYS_GetStrHeight( p_strbuf, MSG_GetFont(cp_msg) );
				PRINTSYS_Print( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin[i]), 
					cp_setup->w*4-w/2, cp_setup->h*4-h/2, p_strbuf, MSG_GetFont(cp_msg) );
			}
			GFL_STR_DeleteBuffer( p_strbuf );

			GFL_BMPWIN_TransVramCharacter( p_wk->p_bmpwin[i] );

		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���j��
 *
 *	@param	BUTTON_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void BUTTON_Exit( BUTTON_WORK *p_wk )
{	
	//BMPWIN�j��
	{	
		int i;
		for( i = 0; i < p_wk->btn_num; i++ )
		{	
			GFL_BMPWIN_Delete(p_wk->p_bmpwin[i]);
		}
	}

	//BMN�j��
	GFL_BMN_Delete( p_wk->p_btn );

	GFL_STD_MemClear( p_wk, sizeof(BUTTON_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����C������
 *
 *	@param	BUTTON_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void BUTTON_Main( BUTTON_WORK *p_wk )
{
	p_wk->is_touch	= GFL_BMN_Main( p_wk->p_btn );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^�����^�b�`���ꂽ���ǂ���
 *
 *	@param	const BUTTON_WORK *cp_wk	���[�N
 *	@param	*p_btnID									�^�b�`���ꂽ�{�^��ID�󂯎��
 *
 *	@retval	TRUE�Ȃ�Ή����ꂽ
 *	@retval	FALSE�Ȃ�Ή�����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL BUTTON_IsTouch( const BUTTON_WORK *cp_wk, u32 *p_btnID )
{	
	if( cp_wk->is_touch )
	{	
		if( p_btnID )
		{	
			*p_btnID	= cp_wk->select_btn_id;
		}
		return TRUE;
	}

	return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�{�^���������ꂽ���̃R�[���o�b�N
 *
 *	@param	u32 btnID	�{�^��ID
 *	@param	event			�C�x���g
 *	@param	*p_param	Create�œn���ꂽ�A�h���X
 *
 */
//-----------------------------------------------------------------------------
static void Button_TouchCallBack( u32 btnID, u32 event, void *p_param )
{	
	BUTTON_WORK *p_wk;
	p_wk	= p_param;

	if( event == GFL_BMN_EVENT_TOUCH )
	{	
		p_wk->select_btn_id	= btnID;
	}
}
//=============================================================================
/**
 *				�ėp
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	��`����Trg�������ǂ���
 *
 *	@param	const GFL_RECT *cp_rect	��`
 *	@param	*p_trg									���W�󂯎��
 *
 *	@retval	TRUE�^�b�`����
 *	@retval	FALSE�^�b�`���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL TP_GetRectTrg( const BUTTON_SETUP *cp_btn )
{	
	u32 x, y;

	//Cont���ŁA��`���̂Ƃ�
	if( GFL_UI_TP_GetPointTrg( &x, &y ) )
	{	
		if( ((u32)( x - cp_btn->x*8) < (u32)(cp_btn->w*8))
				&	((u32)( y - cp_btn->y*8) < (u32)(cp_btn->h*8))
			)
		{
			return TRUE;
		}
	}

	return FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�߂�{�^�������������ǂ���
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

