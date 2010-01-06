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
///	�V�[�P���X�Ǘ�
//=====================================
typedef struct _SEQ_WORK SEQ_WORK;	//�֐��^��邽�ߑO���錾
typedef void (*SEQ_FUNCTION)( SEQ_WORK *p_wk, int *p_seq, void *p_wk_adrs );
struct _SEQ_WORK
{
	SEQ_FUNCTION	seq_function;		//���s���̃V�[�P���X�֐�
	BOOL is_end;									//�V�[�P���X�V�X�e���I���t���O
	int seq;											//���s���̃V�[�P���X�֐��̒��̃V�[�P���X
	void *p_wk_adrs;							//���s���̃V�[�P���X�֐��ɓn�����[�N
};

//-------------------------------------
///	�N�����C�����[�N
//=====================================
typedef struct 
{
  //�V�[�P���X�Ǘ�
  SEQ_WORK        seq;

  //�q�[�vID
	HEAPID          heapID;

  //BMPWIN
  BR_MSGWIN_WORK  *p_here;

  //�v�����g�L���[
  PRINT_QUE       *p_que;

  //�ėp�J�E���^
  u32             cnt;


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
static void SEQ_Init( SEQ_WORK *p_wk, void *p_wk_adrs, SEQ_FUNCTION seq_function );
static void SEQ_Exit( SEQ_WORK *p_wk );
static void SEQ_Main( SEQ_WORK *p_wk );
static BOOL SEQ_IsEnd( const SEQ_WORK *cp_wk );
static void SEQ_SetNext( SEQ_WORK *p_wk, SEQ_FUNCTION seq_function );
static void SEQ_End( SEQ_WORK *p_wk );

//-------------------------------------
///	SEQ
//=====================================
static void SEQFUNC_Open( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_Close( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_None( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_End( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

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

	//�O���t�B�b�N������

  //���W���[���쐬
  p_wk->p_que       = PRINTSYS_QUE_Create( p_wk->heapID );
  { 
    SEQ_FUNCTION  seq_function;
    if( p_param->mode == BR_START_PROC_MODE_OPEN )
    { 
      seq_function  = SEQFUNC_Open;
    }
    else if( p_param->mode == BR_START_PROC_MODE_CLOSE )
    { 
      seq_function  = SEQFUNC_Close;
    }
    else
    { 
      seq_function  = SEQFUNC_None;
    }
    SEQ_Init( &p_wk->seq, p_wk, seq_function );
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
  PRINTSYS_QUE_Delete( p_wk->p_que );
  SEQ_Exit( &p_wk->seq );

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
	SEQ_Main( &p_wk->seq );

  //�����`��
  PRINTSYS_QUE_Main( p_wk->p_que );

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
 *	@param	*p_param				�p�����[�^
 *	@param	seq_function		�V�[�P���X
 *
 */
//-----------------------------------------------------------------------------
static void SEQ_Init( SEQ_WORK *p_wk, void *p_wk_adrs, SEQ_FUNCTION seq_function )
{	
	//�N���A
	GFL_STD_MemClear( p_wk, sizeof(SEQ_WORK) );

	//������
	p_wk->p_wk_adrs	= p_wk_adrs;

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
		p_wk->seq_function( p_wk, &p_wk->seq, p_wk->p_wk_adrs );
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
 *	@brief	�J�n
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Open( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_FADE_IN,
    SEQ_FADE_WAIT,
    SEQ_TOUCH,
    SEQ_FADESTART,
    SEQ_FADEWAIT,
    SEQ_END,
  };

  BR_START_WORK  *p_wk     = p_wk_adrs;
 
  switch( *p_seq )
  { 
  case SEQ_INIT:
    p_wk->p_here  = BR_MSGWIN_Init( BG_FRAME_S_FONT, 8, 16, 16, 2, PLT_BG_S_FONT, p_wk->p_que, p_wk->heapID );
    { 
      GFL_FONT *p_font;
      GFL_MSGDATA *p_msg; 

      p_font  = BR_RES_GetFont( p_wk->p_param->p_res );
      p_msg   = BR_RES_GetMsgData( p_wk->p_param->p_res );
      BR_MSGWIN_PrintColor( p_wk->p_here, p_msg, msg_10000, p_font, PRINTSYS_LSB_Make(1,2,0) );
    }
    BR_FADE_StartFadeEx( p_wk->p_param->p_fade, BR_FADE_TYPE_PALLETE, BR_FADE_DISPLAY_TOUCH_HERE, BR_FADE_DIR_OUT, 4 );
    (*p_seq)  = SEQ_FADE_IN;
    break;

  case SEQ_FADE_IN:
    if( BR_FADE_IsEnd(p_wk->p_param->p_fade) )
    { 
      BR_FADE_StartFadeEx( p_wk->p_param->p_fade, BR_FADE_TYPE_PALLETE, BR_FADE_DISPLAY_TOUCH_HERE, BR_FADE_DIR_IN, 4 );
      (*p_seq)  = SEQ_FADE_WAIT;
    }
    break;

  case SEQ_FADE_WAIT:
    if( BR_FADE_IsEnd(p_wk->p_param->p_fade) )
    { 
      (*p_seq)  = SEQ_TOUCH;
    }
    break;

  case SEQ_TOUCH:
    if( GFL_UI_TP_GetTrg() )
    { 
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
    SEQ_SetNext( p_seqwk, SEQFUNC_End );
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
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Close( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
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
    SEQ_SetNext( p_seqwk, SEQFUNC_End );
    break;
  }
  
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���������ɏI��
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_None( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BR_START_WORK  *p_wk     = p_wk_adrs;

  BR_SIDEBAR_SetShakePos( p_wk->p_param->p_sidebar );
  BR_SIDEBAR_StartShake( p_wk->p_param->p_sidebar );

  BR_PROC_SYS_Push( p_wk->p_param->p_procsys, BR_PROCID_MENU );
  SEQ_SetNext( p_seqwk, SEQFUNC_End );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�I��
 *
 *	@param	SEQ_WORK *p_seqwk	�V�[�P���X���[�N
 *	@param	*p_seq					�V�[�P���X
 *	@param	*p_wk_adrs				���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_End( SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{
  BR_START_WORK  *p_wk     = p_wk_adrs;

  SEQ_End( p_seqwk );
}

