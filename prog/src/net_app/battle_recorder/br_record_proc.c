//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_record_proc.c
 *	@brief	���R�[�h�v���Z�X
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

//�O���Q��
#include "br_record_proc.h"

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
	BR_RECORD_BTNID_RETURN,
	BR_RECORD_BTNID_SAVE,
	BR_RECORD_BTNID_MAX
} BR_RECORD_BTNID;

//-------------------------------------
///	MSGWIN
//=====================================
typedef enum
{
  BR_RECORD_MSGWINID_S_PROFILE,  //�v���t�B�[�����݂�^���܂�
  BR_RECORD_MSGWINID_S_MAX,
} BR_RECORD_MSGWINID_S;
typedef enum
{
  BR_RECORD_MSGWINID_M_BTL_NAME = 0,  //�������̋L�^
  BR_RECORD_MSGWINID_M_BTL_RULE,       //�R���V�A���@�V���O���@��������Ȃ�
  BR_RECORD_MSGWINID_M_BTL_NUMBER,    //�r�f�I�i���o�[
  BR_RECORD_MSGWINID_M_BTL_MAX,   

  BR_RECORD_MSGWINID_M_MAX  = BR_RECORD_MSGWINID_M_BTL_MAX,
} BR_RECORD_MSGWINID_M;


//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���R�[�h���C�����[�N
//=====================================
typedef struct 
{
  BMPOAM_SYS_PTR		p_bmpoam;	//BMPOAM�V�X�e��
  PRINT_QUE         *p_que;   //�v�����g�L���[

	BR_BTN_WORK	      *p_btn[ BR_RECORD_BTNID_MAX ];
  BR_MSGWIN_WORK    *p_msgwin_s[ BR_RECORD_MSGWINID_S_MAX ];
  BR_MSGWIN_WORK    *p_msgwin_m[ BR_RECORD_MSGWINID_M_MAX ];

  BR_PROFILE_WORK   *p_profile_disp;

  BOOL              is_profile;
	HEAPID            heapID;
} BR_RECORD_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	���R�[�h�v���Z�X
//=====================================
static GFL_PROC_RESULT BR_RECORD_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_RECORD_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_RECORD_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	���R�[�h�v���Z�X
//=====================================
const GFL_PROC_DATA BR_RECORD_ProcData =
{	
	BR_RECORD_PROC_Init,
	BR_RECORD_PROC_Main,
	BR_RECORD_PROC_Exit,
};

//=============================================================================
/**
 *					�v���g�^�C�v
 */
//=============================================================================
//��ʍ쐬
static void Br_Record_CreateMainDisplaySingle( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );
static void Br_Record_CreateMainDisplayDouble( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );
static void Br_Record_CreateMainDisplayProfile( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param, GDS_PROFILE_PTR p_profile );
static void Br_Record_DeleteMainDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );

static void Br_Record_CreateSubDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );
static void Br_Record_DeleteSubDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );

//���̑�
static BOOL Br_Record_GetTrgProfile( BR_RECORD_WORK * p_wk, u32 x, u32 y );

