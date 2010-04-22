//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		br_bvsave_proc.c
 *	@brief	�o�g���r�f�I�ۑ��v���Z�X
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

//�������W���[��
#include "br_util.h"
#include "br_snd.h"
#include "br_btn.h"
#include "br_res.h"

//�O���Q��
#include "br_bvsave_proc.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	MSGWIN��
//=====================================
enum
{
  BR_BVSAVE_MSGWINID_S_YES,
  BR_BVSAVE_MSGWINID_S_NO,
  BR_BVSAVE_MSGWINID_S_MAX,
};

//-------------------------------------
///	�{�^���C���f�b�N�X
//=====================================
typedef enum
{
	BR_BVSAVE_SAVE_BTNID_START,
	BR_BVSAVE_SAVE_BTNID_SAVE_01 = BR_BVSAVE_SAVE_BTNID_START,
	BR_BVSAVE_SAVE_BTNID_SAVE_02,
	BR_BVSAVE_SAVE_BTNID_SAVE_03,
	BR_BVSAVE_SAVE_BTNID_RETURN,
	BR_BVSAVE_SAVE_BTNID_MAX,

	BR_BVSAVE_BTNID_MAX,
} BR_BVSAVE_MAIN_BTNID;


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
  BMPOAM_SYS_PTR		    p_bmpoam;	//BMPOAM�V�X�e��
  PRINT_QUE             *p_que;   //�v�����g�L���[
  BR_TEXT_WORK          *p_text;  //�e�L�X�g
  BR_SEQ_WORK           *p_seq;
  u32                   save_btn_idx;

  BR_BTN_WORK	          *p_btn[ BR_BVSAVE_BTNID_MAX ];
  BR_MSGWIN_WORK        *p_msgwin_s[ BR_BVSAVE_MSGWINID_S_MAX ];
  BR_BALLEFF_WORK       *p_balleff[ CLSYS_DRAW_MAX ];
	HEAPID                heapID;
  BR_BVSAVE_PROC_PARAM	*p_param;

  u16                   work0;
  u16                   work1;
  u32                   cnt;
} BR_BVSAVE_WORK;


