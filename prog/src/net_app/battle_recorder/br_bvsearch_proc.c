//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_bvsearch_proc.c
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
#include "system/main.h"

//�A�[�J�C�u
#include "msg/msg_battle_rec.h"
#include "print/global_msg.h"

//�����̃��W���[��
#include "br_pokesearch.h"
#include "br_inner.h"
#include "br_util.h"
#include "br_btn.h"
#include "br_net.h"
#include "br_snd.h"

//�O�����J
#include "br_bvsearch_proc.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define BR_BVSEARCH_SUBSEQ_END (0xFFFFFFFF)

//-------------------------------------
///	������
//=====================================
//����
enum 
{
  BR_BVSEARCH_MSGWINID_M_PLACE_CAPTION,
  BR_BVSEARCH_MSGWINID_M_PLACE,
  BR_BVSEARCH_MSGWINID_M_POKE_CAPTION,
  BR_BVSEARCH_MSGWINID_M_POKE,
  BR_BVSEARCH_MSGWINID_M_AREA_CAPTION,
  BR_BVSEARCH_MSGWINID_M_AREA,
  BR_BVSEARCH_MSGWINID_M_MAX,
} ;
//����ʃ��j���[
enum 
{
  BR_BVSEARCH_MSGWINID_S_PLACE,
  BR_BVSEARCH_MSGWINID_S_POKE,
  BR_BVSEARCH_MSGWINID_S_AREA,
  BR_BVSEARCH_MSGWINID_S_MAX,
} ;
//����ʃ{�^��
enum 
{
  BR_BVSEARCH_BTNID_MENU_RETURN = 0,
  BR_BVSEARCH_BTNID_MENU_OK,
  BR_BVSEARCH_BTNID_MENU_MAX,

  BR_BVSEARCH_BTNID_PLACE_RETURN = 0,
  BR_BVSEARCH_BTNID_PLACE_MAX,


  BR_BVSEARCH_BTNID_MAX = BR_BVSEARCH_BTNID_MENU_MAX,
} ;


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
  //menu
  BR_MSGWIN_WORK      *p_msgwin_s[ BR_BVSEARCH_MSGWINID_S_MAX ];
  BR_BTN_WORK         *p_btn[ BR_BVSEARCH_BTNID_MAX ];

  //place_select
  BMP_MENULIST_DATA   *p_list_data;
  BR_LIST_WORK        *p_list;

  //poke_search
  BR_POKESEARCH_WORK  *p_search;

  //common
  BR_MSGWIN_WORK      *p_msgwin_m[ BR_BVSEARCH_MSGWINID_M_MAX ];
  BR_TEXT_WORK        *p_text;
  BR_BALLEFF_WORK     *p_balleff[ CLSYS_DRAW_MAX ];
  BMPOAM_SYS_PTR	  	p_bmpoam;	//BMPOAM�V�X�e��
  PRINT_QUE           *p_que;
  u32                 sub_seq;
  u32                 next_sub_seq;
	HEAPID heapID;
 
} BR_BVSEARCH_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�o�g���r�f�I�����v���Z�X
//=====================================
static GFL_PROC_RESULT BR_BVSEARCH_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_BVSEARCH_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_BVSEARCH_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	����
//=====================================
typedef BOOL (*SUBSEQ_FUNCTION)( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param );
//���j���[���
static BOOL Br_BvSearch_Seq_Menu_Init( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param );
static BOOL Br_BvSearch_Seq_Menu_Main( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param );
static BOOL Br_BvSearch_Seq_Menu_Exit( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param );
//�����ŒT�����
static BOOL Br_BvSearch_Seq_Place_Init( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param );
static BOOL Br_BvSearch_Seq_Place_Main( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param );
static BOOL Br_BvSearch_Seq_Place_Exit( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param );
//�|�P�����������
static BOOL Br_BvSearch_Seq_Poke_Init( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param );
static BOOL Br_BvSearch_Seq_Poke_Main( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param );
static BOOL Br_BvSearch_Seq_Poke_Exit( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param );

//-------------------------------------
///	���̑�
//=====================================
static void Br_BvSearch_CreateMainDisplay( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param );
static void Br_BvSearch_DeleteMainDisplay( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param );