//=============================================================================
/**
 *					���R�[�h�v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	���R�[�h�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_RECORD_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_RECORD_WORK				*p_wk;
	BR_RECORD_PROC_PARAM	*p_param	= p_param_adrs;

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_RECORD_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_RECORD_WORK) );	
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );
  p_wk->is_profile  = FALSE;

  //���W���[���쐬
  p_wk->p_bmpoam		= BmpOam_Init( p_wk->heapID, p_param->p_unit);
  p_wk->p_que       = PRINTSYS_QUE_Create( p_wk->heapID );

	//�O���t�B�b�N������
  Br_Record_CreateSubDisplay( p_wk, p_param );
  Br_Record_CreateMainDisplaySingle( p_wk, p_param );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���R�[�h�v���Z�X	�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_RECORD_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	

	BR_RECORD_WORK				*p_wk	= p_wk_adrs;
	BR_RECORD_PROC_PARAM	*p_param	= p_param_adrs;


  //��ʍ\�z�j��
  Br_Record_DeleteSubDisplay( p_wk, p_param );
  Br_Record_DeleteMainDisplay( p_wk, p_param );
  GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );
  GFL_BG_LoadScreenReq( BG_FRAME_M_FONT );

	//���W���[���j��
  PRINTSYS_QUE_Delete( p_wk->p_que );
  BmpOam_Exit( p_wk->p_bmpoam );

	//�O���t�B�b�N�j��


	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	���R�[�h�v���Z�X	���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_RECORD_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
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

	BR_RECORD_WORK	*p_wk	= p_wk_adrs;
	BR_RECORD_PROC_PARAM	*p_param	= p_param_adrs;


  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    *p_seq  = SEQ_FADEIN_WAIT;
    break;
  case SEQ_FADEIN_WAIT:
    *p_seq  = SEQ_MAIN;
    break;
  case SEQ_MAIN:
    {
      u32 x, y;
      if( GFL_UI_TP_GetPointTrg( &x, &y ) )
      {
        //�v���t�B�[���؂�ւ��`�F�b�N
        if( Br_Record_GetTrgProfile( p_wk, x, y ) )
        { 
          p_wk->is_profile ^= 1;
          *p_seq  = SEQ_CHANGEOUT_START;
        }

        //�I���`�F�b�N
        if( BR_BTN_GetTrg( p_wk->p_btn[BR_RECORD_BTNID_RETURN], x, y ) )
        {	
          *p_seq  = SEQ_FADEOUT_START;
        }	
      }
    }
    break;

  case SEQ_CHANGEOUT_START:
    (*p_seq)++;
    break;
  case SEQ_CHANGEOUT_WAIT:
    (*p_seq)++;
    break;
  case SEQ_CHANGEIN_START:
    { 
      GFL_FONT *p_font;
      GFL_MSGDATA *p_msg;

      p_font  = BR_RES_GetFont( p_param->p_res );
      p_msg   = BR_RES_GetMsgData( p_param->p_res );
      //�ǂݍ��ݕς�
      Br_Record_DeleteMainDisplay( p_wk, p_param );
      if( p_wk->is_profile )
      { 
        BR_MSGWIN_PrintColor( p_wk->p_msgwin_s[BR_RECORD_MSGWINID_S_PROFILE], p_msg, msg_719, p_font, BR_PRINT_COL_NORMAL );
        Br_Record_CreateMainDisplayProfile( p_wk, p_param, NULL );
      }
      else
      { 
        BR_MSGWIN_PrintColor( p_wk->p_msgwin_s[BR_RECORD_MSGWINID_S_PROFILE], p_msg, msg_718, p_font, BR_PRINT_COL_NORMAL );
        Br_Record_CreateMainDisplaySingle( p_wk, p_param );
      }
    }
    (*p_seq)++;
    break;
  case SEQ_CHANGEIN_WAIT:
    *p_seq  = SEQ_MAIN;
    break;

  case SEQ_FADEOUT_START:
    *p_seq  = SEQ_FADEOUT_WAIT;
    break;
  case SEQ_FADEOUT_WAIT:
    *p_seq  = SEQ_EXIT;
    break;
  case SEQ_EXIT:
    NAGI_Printf( "RECORD: Exit!\n" );
    BR_PROC_SYS_Pop( p_param->p_procsys );
    return GFL_PROC_RES_FINISH;
  }

  //�����\��
  { 
    int i;
    for( i = 0; i < BR_RECORD_MSGWINID_S_MAX; i++ )
    { 
      if( p_wk->p_msgwin_s[i] )
      { 
        BR_MSGWIN_PrintMain( p_wk->p_msgwin_s[i] );
      }
    }
    for( i = 0; i < BR_RECORD_MSGWINID_M_MAX; i++ )
    { 
      if( p_wk->p_msgwin_m[i] )
      { 
        BR_MSGWIN_PrintMain( p_wk->p_msgwin_m[i] );
      }
    }
  }

  if( p_wk->p_profile_disp )
  { 
    BR_PROFILE_PrintMain( p_wk->p_profile_disp );
  }

  PRINTSYS_QUE_Main( p_wk->p_que );


	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *			�v���C�x�[�g
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�V���O����p�^��w�b�_��ʍ\�z
 *
 *	@param	BR_RECORD_WORK * p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Record_CreateMainDisplaySingle( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param )
{	
  static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_RECORD_MSGWINID_M_BTL_MAX]  =
  { 
    {
      4,
      4,
      24,
      2,
      msg_09,
    },
    {
      4,
      6,
      24,
      4,
      msg_rule_000,
    },
    {
      4,
      21,
      24,
      2,
      msg_12,
    },
  };

	BR_RES_LoadBG( p_param->p_res, BR_RES_BG_RECORD_M_BTL_SINGLE, p_wk->heapID );
  
  
  //���b�Z�[�WWIN�쐬
  {
    int i;
    GFL_FONT *p_font;
    GFL_MSGDATA *p_msg;

    p_font  = BR_RES_GetFont( p_param->p_res );
    p_msg   = BR_RES_GetMsgData( p_param->p_res );

    for( i = 0; i < BR_RECORD_MSGWINID_M_BTL_MAX; i++ )
    { 
      p_wk->p_msgwin_m[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );
      BR_MSGWIN_PrintColor( p_wk->p_msgwin_m[i], p_msg, sc_msgwin_data[i].msgID, p_font, BR_PRINT_COL_NORMAL );
    }
  }


  //@todo�|�P�A�C�R��
  { 

  }

}
//----------------------------------------------------------------------------
/**
 *	@brief	�_�u����p�^��w�b�_��ʍ\�z
 *
 *	@param	BR_RECORD_WORK * p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Record_CreateMainDisplayDouble( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param )
{	
	BR_RES_LoadBG( p_param->p_res, BR_RES_BG_RECORD_M_BTL_DOUBLE, p_wk->heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���t�B�[����ʍ\�z
 *
 *	@param	BR_RECORD_WORK * p_wk ���[�N
 *	@param	p_param								���R�[�h�̈���
 *	@param	p_profile							profile�̃|�C���^
 */
