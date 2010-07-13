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
#include "pokeicon/pokeicon.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "msg/msg_battle_rec.h"

//�����̃��W���[��
#include "br_btn.h"
#include "br_util.h"
#include "br_snd.h"

//�Z�[�u�f�[�^
#include "savedata/battle_rec.h"

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
	BR_RECORD_MAIN_BTNID_START,
	BR_RECORD_MAIN_BTNID_RETURN = BR_RECORD_MAIN_BTNID_START,
	BR_RECORD_MAIN_BTNID_SAVE,
	BR_RECORD_MAIN_BTNID_MAX,

	BR_RECORD_BTNID_MAX,
} BR_RECORD_MAIN_BTNID;

//-------------------------------------
///	MSGWIN
//=====================================
typedef enum
{
  BR_RECORD_MSGWINID_S_PROFILE  = 0,       //�v���t�B�[�����݂�^���܂�
  BR_RECORD_MSGWINID_S_VIDEO_MAX,

  BR_RECORD_MSGWINID_S_YES  = 0,
  BR_RECORD_MSGWINID_S_NO,
  BR_RECORD_MSGWINID_S_SAVE_MAX,


  BR_RECORD_MSGWINID_S_MAX  = BR_RECORD_MSGWINID_S_SAVE_MAX,
} BR_RECORD_MSGWINID_S;
typedef enum
{
  BR_RECORD_MSGWINID_M_BTL_NAME = 0,  //�������̋L�^
  BR_RECORD_MSGWINID_M_BTL_RULE,       //�R���V�A���@�V���O���@��������Ȃ�
  BR_RECORD_MSGWINID_M_BTL_NUMBER,    //�r�f�I�i���o�[
  BR_RECORD_MSGWINID_M_BTL_WIN_NUM,    //��������ڐ�
  BR_RECORD_MSGWINID_M_BTL_WIN,    //���
  BR_RECORD_MSGWINID_M_BTL_MAX,   

  BR_RECORD_MSGWINID_M_MAX  = BR_RECORD_MSGWINID_M_BTL_MAX,
} BR_RECORD_MSGWINID_M;

//-------------------------------------
///	�����ԍ�  ��ʂ̐����E����`�F�b�N�Ɏg�p
//=====================================
typedef enum
{
  BR_RECORD_CREATE_ID_MAIN_DISP_M,
  BR_RECORD_CREATE_ID_MAIN_DISP_S,
  BR_RECORD_CREATE_ID_PROFILE,
  BR_RECORD_CREATE_ID_DOWNLOAD,
  BR_RECORD_CREATE_ID_SAVE,

  BR_RECORD_CREATE_ID_MAX,
} BR_RECORD_CREATE_ID;


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
  BR_SEQ_WORK       *p_seq;
  BR_BALLEFF_WORK   *p_balleff[ CLSYS_DRAW_MAX ];
  BR_BALLEFF_WORK   *p_balleff_sub;
  BR_TEXT_WORK      *p_text;

	BR_BTN_WORK	      *p_btn[ BR_RECORD_BTNID_MAX ];
  BR_MSGWIN_WORK    *p_msgwin_s[ BR_RECORD_MSGWINID_S_MAX ];
  BR_MSGWIN_WORK    *p_msgwin_m[ BR_RECORD_MSGWINID_M_MAX ];

  BR_PROFILE_WORK   *p_profile_disp;

  u32               res_icon_plt;
  u32               res_icon_cel;
  u32               res_icon_chr[HEADER_MONSNO_MAX];
  GFL_CLWK          *p_icon[HEADER_MONSNO_MAX];

  BOOL                  is_profile;
	HEAPID                heapID;

  u16                   sv_wk0;
  u16                   sv_wk1;
  u32                   cnt;

  GDS_PROFILE_PTR       p_profile;
  BATTLE_REC_HEADER_PTR p_header;
  BOOL                  is_secure;

  BOOL                  can_save;

  BR_RECORD_PROC_PARAM	*p_param;


  BOOL                  is_create[BR_RECORD_CREATE_ID_MAX];
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

