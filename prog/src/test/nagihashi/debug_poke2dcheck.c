//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		debug_poke2dcheck.c
 *	@brief	�|�P�����QD�G�`�F�b�N���
 *	@author	Toru=Nagihashi
 *	@data		2009.10.06
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>
#include "system/gfl_use.h"
#include "system/main.h"	//HEAPID

//�|�P�����̊G
#include "system/poke2dgra.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "arc\debug\d_nagi_graphic.naix"

//�v�����g
#include "font/font.naix"
#include "message.naix"
#include "print/gf_font.h"
#include "print/printsys.h"

//�O���Q��
#include "debug_poke2dcheck.h"

#include "debug/debug_str_conv.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�}�N��
//=====================================
#define MATH_ROUND(x,min,max)	(x < min ? max: x > max ? min: x)

//-------------------------------------
///	�t���[��
//=====================================
enum
{	
	BG_FRAME_PRINT_M	= GFL_BG_FRAME0_M,
	BG_FRAME_BACK_S	= GFL_BG_FRAME0_S,
};
//-------------------------------------
///	�p���b�g
//=====================================
enum
{	
	//���C��BG
	PLTID_BG_M		= 0,

	//�T�uBG
	PLTID_BG_BACK_S		= 0,

	//���C��OBJ
	PLTID_OBJ_M		= 0,

	//�T�uOBJ
	PLTID_OBJ_POKE1_S		= 0,
	PLTID_OBJ_POKE2_S		= 1,
};
//-------------------------------------
///	CLWK�C���f�b�N�X
//=====================================
typedef enum
{
	CLWKID_POKE1,
	CLWKID_POKE2,
	CLWKID_MAX,
} CLWKID;
//-------------------------------------
///	���\�[�X�C���f�b�N�X
//=====================================
enum 
{
	RESID_OBJ_PM1_CHR,
	RESID_OBJ_PM2_CHR,

	RESID_OBJ_PM1_PLT,
	RESID_OBJ_PM2_PLT,

	RESID_OBJ_PM1_CEL,
	RESID_OBJ_PM2_CEL,

	RESID_MAX,
} ;

//-------------------------------------
///	�f�o�b�O�v�����g
//=====================================
#define DEBUGPRINT_STRBUFF_LEN	(255)

//-------------------------------------
///	�|�P�����O���t�B�b�N
//=====================================
static const GFL_POINT	sc_pokemon_pos[CLWKID_MAX]	=
{	
	{	
		64,96
	},
	{	
		128+64,96
	}
};
enum
{	
	POKEDATA_PARAM_MONSNO,
	POKEDATA_PARAM_FORMNO,
	POKEDATA_PARAM_SEX,
	POKEDATA_PARAM_RARE,
	POKEDATA_PARAM_MAX,
};

//-------------------------------------
///	�V���N
//=====================================
#define CONT_CURSORMOVE_SYNC	(10)


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
	int dummy;
} GRAPHIC_BG_WORK;
//-------------------------------------
///	OBJ���[�N
//=====================================
typedef struct 
{
	GFL_CLUNIT			*p_clunit;
} GRAPHIC_OBJ_WORK;
//-------------------------------------
///	GRAPHIC���[�N
//=====================================
typedef struct 
{
	GRAPHIC_BG_WORK		bg;
	GRAPHIC_OBJ_WORK	obj;
	GFL_TCB						*p_vblank_task;
} GRAPHIC_WORK;
//-------------------------------------
///	�|�P�����̊G�̂��ƃf�[�^
//=====================================
typedef struct 
{
	union
	{	
		struct
		{	
			int mons_no;
			int form_no;
			int sex;
			int rare;
		};
		int a[POKEDATA_PARAM_MAX];
	};
} POKEGRA_DATA;

//-------------------------------------
///	�|�P�����G���[�N
//=====================================
typedef struct 
{
	//CLWK
	GFL_CLWK			*p_clwk[CLWKID_MAX];
	//���\�[�X
	u32						res[RESID_MAX];
} POKEGRA_WORK;
//-------------------------------------
///	�f�o�b�O�v�����g�p���
//=====================================
typedef struct 
{
	GFL_BMP_DATA	*p_bmp;
	GFL_FONT			*p_font;
	STRBUF				*p_strbuf;
	u8	frm;
	u8	w;
	u8	h;
	u8	dummy;
	u16	dummy2;
	HEAPID heapID;
} DEBUGPRINT_WORK;
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
	//�O���t�B�b�N�V�X�e��
	GRAPHIC_WORK	graphic;

	//�V�[�P���X�V�X�e��
	SEQ_WORK			seq;

	//�|�P�����O���t�B�b�N
	POKEGRA_WORK	pokegra;

	//�|�P�����̏��
	POKEGRA_DATA	max;
	POKEGRA_DATA	min;
	POKEGRA_DATA	now;
	int						cursor;
	u32						sync;

	//�f�o�b�O�v�����g�V�X�e��
	DEBUGPRINT_WORK	print;

	//���ʂŎg���t�H���g
	GFL_FONT			*p_font;

	//���ʂŎg���L���[
	PRINT_QUE			*p_que;

} POKE2DCHECK_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT DEBUG_POKE2DCHECK_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT DEBUG_POKE2DCHECK_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT DEBUG_POKE2DCHECK_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
/// GRAPHIC
//=====================================
static void GRAPHIC_Init( GRAPHIC_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_Exit( GRAPHIC_WORK *p_wk );
static void GRAPHIC_Main( GRAPHIC_WORK *p_wk );
static GFL_CLUNIT* GRAPHIC_GetClunit( const GRAPHIC_WORK *cp_wk );
static void Graphic_VBlankTask( GFL_TCB *p_tcb, void *p_work );
//-------------------------------------
///	BG
//=====================================
static void GRAPHIC_BG_Init( GRAPHIC_BG_WORK *p_wk, HEAPID heapID );
static void GRAPHIC_BG_Exit( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_Main( GRAPHIC_BG_WORK *p_wk );
static void GRAPHIC_BG_VBlankFunction( GRAPHIC_BG_WORK *p_wk );
//-------------------------------------
///	OBJ
//=====================================
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID );
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk );
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk );
static GFL_CLUNIT* GRAPHIC_OBJ_GetClunit( const GRAPHIC_OBJ_WORK *cp_wk );
//-------------------------------------
///	POKEMONOBJ
//=====================================
static void POKEGRA_Init( POKEGRA_WORK *p_wk, GFL_CLUNIT *clunit, HEAPID heapID );
static void POKEGRA_Exit( POKEGRA_WORK *p_wk );
static void POKEGRA_Main( POKEGRA_WORK *p_wk );
static void POKEGRA_ReLoad( POKEGRA_WORK *p_wk, GFL_CLUNIT *clunit, const POKEGRA_DATA *cp_data, HEAPID heapID );
//-------------------------------------
///	DEBUGPRINT
//=====================================
static void DEBUGPRINT_Init( DEBUGPRINT_WORK *p_wk, u8 frm, u8 w, u8 h, HEAPID heapID );
static void DEBUGPRINT_Exit( DEBUGPRINT_WORK *p_wk );
static void DEBUGPRINT_Open( DEBUGPRINT_WORK *p_wk );
static void DEBUGPRINT_Close( DEBUGPRINT_WORK *p_wk );
static void DEBUGPRINT_Print( DEBUGPRINT_WORK *p_wk, u16 x, u16 y, const char *cp_str );
static void DEBUGPRINT_PrintV( DEBUGPRINT_WORK *p_wk, u16 x, u16 y, const char *cp_str, ... );
static void DEBUGPRINT_Clear( DEBUGPRINT_WORK *p_wk );
static void DEBUGPRINT_Update( DEBUGPRINT_WORK *p_wk, const int *cp_cursor, const POKEGRA_DATA *cp_now, const POKEGRA_DATA *cp_max );
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
///	SEQFUNC	*SEQ_FUNCTION�^
//=====================================
static void SEQFUNC_FadeOut( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_FadeIn( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
static void SEQFUNC_Main( SEQ_WORK *p_seqwk, int *p_seq, void *p_param );
//-------------------------------------
///	ETC
//=====================================
static void BG_LoadResource( HEAPID heapID );
static void POKEGRADATA_GetMinMax( POKEGRA_DATA *p_min, POKEGRA_DATA *p_max, POKEGRA_DATA *p_now, HEAPID heapID );

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	BG�O���t�B�b�N���[�h�ݒ�
//=====================================
static const GFL_BG_SYS_HEADER sc_bgsys_header	=
{	
	GX_DISPMODE_GRAPHICS,GX_BGMODE_0,GX_BGMODE_0,GX_BG0_AS_2D	//�O���t�B�b�N���[�h�A���C��BG�ʐݒ�A�T�uBG�ʐݒ�ABG0�ʐݒ�
};
//-------------------------------------
///	BG�ʐݒ�
//=====================================
static const struct 
{
	u32									frame;
	GFL_BG_BGCNT_HEADER	bgcnt_header;
	u32									mode;
	BOOL								is_visible;
}	sc_bgsetup[]	=
{	
	//MAIN------------------------
	{	
		GFL_BG_FRAME0_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,	//BG�̎��
		TRUE,	//�����\��
	},
	{	
		GFL_BG_FRAME1_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
	{	
		GFL_BG_FRAME2_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x0800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
	{	
		GFL_BG_FRAME3_M,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},

	//SUB---------------------------
	{	
		GFL_BG_FRAME0_S,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x0000, GX_BG_CHARBASE_0x04000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 0, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
	{	
		GFL_BG_FRAME1_S,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x1000, GX_BG_CHARBASE_0x08000, GFL_BG_CHRSIZ_256x256,//��ذ��ް��A����ް��A��׻���
			GX_BG_EXTPLTT_01, 1, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
	{	
		GFL_BG_FRAME2_S,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x2000, GX_BG_CHARBASE_0x10000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 2, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
#if 0	//�g��Ȃ��ꍇ�́A���̃e�[�u����������Ă��������B(#if#endif����Ȃ��Ă����v�ł�)
	{	
		GFL_BG_FRAME3_S,	//�ݒ肷��t���[��
		{
			0, 0, 0x800, 0,	//X,Y,��ذ��ޯ̧�A��ذݵ̾��
			GFL_BG_SCRSIZ_256x256, GX_BG_COLORMODE_16,	//��ذݻ��ށA�װӰ��
			GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x14000, GFL_BG_CHRSIZ_256x256,
			GX_BG_EXTPLTT_01, 3, 0, 0, FALSE//�g����گĽۯāA�\���D��x�A�ر���ް�A��а�Aӻ޲��׸�
		},
		GFL_BG_MODE_TEXT,//BG�̎��
		TRUE,	//�����\��
	},
#endif
};
//-------------------------------------
///	CLSYS_INIT�f�[�^
//=====================================
static const GFL_CLSYS_INIT sc_clsys_init	=
{	
	0, 0,		//���C���T�[�t�F�[�X�̍���X,Y���W
	0, 512,	//�T�u�T�[�t�F�[�X�̍���X,Y���W
	4, 124,	//���C��OAM�Ǘ��J�n�`�I��	�i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j
	4, 124,	//��bOAM�Ǘ��J�n�`�I��	�i�ʐM�A�C�R���p�ɊJ�n��4�ȏ�ɁA�܂����ׂ�4�̔{�������Ă��������j
	0,			//�Z��Vram�]���Ǘ���
	32,32,32,32,	//��ׁA��گāA�ٱ�ҁA����ق̓o�^�ł��郊�\�[�X�Ǘ��ő吔
  16, 16,				//���C���A�T�u��CGRVRAM�Ǘ��̈�J�n�I�t�Z�b�g�i�ʐM�A�C�R���p��16�ȏ�ɂ��Ă��������j
};
//-------------------------------------
///	���[�N�쐬�ő吔
//=====================================
#define GRAPHIC_OBJ_CLWK_CREATE_MAX	(128)

