//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		map_main.c
 *	@brief	�^�E���}�b�v���C��
 *	@author	Toru=Nagihashi
 *	@data		2009.07.17
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//lib
#include <gflib.h>

//	constant
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"

//	archive
#include "arc_def.h"
#include "townmap_gra.naix"
#include "message.naix"
//#include "msg/msg_townmap.h"

//	common
#include "app/app_menu_common.h"

//	Module
#include "townmap_data_sys.h"

//mine
#include "townmap_grh.h"
#include "app/townmap.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	APPBAR
//=====================================
typedef enum
{	
	APPBAR_OPTION_MASK_CLOSE		= 1<<0,	//����
	APPBAR_OPTION_MASK_RETURN		= 1<<1,	//�߂�
	APPBAR_OPTION_MASK_CUR_D		= 1<<2,	//�J�[�\����
	APPBAR_OPTION_MASK_CUR_U		= 1<<3,	//�J�[�\����
	APPBAR_OPTION_MASK_CUR_L		= 1<<4,	//�J�[�\����
	APPBAR_OPTION_MASK_CUR_R		= 1<<5,	//�J�[�\���E
	APPBAR_OPTION_MASK_CHECK		= 1<<6,	//�`�F�b�N�{�b�N�X
	APPBAR_OPTION_MASK_SCALE		= 1<<7,	//�g��k��


	APPBAR_OPTION_MASK_SKY	= APPBAR_OPTION_MASK_CLOSE|APPBAR_OPTION_MASK_RETURN,	//����ԉ�ʂ̂Ƃ�
	APPBAR_OPTION_MASK_TOWN	= APPBAR_OPTION_MASK_CUR_L|APPBAR_OPTION_MASK_CUR_R|
		APPBAR_OPTION_MASK_SCALE|
		APPBAR_OPTION_MASK_CLOSE|APPBAR_OPTION_MASK_RETURN,	//�n�}��ʂ̂Ƃ�
} APPBAR_OPTION_MASK;
//�I���������̎擾
typedef enum
{
	APPBAR_SELECT_NONE	= GFL_UI_TP_HIT_NONE,
	APPBAR_SELECT_CLOSE		= 0,
	APPBAR_SELECT_RETURN,
	APPBAR_SELECT_CUR_D,
	APPBAR_SELECT_CUR_U,
	APPBAR_SELECT_CUR_L,
	APPBAR_SELECT_CUR_R,
	APPBAR_SELECT_CHECK,
	APPBAR_SELECT_SCALE_UP,
	APPBAR_SELECT_SCALE_DOWN,
} APPBAR_SELECT;
//���\�[�X
enum
{	
	APPBAR_RES_COMMON_PLT,
	APPBAR_RES_COMMON_CHR,
	APPBAR_RES_COMMON_CEL,
	APPBAR_RES_SCALE_PLT,
	APPBAR_RES_SCALE_CHR,
	APPBAR_RES_SCALE_CEL,

	APPBAR_RES_MAX
};
//�A�C�R�����
enum
{	
	APPBAR_BARICON_CLOSE,
	APPBAR_BARICON_RETURN,
	APPBAR_BARICON_CUR_D,
	APPBAR_BARICON_CUR_U,
	APPBAR_BARICON_CUR_L,
	APPBAR_BARICON_CUR_R,
	APPBAR_BARICON_CHECK,
	APPBAR_BARICON_SCALE_UP,
	APPBAR_BARICON_SCALE_DOWN,
	APPBAR_BARICON_MAX,
};
//�ʒu
//�o�[
#define APPBAR_MENUBAR_X	(0)
#define APPBAR_MENUBAR_Y	(21)
#define APPBAR_MENUBAR_W	(32)
#define APPBAR_MENUBAR_H	(3)
//CLOSE
#define APPBAR_ICON_Y	(168)
#define APPBAR_ICON_CUR_L_X				(16)
#define APPBAR_ICON_CUR_R_X				(38)
#define APPBAR_ICON_SCALE_DOWN_X	(144)
#define APPBAR_ICON_SCALE_UP_X		(168)
#define APPBAR_ICON_CLOSE_X				(200)
#define APPBAR_ICON_RETURN_X			(232)