//-------------------------------------
///	�V�[�P���X
//=====================================
static void Br_Record_Seq_FadeInBefore( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Record_Seq_FadeIn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Record_Seq_FadeOut( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Record_Seq_FadeOutAfter( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Record_Seq_Main( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Record_Seq_ChangeDrawUp( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Record_Seq_NumberDownload( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Record_Seq_VideoDownloadRecPlay( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Record_Seq_SecureSave( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_Record_Seq_RecPlayBeforeSave( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	��ʍ쐬
//=====================================
static void Br_Record_CreateMainDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );
static void Br_Record_AddPokeIcon( BR_RECORD_WORK * p_wk, GFL_CLUNIT *p_clunit, BATTLE_REC_HEADER_PTR p_header, HEAPID heapID );
static void Br_Record_CreateMainDisplayProfile( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );
static void Br_Record_DeleteMainDisplayProfile( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );
static void Br_Record_DeleteMainDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );
//���R�[�h���C�������
static void Br_Record_CreateSubDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );
static void Br_Record_DeleteSubDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );
//�_�E�����[�h���
static void Br_Record_Download_CreateDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );
static void Br_Record_Download_DeleteDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param );
//�Z�[�u���܂����H
static void Br_Record_Save_CreateDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM *p_param );
static void Br_Record_Save_DeleteDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM *p_param );

//-------------------------------------
///	���̑�
//=====================================
static BOOL Br_Record_Check2vs2( BATTLE_REC_HEADER_PTR p_head );
static BOOL Br_Record_CheckCounterVisible( BATTLE_REC_HEADER_PTR p_head );
static BOOL Br_Record_GetTrgProfile( BR_RECORD_WORK * p_wk, u32 x, u32 y );
static BOOL Br_Record_GetTrgStart( BR_RECORD_WORK * p_wk, u32 x, u32 y );
static BOOL Br_Record_GetTrgYes( BR_RECORD_WORK *p_wk, u32 x, u32 y );
static BOOL Br_Record_GetTrgNo( BR_RECORD_WORK *p_wk, u32 x, u32 y );
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
	p_wk->heapID	    = BR_PROC_SYS_GetHeapID( p_param->p_procsys );
  p_wk->is_profile  = FALSE;
  p_wk->p_param     = p_param;

  //���W���[���쐬
  p_wk->p_bmpoam		= BmpOam_Init( p_wk->heapID, p_param->p_unit);
  p_wk->p_que       = PRINTSYS_QUE_Create( p_wk->heapID );
  p_wk->p_seq       = BR_SEQ_Init( p_wk, NULL, p_wk->heapID );

  {
    int i;
    for( i = 0; i < CLSYS_DRAW_MAX; i++ )
    { 
      p_wk->p_balleff[i]   = BR_BALLEFF_Init( p_param->p_unit, p_param->p_res, i, p_wk->heapID );
    }
  }

  //�ǂݍ��ރf�[�^��I��
  switch( p_wk->p_param->mode )
  { 
  case BR_RECODE_PROC_DOWNLOAD_RANK:
    //�����N���炫���Ƃ��̓A�E�g���C������M���Ă��Ă���̂ŁA
    //�A�E�g���C���̃w�b�_���g��
    p_wk->p_header  = &p_wk->p_param->p_outline->data[ p_wk->p_param->p_outline->data_idx ].head;
    p_wk->p_profile  = &p_wk->p_param->p_outline->data[ p_wk->p_param->p_outline->data_idx ].profile;
    p_wk->can_save  = TRUE;

    //�o�g�����畜�A�������������Ă���Z�L���A�t���O�𗧂Ă�
    if( (*p_wk->p_param->cp_is_recplay_finish) && p_wk->p_param->is_return )
    { 
      p_wk->p_param->p_outline->is_secure[ p_wk->p_param->p_outline->data_idx ] = TRUE;
    }
    p_wk->is_secure = p_wk->p_param->p_outline->is_secure[ p_wk->p_param->p_outline->data_idx ];
    break;

  case BR_RECODE_PROC_MY:
    p_wk->p_header  = BattleRec_HeaderPtrGet();
    p_wk->p_profile = BattleRec_GDSProfilePtrGet();
    p_wk->is_secure = TRUE;
    p_wk->can_save  = FALSE;
    break;

  case BR_RECODE_PROC_OTHER_00:
  case BR_RECODE_PROC_OTHER_01:
  case BR_RECODE_PROC_OTHER_02:
    //�ۑ����Ă���f�[�^���g���Ƃ���BRS��ɓǂݍ���ł���̂ŁA������g��
    p_wk->p_header  = BattleRec_HeaderPtrGet();
    p_wk->p_profile = BattleRec_GDSProfilePtrGet();
    p_wk->is_secure = RecHeader_ParamGet( p_wk->p_header, RECHEAD_IDX_SECURE, 0 );
    p_wk->can_save  = FALSE;
    break;

  case BR_RECODE_PROC_DOWNLOAD_NUMBER:
    //�ԍ����痈���Ƃ��͐�ɂ�݂ɂ����̂ŁA�܂��Ȃ�
    if( !p_wk->p_param->is_return )
    { 
      p_wk->p_header  = NULL;
      p_wk->p_profile = NULL;
    }
    else
    { 
      //���������A����BRS��ɓǂ�ł���̂ł�����g��
      p_wk->p_header  = BattleRec_HeaderPtrGet();
      p_wk->p_profile = BattleRec_GDSProfilePtrGet();
      p_wk->is_secure = *p_wk->p_param->cp_is_recplay_finish;
    }
    p_wk->can_save  = TRUE;
    break;
  }

  //�V�[�P���X�w��
  if( p_wk->p_param->mode == BR_RECODE_PROC_DOWNLOAD_NUMBER )
  { 
    //�ԍ��w�肩�痈���ꍇ�̓w�b�_�ƃv���t�@�C�����Ȃ��̂ł����_�E�����[�h
    if( !p_wk->p_param->is_return )
    { 
      BR_SEQ_SetNext( p_wk->p_seq, Br_Record_Seq_NumberDownload );
    }
    else
    { 
      //���A���͂��̂܂܃��R�[�h��ʂ�
      BR_SEQ_SetNext( p_wk->p_seq, Br_Record_Seq_FadeInBefore );
    }
  }
  else
  { 
    //�u���E�U���[�h�̂Ƃ��A�퓬���畜�A���A�����������ăZ�L���A��FALSE�̏ꍇ
    //�Z�L���A��TRUE�ɂ��Ɏ����ς݃Z�[�u�֍s��
    if( (  p_wk->p_param->mode == BR_RECODE_PROC_OTHER_00
        || p_wk->p_param->mode == BR_RECODE_PROC_OTHER_01
        || p_wk->p_param->mode == BR_RECODE_PROC_OTHER_02
        ) && p_wk->p_param->is_return 
        && (*p_wk->p_param->cp_is_recplay_finish && p_wk->is_secure == FALSE) )
    { 
      BR_SEQ_SetNext( p_wk->p_seq, Br_Record_Seq_SecureSave );
    }
    else
    {
      BR_SEQ_SetNext( p_wk->p_seq, Br_Record_Seq_FadeInBefore );
    }
  }

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

  //���
  Br_Record_DeleteMainDisplayProfile( p_wk, p_param );
  Br_Record_DeleteMainDisplay( p_wk, p_param );
  Br_Record_DeleteSubDisplay( p_wk, p_param );
  Br_Record_Download_DeleteDisplay( p_wk, p_param );
  Br_Record_Save_DeleteDisplay( p_wk, p_param );

  //�Ԃ��l
  p_param->is_secure  = p_wk->is_secure;

  //��ʍ\�z�j��
  GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );
  GFL_BG_LoadScreenReq( BG_FRAME_M_FONT );

	//���W���[���j��
  if( p_wk->p_text )
  { 
    BR_TEXT_Exit( p_wk->p_text, p_wk->p_param->p_res );
    p_wk->p_text  = NULL;
  }
  { 
    int i;
    for( i = 0; i < CLSYS_DRAW_MAX; i++ )
    { 
      BR_BALLEFF_Exit( p_wk->p_balleff[i] );
    }
  }
  BR_SEQ_Exit( p_wk->p_seq );
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

	BR_RECORD_WORK	*p_wk	= p_wk_adrs;

  //�G���[�`�F�b�N
  if( BR_NET_SYSERR_RETURN_DISCONNECT == BR_NET_GetSysError( p_wk->p_param->p_net ) )
  { 
    BR_PROC_SYS_Abort( p_wk->p_param->p_procsys );
    return GFL_PROC_RES_FINISH;
  }

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

  //�����\��
  PRINTSYS_QUE_Main( p_wk->p_que );
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
  
  if( p_wk->p_text )
  { 
    BR_TEXT_PrintMain( p_wk->p_text );
  }



	return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *			�V�[�P���X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�C���O����
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Record_Seq_FadeInBefore( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BR_RECORD_WORK	*p_wk	= p_wk_adrs;
	//�O���t�B�b�N������
  Br_Record_CreateSubDisplay( p_wk, p_wk->p_param );

  if( p_wk->is_profile )
  {
    Br_Record_CreateMainDisplayProfile( p_wk, p_wk->p_param );
  }
  else
  { 
    Br_Record_CreateMainDisplay( p_wk, p_wk->p_param );
  }

  BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_FadeIn );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�C��
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Record_Seq_FadeIn( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_FADEIN_END,
  };

  BR_RECORD_WORK	*p_wk	= p_wk_adrs;

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
    BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_Main );
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�A�E�g
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Record_Seq_FadeOut( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_FADEOUT_END,
  };

  BR_RECORD_WORK	*p_wk	= p_wk_adrs;

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
    BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_FadeOutAfter );
    break;
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�A�E�g�㏈��
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Record_Seq_FadeOutAfter( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  BR_RECORD_WORK	*p_wk	= p_wk_adrs;

  //�O���t�B�b�N�j��
  if( p_wk->is_profile )
  { 
    Br_Record_DeleteMainDisplayProfile( p_wk, p_wk->p_param );
  }
  else
  { 
    Br_Record_DeleteMainDisplay( p_wk,  p_wk->p_param );
  }
  Br_Record_DeleteSubDisplay( p_wk, p_wk->p_param );

  BR_SEQ_End( p_seqwk );
}

