//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_start_proc.c
 *	@brief	�N���v���Z�X
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

//�o�g�����R�[�_�[�����W���[��
#include "br_util.h"
#include "br_inner.h"
#include "br_snd.h"

//�A�[�J�C�u
#include "msg/msg_battle_rec.h"

//�O���Q��
#include "br_start_proc.h"

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
///	�N�����C�����[�N
//=====================================
typedef struct 
{
  //�V�[�P���X�Ǘ�
  BR_SEQ_WORK     *p_seq;

  //�q�[�vID
	HEAPID          heapID;

  //BMPWIN
  BR_MSGWIN_WORK  *p_here;

  //�v�����g�L���[
  PRINT_QUE       *p_que;

  //�ėp�J�E���^
  u32             cnt;

  //�{�[�����o
  BR_BALLEFF_WORK *p_balleff[CLSYS_DRAW_MAX];

  //����
  BR_START_PROC_PARAM *p_param;
} BR_START_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�N���v���Z�X
//=====================================
static GFL_PROC_RESULT BR_START_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_START_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_START_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	SEQ
//=====================================
static void Br_Start_Seq_Open( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Start_Seq_Close( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Start_Seq_None( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Start_Seq_End( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	�N���v���Z�X�v���Z�X
//=====================================
const GFL_PROC_DATA BR_START_ProcData =
{	
	BR_START_PROC_Init,
	BR_START_PROC_Main,
	BR_START_PROC_Exit,
};

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================

//=============================================================================
/**
 *					�N���v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�N���v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_START_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_START_WORK				*p_wk;
	BR_START_PROC_PARAM	*p_param	= p_param_adrs;

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_START_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_START_WORK) );	
  p_wk->p_param = p_param;
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );

  //���W���[���쐬
  p_wk->p_que       = PRINTSYS_QUE_Create( p_wk->heapID );
  p_wk->p_balleff[CLSYS_DRAW_MAIN]   = BR_BALLEFF_Init( p_param->p_unit, p_param->p_res, CLSYS_DRAW_MAIN, p_wk->heapID );
  p_wk->p_balleff[CLSYS_DRAW_SUB]   = BR_BALLEFF_Init( p_param->p_unit, p_param->p_res, CLSYS_DRAW_SUB, p_wk->heapID );
  BR_BALLEFF_SetAnmSeq( p_wk->p_balleff[CLSYS_DRAW_MAIN], 1 );
  BR_BALLEFF_SetAnmSeq( p_wk->p_balleff[CLSYS_DRAW_SUB], 1 );

  //�V�[�P���X�쐬
  { 
    BR_SEQ_FUNCTION  seq_function;
    if( p_param->mode == BR_START_PROC_MODE_OPEN )
    { 
      seq_function  = Br_Start_Seq_Open;
    }
    else if( p_param->mode == BR_START_PROC_MODE_CLOSE )
    { 
      seq_function  = Br_Start_Seq_Close;
    }
    else
    { 
      seq_function  = Br_Start_Seq_None;
    }
    p_wk->p_seq = BR_SEQ_Init( p_wk, seq_function, p_wk->heapID );
  }

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�N���v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_START_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_START_WORK				*p_wk	= p_wk_adrs;
	BR_START_PROC_PARAM	*p_param	= p_param_adrs;

	//���W���[���j��
  BR_BALLEFF_Exit( p_wk->p_balleff[CLSYS_DRAW_MAIN] );
  BR_BALLEFF_Exit( p_wk->p_balleff[CLSYS_DRAW_SUB] );
  PRINTSYS_QUE_Delete( p_wk->p_que );
  BR_SEQ_Exit( p_wk->p_seq );

  //�O���t�B�b�N�j��

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�N���v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_START_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_START_WORK	*p_wk	= p_wk_adrs;
	BR_START_PROC_PARAM	*p_param	= p_param_adrs;

  //�V�[�P���X
	BR_SEQ_Main( p_wk->p_seq );

  //�����`��
  PRINTSYS_QUE_Main( p_wk->p_que );

  //�{�[�����o
  BR_BALLEFF_Main( p_wk->p_balleff[CLSYS_DRAW_MAIN] );
  BR_BALLEFF_Main( p_wk->p_balleff[CLSYS_DRAW_SUB] );

	//�I��
	if( BR_SEQ_IsEnd( p_wk->p_seq ) )
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
 *					Br_Start_Seq
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�J�n
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Start_Seq_Open( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_BALLEFF_UP_START,   //���ʃ{�[�����o�J�n
    SEQ_BALLEFF_UP_WAIT,    //���o�҂�
    SEQ_BALLEFF_DOWN_START, //����ʃ{�[�����o�J�n
    SEQ_BALLEFF_DOWN_WAIT,  //���o�҂�

    SEQ_LOGOFADEIN_INIT,
    SEQ_LOGOFADEIN_WAIT,
    SEQ_LOGOFADEOUT_INIT,
    SEQ_LOGOFADEOUT_WAIT,
    SEQ_TOUCH,
    SEQ_FADESTART,
    SEQ_FADEWAIT,
    SEQ_END,
  };

  BR_START_WORK  *p_wk     = p_wk_adrs;
 
  switch( *p_seq )
  { 
  case SEQ_BALLEFF_UP_START: //���ʃ{�[�����o�J�n
    { 
      GFL_POINT pos;
      pos.x = 128;
      pos.y = 96;
      BR_BALLEFF_StartMove( p_wk->p_balleff[CLSYS_DRAW_MAIN], BR_BALLEFF_MOVE_LINE, &pos );
      PMSND_PlaySE( BR_SND_SE_BOOT );
    }
    *p_seq  = SEQ_BALLEFF_UP_WAIT;
    break;
  case SEQ_BALLEFF_UP_WAIT:  //���o�҂�
    if( BR_BALLEFF_IsMoveEnd( p_wk->p_balleff[CLSYS_DRAW_MAIN] ) )
    { 
      *p_seq  = SEQ_BALLEFF_DOWN_START;
    }
    break;
  case SEQ_BALLEFF_DOWN_START: //����ʃ{�[�����o�J�n
    { 
      GFL_POINT pos;
      pos.x = 128;
      pos.y = -8;
      BR_BALLEFF_StartMove( p_wk->p_balleff[CLSYS_DRAW_SUB], BR_BALLEFF_MOVE_OPENING, &pos );
    }
    *p_seq  = SEQ_BALLEFF_DOWN_WAIT;
    break;
  case SEQ_BALLEFF_DOWN_WAIT:  //���o�҂�
    if( BR_BALLEFF_IsMoveEnd( p_wk->p_balleff[CLSYS_DRAW_SUB] ) )
    { 
      *p_seq  = SEQ_LOGOFADEIN_INIT;
    }
    break;

  case SEQ_LOGOFADEIN_INIT:
    p_wk->p_here  = BR_MSGWIN_Init( BG_FRAME_S_FONT, 8, 16, 16, 2, PLT_BG_S_FONT, p_wk->p_que, p_wk->heapID );
    { 
      GFL_FONT *p_font;
      GFL_MSGDATA *p_msg; 

      p_font  = BR_RES_GetFont( p_wk->p_param->p_res );
      p_msg   = BR_RES_GetMsgData( p_wk->p_param->p_res );
      BR_MSGWIN_PrintColor( p_wk->p_here, p_msg, msg_10000, p_font, PRINTSYS_LSB_Make(1,2,0) );
    }
    BR_FADE_StartFadeEx( p_wk->p_param->p_fade, BR_FADE_TYPE_PALLETE, BR_FADE_DISPLAY_TOUCH_HERE, BR_FADE_DIR_OUT, 4 );
    (*p_seq)  = SEQ_LOGOFADEIN_WAIT;
    break;

  case SEQ_LOGOFADEIN_WAIT:
    if( BR_FADE_IsEnd(p_wk->p_param->p_fade) )
    {
      *p_seq  = SEQ_LOGOFADEOUT_INIT;
    }
    break;

  case SEQ_LOGOFADEOUT_INIT:
    BR_FADE_StartFadeEx( p_wk->p_param->p_fade, BR_FADE_TYPE_PALLETE, BR_FADE_DISPLAY_TOUCH_HERE, BR_FADE_DIR_IN, 4 );
    (*p_seq)  = SEQ_LOGOFADEOUT_WAIT;
    break;

  case SEQ_LOGOFADEOUT_WAIT:
    if( BR_FADE_IsEnd(p_wk->p_param->p_fade) )
    { 
      (*p_seq)  = SEQ_TOUCH;
    }
    break;

  case SEQ_TOUCH:
    if( GFL_UI_TP_GetTrg() )
    { 
      PMSND_PlaySE( BR_SND_SE_TOUCHHERE );
      BR_MSGWIN_Exit( p_wk->p_here );
      p_wk->p_here  = NULL;
      GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );

      (*p_seq)  = SEQ_FADESTART;
    }
    break;

  case SEQ_FADESTART:
    BR_FADE_StartFadeEx( p_wk->p_param->p_fade, BR_FADE_TYPE_PALLETE, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN, 48 );
    BR_SIDEBAR_StartBoot( p_wk->p_param->p_sidebar );
    (*p_seq)  = SEQ_FADEWAIT;
    break;

  case SEQ_FADEWAIT:
    { 
      BOOL is_end = TRUE;
      is_end  &= BR_FADE_IsEnd( p_wk->p_param->p_fade );
      //is_end  &= BR_SIDEBAR_IsMoveEnd( p_wk->p_param->p_sidebar );

      if( is_end )
      { 
        (*p_seq)  = SEQ_END;
      }
    }
    break;

  case SEQ_END:
    BR_PROC_SYS_Push( p_wk->p_param->p_procsys, BR_PROCID_MENU );
    BR_SEQ_SetNext( p_seqwk, Br_Start_Seq_End );
    break;
  }
  
  if( p_wk->p_here != NULL )
  { 
    BR_MSGWIN_PrintMain( p_wk->p_here );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I��
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Start_Seq_Close( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_SIDEBAR_START,
    SEQ_FADESTART,
    SEQ_FADEWAIT,
    SEQ_END,
  };

  BR_START_WORK  *p_wk     = p_wk_adrs;
 
  switch( *p_seq )
  { 
  case SEQ_INIT:
    (*p_seq)  = SEQ_SIDEBAR_START;
    break;

  case SEQ_SIDEBAR_START:
    BR_SIDEBAR_StartClose( p_wk->p_param->p_sidebar );
    (*p_seq)  = SEQ_FADESTART;
    break;

  case SEQ_FADESTART:
    if( p_wk->cnt++ > 0 )
    { 
      p_wk->cnt = 0;
      BR_FADE_StartFadeEx( p_wk->p_param->p_fade, BR_FADE_TYPE_PALLETE, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT, 26 );
      (*p_seq)  = SEQ_FADEWAIT;
    }
    break;

  case SEQ_FADEWAIT:
    { 
      BOOL is_end = TRUE;
      is_end  &= BR_FADE_IsEnd( p_wk->p_param->p_fade );
      //is_end  &= BR_SIDEBAR_IsMoveEnd( p_wk->p_param->p_sidebar );

      if( is_end )
      { 
        (*p_seq)  = SEQ_END;
      }
    }
    break;

  case SEQ_END:
    BR_PROC_SYS_Pop( p_wk->p_param->p_procsys );
    BR_SEQ_SetNext( p_seqwk, Br_Start_Seq_End );
    break;
  }
  
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���������ɏI��
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Start_Seq_None( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BR_START_WORK  *p_wk     = p_wk_adrs;

  BR_SIDEBAR_SetShakePos( p_wk->p_param->p_sidebar );
  BR_SIDEBAR_StartShake( p_wk->p_param->p_sidebar );

  BR_PROC_SYS_Push( p_wk->p_param->p_procsys, BR_PROCID_MENU );
  BR_SEQ_SetNext( p_seqwk, Br_Start_Seq_End );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I��
 *
 *	@param	BR_SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Start_Seq_End( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  BR_START_WORK  *p_wk     = p_wk_adrs;

  BR_SEQ_End( p_seqwk );
}