#define APPBAR_ICON_W	(24)
#define APPBAR_ICON_H	(24)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	����ʃo�[(�A�v���P�[�V�����o�[�Ə���ɖ���)
//=====================================
typedef struct 
{
	GFL_CLWK	*p_clwk[APPBAR_BARICON_MAX];
	u32				res[APPBAR_RES_MAX];
	u32				bg_frm;
	GFL_ARCUTIL_TRANSINFO				chr_pos;
	s32				trg;
	s32				cont;
	u32				mode;
} APPBAR_WORK;
#define APPBAR_BG_CHARAAREA_SIZE	(8*8*GFL_BG_1CHRDATASIZ)
//-------------------------------------
///	�J�[�\������
//=====================================
typedef struct 
{
	int dummy;	
} CURSOR_WORK;
//-------------------------------------
///	�ꏊ����
//=====================================
typedef struct
{
	int dummy;	
} PLACE_WORK;
//-------------------------------------
///	�n�}����	
//=====================================
typedef struct 
{
	int dummy;	
} MAP_WORK;
//-------------------------------------
///	Module����
//=====================================
typedef struct 
{
	int dummy;	
} CONTROL_WORK;
//-------------------------------------
///	�V�[�P���X�Ǘ�
//=====================================
typedef struct _SEQ_WORK SEQ_WORK;
typedef void (*SEQ_FUNCTION)( SEQ_WORK *p_wk, int *p_seq, void *p_param );
struct _SEQ_WORK
{
	SEQ_FUNCTION	seq_function;
	BOOL is_end;
	int seq;
	void *p_param_adrs;
}; 
//-------------------------------------
///	���C�����[�N
//=====================================
typedef struct 
{
	//�O���t�B�b�N�V�X�e��
	TOWNMAP_GRAPHIC_SYS	*p_grh;

	//�A�v���P�[�V�����o�[
	APPBAR_WORK		appbar;

	//�f�[�^
	TOWNMAP_DATA	*p_data;

	//���W���[��
	SEQ_WORK			seq;
} TOWNMAP_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
static GFL_PROC_RESULT TOWNMAP_PROC_Init(
		GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT TOWNMAP_PROC_Exit(
		GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT TOWNMAP_PROC_Main(
		GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
///	SEQ
//=====================================
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param_adrs, SEQ_FUNCTION seq_function );
static void SEQ_Exit( SEQ_WORK *p_wk );
static void SEQ_Main( SEQ_WORK *p_wk );
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk );
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function );
static void SEQ_End( SEQ_WORK *p_wk );
//-------------------------------------
///	SEQFUNC
//=====================================
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param_adrs );
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param_adrs );
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param_adrs );
//-------------------------------------
///	APPBAR
//=====================================
static void APPBAR_Init( APPBAR_WORK *p_wk, APPBAR_OPTION_MASK mask, GFL_CLUNIT* p_unit, u8 bar_frm, u8 bg_plt, u8 obj_plt, HEAPID heapID );
static void APPBAR_Exit( APPBAR_WORK *p_wk );
static void APPBAR_Main( APPBAR_WORK *p_wk );
static APPBAR_SELECT APPBAR_GetTrg( const APPBAR_WORK *cp_wk );
static APPBAR_SELECT APPBAR_GetCont( const APPBAR_WORK *cp_wk );
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h,  u8 plt, BOOL compressedFlag, HEAPID heapID );
//-------------------------------------
///	CURSOR
//=====================================
static void CURSOR_Init( CURSOR_WORK *p_wk, GFL_CLUNIT *p_unit, HEAPID heapID );
static void CURSOR_Exit( CURSOR_WORK *p_wk );
static void CURSOR_Main( CURSOR_WORK *p_wk );
static void CURSOR_GetPos( const CURSOR_WORK *cp_wk, GFL_POINT *p_pos );
//-------------------------------------
///	PLACE
//=====================================
static void PLACE_Init( PLACE_WORK *p_wk, const TOWNMAP_DATA *cp_data, HEAPID heapID);
static void PLACE_Exit( PLACE_WORK *p_wk );
static void PLACE_Main( PLACE_WORK *p_wk, const CURSOR_WORK *cp_wk );
static void PLACE_ScaleUp( PLACE_WORK *p_wk );
static void PLACE_ScaleDown( PLACE_WORK *p_wk );
static void PLACE_SetVisible( PLACE_WORK *p_wk, BOOL is_visible );
static void PLACE_SetWldPos( PLACE_WORK *p_wk, const GFL_POINT *cp_pos );
static void PLACE_GetWldPos( const PLACE_WORK *cp_wk, GFL_POINT *p_pos );
//-------------------------------------
///	MAP
//=====================================
static void MAP_Init( MAP_WORK *p_wk, HEAPID heapID );
static void MAP_Exit( MAP_WORK *p_wk );
static void MAP_Main( MAP_WORK *p_wk );
static void MAP_ScaleUp( MAP_WORK *p_wk );
static void MAP_ScaleDown( MAP_WORK *p_wk );
static BOOL MAP_IsScaleEnd( MAP_WORK *p_wk );
static void MAP_SetWldPos( MAP_WORK *p_wk, const GFL_POINT *cp_pos );
static void MAP_GetWldPos( const MAP_WORK *cp_wk, GFL_POINT *p_pos );
//-------------------------------------
///	CONTROL
//=====================================
static void CONTROL_Init( CONTROL_WORK *p_wk );
static void CONTROL_Exit( CONTROL_WORK *p_wk );

