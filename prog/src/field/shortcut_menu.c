//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		shortcut_menu.c
 *	@brief	�V���[�g�J�b�g���j���[����
 *	@author	Toru=Nagihashi
 *	@data		2009.10.20
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>
#include "system/gfl_use.h"

//�V�X�e��
#include "system/bmp_menu.h"
#include "system/bmp_menulist.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "font/font.naix"
#include "message.naix"
#include "fld_shortcut_menu_gra.naix"
#include "msg/msg_shortcut_menu.h"

//�Z�[�u�f�[�^
#include "system/shortcut_data.h"
#include "savedata/shortcut.h"

//�O�����J
#include "shortcut_menu.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�S��
//=====================================
//BG��
enum
{	
	BG_FRAME_SCROLL_M	= GFL_BG_FRAME1_M,
};
//�p���b�g�ԍ�
enum
{	
	//����BG
	PLT_BG_SCROLL_M	= 11,
	PLT_BG_SELECT_M	= 12,

	//����OBJ
	PLT_OBJ_M	= 2,
};
//BMPWIN�T�C�Y
#define SCROLL_BMPWIN_X	(1)
#define SCROLL_BMPWIN_Y	(12)
#define SCROLL_BMPWIN_W	(30)
#define SCROLL_BMPWIN_H	(12)
//�p���b�g�J���[
enum
{	
	COLOR_WHITE_LETTER	=	1,
	COLOR_WHITE_SHADOW	=	2,
	COLOR_RED_LETTER		=	3,
	COLOR_RED_SHADOW		=	4,
	COLOR_BLUE_LETTER		=	5,
	COLOR_BLUE_SHADOW		=	6,
	COLOR_DECIDE_LETTER	=	7,
	COLOR_DECIDE_SHADOW	=	8,

	COLOR_DECIDE_BACK		=	0xE,
	COLOR_BACK					=	0xF,
};

//-------------------------------------
///	OBJ
//=====================================
#define OBJ_CLWK_MAX	(128)
#define OBJ_UNIT_PRI	(0)

enum
{	
	RESID_PLT,
	RESID_CHR,
	RESID_CEL,
	RESID_MAX,
};

//-------------------------------------
///	���X�g����
//=====================================
//�V���[�g�J�b�g���j���[�̕���J������
#define LISTMOVE_START	(-192)
#define LISTMOVE_END		(0)
#define LISTMOVE_DIFF		(LISTMOVE_END-LISTMOVE_START)
#define LISTMOVE_SYNC		(5)

//-------------------------------------
///	�X�N���[��
//=====================================
//GetInput�̖߂�l
#define SCROLL_INPUT_NULL		(BMPMENULIST_NULL)
#define SCROLL_INPUT_CANCEL (BMPMENULIST_CANCEL)
//���[�h
typedef enum
{	
	SCROLL_MOVE_NORMAL,
	SCROLL_MOVE_INSERT,
} SCROLL_MOVE_MODE;

//-------------------------------------
///	SHORTCUTMENU
//=====================================
//SEQ
enum 
{
	MAINSEQ_NONE,					//�������Ă��Ȃ�
	MAINSEQ_OPEN_START,		//�J���J�n
	MAINSEQ_OPEN_WAIT,		//�J���҂�
	MAINSEQ_MAIN,					//���C��
	MAINSEQ_CLOSE_START,	//����J�n
	MAINSEQ_CLOSE_WAIT,		//����҂�
} ;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================

//-------------------------------------
///	SCROLL_WORK
//=====================================
typedef struct 
{
	BMP_MENULIST_DATA	*p_data;
	BMPMENULIST_WORK	*p_list;
	GFL_BMPWIN				*p_bmpwin;
	PRINT_QUE					*p_que;		//�v�����g�L���[
	PRINT_UTIL				util;			//�v�����g
	u32								select;
	u16								insert_list;			//�}������J�[�\���̃X�N���[��
	u16								insert_cursor;		//�}������J�[�\���̈ʒu
	SHORTCUT					*p_sv;
	SCROLL_MOVE_MODE	mode;	
	GFL_CLWK					*p_clwk;
	HEAPID						heapID;
	GFL_MSGDATA				*p_msg;		//���b�Z�[�W�f�[�^
	GFL_FONT					*p_font;	//�t�H���g
} SCROLL_WORK;

//-------------------------------------
///	�V���[�g�J�b�g���j���[
//=====================================
struct _SHORTCUTMENU_WORK
{
	SCROLL_WORK	scroll;	//�X�N���[���Ǘ�

	GFL_CLUNIT	*p_unit;	//OBJ���j�b�g
	u16					res[RESID_MAX];		//���\�[�X
	GFL_CLWK		*p_clwk;	//�}���p�J�[�\��

	GFL_MSGDATA *p_msg;		//���b�Z�[�W�f�[�^
	GFL_FONT		*p_font;	//�t�H���g
	PRINT_QUE		*p_que;		//�v�����g�L���[
	u16									seq;				//���C���V�[�P���X
	u16									cnt;				//�J�E���^�[		
	SHORTCUTMENU_INPUT	input;			//���s��������
	SHORTCUT_ID					shortcutID;	//�I�񂾃V���[�g�J�b�gID
	SHORTCUT_CURSOR			*p_cursor;	//�J�[�\��
};

//=============================================================================
/**
 *					PRIVATE
 */
//=============================================================================
//-------------------------------------
///	SCROLL
//=====================================
static void SCROLL_Init( SCROLL_WORK *p_wk, const SHORTCUT_CURSOR	*cp_cursor, GFL_MSGDATA *p_msg, GFL_FONT *p_font, PRINT_QUE *p_que, SHORTCUT *p_sv, GFL_CLWK *p_cursor, HEAPID heapID );
static void SCROLL_Exit( SCROLL_WORK *p_wk, SHORTCUT_CURSOR	*p_cursor );
static void SCROLL_Main( SCROLL_WORK *p_wk );
static BOOL SCROLL_PrintMain( SCROLL_WORK *p_wk );
static u32 SCROLL_GetInput( const SCROLL_WORK *cp_wk );
static void Scroll_MoveCursorCallBack( BMPMENULIST_WORK * p_wk, u32 param, u8 mode );
static void Scroll_CreateList( SCROLL_WORK *p_wk, u16 list_bak, u16 cursor_bak, HEAPID heapID );
static void Scroll_DeleteList( SCROLL_WORK *p_wk, u16 *p_list_bak, u16 *p_cursor_bak );

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�V���[�g�J�b�g���j���[	������
 *
 *	@param	mode						�N�����[�h
 *	@param	p_cursor				�J�[�\��
 *	@param	HEAPID heapID		�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
SHORTCUTMENU_WORK *SHORTCUTMENU_Init( SHORTCUTMENU_MODE mode, SHORTCUT_CURSOR *p_cursor, HEAPID heapID )
{	
	//���C�����[�N�쐬
	SHORTCUTMENU_WORK	*p_wk;
	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(SHORTCUTMENU_WORK) );
	GFL_STD_MemClear( p_wk, sizeof(SHORTCUTMENU_WORK) );
	p_wk->input				= SHORTCUTMENU_INPUT_NONE;
	p_wk->shortcutID	= SHORTCUT_ID_NULL;
	p_wk->p_cursor		= p_cursor;

	//BG������
	{	
		static const GFL_BG_BGCNT_HEADER bgcnt = 
		{
			0, 0, 0x1000, 0,
			GFL_BG_SCRSIZ_256x512, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x10000, 0x8000,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		};
		
		GFL_BG_SetBGControl( BG_FRAME_SCROLL_M, &bgcnt, GFL_BG_MODE_TEXT );
		GFL_BG_ClearFrame( BG_FRAME_SCROLL_M );

		GFL_BG_FillCharacter( BG_FRAME_SCROLL_M, 0, 1, 0 );
	}

	//OBJ������
	{	
		p_wk->p_unit	= GFL_CLACT_UNIT_Create( OBJ_CLWK_MAX, OBJ_UNIT_PRI, heapID );
	}

	//���\�[�X�ǂ݂���
	{	
		ARCHANDLE	*p_handle	= GFL_ARC_OpenDataHandle( ARCID_SHORTCUT_MENU_GRA, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_fld_shortcut_menu_gra_bg_NCLR,
				PALTYPE_MAIN_BG, PLT_BG_SCROLL_M*0x20, 0x20*2, heapID );

		p_wk->res[RESID_PLT]	= GFL_CLGRP_PLTT_Register( p_handle, 
				NARC_fld_shortcut_menu_gra_obj_NCLR, CLSYS_DRAW_MAIN, PLT_OBJ_M	*0x20, heapID );

		p_wk->res[RESID_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
				NARC_fld_shortcut_menu_gra_obj_NCGR, FALSE, CLSYS_DRAW_MAIN, heapID );

		p_wk->res[RESID_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
				NARC_fld_shortcut_menu_gra_obj_NCER, NARC_fld_shortcut_menu_gra_obj_NANR, heapID );


		GFL_ARC_CloseDataHandle( p_handle );
	}

	//���[�N�쐬
	{	
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.pos_x	= 128;
		cldata.pos_y	= 96;
		p_wk->p_clwk	= GFL_CLACT_WK_Create( p_wk->p_unit, p_wk->res[RESID_CHR], p_wk->res[RESID_PLT], p_wk->res[RESID_CEL], &cldata, CLSYS_DRAW_MAIN, heapID );
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, FALSE );
	}

	//���ʃ��W���[��������
	{	
		p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, NARC_message_shortcut_menu_dat, heapID );
		p_wk->p_font	= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );
		p_wk->p_que		= PRINTSYS_QUE_Create( heapID );
	}

	//���W���[��������
	{	
		SAVE_CONTROL_WORK *p_sv_ptr	= SaveControl_GetPointer();
		SHORTCUT *p_shortcut_sv = SaveData_GetShortCut( p_sv_ptr );
		SCROLL_Init( &p_wk->scroll, p_wk->p_cursor, p_wk->p_msg, p_wk->p_font, p_wk->p_que, p_shortcut_sv, p_wk->p_clwk, heapID );
		SCROLL_PrintMain( &p_wk->scroll );
	}

	GFL_BG_LoadScreenV_Req( BG_FRAME_SCROLL_M );


	//���W�X�^�ݒ�
	G2_SetBlendAlpha( GX_BLEND_PLANEMASK_BG1, GX_BLEND_PLANEMASK_BG0, 14, 8 );

	//���[�h�ʐݒ�
	switch( mode )
	{	
	case SHORTCUTMENU_MODE_POPUP:
		GFL_BG_SetScroll( BG_FRAME_SCROLL_M, GFL_BG_SCROLL_Y_SET, LISTMOVE_START );
		GFL_BG_SetVisible( BG_FRAME_SCROLL_M, VISIBLE_OFF );
		p_wk->seq	= MAINSEQ_NONE;
		break;
	case SHORTCUTMENU_MODE_STAY:
		GFL_BG_SetScroll( BG_FRAME_SCROLL_M, GFL_BG_SCROLL_Y_SET, 0 );
		GFL_BG_SetVisible( BG_FRAME_SCROLL_M, VISIBLE_ON );
		p_wk->seq	= MAINSEQ_MAIN;
		break;
	}


	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�V���[�g�J�b�g���j���[	�j��
 *
 *	@param	SHORTCUTMENU_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void SHORTCUTMENU_Exit( SHORTCUTMENU_WORK *p_wk )
{	
	//���W�X�^�N���A
	G2_BlendNone();

	//���W���[���j��
	{	
		SCROLL_Exit( &p_wk->scroll, p_wk->p_cursor );
	}

	//���[�N�j��
	{	
		GFL_CLACT_WK_Remove( p_wk->p_clwk );
	}

	//���ʃ��W���[���j��
	{	
		PRINTSYS_QUE_Delete( p_wk->p_que );
		GFL_FONT_Delete( p_wk->p_font );
		GFL_MSG_Delete( p_wk->p_msg );
	}

	//���\�[�X�j��
	{	
		GFL_CLGRP_PLTT_Release( p_wk->res[RESID_PLT] );
		GFL_CLGRP_CGR_Release( p_wk->res[RESID_CHR] );
		GFL_CLGRP_CELLANIM_Release( p_wk->res[RESID_CEL] );
	}

	//OBJ�j��
	{	
		GFL_CLACT_UNIT_Delete( p_wk->p_unit );
	}

	//BG�j��
	{	
		GFL_BG_FillCharacterRelease( BG_FRAME_SCROLL_M, 1, 0 );
		GFL_BG_ClearFrame( BG_FRAME_SCROLL_M );
		GFL_BG_FreeBGControl( BG_FRAME_SCROLL_M );
	}

	//���C�����[�N�j��
	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�V���[�g�J�b�g���j���[	���C������
 *
 *	@param	SHORTCUTMENU_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void SHORTCUTMENU_Main( SHORTCUTMENU_WORK *p_wk )
{	
	switch( p_wk->seq )
	{	
	case MAINSEQ_NONE:					//�������Ă��Ȃ�
		/* �������Ȃ� */
		break;

	case MAINSEQ_OPEN_START:		//�J���J�n
		p_wk->cnt	=	0;
		p_wk->seq	= MAINSEQ_OPEN_WAIT;
		break;

	case MAINSEQ_OPEN_WAIT:		//�J���҂�
		{	
			int scroll_y;
			scroll_y	=	LISTMOVE_START + LISTMOVE_DIFF * p_wk->cnt / LISTMOVE_SYNC;
			GFL_BG_SetScroll( BG_FRAME_SCROLL_M, GFL_BG_SCROLL_Y_SET, scroll_y );
			if( p_wk->cnt ++ >= LISTMOVE_SYNC )
			{	
				p_wk->seq	= MAINSEQ_MAIN;
			}
		}
		break;

	case MAINSEQ_MAIN:					//���C��
		{
			u32 input;
			SCROLL_Main( &p_wk->scroll );
			input = SCROLL_GetInput( &p_wk->scroll );
			
			switch( input )
			{	
			case SCROLL_INPUT_NULL:
				p_wk->input	= SHORTCUTMENU_INPUT_NONE;
				break;
			case SCROLL_INPUT_CANCEL:
				p_wk->input	= SHORTCUTMENU_INPUT_CANCEL;
				break;
			default:
				p_wk->input	= SHORTCUTMENU_INPUT_SELECT;
				p_wk->shortcutID	= input;
			}
		}
		break;

	case MAINSEQ_CLOSE_START:	//����J�n
		GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, FALSE );
		p_wk->cnt	=	LISTMOVE_SYNC;
		p_wk->seq	= MAINSEQ_CLOSE_WAIT;
		break;

	case MAINSEQ_CLOSE_WAIT:		//����҂�
		{	
			int scroll_y;
			scroll_y	=	LISTMOVE_START + LISTMOVE_DIFF * p_wk->cnt / LISTMOVE_SYNC;
			GFL_BG_SetScroll( BG_FRAME_SCROLL_M, GFL_BG_SCROLL_Y_SET, scroll_y );
			if( p_wk->cnt -- == 0 )
			{	
				p_wk->seq	= MAINSEQ_NONE;
			}
		}
		break;

	default:
		GF_ASSERT(0);
	}

	SCROLL_PrintMain( &p_wk->scroll );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�V���[�g�J�b�g���j���[	�J��
 *
 *	@param	SHORTCUTMENU_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void SHORTCUTMENU_Open( SHORTCUTMENU_WORK *p_wk )
{	
	if( p_wk->seq == MAINSEQ_NONE )
	{	
		//������
		p_wk->input				= SHORTCUTMENU_INPUT_NONE;
		p_wk->shortcutID	= SHORTCUT_ID_NULL;

		GFL_BG_SetVisible( BG_FRAME_SCROLL_M, VISIBLE_ON );

		p_wk->seq	= MAINSEQ_OPEN_START;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�V���[�g�J�b�g���j���[	����
 *
 *	@param	SHORTCUTMENU_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void SHORTCUTMENU_Close( SHORTCUTMENU_WORK *p_wk )
{	
	if( p_wk->seq == MAINSEQ_MAIN )
	{	
		p_wk->seq	= MAINSEQ_CLOSE_START;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�V���[�g�J�b�g���j���[	�ړ�
 *
 *	@param	const SHORTCUTMENU_WORK *cp_wk	���[�N
 *
 *	@retval	TRUE�ړ���
 *	@retval	FALSE�ړ����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
BOOL SHORTCUTMENU_IsMove( const SHORTCUTMENU_WORK *cp_wk )
{	
	return cp_wk->seq	== MAINSEQ_OPEN_WAIT
		|| cp_wk->seq	== MAINSEQ_CLOSE_WAIT;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�V���[�g�J�b�g���j���[	�I���������̂��擾
 *
 *	@param	const SHORTCUTMENU_WORK *cp_wk				���[�N
 *	@param	SHORTCUT_ID							*p_shortcutID	SHORTCUTMENU_INPUT_SELECT����
 *																								�I�񂾃V���[�g�J�b�gID�󂯎��
 *
 *	@return	SHORTCUTMENU_INPUT	���͎�ށi�񋓂��Q�Ɓj
 */
//-----------------------------------------------------------------------------
SHORTCUTMENU_INPUT SHORTCUTMENU_GetInput( const SHORTCUTMENU_WORK *cp_wk, SHORTCUT_ID *p_shortcutID )
{	
	if( p_shortcutID )
	{	
		*p_shortcutID	= cp_wk->shortcutID;
	}
	return cp_wk->input;
}
//=============================================================================
/**
 *		SCROLL
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[��������
 *
 *	@param	SCROLL_WORK *p_wk	���[�N
 *	@param	p_cursor					�J�[�\��
 *	@param	*p_msg						���b�Z�[�W
 *	@param	*p_font						�t�H���g
 *	@param	*p_que						�L���[
 *	@param	p_sv							�V���[�g�J�b�g�Z�[�u�f�[�^
 *	@param	p_cursor					�J�[�\��
 *	@param	heapID						�q�[�vID
 */
//-----------------------------------------------------------------------------
static void SCROLL_Init( SCROLL_WORK *p_wk, const SHORTCUT_CURSOR	*cp_cursor, GFL_MSGDATA *p_msg, GFL_FONT *p_font, PRINT_QUE *p_que, SHORTCUT *p_sv, GFL_CLWK *p_cursor, HEAPID heapID )
{	
	//���[�N�N���A
	GFL_STD_MemClear( p_wk, sizeof(SCROLL_WORK) );
	p_wk->p_que		= p_que;
	p_wk->p_sv		= p_sv;
	p_wk->p_clwk	= p_cursor;
	p_wk->heapID	= heapID;
	p_wk->p_msg		= p_msg;
	p_wk->p_font	= p_font;

	//BMPWIN�쐬
	p_wk->p_bmpwin	= GFL_BMPWIN_Create( BG_FRAME_SCROLL_M, 
			SCROLL_BMPWIN_X, SCROLL_BMPWIN_Y, SCROLL_BMPWIN_W, SCROLL_BMPWIN_H,
			PLT_BG_SCROLL_M, GFL_BMP_CHRAREA_GET_B );
	GFL_BMPWIN_MakeTransWindow( p_wk->p_bmpwin );

	//PRINT_UTIL�Z�b�g�A�b�v
	PRINT_UTIL_Setup( &p_wk->util, p_wk->p_bmpwin );

	//���X�g�쐬
	Scroll_CreateList( p_wk, cp_cursor->list, cp_cursor->cursor, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[��	�j��
 *
 *	@param	SCROLL_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SCROLL_Exit( SCROLL_WORK *p_wk, SHORTCUT_CURSOR	*p_cursor )
{	
	//���X�g�j��
	Scroll_DeleteList( p_wk, &p_cursor->list, &p_cursor->cursor );

	//BMPWIN�j��
	GFL_BMPWIN_Delete( p_wk->p_bmpwin );

	//���[�N�N���A
	GFL_STD_MemClear( p_wk, sizeof(SCROLL_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[�����C������
 *
 *	@param	SCROLL_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void SCROLL_Main( SCROLL_WORK *p_wk )
{	
	u32	select	= BMPMENULIST_NULL;
	BOOL is_move_change	= FALSE;
	BOOL is_loop	= FALSE;

	//��ԏ�ƈ�ԉ��̏ꍇ���[�v����
	//�ړ���2�A���ɂȂ��Ă��܂��̂ŁA���[�v�������邽�߃��C���̑O�ɍs��
	{	
		u16 cursor_pos;
		u32 max;
		BmpMenuList_DirectPosGet( p_wk->p_list, &cursor_pos );
		max	= BmpListParamGet( p_wk->p_list, BMPMENULIST_ID_COUNT );
		if( cursor_pos == 0 && GFL_UI_KEY_GetRepeat() & PAD_KEY_UP )
		{	
			//�ォ�牺�ւ̃��[�v
			u16 list;
			u16 cursor;
			u32 line;
			line	= BmpListParamGet( p_wk->p_list, BMPMENULIST_ID_LINE );

			list		= (max-1) - (line-1);
			cursor	= line-1;

			Scroll_DeleteList( p_wk, NULL, NULL );
			Scroll_CreateList( p_wk, list, cursor, p_wk->heapID );

			is_loop	= TRUE;
		}
		else if( cursor_pos == max-1 && GFL_UI_KEY_GetRepeat() & PAD_KEY_DOWN )
		{	
			Scroll_DeleteList( p_wk, NULL, NULL );
			Scroll_CreateList( p_wk, 0, 0, p_wk->heapID );

			is_loop	= TRUE;
		}
	}

	//���C��
	//	���[�v����Trg�F�����ē����Ă��܂�
	if( !is_loop )
	{	
		select	= BmpMenuList_Main( p_wk->p_list );
	}
	if( select == BMPMENULIST_NULL )
	{	
		if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
		{	
			//X�ł��L�����Z��
			select	= BMPMENULIST_CANCEL;
		}
		else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_Y )
		{
			//Y�ł�����
			u16 pos;
			BmpMenuList_DirectPosGet( p_wk->p_list, &pos );
			select	= BmpMenuList_PosParamGet( p_wk->p_list, pos );
		}
		else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT && p_wk->mode == SCROLL_MOVE_INSERT  )
		{
			//Y�ł�����
			u16 pos;
			BmpMenuList_DirectPosGet( p_wk->p_list, &pos );
			select	= BmpMenuList_PosParamGet( p_wk->p_list, pos );
		}
	}

	//���[�h�ɂ���ĈႤ
	if( p_wk->mode == SCROLL_MOVE_NORMAL )
	{
		p_wk->select	= select;
	}
	else if( p_wk->mode == SCROLL_MOVE_INSERT )
	{
		if( select == SCROLL_INPUT_CANCEL )
		{	
			is_move_change	= TRUE;
		}
		else if( select != SCROLL_INPUT_NULL )
		{	
			u16 cursor_pos;
			u16 cursor_bak;
			u16 list_bak;
			u16 insert_pos;
			SHORTCUTMENU_INPUT insertID;
			
			insertID	= BmpMenuList_PosParamGet( p_wk->p_list, p_wk->insert_list + p_wk->insert_cursor );


			//�Z�[�u�f�[�^�ւ̑}������
			BmpMenuList_DirectPosGet( p_wk->p_list, &cursor_pos );
			insert_pos	= SHORTCUT_Insert( p_wk->p_sv, insertID, cursor_pos );


			NAGI_Printf( "�}�� cursor_pos%d insertID%d insert_l%dp%d\n", cursor_pos, insertID, p_wk->insert_list, p_wk->insert_cursor );


			//�Đݒ�
			Scroll_DeleteList( p_wk, &list_bak, &cursor_bak );
			//�}���ʒu�ɃJ�[�\�����Đݒ�
			if( cursor_pos - 1 == insert_pos )
			{	
				cursor_bak--;
			}
			Scroll_CreateList( p_wk, list_bak, cursor_bak, p_wk->heapID );

			is_move_change	= TRUE;
		}
	}


	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT && p_wk->mode == SCROLL_MOVE_NORMAL )
	{	
		is_move_change	= TRUE;
	}

	//���[�h�ؑ�
	if( is_move_change )
	{	
		p_wk->mode ^= 1;
		if( p_wk->mode == SCROLL_MOVE_NORMAL )
		{	
			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, FALSE );
		}
		else if( p_wk->mode == SCROLL_MOVE_INSERT )
		{	
			BmpMenuList_PosGet( p_wk->p_list, &p_wk->insert_list, &p_wk->insert_cursor );

			GFL_CLACT_WK_SetDrawEnable( p_wk->p_clwk, TRUE );
		}
		Scroll_MoveCursorCallBack( p_wk->p_list, 0, 1 );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�X�N���[���v�����g����
 *
 *	@param	SCROLL_WORK *p_wk		���[�N
 *
 * @retval  BOOL  �]�����I����Ă���ꍇ��TRUE�^�I����Ă��Ȃ��ꍇ��FALSE
 */
//-----------------------------------------------------------------------------
static BOOL SCROLL_PrintMain( SCROLL_WORK *p_wk )
{	
	return PRINT_UTIL_Trans( &p_wk->util, p_wk->p_que );
}
//----------------------------------------------------------------------------
/**
 *	@brief
 *
 *	@param	const SCROLL_WORK *cp_wk 
 *
 *	@retval	�I������������A		���X�g�̃C���f�b�N�X
 *	@retval	SCROLL_INPUT_NULL		�I��
 *	@retval SCROLL_INPUT_CANCEL	�L�����Z��
 *				
 */
//-----------------------------------------------------------------------------
static u32 SCROLL_GetInput( const SCROLL_WORK *cp_wk )
{	
	return cp_wk->select;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�J�[�\�����ړ������Ƃ��ɌĂ΂��R�[���o�b�N
 *
 *	@param	BMPMENULIST_WORK * p_list	���[�N
 *	@param	param										�I�����Ă��郊�X�g�̃p�����[�^
 *	@param	mode	1�Ȃ�Ώ�������	0�Ȃ�΃J�[�\���ړ���
 */
//-----------------------------------------------------------------------------
static void Scroll_MoveCursorCallBack( BMPMENULIST_WORK * p_list, u32 param, u8 mode )
{	
	u16 cursor;
	u32 yblk;
	SCROLL_WORK *p_wk;

	//���[�N�擾
	p_wk	= (SCROLL_WORK *)BmpListParamGet( p_list, BMPMENULIST_ID_WORK );

	//�J�[�\���ʒu���n�C���C�g���鏈��
	cursor	= BmpMenuList_CursorYGet( p_list ) / 8;
	yblk		= BmpListParamGet( p_list, BMPMENULIST_ID_LINE_YBLK )/8;

	//NAGI_Printf( "yblk%d cursor%d\n", yblk, cursor );

	if( p_wk->mode == SCROLL_MOVE_NORMAL )
	{	
		GFL_BG_ChangeScreenPalette( 
				BG_FRAME_SCROLL_M, 
				SCROLL_BMPWIN_X, 
				SCROLL_BMPWIN_Y, 
				SCROLL_BMPWIN_W, 
				SCROLL_BMPWIN_H, 
				PLT_BG_SCROLL_M
				);
		GFL_BG_ChangeScreenPalette( 
				BG_FRAME_SCROLL_M, 
				SCROLL_BMPWIN_X, 
				SCROLL_BMPWIN_Y + cursor, 
				SCROLL_BMPWIN_W, 
				yblk,
				PLT_BG_SELECT_M );

		GFL_BG_LoadScreenV_Req( BG_FRAME_SCROLL_M );
	}
	else if( p_wk->mode == SCROLL_MOVE_INSERT )
	{	
		GFL_CLACTPOS	clpos;
		clpos.x	= 128;
		clpos.y	= (SCROLL_BMPWIN_Y + cursor)*8;
		GFL_CLACT_WK_SetPos( p_wk->p_clwk, &clpos, CLSYS_DRAW_MAIN );

		GFL_BG_ChangeScreenPalette( 
				BG_FRAME_SCROLL_M, 
				SCROLL_BMPWIN_X, 
				SCROLL_BMPWIN_Y, 
				SCROLL_BMPWIN_W, 
				SCROLL_BMPWIN_H, 
				PLT_BG_SCROLL_M
				);

		//�}������{�^��
		{
			u16 now_list;
			u16 now_cursor;
			u32 insert_y;

			BmpMenuList_PosGet( p_list, &now_list, &now_cursor );

			NAGI_Printf( "list %d cursor %d\n", now_list, now_cursor );
			insert_y	=	(p_wk->insert_cursor + p_wk->insert_list - now_list) 
				* BmpListParamGet( p_list, BMPMENULIST_ID_LINE_YBLK ) 
				+ BmpListParamGet( p_list, BMPMENULIST_ID_LINE_Y );

			insert_y	/= 8;

			GFL_BG_ChangeScreenPalette( 
					BG_FRAME_SCROLL_M, 
					SCROLL_BMPWIN_X, 
					SCROLL_BMPWIN_Y + insert_y , 
					SCROLL_BMPWIN_W, 
					yblk,
					PLT_BG_SELECT_M );
		}

		GFL_BG_LoadScreenV_Req( BG_FRAME_SCROLL_M );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	���X�g���쐬
 *
 *	@param	SCROLL_WORK *p_wk ���[�N
 *	@param	list_bak		���X�g�o�b�N�A�b�v
 *	@param	cursor_bak	�J�[�\���ʒu�o�b�N�A�b�v
 *	@param	heapID						�q�[�vID
 */
//-----------------------------------------------------------------------------
static void Scroll_CreateList( SCROLL_WORK *p_wk, u16 list_bak, u16 cursor_bak, HEAPID heapID )
{	
	u32 max;
	//�Z�[�u�f�[�^����ǂݎ��
	max	= SHORTCUT_GetMax( p_wk->p_sv );
	NAGI_Printf( "Y�{�^���o�^��%d\n", max );


	//���X�g�f�[�^�쐬
	{	
		int i;
		SHORTCUT_ID shorcutID;
		p_wk->p_data	= BmpMenuWork_ListCreate( max, heapID ); 

		for( i = 0; i < max ; i ++ )
		{	
			shorcutID	= SHORTCUT_GetType( p_wk->p_sv, i );
			BmpMenuWork_ListAddArchiveString( p_wk->p_data,
					p_wk->p_msg, msg_shortcut_menu_01_01+shorcutID, shorcutID, heapID );
		}
	}

	//�J�[�\���̍��W���C��
	while( (cursor_bak + list_bak) >= max )
	{	
		if( cursor_bak > 0 )
		{	
			cursor_bak--;
		}
		else if( list_bak > 0 )
		{	
			list_bak--;
		}
		else
		{
			GF_ASSERT_MSG( 0, "shortcut_menu��max=%d cursor %d list %d \n", max, cursor_bak, list_bak );
		}
	}

	//���X�g���C���쐬
	{	
		static const BMPMENULIST_HEADER sc_menulist_default	=
		{	
			NULL, //���X�g�f�[�^
			NULL,	//�J�[�\���ړ��R�[���o�b�N
			NULL,	//��s���Ƃ̃R�[���o�b�N
			NULL,	//BMPWIN
			0,		//���X�g���ڐ�
			5,		//�\���ő區�ڐ�
			0,		//���x���\��X�ʒu
			13,		//���ڕ\��X���W
			0,		//�J�[�\���\��X���W
			8,		//�\��Y���W
			COLOR_WHITE_LETTER,		//�\�������F
			COLOR_BACK,		//�\���w�i�F
			COLOR_WHITE_SHADOW,		//�\�������e�F
			0,		//�����ԊuY
			0,		//�����ԊuX
			BMPMENULIST_NO_SKIP,	//�y�[�W�X�L�b�v�^�C�v
			0,		//���������w��
			1,		//BG�J�[�\���\���t���O
			NULL,	//���[�N
			12,		//�t�H���g�T�C�Y
			16,		//�t�H���g�T�C�Y
			NULL,	//���b�Z�[�W
			NULL,	//�v�����gUTIL
			NULL,	//�v�����gQUE
			NULL,	//�t�H���g
		};
		BMPMENULIST_HEADER	header	= sc_menulist_default;
		header.list				= p_wk->p_data;
		header.count			= max;
		header.call_back	= Scroll_MoveCursorCallBack;
		header.icon				= NULL;
		header.win				= p_wk->p_bmpwin;
		header.work				= p_wk;
		header.msgdata		= p_wk->p_msg;
		header.print_util	= &p_wk->util;
		header.print_que	= p_wk->p_que;
		header.font_handle	= p_wk->p_font;

		p_wk->p_list	= BmpMenuList_Set( &header, list_bak, cursor_bak, heapID );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief	���X�g��j��
 *
 *	@param	SCROLL_WORK *p_wk ���[�N
 *	@param	p_list_bak		���X�g�o�b�N�A�b�v
 *	@param	p_cursor_bak	�J�[�\���ʒu�o�b�N�A�b�v
 */
//-----------------------------------------------------------------------------
static void Scroll_DeleteList( SCROLL_WORK *p_wk, u16 *p_list_bak, u16 *p_cursor_bak )
{	
	//���X�g�j��
	{	
		BmpMenuList_Exit( p_wk->p_list, p_list_bak, p_cursor_bak );
	}

	//���X�g�f�[�^�j��
	{	
		BmpMenuWork_ListDelete( p_wk->p_data );
	}
}