//=============================================================================
/**
 *					�O�����J
 */
//=============================================================================
//-------------------------------------
///	�v���b�N�f�[�^
//=====================================
const GFL_PROC_DATA DebugPoke2D_ProcData = 
{	
	DEBUG_POKE2DCHECK_PROC_Init,
	DEBUG_POKE2DCHECK_PROC_Main,
	DEBUG_POKE2DCHECK_PROC_Exit,
};
//=============================================================================
/**
 *					�v���Z�X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����QD�`�F�b�N	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_POKE2DCHECK_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	POKE2DCHECK_WORK	*p_wk;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NAGI_DEBUG_SUB, 0x30000 );
	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(POKE2DCHECK_WORK), HEAPID_NAGI_DEBUG_SUB );
	GFL_STD_MemClear( p_wk, sizeof(POKE2DCHECK_WORK) );

	//���ʂŎg�����W���[��������
	p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_NAGI_DEBUG_SUB );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_NAGI_DEBUG_SUB );

	//�O���t�B�b�N������
	GRAPHIC_Init( &p_wk->graphic, HEAPID_NAGI_DEBUG_SUB );

	BG_LoadResource( HEAPID_NAGI_DEBUG_SUB );

	//���W���[��������
	SEQ_Init( &p_wk->seq, p_wk, SEQFUNC_FadeOut );	//�ŏ���FadeOut�V�[�P���X
	{	
		GFL_CLUNIT *clunit	= GRAPHIC_GetClunit( &p_wk->graphic );
		POKEGRA_Init( &p_wk->pokegra, clunit, HEAPID_NAGI_DEBUG_SUB );
	}
	DEBUGPRINT_Init( &p_wk->print, BG_FRAME_PRINT_M, 32, 24, HEAPID_NAGI_DEBUG_SUB );
	DEBUGPRINT_Open( &p_wk->print );

	GFL_STD_MemClear( &p_wk->now, sizeof(POKEGRA_DATA) );
	p_wk->now.mons_no	= MONSNO_HUSIGIDANE;

	POKEGRADATA_GetMinMax( &p_wk->min, &p_wk->max, &p_wk->now, HEAPID_NAGI_DEBUG_SUB );

	//�����v�����g
	DEBUGPRINT_Update( &p_wk->print, &p_wk->cursor, &p_wk->now, &p_wk->max );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����QD�`�F�b�N	���C���v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_POKE2DCHECK_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	POKE2DCHECK_WORK	*p_wk	= p_wk_adrs;

	//���W���[���j��
	DEBUGPRINT_Close( &p_wk->print );
	DEBUGPRINT_Exit( &p_wk->print );
	POKEGRA_Exit( &p_wk->pokegra );
	SEQ_Exit( &p_wk->seq );

	//�O���t�B�b�N�j��
	GRAPHIC_Exit( &p_wk->graphic );

	//���ʂŎg�����W���[���j��
	PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_font );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );
	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_NAGI_DEBUG_SUB );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����QD�`�F�b�N	���C���v���Z�X����
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_POKE2DCHECK_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	POKE2DCHECK_WORK	*p_wk	= p_wk_adrs;

	//�V�[�P���X
	SEQ_Main( &p_wk->seq );

	//�O���t�B�b�N�`��
	GRAPHIC_Main( &p_wk->graphic );

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
		GX_VRAM_BG_128_A,						// ���C��2D�G���W����BG
		GX_VRAM_BGEXTPLTT_NONE,     // ���C��2D�G���W����BG�g���p���b�g
		GX_VRAM_SUB_BG_128_C,				// �T�u2D�G���W����BG
		GX_VRAM_SUB_BGEXTPLTT_NONE, // �T�u2D�G���W����BG�g���p���b�g
		GX_VRAM_OBJ_128_B,					// ���C��2D�G���W����OBJ
		GX_VRAM_OBJEXTPLTT_NONE,		// ���C��2D�G���W����OBJ�g���p���b�g
		GX_VRAM_SUB_OBJ_128_D,       // �T�u2D�G���W����OBJ
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

	//�f�B�X�v���CON
	GFL_DISP_SetDispSelect( GX_DISP_SELECT_SUB_MAIN );
	GFL_DISP_SetDispOn();

	//�\��
	GFL_DISP_GX_InitVisibleControl();
	GFL_DISP_GXS_InitVisibleControl();

	//�t�H���g������
	GFL_FONTSYS_Init();

	//���W���[��������
	GRAPHIC_BG_Init( &p_wk->bg, heapID );
	GRAPHIC_OBJ_Init( &p_wk->obj, &sc_vramSetTable, heapID );

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
	GRAPHIC_OBJ_Exit( &p_wk->obj );
	GRAPHIC_BG_Exit( &p_wk->bg );

	//�f�t�H���g�F�֖߂�
	GFL_FONTSYS_SetDefaultColor();

	//���W�X�^������
	G2_BlendNone();
	G2S_BlendNone();	
	GX_SetVisibleWnd( GX_WNDMASK_NONE );
	GXS_SetVisibleWnd( GX_WNDMASK_NONE );

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
static void GRAPHIC_Main( GRAPHIC_WORK *p_wk )
{	
	GRAPHIC_OBJ_Main( &p_wk->obj );
	GRAPHIC_BG_Main( &p_wk->bg );
}
//----------------------------------------------------------------------------
/**
 *	@brief	CLUNIT�擾
 *
 *	@param	const GRAPHIC_WORK *cp_wk	���[�N
 *
 *	@return	CLUNIT
 */