//=============================================================================
/**
 *						DATA
 */
//=============================================================================
//=============================================================================
/**
 *						GLOBAL
 */
//=============================================================================
//-------------------------------------
///	PROC�f�[�^�O�����J
//=====================================
const GFL_PROC_DATA	TownMap_ProcData	=
{
	TOWNMAP_PROC_Init,
	TOWNMAP_PROC_Main,
	TOWNMAP_PROC_Exit,
};
//=============================================================================
/**
 *						PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	PROC������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param_adrs					����
 *	@param	*p_wk_adrs						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT TOWNMAP_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	TOWNMAP_WORK	*p_wk;
	TOWNMAP_PARAM	*p_rank_param;
	u16	data_len;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_TOWNMAP, 0x60000 );

	//���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(TOWNMAP_WORK), HEAPID_TOWNMAP );
	GFL_STD_MemClear( p_wk, sizeof(TOWNMAP_WORK) );

	//���W���[���쐬
	p_wk->p_grh	= TOWNMAP_GRAPHIC_Init( HEAPID_TOWNMAP );
	SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_FadeOut );

	APPBAR_Init( &p_wk->appbar, APPBAR_OPTION_MASK_TOWN,
			TOWNMAP_GRAPHIC_GetUnit( p_wk->p_grh ),
			TOWNMAP_GRAPHIC_GetFrame( p_wk->p_grh, TOWNMAP_BG_FRAME_BAR_M),
			TOWNMAP_BG_PAL_M_15, TOWNMAP_OBJ_PAL_M_12, HEAPID_TOWNMAP );

	p_wk->p_data	= TOWNMAP_DATA_Alloc( HEAPID_TOWNMAP );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PROC�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param_adrs					����
 *	@param	*p_wk_adrs						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT TOWNMAP_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;

	//���W���[���j��
	TOWNMAP_DATA_Free( p_wk->p_data );

	APPBAR_Exit( &p_wk->appbar );

	SEQ_Exit( &p_wk->seq );	
	TOWNMAP_GRAPHIC_Exit( p_wk->p_grh );

	//���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap(HEAPID_TOWNMAP );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PROC���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param_adrs					����
 *	@param	*p_wk_adrs						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT TOWNMAP_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	TOWNMAP_WORK	*p_wk	= p_wk_adrs;
	//�V�[�P���X
	SEQ_Main( &p_wk->seq );

	//�`��
	TOWNMAP_GRAPHIC_Draw( p_wk->p_grh );

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
 *	@param	*p_param_adrs		�p�����[�^
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Init( SEQ_WORK *p_wk, void *p_param_adrs, SEQ_FUNCTION seq_function )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );

	//������
	p_wk->p_param_adrs	= p_param_adrs;

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
		p_wk->seq_function( p_wk, &p_wk->seq, p_wk->p_param_adrs );
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
 *	@param	*p_param_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param_adrs )
{	
	enum
	{	
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_END,
	};

	TOWNMAP_WORK	*p_wk	= p_param_adrs;

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
 *	@param	*p_param_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param_adrs )
{
	enum
	{
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};	

	TOWNMAP_WORK	*p_wk	= p_param_adrs;

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
 *	@brief	�Q�[���J�n
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_param_adrs				���[�N
 *
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param_adrs )
{	
	TOWNMAP_WORK	*p_wk	= p_param_adrs;

	switch( APPBAR_GetTrg( &p_wk->appbar ) )
	{	
	case APPBAR_SELECT_CLOSE:
		SEQ_SetNext( &p_wk->seq, SEQFUNC_FadeIn );
		break;
	case APPBAR_SELECT_RETURN:
		SEQ_SetNext( &p_wk->seq, SEQFUNC_FadeIn );
		break;
	}

	APPBAR_Main( &p_wk->appbar );
}
//=============================================================================
/**
 *	APPBAR
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	������
 *
 *	@param	APPBAR_WORK *p_wk	���[�N
 *	@param	mask							���[�h�}�X�N
 *	@param	p_unit						APPBAR��OBJ�����pp_unit
 *	@param	bar_frm						�g�p����BG�ʁi�����Ƀ��C�����T�u���𔻒肷��̂ɂ��g�p�j
 *	@param	bg_plt						�g�p����BG�p���b�g�ԍ�
 *	@param	obj_plt						�g�p����OBJ�p���b�g�ԍ�
 *	@param	heapID						�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void APPBAR_Init( APPBAR_WORK *p_wk, APPBAR_OPTION_MASK mask, GFL_CLUNIT* p_unit, u8 bar_frm, u8 bg_plt, u8 obj_plt, HEAPID heapID )
{	
	CLSYS_DRAW_TYPE	clsys_draw_type;
	CLSYS_DEFREND_TYPE	clsys_def_type;
	PALTYPE							bgplttype;

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(APPBAR_WORK) );
	p_wk->bg_frm	= bar_frm;
	p_wk->trg			= APPBAR_SELECT_NONE;
	p_wk->cont		= APPBAR_SELECT_NONE;
	p_wk->mode		= mask;

	//OBJ�ǂݍ��ޏꏊ���`�F�b�N
	{	
		if( bar_frm >= GFL_BG_FRAME0_S )
		{	
			clsys_draw_type	= CLSYS_DRAW_SUB;
			clsys_def_type	= CLSYS_DEFREND_SUB;
			bgplttype				= PALTYPE_SUB_BG;
		}
		else
		{	
			clsys_draw_type	= CLSYS_DRAW_MAIN;
			clsys_def_type	= CLSYS_DEFREND_MAIN;
			bgplttype				= PALTYPE_MAIN_BG;
		}
	}
	
	//���ʃ��\�[�X
	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( APP_COMMON_GetArcId(), heapID );

		//BG
		//�̈�̊m��
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, APP_COMMON_GetBarPltArcIdx(),
				bgplttype, bg_plt*0x20, APP_COMMON_BAR_PLT_NUM*0x20, heapID );

		p_wk->chr_pos	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, APP_COMMON_GetBarCharArcIdx(), p_wk->bg_frm, APPBAR_BG_CHARAAREA_SIZE, FALSE, heapID );
		GF_ASSERT( p_wk->chr_pos != GFL_ARCUTIL_TRANSINFO_FAIL );
		//�X�N���[���̓�������ɒu���āA����32*3������������
		ARCHDL_UTIL_TransVramScreenEx( p_handle, APP_COMMON_GetBarScrnArcIdx(), p_wk->bg_frm,
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos), 0, 21, 32, 24, 
				APPBAR_MENUBAR_X, APPBAR_MENUBAR_Y, APPBAR_MENUBAR_W, APPBAR_MENUBAR_H,
				bg_plt, FALSE, heapID );

		NAGI_Printf( "pos %d size %d\n", GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos), GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->chr_pos) );

		//OBJ
		//���ʃA�C�R�����\�[�X
		if( (p_wk->mode & APPBAR_OPTION_MASK_CLOSE)
				|| (p_wk->mode & APPBAR_OPTION_MASK_RETURN)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_D)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_U)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_L)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_R)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CHECK)
				)
		{	
			p_wk->res[APPBAR_RES_COMMON_PLT]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
				APP_COMMON_GetBarIconPltArcIdx(), clsys_draw_type, obj_plt*0x20, 0, APP_COMMON_BARICON_PLT_NUM, heapID );	
			p_wk->res[APPBAR_RES_COMMON_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
					APP_COMMON_GetBarIconCharArcIdx(), FALSE, clsys_draw_type, heapID );

			p_wk->res[APPBAR_RES_COMMON_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
					APP_COMMON_GetBarIconCellArcIdx(APP_COMMON_MAPPING_128K),
				 	APP_COMMON_GetBarIconAnimeArcIdx(APP_COMMON_MAPPING_128K), heapID );
		}
		GFL_ARC_CloseDataHandle( p_handle );
	}

	//�A�v�����Ƃ̃��\�[�X
	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_TOWNMAP_GRAPHIC, heapID );
		if( (p_wk->mode & APPBAR_OPTION_MASK_SCALE) )
		{	
			p_wk->res[APPBAR_RES_SCALE_PLT]	= GFL_CLGRP_PLTT_RegisterEx( p_handle,
				NARC_townmap_gra_tmap_bg_d_NCLR, clsys_draw_type, (obj_plt+2)*0x20,
				2, 1, heapID );	

			p_wk->res[APPBAR_RES_SCALE_CHR]	= GFL_CLGRP_CGR_Register( p_handle,
					NARC_townmap_gra_tmap_obj_d_NCGR, FALSE, clsys_draw_type, heapID );

			p_wk->res[APPBAR_RES_SCALE_CEL]	= GFL_CLGRP_CELLANIM_Register( p_handle,
					NARC_townmap_gra_tmap_obj_d_NCER,
				 	NARC_townmap_gra_tmap_obj_d_NANR, heapID );
		}

		GFL_ARC_CloseDataHandle( p_handle );
	}

	//CLWK�̍쐬
	{
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

		//�I���{�^��
 		if( p_wk->mode & APPBAR_OPTION_MASK_CLOSE )
 		{	
			cldata.pos_x	= APPBAR_ICON_CLOSE_X;
			cldata.pos_y	= APPBAR_ICON_Y;
			cldata.anmseq	= APP_COMMON_BARICON_CLOSE;
			p_wk->p_clwk[APPBAR_BARICON_CLOSE]	= GFL_CLACT_WK_Create( p_unit, 
					p_wk->res[APPBAR_RES_COMMON_CHR],
					p_wk->res[APPBAR_RES_COMMON_PLT],
					p_wk->res[APPBAR_RES_COMMON_CEL],
					&cldata,
					clsys_def_type,
					heapID
					);
		}

		//�߂�{�^��
 		if( p_wk->mode & APPBAR_OPTION_MASK_RETURN )
 		{	
			cldata.pos_x	= APPBAR_ICON_RETURN_X;
			cldata.pos_y	= APPBAR_ICON_Y;
			cldata.anmseq	= APP_COMMON_BARICON_RETURN;
			p_wk->p_clwk[APPBAR_BARICON_RETURN]	= GFL_CLACT_WK_Create( p_unit, 
					p_wk->res[APPBAR_RES_COMMON_CHR],
					p_wk->res[APPBAR_RES_COMMON_PLT],
					p_wk->res[APPBAR_RES_COMMON_CEL],
					&cldata,
					clsys_def_type,
					heapID
					);
		}

		//���{�^��
 		if( p_wk->mode & APPBAR_OPTION_MASK_CUR_L )
 		{	
			cldata.pos_x	= APPBAR_ICON_CUR_L_X;
			cldata.pos_y	= APPBAR_ICON_Y;
			cldata.anmseq	= APP_COMMON_BARICON_CURSOR_LEFT;
			p_wk->p_clwk[APPBAR_BARICON_CUR_L]	= GFL_CLACT_WK_Create( p_unit, 
					p_wk->res[APPBAR_RES_COMMON_CHR],
					p_wk->res[APPBAR_RES_COMMON_PLT],
					p_wk->res[APPBAR_RES_COMMON_CEL],
					&cldata,
					clsys_def_type,
					heapID
					);
		}

		//�E�{�^��
 		if( p_wk->mode & APPBAR_OPTION_MASK_CUR_R )
 		{	
			cldata.pos_x	= APPBAR_ICON_CUR_R_X;
			cldata.pos_y	= APPBAR_ICON_Y;
			cldata.anmseq	= APP_COMMON_BARICON_CURSOR_RIGHT;
			p_wk->p_clwk[APPBAR_BARICON_CUR_R]	= GFL_CLACT_WK_Create( p_unit, 
					p_wk->res[APPBAR_RES_COMMON_CHR],
					p_wk->res[APPBAR_RES_COMMON_PLT],
					p_wk->res[APPBAR_RES_COMMON_CEL],
					&cldata,
					clsys_def_type,
					heapID
					);
		}

		//���ꃊ�\�[�X�j��
		if( p_wk->mode & APPBAR_OPTION_MASK_SCALE )
		{	
			cldata.pos_x	= APPBAR_ICON_SCALE_UP_X;
			cldata.pos_y	= APPBAR_ICON_Y;
			cldata.anmseq	= 7;
			p_wk->p_clwk[APPBAR_BARICON_SCALE_UP]	= GFL_CLACT_WK_Create( p_unit, 
					p_wk->res[APPBAR_RES_SCALE_CHR],
					p_wk->res[APPBAR_RES_SCALE_PLT],
					p_wk->res[APPBAR_RES_SCALE_CEL],
					&cldata,
					clsys_def_type,
					heapID
					);

			cldata.pos_x	= APPBAR_ICON_SCALE_DOWN_X;
			cldata.pos_y	= APPBAR_ICON_Y;
			cldata.anmseq	= 8;
			p_wk->p_clwk[APPBAR_BARICON_SCALE_DOWN]	= GFL_CLACT_WK_Create( p_unit, 
					p_wk->res[APPBAR_RES_SCALE_CHR],
					p_wk->res[APPBAR_RES_SCALE_PLT],
					p_wk->res[APPBAR_RES_SCALE_CEL],
					&cldata,
					clsys_def_type,
					heapID
					);
			GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk[APPBAR_BARICON_SCALE_DOWN], obj_plt+3, CLWK_PLTTOFFS_MODE_PLTT_TOP );
			GFL_CLACT_WK_SetPlttOffs( p_wk->p_clwk[APPBAR_BARICON_SCALE_UP], obj_plt+3, CLWK_PLTTOFFS_MODE_PLTT_TOP );
		}
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	�j��
 *
 *	@param	APPBAR_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void APPBAR_Exit( APPBAR_WORK *p_wk )
{	
	//CLWK
	{	
		int i;
		for( i = 0; i < APPBAR_BARICON_MAX; i++ )
		{	
			if( p_wk->p_clwk[i] )
			{	
				GFL_CLACT_WK_Remove( p_wk->p_clwk[i] );
			}
		}
	}

	//OBJ
	{	
		//���ꃊ�\�[�X�j��
		if( p_wk->mode & APPBAR_OPTION_MASK_SCALE )
		{	
			GFL_CLGRP_CELLANIM_Release( p_wk->res[APPBAR_RES_SCALE_CEL] );
			GFL_CLGRP_CGR_Release( p_wk->res[APPBAR_RES_SCALE_CHR] );
			GFL_CLGRP_PLTT_Release( p_wk->res[APPBAR_RES_SCALE_PLT] );
		}

		//���ʃ��\�[�X�j��
		if( (p_wk->mode & APPBAR_OPTION_MASK_CLOSE)
				|| (p_wk->mode & APPBAR_OPTION_MASK_RETURN)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_D)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_U)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_L)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CUR_R)
				|| (p_wk->mode & APPBAR_OPTION_MASK_CHECK)
				)
 		{	
			GFL_CLGRP_CELLANIM_Release( p_wk->res[APPBAR_RES_COMMON_CEL] );
			GFL_CLGRP_CGR_Release( p_wk->res[APPBAR_RES_COMMON_CHR] );
			GFL_CLGRP_PLTT_Release( p_wk->res[APPBAR_RES_COMMON_PLT] );
		}
	}

	//BG
	{	
		GFL_BG_FreeCharacterArea(p_wk->bg_frm,
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->chr_pos),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->chr_pos));
	}

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(APPBAR_WORK) );

}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	���C������
 *
 *	@param	APPBAR_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void APPBAR_Main( APPBAR_WORK *p_wk )
{	
	static const GFL_UI_TP_HITTBL	sc_hit_tbl[APPBAR_BARICON_MAX]	=
	{	
		{	
			APPBAR_ICON_Y, APPBAR_ICON_Y + APPBAR_ICON_W, 
			APPBAR_ICON_CLOSE_X, APPBAR_ICON_CLOSE_X + APPBAR_ICON_W
		},
		{	
			APPBAR_ICON_Y, APPBAR_ICON_Y + APPBAR_ICON_W, 
			APPBAR_ICON_RETURN_X, APPBAR_ICON_RETURN_X + APPBAR_ICON_W
		},
		{	
			0
		},
		{	
			0
		},
		{	
			APPBAR_ICON_Y, APPBAR_ICON_Y + APPBAR_ICON_W, 
			APPBAR_ICON_CUR_L_X, APPBAR_ICON_CUR_L_X + APPBAR_ICON_W
		},
		{	
			APPBAR_ICON_Y, APPBAR_ICON_Y + APPBAR_ICON_W, 
			APPBAR_ICON_CUR_R_X, APPBAR_ICON_CUR_R_X + APPBAR_ICON_W
		},
		{	
			APPBAR_ICON_Y, APPBAR_ICON_Y + APPBAR_ICON_W, 
			APPBAR_ICON_SCALE_UP_X, APPBAR_ICON_SCALE_UP_X + APPBAR_ICON_W
		},
		{	
			APPBAR_ICON_Y, APPBAR_ICON_Y + APPBAR_ICON_W, 
			APPBAR_ICON_SCALE_DOWN_X, APPBAR_ICON_SCALE_DOWN_X + APPBAR_ICON_W
		},
		
	};
	
	//�^�b�`�̓���
	int i;
	u32 x, y;

	p_wk->trg		= APPBAR_SELECT_NONE;
	p_wk->cont	= APPBAR_SELECT_NONE;
	for( i = 0; i < NELEMS(sc_hit_tbl); i++ )
	{	
		if( p_wk->mode & 1<<i )
		{	
			if( GFL_UI_TP_GetPointTrg( &x, &y ) )
			{	
				if( ((u32)( x - sc_hit_tbl[i].rect.left) <= 
							(u32)(sc_hit_tbl[i].rect.right - sc_hit_tbl[i].rect.left))
						&	((u32)( y - sc_hit_tbl[i].rect.top) <=
							(u32)(sc_hit_tbl[i].rect.bottom - sc_hit_tbl[i].rect.top)))
				{
					p_wk->trg	= i;
				}
			}

			if( GFL_UI_TP_GetPointCont( &x, &y ) )
			{	
				if( ((u32)( x - sc_hit_tbl[i].rect.left) <= 
							(u32)(sc_hit_tbl[i].rect.right - sc_hit_tbl[i].rect.left))
						&	((u32)( y - sc_hit_tbl[i].rect.top) <=
							(u32)(sc_hit_tbl[i].rect.bottom - sc_hit_tbl[i].rect.top)))
				{
					p_wk->cont	= i;
				}
			}
		}
	}

	//�蓮�̓���
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
	{	
		p_wk->trg	= APPBAR_SELECT_RETURN;
	}
	else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
	{	
		p_wk->trg	= APPBAR_SELECT_CLOSE;
	}
	if( GFL_UI_KEY_GetCont() & PAD_KEY_LEFT )
	{	
		p_wk->cont	= APPBAR_SELECT_CUR_L;
	}
	else if( GFL_UI_KEY_GetCont() & PAD_KEY_RIGHT )
	{	
		p_wk->cont	= APPBAR_SELECT_CUR_R;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	�I�����ꂽ���̂��擾
 *
 *	@param	const APPBAR_WORK *cp_wk ���[�N
 *
 *	@return	APPBAR_SELECT��
 */