typedef enum
{ 
  BRSEARCH_DISP_PRINT_PLACE,
  BRSEARCH_DISP_PRINT_POKE,
  BRSEARCH_DISP_PRINT_AREA,
}BRSEARCH_DISP_PRINT_TYPE;
static void Br_BvSearch_PrintMainDisplay( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param, BRSEARCH_DISP_PRINT_TYPE type  );

typedef enum
{ 
  BRSEARCH_MENU_SELECT_PLACE,
  BRSEARCH_MENU_SELECT_POKE,
  BRSEARCH_MENU_SELECT_AREA,
  BRSEARCH_MENU_SELECT_MAX,
  BRSEARCH_MENU_SELECT_NULL = BRSEARCH_MENU_SELECT_MAX,
}BRSEARCH_MENU_SELECT;
static BRSEARCH_MENU_SELECT Br_BvSearch_GetTrgMenu( BR_BVSEARCH_WORK *p_wk );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	�o�g���r�f�I�����v���Z�X
//=====================================
const GFL_PROC_DATA BR_BVSEARCH_ProcData =
{	
	BR_BVSEARCH_PROC_Init,
	BR_BVSEARCH_PROC_Main,
	BR_BVSEARCH_PROC_Exit,
};

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================
//-------------------------------------
///	����
//=====================================
enum
{ 
  SUBSEQ_INIT,
  SUBSEQ_MAIN,
  SUBSEQ_EXIT,
  SUBSEQ_MAX,
};
enum
{ 
  SUBSEQ_MENU,
  SUBSEQ_SELECT_PLACE,
  SUBSEQ_SEARCH_POKE,
};

