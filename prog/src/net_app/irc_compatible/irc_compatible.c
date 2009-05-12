//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_aura_main.c
 *	@brief	�ԊO���~�j�Q�[��
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
#include "msg/msg_irc_aura.h"

//	aura
#include "net_app/irc_aura.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�p���b�g
//=====================================
enum{	
	// ���C�����BG
	IRC_COMPATIBLE_BG_PAL_M_00 = 0,//
	IRC_COMPATIBLE_BG_PAL_M_01,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_02,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_03,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_04,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_05,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_06,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_07,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_08,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_09,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_10,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_11,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_12,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_13,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_14,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_M_15,		// INFOWIN


	// �T�u���BG
	IRC_COMPATIBLE_BG_PAL_S_00 = 0,	//�t�H���g
	IRC_COMPATIBLE_BG_PAL_S_01,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_02,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_03,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_04,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_05,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_06,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_07,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_08,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_09,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_10,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_11,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_12,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_13,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_14,		// �g�p���ĂȂ�
	IRC_COMPATIBLE_BG_PAL_S_15,		// �g�p���ĂȂ�
};

//-------------------------------------
///	���o�[
//=====================================
#define INFOWIN_PLT_NO		(IRC_COMPATIBLE_BG_PAL_M_15)
#define INFOWIN_BG_FRAME	(GFL_BG_FRAME0_M)

//-------------------------------------
///	����
//=====================================
#define TEXTSTR_PLT_NO				(IRC_COMPATIBLE_BG_PAL_S_00)
#define TEXTSTR_BUFFER_LENGTH	(255)

//-------------------------------------
///	�ʒu
//=====================================
#define	MSGTEXT_WND_X	(1)
#define	MSGTEXT_WND_Y	(18)
#define	MSGTEXT_WND_W	(30)
#define	MSGTEXT_WND_H	(5)

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
	int dummy;
} GRAPHIC_BG_WORK;
//-------------------------------------
///	3D�`���
//=====================================
typedef struct 
{
	GFL_G3D_CAMERA		*p_camera;
} GRAPHIC_3D_WORK;
//-------------------------------------
///	�`��֌W
//=====================================
typedef struct 
{
	GRAPHIC_BG_WORK		gbg;
	GRAPHIC_3D_WORK		g3d;
	GFL_TCB						*p_vblank_task;
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
typedef struct {
	GFL_BMPWIN*				p_bmpwin;
	PRINT_UTIL        print_util;
	STRBUF*						p_strbuf;
} MSGWND_WORK;

//-------------------------------------
///	�I�[���`�F�b�N���C�����[�N
//=====================================
typedef struct _IRC_COMPATIBLE_MAIN_WORK IRC_COMPATIBLE_MAIN_WORK;
typedef void (*SEQ_FUNCTION)( IRC_COMPATIBLE_MAIN_WORK *p_wk, u16 *p_seq );
struct _IRC_COMPATIBLE_MAIN_WORK
{
	//�O���t�B�b�N���W���[��
	GRAPHIC_WORK		grp;
	MSG_WORK				msg;
	MSGWND_WORK			msgwnd;

	//�f�o�b�O�p
	MSGWND_WORK			d_msgwnd;

	u32	cnt;			//�J�E���^

	//�V�[�P���X�Ǘ�
	SEQ_FUNCTION		seq_function;
	u16		seq;
	BOOL is_end;

	//����
	GFL_POINT	left;
	GFL_POINT	right;
};


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//proc
static GFL_PROC_RESULT IRC_COMPATIBLE_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_COMPATIBLE_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_COMPATIBLE_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
//grp
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk );
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//BG
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
//3d
static void GRAPHIC_3D_Init( GRAPHIC_3D_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_3D_Exit( GRAPHIC_3D_WORK *p_wk );
static void GRAPHIC_3D_StartDraw( GRAPHIC_3D_WORK *p_wk );
static void GRAPHIC_3D_EndDraw( GRAPHIC_3D_WORK *p_wk );
static void Graphic_3d_SetUp( void );
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
		u8 x, u8 y, u8 w, u8 h, HEAPID heapID );
