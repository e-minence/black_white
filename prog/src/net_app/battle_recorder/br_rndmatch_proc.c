//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_rndmatch_proc.c
 *	@brief	�����_���}�b�`�v���Z�X
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
#include "br_btn.h"
#include "br_util.h"
#include "br_snd.h"

//�O���Q��
#include "br_rndmatch_proc.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�{�^���C���f�b�N�X
//=====================================
typedef enum
{
	BR_RNDMATCH_BTNID_RETURN,
	BR_RNDMATCH_BTNID_MAX
} BR_RNDMATCH_BTNID;
//-------------------------------------
///	MSGWIN
//=====================================
typedef enum
{
  BR_RNDMATCH_MSGWINID_S_RNDRATE,   //�����_���}�b�` ���[�e�B���O
  BR_RNDMATCH_MSGWINID_S_RATE,  //���[�e�B���O���[�h
  BR_RNDMATCH_MSGWINID_S_FREE,  //�t���[���[�h
  BR_RNDMATCH_MSGWINID_S_MAX,
} BR_RNDMATCH_MSGWINID_S;

typedef enum
{
  BR_RNDMATCH_MSGWINID_M_NONE_MSG = 0,    //���b�Z�[�W��
  BR_RNDMATCH_MSGWINID_M_NONE_MAX,


  BR_RNDMATCH_MSGWINID_M_RND_CAPTION = 0, //�t���[���[�h����
  BR_RNDMATCH_MSGWINID_M_RND_WIN,         //����
  BR_RNDMATCH_MSGWINID_M_RND_LOSE,        //�܂�
  BR_RNDMATCH_MSGWINID_M_RND_SINGLE,        //�V���O��
  BR_RNDMATCH_MSGWINID_M_RND_DOUBLE,        //�_�u��
  BR_RNDMATCH_MSGWINID_M_RND_TRIPLE,        //�g���v��
  BR_RNDMATCH_MSGWINID_M_RND_ROT,        //���[�e
  BR_RNDMATCH_MSGWINID_M_RND_SHOT,        //�V���[�^�[

  BR_RNDMATCH_MSGWINID_M_RND_SINGLE_W,      //�V���O�������_
  BR_RNDMATCH_MSGWINID_M_RND_DOUBLE_W,      //�_�u�������_
  BR_RNDMATCH_MSGWINID_M_RND_TRIPLE_W,      //�g���v�������_
  BR_RNDMATCH_MSGWINID_M_RND_ROT_W,      //���[�e�����_
  BR_RNDMATCH_MSGWINID_M_RND_SHOT_W,      //�V���[�^�[�����_

  BR_RNDMATCH_MSGWINID_M_RND_SINGLE_L,       //�V���O�������_
  BR_RNDMATCH_MSGWINID_M_RND_DOUBLE_L,       //�_�u�������_
  BR_RNDMATCH_MSGWINID_M_RND_TRIPLE_L,       //�g���v�������_
  BR_RNDMATCH_MSGWINID_M_RND_ROT_L,       //���[�e�����_
  BR_RNDMATCH_MSGWINID_M_RND_SHOT_L,       //�V���[�^�[�����_

  BR_RNDMATCH_MSGWINID_M_RND_MAX,


  BR_RNDMATCH_MSGWINID_M_RATE_CAPTION = 0, //�����_���}�b�`�@���[�e�B���O
  BR_RNDMATCH_MSGWINID_M_RATE_RATE,        //���[�e�B���O
  BR_RNDMATCH_MSGWINID_M_RATE_SINGLE,      //�V���O��
  BR_RNDMATCH_MSGWINID_M_RATE_DOUBLE,      //�_�u��
  BR_RNDMATCH_MSGWINID_M_RATE_TRIPLE,      //�g���v��
  BR_RNDMATCH_MSGWINID_M_RATE_ROT,         //���[�e
  BR_RNDMATCH_MSGWINID_M_RATE_SHOT,        //�V���[�^�[

  BR_RNDMATCH_MSGWINID_M_RATE_SINGLE_RATE, //�V���O�����[�e�B���O
  BR_RNDMATCH_MSGWINID_M_RATE_DOUBLE_RATE, //�_�u�����[�e�B���O
  BR_RNDMATCH_MSGWINID_M_RATE_TRIPLE_RATE, //�g���v�����[�e�B���O
  BR_RNDMATCH_MSGWINID_M_RATE_ROT_RATE,    //���[�e���[�e�B���O
  BR_RNDMATCH_MSGWINID_M_RATE_SHOT_RATE,   //�V���[�^�[���[�e�B���O
  BR_RNDMATCH_MSGWINID_M_RATE_MAX,



  BR_RNDMATCH_MSGWINID_M_MAX  = BR_RNDMATCH_MSGWINID_M_RND_MAX,
} BR_RNDMATCH_MSGWINID_M;