static const SUBSEQ_FUNCTION sc_subseq_tbl[][SUBSEQ_MAX] =
{ 
  {
    Br_BvSearch_Seq_Menu_Init,
    Br_BvSearch_Seq_Menu_Main,
    Br_BvSearch_Seq_Menu_Exit,
  },
  {
    Br_BvSearch_Seq_Place_Init,
    Br_BvSearch_Seq_Place_Main,
    Br_BvSearch_Seq_Place_Exit,
  },
  {
    Br_BvSearch_Seq_Poke_Init,
    Br_BvSearch_Seq_Poke_Main,
    Br_BvSearch_Seq_Poke_Exit,
  },
};

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
static GFL_PROC_RESULT BR_BVSEARCH_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BVSEARCH_WORK				*p_wk;
	BR_BVSEARCH_PROC_PARAM	*p_param	= p_param_adrs;

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_BVSEARCH_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_BVSEARCH_WORK) );	
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );
  p_wk->p_que   = PRINTSYS_QUE_Create( p_wk->heapID );
  p_wk->p_bmpoam	= BmpOam_Init( p_wk->heapID, p_param->p_unit );
  p_wk->p_text  = BR_TEXT_Init( p_param->p_res, p_wk->p_que, p_wk->heapID );
  BR_TEXT_Print( p_wk->p_text, p_param->p_res, msg_712 );

  {
    int i;
    for( i = 0; i < CLSYS_DRAW_MAX; i++ )
    { 
      p_wk->p_balleff[i] = BR_BALLEFF_Init( p_param->p_unit, p_param->p_res, i, p_wk->heapID );
    }
  }

  //�������ʂ�������
  p_param->search_data.monsno       = BATTLE_REC_SEARCH_MONSNO_NONE;
  p_param->search_data.country_code = BATTLE_REC_SEARCH_COUNTRY_CODE_NONE;
  p_param->search_data.local_code   = BATTLE_REC_SEARCH_LOCAL_CODE_NONE;
  p_param->search_data.battle_mode_no  = BATTLEMODE_SEARCH_NO_ALL;


  //���\�[�X�ǂݍ���
  BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, p_wk->heapID ); 

  //������ʍ\�z
  Br_BvSearch_CreateMainDisplay( p_wk, p_param );
  sc_subseq_tbl[ p_wk->sub_seq ][SUBSEQ_INIT]( p_wk, p_param );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�����v���Z�X	�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVSEARCH_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BVSEARCH_WORK				*p_wk	= p_wk_adrs;
	BR_BVSEARCH_PROC_PARAM	*p_param	= p_param_adrs;

  sc_subseq_tbl[ p_wk->sub_seq ][SUBSEQ_EXIT]( p_wk, p_param );
  Br_BvSearch_DeleteMainDisplay( p_wk, p_param );
  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S ); 

	//���W���[���j��
  { 
    int i;
    for( i = 0; i < CLSYS_DRAW_MAX; i++ )
    { 
      BR_BALLEFF_Exit( p_wk->p_balleff[i] );
    }
  }

  BR_TEXT_Exit( p_wk->p_text, p_param->p_res );
  BmpOam_Exit( p_wk->p_bmpoam );
  PRINTSYS_QUE_Delete( p_wk->p_que );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�����v���Z�X	���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVSEARCH_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
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

	BR_BVSEARCH_WORK	*p_wk	= p_wk_adrs;
	BR_BVSEARCH_PROC_PARAM	*p_param	= p_param_adrs;

	//�v���Z�X����
  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    (*p_seq)++;
    break;
  case SEQ_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_param->p_fade ) )
    { 
      *p_seq  = SEQ_MAIN;
    }
    break;
  case SEQ_MAIN:
    {
      if( sc_subseq_tbl[ p_wk->sub_seq ][SUBSEQ_MAIN]( p_wk, p_param ) )
      { 
        if( p_wk->next_sub_seq == BR_BVSEARCH_SUBSEQ_END )
        { 
          *p_seq  = SEQ_FADEOUT_START;
        }
        else
        { 
          *p_seq  = SEQ_CHANGEOUT_START;
        }
      }
    }
    break;

  case SEQ_CHANGEOUT_START:
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_SUB, BR_FADE_DIR_OUT );
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
      sc_subseq_tbl[ p_wk->sub_seq ][SUBSEQ_EXIT]( p_wk, p_param );
      p_wk->sub_seq = p_wk->next_sub_seq;
      sc_subseq_tbl[ p_wk->sub_seq ][SUBSEQ_INIT]( p_wk, p_param );
    }
    BR_FADE_StartFade( p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_SUB, BR_FADE_DIR_IN );
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
    (*p_seq)++;
    break;
  case SEQ_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_param->p_fade ) )
    { 
      *p_seq  = SEQ_EXIT;
    }
    break;
  case SEQ_EXIT:
    NAGI_Printf( "BVSEARCH: Exit!\n" );
    return GFL_PROC_RES_FINISH;
  }

  //�����\��
  { 
    int i;
    for( i = 0; i < BR_BVSEARCH_MSGWINID_S_MAX; i++ )
    { 
      if( p_wk->p_msgwin_s[i] )
      { 
        BR_MSGWIN_PrintMain( p_wk->p_msgwin_s[i] );
      }
    }
    for( i = 0; i < BR_BVSEARCH_MSGWINID_M_MAX; i++ )
    { 
      if( p_wk->p_msgwin_m[i] )
      { 
        BR_MSGWIN_PrintMain( p_wk->p_msgwin_m[i] );
      }
    }
  }
  if( p_wk->p_text )
  {  
    BR_TEXT_PrintMain( p_wk->p_text );
  }

  PRINTSYS_QUE_Main( p_wk->p_que );
  //�{�[������
  { 
    int i;
    for( i = 0; i < CLSYS_DRAW_MAX; i++ )
    { 
      BR_BALLEFF_Main( p_wk->p_balleff[i] );
    }
  }

  return GFL_PROC_RES_CONTINUE;
}

//=============================================================================
/**
 *    �T�u�V�[�P���X
 */
//=============================================================================
//-------------------------------------
///	���j���[
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���j���[  ������
 *
 *	@param	BR_BVSEARCH_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱���� (���C���̂�)
 */
//-----------------------------------------------------------------------------
static BOOL Br_BvSearch_Seq_Menu_Init( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param )
{ 
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );

  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_BVSEARCH_MENU_S, p_wk->heapID );

  //�{�^���쐬
  { 
    static const struct
    { 
      u8 x;
      u8 y;
      u8 anmseq;
      u16 msgID;
    } sc_btn_data[ BR_BVSEARCH_BTNID_MENU_MAX  ]  =
    { 
      { 
        32,
        168,
        0,
        msg_05,
      },
      { 
        128,
        168,
        1,
        msg_708,
      },
    };

    int i;
    GFL_CLWK_DATA cldata;
    BR_RES_OBJ_DATA res;

    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, &res );

    for( i = 0; i < BR_BVSEARCH_BTNID_MENU_MAX; i++ )
    { 
      cldata.pos_x    = sc_btn_data[ i ].x;
      cldata.pos_y    = sc_btn_data[ i ].y;
      cldata.anmseq   = sc_btn_data[ i ].anmseq;
      cldata.softpri  = 1;
      p_wk->p_btn[i] = BR_BTN_Init( &cldata,  sc_btn_data[ i ].msgID, BR_BTN_DATA_SHORT_WIDTH,CLSYS_DRAW_SUB, p_param->p_unit, p_wk->p_bmpoam, p_font, p_msg, &res, p_wk->heapID );
    }
