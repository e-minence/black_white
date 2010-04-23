//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_bvrank_proc.c
 *	@brief	�o�g���r�f�I�����N�v���Z�X
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
#include "print/wordset.h"
#include "pokeicon/pokeicon.h"
#include "pm_define.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "msg/msg_battle_rec.h"

//�����̃��W���[��
#include "br_pokesearch.h"
#include "br_inner.h"
#include "br_util.h"
#include "br_btn.h"
#include "br_snd.h"

//�O�����J
#include "br_bvrank_proc.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�����L���O
//=====================================
#define BR_RANK_LIST_LINE  (5)

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	�����L���O
//=====================================
typedef struct 
{
  BMP_MENULIST_DATA *p_list_data;
  BR_LIST_WORK      *p_list;
  WORDSET           *p_word;
  BR_BALLEFF_WORK   *p_balleff[ CLSYS_DRAW_MAX ];

  u16               plt;
  u16               cel;
  u16               cgr[ BR_RANK_LIST_LINE ][ TEMOTI_POKEMAX ];
  s32               cgr_idx;
  GFL_CLWK          *p_poke[ BR_RANK_LIST_LINE ][ TEMOTI_POKEMAX ];
  
  const BR_OUTLINE_DATA *cp_data;
} BR_RANK_WORK;

