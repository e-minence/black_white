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

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
//-------------------------------------
///	BG�t���[��
//=====================================
enum
{
	//���C�����
	BG_FRAME_M_TEXT	=	GFL_BG_FRAME0_M, 
	BG_FRAME_M_BACK	=	GFL_BG_FRAME1_M, 

	//�T�u���
	BG_FRAME_S_BACK	=	GFL_BG_FRAME1_M, 
} ;

//-------------------------------------
///	�p���b�g
//=====================================
enum
{
	//���C�����BG
	PLT_BG_M	  =	0,
  PLT_LIST_M  = 13,
  PLT_TEXT_M  = 14,
	PLT_FONT_M	= 15,

	//�T�u���BG
	PLT_BG_S	  = 0,
	PLT_FONT_S	= MATCHINFO_PLT_BG_FONT,

	//���C�����OBJ
	
	//�T�u���OBJ
} ;

//-------------------------------------
///	�L����
//=====================================
enum
{ 
  //���C�����  BG_FRAME_M_TEXT
  CGR_OFS_M_CLEAR = 0,
  CGR_OFS_M_LIST  = 1,
  CGR_OFS_M_TEXT  = 10,
};

//-------------------------------------
///	�V���N
//=====================================
#define ENEMYDATA_WAIT_SYNC (180)

//=============================================================================
/**
 *        ��`
*/
//=============================================================================
//-------------------------------------
///	���C�����[�N�O���錾
//=====================================
typedef struct _WIFIBATTLEMATCH_WORK WIFIBATTLEMATCH_WORK;

//-------------------------------------
///	  ���C���V�[�P���X��`
//=====================================
typedef BOOL (*WIFIBATTLEMATCH_MAINSEQ_FUNCTION)( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_seq );

//-------------------------------------
///	  �T�u�V�[�P���X��`
//=====================================
typedef int (*WIFIBATTLEMATCH_SUBSEQ_FUNCTION)( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_seq );

//-------------------------------------
///	���C�����[�N
//=====================================
struct _WIFIBATTLEMATCH_WORK
{
	//�O���t�B�b�N�ݒ�
	WIFIBATTLEMATCH_GRAPHIC_WORK	*p_graphic;

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
  WIFIBATTLEMATCH_MAINSEQ_FUNCTION  main_seq;

  //�T�u�V�[�P���X
  int sub_seq;
  int next_seq;
  int fade_seq;

  //�T�[�o�[����̎�M�o�b�t�@
  WIFIBATTLEMATCH_GDB_RND_SCORE_DATA rnd_score;

  //�E�G�C�g
  u32 cnt;
} ;

//=============================================================================
/**
 *					�v���g�^�C�v
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT WIFIBATTLEMATCH_CORE_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_CORE_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT WIFIBATTLEMATCH_CORE_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	���C���V�[�P���X
//    �i���[�h�ɂ��Ⴄ�j
//=====================================
static BOOL WifiBattleMatch_Random_MainSeq( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_seq );
static BOOL WifiBattleMatch_WiFi_MainSeq( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_seq );
static BOOL WifiBattleMatch_Live_MainSeq( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_seq );
static BOOL WifiBattleMatch_Debug_MainSeq( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_seq );

//-------------------------------------
///	�s�����̃`�F�b�N
//=====================================
static void WifiBattleMatch_Darty_ModifiEnemyData( WIFIBATTLEMATCH_ENEMYDATA *p_data );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	WifiBattleMaptchCore_ProcData =
{	
	WIFIBATTLEMATCH_CORE_PROC_Init,
	WIFIBATTLEMATCH_CORE_PROC_Main,
	WIFIBATTLEMATCH_CORE_PROC_Exit,
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
static GFL_PROC_RESULT WIFIBATTLEMATCH_CORE_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	WIFIBATTLEMATCH_WORK	*p_wk;
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;


	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));
	GFL_OVERLAY_Load( FS_OVERLAY_ID(dpw_common));

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_WIFIBATTLEMATCH_CORE, 0x30000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(WIFIBATTLEMATCH_WORK), HEAPID_WIFIBATTLEMATCH_CORE );
	GFL_STD_MemClear( p_wk, sizeof(WIFIBATTLEMATCH_WORK) );	


	//�O���t�B�b�N�ݒ�
	p_wk->p_graphic	= WIFIBATTLEMATCH_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_WIFIBATTLEMATCH_CORE );

	//���ʃ��W���[���̍쐬
	p_wk->p_font		= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr,
			GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_que			= PRINTSYS_QUE_Create( HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
												NARC_message_wifi_match_dat, HEAPID_WIFIBATTLEMATCH_CORE );
	p_wk->p_word	= WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_COUNTRY_BUFLEN, HEAPID_WIFIBATTLEMATCH_CORE );

	//�ǂ݂���
	{	
		ARCHANDLE	*	p_handle	= GFL_ARC_OpenDataHandle( ARCID_WIFIMATCH_GRA, HEAPID_WIFIBATTLEMATCH_CORE );

		//PLT
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifimatch_gra_bg_NCLR,
				PALTYPE_MAIN_BG, 0, 0, HEAPID_WIFIBATTLEMATCH_CORE );
		GFL_ARCHDL_UTIL_TransVramPalette( p_handle, NARC_wifimatch_gra_bg_NCLR,
				PALTYPE_SUB_BG, 0, 0, HEAPID_WIFIBATTLEMATCH_CORE );

		//CHR
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifimatch_gra_bg_back_NCGR, 
				GFL_BG_FRAME1_M, 0, 0, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
		GFL_ARCHDL_UTIL_TransVramBgCharacter( p_handle, NARC_wifimatch_gra_bg_back_NCGR, 
				GFL_BG_FRAME1_S, 0, 0, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );

		//SCR
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_wifimatch_gra_back_NSCR,
				GFL_BG_FRAME1_M, 0, 0, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
		GFL_ARCHDL_UTIL_TransVramScreen( p_handle, NARC_wifimatch_gra_back_NSCR,
				GFL_BG_FRAME1_S, 0, 0, FALSE, HEAPID_WIFIBATTLEMATCH_CORE );
	
		GFL_ARC_CloseDataHandle( p_handle );

		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr,
				PALTYPE_MAIN_BG, PLT_FONT_M*0x20, 0, HEAPID_WIFIBATTLEMATCH_CORE );
		GFL_ARC_UTIL_TransVramPalette( ARCID_FONT, NARC_font_default_nclr,
				PALTYPE_SUB_BG, PLT_FONT_S*0x20, 0, HEAPID_WIFIBATTLEMATCH_CORE );

    
    GFL_BG_FillCharacter( BG_FRAME_M_TEXT, 0, 1,  CGR_OFS_M_CLEAR );
    BmpWinFrame_GraphicSet( BG_FRAME_M_TEXT, CGR_OFS_M_LIST, PLT_LIST_M, MENU_TYPE_SYSTEM, HEAPID_WIFIBATTLEMATCH_CORE );
    TalkWinFrame_GraphicSet( BG_FRAME_M_TEXT, CGR_OFS_M_TEXT, PLT_TEXT_M, MENU_TYPE_SYSTEM, HEAPID_WIFIBATTLEMATCH_CORE );
	}

	//���W���[���̍쐬
	{	
    DWCUserData *p_user_data;
    WIFI_LIST   *p_wifilist;
    p_wifilist  = GAMEDATA_GetWiFiList( p_param->p_param->p_game_data );
    p_user_data = WifiList_GetMyUserInfo( p_wifilist );

    p_wk->p_net   = WIFIBATTLEMATCH_NET_Init( p_user_data, p_wifilist, HEAPID_WIFIBATTLEMATCH_CORE );
    p_wk->p_text  = WBM_TEXT_Init( BG_FRAME_M_TEXT, PLT_FONT_M, PLT_TEXT_M, CGR_OFS_M_TEXT, HEAPID_WIFIBATTLEMATCH_CORE );

	}
  //���C���V�[�P���X
  p_wk->main_seq  = WifiBattleMatch_Random_MainSeq;
	
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
static GFL_PROC_RESULT WIFIBATTLEMATCH_CORE_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum 
  { 
    SEQ_START_DISCONNECT,
    SEQ_WAIT_DISCONNECT,
  };

	WIFIBATTLEMATCH_WORK	      *p_wk	= p_wk_adrs;
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

  //���\�[�X���
  GFL_BG_FillCharacterRelease(BG_FRAME_M_TEXT, 1, CGR_OFS_M_CLEAR );

	//���ʃ��W���[���̔j��
	WORDSET_Delete( p_wk->p_word );
	GFL_MSG_Delete( p_wk->p_msg );
	PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_font );

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
static GFL_PROC_RESULT WIFIBATTLEMATCH_CORE_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_MAIN,
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
  };


	WIFIBATTLEMATCH_WORK	*p_wk	    = p_wk_adrs;
  WIFIBATTLEMATCH_CORE_PARAM *p_param  = p_param_adrs;


  switch( p_wk->fade_seq )
  { 
  case SEQ_FADEIN_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 16, 0, 0 );
      p_wk->fade_seq  = SEQ_FADEIN_WAIT;
    break;
  case SEQ_FADEIN_WAIT:
		if( !GFL_FADE_CheckFade() )
		{
      p_wk->fade_seq  = SEQ_MAIN;
    }
    break;
  case SEQ_MAIN:
    //���C���V�[�P���X
    if( p_wk->main_seq( p_wk, p_param, p_seq ) )
    { 
      p_wk->fade_seq  = SEQ_FADEOUT_START;
    }

    //���ʂ̃G���[�����iSC,GDB�ȊO�j
    if( WIFIBATTLEMATCH_NET_CheckError( p_wk->p_net ) )
    { 
      p_param->result = WIFIBATTLEMATCH_CORE_RESULT_ERR_NEXT_LOGIN;
      p_wk->fade_seq  = SEQ_FADEOUT_START;
    }
    break;

  case SEQ_FADEOUT_START:
		GFL_FADE_SetMasterBrightReq( GFL_FADE_MASTER_BRIGHT_BLACKOUT, 0, 16, 0 );
    p_wk->fade_seq  = SEQ_FADEOUT_WAIT;
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
 *    �����_���}�b�`�̏���
 */
