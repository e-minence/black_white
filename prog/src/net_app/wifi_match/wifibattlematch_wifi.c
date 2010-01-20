//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		wifibattlematch_core.c
 *	@brief	WIFI�̃o�g���}�b�`�R�A���
 *	@author	Toru=Nagihashi
 *	@data		2009.11.02
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	���C�u����
#include <gflib.h>

//	�V�X�e��
#include "system/main.h"
#include "system/gfl_use.h"
#include "sound/pm_sndsys.h"
#include "system/gf_date.h"
#include "system/bmp_winframe.h"
#include "pokeicon/pokeicon.h"
#include "gamesystem/game_data.h"
#include "system/net_err.h"

//	�A�[�J�C�u
#include "arc_def.h"
#include "message.naix"
#include "font/font.naix"
#include "wifimatch_gra.naix"
#include "msg/msg_wifi_match.h"

//	�����\��
#include "print/gf_font.h"
#include "print/printsys.h"
#include "print/wordset.h"

//  �Z�[�u�f�[�^
#include "savedata/dreamworld_data.h"
#include "savedata/regulation.h"

//WIFI�o�g���}�b�`�̃��W���[��
#include "wifibattlematch_graphic.h"
#include "wifibattlematch_view.h"
#include "wifibattlematch_net.h"
#include "wifibattlematch_util.h"

//�O�����J
#include "wifibattlematch_core.h"

//-------------------------------------
///	�I�[�o�[���C
//=====================================
FS_EXTERN_OVERLAY(ui_common);
FS_EXTERN_OVERLAY(dpw_common);

//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef PM_DEBUG
#define PDF_FLAG_OFF
#endif //PM_DEBUG


//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================

//=============================================================================
/**
 *        ��`
*/
//=============================================================================
//-------------------------------------
///	���C�����[�N
//=====================================
typedef struct 
{
	//�O���t�B�b�N�ݒ�
	WIFIBATTLEMATCH_GRAPHIC_WORK	*p_graphic;

  //���\�[�X
  WIFIBATTLEMATCH_VIEW_RESOURCE *p_res;

	//���ʂŎg���t�H���g
	GFL_FONT			            *p_font;

	//���ʂŎg���L���[
	PRINT_QUE				          *p_que;

	//���ʂŎg�����b�Z�[�W
	GFL_MSGDATA	          		*p_msg;

	//���ʂŎg���P��
	WORDSET				          	*p_word;

	//���ʏ��
	PLAYERINFO_WORK         	*p_playerinfo;

	//�ΐ�ҏ��
	MATCHINFO_WORK          	*p_matchinfo;

  //�e�L�X�g��
  WBM_TEXT_WORK             *p_text;

  //���X�g
  WBM_LIST_WORK             *p_list;

  //�l�b�g
  WIFIBATTLEMATCH_NET_WORK  *p_net;

  //���C���V�[�P���X
  WBM_SEQ_WORK              *p_seq;

  //PDF�T�[�o�[���痎�Ƃ��Ă����I��؃f�[�^
  REGULATION_CARDDATA       recv_card;

  //����
  WIFIBATTLEMATCH_CORE_PARAM  *p_param;

  //�E�G�C�g
  u32 cnt;
} WIFIBATTLEMATCH_WIFI_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT WIFIBATTLEMATCH_WIFI_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_WIFI_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_WIFI_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	WIFI���V�[�P���X�֐�
//=====================================
static void WbmWifiSeq_Init( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Join( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_RecvDigCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_CheckDigCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Register( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_UnRegister( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void WbmWifiSeq_Err_ReturnLogin( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	WifiBattleMaptchWifi_ProcData =
{	
	WIFIBATTLEMATCH_WIFI_PROC_Init,
	WIFIBATTLEMATCH_WIFI_PROC_Main,
	WIFIBATTLEMATCH_WIFI_PROC_Exit,
};

//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	�R�A�v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_WIFI_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	WIFIBATTLEMATCH_WIFI_WORK	*p_wk;
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;


	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common));

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_CORE, 0x30000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFIBATTLEMATCH_WIFI_WORK), HEAPID_WIFIBATTLEMATCH_CORE );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_WIFI_WORK) );
  p_wk->p_param = p_param;

	//�O���t�B�b�N�ݒ�
	p_wk->p_graphic	= WIFIBATTLEMATCH_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_WIFIBATTLEMATCH_CORE );

  //���\�[�X�ǂݍ���
  p_wk->p_res     = WIFIBATTLEMATCH_VIEW_LoadResource( WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic ), WIFIBATTLEMATCH_MODE_WIFI, HEAPID_WIFIBATTLEMATCH_CORE );

	//���ʃ��W���[���̍쐬
	p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
												NARC_message_wifi_match_dat, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_word	= WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_COUNTRY_BUFLEN, HEAPID_WIFIBATTLEMATCH_CORE );

	//���W���[���̍쐬
	{	
    DWCUserData *p_user_data;
    WIFI_LIST   *p_wifilist;
    p_wifilist  = GAMEDATA_GetWiFiList( p_param->p_param->p_game_data );
    p_user_data = WifiList_GetMyUserInfo( p_wifilist );

    p_wk->p_net   = WIFIBATTLEMATCH_NET_Init( p_user_data, p_wifilist, HEAPID_WIFIBATTLEMATCH_CORE );
    p_wk->p_text  = WBM_TEXT_Init( BG_FRAME_M_TEXT, PLT_FONT_M, PLT_TEXT_M, CGR_OFS_M_TEXT, p_wk->p_que, p_wk->p_font, HEAPID_WIFIBATTLEMATCH_CORE );
    p_wk->p_seq   = WBM_SEQ_Init( p_wk, WbmWifiSeq_Init, HEAPID_WIFIBATTLEMATCH_CORE );

	}
	
	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	�R�A�v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_WIFI_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum 
  { 
    SEQ_START_DISCONNECT,
    SEQ_WAIT_DISCONNECT,
  };

	WIFIBATTLEMATCH_WIFI_WORK	      *p_wk	= p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_param_adrs;

  if( p_param->result ==  WIFIBATTLEMATCH_CORE_RESULT_FINISH )
  { 
    switch( *p_seq )
    { 
    case SEQ_START_DISCONNECT:
      WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );
      GFL_NET_Exit( NULL );
      *p_seq  = SEQ_WAIT_DISCONNECT;
      return GFL_PROC_RES_CONTINUE; 

    case SEQ_WAIT_DISCONNECT:
      if( GFL_NET_IsExit() )
      { 
        break;
      }
      return GFL_PROC_RES_CONTINUE;
    }
  }

	//���W���[���̔j��
	{	
    WBM_SEQ_Exit( p_wk->p_seq );

    WBM_TEXT_Exit( p_wk->p_text );

    if( p_param->result !=  WIFIBATTLEMATCH_CORE_RESULT_FINISH )
    { 
      WIFIBATTLEMATCH_NET_Exit( p_wk->p_net );
    }

    if( p_wk->p_matchinfo )
    { 
      MATCHINFO_Exit( p_wk->p_matchinfo );
    }

    if( p_wk->p_playerinfo )
    { 
      PLAYERINFO_RND_Exit( p_wk->p_playerinfo );
      p_wk->p_playerinfo  = NULL;
    }
	}

	//���ʃ��W���[���̔j��
	WORDSET_Delete( p_wk->p_word );
	GFL_MSG_Delete( p_wk->p_msg );
	PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_font );

  //���\�[�X�j��
  WIFIBATTLEMATCH_VIEW_UnLoadResource( p_wk->p_res );

	//�O���t�B�b�N�j��
	WIFIBATTLEMATCH_GRAPHIC_Exit( p_wk->p_graphic );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_WIFIBATTLEMATCH_CORE );
	
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(dpw_common));
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	WIFI�o�g���}�b�`���	���C���v���Z�X����
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT WIFIBATTLEMATCH_WIFI_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_MAIN,
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
  };


	WIFIBATTLEMATCH_WIFI_WORK	*p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;

  switch( *p_seq )
  { 
  case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
      *p_seq  = SEQ_FADEIN_WAIT;
    break;
  case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{
      *p_seq  = SEQ_MAIN;
    }
    break;
  case SEQ_MAIN:
    //���C���V�[�P���X
    WBM_SEQ_Main( p_wk->p_seq );

    if( WBM_SEQ_IsEnd( p_wk->p_seq ) )
    { 
      *p_seq  = SEQ_FADEOUT_START;
    }

    //���ʂ̃G���[�����iSC,GDB�ȊO�j
    if( WIFIBATTLEMATCH_NET_CheckError( p_wk->p_net ) )
    { 
      p_param->result = WIFIBATTLEMATCH_CORE_RESULT_ERR_NEXT_LOGIN;
      *p_seq  = SEQ_FADEOUT_START;
    }
    break;

  case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    *p_seq  = SEQ_FADEOUT_WAIT;
    break;

  case SEQ_FADEOUT_WAIT:
		if( !GFL_FADE_CheckFade() )
		{
      return GFL_PROC_RES_FINISH;
    }
  }

	//�`��
	WIFIBATTLEMATCH_GRAPHIC_2D_Draw( p_wk->p_graphic );

	//�v�����g
	PRINTSYS_QUE_Main( p_wk->p_que );

  //�e�L�X�g
  WBM_TEXT_Main( p_wk->p_text );

  //�����\��
  if( p_wk->p_playerinfo )
  { 
    PLAYERINFO_PrintMain( p_wk->p_playerinfo, p_wk->p_que );
  }
  if( p_wk->p_matchinfo )
  { 
    MATCHINFO_PrintMain( p_wk->p_matchinfo, p_wk->p_que );
  }

  return GFL_PROC_RES_CONTINUE;
}
//=============================================================================
/**
 *  WIFI�V�[�P���X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  ������
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Init( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  NAGI_Printf( " mode%d ret%d\n", p_param->mode, p_param->retmode );
  switch( p_param->mode )
  { 
  case WIFIBATTLEMATCH_CORE_MODE_START:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Join );
    break;

  case WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_EndBattle );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  �Q���I��
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Join( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG,    //WIFI�o�g�����ɎQ�����܂����H
    SEQ_START_JOIN_LIST,  //�Q�����܂����H
    SEQ_WAIT_JOIN_LIST,   //�҂�
    SEQ_START_INFO_MSG,    //������
    SEQ_START_CANCEL_MSG,   //���֎Q������̂���߂�H
    SEQ_START_CANCEL_LIST,
    SEQ_WAIT_CANCEL_LIST,
    SEQ_JOIN,

    SEQ_WAIT_MSG, //���b�Z�[�W�҂�
  };
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_MSG:    //WIFI�o�g�����ɎQ�����܂����H
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_01, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_JOIN_LIST );
    break;
  case SEQ_START_JOIN_LIST:  //�Q�����܂����H
    { 
      WBM_LIST_SETUP  setup;
      GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
      setup.p_msg   = p_wk->p_msg;
      setup.p_font  = p_wk->p_font;
      setup.p_que   = p_wk->p_que;
      setup.strID[0]= WIFIMATCH_WIFI_SELECT_01;
      setup.strID[1]= WIFIMATCH_WIFI_SELECT_02;
      setup.strID[2]= WIFIMATCH_WIFI_SELECT_03;
      setup.list_max= 3;
      setup.frm     = BG_FRAME_M_TEXT;
      setup.font_plt= PLT_FONT_M;
      setup.frm_plt = PLT_LIST_M;
      setup.frm_chr = CGR_OFS_M_LIST;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_seq     = SEQ_WAIT_JOIN_LIST;
    }
    break;
  case SEQ_WAIT_JOIN_LIST:   //�҂�
    {
      const u32 select  = WBM_LIST_Main( p_wk->p_list );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        WBM_LIST_Exit( p_wk->p_list );
        switch( select )
        { 
        case 0: //���񂩂���
          WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_RecvDigCard );
          break;
        case 1: //���߂�������
          *p_seq = SEQ_START_INFO_MSG;
          break;
        case 2: //��߂�
          *p_seq = SEQ_START_CANCEL_MSG;
          break;
        }
      }
    }
    break;
  case SEQ_START_INFO_MSG:    //������
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_02, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_MSG );
    break;
  case SEQ_START_CANCEL_MSG:   //���֎Q������̂���߂�H
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_03, WBM_TEXT_TYPE_STREAM );
    *p_seq       = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_CANCEL_LIST );
    break;
  case SEQ_START_CANCEL_LIST:
    { 
      WBM_LIST_SETUP  setup;
      GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
      setup.p_msg   = p_wk->p_msg;
      setup.p_font  = p_wk->p_font;
      setup.p_que   = p_wk->p_que;
      setup.strID[0]= WIFIMATCH_WIFI_SELECT_04;
      setup.strID[1]= WIFIMATCH_WIFI_SELECT_05;
      setup.list_max= 2;
      setup.frm     = BG_FRAME_M_TEXT;
      setup.font_plt= PLT_FONT_M;
      setup.frm_plt = PLT_LIST_M;
      setup.frm_chr = CGR_OFS_M_LIST;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_seq     = SEQ_WAIT_CANCEL_LIST;
    }
    break;
  case SEQ_WAIT_CANCEL_LIST:
    {
      const u32 select  = WBM_LIST_Main( p_wk->p_list );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        WBM_LIST_Exit( p_wk->p_list );
        switch( select )
        { 
        case 0: //�͂�
          p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
          WBM_SEQ_End( p_seqwk );
          break;
        case 1: //������
          *p_seq = SEQ_START_MSG;
          break;
        }
      }
    }
    break;

  case SEQ_WAIT_MSG: //���b�Z�[�W�҂�
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  �T�[�o��̃f�W�^���I��؃f�[�^���_�E�����[�h
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_RecvDigCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_SEARCH_MSG,
    SEQ_CHECK_GPF,

    SEQ_START_NONE_MSG,
    SEQ_WAIT_NONE_MSG,

    SEQ_START_DOWNLOAD_GPF_DATA,
    SEQ_WAIT_DOWNLOAD_GPF_DATA,
    SEQ_START_DOWNLOAD_SAKE_DATA,
    SEQ_WAIT_DOWNLOAD_SAKE_DATA,

    SEQ_END
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  switch( *p_seq )
  { 
  case SEQ_START_SEARCH_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_04, WBM_TEXT_TYPE_STREAM );
    *p_seq  = SEQ_CHECK_GPF;
    break;
    
  case SEQ_CHECK_GPF:
    { 
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data );
      DREAMWORLD_SAVEDATA *p_dream = DREAMWORLD_SV_GetDreamWorldSaveData( p_sv );
#ifdef PDF_FLAG_OFF
      *p_seq = SEQ_START_DOWNLOAD_GPF_DATA;
#else
      if( DREAMWORLD_SV_GetSignin( p_dream ) )
      { 
        *p_seq = SEQ_START_DOWNLOAD_GPF_DATA;
      }
      else
      { 
        *p_seq = SEQ_START_NONE_MSG;
      }
#endif
    }
    break;

  case SEQ_START_NONE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_05, WBM_TEXT_TYPE_STREAM );
    *p_seq  = SEQ_WAIT_NONE_MSG;
    break;

  case SEQ_WAIT_NONE_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    {
      WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_Join );
    }
    break;

  case SEQ_START_DOWNLOAD_GPF_DATA:
    *p_seq = SEQ_WAIT_DOWNLOAD_GPF_DATA;
    break;

  case SEQ_WAIT_DOWNLOAD_GPF_DATA:
    *p_seq = SEQ_START_DOWNLOAD_SAKE_DATA;
    break;

  case SEQ_START_DOWNLOAD_SAKE_DATA:
    *p_seq = SEQ_WAIT_DOWNLOAD_SAKE_DATA;
    break;

  case SEQ_WAIT_DOWNLOAD_SAKE_DATA:
    *p_seq = SEQ_END;
    break;

  case SEQ_END:
    WBM_SEQ_SetNext( p_seqwk, WbmWifiSeq_CheckDigCard );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  �f�W�^���I��؃f�[�^���`�F�b�N
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_CheckDigCard( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_CHECK_DATA,
    SEQ_CHECK_STATUS,
    SEQ_START_END_MSG,
    SEQ_START_GIVEUP_MSG,
    SEQ_START_EXIT_MSG,
    SEQ_NG_EXIT, //�I��؃`�F�b�N���O�ɂ��ďI��

    SEQ_START_DOWNLOAD_REG,
    SEQ_WAIT_DOWNLOAD_REG,

    SEQ_START_WRITE_GPF,
    SEQ_WAIT_WRITE_GPF,

    //
    SEQ_CHECK_RATING,
    SEQ_START_RENEWAL_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,

    

    SEQ_WAIT_MSG,
  };

  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  const REGULATION_CARDDATA *cp_reg_card  = SaveData_GetRegulationCardData(GAMEDATA_GetSaveControlWork( p_param->p_param->p_game_data ));

  switch( *p_seq )
  { 
  case SEQ_CHECK_DATA:

    //���NO�`�F�b�N
    if( p_wk->recv_card.no == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_CUPNO ) )
    { 
      //���NO�͈�v

      //�J�Ã��[�N�`�F�b�N
      if( p_wk->recv_card.status == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS ) )
      { 
        //��v���Ă���
        *p_seq  = SEQ_CHECK_STATUS;
      }
      else
      { 
        //��v���Ă��Ȃ�

        //�S�Ȃ�΁A�������݂ɍs��
        //����ȊO�Ȃ�Ώ㏑��
        if( REGULATION_CARD_STATUS_TYPE_GIVEUP == Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_STATUS ) )
        { 
          *p_seq  = SEQ_START_WRITE_GPF;
        }
        else
        { 
          *p_seq  = SEQ_CHECK_STATUS;
        }
      }
    }
    else
    { 
      //���NO�s��v

      //�J�Ã��[�N�`�F�b�N
      if( p_wk->recv_card.status == REGULATION_CARD_STATUS_TYPE_PRE )
      { 
        //���J�ÂȂ̂ŁA�擾��
        *p_seq  = SEQ_START_DOWNLOAD_REG;
      }
      else
      { 
        //�Q���ł�����Ȃ��̂Ń��b�Z�[�W���o���ďI��
        *p_seq  = SEQ_START_EXIT_MSG;
      }
    }

    break;

  case SEQ_CHECK_STATUS:
    if( p_wk->recv_card.status == REGULATION_CARD_STATUS_TYPE_END )
    { 
      *p_seq  = SEQ_START_END_MSG;
    }
    else if( p_wk->recv_card.status == REGULATION_CARD_STATUS_TYPE_GIVEUP )
    { 
      *p_seq  = SEQ_START_GIVEUP_MSG;
    }
    else
    { 
      *p_seq  = SEQ_START_DOWNLOAD_REG;
    }
    break;

  case SEQ_START_END_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_07, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_CHECK_RATING );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_GIVEUP_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_07, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_CHECK_RATING );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_EXIT_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_06, WBM_TEXT_TYPE_STREAM );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_NG_EXIT );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_NG_EXIT:
    //@todo
    break;

  case SEQ_START_DOWNLOAD_REG:
    *p_seq  = SEQ_WAIT_DOWNLOAD_REG;  // @todo
    break;

  case SEQ_WAIT_DOWNLOAD_REG:
    if( 1 )
    { 
      //�擾����
    }
    else
    { 
      //�擾���s
    }
    break;

  case SEQ_START_WRITE_GPF: //@todo
    *p_seq  = SEQ_WAIT_WRITE_GPF;
    break;

  case SEQ_WAIT_WRITE_GPF:
    *p_seq  = SEQ_START_GIVEUP_MSG;
    break;

  case SEQ_CHECK_RATING:
    if( 1 )  //���[�e�B���O�`�F�b�N @todo
    { 
      *p_seq  = SEQ_START_RENEWAL_MSG;
    }
    else
    { 
      *p_seq  = SEQ_NG_EXIT;
    }
    break;

  case SEQ_START_RENEWAL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_WIFI_STR_08, WBM_TEXT_TYPE_QUE );
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_SAVE );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_START_SAVE:
    *p_seq  = SEQ_WAIT_SAVE;
    break;

  case SEQ_WAIT_SAVE:
    if( 1 ) //@todo
    { 
      *p_seq  = SEQ_NG_EXIT;
    }
    break;


  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Register( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;
}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Start( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Matching( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_EndBattle( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_UnRegister( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

}
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_peq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void WbmWifiSeq_Err_ReturnLogin( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  WIFIBATTLEMATCH_WIFI_WORK	  *p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM  *p_param  = p_wk->p_param;

  p_param->result = WIFIBATTLEMATCH_CORE_RESULT_ERR_NEXT_LOGIN;
  WBM_SEQ_End( p_seqwk );
}
