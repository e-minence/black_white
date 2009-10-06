//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		debug_template.c
 *	@brief	�e���v���[�g�e�X�g�v���b�N
 *	@author	Toru=Nagihashi
 *	@data		2009.09.29
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	lib
#include <gflib.h>
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"

//	SYSTEM
#include "gamesystem/gamesystem.h"

//	module
#include "app/app_menu_common.h"
#include "app/app_taskmenu.h"
#include "infowin/infowin.h"
#include "ui/touchbar.h"
#include "../../ui/debug/ui_template_graphic.h"
#include "system/poke2dgra.h"

//archive
#include "arc_def.h"
//#include "config_gra.naix"

//	print
#include "font/font.naix"
#include "message.naix"
#include "msg/msg_config.h"
#include "print/gf_font.h"
#include "print/printsys.h"

//	mine
#include "debug_template.h"

FS_EXTERN_OVERLAY(ui_common);
FS_EXTERN_OVERLAY(ui_debug);

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�t���[��
//=====================================
enum
{	
	BG_FRAME_BAR_M	= GFL_BG_FRAME1_M,
	BG_FRAME_POKEMON_M	= GFL_BG_FRAME2_M,
};
//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	//���C��BG
	PLTID_BG_POKEMON_M		= 0,
	PLTID_BG_TOUCHBAR_M		= 13,
	PLTID_BG_INFOWIN_M		= 15,

	//���C��OBJ
	PLTID_OBJ_POKEMON_M		= 0,
	PLTID_OBJ_TOUCHBAR_M	= 13,
};
//-------------------------------------
///	���\�[�X�C���f�b�N�X
//=====================================
enum 
{
	OBJRESID_PM_CHR,
	OBJRESID_PM_PLT,
	OBJRESID_PM_CEL,

	OBJRESID_MAX,
} ;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�V�[�P���X�Ǘ�
//=====================================
typedef struct _SEQ_WORK SEQ_WORK;	//�֐��^��邽�ߑO���錾
typedef void (*SEQ_FUNCTION)( SEQ_WORK *p_wk, int *p_seq, void *p_param );
struct _SEQ_WORK
{
	SEQ_FUNCTION	seq_function;
	BOOL is_end;
	int seq;
	void *p_param;
};

//-------------------------------------
///	���C�����[�N
//=====================================
typedef struct
{
	//�O���t�B�b�N���W���[��
	UI_TEMPLATE_GRAPHIC_WORK	*p_graphic;

	//�V�[�P���X�V�X�e��
	SEQ_WORK			seq;

	//�^�b�`�o�[
	TOUCHBAR_WORK	*p_touchbar;

	//�|�P��������OBJ
	u32						objres[OBJRESID_MAX];
	GFL_CLWK			*p_clwk;

	//���ʂŎg���t�H���g
	GFL_FONT			*p_font;

	//���ʂŎg���L���[
	PRINT_QUE			*p_que;

	//���ʂŎg���^�X�N�V�X�e��
	GFL_TCBLSYS		*p_tcbl;

}TEMPLATE_WORK;
//=============================================================================
/**
 *					�v���g�^�C�v�錾
 *							���W���[�������啶��	XXXX_Init
 *								���\�[�X���O���[�o���֐��i�p�u���b�N�j
 *							���W���[������������	Xxxx_Init
 *								�����W���[�����֐��i�v���C�x�[�g�j
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT DEBUG_TEMPLATE_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT DEBUG_TEMPLATE_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT DEBUG_TEMPLATE_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
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
//=============================================================================
/**
 *					�O�����J
 */
//=============================================================================
//-------------------------------------
///	�v���b�N�f�[�^
//=====================================
const GFL_PROC_DATA DebugTemplate_ProcData = 
{	
	DEBUG_TEMPLATE_PROC_Init,
	DEBUG_TEMPLATE_PROC_Main,
	DEBUG_TEMPLATE_PROC_Exit,
};
//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================

//=============================================================================
/**
 *					proc
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�e���v���[�g	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_TEMPLATE_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	TEMPLATE_WORK		*p_wk;
	TEMPLATE_PARAM	*p_param;

	//�I�[�o�[���C�ǂݍ���
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_debug));

	//�����󂯎��
	p_param	= p_param_adrs;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NAGI_DEBUG_SUB, 0x30000 );
	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(TEMPLATE_WORK), HEAPID_NAGI_DEBUG_SUB );
	GFL_STD_MemClear( p_wk, sizeof(TEMPLATE_WORK) );

	//���ʃ��W���[���̍쐬
	p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_NAGI_DEBUG_SUB );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_NAGI_DEBUG_SUB );
	p_wk->p_tcbl		= GFL_TCBL_Init( HEAPID_NAGI_DEBUG_SUB, HEAPID_NAGI_DEBUG_SUB, 32, 32 );

	//���W���[���쐬
	p_wk->p_graphic	= UI_TEMPLATE_GRAPHIC_Init( 0, HEAPID_NAGI_DEBUG_SUB );
	SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_FadeOut );	//�ŏ���FadeOut�V�[�P���X

	//INFOWIN�̐ݒ�
	{
		GAME_COMM_SYS_PTR comm	= NULL;
		if( p_param->p_gamesys )
		{	
			comm	= GAMESYSTEM_GetGameCommSysPtr(p_param->p_gamesys);
		}
		INFOWIN_Init( BG_FRAME_BAR_M, 
				PLTID_BG_INFOWIN_M,
				comm, HEAPID_NAGI_DEBUG_SUB );
	}

	//�^�b�`�o�[�̐ݒ�
	{	
		TOUCHBAR_ITEM_ICON touchbar_icon_tbl[]	=
		{	
			{	
				TOUCHBAR_ICON_RETURN,
				{	TOUCHBAR_ICON_X_07, TOUCHBAR_ICON_Y },
			},
			{	
				TOUCHBAR_ICON_CLOSE,
				{	TOUCHBAR_ICON_X_06, TOUCHBAR_ICON_Y },
			},
			{	
				TOUCHBAR_ICON_CHECK,
				{	TOUCHBAR_ICON_X_05, TOUCHBAR_ICON_Y_CHECK },
			},
		};
		TOUCHBAR_SETUP	touchbar_setup;
		touchbar_setup.p_item		= touchbar_icon_tbl;
		touchbar_setup.item_num	= NELEMS(touchbar_icon_tbl);
		touchbar_setup.p_unit		= UI_TEMPLATE_GRAPHIC_GetClunit( p_wk->p_graphic );
		touchbar_setup.bar_frm	= BG_FRAME_BAR_M;
		touchbar_setup.bg_plt		= PLTID_BG_TOUCHBAR_M;
		touchbar_setup.obj_plt	= PLTID_OBJ_TOUCHBAR_M;
		touchbar_setup.mapping	= APP_COMMON_MAPPING_128K;

		p_wk->p_touchbar	= TOUCHBAR_Init( &touchbar_setup, HEAPID_NAGI_DEBUG_SUB );

	}

	//�|�P����OBJ
	{	
		POKEMON_PASO_PARAM	*p_ppp;
		ARCHANDLE						*p_handle;
			
		p_ppp	= (POKEMON_PASO_PARAM	*)PP_Create( 3, 0, 0, HEAPID_NAGI_DEBUG_SUB );

		p_handle	= POKE2DGRA_OpenHandle( HEAPID_NAGI_DEBUG_SUB );
		p_wk->objres[ OBJRESID_PM_CHR ]	= POKE2DGRA_OBJ_CGR_RegisterPPP( p_handle, p_ppp, POKEGRA_DIR_BACK, CLSYS_DRAW_MAIN, HEAPID_NAGI_DEBUG_SUB );
		p_wk->objres[ OBJRESID_PM_PLT ]	= POKE2DGRA_OBJ_PLTT_RegisterPPP( p_handle, p_ppp, POKEGRA_DIR_BACK ,CLSYS_DRAW_MAIN,  PLTID_OBJ_POKEMON_M,  HEAPID_NAGI_DEBUG_SUB );
		p_wk->objres[ OBJRESID_PM_CEL ]	= POKE2DGRA_OBJ_CELLANM_RegisterPPP( p_ppp, POKEGRA_DIR_BACK, APP_COMMON_MAPPING_128K, CLSYS_DRAW_MAIN, HEAPID_NAGI_DEBUG_SUB );
		GFL_ARC_CloseDataHandle( p_handle );

		GFL_HEAP_FreeMemory( p_ppp );

	}
	{	
		GFL_CLUNIT					*p_unit;
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

		cldata.pos_x	= 188;
		cldata.pos_y	= 96;

		p_unit	= UI_TEMPLATE_GRAPHIC_GetClunit( p_wk->p_graphic );
		p_wk->p_clwk	= GFL_CLACT_WK_Create( p_unit, 
				p_wk->objres[OBJRESID_PM_CHR],
				p_wk->objres[OBJRESID_PM_PLT],
				p_wk->objres[OBJRESID_PM_CEL],
				&cldata, 
				CLSYS_DEFREND_MAIN, HEAPID_NAGI_DEBUG_SUB );
	}

	//�|�P����BG
	{	
		POKEMON_PASO_PARAM	*p_ppp;

		p_ppp	= (POKEMON_PASO_PARAM	*)PP_Create( 6, 0, 0, HEAPID_NAGI_DEBUG_SUB );

		GFL_BG_FillCharacter( BG_FRAME_POKEMON_M, 0, 1,  0 );

		POKE2DGRA_BG_TransResourcePPP( p_ppp, POKEGRA_DIR_BACK, BG_FRAME_POKEMON_M,
				1, PLTID_BG_POKEMON_M, HEAPID_NAGI_DEBUG_SUB );

		POKE2DGRA_BG_WriteScreen( BG_FRAME_POKEMON_M, 1, PLTID_BG_POKEMON_M, 0, 4 );
		GFL_BG_LoadScreenReq( BG_FRAME_POKEMON_M );

		GFL_HEAP_FreeMemory( p_ppp );
	}

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�e���v���[�g	���C���v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_TEMPLATE_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	TEMPLATE_WORK	*p_wk;
	
	p_wk	= p_wk_adrs;

	//�|�P����BG�j��
	{	
		GFL_BG_FillCharacterRelease( BG_FRAME_POKEMON_M, 1, 0 );
	}

	//�|�P����OBJ�j��
	{	
		GFL_CLACT_WK_Remove( p_wk->p_clwk );	
		GFL_CLGRP_PLTT_Release( p_wk->objres[ OBJRESID_PM_PLT ] );
		GFL_CLGRP_CGR_Release( p_wk->objres[ OBJRESID_PM_CHR ] );
		GFL_CLGRP_CELLANIM_Release( p_wk->objres[ OBJRESID_PM_CEL ] );
	}

	//���W���[���j��
	TOUCHBAR_Exit( p_wk->p_touchbar );
	INFOWIN_Exit();
	SEQ_Exit( &p_wk->seq );
	UI_TEMPLATE_GRAPHIC_Exit( p_wk->p_graphic );

	//���ʃ��W���[���̔j��
	GFL_TCBL_Exit( p_wk->p_tcbl );
	PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_font );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );
	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_NAGI_DEBUG_SUB );

	//�I�[�o�[���C�j��
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_debug));
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

	
	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�e���v���[�g	���C���v���Z�X���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_TEMPLATE_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	TEMPLATE_WORK	*p_wk	= p_wk_adrs;

	//�V�[�P���X
	SEQ_Main( &p_wk->seq );

	//�`��
	UI_TEMPLATE_GRAPHIC_2D_Draw( p_wk->p_graphic );

	//INFO
	INFOWIN_Update();

	//TOUCHBAR
	TOUCHBAR_Main( p_wk->p_touchbar );

	//�v�����g
	PRINTSYS_QUE_Main( p_wk->p_que );

	//�^�X�N
	GFL_TCBL_Main( p_wk->p_tcbl );	

	//�I��
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
 *	@brief	SEQ	������
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 *	@param	*p_param				�p�����[�^
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param, SEQ_FUNCTION seq_function )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );

	//������
	p_wk->p_param	= p_param;

	//�Z�b�g
	SEQ_SetNext( p_wk, seq_function );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	�j��
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 */
//-----------------------------------------------------------------------------
static void SEQ_Exit( SEQ_WORK *p_wk )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���C������
 *
 *	@param	SEQ_WORK *p_wk ���[�N
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
 *	@brief	SEQ	�I���擾
 *
 *	@param	const SEQ_WORK *cp_wk		���[�N
 *
 *	@return	TRUE�Ȃ�ΏI��	FALSE�Ȃ�Ώ�����
 */	
//-----------------------------------------------------------------------------
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk )
{	
	return cp_wk->is_end;
}
//----------------------------------------------------------------------------
/**
 *	@brief	SEQ	���̃V�[�P���X��ݒ�
 *
 *	@param	SEQ_WORK *p_wk	���[�N
 *	@param	seq_function		�V�[�P���X
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
 *	@brief	SEQ	�I��
 *
 *	@param	SEQ_WORK *p_wk	���[�N
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
 *	@brief	�t�F�[�hOUT
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param				���[�N
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
 *	@brief	�t�F�[�hIN
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param				���[�N
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
		SEQ_End( p_seqwk );
		break;

	default:
		GF_ASSERT(0);
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	�ݒ��ʃ��C������
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param )
{	
	enum
	{
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};	

	TEMPLATE_WORK	*p_wk	= p_param;

	if( GFL_UI_TP_GetTrg() )
	{
		SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
	}
}

