//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_bvdelete_proc.c
 *	@brief	�o�g���r�f�I�����v���Z�X
 *	@author	Toru=Nagihashi
 *	@data		2009.11.13
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//���C�u����
#include <gflib.h>

//�V�X�e��
#include "system/gfl_use.h"
#include "system/main.h"  //HEAPID

//�������W���[��
#include "br_util.h"

//�O���Q��
#include "br_bvdelete_proc.h"

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
///	�o�g���r�f�I�������C�����[�N
//=====================================
typedef struct 
{
  BR_SEQ_WORK           *p_seq;
	HEAPID                heapID;
  BR_BVDELETE_PROC_PARAM	*p_param;
} BR_BVDELETE_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	BR�R�A�v���Z�X
//=====================================
static GFL_PROC_RESULT BR_BVDELETE_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_BVDELETE_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_BVDELETE_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
///	�V�[�P���X
//=====================================
static void Br_BvDelete_Seq_FadeIn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BvDelete_Seq_FadeOut( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BvDelete_Seq_Main( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );


//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	�o�g���r�f�I�����v���Z�X�v���Z�X
//=====================================
const GFL_PROC_DATA BR_BVDELETE_ProcData =
{	
	BR_BVDELETE_PROC_Init,
	BR_BVDELETE_PROC_Main,
	BR_BVDELETE_PROC_Exit,
};

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================

//=============================================================================
/**
 *					�o�g���r�f�I�����v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�����v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVDELETE_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BVDELETE_WORK				*p_wk;
	BR_BVDELETE_PROC_PARAM	*p_param	= p_param_adrs;

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_BVDELETE_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_BVDELETE_WORK) );	
  p_wk->p_param = p_param;
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );

	//�O���t�B�b�N������
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_BVDELETE_S, p_wk->heapID );

  //���W���[��������
  p_wk->p_seq = BR_SEQ_Init( p_wk, Br_BvDelete_Seq_FadeIn, p_wk->heapID );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�����v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVDELETE_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BVDELETE_WORK				*p_wk	= p_wk_adrs;
	BR_BVDELETE_PROC_PARAM	*p_param	= p_param_adrs;

	//���W���[���j��
  BR_SEQ_Exit( p_wk->p_seq );

  //�O���t�B�b�N�j��
  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_BVDELETE_S );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�����v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVDELETE_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	

	BR_BVDELETE_WORK	*p_wk	= p_wk_adrs;

  BR_SEQ_Main( p_wk->p_seq );
  if( BR_SEQ_IsEnd( p_wk->p_seq ) )
  { 
    return GFL_PROC_RES_FINISH;
  }

	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *  �V�[�P���X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�C��
 *
 *	@param	BR_SEQ_WORK *p_seqwk    �V�[�P���X���[�N
 *	@param	*p_seq                  �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs              ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BvDelete_Seq_FadeIn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_FADEIN_END,
  };

  BR_BVDELETE_WORK	*p_wk	= p_wk_adrs;

	//�v���Z�X����
  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    *p_seq  = SEQ_FADEIN_WAIT;
    break;

  case SEQ_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      *p_seq  = SEQ_FADEIN_END;
    }
    break;

  case SEQ_FADEIN_END:
    BR_SEQ_SetNext( p_seqwk, Br_BvDelete_Seq_Main );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�A�E�g
 *
 *	@param	BR_SEQ_WORK *p_seqwk    �V�[�P���X���[�N
 *	@param	*p_seq                  �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs              ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BvDelete_Seq_FadeOut( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_FADEOUT_END,
  };

  BR_BVDELETE_WORK	*p_wk	= p_wk_adrs;

  //�v���Z�X����
  switch( *p_seq )
  { 
  case SEQ_FADEOUT_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_FADEOUT_WAIT;
    break;

  case SEQ_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      *p_seq  = SEQ_FADEOUT_END;
    }
    break;

  case SEQ_FADEOUT_END:
    BR_PROC_SYS_Pop( p_wk->p_param->p_procsys );
    BR_SEQ_End( p_seqwk );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���r�f�I�����I��
 *
 *	@param	BR_SEQ_WORK *p_seqwk    �V�[�P���X���[�N
 *	@param	*p_seq                  �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs              ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BvDelete_Seq_Main( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_LOAD_BTLVIDEO,

  };

  BR_BVDELETE_WORK  *p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_LOAD_BTLVIDEO:
     break;

  }
}