//-----------------------------------------------------------------------------
static void Br_Record_CreateMainDisplayProfile( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param,  GDS_PROFILE_PTR p_profile )
{
  p_wk->p_profile_disp  = BR_PROFILE_CreateMainDisplay( p_profile, p_param->p_res, p_param->p_unit, p_wk->p_que, p_wk->heapID );

}
//----------------------------------------------------------------------------
/**
 *	@brief  ���C����ʔj��
 *
 *	@param	BR_RECORD_WORK * p_wk   ���[�N
 *	@param	p_param                 ����
 */
//-----------------------------------------------------------------------------
static void Br_Record_DeleteMainDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM *p_param )
{ 
  { 
    int i;
    for( i = 0; i < BR_RECORD_MSGWINID_M_MAX; i++ )
    { 
      if( p_wk->p_msgwin_m[i] )
      { 
        BR_MSGWIN_Exit( p_wk->p_msgwin_m[i] );
        p_wk->p_msgwin_m[i] = NULL;
      }
    }
  }

  if( p_wk->p_profile_disp )
  { 
    BR_PROFILE_DeleteMainDisplay( p_wk->p_profile_disp );
    p_wk->p_profile_disp  = NULL;
  }

  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_RECORD_M_BTL_SINGLE );
}

//----------------------------------------------------------------------------
/**
 *	@brief	����ʍ\�z
 *
 *	@param	BR_RECORD_WORK * p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Record_CreateSubDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param )
{	
  static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_RECORD_MSGWINID_S_MAX]  =
  { 
    {
      9,
      3,
      14,
      2,
      msg_718
    }
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg;

	BR_RES_LoadBG( p_param->p_res, BR_RES_BG_RECORD_S, p_wk->heapID );
	BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, p_wk->heapID );


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

    p_wk->p_btn[BR_RECORD_BTNID_RETURN] = BR_BTN_Init( &cldata, msg_05, BR_BTN_DATA_SHORT_WIDTH,CLSYS_DRAW_SUB, p_param->p_unit, p_wk->p_bmpoam, p_font, p_msg, &res, p_wk->heapID );
;
  }

  //���b�Z�[�WWIN�쐬
  {
    int i;
    for( i = 0; i < BR_RECORD_MSGWINID_S_MAX; i++ )
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
 *	@param	BR_RECORD_WORK * p_wk   ���[�N
 *	@param	p_param                 ����
 */
//-----------------------------------------------------------------------------
static void Br_Record_DeleteSubDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param )
{ 
  {
    int i;
    for( i = 0; i < BR_RECORD_MSGWINID_S_MAX; i++ )
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
    for( i = 0; i < BR_RECORD_BTNID_MAX; i++ )
    { 
      if( p_wk->p_btn[i] )
      { 
        BR_BTN_Exit( p_wk->p_btn[i] );
        p_wk->p_btn[i]  = NULL;
      }
    }
  }

  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S ); 
	BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_RECORD_S );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �v���t�B�[���؂�ւ��{�^�������������ǂ���
 *
 *	@param	BR_RECORD_WORK * p_wk   ���[�N
 *	@param  x�^�b�`���W
 *	@param  y�^�b�`���W
 *
 *	@return TRUE�ŉ�����  FALSE�ŉ����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Br_Record_GetTrgProfile( BR_RECORD_WORK * p_wk, u32 x, u32 y )
{ 
	GFL_RECT rect;

	rect.left		= (8)*8;
	rect.right	= (8 + 15)*8;
	rect.top		= (2)*8;
	rect.bottom	= (2 + 4)*8;

  return ( ((u32)( x - rect.left) <= (u32)(rect.right - rect.left))
            & ((u32)( y - rect.top) <= (u32)(rect.bottom - rect.top)));
}
