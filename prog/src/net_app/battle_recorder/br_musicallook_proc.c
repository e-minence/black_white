//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_musicallook_proc.c
 *	@brief	�~���[�W�J��������v���Z�X
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

//�~���[�W�J��
#include "musical/mus_shot_photo.h"
#include "poke_tool/monsno_def.h" //debug�p

//�A�[�J�C�u
#include "msg/msg_battle_rec.h"

//�����̃��W���[��
#include "br_pokesearch.h"
#include "br_inner.h"
#include "br_util.h"
#include "br_btn.h"

//�O�����J
#include "br_musicallook_proc.h"


//�~���[�W�J�����W���[�����g���̂�
FS_EXTERN_OVERLAY( musical );

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define BR_MUSICALLOOK_SUBSEQ_END (0xFFFFFFFF)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�~���[�W�J�������郁�C�����[�N
//=====================================
typedef struct 
{
  //search
  BR_POKESEARCH_WORK  *p_search;
  BR_TEXT_WORK        *p_text;

  //photo
  u32                 photo_seq;
  BOOL                is_photo;
  BR_PROFILE_WORK     *p_profile_disp;
  MUS_SHOT_PHOTO_WORK *p_photo;
  BR_BTN_WORK	        *p_btn;
  BR_MSGWIN_WORK      *p_msgwin;

  //common
  BMPOAM_SYS_PTR	  	p_bmpoam;	//BMPOAM�V�X�e��
  PRINT_QUE           *p_que;
  u32                 sub_seq;
  u32                 next_sub_seq;
	HEAPID              heapID;
} BR_MUSICALLOOK_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�~���[�W�J������v���Z�X
//=====================================
static GFL_PROC_RESULT BR_MUSICALLOOK_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_MUSICALLOOK_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_MUSICALLOOK_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	����
//=====================================
typedef BOOL (*SUBSEQ_FUNCTION)( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
//�������
static BOOL Br_MusicalLook_Seq_Search_Init( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static BOOL Br_MusicalLook_Seq_Search_Main( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static BOOL Br_MusicalLook_Seq_Search_Exit( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
//�~���[�W�J��
static BOOL Br_MusicalLook_Seq_Photo_Init( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static BOOL Br_MusicalLook_Seq_Photo_Main( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static BOOL Br_MusicalLook_Seq_Photo_Exit( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static BOOL Br_MusicalLook_GetTrgProfile( u32 x, u32 y );
//���̑�
static BOOL Br_MusicalLook_GetTrgProfile( u32 x, u32 y );
static void Br_MusicalLook_Photo_CreateProfile( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static void Br_MusicalLook_Photo_DeleteProfile( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static void Br_MusicalLook_Photo_CreateMusical( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static void Br_MusicalLook_Photo_DeleteMusical( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//-------------------------------------
///	�~���[�W�J��������v���Z�X�v���Z�X
//=====================================
const GFL_PROC_DATA BR_MUSICALLOOK_ProcData =
{	
	BR_MUSICALLOOK_PROC_Init,
	BR_MUSICALLOOK_PROC_Main,
	BR_MUSICALLOOK_PROC_Exit,
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
  SUBSEQ_SEARCH,
  SUBSEQ_PHOTO,
};
static const SUBSEQ_FUNCTION sc_subseq_tbl[][SUBSEQ_MAX] =
{ 
  { 
    Br_MusicalLook_Seq_Search_Init,
    Br_MusicalLook_Seq_Search_Main,
    Br_MusicalLook_Seq_Search_Exit,
  },
  { 
    Br_MusicalLook_Seq_Photo_Init,
    Br_MusicalLook_Seq_Photo_Main,
    Br_MusicalLook_Seq_Photo_Exit,
  },

};

//=============================================================================
/**
 *					�~���[�W�J��������v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�~���[�W�J��������v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_MUSICALLOOK_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_MUSICALLOOK_WORK				*p_wk;
	BR_MUSICALLOOK_PROC_PARAM	*p_param	= p_param_adrs;


  GFL_OVERLAY_Load( FS_OVERLAY_ID(musical) );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_MUSICALLOOK_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_MUSICALLOOK_WORK) );	
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );
  p_wk->p_que   = PRINTSYS_QUE_Create( p_wk->heapID );
  { 
    GFL_CLUNIT *p_unit;
    p_unit  = BR_GRAPHIC_GetClunit( p_param->p_graphic );
    p_wk->p_bmpoam	= BmpOam_Init( p_wk->heapID, p_unit );
  }

  BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, p_wk->heapID );

  sc_subseq_tbl[ p_wk->sub_seq ][SUBSEQ_INIT]( p_wk, p_param );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�~���[�W�J��������v���Z�X	�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_MUSICALLOOK_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_MUSICALLOOK_WORK				*p_wk	= p_wk_adrs;
	BR_MUSICALLOOK_PROC_PARAM	*p_param	= p_param_adrs;

  sc_subseq_tbl[ p_wk->sub_seq ][SUBSEQ_EXIT]( p_wk, p_param );

  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S );

  BmpOam_Exit( p_wk->p_bmpoam );
  PRINTSYS_QUE_Delete( p_wk->p_que );
	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(musical) );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�~���[�W�J��������v���Z�X	���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_MUSICALLOOK_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
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

	BR_MUSICALLOOK_WORK	*p_wk	= p_wk_adrs;
	BR_MUSICALLOOK_PROC_PARAM	*p_param	= p_param_adrs;

 switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    (*p_seq)++;
    break;
  case SEQ_FADEIN_WAIT:
    *p_seq  = SEQ_MAIN;
    break;
  case SEQ_MAIN:
    {
      if( sc_subseq_tbl[ p_wk->sub_seq ][SUBSEQ_MAIN]( p_wk, p_param ) )
      { 
        if( p_wk->next_sub_seq == BR_MUSICALLOOK_SUBSEQ_END )
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
    (*p_seq)++;
    break;
  case SEQ_CHANGEOUT_WAIT:
    (*p_seq)++;
    break;
  case SEQ_CHANGEIN_START:
    { 
      sc_subseq_tbl[ p_wk->sub_seq ][SUBSEQ_EXIT]( p_wk, p_param );
      p_wk->sub_seq = p_wk->next_sub_seq;
      sc_subseq_tbl[ p_wk->sub_seq ][SUBSEQ_INIT]( p_wk, p_param );
    }
    (*p_seq)++;
    break;
  case SEQ_CHANGEIN_WAIT:
    *p_seq  = SEQ_MAIN;
    break;

  case SEQ_FADEOUT_START:
    (*p_seq)++;
    break;
  case SEQ_FADEOUT_WAIT:
    *p_seq  = SEQ_EXIT;
    break;
  case SEQ_EXIT:
    NAGI_Printf( "MUSICAL_LOOK: Exit!\n" );
    BR_PROC_SYS_Pop( p_param->p_procsys );
    return GFL_PROC_RES_FINISH;
  }

 if( p_wk->p_msgwin )
 {  
   BR_MSGWIN_PrintMain( p_wk->p_msgwin );
 }
 if( p_wk->p_profile_disp )
 { 
   BR_PROFILE_PrintMain( p_wk->p_profile_disp );
 }
 if( p_wk->p_text )
 {  
   BR_TEXT_PrintMain( p_wk->p_text );
 }

  PRINTSYS_QUE_Main( p_wk->p_que );

	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *      �V�[�P���X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  ����  ������
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱����
 */
//-----------------------------------------------------------------------------
static BOOL Br_MusicalLook_Seq_Search_Init( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  GFL_CLUNIT *p_unit;

  p_unit  = BR_GRAPHIC_GetClunit( p_param->p_graphic );

  p_wk->p_text    = BR_TEXT_Init( p_param->p_res, p_wk->p_que, p_wk->heapID );
  p_wk->p_search  = BR_POKESEARCH_Init( NULL, p_param->p_res, p_unit, p_wk->p_bmpoam, p_wk->heapID ); 
  BR_POKESEARCH_StartUp( p_wk->p_search );

  BR_TEXT_Print( p_wk->p_text, p_param->p_res, msg_303 );

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ����  �j��
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱����
 */
//-----------------------------------------------------------------------------
static BOOL Br_MusicalLook_Seq_Search_Exit( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  BR_POKESEARCH_CleanUp( p_wk->p_search );
  BR_POKESEARCH_Exit( p_wk->p_search );
  GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );
  BR_TEXT_Exit( p_wk->p_text, p_param->p_res );
  p_wk->p_text  = NULL;
  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ����  ���C������
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱����
 */
//-----------------------------------------------------------------------------
static BOOL Br_MusicalLook_Seq_Search_Main( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  BR_POKESEARCH_SELECT select;
  u32 monst_now;

  BR_POKESEARCH_Main( p_wk->p_search );
  select  = BR_POKESEARCH_GetSelect( p_wk->p_search, &monst_now );
  if( select != BR_POKESEARCH_SELECT_NONE )
  { 
    if( select == BR_POKESEARCH_SELECT_DECIDE )
    { 
      p_wk->next_sub_seq  = SUBSEQ_PHOTO;
    }
    else
    { 
      p_wk->next_sub_seq  = BR_MUSICALLOOK_SUBSEQ_END;
    }
    return TRUE;
  }
  else
  { 
    return FALSE;
  }


}

//----------------------------------------------------------------------------
/**
 *	@brief  �V���b�g  ������
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱����
 */
//-----------------------------------------------------------------------------
static BOOL Br_MusicalLook_Seq_Photo_Init( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  GFL_CLUNIT *p_unit;
  GFL_FONT    *p_font;
  GFL_MSGDATA *p_msg;

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );
  p_unit  = BR_GRAPHIC_GetClunit( p_param->p_graphic );

  { 
    GFL_CLUNIT  *p_unit;
    GFL_CLWK_DATA cldata;
    BR_RES_OBJ_DATA res;
    BOOL ret;

    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    cldata.pos_x    = 80;
    cldata.pos_y    = 168;
    cldata.anmseq   = 0;
    cldata.softpri  = 1;

    p_unit  = BR_GRAPHIC_GetClunit( p_param->p_graphic );
    ret = BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, &res );
    GF_ASSERT( ret );

    p_wk->p_btn = BR_BTN_Init( &cldata, msg_05, BR_BTN_DATA_SHORT_WIDTH, CLSYS_DRAW_SUB, p_unit, p_wk->p_bmpoam, p_font, p_msg, &res, p_wk->heapID );
;
  }
 
  { 
    //�v���t�B�[�����݂邵�܂�
    p_wk->p_msgwin  = BR_MSGWIN_Init( BG_FRAME_S_FONT, 9, 3, 14, 2, PLT_BG_S_FONT, p_wk->p_que, p_wk->heapID );
    BR_MSGWIN_PrintColor( p_wk->p_msgwin, p_msg, msg_718, p_font, BR_PRINT_COL_NORMAL );
  }

  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_PHOTO_S, p_wk->heapID );

  //�ŏ��͎ʐ^���o���Ă���
  Br_MusicalLook_Photo_CreateMusical( p_wk, p_param );
  p_wk->is_photo  = TRUE;

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �V���b�g  �j��
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱����
 */
//-----------------------------------------------------------------------------
static BOOL Br_MusicalLook_Seq_Photo_Exit( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 

  if( p_wk->is_photo )
  { 
    Br_MusicalLook_Photo_DeleteMusical( p_wk, p_param );
  }
  else
  { 
    Br_MusicalLook_Photo_DeleteProfile( p_wk, p_param );
  }

  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_PHOTO_S );

  BR_BTN_Exit( p_wk->p_btn );
  p_wk->p_btn = NULL;

  BR_MSGWIN_Exit( p_wk->p_msgwin );
  p_wk->p_msgwin  = NULL;

  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_PHOTO_S );

  return TRUE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �V���b�g  ���C��
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 *
 *	@return TRUE�ŏI��  FALSE�ŉ�葱����
 */
//-----------------------------------------------------------------------------
static BOOL Br_MusicalLook_Seq_Photo_Main( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 

  enum
  { 
    SEQ_MAIN,

    SEQ_CHANGEOUT_START,
    SEQ_CHANGEOUT_WAIT,
    SEQ_CHANGEIN_START,
    SEQ_CHANGEIN_WAIT,
  };

  switch( p_wk->photo_seq )
  { 
  case SEQ_MAIN:
    { 
      u32 x, y;
      if( GFL_UI_TP_GetPointTrg( &x, &y ) )
      {
        if( BR_BTN_GetTrg( p_wk->p_btn, x, y ) )
        { 
          p_wk->next_sub_seq  = SUBSEQ_SEARCH;
          return TRUE;
        }
        if( Br_MusicalLook_GetTrgProfile( x, y ) )
        { 
          p_wk->photo_seq = SEQ_CHANGEOUT_START;
        }
      }
    }
    break;

  case SEQ_CHANGEOUT_START:
    p_wk->photo_seq++;
    break;
  case SEQ_CHANGEOUT_WAIT:
    p_wk->photo_seq++;
    break;
  case SEQ_CHANGEIN_START:
    if( p_wk->is_photo )
    { 
      Br_MusicalLook_Photo_DeleteMusical( p_wk, p_param );
      Br_MusicalLook_Photo_CreateProfile( p_wk, p_param );
    }
    else
    { 
      Br_MusicalLook_Photo_DeleteProfile( p_wk, p_param );
      Br_MusicalLook_Photo_CreateMusical( p_wk, p_param );
    }
    p_wk->is_photo  ^=  1;
    p_wk->photo_seq++;
    break;
  case SEQ_CHANGEIN_WAIT:
    p_wk->photo_seq = SEQ_MAIN;
    break;
  }

  if( p_wk->is_photo )
  { 
    MUS_SHOT_PHOTO_UpdateSystem( p_wk->p_photo );
  }
  else
  { 

  }

  return FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �v���t�B�[��  ������
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Photo_CreateProfile( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  GFL_CLUNIT *p_unit;
  GFL_FONT    *p_font;
  GFL_MSGDATA *p_msg;

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );
  p_unit  = BR_GRAPHIC_GetClunit( p_param->p_graphic );

  p_wk->p_profile_disp  = BR_PROFILE_CreateMainDisplay( NULL, p_param->p_res, p_unit, p_wk->p_que, p_wk->heapID );

  BR_MSGWIN_PrintColor( p_wk->p_msgwin, p_msg, msg_719, p_font, BR_PRINT_COL_NORMAL );


}
//----------------------------------------------------------------------------
/**
 *	@brief  �v���t�B�[�� �j��
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Photo_DeleteProfile( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  BR_PROFILE_DeleteMainDisplay( p_wk->p_profile_disp );
  p_wk->p_profile_disp  = NULL;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �~���[�W�J���ʐ^  ������
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Photo_CreateMusical( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  GFL_FONT    *p_font;
  GFL_MSGDATA *p_msg;

  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );

  //���ʓǂݒ����̂��ߔj��
  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_START_M );
  BR_RES_UnLoadCommon( p_param->p_res, CLSYS_DRAW_MAIN );
  //���ʓǂݒ���
  BR_GRAPHIC_SetMusicalMode( p_param->p_graphic, p_wk->heapID );

 
  //�~���[�W�J���V���b�g�쐬
  { 
    MUSICAL_SHOT_DATA data;
    DEBUG_Br_MusicalLook_GetPhotData( &data );
    p_wk->p_photo = MUS_SHOT_PHOTO_InitSystem( &data, p_wk->heapID );
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  �~���[�W�J���ʐ^  �j��
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Photo_DeleteMusical( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
 //���W���[���j��
  MUS_SHOT_PHOTO_ExitSystem( p_wk->p_photo );

  //�O���t�B�b�N�ǂݑւ�
  BR_GRAPHIC_ReSetMusicalMode( p_param->p_graphic, p_wk->heapID );
  //�ǂݍ��݂Ȃ���
  BR_RES_LoadCommon( p_param->p_res, CLSYS_DRAW_MAIN, p_wk->heapID );
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_START_M, p_wk->heapID );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �v���t�B�[���؂�ւ��{�^�������������ǂ���
 *
 *	@param  x�^�b�`���W
 *	@param  y�^�b�`���W
 *
 *	@return TRUE�ŉ�����  FALSE�ŉ����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Br_MusicalLook_GetTrgProfile( u32 x, u32 y )
{ 
	GFL_RECT rect;

	rect.left		= (8)*8;
	rect.right	= (8 + 15)*8;
	rect.top		= (2)*8;
	rect.bottom	= (2 + 4)*8;

  return ( ((u32)( x - rect.left) <= (u32)(rect.right - rect.left))
            & ((u32)( y - rect.top) <= (u32)(rect.bottom - rect.top)));
}

