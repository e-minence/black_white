//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		src\test\nagihashi\debug_utilproc.c
 *	@brief
 *	@author		Toru=Nagihashi
 *	@data		2010.03.09
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]

#ifdef PM_DEBUG

//	���C�u����
#include <gflib.h>

//�V�X�e��
#include "pm_version.h"
#include "system/main.h"
#include "gamesystem/game_data.h"

//�Z�[�u�f�[�^
#include "savedata/regulation.h"

//�A�[�J�C�u
#include "arc_def.h"
#include "debug_message.naix"
#include "font/font.naix"
#include "msg/debug/msg_d_nagihashi.h"
#include "debug_regulation_card.naix"

//���W���[��
#include "net/delivery_irc.h"
#include "debug_util.h"
#include "debug_graphic.h"

//�O�����J
#include "debug_utilproc.h"

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	�p���b�g
//=====================================
enum
{ 
  PLT_BG_FONT_M = 0,

  PLT_BG_FONT_S = 0,
};

//-------------------------------------
///	�t���[��
//=====================================
enum
{
  BG_FRAME_TEXT_M = GFL_BG_FRAME0_M,

  BG_FRAME_BACK_S = GFL_BG_FRAME0_S,
};

//=============================================================================
/**
 *					�\���̐錾
*/
//=============================================================================
//-------------------------------------
///	���C�����[�N
//=====================================
typedef struct
{
  DELIVERY_IRC_WORK         *p_delivery;
  GAMEDATA                  *p_gamedata;

  DEUBG_GRAPHIC_WORK        *p_graphic;

  DEBUG_SEQ_WORK            *p_seq;
  DEBUG_TEXT_WORK           *p_text;
  DEBUG_NUMIN_WORK          *p_numin;

  GFL_FONT                  *p_font;
  PRINT_QUE                 *p_que;
  GFL_MSGDATA               *p_msg;

  u32                       reg_card_num;

  REGULATION_CARDDATA       *p_buff;

} DEBUG_NAGI_IRC_REGULATION_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v�錾
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT DEBUG_NAGI_IRC_REGULATION_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT DEBUG_NAGI_IRC_REGULATION_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT DEBUG_NAGI_IRC_REGULATION_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
//-------------------------------------
/// �V�[�P���X
//=====================================
static void SEQFUNC_FadeIn( DEBUG_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_Select( DEBUG_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_Derivery( DEBUG_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void SEQFUNC_FadeOut( DEBUG_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
/// ���̑�
//=====================================
static void UTIL_LoadBg( HEAPID heapID );

//=============================================================================
/**
 *					�f�[�^
*/
//=============================================================================
//-------------------------------------
///	�ԊO�����M�����[�V�������M�v���Z�X
//=====================================
const GFL_PROC_DATA	DEBUG_NAGI_IRC_REGULATION_ProcData =
{ 
  DEBUG_NAGI_IRC_REGULATION_PROC_Init,
  DEBUG_NAGI_IRC_REGULATION_PROC_Main,
  DEBUG_NAGI_IRC_REGULATION_PROC_Exit
};
//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO�����M�����[�V�����z�M�ł΂���  ������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_NAGI_IRC_REGULATION_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
  DEBUG_NAGI_IRC_REGULATION_WORK  *p_wk;

  //�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_NAGI_DEBUG_SUB, 0x20000 );

  //�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(DEBUG_NAGI_IRC_REGULATION_WORK), HEAPID_NAGI_DEBUG_SUB );
	GFL_STD_MemClear( p_wk, sizeof(DEBUG_NAGI_IRC_REGULATION_WORK) );	
  p_wk->p_gamedata  = GAMEDATA_Create( HEAPID_NAGI_DEBUG_SUB );

  //���ʃ��W���[���̏�����
  p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_NAGI_DEBUG_SUB );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_NAGI_DEBUG_SUB );
  p_wk->p_msg     = GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_DEBUG_MESSAGE, NARC_debug_message_d_nagihashi_dat, HEAPID_NAGI_DEBUG_SUB );

  //�O���t�B�b�N
  p_wk->p_graphic = DEUBG_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_NAGI_DEBUG_SUB );
  UTIL_LoadBg( HEAPID_NAGI_DEBUG_SUB );

  //�V�[�P���X
  p_wk->p_seq = DEBUG_SEQ_Init( p_wk, SEQFUNC_FadeIn, HEAPID_NAGI_DEBUG_SUB );

  //���W���[��
  p_wk->p_text  = DEBUG_TEXT_Init( BG_FRAME_TEXT_M, PLT_BG_FONT_M, p_wk->p_que, p_wk->p_font, HEAPID_NAGI_DEBUG_SUB );
  p_wk->p_numin = DEBUG_NUMIN_Init( BG_FRAME_TEXT_M, PLT_BG_FONT_M, p_wk->p_font, HEAPID_NAGI_DEBUG_SUB );

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO�����M�����[�V�����z�M�ł΂���  �j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_NAGI_IRC_REGULATION_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  DEBUG_NAGI_IRC_REGULATION_WORK  *p_wk     = p_wk_adrs;


  //���W���[���j��
  DEBUG_NUMIN_Exit( p_wk->p_numin ); 
  DEBUG_TEXT_Exit( p_wk->p_text );

  //�V�[�P���X
  DEBUG_SEQ_Exit( p_wk->p_seq );

  //�O���t�B�b�N
  DEUBG_GRAPHIC_Exit( p_wk->p_graphic );

	//���ʃ��W���[���̔j��
  GFL_MSG_Delete( p_wk->p_msg );
	PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_font );

  //�v���Z�X���[�N�j��
  GAMEDATA_Delete( p_wk->p_gamedata );
	GFL_PROC_FreeWork( p_proc );

  //�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_NAGI_DEBUG_SUB );

  return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�ԊO�����M�����[�V�����z�M�ł΂���  ���C������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DEBUG_NAGI_IRC_REGULATION_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  DEBUG_NAGI_IRC_REGULATION_WORK  *p_wk     = p_wk_adrs;

  //�V�[�P���X
  DEBUG_SEQ_Main( p_wk->p_seq );
  if( DEBUG_SEQ_IsEnd( p_wk->p_seq ) )
  { 
    return GFL_PROC_RES_FINISH;
  }

  //�`��
  DEUBG_GRAPHIC_2D_Draw( p_wk->p_graphic );

  PRINTSYS_QUE_Main( p_wk->p_que );

  DEBUG_TEXT_Main( p_wk->p_text );

  DEBUG_NUMIN_Main( p_wk->p_numin ); 

  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *    �V�[�P���X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�C��
 *
 *	@param	DEBUG_SEQ_WORK *p_wk  �V�[�P���X�Ǘ�
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeIn( DEBUG_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
	enum
	{	
		SEQ_FADEOUT_START,
		SEQ_FADEOUT_WAIT,
		SEQ_END,
	};

	switch( *p_seq )
	{	
	case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
		*p_seq	= SEQ_FADEOUT_WAIT;
		break;

	case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_END;
		}
		break;

	case SEQ_END:
		DEBUG_SEQ_SetNext( p_seqwk, SEQFUNC_Select );
		break;

	default:
		GF_ASSERT(0);
	}
}
//----------------------------------------------------------------------------
/**
 *	@brief  �I��
 *
 *	@param	DEBUG_SEQ_WORK *p_wk  �V�[�P���X�Ǘ�
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Select( DEBUG_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG,
    SEQ_START_LIST,
    SEQ_WAIT_LIST,
    SEQ_END,
  };

  DEBUG_NAGI_IRC_REGULATION_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_MSG:
    DEBUG_TEXT_Print( p_wk->p_text, p_wk->p_msg, TEST_STR_01, DEBUG_TEXT_TYPE_STREAM );
    (*p_seq)++;
    break;
  case SEQ_START_LIST:
    if( DEBUG_TEXT_IsEndPrint( p_wk->p_text ) )
    { 
      DEBUG_NUMIN_Start( p_wk->p_numin, 4, 0, NARC_DEBUG_REGULATION_CARD_MAX-1, p_wk->reg_card_num, 12, 8 );
      (*p_seq)++;
    }
    break;
  case SEQ_WAIT_LIST:
    if( GFL_UI_KEY_GetTrg() == PAD_BUTTON_B )
    { 
      DEBUG_SEQ_SetNext( p_seqwk, SEQFUNC_FadeOut );
    }

    if( DEBUG_NUMIN_IsEnd( p_wk->p_numin ) )
    { 
      p_wk->reg_card_num  = DEBUG_NUMIN_GetNum( p_wk->p_numin );
      (*p_seq)++;
    }
    break;
  case SEQ_END:
    DEBUG_TEXT_Print( p_wk->p_text, p_wk->p_msg, TEST_STR_03, DEBUG_TEXT_TYPE_WAIT );

    DEBUG_SEQ_SetNext( p_seqwk, SEQFUNC_Derivery );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �z�M
 *
 *	@param	DEBUG_SEQ_WORK *p_wk  �V�[�P���X�Ǘ�
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_Derivery( DEBUG_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_INIT,
    SEQ_START_RECVCARD,
    SEQ_WAIT_RECVCARD,
    SEQ_DELETE,
    SEQ_END,
  };

  DEBUG_NAGI_IRC_REGULATION_WORK  *p_wk     = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_INIT:
    { 
      p_wk->p_buff  = GFL_ARC_LoadDataAlloc( ARCID_DEBUG_REGCARD, p_wk->reg_card_num, HEAPID_NAGI_DEBUG_SUB );
    }

    //�z�M�f�[�^��ݒ�
    { 
      DELIVERY_IRC_INIT init;
      GFL_STD_MemClear( &init, sizeof(DELIVERY_IRC_INIT) );
      init.NetDevID = WB_NET_IRC_REGULATION;
      init.ConfusionID  = 0;
      init.heapID = HEAPID_NAGI_DEBUG_SUB;
      init.dataNum  = 1;

      init.data[0].datasize = sizeof(REGULATION_CARDDATA);
      init.data[0].pData  = (u8*)p_wk->p_buff;
      init.data[0].LangCode = Regulation_GetCardParam( p_wk->p_buff, REGULATION_CARD_LANGCODE );
      init.data[0].version  = Regulation_GetCardParam( p_wk->p_buff, REGULATION_CARD_ROMVER );
      OS_TPrintf( "lang %d ver %d\n", init.data[0].LangCode, init.data[0].version );

      p_wk->p_delivery  = DELIVERY_IRC_Init(&init);
    }
    (*p_seq)++;
    break;
  case SEQ_START_RECVCARD:
    DELIVERY_IRC_Main( p_wk->p_delivery );
    if( DELIVERY_IRC_SendStart( p_wk->p_delivery) )
    { 
      *p_seq  = SEQ_WAIT_RECVCARD;
    }
    break;
  case SEQ_WAIT_RECVCARD:
    DELIVERY_IRC_Main( p_wk->p_delivery );
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
    { 
      *p_seq  = SEQ_DELETE;
    }
    break;
  case SEQ_DELETE:
    DELIVERY_IRC_End( p_wk->p_delivery );
    *p_seq  = SEQ_END;
    break;
  case SEQ_END:
    if( GFL_NET_IsResetEnable() )
    {
      p_wk->p_delivery  = NULL;

      GFL_HEAP_FreeMemory( p_wk->p_buff );
      DEBUG_SEQ_SetNext( p_seqwk, SEQFUNC_Select );
    }
  }


}
//----------------------------------------------------------------------------
/**
 *	@brief  �t�F�[�h�A�E�g
 *
 *	@param	DEBUG_SEQ_WORK *p_wk  �V�[�P���X�Ǘ�
 *	@param	*p_seq                �V�[�P���X�ϐ�
 *	@param	*p_wk_adrs            ���[�N
 */
//-----------------------------------------------------------------------------
static void SEQFUNC_FadeOut( DEBUG_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
	enum
	{
		SEQ_FADEIN_START,
		SEQ_FADEIN_WAIT,
		SEQ_EXIT,
	};	

	switch( *p_seq )
	{	
	case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
		*p_seq	= SEQ_FADEIN_WAIT;
		break;

	case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{	
			*p_seq	= SEQ_EXIT;
		}
		break;

	case SEQ_EXIT:
		DEBUG_SEQ_End( p_seqwk );
		break;

	default:
		GF_ASSERT(0);
	}
}


//----------------------------------------------------------------------------
/**
 *	@brief  BG�ǂݍ���
 */
//-----------------------------------------------------------------------------
static void UTIL_LoadBg( HEAPID heapID )
{ 
  //�t�H���g
  {	
    ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_FONT, heapID );

    //�㉺��ʃt�H���g�p���b�g
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_font_default_nclr,
        PALTYPE_MAIN_BG, PLT_BG_FONT_M*0x20, 0x20, heapID );
    GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_font_default_nclr,
        PALTYPE_SUB_BG, PLT_BG_FONT_S*0x20, 0x20, heapID );

      GFL_ARC_CloseDataHandle( p_handle );
  }

  //����BG�͐^����
  GFL_BG_SetBackGroundColor( BG_FRAME_TEXT_M, 0 );
  GFL_BG_SetBackGroundColor( BG_FRAME_BACK_S, 0 );
  GFL_BG_LoadScreenReq( BG_FRAME_TEXT_M );
  GFL_BG_LoadScreenReq( BG_FRAME_BACK_S );
}

#endif //PM_DEBUG
