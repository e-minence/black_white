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
#include "br_snd.h"
#include "br_net.h"

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
	BR_BTN_SYS_WORK	    *p_btn;
  BR_BALLEFF_WORK     *p_balleff;
  GFL_TCB             *p_v_task;
  BR_MENU_PROC_PARAM	*p_param;
  BOOL                is_task_end;
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

static void Br_Menu_Proc_ChangeColorVTask( GFL_TCB *p_tcb, void *p_wk_adrs );

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
  p_wk->p_param = p_param;
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );

	//�O���t�B�b�N������
  BR_RES_UnLoadCommon( p_param->p_res, CLSYS_DRAW_MAIN );
  BR_RES_UnLoadCommon( p_param->p_res, CLSYS_DRAW_SUB );
  BR_RES_LoadCommon( p_param->p_res, CLSYS_DRAW_MAIN, p_wk->heapID );
  BR_RES_LoadCommon( p_param->p_res, CLSYS_DRAW_SUB, p_wk->heapID );

  if( p_param->menuID < BR_BTLVIDEO_MENUID_TOP )
  { 
    BR_RES_LoadBG( p_param->p_res, BR_RES_BG_BROWSE_LOGO_M, p_wk->heapID );
  }
  else
  { 
    BR_RES_LoadBG( p_param->p_res, BR_RES_BG_GDS_LOGO_M, p_wk->heapID );
  }

	//���W���[��������
  { 
    p_wk->p_balleff   = BR_BALLEFF_Init( p_param->p_unit, p_param->p_res, CLSYS_DRAW_SUB, p_wk->heapID );
    p_wk->p_btn	= BR_BTN_SYS_Init( p_param->menuID, p_param->p_unit, p_param->p_res, p_param->cp_saveinfo, p_param->p_btn_recovery, p_wk->p_balleff, p_wk->heapID );
  }

  if( p_param->fade_type == BR_FADE_TYPE_MASTERBRIGHT_AND_ALPHA )
  {
    BR_FADE_ALPHA_SetAlpha( p_param->p_fade, BR_FADE_DISPLAY_SUB, 0 );
  }
  else
  {
    BR_FADE_ALPHA_SetAlpha( p_param->p_fade, BR_FADE_DISPLAY_BOTH, 0 );
  }

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

  if( p_param->menuID < BR_BTLVIDEO_MENUID_TOP )
  { 
    BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_BROWSE_LOGO_M );
  }
  else
  { 
    BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_GDS_LOGO_M );
  }

	//���W���[���j��
	BR_BTN_SYS_Exit( p_wk->p_btn );
  BR_BALLEFF_Exit( p_wk->p_balleff );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

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

    //�ȉ��F�ւ�
    SEQ_COLOR_PLTFADEOUT_START,
    SEQ_COLOR_PLTFADEOUT_WAIT,
    SEQ_COLOR_CHANGE_START,
    SEQ_COLOR_CHANGE_WAIT,
    SEQ_COLOR_PLTFADEIN_START,
    SEQ_COLOR_PLTFADEIN_WAIT,
  };

	BR_MENU_WORK	*p_wk	= p_wk_adrs;
	BR_MENU_PROC_PARAM	*p_param	= p_param_adrs;

  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    BR_FADE_StartFade( p_param->p_fade, p_param->fade_type, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    *p_seq  = SEQ_FADEIN_WAIT;
    break;

  case SEQ_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_param->p_fade ) )
    { 
      *p_seq  = SEQ_MAIN;
    }
    break;
   
  case SEQ_MAIN:
    //�u���E�U���[�h�Ȃ��
    if( p_param->menuID <= BR_BROWSE_MENUID_DELETE_OTHER )
    { 
      if( BR_BTN_SYS_STATE_WAIT == BR_BTN_SYS_GetState( p_wk->p_btn ) )
      { 
        u32 x, y;
        if( GFL_UI_TP_GetPointTrg( &x, &y ) )
        { 
          if( x < 8 || 256-8 <x )
          { 
            PMSND_PlaySE( BR_SND_SE_OK );
            *p_seq  = SEQ_COLOR_PLTFADEOUT_START;
            break;
          }
        }
      }
    }

    //�v���Z�X����
    BR_BTN_SYS_Main( p_wk->p_btn );
    if( BR_BTN_SYS_GetInput( p_wk->p_btn, &p_param->next_proc, &p_param->next_mode ) != BR_BTN_SYS_INPUT_NONE )
    { 
      *p_seq  = SEQ_FADEOUT_START;
    }
    break;

  case SEQ_FADEOUT_START:
    if( p_param->next_proc == BR_PROCID_MUSICAL_SEND )
    { 
      BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_MASTERBRIGHT_AND_ALPHA, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    }
    else
    { 
      BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    }
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

      *p_param->p_result = BR_BTN_SYS_GetExitType( p_wk->p_btn );
      return GFL_PROC_RES_FINISH;
    }
    break;

    //�ȉ��F�ւ�
  case SEQ_COLOR_PLTFADEOUT_START:
    {
      static const sc_change_plt_tbl[]  =
      { 
        BR_RES_COLOR_TYPE_GREEN,
        BR_RES_COLOR_TYPE_PINK,
        BR_RES_COLOR_TYPE_BLACK,
        BR_RES_COLOR_TYPE_YELLOW,
        BR_RES_COLOR_TYPE_RED,
        BR_RES_COLOR_TYPE_DBROWN, //�_�[�N�u���E��
        BR_RES_COLOR_TYPE_BROWN,
      };
      int i;
      BR_RES_COLOR_TYPE color = BR_RES_GetColor( p_param->p_res );

      for( i = 0; i < NELEMS(sc_change_plt_tbl); i++ )
      { 
        if( color == sc_change_plt_tbl[i] )
        { 
          color = sc_change_plt_tbl[ (i+1) % NELEMS(sc_change_plt_tbl) ];
          break;
        }
      }

      BR_RES_ChangeColor( p_param->p_res, color );
    }
    //�F�ւ���̐F�Ńp���b�g�t�F�[�h�J�n
    BR_FADE_PALETTE_SetColor( p_param->p_fade, BR_RES_GetFadeColor( p_param->p_res ) );
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_PALLETE, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_COLOR_PLTFADEOUT_WAIT;
    break;
  case SEQ_COLOR_PLTFADEOUT_WAIT:
    if( BR_FADE_IsEnd(p_param->p_fade) )
    { 
      *p_seq  = SEQ_COLOR_CHANGE_START;
    }
    break;
  case SEQ_COLOR_CHANGE_START:
    p_wk->p_v_task  = GFUser_VIntr_CreateTCB( Br_Menu_Proc_ChangeColorVTask, p_wk, 0 );
    p_wk->is_task_end = FALSE;
    *p_seq  = SEQ_COLOR_CHANGE_WAIT;
    break;
  case SEQ_COLOR_CHANGE_WAIT:
    if( p_wk->is_task_end )
    { 
      GFL_TCB_DeleteTask( p_wk->p_v_task );
      *p_seq  = SEQ_COLOR_PLTFADEIN_START;
    }
    break;
  case SEQ_COLOR_PLTFADEIN_START:
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_PALLETE, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    *p_seq  = SEQ_COLOR_PLTFADEIN_WAIT;
    break;
  case SEQ_COLOR_PLTFADEIN_WAIT:
    if( BR_FADE_IsEnd(p_param->p_fade) )
    { 
      *p_seq  = SEQ_MAIN;
    }
    break;

  }

  BR_BALLEFF_Main( p_wk->p_balleff );
  return GFL_PROC_RES_CONTINUE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �F�ւ��^�X�N
 *
 *	@param	GFL_TCB *p_tcb  TCB
 *	@param	*p_wk_adrs      ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Menu_Proc_ChangeColorVTask( GFL_TCB *p_tcb, void *p_wk_adrs )
{ 
  BR_MENU_WORK	      *p_wk	= p_wk_adrs;
	BR_MENU_PROC_PARAM	*p_param	= p_wk->p_param;

  if( p_wk->is_task_end == FALSE )
  { 
    BR_RES_TransPaletteFade( p_param->p_res, p_param->p_fade, p_wk->heapID );
  }
  p_wk->is_task_end = TRUE;
}