//=============================================================================
//-------------------------------------
///	�����_���}�b�`�V�[�P���X
//=====================================
typedef enum
{
  WIFIBATTLEMATCH_RND_SUBSEQ_INIT,         //������
  WIFIBATTLEMATCH_RND_SUBSEQ_START,         //�J�n
  WIFIBATTLEMATCH_RND_SUBSEQ_RATE_START,    //���[�e�B���O�J�n
  WIFIBATTLEMATCH_RND_SUBSEQ_RATE_MATCHING, //���[�e�B���O�}�b�`���O
  WIFIBATTLEMATCH_RND_SUBSEQ_RATE_BTL_END,  //���[�e�B���O�o�g���I����
  WIFIBATTLEMATCH_RND_SUBSEQ_FREE_START,    //�t���[�J�n
  WIFIBATTLEMATCH_RND_SUBSEQ_FREE_MATCHING, //�t���[�}�b�`���O
  WIFIBATTLEMATCH_RND_SUBSEQ_FREE_BTL_END,  //�t���[�o�g���I����

  WIFIBATTLEMATCH_RND_SUBSEQ_ERR_RETURN_LOGIN,  //���O�C���܂ł��ǂ�

  WIFIBATTLEMATCH_RND_SUBSEQ_MAX,
  WIFIBATTLEMATCH_RND_SUBSEQ_EXIT = WIFIBATTLEMATCH_RND_SUBSEQ_MAX,

} WIFIBATTLEMATCH_RND_SUBSEQ;

//-------------------------------------
///	�����_���}�b�`�V�[�P���X�֐�
//=====================================
static int WIFIBATTLEMATCH_RND_SUBSEQ_Init( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq );
static int WIFIBATTLEMATCH_RND_SUBSEQ_Start( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq );
//���[�e�B���O����
static int WIFIBATTLEMATCH_RND_SUBSEQ_Rate_Start( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq );
static int WIFIBATTLEMATCH_RND_SUBSEQ_Rate_Matching( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq );
static int WIFIBATTLEMATCH_RND_SUBSEQ_Rate_EndBattle( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq );
//�t���[����
static int WIFIBATTLEMATCH_RND_SUBSEQ_Free_Start( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq );
static int WIFIBATTLEMATCH_RND_SUBSEQ_Free_Matching( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq );
static int WIFIBATTLEMATCH_RND_SUBSEQ_Free_EndBattle( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq );

static int WIFIBATTLEMATCH_RND_SUBSEQ_Err_ReturnLogin( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq );

//-------------------------------------
///	�����_���}�b�`�V�[�P���X�֐��e�[�u��
//=====================================
static const WIFIBATTLEMATCH_SUBSEQ_FUNCTION  sc_rnd_subseq[WIFIBATTLEMATCH_RND_SUBSEQ_MAX]  =
{ 
  WIFIBATTLEMATCH_RND_SUBSEQ_Init,
  WIFIBATTLEMATCH_RND_SUBSEQ_Start,
  WIFIBATTLEMATCH_RND_SUBSEQ_Rate_Start,
  WIFIBATTLEMATCH_RND_SUBSEQ_Rate_Matching,
  WIFIBATTLEMATCH_RND_SUBSEQ_Rate_EndBattle,
  WIFIBATTLEMATCH_RND_SUBSEQ_Free_Start,
  WIFIBATTLEMATCH_RND_SUBSEQ_Free_Matching,
  WIFIBATTLEMATCH_RND_SUBSEQ_Free_EndBattle,
  WIFIBATTLEMATCH_RND_SUBSEQ_Err_ReturnLogin,
};

