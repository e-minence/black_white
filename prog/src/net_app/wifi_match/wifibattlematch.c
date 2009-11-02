//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch.c
 *	@brief	WIFI�̃o�g���}�b�`���
 *	@author	Toru=Nagihashi
 *	@data		2009.11.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	lib
#include <gflib.h>

//	SYSTEM
#include "system/main.h"
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"

//	module
#include "pokeicon/pokeicon.h"

//	archive
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"

//	print
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"


//	mine
#include "wifibattlematch_graphic.h"
#include "net_app/wifibattlematch.h"

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
	//�O���t�B�b�N�ݒ�
	WIFIBATTLEMATCH_GRAPHIC_WORK	*p_graphic;

	//���ʂŎg���t�H���g
	GFL_FONT			*p_font;

	//���ʂŎg���L���[
	PRINT_QUE			*p_que;

	//���ʂŎg�����b�Z�[�W
	GFL_MSGDATA		*p_msg;

	//���ʂŎg���P��
	WORDSET				*p_word;

	//����
	WIFIBATTLEMATCH_PARAM	*p_param;
} WIFIBATTLEMATCH_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	WifiBattleMaptch_ProcData =
{	
	WIFIBATTLEMATCH_PROC_Init,
	WIFIBATTLEMATCH_PROC_Main,
	WIFIBATTLEMATCH_PROC_Exit,
};

//=============================================================================
/**
 *					PRCO
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	���C���v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	WIFIBATTLEMATCH_WORK	*p_wk;

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH, 0x30000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFIBATTLEMATCH_WORK), HEAPID_WIFIBATTLEMATCH );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_WORK) );	

	//�����󂯎��
	p_wk->p_param	= p_param_adrs;

	//�O���t�B�b�N�ݒ�
	p_wk->p_graphic	= WIFIBATTLEMATCH_GRAPHIC_Init( 0, HEAPID_WIFIBATTLEMATCH );

	//���ʃ��W���[���̍쐬
	p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WIFIBATTLEMATCH );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_WIFIBATTLEMATCH );
	p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
												NARC_message_namein_dat, HEAPID_WIFIBATTLEMATCH );
	p_wk->p_word	= WORDSET_Create( HEAPID_WIFIBATTLEMATCH );

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	���C���v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
	WIFIBATTLEMATCH_WORK	*p_wk	= p_wk_adrs;

	//���ʃ��W���[���̔j��
	WORDSET_Delete( p_wk->p_word );
	GFL_MSG_Delete( p_wk->p_msg );
	PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_font );

	//�O���t�B�b�N�j��
	WIFIBATTLEMATCH_GRAPHIC_Exit( p_wk->p_graphic );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH );
	
	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	���C���v���Z�X����
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
	WIFIBATTLEMATCH_WORK	*p_wk	= p_wk_adrs;

	//�`��
	WIFIBATTLEMATCH_GRAPHIC_2D_Draw( p_wk->p_graphic );

	//�v�����g
	PRINTSYS_QUE_Main( p_wk->p_que );

	return GFL_PROC_RES_CONTINUE;
}