static void MSGWND_Exit( MSGWND_WORK* p_wk );
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, MSG_WORK *p_msg );
static void MSGWND_Print( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 x, u16 y );
static void MSGWND_PrintNumber( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, u32 strID, u16 number, u16 buff_id, u16 x, u16 y );
//SEQ
static void SEQ_Change( IRC_COMPATIBLE_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function );
static void SEQ_End( IRC_COMPATIBLE_MAIN_WORK *p_wk );
//SEQ_FUNC

//�ėp
static BOOL TP_GetRectTrg( const GFL_RECT *cp_rect, GFL_POINT *p_trg );
static BOOL TP_GetRectCont( const GFL_RECT *cp_rect, GFL_POINT *p_cont );


//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�I�[���`�F�b�N�p�v���b�N�f�[�^
//=====================================
const GFL_PROC_DATA IrcCompatible_ProcData	= 
{	
	IRC_COMPATIBLE_PROC_Init,
	IRC_COMPATIBLE_PROC_Main,
	IRC_COMPATIBLE_PROC_Exit,
};
//-------------------------------------
///	BG�V�X�e��
//=====================================
typedef enum 
{
	GRAPHIC_BG_FRAME_M_INFOWIN,
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
	// GRAPHIC_BG_FRAME_M_BACK
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
	},

};

//-------------------------------------
///	�^�b�`�͈�
//=====================================
static const GFL_RECT	sc_left	=
{	
	0, 2*8, 14*8, 20*8
};
#if 0
static const GFL_RECT	sc_right	=
{	
	19, 2, 33, 20
};
#else
static const GFL_RECT	sc_right	=
{	
	9*8, 2*8, 23*8, 20*8
};
#endif


//=============================================================================
/**
 *			PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[���`�F�b�N	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_COMPATIBLE_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_COMPATIBLE_MAIN_WORK	*p_wk;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCCOMPATIBLE, 0x10000 );
	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(IRC_COMPATIBLE_MAIN_WORK), HEAPID_IRCCOMPATIBLE );
	GFL_STD_MemClear( p_wk, sizeof(IRC_COMPATIBLE_MAIN_WORK) );

	//���W���[��������
	MSG_Init( &p_wk->msg, HEAPID_IRCCOMPATIBLE );
	GRAPHIC_Init( &p_wk->grp, HEAPID_IRCCOMPATIBLE );
	INFOWIN_Init( INFOWIN_BG_FRAME, INFOWIN_PLT_NO, HEAPID_IRCCOMPATIBLE );
	MSGWND_Init( &p_wk->msgwnd, sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TEXT],
			MSGTEXT_WND_X, MSGTEXT_WND_Y, MSGTEXT_WND_W, MSGTEXT_WND_H, HEAPID_IRCCOMPATIBLE );
	MSGWND_Init( &p_wk->d_msgwnd, sc_bgcnt_frame[GRAPHIC_BG_FRAME_S_TEXT],
			0, 0, 18, 4, HEAPID_IRCCOMPATIBLE );

	SEQ_Change( p_wk, NULL );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[���`�F�b�N	���C���v���Z�X�j������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_COMPATIBLE_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_COMPATIBLE_MAIN_WORK	*p_wk;

	p_wk	= p_work;

	//���W���[���j��
	MSGWND_Exit( &p_wk->d_msgwnd );
	MSGWND_Exit( &p_wk->msgwnd );
	INFOWIN_Exit();
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
 *	@brief	�I�[���`�F�b�N	���C���v���Z�X���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_COMPATIBLE_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
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

	IRC_COMPATIBLE_MAIN_WORK	*p_wk;
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

		//p_wk->seq_function( p_wk, &p_wk->seq );
		if( p_wk->is_end )
		{	
			*p_seq	= SEQ_FADEIN_START;
		}

		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
		{
			SEQ_End( p_wk );
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
		GF_ASSERT_MSG( 0, "IRC_COMPATIBLE_PROC_Main��SEQ�G���[ %d", *p_seq );
	}

	INFOWIN_Update();
	if( MSG_Main( &p_wk->msg ) )
	{	
		MSGWND_Main( &p_wk->msgwnd, &p_wk->msg );
		MSGWND_Main( &p_wk->d_msgwnd, &p_wk->msg );
	}

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

	//�`�惂�W���[��
	GRAPHIC_BG_Init( &p_wk->gbg, heapID );
//	GRAPHIC_3D_Init( &p_wk->g3d, heapID );

	//VBlackTask�o�^
	p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(Graphic_VBlankTask, p_wk, 0 );
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
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

//	GRAPHIC_3D_Exit( &p_wk->g3d );
	GRAPHIC_BG_Exit( &p_wk->gbg );
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

//----------------------------------------------------------------------------
/**
 *	@brief	VBlank�p�^�X�N
 *
 *	@param	GFL_TCB *p_tcb	�^�X�NControl�u���b�N
 *	@param	*p_work					���[�N�A�h���X
 *
 */