;
  }

  //���b�Z�[�WWIN�쐬
  {
    static const struct 
    { 
      u8 x;
      u8 y;
      u8 w;
      u8 h;
      u32 msgID;
    } sc_msgwin_data[BR_BVSEARCH_MSGWINID_S_MAX]  =
    { 
      {
        7,
        3,
        17,
        2,
        msg_705,
      },
      { 
        7,
        8,
        17,
        2,
        msg_706,
      },
      { 
        7,
        13,
        17,
        2,
        msg_707,
      },
    };


    int i;
    for( i = 0; i < BR_BVSEARCH_MSGWINID_S_MAX; i++ )
    { 
      p_wk->p_msgwin_s[i]  = BR_MSGWIN_Init( BG_FRAME_S_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_S_FONT, p_wk->p_que, p_wk->heapID );
      BR_MSGWIN_PrintColor( p_wk->p_msgwin_s[i], p_msg, sc_msgwin_data[i].msgID, p_font, BR_PRINT_COL_NORMAL );
    }
  }

  BR_TEXT_Print( p_wk->p_text, p_param->p_res, msg_712 );


  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���j���[  �j��
 *
 *	@param	BR_BVSEARCH_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱���� (���C���̂�)
 */
//-----------------------------------------------------------------------------
static BOOL Br_BvSearch_Seq_Menu_Exit( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param )
{ 

  //���b�Z�[�WWIN�j��
  { 
    int i;
    for( i = 0; i < BR_BVSEARCH_MSGWINID_S_MAX; i++ )
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
    for( i = 0; i < BR_BVSEARCH_BTNID_MENU_MAX; i++ )
    { 
      if( p_wk->p_btn[i] )
      { 
        BR_BTN_Exit( p_wk->p_btn[i] );
        p_wk->p_btn[i]  = NULL;
      }
    }
  }
  
  GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );
  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_BVSEARCH_MENU_S );

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���j���[  ���C������
 *
 *	@param	BR_BVSEARCH_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱���� (���C���̂�)
 */
//-----------------------------------------------------------------------------
static BOOL Br_BvSearch_Seq_Menu_Main( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param )
{ 
  //���j���[�I��
  { 
    BRSEARCH_MENU_SELECT select;
    select  = Br_BvSearch_GetTrgMenu( p_wk );
    if( select != BRSEARCH_MENU_SELECT_NULL )
    { 
      switch( select )
      { 
      case BRSEARCH_DISP_PRINT_PLACE:
        { 
          u32 x, y;
          GFL_POINT pos;

          GFL_UI_TP_GetPointTrg( &x, &y );
          pos.x = x;
          pos.y = y;
          BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
        }
        PMSND_PlaySE( BR_SND_SE_OK );
        p_param->search_data.battle_mode_no = BATTLEMODE_SEARCH_NO_ALL;
        Br_BvSearch_PrintMainDisplay( p_wk, p_param, BRSEARCH_DISP_PRINT_PLACE );
        BR_TEXT_Print( p_wk->p_text, p_param->p_res, msg_710 );
        p_wk->next_sub_seq  = SUBSEQ_SELECT_PLACE;
        return TRUE;

      case BRSEARCH_DISP_PRINT_POKE:
        { 
          u32 x, y;
          GFL_POINT pos;

          GFL_UI_TP_GetPointTrg( &x, &y );
          pos.x = x;
          pos.y = y;
          BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
        }
        PMSND_PlaySE( BR_SND_SE_OK );
        p_param->search_data.monsno     = BATTLE_REC_SEARCH_MONSNO_NONE;
        Br_BvSearch_PrintMainDisplay( p_wk, p_param, BRSEARCH_DISP_PRINT_POKE );
        BR_TEXT_Print( p_wk->p_text, p_param->p_res, msg_711 );
        p_wk->next_sub_seq  = SUBSEQ_SEARCH_POKE;
        return TRUE;

      case BRSEARCH_MENU_SELECT_AREA:
        { 
          MYSTATUS  *p_status = GAMEDATA_GetMyStatus( p_param->p_gamedata );
          int country_code = MyStatus_GetMyNation( p_status );
          int local_code   = MyStatus_GetMyArea( p_status );
          if( country_code == 0 )
          { 
            BR_TEXT_Print( p_wk->p_text, p_param->p_res, msg_725 );
            PMSND_PlaySE( BR_SND_SE_NG );
          }
          else
          { 

            { 
              u32 x, y;
              GFL_POINT pos;

              GFL_UI_TP_GetPointTrg( &x, &y );
              pos.x = x;
              pos.y = y;
              BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
            }
            PMSND_PlaySE( BR_SND_SE_OK );
            if( p_param->search_data.local_code == BATTLE_REC_SEARCH_LOCAL_CODE_NONE )
            { 
              MYSTATUS  *p_status = GAMEDATA_GetMyStatus( p_param->p_gamedata );
              p_param->search_data.country_code = country_code;
              p_param->search_data.local_code   = local_code;
            }
            else
            { 
              p_param->search_data.country_code = BATTLE_REC_SEARCH_COUNTRY_CODE_NONE;
              p_param->search_data.local_code   = BATTLE_REC_SEARCH_LOCAL_CODE_NONE;
            }
            Br_BvSearch_PrintMainDisplay( p_wk, p_param, BRSEARCH_DISP_PRINT_AREA );
          }
        }
        break;
      }
    }
  }

  //�{�^���I��
  { 
    u32 x, y;
    if( GFL_UI_TP_GetPointTrg( &x, &y ) )
    { 
      if( BR_BTN_GetTrg( p_wk->p_btn[BR_BVSEARCH_BTNID_MENU_RETURN], x, y ))
      { 

        GFL_POINT pos;
        pos.x = x;
        pos.y = y;
        BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );

        p_wk->next_sub_seq  = BR_BVSEARCH_SUBSEQ_END;
        BR_PROC_SYS_Pop( p_param->p_procsys );
        return TRUE;
      }
      if( BR_BTN_GetTrg( p_wk->p_btn[BR_BVSEARCH_BTNID_MENU_OK], x, y ))
      { 
        if( p_param->search_data.monsno != BATTLE_REC_SEARCH_MONSNO_NONE
            || p_param->search_data.local_code != BATTLE_REC_SEARCH_LOCAL_CODE_NONE
            || p_param->search_data.battle_mode_no != BATTLEMODE_SEARCH_NO_ALL )
        { 

          GFL_POINT pos;
          pos.x = x;
          pos.y = y;
          BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );

          p_wk->next_sub_seq  = BR_BVSEARCH_SUBSEQ_END;
          BR_PROC_SYS_Push( p_param->p_procsys, BR_PROCID_BV_RANK );
          return TRUE;
        }
        else
        { 
          BR_TEXT_Print( p_wk->p_text, p_param->p_res, msg_713 );
        }
      }
    }
  }

  return FALSE;
}
//-------------------------------------
///	�ꏊ
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �ꏊ�I��  ������
 *
 *	@param	BR_BVSEARCH_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱���� (���C���̂�)
 */