//----------------------------------------------------------------------------
/**
 *	@brief  ���C������
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Record_Seq_Main( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
 	BR_RECORD_WORK	*p_wk	= p_wk_adrs;
  
  u32 x, y;
  if( GFL_UI_TP_GetPointTrg( &x, &y ) )
  {
    //�v���t�B�[���؂�ւ��`�F�b�N
    if( Br_Record_GetTrgProfile( p_wk, x, y ) )
    { 
      p_wk->is_profile ^= 1;
      BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_ChangeDrawUp );
    }

    //�I���`�F�b�N
    if( BR_BTN_GetTrg( p_wk->p_btn[BR_RECORD_MAIN_BTNID_RETURN], x, y ) )
    {	
      GFL_POINT pos;
      pos.x = x;
      pos.y = y;
      BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
      p_wk->p_param->ret  = BR_RECORD_RETURN_FINISH;
      BR_PROC_SYS_Pop( p_wk->p_param->p_procsys );
      BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_FadeOut );
    }	

    //�^��{�^��
    if( p_wk->p_btn[BR_RECORD_MAIN_BTNID_SAVE] )
    { 
      if( BR_BTN_GetTrg( p_wk->p_btn[BR_RECORD_MAIN_BTNID_SAVE], x, y ) )
      {	
        GFL_POINT pos;
        pos.x = x;
        pos.y = y;
        BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
        //�_�E�����[�h��^��֍s��
        BR_PROC_SYS_Push( p_wk->p_param->p_procsys, BR_PROCID_BV_SAVE );
        p_wk->p_param->video_number = RecHeader_ParamGet( p_wk->p_header, RECHEAD_IDX_DATA_NUMBER, 0 );
        BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_FadeOut );
      }	
    }

    //�Đ��{�^��
    if( Br_Record_GetTrgStart( p_wk, x, y ) )
    { 
      if( p_wk->p_param->mode == BR_RECODE_PROC_DOWNLOAD_RANK
          || p_wk->p_param->mode == BR_RECODE_PROC_DOWNLOAD_NUMBER )
      { 
        //GDS���[�h�Ȃ�΃f�[�^�_�E�����[�h���Ă���Đ��ɂ���
        BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_VideoDownloadRecPlay );
      }
      else
      { 
        //�u���E�U���[�h�Ȃ�Έ��S�����t���O�������Ă��Ȃ�������A
        //�Z�[�u�ɂ����Ă���Đ�
        if( p_wk->is_secure == FALSE )
        { 
          BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_RecPlayBeforeSave ); 
        }
        else
        { 
          p_wk->p_param->ret  = BR_RECORD_RETURN_BTLREC;
          BR_PROC_SYS_Interruput( p_wk->p_param->p_procsys );
          BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_FadeOut );
        }
      }
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���ʕ\���֐؂�ւ�
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Record_Seq_ChangeDrawUp( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHANGEOUT_START,
    SEQ_CHANGEOUT_WAIT,
    SEQ_CHANGEIN_START,
    SEQ_CHANGEIN_WAIT,
  };

 	BR_RECORD_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHANGEOUT_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_MAIN, BR_FADE_DIR_OUT );
    (*p_seq)++;
    break;
  case SEQ_CHANGEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      (*p_seq)++;
    }
    break;
  case SEQ_CHANGEIN_START:
    { 
      GFL_FONT *p_font;
      GFL_MSGDATA *p_msg;

      p_font  = BR_RES_GetFont( p_wk->p_param->p_res );
      p_msg   = BR_RES_GetMsgData( p_wk->p_param->p_res );
      //�ǂݍ��ݕς�
      if( p_wk->is_profile )
      {
        Br_Record_DeleteMainDisplay( p_wk, p_wk->p_param );
        BR_MSGWIN_PrintColor( p_wk->p_msgwin_s[BR_RECORD_MSGWINID_S_PROFILE], p_msg, msg_719, p_font, BR_PRINT_COL_NORMAL );
        Br_Record_CreateMainDisplayProfile( p_wk, p_wk->p_param );
      }
      else
      { 
        Br_Record_DeleteMainDisplayProfile( p_wk, p_wk->p_param );
        BR_MSGWIN_PrintColor( p_wk->p_msgwin_s[BR_RECORD_MSGWINID_S_PROFILE], p_msg, msg_718, p_font, BR_PRINT_COL_NORMAL );
        Br_Record_CreateMainDisplay( p_wk, p_wk->p_param );
      }
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_MAIN, BR_FADE_DIR_IN );
    }
    (*p_seq)++;
    break;
  case SEQ_CHANGEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_Main );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �r�f�I�i���o�[����v���t�@�C���ƃw�b�_���_�E�����[�h
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Record_Seq_NumberDownload( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,

    SEQ_DOWNLOAD_START,
    SEQ_DOWNLOAD_WAIT,
    SEQ_DOWNLOAD_END,

    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_FADECHANGE_START,
    SEQ_FADECHANGE_WAIT,
    SEQ_END,

    SEQ_MSG_WAIT,

    SEQ_FADEOUT_START_EXIT,
    SEQ_FADEOUT_WAIT_EXIT,
    SEQ_END_EXIT,
  };

  BR_RECORD_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    //�쐬
    Br_Record_Download_CreateDisplay( p_wk, p_wk->p_param );

    BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_info_023 );
    *p_seq  = SEQ_FADEIN_START;
    break;
  case SEQ_FADEIN_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    *p_seq  = SEQ_FADEIN_WAIT;
    break;
  case SEQ_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      *p_seq  = SEQ_DOWNLOAD_START;
    }
    break;
  case SEQ_DOWNLOAD_START:
    {
      BR_NET_REQUEST_PARAM  req_param;
      GFL_STD_MemClear( &req_param, sizeof(BR_NET_REQUEST_PARAM) );
      req_param.download_video_number = p_wk->p_param->video_number;
      BR_NET_StartRequest( p_wk->p_param->p_net, BR_NET_REQUEST_BATTLE_VIDEO_DOWNLOAD, &req_param );
      p_wk->cnt = 0;

      *p_seq  = SEQ_DOWNLOAD_WAIT;
    }
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
    { 
      BR_NET_ERR_RETURN err;
      int msg;
      BATTLE_REC_RECV *p_recv;
      int video_number;

      err = BR_NET_GetError( p_wk->p_param->p_net, &msg );


      if( err == BR_NET_ERR_RETURN_NONE )
      { 
        if( BR_NET_GetDownloadBattleVideo( p_wk->p_param->p_net, &p_recv, &video_number ) )
        { 
          p_wk->p_profile = &p_recv->profile;
          p_wk->p_header  = &p_recv->head;

          *p_seq  = SEQ_FADEOUT_START;
        }
        else
        { 
          BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_err_042 );
          *p_seq  = SEQ_MSG_WAIT;
        }
      }
      else
      {
        BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg );
        *p_seq  = SEQ_MSG_WAIT;
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

      BR_PROC_SYS_Pop( p_wk->p_param->p_procsys );
      *p_seq  = SEQ_FADEOUT_START_EXIT;
    }
    break;

  case SEQ_FADEOUT_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_FADEOUT_WAIT;
    break;
  case SEQ_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      Br_Record_Download_DeleteDisplay( p_wk, p_wk->p_param );
      Br_Record_CreateSubDisplay( p_wk, p_wk->p_param );
      Br_Record_CreateMainDisplay( p_wk, p_wk->p_param );
      *p_seq  = SEQ_FADECHANGE_START;
    }
    break;
  case SEQ_FADECHANGE_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    *p_seq  = SEQ_FADECHANGE_WAIT;
    break;
  case SEQ_FADECHANGE_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      *p_seq  = SEQ_END;
    }
    break;
  case SEQ_END:
    BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_Main );
    break;

  case SEQ_FADEOUT_START_EXIT:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_FADEOUT_WAIT_EXIT;
    break;
  case SEQ_FADEOUT_WAIT_EXIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      *p_seq  = SEQ_END_EXIT;
    }
    break;
  case SEQ_END_EXIT:
    Br_Record_Download_DeleteDisplay( p_wk, p_wk->p_param );

    BR_SEQ_End( p_seqwk );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �^��f�[�^�{�̂��_�E�����[�h�����Ę^��Đ��ւ���
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Record_Seq_VideoDownloadRecPlay( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHANGE_FADEOUT_START,
    SEQ_CHANGE_FADEOUT_WAIT,
    SEQ_CHANGE_MAIN,
    SEQ_CHANGE_FADEIN_START,
    SEQ_CHANGE_FADEIN_WAIT,

    SEQ_DOWNLOAD_START,
    SEQ_DOWNLOAD_WAIT,
    SEQ_DOWNLOAD_END,

    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_FADEOUT_END,

    SEQ_MSG_WAIT,

    SEQ_FADEOUT_START_RET,
    SEQ_FADEOUT_WAIT_RET,
    SEQ_FADEOUT_END_RET,

  };

  BR_RECORD_WORK	*p_wk	= p_wk_adrs;

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

    //�j��
    if( p_wk->is_profile )
    { 
      Br_Record_DeleteMainDisplayProfile( p_wk, p_wk->p_param );
    }
    else
    { 
      Br_Record_DeleteMainDisplay( p_wk,  p_wk->p_param );
    }
    Br_Record_DeleteSubDisplay( p_wk, p_wk->p_param );

    //�쐬
    Br_Record_Download_CreateDisplay( p_wk, p_wk->p_param );
    BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_info_023 );

    *p_seq  = SEQ_CHANGE_FADEIN_START;
    break;
  case SEQ_CHANGE_FADEIN_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    *p_seq  = SEQ_CHANGE_FADEIN_WAIT;
    break;
  case SEQ_CHANGE_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_DOWNLOAD_START;
    }
    break;

  case SEQ_DOWNLOAD_START:
    {
      BR_NET_REQUEST_PARAM  req_param;
      GFL_STD_MemClear( &req_param, sizeof(BR_NET_REQUEST_PARAM) );
      req_param.download_video_number = RecHeader_ParamGet( p_wk->p_header, RECHEAD_IDX_DATA_NUMBER, 0 );
      BR_NET_StartRequest( p_wk->p_param->p_net, BR_NET_REQUEST_BATTLE_VIDEO_DOWNLOAD, &req_param );
      p_wk->cnt = 0;

      *p_seq  = SEQ_DOWNLOAD_WAIT;
    }
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
    { 
      BR_NET_ERR_RETURN err;
      int msg;
      int video_number;
      BATTLE_REC_RECV       *p_btl_rec;

      err = BR_NET_GetError( p_wk->p_param->p_net, &msg );

      if( err == BR_NET_ERR_RETURN_NONE )
      { 
        if( BR_NET_GetDownloadBattleVideo( p_wk->p_param->p_net, &p_btl_rec, &video_number ) )
        {   
          //��M�����̂ŁA���R�[�h��ݒ�
          BattleRec_DataSet( &p_btl_rec->profile, &p_btl_rec->head,
              &p_btl_rec->rec, GAMEDATA_GetSaveControlWork( p_wk->p_param->p_gamedata ) );

          p_wk->p_param->p_record->video_number = video_number;

          p_wk->p_param->ret  = BR_RECORD_RETURN_BTLREC;
          BR_PROC_SYS_Interruput( p_wk->p_param->p_procsys );
          *p_seq  = SEQ_FADEOUT_START;
        }
        else
        { 
          //��M�Ɏ��s�����̂ŁA�߂�
          BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_err_042 );
          *p_seq  = SEQ_MSG_WAIT;
        }
      }
      else
      { 
          //��M�Ɏ��s�����̂ŁA�߂�
        BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg );
          *p_seq  = SEQ_MSG_WAIT;
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


      *p_seq  = SEQ_FADEOUT_START_RET;
    }
    break;

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
    //�j��
    if( !PMSND_CheckPlayingSEIdx( SEQ_SE_BREC_04 ) )
    { 
      Br_Record_Download_DeleteDisplay( p_wk, p_wk->p_param );
      BR_SEQ_End( p_wk->p_seq );
    }
    break;

  case SEQ_FADEOUT_START_RET:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_FADEOUT_WAIT_RET;
    break;
  case SEQ_FADEOUT_WAIT_RET:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      *p_seq  = SEQ_FADEOUT_END_RET;
    }
    break;
  case SEQ_FADEOUT_END_RET:
    //�j��

    Br_Record_Download_DeleteDisplay( p_wk, p_wk->p_param );


    BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_FadeInBefore );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���S�����ς݃t���O���Z�[�u
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Record_Seq_SecureSave( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_FADEIN_END,

    SEQ_SAVE_START,
    SEQ_SAVE_WAIT,

    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_FADEOUT_END,
  };

  BR_RECORD_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    Br_Record_Download_CreateDisplay( p_wk, p_wk->p_param );
    BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_info_030 );

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
    *p_seq  = SEQ_SAVE_START;
    break;

  case SEQ_SAVE_START:
    p_wk->sv_wk0  = 0;
    p_wk->sv_wk1  = 0;
    *p_seq  = SEQ_SAVE_WAIT;
    break;

  case SEQ_SAVE_WAIT:
    {
      SAVE_RESULT result;
      GF_ASSERT(p_wk->p_param->mode <= BR_RECODE_PROC_OTHER_02 );
      result  = BattleRec_SecureSetSave( p_wk->p_param->p_gamedata, p_wk->p_param->mode, &p_wk->sv_wk0, &p_wk->sv_wk1, p_wk->heapID );
      if( result == SAVE_RESULT_OK )
      { 
        //�Z�[�u�̒��ňÍ������邽�߁A���������Ĉ��������g��
        BattleRec_DataDecoded();

        //�Z�L���A�Z�[�u�������Ƃ͍ēx�Z�L���A�`�F�b�N
        p_wk->is_secure = RecHeader_ParamGet( p_wk->p_header, RECHEAD_IDX_SECURE, 0 );
        *p_seq  = SEQ_FADEOUT_START;
      }
    }
    break;

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
    Br_Record_Download_DeleteDisplay( p_wk, p_wk->p_param );
    BR_SEQ_SetNext( p_seqwk, Br_Record_Seq_FadeInBefore );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����O�̈��S�Z�[�u
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_Record_Seq_RecPlayBeforeSave( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    //�͂��A������
    SEQ_CHANGE_FADEOUT_START,
    SEQ_CHANGE_FADEOUT_WAIT,
    SEQ_CHANGE_MAIN,

    SEQ_FADEIN_START_SEL,
    SEQ_FADEIN_WAIT_SEL,
    SEQ_FADEIN_END_SEL,

    SEQ_SELECT,

    SEQ_FADEOUT_START_SEL,
    SEQ_FADEOUT_WAIT_SEL,
    SEQ_FADEOUT_END_SEL,

    //�I��������ǂ�
    SEQ_FADEOUT_START_RET,
    SEQ_FADEOUT_WAIT_RET,
    SEQ_FADEOUT_END_RET,

    //�Z�[�u
    SEQ_CHANGE_SAVE,
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_FADEIN_END,

    SEQ_SAVE_START,
    SEQ_SAVE_WAIT,

    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_FADEOUT_END,
  };

  BR_RECORD_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
  { 
    //-------------------------------------
    ///	�͂��A�������I��
    //=====================================
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
    //�j��
    if( p_wk->is_profile )
    { 
      Br_Record_DeleteMainDisplayProfile( p_wk, p_wk->p_param );
    }
    else
    { 
      Br_Record_DeleteMainDisplay( p_wk,  p_wk->p_param );
    }
    Br_Record_DeleteSubDisplay( p_wk, p_wk->p_param );

    Br_Record_Save_CreateDisplay( p_wk, p_wk->p_param );

    *p_seq  = SEQ_FADEIN_START_SEL;
    break;


  case SEQ_FADEIN_START_SEL:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    (*p_seq)++;
    break;
  case SEQ_FADEIN_WAIT_SEL:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      (*p_seq)++;
    }
    break;
  case SEQ_FADEIN_END_SEL:
      (*p_seq)++;
    break;

  case SEQ_SELECT:
    { 
      u32 x, y;
      if( GFL_UI_TP_GetPointTrg( &x, &y ) )
      { 
        if( Br_Record_GetTrgYes( p_wk, x, y ) )
        { 
          *p_seq  = SEQ_FADEOUT_START_SEL;
          break;
        }

        if( Br_Record_GetTrgNo( p_wk, x, y ) )
        { 
          *p_seq  = SEQ_FADEOUT_START_RET;
          break;
        }
      }
    }
    break;

  case SEQ_FADEOUT_START_SEL:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    (*p_seq)++;
    break;
  case SEQ_FADEOUT_WAIT_SEL:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      (*p_seq)++;
    }
    break;
  case SEQ_FADEOUT_END_SEL:
    Br_Record_Save_DeleteDisplay( p_wk, p_wk->p_param );
    *p_seq  = SEQ_CHANGE_SAVE;
    break;

    //-------------------------------------
    ///	�߂�
    //=====================================
  case SEQ_FADEOUT_START_RET:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    (*p_seq)++;
    break;
  case SEQ_FADEOUT_WAIT_RET:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      (*p_seq)++;
    }
    break;
  case SEQ_FADEOUT_END_RET:

    Br_Record_Save_DeleteDisplay( p_wk, p_wk->p_param );
    BR_SEQ_SetNext( p_seqwk,Br_Record_Seq_FadeInBefore );
    break;

    //-------------------------------------
    ///	�Z�[�u
    //=====================================
  case SEQ_CHANGE_SAVE:
    Br_Record_Download_CreateDisplay( p_wk, p_wk->p_param );
    *p_seq  = SEQ_FADEIN_START;
    break;
  case SEQ_FADEIN_START:
    BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_info_030 );

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
    *p_seq  = SEQ_SAVE_START;
    break;

  case SEQ_SAVE_START:

    GAMEDATA_SaveAsyncStart( p_wk->p_param->p_gamedata );
    *p_seq  = SEQ_SAVE_WAIT;
    break;

  case SEQ_SAVE_WAIT:
    {
      SAVE_RESULT result;
      result  = GAMEDATA_SaveAsyncMain( p_wk->p_param->p_gamedata );
      if( result == SAVE_RESULT_OK )
      { 
        BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_MAIN ], BR_BALLEFF_MOVE_NOP, NULL );
        *p_seq  = SEQ_FADEOUT_START;
      }
    }
    break;

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
    if( !PMSND_CheckPlayingSEIdx( SEQ_SE_BREC_04 ) )
    { 
      Br_Record_Download_DeleteDisplay( p_wk, p_wk->p_param );

      p_wk->p_param->ret  = BR_RECORD_RETURN_BTLREC;
      BR_PROC_SYS_Interruput( p_wk->p_param->p_procsys );
      BR_SEQ_End( p_seqwk );
    }
    break;
  }
}

//=============================================================================
/**
 *			�`��
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�V���O����p�^��w�b�_��ʍ\�z
 *
 *	@param	BR_RECORD_WORK * p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Br_Record_CreateMainDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM *p_param )
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
      2,
      21,
      28,
      2,
      msg_12,
    },
    { 
      3,
      11,
      7,
      2,
      msg_11,
    },
    { 
      3,
      13,
      7,
      2,
      msg_11_2,
    },
  };

  //�쐬����Ă���΍��Ȃ�
  if( p_wk->is_create[BR_RECORD_CREATE_ID_MAIN_DISP_M] )
  {
    return;
  }
  p_wk->is_create[BR_RECORD_CREATE_ID_MAIN_DISP_M]  = TRUE;

  if( Br_Record_Check2vs2( p_wk->p_header ) )
  { 
    BR_RES_LoadBG( p_param->p_res, BR_RES_BG_RECORD_M_BTL_DOUBLE, p_wk->heapID );
  }
  else
  { 
    BR_RES_LoadBG( p_param->p_res, BR_RES_BG_RECORD_M_BTL_SINGLE, p_wk->heapID );
  }
  
  
  //���b�Z�[�WWIN�쐬
  {
    int i;
    GFL_FONT *p_font;
    GFL_MSGDATA *p_msg;
    WORDSET *p_word;
    STRBUF  *p_strbuf;
    STRBUF  *p_src;

    BOOL is_print ;

    p_font  = BR_RES_GetFont( p_param->p_res );
    p_msg   = BR_RES_GetMsgData( p_param->p_res );
    p_word  = BR_RES_GetWordSet( p_param->p_res );

    for( i = 0; i < BR_RECORD_MSGWINID_M_BTL_MAX; i++ )
    { 
      p_wk->p_msgwin_m[i]  = BR_MSGWIN_Init( BG_FRAME_M_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_M_FONT, p_wk->p_que, p_wk->heapID );

      switch( i )
      { 
      case BR_RECORD_MSGWINID_M_BTL_NAME:  //�������̋L�^
        { 
          STRBUF  *p_name;
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          p_name    = GDS_Profile_CreateNameString( p_wk->p_profile, p_wk->heapID );
          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterWord( p_word, 0, p_name, GDS_Profile_GetSex(p_wk->p_profile), TRUE, PM_LANG );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_name );
          GFL_STR_DeleteBuffer( p_src );
          BR_MSGWIN_SetPos(p_wk->p_msgwin_m[i], 0, 0, BR_MSGWIN_POS_ABSOLUTE );
          is_print  = TRUE;
        }
        break;
      case BR_RECORD_MSGWINID_M_BTL_RULE:       //�R���V�A���@�V���O���@��������Ȃ�
        {
          const u32 btl_rule  = RecHeader_ParamGet( p_wk->p_header, RECHEAD_IDX_MODE, 0);
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID + btl_rule );
          BR_MSGWIN_SetPos(p_wk->p_msgwin_m[i], 0, 0, BR_MSGWIN_POS_ABSOLUTE );
          is_print  = TRUE;
        }
        break;
      case BR_RECORD_MSGWINID_M_BTL_NUMBER:    //�r�f�I�i���o�[
        { 
          u64 number  = RecHeader_ParamGet( p_wk->p_header, RECHEAD_IDX_DATA_NUMBER, 0);
          u32 dtmp2[3];
          STRBUF  *p_src;
          
          //�i���o�[���R�u���b�N��
          BR_TOOL_GetVideoNumberToBlock( number, dtmp2, 3 );

          { 
            STRBUF  *p_intro;
            PMS_DATA  pms_data;

            p_intro  = GDS_Profile_GetSelfIntroduction( p_wk->p_profile, &pms_data, p_wk->heapID);

            if( p_intro != NULL )
            {
              p_src = GFL_MSG_CreateString( p_msg,  msg_12_2 );  //��
              OS_Printf( "����ȃf�[�^�i���o�[\n");

              GFL_STR_DeleteBuffer( p_intro );
            }
            else
            { 
              p_src  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
            }
          }
          p_strbuf     = GFL_STR_CreateBuffer( 128, p_wk->heapID );
          WORDSET_RegisterNumber( p_word, 2, dtmp2[0], 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
          WORDSET_RegisterNumber( p_word, 1, dtmp2[1], 5, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
          WORDSET_RegisterNumber( p_word, 0, dtmp2[2], 2, STR_NUM_DISP_ZERO, STR_NUM_CODE_DEFAULT );
          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          BR_MSGWIN_SetPos(p_wk->p_msgwin_m[i], 0, 0, BR_MSGWIN_POS_ABSOLUTE ); //GMM�ŃZ���^�����O����Ă���
          GFL_STR_DeleteBuffer( p_src );
          is_print  = TRUE;
        }
        break;
      case BR_RECORD_MSGWINID_M_BTL_WIN_NUM:
        if( Br_Record_CheckCounterVisible(p_wk->p_header) )
        { 
          u32 win = RecHeader_ParamGet( p_wk->p_header, RECHEAD_IDX_COUNTER, 0 );
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );

          p_src     = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          WORDSET_RegisterNumber( p_word, 0, win, 4, STR_NUM_DISP_LEFT, STR_NUM_DISP_LEFT );

          WORDSET_ExpandStr( p_word, p_strbuf, p_src );
          GFL_STR_DeleteBuffer( p_src );
          BR_MSGWIN_SetPos(p_wk->p_msgwin_m[i], 0, 0, BR_MSGWIN_POS_WH_CENTER );
          is_print  = TRUE;
        }
        else
        { 
          is_print  = FALSE;
        }
        break;

      case BR_RECORD_MSGWINID_M_BTL_WIN:  //���A��
        if( Br_Record_CheckCounterVisible(p_wk->p_header) )
        { 
          p_strbuf  = GFL_MSG_CreateString( p_msg, sc_msgwin_data[i].msgID );
          BR_MSGWIN_SetPos(p_wk->p_msgwin_m[i], 0, 0, BR_MSGWIN_POS_WH_CENTER );
          is_print  = TRUE;
        }
        else
        { 
          is_print  = FALSE;
        }
        break;

      }

      if( is_print )
      { 
        BR_MSGWIN_PrintBufColor( p_wk->p_msgwin_m[i], p_strbuf, p_font, BR_PRINT_COL_NORMAL );
        GFL_STR_DeleteBuffer( p_strbuf ); 
      }
    }
  }

  //�|�P�A�C�R��
  { 
    Br_Record_AddPokeIcon( p_wk, p_param->p_unit, p_wk->p_header, p_wk->heapID );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �^��w�b�_��ʂɃ|�P�A�C�R��������
 *	        ���̃\�[�X��GS����ڐA
 *
 *	@param	BR_RECORD_WORK * p_wk ���[�N
 *	@param  p_param	              ����
 */
