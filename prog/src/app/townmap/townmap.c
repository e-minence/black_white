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
#include "message.naix"
//#include "msg/msg_townmap.h"


//mine
#include "townmap_grh.h"
#include "app/townmap.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
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
	//�V�X�e��
	TOWNMAP_GRAPHIC_SYS	*p_grh;

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

	if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
	{	
		SEQ_SetNext( &p_wk->seq, SEQFUNC_FadeIn );
	}
}