//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	BR�ۑ��v���Z�X
//=====================================
static GFL_PROC_RESULT BR_BVSAVE_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_BVSAVE_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT BR_BVSAVE_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
///	�V�[�P���X
//=====================================
static void Br_BvSave_Seq_VideoDownloadSave( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BvSave_Seq_SaveSelect( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BvSave_Seq_OverWrite( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void Br_BvSave_Seq_Save( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	��ʍ쐬
//=====================================
//�㏑���m�F
static void Br_BvSave_OverWrite_CreateDisplay( BR_BVSAVE_WORK *p_wk, BR_BVSAVE_PROC_PARAM *p_param );
static void Br_BvSave_OverWrite_DeleteDisplay( BR_BVSAVE_WORK *p_wk, BR_BVSAVE_PROC_PARAM *p_param );
//�_�E�����[�h���
static void Br_BvSave_Download_CreateDisplay( BR_BVSAVE_WORK * p_wk, BR_BVSAVE_PROC_PARAM	*p_param );
static void Br_BvSave_Download_DeleteDisplay( BR_BVSAVE_WORK * p_wk, BR_BVSAVE_PROC_PARAM	*p_param );
//�ۑ����
static void Br_BvSave_Save_CreateDisplay( BR_BVSAVE_WORK * p_wk, BR_BVSAVE_PROC_PARAM	*p_param );
static void Br_BvSave_Save_DeleteDisplay( BR_BVSAVE_WORK * p_wk, BR_BVSAVE_PROC_PARAM	*p_param );

//-------------------------------------
///	���̑�
//=====================================
static BOOL Br_BvSave_GetTrgYes( BR_BVSAVE_WORK * p_wk, u32 x, u32 y );
static BOOL Br_BvSave_GetTrgNo( BR_BVSAVE_WORK * p_wk, u32 x, u32 y );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	�o�g���r�f�I�ۑ��v���Z�X
//=====================================
const GFL_PROC_DATA BR_BVSAVE_ProcData =
{	
	BR_BVSAVE_PROC_Init,
	BR_BVSAVE_PROC_Main,
	BR_BVSAVE_PROC_Exit,
};

//=============================================================================
/**
 *					�f�[�^
 */
//=============================================================================

//=============================================================================
/**
 *					�o�g���r�f�I�ۑ��v���Z�X
*/
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�ۑ��v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVSAVE_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BVSAVE_WORK				*p_wk;
	BR_BVSAVE_PROC_PARAM	*p_param	= p_param_adrs;

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(BR_BVSAVE_WORK), BR_PROC_SYS_GetHeapID( p_param->p_procsys ) );
	GFL_STD_MemClear( p_wk, sizeof(BR_BVSAVE_WORK) );	
  p_wk->p_param = p_param;
	p_wk->heapID	= BR_PROC_SYS_GetHeapID( p_param->p_procsys );

  //���W���[��������
  p_wk->p_bmpoam		= BmpOam_Init( p_wk->heapID, p_param->p_unit);
  p_wk->p_seq = BR_SEQ_Init( p_wk, Br_BvSave_Seq_VideoDownloadSave, p_wk->heapID );
  p_wk->p_que = PRINTSYS_QUE_Create( p_wk->heapID );

  { 
    int i;
    for( i = 0; i < CLSYS_DRAW_MAX; i++ )
    { 
      p_wk->p_balleff[i] = BR_BALLEFF_Init( p_param->p_unit, p_param->p_res, i, p_wk->heapID );
    }
  }

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�ۑ��v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVSAVE_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	BR_BVSAVE_WORK				*p_wk	= p_wk_adrs;
	BR_BVSAVE_PROC_PARAM	*p_param	= p_param_adrs;

  { 
    int i;
    for( i = 0; i < CLSYS_DRAW_MAX; i++ )
    { 
      BR_BALLEFF_Exit( p_wk->p_balleff[i] );
    }
  }

	//���W���[���j��
  PRINTSYS_QUE_Delete( p_wk->p_que );
  BR_SEQ_Exit( p_wk->p_seq );
  BmpOam_Exit( p_wk->p_bmpoam );

  BR_PROC_SYS_Pop( p_wk->p_param->p_procsys );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	return GFL_PROC_RES_FINISH;
}
//----------------------------------------------------------------------------
/**
 *	@brief	�o�g���r�f�I�ۑ��v���Z�X	������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT BR_BVSAVE_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	

	BR_BVSAVE_WORK	*p_wk	= p_wk_adrs;

  //�V�[�P���X�Ǘ�
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

  //�\��
  PRINTSYS_QUE_Main( p_wk->p_que );
  if( p_wk->p_text )
  { 
    BR_TEXT_PrintMain( p_wk->p_text );
  }

  //���b�Z�[�W�E�B���h�E
  {
    int i;
    for( i = 0; i < BR_BVSAVE_MSGWINID_S_MAX; i++ )
    {
      if( p_wk->p_msgwin_s[i] )
      { 
        BR_MSGWIN_PrintMain( p_wk->p_msgwin_s[i] );
      }
    }
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
 *	@brief  �^��f�[�^�{�̂��_�E�����[�h�����ĕۑ���ʂ֍s��
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BvSave_Seq_VideoDownloadSave( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
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

  BR_BVSAVE_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHANGE_MAIN:
    //�쐬
    Br_BvSave_Download_CreateDisplay( p_wk, p_wk->p_param );
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
      req_param.download_video_number = p_wk->p_param->video_number;
      BR_NET_StartRequest( p_wk->p_param->p_net, BR_NET_REQUEST_BATTLE_VIDEO_DOWNLOAD, &req_param );
      PMSND_PlaySE( BR_SND_SE_SEARCH );
      p_wk->cnt = 0;

      *p_seq  = SEQ_DOWNLOAD_WAIT;
    }
    break;
  case SEQ_DOWNLOAD_WAIT:
    if( BR_NET_WaitRequest( p_wk->p_param->p_net ) )
    { 
      if( p_wk->cnt++ > RR_SEARCH_SE_FRAME )
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

      int video_number;
      BATTLE_REC_RECV       *p_btl_rec;

      err = BR_NET_GetError( p_wk->p_param->p_net, &msg );

      if( err == BR_NET_ERR_RETURN_NONE )
      { 
        if( BR_NET_GetDownloadBattleVideo( p_wk->p_param->p_net, &p_btl_rec, &video_number ) )
        {   
          //��M�����̂ŁAbattle_rec��ݒ�
          BattleRec_DataSet( &p_btl_rec->profile, &p_btl_rec->head,
              &p_btl_rec->rec, GAMEDATA_GetSaveControlWork( p_wk->p_param->p_gamedata ) );
          *p_seq  = SEQ_FADEOUT_START;
        }
        else
        { 
          //��M�Ɏ��s�����̂ŁA�߂�
          BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_info_031 );
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
      *p_seq  = SEQ_FADEOUT_START_RET;
    }
    break;

    //-------------------------------------
    ///	����
    //=====================================
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
    Br_BvSave_Download_DeleteDisplay( p_wk, p_wk->p_param );
    BR_SEQ_SetNext( p_seqwk, Br_BvSave_Seq_SaveSelect );
    break;

    //-------------------------------------
    ///	���ǂ�
    //=====================================
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
    Br_BvSave_Download_DeleteDisplay( p_wk, p_wk->p_param );
    BR_SEQ_End( p_seqwk );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ۑ��I���V�[�P���X
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BvSave_Seq_SaveSelect( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHANGE_MAIN,
    SEQ_CHANGE_FADEIN_START,
    SEQ_CHANGE_FADEIN_WAIT,

    //���C��
    SEQ_SAVE_MAIN,

    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_FADEOUT_END,

    //�㏑��
    SEQ_FADEOUT_START_OV,
    SEQ_FADEOUT_WAIT_OV,
    SEQ_FADEOUT_END_OV,

    //�߂�
    SEQ_FADEOUT_START_RET,
    SEQ_FADEOUT_WAIT_RET,
    SEQ_FADEOUT_MAIN_RET,
  };

  BR_BVSAVE_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CHANGE_MAIN:
    //�쐬
    Br_BvSave_Save_CreateDisplay( p_wk, p_wk->p_param );
    *p_seq  = SEQ_CHANGE_FADEIN_START;
    break;

  case SEQ_CHANGE_FADEIN_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    *p_seq  = SEQ_CHANGE_FADEIN_WAIT;
    break;

  case SEQ_CHANGE_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      *p_seq  = SEQ_SAVE_MAIN;
    }
    break;

    //-------------------------------------
    /// �{�^��
    //=====================================
  case SEQ_SAVE_MAIN:
    {
      int i;
      u32 idx;
      u32 x, y;
      if( GFL_UI_TP_GetPointTrg( &x, &y ) )
      {
        for( i = BR_BVSAVE_SAVE_BTNID_SAVE_01; i <= BR_BVSAVE_SAVE_BTNID_SAVE_03; i++ )
        { 
          idx = i - BR_BVSAVE_SAVE_BTNID_SAVE_01;

          //�ۑ��ꏊ�{�^����������
          if( BR_BTN_GetTrg( p_wk->p_btn[i], x, y ) )
          {	
            GFL_POINT pos;
            pos.x = x;
            pos.y = y;
            BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
            p_wk->save_btn_idx  = idx;
            if( p_wk->p_param->cp_rec_saveinfo->is_valid[BR_SAVEDATA_OTHER_00+idx] )
            { 
              *p_seq  = SEQ_FADEOUT_START_OV;
            }
            else
            { 
              *p_seq  = SEQ_FADEOUT_START;
            }
          }
        }

        //�߂�{�^����������
        if( BR_BTN_GetTrg( p_wk->p_btn[BR_BVSAVE_SAVE_BTNID_RETURN], x, y ) )
        {
          GFL_POINT pos;
          pos.x = x;
          pos.y = y;
          BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_SUB ], BR_BALLEFF_MOVE_EMIT, &pos );
          *p_seq  = SEQ_FADEOUT_START_RET;
        }
      }
    }
    break;

    //-------------------------------------
    /// ��������
    //=====================================
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

    Br_BvSave_Save_DeleteDisplay( p_wk, p_wk->p_param );
    BR_SEQ_SetNext( p_wk->p_seq, Br_BvSave_Seq_Save );
    break;

    //-------------------------------------
    /// �I�[�o�[���C�g
    //=====================================
  case SEQ_FADEOUT_START_OV:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    (*p_seq)++;
    break;

  case SEQ_FADEOUT_WAIT_OV:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      (*p_seq)++;
    }
    break;

  case SEQ_FADEOUT_END_OV:
    Br_BvSave_Save_DeleteDisplay( p_wk, p_wk->p_param );
    BR_SEQ_SetNext( p_wk->p_seq, Br_BvSave_Seq_OverWrite );
    break;

    //-------------------------------------
    ///	�߂�{�^��
    //=====================================
  case SEQ_FADEOUT_START_RET:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    *p_seq  = SEQ_FADEOUT_WAIT_RET;
    break;

  case SEQ_FADEOUT_WAIT_RET:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    { 
      *p_seq  = SEQ_FADEOUT_MAIN_RET;
    }
    break;

  case SEQ_FADEOUT_MAIN_RET:
    //�č\�z
    Br_BvSave_Save_DeleteDisplay( p_wk, p_wk->p_param );
    BR_SEQ_End( p_seqwk );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �㏑���V�[�P���X
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BvSave_Seq_OverWrite( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CREATE_DISPLAY,
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,

    //���C��
    SEQ_SAVE_MAIN,

    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_FADEOUT_END,

    SEQ_FADEOUT_START_RET,
    SEQ_FADEOUT_WAIT_RET,
    SEQ_FADEOUT_END_RET,
  };

  BR_BVSAVE_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_CREATE_DISPLAY:
    Br_BvSave_OverWrite_CreateDisplay( p_wk, p_wk->p_param );
    (*p_seq)++;
    break;
  case SEQ_FADEIN_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    (*p_seq)++;
    break;
  case SEQ_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      (*p_seq)++;
    }
    break;

    //���C��
  case SEQ_SAVE_MAIN:
    { 
      u32 x, y;
      if( GFL_UI_TP_GetPointTrg( &x, &y ) )
      {
        if( Br_BvSave_GetTrgYes( p_wk, x, y ) )
        { 
          *p_seq  = SEQ_FADEOUT_START;
        }

        if( Br_BvSave_GetTrgNo( p_wk, x, y ) )
        { 
          *p_seq  = SEQ_FADEOUT_START_RET;
        }
      }
    }
    break;

    //-------------------------------------
    ///	�Z�[�u��
    //=====================================
  case SEQ_FADEOUT_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    (*p_seq)++;
    break;
  case SEQ_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      (*p_seq)++;
    }
    break;
  case SEQ_FADEOUT_END:
    Br_BvSave_OverWrite_DeleteDisplay( p_wk, p_wk->p_param );
    BR_SEQ_SetNext( p_seqwk, Br_BvSave_Seq_Save );
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
    Br_BvSave_OverWrite_DeleteDisplay( p_wk, p_wk->p_param );
    BR_SEQ_SetNext( p_seqwk, Br_BvSave_Seq_SaveSelect );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ۑ�
 *
 *	@param	BR_SEQ_WORK *p_seqwk  �V�[�P���X���[�N
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void Br_BvSave_Seq_Save( BR_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,

    //�ۑ�����
    SEQ_SAVE_INIT,
    SEQ_SAVE_WAIT,

    //���C�ɓ��著�M����
    SEQ_SEND_INIT,
    SEQ_SEND_WAIT,
    SEQ_TRG,

    //�t�F�[�h�A�E�g
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
    SEQ_FADEOUT_MAIN,
  };

  BR_BVSAVE_WORK	*p_wk	= p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
    if( p_wk->p_text == NULL )
    { 
      p_wk->p_text    = BR_TEXT_Init( p_wk->p_param->p_res, p_wk->p_que, p_wk->heapID );
    }
    BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_info_009 );

    {
      GFL_POINT pos;
      pos.x = 256/2;
      pos.y = 192/2;
      BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_MAIN ], BR_BALLEFF_MOVE_BIG_CIRCLE, &pos );
    }

    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_IN );
    (*p_seq)++;
    break;

  case SEQ_FADEIN_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      (*p_seq)++;
    }
    break;

    //�ۑ�����
  case SEQ_SAVE_INIT:
    PMSND_PlaySE( BR_SND_SE_SEARCH );
    p_wk->cnt = 0;
    p_wk->work0 = 0;
    p_wk->work1 = 0;
    (*p_seq)++;
    break;

  case SEQ_SAVE_WAIT:
    { 
      SAVE_RESULT result;

      int save_idx  = BR_SAVEDATA_OTHER_00+p_wk->save_btn_idx;

      result  = BattleRec_GDS_RecvData_Save(p_wk->p_param->p_gamedata, save_idx, p_wk->p_param->is_secure, &p_wk->work0, &p_wk->work1, p_wk->heapID );
      if( result == SAVE_RESULT_OK || result == SAVE_RESULT_NG )
      { 
        p_wk->p_param->is_save  = TRUE; 
        (*p_seq)++;
      }
      p_wk->cnt++;
    }
    break;

    //���C�ɓ��著�M�����@&& ���b�Z�[�W�\��
  case SEQ_SEND_INIT:
    {
      BR_NET_REQUEST_PARAM  req_param;
      GFL_STD_MemClear( &req_param, sizeof(BR_NET_REQUEST_PARAM) );
      req_param.upload_favorite_video_number  = RecHeader_ParamGet( BattleRec_HeaderPtrGet(), RECHEAD_IDX_DATA_NUMBER, 0 );
      BR_NET_StartRequest( p_wk->p_param->p_net, BR_NET_REQUEST_FAVORITE_VIDEO_UPLOAD, &req_param );
      BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_info_011 );
    }
    p_wk->cnt++;
    (*p_seq)++;
    break;

  case SEQ_SEND_WAIT:
    if( BR_NET_WaitRequest( p_wk->p_param->p_net ) )
    { 
      if( p_wk->cnt++ > RR_SEARCH_SE_FRAME )
      { 
        BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_MAIN ], BR_BALLEFF_MOVE_NOP, NULL );

        BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_info_012 );
        PMSND_PlaySE( BR_SND_SE_SEARCH_OK );
        (*p_seq)++;
      }
    }
    break;
  
  case SEQ_TRG:
    if( GFL_UI_TP_GetTrg() )
    { 
      (*p_seq)++;
    }
    break;

  case SEQ_FADEOUT_START:
    BR_FADE_StartFade( p_wk->p_param->p_fade, BR_FADE_TYPE_ALPHA_BG012OBJ, BR_FADE_DISPLAY_BOTH, BR_FADE_DIR_OUT );
    (*p_seq)++;
    break;

  case SEQ_FADEOUT_WAIT:
    if( BR_FADE_IsEnd( p_wk->p_param->p_fade ) )
    {
      (*p_seq)++;
    }
    break;
  case SEQ_FADEOUT_MAIN:
    if( p_wk->p_text )
    { 
      BR_TEXT_Exit( p_wk->p_text, p_wk->p_param->p_res );
      p_wk->p_text  = NULL;
    }
    BR_SEQ_End( p_seqwk );
    break;
  }
}


//=============================================================================
/**
 *    ��ʍ쐬
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �㏑���m�F��ʍ쐬
 *
 *	@param	BR_BVSAVE_WORK *p_wk  ���[�N
 *	@param	*p_param                ����
 */
//-----------------------------------------------------------------------------
static void Br_BvSave_OverWrite_CreateDisplay( BR_BVSAVE_WORK *p_wk, BR_BVSAVE_PROC_PARAM *p_param )
{
  //���ʂ̓e�L�X�g����
  if( p_wk->p_text == NULL )
  { 
    p_wk->p_text  = BR_TEXT_Init( p_param->p_res, p_wk->p_que, p_wk->heapID );
  }

  { 
    WORDSET *p_word     = BR_RES_GetWordSet( p_param->p_res );
    GFL_MSGDATA *p_msg  = BR_RES_GetMsgData( p_param->p_res );
    STRBUF  *p_src  = GFL_MSG_CreateString( p_msg, msg_info_013 );
    STRBUF  *p_dst  = GFL_STR_CreateCopyBuffer( p_src, p_wk->heapID );

    WORDSET_RegisterWord( p_word, 0, 
        p_param->cp_rec_saveinfo->p_name[p_wk->save_btn_idx],
        p_param->cp_rec_saveinfo->sex[p_wk->save_btn_idx], 
        TRUE, PM_LANG );

    WORDSET_ExpandStr( p_word, p_dst, p_src );

    BR_TEXT_PrintBuff( p_wk->p_text, p_param->p_res, p_dst );

    GFL_STR_DeleteBuffer( p_dst );
    GFL_STR_DeleteBuffer( p_src );
  }

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
    } sc_msgwin_data[BR_BVSAVE_MSGWINID_S_MAX]  =
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
    for( i = 0; i < BR_BVSAVE_MSGWINID_S_MAX; i++ )
    { 
      p_wk->p_msgwin_s[i]  = BR_MSGWIN_Init( BG_FRAME_S_FONT, sc_msgwin_data[i].x, sc_msgwin_data[i].y, sc_msgwin_data[i].w, sc_msgwin_data[i].h, PLT_BG_S_FONT, p_wk->p_que, p_wk->heapID );
      BR_MSGWIN_PrintColor( p_wk->p_msgwin_s[i], p_msg, sc_msgwin_data[i].msgID, p_font, BR_PRINT_COL_NORMAL );
    }
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �㏑���m�F��ʔj��
 *
 *	@param	BR_BVSAVE_WORK *p_wk  ���[�N
 *	@param	*p_param                ����
 */
//-----------------------------------------------------------------------------
static void Br_BvSave_OverWrite_DeleteDisplay( BR_BVSAVE_WORK *p_wk, BR_BVSAVE_PROC_PARAM *p_param )
{ 
  //���b�Z�[�W�E�B���h�E
  {
    int i;
    for( i = 0; i < BR_BVSAVE_MSGWINID_S_MAX; i++ )
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
}

//----------------------------------------------------------------------------
/**
 *	@brief  �_�E�����[�h�p�̊G���쐬
 *
 *	@param	BR_BVSAVE_WORK * p_wk ���[�N
 *	@param	                      ����
 */
//-----------------------------------------------------------------------------
static void Br_BvSave_Download_CreateDisplay( BR_BVSAVE_WORK * p_wk, BR_BVSAVE_PROC_PARAM	*p_param )
{ 
  //�_�E�����[�h�p�̊G���쐬
  if( p_wk->p_text == NULL )
  { 
    p_wk->p_text    = BR_TEXT_Init( p_wk->p_param->p_res, p_wk->p_que, p_wk->heapID );
  }
  BR_TEXT_Print( p_wk->p_text, p_wk->p_param->p_res, msg_info_023 );
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
 *	@param	BR_BVSAVE_WORK * p_wk ���[�N
 *	@param	                      ����
 */
//-----------------------------------------------------------------------------
static void Br_BvSave_Download_DeleteDisplay( BR_BVSAVE_WORK * p_wk, BR_BVSAVE_PROC_PARAM	*p_param )
{ 
  BR_BALLEFF_StartMove( p_wk->p_balleff[ CLSYS_DRAW_MAIN ], BR_BALLEFF_MOVE_NOP, NULL );
  if( p_wk->p_text )
  { 
    BR_TEXT_Exit( p_wk->p_text, p_wk->p_param->p_res );
    p_wk->p_text  = NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ۑ��I����ʍ쐬
 *
 *	@param	BR_BVSAVE_WORK * p_wk ���[�N
 *	@param	BR_BVSAVE_PROC_PARAM  ����
 */
//-----------------------------------------------------------------------------
static void Br_BvSave_Save_CreateDisplay( BR_BVSAVE_WORK * p_wk, BR_BVSAVE_PROC_PARAM	*p_param )
{ 
  enum
  { 
    OAM_POS_INIT_Y  = 40,
    OAM_POS_OFS_Y   = 8+32,
  };

  GFL_FONT    *p_font  = BR_RES_GetFont( p_param->p_res );
  GFL_MSGDATA *p_msg   = BR_RES_GetMsgData( p_param->p_res );
  WORDSET     *p_word  = BR_RES_GetWordSet( p_param->p_res );
  STRBUF      *p_strbuf= GFL_STR_CreateBuffer( 128, p_wk->heapID );    
  STRBUF      *p_src   = GFL_STR_CreateBuffer( 128, p_wk->heapID );    

  int i;
  u32 idx;
  GFL_CLWK_DATA cldata;
  BR_RES_OBJ_DATA res;

  GFL_STD_MemClear( &cldata, sizeof(GFL_CLWK_DATA) );

  cldata.pos_x    = 48;
  cldata.pos_y    = OAM_POS_INIT_Y;
  cldata.anmseq   = 11;
  cldata.softpri  = 1;

	BR_RES_LoadOBJ( p_param->p_res, BR_RES_OBJ_BROWSE_BTN_S, p_wk->heapID );
  BR_RES_GetOBJRes( p_param->p_res, BR_RES_OBJ_BROWSE_BTN_S, &res );

  //�Z�[�u�ӏ��{�^��
  for( i = BR_BVSAVE_SAVE_BTNID_START; i < BR_BVSAVE_SAVE_BTNID_RETURN; i++ )
  { 
    idx = BR_SAVEDATA_OTHER_00 + (i - BR_BVSAVE_SAVE_BTNID_START);

    //�^��̕ۑ��󋵂ɂ��{�^���̕����ύX
    if( p_param->cp_rec_saveinfo->is_valid[idx] )
    { 
      //�����̋L�^
      GFL_MSG_GetString( p_msg, msg_25, p_src );

      //�P��o�^
      WORDSET_RegisterWord( p_word, 0, 
          p_param->cp_rec_saveinfo->p_name[idx],
          p_param->cp_rec_saveinfo->sex[idx], 
          TRUE, PM_LANG );
        
      WORDSET_ExpandStr( p_word, p_strbuf, p_src );
    }
    else
    { 
      //��
      GFL_MSG_GetString( p_msg, msg_10, p_strbuf );
    }

    p_wk->p_btn[ i ]  = BR_BTN_InitEx( &cldata, p_strbuf, BR_BTN_DATA_WIDTH, CLSYS_DRAW_SUB, p_param->p_unit, p_wk->p_bmpoam, p_font, &res, p_wk->heapID );


    cldata.pos_y    += OAM_POS_OFS_Y;
  }

  //�߂�{�^��
  cldata.anmseq   = 1;
  p_wk->p_btn[ BR_BVSAVE_SAVE_BTNID_RETURN ]  = BR_BTN_Init( &cldata, msg_05, BR_BTN_DATA_WIDTH, CLSYS_DRAW_SUB, p_param->p_unit, p_wk->p_bmpoam, p_font, p_msg, &res, p_wk->heapID );

  //����
  { 
    GDS_PROFILE_PTR p_profile = BattleRec_GDSProfilePtrGet();
    STRBUF  *p_profile_name = GDS_Profile_CreateNameString( p_profile, GFL_HEAP_LOWID(p_wk->heapID) );

    if( p_wk->p_text == NULL )
    { 
      p_wk->p_text    = BR_TEXT_Init( p_param->p_res, p_wk->p_que, p_wk->heapID );
    }

    //�����̋L�^
    GFL_MSG_GetString( p_msg, msg_608, p_src );

    //�P��o�^
    WORDSET_RegisterWord( p_word, 0, 
        p_profile_name,
        GDS_Profile_GetSex( p_profile ), 
        TRUE, PM_LANG );

    WORDSET_ExpandStr( p_word, p_strbuf, p_src );

    BR_TEXT_PrintBuff( p_wk->p_text, p_param->p_res, p_strbuf );

    GFL_STR_DeleteBuffer( p_profile_name );
  }

  GFL_STR_DeleteBuffer( p_src );
  GFL_STR_DeleteBuffer( p_strbuf );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �ۑ��I����ʔj��
 *
 *	@param	BR_BVSAVE_WORK * p_wk ���[�N
 *	@param	BR_BVSAVE_PROC_PARAM  ����
 */
//-----------------------------------------------------------------------------
static void Br_BvSave_Save_DeleteDisplay( BR_BVSAVE_WORK * p_wk, BR_BVSAVE_PROC_PARAM	*p_param )
{ 
  //���ʔj��
  if( p_wk->p_text )
  { 
    BR_TEXT_Exit( p_wk->p_text, p_param->p_res );
    p_wk->p_text  = NULL;
  }

  //�{�^���j��
  { 
    int i;
    for( i = BR_BVSAVE_SAVE_BTNID_START; i < BR_BVSAVE_SAVE_BTNID_MAX; i++ )
    { 
      if( p_wk->p_btn[i] )
      { 
        BR_BTN_Exit( p_wk->p_btn[i] );
        p_wk->p_btn[i]  = NULL;
      }
    }
  }

  BR_RES_UnLoadOBJ( p_param->p_res, BR_RES_OBJ_BROWSE_BTN_S ); 
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
static BOOL Br_BvSave_GetTrgYes( BR_BVSAVE_WORK * p_wk, u32 x, u32 y )
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
static BOOL Br_BvSave_GetTrgNo( BR_BVSAVE_WORK * p_wk, u32 x, u32 y )
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