//-----------------------------------------------------------------------------
static BOOL Br_BvSearch_Seq_Place_Init( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param )
{
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 


  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );

  //���\�[�X�擾
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_BVSEARCH_PLACE_S, p_wk->heapID );

  //���X�g�f�[�^�쐬
  { 
    int i;
    p_wk->p_list_data = BmpMenuWork_ListCreate( BATTLEMODE_SEARCH_NO_ALL, p_wk->heapID );
    for( i = 0; i < BATTLEMODE_SEARCH_NO_ALL; i++ )
    { 
      BmpMenuWork_ListAddArchiveString( &p_wk->p_list_data[i], p_msg,
         msg_rule_no_000 + i, i, p_wk->heapID );
    }
  }
  //���X�g�쐬
  {
    static const BR_LIST_PARAM sc_list_param  =
    { 
      NULL,
      0,
      5,
      3,
      22,
      12,
      PLT_BG_S_FONT,
      BG_FRAME_S_FONT,
      4,
      BR_LIST_TYPE_TOUCH,
      NULL,
      NULL,
    };
    BR_LIST_PARAM list_param  = sc_list_param;
    list_param.cp_list  = p_wk->p_list_data;
    list_param.list_max = BATTLEMODE_SEARCH_NO_ALL;
    list_param.p_res    = p_param->p_res;
    list_param.p_unit   = p_param->p_unit;

    p_wk->p_list  = BR_LIST_Init( &list_param, p_wk->heapID );
  } 
  //�{�^���쐬
  { 
    static const struct
    { 
      u8 x;
      u8 y;
      u8 anmseq;
      u16 msgID;
    } sc_btn_data[ BR_BVSEARCH_BTNID_PLACE_MAX  ]  =
    { 
      { 
        80,
        168,
        0,
        msg_05,
      },
    };

    int i;
    GFL_CLWK_DATA cldata;
    BR_RES_OBJ_DATA res;

    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, &res );

    for( i = 0; i < BR_BVSEARCH_BTNID_PLACE_MAX; i++ )
    { 
      cldata.pos_x    = sc_btn_data[ i ].x;
      cldata.pos_y    = sc_btn_data[ i ].y;
      cldata.anmseq   = sc_btn_data[ i ].anmseq;
      cldata.softpri  = 1;
      p_wk->p_btn[i] = BR_BTN_Init( &cldata,  sc_btn_data[ i ].msgID, BR_BTN_DATA_SHORT_WIDTH,CLSYS_DRAW_SUB, p_param->p_unit, p_wk->p_bmpoam, p_font, p_msg, &res, p_wk->heapID );
    }
