//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_menu_proc.c
 *	@brief	���j���[�v���Z�X
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

//�����̃��W���[��
#include "br_btn.h"

//�Z�[�u�f�[�^
#include "savedata/battle_rec.h"

//�O���Q��
#include "br_menu_proc.h"

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
///	���j���[���C�����[�N
//=====================================
typedef struct 
{
	BR_BTN_SYS_WORK	*p_btn;

	HEAPID heapID;
} BR_MENU_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	BR�R�A�v���Z�X
//=====================================
static GFL_PROC_RESULT BR_MENU_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_MENU_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_MENU_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	���j���[�v���Z�X�v���Z�X
//=====================================
const GFL_PROC_DATA BR_MENU_ProcData =
{	
	BR_MENU_PROC_Init,
	BR_MENU_PROC_Main,
	BR_MENU_PROC_Exit,
};

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================

//=============================================================================
/**
 *					���j���[�v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_MENU_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_MENU_WORK				*p_wk;
	BR_MENU_PROC_PARAM	*p_param	= p_param_adrs;

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_MENU_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_MENU_WORK) );	
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );

	//�O���t�B�b�N������
  BR_RES_UnLoadCommon( p_param->p_res, CLSYS_DRAW_MAIN );
  BR_RES_UnLoadCommon( p_param->p_res, CLSYS_DRAW_SUB );
  BR_RES_LoadCommon( p_param->p_res, CLSYS_DRAW_MAIN, p_wk->heapID );
  BR_RES_LoadCommon( p_param->p_res, CLSYS_DRAW_SUB, p_wk->heapID );

	//���W���[��������
  { 
    int i;
    BR_BTN_DATA_SETUP setup;
    for( i = 0; i < BR_RECORD_NUM; i++ )
    { 
      setup.is_valid[i] = p_param->cp_btlrec->is_valid[i];
      setup.p_name[i]   = p_param->cp_btlrec->p_name[i];
      setup.sex[i]      = p_param->cp_btlrec->sex[i];
    }
    p_wk->p_btn	= BR_BTN_SYS_Init( p_param->menuID, p_param->p_unit, p_param->p_res, &setup, p_wk->heapID );
  }

  BR_FADE_ALPHA_SetAlpha( p_param->p_fade, BR_FADE_DISPLAY_BOTH, 0 );

	NAGI_Printf( "MENU: Init!\n" );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_MENU_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_MENU_WORK				*p_wk	= p_wk_adrs;
	BR_MENU_PROC_PARAM	*p_param	= p_param_adrs;

	//���W���[���j��
	BR_BTN_SYS_Exit( p_wk->p_btn );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

  //br_btn.c�̒���OFF�ɂ��Ă��邱�Ƃ�����̂�
  GFL_BG_SetVisible( BG_FRAME_M_TEXT, TRUE );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���j���[�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_MENU_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
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

	BR_MENU_WORK	*p_wk	= p_wk_adrs;
	BR_MENU_PROC_PARAM	*p_param	= p_param_adrs;

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
    //�v���Z�X����
    BR_BTN_SYS_Main( p_wk->p_btn );
    if( BR_BTN_SYS_GetInput( p_wk->p_btn, &p_param->next_proc, &p_param->next_mode ) != BR_BTN_SYS_INPUT_NONE )
    { 
      *p_seq  = SEQ_FADEOUT_START;
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
    //�{�^���������ꂽ����
    switch( BR_BTN_SYS_GetInput( p_wk->p_btn, &p_param->next_proc, &p_param->next_mode ) )
    {	
      //���̃v���Z�X�֍s��
    case BR_BTN_SYS_INPUT_CHANGESEQ:
      BR_PROC_SYS_Push( p_param->p_procsys, p_param->next_proc );
      NAGI_Printf( "MENU: Next %d!\n", p_param->next_proc );
      return GFL_PROC_RES_FINISH;

    case BR_BTN_SYS_INPUT_EXIT:
      NAGI_Printf( "MENU: Exit!\n" );
      BR_PROC_SYS_Pop( p_param->p_procsys );
      return GFL_PROC_RES_FINISH;
    }
    break;
  }

  return GFL_PROC_RES_CONTINUE;
}

