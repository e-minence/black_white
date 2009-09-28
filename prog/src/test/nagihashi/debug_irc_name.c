//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		debug_irc_name.c
 *	@brief	���O���������^���l�`�F�b�N�v���b�N
 *	@author	Toru=Nagihashi
 *	@data		2009.05.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	lib
#include <gflib.h>
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"

//	module
#include "savedata/config.h"
#include "print/gf_font.h"
#include "print/printsys.h"

//	proc
#include "app/name_input.h"

//	archive
#include "arc_def.h"
#include "font/font.naix"

//	mine
#include "debug_irc_name.h"

//	etc
#include <wchar.h>					//wcslen

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	����
//=====================================
#define TEXTSTR_PLT_NO				(0)
#define TEXTSTR_BUFFER_LENGTH	(255)

//-------------------------------------
///	�ʒu
//=====================================
#define	MSGWND_PLAYER1_X	(6)
#define	MSGWND_PLAYER1_Y	(4)
#define	MSGWND_PLAYER1_W	(13)
#define	MSGWND_PLAYER1_H	(2)

#define	MSGWND_PLAYER2_X	(20)
#define	MSGWND_PLAYER2_Y	(4)
#define	MSGWND_PLAYER2_W	(13)
#define	MSGWND_PLAYER2_H	(2)

#define	MSGWND_SCORE_X		(12)
#define	MSGWND_SCORE_Y		(8)
#define	MSGWND_SCORE_W		(10)
#define	MSGWND_SCORE_H		(2)

//-------------------------------------
///		MSG_FONT
//=====================================
typedef enum {	
	MSG_FONT_TYPE_LARGE,
	MSG_FONT_TYPE_SMALL,
}MSG_FONT_TYPE;

//-------------------------------------
///	���b�Z�[�W�\���E�B���h�E
//=====================================
enum {
	MSGWNDID_PLAYER1,
	MSGWNDID_PLAYER2,
	MSGWNDID_SCORE,

	MSGWNDID_MAX,
};

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
///	�`��֌W
//=====================================
typedef struct 
{
	GRAPHIC_BG_WORK		bg;

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
///	���C�����[�N
//=====================================
typedef struct
{
	//�O���t�B�b�N���W���[��
	GRAPHIC_WORK		grp;
	MSG_WORK				msg;
	MSGWND_WORK			msgwnd[MSGWNDID_MAX];
	BOOL						is_start_modules;

	CONFIG	config;
	STRBUF	*p_player1_name;
	STRBUF	*p_player2_name;
	STRBUF	*p_score;

	void	*p_param_adrs;


	//	�X�R�A
	u32	score;

}IRCNAME_MAIN_WORK;
//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//proc
static GFL_PROC_RESULT DEBUG_IRC_NAME_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT DEBUG_IRC_NAME_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT DEBUG_IRC_NAME_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
//grp
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk );
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//BG
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
//MSG_WORK
static void MSG_Init( MSG_WORK *p_wk, MSG_FONT_TYPE font, HEAPID heapID );
static void MSG_Exit( MSG_WORK *p_wk );
static BOOL MSG_Main( MSG_WORK *p_wk );
static GFL_FONT*	MSG_GetFont( const MSG_WORK *cp_wk );
static PRINT_QUE* MSG_GetPrintQue( const MSG_WORK *cp_wk );
static const GFL_MSGDATA * MSG_GetMsgDataConst( const MSG_WORK *cp_wk );
//MSG_WINDOW
static void MSGWND_Init( MSGWND_WORK* p_wk, u8 bgframe,
		u8 x, u8 y, u8 w, u8 h, HEAPID heapID );