;
  }

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ꏊ�I��  �j��
 *
 *	@param	BR_BVSEARCH_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱���� (���C���̂�)
 */
//-----------------------------------------------------------------------------
static BOOL Br_BvSearch_Seq_Place_Exit( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param )
{ 
  //�{�^���j��
  { 
    int i;
    for( i = 0; i < BR_BVSEARCH_BTNID_PLACE_MAX; i++ )
    { 
      if( p_wk->p_btn[i] )
      { 
        BR_BTN_Exit( p_wk->p_btn[i] );
        p_wk->p_btn[i]  = NULL;
      }
    }
  }

  //���X�g�j��
  BR_LIST_Exit( p_wk->p_list );
  p_wk->p_list  = NULL;
  BmpMenuWork_ListDelete( p_wk->p_list_data );
  p_wk->p_list_data = NULL;

  //���\�[�X�j��
  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_BVSEARCH_PLACE_S );
  GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );

  return TRUE;

}
//----------------------------------------------------------------------------
/**
 *	@brief  �ꏊ�I��  ���C������
 *
 *	@param	BR_BVSEARCH_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱���� (���C���̂�)
 */
//-----------------------------------------------------------------------------
static BOOL Br_BvSearch_Seq_Place_Main( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param )
{ 
  u32 x, y;
  u32 select;
  BOOL cancel;

  BR_LIST_Main( p_wk->p_list );
  select  = BR_LIST_GetSelect( p_wk->p_list );

  cancel  = GFL_UI_TP_GetPointTrg( &x, &y );
  cancel  &= BR_BTN_GetTrg( p_wk->p_btn[BR_BVSEARCH_BTNID_PLACE_RETURN], x, y );

  //����
  if( select != BR_LIST_SELECT_NONE )
  { 

    { 
      u32 x, y;
      GFL_POINT pos;

      GFL_UI_TP_GetPointTrg( &x, &y );
      pos.x = x;
      pos.y = y;
      BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
    }

    p_param->search_data.battle_mode_no = select;
    Br_BvSearch_PrintMainDisplay( p_wk, p_param, BRSEARCH_DISP_PRINT_PLACE );
    p_wk->next_sub_seq  = SUBSEQ_MENU;
    return TRUE;
  }

  //�L�����Z��
  if( cancel )
  { 
    { 
      u32 x, y;
      GFL_POINT pos;

      GFL_UI_TP_GetPointTrg( &x, &y );
      pos.x = x;
      pos.y = y;
      BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
    }

    p_wk->next_sub_seq  = SUBSEQ_MENU;
    return TRUE;
  }

  return FALSE;
}
//-------------------------------------
///	�|�P����search
//=====================================
//----------------------------------------------------------------------------
/**
 *	@brief  �|�P��������  ������
 *
 *	@param	BR_BVSEARCH_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱���� (���C���̂�)
 */