//-----------------------------------------------------------------------------
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work )
{	
	GRAPHIC_WORK* p_wk	= p_work;
	GRAPHIC_BG_VBlankFunction( &p_wk->gbg );
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
		GFL_BG_SetBackGroundColor( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_BACK], GX_RGB(31,31,31) );
		GFL_BG_SetBackGroundColor( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_TEXT], GX_RGB(31,31,31) );
		GFL_BG_SetBackGroundColor( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_S_BACK], GX_RGB(31,31,31) );
	}
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
	int i;

	// BG�R���g���[���j��
	for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
	{
		GFL_BG_FreeBGControl( sc_bgcnt_frame[i] );
	}

	// BG�V�X�e���j��
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();
}

//----------------------------------------------------------------------------
/**
 *	@brief	BG�`��	VBlack�֐�
 *
 *	@param	GRAPHIC_BG_WORK *p_wk ���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk )
{	
	GFL_BG_VBlankFunc();
}

//=============================================================================
/**
 *					GRAPHIC_3D
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�RD���̏�����
 *
 *	@param	p_wk			���[�N
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_3D_Init( GRAPHIC_3D_WORK *p_wk, HEAPID heapID )
{
	static const VecFx32 sc_CAMERA_PER_POS		= { 0,0,FX32_CONST(5) };
	static const VecFx32 sc_CAMERA_PER_UP			= { 0,FX32_ONE,0 };
	static const VecFx32 sc_CAMERA_PER_TARGET	= { 0,0,FX32_CONST( 0 ) };

	enum{	
		CAMERA_PER_FOVY	=	(40),
		CAMERA_PER_ASPECT =	(FX32_ONE * 4 / 3),
		CAMERA_PER_NEAR	=	(FX32_ONE * 1),
		CAMERA_PER_FER	=	(FX32_ONE * 800),
		CAMERA_PER_SCALEW	=(0),
	};

	GFL_G3D_Init( GFL_G3D_VMANLNK, GFL_G3D_TEX128K,
			GFL_G3D_VMANLNK, GFL_G3D_PLT32K, 0, heapID, Graphic_3d_SetUp );
	p_wk->p_camera = GFL_G3D_CAMERA_CreatePerspective( CAMERA_PER_FOVY, CAMERA_PER_ASPECT,
				CAMERA_PER_NEAR, CAMERA_PER_FER, CAMERA_PER_SCALEW, 
				&sc_CAMERA_PER_POS, &sc_CAMERA_PER_UP, &sc_CAMERA_PER_TARGET, heapID );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�RD���̔j��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_3D_Exit( GRAPHIC_3D_WORK *p_wk )
{
	GFL_G3D_CAMERA_Delete( p_wk->p_camera );
	GFL_G3D_Exit();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��J�n
 *	
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_3D_StartDraw( GRAPHIC_3D_WORK *p_wk )
{	
	GFL_G3D_DRAW_Start();
	GFL_G3D_CAMERA_Switching( p_wk->p_camera );
	GFL_G3D_DRAW_SetLookAt();
}

//----------------------------------------------------------------------------
/**
 *	@brief	�`��I��
 *
 *	@param	p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_3D_EndDraw( GRAPHIC_3D_WORK *p_wk )
{	
	GFL_G3D_DRAW_End();
}

//----------------------------------------------------------------------------
/**
 *	@brief	���Z�b�g�A�b�v�R�[���o�b�N�֐�
 */