static void MSGWND_Exit( MSGWND_WORK* p_wk );
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, const MSG_WORK *cp_msg );
static void MSGWND_Print( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, STRBUF *p_strbuf, u16 x, u16 y );
//common
static void CreateTemporaryModules( IRCNAME_MAIN_WORK *p_wk, HEAPID heapID );
static void DeleteTemporaryModules( IRCNAME_MAIN_WORK *p_wk );
static void MainTemporaryModules( IRCNAME_MAIN_WORK *p_wk );
static STRBUF * DEBUGPRINT_CreateWideChar( const u16 *cp_str, HEAPID heapID );
static STRBUF * DEBUGPRINT_CreateWideCharNumber( const u16 *cp_str, int number, HEAPID heapID );
//score
static u32 RULE_CalcNameScore( const STRBUF	*cp_player1_name, const STRBUF	*cp_player2_name );
static u32 MATH_GetMostOnebit( u32 x, u8 bit );
//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	�v���b�N�f�[�^
//=====================================
const GFL_PROC_DATA DebugIrcName_ProcData	= 
{	
	DEBUG_IRC_NAME_PROC_Init,
	DEBUG_IRC_NAME_PROC_Main,
	DEBUG_IRC_NAME_PROC_Exit,
};
//-------------------------------------
///	BG�V�X�e��
//=====================================
typedef enum 
{
	GRAPHIC_BG_FRAME_M_TEXT,
	GRAPHIC_BG_FRAME_MAX
} GRAPHIC_BG_FRAME;
static const u32 sc_bgcnt_frame[ GRAPHIC_BG_FRAME_MAX ] = 
{
	 GFL_BG_FRAME0_M
};
static const GFL_BG_BGCNT_HEADER sc_bgcnt_data[ GRAPHIC_BG_FRAME_MAX ] = 
{
	// GRAPHIC_BG_FRAME_M_TEXT
	{
		0, 0, 0x800, 0,
		GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
		GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
		GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
	},
};
//-------------------------------------
///	�f�o�b�O���b�Z�[�W�f�[�^
//=====================================
enum
{	
	MSGID_SCORE,
};
static const u16 * scp_msg_tbl[]	=
{	
	L"�^���l�@%d�_",
};