//-----------------------------------------------------------------------------
static void Br_Record_AddPokeIcon( BR_RECORD_WORK * p_wk, GFL_CLUNIT *p_clunit, BATTLE_REC_HEADER_PTR p_header, HEAPID heapID )
{	
	int i;
	int monsno;
	int form;
	int gender;
	int no = 0;
	
	int gra_id;
	int monsno_tbl[ HEADER_MONSNO_MAX ];
	int form_tbl[ HEADER_MONSNO_MAX ];
	int gender_tbl[ HEADER_MONSNO_MAX ];
	int type;
  ARCHANDLE *p_handle;
  GFL_CLWK_DATA cldata;
	
	
	static const s16 pos_tbl[][ HEADER_MONSNO_MAX ][ 2 ] = 
  {
		{
#ifdef BUGFIX_GFBTS1974_20100713
			{  3*8, 19*8 },{  6*8, 19*8 },{  9*8, 19*8 },{ 12*8, 19*8 },{ 15*8, 19*8 },{ 18*8, 19*8 },  //��������
			{ 11*8, 13*8 },{ 14*8, 13*8 },{ 17*8, 13*8 },{ 20*8, 13*8 },{ 23*8, 13*8 },{ 26*8, 13*8 },  //���肪��
#else
			{ 11*8, 13*8 },{ 14*8, 13*8 },{ 17*8, 13*8 },{ 20*8, 13*8 },{ 23*8, 13*8 },{ 26*8, 13*8 },
			{  3*8, 19*8 },{  6*8, 19*8 },{  9*8, 19*8 },{ 12*8, 19*8 },{ 15*8, 19*8 },{ 18*8, 19*8 },
#endif //BUGFIX_GFBTS1974_20100713
		},
		{
      //BTL_CLIENT_ID�̏���
			{  3*8, 17*8 },{  6*8, 17*8 },{  9*8, 17*8 },	///< ����
			{ 18*8, 17*8 },{ 21*8, 17*8 },{ 24*8, 17*8 },	///< ����P
			{  5*8, 20*8 },{  8*8, 20*8 },{ 11*8, 20*8 },	///< �p�[�g�i�[
			{ 20*8, 20*8 },{ 23*8, 20*8 },{ 26*8, 20*8 },	///< ����Q
		},
	};
  p_handle  = GFL_ARC_OpenDataHandle( ARCID_POKEICON, GFL_HEAP_LOWID(heapID) );


  //�p���b�g�A�Z��
  p_wk->res_icon_plt  = GFL_CLGRP_PLTT_RegisterComp( p_handle, 
      POKEICON_GetPalArcIndex(),
      CLSYS_DRAW_MAIN, PLT_OBJ_M_POKEICON*0x20, heapID );
  p_wk->res_icon_cel = GFL_CLGRP_CELLANIM_Register( p_handle,
      POKEICON_GetCellArcIndex(), POKEICON_GetAnmArcIndex(), heapID );

	
	///< �e�[�u���̍쐬
	{	
		int temp_no = 0;
		int temp_monsno = 0;
		int temp_form = 0;
		int temp_gender = 0;
		int add = 6;
		int si = 0;
		int ei = add;
		
		type = 0;

		///< 2vs2�̏ꍇ
		if ( Br_Record_Check2vs2( p_header ) == TRUE ){
			add = 3;
			ei = 3;
			type = 1;
		}
		do {
			for ( i = si; i < ei; i++ ){
				monsno_tbl[ temp_no ] = 0xFF;
				
				temp_monsno = RecHeader_ParamGet( p_header, RECHEAD_IDX_MONSNO, i );
				temp_form	= RecHeader_ParamGet( p_header, RECHEAD_IDX_FORM_NO, i );
				temp_gender	= RecHeader_ParamGet( p_header, RECHEAD_IDX_GENDER, i );

				if ( temp_monsno == 0 ){ continue; }
				
				monsno_tbl[ temp_no ] = temp_monsno;
				form_tbl[ temp_no]	  = temp_form;
				gender_tbl[ temp_no]	  = temp_gender;
				
				temp_no++;
			}
			for ( i = temp_no; i < ei; i++ ){	
				monsno_tbl[ temp_no ] = 0;
				form_tbl[ temp_no ]	  = 0;
				gender_tbl[ temp_no ]	  = 0;
				temp_no++;
			}		
			si += add;
			ei += add;
			
		} while ( ei <= HEADER_MONSNO_MAX );
	}
	
	for ( i = 0; i < HEADER_MONSNO_MAX; i++ ){
				
		p_wk->p_icon[ i ]     = NULL;
    p_wk->res_icon_chr[i] = GFL_CLGRP_REGISTER_FAILED;
		
		monsno = monsno_tbl[ i ];
		
		if ( monsno == 0 ){ continue; }		///< ���܂�����
		
		form = form_tbl[ i ];
		gender = gender_tbl[ i ];

    p_wk->res_icon_chr[i] = GFL_CLGRP_CGR_Register( p_handle,
              POKEICON_GetCgxArcIndexByMonsNumber( monsno, form, gender, 0 ),
              FALSE, CLSYS_DRAW_MAIN, heapID );
  
    GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

		cldata.pos_x		= pos_tbl[ type ][ i ][ 0 ] + 12 - 4;
		cldata.pos_y		= pos_tbl[ type ][ i ][ 1 ] - 12;
		cldata.anmseq 	= POKEICON_ANM_HPMAX;
    cldata.softpri  = HEADER_MONSNO_MAX-no;//�E�ɍs���قǗD��x�������Ȃ�
				
		p_wk->p_icon[ no ] = GFL_CLACT_WK_Create( p_clunit,
          p_wk->res_icon_chr[i],p_wk->res_icon_plt,p_wk->res_icon_cel,
            &cldata, CLSYS_DEFREND_MAIN, heapID );

    GFL_CLACT_WK_SetObjMode( p_wk->p_icon[ no ], GX_OAM_MODE_XLU );
		
    GFL_CLACT_WK_SetPlttOffs( p_wk->p_icon[ no ], POKEICON_GetPalNum( monsno, form, gender, 0 ),
        CLWK_PLTTOFFS_MODE_OAM_COLOR );

		
		OS_Printf( "no = %2d\n", no );
		no++;	
	}

  GFL_ARC_CloseDataHandle( p_handle );
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���t�B�[����ʍ\�z
 *
 *	@param	BR_RECORD_WORK * p_wk ���[�N
 *	@param	p_param								���R�[�h�̈���
 */
//-----------------------------------------------------------------------------
static void Br_Record_CreateMainDisplayProfile( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param )
{
  if( p_wk->p_profile_disp == NULL )
  { 
    //�쐬����Ă���΍��Ȃ�
    if( p_wk->is_create[BR_RECORD_CREATE_ID_PROFILE] )
    {
      return;
    }
    p_wk->is_create[BR_RECORD_CREATE_ID_PROFILE]  = TRUE;

    {
      BR_PROFILE_TYPE type;

      if( p_wk->p_param->mode == BR_RECODE_PROC_MY )
      {
        type  = BR_PROFILE_TYPE_MY;
      }
      else
      {
        type  = BR_PROFILE_TYPE_OTHER;
      }

      p_wk->p_profile_disp  = BR_PROFILE_CreateMainDisplay( p_wk->p_profile, p_param->p_res, p_param->p_unit, p_wk->p_que, type, p_wk->heapID );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief	�v���t�B�[����ʔj��
 *
 *	@param	BR_RECORD_WORK * p_wk ���[�N
 *	@param	p_param								���R�[�h�̈���
 */
//-----------------------------------------------------------------------------
static void Br_Record_DeleteMainDisplayProfile( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param )
{ 
  //�쐬����Ă��Ȃ���Ή�����Ȃ�
  if( !p_wk->is_create[BR_RECORD_CREATE_ID_PROFILE] )
  {
    return;
  }

  if( p_wk->p_profile_disp )
  { 
    BR_PROFILE_DeleteMainDisplay( p_wk->p_profile_disp );
    p_wk->p_profile_disp  = NULL;

    p_wk->is_create[BR_RECORD_CREATE_ID_PROFILE]  = FALSE;
  }
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

  //�쐬����Ă��Ȃ���Ή�����Ȃ�
  if( !p_wk->is_create[BR_RECORD_CREATE_ID_MAIN_DISP_M] )
  {
    return;
  }

  //OBJ�j��
  { 
    int i;
    for( i = 0; i < HEADER_MONSNO_MAX; i++ )
    { 
      if( p_wk->p_icon[i] )
      { 
        GFL_CLACT_WK_SetDrawEnable( p_wk->p_icon[i], FALSE );
        GFL_CLACT_WK_Remove( p_wk->p_icon[i] );
        p_wk->p_icon[i] = NULL;
      }
      if( p_wk->res_icon_chr[i] != GFL_CLGRP_REGISTER_FAILED )
      { 
        GFL_CLGRP_CGR_Release(p_wk->res_icon_chr[i]);
        p_wk->res_icon_chr[i] = GFL_CLGRP_REGISTER_FAILED;
      }
    }
    if( p_wk->res_icon_plt != GFL_CLGRP_REGISTER_FAILED )
    { 
      GFL_CLGRP_PLTT_Release(p_wk->res_icon_plt);
      p_wk->res_icon_plt  = GFL_CLGRP_REGISTER_FAILED;
    }
    if( p_wk->res_icon_cel != GFL_CLGRP_REGISTER_FAILED )
    { 
      GFL_CLGRP_CELLANIM_Release(p_wk->res_icon_cel);
      p_wk->res_icon_cel  = GFL_CLGRP_REGISTER_FAILED;
    }

  }

  //BG�j��
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

  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_RECORD_M_BTL_SINGLE );

  GFL_BG_LoadScreenReq( BG_FRAME_M_FONT );

  p_wk->is_create[BR_RECORD_CREATE_ID_MAIN_DISP_M]  = FALSE;
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
      8,
      3,
      16,
      2,
      msg_718
    }
  };
  GFL_FONT *p_font;
  GFL_MSGDATA *p_msg;

  //�쐬����Ă���΍��Ȃ�
  if( p_wk->is_create[BR_RECORD_CREATE_ID_MAIN_DISP_S] )
  {
    return;
  }
  p_wk->is_create[BR_RECORD_CREATE_ID_MAIN_DISP_S]  = TRUE;

	BR_RES_LoadBG( p_param->p_res, BR_RES_BG_RECORD_S, p_wk->heapID );
	BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, p_wk->heapID );


  p_font  = BR_RES_GetFont( p_param->p_res );
  p_msg   = BR_RES_GetMsgData( p_param->p_res );


  //�{�^���쐬  
  if( p_wk->can_save )
  { 
    { 
      GFL_CLWK_DATA cldata;
      BR_RES_OBJ_DATA res;

      GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

      cldata.pos_x    = 24;
      cldata.pos_y    = 168;
      cldata.anmseq   = 0;
      cldata.softpri  = 1;

      BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, &res );

      p_wk->p_btn[BR_RECORD_MAIN_BTNID_RETURN] = BR_BTN_Init( &cldata, msg_05, BR_BTN_DATA_SHORT_WIDTH,CLSYS_DRAW_SUB, p_param->p_unit, p_wk->p_bmpoam, p_font, p_msg, &res, p_wk->heapID );

      cldata.pos_x    = 136;
      cldata.anmseq   = 3;
      p_wk->p_btn[BR_RECORD_MAIN_BTNID_SAVE] = BR_BTN_Init( &cldata, msg_602, BR_BTN_DATA_SHORT_WIDTH,CLSYS_DRAW_SUB, p_param->p_unit, p_wk->p_bmpoam, p_font, p_msg, &res, p_wk->heapID );
    }
  }
  else
  { 
    { 
      GFL_CLWK_DATA cldata;
      BR_RES_OBJ_DATA res;

      GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

      cldata.pos_x    = 80;
      cldata.pos_y    = 168;
      cldata.anmseq   = 0;
      cldata.softpri  = 1;

      BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S, &res );

      p_wk->p_btn[BR_RECORD_MAIN_BTNID_RETURN] = BR_BTN_Init( &cldata, msg_05, BR_BTN_DATA_SHORT_WIDTH,CLSYS_DRAW_SUB, p_param->p_unit, p_wk->p_bmpoam, p_font, p_msg, &res, p_wk->heapID );
    }
  }

  //���b�Z�[�WWIN�쐬
  {
    int i = BR_RECORD_MSGWINID_S_PROFILE;
    p_wk->p_msgwin_s[i]  = BR_MSGWIN_Init( BG_FRAME_S_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_S_FONT, p_wk->p_que, p_wk->heapID );

    if( p_wk->is_profile )
    {
      BR_MSGWIN_PrintColor( p_wk->p_msgwin_s[BR_RECORD_MSGWINID_S_PROFILE], p_msg, msg_719, p_font, BR_PRINT_COL_NORMAL );
    }
    else
    {
      BR_MSGWIN_PrintColor( p_wk->p_msgwin_s[BR_RECORD_MSGWINID_S_PROFILE], p_msg, msg_718, p_font, BR_PRINT_COL_NORMAL );
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

  //�쐬����Ă��Ȃ���Ή�����Ȃ�
  if( !p_wk->is_create[BR_RECORD_CREATE_ID_MAIN_DISP_S] )
  {
    return;
  }
  {
    int i = BR_RECORD_MSGWINID_S_PROFILE;
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
    for( i = BR_RECORD_MAIN_BTNID_START; i < BR_RECORD_MAIN_BTNID_MAX; i++ )
    { 
      if( p_wk->p_btn[i] )
      { 
        BR_BTN_Exit( p_wk->p_btn[i] );
        p_wk->p_btn[i]  = NULL;
      }
    }
  }
  GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );

  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_SHORT_BTN_S ); 
	BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_RECORD_S );


  p_wk->is_create[BR_RECORD_CREATE_ID_MAIN_DISP_S]  = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �_�E�����[�h�p�̊G���쐬
 *
 *	@param	BR_RECORD_WORK * p_wk ���[�N
 *	@param	                      ����
 */
//-----------------------------------------------------------------------------
static void Br_Record_Download_CreateDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param )
{ 
  //�쐬����Ă���΍��Ȃ�
  if( p_wk->is_create[BR_RECORD_CREATE_ID_DOWNLOAD] )
  {
    return;
  }
  p_wk->is_create[BR_RECORD_CREATE_ID_DOWNLOAD]  = TRUE;

  //�_�E�����[�h�p�̊G���쐬
  if( p_wk->p_text == NULL )
  { 
    p_wk->p_text    = BR_TEXT_Init( p_wk->p_param->p_res, p_wk->p_que, p_wk->heapID );
  }
  {
    GFL_POINT pos;
    pos.x = 256/2;
    pos.y = 192/2;
    BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_MAIN ], BR_BALLEFF_MOVE_BIG_CIRCLE, &pos );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �_�E�����[�h�p�̊G��j��
 *
 *	@param	BR_RECORD_WORK * p_wk ���[�N
 *	@param	                      ����
 */
//-----------------------------------------------------------------------------
static void Br_Record_Download_DeleteDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM	*p_param )
{ 
  //�쐬����Ă��Ȃ���Ή�����Ȃ�
  if( !p_wk->is_create[BR_RECORD_CREATE_ID_DOWNLOAD] )
  {
    return;
  }

  BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_MAIN ], BR_BALLEFF_MOVE_NOP, NULL );
  if( p_wk->p_text )
  { 
    BR_TEXT_Exit( p_wk->p_text, p_wk->p_param->p_res );
    p_wk->p_text  = NULL;
  }

  p_wk->is_create[BR_RECORD_CREATE_ID_DOWNLOAD]  = FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u��ʂ��쐬
 *
 *	@param	BR_RECORD_WORK * p_wk ���[�N
 *	@param	                      ����
 */
//-----------------------------------------------------------------------------
static void Br_Record_Save_CreateDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM *p_param )
{ 
  //�쐬����Ă���΍��Ȃ�
  if( p_wk->is_create[BR_RECORD_CREATE_ID_SAVE] )
  {
    return;
  }
  p_wk->is_create[BR_RECORD_CREATE_ID_SAVE]  = TRUE;


  if( p_wk->p_text == NULL )
  { 
    p_wk->p_text  = BR_TEXT_Init( p_param->p_res, p_wk->p_que, p_wk->heapID );
  }
  BR_TEXT_Print( p_wk->p_text, p_param->p_res, msg_info_027 );

  //����ʂ͐�pBG
  BR_RES_LoadBG( p_param->p_res, BR_RES_BG_BVDELETE_S, p_wk->heapID );

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
        4,
        6,
        9,
        2,
        msg_1000,
      },
      { 
        18,
        6,
        9,
        2,
        msg_1001,
      },
    };
    int i;

    GFL_FONT    *p_font  = BR_RES_GetFont( p_param->p_res );
    GFL_MSGDATA *p_msg   = BR_RES_GetMsgData( p_param->p_res );
    for( i = 0; i < BR_RECORD_MSGWINID_S_MAX; i++ )
    { 
      p_wk->p_msgwin_s[i]  = BR_MSGWIN_Init( BG_FRAME_S_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_S_FONT, p_wk->p_que, p_wk->heapID );
      BR_MSGWIN_PrintColor( p_wk->p_msgwin_s[i], p_msg, sc_msgwin_data[i].msgID, p_font, BR_PRINT_COL_NORMAL );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �Z�[�u
 *
 *	@param	BR_RECORD_WORK * p_wk ���[�N
 *	@param	                      ����
 */
//-----------------------------------------------------------------------------
static void Br_Record_Save_DeleteDisplay( BR_RECORD_WORK * p_wk, BR_RECORD_PROC_PARAM *p_param )
{ 

  //�쐬����Ă��Ȃ���Ή�����Ȃ�
  if( !p_wk->is_create[BR_RECORD_CREATE_ID_SAVE] )
  {
    return;
  }

  //���b�Z�[�W�E�B���h�E
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
  GFL_BG_LoadScreenReq( BG_FRAME_S_FONT );

  //����ʔj��
  BR_RES_UnLoadBG( p_param->p_res, BR_RES_BG_BVDELETE_S );

  //���ʔj��
  if( p_wk->p_text )
  { 
    BR_TEXT_Exit( p_wk->p_text, p_param->p_res );
    p_wk->p_text  = NULL;
  }

  p_wk->is_create[BR_RECORD_CREATE_ID_SAVE]  = FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �Q���Q���ǂ������`�F�b�N����
 *
 *	@param	BATTLE_REC_HEADER_PTR p_head  �w�b�_ 
 *
 *	@return TRUE�Ȃ�΂Q���Q�p�^��w�b�_
 */
//-----------------------------------------------------------------------------
static BOOL Br_Record_Check2vs2( BATTLE_REC_HEADER_PTR p_head )
{
	int	type;
	BOOL b2vs2 = FALSE; 
	
	type = RecHeader_ParamGet( p_head, RECHEAD_IDX_MODE, 0 );
	
	switch ( type ){
  case BATTLE_MODE_COLOSSEUM_MULTI_FREE:           //�R���V�A���@�}���`�@��������
  case BATTLE_MODE_COLOSSEUM_MULTI_50:             //�R���V�A���@�}���`�@�t���b�g
  case BATTLE_MODE_COLOSSEUM_MULTI_FREE_SHOOTER:   //�R���V�A���@�}���`�@���������@�V���[�^�[����
  case BATTLE_MODE_COLOSSEUM_MULTI_50_SHOOTER:     //�R���V�A���@�}���`�@�t���b�g�@�V���[�^�[����
  case BATTLE_MODE_SUBWAY_MULTI:
		b2vs2 = TRUE;
		break;
	default:
		b2vs2 = FALSE;
		break;
	};
	
	return b2vs2;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �u��ځv��\��������ӏ����ǂ���
 *
 *	@param	BATTLE_REC_HEADER_PTR p_head  �w�b�_
 *
 *	@return TRUE�Ȃ�΃o�g���T�u�E�F�C  FALSE�Ȃ�ΈႤ
 */
//-----------------------------------------------------------------------------
static BOOL Br_Record_CheckCounterVisible( BATTLE_REC_HEADER_PTR p_head )
{ 
	int	type;
	
	type = RecHeader_ParamGet( p_head, RECHEAD_IDX_MODE, 0 );
	
	switch ( type ){
  //����u��ځv��\��������̂͒n���S�̂�
  case BATTLE_MODE_SUBWAY_SINGLE:     //WIFI DL�܂�      �n���S�@�V���O��
  case BATTLE_MODE_SUBWAY_DOUBLE:                      //�n���S�@�_�u��
  case BATTLE_MODE_SUBWAY_MULTI:      //NPC, COMM, WIFI  �n���S�@�}���`
    return TRUE;

	default:
    return FALSE;
	};
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
  BOOL ret;

	rect.left		= (8)*8;
	rect.right	= (8 + 17)*8;
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
 *	@brief  �^�b�`�X�^�[�g
 *
 *	@param  x�^�b�`���W
 *	@param  y�^�b�`���W
 *
 *	@return TRUE�ŉ�����  FALSE�ŉ����Ă��Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Br_Record_GetTrgStart( BR_RECORD_WORK * p_wk, u32 x, u32 y )
{ 
	GFL_RECT rect;
  BOOL ret;

	rect.left		= (10)*8;
	rect.right	= (10 + 12)*8;
	rect.top		= (7)*8;
	rect.bottom	= (7 + 8)*8;

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
 *	@brief  �͂���I��
 *
 *	@param	x     X���W
 *	@param	y     Y���W
 *
 *	@return TRUE����  FALSE�������Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Br_Record_GetTrgYes( BR_RECORD_WORK *p_wk, u32 x, u32 y )
{ 
	GFL_RECT rect;
  BOOL ret;

	rect.left		= (4)*8;
	rect.right	= (4 + 9)*8;
	rect.top		= (6)*8;
	rect.bottom	= (6 + 2)*8;

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
 *	@brief  ��������I��
 *
 *	@param	x     X���W
 *	@param	y     Y���W
 *
 *	@return TRUE����  FALSE�������Ȃ�
 */
//-----------------------------------------------------------------------------
static BOOL Br_Record_GetTrgNo( BR_RECORD_WORK *p_wk, u32 x, u32 y )
{ 
	GFL_RECT rect;
  BOOL ret;

	rect.left		= (18)*8;
	rect.right	= (18 + 9)*8;
	rect.top		= (6)*8;
	rect.bottom	= (6 + 2)*8;


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
