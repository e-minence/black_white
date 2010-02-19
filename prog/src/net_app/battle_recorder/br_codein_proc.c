//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_codein_proc.c
 *	@brief	���l���̓v���Z�X
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

//�A�[�J�C�u
#include "msg/msg_battle_rec.h"

//�����̃��W���[��
#include "br_pokesearch.h"
#include "br_inner.h"
#include "br_util.h"
#include "br_btn.h"
#include "br_codein.h"

//�O���Q��
#include "br_codein_proc.h"

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
///	���l���̓��C�����[�N
//=====================================
typedef struct 
{
  BR_CODEIN_PARAM  *p_codein_param;
  BR_CODEIN_WORK   *p_codein_wk;
  BR_TEXT_WORK  *p_text;
  PRINT_QUE     *p_que;
	HEAPID        heapID;
} BR_BR_CODEIN_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	���l���̓v���Z�X
//=====================================
static GFL_PROC_RESULT BR_CODEIN_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_CODEIN_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_CODEIN_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	���l���̓v���Z�X�v���Z�X
//=====================================
const GFL_PROC_DATA BR_CODEIN_ProcData =
{	
	BR_CODEIN_PROC_Init,
	BR_CODEIN_PROC_Main,
	BR_CODEIN_PROC_Exit,
};

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================

//=============================================================================
/**
 *					���l���̓v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���l���̓v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_CODEIN_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BR_CODEIN_WORK				*p_wk;
	BR_CODEIN_PROC_PARAM	*p_param	= p_param_adrs;

  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_BR_CODEIN_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_BR_CODEIN_WORK) );	
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );

  //�O���t�B�b�N
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_CODEIN_NUMBER_S, p_wk->heapID );
  BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_NUM_S, p_wk->heapID );
  BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_NUM_CURSOR_S, p_wk->heapID );
  BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, p_wk->heapID );

  //���W���[��
  {
    int block[BR_CODE_BLOCK_MAX];
    Br_CodeIn_BlockDataMake_2_5_5( block );
    p_wk->p_codein_param  = CodeInput_ParamCreate( p_wk->heapID, 12, p_param->p_unit, p_param->p_res, block );
    p_wk->p_codein_wk     = BR_CODEIN_Init( p_wk->p_codein_param, p_wk->heapID );
  }
  { 
    p_wk->p_que   = PRINTSYS_QUE_Create( p_wk->heapID );
    p_wk->p_text  = BR_TEXT_Init( p_param->p_res, p_wk->p_que, p_wk->heapID );
    BR_TEXT_Print( p_wk->p_text, p_param->p_res, msg_709 );
  }

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���l���̓v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_CODEIN_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BR_CODEIN_WORK				*p_wk	= p_wk_adrs;
	BR_CODEIN_PROC_PARAM	*p_param	= p_param_adrs;

	//���W���[���j��
  { 
    BR_TEXT_Exit( p_wk->p_text, p_param->p_res );
    BR_CODEIN_Exit( p_wk->p_codein_wk );
    CodeInput_ParamDelete( p_wk->p_codein_param );
    PRINTSYS_QUE_Delete( p_wk->p_que );
  }

  //�O���t�B�b�N�j��
  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_CODEIN_NUMBER_S );
  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_NUM_S );
  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_NUM_CURSOR_S );
  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S );


	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���l���̓v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_CODEIN_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_MAIN,
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_END,
  }; 
	BR_BR_CODEIN_WORK	*p_wk	= p_wk_adrs;
	BR_CODEIN_PROC_PARAM	*p_param	= p_param_adrs;

  //�v���Z�X����
  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    *p_seq  = SEQ_FADEIN_WAIT;
    break;

  case SEQ_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_param->p_fade ) )
    { 
      *p_seq  = SEQ_MAIN;
    }
    break;
   
  case SEQ_MAIN:
    { 
      //�R�[�h�C��
      BR_CODEIN_Main( p_wk->p_codein_wk );

      //
      { 
        BR_CODEIN_SELECT select;
        select  = BR_CODEIN_GetSelect( p_wk->p_codein_wk );
        switch( select )
        { 
        case BR_CODEIN_SELECT_CANCEL:
          NAGI_Printf( "CODEIN: Exit!\n" );
          BR_PROC_SYS_Pop( p_param->p_procsys );
          *p_seq  = SEQ_FADEOUT_START;
          break;

        case BR_CODEIN_SELECT_DECIDE:
          NAGI_Printf( "CODEIN: Exit!\n" );
          BR_PROC_SYS_Push( p_param->p_procsys, BR_PROCID_RECORD );
          *p_seq  = SEQ_FADEOUT_START;
          break;
        }
      }
    }
    break;
  case SEQ_FADEOUT_START:
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_FADEOUT_WAIT;
    break;

  case SEQ_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_param->p_fade ) )
    { 
      *p_seq  = SEQ_END;
    }
    break;

  case SEQ_END:
    return GFL_PROC_RES_FINISH;
  }

  //����
  if( p_wk->p_text )
  {  
    BR_TEXT_PrintMain( p_wk->p_text );
  }
  PRINTSYS_QUE_Main( p_wk->p_que );

	return GFL_PROC_RES_CONTINUE;
}