//-----------------------------------------------------------------------------
static GFL_CLUNIT* GRAPHIC_GetClunit( const GRAPHIC_WORK *cp_wk )
{	
	return GRAPHIC_OBJ_GetClunit( &cp_wk->obj );
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
	GRAPHIC_OBJ_VBlankFunction( &p_wk->obj );
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
		GFL_BG_SetBGMode( &sc_bgsys_header );
	}

	//BG�ʐݒ�
	{	
		int i;
		for( i = 0; i < NELEMS(sc_bgsetup); i++ )
		{	
			GFL_BG_SetBGControl( sc_bgsetup[i].frame, &sc_bgsetup[i].bgcnt_header, sc_bgsetup[i].mode );
			GFL_BG_ClearFrame( sc_bgsetup[i].frame );
			GFL_BG_SetVisible( sc_bgsetup[i].frame, sc_bgsetup[i].is_visible );
		}
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
	//BG�ʔj��
	{	
		int i;
		for( i = 0; i < NELEMS(sc_bgsetup); i++ )
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
 *	@brief	BG	���C������
 *
 *	@param	GRAPHIC_BG_WORK *p_wk		���[�N
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_BG_Main( GRAPHIC_BG_WORK *p_wk )
{	
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
 *				OBJ
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	������
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk			���[�N
 *	@param	GFL_DISP_VRAM* cp_vram_bank	�o���N�e�[�u��
 *	@param	heapID											�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Init( GRAPHIC_OBJ_WORK *p_wk, const GFL_DISP_VRAM* cp_vram_bank, HEAPID heapID )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );

	//�V�X�e���쐬
	GFL_CLACT_SYS_Create( &sc_clsys_init, cp_vram_bank, heapID );
	p_wk->p_clunit	= GFL_CLACT_UNIT_Create( GRAPHIC_OBJ_CLWK_CREATE_MAX, 0, heapID );
	GFL_CLACT_UNIT_SetDefaultRend( p_wk->p_clunit );

	//�\��
	GFL_DISP_GX_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );
	GFL_DISP_GXS_SetVisibleControl( GX_PLANEMASK_OBJ, VISIBLE_ON );

}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	�j��
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Exit( GRAPHIC_OBJ_WORK *p_wk )
{	
	//�V�X�e���j��
	GFL_CLACT_UNIT_Delete( p_wk->p_clunit );
	GFL_CLACT_SYS_Delete();
	GFL_STD_MemClear( p_wk, sizeof(GRAPHIC_OBJ_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	���C������
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_Main( GRAPHIC_OBJ_WORK *p_wk )
{	
	GFL_CLACT_SYS_Main();
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��	V�u�����N����
 *
 *	@param	GRAPHIC_OBJ_WORK *p_wk	���[�N
 *
 */
//-----------------------------------------------------------------------------
static void GRAPHIC_OBJ_VBlankFunction( GRAPHIC_OBJ_WORK *p_wk )
{	
	GFL_CLACT_SYS_VBlankFunc();
}
//----------------------------------------------------------------------------
/**
 *	@brief	OBJ�`��CLUNIT�擾
 *
 *	@param	const GRAPHIC_OBJ_WORK *cp_wk	���[�N
 *
 *	@return	CLUNIT
 */
//-----------------------------------------------------------------------------
static GFL_CLUNIT* GRAPHIC_OBJ_GetClunit( const GRAPHIC_OBJ_WORK *cp_wk )
{	
	return cp_wk->p_clunit;
}
//=============================================================================
/**
 *	�|�P�����̊G
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����̊G������
 *
 *	@param	POKEGRA_WORK *p_wk	���[�N
 *	@param	*clunit		CLUNIT
 *	@param	heapID		�q�[�vID
 */
//-----------------------------------------------------------------------------
static void POKEGRA_Init( POKEGRA_WORK *p_wk, GFL_CLUNIT *clunit, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(POKEGRA_WORK) );

	//���\�[�X������
	{	
		int i;
		for( i = 0; i < RESID_MAX; i++ )
		{	
			p_wk->res[i]	= GFL_CLGRP_REGISTER_FAILED;
		}
	}

	//���\�[�X�쐬
	//�f�t�H���g�͂ӂ�������
	{	
		POKEGRA_DATA pokedata;
		GFL_STD_MemClear( &pokedata, sizeof(POKEGRA_DATA) );
		pokedata.mons_no	= MONSNO_HUSIGIDANE;
		POKEGRA_ReLoad( p_wk, clunit, &pokedata, heapID );
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����G�j��
 *
 *	@param	POKEGRA_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void POKEGRA_Exit( POKEGRA_WORK *p_wk )
{	
	int i;
	for( i = 0; i < CLWKID_MAX; i++ )
	{	
		//���\�[�X�j��
		if( p_wk->res[RESID_OBJ_PM1_CHR + i] != GFL_CLGRP_REGISTER_FAILED )
		{	
			GFL_CLGRP_CGR_Release( p_wk->res[RESID_OBJ_PM1_CHR + i] );
		}
		if( p_wk->res[RESID_OBJ_PM1_PLT + i] != GFL_CLGRP_REGISTER_FAILED )
		{	
			GFL_CLGRP_PLTT_Release( p_wk->res[RESID_OBJ_PM1_PLT + i] );
		}
		if( p_wk->res[RESID_OBJ_PM1_CEL + i] != GFL_CLGRP_REGISTER_FAILED )
		{	
			GFL_CLGRP_CELLANIM_Release( p_wk->res[RESID_OBJ_PM1_CEL + i] );
		}
	}

	GFL_STD_MemClear( p_wk, sizeof(POKEGRA_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����̊G���C��
 *
 *	@param	POKEGRA_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void POKEGRA_Main( POKEGRA_WORK *p_wk )
{	

}
//----------------------------------------------------------------------------
/**
 *	@brief	�|�P�����̊G�ēǂݍ���
 *
 *	@param	POKEGRA_WORK *p_wk				���[�N
 *	@param	*clunit										���j�b�g
 *	@param	POKEGRA_DATA *cp_data			�f�[�^
 *	@param	heapID										�q�[�vID
 */
//-----------------------------------------------------------------------------
static void POKEGRA_ReLoad( POKEGRA_WORK *p_wk, GFL_CLUNIT *clunit, const POKEGRA_DATA *cp_data, HEAPID heapID )
{	
	int i;
	for( i = 0; i < CLWKID_MAX; i++ )
	{	
		//���\�[�X�j��
		if( p_wk->res[RESID_OBJ_PM1_CHR + i] != GFL_CLGRP_REGISTER_FAILED )
		{	
			GFL_CLGRP_CGR_Release( p_wk->res[RESID_OBJ_PM1_CHR + i] );
		}
		if( p_wk->res[RESID_OBJ_PM1_PLT + i] != GFL_CLGRP_REGISTER_FAILED )
		{	
			GFL_CLGRP_PLTT_Release( p_wk->res[RESID_OBJ_PM1_PLT + i] );
		}
		if( p_wk->res[RESID_OBJ_PM1_CEL + i] != GFL_CLGRP_REGISTER_FAILED )
		{	
			GFL_CLGRP_CELLANIM_Release( p_wk->res[RESID_OBJ_PM1_CEL + i] );
		}
	
		//CLWK�j��
		if( p_wk->p_clwk[ i ] )
		{	
			GFL_CLACT_WK_Remove( p_wk->p_clwk[ i ] );
		}
	}
	
	//���\�[�X�ǂ݂���
	{	
		ARCHANDLE *p_handle	= POKE2DGRA_OpenHandle( heapID );
		int dir;
		int sex;
	
		for( i = 0; i < CLWKID_MAX; i++ )
		{	
			dir = i;
			if( cp_data->sex == 0 )
			{	
				sex	= PTL_SEX_MALE;
			}
			else if( cp_data->sex == 1 )
			{	
				sex	= PTL_SEX_FEMALE;
			}
			p_wk->res[RESID_OBJ_PM1_CHR + i]	= POKE2DGRA_OBJ_CGR_Register( p_handle, cp_data->mons_no, cp_data->form_no, sex, cp_data->rare, dir, FALSE, CLSYS_DRAW_SUB, heapID );
			p_wk->res[RESID_OBJ_PM1_PLT + i]	= POKE2DGRA_OBJ_PLTT_Register( p_handle, cp_data->mons_no, cp_data->form_no, sex, cp_data->rare, dir, FALSE, CLSYS_DRAW_SUB, (i+PLTID_OBJ_POKE1_S)*0x20, heapID );
			p_wk->res[RESID_OBJ_PM1_CEL + i]	= POKE2DGRA_OBJ_CELLANM_Register( cp_data->mons_no, cp_data->form_no, sex, cp_data->rare, dir, FALSE, APP_COMMON_MAPPING_128K, CLSYS_DRAW_SUB, heapID );
		}
		GFL_ARC_CloseDataHandle( p_handle );
	}
	
	//CLWK�ǂ݂���
	for( i = 0; i < CLWKID_MAX; i++ )
	{	
		GFL_CLWK_DATA	cldata;
		GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
		cldata.pos_x	= sc_pokemon_pos[i].x;
		cldata.pos_y	= sc_pokemon_pos[i].y;
		p_wk->p_clwk[ i ]	= GFL_CLACT_WK_Create( clunit,
				p_wk->res[RESID_OBJ_PM1_CHR+i],
				p_wk->res[RESID_OBJ_PM1_PLT+i],
				p_wk->res[RESID_OBJ_PM1_CEL+i],
				&cldata,
				CLSYS_DEFREND_SUB,
				heapID
					);
	}
}
//=============================================================================
/**
 *						DEBUGPRINT
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�	������
 *
 *	@param	frm											�t���[��
 *	@param	heapID									�q�[�vID
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Init( DEBUGPRINT_WORK *p_wk, u8 frm, u8 w, u8 h, HEAPID heapID )
{	
	GFL_STD_MemClear( p_wk, sizeof(DEBUGPRINT_WORK) );
	p_wk->heapID						= heapID;
	p_wk->frm								= frm;
	p_wk->w									= w;
	p_wk->h									= h;

	//�f�o�b�O�v�����g�p�t�H���g
	p_wk->p_font	= GFL_FONT_Create( ARCID_FONT,
				NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, heapID );	

	p_wk->p_strbuf	= GFL_STR_CreateBuffer( DEBUGPRINT_STRBUFF_LEN, heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�	�j��
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Exit( DEBUGPRINT_WORK *p_wk )
{	
	GFL_STR_DeleteBuffer( p_wk->p_strbuf );
	GFL_FONT_Delete( p_wk->p_font );
	GFL_STD_MemClear( p_wk, sizeof(DEBUGPRINT_WORK) );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�I�[�v��
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Open( DEBUGPRINT_WORK *p_wk )
{	
	//�f�o�b�O�v�����g�p�ݒ�
	//�X�N���[���̍쐬
  {
    u8 x,y;
		u16 buf;
    for( y = 0;y<p_wk->h;y++ )
    {
      for( x=0;x<p_wk->w;x++ )
      {
        buf = x+y*p_wk->w;
        GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
      }
      for( x=p_wk->w;x<32;x++ )
      {
        buf = p_wk->h*p_wk->w;
        GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
      }
    }
		for( y = p_wk->h;y<24;y++ )
    {
	 		buf = p_wk->h*p_wk->w;
 			GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
			for( x=0;x<32;x++ )
      {
        buf = p_wk->h*p_wk->w;
        GFL_BG_WriteScreen( p_wk->frm, &buf, x,y,1,1 );
      }
		}
    GFL_BG_LoadScreenReq( p_wk->frm );
  }
  
  //�p���b�g�̍쐬
  {
    u16 col[4]={ 0xFFFF , 0x0000 , 0x7fff , 0x001f };
    GFL_BG_LoadPalette( p_wk->frm, col, sizeof(u16)*4, 0 );
  }

	//�������ނ��߂�BMP�쐬
	p_wk->p_bmp	= GFL_BMP_CreateInVRAM( GFL_DISPUT_GetCgxPtr(p_wk->frm), p_wk->w, p_wk->h, GFL_BMP_16_COLOR, p_wk->heapID );
	GFL_STD_MemClear16( GFL_DISPUT_GetCgxPtr(p_wk->frm) , p_wk->w*p_wk->h*GFL_BG_1CHRDATASIZ );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�	�I��
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Close( DEBUGPRINT_WORK *p_wk )
{	

	GF_ASSERT( p_wk );

	GFL_BMP_Delete( p_wk->p_bmp );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�ɏ�������
 *
 *	@param	u16 *cp_str							������
 *	@param	x												���WX
 *	@param	y												���WY
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Print( DEBUGPRINT_WORK *p_wk, u16 x, u16 y, const char *cp_str )
{	
  int bufLen = DEBUGPRINT_STRBUFF_LEN-1;
  STRCODE strCode[DEBUGPRINT_STRBUFF_LEN];

  //STD_ConvertStringSjisToUnicode( strCode , &bufLen , cp_str, NULL , NULL );
  DEB_STR_CONV_SjisToStrcode( cp_str , strCode , bufLen );
  strCode[bufLen] = GFL_STR_GetEOMCode();

  GFL_STR_SetStringCode( p_wk->p_strbuf, strCode );

	//��������
	PRINTSYS_Print( p_wk->p_bmp, x, y, p_wk->p_strbuf, p_wk->p_font );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈�ɐ��l��������������
 *
 *	@param	u16 *cp_str							���C�h������i%d��%f���g���Ă��������j
 *	@param	number									����
 *	@param	x												���WX
 *	@param	y												���WY
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_PrintV( DEBUGPRINT_WORK *p_wk, u16 x, u16 y, const char *cp_str, ... )
{	
  char str[DEBUGPRINT_STRBUFF_LEN-1];
  va_list vlist;
  va_start(vlist, cp_str);
  STD_TVSNPrintf( str , DEBUGPRINT_STRBUFF_LEN-1 , cp_str, vlist );
  va_end(vlist);
	DEBUGPRINT_Print( p_wk, x, y, str );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�p�v�����g�̈���N���A�[
 *
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Clear( DEBUGPRINT_WORK *p_wk )
{	
	GFL_BMP_Clear( p_wk->p_bmp, 0 );
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�o�b�O�v�����g���C��
 *
 *	@param	DEBUGPRINT_WORK *p_wk	���[�N
 *	@param	int *p_cursor					���݂̃J�[�\��
 *	@param	POKEGRA_DATA *cp_now	�f�[�^����
 *	@param	POKEGRA_DATA *cp_max	�f�[�^�ő�
 */
//-----------------------------------------------------------------------------
static void DEBUGPRINT_Update( DEBUGPRINT_WORK *p_wk, const int *cp_cursor, const POKEGRA_DATA *cp_now, const POKEGRA_DATA *cp_max )
{	
	enum
	{	
		START_X	= 16,
		START_Y	= 32,
	};

	DEBUGPRINT_Clear( p_wk );

	DEBUGPRINT_PrintV( p_wk, START_X, START_Y+*cp_cursor*16, "��" );
	DEBUGPRINT_PrintV( p_wk, START_X+16, START_Y, "�}�Ӕԍ��F%d/%d" , cp_now->mons_no,
			cp_max->mons_no );
	DEBUGPRINT_PrintV( p_wk, START_X+16, START_Y+16, "�t�H�����F%d/%d" , cp_now->form_no,
			cp_max->form_no );
	DEBUGPRINT_PrintV( p_wk, START_X+16, START_Y+32, "�����ׂF%d/%d" , cp_now->sex,
			cp_max->sex );
	DEBUGPRINT_PrintV( p_wk, START_X+16, START_Y+48, "���A�F%d/%d" , cp_now->rare,
				cp_max->rare );

	DEBUGPRINT_Print( p_wk, START_X+32, 112, "�㉺�őI��" );
	DEBUGPRINT_Print( p_wk, START_X+32, 128, "���E�ŕύX" );
	DEBUGPRINT_Print( p_wk, START_X+32, 144, "LR��10���ύX" );
	DEBUGPRINT_Print( p_wk, START_X+32, 160, "SELECT�ŏI��" );
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
	enum
	{	
		UPDATE_NONE		= 0,
		UPDATE_CHANGE	= 1<<0,
		UPDATE_CUSOR	= 1<<1,
	};

	POKE2DCHECK_WORK	*p_wk	= p_param;
	int is_update;

	is_update	= UPDATE_NONE;

	//�|�P�����ԍ�	MONSNO_MAX
	//�t�H����		POKEPER_ID_form_max	
	//�����ׂ�		POKEPER_ID_sex
	//���A				off. on
	
	//�L�[����
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_UP )
	{
		p_wk->cursor--;
		p_wk->cursor	= p_wk->cursor < 0 ? POKEDATA_PARAM_MAX-1: p_wk->cursor;

		is_update	= UPDATE_CUSOR;
	}
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_DOWN )
	{
		p_wk->cursor++;
		p_wk->cursor	%= POKEDATA_PARAM_MAX;

		is_update	= UPDATE_CUSOR;
	}
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_LEFT
		|| GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
	{
		p_wk->now.a[ p_wk->cursor ]--;
		p_wk->now.a[ p_wk->cursor ]	= MATH_ROUND( p_wk->now.a[ p_wk->cursor ], p_wk->min.a[ p_wk->cursor ], p_wk->max.a[ p_wk->cursor ]);
		is_update	= UPDATE_CHANGE;
	}
	if( GFL_UI_KEY_GetTrg() & PAD_KEY_RIGHT
		|| GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
	{
		p_wk->now.a[ p_wk->cursor ]++;
		p_wk->now.a[ p_wk->cursor ]	= MATH_ROUND( p_wk->now.a[ p_wk->cursor ], p_wk->min.a[ p_wk->cursor ], p_wk->max.a[ p_wk->cursor ]);
		is_update	= UPDATE_CHANGE;
	}
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_L )
	{
		p_wk->now.a[ p_wk->cursor ]-=10;
		p_wk->now.a[ p_wk->cursor ]	= MATH_ROUND( p_wk->now.a[ p_wk->cursor ], p_wk->min.a[ p_wk->cursor ], p_wk->max.a[ p_wk->cursor ]);
		is_update	= UPDATE_CHANGE;
	}
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_R )
	{
		p_wk->now.a[ p_wk->cursor ]+=10;
		p_wk->now.a[ p_wk->cursor ]	= MATH_ROUND( p_wk->now.a[ p_wk->cursor ], p_wk->min.a[ p_wk->cursor ], p_wk->max.a[ p_wk->cursor ]);
		is_update	= UPDATE_CHANGE;
	}

	//�|�P�����ǂݑւ�
	if( is_update & UPDATE_CHANGE )
	{	
		GFL_CLUNIT *clunit	= GRAPHIC_GetClunit( &p_wk->graphic );
		POKEGRADATA_GetMinMax( &p_wk->min, &p_wk->max, &p_wk->now, HEAPID_NAGI_DEBUG_SUB );
		POKEGRA_ReLoad( &p_wk->pokegra, clunit, &p_wk->now, HEAPID_NAGI_DEBUG_SUB );
	}

	//�����`��UPDATE
	if( is_update )
	{	
		DEBUGPRINT_Update( &p_wk->print, &p_wk->cursor, &p_wk->now, &p_wk->max );
	}


	//�I��
	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_SELECT )
	{
		SEQ_SetNext( p_seqwk, SEQFUNC_FadeIn );
	}
}

//----------------------------------------------------------------------------
/**
 *	@brief BG�ǂ݂���
 */
//-----------------------------------------------------------------------------
static void BG_LoadResource( HEAPID heapID )
{	
	//�f�ޓǂݍ���
	ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_DEBUG_NAGI, heapID );

	//PLT
	GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_d_nagi_graphic_back_NCLR,
			PALTYPE_SUB_BG, PLTID_BG_BACK_S*0x20, 0, heapID );

	//CHR
	GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_d_nagi_graphic_back_NCGR, 
			BG_FRAME_BACK_S, 0, 0, FALSE, heapID );	

	GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_d_nagi_graphic_back_NSCR,
			BG_FRAME_BACK_S, 0, 0, FALSE, heapID );

	GFL_ARC_CloseDataHandle( p_handle );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�ŏ��l�ƍő�l���擾
 *
 *	@param	POKEGRA_DATA *p_min			�ŏ�
 *	@param	POKEGRA_DATA *p_max			�ő�
 *	@param	POKEGRA_DATA *cp_now		���݂̒l
 */
//-----------------------------------------------------------------------------
static void POKEGRADATA_GetMinMax( POKEGRA_DATA *p_min, POKEGRA_DATA *p_max, POKEGRA_DATA *p_now, HEAPID heapID )
{	
	GFL_STD_MemClear( p_min, sizeof(POKEGRA_DATA) );
	GFL_STD_MemClear( p_max, sizeof(POKEGRA_DATA) );
	p_min->mons_no	= 1;
	p_min->rare			= 0;
	p_min->form_no	= 0;
	p_max->mons_no	= MONSNO_END;
	p_max->rare			= 1;

	if( p_now )
	{	
		POKEMON_PERSONAL_DATA*	p_ppd;
		p_ppd	= POKE_PERSONAL_OpenHandle( p_now->mons_no, p_now->form_no, heapID );
	
		p_max->form_no	= POKE_PERSONAL_GetParam( p_ppd, POKEPER_ID_form_max );
		{	
			u32 sex;
			sex	= POKE_PERSONAL_GetParam( p_ppd, POKEPER_ID_sex );
			switch( sex )
			{	
			case POKEPER_SEX_MALE:
			case POKEPER_SEX_UNKNOWN:
				p_min->sex			= 0;
				p_max->sex			= 0;
				p_now->sex			= 0;
				break;
			case POKEPER_SEX_FEMALE:
				p_min->sex			= 1;
				p_max->sex			= 1;
				p_now->sex			= 1;
				break;
			default:
				p_max->sex			= 0;
				p_max->sex			= 1;
			}
		}

		POKE_PERSONAL_CloseHandle( p_ppd );
	}
}