//-------------------------------------
///	�o�g���r�f�I�����N���C�����[�N
//=====================================
typedef struct 
{
  BMPOAM_SYS_PTR	  p_bmpoam;
  BR_BTN_WORK       *p_btn;
  BR_MSGWIN_WORK    *p_msgwin;
  BR_MSGWIN_WORK    *p_title;
  BR_SEQ_WORK       *p_seq;
  PRINT_QUE         *p_que;
	HEAPID            heapID;
  BR_BALLEFF_WORK   *p_balleff[ CLSYS_DRAW_MAX ];
  BR_TEXT_WORK      *p_text;

  BR_RANK_WORK      rank;
  
  u32               cnt;

  //����
  BR_BVRANK_PROC_PARAM	*p_param;
  
} BR_BVRANK_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�o�g���r�f�I�����L���O�v���Z�X
//=====================================
static GFL_PROC_RESULT BR_BVRANK_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_BVRANK_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_BVRANK_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	�V�[�P���X
//=====================================
static void Br_Seq_FadeInBefore( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Seq_FadeOutAfter( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Seq_FadeIn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Seq_FadeOut( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Seq_RankingMain( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Seq_Download( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Seq_NextDownload( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Seq_NextRanking( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	�����L���O���[�N
//=====================================
static void BR_RANK_Init( BR_RANK_WORK *p_wk, const BR_OUTLINE_DATA *cp_data, BR_RES_WORK *p_res, GFL_CLUNIT *p_unit, BR_BALLEFF_WORK *p_balleff_main, BR_BALLEFF_WORK *p_balleff_sub, HEAPID heapID );
static void BR_RANK_Exit( BR_RANK_WORK *p_wk );
static void BR_RANK_Main( BR_RANK_WORK *p_wk, HEAPID heapID );
static u32  BR_RANK_GetSelect( const BR_RANK_WORK *cp_wk );
static void Br_Rank_CreatePokeIcon( BR_RANK_WORK *p_wk, GFL_CLUNIT *p_unit, HEAPID heapID );
static void Br_Rank_DeletePokeIcon( BR_RANK_WORK *p_wk );
static void Br_Rank_ScrollPokeIcon( BR_RANK_WORK *p_wk, u32 list, s8 dir, HEAPID heapID );

typedef struct
{ 
  u32 mons_tbl[ TEMOTI_POKEMAX ];
  u32 form_tbl[ TEMOTI_POKEMAX ];
  u32 gender_tbl[ TEMOTI_POKEMAX ];
}BR_RANK_SORT_POKE_DATA;
static void Br_Rank_SortPoke( const BATTLE_REC_OUTLINE_RECV *cp_recv, BR_RANK_SORT_POKE_DATA *p_sort );

//-------------------------------------
///	���̑�
//=====================================
static BOOL Br_Rank_IsPushBattleBV( u32 x, u32 y );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	�o�g���r�f�I�����N�v���Z�X
//=====================================
const GFL_PROC_DATA BR_BVRANK_ProcData =
{	
	BR_BVRANK_PROC_Init,
	BR_BVRANK_PROC_Main,
	BR_BVRANK_PROC_Exit,
};

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================

//=============================================================================
/**
 *					�o�g���r�f�I�����N�v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�����N�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVRANK_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BVRANK_WORK				*p_wk;
	BR_BVRANK_PROC_PARAM	*p_param	= p_param_adrs;

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_BVRANK_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_BVRANK_WORK) );	
  p_wk->p_param   = p_param_adrs;
	p_wk->heapID	  = BR_PROC_SYS_GetHeapID( p_param->p_procsys );
  p_wk->p_bmpoam	= BmpOam_Init( p_wk->heapID, p_param->p_unit );
  p_wk->p_que     = PRINTSYS_QUE_Create( p_wk->heapID );

  {
    int i;
    for( i = 0; i < CLSYS_DRAW_MAX; i++ )
    {
      p_wk->p_balleff[i] = BR_BALLEFF_Init( p_param->p_unit, p_param->p_res, i, p_wk->heapID );
    }
  }

  //�V�[�P���X�Ǘ�
  p_wk->p_seq = BR_SEQ_Init( p_wk, Br_Seq_NextDownload, p_wk->heapID );

  //���R�[�h����̖߂�̏ꍇ�͂��łɃ_�E�����[�h���Ă���
  if( p_wk->p_param->is_return )
  { 
    BR_SEQ_SetNext( p_wk->p_seq, Br_Seq_FadeInBefore );
  }
  else
  { 
    BR_SEQ_SetNext( p_wk->p_seq, Br_Seq_NextDownload );
  }

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�����N�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVRANK_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BVRANK_WORK				*p_wk	= p_wk_adrs;
	BR_BVRANK_PROC_PARAM	*p_param	= p_param_adrs;

  BR_SEQ_Exit( p_wk->p_seq );

  if( p_wk->p_text )
  { 
    BR_TEXT_Exit( p_wk->p_text, p_param->p_res );
    p_wk->p_text  = NULL;
  }

	//���W���[���j��
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

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�����N�v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVRANK_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BVRANK_WORK	*p_wk	= p_wk_adrs;

  //�V�[�P���X����
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

  //�����\������
  PRINTSYS_QUE_Main( p_wk->p_que );
  if( p_wk->p_msgwin )
  { 
    BR_MSGWIN_PrintMain( p_wk->p_msgwin );
  }
  if( p_wk->p_title )
  { 
    BR_MSGWIN_PrintMain( p_wk->p_title );
  }

  //�e�L�X�g����
  if( p_wk->p_text )
  {  
    BR_TEXT_PrintMain( p_wk->p_text );
  }

	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *  �V�[�P���X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�C���O����
 *
 *	@param	BR_SEQ_WORK *p_seqwk   ���[�N
 *	@param	*p_seq                 �����V�[�P���X
 *	@param	*p_wk_adrs             ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Seq_FadeInBefore( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BR_BVRANK_WORK	*p_wk	= p_wk_adrs;
  GFL_FONT    *p_font = BR_RES_GetFont( p_wk->p_param->p_res );
  GFL_MSGDATA *p_msg  = BR_RES_GetMsgData( p_wk->p_param->p_res );

  if( p_wk->p_text )
  { 
    BR_TEXT_Exit( p_wk->p_text, p_wk->p_param->p_res );
    p_wk->p_text  = NULL;
  }

	//�O���t�B�b�N������
  BR_RES_LoadBG( p_wk->p_param->p_res, BR_RES_BG_BVRANK, p_wk->heapID );
  BR_RES_LoadOBJ( p_wk->p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, p_wk->heapID );

  //�{�^��
  { 
    GFL_CLWK_DATA cldata;
    BR_RES_OBJ_DATA res;
    BOOL ret;

    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

    cldata.pos_x    = 80;
    cldata.pos_y    = 168;
    cldata.anmseq   = 0;
    cldata.softpri  = 1;

    ret = BR_RES_GetOBJRes( p_wk->p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, &res );
    GF_ASSERT( ret );

    p_wk->p_btn = BR_BTN_Init( &cldata, msg_05, BR_BTN_DATA_SHORT_WIDTH, CLSYS_DRAW_SUB, p_wk->p_param->p_unit, p_wk->p_bmpoam, p_font, p_msg, &res, p_wk->heapID );
;
  }

  //���b�Z�[�W
  { 
    //�o�g���r�f�I������
    p_wk->p_msgwin  = BR_MSGWIN_Init( BG_FRAME_S_FONT, 11, 6, 10, 4, PLT_BG_S_FONT, p_wk->p_que, p_wk->heapID );
    BR_MSGWIN_PrintColor( p_wk->p_msgwin, p_msg, msg_603, p_font, BR_PRINT_COL_NORMAL );

    //�o�g���r�f�I��������R�O��
    p_wk->p_title = BR_MSGWIN_Init( BG_FRAME_M_FONT, 3, 3, 26, 4, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );
    BR_MSGWIN_PrintColor( p_wk->p_title, p_msg, msg_605+p_wk->p_param->mode, p_font, BR_PRINT_COL_NORMAL );
    //todo
  }

  //�_�E�����[�h���ăA�E�g���C�������܂��Ă��烉���N���쐬
  BR_RANK_Init( &p_wk->rank, p_wk->p_param->p_outline, p_wk->p_param->p_res, p_wk->p_param->p_unit, p_wk->p_balleff[ CLSYS_DRAW_MAIN ], p_wk->p_balleff[ CLSYS_DRAW_SUB ], p_wk->heapID );
  //�A�C�R���쐬
  Br_Rank_CreatePokeIcon( &p_wk->rank, p_wk->p_param->p_unit, p_wk->heapID );

  BR_SEQ_SetNext( p_seqwk,Br_Seq_FadeIn );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�A�E�g�㏈��
 *
 *	@param	BR_SEQ_WORK *p_seqwk   ���[�N
 *	@param	*p_seq                 �����V�[�P���X
 *	@param	*p_wk_adrs             ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Seq_FadeOutAfter( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BR_BVRANK_WORK	*p_wk	= p_wk_adrs;

  //�����N�j��
  Br_Rank_DeletePokeIcon( &p_wk->rank );
  BR_RANK_Exit( &p_wk->rank );  
  
  BR_MSGWIN_Exit( p_wk->p_title );
  BR_MSGWIN_Exit( p_wk->p_msgwin );
  GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );
  GFL_BG_LoadScreenReq( BG_FRAME_M_FONT );

  BR_BTN_Exit( p_wk->p_btn );
  
  BR_RES_UnLoadOBJ( p_wk->p_param->p_res, BR_RES_OBJ_SHORT_BTN_S );
  BR_RES_UnLoadBG( p_wk->p_param->p_res, BR_RES_BG_BVRANK );


  BR_SEQ_End( p_seqwk );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�C��
 *
 *	@param	BR_SEQ_WORK *p_seqwk   ���[�N
 *	@param	*p_seq                 �����V�[�P���X
 *	@param	*p_wk_adrs             ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Seq_FadeIn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_FADEIN_END,
  };

  BR_BVRANK_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    *p_seq  = SEQ_FADEIN_WAIT;
    break;

  case SEQ_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      *p_seq  = SEQ_FADEIN_END;
    }
    break;

  case SEQ_FADEIN_END:
    BR_SEQ_SetNext( p_seqwk,Br_Seq_RankingMain );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�A�E�g
 *
 *	@param	BR_SEQ_WORK *p_seqwk   ���[�N
 *	@param	*p_seq                �����V�[�P���X
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Seq_FadeOut( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_FADEOUT_END,
  };

  BR_BVRANK_WORK	*p_wk	= p_wk_adrs;

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
    BR_SEQ_SetNext( p_seqwk, Br_Seq_FadeOutAfter );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����L���O���상�C��
 *
 *	@param	BR_SEQ_WORK *p_seqwk   ���[�N
 *	@param	*p_seq                 �����V�[�P���X
 *	@param	*p_wk_adrs             ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Seq_RankingMain( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BR_BVRANK_WORK	*p_wk	= p_wk_adrs;


  BR_RANK_Main( &p_wk->rank, p_wk->heapID );
  {
    u32 x, y;
    if( GFL_UI_TP_GetPointTrg( &x, &y ) )
    {
      if( BR_BTN_GetTrg( p_wk->p_btn, x, y ) )
      { 
        { 
          GFL_POINT pos;
          pos.x = x;
          pos.y = y;
          BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
        }

        BR_PROC_SYS_Pop( p_wk->p_param->p_procsys );
        BR_SEQ_SetNext( p_seqwk,Br_Seq_FadeOut );
      }

      if( Br_Rank_IsPushBattleBV( x, y ) )
      { 
        u32 select  = BR_RANK_GetSelect( &p_wk->rank );
        BATTLE_REC_OUTLINE_RECV *p_recv = p_wk->p_param->p_outline->data;
 
        GF_ASSERT( select < p_wk->p_param->p_outline->data_num );

        //���Ԗڂ̃f�[�^����݂ɍs����
        p_wk->p_param->p_outline->data_idx  = select;

        BR_PROC_SYS_Push( p_wk->p_param->p_procsys, BR_PROCID_RECORD );
        BR_SEQ_SetNext( p_seqwk,Br_Seq_FadeOut );

        //���o
        { 
          GFL_POINT pos;
          pos.x = x;
          pos.y = y;
          BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
        }
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �_�E�����[�h���C��
 *
 *	@param	BR_SEQ_WORK *p_seqwk   ���[�N
 *	@param	*p_seq                 �����V�[�P���X
 *	@param	*p_wk_adrs             ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Seq_Download( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_DOWNLOAD_START,
    SEQ_DOWNLOAD_WAIT,
    SEQ_DOWNLOAD_END,

    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_FADEOUT_END,

  };

  BR_BVRANK_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_DOWNLOAD_START:
    { 
      BR_NET_REQUEST        type;
      BR_NET_REQUEST_PARAM  req_param;

      GFL_STD_MemClear( &req_param, sizeof(BR_NET_REQUEST_PARAM) );
      switch( p_wk->p_param->mode )
      { 
      case BR_BVRANK_MODE_NEW:       //�ŐV�R�O��
        /* req_param�͕K�v�Ȃ� */
        type  = BR_NET_REQUEST_NEW_RANKING_DOWNLOAD;
        break;
      case BR_BVRANK_MODE_FAVORITE:  //�ʐM�ΐ탉���L���O
        /* req_param�͕K�v�Ȃ� */
        type  = BR_NET_REQUEST_FAVORITE_RANKING_DOWLOAD;
        break;
      case BR_BVRANK_MODE_SUBWAY:    //�T�u�E�F�C
        /* req_param�͕K�v�Ȃ� */
        type  = BR_NET_REQUEST_SUBWAY_RANKING_DOWNLOAD;
        break;
      case BR_BVRANK_MODE_SEARCH:    //��������
        req_param.download_video_search_data  = p_wk->p_param->search_data;
        type  = BR_NET_REQUEST_VIDEO_SEARCH_DOWNLOAD;
        break;
      default:
        GF_ASSERT(0);
      }

      //��M���Ȃ����̂ŃN���A
      GFL_STD_MemClear( p_wk->p_param->p_outline, sizeof(BR_OUTLINE_DATA ) );

      PMSND_PlaySE( BR_SND_SE_SEARCH );
      p_wk->cnt = 0;
      BR_NET_StartRequest( p_wk->p_param->p_net, type, &req_param );
      *p_seq  = SEQ_DOWNLOAD_WAIT;
    }
    break;
  case SEQ_DOWNLOAD_WAIT:
    p_wk->cnt++;
    if( BR_NET_WaitRequest( p_wk->p_param->p_net ) )
    { 
      if( p_wk->cnt > RR_SEARCH_SE_FRAME )
      { 
        PMSND_PlaySE( BR_SND_SE_SEARCH_OK );
        BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_MAIN ], BR_BALLEFF_MOVE_NOP, NULL );
        *p_seq  = SEQ_DOWNLOAD_END;
      }
    }
    break;
  case SEQ_DOWNLOAD_END:
    { 
      BR_NET_ERR_RETURN err;
      int msg;
      BOOL is_exist;

      BATTLE_REC_OUTLINE_RECV *p_recv = p_wk->p_param->p_outline->data;
      int *p_data_num                 = &p_wk->p_param->p_outline->data_num;


      err = BR_NET_GetError( p_wk->p_param->p_net, &msg );

      switch( p_wk->p_param->mode )
      { 
      case BR_BVRANK_MODE_NEW:       //�ŐV�R�O��
        is_exist  = BR_NET_GetDownloadNewRanking( p_wk->p_param->p_net, p_recv, BR_OUTLINE_RECV_MAX, p_data_num );
        break;
      case BR_BVRANK_MODE_FAVORITE:  //�ʐM�ΐ탉���L���O
        is_exist  = BR_NET_GetDownloadNewRanking( p_wk->p_param->p_net, p_recv, BR_OUTLINE_RECV_MAX, p_data_num);
        break;
      case BR_BVRANK_MODE_SUBWAY:    //�T�u�E�F�C
        is_exist  = BR_NET_GetDownloadFavoriteRanking( p_wk->p_param->p_net, p_recv, BR_OUTLINE_RECV_MAX, p_data_num);
        break;
      case BR_BVRANK_MODE_SEARCH:    //��������
        is_exist  = BR_NET_GetDownloadBattleSearch( p_wk->p_param->p_net, p_recv, BR_OUTLINE_RECV_MAX, p_data_num );
        break;
      default:
        GF_ASSERT(0);
      }


      if( err == BR_NET_ERR_RETURN_NONE )
      { 
        if( is_exist )
        { 
          //�擾�ł���
          BR_SEQ_SetNext( p_seqwk, Br_Seq_NextRanking );
        }
        else
        { 
          BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_info_031 );
          *p_seq  = SEQ_FADEOUT_START;
        }
      }
      else
      { 
        BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg );
        *p_seq  = SEQ_FADEOUT_START;
      }
    }
    break;

  case SEQ_FADEOUT_START:
    if( GFL_UI_TP_GetTrg() )
    { 
      BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
      *p_seq  = SEQ_FADEOUT_WAIT;
    }
    break;

  case SEQ_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      *p_seq  = SEQ_FADEOUT_END;
    }
    break;

  case SEQ_FADEOUT_END:
    //�擾�ł��Ȃ�����
    BR_PROC_SYS_Pop( p_wk->p_param->p_procsys );
    BR_SEQ_End( p_seqwk );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �_�E�����[�h�ւ̐ڑ�
 *
 *	@param	BR_SEQ_WORK *p_seqwk   ���[�N
 *	@param	*p_seq                 �����V�[�P���X
 *	@param	*p_wk_adrs             ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Seq_NextDownload( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHANGE_MAIN,
    SEQ_CHANGE_FADEIN_START,
    SEQ_CHANGE_FADEIN_WAIT,
    SEQ_END,
  };

  BR_BVRANK_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHANGE_MAIN:

    if( p_wk->p_text == NULL )
    { 
      p_wk->p_text    = BR_TEXT_Init( p_wk->p_param->p_res, p_wk->p_que, p_wk->heapID );
    }
    BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_info_010 );

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
    BR_SEQ_SetNext( p_seqwk, Br_Seq_Download );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����L���O�ւ̐ڑ�
 *
 *	@param	BR_SEQ_WORK *p_seqwk   ���[�N
 *	@param	*p_seq                 �����V�[�P���X
 *	@param	*p_wk_adrs             ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Seq_NextRanking( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHANGE_FADEOUT_START,
    SEQ_CHANGE_FADEOUT_WAIT,
    SEQ_END,
  };

  BR_BVRANK_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHANGE_FADEOUT_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_CHANGE_FADEOUT_WAIT;
    break;
  case SEQ_CHANGE_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_END;
    }
    break;

  case SEQ_END:
    BR_SEQ_SetNext( p_seqwk, Br_Seq_FadeInBefore );
    break;
  }
}

//=============================================================================
/**
 *  �����L���O���[�N
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �����L���O��������
 *
 *	@param	BR_RANK_WORK *p_wk        ���[�N
 *	@param	BR_OUTLINE_DATA *cp_data  ���X�g�ɕ\������f�[�^
 *	@param	*p_res  ���\�[�X
 *	@param	*p_unit ���j�b�g
 *	@param  balleff �{�[�����o
 *	@param	heapID  �q�[�vID
 */
//-----------------------------------------------------------------------------
static void BR_RANK_Init( BR_RANK_WORK *p_wk, const BR_OUTLINE_DATA *cp_data, BR_RES_WORK *p_res, GFL_CLUNIT *p_unit, BR_BALLEFF_WORK *p_balleff_main, BR_BALLEFF_WORK *p_balleff_sub, HEAPID heapID )
{ 
  GFL_FONT    *p_font;
  GFL_MSGDATA *p_msg;
  p_font  = BR_RES_GetFont( p_res );
  p_msg   = BR_RES_GetMsgData( p_res );

  //���[�N������
  GFL_STD_MemClear( p_wk, sizeof(BR_RANK_WORK) );
  p_wk->cp_data   = cp_data;
  p_wk->p_word    = WORDSET_Create( heapID );
  p_wk->p_balleff[ CLSYS_DRAW_MAIN ] = p_balleff_main;
  p_wk->p_balleff[ CLSYS_DRAW_SUB ] = p_balleff_sub;

  //���X�g�f�[�^�쐬
  { 
    int i;
    p_wk->p_list_data = BmpMenuWork_ListCreate( cp_data->data_num, heapID );

    for( i = 0; i < cp_data->data_num; i++ )
    { 
      p_wk->p_list_data[i].param  = i;
    }
  }
  { 
    static const BR_LIST_PARAM sc_list_param =
    {
      NULL,
      0,
      2,
      8,
      10,
      15,
      PLT_BG_M_FONT,
      BG_FRAME_M_FONT,
      3,
      BR_LIST_TYPE_CURSOR,
      NULL,
      NULL, 
    };
    int i;
    STRBUF  *p_colon;
    STRBUF  *p_str1;
    STRBUF  *p_str2;
    STRBUF  *p_str_name;
    int     w;

    //���X�g�ݒ�
    BR_LIST_PARAM list_param  = sc_list_param;
    list_param.cp_list  = p_wk->p_list_data;
    list_param.list_max = cp_data->data_num;
    list_param.p_res    = p_res;
    list_param.p_unit   = p_unit;
    list_param.p_balleff_main = p_wk->p_balleff[ CLSYS_DRAW_MAIN ];
    list_param.p_balleff_sub  = p_wk->p_balleff[ CLSYS_DRAW_SUB ];
    //���X�g�쐬
    p_wk->p_list  = BR_LIST_Init( &list_param, heapID );

    //���X�g���g��  BMP���O������ݒ肷��
    p_colon     = GFL_MSG_CreateString( p_msg, msg_607 );
    p_str1      = GFL_STR_CreateBuffer( 255, heapID );
    p_str2      = GFL_MSG_CreateString( p_msg, msg_727 );
    p_str_name  = GFL_STR_CreateBuffer( 255, heapID );
    for( i = 0; i < cp_data->data_num; i++ )
    { 
      GFL_BMP_DATA  *p_bmp;
      //�쐬
      p_bmp = GFL_BMP_Create( list_param.w, list_param.str_line, GFL_BMP_16_COLOR, heapID );
      GFL_BMP_Clear( p_bmp, 0 );

      //������
      WORDSET_RegisterNumber( p_wk->p_word, 0, i+1, 2, STR_NUM_DISP_LEFT, STR_NUM_CODE_DEFAULT );
      WORDSET_ExpandStr( p_wk->p_word, p_str1, p_str2 );
      GFL_STR_SetStringCode( p_str_name, cp_data->data[ i ].profile.name );

      w = PRINTSYS_GetStrWidth( p_str1, p_font, 0 );

      //��������
      PRINTSYS_PrintColor( p_bmp, 16-w, 4, p_str1, p_font, BR_PRINT_COL_NORMAL );
      PRINTSYS_PrintColor( p_bmp, 16, 4, p_colon, p_font, BR_PRINT_COL_NORMAL );
      PRINTSYS_PrintColor( p_bmp, 20, 4, p_str_name, p_font, BR_PRINT_COL_NORMAL );

      //���X�g�ɐݒ�
      BR_LIST_SetBmp( p_wk->p_list, i, p_bmp );
    }

    GFL_STR_DeleteBuffer( p_str_name );
    GFL_STR_DeleteBuffer( p_str1 );
    GFL_STR_DeleteBuffer( p_str2 );
    GFL_STR_DeleteBuffer( p_colon );
  }

  BR_LIST_Write( p_wk->p_list );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����L���O��j��
 *
 *	@param	BR_RANK_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BR_RANK_Exit( BR_RANK_WORK *p_wk )
{ 
  int i;
  for( i = 0; i < BR_LIST_GetParam( p_wk->p_list,BR_LIST_PARAM_IDX_LIST_MAX); i++ )
  { 
    GFL_BMP_DATA *p_bmp = BR_LIST_GetBmp( p_wk->p_list, i );
    GFL_BMP_Delete( p_bmp );
  }
  WORDSET_Delete( p_wk->p_word );
  BR_LIST_Exit( p_wk->p_list );
  BmpMenuWork_ListDelete( p_wk->p_list_data );
  GFL_STD_MemClear( p_wk, sizeof(BR_RANK_WORK) );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����L���O���C������
 *
 *	@param	BR_RANK_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void BR_RANK_Main( BR_RANK_WORK *p_wk, HEAPID heapID )
{ 
  u32 pre_list, now_list;
  s8  dir;

  //�����O�̈ʒu���~�����̂ŁAMain�̑O
  pre_list  = BR_LIST_GetParam( p_wk->p_list, BR_LIST_PARAM_IDX_LIST_POS );

  BR_LIST_Main( p_wk->p_list );

  now_list  = BR_LIST_GetParam( p_wk->p_list, BR_LIST_PARAM_IDX_LIST_POS );

  dir   = BR_LIST_GetParam( p_wk->p_list, BR_LIST_PARAM_IDX_MOVE_TIMING );
  if( dir != 0 && pre_list != now_list )
  {
    NAGI_Printf( "timing %d\n", dir );
    Br_Rank_ScrollPokeIcon( p_wk, now_list, dir, heapID );
  }

}

//----------------------------------------------------------------------------
/**
 *	@brief  �����L���O�Ō��ݑI�𒆂̂��̂��擾
 *
 *	@param	BR_RANK_WORK *p_wk  ���[�N
 *
 *	@return �I����������
 */
//-----------------------------------------------------------------------------
static u32  BR_RANK_GetSelect( const BR_RANK_WORK *cp_wk )
{ 
  return BR_LIST_GetSelect( cp_wk->p_list );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����A�C�R�����쐬
 *
 *	@param	BR_RANK_WORK *p_wk  ���[�N
 *	@param	*p_unit             ���j�b�g
 *	@param	heapID              �q�[�vID
 */
//-----------------------------------------------------------------------------
static void Br_Rank_CreatePokeIcon( BR_RANK_WORK *p_wk, GFL_CLUNIT *p_unit, HEAPID heapID )
{ 
  //���X�g�̍ő�l�����C����菭�Ȃ��Ȃ�΁A����ȍ~�͍ő�l�̓��X�g�̍ő�l
  const int c_max = p_wk->cp_data->data_num < BR_RANK_LIST_LINE ? 
    p_wk->cp_data->data_num: BR_RANK_LIST_LINE;

  int i, j;
  GFL_CLWK_DATA cldata;
  ARCHANDLE *p_handle;

  BR_RANK_SORT_POKE_DATA  sort_data;

  //���ʃ��\�[�X�ǂݍ���
  p_handle  = GFL_ARC_OpenDataHandle( ARCID_POKEICON, GFL_HEAP_LOWID(heapID) );
  p_wk->plt = GFL_CLGRP_PLTT_RegisterComp( p_handle, 
          POKEICON_GetPalArcIndex(),
          CLSYS_DRAW_MAIN, PLT_OBJ_M_POKEICON*0x20, heapID );
  p_wk->cel = GFL_CLGRP_CELLANIM_Register( p_handle,
					POKEICON_GetCellArcIndex(), 
          POKEICON_GetAnmArcIndex(), heapID );

  //CLDATA�ݒ�
  GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );
  cldata.anmseq = POKEICON_ANM_HPMAX;
  cldata.softpri  = 0;
  cldata.bgpri    = 0;

  //CLWk�쐬
  for( i = 0; i < c_max; i++ )
  { 
    //�\�[�g
    Br_Rank_SortPoke( &p_wk->cp_data->data[ i ], &sort_data );
    
    //�ǂݍ���
    for( j = 0; j < TEMOTI_POKEMAX; j++ )
    { 
      NAGI_Printf( "mons %d form %d gender %d \n", sort_data.mons_tbl[j], sort_data.form_tbl[i], sort_data.gender_tbl[i] );

      { 
        //�|�P���������Ȃ��Ă��Ă�ł����A���Ƃŕ\�����삷��
        //���X�N���[�����A���\�[�X��؂�ւ���CLWK�����̂܂ܗ��p���邽��
        u32 temp  = sort_data.mons_tbl[j];
        temp = temp == 0? 1: temp;

        //���\�[�X��BR_RANK_LIST_LINE * TEMOTI_POKEMAX���ǂݍ���
        p_wk->cgr[i][j] = GFL_CLGRP_CGR_Register( p_handle,
            POKEICON_GetCgxArcIndexByMonsNumber( temp, sort_data.form_tbl[i], sort_data.gender_tbl[i], FALSE ), FALSE, CLSYS_DRAW_MAIN, heapID );

        //CLWK�쐬
        cldata.pos_x  = 106+4 + j * 24;
        cldata.pos_y  =  76-4 + i * 24;
        p_wk->p_poke[i][j]  = GFL_CLACT_WK_Create( p_unit,
            p_wk->cgr[i][j], p_wk->plt, p_wk->cel,
            &cldata, CLSYS_DRAW_MAIN, heapID );

        GFL_CLACT_WK_SetPlttOffs( p_wk->p_poke[i][j], POKEICON_GetPalNum( temp, sort_data.form_tbl[j], sort_data.gender_tbl[j], FALSE) , CLWK_PLTTOFFS_MODE_OAM_COLOR );
        GFL_CLACT_WK_SetObjMode( p_wk->p_poke[i][j], GX_OAM_MODE_XLU );

        if( sort_data.mons_tbl[j] == 0 )
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_poke[i][j], FALSE );
        }
        else
        { 
          GFL_CLACT_WK_SetDrawEnable( p_wk->p_poke[i][j], TRUE );
        }
      }

    }
  }

  GFL_ARC_CloseDataHandle( p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�����A�C�R������
 *
 *	@param	BR_RANK_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Rank_DeletePokeIcon( BR_RANK_WORK *p_wk )
{  
  int i, j;
  GFL_CLGRP_CELLANIM_Release( p_wk->cel );
  GFL_CLGRP_PLTT_Release( p_wk->plt );

  for( i = 0; i < BR_RANK_LIST_LINE; i++ )
  { 
    for( j = 0; j < TEMOTI_POKEMAX; j++ )
    { 
      if( p_wk->p_poke[i][j] )
      { 
        //���\�[�X�����[�X
        GFL_CLGRP_CGR_Release( p_wk->cgr[i][j] );

        //CLWK�����[�X
        GFL_CLACT_WK_Remove( p_wk->p_poke[i][j] );
        p_wk->p_poke[i][j]  = NULL;
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �|�P�A�C�R�����ēǂݍ���
 *
 *	@param	BR_RANK_WORK *p_wk  ���[�N
 *	@param  now                 �ړ��O�̃��X�g�̈ʒu
 *	@param	dir                 ����
 */
//-----------------------------------------------------------------------------
static void Br_Rank_ScrollPokeIcon( BR_RANK_WORK *p_wk, u32 list, s8 dir, HEAPID heapID )
{ 
  int i, j;
  ARCHANDLE *p_handle;
  BR_RANK_SORT_POKE_DATA  sort_data;

  //���\�[�X�ǂݑւ�
  for( i = 0; i < BR_RANK_LIST_LINE; i++ )
  { 
    for( j = 0; j < TEMOTI_POKEMAX; j++ )
    { 

      //�擪�ƌ���͍s��Ȃ�
      //�����ɂ���ēǂݑւ����Ԃ��Ⴄ
      if( dir == 1 && i != BR_RANK_LIST_LINE-1 )
      { 
        const GFL_CLWK  *cp_src = p_wk->p_poke[i + 1 ][j];
        GFL_CLWK        *p_dst  = p_wk->p_poke[i ][j];
        if( GFL_CLACT_WK_GetDrawEnable( cp_src ) )
        { 
          //CLWK�f�ޓǂݑւ�
          NNSG2dImageProxy  img;
          u8                plt_ofs;
          GFL_CLACT_WK_GetImgProxy( cp_src, &img );
          plt_ofs = GFL_CLACT_WK_GetPlttOffs( cp_src );
          GFL_CLACT_WK_SetImgProxy( p_dst, &img );
          GFL_CLACT_WK_SetPlttOffs( p_dst, plt_ofs, CLWK_PLTTOFFS_MODE_OAM_COLOR );
          GFL_CLACT_WK_SetDrawEnable( p_dst, TRUE );
        }
        else
        { 
          GFL_CLACT_WK_SetDrawEnable( p_dst, FALSE );
        }
      }
      else if( dir == -1 && i != BR_RANK_LIST_LINE-1 )
      { 
        const GFL_CLWK  *cp_src = p_wk->p_poke[BR_RANK_LIST_LINE - 2 -i][j];
        GFL_CLWK        *p_dst  = p_wk->p_poke[BR_RANK_LIST_LINE - 1 -i][j];
        if( GFL_CLACT_WK_GetDrawEnable( cp_src ) )
        { 
          //CLWK�f�ޓǂݑւ�
          //�t����ǂݑւ���
          NNSG2dImageProxy  img;
          u8                plt_ofs;
          GFL_CLACT_WK_GetImgProxy( cp_src, &img );
          plt_ofs = GFL_CLACT_WK_GetPlttOffs( cp_src );
          GFL_CLACT_WK_SetImgProxy( p_dst, &img );
          GFL_CLACT_WK_SetPlttOffs( p_dst, plt_ofs, CLWK_PLTTOFFS_MODE_OAM_COLOR );
          GFL_CLACT_WK_SetDrawEnable( p_dst, TRUE );
        }
        else
        { 
          GFL_CLACT_WK_SetDrawEnable( p_dst, FALSE );
        }
      }
    }
  }

  p_handle  = GFL_ARC_OpenDataHandle( ARCID_POKEICON, GFL_HEAP_LOWID(heapID) );

  //��or�����̃��\�[�X�� TEMOTI_POKEMAX���ǂݍ���

  if( dir == -1 )
  { 
    p_wk->cgr_idx--;
    if( p_wk->cgr_idx  < 0 )
    { 
      p_wk->cgr_idx = BR_RANK_LIST_LINE - 1;
    }
  }

  for( j = 0; j < TEMOTI_POKEMAX; j++ )
  { 
    if( dir == 1 )
    { 
      i = BR_RANK_LIST_LINE - 1;
    }
    else if( dir == -1 )
    { 
      i = 0;
    }

    //�\�[�g
    Br_Rank_SortPoke( &p_wk->cp_data->data[ i + list ], &sort_data );


    if( sort_data.mons_tbl[j] != 0)
    {
      NNSG2dCharacterData *p_chr_data;
      NNSG2dImageProxy  img;
      void *p_chr_buff  = GFL_ARCHDL_UTIL_LoadOBJCharacter( p_handle, POKEICON_GetCgxArcIndexByMonsNumber( sort_data.mons_tbl[j], sort_data.form_tbl[j], sort_data.gender_tbl[j], FALSE ), FALSE, &p_chr_data, GFL_HEAP_LOWID(heapID) );
      GFL_CLGRP_CGR_Replace( p_wk->cgr[ p_wk->cgr_idx ][j], p_chr_data );
      GFL_CLGRP_CGR_GetProxy( p_wk->cgr[ p_wk->cgr_idx ][j], &img );
      GFL_CLACT_WK_SetImgProxy( p_wk->p_poke[i][j], &img );
      GFL_CLACT_WK_SetPlttOffs( p_wk->p_poke[i][j], POKEICON_GetPalNum( sort_data.mons_tbl[j], sort_data.form_tbl[j], sort_data.gender_tbl[j], FALSE) , CLWK_PLTTOFFS_MODE_OAM_COLOR );
      GFL_HEAP_FreeMemory( p_chr_buff );
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_poke[i][j], TRUE );
    }
    else
    { 
      GFL_CLACT_WK_SetDrawEnable( p_wk->p_poke[i][j], FALSE );
    }
  }

  //�L�����u����������O�o�b�t�@�̂悤�Ɏg������
  //�C���f�b�N�X���v�Z
  if( dir == 1 )
  { 
    p_wk->cgr_idx++;
    if( BR_RANK_LIST_LINE - 1 < p_wk->cgr_idx )
    { 
      p_wk->cgr_idx = 0;
    }
  }

  NAGI_Printf( "dir %d cgr%d\n", dir, p_wk->cgr_idx );

  GFL_ARC_CloseDataHandle( p_handle );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ��M�����|�P����
 *
 *	@param	const BATTLE_REC_OUTLINE_RECV *cp_recv
 *	@param	*p_sort                       �\�[�g�����f�[�^
 */
//-----------------------------------------------------------------------------
static void Br_Rank_SortPoke( const BATTLE_REC_OUTLINE_RECV *cp_recv, BR_RANK_SORT_POKE_DATA *p_sort )
{
  GFL_STD_MemClear( p_sort, sizeof(BR_RANK_SORT_POKE_DATA) );

  //�������\�[�g
  { 
    int i;
    int idx;
    int monsno;
    int formno;
    int gender;
    idx = 0;
    //@todo �_�u���ƃV���O���ŋl�ߕ����Ⴄ
    for( i = 0; i < TEMOTI_POKEMAX; i++ )
    { 

      monsno  = cp_recv->head.monsno[ i ];
      formno  = cp_recv->head.form_no_and_sex[ i ] & HEADER_FORM_NO_MASK;
      gender  = ( cp_recv->head.form_no_and_sex[ i ] & HEADER_GENDER_MASK ) >> HEADER_GENDER_SHIFT;

      if( monsno >= MONSNO_MAX )
      { 
        OS_TPrintf( "�I�I�I�����X�^�[�ԍ����s���ł�monsno=%d�I�I�I",monsno );
        monsno  = 0;
      }

      if( monsno == 0 )
      { 
        continue;
      }
      p_sort->mons_tbl[idx]  = monsno;
      p_sort->form_tbl[idx]  = formno;
      p_sort->gender_tbl[idx]  = gender;
      idx++;
    }
  } 
}

//----------------------------------------------------------------------------
/**
 *	@brief  �o�g���r�f�I���݂�  ��������
 *
 *	@param	u32 x X
 *	@param	y     Y
 *
 *	@return TRUE�Ȃ�Ή�����  FALSE�Ȃ�Ή����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Br_Rank_IsPushBattleBV( u32 x, u32 y )
{ 
	GFL_RECT rect;
  BOOL ret;

	rect.left		= (10)*8;
	rect.right	= (21)*8;
	rect.top		= (4)*8;
	rect.bottom	= (11)*8;

  ret = ( ((u32)( x - rect.left) <= (u32)(rect.right - rect.left))
            & ((u32)( y - rect.top) <= (u32)(rect.bottom - rect.top)));

  if( ret )
  { 
    PMSND_PlaySE( BR_SND_SE_OK );
  }

  return ret;
}
