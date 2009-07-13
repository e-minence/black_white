//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		irc_ranking.c
 *	@brief	�ԊO���~�j�Q�[�������L���O���
 *	@author	Toru=Nagihashi
 *	@data		2009.07.10
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
#include <gflib.h>

//	constant
#include "system/main.h"	//HEAPID
#include "system/gfl_use.h"

//	archive
#include "arc_def.h"
#include "message.naix"
#include "msg/msg_irc_ranking.h"
#include "ircranking_gra.naix"

//mine
#include "net_app/irc_ranking.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	// ���C�����BG
	RANKING_BG_PAL_M_00 = 0,// �g�p���ĂȂ�
	RANKING_BG_PAL_M_01,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_02,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_03,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_04,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_05,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_06,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_07,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_08,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_09,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_10,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_11,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_12,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_13,		// �g�p���ĂȂ�
	RANKING_BG_PAL_M_14,		// �t�H���g
	RANKING_BG_PAL_M_15,		// INFOWIN


	// �T�u���BG
	RANKING_BG_PAL_S_00 = 0,	//�g�p���ĂȂ�
	RANKING_BG_PAL_S_01,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_02,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_03,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_04,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_05,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_06,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_07,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_08,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_09,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_10,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_11,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_12,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_13,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_14,		// �g�p���ĂȂ�
	RANKING_BG_PAL_S_15,		// �g�p���ĂȂ�
};

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	BG���[�N
//=====================================
typedef struct 
{
	GFL_ARCUTIL_TRANSINFO	bar_char_m;
	GFL_ARCUTIL_TRANSINFO	bar_char_s;
} GRAPHIC_BG_WORK;
//-------------------------------------
///	�O���t�B�b�N���[�N
//=====================================
typedef struct 
{
	GRAPHIC_BG_WORK	bg;
	GFL_TCB	*p_vblank_task;
} GRAPHIC_WORK;
//-------------------------------------
///	�V�[�P���X�Ǘ�
//=====================================
typedef struct _SEQ_WORK SEQ_WORK;
typedef void (*SEQ_FUNCTION)( SEQ_WORK *p_wk, int *p_seq, void *p_param );
struct _SEQ_WORK{
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
	GRAPHIC_WORK	grp;
	SEQ_WORK			seq;
} IRC_RANKING_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
static GFL_PROC_RESULT IRC_RANKING_PROC_Init(
		GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_RANKING_PROC_Exit(
		GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
static GFL_PROC_RESULT IRC_RANKING_PROC_Main(
		GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work );
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
///	BG
//=====================================
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
//-------------------------------------
///	GRAPHIC
//=====================================
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk );
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );


//=============================================================================
/**
 *						DATA
 */
//=============================================================================
//-------------------------------------
///	�f�[�^
//=====================================
enum
{	
	GRAPHIC_BG_FRAME_TOP_M,
	GRAPHIC_BG_FRAME_FONT_M,
	GRAPHIC_BG_FRAME_BAR_M,
	GRAPHIC_BG_FRAME_BACK_M,

	GRAPHIC_BG_FRAME_TOP_S,
	GRAPHIC_BG_FRAME_FONT_S,
	GRAPHIC_BG_FRAME_BAR_S,
	GRAPHIC_BG_FRAME_BACK_S,

	GRAPHIC_BG_FRAME_MAX
};
static const struct 
{
	u32									frame;
	GFL_BG_BGCNT_HEADER	bgcnt_header;
	u32									mode;
}	sc_bgsetup[GRAPHIC_BG_FRAME_MAX]	=
{	
	{	
		GFL_BG_FRAME0_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME1_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME2_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME3_M,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME0_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME1_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x0800, GX_BG_CHARBASE_0x0c000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME2_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	},
	{	
		GFL_BG_FRAME3_S,
		{
			0, 0, 0x800, 0,
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,
			GX_BG_SCRBASE_0x1800, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE
		},
		GFL_BG_MODE_TEXT
	}
};
//�t���[���擾�p�}�N��
#define GRAPHIC_BG_GetFrame( x )	(sc_bgsetup[x].frame)

//=============================================================================
/**
 *						GLOBAL
 */
//=============================================================================
//-------------------------------------
///	PROC�f�[�^�O�����J
//=====================================
const GFL_PROC_DATA	IrcRanking_ProcData	=
{
	IRC_RANKING_PROC_Init,
	IRC_RANKING_PROC_Main,
	IRC_RANKING_PROC_Exit,
} ;

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
 *	@param	*p_param					����
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RANKING_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_RANKING_WORK	*p_wk;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_IRCRANKING, 0x20000 );

	//���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(IRC_RANKING_WORK), HEAPID_IRCRANKING );
	GFL_STD_MemClear( p_wk, sizeof(IRC_RANKING_WORK) );

	//���W���[��������
	GRAPHIC_Init( &p_wk->grp, HEAPID_IRCRANKING );
	SEQ_Init( &p_wk->seq, p_wk, NULL );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PROC�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					����
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RANKING_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_RANKING_WORK	*p_wk	= p_work;

	//���W���[���j��
	SEQ_Exit( &p_wk->seq );
	GRAPHIC_Exit( &p_wk->grp );

	//���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap(HEAPID_IRCRANKING );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PROC���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					����
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT IRC_RANKING_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param, void *p_work )
{	
	IRC_RANKING_WORK	*p_wk	= p_work;

	SEQ_Main( &p_wk->seq );

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
 *						BG
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	BG	������
 *
 *	@param	GRAPHIC_BG_WORK *p_wk	���[�N
 *	@param	heapID								�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_BG_WORK) );

	//������
	GFL_BG_Init( heapID );
	GFL_BMPWIN_Init( heapID );

	//�O���t�B�b�N���[�h�ݒ�
	{	
		static const GFL_BG_SYS_HEADER sc_bgsys_header	=
		{	
			GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D
		};
		GFL_BG_SetBGMode( &sc_bgsys_header );
	}

	//BG�ʐݒ�
	{	
		int i;
		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{	
			GFL_BG_SetBGControl( sc_bgsetup[i].frame, &sc_bgsetup[i].bgcnt_header, sc_bgsetup[i].mode );
			GFL_BG_ClearFrame( sc_bgsetup[i].frame );
			GFL_BG_SetVisible( sc_bgsetup[i].frame, VISIBLE_ON );
		}
	}

	//�ǂݍ���
	{	
		ARCHANDLE *p_handle;
		p_handle	= GFL_ARC_OpenDataHandle( ARCID_IRCRANKING_GRAPHIC, heapID );

		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_ircranking_gra_bg_NCLR,
				PALTYPE_MAIN_BG, RANKING_BG_PAL_M_00*0x20, 0, heapID );
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_ircranking_gra_bg_NCLR,
				PALTYPE_SUB_BG, RANKING_BG_PAL_S_00*0x20, 0, heapID );

		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_ircranking_gra_back_NCGR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_M), 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_ircranking_gra_back_NCGR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_S), 0, 0, FALSE, heapID );

		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_ircranking_gra_back_NSCR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_M), 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_ircranking_gra_back_NSCR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_S), 0, 0, FALSE, heapID );

		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_ircranking_gra_top_NSCR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_M), 0, 0, FALSE, heapID );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_ircranking_gra_top2_NSCR,
				GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_TOP_S), 0, 0, FALSE, heapID );

		GFL_BG_FillCharacter( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 0, 1,  0 );
		GFL_BG_FillCharacter( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S), 0, 1,  0 );

		p_wk->bar_char_m	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, 
				NARC_ircranking_gra_frame_NCGR, GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M), 0, FALSE, heapID );
		p_wk->bar_char_s	= GFL_ARCHDL_UTIL_TransVramBgCharacterAreaMan( p_handle, 
				NARC_ircranking_gra_frame_NCGR, GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S), 0, FALSE, heapID );

		GFL_ARC_CloseDataHandle( p_handle );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	BG	�j��
 *
 *	@param	GRAPHIC_BG_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk )
{	
	//�j��
	{	
		GFL_BG_FreeCharacterArea(GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_M),
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->bar_char_m),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->bar_char_m));

		GFL_BG_FreeCharacterArea(GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BAR_S),
				GFL_ARCUTIL_TRANSINFO_GetPos(p_wk->bar_char_s),
				GFL_ARCUTIL_TRANSINFO_GetSize(p_wk->bar_char_s));

		GFL_BG_FillCharacterRelease( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_M), 1, 0 );
		GFL_BG_FillCharacterRelease( GRAPHIC_BG_GetFrame(GRAPHIC_BG_FRAME_BACK_S), 1, 0 );
	}

	//BG�ʔj��
	{	
		int i;
		for( i = 0; i < GRAPHIC_BG_FRAME_MAX; i++ )
		{	
			GFL_BG_FreeBGControl( sc_bgsetup[i].frame );
		}
	}

	//�I��
	GFL_BMPWIN_Exit();
	GFL_BG_Exit();

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_BG_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	BG	VBlank�֐�
 *
 *	@param	GRAPHIC_BG_WORK *p_wk		���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk )
{	
	GFL_BG_VBlankFunc();
}
//=============================================================================
/**
 *						GRAPHIC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�ݒ�
 *
 *	@param	GRAPHIC_WORK *p_wk	���[�N
 *	@param	heapID							�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID )
{	
	static const GFL_DISP_VRAM sc_vramSetTable =
	{
		GX_VRAM_BG_128_A,					// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,				// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,					// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_16_I,       // �T�u2D�G���W����OBJ
		GX_VRAM_SUB_OBJEXTPLTT_NONE,// �T�u2D�G���W����OBJ�g���p���b�g
		GX_VRAM_TEX_NONE,						// �e�N�X�`���C���[�W�X���b�g
		GX_VRAM_TEXPLTT_NONE,				// �e�N�X�`���p���b�g�X���b�g
		GX_OBJVRAMMODE_CHAR_1D_128K,		
		GX_OBJVRAMMODE_CHAR_1D_128K,		
	};

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );

	//���W�X�^������
	G2_BlendNone();
	G2S_BlendNone();	
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

	//VRAM�N���A�[
	GFL_DISP_ClearVRAM( 0 );

	//VRAM�o���N�ݒ�
	GFL_DISP_SetBank( &sc_vramSetTable );

	// �f�B�X�v���CON
	GFL_DISP_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
	GFL_DISP_SetDispOn();

	//	�\��
	GFL_DISP_GX_InitVisibleControl();

	//	���W���[��������
	GRAPHIC_BG_Init( &p_wk->bg, heapID );

	//VBlankTask�o�^
	p_wk->p_vblank_task	= GFUser_VIntr_CreateTCB(Graphic_VBlankTask, p_wk, 0 );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�j��
 *
 *	@param	GRAPHIC_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk )
{	
	//VBLANKTask����
	GFL_TCB_DeleteTask( p_wk->p_vblank_task );

	//���W���[���j��
	GRAPHIC_BG_Exit( &p_wk->bg );

	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�N�`��
 *
 *	@param	GRAPHIC_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_Draw( GRAPHIC_WORK *p_wk )
{	

}
//----------------------------------------------------------------------------
/**
 *	@brief	�O���t�B�b�NVBLANK�֐�
 *
 *	@param	GRAPHIC_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work )
{	
	GRAPHIC_WORK *p_wk	= p_work;
	GRAPHIC_BG_VBlankFunction( &p_wk->bg );
}