//=============================================================================
/**
 *					proc
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
static GFL_PROC_RESULT DEBUG_IRC_NAME_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRCNAME_MAIN_WORK	*p_wk;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NAGI_DEBUG_SUB, 0x10000 );
	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(IRCNAME_MAIN_WORK), HEAPID_NAGI_DEBUG_SUB );
	GFL_STD_MemClear( p_wk, sizeof(IRCNAME_MAIN_WORK) );

	p_wk->p_player1_name	= GFL_STR_CreateBuffer( NAMEIN_PERSON_LENGTH+1, HEAPID_NAGI_DEBUG_SUB );
	p_wk->p_player2_name	= GFL_STR_CreateBuffer( NAMEIN_PERSON_LENGTH+1, HEAPID_NAGI_DEBUG_SUB );

	CONFIG_Init( &p_wk->config );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I�[���`�F�b�N	���C���v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_IRC_NAME_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRCNAME_MAIN_WORK	*p_wk;
	
	p_wk	= p_work;

	GFL_STR_DeleteBuffer( p_wk->p_player1_name );
	GFL_STR_DeleteBuffer( p_wk->p_player2_name );

	//Module�j��
	DeleteTemporaryModules( p_wk );
	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );
	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_NAGI_DEBUG_SUB );
	
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
static GFL_PROC_RESULT DEBUG_IRC_NAME_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	enum
	{	
		SEQ_PLAYER1_NAME_START,
		SEQ_PLAYER1_NAME_INPUT,
		SEQ_PLAYER1_NAME_END,
		SEQ_PLAYER2_NAME_START,
		SEQ_PLAYER2_NAME_INPUT,
		SEQ_PLAYER2_NAME_END,
		SEQ_INIT,
		SEQ_MAIN,
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};


	IRCNAME_MAIN_WORK	*p_wk;
	
	p_wk	= p_work;

	switch( *p_seq )
	{	
	case SEQ_PLAYER1_NAME_START:
		p_wk->p_param_adrs	= NameIn_ParamAllocMake(
				HEAPID_NAGI_DEBUG_SUB, NAMEIN_MYNAME, 0, NAMEIN_PERSON_LENGTH, &p_wk->config );
		*p_seq	= SEQ_PLAYER1_NAME_INPUT;
		break;

	case SEQ_PLAYER1_NAME_INPUT:
		GFL_PROC_SysCallProc( NO_OVERLAY_ID, &NameInputProcData, p_wk->p_param_adrs );
		*p_seq	= SEQ_PLAYER1_NAME_END;
		break;

	case SEQ_PLAYER1_NAME_END:
		{	
			NAMEIN_PARAM	*p_param	= p_wk->p_param_adrs;
			GFL_STR_CopyBuffer( p_wk->p_player1_name, p_param->strbuf );
			NameIn_ParamDelete( p_wk->p_param_adrs );
		}
		*p_seq	= SEQ_PLAYER2_NAME_START;
		break;

	case SEQ_PLAYER2_NAME_START:
		p_wk->p_param_adrs	= NameIn_ParamAllocMake(
				HEAPID_NAGI_DEBUG_SUB, NAMEIN_MYNAME, 0, NAMEIN_PERSON_LENGTH, &p_wk->config );
		*p_seq	= SEQ_PLAYER2_NAME_INPUT;
		break;

	case SEQ_PLAYER2_NAME_INPUT:
		GFL_PROC_SysCallProc( NO_OVERLAY_ID, &NameInputProcData, p_wk->p_param_adrs );
		*p_seq	= SEQ_PLAYER2_NAME_END;
		break;

	case SEQ_PLAYER2_NAME_END:
		{	
			NAMEIN_PARAM	*p_param	= p_wk->p_param_adrs;
			GFL_STR_CopyBuffer( p_wk->p_player2_name, p_param->strbuf );
			NameIn_ParamDelete( p_wk->p_param_adrs );
		}
		*p_seq	= SEQ_FADEOUT_START;
		break;

	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_INIT;
		}
		break;

	case SEQ_INIT:
		p_wk->score	= RULE_CalcNameScore( p_wk->p_player1_name, p_wk->p_player2_name );
		CreateTemporaryModules( p_wk, HEAPID_NAGI_DEBUG_SUB );
		*p_seq	= SEQ_MAIN;
		break;

	case SEQ_MAIN:
		MainTemporaryModules( p_wk );
		if( GFL_UI_TP_GetTrg() )
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

	}

	return GFL_PROC_RES_CONTINUE;
}

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
	static const GFL_DISP_VRAM sc_vramSetTable =
	{
		GX_VRAM_BG_128_A,						// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,				// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_64_E,						// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,       // �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,						// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_NONE,				// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_128K,		
		GX_OBJVRAMMODE_CHAR_1D_128K,		
	};

	//���[�N�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );

	//VRAM�N���A�[
	GFL_DISP_ClearVRAM( 0 );

	// VRAM�o���N�ݒ�
	GFL_DISP_SetBank( &sc_vramSetTable );

	// �f�B�X�v���CON
	GFL_DISP_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
	GFL_DISP_SetDispOn();

	//�\��
	GFL_DISP_GX_InitVisibleControl();

	//�`�惂�W���[��
	GRAPHIC_BG_Init( &p_wk->bg, heapID );


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

	GRAPHIC_BG_Exit( &p_wk->bg );
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
	GRAPHIC_BG_VBlankFunction( &p_wk->bg );
}

//=============================================================================
/**
 *	bg
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
 *	msg
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	MSG�֌W��ݒ�
 *
 *	@param	MSG_WORK *p_wk	���[�N
 *	@param	font						�t�H���g�^�C�v
 *	@param	heapID					�q�[�vID
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
				NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
		break;
	case MSG_FONT_TYPE_SMALL:
		p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
				NARC_font_small_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
		break;
	default:
		GF_ASSERT_MSG( 0, "MSGFONT_ERRO %d", font );
	};

	p_wk->p_print_que = PRINTSYS_QUE_Create( heapID );



	{	
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr, PALTYPE_MAIN_BG, TEXTSTR_PLT_NO*0x20, 0x20, heapID );
		GFL_BG_SetBackGroundColor( sc_bgcnt_frame[ GRAPHIC_BG_FRAME_M_TEXT], GX_RGB(31,31,31) );
	}
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

//=============================================================================
/**
 *	msgwnd
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
static BOOL MSGWND_Main( MSGWND_WORK *p_wk, const MSG_WORK *cp_msg )
{	
	return PRINT_UTIL_Trans( &p_wk->print_util, cp_msg->p_print_que );
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
static void MSGWND_Print( MSGWND_WORK* p_wk, const MSG_WORK *cp_msg, STRBUF *p_strbuf, u16 x, u16 y )
{	
	PRINT_QUE*	p_que;
	GFL_FONT*		p_font;

	p_que		= MSG_GetPrintQue( cp_msg );
	p_font	= MSG_GetFont( cp_msg );

	//��[����
	GFL_BMP_Clear( GFL_BMPWIN_GetBmp(p_wk->p_bmpwin), 0 );	

	//�\��
	PRINT_UTIL_Print( &p_wk->print_util, p_que, x, y, p_strbuf, p_font );
}
//=============================================================================
/**
 *				common
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	Module�쐬
 *
 *	@param	IRCNAME_MAIN_WORK *p_wk	���[�N
 *	@param	heapID									�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void CreateTemporaryModules( IRCNAME_MAIN_WORK *p_wk, HEAPID heapID )
{	
	p_wk->p_score	= DEBUGPRINT_CreateWideCharNumber( scp_msg_tbl[MSGID_SCORE], p_wk->score, heapID );

	GRAPHIC_Init( &p_wk->grp, heapID );

	MSG_Init( &p_wk->msg, MSG_FONT_TYPE_LARGE, heapID );

	MSGWND_Init( &p_wk->msgwnd[MSGWNDID_PLAYER1], sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT],
			MSGWND_PLAYER1_X, MSGWND_PLAYER1_Y, MSGWND_PLAYER1_W, MSGWND_PLAYER1_H, heapID );

	MSGWND_Init( &p_wk->msgwnd[MSGWNDID_PLAYER2], sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT],
			MSGWND_PLAYER2_X, MSGWND_PLAYER2_Y, MSGWND_PLAYER2_W, MSGWND_PLAYER2_H, heapID );

	MSGWND_Init( &p_wk->msgwnd[MSGWNDID_SCORE], sc_bgcnt_frame[GRAPHIC_BG_FRAME_M_TEXT],
			MSGWND_SCORE_X, MSGWND_SCORE_Y, MSGWND_SCORE_W, MSGWND_SCORE_H, heapID );


	MSGWND_Print( &p_wk->msgwnd[MSGWNDID_PLAYER1], &p_wk->msg, p_wk->p_player1_name, 0, 0 );
	MSGWND_Print( &p_wk->msgwnd[MSGWNDID_PLAYER2], &p_wk->msg, p_wk->p_player2_name, 0, 0 );
	MSGWND_Print( &p_wk->msgwnd[MSGWNDID_SCORE], &p_wk->msg, p_wk->p_score, 0, 0 );

	p_wk->is_start_modules	= TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief	Module�j��
 *
 *	@param	IRCNAME_MAIN_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void DeleteTemporaryModules( IRCNAME_MAIN_WORK *p_wk )
{	
	//���W���[���j��
	{	
		int i;
		for( i = 0; i < MSGWNDID_MAX; i++ )
		{	
			MSGWND_Exit( &p_wk->msgwnd[i] );
		}
	}

	MSG_Exit( &p_wk->msg );
	GRAPHIC_Exit( &p_wk->grp );

	GFL_STR_DeleteBuffer( p_wk->p_score );

	p_wk->is_start_modules	= FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief	Module���C��
 *
 *	@param	IRCNAME_MAIN_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void MainTemporaryModules( IRCNAME_MAIN_WORK *p_wk )
{	
	if( p_wk->is_start_modules )
	{	
		if( MSG_Main( &p_wk->msg ) )
		{	
			int i;
			for( i = 0; i < MSGWNDID_MAX; i++ )
			{	
				MSGWND_Main( &p_wk->msgwnd[i], &p_wk->msg );
			}
		}

		GRAPHIC_Draw( &p_wk->grp );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	Wide�������STRBUF�Ɋi�[
 *
 *	@param	const u16 *cp_char	���C�h������
 *	@param	heapID							�q�[�vID
 *
 *	@return	STRBUF
 */