//-------------------------------------
///	�I��
//=====================================
typedef enum 
{
  BR_RNDMATCH_SELECT_NONE,
  BR_RNDMATCH_SELECT_RND,
  BR_RNDMATCH_SELECT_RATE,
  BR_RNDMATCH_SELECT_FREE,

  BR_RNDMATCH_SELECT_MAX
} BR_RNDMATCH_SELECT;

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�����_���}�b�`���C�����[�N
//=====================================
typedef struct 
{
  BMPOAM_SYS_PTR	  	p_bmpoam;	//BMPOAM�V�X�e��
  PRINT_QUE           *p_que;   //�v�����g�L���[

	BR_BTN_WORK	        *p_btn[ BR_RNDMATCH_BTNID_MAX ];
  BR_MSGWIN_WORK      *p_msgwin_s[ BR_RNDMATCH_MSGWINID_S_MAX ];
  BR_MSGWIN_WORK      *p_msgwin_m[ BR_RNDMATCH_MSGWINID_M_MAX ];
  BR_BALLEFF_WORK     *p_balleff;

  BR_RNDMATCH_SELECT select;
	HEAPID heapID;
} BR_RNDMATCH_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�����_���}�b�`�v���Z�X
//=====================================
static GFL_PROC_RESULT BR_RNDMATCH_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_RNDMATCH_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_RNDMATCH_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	�����_���}�b�`�v���Z�X
//=====================================
const GFL_PROC_DATA BR_RNDMATCH_ProcData =
{	
	BR_RNDMATCH_PROC_Init,
	BR_RNDMATCH_PROC_Main,
	BR_RNDMATCH_PROC_Exit,
};

//=============================================================================
/**
 *					PRIVATE
 */
//=============================================================================
//-------------------------------------
///	�e��ʍ\�z
//=====================================
static void Br_RndMatch_CreateMainDisplayNone( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param );
static void Br_RndMatch_CreateMainDisplayRnd( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param );
static void Br_RndMatch_CreateMainDisplayRate( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param );
static void Br_RndMatch_CreateMainDisplayFree( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param );
static void Br_RndMatch_DeleteMainDisplay( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param );
static void Br_RndMatch_CreateSubDisplay( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param );
static void Br_RndMatch_DeleteSubDisplay( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param );
//-------------------------------------
///	private
//=====================================
static BR_RNDMATCH_SELECT Br_RndMatch_GetSelect( BR_RNDMATCH_WORK *p_wk, u32 x, u32 y );

//=============================================================================
/**
 *					�����_���}�b�`�v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�����_���}�b�`�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_RNDMATCH_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_RNDMATCH_WORK				*p_wk;
	BR_RNDMATCH_PROC_PARAM	*p_param	= p_param_adrs;

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_RNDMATCH_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_RNDMATCH_WORK) );	
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );

  //���W���[���쐬
  p_wk->p_bmpoam		= BmpOam_Init( p_wk->heapID, p_param->p_unit);
  p_wk->p_que       = PRINTSYS_QUE_Create( p_wk->heapID );
  p_wk->p_balleff   = BR_BALLEFF_Init( p_param->p_unit, p_param->p_res, CLSYS_DRAW_SUB, p_wk->heapID );

	//�O���t�B�b�N������
  Br_RndMatch_CreateMainDisplayNone( p_wk, p_param );
  Br_RndMatch_CreateSubDisplay( p_wk, p_param );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����_���}�b�`�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_RNDMATCH_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_RNDMATCH_WORK				*p_wk	= p_wk_adrs;
	BR_RNDMATCH_PROC_PARAM	*p_param	= p_param_adrs;

  //�O���t�B�b�N�j��
  Br_RndMatch_DeleteMainDisplay( p_wk, p_param );
  Br_RndMatch_DeleteSubDisplay( p_wk, p_param );
  GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );
  GFL_BG_LoadScreenReq( BG_FRAME_M_FONT ); 

	//���W���[���j��
  BR_BALLEFF_Exit( p_wk->p_balleff );
  PRINTSYS_QUE_Delete( p_wk->p_que );
  BmpOam_Exit( p_wk->p_bmpoam );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�����_���}�b�`�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_RNDMATCH_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_MAIN,

    SEQ_CHANGEOUT_START,
    SEQ_CHANGEOUT_WAIT,
    SEQ_CHANGEIN_START,
    SEQ_CHANGEIN_WAIT,

    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_EXIT,
  };

	BR_RNDMATCH_WORK	*p_wk	= p_wk_adrs;
	BR_RNDMATCH_PROC_PARAM	*p_param	= p_param_adrs;

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
      u32 x, y;
      if( GFL_UI_TP_GetPointTrg( &x, &y ) )
      {
        //�v���t�B�[���؂�ւ��`�F�b�N
        p_wk->select  = Br_RndMatch_GetSelect( p_wk, x, y );
        if( p_wk->select != BR_RNDMATCH_SELECT_NONE )
        { 
          *p_seq  = SEQ_CHANGEOUT_START;
        }

        //�I���`�F�b�N
        if( BR_BTN_GetTrg( p_wk->p_btn[BR_RNDMATCH_BTNID_RETURN], x, y ) )
        {	
          GFL_POINT pos;
          pos.x = x;
          pos.y = y;
          BR_BALLEFF_StartMove( p_wk->p_balleff, BR_BALLEFF_MOVE_EMIT, &pos );
          *p_seq  = SEQ_FADEOUT_START;
        }	
      }
    }
    break;

  case SEQ_CHANGEOUT_START:
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_MAIN, BR_FADE_DIR_OUT );
    (*p_seq)++;
    break;
  case SEQ_CHANGEOUT_WAIT:
    if( BR_FADE_IsEnd( p_param->p_fade ) )
    { 
      (*p_seq)++;
    }
    break;
  case SEQ_CHANGEIN_START:
    { 
      //�ǂݍ��ݕς�
      Br_RndMatch_DeleteMainDisplay( p_wk, p_param );
      switch( p_wk->select )
      {
      case BR_RNDMATCH_SELECT_RND:
        Br_RndMatch_CreateMainDisplayRnd( p_wk, p_param );
        break;
      case BR_RNDMATCH_SELECT_RATE:
        Br_RndMatch_CreateMainDisplayRate( p_wk, p_param );
        break;
      case BR_RNDMATCH_SELECT_FREE:
        Br_RndMatch_CreateMainDisplayFree( p_wk, p_param );
        break;
      }
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_MAIN, BR_FADE_DIR_IN );
    }
    (*p_seq)++;
    break;
  case SEQ_CHANGEIN_WAIT:
    if( BR_FADE_IsEnd( p_param->p_fade ) )
    { 
      *p_seq  = SEQ_MAIN;
    }
    break;

  case SEQ_FADEOUT_START:
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_FADEOUT_WAIT;
    break;
  case SEQ_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_param->p_fade ) )
    { 
      *p_seq  = SEQ_EXIT;
    }
    break;
  case SEQ_EXIT:
    NAGI_Printf( "RNDMATCH: Exit!\n" );
    BR_PROC_SYS_Pop( p_param->p_procsys );
    return GFL_PROC_RES_FINISH;
  }

   //�����\��
  { 
    int i;
    for( i = 0; i < BR_RNDMATCH_MSGWINID_S_MAX; i++ )
    { 
      if( p_wk->p_msgwin_s[i] )
      { 
        BR_MSGWIN_PrintMain( p_wk->p_msgwin_s[i] );
      }
    }
    for( i = 0; i < BR_RNDMATCH_MSGWINID_M_MAX; i++ )
    { 
      if( p_wk->p_msgwin_m[i] )
      { 
        BR_MSGWIN_PrintMain( p_wk->p_msgwin_m[i] );
      }
    }
  }

  PRINTSYS_QUE_Main( p_wk->p_que );

  BR_BALLEFF_Main( p_wk->p_balleff );

	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *    �e��ʍ\�z�E�j��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���ʉ����Ȃ���ԍ\�z
 *
 *	@param	BR_RNDMATCH_WORK *p_wk  ���[�N
 *	@param	*p_param                ����
 *
 */
//-----------------------------------------------------------------------------
static void Br_RndMatch_CreateMainDisplayNone( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param )
{ 
  static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_RNDMATCH_MSGWINID_M_NONE_MAX]  =
  { 
    { 
      1,
      19,
      30,
      4,
      msg_805,
    }
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );


  //���\�[�X�ǂݍ���
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_RNDMATCH_M_NONE, p_wk->heapID );

  //���b�Z�[�WWIN�쐬
  {
    int i;
    for( i = 0; i < BR_RNDMATCH_MSGWINID_M_NONE_MAX; i++ )
    { 
      p_wk->p_msgwin_m[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );
      BR_MSGWIN_PrintColor( p_wk->p_msgwin_m[i], p_msg, sc_msgwin_data[i].msgID, p_font, BR_PRINT_COL_NORMAL );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`���[�e�B���O  ����
 *
 *	@param	BR_RNDMATCH_WORK *p_wk  ���[�N
 *	@param	*p_param                ����
 */
//-----------------------------------------------------------------------------
static void Br_RndMatch_CreateMainDisplayRnd( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param )
{ 
  static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_RNDMATCH_MSGWINID_M_RATE_MAX]  =
  { 
    { 
      4,
      3,
      24,
      2,
      msg_900,
    },
    { 
      14,
      6,
      14,
      2,
      msg_912,
    },

    { 
      4,
      9,
      8,
      2,
      msg_905,
    },
    { 
      4,
      12,
      8,
      2,
      msg_906,
    },
    { 
      4,
      15,
      8,
      2,
      msg_907,
    },
    { 
      4,
      18,
      8,
      2,
      msg_908,
    },
    { 
      4,
      21,
      8,
      2,
      msg_909,
    },

    { 
      23,
      9,
      8,
      2,
      msg_910,
    },
    { 
      23,
      12,
      8,
      2,
      msg_910,
    },
    { 
      23,
      15,
      8,
      2,
      msg_910,
    },
    { 
      23,
      18,
      8,
      2,
      msg_910,
    },
    { 
      23,
      21,
      8,
      2,
      msg_910,
    },
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 
  WORDSET *p_word;
  STRBUF  *p_strbuf;
  STRBUF  *p_src;

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );
  p_word  = BR_RES_GetWordSet( p_param->p_res );


  //���\�[�X�ǂݍ���
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_RNDMATCH_M_RND, p_wk->heapID );

  //���b�Z�[�WWIN�쐬
  {
    int i;
    int number;
    BOOL is_expand;
    for( i = 0; i < BR_RNDMATCH_MSGWINID_M_RATE_MAX; i++ )
    { 
      is_expand = TRUE;

      p_wk->p_msgwin_m[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );

      switch( i )
      { 
      case BR_RNDMATCH_MSGWINID_M_RATE_SINGLE_RATE:  //�V���O��
         number  = RNDMATCH_GetParam( p_param->p_rndmatch, RNDMATCH_TYPE_RATE_SINGLE, RNDMATCH_PARAM_IDX_RATE ); 
        break;

      case BR_RNDMATCH_MSGWINID_M_RATE_DOUBLE_RATE:  //�_�u��
         number  = RNDMATCH_GetParam( p_param->p_rndmatch, RNDMATCH_TYPE_RATE_DOUBLE, RNDMATCH_PARAM_IDX_RATE ); 
        break;

      case BR_RNDMATCH_MSGWINID_M_RATE_TRIPLE_RATE:  //�g���v��
         number  = RNDMATCH_GetParam( p_param->p_rndmatch, RNDMATCH_TYPE_RATE_TRIPLE, RNDMATCH_PARAM_IDX_RATE ); 
        break;

      case BR_RNDMATCH_MSGWINID_M_RATE_ROT_RATE:  //���[�e
         number  = RNDMATCH_GetParam( p_param->p_rndmatch, RNDMATCH_TYPE_RATE_ROTATE, RNDMATCH_PARAM_IDX_RATE ); 
        break;

      case BR_RNDMATCH_MSGWINID_M_RATE_SHOT_RATE:  //�V���[�^
         number  = RNDMATCH_GetParam( p_param->p_rndmatch, RNDMATCH_TYPE_RATE_SHOOTER, RNDMATCH_PARAM_IDX_RATE ); 
        break;

      default:
        p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
        is_expand = FALSE;
      }

      if( is_expand )
      { 
        p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
        p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
        WORDSET_RegisterNumber( p_word, 0, number, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
        WORDSET_ExpandStr( p_word, p_strbuf, p_src );
        GFL_STR_DeleteBuffer( p_src );
      }


      BR_MSGWIN_PrintBufColor( p_wk->p_msgwin_m[i], p_strbuf, p_font, BR_PRINT_COL_NORMAL );

      GFL_STR_DeleteBuffer( p_strbuf ); 
    }
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  ���[�e�B���O���[�h�̐���  ���ʍ\�z
 *
 *	@param	BR_RNDMATCH_WORK *p_wk  ���[�N
 *	@param	*p_param                ����
 */
//-----------------------------------------------------------------------------
static void Br_RndMatch_CreateMainDisplayRate( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param )
{ 
  static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_RNDMATCH_MSGWINID_M_RND_MAX]  =
  { 
    { 
      4,
      3,
      24,
      2,
      msg_901,
    },
    { 
      15,
      6,
      5,
      2,
      msg_903,
    },
    { 
      23,
      6,
      5,
      2,
      msg_904,
    },

    { 
      4,
      9,
      8,
      2,
      msg_905,
    },
    { 
      4,
      12,
      8,
      2,
      msg_906,
    },
    { 
      4,
      15,
      8,
      2,
      msg_907,
    },
    { 
      4,
      18,
      8,
      2,
      msg_908,
    },
    { 
      4,
      21,
      8,
      2,
      msg_909,
    },
    //����
    { 
      15,
      9,
      8,
      2,
      msg_911,
    },
    { 
      15,
      12,
      8,
      2,
      msg_911,
    },
    { 
      15,
      15,
      8,
      2,
      msg_911,
    },
    { 
      15,
      18,
      8,
      2,
      msg_911,
    },
    { 
      15,
      21,
      8,
      2,
      msg_911,
    },

  
    //����
    { 
      23,
      9,
      8,
      2,
      msg_911,
    },
    { 
      23,
      12,
      8,
      2,
      msg_911,
    },
    { 
      23,
      15,
      8,
      2,
      msg_911,
    },
    { 
      23,
      18,
      8,
      2,
      msg_911,
    },
    { 
      23,
      21,
      8,
      2,
      msg_911,
    },
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 
  WORDSET *p_word;
  STRBUF  *p_strbuf;
  STRBUF  *p_src;

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );
  p_word  = BR_RES_GetWordSet( p_param->p_res );


  //���\�[�X�ǂݍ���
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_RNDMATCH_M_RATE, p_wk->heapID );

  //���b�Z�[�WWIN�쐬
  {
    int i;
    int number;
    BOOL is_expand;
    for( i = 0; i < BR_RNDMATCH_MSGWINID_M_RND_MAX; i++ )
    { 
      p_wk->p_msgwin_m[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );

      if( BR_RNDMATCH_MSGWINID_M_RND_SINGLE_W <= i && i <= BR_RNDMATCH_MSGWINID_M_RND_SHOT_W )
      { 
        //�����_
        number  = RNDMATCH_GetParam( p_param->p_rndmatch, RNDMATCH_TYPE_RATE_SINGLE+i-BR_RNDMATCH_MSGWINID_M_RND_SINGLE_W,
            RNDMATCH_PARAM_IDX_WIN ); 
        is_expand = TRUE;
      }
      else if( BR_RNDMATCH_MSGWINID_M_RND_SINGLE_L <= i && i <= BR_RNDMATCH_MSGWINID_M_RND_SHOT_L )
      { 
        //�����_
        number  = RNDMATCH_GetParam( p_param->p_rndmatch, RNDMATCH_TYPE_RATE_SINGLE+i-BR_RNDMATCH_MSGWINID_M_RND_SINGLE_L,
            RNDMATCH_PARAM_IDX_LOSE ); 

        is_expand = TRUE;
      }
      else
      { 
        p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
        is_expand = FALSE;
      }

      if( is_expand )
      { 
        p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
        p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
        WORDSET_RegisterNumber( p_word, 0, number, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
        WORDSET_ExpandStr( p_word, p_strbuf, p_src );
        GFL_STR_DeleteBuffer( p_src );
      }


      BR_MSGWIN_PrintBufColor( p_wk->p_msgwin_m[i], p_strbuf, p_font, BR_PRINT_COL_NORMAL );

      GFL_STR_DeleteBuffer( p_strbuf ); 
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t���[���[�h�̐���
 *
 *	@param	BR_RNDMATCH_WORK *p_wk  ���[�N
 *	@param	*p_param                ����
 */
//-----------------------------------------------------------------------------
static void Br_RndMatch_CreateMainDisplayFree( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param )
{ 
  static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_RNDMATCH_MSGWINID_M_RND_MAX]  =
  { 
    { 
      4,
      3,
      24,
      2,
      msg_902,
    },
    { 
      15,
      6,
      5,
      2,
      msg_903,
    },
    { 
      23,
      6,
      5,
      2,
      msg_904,
    },

    { 
      4,
      9,
      8,
      2,
      msg_905,
    },
    { 
      4,
      12,
      8,
      2,
      msg_906,
    },
    { 
      4,
      15,
      8,
      2,
      msg_907,
    },
    { 
      4,
      18,
      8,
      2,
      msg_908,
    },
    { 
      4,
      21,
      8,
      2,
      msg_909,
    },
    //����
    { 
      15,
      9,
      8,
      2,
      msg_911,
    },
    { 
      15,
      12,
      8,
      2,
      msg_911,
    },
    { 
      15,
      15,
      8,
      2,
      msg_911,
    },
    { 
      15,
      18,
      8,
      2,
      msg_911,
    },
    { 
      15,
      21,
      8,
      2,
      msg_911,
    },

  
    //����
    { 
      23,
      9,
      8,
      2,
      msg_911,
    },
    { 
      23,
      12,
      8,
      2,
      msg_911,
    },
    { 
      23,
      15,
      8,
      2,
      msg_911,
    },
    { 
      23,
      18,
      8,
      2,
      msg_911,
    },
    { 
      23,
      21,
      8,
      2,
      msg_911,
    },
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 
  WORDSET *p_word;
  STRBUF  *p_strbuf;
  STRBUF  *p_src;

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );
  p_word  = BR_RES_GetWordSet( p_param->p_res );


  //���\�[�X�ǂݍ���
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_RNDMATCH_M_RATE, p_wk->heapID );


  //���b�Z�[�WWIN�쐬
  {
    int i;
    int number;
    BOOL is_expand;
    for( i = 0; i < BR_RNDMATCH_MSGWINID_M_RND_MAX; i++ )
    { 
      p_wk->p_msgwin_m[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );

      if( BR_RNDMATCH_MSGWINID_M_RND_SINGLE_W <= i && i <= BR_RNDMATCH_MSGWINID_M_RND_SHOT_W )
      { 
        //�����_
        number  = RNDMATCH_GetParam( p_param->p_rndmatch, i-BR_RNDMATCH_MSGWINID_M_RND_SINGLE_W,
            RNDMATCH_PARAM_IDX_WIN ); 
        is_expand = TRUE;
      }
      else if( BR_RNDMATCH_MSGWINID_M_RND_SINGLE_L <= i && i <= BR_RNDMATCH_MSGWINID_M_RND_SHOT_L )
      { 
        //�����_
        number  = RNDMATCH_GetParam( p_param->p_rndmatch, i-BR_RNDMATCH_MSGWINID_M_RND_SINGLE_L,
            RNDMATCH_PARAM_IDX_LOSE ); 

        is_expand = TRUE;
      }
      else
      { 
        p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
        is_expand = FALSE;
      }

      if( is_expand )
      { 
        p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
        p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
        WORDSET_RegisterNumber( p_word, 0, number, 4, STR_NUM_DISP_SPACE, STR_NUM_CODE_DEFAULT );
        WORDSET_ExpandStr( p_word, p_strbuf, p_src );
        GFL_STR_DeleteBuffer( p_src );
      }


      BR_MSGWIN_PrintBufColor( p_wk->p_msgwin_m[i], p_strbuf, p_font, BR_PRINT_COL_NORMAL );

      GFL_STR_DeleteBuffer( p_strbuf ); 
    }
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  ���ʔj��
 *
 *	@param	BR_RNDMATCH_WORK *p_wk  ���[�N
 *	@param	*p_param                ����
 */
//-----------------------------------------------------------------------------
static void Br_RndMatch_DeleteMainDisplay( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param )
{ 
  { 
    int i;
    for( i = 0; i < BR_RNDMATCH_MSGWINID_M_MAX; i++ )
    { 
      if( p_wk->p_msgwin_m[i] )
      { 
        BR_MSGWIN_Exit( p_wk->p_msgwin_m[i] );
        p_wk->p_msgwin_m[i] = NULL;
      }
    }
  }
  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_RNDMATCH_M_RATE );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ����ʍ\�z
 *
 *	@param	BR_RNDMATCH_WORK *p_wk  ���[�N
 *	@param	*p_param                ����
 */
//-----------------------------------------------------------------------------
static void Br_RndMatch_CreateSubDisplay( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param )
{ 
  static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_RNDMATCH_MSGWINID_S_MAX]  =
  { 
    {
      4,
      3,
      24,
      2,
      msg_900
    },
    {
      4,
      8,
      24,
      2,
      msg_901
    },
    {
      4,
      13,
      24,
      2,
      msg_902
    },
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 


  //���\�[�X�ǂݍ���
  BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, p_wk->heapID ); 
	BR_RES_LoadBG( p_param->p_res, BR_RES_BG_RNDMATCH_S, p_wk->heapID );

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );


  //�{�^���쐬
  { 
    GFL_CLWK_DATA cldata;
    BR_RES_OBJ_DATA res;

    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    cldata.pos_x    = 80;
    cldata.pos_y    = 168;
    cldata.anmseq   = 0;
    cldata.softpri  = 1;

    BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, &res );

    p_wk->p_btn[BR_RNDMATCH_BTNID_RETURN] = BR_BTN_Init( &cldata, msg_05, BR_BTN_DATA_SHORT_WIDTH, CLSYS_DRAW_SUB, p_param->p_unit, p_wk->p_bmpoam, p_font, p_msg, &res, p_wk->heapID );
;
  }

  //���b�Z�[�WWIN�쐬
  {
    int i;
    for( i = 0; i < BR_RNDMATCH_MSGWINID_S_MAX; i++ )
    { 
      p_wk->p_msgwin_s[i]  = BR_MSGWIN_Init( BG_FRAME_S_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_S_FONT, p_wk->p_que, p_wk->heapID );
      BR_MSGWIN_PrintColor( p_wk->p_msgwin_s[i], p_msg, sc_msgwin_data[i].msgID, p_font, BR_PRINT_COL_NORMAL );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ����ʔj��
 *
 *	@param	BR_RNDMATCH_WORK *p_wk  ���[�N
 *	@param	*p_param                ����
 */
//-----------------------------------------------------------------------------
static void Br_RndMatch_DeleteSubDisplay( BR_RNDMATCH_WORK *p_wk, BR_RNDMATCH_PROC_PARAM *p_param )
{ 
  //���b�Z�[�W��j��
  {
    int i;
    for( i = 0; i < BR_RNDMATCH_MSGWINID_S_MAX; i++ )
    {
      if( p_wk->p_msgwin_s[i] )
      { 
        BR_MSGWIN_Exit( p_wk->p_msgwin_s[i] );
        p_wk->p_msgwin_s[i] = NULL;
      }
    }
  }

  //�{�^���j��
  { 
    int i;
    for( i = 0; i < BR_RNDMATCH_BTNID_MAX; i++ )
    { 
      if( p_wk->p_btn[i] )
      { 
        BR_BTN_Exit( p_wk->p_btn[i] );
        p_wk->p_btn[i]  = NULL;
      }
    }
  }

  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S ); 
	BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_RNDMATCH_S );
}
//=============================================================================
/**
 *    �I��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ������ʑI��
 *
 *	@param	BR_RNDMATCH_WORK *p_wk  ���[�N
 *	@param	x �^�b�`���WX
 *	@param	y �^�b�`���WY
 *
 *	@return �I��
 */
//-----------------------------------------------------------------------------
static BR_RNDMATCH_SELECT Br_RndMatch_GetSelect( BR_RNDMATCH_WORK *p_wk, u32 x, u32 y )
{ 
  static const GFL_RECT sc_select_rect[BR_RNDMATCH_SELECT_MAX] =
  { 
    { 
      0,0,0,0
    },
    { 
      7*8,2*8,24*8,5*8,
    },
    { 
      7*8,7*8,24*8,10*8,
    },
    { 
      7*8,12*8,24*8,15*8,
    },
  };

  int i;

  for( i = 0; i < BR_RNDMATCH_SELECT_MAX; i++ )
  { 
    if( i != p_wk->select )
    { 
      if( ((u32)( x - sc_select_rect[i].left) <= (u32)(sc_select_rect[i].right - sc_select_rect[i].left))
          & ((u32)( y - sc_select_rect[i].top) <= (u32)(sc_select_rect[i].bottom - sc_select_rect[i].top)))
      { 
        GFL_POINT pos;
        pos.x = x;
        pos.y = y;
        BR_BALLEFF_StartMove( p_wk->p_balleff, BR_BALLEFF_MOVE_EMIT, &pos );
        PMSND_PlaySE( BR_SND_SE_OK );
        return i;
      }
    }
  }

  return BR_RNDMATCH_SELECT_NONE;
}