//-----------------------------------------------------------------------------
static BOOL Br_BvSearch_Seq_Poke_Init( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param )
{ 
  ZUKAN_SAVEDATA *p_zkn = GAMEDATA_GetZukanSave( p_param->p_gamedata );
  p_wk->p_search  = BR_POKESEARCH_Init( p_zkn, p_param->p_res, p_param->p_unit, p_wk->p_bmpoam, p_param->p_fade, p_wk->p_balleff[CLSYS_DRAW_MAIN], p_wk->p_balleff[CLSYS_DRAW_SUB],p_wk->heapID ); 
  BR_POKESEARCH_StartUp( p_wk->p_search );
  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �|�P��������  �j��
 *
 *	@param	BR_BVSEARCH_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱���� (���C���̂�)
 */
//-----------------------------------------------------------------------------
static BOOL Br_BvSearch_Seq_Poke_Exit( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param )
{ 
  BR_POKESEARCH_CleanUp( p_wk->p_search );
  BR_POKESEARCH_Exit( p_wk->p_search );
  GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �|�P��������  ���C������
 *
 *	@param	BR_BVSEARCH_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱���� (���C���̂�)
 */
//-----------------------------------------------------------------------------
static BOOL Br_BvSearch_Seq_Poke_Main( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param )
{ 
 
  BR_POKESEARCH_SELECT select;
  u16 mons_no;

  BR_POKESEARCH_Main( p_wk->p_search );
  select  = BR_POKESEARCH_GetSelect( p_wk->p_search, &mons_no );
  if( select != BR_POKESEARCH_SELECT_NONE )
  { 
    if( select == BR_POKESEARCH_SELECT_DECIDE )
    { 
      //����
      p_param->search_data.monsno = mons_no;
      Br_BvSearch_PrintMainDisplay( p_wk, p_param, BRSEARCH_DISP_PRINT_POKE );
      p_wk->next_sub_seq  = SUBSEQ_MENU;
    }
    else
    { 
      //�L�����Z��
      p_wk->next_sub_seq  = SUBSEQ_MENU;
    }
    return TRUE;
  }
  else
  { 
    return FALSE;
  }
}

//=============================================================================
/**
 *    ����
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ���ʍ쐬
 *
 *	@param	BR_BVSEARCH_WORK	*p_wk ���[�N
 *	@param	*p_param                �p�����[�^
 */
//-----------------------------------------------------------------------------
static void Br_BvSearch_CreateMainDisplay( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param )
{ 
  static const struct 
  { 
    u8 x;
    u8 y;
    u8 w;
    u8 h;
    u32 msgID;
  } sc_msgwin_data[BR_BVSEARCH_MSGWINID_M_MAX]  =
  { 
    { 
      4,
      3,
      16,
      2,
      msg_205,
    },
    { 
      5,
      5,
      22,
      4,
      msg_722,
    },
    { 
      4,
      9,
      16,
      2,
      msg_206,
    },
    { 
      5,
      11,
      22,
      4,
      msg_722,
    },
    { 
      4,
      13,
      16,
      2,
      msg_207,
    },
    { 
      5,
      15,
      22,
      4,
      msg_722,
    },

  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 
  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );

  //�O���t�B�b�N�ǂݍ���
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_BVSEARCH_M, p_wk->heapID );

  //���b�Z�[�WWIN�쐬
  {
    int i;
    for( i = 0; i < BR_BVSEARCH_MSGWINID_M_MAX; i++ )
    { 
      p_wk->p_msgwin_m[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );
      BR_MSGWIN_PrintColor( p_wk->p_msgwin_m[i], p_msg, sc_msgwin_data[i].msgID, p_font, BR_PRINT_COL_NORMAL );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���ʔj��
 *
 *	@param	BR_BVSEARCH_WORK	*p_wk ���[�N
 *	@param	*p_param                �p�����[�^
 */
//-----------------------------------------------------------------------------
static void Br_BvSearch_DeleteMainDisplay( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param )
{ 
  //���b�Z�[�W��j��
  {
    int i;
    for( i = 0; i < BR_BVSEARCH_MSGWINID_M_MAX; i++ )
    {
      if( p_wk->p_msgwin_m[i] )
      { 
        BR_MSGWIN_Exit( p_wk->p_msgwin_m[i] );
        p_wk->p_msgwin_m[i] = NULL;
      }
    }
  }

  //�O���t�B�b�N�j��
  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_BVSEARCH_M );

  GFL_BG_LoadScreenReq( BG_FRAME_M_FONT );
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���ʂ�  ������ݒ�
 *
 *	@param	BR_BVSEARCH_WORK	*p_wk ���[�N
 *	@param	*p_param                �p�����[�^
 */
//-----------------------------------------------------------------------------
static void Br_BvSearch_PrintMainDisplay( BR_BVSEARCH_WORK	*p_wk, BR_BVSEARCH_PROC_PARAM *p_param, BRSEARCH_DISP_PRINT_TYPE type  )
{
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg; 
  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );

  switch( type )
  { 
  case BRSEARCH_DISP_PRINT_PLACE:
    { 
      if( p_param->search_data.battle_mode_no != BATTLEMODE_SEARCH_NO_ALL )
      { 
        BR_MSGWIN_PrintColor( p_wk->p_msgwin_m[ BR_BVSEARCH_MSGWINID_M_PLACE ], p_msg,
            msg_rule_no_000 + p_param->search_data.battle_mode_no, p_font, BR_PRINT_COL_NORMAL );
      }
      else
      { 
        BR_MSGWIN_PrintColor( p_wk->p_msgwin_m[ BR_BVSEARCH_MSGWINID_M_PLACE ], p_msg,
          msg_722, p_font, BR_PRINT_COL_NORMAL );
      }
    }
    break;
  case BRSEARCH_DISP_PRINT_POKE:
    { 
      if( p_param->search_data.monsno != BATTLE_REC_SEARCH_MONSNO_NONE )
      { 
        BR_MSGWIN_PrintColor( p_wk->p_msgwin_m[ BR_BVSEARCH_MSGWINID_M_POKE ], (GFL_MSGDATA*)GlobalMsg_PokeName,
          p_param->search_data.monsno, p_font, BR_PRINT_COL_NORMAL );
      }
      else
      { 
        BR_MSGWIN_PrintColor( p_wk->p_msgwin_m[ BR_BVSEARCH_MSGWINID_M_POKE ], p_msg,
          msg_722, p_font, BR_PRINT_COL_NORMAL );
      }
    }
    break;
  case BRSEARCH_DISP_PRINT_AREA:
    { 
      if( p_param->search_data.local_code != 0 )
      { 
        BR_MSGWIN_PrintColor( p_wk->p_msgwin_m[ BR_BVSEARCH_MSGWINID_M_AREA ], p_msg, msg_722, p_font, BR_PRINT_COL_NORMAL );
      }
      else
      {
        BR_MSGWIN_PrintColor( p_wk->p_msgwin_m[ BR_BVSEARCH_MSGWINID_M_AREA ], p_msg,
          msg_722, p_font, BR_PRINT_COL_NORMAL );
      }
    }
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �v���t�B�[���؂�ւ��{�^�������������ǂ���
 *
 *	@return -1�ŉ����Ă��Ȃ�  ���͎��̃V�[�P���X
 */
//-----------------------------------------------------------------------------
static BRSEARCH_MENU_SELECT Br_BvSearch_GetTrgMenu( BR_BVSEARCH_WORK *p_wk )
{
  static const GFL_RECT sc_rect[ BRSEARCH_MENU_SELECT_MAX ] =
  {
    { 
      3*8,2*8,29*8,5*8,
    },
    { 
      3*8,7*8,29*8,10*8,
    },
    { 
      3*8,12*8,29*8,15*8,
    },
  };
  int i;
  u32 x;
  u32 y;

  if( GFL_UI_TP_GetPointTrg( &x, &y ) )
  { 
    for( i = 0; i < BRSEARCH_MENU_SELECT_MAX; i++ )
    { 
      if( ( ((u32)( x - sc_rect[i].left) <= (u32)(sc_rect[i].right - sc_rect[i].left))
            & ((u32)( y - sc_rect[i].top) <= (u32)(sc_rect[i].bottom - sc_rect[i].top))) )
      { 
        return i;
      }
    }
  }

  return BRSEARCH_MENU_SELECT_NULL;
}

