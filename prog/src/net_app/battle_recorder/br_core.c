//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_core.c
 *	@brief	�o�g�����R�[�_�[�R�A�����i�o�g�����R�[�h�v���C���[�ȊO�̕����j
 *	@author	Toru=Nagihashi
 *	@data		2009.11.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID

//�����̃��W���[��
#include "br_graphic.h"
#include "br_res.h"

//�O���Q��
#include "br_core.h"

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
///	���C�����[�N
//=====================================
typedef struct 
{

	//�O���t�B�b�N
	BR_GRAPHIC_WORK	*p_graphic;

	//���\�[�X�Ǘ�
	BR_RES_WORK			*p_res;

	//����
	BR_CORE_PARAM		*p_param;
} BR_CORE_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	BR�R�A�v���Z�X
//=====================================
static GFL_PROC_RESULT BR_CORE_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_CORE_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_CORE_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	�o�g�����R�[�_�[�R�A�v���Z�X
//=====================================
const GFL_PROC_DATA BR_CORE_ProcData =
{	
	BR_CORE_PROC_Init,
	BR_CORE_PROC_Main,
	BR_CORE_PROC_Exit,
};


//=============================================================================
/**
 *					BR�R�A�v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[�R�A	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_CORE_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_CORE_WORK	*p_wk;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_BATTLE_RECORDER_CORE, 0x10000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_CORE_WORK), HEAPID_BATTLE_RECORDER_CORE );
	GFL_STD_MemClear( p_wk, sizeof(BR_CORE_WORK) );	
	p_wk->p_param		= p_param_adrs;

	//�O���t�B�b�N������
	p_wk->p_graphic	= BR_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_BATTLE_RECORDER_CORE );
	p_wk->p_res			= BR_RES_Init( HEAPID_BATTLE_RECORDER_CORE );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[�R�A	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_CORE_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_CORE_WORK	*p_wk	= p_wk_adrs;

	//���W���[���j��


	//�O���t�B�b�N�j��
	BR_RES_Exit( p_wk->p_res );
	BR_GRAPHIC_Exit( p_wk->p_graphic );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_BATTLE_RECORDER_CORE );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g�����R�[�_�[�R�A	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_CORE_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_CORE_WORK	*p_wk	= p_wk_adrs;

	//�`��
	BR_GRAPHIC_2D_Draw( p_wk->p_graphic );

	return GFL_PROC_RES_CONTINUE;
}