//-----------------------------------------------------------------------------
static void Graphic_3d_SetUp( void )
{
	// �R�c�g�p�ʂ̐ݒ�(�\�����v���C�I���e�B�[)
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_BG0, VISIBLE_ON );
	G2_SetBG0Priority(0);

	// �e��`�惂�[�h�̐ݒ�(�V�F�[�h���A���`�G�C���A�X��������)
	G3X_SetShading( GX_SHADING_HIGHLIGHT );
	G3X_AntiAlias( FALSE );
	G3X_AlphaTest( FALSE, 0 );	// �A���t�@�e�X�g�@�@�I�t
	G3X_AlphaBlend( TRUE );		// �A���t�@�u�����h�@�I��
	G3X_EdgeMarking( FALSE );
	G3X_SetFog( FALSE, GX_FOGBLEND_COLOR_ALPHA, GX_FOGSLOPE_0x8000, 0 );

	// �N���A�J���[�̐ݒ�
	G3X_SetClearColor(GX_RGB(0,0,0),31,0x7fff,63,FALSE);	//color,alpha,depth,polygonID,fog
	// �r���[�|�[�g�̐ݒ�
	G3_ViewPort(0, 0, 255, 191);

	// ���C�g�ݒ�
	{
		static const GFL_G3D_LIGHT sc_GFL_G3D_LIGHT[] = 
		{
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
			{
				{ 0, -FX16_ONE, 0 },
				GX_RGB( 16,16,16),
			},
		};
		int i;
		
		for( i=0; i<NELEMS(sc_GFL_G3D_LIGHT); i++ ){
			GFL_G3D_SetSystemLight( i, &sc_GFL_G3D_LIGHT[i] );
		}
	}

	//�����_�����O�X���b�v�o�b�t�@
	GFL_G3D_SetSystemSwapBufferMode( GX_SORTMODE_AUTO, GX_BUFFERMODE_Z );
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
		GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_irc_aura_dat, heapID );

	p_wk->p_wordset	= WORDSET_Create( heapID );

	GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_SUB_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
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
 *	@param	heapID						�q�[�vID
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
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	

	//������쐬
	GFL_MSG_GetString( cp_msgdata, strID, p_wk->p_strbuf );

	//�\��
	PRINT_UTIL_Print( &p_wk->print_util, p_que, x, y, p_wk->p_strbuf, p_font );
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
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	

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

//=============================================================================
/**
 *				SEQ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ�`�F���W
 *
 *	@param	IRC_COMPATIBLE_MAIN_WORK *p_wk	���[�N
 *	@param	SEQ_FUNCTION										SEQ�֐�
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Change( IRC_COMPATIBLE_MAIN_WORK *p_wk, SEQ_FUNCTION	seq_function )
{	
	p_wk->seq_function	= seq_function;
	p_wk->seq	= 0;
}

//----------------------------------------------------------------------------
/**
 *	@brief	PROC�I��
 *
 *	@param	IRC_COMPATIBLE_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_End( IRC_COMPATIBLE_MAIN_WORK *p_wk )
{	
	p_wk->is_end	= TRUE;
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
static BOOL TP_GetRectTrg( const GFL_RECT *cp_rect, GFL_POINT *p_trg )
{	
	u32 x, y;
	BOOL ret;

	//Cont���ŁA��`���̂Ƃ�
	if( GFL_UI_TP_GetPointTrg( &x, &y ) )
	{	
		if( ((u32)( x - cp_rect->left) < (u32)(cp_rect->right - cp_rect->left))
				&	((u32)( y - cp_rect->top) < (u32)(cp_rect->bottom - cp_rect->top))
			)
		{
			//�󂯎�肪���݂����������ĕԂ�
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
 *	@brief	��`���Ƀ^�b�`�������ǂ���
 *
 *	@param	const GFL_RECT *cp_rect	��`
 *	@param	*p_cont									���W�󂯎��
 *
 *	@retval	TRUE�^�b�`����
 *	@retval	FALSE�^�b�`���Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL TP_GetRectCont( const GFL_RECT *cp_rect, GFL_POINT *p_cont )
{	
	u32 x, y;
	BOOL ret;

	//Cont���ŁA��`���̂Ƃ�
	if( GFL_UI_TP_GetPointCont( &x, &y ) )
	{	
		if( ((u32)( x - cp_rect->left) < (u32)(cp_rect->right - cp_rect->left))
				&	((u32)( y - cp_rect->top) < (u32)(cp_rect->bottom - cp_rect->top))
			)
		{
			//�󂯎�肪���݂����������ĕԂ�
			if( p_cont )
			{	
				p_cont->x	= x;
				p_cont->y	= y;
			}
			return TRUE;
		}
	}

	return FALSE;
}

