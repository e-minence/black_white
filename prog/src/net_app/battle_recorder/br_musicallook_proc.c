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
#include "br_snd.h"

//�O�����J
#include "br_musicallook_proc.h"

//�~���[�W�J�����W���[�����g���̂�
FS_EXTERN_OVERLAY( musical_shot );

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define BR_MUSICALLOOK_RECV_MAX  (5)

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
  u16                 mons_no;
  u16                 cnt;

  //photo
  u16                 photo_idx;
  BR_PROFILE_WORK     *p_profile_disp;
  MUS_SHOT_PHOTO_WORK *p_photo;
  BR_BTN_WORK	        *p_btn;
  BR_MSGWIN_WORK      *p_msgwin;
  BOOL                is_photo_display; //TRUE�Ŏʐ^FALSE�Ńv���t�B�[��

  //common
  BR_SEQ_WORK         *p_seq;
  BMPOAM_SYS_PTR	  	p_bmpoam;	//BMPOAM�V�X�e��
  PRINT_QUE           *p_que;
	HEAPID              heapID;
  BR_MUSICALLOOK_PROC_PARAM *p_param;
  BR_BALLEFF_WORK     *p_balleff[ CLSYS_DRAW_MAX ];

  //��M�����f�[�^
  MUSICAL_SHOT_RECV   *p_musical_shot_tbl[BR_MUSICALLOOK_RECV_MAX];
  int                 musical_shot_recv_num;
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
///	�V�[�P���X
//=====================================
static void Br_MusicalLook_Seq_FadeIn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_MusicalLook_Seq_FadeOut( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_MusicalLook_Seq_SearchMain( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_MusicalLook_Seq_PhotoMain( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_MusicalLook_Seq_Download( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_MusicalLook_Seq_NextPhoto( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_MusicalLook_Seq_ReturnPhoto( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_MusicalLook_Seq_ReturnSearch( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_MusicalLook_Seq_NextDownload( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_MusicalLook_Seq_ChengePhoto( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_MusicalLook_Seq_ChengeProfile( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	�`��{UTIL
//=====================================
static void Br_MusicalLook_Photo_CreateMainDisplay( BR_MUSICALLOOK_WORK *p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static void Br_MusicalLook_Photo_DeleteMainDisplay( BR_MUSICALLOOK_WORK *p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static void Br_MusicalLook_Photo_CreateSubDisplay( BR_MUSICALLOOK_WORK *p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static void Br_MusicalLook_Photo_DeleteSubDisplay( BR_MUSICALLOOK_WORK *p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static void Br_MusicalLook_Photo_CreateProfile( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static void Br_MusicalLook_Photo_DeleteProfile( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );

static void Br_MusicalLook_Search_CreateDisplay( BR_MUSICALLOOK_WORK *p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static void Br_MusicalLook_Search_DeleteDisplay( BR_MUSICALLOOK_WORK *p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );

static void Br_MusicalLook_Download_CreateDisplay( BR_MUSICALLOOK_WORK *p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );
static void Br_MusicalLook_Download_DeleteDisplay( BR_MUSICALLOOK_WORK *p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param );

static BOOL Br_MusicalLook_GetTrgProfile( BR_MUSICALLOOK_WORK *p_wk, u32 x, u32 y );
static BOOL Br_MusicalLook_GetTrgLeft( BR_MUSICALLOOK_WORK *p_wk, u32 x, u32 y );
static BOOL Br_MusicalLook_GetTrgRight( BR_MUSICALLOOK_WORK *p_wk, u32 x, u32 y );

//=============================================================================
/**
 *					�O�����J
*/
//=============================================================================
//-------------------------------------
///	�~���[�W�J��������v���Z�X
//=====================================
const GFL_PROC_DATA BR_MUSICALLOOK_ProcData =
{	
	BR_MUSICALLOOK_PROC_Init,
	BR_MUSICALLOOK_PROC_Main,
	BR_MUSICALLOOK_PROC_Exit,
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


  GFL_OVERLAY_Load( FS_OVERLAY_ID(musical_shot) );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_MUSICALLOOK_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_MUSICALLOOK_WORK) );	
  p_wk->p_param = p_param;
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );
  p_wk->p_que   = PRINTSYS_QUE_Create( p_wk->heapID );
  { 
    int i;
    GFL_CLUNIT *p_unit;
    p_unit  = BR_GRAPHIC_GetClunit( p_param->p_graphic );
    p_wk->p_bmpoam	= BmpOam_Init( p_wk->heapID, p_unit );

    for( i = 0; i < CLSYS_DRAW_MAX; i++ )
    { 
      p_wk->p_balleff[i] = BR_BALLEFF_Init( p_unit, p_param->p_res, i, p_wk->heapID );
    }
  }

  BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, p_wk->heapID );
  GFL_BG_SetVisible( BG_FRAME_M_TEXT, TRUE );
  //�����`��
  Br_MusicalLook_Search_CreateDisplay( p_wk, p_param );

  //�V�[�P���X�Ǘ�
  p_wk->p_seq = BR_SEQ_Init( p_wk, Br_MusicalLook_Seq_FadeIn, p_wk->heapID );

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

  //�G���[���̉������
  Br_MusicalLook_Photo_DeleteMainDisplay( p_wk, p_param );
  Br_MusicalLook_Photo_DeleteSubDisplay( p_wk, p_param );
  Br_MusicalLook_Photo_DeleteProfile( p_wk, p_param );
  Br_MusicalLook_Search_DeleteDisplay( p_wk, p_param );
  Br_MusicalLook_Download_DeleteDisplay( p_wk, p_param );


  //�V�[�P���X�Ǘ��j��
  BR_SEQ_Exit( p_wk->p_seq );

  //�����j��
  Br_MusicalLook_Search_DeleteDisplay( p_wk, p_param );

  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S );

  { 
    int i;
    for( i = 0; i < CLSYS_DRAW_MAX; i++ )
    { 
      BR_BALLEFF_Exit( p_wk->p_balleff[i] );
    }
  }
  BmpOam_Exit( p_wk->p_bmpoam );
  PRINTSYS_QUE_Delete( p_wk->p_que );
	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );
  GFL_OVERLAY_Unload( FS_OVERLAY_ID(musical_shot) );

	return GFL_PROC_RES_FINISH;
}
#include "debug/debug_nagihashi.h"
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
	BR_MUSICALLOOK_WORK	*p_wk	= p_wk_adrs;

  //�G���[�`�F�b�N
  if( BR_NET_SYSERR_RETURN_DISCONNECT == BR_NET_GetSysError( p_wk->p_param->p_net ) )
  { 
    BR_PROC_SYS_Abort( p_wk->p_param->p_procsys );
    return GFL_PROC_RES_FINISH;
  }

  //�V�[�P���X
  BR_SEQ_Main( p_wk->p_seq );
  if( BR_SEQ_IsEnd( p_wk->p_seq ) )
  { 
    return GFL_PROC_RES_FINISH;
  }

  //�{�[������
  { 
    int i;
    for( i = 0; i < CLSYS_DRAW_MAX; i++ )
    { 
      BR_BALLEFF_Main( p_wk->p_balleff[i] );
    }
  }
 
  //�e�v�����g�\��
  PRINTSYS_QUE_Main( p_wk->p_que );

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

  if( p_wk->p_photo )
  { 
    MUS_SHOT_PHOTO_UpdateSystem( p_wk->p_photo );
  }

	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *      �V�[�P���X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�C��
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Seq_FadeIn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_FADEIN_END,
  };

  BR_MUSICALLOOK_WORK	*p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    if( BR_POKESEARCH_PrintMain( p_wk->p_search ) )
    {
      BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
      *p_seq  = SEQ_FADEIN_WAIT;
    }
    break;
  case SEQ_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      *p_seq  = SEQ_FADEIN_END;
    }
    break;
  case SEQ_FADEIN_END:
    BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_SearchMain );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�A�E�g
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Seq_FadeOut( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_FADEOUT_END,
  };

  BR_MUSICALLOOK_WORK	*p_wk = p_wk_adrs;

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
    BR_SEQ_End( p_wk->p_seq );
    BR_PROC_SYS_Pop( p_wk->p_param->p_procsys );
    break;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �������C��
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Seq_SearchMain( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 

  BR_MUSICALLOOK_WORK	*p_wk = p_wk_adrs;

  BR_POKESEARCH_SELECT select;
  u16 mons_no;

  BR_POKESEARCH_Main( p_wk->p_search );
  select  = BR_POKESEARCH_GetSelect( p_wk->p_search, &mons_no );
  if( select != BR_POKESEARCH_SELECT_NONE )
  { 
    if( select == BR_POKESEARCH_SELECT_DECIDE )
    { 
      p_wk->mons_no = mons_no;
      BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_NextDownload );
    }
    else
    { 
      BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_FadeOut );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ʐ^���
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Seq_PhotoMain( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BR_MUSICALLOOK_WORK	*p_wk = p_wk_adrs;
  u32 x, y;
  if( GFL_UI_TP_GetPointTrg( &x, &y ) )
  {
    //�߂�
    if( BR_BTN_GetTrg( p_wk->p_btn, x, y ) )
    { 
      GFL_POINT pos;
      pos.x = x;
      pos.y = y;
      BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
      BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_ReturnPhoto );
    }

    //�v���t�B�[��
    if( Br_MusicalLook_GetTrgProfile( p_wk, x, y ) )
    { 
      BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_ChengeProfile );
    }

    //��
    if( Br_MusicalLook_GetTrgLeft( p_wk, x, y ) )
    { 
      if( p_wk->photo_idx - 1 < 0 )
      { 
        p_wk->photo_idx = p_wk->musical_shot_recv_num - 1;
      }
      else
      { 
        p_wk->photo_idx--;
      }
      BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_ChengePhoto );
    }

    //�E
    if( Br_MusicalLook_GetTrgRight( p_wk, x, y ) )
    { 
      p_wk->photo_idx++;
      p_wk->photo_idx %= p_wk->musical_shot_recv_num;
      BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_ChengePhoto );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �_�E�����[�h����
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Seq_Download( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_DOWNLOAD_START,
    SEQ_DOWNLOAD_WAIT,
    SEQ_DOWNLOAD_END,

    SEQ_MSG_WAIT,
  };

  BR_MUSICALLOOK_WORK	*p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_DOWNLOAD_START:
    {
      BR_NET_REQUEST_PARAM  req_param;
      GFL_STD_MemClear( &req_param, sizeof(BR_NET_REQUEST_PARAM) );
      req_param.download_musical_shot_search_monsno = p_wk->mons_no;

      p_wk->cnt = 0;
      BR_NET_StartRequest( p_wk->p_param->p_net, BR_NET_REQUEST_MUSICAL_SHOT_DOWNLOAD, &req_param );
    }
    *p_seq  = SEQ_DOWNLOAD_WAIT;
    break;
  case SEQ_DOWNLOAD_WAIT:
    p_wk->cnt++;
    if( BR_NET_WaitRequest( p_wk->p_param->p_net ) )
    { 
      if( p_wk->cnt > RR_SEARCH_SE_FRAME )
      { 
        BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_MAIN ], BR_BALLEFF_MOVE_NOP, NULL );
        *p_seq  = SEQ_DOWNLOAD_END;
      }
    } 
    break;
  case SEQ_DOWNLOAD_END:
    //��M�o�b�t�@�󂯎��
    { 
      BR_NET_ERR_RETURN err;
      int msg;
      BOOL is_recv;

      err = BR_NET_GetError( p_wk->p_param->p_net, &msg );

      if( err == BR_NET_ERR_RETURN_NONE )
      { 

        is_recv = BR_NET_GetDownloadMusicalShot( p_wk->p_param->p_net, p_wk->p_musical_shot_tbl, BR_MUSICALLOOK_RECV_MAX, &p_wk->musical_shot_recv_num );
        if( is_recv && p_wk->musical_shot_recv_num )
        { 
          p_wk->photo_idx = 0;
          BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_NextPhoto );
        }
        else
        { 
          *p_seq  = SEQ_MSG_WAIT;
          BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_info_025 );
        }
      }
      else
      { 
        *p_seq  = SEQ_MSG_WAIT;
        BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg );
      }
    }
    break;
    
  case SEQ_MSG_WAIT:
    if( GFL_UI_TP_GetTrg() )
    { 

      { 
        u32 x, y;
        GFL_POINT pos;

        GFL_UI_TP_GetPointTrg( &x, &y );
        pos.x = x;
        pos.y = y;
        BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
        PMSND_PlaySE( BR_SND_SE_OK );
      }

      BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_ReturnSearch );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �_�E�����[�h��ʂ���ʐ^��ʂւ̂Ȃ�
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Seq_NextPhoto( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHANGE_FADEOUT_START,
    SEQ_CHANGE_FADEOUT_WAIT,
    SEQ_CHANGE_MAIN,
    SEQ_CHANGE_FADEIN_START,
    SEQ_CHANGE_FADEIN_WAIT,
    SEQ_END,
  };

  BR_MUSICALLOOK_WORK	*p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHANGE_FADEOUT_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_MASTERBRIGHT_AND_ALPHA, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_CHANGE_FADEOUT_WAIT;
    break;
  case SEQ_CHANGE_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_CHANGE_MAIN;
    }
    break;
  case SEQ_CHANGE_MAIN:

    Br_MusicalLook_Download_DeleteDisplay( p_wk, p_wk->p_param );
    Br_MusicalLook_Photo_CreateMainDisplay( p_wk, p_wk->p_param );
    Br_MusicalLook_Photo_CreateSubDisplay( p_wk, p_wk->p_param );

    *p_seq  = SEQ_CHANGE_FADEIN_START;
    break;
  case SEQ_CHANGE_FADEIN_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_MASTERBRIGHT_AND_ALPHA, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    *p_seq  = SEQ_CHANGE_FADEIN_WAIT;
    break;
  case SEQ_CHANGE_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_END;
    }
    break;
  case SEQ_END:
    p_wk->is_photo_display  = TRUE;
    BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_PhotoMain );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ʐ^��ʂ��猟����ʂւ̂Ȃ�
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Seq_ReturnPhoto( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHANGE_FADEOUT_START,
    SEQ_CHANGE_FADEOUT_WAIT,
    SEQ_CHANGE_MAIN,
    SEQ_CHANGE_FADEIN_START,
    SEQ_CHANGE_FADEIN_WAIT,
    SEQ_END,
  };

  BR_MUSICALLOOK_WORK	*p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHANGE_FADEOUT_START:  
    if( p_wk->is_photo_display )
    { 
      BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_MASTERBRIGHT_AND_ALPHA, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    }
    else
    { 
      BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    }
    *p_seq  = SEQ_CHANGE_FADEOUT_WAIT;
    break;
  case SEQ_CHANGE_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_CHANGE_MAIN;
    }
    break;
  case SEQ_CHANGE_MAIN:

    Br_MusicalLook_Photo_DeleteProfile( p_wk, p_wk->p_param );
    Br_MusicalLook_Photo_DeleteMainDisplay( p_wk, p_wk->p_param );
    Br_MusicalLook_Photo_DeleteSubDisplay( p_wk, p_wk->p_param );
    Br_MusicalLook_Search_CreateDisplay( p_wk, p_wk->p_param );

    *p_seq  = SEQ_CHANGE_FADEIN_START;
    break;
  case SEQ_CHANGE_FADEIN_START:
    if( p_wk->is_photo_display )
    { 
      BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_MASTERBRIGHT_AND_ALPHA, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    }
    else
    { 
      BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    }
    *p_seq  = SEQ_CHANGE_FADEIN_WAIT;
    break;
  case SEQ_CHANGE_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_END;
    }
    break;
  case SEQ_END:
    BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_SearchMain );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �_�E�����[�h���猟����ʂւ̂Ȃ�
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Seq_ReturnSearch( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHANGE_FADEOUT_START,
    SEQ_CHANGE_FADEOUT_WAIT,
    SEQ_CHANGE_MAIN,
    SEQ_CHANGE_FADEIN_START,
    SEQ_CHANGE_FADEIN_WAIT,
    SEQ_END,
  };

  BR_MUSICALLOOK_WORK	*p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHANGE_FADEOUT_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_CHANGE_FADEOUT_WAIT;
    break;
  case SEQ_CHANGE_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_CHANGE_MAIN;
    }
    break;
  case SEQ_CHANGE_MAIN:

    Br_MusicalLook_Download_DeleteDisplay( p_wk, p_wk->p_param );
    Br_MusicalLook_Search_CreateDisplay( p_wk, p_wk->p_param );

    *p_seq  = SEQ_CHANGE_FADEIN_START;
    break;
  case SEQ_CHANGE_FADEIN_START:
    if( BR_POKESEARCH_PrintMain( p_wk->p_search ) )
    {
      BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
      *p_seq  = SEQ_CHANGE_FADEIN_WAIT;
    }
    break;
  case SEQ_CHANGE_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_END;
    }
    break;
  case SEQ_END:
    BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_SearchMain );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ������ʂ���_�E�����[�h��ʂւ̂Ȃ�
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Seq_NextDownload( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHANGE_FADEOUT_START,
    SEQ_CHANGE_FADEOUT_WAIT,
    SEQ_CHANGE_MAIN,
    SEQ_CHANGE_FADEIN_START,
    SEQ_CHANGE_FADEIN_WAIT,
    SEQ_END,
  };

  BR_MUSICALLOOK_WORK	*p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHANGE_FADEOUT_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_CHANGE_FADEOUT_WAIT;
    break;
  case SEQ_CHANGE_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_CHANGE_MAIN;
    }
    break;
  case SEQ_CHANGE_MAIN:

    Br_MusicalLook_Search_DeleteDisplay( p_wk, p_wk->p_param );
    Br_MusicalLook_Download_CreateDisplay( p_wk, p_wk->p_param );

    {
      GFL_POINT pos;
      pos.x = 256/2;
      pos.y = 192/2;
      BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_MAIN ], BR_BALLEFF_MOVE_BIG_CIRCLE, &pos );
    }

    *p_seq  = SEQ_CHANGE_FADEIN_START;
    break;
  case SEQ_CHANGE_FADEIN_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    *p_seq  = SEQ_CHANGE_FADEIN_WAIT;
    break;
  case SEQ_CHANGE_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_END;
    }
    break;
  case SEQ_END:
    BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_Download );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ʐ^��ʂł̎ʐ^�؂�ւ�
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Seq_ChengePhoto( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHANGE_FADEOUT_START,
    SEQ_CHANGE_FADEOUT_WAIT,
    SEQ_CHANGE_MAIN,
    SEQ_CHANGE_FADEIN_START,
    SEQ_CHANGE_FADEIN_WAIT,
    SEQ_END,
  };

  BR_MUSICALLOOK_WORK	*p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHANGE_FADEOUT_START:
    if( p_wk->is_photo_display )
    { 
      BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_MASTERBRIGHT_WHITE, BR_FADE_DISPLAY_MAIN, BR_FADE_DIR_OUT );
    }
    else
    { 
      BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_MAIN, BR_FADE_DIR_OUT );
    }
    *p_seq  = SEQ_CHANGE_FADEOUT_WAIT;
    break;
  case SEQ_CHANGE_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_CHANGE_MAIN;
    }
    break;
  case SEQ_CHANGE_MAIN:

    if( p_wk->is_photo_display )
    { 
      Br_MusicalLook_Photo_DeleteProfile( p_wk, p_wk->p_param );
      Br_MusicalLook_Photo_DeleteMainDisplay( p_wk, p_wk->p_param );
      Br_MusicalLook_Photo_CreateMainDisplay( p_wk, p_wk->p_param );
    }
    else
    { 
      Br_MusicalLook_Photo_DeleteProfile( p_wk, p_wk->p_param );
      Br_MusicalLook_Photo_DeleteMainDisplay( p_wk, p_wk->p_param );
      Br_MusicalLook_Photo_CreateProfile( p_wk, p_wk->p_param );
    }

    *p_seq  = SEQ_CHANGE_FADEIN_START;
    break;
  case SEQ_CHANGE_FADEIN_START:
    if( p_wk->is_photo_display )
    { 
      BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_MASTERBRIGHT_WHITE, BR_FADE_DISPLAY_MAIN, BR_FADE_DIR_IN );
    }
    else
    { 
      BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_MAIN, BR_FADE_DIR_IN );
    }
    *p_seq  = SEQ_CHANGE_FADEIN_WAIT;
    break;
  case SEQ_CHANGE_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_END;
    }
    break;
  case SEQ_END:
    BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_PhotoMain );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ʐ^��ʂł̃v���t�B�[���؂�ւ�
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Seq_ChengeProfile( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHANGE_FADEOUT_START,
    SEQ_CHANGE_FADEOUT_WAIT,
    SEQ_CHANGE_MAIN,
    SEQ_CHANGE_FADEIN_START,
    SEQ_CHANGE_FADEIN_WAIT,
    SEQ_END,
  };

  BR_MUSICALLOOK_WORK	*p_wk = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHANGE_FADEOUT_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_MASTERBRIGHT_WHITE, BR_FADE_DISPLAY_MAIN, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_CHANGE_FADEOUT_WAIT;
    break;
  case SEQ_CHANGE_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_CHANGE_MAIN;
    }
    break;
  case SEQ_CHANGE_MAIN:
    { 
      BOOL is_musical = p_wk->p_photo != NULL;

      if( is_musical )
      { 
        Br_MusicalLook_Photo_DeleteMainDisplay( p_wk, p_wk->p_param );
        Br_MusicalLook_Photo_CreateProfile( p_wk, p_wk->p_param );
      }
      else
      { 
        Br_MusicalLook_Photo_DeleteProfile( p_wk, p_wk->p_param );
        Br_MusicalLook_Photo_CreateMainDisplay( p_wk, p_wk->p_param );
      }

      //�{�^���̕�����������
      { 
        GFL_FONT    *p_font;
        GFL_MSGDATA *p_msg;
        u16 strID;

        p_font  = BR_RES_GetFont( p_wk->p_param->p_res );
        p_msg   = BR_RES_GetMsgData( p_wk->p_param->p_res );

        if( is_musical )
        { 

          p_wk->is_photo_display  = FALSE;
          strID = msg_719;
        }
        else
        { 
          p_wk->is_photo_display  = TRUE;
          strID = msg_718;
        }

        BR_MSGWIN_PrintColor( p_wk->p_msgwin, p_msg, strID, p_font, BR_PRINT_COL_NORMAL );
      }
    }
    *p_seq  = SEQ_CHANGE_FADEIN_START;
    break;
  case SEQ_CHANGE_FADEIN_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_MASTERBRIGHT_WHITE, BR_FADE_DISPLAY_MAIN, BR_FADE_DIR_IN );
    *p_seq  = SEQ_CHANGE_FADEIN_WAIT;
    break;
  case SEQ_CHANGE_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_END;
    }
    break;
  case SEQ_END:
    BR_SEQ_SetNext( p_seqwk, Br_MusicalLook_Seq_PhotoMain );
    break;
  }
}