//-----------------------------------------------------------------------------
static APPBAR_SELECT APPBAR_GetTrg( const APPBAR_WORK *cp_wk )
{	
	return cp_wk->trg;
}
//----------------------------------------------------------------------------
/**
 *	@brief	APPBAR	�I�����ꂽ���̂��擾
 *
 *	@param	const APPBAR_WORK *cp_wk ���[�N
 *
 *	@return	APPBAR_SELECT��
 */
//-----------------------------------------------------------------------------
static APPBAR_SELECT APPBAR_GetCont( const APPBAR_WORK *cp_wk )
{	
	return cp_wk->cont;
}
//----------------------------------------------------------------------------
/**
 *	@brief	Screen�f�[�^�́@VRAM�]���g���Łi�ǂݍ��񂾃X�N���[���̈ꕔ�͈͂��o�b�t�@�ɒ������j
 *
 *	@param	ARCHANDLE *handle	�n���h��
 *	@param	datID							�f�[�^ID
 *	@param	frm								�t���[��
 *	@param	chr_ofs						�L�����I�t�Z�b�g
 *	@param	src_x							�]���������W
 *	@param	src_y							�]����Y���W
 *	@param	src_w							�]������			//�f�[�^�̑S�̂̑傫��
 *	@param	src_h							�]��������		//�f�[�^�̑S�̂̑傫��
 *	@param	dst_x							�]����X���W
 *	@param	dst_y							�]����Y���W
 *	@param	dst_w							�]���敝
 *	@param	dst_h							�]���捂��
 *	@param	plt								�p���b�g�ԍ�
 *	@param	compressedFlag		���k�t���O
 *	@param	heapID						�ꎞ�o�b�t�@�p�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void ARCHDL_UTIL_TransVramScreenEx( ARCHANDLE *handle, ARCDATID datID, u32 frm, u32 chr_ofs, u8 src_x, u8 src_y, u8 src_w, u8 src_h, u8 dst_x, u8 dst_y, u8 dst_w, u8 dst_h, u8 plt, BOOL compressedFlag, HEAPID heapID )
{	
	void *p_src_arc;
	NNSG2dScreenData* p_scr_data;

	//�ǂݍ���
	p_src_arc = GFL_ARCHDL_UTIL_Load( handle, datID, compressedFlag, GetHeapLowID(heapID) );

	//�A���p�b�N
	if(!NNS_G2dGetUnpackedScreenData( p_src_arc, &p_scr_data ) )
	{	
		GF_ASSERT(0);	//�X�N���[���f�[�^����Ȃ���
	}

	//�G���[
	GF_ASSERT( src_w * src_h * 2 <= p_scr_data->szByte );

	//�L�����I�t�Z�b�g�v�Z
	if( chr_ofs )
	{	
		int i;
		if( GFL_BG_GetScreenColorMode( frm ) == GX_BG_COLORMODE_16 )
		{
			u16 *p_scr16;
			
			p_scr16	=	(u16 *)&p_scr_data->rawData;
			for( i = 0; i < src_w * src_h ; i++ )
			{
				p_scr16[i]	+= chr_ofs;
			}	
		}
		else
		{	
			GF_ASSERT(0);	//256�ł͍���ĂȂ�
		}
	}

	//��������
	if( GFL_BG_GetScreenBufferAdrs( frm ) != NULL )
	{	
		GFL_BG_WriteScreenExpand( frm, dst_x, dst_y, dst_w, dst_h,
				&p_scr_data->rawData, src_x, src_y, src_w, src_h );	
		GFL_BG_ChangeScreenPalette( frm, dst_x, dst_y, dst_w, dst_h, plt );
		GFL_BG_LoadScreenReq( frm );
	}
	else
	{	
		GF_ASSERT(0);	//�o�b�t�@���Ȃ�
	}

	GFL_HEAP_FreeMemory( p_src_arc );
}	
