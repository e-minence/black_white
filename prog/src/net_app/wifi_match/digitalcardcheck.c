//[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[[
/**
 *
 *	@file		digitalcardcheck.c
 *	@brief	�f�W�^���I��؊m�F���
 *	@author	Toru=Nagihashi
 *	@data		2010.1.26
 *
 */
//]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]]
//	���C�u����
#include <gflib.h>

//	�V�X�e��
#include "system/main.h"
#include "system/gfl_use.h"
#include "net/dreamworld_netdata.h"

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
#include "savedata/regulation.h"

//  WIFI�o�g���}�b�`�̃��W���[��
#include "wifibattlematch_graphic.h"
#include "wifibattlematch_view.h"
#include "wifibattlematch_util.h"

//  PROC
#include "battle_championship/battle_championship.h"

//  �O�����J
#include "net_app/digitalcardcheck.h"

//-------------------------------------
///	�I�[�o�[���C
//=====================================
FS_EXTERN_OVERLAY(ui_common);
FS_EXTERN_OVERLAY(battle_championship);

//-------------------------------------
///	�f�o�b�O
//=====================================
#ifdef PM_DEBUG
#define DEBUG_INPUT_STATUS  (2)   //0��undef���Ƃ��Ȃ�  ���̑��͊J�ÃX�e�[�^�X�ɂȂ�
#endif //PM_DEBUG

//=============================================================================
/**
 *					�萔�錾
*/
//=============================================================================
#define DIGITALCARDCHECK_PUSH_BUTTON  (PAD_BUTTON_DECIDE|PAD_BUTTON_CANCEL)

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

  //���\�[�X�ǂݍ���
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

  //�e�L�X�g��
  WBM_TEXT_WORK             *p_text;

  //���X�g
  WBM_LIST_WORK             *p_list;

  //���C���V�[�P���X
  WBM_SEQ_WORK              *p_seq;

  //����
  GAMEDATA                  *p_game_data;
} DIGITALCARDCHECK_WORK;

//=============================================================================
/**
 *					�v���g�^�C�v
*/
//=============================================================================
//-------------------------------------
///	�v���Z�X
//=====================================
static GFL_PROC_RESULT DIGITALCARDCHECK_PROC_Init
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT DIGITALCARDCHECK_PROC_Exit
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );
static GFL_PROC_RESULT DIGITALCARDCHECK_PROC_Main
	( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs );

//-------------------------------------
///	WIFI���V�[�P���X�֐�
//=====================================
static void DC_SEQFUNC_Init( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_SignUp( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_Entry( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_Retire( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_NoData( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );
static void DC_SEQFUNC_End( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs );

//-------------------------------------
///	���W���[�����g���₷���܂Ƃ߂�����
//=====================================
//�v���C���[���
static void Util_PlayerInfo_Create( DIGITALCARDCHECK_WORK *p_wk );
static void Util_PlayerInfo_Delete( DIGITALCARDCHECK_WORK *p_wk );
static BOOL Util_PlayerInfo_Move( DIGITALCARDCHECK_WORK *p_wk );
static void Util_PlayerInfo_RenewalData( DIGITALCARDCHECK_WORK *p_wk, PLAYERINFO_WIFI_UPDATE_TYPE type );
//�I����
typedef enum
{ 
  UTIL_LIST_TYPE_YESNO,
  UTIL_LIST_TYPE_RETURN,
  UTIL_LIST_TYPE_UNREGISTER,
}UTIL_LIST_TYPE;
static void Util_List_Create( DIGITALCARDCHECK_WORK *p_wk, UTIL_LIST_TYPE type );
static void Util_List_Delete( DIGITALCARDCHECK_WORK *p_wk );
static u32 Util_List_Main( DIGITALCARDCHECK_WORK *p_wk );

//�e�L�X�g
static void Util_Text_Print( DIGITALCARDCHECK_WORK *p_wk, u32 strID );
static BOOL Util_Text_IsEnd( DIGITALCARDCHECK_WORK *p_wk );
static void Util_Text_SetVisible( DIGITALCARDCHECK_WORK *p_wk, BOOL is_visible );

//=============================================================================
/**
 *					�O���Q��
*/
//=============================================================================
//-------------------------------------
///	PROC
//=====================================
const GFL_PROC_DATA	DigitalCard_ProcData =
{	
	DIGITALCARDCHECK_PROC_Init,
	DIGITALCARDCHECK_PROC_Main,
	DIGITALCARDCHECK_PROC_Exit,
};

//=============================================================================
/**
 *					PROC
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief	�f�W�^���J�[�h�m�F���	�R�A�v���Z�X������
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DIGITALCARDCHECK_PROC_Init( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{	
	DIGITALCARDCHECK_WORK	*p_wk;


	GFL_OVERLAY_Load( FS_OVERLAY_ID(ui_common));

	//�q�[�v�쐬
	GFL_HEAP_CreateHeap( GFL_HEAPID_APP, HEAPID_DIGITALCARD_CHECK, 0x30000 );

	//�v���Z�X���[�N�쐬
	p_wk	= GFL_PROC_AllocWork( p_proc, sizeof(DIGITALCARDCHECK_WORK), HEAPID_DIGITALCARD_CHECK );
	GFL_STD_MemClear( p_wk, sizeof(DIGITALCARDCHECK_WORK) );
  p_wk->p_game_data   = GAMEDATA_Create( HEAPID_DIGITALCARD_CHECK );

	//�O���t�B�b�N�ݒ�
	p_wk->p_graphic	= WIFIBATTLEMATCH_GRAPHIC_Init( GX_DISP_SELECT_MAIN_SUB, HEAPID_DIGITALCARD_CHECK );

  //���\�[�X�ǂݍ���
  p_wk->p_res     = WIFIBATTLEMATCH_VIEW_LoadResource( WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic ), WIFIBATTLEMATCH_VIEW_RES_MODE_DIGITALCARD, HEAPID_DIGITALCARD_CHECK );

	//���ʃ��W���[���̍쐬
	p_wk->p_font	= GFL_FONT_Create( ARCID_FONT, NARC_font_large_gftr, 
      GFL_FONT_LOADTYPE_FILE, FALSE, HEAPID_DIGITALCARD_CHECK );
	p_wk->p_que		= PRINTSYS_QUE_Create( HEAPID_DIGITALCARD_CHECK );
	p_wk->p_msg		= GFL_MSG_Create( GFL_MSG_LOAD_NORMAL, ARCID_MESSAGE, 
												NARC_message_wifi_match_dat, HEAPID_DIGITALCARD_CHECK );
	p_wk->p_word	= WORDSET_CreateEx( WORDSET_DEFAULT_SETNUM, WORDSET_COUNTRY_BUFLEN, HEAPID_DIGITALCARD_CHECK );

	//���W���[���̍쐬
  p_wk->p_text  = WBM_TEXT_Init( BG_FRAME_M_TEXT, PLT_FONT_M, PLT_TEXT_M, CGR_OFS_M_TEXT, p_wk->p_que, p_wk->p_font, HEAPID_DIGITALCARD_CHECK );
  Util_Text_SetVisible( p_wk, FALSE );
  p_wk->p_seq   = WBM_SEQ_Init( p_wk, DC_SEQFUNC_Init, HEAPID_DIGITALCARD_CHECK );

#ifdef DEBUG_INPUT_STATUS
#if DEBUG_INPUT_STATUS
  { 
    SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->p_game_data );
    REGULATION_CARDDATA*p_reg = SaveData_GetRegulationCardData( p_sv );
    Regulation_SetDebugData( p_reg );
    Regulation_SetCardParam( p_reg, REGULATION_CARD_STATUS, DEBUG_INPUT_STATUS );
  }
#endif
#endif 

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�W�^���J�[�h�m�F���	�R�A�v���Z�X�j��
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DIGITALCARDCHECK_PROC_Exit( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
	DIGITALCARDCHECK_WORK	  *p_wk	= p_wk_adrs;

	//���W���[���̔j��
  WBM_SEQ_Exit( p_wk->p_seq );
  WBM_TEXT_Exit( p_wk->p_text );
  Util_PlayerInfo_Delete( p_wk );

	//���ʃ��W���[���̔j��
	WORDSET_Delete( p_wk->p_word );
	GFL_MSG_Delete( p_wk->p_msg );
	PRINTSYS_QUE_Delete( p_wk->p_que );
	GFL_FONT_Delete( p_wk->p_font );

  //���\�[�X�j��
  WIFIBATTLEMATCH_VIEW_UnLoadResource( p_wk->p_res );

	//�O���t�B�b�N�j��
	WIFIBATTLEMATCH_GRAPHIC_Exit( p_wk->p_graphic );

  //�Q�[���f�[�^�j��
  GAMEDATA_Delete( p_wk->p_game_data );

	//�v���Z�X���[�N�j��
	GFL_PROC_FreeWork( p_proc );

	//�q�[�v�j��
	GFL_HEAP_DeleteHeap( HEAPID_DIGITALCARD_CHECK );
	
	GFL_OVERLAY_Unload( FS_OVERLAY_ID(ui_common));

	return GFL_PROC_RES_FINISH;
}

//----------------------------------------------------------------------------
/**
 *	@brief	�f�W�^���J�[�h�m�F���	���C���v���Z�X����
 *
 *	@param	GFL_PROC *p_proc	�v���Z�X
 *	@param	*p_seq						�V�[�P���X
 *	@param	*p_param					�e���[�N
 *	@param	*p_work						���[�N
 *
 *	@return	�I���R�[�h
 */
//-----------------------------------------------------------------------------
static GFL_PROC_RESULT DIGITALCARDCHECK_PROC_Main( GFL_PROC *p_proc, int *p_seq, void *p_param_adrs, void *p_wk_adrs )
{
  enum
  { 
    SEQ_FADEIN_START,
    SEQ_FADEIN_WAIT,
    SEQ_MAIN,
    SEQ_FADEOUT_START,
    SEQ_FADEOUT_WAIT,
  };

	DIGITALCARDCHECK_WORK	*p_wk	    = p_wk_adrs;

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

  //BG
  WIFIBATTLEMATCH_VIEW_Main( p_wk->p_res );

  //�����\��
  if( p_wk->p_playerinfo )
  { 
    PLAYERINFO_PrintMain( p_wk->p_playerinfo, p_wk->p_que );
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
 *	@brief  �f�W�^���I��؃`�F�b�N  ����������
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void DC_SEQFUNC_Init( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  DIGITALCARDCHECK_WORK	  *p_wk	    = p_wk_adrs;
  SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->p_game_data );
  REGULATION_CARDDATA * p_reg = SaveData_GetRegulationCardData( p_sv );
  const u32 cup_no      = Regulation_GetCardParam( p_reg, REGULATION_CARD_CUPNO );
  const u32 cup_status  = Regulation_GetCardParam( p_reg, REGULATION_CARD_STATUS );


  if( cup_no > 0 && cup_status == DREAM_WORLD_MATCHUP_SIGNUP )
  { 
    //���ɓo�^���āA�|�P�������o�^
    WBM_SEQ_SetNext( p_seqwk, DC_SEQFUNC_SignUp );
  }
  else if( cup_no > 0 && cup_status == DREAM_WORLD_MATCHUP_ENTRY )
  { 
    //���ɓo�^���āA�|�P�����o�^�ς�
    WBM_SEQ_SetNext( p_seqwk, DC_SEQFUNC_Entry );
  }
  else if( cup_no > 0 && cup_status == DREAM_WORLD_MATCHUP_END )
  { 
    //���I��
    WBM_SEQ_SetNext( p_seqwk, DC_SEQFUNC_CupEnd );
  }
  else if( cup_no > 0 && cup_status == DREAM_WORLD_MATCHUP_RETIRE )
  { 
    //������������
    WBM_SEQ_SetNext( p_seqwk, DC_SEQFUNC_Retire );
  }
  else
  { 
    WBM_SEQ_SetNext( p_seqwk, DC_SEQFUNC_NoData );
  }

}
//----------------------------------------------------------------------------
/**
 *	@brief  �f�W�^���I��؃`�F�b�N  �T�C���A�b�v����
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void DC_SEQFUNC_SignUp( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_DRAW_PLAYERINFO,
    SEQ_WAIT_DRAW_PLAYERINFO,
    SEQ_WAIT_PUSH,
    SEQ_START_MSG_SIGNUP,
    SEQ_WAIT_MSG,
    SEQ_START_LIST_RETURN,
    SEQ_WAIT_LIST_RETURN,
    SEQ_PROC_END,
  };
  DIGITALCARDCHECK_WORK	  *p_wk	    = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_PLAYERINFO:
    Util_PlayerInfo_Create( p_wk );
    *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    if( Util_PlayerInfo_Move( p_wk ) )
    { 
      *p_seq  = SEQ_WAIT_PUSH;
    }
    break;

  case SEQ_WAIT_PUSH:
    if( GFL_UI_KEY_GetTrg() & DIGITALCARDCHECK_PUSH_BUTTON )
    { 
      *p_seq  = SEQ_START_MSG_SIGNUP;
    }
    break;

  case SEQ_START_MSG_SIGNUP:
    Util_Text_SetVisible( p_wk, TRUE );
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_00 );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_WAIT_MSG:
    if( Util_Text_IsEnd( p_wk ) )
    { 
      *p_seq  = SEQ_START_LIST_RETURN;
    }
    break;

  case SEQ_START_LIST_RETURN:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_RETURN );
    *p_seq  = SEQ_WAIT_LIST_RETURN;
    break;

  case SEQ_WAIT_LIST_RETURN:
    { 
      const u32 select = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        if( select == 0 )
        { 
          *p_seq  = SEQ_PROC_END;
        }
      }
    }
    break;
    
  case SEQ_PROC_END:
    WBM_SEQ_SetNext( p_seqwk, DC_SEQFUNC_End );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �f�W�^���I��؃`�F�b�N  �G���g���[��
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void DC_SEQFUNC_Entry( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_DRAW_PLAYERINFO,
    SEQ_WAIT_DRAW_PLAYERINFO,
    SEQ_WAIT_PUSH,
    SEQ_START_MSG_ENTRY,
    SEQ_START_LIST_UNREGISTER,
    SEQ_WAIT_LIST_UNREGISTER,

    SEQ_START_MSG_CONFIRM1,
    SEQ_START_LIST_CONFIRM1,
    SEQ_WAIT_LIST_CONFIRM1,

    SEQ_START_MSG_CONFIRM2,
    SEQ_START_LIST_CONFIRM2,
    SEQ_WAIT_LIST_CONFIRM2,

    SEQ_START_MSG_UNREGISTER,
    SEQ_RETIRE,
    SEQ_START_SAVE,
    SEQ_WAIT_SAVE,
    SEQ_START_MSG_UNLOCK,

    SEQ_PROC_END,
    SEQ_WAIT_MSG,
  };
  DIGITALCARDCHECK_WORK	  *p_wk	    = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_PLAYERINFO:
    Util_PlayerInfo_Create( p_wk );
    *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    if( Util_PlayerInfo_Move( p_wk ) )
    { 
      *p_seq  = SEQ_WAIT_PUSH;
    }
    break;

  case SEQ_WAIT_PUSH:
    if( GFL_UI_KEY_GetTrg() & DIGITALCARDCHECK_PUSH_BUTTON )
    { 
      *p_seq  = SEQ_START_MSG_ENTRY;
    }
    break;

  case SEQ_START_MSG_ENTRY:
    Util_Text_SetVisible( p_wk, TRUE );
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_01 );
    *p_seq  = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_LIST_UNREGISTER );
    break;

  case SEQ_START_LIST_UNREGISTER:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_UNREGISTER );
    *p_seq  = SEQ_WAIT_LIST_UNREGISTER;
    break;

  case SEQ_WAIT_LIST_UNREGISTER:
    { 
      const u32 select = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        if( select == 0 )   //�Q���̉���
        { 
          *p_seq  = SEQ_START_MSG_CONFIRM1;
        }
        else if( select == 1 )  //���ǂ�
        { 
          *p_seq  = SEQ_PROC_END;
        }
      }
    }
    break;

  case SEQ_START_MSG_CONFIRM1:
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_02 );
    *p_seq  = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_LIST_CONFIRM1 );
    break;
  case SEQ_START_LIST_CONFIRM1:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_WAIT_LIST_CONFIRM1;
    break;
  case SEQ_WAIT_LIST_CONFIRM1:
    { 
      const u32 select = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        if( select == 0 )   //�͂�
        { 
          *p_seq  = SEQ_START_MSG_CONFIRM2;
        }
        else if( select == 1 )  //������
        { 
          *p_seq  = SEQ_PROC_END;
        }
      }
    }
    break;
  case SEQ_START_MSG_CONFIRM2:
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_03 );
    *p_seq  = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_START_LIST_CONFIRM2 );
    break;
  case SEQ_START_LIST_CONFIRM2:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_YESNO );
    *p_seq  = SEQ_WAIT_LIST_CONFIRM2;
    break;
  case SEQ_WAIT_LIST_CONFIRM2:
    { 
      const u32 select = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        if( select == 0 )   //�͂�
        { 
          *p_seq  = SEQ_RETIRE;
        }
        else if( select == 1 )  //������
        { 
          *p_seq  = SEQ_PROC_END;
        }
      }
    }
    break;    

  case SEQ_START_MSG_UNREGISTER:
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_04 );
    *p_seq  = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_RETIRE );
    break;

  case SEQ_RETIRE:
    { 
      
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->p_game_data );
      BATTLE_BOX_SAVE   *p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );
      REGULATION_CARDDATA*p_reg = SaveData_GetRegulationCardData( p_sv );

      BATTLE_BOX_SAVE_SetLockFlg( p_bbox_save, FALSE );
      Regulation_SetCardParam( p_reg, REGULATION_CARD_STATUS, DREAM_WORLD_MATCHUP_RETIRE );
      *p_seq  = SEQ_START_SAVE;
    }
    break;
    
  case SEQ_START_SAVE:
    { 
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->p_game_data );
      SaveControl_SaveAsyncInit(p_sv);
      *p_seq  = SEQ_WAIT_SAVE;
    }
    break;
  case SEQ_WAIT_SAVE:
    {
      SAVE_RESULT ret;
      SAVE_CONTROL_WORK *p_sv = GAMEDATA_GetSaveControlWork( p_wk->p_game_data );
      ret = SaveControl_SaveAsyncMain(p_sv);
      if( ret == SAVE_RESULT_OK )
      { 
        *p_seq  = SEQ_START_MSG_UNLOCK;
      }
      else if( ret == SAVE_RESULT_NG )
      { 
        *p_seq  = SEQ_START_MSG_UNLOCK;
      }
    }
    break;
  case SEQ_START_MSG_UNLOCK:
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_05 );
    *p_seq  = SEQ_WAIT_MSG;
    WBM_SEQ_SetReservSeq( p_seqwk, SEQ_PROC_END );
    break;

  case SEQ_PROC_END:
    WBM_SEQ_SetNext( p_seqwk, DC_SEQFUNC_End );
    break;


  case SEQ_WAIT_MSG:
    if( Util_Text_IsEnd( p_wk ) )
    {
      WBM_SEQ_NextReservSeq( p_seqwk );
    }
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �f�W�^���I��؃`�F�b�N  ���I������
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void DC_SEQFUNC_CupEnd( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_DRAW_PLAYERINFO,
    SEQ_WAIT_DRAW_PLAYERINFO,
    SEQ_WAIT_PUSH,
    SEQ_START_MSG_CUPEND,
    SEQ_WAIT_MSG,
    SEQ_START_LIST_RETURN,
    SEQ_WAIT_LIST_RETURN,
    SEQ_PROC_END,
  };
  DIGITALCARDCHECK_WORK	  *p_wk	    = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_PLAYERINFO:
    Util_PlayerInfo_Create( p_wk );
    *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    if( Util_PlayerInfo_Move( p_wk ) )
    { 
      *p_seq  = SEQ_WAIT_PUSH;
    }
    break;

  case SEQ_WAIT_PUSH:
    if( GFL_UI_KEY_GetTrg() & DIGITALCARDCHECK_PUSH_BUTTON )
    { 
      *p_seq  = SEQ_START_MSG_CUPEND;
    }
    break;

  case SEQ_START_MSG_CUPEND:
    Util_Text_SetVisible( p_wk, TRUE );
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_06 );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_WAIT_MSG:
    if( Util_Text_IsEnd( p_wk ) )
    { 
      *p_seq  = SEQ_START_LIST_RETURN;
    }
    break;

  case SEQ_START_LIST_RETURN:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_RETURN );
    *p_seq  = SEQ_WAIT_LIST_RETURN;
    break;

  case SEQ_WAIT_LIST_RETURN:
    { 
      const u32 select = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        if( select == 0 )
        { 
          *p_seq  = SEQ_PROC_END;
        }
      }
    }
    break;
    
  case SEQ_PROC_END:
    WBM_SEQ_SetNext( p_seqwk, DC_SEQFUNC_End );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �f�W�^���I��؃`�F�b�N  ���^�C�A
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void DC_SEQFUNC_Retire( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_DRAW_PLAYERINFO,
    SEQ_WAIT_DRAW_PLAYERINFO,
    SEQ_WAIT_PUSH,
    SEQ_START_MSG_RETIRE,
    SEQ_WAIT_MSG,
    SEQ_START_LIST_RETURN,
    SEQ_WAIT_LIST_RETURN,
    SEQ_PROC_END,
  };
  DIGITALCARDCHECK_WORK	  *p_wk	    = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_DRAW_PLAYERINFO:
    Util_PlayerInfo_Create( p_wk );
    *p_seq  = SEQ_WAIT_DRAW_PLAYERINFO;
    break;

  case SEQ_WAIT_DRAW_PLAYERINFO:
    if( Util_PlayerInfo_Move( p_wk ) )
    { 
      *p_seq  = SEQ_WAIT_PUSH;
    }
    break;

  case SEQ_WAIT_PUSH:
    if( GFL_UI_KEY_GetTrg() & DIGITALCARDCHECK_PUSH_BUTTON )
    { 
      *p_seq  = SEQ_START_MSG_RETIRE;
    }
    break;

  case SEQ_START_MSG_RETIRE:
    Util_Text_SetVisible( p_wk, TRUE );
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_07 );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_WAIT_MSG:
    if( Util_Text_IsEnd( p_wk ) )
    { 
      *p_seq  = SEQ_START_LIST_RETURN;
    }
    break;

  case SEQ_START_LIST_RETURN:
    Util_List_Create( p_wk, UTIL_LIST_TYPE_RETURN );
    *p_seq  = SEQ_WAIT_LIST_RETURN;
    break;

  case SEQ_WAIT_LIST_RETURN:
    { 
      const u32 select = Util_List_Main( p_wk );
      if( select != WBM_LIST_SELECT_NULL )
      { 
        Util_List_Delete( p_wk );
        if( select == 0 )
        { 
          *p_seq  = SEQ_PROC_END;
        }
      }
    }
    break;
    
  case SEQ_PROC_END:
    WBM_SEQ_SetNext( p_seqwk, DC_SEQFUNC_End );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �f�W�^���I��؃`�F�b�N  �f�[�^���Ȃ�
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void DC_SEQFUNC_NoData( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  enum
  { 
    SEQ_START_MSG_NODATA,
    SEQ_WAIT_MSG,
    SEQ_PROC_END,
  };
  DIGITALCARDCHECK_WORK	  *p_wk	    = p_wk_adrs;

  switch( *p_seq )
  { 
  case SEQ_START_MSG_NODATA:
    Util_Text_SetVisible( p_wk, TRUE );
    Util_Text_Print( p_wk,  WIFIMATCH_DPC_STR_08 );
    *p_seq  = SEQ_WAIT_MSG;
    break;

  case SEQ_WAIT_MSG:
    if( Util_Text_IsEnd( p_wk ) )
    { 
      *p_seq  = SEQ_PROC_END;
    }
    break;

  case SEQ_PROC_END:
    WBM_SEQ_SetNext( p_seqwk, DC_SEQFUNC_End );
    break;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �f�W�^���I��؃`�F�b�N  �V�[�P���X�I��
 *
 *	@param	WBM_SEQ_WORK *p_seqwk       �V�[�P���X���[�N
 *	@param  p_seq                       �V�[�P���X
 *	@param	p_wk_adrs                   ���[�N�A�h���X
 */
//-----------------------------------------------------------------------------
static void DC_SEQFUNC_End( WBM_SEQ_WORK *p_seqwk, int *p_seq, void *p_wk_adrs )
{ 
  DIGITALCARDCHECK_WORK	  *p_wk	    = p_wk_adrs;
  WBM_SEQ_End( p_seqwk );
}

//=============================================================================
/**
 *  �֗�
 */
//=============================================================================
//----------------------------------------------------------------------------
/**
 *	@brief  �����̏���\��
 *
 *	@param	DIGITALCARDCHECK_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_Create( DIGITALCARDCHECK_WORK *p_wk )
{ 
  if( p_wk->p_playerinfo == NULL )
  {
    MYSTATUS    *p_my;
    GFL_CLUNIT	*p_unit;
    SAVE_CONTROL_WORK *p_sv;
    BATTLE_BOX_SAVE   *p_bbox_save;

    PLAYERINFO_WIFICUP_DATA info_setup;

    const REGULATION_CARDDATA *cp_reg_card  = SaveData_GetRegulationCardData(GAMEDATA_GetSaveControlWork( p_wk->p_game_data ));


    p_my    = GAMEDATA_GetMyStatus( p_wk->p_game_data); 
    p_unit	= WIFIBATTLEMATCH_GRAPHIC_GetClunit( p_wk->p_graphic );
    p_sv    = GAMEDATA_GetSaveControlWork( p_wk->p_game_data );
    p_bbox_save  = BATTLE_BOX_SAVE_GetBattleBoxSave( p_sv );


    //�����̃f�[�^��\��
    GFL_STD_MemClear( &info_setup, sizeof(PLAYERINFO_WIFICUP_DATA) );
    GFL_STD_MemCopy( Regulation_GetCardCupNamePointer( cp_reg_card ), info_setup.cup_name, 2*(WIFI_PLAYER_TIX_CUPNAME_MOJINUM + EOM_SIZE) );

    info_setup.start_date = GFDATE_Set( 
        Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_START_YEAR ),
        Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_START_MONTH ), 
        Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_START_DAY ),
          0);

    info_setup.end_date = GFDATE_Set( 
        Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_END_YEAR ),
        Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_END_MONTH ), 
        Regulation_GetCardParam( cp_reg_card, REGULATION_CARD_END_DAY ),
          0);

    info_setup.trainerID  = MyStatus_GetTrainerView( p_my );

    info_setup.rate = 0;    //@todo
    info_setup.btl_cnt = 0;//@todo

    p_wk->p_playerinfo	= PLAYERINFO_WIFI_Init( &info_setup, FALSE, p_my, p_unit, p_wk->p_res, p_wk->p_font, p_wk->p_que, p_wk->p_msg, p_wk->p_word, p_bbox_save, TRUE, HEAPID_DIGITALCARD_CHECK );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̏���j��
 *
 *	@param	DIGITALCARDCHECK_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_Delete( DIGITALCARDCHECK_WORK *p_wk )
{ 
  if( p_wk->p_playerinfo )
  { 
    PLAYERINFO_WIFI_Exit( p_wk->p_playerinfo );
    p_wk->p_playerinfo  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h���X���C�h�C��
 *
 *	@param	DIGITALCARDCHECK_WORK *p_wk  ���[�N
 *
 *	@return TRUE�Ŋ���  FALSE�ŏ�����
 */
//-----------------------------------------------------------------------------
static BOOL Util_PlayerInfo_Move( DIGITALCARDCHECK_WORK *p_wk )
{ 
  return PLAYERINFO_MoveMain( p_wk->p_playerinfo );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �����̃J�[�h�̃f�[�^���X�V
 *
 *	@param	DIGITALCARDCHECK_WORK *p_wk
 *	@param	type 
 */
//-----------------------------------------------------------------------------
static void Util_PlayerInfo_RenewalData( DIGITALCARDCHECK_WORK *p_wk, PLAYERINFO_WIFI_UPDATE_TYPE type )
{ 
  PLAYERINFO_WIFI_RenewalData( p_wk->p_playerinfo, type, p_wk->p_msg, p_wk->p_que, p_wk->p_font, HEAPID_DIGITALCARD_CHECK );

}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g������
 *
 *	@param	DIGITALCARDCHECK_WORK *p_wk  ���[�N
 *	@param	type                            ���X�g�̎��
 */
//-----------------------------------------------------------------------------
static void Util_List_Create( DIGITALCARDCHECK_WORK *p_wk, UTIL_LIST_TYPE type )
{ 
  if( p_wk->p_list == NULL )
  { 
    WBM_LIST_SETUP  setup;
    GFL_STD_MemClear( &setup, sizeof(WBM_LIST_SETUP) );
    setup.p_msg   = p_wk->p_msg;
    setup.p_font  = p_wk->p_font;
    setup.p_que   = p_wk->p_que;
    setup.frm     = BG_FRAME_M_TEXT;
    setup.font_plt= PLT_FONT_M;
    setup.frm_plt = PLT_LIST_M;
    setup.frm_chr = CGR_OFS_M_LIST;

    switch( type )
    {
    case UTIL_LIST_TYPE_YESNO:
      setup.strID[0]= WIFIMATCH_DPC_SELECT_02;
      setup.strID[1]= WIFIMATCH_DPC_SELECT_03;
      setup.list_max= 2;
      break;
    case UTIL_LIST_TYPE_RETURN:
      setup.strID[0]= WIFIMATCH_DPC_SELECT_00;
      setup.list_max= 1;
      break;
    case UTIL_LIST_TYPE_UNREGISTER:
      setup.strID[0]= WIFIMATCH_DPC_SELECT_01;
      setup.strID[1]= WIFIMATCH_DPC_SELECT_00;
      setup.list_max= 2;
      break;
    }
    p_wk->p_list  = WBM_LIST_Init( &setup, HEAPID_DIGITALCARD_CHECK );
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g�j��
 *
 *	@param	DIGITALCARDCHECK_WORK *p_wk ���[�N
 */
//-----------------------------------------------------------------------------
static void Util_List_Delete( DIGITALCARDCHECK_WORK *p_wk )
{
  if( p_wk->p_list )
  { 
    WBM_LIST_Exit( p_wk->p_list );
    p_wk->p_list  = NULL;
  }
}
//----------------------------------------------------------------------------
/**
 *	@brief  ���X�g���C��
 *
 *	@param	DIGITALCARDCHECK_WORK *p_wk ���[�N
 *
 *	@return �I����������
 */
//-----------------------------------------------------------------------------
static u32 Util_List_Main( DIGITALCARDCHECK_WORK *p_wk )
{ 
  if( p_wk->p_list )
  { 
    return WBM_LIST_Main( p_wk->p_list );
  }
  else
  { 
    NAGI_Printf( "List not exist !!!\n" );
    return WBM_LIST_SELECT_NULL;
  }
}

//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�ɕ����o��
 *
 *	@param	DIGITALCARDCHECK_WORK *p_wk ���[�N
 *	@param	strID                       ����ID
 */
//-----------------------------------------------------------------------------
static void Util_Text_Print( DIGITALCARDCHECK_WORK *p_wk, u32 strID )
{ 
  WBM_TEXT_Print( p_wk->p_text, p_wk->p_msg, strID, WBM_TEXT_TYPE_STREAM );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�̕����o�͂��I��������ǂ���
 *
 *	@param	DIGITALCARDCHECK_WORK *p_wk   ���[�N
 *
 *	@return TRUE�ŕ`��I��  FALSE�ŕ`�撆
 */
//-----------------------------------------------------------------------------
static BOOL Util_Text_IsEnd( DIGITALCARDCHECK_WORK *p_wk )
{ 
  return WBM_TEXT_IsEnd( p_wk->p_text );
}
//----------------------------------------------------------------------------
/**
 *	@brief  �e�L�X�g�̕\���ݒ�
 *
 *	@param	DIGITALCARDCHECK_WORK *p_wk ���[�N
 *	@param	is_visible                  TRUE�ŕ\��FALSE�Ŕ�\��
 */
//-----------------------------------------------------------------------------
static void Util_Text_SetVisible( DIGITALCARDCHECK_WORK *p_wk, BOOL is_visible )
{ 
  GFL_BG_SetVisible( BG_FRAME_M_TEXT, is_visible );
}