//=============================================================================
/**
 *  �`��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �ʐ^  ������
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Photo_CreateMainDisplay( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  if( p_wk->p_photo == NULL )
  { 
    // ----���ʐݒ�

    //���ʓǂݒ����̂��ߔj��
    BR_SIDEBAR_SetVisible( p_param->p_sidebar, CLSYS_DRAW_MAIN, FALSE );
    BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_START_M );
    BR_RES_UnLoadCommon( p_param->p_res, CLSYS_DRAW_MAIN );
    //���ʓǂݒ���
    BR_GRAPHIC_SetMusicalMode( p_param->p_graphic, p_wk->heapID );

    //�~���[�W�J���V���b�g�쐬
    { 
      MUSICAL_SHOT_RECV   *p_musical_shot = p_wk->p_musical_shot_tbl[ p_wk->photo_idx ];
      p_wk->p_photo = MUS_SHOT_PHOTO_InitSystem( &p_musical_shot->mus_shot, p_wk->heapID );
    }
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  �ʐ^  �j��
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Photo_DeleteMainDisplay( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  if( p_wk->p_photo )
  { 
    //����--------------
    //���W���[���j��
    MUS_SHOT_PHOTO_ExitSystem( p_wk->p_photo );
    p_wk->p_photo = NULL;

    //�O���t�B�b�N�ǂݑւ�
    BR_GRAPHIC_ReSetMusicalMode( p_param->p_graphic, p_wk->heapID );
    //�ǂݍ��݂Ȃ���
    BR_RES_LoadCommon( p_param->p_res, CLSYS_DRAW_MAIN, p_wk->heapID );
    BR_RES_LoadBG( p_param->p_res, BR_RES_BG_START_M, p_wk->heapID );
    BR_SIDEBAR_SetVisible( p_param->p_sidebar, CLSYS_DRAW_MAIN, TRUE );
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �t�H�g�̉���ʍ쐬
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Photo_CreateSubDisplay( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  if( p_wk->p_msgwin == NULL )
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
    }

    { 
      //�v���t�B�[�����݂邵�܂�
      p_wk->p_msgwin  = BR_MSGWIN_Init( BG_FRAME_S_FONT, 8, 3, 16, 2, PLT_BG_S_FONT, p_wk->p_que, p_wk->heapID );
      BR_MSGWIN_PrintColor( p_wk->p_msgwin, p_msg, msg_718, p_font, BR_PRINT_COL_NORMAL );
    }

    BR_RES_LoadBG( p_param->p_res, BR_RES_BG_PHOTO_S, p_wk->heapID );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t�H�g�̉���ʔj��
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Photo_DeleteSubDisplay( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  if( p_wk->p_msgwin )
  { 
    BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_PHOTO_S );

    BR_BTN_Exit( p_wk->p_btn );
    p_wk->p_btn = NULL;

    BR_MSGWIN_Exit( p_wk->p_msgwin );
    p_wk->p_msgwin  = NULL;

    BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_PHOTO_S );
  }
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
  if( p_wk->p_profile_disp == NULL )
  { 
    GFL_CLUNIT *p_unit;
    GFL_FONT    *p_font;
    GFL_MSGDATA *p_msg;

    p_font  = BR_RES_GetFont( p_param->p_res );
    p_msg   = BR_RES_GetMsgData( p_param->p_res );
    p_unit  = BR_GRAPHIC_GetClunit( p_param->p_graphic );

    { 
      MUSICAL_SHOT_RECV   *p_musical_shot = p_wk->p_musical_shot_tbl[ p_wk->photo_idx ];
      p_wk->p_profile_disp  = BR_PROFILE_CreateMainDisplay( &p_musical_shot->profile, p_param->p_res, p_unit, p_wk->p_que, BR_PROFILE_TYPE_OTHER, p_wk->heapID );
    }

    BR_MSGWIN_PrintColor( p_wk->p_msgwin, p_msg, msg_719, p_font, BR_PRINT_COL_NORMAL );
  }
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
  if( p_wk->p_profile_disp )
  { 
    BR_PROFILE_DeleteMainDisplay( p_wk->p_profile_disp );
    p_wk->p_profile_disp  = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����쐬
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Search_CreateDisplay( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  if( p_wk->p_search == NULL )
  { 
    GFL_CLUNIT *p_unit  = BR_GRAPHIC_GetClunit( p_param->p_graphic );
    if( p_wk->p_text == NULL )
    { 
      p_wk->p_text    = BR_TEXT_Init( p_param->p_res, p_wk->p_que, p_wk->heapID );
    }
    BR_TEXT_Print( p_wk->p_text, p_param->p_res, msg_303 );
    { 
      ZUKAN_SAVEDATA *p_zkn = GAMEDATA_GetZukanSave( p_param->p_gamedata );
      p_wk->p_search  = BR_POKESEARCH_Init( p_zkn, p_param->p_res, p_unit, p_wk->p_bmpoam, p_param->p_fade, p_wk->p_balleff[ CLSYS_DRAW_MAIN ], p_wk->p_balleff[ CLSYS_DRAW_SUB ], p_wk->heapID ); 
    }
    BR_POKESEARCH_StartUp( p_wk->p_search );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����j��
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Search_DeleteDisplay( BR_MUSICALLOOK_WORK	*p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  if( p_wk->p_search )
  { 
    BR_POKESEARCH_CleanUp( p_wk->p_search );
    BR_POKESEARCH_Exit( p_wk->p_search );
    p_wk->p_search  = NULL;
    GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );
    if( p_wk->p_text )
    { 
      BR_TEXT_Exit( p_wk->p_text, p_param->p_res );
      p_wk->p_text  = NULL;
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �_�E�����[�h��ʍ쐬
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Download_CreateDisplay( BR_MUSICALLOOK_WORK *p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  if( p_wk->p_text == NULL )
  { 
    p_wk->p_text    = BR_TEXT_Init( p_param->p_res, p_wk->p_que, p_wk->heapID );
  }
  BR_TEXT_Print( p_wk->p_text, p_param->p_res, msg_info_018 ); 
}
//----------------------------------------------------------------------------
/**
 *	@brief  �_�E�����[�h��ʔj��
 *
 *	@param	BR_MUSICALLOOK_WORK	*p_wk ���[�N
 *	@param	*p_param                  ����
 */