//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  ���C���V�[�P���X
 *
 *	@param	WIFIBATTLEMATCH_WORK  *p_wk     ���[�N
 *	@param  WIFIBATTLEMATCH_CORE_PARAM *p_param  ����
 *	@param	int *p_seq                      �V�[�P���X
 *
 *	@return TRUE�ŏI��  FALSE�Ń��[�v
 */
//-----------------------------------------------------------------------------
static BOOL WifiBattleMatch_Random_MainSeq( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_seq )
{ 
  *p_seq  = sc_rnd_subseq[ *p_seq ]( p_wk, p_param, &p_wk->sub_seq );

  if( *p_seq  == WIFIBATTLEMATCH_RND_SUBSEQ_EXIT )
  { 
    return TRUE;
  }
  else
  { 
    return  FALSE;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`������
 *
 *	@param	WIFIBATTLEMATCH_WORK *p_wk  ���[�N
 *	@param  p_param                     ����
 *	@param	int *p_subseq                  ���݂̃T�u�V�[�P���X
 *
 *	@return ���̃��C���V�[�P���X
 */
//-----------------------------------------------------------------------------
static int WIFIBATTLEMATCH_RND_SUBSEQ_Init( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq )
{ 

  NAGI_Printf( " mode%d ret%d\n", p_param->mode, p_param->retmode );
  switch( p_param->mode )
  { 
  case WIFIBATTLEMATCH_CORE_MODE_START:
    *p_subseq = 0;
    return WIFIBATTLEMATCH_RND_SUBSEQ_START;

  case WIFIBATTLEMATCH_CORE_MODE_ENDBATTLE:
    if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_RATE )
    { 
      *p_subseq = 0;
      return WIFIBATTLEMATCH_RND_SUBSEQ_RATE_BTL_END;
    }
    else if( p_param->retmode == WIFIBATTLEMATCH_CORE_RETMODE_FREE )
    { 
      *p_subseq = 0;
      return WIFIBATTLEMATCH_RND_SUBSEQ_FREE_BTL_END;
    }
  }

  GF_ASSERT( 0 );
  return 0;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`�J�n
 *
 *	@param	WIFIBATTLEMATCH_WORK *p_wk  ���[�N
 *	@param  p_param                     ����
 *	@param	int *p_subseq                  ���݂̃T�u�V�[�P���X
 *
 *	@return ���̃��C���V�[�P���X
 */
//-----------------------------------------------------------------------------
static int WIFIBATTLEMATCH_RND_SUBSEQ_Start( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq )
{ 
  enum
  { 
    SEQ_START_INIT,
    SEQ_WAIT_INIT,
    SEQ_START_RECVDATA_SAKE,
    SEQ_WAIT_RECVDATA_SAKE,
    SEQ_CHECK_GPF,
    SEQ_SELECT_MSG,
    SEQ_START_SELECT_MODE,
    SEQ_WAIT_SELECT_MODE,
    SEQ_START_CANCEL_MSG,
    SEQ_START_CANCEL_SELECT,
    SEQ_WAIT_CANCEL_SELECT,


    SEQ_WAIT_MSG,
  };

  switch( *p_subseq )
  { 
    //-------------------------------------
    ///	������
    //=====================================
  case SEQ_START_INIT:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, p_wk->p_font );
    WIFIBATTLEMATCH_NET_StartInitialize( p_wk->p_net );
    *p_subseq = SEQ_WAIT_INIT;
    break;
    
  case SEQ_WAIT_INIT:
    { 
      DWCGdbError error;
      SAVE_CONTROL_WORK *p_save = GAMEDATA_GetSaveControlWork(p_param->p_param->p_game_data);
      if( WIFIBATTLEMATCH_NET_WaitInitialize( p_wk->p_net, p_save, &error ) )
      { 
        *p_subseq = SEQ_START_RECVDATA_SAKE;
      }
      else
      { 
        WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE type;
        type = WIFIBATTLEMATCH_GDB_GetErrorRepairType( error );
        switch( type )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY:       //������x
          *p_subseq = SEQ_START_INIT;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          return WIFIBATTLEMATCH_RND_SUBSEQ_ERR_RETURN_LOGIN;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_FATAL:       //�d���ؒf
          NetErr_DispCallFatal();
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	�����f�[�^��M
    //=====================================
  case SEQ_START_RECVDATA_SAKE:
    *p_subseq       = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_WAIT_RECVDATA_SAKE;
    break;

  case SEQ_WAIT_RECVDATA_SAKE:
    *p_subseq       = SEQ_CHECK_GPF;
    break;

  case SEQ_CHECK_GPF:
    if( 1 )
    { 
      *p_subseq = SEQ_SELECT_MSG;
    }
    else
    { 
      *p_subseq = SEQ_SELECT_MSG;
    }
    break;

    //-------------------------------------
    ///	���[�h�I��
    //=====================================
  case SEQ_SELECT_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_001, p_wk->p_font );
    *p_subseq       = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_START_SELECT_MODE;
    break;

  case SEQ_START_SELECT_MODE:
    { 
      WBM_LIST_SETUP  setup;
      GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
      setup.p_msg   = p_wk->p_msg;
      setup.p_font  = p_wk->p_font;
      setup.p_que   = p_wk->p_que;
      setup.strID[0]= WIFIMATCH_TEXT_003;
      setup.strID[1]= WIFIMATCH_TEXT_004;
      setup.strID[2]= WIFIMATCH_TEXT_005;
      setup.list_max= 3;
      setup.frm     = BG_FRAME_M_TEXT;
      setup.font_plt= PLT_FONT_M;
      setup.frm_plt = PLT_LIST_M;
      setup.frm_chr = CGR_OFS_M_LIST;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_subseq     = SEQ_WAIT_SELECT_MODE;
    }
    break;

  case SEQ_WAIT_SELECT_MODE:
    {
      const u32 select  = WBM_LIST_Main( p_wk->p_list );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        WBM_LIST_Exit( p_wk->p_list );
        switch( select )
        { 
        case 0:
          *p_subseq = 0;
          p_param->retmode = WIFIBATTLEMATCH_CORE_RETMODE_FREE;
          return WIFIBATTLEMATCH_RND_SUBSEQ_FREE_START;
        case 1:
          *p_subseq = 0;
          p_param->retmode = WIFIBATTLEMATCH_CORE_RETMODE_RATE;
          return WIFIBATTLEMATCH_RND_SUBSEQ_RATE_START;
        case 2:
          *p_subseq = SEQ_START_CANCEL_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	��߂�
    //=====================================
  case SEQ_START_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_006, p_wk->p_font );
    *p_subseq       = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_START_CANCEL_SELECT;
    break;

  case SEQ_START_CANCEL_SELECT:
    { 
      WBM_LIST_SETUP  setup;
      GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
      setup.p_msg   = p_wk->p_msg;
      setup.p_font  = p_wk->p_font;
      setup.p_que   = p_wk->p_que;
      setup.strID[0]= WIFIMATCH_TEXT_007;
      setup.strID[1]= WIFIMATCH_TEXT_008;
      setup.list_max= 2;
      setup.frm     = BG_FRAME_M_TEXT;
      setup.font_plt= PLT_FONT_M;
      setup.frm_plt = PLT_LIST_M;
      setup.frm_chr = CGR_OFS_M_LIST;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_subseq     = SEQ_WAIT_CANCEL_SELECT;
    }
    break;

  case SEQ_WAIT_CANCEL_SELECT:
    {
      const u32 select  = WBM_LIST_Main( p_wk->p_list );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        WBM_LIST_Exit( p_wk->p_list );
        switch( select )
        { 
        case 0:
          p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
          return WIFIBATTLEMATCH_RND_SUBSEQ_EXIT; 

        case 1:
          *p_subseq = SEQ_SELECT_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	����
    //=====================================
  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      *p_subseq = p_wk->next_seq;
    }
    break;
  };

  //���̂܂�
  return WIFIBATTLEMATCH_RND_SUBSEQ_START;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  ���[�e�B���O�J�n
 *
 *	@param	WIFIBATTLEMATCH_WORK *p_wk  ���[�N
 *	@param  p_param                     ����
 *	@param	int *p_subseq                  ���݂̃T�u�V�[�P���X
 *
 *	@return ���̃��C���V�[�P���X
 */
//-----------------------------------------------------------------------------
static int WIFIBATTLEMATCH_RND_SUBSEQ_Rate_Start( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq )
{ 
  enum
  { 
    SEQ_START_RECVRATE_SAKE,
    SEQ_WAIT_RECVRATE_SAKE,
    SEQ_START_POKECHECK_SERVER,
    SEQ_WAIT_POKECHECK_SERVER,
    SEQ_CHECK_POKE,

    SEQ_WAIT_MSG,
  };

  switch( *p_subseq )
  { 
    //-------------------------------------
    ///	���[�e�B���O�f�[�^��M
    //=====================================
  case SEQ_START_RECVRATE_SAKE:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_000, p_wk->p_font );
    WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, WIFIBATTLEMATCH_GDB_GET_RND_SCORE, &p_wk->rnd_score );
    *p_subseq       = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_WAIT_RECVRATE_SAKE;
    break;

  case SEQ_WAIT_RECVRATE_SAKE:
    { 
      DWCGdbError result  = DWC_GDB_ERROR_NONE;
      if( WIFIBATTLEMATCH_GDB_Process( p_wk->p_net, &result ) )
      { 

        //�����̃f�[�^��\��
        PLAYERINFO_RANDOMMATCH_DATA info_setup;
        GFL_CLUNIT	*p_unit;

        p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );

        GFL_STD_MemClear( &info_setup, sizeof(PLAYERINFO_RANDOMMATCH_DATA) );
        info_setup.btl_rule = p_param->p_param->btl_rule;
        info_setup.rate     = p_wk->rnd_score.single_rate;
        info_setup.win_cnt  = p_wk->rnd_score.single_win;
        info_setup.lose_cnt = p_wk->rnd_score.single_lose;
        info_setup.btl_cnt  = p_wk->rnd_score.single_win + p_wk->rnd_score.single_lose; //@todo
        if( p_wk->p_playerinfo )
        { 
          PLAYERINFO_RND_Exit( p_wk->p_playerinfo );
          p_wk->p_playerinfo  = NULL;
        }
        {
          MYSTATUS  *p_my;
          p_my  = GAMEDATA_GetMyStatus(p_param->p_param->p_game_data); 
          p_wk->p_playerinfo	= PLAYERINFO_RND_Init( &info_setup, TRUE, p_my, p_unit, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, HEAPID_WIFIBATTLEMATCH_CORE );
        }

        //�����̃��[�g��ۑ�
        { 
          WIFIBATTLEMATCH_ENEMYDATA *p_player;
          p_player  = p_param->p_player_data;
          p_player->rate  = p_wk->rnd_score.single_rate;
        }


        *p_subseq       = SEQ_START_POKECHECK_SERVER;
      }
      else
      { 
        WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE type;
        type = WIFIBATTLEMATCH_GDB_GetErrorRepairType( result );
        switch( type )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY:       //������x
          *p_subseq = SEQ_START_RECVRATE_SAKE;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          return WIFIBATTLEMATCH_RND_SUBSEQ_ERR_RETURN_LOGIN;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_FATAL:       //�d���ؒf
          NetErr_DispCallFatal();
          break;
        }
      }
    }
    break;

    //-------------------------------------
    /// �|�P�����s���`�F�b�N
    //=====================================
  case SEQ_START_POKECHECK_SERVER:
    *p_subseq       = SEQ_WAIT_POKECHECK_SERVER;
    break;

  case SEQ_WAIT_POKECHECK_SERVER:
    *p_subseq       = SEQ_CHECK_POKE;
    break;

  case SEQ_CHECK_POKE:
    *p_subseq = 0;
    return WIFIBATTLEMATCH_RND_SUBSEQ_RATE_MATCHING;

    //-------------------------------------
    ///	����
    //=====================================
  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      *p_subseq = p_wk->next_seq;
    }
    break;

  }

  //���̂܂�
  return WIFIBATTLEMATCH_RND_SUBSEQ_RATE_START;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  ���[�e�B���O�̃}�b�`���O
 *
 *	@param	WIFIBATTLEMATCH_WORK *p_wk  ���[�N
 *	@param  p_param                     ����
 *	@param	int *p_subseq                  ���݂̃T�u�V�[�P���X
 *
 *	@return ���̃��C���V�[�P���X
 */
//-----------------------------------------------------------------------------
static int WIFIBATTLEMATCH_RND_SUBSEQ_Rate_Matching( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq )
{ 
  enum
  { 
    SEQ_START_MATCHING,
    SEQ_START_MATCHING_MSG,
    SEQ_WAIT_MATCHING,
    SEQ_START_CHECK_CHEAT,
    SEQ_WAIT_CHECK_CHEAT,
    SEQ_START_SENDDATA,
    SEQ_WAIT_SENDDATA,
    SEQ_OK_MATCHING_MSG,
    SEQ_OK_MATCHING_WAIT,
    SEQ_END_MATCHING_MSG,
    SEQ_END_MATCHING,

    SEQ_START_SELECT_CANCEL_MSG,
    SEQ_START_SELECT_CANCEL,
    SEQ_WAIT_SELECT_CANCEL,

    SEQ_WAIT_MSG,
  };

  switch( *p_subseq )
  { 
    //-------------------------------------
    ///	�}�b�`���O�J�n
    //=====================================
  case SEQ_START_MATCHING:
    WIFIBATTLEMATCH_NET_StartMatchMake( p_wk->p_net, p_param->p_param->btl_rule ); 
    *p_subseq = SEQ_START_MATCHING_MSG;
    break;

  case SEQ_START_MATCHING_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_009, p_wk->p_font );
    *p_subseq = SEQ_WAIT_MATCHING;
    break;

  case SEQ_WAIT_MATCHING:
    if( WIFIBATTLEMATCH_NET_GetSeqMatchMake( p_wk->p_net ) < WIFIBATTLEMATCH_NET_SEQ_CONNECT_START )
    { 
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
      { 
        *p_subseq = SEQ_START_SELECT_CANCEL_MSG;
      }
    }

    if( WIFIBATTLEMATCH_NET_WaitMatchMake( p_wk->p_net ) )
    { 
      *p_subseq = SEQ_START_CHECK_CHEAT;
    }
    break;

    //-------------------------------------
    ///	����������̑���̕s���`�F�b�N
    //=====================================
  case SEQ_START_CHECK_CHEAT:


    *p_subseq = SEQ_WAIT_CHECK_CHEAT;
    break;

  case SEQ_WAIT_CHECK_CHEAT:
    *p_subseq = SEQ_START_SENDDATA;
    break;

  case SEQ_START_SENDDATA:
    { 
      WIFIBATTLEMATCH_ENEMYDATA *p_my_data = p_param->p_player_data;
      if( WIFIBATTLEMATCH_NET_StartEnemyData( p_wk->p_net, p_my_data ) )
      { 
        *p_subseq       = SEQ_WAIT_SENDDATA;
      }
    }
    break;

  case SEQ_WAIT_SENDDATA:
    {
      WIFIBATTLEMATCH_ENEMYDATA *p_recv_data;
      if( WIFIBATTLEMATCH_NET_WaitEnemyData( p_wk->p_net, &p_recv_data ) )
      { 
        GFL_CLUNIT  *p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
        WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data = p_param->p_enemy_data;
        GFL_STD_MemCopy( p_recv_data, p_enemy_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );

        WifiBattleMatch_Darty_ModifiEnemyData( p_enemy_data );

        if( p_wk->p_matchinfo )
        { 
          MATCHINFO_Exit( p_wk->p_matchinfo );
        }
        { 
          p_wk->p_matchinfo		= MATCHINFO_Init( p_enemy_data, p_unit, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, HEAPID_WIFIBATTLEMATCH_CORE );
        }

        *p_subseq       = SEQ_OK_MATCHING_MSG;
      }
    }
    break;

  case SEQ_OK_MATCHING_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_010, p_wk->p_font );
    *p_subseq       = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_OK_MATCHING_WAIT;
    break;

  case SEQ_OK_MATCHING_WAIT:
    if( p_wk->cnt++ > ENEMYDATA_WAIT_SYNC )
    { 
      p_wk->cnt      = 0;
      *p_subseq      = SEQ_END_MATCHING_MSG;
    }
    break;

  case SEQ_END_MATCHING_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_012, p_wk->p_font );
    *p_subseq = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_END_MATCHING;
    break;

  case SEQ_END_MATCHING:
    p_param->result = WIFIBATTLEMATCH_CORE_RESULT_NEXT_BATTLE;
    return WIFIBATTLEMATCH_RND_SUBSEQ_EXIT; 

    //-------------------------------------
    ///	�L�����Z������
    //=====================================
  case SEQ_START_SELECT_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_011, p_wk->p_font );
    *p_subseq       = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_START_SELECT_CANCEL;
    break;

  case SEQ_START_SELECT_CANCEL:
    { 
      WBM_LIST_SETUP  setup;
      GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
      setup.p_msg   = p_wk->p_msg;
      setup.p_font  = p_wk->p_font;
      setup.p_que   = p_wk->p_que;
      setup.strID[0]= WIFIMATCH_TEXT_007;
      setup.strID[1]= WIFIMATCH_TEXT_008;
      setup.list_max= 2;
      setup.frm     = BG_FRAME_M_TEXT;
      setup.font_plt= PLT_FONT_M;
      setup.frm_plt = PLT_LIST_M;
      setup.frm_chr = CGR_OFS_M_LIST;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_subseq     = SEQ_WAIT_SELECT_CANCEL;
    }
    break;

  case SEQ_WAIT_SELECT_CANCEL:
    {
      const u32 select  = WBM_LIST_Main( p_wk->p_list );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        WBM_LIST_Exit( p_wk->p_list );
        switch( select )
        { 
        case 0:
          p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
          return WIFIBATTLEMATCH_RND_SUBSEQ_EXIT; //�I��
        case 1:
          *p_subseq = SEQ_START_MATCHING_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	����
    //=====================================
  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      *p_subseq = p_wk->next_seq;
    }
    break;
  }

  //���̂܂�
  return WIFIBATTLEMATCH_RND_SUBSEQ_RATE_MATCHING;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  ���[�e�B���O�̐퓬�I����
 *
 *	@param	WIFIBATTLEMATCH_WORK *p_wk  ���[�N
 *	@param  p_param                     ����
 *	@param	int *p_subseq                  ���݂̃T�u�V�[�P���X
 *
 *	@return ���̃��C���V�[�P���X
 */
//-----------------------------------------------------------------------------
static int WIFIBATTLEMATCH_RND_SUBSEQ_Rate_EndBattle( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq )
{ 
 
  enum
  { 
    SEQ_START_MSG,
    SEQ_START_REPORT_ATLAS,
    SEQ_WAIT_REPORT_ATLAS,

    SEQ_START_RECVDATA_SAKE,
    SEQ_WAIT_RECVDATA_SAKE,

    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,

    SEQ_START_SELECT_BTLREC_MSG,
    SEQ_START_SELECTBTLREC,
    SEQ_WAIT_SELECTBTLREC,

    SEQ_START_DISCONNECT,
    SEQ_WAIT_DISCONNECT,

    SEQ_START_SELECT_CONTINUE_MSG,
    SEQ_START_SELECT_CONTINUE,
    SEQ_WAIT_SELECT_CONTINUE,

    SEQ_START_SELECT_CANCEL_MSG,
    SEQ_START_SELECT_CANCEL,
    SEQ_WAIT_SELECT_CANCEL,

    SEQ_WAIT_MSG,
  };

  switch( *p_subseq )
  { 
    //-------------------------------------
    ///	Atlas�ւ̏�������
    //=====================================
  case SEQ_START_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_013, p_wk->p_font );
    *p_subseq       = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_START_REPORT_ATLAS;
    break;
  case SEQ_START_REPORT_ATLAS:
    WIFIBATTLEMATCH_SC_Start( p_wk->p_net, p_param->p_param->btl_rule, p_param->btl_result );
    *p_subseq = SEQ_WAIT_REPORT_ATLAS;
    break;
  case SEQ_WAIT_REPORT_ATLAS:
    { 
      DWCScResult result;
      if( WIFIBATTLEMATCH_SC_Process( p_wk->p_net, &result ) )
      { 
        NAGI_Printf( "��������I%d\n", result );
        *p_subseq = SEQ_START_RECVDATA_SAKE;
      }
      else
      { 
        WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE type;
        type = WIFIBATTLEMATCH_SC_GetErrorRepairType( result );
        switch( type )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY:       //������x
          *p_subseq = SEQ_START_REPORT_ATLAS;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          return WIFIBATTLEMATCH_RND_SUBSEQ_ERR_RETURN_LOGIN;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_FATAL:       //�d���ؒf
          NetErr_DispCallFatal();
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	Sake����擾
    //=====================================
  case SEQ_START_RECVDATA_SAKE:
    WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, WIFIBATTLEMATCH_GDB_GET_RND_SCORE, &p_wk->rnd_score );
    *p_subseq = SEQ_WAIT_RECVDATA_SAKE;
    break;
  case SEQ_WAIT_RECVDATA_SAKE:
    { 
      DWCGdbError result  = DWC_GDB_ERROR_NONE;
      if( WIFIBATTLEMATCH_GDB_Process( p_wk->p_net, &result ) )
      { 
        PLAYERINFO_RANDOMMATCH_DATA info_setup;
        GFL_CLUNIT	*p_unit;

        p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );

        GFL_STD_MemClear( &info_setup, sizeof(PLAYERINFO_RANDOMMATCH_DATA) );
        info_setup.btl_rule = p_param->p_param->btl_rule;
        info_setup.rate     = p_wk->rnd_score.single_rate;
        info_setup.win_cnt  = p_wk->rnd_score.single_win;
        info_setup.lose_cnt = p_wk->rnd_score.single_lose;
        info_setup.btl_cnt  = p_wk->rnd_score.single_win + p_wk->rnd_score.single_lose; //@todo

        //�Z�[�u�f�[�^
        RNDMATCH_SetParam( p_param->p_rndmatch, RNDMATCH_TYPE_RATE_SINGLE + info_setup.btl_rule, RNDMATCH_PARAM_IDX_RATE, info_setup.rate );
        RNDMATCH_SetParam( p_param->p_rndmatch, RNDMATCH_TYPE_RATE_SINGLE + info_setup.btl_rule, RNDMATCH_PARAM_IDX_WIN, info_setup.win_cnt );
        RNDMATCH_SetParam( p_param->p_rndmatch, RNDMATCH_TYPE_RATE_SINGLE + info_setup.btl_rule, RNDMATCH_PARAM_IDX_LOSE, info_setup.btl_cnt );

        if( p_wk->p_playerinfo )
        { 
          PLAYERINFO_RND_Exit( p_wk->p_playerinfo );
          p_wk->p_playerinfo  = NULL;
        }

        {
          MYSTATUS  *p_my;
          p_my  = GAMEDATA_GetMyStatus(p_param->p_param->p_game_data); 
          p_wk->p_playerinfo	= PLAYERINFO_RND_Init( &info_setup, TRUE, p_my, p_unit, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, HEAPID_WIFIBATTLEMATCH_CORE );
        }

        *p_subseq       = SEQ_START_DISCONNECT;
      }
      else
      { 
        WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE type;
        type = WIFIBATTLEMATCH_GDB_GetErrorRepairType( result );
        switch( type )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY:       //������x
          *p_subseq = SEQ_START_RECVDATA_SAKE;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          return WIFIBATTLEMATCH_RND_SUBSEQ_ERR_RETURN_LOGIN;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_FATAL:       //�d���ؒf
          NetErr_DispCallFatal();
          break;
        }
      }
    }
    break;

    //-------------------------------------
    /// �ؒf����
    //=====================================
  case SEQ_START_DISCONNECT:
    *p_subseq = SEQ_WAIT_DISCONNECT;
    break;

  case SEQ_WAIT_DISCONNECT:
    if( WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE ) )
    { 
      *p_subseq = SEQ_START_SELECT_BTLREC_MSG;
    }
    break;

    //-------------------------------------
    /// ���[�e�B���O�Z�[�u����
    //=====================================
  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, p_wk->p_font );
    *p_subseq       = SEQ_START_SAVE;
    break;
  case SEQ_START_SAVE:
    GAMEDATA_SaveAsyncStart(p_param->p_param->p_game_data);
    *p_subseq       = SEQ_WAIT_SAVE;
    break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT result;
      result = GAMEDATA_SaveAsyncMain(p_param->p_param->p_game_data);
      switch(result){
      case SAVE_RESULT_CONTINUE:
      case SAVE_RESULT_LAST:
        break;
      case SAVE_RESULT_OK:
      case SAVE_RESULT_NG:
        *p_subseq       = SEQ_START_SELECT_BTLREC_MSG;
        break;
      }
    }
    break;

    //-------------------------------------
    /// �^��m�F
    //=====================================
  case SEQ_START_SELECT_BTLREC_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_014, p_wk->p_font );
    *p_subseq = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_START_SELECTBTLREC;
    break;

  case SEQ_START_SELECTBTLREC:
    { 
      WBM_LIST_SETUP  setup;
      GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
      setup.p_msg   = p_wk->p_msg;
      setup.p_font  = p_wk->p_font;
      setup.p_que   = p_wk->p_que;
      setup.strID[0]= WIFIMATCH_TEXT_007;
      setup.strID[1]= WIFIMATCH_TEXT_008;
      setup.list_max= 2;
      setup.frm     = BG_FRAME_M_TEXT;
      setup.font_plt= PLT_FONT_M;
      setup.frm_plt = PLT_LIST_M;
      setup.frm_chr = CGR_OFS_M_LIST;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_subseq     = SEQ_WAIT_SELECTBTLREC;
    }
    break;

  case SEQ_WAIT_SELECTBTLREC:
    {
      const u32 select  = WBM_LIST_Main( p_wk->p_list );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        WBM_LIST_Exit( p_wk->p_list );
        switch( select )
        { 
        case 0:
          *p_subseq = SEQ_START_SELECT_CONTINUE_MSG;
          break;
        case 1:
          *p_subseq = SEQ_START_SELECT_CONTINUE_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    /// �ΐ푱�s�m�F
    //=====================================
  case SEQ_START_SELECT_CONTINUE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_015, p_wk->p_font );
    *p_subseq = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_START_SELECT_CONTINUE;
    break;
  case SEQ_START_SELECT_CONTINUE:
    { 
      WBM_LIST_SETUP  setup;
      GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
      setup.p_msg   = p_wk->p_msg;
      setup.p_font  = p_wk->p_font;
      setup.p_que   = p_wk->p_que;
      setup.strID[0]= WIFIMATCH_TEXT_007;
      setup.strID[1]= WIFIMATCH_TEXT_008;
      setup.list_max= 2;
      setup.frm     = BG_FRAME_M_TEXT;
      setup.font_plt= PLT_FONT_M;
      setup.frm_plt = PLT_LIST_M;
      setup.frm_chr = CGR_OFS_M_LIST;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_subseq     = SEQ_WAIT_SELECT_CONTINUE;
    }
    break;
  case SEQ_WAIT_SELECT_CONTINUE:
    {
      const u32 select  = WBM_LIST_Main( p_wk->p_list );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        WBM_LIST_Exit( p_wk->p_list );
        switch( select )
        { 
        case 0:
          *p_subseq = 0;
          return WIFIBATTLEMATCH_RND_SUBSEQ_RATE_START;
        case 1:
          *p_subseq = SEQ_START_SELECT_CANCEL_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    /// �����_���}�b�`�I���m�F
    //=====================================
  case SEQ_START_SELECT_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_016, p_wk->p_font );
    *p_subseq = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_START_SELECT_CANCEL;
    break;
  case SEQ_START_SELECT_CANCEL:
    { 
      WBM_LIST_SETUP  setup;
      GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
      setup.p_msg   = p_wk->p_msg;
      setup.p_font  = p_wk->p_font;
      setup.p_que   = p_wk->p_que;
      setup.strID[0]= WIFIMATCH_TEXT_007;
      setup.strID[1]= WIFIMATCH_TEXT_008;
      setup.list_max= 2;
      setup.frm     = BG_FRAME_M_TEXT;
      setup.font_plt= PLT_FONT_M;
      setup.frm_plt = PLT_LIST_M;
      setup.frm_chr = CGR_OFS_M_LIST;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_subseq     = SEQ_WAIT_SELECT_CANCEL;
    }
    break;
  case SEQ_WAIT_SELECT_CANCEL:
    {
      const u32 select  = WBM_LIST_Main( p_wk->p_list );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        WBM_LIST_Exit( p_wk->p_list );
        switch( select )
        { 
        case 0:
          p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
          return WIFIBATTLEMATCH_RND_SUBSEQ_EXIT; //�I��
        case 1:
          *p_subseq = SEQ_START_SELECT_CONTINUE_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	����
    //=====================================
  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      *p_subseq = p_wk->next_seq;
    }
    break;
  }


  //���̂܂�
  return WIFIBATTLEMATCH_RND_SUBSEQ_RATE_BTL_END;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  �t���[���[�h�J�n
 *
 *	@param	WIFIBATTLEMATCH_WORK *p_wk  ���[�N
 *	@param  p_param                     ����
 *	@param	int *p_subseq               ���݂̃T�u�V�[�P���X
 *
 *	@return ���̃��C���V�[�P���X
 */
//-----------------------------------------------------------------------------
static int WIFIBATTLEMATCH_RND_SUBSEQ_Free_Start( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq )
{
  enum
  { 
    SEQ_START_FREE_MSG,
    SEQ_WAIT_MSG,
  };

  switch( *p_subseq )
  { 
    //-------------------------------------
    ///	�t���[���[�h�J�n���b�Z�[�W
    //=====================================
  case SEQ_START_FREE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_002, p_wk->p_font );
    { 
      //�����̃f�[�^��\��
      PLAYERINFO_RANDOMMATCH_DATA info_setup;
      GFL_CLUNIT	*p_unit;

      p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );

      GFL_STD_MemClear( &info_setup, sizeof(PLAYERINFO_RANDOMMATCH_DATA) );
      info_setup.btl_rule = p_param->p_param->btl_rule;
      info_setup.rate     = 0;
      info_setup.win_cnt  = RNDMATCH_GetParam( p_param->p_rndmatch, p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
      info_setup.lose_cnt = RNDMATCH_GetParam( p_param->p_rndmatch, p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );
      info_setup.btl_cnt  = info_setup.win_cnt + info_setup.lose_cnt; //@todo
      if( p_wk->p_playerinfo )
      { 
        PLAYERINFO_RND_Exit( p_wk->p_playerinfo );
        p_wk->p_playerinfo  = NULL;
      }
      {
        MYSTATUS  *p_my;
        p_my  = GAMEDATA_GetMyStatus(p_param->p_param->p_game_data); 
        p_wk->p_playerinfo	= PLAYERINFO_RND_Init( &info_setup, FALSE, p_my, p_unit, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, HEAPID_WIFIBATTLEMATCH_CORE );
      }


    }


    *p_subseq       = SEQ_WAIT_MSG;
    break;

  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    {
      *p_subseq = 0;
      return WIFIBATTLEMATCH_RND_SUBSEQ_FREE_MATCHING;
    }
    break;

  }

  //���̂܂�
  return WIFIBATTLEMATCH_RND_SUBSEQ_FREE_START;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  �t���[���[�h�̃}�b�`���O����
 *
 *	@param	WIFIBATTLEMATCH_WORK *p_wk  ���[�N
 *	@param  p_param                     ����
 *	@param	int *p_subseq               ���݂̃T�u�V�[�P���X
 *
 *	@return ���̃��C���V�[�P���X
 */
//-----------------------------------------------------------------------------
static int WIFIBATTLEMATCH_RND_SUBSEQ_Free_Matching( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq )
{ 

  enum
  { 
    SEQ_START_MATCHING,
    SEQ_START_MATCHING_MSG,
    SEQ_WAIT_MATCHING,
    SEQ_START_CHECK_CHEAT,
    SEQ_WAIT_CHECK_CHEAT,
    SEQ_START_SENDDATA,
    SEQ_WAIT_SENDDATA,
    SEQ_OK_MATCHING_MSG,
    SEQ_OK_MATCHING_WAIT,
    SEQ_END_MATCHING_MSG,
    SEQ_END_MATCHING,

    SEQ_START_SELECT_CANCEL_MSG,
    SEQ_START_SELECT_CANCEL,
    SEQ_WAIT_SELECT_CANCEL,

    SEQ_WAIT_MSG,
  };

  switch( *p_subseq )
  { 
    //-------------------------------------
    ///	�}�b�`���O�J�n
    //=====================================
  case SEQ_START_MATCHING:
    WIFIBATTLEMATCH_NET_StartMatchMake( p_wk->p_net, p_param->p_param->btl_rule ); 
    *p_subseq = SEQ_START_MATCHING_MSG;
    break;

  case SEQ_START_MATCHING_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_009, p_wk->p_font );
    *p_subseq = SEQ_WAIT_MATCHING;
    break;

  case SEQ_WAIT_MATCHING:
    if( WIFIBATTLEMATCH_NET_GetSeqMatchMake( p_wk->p_net ) < WIFIBATTLEMATCH_NET_SEQ_CONNECT_START )
    { 
      if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
      { 
        *p_subseq = SEQ_START_SELECT_CANCEL_MSG;
      }
    }

    if( WIFIBATTLEMATCH_NET_WaitMatchMake( p_wk->p_net ) )
    { 
      *p_subseq = SEQ_START_SENDDATA;
    }
    break;

    //-------------------------------------
    ///	�f�[�^����M
    //=====================================
  case SEQ_START_SENDDATA:
    { 
      WIFIBATTLEMATCH_ENEMYDATA *p_my_data = p_param->p_player_data;
      if( WIFIBATTLEMATCH_NET_StartEnemyData( p_wk->p_net, p_my_data ) )
      { 
        *p_subseq       = SEQ_WAIT_SENDDATA;
      }
    }
    break;

  case SEQ_WAIT_SENDDATA:
    {
      WIFIBATTLEMATCH_ENEMYDATA *p_recv_data;
      if( WIFIBATTLEMATCH_NET_WaitEnemyData( p_wk->p_net, &p_recv_data ) )
      { 
        GFL_CLUNIT  *p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
        WIFIBATTLEMATCH_ENEMYDATA *p_enemy_data = p_param->p_enemy_data;
        GFL_STD_MemCopy( p_recv_data, p_enemy_data, WIFIBATTLEMATCH_DATA_ENEMYDATA_SIZE );

        if( p_wk->p_matchinfo )
        { 
          MATCHINFO_Exit( p_wk->p_matchinfo );
        }
        { 
          p_wk->p_matchinfo		= MATCHINFO_Init( p_enemy_data, p_unit, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, HEAPID_WIFIBATTLEMATCH_CORE );
        }

        *p_subseq       = SEQ_OK_MATCHING_MSG;
      }
    }
    break;

  case SEQ_OK_MATCHING_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_010, p_wk->p_font );
    *p_subseq       = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_OK_MATCHING_WAIT;
    break;

  case SEQ_OK_MATCHING_WAIT:
    if( p_wk->cnt++ > ENEMYDATA_WAIT_SYNC )
    { 
      p_wk->cnt      = 0;
      *p_subseq      = SEQ_END_MATCHING_MSG;
    }
    break;

  case SEQ_END_MATCHING_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_012, p_wk->p_font );
    *p_subseq = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_END_MATCHING;
    break;

  case SEQ_END_MATCHING:
    p_param->result = WIFIBATTLEMATCH_CORE_RESULT_NEXT_BATTLE;
    return WIFIBATTLEMATCH_RND_SUBSEQ_EXIT; 

    //-------------------------------------
    ///	�L�����Z������
    //=====================================
  case SEQ_START_SELECT_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_011, p_wk->p_font );
    *p_subseq       = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_START_SELECT_CANCEL;
    break;

  case SEQ_START_SELECT_CANCEL:
    { 
      WBM_LIST_SETUP  setup;
      GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
      setup.p_msg   = p_wk->p_msg;
      setup.p_font  = p_wk->p_font;
      setup.p_que   = p_wk->p_que;
      setup.strID[0]= WIFIMATCH_TEXT_007;
      setup.strID[1]= WIFIMATCH_TEXT_008;
      setup.list_max= 2;
      setup.frm     = BG_FRAME_M_TEXT;
      setup.font_plt= PLT_FONT_M;
      setup.frm_plt = PLT_LIST_M;
      setup.frm_chr = CGR_OFS_M_LIST;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_subseq     = SEQ_WAIT_SELECT_CANCEL;
    }
    break;

  case SEQ_WAIT_SELECT_CANCEL:
    {
      const u32 select  = WBM_LIST_Main( p_wk->p_list );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        WBM_LIST_Exit( p_wk->p_list );
        switch( select )
        { 
        case 0:
          p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
          return WIFIBATTLEMATCH_RND_SUBSEQ_EXIT; //�I��
        case 1:
          *p_subseq = SEQ_START_MATCHING_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	����
    //=====================================
  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      *p_subseq = p_wk->next_seq;
    }
    break;
  }

  //���̂܂�
  return WIFIBATTLEMATCH_RND_SUBSEQ_FREE_MATCHING;

}
//----------------------------------------------------------------------------
/**
 *	@brief  �����_���}�b�`  �t���[���[�h�̐퓬�I����
 *
 *	@param	WIFIBATTLEMATCH_WORK *p_wk  ���[�N
 *	@param  p_param                     ����
 *	@param	int *p_subseq               ���݂̃T�u�V�[�P���X
 *
 *	@return ���̃��C���V�[�P���X
 */
//-----------------------------------------------------------------------------
static int WIFIBATTLEMATCH_RND_SUBSEQ_Free_EndBattle( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq )
{ 
  enum
  { 
    SEQ_START_SELECT_BTLREC_MSG,
    SEQ_START_SELECTBTLREC,
    SEQ_WAIT_SELECTBTLREC,

    SEQ_START_DISCONNECT,
    SEQ_WAIT_DISCONNECT,

    SEQ_START_SAVE_MSG,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,

    SEQ_START_SELECT_CONTINUE_MSG,
    SEQ_START_SELECT_CONTINUE,
    SEQ_WAIT_SELECT_CONTINUE,

    SEQ_START_SELECT_CANCEL_MSG,
    SEQ_START_SELECT_CANCEL,
    SEQ_WAIT_SELECT_CANCEL,

    SEQ_WAIT_MSG,
  };

  switch( *p_subseq )
  { 
    //-------------------------------------
    /// �^��m�F
    //=====================================
  case SEQ_START_SELECT_BTLREC_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_014, p_wk->p_font );
    *p_subseq = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_START_SELECTBTLREC;
    break;
  case SEQ_START_SELECTBTLREC:
    { 
      WBM_LIST_SETUP  setup;
      GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
      setup.p_msg   = p_wk->p_msg;
      setup.p_font  = p_wk->p_font;
      setup.p_que   = p_wk->p_que;
      setup.strID[0]= WIFIMATCH_TEXT_007;
      setup.strID[1]= WIFIMATCH_TEXT_008;
      setup.list_max= 2;
      setup.frm     = BG_FRAME_M_TEXT;
      setup.font_plt= PLT_FONT_M;
      setup.frm_plt = PLT_LIST_M;
      setup.frm_chr = CGR_OFS_M_LIST;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_subseq     = SEQ_WAIT_SELECTBTLREC;
    }
    break;
  case SEQ_WAIT_SELECTBTLREC:
    {
      const u32 select  = WBM_LIST_Main( p_wk->p_list );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        WBM_LIST_Exit( p_wk->p_list );
        switch( select )
        { 
        case 0:
          *p_subseq = SEQ_START_DISCONNECT;  //@todo
          break;
        case 1:
          *p_subseq = SEQ_START_DISCONNECT;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    /// �ؒf����
    //=====================================
  case SEQ_START_DISCONNECT:
    *p_subseq = SEQ_WAIT_DISCONNECT;
    break;

  case SEQ_WAIT_DISCONNECT:
    if( WIFIBATTLEMATCH_NET_SetDisConnect( p_wk->p_net, TRUE ) )
    { 

      switch( p_param->btl_result )
      {
      case BTL_RESULT_WIN:
      case BTL_RESULT_RUN_ENEMY:
        RNDMATCH_AddParam( p_param->p_rndmatch, p_param->btl_rule, RNDMATCH_PARAM_IDX_WIN );
        break;
      case BTL_RESULT_LOSE:
      case BTL_RESULT_RUN:
        RNDMATCH_AddParam( p_param->p_rndmatch, p_param->btl_rule, RNDMATCH_PARAM_IDX_LOSE );
        break;
      }


      *p_subseq = SEQ_START_SELECT_CONTINUE_MSG;
    }
    break;

    //-------------------------------------
    /// �Z�[�u����
    //=====================================
  case SEQ_START_SAVE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_017, p_wk->p_font );
    *p_subseq = SEQ_START_SAVE;
    break;

  case SEQ_START_SAVE:
    GAMEDATA_SaveAsyncStart(p_param->p_param->p_game_data);
    *p_subseq       = SEQ_WAIT_SAVE;
    break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT result;
      result = GAMEDATA_SaveAsyncMain(p_param->p_param->p_game_data);
      switch(result){
      case SAVE_RESULT_CONTINUE:
      case SAVE_RESULT_LAST:
        break;
      case SAVE_RESULT_OK:
      case SAVE_RESULT_NG:
        *p_subseq       = SEQ_START_SELECT_CONTINUE_MSG;
        break;
      }
    }
    break;

    //-------------------------------------
    /// �ΐ푱�s�m�F
    //=====================================
  case SEQ_START_SELECT_CONTINUE_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_015, p_wk->p_font );
    *p_subseq = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_START_SELECT_CONTINUE;
    break;
  case SEQ_START_SELECT_CONTINUE:
    { 
      WBM_LIST_SETUP  setup;
      GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
      setup.p_msg   = p_wk->p_msg;
      setup.p_font  = p_wk->p_font;
      setup.p_que   = p_wk->p_que;
      setup.strID[0]= WIFIMATCH_TEXT_007;
      setup.strID[1]= WIFIMATCH_TEXT_008;
      setup.list_max= 2;
      setup.frm     = BG_FRAME_M_TEXT;
      setup.font_plt= PLT_FONT_M;
      setup.frm_plt = PLT_LIST_M;
      setup.frm_chr = CGR_OFS_M_LIST;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_subseq     = SEQ_WAIT_SELECT_CONTINUE;
    }
    break;
  case SEQ_WAIT_SELECT_CONTINUE:
    {
      const u32 select  = WBM_LIST_Main( p_wk->p_list );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        WBM_LIST_Exit( p_wk->p_list );
        switch( select )
        { 
        case 0:
          *p_subseq = 0;
          return WIFIBATTLEMATCH_RND_SUBSEQ_FREE_START;
        case 1:
          *p_subseq = SEQ_START_SELECT_CANCEL_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    /// �����_���}�b�`�I���m�F
    //=====================================
  case SEQ_START_SELECT_CANCEL_MSG:
    WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, WIFIMATCH_TEXT_016, p_wk->p_font );
    *p_subseq = SEQ_WAIT_MSG;
    p_wk->next_seq  = SEQ_START_SELECT_CANCEL;
    break;
  case SEQ_START_SELECT_CANCEL:
    { 
      WBM_LIST_SETUP  setup;
      GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
      setup.p_msg   = p_wk->p_msg;
      setup.p_font  = p_wk->p_font;
      setup.p_que   = p_wk->p_que;
      setup.strID[0]= WIFIMATCH_TEXT_007;
      setup.strID[1]= WIFIMATCH_TEXT_008;
      setup.list_max= 2;
      setup.frm     = BG_FRAME_M_TEXT;
      setup.font_plt= PLT_FONT_M;
      setup.frm_plt = PLT_LIST_M;
      setup.frm_chr = CGR_OFS_M_LIST;
      p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_WIFIBATTLEMATCH_CORE );
      *p_subseq     = SEQ_WAIT_SELECT_CANCEL;
    }
    break;
  case SEQ_WAIT_SELECT_CANCEL:
    {
      const u32 select  = WBM_LIST_Main( p_wk->p_list );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        WBM_LIST_Exit( p_wk->p_list );
        switch( select )
        { 
        case 0:
          p_param->result = WIFIBATTLEMATCH_CORE_RESULT_FINISH;
          return WIFIBATTLEMATCH_RND_SUBSEQ_EXIT; //�I��
        case 1:
          *p_subseq = SEQ_START_SELECT_CONTINUE_MSG;
          break;
        }
      }
    }
    break;

    //-------------------------------------
    ///	����
    //=====================================
  case SEQ_WAIT_MSG:
    if( WBM_TEXT_IsEnd( p_wk->p_text ) )
    { 
      *p_subseq = p_wk->next_seq;
    }
    break;
  }


  //���̂܂�
  return WIFIBATTLEMATCH_RND_SUBSEQ_FREE_BTL_END;
}


//----------------------------------------------------------------------------
/**
 *	@brief  �G���[�̂��߃��O�C���ɖ߂�
 *
 *	@param	WIFIBATTLEMATCH_WORK *p_wk  ���[�N
 *	@param  p_param                     ����
 *	@param	int *p_subseq               ���݂̃T�u�V�[�P���X
 *
 *	@return ���̃��C���V�[�P���X
 */
//-----------------------------------------------------------------------------
static int WIFIBATTLEMATCH_RND_SUBSEQ_Err_ReturnLogin( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_subseq )
{ 
  p_param->result = WIFIBATTLEMATCH_CORE_RESULT_ERR_NEXT_LOGIN;
  return WIFIBATTLEMATCH_RND_SUBSEQ_EXIT; //�I��
}

//=============================================================================
/**
 *  ���C���V�[�P���X
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  WIFI���  ���C���V�[�P���X
 *
 *	@param	WIFIBATTLEMATCH_WORK  *p_wk     ���[�N
 *	@param  WIFIBATTLEMATCH_CORE_PARAM *p_param  ����
 *	@param	int *p_seq                      �V�[�P���X
 *
 *	@return TRUE�ŏI��  FALSE�Ń��[�v
 */
//-----------------------------------------------------------------------------
static BOOL WifiBattleMatch_WiFi_MainSeq( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_seq )
{ 

  return  FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���C�u���  ���C���V�[�P���X
 *
 *	@param	WIFIBATTLEMATCH_WORK  *p_wk     ���[�N
 *	@param  WIFIBATTLEMATCH_CORE_PARAM *p_param  ����
 *	@param	int *p_seq                      �V�[�P���X
 *
 *	@return TRUE�ŏI��  FALSE�Ń��[�v
 */
//-----------------------------------------------------------------------------
static BOOL WifiBattleMatch_Live_MainSeq( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_seq )
{ 

  return  FALSE;
}
//----------------------------------------------------------------------------
/**
 *	@brief  �f�o�b�O�p  ���C���V�[�P���X
 *
 *	@param	WIFIBATTLEMATCH_WORK  *p_wk     ���[�N
 *	@param  WIFIBATTLEMATCH_CORE_PARAM *p_param  ����
 *	@param	int *p_seq                      �V�[�P���X
 *
 *	@return TRUE�ŏI��  FALSE�Ń��[�v
 */
//-----------------------------------------------------------------------------
static BOOL WifiBattleMatch_Debug_MainSeq( WIFIBATTLEMATCH_WORK *p_wk, WIFIBATTLEMATCH_CORE_PARAM	*p_param, int *p_seq )
{ 
  switch( *p_seq )
  { 
  case 0:
    WIFIBATTLEMATCH_NET_StartMatchMake( p_wk->p_net, p_param->p_param->btl_rule ); 
    (*p_seq)++;
    break;

  case 1:
    if( WIFIBATTLEMATCH_NET_WaitMatchMake( p_wk->p_net ) )
    { 
      NAGI_Printf( "�ڑ������I\n" );
      (*p_seq)++;
    }
    break;

  case 2:
    if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_B )
    {	
      return GFL_PROC_RES_FINISH;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_A )
    { 
      WIFIBATTLEMATCH_SC_Start( p_wk->p_net, p_param->p_param->btl_rule, p_param->btl_result );
      NAGI_Printf( "SC�J�n�I\n" );
      (*p_seq)  = 3;
    }
    else if( GFL_UI_KEY_GetTrg() & PAD_BUTTON_X )
    { 
      NAGI_Printf( "GDB�J�n�I\n" );
      WIFIBATTLEMATCH_GDB_Start( p_wk->p_net, WIFIBATTLEMATCH_GDB_GET_RND_SCORE, &p_wk->rnd_score );
      (*p_seq)  = 4;
    }
    break;

  case 3:
    { 
      DWCScResult result;
      if( WIFIBATTLEMATCH_SC_Process( p_wk->p_net, &result ) )
      { 
        NAGI_Printf( "��������I%d\n", result );
        (*p_seq)  = 2;
      }
      GF_ASSERT( result == DWC_SC_RESULT_NO_ERROR );
    }
    break;

  case 4:
    { 
      DWCGdbError error;
      if( WIFIBATTLEMATCH_GDB_Process( p_wk->p_net, &error ) )
      { 
        NAGI_Printf( "�f�[�^�x�[�X������������I%d\n", error );
        (*p_seq)  = 2;
      }
      else
      { 
        WIFIBATTLEMATCH_NET_ERROR_REPAIR_TYPE type;
        type = WIFIBATTLEMATCH_GDB_GetErrorRepairType( error );
        switch( type )
        { 
        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_RETRY:       //������x
          (*p_seq)  = 2;
          break;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_DISCONNECT:  //�ؒf�����O�C�������蒼��
          return WIFIBATTLEMATCH_RND_SUBSEQ_ERR_RETURN_LOGIN;

        case WIFIBATTLEMATCH_NET_ERROR_REPAIR_FATAL:       //�d���ؒf
          NetErr_DispCallFatal();
          break;
        }
      }
    }
    break;
  }

  return  FALSE;
}

//----------------------------------------------------------------------------
/**
 *	@brief  �ΐ�ҏ�񂪕s���������ꍇ�A������萳��l�ɂ���
 *
 *	@param	WIFIBATTLEMATCH_ENEMYDATA *p_data ���[�N
 */
//-----------------------------------------------------------------------------
static void WifiBattleMatch_Darty_ModifiEnemyData( WIFIBATTLEMATCH_ENEMYDATA *p_data )
{
  //���O���s��
  { 
    int i;
    BOOL is_darty = TRUE;
    //EOM�������Ă���ΐ���Ƃ݂Ȃ�
    for( i = 0; i < PERSON_NAME_SIZE+EOM_SIZE; i++ )
    { 
      if( p_data->name[i] == GFL_STR_GetEOMCode() )
      { 
        is_darty  = FALSE;
      }
    }

    if( is_darty )
    { 
      STRBUF* p_strbuf;
      GFL_MSGDATA *p_msg;
      p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
												NARC_message_wifi_match_dat, HEAPID_WIFIBATTLEMATCH_CORE );
      p_strbuf  = GFL_MSG_CreateString( p_msg, HEAPID_WIFIBATTLEMATCH_CORE );

      //�K�薼�ɕύX
      OS_TPrintf( "�ΐ�Җ����s���������̂ŁA������菑�������܂�\n" );
      GFL_STR_GetStringCode( p_strbuf, p_data->name, PERSON_NAME_SIZE+EOM_SIZE );

      GFL_STR_DeleteBuffer( p_strbuf );
      GFL_MSG_Delete( p_msg );
    }
  }

  //���ʂ��s��
  if( p_data->sex != PTL_SEX_MALE && p_data->sex != PTL_SEX_FEMALE )
  { 
    OS_TPrintf( "���ʂ��s���������̂ŁA������菑�������܂� %d\n", p_data->sex );
    p_data->sex = PTL_SEX_MALE;
  }

  //�g���[�i�[�r���[
  //@todo
  if( p_data->trainer_view == 0xFFFF )
  { 
    OS_TPrintf( "�����ڂ��s���������̂ŁA������菑�������܂� \n" );
    p_data->trainer_view  = 0;
  }

  //�ꏊ
  if( p_data->nation >= WIFI_COUNTRY_GetDataLen() )
  { 
    OS_TPrintf( "�����s���������̂ŁA������菑�������܂� %d %d\n", p_data->nation, p_data->area );
    p_data->nation  = 0;
  }

  //�G���A
  { 
    if( p_data->area >= WIFI_COUNTRY_CountryCodeToPlaceIndexMax( p_data->nation ) )
    {
      OS_TPrintf( "�n�悪�s���������̂ŁA������菑�������܂� %d %d\n", p_data->nation, p_data->area );
      p_data->area  = 0;
    }
  }

  //PMS
  if( !PMSDAT_IsValid( &p_data->pms, HEAPID_WIFIBATTLEMATCH_CORE ) )
  { 
    OS_TPrintf( "PMS���s���������̂ŁA������菑�������܂�\n" );
    PMSDAT_Init( &p_data->pms, PMS_TYPE_BATTLE_READY ); //�����A�H @todo
  }
}
