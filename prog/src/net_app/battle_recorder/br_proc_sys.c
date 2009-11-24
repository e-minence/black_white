//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_proc.c
 *	@brief	�o�g�����R�[�_�[�v���Z�X�q����Ǘ�
 *	@author	Toru=Nagihashi
 *	@data		2009.11.12
 *
 *	�X�^�b�N���v���Z�X�q����Ǘ��ł��B
 *	�O�Őݒ肵���e�[�u���̃C���f�b�N�X��n�����ƂŁA�J�n���A
 *	�v���Z�X���ŁAPUSHorPOP���邱�ƂŁA�q�����Ă����܂��B
 *	�����͕ێ����ꂸ�A�ȑO��PROCID�̂ݕۑ����܂�
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID

//�O�����
#include "br_proc_sys.h"
//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define BR_PROC_SYS_STACK_MAX	(5)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X���	
//=====================================
typedef struct _BR_PROC_SYS_WORK
{
	BOOL											is_use;
	void											*p_param;
	const BR_PROC_SYS_DATA		*cp_data;
	u32												procID;
} BR_PROC_WORK;

//-------------------------------------
///	�v���Z�X�q����Ǘ����[�N
//=====================================
struct _BR_PROC_SYS
{
	GFL_PROCSYS					*p_procsys;
	const BR_PROC_SYS_DATA	*cp_procdata_tbl;
	u16									tbl_max;
	void								*p_wk_adrs;
	HEAPID							heapID;
	BOOL								is_end;
	u32									seq;

	BR_PROC_WORK				prev_proc;
	BR_PROC_WORK				now_proc;
	BR_PROC_WORK				next_proc;
	u32									stackID[BR_PROC_SYS_STACK_MAX];
	u32									stack_num;
};
//=============================================================================
/**
 *					�v���g�^�C�v
*/
//=============================================================================
static void BR_PROC_Clear( BR_PROC_WORK *p_wk );

//=============================================================================
/**
 *					�V�X�e��
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�v���Z�X�Ǘ�	������
 *
 *	@param	u16 startID		�J�nPROC
 *	@param	BR_PROC_SYS_DATA *cp_procdata_tbl	�v���Z�X�e�[�u��
 *	@param	tbl_max												�e�[�u���ő�
 *	@param	p_wk_adrs											���[�N�A�h���X�iALLOC,FREE_FUNC�ɓn���l�j
 *	@param	heapID												�q�[�vID
 *
 *	@return	���[�N
 */
//-----------------------------------------------------------------------------
BR_PROC_SYS * BR_PROC_SYS_Init( u16 startID, const BR_PROC_SYS_DATA *cp_procdata_tbl, u16 tbl_max, void *p_wk_adrs, HEAPID heapID )
{	
	BR_PROC_SYS *p_wk;

	p_wk	= GFL_HEAP_AllocMemory( heapID, sizeof(BR_PROC_SYS) );
	GFL_STD_MemClear( p_wk, sizeof(BR_PROC_SYS) );
	p_wk->heapID					= heapID;
	p_wk->tbl_max					= tbl_max;
	p_wk->p_wk_adrs				= p_wk_adrs;
	p_wk->cp_procdata_tbl	= cp_procdata_tbl;

	p_wk->p_procsys				= GFL_PROC_LOCAL_boot( heapID );


	BR_PROC_Clear( &p_wk->now_proc );
	BR_PROC_Clear( &p_wk->next_proc );
	BR_PROC_Clear( &p_wk->prev_proc );

	//�ŏ��̃v���Z�X��ς�
	BR_PROC_SYS_Push( p_wk, startID );
	return p_wk;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���Z�X�Ǘ�	�j��
 *
 *	@param	BR_PROC_SYS *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_PROC_SYS_Exit( BR_PROC_SYS *p_wk )
{	
	GFL_PROC_LOCAL_Exit( p_wk->p_procsys );
	GFL_HEAP_FreeMemory( p_wk );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���Z�X�Ǘ�	���C������
 *
 *	@param	BR_PROC_SYS *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_PROC_SYS_Main( BR_PROC_SYS *p_wk )
{	
	enum
	{	
		SEQ_INIT,
		SEQ_PROC_BEFORE,
		SEQ_PROC_MAIN,
		SEQ_PROC_AFTER,
		SEQ_END,
	};
	BR_PROC_WORK	*p_now	= &p_wk->now_proc;
	BR_PROC_WORK	*p_prev	= &p_wk->prev_proc;
	BR_PROC_WORK	*p_next	= &p_wk->next_proc;


	switch( p_wk->seq )
	{	
	case SEQ_INIT:
		//�f�[�^��ۑ�����
		*p_prev	= *p_now;
		*p_now	=	*p_next;

		p_wk->seq	= SEQ_PROC_BEFORE;
		break;

	case SEQ_PROC_BEFORE:
		GF_ASSERT( p_now->is_use );
		GF_ASSERT( p_now->cp_data->before_func );
		//�O�����֐����Ă�
		p_now->p_param	= GFL_HEAP_AllocMemory( p_wk->heapID, p_now->cp_data->param_size );
		p_now->cp_data->before_func( p_now->p_param, p_wk->p_wk_adrs, p_prev->p_param, p_prev->procID );
		//�ȑO�̃f�[�^�̃p�����[�^������
		if( p_prev->p_param )
		{	
			GFL_HEAP_FreeMemory( p_prev->p_param );
		}

		//�v���Z�X���Ă�
		GFL_PROC_LOCAL_CallProc( p_wk->p_procsys, NO_OVERLAY_ID,
					p_now->cp_data->cp_proc_data, p_now->p_param );	

		p_wk->seq	= SEQ_PROC_MAIN;
		break;

	case SEQ_PROC_MAIN:
		//�v���Z�X������
		{	
			GFL_PROC_MAIN_STATUS	result;
			result	= GFL_PROC_LOCAL_Main( p_wk->p_procsys );
			if( result == GFL_PROC_MAIN_NULL )
			{	
				p_wk->seq	= SEQ_PROC_AFTER;
			}
		}
		break;

	case SEQ_PROC_AFTER:
		GF_ASSERT( p_now->is_use );
		//�㏈���֐����Ă�
		if( p_now->cp_data->after_func )
		{	
			p_now->cp_data->after_func( p_now->p_param, p_wk->p_wk_adrs );
		}

		//�����X�^�b�N�ɐς�ł�������A���̃v���Z�X��
		//�X�^�b�N�ɐς�łȂ�������A�I��
		if( p_wk->stack_num != 0 )
		{	
			p_wk->seq	= SEQ_INIT;
		}
		else
		{	
			//�ȑO�̃f�[�^�̃p�����[�^������
			if( p_now->p_param )
			{	
				GFL_HEAP_FreeMemory( p_now->p_param );
			}
			p_wk->seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		p_wk->is_end	= TRUE;
		break;
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���Z�X�Ǘ�	�I���`�F�b�N
 *
 *	@param	const BR_PROC_SYS *cp_wk		���[�N
 *
 *	@return	TRUE�ŏI��	FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
BOOL BR_PROC_SYS_IsEnd( const BR_PROC_SYS *cp_wk )
{	
	return cp_wk->is_end;
}
//=============================================================================
/**
 *						PROC�̒��Ŏg��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	PROC���֐�	�p�����[�^�擾
 *
 *	@param	BR_PROC_SYS *p_wk ���[�N
 *
 *	@return	�p�����[�^�擾
 */
//-----------------------------------------------------------------------------
void * BR_PROC_SYS_GetParam( const BR_PROC_SYS *cp_wk )
{	
	return cp_wk->now_proc.p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PROC���֐�	�q�[�vID�擾
 *
 *	@param	const BR_PROC_SYS *cp_wk	���[�N
 *
 *	@return	HEAPID	�q�[�vID
 */
//-----------------------------------------------------------------------------
HEAPID BR_PROC_SYS_GetHeapID( const BR_PROC_SYS *cp_wk )
{	
	return cp_wk->heapID;
}
//----------------------------------------------------------------------------
/**
 *	@brief	PROC���֐�	�X�^�b�N����~�낷
 *
 *	@param	BR_PROC_SYS *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
void BR_PROC_SYS_Pop( BR_PROC_SYS *p_wk )
{	

	p_wk->stack_num--;

	if( p_wk->stack_num != 0 )
	{	
		const u16 procID	= p_wk->stackID[ p_wk->stack_num-1 ];
		const BR_PROC_SYS_DATA	*cp_data	= &p_wk->cp_procdata_tbl[procID];
		BR_PROC_WORK	proc;

		//PROC�쐬
		GFL_STD_MemClear( &proc, sizeof(BR_PROC_WORK) );
		proc.is_use		= TRUE;
		proc.p_param	= NULL;	//�����͎��ۂ̈ړ����ɍ쐬�����
		proc.procID		= procID;
		proc.cp_data	= cp_data;

		p_wk->next_proc	= proc;
	}
	else
	{	
		BR_PROC_Clear( &p_wk->next_proc );
	}

}
//----------------------------------------------------------------------------
/**
 *	@brief	PROC���֐�	�X�^�b�N�ɐς�
 *
 *	@param	BR_PROC_SYS *p_wk ���[�N
 *	@param	procID			�X�^�b�N�ɐς�ID
 */
//-----------------------------------------------------------------------------
void BR_PROC_SYS_Push( BR_PROC_SYS *p_wk, u16 procID )
{	
	int i;
	BR_PROC_WORK	proc;
	const BR_PROC_SYS_DATA	*cp_data	= &p_wk->cp_procdata_tbl[procID];
	
	GF_ASSERT( procID < p_wk->tbl_max );

	//PROC�쐬
	{	
		GFL_STD_MemClear( &proc, sizeof(BR_PROC_WORK) );
		proc.is_use		= TRUE;
		proc.p_param	= NULL;	//�����͎��ۂ̈ړ����ɍ쐬�����
		proc.procID		= procID;
		proc.cp_data	= cp_data;
	}

	//�X�^�b�N�ɐς�
	GF_ASSERT( p_wk->stack_num < BR_PROC_SYS_STACK_MAX );
	p_wk->stackID[ p_wk->stack_num ]	= procID;
	p_wk->stack_num++;

	//�v���Z�X�Ȃ����
	p_wk->next_proc	= proc;
}

//=============================================================================
/**
 *							PRIVATE
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�v���Z�X��������
 *
 *	@param	BR_PROC_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BR_PROC_Clear( BR_PROC_WORK *p_wk )
{	
	GFL_STD_MemClear( p_wk, sizeof(BR_PROC_WORK) );
	p_wk->procID	= BR_PROC_SYS_NONE_ID;
}