//-----------------------------------------------------------------------------
static void Br_MusicalLook_Download_DeleteDisplay( BR_MUSICALLOOK_WORK *p_wk, BR_MUSICALLOOK_PROC_PARAM *p_param )
{ 
  if( p_wk->p_text )
  { 
    BR_TEXT_Exit( p_wk->p_text, p_param->p_res );
    p_wk->p_text  = NULL;
  }
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
static BOOL Br_MusicalLook_GetTrgProfile( BR_MUSICALLOOK_WORK *p_wk, u32 x, u32 y )
{ 
	GFL_RECT rect;
  BOOL ret;

	rect.left		= (8)*8;
	rect.right	= (8 + 15)*8;
	rect.top		= (2)*8;
	rect.bottom	= (2 + 4)*8;

  ret = ( ((u32)( x - rect.left) <= (u32)(rect.right - rect.left))
            & ((u32)( y - rect.top) <= (u32)(rect.bottom - rect.top)));

  if( ret )
  { 
    GFL_POINT pos;
    pos.x = x;
    pos.y = y;
    BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
    PMSND_PlaySE( BR_SND_SE_OK );
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���{�^������������
 *
 *	@param  x�^�b�`���W
 *	@param  y�^�b�`���W
 *
 *	@return TRUE�ŉ�����  FALSE�ŉ����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Br_MusicalLook_GetTrgLeft( BR_MUSICALLOOK_WORK *p_wk, u32 x, u32 y )
{ 
	GFL_RECT rect;
  BOOL ret;

	rect.left		= (3)*8;
	rect.right	= (8)*8;
	rect.top		= (8)*8;
	rect.bottom	= (13)*8;

  ret = ( ((u32)( x - rect.left) <= (u32)(rect.right - rect.left))
            & ((u32)( y - rect.top) <= (u32)(rect.bottom - rect.top)));

  if( ret )
  { 
    GFL_POINT pos;
    pos.x = x;
    pos.y = y;
    BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
    PMSND_PlaySE( BR_SND_SE_OK );
  }

  return ret;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �E�{�^������������
 *
 *	@param  x�^�b�`���W
 *	@param  y�^�b�`���W
 *
 *	@return TRUE�ŉ�����  FALSE�ŉ����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Br_MusicalLook_GetTrgRight( BR_MUSICALLOOK_WORK *p_wk, u32 x, u32 y )
{ 
	GFL_RECT rect;
  BOOL ret;

	rect.left		= (23)*8;
	rect.right	= (28)*8;
	rect.top		= (8)*8;
	rect.bottom	= (13)*8;

  ret = ( ((u32)( x - rect.left) <= (u32)(rect.right - rect.left))
            & ((u32)( y - rect.top) <= (u32)(rect.bottom - rect.top)));

  if( ret )
  { 
    GFL_POINT pos;
    pos.x = x;
    pos.y = y;
    BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );

    PMSND_PlaySE( BR_SND_SE_OK );
  }

  return ret;
}