//-----------------------------------------------------------------------------
static STRBUF * DEBUGPRINT_CreateWideChar( const u16 *cp_str, HEAPID heapID )
{	
	STRBUF	*p_strbuf;
	STRCODE	str[128];
	u16	strlen;

	//STRBUF�p�ɕϊ�
	strlen	= wcslen(cp_str);
	GFL_STD_MemCopy(cp_str, str, strlen*2);
	str[strlen]	= GFL_STR_GetEOMCode();

	//STRBUF�ɓ]��
	p_strbuf	= GFL_STR_CreateBuffer( strlen*2, heapID );
	GFL_STR_SetStringCode( p_strbuf, str);

	return p_strbuf;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���l��Wide�������STRBUF�Ɋi�[
 *
 *	@param	const u16 *cp_char	���C�h������
 *	@param	number							���l
 *	@param	heapID							�q�[�vID
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


//----------------------------------------------------------------------------
/**
 *	@brief	�^���l���v�Z
 *
 *	@param	STRBUF	*p_player1_name	���O�P
 *	@param	STRBUF	*p_player2_name	���O�Q
 *
 *	@return	�X�R�A
 */
//-----------------------------------------------------------------------------
static u32 RULE_CalcNameScore( const STRBUF	*cp_player1_name, const STRBUF	*cp_player2_name )
{	
	enum
	{	
		BIT_NUM	= 8,
	};

	const STRCODE	*cp_p1;
	const STRCODE	*cp_p2;
	
	u32 num1;
	u32 num2;
	u32 b1;
	u32 b2;
	u32 bit1;
	u32 bit2;
	u32 ans_cnt;
	u32 ans_max;
	u32 score;

	cp_p1	= GFL_STR_GetStringCodePointer( cp_player1_name );
	cp_p2	= GFL_STR_GetStringCodePointer( cp_player2_name );
	num1	= *cp_p1;
	num2	= *cp_p2;
	bit1	= MATH_GetMostOnebit( *cp_p1, BIT_NUM );
	bit2	= MATH_GetMostOnebit( *cp_p2, BIT_NUM );

	ans_max	= 0;
	ans_cnt	= 0;
	while( 1 )
	{
	 OS_Printf( "num1%d bit1%d num2%d bit2%d\n", *cp_p1, bit1, *cp_p2, bit2 );
	 if( bit1 == 0 )
	 {	
		cp_p1++;
		num1	= *cp_p1;
		if( num1 == GFL_STR_GetEOMCode() )
		{	
			break;
		}
		bit1	= MATH_GetMostOnebit( num1, BIT_NUM );
	 }
	 if( bit2 == 0 )
	 {	
		cp_p2++;
		num2	= *cp_p2;
		if( num2 == GFL_STR_GetEOMCode() )
		{	
			break;
		}
 		bit2	= MATH_GetMostOnebit( num2, BIT_NUM );
	 }

	 b1	= ((num1) >> bit1) & 0x1;
	 b2	= ((num2) >> bit2) & 0x1;

	 //bit�̈�v�����`�F�b�N
	 if( b1 == b2 )
	 {	
			ans_cnt++;
	 }
	 ans_max++;

	 //�r�b�g�����炷
	 bit1--;
	 bit2--;
	}
	
	score	= 100*ans_cnt/ans_max;
	OS_Printf( "�S�̂̃r�b�g%d ��v%d �_��\n", ans_max, ans_cnt, score );

	return score;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ŏ�ʂ��P�̃r�b�g�𐔓���
 *
 *	@param	u32 x		�l
 *	@param	u8 bit	�r�b�g��
 *
 *	@return	�ŏ�ʂ̂P�̃r�b�g�̈ʒu
 */
//-----------------------------------------------------------------------------
static u32 MATH_GetMostOnebit( u32 x, u8 bit )
{	
	int i;
	for( i = bit-1; i != 0; i-- )
	{	
		if( (x >> i) & 0x1 )
		{
			break;
		}
	}

	return i;
}
